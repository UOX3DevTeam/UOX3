#ifndef __Classes_h
#define __Classes_h

#define MAXPOSTS 128
#include <queue>
//using namespace std;
#include "threadsafeobject.h"
class CChar;
class CItem;
class cSocket;
class CMultiObj;
#include "mapstuff.h"
#include "cBaseObject.h"


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
	void		menuAddItem( cSocket *s, std::string item );
	CItem *		CreateScriptItem( cSocket *s, std::string name, bool nSpawned, UI08 worldNumber );
	CItem *		SpawnItem( cSocket *nSocket, CChar *ch, UI32 nAmount, const char *cName, bool nStackable, UI16 realItemID, UI16 realColour, bool nPack, bool nSend );
	CItem *		SpawnItemToPack( cSocket *s, CChar *mChar, int nItem, bool nDigging );
	CItem *		SpawnItemToPack( cSocket *s, CChar *mChar, std::string name, bool nDigging );
	CMultiObj *	SpawnMulti( CChar *ch, const char *cName, UI16 realItemID );
	bool		DecayItem( CItem *i );
	void		RespawnItem( CItem *i );
	void		AddRespawnItem( CItem *s, UI32 x, bool inCont );
	void		AddRespawnItem( CItem *s, const char *x, bool inCont, bool randomItem );
	void		CheckEquipment( CChar *p );

private:
	CItem *		CreateItem( cSocket *s, std::string name, UI08 worldNumber );
	CItem *		CreateRandomItem( cSocket *s, const char *sItemList, bool nSpawned, UI08 worldNumber );
	void		GetScriptItemSetting( CItem *c );
	UI08		PackType( UI16 id );
};

// use this value whereever you need to return an illegal z value
const SI08 illegal_z = -128;

class cCharStuff
{
private:
	void		FindSpotForNPC( CChar *c, SI16 originX, SI16 originY, SI16 xAway, SI16 yAway, SI08 z );
	CChar *		CreateScriptNpc( std::string targNPC, UI08 worldNumber );
	CChar *		CreateScriptNpc( int    targNPC, UI08 worldNumber );
	bool		ApplyNpcSection( CChar *applyTo, ScriptSection *NpcCreation );
	void		PostSpawnUpdate( CChar *x );
	void		LoadShopList( const char *list, CChar *c );

public:
	void		DeleteChar( CChar *k );
	CChar *		MemCharFree( CHARACTER& offset, bool zeroSer = false );
	CItem *		addRandomLoot( CItem *s, const char * lootlist );
	CChar *		CreateRandomNpc( const char *npclist, UI08 worldNumber );
	CChar *		SpawnNPC( cSpawnRegion *spawnReg, std::string npcNum, UI08 worldNumber );
	CChar *		SpawnNPC( CItem *i, std::string npcNum, UI08 worldNumber, bool randomNPC );
	CChar *		AddNPC( cSocket *s, cSpawnRegion *spawnReg, std::string npcNum, UI08 worldNumber );
	CChar *		AddNPCxyz( cSocket *s, std::string npcNum, SI16 x1, SI16 y1, SI08 z1, UI08 worldNumber );
	void		Split( CChar *k );
	void		CheckAI( CChar *i );
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
#include "packets.h"
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

