# Microsoft Developer Studio Project File - Name="UOX" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=UOX - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UOX.MAK".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UOX.MAK" CFG="UOX - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UOX - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "UOX - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UOX - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\"
# PROP Intermediate_Dir "..\obj\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G5 /Gr /W3 /GX /I "..\h\\" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /J /FD /c
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
# SUBTRACT RSC /x
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:console /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "UOX - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin\"
# PROP Intermediate_Dir "..\obj\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /I "..\h\\" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /J /FD /c
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
# SUBTRACT RSC /x
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"../obj/UOX.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /profile /debug /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "UOX - Win32 Release"
# Name "UOX - Win32 Debug"
# Begin Source File

SOURCE=..\cpp\admin.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\archive.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\boats.cpp
# End Source File
# Begin Source File

SOURCE=..\h\boats.h
# End Source File
# Begin Source File

SOURCE=..\cpp\books.cpp
# End Source File
# Begin Source File

SOURCE=..\h\books.h
# End Source File
# Begin Source File

SOURCE=..\h\classes.h
# End Source File
# Begin Source File

SOURCE=..\cpp\cmdtable.cpp
# End Source File
# Begin Source File

SOURCE=..\h\cmdtable.h
# End Source File
# Begin Source File

SOURCE=..\cpp\combat.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\commands.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\craces.cpp
# End Source File
# Begin Source File

SOURCE=..\h\craces.h
# End Source File
# Begin Source File

SOURCE=..\cpp\cweather.cpp
# End Source File
# Begin Source File

SOURCE=..\h\cweather.h
# End Source File
# Begin Source File

SOURCE=..\cpp\dbl_single_click.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\h\debug.h
# End Source File
# Begin Source File

SOURCE=..\cpp\fileio.cpp
# End Source File
# Begin Source File

SOURCE=..\h\fileio.h
# End Source File
# Begin Source File

SOURCE=..\cpp\globals.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\guildstones.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\gumps.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\house.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\html.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\im.cpp
# End Source File
# Begin Source File

SOURCE=..\h\im.h
# End Source File
# Begin Source File

SOURCE=..\cpp\items.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\magic.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\mapstuff.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\msgboard.cpp
# End Source File
# Begin Source File

SOURCE=..\h\msgboard.h
# End Source File
# Begin Source File

SOURCE=..\cpp\necro.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\network.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\newbie.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\npcs.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\p_ai.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\pointer.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\regions.cpp
# End Source File
# Begin Source File

SOURCE=..\h\regions.h
# End Source File
# Begin Source File

SOURCE=..\h\resource.h
# End Source File
# Begin Source File

SOURCE=..\cpp\scriptc.cpp
# End Source File
# Begin Source File

SOURCE=..\h\scriptc.h
# End Source File
# Begin Source File

SOURCE=..\cpp\skills.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\speech.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\targeting.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\teffect.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\townstones.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\trigger.cpp
# End Source File
# Begin Source File

SOURCE=..\h\typedefs.h
# End Source File
# Begin Source File

SOURCE=..\cpp\uox3.cpp
# End Source File
# Begin Source File

SOURCE=..\h\uox3.h
# End Source File
# Begin Source File

SOURCE=..\obj\uox3.ico
# End Source File
# Begin Source File

SOURCE=..\obj\uox3.rc
# End Source File
# Begin Source File

SOURCE=..\h\uoxlist.h
# End Source File
# Begin Source File

SOURCE=..\h\uoxstruct.h
# End Source File
# Begin Source File

SOURCE=..\cpp\walking.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\weight.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\wholist.cpp
# End Source File
# Begin Source File

SOURCE=..\cpp\worldmain.cpp
# End Source File
# Begin Source File

SOURCE=..\h\worldmain.h
# End Source File
# Begin Source File

SOURCE=..\cpp\xgm.cpp
# End Source File
# Begin Source File

SOURCE=..\h\xgm.h
# End Source File
# End Target
# End Project
