
//o---------------------------------------------------------------------------o
//| ssection.h
//o---------------------------------------------------------------------------o
//| Script Section class implementation
//o---------------------------------------------------------------------------o

#pragma warning( disable : 4097 )
#pragma warning( disable : 4100 )	// unreferenced formal parameter
#pragma warning( disable : 4201 )	// nameless struct/union
#pragma warning( disable : 4503 )
#pragma warning( disable : 4511 )	// copy constructor could not be generated
#pragma warning( disable : 4512 )	// assignment operator could not be generated
#pragma warning( disable : 4663 )	// C++ language change: to explicitly specialize class template '' use the following syntax...
#pragma warning( disable : 4786 )	// Gets rid of BAD stl warnings

#ifndef __SSECTION_H__
#define __SSECTION_H__

#include <vector>
#include <cstdio>
#include "typedefs.h"
using namespace std;

enum DFNTAGS
{
	DFNTAG_AC = 0,
	DFNTAG_ALCHEMY,
	DFNTAG_AMOUNT,
	DFNTAG_ANATOMY,
	DFNTAG_ANIMALLORE,
	DFNTAG_ARCHERY,
	DFNTAG_ARMSLORE,
	DFNTAG_ATT,
	DFNTAG_BACKPACK,
	DFNTAG_BEGGING,
	DFNTAG_BLACKSMITHING,
	DFNTAG_BOWCRAFT,
	DFNTAG_CAMPING,
	DFNTAG_CARPENTRY,
	DFNTAG_CARTOGRAPHY,
	DFNTAG_CARVE,
	DFNTAG_COLD,
	DFNTAG_COLOUR,
	DFNTAG_COLOURLIST,
	DFNTAG_COLOURMATCHHAIR,
	DFNTAG_COOKING,
	DFNTAG_CORPSE,
	DFNTAG_CREATOR,
	DFNTAG_DAMAGE,
	DFNTAG_DECAY,
	DFNTAG_DEF,
	DFNTAG_DETECTINGHIDDEN,
	DFNTAG_DEX,
	DFNTAG_DEXADD,
	DFNTAG_DIR,
	DFNTAG_DISPELLABLE,
	DFNTAG_DISABLED,
	DFNTAG_DOORFLAG,
	DFNTAG_DYE,
	DFNTAG_EMOTECOLOUR,
	DFNTAG_ENHANCED,
	DFNTAG_ENTICEMENT,
	DFNTAG_EVALUATINGINTEL,
	DFNTAG_FAME,
	DFNTAG_FENCING,
	DFNTAG_FISHING,
	DFNTAG_FLEEAT,
	DFNTAG_FORENSICS,
	DFNTAG_FX1,
	DFNTAG_FX2,
	DFNTAG_FY1,
	DFNTAG_FY2,
	DFNTAG_FZ1,
	DFNTAG_GET,
	DFNTAG_GLOW,
	DFNTAG_GLOWBC,
	DFNTAG_GLOWTYPE,
	DFNTAG_GOLD,
	DFNTAG_GOOD,
	DFNTAG_HAIRCOLOUR,
	DFNTAG_HEALING,
	DFNTAG_HEAT,
	DFNTAG_HERDING,
	DFNTAG_HIDAMAGE,
	DFNTAG_HIDING,
	DFNTAG_HP,
	DFNTAG_ID,
	DFNTAG_INTELLIGENCE,
	DFNTAG_INTADD,
	DFNTAG_INSCRIPTION,
	DFNTAG_ITEM,
	DFNTAG_ITEMID,
	DFNTAG_KARMA,
	DFNTAG_LAYER,
	DFNTAG_LIGHT,
	DFNTAG_LIGHTNING,
	DFNTAG_LOCKPICKING,
	DFNTAG_LODAMAGE,
	DFNTAG_LOOT,
	DFNTAG_LUMBERJACKING,
	DFNTAG_MACEFIGHTING,
	DFNTAG_MAGERY,
	DFNTAG_MAGICRESISTANCE,
	DFNTAG_MAXHP,
	DFNTAG_MEDITATION,
	DFNTAG_MINING,
	DFNTAG_MOVABLE,
	DFNTAG_MORE,
	DFNTAG_MORE2,
	DFNTAG_MOREX,
	DFNTAG_MOREY,
	DFNTAG_MOREZ,
	DFNTAG_MUSICIANSHIP,
	DFNTAG_NAME,
	DFNTAG_NAME2,
	DFNTAG_NAMELIST,
	DFNTAG_NEWBIE,
	DFNTAG_NOTRAIN,
	DFNTAG_NPCAI,
	DFNTAG_NPCWANDER,
	DFNTAG_OFFSPELL,
	DFNTAG_PACKITEM,
	DFNTAG_PARRYING,
	DFNTAG_PEACEMAKING,
	DFNTAG_PILEABLE,
	DFNTAG_POISON,
	DFNTAG_POISONED,
	DFNTAG_POISONING,
	DFNTAG_PRIV,
	DFNTAG_PRIV1,
	DFNTAG_PRIV2,
	DFNTAG_PROVOCATION,
	DFNTAG_RACE,
	DFNTAG_RAIN,
	DFNTAG_RANK,
	DFNTAG_REATTACKAT,
	DFNTAG_REMOVETRAPS,
	DFNTAG_RESTOCK,
	DFNTAG_RSHOPITEM,
	DFNTAG_RUNS,
	DFNTAG_SAYCOLOUR,
	DFNTAG_SCRIPT,
	DFNTAG_SELLITEM,
	DFNTAG_SHOPITEM,
	DFNTAG_SHOPKEEPER,
	DFNTAG_SHOPLIST,
	DFNTAG_SK_MADE,
	DFNTAG_SKILL,
	DFNTAG_SKILLVAR,
	DFNTAG_SKIN,
	DFNTAG_SKINLIST,
	DFNTAG_SNOOPING,
	DFNTAG_SNOW,
	DFNTAG_SPADELAY,
	DFNTAG_SPATTACK,
	DFNTAG_SPAWNOBJ,
	DFNTAG_SPAWNOBJLIST,
	DFNTAG_SPD,
	DFNTAG_SPIRITSPEAK,
	DFNTAG_SPLIT,
	DFNTAG_SPLITCHANCE,
	DFNTAG_STRENGTH,
	DFNTAG_STRADD,
	DFNTAG_STEALING,
	DFNTAG_STEALTH,
	DFNTAG_SWORDSMANSHIP,
	DFNTAG_TACTICS,
	DFNTAG_TAILORING,
	DFNTAG_TAMING,
	DFNTAG_TASTEID,
	DFNTAG_TINKERING,
	DFNTAG_TITLE,
	DFNTAG_TOTAME,
	DFNTAG_TRACKING,
	DFNTAG_TYPE,
	DFNTAG_TYPE2,
	DFNTAG_VALUE,
	DFNTAG_VETERINARY,
	DFNTAG_VISIBLE,
	DFNTAG_WEIGHT,
	DFNTAG_WIPE,
	DFNTAG_WRESTLING,
	DFNTAG_ITEMLIST,
	DFNTAG_NPCLIST,
	DFNTAG_CATEGORY,
	DFNTAG_NOTES,
	DFNTAG_COUNTOFTAGS
};

