#ifndef __CITEM_H__
#define __CITEM_H__
#include "GenericList.h"
#include <array>
enum CITempVars
{
	CITV_MORE	= 0,
	CITV_MOREX,
	CITV_MOREY,
	CITV_MOREZ,
	CITV_MORE0,
	CITV_MORE1,
	CITV_MORE2,
	CITV_COUNT
};

class CItem : public CBaseObject
{
protected:
	GenericList<CItem *>	Contains;
	GenericList<CSocket *>	contOpenedBy;

	CBaseObject * 	contObj;
	UI08			glowEffect;
	SERIAL			glow;			// identifies glowing objects
	COLOUR			glowColour;
	SI08			madeWith;		// Store the skills used to make this item
	SI32			rndValueRate;	// Store the value calculated base on RANDOMVALUE in region.dfn
	SI16			good;			// Store type of GOODs to trade system! (Plz not set as UNSIGNED)

	// for example: RANK 5 --> 5*10 = 50% of malus
	// this item has same values decreased by 50%..
	// RANK 1 --> 1*10=10% this item has 90% of malus!
	// RANK 10 --> 10*10 = 100% this item has no malus! RANK 10 is automatically setted if you select RANKSYSTEM 0.
	// Vars: LODAMAGE, HIDAMAGE, ATT, DEF, HP, MAXHP
	SI08			rank;			// for rank system, this value is the LEVEL of the item from 1 to 10.  Simply multiply t he rank*10 and calculate the MALUS this item has from the original.
	ARMORCLASS		armorClass;
	UI16			restock;		// Number up to which shopkeeper should restock this item
	SI08			movable;		// 0=Default as stored in client, 1=Always movable, 2=Never movable, 3=Owner movable.
	TIMERVAL		tempTimer;
	TIMERVAL		decayTime;
	UI08			spd;			// The speed of the weapon
	UI16			maxHp;			// Max number of hit points an item can have.
	UI16			amount;			// Amount of items in pile
	ItemLayers		layer;			// Layer if equipped on paperdoll
	ItemTypes		type;			// For things that do special things on doubleclicking
	SI08			offspell;
	UI16			entryMadeFrom;
	SERIAL			creator;		// Store the serial of the player made this item
	SI08			gridLoc;
	SI16			artifactRarity;
	SI16			durabilityHpBonus;
	SI32			weightMax;		// Maximum weight a container can hold
	SI32			baseWeight;		// Base weight of item. Applied when item is created for the first time, based on weight. Primarily used to determine base weight of containers
	UI16			maxItems;		// Maximum amount of items a container can hold
	UI08			maxRange;		// Max range of ranged weapon
	UI08			baseRange;		// Base range of thrown weapon
	UI16			maxUses;		// Max number of uses an item can have
	UI16			usesLeft;		// Current number of uses left on an item
	UI16			regionNum;
	TIMERVAL		tempLastTraded;		// Temporary timestamp for when item was last traded between players via secure trade window (not saved)
	UI08			stealable;		// 0=Not stealable, 1=Stealable (default, most items), 2=Special Stealable (town rares, etc)

	std::bitset<8>	bools;
	std::bitset<8>	priv; 			// Bit 0, decay off/on.  Bit 1, newbie item off/on.  Bit 2 Dispellable

	std::string 	name2;
	std::string		desc;
	std::string		eventName;		// Name of custom event item belongs to

	UI32			tempVars[CITV_COUNT];
	UI08			dir;			// direction an item can have

	UI32			value[3];		// Price a shopkeep buys and sells items for, with price on player vendor as optional third value
	UI16			ammo[2];		// Ammo ID and Hue
	UI16			ammoFX[3];		// Ammo-effect ID, Hue and rendermode

	std::bitset<WEATHNUM>	weatherBools;	// For elemental weaponry.  So a Heat weapon would be a fire weapon, and does elemental damage to Heat weak races

	auto			RemoveSelfFromCont() -> void;
	virtual void	RemoveSelfFromOwner( void ) override;
	virtual void	AddSelfToOwner( void ) override;

