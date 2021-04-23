#ifndef __Classes_h
#define __Classes_h

enum PackTypes
{
	PT_UNKNOWN = 0,
	PT_COFFIN,
	PT_PACK,
	PT_PACK2,
	PT_BAG,
	PT_BARREL,
	PT_SQBASKET,
	PT_RBASKET,
	PT_WCHEST,
	PT_SCHEST,
	PT_GCHEST,
	PT_WBOX,
	PT_MBOX,
	PT_CRATE,
	PT_SHOPPACK,
	PT_DRAWER,
	PT_BANK,
	PT_BOOKCASE,
	PT_FARMOIRE,
	PT_WARMOIRE,
	PT_DRESSER,
	PT_STOCKING,
	PT_GIFTBOX1,
	PT_GIFTBOX2,
	PT_GIFTBOX3,
	PT_GIFTBOX4,
	PT_GIFTBOX5,
	PT_GIFTBOX6,
	PT_SECHEST1,
	PT_SECHEST2,
	PT_SECHEST3,
	PT_SECHEST4,
	PT_SECHEST5,
	PT_SEBASKET,
	PT_SEARMOIRE1,
	PT_SEARMOIRE2,
	PT_SEARMOIRE3,
	PT_GAME_BACKGAMMON,
	PT_GAME_CHESS,
	PT_MAILBOX1,
	PT_MAILBOX2,
	PT_MAILBOX3,
	PT_MAILBOX4,
	PT_MAILBOX5,
	PT_MAILBOX6,
	PT_MAILBOX7,
	PT_MAILBOX8,
	PT_MAILBOX9,
};

class cItem
{
public:
	CItem *		DupeItem( CSocket *s, CItem *i, UI32 amount );
	void		GlowItem( CItem *i );
	void		AddRespawnItem( CItem *s, const std::string& x, const bool inCont, const bool randomItem = false, UI16 itemAmount = 1 );
	void		CheckEquipment( CChar *p );
	void		StoreItemRandomValue( CItem *i, CTownRegion *tReg );
	PackTypes	getPackType( CItem *i );

	CItem *		CreateItem( CSocket *mSock, CChar *mChar, const UI16 iID, const UI16 iAmount, const UI16 iColour, const ObjectType itemType, bool inPack = false );
	CItem *		CreateScriptItem( CSocket *mSock, CChar *mChar, const std::string &item, const UI16 iAmount, const ObjectType itemType, bool inPack = false, const UI16 iColor = 0xFFFF );
	CItem *		CreateBaseScriptItem( std::string ourItem, const UI08 worldNumber, const UI16 iAmount, const UI16 instanceID = 0, const ObjectType itemType = OT_ITEM );
	CMultiObj *	CreateMulti( CChar *mChar, const std::string& cName, const UI16 iID, const bool isBoat );
	CItem *		CreateRandomItem( CSocket *mSock, const std::string& itemList );
	CItem *		CreateBaseItem( const UI08 worldNumber, const ObjectType itemType = OT_ITEM, const UI16 instanceID = 0 );

private:
	void		GetScriptItemSettings( CItem *iCreated );
	CItem *		PlaceItem( CSocket *mSock, CChar *mChar, CItem *iCreated, const bool inPack );
	CItem *		CreateRandomItem( const std::string& sItemList, const UI08 worldNumber, const UI16 instanceID = 0 );
};

class cCharStuff
{
private:
	void		FindSpotForNPC( CChar *c, const SI16 originX, const SI16 originY, const SI16 xAway, const SI16 yAway, const SI08 z, const UI08 worldNumber, const UI16 instanceID );
	void		LoadShopList( const std::string& list, CChar *c );
	CItem *		addRandomLoot( CItem *s, const std::string& lootlist );

public:
	CChar *		CreateBaseNPC( std::string ourNPC );
	CChar *		CreateRandomNPC( const std::string& npcList );

	CChar *		CreateNPC( CSpawnItem *iSpawner, const std::string &npc );
	CChar *		CreateNPCxyz( const std::string &npc, SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceID );
	void		PostSpawnUpdate( CChar *cCreated );
	bool		ApplyNpcSection( CChar *applyTo, ScriptSection *NpcCreation, std::string sectionID, bool isGate = false );
	CChar *		getGuardingPet( CChar *mChar, CBaseObject *guarded );
	bool		checkPetFriend( CChar *mChar, CChar *pet );
	void		stopPetGuarding( CChar *pet );
};

extern cItem *Items;
extern cCharStuff *Npcs;

#endif

