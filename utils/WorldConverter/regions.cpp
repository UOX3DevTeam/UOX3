#include "uox3.h"

int MemCharFree( bool zeroSer );
void DeleteChar( CHARACTER k );
int MemItemFree( UI08 itemType = 0 );
void InitItem( ITEM nItem, bool ser );
void DeleItem( int i );

void readline( ifstream &toRead, char *buffer, int bufferLen )
{
	bool valid = false;
	do 
	{
		toRead.getline( buffer, bufferLen );
		if( toRead.eof() || toRead.fail() )
		{
			strcpy( buffer, "---REGION---" );
			valid = true;
		}
		else if( buffer[0] == '/' || buffer[0] == ' ' || buffer[0] == 10 || buffer[0] == 13 || buffer[0] == 0 )
			valid = false;
		else
			valid = true;
	} while( !valid );
}

void SavePretendMulti( CItem *toSave, ofstream& writeDestination, SI32 mode )
{
	if( toSave == NULL )
		return;
	writeDestination << "[HOUSE]" << endl;
	toSave->DumpBody( writeDestination, mode );

	writeDestination << "MaxLockedDown=" << 10 << endl;
	toSave->DumpFooter( writeDestination, mode );
}

void LoadChar( ifstream& readDestination, SI32 mode )
{
	long x = MemCharFree( true );
	if( x == -1 ) 
		return;
	if( !chars[x].Load( readDestination, mode, x ) )
	{	// if no load, DELETE
		DeleteChar( x );
	}
}
void LoadItem( ifstream& readDestination, SI32 mode )
{
	long x = MemItemFree();
	if( x == -1 ) 
		return;
	InitItem( x, false );
	if( !items[x].Load( readDestination, mode, x ) )
	{	// if no load, DELETE
		DeleItem( x );
	}
}
void LoadMulti( ifstream& readDestination, SI32 mode )
{
	int x = MemItemFree( 1 );
	if( x == -1 ) 
		return;
	InitItem( x, false );
	CMultiObj *ourHouse = static_cast<CMultiObj *>(&items[x]);
	if( !ourHouse->Load( readDestination, mode, x ) )
	{	// if no load, DELETE
		DeleItem( x );
	}
}

typedef vector< int >::iterator vIterator;
//o--------------------------------------------------------------------------
//|	Function		-	SubRegion constructor
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	This function is does basically what the name implies
//o--------------------------------------------------------------------------
SubRegion::SubRegion()  //constructor
{
}

//o--------------------------------------------------------------------------
//|	Function		-	SubRegion destructor
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Removes all map entries
//o--------------------------------------------------------------------------
SubRegion::~SubRegion()
{
	itemData.clear();
	charData.clear();
}

//o--------------------------------------------------------------------------
//|	Function		-	CItem *GetCurrentItem()
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns a CItem * to the current referenced item
//o--------------------------------------------------------------------------
CItem *SubRegion::GetCurrentItem( void )
{
	return itemData[itemCounter];
}

//o--------------------------------------------------------------------------
//|	Function		-	CChar *GetCurrentChar()
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns a CChar * to the current referenced char
//o--------------------------------------------------------------------------
CChar *SubRegion::GetCurrentChar( void )
{
	return charData[charCounter];
}

//o--------------------------------------------------------------------------
//|	Function		-	CItem *FirstItem()
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Moves iterator to start and returns the first CItem *
//o--------------------------------------------------------------------------
CItem *SubRegion::FirstItem( void )
{
	itemCounter = 0;
	if( itemCounter >= itemData.size() )
		return NULL;
	else
		return itemData[itemCounter];
}

//o--------------------------------------------------------------------------
//|	Function		-	CChar *FirstChar()
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Moves iterator to start and returns the first CChar *
//o--------------------------------------------------------------------------
CChar *SubRegion::FirstChar( void )
{
	charCounter = 0;
	if( charCounter >= charData.size() )
		return NULL;
	else
		return charData[charCounter];
}

//o--------------------------------------------------------------------------
//|	Function		-	CItem *GetNextItem()
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Moves along the iterator, returning the next CItem *
//o--------------------------------------------------------------------------
CItem *SubRegion::GetNextItem( void )
{
	if( ++itemCounter >= itemData.size() )
		return NULL;
	else
		return itemData[itemCounter];
}

//o--------------------------------------------------------------------------
//|	Function		-	CChar *GetNextChar()
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Moves along the iterator, returning the next CChar *
//o--------------------------------------------------------------------------
CChar *SubRegion::GetNextChar( void )
{
	if( ++charCounter >= charData.size() )
		return NULL;
	else
		return charData[charCounter];
}


