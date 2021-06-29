
//o-----------------------------------------------------------------------------------------------o
//|	File		-	 ssection.h
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Script Section class implementation
//o-----------------------------------------------------------------------------------------------o
#ifndef __SSECTION_H__
#define __SSECTION_H__

#include <string>

// 04302004 - Added a new tag DFNTAG_ADDMENUITEM to support the auto generation of the addmenu based on items that are contained in the DFN's
//						We will still use the menu group item and group/subgroup stuff, however we will no longer have a list of items in a dfn that
//						we display in the addmenu.
//						Format: ADDMENUITEM=Group/SubGroupID,ImageID,PositionWeight,Type,Flags
//						Where:
//							Group				: Actual group owner that this item will be listed under on the addmenu gump
//							TileID				: (-2) None, (-1) Use block ItemID for image (=>0) New tileid to display
//							PositionWeight		:	Basic measure in which is used to base the items location in the addmenu page.
//													Basically a value of 0 - 8/10 which ever max amount of entries we get on an addgump page
//													(0) having the highest priority, etc. To kind of help sorting Otherwise items will be listed
//													as they are read in from the DFN's at parsing time.
//							Flags				:	(0) not visible on the addmenu  (1) Visible on the addmenu
//							ObjectID			: This is the ID number that is added via the addmenu. ADDITEM=ObjectID escentially
//
enum DFNTAGS
{
	DFNTAG_AC = 0,
	DFNTAG_ADDMENUITEM,
	DFNTAG_ADVOBJ,
	DFNTAG_ALCHEMY,
	DFNTAG_AMMO,
	DFNTAG_AMMOFX,
	DFNTAG_AMOUNT,
	DFNTAG_ANATOMY,
	DFNTAG_ANIMALLORE,
	DFNTAG_ARCHERY,
	DFNTAG_ARMSLORE,
	DFNTAG_ATT,
	DFNTAG_BACKPACK,
	DFNTAG_BASERANGE,
	DFNTAG_BEGGING,
	DFNTAG_BLACKSMITHING,
	DFNTAG_BOWCRAFT,
	DFNTAG_BUSHIDO,
	DFNTAG_CAMPING,
	DFNTAG_CARPENTRY,
	DFNTAG_CARTOGRAPHY,
	DFNTAG_CARVE,
	DFNTAG_CHIVALRY,
	DFNTAG_COLD,
	DFNTAG_COLOUR,
	DFNTAG_COLOURLIST,
	DFNTAG_COLOURMATCHHAIR,
	DFNTAG_COOKING,
	DFNTAG_CORPSE,
	DFNTAG_CREATOR,
	DFNTAG_CUSTOMSTRINGTAG,
	DFNTAG_CUSTOMINTTAG,
	DFNTAG_DAMAGE,
	DFNTAG_DAMAGEABLE,
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
	DFNTAG_DYEBEARD,
	DFNTAG_DYEHAIR,
	DFNTAG_ELEMENTRESIST,
	DFNTAG_EMOTECOLOUR,
	DFNTAG_ENTICEMENT,
	DFNTAG_EQUIPITEM,
	DFNTAG_EVALUATINGINTEL,
	DFNTAG_FAME,
	DFNTAG_FENCING,
	DFNTAG_FISHING,
	DFNTAG_FLAG,
	DFNTAG_FLEEAT,
	DFNTAG_FLEEINGSPEED,
	DFNTAG_FLEEINGSPEEDMOUNTED,
	DFNTAG_FOCUS,
	DFNTAG_FORENSICS,
	DFNTAG_FX1,
	DFNTAG_FX2,
	DFNTAG_FY1,
	DFNTAG_FY2,
	DFNTAG_FZ1,
	DFNTAG_FOOD,
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
	DFNTAG_HPMAX,
	DFNTAG_ID,
	DFNTAG_IMBUING,
	DFNTAG_INTADD,
	DFNTAG_INTELLIGENCE,
	DFNTAG_INTERVAL,
	DFNTAG_INSCRIPTION,
	DFNTAG_ITEMID,
	DFNTAG_ITEMLIST,
	DFNTAG_KARMA,
	DFNTAG_KILLHAIR,
	DFNTAG_KILLBEARD,
	DFNTAG_KILLPACK,
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
	DFNTAG_MANA,
	DFNTAG_MANAMAX,
	DFNTAG_MAXHP,
	DFNTAG_MAXITEMS,
	DFNTAG_MAXRANGE,
	DFNTAG_MEDITATION,
	DFNTAG_MINING,
	DFNTAG_MOVABLE,
	DFNTAG_MORE,
	DFNTAG_MORE2,
	DFNTAG_MOREX,
	DFNTAG_MOREY,
	DFNTAG_MOREZ,
	DFNTAG_MUSICIANSHIP,
	DFNTAG_MYSTICISM,
	DFNTAG_NAME,
	DFNTAG_NAME2,
	DFNTAG_NAMELIST,
	DFNTAG_NECROMANCY,
	DFNTAG_NEWBIE,
	DFNTAG_NINJITSU,
	DFNTAG_NOTRAIN,
	DFNTAG_NPCAI,
	DFNTAG_NPCLIST,
	DFNTAG_NPCWANDER,
	DFNTAG_OFFSPELL,
	DFNTAG_PACKITEM,
	DFNTAG_PARRYING,
	DFNTAG_PEACEMAKING,
	DFNTAG_PILEABLE,
	DFNTAG_POISONDAMAGE,
	DFNTAG_POISONSTRENGTH,
	DFNTAG_POISONED,
	DFNTAG_POISONING,
	DFNTAG_POLY,
	DFNTAG_PRIV,
	DFNTAG_PROVOCATION,
	DFNTAG_RACE,
	DFNTAG_RAIN,
	DFNTAG_RANK,
	DFNTAG_REATTACKAT,
	DFNTAG_REMOVETRAPS,
	DFNTAG_RESTOCK,
	DFNTAG_RESISTFIRE,
	DFNTAG_RESISTCOLD,
	DFNTAG_RESISTLIGHTNING,
	DFNTAG_RESISTPOISON,
	DFNTAG_RSHOPITEM,
	DFNTAG_RUNNINGSPEED,
	DFNTAG_RUNNINGSPEEDMOUNTED,
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
	DFNTAG_SPELLWEAVING,
	DFNTAG_SPIRITSPEAK,
	DFNTAG_SPLIT,
	DFNTAG_SPLITCHANCE,
	DFNTAG_STAMINA,
	DFNTAG_STAMINAMAX,
	DFNTAG_STRENGTH,
	DFNTAG_STRADD,
	DFNTAG_STEALING,
	DFNTAG_STEALTH,
	DFNTAG_SWORDSMANSHIP,
	DFNTAG_TACTICS,
	DFNTAG_TAILORING,
	DFNTAG_TAMING,
	DFNTAG_TAMEDHUNGER,
	DFNTAG_TASTEID,
	DFNTAG_THROWING,
	DFNTAG_TINKERING,
	DFNTAG_TITLE,
	DFNTAG_TOTAME,
	DFNTAG_TOPROV,
	DFNTAG_TOPEACE,
	DFNTAG_TRACKING,
	DFNTAG_TYPE,
	DFNTAG_VALUE,
	DFNTAG_VETERINARY,
	DFNTAG_VISIBLE,
	DFNTAG_WALKINGSPEED,
	DFNTAG_WALKINGSPEEDMOUNTED,
	DFNTAG_WEIGHT,
	DFNTAG_WEIGHTMAX,
	DFNTAG_WILLHUNGER,
	DFNTAG_WILLTHIRST,
	DFNTAG_WIPE,
	DFNTAG_WRESTLING,
	DFNTAG_COUNTOFTAGS
};

