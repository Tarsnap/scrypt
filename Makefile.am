AUTOMAKE_OPTIONS=	foreign

bin_PROGRAMS=	scrypt
dist_man_MANS=$(scrypt_man_MANS)

scrypt_SOURCES=		main.c				\
			lib/util/memlimit.c		\
			libcperciva/util/readpass.c		\
			libcperciva/util/warnp.c		\
			lib/scryptenc/scryptenc.c	\
			lib/scryptenc/scryptenc_cpuperf.c	\
			libcperciva/crypto/crypto_aesctr.c	\
			libcperciva/alg/sha256.c		\
			lib/crypto/crypto_scrypt-@SCRYPTVER@.c
scrypt_CFLAGS=		-I $(srcdir)/lib/util -I $(srcdir)/lib/scryptenc \
			-I $(srcdir)/lib/crypto -I $(srcdir)/libcperciva/alg \
			-I $(srcdir)/libcperciva/crypto	-I $(srcdir)/libcperciva/util
scrypt_LDADD=		-lcrypto
scrypt_man_MANS=	scrypt.1
