
#include "cjsmapping.h"

#include <memory>

#include "subsystem/console.hpp"
#include "cscript.h"
#include "funcdecl.h"
#include "scriptc.h"
#include "configuration/serverconfig.hpp"
#include "ssection.h"
#include "stringutility.hpp"
#include "utility/strutil.hpp"

using namespace std::string_literals;


//================================================================================================
// CJSMappingSection
//================================================================================================

CJSMapping *JSMapping = nullptr;
// o------------------------------------------------------------------------------------------------o
//|	File		-	cjsmapping.cpp
//|	Date		-	Feb 7, 2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	JavaScript File Mapping
// o------------------------------------------------------------------------------------------------o
//| Changes		-	Version History
//|					1.0			 		Feb 7, 2005
//|					Initial Implementation
//|
//|					1.1			 		Feb 8, 2005
//|					Added the ability to reload JS files on a per-script basis
//|
//|					1.2			 		Feb 28, 2005
//|					Added the ability to reload JS files on a per-section basis
// o------------------------------------------------------------------------------------------------o

// o------------------------------------------------------------------------------------------------o
//|	Class		-	CJSMapping
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Base global class that holds within it an array of JS Mapping
// Sections
// o------------------------------------------------------------------------------------------------o
/*
CJSMapping::CJSMapping()
{
}
 */
