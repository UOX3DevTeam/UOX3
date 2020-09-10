//o-----------------------------------------------------------------------------------------------o
//|	File		-	cMultiObj.cpp
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Version History
//|
//|						1.0		28th July, 2000
//|						Very basically fleshed out
//|						Owner, ban list and object inside support included
//|						Note that lockdown stuff is NOT persistent yet, needs support later on
//|						Best place would be world loading, any locked down objects flagged at time added to multi
//|
//|						1.1		28th August, 2000
//|						Function declaration headers
//|						Addition of SetMaxLockDowns
//|
//|						1.2		30th November, 2000
//|						Changed lock downs to a vector, and full vector support added
//|						Save needs modification
//|
//| 					1.3		25th September 2003
//|						Added CBoatObj as a derived class of CMultiObj to simplify some processes in the cBoat class
//o-----------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "mapstuff.h"

const UI16	DEFMULTI_MAXLOCKEDDOWN	= 256;

const UI08 HOUSEPRIV_OWNER	= 0;
const UI08 HOUSEPRIV_BANNED = 1;

CMultiObj::CMultiObj() : CItem(), deed( "" ), maxLockedDown( DEFMULTI_MAXLOCKEDDOWN )
{
	objType = OT_MULTI;
	housePrivList.clear();
}

