// Dictionary.cpp: implementation of the CDictionary class.
//
//////////////////////////////////////////////////////////////////////

#include "uox3.h"
#include "Dictionary.h"
#include "speech.h"
#include "scriptc.h"
#include "ssection.h"
#include "StringUtility.hpp"

#include <memory>
#include <filesystem>
#include <fstream>

using namespace std::string_literals ;

CDictionaryContainer *Dictionary;
static auto invalid_dictionary_string = std::string() ;

//const SI32 dictCANTOPEN			= -1;
//const SI32 dictDUPESECTION		= -2;
//const SI32 dictDUPEOPENBRACE	= -3;
//const SI32 dictNOOPENBRACE		= -4;


//====================================================================================================
// CDictionary
//====================================================================================================

//====================================================================================================
CDictionary::CDictionary( const std::string& filepath, const std::string& language ){
	setLocationLanguage(filepath, language);
}

//====================================================================================================
auto CDictionary::setLocationLanguage(const std::string &filepath, const std::string &language) ->void{
	if( language.empty() ){
		Language = "ZRO";
	}
	else{
		Language = language;
	}
	auto path = std::filesystem::path(filepath) ;
	if( filepath.empty() || !std::filesystem::exists(path)){
		PathToDictionary = "./dictionary/dictionary.ZRO";
	}
	else{
		PathToDictionary = filepath;
	}

}
//====================================================================================================
auto CDictionary::operator[](int message_number)->std::string & {
	return GetEntry( message_number );
}
//====================================================================================================
auto CDictionary::operator[](int message_number)const ->const std::string & {
	return GetEntry( message_number );
}
//====================================================================================================
auto  CDictionary::GetEntry(int message_number ) const ->const std::string& {
	try {
		return msgdata.at(message_number) ;
	}
	catch(...) {
		Console.warning( "Dictionary reference "s + std::to_string(message_number)+" not found in "s+PathToDictionary );
		return invalid_dictionary_string ;
	}
}
//====================================================================================================
auto  CDictionary::GetEntry(int message_number ) ->std::string& {
	try {
		return msgdata.at(message_number) ;
	}
	catch(...) {
		Console.warning( "Dictionary reference "s + std::to_string(message_number)+" not found in "s+PathToDictionary );
		return invalid_dictionary_string ;
	}
}

//====================================================================================================
auto CDictionary::ShowList()->void{
	Console << "Dictionary Entries for language: "<<Language << " file: "<<PathToDictionary << myendl;
	for (auto &[entrynum,text] : msgdata){
		Console<< entrynum << " : "s << text  << myendl ;
	}
}

//====================================================================================================
auto CDictionary::LoadDictionary(const std::string filepath, const std::string &language) ->std::int32_t {
	if (!filepath.empty()){
		PathToDictionary = filepath ;
	}
	if (!language.empty()) {
		Language = language ;
	}
	msgdata.clear();

	auto status = parseFile(PathToDictionary);
	Console.print( " " );
	Console.MoveTo( 15 );
	Console << "Dictionary." << Language;
	if (status) {
		Console.PrintSpecial( CGREEN, "done" );
	}
	else {
		Console.PrintSpecial(CRED, "failed");
	}

	return static_cast<std::int32_t>(msgdata.size()) ;
}
//====================================================================================================
auto CDictionary::parseFile(const std::string &dictionaryfile) ->bool {
	auto rvalue = false ;
	auto input = std::ifstream(dictionaryfile) ;
	enum search_t {header,startsection,endsection};
	auto state = search_t::header ;
	if (input.is_open()) {
		char input_buffer[4096] ;
		while (input.good() && !input.eof()){
			input.getline(input_buffer,4095) ;
			if (input.gcount()>0){
				input_buffer[input.gcount()] = 0 ;
				auto line = std::string(input_buffer) ;
				line = oldstrutil::removeTrailing(line);
				line = oldstrutil::trim(line) ;
				if (!line.empty()){
					switch (static_cast<int>(state)){
						case static_cast<int>(search_t::header): {
							if ((line[0] == '[') && (*(line.rbegin()) == ']')) {
								// it is a section header!
								line = oldstrutil::upper(oldstrutil::simplify(line.substr(1,line.size()-2))) ;
								auto [key,value] = oldstrutil::split(line, " ") ;
								if ((key == "DICTIONARY") && (value == "CLIENTMSG")){
									// This is a good section start!
									state = search_t::startsection ;
								}
												    
							}
							break;
						}
							
						case static_cast<int>(search_t::startsection): {
							if (line == "{") {
								state = search_t::endsection ;
								rvalue = true ;
							}
						}
						case static_cast<int>(search_t::endsection):{
							if (line!= "}"){
								auto [key,value] = oldstrutil::split(line, "=") ;
								try {
									auto number = std::stoi(line,nullptr,0) ;
									msgdata.insert_or_assign(number, line);
								}
								catch(...){
									// just skip this, no idea what it is
								}
							}
							else {
								// We dont process more then one section,maybe some day.
								break;
							}
						}

					}
				}
			}
		}
	}
	return rvalue ;
}


//====================================================================================================
auto CDictionary::GetValid() const ->bool {
	return !msgdata.empty();
}

