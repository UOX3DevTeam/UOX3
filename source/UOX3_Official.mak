# Microsoft Developer Studio Generated NMAKE File, Based on UOX3_Official.dsp
!IF "$(CFG)" == ""
CFG=UOX3_Official - Win32 Debug
!MESSAGE No configuration specified. Defaulting to UOX3_Official - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "UOX3_Official - Win32 Release" && "$(CFG)" != "UOX3_Official - Win32 Debug" && "$(CFG)" != "UOX3_Official - Win32 DebugProfile"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "UOX3_Official - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\UOX3.exe" ".\Debug\UOX3_Official.bsc"


CLEAN :
	-@erase "$(INTDIR)\ai.obj"
	-@erase "$(INTDIR)\ai.sbr"
	-@erase "$(INTDIR)\archive.obj"
	-@erase "$(INTDIR)\archive.sbr"
	-@erase "$(INTDIR)\boats.obj"
	-@erase "$(INTDIR)\boats.sbr"
	-@erase "$(INTDIR)\books.obj"
	-@erase "$(INTDIR)\books.sbr"
	-@erase "$(INTDIR)\cAccountClass.obj"
	-@erase "$(INTDIR)\cAccountClass.sbr"
	-@erase "$(INTDIR)\calcfuncs.obj"
	-@erase "$(INTDIR)\calcfuncs.sbr"
	-@erase "$(INTDIR)\cBaseobject.obj"
	-@erase "$(INTDIR)\cBaseobject.sbr"
	-@erase "$(INTDIR)\cChar.obj"
	-@erase "$(INTDIR)\cChar.sbr"
	-@erase "$(INTDIR)\cClick.obj"
	-@erase "$(INTDIR)\cClick.sbr"
	-@erase "$(INTDIR)\cConsole.obj"
	-@erase "$(INTDIR)\cConsole.sbr"
	-@erase "$(INTDIR)\cDice.obj"
	-@erase "$(INTDIR)\cDice.sbr"
	-@erase "$(INTDIR)\cGuild.obj"
	-@erase "$(INTDIR)\cGuild.sbr"
	-@erase "$(INTDIR)\CGump.obj"
	-@erase "$(INTDIR)\CGump.sbr"
	-@erase "$(INTDIR)\charhandle.obj"
	-@erase "$(INTDIR)\charhandle.sbr"
	-@erase "$(INTDIR)\cHTMLSystem.obj"
	-@erase "$(INTDIR)\cHTMLSystem.sbr"
	-@erase "$(INTDIR)\cItem.obj"
	-@erase "$(INTDIR)\cItem.sbr"
	-@erase "$(INTDIR)\cmdtable.obj"
	-@erase "$(INTDIR)\cmdtable.sbr"
	-@erase "$(INTDIR)\cMultiObj.obj"
	-@erase "$(INTDIR)\cMultiObj.sbr"
	-@erase "$(INTDIR)\combat.obj"
	-@erase "$(INTDIR)\combat.sbr"
	-@erase "$(INTDIR)\commands.obj"
	-@erase "$(INTDIR)\commands.sbr"
	-@erase "$(INTDIR)\cRaces.obj"
	-@erase "$(INTDIR)\cRaces.sbr"
	-@erase "$(INTDIR)\cScript.obj"
	-@erase "$(INTDIR)\cScript.sbr"
	-@erase "$(INTDIR)\cServerData.obj"
	-@erase "$(INTDIR)\cServerData.sbr"
	-@erase "$(INTDIR)\cServerDefinitions.obj"
	-@erase "$(INTDIR)\cServerDefinitions.sbr"
	-@erase "$(INTDIR)\cSocket.obj"
	-@erase "$(INTDIR)\cSocket.sbr"
	-@erase "$(INTDIR)\cSpawnRegion.obj"
	-@erase "$(INTDIR)\cSpawnRegion.sbr"
	-@erase "$(INTDIR)\cStringTools.obj"
	-@erase "$(INTDIR)\cStringTools.sbr"
	-@erase "$(INTDIR)\cThreadQueue.obj"
	-@erase "$(INTDIR)\cThreadQueue.sbr"
	-@erase "$(INTDIR)\cVersionClass.obj"
	-@erase "$(INTDIR)\cVersionClass.sbr"
	-@erase "$(INTDIR)\cWeather.obj"
	-@erase "$(INTDIR)\cWeather.sbr"
	-@erase "$(INTDIR)\Dictionary.obj"
	-@erase "$(INTDIR)\Dictionary.sbr"
	-@erase "$(INTDIR)\dist.obj"
	-@erase "$(INTDIR)\dist.sbr"
	-@erase "$(INTDIR)\door.obj"
	-@erase "$(INTDIR)\door.sbr"
	-@erase "$(INTDIR)\effect.obj"
	-@erase "$(INTDIR)\effect.sbr"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\fileio.sbr"
	-@erase "$(INTDIR)\findfuncs.obj"
	-@erase "$(INTDIR)\findfuncs.sbr"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\globals.sbr"
	-@erase "$(INTDIR)\gumps.obj"
	-@erase "$(INTDIR)\gumps.sbr"
	-@erase "$(INTDIR)\house.obj"
	-@erase "$(INTDIR)\house.sbr"
	-@erase "$(INTDIR)\itemhandler.obj"
	-@erase "$(INTDIR)\itemhandler.sbr"
	-@erase "$(INTDIR)\items.obj"
	-@erase "$(INTDIR)\items.sbr"
	-@erase "$(INTDIR)\jail.obj"
	-@erase "$(INTDIR)\jail.sbr"
	-@erase "$(INTDIR)\lineofsight.obj"
	-@erase "$(INTDIR)\lineofsight.sbr"
	-@erase "$(INTDIR)\magic.obj"
	-@erase "$(INTDIR)\magic.sbr"
	-@erase "$(INTDIR)\mapstuff.obj"
	-@erase "$(INTDIR)\mapstuff.sbr"
	-@erase "$(INTDIR)\movement.obj"
	-@erase "$(INTDIR)\movement.sbr"
	-@erase "$(INTDIR)\msgboard.obj"
	-@erase "$(INTDIR)\msgboard.sbr"
	-@erase "$(INTDIR)\necro.obj"
	-@erase "$(INTDIR)\necro.sbr"
	-@erase "$(INTDIR)\network.obj"
	-@erase "$(INTDIR)\network.sbr"
	-@erase "$(INTDIR)\npcs.obj"
	-@erase "$(INTDIR)\npcs.sbr"
	-@erase "$(INTDIR)\packets.obj"
	-@erase "$(INTDIR)\packets.sbr"
	-@erase "$(INTDIR)\pcmanage.obj"
	-@erase "$(INTDIR)\pcmanage.sbr"
	-@erase "$(INTDIR)\quantityfuncs.obj"
	-@erase "$(INTDIR)\quantityfuncs.sbr"
	-@erase "$(INTDIR)\queue.obj"
	-@erase "$(INTDIR)\queue.sbr"
	-@erase "$(INTDIR)\regions.obj"
	-@erase "$(INTDIR)\regions.sbr"
	-@erase "$(INTDIR)\scriptc.obj"
	-@erase "$(INTDIR)\scriptc.sbr"
	-@erase "$(INTDIR)\SEFunctions.obj"
	-@erase "$(INTDIR)\SEFunctions.sbr"
	-@erase "$(INTDIR)\skills.obj"
	-@erase "$(INTDIR)\skills.sbr"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\sound.sbr"
	-@erase "$(INTDIR)\speech.obj"
	-@erase "$(INTDIR)\speech.sbr"
	-@erase "$(INTDIR)\ssection.obj"
	-@erase "$(INTDIR)\ssection.sbr"
	-@erase "$(INTDIR)\targeting.obj"
	-@erase "$(INTDIR)\targeting.sbr"
	-@erase "$(INTDIR)\teffect.obj"
	-@erase "$(INTDIR)\teffect.sbr"
	-@erase "$(INTDIR)\threadsafeobject.obj"
	-@erase "$(INTDIR)\threadsafeobject.sbr"
	-@erase "$(INTDIR)\townregion.obj"
	-@erase "$(INTDIR)\townregion.sbr"
	-@erase "$(INTDIR)\trade.obj"
	-@erase "$(INTDIR)\trade.sbr"
	-@erase "$(INTDIR)\trigger.obj"
	-@erase "$(INTDIR)\trigger.sbr"
	-@erase "$(INTDIR)\uox3.obj"
	-@erase "$(INTDIR)\uox3.res"
	-@erase "$(INTDIR)\uox3.sbr"
	-@erase "$(INTDIR)\UOXJSMethods.obj"
	-@erase "$(INTDIR)\UOXJSMethods.sbr"
	-@erase "$(INTDIR)\UOXJSPropertyFuncs.obj"
	-@erase "$(INTDIR)\UOXJSPropertyFuncs.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vendor.obj"
	-@erase "$(INTDIR)\vendor.sbr"
	-@erase "$(INTDIR)\weight.obj"
	-@erase "$(INTDIR)\weight.sbr"
	-@erase "$(INTDIR)\wholist.obj"
	-@erase "$(INTDIR)\wholist.sbr"
	-@erase "$(INTDIR)\worldmain.obj"
	-@erase "$(INTDIR)\worldmain.sbr"
	-@erase "$(INTDIR)\xgm.obj"
	-@erase "$(INTDIR)\xgm.sbr"
	-@erase "$(OUTDIR)\UOX3.exe"
	-@erase ".\Debug\UOX3_Official.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "__NEW_NETWORK__" /D "__USE_CMULTIOBJ__" /D "__NEW_PATHFIND__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\UOX3_Official.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /J /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0xc09 /fo"$(INTDIR)\uox3.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"Debug/UOX3_Official.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Dictionary.sbr" \
	"$(INTDIR)\scriptc.sbr" \
	"$(INTDIR)\ssection.sbr" \
	"$(INTDIR)\cBaseobject.sbr" \
	"$(INTDIR)\cChar.sbr" \
	"$(INTDIR)\cConsole.sbr" \
	"$(INTDIR)\cGuild.sbr" \
	"$(INTDIR)\charhandle.sbr" \
	"$(INTDIR)\cItem.sbr" \
	"$(INTDIR)\cMultiObj.sbr" \
	"$(INTDIR)\cSocket.sbr" \
	"$(INTDIR)\cSpawnRegion.sbr" \
	"$(INTDIR)\cThreadQueue.sbr" \
	"$(INTDIR)\itemhandler.sbr" \
	"$(INTDIR)\queue.sbr" \
	"$(INTDIR)\threadsafeobject.sbr" \
	"$(INTDIR)\townregion.sbr" \
	"$(INTDIR)\boats.sbr" \
	"$(INTDIR)\books.sbr" \
	"$(INTDIR)\cAccountClass.sbr" \
	"$(INTDIR)\cClick.sbr" \
	"$(INTDIR)\CGump.sbr" \
	"$(INTDIR)\cHTMLSystem.sbr" \
	"$(INTDIR)\cmdtable.sbr" \
	"$(INTDIR)\combat.sbr" \
	"$(INTDIR)\commands.sbr" \
	"$(INTDIR)\cRaces.sbr" \
	"$(INTDIR)\cServerData.sbr" \
	"$(INTDIR)\cWeather.sbr" \
	"$(INTDIR)\gumps.sbr" \
	"$(INTDIR)\house.sbr" \
	"$(INTDIR)\items.sbr" \
	"$(INTDIR)\jail.sbr" \
	"$(INTDIR)\lineofsight.sbr" \
	"$(INTDIR)\magic.sbr" \
	"$(INTDIR)\mapstuff.sbr" \
	"$(INTDIR)\movement.sbr" \
	"$(INTDIR)\msgboard.sbr" \
	"$(INTDIR)\necro.sbr" \
	"$(INTDIR)\network.sbr" \
	"$(INTDIR)\quantityfuncs.sbr" \
	"$(INTDIR)\regions.sbr" \
	"$(INTDIR)\skills.sbr" \
	"$(INTDIR)\sound.sbr" \
	"$(INTDIR)\speech.sbr" \
	"$(INTDIR)\targeting.sbr" \
	"$(INTDIR)\teffect.sbr" \
	"$(INTDIR)\trade.sbr" \
	"$(INTDIR)\vendor.sbr" \
	"$(INTDIR)\weight.sbr" \
	"$(INTDIR)\wholist.sbr" \
	"$(INTDIR)\worldmain.sbr" \
	"$(INTDIR)\xgm.sbr" \
	"$(INTDIR)\ai.sbr" \
	"$(INTDIR)\archive.sbr" \
	"$(INTDIR)\calcfuncs.sbr" \
	"$(INTDIR)\cDice.sbr" \
	"$(INTDIR)\cServerDefinitions.sbr" \
	"$(INTDIR)\cVersionClass.sbr" \
	"$(INTDIR)\dist.sbr" \
	"$(INTDIR)\door.sbr" \
	"$(INTDIR)\effect.sbr" \
	"$(INTDIR)\fileio.sbr" \
	"$(INTDIR)\findfuncs.sbr" \
	"$(INTDIR)\globals.sbr" \
	"$(INTDIR)\npcs.sbr" \
	"$(INTDIR)\packets.sbr" \
	"$(INTDIR)\pcmanage.sbr" \
	"$(INTDIR)\trigger.sbr" \
	"$(INTDIR)\cScript.sbr" \
	"$(INTDIR)\SEFunctions.sbr" \
	"$(INTDIR)\UOXJSMethods.sbr" \
	"$(INTDIR)\UOXJSPropertyFuncs.sbr" \
	"$(INTDIR)\cStringTools.sbr" \
	"$(INTDIR)\uox3.sbr"

