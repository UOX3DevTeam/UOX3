#include "cServerDefinitions.h"
#include "ssection.h"

#ifdef __LINUX__
	#include <dirent.h>
#   include <unistd.h>
#else
	#include <direct.h>
#endif

const string dirnames[NUM_DEFS] = 
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
	"necro",
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
	"carve"
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
			ScpList *toDel = &(ScriptListings[i]);
			if( toDel == NULL )
				continue;
			delete (*toDel)[j];
			(*toDel)[j] = NULL;
		}
	}
}

ScriptSection *cServerDefinitions::FindEntry( const char *toFind, DefinitionCategories typeToFind )
{
	if( toFind == NULL || typeToFind == NUM_DEFS )
		return NULL;
	ScpList *toDel = &(ScriptListings[typeToFind]);
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

Script *cServerDefinitions::FindScript( const char *toFind, DefinitionCategories typeToFind )
{
	if( toFind == NULL || typeToFind == NUM_DEFS )
		return NULL;
	ScpList *toDel = &(ScriptListings[typeToFind]);
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

const string defExt = "*.dfn";

struct PrioScan
{
	string	filename;
	SI16	priority;
	PrioScan() : filename( "" ), priority( 0 ) { }
	PrioScan( const char *toUse, SI16 mPrio ) : filename( toUse ), priority( mPrio ) { }
};

void SortPrioVec( vector< PrioScan * > *toSort )
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
		CleanFileList();
		defaultPriority = 0;
		UI08 wasPriod = 2;
		BuildPriorityMap( (DefinitionCategories)sCtr, wasPriod );
		BuildFileList( (DefinitionCategories)sCtr );
		vector< PrioScan * >	mSort;
		for( UI32 i = 0; i < filenameListings.size(); i++ )
		{
			const char *fname = filenameListings[i].c_str();
			mSort.push_back( new PrioScan( fname, GetPriority( fname ) ) );
		}
		if( mSort.size() > 0 )
		{
			SortPrioVec( &mSort );
			bool dirSet = PushDir( (DefinitionCategories)sCtr );
			Console.Print( "Section %20s : %6i", dirnames[sCtr].c_str(), 0 );
			UI32 iTotal = 0;
			Console.TurnYellow();
			for( UI32 iFile = 0; iFile < mSort.size(); iFile++ )
			{
				Console.Print( "\b\b\b\b\b\b" );
				ScriptListings[sCtr].push_back( new Script( mSort[iFile]->filename, (DefinitionCategories)sCtr, false ) );
				iTotal += (ScriptListings[sCtr])[iFile]->NumEntries();
				Console.Print( "%6i", iTotal );
				delete mSort[iFile];
				mSort[iFile] = NULL;
			}
			Console.Print( "\b\b\b\b\b\b%6i", CountOfEntries( (DefinitionCategories)sCtr ) );
			Console.TurnNormal();
			Console.Print( " entries [" );
			switch( wasPriod )
			{
			case 0:	Console.TurnGreen();	Console << "prioritized";					break;	// prioritized
			case 1:	Console.TurnRed();		Console << "unprioritized - no section";	break;	// file exist, no section
			default:
			case 2:	Console.TurnBlue();		Console << "unprioritized - no file";		break;	// no file
			};
			
			Console.TurnNormal();
			Console << "]";
			Console << myendl;
			if( dirSet )
				PopDir();
		}
	}
	CleanPriorityMap();
	CleanFileList();
}

SI32 cServerDefinitions::CountOfEntries( DefinitionCategories typeToFind )
{
	SI32 sumEntries = 0;
	ScpList *toScan = &(ScriptListings[typeToFind]);
	if( toScan == NULL )
		return 0;
	for( UI32 counter = 0; counter < toScan->size(); counter++ )
		sumEntries += (*toScan)[counter]->NumEntries();
	return sumEntries;
}

SI32 cServerDefinitions::CountOfFiles( DefinitionCategories typeToFind )
{
	return ScriptListings[typeToFind].size();
}

