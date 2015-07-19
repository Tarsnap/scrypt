#!/bin/sh

./test_scrypt > test_scrypt.log
if cmp -s test_scrypt.log test_scrypt.good; then
	rm test_scrypt.log
	echo "PASSED!"
else
	echo "FAILED!"
fi
