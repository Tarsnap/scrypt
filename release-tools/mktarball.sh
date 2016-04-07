#!/bin/sh

VERSION=$1
if [ -z $VERSION ]; then
	echo "Please specify the version number"
	exit 1
fi
DESTDIR=scrypt-${VERSION}
RELEASEDATE=`date "+%B %d, %Y"`

# Copy bits in
mkdir ${DESTDIR} ${DESTDIR}/autocrap
cp scrypt_platform.h main.c FORMAT ${DESTDIR}
cp Makefile.am configure.ac .autom4te.cfg ${DESTDIR}
cp Makefile.am configure.ac ${DESTDIR}/autocrap
cp -R lib libcperciva ${DESTDIR}
# Copy with substitution
sed -e "s/@DATE@/$RELEASEDATE/" < scrypt.1 > ${DESTDIR}/scrypt.1

# Generate autotools files
( cd ${DESTDIR}
printf ${VERSION} > scrypt-version
autoreconf -i
rm .autom4te.cfg Makefile.am aclocal.m4 configure.ac scrypt-version )

# Create tarball
tar -czf ${DESTDIR}.tgz ${DESTDIR}
rm -r ${DESTDIR}