CJSMapping::~CJSMapping() {
    // Cleanup();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::ResetDefaults()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Resets all parameters of the CJSMapping class to default
// o------------------------------------------------------------------------------------------------o
void CJSMapping::ResetDefaults() {
    mapSection = std::vector<CJSMappingSection *>(CJSMappingSection::ScriptNames.size(),nullptr);
    for (size_t i= 0 ; i < CJSMappingSection::ScriptNames.size();i++){
        mapSection[i] = new CJSMappingSection(static_cast<CJSMappingSection::type_t>(i));
    }

    envokeById = new CEnvoke("object");
    envokeByType = new CEnvoke("type");

    Parse();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::Cleanup()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Frees all memory used by CJSMapping
// o------------------------------------------------------------------------------------------------o
void CJSMapping::Cleanup() {
    for (size_t i = CJSMappingSection::SCPT_NORMAL; i < CJSMappingSection::ScriptNames.size(); ++i) {
        if (mapSection[1] != nullptr) {
            delete mapSection[i];
            mapSection[i] = nullptr;
        }
    }

    if (envokeById != nullptr) {
        delete envokeById;
        envokeById = nullptr;
    }
    if (envokeByType != nullptr) {
        delete envokeByType;
        envokeByType = nullptr;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::Reload()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reloads the JS Scripts
// o------------------------------------------------------------------------------------------------o
void CJSMapping::Reload(std::uint16_t scriptId) {
    if (scriptId != 0xFFFF) {
        Console::shared().print(
            util::format("CMD: Attempting Reload of JavaScript (ScriptId %u)\n", scriptId));
        for (size_t i = CJSMappingSection::SCPT_NORMAL; i < CJSMappingSection::ScriptNames.size(); ++i) {
            if (mapSection[i]->IsInMap(scriptId)) {
                mapSection[i]->Reload(scriptId);
                return;
            }
        }
        Console::shared().warning(util::format(
            "Unable to locate specified JavaScript in the map (ScriptId %u)", scriptId));
    }
    else {
        Console::shared().print(util::format("CMD: Loading JSE Scripts... \n"));
        Cleanup();
        ResetDefaults();
        envokeById->Parse();
        envokeByType->Parse();
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::Reload()
//|	Date		-	2/28/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reloads a specific section of the JS Scripts
// o------------------------------------------------------------------------------------------------o
void CJSMapping::Reload(CJSMappingSection::type_t sectionId) {
    Console::shared().print(util::format("CMD: Attempting Reload of JavaScript (SectionId %u)\n",
                                         static_cast<std::int32_t>(sectionId)));
    if (mapSection[sectionId] != nullptr) {
        delete mapSection[sectionId];
        mapSection[sectionId] = new CJSMappingSection(sectionId);

        Parse(sectionId);
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::Parse()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Parses through jse_fileassociations.scp doling out the work
//|						to each CJSMappingSection Parse() routine.
// o------------------------------------------------------------------------------------------------o
void CJSMapping::Parse(CJSMappingSection::type_t toParse) {
    Console::shared().print("Loading JS Scripts\n");

    auto scpFileName = ServerConfig::shared().directoryFor(dirlocation_t::SCRIPT) / std::filesystem::path("jse_fileassociations.scp");
    if (!std::filesystem::exists(scpFileName)) {
        Console::shared().error(util::format("Failed to open %s", scpFileName.string().c_str()));
        return;
    }

    Script *fileAssocData = new Script(scpFileName, NUM_DEFS, false);
    if (fileAssocData != nullptr) {
        if (toParse != CJSMappingSection::ScriptNames.size()) {
            if (mapSection[toParse] != nullptr) {
                mapSection[toParse]->Parse(fileAssocData);
            }
        }
        else {
            for (size_t i = CJSMappingSection::SCPT_NORMAL; i < CJSMappingSection::ScriptNames.size(); ++i) {
                if (mapSection[i] != nullptr) {
                    mapSection[i]->Parse(fileAssocData);
                }
            }
        }

        delete fileAssocData;
    }
}


//================================================================================================
// CJSMappingSection
//================================================================================================
const std::vector<std::string> CJSMappingSection::ScriptNames{
    "SCRIPT_LIST"s,"COMMAND_SCRIPTS"s,
    "MAGIC_SCRIPTS"s,  "SKILLUSE_SCRIPTS"s,
    "PACKET_SCRIPTS"s, "CONSOLE_SCRIPTS"s
    
};


// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::GetSection()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a pointer to the specified JSMappingSection
// o------------------------------------------------------------------------------------------------o
CJSMappingSection *CJSMapping::GetSection(CJSMappingSection::type_t toGet) {
    if (mapSection[toGet] != nullptr)
        return mapSection[toGet];

    return nullptr;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::GetScriptId()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the scriptId relating to the specified JSObject
// o------------------------------------------------------------------------------------------------o
std::uint16_t CJSMapping::GetScriptId(JSObject *toFind) {
    std::uint16_t retVal = 0xFFFF;

    for (size_t i = CJSMappingSection::SCPT_NORMAL; i < CJSMappingSection::ScriptNames.size(); ++i) {
        retVal = mapSection[i]->GetScriptId(toFind);
        if (retVal != 0xFFFF)
            break;
    }
    return retVal;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::GetScript()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the cScript relating to the specified JSObject
// o------------------------------------------------------------------------------------------------o
cScript *CJSMapping::GetScript(JSObject *toFind) {
    cScript *retVal = nullptr;
    cScript *toCheck = nullptr;

    for (size_t i = CJSMappingSection::SCPT_NORMAL; i < CJSMappingSection::ScriptNames.size(); ++i) {
        toCheck = mapSection[i]->GetScript(toFind);
        if (toCheck != nullptr) {
            retVal = toCheck;
            break;
        }
    }
    return retVal;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::GetScript()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the cScript relating to the specified scriptId
// o------------------------------------------------------------------------------------------------o
cScript *CJSMapping::GetScript(std::uint16_t toFind) {
    cScript *retVal = nullptr;
    cScript *toCheck = nullptr;

    for (size_t i = CJSMappingSection::SCPT_NORMAL; i < CJSMappingSection::ScriptNames.size(); ++i) {
        toCheck = mapSection[i]->GetScript(toFind);
        if (toCheck != nullptr) {
            retVal = toCheck;
            break;
        }
    }
    return retVal;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::GetEnvokeById()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a pointer to the CEnvoke class handling EnvokeByID
// o------------------------------------------------------------------------------------------------o
CEnvoke *CJSMapping::GetEnvokeById() { return envokeById; }
// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::GetEnvokeByType()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a pointer to the CEnvoke class handling EnvokeByType
// o------------------------------------------------------------------------------------------------o
CEnvoke *CJSMapping::GetEnvokeByType() { return envokeByType; }

// o------------------------------------------------------------------------------------------------o
//|	Class		-	CJSMappingSection::CJSMappingSection()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Class containing the specified section of jse_fileassociations.scp
// o------------------------------------------------------------------------------------------------o
CJSMappingSection::CJSMappingSection(CJSMappingSection::type_t sT) {
    scriptType = sT;

    scriptIdMap.clear();
    scriptJSMap.clear();

    scriptIdIter = scriptIdMap.end();
}
CJSMappingSection::~CJSMappingSection() {

    for (std::map<std::uint16_t, cScript *>::const_iterator sIter = scriptIdMap.begin();
         sIter != scriptIdMap.end(); ++sIter) {
        cScript *toDelete = sIter->second;
        if (toDelete != nullptr) {
            delete toDelete;
        }
    }

    scriptIdMap.clear();
    scriptJSMap.clear();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::Parse()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Parses through specified section of jse_fileassociations.scp
//|						populating the map with each entry.
// o------------------------------------------------------------------------------------------------o
auto CJSMappingSection::Parse(Script *fileAssocData) -> void {
    auto basePath = ServerConfig::shared().directoryFor(dirlocation_t::SCRIPT);
    auto mSection = fileAssocData->FindEntry(ScriptNames[scriptType]);
    std::uint8_t runTime = 0;

    if (scriptType == SCPT_CONSOLE) {
        runTime = 1;
    }

    if (mSection) {
        std::uint16_t scriptId = 0xFFFF;
        size_t i = 0;
        for (const auto &sec : mSection->collection()) {
            auto tag = sec->tag;
            auto data = sec->data;

            scriptId = static_cast<std::uint16_t>(std::stoul(tag, nullptr, 0));
            auto fullPath = basePath / std::filesystem::path(data).make_preferred();

            if (!std::filesystem::exists(fullPath)) {
                Console::shared().error(util::format( "SE mapping of %i to %s failed, file does not exist!", scriptId, data.c_str()));
            }
            else {
                try {
                    auto toAdd = new cScript(fullPath, runTime);
                    if (toAdd) {
                        scriptIdMap[scriptId] = toAdd;
                        scriptJSMap[toAdd->Object()] = scriptId;
                        ++i;
                    }
                } catch (std::runtime_error &e) {
                    Console::shared().error( util::format("Compiling %s caused a construction failure (Details: %s)", fullPath.string().c_str(), e.what()));
                }
            }
        }
        Console::shared().print("  o Loaded ");
        Console::shared().turnYellow();
        Console::shared().print(util::format("%4u ", i));
        Console::shared().turnNormal();
        Console::shared().print("scripts from section ");
        Console::shared().turnYellow();
        Console::shared().print(util::format("%s\n", ScriptNames[scriptType].c_str()));
        Console::shared().turnNormal();
    }
    else {
        Console::shared().warning(util::format("No JS file mappings found in section %s", ScriptNames[scriptType].c_str()));
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::Reload()
//|	Date		-	2/8/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reloads the specified JS file (by its scriptId)
// o------------------------------------------------------------------------------------------------o
auto CJSMappingSection::Reload(std::uint16_t toLoad) -> void {
    auto scpFileName = ServerConfig::shared().directoryFor(dirlocation_t::SCRIPT) / std::filesystem::path("jse_fileassociations.scp"s);
    if (!std::filesystem::exists(scpFileName)) {
        Console::shared().error(util::format("Failed to open %s", scpFileName.string().c_str()));
        return;
    }

    auto fileAssocData = std::make_unique<Script>(scpFileName, NUM_DEFS, false);
    if (fileAssocData) {
        auto mSection = fileAssocData->FindEntry(ScriptNames[scriptType]);
        if (mSection) {
            std::uint16_t scriptId = 0xFFFF;
            auto basePath = ServerConfig::shared().directoryFor(dirlocation_t::SCRIPT);
            std::string data ;
            auto fullPath = std::filesystem::path();
            std::uint8_t runTime = 0;
            if (scriptType == SCPT_CONSOLE) {
                runTime = 1;
            }
            for (const auto &sec : mSection->collection()) {
                auto tag = sec->tag;
                scriptId = static_cast<std::uint16_t>(std::stoul(tag, nullptr, 0));
                if (scriptId == toLoad) {
                    data = sec->data;
                    fullPath = basePath / std::filesystem::path(data).make_preferred();

                    if (std::filesystem::exists(fullPath)) {
                        Console::shared().error(util::format("SE mapping of %i to %s failed, file does not exist!",scriptId, data.c_str()));
                    }
                    else {
                        try {
                            std::map<std::uint16_t, cScript *>::const_iterator iFind = scriptIdMap.find(toLoad);
                            if (iFind != scriptIdMap.end()) {
                                if (scriptIdMap[toLoad]) {
                                    JSObject *jsObj = scriptIdMap[toLoad]->Object();
                                    std::map<JSObject *, std::uint16_t>::iterator jFind = scriptJSMap.find(jsObj);
                                    if (jFind != scriptJSMap.end()) {
                                        scriptJSMap.erase(jFind);
                                    }

                                    delete scriptIdMap[toLoad];
                                    scriptIdMap[toLoad] = nullptr;
                                }
                            }
                            auto toAdd = new cScript(fullPath, runTime);
                            if (toAdd) {
                                scriptIdMap[scriptId] = toAdd;
                                scriptJSMap[toAdd->Object()] = scriptId;
                                Console::shared().print(util::format( "Reload of JavaScript (ScriptId %u) Successful\n", toLoad));
                            }
                        } catch (std::runtime_error &e) {
                            Console::shared().error(util::format( "Compiling %s caused a construction failure (Details: %s)", fullPath.string().c_str(), e.what()));
                        }
                    }
                    return;
                }
            }
            Console::shared().warning(util::format( "Unable to locate the specified JavaScript in the file (ScriptId %u)", toLoad));
        }
        else {
            Console::shared().warning(util::format("No JS file mappings found in section %s", ScriptNames[scriptType].c_str()));
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::IsInMap()
//|	Date		-	2/8/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if the specified scriptId exists in the map
// o------------------------------------------------------------------------------------------------o
bool CJSMappingSection::IsInMap(std::uint16_t scriptId) {
    bool retVal = false;

    std::map<std::uint16_t, cScript *>::const_iterator sIter = scriptIdMap.find(scriptId);
    if (sIter != scriptIdMap.end()) {
        retVal = true;
    }

    return retVal;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::GetScriptId()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the scriptId associated with the specified JSObject
// o------------------------------------------------------------------------------------------------o
std::uint16_t CJSMappingSection::GetScriptId(JSObject *toFind) {
    std::uint16_t retVal = 0xFFFF;

    std::map<JSObject *, std::uint16_t>::const_iterator sIter = scriptJSMap.find(toFind);
    if (sIter != scriptJSMap.end()) {
        retVal = sIter->second;
    }

    return retVal;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::GetScript()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the cScript associated with the specified scriptId
// o------------------------------------------------------------------------------------------------o
cScript *CJSMappingSection::GetScript(std::uint16_t toFind) {
    cScript *retVal = nullptr;

    std::map<std::uint16_t, cScript *>::const_iterator idIter = scriptIdMap.find(toFind);
    if (idIter != scriptIdMap.end()) {
        retVal = idIter->second;
    }

    return retVal;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::GetScript()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the cScript associated with the specified JSObject
// o------------------------------------------------------------------------------------------------o
cScript *CJSMappingSection::GetScript(JSObject *toFind) {
    std::uint16_t scriptId = GetScriptId(toFind);
    return GetScript(scriptId);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::First()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the first cScript in the scriptIdMap
// o------------------------------------------------------------------------------------------------o
cScript *CJSMappingSection::First() {
    scriptIdIter = scriptIdMap.begin();
    if (!Finished())
        return scriptIdIter->second;

    return nullptr;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::Next()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the next cScript in the scriptIdMap
// o------------------------------------------------------------------------------------------------o
cScript *CJSMappingSection::Next() {
    if (!Finished()) {
        ++scriptIdIter;
        if (!Finished())
            return scriptIdIter->second;
    }
    return nullptr;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::Finished()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Tells us when we have reached the end of the scriptIdMap
// o------------------------------------------------------------------------------------------------o
bool CJSMappingSection::Finished() { return (scriptIdIter == scriptIdMap.end()); }

// o------------------------------------------------------------------------------------------------o
//|	Class		-	CEnvoke::CEnvoke()
//|	Date		-	2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Class containing the specified envoke file
// o------------------------------------------------------------------------------------------------o
CEnvoke::CEnvoke(const std::string &eT) {
    envokeType = eT;
    envokeList.clear();
}
CEnvoke::~CEnvoke() { envokeList.clear(); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CEnvoke::Check()
//|	Date		-	2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified envokeId is in the map
// o------------------------------------------------------------------------------------------------o
bool CEnvoke::Check(std::uint16_t envokeId) const {
    std::map<std::uint16_t, std::uint16_t>::const_iterator p = envokeList.find(envokeId);
    return (p != envokeList.end());
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CEnvoke::GetScript()
//|	Date		-	2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns specified envokeId
// o------------------------------------------------------------------------------------------------o
std::uint16_t CEnvoke::GetScript(std::uint16_t envokeId) const {
    std::map<std::uint16_t, std::uint16_t>::const_iterator p = envokeList.find(envokeId);
    if (p != envokeList.end())
        return p->second;

    return 0xFFFF;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	CEnvoke::Parse()
//|	Date		-	2/7/2005
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Parses through specific envoke file mapping the scriptId to the
// envoke type
// o------------------------------------------------------------------------------------------------o
auto CEnvoke::Parse() -> void {
    envokeList.clear();
    
    auto filename = ServerConfig::shared().directoryFor(dirlocation_t::SCRIPT) / std::filesystem::path("jse_"s + envokeType + "associations.scp"s);
    if (!std::filesystem::exists(filename)){
        Console::shared() << "Unable to open " << filename.string() << " for parsing" << myendl;
        return;
    }

    auto fileAssocData = new Script(filename, NUM_DEFS, false);
    if (fileAssocData) {
        auto mSection = fileAssocData->FindEntry("ENVOKE");
        if (mSection) {
            for (const auto &sec : mSection->collection()) {
                auto tag = sec->tag;
                if (!tag.empty() && tag != "\n" && tag != "\r") {
                    auto data = sec->data;
                    auto envokeId = static_cast<std::uint16_t>(std::stoul(tag, nullptr, 0));
                    auto scriptId = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                    auto verify = JSMapping->GetScript(scriptId);
                    if (verify) {
                        envokeList[envokeId] = scriptId;
                    }
                    else {
                        Console::shared().error(util::format("(ENVOKE) Item %s refers to scriptId %u which does not exist.",tag.c_str(), scriptId));
                    }
                }
            }
        }
        else {
            Console::shared().warning("Envoke section not found, no hard id->script matching being done");
        }
        delete fileAssocData;
    }
}
