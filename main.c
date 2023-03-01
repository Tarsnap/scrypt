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

#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "getopt.h"
#include "humansize.h"
#include "parsenum.h"
#include "passphrase_entry.h"
#include "scrypt_mode.h"
#include "scryptenc.h"
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
main(int argc, char *argv[])
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
		warn0("First argument must be 'enc', 'dec', or 'info'.");
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
				warn0("Could not parse the parameter to -M.");
				exit(1);
			}
			if (maxmem64 > SIZE_MAX) {
				warn0("The parameter to -M is too large.");
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
			GETOPT_PARSENUM_WITHIN_UNSIGNED(&params.p, 1, 32);
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
