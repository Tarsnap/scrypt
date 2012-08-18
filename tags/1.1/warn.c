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
 *
 * This file was originally written by Colin Percival as part of the Tarsnap
 * online backup system.
 */
#include "scrypt_platform.h"

#ifdef HAVE_ERR_H
/*
 * Everything is provided through err.h and the associated library, so we
 * don't need to do anything here.
 */
#else
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "warn.h"

const char * warn_progname = "(null)";

void
warn(const char * fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, "%s", warn_progname);
	if (fmt != NULL) {
		fprintf(stderr, ": ");
		vfprintf(stderr, fmt, ap);
	}
	fprintf(stderr, ": %s\n", strerror(errno));
	va_end(ap);
}

void
warnx(const char * fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, "%s", warn_progname);
	if (fmt != NULL) {
		fprintf(stderr, ": ");
		vfprintf(stderr, fmt, ap);
	}
	fprintf(stderr, "\n");
	va_end(ap);
}
#endif
