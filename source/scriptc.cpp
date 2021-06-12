//
//	Cache script section locations
//	Reads through the contents of the file and saves location of each
//	SECTION XYZ entry

//	Calling Script::find() will then seek to that location directly rather
//	than having to parse through all of the script
//
#include "uox3.h"
#include "ssection.h"
#include "scriptc.h"
#include "StringUtility.hpp"


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool get_modification_date( const std::string& filename, time_t* mod_time )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the file's stats can be found (testing its existence)
//o-----------------------------------------------------------------------------------------------o
bool get_modification_date( const std::string& filename, time_t* mod_time )
{
	struct stat stat_buf;

	if( stat( filename.c_str(), &stat_buf ) )
		return false;

	*mod_time = stat_buf.st_mtime;
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function		-	void reload( bool disp )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose			-	Reload's the scripts data.  If disp is true, then information is output to
//|						the console (# of sections). Will not run if the script is in an erroneous state
//o-----------------------------------------------------------------------------------------------o
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
			std::string sLine;
			SI32 count = 0;
			while( !input.eof() && !input.fail() )
			{
				input.getline(line, 2047);
				line[input.gcount()] = 0 ;
				sLine = std::string(line);
				sLine = strutil::stripTrim( sLine );
				if( !sLine.empty() )
				{
					// We have some real data
					// see if in a section
					if( sLine.substr( 0, 1 ) == "[" && sLine.substr( sLine.size() - 1 ) == "]" )
					{
						// Ok a section is starting here, get the name
						std::string sectionname = sLine.substr( 1, sLine.size() - 2 );
						sectionname				= strutil::toupper( strutil::simplify( sectionname ));
						// Now why we look for a {, no idea, but we do - Because we want to make sure that were IN a block not before the block. At least this makes sure that were inside the {}'s of a block...
						while( !input.eof() && sLine.substr( 0, 1 ) != "{" && !input.fail() )
						{
							input.getline(line, 2047);
							line[input.gcount()] = 0 ;
							sLine = std::string(line);
							sLine = strutil::stripTrim( sLine );
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
			std::cerr<< "Cannot open "<< filename<<": "<< std::string(strerror( errno )) <<std::endl;
			errorState = true;
		}
	}
	if( disp ){
		Console.print( strutil::format("Reloading %-15s: ", filename.c_str()) );
	}

	fflush( stdout );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function		-	Script( const std::string& _filename, DEFINITIONCATEGORIES d, bool disp ) : errorState( false ), dfnCat( d )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose			-	Builds the script, reading in the information from the script file.
//o-----------------------------------------------------------------------------------------------o
Script::Script( const std::string& _filename, DEFINITIONCATEGORIES d, bool disp ) : errorState( false ), dfnCat( d )
{
	filename = _filename;
	if( !get_modification_date( filename, &last_modification ) )
	{
		std::cerr<< "Cannot open "<< filename<< ": "<< std::string(strerror( errno )) << std::endl;;
		errorState = true;
	}
	reload( disp );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function		-	~Script()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose			-	Destroys any memory that has been allocated
//o-----------------------------------------------------------------------------------------------o
Script::~Script()
{
	deleteMap();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function		-	bool isin( const std::string& section )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose			-	Returns true if the section named section is in the script
//o-----------------------------------------------------------------------------------------------o
bool Script::isin( const std::string& section )
{
	std::string temp(section);
	SSMAP::const_iterator iSearch = defEntries.find(strutil::toupper( temp ));
	if( iSearch != defEntries.end() )
		return true;
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function		-	ScriptSection *FindEntry( const std::string& section )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose			-	Returns a ScriptSection * to the section named "section"
//|						if it exists, otherwise returning nullptr
//o-----------------------------------------------------------------------------------------------o
ScriptSection *Script::FindEntry( const std::string& section )
{
	ScriptSection *rvalue = nullptr;
	SSMAP::const_iterator iSearch = defEntries.find( section );
	if( iSearch != defEntries.end() )
		rvalue = iSearch->second;
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function		-	ScriptSection *FindEntrySubStr( const std::string& section )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose			-	Find the first ScriptSection * (if any) that has the
//|						string section in the section name
//o-----------------------------------------------------------------------------------------------o
ScriptSection *Script::FindEntrySubStr( const std::string& section )
{
	ScriptSection *rvalue = nullptr;
	auto usection = std::string( section );
	usection = strutil::toupper(usection);
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

//o-----------------------------------------------------------------------------------------------o
//|	Function		-	ScriptSection *FirstEntry( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose			-	Returns the first ScriptSection in the Script (if any)
//o-----------------------------------------------------------------------------------------------o
ScriptSection *Script::FirstEntry( void )
{
	ScriptSection *rvalue	= nullptr;
	iSearch					= defEntries.begin();
	if( iSearch != defEntries.end() )
		rvalue = iSearch->second;
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function		-	ScriptSection *NextEntry( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose			-	Returns the next ScriptSection (if any) in the Script
//o-----------------------------------------------------------------------------------------------o
ScriptSection *Script::NextEntry( void )
{
	ScriptSection *rvalue = nullptr;
	if( iSearch != defEntries.end() )
	{
		++iSearch;
		if( iSearch != defEntries.end() )
			rvalue = iSearch->second;
	}
	return rvalue;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function		-	deleteMap( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose			-	Destroys any memory that has been allocated
//o-----------------------------------------------------------------------------------------------o
void Script::deleteMap( void )
{
	for( SSMAP::const_iterator iTest = defEntries.begin(); iTest != defEntries.end(); ++iTest )
		delete iTest->second;
	defEntries.clear();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function		-	std::string EntryName( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose			-	Returns the section name for the current entry (if any)
//o-----------------------------------------------------------------------------------------------o
std::string Script::EntryName( void )
{
	std::string rvalue;
	if( iSearch != defEntries.end() )
		rvalue = iSearch->first;
	return rvalue;
}
