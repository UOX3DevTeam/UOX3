#pragma warning( disable : 4786 )
#include "uox3.h"
#include "mapclasses.h"
#include "cGuild.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "cSpawnRegion.h"
#include "scriptc.h"
#include "townregion.h"

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

//o---------------------------------------------------------------------------o
//|   Function    :  void cFileIO::LoadINIFile( void )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Loads uox3.ini
//o---------------------------------------------------------------------------o
void cFileIO::LoadINIFile( void )
{
	// hmm, fileExists isn't a valid func... let's call our
	bool uox3test = FileExists( "uox3test.ini" );
	bool uox3     = FileExists( "uox3.ini" );
	bool uox      = FileExists( "uox.ini" );
	if( !uox )
	{
		if( uox3 ) 
		{
			Console << "NOTICE: uox3.ini is no longer needed." << myendl;
			Console << "Rewriting as uox.ini." << myendl;
			cwmWorldState->ServerData()->load( "uox3.ini" );//load this anyway, in case they don't have the other one.
			cwmWorldState->ServerData()->save();
		}
		else if( uox3test ) 
		{
			Console << "NOTICE: uox3test.ini is no longer needed." << myendl;
			Console << "Rewriting as uox.ini." << myendl;
			cwmWorldState->ServerData()->load( "uox3test.ini" );//load this anyway, in case they don't have the other one.
			cwmWorldState->ServerData()->save();
		}
	}
	else if( uox3 || uox3test )
	{
		Console << "You have both old style (uox3.ini and/or uox3test.ini) and new style (uox.ini) files." << myendl;
		Console << "We will only be reading the uox.ini file" << myendl;
	}
	cwmWorldState->ServerData()->load();
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cFileIO::LoadNewWorld( void )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Load character and item data from load chars.wsc and items.wsc
//o---------------------------------------------------------------------------o
void cFileIO::LoadNewWorld( void )
{
	cwmWorldState->SetCMem( 0 );
	cwmWorldState->SetIMem( 0 );
	//Console.PrintSectionBegin();
	//Console << "Loading the world..." << myendl;// World and Guilds, Building Map Regions..." << myendl;
	cwmWorldState->SetCharCount( 0 );
	cwmWorldState->SetItemCount( 0 );
	cwmWorldState->SetItemCount2( 1 );
	cwmWorldState->SetItemCount2( BASEITEMSERIAL );
	//Reticulate();
	MapRegion->Load();

	Console << "Loading Guilds                 ";
	GuildSys->Load();
	Console.PrintDone();
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cFileIO::LoadCustomTitle( void )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Loads players titles (Karma, Fame, Murder, ect)
//o---------------------------------------------------------------------------o
void cFileIO::LoadCustomTitle( void )
{ 
	int titlecount = 0;
	char sect[512]; 
	
	strcpy( sect, "SKILL" );
	ScriptSection *CustomTitle = FileLookup->FindEntry( sect, titles_def );
	if( CustomTitle == NULL)
		return;
	const char *tag = NULL;
	const char *data = NULL;
	char tempSkill[256];
	for( tag = CustomTitle->First(); !CustomTitle->AtEnd(); tag = CustomTitle->Next() )
	{
		data = CustomTitle->GrabData();
		sprintf( tempSkill, "%s %s", tag, data );
		strncpy( cwmWorldState->title[titlecount++].skill, tempSkill, MAX_TITLE );
	}
	strcpy( sect, "PROWESS" );
	CustomTitle = FileLookup->FindEntry( sect, titles_def );
	if( CustomTitle == NULL)
		return;
	titlecount = 0;
	for( tag = CustomTitle->First(); !CustomTitle->AtEnd(); tag = CustomTitle->Next() )
		strncpy( cwmWorldState->title[titlecount++].prowess, tag, MAX_TITLE );

	strcpy( sect, "FAME" );
	CustomTitle = FileLookup->FindEntry( sect, titles_def );
	titlecount = 0;

	for( tag = CustomTitle->First(); !CustomTitle->AtEnd(); tag = CustomTitle->Next() )
	{
		strncpy( cwmWorldState->title[titlecount].fame, tag, MAX_FAMETITLE );
		if( titlecount == 23 )
		{
			cwmWorldState->title[titlecount].fame[0] = 0;
			strncpy( cwmWorldState->title[++titlecount].fame, tag, MAX_FAMETITLE );
		}
		titlecount++;
	}

	// Murder tags now scriptable in SECTION MURDER - Titles.scp - Thanks Ab - Zane
	CustomTitle = FileLookup->FindEntry( "MURDERER", titles_def );
	if( CustomTitle == NULL )	// couldn't find it
		return;
	for( tag = CustomTitle->First(); !CustomTitle->AtEnd(); tag = CustomTitle->Next() )
	{
		data = CustomTitle->GrabData();
		cwmWorldState->murdererTags.push_back( MurderPair( (SI16)makeNum( tag ), data ) );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cFileIO::LoadSkills( void )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Load skills
//o---------------------------------------------------------------------------o
void cFileIO::LoadSkills( void )
{
	char sect[512];
	ScriptSection *SkillList = NULL;
	const char *tag = NULL;
	const char *data = NULL;
	for( UI08 i = 0; i <= ALLSKILLS + 3; i++)
	{
		cwmWorldState->skill[i].strength = 0;
		cwmWorldState->skill[i].dexterity = 0;
		cwmWorldState->skill[i].intelligence = 0;
		sprintf( sect, "SKILL %i", i );
		SkillList = FileLookup->FindEntry( sect, skills_def );
		if( SkillList != NULL )
		{
			for( tag = SkillList->First(); !SkillList->AtEnd(); tag = SkillList->Next() )
			{
				data = SkillList->GrabData();
				if( !strcmp( "STR", tag ) )
					cwmWorldState->skill[i].strength = (UI16)makeNum( data );
				else if( !strcmp( "DEX", tag ) )
					cwmWorldState->skill[i].dexterity = (UI16)makeNum( data );
				else if( !strcmp( "INT", tag ) )
					cwmWorldState->skill[i].intelligence = (UI16)makeNum( data );
				else if( !strcmp( "SKILLPOINT", tag ) )
				{
					advance_st tempAdvance;
					char temp[256];
					gettokennum( data, 0, temp );
					tempAdvance.base = (UI16)makeNum( temp );
					gettokennum( data, 1, temp );
					tempAdvance.success = (UI16)makeNum( temp );
					gettokennum( data, 2, temp );
					tempAdvance.failure = (UI16)makeNum( temp );
					cwmWorldState->skill[i].advancement.push_back( tempAdvance );
				}
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cFileIO::LoadPreDefSpawnRegion( UI16 r, std::string name )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Load scripted spawn regions
//o---------------------------------------------------------------------------o
void cFileIO::LoadPreDefSpawnRegion( UI16 r, std::string name )
{
	char sect[512];
	sprintf( sect, "PREDEFINED_SPAWN %s", name.c_str() );
	ScriptSection *predefSpawn = FileLookup->FindEntry( sect, spawn_def );
	if( predefSpawn == NULL )
	{
		Console << "WARNING: Undefined region spawn " << name << ", check your regions.scp and spawn.scp files" << myendl;
		return;
	}

	spawnregion[r]->Load( predefSpawn );
	Console << sect << " loaded into spawn region #" << r << myendl;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cFileIO::LoadSpawnRegions( void )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Loads spawning regions
//o---------------------------------------------------------------------------o
void cFileIO::LoadSpawnRegions( void )
{
	UI16 i = 0;

	spawnregion[i] = new cSpawnRegion( i );
	
	i++;
	cwmWorldState->SetTotalSpawnRegions( 0 );
	
	ScriptSection *toScan = NULL;
	VECSCRIPTLIST *tScn = FileLookup->GetFiles( spawn_def );
	if( tScn == NULL )
		return;
	for( UI32 iCtr = 0; iCtr < tScn->size(); iCtr++ )
	{
		Script *spnScp = (*tScn)[iCtr];
		if( spnScp == NULL )
			continue;
		for( toScan = spnScp->FirstEntry(); toScan != NULL; toScan = spnScp->NextEntry() )
		{
			if( toScan == NULL )
				continue;
			if( !strncmp( "REGIONSPAWN", spnScp->EntryName(), 11 ) ) // Is it a region spawn entry?
			{
				spawnregion[i] = new cSpawnRegion( i );
				spawnregion[i]->Load( toScan );
				i++;
			}
		}
	}
	cwmWorldState->SetTotalSpawnRegions( i );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cFileIO::LoadRegions( void )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Load regions from regions.wsc
//o---------------------------------------------------------------------------o
void cFileIO::LoadRegions( void )
{
	int l = 0;
	const char *tag = NULL;
	const char *data = NULL;
	bool performLoad = false;
	Script *ourRegions = NULL;
	char regionsFile[MAX_PATH];
	sprintf(regionsFile, "%s%s", cwmWorldState->ServerData()->GetSharedDirectory(), "regions.wsc");
	FILE *ourReg = fopen( regionsFile, "r" );
	if( ourReg != NULL )
	{
		performLoad = true;
		fclose( ourReg );
		ourRegions = new Script( regionsFile, NUM_DEFS, false );
	}
	for( UI16 i = 0; i < 256; i++ )
	{
		region[i] = new cTownRegion( (UI08)i );
		region[i]->InitFromScript( l );
		if( performLoad )
			region[i]->Load( ourRegions );
	}
	if( performLoad )
	{
		delete ourRegions;
		ourRegions = NULL;
	}
	cwmWorldState->SetLocationCount( (UI16)l );
	cwmWorldState->SetLogoutCount( 0 );	//Instalog
	ScriptSection *InstaLog = FileLookup->FindEntry( "INSTALOG", regions_def );
	if( InstaLog == NULL ) 
		return;
	for( tag = InstaLog->First(); !InstaLog->AtEnd(); tag = InstaLog->Next() )
	{
		data = InstaLog->GrabData();
		if( !strcmp( tag,"X1" ) ) 
			cwmWorldState->logout[cwmWorldState->GetLogoutCount()].x1 = (SI16)makeNum( data );
		else if( !strcmp( tag, "Y1" ) ) 
			cwmWorldState->logout[cwmWorldState->GetLogoutCount()].y1 = (SI16)makeNum( data );
		else if( !strcmp( tag, "X2" ) ) 
			cwmWorldState->logout[cwmWorldState->GetLogoutCount()].x2 = (SI16)makeNum( data );
		else if( !strcmp( tag, "Y2" ) )
		{
			cwmWorldState->logout[cwmWorldState->GetLogoutCount()].y2 = (SI16)makeNum( data );
			cwmWorldState->IncLogoutCount();
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cFileIO::LoadTeleportLocations( void )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Load teleport locations
//o---------------------------------------------------------------------------o
void cFileIO::LoadTeleportLocations( void )
{
	char text[256];
	int i;
	char seps[]   = " ,\t\n";
	char *token;
	char filename[MAX_PATH];
	sprintf( filename, "%steleport.scp", cwmWorldState->ServerData()->GetScriptsDirectory() ); 
	FILE *fp = fopen( filename, "r" );
	cwmWorldState->teleLocs.resize( 0 );
	
	if( fp == NULL )
	{
		Console << myendl;
		Console.Error( 1, " Failed to open teleport data script %s", filename );
		Console.Error( 1, " Teleport Data not found" );
		cwmWorldState->SetKeepRun( false );
		cwmWorldState->SetError( true );
		return;
	}
	
	while( !feof( fp ) )
	{
		fgets( text, 255, fp );
		
		if( text[0] != ';' )
		{
			TeleLocationEntry toAdd;
			token = strtok( text, seps );
			i = 0;
			while( token != NULL )
			{
				if( i == 2 )
				{
					if( token[0] == 'A' )
						toAdd.src[2] = 999;
					else
						toAdd.src[2] = (UI16)(makeNum( token ) );
				}
				else if( i < 2 )
					toAdd.src[i] = (UI16)(makeNum( token ) );
				else if( i > 2 && i < 6 )
					toAdd.trg[i - 3] = (UI16)(makeNum( token ) );
				else if( i == 6 )
					toAdd.srcWorld = (SI16)(makeNum( token ) );
				else if( i == 7 )
					toAdd.trgWorld = (UI08)(makeNum( token ) );
					
				i++;
				if( i == 8 )
					break;
				
				token = strtok( NULL, seps );
			}
			cwmWorldState->teleLocs.push_back( toAdd );
		}
	}
	fclose( fp );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cFileIO::LoadCreatures( void )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Loads creatures from npc.dat
//o---------------------------------------------------------------------------o
void cFileIO::LoadCreatures( void )
{
	FILE *npcExists = fopen( "npc.dat", "r" );
	if( npcExists == NULL )
	{
		Console << "Loading creatures from internal memory...";
		initCreatures();
	}
	else
	{
		fclose( npcExists );
		Script *npcData = new Script( "npc.dat", NUM_DEFS );
		Console << "Loading creatures from file...";
		memset( creatures, 0, sizeof( creat_st ) * 2048 );	// init all creatures to 0
		char sect[128];
		const char *tag = NULL;
		const char *data = NULL;
		for( UI16 iCounter = 0; iCounter < 2048; iCounter++ )
		{
			sprintf( sect, "CREATURE %i", iCounter );
			ScriptSection *creatureData = npcData->FindEntry( sect );
			if( creatureData != NULL )
			{
				for( tag = creatureData->First(); !creatureData->AtEnd(); tag = creatureData->Next() )
				{
					if( tag == NULL )
						continue;
					data = creatureData->GrabData();
					switch( tag[0] )
					{
					case 'A':
						if( !strcmp( "ANTIBLINK", tag ) )
							creatures[iCounter].AntiBlink( true );
						else if( !strcmp( "ANIMAL", tag ) )
							creatures[iCounter].IsAnimal( true );
						break;
					case 'B':
						if( !strcmp( "BASESOUND", tag ) )
							creatures[iCounter].BaseSound( makeNum( data ) );
						break;
					case 'F':
						if( !strcmp( "FLIES", tag ) )
							creatures[iCounter].CanFly( true );
						break;
					case 'I':
						if( !strcmp( "ICON", tag ) )
							creatures[iCounter].Icon( makeNum( data ) );
						break;
					case 'S':
						if( !strcmp( "SOUNDFLAG", tag ) )
							creatures[iCounter].SoundFlag( (UI08)makeNum( data ) );
						break;
					case 'W':
						if( !strcmp( "WATERCREATURE", tag ) )
							creatures[iCounter].IsWater( true );
						break;
					}
				}
			}
		}
		delete npcData;
	}
	Console.PrintDone();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cFileIO::initCreatures( void )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Initialize NPC's, assigning basesound, soundflag, and
//|					who_am_i flag
//o---------------------------------------------------------------------------o
void cFileIO::initCreatures( void )
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// soundflags  0: normal, 5 sounds (attack-started,idle, attack, defence, dying, see uox.h)
    //             1: birds .. only one "bird-shape" and zillions of sounds ...
	//             2: only 3 sounds ->  (attack,defence,dying)    
	//             3: only 4 sounds ->   (attack-started,attack,defnce,dying)
	//             4: only 1 sound !!
	//
	// who_am_i bit # 1 creature can fly (must have the animations, so better not change)
	//              # 2 anti-blink: these creatures don't have animation #4, if not set creature will randomly disappear in battle
	//                              if you find a creature that blinks while fighting, set that bit
	//              # 3 animal-bit (currently not used/set)
	//              # 4 water creatures (currently not used/set)
	// icon: used for tracking, to set the appropriate icon
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	creatures[0x01].BaseSound( 0x01AB );                            // Ogre
	creatures[0x01].Icon( 8415 );
    creatures[0x02].BaseSound( 0x016F );                            // Ettin 				
	creatures[0x02].Icon( 8408 );
    creatures[0x03].BaseSound( 0x01D7 );                            // Zombie
	creatures[0x03].Icon( 8428 );
    creatures[0x04].BaseSound( 0x0174 );                            // Gargoyle
	creatures[0x04].CanFly( true ); // set can_fly_bit
	creatures[0x04].Icon( 8409 );
    creatures[0x05].BaseSound( 0x008F );                            // Eagle
	creatures[0x05].CanFly( true ); // set can_fly bit
	creatures[0x05].AntiBlink( true ); // set anti blink bit
	creatures[0x05].IsAnimal( true ); // set anti blink bit
	creatures[0x05].Icon( 8434 );
    creatures[0x06].BaseSound( 0x007D );                            // Bird
	creatures[0x06].CanFly( true ); //set fly bit
	creatures[0x06].IsAnimal( true );
	creatures[0x06].SoundFlag( 1 ); // birds need special treatment cause there are about 20 bird-sounds
	creatures[0x06].Icon( 8430 );
	creatures[0x07].BaseSound( 0x01B0 );                            // Orc	                      
	creatures[0x07].Icon( 8416 );
    creatures[0x08].BaseSound( 0x01BA );                            // corpser
	creatures[0x08].SoundFlag( 3 );
	creatures[0x08].Icon( 8402 );
	creatures[0x09].BaseSound( 0x0165 );                            // daemon 
	creatures[0x09].CanFly( true );
	creatures[0x09].Icon( 8403 );
	creatures[0x0a].BaseSound( 0x0165 );                            // daemon 2
	creatures[0x0a].CanFly( true );
	creatures[0x0a].Icon( 8403 );
	
	
	creatures[0x0c].BaseSound( 362 );                               // Green dragon
	creatures[0x0c].CanFly( true ); // flying creature
	creatures[0x0c].Icon( 8406 );
	creatures[0x0d].BaseSound( 263 );                               // air-ele
	creatures[0x0d].Icon( 8429 );
	creatures[0x0e].BaseSound( 268 );                               // earth-ele		
	creatures[0x0e].Icon( 8407 );
	creatures[0x0f].BaseSound( 273 );                               // fire-ele
	creatures[0x0f].Icon( 8435 );
	creatures[0x10].BaseSound( 0x0116 ); 	                         // water ele
	creatures[0x10].Icon( 8459 );
    creatures[0x11].BaseSound( 0x01B0 );                            // Orc	2
	creatures[0x11].Icon( 8416 );
    creatures[0x12].BaseSound( 0x016F );                           // Ettin 2
	creatures[0x12].Icon( 8408 );
	
	
	creatures[0x15].BaseSound( 219 );                               // Giant snake
	creatures[0x15].Icon( 8446 );
	creatures[0x16].BaseSound( 377 );                               // gazer
	creatures[0x16].Icon( 8426 );
	
	creatures[0x18].BaseSound( 412 );                               // liche
	creatures[0x18].Icon( 8440 );									// counldnt find a better one :(
	
	creatures[0x1a].BaseSound( 382 );                               // ghost 1
	creatures[0x1a].Icon( 8457 );
	
	creatures[0x1c].BaseSound( 387 );                               // giant spider
    creatures[0x1c].Icon( 8445 );
	creatures[0x1d].BaseSound( 158 );                               // gorialla
	creatures[0x1d].Icon( 8437 );
	creatures[0x1e].BaseSound( 402 );                               // harpy			
	creatures[0x1e].Icon( 8412 );
	creatures[0x1f].BaseSound( 407 );                               // headless
	creatures[0x1f].Icon( 8458 );
	
	creatures[0x21].BaseSound( 417 );                               // lizardman
	creatures[0x23].BaseSound( 417 );                            
	creatures[0x24].BaseSound( 417 );        
	creatures[0x25].BaseSound( 417 );
	creatures[0x26].BaseSound( 417 );
	creatures[0x21].Icon( 8414 );
	creatures[0x23].Icon( 8414 );
	creatures[0x24].Icon( 8414 );
	creatures[0x25].Icon( 8414 );
	creatures[0x26].Icon( 8414 );
	
	
	creatures[0x27].BaseSound( 422 );                               // mongbat
	creatures[0x27].CanFly( true ); // yes, they can fly
	creatures[0x27].Icon( 8441 );
	
	creatures[0x29].BaseSound( 0x01B0 );                            // orc 3
	creatures[0x29].Icon( 8416 );

	creatures[0x2A].BaseSound( 437 );                               // ratman
	creatures[0x2C].BaseSound( 437 );                            
	creatures[0x2D].BaseSound( 437 );                            
	creatures[0x2A].Icon( 8419 );
	creatures[0x2C].Icon( 8419 );
	creatures[0x2D].Icon( 8419 );
	
	creatures[0x2F].BaseSound( 0x01BA );                            // Reaper			
	creatures[0x2F].Icon( 8442 );
	creatures[0x30].BaseSound( 397 );                               // giant scorprion	
	creatures[0x30].Icon( 8420 );
	
	creatures[0x32].BaseSound( 452 );                               // skeleton 2
	creatures[0x32].Icon( 8423 );
	creatures[0x33].BaseSound( 456 );                               // slime	
    creatures[0x33].Icon( 8424 );
	creatures[0x34].BaseSound( 219 );                               // Snake
	creatures[0x34].Icon( 8444 );
	creatures[0x34].IsAnimal( true ); // set anti blink bit
    creatures[0x35].BaseSound( 461 );                               // troll 				
    creatures[0x35].Icon( 8425 );
    creatures[0x36].BaseSound( 461 );                               // troll 2
	creatures[0x36].Icon( 8425 );
    creatures[0x37].BaseSound( 461 );                               // troll 3
	creatures[0x37].Icon( 8425 );
    creatures[0x38].BaseSound( 452 );                               // skeleton 3
	creatures[0x38].Icon( 8423 );
    creatures[0x39].BaseSound( 452 );                               // skeleton 4
	creatures[0x39].Icon( 8423 );
	creatures[0x3A].BaseSound( 466 );                               // wisp	                      
	creatures[0x3A].Icon( 8448 );
    creatures[0x3B].BaseSound( 362 );                               // red dragon
	creatures[0x3B].CanFly( true ); // set fly bit
	creatures[0x3C].BaseSound( 362 );                               // smaller red dragon
	creatures[0x3C].CanFly( true );
	creatures[0x3D].BaseSound( 362 );                               // smaller green dragon
	creatures[0x3D].CanFly( true );
	creatures[0x3B].Icon( 8406 );
	creatures[0x3C].Icon( 8406 );
	creatures[0x3D].Icon( 8406 );
	
	
	creatures[0x96].BaseSound( 477 );                               // sea serpant
	creatures[0x96].SoundFlag( 3 );
	creatures[0x96].Icon( 8446 ); // normal serpant icon
	creatures[0x97].BaseSound( 138 );                               // dolphin
	creatures[0x97].Icon( 8433 ); // correct icon ???
	
	creatures[0xC8].BaseSound( 168 );                               // white horse		
    creatures[0xC8].Icon( 8479 );
	creatures[0xC8].IsAnimal( true ); // set anti blink bit
	creatures[0xC9].BaseSound( 105 );                               // cat
	creatures[0xC9].AntiBlink( true ); // set blink flag
	creatures[0xC9].IsAnimal( true );
    creatures[0xC9].Icon( 8475 );
	creatures[0xCA].BaseSound( 90 );   	                         // alligator
    creatures[0xCA].Icon( 8410 );
	creatures[0xCA].IsAnimal( true );
    creatures[0xCB].BaseSound( 196 );                               // small pig
    creatures[0xCB].Icon( 8449 );
	creatures[0xCB].IsAnimal( true );
    creatures[0xCC].BaseSound( 168 );                               // brown horse
    creatures[0xCC].Icon( 8481 );
	creatures[0xCC].IsAnimal( true );
	creatures[0xCD].BaseSound( 201 );                               // rabbit
	creatures[0xCD].SoundFlag( 2 );                                 // rabbits only have 3 sounds, thus need special treatment
	creatures[0xCD].Icon( 8485 );
	creatures[0xCD].IsAnimal( true );
	
	creatures[0xCF].BaseSound( 214 );                               // wooly sheep
	creatures[0xCF].Icon( 8427 );
	creatures[0xCF].IsAnimal( true );
	
	creatures[0xD0].BaseSound( 110 );                               // chicken
	creatures[0xD0].Icon( 8401 );
	creatures[0xD0].IsAnimal( true );
	creatures[0xD1].BaseSound( 153 );                               // goat
	creatures[0xD1].Icon( 8422 ); // theres no goat icon, so i took a (differnt) sheep
	creatures[0xD1].IsAnimal( true );
	
	creatures[0xD3].BaseSound( 95 );                                // brown bear
    creatures[0xD3].Icon( 8399 );
	creatures[0xD3].IsAnimal( true );
	creatures[0xD4].BaseSound( 95 );                                // grizzly bear
	creatures[0xD4].Icon( 8411 );
	creatures[0xD4].IsAnimal( true );
	creatures[0xD5].BaseSound( 95 );                                // polar bear	
	creatures[0xD5].Icon( 8417 );
	creatures[0xD5].IsAnimal( true );
	creatures[0xD6].BaseSound( 186 );                               // panther
	creatures[0xD6].AntiBlink( true );
    creatures[0xD6].Icon( 8473 );
	creatures[0xD7].BaseSound( 392 );                               // giant rat
	creatures[0xD7].Icon( 8400 );
	creatures[0xD8].BaseSound( 120 );                               // cow
    creatures[0xD8].Icon( 8432 );
	creatures[0xD8].IsAnimal( true );
	creatures[0xD9].BaseSound( 133 );                               // dog
	creatures[0xD9].Icon( 8405 );
	creatures[0xD9].IsAnimal( true );
	
	creatures[0xDC].BaseSound( 183 );                               // llama
	creatures[0xDC].SoundFlag( 2 );
	creatures[0xDC].Icon( 8438 );
	creatures[0xDC].IsAnimal( true );
	
	creatures[0xDD].BaseSound( 224 );                               // walrus
	creatures[0xDD].Icon( 8447 );
	creatures[0xDD].IsAnimal( true );
	
	creatures[0xDF].BaseSound( 216 );                               // lamb/shorn sheep
	creatures[0xDF].SoundFlag( 2 );
	creatures[0xDF].Icon( 8422 );
	creatures[0xDF].IsAnimal( true );
	creatures[0xE1].BaseSound( 229 );                               // jackal
	creatures[0xE1].SoundFlag( 2 );
	creatures[0xE1].AntiBlink( true ); // set anti blink bit
    creatures[0xE1].Icon( 8426 );
	creatures[0xE2].BaseSound( 168 );                               // yet another horse
	creatures[0xE2].Icon( 8484 );
	creatures[0xE2].IsAnimal( true );
	
	creatures[0xE4].BaseSound( 168 );                               // horse ...
	creatures[0xE4].Icon( 8480 );
	creatures[0xE4].IsAnimal( true );
	
	creatures[0xE7].BaseSound( 120 );                               // brown cow
	creatures[0xE7].AntiBlink( true );
	creatures[0xE7].Icon( 8432 );
	creatures[0xE7].IsAnimal( true );

	creatures[0xE8].BaseSound( 100 );                               // bull
	creatures[0xE8].AntiBlink( true );
	creatures[0xE8].Icon( 8431 );
	creatures[0xE8].IsAnimal( true );

	creatures[0xE9].BaseSound( 120 );                               // b/w cow
	creatures[0xE9].AntiBlink( true );
	creatures[0xE9].Icon( 8451 );
	creatures[0xE9].IsAnimal( true );

	creatures[0xEA].BaseSound( 130 );                               // deer
	creatures[0xEA].SoundFlag( 2 );
	creatures[0xEA].Icon( 8404 );
	creatures[0xEA].IsAnimal( true );
	
	creatures[0xED].BaseSound( 130 );                               // small deer
	creatures[0xED].SoundFlag( 2 );
    creatures[0xED].Icon( 8404 );
	creatures[0xED].IsAnimal( true );

	creatures[0xEE].BaseSound( 204 );                               // rat
	creatures[0xEE].Icon( 8483 );
	creatures[0xEE].IsAnimal( true );
	
	creatures[0x122].BaseSound( 196 );                                // large pig
    creatures[0x122].Icon( 8449 );
	creatures[0x122].IsAnimal( true );

	creatures[0x123].BaseSound( 168 );                                // pack horse
	creatures[0x123].Icon( 8486 );
	creatures[0x123].IsAnimal( true );

	creatures[0x124].BaseSound( 183 );                                // pack llama	
	creatures[0x124].SoundFlag( 2 );
	creatures[0x124].Icon( 8487 );
	creatures[0x124].IsAnimal( true );
	
	creatures[0x23D].BaseSound( 263 );                                 // e-vortex
	creatures[0x23E].BaseSound( 512 );                                 // blade spritit
	creatures[0x23E].SoundFlag( 4 );
	
	creatures[0x600].BaseSound( 115 );                                // cougar;
	creatures[0x600].Icon( 8473 );
	creatures[0x600].IsAnimal( true );
	
	creatures[0x190].Icon( 8454 );
	creatures[0x191].Icon( 8455 );
	
}

void ReadWorldTagData( std::ifstream &inStream, char *tag, char *data )
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
#if defined(__unix__)
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
	std::ifstream ifsFile;
	ifsFile.open( filepath, std::ios::in );
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

BinBuffer::BinBuffer( std::ifstream &in )
{
	Read( in );//resets inside Read()
}

BinBuffer::~BinBuffer()
{
}

void BinBuffer::Read( std::ifstream &inFile )
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

void BinBuffer::Write( std::ofstream &outFile )
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
	if( fp < static_cast<int>(Buff.size()) )
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
	return ( fp >= static_cast<int>(Buff.size()) ? true : false );
}

int BinBuffer::Position( void )
{
	return fp;
}

int BinBuffer::Length( void )
{
	return Buff.size();
}

