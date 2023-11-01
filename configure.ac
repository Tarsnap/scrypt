AC_PREREQ([2.69])
AC_INIT([scrypt],[m4_esyscmd([sh get-version.sh])],
  [https://github.com/Tarsnap/scrypt],[scrypt],[http://www.tarsnap.com/scrypt/])

# Safety check: make sure that the srcdir contains this file.
AC_CONFIG_SRCDIR([lib/scryptenc/scryptenc.c])
# Use auxiliary subscripts from this subdirectory (cleans up root).
AC_CONFIG_AUX_DIR([config.aux])
# Directory for our additional macros.
AC_CONFIG_MACRO_DIR([m4])
# Compute $target_os and related variables.
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([foreign subdir-objects])

# Checks for programs.
m4_version_prereq([2.70], [AC_PROG_CC], [AC_PROG_CC_C99])
AM_PROG_AR
LT_INIT

# Don't rebuild the configure script.  I'm distributing a perfectly good one.
AM_MAINTAINER_MODE

# Get the POSIX sh early, in case the configure script causes
# `command -p getconf` to fail in a broken environment.
CHECK_POSIX_SH

# Check if we need -std=c99 in LDFLAGS (for gcc on Solaris).
CHECK_SOLARIS_C99

# Check if we need to add extra paths to CPPFLAGS and LDFLAGS for macOS.
CHECK_DARWIN_PATHS

# Checks for AES support in OpenSSL.
AC_SEARCH_LIBS([AES_encrypt], [crypto],,
    AC_MSG_ERROR([function AES_ENCRYPT not found]))
AC_CHECK_HEADERS([openssl/aes.h],,
  [AC_MSG_ERROR([Unable to find the openssl/aes.h header])])

# Check how we can find out the available memory.
CHECK_MEMLIMIT_SUPPORT

# Check for posix_memalign.
AC_CHECK_FUNCS_ONCE([posix_memalign])

# Check for mmap so we can work around its absence on Minix.
AC_CHECK_FUNCS_ONCE([mmap])

# Use 64-bit file offsets.
AC_SYS_LARGEFILE

# Check whether the user has requested to disable compiler warnings.
CHECK_DISABLE_COMPILER_WARNINGS

# Check libcperciva POSIX compatibility.
CHECK_LIBCPERCIVA_POSIX([${srcdir}/libcperciva])

# Check whether the user wants to install libscrypt-kdf.
AC_ARG_ENABLE(libscrypt-kdf, AS_HELP_STRING([--enable-libscrypt-kdf],
    [Install libscrypt-kdf and development headers.]))
AM_CONDITIONAL([LIBSCRYPT_KDF], [test "x${enable_libscrypt_kdf}" = "xyes"])

# Write configure results and the Makefile.
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
