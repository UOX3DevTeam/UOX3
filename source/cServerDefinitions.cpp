#include "uox3.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "scriptc.h"
#include <filesystem>
#include "StringUtility.hpp"
#include "IP4Address.hpp"

CServerDefinitions *FileLookup;

std::string CurrentWorkingDir( void )
{
	return std::filesystem::current_path().string();
}

std::string BuildPath( const std::string &extra )
{
	std::string temp = CurrentWorkingDir();
	temp += "/" + extra;
	return temp;
}

std::string ShortDirectory( std::string sPath )
{
	std::string building = "";
	size_t iFound = 0xFFFFFFFF;
	for( iFound = sPath.length() - 1; iFound >= 0 && iFound < sPath.length(); --iFound )
	{
		if( sPath[iFound] == '/' )
			break;
	}
	if( iFound >= sPath.length() )
		return sPath;
	else
	{
		for( size_t i = iFound + 1; i < sPath.length(); ++i )
			building += sPath[i];
	}
	return building;
}

const std::string dirnames[NUM_DEFS] =
{
	"items",
	"npc",
	"create",
	"regions",
	"misc",
	"skills",
	"location",
	"menus",
	"spells",
	"newbie",
	"titles",
	"advance",
	"house",
	"colors",
	"spawn",
	"html",
	"race",
	"weather",
	"harditems",
	"command",
	"msgboard",
	"carve",
	"creatures",
	"maps"
};

std::multimap<UI32,ADDMENUITEM> g_mmapAddMenuMap;

CServerDefinitions::CServerDefinitions() : defaultPriority( 0 )
{
	// Load our device ips
	IP4Address::loadIPs();
	Console.PrintSectionBegin();
	Console << "Loading server scripts..." << myendl;
	Console << "   o Clearing AddMenuMap entries(" << static_cast<UI64>(g_mmapAddMenuMap.size()) << ")" << myendl;
	g_mmapAddMenuMap.clear();
	ScriptListings.resize( NUM_DEFS );
	ReloadScriptObjects();
	Console.PrintSectionBegin();
}

CServerDefinitions::CServerDefinitions( const char *indexfilename ) : defaultPriority( 0 )
{
	// Load our device ips
	IP4Address::loadIPs();
	
	Console.PrintSectionBegin();
	Console << "Loading server scripts...." << myendl;

	ScriptListings.resize( NUM_DEFS );
	ReloadScriptObjects();

	Console.PrintSectionBegin();
}


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool Reload( void )
//|	Date		-	04/17/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Reload the dfn files.
//|	Changes		-	04042004 - Added the code to clear out the
//|									Auto-AddMenu items so there isn't any duplication in the
//|									multimap
//o-----------------------------------------------------------------------------------------------o
bool CServerDefinitions::Reload( void )
{
	// We need to clear out the AddMenuItem Map
	g_mmapAddMenuMap.clear();
	//
	Cleanup();
	ScriptListings.clear();
	ScriptListings.resize( NUM_DEFS );
	ReloadScriptObjects();
	return true;
}

void CServerDefinitions::Cleanup( void )
{
	std::vector< VECSCRIPTLIST >::iterator slIter;
	for( slIter = ScriptListings.begin(); slIter != ScriptListings.end(); ++slIter )
	{
		VECSCRIPTLIST& toDel = (*slIter);
		for( size_t j = 0; j < toDel.size(); ++j )
		{
			if( toDel[j] == nullptr )
				continue;

			delete toDel[j];
			toDel[j] = nullptr;
		}
	}
}

CServerDefinitions::~CServerDefinitions()
{
	Cleanup();
}

bool CServerDefinitions::Dispose( DEFINITIONCATEGORIES toDispose )
{
	bool retVal = false;
	if( toDispose != NUM_DEFS )
	{
		VECSCRIPTLIST& toDel = ScriptListings[toDispose];
		for( VECSCRIPTLIST_CITERATOR dIter = toDel.begin(); dIter != toDel.end(); ++dIter )
		{
			Script *toDelete = (*dIter);
			if( toDelete != nullptr )
			{
				retVal = true;
				delete toDelete;
			}
		}
		toDel.clear();
	}
	return retVal;
}

