#ifndef __CSERVERDEFINITIONS__
#define __CSERVERDEFINITIONS__

#include <cstdint>
#include <map>
#include <stack>
#include <string>
#include <vector>

#include "typedefs.h"

class CScriptSection;
class Script;


class CDirectoryListing {
  private:

    auto PushDir(DEFINITIONCATEGORIES toMove) -> bool;
    auto PushDir(std::string toMove) -> bool;
    auto PopDir() -> void;

    std::vector<std::string> filenameList, shortList;
    std::vector<std::string> flattenedShort, flattenedFull;
    std::stack<std::string> dirs;
    std::string extension;
    std::string currentDir;
    std::string shortCurrentDir;

    std::vector<CDirectoryListing> subdirectories;
    bool doRecursion;

    void InternalRetrieve();

  public:
    CDirectoryListing(bool recurse = true);
    CDirectoryListing(const std::string &dir, const std::string &extent, bool recurse = true);
    CDirectoryListing(DEFINITIONCATEGORIES dir, const std::string &extent, bool recurse = true);
    ~CDirectoryListing();

    auto Extension(const std::string &extent) -> void;
    auto Retrieve(const std::string &dir) -> void;
    auto Retrieve(DEFINITIONCATEGORIES dir) -> void;
    auto Flatten(bool isParent) -> void;
    auto ClearFlatten() -> void;

    auto List() -> std::vector<std::string> *;
    auto ShortList() -> std::vector<std::string> *;
    auto FlattenedList() -> std::vector<std::string> *;
    auto FlattenedShortList() -> std::vector<std::string> *;
};

class CServerDefinitions {
  private:
    std::vector<Script *>::iterator slIter;

    std::map<std::string, std::int16_t> priorityMap;
    std::int16_t defaultPriority;

    auto LoadDFNCategory(DEFINITIONCATEGORIES toLoad) -> void;
    auto ReloadScriptObjects() -> void;
    auto BuildPriorityMap(DEFINITIONCATEGORIES category, std::uint8_t &wasPrioritized) -> void;
    auto CleanPriorityMap() -> void;

    auto GetPriority(const char *file) -> std::int16_t;

    auto Cleanup() -> void;

  public:
    std::vector<std::vector<Script *>> ScriptListings;

    CServerDefinitions();
    ~CServerDefinitions();
    auto Startup() -> void;
    auto Reload() -> bool;
    auto Dispose(DEFINITIONCATEGORIES toDispose) -> bool;

    auto FindEntry(const std::string &toFind, DEFINITIONCATEGORIES typeToFind) -> CScriptSection *;
    auto FindEntrySubStr(const std::string &toFind, DEFINITIONCATEGORIES typeToFind)
        -> CScriptSection *;
    auto CountOfEntries(DEFINITIONCATEGORIES typeToFind) -> size_t;
    auto CountOfFiles(DEFINITIONCATEGORIES typeToFind) -> size_t;
    auto DisplayPriorityMap() -> void;

    auto FirstScript(DEFINITIONCATEGORIES typeToFind) -> Script *;
    auto NextScript(DEFINITIONCATEGORIES typeToFind) -> Script *;
    auto FinishedScripts(DEFINITIONCATEGORIES typeToFind) -> bool;
};

extern CServerDefinitions *FileLookup;

#endif
