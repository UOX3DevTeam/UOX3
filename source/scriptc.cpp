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
#include "osunique.hpp"

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetModificationDate()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the file's stats can be found (testing its existence)
//o------------------------------------------------------------------------------------------------o
bool GetModificationDate( const std::string& filename, time_t* mod_time )
{
	struct stat stat_buf;

	if( stat( filename.c_str(), &stat_buf ))
		return false;

	*mod_time = stat_buf.st_mtime;
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::Reload()
//o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Reload's the scripts data.  If disp is true, then information is output to
//|						the console (# of sections). Will not run if the script is in an erroneous state
//o------------------------------------------------------------------------------------------------o
void Script::Reload( bool disp )
{
	if( !errorState )
	{
		// Clear the map, we are starting from scratch;
		DeleteMap();
		char line[2048];
		input.open( filename.c_str(), std::ios_base::in );
		if( input.is_open() )
		{
			std::string sLine;
			SI32 count = 0;
			while( !input.eof() && !input.fail() )
			{
				input.getline( line, 2047 );
				line[input.gcount()] = 0;
				sLine = std::string( line );
				sLine = oldstrutil::trim( oldstrutil::removeTrailing( sLine, "//" ));
				if( !sLine.empty() )
				{
					// We have some real data
					// see if in a section
					if( sLine.substr( 0, 1 ) == "[" && sLine.substr( sLine.size() - 1 ) == "]" )
					{
						// Ok a section is starting here, get the name
						std::string sectionname = sLine.substr( 1, sLine.size() - 2 );
						sectionname				= oldstrutil::upper( oldstrutil::simplify( sectionname ));
						// Now why we look for a {, no idea, but we do - Because we want to make sure that were IN a block not before the block. At least this makes sure that were inside the {}'s of a block...
						while( !input.eof() && sLine.substr( 0, 1 ) != "{" && !input.fail() )
						{
							input.getline( line, 2047 );
							line[input.gcount()] = 0;
							sLine = std::string( line );
							sLine = oldstrutil::trim( oldstrutil::removeTrailing( sLine, "//" ));
						}
						// We are finally in the actual section!
						// We waited until now to create it, incase a total invalid file
						lastModTime = 0;
						defEntries[sectionname] = new CScriptSection( input, dfnCat );
						++count;
					}
				}
			}
			input.close();
		}
		else
		{
			char buffer[200];
			std::cerr << "Cannot open " << filename << ": " << std::string( mstrerror( buffer, 200, errno )) << std::endl;
			errorState = true;
		}
	}
	if( disp )
	{
		Console.Print( oldstrutil::format( "Reloading %-15s: ", filename.c_str() ));
	}

	fflush( stdout );
}

//o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::Script()
//o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Builds the script, reading in the information from the script file.
//o------------------------------------------------------------------------------------------------o
Script::Script( const std::string& _filename, DEFINITIONCATEGORIES d, bool disp ) : errorState( false ), dfnCat( d )
{
	filename = _filename;
	if( !GetModificationDate( filename, &last_modification ))
	{
		char buffer[200];
		std::cerr << "Cannot open " << filename << ": " << std::string( mstrerror( buffer, 200, errno )) << std::endl;
		errorState = true;
	}
	Reload( disp );
}
//===============================================================================================
auto Script::collection() const -> const SSMAP&
{
	return defEntries;
}
//===============================================================================================
auto Script::collection() -> SSMAP&
{
	return defEntries;
}

//o------------------------------------------------------------------------------------------------o
//|	Function		-	~Script()
//o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Destroys any memory that has been allocated
//o------------------------------------------------------------------------------------------------o
Script::~Script()
{
	DeleteMap();
}

//o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::IsInSection()
//o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Returns true if the section named section is in the script
//o------------------------------------------------------------------------------------------------o
bool Script::IsInSection( const std::string& section )
{
	std::string temp( section );
	SSMAP::const_iterator iSearch = defEntries.find( oldstrutil::upper( temp ));
	if( iSearch != defEntries.end() )
		return true;

	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::FindEntry()
//o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Returns a CScriptSection * to the section named "section"
//|						if it exists, otherwise returning nullptr
//o------------------------------------------------------------------------------------------------o
CScriptSection *Script::FindEntry( const std::string& section )
{
	CScriptSection *rValue = nullptr;
	SSMAP::const_iterator iSearch = defEntries.find( section );
	if( iSearch != defEntries.end() )
	{
		rValue = iSearch->second;
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::FindEntrySubStr()
//o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Find the first CScriptSection * (if any) that has the
//|						string section in the section name
//o------------------------------------------------------------------------------------------------o
CScriptSection *Script::FindEntrySubStr( const std::string& section )
{
	CScriptSection *rValue = nullptr;
	auto usection = std::string( section );
	usection = oldstrutil::upper( usection );
	for( SSMAP::const_iterator iSearch = defEntries.begin(); iSearch != defEntries.end(); ++iSearch )
	{
		if( iSearch->first.find( usection ) != std::string::npos )	// FOUND IT!
		{
			rValue = iSearch->second;
			break;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::FirstEntry()
//o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Returns the first CScriptSection in the Script (if any)
//o------------------------------------------------------------------------------------------------o
CScriptSection *Script::FirstEntry( void )
{
	CScriptSection *rValue	= nullptr;
	iSearch					= defEntries.begin();
	if( iSearch != defEntries.end() )
	{
		rValue = iSearch->second;
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::NextEntry()
//o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Returns the next CScriptSection (if any) in the Script
//o------------------------------------------------------------------------------------------------o
CScriptSection *Script::NextEntry( void )
{
	CScriptSection *rValue = nullptr;
	if( iSearch != defEntries.end() )
	{
		++iSearch;
		if( iSearch != defEntries.end() )
		{
			rValue = iSearch->second;
		}
	}
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::DeleteMap()
//o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Destroys any memory that has been allocated
//o------------------------------------------------------------------------------------------------o
void Script::DeleteMap( void )
{
	for( SSMAP::const_iterator iTest = defEntries.begin(); iTest != defEntries.end(); ++iTest )
	{
		delete iTest->second;
	}
	defEntries.clear();
}

//o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::EntryName()
//o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Returns the section name for the current entry (if any)
//o------------------------------------------------------------------------------------------------o
std::string Script::EntryName( void )
{
	std::string rValue;
	if( iSearch != defEntries.end() )
	{
		rValue = iSearch->first;
	}
	return rValue;
}