	auto			CheckItemIntegrity() -> void;
	virtual bool	DumpHeader( std::ostream &outStream ) const override;
	virtual bool	LoadRemnants( void ) override;

	UI32			spells[3];		// For spellbooks (eventually should be a derived class)

	auto			CopyData( CItem *target ) -> void;

public:

	auto	GetContainsList() -> GenericList<CItem *> *;
	auto	GetContOpenedByList() -> GenericList<CSocket *> *;

	virtual void	SetWeight( SI32 newVal, bool doWeightUpdate = true ) override;
	auto			EntryMadeFrom() const -> UI16;
	auto			EntryMadeFrom( UI16 newValue ) -> void;

	auto			GetWeatherDamage( WeatherType effectNum ) const -> bool;
	auto			SetWeatherDamage( WeatherType effectNum, bool value ) -> void;

	auto			Dupe( ObjectType itemType = OT_ITEM ) -> CItem *;

	auto			GetCont() const -> CBaseObject *;
	auto			GetContSerial() const -> SERIAL;

	auto			SetCont(   CBaseObject *newCont, bool removeFromView = false ) -> bool;
	auto			SetContSerial( SERIAL newSerial ) -> bool;

	auto			GetGridLocation() const -> SI08;
	auto			SetGridLocation( SI08 newLoc ) -> void;

	SI16			GetArtifactRarity(void) const;
	void			SetArtifactRarity(SI16 newValue);

	SI16			GetDurabilityHpBonus(void) const;
	void			SetDurabilityHpBonus(SI16 newValue);

	auto			GetStealable() const -> UI08;
	auto			SetStealable( UI08 newValue ) -> void;

	auto			IsDoorOpen() const -> bool;
	auto			IsPileable() const -> bool;
	auto			IsDyeable() const -> bool;
	auto			IsCorpse() const -> bool;
	auto			IsHeldOnCursor() const -> bool;
	auto			IsGuarded() const -> bool;
	auto			IsSpawnerList() const -> bool;
	auto			IsMarkedByMaker() const -> bool;

	auto			IsNewbie() const -> bool;
	auto			IsDecayable() const -> bool;
	auto			IsDispellable() const -> bool;
	auto			IsDivineLocked() const -> bool;

	auto			SetDoorOpen( bool newValue ) -> void;
	auto			SetPileable( bool newValue ) -> void;
	auto			SetDye( bool newValue ) -> void;
	auto			SetCorpse( bool newValue ) -> void;
	auto			SetHeldOnCursor( bool newValue ) -> void;
	virtual void	SetGuarded( bool newValue );
	auto			SetSpawnerList( bool newValue ) -> void;
	auto			SetMakersMark( bool newValue ) -> void;

	auto			SetNewbie( bool newValue ) -> void;
	auto			SetDecayable( bool newValue ) -> void;
	auto			SetDispellable( bool newValue ) -> void;
	auto			SetDivineLock( bool newValue ) -> void;

	auto			GetName2() const -> const std::string&;
	auto			GetCreator() const -> SERIAL;
	auto			GetDesc() const -> std::string;
	auto 			GetEvent() const -> std::string;

	auto			SetName2( const std::string &value ) -> void;
	auto			SetCreator( SERIAL newValue ) -> void;
	auto			SetDesc( std::string newValue ) -> void;
	auto			SetEvent( std::string newValue ) -> void;

