#ifndef __CITEM_H__
#define __CITEM_H__
#include "GenericList.h"
enum CITempVars
{
	CITV_MORE	= 0,
	CITV_MOREX,
	CITV_MOREY,
	CITV_MOREZ,
	CITV_COUNT
};

class CItem : public CBaseObject
{
protected:
	GenericList< CItem * >	Contains;

	SI08				gridLoc;

	CBaseObject	*		contObj;
	std::bitset< 8 >	bools;
	std::bitset< 8 >	priv; // Bit 0, decay off/on.  Bit 1, newbie item off/on.  Bit 2 Dispellable

	char				name2[MAX_NAME];
	SERIAL				creator;	// Store the serial of the player made this item
	std::string			desc;

	ItemLayers		layer; // Layer if equipped on paperdoll
	ItemTypes		type; // For things that do special things on doubleclicking

	SI08			offspell;
	UI32			tempVars[CITV_COUNT];
	UI16			amount; // Amount of items in pile
	UI16			maxhp; // Max number of hit points an item can have.
	UI08			spd; //The speed of the weapon
	SI08			movable; // 0=Default as stored in client, 1=Always movable, 2=Never movable, 3=Owner movable.
	UI08			dir; //direction an item can have
	TIMERVAL		tempTimer;
	TIMERVAL		decaytime;

	UI32			value[2];	// Price a shopkeep buys and sells items for
	UI16			restock; // Number up to which shopkeeper should restock this item
	ARMORCLASS		armorClass;
	SI08			rank;	// for rank system, this value is the LEVEL of the item from 1 to 10.  Simply multiply t he rank*10 and calculate the MALUS this item has from the original.
	// for example: RANK 5 --> 5*10 = 50% of malus
	// this item has same values decreased by 50%..
	// RANK 1 --> 1*10=10% this item has 90% of malus!
	// RANK 10 --> 10*10 = 100% this item has no malus! RANK 10 is automatically setted if you select RANKSYSTEM 0.
	// Vars: LODAMAGE, HIDAMAGE, ATT, DEF, HP, MAXHP
	SI16			good; // Store type of GOODs to trade system! (Plz not set as UNSIGNED)
	SI32			rndvaluerate; // Store the value calculated base on RANDOMVALUE in region.dfn
	SI08			madewith; // Store the skills used to make this item
	SERIAL			glow;	// identifies glowing objects
	COLOUR			glowColour;
	UI08			glow_effect;
	UI16			ammo[2]; // Ammo ID and Hue
	UI16			ammoFX[3]; // Ammo-effect ID, Hue and rendermode
	UI08			baseRange; // Base range of thrown weapon
	UI08			maxRange; // Max range of ranged weapon
	SI32			weightMax; //Maximum weight a container can hold
	SI32			baseWeight; //Base weight of item. Applied when item is created for the first time, based on weight. Primarily used to determine base weight of containers
	UI16			maxItems; // Maximum amount of items a container can hold

	std::bitset< WEATHNUM >	weatherBools;	// For elemental weaponry.  So a Heat weapon would be a fire weapon, and does elemental damage to Heat weak races

	void			RemoveSelfFromCont( void );
	virtual void	RemoveSelfFromOwner( void ) override;
	virtual void	AddSelfToOwner( void ) override;

	void			CheckItemIntegrity( void );
	virtual bool	DumpHeader( std::ofstream &outStream ) const override;
	virtual bool	LoadRemnants( void ) override;

	UI16			entryMadeFrom;
	UI32			spells[3];		// For spellbooks (eventually should be a derived class)

	void			CopyData( CItem *target );

public:

	GenericList< CItem * > *	GetContainsList( void );

	virtual void	SetWeight( SI32 newVal, bool doWeightUpdate = true ) override;
	UI16			EntryMadeFrom( void ) const;
	void			EntryMadeFrom( UI16 newValue );

	bool			GetWeatherDamage( WeatherType effectNum ) const;
	void			SetWeatherDamage( WeatherType effectNum, bool value );

	CItem *			Dupe( ObjectType itemType = OT_ITEM );

	CBaseObject *	GetCont(   void ) const;
	SERIAL			GetContSerial( void ) const;

	bool			SetCont(   CBaseObject *newCont );
	bool			SetContSerial( SERIAL newSerial );

