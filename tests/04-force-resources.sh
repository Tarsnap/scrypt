#!/bin/sh

### Constants
c_valgrind_min=1
reference_file="${scriptdir}/verify-strings/test_scrypt.good"
longwait_encrypted_file="${s_basename}.enc"
longwait_decrypted_file="${s_basename}.txt"
longwait_failed_log="${s_basename}-failed.log"

scenario_cmd() {
	# Encrypt file which should take a long time to decrypt.
	setup_check_variables "scrypt enc 10 seconds"
	(
		echo ${password} | ${c_valgrind_cmd} ${bindir}/scrypt	\
			enc -P -t 10 ${reference_file}			\
			${longwait_encrypted_file}
		echo $? > ${c_exitfile}
	)

	# Attempt to decrypt it with limited time.  We want this
	# command to fail, so we negate the normal return code.
	setup_check_variables "scrypt dec 1 second"
	(
		echo ${password} | ${c_valgrind_cmd} ${bindir}/scrypt	\
			dec -P -t 1 ${longwait_encrypted_file}		\
			${longwait_decrypted_file}			\
			2> ${longwait_failed_log}
		expected_exitcode 1 $? > ${c_exitfile}
	)

	# We should have received an error message.
	setup_check_variables "scrypt dec 1 second error"
	grep -q "scrypt: Decrypting file would take too much CPU time" \
	    ${longwait_failed_log}
	echo "$?" > ${c_exitfile}

	# Attempt to decrypt it with limited time, but force success.
	setup_check_variables "scrypt dec force"
	(
		echo ${password} | ${c_valgrind_cmd} ${bindir}/scrypt	\
			dec -P -t 1 -f ${longwait_encrypted_file}	\
			${longwait_decrypted_file}
		echo $? > ${c_exitfile}
	)

	# The decrypted reference file should match the reference.
	setup_check_variables "scrypt dec force output against reference"
	cmp -s ${longwait_decrypted_file} ${reference_file}
	echo $? > ${c_exitfile}
}
