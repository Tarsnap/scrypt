PROG=	scrypt
SRCS=	main.c
LDADD+=	-lcrypto
WARNS?=	6

# We have a config file for FreeBSD
CFLAGS	+=	-I .
CFLAGS	+=	-DCONFIG_H_FILE=\"config_freebsd.h\"

.PATH.c	:	libcperciva/alg
SRCS	+=	sha256.c
CFLAGS	+=	-I libcperciva/alg
.PATH.c	:	libcperciva/cpusupport
SRCS	+=	cpusupport_x86_aesni.c
SRCS	+=	cpusupport_x86_sse2.c
CFLAGS	+=	-I libcperciva/cpusupport
.PATH.c	:	libcperciva/crypto
SRCS	+=	crypto_aes.c
SRCS	+=	crypto_aes_aesni.c
SRCS	+=	crypto_aesctr.c
SRCS	+=	crypto_entropy.c
CFLAGS	+=	-I libcperciva/crypto
.PATH.c	:	libcperciva/util
SRCS	+=	asprintf.c
SRCS	+=	entropy.c
SRCS	+=	humansize.c
SRCS	+=	insecure_memzero.c
SRCS	+=	readpass.c
SRCS	+=	warnp.c
CFLAGS	+=	-I libcperciva/util
.PATH.c	:	lib/crypto
SRCS	+=	crypto_scrypt.c
SRCS	+=	crypto_scrypt_smix.c
SRCS	+=	crypto_scrypt_smix_sse2.c
CFLAGS	+=	-I lib/crypto
.PATH.c	:	lib/scryptenc
SRCS	+=	scryptenc.c
SRCS	+=	scryptenc_cpuperf.c
CFLAGS	+=	-I lib/scryptenc
.PATH.c	:	lib/util
SRCS	+=	memlimit.c
CFLAGS	+=	-I lib/util

# CPU features compiler support detection
SRCS	+=	cpusupport-config.h
cpusupport-config.h:
	( export CC="${CC}"; export CFLAGS="${CFLAGS}"; command -p sh libcperciva/cpusupport/Build/cpusupport.sh ) > cpusupport-config.h.tmp && command -p mv cpusupport-config.h.tmp cpusupport-config.h
CLEANFILES+=	cpusupport-config.h cpusupport-config.h.tmp
CFLAGS	+=	-I . -D CPUSUPPORT_CONFIG_FILE=\"cpusupport-config.h\"

# Building crypto_aes_aesni.o needs CFLAGS_X86_AESNI
crypto_aes_aesni.o: crypto_aes_aesni.c cpusupport-config.h
	. ./cpusupport-config.h; ${CC} ${CFLAGS} $${CFLAGS_X86_AESNI} -c $< -o $@

# Building crypto_scrypt_smix_sse2.o needs CFLAGS_X86_SSE2
crypto_scrypt_smix_sse2.o: crypto_scrypt_smix_sse2.c cpusupport-config.h
	. ./cpusupport-config.h; ${CC} ${CFLAGS} $${CFLAGS_X86_SSE2} -c $< -o $@

#======== public code ends here
PKGNAME=scrypt-${SCRYPTVERSION}
PKGSIGS=scrypt-sigs-${SCRYPTVERSION}

publish-at:
	sh release-tools/mktarball.sh ${SCRYPTVERSION}

publish: publish-at
	sha256 ${PKGNAME}.tgz |			\
	    gpg --secret-keyring $GPGKEYFILE --clearsign -u $GPGKEYID \
	    > ${PKGSIGS}.asc

.include <bsd.prog.mk>
