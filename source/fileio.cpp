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
#include <algorithm>

#if PLATFORM != WINDOWS
#  include <fcntl.h>     // open
#  include <sys/mman.h>  // mmap, mmunmap
#endif


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

#if PLATFORM == WINDOWS
	HANDLE hFile = ::CreateFileA(
								 fileName,
								 GENERIC_READ,
								 FILE_SHARE_READ,
								 nullptr,
								 OPEN_EXISTING,
								 FILE_FLAG_SEQUENTIAL_SCAN,
								 nullptr
								 );

	if( hFile == INVALID_HANDLE_VALUE )
		return;

	// Store the size of the file, it's used to construct
	//  the end iterator
	fileSize = ::GetFileSize( hFile, nullptr );

	HANDLE hMap = ::CreateFileMapping( hFile, nullptr, PAGE_READONLY, 0, 0, nullptr );

	if( hMap == nullptr )
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
#if PLATFORM == WINDOWS
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
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads players titles (Karma, Fame, Murder, ect)
//o-----------------------------------------------------------------------------------------------o
void LoadCustomTitle( void )
{
	size_t titlecount = 0;
	std::string tag;
	std::string data;
	ScriptSection *CustomTitle = FileLookup->FindEntry( "SKILL", titles_def );
	if( CustomTitle == nullptr )
	{
		return;
	}
	for( tag = CustomTitle->First(); !CustomTitle->AtEnd() && titlecount < ALLSKILLS; tag = CustomTitle->Next() )
	{
		data = CustomTitle->GrabData();
		cwmWorldState->title[titlecount].skill	= data;
		++titlecount;
	}
	CustomTitle = FileLookup->FindEntry( "PROWESS", titles_def );
	if( CustomTitle == nullptr )
	{
		return;
	}
	for( tag = CustomTitle->First(); !CustomTitle->AtEnd(); tag = CustomTitle->Next() )
	{
		data = CustomTitle->GrabData();
		cwmWorldState->prowessTitles.push_back( TitlePair_st( static_cast<SI16>(std::stoi(tag, nullptr, 0)), data ) );
	}

	CustomTitle = FileLookup->FindEntry( "FAME", titles_def );
	if( CustomTitle == nullptr )
	{
		return;
	}
	titlecount	= 0;

	for( tag = CustomTitle->First(); !CustomTitle->AtEnd() && titlecount < ALLSKILLS; tag = CustomTitle->Next() )
	{
		data = CustomTitle->GrabData();
		cwmWorldState->title[titlecount].fame = data;
		++titlecount;
	}

	// Murder tags now scriptable in SECTION MURDER - Titles.dfn
	CustomTitle = FileLookup->FindEntry( "MURDERER", titles_def );
	if( CustomTitle == nullptr )
	{
		return;
	}
	for( tag = CustomTitle->First(); !CustomTitle->AtEnd(); tag = CustomTitle->Next() )
	{
		data = CustomTitle->GrabData();
		cwmWorldState->murdererTags.push_back( TitlePair_st( static_cast<SI16>(std::stoi(tag, nullptr, 0)), data ) );
	}

	FileLookup->Dispose( titles_def );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadSkills( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Load skills from definition files
//o-----------------------------------------------------------------------------------------------o
void LoadSkills( void )
{
	std::string skEntry;
	std::string tag, data, UTag;
	UI08 i = 0;
	for( Script *creatScp = FileLookup->FirstScript( skills_def ); !FileLookup->FinishedScripts( skills_def ); creatScp = FileLookup->NextScript( skills_def ) )
	{
		if( creatScp == nullptr )
			continue;

		for( ScriptSection *SkillList = creatScp->FirstEntry(); SkillList != nullptr; SkillList = creatScp->NextEntry() )
		{
			if( SkillList == nullptr )
				continue;

			skEntry = creatScp->EntryName();
			auto ssecs = strutil::sections( skEntry, " " );
			if( strutil::stripTrim( ssecs[0] ) == "SKILL" )
			{
				if( ssecs.size() > 1 )
				{
					i = static_cast<UI08>(std::stoul(strutil::stripTrim( ssecs[1] )));
					if( i <= INTELLECT )
					{
						cwmWorldState->skill[i].ResetDefaults();
						for( tag = SkillList->First(); !SkillList->AtEnd(); tag = SkillList->Next() )
						{
							UTag = strutil::toupper( tag );
							data = SkillList->GrabData();
							data = strutil::stripTrim( data );
							if( UTag == "STR" )
							{
								cwmWorldState->skill[i].strength = static_cast<UI16>(std::stoul(data, nullptr, 0));
							}
							else if( UTag == "DEX" )
							{
								cwmWorldState->skill[i].dexterity = static_cast<UI16>(std::stoul(data, nullptr, 0));
							}
							else if( UTag == "INT" )
							{
								cwmWorldState->skill[i].intelligence = static_cast<UI16>(std::stoul(data, nullptr, 0));
							}
							else if( UTag == "SKILLPOINT" )
							{
								advance_st tempAdvance;
								data = strutil::simplify( data );
								auto csecs = strutil::sections( data, "," );
								tempAdvance.base	= static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0));
								tempAdvance.success = static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0));
								tempAdvance.failure = static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[2] ), nullptr, 0));
								if( csecs.size() == 4 )
								{
									tempAdvance.amtToGain = static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[3] ), nullptr, 0));
								}
								cwmWorldState->skill[i].advancement.push_back( tempAdvance );
							}
							else if( UTag == "MADEWORD" )
							{
								cwmWorldState->skill[i].madeword = data;
							}
							else if( UTag == "NAME" )
							{
								cwmWorldState->skill[i].name = data;
							}
							else
							{
								Console.warning( strutil::format("Unknown tag in skills.dfn: %s", data.c_str() ));
							}
						}
					}
				}
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadSpawnRegions( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads spawning regions from definition files
//o-----------------------------------------------------------------------------------------------o
void LoadSpawnRegions( void )
{
	cwmWorldState->spawnRegions.clear();
	UI16 i					= 0;
	for( Script *spnScp = FileLookup->FirstScript( spawn_def ); !FileLookup->FinishedScripts( spawn_def ); spnScp = FileLookup->NextScript( spawn_def ) )
	{
		if( spnScp == nullptr )
			continue;
		for( ScriptSection *toScan = spnScp->FirstEntry(); toScan != nullptr; toScan = spnScp->NextEntry() )
		{
			if( toScan == nullptr )
				continue;
			std::string sectionName = spnScp->EntryName();
			auto ssecs = strutil::sections( sectionName, " " );
			if( "REGIONSPAWN" == ssecs[0] ) // Is it a region spawn entry?
			{
				i = static_cast<UI16>(std::stoul(strutil::stripTrim( ssecs[1] ), nullptr, 0));
				if( cwmWorldState->spawnRegions.find( i ) == cwmWorldState->spawnRegions.end() )
				{
					cwmWorldState->spawnRegions[i] = new CSpawnRegion( i );
					cwmWorldState->spawnRegions[i]->Load( toScan );
				}
				else
				{
					Console.warning( strutil::format("spawn.dfn has a duplicate REGIONSPAWN entry, Entry Number: %u", i) );
				}
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadRegions( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Load regions from regions.dfn and townregions from regions.wsc
//o-----------------------------------------------------------------------------------------------o
void LoadRegions( void )
{
	cwmWorldState->townRegions.clear();
	std::string regionsFile = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "regions.wsc";
	bool performLoad		= false;
	Script *ourRegions		= nullptr;
	if( FileExists( regionsFile ) )
	{
		performLoad = true;
		ourRegions = new Script( regionsFile, NUM_DEFS, false );
	}

	UI16 i = 0;
	std::string regEntry;
	for( Script *regScp = FileLookup->FirstScript( regions_def ); !FileLookup->FinishedScripts( regions_def ); regScp = FileLookup->NextScript( regions_def ) )
	{
		if( regScp == nullptr )
			continue;

		for( ScriptSection *toScan = regScp->FirstEntry(); toScan != nullptr; toScan = regScp->NextEntry() )
		{
			if( toScan == nullptr )
				continue;

			regEntry = regScp->EntryName();
			auto ssecs = strutil::sections( regEntry, " " );
			if( strutil::stripTrim( ssecs[0] ) == "REGION" )
			{
				i = static_cast<UI16>(std::stoul(strutil::stripTrim( ssecs[1] ), nullptr, 0));
				if( cwmWorldState->townRegions.find( i ) == cwmWorldState->townRegions.end() )
				{
					cwmWorldState->townRegions[i] = new CTownRegion( i );
					cwmWorldState->townRegions[i]->InitFromScript( toScan );
					if( performLoad )
						cwmWorldState->townRegions[i]->Load( ourRegions );
				}
				else
				{
					Console.warning( strutil::format("regions.dfn has a duplicate REGION entry, Entry Number: %u", i) );
				}
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
		ourRegions = nullptr;
	}
	ScriptSection *InstaLog = FileLookup->FindEntry( "INSTALOG", regions_def );
	if( InstaLog == nullptr )
		return;
	LogoutLocationEntry toAdd;
	std::string data, UTag;
	for( std::string tag = InstaLog->First(); !InstaLog->AtEnd(); tag = InstaLog->Next() )
	{
		UTag = strutil::toupper( tag );
		data	= InstaLog->GrabData();
		data = strutil::stripTrim( data );
		if( UTag == "X1" )
		{
			toAdd.x1 = static_cast<SI16>(std::stoi(data, nullptr, 0));
		}
		else if( UTag == "Y1" )
		{
			toAdd.y1 = static_cast<SI16>(std::stoi(data, nullptr, 0));
		}
		else if( UTag == "X2" )
		{
			toAdd.x2 = static_cast<SI16>(std::stoi(data, nullptr, 0));
		}
		else if( UTag == "Y2" )
		{
			toAdd.y2 = static_cast<SI16>(std::stoi(data, nullptr, 0));
		}
		else if( UTag == "WORLD" )
		{
			toAdd.worldNum = static_cast<SI08>(std::stoi(data, nullptr, 0));
			cwmWorldState->logoutLocs.push_back( toAdd );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadTeleportLocations( void )
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
		Console.error(strutil::format(" Failed to open teleport data script %s", filename.c_str()) );
		Console.error(strutil::format( " Teleport Data not found") );
		cwmWorldState->SetKeepRun( false );
		cwmWorldState->SetError( true );
		return;
	}

	Script *teleportData = new Script( filename, NUM_DEFS, false );
	if( teleportData != nullptr )
	{
		cwmWorldState->teleLocs.reserve( teleportData->NumEntries() );

		UI16 tempX, tempY;
		SI08 tempZ;
		ScriptSection *teleportSect = nullptr;
		std::string tag, data, temp;
		for( teleportSect = teleportData->FirstEntry(); teleportSect != nullptr; teleportSect = teleportData->NextEntry() )
		{
			if( teleportSect != nullptr )
			{
				for( tag = teleportSect->First(); !teleportSect->AtEnd(); tag = teleportSect->Next() )
				{
					CTeleLocationEntry toAdd;
					if( strutil::toupper( tag ) == "ENTRY" )
					{
						tempX = 0;
						tempY = 0;
						tempZ = ILLEGAL_Z;
						data = strutil::simplify( teleportSect->GrabData() );
						auto csecs = strutil::sections( data, "," );
						SI32 sectCount = static_cast<SI32>(csecs.size());
						if( sectCount >= 6 )
						{
							tempX	= static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[0] ), nullptr, 0) );
							tempY	= static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[1] ), nullptr, 0) );
							temp	= strutil::toupper(strutil::stripTrim( csecs[2] ));
							if( temp != "ALL" && temp != "A" )
							{
								tempZ =  static_cast<UI16>(std::stoul(temp, nullptr, 0) );
							}
							toAdd.SourceLocation( tempX, tempY, tempZ );

							tempX	=  static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[3] ), nullptr, 0) );
							tempY	=  static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[4] ), nullptr, 0) );
							tempZ	=  static_cast<SI08>(std::stoi(strutil::stripTrim( csecs[5] ), nullptr, 0) );
							toAdd.TargetLocation( tempX, tempY, tempZ );

							if( sectCount >= 7 )
							{
								toAdd.SourceWorld(  static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[6] ), nullptr, 0) ));
								if( sectCount >= 8 )
								{
									toAdd.TargetWorld(  static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[7] ), nullptr, 0) ) );
								}
							}
							cwmWorldState->teleLocs.push_back( toAdd );
						}
						else
						{
							Console.error( "Insufficient parameters for teleport entry" );
						}
					}
				}
			}
		}
		std::sort(cwmWorldState->teleLocs.begin(), cwmWorldState->teleLocs.end(), [] ( CTeleLocationEntry  &one, CTeleLocationEntry  &two)
		{
			return static_cast<UI32>(one.SourceLocation().x) < static_cast<UI32>(two.SourceLocation().x);
		});
		delete teleportData;
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	LoadCreatures()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads creatures from creature definition files
//o-----------------------------------------------------------------------------------------------o
void LoadCreatures( void )
{
	std::string cEntry;
	std::string tag, data, UTag;
	UI16 i = 0;
	for( Script *creatScp = FileLookup->FirstScript( creatures_def ); !FileLookup->FinishedScripts( creatures_def ); creatScp = FileLookup->NextScript( creatures_def ) )
	{
		if( creatScp == nullptr )
			continue;

		for( ScriptSection *creatureData = creatScp->FirstEntry(); creatureData != nullptr; creatureData = creatScp->NextEntry() )
		{
			if( creatureData == nullptr )
				continue;

			cEntry = creatScp->EntryName();
			auto ssecs = strutil::sections( cEntry," " );
			if( ssecs[0] == "CREATURE" )
			{
				i =  static_cast<UI16>(std::stoul(strutil::stripTrim( ssecs[1] ), nullptr, 0) );

				for( tag = creatureData->First(); !creatureData->AtEnd(); tag = creatureData->Next() )
				{
					if( tag.empty() )
					{
						continue;
					}
					data = creatureData->GrabData();
					data = strutil::stripTrim( data );
					UTag = strutil::toupper( tag );
					switch( (UTag.data()[0]) )
					{
						case 'A':
							if( UTag == "ANTIBLINK" )
							{
								cwmWorldState->creatures[i].AntiBlink( true );
							}
							else if( UTag == "ANIMAL" )
							{
								cwmWorldState->creatures[i].IsAnimal( true );
							}
							break;
						case 'B':
							if( UTag == "BASESOUND" )
							{
								break;
							}
							break;
						case 'F':
							if( UTag == "FLIES" )
							{
								cwmWorldState->creatures[i].CanFly( true );
							}
							break;
						case 'H':
							if( UTag == "HUMAN" )
							{
								cwmWorldState->creatures[i].IsHuman( true );
							}
							break;
						case 'I':
							if( UTag == "ICON" )
							{
								cwmWorldState->creatures[i].Icon( static_cast<UI16>(std::stoul(data, nullptr, 0) ) );
							}
							break;
						case 'M':
							if( UTag == "MOVEMENT" )
							{
								if( strutil::toupper(data) == "WATER" )
								{
									cwmWorldState->creatures[i].IsWater( true );
								}
								else if( strutil::toupper(data) == "BOTH" )
								{
									cwmWorldState->creatures[i].IsAmphibian( true );
								}
								else
								{
									cwmWorldState->creatures[i].IsWater( false );
									cwmWorldState->creatures[i].IsAmphibian( false );
								}
							}
							else if( UTag == "MOUNTID" )
							{
								cwmWorldState->creatures[i].MountID( static_cast<UI16>(std::stoul(data, nullptr, 0) ) );
							}
							break;
						case 'S':
							if( UTag == "SOUNDFLAG" )
							{
								break;
							}
							else if( UTag == "SOUND_IDLE" )
							{
								cwmWorldState->creatures[i].SetSound( SND_IDLE, static_cast<UI16>(std::stoul(data, nullptr, 0) )  );
							}
							else if( UTag == "SOUND_STARTATTACK" )
							{
								cwmWorldState->creatures[i].SetSound( SND_STARTATTACK,static_cast<UI16>(std::stoul(data, nullptr, 0) )  );
							}
							else if( UTag == "SOUND_ATTACK" )
							{
								cwmWorldState->creatures[i].SetSound( SND_ATTACK, static_cast<UI16>(std::stoul(data, nullptr, 0) ) );
							}
							else if( UTag == "SOUND_DEFEND" )
							{
								cwmWorldState->creatures[i].SetSound( SND_DEFEND, static_cast<UI16>(std::stoul(data, nullptr, 0) )  );
							}
							else if( UTag == "SOUND_DIE" )
							{
								cwmWorldState->creatures[i].SetSound( SND_DIE, static_cast<UI16>(std::stoul(data, nullptr, 0) )  );
							}
							break;
					}
				}
			}
		}
	}

	FileLookup->Dispose( creatures_def );
}

