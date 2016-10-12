#!/bin/sh

### Constants
out="tests-output"
out_valgrind="tests-valgrind"
valgrind_suppressions="${out_valgrind}/suppressions"
valgrind_suppressions_log="${out_valgrind}/suppressions.pre"

# A non-zero value unlikely to be used as an exit code by the programs being
# tested.
valgrind_exit_code=108

## prepare_directories():
# Delete any old directories, and create new ones as necessary.  Must be run
# after check_optional_valgrind().
prepare_directories() {
	# Clean up previous directories.
	if [ -d "$out" ]; then
		rm -rf $out
	fi
	if [ -d "$out_valgrind" ]; then
		rm -rf $out_valgrind
	fi

	# Make new directories.
	mkdir $out
	if [ "$USE_VALGRIND" -gt 0 ]; then
		mkdir $out_valgrind
	fi
}


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

## ensure_valgrind_suppresssion (potential_memleaks_binary):
# Runs the ${potential_memleaks_binary} through valgrind, keeping
# track of any apparent memory leak in order to suppress reporting
# those leaks when testing other binaries.
ensure_valgrind_suppression() {
	potential_memleaks_binary=$1

	# Quit if we're not using valgrind.
	if [ ! "$USE_VALGRIND" -gt 0 ]; then
		return
	fi;
	printf "Generating valgrind suppressions... "

	# Run valgrind on the binary, sending it a "\n" so that
	# a test which uses STDIN will not wait for user input.
	printf "\n" | (valgrind --leak-check=full --show-leak-kinds=all	\
		--gen-suppressions=all					\
		--log-file=${valgrind_suppressions_log}			\
		${potential_memleaks_binary})

	# Strip out useless parts from the log file, as well as
	# removing references to the main and "pl_*" ("potential
	# loss") functions so that the suppressions can apply to
	# other binaries.
	(grep -v "^==" ${valgrind_suppressions_log} 			\
		| grep -v "   fun:pl_" -				\
		| grep -v "   fun:main" -				\
		> ${valgrind_suppressions} )

	# Clean up
	rm -f ${valgrind_suppressions_log}
	printf "done.\n"
}

## setup_valgrind_cmd (val_logfilename, valgrind_min=0):
# Return a valid valgrind command if $USE_VALGRIND is greater than or equal to
# $valgrind_min; otherwise, returns an empty string.
setup_valgrind_cmd() {
	val_logfilename=$1
	# The user-specified $USE_VALGRIND number must be higher than
	# $valgrind_min; this allows us to specify certain tests as being
	# "normal memory usage" or "lots of memory required; most people won't
	# want to run valgrind on this".
	valgrind_min=${2:-0}

	# Set up the valgrind command (if requested).  Using --error-exitcode
	# means that if there is a serious problem (such that scrypt calls
	# exit(1)) *and* a memory leak, the test suite reports an exit value
	# of $valgrind_exit_code.  However, if there is a serious problem but
	# no memory leak, we still receive a non-zero exit code.  The most
	# important thing is that we only receive an exit code of 0 if both
	# the program and valgrind are happy.
	if [ "$USE_VALGRIND" -ge "$valgrind_min" ]; then
		valgrind_cmd="valgrind \
			--log-file=$val_logfilename \
			--leak-check=full --show-leak-kinds=all \
			--errors-for-leak-kinds=all \
			--suppressions=${valgrind_suppressions} \
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

## scenario_runner (scenario_filename):
# Runs a test scenario from $scenario_filename.  That file must define:
# - scenario_valgrind_min: an integer specifying the minimum USE_VALGRIND
#       value which enables valgrind memory testing.
# - scenario_cmd: a function which runs any commands whose exit value(s) are
#       checked, and produce any desired files.
# - scenario_check: a function which checks any files produced by
#       scenario_cmd.
scenario_runner() {
	scenario_filename=$1
	basename=`basename $scenario_filename .sh`
	printf "Running test: $basename... "

	# Load variables from the scenario file.
	. $scenario_filename

	# Set up valgrind command (if requested).
	val_logfilename=$out_valgrind/$basename-val.log
	val_cmd=$( setup_valgrind_cmd $val_logfilename $scenario_valgrind_min )

	# Run actual test command.
	cmd_retval=$( scenario_cmd )

	# Check results.
	retval=$( scenario_check $cmd_retval )

	# Print PASS or FAIL, and return result.
	notify_success_or_fail $retval $cmd_retval $val_logfilename
	return "$retval"
}
