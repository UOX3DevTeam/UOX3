#include "uox3.h"

class ScriptSection;

typedef map< string, ScriptSection * >	SSMAP;

class Script 
{
public:
	long lastModTime;
	Script( const string _filename, DefinitionCategories d, bool disp = true );
	virtual	~Script();

	ScriptSection * FindEntry( const string section );
	ScriptSection *	FindEntrySubStr( const string section );
	ScriptSection *	FirstEntry( void );
	ScriptSection *	NextEntry( void );

  bool isin( const string section );
	const char * EntryName( void );
	long NumEntries( void ) const { return defEntries.size(); }
	bool IsErrored( void ) const { return errorState; }

private:
	void deleteMap( void );
  void reload( bool disp = true );

	SSMAP defEntries;			// string is the name of section
	SSMAP::iterator iSearch;
  time_t last_modification;
  string filename;
	bool errorState;
	DefinitionCategories	dfnCat;
};
