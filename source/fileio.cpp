
#include "fileio.h"

const UI16 IOBUFFLEN = 2048;

UOXFile::UOXFile( const char *fileName, char *mode )
{
	char  localMode[16];
	fmode = -1, ok = false;

	ioBuff = new UI08[IOBUFFLEN];

	memset( ioBuff, 0x00, sizeof( UI08 ) * IOBUFFLEN );

	if( ioBuff != NULL )
	{
		strcpy( localMode, mode );

		if( *mode == 'r' ) 
			fmode = 0;
		else if( *mode == 'w' ) 
			fmode = 1;

		theFile = fopen( fileName, localMode );

		if( theFile == NULL ) 
		{ 
			ok = false; 
			return; 
		}
		else
		{
			bSize = bIndex = IOBUFFLEN;
			ok = true;
		}
	}
}

UOXFile::~UOXFile()
{
	if( ioBuff != NULL ) 
		delete[] ioBuff;
	if( theFile ) 
		fclose( theFile );
}

void UOXFile::rewind( void )
{
	fseek( theFile, 0, SEEK_SET );
	bSize = bIndex = IOBUFFLEN;
}

void UOXFile::seek( long offset, int whence )
{
	if( whence == SEEK_SET || whence == SEEK_CUR || whence == SEEK_END )
	{
		fseek( theFile, offset, whence );
		bSize = bIndex = IOBUFFLEN;
	}
}

int UOXFile::getch(void)
{
	if( qRefill() ) 
		refill();

	if( bSize != 0 ) 
		return ioBuff[(int)bIndex++];
	else 
		return -1;
}

void UOXFile::refill( void )
{
	bSize = fread( ioBuff, sizeof(char), IOBUFFLEN, theFile );
	bIndex = 0;
}

char *UOXFile::gets( char *lnBuff, int lnSize )
{
	if( fmode == 0 )
	{
		int i;

		lnSize--;
		i = 0;
		do
		{
			if( qRefill() ) 
				refill();

			while( i < lnSize && bIndex != bSize && ioBuff[(int)bIndex] != 0x0A )
			{
				if( ioBuff[(int)bIndex] == 0x0D ) 
					bIndex++;
				else 
					lnBuff[i++] = ioBuff[(int)bIndex++];
			}

			if( ioBuff[(int)bIndex] == 0x0A )
			{
				bIndex++;
				if( i != lnSize ) 
					lnBuff[i++] = 0x0A;
				break;
			}

			if( i == lnSize ) 
			{ 
				break; 
			}
		} while( bSize != 0 );

		lnBuff[i] = '\0';

		if( bSize != IOBUFFLEN && i == 0 ) 
			return NULL;

		return lnBuff;
	}
	else 
		return NULL;
}

int UOXFile::puts( char *lnBuff )
{
	if( fmode == 1 && lnBuff )
		return fwrite( lnBuff, sizeof( char ), strlen( lnBuff ), theFile );

	return -1;
}

void UOXFile::getUChar( UI08 *buff, UI32 number )
{
	for( UI32 i = 0; i < number; i++ )
		buff[i] = this->getch();
}

void UOXFile::getChar( SI08 *buff, UI32 number )
{
	for( UI32 i = 0; i < number; i++ )
		buff[i] = this->getch();
}

void UOXFile::getUShort( UI16 *buff, UI32 number )
{
	for( UI32 i = 0; i < number; i++ )
	{
		buff[i] = this->getch();
		buff[i] |= this->getch() << 8;
	}
}

void UOXFile::getShort( SI16 *buff, UI32 number )
{
	for( UI32 i = 0; i < number; i++ )
	{
		buff[i] = this->getch();
		buff[i] |= this->getch() << 8;
	}
}

void UOXFile::getULong( UI32 *buff, UI32 number )
{
	for( UI32 i = 0; i < number; i++ )
	{
		buff[i] = this->getch();
		buff[i] |= this->getch() << 8;
		buff[i] |= this->getch() << 16;
		buff[i] |= this->getch() << 24;
	}
}

void UOXFile::getLong( SI32 *buff, UI32 number )
{
	for( UI32 i = 0; i < number; i++ )
	{
		buff[i] = this->getch();
		buff[i] |= this->getch() << 8;
		buff[i] |= this->getch() << 16;
		buff[i] |= this->getch() << 24;
	}
}
int UOXFile::getLength( void )
{
	long currentPos = ftell( theFile );
	fseek( theFile, 0L, SEEK_END );
	long pos = ftell( theFile );
	fseek( theFile, currentPos, SEEK_SET );
	return pos;
}


