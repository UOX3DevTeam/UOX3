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
	std::map< SERIAL, CChar *>::iterator p = banList.find( toRemove->GetSerial() );
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
	std::map< SERIAL, CChar *>::iterator p = owners.find( toRemove->GetSerial() );
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

CMultiObj::CMultiObj( ITEM nItem, bool zeroSer ) : CItem( nItem, zeroSer ), maxLockedDown( 256 )
{
	objType = OT_MULTI;
}

CMultiObj::~CMultiObj()
{
	Cleanup();
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
UI32 CMultiObj::GetLockDownCount( void ) const
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
UI32 CMultiObj::GetMaxLockDowns( void ) const
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
		for( UI32 i = 0; i < lockedList.size(); i++ )
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
	for( UI32 i = 0; i < lockedList.size(); i++ )
	{
		if( lockedList[i] == toRemove )
		{
			for( UI32 j = i; j < lockedList.size() - 1; j++ )
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
bool CMultiObj::Save( std::ofstream &outStream, int mode )
{
	if( isFree() )
		return false;
	typedef std::map< SERIAL, CChar *>::iterator iCounter;
	if( GetCont() != INVALIDSERIAL || ( GetX() > 0 && GetX() < 6144 && GetY() < 4096 ) )
	{
		DumpHeader( outStream, mode );
		DumpBody( outStream, mode );
		DumpFooter( outStream, mode );
	}
	return true;
}

bool CMultiObj::Load( BinBuffer &buff, int arrayOffset )
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
				Console.Warning( 2, "Unknown CMultiObj world file tag %i [%x] length of %i (Last Tag %i[%x])", tag, tag, ItemSaveTable[tag], ltag, ltag );
		}
	}
	
	// We have to do this here, otherwise we return prematurely, WITHOUT having read the entire record!
//	if( isDirty )
//		return false;
	return LoadRemnants( arrayOffset );

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
bool CMultiObj::Load( std::ifstream &inStream, int arrayOffset )
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
				Console.Warning( 2, "Unknown world file tag %s with contents of %s", tag, data );
			}
		}
	}
	return LoadRemnants( arrayOffset );	
}

