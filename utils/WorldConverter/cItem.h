#ifndef __CITEM_H__
#define __CITEM_H__

typedef UI08 WEATHBITLIST;

class CItem : public cBaseObject
{
private:
	UI32			more;
	UI32			moreb;

	UI08			bools;

	char			name2[MAX_NAME];
	char			murderer[50]; //AntiChrist -- for corpse -- char's name who kille the char (forensic ev.)
	char			creator[50]; // Store the name of the player made this item -- Magius(CHE)
	char			desc[100];

	SI08			layer; // Layer if equipped on paperdoll
	SI08			itmhand; // ITEMHAND system - AntiChrist

	UI08			type; // For things that do special things on doubleclicking
	UI08			type2;

	SI08			offspell;

	SI32			weight;

	UI32			morex;
	UI32			morey;
	UI32			morez;

	UI32			amount; // Amount of items in pile
	UI32			amount2; //Used to track things like number of yards left in a roll of cloth

	UI08			doordir; // Reserved for doors

	SI16			maxhp; // Max number of hit points an item can have.

	UI08			spd; //The speed of the weapon

	SI08			magic; // 0=Default as stored in client, 1=Always movable, 2=Never movable, 3=Owner movable.

	UI32			gatetime;
	UI32			decaytime;
	UI32			murdertime; //AntiChrist -- for corpse -- when the people has been killed	// used to be long, don't need THAT long

	UI08			gatenumber;
	UI08			priv; // Bit 0, decay off/on.  Bit 1, newbie item off/on.  Bit 2 Dispellable
	SI32			value; // Price shopkeeper sells item at.
	SI32			restock; // Number up to which shopkeeper should restock this item
	UI32			poisoned; //AntiChrist -- for poisoning skill
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

	// For elemental weaponry.  So a bHeat weapon would be a fire weapon, and does elemental damage to bHeat weak races
	struct
	{
		WEATHBITLIST	bLight		: 1;
		WEATHBITLIST	bRain		: 1;
		WEATHBITLIST	bCold		: 1;
		WEATHBITLIST	bHeat		: 1;
		WEATHBITLIST	bLightning	: 1;
		WEATHBITLIST	bSnow		: 1;
	};

	void			RemoveSelfFromOwner( void );
	void			AddSelfToOwner( void );

public:
	SERIAL			contserial;

	virtual bool	WeatherDamage( WeatherType effectNum );
	virtual bool	LightDamage( void );
	virtual bool	RainDamage( void );
	virtual bool	ColdDamage( void );
	virtual bool	HeatDamage( void );
	virtual bool	LightningDamage( void );
	virtual bool	SnowDamage( void );

	virtual void	LightDamage( bool value );
	virtual void	RainDamage( bool value );
	virtual void	ColdDamage( bool value );
	virtual void	HeatDamage( bool value );
	virtual void	LightningDamage( bool value );
	virtual void	SnowDamage( bool value );

	virtual void	Dupe( CItem& target );

	virtual SI32	FirstItem( void ) const;
	virtual SI32	NextItem( void ) const;
	virtual bool	FinishedItems( void ) const;
	virtual CItem *	FirstItemObj( void ) const;
	virtual CItem *	NextItemObj( void ) const;

	virtual UI32	GetCont(   void ) const;
	virtual UI32	GetMore(   void ) const;
	virtual UI32	GetMoreB(  void ) const;
	virtual UI08	GetCont(   UI08 part ) const; 
	virtual UI08	GetMore(   UI08 part ) const;
	virtual UI08	GetMoreB(  UI08 part ) const;

	virtual void	SetSerial( SERIAL newValue );
	virtual SI08	SetCont(   SERIAL newValue );
	virtual SI08	SetContSerial( SERIAL newValue );	
	virtual SERIAL	GetContSerial( void );

	virtual void	SetMore(   UI32 newValue );
	virtual void	SetMoreB(  UI32 newValue );
	virtual void	SetOwner(  SERIAL newValue );
	virtual void	SetSpawn(  SERIAL newValue, ITEM index );
	virtual void	SetSerial( UI08 newValue, UI08 part );
	virtual void	SetMore(   UI08 newValue, UI08 part );
	virtual void	SetMoreB(  UI08 newValue, UI08 part );
	virtual void	SetOwner(  UI08 newValue, UI08 part );
	virtual void	SetSpawn(  UI08 newValue, UI08 part, ITEM index );
	virtual void	SetSerial( UI08 part1, UI08 part2, UI08 part3, UI08 part4 );
	virtual SI08	SetCont(   UI08 part1, UI08 part2, UI08 part3, UI08 part4 );
	virtual void	SetMore(   UI08 part1, UI08 part2, UI08 part3, UI08 part4 );
	virtual void	SetMoreB(  UI08 part1, UI08 part2, UI08 part3, UI08 part4 );
	virtual void	SetOwner(  UI08 part1, UI08 part2, UI08 part3, UI08 part4 );
	virtual void	SetSpawn(  UI08 part1, UI08 part2, UI08 part3, UI08 part4, ITEM index );

