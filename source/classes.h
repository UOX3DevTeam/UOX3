//------------------------------------------------------------------------
//  classes.h
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1999 - 2001
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//	
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//   
//------------------------------------------------------------------------
//---Classes code by Zippy Started 7/29/99---//

#ifndef __Classes_h
#define __Classes_h

//Moved from uox3.h
struct tile_st
{
	SI32 unknown1;  // longs must go at top to avoid bus errors - fur
	SI32 animation;
	unsigned char flag1;
	unsigned char flag2;
	unsigned char flag3;
	unsigned char flag4;
	unsigned char weight;
	signed char layer;
	signed char unknown2;
	signed char unknown3;
	signed char height;
	signed char name[23];	// manually padded to long to avoid bus errors - fur
} PACK_NEEDED;

struct land_st
{
	unsigned char flag1;
	unsigned char flag2;
	unsigned char flag3;
	unsigned char flag4;
	char unknown1;
	char unknown2;
	char name[20];
};

class cItemHandle
{
protected:
	item_st *DefaultItem;//The item send if an out of bounds is referenced.

	vector<item_st *> Items;//Vector of pointers to items, NULL if no item at that pos
	unsigned long Acctual; //Number of items in existance

	vector<unsigned long> FreeNums;//Vector of free item numbers
	unsigned int Free; //Number of free spaces in Acctual (Recyle item numbers)
	bool isFree( unsigned long Num );//Check to see if this item is marked free

public:
	cItemHandle( void ); //Class Constructor
	~cItemHandle();//Class Destructor

	unsigned long New( void );//Get Memory for a new item, Returns Item number
	void Delete( long int );//Free memory used by this item
	unsigned long Size( void );//Return the size (in bytes) of ram items are taking up
	void Reserve( unsigned int );//Reserve memory for this number of items (UNUSED)
	unsigned long Count( void );//Return Acctual-> the number of items in world.

	item_st& operator[] ( long int );//Reference an item
};

class cCharacterHandle
{
protected:
	char_st *DefaultChar;//The item send if an out of bounds is referenced.

	vector<char_st *> Chars;//Vector of pointers to items, NULL if no item at that pos
	unsigned long Acctual; //Number of items in existance

	vector<unsigned long> FreeNums;//Vector of free item numbers
	unsigned int Free; //Number of free spaces in Acctual (Recyle item numbers)
	bool isFree( unsigned long Num );//Check to see if this item is marked free

public:
	cCharacterHandle( void ); //Class Constructor
	~cCharacterHandle();//Class Destructor

	unsigned long New( void );//Get Memory for a new character, Returns char number
	void Delete( long int );//Free memory used by this character
	unsigned long Size( void );//Return the size (in bytes) of ram characters are taking up
	void Reserve( unsigned int );//Reserve memory for this number of characters (UNUSED)
	unsigned long Count( void );//Return Acctual-> the number of characters in world.

	char_st& operator[] ( long int );//Reference a character
};

class cAdmin   // Revana*
{
private:
	void Account();
	void ReadString();
	void Wiped( void );
	void GumpALine( int line );
	void GumpAText( int line, int s );
public:
	void LoadAccounts();
	void ReadIni();
	void LoadWipe( void );
	void GumpAMenu( int s, int j );
	void CheckLocks( int nAcct );
};

class cCombat
{
private:
	void ItemCastSpell(int s, int c, int i);
	int TimerOk(int c);
	void ItemSpell(int attacker, int defender);
	void doSoundEffect( CHARACTER p, int fightskill, ITEM weapon ); // AntiChrist
	void doMissedSoundEffect( CHARACTER p ); // AntiChrist
public:
	int GetSwingRate( int iNPC, int weapon );
	int GetArrowType( int i );
	int GetBowType(int i);
	int GetWeapon(int i);
	int CalcAtt(int p);
	int CalcDef(int p, int x);
	void CombatOnHorse(int i);
	void CombatOnFoot(int i);
	void CombatHit(int a, int d, unsigned int currenttime, signed int arrowType = -1 );
	void DoCombat(int a, unsigned int currenttime);
	void SpawnGuard( CHARACTER s, CHARACTER i, int x, int y, signed char z);
	bool weapon2Handed( int weapon );
};

