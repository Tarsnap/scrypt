#!/bin/sh

### Constants
scenario_need_valgrind=0

encrypted_file="attempt.enc"

scenario_cmd() {
	echo $password | $val_cmd $bindir/scrypt enc -P \
		$scriptdir/test_scrypt.good $out/$encrypted_file
	cmd_retval=$?

	echo "$cmd_retval"
}

scenario_check() {
	retval=$1

	# The encrypted file should be different from the original file.
	# We cannot check against the "reference" encrypted file, because
	# encrypted files include random salt.  If successful, don't delete
	# $encrypted_file yet; we need it for the next test.
	if cmp -s $out/$encrypted_file $scriptdir/test_scrypt.good; then
		retval=1
	fi

	echo "$retval"
}
