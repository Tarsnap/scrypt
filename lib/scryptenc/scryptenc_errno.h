#ifndef SCRYPTENC_ERRNO_H_
#define SCRYPTENC_ERRNO_H_

#include <assert.h>

#include "warnp.h"

/* Return codes from scrypt(enc|dec)_(buf|file|prep). */
#define SCRYPT_OK	0	/* success */
#define SCRYPT_ELIMIT	1	/* getrlimit or sysctrl(hw.usermem) failed */
#define SCRYPT_ECLOCK	2	/* clock_getres or clock_gettime failed */
#define SCRYPT_EKEY	3	/* error computing derived key */
#define SCRYPT_ESALT	4	/* could not read salt */
#define SCRYPT_EOPENSSL	5	/* error in OpenSSL */
#define SCRYPT_ENOMEM	6	/* malloc failed */
#define SCRYPT_EINVAL	7	/* data is not a valid scrypt-encrypted block */
#define SCRYPT_EVERSION	8	/* unrecognized scrypt version number */
#define SCRYPT_ETOOBIG	9	/* decrypting would take too much memory */
#define SCRYPT_ETOOSLOW	10	/* decrypting would take too long */
#define SCRYPT_EPASS	11	/* password is incorrect */
#define SCRYPT_EWRFILE	12	/* error writing output file */
#define SCRYPT_ERDFILE	13	/* error reading input file */
#define SCRYPT_EPARAM	14	/* error in explicit parameters */
#define SCRYPT_EBIGSLOW 15	/* both SCRYPT_ETOOBIG and SCRYPT_ETOOSLOW */

/**
 * scryptenc_errno_print(rc, infilename, outfilename):
 * Print the error corresponding to ${rc}.  If relevant, use ${infilename}
 * or ${outfilename} to display an error about reading or writing; these
 * arguments can be NULL to indicate standard input or output.
 */
static inline void
scryptenc_errno_print(int rc, const char * infilename,
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

#endif /* !SCRYPTENC_ERRNO_H_ */
