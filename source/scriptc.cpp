//
//	Cache script section locations
//	Reads through the contents of the file and saves location of each
//	SECTION XYZ entry

//	Calling Script::find() will then seek to that location directly rather
//	than having to parse through all of the script
//

#include "ssection.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#if !defined(_WIN32)
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#endif

#include "osunique.hpp"
#include "scriptc.h"
#include "stringutility.hpp"
#include "subsystem/console.hpp"
#include "utility/strutil.hpp"


// o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::Reload()
// o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Reload's the scripts data.  If disp is true, then
// information is output to
//|						the console (# of sections). Will not run if the script is
//in an erroneous state
// o------------------------------------------------------------------------------------------------o
void Script::Reload(bool disp) {
    if (!errorState) {
        // Clear the map, we are starting from scratch;
        DeleteMap();
        char line[2048];
        input.open(filename.string(), std::ios_base::in);
        if (input.is_open()) {
            std::string sLine;
            std::int32_t count = 0;
            while (!input.eof() && !input.fail()) {
                input.getline(line, 2047);
                line[input.gcount()] = 0;
                sLine = std::string(line);
                sLine = util::trim(util::strip(sLine, "//"));
                if (!sLine.empty()) {
                    // We have some real data
                    // see if in a section
                    if (sLine.substr(0, 1) == "[" && sLine.substr(sLine.size() - 1) == "]") {
                        // Ok a section is starting here, get the name
                        std::string sectionname = sLine.substr(1, sLine.size() - 2);
                        sectionname = util::upper(util::simplify(sectionname));
                        // Now why we look for a {, no idea, but we do - Because we want to make
                        // sure that were IN a block not before the block. At least this makes sure
                        // that were inside the {}'s of a block...
                        while (!input.eof() && sLine.substr(0, 1) != "{" && !input.fail()) {
                            input.getline(line, 2047);
                            line[input.gcount()] = 0;
                            sLine = std::string(line);
                            sLine = util::trim(util::strip(sLine, "//"));
                        }
                        // We are finally in the actual section!
                        // We waited until now to create it, incase a total invalid file
                        lastModTime = 0;
                        defEntries[sectionname] = std::make_unique< CScriptSection>(input, dfnCat);
                        ++count;
                    }
                }
            }
            input.close();
        }
        else {
            char buffer[200];
            std::cerr << "Cannot open " << filename.string() << ": "
            << std::string(mstrerror(buffer, 200, errno)) << std::endl;
            errorState = true;
        }
    }
    if (disp) {
        Console::shared().print(util::format("Reloading %-15s: ", filename.string().c_str()));
    }
    
    fflush(stdout);
}

// o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::Script()
// o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Builds the script, reading in the information from the
// script file.
// o------------------------------------------------------------------------------------------------o
Script::Script(const std::filesystem::path &filename, definitioncategories_t d, bool disp) : errorState(false), dfnCat(d) {
    this->filename = filename;
    if (!std::filesystem::exists(filename)){
        std::cerr << "Cannot open " << filename.string()  << std::endl;
        errorState = true;
    }
    last_modification = std::filesystem::last_write_time(this->filename);
    Reload(disp);
}
//===============================================================================================
auto Script::collection() const -> const std::unordered_map<std::string, std::unique_ptr<CScriptSection>> & { return defEntries; }
//===============================================================================================
auto Script::collection() -> std::unordered_map<std::string, std::unique_ptr<CScriptSection>> & { return defEntries; }

// o------------------------------------------------------------------------------------------------o
//|	Function		-	~Script()
// o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Destroys any memory that has been allocated
// o------------------------------------------------------------------------------------------------o
Script::~Script() { DeleteMap(); }

// o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::IsInSection()
// o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Returns true if the section named section is in the script
// o------------------------------------------------------------------------------------------------o
bool Script::IsInSection(const std::string &section) {
    std::string temp(section);
    auto iSearch = defEntries.find(util::upper(temp));
    if (iSearch != defEntries.end())
        return true;
    
    return false;
}

// o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::FindEntry()
// o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Returns a CScriptSection * to the section named "section"
//|						if it exists, otherwise returning nullptr
// o------------------------------------------------------------------------------------------------o
CScriptSection *Script::FindEntry(const std::string &section) {
    CScriptSection *rValue = nullptr;
    auto iSearch = defEntries.find(section);
    if (iSearch != defEntries.end()) {
        rValue = iSearch->second.get();
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::FindEntrySubStr()
// o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Find the first CScriptSection * (if any) that has the
//|						string section in the section name
// o------------------------------------------------------------------------------------------------o
CScriptSection *Script::FindEntrySubStr(const std::string &section) {
    CScriptSection *rValue = nullptr;
    auto usection = std::string(section);
    usection = util::upper(usection);
    for (auto iSearch = defEntries.begin(); iSearch != defEntries.end();
         ++iSearch) {
        if (iSearch->first.find(usection) != std::string::npos) // FOUND IT!
        {
            rValue = iSearch->second.get();
            break;
        }
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::FirstEntry()
// o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Returns the first CScriptSection in the Script (if any)
// o------------------------------------------------------------------------------------------------o
CScriptSection *Script::FirstEntry() {
    CScriptSection *rValue = nullptr;
    iSearch = defEntries.begin();
    if (iSearch != defEntries.end()) {
        rValue = iSearch->second.get();
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::NextEntry()
// o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Returns the next CScriptSection (if any) in the Script
// o------------------------------------------------------------------------------------------------o
CScriptSection *Script::NextEntry() {
    CScriptSection *rValue = nullptr;
    if (iSearch != defEntries.end()) {
        ++iSearch;
        if (iSearch != defEntries.end()) {
            rValue = iSearch->second.get();
        }
    }
    return rValue;
}

// o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::DeleteMap()
// o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Destroys any memory that has been allocated
// o------------------------------------------------------------------------------------------------o
void Script::DeleteMap() {
    defEntries.clear();
}

// o------------------------------------------------------------------------------------------------o
//|	Function		-	Script::EntryName()
// o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Returns the section name for the current entry (if any)
// o------------------------------------------------------------------------------------------------o
std::string Script::EntryName() {
    std::string rValue;
    if (iSearch != defEntries.end()) {
        rValue = iSearch->first;
    }
    return rValue;
}