void ReadWorldTagData( std::ifstream &inStream, std::string &tag, std::string &data )
{
	char temp[4097];
    tag = "o---o";
    data = "o---o";
    while( !inStream.eof() && !inStream.fail() )
    {
        inStream.getline(temp, 4096);
	    temp[inStream.gcount()] = 0;
		auto sLine = std::string( temp );
		auto cloc = sLine.find( "//" );

		if( cloc != std::string::npos )
		{
			sLine = sLine.substr( 0, cloc );
		}
		cloc = sLine.find_first_not_of( " \t\v\f\0" );
		if( cloc != std::string::npos )
		{
			if( cloc != std::string::npos )
			{
				auto temp2 = sLine.find_last_not_of( " \t\v\f\0" );
				sLine = sLine.substr( cloc, ( temp2 - cloc ) + 1 );
			}
		}

        if( !sLine.empty() )
        {
            if( sLine != "o---o" )
            {
                auto loc = sLine.find( "=" );
                if( loc!= std::string::npos )
                {
                    tag = sLine.substr( 0, loc );
                    auto temp = tag.find_first_not_of(" \t\v\f\0");
                    if( temp != std::string::npos )
					{
                        auto temp2 = tag.find_last_not_of( " \t\v\f\0" );
                        tag = tag.substr( temp, ( temp2 - temp ) + 1 );
                    }
                
                    if(( loc+1 ) < sLine.size() )
					{
                        data = sLine.substr( loc + 1 );
                        auto temp = data.find_first_not_of( " \t\v\f\0" );
                        if( temp != std::string::npos )
						{
                            auto temp2 = data.find_last_not_of( " \t\v\f\0" );
                            data = data.substr( temp, ( temp2 - temp ) + 1 );
                        }
                    }
                    else
					{
                        data = "";
                    }
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
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Load locations from location definition files
//o-----------------------------------------------------------------------------------------------o
void LoadPlaces( void )
{
	cwmWorldState->goPlaces.clear();
	std::string data, UTag, entryName;
	GoPlaces_st *toAdd		= nullptr;

	for( Script *locScp = FileLookup->FirstScript( location_def ); !FileLookup->FinishedScripts( location_def ); locScp = FileLookup->NextScript( location_def ) )
	{
		if( locScp == nullptr )
		{
			continue;
		}
		for( ScriptSection *toScan = locScp->FirstEntry(); toScan != nullptr; toScan = locScp->NextEntry() )
		{
			if( toScan == nullptr )
			{
				continue;
			}
			entryName			= locScp->EntryName();
			auto ssecs = strutil::sections( entryName, " " );
			
			size_t entryNum		= static_cast<UI32>(std::stoul(strutil::stripTrim( ssecs[1] ), nullptr, 0));
			
			if( (strutil::toupper(strutil::stripTrim( ssecs[0])) == "LOCATION") && entryNum )
			{
				if( cwmWorldState->goPlaces.find( static_cast<UI16>(entryNum) ) != cwmWorldState->goPlaces.end() )
				{
					Console.warning( strutil::format("Doubled up entry in Location.dfn (%u)", entryNum) );
				}
				toAdd = &cwmWorldState->goPlaces[static_cast<UI16>(entryNum)];
				if( toAdd != nullptr )
				{
					for( std::string tag = toScan->First(); !toScan->AtEnd(); tag = toScan->Next() )
					{
						data = toScan->GrabData();
						data = strutil::stripTrim( data );
						UTag = strutil::toupper( tag );
						if( UTag == "X" )
						{
							toAdd->x = static_cast<SI16>(std::stoi(data, nullptr, 0));
						}
						else if( UTag == "Y" )
						{
							toAdd->y = static_cast<SI16>(std::stoi(data, nullptr, 0));
						}
						else if( UTag == "Z" )
						{
							toAdd->z = static_cast<SI08>(std::stoi(data, nullptr, 0));
						}
						else if( UTag == "WORLD" )
						{
							toAdd->worldNum = static_cast<UI16>(std::stoul(data, nullptr, 0));
						}
						else if( UTag == "INSTANCEID" )
						{
							toAdd->instanceID = static_cast<UI16>(std::stoul(data, nullptr, 0));
						}
						else if( UTag == "LOCATION" )
						{
							auto csecs = strutil::sections( data, "," );
							size_t sectionCount = csecs.size() + 1;
							if( sectionCount >= 3 )
							{
								toAdd->x		= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[0] ), nullptr, 0));
								toAdd->y		= static_cast<SI16>(std::stoi(strutil::stripTrim( csecs[1] ), nullptr, 0));
								toAdd->z		= static_cast<SI08>(std::stoi(strutil::stripTrim( csecs[2] ), nullptr, 0));
								toAdd->worldNum = static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[3] ), nullptr, 0));
							}

							if( sectionCount == 4 )
							{
								toAdd->instanceID = static_cast<UI16>(std::stoul(strutil::stripTrim( csecs[4] ), nullptr, 0));
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
