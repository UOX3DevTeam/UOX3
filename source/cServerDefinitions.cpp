#include "uox3.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "scriptc.h"
#include "StringUtility.hpp"

#include <filesystem>
#include <memory>
#include <array>

using namespace std::string_literals;

CServerDefinitions *FileLookup;
//==================================================================================================
auto CurrentWorkingDir() -> std::string
{
	return std::filesystem::current_path().string();
}

//==================================================================================================
auto BuildPath( const std::string &extra ) -> std::string
{
	auto temp = std::filesystem::current_path();
	temp /= std::filesystem::path( extra );
	return temp.string();
}

//==================================================================================================
auto ShortDirectory( std::string sPath ) -> std::string
{
	return std::filesystem::path( sPath ).filename().string();
}

//==================================================================================================
const std::array<std::string, NUM_DEFS> dirNames
{
	"items"s,
	"npc"s,
	"create"s,
	"regions"s,
	"misc"s,
	"skills"s,
	"location"s,
	"menus"s,
	"spells"s,
	"newbie"s,
	"titles"s,
	"advance"s,
	"house"s,
	"colors"s,
	"spawn"s,
	"html"s,
	"race"s,
	"weather"s,
	"harditems"s,
	"command"s,
	"msgboard"s,
	"carve"s,
	"creatures"s,
	"maps"s
};

std::multimap<UI32, ADDMENUITEM> g_mmapAddMenuMap;

