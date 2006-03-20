# Microsoft Developer Studio Project File - Name="UOX3_Official" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=UOX3_Official - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UOX3_Official.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UOX3_Official.mak" CFG="UOX3_Official - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UOX3_Official - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "UOX3_Official - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "UOX3_Official - Win32 DebugProfile" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/UOX3", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UOX3_Official - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /Fr /YX /J /FD /c
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/UOX3_Official.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib ws2_32.lib js32.lib /nologo /subsystem:console /machine:I386 /out:"Release/UOX3.exe" /libpath:"JavaScript"

!ELSEIF  "$(CFG)" == "UOX3_Official - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gi /GX /ZI /Od /I "../../boost" /D "WIN32" /D "_DEBUG" /D "_MBCS" /Fr /YX /J /FD /c
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib kernel32.lib advapi32.lib shell32.lib js32.lib /nologo /subsystem:console /debug /machine:I386 /out:"Debug/UOX3.exe" /libpath:"JavaScript/" /libpath:"JavaScript"
# SUBTRACT LINK32 /profile /nodefaultlib

!ELSEIF  "$(CFG)" == "UOX3_Official - Win32 DebugProfile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "UOX3_Official___Win32_DebugProfile"
# PROP BASE Intermediate_Dir "UOX3_Official___Win32_DebugProfile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugProfile"
# PROP Intermediate_Dir "DebugProfile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "__NEW_NETWORK__" /D "__USE_CMULTIOBJ__" /FR /YX /J /FD /c
# ADD CPP /nologo /MTd /W3 /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /FR /YX /J /FD /c
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/UOX3_Official.bsc"
LINK32=link.exe
# ADD BASE LINK32 winspool.lib ole32.lib oleaut32.lib odbc32.lib odbccp32.lib ws2_32.lib JavaScript/js32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib js32.lib /nologo /subsystem:console /debug /machine:I386 /libpath:"JavaScript/" /libpath:"JavaScript"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 winspool.lib ole32.lib oleaut32.lib odbc32.lib odbccp32.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib js32.lib /nologo /subsystem:console /profile /debug /machine:I386 /out:"DebugProfile\UOX3.exe" /libpath:"JavaScript"

!ENDIF 

# Begin Target

# Name "UOX3_Official - Win32 Release"
# Name "UOX3_Official - Win32 Debug"
# Name "UOX3_Official - Win32 DebugProfile"
# Begin Group "Source Files"

# PROP Default_Filter "*.cpp"
# Begin Group "Scripts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cServerDefinitions.cpp
# End Source File
# Begin Source File

SOURCE=.\Dictionary.cpp
# End Source File
# Begin Source File

SOURCE=.\scriptc.cpp
# End Source File
# Begin Source File

SOURCE=.\ssection.cpp
# End Source File
# End Group
# Begin Group "Base Objects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cBaseobject.cpp
# End Source File
# Begin Source File

SOURCE=.\cChar.cpp
# End Source File
# Begin Source File

SOURCE=.\cConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\cGuild.cpp
# End Source File
# Begin Source File

SOURCE=.\cItem.cpp
# End Source File
# Begin Source File

SOURCE=.\cMultiObj.cpp
# End Source File
# Begin Source File

SOURCE=.\cSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\cSpawnRegion.cpp
# End Source File
# Begin Source File

SOURCE=.\cThreadQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\queue.cpp
# End Source File
# Begin Source File

SOURCE=.\threadsafeobject.cpp
# End Source File
# Begin Source File

SOURCE=.\townregion.cpp
# End Source File
# End Group
# Begin Group "Subsystem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\boats.cpp
# End Source File
# Begin Source File

SOURCE=.\books.cpp
# End Source File
# Begin Source File

SOURCE=.\cAccountClass.cpp
# End Source File
# Begin Source File

SOURCE=.\CGump.cpp
# End Source File
# Begin Source File

SOURCE=.\cHTMLSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\cmdtable.cpp
# End Source File
# Begin Source File

SOURCE=.\combat.cpp
# End Source File
# Begin Source File

SOURCE=.\commands.cpp
# End Source File
# Begin Source File

SOURCE=.\cRaces.cpp
# End Source File
# Begin Source File

SOURCE=.\cServerData.cpp
# End Source File
# Begin Source File

