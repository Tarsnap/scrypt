#!/bin/sh

### Constants
c_valgrind_min=1
reference_file="${scriptdir}/verify-strings/test_scrypt.good"
large_file="${s_basename}-large.txt"
large_enc="${s_basename}-large.enc"
stderr_enc="${s_basename}-enc.stderr"
stderr_dec="${s_basename}-dec.stderr"

# Explicit parameters keep the key derivation cheap, and -f skips the
# resource checks, which would otherwise measure the CPU speed every time.
fast_params="-f --logN 10 -r 1 -p 1"

scenario_cmd() {
	# Build an input which is larger than stdio's buffer, so that a write
	# failure happens inside the encrypt and decrypt loops rather than
	# when the output is flushed at the end.
	i=0
	while [ "${i}" -lt 128 ]; do
		cat "${reference_file}"
		i=$((i + 1))
	done > "${large_file}"

	# Encrypting it to a writable file must work.  This also gives us a
	# ciphertext which is larger than stdio's buffer.
	setup_check "scrypt enc large file"
	echo "${password}" | ${c_valgrind_cmd} "${bindir}/scrypt"	\
	    enc ${fast_params} --passphrase dev:stdin-once		\
	    "${large_file}" "${large_enc}"
	echo $? > "${c_exitfile}"

	# The write-error paths need an output file which never accepts data.
	# /dev/full is not portable, so stop here if we don't have it.
	if ! [ -c /dev/full ] || ! [ -w /dev/full ]; then
		return
	fi

	setup_check "scrypt enc write error"
	(
		echo "${password}" | ${c_valgrind_cmd} "${bindir}/scrypt" \
		    enc ${fast_params} --passphrase dev:stdin-once	\
		    "${large_file}" /dev/full				\
		    2> "${stderr_enc}"
		expected_exitcode 1 $? > "${c_exitfile}"
	)

	setup_check "scrypt enc write error message"
	grep -q "scrypt: Error writing file: /dev/full" "${stderr_enc}"
	echo $? > "${c_exitfile}"

	setup_check "scrypt dec write error"
	(
		echo "${password}" | ${c_valgrind_cmd} "${bindir}/scrypt" \
		    dec -f --passphrase dev:stdin-once			\
		    "${large_enc}" /dev/full				\
		    2> "${stderr_dec}"
		expected_exitcode 1 $? > "${c_exitfile}"
	)

	setup_check "scrypt dec write error message"
	grep -q "scrypt: Error writing file: /dev/full" "${stderr_dec}"
	echo $? > "${c_exitfile}"
}
