cmake_minimum_required(VERSION 3.18)
project(jscript VERSION 1.8.0 LANGUAGES C)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/../../../install" CACHE STRING "" FORCE)

add_executable(jscpucfg ../../jscpucfg.cpp)

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

add_executable(host_jskwgen)

target_sources(host_jskwgen
    PRIVATE
        ../../jskwgen.cpp
        ../../jsversion.h
        ../../jskeyword.tbl
)

add_custom_command(
    OUTPUT ../../jsautokw.h
    COMMAND echo ${CMAKE_CURRENT_SOURCE_DIR} && ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/host_jskwgen ${CMAKE_CURRENT_SOURCE_DIR}/../../jsautokw.h && echo "Build complete"
    DEPENDS host_jskwgen
    COMMENT "Generating jsautokw.h"
)

add_executable(host_jsoplengen)

target_sources(host_jsoplengen
    PRIVATE
        ../../jsoplengen.cpp
        ../../jsversion.h
        ../../jsopcode.tbl
)


add_custom_command(
    OUTPUT ../../jsautooplen.h
    COMMAND echo ${CMAKE_CURRENT_SOURCE_DIR} && ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/host_jsoplengen ${CMAKE_CURRENT_SOURCE_DIR}/../../jsautooplen.h && echo "Build complete"
    DEPENDS host_jsoplengen
    COMMENT "Generating jsautooplen.h"
)

add_library(jscript STATIC)

target_sources(jscript
    PRIVATE
        ../../jsapi.cpp
        ../../jsarena.cpp
        ../../jsarray.cpp
        ../../jsatom.cpp
        ../../jsbool.cpp
        ../../jsbuiltins.cpp
        ../../jscntxt.cpp
        ../../jsdate.cpp
        ../../jsdbgapi.cpp
        ../../jsdhash.cpp
        ../../jsdtoa.cpp
        ../../jsemit.cpp
        ../../jsexn.cpp
        ../../jsfun.cpp
        ../../jsgc.cpp
        ../../jsgcchunk.cpp
        ../../jshash.cpp
        ../../jsinterp.cpp
        ../../jsinvoke.cpp
        ../../jsiter.cpp
        ../../jslock.cpp
        ../../jslog2.cpp
        ../../jsmath.cpp
        ../../jsnativestack.cpp
        ../../jsnum.cpp
        ../../jsobj.cpp
        ../../json.cpp
        ../../jsopcode.cpp
        ../../jsoplengen.cpp
        ../../jsparse.cpp
        ../../jsprf.cpp
        ../../jspropertycache.cpp
        ../../jspropertytree.cpp
        ../../jsproxy.cpp
        ../../jsregexp.cpp
        ../../jsscan.cpp
        ../../jsscope.cpp
        ../../jsscript.cpp
        ../../jsstr.cpp
        ../../jstask.cpp
        ../../jstracer.cpp
        ../../jstypedarray.cpp
        ../../jsutil.cpp
        ../../jswrapper.cpp
        ../../jsxdrapi.cpp
        ../../jsxml.cpp
        ../../prmjtime.cpp
        ../../jsapi.h
        ../../jsarena.h
        ../../jsarray.h
        ../../jsatom.h
        ../../jsatominlines.h
        ../../jsautokw.h
        ../../jsautooplen.h
        ../../jsbit.h
        ../../jsbool.h
        ../../jsbuiltins.h
        ../../jsclist.h
        ../../jscntxt.h
        ../../jscntxtinlines.h
        ../../jscompat.h
        ../../jsdate.h
        ../../jsdbgapi.h
        ../../jsdhash.h
        ../../jsdtoa.h
        ../../jsdtracef.h
        ../../jsemit.h
        ../../jsexn.h
        ../../jsfun.h
        ../../jsgc.h
        ../../jsgcchunk.h
        ../../jshash.h
        ../../jshashtable.h
        ../../jsinterp.h
        ../../jsinttypes.h
        ../../jsiter.h
        ../../jslibmath.h
        ../../jslock.h
        ../../jslong.h
        ../../jsmath.h
        ../../jsnativestack.h
        ../../jsnum.h
        ../../jsobj.h
        ../../jsobjinlines.h
        ../../json.h
        ../../jsopcode.h
        ../../jsparse.h
        ../../jsprf.h
        ../../jspropertycache.h
        ../../jspropertycacheinlines.h
        ../../jspropertytree.h
        ../../jsproxy.h
        ../../jsprvtd.h
        ../../jspubtd.h
        ../../jsregexp.h
        ../../jsscan.h
        ../../jsscope.h
        ../../jsscopeinlines.h
        ../../jsscript.h
        ../../jsscriptinlines.h
        ../../jsstaticcheck.h
        ../../jsstr.h
        ../../jsstrinlines.h
        ../../jstask.h
        ../../jstl.h
        ../../jstracer.h
        ../../jstypedarray.h
        ../../jstypes.h
        ../../jsutil.h
        ../../jsversion.h
        ../../jswrapper.h
        ../../jsxdrapi.h
        ../../jsxml.h
        ../../prmjtime.h
        ../../builtins.tbl
        ../../jitstats.tbl
        ../../jskeyword.tbl
        ../../jsopcode.tbl
        ../../jsproto.tbl
        ../../jsreops.tbl
)

# We only want this files for Unix platforms
if( NOT WIN32 )
    target_sources(jscript PRIVATE
        ../../jsautocfg.h
    )
endif()

if (WIN32)

    # See Configure.in
    target_compile_definitions( jscript PRIVATE
        _LIB
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_DEPRECATE
        _WINDOWS
        EXPORT_JS_API
        JSFILE
        XP_WIN
        XP_WIN32
        WIN32
        _WIN32
        NO_X11
        $<$<EQUAL:${CMAKE_SIZEOF_VOID_P},4>:_X86_>
        HW_THREADS
        STDC_HEADERS
        WIN32_LEAN_AND_MEAN
        HAVE_SNPRINTF
        ENABLE_TRACEJIT=1
        FEATURE_NANOJIT
        JS_TRACER
        $<$<EQUAL:${CMAKE_SIZEOF_VOID_P},4>:NANOJIT_ARCH=i386>
        $<$<EQUAL:${CMAKE_SIZEOF_VOID_P},4>:AVMPLUS_IA32>
        $<$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>:NANOJIT_ARCH=X64>
        $<$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>:AVMPLUS_AMD64>
        $<$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>:AVMPLUS_64BIT>
        $<$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>:_WIN64>
        $<$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>:_AMD64_>
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
    $<$<PLATFORM_ID:Windows>:wsock32>
    $<$<PLATFORM_ID:Windows>:advapi32>
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
        AVMPLUS_UNIX
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
        target_compile_definitions( jscript PRIVATE
            XP_MACOSX
            NO_X11
        )

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
        target_compile_definitions( jscript PRIVATE
            AVMPLUS_LINUX
        )
        target_compile_options(jscript PRIVATE
            -MMD
            $<$<CONFIG:Release>:-Os>
        )
    endif()
endif(WIN32)