#ifndef __CITEM_H__
#define __CITEM_H__

class CItem : public cBaseObject
{
private:
	cBaseObject	*	contObj;
	UI32			more;
	UI32			moreb;

	UI08			bools;

	char			name2[MAX_NAME];
	SERIAL			murderer; // char's serial who killed the char (forensic ev.)
	SERIAL			creator;	// Store the serial of the player made this item
	char			desc[100];

	UI08			layer; // Layer if equipped on paperdoll

	UI08			type; // For things that do special things on doubleclicking
	UI08			type2;

	SI08			offspell;

	SI32			weight;

	UI32			morex;
	UI32			morey;
	UI32			morez;

	UI16			amount; // Amount of items in pile

	UI08			doordir; // Reserved for doors

	SI16			maxhp; // Max number of hit points an item can have.

	UI08			spd; //The speed of the weapon

	SI08			movable; // 0=Default as stored in client, 1=Always movable, 2=Never movable, 3=Owner movable.

	UI32			gatetime;
	UI32			decaytime;
	UI32			murdertime; //AntiChrist -- for corpse -- when the people has been killed	// used to be long, don't need THAT long

	UI08			priv; // Bit 0, decay off/on.  Bit 1, newbie item off/on.  Bit 2 Dispellable
	SI32			value; // Price shopkeeper sells item at.
	UI16			restock; // Number up to which shopkeeper should restock this item
	SI08			poisoned; //AntiChrist -- for poisoning skill
	ARMORCLASS		armorClass;
	SI08			rank;	// Magius(CHE) --- for rank system, this value is the LEVEL of the item from 1 to 10.  Simply multiply t he rank*10 and calculate the MALUS this item has from the original.
									// for example: RANK 5 --> 5*10 = 50% of malus
									// this item has same values decreased by 50%..
									// RANK 1 --> 1*10=10% this item has 90% of malus!
									// RANK 10 --> 10*10 = 100% this item has no malus! RANK 10 is automatically setted if you select RANKSYSTEM 0.
									// Vars: LODAMAGE, HIDAMAGE, ATT, DEF, HP, MAXHP
	SI32			good; // Store type of GOODs to trade system! (Plz not set as UNSIGNED) --- Magius(CHE)
	SI32			rndvaluerate; // Store the value calculated base on RANDOMVALUE in region.scp. ---- MAgius(CHE) (2)
	SI08			madewith; // Store the skills used to make this item -- Magius(CHE)
	SI32			glow;	// LB identifies glowing objects
	UI16			glowColour;
	UI08			glow_effect;
	SI32			carve; // AntiChrist - for new carve system

	ITEMLIST		Contains;
	bool			remove;

	UI08			weatherBools;	// For elemental weaponry.  So a Heat weapon would be a fire weapon, and does elemental damage to Heat weak races

	void			RemoveSelfFromCont( void );
	void			RemoveSelfFromOwner( void );
	void			AddSelfToOwner( void );

	SI32			entryMadeFrom;

public:

	virtual SI32	EntryMadeFrom( void ) const;
	virtual void	EntryMadeFrom( SI32 newValue );

	virtual bool	WeatherDamage( WeatherType effectNum ) const;
	virtual bool	LightDamage( void ) const;
	virtual bool	RainDamage( void ) const;
	virtual bool	ColdDamage( void ) const;
	virtual bool	HeatDamage( void ) const;
	virtual bool	LightningDamage( void ) const;
	virtual bool	SnowDamage( void ) const;

	virtual void	LightDamage( bool value );
	virtual void	RainDamage( bool value );
	virtual void	ColdDamage( bool value );
	virtual void	HeatDamage( bool value );
	virtual void	LightningDamage( bool value );
	virtual void	SnowDamage( bool value );

	virtual CItem *	Dupe( void );

	virtual ITEM	FirstItem( void ) const;
	virtual ITEM	NextItem( void ) const;
	virtual bool	FinishedItems( void ) const;
	virtual CItem *	FirstItemObj( void ) const;
	virtual CItem *	NextItemObj( void ) const;

