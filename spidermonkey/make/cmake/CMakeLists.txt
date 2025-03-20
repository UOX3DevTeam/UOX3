cmake_minimum_required(VERSION 3.18)
project(jscript VERSION 1.8.0 LANGUAGES C)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/../../../install" CACHE STRING "" FORCE)

add_executable(jscpucfg ../../jscpucfg.cpp)

target_compile_options( jscpucfg PRIVATE
    $<$<CONFIG:Release>:-Os>
)

set_target_properties( jscpucfg PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/" )

add_custom_command(
    OUTPUT ../../jsautocfg.h
    COMMAND echo ${CMAKE_CURRENT_SOURCE_DIR} && ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/jscpucfg > ${CMAKE_CURRENT_SOURCE_DIR}/../../jsautocfg.h && echo "Build complete"
    DEPENDS jscpucfg
    COMMENT "Generating jsautocfg.h"
)

add_executable(host_jskwgen)

set_target_properties( host_jskwgen PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/" )

target_sources(host_jskwgen PRIVATE
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

set_target_properties( host_jsoplengen PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/" )

target_sources(host_jsoplengen  PRIVATE
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
        ../../jstypedarray.cpp
        ../../jsutil.cpp
        ../../jswrapper.cpp
        ../../jsxdrapi.cpp
        ../../jsxml.cpp
        ../../prmjtime.cpp
# TraceJit
        ../../tracejit/Writer.cpp
# Headers
        ../../jsapi.h
        ../../jsarena.h
        ../../jsarray.h
        ../../jsatom.h
        ../../jsatominlines.h
        ../../jsautocfg.h
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
# TraceJit
        ../../tracejit/Writer.h
# OpCode tables
        ../../jitstats.tbl
        ../../jskeyword.tbl
        ../../jsopcode.tbl
        ../../jsproto.tbl
        ../../jsreops.tbl
)

if ((WIN32 OR LINUX) AND NOT APPLE)

target_sources(jscript
    PRIVATE

    # NanoJit
    ../../jsbuiltins.cpp
    ../../jstracer.cpp
    ../../nanojit/Assembler.cpp
    ../../nanojit/Allocator.cpp
    ../../nanojit/CodeAlloc.cpp
    ../../nanojit/Containers.cpp
    ../../nanojit/Fragmento.cpp
    ../../nanojit/LIR.cpp
    ../../nanojit/njconfig.cpp
    ../../nanojit/RegAlloc.cpp
    ../../nanojit/avmplus.cpp
    ../../nanojit/NativeX64.cpp
    ../../nanojit/VMPI.cpp
    ../../nanojit/Assembler.h
    ../../nanojit/Allocator.h
    ../../nanojit/CodeAlloc.h
    ../../nanojit/Containers.h
    ../../nanojit/LIR.h
    ../../nanojit/avmplus.h
    ../../nanojit/Fragmento.h
    ../../nanojit/Native.h
    ../../nanojit/NativeX64.h
    ../../nanojit/njconfig.h
    ../../nanojit/RegAlloc.h
    ../../nanojit/nanojit.h
    ../../nanojit/VMPI.h
)
endif()

target_compile_definitions( jscript PRIVATE
    _LIB
    EXPORT_JS_API
    HW_THREADS
    JSFILE
    STDC_HEADERS
)


if (WIN32)


    # See Configure.in
    target_compile_definitions( jscript PRIVATE
        AVMPLUS_64BIT
        ENABLE_TRACEJIT=1
        FEATURE_NANOJIT
        JS_TRACER
        _AMD64_
        _CRT_NONSTDC_NO_DEPRECATE
        _CRT_SECURE_NO_DEPRECATE
        _WIN32
        _WIN64
        _WINDOWS
        AVMPLUS_AMD64
        AVMPLUS_WIN32
        HAVE_SNPRINTF
        NANOJIT_ARCH=X64
        NO_X11
        WIN32
        WIN32_LEAN_AND_MEAN
        XP_WIN
        XP_WIN32
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
        HAVE_VA_LIST_AS_ARRAY
        SVR4
        SYSV
        POSIX_SOURCE
        _DEFAULT_SOURCE
        HAVE_LOCALTIME_R
    )

    target_compile_options( jscript PRIVATE
        -Wno-shift-negative-value
        -Wno-logical-not-parentheses
        -Wno-switch
        -Wno-deprecated-declarations
        -Wno-format-security
    )
    if (APPLE)
        target_compile_definitions( jscript PRIVATE
            XP_MACOSX
            NO_X11
            USE_PTHREADS=1
            UNIX_ASYNC_DNS
            _BSD_SOURCE
        )

        target_compile_options( jscript PRIVATE
            -Wno-non-literal-null-conversion
            -Wno-tautological-constant-out-of-range-compare
            -Wno-tautological-constant-compare
            -Wno-narrowing
            -Wno-register
            -fpermissive
            $<$<CONFIG:Release>:-Os>
        )
    elseif (UNIX AND NOT APPLE AND NOT LINUX)
        target_compile_options( jscript PRIVATE
              $<$<CONFIG:Release>:-Os>
             )
    else()
        target_include_directories(jscript PRIVATE 
            ${PROJECT_SOURCE_DIR}/../../nanojit 
            ${PROJECT_SOURCE_DIR}/../../tracejit 
            ${PROJECT_SOURCE_DIR}/../../yarr 
            ${PROJECT_SOURCE_DIR}/../..
        )
        target_compile_definitions(jscript PRIVATE
            AVMPLUS_64BIT
            AVMPLUS_UNIX
            ENABLE_TRACEJIT=1
            FEATURE_NANOJIT
            JS_TRACER
            NANOJIT_ARCH=X64
            AVMPLUS_LINUX
            AVMPLUS_AMD64
            AVMPLUS_64BIT
            )
        target_compile_options(jscript PRIVATE
            -fpermissive
            -Wno-narrowing
            -MMD
            $<$<CONFIG:Release>:-Os>
        )
    endif()
endif(WIN32)