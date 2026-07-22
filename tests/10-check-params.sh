#!/bin/sh

### Constants
c_valgrind_min=1

### Actual command
scenario_cmd() {
	# Check parameters with the normal function.
	setup_check "check-params-normal"
	${c_valgrind_cmd} "${bindir}/tests/check-params/check-params-normal"
	echo $? > "${c_exitfile}"

	# Check parameters with the reference function.
	setup_check "check-params-reference"
	${c_valgrind_cmd} "${bindir}/tests/check-params/check-params-reference"
	echo $? > "${c_exitfile}"
}
