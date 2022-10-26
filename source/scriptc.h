#ifndef __SCRIPTC_H__
#define __SCRIPTC_H__

#include <unordered_map>

typedef std::unordered_map<std::string, CScriptSection *>	SSMAP;

class Script
{
public:
	UI32 lastModTime;
	Script( const std::string& _filename, DEFINITIONCATEGORIES d, bool disp = true );
	~Script();

	CScriptSection * 	FindEntry( const std::string& section );
	CScriptSection *	FindEntrySubStr( const std::string& section );
	CScriptSection *	FirstEntry();
	CScriptSection *	NextEntry();
	auto collection() const -> const SSMAP &;
	auto collection() -> SSMAP&;

	bool IsInSection( const std::string& section );
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
	void DeleteMap( void );
	void Reload( bool disp = true );
	bool CreateSection( std::string& name );

	SSMAP	defEntries;			// string is the name of section
	SSMAP::iterator iSearch;
	time_t last_modification;
	std::string	filename;
	bool errorState;
	DEFINITIONCATEGORIES dfnCat;
	std::ifstream input;
};

#endif
