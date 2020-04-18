#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(UOX3, 0.99-2g, https://www.uox3.org/forums/)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([uox3.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET

CXXFLAGS="-I../spidermonkey --std=c++17"

AC_ARG_ENABLE(debug,   [  --enable-debug          compile with debugging support],,enable_debug=no)

if test "$enable_debug" = yes ; then
	CXXFLAGS="$CXXFLAGS -g"
	AC_DEFINE(DEBUG, 1, [Define if debugging is enabled.])
else
	CXXFLAGS="$CXXFLAGS -O"
	AC_DEFINE(DEBUG, 0, [Define if debugging is enabled.])
fi


# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h malloc.h netdb.h netinet/in.h stddef.h string.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h sys/timeb.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_MODE_T
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_ERROR_AT_LINE
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MEMCMP
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([floor ftime getcwd gethostbyaddr gethostbyname gettimeofday inet_ntoa memset mkdir pow select socket sqrt strcasecmp strchr strerror strncasecmp strstr strtol])

AC_CONFIG_FILES(Makefile)
AC_OUTPUT
# ([[chmod +x ../source/uox3]])
