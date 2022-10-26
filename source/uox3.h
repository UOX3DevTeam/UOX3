//o------------------------------------------------------------------------------------------------o
//| File		-	UOX3.h
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	This is the main UOX3 header file
//o------------------------------------------------------------------------------------------------o
//| Notes		-	Version History
//|					1.5		Added comments and changed int variables into typefefs
//|							 5. November 2001)
//|					1.6		Removed the warning promotions from level 3 to 4 off.
//|							022602)
//|					1.7		Most all global includes are now located here (rather than included in other global includes)
//|							Early class declarations to allow removal of most files from global scope
//|							Removed a majority of header files from global scope
//|							Removed some unneeded / duplicate external header includes
//|							Moved or Removed many global variables (many to cWorldMain class)
//|							4/01/2003 )
//|					1.8		Moved most class assignments out of global scope, they are now extern'd
//|							In their associated files (IE extern cBoat *Boats is in cBoat.h).
//|							Removed many early class declarations that were no longer necessary.
//o------------------------------------------------------------------------------------------------o


//o------------------------------------------------------------------------------------------------o
// Only run once
//o------------------------------------------------------------------------------------------------o
#ifndef __UOX3_H
#define __UOX3_H

#include "Prerequisites.h"
/* Include all the standard header *after* all the configuration
 settings have been made.
 */
#include "UOXStdHeaders.h"


// Pre-declare classes
// Allows use of pointers in header files without including individual .h
// so decreases dependencies between files
class CBaseTile;
class cAccountClass;
class CBaseObject;
class CBooks;
class CBoatObj;
class CChar;
class CCharStuff;
class CCommands;
class CConsole;
class cDice;
class CDictionary;
class CDictionaryContainer;
class CDirectoryListing;
class CEndL;
class cEffects;
class CEnvoke;
class CGuild;
class CGuildCollection;
class CGump;
class cHTMLTemplate;
class cHTMLTemplates;
class cItem;
class CItem;
class CJSMapping;
class CJSMappingSection;
class CLand;
class CMagic;
class CMagicMove;
class CMagicStat;
class CMapHandler;
class CMapRegion;
class CMulHandler;
class CMovement;
class CMultiObj;
class CNetworkStuff;
class CPUOXBuffer;
class CPInputBuffer;
class cRaces;
class CRace;
class cScript;
class CServerData;
class CServerDefinitions;
class CServerProfile;
class cSkillClass;
class CSkills;
class CSocket;
class CSpawnItem;
class CSpawnRegion;
class CSpeechEntry;
class CSpeechQueue;
class CTEffect;
class CThreadQueue;
class CTile;
struct Tile_st;
class CTownRegion;
class CVersionClass;
class CWeather;
class cWeatherAb;
class CWeight;
class CWhoList;
class CWorldMain;
class CGumpDisplay;
class CHelpRequest;
class CJailCell;
class CJailSystem;
class ObjectFactory;
class Script;
class CScriptSection;
class CSpellInfo;

/** In order to avoid finger-aches :)
 */
#include "enums.h"
#include "typedefs.h"
//o------------------------------------------------------------------------------------------------o
// Class Declarations
//o------------------------------------------------------------------------------------------------o


//o------------------------------------------------------------------------------------------------o
// The UOX project includes
//o------------------------------------------------------------------------------------------------o
#include "uoxstruct.h"
#include "GenericList.h"
#include "cAccountClass.h"
#include "cBaseObject.h"
#include "cChar.h"
#include "cItem.h"
#include "cMultiObj.h"
#include "cConsole.h"
#include "cSocket.h"
#include "cServerData.h"
#include "worldmain.h"
#include "funcdecl.h"


#endif // __UOX3_H
