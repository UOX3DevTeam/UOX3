//	
//	Cache script section locations by <erwin@andreasen.com>
//	Reads through the contents of the file and saves location of each
//	SECTION XYZ entry

//	Calling Script::find() will then seek to that location directly rather
//	than having to parse through all of the script
//
#include "uox3.h"
#include "ssection.h"
#include "scriptc.h"

#undef DBGFILE
#define DBGFILE "scriptc.cpp"

namespace UOX
{

//o--------------------------------------------------------------------------
//|	Function		-	bool get_modification_date( std::string filename, time_t *mod_time )
//|	Date			-	Unknown
//|	Programmer		-	Unknown
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the file's stats can be found (testing 
//|						its existence)
//o--------------------------------------------------------------------------
bool get_modification_date( const std::string filename, time_t* mod_time )
{
	struct stat stat_buf;

	if( stat( filename.c_str(), &stat_buf ) )
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
	if( !errorState )
	{
		// Clear the map, we are starting from scratch;
		deleteMap();
		char line[2048];
		input.open( filename.c_str(), std::ios_base::in );
		if( input.is_open() )
		{
			UString sLine;
			SI32 count = 0;
			while( !input.eof() && !input.fail() )
			{
				input.getline( line, 2048 );
				sLine = line;
				sLine = sLine.removeComment().stripWhiteSpace();
				if( !sLine.empty() )
				{
					// We have some real data
					// see if in a section
					if( sLine.substr( 0, 1 ) == "[" && sLine.substr( sLine.size() - 1 ) == "]" )
					{
						// Ok a section is starting here, get the name
						UString sectionname = sLine.substr( 1, sLine.size() - 2 );
						sectionname			= sectionname.simplifyWhiteSpace().upper();
						// Now why we look for a {, no idea, but we do - Because we want to make sure that were IN a block not before the block. At least this makes sure that were inside the {}'s of a block...
						while( !input.eof() && sLine.substr( 0, 1 ) != "{" && !input.fail() )
						{
							input.getline( line, 2048 );
							sLine = line;
							sLine = sLine.removeComment().stripWhiteSpace();
						}
						// We are finally in the actual section!
						// We waited until now to create it, incase a total invalid file
						lastModTime = 0;
						defEntries[sectionname] = new ScriptSection( input, dfnCat );
						++count;
					}
				}
			}
			input.close();
		}
		else
		{
			fprintf( stderr, "Cannot open %s: %s", filename.c_str(), strerror( errno ) );
			errorState = true;
		}
	}
	if( disp )
		Console.Print( "Reloading %-15s: ", filename.c_str() );
		
	fflush( stdout );
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
Script::Script( const std::string _filename, DEFINITIONCATEGORIES d, bool disp ) : errorState( false ), dfnCat( d )
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
	UString temp( section );
	SSMAP::const_iterator iSearch = defEntries.find( temp.upper() );
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
	ScriptSection *rvalue = NULL;
	SSMAP::const_iterator iSearch = defEntries.find( section );
	if( iSearch != defEntries.end() )
		rvalue = iSearch->second;
    return rvalue;
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
	ScriptSection *rvalue = NULL;
	UString usection( section );
	usection = usection.upper();
	for( SSMAP::const_iterator iSearch = defEntries.begin(); iSearch != defEntries.end(); ++iSearch )
	{
		if( iSearch->first.find( usection ) != std::string::npos )	// FOUND IT!
		{
			rvalue = iSearch->second;
			break;
		}
	}
    return rvalue;
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
	ScriptSection *rvalue	= NULL;
	iSearch					= defEntries.begin();
	if( iSearch != defEntries.end() )
		rvalue = iSearch->second;
    return rvalue;
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
	ScriptSection *rvalue = NULL;
	if( iSearch != defEntries.end() )
	{
		++iSearch;
		if( iSearch != defEntries.end() )
			rvalue = iSearch->second;
	}
	return rvalue;
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
	for( SSMAP::const_iterator iTest = defEntries.begin(); iTest != defEntries.end(); ++iTest )
		delete iTest->second;
	defEntries.clear();
}

//o--------------------------------------------------------------------------
//|	Function		-	std::string EntryName( void )
//|	Date			-	Unknown
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns the section name for the current entry (if any)
//o--------------------------------------------------------------------------
std::string Script::EntryName( void )
{
	std::string rvalue;
	if( iSearch != defEntries.end() )
		rvalue = iSearch->first;
	return rvalue;
}

}