typedef struct __ADDMENUITEM__
{
	UI32	itemIndex;
	std::string itemName;
	UI32	groupID;
	UI32	tileID;
	UI32	weightPosition;
	UI32	objectFlags;
	std::string	objectID;
	__ADDMENUITEM__() : itemIndex( 0 ), itemName( "" ), groupID( 0 ), tileID( 0 ),
	weightPosition( 0 ), objectFlags( 0 ), objectID( "" )
	{
	}
} ADDMENUITEM, *LPADDMENUITEM;

extern std::multimap< UI32, ADDMENUITEM >						g_mmapAddMenuMap;
typedef std::multimap< UI32, ADDMENUITEM >::iterator			ADDMENUMAP_ITERATOR;
typedef std::multimap< UI32, ADDMENUITEM >::const_iterator		ADDMENUMAP_CITERATOR;

class ScriptSection
{
	friend class Script;
private:
	struct sectData
	{
		std::string tag;
		std::string data;
		sectData() : tag( "" ), data( "" )
		{
		}
	};
	struct sectDataV2
	{
		DFNTAGS tag;
		std::string	cdata;
		SI32	ndata;
		SI32	odata;
		sectDataV2() : tag( DFNTAG_COUNTOFTAGS ), cdata( "" ), ndata( -1 ), odata( -1 )
		{
		}
	};

	std::vector< sectData * >				data;
	std::vector< sectDataV2 * >				dataV2;
	std::vector< sectData * >::iterator		currentPos;
	std::vector< sectDataV2 * >::iterator	currentPos2;
	DEFINITIONCATEGORIES	dfnCat;

	bool					npcList;
	bool					itemList;

	std::string				npcListData;
	std::string				itemListData;

public:
	ScriptSection( void );
	ScriptSection( std::fstream& input, DEFINITIONCATEGORIES d );
	~ScriptSection();
	const std::string		First( void );
	DFNTAGS					FirstTag( void );
	const std::string		Next( void );
	DFNTAGS					NextTag( void );
	const std::string		Prev( void );
	DFNTAGS					PrevTag( void );
	bool					AtEnd( void );
	bool					AtEndTags( void );
	const std::string		GrabData( void );
	const std::string		GrabData( SI32& ndata, SI32& odata );
	bool					FlushData( void );
	size_t					NumEntries( void ) const;
	const std::string		MoveTo( size_t position );
	bool					CloseFile( void );
	void					Remove( size_t position );
	void					Append( std::string tag, std::string data );
	bool					ItemListExist( void ) const;
	bool					NpcListExist( void ) const;
	const std::string		ItemListData( void ) const;
	const std::string		NpcListData( void ) const;
	void					createSection( std::fstream& inputbuf );
};

#endif

