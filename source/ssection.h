
//o------------------------------------------------------------------------------------------------o
//|	File		-	 ssection.h
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Script Section class implementation
//o------------------------------------------------------------------------------------------------o
#ifndef __SSECTION_H__
#define __SSECTION_H__

#include <string>

// 04302004 - Added a new tag DFNTAG_ADDMENUITEM to support the auto generation of the addmenu based on items that are contained in the DFN's
//						We will still use the menu group item and group/subgroup stuff, however we will no longer have a list of items in a dfn that
//						we display in the addmenu.
//						Format: ADDMENUITEM=Group/SubGroupID,ImageID,PositionWeight,Type,Flags
//						Where:
//							Group				: Actual group owner that this item will be listed under on the addmenu gump
//							TileId				: (-2) None, (-1) Use block ItemID for image (=>0) New tileid to display
//							PositionWeight		:	Basic measure in which is used to base the items location in the addmenu page.
//													Basically a value of 0 - 8/10 which ever max amount of entries we get on an addgump page
//													(0) having the highest priority, etc. To kind of help sorting Otherwise items will be listed
//													as they are read in from the DFN's at parsing time.
//							Flags				:	(0) not visible on the addmenu  (1) Visible on the addmenu
//							ObjectId			: This is the ID number that is added via the addmenu. ADDITEM=ObjectID escentially
//
enum DFNTAGS
{
	DFNTAG_AC = 0,
	DFNTAG_ADDMENUITEM,
	DFNTAG_ADVOBJ,
	DFNTAG_ALCHEMY,
	DFNTAG_ARTIFACTRARITY,
	DFNTAG_AMMO,
	DFNTAG_AMMOFX,
	DFNTAG_AMOUNT,
	DFNTAG_ANATOMY,
	DFNTAG_ANIMALLORE,
	DFNTAG_ARCHERY,
	DFNTAG_ARMSLORE,
	DFNTAG_ATT,
	DFNTAG_AWAKE,
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
	DFNTAG_CONTROLSLOTS,
	DFNTAG_COOKING,
	DFNTAG_CORPSE,
	DFNTAG_CREATOR,
	DFNTAG_CUSTOMSTRINGTAG,
	DFNTAG_CUSTOMINTTAG,
	DFNTAG_DAMAGE,
	DFNTAG_DAMAGEABLE,
	DFNTAG_DECAY,
	DFNTAG_DEF,
	DFNTAG_DEFBONUS,
	DFNTAG_DETECTINGHIDDEN,
	DFNTAG_DEX,
	DFNTAG_DEXADD,
	DFNTAG_DIR,
	DFNTAG_DISPELLABLE,
	DFNTAG_DISABLED,
	DFNTAG_DOORFLAG,
	DFNTAG_DEFENSECHANCE,
	DFNTAG_DYE,
	DFNTAG_DYEBEARD,
	DFNTAG_DYEHAIR,
	DFNTAG_ELEMENTRESIST,
	DFNTAG_ERBONUS,
	DFNTAG_EMOTECOLOUR,
	DFNTAG_ENTICEMENT,
	DFNTAG_EQUIPITEM,
	DFNTAG_EVALUATINGINTEL,
	DFNTAG_EVENT,
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
	DFNTAG_GETUO,
	DFNTAG_GETT2A,
	DFNTAG_GETUOR,
	DFNTAG_GETTD,
	DFNTAG_GETLBR,
	DFNTAG_GETAOS,
	DFNTAG_GETSE,
	DFNTAG_GETML,
	DFNTAG_GETSA,
	DFNTAG_GETHS,
	DFNTAG_GETTOL,
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
	DFNTAG_HITCHANCE,
	DFNTAG_HIDING,
	DFNTAG_HIRELING,
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
	DFNTAG_LOYALTY,
	DFNTAG_LUMBERJACKING,
	DFNTAG_MACEFIGHTING,
	DFNTAG_MAGERY,
	DFNTAG_MAGICRESISTANCE,
	DFNTAG_MANA,
	DFNTAG_MANAMAX,
	DFNTAG_MAXHP,
	DFNTAG_MAXITEMS,
	DFNTAG_MAXLOYALTY,
	DFNTAG_MAXRANGE,
	DFNTAG_MAXUSES,
	DFNTAG_MEDITATION,
	DFNTAG_MINING,
	DFNTAG_MOVABLE,
	DFNTAG_MORE,
	DFNTAG_MORE0,
	DFNTAG_MORE1,
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
	DFNTAG_NOHIRELING,
	DFNTAG_NOTRAIN,
	DFNTAG_NPCAI,
	DFNTAG_NPCGUILD,
	DFNTAG_NPCLIST,
	DFNTAG_NPCWANDER,
	DFNTAG_OFFSPELL,
	DFNTAG_ORIGIN,
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
	DFNTAG_REMOVETRAP,
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
	DFNTAG_SECTIONID,
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
	DFNTAG_SPELLS,
	DFNTAG_SPELLWEAVING,
	DFNTAG_SPIRITSPEAK,
	DFNTAG_SPLIT,
	DFNTAG_SPLITCHANCE,
	DFNTAG_STAMINA,
	DFNTAG_STAMINAMAX,
	DFNTAG_STRENGTH,
	DFNTAG_STRADD,
	DFNTAG_STEALABLE,
	DFNTAG_STEALING,
	DFNTAG_STEALTH,
	DFNTAG_SWORDSMANSHIP,
	DFNTAG_TACTICS,
	DFNTAG_TAILORING,
	DFNTAG_TAMING,
	DFNTAG_TAMEDHUNGER,
	DFNTAG_TAMEDTHIRST,
	DFNTAG_TASTEID,
	DFNTAG_THROWING,
	DFNTAG_TINKERING,
	DFNTAG_TITLE,
	DFNTAG_TOTAME,
	DFNTAG_TOPROV,
	DFNTAG_TOPEACE,
	DFNTAG_TRACKING,
	DFNTAG_TYPE,
	DFNTAG_USESLEFT,
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
	UI32	groupId;
	UI32	tileId;
	UI32	weightPosition;
	UI32	objectFlags;
	std::string	objectId;
	__ADDMENUITEM__() : itemIndex( 0 ), itemName( "" ), groupId( 0 ), tileId( 0 ),
	weightPosition( 0 ), objectFlags( 0 ), objectId( "" )
	{
	}
} ADDMENUITEM, *LPADDMENUITEM;