//o--------------------------------------------------------------------------
//|	Function		-	bool LoadRemnants( int arrayOffset )
//|	Date			-	20th January, 2002
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	After handling data specific load, other parts go here
//o--------------------------------------------------------------------------
bool CMultiObj::LoadRemnants( int arrayOffset )
{
	if( itemcount2 <= serial ) 
		itemcount2 = serial + 1;
	SetSerial( serial, arrayOffset );
	StoreItemRandomValue( &items[arrayOffset], -1 ); // Magius(CHE) (2)
	

	// Add item weight if item doesn't have it yet
	if( GetWeight() <= 0 )
		SetWeight( 0 );

	// Tauriel adding region pointers
	if( GetCont() == INVALIDSERIAL )
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
bool CMultiObj::DumpHeader( std::ofstream &outStream, int mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
		outStream << "[HOUSE]" << std::endl;
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
bool CMultiObj::DumpBody( std::ofstream &outStream, int mode ) const
{
	std::string destination; 
	std::ostringstream dumping( destination ); 
	BinBuffer buff;
	baseCharMapConstIterator ban, owner;
	UI32 iCtr;

	switch( mode )
	{
	case 1:	//binary
		if( !CItem::DumpBody( buff ) )
			return false;
		buff.SetType( 3 );

		for( ban = banList.begin(); ban != banList.end(); ban++ )
		{
			buff.PutByte( MULTITAG_BAN );
			buff.PutLong( (ban->second)->GetSerial() );
		}

		for( owner = owners.begin(); owner != owners.end(); owner++ )
		{
			buff.PutByte( MULTITAG_OWNER );
			buff.PutLong( (owner->second)->GetSerial() );
		}

		for( iCtr = 0; iCtr < lockedList.size(); iCtr++ )
		{
			if( lockedList[iCtr] != NULL )
			{
				buff.PutByte( MULTITAG_LOCKDOWN );
				buff.PutLong( lockedList[iCtr]->GetSerial() );
			}
		}

		buff.PutByte( MULTITAG_MAXLOCK );
		buff.PutShort( maxLockedDown );

		FilePosition = outStream.tellp();
		buff.Write( outStream );
		break;
	case 0:
	default:
		CItem::DumpBody( outStream, mode );

		for( ban = banList.begin(); ban != banList.end(); ban++ )
			dumping << "Banned=" << (ban->second)->GetSerial() << std::endl;

		for( owner = owners.begin(); owner != owners.end(); owner++ )
			dumping << "CoOwner=" << (owner->second)->GetSerial() << std::endl;

		for( iCtr = 0; iCtr < lockedList.size(); iCtr++ )
		{
			if( lockedList[iCtr] != NULL )
				dumping << "LockedItem=" << lockedList[iCtr]->GetSerial() << std::endl;
		}

		dumping << "MaxLockedDown=" << maxLockedDown << std::endl;
		outStream << dumping.str();
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
bool CMultiObj::DumpFooter( std::ofstream &outStream, int mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
		outStream << std::endl << std::endl << "o---o" << std::endl;
		break;
	}
	return true;
}

bool CMultiObj::HandleBinTag( UI08 tag, BinBuffer &buff )
{
	CChar *cList = NULL;
	CItem *iList = NULL;

	if( CItem::HandleBinTag( tag, buff ) )
		return true;

	switch ( tag )
	{
	case MULTITAG_BAN:	// b, o, l, m
		cList = calcCharObjFromSer( buff.GetLong() );
		if( cList != NULL ) 
			AddToBanList( cList );
		break;

	case MULTITAG_OWNER:
		cList = calcCharObjFromSer( buff.GetLong() );
		if( cList != NULL ) 
			AddAsOwner( cList );
		break;

	case MULTITAG_LOCKDOWN:
		iList = calcItemObjFromSer( buff.GetLong() );
		if( iList != NULL )	
			LockDownItem( iList );
		break;

	case MULTITAG_MAXLOCK:
		maxLockedDown = buff.GetShort();
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
			CChar *bList = calcCharObjFromSer( makeNum( data ) );
			if( bList != NULL )
				AddToBanList( bList );
			return true;
		}
		break;
	case 'C':
		if( !strcmp( tag, "CoOwner" ) )
		{
			CChar *cList = calcCharObjFromSer( makeNum( data ) );
			if( cList != NULL )
				AddAsOwner( cList );
			return true;
		}
		break;
	case 'L':
		if( !strcmp( tag, "LockedItem" ) )
		{
			CItem *iList = calcItemObjFromSer( makeNum( data ) );
			if( iList != NULL )
				LockDownItem( iList );
			return true;
		}
		break;
	case 'M':
		if( !strcmp( tag, "MaxLockedDown" ) )
		{
			maxLockedDown = (UI16)makeNum( data );
			return true;
		}
		break;
	}
	return false;
}

//o--------------------------------------------------------------------------
//|	Function		-	void PostLoadProcessing( UI32 index )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Does any post load processing required
//o--------------------------------------------------------------------------
void CMultiObj::PostLoadProcessing( UI32 index )
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

//o--------------------------------------------------------------------------
//|	Function		-	Cleanup( void )
//|	Date			-	26th September, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Makes sure that any items and chars inside the multi
//|						are removed
//o--------------------------------------------------------------------------
void CMultiObj::Cleanup( void )
{
	std::vector< cBaseObject * > toRemove;
	baseMapIterator iCleanup = itemInMulti.begin();
	while( iCleanup != itemInMulti.end() )
	{
		cBaseObject *iRemove = iCleanup->second;
		if( iRemove != NULL )
			toRemove.push_back( iRemove );
		iCleanup++;
	}
	baseMapIterator cCleanup = charInMulti.begin();
	while( cCleanup != charInMulti.end() )
	{
		cBaseObject *cRemove = cCleanup->second;
		if( cRemove != NULL )
			toRemove.push_back( cRemove );
		cCleanup++;
	}
	for( UI32 i = 0; i < toRemove.size(); i++ )
		toRemove[i]->SetMulti( (cBaseObject *)NULL );
}
