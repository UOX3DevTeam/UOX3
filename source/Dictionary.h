// Dictionary.h: interface for the CDictionary class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__
#include <array>


//o------------------------------------------------------------------------------------------------o
// CDictionary
//o------------------------------------------------------------------------------------------------o
class CDictionary
{
public:
	~CDictionary() = default;
	CDictionary( const std::string& filepath = "./dictionary/dictionary.ZRO", const std::string& language = "ZRO" );
	auto SetLocationLanguage( const std::string &filepath, const std::string &language ) -> void;

	auto operator[]( int message_number ) const -> const std::string&;
	auto operator[]( int message_number ) -> std::string&;
	auto GetEntry( int message_number ) const -> const std::string&;
	auto GetEntry( int message_number ) -> std::string&;

	auto ShowList() -> void;
	auto LoadDictionary( const std::string filepath = "", const std::string &language = "" ) -> SI32;
	auto NumberOfEntries() const -> size_t;
	auto GetValid() const -> bool;
private:
	auto ParseFile( const std::string &dictionaryfile ) -> bool;
	std::string pathToDictionary;
	std::string dictLanguage;
	std::map<int, std::string> msgdata;   // The key here really needs to match the index in operator[]
};

//===================================================================================================
// CDictionaryContainer
//===================================================================================================
class CDictionaryContainer {
private:
	std::array<CDictionary, static_cast<int>( DL_COUNT )> dictList;
	UnicodeTypes defaultLang;
public:
	CDictionaryContainer();
	~CDictionaryContainer() = default;
	auto LoadDictionaries( const std::string& filepath = "" ) -> int;
	auto SetDefaultLang( UnicodeTypes newType ) -> void;

	auto operator[]( int message_number ) const -> const std::string&; // only default lang
	auto operator[]( int message_number ) -> std::string&; // only default lang
	auto GetEntry( int message_number, const UnicodeTypes toDisp = ZERO ) const -> const std::string&;
	auto GetEntry( int message_number, const UnicodeTypes toDisp = ZERO )  -> std::string&;
};

extern CDictionaryContainer *Dictionary;

#endif