ScpList *cServerDefinitions::GetFiles( DefinitionCategories typeToFind )
{
	return &(ScriptListings[typeToFind]);
}

void cServerDefinitions::BuildFileList( DefinitionCategories category )
{
	bool dirSet = PushDir( category );
#ifdef __LINUX__
	DIR *dir = opendir(".");
	struct dirent *dirp = NULL;

	while( ( dirp = readdir( dir ) ) )	
	{
		if( dirp->d_type == DT_DIR )
			continue;
		filenameListings.push_back( dirp->d_name );
	}

#else

	WIN32_FIND_DATA toFind;
	HANDLE findHandle = FindFirstFile( defExt.c_str(), &toFind );		// grab first file that meets spec
	bool retVal = 0;
	if( findHandle != INVALID_HANDLE_VALUE )	// there is a file
	{
		filenameListings.push_back( toFind.cFileName );
		retVal = 1;	// let's continue
	}
	while( retVal != 0 )	// while there are still files
	{
		retVal = FindNextFile( findHandle, &toFind );	// grab the next file
		if( retVal != 0 )
			filenameListings.push_back( toFind.cFileName );
	}
	if( findHandle != INVALID_HANDLE_VALUE )
	{
		FindClose( findHandle );
		findHandle = INVALID_HANDLE_VALUE;
	}
#endif
	if( dirSet )
		PopDir();
}
void cServerDefinitions::CleanPriorityMap( void )
{
	priorityMap.erase( priorityMap.begin(), priorityMap.end() );
}
void cServerDefinitions::BuildPriorityMap( DefinitionCategories category, UI08& wasPrioritized )
{
	char fullName[256];
	// for some reason, you need to change dir for it to work right, don't ask me why
	bool dirSet = PushDir( category );
	strcpy( fullName, defExt.c_str() );	// setup the file spec

	//	Do we have any priority informat?
	FILE *toScan = fopen( "priority.nfo", "r" );
	if( toScan != NULL )	// the file exists, so perhaps we do
	{
		Script *prio = new Script( "priority.nfo", category, false );	// generate a script for it
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
	}
	else
	{
#ifdef _DEBUG
//		Console.Warning( 1, "Failed to open priority.nfo for reading in %s DFN", dirnames[category].c_str() );
#endif
		wasPrioritized = 2;
	}
	if( dirSet )
		PopDir();
}

void cServerDefinitions::CleanFileList( void )
{
	filenameListings.resize( 0 );
}

void cServerDefinitions::DisplayPriorityMap( void )
{
	Console << "Dumping map... " << myendl;
	map< string, SI16 >::iterator p = priorityMap.begin();
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
	map< string, SI16 >::const_iterator p = priorityMap.find( lowername );
	if( p == priorityMap.end() )
		return defaultPriority;
	else
		return p->second;
}

bool cServerDefinitions::PushDir( DefinitionCategories toMove )
{
	char filePath[MAX_PATH];
	sprintf( filePath, "%s%s", cwmWorldState->ServerData()->GetDefsDirectory(), dirnames[toMove].c_str() );
#ifdef _WIN32
# define getcwd _getcwd
#endif

	char cwd[MAX_PATH + 1];
	if (!getcwd(cwd, MAX_PATH + 1))
	  {
	    Console.Error(1, "Failed to allocate enough room for cwd");
	    Shutdown( FATAL_UOX3_DIR_NOT_FOUND);
	  }
	dirs.push(cwd);
	
	if ( _chdir( filePath ) == 0 )
	  {
	    return 1;
	  }
	else
	  {
	    Console.Error( 1, "DFN directory %s does not exist", filePath );
	    Shutdown( FATAL_UOX3_DIR_NOT_FOUND );
	  }

}

void cServerDefinitions::PopDir( void )
{
  if (dirs.empty())
  {
          Console.Error(1, "cServerDefinition::PopDir called, but dirs is empty");
          Shutdown( FATAL_UOX3_DIR_NOT_FOUND);
  }
  else
  {
    _chdir(dirs.top().c_str());
	  dirs.pop();
  }
}
