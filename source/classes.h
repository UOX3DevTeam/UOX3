#ifndef __Classes_h
#define __Classes_h

#define MAXPOSTS 128
#include <queue>
using namespace std;
#include "threadsafeobject.h"
class CChar;
class CItem;
class cSocket;
class CMultiObj;
#include "mapstuff.h"
#include "cBaseObject.h"


typedef vector< CChar * > CHARLIST;
typedef vector< CItem * > ITEMLIST;

#include "cChar.h"
#include "cItem.h"

class ScriptSection;	// forward declaration
class cScript;

#include "cSpawnRegion.h"
#include "combat.h"
#include "commands.h"

class cHTMLTemplate;
class cHTMLTemplates;
#include "cHTMLSystem.h"

class cItem
{
public:
	void		GlowItem( CItem *i );
	CItem *		MemItemFree( ITEM& offset, bool zeroSer, UI08 itemType = 0 );
	void		DeleItem( CItem *i );
	CItem *		CreateScriptItem( cSocket *s, string name, bool nSpawned, UI08 worldNumber );
	CItem *		CreateRandomItem( cSocket *s, const char *sItemList, bool nSpawned, UI08 worldNumber );
	CItem *		SpawnItem( cSocket *nSocket, UI32 nAmount, const char *cName, bool nStackable, UI16 realItemId, UI16 realColour, bool nPack, bool nSend );
	CItem *		SpawnItem( cSocket *nSocket, CChar *ch, UI32 nAmount, const char *cName, bool nStackable, UI16 realItemID, UI16 realColour, bool nPack, bool nSend );
	CItem *		SpawnItemToPack( cSocket *s, CChar *ch, int nItem, bool nDigging );
	CItem *		SpawnItemToPack( cSocket *s, CChar *ch, string name, bool nDigging );
	CMultiObj *	SpawnMulti( cSocket *nSocket, CChar *ch, const char *cName, UI16 realItemID );
	void		GetScriptItemSetting( CItem *c );
	bool		DecayItem( CItem *i );
	void		Decay( void );
	void		RespawnItem( CItem *i );
	void		AddRespawnItem( CItem *s, UI32 x, bool inCont );
	void		AddRespawnItem( CItem *s, const char *x, bool inCont, bool randomItem );
	UI08		PackType( UI16 id );
	void		CheckEquipment( CChar *p );

private:
	CItem *		CreateItem( cSocket *s, string name, UI08 worldNumber );
};

// use this value whereever you need to return an illegal z value
const SI08 illegal_z = -128;

class cCharStuff
{
private:
	void		FindSpotForNPC( CChar *c, SI16 originX, SI16 originY, SI16 xAway, SI16 yAway, SI08 z );
	CChar *		CreateScriptNpc( cSocket *s, string targNPC, UI08 worldNumber );
	CChar *		CreateScriptNpc( cSocket *s, int    targNPC, UI08 worldNumber );
	bool		ApplyNpcSection( CChar *applyTo, ScriptSection *NpcCreation );
	void		PostSpawnUpdate( CChar *x );

public:
	void		DeleteChar( CChar *k );
	CChar *		MemCharFree( CHARACTER& offset, bool zeroSer = false );
	CItem *		addRandomLoot( CItem *s, const char * lootlist );
	CChar *		CreateRandomNpc( cSocket *s, const char *npclist, UI08 worldNumber );
	CChar *		SpawnNPC( cSpawnRegion *spawnReg, string npcNum, UI08 worldNumber );
	CChar *		SpawnNPC( CItem *i, string npcNum, UI08 worldNumber, bool randomNPC );
	CChar *		AddNPC( cSocket *s, cSpawnRegion *spawnReg, string npcNum, UI08 worldNumber );
	CChar *		AddNPCxyz( cSocket *s, int npcNum, SI16 x1, SI16 y1, SI08 z1, UI08 worldNumber );
	void		Split( CChar *k );
	void		CheckAI( CChar *i );
	void		LoadShopList( const char *list, CChar *c );
	CChar *		getGuardingPet( CChar *mChar, SERIAL guarded );
	bool		checkPetFriend( CChar *mChar, CChar *pet );
	void		stopPetGuarding( CChar *pet );
};

#include "skills.h"
#include "weight.h"
#include "targeting.h"
#include "network.h"
#include "cMagic.h"
#include "movement.h"
#include "teffect.h"
#include "gump.h"
#include "PageVector.h"
#include "cMultiObj.h"
#include "wholist.h"
#include "handlers.h"
#include "trigger.h"
#include "cScript.h"
#include "cSocket.h"
#include "jail.h"

#endif

