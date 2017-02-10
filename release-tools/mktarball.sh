#!/bin/sh

VERSION=$1
if [ -z $VERSION ]; then
	echo "Please specify the version number"
	exit 1
fi
DESTDIR=scrypt-${VERSION}
RELEASEDATE=`date "+%B %d, %Y"`

# Copy bits in
mkdir ${DESTDIR} ${DESTDIR}/autotools
cp scrypt_platform.h main.c FORMAT COPYRIGHT BUILDING ${DESTDIR}
cp Makefile.am .autom4te.cfg ${DESTDIR}
cp Makefile.am ${DESTDIR}/autotools
cp -R lib libcperciva tests ${DESTDIR}
# Copy with substitution
sed -e "s/@DATE@/$RELEASEDATE/" < scrypt.1 > ${DESTDIR}/scrypt.1
sed -e "s/\[m4_esyscmd(\[sh get-version\.sh\])]/${VERSION}/" \
	< configure.ac > ${DESTDIR}/configure.ac
cp ${DESTDIR}/configure.ac ${DESTDIR}/autotools

# Generate autotools files
( cd ${DESTDIR}
autoreconf -i
rm .autom4te.cfg Makefile.am aclocal.m4 configure.ac )

# Create tarball
tar -czf ${DESTDIR}.tgz ${DESTDIR}
rm -r ${DESTDIR}
