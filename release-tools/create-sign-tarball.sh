#!/bin/sh

# Process command-line and environment variables
SCRYPTVERSION=$1
if [ -z "$SCRYPTVERSION" ]; then
	echo "Please specify the version number"
	exit 1
fi

if [ -z "$GPGKEYFILE" ]; then
	echo "Please set your \$GPGKEYFILE"
	exit 1
fi

if [ -z "$GPGKEYID" ]; then
	echo "Please set your \$GPGKEYID"
	exit 1
fi

# Constants
PKGNAME=scrypt-${SCRYPTVERSION}
PKGSIGS=scrypt-sigs-${SCRYPTVERSION}

# Get directory of script
dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)

# Create tarball
sh ${dir}/mktarball.sh $SCRYPTVERSION

# Sign tarball
sha256 ${PKGNAME}.tgz |			\
    gpg --secret-keyring $GPGKEYFILE --clearsign -u $GPGKEYID \
    > ${PKGSIGS}.asc
