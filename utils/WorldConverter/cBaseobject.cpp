#include "uox3.h"
#include "power.h"

// Version History
// 1.0		Abaddon		26th July, 2000
//			Initial implementation.  Most, if not all, common between
//			items and chars is now implemented here
//			Functions declared as const where possible
// 1.1		EviLDeD		Unknown
//			Significant fraction of things moved to CBO to support future functionality
// 1.2		Abaddon		August 27th, 2000
//			Addition of basic script trigger stuff
//			Function documentation finished for all functions

//o--------------------------------------------------------------------------
//|	Function		-	cBaseObject destructor
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	This function is does basically what the name implies
//o--------------------------------------------------------------------------
cBaseObject::~cBaseObject()
{

}

//o--------------------------------------------------------------------------
//|	Function		-	cBaseObject constructor
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	This function is does basically what the name implies
//o--------------------------------------------------------------------------
cBaseObject::cBaseObject( void ) : objType( OT_CBO ), race( 0 ), x( 100 ), y( 100 ), z( 0 ), id( 1 ), colour( 0 ),
dir( 0 ), serial( 0xFFFFFFFF ), multis( NULL ), spawnserial( NULL ), owner( NULL ), iCounter( 0 ),
strength( 1 ), dexterity( 1 ), intelligence( 1 ), hitpoints( 1 ), visible( 0 ), disabled( false ), def( 0 ), hidamage( 0 ),
lodamage( 0 ), kills( 0 ), karma( 0 ), fame( 0 ), mana( 1 ), stamina( 1 ), scriptTrig( 0xFFFF ), st2( 0 ), dx2( 0 ), in2( 0 )
{
	name[0] = 0x00;
	title[0] = 0;
	genericDWords[0] = genericDWords[1] = genericDWords[2] = genericDWords[3] = 0;
}

//o--------------------------------------------------------------------------
//|	Function		-	short GetX()
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns X location of object
//o--------------------------------------------------------------------------
SI16 cBaseObject::GetX( void ) const
{
	return x;
}

