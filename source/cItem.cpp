//o------------------------------------------------------------------------------------------------o
//|	File			-	cItem.cpp
//|	Date			-	2nd April, 2000
//o------------------------------------------------------------------------------------------------o
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
//o------------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "weight.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "ObjectFactory.h"
#include "speech.h"
#include "townregion.h"
#include "cRaces.h"
#include "cSpawnRegion.h"
#include "CJSEngine.h"
#include "CJSMapping.h"
#include "Dictionary.h"
#include "regions.h"
#include "msgboard.h"
#include "books.h"
#include "power.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "StringUtility.hpp"
#include "osunique.hpp"
#include <charconv>

const UI32 BIT_MAKERSMARK	=	0;
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
const UI16			DEFITEM_USESLEFT		= 0;
const UI16			DEFITEM_MAXUSES			= 0;
const UI16			DEFITEM_REGIONNUM 		= 255;
const UI16			DEFITEM_TEMPLASTTRADED	= 0;
const SI08			DEFITEM_STEALABLE	 	= 1;
const SI16			DEFITEM_ARTIFACTRARITY = 0;
const SI16			DEFITEM_LOWERSTATREQ	= 0;

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Constructor
//o------------------------------------------------------------------------------------------------o
CItem::CItem() : CBaseObject(),
contObj( nullptr ), glowEffect( DEFITEM_GLOWEFFECT ), glow( DEFITEM_GLOW ), glowColour( DEFITEM_GLOWCOLOUR ),
madeWith( DEFITEM_MADEWITH ), rndValueRate( DEFITEM_RANDVALUE ), good( DEFITEM_GOOD ), rank( DEFITEM_RANK ), armorClass( DEFITEM_ARMORCLASS ),
restock( DEFITEM_RESTOCK ), movable( DEFITEM_MOVEABLE ), tempTimer( DEFITEM_TEMPTIMER ), decayTime( DEFITEM_DECAYTIME ),
spd( DEFITEM_SPEED ), maxHp( DEFITEM_MAXHP ), amount( DEFITEM_AMOUNT ),
layer( DEFITEM_LAYER ), type( DEFITEM_TYPE ), offspell( DEFITEM_OFFSPELL ), entryMadeFrom( DEFITEM_ENTRYMADEFROM ),
creator( DEFITEM_CREATOR ), gridLoc( DEFITEM_GRIDLOC ), weightMax( DEFITEM_WEIGHTMAX ), baseWeight( DEFITEM_BASEWEIGHT ), maxItems( DEFITEM_MAXITEMS ),
maxRange( DEFITEM_MAXRANGE ), baseRange( DEFITEM_BASERANGE ), maxUses( DEFITEM_MAXUSES ), usesLeft( DEFITEM_USESLEFT ), regionNum( DEFITEM_REGIONNUM ), 
tempLastTraded( DEFITEM_TEMPLASTTRADED ), stealable( DEFITEM_STEALABLE ), artifactRarity( DEFITEM_ARTIFACTRARITY ), lowerStatReq( DEFITEM_LOWERSTATREQ )
{
	spells[0]	= spells[1] = spells[2] = 0;
	value[0]	= value[1] = value[2] = 0;
	ammo[0]		= ammo[1] = 0;
	ammoFX[0]	= ammoFX[1] = ammoFX[2] = 0;
	objType		= OT_ITEM;
	name2		= "#";
	name		= "#";
	sectionId	= "UNKNOWN";
	race		= 65535;
	memset( tempVars, 0, sizeof( tempVars[0] ) * CITV_COUNT );
	desc.reserve( MAX_NAME );
	eventName.reserve( MAX_NAME );
	id			= 0x0000;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	~CItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Destructor to clean things up when deleted
//o------------------------------------------------------------------------------------------------o
CItem::~CItem()
{
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetCont()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the container object
//o------------------------------------------------------------------------------------------------o
auto CItem::GetCont( void ) const -> CBaseObject *
{
	return contObj;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetContSerial()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the container's serial
//o------------------------------------------------------------------------------------------------o
auto CItem::GetContSerial( void ) const -> SERIAL
{
	if( contObj != nullptr )
		return contObj->GetSerial();

	return INVALIDSERIAL;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetGridLocation()
//|					CItem::SetGridLocation()
//|	Date		-	7-23-2007
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets "Grid Location" - for UOKR
//o------------------------------------------------------------------------------------------------o
auto CItem::GetGridLocation( void ) const -> SI08
{
	return gridLoc;
}
auto CItem::SetGridLocation( SI08 newLoc ) -> void
{
	gridLoc = newLoc;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetTempVar()
//|					CItem::SetTempVar()
//|	Date		-	7/6/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the temp value of the object
//o------------------------------------------------------------------------------------------------o
auto CItem::GetTempVar( CITempVars whichVar ) const -> UI32
{
	if( whichVar == CITV_COUNT )
		return 0;

	return tempVars[whichVar];
}
auto CItem::SetTempVar( CITempVars whichVar, UI32 newVal ) -> void
{
	if( whichVar == CITV_COUNT )
		return;

	tempVars[whichVar] = newVal;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetTempVar()
//|					CItem::SetTempVar()
//|	Date		-	7/6/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets one of the words of the temp value
//|	Notes		-	Valid values for part are 1->4.  If outside that, behaves as if it were 1
//o------------------------------------------------------------------------------------------------o
auto CItem::GetTempVar( CITempVars whichVar, UI08 part ) const -> UI08
{
	UI08 rVal = 0;

	if( whichVar != CITV_COUNT )
	{
		switch( part )
		{
			default:
			case 1:		rVal = static_cast<UI08>( tempVars[whichVar] >> 24 );	break;
			case 2:		rVal = static_cast<UI08>( tempVars[whichVar] >> 16 );	break;
			case 3:		rVal = static_cast<UI08>( tempVars[whichVar] >> 8 );	break;
			case 4:		rVal = static_cast<UI08>( tempVars[whichVar] % 256 );	break;
		}
	}
	return rVal;
}
auto CItem::SetTempVar( CITempVars whichVar, UI08 part, UI08 newVal ) -> void
{
	if( whichVar == CITV_COUNT )
		return;

	UI08 part1 = static_cast<UI08>( tempVars[whichVar] >> 24 );
	UI08 part2 = static_cast<UI08>( tempVars[whichVar] >> 16 );
	UI08 part3 = static_cast<UI08>( tempVars[whichVar] >> 8 );
	UI08 part4 = static_cast<UI08>( tempVars[whichVar] % 256 );

	switch( part )
	{
		default:
		case 1:		part1 = newVal;		break;
		case 2:		part2 = newVal;		break;
		case 3:		part3 = newVal;		break;
		case 4:		part4 = newVal;		break;
	}
	tempVars[whichVar] = ( part1 << 24 ) + ( part2 << 16 ) + ( part3 << 8 ) + part4;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::SetContSerial()
//|					CItem::SetCont()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set's the item's container value to newValue
//|						Takes it out of the old container, and puts in the new
//|						Copes with being on paperdolls, ground and in containers
//|						Also copes with removing and adding to a map region
//|						Returns false if item needs deleting, true if fine
//o------------------------------------------------------------------------------------------------o
auto CItem::SetContSerial( SERIAL newSerial ) -> bool
{
	if( newSerial != INVALIDSERIAL )
	{
		if( newSerial >= BASEITEMSERIAL )
		{
			return SetCont( CalcItemObjFromSer( newSerial ));
		}
		else
		{
			return SetCont( CalcCharObjFromSer( newSerial ));
		}
	}
	return SetCont( nullptr );
}
auto CItem::SetCont( CBaseObject *newCont, bool removeFromView ) -> bool
{
	if( IsPostLoaded() )
	{
		Dirty( UT_UPDATE );
		RemoveSelfFromCont();
	}

	bool contIsGround = true;
	if( ValidateObject( newCont ))
	{
		contObj = newCont;
		if( newCont->GetObjType() == OT_CHAR )
		{
			CChar *charWearing = static_cast<CChar *>( newCont );
			if( ValidateObject( charWearing ))
			{
				if( charWearing->IsDead() && ( this->GetLayer() == IL_HAIR || this->GetLayer() == IL_FACIALHAIR ))
				{
					// if charWaring is dead, it means we're setting the cont on a duped item soon-to-be-moved to his corpse,
					// so we don't want him to attempt to wear it.
					contIsGround = false;
					if( IsPostLoaded() )
					{
						Weight->AddItemWeight( charWearing, this );
					}
				}
				else if( charWearing->WearItem( this ))
				{
					contIsGround = false;
					if( IsPostLoaded() )
					{
						Weight->AddItemWeight( charWearing, this );
					}
					if( this->GetLayer() == IL_MOUNT && charWearing->IsNpc() )
					{
						charWearing->SetOnHorse( true );
					}

					// Set new save flag on item based on save flag of new owner
					ShouldSave( charWearing->ShouldSave() );
				}
			}

			// Update item's townregion based on parent character's location
			CTownRegion *tRegion = CalcRegionFromXY( charWearing->GetX(), charWearing->GetY(), charWearing->WorldNumber(), charWearing->GetInstanceId(), this );
			SetRegion(( tRegion != nullptr ? tRegion->GetRegionNum() : 0xFF ));
		}
		else
		{
			CItem *itemHolder = static_cast<CItem *>( newCont );
			if( itemHolder != nullptr )
			{
				contIsGround = false;
				// ok heres what hair/beards should be handled like
				if((( GetLayer() == IL_HAIR ) || ( GetLayer() == IL_FACIALHAIR )) && !itemHolder->IsCorpse() )
				{
					CChar *itemPackOwner = FindItemOwner( itemHolder );
					if( ValidateObject( itemPackOwner ))
					{
						CItem *oldItem = itemPackOwner->GetItemAtLayer( GetLayer() );
						if( ValidateObject( oldItem ))
						{
							oldItem->Delete();
						}

						SetCont( itemPackOwner );

						// Set new save flag on item based on save flag of new owner
						ShouldSave( itemPackOwner->ShouldSave() );
					}
				}
				else
				{
					if( removeFromView )
					{
						// Update old location of item
						oldLocX = x;
						oldLocY = y;
						oldLocZ = z;

						// Remove item from view of all nearby players
						RemoveFromSight();
					}

					//itemHolder->GetContainsList()->Add( this );
					itemHolder->GetContainsList()->AddInFront( this );

					// Set new save flag on item based on save flag of new container
					ShouldSave( itemHolder->ShouldSave() );
				}
				if( IsPostLoaded() )
				{
					Weight->AddItemWeight( itemHolder, this );
				}

				// Update item's townregion to match root container's location
				CTownRegion *tRegion = CalcRegionFromXY( itemHolder->GetX(), itemHolder->GetY(), itemHolder->WorldNumber(), itemHolder->GetInstanceId(), this );
				SetRegion(( tRegion != nullptr ? tRegion->GetRegionNum() : 0xFF ));
			}
		}
	}

	if( contIsGround )
	{
		contObj = nullptr;
		MapRegion->AddItem( this );

		// If item has been moved to the ground, make sure we save it
		ShouldSave( true );
	}
	else
	{
		// Remove item from any multi it might be in
		if( ValidateObject( GetMultiObj() ))
		{
			RemoveFromMulti( false );
			multis = nullptr;
		}
	}

	if( GetGlow() != INVALIDSERIAL )
	{
		Items->GlowItem( this );
	}

	UpdateRegion();

	return !contIsGround;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::InDungeon()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Determine if item is inside a dungeon
//o------------------------------------------------------------------------------------------------o
auto CItem::InDungeon() -> bool
{
	bool rValue = false;
	if( GetRegion() != nullptr )
	{
		rValue = GetRegion()->IsDungeon();
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsDoorOpen()
//|					CItem::SetDoorOpen()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets door's open state
//o------------------------------------------------------------------------------------------------o
auto CItem::IsDoorOpen() const -> bool
{
	return bools.test( BIT_DOOROPEN );
}
auto CItem::SetDoorOpen( bool newValue ) -> void
{
	bools.set( BIT_DOOROPEN, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsPileable()
//|					CItem::SetPileable()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's pileable state
//o------------------------------------------------------------------------------------------------o
auto CItem::IsPileable() const -> bool
{
	return bools.test( BIT_PILEABLE );
}
auto CItem::SetPileable( bool newValue ) -> void
{
	bools.set( BIT_PILEABLE, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsMarkedByMaker()
//|					CItem::SetMakersMark()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maker's mark on item
//o------------------------------------------------------------------------------------------------o
auto CItem::IsMarkedByMaker() const -> bool
{
	return bools.test( BIT_MAKERSMARK );
}
auto CItem::SetMakersMark( bool newValue ) -> void
{
	bools.set( BIT_MAKERSMARK, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsDyeable()
//|					CItem::SetDye()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's dyeable state
//o------------------------------------------------------------------------------------------------o
auto CItem::IsDyeable() const -> bool
{
	return bools.test( BIT_DYEABLE );
}
auto CItem::SetDye( bool newValue ) -> void
{
	bools.set( BIT_DYEABLE, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsCorpse()
//|					CItem::SetCorpse()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's corpse state
//o------------------------------------------------------------------------------------------------o
auto CItem::IsCorpse() const -> bool
{
	return bools.test( BIT_CORPSE );
}
auto CItem::SetCorpse( bool newValue ) -> void
{
	bools.set( BIT_CORPSE, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsHeldOnCursor()
//|					CItem::SetHeldOnCursor()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether item is being held on cursor by a player
//o------------------------------------------------------------------------------------------------o
auto CItem::IsHeldOnCursor() const -> bool
{
	return bools.test( BIT_HELDONCURSOR );
}
auto CItem::SetHeldOnCursor( bool newValue ) -> void
{
	bools.set( BIT_HELDONCURSOR, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsGuarded()
//|					CItem::SetGuarded()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's guarded state
//o------------------------------------------------------------------------------------------------o
auto CItem::IsGuarded() const -> bool
{
	return bools.test( BIT_GUARDED );
}
void CItem::SetGuarded( bool newValue )
{
	bools.set( BIT_GUARDED, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsSpawnerList()
//|					CItem::SetSpawnerList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether spawner points to a a NPCLIST/ITEMLIST instead of an NPC/Item
//o------------------------------------------------------------------------------------------------o
auto CItem::IsSpawnerList() const -> bool
{
	return bools.test( BIT_SPAWNERLIST );
}
auto CItem::SetSpawnerList( bool newValue ) -> void
{
	bools.set( BIT_SPAWNERLIST, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CBaseObject::GetArtifactRarity()
//|					CBaseObject::SetArtifactRarity()
//|	Date		-	9 May, 2024
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Artifacts Rarity of the object
//o------------------------------------------------------------------------------------------------o
SI16 CItem::GetArtifactRarity( void ) const
{
	return artifactRarity;
}
void CItem::SetArtifactRarity( SI16 newValue )
{
	artifactRarity = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetLowerStatReq()
//|					CItem::GetLowerStatReq()
//|	Date		-	30 April, 2024
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Stat Requirements of the object
//o------------------------------------------------------------------------------------------------o
SI16 CItem::GetLowerStatReq( void ) const
{
	return lowerStatReq;
}
void CItem::SetLowerStatReq( SI16 newValue )
{
	lowerStatReq = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetName2()
//|					CItem::SetName2()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's name2 property - used for magical items
//o------------------------------------------------------------------------------------------------o
auto CItem::GetName2() const -> const std::string&
{
	return name2;
}
auto CItem::SetName2( const std::string &value ) -> void
{
	name2 = value;
	if( name2.size() >= MAX_NAME )
	{
		name2 = name2.substr( 0, MAX_NAME - 1 );
	}
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetCreator()
//|					CItem::SetCreator()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets serial of item's creator - used for crafting and maker's marks
//o------------------------------------------------------------------------------------------------o
auto CItem::GetCreator() const -> SERIAL
{
	return creator;
}
auto CItem::SetCreator( SERIAL newValue ) -> void
{
	creator = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetDesc()
//|					CItem::SetDesc()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's description property - used for playervendors, among other things
//o------------------------------------------------------------------------------------------------o
auto CItem::GetDesc() const -> std::string
{
	return desc;
}
auto CItem::SetDesc( std::string newValue ) -> void
{
	desc = newValue.substr( 0, MAX_NAME - 1 );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetEvent()
//|					CItem::SetEvent()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's event property - used to attach items to specific events
//o------------------------------------------------------------------------------------------------o
auto CItem::GetEvent() const -> std::string
{
	return eventName;
}
auto CItem::SetEvent( std::string newValue ) -> void
{
	eventName = newValue.substr( 0, MAX_NAME - 1 );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IncZ()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Increases item's Z height by specified value
//o------------------------------------------------------------------------------------------------o
auto CItem::IncZ( SI16 newValue ) -> void
{
	SetZ( z + newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IncLocation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Increases item's X and Y coordinates by specified values
//o------------------------------------------------------------------------------------------------o
auto CItem::IncLocation( SI16 xInc, SI16 yInc ) -> void
{
	SetLocation(( GetX() + xInc ), ( GetY() + yInc ), GetZ(), GetGridLocation(), WorldNumber(), GetInstanceId() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::SetOldLocation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates item's old location
//o------------------------------------------------------------------------------------------------o
void CItem::SetOldLocation( SI16 newX, SI16 newY, SI08 newZ )
{
	oldLocX = newX;
	oldLocY = newY;
	oldLocZ = newZ;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::SetLocation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets item's new location to match specified object's location
//o------------------------------------------------------------------------------------------------o
void CItem::SetLocation( const CBaseObject *toSet )
{
	if( toSet == nullptr )
		return;
	SetLocation( toSet->GetX(), toSet->GetY(), toSet->GetZ(), toSet->WorldNumber(), toSet->GetInstanceId() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::SetLocation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets item's new location to match specified coordinates
//o------------------------------------------------------------------------------------------------o
void CItem::SetLocation( SI16 newX, SI16 newY, SI08 newZ )
{
	SetLocation( newX, newY, newZ, GetGridLocation(), WorldNumber(), GetInstanceId() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::SetLocation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets item's new location to match specified coordinates, grid location, world and instance
//o------------------------------------------------------------------------------------------------o
void CItem::SetLocation( SI16 newX, SI16 newY, SI08 newZ, SI08 newLoc, UI08 world, UI16 instance_id )
{
	if( GetCont() == nullptr )
	{
		MapRegion->ChangeRegion( this, newX, newY, world );
	}
	oldLocX = x;
	oldLocY = y;
	oldLocZ = z;
	x = newX;
	y = newY;
	z = newZ;
	gridLoc = newLoc;
	worldNumber = world;
	instanceId = instance_id;
	if( GetCont() == nullptr )
	{
		// Calculate which townregion item exists in, based on its own location
		CTownRegion *tRegion = CalcRegionFromXY( x, y, worldNumber, instanceId, this );
		SetRegion(( tRegion != nullptr ? tRegion->GetRegionNum() : 0xFF ));

		if( !CanBeObjType( OT_MULTI ))
		{
			// If it's a sign with a more value, assume that the more value contains the serial of the multi the sign belongs to
			if((( id >= 0x0b95 && id <= 0x0c0e ) || id == 0x1f28 || id == 0x1f29 ) && GetTempVar( CITV_MORE ) != 0 )
			{
				CMultiObj *mMulti = CalcMultiFromSer( GetTempVar( CITV_MORE ));
				if( ValidateObject( mMulti ))
				{
					SetMulti( mMulti );
				}
			}
			else
			{
				CMultiObj *mMulti = FindMulti( newX, newY, newZ, world, instance_id );
				if( GetMultiObj() != mMulti )
				{
					SetMulti( mMulti );
				}
			}
		}
	}
	UpdateRegion();
	Dirty( UT_LOCATION );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::SetLocation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets item's new location to match specified coordinates, world and instance
//o------------------------------------------------------------------------------------------------o
void CItem::SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world, UI16 instanceId )
{
	SetLocation( newX, newY, newZ, GetGridLocation(), world, instanceId );
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CItem::GetRegion()
//|					CItem::SetRegion()
//|					CItem::GetRegionNum()
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Gets/Sets the town region the item is in
//o------------------------------------------------------------------------------------------------o
auto CItem::GetRegion() const -> CTownRegion*
{
	if( cwmWorldState->townRegions.find( regionNum ) == cwmWorldState->townRegions.end() )
		return cwmWorldState->townRegions[0xFF];

	return cwmWorldState->townRegions[regionNum];
}
auto CItem::SetRegion( UI16 newValue ) -> void
{
	regionNum = newValue;
	UpdateRegion();
}
auto CItem::GetRegionNum() const -> UI16
{
	return regionNum;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetLayer()
//|					CItem::SetLayer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's layer property
//o------------------------------------------------------------------------------------------------o
auto CItem::GetLayer() const -> ItemLayers
{
	return layer;
}
auto CItem::SetLayer( ItemLayers newValue ) -> void
{
	CBaseObject *getCont = GetCont();
	if( ValidateObject( getCont ) && getCont->GetObjType() == OT_CHAR )	// if we're on a char
	{
		CChar *charAffected = static_cast<CChar *>( getCont );
		if( ValidateObject( charAffected ))
		{
			if( layer != IL_NONE )
			{
				charAffected->TakeOffItem( layer );
			}
			layer = newValue;
			charAffected->WearItem( this );
			charAffected->UpdateRegion();
			return;
		}
	}
	layer = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetType()
//|					CItem::SetType()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's type property
//o------------------------------------------------------------------------------------------------o
auto CItem::GetType() const -> ItemTypes
{
	return type;
}
auto CItem::SetType( ItemTypes newValue ) -> void
{
	type = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetOffSpell()
//|					CItem::SetOffSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's offspell property - Unused?
//o------------------------------------------------------------------------------------------------o
auto CItem::GetOffSpell() const -> SI08
{
	return offspell;
}
auto CItem::SetOffSpell( SI08 newValue ) -> void
{
	offspell = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetAmount()
//|					CItem::SetAmount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's amount property, adjusts weight according to new value
//o------------------------------------------------------------------------------------------------o
auto CItem::GetAmount() const -> UI16
{
	return amount;
}
auto CItem::SetAmount( UI32 newValue ) -> void
{
	CBaseObject *getCont = GetCont();
	if( ValidateObject( getCont ))
	{
		Weight->SubtractItemWeight( getCont, this );
	}

	if( newValue > MAX_STACK )
	{
		amount = MAX_STACK;
	}
	else
	{
		amount = static_cast<UI16>( newValue );
	}

	if( ValidateObject( getCont ))
	{
		Weight->AddItemWeight( getCont, this );
	}
	Dirty( UT_UPDATE );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IncAmount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Increases item's amount property by specified amount,
//|					adjusts weight according to new value
//o------------------------------------------------------------------------------------------------o
auto CItem::IncAmount( SI32 incValue, bool noDelete ) -> bool
{
	bool iDeleted = false;
	SI32 newAmt = ( amount + incValue );
	if( newAmt >= 0 )
	{
		if( newAmt > MAX_STACK )
		{
			SetAmount( MAX_STACK );
		}
		else
		{
			SetAmount( static_cast<UI16>( newAmt ));
		}
	}
	if( !noDelete && newAmt <= 0 )
	{
		Delete();
		iDeleted = true;
	}
	return iDeleted;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetMaxHP()
//|					CItem::SetMaxHP()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's max hp property
//o------------------------------------------------------------------------------------------------o
UI16 CItem::GetMaxHP( void ) const
{
	return maxHp;
}
void CItem::SetMaxHP( UI16 newValue )
{
	maxHp = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetMaxUses()
//|					CItem::SetMaxUses()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's max uses property
//o------------------------------------------------------------------------------------------------o
auto CItem::GetMaxUses() const -> UI16
{
	return maxUses;
}
auto CItem::SetMaxUses( UI16 newValue ) -> void
{
	maxUses = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetUsesLeft()
//|					CItem::SetUsesLeft()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's property for remaining uses
//o------------------------------------------------------------------------------------------------o
auto CItem::GetUsesLeft() const -> UI16
{
	return usesLeft;
}
auto CItem::SetUsesLeft( UI16 newValue ) -> void
{
	usesLeft = newValue;
	Dirty( UT_UPDATE );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetSpeed()
//|					CItem::SetSpeed()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's speed property - used to determine weapon attack speed
//o------------------------------------------------------------------------------------------------o
auto CItem::GetSpeed() const -> UI08
{
	return spd;
}
auto CItem::SetSpeed( UI08 newValue ) -> void
{
	spd = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetMaxRange()
//|					CItem::SetMaxRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's maximum range - primarily used by throwing/archery weapons
//o------------------------------------------------------------------------------------------------o
auto CItem::GetMaxRange() const -> UI08
{
	return maxRange;
}
auto CItem::SetMaxRange( UI08 newValue ) -> void
{
	maxRange = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetBaseRange()
//|					CItem::SetBaseRange()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's base range - primarily used by throwing weapons
//o------------------------------------------------------------------------------------------------o
auto CItem::GetBaseRange() const -> UI08
{
	return baseRange;
}
auto CItem::SetBaseRange( UI08 newValue ) -> void
{
	baseRange = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetMovable()
//|					CItem::SetMovable()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's movable property
//o------------------------------------------------------------------------------------------------o
auto CItem::GetMovable() const -> SI08
{
	return movable;
}
auto CItem::SetMovable( SI08 newValue ) -> void
{
	movable = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetTempLastTraded()
//|					CItem::SetTempLastTraded()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets timestamp for when item was last traded using secure trade window (not saved)
//o------------------------------------------------------------------------------------------------o
auto CItem::GetTempLastTraded() const -> TIMERVAL
{
	return tempLastTraded;
}
auto CItem::SetTempLastTraded( TIMERVAL newValue ) -> void
{
	tempLastTraded = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetTempTimer()
//|					CItem::SetTempTimer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's temptimer
//o------------------------------------------------------------------------------------------------o
auto CItem::GetTempTimer() const -> TIMERVAL
{
	return tempTimer;
}
auto CItem::SetTempTimer( TIMERVAL newValue ) -> void
{
	tempTimer = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetDecayTime()
//|					CItem::SetDecayTime()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's decay timer
//o------------------------------------------------------------------------------------------------o
auto CItem::GetDecayTime() const -> TIMERVAL
{
	return decayTime;
}
auto CItem::SetDecayTime( TIMERVAL newValue ) -> void
{
	decayTime = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetPriv()
//|					CItem::SetPriv()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets priv settings on item
//o------------------------------------------------------------------------------------------------o
UI08 CItem::GetPriv( void ) const
{
	return static_cast<UI08>( priv.to_ulong() );
}
void CItem::SetPriv( UI08 newValue )
{
	priv = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetSellValue()
//|					CItem::SetSellValue()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the item's sell value - determines how much player can sell it for
//o------------------------------------------------------------------------------------------------o
auto CItem::GetSellValue() const -> UI32
{
	return value[1];
}
auto CItem::SetSellValue( UI32 newValue ) -> void
{
	value[1] = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetBuyValue()
//|					CItem::SetBuyValue()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's buy value - determines how much player must pay for it
//o------------------------------------------------------------------------------------------------o
auto CItem::GetBuyValue() const -> UI32
{
	return value[0];
}
auto CItem::SetBuyValue( UI32 newValue ) -> void
{
	value[0] = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetVendorPrice()
//|					CItem::SetVendorPrice()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's buy value - determines how much player must pay for it
//o------------------------------------------------------------------------------------------------o
auto CItem::GetVendorPrice() const -> UI32
{
	return value[2];
}
auto CItem::SetVendorPrice( UI32 newValue ) -> void
{
	value[2] = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetRestock()
//|					CItem::SetRestock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's restock value in vendor sellpack
//o------------------------------------------------------------------------------------------------o
auto CItem::GetRestock() const -> UI16
{
	return restock;
}
auto CItem::SetRestock( UI16 newValue ) -> void
{
	restock = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetArmourClass()
//|					CItem::SetArmourClass()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's armour class
//o------------------------------------------------------------------------------------------------o
auto CItem::GetArmourClass() const -> ARMORCLASS
{
	return armorClass;
}
auto CItem::SetArmourClass( ARMORCLASS newValue ) -> void
{
	armorClass = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetRank()
//|					CItem::SetRank()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's rank value - used in crafting
//o------------------------------------------------------------------------------------------------o
auto CItem::GetRank() const -> SI08
{
	return rank;
}
auto CItem::SetRank( SI08 newValue ) -> void
{
	rank = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetGood()
//|					CItem::SetGood()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's good value - used by advanced trade system
//o------------------------------------------------------------------------------------------------o
auto CItem::GetGood() const -> SI16
{
	return good;
}
auto CItem::SetGood( SI16 newValue ) -> void
{
	good = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetRndValueRate()
//|					CItem::SetRndValueRate()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item's random value rate - used by advanced trade system
//o------------------------------------------------------------------------------------------------o
auto CItem::GetRndValueRate() const -> SI32
{
	return rndValueRate;
}
auto CItem::SetRndValueRate( SI32 newValue ) -> void
{
	rndValueRate = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetMadeWith()
//|					CItem::SetMadeWith()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets ID of skill used to create item
//o------------------------------------------------------------------------------------------------o
auto CItem::GetMadeWith() const -> SI08
{
	return madeWith;
}
auto CItem::SetMadeWith( SI08 newValue ) -> void
{
	madeWith = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetGlow()
//|					CItem::SetGlow()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets serial of glow item attached to item
//o------------------------------------------------------------------------------------------------o
auto CItem::GetGlow() const -> SERIAL
{
	return glow;
}
auto CItem::SetGlow( SERIAL newValue ) -> void
{
	glow = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetGlowColour()
//|					CItem::SetGlowColour()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets colour of glow item attached to item
//o------------------------------------------------------------------------------------------------o
auto CItem::GetGlowColour( void ) const -> COLOUR
{
	return glowColour;
}
auto CItem::SetGlowColour( COLOUR newValue ) -> void
{
	glowColour = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetGlowEffect()
//|					CItem::SetGlowEffect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets colour of glow item attached to item
//o------------------------------------------------------------------------------------------------o
auto CItem::GetGlowEffect( void ) const -> UI08
{
	return glowEffect;
}
auto CItem::SetGlowEffect( UI08 newValue ) -> void
{
	glowEffect = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetAmmoId()
//|					CItem::SetAmmoId()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets ammo Id for ammo used by ranged weapon
//o------------------------------------------------------------------------------------------------o
auto CItem::GetAmmoId() const -> UI16
{
	return ammo[0];
}
auto CItem::SetAmmoId( UI16 newValue ) -> void
{
	ammo[0] = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetAmmoHue()
//|					CItem::SetAmmoHue()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets hue of ammo used by ranged weapon
//o------------------------------------------------------------------------------------------------o
auto CItem::GetAmmoHue() const -> UI16
{
	return ammo[1];
}
auto CItem::SetAmmoHue( UI16 newValue ) -> void
{
	ammo[1] = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetAmmoFX()
//|					CItem::SetAmmoFX()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets projectile FX shown when firing ranged weapon
//o------------------------------------------------------------------------------------------------o
auto CItem::GetAmmoFX() const -> UI16
{
	return ammoFX[0];
}
auto CItem::SetAmmoFX( UI16 newValue ) -> void
{
	ammoFX[0] = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetAmmoFXHue()
//|					CItem::SetAmmoFXHue()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets colour of projectiles fired by ranged weapons
//o------------------------------------------------------------------------------------------------o
auto CItem::GetAmmoFXHue() const -> UI16
{
	return ammoFX[1];
}
auto CItem::SetAmmoFXHue( UI16 newValue ) -> void
{
	ammoFX[1] = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetAmmoFXRender()
//|					CItem::SetAmmoFXRender()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the render mode of projectile FX played when firing ranged weapons
//o------------------------------------------------------------------------------------------------o
auto CItem::GetAmmoFXRender() const -> UI16
{
	return ammoFX[2];
}
auto CItem::SetAmmoFXRender( UI16 newValue ) -> void
{
	ammoFX[2] = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetWeightMax()
//|					CItem::SetWeightMax()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum weight (in stones) a container can hold
//o------------------------------------------------------------------------------------------------o
auto CItem::GetWeightMax() const -> SI32
{
	return weightMax;
}
auto CItem::SetWeightMax( SI32 newValue ) -> void
{
	weightMax = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetBaseWeight()
//|					CItem::SetBaseWeight()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets base item weight - primarily used to store original container weight
//o------------------------------------------------------------------------------------------------o
auto CItem::GetBaseWeight() const -> SI32
{
	return baseWeight;
}
auto CItem::SetBaseWeight( SI32 newValue ) -> void
{
	baseWeight = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetMaxItems()
//|					CItem::SetMaxItems()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets maximum amount of items a container can hold
//o------------------------------------------------------------------------------------------------o
auto CItem::GetMaxItems() const -> UI16
{
	return maxItems;
}
auto CItem::SetMaxItems( UI16 newValue ) -> void
{
	maxItems = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsFieldSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns whether item belongs to a field spell - and if so - which one
//o------------------------------------------------------------------------------------------------o
auto CItem::IsFieldSpell() const -> UI08
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
		case 0x0080:	return 5;// wall of stone
		default:		return 0;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::CanBeLockedDown()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether item can be locked down
//o------------------------------------------------------------------------------------------------o
auto CItem::CanBeLockedDown() const -> bool
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsLockedDown()
//|					CItem::LockDown()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether item is locked down and immovable
//o------------------------------------------------------------------------------------------------o
auto CItem::IsLockedDown() const -> bool
{
	return ( movable == 3 );
}
auto CItem::LockDown( CMultiObj *multiObj ) -> void
{
	if( ValidateObject( multiObj ))
	{
		multis = multiObj;
	}
	movable = 3;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsShieldType()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks whether item is a shield
//o------------------------------------------------------------------------------------------------o
auto CItem::IsShieldType() const -> bool
{
	return  type == IT_SHIELD;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::Save()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Save item details to worldfile
//o------------------------------------------------------------------------------------------------o
bool CItem::Save( std::ostream &outStream )
{
	if( IsFree() )
		return false;

	auto [mapWidth, mapHeight] = Map->SizeOfMap( worldNumber );
	if( GetCont() || ( GetX() > 0 && GetX() < mapWidth && GetY() < mapHeight ))
	{
		DumpHeader( outStream );
		DumpBody( outStream );
		DumpFooter( outStream );

		for( CItem *toSave = Contains.First(); !Contains.Finished(); toSave = Contains.Next() )
		{
			if( ValidateObject( toSave ) && toSave->ShouldSave() )
			{
				toSave->Save( outStream );
			}
		}
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::RemoveSelfFromOwner()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove item (self) from owner it belongs to
//o------------------------------------------------------------------------------------------------o
void CItem::RemoveSelfFromOwner( void )
{
	CChar *oldOwner = GetOwnerObj();
	if( oldOwner != nullptr )
	{
		oldOwner->RemoveOwnedItem( this );
		oldOwner->UpdateRegion();
	}
	
	if( oldOwner == nullptr || oldOwner->ShouldSave() || ( !oldOwner->ShouldSave() && !IsDeleted() ))
	{
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::AddSelfToOwner()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Add item (self) to owner
//o------------------------------------------------------------------------------------------------o
void CItem::AddSelfToOwner( void )
{
	CChar *newOwner = GetOwnerObj();
	if( !ValidateObject( newOwner ))
		return;

	if( newOwner->GetSerial() != GetSerial() )
	{
		newOwner->AddOwnedItem( this );
		newOwner->UpdateRegion();
	}
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::RemoveSelfFromCont()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove item (self) from container it's in
//o------------------------------------------------------------------------------------------------o
auto CItem::RemoveSelfFromCont() -> void
{
	if( contObj != nullptr )
	{
		if( contObj->GetObjType() == OT_CHAR )	// it's a char!
		{
			CChar *targChar = dynamic_cast<CChar *>( contObj );
			if( ValidateObject( targChar ))
			{
				Weight->SubtractItemWeight( targChar, this );
				targChar->TakeOffItem( GetLayer() );
				if( ShouldSave() && targChar->ShouldSave() )
				{
					UpdateRegion();
					targChar->UpdateRegion();
				}
			}
		}
		else
		{
			CItem *targItem = dynamic_cast<CItem *>( contObj );
			if( ValidateObject( targItem ))
			{
				Weight->SubtractItemWeight( targItem, this );
				targItem->GetContainsList()->Remove( this );
				UpdateRegion();
			}
		}
	}
	else
	{
		MapRegion->RemoveItem( this );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::Dupe()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Make a copy of item and copy all properties from old item to new
//o------------------------------------------------------------------------------------------------o
auto CItem::Dupe( ObjectType itemType ) -> CItem *
{
	CItem *target = static_cast<CItem *>(ObjectFactory::GetSingleton().CreateObject( itemType ));
	if( target == nullptr )
		return nullptr;

	CopyData( target );

	return target;
}

auto CItem::CopyData( CItem *target ) -> void
{
	CBaseObject::CopyData( target );

	target->SetGridLocation( GetGridLocation() );
	target->SetPostLoaded( IsPostLoaded() );
	target->SetAmount( GetAmount() );
	target->SetArmourClass( GetArmourClass() );
	target->SetCarve( GetCarve() );
	target->SetColour( GetColour() );
	target->SetLayer( GetLayer() );
	target->SetCont( GetCont() );
	target->SetCreator( GetCreator() );
	target->SetCorpse( IsCorpse() );
	target->SetDecayTime( GetDecayTime() );
	target->SetDesc( GetDesc() );
	target->SetEvent( GetEvent() );
	target->SetDexterity( GetDexterity() );
	target->SetDexterity2( GetDexterity2() );
	target->SetResist( GetResist( PHYSICAL ), PHYSICAL );
	target->SetDir( GetDir() );
	target->SetDisabled( IsDisabled() );
	target->SetDoorOpen( IsDoorOpen() );
	target->SetDye( IsDyeable() );
	target->SetFree( IsFree() );
	target->SetTempTimer( GetTempTimer() );
	target->SetGlow( GetGlow() );
	target->SetGlowColour( GetGlowColour() );
	target->SetGlowEffect( GetGlowEffect() );
	target->SetAmmoId( GetAmmoId() );
	target->SetAmmoHue( GetAmmoHue() );
	target->SetAmmoFX( GetAmmoFX() );
	target->SetAmmoFXHue( GetAmmoFXHue() );
	target->SetAmmoFXRender( GetAmmoFXRender() );
	target->SetGood( GetGood() );
	target->SetHiDamage( GetHiDamage() );
	target->SetHP( GetHP() );
	target->SetId( GetId() );
	target->SetIntelligence( GetIntelligence() );
	target->SetIntelligence2( GetIntelligence2() );
	target->SetLocation( this );
	target->SetLoDamage( GetLoDamage() );
	target->SetMadeWith( GetMadeWith() );
	target->SetMovable( GetMovable() );
	target->SetMana( GetMana() );
	target->SetMaxHP( GetMaxHP() );
	target->SetTempVar( CITV_MORE, GetTempVar( CITV_MORE ));
	target->SetTempVar( CITV_MORE0, GetTempVar( CITV_MORE0 ));
	target->SetTempVar( CITV_MORE1, GetTempVar( CITV_MORE1 ));
	target->SetTempVar( CITV_MORE2, GetTempVar( CITV_MORE2 ));
	target->SetTempVar( CITV_MOREX, GetTempVar( CITV_MOREX ));
	target->SetTempVar( CITV_MOREY, GetTempVar( CITV_MOREY ));
	target->SetTempVar( CITV_MOREZ, GetTempVar( CITV_MOREZ ));
	target->SetMulti( GetMultiObj() );
	target->SetName( GetName() ); // Double up? Also happens in CBaseObject::CopyData() above
	target->SetName2( GetName2() );
	target->SetOffSpell( GetOffSpell() );
	target->SetOwner( GetOwnerObj() );
	target->SetPileable( IsPileable() );
	target->SetMakersMark( IsMarkedByMaker() );
	target->SetPoisoned( GetPoisoned() );
	target->SetRace( GetRace() );
	target->SetRank( GetRank() );
	target->SetRestock( GetRestock() );
	target->SetRndValueRate( GetRndValueRate() );
	target->SetSpawn( GetSpawn() );
	target->SetSpeed( GetSpeed() );
	target->SetArtifactRarity( GetArtifactRarity() );
	target->SetSpell( 0, GetSpell( 0 ));
	target->SetSpell( 1, GetSpell( 1 ));
	target->SetSpell( 2, GetSpell( 2 ));
	target->SetStamina( GetStamina() );
	target->SetStrength( GetStrength() );
	target->SetStrength2( GetStrength2() );
	target->SetTitle( GetTitle() );
	target->SetType( GetType() );
	target->SetBuyValue( GetBuyValue() );
	target->SetSellValue( GetSellValue() );
	target->SetVendorPrice( GetVendorPrice() );
	target->SetVisible( GetVisible() );
	target->SetWeight( GetWeight() );
	target->SetWeightMax( GetWeightMax() );
	target->SetBaseWeight( GetBaseWeight() );
	target->SetMaxItems( GetMaxItems() );
	//target->SetWipeable( IsWipeable() );
	target->SetPriv( GetPriv() );
	target->SetBaseRange( GetBaseRange() );
	target->SetMaxRange( GetMaxRange() );
	target->SetMaxUses( GetMaxUses() );
	target->SetUsesLeft( GetUsesLeft() );
	target->SetLowerStatReq( GetLowerStatReq() );
	target->SetStealable( GetStealable() );

	// Set damage types on new item
	for( SI32 i = 0; i < WEATHNUM; ++i )
	{
		target->SetWeatherDamage( static_cast<WeatherType>( i ), GetWeatherDamage( static_cast<WeatherType>( i )));
	}

	// Add any script triggers present on object to the new object
	target->scriptTriggers = GetScriptTriggers();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetWeatherDamage()
//|					CItem::SetWeatherDamage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets weather damage of item - primarily used by weapons
//o------------------------------------------------------------------------------------------------o
auto CItem::GetWeatherDamage( WeatherType effectNum ) const -> bool
{
	return weatherBools.test( effectNum );
}
auto CItem::SetWeatherDamage( WeatherType effectNum, bool value ) -> void
{
	weatherBools.set( effectNum, value );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::DumpHeader()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dump item header to worldfile
//o------------------------------------------------------------------------------------------------o
bool CItem::DumpHeader( std::ostream &outStream ) const
{
	outStream << "[ITEM]" << '\n';
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::DumpBody()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dump item tags and values to worldfile
//o------------------------------------------------------------------------------------------------o
bool CItem::DumpBody( std::ostream &outStream ) const
{
	CBaseObject::DumpBody( outStream );
	const char newLine = '\n';

	// Hexadecimal Values
	outStream << std::hex;
	outStream << "GridLoc=0x" << static_cast<SI16>( GetGridLocation() ) << newLine;
	outStream << "Layer=0x" << static_cast<SI16>( GetLayer() ) << newLine;
	outStream << "Cont=0x" << GetContSerial() << newLine;
	outStream << "Creator=0x" << GetCreator() << newLine;
	outStream << "More=0x" << GetTempVar( CITV_MORE ) << newLine;
	outStream << "More012=0x" << GetTempVar( CITV_MORE0 ) << ",0x" << GetTempVar( CITV_MORE1 ) << ",0x" << GetTempVar( CITV_MORE2 ) << newLine;
	outStream << "MoreXYZ=0x" << GetTempVar( CITV_MOREX ) << ",0x" << GetTempVar( CITV_MOREY ) << ",0x" << GetTempVar( CITV_MOREZ ) << newLine;
	outStream << "Glow=0x" << GetGlow() << newLine;
	outStream << "GlowBC=0x" << GetGlowColour() << newLine;
	outStream << "Ammo=0x" << GetAmmoId() << ",0x" << GetAmmoHue() << newLine;
	outStream << "AmmoFX=0x" << GetAmmoFX() << ",0x" << GetAmmoFXHue() << ",0x" << GetAmmoFXRender() << newLine;
	outStream << "Spells=0x" << GetSpell( 0 ) << ",0x" << GetSpell( 1 ) << ",0x" << GetSpell( 2 ) << newLine;

	// Decimal / String Values
	outStream << std::dec;
	outStream << "Name2=" << GetName2() << newLine;
	outStream << "Desc=" << GetDesc() << newLine;
	outStream << "Event=" << GetEvent() << newLine;
	outStream << "Type=" + std::to_string( GetType() ) + newLine;
	outStream << "Offspell=" + std::to_string( GetOffSpell() ) + newLine;
	outStream << "Amount=" + std::to_string( GetAmount() ) + newLine;
	outStream << "WeightMax=" + std::to_string( GetWeightMax() ) + newLine;
	outStream << "BaseWeight=" + std::to_string( GetBaseWeight() ) + newLine;
	outStream << "MaxItems=" + std::to_string( GetMaxItems() ) + newLine;
	outStream << "MaxHP=" + std::to_string( GetMaxHP() ) + newLine;
	outStream << "Speed=" + std::to_string( GetSpeed() ) + newLine;
	outStream << "ArtifactRarity=" + std::to_string( GetArtifactRarity() ) + newLine;
	outStream << "Movable=" + std::to_string( GetMovable() ) + newLine;
	outStream << "Priv=" + std::to_string( GetPriv() ) + newLine;
	outStream << "LowerStatReq=" + std::to_string( GetLowerStatReq() ) + newLine;
	outStream << "Value=" + std::to_string( GetBuyValue() ) + "," + std::to_string( GetSellValue() ) + "," + std::to_string( GetVendorPrice() ) + newLine;
	outStream << "Restock=" + std::to_string( GetRestock() ) + newLine;
	outStream << "AC=" + std::to_string( GetArmourClass() ) + newLine;
	outStream << "Rank=" + std::to_string( GetRank() ) + newLine;
	outStream << "Sk_Made=" + std::to_string( GetMadeWith() ) + newLine;
	outStream << "Bools=" + std::to_string(( bools.to_ulong() )) + newLine;
	outStream << "Good=" + std::to_string( GetGood() ) + newLine;
	outStream << "GlowType=" + std::to_string( GetGlowEffect() ) + newLine;
	outStream << "Range=" + std::to_string( GetBaseRange() ) + "," + std::to_string(GetMaxRange() ) + newLine;
	outStream << "MaxUses=" + std::to_string( GetMaxUses() ) + newLine;
	outStream << "UsesLeft=" + std::to_string( GetUsesLeft() ) + newLine;
	outStream << "RaceDamage=" + std::to_string(static_cast<SI16>( GetWeatherDamage( LIGHT ) ? 1 : 0 )) + "," + std::to_string(static_cast<SI16>( GetWeatherDamage( RAIN ) ? 1 : 0 )) + ","
		+ std::to_string( static_cast<SI16>( GetWeatherDamage( HEAT ) ? 1 : 0 )) + "," + std::to_string( static_cast<SI16>( GetWeatherDamage( COLD ) ? 1 : 0 )) + ","
		+ std::to_string( static_cast<SI16>( GetWeatherDamage( SNOW ) ? 1 : 0 )) + "," + std::to_string( static_cast<SI16>( GetWeatherDamage( LIGHTNING ) ? 1 : 0 )) + newLine;
	outStream << "EntryMadeFrom=" + std::to_string( EntryMadeFrom() ) + newLine;
	outStream << "Stealable=" + std::to_string( GetStealable() ) + newLine;

	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::HandleLine()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles loading in tags and values from world files on startup
//o------------------------------------------------------------------------------------------------o
bool CItem::HandleLine( std::string &UTag, std::string &data )
{
	bool rValue = CBaseObject::HandleLine( UTag, data );
	if( !rValue )
	{
		auto csecs = oldstrutil::sections( data, "," );
		switch(( UTag.data()[0] ))
		{
			case 'A':
				if( UTag == "ARTIFACTRARITY" )
				{
					SetArtifactRarity( static_cast<SI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "AMMO" )
				{
					if( csecs.size() == 2 )
					{
						SetAmmoId( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
						SetAmmoHue( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					}
					else
					{
						SetAmmoId( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
						SetAmmoHue(( 0 ));
					}
					rValue = true;
				}
				else if( UTag == "AMMOFX" )
				{
					if( csecs.size() == 2 )
					{
						SetAmmoFX( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
						SetAmmoFXHue( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
						SetAmmoFXRender( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					}
					else
					{
						SetAmmoFX( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
						SetAmmoFXHue(( 0 ));
						SetAmmoFXRender(( 0 ));
					}
					rValue = true;
				}
				else if( UTag == "AMOUNT" )
				{
					amount = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
					rValue = true;
				}
				else if( UTag == "AC" )
				{
					SetArmourClass( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'B':
				if( UTag == "BOOLS" )
				{
					bools = static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
					rValue = true;
				}
				break;
			case 'C':
				if( UTag == "CONT" )
				{
					tempContainerSerial = static_cast<SERIAL>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
					rValue = true;
				}
				else if( UTag == "CREATOR" || UTag == "CREATER" )
				{
					SetCreator( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "CORPSE" )
				{
					SetCorpse( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				else if( UTag == "COLD" )
				{
					SetWeatherDamage( COLD, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				break;
			case 'D':
				if( UTag == "DESC" )
				{
					SetDesc( data.c_str() );
					rValue = true;
				}
				if( UTag == "DIR" )
				{
					SetDir( static_cast<SI08>( std::stoi(oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "DYEABLE" )
				{
					SetDye( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				break;
			case 'E':
				if( UTag == "ENTRYMADEFROM" )
				{
					EntryMadeFrom( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "EVENT" )
				{
					SetEvent( data.c_str() );
					rValue = true;
				}
				break;
			case 'G':
				if( UTag == "GRIDLOC" )
				{
					SetGridLocation( static_cast<SI08>( std::stoi(oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "GLOWTYPE" )
				{
					SetGlowEffect( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "GLOWBC" )
				{
					SetGlowColour( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "GLOW" )
				{
					SetGlow( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "GOOD" )
				{
					SetGood( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'H':
				if( UTag == "HEAT" )
				{
					SetWeatherDamage( HEAT, static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				break;
			case 'L':
				if( UTag == "LAYER" )
				{
					layer = static_cast<ItemLayers>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
					rValue = true;
				}
				else if( UTag == "LIGHT" )
				{
					SetWeatherDamage( LIGHT, static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				else if( UTag == "LIGHTNING" )
				{
					SetWeatherDamage( LIGHTNING, static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				else if( UTag == "LOWERSTATREQ" )
				{
					SetLowerStatReq( static_cast<SI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'M':
				if( UTag == "MAXITEMS" )
				{
					SetMaxItems( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "MORE" )
				{
					if( csecs.size() >= 4 )
					{
						SetTempVar( CITV_MORE, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
						SetTempVar( CITV_MORE, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
						SetTempVar( CITV_MORE, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 )));
						SetTempVar( CITV_MORE, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[3], "//" )), nullptr, 0 )));
					}
					else
					{
						SetTempVar( CITV_MORE, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					}
					rValue = true;
				}
				else if( UTag == "MORE2" )	// Depreciated
				{
					rValue = true;
				}
				else if( UTag == "MORE012" )
				{
					SetTempVar( CITV_MOREX, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
					SetTempVar( CITV_MOREY, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					SetTempVar( CITV_MOREZ, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "MOREXYZ" )
				{
					SetTempVar( CITV_MOREX, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
					SetTempVar( CITV_MOREY, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					SetTempVar( CITV_MOREZ, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "MOREX" )
				{
					if( csecs.size() >= 4 )
					{
						SetTempVar( CITV_MOREX, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
						SetTempVar( CITV_MOREX, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
						SetTempVar( CITV_MOREX, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 )));
						SetTempVar( CITV_MOREX, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[3], "//" )), nullptr, 0 )));
					}
					else
					{
						SetTempVar( CITV_MOREX, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					}
					rValue = true;
				}
				else if( UTag == "MOREY" )
				{
					if( csecs.size() >= 4 )
					{
						SetTempVar( CITV_MOREY, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
						SetTempVar( CITV_MOREY, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
						SetTempVar( CITV_MOREY, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 )));
						SetTempVar( CITV_MOREY, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[3], "//" )), nullptr, 0 )));
					}
					else
					{
						SetTempVar( CITV_MOREY, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					}
					rValue = true;
				}
				else if( UTag == "MOREZ" )
				{
					if( csecs.size() >= 4 )
					{
						SetTempVar( CITV_MOREZ, 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
						SetTempVar( CITV_MOREZ, 2, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
						SetTempVar( CITV_MOREZ, 3, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 )));
						SetTempVar( CITV_MOREZ, 4, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[3], "//" )), nullptr, 0 )));
					}
					else
					{
						SetTempVar( CITV_MOREZ, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					}
					rValue = true;
				}
				else if( UTag == "MURDERER" )
				{
					rValue = true;
				}
				else if( UTag == "MOVABLE" )
				{
					SetMovable( static_cast<SI08>( std::stoi(oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "MAXHP" )
				{
					SetMaxHP( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "MAXUSES" )
				{
					SetMaxUses( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'N':
				if( UTag == "NAME2" )
				{
					SetName2(data);
					rValue = true;
				}
				break;
			case 'O':
				if( UTag == "OFFSPELL" )
				{
					SetOffSpell( static_cast<SI08>( std::stoi(oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'P':
				if( UTag == "PRIV" )
				{
					SetPriv( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "PILEABLE" )
				{
					SetPileable( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				break;
			case 'R':
				if( UTag == "RESTOCK" )
				{
					SetRestock( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "RACEDAMAGE" )
				{
					SetWeatherDamage( LIGHT, static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )) == 1 );
					SetWeatherDamage( RAIN, static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )) == 1 );
					SetWeatherDamage( HEAT, static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 )) == 1 );
					SetWeatherDamage( COLD, static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[3], "//" )), nullptr, 0 )) == 1 );
					SetWeatherDamage( SNOW, static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[4], "//" )), nullptr, 0 )) == 1 );
					SetWeatherDamage( LIGHTNING, static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[5], "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				else if( UTag == "RANK" )
				{
					SetRank( static_cast<SI08>( std::stoi(oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "RAIN" )
				{
					SetWeatherDamage( RAIN, static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				else if( UTag == "RANGE" )
				{
					if( csecs.size() > 1 )
					{
						SetBaseRange( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
						SetMaxRange( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					}
					else
					{
						auto val = static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
						SetBaseRange( val );
						SetMaxRange( val / 2 );
					}
					rValue = true;
				}
				else if( UTag == "REPUTATION" )
				{
					rValue = true;
				}
				break;
			case 'S':
				if( UTag == "SPEED" )
				{
					SetSpeed( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "SK_MADE" )
				{
					SetMadeWith( static_cast<SI08>( std::stoi(oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "SNOW" )
				{
					SetWeatherDamage( SNOW, static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1 );
					rValue = true;
				}
				else if( UTag == "SPELLS" )
				{
					SetSpell( 0, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
					SetSpell( 1, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					SetSpell( 2, static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "STEALABLE" )
				{
					SetStealable( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'T':
				if( UTag == "TYPE" )
				{
					if( csecs.size() != 1 )
					{
						SetType( static_cast<ItemTypes>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
					}
					else
					{
						SetType( static_cast<ItemTypes>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					}
					rValue = true;
				}
				else if( UTag == "TYPE2" )
				{
					rValue = true;
				}
				break;
			case 'U':
				if( UTag == "USESLEFT" )
				{
					SetUsesLeft( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
			case 'V':
				if( UTag == "VALUE" )
				{
					if( csecs.size() > 2 )
					{
						SetBuyValue( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
						SetSellValue( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
						SetVendorPrice( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[2], "//" )), nullptr, 0 )));
					}
					else if( csecs.size() > 1 )
					{
						SetBuyValue( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
						SetSellValue( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					}
					else
					{
						auto val = static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
						SetBuyValue( val );
						SetSellValue( val / 2 );
					}
					rValue = true;
				}
				break;
			case 'W':
				if( UTag == "WEIGHTMAX" )
				{
					SetWeightMax( static_cast<SI32>( std::stoi(oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
					rValue = true;
				}
				break;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::LoadRemnants()
//|	Date		-	21st January, 2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	After handling data specific load, other parts go here
//o------------------------------------------------------------------------------------------------o
bool CItem::LoadRemnants( void )
{
	SetSerial( serial );

	// Tauriel adding region pointers
	if( !contObj || tempContainerSerial == INVALIDSERIAL )
	{
		auto [mapWidth, mapHeight] = Map->SizeOfMap( worldNumber );
		if( GetX() < 0 || GetY() < 0 || GetX() > mapWidth || GetY() > mapHeight )
			return false;

		// Calculate which townregion item exists in, based on its own location
		CTownRegion *tRegion = CalcRegionFromXY( GetX(), GetY(), worldNumber, instanceId, this );
		SetRegion(( tRegion != nullptr ? tRegion->GetRegionNum() : 0xFF ));
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsContType()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if item has a container type
//o------------------------------------------------------------------------------------------------o
auto CItem::IsContType() const -> bool
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::PostLoadProcessing()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to setup any pointers that may need adjustment following the loading of the world
//o------------------------------------------------------------------------------------------------o
void CItem::PostLoadProcessing( void )
{
	CBaseObject::PostLoadProcessing();
	// Add item weight if item doesn't have it yet
	if( GetWeight() < 0 || GetWeight() > MAX_WEIGHT )
	{
		SetWeight( Weight->CalcWeight( this ));
	}

	CBaseObject *tmpObj	= nullptr;
	contObj				= nullptr;
	
	if( tempContainerSerial != INVALIDSERIAL )
	{
		if( tempContainerSerial >= BASEITEMSERIAL )
		{
			tmpObj = CalcItemObjFromSer( tempContainerSerial );
		}
		else
		{
			tmpObj = CalcCharObjFromSer( tempContainerSerial );
		}
	}
	SetCont( tmpObj );

	Items->StoreItemRandomValue( this, nullptr );
	CheckItemIntegrity();
	SetPostLoaded( true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::CheckItemIntegrity()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Run some integrity checks on item
//o------------------------------------------------------------------------------------------------o
void CItem::CheckItemIntegrity( void )
{
	SERIAL getSerial = GetSerial();
	if( getSerial == INVALIDSERIAL )
	{
		Console.Warning( oldstrutil::format( "Item (%s, from DFN section [%s]) has an invalid serial number, Deleting", GetName().c_str(), GetSectionId().c_str() ));
		Delete();
		return;
	}

	if( getSerial == GetContSerial() )
	{
		Console.Warning( oldstrutil::format( "Item 0x%X (%s, from DFN section [%s]) has dangerous container value, Auto-Correcting", getSerial, GetName().c_str(), GetSectionId().c_str() ));
		SetCont( nullptr );
	}
	if( getSerial == GetOwner() )
	{
		Console.Warning( oldstrutil::format( "Item 0x%X (%s, from DFN section [%s]) has dangerous owner value, Auto-Correcting", getSerial, GetName().c_str(), GetSectionId().c_str() ));
		SetOwner( nullptr );
	}
	if( getSerial == GetSpawn() )
	{
		Console.Warning( oldstrutil::format( "Item 0x%X (%s, from DFN section [%s]) has dangerous spawner value, Auto-Correcting", getSerial, GetName().c_str(), GetSectionId().c_str() ));
		SetSpawn( INVALIDSERIAL );
	}

	if( type == IT_CONTAINER && GetLayer() == IL_PACKITEM && (contObj != nullptr && contObj->CanBeObjType( OT_CHAR )))
	{
		if( GetMaxItems() == 0 )
		{
			SetMaxItems( cwmWorldState->ServerData()->MaxPlayerPackItems() );
			Console.Warning( oldstrutil::format( "Container (%s, from DFN section [%s]) with maxItems set to 0 detected on character (%s). Resetting maxItems for container to default value.", std::to_string( GetSerial() ).c_str(), GetSectionId().c_str(), std::to_string( contObj->GetSerial() ).c_str() ));
		}
		if( GetWeightMax() == 0 )
		{
			SetWeightMax( cwmWorldState->ServerData()->MaxPlayerPackWeight() );
			Console.Warning( oldstrutil::format( "Container (%s, from DFN section [%s]) with weightMax set to 0 detected on character (%s). Resetting weightMax for container to default value.", std::to_string( GetSerial() ).c_str(), GetSectionId().c_str(), std::to_string( contObj->GetSerial() ).c_str() ));
		}
	}
}

const UI32 BIT_DECAYABLE	=	0;
const UI32 BIT_NEWBIE		=	1;
const UI32 BIT_DISPELLABLE	=	2;
const UI32 BIT_DIVINELOCK	=	3;

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsDecayable()
//|					CItem::SetDecayable()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether item can decay
//o------------------------------------------------------------------------------------------------o
auto CItem::IsDecayable() const -> bool
{
	return priv.test( BIT_DECAYABLE );
}
auto CItem::SetDecayable( bool newValue ) -> void
{
	if( newValue )
	{
		SetDecayTime( 0 );
	}

	priv.set( BIT_DECAYABLE, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsNewbie()
//|					CItem::SetNewbie()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether item is marked as a newbie item (doesn't drop on death)
//o------------------------------------------------------------------------------------------------o
auto CItem::IsNewbie() const -> bool
{
	return priv.test( BIT_NEWBIE );
}
auto CItem::SetNewbie( bool newValue ) -> void
{
	priv.set( BIT_NEWBIE, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsDispellable()
//|					CItem::SetDispellable()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether item is dispellable (part of a field spell, for instance)
//o------------------------------------------------------------------------------------------------o
auto CItem::IsDispellable() const -> bool
{
	return priv.test( BIT_DISPELLABLE );
}
auto CItem::SetDispellable( bool newValue ) -> void
{
	priv.set( BIT_DISPELLABLE, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsDivineLocked()
//|					CItem::SetDivineLock()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets whether container was locked by a GM
//o------------------------------------------------------------------------------------------------o
auto CItem::IsDivineLocked() const -> bool
{
	return priv.test( BIT_DIVINELOCK );
}
auto CItem::SetDivineLock( bool newValue ) -> void
{
	priv.set( BIT_DIVINELOCK, newValue );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetStealable()
//|					CItem::SetStealable()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets property that determines under which conditions item can be stolen
//o------------------------------------------------------------------------------------------------o
auto CItem::GetStealable() const -> UI08
{
	return stealable;
}
auto CItem::SetStealable( UI08 newValue ) -> void
{
	stealable = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::EntryMadeFrom()
//|	Date		-	13 September, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Create-DFNs entry the item was made from
//o------------------------------------------------------------------------------------------------o
auto CItem::EntryMadeFrom() const -> UI16
{
	return entryMadeFrom;
}
auto CItem::EntryMadeFrom( UI16 newValue ) -> void
{
	entryMadeFrom = newValue;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::SetWeight()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the weight of the item
//o------------------------------------------------------------------------------------------------o
void CItem::SetWeight( SI32 newVal, bool doWeightUpdate )
{
	CBaseObject *checkCont = nullptr;
	if( IsPostLoaded() && doWeightUpdate )
	{
		checkCont = GetCont();
	}

	if( ValidateObject( checkCont ))
	{
		Weight->SubtractItemWeight( checkCont, this );
	}

	weight = newVal;

	if( ValidateObject( checkCont ))
	{
		Weight->AddItemWeight( checkCont, this );
	}
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsMetalType()
//|	Date		-	13 November, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if it's a metal object
//o------------------------------------------------------------------------------------------------o
auto CItem::IsMetalType() const -> bool
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::IsLeatherType()
//|	Date		-	13 November, 2001
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if it's a leather object
//o------------------------------------------------------------------------------------------------o
auto CItem::IsLeatherType() const -> bool
{
	if( id >= 0x13C6 && id <= 0x13E2 )
		return true;
	if( id >= 0x144E && id <= 0x1457 )
		return true;
	return false;
}

inline bool operator == ( const CItem& x, const CItem& y )
{
	return ( x.GetSerial() == y.GetSerial() );
}

inline bool operator < ( const CItem& x, const CItem& y )
{
	return ( x.GetSerial() < y.GetSerial() );
}

inline bool operator > ( const CItem& x, const CItem& y )
{
	return ( x.GetSerial() > y.GetSerial() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::TextMessage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Display "speech" over an item
//o------------------------------------------------------------------------------------------------o
auto CItem::TextMessage( CSocket *s, SI32 dictEntry, R32 secsFromNow, UI16 Colour ) -> void
{
	UnicodeTypes dictLang	= ZERO;
	SERIAL speakTo			= INVALIDSERIAL;
	SpeechTarget target		= SPTRG_PCNPC;
	CChar *mChar			= nullptr;
	if( s != nullptr )
	{
		dictLang		= s->Language();
		mChar			= s->CurrcharObj();
		speakTo			= mChar->GetSerial();
		target			= SPTRG_INDIVIDUAL;
	}

	std::string txt = Dictionary->GetEntry( dictEntry, dictLang );
	if( txt.empty() )
		return;

	if( s != nullptr && cwmWorldState->ServerData()->UseUnicodeMessages() )
	{
		[[maybe_unused]] bool sendAll = true;
		if( target == SPTRG_INDIVIDUAL )
		{
			sendAll = false;
		}

		if( Colour == 0x0 || Colour == 0x1700 )
		{
			Colour = 0x03B2;
		}

		CPUnicodeMessage unicodeMessage;
		unicodeMessage.Message( txt );
		unicodeMessage.Font( FNT_NORMAL );
		unicodeMessage.Colour( 0x000B );
		unicodeMessage.Type( SYSTEM );
		unicodeMessage.Language( "ENG" );
		unicodeMessage.Name( GetNameRequest( mChar, NRS_SPEECH ));
		unicodeMessage.ID( GetId() );
		unicodeMessage.Serial( GetSerial() );

		s->Send( &unicodeMessage );
	}
	else
	{
		CSpeechEntry& toAdd = SpeechSys->Add();
		toAdd.Speech( txt );
		toAdd.Font( FNT_NORMAL );
		toAdd.Speaker( GetSerial() );
		toAdd.SpokenTo( speakTo );
		toAdd.Type( OBJ );
		toAdd.At( BuildTimeValue( secsFromNow ));
		toAdd.TargType( target );
		if( Colour == 0x0 || Colour == 0x1700)
		{
			toAdd.Colour( 0x03B2 );
		}
		else
		{
			toAdd.Colour( Colour );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::Update()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send this item to specified socket or all online people in range
//o------------------------------------------------------------------------------------------------o
void CItem::Update( [[maybe_unused]] CSocket *mSock, [[maybe_unused]] bool drawGamePlayer, [[maybe_unused]] bool sendToSelf )
{
	if( GetType() == IT_TRADEWINDOW )
		return;

	//RemoveFromSight( mSock );
	// Note: To monitor: Commented out RemoveFromSight() in CItem::Update() to potentially fix a lot of flickering issues with animated items, boats, etc.

	if( GetCont() == this )
	{
		Console.Warning( oldstrutil::format( "Item %s(0x%X) has a dangerous container value, auto-correcting", GetName().c_str(), GetSerial() ));
		SetCont( nullptr );
	}

	CBaseObject *iCont = GetCont();
	if( iCont == nullptr )
	{
		std::vector<CSocket *> nearbyChars;
		if( GetId( 1 ) >= 0x40 )
		{
			nearbyChars = FindNearbyPlayers( this, DIST_BUILDRANGE );
		}
		else
		{
			nearbyChars = FindPlayersInVisrange( this );
		}
		std::for_each( nearbyChars.begin(), nearbyChars.end(), [this]( CSocket *tSock )
		{
			SendToSocket( tSock );
		});
		return;
	}
	else if( iCont->GetObjType() == OT_CHAR )
	{
		CChar *charCont = static_cast<CChar *>( iCont );
		if( charCont != nullptr )
		{
			CPWornItem toWear = ( *this );
			auto nearbyChar = FindNearbyPlayers( charCont );
			for( auto &tSock : nearbyChar )
			{
				if( tSock->LoginComplete() )
				{
					tSock->Send( &toWear );
					
					// Only send tooltip if server feature for tooltips is enabled
					if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_AOS ))
					{
						CPToolTip pSend( GetSerial(), tSock );
						tSock->Send( &pSend );
					}
				}
			}
			return;
		}
	}
	else
	{
		CItem *itemCont = static_cast<CItem *>( iCont );
		if( itemCont != nullptr )
		{
			ObjectType oType = OT_CBO;
			for( auto &tSock: FindNearbyPlayers( FindItemOwner( this, oType ), DIST_NEARBY ))
			{
				if( tSock->LoginComplete() )
				{
					SendPackItemToSocket( tSock );
				}
			}
			return;
		}
	}
	Console.Error( oldstrutil::format( " CItem::Update(0x%X): cannot determine container type!", GetSerial() ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::SendToSocket()
//|	Date		-	July 27, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates an item on the ground to specified socket
//o------------------------------------------------------------------------------------------------o
void CItem::SendToSocket( CSocket *mSock, [[maybe_unused]] bool drawGamePlayer )
{
	if( !mSock->LoginComplete() )
		return;

	CChar *mChar = mSock->CurrcharObj();
	if( mChar != nullptr )
	{
		if( !mChar->IsGM() )
		{
			// Not the owner, nor a GM
			auto iVisible = GetVisible();
			auto ownerObj = GetOwnerObj();
			if(( iVisible != VT_VISIBLE && iVisible != VT_TEMPHIDDEN )
				|| ( iVisible == VT_TEMPHIDDEN &&
				( !ValidateObject( ownerObj ) || ( ValidateObject( ownerObj ) && ownerObj->GetSerial() != mChar->GetSerial() ))))
			{
				return;
			}
		}
		if( mSock->ClientType() >= CV_SA2D )
		{
			CPNewObjectInfo toSend(( *this ), ( *mChar ));
			mSock->Send( &toSend );
		}
		else
		{
			CPObjectInfo toSend(( *this ), ( *mChar ));
			mSock->Send( &toSend );
		}
		if( IsCorpse() )
		{
			CPCorpseClothing cpcc( this );
			mSock->Send( &cpcc );
			CPItemsInContainer itemsIn( mSock, this, 0x01 );
			mSock->Send( &itemsIn );
		}
		if( !CanBeObjType( OT_MULTI ))
		{
			// Only send tooltip if server feature for tooltips is enabled
			if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_AOS ))
			{
				CPToolTip pSend( GetSerial(), mSock );
				mSock->Send( &pSend );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::SendPackItemToSocket()
//|	Date		-	July 27, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates an item contained in a pack to specified socket
//o------------------------------------------------------------------------------------------------o
auto CItem::SendPackItemToSocket( CSocket *mSock ) -> void
{
	CChar *mChar = mSock->CurrcharObj();
	if( mChar != nullptr )
	{
		bool isGM = mChar->IsGM();
		ItemLayers iLayer = GetLayer();
		if( !isGM && ( iLayer == IL_SELLCONTAINER || iLayer == IL_BOUGHTCONTAINER || iLayer == IL_BUYCONTAINER ))
			return;

		CPAddItemToCont itemSend;
		if( mSock->ClientVerShort() >= CVS_6017 )
		{
			itemSend.UOKRFlag( true );
		}
		itemSend.Object(( *this ));
		if( isGM && GetId() == 0x1647 )
		{
			itemSend.Model( 0x0A0F );
			itemSend.Colour( 0x00C6 );
		}
		mSock->Send( &itemSend );
		// Only send tooltip if server feature for tooltips is enabled
		if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_AOS ))
		{
			CPToolTip pSend( GetSerial(), mSock );
			mSock->Send( &pSend );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::RemoveFromSight()
//|	Date		-	September 7th, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops through all online chars and removes the item from their sight
//o------------------------------------------------------------------------------------------------o
void CItem::RemoveFromSight( CSocket *mSock )
{
	CPRemoveItem toRemove	= ( *this );
	CBaseObject *iCont		= GetCont();

	ObjectType oType	= OT_CBO;
	CBaseObject *iOwner	= nullptr;
	if( this->GetOwner() != INVALIDSERIAL )
	{
		iOwner = FindItemOwner( this, oType );
	}

	if( iCont == nullptr || oType == OT_ITEM )
	{
		CItem *rItem = nullptr;
		if( iCont == nullptr )
		{
			rItem = this;
		}
		else
		{
			rItem = static_cast<CItem *>( iOwner );
		}
		if( rItem != nullptr )
		{
			if( mSock != nullptr )
			{
				mSock->Send( &toRemove );
			}
			else
			{
				CChar *tChar			= nullptr;
				std::vector<CSocket *> nearbyChars;
				if( rItem == this )
				{
					nearbyChars = FindPlayersInOldVisrange( rItem );
				}
				else
				{
					nearbyChars = FindPlayersInVisrange( rItem );
				}
				for( auto &tSock : nearbyChars )
				{
					if( tSock->LoginComplete() )
					{
						tChar = tSock->CurrcharObj();
						if( ValidateObject( tChar ))
						{
							tSock->Send( &toRemove );
						}
					}
				}
			}
		}
	}
	else if( iCont->GetObjType() == OT_CHAR || oType == OT_CHAR )
	{
		CChar *rChar = nullptr;
		if( iCont->GetObjType() == OT_CHAR )
		{
			rChar = static_cast<CChar *>( iCont );
		}
		else
		{
			rChar = static_cast<CChar *>( iOwner );
		}
		if( rChar != nullptr )
		{
			if( mSock != nullptr )
			{
				mSock->Send( &toRemove );
			}
			else
			{
				for( auto &tSock : FindNearbyPlayers( rChar ))
				{
					if( tSock->LoginComplete() )
					{
						tSock->Send( &toRemove );
					}
				}
			}
		}
	}
	else
	{
		if( mSock != nullptr )
		{
			mSock->Send( &toRemove );
		}
		else
		{
			// Iterate through list of players who have opened the container the item was in
			auto itemCont = static_cast<CItem *>( iCont );
			auto contOpenedByList = itemCont->GetContOpenedByList();
			for( const auto &oSock : contOpenedByList->collection() )
			{
				// For any of those players who are still within range, send remove item packet
				// We don't know if they still have eyes on the inside of the container, but we
				// can still minimize sending the packet to those who
				//  A) opened the container at some point, and are still on the container's list of players who opened it
				//  B) are within range of the container
				// have to rely on range to cover it instead
				if( oSock != nullptr && oSock->LoginComplete() )
				{
					auto oChar = oSock->CurrcharObj();
					if( ValidateObject( oChar ))
					{
						// Get character's visual range
						auto visRange = static_cast<UI16>( oSock->Range() + Races->VisRange( oChar->GetRace() ));

						// Find the root container (if any) of the container
						auto rootCont = FindRootContainer( itemCont );
						if( !ValidateObject( rootCont ))
						{
							rootCont = itemCont;
						}

						// Find the owner of the root container (if for instance it's a container inside player's backpack - or their backpack)
						CChar *rootContOwner = FindItemOwner( rootCont );

						CBaseObject *objToCheck = nullptr;
						if( ValidateObject( rootContOwner ))
						{
							objToCheck = static_cast<CBaseObject *>( rootContOwner );
						}
						else
						{
							objToCheck = static_cast<CBaseObject *>( rootCont );
						}

						// If owner of root container exists, use that in range check
						if( ObjInRangeSquare( objToCheck, oChar, visRange ))
						{
							oSock->Send( &toRemove );
						}
					}
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::PlaceInPack()
//|	Date		-	October 18th, 2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Puts an item at a random location inside a pack
//o------------------------------------------------------------------------------------------------o
auto CItem::PlaceInPack() -> void
{
	auto itemCont = this->GetCont();
	if( !ValidateObject( itemCont ))
		return;

	PackTypes packType = Items->GetPackType( static_cast<CItem *>( itemCont ));
	switch( packType )
	{
		case PT_PACK:
			SetX(( RandomNum( 44, 142 )));
			SetY(( RandomNum( 65, 127 )));
			break;
		case PT_BAG:
			SetX(( RandomNum( 29, 93 )));
			SetY(( RandomNum( 34, 96 )));
			break;
		case PT_SQBASKET:
			SetX(( RandomNum( 19, 138 )));
			SetY(( RandomNum( 47, 91 )));
			break;
		case PT_RBASKET:
			SetX(( RandomNum( 40, 95 )));
			SetY(( RandomNum( 30, 91 )));
			break;
		case PT_SEBASKET:
			SetX(( RandomNum( 10, 112 )));
			SetY(( RandomNum( 10, 73 )));
			break;
		case PT_BOOKCASE:
			SetX(( RandomNum( 13, 36 )));
			SetY(( RandomNum( 76, 96 )));
			break;
		case PT_FARMOIRE:
		case PT_WARMOIRE:
			SetX(( RandomNum( 24, 56 )));
			SetY(( RandomNum( 18, 120 )));
			break;
		case PT_DRAWER:
		case PT_DRESSER:
			SetX(( RandomNum( 16, 110 )));
			SetY(( RandomNum( 10, 62 )));
			break;
		case PT_SECHEST1:
		case PT_SECHEST2:
		case PT_SECHEST3:
		case PT_SECHEST4:
		case PT_SECHEST5:
		case PT_SEARMOIRE1:
		case PT_SEARMOIRE2:
		case PT_SEARMOIRE3:
			SetX(( RandomNum( 10, 115 )));
			SetY(( RandomNum( 10, 73 )));
			break;
		case PT_MBOX:
		case PT_WBOX:
			SetY(( RandomNum( 51, 92 )));
			SetX(( RandomNum( 16, 140 )));
			break;
		case PT_BARREL:
			SetY(( RandomNum( 36, 116 )));
			SetX(( RandomNum( 33, 98 )));
			break;
		case PT_CRATE:
			SetY(( RandomNum( 10, 68 )));
			SetX(( RandomNum( 20, 126 )));
			break;
		case PT_WCHEST:
		case PT_SCHEST:
		case PT_GCHEST:
			SetY(( RandomNum( 105, 139 )));
			SetX(( RandomNum( 18, 118 )));
			break;
		case PT_COFFIN:
		case PT_SHOPPACK:
		case PT_PACK2:
		case PT_BANK:
		case PT_UNKNOWN:
		default:
			SetX( static_cast<SI16>( 25 + RandomNum( 0, 79 )));
			SetY( static_cast<SI16>( 25 + RandomNum( 0, 79 )));
			break;
	}
	SetZ( 9 );
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::GetSpell()
//|					CItem::SetSpell()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets spells in a spellbook item
//o------------------------------------------------------------------------------------------------o
auto CItem::GetSpell( UI08 part ) const -> UI32
{
	UI32 rValue = 0;
	if( part < 3 )
	{
		rValue = spells[part];
	}
	return rValue;
}
auto CItem::SetSpell( UI08 part, UI32 newValue ) -> void
{
	if( part < 3 )
	{
		spells[part] = newValue;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::Cleanup()
//|	Date		-	11/6/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Cleans up after item
//o------------------------------------------------------------------------------------------------o
void CItem::Cleanup( void )
{
	if( !IsFree() )	// We're not the default item in the handler
	{
		MapRegion->RemoveItem( this );

		JSEngine->ReleaseObject( IUE_ITEM, this );

		CBaseObject::Cleanup();

		CBaseObject *iCont = GetCont();
		RemoveFromSight();
		RemoveSelfFromCont();

		// If a corpse, with valid player owner, remove from owner's list of corpses
		if( IsCorpse() )
		{
			CChar *iOwner = GetOwnerObj();
			if( ValidateObject( iOwner ) && !iOwner->IsNpc() )
			{
				iOwner->GetOwnedCorpses()->Remove( this );
			}
		}

		RemoveSelfFromOwner();

		for( CItem *tItem = Contains.First(); !Contains.Finished(); tItem = Contains.Next() )
		{
			if( ValidateObject( tItem ))
			{
				tItem->Delete();
			}
		}
		// Iterate through list of players that previously opened container, and remove self from those players
		auto contOpenedBy = GetContOpenedByList();
		for( const auto &pSock : contOpenedBy->collection() )
		{
			if( pSock != nullptr )
			{
				// Remove player from container's own list of players who have previously opened it
				pSock->GetContsOpenedList()->Remove( this );
			}
		}
		GetContOpenedByList()->Clear();	// Clear container's list of sockets that opened it previously

		// if we delete an item we should delete it from spawnregions
		// this will fix several crashes
		if( IsSpawned() )
		{
			if( GetSpawn() < BASEITEMSERIAL )
			{
				UI16 spawnRegNum = static_cast<UI16>( GetSpawn() );
				if( cwmWorldState->spawnRegions.find( spawnRegNum ) != cwmWorldState->spawnRegions.end() )
				{
					CSpawnRegion *spawnReg = cwmWorldState->spawnRegions[spawnRegNum];
					if( spawnReg != nullptr )
					{
						spawnReg->DeleteSpawnedItem( this );
					}
				}
			}
			SetSpawn( INVALIDSERIAL );
		}
		if( GetSpawnObj() != nullptr )
		{
			SetSpawn( INVALIDSERIAL );
		}

		if( GetGlow() != INVALIDSERIAL )
		{
			CItem *j = CalcItemObjFromSer( GetGlow() );
			if( ValidateObject( j ))
			{
				j->Delete();  // glow stuff, deletes the glower of a glowing stuff automatically
			}
		}

		if( IsGuarded() )
		{
			CChar *owner = nullptr;
			CMultiObj *multi = FindMulti( this );
			if( ValidateObject( multi ))
			{
				owner = multi->GetOwnerObj();
			}
			if( !ValidateObject( owner ))
			{
				owner = FindItemOwner( this );
			}
			if( ValidateObject( owner ))
			{
				CChar *petGuard = Npcs->GetGuardingFollower( owner, this );
				if( ValidateObject( petGuard ))
				{
					petGuard->SetGuarding( nullptr );
				}
				SetGuarded( false );
			}
		}

		if( GetType() == IT_MESSAGEBOARD )
		{
			MsgBoardRemoveFile( GetSerial() );
		}

		if( GetType() == IT_BOOK && ( GetTempVar( CITV_MOREX ) == 666 || GetTempVar( CITV_MOREX ) == 999 ))
		{
			Books->DeleteBook( this );
		}

		// Update container tooltip for nearby players
		if( ValidateObject( iCont ) && iCont->GetObjType() == OT_ITEM )
		{
			CChar *rootOwner = FindItemOwner( static_cast<CItem *>( iCont ));
			if( ValidateObject( rootOwner ) && rootOwner->GetObjType() == OT_CHAR )
			{
				CSocket *ownerSocket = rootOwner->GetSocket();
				if( ownerSocket != nullptr )
				{
					// Only send tooltip if server feature for tooltips is enabled
					if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_AOS ))
					{
						// Refresh container tooltip
						CPToolTip pSend( iCont->GetSerial(), ownerSocket );
						ownerSocket->Send( &pSend );
					}
				}
			}
			else
			{
				for( auto &tSock : FindNearbyPlayers( iCont, DIST_NEARBY ))
				{
					if( tSock->LoginComplete() )
					{
						// Only send tooltip if server feature for tooltips is enabled
						if( cwmWorldState->ServerData()->GetServerFeature( SF_BIT_AOS ))
						{
							// Refresh container tooltip
							CPToolTip pSend( iCont->GetSerial(), tSock );
							tSock->Send( &pSend );
						}
					}
				}
			}
		}

		//Ensure that object is removed from refreshQueue
		RemoveFromRefreshQueue();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::UpdateRegion()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Marks region item exists in as updated since last save
//o------------------------------------------------------------------------------------------------o
auto CItem::UpdateRegion() -> void
{
	// Make sure to only mark region as changed if item is supposed to be saved
	if( ShouldSave() )
	{
		CMapRegion *curCell = MapRegion->GetMapRegion( this );
		curCell->HasRegionChanged( true );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::CanBeObjType()
//|	Date		-	24 June, 2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Indicates whether an object can behave as a particular type
//o------------------------------------------------------------------------------------------------o
bool CItem::CanBeObjType( ObjectType toCompare ) const
{
	bool rValue = CBaseObject::CanBeObjType( toCompare );
	if( !rValue )
	{
		if( toCompare == OT_ITEM )
		{
			rValue = true;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CItem::Delete()
//|	Date		-	11/6/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds item to deletion queue
//o------------------------------------------------------------------------------------------------o
void CItem::Delete( void )
{
	if( cwmWorldState->deletionQueue.count( this ) == 0 )
	{
		++( cwmWorldState->deletionQueue[this] );
		Cleanup();
		SetDeleted( true );
		ShouldSave( false );
		UpdateRegion();
	}
}

auto CItem::GetContainsList() -> GenericList<CItem *> *
{
	return &Contains;
}

auto CItem::GetContOpenedByList() -> GenericList<CSocket *> *
{
	return &contOpenedBy;
}

//o------------------------------------------------------------------------------------------------o
//|	Class		-	CSpawnItem::CSpawnItem()
//|	Date		-	29th June, 2004
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Version History
//|
//|						1.0		 		29th June, 2004
//|						Original implementation
//|						First attempt to begin breaking CItem into multiple smaller
//|						classes, reducing overall memory usage. Currently stores all
//|						objects spawned by an item.
//o------------------------------------------------------------------------------------------------o
CSpawnItem::CSpawnItem() : CItem(),
isSectionAList( false )
{
	objType = OT_SPAWNER;
	spawnInterval[0] = spawnInterval[1] = 0;
	spawnSection.reserve( 100 );
	spawnSection = "";
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnItem::GetInterval()
//|					CSpawnItem::SetInterval()
//|	Date		-	6/29/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets Min and Max interval for spawner to respawn
//o------------------------------------------------------------------------------------------------o
auto CSpawnItem::GetInterval( UI08 part ) const -> UI08
{
	UI08 rValue = 0;
	if( part < 2 )
	{
		rValue = spawnInterval[part];
	}
	return rValue;
}
auto CSpawnItem::SetInterval( UI08 part, UI08 newVal ) -> void
{
	if( part < 2 )
	{
		spawnInterval[part] = newVal;
		UpdateRegion();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnItem::GetSpawnSection()
//|					CSpawnItem::SetSpawnSection()
//|	Date		-	6/29/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets Script section to spawn from
//o------------------------------------------------------------------------------------------------o
auto CSpawnItem::GetSpawnSection() const -> std::string
{
	return spawnSection;
}
auto CSpawnItem::SetSpawnSection( const std::string &newVal ) -> void
{
	spawnSection = newVal;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnItem::IsSectionAList()
//|	Date		-	7/05/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets script section as a spawner list
//o------------------------------------------------------------------------------------------------o
auto CSpawnItem::IsSectionAList() const -> bool
{
	return isSectionAList;
}
auto CSpawnItem::IsSectionAList( bool newVal ) -> void
{
	isSectionAList = newVal;
	UpdateRegion();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnItem::DumpHeader()
//|	Date		-	6/29/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dumps Header to Worldfile
//o------------------------------------------------------------------------------------------------o
bool CSpawnItem::DumpHeader( std::ostream &outStream ) const
{
	outStream << "[SPAWNITEM]" << '\n';
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnItem::DumpBody()
//|	Date		-	6/29/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dumps Spawn Item to Worldfile
//o------------------------------------------------------------------------------------------------o
bool CSpawnItem::DumpBody( std::ostream &outStream ) const
{
	CItem::DumpBody( outStream );
	outStream << "Interval=" << static_cast<UI16>( GetInterval( 0 )) << "," << static_cast<UI16>( GetInterval( 1 )) << '\n';
	outStream << "SpawnSection=" << GetSpawnSection() << '\n';
	outStream << "IsSectionAList=" << static_cast<UI16>( IsSectionAList() ? 1 : 0 ) << '\n';
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnItem::HandleLine()
//|	Date		-	6/29/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reads data from Worldfile into the class
//o------------------------------------------------------------------------------------------------o
bool CSpawnItem::HandleLine( std::string &UTag, std::string &data )
{
	bool rValue = CItem::HandleLine( UTag, data );
	if( !rValue )
	{
		auto csecs = oldstrutil::sections( data, "," );
		switch(( UTag.data()[0] ))
		{
			case 'I':
				if( UTag == "INTERVAL" )
				{
					SetInterval( 0, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 )));
					SetInterval( 1, static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )));
					rValue = true;
				}
				else if( UTag == "ISSECTIONALIST" )
				{
					IsSectionAList(( static_cast<UI08>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 1) );
					rValue = true;
				}
				break;
			case 'S':
				if( UTag == "SPAWNSECTION" )
				{
					SetSpawnSection( data );
					rValue = true;
				}
				break;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnItem::DoRespawn()
//|	Date		-	6/29/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Will eventually take the place of RespawnItem()
//o------------------------------------------------------------------------------------------------o
auto CSpawnItem::DoRespawn() -> bool
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
auto CSpawnItem::HandleItemSpawner() -> bool
{
	bool shouldSpawn = true;
	for( CBaseObject *mObj = spawnedList.First(); !spawnedList.Finished(); mObj = spawnedList.Next() )
	{
		if( ValidateObject( mObj ) && !mObj->IsFree() )
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
		{
			Items->AddRespawnItem( this, listObj, false, IsSectionAList(), 1 );
		}
		else if( GetTempVar( CITV_MOREX ) != 0 )
		{
			Items->AddRespawnItem( this, oldstrutil::number( GetTempVar( CITV_MOREX )), false, 1 );
		}
		else
		{
			Console.Warning( "Bad Item Spawner Found, Deleting" );
			Delete();
			return true;
		}
	}
	return false;
}

auto CSpawnItem::HandleNPCSpawner() -> bool
{
	if( spawnedList.Num() < GetAmount() )
	{
		std::string listObj = GetSpawnSection();
		if( !listObj.empty() )
		{
			Npcs->CreateNPC( this, listObj );
		}
		else if( GetTempVar( CITV_MOREX ) != 0 )
		{
			Npcs->CreateNPC( this, oldstrutil::number( GetTempVar( CITV_MOREX )));
		}
		else
		{
			Console.Warning( "Bad Npc/Area Spawner found; SPAWNSECTION or MOREX values missing! Deleting Spawner." );
			Delete();
			return true;
		}
	}
	return false;
}
auto CSpawnItem::HandleSpawnContainer() -> bool
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
		{
			std::string sect	= "ITEMLIST " + listObj;
			sect				= oldstrutil::trim( oldstrutil::removeTrailing( sect, "//" ));

			// Look up the relevant ITEMLIST from DFNs
			CScriptSection *itemList = FileLookup->FindEntry( sect, items_def );
			if( itemList != nullptr )
			{
				// Count the number of entries in the list
				const size_t itemListSize = itemList->NumEntries();
				if( itemListSize > 0 )
				{
					// Spawn one instance of EACH entry in the list
					std::string listEntry = "";
					for( size_t i = 0; i < itemListSize; i++ )
					{
						// listObj will either contain an itemID and amount, or an itemlist/lootlist tag
						auto listObj = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( itemList->MoveTo( i ), "//" )), "," );
						if( !listObj.empty() )
						{
							UI16 amountToSpawn = 1;
							std::vector<std::string> itemListData;
							if( oldstrutil::upper( listObj[0] ) == "ITEMLIST" || oldstrutil::upper( listObj[0] ) == "LOOTLIST" )
							{
								bool useLootList = oldstrutil::upper( listObj[0] ) == "LOOTLIST";

								// Itemlist/Lootlist
								itemListData = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( itemList->GrabData(), "//" )), "," );
								listEntry = itemListData[0];

								if( itemListData.size() > 1 )
								{
									// Also grab amount
									std::string amountData = oldstrutil::trim( oldstrutil::removeTrailing( itemListData[1], "//" ));
									auto tsects = oldstrutil::sections( amountData, " " );
									if( tsects.size() > 1 ) // check if the second part of the tag-data contains two sections separated by a space
									{
										auto first = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( tsects[0], "//" )), nullptr, 0 ));
										auto second = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( tsects[1], "//" )), nullptr, 0 ));

										// Tag contained a minimum and maximum value for amount! Let's randomize!
										amountToSpawn = static_cast<UI16>( RandomNum( first, second ));
									}
									else
									{
										amountToSpawn = static_cast<UI16>( std::stoul( amountData, nullptr, 0 ));
									}
								}

								// The chosen entry contained another ITEMLIST or LOOTLIST reference! Let's dive back into it...
								for( int i = 0; i < amountToSpawn; i++ )
								{
									CItem *iCreated = Items->CreateRandomItem( this, listEntry, this->WorldNumber(), this->GetInstanceId(), false, useLootList );
									if( ValidateObject( iCreated ))
									{
										// Place item in container and randomize location
										iCreated->SetCont( this );
										iCreated->PlaceInPack();
									}
								}
							}
							else
							{
								// Direct item reference
								listEntry = listObj[0];

								if( listObj.size() > 1 )
								{
									// Grab amount
									std::string amountData = oldstrutil::trim( oldstrutil::removeTrailing( listObj[1], "//" ));
									auto tsects = oldstrutil::sections( amountData, " " );
									if( tsects.size() > 1 ) // check if the second part of the tag-data contains two sections separated by a space
									{
										auto first = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( tsects[0], "//" )), nullptr, 0 ));
										auto second = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( tsects[1], "//" )), nullptr, 0 ));

										// Tag contained a minimum and maximum value for amount! Let's randomize!
										amountToSpawn = static_cast<UI16>( RandomNum( first, second ));
									}
									else
									{
										amountToSpawn = static_cast<UI16>( std::stoul( amountData, nullptr, 0 ));
									}
								}

								// We have a direct item reference, it seems like. Spawn it!
								CItem *iCreated = Items->CreateBaseScriptItem( this, listEntry, this->WorldNumber(), amountToSpawn, this->GetInstanceId(), OT_ITEM, 0xFFFF, false );
								if( ValidateObject( iCreated ))
								{
									// Place item in container and randomize location
									iCreated->SetCont( this );
									iCreated->PlaceInPack();

									if( amountToSpawn > 1 && !iCreated->IsPileable() )
									{
										// Eee, item cannot pile, we need to spawn individual ones
										for( int i = 1; i < amountToSpawn; i++ )
										{
											CItem *iCreated2 = Items->CreateBaseScriptItem( this, listEntry, this->WorldNumber(), 1, this->GetInstanceId(), OT_ITEM, 0xFFFF, false );
											if( ValidateObject( iCreated2 ))
											{
												// Place item in container and randomize location
												iCreated2->SetCont( this );
												iCreated2->PlaceInPack();
											}
										}
									}
								}
							}	
						}
					}
				}
			}
		}
		else if( GetTempVar( CITV_MOREX ) != 0 )
		{
			Items->AddRespawnItem( this, oldstrutil::number( GetTempVar( CITV_MOREX )), true, 1 );
		}
		else
		{
			Console.Warning( "Bad Spawn Container found; missing SPAWNSECTION or MOREX! Deleting Spawner." );
			Delete();
			return true;
		}
		RemoveFromSight();
		Update();
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnItem::Cleanup()
//|	Date		-	6/29/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Cleans up after spawner item
//o------------------------------------------------------------------------------------------------o
void CSpawnItem::Cleanup( void )
{
	CItem::Cleanup();

	for( CBaseObject *mObj = spawnedList.First(); !spawnedList.Finished(); mObj = spawnedList.Next() )
	{
		if( mObj->GetSpawnObj() == this )
		{
			mObj->SetSpawn( INVALIDSERIAL );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnItem::CanBeObjType()
//|	Date		-	24 June, 2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Indicates whether an object can behave as a particular type
//o------------------------------------------------------------------------------------------------o
bool CSpawnItem::CanBeObjType( ObjectType toCompare ) const
{
	bool rValue = CItem::CanBeObjType( toCompare );
	if( !rValue )
	{
		if( toCompare == OT_SPAWNER )
		{
			rValue = true;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnItem::Dupe()
//|	Date		-	7/11/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a new CSpawnItem and copies all of this objects properties to the new one
//o------------------------------------------------------------------------------------------------o
auto CSpawnItem::Dupe() -> CSpawnItem *
{
	CSpawnItem *target = static_cast<CSpawnItem *> (ObjectFactory::GetSingleton().CreateObject( OT_SPAWNER ));
	if( target == nullptr )
		return nullptr;

	CopyData( target );

	return target;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSpawnItem::CopyData()
//|	Date		-	7/11/2004
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Copies all of this objects properties to the new one
//o------------------------------------------------------------------------------------------------o
auto CSpawnItem::CopyData( CSpawnItem *target ) -> void
{
	CItem::CopyData( target );

	target->SetSpawnSection( GetSpawnSection() );
	target->IsSectionAList( IsSectionAList() );
	target->SetInterval( 0, GetInterval( 0 ));
	target->SetInterval( 1, GetInterval( 1 ));
}
