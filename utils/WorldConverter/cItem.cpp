#include "uox3.h"

// New class written based upon old UOX char_st.  Number of old members removed
// and a number of members types modified as well

// Version History
// 1.0		Abaddon		2nd April, 2000
//			Initial implementation
//			Bools reduced down to a single short with get/set mechanisms for setting/clearings bits
//			No documentation currently done, all functions obvious so far
//			Note:  individual byte setting within longs/shorts need speeding up
//			consider memcpy into correct word, note that this will be endian specific!
// 1.1		Abaddon		23rd July, 2000
//			Addition of SaveItem routine for text mode only
// 1.2		Abaddon		25 July, 2000
//			All Accessor funcs plus a few others are now flagged as const functions, meaning that
//			those functions GUARENTEE never to alter data, at compile time
//			Thoughts about cBaseObject and prelim plans made
// 1.3		Abaddon		28 July, 2000
//			cBaseObject initial implementation plus rework of CItem to deal with only what it needs to
//			Proper constructor written
//			Plans for CItem derived classes thought upon

//o--------------------------------------------------------------------------
//|	Function		-	UI32 GetCont()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the container's serial
//o--------------------------------------------------------------------------
UI32 CItem::GetCont(   void ) const
{
	return contserial;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 GetMore()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the more value of the object
//o--------------------------------------------------------------------------
UI32 CItem::GetMore(   void ) const
{
	return more;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 GetMoreB()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the moreb value of the object
//o--------------------------------------------------------------------------
UI32 CItem::GetMoreB(  void ) const
{
	return moreb;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI08 GetCont( UI08 part )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns one of the words of the container's serial
//|						Valid values for part are 1->4.  If outside that, behaves
//|						as if it were 1
//o--------------------------------------------------------------------------
UI08 CItem::GetCont( UI08 part ) const
{
	switch( part )
	{
	default:
	case 1:		return (UI08)(contserial>>24);
	case 2:		return (UI08)(contserial>>16);
	case 3:		return (UI08)(contserial>>8);
	case 4:		return (UI08)(contserial%256);
	}
	return (UI08)(contserial>>24);
}

//o--------------------------------------------------------------------------
//|	Function		-	UI08 GetMore( UI08 part )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns one of the words of the more value
//|						Valid values for part are 1->4.  If outside that, behaves
//|						as if it were 1
//o--------------------------------------------------------------------------
UI08 CItem::GetMore(   UI08 part ) const
{
	switch( part )
	{
	default:
	case 1:		return (UI08)(more>>24);
	case 2:		return (UI08)(more>>16);
	case 3:		return (UI08)(more>>8);
	case 4:		return (UI08)(more%256);
	}
	return (UI08)(more>>24);
}

//o--------------------------------------------------------------------------
//|	Function		-	UI08 GetMoreB( UI08 part )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns one of the words of the item's moreb value
//|						Valid values for part are 1->4.  If outside that, behaves
//|						as if it were 1
//o--------------------------------------------------------------------------
UI08 CItem::GetMoreB(  UI08 part ) const
{
	switch( part )
	{
	default:
	case 1:		return (UI08)(moreb>>24);
	case 2:		return (UI08)(moreb>>16);
	case 3:		return (UI08)(moreb>>8);
	case 4:		return (UI08)(moreb%256);
	}
	return (UI08)(moreb>>24);
}

//o--------------------------------------------------------------------------
//|	Function		-	SetSerial( UI32 newValue )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the item's serial
//o--------------------------------------------------------------------------
void CItem::SetSerial( UI32 newValue )
{
	// remove from hashtable
	serial = newValue;
	// add to hashtable
}

//o--------------------------------------------------------------------------
//|	Function		-	SI08 SetCont( UI32 newValue )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Set's the item's container value to newValue
//|						Takes it out of the old container, and puts in the new
//|						Copes with being on paperdolls, ground and in containers
//|						Also copes with removing and adding to a map region
//|						Returns -1 if item needs deleting, 0 if fine
//o--------------------------------------------------------------------------
SI08 CItem::SetCont( UI32 newValue )
// returns -1 if needs deleting, 0 if fine
{
	if( contserial != 0xFFFFFFFF )
	{
		if( (contserial>>24) < 0x40 )	// it's a char!
		{
			CChar *targChar = calcCharObjFromSer( contserial );
			if( targChar != NULL )
				if( !targChar->TakeOffItem( GetLayer(), this ) )
					fprintf( errorLog, "Char %s (%i) was never wearing item on layer %i\n", targChar->GetName(), contserial, GetLayer() );
		}
		else
		{
			CItem *targItem = calcItemObjFromSer( contserial );
			if( targItem != NULL )
				if( !targItem->ReleaseItem( this ) )
					fprintf( errorLog, "Error removing %s(%i) from %s(%i)\n", GetName(), GetSerial(), targItem->GetName(), contserial );
		}
	}
	else
		MapRegion->RemoveItem( this );

	contserial = newValue;
	if( newValue != 0xFFFFFFFF ) 
	{
		if( (newValue>>24) < 0x40 )
		{
			CChar *charWearing = calcCharObjFromSer( newValue );
			if( charWearing != NULL )
			{
				if( !charWearing->WearItem( this ) )
				{
					if( layer >= MAXLAYERS )
					{
						contserial = 0xFFFFFFFF;
						MapRegion->AddItem( this );
						return 0;
					}
//					return -1;	// disable for now
				}
				return 0;
			}
			else
			{
				contserial = 0xFFFFFFFF;
				MapRegion->AddItem( this );
				return 0;
			}
		}
		else
		{
			CItem *itemHolder = calcItemObjFromSer( newValue );
			if( itemHolder != NULL )
			{
				itemHolder->HoldItem( this );
				return 0;
			}
			else
			{
				contserial = 0xFFFFFFFF;
				MapRegion->AddItem( this );
				return 0;
			}
		}
	}
	else	// this is in case we're initing the default item
	{
		MapRegion->AddItem( this );
	}
	return 0;
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetMore( UI32 newValue )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the item's more value to newValue
//o--------------------------------------------------------------------------
void CItem::SetMore( UI32 newValue )
{
	more = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetMoreB( UI32 newValue )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the MoreB to newValue
//o--------------------------------------------------------------------------
void CItem::SetMoreB( UI32 newValue )
{
	moreb = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetOwner( SERIAL newValue )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the owner to newValue, ensuring it's removed from
//|						the old owner
//o--------------------------------------------------------------------------
void CItem::SetOwner( SERIAL newValue )
{
	RemoveSelfFromOwner();
	cBaseObject::SetOwner( newValue );
	AddSelfToOwner();
}

//o--------------------------------------------------------------------------
//|	Function		-	void ( UI32 newValue, ITEM index )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the item's spawner to newValue, ensuring it's
//|						removed from the old hash entry (if any)
//|						Also ensures it is added to the hash table afterwards
//o--------------------------------------------------------------------------
void CItem::SetSpawn( UI32 newValue, ITEM index )
{
	cBaseObject::SetSpawn( newValue );
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetSerial( UI08 newValue, UI08 part )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets byte part of the serial to newValue
//o--------------------------------------------------------------------------
void CItem::SetSerial( UI08 newValue, UI08 part )
{
	// remove from hash table
	cBaseObject::SetSerial( newValue, part );
	// add to hash table
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetMore( UI08 newValue, UI08 part )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the more value's part byte to newValue 
//o--------------------------------------------------------------------------
void CItem::SetMore(   UI08 newValue, UI08 part )
{
	UI08 part1, part2, part3, part4;
	part1 = (UI08)(more>>24);
	part2 = (UI08)(more>>16);
	part3 = (UI08)(more>>8);
	part4 = (UI08)(more%256);
	switch( part )
	{
	case 1:		part1 = newValue;		break;
	case 2:		part1 = newValue;		break;
	case 3:		part1 = newValue;		break;
	case 4:		part1 = newValue;		break;
	}
	more = (part1<<24) + (part2<<16) + (part3<<8) + part4;
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetMoreB( UI08 newValue, UI08 part )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the moreb value's part byte to newValue
//o--------------------------------------------------------------------------
void CItem::SetMoreB(  UI08 newValue, UI08 part )
{
	UI08 part1, part2, part3, part4;
	part1 = (UI08)(moreb>>24);
	part2 = (UI08)(moreb>>16);
	part3 = (UI08)(moreb>>8);
	part4 = (UI08)(moreb%256);
	switch( part )
	{
	case 1:		part1 = newValue;		break;
	case 2:		part1 = newValue;		break;
	case 3:		part1 = newValue;		break;
	case 4:		part1 = newValue;		break;
	}
	moreb = (part1<<24) + (part2<<16) + (part3<<8) + part4;
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetOwner( UI08 newValue, UI08 part )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the owner's part byte to newValue, ensuring
//|						that it is added/removed properly from owners
//o--------------------------------------------------------------------------
void CItem::SetOwner(    UI08 newValue, UI08 part )
{
	RemoveSelfFromOwner();
	cBaseObject::SetOwner( newValue, part );
	AddSelfToOwner();
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetSpawn( UI08 newValue, UI08 part, ITEM index )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the spawner's part byte to newValue.
//|						It also ensures hash table integrity
//o--------------------------------------------------------------------------
void CItem::SetSpawn(  UI08 newValue, UI08 part, ITEM index )
{
	cBaseObject::SetSpawn( newValue, part );
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetSerial( UI08 part1, UI08 part2, UI08 part3, UI08 part4 )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the item's serial
//o--------------------------------------------------------------------------
void CItem::SetSerial( UI08 part1, UI08 part2, UI08 part3, UI08 part4 )
{
	// remove from hash table
	cBaseObject::SetSerial( (part1<<24) + (part2<<16) + (part3<<8) + part4 );
	// add to hash table
}

//o--------------------------------------------------------------------------
//|	Function		-	SI08 SetCont( UI08 part1, UI08 part2, UI08 part3, UI08 part4 )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the container value.  Returns -1 if it needs deleting
//o--------------------------------------------------------------------------
SI08 CItem::SetCont( UI08 part1, UI08 part2, UI08 part3, UI08 part4 )
{
	SERIAL newSerial = calcserial( part1, part2, part3, part4 );
	return SetCont( newSerial );
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetMore( UI08 part1, UI08 part2, UI08 part3, UI08 part4 )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the more value.
//o--------------------------------------------------------------------------
void CItem::SetMore( UI08 part1, UI08 part2, UI08 part3, UI08 part4 )
{
	more = (part1<<24) + (part2<<16) + (part3<<8) + part4;
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetMoreB( UI08 part1, UI08 part2, UI08 part3, UI08 part4 )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the moreb value.
//o--------------------------------------------------------------------------
void CItem::SetMoreB( UI08 part1, UI08 part2, UI08 part3, UI08 part4 )
{
	moreb = (part1<<24) + (part2<<16) + (part3<<8) + part4;
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetCont( UI08 part1, UI08 part2, UI08 part3, UI08 part4 )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the owner value, ensuring no ownership duplication
//o--------------------------------------------------------------------------
void CItem::SetOwner( UI08 part1, UI08 part2, UI08 part3, UI08 part4 )
{
	// remove from hash table
	CItem::SetOwner( (part1<<24) + (part2<<16) + (part3<<8) + part4 );
	// add to hash table
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetSpawn( UI08 part1, UI08 part2, UI08 part3, UI08 part4 )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the spawn value, ensuring table integrity
//o--------------------------------------------------------------------------
void CItem::SetSpawn( UI08 part1, UI08 part2, UI08 part3, UI08 part4, ITEM index )
{
	CItem::SetSpawn( (part1<<24) + (part2<<16) + (part3<<8) + part4, index );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool isFree()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the item is free
//o--------------------------------------------------------------------------
bool CItem::isFree(      void ) const
{
	return ( (bools&0x01) == 0x01 );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool isDoorOpen()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if it's a door and is open
//o--------------------------------------------------------------------------
bool CItem::isDoorOpen(  void ) const
{
	return ( (bools&0x02) == 0x02 );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool isPileable()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the item is pileable
//o--------------------------------------------------------------------------
bool CItem::isPileable(  void ) const
{
	return ( (bools&0x04) == 0x04 );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool isDyeable()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the item is dyeable
//o--------------------------------------------------------------------------
bool CItem::isDyeable(   void ) const
{
	return ( (bools&0x08) == 0x08 );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool isCorpse()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the item is a corpse
//o--------------------------------------------------------------------------
bool CItem::isCorpse(    void ) const
{
	return ( (bools&0x10) == 0x10 );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool isWipeable()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if is wipeable
//o--------------------------------------------------------------------------
bool CItem::isWipeable(  void ) const
{
	return ( (bools&0x20) == 0x20 );
}

void CItem::SetFree(     bool newValue )
{
	if( newValue )
		bools = bools|0x01;
	else
		bools = bools&0xFE;
}
void CItem::SetDoorOpen( bool newValue )
{
	if( newValue )
		bools = bools|0x02;
	else
		bools = bools&0xFD;
}
void CItem::SetPileable( bool newValue )
{
	if( newValue )
		bools = bools|0x04;
	else
		bools = bools&0xFB;
}
void CItem::SetDye(      bool newValue )
{
	if( newValue )
		bools = bools|0x08;
	else
		bools = bools&0xF7;
}
void CItem::SetCorpse(   bool newValue )
{
	if( newValue )
		bools = bools|0x10;
	else
		bools = bools&0xEF;
}
void CItem::SetWipeable( bool newValue )
{
	if( newValue )
		bools = bools|0x20;
	else
		bools = bools&0xDF;
}

const char *CItem::GetName2(     void ) const
{
	return name2;
}
const char *CItem::GetMurderer(  void ) const
{
	return murderer;
}
const char *CItem::GetCreator(   void ) const
{
	return creator;
}
const char *CItem::GetDesc(      void ) const
{
	return desc;
}

void CItem::SetName2( const char *newValue )
{
	strncpy( name2, newValue, MAX_NAME );
}
void CItem::SetMurderer( const char *newValue )
{
	strncpy( murderer, newValue, 50 );
}
void CItem::SetCreator( const char *newValue )
{
	strncpy( creator, newValue, 50 );
}
void CItem::SetDesc( const char *newValue )
{
	strncpy( desc, newValue, 100 );
}

void CItem::IncX( short int newValue )
{
	x += newValue;
}
void CItem::IncY( short int newValue )
{
	y += newValue;
}
void CItem::IncZ( short int newValue )
{
	z += newValue;
}
void CItem::SetLocation( short int newX, short int newY, SI08 newZ )
{
	MapRegion->RemoveItem( this );
	x = newX;
	y = newY;
	z = newZ;
	MapRegion->AddItem( this );
}

SI08 CItem::GetLayer( void ) const
{
	return layer;
}
void CItem::SetLayer( SI08 newValue ) 
{
	if( contserial != 0xFFFFFFFF && (contserial>>24) < 0x40 )	// if we're on a char
	{
		CChar *charAffected = calcCharObjFromSer( contserial );
		if( charAffected != NULL )
		{
			if( !charAffected->TakeOffItem( GetLayer(), this ) )
				fprintf( errorLog, "Char %s(%i) was never wearing item on layer %i\n", charAffected->GetName(), contserial, GetLayer() );
			layer = newValue;
			charAffected->WearItem( this );
			return;
		}
	}
	layer = newValue;
}

SI08 CItem::GetItmHand( void ) const
{
	return itmhand;
}
void CItem::SetItmHand( SI08 newValue )
{
	itmhand = newValue;
}
UI08 CItem::GetType(  void ) const
{
	return type;
}
UI08 CItem::GetType2( void ) const
{
	return type2;
}

void CItem::SetType(  UI08 newValue )
{
	type = newValue;
}
void CItem::SetType2( UI08 newValue )
{
	type2 = newValue;
}

SI08 CItem::GetOffSpell( void ) const
{
	return offspell;
}
void CItem::SetOffSpell( SI08 newValue )
{
	offspell = newValue;
}

SI32 CItem::GetWeight( void ) const
{
	return weight;
}
void CItem::SetWeight( SI32 newValue )
{
	weight = newValue;
}

UI32 CItem::GetMoreX( void ) const
{
	return morex;
}
UI32 CItem::GetMoreY( void ) const
{
	return morey;
}
UI32 CItem::GetMoreZ( void ) const
{
	return morez;
}

void CItem::SetMoreX( UI32 newValue )
{
	morex = newValue;
}
void CItem::SetMoreY( UI32 newValue )
{
	morey = newValue;
}
void CItem::SetMoreZ( UI32 newValue )
{
	morez = newValue;
}

SI32 CItem::GetAmount(  void ) const
{
	return amount;
}
SI32 CItem::GetAmount2( void ) const
{
	return amount2;
}
void CItem::SetAmount( SI32 newValue )
{
	amount = newValue;
}
void CItem::SetAmount2( SI32 newValue )
{
	amount2 = newValue;
}

UI08 CItem::GetDoorDir( void ) const
{
	return doordir;
}
void CItem::SetDoorDir( UI08 newValue )
{
	doordir = newValue;
}

SI16 CItem::GetMaxHP( void ) const
{
	return maxhp;
}

void CItem::SetMaxHP( SI16 newValue )
{
	maxhp = newValue;
}

UI08 CItem::GetSpeed( void ) const
{
	return spd;
}
void CItem::SetSpeed( UI08 newValue )
{
	spd = newValue;
}

SI08 CItem::GetMagic( void ) const
{
	return magic;
}
void CItem::SetMagic( SI08 newValue )
{
	magic = newValue;
}

UI32 CItem::GetGateTime(   void ) const
{
	return gatetime;
}
UI32 CItem::GetDecayTime(  void ) const
{
	return decaytime;
}
UI32 CItem::GetMurderTime( void ) const
{
	return murdertime;
}

void CItem::SetGateTime(   UI32 newValue )
{
	gatetime = newValue;
}
void CItem::SetDecayTime(  UI32 newValue )
{
	decaytime = newValue;
}
void CItem::SetMurderTime( UI32 newValue )
{
	murdertime = newValue;
}

char	CItem::GetGateNumber( void ) const
{
	return gatenumber;
}
void	CItem::SetGateNumber( char newValue )
{
	gatenumber = newValue;
}

UI08 CItem::GetPriv( void ) const
{
	return priv;
}
void CItem::SetPriv( UI08 newValue )
{
	priv = newValue;
}

SI32 CItem::GetValue( void ) const
{
	return value;
}
void	CItem::SetValue( SI32 newValue )
{
	value = newValue;
}

SI32 CItem::GetRestock( void ) const
{
	return restock;
}
void CItem::SetRestock( SI32 newValue )
{
	restock = newValue;
}

UI32 CItem::GetPoisoned( void ) const
{
	return poisoned;
}
void CItem::SetPoisoned( UI32 newValue )
{
	poisoned = newValue;
}

ARMORCLASS CItem::GetArmourClass( void ) const
{
	return armorClass;
}
void CItem::SetArmourClass( ARMORCLASS newValue )
{
	armorClass = newValue;
}

SI08 CItem::GetRank( void ) const
{
	return rank;
}
void CItem::SetRank( SI08 newValue )
{
	rank = newValue;
}

SI32 CItem::GetGood( void ) const
{
	return good;
}
void CItem::SetGood( SI32 newValue )
{
	good = newValue;
}

SI32 CItem::GetRndValueRate( void ) const
{
	return rndvaluerate;
}
void CItem::SetRndValueRate( SI32 newValue )
{
	rndvaluerate = newValue;
}

SI08 CItem::GetMadeWith( void ) const
{
	return madewith;
}
void CItem::SetMadeWith( SI08 newValue )
{
	madewith = newValue;
}

SI32 CItem::GetGlow( void ) const
{
	return glow;
}
void CItem::SetGlow( SI32 newValue )
{
	glow = newValue;
}

UI16 CItem::GetGlowColour( void ) const
{
	return glowColour;
}
UI08 CItem::GetGlowColour( UI08 part ) const
{
	switch( part )
	{
	case 1:		return (UI08)(glowColour>>8);
	case 2:		return (UI08)(glowColour%256);
	default:	return (UI08)(glowColour>>8);
	}
	return (UI08)(glowColour>>8);
}

void CItem::SetGlowColour( UI16 newValue )
{
	glowColour = newValue;
}
void CItem::SetGlowColour( UI08 newValue, UI08 part )
{
	UI08 part1, part2;
	part1 = (UI08)(glowColour>>8);
	part2 = (UI08)(glowColour%256);
	switch( part )
	{
	case 1:		part1 = newValue;		break;
	case 2:		part2 = newValue;		break;
	}
	glowColour = (part1<<8) + part2;
}
void CItem::SetGlowColourEx( UI08 part1, UI08 part2 )
{
	glowColour = (part1<<8) + part2;
}

UI08 CItem::GetGlowEffect( void ) const
{
	return glow_effect;
}
void CItem::SetGlowEffect( UI08 newValue )
{
	glow_effect = newValue;
}

SI32 CItem::GetCarve( void ) const
{
	return carve;
}
void CItem::SetCarve( SI32 newValue )
{
	carve = newValue;
}

void CItem::IncHP( short newValue )
{
	hitpoints += newValue;
}

void CItem::IncDef( UI16 newValue )
{
	def += newValue;
}

void CItem::IncID( short incAmount )
{
	id += incAmount;
}

CItem::CItem() : contserial( INVALIDSERIAL ), remove( false ), carve( -1 ), glow_effect( 0 ), glow( -1 ), glowColour( 0 ),
madewith( 0 ), rndvaluerate( 0 ), good( -1 ), rank( 0 ), armorClass( 0 ), poisoned( 0 ), restock( 0 ), value( 0 ),
priv( 0 ), gatenumber( 0xFF ), magic( 0 ), gatetime( 0 ), decaytime( 0 ), murdertime( 0 ), spd( 0 ),
maxhp( 0 ), doordir( 0 ), amount( 1 ), amount2( 0 ), morex( 0 ), morey( 0 ), enhanced( 0 ),
morez( 0 ), more( 0 ), moreb( 0 ), bools( 0 ), layer( 0 ), itmhand( 0 ), type( 0 ), type2( 0 ), offspell( 0 ), weight( 0 )
{
	objType = OT_ITEM;
	Contains.resize( 0 );
	strcpy( name2, "#" );
	strcpy( name, "#" );
	murderer[0] = 0;
	creator[0] = 0;
	desc[0] = 0;
	race = 65535;
	LightDamage( false );
	LightningDamage( false );
	RainDamage( false );
	SnowDamage( false );
	HeatDamage( false );
	ColdDamage( false );
}

CItem::~CItem()
{

}

bool CItem::HoldItem( CItem *toHold )
{
	// should we check if they're already in???  I'd say no... but who knows?
#define __SECURE__HOLD__
#ifdef __SECURE__HOLD__
	for( int counter = 0; counter < Contains.size(); counter++ )
	{
		if( Contains[counter] == toHold )	// same pointer ie same thing
			fprintf( errorLog, "%s(%i) already contains %s(%i)\n", GetName(), GetSerial(), toHold->GetName(), toHold->GetSerial() );
	}
#endif
	int index = Contains.size();
	Contains.resize( Contains.size() + 1 );
	Contains[index] = toHold;
	return true;
}
bool CItem::ReleaseItem( int index )
{
	for( int counter = 0; counter < Contains.size(); counter++ )
	{
		if( Contains[counter]->GetSerial() == items[index].GetSerial() )
		{
			// remove from vector
			for( int counter2 = counter; counter2 < (Contains.size()-1); counter2++ )
			{
				Contains[counter2] = Contains[counter2+1];
			}
			Contains.resize( Contains.size() - 1 );
			if( iCounter >= 0 && counter <= iCounter )	// if what we're deleting is in our list, and before the current item
				iCounter--;
			return true;
		}
	}
	return false;
}
bool CItem::ReleaseItem( CItem *index )
{
	for( int counter = 0; counter < Contains.size(); counter++ )
	{
		if( Contains[counter] == index )
		{
			for( int counter2 = counter; counter2 < (Contains.size()-1); counter2++ )
				Contains[counter2] = Contains[counter2+1];
			Contains.resize( Contains.size() - 1 );
			if( iCounter >= 0 && counter <= iCounter )	// if what we're deleting is in our list, and before the current item
				iCounter--;
			return true;
		}
	}
	return false;
}

SI32 CItem::FirstItem( void ) const
{
	iCounter = 0;
	if( iCounter >= Contains.size() )
		return -1;
	return calcItemFromSer( Contains[iCounter]->GetSerial() );
}

CItem *CItem::FirstItemObj( void ) const
{
	iCounter = 0;
	if( iCounter >= Contains.size() )
		return NULL;
	return Contains[iCounter];
}

SI32 CItem::NextItem( void ) const
{
	iCounter++;
	if( iCounter >= Contains.size() )
		return -1;
	return calcItemFromSer( Contains[iCounter]->GetSerial() );
}

CItem *CItem::NextItemObj( void ) const
{
	iCounter++;
	if( iCounter >= Contains.size() )
		return NULL;
	return Contains[iCounter];
}

bool CItem::FinishedItems( void ) const
{
	return( iCounter >= Contains.size() );
}

int	CItem::NumItems( void ) const
{
	return Contains.size();
}

int CItem::GetItem( int index ) const
{
	if( index >= Contains.size() )
		return -1;
	return calcItemFromSer( Contains[index]->GetSerial() );
}

bool CItem::WillDecay( void ) const
{
	return ( (priv&0x01) == 0x01 );
}

UI08 CItem::IsFieldSpell( void ) const
{
	switch( id )
	{
	case 0x3996:
	case 0x398C:	return 1;// fire
	case 0x3915:
	case 0x3920:	return 2;// poison
	case 0x3979:
	case 0x3967:	return 3;// paralyze
	case 0x3956:
	case 0x3946:	return 4;// energy
	default:		return 0;
	}
}
bool CItem::IsLockedDown( void ) const
{
	return( magic == 3 );
}
bool CItem::IsShieldType( void ) const
{
	if( id >= 0x1B72 && id <= 0x1B7B )
		return true;
	if( id >= 0x1BC3 && id <= 0x1BC5 )
		return true;
	return false;
}

bool CItem::CanBeLockedDown( void ) const
{
	if( type == 12 || type == 13 )	// can't lock down a door
		return false;
	if( IsFieldSpell() )			// can't lock down a field spell
		return false;
	if( id == 0x0BD2 || type == 203 )	// house sign
		return false;
	if( IsLockedDown() )	// can't lock down if already locked down
		return false;
	return true;
}

void CItem::LockDown( void )
{
	magic = 3;
}

bool CItem::Save( ofstream &outStream, int mode )
{
	int itemIndex = -1;
	itemIndex = calcItemFromSer( GetSerial() );	// only if needed to know it

	itemsWritten++;
	if( !isFree() && ( GetCont() != 0xFFFFFFFF || ( GetX() > 0 && GetX() < 6144 && GetY() < 4096 ) ) )
	{
		DumpHeader( outStream, mode );
		DumpBody( outStream, mode );
		if( Contains.size() == 0 )
			DumpFooter( outStream, mode );
	}
	return true;
}

void CItem::RemoveSelfFromOwner( void )
{
	if( GetOwner() != 0xFFFFFFFF )
	{
		CChar *oldOwner = calcCharObjFromSer( GetOwner() );
		if( oldOwner != NULL )
			oldOwner->RemoveOwnedItem( this );
	}
}
void CItem::AddSelfToOwner( void )
{
	if( GetOwner() != 0xFFFFFFFF )
	{
		CChar *newOwner = calcCharObjFromSer( GetOwner() );
		if( newOwner != NULL )
			newOwner->AddOwnedItem( this );
	}
}

void CItem::Dupe( CItem& target )
{
	target.SetAmount( GetAmount() );
	target.SetAmount2( GetAmount2() );
	target.SetArmourClass( GetArmourClass() );
	target.SetCarve( GetCarve() );
	target.SetColour( GetColour() );
	target.SetLayer( GetLayer() );
	target.SetCont( GetCont() );
	target.SetCreator( GetCreator() );
	target.SetCorpse( isCorpse() );
	target.SetDecayTime( GetDecayTime() );
	target.SetDesc( GetDesc() );
	target.SetDoorDir( GetDoorDir() );
	target.SetDexterity( GetDexterity() );
	target.Dexterity2( Dexterity2() );
	target.SetDef( GetDef() );
	target.SetDir( GetDir() );
	target.SetDisabled( IsDisabled() );
	target.SetDoorOpen( isDoorOpen() );
	target.SetDye( isDyeable() );
	target.SetFame( GetFame() );
	target.SetFree( isFree() );
	target.SetGateNumber( GetGateNumber() );
	target.SetGateTime( GetGateTime() );
	target.SetGlow( GetGlow() );
	target.SetGlowColour( GetGlowColour() );
	target.SetGlowEffect( GetGlowEffect() );
	target.SetGood( GetGood() );
	target.SetHiDamage( GetHiDamage() );
	target.SetHP( GetHP() );
	target.SetID( GetID() );
	target.SetIntelligence( GetIntelligence() );
	target.Intelligence2( Intelligence2() );
	target.SetItmHand( GetItmHand() );
	target.SetKarma( GetKarma() );
	target.SetKills( GetKills() );
	target.SetLocation( GetX(), GetY(), GetZ() );
	target.SetLoDamage( GetLoDamage() );
	target.SetMadeWith( GetMadeWith() );
	target.SetMagic( GetMagic() );
	target.SetMana( GetMana() );
	target.SetMaxHP( GetMaxHP() );
	target.SetMore( GetMore() );
	target.SetMoreX( GetMoreX() );
	target.SetMoreY( GetMoreY() );
	target.SetMoreZ( GetMoreZ() );
	target.SetMulti( GetMulti() );
	target.SetMurderer( GetMurderer() );
	target.SetMurderTime( GetMurderTime() );
	target.SetName( GetName() );
	target.SetName2( GetName2() );
	target.SetOffSpell( GetOffSpell() );
	target.SetOwner( GetOwner() );
	target.SetPileable( isPileable() );
	target.SetPoisoned( GetPoisoned() );
	target.SetRace( GetRace() );
	target.SetRank( GetRank() );
	target.SetRestock( GetRestock() );
	target.SetRndValueRate( GetRndValueRate() );
	target.SetScriptTrigger( GetScriptTrigger() );
	target.SetSpawn( GetSpawn(), calcItemFromSer( target.GetSerial() ) );
	target.SetSpeed( GetSpeed() );
	target.SetStamina( GetStamina() );
	target.SetStrength( GetStrength() );
	target.Strength2( Strength2() );
	target.SetTitle( GetTitle() );
	target.SetType( GetType() );
	target.SetType2( GetType2() );
	target.SetValue( GetValue() );
	target.SetVisible( GetVisible() );
	target.SetWeight( GetWeight() );
	target.SetWipeable( isWipeable() );
	target.SetPriv( GetPriv() );

	target.LightDamage( LightDamage() );
	target.RainDamage( RainDamage() );
	target.ColdDamage( ColdDamage() );
	target.HeatDamage( HeatDamage() );
	target.LightningDamage( LightningDamage() );
	target.SnowDamage( SnowDamage() );
}

#undef FALSE
#undef TRUE

const WEATHBITLIST FALSE = 0;
const WEATHBITLIST TRUE  = 1;

bool CItem::WeatherDamage( WeatherType effectNum )
{
	switch( effectNum )
	{
	case LIGHT:	return LightDamage();
	case RAIN:	return RainDamage();
	case COLD:	return ColdDamage();
	case HEAT:	return HeatDamage();
	case LIGHTNING:	return LightningDamage();
	case SNOW:	return SnowDamage();
	default:
		return false;
	}
	return false;
}
bool CItem::LightDamage( void )
{
	return( bLight == 1 );
}
bool CItem::RainDamage( void )
{
	return( bRain == 1 );
}
bool CItem::ColdDamage( void )
{
	return( bCold == 1 );
}
bool CItem::HeatDamage( void )
{
	return( bHeat == 1 );
}
bool CItem::LightningDamage( void )
{
	return( bLightning == 1 );
}
bool CItem::SnowDamage( void )
{
	return( bSnow == 1 );
}

void CItem::LightDamage( bool value )
{
	bLight = (value) ? TRUE : FALSE;
}
void CItem::RainDamage( bool value )
{
	bRain = (value) ? TRUE : FALSE;
}
void CItem::ColdDamage( bool value )
{
	bCold = (value) ? TRUE : FALSE;
}
void CItem::HeatDamage( bool value )
{
	bHeat = (value) ? TRUE : FALSE;
}
void CItem::LightningDamage( bool value )
{
	bLightning = (value) ? TRUE : FALSE;
}
void CItem::SnowDamage( bool value )
{
	bSnow = (value) ? TRUE : FALSE;
}

bool CItem::DumpHeader( ofstream &outStream, int mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
//		LONG zFilePosition = outStream.tellp();
//		outStream << "[" << GetSerial() << "]" << GetObjType() << "[" << zFilePosition << "] - Item Dump" << endl;
		outStream << "[ITEM]" << endl;
		break;
	}
	return true;
}
bool CItem::DumpBody( ofstream &outStream, int mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
		cBaseObject::DumpBody( outStream, mode );

		outStream << "Layer=" << (short)GetLayer() << endl;
		outStream << "Cont=" << GetCont() << endl;
		outStream << "More=" << GetMore() << endl;
		outStream << "More2=" << GetMoreB() << endl;
		outStream << "Name2=" << GetName2() << endl;
		outStream << "Murderer=" << GetMurderer() << endl;
		outStream << "Creater=" << GetCreator() << endl;
		outStream << "Desc=" << GetDesc() << endl;
		outStream << "Type=" << (short)GetType() << endl;
		outStream << "Type2=" << (short)GetType2() << endl;
		outStream << "Offspell=" << (short)GetOffSpell() << endl;
		outStream << "Weight=" << GetWeight() << endl;
		outStream << "MoreX=" << GetMoreX() << endl;
		outStream << "MoreY=" << GetMoreY() << endl;
		outStream << "MoreZ=" << GetMoreZ() << endl;
		outStream << "Amount=" << GetAmount() << endl;
		outStream << "Doorflag=" << (short)GetDoorDir() << endl;
		outStream << "MaxHP=" << GetMaxHP() << endl;
		outStream << "Speed=" << (short)GetSpeed() << endl;
		outStream << "Movable=" << (short)GetMagic() << endl;
//		outStream << "GateTime=" << GetGateTime() << endl;
//		outStream << "DecayTime=" << GetDecayTime() << endl;
//		outStream << "MurderTime=" << GetMurderTime() << endl;
//		outStream << "GateNumber=" << GetGateNumber() << endl;
		outStream << "Priv=" << (short)GetPriv() << endl;
		outStream << "Value=" << GetValue() << endl;
		outStream << "Restock=" << GetRestock() << endl;
		outStream << "Poisoned=" << GetPoisoned() << endl;
		outStream << "AC=" << (short)GetArmourClass() << endl;
		outStream << "Rank=" << (short)GetRank() << endl;
		outStream << "Sk_Made=" << (short)GetMadeWith() << endl;
		outStream << "Enhanced=" << enhanced << endl;
		outStream << "Pileable=" << (int)(isPileable()?1:0) << endl;
		outStream << "Dyeable=" << (int)(isDyeable()?1:0) << endl;
		outStream << "Corpse=" << (int)(isCorpse()?1:0) << endl;
		outStream << "Wipe=" << (int)(isWipeable()?1:0) << endl;
		outStream << "Good=" << GetGood() << endl;
		outStream << "Glow=" << GetGlow() << endl;
		outStream << "GlowBC=" << GetGlowColour() << endl;
		outStream << "GlowType=" << (short)GetGlowEffect() << endl;
		outStream << "Carve=" << GetCarve() << endl;
		outStream << "Light=" << (short)bLight << endl;
		outStream << "Rain=" << (short)bRain << endl;
		outStream << "Heat=" << (short)bHeat << endl;
		outStream << "Cold=" << (short)bCold << endl;
		outStream << "Snow=" << (short)bSnow << endl;
		outStream << "Lightning=" << (short)bLightning << endl;
		outStream << endl << endl;

		if( Contains.size() > 0 )	// only containers will need to dump it
			DumpFooter( outStream, mode );	// This will NOT happen for multis
		for( int tempCounter = 0; tempCounter < Contains.size(); tempCounter++ )
		{
			if( Contains[tempCounter] != NULL )
				Contains[tempCounter]->Save( outStream, mode );
		}
		break;
	}
	return true;
}

bool CItem::DumpFooter( ofstream &outStream, int mode ) const
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

bool CItem::HandleLine( char *tag, char *data )
{
	if( cBaseObject::HandleLine( tag, data ) )
		return true;
	switch( tag[0] )
	{
	case 'A':
		if( !strcmp( tag, "Amount" ) )
		{
			SetAmount( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Amount2" ) )
		{
			SetAmount2( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "AC" ) )
		{
			SetArmourClass( atoi( data ) );
			return true;
		}
		break;

	case 'C':
		if( !strcmp( tag, "Cont" ) )
		{
			SetCont( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Creater" ) )
		{
			SetCreator( data );
			return true;
		}
		else if( !strcmp( tag, "Corpse" ) )
		{
			SetCorpse( atoi( data ) == 1 );
			return true;
		}
		else if( !strcmp( tag, "Carve" ) )
		{
			SetCarve( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Cold" ) )
		{
			ColdDamage( atoi( data ) == 1 );
			return true;
		}
		break;
	case 'D':
		if( !strcmp( tag, "Desc" ) )
		{
			SetDesc( data );
			return true;
		}
		else if( !strcmp( tag, "Doorflag" ) )
		{
			SetDoorDir( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Dyeable" ) )
		{
			SetDye( atoi( data ) == 1 );
			return true;
		}
		break;
	case 'E':
		if( !strcmp( tag, "Enhanced" ) )
		{
			enhanced = atoi( data );
			return true;
		}
		break;
	case 'G':
		if( !strcmp( tag, "GlowType" ) )
		{
			SetGlowEffect( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "GlowBC" ) )
		{
			SetGlowColour( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Glow" ) )
		{
			SetGlow( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Good" ) )
		{
			SetGood( atoi( data ) );
			return true;
		}
		break;
	case 'H':
		if( !strcmp( tag, "Heat" ) )
		{
			HeatDamage( atoi( data ) == 1 );
			return true;
		}
		break;
	case 'I':
		if( !strcmp( tag, "Itemhand" ) )
		{
			SetItmHand( atoi( data ) );
			return true;
		}
		break;
	case 'L':
		if( !strcmp( tag, "Layer" ) )
		{
			SetLayer( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Light" ) )
		{
			LightDamage( atoi( data ) == 1 );
			return true;
		}
		else if( !strcmp( tag, "Lightning" ) )
		{
			LightningDamage( atoi( data ) == 1 );
			return true;
		}
		break;
	case 'M':
		if( !strcmp( tag, "More" ) )
		{
			SetMore( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "More2" ) )
		{
			SetMoreB( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Murderer" ) )
		{
			SetMurderer( data );
			return true;
		}
		else if( !strcmp( tag, "MoreX" ) )
		{
			SetMoreX( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "MoreY" ) )
		{
			SetMoreY( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "MoreZ" ) )
		{
			SetMoreZ( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Movable" ) )
		{
			SetMagic( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "MaxHP" ) )
		{
			SetMaxHP( atoi( data ) );
			return true;
		}
		break;
	case 'N':
		if( !strcmp( tag, "Name2" ) )
		{
			SetName2( data );
			return true;
		}
		break;
	case 'O':
		if( !strcmp( tag, "Offspell" ) )
		{
			SetOffSpell( atoi( data ) );
			return true;
		}
		break;
	case 'P':
		if( !strcmp( tag, "Priv" ) )
		{
			SetPriv( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Poisoned" ) )
		{
			SetPoisoned( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Pileable" ) )
		{
			SetPileable( atoi( data ) == 1 );
			return true;
		}
		break;
	case 'R':
		if( !strcmp( tag, "Restock" ) )
		{
			SetRestock( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Rank" ) )
		{
			SetRank( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Rain" ) )
		{
			RainDamage( atoi( data ) == 1 );
			return true;
		}
		break;
	case 'S':
		if( !strcmp( tag, "Speed" ) )
		{
			SetSpeed( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Sk_Made" ) )
		{
			SetMadeWith( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Snow" ) )
		{
			SnowDamage( atoi( data ) == 1 );
			return true;
		}
		break;
	case 'T':
		if( !strcmp( tag, "Type" ) )
		{
			SetType( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Type2" ) )
		{
			SetType2( atoi( data ) );
			return true;
		}
		break;
	case 'V':
		if( !strcmp( tag, "Value" ) )
		{
			SetValue( atoi( data ) );
			return true;
		}
		break;
	case 'W':
		if( !strcmp( tag, "Weight" ) )
		{
			SetWeight( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Wipe" ) )
		{
			SetWipeable( atoi( data ) == 1 );
			return true;
		}
		break;
	}
	return false;
}

bool CItem::Load( ifstream &inStream, int mode, int arrayOffset )
{
	switch( mode )
	{
	case 1:	break;	// binary
	case 0:
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
	if( itemcount2 <= serial ) 
		itemcount2 = serial + 1;
	nitemsp.AddSerial( serial, arrayOffset );
	// Tauriel adding region pointers

	if( GetCont() == 0xFFFFFFFF )
	{ 
		MapRegion->AddItem( this );
		if( GetX() < 0 || GetY() < 0 || GetX() > 6144 || GetY() > 4096 )
			return false;
	}          
	return true;
}

bool CItem::IsContType( void ) const
{
	if( GetLayer() == 0x15 )
		return true;
	switch( GetType() )
	{
	case 1:
	case 63:
	case 64:
	case 65:
	case 87:	return true;
	default:	return false;
	}
	return false;
}

void CItem::PostLoadProcessing( SI32 index )
{
	cBaseObject::PostLoadProcessing( index );
	// Add item weight if item doesn't have it yet
}

SI08 CItem::SetContSerial( SERIAL newValue )
{
	contserial = newValue;
	return 1;
}

SERIAL CItem::GetContSerial( void )
{
	return contserial;
}

