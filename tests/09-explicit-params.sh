#!/bin/sh

### Constants
c_valgrind_min=1
reference_file="${scriptdir}/verify-strings/test_scrypt.good"
encrypted_file="${s_basename}-reference.enc"
stderr="${s_basename}-reference.stderr"
encrypted_file_bad="${s_basename}-reference-bad.enc"
stderr_bad="${s_basename}-reference-bad.stderr"

scenario_cmd() {
	# Encrypt with manually-specified N, r, p.
	setup_check_variables "scrypt enc Nrp"
	echo ${password} | ${c_valgrind_cmd} ${bindir}/scrypt		\
	    enc -v --logN 12 -r 2 -p 3					\
	    --passphrase dev:stdin-once					\
	    ${reference_file} ${encrypted_file}				\
	    2> ${stderr}
	echo $? > ${c_exitfile}

	# Check that the options were used.
	setup_check_variables "scrypt enc Nrp output N"
	grep -q "N = 4096" ${stderr}
	echo $? > ${c_exitfile}

	setup_check_variables "scrypt enc Nrp output r"
	grep -q "r = 2" ${stderr}
	echo $? > ${c_exitfile}

	setup_check_variables "scrypt enc Nrp output p"
	grep -q "p = 3" ${stderr}
	echo $? > ${c_exitfile}

	# Try to encrypt with badly-specified N, r, p; should fail.
	setup_check_variables "scrypt enc Nrp bad"
	echo ${password} | ${c_valgrind_cmd} ${bindir}/scrypt		\
	    enc -v --logN 2 -r 0 -p 0					\
	    --passphrase dev:stdin-once					\
	    ${reference_file} ${encrypted_file_bad}			\
	    2> ${stderr_bad}
	expected_exitcode 1 $? > ${c_exitfile}

	# Check that we got an error.
	setup_check_variables "scrypt enc Nrp bad output"
	grep -q "\--logN must be between 10 and 40 (inclusive)" ${stderr_bad}
	echo $? > ${c_exitfile}

	# Check that we can't partially set explicit parameters.
	setup_check_variables "scrypt enc --logN only"
	${c_valgrind_cmd} ${bindir}/scrypt				\
	    enc --logN 12 ${reference_file} 2>&1 |			\
	    grep -q "If --logN is set, -r and -p must also be set"
	echo $? > ${c_exitfile}

	setup_check_variables "scrypt enc -r only"
	${c_valgrind_cmd} ${bindir}/scrypt				\
	    enc -r 12 ${reference_file} 2>&1 |				\
	    grep -q "If -r is set, --logN and -p must also be set"
	echo $? > ${c_exitfile}

	setup_check_variables "scrypt enc -p only"
	${c_valgrind_cmd} ${bindir}/scrypt				\
	    enc -p 12 ${reference_file} 2>&1 |				\
	    grep -q "If -p is set, --logN and -r must also be set"
	echo $? > ${c_exitfile}
}
