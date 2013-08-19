PROG=	scrypt
VER?=	nosse
SRCS=	main.c
LDADD+=	-lcrypto
WARNS?=	6

# We have a config file for FreeBSD
CFLAGS	+=	-I .
CFLAGS	+=	-DCONFIG_H_FILE=\"config_freebsd.h\"

# Include all possible object files containing built scrypt code.
CLEANFILES	+=	crypto_scrypt-ref.o
CLEANFILES	+=	crypto_scrypt-sse.o
CLEANFILES	+=	crypto_scrypt-nosse.o

.PATH.c	:	lib/util
.PATH.c	:	libcperciva/util
SRCS	+=	memlimit.c readpass.c warnp.c
CFLAGS	+=	-I lib/util -I libcperciva/util
.PATH.c	:	lib/crypto
.PATH.c	:	libcperciva/alg
.PATH.c	:	libcperciva/crypto
SRCS	+=	crypto_aesctr.c crypto_scrypt-${VER}.c sha256.c
CFLAGS	+=	-I lib/crypto -I libcperciva/alg -I libcperciva/crypto
.PATH.c	:	lib/scryptenc
SRCS	+=	scryptenc_cpuperf.c scryptenc.c
CFLAGS	+=	-I lib/scryptenc

#======== public code ends here
SCRYPTVERSION!=basename `pwd` | cut -f 2 -d -
PKGNAME=scrypt-${SCRYPTVERSION}
PKGSIGS=scrypt-sigs-${SCRYPTVERSION}

publish-at:
	mkdir -p ${PKGNAME}
	cp -R lib libcperciva autocrap ${PKGNAME}
	cp scrypt_platform.h main.c FORMAT scrypt.1 ${PKGNAME}
	echo -n '${SCRYPTVERSION}' > scrypt-version
	mkdir -p config.aux
	aclocal -I .
	autoheader
	automake -a -c
	autoconf
	mv Makefile.in config.h.in configure ${PKGNAME}/
	rm aclocal.m4
	mv config.aux ${PKGNAME}/
	rm scrypt-version
	tar -czf ${PKGNAME}.tgz ${PKGNAME}
	rm -rf ${PKGNAME}

publish: publish-at
	sha256 ${PKGNAME}.tgz |			\
	    gpg --secret-keyring $GPGKEYFILE --clearsign -u $GPGKEYID \
	    > ${PKGSIGS}.asc

.include <bsd.prog.mk>
