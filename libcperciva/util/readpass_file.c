#include <stdio.h>
#include <string.h>

#include "insecure_memzero.h"
#include "warnp.h"

#include "readpass.h"

/* Maximum file length. */
#define MAXPASSLEN 2048

/**
 * readpass_file(passwd, filename):
 * Read a passphrase from ${filename} and return it as a malloced
 * NUL-terminated string via ${passwd}.  Print an error and fail if the file
 * is 2048 characters or more, or if it contains any newline \n or \r\n
 * characters other than at the end of the file.  Do not include the \n or
 * \r\n characters in the passphrase.
 */
int
readpass_file(char ** passwd, const char * filename)
{
	FILE * f;
	char passbuf[MAXPASSLEN];

	/* Open the file. */
	if ((f = fopen(filename, "r")) == NULL) {
		warnp("fopen(%s)", filename);
		goto err1;
	}

	/* Get a line from the file. */
	if ((fgets(passbuf, MAXPASSLEN, f)) == NULL) {
		if (ferror(f)) {
			warnp("fgets(%s)", filename);
			goto err2;
		} else {
			/* We have a 0-byte password. */
			passbuf[0] = '\0';
		}
	}

	/* Bail if there's the line is too long, or if there's a second line. */
	if (fgetc(f) != EOF) {
		warn0("line too long, or more than 1 line in %s", filename);
		goto err2;
	}

	/* Close the file. */
	if (fclose(f)) {
		warnp("fclose(%s)", filename);
		goto err1;
	}

	/*
	 * Strip a trailing "\n" or a trailing "\r\n" explicitly, then reject the
	 * file if any "\r" or "\n" remains.  This matches the contract in
	 * readpass.h: only a single trailing newline ("\n") or CRLF ("\r\n") is
	 * permitted at the end of the file; any other occurrences of CR or LF are
	 * an error.
	 */
	{
		size_t len = strlen(passbuf);

		/* Strip trailing '\n', and an optional preceding '\r'. */
		if (len > 0 && passbuf[len - 1] == '\n') {
			passbuf[--len] = '\0';
			if (len > 0 && passbuf[len - 1] == '\r')
				passbuf[--len] = '\0';
		}

		/* If any CR or LF remains, the file contains embedded newlines. */
		if (strchr(passbuf, '\r') != NULL || strchr(passbuf, '\n') != NULL) {
			warn0("Invalid passphrase file: %s", filename);
			goto err1;
		}
	}

	/* Copy the password out. */
	if ((*passwd = strdup(passbuf)) == NULL) {
		warnp("Cannot allocate memory");
		goto err1;
	}

	/* Clean up. */
	insecure_memzero(passbuf, MAXPASSLEN);

	/* Success! */
	return (0);

err2:
	if (fclose(f))
		warnp("fclose");
err1:
	/* No harm in running this for all error paths. */
	insecure_memzero(passbuf, MAXPASSLEN);

	/* Failure! */
	return (-1);
}