".\Debug\UOX3_Official.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=wsock32.lib kernel32.lib ws2_32.lib js32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\UOX3.pdb" /machine:I386 /out:"$(OUTDIR)\UOX3.exe" /libpath:"JavaScript" 
LINK32_OBJS= \
	"$(INTDIR)\Dictionary.obj" \
	"$(INTDIR)\scriptc.obj" \
	"$(INTDIR)\ssection.obj" \
	"$(INTDIR)\cBaseobject.obj" \
	"$(INTDIR)\cChar.obj" \
	"$(INTDIR)\cConsole.obj" \
	"$(INTDIR)\cGuild.obj" \
	"$(INTDIR)\charhandle.obj" \
	"$(INTDIR)\cItem.obj" \
	"$(INTDIR)\cMultiObj.obj" \
	"$(INTDIR)\cSocket.obj" \
	"$(INTDIR)\cSpawnRegion.obj" \
	"$(INTDIR)\cThreadQueue.obj" \
	"$(INTDIR)\itemhandler.obj" \
	"$(INTDIR)\queue.obj" \
	"$(INTDIR)\threadsafeobject.obj" \
	"$(INTDIR)\townregion.obj" \
	"$(INTDIR)\boats.obj" \
	"$(INTDIR)\books.obj" \
	"$(INTDIR)\cAccountClass.obj" \
	"$(INTDIR)\cClick.obj" \
	"$(INTDIR)\CGump.obj" \
	"$(INTDIR)\cHTMLSystem.obj" \
	"$(INTDIR)\cmdtable.obj" \
	"$(INTDIR)\combat.obj" \
	"$(INTDIR)\commands.obj" \
	"$(INTDIR)\cRaces.obj" \
	"$(INTDIR)\cServerData.obj" \
	"$(INTDIR)\cWeather.obj" \
	"$(INTDIR)\gumps.obj" \
	"$(INTDIR)\house.obj" \
	"$(INTDIR)\items.obj" \
	"$(INTDIR)\jail.obj" \
	"$(INTDIR)\lineofsight.obj" \
	"$(INTDIR)\magic.obj" \
	"$(INTDIR)\mapstuff.obj" \
	"$(INTDIR)\movement.obj" \
	"$(INTDIR)\msgboard.obj" \
	"$(INTDIR)\necro.obj" \
	"$(INTDIR)\network.obj" \
	"$(INTDIR)\quantityfuncs.obj" \
	"$(INTDIR)\regions.obj" \
	"$(INTDIR)\skills.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\speech.obj" \
	"$(INTDIR)\targeting.obj" \
	"$(INTDIR)\teffect.obj" \
	"$(INTDIR)\trade.obj" \
	"$(INTDIR)\vendor.obj" \
	"$(INTDIR)\weight.obj" \
	"$(INTDIR)\wholist.obj" \
	"$(INTDIR)\worldmain.obj" \
	"$(INTDIR)\xgm.obj" \
	"$(INTDIR)\ai.obj" \
	"$(INTDIR)\archive.obj" \
	"$(INTDIR)\calcfuncs.obj" \
	"$(INTDIR)\cDice.obj" \
	"$(INTDIR)\cServerDefinitions.obj" \
	"$(INTDIR)\cVersionClass.obj" \
	"$(INTDIR)\dist.obj" \
	"$(INTDIR)\door.obj" \
	"$(INTDIR)\effect.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\findfuncs.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\npcs.obj" \
	"$(INTDIR)\packets.obj" \
	"$(INTDIR)\pcmanage.obj" \
	"$(INTDIR)\trigger.obj" \
	"$(INTDIR)\cScript.obj" \
	"$(INTDIR)\SEFunctions.obj" \
	"$(INTDIR)\UOXJSMethods.obj" \
	"$(INTDIR)\UOXJSPropertyFuncs.obj" \
	"$(INTDIR)\cStringTools.obj" \
	"$(INTDIR)\uox3.obj" \
	"$(INTDIR)\uox3.res"