ScriptSection *CServerDefinitions::FindEntry( std::string toFind, DEFINITIONCATEGORIES typeToFind )
{
	ScriptSection *rvalue = nullptr;

	if( !toFind.empty() && typeToFind != NUM_DEFS )
	{
		auto tUFind = strutil::toupper( toFind );

		VECSCRIPTLIST& toDel = ScriptListings[typeToFind];
		for( VECSCRIPTLIST_CITERATOR dIter = toDel.begin(); dIter != toDel.end(); ++dIter )
		{
			Script *toCheck = (*dIter);
			if( toCheck != nullptr )
			{
				rvalue = toCheck->FindEntry( tUFind );
				if( rvalue != nullptr )
					break;
			}
		}
	}
	return rvalue;
}

ScriptSection *CServerDefinitions::FindEntrySubStr( std::string toFind, DEFINITIONCATEGORIES typeToFind )
{
	ScriptSection *rvalue = nullptr;
	if( !toFind.empty() && typeToFind != NUM_DEFS )
	{
		VECSCRIPTLIST& toDel = ScriptListings[typeToFind];
		for( VECSCRIPTLIST_CITERATOR dIter = toDel.begin(); dIter != toDel.end(); ++dIter )
		{
			Script *toCheck = (*dIter);
			if( toCheck != nullptr )
			{
				rvalue = toCheck->FindEntrySubStr( toFind );
				if( rvalue != nullptr )
					break;
			}
		}
	}
	return rvalue;
}

const std::string defExt = ".dfn";

struct PrioScan
{
	std::string	filename;
	SI16	priority;
	PrioScan() : filename( "" ), priority( 0 )
	{
	}
	PrioScan( const char *toUse, SI16 mPrio ) : filename( toUse ), priority( mPrio )
	{
	}
};

inline bool operator==( const PrioScan& x, const PrioScan& y )
{
	return ( x.priority == y.priority );
}

inline bool operator<( const PrioScan& x, const PrioScan& y )
{
	return ( x.priority < y.priority );
}

inline bool operator>( const PrioScan& x, const PrioScan& y )
{
	return ( x.priority > y.priority );
}

void CServerDefinitions::LoadDFNCategory( DEFINITIONCATEGORIES toLoad )
{
	CleanPriorityMap();
	defaultPriority = 0;
	UI08 wasPriod = 2;
	BuildPriorityMap( toLoad, wasPriod );

	cDirectoryListing fileList( toLoad, defExt );
	fileList.Flatten( true );
	STRINGLIST *shortListing	= fileList.FlattenedShortList();
	STRINGLIST *longListing		= fileList.FlattenedList();

	std::vector< PrioScan >	mSort;
	for( size_t i = 0; i < shortListing->size(); ++i )
	{
		mSort.push_back( PrioScan( (*longListing)[i].c_str(), GetPriority( (*shortListing)[i].c_str() ) ) );
	}
	if( !mSort.empty() )
	{
		std::sort( mSort.begin(), mSort.end() );
		Console.print( strutil::format("Section %20s : %6i", dirnames[toLoad].c_str(), 0 ));
		size_t iTotal = 0;
		Console.TurnYellow();

		std::vector< PrioScan >::const_iterator mIter;
		for( mIter = mSort.begin(); mIter != mSort.end(); ++mIter )
		{
			Console.print( "\b\b\b\b\b\b" );
			ScriptListings[toLoad].push_back( new Script( (*mIter).filename, toLoad, false ) );
			iTotal += ScriptListings[toLoad].back()->NumEntries();
			Console.print( strutil::format("%6i", iTotal) );
		}

		Console.print( strutil::format("\b\b\b\b\b\b%6i", CountOfEntries( toLoad )) );
		Console.TurnNormal();
		Console.print( " entries" );
		switch( wasPriod )
		{
			case 0:	Console.PrintSpecial( CGREEN,	"prioritized" );					break;	// prioritized
			case 1:
				Console.PrintSpecial( CGREEN,		"done" );
				break;	// file exist, no section
			default:
			case 2:
				Console.PrintSpecial( CBLUE,	"done" );
				break;	// no file
		};
	}
}

void CServerDefinitions::ReloadScriptObjects( void )
{
	Console << myendl;

	for( SI32 sCtr = 0; sCtr < NUM_DEFS; ++sCtr )
	{
		LoadDFNCategory( (DEFINITIONCATEGORIES)sCtr );
	}
	CleanPriorityMap();
}

