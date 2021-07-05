//o-----------------------------------------------------------------------------------------------o
//|	File			-	cItem.cpp
//|	Date			-	2nd April, 2000
//o-----------------------------------------------------------------------------------------------o
//| Purpose			-	New class written based upon old UOX item_st.  Number of old members removed
//|						and a number of members types modified as well
//|
//|	Version History -
//|
//|						1.0		 		2nd April, 2000
//|						Initial implementation
//|						Bools reduced down to a single short with get/set mechanisms for setting/clearings bits
//|						No documentation currently done, all functions obvious so far
//|						Note:  individual byte setting within longs/shorts need speeding up
//|						consider memcpy into correct word, note that this will be endian specific!
//|
//|						1.1		 		23rd July, 2000
//|						Addition of SaveItem routine for text mode only
//|
//|						1.2		 		25th July, 2000
//|						All Accessor funcs plus a few others are now flagged as const functions, meaning that
//|						those functions GUARENTEE never to alter data, at compile time
//|						Thoughts about CBaseObject and prelim plans made
//|
//|						1.3		 		28th July, 2000
//|						CBaseObject initial implementation plus rework of CItem to deal with only what it needs to
//|						Proper constructor written
//|						Plans for CItem derived classes thought upon
//|
//|						1.4		 		17th July, 2004
//|						Added CSpawnItem to begin breaking CItem into smaller subclasses
//o-----------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "weight.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "ObjectFactory.h"
#include "speech.h"
#include "cRaces.h"
#include "cSpawnRegion.h"
#include "CJSEngine.h"
#include "CJSMapping.h"
#include "Dictionary.h"
#include "msgboard.h"
#include "books.h"
#include "power.h"
#include "StringUtility.hpp"

const UI32 BIT_DOOROPEN		=	1;
const UI32 BIT_PILEABLE		=	2;
const UI32 BIT_DYEABLE		=	3;
const UI32 BIT_CORPSE		=	4;
const UI32 BIT_HELDONCURSOR	=	5;
const UI32 BIT_GUARDED		=	6;
const UI32 BIT_SPAWNERLIST	=	7;