"$(OUTDIR)\UOX3.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "UOX3_Official - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\UOX3.exe" "$(OUTDIR)\UOX3_Official.bsc"


CLEAN :
	-@erase "$(INTDIR)\ai.obj"
	-@erase "$(INTDIR)\ai.sbr"
	-@erase "$(INTDIR)\archive.obj"
	-@erase "$(INTDIR)\archive.sbr"
	-@erase "$(INTDIR)\boats.obj"
	-@erase "$(INTDIR)\boats.sbr"
	-@erase "$(INTDIR)\books.obj"
	-@erase "$(INTDIR)\books.sbr"
	-@erase "$(INTDIR)\cAccountClass.obj"
	-@erase "$(INTDIR)\cAccountClass.sbr"
	-@erase "$(INTDIR)\calcfuncs.obj"
	-@erase "$(INTDIR)\calcfuncs.sbr"
	-@erase "$(INTDIR)\cBaseobject.obj"
	-@erase "$(INTDIR)\cBaseobject.sbr"
	-@erase "$(INTDIR)\cChar.obj"
	-@erase "$(INTDIR)\cChar.sbr"
	-@erase "$(INTDIR)\cClick.obj"
	-@erase "$(INTDIR)\cClick.sbr"
	-@erase "$(INTDIR)\cConsole.obj"
	-@erase "$(INTDIR)\cConsole.sbr"
	-@erase "$(INTDIR)\cDice.obj"
	-@erase "$(INTDIR)\cDice.sbr"
	-@erase "$(INTDIR)\cGuild.obj"
	-@erase "$(INTDIR)\cGuild.sbr"
	-@erase "$(INTDIR)\CGump.obj"
	-@erase "$(INTDIR)\CGump.sbr"
	-@erase "$(INTDIR)\charhandle.obj"
	-@erase "$(INTDIR)\charhandle.sbr"
	-@erase "$(INTDIR)\cHTMLSystem.obj"
	-@erase "$(INTDIR)\cHTMLSystem.sbr"
	-@erase "$(INTDIR)\cItem.obj"
	-@erase "$(INTDIR)\cItem.sbr"
	-@erase "$(INTDIR)\cmdtable.obj"
	-@erase "$(INTDIR)\cmdtable.sbr"
	-@erase "$(INTDIR)\cMultiObj.obj"
	-@erase "$(INTDIR)\cMultiObj.sbr"
	-@erase "$(INTDIR)\combat.obj"
	-@erase "$(INTDIR)\combat.sbr"
	-@erase "$(INTDIR)\commands.obj"
	-@erase "$(INTDIR)\commands.sbr"
	-@erase "$(INTDIR)\cRaces.obj"
	-@erase "$(INTDIR)\cRaces.sbr"
	-@erase "$(INTDIR)\cScript.obj"
	-@erase "$(INTDIR)\cScript.sbr"
	-@erase "$(INTDIR)\cServerData.obj"
	-@erase "$(INTDIR)\cServerData.sbr"
	-@erase "$(INTDIR)\cServerDefinitions.obj"
	-@erase "$(INTDIR)\cServerDefinitions.sbr"
	-@erase "$(INTDIR)\cSocket.obj"
	-@erase "$(INTDIR)\cSocket.sbr"
	-@erase "$(INTDIR)\cSpawnRegion.obj"
	-@erase "$(INTDIR)\cSpawnRegion.sbr"
	-@erase "$(INTDIR)\cStringTools.obj"
	-@erase "$(INTDIR)\cStringTools.sbr"
	-@erase "$(INTDIR)\cThreadQueue.obj"
	-@erase "$(INTDIR)\cThreadQueue.sbr"
	-@erase "$(INTDIR)\cVersionClass.obj"
	-@erase "$(INTDIR)\cVersionClass.sbr"
	-@erase "$(INTDIR)\cWeather.obj"
	-@erase "$(INTDIR)\cWeather.sbr"
	-@erase "$(INTDIR)\Dictionary.obj"
	-@erase "$(INTDIR)\Dictionary.sbr"
	-@erase "$(INTDIR)\dist.obj"
	-@erase "$(INTDIR)\dist.sbr"
	-@erase "$(INTDIR)\door.obj"
	-@erase "$(INTDIR)\door.sbr"
	-@erase "$(INTDIR)\effect.obj"
	-@erase "$(INTDIR)\effect.sbr"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\fileio.sbr"
	-@erase "$(INTDIR)\findfuncs.obj"
	-@erase "$(INTDIR)\findfuncs.sbr"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\globals.sbr"
	-@erase "$(INTDIR)\gumps.obj"
	-@erase "$(INTDIR)\gumps.sbr"
	-@erase "$(INTDIR)\house.obj"
	-@erase "$(INTDIR)\house.sbr"
	-@erase "$(INTDIR)\itemhandler.obj"
	-@erase "$(INTDIR)\itemhandler.sbr"
	-@erase "$(INTDIR)\items.obj"
	-@erase "$(INTDIR)\items.sbr"
	-@erase "$(INTDIR)\jail.obj"
	-@erase "$(INTDIR)\jail.sbr"
	-@erase "$(INTDIR)\lineofsight.obj"
	-@erase "$(INTDIR)\lineofsight.sbr"
	-@erase "$(INTDIR)\magic.obj"
	-@erase "$(INTDIR)\magic.sbr"
	-@erase "$(INTDIR)\mapstuff.obj"
	-@erase "$(INTDIR)\mapstuff.sbr"
	-@erase "$(INTDIR)\movement.obj"
	-@erase "$(INTDIR)\movement.sbr"
	-@erase "$(INTDIR)\msgboard.obj"
	-@erase "$(INTDIR)\msgboard.sbr"
	-@erase "$(INTDIR)\necro.obj"
	-@erase "$(INTDIR)\necro.sbr"
	-@erase "$(INTDIR)\network.obj"
	-@erase "$(INTDIR)\network.sbr"
	-@erase "$(INTDIR)\npcs.obj"
	-@erase "$(INTDIR)\npcs.sbr"
	-@erase "$(INTDIR)\packets.obj"
	-@erase "$(INTDIR)\packets.sbr"
	-@erase "$(INTDIR)\pcmanage.obj"
	-@erase "$(INTDIR)\pcmanage.sbr"
	-@erase "$(INTDIR)\quantityfuncs.obj"
	-@erase "$(INTDIR)\quantityfuncs.sbr"
	-@erase "$(INTDIR)\queue.obj"
	-@erase "$(INTDIR)\queue.sbr"
	-@erase "$(INTDIR)\regions.obj"
	-@erase "$(INTDIR)\regions.sbr"
	-@erase "$(INTDIR)\scriptc.obj"
	-@erase "$(INTDIR)\scriptc.sbr"
	-@erase "$(INTDIR)\SEFunctions.obj"
	-@erase "$(INTDIR)\SEFunctions.sbr"
	-@erase "$(INTDIR)\skills.obj"
	-@erase "$(INTDIR)\skills.sbr"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\sound.sbr"
	-@erase "$(INTDIR)\speech.obj"
	-@erase "$(INTDIR)\speech.sbr"
	-@erase "$(INTDIR)\ssection.obj"
	-@erase "$(INTDIR)\ssection.sbr"
	-@erase "$(INTDIR)\targeting.obj"
	-@erase "$(INTDIR)\targeting.sbr"
	-@erase "$(INTDIR)\teffect.obj"
	-@erase "$(INTDIR)\teffect.sbr"
	-@erase "$(INTDIR)\threadsafeobject.obj"
	-@erase "$(INTDIR)\threadsafeobject.sbr"
	-@erase "$(INTDIR)\townregion.obj"
	-@erase "$(INTDIR)\townregion.sbr"
	-@erase "$(INTDIR)\trade.obj"
	-@erase "$(INTDIR)\trade.sbr"
	-@erase "$(INTDIR)\trigger.obj"
	-@erase "$(INTDIR)\trigger.sbr"
	-@erase "$(INTDIR)\uox3.obj"
	-@erase "$(INTDIR)\uox3.res"
	-@erase "$(INTDIR)\uox3.sbr"
	-@erase "$(INTDIR)\UOXJSMethods.obj"
	-@erase "$(INTDIR)\UOXJSMethods.sbr"
	-@erase "$(INTDIR)\UOXJSPropertyFuncs.obj"
	-@erase "$(INTDIR)\UOXJSPropertyFuncs.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vendor.obj"
	-@erase "$(INTDIR)\vendor.sbr"
	-@erase "$(INTDIR)\weight.obj"
	-@erase "$(INTDIR)\weight.sbr"
	-@erase "$(INTDIR)\wholist.obj"
	-@erase "$(INTDIR)\wholist.sbr"
	-@erase "$(INTDIR)\worldmain.obj"
	-@erase "$(INTDIR)\worldmain.sbr"
	-@erase "$(INTDIR)\xgm.obj"
	-@erase "$(INTDIR)\xgm.sbr"
	-@erase "$(OUTDIR)\UOX3.exe"
	-@erase "$(OUTDIR)\UOX3.ilk"
	-@erase "$(OUTDIR)\UOX3.pdb"
	-@erase "$(OUTDIR)\UOX3_Official.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\UOX3_Official.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /J /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0xc09 /fo"$(INTDIR)\uox3.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UOX3_Official.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Dictionary.sbr" \
	"$(INTDIR)\scriptc.sbr" \
	"$(INTDIR)\ssection.sbr" \
	"$(INTDIR)\cBaseobject.sbr" \
	"$(INTDIR)\cChar.sbr" \
	"$(INTDIR)\cConsole.sbr" \
	"$(INTDIR)\cGuild.sbr" \
	"$(INTDIR)\charhandle.sbr" \
	"$(INTDIR)\cItem.sbr" \
	"$(INTDIR)\cMultiObj.sbr" \
	"$(INTDIR)\cSocket.sbr" \
	"$(INTDIR)\cSpawnRegion.sbr" \
	"$(INTDIR)\cThreadQueue.sbr" \
	"$(INTDIR)\itemhandler.sbr" \
	"$(INTDIR)\queue.sbr" \
	"$(INTDIR)\threadsafeobject.sbr" \
	"$(INTDIR)\townregion.sbr" \
	"$(INTDIR)\boats.sbr" \
	"$(INTDIR)\books.sbr" \
	"$(INTDIR)\cAccountClass.sbr" \
	"$(INTDIR)\cClick.sbr" \
	"$(INTDIR)\CGump.sbr" \
	"$(INTDIR)\cHTMLSystem.sbr" \
	"$(INTDIR)\cmdtable.sbr" \
	"$(INTDIR)\combat.sbr" \
	"$(INTDIR)\commands.sbr" \
	"$(INTDIR)\cRaces.sbr" \
	"$(INTDIR)\cServerData.sbr" \
	"$(INTDIR)\cWeather.sbr" \
	"$(INTDIR)\gumps.sbr" \
	"$(INTDIR)\house.sbr" \
	"$(INTDIR)\items.sbr" \
	"$(INTDIR)\jail.sbr" \
	"$(INTDIR)\lineofsight.sbr" \
	"$(INTDIR)\magic.sbr" \
	"$(INTDIR)\mapstuff.sbr" \
	"$(INTDIR)\movement.sbr" \
	"$(INTDIR)\msgboard.sbr" \
	"$(INTDIR)\necro.sbr" \
	"$(INTDIR)\network.sbr" \
	"$(INTDIR)\quantityfuncs.sbr" \
	"$(INTDIR)\regions.sbr" \
	"$(INTDIR)\skills.sbr" \
	"$(INTDIR)\sound.sbr" \
	"$(INTDIR)\speech.sbr" \
	"$(INTDIR)\targeting.sbr" \
	"$(INTDIR)\teffect.sbr" \
	"$(INTDIR)\trade.sbr" \
	"$(INTDIR)\vendor.sbr" \
	"$(INTDIR)\weight.sbr" \
	"$(INTDIR)\wholist.sbr" \
	"$(INTDIR)\worldmain.sbr" \
	"$(INTDIR)\xgm.sbr" \
	"$(INTDIR)\ai.sbr" \
	"$(INTDIR)\archive.sbr" \
	"$(INTDIR)\calcfuncs.sbr" \
	"$(INTDIR)\cDice.sbr" \
	"$(INTDIR)\cServerDefinitions.sbr" \
	"$(INTDIR)\cVersionClass.sbr" \
	"$(INTDIR)\dist.sbr" \
	"$(INTDIR)\door.sbr" \
	"$(INTDIR)\effect.sbr" \
	"$(INTDIR)\fileio.sbr" \
	"$(INTDIR)\findfuncs.sbr" \
	"$(INTDIR)\globals.sbr" \
	"$(INTDIR)\npcs.sbr" \
	"$(INTDIR)\packets.sbr" \
	"$(INTDIR)\pcmanage.sbr" \
	"$(INTDIR)\trigger.sbr" \
	"$(INTDIR)\cScript.sbr" \
	"$(INTDIR)\SEFunctions.sbr" \
	"$(INTDIR)\UOXJSMethods.sbr" \
	"$(INTDIR)\UOXJSPropertyFuncs.sbr" \
	"$(INTDIR)\cStringTools.sbr" \
	"$(INTDIR)\uox3.sbr"