//====================================================================================================
auto CDictionary::NumberOfEntries() const ->size_t {
	return msgdata.size();
}
//====================================================================================================
// CDictionaryContainer
//====================================================================================================
//====================================================================================================
CDictionaryContainer::CDictionaryContainer() {
	defaultLang = ZERO ;
}

//====================================================================================================
CDictionaryContainer::CDictionaryContainer( const std::string& filepath,UnicodeTypes lang )  {
	defaultLang = lang ;
	auto basepath = filepath ;
	if (filepath.empty()){
		basepath = cwmWorldState->ServerData()->Directory( CSDDP_DICTIONARIES );
	}
	for( UI16 i = (UI16)DL_DEFAULT; i < (UI16)DL_COUNT; ++i ) {
		auto buildName = basepath + "dictionary."s + DistinctLanguageNames[i];
		dictList[i].setLocationLanguage( buildName, DistinctLanguageNames[i] );
	}
}



//=====================================================================================================================================
auto CDictionaryContainer::LoadDictionary(const std::string &filepath )->std::int32_t{
	SI32 rvalue = 0;
	for(auto i = static_cast<int>(DL_DEFAULT); i < static_cast<int>(DL_COUNT); i++ ) {
		auto basepath = filepath ;
		auto buildName = std::string();
		if (!filepath.empty()){
			buildName = basepath + "dictionary."s + DistinctLanguageNames[i];
		}
		dictList[i].LoadDictionary(buildName,DistinctLanguageNames[i]);
	}
	if( !dictList[LanguageCodesLang[ZERO]].GetValid() ) {
		Console.error( "Dictionary.ZRO is bad or nonexistant" );
		Shutdown( FATAL_UOX3_BAD_DEF_DICT );
		rvalue = -1;
	}
	return rvalue;
}

//=====================================================================================================================================
auto CDictionaryContainer::operator[]( int message_number ) const ->const std::string & {
	return GetEntry( message_number );
}
//=====================================================================================================================================
auto CDictionaryContainer::operator[]( int message_number )  -> std::string & {
	return GetEntry( message_number );
}

//=====================================================================================================================================
auto CDictionaryContainer::GetEntry( const SI32 message_number, const UnicodeTypes toDisp )->std::string & {
	
	if( cwmWorldState->ServerData()->ServerLanguage() != DL_DEFAULT ) { // defaultServerLang != DL_DEFAULT )
		// If a default server language has been specified in uox.ini, force the use of that language
		if (dictList[cwmWorldState->ServerData()->ServerLanguage()].GetValid()){
			return dictList[cwmWorldState->ServerData()->ServerLanguage()][message_number];
		}
		else {
			return dictList[LanguageCodesLang[defaultLang]][message_number];

		}
	}

	auto typetouse = toDisp ;
	if ((static_cast<SI32>(toDisp) < 0) || (static_cast<SI32>(toDisp)>= UnicodeTypes::TOTAL_LANGUAGES)){
		typetouse = ZERO ;
	}
	try {
		auto mLanguage = LanguageCodesLang[typetouse];
		if (dictList[mLanguage].GetValid()) {
			return dictList[mLanguage][message_number];
		}
		else {
			// The langague wasn't valid, so.....
			if (dictList[LanguageCodesLang[defaultLang]].GetValid()) {
				return dictList[LanguageCodesLang[defaultLang]].GetEntry(message_number);
			}
			else {
				return invalid_dictionary_string;
			}
		}
		
	}
	catch(...){
		return invalid_dictionary_string;

	}
}
//=====================================================================================================================================
auto CDictionaryContainer::GetEntry( const SI32 message_number, const UnicodeTypes toDisp )const ->const std::string & {
	
	if( cwmWorldState->ServerData()->ServerLanguage() != DL_DEFAULT ) { // defaultServerLang != DL_DEFAULT )
		// If a default server language has been specified in uox.ini, force the use of that language
		if (dictList[cwmWorldState->ServerData()->ServerLanguage()].GetValid()){
			return dictList[cwmWorldState->ServerData()->ServerLanguage()][message_number];
		}
		else {
			return dictList[LanguageCodesLang[defaultLang]][message_number];
			
		}
	}
	
	auto typetouse = toDisp ;
	if ((static_cast<SI32>(toDisp) < 0) || (static_cast<SI32>(toDisp)>= UnicodeTypes::TOTAL_LANGUAGES)){
		typetouse = ZERO ;
	}
	try {
		auto mLanguage = LanguageCodesLang[typetouse];
		if (dictList[mLanguage].GetValid()) {
			return dictList[mLanguage][message_number];
		}
		else {
			// The langague wasn't valid, so.....
			if (dictList[LanguageCodesLang[defaultLang]].GetValid()) {
				return dictList[LanguageCodesLang[defaultLang]].GetEntry(message_number);
			}
			else {
				return invalid_dictionary_string;
			}
		}
		
	}
	catch(...){
		return invalid_dictionary_string;
		
	}
}
//=====================================================================================================================================
auto CDictionaryContainer::SetDefaultLang( UnicodeTypes newType )->void {
	if( dictList[LanguageCodesLang[newType]].GetValid() ){
		defaultLang = newType;
	}
}

