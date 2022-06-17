#ifndef __CDICE__
#define __CDICE__

#include <string>

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
class cBooks;
class CBoatObj;
class CChar;
class cCharStuff;
class cCommands;
class CConsole;
class cDice;
class CDictionary;
class CDictionaryContainer;
class cDirectoryListing;
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
class cMagic;
class CMagicMove;
class CMagicStat;
class CMapHandler;
class CMapRegion;
class CMulHandler;
class cMovement;
class CMultiObj;
class cNetworkStuff;
class CPUOXBuffer;
class CPInputBuffer;
class cRaces;
class CRace;
class cScript;
class CServerData;
class CServerDefinitions;
class CServerProfile;
class cSkillClass;
class cSkills;
class CSocket;
class CSpawnItem;
class CSpawnRegion;
class CSpeechEntry;
class CSpeechQueue;
class CTEffect;
class CThreadQueue;
class CTile;
struct tile_t ;
class CTownRegion;
class CVersionClass;
class CWeather;
class cWeatherAb;
class CWeight;
class cWhoList;
class CWorldMain;
class GumpDisplay;
class HelpRequest;
class JailCell;
class JailSystem;
class ObjectFactory;
class Script;
class ScriptSection;
class SpellInfo;

/** In order to avoid finger-aches :)
 */
#include "enums.h"
#include "typedefs.h"
class cDice
{
private:
	SI32	dice;
	SI32	sides;
	SI32	addition;

	bool	convStringToDice( std::string dieString );
public:
	cDice();
	cDice( const std::string &dieString );
	cDice( SI32 d, SI32 s, SI32 a );
	~cDice();

	SI32	roll( void );
	void	setDice( SI32 newDice );
	void	setSides( SI32 newSides );
	void	setAddition( SI32 newAddition );
};

#endif
