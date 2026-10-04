/*-
 * Copyright 2009 Colin Percival
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "platform.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "getopt.h"
#include "humansize.h"
#include "insecure_memzero.h"
#include "parsenum.h"
#include "passphrase_entry.h"
#include "scryptenc.h"
#include "scryptenc_print_error.h"
#include "warnp.h"

static void
usage(void)
{

	fprintf(stderr,
	    "usage: scrypt {enc | dec | info} [-f] [--logN value] [-M maxmem]\n"
	    "              [-m maxmemfrac] [-P] [-p value] [-r value]"
	    " [-t maxtime] [-v]\n"
	    "              [--passphrase method:arg] infile [outfile]\n"
	    "       scrypt --version\n");
	exit(1);
}

/**
 * scrypt_mode_info(infilename):
 * Print scrypt parameters used for the specified ${infilename}, or read from
 * stdin if that argument is NULL.
 */
static int
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
 * same_destructive_object(sb_in, sb_out):
 * Return non-zero if the two stat results identify storage which scrypt must
 * not read and write at the same time.  Regular files are identified by
 * device/inode; block-device aliases are identified by the underlying device.
 * Character devices such as /dev/null remain permitted.
 */
static int
same_destructive_object(const struct stat * sb_in, const struct stat * sb_out)
{

	if (S_ISREG(sb_in->st_mode) && S_ISREG(sb_out->st_mode))
		return ((sb_in->st_dev == sb_out->st_dev) &&
		    (sb_in->st_ino == sb_out->st_ino));

	if (S_ISBLK(sb_in->st_mode) && S_ISBLK(sb_out->st_mode))
		return (sb_in->st_rdev == sb_out->st_rdev);

	return (0);
}

/**
 * same_file(infile, outfilename):
 * Return non-zero if the already-open ${infile} and the path ${outfilename}
 * identify the same destructive storage object.  If ${outfilename} is NULL,
 * compare against standard output instead.  If we cannot tell -- most
 * importantly if ${outfilename} does not exist yet -- return zero.
 */
static int
same_file(FILE * infile, const char * outfilename)
{
	struct stat sb_in;
	struct stat sb_out;

	/* If we can't stat either file, assume that they're different. */
	if (fstat(fileno(infile), &sb_in))
		return (0);
	if (outfilename != NULL) {
		if (stat(outfilename, &sb_out))
			return (0);
	} else if (fstat(fileno(stdout), &sb_out)) {
		return (0);
	}

	return (same_destructive_object(&sb_in, &sb_out));
}

/**
 * open_output(infile, outfilename, outfile):
 * Open ${outfilename} for writing without truncating it until the descriptor
 * we actually opened has been proved to be different from ${infile}.  Return
 * 1 if both descriptors identify the same destructive object, -1 on error,
 * and 0
 * on success with ${outfile} set.
 */
