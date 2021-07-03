//o-----------------------------------------------------------------------------------------------o
//|	File			-	cBaseobject.cpp
//|	Date			-	7/26/2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose			-	Handles base object stuff shared between characters, items and multis
//|
//|	Version History	-
//|
//|							1.0		26th July, 2000
//|							Initial implementation.  Most, if not all, common between
//|							items and chars is now implemented here	Functions declared
//|							as const where possible
//|
//|							1.1		Unknown
//|							Significant fraction of things moved to CBO to support
//|							future functionality
//|
//|							1.2		August 27th, 2000
//|							Addition of basic script trigger stuff. Function documentation
//|							finished for all functions
//o-----------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "power.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "network.h"
#include "ObjectFactory.h"
#include "weight.h"
#include <bitset>


const UI32 BIT_FREE			=	0;
const UI32 BIT_DELETED		=	1;
const UI32 BIT_POSTLOADED	=	2;
const UI32 BIT_SPAWNED		=	3;
const UI32 BIT_SAVE			=	4;
const UI32 BIT_DISABLED		=	5;
const UI32 BIT_WIPEABLE		=	6;
const UI32 BIT_DAMAGEABLE	=	7;


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject destructor
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	This function is does basically what the name implies
//o-----------------------------------------------------------------------------------------------o
CBaseObject::~CBaseObject()
{
	if( multis != nullptr )
		RemoveFromMulti( false );
	// Delete all tags.
	tags.clear();
}

