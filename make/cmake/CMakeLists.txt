cmake_minimum_required(VERSION 3.18)
project(uox3 VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# *************************************************************************
# The basic executable we are making
# *************************************************************************
# We define the groupings for the files
#

add_executable(uox3
	${PROJECT_SOURCE_DIR}/../../source/uox3.cpp
	
	$<$<STREQUAL:${CMAKE_SYSTEM_NAME},Windows>:${PROJECT_SOURCE_DIR}/../../assets/uox3.rc>
# Script Group
	${PROJECT_SOURCE_DIR}/../../source/cServerData.cpp  
	${PROJECT_SOURCE_DIR}/../../source/cServerData.h
	${PROJECT_SOURCE_DIR}/../../source/cServerDefinitions.cpp
	${PROJECT_SOURCE_DIR}/../../source/cServerDefinitions.h
	${PROJECT_SOURCE_DIR}/../../source/Dictionary.cpp
	${PROJECT_SOURCE_DIR}/../../source/Dictionary.h
	${PROJECT_SOURCE_DIR}/../../source/scriptc.cpp
	${PROJECT_SOURCE_DIR}/../../source/scriptc.h
	${PROJECT_SOURCE_DIR}/../../source/ssection.cpp
	${PROJECT_SOURCE_DIR}/../../source/ssection.h	
# Base Object Group
	${PROJECT_SOURCE_DIR}/../../source/boats.cpp   
	${PROJECT_SOURCE_DIR}/../../source/cBaseObject.cpp
	${PROJECT_SOURCE_DIR}/../../source/cBaseObject.h
	${PROJECT_SOURCE_DIR}/../../source/cChar.cpp
	${PROJECT_SOURCE_DIR}/../../source/cChar.h
	${PROJECT_SOURCE_DIR}/../../source/cGuild.cpp
	${PROJECT_SOURCE_DIR}/../../source/cGuild.h
	${PROJECT_SOURCE_DIR}/../../source/cItem.cpp
	${PROJECT_SOURCE_DIR}/../../source/cItem.h
	${PROJECT_SOURCE_DIR}/../../source/cMultiObj.cpp
	${PROJECT_SOURCE_DIR}/../../source/cMultiObj.h
	${PROJECT_SOURCE_DIR}/../../source/cSocket.cpp
	${PROJECT_SOURCE_DIR}/../../source/cSocket.h
	${PROJECT_SOURCE_DIR}/../../source/cSpawnRegion.cpp
	${PROJECT_SOURCE_DIR}/../../source/cSpawnRegion.h
	${PROJECT_SOURCE_DIR}/../../source/cThreadQueue.cpp
	${PROJECT_SOURCE_DIR}/../../source/cThreadQueue.h
	${PROJECT_SOURCE_DIR}/../../source/funcdecl.h
	${PROJECT_SOURCE_DIR}/../../source/ObjectFactory.cpp
	${PROJECT_SOURCE_DIR}/../../source/ObjectFactory.h
	${PROJECT_SOURCE_DIR}/../../source/PageVector.h
	${PROJECT_SOURCE_DIR}/../../source/uoxstruct.h
# Subsystem Group Group
   ${PROJECT_SOURCE_DIR}/../../source/books.cpp
   ${PROJECT_SOURCE_DIR}/../../source/books.h
   ${PROJECT_SOURCE_DIR}/../../source/subsystem/account.cpp
   ${PROJECT_SOURCE_DIR}/../../source/subsystem/account.hpp
   ${PROJECT_SOURCE_DIR}/../../source/cEffects.h
   ${PROJECT_SOURCE_DIR}/../../source/CGump.cpp
   ${PROJECT_SOURCE_DIR}/../../source/CGump.h
   ${PROJECT_SOURCE_DIR}/../../source/cHTMLSystem.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cHTMLSystem.h  
   ${PROJECT_SOURCE_DIR}/../../source/cMagic.h
	${PROJECT_SOURCE_DIR}/../../source/subsystem/console.cpp
	${PROJECT_SOURCE_DIR}/../../source/subsystem/console.hpp
   ${PROJECT_SOURCE_DIR}/../../source/combat.cpp
   ${PROJECT_SOURCE_DIR}/../../source/combat.h
   ${PROJECT_SOURCE_DIR}/../../source/commands.cpp
   ${PROJECT_SOURCE_DIR}/../../source/commands.h
   ${PROJECT_SOURCE_DIR}/../../source/cRaces.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cRaces.h
   ${PROJECT_SOURCE_DIR}/../../source/cWeather.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cWeather.hpp
   ${PROJECT_SOURCE_DIR}/../../source/gumps.cpp
   ${PROJECT_SOURCE_DIR}/../../source/house.cpp
   ${PROJECT_SOURCE_DIR}/../../source/items.cpp
   ${PROJECT_SOURCE_DIR}/../../source/jail.cpp
   ${PROJECT_SOURCE_DIR}/../../source/jail.h
   ${PROJECT_SOURCE_DIR}/../../source/lineofsight.cpp
   ${PROJECT_SOURCE_DIR}/../../source/magic.cpp
   ${PROJECT_SOURCE_DIR}/../../source/magic.h
   ${PROJECT_SOURCE_DIR}/../../source/movement.cpp
   ${PROJECT_SOURCE_DIR}/../../source/movement.h
   ${PROJECT_SOURCE_DIR}/../../source/msgboard.cpp
   ${PROJECT_SOURCE_DIR}/../../source/msgboard.h
   ${PROJECT_SOURCE_DIR}/../../source/PartySystem.cpp
   ${PROJECT_SOURCE_DIR}/../../source/PartySystem.h
   ${PROJECT_SOURCE_DIR}/../../source/quantityfuncs.cpp
   ${PROJECT_SOURCE_DIR}/../../source/queue.cpp
   ${PROJECT_SOURCE_DIR}/../../source/regions.cpp
   ${PROJECT_SOURCE_DIR}/../../source/regions.h
   ${PROJECT_SOURCE_DIR}/../../source/skills.cpp
   ${PROJECT_SOURCE_DIR}/../../source/skills.h
   ${PROJECT_SOURCE_DIR}/../../source/sound.cpp
   ${PROJECT_SOURCE_DIR}/../../source/speech.cpp
   ${PROJECT_SOURCE_DIR}/../../source/speech.h
   ${PROJECT_SOURCE_DIR}/../../source/targeting.cpp
   ${PROJECT_SOURCE_DIR}/../../source/teffect.h
   ${PROJECT_SOURCE_DIR}/../../source/townregion.cpp
   ${PROJECT_SOURCE_DIR}/../../source/townregion.h
   ${PROJECT_SOURCE_DIR}/../../source/trade.cpp
   ${PROJECT_SOURCE_DIR}/../../source/vendor.cpp
   ${PROJECT_SOURCE_DIR}/../../source/weight.cpp
   ${PROJECT_SOURCE_DIR}/../../source/weight.h
   ${PROJECT_SOURCE_DIR}/../../source/wholist.cpp
   ${PROJECT_SOURCE_DIR}/../../source/wholist.h
   ${PROJECT_SOURCE_DIR}/../../source/worldmain.cpp
   ${PROJECT_SOURCE_DIR}/../../source/worldmain.h
# Other group
   ${PROJECT_SOURCE_DIR}/../../source/ai.cpp 
   ${PROJECT_SOURCE_DIR}/../../source/archive.cpp
   ${PROJECT_SOURCE_DIR}/../../source/calcfuncs.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cDice.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cdice.h
   ${PROJECT_SOURCE_DIR}/../../source/classes.h
   ${PROJECT_SOURCE_DIR}/../../source/cmdtable.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cmdtable.h
   ${PROJECT_SOURCE_DIR}/../../source/cPlayerAction.cpp
   ${PROJECT_SOURCE_DIR}/../../source/CResponse.cpp
   ${PROJECT_SOURCE_DIR}/../../source/CResponse.h
   ${PROJECT_SOURCE_DIR}/../../source/cSkillClass.h
   ${PROJECT_SOURCE_DIR}/../../source/other/uoxversion.cpp
   ${PROJECT_SOURCE_DIR}/../../source/other/uoxversion.hpp 
   ${PROJECT_SOURCE_DIR}/../../source/dist.cpp
   ${PROJECT_SOURCE_DIR}/../../source/effect.cpp
   ${PROJECT_SOURCE_DIR}/../../source/enums.h
   ${PROJECT_SOURCE_DIR}/../../source/fileio.cpp
   ${PROJECT_SOURCE_DIR}/../../source/findfuncs.cpp
   ${PROJECT_SOURCE_DIR}/../../source/GenericList.h
   ${PROJECT_SOURCE_DIR}/../../source/npcs.cpp
   ${PROJECT_SOURCE_DIR}/../../source/osunique.cpp
   ${PROJECT_SOURCE_DIR}/../../source/osunique.hpp
   ${PROJECT_SOURCE_DIR}/../../source/pcmanage.cpp
   ${PROJECT_SOURCE_DIR}/../../source/power.h
   ${PROJECT_SOURCE_DIR}/../../source/ostype.h
   ${PROJECT_SOURCE_DIR}/../../source/typedefs.h
   ${PROJECT_SOURCE_DIR}/../../source/useful.h   
   ${PROJECT_SOURCE_DIR}/../../source/useful.h
# JS Engine group
   ${PROJECT_SOURCE_DIR}/../../source/CJSEngine.cpp 
   ${PROJECT_SOURCE_DIR}/../../source/CJSEngine.h
   ${PROJECT_SOURCE_DIR}/../../source/CJSMapping.cpp
   ${PROJECT_SOURCE_DIR}/../../source/CJSMapping.h
   ${PROJECT_SOURCE_DIR}/../../source/cScript.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cScript.h
   ${PROJECT_SOURCE_DIR}/../../source/JSEncapsulate.cpp
   ${PROJECT_SOURCE_DIR}/../../source/JSEncapsulate.h
   ${PROJECT_SOURCE_DIR}/../../source/SEFunctions.cpp
   ${PROJECT_SOURCE_DIR}/../../source/SEFunctions.h
   ${PROJECT_SOURCE_DIR}/../../source/spidermonkey.h
   ${PROJECT_SOURCE_DIR}/../../source/UOXJSClasses.h
   ${PROJECT_SOURCE_DIR}/../../source/UOXJSMethods.cpp
   ${PROJECT_SOURCE_DIR}/../../source/UOXJSMethods.h
   ${PROJECT_SOURCE_DIR}/../../source/UOXJSPropertyEnums.h
   ${PROJECT_SOURCE_DIR}/../../source/UOXJSPropertyFuncs.cpp
   ${PROJECT_SOURCE_DIR}/../../source/UOXJSPropertyFuncs.h
   ${PROJECT_SOURCE_DIR}/../../source/UOXJSPropertySpecs.h
# Network group
   ${PROJECT_SOURCE_DIR}/../../source/CPacketReceive.cpp  
   ${PROJECT_SOURCE_DIR}/../../source/CPacketReceive.h
   ${PROJECT_SOURCE_DIR}/../../source/CPacketSend.cpp
   ${PROJECT_SOURCE_DIR}/../../source/CPacketSend.h
   ${PROJECT_SOURCE_DIR}/../../source/network.cpp
   ${PROJECT_SOURCE_DIR}/../../source/network.h
   ${PROJECT_SOURCE_DIR}/../../source/IP4Address.cpp
   ${PROJECT_SOURCE_DIR}/../../source/IP4Address.hpp
# Utility group
   ${PROJECT_SOURCE_DIR}/../../source/StringUtility.cpp  
   ${PROJECT_SOURCE_DIR}/../../source/StringUtility.hpp
   ${PROJECT_SOURCE_DIR}/../../source/TimeUtility.cpp
   ${PROJECT_SOURCE_DIR}/../../source/TimeUtility.hpp
   ${PROJECT_SOURCE_DIR}/../../source/EventTimer.cpp
   ${PROJECT_SOURCE_DIR}/../../source/EventTimer.hpp
   ${PROJECT_SOURCE_DIR}/../../source/utility/strutil.hpp
# UOData group
    ${PROJECT_SOURCE_DIR}/../../source/mapclasses.h  
    ${PROJECT_SOURCE_DIR}/../../source/mapstuff.cpp
    ${PROJECT_SOURCE_DIR}/../../source/mapstuff.h
    ${PROJECT_SOURCE_DIR}/../../source/MultiMul.cpp
    ${PROJECT_SOURCE_DIR}/../../source/MultiMul.hpp
    ${PROJECT_SOURCE_DIR}/../../source/UOPData.cpp
    ${PROJECT_SOURCE_DIR}/../../source/UOPData.hpp
)

# *************************************************************************
# Platform specific settings
# *************************************************************************

if (WIN32)
set (CMAKE_RC_STANDARD_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/../../assets/)
	
target_compile_definitions(uox3 PRIVATE
		_WINDOWS
		EXPORT_JS_API
		_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
		_CRT_NO_VA_START_VALIDATION
		_CRT_SECURE_NO_DEPRECATE
		_CRT_NONSTDC_NO_DEPRECATE
		_CONSOLE
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        P_TIMESTAMP=1
		$<$<CONFIG:Release>:NDEBUG>
		$<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Debug>:UOX_DEBUG_MODE>
		$<$<CONFIG:Debug>:_ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH>
	)
	
	target_compile_options( uox3 PRIVATE
		/J
		$<$<CONFIG:Release>:/O2>
	)
else()

	target_compile_definitions( uox3 PRIVATE
		EXPORT_JS_API
        P_TIMESTAMP=1
		$<$<CONFIG:Debug>:DEBUG>
        $<$<CONFIG:Debug>:UOX_DEBUG_MODE>
	)
	
	target_compile_options( uox3 PRIVATE
		-funsigned-char
		 -Wno-shift-negative-value
		 -Wno-deprecated-declarations
         -Wno-format-security
	)
	if (APPLE)
# We always use the system zlib , as it is most likely better optimized
		set(SYSTEM_ZLIB ON)

		target_compile_options( uox3 PRIVATE
			$<$<CONFIG:Release>:-Os>
		)
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
		${PROJECT_SOURCE_DIR}/../../spidermonkey
)
if (NOT SYSTEM_ZLIB)
target_include_directories(uox3
	PUBLIC
		${PROJECT_SOURCE_DIR}/../../zlib
)
endif (NOT SYSTEM_ZLIB)
# *************************************************************************
# The libraries we need
# *************************************************************************
target_link_libraries(uox3 PRIVATE
	jscript
	$<$<PLATFORM_ID:Windows>:ws2_32>
	$<$<PLATFORM_ID:Windows>:Kernel32>
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
add_subdirectory(${PROJECT_SOURCE_DIR}/../../spidermonkey/make/cmake subproject/jscript)
if (NOT SYSTEM_ZLIB)
add_subdirectory(${PROJECT_SOURCE_DIR}/../../zlib/make/cmake subproject/zlib)
endif(NOT SYSTEM_ZLIB)
