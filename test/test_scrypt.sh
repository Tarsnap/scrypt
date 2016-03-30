#!/bin/sh

# File locations (allowing flexible out-of-tree builds)
scrypt_binary=$1
test_scrypt_binary=$2
reference_txt=$3
reference_enc=$4

# Constants
password="hunter2"
known_values="known_values.txt"
encrypted_file="attempt.enc"
decrypted_file="attempt.txt"
decrypted_reference_file="attempt_reference.txt"

# Check for parameters
if [ -z $scrypt_binary ] || [ -z $test_scrypt_binary ] || \
    [ -z $reference_txt ] || [ -z $reference_enc ]; then
	printf "Error: Scrypt binary, test binary, good file, or good "
	echo "encrypted file not given."
	echo "Attempting to use default values for in-source-tree build."
	scrypt_binary="../scrypt"
	test_scrypt_binary="./test_scrypt"
	reference_txt="./test_scrypt.good"
	reference_enc="./test_scrypt_good.enc"
fi
if [ ! -f $scrypt_binary ] || [ ! -f $test_scrypt_binary ] || \
    [ ! -f $reference_txt ] || [ ! -f $reference_enc ]; then
	echo "Error: Cannot find at least one required file."
	exit 1
fi

# Test functions
test_known_values() {
	printf "Test 01: Generate known test values... "
	$test_scrypt_binary > $known_values

	# The generated values should match the known good values.
	if cmp -s $known_values $reference_txt; then
		rm $known_values
		echo "PASSED!"
		retval=0
	else
		echo "FAILED!"
		retval=1
	fi
	return "$retval"
}

test_encrypt_file() {
	printf "Test 02: Encrypt a file... "
	echo $password | $scrypt_binary enc -P $reference_txt \
		$encrypted_file

	# The encrypted file should be different from the original file.
	# We cannot check against the "reference" encrypted file, because
	# encrypted files include random salt.
	if ! cmp -s $encrypted_file $reference_txt; then
		# don't delete $encrypted_file yet; we need it for the
		# next test.
		echo "PASSED!"
		retval=0
	else
		echo "FAILED!"
		retval=1
	fi
	return "$retval"
}

test_decrypt_file() {
	printf "Test 03: Decrypt a file... "
	echo $password | $scrypt_binary dec -P $encrypted_file \
		$decrypted_file

	# The decrypted file should match the reference.
	if cmp -s $decrypted_file $reference_txt; then
		rm $encrypted_file
		rm $decrypted_file
		echo "PASSED!"
		retval=0
	else
		echo "FAILED!"
		retval=1
	fi
	return "$retval"
}

test_decrypt_reference_file() {
	printf "Test 04: Decrypt a reference encrypted file... "
	echo $password | $scrypt_binary dec -P $reference_enc \
		$decrypted_reference_file

	# The decrypted reference file should match the reference.
	if cmp -s $decrypted_reference_file $reference_txt; then
		rm $decrypted_reference_file
		echo "PASSED!"
		retval=0
	else
		echo "FAILED!"
		retval=1
	fi
	return "$retval"
}

# Run tests
test_known_values &&			\
	test_encrypt_file &&		\
	test_decrypt_file &&		\
	test_decrypt_reference_file	\

# Return value to Makefile
exit $?

