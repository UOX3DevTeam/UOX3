#pragma warning( disable : 4786 )

#include "uox3.h"

class ScriptSection;

typedef std::map< std::string, ScriptSection * >	SSMAP;

class Script 
{
public:
	long lastModTime;
	Script( const std::string _filename, DEFINITIONCATEGORIES d, bool disp = true );
	virtual	~Script();

	ScriptSection * FindEntry( const std::string section );
	ScriptSection *	FindEntrySubStr( const std::string section );
	ScriptSection *	FirstEntry( void );
	ScriptSection *	NextEntry( void );

  bool isin( const std::string section );
	const char * EntryName( void );
	long NumEntries( void ) const { return defEntries.size(); }
	bool IsErrored( void ) const { return errorState; }

private:
	void deleteMap( void );
  void reload( bool disp = true );

	SSMAP defEntries;			// string is the name of section
	SSMAP::iterator iSearch;
  time_t last_modification;
  std::string filename;
	bool errorState;
	DEFINITIONCATEGORIES	dfnCat;
};