//o--------------------------------------------------------------------------
//|	Function		-	short GetY()
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns Y location of object
//o--------------------------------------------------------------------------
SI16 cBaseObject::GetY( void ) const
{
	return y;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI08 GetZ()
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns Z location of object
//o--------------------------------------------------------------------------
SI08 cBaseObject::GetZ( void ) const
{
	return z;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetX( short newValue )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the new X value
//o--------------------------------------------------------------------------
void cBaseObject::SetX( SI16 newValue )
{
	x = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetY( short newValue )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the new Y value
//o--------------------------------------------------------------------------
void cBaseObject::SetY( SI16 newValue )
{
	y = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetZ( SI08 newValue )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the new Z value
//o--------------------------------------------------------------------------
void cBaseObject::SetZ( SI08 newValue )
{
	z = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetLocation( short newX, short newY, SI08 newZ )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Moves to new location specified
//o--------------------------------------------------------------------------
void cBaseObject::SetLocation( SI16 newX, SI16 newY, SI08 newZ )
{
	x = newX;
	y = newY;
	z = newZ;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI16 GetID()
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the ID of the object
//o--------------------------------------------------------------------------
UI16 cBaseObject::GetID( void ) const
{
	return id;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI16 GetColour()
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the colour of the object
//o--------------------------------------------------------------------------
UI16 cBaseObject::GetColour( void ) const
{
	return colour;
}

//o--------------------------------------------------------------------------
//|	Function		-	UCHAR GetID( UCHAR part )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns part of the ID
//o--------------------------------------------------------------------------
UCHAR cBaseObject::GetID( UCHAR part ) const
{
	switch( part )
	{
	default:
	case 1:	return (UCHAR)(id>>8);
	case 2:	return (UCHAR)(id%256);
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	UCHAR GetColour( UCHAR part )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns part of the colour
//o--------------------------------------------------------------------------
UCHAR cBaseObject::GetColour( UCHAR part ) const
{
	switch( part )
	{
	default:
	case 1:	return (UCHAR)(colour>>8);
	case 2:	return (UCHAR)(colour%256);
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	SetID( UI16 newValue )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the ID
//o--------------------------------------------------------------------------
void cBaseObject::SetID( UI16 newValue )
{
	id = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetColour( UI16 newValue )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the colour to newValue
//o--------------------------------------------------------------------------
void cBaseObject::SetColour( UI16 newValue )
{
	colour = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetID( UCHAR newValue, UCHAR part )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets part of the ID
//o--------------------------------------------------------------------------
void cBaseObject::SetID( UCHAR newValue, UCHAR part )
{
	if( part > 2 )
		printf( "Warning, warning Will Robinson!\n");
	UCHAR parts[2];
	parts[0] = id>>8;
	parts[1] = id%256;
	parts[part-1] = newValue;
	id = (parts[0]<<8) + parts[1];
}

//o--------------------------------------------------------------------------
//|	Function		-	SetColour( UCHAR newValue, UCHAR part )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets part of the colour
//o--------------------------------------------------------------------------
void cBaseObject::SetColour( UCHAR newValue, UCHAR part )
{
	UCHAR parts[2];
	parts[0] = colour>>8;
	parts[1] = colour%256;
	parts[part-1] = newValue;
	colour = (parts[0]<<8) + parts[1];
}

//o--------------------------------------------------------------------------
//|	Function		-	SERIAL GetMulti()
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns SERIAL of multi object we're within
//o--------------------------------------------------------------------------
SERIAL cBaseObject::GetMulti( void ) const
{
	if( multis == NULL )
		return 0xFFFFFFFF;
	return multis->GetSerial();
}

//o--------------------------------------------------------------------------
//|	Function		-	cBaseObject *GetMultiObj()
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns multi object we're within
//o--------------------------------------------------------------------------
cBaseObject *cBaseObject::GetMultiObj( void ) const
{
	return multis;
}

//o--------------------------------------------------------------------------
//|	Function		-	SERIAL GetSerial()
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns serial of the item
//o--------------------------------------------------------------------------
SERIAL cBaseObject::GetSerial( void ) const
{
	return serial;
}

//o--------------------------------------------------------------------------
//|	Function		-	SERIAL GetSpawn()
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns SERIAL of thing that spawned it
//o--------------------------------------------------------------------------
SERIAL cBaseObject::GetSpawn( void ) const
{
	if( spawnserial == NULL )
		return 0;
	return spawnserial->GetSerial();
}

//o--------------------------------------------------------------------------
//|	Function		-	cBaseObject *GetSpawnObj()
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns thing that spawned us
//o--------------------------------------------------------------------------
cBaseObject *cBaseObject::GetSpawnObj( void ) const
{
	return spawnserial;
}

//o--------------------------------------------------------------------------
//|	Function		-	SERIAL GetOwner()
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns SERIAL of thing that owns us
//o--------------------------------------------------------------------------
SERIAL cBaseObject::GetOwner( void ) const
{
	if( owner == NULL )
		return 0xFFFFFFFF;
	return owner->GetSerial();
}

//o--------------------------------------------------------------------------
//|	Function		-	cBaseObject *GetOwnerObj()
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns thing that owns us
//o--------------------------------------------------------------------------
cBaseObject *cBaseObject::GetOwnerObj( void ) const
{
	return owner;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetMulti( SERIAL newSerial )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets serial of item we're within
//o--------------------------------------------------------------------------
void cBaseObject::SetMulti( SERIAL newSerial )
{
	CMultiObj *mulObj = NULL;
	if( multis != NULL )
	{
		if( multis->GetObjType() == OT_MULTI )
		{
			mulObj = static_cast<CMultiObj *>(multis);
			mulObj->RemoveFromMulti( this );
		}
	}
	if( (newSerial>>24) >= 0x40 )
		multis = (cBaseObject *)calcItemObjFromSer( newSerial );
	else
		throw "Shit, bad multi";
	if( multis != NULL )
	{
		if( multis->GetObjType() == OT_MULTI )
		{
			mulObj = static_cast<CMultiObj *>(multis);
			mulObj->AddToMulti( this );
		}
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	SetSerial( SERIAL newSerial )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets serial of item we are
//o--------------------------------------------------------------------------
void cBaseObject::SetSerial( SERIAL newSerial )
{
	serial = newSerial;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetSpawn( SERIAL newSerial )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets serial of item that spawned us
//o--------------------------------------------------------------------------
void cBaseObject::SetSpawn( SERIAL newSerial )
{
	if( (newSerial>>24) >= 0x40 )
		spawnserial = (cBaseObject *)calcItemObjFromSer( newSerial );
	else
		spawnserial = (cBaseObject *)calcCharObjFromSer( newSerial );
}

//o--------------------------------------------------------------------------
//|	Function		-	SetMulti( UCHAR newSerial, UCHAR newPart )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets part of the serial of the multi we're in
//o--------------------------------------------------------------------------
void cBaseObject::SetMulti( UCHAR newSerial, UCHAR newPart )
{
	UCHAR parts[4];
	CMultiObj *mulObj = NULL;
	if( multis == NULL )
		memset( parts, 0, sizeof( parts[0] ) * 4 );
	else
	{
		mulObj = static_cast<CMultiObj *>(multis);
		mulObj->RemoveFromMulti( this );
		for( int j = 0; j < 4; j++ )
			parts[j] = multis->GetSerial( j );
	}
	parts[newPart-1] = newSerial;
	if( parts[0] >= 0x40 )
		multis = (cBaseObject *)calcItemObjFromSer( calcserial( parts[0], parts[1], parts[2], parts[3] ) );
	else
		multis = (cBaseObject *)calcCharObjFromSer( calcserial( parts[0], parts[1], parts[2], parts[3] ) );
	if( multis != NULL )
	{
		mulObj = static_cast<CMultiObj *>(multis);
		mulObj->AddToMulti( this );
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	SetSerial( UCHAR newSerial, UCHAR newPart )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets part of our serial
//o--------------------------------------------------------------------------
void cBaseObject::SetSerial( UCHAR newSerial, UCHAR newPart )
{
	UCHAR parts[4];
	parts[0] = (UCHAR)(serial>>24);
	parts[1] = (UCHAR)(serial>>16);
	parts[2] = (UCHAR)(serial>>8);
	parts[3] = (UCHAR)(serial%256);
	parts[newPart-1] = newSerial;
	serial = (parts[0]<<24) + (parts[1]<<16) + (parts[2]<<8) + parts[3];
}

//o--------------------------------------------------------------------------
//|	Function		-	SetSpawn( UCHAR newSerial, UCHAR newPart )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets part of the spawner serial
//o--------------------------------------------------------------------------
void cBaseObject::SetSpawn(  UCHAR newSerial, UCHAR newPart )
{
	UCHAR parts[4];
	if( spawnserial == NULL )
		memset( parts, 0, sizeof( parts[0] ) * 4 );
	else
	{
		for( int j = 0; j < 4l; j++ )
			parts[j] = spawnserial->GetSerial( j );
	}
	parts[newPart-1] = newSerial;
	if( parts[0] >= 0x40 )
		spawnserial = (cBaseObject *)calcItemObjFromSer( calcserial( parts[0], parts[1], parts[2], parts[3] ) );
	else
		spawnserial = (cBaseObject *)calcCharObjFromSer( calcserial( parts[0], parts[1], parts[2], parts[3] ) );
}

//o--------------------------------------------------------------------------
//|	Function		-	DumpHeader( ofstream &outStream, int mode ) const
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Dumps out header information for world saving
//o--------------------------------------------------------------------------
bool cBaseObject::DumpHeader( ofstream &outStream, int mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
//		LONG zFilePosition = outStream.tellp();
//		outStream << "[" << id << "]" << name << "[" << zFilePosition << "] - BaseObject Dump" << endl;
		outStream << "[BASEOBJECT]" << endl;
		break;
	}
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetSpawn( UCHAR newSerial, UCHAR newPart )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Dumps out body information of the object
//|						This is tag/data pairing information
//o--------------------------------------------------------------------------
bool cBaseObject::DumpBody( ofstream &outStream, int mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
		outStream << "Title=" << title << endl;
		outStream << "Kills=" << kills << endl;
		outStream << "Fame=" << fame << endl;
		outStream << "Karma=" << karma << endl;
		outStream << "Mana=" << mana << endl;
		outStream << "Stamina=" << stamina << endl;
		outStream << "ObjectType=" << (int)objType << endl;
		outStream << "XYZ=" << x << "," << y << "," << (short)z << endl;
		outStream << "ID=" << id << endl;
		outStream << "Colour=" << colour << endl;
		outStream << "Direction=" << (short)dir << endl;
		outStream << "Serial=" << serial << endl;
		//====================================================================================
		// Ab, here is another problem, tell me if this is a suitble way of doing this for you
		if( multis != NULL )
			outStream << "MultiID=" << multis->GetSerial() << endl;
		if( spawnserial != NULL )
			outStream << "SpawnerID=" << spawnserial->GetSerial() << endl;
		if( owner != NULL )
			outStream << "OwnerID=" << owner->GetSerial() << endl;
		//====================================================================================
		outStream << "Race=" << race << endl;
		outStream << "Name=" << name << endl;
		outStream << "Strength=" << strength << endl;
		outStream << "Dexterity=" << dexterity << endl;
		outStream << "Intelligence=" << intelligence << endl;
		outStream << "HitPoints=" << hitpoints << endl;
		outStream << "Strength2=" << st2 << endl;
		outStream << "Dexterity2=" << dx2 << endl;
		outStream << "Intelligence2=" << in2 << endl;
		outStream << "Visible=" << (short)visible << endl;
		outStream << "Disabled=" << (disabled?"1":"0") << endl;
		outStream << "HiDamage=" << hidamage << endl;
		outStream << "LoDamage=" << lodamage << endl;
		outStream << "Defense=" << def << endl;
		outStream << "ScpTrig=" << scriptTrig << endl;
		outStream << "DWord0=" << genericDWords[0] << endl;
		outStream << "DWord1=" << genericDWords[1] << endl;
		outStream << "DWord2=" << genericDWords[2] << endl;
		outStream << "DWord3=" << genericDWords[3] << endl;
		// Ab, does this need to be written to the file? I am going to make it write it but comment or remove it if its not(remember to remove it from the Load routine as well)
		outStream << "iCounter=" << iCounter << endl;
		//====================================================================================
		break;
	}
	return true;
}
//o--------------------------------------------------------------------------
//|	Function		-	bool Save( ofstream &outStream, int mode ) const
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Saves us out to disk based on mode
//|						0	Text
//|						1	Binary
//o--------------------------------------------------------------------------
bool cBaseObject::Save( ofstream &outStream, int mode ) const
{
	DumpHeader( outStream, mode );
	DumpBody( outStream, mode );
	DumpFooter( outStream, mode );
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	cBaseObject *First()
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns first object that it contains
//o--------------------------------------------------------------------------
cBaseObject *cBaseObject::First( void )
{
	return NULL;
}

//o--------------------------------------------------------------------------
//|	Function		-	cBaseObject *Next()
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns next object in the list
//o--------------------------------------------------------------------------
cBaseObject	*cBaseObject::Next( void )
{
	return NULL;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool AtEnd()
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if at end of the object list
//o--------------------------------------------------------------------------
bool cBaseObject::AtEnd( void )
{
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	RACEID GetRace()
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the associated race of the object
//o--------------------------------------------------------------------------
RACEID cBaseObject::GetRace( void ) const
{
	return race;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetRace( RACEID newRace )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the objects Race property
//o--------------------------------------------------------------------------
void cBaseObject::SetRace( RACEID newValue )
{
	race = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	const char *GetName()
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the name of the object
//o--------------------------------------------------------------------------
const char *cBaseObject::GetName( void ) const
{
	return name;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetName( const char *newName )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the name of the object
//o--------------------------------------------------------------------------
void cBaseObject::SetName( const char *newName )
{
	strncpy( name, newName, MAX_NAME );
}

//o--------------------------------------------------------------------------
//|	Function		-	SI16 GetStrength()
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the strength of the object
//o--------------------------------------------------------------------------
SI16 cBaseObject::GetStrength( void ) const
{
	return strength;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI16 GetDexterity()
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the dexterity of the object
//o--------------------------------------------------------------------------
SI16 cBaseObject::GetDexterity( void ) const
{
	return dexterity;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI16 GetIntelligence()
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the intelligence of the object
//o--------------------------------------------------------------------------
SI16 cBaseObject::GetIntelligence( void ) const
{
	return intelligence;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI16 GetHP()
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the hitpoints of the object
//o--------------------------------------------------------------------------
SI16 cBaseObject::GetHP( void ) const
{
	return hitpoints;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetStrength( SI16 newValue )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the strength of the obhect
//o--------------------------------------------------------------------------
void cBaseObject::SetStrength( SI16 newValue )
{
	strength = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetDexterity( SI16 newValue )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the dexterity of the object
//o--------------------------------------------------------------------------
void cBaseObject::SetDexterity( SI16 newValue )
{
	dexterity = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetIntelligence( SI16 newValue )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the intelligence of the object
//o--------------------------------------------------------------------------
void cBaseObject::SetIntelligence( SI16 newValue )
{
	intelligence = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetHP( SI16 newValue )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the hitpoints of the object
//o--------------------------------------------------------------------------
void cBaseObject::SetHP( SI16 newValue )
{
	hitpoints = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	IncHP( SI16 amtToChange )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Increments the hitpoints of the object
//o--------------------------------------------------------------------------
void cBaseObject::IncHP( SI16 amtToChange )
{
	hitpoints += amtToChange;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetDir( UCHAR newDir )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the direction of the object
//o--------------------------------------------------------------------------
void cBaseObject::SetDir( UCHAR newDir )
{
	dir = newDir;
}

//o--------------------------------------------------------------------------
//|	Function		-	UCHAR GetDir()
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the direction of the object
//o--------------------------------------------------------------------------
UCHAR cBaseObject::GetDir( void ) const
{
	return dir;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetVisible( char newValue )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the visibility property of the object
//|						Generally it is
//|						0	Visible
//|						1	GM only/hidden
//|						2	Owner||GM / Magically invisible
//o--------------------------------------------------------------------------
void cBaseObject::SetVisible( SI08 newValue )
{
	visible = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	char GetVisible()
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the visibility property of the object
//o--------------------------------------------------------------------------
SI08 cBaseObject::GetVisible( void ) const
{
	return visible;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool IsVisible()
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the object is visible to all players
//o--------------------------------------------------------------------------
bool cBaseObject::IsVisible( void ) const
{
	return visible == 0;
}

//o--------------------------------------------------------------------------
//|	Function		-	string GetObjType()
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns a string that indicates the item's type
//o--------------------------------------------------------------------------
ObjectType cBaseObject::GetObjType( void ) const
{
	return objType;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool IsDisabled()
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the object is disabled
//o--------------------------------------------------------------------------
bool cBaseObject::IsDisabled( void ) const
{
	return disabled;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetDisabled( bool newValue )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the disabled value of the object
//o--------------------------------------------------------------------------
void cBaseObject::SetDisabled( bool newValue )
{
	disabled = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetMulti( cBaseObject *newMulti )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's multi to newMulti
//o--------------------------------------------------------------------------
void cBaseObject::SetMulti( cBaseObject *newMulti )
{
	CMultiObj *mulObj;
	if( multis != NULL )
	{
		mulObj = static_cast<CMultiObj *>(multis);
		mulObj->RemoveFromMulti( this );
	}
	multis = newMulti;
	if( multis != NULL )
	{
		mulObj = static_cast<CMultiObj *>(multis);
		mulObj->AddToMulti( this );
	}

}

//o--------------------------------------------------------------------------
//|	Function		-	SetSpawn( cBaseObject *newSpawn )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's spawner to newSpawn
//o--------------------------------------------------------------------------
void cBaseObject::SetSpawn( cBaseObject *newSpawn )
{
	spawnserial = newSpawn;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetOwner( cBaseObject *newOwner )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's owner to newOwner
//o--------------------------------------------------------------------------
void cBaseObject::SetOwner( cBaseObject *newOwner )
{
	owner = newOwner;
}

//o--------------------------------------------------------------------------
//|	Function		-	UCHAR GetSerial( UCHAR part )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns part of a serial #
//o--------------------------------------------------------------------------
UCHAR cBaseObject::GetSerial( UCHAR part ) const
{
	switch( part )
	{
	case 1:	return (UCHAR)(serial>>24);
	case 2:	return (UCHAR)(serial>>16);
	case 3: return (UCHAR)(serial>>8);
	case 4: return (UCHAR)(serial%256);
	}
	return 0;
}

//o--------------------------------------------------------------------------
//|	Function		-	UCHAR GetMulti( UCHAR part )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns part of the multi's serial
//o--------------------------------------------------------------------------
UCHAR cBaseObject::GetMulti(  UCHAR part ) const
{
	if( multis == NULL )
		return 0xFF;
	return multis->GetSerial( part );
}

//o--------------------------------------------------------------------------
//|	Function		-	UCHAR GetSpawn( UCHAR part )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns part of the item's spawner serial
//o--------------------------------------------------------------------------
UCHAR cBaseObject::GetSpawn(  UCHAR part ) const
{
	if( spawnserial == NULL )
		return 0x0;
	return spawnserial->GetSerial( part );
}

//o--------------------------------------------------------------------------
//|	Function		-	UCHAR GetOwner( UCHAR part )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns part of the item's owner's serial
//o--------------------------------------------------------------------------
UCHAR cBaseObject::GetOwner(  UCHAR part ) const
{
	if( owner == NULL )
		return 0;
	return owner->GetSerial( part );
}

//o--------------------------------------------------------------------------
//|	Function		-	SetOwner( UCHAR newSerial, UCHAR newPart )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets part of the owner's serial
//o--------------------------------------------------------------------------
void cBaseObject::SetOwner( UCHAR newSerial, UCHAR newPart )
{
	UCHAR parts[4];
	if( owner == NULL )
		memset( parts, 0, sizeof( parts[0] ) * 4 );
	else
	{
		for( UCHAR j = 0; j < 4; j++ )
			parts[j] = owner->GetSerial( j );
	}
	parts[newPart-1] = newSerial;
	if( parts[0] >= 0x40 )
		owner = (cBaseObject *)calcItemObjFromSer( calcserial( parts[0], parts[1], parts[2], parts[3] ) );
	else
		owner = (cBaseObject *)calcCharObjFromSer( calcserial( parts[0], parts[1], parts[2], parts[3] ) );
}

//o--------------------------------------------------------------------------
//|	Function		-	SetOwner( SERIAL newSerial )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's serial to newSerial
//o--------------------------------------------------------------------------
void cBaseObject::SetOwner( SERIAL newSerial )
{
	if( (newSerial>>24) >= 0x40 )
		owner = (cBaseObject *)calcItemObjFromSer( newSerial );
	else
		owner = (cBaseObject *)calcCharObjFromSer( newSerial );
}

//o--------------------------------------------------------------------------
//|	Function		-	short GetHiDamage( void )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the high amount of damage that it can do
//o--------------------------------------------------------------------------
SI16 cBaseObject::GetHiDamage( void ) const
{
	return hidamage;
}

//o--------------------------------------------------------------------------
//|	Function		-	short GetLoDamage( void )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the low amount of damage that it can do
//o--------------------------------------------------------------------------
SI16 cBaseObject::GetLoDamage( void ) const
{
	return lodamage;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI16 GetDef( void )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's multi to newMulti
//o--------------------------------------------------------------------------
UI16 cBaseObject::GetDef( void ) const
{
	return def;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetHiDamage( short newValue )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's high damage value
//o--------------------------------------------------------------------------
void cBaseObject::SetHiDamage( SI16 newValue )
{
	hidamage = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetLoDamage( short newValue )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's multi to newMulti
//o--------------------------------------------------------------------------
void cBaseObject::SetLoDamage( SI16 newValue )
{
	lodamage = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetDef( UI16 newValue )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's defense value
//o--------------------------------------------------------------------------
void cBaseObject::SetDef( UI16 newValue )
{
	def = newValue;
}


//o--------------------------------------------------------------------------
//|	Function		-	LONG SetFilePosition( LONG filepos )
//|	Date			-	Unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's file position
//o--------------------------------------------------------------------------
LONG cBaseObject::SetFilePosition( LONG filepos )
{
	FilePosition = filepos;
	return FilePosition;
}

//o--------------------------------------------------------------------------
//|	Function		-	LONG GetFilePosition( void )
//|	Date			-	Unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the object's file position
//o--------------------------------------------------------------------------
LONG cBaseObject::GetFilePosition( void ) const
{
	return FilePosition;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI16 GetStamina( void )
//|	Date			-	unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the object's stamina
//o--------------------------------------------------------------------------
SI16 cBaseObject::GetStamina( void ) const
{
	return stamina;
}

//o--------------------------------------------------------------------------
//|	Function		-	GetStamina( SI16 stam )
//|	Date			-	unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's stamina
//o--------------------------------------------------------------------------
void cBaseObject::SetStamina( SI16 stam )
{
	stamina = stam;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI16 GetMana( void )
//|	Date			-	unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the object's mana
//o--------------------------------------------------------------------------
SI16 cBaseObject::GetMana( void ) const 
{
	return mana;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetMana( SI16 mn )
//|	Date			-	unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's mana
//o--------------------------------------------------------------------------
void cBaseObject::SetMana( SI16 mn )
{
	mana = mn;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI16 GetKarma( void )
//|	Date			-	unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the object's karma
//o--------------------------------------------------------------------------
SI16 cBaseObject::GetKarma( void ) const
{
	return karma;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetKarma( SI16 value )
//|	Date			-	unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's karma
//o--------------------------------------------------------------------------
void cBaseObject::SetKarma( SI16 value )
{
	karma = value;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI16 GetFame( void )
//|	Date			-	unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the object's fame
//o--------------------------------------------------------------------------
SI16 cBaseObject::GetFame( void ) const
{
	return fame;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetFame( SI16 value )
//|	Date			-	unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's fame
//o--------------------------------------------------------------------------
void cBaseObject::SetFame( SI16 value )
{
	fame = value;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI16 GetKills( void )
//|	Date			-	unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the object's kills
//o--------------------------------------------------------------------------
SI16 cBaseObject::GetKills( void ) const
{
	return kills;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetKills( SI16 value )
//|	Date			-	unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's kills
//o--------------------------------------------------------------------------
void cBaseObject::SetKills( SI16 value )
{
	kills = value;
}

//o--------------------------------------------------------------------------
//|	Function		-	const char *GetTitle( void )
//|	Date			-	unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the object's title
//o--------------------------------------------------------------------------
const char * cBaseObject::GetTitle( void ) const
{
	return title;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetTitle( const char *newtitle )
//|	Date			-	unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's title
//o--------------------------------------------------------------------------
void cBaseObject::SetTitle( const char *newtitle )
{
	strncpy( title, newtitle, MAX_TITLE );
}

//o--------------------------------------------------------------------------
//|	Function		-	UI16 GetScriptTrigger( void )
//|	Date			-	August 27th, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the object's script trigger value
//o--------------------------------------------------------------------------
UI16 cBaseObject::GetScriptTrigger( void ) const
{
	return scriptTrig;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetScriptTrigger( UI16 newValue )
//|	Date			-	August 27th, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's script trigger value
//o--------------------------------------------------------------------------
void cBaseObject::SetScriptTrigger( UI16 newValue )
{
	scriptTrig = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	point3 GetLocation( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns a point3 structure pointing to the object's 
//|						current location
//o--------------------------------------------------------------------------
point3 cBaseObject::GetLocation( void ) const
{
	return point3( x, y, z );
}

//o--------------------------------------------------------------------------
//|	Function		-	void GetLocation( point3 &toSet )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the location of the current object to point3 var
//o--------------------------------------------------------------------------
void cBaseObject::SetLocation( point3 &toSet )
{
	SetLocation( toSet.x, toSet.y, toSet.z );
}


//o--------------------------------------------------------------------------
//|	Function		-	SI16 Strength2( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the second strength var associated with the 
//|						object.  For chars, it's the bonuses 
//|						(via armour and such)
//o--------------------------------------------------------------------------
SI16 cBaseObject::Strength2( void ) const
{
	return st2;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI16 Dexterity2( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the second dexterity var associated with the 
//|						object.  For chars, it's the bonuses 
//|						(via armour and such)
//o--------------------------------------------------------------------------
SI16 cBaseObject::Dexterity2( void ) const
{
	return dx2;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI16 Intelligence2( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the second intelligence var associated with
//|						the object.  For chars, it's the bonuses 
//|						(via armour and such)
//o--------------------------------------------------------------------------
SI16 cBaseObject::Intelligence2( void ) const
{
	return in2;
}

//o--------------------------------------------------------------------------
//|	Function		-	void Strength2( SI16 nVal )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the second strength var associated with the
//|						object.  For chars, it's the bonuses 
//|						(via armour and such)
//o--------------------------------------------------------------------------
void cBaseObject::Strength2( SI16 nVal )
{
	st2 = nVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	void Dexterity2( SI16 nVal )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the second dexterity var associated with the 
//|						object.  For chars, it's the bonuses 
//|						(via armour and such)
//o--------------------------------------------------------------------------
void cBaseObject::Dexterity2( SI16 nVal )
{
	dx2 = nVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	void Intelligence2( SI16 nVal )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the second intelligence var associated with
//|						the object.  For chars, it's the bonuses 
//|						(via armour and such)
//o--------------------------------------------------------------------------
void cBaseObject::Intelligence2( SI16 nVal )
{
	in2 = nVal;
}

//o--------------------------------------------------------------------------
//|	Function		-	void IncStrength( SI16 toInc = 1 )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Increments the object's strength value
//o--------------------------------------------------------------------------
void cBaseObject::IncStrength( SI16 toInc )
{
	SetStrength( GetStrength() + toInc );
}

//o--------------------------------------------------------------------------
//|	Function		-	void DecStrength( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Decrements the object's strength value by 1
//o--------------------------------------------------------------------------
void cBaseObject::DecStrength( void )
{
	SetStrength( GetStrength() - 1 );
}

//o--------------------------------------------------------------------------
//|	Function		-	void IncDexterity( SI16 toInc = 1 )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Increments the object's dexterity value
//o--------------------------------------------------------------------------
void cBaseObject::IncDexterity( SI16 toInc )
{
	SetDexterity( GetDexterity() + toInc );
}

//o--------------------------------------------------------------------------
//|	Function		-	void DecDexterity( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Decrements the object's dexterity value
//o--------------------------------------------------------------------------
void cBaseObject::DecDexterity( void )
{
	SetDexterity( GetDexterity() - 1 );
}

//o--------------------------------------------------------------------------
//|	Function		-	void IncIntelligence( SI16 toInc = 1 )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Increments the object's intelligence value
//o--------------------------------------------------------------------------
void cBaseObject::IncIntelligence( SI16 toInc )
{
	SetIntelligence( GetIntelligence() + toInc );
}

//o--------------------------------------------------------------------------
//|	Function		-	void IncIntelligence( SI16 toInc = 1 )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Decrements the object's intelligence value
//o--------------------------------------------------------------------------
void cBaseObject::DecIntelligence( void )
{
	SetIntelligence( GetIntelligence() - 1 );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool DumpFooter( ofstream &outStream, int mode )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Dumps out footer information so that a logical break
//|						between entries can be found without moving file 
//|						pointers
//|						Mode 0 - Text
//|						Mode 1 - Binary
//o--------------------------------------------------------------------------
bool cBaseObject::DumpFooter( ofstream &outStream, int mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
		outStream << endl << endl << "o---o" << endl;
	}
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool Load( :ifstream &inStream, int mode)
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Loads object from disk based on mode
//|						0	Text
//|						1	Binary
//o--------------------------------------------------------------------------
bool cBaseObject::Load( ifstream &inStream, int mode, int arrayOffset )
{
	switch( mode )
	{
	case 1:	// Binary load
		break;
	case 0:	// Text load
	default:
		char tag[128], data[512];
		bool bFinished;
		bFinished = false;
		while( !bFinished )
		{
			ReadWorldTagData( inStream, tag, data );
			bFinished = ( strcmp( tag, "o---o" ) == 0 );
			if( !bFinished )
			{
				if( !HandleLine( tag, data ) )
					printf( "ERROR: Unknown world file tag %s with contents of %s\n", tag, data );
			}
		}
		break;
	}
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool HandleLine( char *tag, char *data )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Used to handle world lines.  Returns true if the tag
//|						is known.  If known, internal information updated
//|						and load routine continues to next tag.  Otherwise,
//|						passed up inheritance tree (if any)
//o--------------------------------------------------------------------------
bool cBaseObject::HandleLine( char *tag, char *data )
{
	if( !strcmp( tag, "o---o" ) )
		return true;
	switch( tag[0] )
	{
	case 'C':
		if( !strcmp( tag, "Colour" ) )
		{
			colour = atoi( data );
			return true;
		}
		break;
	case 'D':
		if( !strcmp( tag, "Direction" ) )
		{
			dir = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "Dexterity" ) )
		{
			dexterity = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "Dexterity2" ) )
		{
			dx2 = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "Defense" ) )
		{
			def = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "DWord0" ) )
		{
			genericDWords[0] = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "DWord1" ) )
		{
			genericDWords[1] = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "DWord2" ) )
		{
			genericDWords[2] = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "DWord3" ) )
		{
			genericDWords[3] = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "Disabled" ) )
		{
			SetDisabled( atoi( data ) == 1 );
			return true;
		}
		break;
	case 'F':
		if( !strcmp( tag, "Fame" ) )
		{
			fame = atoi( data );
			return true;
		}
		break;
	case 'H':
		if( !strcmp( tag, "HitPoints" ) )
		{
			hitpoints = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "HiDamage" ) )
		{
			hidamage = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "HiDamage" ) )
		{
			hidamage = atoi( data );
			return true;
		}
		break;
	case 'I':
		if( !strcmp( tag, "ID" ) )
		{
			id = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "Intelligence" ) )
		{
			intelligence = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "Intelligence2" ) )
		{
			in2 = atoi( data );
			return true;
		}
		break;
	case 'i':
		if( !strcmp( tag, "iCounter" ) )
		{
			return true;	// don't process anything about it
		}
		break;
	case 'K':
		if( !strcmp( tag, "Kills" ) )
		{
			kills = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "Karma" ) )
		{
			karma = atoi( data );
			return true;
		}
		break;
	case 'L':
		if( !strcmp( tag, "LoDamage" ) )
		{
			lodamage = atoi( data );
			return true;
		}
		break;
	case 'M':
		if( !strcmp( tag, "Mana" ) )
		{
			mana = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "MultiID" ) )
		{
			multis = (cBaseObject *)atoi( data );
			return true;
		}
		break;
	case 'N':
		if( !strcmp( tag, "Name" ) )
		{
			strcpy( name, data );
			return true;
		}
		break;
	case 'O':
		if( !strcmp( tag, "ObjectType" ) )
		{
			objType = (ObjectType)atoi( data );
			return true;
		}
		else if( !strcmp( tag, "OwnerID" ) )
		{
			owner = (cBaseObject *)atoi( data );
			return true;
		}
		break;
	case 'R':
		if( !strcmp( tag, "Race" ) )
		{
			race = atoi( data );
			return true;
		}
		break;
	case 'S':
		if( !strcmp( tag, "Stamina" ) )
		{
			stamina = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "SpawnerID" ) )
		{
			spawnserial = (cBaseObject *)atoi( data );
			return true;
		}
		else if( !strcmp( tag, "Serial" ) )
		{
			serial = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "Strength" ) )
		{
			strength = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "Strength2" ) )
		{
			st2 = atoi( data );
			return true;
		}
		else if( !strcmp( tag, "ScpTrig" ) )
		{
			scriptTrig = atoi( data );
			return true;
		}
		break;
	case 'T':
		if( !strcmp( tag, "Title" ) )
		{
			strcpy( title, data );
			return true;
		}
		break;
	case 'V':
		if( !strcmp( tag, "Visible" ) )
		{
			visible = atoi( data );
			return true;
		}
		break;
	case 'X':
		if( !strcmp( tag, "XYZ" ) )
		{
			char *yOff = strstr( data, "," );
			char *zOff = strstr( yOff+1, "," );
			char tmp[32];
			strncpy( tmp, data, yOff - data );
			tmp[yOff - data] = 0;
			x = atoi( tmp );
			strncpy( tmp, yOff + 1, zOff - yOff - 1 );
			tmp[zOff - yOff - 1] = 0;
			y = atoi( tmp );
			z = atoi( zOff + 1 );
			return true;
		}
		break;
	}
	return false;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 GetWord( UI08 wordNum )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns one of the generic words of the object
//|						There are 4 words.  wordNum must be between 0 and 3
//o--------------------------------------------------------------------------
UI32 cBaseObject::GetWord( UI08 wordNum )
{
	if( wordNum > 3 )
		return 0;
	return genericDWords[wordNum];
}

//o--------------------------------------------------------------------------
//|	Function		-	bool GetBit( UI08 wordNum, UI08 bitNum )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the bit bitNum in word wordNum is on
//o--------------------------------------------------------------------------
bool cBaseObject::GetBit( UI08 wordNum, UI08 bitNum )
{
	if( wordNum > 3 )
		return false;
	if( bitNum > 31 )
		return false;
	UI32 mask = power( 2, bitNum );
	return ( ( genericDWords[wordNum] & mask ) == mask );
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 GetBitRange( UI08 wordNUm, UI08 lowBit, UI08 highBit )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the value of the bit range from lowBit to highBit
//|						in word wordNum.  Note that if lowBit is NOT 0, it's
//|						right shifted by lowBit bits.
//o--------------------------------------------------------------------------
UI32 cBaseObject::GetBitRange( UI08 wordNum, UI08 lowBit, UI08 highBit )
{
	if( wordNum > 3 )
		return 0;
	if( lowBit > 31 )
		return 0;
	if( highBit > 31 )
		return 0;

	UI32 mask = 0;
	for( int i = lowBit; i <= highBit; i++ )
		mask |= power( 2, i );

	if( lowBit == 0 )
		return (genericDWords[wordNum]&mask);
	else
		return ( (genericDWords[wordNum]&mask)>>lowBit );
}

//o--------------------------------------------------------------------------
//|	Function		-	SetWord( UI08 wordNum, UI32 value )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets word wordNum to value.  wordNum must be 0->3
//o--------------------------------------------------------------------------
void cBaseObject::SetWord( UI08 wordNum, UI32 value )
{
	if( wordNum > 3 )
		return;
	genericDWords[wordNum] = value;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetBit( UI08 wordNum, UI08 bitNum, bool value )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Turns on (value==true) or off (value==false) bit bitNum
//|						in word wordNum
//o--------------------------------------------------------------------------
void cBaseObject::SetBit( UI08 wordNum, UI08 bitNum, bool value )
{
	if( wordNum > 3 )
		return;
	if( bitNum > 31 )
		return;
	UI32 mask = power( 2, bitNum );
	genericDWords[wordNum] |= mask;
}

//o--------------------------------------------------------------------------
//|	Function		-	SetBitRange( UI08 wordNum, UI08 lowBit, UI08 highBit, UI32 value )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the range of lowBit to highBit to value in word 
//|						wordNum.  Note it does no range checking to ensure
//|						that value will fit in that bit range
//o--------------------------------------------------------------------------
void cBaseObject::SetBitRange( UI08 wordNum, UI08 lowBit, UI08 highBit, UI32 value )
{
	if( wordNum > 3 )
		return;
	if( lowBit > 31 )
		return;
	if( highBit > 31 )
		return;

	UI32 mask = 0xFFFFFFFF;	// start with every bit set
	for( int i = lowBit; i <= highBit; i++ )
		mask -= power( 2, i );	// subtract off the bit

	genericDWords[wordNum] &= mask;	// reset the words

	if( lowBit == 0 )
		genericDWords[wordNum] += value;
	else
		genericDWords[wordNum] |= ( value<<lowBit );	// shift the information along lowBit # of bits, so it's in the right spot
}

//o--------------------------------------------------------------------------
//|	Function		-	PostLoadProcessing( SI32 index )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Used to setup any pointers that may need adjustment
//|						following the loading of the world
//o--------------------------------------------------------------------------
void cBaseObject::PostLoadProcessing( SI32 index )
{
	SERIAL tmpSerial = 0xFFFFFFFF;
	if( multis != NULL )
	{
		tmpSerial = (SERIAL)multis;
		multis = NULL;
		if( tmpSerial != 0xFFFFFFFF )
			SetMulti( tmpSerial );
	}
	if( spawnserial != NULL )
	{
		tmpSerial = (SERIAL)spawnserial;
		spawnserial = NULL;
		SetSpawn( (SERIAL)spawnserial );
	}
	if( owner != NULL )
	{
		tmpSerial = (SERIAL)owner;
		owner = NULL;
		if( tmpSerial = 0xFFFFFFFF )
			SetOwner( (SERIAL)owner );
	}
}
