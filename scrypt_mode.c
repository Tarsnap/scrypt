#include <stdio.h>

#include "scryptenc.h"
#include "scryptenc_print_error.h"
#include "warnp.h"

#include "scrypt_mode.h"

/**
 * scrypt_mode_info(infilename):
 * Print scrypt parameters used for the specified ${infilename}, or read from
 * stdin if that argument is NULL.
 */
int
scrypt_mode_info(const char * infilename)
{
	FILE * infile;
	int rc;

	/* If the input isn't stdin, open the file. */
	if (infilename != NULL) {
		if ((infile = fopen(infilename, "rb")) == NULL) {
			warnp("Cannot open input file: %s", infilename);
			goto err0;
		}
	} else {
		infile = stdin;
	}

	/* Print the encryption parameters used for the file. */
	if ((rc = scryptdec_file_printparams(infile)) != SCRYPT_OK) {
		scryptenc_print_error(rc, infilename, NULL);
		goto err1;
	}

	/* Clean up. */
	if ((infile != stdin) && fclose(infile))
		warnp("fclose");

	/* Success! */
	return (0);

err1:
	if ((infile != stdin) && fclose(infile))
		warnp("fclose");
err0:
	/* Failure! */
	return (-1);
}