"$(OUTDIR)\UOX3_Official.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=ws2_32.lib kernel32.lib advapi32.lib shell32.lib js32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\UOX3.pdb" /debug /machine:I386 /out:"$(OUTDIR)\UOX3.exe" /libpath:"JavaScript/" /libpath:"JavaScript" 
LINK32_OBJS= \
	"$(INTDIR)\Dictionary.obj" \
	"$(INTDIR)\scriptc.obj" \
	"$(INTDIR)\ssection.obj" \
	"$(INTDIR)\cBaseobject.obj" \
	"$(INTDIR)\cChar.obj" \
	"$(INTDIR)\cConsole.obj" \
	"$(INTDIR)\cGuild.obj" \
	"$(INTDIR)\charhandle.obj" \
	"$(INTDIR)\cItem.obj" \
	"$(INTDIR)\cMultiObj.obj" \
	"$(INTDIR)\cSocket.obj" \
	"$(INTDIR)\cSpawnRegion.obj" \
	"$(INTDIR)\cThreadQueue.obj" \
	"$(INTDIR)\itemhandler.obj" \
	"$(INTDIR)\queue.obj" \
	"$(INTDIR)\threadsafeobject.obj" \
	"$(INTDIR)\townregion.obj" \
	"$(INTDIR)\boats.obj" \
	"$(INTDIR)\books.obj" \
	"$(INTDIR)\cAccountClass.obj" \
	"$(INTDIR)\cClick.obj" \
	"$(INTDIR)\CGump.obj" \
	"$(INTDIR)\cHTMLSystem.obj" \
	"$(INTDIR)\cmdtable.obj" \
	"$(INTDIR)\combat.obj" \
	"$(INTDIR)\commands.obj" \
	"$(INTDIR)\cRaces.obj" \
	"$(INTDIR)\cServerData.obj" \
	"$(INTDIR)\cWeather.obj" \
	"$(INTDIR)\gumps.obj" \
	"$(INTDIR)\house.obj" \
	"$(INTDIR)\items.obj" \
	"$(INTDIR)\jail.obj" \
	"$(INTDIR)\lineofsight.obj" \
	"$(INTDIR)\magic.obj" \
	"$(INTDIR)\mapstuff.obj" \
	"$(INTDIR)\movement.obj" \
	"$(INTDIR)\msgboard.obj" \
	"$(INTDIR)\necro.obj" \
	"$(INTDIR)\network.obj" \
	"$(INTDIR)\quantityfuncs.obj" \
	"$(INTDIR)\regions.obj" \
	"$(INTDIR)\skills.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\speech.obj" \
	"$(INTDIR)\targeting.obj" \
	"$(INTDIR)\teffect.obj" \
	"$(INTDIR)\trade.obj" \
	"$(INTDIR)\vendor.obj" \
	"$(INTDIR)\weight.obj" \
	"$(INTDIR)\wholist.obj" \
	"$(INTDIR)\worldmain.obj" \
	"$(INTDIR)\xgm.obj" \
	"$(INTDIR)\ai.obj" \
	"$(INTDIR)\archive.obj" \
	"$(INTDIR)\calcfuncs.obj" \
	"$(INTDIR)\cDice.obj" \
	"$(INTDIR)\cServerDefinitions.obj" \
	"$(INTDIR)\cVersionClass.obj" \
	"$(INTDIR)\dist.obj" \
	"$(INTDIR)\door.obj" \
	"$(INTDIR)\effect.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\findfuncs.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\npcs.obj" \
	"$(INTDIR)\packets.obj" \
	"$(INTDIR)\pcmanage.obj" \
	"$(INTDIR)\trigger.obj" \
	"$(INTDIR)\cScript.obj" \
	"$(INTDIR)\SEFunctions.obj" \
	"$(INTDIR)\UOXJSMethods.obj" \
	"$(INTDIR)\UOXJSPropertyFuncs.obj" \
	"$(INTDIR)\cStringTools.obj" \
	"$(INTDIR)\uox3.obj" \
	"$(INTDIR)\uox3.res"

