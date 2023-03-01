#ifndef SCRYPT_MODE_H_
#define SCRYPT_MODE_H_

#include "passphrase_entry.h"

/* Forward declaration. */
struct scryptenc_params;

/**
 * scrypt_mode_info(infilename):
 * Print scrypt parameters used for the specified ${infilename}, or read from
 * stdin if that argument is NULL.
 */
int scrypt_mode_info(const char *);

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
int scrypt_mode_enc_dec(struct scryptenc_params, enum passphrase_entry,
    const char *, int, int, int, const char *, const char *);

#endif /* !SCRYPT_MODE_H_ */
