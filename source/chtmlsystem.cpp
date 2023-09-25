// HTML Template

#include "chtmlsystem.h"

#include "cchar.h"
#include "cguild.h"
#include "cgump.h"
#include "cpacketsend.h"
#include "craces.h"
#include "cserverdefinitions.h"
#include "csocket.h"
#include "funcdecl.h"
#include "objectfactory.h"
#include "ostype.h"
#include "osunique.hpp"
#include "scriptc.h"
#include "configuration/serverconfig.hpp"
#include "ssection.h"
#include "subsystem/console.hpp"
#include "townregion.h"
#include "utility/strutil.hpp"
#include "other/uoxversion.hpp"

extern cRaces worldRace ;
extern CGuildCollection worldGuildSystem ;
extern CServerDefinitions worldFileLookup ;
extern CNetworkStuff worldNetwork ;

using namespace std::string_literals;


cHTMLTemplate::cHTMLTemplate() : updateTimer(60), loaded(false), type(ETT_INVALIDTEMPLATE), scheduledUpdate(0) {
    name = "";
    content = "";
}

cHTMLTemplate::~cHTMLTemplate() {}

std::string GetUptime() {
    std::uint32_t total = (worldMain.GetUICurrentTime() - worldMain.GetStartTime()) / 1000;
    std::uint32_t ho = total / 3600;
    total -= ho * 3600;
    std::uint32_t mi = total / 60;
    total -= mi * 60;
    std::uint32_t se = total;
    total = 0;
    std::string builtString = "";
    if (ho < 10) {
        builtString += std::string("0");
    }
    builtString += util::ntos(ho) + ":";
    if (mi < 10) {
        builtString += std::string("0");
    }
    builtString += util::ntos(mi) + std::string(":");
    if (se < 10) {
        builtString += std::string("0");
    }
    builtString += util::ntos(se);
    return builtString;
}

