#!/bin/sh

### Constants
c_valgrind_min=1
reference_file="${scriptdir}/verify-strings/test_scrypt.good"
encrypted_reference_file="${scriptdir}/verify-strings/test_scrypt_good.enc"
decrypted_reference_file="${s_basename}-attempt_reference.txt"
decrypted_reference_file_stderr="${s_basename}-attempt_reference.stderr"
decrypted_badpass_file="${s_basename}-decrypt-badpass.txt"
decrypted_badpass_log="${s_basename}-decrypt-badpass.log"

scenario_cmd() {
	# Decrypt a reference file.
	setup_check_variables "scrypt dec"
	(
		echo ${password} | ${c_valgrind_cmd} ${bindir}/scrypt	\
		    dec -P ${encrypted_reference_file}			\
		    ${decrypted_reference_file}				\
		    2> ${decrypted_reference_file_stderr}
		echo $? > ${c_exitfile}
	)

	# The decrypted reference file should match the reference.
	setup_check_variables "scrypt dec output against reference"
	cmp -s ${decrypted_reference_file} ${reference_file}
	echo $? > ${c_exitfile}

	# We should not have any output on stderr.
	setup_check_variables "scrypt dec no stderr"
	test -s ${decrypted_reference_file_stderr}
	expected_exitcode 1 $? > ${c_exitfile}

	# Attempt to decrypt the reference file with an incorrect passphrase.
	# We want this command to fail with 1.
	setup_check_variables "scrypt dec bad passphrase"
	(
		echo "bad-pass" | ${c_valgrind_cmd} ${bindir}/scrypt	\
		    dec -P ${encrypted_reference_file}			\
		    ${decrypted_badpass_file}				\
		    2> ${decrypted_badpass_log}
		expected_exitcode 1 $? > ${c_exitfile}
	)

	# We should have received an error message.
	setup_check_variables "scrypt dec bad passphrase error"
	if grep -q "scrypt: Passphrase is incorrect" \
	    ${decrypted_badpass_log}; then
		echo "0"
	else
		echo "1"
	fi > ${c_exitfile}

	# We should not have created a file.
	setup_check_variables "scrypt dec bad passphrase no file"
	if [ -e ${decrypted_badpass_file} ]; then
		echo "1"
	else
		echo "0"
	fi > ${c_exitfile}
}
