#ifndef SCRYPTENC_ERRNO_H_
#define SCRYPTENC_ERRNO_H_

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

#endif /* !SCRYPTENC_ERRNO_H_ */
