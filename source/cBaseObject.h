//o------------------------------------------------------------------------------------------------o
//|	File		-	cBaseObject.h
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Defines the property specifications for the base object class
//o------------------------------------------------------------------------------------------------o
#ifndef __CBO_H__
#define __CBO_H__

#include "typedefs.h"
#include "uoxstruct.h"

enum TAGMAPOBJECT_TYPE
{
	TAGMAP_TYPE_INT		= 0,
	TAGMAP_TYPE_STRING,
	TAGMAP_TYPE_BOOL
};

typedef struct __TAGMAP_STRUCT__
{
	UI08	m_ObjectType;
	SI32	m_IntValue;
	bool	m_Destroy;
	std::string m_StringValue;
} TAGMAPOBJECT, *LPTAGMAPOBJECT;

typedef std::map<std::string, TAGMAPOBJECT> TAGMAP2;
typedef std::map<std::string, TAGMAPOBJECT>::iterator TAGMAP2_ITERATOR;
typedef std::map<std::string, TAGMAPOBJECT>::const_iterator TAGMAP2_CITERATOR;

enum UpdateTypes
{
	UT_UPDATE = 0,
	UT_LOCATION,
	UT_HITPOINTS,
	UT_STAMINA,
	UT_MANA,
	UT_HIDE,
	UT_STATWINDOW,
	UT_COUNT
};

//o------------------------------------------------------------------------------------------------o
//|	Class		-	class CBaseObject
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	BaseObject class with common, shared properties for all object types
//o------------------------------------------------------------------------------------------------o
class CBaseObject
{
protected:
	TAGMAP2				tags;
	TAGMAP2				tempTags;
	std::string			title;
	ObjectType			objType;
	RACEID				race;
	SI16				x;
	SI16				y;
	SI08				z;
	UI16				id;
	UI16				colour;
	UI08				dir;
	SERIAL				serial;
	CMultiObj			*multis;
	SERIAL				spawnSerial;
	SERIAL				owner;
	UI08				worldNumber;
	UI16				instanceId;
	SI16				strength;
	SI16				dexterity;
	SI16				intelligence;
	SI16				hitpoints;
	VisibleTypes		visible;
	SI16				hiDamage;
	SI16				loDamage;
	SI32				weight;
	SI16				mana;
	SI16				stamina;
	SI16				healthLeech;
	SI16				staminaLeech;
	SI16				manaLeech;
	UI16				scriptTrig;
	SI16				st2;
	SI16				dx2;
	SI16				in2;
	mutable SI32		FilePosition;
	SERIAL				tempMulti;
	std::string			name;
	std::string			sectionId;
	std::vector<UI16>	scriptTriggers;
	UI08				poisoned;
	SI16				carve; // Carve.dfn entry
	SI16				oldLocX;
	SI16				oldLocY;
	SI08				oldLocZ;
	SI16				oldTargLocX;
	SI16				oldTargLocY;
	SI16				fame;
	SI16				karma;
	SI16				kills;
	UI16				subRegion;

	void			RemoveFromMulti( bool fireTrigger = true );
	void			AddToMulti( bool fireTrigger = true );

	std::bitset<8>	objSettings;

	UI16			resistances[WEATHNUM];

	SERIAL			tempContainerSerial;

	bool			nameRequestActive;
	//std::string	origin;	// Stores expansion item originates from
	UI08			origin;	// Stores expansion item originates from

	void			CopyData( CBaseObject *target );

public:

	Point3_st				GetOldLocation( void );

	size_t					GetNumTags( void ) const;
	auto					GetTagMap() const -> const TAGMAP2;
	auto					GetTempTagMap() const -> const TAGMAP2;

	TAGMAPOBJECT			GetTag( std::string tagname ) const;
	void					SetTag( std::string tagname, TAGMAPOBJECT tagval );

	TAGMAPOBJECT			GetTempTag( std::string tempTagName ) const;
	void					SetTempTag( std::string tempTagName, TAGMAPOBJECT tagVal );

