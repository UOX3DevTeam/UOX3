//o--------------------------------------------------------------------------o
//|	File					-	cBaseObject.h
//|	Date					-	
//|	Developers		-	
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
#ifndef __CBO_H__
#define __CBO_H__

// Some helpers for iteators, and readability
typedef std::map< std::string, jsval> TAGMAP;
typedef std::map< std::string, jsval>::iterator TAGMAP_ITERATOR;
typedef std::map< std::string, jsval>::const_iterator TAGMAP_CITERATOR;

//o--------------------------------------------------------------------------o
//|	Class/Struct	-	class cBaseObject
//|	Date					-	
//|	Developers		-	Abaddon/EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
class cBaseObject
{
protected:
	TAGMAP		tags;
	char			title[MAX_TITLE];
	SI16			kills;
	SI16			fame;
	SI16			karma;
	SI16			mana;
	SI16			stamina;
	ObjectType		objType;
	mutable SI32	FilePosition;
	SI16			x;
	SI16			y;
	SI08			z;
	UI16			id;
	UI16			colour;
	UI08			dir;
	SERIAL			serial;
	cBaseObject *	multis;
	cBaseObject *	spawnserial;
	cBaseObject *	owner;
	mutable short	iCounter;	// item counter
	RACEID			race;
	char			name[MAX_NAME];
	SI16			strength;
	SI16			dexterity;
	SI16			intelligence;
	SI16			hitpoints;
	SI16			st2;
	SI16			dx2;
	SI16			in2;
	SI08			visible;
	bool			disabled;
	SI16			hidamage;
	SI16			lodamage;
	UI16			def;
	UI16			scriptTrig;

	UI32			genericDWords[4];
	UI08			worldNumber;
	bool			shouldSave;
	UI08			isDirty;

	void			RemoveFromMulti( bool fireTrigger = true );
	void			AddToMulti( bool fireTrigger = true );

	bool			spawned;

public:

	bool					ShouldSave( void ) const;
	void					ShouldSave( bool value );
	SI32 					GetNumTags( void ) const;
	jsval 					GetTag( char* tagname ) const;
	void 					SetTag( char* tagname, jsval tagval );
	
	void					SetTitle( const char *newtitle );
	const char *			GetTitle( void ) const;
	void					SetKills( SI16 value );
	SI16					GetKills( void ) const;
	SI16					GetFame( void ) const;
	void					SetFame( SI16 value );
	void					SetKarma( SI16 value );
	SI16					GetKarma( void ) const;
	void					SetMana( SI16 mn );
	SI16					GetMana( void ) const;
	void					SetStamina( SI16 stam );
	SI16					GetStamina( void ) const;
	SI32					GetFilePosition( void ) const;
	SI32					SetFilePosition( SI32 filepos );

	virtual					~cBaseObject();
							cBaseObject( void );

	virtual SI16			GetX( void ) const;
	virtual SI16			GetY( void ) const;
	virtual SI08			GetZ( void ) const;
	virtual point3			GetLocation( void ) const;

	virtual void			SetX( SI16 newValue );
	virtual void			SetY( SI16 newValue );
	virtual void			SetZ( SI08 newValue );
	virtual void			SetLocation( SI16 newX, SI16 newY, SI08 newZ );
	virtual void			SetLocation( point3 &toSet );

	virtual UI16			GetID(     void ) const;
	virtual UI16			GetColour( void ) const;
	virtual UI08			GetID(     UI08 part ) const;
	virtual UI08			GetColour( UI08 part ) const;

	virtual void			SetID(     UI16 newValue );
	virtual void			SetColour( UI16 newValue );
	virtual void			SetID(     UI08 newValue, UI08 part );
	virtual void			SetColour( UI08 newValue, UI08 part );

	virtual SERIAL			GetMulti(  void ) const;
	virtual SERIAL			GetSerial( void ) const;
	virtual SERIAL			GetSpawn( void ) const;
	virtual SERIAL			GetOwner( void ) const;
	virtual cBaseObject *	GetMultiObj( void ) const;
	virtual cBaseObject *	GetSpawnObj( void ) const;
	virtual cBaseObject *	GetOwnerObj( void ) const;

	virtual UI08			GetSerial( UI08 part ) const;
	virtual UI08			GetMulti(  UI08 part ) const;
	virtual UI08			GetSpawn(  UI08 part ) const;
	virtual UI08			GetOwner(  UI08 part ) const;

