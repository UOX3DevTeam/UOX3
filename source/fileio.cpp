#include "uox3.h"
#include "fileio.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "cSpawnRegion.h"
#include "scriptc.h"
#include "townregion.h"
#include <stdexcept>
#include <vector>
#include <map>
#include <fstream>
#include <cstring>
#include <filesystem>
#include <cmath>
#include <algorithm>
#include <sstream>
#include "UOPInterface.hpp"

#if UOX_PLATFORM != PLATFORM_WIN32
#  include <fcntl.h>     // open
#  include <sys/mman.h>  // mmap, mmunmap
#endif

namespace UOX
{

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UOXFile::UOXFile( const char* const fileName, const char * const )
//|						: memPtr( 0 ), fileSize( 0 ), bIndex( 0 ), usingUOP( false )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Support read-only binary mode.
//o-----------------------------------------------------------------------------------------------o
UOXFile::UOXFile( const char* const fileName, const char * const )
: memPtr( 0 ), fileSize( 0 ), bIndex( 0 ), usingUOP( false )
{
    auto strfilename = std::string( fileName );
    auto filepath = std::filesystem::path( strfilename );
    if( filepath.extension() == ".uop" )
	{
        // UOP File, get the map #
		auto nameonly = filepath.filename();
		auto mapnum = std::stoi( nameonly.string().substr( 3, 1 ));
        try {
            auto rvalue = UOP::loadUOP( strfilename, mapnum );
            this->memPtr = std::get<1>( rvalue );
            if( this->memPtr != nullptr )
			{
                this->fileSize = std::get<0>( rvalue );
                usingUOP = true;
            }
        }
        catch (...) {
            if( this->memPtr != nullptr )
			{
                delete[] memPtr;
                memPtr=nullptr;
            }
            this->memPtr= nullptr;
            this->fileSize = 0;
        }
        return;
    }
        
#if UOX_PLATFORM == PLATFORM_WIN32
	HANDLE hFile = ::CreateFileA(
				fileName,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_SEQUENTIAL_SCAN,
				NULL
			);

	if( hFile == INVALID_HANDLE_VALUE )
		return;

	// Store the size of the file, it's used to construct
	//  the end iterator
	fileSize = ::GetFileSize( hFile, NULL );

    HANDLE hMap = ::CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, 0, NULL );
        
    if( hMap == NULL )
    {
        ::CloseHandle( hFile );
        return;
    }
        
    memPtr = (char*)::MapViewOfFile( hMap, FILE_MAP_READ, 0, 0, 0 );
        
    // We hold both the file handle and the memory pointer.
    // We can close the hMap handle now because Windows holds internally
    //  a reference to it since there is a view mapped.
    ::CloseHandle( hMap );
        
    // It seems like we can close the file handle as well (because
    //  a reference is hold by the filemap object).
    ::CloseHandle( hFile );
        
#else
    // postfix version
    // open the file
    SI32 fd = open(fileName,
#ifdef O_NOCTTY
				O_NOCTTY | // if stdin was closed then opening a file
								// would cause the file to become the controlling
								// terminal if the filename refers to a tty. Setting
								// O_NOCTTY inhibits this.
#endif
                    O_RDONLY);
        
    if( fd == -1 )
        return;
        
    // call fstat to find get information about the file just
    // opened (size and file type)
    struct stat stat_buf;
    if(( fstat( fd, &stat_buf ) != 0 ) || !S_ISREG( stat_buf.st_mode ))
    {	// if fstat returns an error or if the file isn't a
        // regular file we give up.
        close( fd );
        return;
    }
        
    fileSize = stat_buf.st_size;
    // perform the actual mapping
    memPtr = (char*)mmap( 0, stat_buf.st_size, PROT_READ, MAP_SHARED, fd, 0 );
    // it is safe to close() here. POSIX requires that the OS keeps a
    // second handle to the file while the file is mmapped.
    close( fd );
#endif
}