class cCommands
{
public:
	void NextCall(int s, int type);
	void RepairWorld(int s);
	void KillSpawn(int s, int r);
	void RegSpawnMax(int s, int r);
	void RegSpawnNum(int s, int r, int n);
	void KillAll(int s, int percent, unsigned char * sysmsg);
	void AddHere(int s, char z);
	void ShowGMQue(int s, int type);
	void Wipe(int s);
	void CPage(int s, char * reason);
	void GMPage(int s, char * reason);
	void MakePlace(int s, int i);
	void Command( UOXSOCKET s );
	void MakeShop(int c);
	void RemoveShop(int s);
	void DyeItem(int s);
	void SetItemTrigger(int s);
	void SetTriggerType(int s);
	void SetTriggerWord(int s);
	void SetNPCTrigger(int s);
	void DupeItem(int s, int i, int amount);
	void Possess(int s);
	void Load( void );
	signed int FindIndex( char *toFind );
	int cmd_offset;
};

class cGuilds
{
private:
	void EraseMember(int c);
	void EraseGuild(int guildnumber);
	void ToggleAbbreviation(int s);
	int SearchSlot(int guildnumber, int type);
	void ChangeName(int s, char *text);
	void ChangeAbbreviation(int s, char *text);
	void ChangeTitle(int s, char *text);
	void ChangeCharter(int s, char *text);
	void ChangeWebpage(int s, char *text);
	int CheckValidPlace(int x, int y);
	void Broadcast(int guildnumber, char *text);
	void CalcMaster(int guildnumber);
	void SetType(int guildnumber, int type);
public:
	guild_st guilds[MAXGUILDS]; //lb, moved from uox3.h cauz global variabels cant be changed in constuctors ...
	cGuilds();
	virtual ~cGuilds();
	void StonePlacement(int s);
	void Menu(int s, int page);
	void Resign(int s);
	void Recruit(int s);
	void TargetWar(int s);
	void StoneMove(int s);
	int Compare(int player1, int player2);
	void GumpInput(int s, int type, int index, char *text);
	void GumpChoice(int s, int main, int sub);
	int SearchByStone(int s);
	//void Init();
	void Title(int s, int player2);
	void Read(int guildnumber);
	void Write( FILE *wscfile );
};

class cGump
{
public:
	void Button(int s, int button, unsigned char tser1, unsigned char tser2, unsigned char tser3, unsigned char tser4, char type);
	void Input( UOXSOCKET s );
	void Menu(int s, int m);
	void Open(int s, int i, int num, int num2);
};

class cItem
{
public:
	void GlowItem( UOXSOCKET s, int i );
	int MemItemFree();
	void DeleItem(int i);
	int CreateScriptItem(int s, int itemnum, int nSpawned);
	int CreateRandomItem(char *sItemList);
	int CreateScriptRandomItem(int s, char *sItemList);
	int SpawnItem(UOXSOCKET nSocket, CHARACTER ch, int nAmount, char* cName, int nStackable,
        unsigned char cItemId1, unsigned char cItemId2, unsigned char cColorId1, unsigned char cColorId2, int nPack, int nSend);
	int SpawnItem(UOXSOCKET nSocket, int nAmount, char* cName, int nStackable, unsigned char cItemId1, unsigned char cItemId2, unsigned char cColorId1, unsigned char cColorId2, int nPack, int nSend);
	int SpawnItemBackpack2(UOXSOCKET s, CHARACTER ch, int nItem, int nDigging);
	void GetScriptItemSetting( int c ); // by Magius (CHE)
	void DecayItem(unsigned int currenttime, int i);
	void Decay(unsigned int currenttime);
	void RespawnItem(unsigned int Currenttime, int i);
	void AddRespawnItem(int s, int x, int y);
	void AddRandomItem(int s, char *itemlist, int spawnpoint);
	void InitItem(int nItem, char ser=1);
	char isFieldSpellItem(int i);
	bool isShieldType( int i );
	ARMORCLASS ArmorClass( ITEM i );
	unsigned char PackType( unsigned char id1, unsigned char id2 );
	void CheckEquipment( CHARACTER p );  // AntiChrist
	void BounceInBackpack( CHARACTER p, ITEM i ); // AntiChrist
	void BounceItemOnGround( CHARACTER p, ITEM i ); // AntiChrist
};

// full comments on this class are available in mapstuff.cpp
class MapStaticIterator
{
private:
	staticrecord staticArray;
	SI32 baseX, baseY, pos;
	unsigned char remainX, remainY;
	UI32 index, length, tileid;
	bool exactCoords;

public:
	MapStaticIterator(unsigned int x, unsigned int y, bool exact = true);
	~MapStaticIterator() { };

