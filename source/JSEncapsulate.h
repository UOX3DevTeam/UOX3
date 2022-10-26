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

enum JSEncapsObjectType
{
	JSOT_INT = 0,
	JSOT_DOUBLE,
	JSOT_BOOL,
	JSOT_STRING,
	JSOT_OBJECT,
	JSOT_NULL,
	JSOT_VOID,
	JSOT_COUNT
};
class JSEncapsulate
{
public:

	JSEncapsulate( JSContext *jsCX, jsval *jsVP );
	JSEncapsulate( JSContext *jsCX, JSObject *jsVP );
	JSEncapsulate();
	void		SetContext( JSContext *jsCX, jsval *jsVP );
	bool		isType( JSEncapsObjectType toCheck );
	SI32		toInt( void );
	bool		toBool( void );
	R32			toFloat( void );
	std::string toString( void );
	void *		toObject( void );

	std::string	ClassName( void );
private:

	void				InternalReset( void );
	void				Init( void );
	bool				beenParsed[JSOT_COUNT+1];
	JSEncapsObjectType	nativeType;

	SI32				intVal;
	R32					floatVal;
	bool				boolVal;
	std::string			stringVal;
	void *				objectVal;

	JSContext *			cx;
	jsval *				vp;
	JSObject *			obj;

	std::string			className;
	bool				classCached;

	void Parse( JSEncapsObjectType typeConvert );
};
