#!/bin/sh

### Constants
c_valgrind_min=1
reference_file="${scriptdir}/verify-strings/test_scrypt.good"
encrypted_reference_file="${scriptdir}/verify-strings/test_scrypt_good.enc"
decrypted_reference_file="${s_basename}-attempt_reference.txt"
decrypted_badpass_file="${s_basename}-decrypt-badpass.txt"
decrypted_badpass_log="${s_basename}-decrypt-badpass.log"
decrypted_no_envvar_log="${s_basename}-decrypt-no-envvar.log"

scenario_cmd() {
	# Decrypt a reference file using --passphrase env:VAR.
	setup_check_variables
	PASSPHRASE="${password}"					\
	${c_valgrind_cmd} ${bindir}/scrypt				\
	    dec --passphrase env:PASSPHRASE				\
	    ${encrypted_reference_file} ${decrypted_reference_file}
	echo $? > ${c_exitfile}

	# The decrypted reference file should match the reference.
	setup_check_variables
	cmp -s ${decrypted_reference_file} ${reference_file}
	echo $? > ${c_exitfile}

	# Attempt to decrypt the reference file with a non-existent envvar.
	# We want this command to fail with 1.
	setup_check_variables
	${c_valgrind_cmd} ${bindir}/scrypt				\
	    dec --passphrase env:THIS_ENVVAR_DOES_NOT_EXIST		\
	    ${encrypted_reference_file} ${decrypted_reference_file}	\
	    2> ${decrypted_no_envvar_log}
	expected_exitcode 1 $? > ${c_exitfile}

	# We should have received an error message.
	setup_check_variables
	grep -q								\
	    "scrypt: Failed to read from \${THIS_ENVVAR_DOES_NOT_EXIST}" \
	    ${decrypted_no_envvar_log}
	echo "$?" > ${c_exitfile}

	# We should not have created a file.
	setup_check_variables
	test -e ${decrypted_badpass_file}
	expected_exitcode 1 $? > ${c_exitfile}

	# Attempt to decrypt the reference file with an incorrect passphrase.
	# We want this command to fail with 1.
	setup_check_variables
	PASSPHRASE="bad-pass"						\
	${c_valgrind_cmd} ${bindir}/scrypt				\
	    dec --passphrase env:PASSPHRASE				\
	    ${encrypted_reference_file} ${decrypted_reference_file}	\
	    2> ${decrypted_badpass_log}
	expected_exitcode 1 $? > ${c_exitfile}

	# We should have received an error message.
	setup_check_variables
	grep -q "scrypt: Passphrase is incorrect" ${decrypted_badpass_log}
	echo "$?" > ${c_exitfile}

	setup_check_variables
	# We should not have created a file.
	test -e ${decrypted_badpass_file}
	expected_exitcode 1 $? > ${c_exitfile}
}
