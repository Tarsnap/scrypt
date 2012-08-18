m4_define([SCRYPT_VERS],m4_include([scrypt-version]))
AC_INIT([scrypt],SCRYPT_VERS())
AC_CONFIG_AUX_DIR([config.aux])
AM_INIT_AUTOMAKE()
AM_MAINTAINER_MODE
AC_PROG_CC

# Look for sys/endian.h and be64enc.  Old versions of NetBSD have
# sys/endian.h but don't have the (le|be)(32|64)(enc|dec) functions in it.
AC_CHECK_HEADERS([sys/endian.h])
AC_CHECK_DECLS([be64enc], [], [], [[#include <sys/endian.h>]])

# Check for err.h; if not present, we need to provide warn() and warnx().
AC_CHECK_HEADERS([err.h])

# Check for clock_gettime.  On some systems, this is provided via librt.
AC_CHECK_LIB(rt, clock_gettime)
AC_CHECK_FUNCS([clock_gettime])

# Check for a linuxy sysinfo syscall; and while we're doing that, check if
# struct sysinfo is the old version (total RAM == totalmem) or the new
# version (total RAM == totalmem * mem_unit).
AC_CHECK_HEADERS([sys/sysinfo.h])
AC_CHECK_FUNCS([sysinfo])
AC_CHECK_TYPES([struct sysinfo], [], [], [[#include <sys/sysinfo.h>]])
AC_CHECK_MEMBERS([struct sysinfo.totalram, struct sysinfo.mem_unit],
    [], [], [[#include <sys/sysinfo.h>]])

# Check if we have a hw.usermem sysctl.
CHECK_SYSCTL_HW_USERMEM

# Check if we have <sys/param.h>, since some systems require it for sysctl
# to work.
AC_CHECK_HEADERS([sys/param.h])

# Check for posix_memalign
AC_CHECK_FUNCS([posix_memalign])

# Figure out whether we want to use the SSE version of the scrypt code or not.
AC_ARG_ENABLE([sse2],
    [AS_HELP_STRING([--enable-sse2],
	[use optimized SSE2 code])],
    [],
    [enable_sse2=no])
AS_IF([test "x$enable_sse2" != xno], [SCRYPTVER=sse], [SCRYPTVER=nosse])
AC_SUBST([SCRYPTVER])

AC_SYS_LARGEFILE

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
