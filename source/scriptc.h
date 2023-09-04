#ifndef __SCRIPTC_H__
#define __SCRIPTC_H__

#include <cstdint>

#include <fstream>
#include <string>
#include <unordered_map>

#include "typedefs.h"

class CScriptSection;

class Script {
  public:
    std::uint32_t lastModTime;
    Script(const std::string &_filename, DEFINITIONCATEGORIES d, bool disp = true);
    ~Script();

    CScriptSection *FindEntry(const std::string &section);
    CScriptSection *FindEntrySubStr(const std::string &section);
    CScriptSection *FirstEntry();
    CScriptSection *NextEntry();
    auto collection() const -> const std::unordered_map<std::string, CScriptSection *> &;
    auto collection() -> std::unordered_map<std::string, CScriptSection *> &;

    bool IsInSection(const std::string &section);
    std::string EntryName(void);
    size_t NumEntries(void) const { return defEntries.size(); }
    bool IsErrored(void) const { return errorState; }

  private:
    void DeleteMap(void);
    void Reload(bool disp = true);
    bool CreateSection(std::string &name);

    std::unordered_map<std::string, CScriptSection *> defEntries; // string is the name of section
    std::unordered_map<std::string, CScriptSection *>::iterator iSearch;
    time_t last_modification;
    std::string filename;
    bool errorState;
    DEFINITIONCATEGORIES dfnCat;
    std::ifstream input;
};

#endif
