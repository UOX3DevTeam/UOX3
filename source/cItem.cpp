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
cBaseObject * CItem::GetCont( void ) const
{
	return contObj;
}

SERIAL CItem::GetContSerial( void ) const
{
	if( contObj != NULL )
		return contObj->GetSerial();
	return INVALIDSERIAL;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 GetMore()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the more value of the object
//o--------------------------------------------------------------------------
UI32 CItem::GetMore( void ) const
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
UI32 CItem::GetMoreB( void ) const
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
	SERIAL contserial = INVALIDSERIAL;
	if( contObj != NULL )
		contserial = contObj->GetSerial();

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
UI08 CItem::GetMore( UI08 part ) const
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
UI08 CItem::GetMoreB( UI08 part ) const
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
void CItem::SetSerial( SERIAL newValue, ITEM c )
{
	nitemsp.Remove( newValue );
	cBaseObject::SetSerial( newValue );
	if( c != INVALIDSERIAL && newValue != INVALIDSERIAL )
		nitemsp.AddSerial( newValue, c );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool CItem::SetCont( cBaseObject *newCont )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Set's the item's container value to newValue
//|						Takes it out of the old container, and puts in the new
//|						Copes with being on paperdolls, ground and in containers
//|						Also copes with removing and adding to a map region
//|						Returns false if item needs deleting, true if fine
//o--------------------------------------------------------------------------
bool CItem::SetContSerial( SERIAL newSerial )
{
	if( newSerial != INVALIDSERIAL )
	{
		if( newSerial >= BASEITEMSERIAL )
			return SetCont( calcItemObjFromSer( newSerial ) );
		else
			return SetCont( calcCharObjFromSer( newSerial ) );
	}
	return SetCont( NULL );
}

bool CItem::SetCont( cBaseObject *newCont )
{
	RemoveSelfFromCont();
	contObj = newCont;
	if( newCont != NULL ) 
	{
		if( newCont->GetObjType() == OT_CHAR ) 
		{
			CChar *charWearing = (CChar *)newCont;
			if( charWearing != NULL )
			{
				if( !charWearing->WearItem( this ) )
				{
					if( layer >= MAXLAYERS )
					{
						contObj = NULL;
						return false;
					}
					//return false;	// disable for now
				}
				Weight->addItemWeight( charWearing, this );
				return true;
			}
			else
			{
				contObj = NULL;
				return false;
			}
		}
		else
		{
			CItem *itemHolder = (CItem *)newCont;
			if( itemHolder != NULL )
			{
				// ok heres what hair/beards should be handled like (sereg)
				if( ( GetID() >= 0x203B ) && ( GetID() <= 0x204D ) )
				{
					if( getPackOwner( itemHolder ) != NULL )
					{
						CChar *j = getPackOwner( itemHolder );
						CItem *oldItem = j->GetItemAtLayer( GetLayer() );
						if( oldItem != NULL )
							Items->DeleItem( oldItem );
		
						SetCont( j );
						
						CPRemoveItem toRemove = (*this);

						Network->PushConn();
						for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
							tSock->Send( &toRemove );
						Network->PopConn();
						
						RefreshItem( this );
					}
				}
				else
					itemHolder->HoldItem( this );
				Weight->addItemWeight( itemHolder, this );
				return true;
			}
			else
			{
				contObj = NULL;
				return false;
			}
		}
	}
	else	// this is in case we're initing the default item
		MapRegion->AddItem( this );
	return true;
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
//|	Function		-	void SetOwner( cBaseObject *newValue )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the owner to newValue, ensuring it's removed from
//|						the old owner
//o--------------------------------------------------------------------------
void CItem::SetOwner( cBaseObject *newValue )
{
	RemoveSelfFromOwner();
	cBaseObject::SetOwner( newValue );
	AddSelfToOwner();
}

//o--------------------------------------------------------------------------
//|	Function		-	SetSpawn( SERIAL newValue, ITEM index )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the item's spawner to newValue, ensuring it's
//|						removed from the old hash entry (if any)
//|						Also ensures it is added to the hash table afterwards
//o--------------------------------------------------------------------------
void CItem::SetSpawn( SERIAL newValue, ITEM index )
{
	if( GetSpawn() != INVALIDSERIAL )
		nspawnsp.Remove( GetSpawn(), index );
	cBaseObject::SetSpawn( newValue );
	if( GetSpawn() != INVALIDSERIAL )
		nspawnsp.AddSerial( GetSpawn(), index );
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetMore( UI08 newValue, UI08 part )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the more value's part byte to newValue 
//o--------------------------------------------------------------------------
void CItem::SetMore( UI08 newValue, UI08 part )
{
	UI08 part1 = (UI08)(more>>24);
	UI08 part2 = (UI08)(more>>16);
	UI08 part3 = (UI08)(more>>8);
	UI08 part4 = (UI08)(more%256);
	switch( part )
	{
	case 1:		part1 = newValue;		break;
	case 2:		part2 = newValue;		break;
	case 3:		part3 = newValue;		break;
	case 4:		part4 = newValue;		break;
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
void CItem::SetMoreB( UI08 newValue, UI08 part )
{
	UI08 part1 = (UI08)(moreb>>24);
	UI08 part2 = (UI08)(moreb>>16);
	UI08 part3 = (UI08)(moreb>>8);
	UI08 part4 = (UI08)(moreb%256);
	switch( part )
	{
	case 1:		part1 = newValue;		break;
	case 2:		part2 = newValue;		break;
	case 3:		part3 = newValue;		break;
	case 4:		part4 = newValue;		break;
	}
	moreb = (part1<<24) + (part2<<16) + (part3<<8) + part4;
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
//|	Function		-	bool isFree()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the item is free
//o--------------------------------------------------------------------------
bool CItem::isFree( void ) const
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
bool CItem::isDoorOpen( void ) const
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
bool CItem::isPileable( void ) const
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
bool CItem::isDyeable( void ) const
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
bool CItem::isCorpse( void ) const
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
bool CItem::isWipeable( void ) const
{
	return ( (bools&0x20) == 0x20 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool isGuarded( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the ITEM is guarded
//o---------------------------------------------------------------------------o
bool CItem::isGuarded( void ) const
{
	return ( (bools&0x40) == 0x40 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool isSpawnerList( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the item's spawn points to an NPC list
//|					 instead of an NPC
//o---------------------------------------------------------------------------o
bool CItem::isSpawnerList( void ) const
{
	return ( (bools&0x80) == 0x80 );
}

void CItem::SetFree( bool newValue )
{
	if( newValue )
		bools |= 0x01;
	else
		bools &= ~0x01;
}
void CItem::SetDoorOpen( bool newValue )
{
	if( newValue )
		bools |= 0x02;
	else
		bools &= ~0x02;
}
void CItem::SetPileable( bool newValue )
{
	if( newValue )
		bools |= 0x04;
	else
		bools &= ~0x04;
}
void CItem::SetDye( bool newValue )
{
	if( newValue )
		bools |= 0x08;
	else
		bools &= ~0x08;
}
void CItem::SetCorpse( bool newValue )
{
	if( newValue )
		bools |= 0x10;
	else
		bools &= ~0x10;
}
void CItem::SetWipeable( bool newValue )
{
	if( newValue )
		bools |= 0x20;
	else
		bools &= ~0x20;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetGuarded( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the ITEMs's guarded status
//o---------------------------------------------------------------------------o
void CItem::SetGuarded( bool newValue )
{
	if( newValue )
		bools |= 0x40;
	else
		bools &= ~0x40;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetSpawnerList( bool newVal ) 
//|   Date        -  24th January, 2002
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the item's spawnerlist property
//o---------------------------------------------------------------------------o
void CItem::SetSpawnerList( bool newValue )
{
	if( newValue )
		bools |= 0x80;
	else
		bools &= ~0x80;
}

const char *CItem::GetName2( void ) const
{
	return name2;
}
SERIAL CItem::GetMurderer( void ) const
{
	return murderer;
}
SERIAL CItem::GetCreator( void ) const
{
	return creator;
}
const char *CItem::GetDesc( void ) const
{
	return desc;
}

void CItem::SetName2( const char *newValue )
{
	strncpy( name2, newValue, MAX_NAME );
}
void CItem::SetMurderer( SERIAL newValue )
{
	murderer = newValue;
}
void CItem::SetCreator( SERIAL newValue )
{
	creator = newValue;
}
void CItem::SetDesc( const char *newValue )
{
	strncpy( desc, newValue, 100 );
}

void CItem::IncX( SI16 newValue )
{
	x += newValue;
}
void CItem::IncY( SI16 newValue )
{
	y += newValue;
}
void CItem::IncZ( SI16 newValue )
{
	z += newValue;
}
void CItem::SetLocation( const cBaseObject *toSet )
{
	if( GetCont() == NULL )
		MapRegion->RemoveItem( this );
	x = toSet->GetX();
	y = toSet->GetY();
	z = toSet->GetZ();
	worldNumber = toSet->WorldNumber();
	if( GetCont() == NULL )
	{
		MapRegion->AddItem( this );
		CMultiObj *mMulti = findMulti( this );
		SetMulti( mMulti );
	}
// Thyme - Necessary for Region Spawning with Items to work properly
	if( GetCont() != NULL )
		ShouldSave( true );
}
void CItem::SetLocation( SI16 newX, SI16 newY, SI08 newZ )
{
	if( GetCont() == NULL )
		MapRegion->RemoveItem( this );
	x = newX;
	y = newY;
	z = newZ;
	if( GetCont() == NULL )
	{
		MapRegion->AddItem( this );
		CMultiObj *mMulti = findMulti( this );
		SetMulti( mMulti );
	}
// Thyme - Necessary for Region Spawning with Items to work properly
	if( GetCont() != NULL )
		ShouldSave( true );
}

void CItem::SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world )
{
	if( GetCont() == NULL )
		MapRegion->RemoveItem( this );
	x = newX;
	y = newY;
	z = newZ;
	worldNumber = world;
	if( GetCont() == NULL )
	{
		MapRegion->AddItem( this );
		CMultiObj *mMulti = findMulti( this );
		SetMulti( mMulti );
	}
// Thyme - Necessary for Region Spawning with Items to work properly
	if( GetCont() != NULL )
		ShouldSave( true );
}

UI08 CItem::GetLayer( void ) const
{
	return layer;
}
void CItem::SetLayer( UI08 newValue ) 
{
	if( contObj != NULL && contObj->GetObjType() == OT_CHAR )	// if we're on a char
	{
		CChar *charAffected = (CChar *)contObj;
		if( charAffected != NULL )
		{
//			if( !charAffected->TakeOffItem( GetLayer() ) )
//			{
//				Console.Error( "Char %s(%i) was never wearing item on layer %i\n", charAffected->GetName(), contserial, GetLayer() );
//			}
			if( layer != 0 )
				charAffected->TakeOffItem( layer );
			layer = newValue;
			charAffected->WearItem( this );
			return;
		}
	}
	layer = newValue;
}

UI08 CItem::GetType( void ) const
{
	return type;
}
UI08 CItem::GetType2( void ) const
{
	return type2;
}

void CItem::SetType( UI08 newValue )
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

UI16 CItem::GetAmount( void ) const
{
	return amount;
}
void CItem::SetAmount( UI32 newValue )
{
	cBaseObject *getCont = GetCont();
	if( getCont != NULL )
		Weight->subtractItemWeight( getCont, this );

	if( newValue > MAX_STACK )
		amount = MAX_STACK;
	else
		amount = static_cast<UI16>(newValue);

	if( getCont != NULL )
		Weight->addItemWeight( getCont, this );
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

SI08 CItem::GetMovable( void ) const
{
	return movable;
}
void CItem::SetMovable( SI08 newValue )
{
	movable = newValue;
}

UI32 CItem::GetGateTime( void ) const
{
	return gatetime;
}
UI32 CItem::GetDecayTime( void ) const
{
	return decaytime;
}
UI32 CItem::GetMurderTime( void ) const
{
	return murdertime;
}

void CItem::SetGateTime( UI32 newValue )
{
	gatetime = newValue;
}
void CItem::SetDecayTime( UI32 newValue )
{
	decaytime = newValue;
}
void CItem::SetMurderTime( UI32 newValue )
{
	murdertime = newValue;
}

UI08 CItem::GetPriv( void ) const
{
	return priv;
}
void CItem::SetPriv( UI08 newValue )
{
	priv = newValue;
}

SI32 CItem::GetSellValue( void ) const
{
	return sellValue;
}
void CItem::SetSellValue( SI32 newValue )
{
	sellValue = newValue;
}

SI32 CItem::GetBuyValue( void ) const
{
	return buyValue;
}
void CItem::SetBuyValue( SI32 newValue )
{
	buyValue = newValue;
}

UI16 CItem::GetRestock( void ) const
{
	return restock;
}
void CItem::SetRestock( UI16 newValue )
{
	restock = newValue;
}

UI08 CItem::GetPoisoned( void ) const
{
	return poisoned;
}
void CItem::SetPoisoned( UI08 newValue )
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
	UI08 part1 = (UI08)(glowColour>>8);
	UI08 part2 = (UI08)(glowColour%256);
	switch( part )
	{
	case 1:		part1 = newValue;		break;
	case 2:		part2 = newValue;		break;
	}
	glowColour = (UI16)((part1<<8) + part2);
}
void CItem::SetGlowColourEx( UI08 part1, UI08 part2 )
{
	glowColour = (UI16)((part1<<8) + part2);
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

void CItem::IncHP( SI16 newValue )
{
	hitpoints += newValue;
}

void CItem::IncDef( UI16 newValue )
{
	def += newValue;
}

void CItem::IncID( SI16 incAmount )
{
	id += incAmount;
}

CItem::CItem( ITEM nItem, bool zeroSer ) : 
	contObj( NULL ), remove( false ), carve( -1 ), 	glow_effect( 0 ), glow( -1 ), glowColour( 0 ), madewith( 0 ), 
	rndvaluerate( 0 ), good( -1 ), rank( 0 ), armorClass( 0 ), 	poisoned( 0 ), restock( 0 ), sellValue( 0 ), buyValue( 0 ), priv( 0 ), movable( 0 ), 
	gatetime( 0 ), decaytime( 0 ), murdertime( 0 ), spd( 0 ), 	maxhp( 0 ), doordir( 0 ), amount( 1 ), morex( 0 ),
	morey( 0 ), enhanced( 0 ), weatherBools( 0 ), morez( 0 ), 	more( 0 ), moreb( 0 ), bools( 0 ), layer( 0 ), type( 0 ), 
	type2( 0 ), offspell( 0 ), weight( 0 ), entryMadeFrom( -1 )
{
	if( zeroSer )
	{
		SetSerial( cwmWorldState->GetItemCount2(), nItem );
		cwmWorldState->IncItemCount2();
	} 
	else 
		SetSerial( INVALIDSERIAL, nItem );

	objType = OT_ITEM;
	Contains.resize( 0 );
	strcpy( name2, "#" );
	strcpy( name, "#" );
	murderer = INVALIDSERIAL;
	creator = INVALIDSERIAL;
	desc[0] = 0;
	race = 65535;
}

CItem::~CItem()
{
	// Destructor to clean things up when deleted


	if( !isFree() )	// We're not the default item in the handler
	{
		for( CItem *i = FirstItemObj(); !FinishedItems(); i = NextItemObj() )
		{
			if( i != NULL )
				i->SetCont( NULL );
		}
		RemoveSelfFromCont();
		RemoveSelfFromOwner();
//		MapRegion->RemoveItem( this );
	}
}

bool CItem::HoldItem( CItem *toHold )
{
	for( UI32 counter = 0; counter < Contains.size(); counter++ )
	{
		if( Contains[counter] == toHold )	// same pointer ie same thing
		{
			Console.Warning( 3, "%s(%i) already contains %s(%i)\n", GetName(), GetSerial(), toHold->GetName(), toHold->GetSerial() );
			return false;
		}
	}

	int index = Contains.size();
	Contains.resize( Contains.size() + 1 );
	Contains[index] = toHold;
	return true;
}
bool CItem::ReleaseItem( CItem *index )
{
	for( UI32 counter = 0; counter < Contains.size(); counter++ )
	{
		if( Contains[counter] == index )
		{
			for( int counter2 = counter; counter2 < static_cast<int>((Contains.size()-1)); counter2++ )
				Contains[counter2] = Contains[counter2+1];
			Contains.resize( Contains.size() - 1 );
			if( iCounter >= 0 && static_cast<SI32>(counter) <= iCounter )	// if what we're deleting is in our list, and before the current item
				iCounter--;
			return true;
		}
	}
	return false;
}

ITEM CItem::FirstItem( void ) const
{
	iCounter = 0;
	if( static_cast<unsigned int>(iCounter) >= Contains.size() )
		return INVALIDSERIAL;
	return calcItemFromSer( Contains[iCounter]->GetSerial() );
}

CItem *CItem::FirstItemObj( void ) const
{
	iCounter = 0;
	if( static_cast<unsigned int>(iCounter) >= Contains.size() )
		return NULL;
	return Contains[iCounter];
}

ITEM CItem::NextItem( void ) const
{
	iCounter++;
	if( static_cast<unsigned int>(iCounter) >= Contains.size() )
		return INVALIDSERIAL;
	return calcItemFromSer( Contains[iCounter]->GetSerial() );
}

CItem *CItem::NextItemObj( void ) const
{
	iCounter++;
	if( static_cast<unsigned int>(iCounter) >= Contains.size() )
		return NULL;
	return Contains[iCounter];
}

bool CItem::FinishedItems( void ) const
{
	return( static_cast<unsigned int>(iCounter) >= Contains.size() );
}

UI32 CItem::NumItems( void ) const
{
	return Contains.size();
}

CItem *CItem::GetItemObj( ITEM index ) const
{
	if( index >= Contains.size() )
		return NULL;
	return Contains[index];
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
	return( movable == 3 );
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
	movable = 3;
}

bool CItem::Save( std::ofstream &outStream, SI32 mode )
{
	if( isFree() )
		return false;
	CMultiObj *multi = NULL;
	cSocket *k = NULL;

	if( WillDecay() && GetCont() == NULL )
	{
		if( GetDecayTime() == 0 )
			SetDecayTime( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetSystemTimerStatus( DECAY )) ) );
		else if( GetDecayTime() < cwmWorldState->GetUICurrentTime() )
		{
			if( isCorpse() )
			{	// Adjust this to work with the native pointer array
				for( CItem *j = FirstItemObj(); !FinishedItems(); j = NextItemObj() )
				{
					if( j != NULL && !j->isFree() )
					{
						if( j->GetLayer() != 0x0B && j->GetLayer() != 0x10 )
						{
							j->SetCont( NULL );
							j->SetLocation( this );
							Network->PushConn();
							for( k = Network->FirstSocket(); !Network->FinishedSockets(); k = Network->NextSocket() )
							{
								if( itemInRange( k->CurrcharObj(), j ) )
									sendItem( k, j );
							}
							Network->PopConn();
						}
						else
							Items->DeleItem( j );
					}
				}
			}
			else
			{
				if( GetMultiObj() == NULL )
				{
					multi = findMulti( this );
					if( multi == NULL )
					{
						Items->DeleItem( this );
						return false;
					}
				}
			}
		}
	}

	if( GetCont() != NULL || ( GetX() > 0 && GetX() < 6144 && GetY() < 4096 ) )
	{
		DumpHeader( outStream, mode );
		if( mode == 1 )
		{
			BinBuffer buff;

			DumpBody( buff );
			FilePosition = outStream.tellp();
			buff.Write( outStream );
		} 
		else 
			DumpBody( outStream, mode );

		DumpFooter( outStream, mode );

		for( UI32 tempCounter = 0; tempCounter < Contains.size(); tempCounter++ )
		{
			if( Contains[tempCounter] != NULL && Contains[tempCounter]->ShouldSave() )
				Contains[tempCounter]->Save( outStream, mode );
		}
	}
	return true;
}

void CItem::RemoveSelfFromOwner( void )
{
	cBaseObject *oldOwner = GetOwnerObj();
	if( oldOwner != NULL )
	{
		if( oldOwner->GetObjType() == OT_CHAR )
			((CChar *)oldOwner)->RemoveOwnedItem( this );
	}
}
void CItem::AddSelfToOwner( void )
{
	cBaseObject *newOwner = GetOwnerObj();
	if( newOwner == NULL )
		return;
	if( newOwner->GetSerial() != GetSerial() )
	{
		if( newOwner->GetObjType() == OT_CHAR )
			((CChar *)newOwner)->AddOwnedItem( this );
	}
}

void CItem::RemoveSelfFromCont( void )
{
	if( contObj != NULL )
	{
		if( contObj->GetObjType() == OT_CHAR )	// it's a char!
		{
			CChar *targChar = (CChar *)contObj;
			if( targChar != NULL )
			{
				Weight->subtractItemWeight( targChar, this );
				targChar->TakeOffItem( GetLayer() );
//				if( !targChar->TakeOffItem( GetLayer() ) )
//					Console << "Char " << targChar->GetName() << "(" << contserial << ") was never wearing item on layer " << (SI16)GetLayer() << myendl;
			}
		} 
		else 
		{
			CItem *targItem = (CItem *)contObj;;
			if( targItem != NULL )
			{
				Weight->subtractItemWeight( targItem, this );
				targItem->ReleaseItem( this );
//				if( !targItem->ReleaseItem( this ) )
//					Console.Error( "Error removing %s(%i) from %s(%i)\n", GetName(), GetSerial(), targItem->GetName(), contserial );
			}
		}
	}
	else 
		MapRegion->RemoveItem( this );
}

CItem * CItem::Dupe( void )
{
	ITEM targetOff;
	CItem *target = Items->MemItemFree( targetOff, true );
	if( target == NULL )
		return NULL;

	target->SetAmount( GetAmount() );
	target->SetArmourClass( GetArmourClass() );
	target->SetCarve( GetCarve() );
	target->SetColour( GetColour() );
	target->SetLayer( GetLayer() );
	target->SetCont( GetCont() );
	target->SetCreator( GetCreator() );
	target->SetCorpse( isCorpse() );
	target->SetDecayTime( GetDecayTime() );
	target->SetDesc( GetDesc() );
	target->SetDoorDir( GetDoorDir() );
	target->SetDexterity( GetDexterity() );
	target->Dexterity2( Dexterity2() );
	target->SetDef( GetDef() );
	target->SetDir( GetDir() );
	target->SetDisabled( IsDisabled() );
	target->SetDoorOpen( isDoorOpen() );
	target->SetDye( isDyeable() );
	target->SetFame( GetFame() );
	target->SetFree( isFree() );
	target->SetGateTime( GetGateTime() );
	target->SetGlow( GetGlow() );
	target->SetGlowColour( GetGlowColour() );
	target->SetGlowEffect( GetGlowEffect() );
	target->SetGood( GetGood() );
	target->SetHiDamage( GetHiDamage() );
	target->SetHP( GetHP() );
	target->SetID( GetID() );
	target->SetIntelligence( GetIntelligence() );
	target->Intelligence2( Intelligence2() );
	target->SetKarma( GetKarma() );
	target->SetKills( GetKills() );
	target->SetLocation( this );
	target->SetLoDamage( GetLoDamage() );
	target->SetMadeWith( GetMadeWith() );
	target->SetMovable( GetMovable() );
	target->SetMana( GetMana() );
	target->SetMaxHP( GetMaxHP() );
	target->SetMore( GetMore() );
	target->SetMoreX( GetMoreX() );
	target->SetMoreY( GetMoreY() );
	target->SetMoreZ( GetMoreZ() );
	target->SetMulti( GetMultiObj() );
	target->SetMurderer( GetMurderer() );
	target->SetMurderTime( GetMurderTime() );
	target->SetName( GetName() );
	target->SetName2( GetName2() );
	target->SetOffSpell( GetOffSpell() );
	target->SetOwner( GetOwnerObj() );
	target->SetPileable( isPileable() );
	target->SetPoisoned( GetPoisoned() );
	target->SetRace( GetRace() );
	target->SetRank( GetRank() );
	target->SetRestock( GetRestock() );
	target->SetRndValueRate( GetRndValueRate() );
	target->SetScriptTrigger( GetScriptTrigger() );
	target->SetSpawn( GetSpawn(), targetOff );
	target->SetSpeed( GetSpeed() );
	target->SetStamina( GetStamina() );
	target->SetStrength( GetStrength() );
	target->Strength2( Strength2() );
	target->SetTitle( GetTitle() );
	target->SetType( GetType() );
	target->SetType2( GetType2() );
	target->SetBuyValue( GetBuyValue() );
	target->SetSellValue( GetSellValue() );
	target->SetVisible( GetVisible() );
	target->SetWeight( GetWeight() );
	target->SetWipeable( isWipeable() );
	target->SetPriv( GetPriv() );

	target->LightDamage( LightDamage() );
	target->RainDamage( RainDamage() );
	target->ColdDamage( ColdDamage() );
	target->HeatDamage( HeatDamage() );
	target->LightningDamage( LightningDamage() );
	target->SnowDamage( SnowDamage() );

	return target;
}

bool CItem::WeatherDamage( WeatherType effectNum ) const
{
	switch( effectNum )
	{
	case LIGHT:			return LightDamage();
	case RAIN:			return RainDamage();
	case COLD:			return ColdDamage();
	case HEAT:			return HeatDamage();
	case LIGHTNING:		return LightningDamage();
	case SNOW:			return SnowDamage();
	default:			return false;
	}
	return false;
}
bool CItem::LightDamage( void ) const
{
	return ( (weatherBools&0x01) == 0x01 );
}
bool CItem::RainDamage( void ) const
{
	return ( (weatherBools&0x02) == 0x02 );
}
bool CItem::ColdDamage( void ) const
{
	return ( (weatherBools&0x04) == 0x04 );
}
bool CItem::HeatDamage( void ) const
{
	return ( (weatherBools&0x08) == 0x08 );
}
bool CItem::LightningDamage( void ) const
{
	return ( (weatherBools&0x10) == 0x10 );
}
bool CItem::SnowDamage( void ) const
{
	return ( (weatherBools&0x20) == 0x20 );
}

void CItem::LightDamage( bool value )
{
	if( value )
		weatherBools |= 0x01;
	else
		weatherBools &= ~0x01;
}
void CItem::RainDamage( bool value )
{
	if( value )
		weatherBools |= 0x02;
	else
		weatherBools &= ~0x02;
}
void CItem::ColdDamage( bool value )
{
	if( value )
		weatherBools |= 0x04;
	else
		weatherBools &= ~0x04;
}
void CItem::HeatDamage( bool value )
{
	if( value )
		weatherBools |= 0x08;
	else
		weatherBools &= ~0x08;
}
void CItem::LightningDamage( bool value )
{
	if( value )
		weatherBools |= 0x10;
	else
		weatherBools &= ~0x10;
}
void CItem::SnowDamage( bool value )
{
	if( value )
		weatherBools |= 0x20;
	else
		weatherBools &= ~0x20;
}

bool CItem::DumpHeader( std::ofstream &outStream, SI32 mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
		outStream << "[ITEM]" << std::endl;
		break;
	}
	return true;
}

bool CItem::DumpBody( BinBuffer &buff ) const
{
	buff.SetType( 2 );
	if( !cBaseObject::DumpBody( buff ) )
		return false;
	
	if( DefItem->GetLayer() != GetLayer() || DefItem->GetCont() != GetCont() )
	{
		buff.PutByte( ITEMTAG_LAYERCONT );
		buff.PutByte( GetLayer() );
		buff.PutLong( GetContSerial() );
	}
	
	if( DefItem->GetMore() != GetMore() || DefItem->GetMoreB() != GetMoreB() )
	{
		buff.PutByte( ITEMTAG_MORE );
		buff.PutLong( GetMore() );
		buff.PutLong( GetMoreB() );
	}
	
	if( strcmp( DefItem->GetName2(), GetName2() ) )
	{
		buff.PutByte( ITEMTAG_NAME2 );
		buff.PutStr( GetName2() );
	}
	
	if( DefItem->GetMurderer() != GetMurderer() && GetMurderer() != 0 )
	{
		buff.PutByte( ITEMTAG_MURDERER );
		buff.PutLong( GetMurderer() );
	}
	
	if( DefItem->GetCreator() != GetCreator() && GetCreator() != 0 )
	{
		buff.PutByte( ITEMTAG_CREATOR );
		buff.PutLong( GetCreator() );
	}
	
	if( strcmp( DefItem->GetDesc(), GetDesc() ) )
	{
		buff.PutByte( ITEMTAG_DESC );
		buff.PutStr( GetDesc() );
	}
	
	if( DefItem->GetType() != GetType() || DefItem->GetType2() != GetType2() )
	{
		buff.PutByte( ITEMTAG_TYPE );
		buff.PutByte( GetType() );
		buff.PutByte( GetType2() );
	}
	
	if( DefItem->GetOffSpell() != GetOffSpell() )
	{
		buff.PutByte( ITEMTAG_OFFSPELL );
		buff.PutByte( GetOffSpell() );
	}
	
	if( DefItem->GetWeight() != GetWeight() )
	{
		buff.PutByte( ITEMTAG_WEIGHT );
		buff.PutLong( GetWeight() );
	}
	
	if( DefItem->GetMoreX() != GetMoreX() || DefItem->GetMoreY() != GetMoreY() || DefItem->GetMoreZ() != GetMoreZ() )
	{
		buff.PutByte( ITEMTAG_MOREXYZ );
		buff.PutLong( GetMoreX() );
		buff.PutLong( GetMoreY() );
		buff.PutLong( GetMoreZ() );
	}
	
	if( DefItem->GetAmount() != GetAmount() )
	{
		buff.PutByte( ITEMTAG_AMOUNT );
		if( GetAmount() > MAX_STACK )
			buff.PutLong( MAX_STACK );
		else
			buff.PutLong( GetAmount() );
	}
	
	if( DefItem->bools != bools )
	{
		buff.PutByte( ITEMTAG_BOOLS );
		buff.PutByte( bools );
	}
	
	if( DefItem->GetMaxHP() != GetMaxHP() )
	{
		buff.PutByte( ITEMTAG_MAXHP );
		buff.PutShort( GetMaxHP() );
	}
	
	if( DefItem->GetSpeed() != GetSpeed() || DefItem->GetMovable() != GetMovable() || DefItem->GetPriv() != GetPriv() )
	{
		buff.PutByte( ITEMTAG_SPD_MG_PV );
		buff.PutByte( GetSpeed() );
		buff.PutByte( GetMovable() );
		buff.PutByte( GetPriv() );
	}
	
	if( DefItem->GetBuyValue() != GetBuyValue() || DefItem->GetSellValue() != GetSellValue() || DefItem->GetRestock() != GetRestock() )
	{
		buff.PutByte( ITEMTAG_VALUE );
		buff.PutLong( GetBuyValue() );
		buff.PutLong( GetSellValue() );
		buff.PutLong( GetRestock() );
	}
	
	if( DefItem->GetPoisoned() != GetPoisoned() )
	{
		buff.PutByte( ITEMTAG_POISONED );
		buff.PutByte( GetPoisoned() );
	}
	
	if( DefItem->GetArmourClass() != GetArmourClass() || DefItem->GetRank() != GetRank() )
	{
		buff.PutByte( ITEMTAG_AC );
		buff.PutByte( GetArmourClass() );
		buff.PutByte( GetRank() );
	}
	
	if( DefItem->GetMadeWith() != GetMadeWith() )
	{
		buff.PutByte( ITEMTAG_MADEENH );
		buff.PutByte( GetMadeWith() );
		buff.PutLong( enhanced );
	}
	
	if( DefItem->GetGood() != GetGood() )
	{
		buff.PutByte( ITEMTAG_GOOD );
		buff.PutLong( GetGood() );
	}
	
	if( DefItem->GetGlow() != GetGlow() || DefItem->GetGlowColour() != GetGlowColour() || DefItem->GetGlowEffect() != GetGlowEffect() )
	{
		buff.PutByte( ITEMTAG_GLOW );
		buff.PutLong( GetGlow() );
		buff.PutShort( GetGlowColour() );
		buff.PutByte( GetGlowEffect() );
	}
	
	if( DefItem->GetCarve() != GetCarve() )
	{
		buff.PutByte( ITEMTAG_CARVE );
		buff.PutLong( GetCarve() );
	}
	
	if( DefItem->weatherBools != weatherBools )
	{
		buff.PutByte( ITEMTAG_WEATHER );
		buff.PutByte( weatherBools );
	}
	
	if( DefItem->entryMadeFrom != entryMadeFrom )
	{
		buff.PutByte( ITEMTAG_ENTRY );
		buff.PutLong( entryMadeFrom );
	}

	if( DefItem->GetDoorDir() != GetDoorDir() )
	{
		buff.PutByte( ITEMTAG_DOORDIR );
		buff.PutByte( GetDoorDir() );
	}
	
	return true;
}

bool CItem::DumpBody( std::ofstream &outStream, SI32 mode ) const
{
	std::string destination; 
	std::ostringstream dumping( destination ); 
	
	cBaseObject::DumpBody( outStream, mode );
	dumping << "Layer=" << (SI16)GetLayer() << std::endl;
	dumping << "Cont=" << GetContSerial() << std::endl;
	dumping << "More=" << GetMore() << "," << GetMoreB() << std::endl;
	dumping << "Name2=" << GetName2() << std::endl;
	dumping << "Murderer=" << GetMurderer() << std::endl;
	dumping << "Creator=" << GetCreator() << std::endl;
	dumping << "Desc=" << GetDesc() << std::endl;
	dumping << "Type=" << (SI16)GetType() << "," << (SI16)GetType2() << std::endl;
	dumping << "Offspell=" << (SI16)GetOffSpell() << std::endl;
	dumping << "Weight=" << GetWeight() << std::endl;
	dumping << "MoreXYZ=" << GetMoreX() << "," << GetMoreY() << "," << GetMoreZ() << std::endl;
	dumping << "Amount=";
	if( GetAmount() > MAX_STACK )
		dumping << MAX_STACK << std::endl;
	else
		dumping << GetAmount() << std::endl;
	dumping << "Doorflag=" << (SI16)GetDoorDir() << std::endl;
	dumping << "MaxHP=" << GetMaxHP() << std::endl;
	dumping << "Speed=" << (SI16)GetSpeed() << std::endl;
	dumping << "Movable=" << (SI16)GetMovable() << std::endl;
	dumping << "Priv=" << (SI16)GetPriv() << std::endl;
	dumping << "Value=" << GetBuyValue() << "," << GetSellValue() << std::endl;
	dumping << "Restock=" << GetRestock() << std::endl;
	dumping << "Poisoned=" << (SI16)GetPoisoned() << std::endl;
	dumping << "AC=" << (SI16)GetArmourClass() << std::endl;
	dumping << "Rank=" << (SI16)GetRank() << std::endl;
	dumping << "Sk_Made=" << (SI16)GetMadeWith() << std::endl;
	dumping << "Enhanced=" << enhanced << std::endl;
	dumping << "Pileable=" << (SI16)(isPileable()?1:0) << std::endl;
	dumping << "Dyeable=" << (SI16)(isDyeable()?1:0) << std::endl;
	dumping << "Corpse=" << (SI16)(isCorpse()?1:0) << std::endl;
	dumping << "Wipe=" << (SI16)(isWipeable()?1:0) << std::endl;
	dumping << "Good=" << GetGood() << std::endl;
	dumping << "Glow=" << GetGlow() << std::endl;
	dumping << "GlowBC=" << GetGlowColour() << std::endl;
	dumping << "GlowType=" << (SI16)GetGlowEffect() << std::endl;
	dumping << "Carve=" << GetCarve() << std::endl;
	dumping << "RaceDamage=" << (SI16)(LightDamage() ? 1 : 0) << "," << (SI16)(RainDamage() ? 1 : 0) << ","
		<< (SI16)(HeatDamage() ? 1 : 0) << "," << (SI16)(ColdDamage() ? 1 : 0) << "," << (SI16)(SnowDamage() ? 1 : 0) << "," << (SI16)(LightningDamage() ? 1 : 0) << std::endl;
	dumping << "EntryMadeFrom=" << entryMadeFrom << std::endl;
	
	//dumping << std::endl << std::endl;
	outStream << dumping.str();
	
	return true;
}

bool CItem::DumpFooter( std::ofstream &outStream, SI32 mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
		outStream << std::endl << "o---o" << std::endl << std::endl;
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
			amount = (UI16)makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "AC" ) )
		{
			SetArmourClass( (UI08)makeNum( data ) );
			return true;
		}
		break;

	case 'C':
		if( !strcmp( tag, "Cont" ) )
		{
			contObj = (cBaseObject *)makeNum( data);
			return true;
		}
		else if( !strcmp( tag, "Creator" ) || !strcmp(tag,"Creater") )
		{
			SetCreator( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Corpse" ) )
		{
			SetCorpse( makeNum( data ) == 1 );
			return true;
		}
		else if( !strcmp( tag, "Carve" ) )
		{
			SetCarve( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Cold" ) )
		{
			ColdDamage( makeNum( data ) == 1 );
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
			SetDoorDir( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Dyeable" ) )
		{
			SetDye( makeNum( data ) == 1 );
			return true;
		}
		break;
	case 'E':
		if( !strcmp( tag, "Enhanced" ) )
		{
			enhanced = makeNum( data );
			return true;
		}
		else if( !strcmp( tag, "EntryMadeFrom" ) )
		{
			entryMadeFrom = makeNum( data );
			return true;
		}
		break;
	case 'G':
		if( !strcmp( tag, "GlowType" ) )
		{
			SetGlowEffect( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "GlowBC" ) )
		{
			SetGlowColour( (UI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Glow" ) )
		{
			SetGlow( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Good" ) )
		{
			SetGood( makeNum( data ) );
			return true;
		}
		break;
	case 'H':
		if( !strcmp( tag, "Heat" ) )
		{
			HeatDamage( makeNum( data ) == 1 );
			return true;
		}
		break;
	case 'L':
		if( !strcmp( tag, "Layer" ) )
		{
			SetLayer( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Light" ) )
		{
			LightDamage( makeNum( data ) == 1 );
			return true;
		}
		else if( !strcmp( tag, "Lightning" ) )
		{
			LightningDamage( makeNum( data ) == 1 );
			return true;
		}
		break;
	case 'M':
		if( !strcmp( tag, "More" ) )
		{
			char *more2Off = strstr( data, "," );
			if( more2Off != NULL )
			{
				char tmp[32];
				strncpy( tmp, data, more2Off - data );
				tmp[more2Off - data] = 0;
				SetMore( makeNum( tmp ) );
				SetMoreB( makeNum( more2Off + 1 ) );
			}
			else
				SetMore( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "More2" ) )
		{
			SetMoreB( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Murderer" ) )
		{
			SetMurderer( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "MoreXYZ" ) )
		{
			char *moreyOff = strstr( data, "," );
			char *morezOff = strstr( moreyOff+1, "," );
			char tmp[32];
			strncpy( tmp, data, moreyOff - data );
			tmp[moreyOff - data] = 0;
			SetMoreX( (SI16)makeNum( tmp ) );
			strncpy( tmp, moreyOff + 1, morezOff - moreyOff - 1 );
			tmp[morezOff - moreyOff - 1] = 0;
			SetMoreY( (SI16)makeNum( tmp ) );
			SetMoreZ( (SI08)makeNum( morezOff + 1 ) );
			return true;
		}
		else if( !strcmp( tag, "MoreX" ) )
		{
			SetMoreX( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "MoreY" ) )
		{
			SetMoreY( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "MoreZ" ) )
		{
			SetMoreZ( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Movable" ) )
		{
			SetMovable( (SI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "MaxHP" ) )
		{
			SetMaxHP( (SI16)makeNum( data ) );
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
			SetOffSpell( (SI08)makeNum( data ) );
			return true;
		}
		break;
	case 'P':
		if( !strcmp( tag, "Priv" ) )
		{
			SetPriv( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Poisoned" ) )
		{
			SetPoisoned( static_cast<UI08>(makeNum( data ) ));
			return true;
		}
		else if( !strcmp( tag, "Pileable" ) )
		{
			SetPileable( makeNum( data ) == 1 );
			return true;
		}
		break;
	case 'R':
		if( !strcmp( tag, "Restock" ) )
		{
			SetRestock( static_cast<UI16>(makeNum( data ) ));
			return true;
		}
		else if( !strcmp( tag, "RaceDamage" ) )
		{
			char *rainOff = strstr( data, "," );
			char *heatOff = strstr( rainOff+1, "," );
			char *coldOff = strstr( heatOff+1, "," );
			char *snowOff = strstr( coldOff+1, "," );
			char *lightningOff = strstr( snowOff+1, "," );
			char tmp[32];
			strncpy( tmp, data, rainOff - data );
			tmp[rainOff - data] = 0;
			LightDamage( makeNum( tmp ) == 1 );

			strncpy( tmp, rainOff + 1, heatOff - rainOff - 1 );
			tmp[heatOff - rainOff - 1] = 0;
			RainDamage( makeNum( tmp ) == 1 );

			strncpy( tmp, heatOff + 1, coldOff - heatOff - 1 );
			tmp[coldOff - heatOff - 1] = 0;
			HeatDamage( makeNum( tmp ) == 1 );

			strncpy( tmp, coldOff + 1, snowOff - coldOff - 1 );
			tmp[snowOff - coldOff - 1] = 0;
			ColdDamage( makeNum( tmp ) == 1 );

			strncpy( tmp, snowOff + 1, lightningOff - snowOff - 1 );
			tmp[lightningOff - snowOff - 1] = 0;
			SnowDamage( makeNum( tmp ) == 1 );

			LightningDamage( makeNum( lightningOff + 1 ) == 1 );
			return true;	
		}
		else if( !strcmp( tag, "Rank" ) )
		{
			SetRank( (SI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Rain" ) )
		{
			RainDamage( makeNum( data ) == 1 );
			return true;
		}
		break;
	case 'S':
		if( !strcmp( tag, "Speed" ) )
		{
			SetSpeed( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Sk_Made" ) )
		{
			SetMadeWith( (SI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Snow" ) )
		{
			SnowDamage( makeNum( data ) == 1 );
			return true;
		}
		break;
	case 'T':
		if( !strcmp( tag, "Type" ) )
		{
			char *type2Off = strstr( data, "," );
			if( type2Off != NULL )
			{
				char tmp[32];
				strncpy( tmp, data, type2Off - data );
				tmp[type2Off - data] = 0;
				SetType( (UI08)makeNum( tmp ) );
				SetType2( (UI08)makeNum( type2Off + 1 ) );
			}
			else
				SetType( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Type2" ) )
		{
			SetType2( (UI08)makeNum( data ) );
			return true;
		}
		break;
	case 'V':
		if( !strcmp( tag, "Value" ) )
		{
			char *sellValueOff = strstr( data, "," );
			if( sellValueOff != NULL )
			{
				char tmp[32];
				strncpy( tmp, data, sellValueOff - data );
				tmp[sellValueOff - data] = 0;
				SetBuyValue( (UI08)makeNum( tmp ) );
				SetSellValue( (UI08)makeNum( sellValueOff + 1 ) );
			}
			else
			{
				SetBuyValue( makeNum( data ) );
				SetSellValue( (makeNum( data ) / 2) );
			}
			return true;
		}
		break;
	case 'W':
		if( !strcmp( tag, "Weight" ) )
		{
			SetWeight( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Wipe" ) )
		{
			SetWipeable( makeNum( data ) == 1 );
			return true;
		}
		break;
	}
	return false;
}

bool CItem::HandleBinTag( UI08 tag, BinBuffer &buff )
{
	if( cBaseObject::HandleBinTag( tag, buff ) )
		return true;

	switch ( tag )
	{
	case ITEMTAG_LAYERCONT:
		SetLayer( buff.GetByte() );
		SetContSerial( buff.GetLong() );
		break;

	case ITEMTAG_MORE:
		SetMore( buff.GetLong() );
		SetMoreB( buff.GetLong() );
		break;

	case ITEMTAG_NAME2:
		buff.GetStr( name2, MAX_NAME );
		break;

	case ITEMTAG_MURDERER:
		SetMurderer( buff.GetLong() );
		break;

	case ITEMTAG_CREATOR:
		SetCreator( buff.GetLong() );
		break;

	case ITEMTAG_DESC:
		buff.GetStr( desc, 100 );
		break;

	case ITEMTAG_TYPE:
		SetType( buff.GetByte() );
		SetType2( buff.GetByte() );
		break;

	case ITEMTAG_OFFSPELL:
		SetOffSpell( buff.GetByte() );
		break;

	case ITEMTAG_WEIGHT:
		SetWeight( buff.GetLong() );
		break;

	case ITEMTAG_MOREXYZ:
		SetMoreX( buff.GetLong() );
		SetMoreY( buff.GetLong() );
		SetMoreZ( buff.GetLong() );
		break;

	case ITEMTAG_AMOUNT:
		amount = buff.GetShort();
		break;

	case ITEMTAG_BOOLS:
		bools = buff.GetByte();
		break;

	case ITEMTAG_MAXHP:
		SetMaxHP( buff.GetShort() );
		break;

	case ITEMTAG_SPD_MG_PV:
		SetSpeed( buff.GetByte() );
		SetMovable( buff.GetByte() );
		SetPriv( buff.GetByte() );
		break;

	case ITEMTAG_VALUE:
		SetSellValue( buff.GetLong() );
		SetBuyValue( buff.GetLong() );
		SetRestock( static_cast<UI16>(buff.GetLong()) );
		break;

	case ITEMTAG_POISONED:
		SetPoisoned( buff.GetByte() );
		break;

	case ITEMTAG_AC:
		SetArmourClass( buff.GetByte() );
		SetRank( buff.GetByte() );
		break;

	case ITEMTAG_MADEENH:
		SetMadeWith( buff.GetByte() );
		enhanced = buff.GetLong();
		break;

	case ITEMTAG_GOOD:
		SetGood( buff.GetLong() );
		break;

	case ITEMTAG_GLOW:
		SetGlow( buff.GetLong() );
		SetGlowColour( buff.GetShort() );
		SetGlowEffect( buff.GetByte() );
		break;

	case ITEMTAG_CARVE:
		SetCarve( buff.GetLong() );
		break;

	case ITEMTAG_WEATHER:
		weatherBools = buff.GetByte();
		break;

	case ITEMTAG_ENTRY:
		entryMadeFrom = buff.GetLong();
		break;

	case ITEMTAG_DOORDIR:
		SetDoorDir( buff.GetByte() );
		break;

	default:
		return false;
	}

	return true;
}

bool CItem::Load( BinBuffer &buff, ITEM arrayOffset )
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
				Console.Warning( 2, "Unknown CItem world file tag %i [%x] length of %i (Last Tag %i[%x])", tag, tag, ItemSaveTable[tag], ltag, ltag );
		}
	}

	// We have to do this here, otherwise we return prematurely, WITHOUT having read the entire record!
//	if( isDirty )
//		return false;
	return LoadRemnants( arrayOffset );
}

bool CItem::Load( std::ifstream &inStream, ITEM arrayOffset )
{
	char tag[128], data[512];
	bool bFinished;
	bFinished = false;
	
	while( !bFinished )
	{
		ReadWorldTagData( inStream, tag, data );
		bFinished = ( strcmp( tag, "o---o" ) == 0 ) || inStream.eof() || inStream.fail();
		if( !bFinished )
		{
			if( !HandleLine( tag, data ) )
			{
				Console.Warning( 2, "Unknown world file tag %s with contents of %s", tag, data );
			}
		}
	}
	return LoadRemnants( arrayOffset );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool LoadRemnants( int arrayOffset )
//|	Date			-	21st January, 2002
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	After handling data specific load, other parts go here
//o--------------------------------------------------------------------------
bool CItem::LoadRemnants( int arrayOffset )
{
	if( cwmWorldState->GetItemCount2() <= serial ) 
		cwmWorldState->SetItemCount2( serial + 1 );
	SetSerial( serial, arrayOffset );
	
	// Tauriel adding region pointers
	if( (SERIAL)contObj == INVALIDSERIAL )
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
	case 8:
	case 63:
	case 64:
	case 65:
	case 87:	return true;
	default:	return false;
	}
	return false;
}

void CItem::PostLoadProcessing( UI32 index )
{
	cBaseObject::PostLoadProcessing( index );
	// Add item weight if item doesn't have it yet
	if( GetWeight() < 0 || GetWeight() > MAX_WEIGHT )
		SetWeight( Weight->calcWeight( this ) );
	SERIAL tempSerial = (SERIAL)contObj;
	contObj = NULL;
	if( tempSerial != INVALIDSERIAL )
	{
		if( tempSerial >= BASEITEMSERIAL )
			contObj = calcItemObjFromSer( tempSerial );
		else
			contObj = calcCharObjFromSer( tempSerial );
	}
	SetCont( contObj );
	SetAmount( amount );
	Items->StoreItemRandomValue( this, 0xFF );
}

bool CItem::isDecayable( void ) const
{
	return ( (priv&0x01) == 0x01 );
}
bool CItem::isNewbie( void ) const
{
	return ( (priv&0x02) == 0x02 );
}
bool CItem::isDispellable( void ) const
{
	return ( (priv&0x04) == 0x04 );
}

void CItem::SetDecayable( bool newValue )
{
	if( newValue )
		priv |= 0x01;
	else
		priv &= ~0x01;
}
void CItem::SetNewbie( bool newValue )
{
	if( newValue )
		priv |= 0x02;
	else
		priv &= ~0x02;
}
void CItem::SetDispellable( bool newValue )
{
	if( newValue )
		priv |= 0x04;
	else
		priv &= ~0x04;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI32 EntryMadeFrom()
//|	Date			-	13 September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the create.scp entry the item was made from
//o--------------------------------------------------------------------------
SI32 CItem::EntryMadeFrom( void ) const
{
	return entryMadeFrom;
}

//o--------------------------------------------------------------------------
//|	Function		-	void EntryMadeFrom( SI32 newValue )
//|	Date			-	13 September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the create.scp entry the item was made from
//o--------------------------------------------------------------------------
void CItem::EntryMadeFrom( SI32 newValue )
{
	entryMadeFrom = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool IsMetalType( void ) const
//|	Date			-	13 November, 2001
//|	Programmer		-	Bel-CMC
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if it's a metal object
//o--------------------------------------------------------------------------
bool CItem::IsMetalType( void ) const
{
	if( id >= 0x1B72 && id <= 0x1B7B )
		return true;
	else if( id >= 0x1BC3 && id <= 0x1BC5 )
		return true;
	else if( id >= 0x13AF && id <= 0x13B0 )
		return true;
	else if( id >= 0x13B5 && id <= 0x13C4 )
		return true;
	else if( id >= 0x13E5 && id <= 0x13F2 )
		return true;
	else if( id >= 0x13FA && id <= 0x13FB )
		return true;
	else if( id >= 0x13FE && id <= 0x141A )
		return true;
	else if( id >= 0x1438 && id <= 0x1443 )
		return true;
	else if( id >= 0x0F43 && id <= 0x0F4E )
		return true;
	else if( id >= 0x0F51 && id <= 0x0F52 )
		return true;
	else if( id >= 0x0F5C && id <= 0x0F63 )
		return true;
	else 
		return false;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool IsLeatherType( void ) const
//|	Date			-	13 November, 2001
//|	Programmer		-	Bel-CMC
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if it's a leather object
//o--------------------------------------------------------------------------
bool CItem::IsLeatherType( void ) const
{
	if( id >= 0x13C6 && id <= 0x13E2 )
		return true;
	if( id >= 0x144E && id <= 0x1457 )
		return true;
	return false;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool IsBowType( void ) const
//|	Date			-	13 November, 2001
//|	Programmer		-	Bel-CMC
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if it's a bow object
//o--------------------------------------------------------------------------
bool CItem::IsBowType( void ) const
{
	if( id >= 0x0F4F && id <= 0x0F50 )
		return true;
	if( id >= 0x13B1 && id <= 0x13B2 )
		return true;
	if( id >= 0x13FC && id <= 0x13FD )
		return true;
	return false;
}

int compare( const void *arg1, const void *arg2 )
{
	CItem *a = *( CItem ** )arg1;
	CItem *b = *( CItem ** )arg2;
	if( a == NULL )
		return 1;
	else if( b == NULL )
		return -1;
	if( a == b )
		return 0;
	if( a->GetSerial() < b->GetSerial() )
		return -1;
	else if( a->GetSerial() > b->GetSerial() )
		return 1;
	else
		return 0;
}

void CItem::Sort( void )
{
	if( Contains.size() < 2 )
		return;
	qsort( (void *)&Contains[0], (size_t)Contains.size(), sizeof( CItem * ), compare );
	UI16 baseY = 0, baseX = 0;
	switch( GetLayer() )
	{
	case 0x1A:			// buy layer
		break;
	case 0x1B:			// bought layer
		baseY = 100;
		break;
	default:	return;
	}
	for( UI16 i = 0; i < Contains.size(); ++i )
	{
		if( Contains[i] == NULL )
			continue;
		Contains[i]->SetX( baseX++ );
		Contains[i]->SetY( baseY );
		if( baseX == 200 )
		{
			baseX = 0;
			++baseY;
		}
	}
}