size_t CServerDefinitions::CountOfEntries( DEFINITIONCATEGORIES typeToFind )
{
	size_t sumEntries = 0;
	VECSCRIPTLIST *toScan = &(ScriptListings[typeToFind]);
	if( toScan == nullptr )
		return 0;

	for( VECSCRIPTLIST_CITERATOR cIter = toScan->begin(); cIter != toScan->end(); ++cIter )
		sumEntries += (*cIter)->NumEntries();
	return sumEntries;
}

size_t CServerDefinitions::CountOfFiles( DEFINITIONCATEGORIES typeToFind )
{
	return ScriptListings[typeToFind].size();
}

Script * CServerDefinitions::FirstScript( DEFINITIONCATEGORIES typeToFind )
{
	Script *retScript = nullptr;
	slIter = ScriptListings[typeToFind].begin();
	if( !FinishedScripts( typeToFind ) )
		retScript = (*slIter);
	return retScript;
}
Script * CServerDefinitions::NextScript( DEFINITIONCATEGORIES typeToFind )
{
	Script *retScript = nullptr;
	if( !FinishedScripts( typeToFind ) )
	{
		++slIter;
		if( !FinishedScripts( typeToFind ) )
			retScript = (*slIter);
	}
	return retScript;
}
bool CServerDefinitions::FinishedScripts( DEFINITIONCATEGORIES typeToFind )
{
	return ( slIter == ScriptListings[typeToFind].end() );
}

void CServerDefinitions::CleanPriorityMap( void )
{
	priorityMap.clear();
}
void CServerDefinitions::BuildPriorityMap( DEFINITIONCATEGORIES category, UI08& wasPrioritized )
{
	cDirectoryListing priorityFile( category, "priority.nfo", false );
	STRINGLIST *longList = priorityFile.List();
	if( !longList->empty() )
	{
		std::string filename = (*longList)[0];
		//	Do we have any priority informat?
		if( FileExists( filename ) )	// the file exists, so perhaps we do
		{
			Script *prio = new Script( filename, category, false );	// generate a script for it
			if( prio != nullptr )	// successfully made a script
			{
				std::string tag;
				std::string data;
				ScriptSection *prioInfo = prio->FindEntry( "PRIORITY" );	// find the priority entry
				if( prioInfo != nullptr )
				{
					for( tag = prioInfo->First(); !prioInfo->AtEnd(); tag = prioInfo->Next() )	// keep grabbing priority info
					{
						data = prioInfo->GrabData();
						if( strutil::toupper( tag ) == "DEFAULTPRIORITY" )
						{
							defaultPriority = static_cast<UI16>(std::stoul(data, nullptr, 0));
						}
						else
						{
							std::string filenametemp = strutil::tolower( tag );
							priorityMap[filenametemp] =static_cast<SI16>(std::stoi(data, nullptr, 0));
						}
					}
					wasPrioritized = 0;
				}
				else
					wasPrioritized = 1;
				delete prio;	// remove script
				prio = nullptr;
			}
			else
				wasPrioritized = 2;
			return;
		}
	}
#if defined( UOX_DEBUG_MODE )
	//	Console.warning( strutil::format( "Failed to open priority.nfo for reading in %s DFN", dirnames[category].c_str() ));
#endif
	wasPrioritized = 2;
}

void CServerDefinitions::DisplayPriorityMap( void )
{
	Console << "Dumping map... " << myendl;
	std::map< std::string, SI16 >::const_iterator p = priorityMap.begin();
	while( p != priorityMap.end() )
	{
		Console << p->first << " : " << p->second << myendl;
		++p;
	}
	Console << "Dumped" << myendl;
}

SI16 CServerDefinitions::GetPriority( const char *file )
{
	SI16 retVal = defaultPriority;
	auto lowername = strutil::tolower( file );
	std::map< std::string, SI16 >::const_iterator p = priorityMap.find( lowername );
	if( p != priorityMap.end() )
		retVal = p->second;
	return retVal;
}

