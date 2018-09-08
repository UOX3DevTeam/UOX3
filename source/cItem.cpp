//o--------------------------------------------------------------------------o
//|	File			-	cItem.cpp
//|	Date			-	2nd April, 2000
//|	Developers		-	Abaddon/EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	New class written based upon old UOX item_st.  Number of old members removed
//|						and a number of members types modified as well
//|
//|	Version History -
//|									
//|						1.0		Abaddon		2nd April, 2000
//|						Initial implementation
//|						Bools reduced down to a single short with get/set mechanisms for setting/clearings bits
//|						No documentation currently done, all functions obvious so far
//|						Note:  individual byte setting within longs/shorts need speeding up
//|						consider memcpy into correct word, note that this will be endian specific!
//|
//|						1.1		Abaddon		23rd July, 2000
//|						Addition of SaveItem routine for text mode only
//|
//|						1.2		Abaddon		25th July, 2000
//|						All Accessor funcs plus a few others are now flagged as const functions, meaning that
//|						those functions GUARENTEE never to alter data, at compile time
//|						Thoughts about CBaseObject and prelim plans made
//|
//|						1.3		Abaddon		28th July, 2000
//|						CBaseObject initial implementation plus rework of CItem to deal with only what it needs to
//|						Proper constructor written
//|						Plans for CItem derived classes thought upon
//|
//|						1.4		giwo		17th July, 2004
//|						Added CSpawnItem to begin breaking CItem into smaller subclasses
//o--------------------------------------------------------------------------o
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

