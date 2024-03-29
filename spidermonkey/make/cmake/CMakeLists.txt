cmake_minimum_required(VERSION 3.18)
project(jscript VERSION 1.8.0 LANGUAGES C)

add_executable(jscpucfg ../../jscpucfg.c)

if(NOT WIN32)
		target_compile_options( jscpucfg PRIVATE
			-Wno-format-security
			-Wno-deprecated-declarations
			$<$<CONFIG:Release>:-Os>
		)

    add_custom_command(
        OUTPUT ../../jsautocfg.h
        COMMAND echo ${CMAKE_CURRENT_SOURCE_DIR} && ./jscpucfg > ${CMAKE_CURRENT_SOURCE_DIR}/../../jsautocfg.h && echo "Build complete"
        DEPENDS jscpucfg
        COMMENT "Generating jsautocfg.h"
    )
endif(NOT WIN32)

add_library(jscript STATIC)

target_sources(jscript
    PRIVATE
        ../../js.c
        ../../jsapi.c
        ../../jsarena.c
        ../../jsarray.c
        ../../jsatom.c
        ../../jsbool.c
        ../../jscntxt.c
        ../../jsdate.c
        ../../jsdbgapi.c
        ../../jsdhash.c
        ../../jsdtoa.c
        ../../jsemit.c
        ../../jsexn.c
        ../../jsfile.c
        ../../jsfun.c
        ../../jsgc.c
        ../../jshash.c
        ../../jsinterp.c
        ../../jsinvoke.c
        ../../jsiter.c
        ../../jslock.c
        ../../jslog2.c
        ../../jslong.c
        ../../jsmath.c
        ../../jsnum.c
        ../../jsobj.c
        ../../jsopcode.c
        ../../jsparse.c
        ../../jsprf.c
        ../../jsregexp.c
        ../../jsscan.c
        ../../jsscope.c
        ../../jsscript.c
        ../../jsstr.c
        ../../jsutil.c
        ../../jsxdrapi.c
        ../../jsxml.c
        ../../prmjtime.c
        ../../jsapi.h
        ../../jsarena.h
        ../../jsarray.h
        ../../jsatom.h
        ../../jsbool.h
        ../../jscntxt.h
        ../../jsdate.h
        ../../jsdbgapi.h
        ../../jsdhash.h
        ../../jsdtoa.h
        ../../jsemit.h
        ../../jsexn.h
        ../../jsfun.h
        ../../jsgc.h
        ../../jshash.h
        ../../jsinterp.h
        ../../jsiter.h
        ../../jslock.h
        ../../jslong.h
        ../../jsmath.h
        ../../jsnum.h
        ../../jsobj.h
        ../../jsopcode.h
        ../../jsparse.h
        ../../jsprf.h
        ../../jsprvtd.h
        ../../jspubtd.h
        ../../jsregexp.h
        ../../jsscan.h
        ../../jsscope.h
        ../../jsscript.h
        ../../jsstddef.h
        ../../jsstr.h
        ../../jstypes.h
        ../../jsutil.h
        ../../jsxdrapi.h
        ../../jsxml.h
        ../../prmjtime.h
        ../../jsopcode.tbl
)

# We only want this files for Unix platforms
if( NOT WIN32 )
    target_sources(jscript PRIVATE
        ../../jsautocfg.h
    )
endif()

if (WIN32)

	target_compile_definitions( jscript PRIVATE
		_LIB
		_CRT_SECURE_NO_DEPRECATE
		_CRT_NONSTDC_NO_DEPRECATE
		_WINDOWS
		_X86_=1
		EXPORT_JS_API
		JSFILE
		XP_WIN
	)
	
	target_compile_options( jscript PRIVATE
		/W3
		/sdl-
		/wd4244 
		/wd4267 
		/wd4047
		/wd4146
		/wd4334
		/wd4311
		/WX-
		$<$<CONFIG:Release>:/O2>
	)
# *************************************************************************
# The libraries we need
# *************************************************************************
target_link_libraries(jscript PRIVATE
	$<$<PLATFORM_ID:Windows>:winmm>
)

	
else()

	target_compile_definitions( jscript PRIVATE
		XP_UNIX
		EXPORT_JS_API
		HAVE_VA_LIST_AS_ARRAY
		SVR4
		SYSV
		POSIX_SOURCE
		_DEFAULT_SOURCE
		HAVE_LOCALTIME_R
	)
	
	target_compile_options( jscript PRIVATE
		-Wno-shift-negative-value
		-Wno-implicit-function-declaration
		-Wno-strict-prototypes
		-Wno-logical-not-parentheses
		-Wno-pointer-to-int-cast
		-Wno-switch
		-Wno-incompatible-pointer-types
		-Wno-deprecated-declarations
		-Wno-format-security
	)
	if (APPLE)
		target_compile_options( jscript PRIVATE
			-Wno-non-literal-null-conversion
			-Wno-tautological-constant-out-of-range-compare
			-Wno-tautological-constant-compare
			$<$<CONFIG:Release>:-Os>
		)
	elseif (UNIX AND NOT APPLE AND NOT LINUX)
		target_compile_options( jscript PRIVATE
  			$<$<CONFIG:Release>:-Os>
     		)
	else()
		target_compile_options(jscript PRIVATE
			-MMD
			$<$<CONFIG:Release>:-Os>
		)
	endif()
endif(WIN32)



