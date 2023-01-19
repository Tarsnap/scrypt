bin_PROGRAMS=	scrypt
noinst_PROGRAMS=	tests/verify-strings/test_scrypt		\
	tests/valgrind/potential-memleaks
dist_man_MANS=$(scrypt_man_MANS)
ACLOCAL_AMFLAGS=	-I m4

# These files fulfill the crypto_scrypt() function, with the exception of
#     libcperciva/alg/sha256_shani.c
#     lib/crypto/crypto_scrypt_smix_sse2.c
# which require special compiler flags and are thus compiled as separate
# libraries.  See test_scrypt for an example.
#
# crypto_scrypt() does not require aesni or rdrand, but we need to include
# the relevant cpusupport_x86 files so that cpusupport can link to them.
crypto_scrypt_files=	lib/crypto/crypto_scrypt_smix.c			\
			lib/crypto/crypto_scrypt_smix.h			\
			lib/crypto/crypto_scrypt_smix_sse2.h		\
			lib-platform/platform.h				\
			lib-platform/crypto/crypto_scrypt.c		\
			lib-platform/crypto/crypto_scrypt.h		\
			libcperciva/alg/sha256.c			\
			libcperciva/alg/sha256.h			\
			libcperciva/alg/sha256_arm.h			\
			libcperciva/alg/sha256_shani.h			\
			libcperciva/alg/sha256_sse2.h			\
			libcperciva/cpusupport/cpusupport.h		\
			libcperciva/cpusupport/cpusupport_arm_aes.c	\
			libcperciva/cpusupport/cpusupport_arm_sha256.c	\
			libcperciva/cpusupport/cpusupport_x86_aesni.c	\
			libcperciva/cpusupport/cpusupport_x86_rdrand.c	\
			libcperciva/cpusupport/cpusupport_x86_shani.c	\
			libcperciva/cpusupport/cpusupport_x86_sse2.c	\
			libcperciva/cpusupport/cpusupport_x86_ssse3.c	\
			libcperciva/util/insecure_memzero.c		\
			libcperciva/util/insecure_memzero.h		\
			libcperciva/util/warnp.c			\
			libcperciva/util/warnp.h

# Don't include crypto_aesctr_shared.c in this list, as it should be included
# directly into other translation unit(s), and not compiled on its own.
scrypt_SOURCES=		main.c					\
			$(crypto_scrypt_files)			\
			lib/scryptenc/scryptenc.c		\
			lib/scryptenc/scryptenc.h		\
			lib/scryptenc/scryptenc_cpuperf.c	\
			lib/scryptenc/scryptenc_cpuperf.h	\
			lib/scryptenc/scryptenc_print_error.c	\
			lib/scryptenc/scryptenc_print_error.h	\
			lib/util/passphrase_entry.c		\
			lib/util/passphrase_entry.h		\
			lib-platform/util/memlimit.h		\
			libcperciva/crypto/crypto_aes.c		\
			libcperciva/crypto/crypto_aes.h		\
			libcperciva/crypto/crypto_aes_aesni.h	\
			libcperciva/crypto/crypto_aes_aesni_m128i.h \
			libcperciva/crypto/crypto_aes_arm.h	\
			libcperciva/crypto/crypto_aes_arm_u8.h	\
			libcperciva/crypto/crypto_aesctr.c	\
			libcperciva/crypto/crypto_aesctr.h	\
			libcperciva/crypto/crypto_aesctr_aesni.h \
			libcperciva/crypto/crypto_aesctr_arm.h	\
			libcperciva/crypto/crypto_entropy.c	\
			libcperciva/crypto/crypto_entropy.h	\
			libcperciva/crypto/crypto_entropy_rdrand.h \
			libcperciva/crypto/crypto_verify_bytes.c \
			libcperciva/crypto/crypto_verify_bytes.h \
			libcperciva/util/align_ptr.h		\
			libcperciva/util/asprintf.c		\
			libcperciva/util/asprintf.h		\
			libcperciva/util/entropy.c		\
			libcperciva/util/entropy.h		\
			libcperciva/util/getopt.c		\
			libcperciva/util/getopt.h		\
			libcperciva/util/humansize.c		\
			libcperciva/util/humansize.h		\
			libcperciva/util/monoclock.c		\
			libcperciva/util/monoclock.h		\
			libcperciva/util/parsenum.h		\
			libcperciva/util/readpass.c		\
			libcperciva/util/readpass.h		\
			libcperciva/util/readpass_file.c	\
			libcperciva/util/sysendian.h

AM_CPPFLAGS=		-I$(srcdir)/lib				\
			-I$(srcdir)/lib/crypto			\
			-I$(srcdir)/lib/scryptenc		\
			-I$(srcdir)/lib/util			\
			-I$(srcdir)/lib-platform		\
			-I$(srcdir)/lib-platform/crypto		\
			-I$(srcdir)/lib-platform/util		\
			-I$(srcdir)/libcperciva/alg		\
			-I$(srcdir)/libcperciva/cpusupport	\
			-I$(srcdir)/libcperciva/crypto		\
			-I$(srcdir)/libcperciva/util		\
			-DCPUSUPPORT_CONFIG_FILE=\"cpusupport-config.h\" \
			-D_POSIX_C_SOURCE=200809L		\
			-D_XOPEN_SOURCE=700			\
			${CFLAGS_POSIX}

