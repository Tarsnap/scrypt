PROG=	scrypt
VER?=	nosse
SRCS=	main.c sha256.c scryptenc.c crypto_aesctr.c warn.c
SRCS+=	memlimit.c scrypt_cpuperf.c scrypt-${VER}.c
LDADD+=	-lcrypto
WARNS?=	6

# We have a config file for FreeBSD
CFLAGS	+=	-DCONFIG_H_FILE=\"config_freebsd.h\"

# Include all possible object files containing built scrypt code.
CLEANFILES	+=	scrypt-ref.o scrypt-sse.o scrypt-nosse.o

#======== public code ends here
SCRYPTVERSION!=basename `pwd` | cut -f 2 -d -
PKGNAME=scrypt-${SCRYPTVERSION}
PKGSIGS=scrypt-sigs-${SCRYPTVERSION}

publish-at:
	mkdir -p ${PKGNAME}
	cp crypto_aesctr.[ch] memlimit.[ch] sha256.[ch]		\
	    scryptenc.[ch] warn.[ch] scrypt_cpuperf.[ch]	\
	    scrypt-*.c						\
	    scrypt.h sysendian.h scrypt_platform.h main.c	\
	    ${PKGNAME}
	cp FORMAT scrypt.1 ${PKGNAME}
	echo -n '${SCRYPTVERSION}' > scrypt-version
	mkdir -p config.aux
	aclocal-1.10 -I .
	autoheader-2.62
	automake-1.10 -a -c
	autoconf-2.62
	mv Makefile.in config.h.in configure ${PKGNAME}/
	rm aclocal.m4
	mv config.aux ${PKGNAME}/
	rm scrypt-version
	tar -czf ${PKGNAME}.tgz ${PKGNAME}
	rm -rf ${PKGNAME}

publish: publish-at
	sha256 ${PKGNAME}.tgz |			\
	    gpg --secret-keyring ../EC2/md/gpg.key --clearsign -u 3DD61E72 \
	    > ${PKGSIGS}.asc

.include <bsd.prog.mk>
