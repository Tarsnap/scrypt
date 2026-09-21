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
	size_t len;

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
	 * Strip a trailing "\n" or "\r\n".  fgets() stops after a "\n", so
	 * if the file contains one then it is the last character we read.
	 */
	len = strlen(passbuf);
	if ((len > 0) && (passbuf[len - 1] == '\n')) {
		passbuf[--len] = '\0';
		if ((len > 0) && (passbuf[len - 1] == '\r'))
			passbuf[--len] = '\0';
	}

	/*
	 * Any "\r" or "\n" left is not a line ending we strip.  We could cut
	 * the passphrase short there, or keep the character as part of the
	 * passphrase, but both change the passphrase which an existing file
	 * yields, and neither is safe to pick silently.  Refuse the file.
	 */
	if (strcspn(passbuf, "\r\n") != len) {
		warn0("passphrase in %s contains a carriage return or newline",
		    filename);
		goto err1;
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
