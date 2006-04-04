#include "uox3.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "scriptc.h"

#if UOX_PLATFORM != PLATFORM_WIN32
	#include <dirent.h>
#else
	#include <direct.h>
#endif

#if UOX_PLATFORM == PLATFORM_WIN32
	#define getcwd _getcwd
#endif

namespace UOX
{

CServerDefinitions *FileLookup;

std::string CurrentWorkingDir( void )
{
	char cwd[MAX_PATH + 1];

	if( !getcwd( cwd, MAX_PATH + 1 ) )
	{
		Console.Error( "Failed to allocate enough room for cwd" );
		Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
	}
	return cwd;
}
std::string BuildPath( std::string extra )
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
	Console.PrintSectionBegin();
	Console << "Loading server scripts..." << myendl;
	Console << "   o Clearing AddMenuMap entries(" << g_mmapAddMenuMap.size() << ")" << myendl;
	g_mmapAddMenuMap.clear();
	ScriptListings.resize( NUM_DEFS );
	ReloadScriptObjects();
	Console.PrintSectionBegin();
}

CServerDefinitions::CServerDefinitions( const char *indexfilename ) : defaultPriority( 0 )
{
	Console.PrintSectionBegin();
	Console << "Loading server scripts...." << myendl;
	
	ScriptListings.resize( NUM_DEFS );
	ReloadScriptObjects();
	
	Console.PrintSectionBegin();
}


//o--------------------------------------------------------------------------o
//|	Function/Class	-	bool CServerDefinitions::Reload( void )
//|	Date			-	04/17/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Reload the dfn files.
//|	Modification	-	04042004 - EviLDeD - Added the code to clear out the 
//|									Auto-AddMenu items so there isn't any duplication in the
//|									multimap
//o--------------------------------------------------------------------------o
//|	Returns				-	[TRUE] if succesfull
//o--------------------------------------------------------------------------o	
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
			if( toDel[j] == NULL )
				continue;

			delete toDel[j];
			toDel[j] = NULL;
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
			if( toDelete != NULL )
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
	UString tUFind( toFind );
	tUFind = tUFind.upper();

	ScriptSection *rvalue = NULL;
	if( !toFind.empty() && typeToFind != NUM_DEFS )
	{
		VECSCRIPTLIST& toDel = ScriptListings[typeToFind];
		for( VECSCRIPTLIST_CITERATOR dIter = toDel.begin(); dIter != toDel.end(); ++dIter )
		{
			Script *toCheck = (*dIter);
			if( toCheck != NULL )
			{
				rvalue = toCheck->FindEntry( tUFind );
				if( rvalue != NULL )
					break;
			}
		}
	}
	return rvalue;
}

ScriptSection *CServerDefinitions::FindEntrySubStr( std::string toFind, DEFINITIONCATEGORIES typeToFind )
{
	ScriptSection *rvalue = NULL;
	if( !toFind.empty() && typeToFind != NUM_DEFS )
	{
		VECSCRIPTLIST& toDel = ScriptListings[typeToFind];
		for( VECSCRIPTLIST_CITERATOR dIter = toDel.begin(); dIter != toDel.end(); ++dIter )
		{
			Script *toCheck = (*dIter);
			if( toCheck != NULL )
			{
				rvalue = toCheck->FindEntrySubStr( toFind );
				if( rvalue != NULL )
					break;
			}
		}
	}
	return rvalue;
}

const std::string defExt = "*.dfn";

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
		Console.Print( "Section %20s : %6i", dirnames[toLoad].c_str(), 0 );
		size_t iTotal = 0;
		Console.TurnYellow();

		std::vector< PrioScan >::const_iterator mIter;
		for( mIter = mSort.begin(); mIter != mSort.end(); ++mIter )
		{
			Console.Print( "\b\b\b\b\b\b" );
			ScriptListings[toLoad].push_back( new Script( (*mIter).filename, toLoad, false ) );
			iTotal += ScriptListings[toLoad].back()->NumEntries();
			Console.Print( "%6i", iTotal );
		}

		Console.Print( "\b\b\b\b\b\b%6i", CountOfEntries( toLoad ) );
		Console.TurnNormal();
		Console.Print( " entries" );
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

	for( int sCtr = 0; sCtr < NUM_DEFS; ++sCtr )
	{
		LoadDFNCategory( (DEFINITIONCATEGORIES)sCtr );
	}
	CleanPriorityMap();
}

