//o--------------------------------------------------------------------------o
//|	File			-	cBaseObject.h
//|	Date			-	
//|	Developers		-	
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
#ifndef __CBO_H__
#define __CBO_H__

namespace UOX
{

// Some helpers for iteators, and readability
typedef std::map< std::string, jsval> TAGMAP;
typedef std::map< std::string, jsval>::iterator TAGMAP_ITERATOR;
typedef std::map< std::string, jsval>::const_iterator TAGMAP_CITERATOR;

enum UpdateTypes
{
	UT_UPDATE = 0,
	UT_LOCATION,
	UT_HITPOINTS,
	UT_STAMINA,
	UT_MANA,
	UT_HIDE
};

//o--------------------------------------------------------------------------o
//|	Class/Struct	-	class CBaseObject
//|	Date					-	
//|	Developers		-	Abaddon/EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
class CBaseObject
{
protected:
	TAGMAP			tags;
	std::string 	title;
	SI16			kills;
	SI16			fame;
	SI16			karma;
	SI16			mana;
	SI16			stamina;
	SI32			weight;
	ObjectType		objType;
	mutable SI32	FilePosition;
	SI16			x;
	SI16			y;
	SI08			z;
	UI16			id;
	UI16			colour;
	UI08			dir;
	SERIAL			serial;
	CMultiObj *	multis;
	SERIAL			spawnserial;
	CChar *			owner;
	RACEID			race;
	UString		 	name;
	SI16			strength;
	SI16			dexterity;
	SI16			intelligence;
	SI16			hitpoints;
	SI16			st2;
	SI16			dx2;
	SI16			in2;
	VisibleTypes	visible;
	SI16			hidamage;
	SI16			lodamage;
	UI16			def;
	UI16			scriptTrig;

	SI16			carve; // Carve.dfn entry

	UI32			genericDWords[4];
	UI08			worldNumber;
	UI08			isDirty;

	void			RemoveFromMulti( bool fireTrigger = true );
	void			AddToMulti( bool fireTrigger = true );

	UI08			poisoned;
	UI08			updateTypes;
	UI08			objSettings;

	void			CopyData( CBaseObject *target );

public:

	size_t 					GetNumTags( void ) const;
	jsval 					GetTag( std::string tagname ) const;
	void 					SetTag( std::string tagname, jsval tagval );
	
	void					SetTitle( std::string newtitle );
	std::string				GetTitle( void ) const;
	void					SetKills( SI16 value );
	SI16					GetKills( void ) const;
	SI16					GetFame( void ) const;
	void					SetFame( SI16 value );
	void					SetKarma( SI16 value );
	SI16					GetKarma( void ) const;
	virtual void			SetMana( SI16 mn );
	SI16					GetMana( void ) const;
	virtual void			SetStamina( SI16 stam );
	SI16					GetStamina( void ) const;
	SI32					GetFilePosition( void ) const;
	SI32					SetFilePosition( SI32 filepos );

	virtual					~CBaseObject();
							CBaseObject( void );

	SI16					GetX( void ) const;
	SI16					GetY( void ) const;
	SI08					GetZ( void ) const;
	point3					GetLocation( void ) const;

	void					SetX( SI16 newValue );
	void					SetY( SI16 newValue );
	void					SetZ( SI08 newValue );
	virtual void			SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world ) = 0;
	virtual void			SetLocation( SI16 newX, SI16 newY, SI08 newZ ) = 0;
	virtual void			SetLocation( const CBaseObject *toSet ) = 0;

	UI16					GetID(     void ) const;
	UI16					GetColour( void ) const;
	UI08					GetID(     UI08 part ) const;
	UI08					GetColour( UI08 part ) const;

	void					SetID(     UI16 newValue );
	void					SetColour( UI16 newValue );
	void					SetID(     UI08 newValue, UI08 part );

	SI32					GetWeight( void ) const;
	void					SetWeight( SI32 newVal );

	SERIAL					GetSerial( void ) const;
	SERIAL					GetSpawn( void ) const;
	SERIAL					GetOwner( void ) const;
	SERIAL					GetMulti( void ) const;
	CMultiObj *				GetMultiObj( void ) const;
	CSpawnItem *			GetSpawnObj( void ) const;
	CChar *					GetOwnerObj( void ) const;

	UI08					GetSerial( UI08 part ) const;
	UI08					GetSpawn(  UI08 part ) const;