bool cDirectoryListing::PushDir( DEFINITIONCATEGORIES toMove )
{
	std::string filePath	= cwmWorldState->ServerData()->Directory( CSDDP_DEFS );
	filePath				+= dirnames[toMove];
	return PushDir( filePath );
}
bool cDirectoryListing::PushDir( std::string toMove )
{
	std::string cwd = CurrentWorkingDir();
	dirs.push( cwd );
	auto path = std::filesystem::path( toMove );

	if( std::filesystem::exists( path ) )
	{
		std::filesystem::current_path( path );
		currentDir = toMove;
		toMove = strutil::replaceSlash( toMove );
		shortCurrentDir = ShortDirectory( toMove );
		return true;
	}
	else
	{
		Console.error(strutil::format( "DFN directory %s does not exist", toMove.c_str()) );
		Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
	}
	return false;
}
void cDirectoryListing::PopDir( void )
{
	if( dirs.empty() )
	{
		Console.error( "cServerDefinition::PopDir called, but dirs is empty" );
		Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
	}
	else
	{
		auto path = std::filesystem::path( dirs.top() );

		if( std::filesystem::exists( path ) )
		{
			std::filesystem::current_path( path );
		}
		dirs.pop();
	}
}
cDirectoryListing::cDirectoryListing( bool recurse ) : extension( ".dfn" ), doRecursion( recurse )
{
}
cDirectoryListing::cDirectoryListing( const std::string &dir, const std::string &extent, bool recurse ) : doRecursion( recurse )
{
	Extension( extent );
	Retrieve( dir );
}
cDirectoryListing::cDirectoryListing( DEFINITIONCATEGORIES dir, const std::string &extent, bool recurse ) : doRecursion( recurse )
{
	Extension( extent );
	Retrieve( dir );
}
cDirectoryListing::~cDirectoryListing()
{
	while( !dirs.empty() )
	{
		auto path = std::filesystem::path( dirs.top() );

		if( std::filesystem::exists( path ))
		{
			std::filesystem::current_path( path );
		}
		dirs.pop();
	}
}

void cDirectoryListing::Retrieve( const std::string &dir )
{
	bool dirSet = PushDir( dir );
	InternalRetrieve();
	if( dirSet )
		PopDir();
}
void cDirectoryListing::Retrieve( DEFINITIONCATEGORIES dir )
{
	bool dirSet = PushDir( dir );
	InternalRetrieve();
	if( dirSet )
		PopDir();
}

STRINGLIST *cDirectoryListing::List( void )
{
	return &filenameList;
}

STRINGLIST *cDirectoryListing::ShortList( void )
{
	return &shortList;
}

STRINGLIST *cDirectoryListing::FlattenedList( void )
{
	return &flattenedFull;
}

STRINGLIST *cDirectoryListing::FlattenedShortList( void )
{
	return &flattenedShort;
}

void cDirectoryListing::InternalRetrieve( void )
{
	std::string filePath;
	auto path = std::filesystem::current_path();
	for( const auto& entry : std::filesystem::directory_iterator( path ))
	{
		auto name =  entry.path().filename().string();
		filePath = entry.path().string();
		auto ext = entry.path().extension();
		if( std::filesystem::is_regular_file( entry ))
		{
			if( !extension.empty() )
			{
				if( entry.path().extension().string() != extension )
				{
					name = "";
				}
			}
			if( !name.empty() )
			{
				shortList.push_back( name );
				filenameList.push_back( filePath );
			}
		}
		else if( std::filesystem::is_directory(entry) && doRecursion )
		{
			subdirectories.push_back( cDirectoryListing( name, extension, doRecursion ));
		}
	}
}

void cDirectoryListing::Extension( const std::string &extent )
{
	extension = extent;
}

void cDirectoryListing::Flatten( bool isParent )
{
	ClearFlatten();
	std::string temp;
	STRINGLIST_ITERATOR sIter;
	for( sIter = filenameList.begin(); sIter != filenameList.end(); ++sIter )
	{
		flattenedFull.push_back( (*sIter) );
		if( isParent )
			temp = "";
		else
		{
			temp = shortCurrentDir;
			temp += "/";
		}
		temp += (*sIter);
		flattenedShort.push_back( temp );
	}
	DIRLIST_ITERATOR dIter;
	for( dIter = subdirectories.begin(); dIter != subdirectories.end(); ++dIter )
	{
		(*dIter).Flatten( false );
		STRINGLIST *shortFlat	= (*dIter).FlattenedShortList();
		STRINGLIST *longFlat	= (*dIter).FlattenedList();
		for( size_t k = 0; k < longFlat->size(); ++k )
		{
			flattenedFull.push_back( (*longFlat)[k] );
			if( isParent )
				temp = "";
			else
			{
				temp = shortCurrentDir;
				temp += "/";
			}
			temp += (*shortFlat)[k];
			flattenedShort.push_back( temp );
		}
		(*dIter).ClearFlatten();
	}
}
void cDirectoryListing::ClearFlatten( void )
{
	flattenedFull.clear();
	flattenedShort.clear();
}
