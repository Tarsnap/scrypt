bin_PROGRAMS=	scrypt test/test_scrypt
dist_man_MANS=$(scrypt_man_MANS)

scrypt_SOURCES=		main.c					\
			libcperciva/alg/sha256.c		\
			libcperciva/cpusupport/cpusupport_x86_aesni.c \
			libcperciva/cpusupport/cpusupport_x86_sse2.c \
			libcperciva/crypto/crypto_aes.c		\
			libcperciva/crypto/crypto_aesctr.c	\
			libcperciva/crypto/crypto_entropy.c	\
			libcperciva/util/asprintf.c		\
			libcperciva/util/entropy.c		\
			libcperciva/util/humansize.c		\
			libcperciva/util/insecure_memzero.c	\
			libcperciva/util/readpass.c		\
			libcperciva/util/warnp.c		\
			lib/crypto/crypto_scrypt.c		\
			lib/crypto/crypto_scrypt_smix.c		\
			lib/scryptenc/scryptenc.c		\
			lib/scryptenc/scryptenc_cpuperf.c	\
			lib/util/memlimit.c			\
			cpusupport-config.h
AM_CPPFLAGS=		-I $(srcdir)/libcperciva/alg		\
			-I $(srcdir)/libcperciva/cpusupport	\
			-I $(srcdir)/libcperciva/crypto		\
			-I $(srcdir)/libcperciva/util		\
			-I $(srcdir)/lib/crypto			\
			-I $(srcdir)/lib/scryptenc 		\
			-I $(srcdir)/lib/util			\
			-I . -D CPUSUPPORT_CONFIG_FILE=\"cpusupport-config.h\"
scrypt_LDADD=		libcperciva_aesni.a libscrypt_sse2.a
scrypt_man_MANS=	scrypt.1

cpusupport-config.h:
	( export CC="${CC}"; export CFLAGS="${CFLAGS}"; command -p sh $(srcdir)/libcperciva/cpusupport/Build/cpusupport.sh ) > cpusupport-config.h.tmp && command -p mv cpusupport-config.h.tmp cpusupport-config.h
BUILT_SOURCES=	cpusupport-config.h
CLEANFILES=	cpusupport-config.h cpusupport-config.h.tmp

noinst_LIBRARIES=	libcperciva_aesni.a
libcperciva_aesni_a_SOURCES=	libcperciva/crypto/crypto_aes_aesni.c
libcperciva_aesni_a_CFLAGS=`. ./cpusupport-config.h; echo $${CFLAGS_X86_AESNI}`

noinst_LIBRARIES+=	libscrypt_sse2.a
libscrypt_sse2_a_SOURCES=	lib/crypto/crypto_scrypt_smix_sse2.c
libscrypt_sse2_a_CFLAGS=`. ./cpusupport-config.h; echo $${CFLAGS_X86_SSE2}`

EXTRA_DIST = \
	FORMAT							\
	lib/README						\
	libcperciva/COPYRIGHT					\
	BSDmakefile						\
	README.md						\
	config_freebsd.h					\
	lib/crypto/crypto_scrypt-ref.c				\
	lib/crypto/crypto_scrypt.h				\
	lib/crypto/crypto_scrypt_smix.h				\
	lib/crypto/crypto_scrypt_smix_sse2.h			\
	lib/scryptenc/scryptenc.h				\
	lib/scryptenc/scryptenc_cpuperf.h			\
	lib/util/memlimit.h					\
	libcperciva/alg/sha256.h				\
	libcperciva/cpusupport/Build/cpusupport-X86-AESNI.c	\
	libcperciva/cpusupport/Build/cpusupport-X86-CPUID.c	\
	libcperciva/cpusupport/Build/cpusupport-X86-SSE2.c	\
	libcperciva/cpusupport/Build/cpusupport.sh		\
	libcperciva/cpusupport/cpusupport.h			\
	libcperciva/crypto/crypto_aes.h				\
	libcperciva/crypto/crypto_aes_aesni.h			\
	libcperciva/crypto/crypto_aesctr.h			\
	libcperciva/crypto/crypto_entropy.h			\
	libcperciva/util/asprintf.h				\
	libcperciva/util/entropy.h				\
	libcperciva/util/humansize.h				\
	libcperciva/util/insecure_memzero.h			\
	libcperciva/util/readpass.h				\
	libcperciva/util/sysendian.h				\
	libcperciva/util/warnp.h				\
	scrypt_platform.h					\
	test/test_scrypt.c					\
	test/test_scrypt.good					\
	test/test_scrypt.sh

test_test_scrypt_SOURCES =	test/test_scrypt.c			\
			libcperciva/alg/sha256.c			\
			libcperciva/cpusupport/cpusupport_x86_sse2.c	\
			libcperciva/util/insecure_memzero.c		\
			libcperciva/util/warnp.c			\
			lib/crypto/crypto_scrypt.c			\
			lib/crypto/crypto_scrypt_smix.c			\
			cpusupport-config.h
test_test_scrypt_LDADD=	libscrypt_sse2.a
