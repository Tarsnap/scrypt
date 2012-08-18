AUTOMAKE_OPTIONS=	foreign

bin_PROGRAMS=	scrypt
dist_man_MANS=$(scrypt_man_MANS)

scrypt_SOURCES=		main.c				\
			scryptenc.c			\
			crypto_aesctr.c			\
			sha256.c			\
			memlimit.c			\
			warn.c				\
			scrypt_cpuperf.c		\
			scrypt-@SCRYPTVER@.c
scrypt_LDADD=		-lcrypto
scrypt_man_MANS=	scrypt.1