UOXFile::~UOXFile()
{
    if( usingUOP )
	{
        delete[] memPtr;
        memPtr = nullptr;
    }
    if( memPtr )
    {
#if UOX_PLATFORM == PLATFORM_WIN32
        UnmapViewOfFile( memPtr );
#else
        munmap(memPtr, fileSize);
#endif
    }
}

void UOXFile::seek( size_t offset, UI08 whence )
{
	if( whence == SEEK_SET )	// seek from beginnng
		bIndex = offset;
	else if ( whence == SEEK_CUR )	// seek from current
		bIndex += offset;
	else if ( whence == SEEK_END )	// seek from end
		bIndex = fileSize + offset;
}

SI32 UOXFile::getch( void )
{
	return *(memPtr + bIndex++);
}

void UOXFile::getUChar( UI08 *buff, UI32 number )
{
    memcpy( buff, memPtr+bIndex, number);
    bIndex += number;
}   
    
void UOXFile::getChar( SI08 *buff, UI32 number )
{
	memcpy( buff, memPtr+bIndex, number);
	bIndex += number;
}

void UOXFile::getUShort( UI16 *buff, UI32 number )
{
	number *= sizeof(UI16);
	memcpy( buff, memPtr+bIndex, number);
	bIndex += number;
}

void UOXFile::getShort( SI16 *buff, UI32 number )
{
    number *= sizeof(SI16);
    memcpy( buff, memPtr+bIndex, number);
    bIndex += number;
}

void UOXFile::getULong( UI32 *buff, UI32 number )
{
	number *= sizeof(UI32);
	memcpy( buff, memPtr+bIndex, number);
	bIndex += number;
}