	SI08			GetGridLocation( void ) const;
	void			SetGridLocation( SI08 newLoc );

	bool			isDoorOpen( void ) const;
	bool			isPileable( void ) const;
	bool			isDyeable( void ) const;
	bool			isCorpse( void ) const;
	bool			isHeldOnCursor( void ) const;
	bool			isGuarded( void ) const;
	bool			isSpawnerList( void ) const;

	bool			isNewbie( void ) const;
	bool			isDecayable( void ) const;
	bool			isDispellable( void ) const;
	bool			isDivineLocked( void ) const;

	void			SetDoorOpen( bool newValue );
	void			SetPileable( bool newValue );
	void			SetDye( bool newValue );
	void			SetCorpse( bool newValue );
	void			SetHeldOnCursor( bool newValue );
	virtual void	SetGuarded( bool newValue );
	void			SetSpawnerList( bool newValue );

	void			SetNewbie( bool newValue );
	void			SetDecayable( bool newValue );
	void			SetDispellable( bool newValue );
	void			SetDivineLock( bool newValue );

	const char *	GetName2( void ) const;
	SERIAL			GetCreator( void ) const;
	std::string 	GetDesc( void ) const;

	void			SetName2( const char *newValue );
	void			SetCreator( SERIAL newValue );
	void			SetDesc( std::string newValue );