//o--------------------------------------------------------------------------
//|	Function		-	bool FinishedItems()
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	returns true if we hit the end of the iterator
//o--------------------------------------------------------------------------
bool   SubRegion::FinishedItems( void )
{
	return ( itemCounter >= itemData.size() );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool FinishedChars()
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if we hit the end of the iterator
//o--------------------------------------------------------------------------
bool   SubRegion::FinishedChars( void )
{
	return ( charCounter >= charData.size() );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool AddItem( CItem *toAdd )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Adds toAdd to the map if it doesn't already exist
//o--------------------------------------------------------------------------
bool SubRegion::AddItem( CItem *toAdd )
{
	for( int p = 0; p < itemData.size(); p++ )
	{
		if( itemData[p] == toAdd )
			return false;
	}
	itemData.push_back( toAdd );
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool AddChar( CChar *toAdd )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Adds toAdd to the map if it doesn't already exist
//o--------------------------------------------------------------------------
bool   SubRegion::AddChar( CChar *toAdd )
{
	for( int p = 0; p < charData.size(); p++ )
	{
		if( charData[p] == toAdd )
			return false;
	}
	charData.push_back( toAdd );
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool RemoveItem( CItem *toRemove )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Removes toRemove from the map if it is in there
//o--------------------------------------------------------------------------
bool   SubRegion::RemoveItem( CItem *toRemove )
{
	for( int p = 0; p < itemData.size(); p++ )
	{
		if( itemData[p] == toRemove )
		{
			for( int r = p; r < itemData.size() - 1; r++ )
				itemData[r] = itemData[r+1];
			itemData.resize( itemData.size() - 1 );
			for( int q = 0; q < itemIteratorBackup.size(); q++ )
			{
				if( itemIteratorBackup[q] >= p )
					itemIteratorBackup[q]--;
			}
			return true;
		}
	}
	return false;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool RemoveChar( CChar *toRemove )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Removes toRemove from the map if it is in there
//o--------------------------------------------------------------------------
bool   SubRegion::RemoveChar( CChar *toRemove )
{
	for( int p = 0; p < charData.size(); p++ )
	{
		if( charData[p] == toRemove )
		{
			for( int r = p; r < charData.size() - 1; r++ )
				charData[r] = charData[r+1];
			charData.resize( charData.size() - 1 );
			for( int q = 0; q < charIteratorBackup.size(); q++ )
			{
				if( charIteratorBackup[q] >= p )
					charIteratorBackup[q]--;
			}
			return true;
		}
	}
	return false;
}

void SubRegion::PopItem( void )
{
	itemCounter = itemIteratorBackup.back();
	itemIteratorBackup.pop_back();
}
void SubRegion::PopChar( void )
{
	charCounter = charIteratorBackup.back();
	charIteratorBackup.pop_back();
}
void SubRegion::PushItem( void )
{
	itemIteratorBackup.push_back( itemCounter );
}
void SubRegion::PushChar( void )
{
	charIteratorBackup.push_back( charCounter );
}

void SubRegion::SaveToDisk( ofstream& writeDestination, SI32 mode, ofstream &houseDestination )
{
	// reworked SaveChar from WorldMain to deal with pointer based stuff in region rather than index based stuff in array
	// Also saves out all data regardless (in preparation for a simple binary save)
	if( !writeDestination.is_open() )	// couldn't open our file
		return;

	// Let's start by writing out our characters
	for( CChar *charToWrite = FirstChar(); !FinishedChars(); charToWrite = GetNextChar() )
	{
		charToWrite->Save( writeDestination, mode );
	}
	for( CItem *itemToWrite = FirstItem(); !FinishedItems(); itemToWrite = GetNextItem() )
	{
		if( itemToWrite->GetObjType() == OT_MULTI )
			itemToWrite->Save( houseDestination, mode );
		else if( itemToWrite->GetID() >= 0x4000 )
			SavePretendMulti( itemToWrite, houseDestination, mode );
		else
			itemToWrite->Save( writeDestination, mode );

	}
}

//o--------------------------------------------------------------------------
//|	Function		-	LoadFromDisk( char *filename, SI32 mode )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Loads in items/NPCs from disk in filename
//|					-	If mode = 0 then as text, else as binary
//o--------------------------------------------------------------------------
void SubRegion::LoadFromDisk( ifstream& filename, SI32 mode )
{
	char line[1024];
	do
	{
		readline( filename, line, 1024 );
		if( line[0] == '[' )	// in a section
		{
			if( !strcmp( line, "[CHARACTER]" ) )
				LoadChar( filename, mode );
			else if( !strcmp( line, "[ITEM]" ) )
				LoadItem( filename, mode );
			else if( !strcmp( line, "[HOUSE]" ) )
				LoadMulti( filename, mode );
		}
		else if( !strcmp( line, "---REGION---" ) )	// end of region
			return;
	} while( strcmp( line, "EOF" ) && !filename.eof() );
}

//o--------------------------------------------------------------------------
//|	Function		-	cMapRegion constructor
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	This function is does basically what the name implies
//o--------------------------------------------------------------------------
cMapRegion::cMapRegion()
{
}; //constructor

//o--------------------------------------------------------------------------
//|	Function		-	cMapRegion constructor
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	This function is does basically what the name implies
//o--------------------------------------------------------------------------
cMapRegion::~cMapRegion()
{
	
}; //destructor

//o--------------------------------------------------------------------------
//|	Function		-	SubRegion *GetCell( short x, short y )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the subregion that x,y is in
//o--------------------------------------------------------------------------
SubRegion *cMapRegion::GetCell( short int x, short int y )
{
	UI32 targX = GetGridX( x ), targY = GetGridY( y );
	return &internalRegions[targX][targY];
}

//o--------------------------------------------------------------------------
//|	Function		-	bool AddItem( CItem *nItem )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Adds nItem to the proper SubRegion
//o--------------------------------------------------------------------------
bool cMapRegion::AddItem( CItem *nItem )
{
	int x = GetGridX( nItem->GetX() );
	int y = GetGridY( nItem->GetY() );
	if( x < 0 || y < 0 || x > upperArrayX || y > upperArrayY )
	{
		overFlow.AddItem( nItem );
		return false;
	}
	return internalRegions[x][y].AddItem( nItem );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool RemoveItem( CItem *nItem )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Removes nItem from it's CURRENT SubRegion
//|					-	Do this before adjusting the location
//o--------------------------------------------------------------------------
bool cMapRegion::RemoveItem( CItem *nItem )
{
	int x = GetGridX( nItem->GetX() );
	int y = GetGridY( nItem->GetY() );
	if( x < 0 || y < 0 || x > upperArrayX || y > upperArrayY )
	{
		overFlow.RemoveItem( nItem );
		return false;
	}
	return internalRegions[x][y].RemoveItem( nItem );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool AddChar( CChar *toAdd )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Adds toAdd to the proper SubRegion
//o--------------------------------------------------------------------------
bool cMapRegion::AddChar( CChar *toAdd )
{
	int x = GetGridX( toAdd->GetX() );
	int y = GetGridY( toAdd->GetY() );
	if( x < 0 || y < 0 || x > upperArrayX || y > upperArrayY )
	{
		overFlow.AddChar( toAdd );
		return false;
	}
	return internalRegions[x][y].AddChar( toAdd );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool RemoveChar( CChar *toRemove )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Removes toRemove from it's CURRENT SubRegion
//|					-	Do this before adjusting the location
//o--------------------------------------------------------------------------
bool cMapRegion::RemoveChar( CChar *toRemove )
{
	int x = GetGridX( toRemove->GetX() );
	int y = GetGridY( toRemove->GetY() );
	if( x < 0 || y < 0 || x > upperArrayX || y > upperArrayY )
	{
		overFlow.RemoveChar( toRemove );
		return false;
	}
	return internalRegions[x][y].RemoveChar( toRemove );
}

//o--------------------------------------------------------------------------
//|	Function		-	int GetGridX( short x )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Find the Column of SubRegion we want based on location
//o--------------------------------------------------------------------------
int cMapRegion::GetGridX( short int x )
{
	return x / MapColSize;
}

//o--------------------------------------------------------------------------
//|	Function		-	int GetGridY( short y )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Finds the Row of SubRegion we want based on location
//o--------------------------------------------------------------------------
int cMapRegion::GetGridY( short int y )
{
	return y / MapRowSize;
}

//o--------------------------------------------------------------------------
//|	Function		-	Save()
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Saves out each SubRegion
//o--------------------------------------------------------------------------
void cMapRegion::Save( void )
{
	char filename[256];
	const long AreaX = upperArrayX / 8;	// we're storing 8x8 grid arrays together
	const long AreaY = upperArrayY / 8;

	const char blockDiscriminator[] = "\n\n---REGION---\n\n";

	strcpy( filename, "house.wsc" );
	ofstream houseDestination( filename );

	for( SI32 counter1 = 0; counter1 < AreaX; counter1++ )	// move left->right
	{
		long baseX = counter1 * 8;
		for( SI32 counter2 = 0; counter2 < AreaY; counter2++ )	// move up->down
		{
			long baseY = counter2 * 8;								// calculate x grid offset
			sprintf( filename, "%i.%i.wsc", counter1, counter2 );	// let's name our file
			ofstream writeDestination( filename );					// let's open it 
			for( int xCnt = 0; xCnt < 8; xCnt++ )					// walk through each part of the 8x8 grid, left->right
			{
				long writeLong = baseX + xCnt;						// we know which column we're on
				for( int yCnt = 0; yCnt < 8; yCnt++ )				// walk the row
				{
					internalRegions[writeLong][baseY + yCnt].SaveToDisk( writeDestination, 0, houseDestination );	// save it out as ASCII
					writeDestination.write( blockDiscriminator, strlen( blockDiscriminator ) );
				}
			}
			writeDestination.close();
		}
	}
	houseDestination.close();
	strcpy( filename, "overflow.wsc" );
	houseDestination.open( filename );
	if( houseDestination.is_open() )
		overFlow.SaveToDisk( houseDestination, 0, houseDestination );
}

//o--------------------------------------------------------------------------
//|	Function		-	Load()
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Loads in each SubRegion
//o--------------------------------------------------------------------------
void cMapRegion::Load( void )
{
	char filename[50];
	const long AreaX = upperArrayX / 8;	// we're storing 8x8 grid arrays together
	const long AreaY = upperArrayY / 8;

	for( int counter1 = 0; counter1 < AreaX; counter1++ )	// move left->right
	{
		long baseX = counter1 * 8;
		for( int counter2 = 0; counter2 < AreaY; counter2++ )	// move up->down
		{
			long baseY = counter2 * 8;								// calculate x grid offset
			sprintf( filename, "%i.%i.wsc", counter1, counter2 );	// let's name our file
			ifstream readDestination( filename );					// let's open it 
			if( readDestination.eof() || readDestination.fail() )
			{
				readDestination.close();
				continue;
			}
//			internalRegions[baseX][baseY].LoadFromDisk( readDestination, 0 );
			for( int xCnt = 0; xCnt < 8; xCnt++ )					// walk through each part of the 8x8 grid, left->right
			{
				long writeLong = baseX + xCnt;						// we know which column we're on
				for( int yCnt = 0; yCnt < 8; yCnt++ )				// walk the row
					internalRegions[writeLong][baseY + yCnt].LoadFromDisk( readDestination, 0 );	// load it in as ASCII
			}
			readDestination.close();
		}
	}
	strcpy( filename, "overflow.wsc" );
	ifstream flowDestination( filename );
	overFlow.LoadFromDisk( flowDestination, 0 );
	flowDestination.close();

	strcpy( filename, "house.wsc" );
	ifstream houseDestination( filename );
	LoadHouseMulti( houseDestination, 0 );
	for( int cCounter = 0; cCounter < charcount; cCounter++ )
	{
		if( !chars[cCounter].isFree() )
			chars[cCounter].PostLoadProcessing( cCounter );
	}
	for( int iCounter = 0; iCounter < itemcount; iCounter++ )
	{
		if( !items[iCounter].isFree() )
			items[iCounter].PostLoadProcessing( iCounter );
	}
	houseDestination.close();
}

int cMapRegion::GetGridIndex( short x, short y )
{
	int trgX = GetGridX( x );
	int trgY = GetGridY( y );
	return (trgX + trgY * upperArrayX );	// x offset + y * num cols per row
}

SubRegion *cMapRegion::GetGrid( int gridIndex )
{
	int xOffset;
	int yOffset = (gridIndex / upperArrayX );
	xOffset = gridIndex - ( yOffset * upperArrayX );
	return &internalRegions[xOffset][yOffset];
}

SubRegion *cMapRegion::GetGrid( int xOffset, int yOffset )
{
	if( xOffset < 0 || xOffset > upperArrayX || yOffset < 0 || yOffset > upperArrayY )
		return NULL;
	return &internalRegions[xOffset][yOffset];
}

void cMapRegion::LoadHouseMulti( ifstream &houseDestination, SI32 mode )
{
	char line[1024];
	do
	{
		readline( houseDestination, line, 1024 );
		if( line[0] == '[' )	// in a section
		{
			if( !strcmp( line, "[CHARACTER]" ) )
				LoadChar( houseDestination, mode );
			else if( !strcmp( line, "[ITEM]" ) )
				LoadItem( houseDestination, mode );
			else if( !strcmp( line, "[HOUSE]" ) )
				LoadMulti( houseDestination, mode );
		}
		else if( !strcmp( line, "---REGION---" ) )	// end of region
			return;
	} while( strcmp( line, "EOF" ) && !houseDestination.eof() );
}