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
	if( multis != NULL )
		RemoveFromMulti( false );
	// Delete all tags.
	for( TAG *tag = firstTag; tag != NULL; tag = firstTag ) 
	{
		firstTag = tag->nextTag;
		delete tag;
	}
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
dir( 0 ), serial( INVALIDSERIAL ), multis( NULL ), spawnserial( NULL ), owner( NULL ), iCounter( 0 ), worldNumber( 0 ),
strength( 1 ), dexterity( 1 ), intelligence( 1 ), hitpoints( 1 ), visible( 0 ), disabled( false ), def( 0 ), hidamage( 0 ),
lodamage( 0 ), kills( 0 ), karma( 0 ), fame( 0 ), mana( 1 ), stamina( 1 ), scriptTrig( 0xFFFF ), st2( 0 ), dx2( 0 ), in2( 0 ),
firstTag( NULL ), shouldSave( true ), isDirty( 1 ), FilePosition( -1 ), spawned( false )
{
	name[0] = 0x00;
	title[0] = 0;
	genericDWords[0] = genericDWords[1] = genericDWords[2] = genericDWords[3] = 0;
}


SI32 cBaseObject::GetNumTags( void ) const 
{
	SI32 a = 0;
	for( TAG *tag = firstTag; tag != NULL; tag = tag->nextTag )
	{
		a++;
	}
	return a;
}

jsval cBaseObject::GetTag( char* tagname ) const 
{
	for( TAG *tag = firstTag; tag != NULL; tag = tag->nextTag )
	{
		if( !strcmp( tagname, tag->name ) ) 
		{
			return tag->val;
		}
	}
	return (jsval)0;
}