	staticrecord *First();
	staticrecord *Next();
	void GetTile(tile_st *tile) const;
	UI32 GetPos() const { return pos; }
	UI32 GetLength() const { return length; }
};

const int MapTileWidth = 768;
const int MapTileHeight = 512;

class cMapStuff
{
//Variables
private:
	friend class MapStaticIterator;

        // moved from global vars into here - fur 11/3/1999
        UOXFile *mapfile, *sidxfile, *statfile, *verfile, *tilefile, *multifile, *midxfile;

	// tile caching items
	tile_st tilecache[0x4000];

	// static caching items
	unsigned long StaticBlocks;
	struct StaCache_st
	{
		staticrecord *Cache;
		unsigned short CacheLen;   // i've seen this goto to at least 273 - fur 10/29/1999
	};

	// map caching items
	struct MapCache
	{
		unsigned short xb;
		unsigned short yb;
		unsigned char  xo;
		unsigned char  yo;
		map_st Cache;
	};
	MapCache Map0Cache[MAP0CACHE];

	// version caching items
	versionrecord *versionCache;
	UI32 versionRecordCount;

	// caching functions
	void CacheTiles( void );
	void CacheStatics( void );

public:
	// these used to be [512], thats a little excessive for a filename.. - fur
	char mapname[80], sidxname[80], statname[80], vername[80],
	  tilename[80], multiname[80], midxname[80];
	unsigned long StaMem, TileMem, versionMemory;
	unsigned int Map0CacheHit, Map0CacheMiss;
	// ok this is rather silly, allocating all the memory for the cache, even if
	// they haven't chosen to cache?? - fur
	StaCache_st StaticCache[MapTileWidth][MapTileHeight];
	unsigned char Cache;
	
// Functions
private:
	char VerLand(int landnum, land_st *land);
	signed char MultiHeight(int i, short int x, short int y, signed char oldz);
	int MultiTile(int i, short int x, short int y, signed char oldz);
	SI32 VerSeek(SI32 file, SI32 block);
	char VerTile(int tilenum, tile_st *tile);
	bool TileWalk(int tilenum);
	void CacheVersion();

	int DynTile( short int x, short int y, signed char oldz );
	bool DoesTileBlock(int tilenum);
	bool DoesStaticBlock(short int x, short int y, signed char oldz);

public:
	cMapStuff();
	~cMapStuff();

	void Load();

	// height functions
	bool IsUnderRoof(short int x, short int y, signed char z);
	signed char StaticTop(short int x, short int y, signed char oldz);
	signed char DynamicElevation(short int x, short int y, signed char oldz);
	signed char MapElevation(short int x, short int y);
	signed char AverageMapElevation(short int x, short int y, int &id);
	signed char TileHeight( int tilenum );
	signed char Height(short int x, short int y, signed char oldz);

	// look at tile functions
	void MultiArea(int i, int *x1, int *y1, int *x2, int *y2);
	void SeekTile(int tilenum, tile_st *tile);
	void SeekMulti(int multinum, UOXFile **mfile, SI32 *length);
	void SeekLand(int landnum, land_st *land);
	map_st SeekMap0( unsigned short x, unsigned short y );
	bool IsRoofOrFloorTile( tile_st *tile );
	bool IsRoofOrFloorTile( unitile_st *tile );
	bool IsTileWet(int tilenum);

	// misc functions
	bool CanMonsterMoveHere( short int x, short int y, signed char z );
};

// use this value whereever you need to return an illegal z value
const signed char illegal_z = -128;	// reduced from -1280 to -128, to fit in with a valid signed char

class cCharStuff
{
private:
	void FindSpotForNPC(int c, int originX, int originY, int xAway, int yAway, int elev);
	int  SearchSubPackForItem( ITEM toSearch, unsigned char type );
	int  SearchSubPackForItem( ITEM toSearch, unsigned char id1, unsigned char id2 );

public:
	void DeleteChar(int k);
	int MemCharFree();
	int AddRandomLoot(int s, char * lootlist);
	int AddRandomNPC(int s, char *npclist, int spawnpoint);
	int AddRespawnNPC(int s, int region, int npcNum, int type);
	int AddNPCxyz(int s, int npcNum, int type, int x1, int y1, signed char z1);
	int Split(int k);
	void CheckAI(unsigned int currenttime, int i);
	void InitChar(int nChar, char ser=1);
	int FindItem( CHARACTER toFind, unsigned char type );
	int FindItem( CHARACTER toFind, unsigned char id1, unsigned char id2 );
};

