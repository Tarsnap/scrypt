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
decrypted_no_file="${s_basename}-decrypt-no-file.txt"
cr_passphrase_file="${s_basename}-passphrase-cr.txt"
decrypted_cr_file="${s_basename}-decrypt-cr.txt"
decrypted_cr_log="${s_basename}-decrypt-cr.log"
encrypted_cr_file="${s_basename}-encrypt-cr.enc"
encrypted_cr_log="${s_basename}-encrypt-cr.log"
crlf_passphrase_file="${s_basename}-passphrase-crlf.txt"
decrypted_crlf_file="${s_basename}-decrypt-crlf.txt"

scenario_cmd() {
	# Create the passphrase file.
	echo "${password}" > "${passphrase_file}"

	# Decrypt a reference file using --passphrase file:FILENAME.
	setup_check "scrypt dec file"
	${c_valgrind_cmd} "${bindir}/scrypt"				\
	    dec --passphrase file:"${passphrase_file}"			\
	    "${encrypted_reference_file}" "${decrypted_reference_file}"
	echo $? > "${c_exitfile}"

	# The decrypted reference file should match the reference.
	setup_check "scrypt dec file output against reference"
	cmp -s "${decrypted_reference_file}" "${reference_file}"
	echo $? > "${c_exitfile}"

	# Attempt to decrypt the reference file with a non-existent file.
	# We want this command to fail with 1.
	setup_check "scrypt dec file none"
	${c_valgrind_cmd} "${bindir}/scrypt"				\
	    dec --passphrase file:THIS_FILE_DOES_NOT_EXIST		\
	    "${encrypted_reference_file}" "${decrypted_no_file}"		\
	    2> "${decrypted_no_file_log}"
	expected_exitcode 1 $? > "${c_exitfile}"

	# We should have received an error message.
	setup_check "scrypt dec file none error"
	grep -q	"scrypt: fopen(THIS_FILE_DOES_NOT_EXIST)"		\
	    "${decrypted_no_file_log}"
	echo "$?" > "${c_exitfile}"

	# We should not have created a file.
	setup_check "scrypt dec file none no file"
	test -e "${decrypted_no_file}"
	expected_exitcode 1 $? > "${c_exitfile}"

	# Attempt to decrypt the reference file with an incorrect passphrase.
	# We want this command to fail with 1.
	setup_check "scrypt dec file bad"
	echo "bad-pass" > "${bad_passphrase_file}"
	${c_valgrind_cmd} "${bindir}/scrypt"				\
	    dec --passphrase file:"${bad_passphrase_file}"		\
	    "${encrypted_reference_file}" "${decrypted_badpass_file}"		\
	    2> "${decrypted_badpass_log}"
	expected_exitcode 1 $? > "${c_exitfile}"

	# We should have received an error message.
	setup_check "scrypt dec file bad error"
	grep -q "scrypt: Passphrase is incorrect" "${decrypted_badpass_log}"
	echo "$?" > "${c_exitfile}"

	# We should not have created a file.
	setup_check "scrypt dec file bad no file"
	test -e "${decrypted_badpass_file}"
	expected_exitcode 1 $? > "${c_exitfile}"

	# A passphrase file containing a carriage return which is not part of
	# a trailing CRLF must be rejected.  The passphrase used here begins
	# with the correct passphrase, so accepting it would decrypt the file
	# with a passphrase which is not the one the file contains.
	setup_check "scrypt dec file stray CR"
	printf '%s\rextra\n' "${password}" > "${cr_passphrase_file}"
	${c_valgrind_cmd} "${bindir}/scrypt"				\
	    dec --passphrase file:"${cr_passphrase_file}"		\
	    "${encrypted_reference_file}" "${decrypted_cr_file}"	\
	    2> "${decrypted_cr_log}"
	expected_exitcode 1 $? > "${c_exitfile}"

	# We should have received an error message.
	setup_check "scrypt dec file stray CR error"
	grep -q "carriage return or newline" "${decrypted_cr_log}"
	echo "$?" > "${c_exitfile}"

	# We should not have created a file.
	setup_check "scrypt dec file stray CR no file"
	test -e "${decrypted_cr_file}"
	expected_exitcode 1 $? > "${c_exitfile}"

	# Encryption must reject the same malformed passphrase file rather than
	# silently encrypting with the prefix before the carriage return.
	setup_check "scrypt enc file stray CR"
	${c_valgrind_cmd} "${bindir}/scrypt"				\
	    enc --passphrase file:"${cr_passphrase_file}"		\
	    "${reference_file}" "${encrypted_cr_file}"		\
	    2> "${encrypted_cr_log}"
	expected_exitcode 1 $? > "${c_exitfile}"

	# We should have received the same malformed-passphrase diagnostic.
	setup_check "scrypt enc file stray CR error"
	grep -q "carriage return or newline" "${encrypted_cr_log}"
	echo "$?" > "${c_exitfile}"

	# We should not have created an encrypted output file.
	setup_check "scrypt enc file stray CR no file"
	test -e "${encrypted_cr_file}"
	expected_exitcode 1 $? > "${c_exitfile}"

	# A passphrase file with CRLF line endings must still work.
	setup_check "scrypt dec file CRLF"
	printf '%s\r\n' "${password}" > "${crlf_passphrase_file}"
	${c_valgrind_cmd} "${bindir}/scrypt"				\
	    dec --passphrase file:"${crlf_passphrase_file}"		\
	    "${encrypted_reference_file}" "${decrypted_crlf_file}"
	echo $? > "${c_exitfile}"

	setup_check "scrypt dec file CRLF output against reference"
	cmp -s "${decrypted_crlf_file}" "${reference_file}"
	echo $? > "${c_exitfile}"
}
