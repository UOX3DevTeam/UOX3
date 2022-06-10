// Dictionary.h: interface for the CDictionary class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__
#include <array>

class CDictionary
{
public:
	CDictionary();
	CDictionary( const std::string& filepath, const std::string& language );
	~CDictionary();
	std::string 	operator[]( const SI32 Num );
	std::string 	GetEntry( const SI32 Num );

public:
	void			ShowList( void );
	SI32			LoadDictionary( void );
	size_t			NumberOfEntries( void ) const;
	void			SetValid( const bool newVal );
	bool			GetValid( void ) const;
	auto	setLocationLanguage(const std::string &filepath, const std::string &language) ->void;
private:
	bool IsValid;
	std::string PathToDictionary;
	std::string Language;
	std::map< UI32, std::string > Text2;
};

class CDictionaryContainer
{
private:
	std::array<CDictionary*, static_cast<int>(DL_COUNT)> dictList;
	UnicodeTypes		defaultLang;
public:
	CDictionaryContainer();
	CDictionaryContainer( const std::string& filepath );
	~CDictionaryContainer();
	SI32			LoadDictionary( void );
	std::string 	operator[]( const SI32 Num );	// only default lang
	std::string 	GetEntry( const SI32 Num, const UnicodeTypes toDisp = ZERO );
	void			SetDefaultLang( const UnicodeTypes newType );
};

extern CDictionaryContainer *Dictionary;

#endif
