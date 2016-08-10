#!/bin/sh

### Constants
scenario_valgrind_min=1

decrypted_reference_file="attempt_reference.txt"

scenario_cmd() {
	echo $password | $val_cmd $bindir/scrypt dec -P \
		$scriptdir/test_scrypt_good.enc $out/$decrypted_reference_file
	cmd_retval=$?
	echo "$cmd_retval"
}

scenario_check() {
	retval=$1

	# The decrypted reference file should match the reference.
	if ! cmp -s $out/$decrypted_reference_file \
	    $scriptdir/test_scrypt.good; then
		retval=1
	fi

	echo "$retval"
}
