# CHECK_POSIX_SH
# -------------
AC_DEFUN([CHECK_POSIX_SH], [
	AC_MSG_CHECKING([for a POSIX shell])
	POSIX_SH=`command -v -p sh`
	AS_IF([test "x${POSIX_SH}" = "x"],
	    AC_MSG_ERROR(["cannot find a shell"]))
	AC_MSG_RESULT([${POSIX_SH}])
])# CHECK_POSIX_SH
