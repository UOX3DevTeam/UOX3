
//o---------------------------------------------------------------------------o
//| ssection.h
//o---------------------------------------------------------------------------o
//| Script Section class implementation
//o---------------------------------------------------------------------------o

#include "uox3.h"
#include "ssection.h"

const UI08 DFN_STRING		= 0;
const UI08 DFN_NUMERIC		= 1;
const UI08 DFN_UPPERSTRING	= 2;
const UI08 DFN_NODATA		= 3;
const UI08 DFN_UNKNOWN		= 4;
const UI08 DFN_DOUBLENUMERIC= 5;

const UI08 dfnDataTypes[DFNTAG_COUNTOFTAGS] =
{
	DFN_NUMERIC,		//	DFNTAG_AC = 0,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ALCHEMY,
	DFN_NUMERIC,		//	DFNTAG_AMOUNT,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ANATOMY,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ANIMALLORE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ARCHERY,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ARMSLORE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ATT,
	DFN_NODATA,			//	DFNTAG_BACKPACK,
	DFN_DOUBLENUMERIC,	//	DFNTAG_BEGGING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_BLACKSMITHING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_BOWCRAFT,
	DFN_DOUBLENUMERIC,	//	DFNTAG_CAMPING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_CARPENTRY,
	DFN_DOUBLENUMERIC,	//	DFNTAG_CARTOGRAPHY,
	DFN_NUMERIC,		//	DFNTAG_CARVE,
	DFN_NUMERIC,		//	DFNTAG_COLD,
	DFN_NUMERIC,		//	DFNTAG_COLOUR,
	DFN_STRING,			//	DFNTAG_COLOURLIST,
	DFN_NODATA,			//	DFNTAG_COLOURMATCHHAIR,
	DFN_DOUBLENUMERIC,	//	DFNTAG_COOKING,
	DFN_NUMERIC,		//	DFNTAG_CORPSE,
	DFN_NUMERIC,		//	DFNTAG_CREATOR,
	DFN_DOUBLENUMERIC,	//	DFNTAG_DAMAGE,
	DFN_NUMERIC,		//	DFNTAG_DECAY,
	DFN_DOUBLENUMERIC,	//	DFNTAG_DEF,
	DFN_DOUBLENUMERIC,	//	DFNTAG_DETECTINGHIDDEN,
	DFN_DOUBLENUMERIC,	//	DFNTAG_DEX,
	DFN_NUMERIC,		//	DFNTAG_DEXADD,
	DFN_STRING,			//	DFNTAG_DIR,
	DFN_NODATA,			//	DFNTAG_DISPELLABLE,
	DFN_NUMERIC,		//	DFNTAG_DISABLED,
	DFN_NUMERIC,		//	DFNTAG_DOORFLAG,
	DFN_NUMERIC,		//	DFNTAG_DYE,
	DFN_NUMERIC,		//	DFNTAG_EMOTECOLOUR,
	DFN_NUMERIC,		//	DFNTAG_ENHANCED,
	DFN_DOUBLENUMERIC,	//	DFNTAG_ENTICEMENT,
	DFN_DOUBLENUMERIC,	//	DFNTAG_EVALUATINGINTEL,
	DFN_NUMERIC,		//	DFNTAG_FAME,
	DFN_DOUBLENUMERIC,	//	DFNTAG_FENCING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_FISHING,
	DFN_NUMERIC,		//	DFNTAG_FLEEAT,
	DFN_DOUBLENUMERIC,	//	DFNTAG_FORENSICS,
	DFN_NUMERIC,		//	DFNTAG_FX1,
	DFN_NUMERIC,		//	DFNTAG_FX2,
	DFN_NUMERIC,		//	DFNTAG_FY1,
	DFN_NUMERIC,		//	DFNTAG_FY2,
	DFN_NUMERIC,		//	DFNTAG_FZ1,
	DFN_UPPERSTRING,	//	DFNTAG_GET,
	DFN_NUMERIC,		//	DFNTAG_GLOW,
	DFN_NUMERIC,		//	DFNTAG_GLOWBC,
	DFN_NUMERIC,		//	DFNTAG_GLOWTYPE,
	DFN_NUMERIC,		//	DFNTAG_GOLD,
	DFN_NUMERIC,		//	DFNTAG_GOOD,
	DFN_STRING,			//	DFNTAG_HAIRCOLOUR,
	DFN_DOUBLENUMERIC,	//	DFNTAG_HEALING,
	DFN_NUMERIC,		//	DFNTAG_HEAT,
	DFN_DOUBLENUMERIC,	//	DFNTAG_HERDING,
	DFN_NUMERIC,		//	DFNTAG_HIDAMAGE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_HIDING,
	DFN_NUMERIC,		//	DFNTAG_HP,
	DFN_NUMERIC,		//	DFNTAG_ID,
	DFN_DOUBLENUMERIC,	//	DFNTAG_INTELLIGENCE,
	DFN_NUMERIC,		//	DFNTAG_INTADD,
	DFN_DOUBLENUMERIC,	//	DFNTAG_INSCRIPTION,
	DFN_UPPERSTRING,	//	DFNTAG_ITEM,
	DFN_NUMERIC,		//	DFNTAG_ITEMID,
	DFN_NUMERIC,		//	DFNTAG_KARMA,
	DFN_NUMERIC,		//	DFNTAG_LAYER,
	DFN_NUMERIC,		//	DFNTAG_LIGHT,
	DFN_NUMERIC,		//	DFNTAG_LIGHTNING,
	DFN_NUMERIC,		//	DFNTAG_LOCKPICKING,
	DFN_NUMERIC,		//	DFNTAG_LODAMAGE,
	DFN_UPPERSTRING,	//	DFNTAG_LOOT,
	DFN_DOUBLENUMERIC,	//	DFNTAG_LUMBERJACKING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MACEFIGHTING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MAGERY,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MAGICRESISTANCE,
	DFN_NUMERIC,		//	DFNTAG_MAXHP,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MEDITATION,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MINING,
	DFN_NUMERIC,		//	DFNTAG_MOVABLE,
	DFN_NUMERIC,		//	DFNTAG_MORE,
	DFN_NUMERIC,		//	DFNTAG_MORE2,
	DFN_NUMERIC,		//	DFNTAG_MOREX,
	DFN_NUMERIC,		//	DFNTAG_MOREY,
	DFN_NUMERIC,		//	DFNTAG_MOREZ,
	DFN_DOUBLENUMERIC,	//	DFNTAG_MUSICIANSHIP,
	DFN_STRING,			//	DFNTAG_NAME,
	DFN_STRING,			//	DFNTAG_NAME2,
	DFN_STRING,			//	DFNTAG_NAMELIST,
	DFN_NODATA,			//	DFNTAG_NEWBIE,
	DFN_NODATA,			//	DFNTAG_NOTRAIN,
	DFN_NUMERIC,		//	DFNTAG_NPCAI,
	DFN_NUMERIC,		//	DFNTAG_NPCWANDER,
	DFN_NUMERIC,		//	DFNTAG_OFFSPELL,
	DFN_STRING,			//	DFNTAG_PACKITEM,
	DFN_DOUBLENUMERIC,	//	DFNTAG_PARRYING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_PEACEMAKING,
	DFN_NUMERIC,		//	DFNTAG_PILEABLE,
	DFN_NUMERIC,		//	DFNTAG_POISON,
	DFN_NUMERIC,		//	DFNTAG_POISONED,
	DFN_DOUBLENUMERIC,	//	DFNTAG_POISONING,
	DFN_NUMERIC,		//	DFNTAG_PRIV,
	DFN_NUMERIC,		//	DFNTAG_PRIV1,
	DFN_NUMERIC,		//	DFNTAG_PRIV2,
	DFN_DOUBLENUMERIC,	//	DFNTAG_PROVOCATION,
	DFN_NUMERIC,		//	DFNTAG_RACE,
	DFN_NUMERIC,		//	DFNTAG_RAIN,
	DFN_NUMERIC,		//	DFNTAG_RANK,
	DFN_NUMERIC,		//	DFNTAG_REATTACKAT,
	DFN_DOUBLENUMERIC,	//	DFNTAG_REMOVETRAPS,
	DFN_NUMERIC,		//	DFNTAG_RESTOCK,
	DFN_STRING,			//	DFNTAG_RSHOPITEM,
	DFN_NODATA,			//	DFNTAG_RUNS,
	DFN_NUMERIC,		//	DFNTAG_SAYCOLOUR,
	DFN_NUMERIC,		//	DFNTAG_SCRIPT,
	DFN_STRING,			//	DFNTAG_SELLITEM,
	DFN_STRING,			//	DFNTAG_SHOPITEM,
	DFN_NODATA,			//	DFNTAG_SHOPKEEPER,
	DFN_STRING,			//	DFNTAG_SHOPLIST,
	DFN_NUMERIC,		//	DFNTAG_SK_MADE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_SKILL,
	DFN_UNKNOWN,		//	DFNTAG_SKILLVAR,
	DFN_NUMERIC,		//	DFNTAG_SKIN,
	DFN_STRING,			//	DFNTAG_SKINLIST,
	DFN_DOUBLENUMERIC,	//	DFNTAG_SNOOPING,
	DFN_NUMERIC,		//	DFNTAG_SNOW,
	DFN_NUMERIC,		//	DFNTAG_SPADELAY,
	DFN_NUMERIC,		//	DFNTAG_SPATTACK,
	DFN_STRING,			//	DFNTAG_SPAWNOBJ,
	DFN_STRING,			//	DFNTAG_SPAWNOBJLIST,
	DFN_NUMERIC,		//	DFNTAG_SPD,
	DFN_DOUBLENUMERIC,	//	DFNTAG_SPIRITSPEAK,
	DFN_NUMERIC,		//	DFNTAG_SPLIT,
	DFN_NUMERIC,		//	DFNTAG_SPLITCHANCE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_STRENGTH,
	DFN_NUMERIC,		//	DFNTAG_STRADD,
	DFN_DOUBLENUMERIC,	//	DFNTAG_STEALING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_STEALTH,
	DFN_DOUBLENUMERIC,	//	DFNTAG_SWORDSMANSHIP,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TACTICS,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TAILORING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TAMING,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TASTEID,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TINKERING,
	DFN_STRING,			//	DFNTAG_TITLE,
	DFN_NUMERIC,		//	DFNTAG_TOTAME,
	DFN_DOUBLENUMERIC,	//	DFNTAG_TRACKING,
	DFN_NUMERIC,		//	DFNTAG_TYPE,
	DFN_NUMERIC,		//	DFNTAG_TYPE2,
	DFN_NUMERIC,		//	DFNTAG_VALUE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_VETERINARY,
	DFN_NUMERIC,		//	DFNTAG_VISIBLE,
	DFN_NUMERIC,		//	DFNTAG_WEIGHT,
	DFN_NUMERIC,		//	DFNTAG_WIPE,
	DFN_DOUBLENUMERIC,	//	DFNTAG_WRESTLING,
	DFN_UPPERSTRING,	//	DFNTAG_ITEMLIST
	DFN_UPPERSTRING,	//	DFNTAG_NPCLIST
	DFN_STRING,			//  DFNTAG_NOTES
	DFN_STRING			//  DFNTAG_CATEGORY
};

