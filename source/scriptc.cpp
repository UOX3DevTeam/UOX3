//	
//	Cache script section locations by <erwin@andreasen.com>
//	Reads through the contents of the file and saves location of each
//	SECTION XYZ entry

//	Calling Script::find() will then seek to that location directly rather
//	than having to parse through all of the script
//	
#pragma warning( disable : 4786 )

#include <sys/stat.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <errno.h>
#include "uox3.h"
#include "debug.h"
#include <assert.h>
#include "ssection.h"

//using namespace std;

#undef DBGFILE
#define DBGFILE "scriptc.cpp"

//o--------------------------------------------------------------------------
//|	Function		-	bool get_modification_date( const char *filename, time_t *mod_time )
//|	Date			-	Unknown
//|	Programmer		-	Unknown
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the file's stats can be found (testing 
//|						its existence)
//o--------------------------------------------------------------------------
bool get_modification_date( const char *filename, time_t* mod_time )
{
	struct stat stat_buf;

	if( stat( filename, &stat_buf ) )
		return false;

	*mod_time = stat_buf.st_mtime;
	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	reload( bool disp = false )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Reload's the scripts data.  If disp is true, then 
//|						information is output to the console (# of sections)
//|						Will not run if the script is in an erroneous state
//o--------------------------------------------------------------------------
void Script::reload( bool disp ) 
{
	if( errorState )
		return;
    char buf[1024], section_name[256];
    SI32 count = 0;
    
	deleteMap();
	FILE *fp = fopen( filename.c_str(), "r" );
  if( fp == NULL ) 
	{
		fprintf(stderr, "Cannot open %s: %s", filename.c_str(), strerror( errno ) );
    errorState = true;
  }
	if( disp )
		Console.Print( "Reloading %-15s: ", filename.c_str() );
	
	fflush( stdout );

	lastModTime = 0;
	// Snarf the part of SECTION... until EOL
	while( fgets( buf, sizeof( buf ), fp ) )
	{
		if( sscanf( buf, "[%256[^\n]", section_name ) == 1 )
		{
			// check to see if it's terminated
			char *endBracket = strstr( section_name, "]" );
			if( endBracket != NULL )
			{
				section_name[endBracket-section_name] = '\0';
				strupr( section_name );
				defEntries[section_name] = new ScriptSection( fp, dfnCat );
				count++;
			}
		}
	}
	if( disp )
		Console << count << " sections found" << myendl;
    fclose( fp );
}

//o--------------------------------------------------------------------------
//|	Function		-	Script( const string _filename, bool disp )
//|	Date			-	Unknown
//|	Programmer		-	Unknown
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Builds the script, reading in the information from
//|						the script file.
//o--------------------------------------------------------------------------
Script::Script( const std::string _filename, DefinitionCategories d, bool disp ) : errorState( false ), dfnCat( d )
{
	filename = _filename;
  if( !get_modification_date( filename.c_str(), &last_modification ) ) 
	{
		fprintf( stderr, "Cannot open %s: %s", filename.c_str(), strerror( errno ) );
    errorState = true;
  }
  reload( disp );
}

//o--------------------------------------------------------------------------
//|	Function		-	~Script()
//|	Date			-	Unknown
//|	Programmer		-	Unknown
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Destroys any memory that has been allocated
//o--------------------------------------------------------------------------
Script::~Script()
{
	deleteMap();
}

//o--------------------------------------------------------------------------
//|	Function		-	bool isin( const string section )
//|	Date			-	Unknown
//|	Programmer		-	Unknown
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the section named section is in the script
//o--------------------------------------------------------------------------
bool Script::isin( const std::string section )
{
	SSMAP::iterator iSearch;
	iSearch = defEntries.find( section );
	if( iSearch != defEntries.end() )
		return true;
    return false;
}

//o--------------------------------------------------------------------------
//|	Function		-	ScriptSection *FindEntry( const string section )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns a ScriptSection * to the section named "section"
//|						if it exists, otherwise returning NULL
//o--------------------------------------------------------------------------
ScriptSection *Script::FindEntry( const std::string section )
{
	SSMAP::iterator iSearch;
	char section_name[256];
	strcpy( section_name, section.c_str() );
	strupr( section_name );
	iSearch = defEntries.find( section_name );
	if( iSearch != defEntries.end() )
		return iSearch->second;
    return NULL;
}

//o--------------------------------------------------------------------------
//|	Function		-	ScriptSection *FindEntrySubStr( const string section )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Find the first ScriptSection * (if any) that has the
//|						string section in the section name
//o--------------------------------------------------------------------------
ScriptSection *Script::FindEntrySubStr( const std::string section )
{
	SSMAP::iterator iSearch;
	const char *tSearch = section.c_str();
	for( iSearch = defEntries.begin(); iSearch != defEntries.end(); iSearch++ )
	{
		if( strstr( iSearch->first.c_str(), tSearch ) )	// FOUND IT!
			return iSearch->second;
	}
    return NULL;
}


//o--------------------------------------------------------------------------
//|	Function		-	ScriptSection *FirstEntry( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the first ScriptSection in the Script (if any)
//o--------------------------------------------------------------------------
ScriptSection *Script::FirstEntry( void )
{
	iSearch = defEntries.begin();
	if( iSearch != defEntries.end() )
		return iSearch->second;
    return NULL;
}

//o--------------------------------------------------------------------------
//|	Function		-	ScriptSection *NextEntry( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the next ScriptSection (if any) in the Script
//o--------------------------------------------------------------------------
ScriptSection *Script::NextEntry( void )
{
	if( iSearch != defEntries.end() )
	{
		iSearch++;
		if( iSearch != defEntries.end() )
			return iSearch->second;
		else
			return NULL;
	}
	else
		return NULL;
}

//o--------------------------------------------------------------------------
//|	Function		-	deleteMap( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Destroys any memory that has been allocated
//o--------------------------------------------------------------------------
void Script::deleteMap( void )
{
	SSMAP::iterator iTest;
	for( iTest = defEntries.begin(); iTest != defEntries.end(); iTest++ )
		delete iTest->second;
	defEntries.erase( defEntries.begin(), defEntries.end() );
}

//o--------------------------------------------------------------------------
//|	Function		-	const char *EntryName( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the section name for the current entry (if any)
//o--------------------------------------------------------------------------
const char *Script::EntryName( void )
{
	if( iSearch != defEntries.end() )
		return iSearch->first.c_str();
	return NULL;
}

