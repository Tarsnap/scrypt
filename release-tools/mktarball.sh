#!/bin/sh

VERSION=$1
if [ -z $VERSION ]; then
	echo "Please specify the version number"
	exit 1
fi
DESTDIR=scrypt-${VERSION}

# Copy bits in
mkdir ${DESTDIR} ${DESTDIR}/autocrap
cp scrypt_platform.h main.c FORMAT scrypt.1 ${DESTDIR}
cp Makefile.am configure.ac .autom4te.cfg ${DESTDIR}
cp Makefile.am configure.ac ${DESTDIR}/autocrap
cp -R lib libcperciva ${DESTDIR}

# Generate autotools files
( cd ${DESTDIR}
echo -n ${VERSION} > scrypt-version
autoreconf -i
rm .autom4te.cfg Makefile.am aclocal.m4 configure.ac scrypt-version )

# Create tarball
tar -czf ${DESTDIR}.tgz ${DESTDIR}
rm -r ${DESTDIR}
