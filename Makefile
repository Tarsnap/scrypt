PROG=	scrypt
VER?=	nosse
SRCS=	main.c sha256.c scrypt-${VER}.c scryptenc.c crypto_aesctr.c
LDADD+=	-lcrypto
WARNS?=	6
NO_MAN=	yes

# We have a config file for FreeBSD
CFLAGS	+=	-DCONFIG_H_FILE=\"config_freebsd.h\"

# Include all possible object files containing built scrypt code.
CLEANFILES	+=	scrypt-ref.o scrypt-sse.o scrypt-nosse.o

.include <bsd.prog.mk>