class ScriptSection
{
private:
	struct sectData
	{
		char *tag;
		char *data;
		sectData() : tag( NULL ), data( NULL ) {}
	};
	struct sectDataV2
	{
		DFNTAGS tag;
		char *	cdata;
		UI32	ndata;
		UI32	odata;
		sectDataV2() : tag( DFNTAG_COUNTOFTAGS ), cdata( NULL ), ndata( INVALIDSERIAL ), odata( INVALIDSERIAL ) {}
	};

	vector< sectData * >	data;
	vector< sectDataV2 * >	dataV2;
	bool					fileOpened;
	bool					arrayFilled;
	SI08					fileType;
	FILE *					dataFile;
	SI16					currentPos, currentPos2;
	DefinitionCategories	dfnCat;

	void					GrabLine( char *temp ); 
	void					ParseLine( char *scp1, char *scp2 ); 

public:
							ScriptSection( void );
							ScriptSection( FILE *targFile, DefinitionCategories d );
	bool					GrabFromFile( void );
	virtual					~ScriptSection();
	const char *			First( void );
	DFNTAGS					FirstTag( void );
	const char *			Next( void );
	DFNTAGS					NextTag( void );
	const char *			Prev( void );
	DFNTAGS					PrevTag( void );
	bool					AtEnd( void );
	bool					AtEndTags( void );
	const char *			GrabData( void );
	const char *			GrabData( UI32& ndata, UI32& odata );
	bool					FlushData( void );
	SI32					NumEntries( void );
	const char *			MoveTo( short int position );
	bool					CloseFile( void );
	void					Remove( UI16 position );
	void					Append( const char *tag, const char *data );
};

#endif