scrypt_LDADD=		libcperciva_aesni.la libcperciva_rdrand.la \
			libcperciva_shani.la libscrypt_sse2.la	\
			libscrypt_memlimit.la			\
			libcperciva_arm_sha256.la		\
			libcperciva_arm_aes.la			\
			${LDADD_POSIX}
scrypt_man_MANS=	scrypt.1

cpusupport-config.h:
	( export CC="${CC}"; export CFLAGS="${CFLAGS}"; command -p sh $(srcdir)/libcperciva/cpusupport/Build/cpusupport.sh "$$PATH") > cpusupport-config.h.tmp && command -p mv cpusupport-config.h.tmp cpusupport-config.h
BUILT_SOURCES=	cpusupport-config.h
CLEANFILES=	cpusupport-config.h cpusupport-config.h.tmp

# Libraries from libcperciva code.
noinst_LTLIBRARIES=	libcperciva_aesni.la
libcperciva_aesni_la_SOURCES=	libcperciva/crypto/crypto_aes_aesni.c \
			libcperciva/crypto/crypto_aesctr_aesni.c
nodist_libcperciva_aesni_la_SOURCES=	cpusupport-config.h
libcperciva_aesni_la_CFLAGS=`. ./cpusupport-config.h; echo $${CFLAGS_X86_AESNI}`

noinst_LTLIBRARIES+=	libcperciva_arm_aes.la
libcperciva_arm_aes_la_SOURCES=	libcperciva/crypto/crypto_aes_arm.c \
			libcperciva/crypto/crypto_aesctr_arm.c
nodist_libcperciva_arm_aes_la_SOURCES=	cpusupport-config.h
libcperciva_arm_aes_la_CFLAGS=`. ./cpusupport-config.h; echo $${CFLAGS_ARM_AES}`

noinst_LTLIBRARIES+=	libcperciva_rdrand.la
libcperciva_rdrand_la_SOURCES=	libcperciva/crypto/crypto_entropy_rdrand.c
nodist_libcperciva_rdrand_la_SOURCES=	cpusupport-config.h
libcperciva_rdrand_la_CFLAGS=						\
	`. ./cpusupport-config.h; echo $${CFLAGS_X86_RDRAND}`

noinst_LTLIBRARIES+=	libcperciva_shani.la
libcperciva_shani_la_SOURCES=	libcperciva/alg/sha256_shani.c
nodist_libcperciva_shani_la_SOURCES=	cpusupport-config.h
libcperciva_shani_la_CFLAGS=`. ./cpusupport-config.h; echo $${CFLAGS_X86_SHANI} $${CFLAGS_X86_SSSE3}`

noinst_LTLIBRARIES+=	libcperciva_arm_sha256.la
libcperciva_arm_sha256_la_SOURCES=	libcperciva/alg/sha256_arm.c
nodist_libcperciva_arm_sha256_la_SOURCES=	cpusupport-config.h
libcperciva_arm_sha256_la_CFLAGS=`. ./cpusupport-config.h; echo $${CFLAGS_ARM_SHA256}`

# Library from libcperciva and scrypt code.
noinst_LTLIBRARIES+=	libscrypt_sse2.la
libscrypt_sse2_la_SOURCES=	libcperciva/alg/sha256_sse2.c	\
				lib/crypto/crypto_scrypt_smix_sse2.c
nodist_libscrypt_sse2_la_SOURCES=	cpusupport-config.h
libscrypt_sse2_la_CFLAGS=`. ./cpusupport-config.h; echo $${CFLAGS_X86_SSE2}`

# This library uses non-POSIX functionality, so we need to cancel the
# _POSIX_C_SOURCE and _XOPEN_SOURCE defined in AM_CPPFLAGS.
noinst_LTLIBRARIES+=	libscrypt_memlimit.la
libscrypt_memlimit_la_SOURCES=	lib-platform/util/memlimit.c	\
				lib-platform/util/memlimit.h
libscrypt_memlimit_la_CFLAGS=-U_POSIX_C_SOURCE -U_XOPEN_SOURCE

# Install libscrypt-kdf?
if LIBSCRYPT_KDF
lib_LTLIBRARIES=		libscrypt-kdf.la
libscrypt_kdf_la_LDFLAGS=	-version-info 1			\
	-export-symbols-regex 'crypto_scrypt$$'
