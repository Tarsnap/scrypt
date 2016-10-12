#!/bin/sh

# Build directory (allowing flexible out-of-tree builds).
bindir=$1

# Constants used in multiple scenarios.
password="hunter2"
encrypted_file="attempt.enc"


################################ Setup variables from the command-line

# Find script directory and load helper functions.
scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
. $scriptdir/shared_test_functions.sh

if [ -z $bindir ]; then
	printf "Error: Scrypt binary directory not given.\n"
	printf "Attempting to use default values for in-source-tree build.\n"
	bindir=".."
fi

# Check for optional valgrind
USE_VALGRIND=$( check_optional_valgrind )

# Clean up previous directories, and create new ones.
prepare_directories

# Generate valgrind suppression file if it is required.  Must be
# done after preparing directories.
ensure_valgrind_suppression ${bindir}/tests/valgrind/potential-memleaks

################################ Run tests

# Run tests.
scenario_filenames=$scriptdir/??-*.sh
for scenario in $scenario_filenames; do
	# We can't call this function with $( ... ) because we want to allow
	# it to echo values to stdout.
	scenario_runner $scenario
	retval=$?
	if [ $retval -gt 0 ]; then
		exit $retval
	fi
done

# Return value to Makefile.
exit $?

