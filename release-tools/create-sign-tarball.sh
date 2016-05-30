#!/bin/sh

# Process command-line arguments
SCRYPTVERSION=$1
GNUPG_SIGNING_HOME=$2

# Check for required arguments
if [ -z "$SCRYPTVERSION" ] || [ -z "$GNUPG_SIGNING_HOME" ]; then
	echo "Usage: $0 SCRYPTVERSION GNUPG_SIGNING_HOME"
	exit 1
fi

# Check for correct OS
if [ `uname` != "FreeBSD" ]; then
	echo "Error: This script only works on FreeBSD due to the"
	echo "    sha256 \${PKGNAME}.tgz"
	echo "command, which has a different meaning on other OSes."
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
    GNUPGHOME=${GNUPG_SIGNING_HOME} gpg --clearsign \
    > ${PKGSIGS}.asc