SOURCE=.\cWeather.cpp
# End Source File
# Begin Source File

SOURCE=.\gumps.cpp
# End Source File
# Begin Source File

SOURCE=.\house.cpp
# End Source File
# Begin Source File

SOURCE=.\items.cpp
# End Source File
# Begin Source File

SOURCE=.\jail.cpp
# End Source File
# Begin Source File

SOURCE=.\lineofsight.cpp
# End Source File
# Begin Source File

SOURCE=.\magic.cpp
# End Source File
# Begin Source File

SOURCE=.\mapstuff.cpp
# End Source File
# Begin Source File

SOURCE=.\movement.cpp
# End Source File
# Begin Source File

SOURCE=.\msgboard.cpp
# End Source File
# Begin Source File

SOURCE=.\quantityfuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\regions.cpp
# End Source File
# Begin Source File

SOURCE=.\skills.cpp
# End Source File
# Begin Source File

SOURCE=.\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\speech.cpp
# End Source File
# Begin Source File

SOURCE=.\targeting.cpp
# End Source File
# Begin Source File

SOURCE=.\trade.cpp
# End Source File
# Begin Source File

SOURCE=.\vendor.cpp
# End Source File
# Begin Source File

SOURCE=.\weight.cpp
# End Source File
# Begin Source File

SOURCE=.\wholist.cpp
# End Source File
# Begin Source File

SOURCE=.\worldmain.cpp
# End Source File
# End Group
# Begin Group "Other"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ai.cpp
# End Source File
# Begin Source File

SOURCE=.\archive.cpp
# End Source File
# Begin Source File

SOURCE=.\calcfuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\cDice.cpp
# End Source File
# Begin Source File

SOURCE=.\cPlayerAction.cpp
# End Source File
# Begin Source File

SOURCE=.\CResponse.cpp
# End Source File
# Begin Source File

SOURCE=.\cVersionClass.cpp
# End Source File
# Begin Source File

SOURCE=.\dist.cpp
# End Source File
# Begin Source File

SOURCE=.\door.cpp
# End Source File
# Begin Source File

SOURCE=.\effect.cpp
# End Source File
# Begin Source File

SOURCE=.\fileio.cpp
# End Source File
# Begin Source File

SOURCE=.\findfuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\npcs.cpp
# End Source File
# Begin Source File

SOURCE=.\pcmanage.cpp
# End Source File
# End Group
# Begin Group "JS Engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CJSEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\CJSMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\cScript.cpp
# End Source File
# Begin Source File

SOURCE=.\JSEncapsulate.cpp
# End Source File
# Begin Source File

SOURCE=.\SEFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\UOXJSMethods.cpp
# End Source File
# Begin Source File

SOURCE=.\UOXJSPropertyFuncs.cpp
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CPacketReceive.cpp
# End Source File
# Begin Source File

SOURCE=.\CPacketSend.cpp
# End Source File
# Begin Source File

SOURCE=.\network.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ObjectFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\uox3.cpp
# End Source File
# Begin Source File

SOURCE=.\ustring.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "*.h*"
# Begin Group "Base Objects No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cBaseObject.h
# End Source File
# Begin Source File

SOURCE=.\cChar.h
# End Source File
# Begin Source File

SOURCE=.\cConsole.h
# End Source File
# Begin Source File

SOURCE=.\cGuild.h
# End Source File
# Begin Source File

SOURCE=.\cItem.h
# End Source File
# Begin Source File

SOURCE=.\cMultiObj.h
# End Source File
# Begin Source File

SOURCE=.\cSocket.h
# End Source File
# Begin Source File

SOURCE=.\cThreadQueue.h
# End Source File
# Begin Source File

SOURCE=.\funcdecl.h
# End Source File
# Begin Source File

SOURCE=.\PageVector.h
# End Source File
# Begin Source File

SOURCE=.\speech.h
# End Source File
# Begin Source File

SOURCE=.\threadsafeobject.h
# End Source File
# Begin Source File

SOURCE=.\uoxstruct.h
# End Source File
# End Group
# Begin Group "Script "

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cServerData.h
# End Source File
# Begin Source File

SOURCE=.\cServerDefinitions.h
# End Source File
# Begin Source File

SOURCE=.\Dictionary.h
# End Source File
# Begin Source File