"$(OUTDIR)\UOX3.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "UOX3_Official - Win32 DebugProfile"

OUTDIR=.\DebugProfile
INTDIR=.\DebugProfile
# Begin Custom Macros
OutDir=.\DebugProfile
# End Custom Macros

ALL : "$(OUTDIR)\UOX3.exe" ".\Debug\UOX3_Official.bsc"


CLEAN :
	-@erase "$(INTDIR)\ai.obj"
	-@erase "$(INTDIR)\ai.sbr"
	-@erase "$(INTDIR)\archive.obj"
	-@erase "$(INTDIR)\archive.sbr"
	-@erase "$(INTDIR)\boats.obj"
	-@erase "$(INTDIR)\boats.sbr"
	-@erase "$(INTDIR)\books.obj"
	-@erase "$(INTDIR)\books.sbr"
	-@erase "$(INTDIR)\cAccountClass.obj"
	-@erase "$(INTDIR)\cAccountClass.sbr"
	-@erase "$(INTDIR)\calcfuncs.obj"
	-@erase "$(INTDIR)\calcfuncs.sbr"
	-@erase "$(INTDIR)\cBaseobject.obj"
	-@erase "$(INTDIR)\cBaseobject.sbr"
	-@erase "$(INTDIR)\cChar.obj"
	-@erase "$(INTDIR)\cChar.sbr"
	-@erase "$(INTDIR)\cClick.obj"
	-@erase "$(INTDIR)\cClick.sbr"
	-@erase "$(INTDIR)\cConsole.obj"
	-@erase "$(INTDIR)\cConsole.sbr"
	-@erase "$(INTDIR)\cDice.obj"
	-@erase "$(INTDIR)\cDice.sbr"
	-@erase "$(INTDIR)\cGuild.obj"
	-@erase "$(INTDIR)\cGuild.sbr"
	-@erase "$(INTDIR)\CGump.obj"
	-@erase "$(INTDIR)\CGump.sbr"
	-@erase "$(INTDIR)\charhandle.obj"
	-@erase "$(INTDIR)\charhandle.sbr"
	-@erase "$(INTDIR)\cHTMLSystem.obj"
	-@erase "$(INTDIR)\cHTMLSystem.sbr"
	-@erase "$(INTDIR)\cItem.obj"
	-@erase "$(INTDIR)\cItem.sbr"
	-@erase "$(INTDIR)\cmdtable.obj"
	-@erase "$(INTDIR)\cmdtable.sbr"
	-@erase "$(INTDIR)\cMultiObj.obj"
	-@erase "$(INTDIR)\cMultiObj.sbr"
	-@erase "$(INTDIR)\combat.obj"
	-@erase "$(INTDIR)\combat.sbr"
	-@erase "$(INTDIR)\commands.obj"
	-@erase "$(INTDIR)\commands.sbr"
	-@erase "$(INTDIR)\cRaces.obj"
	-@erase "$(INTDIR)\cRaces.sbr"
	-@erase "$(INTDIR)\cScript.obj"
	-@erase "$(INTDIR)\cScript.sbr"
	-@erase "$(INTDIR)\cServerData.obj"
	-@erase "$(INTDIR)\cServerData.sbr"
	-@erase "$(INTDIR)\cServerDefinitions.obj"
	-@erase "$(INTDIR)\cServerDefinitions.sbr"
	-@erase "$(INTDIR)\cSocket.obj"
	-@erase "$(INTDIR)\cSocket.sbr"
	-@erase "$(INTDIR)\cSpawnRegion.obj"
	-@erase "$(INTDIR)\cSpawnRegion.sbr"
	-@erase "$(INTDIR)\cStringTools.obj"
	-@erase "$(INTDIR)\cStringTools.sbr"
	-@erase "$(INTDIR)\cThreadQueue.obj"
	-@erase "$(INTDIR)\cThreadQueue.sbr"
	-@erase "$(INTDIR)\cVersionClass.obj"
	-@erase "$(INTDIR)\cVersionClass.sbr"
	-@erase "$(INTDIR)\cWeather.obj"
	-@erase "$(INTDIR)\cWeather.sbr"
	-@erase "$(INTDIR)\Dictionary.obj"
	-@erase "$(INTDIR)\Dictionary.sbr"
	-@erase "$(INTDIR)\dist.obj"
	-@erase "$(INTDIR)\dist.sbr"
	-@erase "$(INTDIR)\door.obj"
	-@erase "$(INTDIR)\door.sbr"
	-@erase "$(INTDIR)\effect.obj"
	-@erase "$(INTDIR)\effect.sbr"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\fileio.sbr"
	-@erase "$(INTDIR)\findfuncs.obj"
	-@erase "$(INTDIR)\findfuncs.sbr"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\globals.sbr"
	-@erase "$(INTDIR)\gumps.obj"
	-@erase "$(INTDIR)\gumps.sbr"
	-@erase "$(INTDIR)\house.obj"
	-@erase "$(INTDIR)\house.sbr"
	-@erase "$(INTDIR)\itemhandler.obj"
	-@erase "$(INTDIR)\itemhandler.sbr"
	-@erase "$(INTDIR)\items.obj"
	-@erase "$(INTDIR)\items.sbr"
	-@erase "$(INTDIR)\jail.obj"
	-@erase "$(INTDIR)\jail.sbr"
	-@erase "$(INTDIR)\lineofsight.obj"
	-@erase "$(INTDIR)\lineofsight.sbr"
	-@erase "$(INTDIR)\magic.obj"
	-@erase "$(INTDIR)\magic.sbr"
	-@erase "$(INTDIR)\mapstuff.obj"
	-@erase "$(INTDIR)\mapstuff.sbr"
	-@erase "$(INTDIR)\movement.obj"
	-@erase "$(INTDIR)\movement.sbr"
	-@erase "$(INTDIR)\msgboard.obj"
	-@erase "$(INTDIR)\msgboard.sbr"
	-@erase "$(INTDIR)\necro.obj"
	-@erase "$(INTDIR)\necro.sbr"
	-@erase "$(INTDIR)\network.obj"
	-@erase "$(INTDIR)\network.sbr"
	-@erase "$(INTDIR)\npcs.obj"
	-@erase "$(INTDIR)\npcs.sbr"
	-@erase "$(INTDIR)\packets.obj"
	-@erase "$(INTDIR)\packets.sbr"
	-@erase "$(INTDIR)\pcmanage.obj"
	-@erase "$(INTDIR)\pcmanage.sbr"
	-@erase "$(INTDIR)\quantityfuncs.obj"
	-@erase "$(INTDIR)\quantityfuncs.sbr"
	-@erase "$(INTDIR)\queue.obj"
	-@erase "$(INTDIR)\queue.sbr"
	-@erase "$(INTDIR)\regions.obj"
	-@erase "$(INTDIR)\regions.sbr"
	-@erase "$(INTDIR)\scriptc.obj"
	-@erase "$(INTDIR)\scriptc.sbr"
	-@erase "$(INTDIR)\SEFunctions.obj"
	-@erase "$(INTDIR)\SEFunctions.sbr"
	-@erase "$(INTDIR)\skills.obj"
	-@erase "$(INTDIR)\skills.sbr"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\sound.sbr"
	-@erase "$(INTDIR)\speech.obj"
	-@erase "$(INTDIR)\speech.sbr"
	-@erase "$(INTDIR)\ssection.obj"
	-@erase "$(INTDIR)\ssection.sbr"
	-@erase "$(INTDIR)\targeting.obj"
	-@erase "$(INTDIR)\targeting.sbr"
	-@erase "$(INTDIR)\teffect.obj"
	-@erase "$(INTDIR)\teffect.sbr"
	-@erase "$(INTDIR)\threadsafeobject.obj"
	-@erase "$(INTDIR)\threadsafeobject.sbr"
	-@erase "$(INTDIR)\townregion.obj"
	-@erase "$(INTDIR)\townregion.sbr"
	-@erase "$(INTDIR)\trade.obj"
	-@erase "$(INTDIR)\trade.sbr"
	-@erase "$(INTDIR)\trigger.obj"
	-@erase "$(INTDIR)\trigger.sbr"
	-@erase "$(INTDIR)\uox3.obj"
	-@erase "$(INTDIR)\uox3.res"
	-@erase "$(INTDIR)\uox3.sbr"
	-@erase "$(INTDIR)\UOXJSMethods.obj"
	-@erase "$(INTDIR)\UOXJSMethods.sbr"
	-@erase "$(INTDIR)\UOXJSPropertyFuncs.obj"
	-@erase "$(INTDIR)\UOXJSPropertyFuncs.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vendor.obj"
	-@erase "$(INTDIR)\vendor.sbr"
	-@erase "$(INTDIR)\weight.obj"
	-@erase "$(INTDIR)\weight.sbr"
	-@erase "$(INTDIR)\wholist.obj"
	-@erase "$(INTDIR)\wholist.sbr"
	-@erase "$(INTDIR)\worldmain.obj"
	-@erase "$(INTDIR)\worldmain.sbr"
	-@erase "$(INTDIR)\xgm.obj"
	-@erase "$(INTDIR)\xgm.sbr"
	-@erase "$(OUTDIR)\UOX3.exe"
	-@erase ".\Debug\UOX3_Official.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\UOX3_Official.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /J /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\uox3.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"Debug/UOX3_Official.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Dictionary.sbr" \
	"$(INTDIR)\scriptc.sbr" \
	"$(INTDIR)\ssection.sbr" \
	"$(INTDIR)\cBaseobject.sbr" \
	"$(INTDIR)\cChar.sbr" \
	"$(INTDIR)\cConsole.sbr" \
	"$(INTDIR)\cGuild.sbr" \
	"$(INTDIR)\charhandle.sbr" \
	"$(INTDIR)\cItem.sbr" \
	"$(INTDIR)\cMultiObj.sbr" \
	"$(INTDIR)\cSocket.sbr" \
	"$(INTDIR)\cSpawnRegion.sbr" \
	"$(INTDIR)\cThreadQueue.sbr" \
	"$(INTDIR)\itemhandler.sbr" \
	"$(INTDIR)\queue.sbr" \
	"$(INTDIR)\threadsafeobject.sbr" \
	"$(INTDIR)\townregion.sbr" \
	"$(INTDIR)\boats.sbr" \
	"$(INTDIR)\books.sbr" \
	"$(INTDIR)\cAccountClass.sbr" \
	"$(INTDIR)\cClick.sbr" \
	"$(INTDIR)\CGump.sbr" \
	"$(INTDIR)\cHTMLSystem.sbr" \
	"$(INTDIR)\cmdtable.sbr" \
	"$(INTDIR)\combat.sbr" \
	"$(INTDIR)\commands.sbr" \
	"$(INTDIR)\cRaces.sbr" \
	"$(INTDIR)\cServerData.sbr" \
	"$(INTDIR)\cWeather.sbr" \
	"$(INTDIR)\gumps.sbr" \
	"$(INTDIR)\house.sbr" \
	"$(INTDIR)\items.sbr" \
	"$(INTDIR)\jail.sbr" \
	"$(INTDIR)\lineofsight.sbr" \
	"$(INTDIR)\magic.sbr" \
	"$(INTDIR)\mapstuff.sbr" \
	"$(INTDIR)\movement.sbr" \
	"$(INTDIR)\msgboard.sbr" \
	"$(INTDIR)\necro.sbr" \
	"$(INTDIR)\network.sbr" \
	"$(INTDIR)\quantityfuncs.sbr" \
	"$(INTDIR)\regions.sbr" \
	"$(INTDIR)\skills.sbr" \
	"$(INTDIR)\sound.sbr" \
	"$(INTDIR)\speech.sbr" \
	"$(INTDIR)\targeting.sbr" \
	"$(INTDIR)\teffect.sbr" \
	"$(INTDIR)\trade.sbr" \
	"$(INTDIR)\vendor.sbr" \
	"$(INTDIR)\weight.sbr" \
	"$(INTDIR)\wholist.sbr" \
	"$(INTDIR)\worldmain.sbr" \
	"$(INTDIR)\xgm.sbr" \
	"$(INTDIR)\ai.sbr" \
	"$(INTDIR)\archive.sbr" \
	"$(INTDIR)\calcfuncs.sbr" \
	"$(INTDIR)\cDice.sbr" \
	"$(INTDIR)\cServerDefinitions.sbr" \
	"$(INTDIR)\cVersionClass.sbr" \
	"$(INTDIR)\dist.sbr" \
	"$(INTDIR)\door.sbr" \
	"$(INTDIR)\effect.sbr" \
	"$(INTDIR)\fileio.sbr" \
	"$(INTDIR)\findfuncs.sbr" \
	"$(INTDIR)\globals.sbr" \
	"$(INTDIR)\npcs.sbr" \
	"$(INTDIR)\packets.sbr" \
	"$(INTDIR)\pcmanage.sbr" \
	"$(INTDIR)\trigger.sbr" \
	"$(INTDIR)\cScript.sbr" \
	"$(INTDIR)\SEFunctions.sbr" \
	"$(INTDIR)\UOXJSMethods.sbr" \
	"$(INTDIR)\UOXJSPropertyFuncs.sbr" \
	"$(INTDIR)\cStringTools.sbr" \
	"$(INTDIR)\uox3.sbr"