extern std::multimap<UI32, ADDMENUITEM>						g_mmapAddMenuMap;
typedef std::multimap<UI32, ADDMENUITEM>::iterator			ADDMENUMAP_ITERATOR;
typedef std::multimap<UI32, ADDMENUITEM>::const_iterator	ADDMENUMAP_CITERATOR;

class CScriptSection
{
	friend class Script;
private:
	struct SectData_st
	{
		std::string tag;
		std::string data;
		SectData_st() : tag( "" ), data( "" )
		{
		}
	};
	struct SectDataV2_st
	{
		DFNTAGS tag;
		std::string	cdata;
		SI32	ndata;
		SI32	odata;
		SectDataV2_st() : tag( DFNTAG_COUNTOFTAGS ), cdata( "" ), ndata( -1 ), odata( -1 )
		{
		}
	};

	std::vector<SectData_st *>				data;
	std::vector<SectDataV2_st *>			dataV2;
	std::vector<SectData_st *>::iterator	currentPos;
	std::vector<SectDataV2_st *>::iterator	currentPos2;
	DEFINITIONCATEGORIES	dfnCat;

	bool					npcList;
	bool					itemList;

	std::string				npcListData;
	std::string				itemListData;

public:
	auto collection() const -> const std::vector<SectData_st *>&;
	auto collection()  -> std::vector<SectData_st *>&;
	auto collection2() const -> const std::vector<SectDataV2_st *>&;
	auto collection2()  -> std::vector<SectDataV2_st *>&;

	CScriptSection();
	CScriptSection( std::istream& input, DEFINITIONCATEGORIES d );
	~CScriptSection();
	auto First() -> std::string;
	auto Next() -> std::string;
	auto Prev() -> std::string;
	auto AtEnd() -> bool;
	auto FirstTag() -> DFNTAGS;
	auto NextTag() -> DFNTAGS;
	auto PrevTag() -> DFNTAGS;
	auto AtEndTags() -> bool;
	auto GrabData() -> std::string;
	auto GrabData( SI32& ndata, SI32& odata ) -> std::string;
	auto FlushData() -> bool;
	auto NumEntries() const -> size_t;
	auto MoveTo( size_t position ) -> std::string;
	auto CloseFile() -> bool;
	auto Remove( size_t position ) -> void;
	auto Append( std::string tag, std::string data ) -> void;
	auto ItemListExist() const -> bool;
	auto NpcListExist() const -> bool;
	auto ItemListData() const -> std::string;
	auto NpcListData() const -> std::string;
	auto CreateSection( std::istream& inputbuf ) -> void;
};

#endif

