#ifndef SCRYPT_MODE_H_
#define SCRYPT_MODE_H_

/**
 * scrypt_mode_info(infilename):
 * Print scrypt parameters used for the specified ${infilename}, or read from
 * stdin if that argument is NULL.
 */
int scrypt_mode_info(const char *);

#endif /* !SCRYPT_MODE_H_ */