include_HEADERS=		libscrypt-kdf/scrypt-kdf.h
noinst_PROGRAMS+=		tests/libscrypt-kdf/sample-libscrypt-kdf
else
# Allow the user to get a usable library even if they didn't run configure
# with --enable-libscrypt-kdf.  If we didn't include this, they would get
# empty libraries if they ran `make libscrypt-kdf.la`.
EXTRA_LTLIBRARIES=		libscrypt-kdf.la
endif

# Shared definitions for libscrypt-kdf.
libscrypt_kdf_la_SOURCES=	$(crypto_scrypt_files)
libscrypt_kdf_la_LIBADD=	libcperciva_shani.la libscrypt_sse2.la	\
				libcperciva_arm_sha256.la
# Workaround for "created with both libtool and without".
libscrypt_kdf_la_CFLAGS=	$(AM_CFLAGS)

# Test libscrypt-kdf compile.
tests_libscrypt_kdf_sample_libscrypt_kdf_SOURCES=	\
	tests/libscrypt-kdf/sample-libscrypt-kdf.c
tests_libscrypt_kdf_sample_libscrypt_kdf_CPPFLAGS=	\
	-I$(srcdir)/libscrypt-kdf/
tests_libscrypt_kdf_sample_libscrypt_kdf_LDADD=		libscrypt-kdf.la

# crypto_aesctr_shared.c is in this list because it can't be included in the
# _SOURCES because it should only be included as part of another translation
# unit.
EXTRA_DIST=							\
	.autom4te.cfg						\
	BUILDING						\
	COPYRIGHT						\
	FORMAT							\
	README.md						\
	STYLE							\
	get-version.sh						\
	lib/README						\
	lib/crypto/crypto_scrypt-ref.c				\
	libcperciva/POSIX/README				\
	libcperciva/POSIX/posix-abstract-declarator.c		\
	libcperciva/POSIX/posix-cflags.sh			\
	libcperciva/POSIX/posix-clock_gettime.c			\
	libcperciva/POSIX/posix-clock_realtime.c		\
	libcperciva/POSIX/posix-inet-addrstrlen.c		\
	libcperciva/POSIX/posix-inet6-addrstrlen.c		\
	libcperciva/POSIX/posix-l.sh				\
	libcperciva/POSIX/posix-msg_nosignal.c			\
	libcperciva/POSIX/posix-restrict.c			\
	libcperciva/POSIX/posix-stat-st_mtim.c			\
	libcperciva/POSIX/posix-trivial.c			\
	libcperciva/cpusupport/Build/cpusupport-ARM-AES.c	\
	libcperciva/cpusupport/Build/cpusupport-ARM-SHA256.c	\
	libcperciva/cpusupport/Build/cpusupport-HWCAP-ELF_AUX_INFO.c \
	libcperciva/cpusupport/Build/cpusupport-HWCAP-GETAUXVAL.c \
	libcperciva/cpusupport/Build/cpusupport-X86-AESNI.c	\
	libcperciva/cpusupport/Build/cpusupport-X86-CPUID.c	\
	libcperciva/cpusupport/Build/cpusupport-X86-CPUID_COUNT.c \
	libcperciva/cpusupport/Build/cpusupport-X86-RDRAND.c	\
	libcperciva/cpusupport/Build/cpusupport-X86-SHANI.c	\
	libcperciva/cpusupport/Build/cpusupport-X86-SSE2.c	\
	libcperciva/cpusupport/Build/cpusupport-X86-SSSE3.c	\
	libcperciva/cpusupport/Build/cpusupport.sh		\
	libcperciva/crypto/crypto_aesctr_shared.c		\
	tests/01-known-values.sh				\
	tests/02-decrypt-reference-file.sh			\
	tests/03-encrypt-decrypt-file.sh			\
	tests/04-force-resources.sh				\
	tests/05-system-scrypt-encrypt-decrypt.sh		\
	tests/06-decrypt-fail.sh				\
	tests/07-passphrase-env.sh				\
	tests/08-passphrase-file.sh				\
	tests/09-explicit-params.sh				\
	tests/shared_test_functions.sh				\
	tests/shared_valgrind_functions.sh			\
	tests/test_scrypt.sh					\
	tests/verify-strings/test_scrypt.good			\
	tests/verify-strings/test_scrypt_good.enc		\
	tests/verify-strings/test_scrypt_small.good

# Binary to test the crypto_scrypt() function.
tests_verify_strings_test_scrypt_SOURCES=			\
	tests/verify-strings/test_scrypt.c			\
	$(crypto_scrypt_files)

tests_verify_strings_test_scrypt_LDADD=	libcperciva_shani.la	\
	libscrypt_sse2.la					\
	libcperciva_arm_sha256.la				\
	${LDADD_POSIX}

# Eliminate false positives while memory-checking for the test framework.
tests_valgrind_potential_memleaks_SOURCES= tests/valgrind/potential-memleaks.c

.PHONY: test
# we can't only build "scrypt tests/verify-strings/test_scrypt" because that
# won't build the BUILT_SOURCES.
test: all
	$(top_srcdir)/tests/test_scrypt.sh .
