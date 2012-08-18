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

#include <sys/types.h>
#include <sys/resource.h>

#ifdef HAVE_SYSCTL_HW_USERMEM
#include <sys/sysctl.h>
#endif
#ifdef HAVE_SYS_SYSINFO_H
#include <sys/sysinfo.h>
#endif

#include <stddef.h>
#include <stdint.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "memlimit.h"

#ifdef HAVE_SYSCTL_HW_USERMEM
static int
memlimit_sysctl_hw_usermem(size_t * memlimit)
{
	uint8_t usermembuf[8];
	size_t usermemlen = 8;
	uint64_t usermem;

	/* Ask the kernel how much RAM we have. */
	if (sysctlbyname("hw.usermem", usermembuf, &usermemlen, NULL, 0))
		return (1);

	/*
	 * Parse as either a uint64_t or a uint32_t based on the length of
	 * output the kernel reports having copied out.  It appears that all
	 * systems providing a sysctl interface for reading integers copy
	 * them out as system-endian values, so we don't need to worry about
	 * parsing them.
	 */
	if (usermemlen == sizeof(uint64_t))
		usermem = *(uint64_t *)usermembuf;
	else if (usermemlen == sizeof(uint32_t))
		usermem = *(uint32_t *)usermembuf;
	else
		return (1);

	/* Return the sysctl value, but clamp to SIZE_MAX if necessary. */
#if UINT64_MAX > SIZE_MAX
	if (usermem > SIZE_MAX)
		*memlimit = SIZE_MAX;
	else
		*memlimit = usermem;
#else
	*memlimit = usermem;
#endif

	/* Success! */
	return (0);
}
#endif

/* If we don't HAVE_STRUCT_SYSINFO, we can't use sysinfo. */
#ifndef HAVE_STRUCT_SYSINFO
#undef HAVE_SYSINFO
#endif

#ifdef HAVE_SYSINFO
static int
memlimit_sysinfo(size_t * memlimit)
{
	struct sysinfo info;
	uint64_t totalmem;

	/* Get information from the kernel. */
	if (sysinfo(&info))
		return (1);
	totalmem = info.totalram;

	/* If we're on a modern kernel, adjust based on mem_unit. */
#ifdef HAVE_STRUCT_SYSINFO_MEM_UNIT
	totalmem = totalmem * info.mem_unit;
#endif

	/* Return the value, but clamp to SIZE_MAX if necessary. */
#if UINT64_MAX > SIZE_MAX
	if (totalmem > SIZE_MAX)
		*memlimit = SIZE_MAX;
	else
		*memlimit = totalmem;
#else
	*memlimit = totalmem;
#endif

	/* Success! */
	return (0);
}
#endif /* HAVE_SYSINFO */

static int
memlimit_rlimit(size_t * memlimit)
{
	struct rlimit rl;
	uint64_t memrlimit;

	/* Find the least of... */
	memrlimit = (uint64_t)(-1);

	/* ... RLIMIT_AS... */
	if (getrlimit(RLIMIT_AS, &rl))
		return (1);
	if ((rl.rlim_cur != RLIM_INFINITY) &&
	    ((uint64_t)rl.rlim_cur < memrlimit))
		memrlimit = rl.rlim_cur;

	/* ... RLIMIT_DATA... */
	if (getrlimit(RLIMIT_DATA, &rl))
		return (1);
	if ((rl.rlim_cur != RLIM_INFINITY) &&
	    ((uint64_t)rl.rlim_cur < memrlimit))
		memrlimit = rl.rlim_cur;

	/* ... and RLIMIT_RSS. */
#ifdef RLIMIT_RSS
	if (getrlimit(RLIMIT_RSS, &rl))
		return (1);
	if ((rl.rlim_cur != RLIM_INFINITY) &&
	    ((uint64_t)rl.rlim_cur < memrlimit))
		memrlimit = rl.rlim_cur;
#endif

	/* Return the value, but clamp to SIZE_MAX if necessary. */
#if UINT64_MAX > SIZE_MAX
	if (memrlimit > SIZE_MAX)
		*memlimit = SIZE_MAX;
	else
		*memlimit = memrlimit;
#else
	*memlimit = memrlimit;
#endif

	/* Success! */
	return (0);
}

int
memtouse(size_t maxmem, double maxmemfrac, size_t * memlimit)
{
	size_t sysctl_memlimit, sysinfo_memlimit, rlimit_memlimit;
	size_t memlimit_min;
	size_t memavail;

	/* Get memory limits. */
#ifdef HAVE_SYSCTL_HW_USERMEM
	if (memlimit_sysctl_hw_usermem(&sysctl_memlimit))
		return (1);
#else
	sysctl_memlimit = (size_t)(-1);
#endif
#ifdef HAVE_SYSINFO
	if (memlimit_sysinfo(&sysinfo_memlimit))
		return (1);
#else
	sysinfo_memlimit = (size_t)(-1);
#endif
	if (memlimit_rlimit(&rlimit_memlimit))
		return (1);

#ifdef DEBUG
	fprintf(stderr, "Memory limits are %zu %zu %zu\n",
	    sysctl_memlimit, sysinfo_memlimit, rlimit_memlimit);
#endif

	/* Find the smallest of them. */
	memlimit_min = (size_t)(-1);
	if (memlimit_min > sysctl_memlimit)
		memlimit_min = sysctl_memlimit;
	if (memlimit_min > sysinfo_memlimit)
		memlimit_min = sysinfo_memlimit;
	if (memlimit_min > rlimit_memlimit)
		memlimit_min = rlimit_memlimit;

	/* Only use the specified fraction of the available memory. */
	if ((maxmemfrac > 0.5) || (maxmemfrac == 0.0))
		maxmemfrac = 0.5;
	memavail = maxmemfrac * memlimit_min;

	/* Don't use more than the specified maximum. */
	if ((maxmem > 0) && (memavail > maxmem))
		memavail = maxmem;

	/* But always allow at least 1 MiB. */
	if (memavail < 1048576)
		memavail = 1048576;

#ifdef DEBUG
	fprintf(stderr, "Allowing up to %zu memory to be used\n", memavail);
#endif

	/* Return limit via the provided pointer. */
	*memlimit = memavail;
	return (0);
}