struct strToDFNLookup
{
	const char *strToAdd;
	DFNTAGS		dfnToAdd;
};

map< string, DFNTAGS > strToDFNTag;

void InitStrToDFN( void )
{
	strToDFNLookup stuffToAdd[] = 
	{
		{"AC", DFNTAG_AC},
		{"ALCHEMY", DFNTAG_ALCHEMY},
		{"AMOUNT", DFNTAG_AMOUNT},
		{"ANATOMY", DFNTAG_ANATOMY},
		{"ANIMALLORE", DFNTAG_ANIMALLORE},
		{"ARCHERY", DFNTAG_ARCHERY},
		{"ARMSLORE", DFNTAG_ARMSLORE},
		{"ATT", DFNTAG_ATT},
		{"BACKPACK", DFNTAG_BACKPACK},
		{"BEGGING", DFNTAG_BEGGING},
		{"BLACKSMITHING", DFNTAG_BLACKSMITHING},
		{"BOWCRAFT", DFNTAG_BOWCRAFT},
		{"CAMPING", DFNTAG_CAMPING},
		{"CARPENTRY", DFNTAG_CARPENTRY},
		{"CARTOGRAPHY", DFNTAG_CARTOGRAPHY},
		{"CARVE", DFNTAG_CARVE},
		{"CATEGORY", DFNTAG_CATEGORY},
		{"COLD", DFNTAG_COLD},
		{"COLOR", DFNTAG_COLOUR},
		{"COLOUR", DFNTAG_COLOUR},
		{"COLORLIST", DFNTAG_COLOURLIST},
		{"COLOURLIST", DFNTAG_COLOURLIST},
		{"COLORMATCHHAIR", DFNTAG_COLOURMATCHHAIR},
		{"COLOURMATCHHAIR", DFNTAG_COLOURMATCHHAIR},
		{"COOKING", DFNTAG_COOKING},
		{"CORPSE", DFNTAG_CORPSE},
		{"CREATOR", DFNTAG_CREATOR},
		{"DAMAGE", DFNTAG_DAMAGE},
		{"DECAY", DFNTAG_DECAY},
		{"DEF", DFNTAG_DEF},
		{"DETECTINGHIDDEN", DFNTAG_DETECTINGHIDDEN},
		{"DEX", DFNTAG_DEX},
		{"DEXTERITY", DFNTAG_DEX},
		{"DX", DFNTAG_DEX},
		{"DEXADD", DFNTAG_DEXADD},
		{"DX2", DFNTAG_DEXADD},
		{"DIR", DFNTAG_DIR},
		{"DIRECTION", DFNTAG_DIR},
		{"DISPELLABLE", DFNTAG_DISPELLABLE},
		{"DISABLED", DFNTAG_DISABLED},
		{"DOORFLAG", DFNTAG_DOORFLAG},
		{"DYE", DFNTAG_DYE},
		{"DYEABLE", DFNTAG_DYE},
		{"EMOTECOLOR", DFNTAG_EMOTECOLOUR},
		{"EMOTECOLOUR", DFNTAG_EMOTECOLOUR},
		{"ENHANCED", DFNTAG_ENHANCED},
		{"ENTICEMENT", DFNTAG_ENTICEMENT},
		{"EVALUATINGINTEL", DFNTAG_EVALUATINGINTEL},
		{"FAME", DFNTAG_FAME},
		{"FENCING", DFNTAG_FENCING},
		{"FISHING", DFNTAG_FISHING},
		{"FLEEAT", DFNTAG_FLEEAT},
		{"FORENSICS", DFNTAG_FORENSICS},
		{"FX1", DFNTAG_FX1},
		{"FX2", DFNTAG_FX2},
		{"FY1", DFNTAG_FY1},
		{"FY2", DFNTAG_FY2}, 
		{"FZ1", DFNTAG_FZ1},
		{"GET", DFNTAG_GET},
		{"GLOW", DFNTAG_GLOW},
		{"GLOWBC", DFNTAG_GLOWBC},
		{"GLOWTYPE", DFNTAG_GLOWTYPE},
		{"GOLD", DFNTAG_GOLD},
		{"GOOD", DFNTAG_GOOD},
		{"HAIRCOLOR", DFNTAG_HAIRCOLOUR},
		{"HAIRCOLOUR", DFNTAG_HAIRCOLOUR},
		{"HEALING", DFNTAG_HEALING},
		{"HEAT", DFNTAG_HEAT},
		{"HERDING", DFNTAG_HERDING},
		{"HIDAMAGE", DFNTAG_HIDAMAGE},
		{"HIDING", DFNTAG_HIDING},
		{"HP", DFNTAG_HP},
		{"ID", DFNTAG_ID},
		{"ID2", DFNTAG_ID2},
		{"IN", DFNTAG_INTELLIGENCE},
		{"INTELLIGENCE", DFNTAG_INTELLIGENCE},
		{"INT", DFNTAG_INTELLIGENCE},
		{"IN2", DFNTAG_INTADD},
		{"INTADD", DFNTAG_INTADD},
		{"INSCRIPTION", DFNTAG_INSCRIPTION},
		{"ITEM", DFNTAG_ITEM},
		{"ITEMLIST", DFNTAG_ITEMLIST},
		{"ITEMID", DFNTAG_ITEMID},
		{"KARMA", DFNTAG_KARMA},
		{"LAYER", DFNTAG_LAYER},
		{"LIGHT", DFNTAG_LIGHT},
		{"LIGHTNING", DFNTAG_LIGHTNING},
		{"LOCKPICKING", DFNTAG_LOCKPICKING},
		{"LODAMAGE", DFNTAG_LODAMAGE},
		{"LOOT", DFNTAG_LOOT},
		{"LUMBERJACKING", DFNTAG_LUMBERJACKING},
		{"MACEFIGHTING", DFNTAG_MACEFIGHTING},
		{"MAGERY", DFNTAG_MAGERY},
		{"MAGICRESISTANCE", DFNTAG_MAGICRESISTANCE},
		{"MANA", DFNTAG_MANA},
		{"MAXHP", DFNTAG_MAXHP},
		{"MEDITATION", DFNTAG_MEDITATION},
		{"MINING", DFNTAG_MINING},
		{"MOD1", DFNTAG_MOD1},
		{"MOD2", DFNTAG_MOD2},
		{"MOD3", DFNTAG_MOD3},
		{"MOVABLE", DFNTAG_MOVABLE},
		{"MORE", DFNTAG_MORE},
		{"MORE2", DFNTAG_MORE2},
		{"MOREX", DFNTAG_MOREX},
		{"MOREY", DFNTAG_MOREY},
		{"MOREZ", DFNTAG_MOREZ},
		{"MUSICIANSHIP", DFNTAG_MUSICIANSHIP},
		{"NAME", DFNTAG_NAME},
		{"NAME2", DFNTAG_NAME2},
		{"NAMELIST", DFNTAG_NAMELIST},
		{"NEWBIE", DFNTAG_NEWBIE},
		{"NOMOVE", DFNTAG_NOMOVE},
		{"NOTES", DFNTAG_NOTES},
		{"NOTRAIN", DFNTAG_NOTRAIN},
		{"NPCAI", DFNTAG_NPCAI},
		{"NPCLIST", DFNTAG_NPCLIST},
		{"NPCWANDER", DFNTAG_NPCWANDER},
		{"OFFSPELL", DFNTAG_OFFSPELL},
		{"PACKITEM", DFNTAG_PACKITEM},
		{"PARRYING", DFNTAG_PARRYING},
		{"PEACEMAKING", DFNTAG_PEACEMAKING},
		{"PILEABLE", DFNTAG_PILEABLE},
		{"POISON", DFNTAG_POISON},
		{"POISONCHANCE", DFNTAG_POISONCHANCE},
		{"POISONSTRENGTH", DFNTAG_POISONSTRENGTH},
		{"POISONED", DFNTAG_POISONED},
		{"POISONING", DFNTAG_POISONING},
		{"PRIV", DFNTAG_PRIV},
		{"PRIV1", DFNTAG_PRIV1},
		{"PRIV2", DFNTAG_PRIV2},
		{"PROVOCATION", DFNTAG_PROVOCATION},
		{"RACE", DFNTAG_RACE},
		{"RAIN", DFNTAG_RAIN},
		{"RANK", DFNTAG_RANK},
		{"REATTACKAT", DFNTAG_REATTACKAT},
		{"REMOVETRAPS", DFNTAG_REMOVETRAPS},
		{"RESTOCK", DFNTAG_RESTOCK},
		{"RSHOPITEM", DFNTAG_RSHOPITEM},
		{"RUNS", DFNTAG_RUNS},
		{"SAYCOLOR", DFNTAG_SAYCOLOUR},
		{"SAYCOLOUR", DFNTAG_SAYCOLOUR},
		{"SCRIPT", DFNTAG_SCRIPT},
		{"SELLITEM", DFNTAG_SELLITEM},
		{"SHOPITEM", DFNTAG_SHOPITEM},
		{"SHOPKEEPER", DFNTAG_SHOPKEEPER},
		{"SHOPLIST", DFNTAG_SHOPLIST},
		{"SK_MADE", DFNTAG_SK_MADE},
		{"SKILL", DFNTAG_SKILL},
//		{"SKILL%i", DFNTAG_SKILLVAR},
		{"SKIN", DFNTAG_SKIN},
		{"SKIN2", DFNTAG_SKIN2},
		{"SKINLIST", DFNTAG_SKINLIST},
		{"SNOOPING", DFNTAG_SNOOPING},
		{"SNOW", DFNTAG_SNOW},
		{"SPADELAY", DFNTAG_SPADELAY},
		{"SPATTACK", DFNTAG_SPATTACK},
		{"SPAWNOBJ", DFNTAG_SPAWNOBJ},
		{"SPAWNOBJLIST", DFNTAG_SPAWNOBJLIST},
		{"SPD", DFNTAG_SPD},
		{"SPEED", DFNTAG_SPD},
		{"SPIRITSPEAK", DFNTAG_SPIRITSPEAK},
		{"SPLIT", DFNTAG_SPLIT},
		{"SPLITCHANCE", DFNTAG_SPLITCHANCE},
		{"ST", DFNTAG_STRENGTH},
		{"STAMINA", DFNTAG_STAMINA},
		{"STR", DFNTAG_STRENGTH},
		{"STRENGTH", DFNTAG_STRENGTH},
		{"ST2", DFNTAG_STRADD},
		{"STRADD", DFNTAG_STRADD},
		{"STEALING", DFNTAG_STEALING},
		{"STEALTH", DFNTAG_STEALTH},
		{"SWORDSMANSHIP", DFNTAG_SWORDSMANSHIP},
		{"TACTICS", DFNTAG_TACTICS},
		{"TAILORING", DFNTAG_TAILORING},
		{"TAMING", DFNTAG_TAMING},
		{"TASTEID", DFNTAG_TASTEID},
		{"TINKERING", DFNTAG_TINKERING},
		{"TITLE", DFNTAG_TITLE},
		{"TOTAME", DFNTAG_TOTAME},
		{"TRACKING", DFNTAG_TRACKING},
		{"TYPE", DFNTAG_TYPE},
		{"TYPE2", DFNTAG_TYPE2},
		{"VALUE", DFNTAG_VALUE},
		{"VETERINARY", DFNTAG_VETERINARY},
		{"VISIBLE", DFNTAG_VISIBLE},
		{"WEIGHT", DFNTAG_WEIGHT},
		{"WIPE", DFNTAG_WIPE},
		{"WRESTLING", DFNTAG_WRESTLING},
		{NULL, DFNTAG_COUNTOFTAGS}
	};
	int iCounter = 0;
	while( stuffToAdd[iCounter].strToAdd != NULL )
	{
		strToDFNTag[stuffToAdd[iCounter].strToAdd] = stuffToAdd[iCounter].dfnToAdd;
		++iCounter;
	};
}
void CleanupStrToDFN( void )
{
	strToDFNTag.clear();
}

