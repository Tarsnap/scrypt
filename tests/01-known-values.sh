#!/bin/sh

### Constants
# The scenario command requires a lot of memory, so valgrind is only enabled
# if $USE_VALGRIND > 1.
c_valgrind_min=2
test_output="${s_basename}-stdout.txt"
reference="${scriptdir}/verify-strings/test_scrypt.good"
reference_small="${scriptdir}/verify-strings/test_scrypt_small.good"

### Actual command
scenario_cmd() {
	# Run the binary which tests known input/output strings.
	setup_check "test_scrypt"
	(
		${c_valgrind_cmd} "${bindir}/tests/verify-strings/test_scrypt" \
			"${SMALLMEM:-0}" 1> "${test_output}"
		echo $? > "${c_exitfile}"
	)

	# The generated values should match the known good values.
	setup_check "test_scrypt output against reference"
	if [ "${SMALLMEM:-0}" -gt "0" ]; then
		cmp -s "${test_output}" "${reference_small}"
	else
		cmp -s "${test_output}" "${reference}"
	fi
	echo $? > "${c_exitfile}"
}
