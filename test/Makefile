PROG=	test_scrypt
SRCS=	main.c
LDADD+=	-lcrypto
WARNS?=	6
NO_MAN=	YES

# We have a config file for FreeBSD
CFLAGS	+=	-I ..
CFLAGS	+=	-DCONFIG_H_FILE=\"config_freebsd.h\"

.PATH.c	:	../libcperciva/alg
SRCS	+=	sha256.c
CFLAGS	+=	-I ../libcperciva/alg
.PATH.c	:	../libcperciva/cpusupport
SRCS	+=	cpusupport_x86_sse2.c
CFLAGS	+=	-I ../libcperciva/cpusupport
.PATH.c	:	../lib/crypto
SRCS	+=	crypto_scrypt.c crypto_scrypt_smix.c crypto_scrypt_smix_sse2.c
CFLAGS	+=	-I ../lib/crypto
.PATH.c	:	../libcperciva/util
SRCS	+=	insecure_memzero.c warnp.c
CFLAGS	+=	-I ../libcperciva/util

# CPU features compiler support detection
SRCS	+=	cpusupport-config.h
cpusupport-config.h:
	( export CC="${CC}"; export CFLAGS="${CFLAGS}"; cd ../libcperciva/cpusupport/Build && command -p sh cpusupport.sh ) > cpusupport-config.h
CLEANFILES+=	cpusupport-config.h
CFLAGS	+=	-I . -D CPUSUPPORT_CONFIG_FILE=\"cpusupport-config.h\"

# Building crypto_scrypt_smix_sse2.o needs CFLAGS_X86_SSE2
crypto_scrypt_smix_sse2.o: crypto_scrypt_smix_sse2.c cpusupport-config.h
	. ./cpusupport-config.h; ${CC} ${CFLAGS} $${CFLAGS_X86_SSE2} -c $< -o $@
cpusupport_x86_sse2.o: cpusupport-config.h

test: all
	@./test_scrypt.sh
CLEANFILES+=	test_scrypt.log

.include <bsd.prog.mk>
