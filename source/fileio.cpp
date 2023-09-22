#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <vector>
#if !defined(_WIN32)
#include <fcntl.h>    // open
#include <sys/mman.h> // mmap, mmunmap
#endif

#include "subsystem/console.hpp"
#include "cserverdefinitions.h"
#include "cspawnregion.h"
#include "funcdecl.h"

#include "scriptc.h"
#include "configuration/serverconfig.hpp"
#include "ssection.h"
#include "stringutility.hpp"
#include "townregion.h"
#include "utility/strutil.hpp"


// o------------------------------------------------------------------------------------------------o
//|	Function	-	loadCustomTitle()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads players titles (Karma, Fame, Murder, ect)
// o------------------------------------------------------------------------------------------------o
void loadCustomTitle() {
    size_t titlecount = 0;
    std::string tag;
    std::string data;
    CScriptSection *CustomTitle = FileLookup->FindEntry("SKILL", titles_def);
    if (CustomTitle == nullptr) {
        return;
    }
    for (tag = CustomTitle->First(); !CustomTitle->AtEnd() && titlecount < ALLSKILLS; tag = CustomTitle->Next()) {
        data = CustomTitle->GrabData();
        worldMain.title[titlecount].skill = data;
        ++titlecount;
    }
    CustomTitle = FileLookup->FindEntry("PROWESS", titles_def);
    if (CustomTitle == nullptr) {
        return;
    }
    for (tag = CustomTitle->First(); !CustomTitle->AtEnd(); tag = CustomTitle->Next()) {
        data = CustomTitle->GrabData();
        worldMain.prowessTitles.push_back(TitlePair(static_cast<std::int16_t>(std::stoi(tag, nullptr, 0)), data));
    }
    
    CustomTitle = FileLookup->FindEntry("FAME", titles_def);
    if (CustomTitle == nullptr) {
        return;
    }
    titlecount = 0;
    
    for (tag = CustomTitle->First(); !CustomTitle->AtEnd() && titlecount < ALLSKILLS;
         tag = CustomTitle->Next()) {
        data = CustomTitle->GrabData();
        worldMain.title[titlecount].fame = data;
        ++titlecount;
    }
    
    // Murder tags now scriptable in SECTION MURDER - Titles.dfn
    CustomTitle = FileLookup->FindEntry("MURDERER", titles_def);
    if (CustomTitle == nullptr) {
        return;
    }
    for (tag = CustomTitle->First(); !CustomTitle->AtEnd(); tag = CustomTitle->Next()) {
        data = CustomTitle->GrabData();
        worldMain.murdererTags.push_back(TitlePair(static_cast<std::int16_t>(std::stoi(tag, nullptr, 0)), data));
    }
    
    FileLookup->Dispose(titles_def);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	loadSkills()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load skills from definition files
// o------------------------------------------------------------------------------------------------o
void loadSkills() {
    std::string skEntry;
    std::string tag, data, UTag;
    std::uint8_t i = 0;
    for (Script *creatScp = FileLookup->FirstScript(skills_def); !FileLookup->FinishedScripts(skills_def); creatScp = FileLookup->NextScript(skills_def)) {
        if (creatScp == nullptr)
            continue;
        
        for (CScriptSection *SkillList = creatScp->FirstEntry(); SkillList != nullptr; SkillList = creatScp->NextEntry()) {
            if (SkillList == nullptr)
                continue;
            
            skEntry = creatScp->EntryName();
            auto ssecs = oldstrutil::sections(skEntry, " ");
            if (util::trim(util::strip(ssecs[0], "// ")) == "SKILL") {
                if (ssecs.size() > 1) {
                    i = static_cast<std::uint8_t>(std::stoul(util::trim(util::strip(ssecs[1], "//"))));
                    if (i <= INTELLECT) {
                        worldMain.skill[i].ResetDefaults();
                        for (tag = SkillList->First(); !SkillList->AtEnd(); tag = SkillList->Next()) {
                            UTag = util::upper(tag);
                            data = SkillList->GrabData();
                            data = util::trim(util::strip(data, "//"));
                            if (UTag == "STR") {
                                worldMain.skill[i].strength = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                            }
                            else if (UTag == "DEX") {
                                worldMain.skill[i].dexterity = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                            }
                            else if (UTag == "INT") {
                                worldMain.skill[i].intelligence = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                            }
                            else if (UTag == "SKILLPOINT") {
                                Advance tempAdvance;
                                data = util::simplify(data);
                                auto csecs = oldstrutil::sections(data, ",");
                                tempAdvance.base = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[0], "//")), nullptr, 0));
                                tempAdvance.success = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[1], "//")), nullptr, 0));
                                tempAdvance.failure = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[2], "//")), nullptr, 0));
                                if (csecs.size() == 4) {
                                    tempAdvance.amtToGain = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[3], "//")), nullptr, 0));
                                }
                                worldMain.skill[i].advancement.push_back(tempAdvance);
                            }
                            else if (UTag == "SKILLDELAY") {
                                worldMain.skill[i].skillDelay = static_cast<std::int32_t>(std::stoi(data, nullptr, 0));
                            }
                            else if (UTag == "MADEWORD") {
                                worldMain.skill[i].madeWord = data;
                            }
                            else if (UTag == "NAME") {
                                worldMain.skill[i].name = data;
                            }
                            else {
                                Console::shared().warning(util::format("Unknown tag in skills.dfn: %s", data.c_str()));
                            }
                        }
                    }
                }
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	loadSpawnRegions()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads spawning regions from definition files
// o------------------------------------------------------------------------------------------------o
void loadSpawnRegions() {
    worldMain.spawnRegions.clear();
    std::uint16_t i = 0;
    for (Script *spnScp = FileLookup->FirstScript(spawn_def);!FileLookup->FinishedScripts(spawn_def); spnScp = FileLookup->NextScript(spawn_def)) {
        if (spnScp == nullptr)
            continue;
        
        for (CScriptSection *toScan = spnScp->FirstEntry(); toScan != nullptr;toScan = spnScp->NextEntry()) {
            if (toScan == nullptr)
                continue;
            
            std::string sectionName = spnScp->EntryName();
            auto ssecs = oldstrutil::sections(sectionName, " ");
            if ("REGIONSPAWN" == ssecs[0]) // Is it a region spawn entry?
            {
                i = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(ssecs[1], "//")), nullptr, 0));
                if (worldMain.spawnRegions.find(i) == worldMain.spawnRegions.end()) {
                    worldMain.spawnRegions[i] = new CSpawnRegion(i);
                    worldMain.spawnRegions[i]->load(toScan);
                }
                else {
                    Console::shared().warning(util::format("spawn.dfn has a duplicate REGIONSPAWN entry, Entry Number: %u", i));
                }
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	loadRegions()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load regions from regions.dfn and townregions from regions.wsc
// o------------------------------------------------------------------------------------------------o
void loadRegions() {
    worldMain.townRegions.clear();
    auto regionsFile = ServerConfig::shared().directoryFor(dirlocation_t::SAVE) / std::filesystem::path("regions.wsc");
    bool performLoad = false;
    Script *ourRegions = nullptr;
    if (std::filesystem::exists(regionsFile)) {
        performLoad = true;
        ourRegions = new Script(regionsFile, NUM_DEFS, false);
    }
    
    std::uint16_t i = 0;
    std::string regEntry;
    for (Script *regScp = FileLookup->FirstScript(regions_def);!FileLookup->FinishedScripts(regions_def); regScp = FileLookup->NextScript(regions_def)) {
        if (regScp == nullptr)
            continue;
        
        for (CScriptSection *toScan = regScp->FirstEntry(); toScan != nullptr; toScan = regScp->NextEntry()) {
            if (toScan == nullptr)
                continue;
            
            regEntry = regScp->EntryName();
            auto ssecs = oldstrutil::sections(regEntry, " ");
            if (util::trim(util::strip(ssecs[0], "//")) == "REGION") {
                i = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(ssecs[1], "//")), nullptr, 0));
                if (worldMain.townRegions.find(i) == worldMain.townRegions.end()) {
                    worldMain.townRegions[i] = new CTownRegion(i);
                    worldMain.townRegions[i]->InitFromScript(toScan);
                    if (performLoad) {
                        worldMain.townRegions[i]->load(ourRegions);
                    }
                }
                else {
                    Console::shared().warning(util::format("regions.dfn has a duplicate REGION entry, Entry Number: %u", i));
                }
            }
        }
    }
    if (regEntry == "") {
        // No regions found? :O Shut down UOX3, or we'll run into trouble later.
        Console::shared().printFailed();
        Shutdown(FATAL_UOX3_ALLOC_MAPREGIONS);
    }
    
    if (performLoad) {
        delete ourRegions;
        ourRegions = nullptr;
    }
    
    // Load Instant Logout regions from [INSTALOG] section of regions.dfn
    // Note that all the tags below are required to setup valid locations
    CScriptSection *InstaLog = FileLookup->FindEntry("INSTALOG", regions_def);
    if (InstaLog == nullptr)
        return;
    
    LogoutLocationEntry_st toAdd;
    std::string data, UTag;
    for (std::string tag = InstaLog->First(); !InstaLog->AtEnd(); tag = InstaLog->Next()) {
        UTag = util::upper(tag);
        data = InstaLog->GrabData();
        data = util::trim(util::strip(data, "//"));
        if (UTag == "X1") {
            toAdd.x1 = static_cast<std::int16_t>(std::stoi(data, nullptr, 0));
        }
        else if (UTag == "Y1") {
            toAdd.y1 = static_cast<std::int16_t>(std::stoi(data, nullptr, 0));
        }
        else if (UTag == "X2") {
            toAdd.x2 = static_cast<std::int16_t>(std::stoi(data, nullptr, 0));
        }
        else if (UTag == "Y2") {
            toAdd.y2 = static_cast<std::int16_t>(std::stoi(data, nullptr, 0));
        }
        else if (UTag == "WORLD") {
            toAdd.worldNum = static_cast<std::int8_t>(std::stoi(data, nullptr, 0));
        }
        else if (UTag == "INSTANCEID") {
            toAdd.instanceId = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
            worldMain.logoutLocs.push_back(toAdd);
        }
    }
    
    // Load areas valid for SOS coordinates from [SOSAREAS] section of regions.dfn
    // Note that all the tags below are required to setup valid locations
    CScriptSection *sosAreas = FileLookup->FindEntry("SOSAREAS", regions_def);
    if (sosAreas == nullptr)
        return;
    
    SOSLocationEntry toAddSOS;
    for (std::string tag = sosAreas->First(); !sosAreas->AtEnd(); tag = sosAreas->Next()) {
        UTag = util::upper(tag);
        data = sosAreas->GrabData();
        data = util::trim(util::strip(data, "//"));
        if (UTag == "X1") {
            toAddSOS.x1 = static_cast<std::int16_t>(std::stoi(data, nullptr, 0));
        }
        else if (UTag == "Y1") {
            toAddSOS.y1 = static_cast<std::int16_t>(std::stoi(data, nullptr, 0));
        }
        else if (UTag == "X2") {
            toAddSOS.x2 = static_cast<std::int16_t>(std::stoi(data, nullptr, 0));
        }
        else if (UTag == "Y2") {
            toAddSOS.y2 = static_cast<std::int16_t>(std::stoi(data, nullptr, 0));
        }
        else if (UTag == "WORLD") {
            toAddSOS.worldNum = static_cast<std::int8_t>(std::stoi(data, nullptr, 0));
        }
        else if (UTag == "INSTANCEID") {
            toAddSOS.instanceId = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
            worldMain.sosLocs.push_back(toAddSOS);
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	loadTeleportLocations()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load teleport locations from definition files
// o------------------------------------------------------------------------------------------------o
void loadTeleportLocations() {
    auto filename = ServerConfig::shared().directoryFor(dirlocation_t::SCRIPT) / std::filesystem::path("teleport.scp");
    worldMain.teleLocs.resize(0);
    
    if (!std::filesystem::exists(filename)) {
        Console::shared() << myendl;
        Console::shared().error( util::format(" Failed to open teleport data script %s", filename.string().c_str()));
        Console::shared().error(util::format(" Teleport Data not found"));
        worldMain.SetKeepRun(false);
        worldMain.SetError(true);
        return;
    }
    
    Script *teleportData = new Script(filename, NUM_DEFS, false);
    if (teleportData != nullptr) {
        worldMain.teleLocs.reserve(teleportData->NumEntries());
        
        std::uint16_t tempX, tempY;
        std::int8_t tempZ;
        CScriptSection *teleportSect = nullptr;
        std::string tag, data, temp;
        for (teleportSect = teleportData->FirstEntry(); teleportSect != nullptr;teleportSect = teleportData->NextEntry()) {
            if (teleportSect != nullptr) {
                for (tag = teleportSect->First(); !teleportSect->AtEnd();tag = teleportSect->Next()) {
                    CTeleLocationEntry toAdd;
                    if (util::upper(tag) == "ENTRY") {
                        tempX = 0;
                        tempY = 0;
                        tempZ = ILLEGAL_Z;
                        data = util::simplify(teleportSect->GrabData());
                        auto csecs = oldstrutil::sections(data, ",");
                        std::int32_t sectCount = static_cast<std::int32_t>(csecs.size());
                        if (sectCount >= 6) {
                            tempX = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[0], "//")), nullptr, 0));
                            tempY = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[1], "//")), nullptr, 0));
                            temp = util::upper(util::trim(util::strip(csecs[2], "//")));
                            if (temp != "ALL" && temp != "A") {
                                tempZ = static_cast<std::uint16_t>(std::stoul(temp, nullptr, 0));
                            }
                            toAdd.SourceLocation(tempX, tempY, tempZ);
                            
                            tempX = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[3], "//")), nullptr, 0));
                            tempY = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[4], "//")), nullptr, 0));
                            tempZ = static_cast<std::int8_t>(std::stoi(util::trim(util::strip(csecs[5], "//")), nullptr, 0));
                            toAdd.TargetLocation(tempX, tempY, tempZ);
                            
                            if (sectCount >= 7) {
                                toAdd.SourceWorld(static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[6], "//")), nullptr, 0)));
                                if (sectCount >= 8) {
                                    toAdd.TargetWorld(static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[7], "//")), nullptr, 0)));
                                }
                            }
                            worldMain.teleLocs.push_back(toAdd);
                        }
                        else {
                            Console::shared().error("Insufficient parameters for teleport entry");
                        }
                    }
                }
            }
        }
        std::sort(worldMain.teleLocs.begin(), worldMain.teleLocs.end(),[](CTeleLocationEntry &one, CTeleLocationEntry &two) {
            return static_cast<std::uint32_t>(one.SourceLocation().x) < static_cast<std::uint32_t>(two.SourceLocation().x);
        });
        delete teleportData;
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	loadCreatures()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads creatures from creature definition files
// o------------------------------------------------------------------------------------------------o
void loadCreatures() {
    std::string cEntry;
    std::string tag, data, UTag;
    std::uint16_t i = 0;
    for (Script *creatScp = FileLookup->FirstScript(creatures_def);!FileLookup->FinishedScripts(creatures_def);creatScp = FileLookup->NextScript(creatures_def)) {
        if (creatScp == nullptr)
            continue;
        
        for (CScriptSection *creatureData = creatScp->FirstEntry(); creatureData != nullptr;creatureData = creatScp->NextEntry()) {
            if (creatureData == nullptr)
                continue;
            
            cEntry = creatScp->EntryName();
            auto ssecs = oldstrutil::sections(cEntry, " ");
            if (ssecs[0] == "CREATURE") {
                i = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(ssecs[1], "//")), nullptr, 0));
                worldMain.creatures[i].CreatureId(i);
                
                for (tag = creatureData->First(); !creatureData->AtEnd();
                     tag = creatureData->Next()) {
                    if (tag.empty()) {
                        continue;
                    }
                    data = creatureData->GrabData();
                    data = util::trim(util::strip(data, "//"));
                    UTag = util::upper(tag);
                    switch ((UTag.data()[0])) {
                        case 'A':
                            if (UTag == "ANTIBLINK") {
                                worldMain.creatures[i].AntiBlink(true);
                            }
                            else if (UTag == "ANIMAL") {
                                worldMain.creatures[i].IsAnimal(true);
                            }
                            else if (UTag.rfind("ANIM_", 0) == 0) // Check if UTag starts with "ANIM_"
                            {
                                // The following bit is shared between all the "ANIM_" tags
                                std::uint8_t animId = 0;
                                std::uint8_t animLength = 0;
                                auto csecs = oldstrutil::sections(data, ",");
                                if (csecs.size() > 1) {
                                    animId = static_cast<std::uint8_t>(std::stoul(util::trim(util::strip(csecs[0], "//")), nullptr, 0));
                                    animLength = static_cast<std::uint8_t>(std::stoul(util::trim(util::strip(csecs[1], "//")), nullptr, 0));
                                }
                                else {
                                    animId = static_cast<std::uint8_t>(std::stoul(util::trim(util::strip(csecs[0], "//")), nullptr, 0));
                                    animLength = 7; // default to 7 frames if nothing is specified
                                }
                                
                                // Now apply the anim-data to the creature
                                if (UTag == "ANIM_ATTACK1") {
                                    worldMain.creatures[i].AttackAnim1(animId, animLength);
                                }
                                else if (UTag == "ANIM_ATTACK2") {
                                    worldMain.creatures[i].AttackAnim2(animId, animLength);
                                }
                                else if (UTag == "ANIM_ATTACK3") {
                                    worldMain.creatures[i].AttackAnim3(animId, animLength);
                                }
                                else if (UTag == "ANIM_CASTAREA") {
                                    worldMain.creatures[i].CastAnimArea(animId, animLength);
                                }
                                else if (UTag == "ANIM_CASTTARGET") {
                                    worldMain.creatures[i].CastAnimTarget(animId, animLength);
                                }
                                else if (UTag == "ANIM_CAST3") {
                                    worldMain.creatures[i].CastAnim3(animId, animLength);
                                }
                            }
                            break;
                        case 'B':
                            if (UTag == "BASESOUND") {
                                break;
                            }
                            break;
                        case 'F':
                            if (UTag == "FLIES") {
                                worldMain.creatures[i].CanFly(true);
                            }
                            break;
                        case 'H':
                            if (UTag == "HUMAN") {
                                worldMain.creatures[i].IsHuman(true);
                            }
                            break;
                        case 'I':
                            if (UTag == "ICON") {
                                worldMain.creatures[i].Icon(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
                            }
                            break;
                        case 'M':
                            if (UTag == "MOVEMENT") {
                                if (util::upper(data) == "WATER") {
                                    worldMain.creatures[i].IsWater(true);
                                }
                                else if (util::upper(data) == "BOTH") {
                                    worldMain.creatures[i].IsAmphibian(true);
                                }
                                else {
                                    worldMain.creatures[i].IsWater(false);
                                    worldMain.creatures[i].IsAmphibian(false);
                                }
                            }
                            else if (UTag == "MOUNTID") {
                                worldMain.creatures[i].MountId(static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
                            }
                            break;
                        case 'S':
                            if (UTag == "SOUNDFLAG") {
                                break;
                            }
                            else if (UTag == "SOUND_IDLE") {
                                worldMain.creatures[i].SetSound(SND_IDLE, static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
                            }
                            else if (UTag == "SOUND_STARTATTACK") {
                                worldMain.creatures[i].SetSound(SND_STARTATTACK, static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
                            }
                            else if (UTag == "SOUND_ATTACK") {
                                worldMain.creatures[i].SetSound(SND_ATTACK, static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
                            }
                            else if (UTag == "SOUND_DEFEND") {
                                worldMain.creatures[i].SetSound(SND_DEFEND, static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
                            }
                            else if (UTag == "SOUND_DIE") {
                                worldMain.creatures[i].SetSound(SND_DIE, static_cast<std::uint16_t>(std::stoul(data, nullptr, 0)));
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
    
    FileLookup->Dispose(creatures_def);
}

void ReadWorldTagData(std::istream &inStream, std::string &tag, std::string &data) {
    char temp[4097];
    tag = "o---o";
    data = "o---o";
    while (!inStream.eof() && !inStream.fail()) {
        inStream.getline(temp, 4096);
        temp[inStream.gcount()] = 0;
        auto sLine = std::string(temp);
        auto cloc = sLine.find("//");
        
        if (cloc != std::string::npos) {
            sLine = sLine.substr(0, cloc);
        }
        cloc = sLine.find_first_not_of(" \t\v\f\0");
        if (cloc != std::string::npos) {
            if (cloc != std::string::npos) {
                auto temp2 = sLine.find_last_not_of(" \t\v\f\0");
                sLine = sLine.substr(cloc, (temp2 - cloc) + 1);
            }
        }
        
        if (!sLine.empty()) {
            if (sLine != "o---o") {
                auto loc = sLine.find("=");
                if (loc != std::string::npos) {
                    tag = sLine.substr(0, loc);
                    auto temp = tag.find_first_not_of(" \t\v\f\0");
                    if (temp != std::string::npos) {
                        auto temp2 = tag.find_last_not_of(" \t\v\f\0");
                        tag = tag.substr(temp, (temp2 - temp) + 1);
                    }
                    
                    if ((loc + 1) < sLine.size()) {
                        data = sLine.substr(loc + 1);
                        auto temp = data.find_first_not_of(" \t\v\f\0");
                        if (temp != std::string::npos) {
                            auto temp2 = data.find_last_not_of(" \t\v\f\0");
                            data = data.substr(temp, (temp2 - temp) + 1);
                        }
                    }
                    else {
                        data = "";
                    }
                    break;
                }
            }
            else
                break;
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	loadPlaces()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load locations from location definition files
// o------------------------------------------------------------------------------------------------o
void loadPlaces() {
    worldMain.goPlaces.clear();
    std::string data, UTag, entryName;
    GoPlaces *toAdd = nullptr;
    
    for (Script *locScp = FileLookup->FirstScript(location_def); !FileLookup->FinishedScripts(location_def); locScp = FileLookup->NextScript(location_def)) {
        if (locScp == nullptr) {
            continue;
        }
        for (CScriptSection *toScan = locScp->FirstEntry(); toScan != nullptr; toScan = locScp->NextEntry()) {
            if (toScan == nullptr) {
                continue;
            }
            entryName = locScp->EntryName();
            auto ssecs = oldstrutil::sections(entryName, " ");
            
            size_t entryNum = static_cast<std::uint32_t>(std::stoul(util::trim(util::strip(ssecs[1], "//")), nullptr, 0));
            
            if ((util::upper(util::trim(util::strip(ssecs[0], "//"))) == "LOCATION") && entryNum) {
                if (worldMain.goPlaces.find(static_cast<std::uint16_t>(entryNum)) != worldMain.goPlaces.end()) {
                    Console::shared().warning(util::format("Doubled up entry in Location.dfn (%u)", entryNum));
                }
                toAdd = &worldMain.goPlaces[static_cast<std::uint16_t>(entryNum)];
                if (toAdd != nullptr) {
                    for (std::string tag = toScan->First(); !toScan->AtEnd();
                         tag = toScan->Next()) {
                        data = toScan->GrabData();
                        data = util::trim(util::strip(data, "//"));
                        UTag = util::upper(tag);
                        if (UTag == "X") {
                            toAdd->x = static_cast<std::int16_t>(std::stoi(data, nullptr, 0));
                        }
                        else if (UTag == "Y") {
                            toAdd->y = static_cast<std::int16_t>(std::stoi(data, nullptr, 0));
                        }
                        else if (UTag == "Z") {
                            toAdd->z = static_cast<std::int8_t>(std::stoi(data, nullptr, 0));
                        }
                        else if (UTag == "WORLD") {
                            toAdd->worldNum = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                        }
                        else if (UTag == "INSTANCEID") {
                            toAdd->instanceId = static_cast<std::uint16_t>(std::stoul(data, nullptr, 0));
                        }
                        else if (UTag == "LOCATION") {
                            auto csecs = oldstrutil::sections(data, ",");
                            size_t sectionCount = csecs.size() + 1;
                            if (sectionCount >= 3) {
                                toAdd->x = static_cast<std::int16_t>(std::stoi(util::trim(util::strip(csecs[0], "//")), nullptr, 0));
                                toAdd->y = static_cast<std::int16_t>(
                                                                     std::stoi(util::trim(util::strip(csecs[1], "//")), nullptr, 0));
                                toAdd->z = static_cast<std::int8_t>(std::stoi(util::trim(util::strip(csecs[2], "//")), nullptr, 0));
                                toAdd->worldNum = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[3], "//")), nullptr, 0));
                            }
                            
                            if (sectionCount == 4) {
                                toAdd->instanceId = static_cast<std::uint16_t>(std::stoul(util::trim(util::strip(csecs[4], "//")), nullptr, 0));
                            }
                        }
                    }
                }
            }
        }
    }
    
    FileLookup->Dispose(location_def);
}

