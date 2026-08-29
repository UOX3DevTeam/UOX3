cmake_minimum_required(VERSION 3.18)

if(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum OS X version")
endif()

project(uox3 VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
endif()

if(WIN32 AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR
        "mozjs 115 must be compiled with the ClangCL toolset. "
        "Configure Visual Studio builds with -T ClangCL.")
endif()

if(WIN32)
    set(MOZJS_DIR "${PROJECT_SOURCE_DIR}/../../spidermonkey")
    set(MOZJS_ARCHIVE "${MOZJS_DIR}/mozjs-115.13-windows-x64-clangcl.zip")
    set(MOZJS_PREBUILT_DIR "${CMAKE_BINARY_DIR}/mozjs-115.13")
    set(MOZJS_LIBRARY "${MOZJS_PREBUILT_DIR}/mozjs-115.13-windows-x64-clangcl.lib")

    if(NOT EXISTS "${MOZJS_ARCHIVE}")
        message(FATAL_ERROR "Bundled SpiderMonkey archive not found: ${MOZJS_ARCHIVE}")
    endif()

    if(NOT EXISTS "${MOZJS_LIBRARY}")
        file(MAKE_DIRECTORY "${MOZJS_PREBUILT_DIR}")
        file(ARCHIVE_EXTRACT
            INPUT "${MOZJS_ARCHIVE}"
            DESTINATION "${MOZJS_PREBUILT_DIR}")
    endif()

    if(NOT EXISTS "${MOZJS_LIBRARY}")
        message(FATAL_ERROR "Failed to extract bundled SpiderMonkey library")
    endif()
else()
    set(UOX3_MOZJS_LIBRARY "$ENV{UOX3_MOZJS_LIBRARY}" CACHE FILEPATH
        "Optional path to a custom UOX3 SpiderMonkey 115.13 static library")

    if(UOX3_MOZJS_LIBRARY)
        set(MOZJS_LIBRARY "${UOX3_MOZJS_LIBRARY}")
    elseif(APPLE)
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$")
            set(MOZJS_PLATFORM "macos-arm64")
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
            set(MOZJS_PLATFORM "macos-x64")
        else()
            message(FATAL_ERROR
                "No bundled SpiderMonkey library is available for macOS ${CMAKE_SYSTEM_PROCESSOR}.")
        endif()
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux"
            AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
        set(MOZJS_PLATFORM "linux-x64")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD"
            AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
        set(MOZJS_PLATFORM "freebsd-x64")
    else()
        message(FATAL_ERROR
            "No bundled SpiderMonkey library is available for ${CMAKE_SYSTEM_NAME} "
            "${CMAKE_SYSTEM_PROCESSOR}. Set UOX3_MOZJS_LIBRARY to a compatible custom build.")
    endif()

    if(NOT MOZJS_LIBRARY AND NOT MOZJS_LIBRARIES)
        set(MOZJS_ARCHIVE
            "${PROJECT_SOURCE_DIR}/../../spidermonkey/uox3-mozjs-115.13-${MOZJS_PLATFORM}.tar.gz")
        set(MOZJS_PREBUILT_DIR
            "${CMAKE_BINARY_DIR}/mozjs-115.13-${MOZJS_PLATFORM}")
        set(MOZJS_LIBRARY
            "${MOZJS_PREBUILT_DIR}/uox3-mozjs-115.13-${MOZJS_PLATFORM}.a")

        if(NOT EXISTS "${MOZJS_ARCHIVE}")
            message(FATAL_ERROR "Bundled SpiderMonkey archive not found: ${MOZJS_ARCHIVE}")
        endif()

        if(NOT EXISTS "${MOZJS_LIBRARY}")
            file(MAKE_DIRECTORY "${MOZJS_PREBUILT_DIR}")
            file(ARCHIVE_EXTRACT
                INPUT "${MOZJS_ARCHIVE}"
                DESTINATION "${MOZJS_PREBUILT_DIR}")
        endif()
    endif()

    if(MOZJS_LIBRARY AND NOT EXISTS "${MOZJS_LIBRARY}")
        message(FATAL_ERROR "SpiderMonkey library not found: ${MOZJS_LIBRARY}")
    endif()
endif()

if(NOT MOZJS_LIBRARIES)
    set(MOZJS_LIBRARIES "${MOZJS_LIBRARY}")
endif()

# 2. Fix for modern, strict GCC compilers (like on new Ubuntu/WSL)
#    Adds permissive flags only if using GCC greater than 10.0
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 10.0)
    message(STATUS "Applying compatibility flags for modern GCC ${CMAKE_CXX_COMPILER_VERSION}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -Wno-narrowing")

    # Add POSIX headers only needed on Linux
    if(UNIX AND NOT APPLE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include unistd.h -include sys/mman.h")
    endif()
endif()

# *************************************************************************
# The basic executable we are making
# *************************************************************************
# We define the groupings for the files
#

add_executable(uox3
	../../source/uox3.cpp
	../../source/uox3.h
	$<$<STREQUAL:${CMAKE_SYSTEM_NAME},Windows>:${PROJECT_SOURCE_DIR}/../../assets/uox3.rc>
# Script Group
	../../source/cServerData.cpp  
	../../source/cServerData.h
	../../source/cServerDefinitions.cpp
	../../source/cServerDefinitions.h
	../../source/Dictionary.cpp
	../../source/Dictionary.h
	../../source/scriptc.cpp
	../../source/scriptc.h
	../../source/ssection.cpp
	../../source/ssection.h	
# Base Object Group
	../../source/boats.cpp   
	../../source/cBaseObject.cpp
	../../source/cBaseObject.h
	../../source/cChar.cpp
	../../source/cChar.h
	../../source/cConsole.cpp
	../../source/cConsole.h
	../../source/cGuild.cpp
	../../source/cGuild.h
	../../source/cItem.cpp
	../../source/cItem.h
	../../source/cMultiObj.cpp
	../../source/cMultiObj.h
	../../source/cSocket.cpp
	../../source/cSocket.h
	../../source/cSpawnRegion.cpp
	../../source/cSpawnRegion.h
	../../source/cThreadQueue.cpp
	../../source/cThreadQueue.h
	../../source/funcdecl.h
	../../source/ObjectFactory.cpp
	../../source/ObjectFactory.h
	../../source/PageVector.h
	../../source/uoxstruct.h
# Subsystem Group Group
   ../../source/books.cpp
   ../../source/books.h
   ../../source/cAccountClass.cpp
	../../source/cLoginThrottler.cpp
	../../source/cLoginThrottler.h
	../../source/cPasswordHasher.cpp
	../../source/cPasswordHasher.h
   ../../source/cAccountClass.h
   ../../source/cEffects.h
   ../../source/CGump.cpp
   ../../source/CGump.h
   ../../source/cHTMLSystem.cpp
   ../../source/cHTMLSystem.h  
   ../../source/cMagic.h
   ../../source/combat.cpp
   ../../source/combat.h
   ../../source/commands.cpp
   ../../source/commands.h
   ../../source/cRaces.cpp
   ../../source/cRaces.h
   ../../source/cWeather.cpp
   ../../source/cWeather.hpp
   ../../source/gumps.cpp
   ../../source/house.cpp
   ../../source/items.cpp
   ../../source/jail.cpp
   ../../source/jail.h
   ../../source/lineofsight.cpp
   ../../source/magic.cpp
   ../../source/magic.h
   ../../source/movement.cpp
   ../../source/movement.h
   ../../source/msgboard.cpp
   ../../source/msgboard.h
   ../../source/PartySystem.cpp
   ../../source/PartySystem.h
   ../../source/quantityfuncs.cpp
   ../../source/queue.cpp
   ../../source/regions.cpp
   ../../source/regions.h
   ../../source/skills.cpp
   ../../source/skills.h
   ../../source/sound.cpp
   ../../source/speech.cpp
   ../../source/speech.h
   ../../source/targeting.cpp
   ../../source/teffect.h
   ../../source/townregion.cpp
   ../../source/townregion.h
   ../../source/trade.cpp
   ../../source/vendor.cpp
   ../../source/weight.cpp
   ../../source/weight.h
   ../../source/wholist.cpp
   ../../source/wholist.h
   ../../source/worldmain.cpp
   ../../source/worldmain.h
# Other group
   ../../source/ai.cpp 
   ../../source/archive.cpp
   ../../source/calcfuncs.cpp
   ../../source/cDice.cpp
   ../../source/cdice.h
   ../../source/classes.h
   ../../source/cmdtable.cpp
   ../../source/cmdtable.h
   ../../source/ConfigOS.h
   ../../source/cPlayerAction.cpp
   ../../source/CResponse.cpp
   ../../source/CResponse.h
   ../../source/cSkillClass.h
   ../../source/cVersionClass.cpp
   ../../source/cVersionClass.h
   ../../source/dist.cpp
   ../../source/effect.cpp
   ../../source/enums.h
   ../../source/fileio.cpp
   ../../source/findfuncs.cpp
   ../../source/GenericList.h
   ../../source/npcs.cpp
   ../../source/osunique.cpp
   ../../source/osunique.hpp
   ../../source/pcmanage.cpp
   ../../source/power.h
   ../../source/Prerequisites.h
   ../../source/typedefs.h
	../../source/UOXStdHeaders.h
# JS Engine group
   ../../source/CJSEngine.cpp 
   ../../source/CJSEngine.h
   ../../source/CJSMapping.cpp
   ../../source/CJSMapping.h
   ../../source/cScript.cpp
   ../../source/cScript.h
   ../../source/SEFunctions.cpp
   ../../source/SEFunctions.h
   ../../source/UOXJSClasses.h
   ../../source/UOXJSMethods.cpp
   ../../source/UOXJSMethods.h
   ../../source/UOXJSPropertyEnums.h
   ../../source/UOXJSPropertyFuncs.cpp
   ../../source/UOXJSPropertyFuncs.h
   ../../source/UOXJSPropertySpecs.h
   ../../source/UOXJSUtilities.cpp
   ../../source/UOXJSUtilities.h
# Network group
   ../../source/CPacketReceive.cpp  
   ../../source/CPacketReceive.h
   ../../source/CPacketSend.cpp
   ../../source/CPacketSend.h
   ../../source/network.cpp
   ../../source/network.h
   ../../source/IP4Address.cpp
   ../../source/IP4Address.hpp
# Utility group
   ../../source/StringUtility.cpp  
   ../../source/StringUtility.hpp
   ../../source/TimeUtility.cpp
   ../../source/TimeUtility.hpp
   ../../source/EventTimer.cpp
   ../../source/EventTimer.hpp
# UOData group
    ../../source/mapclasses.h  
    ../../source/mapstuff.cpp
    ../../source/mapstuff.h
    ../../source/MultiMul.cpp
    ../../source/MultiMul.hpp
    ../../source/UOPData.cpp
    ../../source/UOPData.hpp
)

if(MSVC)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT uox3)
    set_target_properties(uox3 PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/../../data"
    )
endif()

# *************************************************************************
# Platform specific settings
# *************************************************************************

if (WIN32)
set (CMAKE_RC_STANDARD_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/../../assets/)
	
target_compile_definitions(uox3 PRIVATE
		_WINDOWS
		JS_STANDALONE
		STATIC_JS_API
		_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
		_CRT_NO_VA_START_VALIDATION
		_CRT_SECURE_NO_DEPRECATE
		_CRT_NONSTDC_NO_DEPRECATE
		_CONSOLE
		$<$<CONFIG:Release>:NDEBUG>
		$<$<CONFIG:Debug>:_DEBUG>
		$<$<CONFIG:Debug>:_ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH>
	)
	
	target_compile_options( uox3 PRIVATE
		/J
		/GR-
		$<$<CONFIG:Release>:/O2>
	)
else()

	target_compile_definitions( uox3 PRIVATE
		EXPORT_JS_API
		$<$<CONFIG:Debug>:_DEBUG>
	)
	
	target_compile_options( uox3 PRIVATE
		-funsigned-char
		 -Wno-shift-negative-value
		 -Wno-deprecated-declarations
	)
	if (APPLE)
# We always use the system zlib , as it is most likely better optimized
		set(SYSTEM_ZLIB ON)

		target_compile_options( uox3 PRIVATE
			$<$<CONFIG:Release>:-Os>
		)
		target_link_options(uox3 PRIVATE -Wl,-ld_classic -Wl,-unaligned_pointers,suppress)
	else()
		target_compile_options(uox3 PRIVATE
			$<$<CONFIG:Release>:-O2>
			-Wno-attributes
		)
	endif(APPLE)
endif(WIN32)



# *************************************************************************
# the directories we need on the include path
# *************************************************************************

target_include_directories(uox3
	PUBLIC
		${PROJECT_SOURCE_DIR}/../../source
		$<$<PLATFORM_ID:Windows>:${MOZJS_DIR}/include>
		$<$<NOT:$<PLATFORM_ID:Windows>>:${PROJECT_SOURCE_DIR}/../../spidermonkey/include>
		${PROJECT_SOURCE_DIR}/../../utf8cpp
)
if (NOT SYSTEM_ZLIB)
target_include_directories(uox3 BEFORE
	PUBLIC
		${PROJECT_SOURCE_DIR}/../../zlib
)
endif (NOT SYSTEM_ZLIB)
# *************************************************************************
# The libraries we need
# *************************************************************************
target_link_libraries(uox3 PRIVATE
	${MOZJS_LIBRARIES}
	$<$<PLATFORM_ID:Windows>:ws2_32>
	$<$<PLATFORM_ID:Windows>:Kernel32>
	$<$<PLATFORM_ID:Windows>:winmm>
	$<$<PLATFORM_ID:Windows>:psapi>
	$<$<PLATFORM_ID:Windows>:user32>
	$<$<PLATFORM_ID:Windows>:dbghelp>
	$<$<PLATFORM_ID:Windows>:legacy_stdio_definitions>
	$<$<PLATFORM_ID:Windows>:ntdll>
	$<$<PLATFORM_ID:Windows>:userenv>
	$<$<PLATFORM_ID:Windows>:bcrypt>
	${CMAKE_DL_LIBS}
	$<$<PLATFORM_ID:Linux>:pthread>
   $<$<PLATFORM_ID:FreeBSD>:pthread>
   $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>
)

if (NOT SYSTEM_ZLIB)
target_link_libraries(uox3 PRIVATE
	zlib
)
else()
target_link_libraries(uox3 PRIVATE
	z
)
endif(NOT SYSTEM_ZLIB)

# *************************************************************************
# The items we need built first
# *************************************************************************
if (NOT SYSTEM_ZLIB)
add_subdirectory(${PROJECT_SOURCE_DIR}/../../zlib/make/cmake subproject/zlib)
endif(NOT SYSTEM_ZLIB)

# =========================================================================
# Installation Rules for creating release packages
# =========================================================================

# 1. Installs the executable into the 'UOX3' folder.
install(TARGETS uox3
    RUNTIME DESTINATION UOX3
)

# 2. Installs the 'docs' directory itself (NO TRAILING SLASH).
#    Result: UOX3/docs/
install(DIRECTORY ${PROJECT_SOURCE_DIR}/../../docs
    DESTINATION UOX3
)

# 3. Installs the contents of the 'data' directory (WITH TRAILING SLASH).
#    Result: UOX3/scripts/, UOX3/dfndata/, etc.
install(DIRECTORY ${PROJECT_SOURCE_DIR}/../../data/
    DESTINATION UOX3
)
