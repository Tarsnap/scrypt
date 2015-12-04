AC_PREREQ([2.69])
m4_define([SCRYPT_VERS],m4_include([scrypt-version]))
AC_INIT([scrypt],[SCRYPT_VERS()],[https://github.com/Tarsnap/scrypt],[scrypt],[http://www.tarsnap.com/scrypt/])
AC_CONFIG_SRCDIR([lib/scryptenc/scryptenc.c])
AC_CONFIG_AUX_DIR([config.aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])

# add test_scrypt
AM_EXTRA_RECURSIVE_TARGETS([test])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AR

# Don't rebuild the configure script.  I'm distributing a perfectly good one.
AM_MAINTAINER_MODE

# Checks for libraries.

# Check for clock_gettime.  On some systems, this is provided via librt.
AC_SEARCH_LIBS([clock_gettime], [rt])
AC_CHECK_FUNCS([clock_gettime])

AC_SEARCH_LIBS([AES_encrypt], [crypto],,
    AC_MSG_ERROR([function AES_ENCRYPT not found]))

# Check for a linuxy sysinfo syscall; and while we're doing that, check if
# struct sysinfo is the old version (total RAM == totalmem) or the new
# version (total RAM == totalmem * mem_unit).
AC_CHECK_HEADERS([sys/sysinfo.h])
AC_CHECK_FUNCS([sysinfo])
AC_CHECK_TYPES([struct sysinfo], [], [], [[#include <sys/sysinfo.h>]])
AC_CHECK_MEMBERS([struct sysinfo.totalram, struct sysinfo.mem_unit], [], [],
    [[#include <sys/sysinfo.h>]])

# Check if we have <sys/param.h>, since some systems require it for sysctl
# to work.
AC_CHECK_HEADERS([sys/param.h])

# Check if we have <openssl/aes.h>, to fix the make failing when configure passes.
AC_CHECK_HEADERS([openssl/aes.h],
  [scrypt_found_aes_headers=yes; break;])

AS_IF([test "x$scrypt_found_aes_headers" != "xyes"],
  [AC_MSG_ERROR([Unable to find the openssl/aes.h headers])])

# Check for <sys/sysctl.h>.  If it exists and it defines HW_USERMEM
# and/or HW_MEMSIZE, we'll try using those as memory limits.
AC_CHECK_HEADERS([sys/sysctl.h])

# Check for posix_memalign
AC_CHECK_FUNCS([posix_memalign])

# Check for mmap so we can work around its absence on Minix
AC_CHECK_FUNCS([mmap])

AC_SYS_LARGEFILE

# AX_CFLAGS_WARN_ALL

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
