#!/bin/sh

test_scrypt_binary=$1
test_scrypt_good=$2

if [ -z $test_scrypt_binary ] || [ -z $test_scrypt_good ]; then
	echo -n "Test binary or good file not given: "
	echo "Using default values for in-source-tree build."
	test_scrypt_binary="./test_scrypt"
	test_scrypt_good="./test_scrypt.good"
fi

$test_scrypt_binary > test_scrypt.log
if cmp -s test_scrypt.log $test_scrypt_good; then
	rm test_scrypt.log
	echo "PASSED!"
else
	echo "FAILED!"
fi
