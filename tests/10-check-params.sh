#!/bin/sh

### Constants
c_valgrind_min=1

### Actual command
scenario_cmd() {
	# Check parameters with the normal function.
	setup_check "check-params-normal"
	${c_valgrind_cmd} "${bindir}/tests/check-params/check-params-normal"
	echo $? > "${c_exitfile}"
}