bool CountNPCFunctor(CBaseObject *a, std::uint32_t &b, [[maybe_unused]] void *extraData) {
    bool retVal = true;
    if (ValidateObject(a)) {
        CChar *j = static_cast<CChar *>(a);
        if (j->IsNpc()) {
            ++b;
        }
    }
    return retVal;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::process()
//|	Date		-	1/18/2003 4:43:17 AM
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-
//|
//|	Changes		-	08062003 -  For the record this is some of the
//|									most fucked up shit I have EVER
//SEEN!! Written to truely
//|									handle multiple Templates.
// o------------------------------------------------------------------------------------------------o
void cHTMLTemplate::process() {
    // Need to check to see if the server is actually running, of so we do not want to process the
    // offline template.
    if (worldMain.GetKeepRun() && this->GetTemplateType() == ETT_OFFLINE)
        return;
    
    // Only read the Status Page if it's not already loaded
    if (!loaded) {
        LoadTemplate();
    }
    
    // Parse the Content...
    std::string ParsedContent = content;
    
    // Replacing Placeholders
    
    // Account-Count
    std::string AccountCount = util::ntos((Account::shared().size()));
    size_t Pos = ParsedContent.find("%accounts");
    while (Pos != std::string::npos) {
        ParsedContent.replace(Pos, 9, AccountCount);
        Pos = ParsedContent.find("%accounts");
    }
    
    // Version
    std::string Version = UOXVersion::version;
    Version += ".";
    Version += UOXVersion::build;
    Version += " [";
    Version += OS_STR;
    Version += "]";
    
    Pos = ParsedContent.find("%version");
    while (Pos != std::string::npos) {
        ParsedContent.replace(Pos, 8, Version);
        Pos = ParsedContent.find("%version");
    }
    // Character Count
    std::string CharacterCount = util::ntos(ObjectFactory::shared().CountOfObjects(CBaseObject::OT_CHAR));
    Pos = ParsedContent.find("%charcount");
    while (Pos != std::string::npos) {
        ParsedContent.replace(Pos, 10, CharacterCount);
        Pos = ParsedContent.find("%charcount");
    }
    
    // Item Count
    std::string ItemCount = util::ntos(ObjectFactory::shared().CountOfObjects(CBaseObject::OT_ITEM));
    Pos = ParsedContent.find("%itemcount");
    while (Pos != std::string::npos) {
        ParsedContent.replace(Pos, 10, ItemCount);
        Pos = ParsedContent.find("%itemcount");
    }
    
    // Connection Count (GMs, Counselors, Player)
    std::uint32_t gm = 0, cns = 0, ccount = 0;
    CChar *tChar = nullptr;
    
    // Get all Network Connections
    {
        for (auto &tSock : worldNetwork.connClients) {
            tChar = tSock->CurrcharObj();
            if (ValidateObject(tChar)) {
                if (tChar->IsGM()) {
                    ++gm;
                }
                else if (tChar->IsCounselor()) {
                    ++cns;
                }
                else {
                    ++ccount;
                }
            }
        }
    }
    
    // GMs
    std::string GMCount = util::ntos(gm);
    Pos = ParsedContent.find("%online_gms");
    while (Pos != std::string::npos) {
        (worldMain.GetKeepRun()) ? ParsedContent.replace(Pos, 11, GMCount) : ParsedContent.replace(Pos, 11, "0");
        Pos = ParsedContent.find("%online_gms");
    }
    
    // Counselor
    std::string CounsiCount = util::ntos(cns);
    Pos = ParsedContent.find("%online_couns");
    while (Pos != std::string::npos) {
        (worldMain.GetKeepRun()) ? ParsedContent.replace(Pos, 13, CounsiCount) : ParsedContent.replace(Pos, 13, "0");
        Pos = ParsedContent.find("%online_couns");
    }
    
    // Player
    std::string PlayerCount = util::ntos(ccount);
    Pos = ParsedContent.find("%online_player");
    while (Pos != std::string::npos) {
        (worldMain.GetKeepRun()) ? ParsedContent.replace(Pos, 14, PlayerCount) : ParsedContent.replace(Pos, 14, "0");
        Pos = ParsedContent.find("%online_player");
    }
    
    // Total
    std::string AllCount = util::ntos((ccount + gm + cns));
    Pos = ParsedContent.find("%online_all");
    while (Pos != std::string::npos) {
        (worldMain.GetKeepRun()) ? ParsedContent.replace(Pos, 11, AllCount) : ParsedContent.replace(Pos, 11, "0");
        Pos = ParsedContent.find("%online_all");
    }
    
    // Time
    char time_str[256];
    RealTime(time_str);
    Pos = ParsedContent.find("%time");
    while (Pos != std::string::npos) {
        (worldMain.GetKeepRun()) ? ParsedContent.replace(Pos, 5, time_str) : ParsedContent.replace(Pos, 5, "Down");
        Pos = ParsedContent.find("%time");
    }
    
    // 24Time
    RealTime24(time_str);
    Pos = ParsedContent.find("%24time");
    while (Pos != std::string::npos) {
        (worldMain.GetKeepRun()) ? ParsedContent.replace(Pos, 7, time_str) : ParsedContent.replace(Pos, 7, "Down");
        Pos = ParsedContent.find("%24time");
    }
    
    // Timestamp
    time_t currTime;
    time(&currTime);
    struct tm dtime;
    currTime = mktime(mgmtime(&dtime, &currTime));
    std::string timestamp = util::ntos(currTime);
    Pos = ParsedContent.find("%tstamp");
    while (Pos != std::string::npos) {
        (worldMain.GetKeepRun()) ? ParsedContent.replace(Pos, 7, timestamp) : ParsedContent.replace(Pos, 7, "Down");
        Pos = ParsedContent.find("%tstamp");
    }
    
    // Server Name
    auto serverName = ServerConfig::shared().serverString[ServerString::SERVERNAME];
    Pos = ParsedContent.find("%servername");
    while (Pos != std::string::npos) {
        (worldMain.GetKeepRun()) ? ParsedContent.replace(Pos, 11, serverName) : ParsedContent.replace(Pos, 11, "Unnamed UOX3 Server");
        Pos = ParsedContent.find("%servername");
    }
    
    // External/WAN ServerIP
    auto serverIP = ServerConfig::shared().serverString[ServerString::PUBLICIP];
    Pos = ParsedContent.find("%serverip");
    while (Pos != std::string::npos) {
        (worldMain.GetKeepRun()) ? ParsedContent.replace(Pos, 9, serverIP) : ParsedContent.replace(Pos, 9, "127.0.0.1");
        Pos = ParsedContent.find("%serverip");
    }
    
    // Server Port
    auto serverPort = ServerConfig::shared().ushortValues[UShortValue::PORT];
    Pos = ParsedContent.find("%serverport");
    while (Pos != std::string::npos) {
        (worldMain.GetKeepRun()) ? ParsedContent.replace(Pos, 11, util::ntos(serverPort)) : ParsedContent.replace(Pos, 11, "127.0.0.1");
        Pos = ParsedContent.find("%serverport");
    }
    
    // Playerlist
    Pos = ParsedContent.find("%playerlist%");
    while (Pos != std::string::npos) {
        size_t SecondPos = ParsedContent.find("%playerlist%", Pos + 1);
        if (SecondPos == std::string::npos) // there's no closing part!
            break;
        std::string myInline = ParsedContent.substr(Pos, SecondPos - Pos + 12);
        std::string PlayerList;
        {
            for (auto &tSock : worldNetwork.connClients) {
                try {
                    if (tSock) {
                        auto tChar = tSock->CurrcharObj();
                        if (ValidateObject(tChar)) {
                            // Don't show names of any online GMs and counselors on normal status
                            // page
                            if (this->GetTemplateType() == ETT_ONLINE &&
                                (tChar->IsGM() || tChar->IsCounselor()))
                                continue;
                            
                            std::string parsedInline = myInline;
                            parsedInline.replace(0, 12, "");
                            parsedInline.replace(parsedInline.length() - 12, 12, "");
                            
                            //					Tokens for the PlayerList
                            //					%playername
                            //					%playertitle
                            //					%playerip
                            //					%playeraccount
                            //					%playerx
                            //					%playery
                            //					%playerz
                            //					%playerrace
                            //					%playerregion
                            
                            // PlayerName
                            size_t sPos = parsedInline.find("%playername");
                            while (sPos != std::string::npos) {
                                (worldMain.GetKeepRun()) ? parsedInline.replace(sPos, 11, tChar->GetName()) : parsedInline.replace(sPos, 11, "");
                                sPos = parsedInline.find("%playername");
                            }
                            
                            // PlayerTitle
                            auto playerTitle = tChar->GetTitle();
                            if (!playerTitle.empty()) {
                                playerTitle = "(" + playerTitle + ")";
                            }
                            sPos = parsedInline.find("%playertitle");
                            while (sPos != std::string::npos) {
                                (worldMain.GetKeepRun()) ? parsedInline.replace(sPos, 12, tChar->GetTitle()) : parsedInline.replace(sPos, 12, "");
                                sPos = parsedInline.find("%playertitle");
                            }
                            
                            // PlayerIP
                            sPos = parsedInline.find("%playerip");
                            while (sPos != std::string::npos) {
                                CSocket *mySock = tChar->GetSocket();
                                
                                auto ClientIP = util::format("%s", mySock->clientIP.c_str());
                                (worldMain.GetKeepRun()) ? parsedInline.replace(sPos, 9, ClientIP) : parsedInline.replace(sPos, 9, "");
                                sPos = parsedInline.find("%playerip");
                            }
                            
                            // PlayerAccount
                            sPos = parsedInline.find("%playeraccount");
                            while (sPos != std::string::npos) {
                                AccountEntry &toScan = tChar->GetAccount();
                                if (toScan.accountNumber != AccountEntry::INVALID_ACCOUNT) {
                                    (worldMain.GetKeepRun()) ? parsedInline.replace(sPos, 14, toScan.username) : parsedInline.replace(sPos, 14, "");
                                }
                                sPos = parsedInline.find("%playeraccount");
                            }
                            
                            // PlayerX
                            sPos = parsedInline.find("%playerx");
                            while (sPos != std::string::npos) {
                                std::string myX = util::ntos(tChar->GetX());
                                (worldMain.GetKeepRun()) ? parsedInline.replace(sPos, 8, myX) : parsedInline.replace(sPos, 8, "");
                                sPos = parsedInline.find("%playerx");
                            }
                            
                            // PlayerY
                            sPos = parsedInline.find("%playery");
                            while (sPos != std::string::npos) {
                                std::string myY = util::ntos(tChar->GetY());
                                (worldMain.GetKeepRun()) ? parsedInline.replace(sPos, 8, myY) : parsedInline.replace(sPos, 8, "");
                                sPos = parsedInline.find("%playery");
                            }
                            
                            // PlayerZ
                            sPos = parsedInline.find("%playerz");
                            while (sPos != std::string::npos) {
                                std::string myZ = util::ntos(tChar->GetZ());
                                (worldMain.GetKeepRun()) ? parsedInline.replace(sPos, 8, myZ) : parsedInline.replace(sPos, 8, "");
                                sPos = parsedInline.find("%playerz");
                            }
                            
                            // PlayerRace -- needs testing
                            sPos = parsedInline.find("%playerrace");
                            while (sPos != std::string::npos) {
                                raceid_t myRace = tChar->GetRace();
                                const std::string rName = worldRace.Name(myRace);
                                size_t raceLenName = rName.length();
                                
                                if (raceLenName > 0) {
                                    (worldMain.GetKeepRun()) ? parsedInline.replace(sPos, 11, rName) : parsedInline.replace(sPos, 11, "");
                                }
                                sPos = parsedInline.find("%playerrace");
                            }
                            
                            // PlayerRegion
                            sPos = parsedInline.find("%playerregion");
                            while (sPos != std::string::npos) {
                                (worldMain.GetKeepRun()) ? parsedInline.replace(sPos, 13, tChar->GetRegion()->GetName()) : parsedInline.replace(sPos, 13, "");
                                sPos = parsedInline.find("%playerregion");
                            }
                            
                            PlayerList += parsedInline;
                        }
                    }
                }
                catch (...) {
                    Console::shared() << "| EXCEPTION: Invalid character/socket pointer found. Ignored." << myendl;
                }
            }
        }
        
        (worldMain.GetKeepRun()) ? ParsedContent.replace(Pos, myInline.length(), PlayerList) : ParsedContent.replace(Pos, myInline.length(), "");
        Pos = ParsedContent.find("%playerlist%");
    }
    
    // GuildCount
    std::string GuildCount = util::ntos(static_cast<std::int32_t>(worldGuildSystem.NumGuilds()));
    Pos = ParsedContent.find("%guildcount");
    while (Pos != std::string::npos) {
        (worldMain.GetKeepRun()) ? ParsedContent.replace(Pos, 11, GuildCount) : ParsedContent.replace(Pos, 11, "");
        Pos = ParsedContent.find("%guildcount");
    }
    
    // GUILDLIST
    Pos = ParsedContent.find("%guildlist%");
    while (Pos != std::string::npos) {
        size_t SecondPos = ParsedContent.find("%guildlist%", Pos + 1);
        if (SecondPos == std::string::npos) // can't find closing
            break;
        std::string myInline = ParsedContent.substr(Pos, SecondPos - Pos + 11);
        std::string GuildList;
        
        for (std::int16_t i = 0; i < static_cast<std::int16_t>(worldGuildSystem.NumGuilds()); ++i) {
            std::string parsedInline = myInline;
            parsedInline.replace(0, 11, "");
            parsedInline.replace(parsedInline.length() - 11, 11, "");
            
            //			Tokens for the GuildList
            //			%guildid
            //			%guildname
            //			%guildmembercount
            
            // GuildId
            size_t sPos;
            CGuild *myGuild = worldGuildSystem.Guild(i);
            
            std::string GuildId = util::ntos(i);
            sPos = parsedInline.find("%guildid");
            while (sPos != std::string::npos) {
                (worldMain.GetKeepRun()) ? parsedInline.replace(sPos, 8, GuildId) : parsedInline.replace(sPos, 8, "");
                sPos = parsedInline.find("%guildid");
            }
            
            // GuildName
            sPos = parsedInline.find("%guildname");
            while (sPos != std::string::npos) {
                (worldMain.GetKeepRun()) ? parsedInline.replace(sPos, 10, myGuild->Name()) : parsedInline.replace(sPos, 10, "");
                sPos = parsedInline.find("%guildname");
            }
            
            // Guild Member Count
            sPos = parsedInline.find("%guildmembercount");
            while (sPos != std::string::npos) {
                (worldMain.GetKeepRun()) ? parsedInline.replace(sPos, 17, util::ntos(myGuild->NumMembers())) : parsedInline.replace(sPos, 10, "");
                sPos = parsedInline.find("%guildmembercount");
            }
            
            GuildList += parsedInline;
        }
        
        (worldMain.GetKeepRun()) ? ParsedContent.replace(Pos, myInline.length(), GuildList)
        : ParsedContent.replace(Pos, myInline.length(), "");
        Pos = ParsedContent.find("%guildlist%");
    }
    
    // NPCCount
    std::uint32_t npccount = 0;
    std::uint32_t b = 0;
    ObjectFactory::shared().IterateOver(CBaseObject::OT_CHAR, b, nullptr, &CountNPCFunctor);
    npccount = b;
    
    std::string npcs = util::ntos(npccount);
    Pos = ParsedContent.find("%npcs");
    while (Pos != std::string::npos) {
        (worldMain.GetKeepRun()) ? ParsedContent.replace(Pos, 5, npcs)
        : ParsedContent.replace(Pos, 5, "0");
        Pos = ParsedContent.find("%npcs");
    }
    
    // Performance Dump
    double eps = 0.00000000001;
    Pos = ParsedContent.find("%performance");
    while (Pos != std::string::npos) {
        std::string performance;
        std::ostringstream myStream(performance);
        if (worldMain.GetKeepRun()) {
            std::uint32_t networkTimeCount = worldMain.ServerProfile()->NetworkTimeCount();
            std::uint32_t timerTimeCount = worldMain.ServerProfile()->TimerTimeCount();
            std::uint32_t autoTimeCount = worldMain.ServerProfile()->AutoTimeCount();
            std::uint32_t loopTimeCount = worldMain.ServerProfile()->LoopTimeCount();
            myStream << "Network code: "  << static_cast<float>( static_cast<float>(worldMain.ServerProfile()->NetworkTime()) / static_cast<float>(networkTimeCount)) << "msec [" << networkTimeCount << " samples] <BR>";
            myStream << "Timer code: " << static_cast<float>( static_cast<float>(worldMain.ServerProfile()->TimerTime()) / static_cast<float>(timerTimeCount)) << "msec [" << timerTimeCount << " samples] <BR>";
            myStream << "Auto code: " << static_cast<float>( static_cast<float>(worldMain.ServerProfile()->AutoTime()) / static_cast<float>(autoTimeCount)) << "msec [" << autoTimeCount << " samples] <BR>";
            myStream << "Loop Time: " << static_cast<float>( static_cast<float>(worldMain.ServerProfile()->LoopTime()) / static_cast<float>(loopTimeCount)) << "msec [" << loopTimeCount << " samples] <BR>";
            if (!(worldMain.ServerProfile()->LoopTime() < eps || loopTimeCount < eps)) {
                myStream << "Simulation Cycles: " << (1000.0 * (1.0 / static_cast<float>( static_cast<float>( worldMain.ServerProfile()->LoopTime()) / static_cast<float>(loopTimeCount)))) << " per sec <BR>";
            }
            else {
                myStream << "Simulation Cycles: Greater than 10000 <BR> ";
            }
        }
        else {
            myStream << "Network code: 0"
            << "<BR>";
            myStream << "Timer code: 0"
            << "<BR>";
            myStream << "Auto code: 0"
            << "<BR>";
            myStream << "Loop Time: 0"
            << "<BR>";
            myStream << "Simulation Cycles: 0<BR>";
        }
        ParsedContent.replace(Pos, 12, myStream.str());
        Pos = ParsedContent.find("%performance");
    }
    
    // Uptime
    Pos = ParsedContent.find("%uptime");
    while (Pos != std::string::npos) {
        std::string builtString = GetUptime();
        ParsedContent.replace(Pos, 7, builtString);
        Pos = ParsedContent.find("%uptime");
    }
    
    // Simulation Cycles (whatever that may be...)
    Pos = ParsedContent.find("%simcycles");
    while (Pos != std::string::npos) {
        std::string simcycles;
        std::ostringstream myStream(simcycles);
        
        if (worldMain.GetKeepRun()) {
            if (!(worldMain.ServerProfile()->LoopTime() < eps || worldMain.ServerProfile()->LoopTimeCount() < eps)) {
                myStream << "Simulation Cycles: " << (1000.0 *
                                                      (1.0 / static_cast<float>( static_cast<float>(worldMain.ServerProfile()->LoopTime()) / static_cast<float>( worldMain.ServerProfile()->LoopTimeCount())))) << " per sec <BR>";
            }
            else {
                myStream << "Simulation Cycles: Greater than 10000 <BR> ";
            }
        }
        else {
            myStream << "Simulation Cycles: 0<BR> ";
        }
        ParsedContent.replace(Pos, 10, myStream.str());
        Pos = ParsedContent.find("%simcycles");
    }
    
    // Update Time in SECONDS
    Pos = ParsedContent.find("%updatetime");
    while (Pos != std::string::npos) {
        std::string strUpdateTimer = util::ntos(updateTimer);
        (worldMain.GetKeepRun()) ? ParsedContent.replace(Pos, 11, strUpdateTimer)
        : ParsedContent.replace(Pos, 11, "0");
        Pos = ParsedContent.find("%updatetime");
    }
    
    // End Replacing Placeholders
    
    // Print the Content out to the new file...
    std::ofstream Output;
    Output.open(outputFile.string(), std::ios::out);
    if (Output.is_open()) {
        Output << ParsedContent;
        Output.close();
    }
    else {
        Console::shared().error(util::format(" Couldn't open the template file %s for writing", outputFile.string().c_str()));
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::poll()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates the page if needed
// o------------------------------------------------------------------------------------------------o
void cHTMLTemplate::poll() {
    if (scheduledUpdate < worldMain.GetUICurrentTime() || !worldMain.GetKeepRun()) {
        process();
        scheduledUpdate = BuildTimeValue(static_cast<float>(updateTimer));
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::LoadTemplate()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the Template into memory
//|
//|	Changes		-	08062003 - Updated this member function to actually handle
//|									loading the different templates for
//use later.
// o------------------------------------------------------------------------------------------------o
void cHTMLTemplate::LoadTemplate() {
    content = "";
    
    std::ifstream InputFile1(inputFile.string());
    
    if (!InputFile1.is_open()) {
        Console::shared().error(util::format("Couldn't open HTML Template File %s", inputFile.string().c_str()));
        return;
    }
    
    while (!InputFile1.eof() && !InputFile1.fail()) {
        std::string Line;
        std::getline(InputFile1, Line);
        content += Line;
    }
    
    InputFile1.close();
    
    loaded = true;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::UnloadTemplate()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Unloads the Template (i.e. for reloading)
//|
//|	Changes		-	08062003 - Updated to properly unload a template
//|								and to unload the correect template, instead
//of just the |								status template.
// o------------------------------------------------------------------------------------------------o
void cHTMLTemplate::UnloadTemplate() {
    content = "";
    loaded = false;
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::load( CScriptSection *found )
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the HTML Template from a CScriptSection
// o------------------------------------------------------------------------------------------------o
void cHTMLTemplate::load(CScriptSection *found) {
    for (const auto &sec : found->collection()) {
        auto tag = sec->tag;
        auto data = sec->data;
        auto UTag = util::upper(tag);
        
        if (UTag == "UPDATE") {
            updateTimer = static_cast<std::uint32_t>(std::stoul(data, nullptr, 0));
        }
        else if (UTag == "TYPE") {
            auto UData = util::upper(data);
            if (UData == "STATUS") {
                type = ETT_ONLINE;
            }
            else if (UData == "OFFLINE") {
                type = ETT_OFFLINE;
            }
            else if (UData == "PLAYER") {
                type = ETT_PLAYER;
            }
            else if (UData == "GUILD") {
                type = ETT_GUILD;
            }
            else if (UData == "GMSTATUS") {
                type = ETT_GMSTATUS;
            }
        }
        else if (UTag == "INPUT") {
            inputFile = ServerConfig::shared().directoryFor(dirlocation_t::DEFINITION) / std::filesystem::path("html") / std::filesystem::path(data);
        }
        else if (UTag == "OUTPUT") {
            outputFile = ServerConfig::shared().directoryFor(dirlocation_t::HTML) / std::filesystem::path(data);
        }
        else if (UTag == "NAME") {
            name = data;
        }
    }
    
    scheduledUpdate = 0;
    loaded = false;
}

cHTMLTemplates::~cHTMLTemplates() { Unload(); }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplates::load()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the HTML Templates from the scripts
// o------------------------------------------------------------------------------------------------o
auto cHTMLTemplates::load() -> void {
    for (auto &toCheck : worldFileLookup.ScriptListings[html_def]) {
        if (toCheck) {
            size_t NumEntries = toCheck->NumEntries();
            if (NumEntries != 0) {
                for (const auto &[entryName, found] : toCheck->collection()) {
                    cHTMLTemplate *Template = new cHTMLTemplate();
                    Template->load(found.get());
                    Templates.push_back(Template);
                }
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplates::Unload()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Unloads all Templates
// o------------------------------------------------------------------------------------------------o
void cHTMLTemplates::Unload() {
    if (Templates.empty())
        return;
    
    for (size_t i = 0; i < Templates.size(); ++i) {
        delete Templates[i];
        Templates[i] = nullptr;
    }
    Templates.clear();
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplates::poll()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Polls the templates for updates
// o------------------------------------------------------------------------------------------------o
void cHTMLTemplates::poll(ETemplateType nTemplateId) {
    std::vector<cHTMLTemplate *>::const_iterator tIter;
    for (tIter = Templates.begin(); tIter != Templates.end(); ++tIter) {
        cHTMLTemplate *toPoll = (*tIter);
        if (toPoll != nullptr) {
            if (nTemplateId == -1 || toPoll->GetTemplateType() == nTemplateId) {
                toPoll->poll();
            }
        }
    }
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplates::TemplateInfoGump()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Shows an information gump about current templates
// o------------------------------------------------------------------------------------------------o
void cHTMLTemplates::TemplateInfoGump(CSocket *mySocket) {
    CGump InfoGump = CGump(false, false);
    
    InfoGump.SetSerial(1); // Serial for the HTML Status Callback
    
    InfoGump.StartPage();
    // Add an exit button
    InfoGump.AddButton(40, 300, ServerConfig::shared().ushortValues[UShortValue::BUTTONCANCEL] ,  static_cast<std::uint16_t>(ServerConfig::shared().ushortValues[UShortValue::BUTTONCANCEL] + 1), 1, 0, 0);
    
    InfoGump.AddBackground(0, 0, ServerConfig::shared().ushortValues[UShortValue::BACKGROUNDPIC], 350, 350);
    
    // 10 Max Templates per page
    std::uint32_t Entries = 0;     // Entries per page
    std::uint16_t CurrentPage = 0; // Page
    
    for (std::uint32_t i = 0; i < Templates.size(); ++i) {
        if (Entries == 0) {
            // We should add a next button if we're not starting the first page
            if (CurrentPage != 0) {
                InfoGump.AddButton(300, 250, ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT],  static_cast<std::uint16_t>(ServerConfig::shared().ushortValues[UShortValue::BUTTONRIGHT] + 1), 0, static_cast<std::uint16_t>(CurrentPage + 1), 0);
            }
            
            CurrentPage = InfoGump.StartPage();
        }
        
        // If we're not on the first page add the "back" button
        if (CurrentPage > 1) {
            InfoGump.AddButton(30, 250, ServerConfig::shared().ushortValues[UShortValue::BUTTONLEFT], static_cast<std::uint16_t>(ServerConfig::shared().ushortValues[UShortValue::BUTTONLEFT] + 1), 0, static_cast<std::uint16_t>(CurrentPage - 1), 0);
        }
        
        ++Entries;
        
        // ~25 pixel per entry
        
        InfoGump.AddText(40, static_cast<std::uint16_t>(40 + (Entries - 1) * 25), ServerConfig::shared().ushortValues[UShortValue::LEFTTEXTCOLOR], util::format("%s (%i)", Templates[i]->GetName().c_str(), i));
        
        if (Entries == 5) {
            Entries = 0;
        }
    }
    
    InfoGump.Send(mySocket);
}

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::GetName()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the name of the Template
// o------------------------------------------------------------------------------------------------o
std::string cHTMLTemplate::GetName() const { return name; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::GetOutput()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the Output Filename
// o------------------------------------------------------------------------------------------------o
auto cHTMLTemplate::GetOutput() const -> std::filesystem::path { return outputFile; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::GetInput()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the Input Filename
// o------------------------------------------------------------------------------------------------o
auto cHTMLTemplate::GetInput() const ->std::filesystem::path { return inputFile; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::GetScheduledUpdate()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the next scheduled Update time
// o------------------------------------------------------------------------------------------------o
std::uint32_t cHTMLTemplate::GetScheduledUpdate() const { return scheduledUpdate; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::GetUpdateTimer()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the Update timer
// o------------------------------------------------------------------------------------------------o
std::uint32_t cHTMLTemplate::GetUpdateTimer() const { return updateTimer; }

// o------------------------------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::GetTemplateType()
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the Template Type
// o------------------------------------------------------------------------------------------------o
ETemplateType cHTMLTemplate::GetTemplateType() const { return type; }