void UOXFile::get_versionrecord( struct versionrecord *buff, UI32 number )
{
	for( UI32 i = 0; i < number; i++ )
	{
		getLong( &buff[i].file );
		getLong( &buff[i].block );
		getLong( &buff[i].filepos );
		getLong( &buff[i].length );
		getLong( &buff[i].unknown );
	}
}
/*
** More info from Alazane & Circonian on this...
**
Index entry:
DWORD: File ID (see index below)
DWORD: Block (Item number, Gump number or whatever; like in the file)
DWORD: Position (Where to find this block in verdata.mul)
DWORD: Size (Size in Byte)
DWORD: Unknown (Perhaps some CRC for the block, most blocks in UO files got this) 

    File IDs: (* means used in current verdata)
00 - map0.mul
01 - staidx0.mul
02 - statics0.mul
03 - artidx.mul
04 - art.mul*
05 - anim.idx
06 - anim.mul
07 - soundidx.mul
08 - sound.mul
09 - texidx.mul
0A - texmaps.mul
0B - gumpidx.mul
0C - gumpart.mul*
0D - multi.i
*/

void UOXFile::get_st_multi( struct st_multi *buff, UI32 number )
{
	for( UI32 i = 0; i < number; i++ )
	{
		getUShort( &buff[i].tile );
		getShort( &buff[i].x );
		getShort( &buff[i].y );
		getChar( (SI08 *) &buff[i].z );
		getChar( (SI08 *) &buff[i].empty );
		getLong( &buff[i].visible );
	}
}

void UOXFile::get_land_st( CLand *buff, UI32 number)
{
	for( UI32 i = 0; i < number; i++ )
		buff[i].Read( this );
}

void UOXFile::get_tile_st( CTile *buff, UI32 number)
{
	for( UI32 i = 0; i < number; i++ )
		buff[i].Read( this );
}

void UOXFile::get_map_st(struct map_st *buff, UI32 number)
{
	for( UI32 i = 0; i < number; i++)
	{
		getUShort(&buff[i].id);
		getChar(&buff[i].z);
	}
}

void UOXFile::get_st_multiidx(struct st_multiidx *buff, UI32 number )
{
	for( UI32 i = 0; i < number; i++)
	{
		getLong(&buff[i].start);
		getLong(&buff[i].length);
		getLong(&buff[i].unknown);
	}
}

void UOXFile::get_staticrecord( struct staticrecord *buff, UI32 number )
{
	for( UI32 i = 0; i < number; i++)
	{
		getUShort( &buff[i].itemid );
		getUChar( &buff[i].xoff );
		getUChar( &buff[i].yoff );
		getChar( &buff[i].zoff );
		SI16 extra;
		getShort( &extra );
	}
}

r2Data readline( ifstream& toRead ) // Read line from script
{
	char lineBuf[512], ret1[512], ret2[512];
	toRead.getline( lineBuf, 512 );
	int i = 0;

	while( lineBuf[i] != 0 && lineBuf[i] != ' ' && lineBuf[i] != '=' )
		i++;
	strncpy( ret1, lineBuf, i );
	ret1[i] = 0;
	if( ret1[0] != '}' && lineBuf[i] != 0 ) 
		strcpy( ret2, lineBuf + i + 1 );
	return r2Data( ret1, ret2 );
}

void savelog( const char *msg, const char *logfile )
{
	if( msg == NULL || logfile == NULL )
		return;
	if( cwmWorldState->ServerData()->GetServerConsoleLogStatus() )
	{
		char realLogSpot[MAX_PATH];
		sprintf( realLogSpot, "%s/logs/%s", cwmWorldState->ServerData()->GetRootDirectory(), logfile );
		char time_str[256];
		FILE *file = fopen( realLogSpot, "a" );
		if( file != NULL )
		{
			fprintf( file, "[%s] %s", RealTime( time_str ), msg );
			fclose( file );
		}
		else
		{
			Console.Error( 1, "Failed to open log file %s", realLogSpot );
		}
	}
}

// Load character and item data from load chars.wsc and items.wsc
void loadnewworld( void )
{
	cmem = 0;
	imem = 0;
	//Console.PrintSectionBegin();
	//Console << "Loading the world..." << myendl;// World and Guilds, Building Map Regions..." << myendl;
	charcount = 0;
	itemcount = 0;
	charcount2 = 1;
	itemcount2 = 0x40000000;
	Reticulate();
	MapRegion->Load();

	Console << "Loading Guilds                 ";
	GuildSys->Load();
	Console.PrintDone();
}

void ReadWorldTagData( ifstream &inStream, char *tag, char *data )
{
	char temp[4096];
	tag[0] = 0;
	data[0] = 0;
	bool bValid = false;
	while( !bValid )
	{
		if( inStream.eof() || inStream.fail() )
		{
			strcpy( tag, "o---o" );
			strcpy( data, "o---o" );
			return;
		}
		inStream.getline( temp, 4096 );
#ifdef __LINUX__
		trimWindowsText( temp );
#endif
		if( temp[0] == 'o' && !strcmp( temp, "o---o" ) )
		{
			strcpy( tag, "o---o" );
			return;
		}
		if( temp[0] == '/' || temp[0] == 13 || temp[0] == 10 )	// invalid
			continue;
		char *equalOffset = strstr( temp, "=" );
		if( equalOffset == NULL )	// no pairing
			continue;
		strncpy( tag, temp, equalOffset - temp );
		tag[equalOffset - temp] = 0;
		strcpy( data, equalOffset + 1 );
		bValid = true;
	}
}