static int
open_output(FILE * infile, const char * outfilename, FILE ** outfile)
{
	struct stat sb_in;
	struct stat sb_out;
	int fd;
	int saved_errno;

	/* Do not truncate until we have compared the opened descriptor. */
	if ((fd = open(outfilename, O_WRONLY | O_CREAT, 0666)) == -1)
		return (-1);

	/* Compare the actual objects behind both open descriptors. */
	if (fstat(fileno(infile), &sb_in))
		goto err0;
	if (fstat(fd, &sb_out))
		goto err0;
	if (same_destructive_object(&sb_in, &sb_out)) {
		(void)close(fd);
		return (1);
	}

	/* Match fopen(..., "wb") truncation for regular output files. */
	if (S_ISREG(sb_out.st_mode) && ftruncate(fd, 0))
		goto err0;

	/* fdopen does not truncate an already-open descriptor. */
	if ((*outfile = fdopen(fd, "wb")) == NULL)
		goto err0;

	/* Success! */
	return (0);

err0:
	saved_errno = errno;
	(void)close(fd);
	errno = saved_errno;
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
static int
scrypt_mode_enc_dec(struct scryptenc_params params,
    enum passphrase_entry passphrase_entry, const char * passphrase_arg,
    int dec, int verbose, int force_resources,
    const char * infilename, const char * outfilename)
{
	struct scryptdec_file_cookie * C = NULL;
	FILE * infile;
	FILE * outfile = stdout;
	char * passwd;
	int openrc;
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

	/*
	 * Refuse obvious same-file aliases before prompting.  open_output()
	 * repeats this check on the descriptor it actually opens, closing the
	 * path race before any regular output file is truncated.
	 */
	if (same_file(infile, outfilename)) {
		warn0("Input and output files are the same: %s",
		    outfilename != NULL ? outfilename : "standard output");
		goto err1;
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

	/* Bind the destructive same-file check to the output we actually opened. */
	if (outfilename != NULL) {
		if ((openrc = open_output(infile, outfilename, &outfile)) != 0) {
			if (openrc > 0)
				warn0("Input and output files are the same: %s",
				    outfilename);
			else
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
	if ((infile != stdin) && fclose(infile))
		warnp("fclose");
err0:
	/* Failure! */
	return (-1);
}

/* Parse a numeric optarg within a GETOPT context.  (Requires ch and optarg.) */
#define GETOPT_PARSENUM_WITHIN_UNSIGNED(var, min, max) do {		\
	if (PARSENUM((var), optarg, (min), (max))) {			\
		if (errno == ERANGE) {					\
			warn0("%s must be between %ju and %ju"		\
			    " (inclusive)", ch, (uintmax_t)(min),	\
			    (uintmax_t)(max));				\
		} else							\
			warnp("Invalid option: %s %s", ch, optarg);	\
		exit(1);						\
	}								\
} while (0)

int
main(int argc, char * argv[])
{
	int dec = 0;
	int info = 0;
	int force_resources = 0;
	uint64_t maxmem64;
	struct scryptenc_params params = {0, 0.5, 300.0, 0, 0, 0};
	const char * ch;
	const char * infilename;
	const char * outfilename;
	int verbose = 0;
	enum passphrase_entry passphrase_entry = PASSPHRASE_UNSET;
	const char * passphrase_arg;

	WARNP_INIT;

	/* We should have "enc", "dec", or "info" first. */
	if (argc < 2)
		usage();
	if (strcmp(argv[1], "enc") == 0) {
		params.maxmem = 0;
		params.maxmemfrac = 0.125;
		params.maxtime = 5.0;
	} else if (strcmp(argv[1], "dec") == 0) {
		dec = 1;
	} else if (strcmp(argv[1], "info") == 0) {
		info = 1;
	} else if (strcmp(argv[1], "--version") == 0) {
		fprintf(stdout, "scrypt %s\n", PACKAGE_VERSION);
		exit(0);
	} else {
		warn0("First argument must be 'enc', 'dec', or 'info'");
		usage();
	}
	argc--;
	argv++;

	/* Parse arguments. */
	while ((ch = GETOPT(argc, argv)) != NULL) {
		GETOPT_SWITCH(ch) {
		GETOPT_OPT("-f"):
			force_resources = 1;
			break;
		GETOPT_OPTARG("--logN"):
			GETOPT_PARSENUM_WITHIN_UNSIGNED(&params.logN, 10, 40);
			break;
		GETOPT_OPTARG("-M"):
			if (humansize_parse(optarg, &maxmem64)) {
				warn0("Could not parse the parameter to -M");
				exit(1);
			}
			if (maxmem64 > SIZE_MAX) {
				warn0("The parameter to -M is too large");
				exit(1);
			}
			params.maxmem = (size_t)maxmem64;
			break;
		GETOPT_OPTARG("-m"):
			if (PARSENUM(&params.maxmemfrac, optarg, 0, 0.5)) {
				warnp("Invalid option: -m %s", optarg);
				exit(1);
			}
			break;
		GETOPT_OPTARG("-p"):
			GETOPT_PARSENUM_WITHIN_UNSIGNED(&params.p, 1, 2048);
			break;
		GETOPT_OPTARG("--passphrase"):
			if (passphrase_entry != PASSPHRASE_UNSET) {
				warn0("You can only enter one --passphrase or"
				    " -P argument");
				exit(1);
			}

			/* Parse "method:arg" optarg. */
			if (passphrase_entry_parse(optarg, &passphrase_entry,
			    &passphrase_arg))
				exit(1);
			break;
		GETOPT_OPTARG("-r"):
			GETOPT_PARSENUM_WITHIN_UNSIGNED(&params.r, 1, 32);
			break;
		GETOPT_OPTARG("-t"):
			if (PARSENUM(&params.maxtime, optarg, 0, INFINITY)) {
				warnp("Invalid option: -t %s", optarg);
				exit(1);
			}
			break;
		GETOPT_OPT("-v"):
			verbose = 1;
			break;
		GETOPT_OPT("-P"):
			if (passphrase_entry != PASSPHRASE_UNSET) {
				warn0("You can only enter one --passphrase or"
				    " -P argument");
				exit(1);
			}
			passphrase_entry = PASSPHRASE_STDIN_ONCE;
			passphrase_arg = "";
			break;
		GETOPT_MISSING_ARG:
			warn0("Missing argument to %s", ch);
			usage();
		GETOPT_DEFAULT:
			warn0("illegal option -- %s", ch);
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	/* We must have one or two parameters left. */
	if ((argc < 1) || (argc > 2))
		usage();

	/* The explicit parameters must be zero, or all non-zero. */
	if ((params.logN != 0) && ((params.r == 0) || (params.p == 0))) {
		warn0("If --logN is set, -r and -p must also be set");
		goto err0;
	}
	if ((params.r != 0) && ((params.logN == 0) || (params.p == 0))) {
		warn0("If -r is set, --logN and -p must also be set");
		goto err0;
	}
	if ((params.p != 0) && ((params.logN == 0) || (params.r == 0))) {
		warn0("If -p is set, --logN and -r must also be set");
		goto err0;
	}

	/* We can't have a maxmemfrac of 0. */
	if (params.maxmemfrac == 0.0) {
		warn0("-m must be greater than 0");
		goto err0;
	}

	/* Set the input filename. */
	if (strcmp(argv[0], "-"))
		infilename = argv[0];
	else
		infilename = NULL;

	/* Set the output filename. */
	if (argc > 1)
		outfilename = argv[1];
	else
		outfilename = NULL;

	/* Set the default passphrase entry method. */
	if (passphrase_entry == PASSPHRASE_UNSET) {
		passphrase_entry = PASSPHRASE_TTY_STDIN;
		passphrase_arg = "";
	}

	/* Sanity check passphrase entry method and input filename. */
	if ((passphrase_entry == PASSPHRASE_STDIN_ONCE) &&
	    (infilename == NULL)) {
		warn0("Cannot read both passphrase and input file"
		    " from standard input");
		goto err0;
	}

	/* What type of operation are we doing? */
	if (info) {
		/* User selected 'info' mode. */
		if (scrypt_mode_info(infilename))
			goto err0;
	} else {
		/* User selected encryption or decryption. */
		if (scrypt_mode_enc_dec(params, passphrase_entry,
		    passphrase_arg, dec, verbose, force_resources,
		    infilename, outfilename))
			goto err0;
	}

	/* Success! */
	exit(0);

err0:
	/* Failure! */
	exit(1);
}