class cSkills
{
private:
	int GetSubIngotAmt(int p, char id1, char id2, char color1, char color2);
	void DeleIngot(int s, int id1, int id2, int color1, int color2, int amount);
	int DeleSubIngot(int p, int id1, int id2, int color1, int color2, int amount);
	void AnvilTarget( int s, item_st& item, char *ingotName, int makemenu = 1 );
	void Atrophy( CHARACTER c, unsigned short sk );
	void Hide(int s);
	void Stealth(int s);
	void PeaceMaking(int s);
	void PlayInstrumentWell( UOXSOCKET s, int i);
	void PlayInstrumentPoor( UOXSOCKET s, int i);
	int GetInstrument(int s);
	void RandomSteal(int s);
	int TrackingDirection(int s, int i);
	void TellScroll(char *menu_name, int player, long item_param);
	void CollectAmmo(int s, int a, int b);
	void Meditation( UOXSOCKET s );
	int CalcRank( int s, int skill ); // by Magius(CHE)
	void ApplyRank( int s, int c, int rank ); // by Magius(CHE)
	void Zero_Itemmake( int s ); // by Magius(CHE)

public:
	void Tracking( int s, int selection );
	void CreatePotion(int s, char type, char sub, int mortar);
	void Fish(unsigned int i);
	int GetIngotAmt(int s, unsigned char id1, unsigned char id2, unsigned char color1, unsigned char color2);
	char AdvanceSkill(int s, int sk, char skillused);
	void AdvanceStats(int s, int sk);
	void TinkerAxel(int s);
	void TinkerAwg(int s);
	void TinkerClock(int s);
	void MakeDough(int s);
	void MakePizza(int s);
	void Track(int i);
	void DoPotion(int s, int type, int sub, int mortar);
	void Tailoring( UOXSOCKET s );
	void Fletching(int s);
	void BowCraft(int s);
	void Carpentry(int s);
	void Smith( UOXSOCKET s );
	void Repair( UOXSOCKET s );
	void MakeMenuTarget(int s, int x, int skill);
	void MakeMenu(int s, int m, int skill);
	void Mine(int s);
	void GraveDig(int s);
	void SmeltOre( UOXSOCKET s );
	void Wheel(int s, int mat);
	void Loom(int s);
	void CookMeat(int s);
	void TreeTarget(int s);
	void DetectHidden(int s);
	void ProvocationTarget1( UOXSOCKET s );
	void ProvocationTarget2( UOXSOCKET s );
	void EnticementTarget1( UOXSOCKET s );
	void EnticementTarget2( UOXSOCKET s );
	void AlchemyTarget(int s);
	void BottleTarget(int s);
	void PotionToBottle(int s, int mortar);
	char CheckSkill(int s, int sk, int low, int high);
	void CreateBandageTarget(int s);
	void HealingSkillTarget(int s);
	void SpiritSpeak(int s);
	void ArmsLoreTarget(int s);
	void ItemIdTarget(int s);
	void Evaluate_int_Target(int s);
	void AnatomyTarget(int s);
	void TameTarget(int s);
	void FishTarget(int s);
	int GetCombatSkill(int i);
	int GetShield(int i);
	ITEM GetSecondHand( CHARACTER i );
	void SkillUse(int s, int x);
	void StealingTarget(int s);
	void CreateTrackingMenu(int s, int m);
	void TrackingMenu(int s, int gmindex);
	void BeggingTarget(int s);
	void AnimalLoreTarget(int s);
	void ForensicsTarget(int s);
	void PoisoningTarget( UOXSOCKET s );
	int Inscribe( UOXSOCKET s, long snum);
	int EngraveAction(int s, int i, int cir, int spl);
	void updateSkillLevel(int c, int s);
	void LockPick(UOXSOCKET s);
	void TDummy(int s);
	void NewDummy(unsigned int currenttime);
	void Tinkering(int s);
	void AButte(int s1, int x);
	void Persecute( UOXSOCKET s ); // AntiChrist persecute stuff
    void Snooping( UOXSOCKET s, CHARACTER target, long serial);
};

