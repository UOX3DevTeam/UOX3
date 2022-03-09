cmake_minimum_required(VERSION 3.12.3)

# This code was from SPIDERMONKEY 1.7
# Only the files used have been kept, so it not a complete
# set of the released version
# The linux/macOS jsautocfg.h file was generated on an M1 Max macos
# and a Ryzen Ubuntu WSL2 environment
project(jsengine VERSION 1.7 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
add_library(jsengine 
	STATIC
		jsapi.c
		jsarena.c
		jsarray.c
		jsatom.c
		jsbool.c
		jscntxt.c
		jsdate.c
		jsdbgapi.c
		jsdhash.c
		jsdtoa.c
		jsemit.c
		jsexn.c
		jsfun.c
		jsgc.c
		jshash.c
		jsinterp.c
		jsiter.c
		jslock.c
		jslog2.c
		jslong.c
		jsmath.c
		jsnum.c
		jsobj.c
		jsopcode.c
		jsparse.c
		jsprf.c
		jsregexp.c
		jsscan.c
		jsscope.c
		jsscript.c
		jsstr.c
		jsutil.c
		jsxdrapi.c
		jsxml.c
		prmjtime.c
)

# Dont forget this library is dependent in windows on winmm.lib
#

target_link_libraries(jsengine 
	PRIVATE
		$<$<PLATFORM_ID:Windows>:winmm.lib>
)
#
# This is setup for release only
# Note the optimization level
#
		
target_compile_options(jsengine 
	PRIVATE
		$<$<PLATFORM_ID:Windows>:/O2>
		$<$<PLATFORM_ID:Windows>:/W3>
		$<$<PLATFORM_ID:Windows>:/MT>
		$<$<PLATFORM_ID:Windows>:/wd4090>
		$<$<PLATFORM_ID:Windows>:/wd5105>
		$<$<PLATFORM_ID:Windows>:/wd4047>
		$<$<PLATFORM_ID:Windows>:/wd4244>
		$<$<PLATFORM_ID:Windows>:/wd4267>
		$<$<PLATFORM_ID:Windows>:/wd4334>
		$<$<PLATFORM_ID:Windows>:/wd4267>
		$<$<PLATFORM_ID:Windows>:/wd4311>
		$<$<PLATFORM_ID:Windows>:/wd4146>

		$<$<PLATFORM_ID:Linux>:-O2>
		$<$<PLATFORM_ID:Linux>:-Wno-format>
		$<$<PLATFORM_ID:Linux>:-Wno-unused-but-set-variable>
		$<$<PLATFORM_ID:Linux>:-Wno-unused-local-typedefs>
		$<$<PLATFORM_ID:Linux>:-Wno-pointer-sign>
		$<$<PLATFORM_ID:Linux>:-Wno-switch>

		$<$<PLATFORM_ID:Darwin>:-O2>
		$<$<PLATFORM_ID:Darwin>:-Wno-format>
		$<$<PLATFORM_ID:Darwin>:-Wno-unused-local-typedefs>
		$<$<PLATFORM_ID:Darwin>:-Wno-pointer-sign>
		$<$<PLATFORM_ID:Darwin>:-Wno-switch>
		$<$<PLATFORM_ID:Darwin>:-Wno-uninitialized>
		$<$<PLATFORM_ID:Darwin>:-Wno-shift-negative-value>
		$<$<PLATFORM_ID:Darwin>:-Wno-void-pointer-to-enum-cast>
		$<$<PLATFORM_ID:Darwin>:-Wno-non-literal-null-conversion>
		$<$<PLATFORM_ID:Darwin>:-Wno-tautological-constant-out-of-range-compare>
		$<$<PLATFORM_ID:Darwin>:-Wno-logical-not-parentheses>
		
)
target_compile_definitions(jsengine
	PRIVATE
		$<$<PLATFORM_ID:Windows>:_X86_>
		$<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_DEPRECATE>
		$<$<PLATFORM_ID:Windows>:_CRT_NONSTDC_NO_DEPRECATE>
		$<$<PLATFORM_ID:Windows>:_WINDOWS>
		$<$<PLATFORM_ID:Windows>:EXPORT_JS_API>
		$<$<PLATFORM_ID:Windows>:JSFILE>
		$<$<PLATFORM_ID:Windows>:WIN32>
		$<$<PLATFORM_ID:Windows>:_LIB>
		$<$<PLATFORM_ID:Windows>:XP_WIN>
		$<$<PLATFORM_ID:Windows>:NDEBUG>

		$<$<PLATFORM_ID:Linux>:SVR4>
		$<$<PLATFORM_ID:Linux>:SYSV>
		$<$<PLATFORM_ID:Linux>:_BSD_SOURCE>
		$<$<PLATFORM_ID:Linux>:_DEFAULT_SOURCE>
		$<$<PLATFORM_ID:Linux>:POSIX_SOURCE>
		$<$<PLATFORM_ID:Linux>:HAVE_LOCALTIME_R>
		$<$<PLATFORM_ID:Linux>:HAVE_VA_COPY>
		$<$<PLATFORM_ID:Linux>:VA_COPY=va_copy>
		$<$<PLATFORM_ID:Linux>:PIC>
		$<$<PLATFORM_ID:Linux>:HAVE_VA_LIST_AS_ARRAY>
		$<$<PLATFORM_ID:Linux>:XP_UNIX>
		$<$<PLATFORM_ID:Linux>:NDEBUG>

		$<$<PLATFORM_ID:Darwin>:SVR4>
		$<$<PLATFORM_ID:Darwin>:SYSV>
		$<$<PLATFORM_ID:Darwin>:_BSD_SOURCE>
		$<$<PLATFORM_ID:Darwin>:POSIX_SOURCE>
		$<$<PLATFORM_ID:Darwin>:HAVE_VA_LIST_AS_ARRAY>
		$<$<PLATFORM_ID:Darwin>:XP_UNIX>
		$<$<PLATFORM_ID:Darwin>:NDEBUG>

)



