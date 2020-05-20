# CHECK_POSIX_SH
# -------------
AC_DEFUN([CHECK_POSIX_SH], [
	# Get the default value of PATH which is specified to find the
	# standard POSIX utilities.
	POSIX_PATH=`command -p getconf PATH`
	AS_IF([test "x${POSIX_PATH}" = "x"],
	    AC_MSG_ERROR(["cannot get the default PATH"]))
	# Get the path of sh within POSIX_PATH.
	AC_PATH_PROG([POSIX_SH], [sh], [""], [${POSIX_PATH}])
	AS_IF([test "x${POSIX_SH}" = "x"],
	    AC_MSG_ERROR(["cannot find a POSIX shell"]))
])# CHECK_POSIX_SH

# CHECK_LIBCPERCIVA_POSIX
# -----------------------
AC_DEFUN([CHECK_LIBCPERCIVA_POSIX], [
	AC_REQUIRE([CHECK_POSIX_SH])
	AC_MSG_NOTICE([checking POSIX compatibility...])
	LIBCPERCIVA_DIR="$1"
	LDADD_POSIX=`export CC="${CC}"; ${POSIX_SH} ${LIBCPERCIVA_DIR}/POSIX/posix-l.sh "$PATH"`
	CFLAGS_POSIX=`export CC="${CC}"; ${POSIX_SH} ${LIBCPERCIVA_DIR}/POSIX/posix-cflags.sh "$PATH"`
	AC_SUBST([LDADD_POSIX])
	AC_SUBST([CFLAGS_POSIX])
	AC_MSG_RESULT([... done checking POSIX compatibility])
])# CHECK_LIBCPERCIVA_POSIX