class cTownStones
{
private:
	void Line(int line, int j, char type, int s);
	void Text(int line, int j, char type, int s);
	const char *TownMayor(int j);
	const char *MayorVote(int s);
	int Population(int j);
public:
	cTownStones(); // LB might not work, but I dont care because nobody uses that unfished thing anymore
	virtual ~cTownStones();
	void Menu(int s, int j, int type);
	const char *TownName(int s, int type);
	void CalcNewMayor(int j);
	void VoteForMayorTarget(int s);
};

class cWeight
{
private:
	double RecursePacks(int bp);
	int WhereSub(int i, int p);
	char CheckMaxWeightPack(int chars);
	void AddItemWeight(int i, int s);
	void SubtractItemWeight(int i, int s);
	void SubtractQuanWeight(int i, int s, int total);
	void AddAmountWeight(int i, int s, int amount);
	int WhereItem(int p, int i, int s);
public:
	int CheckWeight(int s, int k);
	int CheckWeight2(int s);
	void NewCalc(int p);
	double CalcWeightPack(int backpack);
	int ItemWeight(int item);
	int CheckWhereItem(int pack, int i, int s);
};

class cTargets
{
private:
	void CommandLevelTarget( UOXSOCKET s ) ;
	void GuardTarget( UOXSOCKET s );
	void GlowTarget( UOXSOCKET s );
	void UnglowTarget( UOXSOCKET s );
	void PlVBuy(int s);
	void RenameTarget(int s);
	void AddTarget(int s);
	void TeleTarget( UOXSOCKET s );
	void RemoveTarget(int s);
	void DyeTarget(int s);
	void NewzTarget(int s);
	void TypeTarget(int s);
	void XgoTarget(int s);
	void MoreXYZTarget(int s);
	void MoreXTarget(int s);
	void MoreYTarget(int s);
	void MoreZTarget(int s);
	void PrivTarget(int s);
	void MoreTarget(int s);
	void KeyTarget(int s);
	void IstatsTarget(int s);
	void WstatsTarget(int s);
	void GMTarget(int s);
	void CnsTarget(int s);
	void KillTarget(int s, int ly);
	void FontTarget(int s);
	void GhostTarget(int s);
	void AmountTarget(int s);
	void SetAmount2Target(int s);
	void CloseTarget(int s);
	void VisibleTarget( UOXSOCKET s );
	void OwnerTarget(int s);
	void ColorsTarget(int s);
	void DvatTarget(int s);
	void AddNpcTarget(int s);
	void FreezeTarget(int s);
	void UnfreezeTarget(int s);
	void AllSetTarget(int s);
	void InfoTarget(int s);
	void LoadCannon(int s);
	void SetInvulFlag(int s);
	void Tiling(int s);
	void ExpPotionTarget(int s);
	void SquelchTarg(int s);
	void TeleStuff(int s);
	void SwordTarget(int s);
	void CorpseTarget(int s);
	void CarveTarget( UOXSOCKET s, int feat, int ribs, int hides, int fur, int wool);
	void newCarveTarget( UOXSOCKET s, ITEM i );
	void TitleTarget(int s);
	void NpcTarget( UOXSOCKET s );
	void NpcTarget2( UOXSOCKET s );
	void NpcRectTarget( UOXSOCKET s );
	void NpcCircleTarget( UOXSOCKET s );
	void NpcWanderTarget( UOXSOCKET s );
	void NpcAITarget(int s);
	void xBankTarget(int s);
	void xSpecialBankTarget( int s ); // AntiChrist
	void DupeTarget(int s);
	void MoveToBagTarget(int s);
	void SellStuffTarget(int s);
	void GmOpenTarget(int s);
	void StaminaTarget(int s);
	void ManaTarget(int s);
	void MakeShopTarget(int s);
	void AttackTarget( UOXSOCKET s );
	void FollowTarget(int s);
	void TransferTarget(int s);
	void BuyShopTarget(int s);
	void SetValueTarget(int s);
	void SetRestockTarget(int s);
	void permHideTarget(int s);
	void unHideTarget(int s);
	void SetWipeTarget(int s);
	void SetSpeechTarget(int s);
	void SetSpAttackTarget(int s);
	void SetPoisonTarget(int s);
	void SetPoisonedTarget(int s);
	void FullStatsTarget(int s);
	void SetAdvObjTarget(int s);
	void CanTrainTarget(int s);
	void SetSplitTarget(int s);
	void SetSplitChanceTarget(int s);
	void AxeTarget(int s);
	void ObjPrivTarget(int s);
	void SetSpaDelayTarget(int s);
	void NewXTarget(int s);
	void NewYTarget(int s);
	void IncXTarget(int s);
	void IncYTarget(int s);
	void IncZTarget( UOXSOCKET s );
	void BoltTarget(int s);
	void MovableTarget(int s);
	void SetDirTarget(int s);
	void HouseOwnerTarget(int s);
	void HouseEjectTarget(int s);
	void HouseBanTarget(int s);
	void HouseFriendTarget(int s);
	void HouseUnlistTarget(int s);
	void BanTarg(int s);
	void triggertarget( int ts ); // By Magius(CHE)
	void ShowSkillTarget( int s );
	void ResurrectionTarget( UOXSOCKET s );
	void SetMurderCount( int s );	// Abaddon 12 Sept 1999
	void HouseLockdown( UOXSOCKET s ); // Abaddon 17th December, 1999
	void HouseRelease( UOXSOCKET s ); // Abaddon 17th December 1999
	void ShowDetail( UOXSOCKET s ); // Abaddon 11th January, 2000
public:
	void CstatsTarget( UOXSOCKET s );
	void TweakTarget( UOXSOCKET s );
	void GetAccount( int s );
	void IDtarget(int s);
	void MultiTarget(int s);
	void Wiping(int s);
	int NpcMenuTarget(int s);
	void NpcResurrectTarget(int s);
	void JailTarget(int s, int c);
	void ReleaseTarget(int s, int c);
	int AddMenuTarget(int s, int x, int addmitem);
	void XTeleport(int s, int x);
	int BuyShop(int s,int c);
};