const ObjectType	DEFBASE_OBJTYPE		= OT_CBO;
const RACEID		DEFBASE_RACE		= 0;
const SI16			DEFBASE_X			= 100;
const SI16			DEFBASE_Y			= 100;
const SI08			DEFBASE_Z			= 0;
const UI16			DEFBASE_ID			= 1;
const UI16			DEFBASE_COLOUR		= 0;
const UI08			DEFBASE_DIR			= 0;
const SERIAL		DEFBASE_SERIAL		= INVALIDSERIAL;
CMultiObj *			DEFBASE_MULTIS		= nullptr;
const SERIAL		DEFBASE_SPAWNSER	= INVALIDSERIAL;
const SERIAL		DEFBASE_OWNER		= INVALIDSERIAL;
const UI08			DEFBASE_WORLD		= 0;
const UI16			DEFBASE_INSTANCEID	= 0;
const SI16			DEFBASE_STR			= 0;
const SI16			DEFBASE_DEX			= 0;
const SI16			DEFBASE_INT			= 0;
const SI16			DEFBASE_HP			= 1;
const VisibleTypes	DEFBASE_VISIBLE		= VT_VISIBLE;
const SI16			DEFBASE_HIDAMAGE	= 0;
const SI16			DEFBASE_LODAMAGE	= 0;
const SI32			DEFBASE_WEIGHT		= 0;
const SI16			DEFBASE_MANA		= 1;
const SI16			DEFBASE_STAMINA		= 1;
const UI16			DEFBASE_SCPTRIG		= 0;
const SI16			DEFBASE_STR2		= 0;
const SI16			DEFBASE_DEX2		= 0;
const SI16			DEFBASE_INT2		= 0;
const SI32			DEFBASE_FP			= -1;
const UI08			DEFBASE_POISONED	= 0;
const SI16			DEFBASE_CARVE		= 0;
const SI16			DEFBASE_KARMA		= 0;
const SI16			DEFBASE_FAME		= 0;
const SI16			DEFBASE_KILLS		= 0;
const UI16			DEFBASE_RESIST 		= 0;

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject constructor
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	This function basically does what the name implies
//o-----------------------------------------------------------------------------------------------o
CBaseObject::CBaseObject( void ) : objType( DEFBASE_OBJTYPE ), race( DEFBASE_RACE ), x( DEFBASE_X ), y( DEFBASE_Y ),
z( DEFBASE_Z ), id( DEFBASE_ID ), colour( DEFBASE_COLOUR ), dir( DEFBASE_DIR ), serial( DEFBASE_SERIAL ),
multis( DEFBASE_MULTIS ), spawnserial( DEFBASE_SPAWNSER ), owner( DEFBASE_OWNER ),
worldNumber( DEFBASE_WORLD ), instanceID( DEFBASE_INSTANCEID), strength( DEFBASE_STR ), dexterity( DEFBASE_DEX ), intelligence( DEFBASE_INT ),
hitpoints( DEFBASE_HP ), visible( DEFBASE_VISIBLE ), hidamage( DEFBASE_HIDAMAGE ),
lodamage( DEFBASE_LODAMAGE ), weight( DEFBASE_WEIGHT ),
mana( DEFBASE_MANA ), stamina( DEFBASE_STAMINA ), scriptTrig( DEFBASE_SCPTRIG ), st2( DEFBASE_STR2 ), dx2( DEFBASE_DEX2 ),
in2( DEFBASE_INT2 ), FilePosition( DEFBASE_FP ),
poisoned( DEFBASE_POISONED ), carve( DEFBASE_CARVE ), oldLocX( 0 ), oldLocY( 0 ), oldLocZ( 0 ), oldTargLocX( 0 ), oldTargLocY( 0 ),
fame( DEFBASE_FAME ), karma( DEFBASE_KARMA ), kills( DEFBASE_KILLS )
{
	multis = nullptr;
	tempmulti = INVALIDSERIAL;
	objSettings.reset();
	temp_container_serial = INVALIDSERIAL;
	name.reserve( MAX_NAME );
	title.reserve( MAX_TITLE );
	if( cwmWorldState != nullptr && cwmWorldState->GetLoaded() )
		SetPostLoaded( true );
	ShouldSave( true );
	memset( &resistances[0], DEFBASE_RESIST, sizeof( UI16 ) * WEATHNUM );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetNumTags( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Return the number of tags in an object's tag map
//o-----------------------------------------------------------------------------------------------o
size_t CBaseObject::GetNumTags( void ) const
{
	return tags.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	TAGMAPOBJECT GetTag( std::string tagname ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Fetch custom tag with specified name from object's tag map
//o-----------------------------------------------------------------------------------------------o
TAGMAPOBJECT CBaseObject::GetTag( std::string tagname ) const
{
	TAGMAPOBJECT localObject;
	localObject.m_ObjectType	= TAGMAP_TYPE_INT;
	localObject.m_IntValue		= 0;
	localObject.m_Destroy		= FALSE;
	localObject.m_StringValue	= "";
	TAGMAP2_CITERATOR CI = tags.find( tagname );
	if( CI != tags.end() )
		localObject = CI->second;

	return localObject;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetTag( std::string tagname, TAGMAPOBJECT tagval )
//|	Date		-	Unknown / Feb 3, 2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Store custom string/int tag in an object's tag map
//o-----------------------------------------------------------------------------------------------o
//| Changes		-	Updated the function to use the internal tagmap object instead of using some
//|					stored jsval in a context that may or may not change when reloaded.
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::SetTag( std::string tagname, TAGMAPOBJECT tagval )
{
	TAGMAP2_ITERATOR I = tags.find( tagname );
	if( I != tags.end() )
	{
		// Check to see if this object needs to be destroyed
		if( I->second.m_Destroy || tagval.m_Destroy )
		{
			tags.erase( I );
			return;
		}
		// Change the tag's TAGMAPOBJECT value. NOTE this will also change type should type be changed
		else if( tagval.m_ObjectType == TAGMAP_TYPE_STRING )
		{
			I->second.m_Destroy		= FALSE;
			I->second.m_ObjectType	= tagval.m_ObjectType;
			I->second.m_StringValue	= tagval.m_StringValue;
			// Just because it seemed like a waste to leave it unused. I put the length of the string in the int member
			I->second.m_IntValue	= static_cast<SI32>(tagval.m_StringValue.length());
		}
		else
		{
			I->second.m_Destroy		= FALSE;
			I->second.m_ObjectType	= tagval.m_ObjectType;
			I->second.m_StringValue	= "";
			I->second.m_IntValue	= tagval.m_IntValue;
		}
	}
	else
	{	// We need to create a TAGMAPOBJECT and initialize and store into the tagmap
		if( !tagval.m_Destroy )
			tags[tagname] = tagval;
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetOldTargLocX( void ) const
//|					void SetOldTargLocX( SI16 newValue )
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets old target X location for object - used in pathfinding
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetOldTargLocX( void ) const
{
	return oldTargLocX;
}
void CBaseObject::SetOldTargLocX( SI16 newValue )
{
	oldTargLocX = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetOldTargLocY( void ) const
//|					void SetOldTargLocY( SI16 newValue )
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets old target Y location for object - used in pathfinding
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetOldTargLocY( void ) const
{
	return oldTargLocY;
}
void CBaseObject::SetOldTargLocY( SI16 newValue )
{
	oldTargLocY = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetX( void ) const
//|					void SetX( SI16 newValue )
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets X location of object, but also stores old location
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetX( void ) const
{
	return x;
}
void CBaseObject::SetX( SI16 newValue )
{
	oldLocX = x;
	x		= newValue;
	Dirty( UT_LOCATION );
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetY( void ) const
//|					void SetY( SI16 newValue )
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets Y location of object, but also stores old location
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetY( void ) const
{
	return y;
}
void CBaseObject::SetY( SI16 newValue )
{
	oldLocY = y;
	y		= newValue;
	Dirty( UT_LOCATION );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 GetZ( void ) const
//|					void SetZ( SI08 newValue )
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets Z location of object, but also stores old location
//o-----------------------------------------------------------------------------------------------o
SI08 CBaseObject::GetZ( void ) const
{
	return z;
}
void CBaseObject::SetZ( SI08 newValue )
{
	oldLocZ = z;
	z		= newValue;
	Dirty( UT_LOCATION );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void WalkXY( SI16 newX, SI16 newY )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Update an object's X and Y location values
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::WalkXY( SI16 newX, SI16 newY )
{
	oldLocX = x;
	oldLocY = y;
	x = newX;
	y = newY;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetResist( WeatherType damage ) const
//|					void SetResist( UI16 newValue, WeatherType damage )
//|	Date		-	19. Mar, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets object's resistances versus different damage types
//o-----------------------------------------------------------------------------------------------o
UI16 CBaseObject::GetResist( WeatherType damage ) const
{
	return resistances[damage];
}
void CBaseObject::SetResist( UI16 newValue, WeatherType damage )
{
	resistances[damage] = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetID( void ) const
//|					void SetID( UI16 newValue )
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the ID of the object
//o-----------------------------------------------------------------------------------------------o
UI16 CBaseObject::GetID( void ) const
{
	return id;
}
void CBaseObject::SetID( UI16 newValue )
{
	CBaseObject *checkCont = nullptr;
	if( isPostLoaded() && CanBeObjType( OT_ITEM ) )
		checkCont = (static_cast<CItem *>(this))->GetCont();

	if( ValidateObject( checkCont ) )
		Weight->subtractItemWeight( checkCont, static_cast<CItem *>(this) );

	id = newValue;

	if( ValidateObject( checkCont ) )
		Weight->addItemWeight( checkCont, static_cast<CItem *>(this) );

	Dirty( UT_HIDE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetID( UI08 part ) const
//|					void SetID( UI08 newValue, UI08 part )
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets part of the ID
//o-----------------------------------------------------------------------------------------------o
UI08 CBaseObject::GetID( UI08 part ) const
{
	UI08 rvalue = static_cast<UI08>(id>>8);
	if( part == 2 )
		rvalue = static_cast<UI08>(id%256);
	return rvalue;
}
void CBaseObject::SetID( UI08 newValue, UI08 part )
{
	if( part <= 2 && part > 0 )
	{
		UI08 parts[2];
		parts[0]		= static_cast<UI08>(id>>8);
		parts[1]		= static_cast<UI08>(id%256);
		parts[part-1]	= newValue;
		SetID( static_cast<UI16>((parts[0]<<8) + parts[1]) );
	}
	else
		Console << "Invalid part requested on SetID" << myendl;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetColour( void ) const
//|					void SetColour( UI16 newValue )
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the colour of the object
//o-----------------------------------------------------------------------------------------------o
UI16 CBaseObject::GetColour( void ) const
{
	return colour;
}
void CBaseObject::SetColour( UI16 newValue )
{
	colour = newValue;
	Dirty( UT_UPDATE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetColour( UI08 part ) const
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns part of the colour
//o-----------------------------------------------------------------------------------------------o
UI08 CBaseObject::GetColour( UI08 part ) const
{
	UI08 rvalue = static_cast<UI08>(colour>>8);
	if( part == 2 )
		rvalue = static_cast<UI08>(colour%256);
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|   Function    -  SI32 GetWeight( void ) const
//|   Date        -  Unknown
//o-----------------------------------------------------------------------------------------------o
//|   Purpose     -  Weight of the CHARACTER
//o-----------------------------------------------------------------------------------------------o
SI32 CBaseObject::GetWeight( void ) const
{
	return weight;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CMultiObj *GetMultiObj( void ) const
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the multi object that the object is inside
//o-----------------------------------------------------------------------------------------------o
CMultiObj *CBaseObject::GetMultiObj( void ) const
{
	return multis;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetMulti( void ) const
//|					void SetMulti( SERIAL newSerial, bool fireTrigger )
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the serial of the multi the object is inside
//o-----------------------------------------------------------------------------------------------o
SERIAL CBaseObject::GetMulti( void ) const
{
	SERIAL multiSer = INVALIDSERIAL;
	if( ValidateObject( multis ) )
		multiSer = multis->GetSerial();

	return multiSer;
}
void CBaseObject::SetMulti( SERIAL newSerial, bool fireTrigger )
{
	RemoveFromMulti( fireTrigger );
	if( newSerial >= BASEITEMSERIAL )
	{
		CMultiObj *newMulti = calcMultiFromSer( newSerial );
		if( ValidateObject( newMulti ) )
		{
			multis = newMulti;
			AddToMulti( fireTrigger );
		}
		else
			multis = nullptr;
	}
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetSerial( void ) const
//|					void SetSerial( SERIAL newSerial )
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sers serial of the object
//o-----------------------------------------------------------------------------------------------o
SERIAL CBaseObject::GetSerial( void ) const
{
	return serial;
}
void CBaseObject::SetSerial( SERIAL newSerial )
{
	if( GetSerial() != INVALIDSERIAL )
		ObjectFactory::getSingleton().UnregisterObject( this );
	serial = newSerial;
	if( newSerial != INVALIDSERIAL )
		ObjectFactory::getSingleton().RegisterObject( this, newSerial );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetSpawn( void ) const
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns SERIAL of thing that spawned it
//o-----------------------------------------------------------------------------------------------o
SERIAL CBaseObject::GetSpawn( void ) const
{
	return spawnserial;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnItem *GetSpawnObj( void ) const
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns thing that spawned us - cannot be a character!
//o-----------------------------------------------------------------------------------------------o
CSpawnItem *CBaseObject::GetSpawnObj( void ) const
{
	CSpawnItem *ourSpawner = static_cast<CSpawnItem *>(calcItemObjFromSer( spawnserial ));
	if( ValidateObject( ourSpawner ) && ourSpawner->GetObjType() == OT_SPAWNER )
		return ourSpawner;
	return nullptr;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetOwner( void ) const
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns SERIAL of thing that owns us
//o-----------------------------------------------------------------------------------------------o
SERIAL CBaseObject::GetOwner( void ) const
{
	return owner;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CChar *GetOwnerObj( void ) const
//|	Date		-	26 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns thing that owns us
//o-----------------------------------------------------------------------------------------------o
CChar *CBaseObject::GetOwnerObj( void ) const
{
	return calcCharObjFromSer( owner );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetOwner( CChar *newOwner )
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the object's owner to newOwner
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::SetOwner( CChar *newOwner )
{
	RemoveSelfFromOwner();
	if( ValidateObject( newOwner ) )
		owner = newOwner->GetSerial();
	else
		owner = INVALIDSERIAL;
	AddSelfToOwner();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DumpBody( std::ofstream &outStream ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Dumps out body information of the object
//|					This is tag/data pairing information
//o-----------------------------------------------------------------------------------------------o
bool CBaseObject::DumpBody( std::ofstream &outStream ) const
{
	//static std::string destination; //static, construct only once
	//static std::ostringstream objectDump( destination ); //static, construct only once
	//objectDump.str(std::string()); // clear the stream
	//destination = ""; // clear the string

	SI16 temp_st2, temp_dx2, temp_in2;

	// Hexadecimal Values
	outStream << std::hex;
	outStream << "Serial=" << "0x" << serial << '\n';
	outStream << "ID=" << "0x" << id << '\n';
	outStream << "Colour=" << "0x" << colour << '\n';
	outStream << "Direction=" << "0x" << (SI16)dir << '\n';
	if( ValidateObject( multis ) )
	{
		outStream << "MultiID=" << "0x";
		try
		{
			outStream << multis->GetSerial() << '\n';
		}
		catch( ... )
		{
			outStream << "FFFFFFFF" << '\n';
			Console << "EXCEPTION: CBaseObject::DumpBody(" << name << "[" << serial << "]) - 'MultiID' points to invalid memory." << myendl;
		}
	}
	outStream << "SpawnerID=" << "0x" << spawnserial << '\n';
	outStream << "OwnerID=" << "0x" << owner << '\n';

	// Decimal / String Values
	outStream << std::dec;
	outStream << "Name=" << name << '\n';
	outStream << "Location=" << x << "," << y << "," << (SI16)z << "," << (SI16)worldNumber << "," << (UI16)instanceID << '\n';
	outStream << "Title=" << title << '\n';
	//=========== BUG (= For Characters the dex+str+int malis get saved and get rebuilt on next serverstartup = increasing malis)
	temp_st2 = st2;
	temp_dx2 = dx2;
	temp_in2 = in2;
	if( objType == OT_CHAR )
	{
		CChar *myChar = (CChar*)(this);

		// For every equipped item
		// We need to reduce Str2+Dex2+Int2
		for( CItem *myItem = myChar->FirstItem(); !myChar->FinishedItems(); myItem = myChar->NextItem() )
		{
			if( ValidateObject( myItem ) )
			{
				temp_st2 -= myItem->GetStrength2();
				temp_dx2 -= myItem->GetDexterity2();
				temp_in2 -= myItem->GetIntelligence2();
			}
		}
	}
	//=========== BUGFIX END
	outStream << "Weight="  << weight << '\n';
	outStream << "Mana=" << mana << '\n';
	outStream << "Stamina=" << stamina << '\n';
	outStream << "Dexterity=" << dexterity << "," << temp_dx2 << '\n';
	outStream << "Intelligence=" << intelligence << "," << temp_in2 << '\n';
	outStream << "Strength=" << strength << "," << temp_st2 << '\n';
	outStream << "HitPoints=" << hitpoints << '\n';
	outStream << "Race=" << race << '\n';
	outStream << "Visible=" << (SI16)visible << '\n';
	outStream << "Disabled=" << (isDisabled()?"1":"0") << '\n';
	outStream << "Damage=" << lodamage << "," << hidamage << '\n';
	outStream << "Poisoned=" << (SI16)poisoned << '\n';
	outStream << "Carve=" << GetCarve() << '\n';
	outStream << "Damageable=" << (isDamageable()?"1":"0") << '\n';
	outStream << "Defense=";
	for( UI08 resist = 1; resist < WEATHNUM; ++resist )
	{
		outStream << GetResist( (WeatherType)resist ) << ",";
	}
	outStream << "[END]" << '\n';
	if( scriptTriggers.size() > 0 )
	{
		for( auto scriptTrig : scriptTriggers )
		{
			outStream << "ScpTrig=" + std::to_string(scriptTrig) + '\n';
		}
	}
	else
	{
		outStream << "ScpTrig=" << scriptTrig << '\n';
	}
	outStream << "Reputation=" << GetFame() << "," << GetKarma() << "," << GetKills() << '\n';
	// Spin the character tags to save make sure to dump them too
	TAGMAP2_CITERATOR CI;
	for( CI = tags.begin(); CI != tags.end(); ++CI )
	{
		outStream << "TAGNAME=" << CI->first << '\n';
		if( CI->second.m_ObjectType == TAGMAP_TYPE_STRING )
		{
			outStream << "TAGVALS=" << CI->second.m_StringValue << '\n';
		}
		else
		{
			outStream << "TAGVAL=" << ((SI32)CI->second.m_IntValue) << '\n';
		}
	}
	//====================================================================================
	// We can have exceptions, but return no errors ?
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	RACEID GetRace( void ) const
//|					void SetRace( RACEID newValue )
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the race ID associaed with the object
//o-----------------------------------------------------------------------------------------------o
RACEID CBaseObject::GetRace( void ) const
{
	return race;
}
void CBaseObject::SetRace( RACEID newValue )
{
	race = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string GetName( void ) const
//|					void SetName( std::string newName )
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the name of the object
//o-----------------------------------------------------------------------------------------------o
std::string CBaseObject::GetName( void ) const
{
	return name;
}
void CBaseObject::SetName( std::string newName )
{
	name = newName.substr( 0, MAX_NAME - 1 );
	Dirty( UT_UPDATE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetStrength( void ) const
//|					void SetStrength( SI16 newValue )
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the strength of the object
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetStrength( void ) const
{
	return strength;
}
void CBaseObject::SetStrength( SI16 newValue )
{
	strength = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetDexterity( void ) const
//|					void SetDexterity( SI16 newValue )
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the dexterity of the object
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetDexterity( void ) const
{
	return dexterity;
}
void CBaseObject::SetDexterity( SI16 newValue )
{
	dexterity = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetIntelligence( void ) const
//|					void SetIntelligence( SI16 newValue )
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the intelligence of the object
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetIntelligence( void ) const
{
	return intelligence;
}
void CBaseObject::SetIntelligence( SI16 newValue )
{
	intelligence = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetHP( void ) const
//|					void SetHP( SI16 newValue )
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the hitpoints of the object
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetHP( void ) const
{
	return hitpoints;
}
void CBaseObject::SetHP( SI16 newValue )
{
	hitpoints = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void IncHP( SI16 amtToChange )
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Increments the hitpoints of the object by the specified value
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::IncHP( SI16 amtToChange )
{
	SetHP( hitpoints + amtToChange );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetDir( void ) const
//|					void SetDir( UI08 newDir )
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the direction of the object
//o-----------------------------------------------------------------------------------------------o
UI08 CBaseObject::GetDir( void ) const
{
	return dir;
}
void CBaseObject::SetDir( UI08 newDir )
{
	dir = newDir;
	Dirty( UT_UPDATE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	VisibleTypes GetVisible( void ) const
//|					void SetVisible( VisibleTypes newValue )
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the visibility property of the object
//|	Notes		-	Generally it is
//|						0 = Visible
//|						1 = Temporary Hidden (Skill, Item visible to owner)
//|						2 = Invisible (Magic Invis)
//|						3 = Permanent Hidden (GM Hide)
//o-----------------------------------------------------------------------------------------------o
VisibleTypes CBaseObject::GetVisible( void ) const
{
	return visible;
}
void CBaseObject::SetVisible( VisibleTypes newValue )
{
	visible = newValue;
	Dirty( UT_HIDE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	ObjectType GetObjType( void ) const
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns an ObjectType that indicates the item's type
//o-----------------------------------------------------------------------------------------------o
ObjectType CBaseObject::GetObjType( void ) const
{
	return objType;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanBeObjType( ObjectType toCompare ) const
//|	Date		-	24 June, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Indicates whether an object can behave as a particular type
//o-----------------------------------------------------------------------------------------------o
bool CBaseObject::CanBeObjType( ObjectType toCompare ) const
{
	if( toCompare == OT_CBO )
		return true;
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveFromMulti( bool fireTrigger )
//|	Date		-	15 December, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes object from a multi, selectively firing the trigger
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::RemoveFromMulti( bool fireTrigger )
{
	if( ValidateObject( multis ) )
	{
		if( multis->CanBeObjType( OT_MULTI ) )
		{
			multis->RemoveFromMulti( this );
			if( fireTrigger )
			{
				// First, trigger the OnLeaving event for the multi an object is removed from
				std::vector<UI16> scriptTriggers = multis->GetScriptTriggers();
				for( auto i : scriptTriggers )
				{
					cScript *toExecute = JSMapping->GetScript( i );
					if( toExecute != nullptr )
					{
						// If script returns true/1, prevent other onLeaving events from triggering
						if( toExecute->OnLeaving( multis, this ) == 1 )
						{
							break;
						}

					}
				}
				// Clear scriptTriggers vector so we can re-use it below
				scriptTriggers.clear();

				// Then, trigger the same event for the object being removed
				scriptTriggers = GetScriptTriggers();
				for( auto i : scriptTriggers )
				{
					cScript *toExecute = JSMapping->GetScript( i );
					if( toExecute != nullptr )
					{
						// If script returns true/1, prevent other onLeaving events from triggering
						if( toExecute->OnLeaving( multis, this ) == 1 )
						{
							break;
						}
					}
				}
			}
		}
		else
			Console.error( strutil::format("Object of type %i with serial 0x%X has a bad multi setting of %i", GetObjType(), serial, multis->GetSerial()) );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddToMulti( bool fireTrigger )
//|	Date		-	15 December, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds object to multi
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::AddToMulti( bool fireTrigger )
{
	if( CanBeObjType( OT_MULTI ) )
	{
		multis = nullptr;
		return;
	}
	if( ValidateObject( multis ) )
	{
		if( multis->CanBeObjType( OT_MULTI ) )
		{
			multis->AddToMulti( this );
			if( fireTrigger )
			{
				// First, trigger the onEntrance script attached to the multi, if any
				std::vector<UI16> scriptTriggers = multis->GetScriptTriggers();
				for( auto i : scriptTriggers )
				{
					cScript *toExecute = JSMapping->GetScript( i );
					if( toExecute != nullptr )
					{
						// If script returns true/1, prevent other onEntrance events from triggering
						if( toExecute->OnEntrance( multis, this ) == 1 )
						{
							break;
						}

					}
				}

				// Clear scriptTriggers vector so we can reuse it below
				scriptTriggers.clear();
				scriptTriggers.shrink_to_fit();

				// Then, trigger the onEntrance script attached to the object entering the multi
				scriptTriggers = GetScriptTriggers();
				for( auto i : scriptTriggers )
				{
					cScript *toExecute = JSMapping->GetScript( i );
					if( toExecute != nullptr )
					{
						// If script returns true/1, prevent other onEntrance events from triggering
						if( toExecute->OnEntrance( multis, this ) == 1 )
						{
							break;
						}
					}
				}
			}
		}
		else
			Console.error(strutil::format( "Object of type %i with serial 0x%X has a bad multi setting of %X", GetObjType(), serial, multis->GetSerial() ));
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetMulti( CMultiObj *newMulti, bool fireTrigger )
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the object's multi to newMulti
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::SetMulti( CMultiObj *newMulti, bool fireTrigger )
{
	RemoveFromMulti( fireTrigger );
	multis = newMulti;
	AddToMulti( fireTrigger );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetSpawn( SERIAL newSpawn )
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the object's spawner to newSpawn
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::SetSpawn( SERIAL newSpawn )
{
	CSpawnItem *ourSpawner = GetSpawnObj();
	if( ourSpawner != nullptr )
		ourSpawner->spawnedList.Remove( this );
	spawnserial = newSpawn;
	if( newSpawn != INVALIDSERIAL )
	{
		ourSpawner = GetSpawnObj();
		if( ourSpawner != nullptr )
			ourSpawner->spawnedList.Add( this );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetSerial( UI08 part ) const
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns part of a serial #
//o-----------------------------------------------------------------------------------------------o
UI08 CBaseObject::GetSerial( UI08 part ) const
{
	switch( part )
	{
		case 1:	return static_cast<UI08>(serial>>24);
		case 2:	return static_cast<UI08>(serial>>16);
		case 3: return static_cast<UI08>(serial>>8);
		case 4: return static_cast<UI08>(serial%256);
	}
	return 0;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetSpawn( UI08 part ) const
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns part of the item's spawner serial
//o-----------------------------------------------------------------------------------------------o
UI08 CBaseObject::GetSpawn( UI08 part ) const
{
	UI08 rvalue = 0;
	switch( part )
	{
		case 1:	rvalue = static_cast<UI08>(spawnserial>>24);	break;
		case 2:	rvalue = static_cast<UI08>(spawnserial>>16);	break;
		case 3:	rvalue = static_cast<UI08>(spawnserial>>8);		break;
		case 4:	rvalue = static_cast<UI08>(spawnserial%256);	break;
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetHiDamage( void ) const
//|					void SetHiDamage( SI16 newValue )
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's high damage value (for randomization purposes)
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetHiDamage( void ) const
{
	return hidamage;
}
void CBaseObject::SetHiDamage( SI16 newValue )
{
	hidamage = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetLoDamage( void ) const
//|					void SetLoDamage( SI16 newValue )
//|	Date		-	28 July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's low damage value (for randomization purposes)
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetLoDamage( void ) const
{
	return lodamage;
}
void CBaseObject::SetLoDamage( SI16 newValue )
{
	lodamage = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 SetFilePosition( SI32 filepos )
//|					SI32 GetFilePosition( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's file position
//o-----------------------------------------------------------------------------------------------o
SI32 CBaseObject::GetFilePosition( void ) const
{
	return FilePosition;
}
SI32 CBaseObject::SetFilePosition( SI32 filepos )
{
	FilePosition = filepos;
	return FilePosition;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetStamina( void ) const
//|					void SetStamina( SI16 stam )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's stamina
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetStamina( void ) const
{
	return stamina;
}
void CBaseObject::SetStamina( SI16 stam )
{
	stamina = stam;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetMana( void ) const
//|					void SetMana( SI16 mn )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's mana
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetMana( void ) const
{
	return mana;
}
void CBaseObject::SetMana( SI16 mn )
{
	mana = mn;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string GetTitle( void ) const
//|					void SetTitle( std::string newtitle )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's title
//o-----------------------------------------------------------------------------------------------o
std::string CBaseObject::GetTitle( void ) const
{
	return title;
}
void CBaseObject::SetTitle( std::string newtitle )
{
	title = newtitle.substr( 0, MAX_TITLE - 1 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetScriptTriggers( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets list of script triggers on object
//o-----------------------------------------------------------------------------------------------o
std::vector<UI16> CBaseObject::GetScriptTriggers( void )
{
	return scriptTriggers;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddScriptTrigger( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a script trigger to object's list of script triggers
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::AddScriptTrigger( UI16 newValue )
{
	if( std::find(scriptTriggers.begin(), scriptTriggers.end(), newValue) == scriptTriggers.end() )
	{
		// Add scriptID to scriptTriggers if not already present
		scriptTriggers.push_back(newValue);
	}

	// Sort vector in ascending order, so order in which scripts are evaluated is predictable
	std::sort(scriptTriggers.begin(), scriptTriggers.end());
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveScriptTrigger( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a specific script trigger to object's list of script triggers
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::RemoveScriptTrigger( UI16 newValue )
{
	// Remove all elements containing specified script trigger from vector
	scriptTriggers.erase(std::remove(scriptTriggers.begin(), scriptTriggers.end(), newValue), scriptTriggers.end());
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ClearScriptTriggers( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Clears out all script triggers from object
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::ClearScriptTriggers( void )
{
	scriptTriggers.clear();
	scriptTriggers.shrink_to_fit();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	point3 GetLocation( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a point3 structure pointing to the object's current location
//o-----------------------------------------------------------------------------------------------o
point3 CBaseObject::GetLocation( void ) const
{
	return point3( x, y, z );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetStrength2( void ) const
//|					void SetStrength2( SI16 nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the second strength var associated with the object. For chars, it's the
//|					bonuses (via armour and such)
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetStrength2( void ) const
{
	return st2;
}
void CBaseObject::SetStrength2( SI16 nVal )
{
	st2 = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetDexterity2( void ) const
//|					void SetDexterity2( SI16 nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the second dexterity var associated with the object. For chars, it's
//|					the bonuses (via armour and such)
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetDexterity2( void ) const
{
	return dx2;
}
void CBaseObject::SetDexterity2( SI16 nVal )
{
	dx2 = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetIntelligence2( void ) const
//|					void SetIntelligence2( SI16 nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the second intelligence var associated with the object. For chars,
//|					it's the bonuses (via armour and such)
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetIntelligence2( void ) const
{
	return in2;
}
void CBaseObject::SetIntelligence2( SI16 nVal )
{
	in2 = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void IncStrength( SI16 toInc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Increments the object's strength value
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::IncStrength( SI16 toInc )
{
	SetStrength( strength + toInc );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void IncDexterity( SI16 toInc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Increments the object's dexterity value
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::IncDexterity( SI16 toInc )
{
	SetDexterity( dexterity + toInc );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void IncIntelligence( SI16 toInc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Increments the object's intelligence value
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::IncIntelligence( SI16 toInc )
{
	SetIntelligence( intelligence + toInc );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DumpFooter( std::ofstream &outStream ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Dumps out footer information so that a logical break between entries can
//|					be found without moving file pointers
//|						Mode 0 - Text
//|						Mode 1 - Binary
//o-----------------------------------------------------------------------------------------------o
bool CBaseObject::DumpFooter( std::ofstream &outStream ) const
{
	outStream << '\n' << "o---o" << '\n' << '\n';
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool Load( std::ifstream &inStream )
//|	Date		-	28 July, 2000
//|	Changes		-	(1/9/02) no longer needs mode
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads object from disk based on mode
//o-----------------------------------------------------------------------------------------------o
void ReadWorldTagData( std::ifstream &inStream, std::string &tag, std::string &data );
bool CBaseObject::Load( std::ifstream &inStream )
{
	std::string tag = "", data = "", UTag = "";
	while( tag != "o---o" )
	{
		ReadWorldTagData( inStream, tag, data );
		if( tag != "o---o" )
		{
			UTag = strutil::toupper( tag );
			if( !HandleLine( UTag, data ) )
				Console.warning( strutil::format("Unknown world file tag %s with contents of %s", tag.c_str(), data.c_str()) );
		}
	}
	return LoadRemnants();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool HandleLine( std::string &UTag, std::string &data )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to handle world lines. Returns true if the tag is known. If known,
//|					internal information updated and load routine continues to next tag.
//|					Otherwise, passed up inheritance tree (if any)
//o-----------------------------------------------------------------------------------------------o
bool CBaseObject::HandleLine( std::string &UTag, std::string &data )
{
	static std::string staticTagName = "";
	bool rvalue = true;
	auto csecs = strutil::sections( data, "," );
	
	switch( (UTag[0]) )
	{
		case 'B':
			if( UTag == "BASEWEIGHT" )
			{
				auto temp = static_cast<UI32>(std::stoul(data, nullptr, 0));
				(static_cast<CItem *>(this))->SetBaseWeight( temp );
			}
			else
				rvalue = false;
			break;
		case 'C':
			if( UTag == "COLOUR" )
			{
				auto temp = static_cast<UI16>(std::stoul(data, nullptr, 0));
				colour = temp;
			}
			else if( UTag == "CARVE" )
			{
				auto temp = static_cast<UI16>(std::stoul(data, nullptr, 0));
				carve = temp;
			}
			else
				rvalue = false;
			break;
		case 'D':
			if( UTag == "DAMAGE" )
			{
				if( csecs.size() >= 2 )
				{
					lodamage = static_cast<SI16>( std::stoi( strutil::stripTrim( csecs[0] ), nullptr, 0 ) );
					hidamage = static_cast<SI16>( std::stoi( strutil::stripTrim( csecs[1] ), nullptr, 0 ) );
				}
				else
				{
					// If there's only one value, set both to the same
					lodamage = static_cast<SI16>( std::stoi( strutil::stripTrim( data ), nullptr, 0 ) );
					hidamage = static_cast<SI16>( std::stoi( strutil::stripTrim( data ), nullptr, 0 ) );
				}
			}
			else if( UTag == "DAMAGEABLE" )
			{
				SetDamageable( strutil::value<std::uint8_t>(data) == 1 );
			}
			else if( UTag == "DIRECTION" )
			{
				auto temp = static_cast<UI08>(std::stoul(data, nullptr, 0));
				dir	= temp;
			}
			else if( UTag == "DEXTERITY" )
			{
				if( csecs.size() >= 2  )
				{
					dexterity = static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[0] ), nullptr, 0));	
					dx2	= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[1] ), nullptr, 0));
				}
				else
				{
					dexterity = static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0));
				}					
			}
			else if( UTag == "DEXTERITY2" )
			{
				dx2	= static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0));
			}
			else if( UTag == "DEFENSE" )
			{
				if( data.find( "," ) != std::string::npos )
				{
					int count = 1;
					for( auto &val : csecs )
					{
						if( !val.empty() )
						{
							auto temp = strutil::toupper( strutil::stripTrim( val ));
							if( temp == "[END]" )
							{
								break;
							}
							auto value = static_cast<SI16>(std::stoi(temp, nullptr, 0));
							SetResist( value, static_cast<WeatherType>(count) );
							count++ ;
						}
					}
				}
				else
				{	
					SetResist( static_cast<SI16>(std::stoi(strutil::stripTrim( data ), nullptr, 0)), PHYSICAL );
				}	
			}
			else if( UTag == "DISABLED" )
			{
				SetDisabled( strutil::value<std::int16_t>(data) == 1 );
			}
			else
			{
				rvalue = false;
			}
			break;
		case 'F':
			if( UTag == "FAME" )
			{
				SetFame( strutil::value<std::int16_t>(data) );
			}
			else
				rvalue = false;
			break;
		case 'H':
			if( UTag == "HITPOINTS" )
			{
				hitpoints	= strutil::value<std::int16_t>(data);
			}
			else if( UTag == "HIDAMAGE" )
			{
				hidamage	= strutil::value<std::int16_t>(data);
			}
			else
				rvalue = false;
			break;
		case 'I':
			if( UTag == "ID" )
			{
				id		= strutil::value<std::int16_t>(data);
			}
			else if( UTag == "INTELLIGENCE" )
			{
				if( data.find( "," ) != std::string::npos )
				{
					intelligence = static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[0] ), nullptr, 0));
					in2 = static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[1] ), nullptr, 0));
				}
				else
				{
					intelligence = strutil::value<std::int16_t>(data);
				}
			}
			else if( UTag == "INTELLIGENCE2" )
			{
				in2		= strutil::value<std::int16_t>(data);
			}
			else
				rvalue = false;
			break;
		case 'K':
			if( UTag == "KARMA" )
			{
				SetKarma( strutil::value<std::int16_t>(data) );
			}
			else if( UTag == "KILLS" )
			{
				SetKills( strutil::value<std::int16_t>(data) );
			}
			else
				rvalue = false;
			break;
		case 'L':
			if( UTag == "LOCATION" )
			{
				x			= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[0] ), nullptr, 0));
				y			= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[1] ), nullptr, 0));
				z			= static_cast<SI08>(std::stoi(strutil::stripTrim( csecs[2] ), nullptr, 0));
				worldNumber = static_cast<UI08>(std::stoi(strutil::stripTrim( csecs[3] ), nullptr, 0));

				// Backwards compatibility with pre-instanceID worldfiles
				if( csecs.size() >= 5 )
				{
					instanceID = static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[4] ), nullptr, 0));
				}
				else
				{
					instanceID = 0;
				}
			}
			else if( UTag == "LODAMAGE" )
			{
				lodamage	= strutil::value<std::int16_t>(data);
			}
			else
				rvalue = false;
			break;
		case 'M':
			if( UTag == "MANA" )
			{
				mana	= strutil::value<std::int16_t>(data);
			}
			else if( UTag == "MULTIID" )
			{
				tempmulti = (strutil::value<std::uint32_t>(data));
				multis = nullptr;
			}
			else
				rvalue = false;
			break;
		case 'N':
			if( UTag == "NAME" )
			{
				name = data.substr( 0, MAX_NAME - 1 );
			}
			else
				rvalue = false;
			break;
		case 'O':
			if( UTag == "OWNERID" )
			{
				owner	= strutil::value<std::uint32_t>(data);
			}
			else
				rvalue = false;
			break;
		case 'P':
			if( UTag == "POISONED" )
			{
				poisoned	= strutil::value<std::uint8_t>(data);
			}
			else
				rvalue = false;
			break;
		case 'R':
			if( UTag == "RACE" )
			{
				race	= strutil::value<std::uint16_t>(data);
			}
			else if( UTag == "REPUTATION" )
			{
				if( csecs.size() == 3 )
				{
					SetFame( static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
					SetKarma( static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
					SetKills( static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[2] ), nullptr, 0)) );
				}
			}
			else
			{
				rvalue = false;
			}
			break;
		case 'S':
			if( UTag == "STAMINA" )
			{
				stamina	= strutil::value<std::int16_t>(data);
			}
			else if( UTag == "SPAWNERID" )
			{
				spawnserial = strutil::value<std::uint32_t>(data);
			}
			else if( UTag == "SERIAL" )
			{
				serial = strutil::value<std::uint32_t>(data);
			}
			else if( UTag == "STRENGTH" )
			{
				if( csecs.size() >= 2 )
				{
					strength	= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[0] ), nullptr, 0));
					st2			= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[1] ), nullptr, 0));
				}
				else
				{
					strength = strutil::value<std::int16_t>(data);
				}
			}
			else if( UTag == "STRENGTH2" )
			{
				st2		= strutil::value<std::int16_t>(data);
			}
			else if( UTag == "SCPTRIG" )
			{
				//scriptTrig	= strutil::value<std::uint16_t>(data);
				std::uint16_t scriptID = strutil::value<std::uint16_t>(data);
				if( scriptID != 0 && scriptID != 65535 )
				{
					cScript *toExecute	= JSMapping->GetScript( scriptID );
					if( toExecute == nullptr )
					{
						Console.warning( strutil::format("SCPTRIG tag found with invalid script ID (%s) while loading world data!", data.c_str()) );
					}
					else
					{
						this->AddScriptTrigger( strutil::value<std::uint16_t>(data) );
					}
				}
			}
			else
				rvalue = false;
			break;
		case 'T':
			if( UTag == "TITLE" )
			{
				title = data.substr( 0, MAX_TITLE - 1 );
			}
			else if( UTag == "TAGNAME" )
			{
				staticTagName	= data;
			}
			else if( UTag == "TAGVAL" )
			{
				TAGMAPOBJECT tagvalObject;
				tagvalObject.m_ObjectType	= TAGMAP_TYPE_INT;
				tagvalObject.m_IntValue		= std::stoi(strutil::stripTrim( data ), nullptr, 0);
				tagvalObject.m_Destroy		= FALSE;
				tagvalObject.m_StringValue	= "";
				SetTag( staticTagName, tagvalObject );
			}
			else if( UTag == "TAGVALS" )
			{
				std::string localString = data;
				TAGMAPOBJECT tagvalObject;
				tagvalObject.m_ObjectType=TAGMAP_TYPE_STRING;
				tagvalObject.m_IntValue= static_cast<SI32>(localString.size());
				tagvalObject.m_Destroy=FALSE;
				tagvalObject.m_StringValue=localString;
				SetTag( staticTagName, tagvalObject );

			}
			else
				rvalue = false;
			break;
		case 'V':
			if( UTag == "VISIBLE" )
			{
				visible	= static_cast<VisibleTypes>(std::stoul(strutil::stripTrim( data )));
			}
			else
				rvalue = false;
			break;
		case 'W':
			if( UTag == "WEIGHT" )
			{
				SetWeight( strutil::value<std::int32_t>(data) );
			}
			else if( UTag == "WIPE" )
			{
				SetWipeable( strutil::value<std::uint8_t>(data) == 1 );
			}
			else if( UTag == "WORLDNUMBER" )
			{
				worldNumber = strutil::value<std::uint8_t>(data);
			}
			else
				rvalue = false;
			break;
		case 'X':
			if( UTag == "XYZ" )
			{
				if( csecs.size() >= 1 )
				{
					x = static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0));
				}
				else
				{
					x = 0;
				}
				if( csecs.size() >= 2 )
				{
					y = static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0));
				}
				else
				{
					y = 0;
				}
				if( csecs.size() >= 3 )
				{
					z = static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[2] ), nullptr, 0));
				}
				else
				{
					z = 0;
				}
			}
			else
				rvalue = false;
			break;
		default:
			rvalue = false;
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PostLoadProcessing( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to setup any pointers that may need adjustment
//|					following the loading of the world
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::PostLoadProcessing( void )
{
	SERIAL tmpSerial = INVALIDSERIAL;
	if( multis != nullptr )
	{
		multis		= nullptr;
		SetMulti( tempmulti, false );
	}
	if( spawnserial != INVALIDSERIAL )
	{
		tmpSerial	= spawnserial;
		spawnserial	= INVALIDSERIAL;
		SetSpawn( tmpSerial );
	}
	if( owner != INVALIDSERIAL ) //To repopulate the petlist of the owner
	{
		tmpSerial	= owner;
		owner		= INVALIDSERIAL;
		SetOwner( calcCharObjFromSer(tmpSerial) );
	}

	oldLocX = x;
	oldLocY = y;
	oldLocZ = z;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 WorldNumber( void ) const
//|					void WorldNumber( UI08 value )
//|	Date		-	26th September, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the world number that the object is in
//o-----------------------------------------------------------------------------------------------o
UI08 CBaseObject::WorldNumber( void ) const
{
	return worldNumber;
}
void CBaseObject::WorldNumber( UI08 value )
{
	worldNumber = value;
	Dirty( UT_LOCATION );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetInstanceID( void ) const
//|					void SetInstanceID( UI16 value )
//|	Date		-	24th June, 2020
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the instance ID the object is in
//o-----------------------------------------------------------------------------------------------o
UI16 CBaseObject::GetInstanceID( void ) const
{
	return instanceID;
}
void CBaseObject::SetInstanceID( UI16 value )
{
	instanceID = value;
	Dirty( UT_LOCATION );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetPoisoned( void ) const
//|					void SetPoisoned( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets an object's poisoned status
//o-----------------------------------------------------------------------------------------------o
UI08 CBaseObject::GetPoisoned( void ) const
{
	return poisoned;
}
void CBaseObject::SetPoisoned( UI08 newValue )
{
	poisoned = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetCarve( void ) const
//|					void SetCarve( SI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets an object's carve ID from carve DFN
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetCarve( void ) const
{
	return carve;
}
void CBaseObject::SetCarve( SI16 newValue )
{
	carve = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isFree( void ) const
//|					void SetFree( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether object is free(??)
//o-----------------------------------------------------------------------------------------------o
bool CBaseObject::isFree( void ) const
{
	return objSettings.test( BIT_FREE );
}
void CBaseObject::SetFree( bool newVal )
{
	objSettings.set( BIT_FREE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isDeleted( void ) const
//|					void SetDeleted( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether object has been marked as deleted
//o-----------------------------------------------------------------------------------------------o
bool CBaseObject::isDeleted( void ) const
{
	return objSettings.test( BIT_DELETED );
}
void CBaseObject::SetDeleted( bool newVal )
{
	objSettings.set( BIT_DELETED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isPostLoaded( void ) const
//|					void SetPostLoaded( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether object has finished loading
//o-----------------------------------------------------------------------------------------------o
bool CBaseObject::isPostLoaded( void ) const
{
	return objSettings.test( BIT_POSTLOADED );
}
void CBaseObject::SetPostLoaded( bool newVal )
{
	objSettings.set( BIT_POSTLOADED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isSpawned( void ) const
//|					void SetSpawned( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether object was spawned
//o-----------------------------------------------------------------------------------------------o
bool CBaseObject::isSpawned( void ) const
{
	return objSettings.test( BIT_SPAWNED );
}
void CBaseObject::SetSpawned( bool newVal )
{
	objSettings.set( BIT_SPAWNED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool ShouldSave( void ) const
//|					void ShouldSave( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether server should save object
//o-----------------------------------------------------------------------------------------------o
bool CBaseObject::ShouldSave( void ) const
{
	return objSettings.test( BIT_SAVE );
}
void CBaseObject::ShouldSave( bool newVal )
{
	objSettings.set( BIT_SAVE, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isDisabled( void ) const
//|					void SetDisabled( bool newVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether the object is disabled
//o-----------------------------------------------------------------------------------------------o
bool CBaseObject::isDisabled( void ) const
{
	return objSettings.test( BIT_DISABLED );
}
void CBaseObject::SetDisabled( bool newVal )
{
	objSettings.set( BIT_DISABLED, newVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool Cleanup()
//|	Date		-	11/6/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Cleans up after the object
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::Cleanup( void )
{
	SetX( 7000 );
	SetY( 7000 );
	SetZ( 0 );

	std::vector<UI16> scriptTriggers = GetScriptTriggers();
	for( auto i : scriptTriggers )
	{
		cScript *tScript = JSMapping->GetScript( i );
		if( tScript != nullptr )
			tScript->OnDelete( this );
	}

	QUEUEMAP_ITERATOR toFind = cwmWorldState->refreshQueue.find( this );
	if( toFind != cwmWorldState->refreshQueue.end() )
		cwmWorldState->refreshQueue.erase( toFind );

	if( ValidateObject( multis ) )
		SetMulti( INVALIDSERIAL, false );

	for( CSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
	{
		if( iSock != nullptr )
		{
			if( iSock->TempObj() != nullptr && iSock->TempObj() == this )
				iSock->TempObj( nullptr );
			if( iSock->TempObj2() != nullptr && iSock->TempObj2() == this )
				iSock->TempObj2( nullptr );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Dirty( void ) const
//|	Date		-	25 July, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Forces the object onto the global refresh queue
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::Dirty( UpdateTypes updateType )
{
	if( isDeleted() )
		Console.error( "Attempt was made to add deleted item to refreshQueue!" );
	else if( isPostLoaded() )
		++(cwmWorldState->refreshQueue[this]);
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveFromRefreshQueue( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes the object from the global refresh queue
//o-----------------------------------------------------------------------------------------------o
void CBaseObject::RemoveFromRefreshQueue()
{
	QUEUEMAP_ITERATOR toFind = cwmWorldState->refreshQueue.find( this );
	if( toFind != cwmWorldState->refreshQueue.end() )
		cwmWorldState->refreshQueue.erase( toFind );
}

void CBaseObject::CopyData( CBaseObject *target )
{
	target->SetTitle( GetTitle() );
	target->SetRace( GetRace() );
	target->SetName( GetName() );
	target->SetStrength( GetStrength() );
	target->SetDexterity( GetDexterity() );
	target->SetIntelligence( GetIntelligence() );
	target->SetHP( GetHP() );
	target->SetDir( GetDir() );
	target->SetVisible( GetVisible() );
	target->SetMana( GetMana() );
	target->SetStamina( GetStamina() );
	target->SetLocation( this );
	target->SetID( GetID() );
	target->SetColour( GetColour() );
	target->SetHiDamage( GetHiDamage() );
	target->SetLoDamage( GetLoDamage() );
	for( UI08 resist = 0; resist < WEATHNUM; ++resist )
	{
		target->SetResist( GetResist( (WeatherType)resist ), (WeatherType)resist );
	}
	target->SetStrength2( GetStrength2() );
	target->SetDexterity2( GetDexterity2() );
	target->SetIntelligence2( GetIntelligence2() );
	target->SetPoisoned( GetPoisoned() );
	target->SetWeight( GetWeight() );

	target->SetKarma( karma );
	target->SetFame( fame );
	target->SetKills( kills );
	target->SetWipeable( isWipeable() );
	target->SetDamageable( isDamageable() );
}

point3 CBaseObject::GetOldLocation( void )
{
	return point3( oldLocX, oldLocY, oldLocZ );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetKarma( void ) const
//|					void SetKarma( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's karma
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetKarma( void ) const
{
	return karma;
}
void CBaseObject::SetKarma( SI16 value )
{
	karma = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetFame( void ) const
//|					void SetFame( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the object's fame
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetFame( void ) const
{
	return fame;
}
void CBaseObject::SetFame( SI16 value )
{
	fame = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetKills( void ) const
//|					void SetKills( SI16 value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets an object's kill/murder count
//o-----------------------------------------------------------------------------------------------o
SI16 CBaseObject::GetKills( void ) const
{
	return kills;
}
void CBaseObject::SetKills( SI16 value )
{
	kills = value;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isWipeable( void ) const
//|					void SetWipeable( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether an item is affected by wipe command or not
//o-----------------------------------------------------------------------------------------------o
bool CBaseObject::isWipeable( void ) const
{
	return objSettings.test( BIT_WIPEABLE );
}
void CBaseObject::SetWipeable( bool newValue )
{
	objSettings.set( BIT_WIPEABLE, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isDamageable( void ) const
//|					void SetDamageable( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's damageable state
//o-----------------------------------------------------------------------------------------------o
bool CBaseObject::isDamageable(void) const
{
	return objSettings.test( BIT_DAMAGEABLE );
}
void CBaseObject::SetDamageable(bool newValue)
{
	objSettings.set( BIT_DAMAGEABLE, newValue );
}
