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

# type
    ${PROJECT_SOURCE_DIR}/../../source/type/clientversion.cpp
    ${PROJECT_SOURCE_DIR}/../../source/type/clientversion.hpp
    ${PROJECT_SOURCE_DIR}/../../source/type/startlocation.cpp
    ${PROJECT_SOURCE_DIR}/../../source/type/startlocation.hpp
    ${PROJECT_SOURCE_DIR}/../../source/type/era.cpp
    ${PROJECT_SOURCE_DIR}/../../source/type/era.hpp
    ${PROJECT_SOURCE_DIR}/../../source/type/uotime.cpp
    ${PROJECT_SOURCE_DIR}/../../source/type/uotime.hpp
    ${PROJECT_SOURCE_DIR}/../../source/type/weather.cpp
    ${PROJECT_SOURCE_DIR}/../../source/type/weather.hpp

# configuration
    ${PROJECT_SOURCE_DIR}/../../source/configuration/assistantfeature.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/assistantfeature.hpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/clientenable.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/clientenable.hpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/clientfeature.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/clientfeature.hpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/directorylocation.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/directorylocation.hpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/expansion.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/expansion.hpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/realnumberconfig.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/realnumberconfig.hpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/serverconfig.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/serverconfig.hpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/serverfeature.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/serverfeature.hpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/serverstring.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/serverstring.hpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/serverswitch.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/serverswitch.hpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/shortvalue.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/shortvalue.hpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/spawnfacet.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/spawnfacet.hpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/startlocconfig.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/startlocconfig.hpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/timersetting.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/timersetting.hpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/uintvalue.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/uintvalue.hpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/ushortvalue.cpp
    ${PROJECT_SOURCE_DIR}/../../source/configuration/ushortvalue.hpp

# Script Group
	${PROJECT_SOURCE_DIR}/../../source/cserverdefinitions.cpp
	${PROJECT_SOURCE_DIR}/../../source/cserverdefinitions.h
	${PROJECT_SOURCE_DIR}/../../source/dictionary.cpp
	${PROJECT_SOURCE_DIR}/../../source/dictionary.h
	${PROJECT_SOURCE_DIR}/../../source/scriptc.cpp
	${PROJECT_SOURCE_DIR}/../../source/scriptc.h
	${PROJECT_SOURCE_DIR}/../../source/ssection.cpp
	${PROJECT_SOURCE_DIR}/../../source/ssection.h	
# Base Object Group
	${PROJECT_SOURCE_DIR}/../../source/boats.cpp   
	${PROJECT_SOURCE_DIR}/../../source/cbaseobject.cpp
	${PROJECT_SOURCE_DIR}/../../source/cbaseobject.h
	${PROJECT_SOURCE_DIR}/../../source/cchar.cpp
	${PROJECT_SOURCE_DIR}/../../source/cchar.h
	${PROJECT_SOURCE_DIR}/../../source/cguild.cpp
	${PROJECT_SOURCE_DIR}/../../source/cguild.h
	${PROJECT_SOURCE_DIR}/../../source/citem.cpp
	${PROJECT_SOURCE_DIR}/../../source/citem.h
	${PROJECT_SOURCE_DIR}/../../source/cmultiobj.cpp
	${PROJECT_SOURCE_DIR}/../../source/cmultiobj.h
	${PROJECT_SOURCE_DIR}/../../source/csocket.cpp
	${PROJECT_SOURCE_DIR}/../../source/csocket.h
	${PROJECT_SOURCE_DIR}/../../source/cspawnregion.cpp
	${PROJECT_SOURCE_DIR}/../../source/cspawnregion.h
	${PROJECT_SOURCE_DIR}/../../source/cthreadqueue.cpp
	${PROJECT_SOURCE_DIR}/../../source/cthreadqueue.h
	${PROJECT_SOURCE_DIR}/../../source/funcdecl.h
	${PROJECT_SOURCE_DIR}/../../source/objectfactory.cpp
	${PROJECT_SOURCE_DIR}/../../source/objectfactory.h
	${PROJECT_SOURCE_DIR}/../../source/pagevector.h
	${PROJECT_SOURCE_DIR}/../../source/uoxstruct.h
# Subsystem Group Group
   ${PROJECT_SOURCE_DIR}/../../source/books.cpp
   ${PROJECT_SOURCE_DIR}/../../source/books.h
   ${PROJECT_SOURCE_DIR}/../../source/subsystem/account.cpp
   ${PROJECT_SOURCE_DIR}/../../source/subsystem/account.hpp
   ${PROJECT_SOURCE_DIR}/../../source/ceffects.h
   ${PROJECT_SOURCE_DIR}/../../source/cgump.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cgump.h
   ${PROJECT_SOURCE_DIR}/../../source/chtmlsystem.cpp
   ${PROJECT_SOURCE_DIR}/../../source/chtmlsystem.h  
   ${PROJECT_SOURCE_DIR}/../../source/cmagic.h
   ${PROJECT_SOURCE_DIR}/../../source/subsystem/console.cpp
   ${PROJECT_SOURCE_DIR}/../../source/subsystem/console.hpp
   ${PROJECT_SOURCE_DIR}/../../source/combat.cpp
   ${PROJECT_SOURCE_DIR}/../../source/combat.h
   ${PROJECT_SOURCE_DIR}/../../source/commands.cpp
   ${PROJECT_SOURCE_DIR}/../../source/commands.h
   ${PROJECT_SOURCE_DIR}/../../source/craces.cpp
   ${PROJECT_SOURCE_DIR}/../../source/craces.h
   ${PROJECT_SOURCE_DIR}/../../source/cweather.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cweather.hpp
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
   ${PROJECT_SOURCE_DIR}/../../source/partysystem.cpp
   ${PROJECT_SOURCE_DIR}/../../source/partysystem.h
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
# other group
   ${PROJECT_SOURCE_DIR}/../../source/other/uoxversion.cpp
   ${PROJECT_SOURCE_DIR}/../../source/other/uoxversion.hpp
   ${PROJECT_SOURCE_DIR}/../../source/other/global.cpp
   ${PROJECT_SOURCE_DIR}/../../source/other/global.hpp

