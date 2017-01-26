#!/bin/sh

version=1.2.0-head

# This script outputs a version number for this project to stdout.
# - if $SCRYPT_VERSION is given, it is used.
# - otherwise, it uses ${version}.
# - if there is a ".git/" directory, it will attempt to get a version number
#   from `git describe` in the form 1.2.0-238-g0a25a7c, where the middle value
#   is the number of commits since the 1.2.0 tag.

# Use $SCRYPT_VERSION if it exists.
if [ -n "${SCRYPT_VERSION}" ]; then
	# Do not use \n; that confuses autoconf.
	printf "${SCRYPT_VERSION}"
	exit 0
fi

# Get a version number from git, if it exists.
if git rev-parse 2>/dev/null; then
	# Get a version string from the latest git tag.
	if version_git=$( git describe --tags --match '[[:digit:]].*' ) \
	    2>/dev/null ; then
		version_decapitated=$( echo ${version} | sed "s/-head//" )
		# Check that the beginning of this tag matches the version.
		case ${version_git} in
		"${version_decapitated}"*)
			# If so, use that version string.
			version=${version_git};;
		*)
			printf "git tag does not match version\n" 1>&2
			exit 1;;
		esac
	fi
fi

# Output the version to stdout.  Do not use \n; that confuses autoconf.
printf ${version}