	virtual bool	isFree(      void ) const;
	virtual bool	isDoorOpen(  void ) const;
	virtual bool	isPileable(  void ) const;
	virtual bool	isDyeable(   void ) const;
	virtual bool	isCorpse(    void ) const;
	virtual bool	isWipeable(  void ) const;

	virtual void	SetFree(     bool newValue );
	virtual void	SetDoorOpen( bool newValue );
	virtual void	SetPileable( bool newValue );
	virtual void	SetDye(      bool newValue );
	virtual void	SetCorpse(   bool newValue );
	virtual void	SetWipeable( bool newValue );

	virtual const char *	GetName2(     void ) const;
	virtual const char *	GetMurderer(  void ) const;
	virtual const char *	GetCreator(   void ) const;
	virtual const char *	GetDesc(      void ) const;

	virtual void	SetName2(	 const char *newValue );
	virtual void	SetMurderer( const char *newValue );
	virtual void	SetCreator(  const char *newValue );
	virtual void	SetDesc(	 const char *newValue );

	virtual void	SetLocation( SI16 newX, SI16 newY, SI08 newZ );
	virtual void	IncX( SI16 newValue );
	virtual void	IncY( SI16 newValue );
	virtual void	IncZ( SI16 newValue );

	virtual SI08	GetLayer( void ) const;
	virtual void	SetLayer( SI08 newValue );

	virtual SI08	GetItmHand( void ) const;
	virtual void	SetItmHand( SI08 newValue );

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

	virtual SI32	GetAmount(  void ) const;
	virtual SI32	GetAmount2( void ) const;
	virtual void	SetAmount(  SI32 newValue );
	virtual void	SetAmount2( SI32 newValue );

	virtual UI08	GetDoorDir( void ) const;
	virtual void	SetDoorDir( UI08 neValue );

	virtual void	IncDef( UI16 newValue );
	virtual SI16	GetMaxHP(         void ) const;
	virtual void	SetMaxHP( SI16 newValue );

	virtual void	IncHP( SI16 newValue );
	virtual void	IncID( SI16 incAmount );

	virtual UI08	GetSpeed( void ) const;
	virtual void	SetSpeed( UI08 newValue );

	virtual SI08	GetMagic( void ) const;
	virtual void	SetMagic( SI08 newValue );
	
	virtual UI32	GetGateTime(   void ) const;
	virtual UI32	GetDecayTime(  void ) const;
	virtual UI32	GetMurderTime( void ) const;

	virtual void	SetGateTime(   UI32 newValue );
	virtual void	SetDecayTime(  UI32 newValue );
	virtual void	SetMurderTime( UI32 newValue );

	virtual char	GetGateNumber( void ) const;
	virtual void	SetGateNumber( char newValue );

	virtual UI08	GetPriv( void ) const;
	virtual void	SetPriv( UI08 newValue );

	virtual SI32	GetValue( void ) const;
	virtual void	SetValue( SI32 newValue );

	virtual SI32	GetRestock( void ) const;
	virtual void	SetRestock( SI32 newValue );

	virtual UI32	GetPoisoned( void ) const;
	virtual void	SetPoisoned( UI32 newValue );

	virtual ARMORCLASS		GetArmourClass( void ) const;
	virtual void			SetArmourClass( ARMORCLASS newValue );

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

	CItem();
	virtual ~CItem();

	virtual bool	HoldItem( CItem *toHold );
	virtual bool	ReleaseItem( int index );
	virtual bool	ReleaseItem( CItem *index );
	virtual int		NumItems( void ) const;
	virtual int		GetItem( int index ) const;

	ULONG			enhanced;
	virtual bool	WillDecay( void ) const;

	virtual UI08	IsFieldSpell( void ) const;
	virtual bool	IsLockedDown( void ) const;
	virtual bool	IsShieldType( void ) const;
	virtual bool	CanBeLockedDown( void ) const;
	virtual void	LockDown( void );
	virtual bool	Save( ofstream &outStream, int mode );
	virtual bool	DumpHeader( ofstream &outStream, int mode ) const;
	virtual bool	DumpBody( ofstream &outStream, int mode ) const;
	virtual bool	DumpFooter( ofstream &outStream, int mode ) const;
	virtual bool	Load( ifstream &inStream, int mode, int arrayOffset );
	virtual bool	HandleLine( char *tag, char *data );
	virtual bool	IsContType( void ) const;
	virtual void	PostLoadProcessing( SI32 index );
};

#endif