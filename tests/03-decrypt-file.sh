#!/bin/sh

### Constants
scenario_valgrind_min=1

decrypted_file="attempt.txt"

# This requires the file created in 02-encrypt-file.sh
scenario_cmd() {
	echo $password | $val_cmd $bindir/scrypt dec -P \
		$out/$encrypted_file $out/$decrypted_file
	cmd_retval=$?

	echo "$cmd_retval"
}

scenario_check() {
	retval=$1

	# The decrypted file should match the reference.
	if ! cmp -s $out/$decrypted_file $scriptdir/test_scrypt.good; then
		retval=1
	fi

	echo "$retval"
}
