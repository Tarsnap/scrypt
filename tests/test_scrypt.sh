#!/bin/sh

# Build directory (allowing flexible out-of-tree builds).
bindir=$1

# Constants
password="hunter2"
known_values="known_values.txt"
encrypted_file="attempt.enc"
decrypted_file="attempt.txt"
decrypted_reference_file="attempt_reference.txt"
out_valgrind="test-valgrind"


################################ Setup variables from the command-line

# Find script directory and load helper functions.
scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
. $scriptdir/shared_test_functions.sh

if [ -z $bindir ]; then
	printf "Error: Scrypt binary directory not given.\n"
	printf "Attempting to use default values for in-source-tree build.\n"
	bindir=".."
fi

# Check for optional valgrind
USE_VALGRIND=$( check_optional_valgrind )

################################ Test functions

test_known_values() {
	basename="01-generate-known-test-values"
	printf "Running test: $basename... "

	# Set up valgrind command (if requested).  $test_scrypt_binary
	# requires a lot of memory, so valgrind is only enabled if
	# $USE_VALGRIND > 1.
	val_logfilename=$out_valgrind/$basename-val.log
	val_cmd=$( setup_valgrind_cmd $val_logfilename 1 )

	# Run actual test command.
	$val_cmd $bindir/test/test_scrypt > $known_values
	cmd_retval=$?

	# Check results.
	retval=$cmd_retval

	# The generated values should match the known good values.
	if cmp -s $known_values $scriptdir/test_scrypt.good; then
		# Clean up temporary file.
		rm $known_values
	else
		retval=1
	fi

	# Print PASS or FAIL, and return result.
	notify_success_or_fail $retval $cmd_retval $val_logfilename
	return "$retval"
}

test_encrypt_file() {
	basename="02-encrypt-a-file"
	printf "Running test: $basename... "

	# Set up valgrind command (if requested).
	val_logfilename=$out_valgrind/$basename-val.log
	val_cmd=$( setup_valgrind_cmd $val_logfilename )

	# Run actual test command.
	echo $password | $val_cmd $bindir/scrypt enc -P \
		$scriptdir/test_scrypt.good $encrypted_file
	cmd_retval=$?

	# Check results.
	retval=$cmd_retval

	# The encrypted file should be different from the original file.
	# We cannot check against the "reference" encrypted file, because
	# encrypted files include random salt.  If successful, don't delete
	# $encrypted_file yet; we need it for the next test.
	if cmp -s $encrypted_file $scriptdir/test_scrypt.good; then
		retval=1
	fi

	# Print PASS or FAIL, and return result.
	notify_success_or_fail $retval $cmd_retval $val_logfilename
	return "$retval"
}

test_decrypt_file() {
	basename="03-decrypt-a-file"
	printf "Running test: $basename... "

	# Set up valgrind command (if requested).
	val_logfilename=$out_valgrind/$basename-val.log
	val_cmd=$( setup_valgrind_cmd $val_logfilename )

	# Run actual test command.
	echo $password | $val_cmd $bindir/scrypt dec -P $encrypted_file \
		$decrypted_file
	cmd_retval=$?

	# Check results.
	retval=$cmd_retval

	# The decrypted file should match the reference.
	if cmp -s $decrypted_file $scriptdir/test_scrypt.good; then
		# Clean up temporary files.
		rm $encrypted_file
		rm $decrypted_file
	else
		retval=1
	fi

	# Print PASS or FAIL, and return result.
	notify_success_or_fail $retval $cmd_retval $val_logfilename
	return "$retval"
}

test_decrypt_reference_file() {
	basename="04-decrypt-a-reference-encrypted-file"
	printf "Running test: $basename... "

	# Set up valgrind command (if requested).
	val_logfilename=$out_valgrind/$basename-val.log
	val_cmd=$( setup_valgrind_cmd $val_logfilename )

	# Run actual test command.
	echo $password | $val_cmd $bindir/scrypt dec -P \
		$scriptdir/test_scrypt_good.enc $decrypted_reference_file
	cmd_retval=$?

	# Check results.
	retval=$cmd_retval

	# The decrypted reference file should match the reference.
	if cmp -s $decrypted_reference_file $scriptdir/test_scrypt.good; then
		rm $decrypted_reference_file
	else
		retval=1
	fi

	# Print PASS or FAIL, and return result.
	notify_success_or_fail $retval $cmd_retval $val_logfilename
	return "$retval"
}

################################ Run tests

# Clean up previous valgrind (if in use).
if [ "$USE_VALGRIND" -gt 0 ]; then
	if [ -d "$out_valgrind" ]; then
		rm -rf $out_valgrind
	fi
	mkdir $out_valgrind
fi

# Run tests.
test_known_values &&			\
	test_encrypt_file &&		\
	test_decrypt_file &&		\
	test_decrypt_reference_file	\

# Return value to Makefile.
exit $?

