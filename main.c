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
#include "scrypt_platform.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "readpass.h"
#include "scryptenc.h"
#include "warn.h"

static void
usage(void)
{

	fprintf(stderr,
	    "usage: scrypt {enc | dec} [...] infile [outfile]\n");
	exit(1);
}

int
main(int argc, char *argv[])
{
	FILE * infile = NULL;
	FILE * outfile = stdout;
	int dec = 0;
	size_t maxmem = 0;
	double maxmemfrac = 0.5;
	double maxtime = 300.0;
	char ch;
	char * passwd;
	int rc;

#ifdef NEED_WARN_PROGNAME
	warn_progname = "scrypt";
#endif

	/* We should have "enc" or "dec" first. */
	if (argc < 2)
		usage();
	if (strcmp(argv[1], "enc") == 0) {
		maxmem = 0;
		maxmemfrac = 0.125;
		maxtime = 5.0;
	} else if (strcmp(argv[1], "dec") == 0) {
		dec = 1;
	} else
		usage();
	argc--;
	argv++;

	/* Parse arguments. */
	while ((ch = getopt(argc, argv, "hm:M:t:")) != -1) {
		switch (ch) {
		case 'M':
			maxmem = strtoumax(optarg, NULL, 0);
			break;
		case 'm':
			maxmemfrac = strtod(optarg, NULL);
			break;
		case 't':
			maxtime = strtod(optarg, NULL);
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	/* We must have one or two parameters left. */
	if ((argc < 1) || (argc > 2))
		usage();

	/* Open the input file. */
	if ((infile = fopen(argv[0], "r")) == NULL) {
		warn("Cannot open input file: %s", argv[0]);
		exit(1);
	}

	/* If we have an output file, open it. */
	if (argc > 1) {
		if ((outfile = fopen(argv[1], "w")) == NULL) {
			warn("Cannot open output file: %s", argv[1]);
			exit(1);
		}
	}

	/* Prompt for a password. */
	if (tarsnap_readpass(&passwd, "Please enter passphrase",
	    dec ? NULL : "Please confirm passphrase", 1))
		exit(1);

	/* Encrypt or decrypt. */
	if (dec)
		rc = scryptdec_file(infile, outfile, (uint8_t *)passwd,
		    strlen(passwd), maxmem, maxmemfrac, maxtime);
	else
		rc = scryptenc_file(infile, outfile, (uint8_t *)passwd,
		    strlen(passwd), maxmem, maxmemfrac, maxtime);

	/* Zero and free the password. */
	memset(passwd, 0, strlen(passwd));
	free(passwd);

	/* If we failed, print the right error message and exit. */
	if (rc != 0) {
		switch (rc) {
		case 1:
			warn("Error determining amount of available memory");
			break;
		case 2:
			warn("Error reading clocks");
			break;
		case 3:
			warn("Error computing derived key");
			break;
		case 4:
			warn("Error reading salt");
			break;
		case 5:
			warn("OpenSSL error");
			break;
		case 6:
			warn("Error allocating memory");
			break;
		case 7:
			warnx("Input is not valid scrypt-encrypted block");
			break;
		case 8:
			warnx("Unrecognized scrypt format version");
			break;
		case 9:
			warnx("Decrypting file would require too much memory");
			break;
		case 10:
			warnx("Decrypting file would take too much CPU time");
			break;
		case 11:
			warnx("Passphrase is incorrect");
			break;
		case 12:
			warn("Error writing file: %s",
			    (argc > 1) ? argv[1] : "standard output");
			break;
		case 13:
			warn("Error reading file: %s", argv[0]);
			break;
		}
		exit(1);
	}

	return (0);
}
