#ifndef SCRYPTENC_PRINT_ERROR_H_
#define SCRYPTENC_PRINT_ERROR_H_

/**
 * scryptenc_print_error(rc, infilename, outfilename):
 * Print the error corresponding to ${rc}.  If relevant, use ${infilename}
 * or ${outfilename} to display an error about reading or writing; these
 * arguments can be NULL to indicate standard input or output.
 */
void scryptenc_print_error(int, const char *, const char *);

#endif /* !SCRYPTENC_PRINT_ERROR_H_ */
