#include "uox3.h"

//	1.0		Abaddon		28th July, 2000
//			Very basically fleshed out
//			Owner, ban list and object inside support included
//			Note that lockdown stuff is NOT persistent yet, needs support later on
//			Best place would be world loading, any locked down objects flagged at time added to multi

//	1.1		Abaddon		28th August, 2000
//			Function declaration headers
//			Addition of SetMaxLockDowns

//	1.2		Abaddon		30th November, 2000
//			Changed lock downs to a vector, and full vector support added
//			Save needs modification

//o--------------------------------------------------------------------------
//|	Function		-	AddToBanList( CChar *toban )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Adds a character to the ban list, if he's not already on it
//o--------------------------------------------------------------------------
void CMultiObj::AddToBanList( CChar *toBan )
{
	if( !IsOnBanList( toBan ) )
		banList[toBan->GetSerial()] = toBan;
}

//o--------------------------------------------------------------------------
//|	Function		-	RemoveFromBanList( CChar *toRemove )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Removes a character from a ban list
//o--------------------------------------------------------------------------
void CMultiObj::RemoveFromBanList( CChar *toRemove )
{
	map< SERIAL, CChar *>::iterator p = banList.find( toRemove->GetSerial() );
	if( p != banList.end() )
		banList.erase( p );
}

//o--------------------------------------------------------------------------
//|	Function		-	AddAsOwner( CChar *newOwner )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Adds a player as an owner, so long as they aren't already
//|						and aren't already on the ban list
//o--------------------------------------------------------------------------
void CMultiObj::AddAsOwner( CChar *newOwner )
{
	if( !IsOwner( newOwner ) && !IsOnBanList( newOwner ) )
		owners[newOwner->GetSerial()] = newOwner;
}

//o--------------------------------------------------------------------------
//|	Function		-	RemoveAsOwner( CChar *toRemove)
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Removes a character from the owner's list
//o--------------------------------------------------------------------------
void CMultiObj::RemoveAsOwner( CChar *toRemove )
{
	map< SERIAL, CChar *>::iterator p = owners.find( toRemove->GetSerial() );
	if( p != owners.end() )
		owners.erase( p );
}

//o--------------------------------------------------------------------------
//|	Function		-	ClearOwners( void )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Empties the owner list
//o--------------------------------------------------------------------------
void CMultiObj::ClearOwners( void )
{
	owners.clear();
}

bool CMultiObj::ItemIsInMulti( cBaseObject *toFind ) const
{
	baseMapConstIterator p = itemInMulti.find( toFind->GetSerial() );
	return ( p != itemInMulti.end() );
}