	void					SetMulti(  SERIAL newSerial, bool fireTrigger = true );
	void					SetMulti(  CMultiObj *newMulti, bool fireTrigger = true );
	void					SetSerial( SERIAL newSerial );
	void					SetSpawn(  SERIAL newSpawn );
	virtual void			SetOwner(  CChar *newOwner );

	virtual bool			Save( std::ofstream &outStream ) = 0;
	virtual bool			DumpHeader( std::ofstream &outStream ) const = 0;
	virtual bool			DumpBody( std::ofstream &outStream ) const;
	bool					DumpFooter( std::ofstream &outStream ) const;
	bool					Load( std::ifstream &inStream );
	virtual bool			HandleLine( UString &UTag, UString &data );

	RACEID					GetRace( void ) const;
	void					SetRace( RACEID newValue );

	std::string				GetName( void ) const;
	void					SetName( std::string newName );

	virtual SI16			GetStrength( void ) const;
	virtual SI16			GetDexterity( void ) const;
	virtual SI16			GetIntelligence( void ) const;
	virtual SI16			GetHP( void ) const;

	virtual void			SetStrength( SI16 newValue );
	virtual void			SetDexterity( SI16 newValue );
	virtual void			SetIntelligence( SI16 newValue );
	virtual void			SetHP( SI16 newValue );
	void					IncHP( SI16 amtToChange );

	void					SetDir( UI08 newDir );
	UI08					GetDir( void ) const;

	void					SetVisible( VisibleTypes newValue );
	VisibleTypes			GetVisible( void ) const;

	ObjectType				GetObjType( void ) const;
	virtual bool			CanBeObjType( ObjectType toCompare ) const;

	SI16					GetHiDamage( void ) const;
	SI16					GetLoDamage( void ) const;
	UI16					GetDef( void ) const;

	void					SetHiDamage( SI16 newValue );
	void					SetLoDamage( SI16 newValue );
	void					SetDef( UI16 newValue );

	UI16					GetScriptTrigger( void ) const;
	void					SetScriptTrigger( UI16 newValue );

	SI16					GetStrength2( void ) const;
	SI16					GetDexterity2( void ) const;
	SI16					GetIntelligence2( void ) const;

	virtual void			SetStrength2( SI16 nVal );
	virtual void			SetDexterity2( SI16 nVal );
	virtual void			SetIntelligence2( SI16 nVal );

	void					IncStrength( SI16 toInc = 1 );
	void					DecStrength( void );

	void					IncDexterity( SI16 toInc = 1 );
	void					DecDexterity( void );

	void					IncIntelligence( SI16 toInc = 1 );
	void					DecIntelligence( void );

	UI32					GetWord( UI08 wordNum ) const;
	bool					GetBit( UI08 wordNum, UI08 bitNum ) const;
	UI32					GetBitRange( UI08 wordNum, UI08 lowBit, UI08 highBit ) const;

	void					SetWord( UI08 wordNum, UI32 value );
	void					SetBit( UI08 wordNum, UI08 bitNum, bool value );
	void					SetBitRange( UI08 wordNum, UI08 lowBit, UI08 highBit, UI32 value );

	virtual void			PostLoadProcessing( void );
	virtual bool			LoadRemnants( void ) = 0;

	UI08					WorldNumber( void ) const;
	void					WorldNumber( UI08 value );

	UI08					GetPoisoned( void ) const;
	virtual void			SetPoisoned( UI08 newValue );

	SI16					GetCarve( void ) const;
	void					SetCarve( SI16 newValue );

	virtual void			Update( CSocket *mSock = NULL ) = 0;
	virtual void			SendToSocket( CSocket *mSock ) = 0;
	void					Dirty( UpdateTypes updateType );

	bool					GetUpdate( UpdateTypes updateType );

	virtual void			Delete( void ) = 0;
	virtual void			Cleanup( void );

	virtual void			RemoveSelfFromOwner( void ) = 0;
	virtual void			AddSelfToOwner( void ) = 0;

	bool					isFree( void ) const;
	bool					isDeleted( void ) const;
	bool					isPostLoaded( void ) const;
	bool					isSpawned( void ) const;
	bool					ShouldSave( void ) const;
	bool					isDisabled( void ) const;

	void					SetFree( bool newVal );
	void					SetDeleted( bool newVal );
	void					SetPostLoaded( bool newVal );
	void					SetSpawned( bool newVal );
	void					ShouldSave( bool newVal );
	void					SetDisabled( bool newVal );

};

}
#endif