	void					SetResist( UI16 newValue, WeatherType damage );
	UI16					GetResist( WeatherType damage ) const;

	void					SetTitle( std::string newtitle );
	std::string				GetTitle( void ) const;

	//void					SetOrigin( std::string newOrigin );
	//std::string				GetOrigin( void ) const;

	void					SetOrigin( UI08 value );
	UI08					GetOrigin() const;

	virtual void			SetMana( SI16 mn );
	SI16					GetMana( void ) const;
	virtual void			SetStamina( SI16 stam );
	SI16					GetStamina( void ) const;
	SI32					GetFilePosition( void ) const;
	SI32					SetFilePosition( SI32 filepos );

	virtual					~CBaseObject();
	CBaseObject( void );

	SI16					GetOldTargLocX( void ) const;
	SI16					GetOldTargLocY( void ) const;
	SI16					GetX( void ) const;
	SI16					GetY( void ) const;
	SI08					GetZ( void ) const;
	Point3_st				GetLocation( void ) const;

	void					SetOldTargLocX( SI16 newvalue );
	void					SetOldTargLocY( SI16 newvalue );
	void					SetX( SI16 newValue );
	void					SetY( SI16 newValue );
	void					SetZ( SI08 newValue );
	void					WalkXY( SI16 newX, SI16 newY );
	virtual void			SetOldLocation( SI16 newX, SI16 newY, SI08 newZ ) = 0;
	virtual void			SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world, UI16 instanceId ) = 0;
	virtual void			SetLocation( SI16 newX, SI16 newY, SI08 newZ ) = 0;
	virtual void			SetLocation( const CBaseObject *toSet ) = 0;

	UI16					GetId( void ) const;
	UI16					GetColour( void ) const;
	UI08					GetId( UI08 part ) const;
	UI08					GetColour( UI08 part ) const;

	void					SetId( UI16 newValue );
	void					SetColour( UI16 newValue );
	void					SetId( UI08 newValue, UI08 part );

	SI32					GetWeight( void ) const;
	virtual void			SetWeight( SI32 newVal, bool doWeightUpdate = true ) = 0;

	SERIAL					GetSerial( void ) const;
	SERIAL					GetSpawn( void ) const;
	SERIAL					GetOwner( void ) const;
	SERIAL					GetMulti( void ) const;
	CMultiObj *				GetMultiObj( void ) const;
	CSpawnItem *			GetSpawnObj( void ) const;
	CChar *					GetOwnerObj( void ) const;

	UI08					GetSerial( UI08 part ) const;
	UI08					GetSpawn( UI08 part ) const;

	void					SetMulti( SERIAL newSerial, bool fireTrigger = true );
	void					SetMulti( CMultiObj *newMulti, bool fireTrigger = true );
	void					SetSerial( SERIAL newSerial );
	void					SetSpawn( SERIAL newSpawn );
	virtual void			SetOwner( CChar *newOwner );

	virtual bool			Save( std::ostream &outStream ) = 0;
	virtual bool			DumpHeader( std::ostream &outStream ) const = 0;
	virtual bool			DumpBody( std::ostream &outStream ) const;
	bool					DumpFooter( std::ostream &outStream ) const;
	bool					Load( std::istream &inStream );

	virtual bool			HandleLine( std::string &UTag, std::string &data );

	RACEID					GetRace( void ) const;
	void					SetRace( RACEID newValue );

	std::string				GetNameRequest( CChar *nameRequester, UI08 requestSource );
	std::string				GetName( void ) const;
	void					SetName( std::string newName );

	std::string				GetSectionId( void ) const;
	void					SetSectionId( std::string newSectionID );

	virtual SI16			GetStrength( void ) const;
	virtual SI16			GetDexterity( void ) const;
	virtual SI16			GetIntelligence( void ) const;
	SI16					GetHP( void ) const;

	virtual void			SetStrength( SI16 newValue );
	virtual void			SetDexterity( SI16 newValue );
	virtual void			SetIntelligence( SI16 newValue );
	virtual void			SetHP( SI16 newValue );
	void					IncHP( SI16 amtToChange );

	void					SetDir( UI08 newDir, bool sendUpdate = true );
	UI08					GetDir( void ) const;

	void					SetVisible( VisibleTypes newValue );
	VisibleTypes			GetVisible( void ) const;

	ObjectType				GetObjType( void ) const;
	virtual bool			CanBeObjType( ObjectType toCompare ) const;

	SI16					GetHiDamage( void ) const;
	SI16					GetLoDamage( void ) const;

	void					SetHiDamage( SI16 newValue );
	void					SetLoDamage( SI16 newValue );

	std::vector<UI16>		GetScriptTriggers( void );
	void					AddScriptTrigger( UI16 newValue );
	bool					HasScriptTrigger( UI16 newValue );
	void					RemoveScriptTrigger( UI16 newValue );
	void					ClearScriptTriggers( void );

	SI16					GetStrength2( void ) const;
	SI16					GetDexterity2( void ) const;
	SI16					GetIntelligence2( void ) const;

	virtual void			SetStrength2( SI16 nVal );
	virtual void			SetDexterity2( SI16 nVal );
	virtual void			SetIntelligence2( SI16 nVal );

	void					IncStrength( SI16 toInc = 1 );
	void					IncDexterity( SI16 toInc = 1 );
	void					IncIntelligence( SI16 toInc = 1 );

	SI16					GetHealthLeech( void ) const;
	virtual void			SetHealthLeech( SI16 nVal );

	SI16					GetStaminaLeech( void ) const;
	virtual void			SetStaminaLeech( SI16 nVal );

	SI16					GetManaLeech( void ) const;
	virtual void			SetManaLeech( SI16 nVal );

	void					IncHealthLeech( SI16 toInc = 1 );
	void					IncStaminaLeech( SI16 toInc = 1 );
	void					IncManaLeech( SI16 toInc = 1 );

	virtual void			PostLoadProcessing( void );
	virtual bool			LoadRemnants( void ) = 0;

	UI08					WorldNumber( void ) const;
	void					WorldNumber( UI08 value );

	UI16					GetInstanceId( void ) const;
	void					SetInstanceId( UI16 value );

	UI16					GetSubRegion( void ) const;
	void					SetSubRegion( UI16 value );

	UI08					GetPoisoned( void ) const;
	virtual void			SetPoisoned( UI08 newValue );

	SI16					GetCarve( void ) const;
	void					SetCarve( SI16 newValue );

	virtual	void			Update( CSocket *mSock = nullptr, bool drawGamePlayer = false, bool sendToSelf = true ) = 0;
	virtual	void			SendToSocket( CSocket *mSock, bool drawGamePlayer = false ) = 0;
	virtual	void			Dirty( UpdateTypes updateType );
	void					RemoveFromRefreshQueue( void );

	virtual void			Delete( void ) = 0;
	virtual void			Cleanup( void );

	virtual	void			RemoveSelfFromOwner( void ) = 0;
	virtual	void			AddSelfToOwner( void ) = 0;

	bool					IsFree( void ) const;
	bool					IsDeleted( void ) const;
	bool					IsPostLoaded( void ) const;
	bool					IsSpawned( void ) const;
	bool					ShouldSave( void ) const;
	bool					IsDisabled( void ) const;
	bool					IsWipeable( void ) const;
	bool					IsDamageable( void ) const;
	bool					NameRequestActive( void ) const;

	void					SetFree( bool newVal );
	void					SetDeleted( bool newVal );
	void					SetPostLoaded( bool newVal );
	void					SetSpawned( bool newVal );
	void					ShouldSave( bool newVal );
	void					SetDisabled( bool newVal );
	void					SetWipeable( bool newValue );
	void					SetDamageable( bool newValue );
	void					NameRequestActive( bool newValue );

	SI16					GetFame( void ) const;
	void					SetFame( SI16 value );
	void					SetKarma( SI16 value );
	SI16					GetKarma( void ) const;
	void					SetKills( SI16 value );
	SI16					GetKills( void ) const;

};

#endif
