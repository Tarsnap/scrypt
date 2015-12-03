bin_PROGRAMS=	test_scrypt

test_scrypt_SOURCES=  test_scrypt.c 	\
			../libcperciva/alg/sha256.c		\
			../libcperciva/cpusupport/cpusupport_x86_aesni.c \
			../libcperciva/cpusupport/cpusupport_x86_sse2.c \
			../libcperciva/crypto/crypto_aes.c		\
			../libcperciva/crypto/crypto_aesctr.c	\
			../libcperciva/crypto/crypto_entropy.c	\
			../libcperciva/util/entropy.c		\
			../libcperciva/util/insecure_memzero.c	\
			../libcperciva/util/readpass.c		\
			../libcperciva/util/warnp.c		\
			../lib/crypto/crypto_scrypt.c		\
			../lib/crypto/crypto_scrypt_smix.c		\
			../lib/scryptenc/scryptenc.c		\
			../lib/scryptenc/scryptenc_cpuperf.c	\
			../lib/util/memlimit.c			\
			../cpusupport-config.h
AM_CPPFLAGS=		-I ../libcperciva/alg		\
			-I ../libcperciva/cpusupport	\
			-I ../libcperciva/crypto		\
			-I ../libcperciva/util		\
			-I ../lib/crypto			\
			-I ../lib/scryptenc 		\
			-I ../lib/util			\
			-I . -D CPUSUPPORT_CONFIG_FILE=\"cpusupport-config.h\"
test_scrypt_LDADD=		../libcperciva_aesni.a ../libscrypt_sse2.a
