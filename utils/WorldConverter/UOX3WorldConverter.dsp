# Microsoft Developer Studio Project File - Name="UOX3WorldConverter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=UOX3WorldConverter - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "UOX3WorldConverter.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "UOX3WorldConverter.mak" CFG="UOX3WorldConverter - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "UOX3WorldConverter - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "UOX3WorldConverter - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "UOX3WorldConverter - Win32 DebugProfile" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/UOX3", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UOX3WorldConverter - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "__NEW_NETWORK__" /D "__USE_CMULTIOBJ__" /YX /J /FD /c
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ole32.lib oleaut32.lib wsock32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ws2_32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "UOX3WorldConverter - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "__NEW_NETWORK__" /D "__USE_CMULTIOBJ__" /D "__NEW_PATHFIND__" /FR /YX /J /FD /c
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winspool.lib ole32.lib oleaut32.lib odbc32.lib odbccp32.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "UOX3WorldConverter - Win32 DebugProfile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "UOX3WorldConverter___Win32_DebugProfile"
# PROP BASE Intermediate_Dir "UOX3WorldConverter___Win32_DebugProfile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UOX3WorldConverter___Win32_DebugProfile"
# PROP Intermediate_Dir "UOX3WorldConverter___Win32_DebugProfile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "__NEW_NETWORK__" /D "__USE_CMULTIOBJ__" /FR /YX /J /FD /c
# ADD CPP /nologo /MTd /W3 /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "__NEW_NETWORK__" /D "__USE_CMULTIOBJ__" /FR /YX /J /FD /c
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winspool.lib ole32.lib oleaut32.lib odbc32.lib odbccp32.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 winspool.lib ole32.lib oleaut32.lib odbc32.lib odbccp32.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /subsystem:console /profile /debug /machine:I386

!ENDIF 

# Begin Target

# Name "UOX3WorldConverter - Win32 Release"
# Name "UOX3WorldConverter - Win32 Debug"
# Name "UOX3WorldConverter - Win32 DebugProfile"
# Begin Source File

SOURCE=.\cBaseobject.cpp
# End Source File
# Begin Source File

SOURCE=.\cBaseObject.h
# End Source File
# Begin Source File

SOURCE=.\cChar.cpp
# End Source File
# Begin Source File

SOURCE=.\CChar.h
# End Source File
# Begin Source File

SOURCE=.\charhandle.cpp
# End Source File
# Begin Source File

SOURCE=.\cItem.cpp
# End Source File
# Begin Source File

SOURCE=.\CItem.h
# End Source File
# Begin Source File

SOURCE=.\classes.h
# End Source File
# Begin Source File

SOURCE=.\cMultiObj.cpp
# End Source File
# Begin Source File

SOURCE=.\CMultiObj.h
# End Source File
# Begin Source File

SOURCE=.\enums.h
# End Source File
# Begin Source File

SOURCE=.\funcdecl.h
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\handlers.h
# End Source File
# Begin Source File

SOURCE=.\hash.cpp
# End Source File
# Begin Source File

SOURCE=.\hash.h
# End Source File
# Begin Source File

SOURCE=.\itemhandler.cpp
# End Source File
# Begin Source File

SOURCE=.\power.h
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

SOURCE=.\uoxstruct.h
# End Source File
# End Target
# End Project