	virtual cBaseObject *	GetCont(   void ) const;
	virtual SERIAL			GetContSerial( void ) const;
	virtual UI32	GetMore(   void ) const;
	virtual UI32	GetMoreB(  void ) const;
	virtual UI08	GetCont(   UI08 part ) const; 
	virtual UI08	GetMore(   UI08 part ) const;
	virtual UI08	GetMoreB(  UI08 part ) const;

	virtual void	SetSerial( SERIAL newValue, ITEM n );
	virtual bool	SetCont(   cBaseObject *newCont );
	virtual bool	SetContSerial( SERIAL newSerial );
	virtual void	SetMore(   UI32 newValue );
	virtual void	SetMoreB(  UI32 newValue );
	virtual void	SetOwner(  cBaseObject *newValue );
	virtual void	SetSpawn(  SERIAL newValue, ITEM index );
	virtual void	SetMore(   UI08 newValue, UI08 part );
	virtual void	SetMoreB(  UI08 newValue, UI08 part );
	virtual void	SetMore(   UI08 part1, UI08 part2, UI08 part3, UI08 part4 );
	virtual void	SetMoreB(  UI08 part1, UI08 part2, UI08 part3, UI08 part4 );
	virtual void	SetGuarded( bool newValue );

	virtual bool	isFree( void ) const;
	virtual bool	isDoorOpen( void ) const;
	virtual bool	isPileable( void ) const;
	virtual bool	isDyeable( void ) const;
	virtual bool	isCorpse( void ) const;
	virtual bool	isWipeable( void ) const;
	virtual bool	isGuarded( void ) const;
	virtual bool	isSpawnerList( void ) const;

	virtual bool	isNewbie( void ) const;
	virtual bool	isDecayable( void ) const;
	virtual bool	isDispellable( void ) const;

	virtual void	SetNewbie( bool newValue );
	virtual void	SetDecayable( bool newValue );
	virtual void	SetDispellable( bool newValue );

	virtual void	SetFree( bool newValue );
	virtual void	SetDoorOpen( bool newValue );
	virtual void	SetPileable( bool newValue );
	virtual void	SetDye( bool newValue );
	virtual void	SetCorpse( bool newValue );
	virtual void	SetWipeable( bool newValue );
	virtual void	SetSpawnerList( bool newValue );

	virtual const char *	GetName2( void ) const;
	virtual SERIAL	GetMurderer( void ) const;
	virtual SERIAL	GetCreator( void ) const;
	virtual const char *	GetDesc( void ) const;

	virtual void	SetName2( const char *newValue );
	virtual void	SetMurderer( SERIAL newValue );
	virtual void	SetCreator( SERIAL newValue );
	virtual void	SetDesc( const char *newValue );

