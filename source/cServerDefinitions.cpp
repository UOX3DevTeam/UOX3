#include "uox3.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "scriptc.h"

#if defined(__unix__)
	#include <dirent.h>
#   include <unistd.h>
#else
	#include <direct.h>
#endif

#if !defined(__unix__)
	#define getcwd _getcwd
#endif

std::string CurrentWorkingDir( void )
{
	char cwd[MAX_PATH + 1];

	if( !getcwd( cwd, MAX_PATH + 1 ) )
	{
		Console.Error( 1, "Failed to allocate enough room for cwd" );
		Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
	}
	return cwd;
}
std::string BuildPath( std::string extra )
{
	std::string temp = CurrentWorkingDir();
	temp += "/";
	temp += extra;
	return temp;
}

void PathFix( char *sPath )
{
	// Loop through each char and replace the '\' to '/'
	for( unsigned int i = 0; i < strlen( sPath ); i++ )
	{
		if( sPath[i] == '\\' )
			sPath[i] = '/';
	}
}
void PathFix( std::string& sPath )
{
	// Loop through each char and replace the '\' to '/'
	for( unsigned int i = 0; i < sPath.length(); i++ )
	{
		if( sPath[i] == '\\' )
			sPath[i] = '/';
	}
}

std::string ShortDirectory( std::string sPath )
{
	std::string building = "";
	int iFound = 0;
	for( iFound = sPath.length() - 1; iFound >= 0; iFound-- )
	{
		if( sPath[iFound] == '/' )
			break;
	}
	if( iFound < 0 )
		return sPath;
	else
	{
		for( unsigned int i = iFound + 1; i < sPath.length(); i++ )
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
	"tracking",
	"newbie",
	"titles",
	"advance",
	"digging",
	"house",
	"colors",
	"spawn",
	"html",
	"race",
	"polymorph",
	"weather",
	"harditems",
	"command",
	"msgboard",
	"carve",
	"fishing"
};

cServerDefinitions::cServerDefinitions() : defaultPriority( 0 )
{
	Console.PrintSectionBegin();
	Console << "Loading server scripts..." << myendl;
	
	ScriptListings.resize( NUM_DEFS );
	ReloadScriptObjects();
	Console.PrintSectionBegin();
}

cServerDefinitions::cServerDefinitions( const char *indexfilename ) : defaultPriority( 0 )
{
	Console.PrintSectionBegin();
	Console << "Loading server scripts..." << myendl;
	
	ScriptListings.resize( NUM_DEFS );
	ReloadScriptObjects();
	
	Console.PrintSectionBegin();
}


//o--------------------------------------------------------------------------o
//|	Function/Class-	bool cServerDefinitions::Reload( void )
//|	Date					-	04/17/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Reload the dfn files.
//o--------------------------------------------------------------------------o
//|	Returns				-	[TRUE] if succesfull
//o--------------------------------------------------------------------------o	
bool cServerDefinitions::Reload( void )
{
	ScriptListings.clear();
	ScriptListings.resize( NUM_DEFS );
	ReloadScriptObjects();
	return true;
}

cServerDefinitions::~cServerDefinitions()
{
	for( UI32 i = 0; i < ScriptListings.size(); i++ )
	{
		for( UI32 j = 0; j < ScriptListings[i].size(); j++ )
		{
			VECSCRIPTLIST *toDel = &(ScriptListings[i]);
			if( toDel == NULL )
				continue;
			delete (*toDel)[j];
			(*toDel)[j] = NULL;
		}
	}
}

ScriptSection *cServerDefinitions::FindEntry( const char *toFind, DEFINITIONCATEGORIES typeToFind )
{
	if( toFind == NULL || typeToFind == NUM_DEFS )
		return NULL;
	VECSCRIPTLIST *toDel = &(ScriptListings[typeToFind]);
	if( toDel == NULL )
		return NULL;
	for( UI32 i = 0; i < toDel->size(); i++ )
	{
		if( (*toDel)[i] != NULL )
		{
			Script *toCheck = (*toDel)[i];
			if( toCheck == NULL )
				continue;
			ScriptSection *found = toCheck->FindEntry( toFind );
			if( found != NULL )
				return found;
		}
	}
	return NULL;
}

Script *cServerDefinitions::FindScript( const char *toFind, DEFINITIONCATEGORIES typeToFind )
{
	if( toFind == NULL || typeToFind == NUM_DEFS )
		return NULL;
	VECSCRIPTLIST *toDel = &(ScriptListings[typeToFind]);
	if( toDel == NULL )
		return NULL;
	for( UI32 i = 0; i < toDel->size(); i++ )
	{
		if( (*toDel)[i] != NULL )
		{
			Script *toCheck = (*toDel)[i];
			if( toCheck == NULL )
				continue;
			ScriptSection *found = toCheck->FindEntry( toFind );
			if( found != NULL )
				return toCheck;
		}
	}
	return NULL;
}

const std::string defExt = "*.dfn";

struct PrioScan
{
	std::string	filename;
	SI16	priority;
	PrioScan() : filename( "" ), priority( 0 ) { }
	PrioScan( const char *toUse, SI16 mPrio ) : filename( toUse ), priority( mPrio ) { }
};

void SortPrioVec( std::vector< PrioScan * > *toSort )
{
	if( toSort == NULL )
		return;
	for( SI32 i = 0; i < (SI32)(toSort->size() - 1); i++ )
	{
		for( SI32 j = i + 1; j < (SI32)toSort->size(); j++ )
		{
			if( (*toSort)[i]->priority < (*toSort)[j]->priority )
			{
				PrioScan *temp = (*toSort)[i];
				(*toSort)[i] = (*toSort)[j];
				(*toSort)[j] = temp;
			}
		}
	}
}

void cServerDefinitions::ReloadScriptObjects( void )
{
	Console << myendl;

	for( int sCtr = 0; sCtr < NUM_DEFS; sCtr++ )
	{
		CleanPriorityMap();
		defaultPriority = 0;
		UI08 wasPriod = 2;
		BuildPriorityMap( (DEFINITIONCATEGORIES)sCtr, wasPriod );
		cDirectoryListing fileList( (DEFINITIONCATEGORIES)sCtr, defExt );
		fileList.Flatten( true );
		std::vector< PrioScan * >	mSort;
		STRINGLIST *shortListing	= fileList.FlattenedShortList();
		STRINGLIST *longListing		= fileList.FlattenedList();
		for( UI32 i = 0; i < shortListing->size(); i++ )
		{
			const char *fullname	= (*longListing)[i].c_str();
			const char *shortname	= (*shortListing)[i].c_str();
			mSort.push_back( new PrioScan( fullname, GetPriority( shortname ) ) );
		}
		if( mSort.size() > 0 )
		{
			SortPrioVec( &mSort );
			Console.Print( "Section %20s : %6i", dirnames[sCtr].c_str(), 0 );
			UI32 iTotal = 0;
			Console.TurnYellow();
			for( UI32 iFile = 0; iFile < mSort.size(); iFile++ )
			{
				Console.Print( "\b\b\b\b\b\b" );
				ScriptListings[sCtr].push_back( new Script( mSort[iFile]->filename, (DEFINITIONCATEGORIES)sCtr, false ) );
				iTotal += (ScriptListings[sCtr])[iFile]->NumEntries();
				Console.Print( "%6i", iTotal );
				delete mSort[iFile];
				mSort[iFile] = NULL;
			}
			Console.Print( "\b\b\b\b\b\b%6i", CountOfEntries( (DEFINITIONCATEGORIES)sCtr ) );
			Console.Print( " entries" );
			switch( wasPriod )
			{
			case 0:	Console.PrintSpecial( CGREEN,	"prioritized" );					break;	// prioritized
			case 1:	Console.PrintSpecial( CRED,		"unprioritized - no section" );		break;	// file exist, no section
			default:
			case 2:	Console.PrintSpecial( CBLUE,	"unprioritized - no file" );		break;	// no file
			};
		}
	}
	CleanPriorityMap();
}

SI32 cServerDefinitions::CountOfEntries( DEFINITIONCATEGORIES typeToFind )
{
	SI32 sumEntries = 0;
	VECSCRIPTLIST *toScan = &(ScriptListings[typeToFind]);
	if( toScan == NULL )
		return 0;
	for( UI32 counter = 0; counter < toScan->size(); counter++ )
		sumEntries += (*toScan)[counter]->NumEntries();
	return sumEntries;
}

SI32 cServerDefinitions::CountOfFiles( DEFINITIONCATEGORIES typeToFind )
{
	return ScriptListings[typeToFind].size();
}

VECSCRIPTLIST *cServerDefinitions::GetFiles( DEFINITIONCATEGORIES typeToFind )
{
	return &(ScriptListings[typeToFind]);
}

void cServerDefinitions::CleanPriorityMap( void )
{
	priorityMap.erase( priorityMap.begin(), priorityMap.end() );
}
void cServerDefinitions::BuildPriorityMap( DEFINITIONCATEGORIES category, UI08& wasPrioritized )
{
	cDirectoryListing priorityFile( category, "priority.nfo", false );
	STRINGLIST *longList = priorityFile.List();
	if( longList->size() > 0 )
	{
		std::string filename = (*longList)[0].c_str();
		//	Do we have any priority informat?
		FILE *toScan = fopen( filename.c_str(), "r" );
		if( toScan != NULL )	// the file exists, so perhaps we do
		{
			Script *prio = new Script( filename.c_str(), category, false );	// generate a script for it
			if( prio != NULL )	// successfully made a script
			{
				const char *tag = NULL;
				const char *data = NULL;
				ScriptSection *prioInfo = prio->FindEntry( "PRIORITY" );	// find the priority entry
				if( prioInfo != NULL )
				{
					for( tag = prioInfo->First(); !prioInfo->AtEnd(); tag = prioInfo->Next() )	// keep grabbing priority info
					{
						data = prioInfo->GrabData();
						if( !strcmp( tag, "DEFAULTPRIORITY" ) )
							defaultPriority = (SI16)(makeNum( data ));
						else
						{
							char filenametemp[128];
							strcpy( filenametemp, tag );
							strlwr( filenametemp );
							priorityMap[filenametemp] = (SI16)(makeNum( data ));
						}
					}
					wasPrioritized = 0;
				}
				else
					wasPrioritized = 1;
				delete prio;	// remove script
			}
			else
				wasPrioritized = 2;
			fclose( toScan );	// close file
			return;
		}
	}
#ifdef _DEBUG
//	Console.Warning( 1, "Failed to open priority.nfo for reading in %s DFN", dirnames[category].c_str() );
#endif
	wasPrioritized = 2;
}

void cServerDefinitions::DisplayPriorityMap( void )
{
	Console << "Dumping map... " << myendl;
	std::map< std::string, SI16 >::iterator p = priorityMap.begin();
	while( p != priorityMap.end() )
	{
		Console << p->first.c_str() << " : " << p->second << myendl;
		p++;
	}
	Console << "Dumped" << myendl;
}

SI16 cServerDefinitions::GetPriority( const char *file )
{
	char lowername[MAX_PATH];
	strcpy( lowername, file );
	strlwr( lowername );
	std::map< std::string, SI16 >::const_iterator p = priorityMap.find( lowername );
	if( p == priorityMap.end() )
		return defaultPriority;
	else
		return p->second;
}

bool cDirectoryListing::PushDir( DEFINITIONCATEGORIES toMove )
{
	char filePath[MAX_PATH];
	sprintf( filePath, "%s%s", cwmWorldState->ServerData()->GetDefsDirectory(), dirnames[toMove].c_str() );
	return PushDir( filePath );
}
bool cDirectoryListing::PushDir( std::string toMove )
{
	std::string cwd = CurrentWorkingDir();
	dirs.push( cwd );

	if( _chdir( toMove.c_str() ) == 0 )
	{
		currentDir = toMove;
		PathFix( toMove );
		shortCurrentDir = ShortDirectory( toMove );
		return true;
	}
	else
	{
		Console.Error( 1, "DFN directory %s does not exist", toMove.c_str() );
		Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
	}
	return false;
}
void cDirectoryListing::PopDir( void )
{
	if( dirs.empty() )
	{
		Console.Error( 1, "cServerDefinition::PopDir called, but dirs is empty" );
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
#if defined(__unix__)
	DIR *dir = opendir(".");
	struct dirent *dirp = NULL;

	while( ( dirp = readdir( dir ) ) )	
	{
		if( dirp->d_type == DT_DIR && doRecursion )
		{
			if( strcmp( dirp->d_name, "." ) && strcmp( dirp->d_name, ".." ) )
				subdirectories.push_back( cDirectoryListing( dirp->d_name, extension, doRecursion ) );
			continue;
		}
		shortList.push_back( dirp->d_name );
		sprintf( filePath, "%s/%s", currentDir.c_str(), dirp->d_name );
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
	for( unsigned int j = 0; j < filenameList.size(); j++ )
	{
		flattenedFull.push_back( filenameList[j] );
		if( isParent )
			temp = "";
		else
		{
			temp = shortCurrentDir;
			temp += "/";
		}
		temp += shortList[j];
		flattenedShort.push_back( temp );
	}
	for( unsigned int i = 0; i < subdirectories.size(); i++ )
	{
		subdirectories[i].Flatten( false );
		STRINGLIST *shortFlat	= subdirectories[i].FlattenedShortList();
		STRINGLIST *longFlat	= subdirectories[i].FlattenedList();
		for( unsigned int k = 0; k < longFlat->size(); k++ )
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
		subdirectories[i].ClearFlatten();
	}
}
void cDirectoryListing::ClearFlatten( void )
{
	flattenedFull.clear();
	flattenedShort.clear();
}