".\Debug\UOX3_Official.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=winspool.lib ole32.lib oleaut32.lib odbc32.lib odbccp32.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib js32.lib /nologo /subsystem:console /profile /debug /machine:I386 /out:"$(OUTDIR)\UOX3.exe" /libpath:"JavaScript" 
LINK32_OBJS= \
	"$(INTDIR)\Dictionary.obj" \
	"$(INTDIR)\scriptc.obj" \
	"$(INTDIR)\ssection.obj" \
	"$(INTDIR)\cBaseobject.obj" \
	"$(INTDIR)\cChar.obj" \
	"$(INTDIR)\cConsole.obj" \
	"$(INTDIR)\cGuild.obj" \
	"$(INTDIR)\charhandle.obj" \
	"$(INTDIR)\cItem.obj" \
	"$(INTDIR)\cMultiObj.obj" \
	"$(INTDIR)\cSocket.obj" \
	"$(INTDIR)\cSpawnRegion.obj" \
	"$(INTDIR)\cThreadQueue.obj" \
	"$(INTDIR)\itemhandler.obj" \
	"$(INTDIR)\queue.obj" \
	"$(INTDIR)\threadsafeobject.obj" \
	"$(INTDIR)\townregion.obj" \
	"$(INTDIR)\boats.obj" \
	"$(INTDIR)\books.obj" \
	"$(INTDIR)\cAccountClass.obj" \
	"$(INTDIR)\cClick.obj" \
	"$(INTDIR)\CGump.obj" \
	"$(INTDIR)\cHTMLSystem.obj" \
	"$(INTDIR)\cmdtable.obj" \
	"$(INTDIR)\combat.obj" \
	"$(INTDIR)\commands.obj" \
	"$(INTDIR)\cRaces.obj" \
	"$(INTDIR)\cServerData.obj" \
	"$(INTDIR)\cWeather.obj" \
	"$(INTDIR)\gumps.obj" \
	"$(INTDIR)\house.obj" \
	"$(INTDIR)\items.obj" \
	"$(INTDIR)\jail.obj" \
	"$(INTDIR)\lineofsight.obj" \
	"$(INTDIR)\magic.obj" \
	"$(INTDIR)\mapstuff.obj" \
	"$(INTDIR)\movement.obj" \
	"$(INTDIR)\msgboard.obj" \
	"$(INTDIR)\necro.obj" \
	"$(INTDIR)\network.obj" \
	"$(INTDIR)\quantityfuncs.obj" \
	"$(INTDIR)\regions.obj" \
	"$(INTDIR)\skills.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\speech.obj" \
	"$(INTDIR)\targeting.obj" \
	"$(INTDIR)\teffect.obj" \
	"$(INTDIR)\trade.obj" \
	"$(INTDIR)\vendor.obj" \
	"$(INTDIR)\weight.obj" \
	"$(INTDIR)\wholist.obj" \
	"$(INTDIR)\worldmain.obj" \
	"$(INTDIR)\xgm.obj" \
	"$(INTDIR)\ai.obj" \
	"$(INTDIR)\archive.obj" \
	"$(INTDIR)\calcfuncs.obj" \
	"$(INTDIR)\cDice.obj" \
	"$(INTDIR)\cServerDefinitions.obj" \
	"$(INTDIR)\cVersionClass.obj" \
	"$(INTDIR)\dist.obj" \
	"$(INTDIR)\door.obj" \
	"$(INTDIR)\effect.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\findfuncs.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\npcs.obj" \
	"$(INTDIR)\packets.obj" \
	"$(INTDIR)\pcmanage.obj" \
	"$(INTDIR)\trigger.obj" \
	"$(INTDIR)\cScript.obj" \
	"$(INTDIR)\SEFunctions.obj" \
	"$(INTDIR)\UOXJSMethods.obj" \
	"$(INTDIR)\UOXJSPropertyFuncs.obj" \
	"$(INTDIR)\cStringTools.obj" \
	"$(INTDIR)\uox3.obj" \
	"$(INTDIR)\uox3.res"