bool FileExists( const char *filepath )
{
	ifstream ifsFile;
	ifsFile.open( filepath, ios::in );
	if( ifsFile.is_open() )
	{
		ifsFile.close();
		return true;
	}
	return false;
}

BinBuffer::BinBuffer()
{
	Reset();
}

BinBuffer::BinBuffer( ifstream &in )
{
	Read( in );//resets inside Read()
}

BinBuffer::~BinBuffer()
{
}

void BinBuffer::Read( ifstream &inFile )
{
	UI16 Size;
	char TempBuff[2];
	Reset();
	
	inFile.read( (char *)&myType, 1 );
	if( myType == 0xFF || inFile.eof() || inFile.fail() )
	{
		myType = 0xFF;
		return;
	}

	inFile.read( TempBuff, 2 );
	Size = ((UI08)(TempBuff[0])) | ((UI16)(((UI08)TempBuff[1]) << 8));
	Buff.resize( Size );
	inFile.read( &Buff[0], Size );
}

void BinBuffer::Write( ofstream &outFile )
{
	char TempBuff[2];
	UI16 Size = (UI16)Buff.size();
	TempBuff[0] = (char)(Size%256);
	TempBuff[1] = (char)(Size>>8);

	outFile.write( (char *)&myType, 1 );
	outFile.write( TempBuff, 2 );
	outFile.write( &Buff[0], Size );
}

void BinBuffer::Reset( void )
{
	Buff.clear();
	Buff.reserve( 500 );
	fp = 0;
	myType = 0;
}

UI32 BinBuffer::Size( void )
{
	return Buff.size();
}

UI08 BinBuffer::GetType( void )
{
	return myType;
}

void BinBuffer::SetType( UI08 newType )
{
	myType = newType;
}

UI08 BinBuffer::GetByte( void )
{
	if( fp < Buff.size() )
		return (UI08)Buff[fp++];
	else
		return 0;
}

void BinBuffer::PutByte( UI08 put )
{
	Buff.push_back( (char)put );
}

SI16 BinBuffer::GetShort( void )
{
	SI16 Ret = GetByte();
	Ret |= GetByte()<<8;
	return Ret;
}
	
void BinBuffer::PutShort( SI16 put )
{
	Buff.push_back( (char)(put%256) );
	Buff.push_back( (char)(put>>8) );
}

SI32 BinBuffer::GetLong( void )
{
	SI32 Ret = GetByte();
	Ret |= GetByte()<<8;
	Ret |= GetByte()<<16;
	Ret |= GetByte()<<24;
	return Ret;
}

void BinBuffer::PutLong( SI32 put )
{
	Buff.push_back( (char)(put%256) );
	Buff.push_back( (char)(put>>8) );
	Buff.push_back( (char)(put>>16) );
	Buff.push_back( (char)(put>>24) );
}

R32 BinBuffer::GetFloat( void )
{
	R32 Ret = 0;
	SI32 read = GetLong();
	memcpy( &Ret, &read, 4 );
	return Ret;
}

void BinBuffer::PutFloat( R32 put )
{
	SI32 write = 0;
	memcpy( &write, &put, 4 );
	PutLong( write );
}

void BinBuffer::PutStr( const char *szOut )
{
	int len = strlen(szOut);

	PutByte( len );
	int bs = Buff.size();

	Buff.resize( bs+len );
	memcpy( &Buff[bs], szOut, len );
}

int BinBuffer::GetStr( char *str, int MaxLen )
{
	int len = GetByte();

	if( len > MaxLen )//don't overwrite our outstring
	{
		memcpy( str, &Buff[fp], MaxLen-1 );
		str[MaxLen-1] = 0;
	} 
	else 
	{
		memcpy( str, &Buff[fp], len );
		str[len] = 0;
	}
	fp += len;

	return len;
}

void BinBuffer::Put( const void *put, int len )
{
	int b = Buff.size();
	Buff.resize( b+len );
	memcpy( &Buff[b], put, len );
}

void BinBuffer::Get( void *get, int len )
{
	int gl = Buff.size() - min(Buff.size(), (UI32)(fp+len));
	memcpy( get, &Buff[fp], gl );
	fp += len;
}

bool BinBuffer::End( void )
{
	return ( fp >= Buff.size() ? true : false );
}

int BinBuffer::Position( void )
{
	return fp;
}

int BinBuffer::Length( void )
{
	return Buff.size();
}

