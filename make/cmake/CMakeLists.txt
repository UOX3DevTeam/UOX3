cmake_minimum_required(VERSION 3.18)
project(uox3 VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# *************************************************************************
# The basic executable we are making
# *************************************************************************

add_executable(uox3
   ../../source/ai.cpp
   ../../source/archive.cpp
   ../../source/boats.cpp
   ../../source/books.cpp
   ../../source/cAccountClass.cpp
   ../../source/calcfuncs.cpp
   ../../source/cBaseobject.cpp
   ../../source/cChar.cpp
   ../../source/cConsole.cpp
   ../../source/cDice.cpp
   ../../source/cGuild.cpp
   ../../source/CGump.cpp
   ../../source/cHTMLSystem.cpp
   ../../source/cItem.cpp
   ../../source/CJSEngine.cpp
   ../../source/CJSMapping.cpp
   ../../source/cmdtable.cpp
   ../../source/cMultiObj.cpp
   ../../source/combat.cpp
   ../../source/commands.cpp
   ../../source/CPacketReceive.cpp
   ../../source/CPacketSend.cpp
   ../../source/cPlayerAction.cpp
   ../../source/cRaces.cpp
   ../../source/CResponse.cpp
   ../../source/cScript.cpp
   ../../source/cServerData.cpp
   ../../source/cServerDefinitions.cpp
   ../../source/cSocket.cpp
   ../../source/cSpawnRegion.cpp
   ../../source/cThreadQueue.cpp
   ../../source/cVersionClass.cpp
   ../../source/cWeather.cpp
   ../../source/Dictionary.cpp
   ../../source/dist.cpp
   ../../source/effect.cpp
   ../../source/EventTimer.cpp
   ../../source/fileio.cpp
   ../../source/findfuncs.cpp
   ../../source/gumps.cpp
   ../../source/house.cpp
   ../../source/IP4Address.cpp
   ../../source/items.cpp
   ../../source/jail.cpp
   ../../source/JSEncapsulate.cpp
   ../../source/lineofsight.cpp
   ../../source/magic.cpp
   ../../source/mapstuff.cpp
   ../../source/movement.cpp
   ../../source/msgboard.cpp
   ../../source/MultiMul.cpp
   ../../source/network.cpp
   ../../source/npcs.cpp
   ../../source/ObjectFactory.cpp
   ../../source/PartySystem.cpp
   ../../source/pcmanage.cpp
   ../../source/quantityfuncs.cpp
   ../../source/queue.cpp
   ../../source/regions.cpp
   ../../source/scriptc.cpp
   ../../source/SEFunctions.cpp
   ../../source/skills.cpp
   ../../source/sound.cpp
   ../../source/speech.cpp
   ../../source/ssection.cpp
   ../../source/osunique.cpp
   ../../source/StringUtility.cpp
   ../../source/targeting.cpp
   ../../source/townregion.cpp
   ../../source/trade.cpp
   ../../source/UOPData.cpp
   ../../source/uox3.cpp
   ../../source/UOXJSMethods.cpp
   ../../source/UOXJSPropertyFuncs.cpp
   ../../source/vendor.cpp
   ../../source/weight.cpp
   ../../source/wholist.cpp
   ../../source/worldmain.cpp
   ../../source/TimeUtility.cpp
   ../../source/osunique.hpp
   ../../source/books.h
   ../../source/cAccountClass.h
   ../../source/cBaseObject.h
   ../../source/cChar.h
   ../../source/cConsole.h
   ../../source/cdice.h
   ../../source/cEffects.h
   ../../source/cGuild.h
   ../../source/cHTMLSystem.h
   ../../source/cItem.h
   ../../source/CJSEngine.h
   ../../source/CJSMapping.h
   ../../source/classes.h
   ../../source/cMagic.h
   ../../source/cmdtable.h
   ../../source/cMultiObj.h
   ../../source/combat.h
   ../../source/commands.h
   ../../source/ConfigOS.h
   ../../source/CPacketReceive.h
   ../../source/CPacketSend.h
   ../../source/cRaces.h
   ../../source/CResponse.h
   ../../source/cScript.h
   ../../source/cServerData.h
   ../../source/cServerDefinitions.h
   ../../source/cSkillClass.h
   ../../source/cSocket.h
   ../../source/cSpawnRegion.h
   ../../source/cThreadQueue.h
   ../../source/cVersionClass.h
   ../../source/cWeather.hpp
   ../../source/Dictionary.h
   ../../source/enums.h
   ../../source/EventTimer.hpp
   ../../source/funcdecl.h
   ../../source/GenericList.h
   ../../source/gump.h
   ../../source/IP4Address.hpp
   ../../source/jail.h
   ../../source/JSEncapsulate.h
   ../../source/magic.h
   ../../source/mapclasses.h
   ../../source/mapstuff.h
   ../../source/movement.h
   ../../source/msgboard.h
   ../../source/MultiMul.hpp
   ../../source/network.h
   ../../source/ObjectFactory.h
   ../../source/PageVector.h
   ../../source/PartySystem.h
   ../../source/power.h
   ../../source/Prerequisites.h
   ../../source/regions.h
   ../../source/scriptc.h
   ../../source/SEFunctions.h
   ../../source/skills.h
   ../../source/speech.h
   ../../source/ssection.h
   ../../source/StringUtility.hpp
   ../../source/teffect.h
   ../../source/townregion.h
   ../../source/typedefs.h
   ../../source/UOPData.hpp
   ../../source/uox3.h
   ../../source/UOXJSClasses.h
   ../../source/UOXJSMethods.h
   ../../source/UOXJSPropertyEnums.h
   ../../source/UOXJSPropertyFuncs.h
   ../..//source/UOXJSPropertySpecs.h
   ../../source/UOXStdHeaders.h
   ../../source/uoxstruct.h
   ../../source/weight.h
   ../../source/wholist.h
   ../../source/worldmain.h
   ../../source/TimeUtility.hpp
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
	)
	if (APPLE)
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
		${PROJECT_SOURCE_DIR}/../../zlib
)

# *************************************************************************
# The libraries we need
# *************************************************************************
target_link_libraries(uox3 PRIVATE
	jscript
	zlib 
	$<$<PLATFORM_ID:Windows>:ws2_32>
	$<$<PLATFORM_ID:Windows>:Kernel32>
	$<$<PLATFORM_ID:Linux>:pthread>
)

# *************************************************************************
# The items we need built first
# *************************************************************************
add_subdirectory(${PROJECT_SOURCE_DIR}/../../spidermonkey/make/cmake subproject/jscript)
add_subdirectory(${PROJECT_SOURCE_DIR}/../../zlib/make/cmake subproject/zlib)