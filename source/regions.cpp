#include "uox3.h"

SI16 upperArrayX[UOMT_COUNT];
SI16 upperArrayY[UOMT_COUNT];

void readline( std::ifstream &toRead, char *buffer, int bufferLen )
{
	bool valid = false;
	do 
	{
		toRead.getline( buffer, bufferLen );
#if defined(__unix__)
		trimWindowsText( buffer );
#endif

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

void LoadChar( BinBuffer &buff )
{
	CHARACTER xOff;
	CChar *x = Npcs->MemCharFree( xOff, true );
	if( x == NULL ) 
		return;
	if( !x->Load( buff, xOff ) ) // if no load, DELETE
		Npcs->DeleteChar( x );
}
void LoadItem( BinBuffer &buff )
{
	ITEM xOff;
	CItem *x = Items->MemItemFree( xOff, false );
	if( x == NULL ) 
		return;
	if( !x->Load( buff, xOff ) ) // if no load, DELETE
		Items->DeleItem( x );
}
void LoadMulti( BinBuffer &buff )
{
	ITEM xOff;
	CItem *x = Items->MemItemFree( xOff, false, 1 );
	if( x == NULL ) 
		return;
	CMultiObj *ourHouse = static_cast<CMultiObj *>(x);
	if( !ourHouse->Load( buff, xOff ) )	// if no load, DELETE
		Items->DeleItem( x );
}

void LoadChar( std::ifstream& readDestination )
{
	CHARACTER xOff;
	CChar *x = Npcs->MemCharFree( xOff, true );
	if( x == NULL ) 
		return;
	if( !x->Load( readDestination, xOff ) ) // if no load, DELETE
		Npcs->DeleteChar( x );
}
void LoadItem( std::ifstream& readDestination )
{
	ITEM xOff;
	CItem *x = Items->MemItemFree( xOff, false );
	if( x == NULL ) 
		return;
	if( !x->Load( readDestination, xOff ) ) // if no load, DELETE
		Items->DeleItem( x );
}
void LoadMulti( std::ifstream& readDestination )
{
	ITEM xOff;
	CItem *x = Items->MemItemFree( xOff, false, 1 );
	if( x == NULL ) 
		return;
	CMultiObj *ourHouse = static_cast<CMultiObj *>(x);
	if( !ourHouse->Load( readDestination, xOff ) )	// if no load, DELETE
		Items->DeleItem( x );
}

typedef UI32 vIterator;
//o--------------------------------------------------------------------------
//|	Function		-	SubRegion constructor
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	This function is does basically what the name implies
//o--------------------------------------------------------------------------
SubRegion::SubRegion() : charCounter( INVALIDSERIAL ), itemCounter( INVALIDSERIAL ) //constructor
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
bool SubRegion::FinishedItems( void )
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
bool SubRegion::FinishedChars( void )
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
	for( vIterator p = 0; p < itemData.size(); p++ )
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
bool SubRegion::AddChar( CChar *toAdd )
{
	for( vIterator p = 0; p < charData.size(); p++ )
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
bool SubRegion::RemoveItem( CItem *toRemove )
{
	for( vIterator p = 0; p < itemData.size(); p++ )
	{
		if( itemData[p] == toRemove )
		{
			for( unsigned int r = p; r < itemData.size() - 1; r++ )
				itemData[r] = itemData[r+1];
			itemData.resize( itemData.size() - 1 );
			for( unsigned int q = 0; q < itemIteratorBackup.size(); q++ )
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
bool SubRegion::RemoveChar( CChar *toRemove )
{
	for( vIterator p = 0; p < charData.size(); p++ )
	{
		if( charData[p] == toRemove )
		{
			for( unsigned int r = p; r < charData.size() - 1; r++ )
				charData[r] = charData[r+1];
			charData.resize( charData.size() - 1 );
			for( unsigned int q = 0; q < charIteratorBackup.size(); q++ )
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

void SubRegion::SaveToDisk( std::ofstream& writeDestination, SI32 mode, std::ofstream &houseDestination )
{
	// reworked SaveChar from WorldMain to deal with pointer based stuff in region rather than index based stuff in array
	// Also saves out all data regardless (in preparation for a simple binary save)

	// Let's start by writing out our characters
	for( CChar *charToWrite = FirstChar(); !FinishedChars(); charToWrite = GetNextChar() )
	{
		//if( !charToWrite->IsNpc() && charToWrite->GetAccount() != -1 ) {
    #pragma note( "PlayerHTML Dumping needs to be reimplemented" )
		//DumpPlayerHTML( charToWrite );
    if ( charToWrite->ShouldSave() ) 
		{
			//Console << "Saving char " << charToWrite->GetName() << myendl;
			charToWrite->Save( writeDestination, mode );
		}
	}
	
	for( CItem *itemToWrite = FirstItem(); !FinishedItems(); itemToWrite = GetNextItem() )
	{
		if( itemToWrite->ShouldSave() )
		{
			if( itemToWrite->GetObjType() == OT_MULTI )
			{
				CMultiObj *iMulti = static_cast< CMultiObj * >(itemToWrite);
				iMulti->Save( houseDestination, mode );
			}
			else
				itemToWrite->Save( writeDestination, mode );
		}
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
void SubRegion::LoadFromDisk( std::ifstream& filename, SI32 mode )
{
	if( mode == 1 )//binary
	{
		do 
		{
			BinBuffer buff( filename );
			switch ( buff.GetType() )
			{
			case 1:		LoadChar( buff );	break;
			case 2:		LoadItem( buff );	break;
			case 3:		LoadMulti( buff );	break;
			case 0xFF:	return;//end of region
			default:
				Console << "Error!: Found unknown binary section header " << (UI16)buff.GetType() << myendl;
				break;
			}
		} while( !filename.eof() && !filename.fail() );
	} 
	else 
	{//ascii
		char line[1024];
		do
		{
			readline( filename, line, 1024 );
			if( line[0] == '[' )	// in a section
			{
				if( !strcmp( line, "[CHARACTER]" ) )
					LoadChar( filename );
				else if( !strcmp( line, "[ITEM]" ) )
					LoadItem( filename );
				else if( !strcmp( line, "[HOUSE]" ) )
					LoadMulti( filename );
			}
			else if( !strcmp( line, "---REGION---" ) )	// end of region
				return;
		} while( strcmp( line, "EOF" ) && !filename.eof() );
	}
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
	for( UI08 i = 0; i < (UI08)UOMT_COUNT; i++ )
	{
		upperArrayX[i] = MapWidths[i] / MapColSize;
		upperArrayY[i] = MapHeights[i] / MapRowSize;
	}
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
//|	Function		-	SubRegion *GetCell( SI16 x, SI16 y )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the subregion that x,y is in
//o--------------------------------------------------------------------------
SubRegion *cMapRegion::GetCell( SI16 x, SI16 y, UI08 worldNumber )
{
	UI32 targX = GetGridX( x ), targY = GetGridY( y );
	if( static_cast<SI16>(worldNumber) >= NumberOfWorlds || targX < 0 || targY < 0 || static_cast<SI16>(targX) >= upperArrayX[worldNumber] || static_cast<SI16>(targY) >= upperArrayY[worldNumber] )
		return &overFlow;
	return &internalRegions[targX][targY][worldNumber];
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
	if( nItem == NULL )
		return false;
	UI08 worldNumber = nItem->WorldNumber();
	int x = GetGridX( nItem->GetX() );
	int y = GetGridY( nItem->GetY() );
	if( x < 0 || y < 0 || x >= upperArrayX[worldNumber] || y >= upperArrayY[worldNumber] )
	{
		overFlow.AddItem( nItem );
		return false;
	}
	return internalRegions[x][y][worldNumber].AddItem( nItem );
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
	if( nItem == NULL )
		return false;
	int x = GetGridX( nItem->GetX() );
	int y = GetGridY( nItem->GetY() );
	UI08 worldNumber = nItem->WorldNumber();
	if( x < 0 || y < 0 || x >= upperArrayX[worldNumber] || y >= upperArrayY[worldNumber] )
	{
		overFlow.RemoveItem( nItem );
		return false;
	}
	return internalRegions[x][y][worldNumber].RemoveItem( nItem );
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
	if( toAdd == NULL )
		return false;
	UI08 worldNumber = toAdd->WorldNumber();
	int x = GetGridX( toAdd->GetX() );
	int y = GetGridY( toAdd->GetY() );
	if( x < 0 || y < 0 || x >= upperArrayX[worldNumber] || y >= upperArrayY[worldNumber] )
	{
		overFlow.AddChar( toAdd );
		return false;
	}
	return internalRegions[x][y][worldNumber].AddChar( toAdd );
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
	if( toRemove == NULL )
		return false;
	UI08 worldNumber = toRemove->WorldNumber();
	int x = GetGridX( toRemove->GetX() );
	int y = GetGridY( toRemove->GetY() );
	if( x < 0 || y < 0 || x >= upperArrayX[worldNumber] || y >= upperArrayY[worldNumber] )
	{
		overFlow.RemoveChar( toRemove );
		return false;
	}
	return internalRegions[x][y][worldNumber].RemoveChar( toRemove );
}

//o--------------------------------------------------------------------------
//|	Function		-	int GetGridX( SI16 x )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Find the Column of SubRegion we want based on location
//o--------------------------------------------------------------------------
int cMapRegion::GetGridX( SI16 x )
{
	return x / MapColSize;
}

//o--------------------------------------------------------------------------
//|	Function		-	int GetGridY( SI16 y )
//|	Date			-	23 July, 2000
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Finds the Row of SubRegion we want based on location
//o--------------------------------------------------------------------------
int cMapRegion::GetGridY( SI16 y )
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
	char filename[MAX_PATH];
	const long AreaX = UpperX / 8;	// we're storing 8x8 grid arrays together
	const long AreaY = UpperY / 8;
	std::ofstream writeDestination, houseDestination;
	const int onePercent = (int)((float)(AreaX*AreaY*8*8*NumberOfWorlds)/100.0f);
	const char blockDiscriminator[] = "\n\n---REGION---\n\n";
	const char binBlockDisc = (char)0xFF;
	int count = 0;

	const UI08 Version = 3;

	int Mode = cwmWorldState->ServerData()->SaveMode();

	int s_t = getclock();

	Console << "Saving Character and Item Map Region data...   ";
	Console.TurnYellow();
	Console << "0%";

	sprintf( filename, "%shouse.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );
	
	if( Mode == 1 ) 
	{
		houseDestination.open( filename, std::ios::binary|std::ios::out );

		sprintf( filename, "%stables.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );
		writeDestination.open( filename, std::ios::out|std::ios::binary );
		writeDestination.write( "B", 1 );
		writeDestination.write( (const char *)&Version, 1 );
		writeDestination.write( (const char *)CharSaveTable, 256 );
		writeDestination.write( (const char *)ItemSaveTable, 256 );
		writeDestination.close();
	} 
	else 
		houseDestination.open( filename );

	for( SI32 counter1 = 0; counter1 < AreaX; counter1++ )	// move left->right
	{
		long baseX = counter1 * 8;
		for( SI32 counter2 = 0; counter2 < AreaY; counter2++ )	// move up->down
		{
			long baseY = counter2 * 8;								// calculate x grid offset

			sprintf( filename, "%s%i.%i.wsc", cwmWorldState->ServerData()->GetSharedDirectory(), counter1, counter2 );	// let's name our file
			if( Mode == 1 )
				writeDestination.open( filename, std::ios::out|std::ios::binary );
			else
				writeDestination.open( filename );

			if( !writeDestination ) 
			{
				Console.Error( 1, "Failed to open %s for writing", filename );
				continue;
			}

			for( UI08 xCnt = 0; xCnt < 8; xCnt++ )					// walk through each part of the 8x8 grid, left->right
			{
				for( UI08 yCnt = 0; yCnt < 8; yCnt++ )				// walk the row
				{
					for( UI08 wCnt = 0; wCnt < NumberOfWorlds; wCnt++ )
					{
						count++;
						if( count%onePercent == 0 )
						{
							if( count/onePercent <= 10 )
								Console << "\b\b" << (UI32)(count/onePercent) << "%";
							else if( count/onePercent <= 100 )
								Console << "\b\b\b" << (UI32)(count/onePercent) << "%";
						}
						internalRegions[baseX + xCnt][baseY + yCnt][wCnt].SaveToDisk( writeDestination, Mode, houseDestination );

						if( Mode == 1 )
							writeDestination.write( &binBlockDisc, 1 );
						else
							writeDestination << blockDiscriminator;//.write( blockDiscriminator, strlen( blockDiscriminator ) );
					}
				}
			}
			writeDestination.close();
		}
	}
	houseDestination.close();

	sprintf( filename, "%soverflow.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );
	if( Mode == 1 )
		writeDestination.open( filename, std::ios::out|std::ios::binary );
	else
		writeDestination.open( filename );

	if( writeDestination.is_open() )
	{
		overFlow.SaveToDisk( writeDestination, Mode, writeDestination );
		writeDestination.close();
	}
	else
	{
		Console.Error( 1, "Failed to open %s for writing", filename );
		return;
	}
	
	Console << "\b\b\b\b";
	Console.PrintDone();

	int e_t = getclock();
	Console.Print( "%s world saved in %.02fsec\n", ( Mode == 1 ? "BINARY" : "ASCII" ), ((float)(e_t-s_t))/1000.0f );
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
	char filename[MAX_PATH];
	const long AreaX = UpperX / 8;	// we're storing 8x8 grid arrays together
	const long AreaY = UpperY / 8;
	const int onePercent = (int)((float)(AreaX*AreaY*8*8*NumberOfWorlds)/100.0f);
	int count = 0;
	std::ifstream readDestination;
	UI08 Version = 0;
	UI08 test;

	Console.TurnYellow();
	Console << "0%";
	int mode = 0;//ascii

	int s_t = getclock();

	sprintf( filename, "%stables.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );	// let's name our file
	readDestination.open( filename, std::ios::in|std::ios::binary );//open first file and check for binary
	if( readDestination.is_open() )
	{
		readDestination.read( (char *)&test, 1 );//read the 'B' to make sure we are binary
		if( test == 'B' )
			mode = 1;
		readDestination.read( (char*)&Version, 1 );//read out little version number
		readDestination.close();
		sprintf( filename, "%s0.0.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );	// let's name our file
		
		readDestination.open( filename, std::ios::in|std::ios::binary );
		if( readDestination.is_open() )
		{
			readDestination.read( (char *)&test, 1 );
			if( test > 3 && test != 255 )
			{//only 0,1,2,3, and 255 are values that should ever appear as the first byte in this file. in binary mode
				Console.Error( 1, "First byte of 0.0.wsc was %i [%c].  If this is an ASCII world, please remove tables.wsc.", test, test );
				Console << "";
				Console.TurnYellow();
				Console << "WARNING: Continuing to load world as BINARY." << myendl;
				Console.TurnNormal();
			}
			readDestination.close();
		}
	}

	for( int counter1 = 0; counter1 < AreaX; counter1++ )	// move left->right
	{
		long baseX = counter1 * 8;
		for( int counter2 = 0; counter2 < AreaY; counter2++ )	// move up->down
		{
			long baseY = counter2 * 8;								// calculate x grid offset
			sprintf( filename, "%s%i.%i.wsc", cwmWorldState->ServerData()->GetSharedDirectory(),  counter1, counter2 );	// let's name our file
			if( mode == 1 )
				readDestination.open( filename, std::ios::in|std::ios::binary );
			else
				readDestination.open( filename );					// let's open it 

			if( readDestination.eof() || readDestination.fail() )
			{
				readDestination.close();
				readDestination.clear();
				continue;
			}
			for( UI08 xCnt = 0; xCnt < 8; xCnt++ )					// walk through each part of the 8x8 grid, left->right
			{
				long writeLong = baseX + xCnt;						// we know which column we're on
				for( UI08 yCnt = 0; yCnt < 8; yCnt++ )				// walk the row
				{
					for( UI08 wCnt = 0; wCnt < NumberOfWorlds; wCnt++ )
					{
						count++;
						if( count%onePercent == 0 )
						{
							if( count/onePercent <= 10 )
								Console << "\b\b" << (UI32)(count/onePercent) << "%";
							else if( count/onePercent <= 100 )
								Console << "\b\b\b" << (UI32)(count/onePercent) << "%";
						}
						internalRegions[writeLong][baseY + yCnt][wCnt].LoadFromDisk( readDestination, mode );	// load it in as ASCII
					}
				}
			}
			readDestination.close();
		}
	}

	Console.TurnNormal();
	Console << "\b\b\b";
	Console.PrintDone();

	sprintf( filename, "%soverflow.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );
	std::ifstream flowDestination( filename );
	overFlow.LoadFromDisk( flowDestination, mode );
	flowDestination.close();

	sprintf( filename, "%shouse.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );
	std::ifstream houseDestination( filename );
	LoadHouseMulti( houseDestination, mode );
	for( UI32 cCounter = 0; cCounter < charcount; cCounter++ )
	{
		if( !chars[cCounter].isFree() )
			chars[cCounter].PostLoadProcessing( cCounter );
	}
	for( UI32 iCounter = 0; iCounter < itemcount; iCounter++ )
	{
		if( !items[iCounter].isFree() )
			items[iCounter].PostLoadProcessing( iCounter );
	}
	houseDestination.close();

	int e_t = getclock();
	Console.Print( "%s world loaded in %.02fsec\n", ( mode == 1 ? "BINARY" : "ASCII" ), ((float)(e_t-s_t))/1000.0f );
	//Console << (mode == 1 ? "BINARY" : "ASCII") << " World loaded in " << ((SI32)(e_t-s_t)) << "ms" << myendl;
}

int cMapRegion::GetGridIndex( SI16 x, SI16 y )
{
	int trgX = GetGridX( x );
	int trgY = GetGridY( y );
	return( trgX + trgY * UpperX );	// x offset + y * num cols per row
}

SubRegion *cMapRegion::GetGrid( int xOffset, int yOffset, UI08 worldNumber )
{
	if( xOffset < 0 || xOffset >= upperArrayX[worldNumber] || yOffset < 0 || yOffset >= upperArrayY[worldNumber] )
		return &overFlow;
	return &internalRegions[xOffset][yOffset][worldNumber];
}

void cMapRegion::LoadHouseMulti( std::ifstream &houseDestination, SI32 mode )
{
	if( mode == 1 )//binary
	{
		do 
		{
			BinBuffer buff( houseDestination );
			switch ( buff.GetType() )
			{
			case 1:		LoadChar( buff );	break;
			case 2:		LoadItem( buff );	break;
			case 3:		LoadMulti( buff );	break;
			case 0xFF:	return;//end of region
			default:
				Console.Error( 1, "Found unknown binary section header %i", (UI16)buff.GetType() );
				break;
			}
		} while( !houseDestination.eof() && !houseDestination.fail() );
	} 
	else 
	{//ascii
		char line[1024];
		do 
		{
			readline( houseDestination, line, 1024 );
			if( line[0] == '[' )	// in a section
			{
				if( !strcmp( line, "[CHARACTER]" ) )
					LoadChar( houseDestination );
				else if( !strcmp( line, "[ITEM]" ) )
					LoadItem( houseDestination );
				else if( !strcmp( line, "[HOUSE]" ) )
					LoadMulti( houseDestination );
			}
			else if( !strcmp( line, "---REGION---" ) )	// end of region
				return;
		} while( strcmp( line, "EOF" ) && !houseDestination.eof() );
	}
}

bool cMapRegion::Add( cBaseObject *toAdd )
{
	if( toAdd == NULL )
		return false;
	if( toAdd->GetObjType() == OT_CHAR )
		return AddChar( static_cast< CChar *>(toAdd) );
	else
		return AddItem( (CItem *)(toAdd) );
}
bool cMapRegion::Remove( cBaseObject *toRemove )
{
	if( toRemove == NULL )
		return false;
	if( toRemove->GetObjType() == OT_CHAR )
		return RemoveChar( static_cast< CChar *>(toRemove) );
	else
		return RemoveItem( (CItem *)(toRemove) );
}