DFNTAGS FindDFNTagFromStr( const char *strToFind )
{
	if( strToDFNTag.size() == 0 )	// if we haven't built our array yet
		InitStrToDFN();
	char tempString[512];
	strcpy( tempString, strToFind );
	strupr( tempString );
	map< string, DFNTAGS >::iterator toFind = strToDFNTag.find( tempString );
	if( toFind != strToDFNTag.end() )
		return toFind->second;
	return DFNTAG_COUNTOFTAGS;
}

//o--------------------------------------------------------------------------
//|	Function		-	ScriptSection( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Default constructor, initializing all variables
//o--------------------------------------------------------------------------
ScriptSection::ScriptSection( void ) : fileOpened( false ), arrayFilled( false ), dataFile( NULL ), 
currentPos( -1 ), fileType( -1 ), dfnCat( NUM_DEFS ), currentPos2( -1 )
{
	data.resize( 0 );
	dataV2.resize( 0 );
}

//o--------------------------------------------------------------------------
//|	Function		-	ScriptSection( FILE *targfile, DefinitionCategories d )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Default constructor, initializing all variables
//|						and grabbing a section from the file passed in
//o--------------------------------------------------------------------------
ScriptSection::ScriptSection( FILE *targFile, DefinitionCategories d ) : fileOpened( true ), arrayFilled( false ), 
dataFile( targFile ), currentPos( -1 ), fileType( -1 ), dfnCat( d ), currentPos2( -1 )
{
	data.resize( 0 );
	dataV2.resize( 0 );
	GrabFromFile();
	fileOpened = false;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool CloseFile( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the file was closed, or false if it
//|						was already closed
//o--------------------------------------------------------------------------
bool ScriptSection::CloseFile( void )
{
	if( !fileOpened )
		return false;
	fileOpened = false;
	if( dataFile == NULL )
		return false;
	fclose( dataFile );
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool AtEnd( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if at end of the section
//o--------------------------------------------------------------------------
bool ScriptSection::AtEnd( void )
// PRE:		vector loaded and init'd
// POST:	returns true if at end of array now
{
	if( currentPos < 0 )	// broken precondition
		return false;
	if( static_cast<UI16>(currentPos) >= data.size() )
		return true;
	return false;
}

//o--------------------------------------------------------------------------
//|	Function		-	char *First( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the first tag at the start of the section
//o--------------------------------------------------------------------------
const char *ScriptSection::First( void )
// PRE:		vector loaded and init'd
// POST:	returns string (tag) of first entry
{
	currentPos = 0;
	if( AtEnd() )
		return NULL;
	return data[currentPos]->tag;
}

//o--------------------------------------------------------------------------
//|	Function		-	char *Next( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the next tag in the section, or NULL if no more
//o--------------------------------------------------------------------------
const char *ScriptSection::Next( void )
{
	currentPos++;
	if( AtEnd() )
		return NULL;
	return data[currentPos]->tag;
}

//o--------------------------------------------------------------------------
//|	Function		-	char *MoveTo( SI16 position )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-   
//o--------------------------------------------------------------------------
//|	Purpose			-	Moves to position in the section and returns the tag there
//o--------------------------------------------------------------------------
const char *ScriptSection::MoveTo( SI16 position )
// PRE:		vector loaded and init'd
// POST:	returns string (tag) of next entry
{
	if( AtEnd() )
		return NULL;
	currentPos = position;
	return data[currentPos]->tag;
}

//o--------------------------------------------------------------------------
//|	Function		-	char *GrabData( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the data for the current tag
//o--------------------------------------------------------------------------
const char *ScriptSection::GrabData( void )
// PRE:		At a valid location, init'd data
// POST:	returns string of data of current entry
{
	if( AtEnd() )
		return NULL;
	return data[currentPos]->data;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool GrabFromFile( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Reads the currently opened file, parsing in a section
//|						Returns true if it contains some entries
//o--------------------------------------------------------------------------
bool ScriptSection::GrabFromFile( void )
{
	if( !fileOpened )
		return false;
	arrayFilled = true;
	sectData *toAdd = NULL;
	sectDataV2 *toAdd2 = NULL;
	char scp1[512], scp2[512];
	DFNTAGS mTag;
	strcpy( scp1, "{" );
	do
	{
		ParseLine( scp1, scp2 );
		if( scp1[0] != '}' && !feof( dataFile ) )
		{
			switch( dfnCat )
			{
			case hard_items_def:	// as it's the same format as items_def, in essence
			case npc_def:
			case items_def:
				mTag = FindDFNTagFromStr( scp1 );
				if( mTag != DFNTAG_COUNTOFTAGS )	// we have a validly recognized tag
				{
					if( dfnDataTypes[mTag] != DFN_NODATA && scp2[0] == 0 )	// it's a valid tag, needs data though!
						break;
					toAdd2 = new sectDataV2;
					toAdd2->tag = mTag;

					switch( dfnDataTypes[mTag] )
					{
					case DFN_UPPERSTRING:	
						strupr( scp2 );
					case DFN_STRING:
						toAdd2->cdata = new char[strlen( scp2 ) + 1];
						strcpy( toAdd2->cdata, scp2 );
						break;
					case DFN_NUMERIC:
						toAdd2->ndata = makeNum( scp2 );
						break;
					case DFN_DOUBLENUMERIC:
						char dntemp[256];
						gettokennum( scp2, 0, dntemp );
						toAdd2->ndata = makeNum( dntemp );
						gettokennum( scp2, 1, dntemp );
						toAdd2->odata = makeNum( dntemp );
						if( toAdd2->odata == 0 )
							toAdd2->odata = toAdd2->ndata;
						break;
					case DFN_NODATA:
					case DFN_UNKNOWN:	
						toAdd2->cdata = NULL;
						break;
					}					
					dataV2.push_back( toAdd2 );
				}
				else
				{
					toAdd = new sectData;
					toAdd->tag = new char[strlen( scp1 ) + 1];
					toAdd->data = new char[strlen( scp2 ) + 1];
					strcpy( toAdd->tag, scp1 );
					strcpy( toAdd->data, scp2 );
					data.push_back( toAdd );
				}
				break;
			case spawn_def:
			case create_def:
			case command_def:
				strupr( scp1 );
			default:
				toAdd = new sectData;
				toAdd->tag = new char[strlen( scp1 ) + 1];
				toAdd->data = new char[strlen( scp2 ) + 1];
				strcpy( toAdd->tag, scp1 );
				strcpy( toAdd->data, scp2 );
				data.push_back( toAdd );
				break;
			}
		}
	} while( scp1[0] != '}' && !feof( dataFile ) );
	if( data.size() == 0 && dataV2.size() == 0 )
	{
		arrayFilled = false;
		currentPos = -1;
		return false;
	}
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	~ScriptSection()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Default deconstructor, removing any allocated memory
//|						and closing any files that may be open
//o--------------------------------------------------------------------------
ScriptSection::~ScriptSection()
{
	bool flushResult = false;
	CloseFile();
	flushResult = FlushData();
	if( !flushResult )
		Console.Error( 3, "Section unable to flush data!" );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool FlushData( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Deletes all tag/data pairs and resizes array to 0
//o--------------------------------------------------------------------------
bool ScriptSection::FlushData( void )
{
	for( UI32 i = 0; i < data.size(); i++ )
	{
		delete [] data[i]->data;
		delete [] data[i]->tag;
		delete data[i];
	}
	for( UI32 j = 0; j < dataV2.size(); j++ )
	{
		switch( dfnDataTypes[dataV2[j]->tag] )
		{
		case DFN_STRING:
		case DFN_UPPERSTRING:
			delete [] dataV2[j]->cdata;
			break;
		default:
			break;
		};
		delete dataV2[j];
	}
	arrayFilled = false;
	data.resize( 0 );
	dataV2.resize( 0 );
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	ParseLine( char *scp1, char *scp2 )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Parses a line from the file, turning it into a tag / data pair
//o--------------------------------------------------------------------------
void ScriptSection::ParseLine( char *scp1, char *scp2 )
{
	if( scp1 == NULL || scp2 == NULL )
		return;
	char temp[1024];
	GrabLine( temp );
	int i = 0;
	scp1[0] = 0;
	scp2[0] = 0;
	while( temp[i] != 0 && temp[i] != '=' )
	{
		i++;
	}
	strncpy( scp1, temp, i );
	scp1[i] = 0;
	if( scp1[0] != '}' && temp[i] != 0 ) 
		strcpy( scp2, temp + i + 1 );
	return;
}

//o--------------------------------------------------------------------------
//|	Function		-	GrabLine( char *temp )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Reads a line from the script, ensuring no buffer overflow
//|						and that it's not a comment, NULL, or start of section
//o--------------------------------------------------------------------------
void ScriptSection::GrabLine( char *temp )
{
	if( temp == NULL )
		return;
	if( dataFile == NULL )
	{
		strcpy( temp, "EOF" );
		return;
	}
	bool valid = false;
	temp[0] = 0;
	bool partialComment = false;
	while( !valid )
	{
		int i = 0;
		if( feof( dataFile ) ) 
		{
			temp[i] = 0;
			return;
		}

		partialComment = false;
		char c = (char)fgetc( dataFile );
		while( c != 10 && i < 1024 )	// don't overflow our buffer
		{
			if( c != 13 )
			{
				temp[i] = c;
				++i;
			}
			if( feof( dataFile ) ) 
			{
				temp[i] = 0;
				return;
			}
			if( c == '/' )
			{
				if( !partialComment )
					partialComment = true;
				else
				{
					temp[i-2] = 0;	// NULL it
					while( c != 10 && c != 13 && i < 1024 && !feof( dataFile ) )
						c = (char)fgetc( dataFile );	// eat data until the end of the line
					break;
				}
			}
			else if( partialComment )
				partialComment = false;
			c = (char)fgetc( dataFile );
			if( c == 9 ) 
				c = 32;	// if we actually have a tab, then replace it with a space!
		}
		temp[i] = 0;
		valid = true;
		if( temp[0] == '/' && temp[1] == '/' ) 
			valid = false;
		else if( temp[0] == '{' )
			valid = false;
		else if( temp[0] == 0 ) 
			valid = false;
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	int NumEntries( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the number of entries in the section
//o--------------------------------------------------------------------------
SI32 ScriptSection::NumEntries( void )
{
	return data.size();
}

//o--------------------------------------------------------------------------
//|	Function		-	char *Prev( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the previous tag, or NULL if at start
//o--------------------------------------------------------------------------
const char *ScriptSection::Prev( void )
{
	currentPos--;
	if( AtEnd() )
		return NULL;
	return data[currentPos]->tag;
}

//o--------------------------------------------------------------------------
//|	Function		-	Remove( UI16 position )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-   Mr. Fixit (11-15-2001)
//o--------------------------------------------------------------------------
//|	Purpose			-	Removes the tag/data pair at position in the array
//o--------------------------------------------------------------------------
void ScriptSection::Remove( UI16 position )
// PRE:		vector loaded and init'd
// POST:	removes thing at position
{
	if( AtEnd() )
		return;
	delete [] data[position]->data;
	delete [] data[position]->tag;
	delete data[position];
	for( UI16 iCounter = position; iCounter < ( data.size() - 1 ); iCounter++ )
		data[iCounter] = data[iCounter + 1];
	data.resize( data.size() - 1 );
}


//o--------------------------------------------------------------------------
//|	Function		-	Append( const char *tagToAdd, const char *dataToAdd )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Adds a new tag/data pair at the end of the section
//o--------------------------------------------------------------------------
void ScriptSection::Append( const char *tagToAdd, const char *dataToAdd )
{
	int targPos = data.size();
	data.resize( data.size() + 1 );

	data[targPos] = new sectData;
	data[targPos]->tag = new char[strlen( tagToAdd ) + 1];
	data[targPos]->data = new char[strlen( dataToAdd ) + 1];
	strcpy( data[targPos]->tag, tagToAdd );
	strcpy( data[targPos]->data, dataToAdd );
}

const char *ScriptSection::GrabData( UI32& ndata, UI32& odata )
{
	if( AtEndTags() )
	{
		ndata = INVALIDSERIAL;
		odata = INVALIDSERIAL;
		return NULL;
	}
	else
	{
		ndata = dataV2[currentPos2]->ndata;
		odata = dataV2[currentPos2]->odata;
		return dataV2[currentPos2]->cdata;
	}
}
bool ScriptSection::AtEndTags( void )
{
	if( currentPos2 < 0 )	// broken precondition
		return false;
	if( static_cast<UI16>(currentPos2) >= dataV2.size() )
		return true;
	return false;
}
DFNTAGS ScriptSection::PrevTag( void )
{
	--currentPos2;
	if( AtEndTags() )
		return DFNTAG_COUNTOFTAGS;
	return dataV2[currentPos2]->tag;
}
DFNTAGS ScriptSection::NextTag( void )
{
	++currentPos2;
	if( AtEndTags() )
		return DFNTAG_COUNTOFTAGS;
	return dataV2[currentPos2]->tag;
}
DFNTAGS ScriptSection::FirstTag( void )
{
	currentPos2 = 0;
	if( AtEndTags() )
		return DFNTAG_COUNTOFTAGS;
	return dataV2[currentPos2]->tag;
}