void cBaseObject::SetTag( char* tagname, jsval tagval ) 
{
	TAG *tag = NULL;
	for( tag = firstTag; tag != NULL; tag = tag->nextTag ) 
	{
		if( !strcmp( tagname, tag->name ) ) 
		{
			if( tagval == (jsval)0 ) 
			{
				//Delete the tag if they set it to "".
				tag->prevTag->nextTag = tag->nextTag;
				tag->nextTag->prevTag = tag->prevTag;
				//delete tag->val;
				delete tag->name;
				delete tag;
			} 
			else 
			{
				//Change the tag's value.
				if( JSVAL_IS_STRING( tagval ) ) 
				{
					char *s_tagval = JS_GetStringBytes( JS_ValueToString( jsContext, tagval ) );
					JSString *s_string = JS_NewStringCopyZ( jsContext, s_tagval);
					tag->val = STRING_TO_JSVAL( s_string );
				} 
				else 
				{
					tag->val = tagval;
				}
				
			}
			return;
		}
	}
	//make new tag
	tag = new TAG;
	tag->prevTag = NULL;
	tag->nextTag = firstTag;
	tag->name = new char[strlen(tagname)+1];
	strcpy( tag->name, tagname );
	if( JSVAL_IS_STRING( tagval ) ) 
	{
		char *s_tagval = JS_GetStringBytes( JS_ValueToString( jsContext, tagval ) );
		JSString *s_string = JS_NewStringCopyZ( jsContext, s_tagval);
		tag->val = STRING_TO_JSVAL( s_string );
	} 
	else 
	{
		tag->val = tagval;
	}
	firstTag = tag;
	return;
}	
//o--------------------------------------------------------------------------
//|	Function		-	SI16 GetX()
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
//|	Function		-	SI16 GetY()
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
//|	Function		-	SetX( SI16 newValue )
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
//|	Function		-	SetY( SI16 newValue )
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
//|	Function		-	SetLocation( SI16 newX, SI16 newY, SI08 newZ )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Moves to new location specified
//o--------------------------------------------------------------------------
void cBaseObject::SetLocation( SI16 newX, SI16 newY, SI08 newZ )
{
	Console << "WARNING: Using cBaseObject::SetLocation for " << (UI32)this << myendl;
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
//|	Function		-	UI08 GetID( UI08 part )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns part of the ID
//o--------------------------------------------------------------------------
UI08 cBaseObject::GetID( UI08 part ) const
{
	switch( part )
	{
	default:
	case 1:	return (UI08)(id>>8);
	case 2:	return (UI08)(id%256);
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	UI08 GetColour( UI08 part )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns part of the colour
//o--------------------------------------------------------------------------
UI08 cBaseObject::GetColour( UI08 part ) const
{
	switch( part )
	{
	default:
	case 1:	return (UI08)(colour>>8);
	case 2:	return (UI08)(colour%256);
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
//|	Function		-	SetID( UI08 newValue, UI08 part )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets part of the ID
//o--------------------------------------------------------------------------
void cBaseObject::SetID( UI08 newValue, UI08 part )
{
	if( part > 2 )
		Console << "Warning, warning Will Robinson!" << myendl;
	UI08 parts[2];
	parts[0] = (UI08)(id>>8);
	parts[1] = (UI08)(id%256);
	parts[part-1] = newValue;
	id = (UI16)((parts[0]<<8) + parts[1]);
}

//o--------------------------------------------------------------------------
//|	Function		-	SetColour( UI08 newValue, UI08 part )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets part of the colour
//o--------------------------------------------------------------------------
void cBaseObject::SetColour( UI08 newValue, UI08 part )
{
	UI08 parts[2];
	parts[0] = (UI08)(colour>>8);
	parts[1] = (UI08)(colour%256);
	parts[part-1] = newValue;
	colour = (UI16)((parts[0]<<8) + parts[1]);
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
		return INVALIDSERIAL;
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
		return INVALIDSERIAL;
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
void cBaseObject::SetMulti( SERIAL newSerial, bool fireTrigger )
{
	RemoveFromMulti( fireTrigger );
	if( (newSerial>>24) >= 0x40 )
		multis = (cBaseObject *)calcItemObjFromSer( newSerial );
	else 
		return;
	AddToMulti( fireTrigger );
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
//|	Function		-	SetMulti( UI08 newSerial, UI08 newPart )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets part of the serial of the multi we're in
//o--------------------------------------------------------------------------
void cBaseObject::SetMulti( UI08 newSerial, UI08 newPart, bool fireTrigger )
{
	UI08 parts[4];
	if( multis != NULL )
	{
		for( UI08 j = 0; j < 4; j++ )
			parts[j] = multis->GetSerial( j );
	}
	RemoveFromMulti( fireTrigger );
	parts[newPart-1] = newSerial;
	if( parts[0] >= 0x40 )
		multis = (cBaseObject *)calcItemObjFromSer( calcserial( parts[0], parts[1], parts[2], parts[3] ) );
	else
		return;
	AddToMulti( fireTrigger );
}

//o--------------------------------------------------------------------------
//|	Function		-	SetSpawn( UI08 newSerial, UI08 newPart )
//|	Date			-	26 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets part of the spawner serial
//o--------------------------------------------------------------------------
void cBaseObject::SetSpawn(  UI08 newSerial, UI08 newPart )
{
	UI08 parts[4];
	if( spawnserial == NULL )
		memset( parts, 0, sizeof( parts[0] ) * 4 );
	else
	{
		for( UI08 j = 0; j < 4l; j++ )
			parts[j] = spawnserial->GetSerial( j );
	}
	parts[newPart-1] = newSerial;
	if( parts[0] >= 0x40 )
		spawnserial = (cBaseObject *)calcItemObjFromSer( calcserial( parts[0], parts[1], parts[2], parts[3] ) );
	else
		spawnserial = (cBaseObject *)calcCharObjFromSer( calcserial( parts[0], parts[1], parts[2], parts[3] ) );
}

//o--------------------------------------------------------------------------
//|	Function		-	void IsSpawned( bool setting )
//|	Date			-	15 March, 2002
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets if a obj is spawned, we dont need to save that for now
//o--------------------------------------------------------------------------
void cBaseObject::IsSpawned( bool setting )
{
	spawned = setting;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool IsSpawned( void )
//|	Date			-	15 March, 2002
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	returns if the obj is spawned, we dont need to save that for now
//o--------------------------------------------------------------------------
bool cBaseObject::IsSpawned( void )
{
	return spawned;
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
		outStream << "[BASEOBJECT]" << endl;
		break;
	}
	return true;
}

//o--------------------------------------------------------------------------
//|	Function	-	DumpBody( BinBuffer &buff ) const
//|	Date		-	1/15/02
//|	Programmer	-	Zippy
//|	Modified	-
//o--------------------------------------------------------------------------
//|	Purpose		-	Dumps out binary information of the object to the buffer
//o--------------------------------------------------------------------------
bool cBaseObject::DumpBody( BinBuffer &buff ) const
{
	if( isDirty == 0 )
	{
		return false;
	}
	if( FilePosition != -1 )
	{
		// We have a previous record (as ascertained by FilePosition), so we need to dirty that record
		// Need to handle dirty record processing here
		// First of all, we need to modify the previously written record to specify that it IS dirty
		// Second of all, we need to write our new record and specify it is clean
		
		// My thoughts (DarkStorm)
		// There are several things we need to keep in mind
		// in order to get a good dirtyflagging system:
		// a) How do we make "old" records dirty 
		//	  - Flag them
		//	  - Overwrite them with 0x00
		//	  
		//	  I have to agree that flagging them is easier
		//	  because it saves  alot of I/O time
		//	  So in fact thats clear
		//	  We just need to add new block identifier bytes
		//    i.e.  0xA1 is an inactive Char
		//			0xA2 is an inactive Item
		//			[...]

		//		<Abaddon>
		//		You could do that, or you could just (considering I put in the code to write it out :)) store the dirty flag
		//		It's the 1 byte that comes right before the serial.  If it's 0, it's clean, if it's 1, it's dirty
		//		If we read a dirty record on load, we MUST delete it
		//		</Abaddon>
		//	
		// b) How do we handle deleted Items. 
		//	  I would say: just store a vector of
		//	  Items which should be deleted on the
		//	  next save (fileposition)

		//		<Abaddon>
		//		The other option is to continue with the delete, and do what we would if the data changes... flag
		//		the existing record as dirty, so that the dirty record doesn't get reloaded on next server start
		//		This requires no overhead on memory for deleted items, and it involves writing just a single byte
		//		to the record where it is stored
		//		</Abaddon>
		//
		// c) How do we want to handle new or changed items
		//	  which are longer than the original item
		//    My idea is to flag the "shorter" items
		//    and just write out to the end of the file
		//    Still we got a problem here as it's get problematic
		//    with regions, here i would eventually remove all
		//	  region delimiters and decide based on x+y+world in which
		//	  region we want to put the stuff.

		//		<Abaddon>
		//		The region based system does not easily allow for updates/flagging as a flat file would
		//		However, if we store the region number that we were last saved in (or perhaps, region block, as it is 8x8)
		//		and on save, if that region is NOT the same, we dirty the old region record, and write out the new 
		//		record in our new region
		//		</Abaddon>

		//		<Abaddon>
		// d) Bigger concern.  How do we handle CHARACTERS that change, when their INVENTORY hasn't?
		//		My suggestion here is two fold
		
		//		1) If the region for the character has changed, dirty everything related to the character (so his inventory becomes dirty)
		//			and write out the character + items to the new region
		//		2) if the region for the character is the same (same region BLOCK?), dirty the character ONLY, and write him to EOF.
		//			Sure, the items will now be out of sequence, but most of that is covered in postloadprocessing anyway
		//			And saving them together was only a convenience for TEXT systems
		//		</Abaddon>

		//		I think my suggestions overall will probably be a simpler and easier method of doing things.  Requires
		//		small amounts of code and logic, and puts more of the onus back onto loading the world
		//		This way, save performance is dramatically increased for a small load hit (if dirty, return false, and delete)
	}

	//In future, we'll want to clear the dirty flag here, once it's saved" )
	TAG *tag = NULL;
	SI16 temp_st2, temp_dx2, temp_in2;
	buff.PutByte( 0 );	// We put something here to say it's CLEAN for next reload
	buff.PutLong( serial );

	buff.PutShort( x ); 
	buff.PutShort( y ); 
	buff.PutByte( z );
	buff.PutByte( worldNumber );
	buff.PutByte( dir );

	buff.PutShort( id );

	buff.PutStr( name );
	
	if( strcmp( DefBase->title, title ) )
	{
		buff.PutByte( BASETAG_TITLE );
		buff.PutStr( title );
	}
	
	if( DefBase->objType != objType )
	{
		buff.PutByte( BASETAG_OBJ_TYPE );
		buff.PutByte( (UI08)objType );
	}
	
	if( DefBase->colour != colour )
	{
		buff.PutByte( BASETAG_COLOR );
		buff.PutShort( colour );
	}
	
	if( DefBase->kills != kills )
	{
		buff.PutByte( BASETAG_KILLS );
		buff.PutShort( kills );
	}
	
	if( DefBase->fame != fame || DefBase->karma != karma )
	{
		buff.PutByte( BASETAG_NOTOR );
		buff.PutShort( fame );
		buff.PutShort( karma );
	}
	
	if( DefBase->intelligence != intelligence || DefBase->mana != mana ||
		DefBase->dexterity != dexterity || DefBase->stamina != stamina ||
		DefBase->strength != strength || DefBase->hitpoints != hitpoints )
	{
		buff.PutByte( BASETAG_STATS );
		buff.PutShort( intelligence );
		buff.PutShort( mana );
		buff.PutShort( dexterity );
		buff.PutShort( stamina );
		buff.PutShort( strength );
		buff.PutShort( hitpoints );
	}
	
	
	//=========== BUG (= For Characters the dex+str+int malis get saved and get rebuilt on next serverstartup = increasing malis)
	if( DefBase->in2 != in2 || DefBase->dx2 != dx2 || DefBase->st2 != st2 )
	{
		temp_st2 = st2;
		temp_dx2 = dx2;
		temp_in2 = in2;
		if( objType == OT_CHAR )
		{
			CChar *myChar = (CChar*)(this);
			
			// For every equipped item 
			// We need to reduce Str2+Dex2+Int2
			for( UI08 i = 0; i < MAXLAYERS; i++ )
			{
				CItem *myItem = myChar->GetItemAtLayer( i );
				if( myItem == NULL )
					continue;
				
				temp_st2 -= myItem->Strength2();
				temp_dx2 -= myItem->Dexterity2();
				temp_in2 -= myItem->Intelligence2();
			}
		}
		
		//=========== BUGFIX END (by Dark-Storm)
		
		buff.PutByte( BASETAG_STATS2 );
		buff.PutShort( temp_st2 );
		buff.PutShort( temp_dx2 );
		buff.PutShort( temp_in2 );
	}

	if( multis != NULL )
	{
		buff.PutByte( BASETAG_MULTI );
		buff.PutLong( multis->GetSerial() );
	}
	if( spawnserial != NULL )
	{
		buff.PutByte( BASETAG_SPAWNER );
		buff.PutLong( spawnserial->GetSerial() );
	}
	if( owner != NULL )
	{
		buff.PutByte( BASETAG_OWNER );
		buff.PutLong( owner->GetSerial() );
	}

	if( DefBase->race != race )
	{
		buff.PutByte( BASETAG_RACE );
		buff.PutShort( race );
	}
	
	if( DefBase->visible != visible )
	{
		buff.PutByte( BASETAG_VIS );
		buff.PutByte( visible );
	}
	
	if( DefBase->disabled != disabled )
	{
		buff.PutByte( BASETAG_DISABLE );
		buff.PutByte( disabled );
	}
	
	if( DefBase->hidamage != hidamage || DefBase->lodamage != lodamage )
	{
		buff.PutByte( BASETAG_DAMAGE );
		buff.PutShort( hidamage );
		buff.PutShort( lodamage );
	}
	
	if( DefBase->def != def )
	{
		buff.PutByte( BASETAG_DEF );
		buff.PutShort( def );
	}
	
	if( DefBase->scriptTrig != scriptTrig )
	{
		buff.PutByte( BASETAG_SCPTRG );
		buff.PutShort( scriptTrig );
	}
	
	if( memcmp( DefBase->genericDWords, genericDWords, 4*4 ) )
	{
		buff.PutByte( BASETAG_DWORDS );
		buff.Put( genericDWords, 4*4 );
	}
	
	for( tag = firstTag; tag != NULL; tag = tag->nextTag ) 
	{
		if( JSVAL_IS_STRING( tag->val ) ) 
		{
			if( tag->val != (jsval)NULL )
			{
				buff.PutByte( BASETAG_JS_STR_TAG );
				buff.PutStr( tag->name );
				char *s_tagval = JS_GetStringBytes( JS_ValueToString( jsContext, tag->val ) );
				buff.PutStr( s_tagval );
			}
		} 
		else 
		{
			buff.PutByte( BASETAG_JS_INT_TAG );
			buff.PutStr( tag->name );
			buff.PutLong( ((UI32)tag->val) );
		}
	}

	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	DumpBody( ofstream &outStream, int mode ) const
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Dumps out body information of the object
//|						This is tag/data pairing information
//o--------------------------------------------------------------------------
bool cBaseObject::DumpBody( ofstream &outStream, int mode ) const
{
	string destination;
	ostringstream dumping( destination );
	TAG *tag = NULL;
	SI16 temp_st2, temp_dx2, temp_in2;
	
	
	//no need to check default for these, this should be in for every thing.
	dumping << "Name=" << name << endl;
	dumping << "Serial=" << serial << endl;
	dumping << "XYZ=" << x << "," << y << "," << (SI16)z << endl;
	dumping << "WorldNumber=" << (SI16)worldNumber << endl;
	dumping << "ObjectType=" << (int)objType << endl;
	dumping << "ID=" << id << endl;
	dumping << "Colour=" << colour << endl;
	dumping << "Direction=" << (SI16)dir << endl;
	dumping << "Title=" << title << endl;
	dumping << "Kills=" << kills << endl;
	dumping << "Fame=" << fame << endl;
	dumping << "Karma=" << karma << endl;
	dumping << "Mana=" << mana << endl;
	dumping << "Stamina=" << stamina << endl;
	dumping << "Dexterity=" << dexterity << endl;
	dumping << "Intelligence=" << intelligence << endl;
	dumping << "Strength=" << strength << endl;
	dumping << "HitPoints=" << hitpoints << endl;
	
	//=========== BUG (= For Characters the dex+str+int malis get saved and get rebuilt on next serverstartup = increasing malis)
	temp_st2 = st2;
	temp_dx2 = dx2;
	temp_in2 = in2;
	if( objType == OT_CHAR )
	{
		CChar *myChar = (CChar*)(this);
		
		// For every equipped item 
		// We need to reduce Str2+Dex2+Int2
		for( UI08 i = 0; i < MAXLAYERS; i++ )
		{
			CItem *myItem = myChar->GetItemAtLayer( i );
			if( myItem == NULL )
				continue;
			
			temp_st2 -= myItem->Strength2();
			temp_dx2 -= myItem->Dexterity2();
			temp_in2 -= myItem->Intelligence2();
		}
	}
	
	//=========== BUGFIX END (by Dark-Storm)
	
	dumping << "Strength2=" << temp_st2 << endl;
	dumping << "Dexterity2=" << temp_dx2 << endl;
	dumping << "Intelligence2=" << temp_in2 << endl;
	//====================================================================================
	// Ab, here is another problem, tell me if this is a suitble way of doing this for you
	if( multis != NULL )
		dumping << "MultiID=" << multis->GetSerial() << endl;
	if( spawnserial != NULL )
		dumping << "SpawnerID=" << spawnserial->GetSerial() << endl;
	if( owner != NULL )
		dumping << "OwnerID=" << owner->GetSerial() << endl;
	//====================================================================================
	dumping << "Race=" << race << endl;
	dumping << "Visible=" << (SI16)visible << endl;
	dumping << "Disabled=" << (disabled?"1":"0") << endl;
	dumping << "HiDamage=" << hidamage << endl;
	dumping << "LoDamage=" << lodamage << endl;
	dumping << "Defense=" << def << endl;
	dumping << "ScpTrig=" << scriptTrig << endl;
	dumping << "DWord0=" << genericDWords[0] << endl;
	dumping << "DWord1=" << genericDWords[1] << endl;
	dumping << "DWord2=" << genericDWords[2] << endl;
	dumping << "DWord3=" << genericDWords[3] << endl;
	// Ab, does this need to be written to the file? I am going to make it write it but comment or remove it if its not(remember to remove it from the Load routine as well)
	dumping << "iCounter=" << iCounter << endl;
	for( tag = firstTag; tag != NULL; tag = tag->nextTag ) 
	{
		dumping << "TAGNAME=" << tag->name << endl;
		if( JSVAL_IS_STRING( tag->val ) ) 
		{
			char *s_tagval = JS_GetStringBytes( JS_ValueToString( jsContext, tag->val ) );
			dumping << "TAGVALS=" << (s_tagval) << endl;
		} 
		else 
		{
			dumping << "TAGVAL=" << ((SI32)tag->val) << endl;
		}
	}
	//====================================================================================
	
	outStream << dumping.str();
	
	
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
	if( mode == 1 )
	{
		BinBuffer buff;
		buff.SetType( 0 );

		DumpBody( buff );
		// need to set FilePosition here for future!
		FilePosition = outStream.tellp();
		buff.Write( outStream );
	} 
	else 
	{
		DumpBody( outStream, mode );
	}
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
	name[MAX_NAME - 1] = 0;
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
//	if ( newValue > 0 || objType != OT_CHAR )//never set a char's stats to 0
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
//	if ( newValue > 0 || objType != OT_CHAR )//never set a char's stats to 0
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
//	if ( newValue > 0 || objType != OT_CHAR )//never set a char's stats to 0
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
//|	Function		-	SetDir( UI08 newDir )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the direction of the object
//o--------------------------------------------------------------------------
void cBaseObject::SetDir( UI08 newDir )
{
	dir = newDir;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI08 GetDir()
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the direction of the object
//o--------------------------------------------------------------------------
UI08 cBaseObject::GetDir( void ) const
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
//|	Function		-	RemoveFromMulti( bool fireTrigger )
//|	Date			-	15 December, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Removes object from a multi, selectively firing the trigger
//o--------------------------------------------------------------------------
void cBaseObject::RemoveFromMulti( bool fireTrigger )
{
	if( multis != NULL )
	{
		if( multis->GetObjType() == OT_MULTI )
		{
			CMultiObj *mulObj = static_cast<CMultiObj *>(multis);
			mulObj->RemoveFromMulti( this );
			if( fireTrigger )
			{
				cScript *onLeaving = Trigger->GetScript( GetScriptTrigger() );
				if( onLeaving != NULL )
					onLeaving->OnLeaving( mulObj, this );
			}
		}
		else
			Console.Error( 2, "Object of type %i with serial %i has a bad multi setting of %i", GetObjType(), serial, multis->GetSerial() );
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	AddToMulti( void )
//|	Date			-	15 December, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Adds object to multi
//o--------------------------------------------------------------------------
void cBaseObject::AddToMulti( bool fireTrigger )
{
	if( multis != NULL )
	{
		if( multis->GetObjType() == OT_MULTI )
		{
			CMultiObj *mulObj = static_cast<CMultiObj *>(multis);
			mulObj->AddToMulti( this );
			if( fireTrigger )
			{
				cScript *onEntrance = Trigger->GetScript( GetScriptTrigger() );
				if( onEntrance != NULL )
					onEntrance->OnEntrance( mulObj, this );
			}
		}
		else
			Console.Error( 2, "Object of type %i with serial %i has a bad multi setting of %i", GetObjType(), serial, multis->GetSerial() );
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	SetMulti( cBaseObject *newMulti )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's multi to newMulti
//o--------------------------------------------------------------------------
void cBaseObject::SetMulti( cBaseObject *newMulti, bool fireTrigger )
{
	RemoveFromMulti( fireTrigger );
	multis = newMulti;
	AddToMulti( fireTrigger );
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
//|	Function		-	UI08 GetSerial( UI08 part )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns part of a serial #
//o--------------------------------------------------------------------------
UI08 cBaseObject::GetSerial( UI08 part ) const
{
	switch( part )
	{
	case 1:	return (UI08)(serial>>24);
	case 2:	return (UI08)(serial>>16);
	case 3: return (UI08)(serial>>8);
	case 4: return (UI08)(serial%256);
	}
	return 0;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI08 GetMulti( UI08 part )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns part of the multi's serial
//o--------------------------------------------------------------------------
UI08 cBaseObject::GetMulti( UI08 part ) const
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
UI08 cBaseObject::GetSpawn( UI08 part ) const
{
	if( spawnserial == NULL )
		return 0x0;
	return spawnserial->GetSerial( part );
}

//o--------------------------------------------------------------------------
//|	Function		-	UI08 GetOwner( UI08 part )
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns part of the item's owner's serial
//o--------------------------------------------------------------------------
UI08 cBaseObject::GetOwner( UI08 part ) const
{
	if( owner == NULL )
		return 0;
	return owner->GetSerial( part );
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
	if( newSerial == INVALIDSERIAL )
		owner = NULL;
	if( newSerial >= 0x40000000 )
		owner = (cBaseObject *)calcItemObjFromSer( newSerial );
	else
		owner = (cBaseObject *)calcCharObjFromSer( newSerial );
}

//o--------------------------------------------------------------------------
//|	Function		-	SI16 GetHiDamage( void )
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
//|	Function		-	SI16 GetLoDamage( void )
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
//|	Function		-	SetHiDamage( SI16 newValue )
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
//|	Function		-	SetLoDamage( SI16 newValue )
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
//|	Function		-	SI32 SetFilePosition( LONG filepos )
//|	Date			-	Unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the object's file position
//o--------------------------------------------------------------------------
SI32 cBaseObject::SetFilePosition( SI32 filepos )
{
	FilePosition = filepos;
	return FilePosition;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI32 GetFilePosition( void )
//|	Date			-	Unknown
//|	Programmer		-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the object's file position
//o--------------------------------------------------------------------------
SI32 cBaseObject::GetFilePosition( void ) const
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
	mana = min( max( 0, mn ), intelligence );
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
		outStream << endl << "o---o" << endl << endl;
	}
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool Load( :ifstream &inStream, int mode)
//|	Date			-	28 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-	Zippy (1/9/02) no longer needs mode
//o--------------------------------------------------------------------------
//|	Purpose			-	Loads object from disk based on mode
//o--------------------------------------------------------------------------
bool cBaseObject::Load( ifstream &inStream, int arrayOffset )
{
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
			{
				Console.Warning( 1, "Unknown world file tag %s with contents of %s", tag, data );
			}
		}
	}
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool Load( BinBuffer &, int arrayOffset)
//|	Date			-	09 January, 2002
//|	Programmer		-	Zippy
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Loads binary object from disk
//o--------------------------------------------------------------------------
bool cBaseObject::Load( BinBuffer &buff, int arrayOffset )
{
	UI08 tag = 0;
	UI08 ltag;

	isDirty = buff.GetByte();
	//Remove this when dirty flagging works right and activate commented code at the end")
	isDirty = 1;
	serial = (UI32)buff.GetLong();

	x = buff.GetShort();
	y = buff.GetShort();
	z = (SI08)buff.GetByte();
	worldNumber = buff.GetByte();
	dir = buff.GetByte();

	id = (UI16)buff.GetShort();

	buff.GetStr( name, MAX_NAME );

	while( !buff.End() )
	{
		ltag = tag;
		tag = buff.GetByte();
		if( !buff.End() )
		{
			if( !HandleBinTag( tag, buff ) )
				Console.Warning( 2, "Unknown cBaseObject world file tag %i [%x] length of %i (Last Tag %i[%x])", tag, tag, CharSaveTable[tag], ltag, ltag );
		}
	}

	// We have to do this here, otherwise we return prematurely, WITHOUT having read the entire record!
//	if( isDirty )
//		return false;
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool HandleBinTag( char tag, BinBuffer &buff )
//|	Date			-	09 January, 2002
//|	Programmer		-	Zippy
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Used to handle world binary tags.  
//|						Returns true if the tag was processed.
//o--------------------------------------------------------------------------
bool cBaseObject::HandleBinTag( UI08 tag, BinBuffer &buff )
{
	char p1[256], p2[256];
	JSString *tagstr = NULL;
	jsval tagval = 0;

	switch ( tag )
	{
	case BASETAG_TITLE:
		buff.GetStr( title, 50 );
		break;

	case BASETAG_OBJ_TYPE:
		objType = (ObjectType)buff.GetByte();
		break;

	case BASETAG_COLOR:
		colour = (UI16)buff.GetShort();
		break;

	case BASETAG_KILLS:
		kills = (UI16)buff.GetShort();
		break;

	case BASETAG_NOTOR:
		fame = (UI16)buff.GetShort();
		karma = (UI16)buff.GetShort();
		break;

	case BASETAG_STATS:
		intelligence = buff.GetShort();
		mana = buff.GetShort();
		dexterity = buff.GetShort();
		stamina = buff.GetShort();
		strength = buff.GetShort();
		hitpoints = buff.GetShort();
		break;

	case BASETAG_STATS2:
		st2 = buff.GetShort();
		dx2 = buff.GetShort();
		in2 = buff.GetShort();
		break;

	case BASETAG_MULTI:
		multis = (cBaseObject *)buff.GetLong();
		break;

	case BASETAG_SPAWNER:
		spawnserial = (cBaseObject *)buff.GetLong();
		break;

	case BASETAG_OWNER:
		owner = (cBaseObject *)buff.GetLong();
		break;

	case BASETAG_RACE:
		race = (UI16)buff.GetShort();
		break;

	case BASETAG_VIS:
		visible = (SI08)buff.GetByte();
		break;

	case BASETAG_DISABLE:
		disabled = ( buff.GetByte() != 0 );
		break;

	case BASETAG_DAMAGE:
		hidamage = buff.GetShort();
		lodamage = buff.GetShort();
		break;

	case BASETAG_DEF:
		def = (UI16)buff.GetShort();
		break;

	case BASETAG_SCPTRG:
		scriptTrig = (UI16)buff.GetShort();
		break;

	case BASETAG_DWORDS:
		buff.Get( genericDWords, 4*4 );
		break;

	case BASETAG_JS_STR_TAG:
		buff.GetStr( p1 );
		buff.GetStr( p2 );

		tagstr = JS_NewStringCopyZ( jsContext, p2 );
		tagval = STRING_TO_JSVAL( tagstr );
		SetTag( p1, tagval );
		break;

	case BASETAG_JS_INT_TAG:
		buff.GetStr( p1 );
		SetTag( p1, ((jsval)(buff.GetLong())) );
		break;

	default:
		return false;
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
	static char* staticTagName = "";
	if( !strcmp( tag, "o---o" ) )
		return true;
	switch( tag[0] )
	{
	case 'C':
		if( !strcmp( tag, "Colour" ) )
		{
			colour = (UI16)makeNum( data );
			return true;
		}
		break;
	case 'D':
		if( !strcmp( tag, "Direction" ) )
		{
			dir = (UI08)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "Dexterity" ) )
		{
			dexterity = (SI16)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "Dexterity2" ) )
		{
			dx2 = (SI16)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "Defense" ) )
		{
			def = (UI16)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "DWord0" ) )
		{
			genericDWords[0] = makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "DWord1" ) )
		{
			genericDWords[1] = makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "DWord2" ) )
		{
			genericDWords[2] = makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "DWord3" ) )
		{
			genericDWords[3] = makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "Disabled" ) )
		{
			SetDisabled( makeNum( data ) == 1 );
			return true;
		}
		break;
	case 'F':
		if( !strcmp( tag, "Fame" ) )
		{
			fame = (SI16)makeNum( data );
			return true;
		}
		break;
	case 'H':
		if( !strcmp( tag, "HitPoints" ) )
		{
			hitpoints = (SI16)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "HiDamage" ) )
		{
			hidamage = (SI16)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "HiDamage" ) )
		{
			hidamage = (SI16)makeNum( data );
			return true;
		}
		break;
	case 'I':
		if( !strcmp( tag, "ID" ) )
		{
			id = (UI16)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "Intelligence" ) )
		{
			intelligence = (SI16)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "Intelligence2" ) )
		{
			in2 = (SI16)makeNum( data );
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
			kills = (SI16)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "Karma" ) )
		{
			karma = (SI16)makeNum( data );
			return true;
		}
		break;
	case 'L':
		if( !strcmp( tag, "LoDamage" ) )
		{
			lodamage = (SI16)makeNum( data );
			return true;
		}
		break;
	case 'M':
		if( !strcmp( tag, "Mana" ) )
		{
			mana = (SI16)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "MultiID" ) )
		{
			multis = (cBaseObject *)makeNum( data );
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
			objType = (ObjectType)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "OwnerID" ) )
		{
			owner = (cBaseObject *)makeNum( data );
			return true;
		}
		break;
	case 'R':
		if( !strcmp( tag, "Race" ) )
		{
			race = makeNum( data );
			return true;
		}
		break;
	case 'S':
		if( !strcmp( tag, "Stamina" ) )
		{
			stamina = (SI16)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "SpawnerID" ) )
		{
			spawnserial = (cBaseObject *)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "Serial" ) )
		{
			serial = makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "Strength" ) )
		{
			strength = (SI16)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "Strength2" ) )
		{
			st2 = (SI16)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "ScpTrig" ) )
		{
			scriptTrig = (UI16)makeNum( data );
			return true;
		}
		break;
	case 'T':
		if( !strcmp( tag, "Title" ) )
		{
			strcpy( title, data );
			return true;
		}
		else if( !strcmp( tag, "TAGNAME" ) )
		{
			staticTagName = new char[strlen(data)+1];
			strcpy(staticTagName, data);
			return true;
		}
		else if( !strcmp( tag, "TAGVAL" ) )
		{
			SetTag( staticTagName, ((jsval)((long int)(makeNum( data )))));
			return true;
		}
		else if( !strcmp( tag, "TAGVALS" ) )
		{
			JSString *tagvals = NULL;
			jsval tagvali = 0;
			tagvals = JS_NewStringCopyZ( jsContext, data );
			tagvali = STRING_TO_JSVAL(tagvals);
			SetTag( staticTagName, tagvali);
			return true;
		}
		break;
	case 'V':
		if( !strcmp( tag, "Visible" ) )
		{
			visible = (SI08)makeNum( data );
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
			x = (SI16)makeNum( tmp );
			strncpy( tmp, yOff + 1, zOff - yOff - 1 );
			tmp[zOff - yOff - 1] = 0;
			y = (SI16)makeNum( tmp );
			z = (SI08)makeNum( zOff + 1 );
			return true;
		}
		break;
	case 'W':
		if( !strcmp( tag, "WorldNumber" ) )
		{
			worldNumber = (UI08)makeNum( data );
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
UI32 cBaseObject::GetWord( UI08 wordNum ) const
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
bool cBaseObject::GetBit( UI08 wordNum, UI08 bitNum ) const
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
UI32 cBaseObject::GetBitRange( UI08 wordNum, UI08 lowBit, UI08 highBit ) const
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
//|	Function		-	PostLoadProcessing( UI32 index )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Used to setup any pointers that may need adjustment
//|						following the loading of the world
//o--------------------------------------------------------------------------
void cBaseObject::PostLoadProcessing( UI32 index )
{
	SERIAL tmpSerial = 0xFFFFFFFF;
	if( multis != NULL )
	{
		tmpSerial = (SERIAL)multis;
		multis = NULL;
		SetMulti( tmpSerial, false );
	}
	if( spawnserial != NULL )
	{
		tmpSerial = (SERIAL)spawnserial;
		spawnserial = NULL;
		SetSpawn( (SERIAL)tmpSerial );
	}
	if( owner != NULL )
	{
		tmpSerial = (SERIAL)owner;
		owner = NULL;
		SetOwner( (SERIAL)tmpSerial );
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	UI08 WorldNumber( void )
//|	Date			-	26th September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the world number that the object belongs in
//o--------------------------------------------------------------------------
UI08 cBaseObject::WorldNumber( void ) const
{	
	return worldNumber;
}

//o--------------------------------------------------------------------------
//|	Function		-	WorldNumber( UI08 value )
//|	Date			-	26th September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the world number that the object belongs in
//o--------------------------------------------------------------------------
void cBaseObject::WorldNumber( UI08 value )
{
//	MapRegion->Remove( this );
	worldNumber = value;
//	MapRegion->Add( this );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool ShouldSave( void )
//|	Date			-	21st December, 2001
//|	Programmer		-	Abaddon for Thyme
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the object should be saved out to disk
//o--------------------------------------------------------------------------
bool cBaseObject::ShouldSave( void ) const
{
	return shouldSave;
}

//o--------------------------------------------------------------------------
//|	Function		-	void ShouldSave( bool value )
//|	Date			-	21st December, 2001
//|	Programmer		-	Abaddon for Thyme
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets whether the object should be saved out to disk
//o--------------------------------------------------------------------------
void cBaseObject::ShouldSave( bool value )
{
	shouldSave = value;
}
