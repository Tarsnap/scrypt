#!/bin/sh

### Constants
c_valgrind_min=1
encrypted_file="${out}/attempt.enc"
decrypted_file="${out}/attempt.txt"
reference_file="${scriptdir}/test_scrypt.good"

# This requires the file created in 02-encrypt-file.sh
scenario_cmd() {
	# Decrypt a file.
	setup_check_variables
	(
		echo ${password} | ${c_valgrind_cmd} ${bindir}/scrypt	\
		    dec -P ${encrypted_file} ${decrypted_file}
		echo $? > ${c_exitfile}
	)

	# The decrypted file should match the reference.
	setup_check_variables
	if cmp -s ${decrypted_file} ${reference_file}; then
		echo "0"
	else
		echo "1"
	fi > ${c_exitfile}
}