bool CMultiObj::CharIsInMulti( cBaseObject *toFind ) const
{
	baseMapConstIterator p = charInMulti.find( toFind->GetSerial() );
	return ( p != charInMulti.end() );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool IsInMulti( cBaseObject *toFind )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if an item or character is in the multi
//o--------------------------------------------------------------------------
bool CMultiObj::IsInMulti( cBaseObject *toFind ) const
{
	if( toFind->GetObjType() == OT_CHAR )
		return CharIsInMulti( toFind );
	else
		return ItemIsInMulti( toFind );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool IsOnBanList( CChar *toBan )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if a character is on the ban list
//o--------------------------------------------------------------------------
bool CMultiObj::IsOnBanList( CChar *toBan ) const
{
	baseCharMapConstIterator p = banList.find( toBan->GetSerial() );
	return ( p != banList.end() );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool IsOwner( CChar *toFind )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the character is an owner
//o--------------------------------------------------------------------------
bool CMultiObj::IsOwner( CChar *toFind ) const
{
	baseCharMapConstIterator p = owners.find( toFind->GetSerial() );
	return ( p != owners.end() );
}

void CMultiObj::AddCharToMulti( cBaseObject *toFind )
{
	charInMulti[toFind->GetSerial()] = toFind;
}
void CMultiObj::AddItemToMulti( cBaseObject *toFind )
{
	itemInMulti[toFind->GetSerial()] = toFind;
}

//o--------------------------------------------------------------------------
//|	Function		-	AddToMulti( cBaseObject *toAdd )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Adds an item or character to the multi
//o--------------------------------------------------------------------------
void CMultiObj::AddToMulti( cBaseObject *toAdd )
{
	if( !IsInMulti( toAdd ) )
	{
		if( toAdd->GetObjType() == OT_CHAR )
			AddCharToMulti( toAdd );
		else
			AddItemToMulti( toAdd );
	}
}

void CMultiObj::RemoveCharFromMulti( cBaseObject *toFind )
{
	baseMapIterator p = charInMulti.find( toFind->GetSerial() );
	if( p != charInMulti.end() )
		charInMulti.erase( p );
}
void CMultiObj::RemoveItemFromMulti( cBaseObject *toFind )
{
	baseMapIterator p = itemInMulti.find( toFind->GetSerial() );
	if( p != itemInMulti.end() )
		itemInMulti.erase( p );
}

//o--------------------------------------------------------------------------
//|	Function		-	RemoveFromMulti( cBaseObject *toRemove )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Removes an item or character from the multi, if possible
//o--------------------------------------------------------------------------
void CMultiObj::RemoveFromMulti( cBaseObject *toRemove )
{
	if( toRemove->GetObjType() == OT_CHAR )
		RemoveCharFromMulti( toRemove );
	else
		RemoveItemFromMulti( toRemove );
}

CMultiObj::CMultiObj() : maxLockedDown( 256 )
{
	objType = OT_MULTI;
}

CMultiObj::~CMultiObj()
{
	charInMulti.clear();
	itemInMulti.clear();
	owners.clear();
	banList.clear();
}

//o--------------------------------------------------------------------------
//|	Function		-	UI16 GetLockDownCount( void )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns a count of the locked down items
//o--------------------------------------------------------------------------
UI16 CMultiObj::GetLockDownCount( void ) const
{
	return lockedList.size();
}

//o--------------------------------------------------------------------------
//|	Function		-	UI16 GetMaxLockDowns( void )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the maximum number of possible lockdowns
//o--------------------------------------------------------------------------
UI16 CMultiObj::GetMaxLockDowns( void ) const
{
	return maxLockedDown;
}

//o--------------------------------------------------------------------------
//|	Function		-	LockDownItem( CItem *toLock )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Locks down an item toLock
//o--------------------------------------------------------------------------
void CMultiObj::LockDownItem( CItem *toLock )
{
	if( lockedList.size() < maxLockedDown )
	{
		for( int i = 0; i < lockedList.size(); i++ )
		{
			if( toLock == lockedList[i] )
				return;
		}
		toLock->LockDown();
		lockedList.push_back( toLock );
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	RemoveLockDown( CItem *toRemove )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Unlocks a locked down item
//o--------------------------------------------------------------------------
void CMultiObj::RemoveLockDown( CItem *toRemove )
{
	for( int i = 0; i < lockedList.size(); i++ )
	{
		if( lockedList[i] == toRemove )
		{
			for( int j = i; j < lockedList.size() - 1; j++ )
				lockedList[j] = lockedList[j+1];
			lockedList.resize( lockedList.size() - 1 );
			toRemove->SetMagic( 0 );
		}
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	bool Save( ofstream &outStream, int mode )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Saves a multi out to disk
//|						outStream is the file to write to
//|						mode is the mode to write in (1 for binary, 0 for text)
//o--------------------------------------------------------------------------
bool CMultiObj::Save( ofstream &outStream, int mode )
{
	typedef map< SERIAL, CChar *>::iterator iCounter;
	if( !isFree() && ( GetCont() != 0xFFFFFFFF || ( GetX() > 0 && GetX() < 6144 && GetY() < 4096 ) ) )
	{
		DumpHeader( outStream, mode );
		DumpBody( outStream, mode );
		DumpFooter( outStream, mode );
	}
	return true;
}
//o--------------------------------------------------------------------------
//|	Function		-	bool Load( ifstream &instream, int mode )
//|	Date			-	28th July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Loads an item from disk, using instream as the source
//|						and mode as the way to load (0 for bin, 1 for text)
//o--------------------------------------------------------------------------
bool CMultiObj::Load( ifstream &inStream, int mode, int arrayOffset )
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


	// Add item weight if item doesn't have it yet
	if( GetWeight() <= 0 )
		SetWeight( 0 );

	// Tauriel adding region pointers
	if( GetCont() == 0xFFFFFFFF )
	{
		MapRegion->AddItem( this );
		if( GetX() < 0 || GetY() < 0 )
			return false;
	}
	return true;

}

//o--------------------------------------------------------------------------
//|	Function		-	SetMaxLockDowns( UI16 newValue )
//|	Date			-	28th August, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets the maximum number of lockdowns possible
//o--------------------------------------------------------------------------
void CMultiObj::SetMaxLockDowns( UI16 newValue )
{
	maxLockedDown = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	CChar *FirstCharMulti()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the first character in the multi (if none, NULL)
//o--------------------------------------------------------------------------
CChar *CMultiObj::FirstCharMulti( void )
{
	charMultiIterator = charInMulti.begin();
	if( FinishedCharMulti() )
		return NULL;
	return (CChar *)charMultiIterator->second;
}

//o--------------------------------------------------------------------------
//|	Function		-	CChar *NextCharMulti()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the next character in the multi
//o--------------------------------------------------------------------------
CChar *CMultiObj::NextCharMulti( void )
{
	charMultiIterator++;
	if( FinishedCharMulti() )
		return NULL;
	return (CChar *)charMultiIterator->second;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool FinishedCharMulti()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if there are no more chars in the multi
//o--------------------------------------------------------------------------
bool CMultiObj::FinishedCharMulti( void ) const
{
	baseMapConstIterator t = charMultiIterator;
	return ( t == charInMulti.end() );
}

//o--------------------------------------------------------------------------
//|	Function		-	CItem *FirstItemMulti()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the first item in the multi
//o--------------------------------------------------------------------------
CItem *CMultiObj::FirstItemMulti( void )
{
	itemMultiIterator = itemInMulti.begin();
	if( FinishedItemMulti() )
		return NULL;
	return (CItem *)itemMultiIterator->second;
}

//o--------------------------------------------------------------------------
//|	Function		-	CItem *NextItemMulti()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the next item (if any) in the multi
//o--------------------------------------------------------------------------
CItem *CMultiObj::NextItemMulti( void )
{
	itemMultiIterator++;
	if( FinishedItemMulti() )
		return NULL;
	return (CItem *)itemMultiIterator->second;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool FinishedItemMulti()
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if there are no more items in the multi
//o--------------------------------------------------------------------------
bool CMultiObj::FinishedItemMulti( void ) const
{
	baseMapConstIterator t = itemMultiIterator;
	return ( t == itemInMulti.end() );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool DumpHeader( ofstream &outStream, int mode )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Dumps out the header for the CMultiObj
//o--------------------------------------------------------------------------
bool CMultiObj::DumpHeader( ofstream &outStream, int mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
//		LONG zFilePosition = outStream.tellp();
//		outStream << "[" << GetSerial() << "]" << GetObjType() << "[" << zFilePosition << "] - House Dump" << endl;
		outStream << "[HOUSE]" << endl;
		break;
	}
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool DumpBody( ofstream &outStream, int mode )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Writes out all CMultiObj specific information to a world
//|						file.  Also prints out the Item and BaseObject portions
//o--------------------------------------------------------------------------
bool CMultiObj::DumpBody( ofstream &outStream, int mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
		CItem::DumpBody( outStream, mode );

		for( baseCharMapConstIterator ban = banList.begin(); ban != banList.end(); ban++ )
			outStream << "Banned=" << (ban->second)->GetSerial() << endl;

		for( baseCharMapConstIterator owner = owners.begin(); owner != owners.end(); owner++ )
			outStream << "CoOwner=" << (owner->second)->GetSerial() << endl;

		for( int iCtr = 0; iCtr < lockedList.size(); iCtr++ )
		{
			if( lockedList[iCtr] != NULL )
				outStream << "LockedItem=" << lockedList[iCtr]->GetSerial() << endl;
		}

		outStream << "MaxLockedDown=" << maxLockedDown << endl;
		break;
	}
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool DumpFooter( ofstream &outStream, int mode )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Writes out a footer
//o--------------------------------------------------------------------------
bool CMultiObj::DumpFooter( ofstream &outStream, int mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
		outStream << endl << endl << "o---o" << endl;
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
//|	Purpose			-	Processes a tag/data pair if it can.  If it can, returns
//|						true.  Otherwise, returns false.
//o--------------------------------------------------------------------------
bool CMultiObj::HandleLine( char *tag, char *data )
{
	if( CItem::HandleLine( tag, data ) )
		return true;
	switch( tag[0] )
	{
	case 'B':
		if( !strcmp( tag, "Banned" ) )
		{
			CChar *bList = calcCharObjFromSer( atoi( data ) );
			if( bList != NULL )
				AddToBanList( bList );
			return true;
		}
		break;
	case 'C':
		if( !strcmp( tag, "CoOwner" ) )
		{
			CChar *cList = calcCharObjFromSer( atoi( data ) );
			if( cList != NULL )
				AddAsOwner( cList );
			return true;
		}
		break;
	case 'L':
		if( !strcmp( tag, "LockedItem" ) )
		{
			CItem *iList = calcItemObjFromSer( atoi( data ) );
			if( iList != NULL )
				LockDownItem( iList );
			return true;
		}
		break;
	case 'M':
		if( !strcmp( tag, "MaxLockedDown" ) )
		{
			maxLockedDown = atoi( data );
			return true;
		}
		break;
	}
	return false;
}

//o--------------------------------------------------------------------------
//|	Function		-	void PostLoadProcessing( SI32 index )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Does any post load processing required
//o--------------------------------------------------------------------------
void CMultiObj::PostLoadProcessing( SI32 index )
{
	CItem::PostLoadProcessing( index );
}


//o--------------------------------------------------------------------------
//|	Function		-	SetOwner( CChar *newOwner )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Ensures that newOwner is also on the house ownership
//|						list.
//o--------------------------------------------------------------------------
void CMultiObj::SetOwner( CChar *newOwner )
{
	cBaseObject::SetOwner( newOwner->GetSerial() );
	AddAsOwner( newOwner );
}