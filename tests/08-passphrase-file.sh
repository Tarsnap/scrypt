#!/bin/sh

### Constants
c_valgrind_min=1
reference_file="${scriptdir}/verify-strings/test_scrypt.good"
passphrase_file="${s_basename}-passphrase.txt"
bad_passphrase_file="${s_basename}-passphrase-bad.txt"
encrypted_reference_file="${scriptdir}/verify-strings/test_scrypt_good.enc"
decrypted_reference_file="${s_basename}-attempt_reference.txt"
decrypted_badpass_file="${s_basename}-decrypt-badpass.txt"
decrypted_badpass_log="${s_basename}-decrypt-badpass.log"
decrypted_no_file_log="${s_basename}-decrypt-no-file.log"

scenario_cmd() {
	# Create the passphrase file.
	echo "${password}" > ${passphrase_file}

	# Decrypt a reference file using --passphrase file:FILENAME.
	setup_check_variables
	${c_valgrind_cmd} ${bindir}/scrypt				\
	    dec --passphrase file:${passphrase_file}			\
	    ${encrypted_reference_file} ${decrypted_reference_file}
	echo $? > ${c_exitfile}

	# The decrypted reference file should match the reference.
	setup_check_variables
	cmp -s ${decrypted_reference_file} ${reference_file}
	echo $? > ${c_exitfile}

	# Attempt to decrypt the reference file with a non-existent file.
	# We want this command to fail with 1.
	setup_check_variables
	${c_valgrind_cmd} ${bindir}/scrypt				\
	    dec --passphrase file:THIS_FILE_DOES_NOT_EXIST		\
	    ${encrypted_reference_file} ${decrypted_reference_file}	\
	    2> ${decrypted_no_file_log}
	expected_exitcode 1 $? > ${c_exitfile}

	# We should have received an error message.
	setup_check_variables
	grep -q	"scrypt: fopen(THIS_FILE_DOES_NOT_EXIST)"		\
	    ${decrypted_no_file_log}
	echo "$?" > ${c_exitfile}

	# We should not have created a file.
	setup_check_variables
	test -e ${decrypted_badpass_file}
	expected_exitcode 1 $? > ${c_exitfile}

	# Attempt to decrypt the reference file with an incorrect passphrase.
	# We want this command to fail with 1.
	setup_check_variables
	echo "bad-pass" > ${bad_passphrase_file}
	${c_valgrind_cmd} ${bindir}/scrypt				\
	    dec --passphrase file:${bad_passphrase_file}		\
	    ${encrypted_reference_file} ${decrypted_reference_file}	\
	    2> ${decrypted_badpass_log}
	expected_exitcode 1 $? > ${c_exitfile}

	# We should have received an error message.
	setup_check_variables
	grep -q "scrypt: Passphrase is incorrect" ${decrypted_badpass_log}
	echo "$?" > ${c_exitfile}

	# We should not have created a file.
	setup_check_variables
	test -e ${decrypted_badpass_file}
	expected_exitcode 1 $? > ${c_exitfile}
}