	virtual void	SetLocation( const cBaseObject *toSet );
	virtual void	SetLocation( SI16 newX, SI16 newY, SI08 newZ );
	virtual void	SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world );
	virtual void	IncX( SI16 newValue );
	virtual void	IncY( SI16 newValue );
	virtual void	IncZ( SI16 newValue );

	virtual UI08	GetLayer( void ) const;
	virtual void	SetLayer( UI08 newValue );

	virtual UI08	GetType(  void ) const;
	virtual UI08	GetType2( void ) const;

	virtual void	SetType(  UI08 newValue );
	virtual void	SetType2( UI08 newValue );

	virtual SI08	GetOffSpell( void ) const;
	virtual void	SetOffSpell( SI08 newValue );

	virtual SI32	GetWeight( void ) const;
	virtual void	SetWeight( SI32 newValue );

	virtual UI32	GetMoreX( void ) const;
	virtual UI32	GetMoreY( void ) const;
	virtual UI32	GetMoreZ( void ) const;

	virtual void	SetMoreX( UI32 newValue );
	virtual void	SetMoreY( UI32 newValue );
	virtual void	SetMoreZ( UI32 newValue );

	virtual UI16	GetAmount(  void ) const;
	virtual void	SetAmount(  UI32 newValue );

	virtual UI08	GetDoorDir( void ) const;
	virtual void	SetDoorDir( UI08 neValue );

	virtual void	IncDef( UI16 newValue );
	virtual SI16	GetMaxHP(         void ) const;
	virtual void	SetMaxHP( SI16 newValue );

	virtual void	IncHP( SI16 newValue );
	virtual void	IncID( SI16 incAmount );

	virtual UI08	GetSpeed( void ) const;
	virtual void	SetSpeed( UI08 newValue );

	virtual SI08	GetMovable( void ) const;
	virtual void	SetMovable( SI08 newValue );
	
	virtual UI32	GetGateTime(   void ) const;
	virtual UI32	GetDecayTime(  void ) const;
	virtual UI32	GetMurderTime( void ) const;

	virtual void	SetGateTime(   UI32 newValue );
	virtual void	SetDecayTime(  UI32 newValue );
	virtual void	SetMurderTime( UI32 newValue );

	virtual UI08	GetPriv( void ) const;
	virtual void	SetPriv( UI08 newValue );

	virtual SI32	GetValue( void ) const;
	virtual void	SetValue( SI32 newValue );

	virtual UI16	GetRestock( void ) const;
	virtual void	SetRestock( UI16 newValue );

	virtual UI08	GetPoisoned( void ) const;
	virtual void	SetPoisoned( UI08 newValue );

	virtual ARMORCLASS	GetArmourClass( void ) const;
	virtual void		SetArmourClass( ARMORCLASS newValue );

	virtual SI08	GetRank( void ) const;
	virtual void	SetRank( SI08 newValue );

	virtual SI32	GetGood( void ) const;
	virtual void	SetGood( SI32 newValue );

	virtual SI32	GetRndValueRate( void ) const;
	virtual void	SetRndValueRate( SI32 newValue );

	virtual SI08	GetMadeWith( void ) const;
	virtual void	SetMadeWith( SI08 newValue );

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

	virtual SI32	GetGlow( void ) const;
	virtual void	SetGlow( SI32 newValue );

	virtual UI16	GetGlowColour( void ) const;
	virtual UI08	GetGlowColour( UI08 part ) const;

	virtual void	SetGlowColour( UI16 newValue );
	virtual void	SetGlowColour( UI08 newValue, UI08 part );
	virtual void	SetGlowColourEx( UI08 part1, UI08 part2 );

	virtual UI08	GetGlowEffect( void ) const;
	virtual void	SetGlowEffect( UI08 newValue );
	
	virtual SI32	GetCarve( void ) const;
	virtual void	SetCarve( SI32 newValue );

					CItem( ITEM nItem, bool zeroSer );
	virtual			~CItem();

	virtual bool	HoldItem( CItem *toHold );
	virtual bool	ReleaseItem( CItem *index );
	virtual UI32	NumItems( void ) const;
	virtual CItem * GetItemObj( ITEM index ) const;

	UI32			enhanced;
	virtual bool	WillDecay( void ) const;

	virtual UI08	IsFieldSpell( void ) const;
	virtual bool	IsLockedDown( void ) const;
	virtual bool	IsShieldType( void ) const;
	virtual bool	IsMetalType( void ) const;
	virtual bool	IsLeatherType( void ) const;
	virtual bool	IsBowType( void ) const;
	virtual bool	CanBeLockedDown( void ) const;
	virtual void	LockDown( void );
	virtual bool	Save( std::ofstream &outStream, SI32 mode );
	virtual bool	DumpHeader( std::ofstream &outStream, SI32 mode ) const;
	virtual bool	DumpBody( std::ofstream &outStream, SI32 mode ) const;
	virtual bool	DumpBody( BinBuffer &buff ) const;
	virtual bool	DumpFooter( std::ofstream &outStream, SI32 mode ) const;
	virtual bool	Load( std::ifstream &inStream, ITEM arrayOffset );
	virtual bool	Load( BinBuffer &buff, ITEM arrayOffset );
	virtual bool	HandleLine( char *tag, char *data );
	virtual bool	HandleBinTag( UI08 tag, BinBuffer &buff );
	virtual bool	LoadRemnants( int arrayOffset );
	virtual bool	IsContType( void ) const;
	virtual void	PostLoadProcessing( UI32 index );
	virtual void	Sort( void );
};

#endif

