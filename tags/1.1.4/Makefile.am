AUTOMAKE_OPTIONS=	foreign

bin_PROGRAMS=	scrypt
dist_man_MANS=$(scrypt_man_MANS)

scrypt_SOURCES=		main.c				\
			lib/util/memlimit.c		\
			lib/util/readpass.c		\
			lib/util/warn.c			\
			lib/scryptenc/scryptenc.c	\
			lib/scryptenc/scryptenc_cpuperf.c	\
			lib/crypto/crypto_aesctr.c	\
			lib/crypto/sha256.c		\
			lib/crypto/crypto_scrypt-@SCRYPTVER@.c
scrypt_CFLAGS=		-I . -I lib/util -I lib/scryptenc -I lib/crypto
scrypt_LDADD=		-lcrypto
scrypt_man_MANS=	scrypt.1
