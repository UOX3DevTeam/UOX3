// dictionary.cpp: implementation of the CDictionary class.
//
//////////////////////////////////////////////////////////////////////

#include "dictionary.h"

#include <filesystem>
#include <fstream>
#include <memory>

#include "subsystem/console.hpp"

#include "funcdecl.h"
#include "scriptc.h"
#include "speech.h"
#include "ssection.h"
#include "stringutility.hpp"
#include "utility/strutil.hpp"

using namespace std::string_literals;

CDictionaryContainer *Dictionary;
static auto invalid_dictionary_string = std::string();

// const std::int32_t dictCANTOPEN			= -1;
// const std::int32_t dictDUPESECTION		= -2;
// const std::int32_t dictDUPEOPENBRACE		= -3;
// const std::int32_t dictNOOPENBRACE		= -4;

// o------------------------------------------------------------------------------------------------o
//|	CDictionary::CDictionary()
// o------------------------------------------------------------------------------------------------o
// o------------------------------------------------------------------------------------------------o
CDictionary::CDictionary(const std::string &filepath, const std::string &language) {
    SetLocationLanguage(filepath, language);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CDictionary::SetLocationLanguage()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set which language to use for dictionary
// o------------------------------------------------------------------------------------------------o
auto CDictionary::SetLocationLanguage(const std::string &filepath, const std::string &language)
-> void {
    if (language.empty()) {
        dictLanguage = "ZRO";
    }
    else {
        dictLanguage = language;
    }
    auto path = std::filesystem::path(filepath);
    if (filepath.empty() || !std::filesystem::exists(path)) {
        pathToDictionary = "./dictionary/dictionary.ZRO";
    }
    else {
        pathToDictionary = filepath;
    }
}
//==================================================================================================
auto CDictionary::operator[](int message_number) -> std::string & {
    return GetEntry(message_number);
}
//==================================================================================================
auto CDictionary::operator[](int message_number) const -> const std::string & {
    return GetEntry(message_number);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CDictionary::GetEntry()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Retrieve a specified entry from dictionary
// o------------------------------------------------------------------------------------------------o
auto CDictionary::GetEntry(int message_number) const -> const std::string & {
    try {
        return msgdata.at(message_number);
    } catch (...) {
        Console::shared().warning("Dictionary reference "s + std::to_string(message_number) +
                                  " not found in "s + pathToDictionary);
        return invalid_dictionary_string;
    }
}
//==================================================================================================
auto CDictionary::GetEntry(int message_number) -> std::string & {
    try {
        return msgdata.at(message_number);
    } catch (...) {
        Console::shared().warning("Dictionary reference "s + std::to_string(message_number) +
                                  " not found in "s + pathToDictionary);
        return invalid_dictionary_string;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CDictionary::ShowList()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dump entire dictionary for selected language to console
// o------------------------------------------------------------------------------------------------o
auto CDictionary::ShowList() -> void {
    Console::shared() << "Dictionary Entries for language: " << dictLanguage
    << " file: " << pathToDictionary << myendl;
    for (auto &[entrynum, text] : msgdata) {
        Console::shared() << entrynum << " : "s << text << myendl;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CDictionary::LoadDictionary()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load dictionary file for selected language
// o------------------------------------------------------------------------------------------------o
auto CDictionary::LoadDictionary(const std::string filepath, const std::string &language) -> std::int32_t {
    if (!filepath.empty()) {
        pathToDictionary = filepath;
    }
    if (!language.empty()) {
        dictLanguage = language;
    }
    msgdata.clear();
    
    auto status = ParseFile(pathToDictionary);
    Console::shared().print(" ");
    Console::shared().moveTo(15);
    Console::shared() << "Dictionary." << dictLanguage;
    if (status) {
        Console::shared().printSpecial(CGREEN, "done");
    }
    else {
        Console::shared().printSpecial(CRED, "failed");
    }
    
    return static_cast<std::int32_t>(msgdata.size());
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CDictionary::ParseFile()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Parse dictionary file for selected language, and load entries into
// memory
// o------------------------------------------------------------------------------------------------o
auto CDictionary::ParseFile(const std::string &dictionaryfile) -> bool {
    auto rValue = false;
    auto input = std::ifstream(dictionaryfile);
    enum search_t { header, startsection, endsection };
    auto state = search_t::header;
    if (input.is_open()) {
        char input_buffer[4096];
        while (input.good() && !input.eof()) {
            input.getline(input_buffer, 4095);
            if (input.gcount() > 0) {
                input_buffer[input.gcount()] = 0;
                auto line = std::string(input_buffer);
                line = util::strip(line, "//");
                line = util::trim(line);
                if (!line.empty()) {
                    switch (static_cast<int>(state)) {
                        case static_cast<int>(search_t::header): {
                            if ((line[0] == '[') && (*(line.rbegin()) == ']')) {
                                // it is a section header!
                                line = util::upper(util::simplify(line.substr(1, line.size() - 2)));
                                auto [key, value] = util::split(line, " ");
                                if ((key == "DICTIONARY") && (value == "CLIENTMSG")) {
                                    // This is a good section start!
                                    state = search_t::startsection;
                                }
                            }
                            break;
                        }
                            
                        case static_cast<int>(search_t::startsection): {
                            if (line == "{") {
                                state = search_t::endsection;
                                rValue = true;
                            }
                        }
                            [[fallthrough]];
                        case static_cast<int>(search_t::endsection): {
                            if (line != "}") {
                                auto [key, value] = util::split(line, "=");
                                try {
                                    auto number = std::stoi(key, nullptr, 0);
                                    msgdata.insert_or_assign(number, value);
                                } catch (...) {
                                    // just skip this, no idea what it is
                                }
                            }
                            else {
                                // We dont process more then one section, maybe some day.
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CDictionary::GetValid()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Validate that dictionary for selected language is not empty
// o------------------------------------------------------------------------------------------------o
auto CDictionary::GetValid() const -> bool { return !msgdata.empty(); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CDictionary::NumberOfEntries()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Return total number of entires in selected dictionary
// o------------------------------------------------------------------------------------------------o
auto CDictionary::NumberOfEntries() const -> size_t { return msgdata.size(); }
// o------------------------------------------------------------------------------------------------o
//|	CDictionaryContainer::CDictionaryContainer()
// o------------------------------------------------------------------------------------------------o
// o------------------------------------------------------------------------------------------------o
CDictionaryContainer::CDictionaryContainer() { defaultLang = ZERO; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CDictionaryContainer::LoadDictionaries()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loop through all supported dictionary languages and load each
// dictionary
// o------------------------------------------------------------------------------------------------o
auto CDictionaryContainer::LoadDictionaries(const std::string &filepath) -> std::int32_t {
    std::int32_t rValue = 0;
    for (auto i = static_cast<int>(DL_DEFAULT); i < static_cast<int>(DL_COUNT); i++) {
        auto basepath = filepath;
        auto buildName = std::string();
        if (!filepath.empty()) {
            buildName = basepath + "dictionary."s + DistinctLanguageNames[i];
        }
        dictList[i].LoadDictionary(buildName, DistinctLanguageNames[i]);
    }
    if (!dictList[LanguageCodesLang[ZERO]].GetValid()) {
        Console::shared().error("Dictionary.ZRO is bad or nonexistant");
        Shutdown(FATAL_UOX3_BAD_DEF_DICT);
        rValue = -1;
    }
    return rValue;
}

//==================================================================================================
auto CDictionaryContainer::operator[](int message_number) const -> const std::string & {
    return GetEntry(message_number);
}
//==================================================================================================
auto CDictionaryContainer::operator[](int message_number) -> std::string & {
    return GetEntry(message_number);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CDictionaryContainer::GetEntry()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Retrieve a specified entry from a dictionary based on language code
// o------------------------------------------------------------------------------------------------o
auto CDictionaryContainer::GetEntry(const std::int32_t message_number, const unicodetypes_t toDisp)
-> std::string & {
    if (cwmWorldState->ServerData()->ServerLanguage() !=
        DL_DEFAULT) // defaultServerLang != DL_DEFAULT )
    {
        // If a default server language has been specified in uox.ini, force the use of that
        // language
        if (dictList[cwmWorldState->ServerData()->ServerLanguage()].GetValid()) {
            return dictList[cwmWorldState->ServerData()->ServerLanguage()][message_number];
        }
        else {
            return dictList[LanguageCodesLang[defaultLang]][message_number];
        }
    }
    
    auto typetouse = toDisp;
    if ((static_cast<std::int32_t>(toDisp) < 0) ||
        (static_cast<std::int32_t>(toDisp) >= unicodetypes_t::TOTAL_LANGUAGES)) {
        typetouse = ZERO;
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
    } catch (...) {
        return invalid_dictionary_string;
    }
}
//==================================================================================================
auto CDictionaryContainer::GetEntry(const std::int32_t message_number, const unicodetypes_t toDisp) const
-> const std::string & {
    if (cwmWorldState->ServerData()->ServerLanguage() !=
        DL_DEFAULT) // defaultServerLang != DL_DEFAULT )
    {
        // If a default server language has been specified in uox.ini, force the use of that
        // language
        if (dictList[cwmWorldState->ServerData()->ServerLanguage()].GetValid()) {
            return dictList[cwmWorldState->ServerData()->ServerLanguage()][message_number];
        }
        else {
            return dictList[LanguageCodesLang[defaultLang]][message_number];
        }
    }
    
    auto typetouse = toDisp;
    if ((static_cast<std::int32_t>(toDisp) < 0) ||
        (static_cast<std::int32_t>(toDisp) >= unicodetypes_t::TOTAL_LANGUAGES)) {
        typetouse = ZERO;
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
    } catch (...) {
        return invalid_dictionary_string;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CDictionaryContainer::SetDefaultLang()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Retrieve a specified entry from a dictionary based on language code
// o------------------------------------------------------------------------------------------------o
auto CDictionaryContainer::SetDefaultLang(unicodetypes_t newType) -> void {
    if (dictList[LanguageCodesLang[newType]].GetValid()) {
        defaultLang = newType;
    }
}