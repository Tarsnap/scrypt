bin_PROGRAMS=	scrypt
noinst_PROGRAMS=	tests/test_scrypt tests/valgrind/potential-memleaks
dist_man_MANS=$(scrypt_man_MANS)

scrypt_SOURCES=		main.c					\
			lib/crypto/crypto_scrypt.c		\
			lib/crypto/crypto_scrypt.h		\
			lib/crypto/crypto_scrypt_smix.c		\
			lib/crypto/crypto_scrypt_smix.h		\
			lib/crypto/crypto_scrypt_smix_sse2.h	\
			lib/scryptenc/scryptenc.c		\
			lib/scryptenc/scryptenc.h		\
			lib/scryptenc/scryptenc_cpuperf.c	\
			lib/scryptenc/scryptenc_cpuperf.h	\
			lib/util/memlimit.c			\
			lib/util/memlimit.h			\
			libcperciva/alg/sha256.c		\
			libcperciva/alg/sha256.h		\
			libcperciva/alg/sha256_shani.h		\
			libcperciva/cpusupport/cpusupport.h	\
			libcperciva/cpusupport/cpusupport_x86_aesni.c \
			libcperciva/cpusupport/cpusupport_x86_rdrand.c \
			libcperciva/cpusupport/cpusupport_x86_shani.c \
			libcperciva/cpusupport/cpusupport_x86_sse2.c \
			libcperciva/cpusupport/cpusupport_x86_ssse3.c \
			libcperciva/crypto/crypto_aes.c		\
			libcperciva/crypto/crypto_aes.h		\
			libcperciva/crypto/crypto_aes_aesni.h	\
			libcperciva/crypto/crypto_aesctr.c	\
			libcperciva/crypto/crypto_aesctr.h	\
			libcperciva/crypto/crypto_entropy.c	\
			libcperciva/crypto/crypto_entropy.h	\
			libcperciva/crypto/crypto_entropy_rdrand.h \
			libcperciva/util/asprintf.c		\
			libcperciva/util/asprintf.h		\
			libcperciva/util/entropy.c		\
			libcperciva/util/entropy.h		\
			libcperciva/util/getopt.c		\
			libcperciva/util/getopt.h		\
			libcperciva/util/humansize.c		\
			libcperciva/util/humansize.h		\
			libcperciva/util/insecure_memzero.c	\
			libcperciva/util/insecure_memzero.h	\
			libcperciva/util/parsenum.h		\
			libcperciva/util/readpass.c		\
			libcperciva/util/readpass.h		\
			libcperciva/util/sysendian.h		\
			libcperciva/util/warnp.c		\
			libcperciva/util/warnp.h		\
			scrypt_platform.h

AM_CPPFLAGS=		-I$(srcdir)/libcperciva/alg		\
			-I$(srcdir)/libcperciva/cpusupport	\
			-I$(srcdir)/libcperciva/crypto		\
			-I$(srcdir)/libcperciva/util		\
			-I$(srcdir)/lib/crypto			\
			-I$(srcdir)/lib/scryptenc 		\
			-I$(srcdir)/lib/util			\
			-DCPUSUPPORT_CONFIG_FILE=\"cpusupport-config.h\" \
			-D_POSIX_C_SOURCE=200809L

scrypt_LDADD=		libcperciva_aesni.a libcperciva_rdrand.a \
			libcperciva_shani.a libscrypt_sse2.a
scrypt_man_MANS=	scrypt.1

cpusupport-config.h:
	( export CC="${CC}"; export CFLAGS="${CFLAGS}"; command -p sh $(srcdir)/libcperciva/cpusupport/Build/cpusupport.sh "$$PATH") > cpusupport-config.h.tmp && command -p mv cpusupport-config.h.tmp cpusupport-config.h
BUILT_SOURCES=	cpusupport-config.h
CLEANFILES=	cpusupport-config.h cpusupport-config.h.tmp

