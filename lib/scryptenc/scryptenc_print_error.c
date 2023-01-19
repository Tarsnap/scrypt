#include <assert.h>

#include "warnp.h"

#include "scryptenc.h"
#include "scryptenc_print_error.h"

/**
 * scryptenc_print_error(rc, infilename, outfilename):
 * Print the error corresponding to ${rc}.  If relevant, use ${infilename}
 * or ${outfilename} to display an error about reading or writing; these
 * arguments can be NULL to indicate standard input or output.
 */
void
scryptenc_print_error(int rc, const char * infilename,
    const char * outfilename)
{

	/* Sanity check: this should only be used for errors. */
	assert(rc != SCRYPT_OK);

	/* Display error. */
	switch (rc) {
	case SCRYPT_ELIMIT:
		warnp("Error determining amount of available memory");
		break;
	case SCRYPT_ECLOCK:
		warnp("Error reading clocks");
		break;
	case SCRYPT_EKEY:
		warnp("Error computing derived key");
		break;
	case SCRYPT_ESALT:
		warnp("Error reading salt");
		break;
	case SCRYPT_EOPENSSL:
		warnp("OpenSSL error");
		break;
	case SCRYPT_ENOMEM:
		warnp("Error allocating memory");
		break;
	case SCRYPT_EINVAL:
		warn0("Input is not valid scrypt-encrypted block");
		break;
	case SCRYPT_EVERSION:
		warn0("Unrecognized scrypt format version");
		break;
	case SCRYPT_ETOOBIG:
		warn0("Decrypting file would require too much memory");
		break;
	case SCRYPT_ETOOSLOW:
		warn0("Decrypting file would take too much CPU time");
		break;
	case SCRYPT_EBIGSLOW:
		warn0("Decrypting file would require too much memory"
		    " and CPU time");
		break;
	case SCRYPT_EPASS:
		warn0("Passphrase is incorrect");
		break;
	case SCRYPT_EWRFILE:
		warnp("Error writing file: %s",
		    (outfilename != NULL) ? outfilename : "standard output");
		break;
	case SCRYPT_ERDFILE:
		warnp("Error reading file: %s",
		    (infilename != NULL) ? infilename : "standard input");
		break;
	case SCRYPT_EPARAM:
		warn0("Error in explicit parameters");
		break;
	}
}