class cNetworkStuff
{
public:
	cNetworkStuff();
	~cNetworkStuff();
	void xSend(int s, void *point, int length, int test);
	void Disconnect(int s);
	void ClearBuffers();
	void CheckConn();
	void CheckMessage();
	void SockClose();
	void FlushBuffer(int s);
	int kr,faul; // needed because global varaibles cant be changes in constructores LB
private:

	int cNetworkStuff::Authenticate (const char *login, const char *password);

	void GenTable(int s, char a1, char a2, char a3, char a4);
	void DoStreamCode(int s);
	int xRecv(int s);
	void Processed(int s, int i);
	void Login1(int s);
	void Relay(int s);
	void GoodAuth(int s);
	void FailAuth(int s);
	void AuthTest(int s);
	void CharList(int s);
	int Receive(int s, int x, int a);
	void GetMsg(int s);
	void LogOut( UOXSOCKET s );
	void pSplit(char *pass0);
	void sockInit( void );
	int Pack(void *pvIn, void *pvOut, int len);
	
};

class cMagic
{
public:
	//:Terrin: adding constructor/destructor
	cMagic();
	~cMagic(); // no need for a virtual destructor as long as no subclasses us

	void AddSpell( int book, int spellNum );
	unsigned char HasSpell( int book, int spellNum );