	auto			PlaceInPack() -> void;
	virtual void	SetOldLocation( SI16 newX, SI16 newY, SI08 newZ ) override;
	virtual void	SetLocation( const CBaseObject *toSet ) override;
	virtual void	SetLocation( SI16 newX, SI16 newY, SI08 newZ ) override;
	virtual void	SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world, UI16 instanceId ) override;
	virtual void	SetLocation( SI16 newX, SI16 newY, SI08 newZ, SI08 newLoc, UI08 world, UI16 instanceId );
	auto			IncZ( SI16 newValue ) -> void;
	auto			IncLocation( SI16 xInc, SI16 yInc ) -> void;

	auto			SetRegion( UI16 newValue ) -> void;
	auto			GetRegion() const -> CTownRegion*;
	auto			GetRegionNum() const -> UI16;

	auto			InDungeon() -> bool;

	auto			GetLayer() const -> ItemLayers;
	auto			SetLayer( ItemLayers newValue ) -> void;

	auto			GetType() const -> ItemTypes;
	auto			SetType( ItemTypes newValue ) -> void;

	auto			GetOffSpell() const -> SI08;
	auto			SetOffSpell( SI08 newValue ) -> void;

	auto			GetTempVar( CITempVars whichVar ) const -> UI32;
	auto			SetTempVar( CITempVars whichVar, UI32 newVal ) -> void;
	auto			GetTempVar( CITempVars whichVar, UI08 part ) const -> UI08;
	auto			SetTempVar( CITempVars whichVar, UI08 part, UI08 newVal ) -> void;

	auto			GetAmount() const -> UI16;
	auto			SetAmount( UI32 newValue ) -> void;
	auto			IncAmount( SI32 incValue, bool noDelete = false ) -> bool;

	virtual UI16	GetMaxHP( void ) const;
	virtual void	SetMaxHP( UI16 newValue );

	auto			GetSpeed() const -> UI08;
	auto			SetSpeed( UI08 newValue ) -> void;

	auto			GetMaxRange() const -> UI08;
	auto			SetMaxRange( UI08 newValue ) -> void;

	auto			GetBaseRange() const -> UI08;
	auto			SetBaseRange( UI08 newValue ) -> void;

	auto			GetMovable() const -> SI08;
	auto			SetMovable( SI08 newValue ) -> void;

	auto			GetTempLastTraded() const -> TIMERVAL;
	auto			GetTempTimer() const -> TIMERVAL;
	auto			GetDecayTime() const -> TIMERVAL;

	auto			SetTempLastTraded( TIMERVAL newValue ) -> void;
	auto			SetTempTimer( TIMERVAL newValue ) -> void;
	auto			SetDecayTime( TIMERVAL newValue ) -> void;

	virtual UI08	GetPriv( void ) const;
	virtual void	SetPriv( UI08 newValue );

	auto			GetBuyValue() const -> UI32;
	auto			SetBuyValue( UI32 newValue ) -> void;
	auto			GetSellValue() const -> UI32;
	auto			SetSellValue( UI32 newValue ) -> void;
	auto			GetVendorPrice() const -> UI32;
	auto			SetVendorPrice( UI32 newValue ) -> void;

	auto			GetRestock() const -> UI16;
	auto			SetRestock( UI16 newValue ) -> void;

	auto			GetMaxUses() const -> UI16;
	auto			SetMaxUses( UI16 newValue ) -> void;

	auto			GetUsesLeft() const -> UI16;
	auto			SetUsesLeft( UI16 newValue ) -> void;

	auto			GetArmourClass() const -> ARMORCLASS;
	auto			SetArmourClass( ARMORCLASS newValue ) -> void;

	auto			GetRank() const -> SI08;
	auto			SetRank( SI08 newValue ) -> void;

	auto			GetGood() const -> SI16;
	auto			SetGood( SI16 newValue ) -> void;

	auto			GetRndValueRate() const -> SI32;
	auto			SetRndValueRate( SI32 newValue ) -> void;

	auto			GetMadeWith() const -> SI08;
	auto			SetMadeWith( SI08 newValue ) -> void;

	auto			GetAmmoId() const -> UI16;
	auto			SetAmmoId( UI16 newValue ) -> void;

	auto			GetAmmoHue() const -> UI16;
	auto			SetAmmoHue( UI16 newValue ) -> void;

	auto			GetAmmoFX() const -> UI16;
	auto			SetAmmoFX( UI16 newValue ) -> void;

	auto			GetAmmoFXHue() const -> UI16;
	auto			SetAmmoFXHue( UI16 newValue ) -> void;

	auto			GetAmmoFXRender() const -> UI16;
	auto			SetAmmoFXRender( UI16 newValue ) -> void;

	auto			GetWeightMax() const -> SI32;
	auto			SetWeightMax( SI32 newValue ) -> void;

	auto			GetBaseWeight() const -> SI32;
	auto			SetBaseWeight( SI32 newValue ) -> void;

	auto			GetMaxItems() const -> UI16;
	auto			SetMaxItems( UI16 newValue ) -> void;

	// Note: Value range to -ALLSKILLS-1 to ALLSKILLS+1
	// To calculate skill used to made this item:
	// if is a positive value, substract 1 it.
	//    Ex) madeWith = 34, 34 - 1 = 33, 33 = STEALING
	// if is a negative value, add 1 from it and invert value.
	//    Ex) madeWith = -34, -34 + 1 = -33, Abs(-33) = 33 = STEALING.
	// 0 = nullptr
	// So... a positive value is used when the item is made by a
	// player with 95.0+ at that skill. Infact in this way when
	// you click on the item appear its name and the name of the
	// creator. A negative value if the play is not skilled
	// enough!

	auto			GetGlow() const -> SERIAL;
	auto			SetGlow( SERIAL newValue ) -> void;

	auto			GetGlowColour() const -> COLOUR;
	auto			SetGlowColour( COLOUR newValue ) -> void;

	auto			GetGlowEffect() const -> UI08;
	auto			SetGlowEffect( UI08 newValue ) -> void;

	CItem();
	virtual			~CItem();

	auto			IsFieldSpell() const -> UI08;
	auto			IsLockedDown() const -> bool;
	auto			IsShieldType() const -> bool;
	auto			IsMetalType() const -> bool;
	auto			IsLeatherType() const -> bool;
	auto			CanBeLockedDown() const -> bool;
	auto			LockDown( CMultiObj *multiObj = nullptr ) -> void;
	auto			IsContType() const -> bool;
	auto			UpdateRegion() -> void;

	auto			TextMessage( CSocket *s, SI32 dictEntry, R32 secsFromNow = 0.0f, UI16 Colour = 0x005A ) -> void;
	virtual void	Update( CSocket *mSock = nullptr, bool drawGamePlayer = false, bool sendToSelf = true ) override;
	virtual void	SendToSocket( CSocket *mSock, bool drawGamePlayer = false ) override;
	auto			SendPackItemToSocket( CSocket *mSock ) -> void;
	virtual void	RemoveFromSight( CSocket *mSock = nullptr );

	virtual bool	Save( std::ostream &outStream ) override;
	virtual bool	DumpBody( std::ostream &outStream ) const override;
	virtual bool	HandleLine( std::string &UTag, std::string &data ) override;
	virtual void	PostLoadProcessing( void ) override;
	virtual void	Cleanup( void ) override;
	virtual void	Delete( void ) override;
	virtual bool	CanBeObjType( ObjectType toCompare ) const override;

	auto			GetSpell( UI08 part ) const -> UI32;
	auto			SetSpell( UI08 part, UI32 newValue ) -> void;
};

