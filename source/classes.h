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
				cItem()
				{
				}
				~cItem()
				{
				}
	CItem *		DupeItem( cSocket *s, CItem *i, UI32 amount );
	void		GlowItem( CItem *i );
	void		AddRespawnItem( CItem *s, std::string x, bool inCont, bool randomItem = false );
	void		CheckEquipment( CChar *p );
	void		StoreItemRandomValue( CItem *i, cTownRegion *tReg );
	PackTypes	getPackType( CItem *i );

	CItem *		CreateItem( cSocket *mSock, CChar *mChar, UI16 iID, UI32 iAmount, UI16 iColour, ObjectType itemType, bool inPack = false );
	CItem *		CreateScriptItem( cSocket *mSock, CChar *mChar, std::string item, UI32 iAmount, ObjectType itemType, bool inPack = false );
	CItem *		CreateBaseScriptItem( std::string name, UI08 worldNumber, ObjectType itemType = OT_ITEM );
	CMultiObj *	CreateMulti( CChar *mChar, std::string cName, UI16 iID, bool isBoat ); 
	CItem *		CreateRandomItem( cSocket *mSock, std::string itemList );

private:
	void		GetScriptItemSettings( CItem *iCreated );
	CItem *		PlaceItem( cSocket *mSock, CChar *mChar, CItem *iCreated, bool inPack );
	CItem *		CreateBaseItem( UI08 worldNumber, ObjectType itemType = OT_ITEM );
	CItem *		CreateRandomItem( std::string sItemList, UI08 worldNumber );
};

class cCharStuff
{
private:
	void		FindSpotForNPC( CChar *c, SI16 originX, SI16 originY, SI16 xAway, SI16 yAway, SI08 z, UI08 worldNumber );
	void		LoadShopList( std::string list, CChar *c );
	CItem *		addRandomLoot( CItem *s, std::string lootlist );

public:
	CChar *		CreateBaseNPC( std::string npc );
	CChar *		CreateRandomNPC( std::string npcList );

	CChar *		CreateNPC( CSpawnItem *iSpawner, std::string npc );
	CChar *		CreateNPCxyz( std::string npc, SI16 x, SI16 y, SI08 z, UI08 worldNumber );
	void		PostSpawnUpdate( CChar *cCreated );
	bool		ApplyNpcSection( CChar *applyTo, ScriptSection *NpcCreation, bool isGate = false );
	void		Split( CChar *k );
	CChar *		getGuardingPet( CChar *mChar, cBaseObject *guarded );
	bool		checkPetFriend( CChar *mChar, CChar *pet );
	void		stopPetGuarding( CChar *pet );
};

extern cItem *Items;
extern cCharStuff *Npcs;

}

#endif