	void LoadScript( void ); //:Terrin: adding function for spell system "cache"
	void SpellBook( UOXSOCKET s );
	char GateCollision(int s);
	void GateDestruction(unsigned int currenttime);
	bool newSelectSpell2Cast( UOXSOCKET s, int num );
	bool requireTarget( unsigned char num );
	bool reqItemTarget( int num );
	bool reqLocTarget( int num );
	bool reqCharTarget( int num );
	bool spellReflectable( int num );
	bool travelSpell( int num );
	stat_st getStatEffects( int num );
	move_st getMoveEffects( int num );
	sound_st getSoundEffects( int num );
	bool aggressiveSpell( int num );
	bool fieldSpell( int num );
	void doMoveEffect( int num, int target, int source );
	void doStaticEffect( int source, int num );
	void playSound( int source, int num );
	void NewDelReagents( CHARACTER s, reag_st reags );
	void NewCastSpell( UOXSOCKET s );
	void SbOpenContainer( UOXSOCKET s );
	char CheckResist( CHARACTER attacker, CHARACTER defender, int circle );	// umm, why?
	void PoisonDamage(int p, int posion);
	void CheckFieldEffects2(unsigned int currenttime, int c, char timecheck);
	void PFireballTarget(int i, int k, int j);
	void NPCFireballTarget(int nAttacker, int nDefender);
	void NPCLightningTarget(int nAttacker, int nDefender);
	void NPCCurseTarget(int nAttacker, int nDefender);
	void NPCWeakenTarget(int s, int t);
	void NPCFeebleMindTarget(int s, int t);
	void NPCCLumsyTarget(int s, int t);
	void NPCMindBlastTarget(int s, int t);
	void NPCMagicArrowTarget(int s, int t);
	void NPCClumsyTarget(int s, int t);
	void NPCHarmTarget(int s, int t);
	void NPCHarmtarget(int s, int t);
	void NPCParalyzeTarget(int s, int t);
	void NPCEBoltTarget(int s, int t);
	void NPCExplosionTarget(int s, int t);
	void NPCDispel( CHARACTER s, CHARACTER i );
	char CheckParry(int player, int circle);
	void NPCFlameStrikeTarget(int s, int t);
	void MagicArrowSpellItem(int attacker, int defender);
	void ClumsySpellItem(int attacker, int defender);
	void FeebleMindSpellItem(int attacker, int defender);
	void WeakenSpellItem(int attacker, int defender);
	void HarmSpellItem(int attacker, int defender);
	void FireballSpellItem(int attacker, int defender);
	void CurseSpellItem(int attacker, int defender);
	void LightningSpellItem(int attacker, int defender);
	void MindBlastSpellItem(int attacker, int defender);
	void ParalyzeSpellItem(int attacker, int defender);
	void ExplosionSpellItem(int attacker, int defender);
	void FlameStrikeSpellItem(int attacker, int defender);
	int CheckBook(int circle, int spell, int i);
	char CheckReagents( int s, reag_st reagents );
	char CheckMana( int s, int num );
	bool CheckStamina( CHARACTER s, int num );
	bool CheckHealth( CHARACTER s, int num );
	void DeleReagents(int s, int ash, int drake, int garlic, int ginseng, int moss, int pearl, int shade, int silk);
	void Recall( UOXSOCKET s); // we need these four for the commands (separately) !!!
	void Mark( UOXSOCKET s);
	void Gate( UOXSOCKET s);
	void Heal( UOXSOCKET s);
	char CheckMagicReflect(int i);
	void MagicDamage(int p, int amount, CHARACTER attacker = -1 );
	void SpellFail( UOXSOCKET s );
	char SubtractMana(int s, int mana);
	char SubtractStamina( CHARACTER s, int stamina );
	char SubtractHealth( CHARACTER s, int health, int spellNum );
	void MagicTrap(int s, int i);	// moved here by AntiChrist (9/99)
	void Polymorph( int s, int gmindex, int creaturenumer); // added by AntiChrist (9/99)


private:
	void SummonMonster( UOXSOCKET s, unsigned char d1, unsigned char id2, char *monstername, unsigned char color1, unsigned char color2, int x, int y, int z);
	void MagicReflect(int s);
	void BoxSpell( UOXSOCKET s, int& x1, int& x2, int& y1, int& y2, int& z1, int& z2);
	int RegMsg( CHARACTER s, reag_st failmsg );
	void DirectDamage(int p, int amount);
	void PolymorphMenu( int s, int gmindex ); // added by AntiChrist (9/99)

};

class cMovement
{
	// Variable/Type definitions
private:
	signed char z, dispz;
	// Function declarations
public:
	void Walking( CHARACTER s, int dir, int seq );
	void CombatWalk( int s );
	int  calc_walk( CHARACTER c, unsigned int x, unsigned int y, unsigned int oldx, unsigned int oldy, bool justask );
	bool calc_move( CHARACTER c, short int x, short int y, signed char &z, int dir );
	int validNPCMove( short int x, short int y, signed char z, CHARACTER s );
	void NpcMovement( unsigned int currenttime, int i );
private:

	bool MoveHeightAdjustment( int MoveType, unitile_st *thisblock, int &ontype, signed int &nItemTop, signed int &nNewZ );
	bool isValidDirection(int dir);
	bool isFrozen(CHARACTER c, UOXSOCKET socket, int sequence);
	bool isOverloaded(CHARACTER c, UOXSOCKET socket, int sequence);

	bool CanGMWalk(unitile_st xyb);
	bool CanPlayerWalk(unitile_st xyb);
	bool CanNPCWalk(unitile_st xyb);
	bool CanFishWalk(unitile_st xyb);
	bool CanBirdWalk(unitile_st xyb);

