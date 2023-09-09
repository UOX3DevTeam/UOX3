// dictionary.h: interface for the CDictionary class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__
#include <array>
#include <cstdint>
#include <filesystem>
#include <map>
#include <string>

#include "typedefs.h"

// o------------------------------------------------------------------------------------------------o
//  CDictionary
// o------------------------------------------------------------------------------------------------o
class CDictionary {
public:
    ~CDictionary() = default;
    CDictionary(const std::string &filepath = "./dictionary/dictionary.ZRO",
                const std::string &language = "ZRO");
    auto SetLocationLanguage(const std::string &filepath, const std::string &language) -> void;
    
    auto operator[](int message_number) const -> const std::string &;
    auto operator[](int message_number) -> std::string &;
    auto GetEntry(int message_number) const -> const std::string &;
    auto GetEntry(int message_number) -> std::string &;
    
    auto ShowList() -> void;
    auto LoadDictionary(const std::filesystem::path &filepath = std::filesystem::path(), const std::string &language = "") -> std::int32_t;
    auto NumberOfEntries() const -> size_t;
    auto GetValid() const -> bool;
    
private:
    auto ParseFile(const std::filesystem::path &dictionaryfile) -> bool;
    std::filesystem::path pathToDictionary;
    std::string dictLanguage;
    std::map<int, std::string>
    msgdata; // The key here really needs to match the index in operator[]
};

//===================================================================================================
// CDictionaryContainer
//===================================================================================================
class CDictionaryContainer {
private:
    std::array<CDictionary, static_cast<int>(DL_COUNT)> dictList;
    unicodetypes_t defaultLang;
    
public:
    CDictionaryContainer();
    ~CDictionaryContainer() = default;
    auto LoadDictionaries(const std::filesystem::path &filepath = std::filesystem::path()) -> int;
    auto SetDefaultLang(unicodetypes_t newType) -> void;
    
    auto operator[](int message_number) const -> const std::string &; // only default lang
    auto operator[](int message_number) -> std::string &;             // only default lang
    auto GetEntry(int message_number, const unicodetypes_t toDisp = ZERO) const
    -> const std::string &;
    auto GetEntry(int message_number, const unicodetypes_t toDisp = ZERO) -> std::string &;
};

extern CDictionaryContainer *Dictionary;

#endif
