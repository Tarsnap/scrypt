#!/bin/sh

### Constants
c_valgrind_min=1
encrypted_reference_file="${scriptdir}/verify-strings/test_scrypt_good.enc"
bad_logN_file="${scriptdir}/verify-strings/test_scrypt_bad_logN.enc"
info_stderr="${s_basename}-info.stderr"
bad_logN_stderr="${s_basename}-bad-logN.stderr"
bad_checksum_file="${s_basename}-bad-checksum.enc"
bad_checksum_stderr="${s_basename}-bad-checksum.stderr"

wide_r_file="${s_basename}-wide-r.enc"
wide_r_stderr="${s_basename}-wide-r.stderr"
huge_n_file="${s_basename}-huge-n.enc"
huge_n_stderr="${s_basename}-huge-n.stderr"
verbose_stderr="${s_basename}-verbose.stderr"
verbose_stdout="${s_basename}-verbose.stdout"

scenario_cmd() {
	# Print the parameters of a reference file.
	setup_check "scrypt info"
	${c_valgrind_cmd} "${bindir}/scrypt"				\
	    info "${encrypted_reference_file}"				\
	    2> "${info_stderr}"
	echo $? > "${c_exitfile}"

	# The reference file was encrypted with N = 2^18, r = 8, p = 1.
	setup_check "scrypt info output Nrp"
	grep -q "N = 262144; r = 8; p = 1;" "${info_stderr}"
	echo $? > "${c_exitfile}"

	# A header claiming logN = 255 must be rejected; computing N = 2^logN
	# would otherwise be an out-of-range shift.  (This header has a valid
	# checksum, so it is not caught by the checksum test.)
	setup_check "scrypt info bad logN"
	(
		${c_valgrind_cmd} "${bindir}/scrypt"			\
		    info "${bad_logN_file}"				\
		    2> "${bad_logN_stderr}"
		expected_exitcode 1 $? > "${c_exitfile}"
	)

	setup_check "scrypt info bad logN error"
	grep -q "scrypt: Input is not valid scrypt-encrypted block"	\
	    "${bad_logN_stderr}"
	echo $? > "${c_exitfile}"

	# A header which fails its checksum must be rejected rather than
	# having its (arbitrary) contents printed.
	cp "${encrypted_reference_file}" "${bad_checksum_file}"
	printf '\001' |							\
	    dd of="${bad_checksum_file}" bs=1 seek=16 count=1		\
	    conv=notrunc 2> /dev/null

	setup_check "scrypt info bad checksum"
	(
		${c_valgrind_cmd} "${bindir}/scrypt"			\
		    info "${bad_checksum_file}"				\
		    2> "${bad_checksum_stderr}"
		expected_exitcode 1 $? > "${c_exitfile}"
	)

	setup_check "scrypt info bad checksum error"
	grep -q "scrypt: Input is not valid scrypt-encrypted block"	\
	    "${bad_checksum_stderr}"
	echo $? > "${c_exitfile}"

	# These synthetic 96-byte headers have valid public checksums, but
	# deliberately have no password authenticator or encrypted payload.
	# First use logN = 1, r = 2^25, p = 1: 128 * r overflows uint32_t.
	printf '%b' \
	    '\0163\0143\0162\0171\0160\0164\0000\0001\0002\0000\0000\0000' \
	    '\0000\0000\0000\0001\0000\0001\0002\0003\0004\0005\0006\0007' \
	    '\0010\0011\0012\0013\0014\0015\0016\0017\0020\0021\0022\0023' \
	    '\0024\0025\0026\0027\0030\0031\0032\0033\0034\0035\0036\0037' \
	    '\0320\0140\0210\0062\0003\0042\0125\0070\0164\0025\0351\0332' \
	    '\0135\0132\0037\0041\0000\0000\0000\0000\0000\0000\0000\0000' \
	    '\0000\0000\0000\0000\0000\0000\0000\0000\0000\0000\0000\0000' \
	    '\0000\0000\0000\0000\0000\0000\0000\0000\0000\0000\0000\0000' \
	    > "${wide_r_file}"

	setup_check "scrypt info wide-r header"
	${c_valgrind_cmd} "${bindir}/scrypt" info "${wide_r_file}" \
	    2> "${wide_r_stderr}"
	echo $? > "${c_exitfile}"

	setup_check "scrypt info wide-r memory estimate"
	grep -q "at least 8.5 GB of memory" "${wide_r_stderr}"
	echo $? > "${c_exitfile}"

	# logN = 63, r = 8, p = 1 also overflows a uint64_t memory product.
	# The reported lower bound must saturate, rather than wrap to zero.
	printf '%b' \
	    '\0163\0143\0162\0171\0160\0164\0000\0077\0000\0000\0000\0010' \
	    '\0000\0000\0000\0001\0000\0001\0002\0003\0004\0005\0006\0007' \
	    '\0010\0011\0012\0013\0014\0015\0016\0017\0020\0021\0022\0023' \
	    '\0024\0025\0026\0027\0030\0031\0032\0033\0034\0035\0036\0037' \
	    '\0001\0154\0251\0144\0341\0157\0156\0002\0170\0121\0217\0101' \
	    '\0241\0341\0136\0070\0000\0000\0000\0000\0000\0000\0000\0000' \
	    '\0000\0000\0000\0000\0000\0000\0000\0000\0000\0000\0000\0000' \
	    '\0000\0000\0000\0000\0000\0000\0000\0000\0000\0000\0000\0000' \
	    > "${huge_n_file}"

	setup_check "scrypt info huge-N header"
	${c_valgrind_cmd} "${bindir}/scrypt" info "${huge_n_file}" \
	    2> "${huge_n_stderr}"
	echo $? > "${c_exitfile}"

	setup_check "scrypt info huge-N memory estimate"
	grep -q "at least 18 EB of memory" "${huge_n_stderr}"
	echo $? > "${c_exitfile}"

	# Exercise the CPU estimate without deriving a key.  -M is clamped
	# to the application's minimum; these parameters still exceed it.
	# Do not use -f, which would bypass the resource limits.
	setup_check "scrypt verbose resource-limited rejection"
	(
		printf 'synthetic-test-password\n' | \
		    ${c_valgrind_cmd} "${bindir}/scrypt" dec -v \
		    -M 1B -t 0 -P "${huge_n_file}" \
		    > "${verbose_stdout}" 2> "${verbose_stderr}"
		expected_exitcode 1 $? > "${c_exitfile}"
	)

	setup_check "scrypt verbose nonzero CPU estimate"
	awk '/and will take approximately/ { if ($5 > 0) good = 1 }
	    END { exit !good }' "${verbose_stderr}"
	echo $? > "${c_exitfile}"

	setup_check "scrypt verbose resource error"
	grep -q "would require too much memory and CPU time" \
	    "${verbose_stderr}"
	echo $? > "${c_exitfile}"

	setup_check "scrypt verbose rejection has no plaintext"
	[ ! -s "${verbose_stdout}" ]
	echo $? > "${c_exitfile}"
}
