#ifndef __Classes_h
#define __Classes_h

namespace UOX
{

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
	PT_DRESSER
};

class cItem
{
public:
	CItem *		DupeItem( CSocket *s, CItem *i, UI32 amount );
	void		GlowItem( CItem *i );
	void		AddRespawnItem( CItem *s, const std::string x, const bool inCont, const bool randomItem = false );
	void		CheckEquipment( CChar *p );
	void		StoreItemRandomValue( CItem *i, CTownRegion *tReg );
	PackTypes	getPackType( CItem *i );

	CItem *		CreateItem( CSocket *mSock, CChar *mChar, const UI16 iID, const UI16 iAmount, const UI16 iColour, const ObjectType itemType, bool inPack = false );
	CItem *		CreateScriptItem( CSocket *mSock, CChar *mChar, std::string item, const UI16 iAmount, const ObjectType itemType, const bool inPack = false, const UI16 iColor = 0xFFFF );
	CItem *		CreateBaseScriptItem( UString ourItem, const UI08 worldNumber, const ObjectType itemType = OT_ITEM );
	CMultiObj *	CreateMulti( CChar *mChar, const std::string cName, const UI16 iID, const bool isBoat ); 
	CItem *		CreateRandomItem( CSocket *mSock, const std::string itemList );
	CItem *		CreateBaseItem( const UI08 worldNumber, const ObjectType itemType = OT_ITEM );

private:
	void		GetScriptItemSettings( CItem *iCreated );
	CItem *		PlaceItem( CSocket *mSock, CChar *mChar, CItem *iCreated, const bool inPack );
	CItem *		CreateRandomItem( const std::string sItemList, const UI08 worldNumber );
};

class cCharStuff
{
private:
	void		FindSpotForNPC( CChar *c, const SI16 originX, const SI16 originY, const SI16 xAway, const SI16 yAway, const SI08 z, const UI08 worldNumber );
	void		LoadShopList( const std::string list, CChar *c );
	CItem *		addRandomLoot( CItem *s, const std::string lootlist );

public:
	CChar *		CreateBaseNPC( UString ourNPC );
	CChar *		CreateRandomNPC( const std::string npcList );

	CChar *		CreateNPC( CSpawnItem *iSpawner, std::string npc );
	CChar *		CreateNPCxyz( std::string npc, SI16 x, SI16 y, SI08 z, UI08 worldNumber );
	void		PostSpawnUpdate( CChar *cCreated );
	bool		ApplyNpcSection( CChar *applyTo, ScriptSection *NpcCreation, bool isGate = false );
	CChar *		getGuardingPet( CChar *mChar, CBaseObject *guarded );
	bool		checkPetFriend( CChar *mChar, CChar *pet );
	void		stopPetGuarding( CChar *pet );
};

extern cItem *Items;
extern cCharStuff *Npcs;

}

#endif