	void			PlaceInPack( void );
	virtual void	SetOldLocation( SI16 newX, SI16 newY, SI08 newZ ) override;
	virtual void	SetLocation( const CBaseObject *toSet ) override;
	virtual void	SetLocation( SI16 newX, SI16 newY, SI08 newZ ) override;
	virtual void	SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world, UI16 instanceID ) override;
	virtual void	SetLocation( SI16 newX, SI16 newY, SI08 newZ, SI08 newLoc, UI08 world, UI16 instanceID );
	void			IncZ( SI16 newValue );
	void			IncLocation( SI16 xInc, SI16 yInc );

	ItemLayers		GetLayer( void ) const;
	void			SetLayer( ItemLayers newValue );

	ItemTypes		GetType( void ) const;
	void			SetType( ItemTypes newValue );

	SI08			GetOffSpell( void ) const;
	void			SetOffSpell( SI08 newValue );

	UI32			GetTempVar( CITempVars whichVar ) const;
	void			SetTempVar( CITempVars whichVar, UI32 newVal );
	UI08			GetTempVar( CITempVars whichVar, UI08 part ) const;
	void			SetTempVar( CITempVars whichVar, UI08 part, UI08 newVal );

	UI16			GetAmount(  void ) const;
	void			SetAmount(  UI32 newValue );
	bool			IncAmount(  SI32 incValue, bool noDelete = false );

	virtual UI16	GetMaxHP( void ) const;
	virtual void	SetMaxHP( UI16 newValue );

	UI08			GetSpeed( void ) const;
	void			SetSpeed( UI08 newValue );

	UI08			GetMaxRange( void ) const;
	void			SetMaxRange( UI08 newValue );

	UI08			GetBaseRange( void ) const;
	void			SetBaseRange( UI08 newValue );

	SI08			GetMovable( void ) const;
	void			SetMovable( SI08 newValue );

	TIMERVAL		GetTempTimer(   void ) const;
	TIMERVAL		GetDecayTime(  void ) const;

	void			SetTempTimer(   TIMERVAL newValue );
	void			SetDecayTime(  TIMERVAL newValue );

	virtual UI08	GetPriv( void ) const;
	virtual void	SetPriv( UI08 newValue );

	UI32			GetBuyValue( void ) const;
	void			SetBuyValue( UI32 newValue );
	UI32			GetSellValue( void ) const;
	void			SetSellValue( UI32 newValue );

	UI16			GetRestock( void ) const;
	void			SetRestock( UI16 newValue );

	ARMORCLASS		GetArmourClass( void ) const;
	void			SetArmourClass( ARMORCLASS newValue );

	SI08			GetRank( void ) const;
	void			SetRank( SI08 newValue );

	SI16			GetGood( void ) const;
	void			SetGood( SI16 newValue );

	SI32			GetRndValueRate( void ) const;
	void			SetRndValueRate( SI32 newValue );

	SI08			GetMadeWith( void ) const;
	void			SetMadeWith( SI08 newValue );

	UI16			GetAmmoID( void ) const;
	void			SetAmmoID( UI16 newValue );

	UI16			GetAmmoHue( void ) const;
	void			SetAmmoHue( UI16 newValue );

	UI16			GetAmmoFX( void ) const;
	void			SetAmmoFX( UI16 newValue );

	UI16			GetAmmoFXHue( void ) const;
	void			SetAmmoFXHue( UI16 newValue );

	UI16			GetAmmoFXRender( void ) const;
	void			SetAmmoFXRender( UI16 newValue );

	SI32			GetWeightMax( void ) const;
	void			SetWeightMax( SI32 newValue );

	SI32			GetBaseWeight( void ) const;
	void			SetBaseWeight( SI32 newValue );

	UI16			GetMaxItems( void ) const;
	void			SetMaxItems( UI16 newValue );

	// Note: Value range to -ALLSKILLS-1 to ALLSKILLS+1
	// To calculate skill used to made this item:
	// if is a positive value, substract 1 it.
	//    Ex) madewith = 34, 34 - 1 = 33, 33 = STEALING
	// if is a negative value, add 1 from it and invert value.
	//    Ex) madewith = -34, -34 + 1 = -33, Abs(-33) = 33 = STEALING.
	// 0 = nullptr
	// So... a positive value is used when the item is made by a
	// player with 95.0+ at that skill. Infact in this way when
	// you click on the item appear its name and the name of the
	// creator. A negative value if the play is not skilled
	// enough!

	SERIAL			GetGlow( void ) const;
	void			SetGlow( SERIAL newValue );

	COLOUR			GetGlowColour( void ) const;
	void			SetGlowColour( COLOUR newValue );

	UI08			GetGlowEffect( void ) const;
	void			SetGlowEffect( UI08 newValue );

	CItem();
	virtual			~CItem();

	UI08			IsFieldSpell( void ) const;
	bool			IsLockedDown( void ) const;
	bool			IsShieldType( void ) const;
	bool			IsMetalType( void ) const;
	bool			IsLeatherType( void ) const;
	bool			CanBeLockedDown( void ) const;
	void			LockDown( void );
	bool			IsContType( void ) const;

	void			TextMessage( CSocket *s, SI32 dictEntry, R32 secsFromNow = 0.0f, UI16 Colour = 0x005A );
	virtual void	Update( CSocket *mSock = nullptr ) override;
	virtual void	SendToSocket( CSocket *mSock ) override;
	void			SendPackItemToSocket( CSocket *mSock );
	virtual void	RemoveFromSight( CSocket *mSock = nullptr );

	virtual bool	Save( std::ofstream &outStream ) override;
	virtual bool	DumpBody( std::ofstream &outStream ) const override;
	virtual bool	HandleLine( std::string &UTag, std::string &data ) override;
	virtual void	PostLoadProcessing( void ) override;
	virtual void	Cleanup( void ) override;
	virtual void	Delete( void ) override;
	virtual bool	CanBeObjType( ObjectType toCompare ) const override;

	UI32			GetSpell( UI08 part ) const;
	void			SetSpell( UI08 part, UI32 newValue );
};

class CSpawnItem : public CItem
{
protected:
	UI08				Interval[2];
	std::string			spawnSection;
	bool				isSectionAList;

	void				CopyData( CSpawnItem *target );
public:
	GenericList< CBaseObject * >		spawnedList;

	CSpawnItem();
	virtual				~CSpawnItem()
	{
	}

	UI08				GetInterval( UI08 part ) const;
	void				SetInterval( UI08 part, UI08 newVal );
	std::string			GetSpawnSection( void ) const;
	void				SetSpawnSection( const std::string &newVal );
	bool				IsSectionAList( void ) const;
	void				IsSectionAList( bool newVal );

	virtual bool		DumpHeader( std::ofstream &outStream ) const override;
	virtual bool		DumpBody( std::ofstream &outStream ) const override;

	virtual bool		HandleLine( std::string &UTag, std::string &data ) override;

	bool				DoRespawn( void );	// Will replace RespawnItem() eventually
	bool				HandleItemSpawner( void );
	bool				HandleNPCSpawner( void );
	bool				HandleSpawnContainer( void );

	virtual void		Cleanup( void ) override;

	virtual bool		CanBeObjType( ObjectType toCompare ) const override;

	CSpawnItem *		Dupe( void );
};

#endif