//==================================================================================================
CServerDefinitions::CServerDefinitions() : defaultPriority( 0 )
{	
}
//==================================================================================================
auto CServerDefinitions::Startup() -> void
{
	Console.PrintSectionBegin();
	Console << "Loading server scripts..." << myendl;
	Console << "   o Clearing AddMenuMap entries(" << static_cast<UI64>( g_mmapAddMenuMap.size() ) << ")" << myendl;
	g_mmapAddMenuMap.clear();
	ScriptListings.resize( NUM_DEFS );
	ReloadScriptObjects();
	Console.PrintSectionBegin();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerDefinitions::Reload()
//|	Date		-	04/17/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reload the dfn files.
//|	Changes		-	04042004 - Added the code to clear out the
//|									Auto-AddMenu items so there isn't any duplication in the
//|									multimap
//o------------------------------------------------------------------------------------------------o
auto CServerDefinitions::Reload() -> bool
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

//==================================================================================================
auto CServerDefinitions::Cleanup() -> void
{
	std::vector<VECSCRIPTLIST>::iterator slIter;
	for( slIter = ScriptListings.begin(); slIter != ScriptListings.end(); ++slIter )
	{
		VECSCRIPTLIST& toDel = ( *slIter );
		for( size_t j = 0; j < toDel.size(); ++j )
		{
			if( toDel[j] )
			{
				delete toDel[j];
				toDel[j] = nullptr;
			}
		}
	}
}

//==================================================================================================
CServerDefinitions::~CServerDefinitions()
{
	Cleanup();
}

//==================================================================================================
auto CServerDefinitions::Dispose( DEFINITIONCATEGORIES toDispose ) -> bool
{
	bool retVal = false;
	if( toDispose != NUM_DEFS )
	{
		VECSCRIPTLIST& toDel = ScriptListings[toDispose];
		for( VECSCRIPTLIST_CITERATOR dIter = toDel.begin(); dIter != toDel.end(); ++dIter )
		{
			Script *toDelete = ( *dIter );
			if( toDelete )
			{
				retVal = true;
				delete toDelete;
			}
		}
		toDel.clear();
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerDefinitions::FindEntry
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find a specific CScriptSection from the DFNs in memory
//o------------------------------------------------------------------------------------------------o
auto CServerDefinitions::FindEntry( const std::string &toFind, DEFINITIONCATEGORIES typeToFind ) ->CScriptSection *
{
	CScriptSection *rValue = nullptr;

	if( !toFind.empty() && typeToFind != NUM_DEFS )
	{
		auto tUFind = oldstrutil::upper( toFind );

		VECSCRIPTLIST& toDel = ScriptListings[typeToFind];
		for( VECSCRIPTLIST_CITERATOR dIter = toDel.begin(); dIter != toDel.end(); ++dIter )
		{
			Script *toCheck = ( *dIter );
			if( toCheck )
			{
				rValue = toCheck->FindEntry( tUFind );
				if( rValue )
				{
					break;
				}
			}
		}
	}
	return rValue;
}

//==================================================================================================
auto CServerDefinitions::FindEntrySubStr( const std::string &toFind, DEFINITIONCATEGORIES typeToFind ) ->CScriptSection *
{
	CScriptSection *rValue = nullptr;
	if( !toFind.empty() && typeToFind != NUM_DEFS )
	{
		VECSCRIPTLIST& toDel = ScriptListings[typeToFind];
		for( VECSCRIPTLIST_CITERATOR dIter = toDel.begin(); dIter != toDel.end(); ++dIter )
		{
			Script *toCheck = ( *dIter );
			if( toCheck )
			{
				rValue = toCheck->FindEntrySubStr( toFind );
				if( rValue )
				{
					break;
				}
			}
		}
	}
	return rValue;
}

const std::string defExt = ".dfn";

//==================================================================================================
struct PrioScan_st
{
	std::string	filename;
	SI16	priority;
	PrioScan_st() : filename( "" ), priority( 0 )
	{
	}
	PrioScan_st( const char *toUse, SI16 mPrio ) : filename( toUse ), priority( mPrio )
	{
	}
};

//==================================================================================================
inline auto operator == ( const PrioScan_st& x, const PrioScan_st& y ) -> bool
{
	return ( x.priority == y.priority );
}

//==================================================================================================
inline auto operator < ( const PrioScan_st& x, const PrioScan_st& y ) -> bool
{
	return ( x.priority < y.priority );
}

//==================================================================================================
inline auto operator > ( const PrioScan_st& x, const PrioScan_st& y ) -> bool
{
	return ( x.priority > y.priority );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerDefinitions::LoadDFNCategory
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load the files found for each DFN category
//o------------------------------------------------------------------------------------------------o
auto CServerDefinitions::LoadDFNCategory( DEFINITIONCATEGORIES toLoad) -> void
{
	CleanPriorityMap();
	defaultPriority = 0;
	UI08 wasPriod = 2;
	BuildPriorityMap( toLoad, wasPriod );

	CDirectoryListing fileList( toLoad, defExt );
	fileList.Flatten( true );
	std::vector<std::string> *shortListing	= fileList.FlattenedShortList();
	std::vector<std::string> *longListing		= fileList.FlattenedList();

	std::vector<PrioScan_st>	mSort;
	for( size_t i = 0; i < shortListing->size(); ++i )
	{
		mSort.push_back( PrioScan_st(( *longListing )[i].c_str(), GetPriority(( *shortListing )[i].c_str() )));
	}
	if( !mSort.empty() )
	{
		std::sort( mSort.begin(), mSort.end() );
		Console.Print( oldstrutil::format( "Section %20s : %6i", dirNames[toLoad].c_str(), 0 ));
		size_t iTotal = 0;
		Console.TurnYellow();

		std::vector<PrioScan_st>::const_iterator mIter;
		for( mIter = mSort.begin(); mIter != mSort.end(); ++mIter )
		{
			Console.Print( "\b\b\b\b\b\b" );
			ScriptListings[toLoad].push_back( new Script(( *mIter ).filename, toLoad, false ));
			iTotal += ScriptListings[toLoad].back()->NumEntries();
			Console.Print( oldstrutil::format( "%6i", iTotal ));
		}

		Console.Print( oldstrutil::format( "\b\b\b\b\b\b%6i", CountOfEntries( toLoad )));
		Console.TurnNormal();
		Console.Print( " entries" );
		switch( wasPriod )
		{
			case 0:
				Console.PrintSpecial( CGREEN, "prioritized" );
				break;	// prioritized
			case 1:
				Console.PrintSpecial( CGREEN, "done" );
				break;	// file exist, no section
			default:
			case 2:
				Console.PrintSpecial( CBLUE, "done" );
				break;	// no file
		};
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerDefinitions::ReloadScriptObjects()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reload all the various categories of DFN scripts
//o------------------------------------------------------------------------------------------------o
auto CServerDefinitions::ReloadScriptObjects() -> void
{
	Console << myendl;

	for( SI32 sCtr = 0; sCtr < NUM_DEFS; ++sCtr )
	{
		LoadDFNCategory( static_cast<DEFINITIONCATEGORIES>( sCtr ));
	}
	CleanPriorityMap();
}

//==================================================================================================
auto CServerDefinitions::CountOfEntries( DEFINITIONCATEGORIES typeToFind ) -> size_t
{
	size_t sumEntries = 0;
	VECSCRIPTLIST *toScan = &( ScriptListings[typeToFind] );
	if( toScan )
	{
		for( auto cIter = toScan->begin(); cIter != toScan->end(); ++cIter )
		{
			sumEntries += (*cIter)->NumEntries();
		}
	}
	return sumEntries;
}

//==================================================================================================
auto CServerDefinitions::CountOfFiles( DEFINITIONCATEGORIES typeToFind ) -> size_t
{
	return ScriptListings[typeToFind].size();
}

//==================================================================================================
auto CServerDefinitions::FirstScript( DEFINITIONCATEGORIES typeToFind ) -> Script *
{
	Script *retScript = nullptr;
	slIter = ScriptListings[typeToFind].begin();
	if( !FinishedScripts( typeToFind ))
	{
		retScript = ( *slIter );
	}
	return retScript;
}
//==================================================================================================
auto CServerDefinitions::NextScript( DEFINITIONCATEGORIES typeToFind ) ->Script *
{
	Script *retScript = nullptr;
	if( !FinishedScripts( typeToFind ))
	{
		++slIter;
		if( !FinishedScripts( typeToFind ))
		{
			retScript = ( *slIter );
		}
	}
	return retScript;
}
//==================================================================================================
auto CServerDefinitions::FinishedScripts( DEFINITIONCATEGORIES typeToFind ) -> bool
{
	return ( slIter == ScriptListings[typeToFind].end() );
}

//==================================================================================================
auto CServerDefinitions::CleanPriorityMap() -> void
{
	priorityMap.clear();
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerDefinitions::BuildPriorityMap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Build DFN priority map based on entires from priority.nfo
//o------------------------------------------------------------------------------------------------o
auto CServerDefinitions::BuildPriorityMap( DEFINITIONCATEGORIES category, UI08& wasPrioritized ) -> void
{
	CDirectoryListing priorityFile( category, "priority.nfo", false );
	std::vector<std::string> *longList = priorityFile.List();
	if( !longList->empty() )
	{
		std::string filename = ( *longList )[0];
		//	Do we have any priority informat?
		if( FileExists( filename )) // the file exists, so perhaps we do
		{
			auto prio = std::make_unique<Script>( filename, category, false );	// generate a script for it
			if( prio ) // successfully made a script
			{
				auto prioInfo = prio->FindEntry( "PRIORITY" );	// find the priority entry
				if( prioInfo )
				{
					for( const auto &sec : prioInfo->collection() )
					{
						auto tag = sec->tag;
						auto data = sec->data;
						if( oldstrutil::upper( tag ) == "DEFAULTPRIORITY" )
						{
							defaultPriority = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
						}
						else
						{
							std::string filenametemp = oldstrutil::lower( tag );
							priorityMap[filenametemp] =static_cast<SI16>( std::stoi( data, nullptr, 0 ));
						}
					}
					wasPrioritized = 0;
				}
				else
				{
					wasPrioritized = 1;
				}
				prio = nullptr;
			}
			else
			{
				wasPrioritized = 2;
			}
			return;
		}
	}
#if defined( UOX_DEBUG_MODE )
	//	Console.Warning( oldstrutil::format( "Failed to open priority.nfo for reading in %s DFN", dirNames[category].c_str() ));
#endif
	wasPrioritized = 2;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CServerDefinitions::DisplayPriorityMap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dump DFN priority map to console
//o------------------------------------------------------------------------------------------------o
void CServerDefinitions::DisplayPriorityMap()
{
	Console << "Dumping map... " << myendl;
	for( auto &[name, priority] : priorityMap )
	{
		Console << name << " : " << priority << myendl;
	}
	Console << "Dumped" << myendl;
}

//==================================================================================================
auto CServerDefinitions::GetPriority( const char *file ) -> SI16
{
	auto retVal = defaultPriority;
	auto lowername = oldstrutil::lower( file );
	auto p = priorityMap.find( lowername );
	if( p != priorityMap.end() )
	{
		retVal = p->second;
	}
	return retVal;
}

//==================================================================================================
auto CDirectoryListing::PushDir( DEFINITIONCATEGORIES toMove ) -> bool
{
	auto filePath = cwmWorldState->ServerData()->Directory( CSDDP_DEFS );
	filePath += dirNames[toMove];
	return PushDir( filePath );
}
//==================================================================================================
auto CDirectoryListing::PushDir( std::string toMove ) -> bool
{
	std::string cwd = CurrentWorkingDir();
	dirs.push( cwd );
	auto path = std::filesystem::path( toMove );
	auto rValue = true;
	if( !std::filesystem::exists( path ))
	{
		Console.Error( oldstrutil::format( "DFN directory %s does not exist", toMove.c_str() ));
		Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
	}
	std::filesystem::current_path( path );
	currentDir = toMove;
	toMove = oldstrutil::replaceSlash( toMove );
	shortCurrentDir = ShortDirectory( toMove );
	return rValue;
}
//==================================================================================================
auto CDirectoryListing::PopDir() -> void
{
	if( dirs.empty() )
	{
		Console.Error( "cServerDefinition::PopDir called, but dirs is empty" );
		Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
	}
	else
	{
		auto path = std::filesystem::path( dirs.top() );

		if( std::filesystem::exists( path ))
		{
			std::filesystem::current_path( path );
		}
		dirs.pop();
	}
}
//==================================================================================================
CDirectoryListing::CDirectoryListing( bool recurse ) : extension( ".dfn" ), doRecursion( recurse )
{
}
//==================================================================================================
CDirectoryListing::CDirectoryListing( const std::string &dir, const std::string &extent, bool recurse ) : doRecursion( recurse )
{
	Extension( extent );
	Retrieve( dir );
}
//==================================================================================================
CDirectoryListing::CDirectoryListing( DEFINITIONCATEGORIES dir, const std::string &extent, bool recurse ) : doRecursion( recurse )
{
	Extension( extent );
	Retrieve( dir );
}
//==================================================================================================
CDirectoryListing::~CDirectoryListing()
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

//==================================================================================================
auto CDirectoryListing::Retrieve( const std::string &dir ) -> void
{
	bool dirSet = PushDir( dir );
	InternalRetrieve();
	if( dirSet )
	{
		PopDir();
	}
}
//==================================================================================================
auto CDirectoryListing::Retrieve( DEFINITIONCATEGORIES dir ) -> void
{
	bool dirSet = PushDir( dir );
	InternalRetrieve();
	if( dirSet )
	{
		PopDir();
	}
}

//==================================================================================================
auto CDirectoryListing::List() -> std::vector<std::string>*
{
	return &filenameList;
}

//==================================================================================================
auto CDirectoryListing::ShortList() -> std::vector<std::string>*
{
	return &shortList;
}

//==================================================================================================
auto CDirectoryListing::FlattenedList() -> std::vector<std::string>*
{
	return &flattenedFull;
}

//==================================================================================================
auto CDirectoryListing::FlattenedShortList() -> std::vector<std::string>*
{
	return &flattenedShort;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CDirectoryListing::InternalRetrieve()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	???
//o------------------------------------------------------------------------------------------------o
auto CDirectoryListing::InternalRetrieve() -> void
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
		else if( std::filesystem::is_directory( entry ) && doRecursion )
		{
			subdirectories.push_back( CDirectoryListing( name, extension, doRecursion ));
		}
	}
}

//==================================================================================================
auto CDirectoryListing::Extension( const std::string &extent ) -> void
{
	extension = extent;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CDirectoryListing::Flatten()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Flatten list of filenames
//o------------------------------------------------------------------------------------------------o
auto CDirectoryListing::Flatten( bool isParent ) -> void
{
	ClearFlatten();
	std::for_each( filenameList.begin(), filenameList.end(), [isParent, this]( const std::string & entry )
	{
		flattenedFull.push_back( entry );
		auto temp = std::string();
		if( !isParent )
		{
			temp = shortCurrentDir + "/"s;
		}
		temp += entry;
		flattenedShort.push_back( temp );
	});
	std::string temp;
	DIRLIST_ITERATOR dIter;
	for( dIter = subdirectories.begin(); dIter != subdirectories.end(); ++dIter )
	{
		( *dIter ).Flatten( false );
		auto shortFlat	= ( *dIter ).FlattenedShortList();
		auto longFlat	= ( *dIter ).FlattenedList();
		for( size_t k = 0; k < longFlat->size(); ++k )
		{
			flattenedFull.push_back(( *longFlat )[k] );
			if( isParent )
			{
				temp = "";
			}
			else
			{
				temp = shortCurrentDir;
				temp += "/";
			}
			temp += ( *shortFlat )[k];
			flattenedShort.push_back( temp );
		}
		( *dIter ).ClearFlatten();
	}
}

//==================================================================================================
auto CDirectoryListing::ClearFlatten() -> void
{
	flattenedFull.clear();
	flattenedShort.clear();
}
