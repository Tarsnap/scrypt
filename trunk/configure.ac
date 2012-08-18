m4_define([SCRYPT_VERS],m4_include([scrypt-version]))
AC_INIT([scrypt],SCRYPT_VERS())
AC_CONFIG_AUX_DIR([config.aux])
AM_INIT_AUTOMAKE()
AM_MAINTAINER_MODE
AC_PROG_CC
AC_CHECK_HEADERS([err.h sys/endian.h sys/sysinfo.h])
AC_CHECK_LIB(rt, clock_gettime)
AC_CHECK_FUNCS([clock_gettime])
AC_CHECK_FUNCS([sysinfo posix_memalign])
AC_CHECK_TYPES([struct sysinfo], [], [], [[#include <sys/sysinfo.h>]])
AC_CHECK_MEMBER([struct sysinfo.mem_unit], [], [], [[#include <sys/sysinfo.h>]])
CHECK_SYSCTL_HW_USERMEM
AC_ARG_ENABLE([sse2],
    [AS_HELP_STRING([--enable-sse2],
	[use optimized SSE2 code])],
    [],
    [enable_sse2=no])
AS_IF([test "x$enable_sse2" != xno], [SCRYPTVER=sse], [SCRYPTVER=nosse])
AC_SUBST([SCRYPTVER])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
