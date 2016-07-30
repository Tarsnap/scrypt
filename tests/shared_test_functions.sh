#!/bin/sh

# A non-zero value unlikely to be used as an exit code by the programs being
# tested.
valgrind_exit_code=108

## check_optional_valgrind ():
# Return a $USE_VALGRIND variable defined; if it was previously defined and
# was greater than 0, then check that valgrind is available in the $PATH.
check_optional_valgrind() {
	if [ -z "$USE_VALGRIND" ]; then
		USE_VALGRIND=0
	fi
	if [ "$USE_VALGRIND" -gt 0 ]; then
		# Look for valgrind in $PATH.
		if ! command -v valgrind >/dev/null 2>&1; then
			echo "valgrind not detected"
			exit 1
		fi
	fi
	echo "$USE_VALGRIND"
}

## setup_valgrind_cmd (val_logfilename, valgrind_disable_number=0):
# Return a valid valgrind command if $USE_VALGRIND is higher than
# $valgrind_disable_number; otherwise, returns an empty string.
setup_valgrind_cmd() {
	val_logfilename=$1
	# The user-specified $USE_VALGRIND number must be higher than
	# $valgrind_disable_number; this allows us to specify certain tests as
	# being "normal memory usage" or "lots of memory required; most people
	# won't want to run valgrind on this".
	valgrind_disable_number=${2:-0}

	# Set up the valgrind command (if requested).  Using --error-exitcode
	# means that if there is a serious problem (such that scrypt calls
	# exit(1)) *and* a memory leak, the test suite reports an exit value
	# of $valgrind_exit_code.  However, if there is a serious problem but
	# no memory leak, we still receive a non-zero exit code.  The most
	# important thing is that we only receive an exit code of 0 if both
	# the program and valgrind are happy.
	if [ "$USE_VALGRIND" -gt "$valgrind_disable_number" ]; then
		valgrind_cmd="valgrind \
			--log-file=$val_logfilename \
			--leak-check=full --show-leak-kinds=all \
			--errors-for-leak-kinds=all \
			--error-exitcode=$valgrind_exit_code "
	else
		valgrind_cmd=""
	fi

	# Return command to calling function.
	echo "$valgrind_cmd"
}

## notify_success_or_fail (retval, (val_retval, val_logfilename)*):
# Print "PASSED!" or "FAILED!" based on $retval.  In a failure condition,
# examine pairs of optional arguments; if $val_retval*k is the constant
# $valgrind_exit_code, output $val_logfilename*k to stdout, for k>=0.
notify_success_or_fail() {
	retval=$1
	shift

	if [ "$retval" -eq 0 ]; then
		echo "PASSED!"
	else
		echo "FAILED!"

		# If valgrind discovered a problem, print it.  This is
		# primarily aimed at automation in travis-CI, since we don't
		# have access to the log files.
		while [ ! "$#" -eq 0 ]; do
			val_retval=$1
			val_logfilename=$2
			if [ "$val_retval" -eq "$valgrind_exit_code" ]; then
				cat $val_logfilename
			fi
			shift
			shift
		done
	fi
}
