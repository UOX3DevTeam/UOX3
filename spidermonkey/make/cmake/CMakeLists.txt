cmake_minimum_required(VERSION 3.18)
project(jscript VERSION 1.8.0 LANGUAGES CXX)

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
        ../../jsclone.cpp
        ../../jscntxt.cpp
        ../../jscompartment.cpp
        ../../jsdate.cpp
        ../../jsdbgapi.cpp
        ../../jsdhash.cpp
        ../../jsdtoa.cpp
        ../../jsemit.cpp
        ../../jsexn.cpp
        ../../jsfriendapi.cpp
        ../../jsfun.cpp
        ../../jsgc.cpp
        ../../jsgcchunk.cpp
        ../../jsgcstats.cpp
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
        ../../jsprobes.cpp
        ../../jspropertycache.cpp
        ../../jspropertytree.cpp
        ../../jsproxy.cpp
        ../../jsreflect.cpp
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
        ../../sharkctl.cpp
# Assembler
        ../../assembler/jit/ExecutableAllocator.cpp
        ../../assembler/jit/ExecutableAllocatorOS2.cpp
        ../../assembler/jit/ExecutableAllocatorPosix.cpp
        ../../assembler/jit/ExecutableAllocatorWin.cpp
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
# TraceJit
        ../../tracejit/Writer.cpp
# V8 DTOA
        ../../v8-dtoa/checks.cc
        ../../v8-dtoa/conversions.cc
        ../../v8-dtoa/diy-fp.cc
        ../../v8-dtoa/fast-dtoa.cc
        ../../v8-dtoa/platform.cc
        ../../v8-dtoa/utils.cc
        ../../v8-dtoa/v8-dtoa.cc
# Yarr
        ../../yarr/pcre/pcre_compile.cpp
        ../../yarr/pcre/pcre_exec.cpp
        ../../yarr/pcre/pcre_tables.cpp
        ../../yarr/pcre/pcre_ucp_searchfuncs.cpp
        ../../yarr/pcre/pcre_xclass.cpp
        ../../yarr/yarr/RegexCompiler.cpp
        ../../yarr/yarr/RegexJIT.cpp
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
# NanoJit
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
# TraceJit
        ../../tracejit/Writer.h
# OpCode tables
        ../../jitstats.tbl
        ../../jskeyword.tbl
        ../../jsopcode.tbl
        ../../jsproto.tbl
        ../../jsreops.tbl
)

target_compile_definitions( jscript PRIVATE
    _LIB
    EXPORT_JS_API
    HW_THREADS
    JSFILE
    STDC_HEADERS
    HAVE_CPP_NAMESPACE_STD
    AVMPLUS_64BIT
    ENABLE_ASSEMBLER=1
    ENABLE_JIT=1
    ENABLE_METHODJIT=1
    ENABLE_MONOIC=1
    ENABLE_POLYIC_TYPED_ARRAY=1
    ENABLE_POLYIC=1
    ENABLE_TRACEJIT=1
    FEATURE_NANOJIT
    JS_TRACER
)

target_include_directories(jscript PRIVATE
    ${PROJECT_SOURCE_DIR}/../../assembler
    ${PROJECT_SOURCE_DIR}/../../nanojit
    ${PROJECT_SOURCE_DIR}/../../tracejit
    ${PROJECT_SOURCE_DIR}/../../yarr
    ${PROJECT_SOURCE_DIR}/../..
)

if (WIN32)

    # See Configure.in
    target_compile_definitions( jscript PRIVATE
        _AMD64_
        _CRT_NONSTDC_NO_DEPRECATE
        _CRT_SECURE_NO_DEPRECATE
        _WIN32
        _WIN64
        _WINDOWS
        AVMPLUS_AMD64
        AVMPLUS_WIN32
        HAVE_SNPRINTF
        JS_STDDEF_H_HAS_INTPTR_T
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
        add_compile_options(-v)
        target_compile_definitions( jscript PRIVATE
            XP_MACOSX
            NO_X11
            USE_PTHREADS=1
            UNIX_ASYNC_DNS
            _BSD_SOURCE
            AVMPLUS_AMD64
            AVMPLUS_MAC
            NANOJIT_ARCH=X64
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
        target_compile_definitions(jscript PRIVATE
            __x86_64__
            _REENTRANT
            AVMPLUS_AMD64
            AVMPLUS_LINUX
            AVMPLUS_UNIX
            NANOJIT_ARCH=X64
        )
        target_compile_options(jscript PRIVATE
            -fpermissive
            -Wno-narrowing
            -MMD
            -Wno-invalid-offsetof
            -Wno-ignored-attributes
            -Wno-int-to-pointer-cast
            -Wno-register
            $<$<CONFIG:Release>:-Os>
        )
    endif()
endif(WIN32)