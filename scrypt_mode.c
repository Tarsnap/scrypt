#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "insecure_memzero.h"
#include "passphrase_entry.h"
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

/**
 * scrypt_mode_enc_dec(params, passphrase_entry, passphrase_arg, dec, verbose,
 *     force_resources, infilename, outfilename):
 * Either encrypt (if ${dec} is 0) or decrypt (if ${dec} is non-zero)
 * ${infilename} (or standard input if this is NULL) to ${outfilename}.
 * Use scrypt parameters ${params}, with passphrase entry method
 * ${passphrase_entry} and argument ${passphrase_arg}.  If ${verbose} is
 * non-zero, print verbose messages.  If ${force_resources} is non-zero,
 * do not check whether encryption or decryption will exceed the estimated
 * time or memory requirements.
 */
int
scrypt_mode_enc_dec(struct scryptenc_params params,
    enum passphrase_entry passphrase_entry, const char * passphrase_arg,
    int dec, int verbose, int force_resources,
    const char * infilename, const char * outfilename)
{
	struct scryptdec_file_cookie * C = NULL;
	FILE * infile;
	FILE * outfile = stdout;
	char * passwd;
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

	/* Get the password. */
	if (passphrase_entry_readpass(&passwd, passphrase_entry,
	    passphrase_arg, "Please enter passphrase",
	    "Please confirm passphrase", dec)) {
		warnp("passphrase_entry_readpass");
		goto err1;
	}

	/*-
	 * If we're decrypting, open the input file and process its header;
	 * doing this here allows us to abort without creating an output
	 * file if the input file does not have a valid scrypt header or if
	 * we have the wrong passphrase.
	 *
	 * If successful, we get back a cookie containing the decryption
	 * parameters (which we'll use after we open the output file).
	 */
	if (dec) {
		if ((rc = scryptdec_file_prep(infile, (uint8_t *)passwd,
		    strlen(passwd), &params, verbose, force_resources,
		    &C)) != 0) {
			goto cleanup;
		}
	}

	/* If we have an output filename, open it. */
	if (outfilename != NULL) {
		if ((outfile = fopen(outfilename, "wb")) == NULL) {
			warnp("Cannot open output file: %s", outfilename);
			goto err2;
		}
	}

	/* Encrypt or decrypt. */
	if (dec)
		rc = scryptdec_file_copy(C, outfile);
	else
		rc = scryptenc_file(infile, outfile, (uint8_t *)passwd,
		    strlen(passwd), &params, verbose, force_resources);

cleanup:
	/* Free the decryption cookie, if any. */
	scryptdec_file_cookie_free(C);

	/* Zero and free the password. */
	insecure_memzero(passwd, strlen(passwd));
	free(passwd);

	/* Close any files we opened. */
	if ((infile != stdin) && fclose(infile))
		warnp("fclose");
	if ((outfile != stdout) && fclose(outfile))
		warnp("fclose");

	/* If we failed, print the right error message and exit. */
	if (rc != SCRYPT_OK) {
		scryptenc_print_error(rc, infilename, outfilename);
		goto err0;
	}

	/* Success! */
	return (0);

err2:
	scryptdec_file_cookie_free(C);
	insecure_memzero(passwd, strlen(passwd));
	free(passwd);
err1:
	if (infile != stdin)
		fclose(infile);
err0:
	/* Failure! */
	return (-1);
}


