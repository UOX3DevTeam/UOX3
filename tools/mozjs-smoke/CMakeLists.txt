cmake_minimum_required(VERSION 3.18)

project(uox3_mozjs_smoke LANGUAGES CXX)

set(MOZJS_BUILD_DIR "" CACHE PATH "mozjs-sys generated build directory")
set(MOZJS_BRIDGE_LIB "" CACHE FILEPATH "uox3-mozjs-bridge static library")
if(NOT EXISTS "${MOZJS_BUILD_DIR}/dist/include/jsapi.h")
    message(FATAL_ERROR "MOZJS_BUILD_DIR must point at mozjs-sys' generated build directory")
endif()
if(NOT EXISTS "${MOZJS_BRIDGE_LIB}")
    message(FATAL_ERROR "MOZJS_BRIDGE_LIB must point at uox3_mozjs_bridge.lib")
endif()

add_executable(uox3_mozjs_smoke mozjs_smoke.cpp)
target_compile_features(uox3_mozjs_smoke PRIVATE cxx_std_17)
target_include_directories(uox3_mozjs_smoke PRIVATE "${MOZJS_BUILD_DIR}/dist/include")

if(MSVC)
    target_compile_options(uox3_mozjs_smoke PRIVATE
        "/FI${MOZJS_BUILD_DIR}/js/src/js-confdefs.h"
        /GR-
    )
    target_compile_definitions(uox3_mozjs_smoke PRIVATE STATIC_JS_API WIN32)
endif()

target_link_libraries(uox3_mozjs_smoke PRIVATE
    "${MOZJS_BRIDGE_LIB}"
    winmm
    psapi
    user32
    dbghelp
    legacy_stdio_definitions
    kernel32
    ntdll
    userenv
    ws2_32
)