	void FillXYBlockStuff(short int x, short int y, unitile_st *xyblock, int &xycount);
	void GetBlockingMap(SI16 x, SI16 y, unitile_st *xyblock, int &xycount, unsigned short oldx, unsigned short oldy );
	void GetBlockingStatics(SI16 x, SI16 y, unitile_st *xyblock, int &xycount);
	void GetBlockingDynamics(SI16 x, SI16 y, unitile_st *xyblock, int &xycount);

	short int Distance(short int sx, short int sy, short int dx, short int dy);
	short int Direction(short int sx, short int sy, short int dx, short int dy);

	short int CheckMovementType(CHARACTER c);
	bool CheckForCharacterAtXY(CHARACTER c);
	bool CheckForCharacterAtXYZ(CHARACTER c, short int cx, short int cy, signed char cz);
	void NpcWalk( CHARACTER i, int j, int type );
	unsigned short GetXfromDir( int dir, unsigned short x );
	unsigned short GetYfromDir( int dir, unsigned short y );
	void PathFind( CHARACTER c, unsigned short gx, unsigned short gy );

	bool VerifySequence(CHARACTER c, UOXSOCKET socket, int sequence);
	bool CheckForRunning(CHARACTER c, UOXSOCKET socket, int dir);
	bool CheckForStealth(CHARACTER c, UOXSOCKET socket);
	bool CheckForHouseBan(CHARACTER c, UOXSOCKET socket);
	void MoveCharForDirection(CHARACTER c, int dir);
	void HandleRegionStuffAfterMove(CHARACTER c, short int oldx, short int oldy);
	void SendWalkToPlayer(CHARACTER c, UOXSOCKET socket, short int sequence);
	void SendWalkToOtherPlayers(CHARACTER c, int dir, short int oldx, short int oldy);
	void OutputShoveMessage(CHARACTER c, UOXSOCKET socket, short int oldx, short int oldy);
	void HandleItemCollision( CHARACTER c, UOXSOCKET socket, bool amTurning, unsigned short oldx, unsigned short oldy );
	void HandleTeleporters(CHARACTER c, UOXSOCKET socket, short int oldx, short int oldy);
	void HandleWeatherChanges(CHARACTER c, UOXSOCKET socket);
	void HandleGlowItems(CHARACTER c, UOXSOCKET socket);
	bool IsGMBody(CHARACTER c);
	signed char CheckWalkable( CHARACTER c, unitile_st *xyblock, int xycount );
	bool CrazyXYBlockStuff(CHARACTER c, UOXSOCKET socket, short int oldx, short int oldy, int sequence);
	void FillXYBlockStuff(CHARACTER c, unitile_st *xyblock, int &xycount, unsigned short oldx, unsigned short oldy );
	void deny(int k, int s, int sequence);
};

// This class is designed as a replacement for the teffect array (which is nasty, and too big)
// It'll grow and shrink as required, and will hopefully help reduce memory troubles
class cTEffect
{
private:
	vector< teffect_st * > internalData;
	UI16 effectCount;
	UI16 currentEffect;
	bool delFlag;

public:
	cTEffect();
	~cTEffect();
	teffect_st *First( void );				// returns the first entry
	teffect_st *Current( void );			// returns current entry
	teffect_st *Next( void );				// returns the next entry
	bool AtEnd( void );						// have we hit the end?
	bool Add( teffect_st toAdd );			// adds another entry
	bool DelCurrent( void );				// deletes the current entry
	UI16 Count( void );						// returns count of number of effects
	teffect_st *GrabSpecific( UI16 index );	// grabs a specific index
};

class cWhoList
{
private:
	bool needsUpdating;					// true if the list needs updating (new player online, new char made)
	int  gmCount;						// number of GMs already in it
	bool	  online;

	vector< SERIAL > whoMenuData;
	stringList one, two;				// replacement for entries1, entries2

	void Update( void );				// force the list to update
	void ResetUpdateFlag( void );
	void AddSerial( SERIAL toAdd );
	void Delete( void );
	void Command( UOXSOCKET toSendTo, unsigned char type, unsigned short int buttonPressed );
public:
	cWhoList( bool trulyOnline = true );
	~cWhoList();
	void FlagUpdate( void );
	void SendSocket( UOXSOCKET toSendTo, unsigned char option = 1 );
	void GMEnter( void );
	void GMLeave( void );
	long int GrabSerial( int index );
	void ButtonSelect( UOXSOCKET toSendTo, unsigned short int buttonPressed, unsigned char type );
	void ZeroWho( void );
	void SetOnline( bool newValue );
};

#endif
