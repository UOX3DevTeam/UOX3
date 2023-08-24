cmake_minimum_required(VERSION 3.18)
project(uox3 VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# *************************************************************************
# The basic executable we are making
# *************************************************************************

add_executable(uox3
	../../source/uox3.cpp
	../../source/uox3.h
	../../source/cServerData.cpp   # Script group	
	../../source/cServerData.h
	../../source/cServerDefinitions.cpp
	../../source/cServerDefinitions.h
	../../source/Dictionary.cpp
	../../source/Dictionary.h
	../../source/scriptc.cpp
	../../source/scriptc.h
	../../source/ssection.cpp
	../../source/ssection.h	
	../../source/boats.cpp     # Base Object group
	../../source/cBaseobject.cpp
	../../source/cBaseobject.h
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
   ../../source/books.cpp # Subystem group
   ../../source/books.h
   ../../source/cAccountClass.cpp
   ../../source/cAccountClass.h
   ../../source/cEffects.h
   ../../source/CGump.cpp
   ../../source/cHTMLSystem.cpp
   ../../source/cHTMLSystem.h  
   ../../source/cMagic.h
   ../../source/cmdtable.cpp
   ../../source/combat.cpp
   ../../source/combat.h
   ../../source/commands.cpp
   ../../source/commands.h
   ../../source/cRaces.cpp
   ../../source/cRaces.h
   ../../source/cWeather.cpp
   ../../source/cWeather.hpp
   ../../source/gumps.cpp
   ../../source/gump.h
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
   ../../source/ai.cpp  # Other group
   ../../source/archive.cpp
   ../../source/calcfuncs.cpp
   ../../source/cDice.cpp
   ../../source/cdice.h
   ../../source/classes.h
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
   ../../source/CJSEngine.cpp  # JS Engine group
   ../../source/CJSEngine.h
   ../../source/CJSMapping.cpp
   ../../source/CJSMapping.h
   ../../source/cScript.cpp
   ../../source/cScript.h
   ../../source/JSEncapsulate.cpp
   ../../source/JSEncapsulate.h
   ../../source/SEFunctions.cpp
   ../../source/SEFunctions.h
   ../../source/UOXJSClasses.h
   ../../source/UOXJSMethods.cpp
   ../../source/UOXJSMethods.h
   ../../source/UOXJSPropertyEnums.h
   ../../source/UOXJSPropertyFuncs.cpp
   ../../source/UOXJSPropertyFuncs.h
   ../../source/UOXJSPropertySpecs.h
   ../../source/CPacketReceive.cpp  # Network group
   ../../source/CPacketReceive.h
   ../../source/CPacketSend.cpp
   ../../source/CPacketSend.h
   ../../source/network.cpp
   ../../source/network.h
   ../../source/IP4Address.cpp
   ../../source/IP4Address.hpp
   ../../source/StringUtility.cpp  # Utility group
   ../../source/StringUtility.hpp
   ../../source/TimeUtility.cpp
   ../../source/TimeUtility.hpp
   ../../source/EventTimer.cpp
   ../../source/EventTimer.hpp
    ../../source/mapclasses.h  # UOData group
    ../../source/mapstuff.cpp
    ../../source/mapstuff.h
    ../../source/MultiMul.cpp
    ../../source/MultiMul.hpp
    ../../source/UOPData.cpp
    ../../source/UOPData.hpp
	$<$<STREQUAL:${CMAKE_SYSTEM_NAME},Windows>:${PROJECT_SOURCE_DIR}/../../assets/uox3.rc>
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
		$<$<CONFIG:Release>:NDEBUG>
		$<$<CONFIG:Debug>:_DEBUG>
		$<$<CONFIG:Debug>:_ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH>
	)
	
	target_compile_options( uox3 PRIVATE
		/J
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