"$(OUTDIR)\UOX3.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("UOX3_Official.dep")
!INCLUDE "UOX3_Official.dep"
!ELSE 
!MESSAGE Warning: cannot find "UOX3_Official.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "UOX3_Official - Win32 Release" || "$(CFG)" == "UOX3_Official - Win32 Debug" || "$(CFG)" == "UOX3_Official - Win32 DebugProfile"
SOURCE=.\Dictionary.cpp

"$(INTDIR)\Dictionary.obj"	"$(INTDIR)\Dictionary.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\scriptc.cpp

"$(INTDIR)\scriptc.obj"	"$(INTDIR)\scriptc.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ssection.cpp

"$(INTDIR)\ssection.obj"	"$(INTDIR)\ssection.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cBaseobject.cpp

"$(INTDIR)\cBaseobject.obj"	"$(INTDIR)\cBaseobject.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cChar.cpp

"$(INTDIR)\cChar.obj"	"$(INTDIR)\cChar.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cConsole.cpp

"$(INTDIR)\cConsole.obj"	"$(INTDIR)\cConsole.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cGuild.cpp

"$(INTDIR)\cGuild.obj"	"$(INTDIR)\cGuild.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\charhandle.cpp

"$(INTDIR)\charhandle.obj"	"$(INTDIR)\charhandle.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cItem.cpp

