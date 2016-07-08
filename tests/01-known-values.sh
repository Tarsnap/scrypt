#!/bin/sh

### Constants
# The scenario command requires a lot of memory, so valgrind is only enabled
# if $USE_VALGRIND > 1.
scenario_need_valgrind=1

### Actual command
scenario_cmd() {
	$val_cmd $bindir/tests/test_scrypt \
		1> $out/$basename-stdout.txt
	cmd_retval=$?

	echo "$cmd_retval"
}

### Check output
scenario_check() {
	retval=$1

	# The generated values should match the known good values.
	if ! cmp -s $out/$basename-stdout.txt $scriptdir/test_scrypt.good; then
		retval=1
	fi

	echo "$retval"
}
