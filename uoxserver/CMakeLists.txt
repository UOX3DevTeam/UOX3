cmake_minimum_required(VERSION 3.12.3)


project(uoxserver VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(uoxserver 
	STATIC
		ai.cpp
		archive.cpp
		boats.cpp
		books.cpp
		cAccountClass.cpp
		calcfuncs.cpp
		cBaseobject.cpp
		cChar.cpp
		cConsole.cpp
		cDice.cpp
		cGuild.cpp
		CGump.cpp
		cHTMLSystem.cpp
		cItem.cpp
		CJSEngine.cpp
		CJSMapping.cpp
		cmdtable.cpp
		cMultiObj.cpp
		combat.cpp
		commands.cpp
		CPacketReceive.cpp
		CPacketSend.cpp
		cPlayerAction.cpp
		cRaces.cpp
		CResponse.cpp
		cScript.cpp
		cServerData.cpp
		cServerDefinitions.cpp
		cSocket.cpp
		cSpawnRegion.cpp
		cThreadQueue.cpp
		cVersionClass.cpp
		cWeather.cpp
		Dictionary.cpp
		dist.cpp
		effect.cpp
		EventTimer.cpp
		fileio.cpp
		findfuncs.cpp
		gumps.cpp
		house.cpp
		IDXMul.cpp
		IP4Address.cpp
		items.cpp
		jail.cpp
		JSEncapsulate.cpp
		lineofsight.cpp
		magic.cpp
		mapstuff.cpp
		movement.cpp
		msgboard.cpp
		MultiMul.cpp
		network.cpp
		npcs.cpp
		ObjectFactory.cpp
		osunique.cpp
		PartySystem.cpp
		pcmanage.cpp
		quantityfuncs.cpp
		queue.cpp
		regions.cpp
		scriptc.cpp
		SEFunctions.cpp
		skills.cpp
		sound.cpp
		speech.cpp
		ssection.cpp
		StringUtility.cpp
		strutil.cpp
		targeting.cpp
		TimeUtility.cpp
		townregion.cpp
		trade.cpp
		UOPData.cpp
		UOPInterface.cpp
		UOXJSMethods.cpp
		UOXJSPropertyFuncs.cpp
		vendor.cpp
		weight.cpp
		wholist.cpp
		worldmain.cpp
)

# Dont forget this library is dependent in windows on jsengine.lib,zlib
#


#
# This is setup for release only
# Note the optimization level
#
target_include_directories(uoxserver
	PUBLIC
		${PROJECT_SOURCE_DIR}/../support/jsengine
		$<$<PLATFORM_ID:Windows>:${PROJECT_SOURCE_DIR}/../support/zlib>
)
target_compile_options(uoxserver 
	PUBLIC
		$<$<PLATFORM_ID:Windows>:/O2>
		$<$<PLATFORM_ID:Windows>:/W3>
		$<$<PLATFORM_ID:Windows>:/fp:precise>
		$<$<PLATFORM_ID:Windows>:/GL>
		$<$<PLATFORM_ID:Windows>:/Gy>
		$<$<PLATFORM_ID:Windows>:/GF>
		$<$<PLATFORM_ID:Windows>:/Zc:inline>

#		$<$<PLATFORM_ID:Windows>:/MT>
		$<$<PLATFORM_ID:Windows>:/J>

		$<$<PLATFORM_ID:Linux>:-O2>
		$<$<PLATFORM_ID:Linux>:-Wno-conversion-null>
		$<$<PLATFORM_ID:Linux>:-pthread>

		$<$<PLATFORM_ID:Darwin>:-O2>
		$<$<PLATFORM_ID:Darwin>:-Wno-shift-negative-value>

)
target_compile_definitions(uoxserver
	PRIVATE
	
		NDEBUG
		$<$<PLATFORM_ID:Windows>:_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING>
		$<$<PLATFORM_ID:Windows>:_CRT_NO_VA_START_VALIDATION>
		$<$<PLATFORM_ID:Windows>:EXPORT_JS_API>
		$<$<PLATFORM_ID:Windows>:_MBCS>
		$<$<PLATFORM_ID:Windows>:_CONSOLE>
	
)
if(WIN32)
target_link_options(uoxserver
	PUBLIC
		 /OPT:NOREF
)
endif(WIN32)


