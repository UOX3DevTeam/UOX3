//o--------------------------------------------------------------------------o
//|	File			-	cMultiObj.cpp
//|	Date			-	28th July, 2000
//|	Developers		-	Abaddon/EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Version History
//|									
//|						1.0		Abaddon		28th July, 2000
//|						Very basically fleshed out
//|						Owner, ban list and object inside support included
//|						Note that lockdown stuff is NOT persistent yet, needs support later on
//|						Best place would be world loading, any locked down objects flagged at time added to multi
//|
//|						1.1		Abaddon		28th August, 2000
//|						Function declaration headers
//|						Addition of SetMaxLockDowns
//|
//|						1.2		Abaddon		30th November, 2000
//|						Changed lock downs to a vector, and full vector support added
//|						Save needs modification
//|
//| 					1.3		giwo		25th September 2003
//|						Added CBoatObj as a derived class of CMultiObj to simplify some processes in the cBoat class
//o--------------------------------------------------------------------------o
#include "uox3.h"
#include "classes.h"
#include "regions.h"

namespace UOX
{

const UI16	DEFMULTI_MAXLOCKEDDOWN	= 256;

CMultiObj::CMultiObj() : CItem(), maxLockedDown( DEFMULTI_MAXLOCKEDDOWN ), deed( "" )
{
	objType = OT_MULTI;
}

CMultiObj::~CMultiObj()
{
	owners.resize( 0 );
	banList.resize( 0 );
}

//o--------------------------------------------------------------------------o
//|	Function		-	AddToBanList( CChar *toban )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Adds a character to the ban list, if he's not already on it
//o--------------------------------------------------------------------------o
void CMultiObj::AddToBanList( CChar *toBan )
{
	if( !IsOnBanList( toBan ) )
		banList.push_back( toBan );
}

//o--------------------------------------------------------------------------o
//|	Function		-	RemoveFromBanList( CChar *toRemove )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Removes a character from a ban list
//o--------------------------------------------------------------------------o
void CMultiObj::RemoveFromBanList( CChar *toRemove )
{
	for( CHARLIST_ITERATOR rIter = banList.begin(); rIter != banList.end(); ++rIter )
	{
		if( toRemove == (*rIter) )
			banList.erase( rIter );
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	AddAsOwner( CChar *newOwner )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Adds a player as an owner, so long as they aren't already
//|						and aren't already on the ban list
//o--------------------------------------------------------------------------o
void CMultiObj::AddAsOwner( CChar *newOwner )
{
	if( !IsOwner( newOwner ) && !IsOnBanList( newOwner ) )
		owners.push_back( newOwner );
}

//o--------------------------------------------------------------------------o
//|	Function		-	RemoveAsOwner( CChar *toRemove)
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Removes a character from the owner's list
//o--------------------------------------------------------------------------o
void CMultiObj::RemoveAsOwner( CChar *toRemove )
{
	for( CHARLIST_ITERATOR rIter = owners.begin(); rIter != owners.end(); ++rIter )
	{
		if( toRemove == (*rIter) )
			owners.erase( rIter );
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	ClearOwners( void )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Empties the owner list
//o--------------------------------------------------------------------------o
void CMultiObj::ClearOwners( void )
{
	owners.resize( 0 );
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool IsOnBanList( CChar *toBan )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Returns true if a character is on the ban list
//o--------------------------------------------------------------------------o
bool CMultiObj::IsOnBanList( CChar *toBan ) const
{
	CHARLIST_CITERATOR bIter;
	for( bIter = banList.begin(); bIter != banList.end(); ++bIter )
	{
		if( toBan == (*bIter) )
			return true;
	}
	return false;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool IsOwner( CChar *toFind )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Returns true if the character is an owner
//o--------------------------------------------------------------------------o
bool CMultiObj::IsOwner( CChar *toFind ) const
{
	CHARLIST_CITERATOR oIter;
	for( oIter = owners.begin(); oIter != owners.end(); ++oIter )
	{
		if( toFind == (*oIter) )
			return true;
	}
	return false;
}

//o--------------------------------------------------------------------------o
//|	Function		-	AddToMulti( CBaseObject *toAdd )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Adds an item or character to the multi
//o--------------------------------------------------------------------------o
void CMultiObj::AddToMulti( CBaseObject *toAdd )
{
	if( toAdd->GetObjType() == OT_CHAR )
		charInMulti.Add( static_cast< CChar * >(toAdd) );
	else
		itemInMulti.Add( static_cast< CItem * >(toAdd) );
}

//o--------------------------------------------------------------------------o
//|	Function		-	RemoveFromMulti( CBaseObject *toRemove )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Removes an item or character from the multi, if possible
//o--------------------------------------------------------------------------o
void CMultiObj::RemoveFromMulti( CBaseObject *toRemove )
{
	if( toRemove->GetObjType() == OT_CHAR )
		charInMulti.Remove( static_cast< CChar * >(toRemove) );
	else
		itemInMulti.Remove( static_cast< CItem * >(toRemove) );
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI16 GetLockDownCount( void )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Returns a count of the locked down items
//o--------------------------------------------------------------------------o
size_t CMultiObj::GetLockDownCount( void ) const
{
	return lockedList.size();
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI16 GetMaxLockDowns( void )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Returns the maximum number of possible lockdowns
//o--------------------------------------------------------------------------o
UI32 CMultiObj::GetMaxLockDowns( void ) const
{
	return maxLockedDown;
}

//o--------------------------------------------------------------------------o
//|	Function		-	LockDownItem( CItem *toLock )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Locks down an item toLock
//o--------------------------------------------------------------------------o
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
		lockedList.push_back( toLock );
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	RemoveLockDown( CItem *toRemove )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Unlocks a locked down item
//o--------------------------------------------------------------------------o
void CMultiObj::RemoveLockDown( CItem *toRemove )
{
	for( ITEMLIST_ITERATOR rIter = lockedList.begin(); rIter != lockedList.end(); ++rIter )
	{
		if( (*rIter) == toRemove )
		{
			lockedList.erase( rIter );
			toRemove->SetMovable( 0 );
			return;
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool Save( ofstream &outStream )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Saves a multi out to disk
//|						outStream is the file to write to
//o--------------------------------------------------------------------------o
bool CMultiObj::Save( std::ofstream &outStream )
{
	bool rvalue = false;
	if( !isFree() )
	{
		rvalue = true;
		if( GetCont() != NULL || ( GetX() > 0 && GetX() < 6144 && GetY() < 4096 ) )
		{
			DumpHeader( outStream );
			DumpBody( outStream );
			DumpFooter( outStream );
		}
	}
	return rvalue;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool LoadRemnants( UI32 arrayOffset )
//|	Date			-	20th January, 2002
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	After handling data specific load, other parts go here
//o--------------------------------------------------------------------------o
bool CMultiObj::LoadRemnants( void )
{
	bool rValue = CItem::LoadRemnants();
//	Items->StoreItemRandomValue( this, NULL );

	// Add item weight if item doesn't have it yet
	if( GetWeight() <= 0 )
		SetWeight( 0 );
	return rValue;
}

//o--------------------------------------------------------------------------o
//|	Function		-	SetMaxLockDowns( UI16 newValue )
//|	Date			-	28th August, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Sets the maximum number of lockdowns possible
//o--------------------------------------------------------------------------o
void CMultiObj::SetMaxLockDowns( UI16 newValue )
{
	maxLockedDown = newValue;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool DumpHeader( ofstream &outStream )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Dumps out the header for the CMultiObj
//o--------------------------------------------------------------------------o
bool CMultiObj::DumpHeader( std::ofstream &outStream ) const
{
	outStream << "[HOUSE]" << std::endl;
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool DumpBody( ofstream &outStream )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Writes out all CMultiObj specific information to a world
//|						file.  Also prints out the Item and BaseObject portions
//o--------------------------------------------------------------------------o
bool CMultiObj::DumpBody( std::ofstream &outStream ) const
{
	std::string destination; 
	std::ostringstream dumping( destination ); 
	
	CHARLIST_CITERATOR ban, owner;
	CItem::DumpBody( outStream );

	for( ban = banList.begin(); ban != banList.end(); ++ban )
	{
		if( ValidateObject( (*ban) ) )
			dumping << "Banned=" << (*ban)->GetSerial() << std::endl;
	}

	for( owner = owners.begin(); owner != owners.end(); ++owner )
	{
		if( ValidateObject( (*owner) ) )
			dumping << "CoOwner=" << (*owner)->GetSerial() << std::endl;
	}

	ITEMLIST_CITERATOR lIter;
	for( lIter = lockedList.begin(); lIter != lockedList.end(); ++lIter )
	{
		if( ValidateObject( (*lIter) ) )
			dumping << "LockedItem=" << (*lIter)->GetSerial() << std::endl;
	}

	dumping << "MaxLockedDown=" << maxLockedDown << std::endl;
	// Add deedname= to the save :) We need a way to preserve the name of the item
	dumping << "DeedName=" << deed << std::endl;
	outStream << dumping.str();
	return true;
}


//o--------------------------------------------------------------------------o
//|	Function		-	bool HandleLine( UString &UTag, UString &data )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Processes a tag/data pair if it can.  If it can, returns
//|						true.  Otherwise, returns false.
//o--------------------------------------------------------------------------o
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
					CChar *bList = calcCharObjFromSer( data.toULong() );
					if( ValidateObject( bList ) )
						AddToBanList( bList );
					rvalue = true;
				}
				break;
			case 'C':
				if( UTag == "COOWNER" )
				{
					CChar *cList = calcCharObjFromSer( data.toULong() );
					if( ValidateObject( cList ) )
						AddAsOwner( cList );
					rvalue = true;
				}
				break;
			case 'D':
				{
					if( UTag == "DEEDNAME" )
					{
						SetDeed( data );
						rvalue = true;
					}
					break;
				}
				break;
			case 'L':
				if( UTag == "LOCKEDITEM" )
				{
					CItem *iList = calcItemObjFromSer( data.toULong() );
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

//o--------------------------------------------------------------------------o
//|	Function		-	SetOwner( CChar *newOwner )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Ensures that newOwner is also on the house ownership
//|						list.
//o--------------------------------------------------------------------------o
void CMultiObj::SetOwner( CChar *newOwner )
{
	owner = newOwner;
	AddAsOwner( newOwner );
}

//o--------------------------------------------------------------------------o
//|	Function		-	Cleanup( void )
//|	Date			-	26th September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Makes sure that any items and chars inside the multi
//|						are removed
//o--------------------------------------------------------------------------o
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
				iRemove->SetMulti( INVALIDSERIAL );
		}
	}
	for( CChar *cRemove = charInMulti.First(); !charInMulti.Finished(); cRemove = charInMulti.Next() )
	{
		if( ValidateObject( cRemove ) )
			cRemove->SetMulti( INVALIDSERIAL );
	}
	CItem::Cleanup();
}

std::string CMultiObj::GetDeed( void ) const
{
	return deed;
}

void CMultiObj::SetDeed( std::string newDeed )
{
	deed = newDeed;
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

//o--------------------------------------------------------------------------o
//|	Class			-	CBoatObj
//|	Date			-	25th September, 2003
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Version History
//|									
//|						1.0		giwo		25th September, 2003
//|						Very basic, currently
//|						Simplifies some processes in cBoat class, eventually should allow
//|						easy removal of some generic containers in CItem class
//|
//|						1.1		giwo		28th October, 2003
//|						Added MoveType() to remove the need for CItem::Type2()
//|						Added MoveTime() instead of using CItem::GateTime()
//|						Changed Plank1() and Plank2() to Plank() using an array
//o--------------------------------------------------------------------------o

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

//o--------------------------------------------------------------------------o
//|	Function		-	bool DumpHeader( ofstream &outStream )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Dumps out the header for the CMultiObj
//o--------------------------------------------------------------------------o
bool CBoatObj::DumpHeader( std::ofstream &outStream ) const
{
	outStream << "[BOAT]" << std::endl;
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool DumpBody( ofstream &outStream )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Writes out all CMultiObj specific information to a world
//|						file.  Also prints out the Item and BaseObject portions
//o--------------------------------------------------------------------------o
bool CBoatObj::DumpBody( std::ofstream &outStream ) const
{
	std::string destination; 
	std::ostringstream dumping( destination ); 

	CMultiObj::DumpBody( outStream );
	dumping << "Hold=" << "0x" << std::hex << hold << std::endl;
	dumping << "Planks=" << "0x" << std::hex << planks[0] << "," << "0x" << planks[1] << std::endl;
	dumping << "Tiller=" << "0x" << std::hex << tiller << std::endl;
	dumping << "MoveType=" << std::dec << moveType << std::endl;
	
	outStream << dumping.str();
	return true;
}


//o--------------------------------------------------------------------------o
//|	Function		-	bool HandleLine( UString &UTag, UString &data )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Processes a tag/data pair if it can.  If it can, returns
//|						true.  Otherwise, returns false.
//o--------------------------------------------------------------------------o
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
					SetMoveType( data.toUByte() );
					rvalue = true;
				}
			case 'H':
				if( UTag == "HOLD" )
				{
					SetHold( data.toULong() );
					rvalue = true;
				}
				break;
			case 'P':
				if( UTag == "PLANKS" )
				{
					SetPlank( 0, data.section( ",", 0, 0 ).stripWhiteSpace().toULong() );
					SetPlank( 1, data.section( ",", 1, 1 ).stripWhiteSpace().toULong() );
					rvalue = true;
				}
				break;
			case 'T':
				if( UTag == "TILLER" )
				{
					SetTiller( data.toULong() );
					rvalue = true;
				}
				break;
		}
	}
	return rvalue;
}

//o--------------------------------------------------------------------------o
//|	Function		-	SERIAL Tiller()
//|	Date			-	September 25, 2003
//|	Programmer		-	giwo
//o--------------------------------------------------------------------------o
//|	Purpose			-	Boats Tiller Item Refrence
//o--------------------------------------------------------------------------o
SERIAL CBoatObj::GetTiller( void ) const
{
	return tiller;
}
void CBoatObj::SetTiller( SERIAL newVal )
{
	tiller = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	SERIAL Plank( UI08 plankNum )
//|	Date			-	September 25, 2003
//|	Programmer		-	giwo
//o--------------------------------------------------------------------------o
//|	Purpose			-	Boats Plank Item Refrences
//o--------------------------------------------------------------------------o
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

//o--------------------------------------------------------------------------o
//|	Function		-	SERIAL Hold()
//|	Date			-	September 25, 2003
//|	Programmer		-	giwo
//o--------------------------------------------------------------------------o
//|	Purpose			-	Boats Hold Item Refrence
//o--------------------------------------------------------------------------o
SERIAL CBoatObj::GetHold( void ) const
{
	return hold;
}
void CBoatObj::SetHold( SERIAL newVal )
{
	hold = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI08 MoveType()
//|	Date			-	October 28, 2003
//|	Programmer		-	giwo
//o--------------------------------------------------------------------------o
//|	Purpose			-	Boats movement type (0 = Not moving, 1 = Moving forward, 2 = Moving backward)
//o--------------------------------------------------------------------------o
UI08 CBoatObj::GetMoveType( void ) const
{
	return moveType;
}
void CBoatObj::SetMoveType( UI08 newVal )
{
	moveType = newVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	TIMERVAL MoveTime()
//|	Date			-	October 28, 2003
//|	Programmer		-	giwo
//o--------------------------------------------------------------------------o
//|	Purpose			-	Next time a boat will move
//o--------------------------------------------------------------------------o
TIMERVAL CBoatObj::GetMoveTime( void ) const
{
	return nextMoveTime;
}
void CBoatObj::SetMoveTime( TIMERVAL newVal )
{
	nextMoveTime = newVal;
}
//o--------------------------------------------------------------------------o
//|	Function		-	bool CanBeObjType()
//|	Date			-	24 June, 2004
//|	Programmer		-	Maarc
//|	Modified		-
//o--------------------------------------------------------------------------o
//|	Purpose			-	Indicates whether an object can behave as a
//|						particular type
//o--------------------------------------------------------------------------o
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

}