size_t CServerDefinitions::CountOfEntries( DEFINITIONCATEGORIES typeToFind )
{
	size_t sumEntries = 0;
	VECSCRIPTLIST *toScan = &(ScriptListings[typeToFind]);
	if( toScan == NULL )
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
	Script *retScript = NULL;
	slIter = ScriptListings[typeToFind].begin();
	if( !FinishedScripts( typeToFind ) )
		retScript = (*slIter);
	return retScript;
}
Script * CServerDefinitions::NextScript( DEFINITIONCATEGORIES typeToFind )
{
	Script *retScript = NULL;
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
	if( longList->size() > 0 )
	{
		std::string filename = (*longList)[0];
		//	Do we have any priority informat?
		if( FileExists( filename ) )	// the file exists, so perhaps we do
		{
			Script *prio = new Script( filename, category, false );	// generate a script for it
			if( prio != NULL )	// successfully made a script
			{
				UString tag;
				UString data;
				ScriptSection *prioInfo = prio->FindEntry( "PRIORITY" );	// find the priority entry
				if( prioInfo != NULL )
				{
					for( tag = prioInfo->First(); !prioInfo->AtEnd(); tag = prioInfo->Next() )	// keep grabbing priority info
					{
						data = prioInfo->GrabData();
						if( tag.upper() == "DEFAULTPRIORITY" )
							defaultPriority = data.toShort();
						else
						{
							std::string filenametemp = tag.lower();
							priorityMap[filenametemp] = data.toShort();
						}
					}
					wasPrioritized = 0;
				}
				else
					wasPrioritized = 1;
				delete prio;	// remove script
				prio = NULL;
			}
			else
				wasPrioritized = 2;
			return;
		}
	}
#if defined( UOX_DEBUG_MODE )
//	Console.Warning( "Failed to open priority.nfo for reading in %s DFN", dirnames[category].c_str() );
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
	UString lowername = UString( file ).lower();
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

	if( _chdir( toMove.c_str() ) == 0 )
	{
		currentDir = toMove;
		UString::replaceSlash( toMove );
		shortCurrentDir = ShortDirectory( toMove );
		return true;
	}
	else
	{
		Console.Error( "DFN directory %s does not exist", toMove.c_str() );
		Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
	}
	return false;
}
void cDirectoryListing::PopDir( void )
{
	if( dirs.empty() )
	{
		Console.Error( "cServerDefinition::PopDir called, but dirs is empty" );
		Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
	}
	else
	{
		_chdir( dirs.top().c_str() );
		dirs.pop();
	}
}
cDirectoryListing::cDirectoryListing( bool recurse ) : extension( "*.dfn" ), doRecursion( recurse )
{
}
cDirectoryListing::cDirectoryListing( std::string dir, std::string extent, bool recurse ) : doRecursion( recurse )
{
	Extension( extent );
	Retrieve( dir );
}
cDirectoryListing::cDirectoryListing( DEFINITIONCATEGORIES dir, std::string extent, bool recurse ) : doRecursion( recurse )
{
	Extension( extent );
	Retrieve( dir );
}
cDirectoryListing::~cDirectoryListing()
{
	while( !dirs.empty() )
	{
		_chdir( dirs.top().c_str() );
		dirs.pop();
	}
}

void cDirectoryListing::Retrieve( std::string dir )
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
	char filePath[512];

#if UOX_PLATFORM != PLATFORM_WIN32
	DIR *dir = opendir("."); 
	struct dirent *dirp = NULL; 
	struct stat dirstat; 

	while( ( dirp = readdir( dir ) ) ) 
	{ 
		stat( dirp->d_name, &dirstat ); 
		if( S_ISDIR( dirstat.st_mode ) ) 
		{ 
			if( strcmp( dirp->d_name, "." ) && strcmp( dirp->d_name, ".." ) && doRecursion ) 
			{ 
				subdirectories.push_back( cDirectoryListing( dirp->d_name, extension, doRecursion ) ); 
				Console.Print( "%s/%s/n", currentDir.c_str(), dirp->d_name ); 
			}
			continue; 
		}
		shortList.push_back( dirp->d_name ); 
		sprintf( filePath, "%s/%s", CurrentWorkingDir().c_str(), dirp->d_name ); 
		filenameList.push_back( filePath ); 
	} 

#else 

	WIN32_FIND_DATA toFind;
	HANDLE findHandle = FindFirstFile( extension.c_str(), &toFind );		// grab first file that meets spec
	BOOL retVal = 0;
	if( findHandle != INVALID_HANDLE_VALUE )	// there is a file
	{
		shortList.push_back( toFind.cFileName );
		sprintf( filePath, "%s/%s", currentDir.c_str(), toFind.cFileName );
		filenameList.push_back( filePath );
		retVal = 1;	// let's continue
	}
	while( retVal != 0 )	// while there are still files
	{
		retVal = FindNextFile( findHandle, &toFind );	// grab the next file
		if( retVal != 0 )
		{
			shortList.push_back( toFind.cFileName );
			sprintf( filePath, "%s/%s", currentDir.c_str(), toFind.cFileName );
			filenameList.push_back( filePath );
		}
	}
	if( findHandle != INVALID_HANDLE_VALUE )
	{
		FindClose( findHandle );
		findHandle = INVALID_HANDLE_VALUE;
	}
	if( doRecursion )
	{
		std::string temp;
		WIN32_FIND_DATA dirFind;
		HANDLE directoryHandle = FindFirstFile( "*.*", &dirFind );
		BOOL dirRetval = 0;

		if( directoryHandle != INVALID_HANDLE_VALUE )	// there is a file
		{
			if( ( dirFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY )
			{
				if( strcmp( dirFind.cFileName, "." ) && strcmp( dirFind.cFileName, ".." ) )
				{
					temp = BuildPath( dirFind.cFileName );
					subdirectories.push_back( cDirectoryListing( temp, extension, doRecursion ) );
				}
				// it's a directory
			}
			dirRetval = 1;	// let's continue
		}
		while( dirRetval != 0 )	// while there are still files
		{
			dirRetval = FindNextFile( directoryHandle, &dirFind );	// grab the next file
			if( dirRetval != 0 )
			{
				if( ( dirFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY )
				{
					if( strcmp( dirFind.cFileName, "." ) && strcmp( dirFind.cFileName, ".." ) )
					{
						temp = BuildPath( dirFind.cFileName );
						subdirectories.push_back( cDirectoryListing( temp, extension, doRecursion ) );
					}
					// it's a directory
				}
			}
		}
		if( directoryHandle != INVALID_HANDLE_VALUE )
		{
			FindClose( directoryHandle );
			directoryHandle = INVALID_HANDLE_VALUE;
		}
	}
#endif
}

void cDirectoryListing::Extension( std::string extent )
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

}
