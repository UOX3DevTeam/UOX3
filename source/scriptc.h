#ifndef __SCRIPTC_H__
#define __SCRIPTC_H__

#include <cstdint>

#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>

#include "typedefs.h"

class CScriptSection;

class Script {
public:
    std::uint32_t lastModTime;
    Script(const std::string &_filename, definitioncategories_t d, bool disp = true);
    ~Script();
    
    CScriptSection *FindEntry(const std::string &section);
    CScriptSection *FindEntrySubStr(const std::string &section);
    CScriptSection *FirstEntry();
    CScriptSection *NextEntry();
    auto collection() const -> const std::unordered_map<std::string, std::unique_ptr<CScriptSection>> &;
    auto collection() -> std::unordered_map<std::string, std::unique_ptr<CScriptSection>> &;
    
    bool IsInSection(const std::string &section);
    std::string EntryName();
    size_t NumEntries() const { return defEntries.size(); }
    bool IsErrored() const { return errorState; }
    
private:
    void DeleteMap();
    void Reload(bool disp = true);
    bool CreateSection(std::string &name);
    
    std::unordered_map<std::string, std::unique_ptr<CScriptSection> > defEntries; // string is the name of section
    std::unordered_map<std::string, std::unique_ptr<CScriptSection>>::iterator iSearch;
    time_t last_modification;
    std::string filename;
    bool errorState;
    definitioncategories_t dfnCat;
    std::ifstream input;
};

#endif