namespace UOX
{

const UI32 BIT_DOOROPEN		=	1;
const UI32 BIT_PILEABLE		=	2;
const UI32 BIT_DYEABLE		=	3;
const UI32 BIT_CORPSE		=	4;
const UI32 BIT_UNUSED		=	5; //was wipeable
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

CItem::CItem() : CBaseObject(),
contObj( NULL ), glow_effect( DEFITEM_GLOWEFFECT ), glow( DEFITEM_GLOW ), glowColour( DEFITEM_GLOWCOLOUR ), 
madewith( DEFITEM_MADEWITH ), rndvaluerate( DEFITEM_RANDVALUE ), good( DEFITEM_GOOD ), rank( DEFITEM_RANK ), armorClass( DEFITEM_ARMORCLASS ), 
restock( DEFITEM_RESTOCK ), movable( DEFITEM_MOVEABLE ), tempTimer( DEFITEM_TEMPTIMER ), decaytime( DEFITEM_DECAYTIME ), 
spd( DEFITEM_SPEED ), maxhp( DEFITEM_MAXHP ), amount( DEFITEM_AMOUNT ), 
layer( DEFITEM_LAYER ), type( DEFITEM_TYPE ), offspell( DEFITEM_OFFSPELL ), entryMadeFrom( DEFITEM_ENTRYMADEFROM ), 
creator( DEFITEM_CREATOR ), gridLoc( DEFITEM_GRIDLOC ), weightMax( DEFITEM_WEIGHTMAX ), baseWeight( DEFITEM_BASEWEIGHT )
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

CItem::~CItem()	// Destructor to clean things up when deleted
{
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 GetCont()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the container's serial
//o--------------------------------------------------------------------------
CBaseObject * CItem::GetCont( void ) const
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
//|	Function		-	SI08 GetGridLocation()
//|	Date			-	7-23-2007
//|	Programmer		-	giwo
//o--------------------------------------------------------------------------
//|	Purpose			-	Support for "Grid Location" in UOKR
//o--------------------------------------------------------------------------
SI08 CItem::GetGridLocation( void ) const
{
	return gridLoc;
}

void CItem::SetGridLocation( SI08 newLoc )
{
	gridLoc = newLoc;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI32 TempVar( CITempVars whichVar  )
//|	Date			-	7/6/2004
//|	Programmer		-	giwo
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the temp value of the object
//o--------------------------------------------------------------------------
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

//o--------------------------------------------------------------------------
//|	Function		-	UI08 GetTempVarPart( CITempVars whichVar, UI08 part )
//|	Date			-	7/6/2004
//|	Programmer		-	giwo
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	One of the words of the temp value
//|							Valid values for part are 1->4.  If outside that, behaves
//|							as if it were 1
//o--------------------------------------------------------------------------
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

//o--------------------------------------------------------------------------
//|	Function		-	bool CItem::SetCont( CBaseObject *newCont )
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
			if( itemHolder != NULL )
			{
				contIsGround = false;
				// ok heres what hair/beards should be handled like (sereg)
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
		contObj = NULL;
		MapRegion->AddItem( this );
	}

	if( GetGlow() != INVALIDSERIAL )
		Items->GlowItem( this );

	return !contIsGround;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool isDoorOpen()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose		-	Item is a door and is open
//o--------------------------------------------------------------------------
bool CItem::isDoorOpen( void ) const
{
	return bools.test( BIT_DOOROPEN );
}
void CItem::SetDoorOpen( bool newValue )
{
	bools.set( BIT_DOOROPEN, newValue );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool isPileable()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose		-	Item is pileable
//o--------------------------------------------------------------------------
bool CItem::isPileable( void ) const
{
	return bools.test( BIT_PILEABLE );
}
void CItem::SetPileable( bool newValue )
{
	bools.set( BIT_PILEABLE, newValue );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool isDyeable()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose		-	Item is dyeable
//o--------------------------------------------------------------------------
bool CItem::isDyeable( void ) const
{
	return bools.test( BIT_DYEABLE );
}
void CItem::SetDye( bool newValue )
{
	bools.set( BIT_DYEABLE, newValue );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool isCorpse()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose		-	Item is a corpse
//o--------------------------------------------------------------------------
bool CItem::isCorpse( void ) const
{
	return bools.test( BIT_CORPSE );
}
void CItem::SetCorpse( bool newValue )
{
	bools.set( BIT_CORPSE, newValue );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool Guarded()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Item is guarded
//o---------------------------------------------------------------------------o
bool CItem::isGuarded( void ) const
{
	return bools.test( BIT_GUARDED );
}
void CItem::SetGuarded( bool newValue )
{
	bools.set( BIT_GUARDED, newValue );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool SpawnerList()
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the item's spawn points to an NPC list
//|					 instead of an NPC
//o---------------------------------------------------------------------------o
bool CItem::isSpawnerList( void ) const
{
	return bools.test( BIT_SPAWNERLIST );
}
void CItem::SetSpawnerList( bool newValue )
{
	bools.set( BIT_SPAWNERLIST, newValue );
}

const char *CItem::GetName2( void ) const
{
	return name2;
}
void CItem::SetName2( const char *newValue )
{
	strncpy( name2, newValue, MAX_NAME );
}

SERIAL CItem::GetCreator( void ) const
{
	return creator;
}
void CItem::SetCreator( SERIAL newValue )
{
	creator = newValue;
}

std::string CItem::GetDesc( void ) const
{
	return desc;
}
void CItem::SetDesc( std::string newValue )
{
	desc = newValue.substr( 0, MAX_NAME - 1 );
}

void CItem::IncZ( SI16 newValue )
{
	SetZ( z + newValue );
}
void CItem::IncLocation( SI16 xInc, SI16 yInc )
{
	SetLocation( (GetX() + xInc), (GetY() + yInc), GetZ(), GetGridLocation(), WorldNumber() );
}

void CItem::SetLocation( const CBaseObject *toSet )
{
	if( toSet == NULL )
		return;
	SetLocation( toSet->GetX(), toSet->GetY(), toSet->GetZ(), toSet->WorldNumber() );
}
void CItem::SetLocation( SI16 newX, SI16 newY, SI08 newZ )
{
	SetLocation( newX, newY, newZ, GetGridLocation(), WorldNumber() );
}

void CItem::SetLocation( SI16 newX, SI16 newY, SI08 newZ, SI08 newLoc, UI08 world )
{
	if( GetCont() == NULL )
		MapRegion->ChangeRegion( this, newX, newY, world );
	oldLocX = x;
	oldLocY = y;
	oldLocZ = z;
	x = newX;
	y = newY;
	z = newZ;
	gridLoc = newLoc;
	worldNumber = world;
	if( GetCont() == NULL )
	{
		if( !CanBeObjType( OT_MULTI ) )
		{
			CMultiObj *mMulti = findMulti( newX, newY, newZ, world );
			if( GetMultiObj() != mMulti )
				SetMulti( mMulti );
		}
	}
	Dirty( UT_LOCATION );
}
void CItem::SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world )
{
	SetLocation( newX, newY, newZ, GetGridLocation(), world );
}

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

ItemTypes CItem::GetType( void ) const
{
	return type;
}
void CItem::SetType( ItemTypes newValue )
{
	type = newValue;
}

SI08 CItem::GetOffSpell( void ) const
{
	return offspell;
}
void CItem::SetOffSpell( SI08 newValue )
{
	offspell = newValue;
}

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

UI16 CItem::GetMaxHP( void ) const
{
	return maxhp;
}
void CItem::SetMaxHP( UI16 newValue )
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

TIMERVAL CItem::GetTempTimer( void ) const
{
	return tempTimer;
}
TIMERVAL CItem::GetDecayTime( void ) const
{
	return decaytime;
}

void CItem::SetTempTimer( TIMERVAL newValue )
{
	tempTimer = newValue;
}
void CItem::SetDecayTime( TIMERVAL newValue )
{
	decaytime = newValue;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI08 Priv()
//|	Date			-	Unknown
//|	Programmer	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Purpose		-	Misc item settings
//o--------------------------------------------------------------------------o
UI08 CItem::GetPriv( void ) const
{
	return static_cast< UI08 >(priv.to_ulong());
}
void CItem::SetPriv( UI08 newValue )
{
	priv = newValue;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI32 SellValue()
//|	Date			-	Unknown
//|	Programmer	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Purpose		-	Items sell value
//o--------------------------------------------------------------------------o
UI32 CItem::GetSellValue( void ) const
{
	return value[1];
}
void CItem::SetSellValue( UI32 newValue )
{
	value[1] = newValue;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI32 BuyValue()
//|	Date			-	Unknown
//|	Programmer	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Purpose		-	Items buy value
//o--------------------------------------------------------------------------o
UI32 CItem::GetBuyValue( void ) const
{
	return value[0];
}
void CItem::SetBuyValue( UI32 newValue )
{
	value[0] = newValue;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI16 Restock()
//|	Date			-	Unknown
//|	Programmer	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Purpose		-	Items restock value
//o--------------------------------------------------------------------------o
UI16 CItem::GetRestock( void ) const
{
	return restock;
}
void CItem::SetRestock( UI16 newValue )
{
	restock = newValue;
}

//o--------------------------------------------------------------------------o
//|	Function		-	ARMORCLASS ArmourClass()
//|	Date			-	Unknown
//|	Programmer	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Purpose		-	Items armour class
//o--------------------------------------------------------------------------o
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

SI16 CItem::GetGood( void ) const
{
	return good;
}
void CItem::SetGood( SI16 newValue )
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

SERIAL CItem::GetGlow( void ) const
{
	return glow;
}
void CItem::SetGlow( SERIAL newValue )
{
	glow = newValue;
}

COLOUR CItem::GetGlowColour( void ) const
{
	return glowColour;
}

void CItem::SetGlowColour( COLOUR newValue )
{
	glowColour = newValue;
}

UI08 CItem::GetGlowEffect( void ) const
{
	return glow_effect;
}
void CItem::SetGlowEffect( UI08 newValue )
{
	glow_effect = newValue;
}

UI16 CItem::GetAmmoID( void ) const
{
	return ammo[0];
}

void CItem::SetAmmoID( UI16 newValue )
{
	ammo[0] = newValue;
}

UI16 CItem::GetAmmoHue( void ) const
{
	return ammo[1];
}

void CItem::SetAmmoHue( UI16 newValue )
{
	ammo[1] = newValue;
}

UI16 CItem::GetAmmoFX( void ) const
{
	return ammoFX[0];
}

void CItem::SetAmmoFX( UI16 newValue )
{
	ammoFX[0] = newValue;
}

UI16 CItem::GetAmmoFXHue( void ) const
{
	return ammoFX[1];
}

void CItem::SetAmmoFXHue( UI16 newValue )
{
	ammoFX[1] = newValue;
}

UI16 CItem::GetAmmoFXRender( void ) const
{
	return ammoFX[2];
}

void CItem::SetAmmoFXRender( UI16 newValue )
{
	ammoFX[2] = newValue;
}

SI32 CItem::GetWeightMax( void ) const
{
	return weightMax;
}

void CItem::SetWeightMax( SI32 newValue )
{
	weightMax = newValue;
}

SI32 CItem::GetBaseWeight( void ) const
{
	return baseWeight;
}

void CItem::SetBaseWeight( SI32 newValue )
{
	baseWeight = newValue;
}

void CItem::IncID( SI16 incAmount )
{
	SetID( id + incAmount );
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
	return ( movable == 3 );
}
bool CItem::IsShieldType( void ) const
{
	if( id >= 0x1B72 && id <= 0x1B7B )
		return true;
	if( id >= 0x1BC3 && id <= 0x1BC7 )
		return true;
	return false;
}

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

void CItem::LockDown( void )
{
	movable = 3;
}

bool CItem::Save( std::ofstream &outStream )
{
	if( isFree() )
		return false;
	MapData_st& mMap = Map->GetMapData( worldNumber );
	if( GetCont() != NULL || ( GetX() > 0 && GetX() < mMap.xBlock && GetY() < mMap.yBlock ) )
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

void CItem::RemoveSelfFromOwner( void )
{
	CChar *oldOwner = GetOwnerObj();
	if( oldOwner != NULL )
		oldOwner->RemoveOwnedItem( this );
}
void CItem::AddSelfToOwner( void )
{
	CChar *newOwner = GetOwnerObj();
	if( !ValidateObject( newOwner ) )
		return;
	if( newOwner->GetSerial() != GetSerial() )
		newOwner->AddOwnedItem( this );
}

void CItem::RemoveSelfFromCont( void )
{
	if( contObj != NULL )
	{
		if( contObj->GetObjType() == OT_CHAR )	// it's a char!
		{
			CChar *targChar = static_cast<CChar *>(contObj);
			if( ValidateObject( targChar ) )
			{
				Weight->subtractItemWeight( targChar, this );
				targChar->TakeOffItem( GetLayer() );
			}
		}
		else
		{
			CItem *targItem = static_cast<CItem *>(contObj);
			if( ValidateObject( targItem ) )
			{
				Weight->subtractItemWeight( targItem, this );
				targItem->GetContainsList()->Remove( this );
			}
		}
	}
	else
		MapRegion->RemoveItem( this );
}

CItem * CItem::Dupe( ObjectType itemType )
{
	CItem *target = static_cast< CItem * >(ObjectFactory::getSingleton().CreateObject( itemType ));
	if( target == NULL )
		return NULL;

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
	target->SetScriptTrigger( GetScriptTrigger() );
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
	//target->SetWipeable( isWipeable() );
	target->SetPriv( GetPriv() );

	for( int i = 0; i < WEATHNUM; ++i )
		target->SetWeatherDamage( (WeatherType)i, GetWeatherDamage( (WeatherType)i ) );
}

bool CItem::GetWeatherDamage( WeatherType effectNum ) const
{
	return weatherBools.test( effectNum );
}

void CItem::SetWeatherDamage( WeatherType effectNum, bool value )
{
	weatherBools.set( effectNum, value );
}

bool CItem::DumpHeader( std::ofstream &outStream ) const
{
	outStream << "[ITEM]" << '\n';
	return true;
}


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

bool CItem::HandleLine( UString &UTag, UString &data )
{
	bool rvalue = CBaseObject::HandleLine( UTag, data );
	if( !rvalue )
	{
		switch( (UTag.data()[0]) )
		{
			case 'A':
				if( UTag == "AMMO" )
				{
					if( data.sectionCount( "," ) != 0 )
					{
						SetAmmoID( data.section( ",", 0, 0 ).stripWhiteSpace().toUShort() );
						SetAmmoHue( data.section( ",", 1, 1 ).stripWhiteSpace().toUShort() );
					}
					else
					{
						SetAmmoID( data.toULong() );
						SetAmmoHue( ( 0 ) );
					}
					rvalue = true;
				}
				else if( UTag == "AMMOFX" )
				{
					if( data.sectionCount( "," ) != 0 )
					{
						SetAmmoFX( data.section( ",", 0, 0 ).stripWhiteSpace().toUShort() );
						SetAmmoFXHue( data.section( ",", 1, 1 ).stripWhiteSpace().toUShort() );
						SetAmmoFXRender( data.section( ",", 1, 1 ).stripWhiteSpace().toUShort() );
					}
					else
					{
						SetAmmoFX( data.toULong() );
						SetAmmoFXHue( ( 0 ) );
						SetAmmoFXRender( ( 0 ) );
					}
					rvalue = true;
				}
				else if( UTag == "AMOUNT" )
				{
					amount = data.toUShort();
					rvalue = true;
				}
				else if( UTag == "AC" )
				{
					SetArmourClass( data.toUByte() );
					rvalue = true;
				}
				break;
			case 'B':
				if( UTag == "BOOLS" )
				{
					bools = data.toUByte();
					rvalue = true;
				}
				break;
			case 'C':
				if( UTag == "CONT" )
				{
					contObj = (CBaseObject *)data.toULong();
					rvalue = true;
				}
				else if( UTag == "CREATOR" || UTag == "CREATER" )
				{
					SetCreator( data.toULong() );
					rvalue = true;
				}
				else if( UTag == "CORPSE" )
				{
					SetCorpse( data.toUByte() == 1 );
					rvalue = true;
				}
				else if( UTag == "COLD" )
				{
					SetWeatherDamage( COLD, data.toUByte() == 1 );
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
					SetDir( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "DYEABLE" )
				{
					SetDye( data.toUByte() == 1 );
					rvalue = true;
				}
				break;
			case 'E':
				if( UTag == "ENTRYMADEFROM" )
				{
					EntryMadeFrom( data.toUShort() );
					rvalue = true;
				}
				break;
			case 'G':
				if( UTag == "GRIDLOC" )
				{
					SetGridLocation( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "GLOWTYPE" )
				{
					SetGlowEffect( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "GLOWBC" )
				{
					SetGlowColour( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "GLOW" )
				{
					SetGlow( data.toULong() );
					rvalue = true;
				}
				else if( UTag == "GOOD" )
				{
					SetGood( data.toShort() );
					rvalue = true;
				}
				break;
			case 'H':
				if( UTag == "HEAT" )
				{
					SetWeatherDamage( HEAT, data.toUByte() == 1 );
					rvalue = true;
				}
				break;
			case 'L':
				if( UTag == "LAYER" )
				{
					layer = static_cast<ItemLayers>(data.toUByte());
					rvalue = true;
				}
				else if( UTag == "LIGHT" )
				{
					SetWeatherDamage( LIGHT, data.toUShort() == 1 );
					rvalue = true;
				}
				else if( UTag == "LIGHTNING" )
				{
					SetWeatherDamage( LIGHTNING, data.toUShort() == 1 );
					rvalue = true;
				}
				break;
			case 'M':
				if( UTag == "MORE" )
				{
					if( data.sectionCount( "," ) != 0 )
						SetTempVar( CITV_MORE, data.section( ",", 0, 0 ).stripWhiteSpace().toULong() );
					else
						SetTempVar( CITV_MORE, data.toULong() );
					rvalue = true;
				}
				else if( UTag == "MORE2" )	// Depreciated
					rvalue = true;
				else if( UTag == "MURDERER" )
					rvalue = true;
				else if( UTag == "MOREXYZ" )
				{
					SetTempVar( CITV_MOREX, data.section( ",", 0, 0 ).stripWhiteSpace().toULong() );
					SetTempVar( CITV_MOREY, data.section( ",", 1, 1 ).stripWhiteSpace().toULong() );
					SetTempVar( CITV_MOREZ, data.section( ",", 2, 2 ).stripWhiteSpace().toULong() );
					rvalue = true;
				}
				else if( UTag == "MOREX" )
				{
					SetTempVar( CITV_MOREX, data.toULong() );
					rvalue = true;
				}
				else if( UTag == "MOREY" )
				{
					SetTempVar( CITV_MOREY, data.toULong() );
					rvalue = true;
				}
				else if( UTag == "MOREZ" )
				{
					SetTempVar( CITV_MOREZ, data.toULong() );
					rvalue = true;
				}
				else if( UTag == "MOVABLE" )
				{
					SetMovable( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "MAXHP" )
				{
					SetMaxHP( data.toUShort() );
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
					SetOffSpell( data.toByte() );
					rvalue = true;
				}
				break;
			case 'P':
				if( UTag == "PRIV" )
				{
					SetPriv( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "PILEABLE" )
				{
					SetPileable( data.toUByte() == 1 );
					rvalue = true;
				}
				break;
			case 'R':
				if( UTag == "RESTOCK" )
				{
					SetRestock( data.toUShort() );
					rvalue = true;
				}
				else if( UTag == "RACEDAMAGE" )
				{
					SetWeatherDamage( LIGHT, data.section( ",", 0, 0 ).stripWhiteSpace().toUByte() == 1 );
					SetWeatherDamage( RAIN, data.section( ",", 1, 1 ).stripWhiteSpace().toUByte() == 1 );
					SetWeatherDamage( HEAT, data.section( ",", 2, 2 ).stripWhiteSpace().toUByte() == 1 );
					SetWeatherDamage( COLD, data.section( ",", 3, 3 ).stripWhiteSpace().toUByte() == 1 );
					SetWeatherDamage( SNOW, data.section( ",", 4, 4 ).stripWhiteSpace().toUByte() == 1 );
					SetWeatherDamage( LIGHTNING, data.section( ",", 5, 5 ).stripWhiteSpace().toUByte() == 1 );
					rvalue = true;	
				}
				else if( UTag == "RANK" )
				{
					SetRank( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "RAIN" )
				{
					SetWeatherDamage( RAIN, data.toUByte() == 1 );
					rvalue = true;
				}
				else if( UTag == "REPUTATION" )
					rvalue = true;
				break;
			case 'S':
				if( UTag == "SPEED" )
				{
					SetSpeed( data.toUByte() );
					rvalue = true;
				}
				else if( UTag == "SK_MADE" )
				{
					SetMadeWith( data.toByte() );
					rvalue = true;
				}
				else if( UTag == "SNOW" )
				{
					SetWeatherDamage( SNOW, data.toUByte() == 1 );
					rvalue = true;
				}
				else if( UTag == "SPELLS" )
				{
					SetSpell( 0, data.section( ",", 0, 0 ).stripWhiteSpace().toULong() );
					SetSpell( 1, data.section( ",", 1, 1 ).stripWhiteSpace().toULong() );
					SetSpell( 2, data.section( ",", 2, 2 ).stripWhiteSpace().toULong() );
					rvalue = true;
				}
				break;
			case 'T':
				if( UTag == "TYPE" )
				{
					if( data.sectionCount( "," ) != 0 )
						SetType( static_cast<ItemTypes>(data.section( ",", 0, 0 ).stripWhiteSpace().toUByte()) );
					else
						SetType( static_cast<ItemTypes>(data.toUByte()) );
					rvalue = true;
				}
				else if( UTag == "TYPE2" )
					rvalue = true;
				break;
			case 'V':
				if( UTag == "VALUE" )
				{
					if( data.sectionCount( "," ) != 0 )
					{
						SetBuyValue( data.section( ",", 0, 0 ).stripWhiteSpace().toULong() );
						SetSellValue( data.section( ",", 1, 1 ).stripWhiteSpace().toULong() );
					}
					else
					{
						SetBuyValue( data.toULong() );
						SetSellValue( (data.toULong() / 2) );
					}
					rvalue = true;
				}
				break;
			case 'W':
				if( UTag == "WEIGHTMAX" )
				{
					SetWeightMax( data.toLong() );
					rvalue = true;
				}
				break;
		}
	}
	return rvalue;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool LoadRemnants( UI32 arrayOffset )
//|	Date			-	21st January, 2002
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	After handling data specific load, other parts go here
//o--------------------------------------------------------------------------
bool CItem::LoadRemnants( void )
{
	SetSerial( serial );

	// Tauriel adding region pointers
	if( (SERIAL)contObj == INVALIDSERIAL )
	{
		MapData_st& mMap = Map->GetMapData( worldNumber );
		if( GetX() < 0 || GetY() < 0 || GetX() > mMap.xBlock || GetY() > mMap.yBlock )
			return false;
	}
	return true;
}

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

void CItem::PostLoadProcessing( void )
{
	CBaseObject::PostLoadProcessing();
	// Add item weight if item doesn't have it yet
	if( GetWeight() < 0 || GetWeight() > MAX_WEIGHT )
		SetWeight( Weight->calcWeight( this ) );

	SERIAL tempSerial	= (SERIAL)contObj;
	CBaseObject *tmpObj	= NULL;
	contObj				= NULL;
	if( tempSerial != INVALIDSERIAL )
	{
		if( tempSerial >= BASEITEMSERIAL )
			tmpObj = calcItemObjFromSer( tempSerial );
		else
			tmpObj	= calcCharObjFromSer( tempSerial );
	}
	SetCont( tmpObj );

	Items->StoreItemRandomValue( this, NULL );
	CheckItemIntegrity();
	SetPostLoaded( true );
}

void CItem::CheckItemIntegrity( void )
{
	SERIAL getSerial = GetSerial();
	if( getSerial == INVALIDSERIAL )
	{
		Console.Warning( "Item (%s) has an invalid serial number, Deleting", GetName().c_str() );
		Delete();
		return;
	}

	if( getSerial == GetContSerial() )
	{
		Console.Warning( "Item 0x%X (%s) has dangerous container value, Auto-Correcting", getSerial, GetName().c_str() );
		SetCont( NULL );
	}
	if( getSerial == GetOwner() )
	{
		Console.Warning( "Item 0x%X (%s) has dangerous owner value, Auto-Correcting", getSerial, GetName().c_str() );
		SetOwner( NULL );
	}
	if( getSerial == GetSpawn() )
	{
		Console.Warning( "Item 0x%X (%s) has dangerous spawner value, Auto-Correcting", getSerial, GetName().c_str() );
		SetSpawn( INVALIDSERIAL );
	}
}

const UI32 BIT_DECAYABLE	=	0;
const UI32 BIT_NEWBIE		=	1;
const UI32 BIT_DISPELLABLE	=	2;
const UI32 BIT_DEVINELOCK	=	3;

bool CItem::isDecayable( void ) const
{
	return priv.test( BIT_DECAYABLE );
}
bool CItem::isNewbie( void ) const
{
	return priv.test( BIT_NEWBIE );
}
bool CItem::isDispellable( void ) const
{
	return priv.test( BIT_DISPELLABLE );
}
bool CItem::isDevineLocked( void ) const
{
	return priv.test( BIT_DEVINELOCK );
}

void CItem::SetDecayable( bool newValue )
{
	if( newValue )
		SetDecayTime( 0 );

	priv.set( BIT_DECAYABLE, newValue );
}
void CItem::SetNewbie( bool newValue )
{
	priv.set( BIT_NEWBIE, newValue );
}
void CItem::SetDispellable( bool newValue )
{
	priv.set( BIT_DISPELLABLE, newValue );
}
void CItem::SetDevineLock( bool newValue )
{
	priv.set( BIT_DEVINELOCK, newValue );
}

//o--------------------------------------------------------------------------
//|	Function		-	UI16 EntryMadeFrom()
//|	Date			-	13 September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Create.scp entry the item was made from
//o--------------------------------------------------------------------------
UI16 CItem::EntryMadeFrom( void ) const
{
	return entryMadeFrom;
}

void CItem::EntryMadeFrom( UI16 newValue )
{
	entryMadeFrom = newValue;
}

void CItem::SetWeight( SI32 newVal, bool doWeightUpdate )
{
	CBaseObject *checkCont = NULL;
	if( isPostLoaded() && doWeightUpdate )
		checkCont = GetCont();
	
	if( ValidateObject( checkCont ) )
		Weight->subtractItemWeight( checkCont, this );

	weight = newVal;

	if( ValidateObject( checkCont ) )
		Weight->addItemWeight( checkCont, this );
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

//o---------------------------------------------------------------------------o
//|		Function    :	void TextMessage( CSocket *s, SI32 dictEntry, R32 secsFromNow, UI16 Colour )
//|		Date        :	Unknown
//|		Programmer  :	UOX DevTeam
//o---------------------------------------------------------------------------o
//|		Purpose     :	Item "speech"
//o---------------------------------------------------------------------------o
void CItem::TextMessage( CSocket *s, SI32 dictEntry, R32 secsFromNow, UI16 Colour )
{
	UnicodeTypes dictLang	= ZERO;
	SERIAL speakTo			= INVALIDSERIAL;
	SpeechTarget target		= SPTRG_PCNPC;
	if( s != NULL )
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


//o---------------------------------------------------------------------------o
//|	Function	-	void Update( CSocket *mSock )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send this item to specified socket or all online people in range
//o---------------------------------------------------------------------------o
void CItem::Update( CSocket *mSock )
{
	if( GetType() == IT_TRADEWINDOW )
		return;

	RemoveFromSight( mSock );
	if( GetCont() == this )
	{
		Console.Warning( "Item %s(0x%X) has a dangerous container value, auto-correcting", GetName().c_str(), GetSerial() );
		SetCont( NULL );
	}

	CBaseObject *iCont = GetCont();
	if( iCont == NULL )
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
		if( charCont != NULL )
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
		if( itemCont != NULL )
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
	Console.Error( " CItem::Update(0x%X): cannot determine container type!", GetSerial() );
}

//o---------------------------------------------------------------------------o
//|		Function    -	void SendToSocket( CSocket *mSock )
//|		Programmer  -	giwo
//|		Date        -	July 27, 2003
//o---------------------------------------------------------------------------o
//|		Purpose     -	Updates an item on the ground to specified socket
//o---------------------------------------------------------------------------o
void CItem::SendToSocket( CSocket *mSock )
{
	if( !mSock->LoginComplete() )
		return;

	CChar *mChar = mSock->CurrcharObj();
	if( mChar != NULL )
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

//o---------------------------------------------------------------------------o
//|		Function    -	void SendPackItemToSocket( CSocket *mSock )
//|		Programmer  -	giwo
//|		Date        -	July 27, 2003
//o---------------------------------------------------------------------------o
//|		Purpose     -	Updates an item contained in a pack to specified socket
//o---------------------------------------------------------------------------o
void CItem::SendPackItemToSocket( CSocket *mSock )
{
	CChar *mChar = mSock->CurrcharObj();
	if( mChar != NULL )
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

//o---------------------------------------------------------------------------o
//|		Function    -	void CItem::RemoveFromSight
//|		Date        -	September 7th, 2003
//|		Programmer  -	giwo
//o---------------------------------------------------------------------------o
//|		Purpose     -	Loops through all online chars and removes the item from their sight
//o---------------------------------------------------------------------------o
void CItem::RemoveFromSight( CSocket *mSock )
{
	CPRemoveItem toRemove	= (*this);
	CBaseObject *iCont		= GetCont();

	ObjectType oType	= OT_CBO;
	CBaseObject *iOwner	= FindItemOwner( this, oType );

	if( iCont == NULL || oType == OT_ITEM )
	{
		CItem *rItem = NULL;
		if( iCont == NULL )
			rItem = this;
		else
			rItem = static_cast<CItem *>(iOwner);
		if( rItem != NULL )
		{
			if( mSock != NULL )
				mSock->Send( &toRemove );
			else
			{
				CChar *tChar			= NULL;
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
						(*cIter)->Send( &toRemove );
				}
			}
		}
	}
	else if( iCont->GetObjType() == OT_CHAR || oType == OT_CHAR )
	{
		CChar *rChar = NULL;
		if( iCont->GetObjType() == OT_CHAR )
			rChar = static_cast<CChar *>(iCont);
		else
			rChar = static_cast<CChar *>(iOwner);
		if( rChar != NULL )
		{
			if( mSock != NULL )
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
		if( mSock != NULL )
			mSock->Send( &toRemove );
		else
		{
			Network->PushConn();
			for( CSocket *nSock = Network->FirstSocket(); !Network->FinishedSockets(); nSock = Network->NextSocket() )
			{
				if( !nSock->LoginComplete() )
					continue;
				nSock->Send( &toRemove );
			}
			Network->PopConn();
		}
	}
}

//o---------------------------------------------------------------------------o
//|		Function    -	void CItem::PlaceInPack( void )
//|		Date        -	October 18th, 2003
//|		Programmer  -	giwo
//o---------------------------------------------------------------------------o
//|		Purpose     -	Puts an item at a random location inside a pack
//o---------------------------------------------------------------------------o
void CItem::PlaceInPack( void )
{
	SetX( static_cast<SI16>(50 + RandomNum( 0, 79 )) );
	SetY( static_cast<SI16>(50 + RandomNum( 0, 79 )) );
	SetZ( 9 );
}

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

//o---------------------------------------------------------------------------o
//|   Function    -  void CItem::Cleanup( void )
//|   Date        -  11/6/2003
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Cleans up after item
//o---------------------------------------------------------------------------o
void CItem::Cleanup( void )
{
	if( !isFree() )	// We're not the default item in the handler
	{
		JSEngine->ReleaseObject( IUE_ITEM, this );

		CBaseObject::Cleanup();

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
					if( spawnReg != NULL )
						spawnReg->deleteSpawnedItem( this );
				}
			}
			SetSpawn( INVALIDSERIAL );
		}
		if( GetSpawnObj() != NULL ) 
			SetSpawn( INVALIDSERIAL );

		if( GetGlow() != INVALIDSERIAL )
		{
			CItem *j = calcItemObjFromSer( GetGlow() );
			if( ValidateObject( j ) )
				j->Delete();  // LB glow stuff, deletes the glower of a glowing stuff automatically
		}

		if( isGuarded() )
		{
			CChar *owner = NULL;
			CMultiObj *multi = findMulti( this );
			if( ValidateObject( multi ) )
				owner = multi->GetOwnerObj();
			if( !ValidateObject( owner ) )
				owner = FindItemOwner( this );
			if( ValidateObject( owner ) )
			{
				CChar *petGuard = Npcs->getGuardingPet( owner, this );
				if( ValidateObject( petGuard ) )
					petGuard->SetGuarding( NULL );
				SetGuarded( false );
			}
		}

		if( GetType() == IT_MESSAGEBOARD )
			MsgBoardRemoveFile( GetSerial() );

		if( GetType() == IT_READABLEBOOK && ( GetTempVar( CITV_MOREX ) == 666 || GetTempVar( CITV_MOREX ) == 999 ) )
			Books->DeleteBook( this );
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	bool CanBeObjType()
//|	Date			-	24 June, 2004
//|	Programmer		-	Maarc
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Indicates whether an object can behave as a
//|						particular type
//o--------------------------------------------------------------------------
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

//o---------------------------------------------------------------------------o
//|   Function    -  bool Delete()
//|   Date        -  11/6/2003
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Adds character to deletion queue
//o---------------------------------------------------------------------------o
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

CDataList< CItem * > * CItem::GetContainsList( void )
{
	return &Contains;
}

//o--------------------------------------------------------------------------o
//|	Class			-	CSpawnItem
//|	Date			-	29th June, 2004
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Version History
//|									
//|						1.0		giwo		29th June, 2004
//|						Original implementation
//|						First attempt to begin breaking CItem into multiple smaller
//|						classes, reducing overall memory usage. Currently stores all
//|						objects spawned by an item.
//o--------------------------------------------------------------------------o
CSpawnItem::CSpawnItem() : CItem(),
isSectionAList( false )
{
	objType = OT_SPAWNER;
	Interval[0] = Interval[1] = 0;
	spawnSection.reserve( 100 );
	spawnSection = "";
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 Interval()
//|   Date        -  6/29/2004
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Min and Max interval for spawner to respawn
//o---------------------------------------------------------------------------o
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

//o---------------------------------------------------------------------------o
//|   Function    -  std::string SpawnSection()
//|   Date        -  6/29/2004
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Script section to spawn from
//o---------------------------------------------------------------------------o
std::string CSpawnItem::GetSpawnSection( void ) const
{
	return spawnSection;
}
void CSpawnItem::SetSpawnSection( std::string newVal )
{
	spawnSection = newVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  std::string IsSectionAList()
//|   Date        -  7/05/2004
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Is the script section a spawner list?
//o---------------------------------------------------------------------------o
bool CSpawnItem::IsSectionAList( void ) const
{
	return isSectionAList;
}
void CSpawnItem::IsSectionAList( bool newVal )
{
	isSectionAList = newVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool DumpHeader( std::ofstream &outStream )
//|   Date        -  6/29/2004
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Dumps Header to Worldfile
//o---------------------------------------------------------------------------o
bool CSpawnItem::DumpHeader( std::ofstream &outStream ) const
{
	outStream << "[SPAWNITEM]" << '\n';
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool DumpBody( std::ofstream &outStream )
//|   Date        -  6/29/2004
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Dumps Spawn Item to Worldfile
//o---------------------------------------------------------------------------o
bool CSpawnItem::DumpBody( std::ofstream &outStream ) const
{
	CItem::DumpBody( outStream );
	outStream << "Interval=" << (UI16)GetInterval( 0 ) << "," << (UI16)GetInterval( 1 ) << '\n';
	outStream << "SpawnSection=" << GetSpawnSection() << '\n';
	outStream << "IsSectionAList=" << (UI16)(IsSectionAList()?1:0) << '\n';
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool HandleLine( UString &UTag, UString &data )
//|   Date        -  6/29/2004
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Reads data from Worldfile into the class
//o---------------------------------------------------------------------------o
bool CSpawnItem::HandleLine( UString &UTag, UString &data )
{
	bool rvalue = CItem::HandleLine( UTag, data );
	if( !rvalue )
	{
		switch( (UTag.data()[0]) )
		{
			case 'I':
				if( UTag == "INTERVAL" )
				{
					SetInterval( 0, data.section( ",", 0, 0 ).stripWhiteSpace().toUByte() );
					SetInterval( 1, data.section( ",", 1, 1 ).stripWhiteSpace().toUByte() );
					rvalue = true;
				}
				else if( UTag == "ISSECTIONALIST" )
				{
					IsSectionAList( (data.toUByte() == 1) );
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

//o---------------------------------------------------------------------------o
//|   Function    -  DoRespawn()
//|   Date        -  6/29/2004
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Will eventually take the place of RespawnItem()
//o---------------------------------------------------------------------------o
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
			Items->AddRespawnItem( this, UString::number( GetTempVar( CITV_MOREX ) ), false, 1 );
		else
		{
			Console.Warning( "Bad Item Spawner Found, Deleting" );
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
			Npcs->CreateNPC( this, UString::number( GetTempVar( CITV_MOREX ) ) );
		else
		{
			Console.Warning( "Bad Npc/Area Spawner Found, Deleting" );
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
			SetType( IT_LOCKEDSPAWNCONT ); // Lock the container
		if( !listObj.empty() )
			Items->AddRespawnItem( this, listObj, true, IsSectionAList(), 1 );
		else if( GetTempVar( CITV_MOREX ) != 0 )
			Items->AddRespawnItem( this, UString::number( GetTempVar( CITV_MOREX ) ), true, 1 );
		else
		{
			Console.Warning( "Bad Spawn Container Found, Deleting" );
			Delete();
			return true;
		}
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|   Function    -  Cleanup()
//|   Date        -  6/29/2004
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Cleans up after spawner item
//o---------------------------------------------------------------------------o
void CSpawnItem::Cleanup( void )
{
	CItem::Cleanup();

	for( CBaseObject *mObj = spawnedList.First(); !spawnedList.Finished(); mObj = spawnedList.Next() )
	{
		if( mObj->GetSpawnObj() == this )
			mObj->SetSpawn( INVALIDSERIAL );
	}
}

//o--------------------------------------------------------------------------o
//|	Function	-	bool CanBeObjType()
//|	Date		-	24 June, 2004
//|	Programmer	-	Maarc
//|	Modified	-
//o--------------------------------------------------------------------------o
//|	Purpose		-	Indicates whether an object can behave as a
//|					particular type
//o--------------------------------------------------------------------------o
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

//o---------------------------------------------------------------------------o
//|   Function    -  Dupe()
//|   Date        -  7/11/2004
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Creates a new CSpawnItem and copies all of this objects properties to the new one
//o---------------------------------------------------------------------------o
CSpawnItem * CSpawnItem::Dupe( void )
{
	CSpawnItem *target = static_cast< CSpawnItem * >(ObjectFactory::getSingleton().CreateObject( OT_SPAWNER ));
	if( target == NULL )
		return NULL;

	CopyData( target );

	return target;
}

//o---------------------------------------------------------------------------o
//|   Function    -  CopyData( CSpawnItem *target )
//|   Date        -  7/11/2004
//|   Programmer  -  giwo
//o---------------------------------------------------------------------------o
//|   Purpose     -  Copies all of this objects properties to the new one
//o---------------------------------------------------------------------------o
void CSpawnItem::CopyData( CSpawnItem *target )
{
	CItem::CopyData( target );

	target->SetSpawnSection( GetSpawnSection() );
	target->IsSectionAList( IsSectionAList() );
	target->SetInterval( 0, GetInterval( 0 ) );
	target->SetInterval( 1, GetInterval( 1 ) );
}

}