SOURCE=.\scriptc.h
# End Source File
# Begin Source File

SOURCE=.\ssection.h
# End Source File
# End Group
# Begin Group "Subsystem No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\books.h
# End Source File
# Begin Source File

SOURCE=.\cAccountClass.h
# End Source File
# Begin Source File

SOURCE=.\cEffects.h
# End Source File
# Begin Source File

SOURCE=.\cHTMLSystem.h
# End Source File
# Begin Source File

SOURCE=.\cMagic.h
# End Source File
# Begin Source File

SOURCE=.\combat.h
# End Source File
# Begin Source File

SOURCE=.\commands.h
# End Source File
# Begin Source File

SOURCE=.\cRaces.h
# End Source File
# Begin Source File

SOURCE=.\cWeather.hpp
# End Source File
# Begin Source File

SOURCE=.\gump.h
# End Source File
# Begin Source File

SOURCE=.\jail.h
# End Source File
# Begin Source File

SOURCE=.\magic.h
# End Source File
# Begin Source File

SOURCE=.\mapstuff.h
# End Source File
# Begin Source File

SOURCE=.\movement.h
# End Source File
# Begin Source File

SOURCE=.\msgboard.h
# End Source File
# Begin Source File

SOURCE=.\regions.h
# End Source File
# Begin Source File

SOURCE=.\skills.h
# End Source File
# Begin Source File

SOURCE=.\teffect.h
# End Source File
# Begin Source File

SOURCE=.\weight.h
# End Source File
# Begin Source File

SOURCE=.\wholist.h
# End Source File
# Begin Source File

SOURCE=.\worldmain.h
# End Source File
# End Group
# Begin Group "Other No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cdice.h
# End Source File
# Begin Source File

SOURCE=.\classes.h
# End Source File
# Begin Source File

SOURCE=.\cmdtable.h
# End Source File
# Begin Source File

SOURCE=.\CResponse.h
# End Source File
# Begin Source File

SOURCE=.\cSkillClass.h
# End Source File
# Begin Source File

SOURCE=.\cSpawnRegion.h
# End Source File
# Begin Source File

SOURCE=.\cVersionClass.h
# End Source File
# Begin Source File

SOURCE=.\enums.h
# End Source File
# Begin Source File

SOURCE=.\fileio.h
# End Source File
# Begin Source File

SOURCE=.\mapclasses.h
# End Source File
# Begin Source File

SOURCE=.\power.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\stream.h
# End Source File
# Begin Source File

SOURCE=.\townregion.h
# End Source File
# Begin Source File

SOURCE=.\typedefs.h
# End Source File
# Begin Source File

SOURCE=.\uox3.h
# End Source File
# End Group
# Begin Group "JS Engine Nr. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CJSEngine.h
# End Source File
# Begin Source File

SOURCE=.\CJSMapping.h
# End Source File
# Begin Source File

SOURCE=.\cScript.h
# End Source File
# Begin Source File

SOURCE=.\JSEncapsulate.h
# End Source File
# Begin Source File

SOURCE=.\SEFunctions.h
# End Source File
# Begin Source File

SOURCE=.\UOXJSClasses.h
# End Source File
# Begin Source File

SOURCE=.\UOXJSMethods.h
# End Source File
# Begin Source File

SOURCE=.\UOXJSPropertyEnums.h
# End Source File
# Begin Source File

SOURCE=.\UOXJSPropertyFuncs.h
# End Source File
# Begin Source File

SOURCE=.\UOXJSPropertySpecs.h
# End Source File
# End Group
# Begin Group "Network No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CPacketReceive.h
# End Source File
# Begin Source File

SOURCE=.\CPacketSend.h
# End Source File
# Begin Source File

SOURCE=.\network.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\CDataList.h
# End Source File
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\ObjectFactory.h
# End Source File
# Begin Source File

SOURCE=.\Platform.h
# End Source File
# Begin Source File

SOURCE=.\Prerequisites.h
# End Source File
# Begin Source File

SOURCE=.\Singleton.h
# End Source File
# Begin Source File

SOURCE=.\UOXStdHeaders.h
# End Source File
# Begin Source File

SOURCE=.\ustring.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\uox3.ico
# End Source File
# Begin Source File

SOURCE=.\uox3.rc
# End Source File
# End Target
# End Project
