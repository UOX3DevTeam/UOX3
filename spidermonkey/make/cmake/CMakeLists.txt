cmake_minimum_required(VERSION 3.18)
project(jscript VERSION 1.8.0 LANGUAGES C)

add_executable(jscpucfg ${PROJECT_SOURCE_DIR}/../../jscpucfg.c)

if(NOT WIN32)
		target_compile_options( jscpucfg PRIVATE
			-Wno-format-security
			-Wno-deprecated-declarations
			$<$<CONFIG:Release>:-Os>
		)
#  if(NOT EXISTS ${PROJECT_SOURCE_DIR}/../../jsautocfg.h)
    add_custom_command(
        OUTPUT ../../jsautocfg.h
        COMMAND echo ${CMAKE_CURRENT_SOURCE_DIR} && ./jscpucfg > ${CMAKE_CURRENT_SOURCE_DIR}/../../jsautocfg.h && echo "Build complete"
        DEPENDS ${PROJECT_SOURCE_DIR}/../../jsautocfg.h
        COMMENT "Generating jsautocfg.h"
    )
#  endif()
endif(NOT WIN32)

add_library(jscript STATIC)

target_sources(jscript
    PRIVATE
        ${PROJECT_SOURCE_DIR}/../../js.c
        ${PROJECT_SOURCE_DIR}/../../jsapi.c
        ${PROJECT_SOURCE_DIR}/../../jsarena.c
        ${PROJECT_SOURCE_DIR}/../../jsarray.c
        ${PROJECT_SOURCE_DIR}/../../jsatom.c
        ${PROJECT_SOURCE_DIR}/../../jsbool.c
        ${PROJECT_SOURCE_DIR}/../../jscntxt.c
        ${PROJECT_SOURCE_DIR}/../../jsdate.c
        ${PROJECT_SOURCE_DIR}/../../jsdbgapi.c
        ${PROJECT_SOURCE_DIR}/../../jsdhash.c
        ${PROJECT_SOURCE_DIR}/../../jsdtoa.c
        ${PROJECT_SOURCE_DIR}/../../jsemit.c
        ${PROJECT_SOURCE_DIR}/../../jsexn.c
        ${PROJECT_SOURCE_DIR}/../../jsfile.c
        ${PROJECT_SOURCE_DIR}/../../jsfun.c
        ${PROJECT_SOURCE_DIR}/../../jsgc.c
        ${PROJECT_SOURCE_DIR}/../../jshash.c
        ${PROJECT_SOURCE_DIR}/../../jsinterp.c
        ${PROJECT_SOURCE_DIR}/../../jsinvoke.c
        ${PROJECT_SOURCE_DIR}/../../jsiter.c
        ${PROJECT_SOURCE_DIR}/../../jslock.c
        ${PROJECT_SOURCE_DIR}/../../jslog2.c
        ${PROJECT_SOURCE_DIR}/../../jslong.c
        ${PROJECT_SOURCE_DIR}/../../jsmath.c
        ${PROJECT_SOURCE_DIR}/../../jsnum.c
        ${PROJECT_SOURCE_DIR}/../../jsobj.c
        ${PROJECT_SOURCE_DIR}/../../jsopcode.c
        ${PROJECT_SOURCE_DIR}/../../jsparse.c
        ${PROJECT_SOURCE_DIR}/../../jsprf.c
        ${PROJECT_SOURCE_DIR}/../../jsregexp.c
        ${PROJECT_SOURCE_DIR}/../../jsscan.c
        ${PROJECT_SOURCE_DIR}/../../jsscope.c
        ${PROJECT_SOURCE_DIR}/../../jsscript.c
        ${PROJECT_SOURCE_DIR}/../../jsstr.c
        ${PROJECT_SOURCE_DIR}/../../jsutil.c
        ${PROJECT_SOURCE_DIR}/../../jsxdrapi.c
        ${PROJECT_SOURCE_DIR}/../../jsxml.c
        ${PROJECT_SOURCE_DIR}/../../prmjtime.c
        ${PROJECT_SOURCE_DIR}/../../jsapi.h
        ${PROJECT_SOURCE_DIR}/../../jsarena.h
        ${PROJECT_SOURCE_DIR}/../../jsarray.h
        ${PROJECT_SOURCE_DIR}/../../jsatom.h
        ${PROJECT_SOURCE_DIR}/../../jsbool.h
        ${PROJECT_SOURCE_DIR}/../../jscntxt.h
        ${PROJECT_SOURCE_DIR}/../../jsdate.h
        ${PROJECT_SOURCE_DIR}/../../jsdbgapi.h
        ${PROJECT_SOURCE_DIR}/../../jsdhash.h
        ${PROJECT_SOURCE_DIR}/../../jsdtoa.h
        ${PROJECT_SOURCE_DIR}/../../jsemit.h
        ${PROJECT_SOURCE_DIR}/../../jsexn.h
        ${PROJECT_SOURCE_DIR}/../../jsfun.h
        ${PROJECT_SOURCE_DIR}/../../jsgc.h
        ${PROJECT_SOURCE_DIR}/../../jshash.h
        ${PROJECT_SOURCE_DIR}/../../jsinterp.h
        ${PROJECT_SOURCE_DIR}/../../jsiter.h
        ${PROJECT_SOURCE_DIR}/../../jslock.h
        ${PROJECT_SOURCE_DIR}/../../jslong.h
        ${PROJECT_SOURCE_DIR}/../../jsmath.h
        ${PROJECT_SOURCE_DIR}/../../jsnum.h
        ${PROJECT_SOURCE_DIR}/../../jsobj.h
        ${PROJECT_SOURCE_DIR}/../../jsopcode.h
        ${PROJECT_SOURCE_DIR}/../../jsparse.h
        ${PROJECT_SOURCE_DIR}/../../jsprf.h
        ${PROJECT_SOURCE_DIR}/../../jsprvtd.h
        ${PROJECT_SOURCE_DIR}/../../jspubtd.h
        ${PROJECT_SOURCE_DIR}/../../jsregexp.h
        ${PROJECT_SOURCE_DIR}/../../jsscan.h
        ${PROJECT_SOURCE_DIR}/../../jsscope.h
        ${PROJECT_SOURCE_DIR}/../../jsscript.h
        ${PROJECT_SOURCE_DIR}/../../jsstddef.h
        ${PROJECT_SOURCE_DIR}/../../jsstr.h
        ${PROJECT_SOURCE_DIR}/../../jstypes.h
        ${PROJECT_SOURCE_DIR}/../../jsutil.h
        ${PROJECT_SOURCE_DIR}/../../jsxdrapi.h
        ${PROJECT_SOURCE_DIR}/../../jsxml.h
        ${PROJECT_SOURCE_DIR}/../../prmjtime.h
        ${PROJECT_SOURCE_DIR}/../../jsopcode.tbl
)

# We only want this files for Unix platforms
if( NOT WIN32 )
    target_sources(jscript PRIVATE
        ${PROJECT_SOURCE_DIR}/../../jsautocfg.h
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
		/wd4090
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
		-Wno-attributes
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



