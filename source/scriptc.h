#ifndef __SCRIPTC_H__
#define __SCRIPTC_H__


typedef std::map< std::string, ScriptSection * >	SSMAP;

class Script
{
public:
	UI32 lastModTime;
	Script( const std::string& _filename, DEFINITIONCATEGORIES d, bool disp = true );
	~Script();

	ScriptSection * FindEntry( const std::string& section );
	ScriptSection *	FindEntrySubStr( const std::string& section );
	ScriptSection *	FirstEntry( void );
	ScriptSection *	NextEntry( void );

	bool isin( const std::string& section );
	std::string EntryName( void );
	size_t NumEntries( void ) const
	{
		return defEntries.size();
	}
	bool IsErrored( void ) const
	{
		return errorState;
	}

private:
	void deleteMap( void );
	void reload( bool disp = true );
	bool createSection( std::string& name );

	SSMAP					defEntries;			// string is the name of section
	SSMAP::iterator			iSearch;
	time_t					last_modification;
	std::string				filename;
	bool					errorState;
	DEFINITIONCATEGORIES	dfnCat;
	std::fstream			input;
};

#endif
