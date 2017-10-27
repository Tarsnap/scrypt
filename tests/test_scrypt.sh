#!/bin/sh

### Find script directory and load helper functions.
scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
. ${scriptdir}/shared_test_functions.sh


### Project-specific constants and setup

out="${bindir}/tests-output"
out_valgrind="${bindir}/tests-valgrind"

# Constants used in multiple scenarios for this project.
password="hunter2"

# Find system scrypt, and ensure it supports -P.
system_scrypt=$( find_system scrypt enc -P )


### Run tests using project-specific constants

# Check for optional valgrind.
check_optional_valgrind

# Clean up previous directories, and create new ones.
prepare_directories

# Generate valgrind suppression file if it is required.  Must be
# done after preparing directories.
ensure_valgrind_suppression ${bindir}/tests/valgrind/potential-memleaks

# Run the test scenarios; this will exit on the first failure.
run_scenarios ${scriptdir}/??-*.sh