# Other group
   ${PROJECT_SOURCE_DIR}/../../source/ai.cpp 
   ${PROJECT_SOURCE_DIR}/../../source/archive.cpp
   ${PROJECT_SOURCE_DIR}/../../source/calcfuncs.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cdice.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cdice.h
   ${PROJECT_SOURCE_DIR}/../../source/classes.h
   ${PROJECT_SOURCE_DIR}/../../source/cmdtable.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cmdtable.h
   ${PROJECT_SOURCE_DIR}/../../source/cplayeraction.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cresponse.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cresponse.h
   ${PROJECT_SOURCE_DIR}/../../source/cskillclass.h
   ${PROJECT_SOURCE_DIR}/../../source/dist.cpp
   ${PROJECT_SOURCE_DIR}/../../source/effect.cpp
   ${PROJECT_SOURCE_DIR}/../../source/enums.h
   ${PROJECT_SOURCE_DIR}/../../source/fileio.cpp
   ${PROJECT_SOURCE_DIR}/../../source/findfuncs.cpp
   ${PROJECT_SOURCE_DIR}/../../source/genericlist.h
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
   ${PROJECT_SOURCE_DIR}/../../source/cjsengine.cpp 
   ${PROJECT_SOURCE_DIR}/../../source/cjsengine.h
   ${PROJECT_SOURCE_DIR}/../../source/cjsmapping.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cjsmapping.h
   ${PROJECT_SOURCE_DIR}/../../source/cscript.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cscript.h
   ${PROJECT_SOURCE_DIR}/../../source/jsencapsulate.cpp
   ${PROJECT_SOURCE_DIR}/../../source/jsencapsulate.h
   ${PROJECT_SOURCE_DIR}/../../source/sefunctions.cpp
   ${PROJECT_SOURCE_DIR}/../../source/sefunctions.h
   ${PROJECT_SOURCE_DIR}/../../source/spidermonkey.h
   ${PROJECT_SOURCE_DIR}/../../source/uoxjsclasses.h
   ${PROJECT_SOURCE_DIR}/../../source/uoxjsmethods.cpp
   ${PROJECT_SOURCE_DIR}/../../source/uoxjsmethods.h
   ${PROJECT_SOURCE_DIR}/../../source/uoxjspropertyenums.h
   ${PROJECT_SOURCE_DIR}/../../source/uoxjspropertyfuncs.cpp
   ${PROJECT_SOURCE_DIR}/../../source/uoxjspropertyfuncs.h
   ${PROJECT_SOURCE_DIR}/../../source/uoxjspropertyspecs.h
# Network group
   ${PROJECT_SOURCE_DIR}/../../source/cpacketreceive.cpp  
   ${PROJECT_SOURCE_DIR}/../../source/cpacketreceive.h
   ${PROJECT_SOURCE_DIR}/../../source/cpacketsend.cpp
   ${PROJECT_SOURCE_DIR}/../../source/cpacketsend.h
   ${PROJECT_SOURCE_DIR}/../../source/network.cpp
   ${PROJECT_SOURCE_DIR}/../../source/network.h
   ${PROJECT_SOURCE_DIR}/../../source/ip4address.cpp
   ${PROJECT_SOURCE_DIR}/../../source/ip4address.hpp
# Utility group
   ${PROJECT_SOURCE_DIR}/../../source/stringutility.cpp  
   ${PROJECT_SOURCE_DIR}/../../source/stringutility.hpp
   ${PROJECT_SOURCE_DIR}/../../source/timeutility.cpp
   ${PROJECT_SOURCE_DIR}/../../source/timeutility.hpp
   ${PROJECT_SOURCE_DIR}/../../source/eventtimer.cpp
   ${PROJECT_SOURCE_DIR}/../../source/eventtimer.hpp
   ${PROJECT_SOURCE_DIR}/../../source/utility/strutil.hpp
   ${PROJECT_SOURCE_DIR}/../../source/utility/random.hpp
# UOData group
    ${PROJECT_SOURCE_DIR}/../../source/mapclasses.h
    ${PROJECT_SOURCE_DIR}/../../source/mapstuff.cpp
    ${PROJECT_SOURCE_DIR}/../../source/mapstuff.h
    ${PROJECT_SOURCE_DIR}/../../source/multimul.cpp
    ${PROJECT_SOURCE_DIR}/../../source/multimul.hpp
    ${PROJECT_SOURCE_DIR}/../../source/uopdata.cpp
    ${PROJECT_SOURCE_DIR}/../../source/uopdata.hpp
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
