#ifndef __CITEM_H__
#define __CITEM_H__

namespace UOX
{
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
	CDataList< CItem * >	Contains;

	CBaseObject	*	contObj;

	UI08			bools;
	UI08			priv; // Bit 0, decay off/on.  Bit 1, newbie item off/on.  Bit 2 Dispellable

	char			name2[MAX_NAME];
	SERIAL			creator;	// Store the serial of the player made this item
	std::string		desc;

	ItemLayers		layer; // Layer if equipped on paperdoll
	ItemTypes		type; // For things that do special things on doubleclicking

	SI08			offspell;

	UI32			tempVars[CITV_COUNT];

	UI16			amount; // Amount of items in pile

	UI16			maxhp; // Max number of hit points an item can have.

	UI08			spd; //The speed of the weapon

	SI08			movable; // 0=Default as stored in client, 1=Always movable, 2=Never movable, 3=Owner movable.

	TIMERVAL		tempTimer;
	TIMERVAL		decaytime;

	UI32			value[2];	// Price a shopkeep buys and sells items for
	UI16			restock; // Number up to which shopkeeper should restock this item
	ARMORCLASS		armorClass;
	SI08			rank;	// Magius(CHE) --- for rank system, this value is the LEVEL of the item from 1 to 10.  Simply multiply t he rank*10 and calculate the MALUS this item has from the original.
									// for example: RANK 5 --> 5*10 = 50% of malus
									// this item has same values decreased by 50%..
									// RANK 1 --> 1*10=10% this item has 90% of malus!
									// RANK 10 --> 10*10 = 100% this item has no malus! RANK 10 is automatically setted if you select RANKSYSTEM 0.
									// Vars: LODAMAGE, HIDAMAGE, ATT, DEF, HP, MAXHP
	SI16			good; // Store type of GOODs to trade system! (Plz not set as UNSIGNED) --- Magius(CHE)
	SI32			rndvaluerate; // Store the value calculated base on RANDOMVALUE in region.scp. ---- MAgius(CHE) (2)
	SI08			madewith; // Store the skills used to make this item -- Magius(CHE)
	SERIAL			glow;	// LB identifies glowing objects
	COLOUR			glowColour;
	UI08			glow_effect;

	UI08			weatherBools;	// For elemental weaponry.  So a Heat weapon would be a fire weapon, and does elemental damage to Heat weak races

	void			RemoveSelfFromCont( void );
	virtual void	RemoveSelfFromOwner( void );
	virtual void	AddSelfToOwner( void );

	void			CheckItemIntegrity( void );
	virtual bool	DumpHeader( std::ofstream &outStream ) const;
	virtual bool	LoadRemnants( void );

	UI16			entryMadeFrom;
	UI32			spells[3];		// For spellbooks (eventually should be a derived class)

	void			CopyData( CItem *target );

public:

	CDataList< CItem * > *	GetContainsList( void );

	UI16			EntryMadeFrom( void ) const;
	void			EntryMadeFrom( UI16 newValue );

	bool			GetWeatherDamage( WeatherType effectNum ) const;
	void			SetWeatherDamage( WeatherType effectNum, bool value );

	CItem *			Dupe( ObjectType itemType = OT_ITEM );

	CBaseObject *	GetCont(   void ) const;
	SERIAL			GetContSerial( void ) const;

	bool			SetCont(   CBaseObject *newCont );
	bool			SetContSerial( SERIAL newSerial );

	bool			isDoorOpen( void ) const;
	bool			isPileable( void ) const;
	bool			isDyeable( void ) const;
	bool			isCorpse( void ) const;
	bool			isWipeable( void ) const;
	bool			isGuarded( void ) const;
	bool			isSpawnerList( void ) const;

	bool			isNewbie( void ) const;
	bool			isDecayable( void ) const;
	bool			isDispellable( void ) const;
	bool			isDevineLocked( void ) const;

	void			SetDoorOpen( bool newValue );
	void			SetPileable( bool newValue );
	void			SetDye( bool newValue );
	void			SetCorpse( bool newValue );
	void			SetWipeable( bool newValue );
	virtual void	SetGuarded( bool newValue );
	void			SetSpawnerList( bool newValue );

	void			SetNewbie( bool newValue );
	void			SetDecayable( bool newValue );
	void			SetDispellable( bool newValue );
	void			SetDevineLock( bool newValue );

	const char *	GetName2( void ) const;
	SERIAL			GetCreator( void ) const;
	std::string 	GetDesc( void ) const;

	void			SetName2( const char *newValue );
	void			SetCreator( SERIAL newValue );
	void			SetDesc( std::string newValue );

	void			PlaceInPack( void );
	virtual void	SetLocation( const CBaseObject *toSet );
	virtual void	SetLocation( SI16 newX, SI16 newY, SI08 newZ );
	virtual void	SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world );
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
	bool			IncAmount(  SI16 incValue );

	virtual UI16	GetMaxHP( void ) const;
	virtual void	SetMaxHP( UI16 newValue );

	void			IncID( SI16 incAmount );

	UI08			GetSpeed( void ) const;
	void			SetSpeed( UI08 newValue );

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

	// Note by Magius: Value range to -ALLSKILLS-1 to ALLSKILLS+1
	// To calculate skill used to made this item:
	// if is a positive value, substract 1 it.
	//    Ex) madewith = 34, 34 - 1 = 33, 33 = STEALING
	// if is a negative value, add 1 from it and invert value.
	//    Ex) madewith = -34, -34 + 1 = -33, Abs(-33) = 33 = STEALING.
	// 0 = NULL
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

	void			itemTalk( CSocket *s, SI32 dictEntry, R32 secsFromNow = 0.0f, UI16 Colour = 0 );
	virtual void	Update( CSocket *mSock = NULL );
	virtual void	SendToSocket( CSocket *mSock );
	void			SendPackItemToSocket( CSocket *mSock );
	virtual void	RemoveFromSight( CSocket *mSock = NULL );

	virtual bool	Save( std::ofstream &outStream );
	virtual bool	DumpBody( std::ofstream &outStream ) const;
	virtual bool	HandleLine( UString &UTag, UString &data );
	virtual void	PostLoadProcessing( void );
	virtual void	Cleanup( void );
	virtual void	Delete( void );
	virtual bool	CanBeObjType( ObjectType toCompare ) const;

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
	CDataList< CBaseObject * >		spawnedList;

						CSpawnItem();
	virtual				~CSpawnItem()
						{
						}

	UI08				GetInterval( UI08 part ) const;
	void				SetInterval( UI08 part, UI08 newVal );
	std::string			GetSpawnSection( void ) const;
	void				SetSpawnSection( std::string newVal );
	bool				IsSectionAList( void ) const;
	void				IsSectionAList( bool newVal );

	virtual bool		DumpHeader( std::ofstream &outStream ) const;
	virtual bool		DumpBody( std::ofstream &outStream ) const;

	virtual bool		HandleLine( UString &UTag, UString &data );

	bool				DoRespawn( void );	// Will replace RespawnItem() eventually
	bool				HandleItemSpawner( void );
	bool				HandleNPCSpawner( void );
	bool				HandleSpawnContainer( void );

	virtual void		Cleanup( void );

	virtual bool		CanBeObjType( ObjectType toCompare ) const;

	CSpawnItem *		Dupe( void );
};

}

#endif