void UOXFile::getLong( SI32 *buff, UI32 number )
{
	number *= sizeof(SI32);
	memcpy( buff, memPtr+bIndex, number);
	bIndex += number;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadCustomTitle( void )
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads players titles (Karma, Fame, Murder, ect)
//o-----------------------------------------------------------------------------------------------o
void LoadCustomTitle( void )
{ 
	size_t titlecount = 0;
	UString tag;
	UString data;
	ScriptSection *CustomTitle = FileLookup->FindEntry( "SKILL", titles_def );
	if( CustomTitle == NULL )
		return;
	for( tag = CustomTitle->First(); !CustomTitle->AtEnd() && titlecount < ALLSKILLS; tag = CustomTitle->Next() )
	{
		data = CustomTitle->GrabData();
		cwmWorldState->title[titlecount].skill	= data;
		++titlecount;
	}
	CustomTitle = FileLookup->FindEntry( "PROWESS", titles_def );
	if( CustomTitle == NULL )
		return;
	for( tag = CustomTitle->First(); !CustomTitle->AtEnd(); tag = CustomTitle->Next() )
	{
		data = CustomTitle->GrabData();
		cwmWorldState->prowessTitles.push_back( TitlePair_st( tag.toShort(), data ) );
	}

	CustomTitle = FileLookup->FindEntry( "FAME", titles_def );
	titlecount	= 0;

	for( tag = CustomTitle->First(); !CustomTitle->AtEnd() && titlecount < ALLSKILLS; tag = CustomTitle->Next() )
	{
		data = CustomTitle->GrabData();
		cwmWorldState->title[titlecount].fame = data;
		++titlecount;
	}

	// Murder tags now scriptable in SECTION MURDER - Titles.dfn - Thanks Ab - Zane
	CustomTitle = FileLookup->FindEntry( "MURDERER", titles_def );
	if( CustomTitle == NULL )
		return;
	for( tag = CustomTitle->First(); !CustomTitle->AtEnd(); tag = CustomTitle->Next() )
	{
		data = CustomTitle->GrabData();
		cwmWorldState->murdererTags.push_back( TitlePair_st( tag.toShort(), data ) );
	}

	FileLookup->Dispose( titles_def );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadSkills( void )
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Load skills from definition files
//o-----------------------------------------------------------------------------------------------o
void LoadSkills( void )
{

	UString skEntry;
	UString tag, data, UTag;
	UI08 i = 0;
	for( Script *creatScp = FileLookup->FirstScript( skills_def ); !FileLookup->FinishedScripts( skills_def ); creatScp = FileLookup->NextScript( skills_def ) )
	{
		if( creatScp == NULL )
			continue;

		for( ScriptSection *SkillList = creatScp->FirstEntry(); SkillList != NULL; SkillList = creatScp->NextEntry() )
		{
			if( SkillList == NULL )
				continue;

			skEntry = creatScp->EntryName();
			if( skEntry.section( " ", 0, 0 ) == "SKILL" )
			{
				i = skEntry.section( " ", 1, 1 ).toUByte();
				if( i <= INTELLECT )
				{
					cwmWorldState->skill[i].ResetDefaults();
					for( tag = SkillList->First(); !SkillList->AtEnd(); tag = SkillList->Next() )
					{
						UTag = tag.upper();
						data = SkillList->GrabData();
						if( UTag == "STR" )
							cwmWorldState->skill[i].strength = data.toUShort();
						else if( UTag == "DEX" )
							cwmWorldState->skill[i].dexterity = data.toUShort();
						else if( UTag == "INT" )
							cwmWorldState->skill[i].intelligence = data.toUShort();
						else if( UTag == "SKILLPOINT" )
						{
							advance_st tempAdvance;
							data = data.simplifyWhiteSpace();
							tempAdvance.base	= data.section( ",", 0, 0 ).toUShort();
							tempAdvance.success = data.section( ",", 1, 1 ).toUByte();
							tempAdvance.failure = data.section( ",", 2, 2 ).toUByte();
							if( data.sectionCount( "," ) == 3 )
								tempAdvance.amtToGain = data.section( ",", 3, 3 ).toUByte();
							cwmWorldState->skill[i].advancement.push_back( tempAdvance );
						}
						else if( UTag == "MADEWORD" )
							cwmWorldState->skill[i].madeword = data.stripWhiteSpace();
						else if( UTag == "NAME" )
							cwmWorldState->skill[i].name = data.stripWhiteSpace();
						else
							Console.Warning( "Unknown tag in skills.dfn: %s", data.stripWhiteSpace().c_str() );
					}
				}
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadSpawnRegions( void )
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads spawning regions from definition files
//o-----------------------------------------------------------------------------------------------o
void LoadSpawnRegions( void )
{
	cwmWorldState->spawnRegions.clear();
	UI16 i					= 0;
	for( Script *spnScp = FileLookup->FirstScript( spawn_def ); !FileLookup->FinishedScripts( spawn_def ); spnScp = FileLookup->NextScript( spawn_def ) )
	{
		if( spnScp == NULL )
			continue;
		for( ScriptSection *toScan = spnScp->FirstEntry(); toScan != NULL; toScan = spnScp->NextEntry() )
		{
			if( toScan == NULL )
				continue;
			UString sectionName = spnScp->EntryName();
			if( "REGIONSPAWN" == sectionName.section( " ", 0, 0 ) ) // Is it a region spawn entry?
			{
				i = sectionName.section( " ", 1, 1 ).toUShort();
				if( cwmWorldState->spawnRegions.find( i ) == cwmWorldState->spawnRegions.end() )
				{
					cwmWorldState->spawnRegions[i] = new CSpawnRegion( i );
					cwmWorldState->spawnRegions[i]->Load( toScan );
				}
				else
					Console.Warning( "spawn.dfn has a duplicate REGIONSPAWN entry, Entry Number: %u", i );
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadRegions( void )
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Load regions from regions.dfn and townregions from regions.wsc
//o-----------------------------------------------------------------------------------------------o
void LoadRegions( void )
{
	cwmWorldState->townRegions.clear();
	std::string regionsFile = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "regions.wsc";
	bool performLoad		= false;
	Script *ourRegions		= NULL;
	if( FileExists( regionsFile ) )
	{
		performLoad = true;
		ourRegions = new Script( regionsFile, NUM_DEFS, false );
	}

	UI16 i = 0;
	UString regEntry;
	for( Script *regScp = FileLookup->FirstScript( regions_def ); !FileLookup->FinishedScripts( regions_def ); regScp = FileLookup->NextScript( regions_def ) )
	{
		if( regScp == NULL )
			continue;

		for( ScriptSection *toScan = regScp->FirstEntry(); toScan != NULL; toScan = regScp->NextEntry() )
		{
			if( toScan == NULL )
				continue;

			regEntry = regScp->EntryName();
			if( regEntry.section( " ", 0, 0 ) == "REGION" )
			{
				i = regEntry.section( " ", 1, 1 ).toUShort();
				if( cwmWorldState->townRegions.find( i ) == cwmWorldState->townRegions.end() )
				{
					cwmWorldState->townRegions[i] = new CTownRegion( i );
					cwmWorldState->townRegions[i]->InitFromScript( toScan );
					if( performLoad )
						cwmWorldState->townRegions[i]->Load( ourRegions );
				}
				else
					Console.Warning( "regions.dfn has a duplicate REGION entry, Entry Number: %u", i );
			}
		}
	}
	if( regEntry == "" )
	{
		// No regions found? :O Shut down UOX3, or we'll run into trouble later.
		Console.PrintFailed();
		Shutdown( FATAL_UOX3_ALLOC_MAPREGIONS );
	}

	if( performLoad )
	{
		delete ourRegions;
		ourRegions = NULL;
	}
	ScriptSection *InstaLog = FileLookup->FindEntry( "INSTALOG", regions_def );
	if( InstaLog == NULL ) 
		return;
	LogoutLocationEntry toAdd;
	UString data, UTag;
	for( UString tag = InstaLog->First(); !InstaLog->AtEnd(); tag = InstaLog->Next() )
	{
		UTag = tag.upper();
		data	= InstaLog->GrabData();
		if( UTag == "X1" ) 
			toAdd.x1 = data.toShort();
		else if( UTag == "Y1" ) 
			toAdd.y1 = data.toShort();
		else if( UTag == "X2" )
			toAdd.x2 = data.toShort();
		else if( UTag == "Y2" )
			toAdd.y2 = data.toShort();
		else if( UTag == "WORLD" )
		{
			toAdd.worldNum = data.toByte();
			cwmWorldState->logoutLocs.push_back( toAdd );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadTeleportLocations( void )
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Load teleport locations from definition files
//o-----------------------------------------------------------------------------------------------o
void LoadTeleportLocations( void )
{
	std::string filename = cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS ) + "teleport.scp"; 
	cwmWorldState->teleLocs.resize( 0 );
	
	if( !FileExists( filename ) )
	{
		Console << myendl;
		Console.Error( " Failed to open teleport data script %s", filename.c_str() );
		Console.Error( " Teleport Data not found" );
		cwmWorldState->SetKeepRun( false );
		cwmWorldState->SetError( true );
		return;
	}

	Script *teleportData = new Script( filename, NUM_DEFS, false );
	if( teleportData != NULL )
	{
		cwmWorldState->teleLocs.reserve( teleportData->NumEntries() );

		UI16 tempX, tempY;
		SI08 tempZ;
		ScriptSection *teleportSect = NULL;
		UString tag, data, temp;
		for( teleportSect = teleportData->FirstEntry(); teleportSect != NULL; teleportSect = teleportData->NextEntry() )
		{
			if( teleportSect != NULL )
			{
				for( tag = teleportSect->First(); !teleportSect->AtEnd(); tag = teleportSect->Next() )
				{
					CTeleLocationEntry toAdd;
					if( tag.upper() == "ENTRY" )
					{
						tempX = 0;
						tempY = 0;
						tempZ = ILLEGAL_Z;
						data = teleportSect->GrabData().simplifyWhiteSpace();
						SI32 sectCount = data.sectionCount( "," );
						if( sectCount >= 5 )
						{
							tempX	= data.section( ",", 0, 0 ).toUShort();
							tempY	= data.section( ",", 1, 1 ).toUShort();
							temp	= data.section( ",", 2, 2 ).upper();
							temp	= temp.stripWhiteSpace().upper();
							if( temp != "ALL" && temp != "A" )
								tempZ = temp.toByte();
							toAdd.SourceLocation( tempX, tempY, tempZ );

							tempX	= data.section( ",", 3, 3 ).toUShort();
							tempY	= data.section( ",", 4, 4 ).toUShort();
							tempZ	= data.section( ",", 5, 5 ).toByte();
							toAdd.TargetLocation( tempX, tempY, tempZ );

							if( sectCount >= 6 )
							{
								toAdd.SourceWorld( data.section( ",", 6, 6 ).toUByte() );
								if( sectCount >= 7 )
									toAdd.TargetWorld( data.section( ",", 7, 7 ).toUByte() );
							}
							cwmWorldState->teleLocs.push_back( toAdd );
						}
						else
							Console.Error( "Insufficient parameters for teleport entry" );
					}
				}
			}
		}
		delete teleportData;
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LoadCreatures()
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads creatures from creature definition files
//o-----------------------------------------------------------------------------------------------o
void LoadCreatures( void )
{
	UString cEntry;
	UString tag, data, UTag;
	UI16 i = 0;
	for( Script *creatScp = FileLookup->FirstScript( creatures_def ); !FileLookup->FinishedScripts( creatures_def ); creatScp = FileLookup->NextScript( creatures_def ) )
	{
		if( creatScp == NULL )
			continue;

		for( ScriptSection *creatureData = creatScp->FirstEntry(); creatureData != NULL; creatureData = creatScp->NextEntry() )
		{
			if( creatureData == NULL )
				continue;

			cEntry = creatScp->EntryName();
			if( cEntry.section( " ", 0, 0 ) == "CREATURE" )
			{
				i = cEntry.section( " ", 1, 1 ).toUShort();

				for( tag = creatureData->First(); !creatureData->AtEnd(); tag = creatureData->Next() )
				{
					if( tag.empty() )
						continue;
					data = creatureData->GrabData();
					UTag = tag.upper();
					switch( (UTag.data()[0]) )
					{
						case 'A':
							if( UTag == "ANTIBLINK" )
								cwmWorldState->creatures[i].AntiBlink( true );
							else if( UTag == "ANIMAL" )
								cwmWorldState->creatures[i].IsAnimal( true );
							break;
						case 'B':
							if( UTag == "BASESOUND" )
								break;
							break;
						case 'F':
							if( UTag == "FLIES" )
								cwmWorldState->creatures[i].CanFly( true );
							break;
						case 'H':
							if( UTag == "HUMAN" )
								cwmWorldState->creatures[i].IsHuman( true );
							break;
						case 'I':
							if( UTag == "ICON" )
								cwmWorldState->creatures[i].Icon( data.toUShort() );
							break;
						case 'M':
							if( UTag == "MOVEMENT" )
							{
								if( data.upper() == "WATER" )
									cwmWorldState->creatures[i].IsWater( true );
								else if( data.upper() == "BOTH" )
									cwmWorldState->creatures[i].IsAmphibian( true );
								else
								{
									cwmWorldState->creatures[i].IsWater( false );
									cwmWorldState->creatures[i].IsAmphibian( false );
								}
							}
							else if( UTag == "MOUNTID" )
								cwmWorldState->creatures[i].MountID( data.toUShort() );
							break;
						case 'S':
							if( UTag == "SOUNDFLAG" )
								break;
							else if( UTag == "SOUND_IDLE" )
								cwmWorldState->creatures[i].SetSound( SND_IDLE, data.toUShort() );
							else if( UTag == "SOUND_STARTATTACK" )
								cwmWorldState->creatures[i].SetSound( SND_STARTATTACK, data.toUShort() );
							else if( UTag == "SOUND_ATTACK" )
								cwmWorldState->creatures[i].SetSound( SND_ATTACK, data.toUShort() );
							else if( UTag == "SOUND_DEFEND" )
								cwmWorldState->creatures[i].SetSound( SND_DEFEND, data.toUShort() );
							else if( UTag == "SOUND_DIE" )
								cwmWorldState->creatures[i].SetSound( SND_DIE, data.toUShort() );
							break;
					}
				}
			}
		}
	}

	FileLookup->Dispose( creatures_def );
}

void ReadWorldTagData( std::ifstream &inStream, UString &tag, UString &data )
{
	char temp[4096];
	tag = "o---o";
	data = "o---o";
	while( !inStream.eof() && !inStream.fail() )
	{
		inStream.getline( temp, 4096 );
		UString sLine( temp );
		sLine = sLine.removeComment().stripWhiteSpace();
		if( !sLine.empty() )
		{
			if( sLine != "o---o" )
			{
				if( sLine.sectionCount( "=" ) == 1 )
				{
					tag		= sLine.section( "=", 0, 0 ).stripWhiteSpace();
					data	= sLine.section( "=", 1 ).stripWhiteSpace();
					break;
				}
			}
			else
				break;
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadPlaces( void )
//|	Org/team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Load locations from location definition files
//o-----------------------------------------------------------------------------------------------o
void LoadPlaces( void )
{
	cwmWorldState->goPlaces.clear();
	UString data, UTag, entryName;
	GoPlaces_st *toAdd		= NULL;

	for( Script *locScp = FileLookup->FirstScript( location_def ); !FileLookup->FinishedScripts( location_def ); locScp = FileLookup->NextScript( location_def ) )
	{
		if( locScp == NULL )
			continue;
		for( ScriptSection *toScan = locScp->FirstEntry(); toScan != NULL; toScan = locScp->NextEntry() )
		{
			if( toScan == NULL )
				continue;
			entryName			= locScp->EntryName();
			size_t entryNum		= entryName.section( " ", 1, 1 ).toUInt();
			if( entryName.section( " ", 0, 0 ).upper() == "LOCATION" && entryNum )
			{
				if( cwmWorldState->goPlaces.find( entryNum ) != cwmWorldState->goPlaces.end() )
					Console.Warning( "Doubled up entry in Location.dfn (%u)", entryNum );
				toAdd = &cwmWorldState->goPlaces[entryNum];
				if( toAdd != NULL )
				{
					for( UString tag = toScan->First(); !toScan->AtEnd(); tag = toScan->Next() )
					{
						data = toScan->GrabData();
						UTag = tag.upper();
						if( UTag == "X" )
							toAdd->x = data.toShort();
						else if( UTag == "Y" )
							toAdd->y = data.toShort();
						else if( UTag == "Z" )
							toAdd->z = data.toByte();
						else if( UTag == "WORLD" )
							toAdd->worldNum = data.toUByte();
						else if( UTag == "INSTANCEID" )
							toAdd->instanceID = data.toUShort();
						else if( UTag == "LOCATION" )
						{
							size_t sectionCount = data.sectionCount( "," );
							if( sectionCount >= 3 )
							{
								toAdd->x		= data.section( ",", 0, 0 ).toShort();
								toAdd->y		= data.section( ",", 1, 1 ).toShort();
								toAdd->z		= data.section( ",", 2, 2 ).toByte();
								toAdd->worldNum = data.section( ",", 3, 3 ).toUByte();
							}

							if( sectionCount == 4 )
							{
								toAdd->instanceID = data.section( ",", 4, 4 ).toUShort();
							}
						}
					}
				}
			}
		}
	}

	FileLookup->Dispose( location_def );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool FileExists( const std::string& filepath )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if specified file/filepath exists
//o-----------------------------------------------------------------------------------------------o
bool FileExists( const std::string& filepath )
{
    auto temp = std::filesystem::path( filepath );
    return std::filesystem::exists(temp);
}

}