class CSpawnItem : public CItem
{
protected:
	UI08				spawnInterval[2];
	std::string			spawnSection;
	bool				isSectionAList;

	auto				CopyData( CSpawnItem *target ) -> void;
public:
	GenericList<CBaseObject *>		spawnedList;

	CSpawnItem();
	virtual				~CSpawnItem()
	{
	}

	auto				GetInterval( UI08 part ) const -> UI08;
	auto				SetInterval( UI08 part, UI08 newVal ) -> void;
	auto				GetSpawnSection() const -> std::string;
	auto				SetSpawnSection( const std::string &newVal ) -> void;
	auto				IsSectionAList() const -> bool;
	auto				IsSectionAList( bool newVal ) -> void;

	virtual bool		DumpHeader( std::ostream &outStream ) const override;
	virtual bool		DumpBody( std::ostream &outStream ) const override;

	virtual bool		HandleLine( std::string &UTag, std::string &data ) override;

	auto				DoRespawn() -> bool;	// Will replace RespawnItem() eventually
	auto				HandleItemSpawner() -> bool;
	auto				HandleNPCSpawner() -> bool;
	auto				HandleSpawnContainer() -> bool;

	virtual void		Cleanup( void ) override;

	virtual bool		CanBeObjType( ObjectType toCompare ) const override;

	auto		Dupe() -> CSpawnItem *;
};

#endif