const SI32			DEFITEM_RANDVALUE		= 0;
const SI16			DEFITEM_GOOD			= -1;
const SI08			DEFITEM_RANK			= 0;
const UI16			DEFITEM_RESTOCK			= 0;
const SI08			DEFITEM_MOVEABLE		= 0;
const TIMERVAL		DEFITEM_DECAYTIME		= 0;
const ItemLayers	DEFITEM_LAYER			= IL_NONE;
const ItemTypes		DEFITEM_TYPE			= IT_NOTYPE;
const UI16			DEFITEM_ENTRYMADEFROM	= 0;
const UI16			DEFITEM_AMOUNT			= 1;
const UI08			DEFITEM_GLOWEFFECT		= 0;
const SERIAL		DEFITEM_GLOW			= INVALIDSERIAL;
const COLOUR		DEFITEM_GLOWCOLOUR		= INVALIDCOLOUR;
const SI08			DEFITEM_MADEWITH		= 0;
const ARMORCLASS	DEFITEM_ARMORCLASS		= 0;
const TIMERVAL		DEFITEM_TEMPTIMER		= 0;
const UI08			DEFITEM_SPEED			= 0;
const UI16			DEFITEM_MAXHP			= 0;
const SI08			DEFITEM_OFFSPELL		= 0;
const SI08			DEFITEM_GRIDLOC			= 0;
const SERIAL		DEFITEM_CREATOR			= INVALIDSERIAL;
const SI32			DEFITEM_WEIGHTMAX		= 0;
const SI32			DEFITEM_BASEWEIGHT		= 0;
const UI16			DEFITEM_MAXITEMS		= 0;
const UI08			DEFITEM_MAXRANGE		= 0;
const UI08			DEFITEM_BASERANGE		= 0;

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Constructor
//o-----------------------------------------------------------------------------------------------o
CItem::CItem() : CBaseObject(),
contObj( nullptr ), glow_effect( DEFITEM_GLOWEFFECT ), glow( DEFITEM_GLOW ), glowColour( DEFITEM_GLOWCOLOUR ),
madewith( DEFITEM_MADEWITH ), rndvaluerate( DEFITEM_RANDVALUE ), good( DEFITEM_GOOD ), rank( DEFITEM_RANK ), armorClass( DEFITEM_ARMORCLASS ),
restock( DEFITEM_RESTOCK ), movable( DEFITEM_MOVEABLE ), tempTimer( DEFITEM_TEMPTIMER ), decaytime( DEFITEM_DECAYTIME ),
spd( DEFITEM_SPEED ), maxhp( DEFITEM_MAXHP ), amount( DEFITEM_AMOUNT ),
layer( DEFITEM_LAYER ), type( DEFITEM_TYPE ), offspell( DEFITEM_OFFSPELL ), entryMadeFrom( DEFITEM_ENTRYMADEFROM ),
creator( DEFITEM_CREATOR ), gridLoc( DEFITEM_GRIDLOC ), weightMax( DEFITEM_WEIGHTMAX ), baseWeight( DEFITEM_BASEWEIGHT ), maxItems( DEFITEM_MAXITEMS ),
maxRange( DEFITEM_MAXRANGE ), baseRange( DEFITEM_BASERANGE )
{
	spells[0] = spells[1] = spells[2] = 0;
	value[0] = value[1] = 0;
	ammo[0] = ammo[1] = 0;
	ammoFX[0] = ammoFX[1] = ammoFX[2] = 0;
	objType		= OT_ITEM;
	strcpy( name2, "#" );
	name		= "#";
	race		= 65535;
	memset( tempVars, 0, sizeof( tempVars[0] ) * CITV_COUNT );
	desc.reserve( MAX_NAME );
	id			= 0x0000;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	~CItem()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Destructor to clean things up when deleted
//o-----------------------------------------------------------------------------------------------o
CItem::~CItem()
{
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject * GetCont( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the container object
//o-----------------------------------------------------------------------------------------------o
CBaseObject * CItem::GetCont( void ) const
{
	return contObj;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetContSerial( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the container's serial
//o-----------------------------------------------------------------------------------------------o
SERIAL CItem::GetContSerial( void ) const
{
	if( contObj != nullptr )
		return contObj->GetSerial();
	return INVALIDSERIAL;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 GetGridLocation( void ) const
//|					void SetGridLocation( SI08 newLoc )
//|	Date		-	7-23-2007
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets "Grid Location" - for UOKR
//o-----------------------------------------------------------------------------------------------o
SI08 CItem::GetGridLocation( void ) const
{
	return gridLoc;
}
void CItem::SetGridLocation( SI08 newLoc )
{
	gridLoc = newLoc;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetTempVar( CITempVars whichVar ) const
//|					void SetTempVar( CITempVars whichVar, UI32 newVal )
//|	Date		-	7/6/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the temp value of the object
//o-----------------------------------------------------------------------------------------------o
UI32 CItem::GetTempVar( CITempVars whichVar ) const
{
	if( whichVar == CITV_COUNT )
		return 0;

	return tempVars[whichVar];
}
void CItem::SetTempVar( CITempVars whichVar, UI32 newVal )
{
	if( whichVar == CITV_COUNT )
		return;

	tempVars[whichVar] = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetTempVar( CITempVars whichVar, UI08 part ) const
//|					void SetTempVar( CITempVars whichVar, UI08 part, UI08 newVal )
//|	Date		-	7/6/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets one of the words of the temp value
//|	Notes		-	Valid values for part are 1->4.  If outside that, behaves as if it were 1
//o-----------------------------------------------------------------------------------------------o
UI08 CItem::GetTempVar( CITempVars whichVar, UI08 part ) const
{
	UI08 rVal = 0;

	if( whichVar != CITV_COUNT )
	{

		switch( part )
		{
			default:
			case 1:		rVal = static_cast<UI08>(tempVars[whichVar]>>24);	break;
			case 2:		rVal = static_cast<UI08>(tempVars[whichVar]>>16);	break;
			case 3:		rVal = static_cast<UI08>(tempVars[whichVar]>>8);	break;
			case 4:		rVal = static_cast<UI08>(tempVars[whichVar]%256);	break;
		}
	}
	return rVal;
}
void CItem::SetTempVar( CITempVars whichVar, UI08 part, UI08 newVal )
{
	if( whichVar == CITV_COUNT )
		return;

	UI08 part1 = static_cast<UI08>(tempVars[whichVar]>>24);
	UI08 part2 = static_cast<UI08>(tempVars[whichVar]>>16);
	UI08 part3 = static_cast<UI08>(tempVars[whichVar]>>8);
	UI08 part4 = static_cast<UI08>(tempVars[whichVar]%256);

	switch( part )
	{
		default:
		case 1:		part1 = newVal;		break;
		case 2:		part2 = newVal;		break;
		case 3:		part3 = newVal;		break;
		case 4:		part4 = newVal;		break;
	}
	tempVars[whichVar] = (part1<<24) + (part2<<16) + (part3<<8) + part4;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool SetCont( CBaseObject *newCont )
//|					bool SetContSerial( SERIAL newSerial )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Set's the item's container value to newValue
//|						Takes it out of the old container, and puts in the new
//|						Copes with being on paperdolls, ground and in containers
//|						Also copes with removing and adding to a map region
//|						Returns false if item needs deleting, true if fine
//o-----------------------------------------------------------------------------------------------o
bool CItem::SetContSerial( SERIAL newSerial )
{
	if( newSerial != INVALIDSERIAL )
	{
		if( newSerial >= BASEITEMSERIAL )
			return SetCont( calcItemObjFromSer( newSerial ) );
		else
			return SetCont( calcCharObjFromSer( newSerial ) );
	}
	return SetCont( nullptr );
}
bool CItem::SetCont( CBaseObject *newCont )
{
	if( isPostLoaded() )
	{
		Dirty( UT_UPDATE );
		RemoveSelfFromCont();
	}

	bool contIsGround = true;
	if( ValidateObject( newCont ) )
	{
		contObj = newCont;
		if( newCont->GetObjType() == OT_CHAR )
		{
			CChar *charWearing = static_cast<CChar *>(newCont);
			if( ValidateObject( charWearing ))
			{
				if( charWearing->IsDead() && ( this->GetLayer() == IL_HAIR || this->GetLayer() == IL_FACIALHAIR ))
				{
					// if charWaring is dead, it means we're setting the cont on a duped item soon-to-be-moved to his corpse,
					// so we don't want him to attempt to wear it.
					contIsGround = false;
					if( isPostLoaded() )
					{
						Weight->addItemWeight( charWearing, this );
					}
				}
				else if( charWearing->WearItem( this ) )
				{
					contIsGround = false;
					if( isPostLoaded() )
						Weight->addItemWeight( charWearing, this );
					if( this->GetLayer() == IL_MOUNT && charWearing->IsNpc() )
						charWearing->SetOnHorse( true );
				}
			}
		}
		else
		{
			CItem *itemHolder = static_cast<CItem *>(newCont);
			if( itemHolder != nullptr )
			{
				contIsGround = false;
				// ok heres what hair/beards should be handled like
				if( ( ( GetLayer() == IL_HAIR ) || ( GetLayer() == IL_FACIALHAIR ) ) && !itemHolder->isCorpse() )
				{
					CChar *itemPackOwner = FindItemOwner( itemHolder );
					if( ValidateObject( itemPackOwner ) )
					{
						CItem *oldItem = itemPackOwner->GetItemAtLayer( GetLayer() );
						if( ValidateObject( oldItem ) )
							oldItem->Delete();

						SetCont( itemPackOwner );
					}
				}
				else
					itemHolder->GetContainsList()->Add( this );
				if( isPostLoaded() )
					Weight->addItemWeight( itemHolder, this );
			}
		}
	}

	if( contIsGround )
	{
		contObj = nullptr;
		MapRegion->AddItem( this );
	}

	if( GetGlow() != INVALIDSERIAL )
		Items->GlowItem( this );

	return !contIsGround;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isDoorOpen( void ) const
//|					void SetDoorOpen( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets door's open state
//o-----------------------------------------------------------------------------------------------o
bool CItem::isDoorOpen( void ) const
{
	return bools.test( BIT_DOOROPEN );
}
void CItem::SetDoorOpen( bool newValue )
{
	bools.set( BIT_DOOROPEN, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isPileable( void ) const
//|					void SetPileable( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's pileable state
//o-----------------------------------------------------------------------------------------------o
bool CItem::isPileable( void ) const
{
	return bools.test( BIT_PILEABLE );
}
void CItem::SetPileable( bool newValue )
{
	bools.set( BIT_PILEABLE, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isDyeable( void ) const
//|					void SetDye( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's dyeable state
//o-----------------------------------------------------------------------------------------------o
bool CItem::isDyeable( void ) const
{
	return bools.test( BIT_DYEABLE );
}
void CItem::SetDye( bool newValue )
{
	bools.set( BIT_DYEABLE, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isCorpse( void ) const
//|					void SetCorpse( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's corpse state
//o-----------------------------------------------------------------------------------------------o
bool CItem::isCorpse( void ) const
{
	return bools.test( BIT_CORPSE );
}
void CItem::SetCorpse( bool newValue )
{
	bools.set( BIT_CORPSE, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isHeldOnCursor( void ) const
//|					void SetHeldOnCursor( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether item is being held on cursor by a player
//o-----------------------------------------------------------------------------------------------o
bool CItem::isHeldOnCursor( void ) const
{
	return bools.test( BIT_HELDONCURSOR );
}
void CItem::SetHeldOnCursor( bool newValue )
{
	bools.set( BIT_HELDONCURSOR, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isGuarded( void ) const
//|					void SetGuarded( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's guarded state
//o-----------------------------------------------------------------------------------------------o
bool CItem::isGuarded( void ) const
{
	return bools.test( BIT_GUARDED );
}
void CItem::SetGuarded( bool newValue )
{
	bools.set( BIT_GUARDED, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isSpawnerList( void ) const
//|					void SetSpawnerList( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether spawner points to a a NPCLIST/ITEMLIST instead of an NPC/Item
//o-----------------------------------------------------------------------------------------------o
bool CItem::isSpawnerList( void ) const
{
	return bools.test( BIT_SPAWNERLIST );
}
void CItem::SetSpawnerList( bool newValue )
{
	bools.set( BIT_SPAWNERLIST, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	const char *GetName2( void ) const
//|					void SetName2( const char *newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's name2 property - used for magical items
//o-----------------------------------------------------------------------------------------------o
const char *CItem::GetName2( void ) const
{
	return name2;
}
void CItem::SetName2( const char *newValue )
{
	strncpy( name2, newValue, MAX_NAME - 1 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetCreator( void ) const
//|					void SetCreator( SERIAL newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets serial of item's creator - used for crafting and maker's marks
//o-----------------------------------------------------------------------------------------------o
SERIAL CItem::GetCreator( void ) const
{
	return creator;
}
void CItem::SetCreator( SERIAL newValue )
{
	creator = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string GetDesc( void ) const
//|					void SetDesc( std::string newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's description property - used for playervendors, among other things
//o-----------------------------------------------------------------------------------------------o
std::string CItem::GetDesc( void ) const
{
	return desc;
}
void CItem::SetDesc( std::string newValue )
{
	desc = newValue.substr( 0, MAX_NAME - 1 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void IncZ( SI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Increases item's Z height by specified value
//o-----------------------------------------------------------------------------------------------o
void CItem::IncZ( SI16 newValue )
{
	SetZ( z + newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void IncLocation( SI16 xInc, SI16 yInc )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Increases item's X and Y coordinates by specified values
//o-----------------------------------------------------------------------------------------------o
void CItem::IncLocation( SI16 xInc, SI16 yInc )
{
	SetLocation( (GetX() + xInc), (GetY() + yInc), GetZ(), GetGridLocation(), WorldNumber(), GetInstanceID() );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetOldLocation( SI16 newX, SI16 newY, SI08 newZ )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates item's old location
//o-----------------------------------------------------------------------------------------------o
void CItem::SetOldLocation( SI16 newX, SI16 newY, SI08 newZ )
{
	oldLocX = newX;
	oldLocY = newY;
	oldLocZ = newZ;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetLocation( const CBaseObject *toSet )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets item's new location to match specified object's location
//o-----------------------------------------------------------------------------------------------o
void CItem::SetLocation( const CBaseObject *toSet )
{
	if( toSet == nullptr )
		return;
	SetLocation( toSet->GetX(), toSet->GetY(), toSet->GetZ(), toSet->WorldNumber(), toSet->GetInstanceID() );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetLocation( SI16 newX, SI16 newY, SI08 newZ )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets item's new location to match specified coordinates
//o-----------------------------------------------------------------------------------------------o
void CItem::SetLocation( SI16 newX, SI16 newY, SI08 newZ )
{
	SetLocation( newX, newY, newZ, GetGridLocation(), WorldNumber(), GetInstanceID() );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetLocation( SI16 newX, SI16 newY, SI08 newZ, SI08 newLoc, UI08 world, UI16 instance_id )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets item's new location to match specified coordinates, grid location, world and instance
//o-----------------------------------------------------------------------------------------------o
void CItem::SetLocation( SI16 newX, SI16 newY, SI08 newZ, SI08 newLoc, UI08 world, UI16 instance_id )
{
	if( GetCont() == nullptr )
		MapRegion->ChangeRegion( this, newX, newY, world );
	oldLocX = x;
	oldLocY = y;
	oldLocZ = z;
	x = newX;
	y = newY;
	z = newZ;
	gridLoc = newLoc;
	worldNumber = world;
	instanceID = instance_id;
	if( GetCont() == nullptr )
	{
		if( !CanBeObjType( OT_MULTI ) )
		{
			// If it's a sign with a more value, assume that the more value contains the serial of the multi the sign belongs to
			if((( id >= 0x0b95 && id <= 0x0c0e ) || id == 0x1f28 || id == 0x1f29 ) && GetTempVar( CITV_MORE ) != 0 )
			{
				CMultiObj *mMulti = calcMultiFromSer( GetTempVar( CITV_MORE ) );
				if( ValidateObject( mMulti ) )
					SetMulti( mMulti );
			}
			else
			{
				CMultiObj *mMulti = findMulti( newX, newY, newZ, world, instance_id );
				if( GetMultiObj() != mMulti )
					SetMulti( mMulti );
			}
		}
	}
	Dirty( UT_LOCATION );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world, UI16 instanceID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets item's new location to match specified coordinates, world and instance
//o-----------------------------------------------------------------------------------------------o
void CItem::SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world, UI16 instanceID )
{
	SetLocation( newX, newY, newZ, GetGridLocation(), world, instanceID );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	ItemLayers GetLayer( void ) const
//|					void SetLayer( ItemLayers newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's layer property
//o-----------------------------------------------------------------------------------------------o
ItemLayers CItem::GetLayer( void ) const
{
	return layer;
}
void CItem::SetLayer( ItemLayers newValue )
{
	CBaseObject *getCont = GetCont();
	if( ValidateObject( getCont ) && getCont->GetObjType() == OT_CHAR )	// if we're on a char
	{
		CChar *charAffected = static_cast<CChar *>(getCont);
		if( ValidateObject( charAffected ) )
		{
			if( layer != IL_NONE )
				charAffected->TakeOffItem( layer );
			layer = newValue;
			charAffected->WearItem( this );
			return;
		}
	}
	layer = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	ItemTypes GetType( void ) const
//|					void SetType( ItemTypes newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's type property
//o-----------------------------------------------------------------------------------------------o
ItemTypes CItem::GetType( void ) const
{
	return type;
}
void CItem::SetType( ItemTypes newValue )
{
	type = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 GetOffSpell( void ) const
//|					void SetOffSpell( SI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's offspell property - Unused?
//o-----------------------------------------------------------------------------------------------o
SI08 CItem::GetOffSpell( void ) const
{
	return offspell;
}
void CItem::SetOffSpell( SI08 newValue )
{
	offspell = newValue;
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetAmount( void ) const
//|					void SetAmount( UI32 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's amount property, adjusts weight according to new value
//o-----------------------------------------------------------------------------------------------o
UI16 CItem::GetAmount( void ) const
{
	return amount;
}
void CItem::SetAmount( UI32 newValue )
{
	CBaseObject *getCont = GetCont();
	if( ValidateObject( getCont ) )
		Weight->subtractItemWeight( getCont, this );

	if( newValue > MAX_STACK )
		amount = MAX_STACK;
	else
		amount = static_cast<UI16>(newValue);

	if( ValidateObject( getCont ) )
		Weight->addItemWeight( getCont, this );
	Dirty( UT_UPDATE );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool :IncAmount( SI32 incValue, bool noDelete )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Increases item's amount property by specified amount,
//|					adjusts weight according to new value
//o-----------------------------------------------------------------------------------------------o
bool CItem::IncAmount( SI32 incValue, bool noDelete )
{
	bool iDeleted = false;
	SI32 newAmt = (amount + incValue);
	if( newAmt >= 0 )
	{
		if( newAmt > MAX_STACK )
			SetAmount( MAX_STACK );
		else
			SetAmount( static_cast< UI16 >( newAmt ) );
	}
	if( !noDelete && newAmt <= 0)
	{
		Delete();
		iDeleted = true;
	}
	return iDeleted;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetMaxHP( void ) const
//|					void SetMaxHP( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's max hp property
//o-----------------------------------------------------------------------------------------------o
UI16 CItem::GetMaxHP( void ) const
{
	return maxhp;
}
void CItem::SetMaxHP( UI16 newValue )
{
	maxhp = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetSpeed( void ) const
//|					void SetSpeed( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's speed property - used to determine weapon attack speed
//o-----------------------------------------------------------------------------------------------o
UI08 CItem::GetSpeed( void ) const
{
	return spd;
}
void CItem::SetSpeed( UI08 newValue )
{
	spd = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetMaxRange( void ) const
//|					void SetMaxRange( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's maximum range - primarily used by throwing/archery weapons
//o-----------------------------------------------------------------------------------------------o
UI08 CItem::GetMaxRange( void ) const
{
	return maxRange;
}
void CItem::SetMaxRange( UI08 newValue )
{
	maxRange = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetBaseRange( void ) const
//|					void SetBaseRange( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's base range - primarily used by throwing weapons
//o-----------------------------------------------------------------------------------------------o
UI08 CItem::GetBaseRange( void ) const
{
	return baseRange;
}
void CItem::SetBaseRange( UI08 newValue )
{
	baseRange = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 GetMovable( void ) const
//|					void SetMovable( SI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's movable property
//o-----------------------------------------------------------------------------------------------o
SI08 CItem::GetMovable( void ) const
{
	return movable;
}
void CItem::SetMovable( SI08 newValue )
{
	movable = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	TIMERVAL GetTempTimer( void ) const
//|					void SetTempTimer( TIMERVAL newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's temptimer
//o-----------------------------------------------------------------------------------------------o
TIMERVAL CItem::GetTempTimer( void ) const
{
	return tempTimer;
}
void CItem::SetTempTimer( TIMERVAL newValue )
{
	tempTimer = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	TIMERVAL GetDecayTime( void ) const
//|					void SetDecayTime( TIMERVAL newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's decay timer
//o-----------------------------------------------------------------------------------------------o
TIMERVAL CItem::GetDecayTime( void ) const
{
	return decaytime;
}
void CItem::SetDecayTime( TIMERVAL newValue )
{
	decaytime = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetPriv( void ) const
//|					void SetPriv( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets priv settings on item
//o-----------------------------------------------------------------------------------------------o
UI08 CItem::GetPriv( void ) const
{
	return static_cast< UI08 >(priv.to_ulong());
}
void CItem::SetPriv( UI08 newValue )
{
	priv = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetSellValue( void ) const
//|					void SetSellValue( UI32 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the item's sell value - determines how much player can sell it for
//o-----------------------------------------------------------------------------------------------o
UI32 CItem::GetSellValue( void ) const
{
	return value[1];
}
void CItem::SetSellValue( UI32 newValue )
{
	value[1] = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetBuyValue( void ) const
//|					void SetBuyValue( UI32 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's buy value - determines how much player must pay for it
//o-----------------------------------------------------------------------------------------------o
UI32 CItem::GetBuyValue( void ) const
{
	return value[0];
}
void CItem::SetBuyValue( UI32 newValue )
{
	value[0] = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetRestock( void ) const
//|					void SetRestock( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's restock value in vendor sellpack
//o-----------------------------------------------------------------------------------------------o
UI16 CItem::GetRestock( void ) const
{
	return restock;
}
void CItem::SetRestock( UI16 newValue )
{
	restock = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	ARMORCLASS GetArmourClass( void ) const
//|					void SetArmourClass( ARMORCLASS newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's armour class
//o-----------------------------------------------------------------------------------------------o
ARMORCLASS CItem::GetArmourClass( void ) const
{
	return armorClass;
}
void CItem::SetArmourClass( ARMORCLASS newValue )
{
	armorClass = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 GetRank( void ) const
//|					void SetRank( SI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's rank value - used in crafting
//o-----------------------------------------------------------------------------------------------o
SI08 CItem::GetRank( void ) const
{
	return rank;
}
void CItem::SetRank( SI08 newValue )
{
	rank = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 GetGood( void ) const
//|					void SetGood( SI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's good value - used by advanced trade system
//o-----------------------------------------------------------------------------------------------o
SI16 CItem::GetGood( void ) const
{
	return good;
}
void CItem::SetGood( SI16 newValue )
{
	good = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 GetRndValueRate( void ) const
//|					void SetRndValueRate( SI32 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's random value rate - used by advanced trade system
//o-----------------------------------------------------------------------------------------------o
SI32 CItem::GetRndValueRate( void ) const
{
	return rndvaluerate;
}
void CItem::SetRndValueRate( SI32 newValue )
{
	rndvaluerate = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 GetMadeWith( void ) const
//|					void SetMadeWith( SI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets ID of skill used to create item
//o-----------------------------------------------------------------------------------------------o
SI08 CItem::GetMadeWith( void ) const
{
	return madewith;
}
void CItem::SetMadeWith( SI08 newValue )
{
	madewith = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetGlow( void ) const
//|					void SetGlow( SERIAL newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets serial of glow item attached to item
//o-----------------------------------------------------------------------------------------------o
SERIAL CItem::GetGlow( void ) const
{
	return glow;
}
void CItem::SetGlow( SERIAL newValue )
{
	glow = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	COLOUR GetGlowColour( void ) const
//|					void SetGlowColour( COLOUR newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets colour of glow item attached to item
//o-----------------------------------------------------------------------------------------------o
COLOUR CItem::GetGlowColour( void ) const
{
	return glowColour;
}
void CItem::SetGlowColour( COLOUR newValue )
{
	glowColour = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetGlowEffect( void ) const
//|					void SetGlowEffect( UI08 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets colour of glow item attached to item
//o-----------------------------------------------------------------------------------------------o
UI08 CItem::GetGlowEffect( void ) const
{
	return glow_effect;
}
void CItem::SetGlowEffect( UI08 newValue )
{
	glow_effect = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetAmmoID( void ) const
//|					void SetAmmoID( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets ammo ID for ammo used by ranged weapon
//o-----------------------------------------------------------------------------------------------o
UI16 CItem::GetAmmoID( void ) const
{
	return ammo[0];
}
void CItem::SetAmmoID( UI16 newValue )
{
	ammo[0] = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetAmmoHue( void ) const
//|					void SetAmmoHue( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets hue of ammo used by ranged weapon
//o-----------------------------------------------------------------------------------------------o
UI16 CItem::GetAmmoHue( void ) const
{
	return ammo[1];
}
void CItem::SetAmmoHue( UI16 newValue )
{
	ammo[1] = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetAmmoFX( void ) const
//|					void SetAmmoFX( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets projectile FX shown when firing ranged weapon
//o-----------------------------------------------------------------------------------------------o
UI16 CItem::GetAmmoFX( void ) const
{
	return ammoFX[0];
}
void CItem::SetAmmoFX( UI16 newValue )
{
	ammoFX[0] = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetAmmoFXHue( UI16 newValue )
//|					void SetAmmoFXHue( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets colour of projectiles fired by ranged weapons
//o-----------------------------------------------------------------------------------------------o
UI16 CItem::GetAmmoFXHue( void ) const
{
	return ammoFX[1];
}
void CItem::SetAmmoFXHue( UI16 newValue )
{
	ammoFX[1] = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetAmmoFXRender( void ) const
//|					void SetAmmoFXRender( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the render mode of projectile FX played when firing ranged weapons
//o-----------------------------------------------------------------------------------------------o
UI16 CItem::GetAmmoFXRender( void ) const
{
	return ammoFX[2];
}
void CItem::SetAmmoFXRender( UI16 newValue )
{
	ammoFX[2] = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 GetWeightMax( void ) const
//|					void SetWeightMax( SI32 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum weight (in stones) a container can hold
//o-----------------------------------------------------------------------------------------------o
SI32 CItem::GetWeightMax( void ) const
{
	return weightMax;
}
void CItem::SetWeightMax( SI32 newValue )
{
	weightMax = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 GetBaseWeight( void ) const
//|					void SetBaseWeight( SI32 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets base item weight - primarily used to store original container weight
//o-----------------------------------------------------------------------------------------------o
SI32 CItem::GetBaseWeight( void ) const
{
	return baseWeight;
}
void CItem::SetBaseWeight( SI32 newValue )
{
	baseWeight = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 GetMaxItems( void ) const
//|					void SetMaxItems( UI16 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum amount of items a container can hold
//o-----------------------------------------------------------------------------------------------o
UI16 CItem::GetMaxItems( void ) const
{
	return maxItems;
}
void CItem::SetMaxItems( UI16 newValue )
{
	maxItems = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 IsFieldSpell( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns whether item belongs to a field spell - and if so - which one
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanBeLockedDown( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether item can be locked down
//o-----------------------------------------------------------------------------------------------o
bool CItem::CanBeLockedDown( void ) const
{
	if( type == IT_DOOR || type == IT_LOCKEDDOOR )	// can't lock down a door
		return false;
	if( IsFieldSpell() )			// can't lock down a field spell
		return false;
	if( id == 0x0BD2 || type == IT_HOUSESIGN )	// house sign
		return false;
	if( IsLockedDown() )	// can't lock down if already locked down
		return false;
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsLockedDown( void ) const
//|					void LockDown( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether item is locked down and immovable
//o-----------------------------------------------------------------------------------------------o
bool CItem::IsLockedDown( void ) const
{
	return ( movable == 3 );
}
void CItem::LockDown( void )
{
	movable = 3;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsShieldType( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether item is a shield
//o-----------------------------------------------------------------------------------------------o
bool CItem::IsShieldType( void ) const
{
	if( id >= 0x1B72 && id <= 0x1B7B )
		return true;
	if( id >= 0x1BC3 && id <= 0x1BC7 )
		return true;
	if( id >= 0x4200 && id <= 0x420B )
		return true;
	if( id >= 0x4228 && id <= 0x422C )
		return true;
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool Save( std::ofstream &outStream )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Save item details to worldfile
//o-----------------------------------------------------------------------------------------------o
bool CItem::Save( std::ofstream &outStream )
{
	if( isFree() )
		return false;
	MapData_st& mMap = Map->GetMapData( worldNumber );
	if( GetCont() != nullptr || ( GetX() > 0 && GetX() < mMap.xBlock && GetY() < mMap.yBlock ) )
	{
		DumpHeader( outStream );
		DumpBody( outStream );
		DumpFooter( outStream );

		for( CItem *toSave = Contains.First(); !Contains.Finished(); toSave = Contains.Next() )
		{
			if( ValidateObject( toSave ) && toSave->ShouldSave() )
				toSave->Save( outStream );
		}
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveSelfFromOwner( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove item (self) from owner it belongs to
//o-----------------------------------------------------------------------------------------------o
void CItem::RemoveSelfFromOwner( void )
{
	CChar *oldOwner = GetOwnerObj();
	if( oldOwner != nullptr )
		oldOwner->RemoveOwnedItem( this );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddSelfToOwner( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Add item (self) to owner
//o-----------------------------------------------------------------------------------------------o
void CItem::AddSelfToOwner( void )
{
	CChar *newOwner = GetOwnerObj();
	if( !ValidateObject( newOwner ) )
		return;
	if( newOwner->GetSerial() != GetSerial() )
		newOwner->AddOwnedItem( this );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveSelfFromCont( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove item (self) from container it's in
//o-----------------------------------------------------------------------------------------------o
void CItem::RemoveSelfFromCont( void )
{
	if( contObj != nullptr )
	{
		if( contObj->GetObjType() == OT_CHAR )	// it's a char!
		{
			CChar *targChar = dynamic_cast<CChar *>(contObj);
			if( ValidateObject( targChar ) )
			{
				Weight->subtractItemWeight( targChar, this );
				targChar->TakeOffItem( GetLayer() );
			}
		}
		else
		{
			CItem *targItem = dynamic_cast<CItem *>(contObj);
			if( ValidateObject( targItem ) )
			{
				Weight->subtractItemWeight( targItem, this );
				targItem->GetContainsList()->Remove( this );
			}
		}
	}
	else {
		MapRegion->RemoveItem( this );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem * Dupe( ObjectType itemType )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Make a copy of item and copy all properties from old item to new
//o-----------------------------------------------------------------------------------------------o
CItem * CItem::Dupe( ObjectType itemType )
{
	CItem *target = static_cast< CItem * >(ObjectFactory::getSingleton().CreateObject( itemType ));
	if( target == nullptr )
		return nullptr;

	CopyData( target );

	return target;
}

void CItem::CopyData( CItem *target )
{
	CBaseObject::CopyData( target );

	target->SetGridLocation( GetGridLocation() );
	target->SetPostLoaded( isPostLoaded() );
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
	target->SetDexterity( GetDexterity() );
	target->SetDexterity2( GetDexterity2() );
	target->SetResist( GetResist( PHYSICAL ), PHYSICAL );
	target->SetDir( GetDir() );
	target->SetDisabled( isDisabled() );
	target->SetDoorOpen( isDoorOpen() );
	target->SetDye( isDyeable() );
	target->SetFree( isFree() );
	target->SetTempTimer( GetTempTimer() );
	target->SetGlow( GetGlow() );
	target->SetGlowColour( GetGlowColour() );
	target->SetGlowEffect( GetGlowEffect() );
	target->SetAmmoID( GetAmmoID() );
	target->SetAmmoHue( GetAmmoHue() );
	target->SetAmmoFX( GetAmmoFX() );
	target->SetAmmoFXHue( GetAmmoFXHue() );
	target->SetAmmoFXRender( GetAmmoFXRender() );
	target->SetGood( GetGood() );
	target->SetHiDamage( GetHiDamage() );
	target->SetHP( GetHP() );
	target->SetID( GetID() );
	target->SetIntelligence( GetIntelligence() );
	target->SetIntelligence2( GetIntelligence2() );
	target->SetLocation( this );
	target->SetLoDamage( GetLoDamage() );
	target->SetMadeWith( GetMadeWith() );
	target->SetMovable( GetMovable() );
	target->SetMana( GetMana() );
	target->SetMaxHP( GetMaxHP() );
	target->SetTempVar( CITV_MORE, GetTempVar( CITV_MORE ) );
	target->SetTempVar( CITV_MOREX, GetTempVar( CITV_MOREX ) );
	target->SetTempVar( CITV_MOREY, GetTempVar( CITV_MOREY ) );
	target->SetTempVar( CITV_MOREZ, GetTempVar( CITV_MOREZ ) );
	target->SetMulti( GetMultiObj() );
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
	target->SetSpawn( GetSpawn() );
	target->SetSpeed( GetSpeed() );
	target->SetSpell( 0, GetSpell( 0 ) );
	target->SetSpell( 1, GetSpell( 1 ) );
	target->SetSpell( 2, GetSpell( 2 ) );
	target->SetStamina( GetStamina() );
	target->SetStrength( GetStrength() );
	target->SetStrength2( GetStrength2() );
	target->SetTitle( GetTitle() );
	target->SetType( GetType() );
	target->SetBuyValue( GetBuyValue() );
	target->SetSellValue( GetSellValue() );
	target->SetVisible( GetVisible() );
	target->SetWeight( GetWeight() );
	target->SetWeightMax( GetWeightMax() );
	target->SetBaseWeight( GetBaseWeight() );
	target->SetMaxItems( GetMaxItems() );
	//target->SetWipeable( isWipeable() );
	target->SetPriv( GetPriv() );

	// Set damage types on new item
	for( SI32 i = 0; i < WEATHNUM; ++i )
	{
		target->SetWeatherDamage( (WeatherType)i, GetWeatherDamage( (WeatherType)i ) );
	}

	// Add any script triggers present on object to the new object
	target->scriptTriggers = GetScriptTriggers();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetWeatherDamage( WeatherType effectNum ) const
//|					void SetWeatherDamage( WeatherType effectNum, bool value )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets weather damage of item - primarily used by weapons
//o-----------------------------------------------------------------------------------------------o
bool CItem::GetWeatherDamage( WeatherType effectNum ) const
{
	return weatherBools.test( effectNum );
}
void CItem::SetWeatherDamage( WeatherType effectNum, bool value )
{
	weatherBools.set( effectNum, value );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DumpHeader( std::ofstream &outStream ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Dump item header to worldfile
//o-----------------------------------------------------------------------------------------------o
bool CItem::DumpHeader( std::ofstream &outStream ) const
{
	outStream << "[ITEM]" << '\n';
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DumpBody( std::ofstream &outStream ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Dump item tags and values to worldfile
//o-----------------------------------------------------------------------------------------------o
bool CItem::DumpBody( std::ofstream &outStream ) const
{
	CBaseObject::DumpBody( outStream );

	// Hexadecimal Values
	outStream << std::hex;
	outStream << "GridLoc=" << "0x" << (SI16)GetGridLocation() << '\n';
	outStream << "Layer=" << "0x" << (SI16)GetLayer() << '\n';
	outStream << "Cont=" << "0x" << GetContSerial() << '\n';
	outStream << "More=" << "0x" << GetTempVar( CITV_MORE ) << '\n';
	outStream << "Creator=" << "0x" << GetCreator() << '\n';
	outStream << "MoreXYZ=" << "0x" << GetTempVar( CITV_MOREX ) << ",0x" << GetTempVar( CITV_MOREY ) << ",0x" << GetTempVar( CITV_MOREZ ) << '\n';
	outStream << "Glow=" << "0x" << GetGlow() << '\n';
	outStream << "GlowBC=" << "0x" << GetGlowColour() << '\n';
	outStream << "Ammo=" << "0x" << GetAmmoID() << ",0x" << GetAmmoHue() << '\n';
	outStream << "AmmoFX=" << "0x" << GetAmmoFX() << ",0x" << GetAmmoFXHue() << ",0x" << GetAmmoFXRender() << '\n';
	outStream << "Spells=" << "0x" << GetSpell( 0 ) << ",0x" << GetSpell( 1 ) << ",0x" << GetSpell( 2 ) << '\n';

	// Decimal / String Values
	outStream << std::dec;
	outStream << "Name2=" << GetName2() << '\n';
	outStream << "Desc=" << GetDesc() << '\n';
	outStream << "Type=" << static_cast<SI16>(GetType()) << '\n';
	outStream << "Offspell=" << (SI16)GetOffSpell() << '\n';
	outStream << "Amount=" << GetAmount() << '\n';
	outStream << "WeightMax=" << GetWeightMax() << '\n';
	outStream << "BaseWeight=" << GetBaseWeight() << '\n';
	outStream << "MaxItems=" << GetMaxItems() << '\n';
	outStream << "MaxHP=" << GetMaxHP() << '\n';
	outStream << "Speed=" << (SI16)GetSpeed() << '\n';
	outStream << "Movable=" << (SI16)GetMovable() << '\n';
	outStream << "Priv=" << (SI16)GetPriv() << '\n';
	outStream << "Value=" << GetBuyValue() << "," << GetSellValue() << '\n';
	outStream << "Restock=" << GetRestock() << '\n';
	outStream << "AC=" << (SI16)GetArmourClass() << '\n';
	outStream << "Rank=" << (SI16)GetRank() << '\n';
	outStream << "Sk_Made=" << (SI16)GetMadeWith() << '\n';
	outStream << "Bools=" << (SI16)(bools.to_ulong()) << '\n';
	outStream << "Good=" << GetGood() << '\n';
	outStream << "GlowType=" << (SI16)GetGlowEffect() << '\n';
	outStream << "RaceDamage=" << (SI16)(GetWeatherDamage( LIGHT ) ? 1 : 0) << "," << (SI16)(GetWeatherDamage( RAIN ) ? 1 : 0) << ","
	<< (SI16)(GetWeatherDamage( HEAT ) ? 1 : 0) << "," << (SI16)(GetWeatherDamage( COLD ) ? 1 : 0) << ","
	<< (SI16)(GetWeatherDamage( SNOW ) ? 1 : 0) << "," << (SI16)(GetWeatherDamage( LIGHTNING ) ? 1 : 0) << '\n';
	outStream << "EntryMadeFrom=" << EntryMadeFrom() << '\n';
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool HandleLine( std::string &UTag, std::string &data )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles loading in tags and values from world files on startup
//o-----------------------------------------------------------------------------------------------o
bool CItem::HandleLine( std::string &UTag, std::string &data )
{
	bool rvalue = CBaseObject::HandleLine( UTag, data );
	if( !rvalue )
	{
		auto csecs = strutil::sections( data, "," );
		switch( (UTag.data()[0]) )
		{
			case 'A':
				if( UTag == "AMMO" )
				{
					if( csecs.size() == 2 )
					{
						SetAmmoID( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
						SetAmmoHue( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
					}
					else
					{
						SetAmmoID( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
						SetAmmoHue( ( 0 ) );
					}
					rvalue = true;
				}
				else if( UTag == "AMMOFX" )
				{
					if( csecs.size() == 2 )
					{
						SetAmmoFX( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
						SetAmmoFXHue( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
						SetAmmoFXRender( static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
					}
					else
					{
						SetAmmoFX( static_cast<UI16>(std::stoul( strutil::stripTrim( data ), nullptr, 0 )));
						SetAmmoFXHue( ( 0 ) );
						SetAmmoFXRender( ( 0 ) );
					}
					rvalue = true;
				}
				else if( UTag == "AMOUNT" )
				{
					amount = static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
					rvalue = true;
				}
				else if( UTag == "AC" )
				{
					SetArmourClass( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				break;
			case 'B':
				if( UTag == "BOOLS" )
				{
					bools = static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
					rvalue = true;
				}
				break;
			case 'C':
				if( UTag == "CONT" )
				{
					temp_container_serial = static_cast<SERIAL>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
					rvalue = true;
				}
				else if( UTag == "CREATOR" || UTag == "CREATER" )
				{
					SetCreator( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "CORPSE" )
				{
					SetCorpse( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				else if( UTag == "COLD" )
				{
					SetWeatherDamage( COLD, static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				break;
			case 'D':
				if( UTag == "DESC" )
				{
					SetDesc( data.c_str() );
					rvalue = true;
				}
				if( UTag == "DIR" )
				{
					SetDir( static_cast<SI08>(std::stoi(strutil::stripTrim( data ), nullptr, 0)));
					rvalue = true;
				}
				else if( UTag == "DYEABLE" )
				{
					SetDye( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				break;
			case 'E':
				if( UTag == "ENTRYMADEFROM" )
				{
					EntryMadeFrom( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				break;
			case 'G':
				if( UTag == "GRIDLOC" )
				{
					SetGridLocation( static_cast<SI08>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "GLOWTYPE" )
				{
					SetGlowEffect( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "GLOWBC" )
				{
					SetGlowColour( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "GLOW" )
				{
					SetGlow( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "GOOD" )
				{
					SetGood( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				break;
			case 'H':
				if( UTag == "HEAT" )
				{
					SetWeatherDamage( HEAT, static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				break;
			case 'L':
				if( UTag == "LAYER" )
				{
					layer = static_cast<ItemLayers>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
					rvalue = true;
				}
				else if( UTag == "LIGHT" )
				{
					SetWeatherDamage( LIGHT, static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				else if( UTag == "LIGHTNING" )
				{
					SetWeatherDamage( LIGHTNING, static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				break;
			case 'M':
				if( UTag == "MAXITEMS" )
				{
					SetMaxItems( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "MORE" )
				{
					if( csecs.size() >= 4 )
					{
						SetTempVar( CITV_MORE, 1, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[0] ), nullptr, 0 ) ) );
						SetTempVar( CITV_MORE, 2, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[1] ), nullptr, 0 ) ) );
						SetTempVar( CITV_MORE, 3, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[2] ), nullptr, 0 ) ) );
						SetTempVar( CITV_MORE, 4, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[3] ), nullptr, 0 ) ) );
					}
					else
					{
						SetTempVar( CITV_MORE, static_cast<UI32>( std::stoul( strutil::stripTrim( data ), nullptr, 0 ) ) );
					}
					rvalue = true;
				}
				else if( UTag == "MORE2" )	// Depreciated
					rvalue = true;
				else if( UTag == "MURDERER" )
					rvalue = true;
				else if( UTag == "MOREXYZ" )
				{
					SetTempVar( CITV_MOREX, static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
					SetTempVar( CITV_MOREY, static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
					SetTempVar( CITV_MOREZ, static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[2] ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "MOREX" )
				{
					if( csecs.size() >= 4 )
					{
						SetTempVar( CITV_MOREX, 1, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[0] ), nullptr, 0 ) ) );
						SetTempVar( CITV_MOREX, 2, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[1] ), nullptr, 0 ) ) );
						SetTempVar( CITV_MOREX, 3, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[2] ), nullptr, 0 ) ) );
						SetTempVar( CITV_MOREX, 4, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[3] ), nullptr, 0 ) ) );
					}
					else
					{
						SetTempVar( CITV_MOREX, static_cast<UI32>( std::stoul( strutil::stripTrim( data ), nullptr, 0 ) ) );
					}
					rvalue = true;
				}
				else if( UTag == "MOREY" )
				{
					if( csecs.size() >= 4 )
					{
						SetTempVar( CITV_MOREY, 1, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[0] ), nullptr, 0 ) ) );
						SetTempVar( CITV_MOREY, 2, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[1] ), nullptr, 0 ) ) );
						SetTempVar( CITV_MOREY, 3, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[2] ), nullptr, 0 ) ) );
						SetTempVar( CITV_MOREY, 4, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[3] ), nullptr, 0 ) ) );
					}
					else
					{
						SetTempVar( CITV_MOREY, static_cast<UI32>( std::stoul( strutil::stripTrim( data ), nullptr, 0 ) ) );
					}
					rvalue = true;
				}
				else if( UTag == "MOREZ" )
				{
					if( csecs.size() >= 4 )
					{
						SetTempVar( CITV_MOREZ, 1, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[0] ), nullptr, 0 ) ) );
						SetTempVar( CITV_MOREZ, 2, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[1] ), nullptr, 0 ) ) );
						SetTempVar( CITV_MOREZ, 3, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[2] ), nullptr, 0 ) ) );
						SetTempVar( CITV_MOREZ, 4, static_cast<UI08>( std::stoul( strutil::stripTrim( csecs[3] ), nullptr, 0 ) ) );
					}
					else
					{
						SetTempVar( CITV_MOREZ, static_cast<UI32>( std::stoul( strutil::stripTrim( data ), nullptr, 0 ) ) );
					}
					rvalue = true;
				}
				else if( UTag == "MOVABLE" )
				{
					SetMovable( static_cast<SI08>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "MAXHP" )
				{
					SetMaxHP( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				break;
			case 'N':
				if( UTag == "NAME2" )
				{
					SetName2( data.c_str() );
					rvalue = true;
				}
				break;
			case 'O':
				if( UTag == "OFFSPELL" )
				{
					SetOffSpell( static_cast<SI08>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				break;
			case 'P':
				if( UTag == "PRIV" )
				{
					SetPriv( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "PILEABLE" )
				{
					SetPileable( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				break;
			case 'R':
				if( UTag == "RESTOCK" )
				{
					SetRestock( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "RACEDAMAGE" )
				{
					SetWeatherDamage( LIGHT, static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) == 1 );
					SetWeatherDamage( RAIN, static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)) == 1 );
					SetWeatherDamage( HEAT, static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[2] ), nullptr, 0)) == 1 );
					SetWeatherDamage( COLD, static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[3] ), nullptr, 0)) == 1 );
					SetWeatherDamage( SNOW, static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[4] ), nullptr, 0)) == 1 );
					SetWeatherDamage( LIGHTNING, static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[5] ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				else if( UTag == "RANK" )
				{
					SetRank( static_cast<SI08>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "RAIN" )
				{
					SetWeatherDamage( RAIN, static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				else if( UTag == "REPUTATION" )
					rvalue = true;
				break;
			case 'S':
				if( UTag == "SPEED" )
				{
					SetSpeed( static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "SK_MADE" )
				{
					SetMadeWith( static_cast<SI08>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "SNOW" )
				{
					SetWeatherDamage( SNOW, static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1 );
					rvalue = true;
				}
				else if( UTag == "SPELLS" )
				{
					SetSpell( 0, static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
					SetSpell( 1, static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
					SetSpell( 2, static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[2] ), nullptr, 0)) );
					rvalue = true;
				}
				break;
			case 'T':
				if( UTag == "TYPE" )
				{
					if( csecs.size() != 1 )
					{
						SetType( static_cast<ItemTypes>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
					}
					else
					{
						SetType( static_cast<ItemTypes>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
					}
					rvalue = true;
				}
				else if( UTag == "TYPE2" )
					rvalue = true;
				break;
			case 'V':
				if( UTag == "VALUE" )
				{
					if( csecs.size() > 1 )
					{
						SetBuyValue( static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
						SetSellValue( static_cast<UI32>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
					}
					else
					{
						auto val = static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
						SetBuyValue( val );
						SetSellValue( val / 2 );
					}
					rvalue = true;
				}
				break;
			case 'W':
				if( UTag == "WEIGHTMAX" )
				{
					SetWeightMax( static_cast<SI32>(std::stoi(strutil::stripTrim( data ), nullptr, 0)) );
					rvalue = true;
				}
				break;
		}
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool LoadRemnants( void )
//|	Date		-	21st January, 2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	After handling data specific load, other parts go here
//o-----------------------------------------------------------------------------------------------o
bool CItem::LoadRemnants( void )
{
	SetSerial( serial );

	// Tauriel adding region pointers
	if( (UI64)contObj == INVALIDSERIAL )
	{
		MapData_st& mMap = Map->GetMapData( worldNumber );
		if( GetX() < 0 || GetY() < 0 || GetX() > mMap.xBlock || GetY() > mMap.yBlock )
			return false;
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsContType( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if item has a container type
//o-----------------------------------------------------------------------------------------------o
bool CItem::IsContType( void ) const
{
	if( GetLayer() == IL_PACKITEM || GetLayer() == IL_BANKBOX )
		return true;
	switch( GetType() )
	{
		case IT_CONTAINER:
		case IT_LOCKEDCONTAINER:
		case IT_SPAWNCONT:
		case IT_LOCKEDSPAWNCONT:
		case IT_UNLOCKABLESPAWNCONT:
		case IT_TRASHCONT:
			return true;
		default:	return false;
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PostLoadProcessing( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to setup any pointers that may need adjustment following the loading of the world
//o-----------------------------------------------------------------------------------------------o
void CItem::PostLoadProcessing( void )
{
	CBaseObject::PostLoadProcessing();
	// Add item weight if item doesn't have it yet
	if( GetWeight() < 0 || GetWeight() > MAX_WEIGHT )
		SetWeight( Weight->calcWeight( this ) );

	CBaseObject *tmpObj	= nullptr;
	contObj				= nullptr;
	
	if( temp_container_serial != INVALIDSERIAL )
	{
		if( temp_container_serial >= BASEITEMSERIAL )
		{
			tmpObj = calcItemObjFromSer( temp_container_serial);
		}
		else
		{
			tmpObj = calcCharObjFromSer( temp_container_serial );
		}
	}
	SetCont( tmpObj );

	Items->StoreItemRandomValue( this, nullptr );
	CheckItemIntegrity();
	SetPostLoaded( true );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CheckItemIntegrity( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Run some integrity checks on item
//o-----------------------------------------------------------------------------------------------o
void CItem::CheckItemIntegrity( void )
{
	SERIAL getSerial = GetSerial();
	if( getSerial == INVALIDSERIAL )
	{
		Console.warning(strutil::format( "Item (%s) has an invalid serial number, Deleting", GetName().c_str()) );
		Delete();
		return;
	}

	if( getSerial == GetContSerial() )
	{
		Console.warning( strutil::format("Item 0x%X (%s) has dangerous container value, Auto-Correcting", getSerial, GetName().c_str()) );
		SetCont( nullptr );
	}
	if( getSerial == GetOwner() )
	{
		Console.warning( strutil::format("Item 0x%X (%s) has dangerous owner value, Auto-Correcting", getSerial, GetName().c_str()) );
		SetOwner( nullptr );
	}
	if( getSerial == GetSpawn() )
	{
		Console.warning( strutil::format("Item 0x%X (%s) has dangerous spawner value, Auto-Correcting", getSerial, GetName().c_str() ));
		SetSpawn( INVALIDSERIAL );
	}

	if( type == IT_CONTAINER && GetLayer() == IL_PACKITEM && (contObj != nullptr && contObj->CanBeObjType( OT_CHAR )))
	{
		UI16 maxItemsVal = GetMaxItems();
		if( maxItemsVal == 0 )
		{
			SetMaxItems(cwmWorldState->ServerData()->MaxPlayerPackItems());
			Console.warning(strutil::format("Container (%s) with maxItems set to 0 detected on character (%s). Resetting maxItems for container to default value.", std::to_string(GetSerial()).c_str(), std::to_string(contObj->GetSerial()).c_str()));
		}
	}
}

const UI32 BIT_DECAYABLE	=	0;
const UI32 BIT_NEWBIE		=	1;
const UI32 BIT_DISPELLABLE	=	2;
const UI32 BIT_DIVINELOCK	=	3;

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isDecayable( void ) const
//|					void SetDecayable( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether item can decay
//o-----------------------------------------------------------------------------------------------o
bool CItem::isDecayable( void ) const
{
	return priv.test( BIT_DECAYABLE );
}
void CItem::SetDecayable( bool newValue )
{
	if( newValue )
		SetDecayTime( 0 );

	priv.set( BIT_DECAYABLE, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isNewbie( void ) const
//|					void SetNewbie( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether item is marked as a newbie item (doesn't drop on death)
//o-----------------------------------------------------------------------------------------------o
bool CItem::isNewbie( void ) const
{
	return priv.test( BIT_NEWBIE );
}
void CItem::SetNewbie( bool newValue )
{
	priv.set( BIT_NEWBIE, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isDispellable( void ) const
//|					void SetDispellable( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether item is dispellable (part of a field spell, for instance)
//o-----------------------------------------------------------------------------------------------o
bool CItem::isDispellable( void ) const
{
	return priv.test( BIT_DISPELLABLE );
}
void CItem::SetDispellable( bool newValue )
{
	priv.set( BIT_DISPELLABLE, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isDivineLocked( void ) const
//|					void SetDivineLock( bool newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether container was locked by a GM
//o-----------------------------------------------------------------------------------------------o
bool CItem::isDivineLocked( void ) const
{
	return priv.test( BIT_DIVINELOCK );
}
void CItem::SetDivineLock( bool newValue )
{
	priv.set( BIT_DIVINELOCK, newValue );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 EntryMadeFrom( void ) const
//|					void EntryMadeFrom( UI16 newValue )
//|	Date		-	13 September, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Create-DFNs entry the item was made from
//o-----------------------------------------------------------------------------------------------o
UI16 CItem::EntryMadeFrom( void ) const
{
	return entryMadeFrom;
}
void CItem::EntryMadeFrom( UI16 newValue )
{
	entryMadeFrom = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetWeight( SI32 newVal, bool doWeightUpdate )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the weight of the item
//o-----------------------------------------------------------------------------------------------o
void CItem::SetWeight( SI32 newVal, bool doWeightUpdate )
{
	CBaseObject *checkCont = nullptr;
	if( isPostLoaded() && doWeightUpdate )
		checkCont = GetCont();

	if( ValidateObject( checkCont ) )
		Weight->subtractItemWeight( checkCont, this );

	weight = newVal;

	if( ValidateObject( checkCont ) )
		Weight->addItemWeight( checkCont, this );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsMetalType( void ) const
//|	Date		-	13 November, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if it's a metal object
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsLeatherType( void ) const
//|	Date		-	13 November, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if it's a leather object
//o-----------------------------------------------------------------------------------------------o
bool CItem::IsLeatherType( void ) const
{
	if( id >= 0x13C6 && id <= 0x13E2 )
		return true;
	if( id >= 0x144E && id <= 0x1457 )
		return true;
	return false;
}

inline bool operator==( const CItem& x, const CItem& y )
{
	return ( x.GetSerial() == y.GetSerial() );
}

inline bool operator<(const CItem& x, const CItem& y )
{
	return ( x.GetSerial() < y.GetSerial() );
}

inline bool operator>(const CItem& x, const CItem& y )
{
	return ( x.GetSerial() > y.GetSerial() );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void TextMessage( CSocket *s, SI32 dictEntry, R32 secsFromNow, UI16 Colour )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Display "speech" over an item
//o-----------------------------------------------------------------------------------------------o
void CItem::TextMessage( CSocket *s, SI32 dictEntry, R32 secsFromNow, UI16 Colour )
{
	UnicodeTypes dictLang	= ZERO;
	SERIAL speakTo			= INVALIDSERIAL;
	SpeechTarget target		= SPTRG_PCNPC;
	if( s != nullptr )
	{
		dictLang = s->Language();
		CChar *mChar	= s->CurrcharObj();
		speakTo			= mChar->GetSerial();
		target			= SPTRG_INDIVIDUAL;
	}

	std::string txt = Dictionary->GetEntry( dictEntry, dictLang );
	if( txt.empty() )
		return;

	CSpeechEntry& toAdd = SpeechSys->Add();
	toAdd.Speech( txt );
	toAdd.Font( FNT_NORMAL );
	toAdd.Speaker( GetSerial() );
	toAdd.SpokenTo( speakTo );
	toAdd.Type( OBJ );
	toAdd.At( BuildTimeValue( secsFromNow ) );
	toAdd.TargType( target );
	if( Colour == 0x0 || Colour == 0x1700)
		toAdd.Colour( 0x03B2 );
	else
		toAdd.Colour( Colour );
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Update( CSocket *mSock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Send this item to specified socket or all online people in range
//o-----------------------------------------------------------------------------------------------o
void CItem::Update( CSocket *mSock )
{
	if( GetType() == IT_TRADEWINDOW )
		return;

	//RemoveFromSight( mSock );
#pragma note( "To monitor: Commented out RemoveFromSight() in CItem::Update() to potentially fix a lot of flickering issues with animated items, boats, etc." )

	if( GetCont() == this )
	{
		Console.warning( strutil::format("Item %s(0x%X) has a dangerous container value, auto-correcting", GetName().c_str(), GetSerial() ));
		SetCont( nullptr );
	}

	CBaseObject *iCont = GetCont();
	if( iCont == nullptr )
	{
		SOCKLIST nearbyChars;
		if( GetID( 1 ) >= 0x40 )
			nearbyChars = FindNearbyPlayers( this, DIST_BUILDRANGE );
		else
			nearbyChars = FindPlayersInVisrange( this );
		for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
		{
			SendToSocket( (*cIter) );
		}
		return;
	}
	else if( iCont->GetObjType() == OT_CHAR )
	{
		CChar *charCont = static_cast<CChar *>(iCont);
		if( charCont != nullptr )
		{
			CPWornItem toWear = (*this);
			CPToolTip pSend( GetSerial() );
			SOCKLIST nearbyChars = FindNearbyPlayers( charCont );
			for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
			{
				if( !(*cIter)->LoginComplete() )
					continue;
				(*cIter)->Send( &toWear );
				(*cIter)->Send( &pSend );
			}
			return;
		}
	}
	else
	{
		CItem *itemCont = static_cast<CItem *>(iCont);
		if( itemCont != nullptr )
		{
			ObjectType oType = OT_CBO;
			SOCKLIST nearbyChars = FindNearbyPlayers( FindItemOwner( this, oType ), DIST_NEARBY );
			for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
			{
				if( !(*cIter)->LoginComplete() )
					continue;
				SendPackItemToSocket( (*cIter) );
			}
			return;
		}
	}
	Console.error(strutil::format( " CItem::Update(0x%X): cannot determine container type!", GetSerial() ));
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendToSocket( CSocket *mSock )
//|	Date		-	July 27, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates an item on the ground to specified socket
//o-----------------------------------------------------------------------------------------------o
void CItem::SendToSocket( CSocket *mSock )
{
	if( !mSock->LoginComplete() )
		return;

	CChar *mChar = mSock->CurrcharObj();
	if( mChar != nullptr )
	{
		if( !mChar->IsGM() )
		{
			if( GetVisible() != VT_VISIBLE || ( GetVisible() == VT_TEMPHIDDEN && mChar != GetOwnerObj() ) )	// Not a GM, and not the Owner
				return;
		}
		if( mSock->ClientType() >= CV_SA2D )
		{
			CPNewObjectInfo toSend( (*this), (*mChar) );
			mSock->Send( &toSend );
		}
		else
		{
			CPObjectInfo toSend( (*this), (*mChar) );
			mSock->Send( &toSend );
		}
		if( isCorpse() )
		{
			CPCorpseClothing cpcc( this );
			mSock->Send( &cpcc );
			CPItemsInContainer itemsIn( mSock, this, 0x01 );
			mSock->Send( &itemsIn );
		}
		if( !CanBeObjType( OT_MULTI ) )
		{
			CPToolTip pSend( GetSerial() );
			mSock->Send( &pSend );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendPackItemToSocket( CSocket *mSock )
//|	Date		-	July 27, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates an item contained in a pack to specified socket
//o-----------------------------------------------------------------------------------------------o
void CItem::SendPackItemToSocket( CSocket *mSock )
{
	CChar *mChar = mSock->CurrcharObj();
	if( mChar != nullptr )
	{
		bool isGM = mChar->IsGM();
		ItemLayers iLayer = GetLayer();
		if( !isGM && ( iLayer == IL_SELLCONTAINER || iLayer == IL_BOUGHTCONTAINER || iLayer == IL_BUYCONTAINER ) )
			return;

		CPAddItemToCont itemSend;
		if( mSock->ClientVerShort() >= CVS_6017 )
			itemSend.UOKRFlag( true );
		itemSend.Object( (*this) );
		if( isGM && GetID() == 0x1647 )
		{
			itemSend.Model( 0x0A0F );
			itemSend.Colour( 0x00C6 );
		}
		mSock->Send( &itemSend );
		CPToolTip pSend( GetSerial() );
		mSock->Send( &pSend );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveFromSight( CSocket *mSock )
//|	Date		-	September 7th, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops through all online chars and removes the item from their sight
//o-----------------------------------------------------------------------------------------------o
void CItem::RemoveFromSight( CSocket *mSock )
{
	CPRemoveItem toRemove	= (*this);
	CBaseObject *iCont		= GetCont();

	ObjectType oType	= OT_CBO;
	CBaseObject *iOwner	= FindItemOwner( this, oType );

	if( iCont == nullptr || oType == OT_ITEM )
	{
		CItem *rItem = nullptr;
		if( iCont == nullptr )
			rItem = this;
		else
			rItem = static_cast<CItem *>(iOwner);
		if( rItem != nullptr )
		{
			if( mSock != nullptr )
				mSock->Send( &toRemove );
			else
			{
				CChar *tChar			= nullptr;
				SOCKLIST nearbyChars;
				if( rItem == this )
					nearbyChars = FindPlayersInOldVisrange( rItem );
				else
					nearbyChars = FindPlayersInVisrange( rItem );
				for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
				{
					if( !(*cIter)->LoginComplete() )
						continue;

					tChar = (*cIter)->CurrcharObj();
					if( ValidateObject( tChar ) )
					{
						(*cIter)->Send( &toRemove );
					}
				}
			}
		}
	}
	else if( iCont->GetObjType() == OT_CHAR || oType == OT_CHAR )
	{
		CChar *rChar = nullptr;
		if( iCont->GetObjType() == OT_CHAR )
			rChar = static_cast<CChar *>(iCont);
		else
			rChar = static_cast<CChar *>(iOwner);
		if( rChar != nullptr )
		{
			if( mSock != nullptr )
				mSock->Send( &toRemove );
			else
			{
				SOCKLIST nearbyChars = FindNearbyPlayers( rChar );
				for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
				{
					if( !(*cIter)->LoginComplete() )
						continue;
					(*cIter)->Send( &toRemove );
				}
			}
		}
	}
	else
	{
		if( mSock != nullptr )
			mSock->Send( &toRemove );
		else
		{
			//std::scoped_lock lock(Network->internallock);
			Network->pushConn();
			for( CSocket *nSock = Network->FirstSocket(); !Network->FinishedSockets(); nSock = Network->NextSocket() )
			{
				if( !nSock->LoginComplete() )
					continue;
				nSock->Send( &toRemove );
			}
			Network->popConn();
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PlaceInPack( void )
//|	Date		-	October 18th, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Puts an item at a random location inside a pack
//o-----------------------------------------------------------------------------------------------o
void CItem::PlaceInPack( void )
{
	SetX( static_cast<SI16>(50 + RandomNum( 0, 79 )) );
	SetY( static_cast<SI16>(50 + RandomNum( 0, 79 )) );
	SetZ( 9 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetSpell( UI08 part ) const
//|					void SetSpell( UI08 part, UI32 newValue )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets spells in a spellbook item
//o-----------------------------------------------------------------------------------------------o
UI32 CItem::GetSpell( UI08 part ) const
{
	UI32 rvalue = 0;
	if( part < 3 )
		rvalue = spells[part];
	return rvalue;
}
void CItem::SetSpell( UI08 part, UI32 newValue )
{
	if( part < 3 )
		spells[part] = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Cleanup( void )
//|	Date		-	11/6/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Cleans up after item
//o-----------------------------------------------------------------------------------------------o
void CItem::Cleanup( void )
{
	if( !isFree() )	// We're not the default item in the handler
	{
		MapRegion->RemoveItem( this );

		JSEngine->ReleaseObject( IUE_ITEM, this );

		CBaseObject::Cleanup();

		CBaseObject *iCont = GetCont();
		RemoveFromSight();
		RemoveSelfFromCont();
		RemoveSelfFromOwner();

		for( CItem *tItem = Contains.First(); !Contains.Finished(); tItem = Contains.Next() )
		{
			if( ValidateObject( tItem ) )
				tItem->Delete();
		}
		// if we delete an item we should delete it from spawnregions
		// this will fix several crashes
		if( isSpawned() )
		{
			if( GetSpawn() < BASEITEMSERIAL )
			{
				UI16 spawnRegNum = static_cast<UI16>(GetSpawn());
				if( cwmWorldState->spawnRegions.find( spawnRegNum ) != cwmWorldState->spawnRegions.end() )
				{
					CSpawnRegion *spawnReg = cwmWorldState->spawnRegions[spawnRegNum];
					if( spawnReg != nullptr )
						spawnReg->deleteSpawnedItem( this );
				}
			}
			SetSpawn( INVALIDSERIAL );
		}
		if( GetSpawnObj() != nullptr )
			SetSpawn( INVALIDSERIAL );

		if( GetGlow() != INVALIDSERIAL )
		{
			CItem *j = calcItemObjFromSer( GetGlow() );
			if( ValidateObject( j ) )
				j->Delete();  // glow stuff, deletes the glower of a glowing stuff automatically
		}

		if( isGuarded() )
		{
			CChar *owner = nullptr;
			CMultiObj *multi = findMulti( this );
			if( ValidateObject( multi ) )
				owner = multi->GetOwnerObj();
			if( !ValidateObject( owner ) )
				owner = FindItemOwner( this );
			if( ValidateObject( owner ) )
			{
				CChar *petGuard = Npcs->getGuardingPet( owner, this );
				if( ValidateObject( petGuard ) )
					petGuard->SetGuarding( nullptr );
				SetGuarded( false );
			}
		}

		if( GetType() == IT_MESSAGEBOARD )
			MsgBoardRemoveFile( GetSerial() );

		if( GetType() == IT_BOOK && ( GetTempVar( CITV_MOREX ) == 666 || GetTempVar( CITV_MOREX ) == 999 ) )
			Books->DeleteBook( this );

		// Update container tooltip for nearby players
		if( ValidateObject( iCont ) && iCont->GetObjType() == OT_ITEM )
		{
			CChar *rootOwner = FindItemOwner( static_cast<CItem *>(iCont) );
			if( ValidateObject( rootOwner ) && rootOwner->GetObjType() == OT_CHAR )
			{
				CSocket *ownerSocket = rootOwner->GetSocket();
				if( ownerSocket != nullptr )
				{
					// Refresh container tooltip
					CPToolTip pSend( iCont->GetSerial() );
					ownerSocket->Send(&pSend);
				}
			}
			else
			{
				SOCKLIST nearbyChars = FindNearbyPlayers( iCont, DIST_NEARBY );
				for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
				{
					if( !(*cIter)->LoginComplete() )
						continue;

					// Refresh container tooltip
					CPToolTip pSend( iCont->GetSerial() );
					(*cIter)->Send(&pSend);
				}
			}
		}

		//Ensure that object is removed from refreshQueue
		RemoveFromRefreshQueue();
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanBeObjType( ObjectType toCompare ) const
//|	Date		-	24 June, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Indicates whether an object can behave as a particular type
//o-----------------------------------------------------------------------------------------------o
bool CItem::CanBeObjType( ObjectType toCompare ) const
{
	bool rvalue = CBaseObject::CanBeObjType( toCompare );
	if( !rvalue )
	{
		if( toCompare == OT_ITEM )
			rvalue = true;
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Delete( void )
//|	Date		-	11/6/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds item to deletion queue
//o-----------------------------------------------------------------------------------------------o
void CItem::Delete( void )
{
	if(cwmWorldState->deletionQueue.count(this)==0)
	{
		++(cwmWorldState->deletionQueue[this]);
		Cleanup();
		SetDeleted( true );
		ShouldSave( false );
	}
}

GenericList< CItem * > * CItem::GetContainsList( void )
{
	return &Contains;
}

//o-----------------------------------------------------------------------------------------------o
//|	Class		-	CSpawnItem() : CItem()
//|	Date		-	29th June, 2004
//o-----------------------------------------------------------------------------------------------o
//| Purpose		-	Version History
//|
//|						1.0		 		29th June, 2004
//|						Original implementation
//|						First attempt to begin breaking CItem into multiple smaller
//|						classes, reducing overall memory usage. Currently stores all
//|						objects spawned by an item.
//o-----------------------------------------------------------------------------------------------o
CSpawnItem::CSpawnItem() : CItem(),
isSectionAList( false )
{
	objType = OT_SPAWNER;
	Interval[0] = Interval[1] = 0;
	spawnSection.reserve( 100 );
	spawnSection = "";
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetInterval( UI08 part ) const
//|					void SetInterval( UI08 part, UI08 newVal )
//|	Date		-	6/29/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets Min and Max interval for spawner to respawn
//o-----------------------------------------------------------------------------------------------o
UI08 CSpawnItem::GetInterval( UI08 part ) const
{
	UI08 rvalue = 0;
	if( part < 2 )
		rvalue = Interval[part];
	return rvalue;
}
void CSpawnItem::SetInterval( UI08 part, UI08 newVal )
{
	if( part < 2 )
		Interval[part] = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string GetSpawnSection( void ) const
//|					void SetSpawnSection( const std::string &newVal )
//|	Date		-	6/29/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets Script section to spawn from
//o-----------------------------------------------------------------------------------------------o
std::string CSpawnItem::GetSpawnSection( void ) const
{
	return spawnSection;
}
void CSpawnItem::SetSpawnSection( const std::string &newVal )
{
	spawnSection = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsSectionAList( void ) const
//|					void IsSectionAList( bool newVal )
//|	Date		-	7/05/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets script section as a spawner list
//o-----------------------------------------------------------------------------------------------o
bool CSpawnItem::IsSectionAList( void ) const
{
	return isSectionAList;
}
void CSpawnItem::IsSectionAList( bool newVal )
{
	isSectionAList = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DumpHeader( std::ofstream &outStream ) const
//|	Date		-	6/29/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Dumps Header to Worldfile
//o-----------------------------------------------------------------------------------------------o
bool CSpawnItem::DumpHeader( std::ofstream &outStream ) const
{
	outStream << "[SPAWNITEM]" << '\n';
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DumpBody( std::ofstream &outStream ) const
//|	Date		-	6/29/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Dumps Spawn Item to Worldfile
//o-----------------------------------------------------------------------------------------------o
bool CSpawnItem::DumpBody( std::ofstream &outStream ) const
{
	CItem::DumpBody( outStream );
	outStream << "Interval=" << (UI16)GetInterval( 0 ) << "," << (UI16)GetInterval( 1 ) << '\n';
	outStream << "SpawnSection=" << GetSpawnSection() << '\n';
	outStream << "IsSectionAList=" << (UI16)(IsSectionAList()?1:0) << '\n';
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool HandleLine( std::string &UTag, std::string &data )
//|	Date		-	6/29/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Reads data from Worldfile into the class
//o-----------------------------------------------------------------------------------------------o
bool CSpawnItem::HandleLine( std::string &UTag, std::string &data )
{
	bool rvalue = CItem::HandleLine( UTag, data );
	if( !rvalue )
	{
		auto csecs = strutil::sections( data, "," );
		switch( (UTag.data()[0]) )
		{
			case 'I':
				if( UTag == "INTERVAL" )
				{
					SetInterval( 0, static_cast<UI08>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0)) );
					SetInterval( 1, static_cast<UI08>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0)) );
					rvalue = true;
				}
				else if( UTag == "ISSECTIONALIST" )
				{
					IsSectionAList( (static_cast<UI08>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 1) );
					rvalue = true;
				}
				break;
			case 'S':
				if( UTag == "SPAWNSECTION" )
				{
					SetSpawnSection( data );
					rvalue = true;
				}
				break;
		}
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DoRespawn( void )
//|	Date		-	6/29/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Will eventually take the place of RespawnItem()
//o-----------------------------------------------------------------------------------------------o
bool CSpawnItem::DoRespawn( void )
{
	bool rValue = false;
	switch( GetType() )
	{
		case IT_ITEMSPAWNER:			rValue = HandleItemSpawner();			break;
		case IT_NPCSPAWNER:
		case IT_AREASPAWNER:
		case IT_ESCORTNPCSPAWNER:		rValue = HandleNPCSpawner();			break;
		case IT_SPAWNCONT:
		case IT_LOCKEDSPAWNCONT:
		case IT_UNLOCKABLESPAWNCONT:	rValue = HandleSpawnContainer();		break;
		default:
			break;
	}

	return rValue;
}
bool CSpawnItem::HandleItemSpawner( void )
{
	bool shouldSpawn = true;
	for( CBaseObject *mObj = spawnedList.First(); !spawnedList.Finished(); mObj = spawnedList.Next() )
	{
		if( ValidateObject( mObj ) && !mObj->isFree() )
		{
			if( mObj->GetSpawnObj() == this )
			{
				if( mObj != this && mObj->GetX() == GetX() && mObj->GetY() == GetY() && mObj->GetZ() == GetZ() )
				{
					shouldSpawn = false;
					break;
				}
			}
		}
	}

	if( shouldSpawn )
	{
		std::string listObj = GetSpawnSection();
		if( !listObj.empty() )
			Items->AddRespawnItem( this, listObj, false, IsSectionAList(), 1 );
		else if( GetTempVar( CITV_MOREX ) != 0 )
			Items->AddRespawnItem( this, strutil::number( GetTempVar( CITV_MOREX ) ), false, 1 );
		else
		{
			Console.warning( "Bad Item Spawner Found, Deleting" );
			Delete();
			return true;
		}
	}
	return false;
}

bool CSpawnItem::HandleNPCSpawner( void )
{
	if( spawnedList.Num() < GetAmount() )
	{
		std::string listObj = GetSpawnSection();
		if( !listObj.empty() )
			Npcs->CreateNPC( this, listObj );
		else if( GetTempVar( CITV_MOREX ) != 0 )
			Npcs->CreateNPC( this, strutil::number( GetTempVar( CITV_MOREX ) ) );
		else
		{
			Console.warning( "Bad Npc/Area Spawner found; SPAWNSECTION or MOREX values missing! Deleting Spawner." );
			Delete();
			return true;
		}
	}
	return false;
}
bool CSpawnItem::HandleSpawnContainer( void )
{
	if( Contains.Num() < GetAmount() )
	{
		std::string listObj = GetSpawnSection();
		if( GetType() == IT_SPAWNCONT )
		{
			SetType( IT_LOCKEDSPAWNCONT ); // Lock the container

			if( GetTempVar( CITV_MOREZ, 2 ) > 0 )
			{
				// Part 2 of MOREZ being higher than 0 indicates container was previously trapped. Reapply trap!
				SetTempVar( CITV_MOREZ, 1, 1 );
			}
		}
		if( !listObj.empty() )
			Items->AddRespawnItem( this, listObj, true, IsSectionAList(), 1 );
		else if( GetTempVar( CITV_MOREX ) != 0 )
			Items->AddRespawnItem( this, strutil::number( GetTempVar( CITV_MOREX ) ), true, 1 );
		else
		{
			Console.warning( "Bad Spawn Container found; missing SPAWNSECTION or MOREX! Deleting Spawner." );
			Delete();
			return true;
		}
		RemoveFromSight();
		Update();
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Cleanup( void )
//|	Date		-	6/29/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Cleans up after spawner item
//o-----------------------------------------------------------------------------------------------o
void CSpawnItem::Cleanup( void )
{
	CItem::Cleanup();

	for( CBaseObject *mObj = spawnedList.First(); !spawnedList.Finished(); mObj = spawnedList.Next() )
	{
		if( mObj->GetSpawnObj() == this )
			mObj->SetSpawn( INVALIDSERIAL );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanBeObjType( ObjectType toCompare ) const
//|	Date		-	24 June, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Indicates whether an object can behave as a particular type
//o-----------------------------------------------------------------------------------------------o
bool CSpawnItem::CanBeObjType( ObjectType toCompare ) const
{
	bool rvalue = CItem::CanBeObjType( toCompare );
	if( !rvalue )
	{
		if( toCompare == OT_SPAWNER )
			rvalue = true;
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnItem * Dupe( void )
//|	Date		-	7/11/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new CSpawnItem and copies all of this objects properties to the new one
//o-----------------------------------------------------------------------------------------------o
CSpawnItem * CSpawnItem::Dupe( void )
{
	CSpawnItem *target = static_cast< CSpawnItem * >(ObjectFactory::getSingleton().CreateObject( OT_SPAWNER ));
	if( target == nullptr )
		return nullptr;

	CopyData( target );

	return target;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CopyData( CSpawnItem *target )
//|	Date		-	7/11/2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Copies all of this objects properties to the new one
//o-----------------------------------------------------------------------------------------------o
void CSpawnItem::CopyData( CSpawnItem *target )
{
	CItem::CopyData( target );

	target->SetSpawnSection( GetSpawnSection() );
	target->IsSectionAList( IsSectionAList() );
	target->SetInterval( 0, GetInterval( 0 ) );
	target->SetInterval( 1, GetInterval( 1 ) );
}