# Libraries from libcperciva code.
noinst_LIBRARIES=	libcperciva_aesni.a
libcperciva_aesni_a_SOURCES=	libcperciva/crypto/crypto_aes_aesni.c
libcperciva_aesni_a_CFLAGS=`. ./cpusupport-config.h; echo $${CFLAGS_X86_AESNI}`

noinst_LIBRARIES+=	libcperciva_rdrand.a
libcperciva_rdrand_a_SOURCES=	libcperciva/crypto/crypto_entropy_rdrand.c
libcperciva_rdrand_a_CFLAGS=						\
	`. ./cpusupport-config.h; echo $${CFLAGS_X86_RDRAND}`

noinst_LIBRARIES+=	libcperciva_shani.a
libcperciva_shani_a_SOURCES=	libcperciva/alg/sha256_shani.c
libcperciva_shani_a_CFLAGS=`. ./cpusupport-config.h; echo $${CFLAGS_X86_SHANI} $${CFLAGS_X86_SSSE3}`

# Library from scrypt code.
noinst_LIBRARIES+=	libscrypt_sse2.a
libscrypt_sse2_a_SOURCES=	lib/crypto/crypto_scrypt_smix_sse2.c
libscrypt_sse2_a_CFLAGS=`. ./cpusupport-config.h; echo $${CFLAGS_X86_SSE2}`

EXTRA_DIST = \
	COPYRIGHT						\
	FORMAT							\
	lib/README						\
	README.md						\
	lib/crypto/crypto_scrypt-ref.c				\
	libcperciva/cpusupport/Build/cpusupport-X86-AESNI.c	\
	libcperciva/cpusupport/Build/cpusupport-X86-CPUID.c	\
	libcperciva/cpusupport/Build/cpusupport-X86-RDRAND.c	\
	libcperciva/cpusupport/Build/cpusupport-X86-SHANI.c	\
	libcperciva/cpusupport/Build/cpusupport-X86-SSE2.c	\
	libcperciva/cpusupport/Build/cpusupport-X86-SSSE3.c	\
	libcperciva/cpusupport/Build/cpusupport.sh		\
	tests/test_scrypt.good					\
	tests/test_scrypt_good.enc				\
	tests/shared_test_functions.sh				\
	tests/01-known-values.sh				\
	tests/02-decrypt-reference-file.sh			\
	tests/03-encrypt-decrypt-file.sh			\
	tests/04-force-resources.sh				\
	tests/05-system-scrypt-encrypt-decrypt.sh		\
	tests/test_scrypt.sh

# Binary to test the crypto_scrypt() function.
tests_test_scrypt_SOURCES =	tests/test_scrypt.c			\
			lib/crypto/crypto_scrypt.c			\
			lib/crypto/crypto_scrypt.h			\
			lib/crypto/crypto_scrypt_smix.c			\
			lib/crypto/crypto_scrypt_smix.h			\
			libcperciva/alg/sha256.c			\
			libcperciva/alg/sha256.h			\
			libcperciva/alg/sha256_shani.h			\
			libcperciva/cpusupport/cpusupport_x86_shani.c	\
			libcperciva/cpusupport/cpusupport_x86_sse2.c	\
			libcperciva/cpusupport/cpusupport_x86_ssse3.c	\
			libcperciva/util/insecure_memzero.c		\
			libcperciva/util/insecure_memzero.h		\
			libcperciva/util/warnp.c			\
			libcperciva/util/warnp.h

tests_test_scrypt_LDADD=	libcperciva_shani.a libscrypt_sse2.a

# Eliminate false positives while memory-checking for the test framework.
tests_valgrind_potential_memleaks_SOURCES = tests/valgrind/potential-memleaks.c

.PHONY: test
# we can't only build "scrypt tests/test_scrypt" because that won't build
# the BUILT_SOURCES.
test: all
	$(top_srcdir)/tests/test_scrypt.sh .
