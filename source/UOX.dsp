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
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /J /FD /c
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "UOX - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /J /FD /c
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /profile /debug /machine:I386

!ENDIF 

# Begin Target

# Name "UOX - Win32 Release"
# Name "UOX - Win32 Debug"
# Begin Source File

SOURCE=.\admin.cpp
# End Source File
# Begin Source File

SOURCE=.\archive.cpp
# End Source File
# Begin Source File

SOURCE=.\boats.cpp
# End Source File
# Begin Source File

SOURCE=.\boats.h
# End Source File
# Begin Source File

SOURCE=.\books.cpp
# End Source File
# Begin Source File

SOURCE=.\books.h
# End Source File
# Begin Source File

SOURCE=.\classes.h
# End Source File
# Begin Source File

SOURCE=.\cmdtable.cpp
# End Source File
# Begin Source File

SOURCE=.\cmdtable.h
# End Source File
# Begin Source File

SOURCE=.\combat.cpp
# End Source File
# Begin Source File

SOURCE=.\commands.cpp
# End Source File
# Begin Source File

SOURCE=.\craces.cpp
# End Source File
# Begin Source File

SOURCE=.\craces.h
# End Source File
# Begin Source File

SOURCE=.\cshop.h
# End Source File
# Begin Source File

SOURCE=.\cweather.cpp
# End Source File
# Begin Source File

SOURCE=.\cweather.h
# End Source File
# Begin Source File

SOURCE=.\dbl_sinlge_click.cpp
# End Source File
# Begin Source File

SOURCE=.\debug.cpp
# End Source File
# Begin Source File

SOURCE=.\debug.h
# End Source File
# Begin Source File

SOURCE=.\fileio.cpp
# End Source File
# Begin Source File

SOURCE=.\fileio.h
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\guildstones.cpp
# End Source File
# Begin Source File

SOURCE=.\gumps.cpp
# End Source File
# Begin Source File

SOURCE=.\house.cpp
# End Source File
# Begin Source File

SOURCE=.\html.cpp
# End Source File
# Begin Source File

SOURCE=.\im.cpp
# End Source File
# Begin Source File

SOURCE=.\im.h
# End Source File
# Begin Source File

SOURCE=.\items.cpp

!IF  "$(CFG)" == "UOX - Win32 Release"

!ELSEIF  "$(CFG)" == "UOX - Win32 Debug"

# ADD CPP /W4

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\magic.cpp
# End Source File
# Begin Source File

SOURCE=.\mapstuff.cpp
# End Source File
# Begin Source File

SOURCE=.\msgboard.cpp
# End Source File
# Begin Source File

SOURCE=.\msgboard.h
# End Source File
# Begin Source File

SOURCE=.\necro.cpp
# End Source File
# Begin Source File

SOURCE=.\network.cpp
# End Source File
# Begin Source File

SOURCE=.\newbie.cpp
# End Source File
# Begin Source File

SOURCE=.\npcs.cpp
# End Source File
# Begin Source File

SOURCE=.\p_ai.cpp
# End Source File
# Begin Source File

SOURCE=.\pointer.cpp
# End Source File
# Begin Source File

SOURCE=.\regions.cpp
# End Source File
# Begin Source File

SOURCE=.\regions.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\scriptc.cpp
# End Source File
# Begin Source File

SOURCE=.\scriptc.h
# End Source File
# Begin Source File

SOURCE=.\skills.cpp
# End Source File
# Begin Source File

SOURCE=.\targeting.cpp
# End Source File
# Begin Source File

SOURCE=.\teffect.cpp
# End Source File
# Begin Source File

SOURCE=.\townstones.cpp
# End Source File
# Begin Source File

SOURCE=.\trigger.cpp
# End Source File
# Begin Source File

SOURCE=.\typedefs.h
# End Source File
# Begin Source File

SOURCE=.\uox3.cpp
# End Source File
# Begin Source File

SOURCE=.\uox3.h
# End Source File
# Begin Source File

SOURCE=.\uox3.ico
# End Source File
# Begin Source File

SOURCE=.\uox3.rc
# End Source File
# Begin Source File

SOURCE=.\uoxlist.h
# End Source File
# Begin Source File

SOURCE=.\uoxstruct.h
# End Source File
# Begin Source File

SOURCE=.\walking.cpp
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
# Begin Source File

SOURCE=.\worldmain.h
# End Source File
# Begin Source File

SOURCE=.\xgm.cpp
# End Source File
# Begin Source File

SOURCE=.\xgm.h
# End Source File
# End Target
# End Project
