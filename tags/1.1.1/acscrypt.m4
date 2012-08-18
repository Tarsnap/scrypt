# CHECK_SYSCTL_HW_USERMEM
# -----------------------
AC_DEFUN([CHECK_SYSCTL_HW_USERMEM],
[if sysctl hw.usermem >/dev/null 2>/dev/null; then
	AC_DEFINE([HAVE_SYSCTL_HW_USERMEM], [1],
	    [Define to 1 if the OS has a hw.usermem sysctl])
fi
AC_SUBST([HAVE_SYSCTL_HW_USERMEM])
])# CHECK_SYSCTL_HW_USERMEM