CMultiObj::~CMultiObj()
{
	housePrivList.clear();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddToBanList( CChar *toBan )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a character to the ban list, if he's not already on it
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::AddToBanList( CChar *toBan )
{
	if( !IsOnBanList( toBan ) && !IsOwner( toBan ) )
		housePrivList[toBan] = HOUSEPRIV_BANNED;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveFromBanList( CChar *toRemove )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a character from a ban list
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::RemoveFromBanList( CChar *toRemove )
{
	std::map< CChar *, UI08 >::iterator rIter = housePrivList.begin();
	if( rIter != housePrivList.end() )
	{
		if( rIter->second == HOUSEPRIV_BANNED )
			housePrivList.erase( rIter );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddAsOwner( CChar *newOwner )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a player as an owner, so long as they aren't already
//|						and aren't already on the ban list
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::AddAsOwner( CChar *newOwner )
{
	if( !IsOwner( newOwner ) && !IsOnBanList( newOwner ) )
		housePrivList[newOwner] = HOUSEPRIV_OWNER;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveAsOwner( CChar *toRemove )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a character from the owner's list
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::RemoveAsOwner( CChar *toRemove )
{
	std::map< CChar *, UI08 >::iterator rIter = housePrivList.find( toRemove );
	if( rIter != housePrivList.end() )
	{
		if( rIter->second == HOUSEPRIV_OWNER )
			housePrivList.erase( rIter );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsOnBanList( CChar *toBan ) const
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if a character is on the ban list
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::IsOnBanList( CChar *toBan ) const
{
	std::map< CChar *, UI08 >::const_iterator bIter = housePrivList.find( toBan );
	if( bIter != housePrivList.end() )
	{
		if( bIter->second == HOUSEPRIV_BANNED )
			return true;
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsOwner( CChar *toFind ) const
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the character is an owner
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::IsOwner( CChar *toFind ) const
{
	std::map< CChar *, UI08 >::const_iterator oIter = housePrivList.find( toFind );
	if( oIter != housePrivList.end() )
	{
		if( oIter->second == HOUSEPRIV_OWNER )
			return true;
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddToMulti( CBaseObject *toAdd )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds an item or character to the multi
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::AddToMulti( CBaseObject *toAdd )
{
	if( toAdd->GetObjType() == OT_CHAR )
		charInMulti.Add( static_cast< CChar * >(toAdd) );
	else
		itemInMulti.Add( static_cast< CItem * >(toAdd) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveFromMulti( CBaseObject *toRemove )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes an item or character from the multi, if possible
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::RemoveFromMulti( CBaseObject *toRemove )
{
	if( toRemove->GetObjType() == OT_CHAR )
	{
		charInMulti.Remove( static_cast< CChar * >(toRemove) );
		if( CanBeObjType( OT_BOAT ) && charInMulti.Num() == 0 )
			(static_cast<CBoatObj *>(this))->SetMoveType( 0 );
	}
	else
		itemInMulti.Remove( static_cast< CItem * >(toRemove) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t GetLockDownCount( void ) const
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a count of the locked down items
//o-----------------------------------------------------------------------------------------------o
size_t CMultiObj::GetLockDownCount( void ) const
{
	return lockedList.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetMaxLockDowns( void ) const
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the maximum number of possible lockdowns
//o-----------------------------------------------------------------------------------------------o
UI32 CMultiObj::GetMaxLockDowns( void ) const
{
	return maxLockedDown;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LockDownItem( CItem *toLock )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Locks down an item toLock
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::LockDownItem( CItem *toLock )
{
	if( lockedList.size() < maxLockedDown )
	{
		for( ITEMLIST_CITERATOR lIter = lockedList.begin(); lIter != lockedList.end(); ++lIter )
		{
			if( (*lIter) == toLock )
				return;
		}
		toLock->LockDown();
		toLock->Dirty( UT_UPDATE );
		lockedList.push_back( toLock );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void RemoveLockDown( CItem *toRemove )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Unlocks a locked down item
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::RemoveLockDown( CItem *toRemove )
{
	for( ITEMLIST_ITERATOR rIter = lockedList.begin(); rIter != lockedList.end(); ++rIter )
	{
		if( (*rIter) == toRemove )
		{
			toRemove->Dirty( UT_UPDATE );
			lockedList.erase( rIter );
			toRemove->SetMovable( 0 );
			return;
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool Save( std::ofstream &outStream )
//|	Date		-	28th July, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves a multi out to disk. outStream is the file to write to
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::Save( std::ofstream &outStream )
{
	bool rvalue = false;
	if( !isFree() )
	{
		rvalue = true;
		MapData_st& mMap = Map->GetMapData( worldNumber );
		if( GetCont() != NULL || ( GetX() > 0 && GetX() < mMap.xBlock && GetY() < mMap.yBlock ) )
		{
			DumpHeader( outStream );
			DumpBody( outStream );
			DumpFooter( outStream );
		}
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool LoadRemnants( void )
//|	Date		-	20th January, 2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	After handling data specific load, other parts go here
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::LoadRemnants( void )
{
	bool rValue = CItem::LoadRemnants();

	// Add item weight if item doesn't have it yet
	if( GetWeight() <= 0 )
		SetWeight( 0 );
	return rValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetMaxLockDowns( UI16 newValue )
//|	Date		-	28th August, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets the maximum number of lockdowns possible
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::SetMaxLockDowns( UI16 newValue )
{
	maxLockedDown = newValue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DumpHeader( std::ofstream &outStream ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Dumps out the header for the CMultiObj
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::DumpHeader( std::ofstream &outStream ) const
{
	outStream << "[HOUSE]" << '\n';
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DumpBody( std::ofstream &outStream ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes out all CMultiObj specific information to a world
//|					file.  Also prints out the Item and BaseObject portions
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::DumpBody( std::ofstream &outStream ) const
{
	CItem::DumpBody( outStream );

	// Hexadecimal Values
	outStream << std::hex;

	// Decimal / String Values
	outStream << std::dec;
	std::map< CChar *, UI08 >::const_iterator oIter;
	for( oIter = housePrivList.begin(); oIter != housePrivList.end(); ++oIter )
	{
		if( ValidateObject( oIter->first ) )
		{
			if( oIter->second == HOUSEPRIV_OWNER )
				outStream << "CoOwner=";
			else if( oIter->second == HOUSEPRIV_BANNED )
				outStream << "Banned=";
			else
				continue;
			outStream << oIter->first->GetSerial() << '\n';
		}
	}

	ITEMLIST_CITERATOR lIter;
	for( lIter = lockedList.begin(); lIter != lockedList.end(); ++lIter )
	{
		if( ValidateObject( (*lIter) ) )
			outStream << "LockedItem=" << (*lIter)->GetSerial() << '\n';
	}

	outStream << "MaxLockedDown=" << maxLockedDown << '\n';
	// Add deedname= to the save :) We need a way to preserve the name of the item
	outStream << "DeedName=" << deed << '\n';
	return true;
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool HandleLine( UString &UTag, UString &data )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Processes a tag/data pair if it can.  If it can, returns
//|					true.  Otherwise, returns false.
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::HandleLine( UString &UTag, UString &data )
{
	bool rvalue = CItem::HandleLine( UTag, data );
	if( !rvalue )
	{
		switch( (UTag.data()[0]) )
		{
			case 'B':
				if( UTag == "BANNED" )
				{
					CChar *bList = calcCharObjFromSer( data.toUInt() );
					if( ValidateObject( bList ) )
						AddToBanList( bList );
					rvalue = true;
				}
				break;
			case 'C':
				if( UTag == "COOWNER" )
				{
					CChar *cList = calcCharObjFromSer( data.toUInt() );
					if( ValidateObject( cList ) )
						AddAsOwner( cList );
					rvalue = true;
				}
				break;
			case 'D':
				if( UTag == "DEEDNAME" )
				{
					SetDeed( data );
					rvalue = true;
				}
				break;
			case 'L':
				if( UTag == "LOCKEDITEM" )
				{
					CItem *iList = calcItemObjFromSer( data.toUInt() );
					if( ValidateObject( iList ) )
						LockDownItem( iList );
					rvalue = true;
				}
				break;
			case 'M':
				if( UTag == "MAXLOCKEDDOWN" )
				{
					maxLockedDown = data.toUShort();
					rvalue = true;
				}
				break;
		}
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetOwner( CChar *newOwner )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Ensures that newOwner is also on the house ownership list.
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::SetOwner( CChar *newOwner )
{
	if( ValidateObject( newOwner ) )
	{
		owner = newOwner->GetSerial();
		AddAsOwner( newOwner );
	}
	else
		owner = INVALIDSERIAL;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Cleanup( void )
//|	Date		-	26th September, 2001
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes sure that any items and chars inside the multi are removed
//o-----------------------------------------------------------------------------------------------o
void CMultiObj::Cleanup( void )
{
	for( CItem *iRemove = itemInMulti.First(); !itemInMulti.Finished(); iRemove = itemInMulti.Next() )
	{
		if( ValidateObject( iRemove ) )
		{
			ItemTypes iType = iRemove->GetType();
			if( iType == IT_DOOR || iType == IT_LOCKEDDOOR || iType == IT_HOUSESIGN )
				iRemove->Delete();
			else
			{
				if( iRemove->IsLockedDown() )
					iRemove->SetMovable( 1 );
				iRemove->SetMulti( INVALIDSERIAL );
				iRemove->SetZ( GetZ() );
			}
		}
	}
	for( CChar *cRemove = charInMulti.First(); !charInMulti.Finished(); cRemove = charInMulti.Next() )
	{
		if( ValidateObject( cRemove ) )
		{
			cRemove->SetMulti( INVALIDSERIAL );
			cRemove->SetZ( GetZ() );
		}
	}
	CItem::Cleanup();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string GetDeed( void ) const
//|					void SetDeed( const std::string &newDeed )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets item ID of multi's deed item, for converting multi to/from deed
//o-----------------------------------------------------------------------------------------------o
std::string CMultiObj::GetDeed( void ) const
{
	return deed;
}
void CMultiObj::SetDeed( const std::string &newDeed )
{
	deed = newDeed;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanBeObjType( ObjectType toCompare ) const
//|	Date		-	24 June, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Indicates whether an object can behave as a particular type
//o-----------------------------------------------------------------------------------------------o
bool CMultiObj::CanBeObjType( ObjectType toCompare ) const
{
	bool rvalue = CItem::CanBeObjType( toCompare );
	if( !rvalue )
	{
		if( toCompare == OT_MULTI )
			rvalue = true;
	}
	return rvalue;
}

CDataList< CChar * > * CMultiObj::GetCharsInMultiList( void )
{
	return &charInMulti;
}

CDataList< CItem * > * CMultiObj::GetItemsInMultiList( void )
{
	return &itemInMulti;
}

//o-----------------------------------------------------------------------------------------------o
//|	Class		-	CBoatObj
//|	Date		-	25th September, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Version History
//|
//|					1.0		 		25th September, 2003
//|					Very basic, currently
//|					Simplifies some processes in cBoat class, eventually should allow
//|					easy removal of some generic containers in CItem class
//|
//|					1.1		 		28th October, 2003
//|					Added MoveType() to remove the need for CItem::Type2()
//|					Added MoveTime() instead of using CItem::GateTime()
//|					Changed Plank1() and Plank2() to Plank() using an array
//o-----------------------------------------------------------------------------------------------o

const SERIAL		DEFBOAT_TILLER		= INVALIDSERIAL;
const SERIAL		DEFBOAT_HOLD		= INVALIDSERIAL;
const UI08			DEFBOAT_MOVETYPE	= 0;
const TIMERVAL		DEFBOAT_MOVETIME	= 0;

CBoatObj::CBoatObj() : CMultiObj(), tiller( DEFBOAT_TILLER ), hold( DEFBOAT_HOLD ),
moveType( DEFBOAT_MOVETYPE ), nextMoveTime( DEFBOAT_MOVETIME )
{
	planks[0] = planks[1] = INVALIDSERIAL;
	objType = OT_BOAT;
}

CBoatObj::~CBoatObj()
{
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DumpHeader( std::ofstream &outStream ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Dumps out the header for the CMultiObj
//o-----------------------------------------------------------------------------------------------o
bool CBoatObj::DumpHeader( std::ofstream &outStream ) const
{
	outStream << "[BOAT]" << '\n';
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool DumpBody( std::ofstream &outStream ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes out all CMultiObj specific information to a world
//|					file.  Also prints out the Item and BaseObject portions
//o-----------------------------------------------------------------------------------------------o
bool CBoatObj::DumpBody( std::ofstream &outStream ) const
{
	CMultiObj::DumpBody( outStream );

	// Hexadecimal Values
	outStream << std::hex;
	outStream << "Hold=" << "0x" << hold << '\n';
	outStream << "Planks=" << "0x" << planks[0] << ",0x" << planks[1] << '\n';
	outStream << "Tiller=" << "0x" << tiller << std::dec << '\n';

	// Decimal / String Values
	outStream << std::dec;
	return true;
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool HandleLine( UString &UTag, UString &data )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Processes a tag/data pair if it can.  If it can, returns
//|					true.  Otherwise, returns false.
//o-----------------------------------------------------------------------------------------------o
bool CBoatObj::HandleLine( UString &UTag, UString &data )
{
	bool rvalue = CMultiObj::HandleLine( UTag, data );
	if( !rvalue )
	{
		switch( (UTag.data()[0]) )
		{
			case 'M':
				if( UTag == "MOVETYPE" )
				{
					rvalue = true;
				}
			case 'H':
				if( UTag == "HOLD" )
				{
					SetHold( data.toUInt() );
					rvalue = true;
				}
				break;
			case 'P':
				if( UTag == "PLANKS" )
				{
					SetPlank( 0, str_value<std::uint32_t>(trim(extractSection(data, ",", 0, 0 ))) );
					SetPlank( 1, str_value<std::uint32_t>(trim(extractSection(data, ",", 1, 1 ))));
					rvalue = true;
				}
				break;
			case 'T':
				if( UTag == "TILLER" )
				{
					SetTiller( str_value<std::uint16_t>(data) );
					rvalue = true;
				}
				break;
		}
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetTiller( void ) const
//|					void SetTiller( SERIAL newVal )
//|	Date		-	September 25, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets boat's Tiller item reference
//o-----------------------------------------------------------------------------------------------o
SERIAL CBoatObj::GetTiller( void ) const
{
	return tiller;
}
void CBoatObj::SetTiller( SERIAL newVal )
{
	tiller = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetPlank( UI08 plankNum ) const
//|					void SetPlank( UI08 plankNum, SERIAL newVal )
//|	Date		-	September 25, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets boat's Plank item reference
//o-----------------------------------------------------------------------------------------------o
SERIAL CBoatObj::GetPlank( UI08 plankNum ) const
{
	SERIAL rvalue = INVALIDSERIAL;
	if( plankNum < 2 )
		rvalue = planks[plankNum];
	return rvalue;
}
void CBoatObj::SetPlank( UI08 plankNum, SERIAL newVal )
{
	if( plankNum < 2 )
		planks[plankNum] = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL GetHold( void ) const
//|					void SetHold( SERIAL newVal )
//|	Date		-	September 25, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets boat's Hold item reference
//o-----------------------------------------------------------------------------------------------o
SERIAL CBoatObj::GetHold( void ) const
{
	return hold;
}
void CBoatObj::SetHold( SERIAL newVal )
{
	hold = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 GetMoveType( void ) const
//|					void SetMoveType( UI08 newVal )
//|	Date		-	October 28, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets boat's movement type (0 = Not moving, 1 = Moving forward, 2 = Moving backward)
//o-----------------------------------------------------------------------------------------------o
UI08 CBoatObj::GetMoveType( void ) const
{
	return moveType;
}
void CBoatObj::SetMoveType( UI08 newVal )
{
	moveType = newVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	TIMERVAL GetMoveTime( void ) const
//|					void SetMoveTime( TIMERVAL newVal )
//|	Date		-	October 28, 2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets time for when boat will move next
//o-----------------------------------------------------------------------------------------------o
TIMERVAL CBoatObj::GetMoveTime( void ) const
{
	return nextMoveTime;
}
void CBoatObj::SetMoveTime( TIMERVAL newVal )
{
	nextMoveTime = newVal;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CanBeObjType( ObjectType toCompare ) const
//|	Date		-	24 June, 2004
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Indicates whether an object can behave as a particular type
//o-----------------------------------------------------------------------------------------------o
bool CBoatObj::CanBeObjType( ObjectType toCompare ) const
{
	bool rvalue = CMultiObj::CanBeObjType( toCompare );
	if( !rvalue )
	{
		if( toCompare == OT_BOAT )
			rvalue = true;
	}
	return rvalue;
}