	virtual void			SetMulti(  SERIAL newSerial, bool fireTrigger = true );
	virtual void			SetSerial( SERIAL newSerial );
	virtual void			SetSpawn(  SERIAL newSerial );
	virtual void			SetOwner(  SERIAL newSerial );
	virtual void			SetMulti(  UI08 newSerial, UI08 newPart, bool fireTrigger = true );
	virtual void			SetSpawn(  UI08 newSerial, UI08 newPart );

	virtual void			SetMulti(  cBaseObject *newMulti, bool fireTrigger = true );
	virtual void			SetSpawn(  cBaseObject *newSpawn );
	virtual void			SetOwner(  cBaseObject *newOwner );

	virtual bool			Save( std::ofstream &outStream, int mode ) const;
	virtual bool			DumpHeader( std::ofstream &outStream, int mode ) const;

	virtual	bool			DumpBody( BinBuffer &buff ) const;
	virtual bool			DumpBody( std::ofstream &outStream, int mode ) const;
	
	virtual bool			DumpFooter( std::ofstream &outStream, int mode ) const;
	virtual bool			Load( std::ifstream &inStream, int arrayOffset );
	virtual bool			HandleLine( char *tag, char *data );
	virtual bool			Load( BinBuffer &buff, int arrayOffset );
	virtual bool			HandleBinTag( UI08 tag, BinBuffer &buff );

	//	Char uses fixed array, Item uses variable based vector
	virtual cBaseObject		*First( void );
	virtual cBaseObject		*Next( void );
	virtual bool			AtEnd( void );

	virtual RACEID			GetRace( void ) const;
	virtual void			SetRace( RACEID newValue );

	virtual const char *	GetName( void ) const;
	virtual void			SetName( const char *newName );

	virtual SI16			GetStrength( void ) const;
	virtual SI16			GetDexterity( void ) const;
	virtual SI16			GetIntelligence( void ) const;
	virtual SI16			GetHP( void ) const;

	virtual void			SetStrength( SI16 newValue );
	virtual void			SetDexterity( SI16 newValue );
	virtual void			SetIntelligence( SI16 newValue );
	virtual void			SetHP( SI16 newValue );
	virtual void			IncHP( SI16 amtToChange );

	virtual void			SetDir( UI08 newDir );
	virtual UI08			GetDir( void ) const;

	virtual void			SetVisible( SI08 newValue );
	virtual SI08			GetVisible( void ) const;
	virtual bool			IsVisible( void ) const;

	virtual ObjectType		GetObjType( void ) const;

	virtual bool			IsDisabled( void ) const;
	virtual void			SetDisabled( bool newValue );

	virtual SI16			GetHiDamage( void ) const;
	virtual SI16			GetLoDamage( void ) const;
	virtual UI16			GetDef( void ) const;

	virtual void			SetHiDamage( SI16 newValue );
	virtual void			SetLoDamage( SI16 newValue );
	virtual void			SetDef( UI16 newValue );

	virtual UI16			GetScriptTrigger( void ) const;
	virtual void			SetScriptTrigger( UI16 newValue );

	virtual SI16			Strength2( void ) const;
	virtual SI16			Dexterity2( void ) const;
	virtual SI16			Intelligence2( void ) const;

	virtual void			Strength2( SI16 nVal );
	virtual void			Dexterity2( SI16 nVal );
	virtual void			Intelligence2( SI16 nVal );

	virtual void			IncStrength( SI16 toInc = 1 );
	virtual void			DecStrength( void );

	virtual void			IncDexterity( SI16 toInc = 1 );
	virtual void			DecDexterity( void );

	virtual void			IncIntelligence( SI16 toInc = 1 );
	virtual void			DecIntelligence( void );

	virtual UI32			GetWord( UI08 wordNum ) const;
	virtual bool			GetBit( UI08 wordNum, UI08 bitNum ) const;
	virtual UI32			GetBitRange( UI08 wordNum, UI08 lowBit, UI08 highBit ) const;

	virtual void			SetWord( UI08 wordNum, UI32 value );
	virtual void			SetBit( UI08 wordNum, UI08 bitNum, bool value );
	virtual void			SetBitRange( UI08 wordNum, UI08 lowBit, UI08 highBit, UI32 value );

	virtual void			PostLoadProcessing( UI32 index );

	virtual UI08			WorldNumber( void ) const;
	virtual void			WorldNumber( UI08 value );

	virtual void			IsSpawned( bool setting );
	virtual bool			IsSpawned( void );

private:
};

#endif

