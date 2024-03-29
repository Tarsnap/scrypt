#!/bin/sh

### Constants
c_valgrind_min=1
reference_file="${scriptdir}/verify-strings/test_scrypt.good"
encrypted_file_1="${s_basename}-sys.enc"
decrypted_file_1="${s_basename}-sys.txt"
encrypted_file_2="${s_basename}-our.enc"
decrypted_file_2="${s_basename}-our.txt"

scenario_cmd() {
	if [ -z "${system_scrypt}" ]; then
		printf "no suitable system scrypt: " 1>&2
		# Inform test suite that we are skipping.
		setup_check "system scrypt skip"
		echo "-1" > "${c_exitfile}"
		return
	fi

	# Encrypt a file with our scrypt.
	setup_check "scrypt enc for system"
	(
		echo "${password}" | ${c_valgrind_cmd} "${bindir}/scrypt" \
		    enc -P -t 1 "${reference_file}" "${encrypted_file_1}"
		echo $? > "${c_exitfile}"
	)

	# Use the system scrypt to decrypt the file we just
	# encrypted.  Don't use valgrind for this.
	setup_check "system scrypt dec"
	(
		echo "${password}" | ${system_scrypt}			\
		    dec -P "${encrypted_file_1}" "${decrypted_file_1}"
		echo $? > "${c_exitfile}"
	)

	# The decrypted file should match the reference.
	setup_check "system scrypt dec output against reference"
	cmp -s "${decrypted_file_1}" "${reference_file}"
	echo $? > "${c_exitfile}"

	# Encrypt a file with the system scrypt.  Don't use
	# valgrind for this.
	setup_check "system scrypt enc"
	(
		echo "${password}" | ${system_scrypt}			\
		    enc -P -t 1 "${reference_file}" "${encrypted_file_2}"
		echo $? > "${c_exitfile}"
	)

	# Use our scrypt to decrypt the file we just encrypted.
	setup_check "scrypt dec for system"
	(
		echo "${password}" | ${c_valgrind_cmd} "${bindir}/scrypt" \
		    dec -P "${encrypted_file_2}" "${decrypted_file_2}"
		echo $? > "${c_exitfile}"
	)

	# The decrypted file should match the reference.
	setup_check "scrypt dec for system output against reference"
	cmp -s "${decrypted_file_2}" "${reference_file}"
	echo $? > "${c_exitfile}"
}
