#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(UOX3, 0.98-3.0b, www.uox3.org/forum/)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([source/uox3.cpp])
AC_CONFIG_HEADER([source/config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create])
UOX3_LIBS="-lpthread"
jse=0
if test x$UOX3_JSLIB = x; then
	UOX3_JSLIB="js"
fi
AC_CHECK_LIB([$UOX3_JSLIB], [JS_GetRuntime], [jse=1; UOX3_LIBS="$UOX3_LIBS -l$UOX3_JSLIB"])
if test $jse = 0; then
	# Checks for mozilla javascript library, debian name is libsmjs, different from others
	UOX3_JSLIB="smjs"
	AC_CHECK_LIB([$UOX3_JSLIB], [JS_GetRuntime], [jse=1; UOX3_LIBS="$UOX3_LIBS -l$UOX3_JSLIB"])
	if test $jse = 0; then
		# check if there is javascript library, ask people to install it first
		AC_MSG_ERROR([Couldn't find the spidermonkey javascript library, please read README.JS])
	fi
fi
# make sure include JS header, debian put it on very different subdirectory
for i in "/usr/local/include/$UOX3_JSLIB" "/usr/include/$UOX3_JSLIB" ; do
	AC_CHECK_FILE([$i/jsapi.h], [jse=1], [jse=0])
	if test $jse = 1; then
		CFLAGS="$CFLAGS -I$i"
		CXXFLAGS="$CXXFLAGS -I$i"
#		CPPFLAGS="$CPPFLAGS -I$i"		
		break
	fi
	# hack for distro which like to put on version, such as old Mandrake
	ver_num="1.5"
	dir_val="$i$ver_num"
	AC_CHECK_FILE([$dir_val/jsapi.h], [jse=1], [jse=0])
	if test $jse = 1; then
		CFLAGS="$CFLAGS -I$dir_val"
		CXXFLAGS="$CXXFLAGS -I$dir_val"
#		CPPFLAGS="$CPPFLAGS -I$dir_val"	
		break
	fi	
done
AC_SUBST(UOX3_LIBS)


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
AC_FUNC_SETVBUF_REVERSED
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([floor ftime getcwd gethostbyaddr gethostbyname gettimeofday inet_ntoa memset mkdir pow select socket sqrt strcasecmp strchr strerror strncasecmp strstr strtol])

AC_CONFIG_FILES(Makefile source/Makefile)
AC_OUTPUT
# ([[chmod +x source/uox3]])