"$(INTDIR)\cItem.obj"	"$(INTDIR)\cItem.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cMultiObj.cpp

"$(INTDIR)\cMultiObj.obj"	"$(INTDIR)\cMultiObj.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cSocket.cpp

"$(INTDIR)\cSocket.obj"	"$(INTDIR)\cSocket.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cSpawnRegion.cpp

"$(INTDIR)\cSpawnRegion.obj"	"$(INTDIR)\cSpawnRegion.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cThreadQueue.cpp

"$(INTDIR)\cThreadQueue.obj"	"$(INTDIR)\cThreadQueue.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\itemhandler.cpp

"$(INTDIR)\itemhandler.obj"	"$(INTDIR)\itemhandler.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\queue.cpp

"$(INTDIR)\queue.obj"	"$(INTDIR)\queue.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\threadsafeobject.cpp

"$(INTDIR)\threadsafeobject.obj"	"$(INTDIR)\threadsafeobject.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\townregion.cpp

"$(INTDIR)\townregion.obj"	"$(INTDIR)\townregion.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\boats.cpp

"$(INTDIR)\boats.obj"	"$(INTDIR)\boats.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\books.cpp

"$(INTDIR)\books.obj"	"$(INTDIR)\books.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cAccountClass.cpp

"$(INTDIR)\cAccountClass.obj"	"$(INTDIR)\cAccountClass.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cClick.cpp

"$(INTDIR)\cClick.obj"	"$(INTDIR)\cClick.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CGump.cpp

"$(INTDIR)\CGump.obj"	"$(INTDIR)\CGump.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cHTMLSystem.cpp

"$(INTDIR)\cHTMLSystem.obj"	"$(INTDIR)\cHTMLSystem.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cmdtable.cpp

"$(INTDIR)\cmdtable.obj"	"$(INTDIR)\cmdtable.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\combat.cpp

"$(INTDIR)\combat.obj"	"$(INTDIR)\combat.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\commands.cpp

"$(INTDIR)\commands.obj"	"$(INTDIR)\commands.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cRaces.cpp

"$(INTDIR)\cRaces.obj"	"$(INTDIR)\cRaces.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cServerData.cpp

"$(INTDIR)\cServerData.obj"	"$(INTDIR)\cServerData.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cWeather.cpp

"$(INTDIR)\cWeather.obj"	"$(INTDIR)\cWeather.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gumps.cpp

"$(INTDIR)\gumps.obj"	"$(INTDIR)\gumps.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\house.cpp

"$(INTDIR)\house.obj"	"$(INTDIR)\house.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\items.cpp

"$(INTDIR)\items.obj"	"$(INTDIR)\items.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\jail.cpp

"$(INTDIR)\jail.obj"	"$(INTDIR)\jail.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\lineofsight.cpp

"$(INTDIR)\lineofsight.obj"	"$(INTDIR)\lineofsight.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\magic.cpp

"$(INTDIR)\magic.obj"	"$(INTDIR)\magic.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mapstuff.cpp

"$(INTDIR)\mapstuff.obj"	"$(INTDIR)\mapstuff.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\movement.cpp

"$(INTDIR)\movement.obj"	"$(INTDIR)\movement.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\msgboard.cpp

"$(INTDIR)\msgboard.obj"	"$(INTDIR)\msgboard.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\necro.cpp

"$(INTDIR)\necro.obj"	"$(INTDIR)\necro.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\network.cpp

"$(INTDIR)\network.obj"	"$(INTDIR)\network.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\quantityfuncs.cpp

"$(INTDIR)\quantityfuncs.obj"	"$(INTDIR)\quantityfuncs.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\regions.cpp

"$(INTDIR)\regions.obj"	"$(INTDIR)\regions.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\skills.cpp

"$(INTDIR)\skills.obj"	"$(INTDIR)\skills.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sound.cpp

"$(INTDIR)\sound.obj"	"$(INTDIR)\sound.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\speech.cpp

"$(INTDIR)\speech.obj"	"$(INTDIR)\speech.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\targeting.cpp

"$(INTDIR)\targeting.obj"	"$(INTDIR)\targeting.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\teffect.cpp

"$(INTDIR)\teffect.obj"	"$(INTDIR)\teffect.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\trade.cpp

"$(INTDIR)\trade.obj"	"$(INTDIR)\trade.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vendor.cpp

"$(INTDIR)\vendor.obj"	"$(INTDIR)\vendor.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\weight.cpp

"$(INTDIR)\weight.obj"	"$(INTDIR)\weight.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wholist.cpp

"$(INTDIR)\wholist.obj"	"$(INTDIR)\wholist.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\worldmain.cpp

"$(INTDIR)\worldmain.obj"	"$(INTDIR)\worldmain.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\xgm.cpp

"$(INTDIR)\xgm.obj"	"$(INTDIR)\xgm.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ai.cpp

"$(INTDIR)\ai.obj"	"$(INTDIR)\ai.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\archive.cpp

"$(INTDIR)\archive.obj"	"$(INTDIR)\archive.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\calcfuncs.cpp

"$(INTDIR)\calcfuncs.obj"	"$(INTDIR)\calcfuncs.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cDice.cpp

"$(INTDIR)\cDice.obj"	"$(INTDIR)\cDice.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cServerDefinitions.cpp

"$(INTDIR)\cServerDefinitions.obj"	"$(INTDIR)\cServerDefinitions.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cVersionClass.cpp

"$(INTDIR)\cVersionClass.obj"	"$(INTDIR)\cVersionClass.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dist.cpp

"$(INTDIR)\dist.obj"	"$(INTDIR)\dist.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\door.cpp

"$(INTDIR)\door.obj"	"$(INTDIR)\door.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\effect.cpp

"$(INTDIR)\effect.obj"	"$(INTDIR)\effect.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\fileio.cpp

"$(INTDIR)\fileio.obj"	"$(INTDIR)\fileio.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\findfuncs.cpp

"$(INTDIR)\findfuncs.obj"	"$(INTDIR)\findfuncs.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\globals.cpp

"$(INTDIR)\globals.obj"	"$(INTDIR)\globals.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\npcs.cpp

"$(INTDIR)\npcs.obj"	"$(INTDIR)\npcs.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\packets.cpp

"$(INTDIR)\packets.obj"	"$(INTDIR)\packets.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pcmanage.cpp

"$(INTDIR)\pcmanage.obj"	"$(INTDIR)\pcmanage.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\trigger.cpp

"$(INTDIR)\trigger.obj"	"$(INTDIR)\trigger.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cScript.cpp

"$(INTDIR)\cScript.obj"	"$(INTDIR)\cScript.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SEFunctions.cpp

"$(INTDIR)\SEFunctions.obj"	"$(INTDIR)\SEFunctions.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\UOXJSMethods.cpp

"$(INTDIR)\UOXJSMethods.obj"	"$(INTDIR)\UOXJSMethods.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\UOXJSPropertyFuncs.cpp

"$(INTDIR)\UOXJSPropertyFuncs.obj"	"$(INTDIR)\UOXJSPropertyFuncs.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cStringTools.cpp

"$(INTDIR)\cStringTools.obj"	"$(INTDIR)\cStringTools.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\uox3.cpp

"$(INTDIR)\uox3.obj"	"$(INTDIR)\uox3.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\uox3.rc

"$(INTDIR)\uox3.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

