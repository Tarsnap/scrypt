bin_PROGRAMS=	test_scrypt

test_scrypt_SOURCES=  test_scrypt.c 	\
			$(top_srcdir)/libcperciva/alg/sha256.c		\
			$(top_srcdir)/libcperciva/cpusupport/cpusupport_x86_aesni.c \
			$(top_srcdir)/libcperciva/cpusupport/cpusupport_x86_sse2.c \
			$(top_srcdir)/libcperciva/crypto/crypto_aes.c		\
			$(top_srcdir)/libcperciva/crypto/crypto_aesctr.c	\
			$(top_srcdir)/libcperciva/crypto/crypto_entropy.c	\
			$(top_srcdir)/libcperciva/util/entropy.c		\
			$(top_srcdir)/libcperciva/util/insecure_memzero.c	\
			$(top_srcdir)/libcperciva/util/readpass.c		\
			$(top_srcdir)/libcperciva/util/warnp.c		\
			$(top_srcdir)/lib/crypto/crypto_scrypt.c		\
			$(top_srcdir)/lib/crypto/crypto_scrypt_smix.c		\
			$(top_srcdir)/lib/scryptenc/scryptenc.c		\
			$(top_srcdir)/lib/scryptenc/scryptenc_cpuperf.c	\
			$(top_srcdir)/lib/util/memlimit.c			\
			$(top_srcdir)/cpusupport-config.h
AM_CPPFLAGS=		-I $(top_srcdir)/libcperciva/alg		\
			-I $(top_srcdir)/libcperciva/cpusupport	\
			-I $(top_srcdir)/libcperciva/crypto		\
			-I $(top_srcdir)/libcperciva/util		\
			-I $(top_srcdir)/lib/crypto			\
			-I $(top_srcdir)/lib/scryptenc 		\
			-I $(top_srcdir)/lib/util			\
			-I . -D CPUSUPPORT_CONFIG_FILE=\"cpusupport-config.h\"
test_scrypt_LDADD=		$(top_srcdir)/libcperciva_aesni.a $(top_srcdir)/libscrypt_sse2.a
