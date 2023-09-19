//Copyright © 2023 Charles Kerr. All rights reserved.

#include "serverconfig.hpp"

#include <fstream>
#include <stdexcept>
#include <vector>

#include "utility/strutil.hpp"

using namespace std::string_literals ;



//======================================================================
const std::vector<StartLocation> ServerConfig::fallbackStartLocation {
    StartLocation("Yew,Center,545,982,0,0,0,1075072"),
    StartLocation("Minoc,Tavern,2477,411,15,0,0,1075073"),
    StartLocation("Britain,Sweet Dreams Inn,1495,1629,10,0,0,1075074"),
    StartLocation("Moonglow,Docks,4406,1045,0,0,0,1075075"),
    StartLocation("Trinsic,West Gate,1832,2779,0,0,0,1075076"),
    StartLocation("Magincia,Docks,3675,2259,26,0,0,1075077"),
    StartLocation("Jhelom,Docks,1492,3696,0,0,0,1075078"),
    StartLocation("Skara Brae,Docks,639,2236,0,0,0,1075079"),
    StartLocation("Vesper,Ironwood Inn,2771,977,0,0,0,1075080"),
};
//======================================================================
const std::vector<StartLocation> ServerConfig::fallbackYoungLocation {
    StartLocation("Yew,Center,545,982,0,0,0,1075072"),
    StartLocation("Minoc,Tavern,2477,411,15,0,0,1075073"),
    StartLocation("Britain,Sweet Dreams Inn,1495,1629,10,0,0,1075074"),
    StartLocation("Moonglow,Docks,4406,1045,0,0,0,1075075"),
    StartLocation("Trinsic,West Gate,1832,2779,0,0,0,1075076"),
    StartLocation("Magincia,Docks,3675,2259,26,0,0,1075077"),
    StartLocation("Jhelom,Docks,1492,3696,0,0,0,1075078"),
    StartLocation("Skara Brae,Docks,639,2236,0,0,0,1075079"),
    StartLocation("Vesper,Ironwood Inn,2771,977,0,0,0,1075080"),
};

//======================================================================
ServerConfig::ServerConfig(){
    startLocation = StartLocConfig("LOCATION");
    youngLocation = StartLocConfig("YOUNGLOCATION");
    reset();
}
//======================================================================
auto ServerConfig::loadKeyValue(const std::string &lkey, const std::string &value)->bool {
    auto key = util::upper(lkey) ;
    auto rvalue = true ;
    // We should order this by most likely, but for another day
    if (!directoryLocation.setLocation(key, value)){
        if (!enableClients.setKeyValue(key,value)){
            if (!clientFeature.setKeyValue(key,value)){
                if (!serverFeature.setKeyValue(key,value)){
                    if (!assistantFeature.setKeyValue(key,value)){
                        if (!startLocation.setKeyValue(key,value)){
                            if (!youngLocation.setKeyValue(key,value)){
                                if (!serverSwitch.setKeyValue(key,value)){
                                    if (!serverString.setKeyValue(key,value)){
                                        if (!ruleSets.setKeyValue(key,value)){
                                            if (!timerSetting.setKeyValue(key,value)){
                                                if (!ushortValues.setKeyValue(key,value)){
                                                    if (!realNumbers.setKeyValue(key,value)){
                                                        if (!uintValues.setKeyValue(key,value)){
                                                            if (!shortValues.setKeyValue(key,value)){
                                                                if (!spawnFacet.setKeyValue(key,value)){
                                                                    std::clog <<"Unhanded key/value: "<<key<<" = " << value << std::endl;
                                                                    rvalue = false ;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return rvalue ;
}
//======================================================================
auto ServerConfig::postCheck() ->void {
    directoryLocation.postLoad() ;
    if (youngLocation.empty()){
        youngLocation.startLocation = fallbackYoungLocation ;
    }
    if (startLocation.empty()){
        startLocation.startLocation = fallbackStartLocation ;
    }
    // Now, we need to see if the animation packet and the client version is mismatched
    if (serverSwitch[ServerSwitch::FORECENEWANIMATIONPACKET] && (enableClients.enableClient4000() || enableClients.enableClient5000() || enableClients.enableClient6000() || enableClients.enableClient6050())){
        serverSwitch.setSetting(ServerSwitch::FORECENEWANIMATIONPACKET,false);
        std::clog<<"FORCENEWANIMATIONPACKET setting not compatible with support for client versions below 7.0.0.0. Setting disabled!"<<std::endl;
    }
}

//======================================================================
auto ServerConfig::shared() ->ServerConfig&{
    static ServerConfig instance ;
    return instance ;
}
//======================================================================
auto ServerConfig::reset() -> void {
    
    assistantFeature = AssistantFeature() ;
    clientFeature = ClientFeature() ;
    directoryLocation.reset();
    enableClients.reset() ;
    realNumbers.reset();
    ruleSets.reset();
    serverFeature = ServerFeature() ;
    serverString.reset();
    serverSwitch.reset();
    shortValues.reset();
    spawnFacet.reset();
    startLocation.reset();
    timerSetting.reset();
    ushortValues.reset();
    uintValues.reset();
    youngLocation.reset();
}
//======================================================================
auto ServerConfig::loadConfig(const std::filesystem::path &config) ->void {
    auto input = std::ifstream(config.string());
    if (!input.is_open()){
        throw std::runtime_error("Unable to open UOX3 configuration file: "s + config.string());
    }
    this->reset();
    configFile = config ;
    auto buffer = std::vector<char>(4096,0) ;
    while(input.good() && !input.eof()){
        input.getline(buffer.data(),buffer.size()-1);
        if (input.gcount()>0){
            buffer[input.gcount()] = 0 ;
            std::string line = buffer.data() ;
            line = util::trim(util::strip(line,"//"));
            if (!line.empty()){
                if (line[0]!='[' && line[0]!='{' && line[0]!='}'){
                    auto [lkey,value] = util::split(line,"=");
                    auto key = util::upper(lkey) ;
                    loadKeyValue(key, value);
                }
            }
        }
    }
    postCheck();
}
//======================================================================
auto ServerConfig::writeConfig(const std::filesystem::path &config) const ->bool {
    auto myconfig = this->configFile ;
    if (!config.empty()){
        myconfig = config;
    }
    auto output = std::ofstream(myconfig.string()) ;
    if (!output.is_open()){
        //throw std::runtime_error("Unable to open: "s+myconfig.string());
        return false ;
    }
    // Now if we stored by "type", we could automate this, but we dont
    output << "//========================================================================\n" ;
    output << "// UOX3 Initialization File. V" ;
    output << (static_cast<std::uint16_t>(1) << 8 | static_cast<std::uint16_t>(2)) <<"\n" ;
    output << "//========================================================================\n\n" ;
    output << "[system]\n{\n" ;
    output << "\t" << ServerString::nameFor(ServerString::SERVERNAME) << " = " << serverString[ServerString::SERVERNAME] << "\n" ;
    output << "\t" << ServerString::nameFor(ServerString::PUBLICIP) << " = " << serverString[ServerString::PUBLICIP] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::PORT) << " = " << ushortValues[UShortValue::PORT] << "\n" ;
    output << "\t" << ServerString::nameFor(ServerString::SHARDKEY) << " = " << serverString[ServerString::SHARDKEY] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::LANGUAGE) << " = " << ushortValues[UShortValue::LANGUAGE] << "\n" ;
    output << "\t" << UIntValue::nameFor(UIntValue::NETRCVTIMEOUT) << " = " << uintValues[UIntValue::NETRCVTIMEOUT] << "\n" ;
    output << "\t" << UIntValue::nameFor(UIntValue::NETSNDTIMEOUT) << " = " << uintValues[UIntValue::NETSNDTIMEOUT] << "\n" ;
    output << "\t" << UIntValue::nameFor(UIntValue::NETRETRYCOUNT) << " = " << uintValues[UIntValue::NETRETRYCOUNT] << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::CONSOLELOG) << " = " << enabled(ServerSwitch::CONSOLELOG) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::NETWORKLOG) << " = " << enabled(ServerSwitch::NETWORKLOG) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::SPEECHLOG) << " = " << enabled(ServerSwitch::SPEECHLOG) << "\n" ;
    output << "\t" << ServerString::nameFor(ServerString::COMMANDPREFIX) << " = " << serverString[ServerString::COMMANDPREFIX] << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::ANNOUNCESAVE) << " = " << enabled(ServerSwitch::ANNOUNCESAVE) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::ANNOUNCEJOINPART) << " = " << enabled(ServerSwitch::ANNOUNCEJOINPART) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::BACKUP) << " = " << enabled(ServerSwitch::BACKUP) << "\n" ;
    output << "\t" << ShortValue::nameFor(ShortValue::SAVERATIO) << " = " << shortValues[ShortValue::SAVERATIO] << "\n" ;
    output << "\t" << UIntValue::nameFor(UIntValue::SAVESTIMER) << " = " << uintValues[UIntValue::SAVESTIMER] << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::UOG) << " = " << enabled(ServerSwitch::UOG) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::FREESHARD) << " = " << enabled(ServerSwitch::FREESHARD) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::RANDOMSTART) << " = " << enabled(ServerSwitch::RANDOMSTART) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::ASSISTANTNEGOTIATION) << " = " << enabled(ServerSwitch::ASSISTANTNEGOTIATION) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::KICKONASSISTANTSILENCE) << " = " << enabled(ServerSwitch::KICKONASSISTANTSILENCE) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::CUOMAPTRACKER) << " = " << enabled(ServerSwitch::CUOMAPTRACKER) << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::JSENGINE) << " = " << ushortValues[UShortValue::JSENGINE] << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::UNICODEMESSAGE) << " = " << enabled(ServerSwitch::UNICODEMESSAGE) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::CONTEXTMENU) << " = " << enabled(ServerSwitch::CONTEXTMENU) << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::SYSMESSAGECOLOR) << " = " << ushortValues[UShortValue::SYSMESSAGECOLOR] << "\n" ;
    output << "\t" << UIntValue::nameFor(UIntValue::MAXCLIENTBYTESIN) << " = " << uintValues[UIntValue::MAXCLIENTBYTESIN] << "\n" ;
    output << "\t" << UIntValue::nameFor(UIntValue::MAXCLIENTBYTESOUT) << " = " << uintValues[UIntValue::MAXCLIENTBYTESOUT] << "\n" ;
    output << "\t" << UIntValue::nameFor(UIntValue::NETTRAFFICTIMEBAN) << " = " << uintValues[UIntValue::NETTRAFFICTIMEBAN] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::APSPERFTHRESHOLD) << " = " << ushortValues[UShortValue::APSPERFTHRESHOLD] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::APSINTERVAL) << " = " << ushortValues[UShortValue::APSINTERVAL] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::APSDELAYSTEP) << " = " << ushortValues[UShortValue::APSDELAYSTEP] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::APSDELAYMAX) << " = " << ushortValues[UShortValue::APSDELAYMAX] << "\n" ;

    output << "}\n\n" ;

    output <<"[clientsupport]\n{\n";
    enableClients.save(output) ;
    output << "}\n\n" ;
    output << "//=================================================================================================\n" ;
    output << "// It is recommened to not define the DATADIRECTORY if one of the following:\n";
    output << "//       1. On windows the location is: C:\\Program Files (x86)\\Electronic Arts\\Ultima Online Classic\n";
    output << "//       2. On unix the location is: $HOME/uodata\n";
    output << "//       3. On unix/windows the ENVIRONMENT variable \"UODATA\" is defined to indicate the location\n";
    output << "//  Otherwise, define the DATADIRECTORY to point to the location of the UO data files.\n";
    output << "//==================================================================================================\n" ;
    output <<"[directories]\n{\n";
    for (const auto &[name,location]:this->directoryLocation.dumpPaths()){
        output <<"\t"<<name<<" = " << location << "\n" ;
    }
    //directoryLocation.save(output) ;
    output << "}\n\n" ;
    
    output <<"[skill & stats]\n{\n";
    output << "\t" << UShortValue::nameFor(UShortValue::SKILLLEVEL) << " = " << ushortValues[UShortValue::SKILLLEVEL] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::SKILLCAP) << " = " << ushortValues[UShortValue::SKILLCAP] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::SKILLDELAY) << " = " << ushortValues[UShortValue::SKILLDELAY] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::STATCAP) << " = " << ushortValues[UShortValue::STATCAP] << "\n" ;

    output << "\t" << ServerSwitch::nameFor(ServerSwitch::STATIMPACTSKILL) << " = " << enabled(ServerSwitch::STATIMPACTSKILL) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::EXTENDEDSTATS) << " = " << enabled(ServerSwitch::EXTENDEDSTATS) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::EXTENDEDSKILLS) << " = " << enabled(ServerSwitch::EXTENDEDSKILLS) << "\n" ;
    output << "\t" << ShortValue::nameFor(ShortValue::MAXSTEALTHMOVEMENT) << " = " << shortValues[ShortValue::MAXSTEALTHMOVEMENT] << "\n" ;
    output << "\t" << ShortValue::nameFor(ShortValue::MAXSTAMINAMOVEMENT) << " = " << shortValues[ShortValue::MAXSTAMINAMOVEMENT] << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::SNOOPISCRIME) << " = " << enabled(ServerSwitch::SNOOPISCRIME) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::SNOOPAWARE) << " = " << enabled(ServerSwitch::SNOOPAWARE) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::ARMORIMPACTSMANA) << " = " << enabled(ServerSwitch::ARMORIMPACTSMANA) << "\n" ;

    output << "}\n\n" ;

    output << "[timers]\n{\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::CORPSEDECAY) << " = " << timerSetting[TimerSetting::CORPSEDECAY] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::NPCCORPSEDECAY) << " = " << timerSetting[TimerSetting::NPCCORPSEDECAY] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::WEATHER) << " = " << timerSetting[TimerSetting::WEATHER] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::SHOPSPAWN) << " = " << timerSetting[TimerSetting::SHOPSPAWN] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::DECAY) << " = " << timerSetting[TimerSetting::DECAY] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::DECAYINHOUSE) << " = " << timerSetting[TimerSetting::DECAYINHOUSE] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::INVISIBILITY) << " = " << timerSetting[TimerSetting::INVISIBILITY] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::OBJECTUSAGE) << " = " << timerSetting[TimerSetting::OBJECTUSAGE] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::GATE) << " = " << timerSetting[TimerSetting::GATE] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::POISON) << " = " << timerSetting[TimerSetting::POISON] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::LOGINTIMEOUT) << " = " << timerSetting[TimerSetting::LOGINTIMEOUT] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::HITPOINTREGEN) << " = " << timerSetting[TimerSetting::HITPOINTREGEN] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::STAMINAREGEN) << " = " << timerSetting[TimerSetting::STAMINAREGEN] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::MANAREGEN) << " = " << timerSetting[TimerSetting::MANAREGEN] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::FISHINGBASE) << " = " << timerSetting[TimerSetting::FISHINGBASE] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::FISHINGRANDOM) << " = " << timerSetting[TimerSetting::FISHINGRANDOM] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::SPIRITSPEAK) << " = " << timerSetting[TimerSetting::SPIRITSPEAK] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::PETOFFLINECHECK) << " = " << timerSetting[TimerSetting::PETOFFLINECHECK] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::NPCFLAGUPDATETIMER) << " = " << timerSetting[TimerSetting::NPCFLAGUPDATETIMER] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::BLOODDECAY) << " = " << timerSetting[TimerSetting::BLOODDECAY] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::BLOODDECAYCORPSE) << " = " << timerSetting[TimerSetting::BLOODDECAYCORPSE] << "\n" ;
    output << "}\n\n" ;

    output << "//========================================================================\n" ;
    output << "// Supported era values: core, uo, t2a, uor, td, lbr, aos, se, ml, sa, hs, tol\n" ;
    output << "// Note: A value of 'core' inherits whatever is set in CORESHARDERA\n";
    output << "//========================================================================\n" ;
    output << "[expansion settings]\n{\n" ;
    // Becaause we care about the order"
    output << "\t" << Expansion::nameFor(Expansion::SHARD) << " = " << ruleSets[Expansion::SHARD].describe() << "\n";
    output << "\t" << Expansion::nameFor(Expansion::ARMOR) << " = " << ruleSets[Expansion::ARMOR].describe() << "\n";
    output << "\t" << Expansion::nameFor(Expansion::STRENGTH) << " = " << ruleSets[Expansion::STRENGTH].describe() << "\n";
    output << "\t" << Expansion::nameFor(Expansion::TATIC) << " = " << ruleSets[Expansion::TATIC].describe() << "\n";
    output << "\t" << Expansion::nameFor(Expansion::ANATOMY) << " = " << ruleSets[Expansion::ANATOMY].describe() << "\n";
    output << "\t" << Expansion::nameFor(Expansion::LUMBERJACK) << " = " << ruleSets[Expansion::LUMBERJACK].describe() << "\n";
    output << "\t" << Expansion::nameFor(Expansion::RACIAL) << " = " << ruleSets[Expansion::RACIAL].describe() << "\n";
    output << "\t" << Expansion::nameFor(Expansion::DAMAGE) << " = " << ruleSets[Expansion::DAMAGE].describe() << "\n";
    output << "\t" << Expansion::nameFor(Expansion::SHIELD) << " = " << ruleSets[Expansion::SHIELD].describe() << "\n";
    output << "\t" << Expansion::nameFor(Expansion::WEAPON) << " = " << ruleSets[Expansion::WEAPON].describe() << "\n";
    output << "\t" << Expansion::nameFor(Expansion::WRESTLING) << " = " << ruleSets[Expansion::WRESTLING].describe() << "\n";
    output << "\t" << Expansion::nameFor(Expansion::COMBAT) << " = " << ruleSets[Expansion::COMBAT].describe() << "\n";
    
    output << "}\n\n" ;

    output <<"[settings]\n{\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::CORPSELOOTDECAY) << " = " << enabled(ServerSwitch::CORPSELOOTDECAY) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::GUARDSACTIVE) << " = " << enabled(ServerSwitch::GUARDSACTIVE) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::DEATHANIMATION) << " = " << enabled(ServerSwitch::DEATHANIMATION) << "\n" ;
    output << "\t" << ShortValue::nameFor(ShortValue::AMBIENTSOUND) << " = " << shortValues[ShortValue::AMBIENTSOUND] << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::AMBIENTFOOTSTEPS) << " = " << enabled(ServerSwitch::AMBIENTFOOTSTEPS) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::AUTOACCOUNT) << " = " << enabled(ServerSwitch::AUTOACCOUNT) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::SHOWOFFLINEPCS) << " = " << enabled(ServerSwitch::SHOWOFFLINEPCS) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::ROGUEENABLE) << " = " << enabled(ServerSwitch::ROGUEENABLE) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::PLAYERPERSECUTION) << " = " << enabled(ServerSwitch::PLAYERPERSECUTION) << "\n" ;

    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::FLUSHTIME) << " = " << realNumbers[RealNumberConfig::FLUSHTIME] << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::HTMLSTAT) << " = " << enabled(ServerSwitch::HTMLSTAT) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::SELLBYNAME) << " = " << enabled(ServerSwitch::SELLBYNAME) << "\n" ;
    output << "\t" << ShortValue::nameFor(ShortValue::MAXSELLITEM) << " = " << shortValues[ShortValue::MAXSELLITEM] << "\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::RESTOCKMULTIPLER) << " = " << realNumbers[RealNumberConfig::RESTOCKMULTIPLER] << "\n" ;
    output << "\t" << ShortValue::nameFor(ShortValue::BANKBUYTHRESHOLD) << " = " << shortValues[ShortValue::BANKBUYTHRESHOLD] << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::TRADESYSTEM) << " = " << enabled(ServerSwitch::TRADESYSTEM) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::RANKSYSTEM) << " = " << enabled(ServerSwitch::RANKSYSTEM) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::MAKERMARK) << " = " << enabled(ServerSwitch::MAKERMARK) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::CUTSCROLLREQ) << " = " << enabled(ServerSwitch::CUTSCROLLREQ) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::NPCTRAINING) << " = " << enabled(ServerSwitch::NPCTRAINING) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::HIDEWHILEMOUNTED) << " = " << enabled(ServerSwitch::HIDEWHILEMOUNTED) << "\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::WEIGHTSTR) << " = " << realNumbers[RealNumberConfig::WEIGHTSTR] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::POLYMORPH) << " = " << timerSetting[TimerSetting::POLYMORPH] << "\n" ;
    output << "\t" << ClientFeature::name << " = " << this->clientFeature.value() << "\n" ;
    output << "\t" << ServerFeature::name << " = " << this->serverFeature.value()  << "\n" ;
    output << "\t" << SpawnFacet::name << " = " << spawnFacet.value() <<"\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::OVERLOADPACKETS) << " = " << enabled(ServerSwitch::OVERLOADPACKETS) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::ADVANCEDPATHFINDING) << " = " << enabled(ServerSwitch::ADVANCEDPATHFINDING) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::LOOTINGISCRIME) << " = " << enabled(ServerSwitch::LOOTINGISCRIME) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::BASICTOOLTIPSONLY) << " = " << enabled(ServerSwitch::BASICTOOLTIPSONLY) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::DISPLAYREPUTATIONTOOLTIP) << " = " << enabled(ServerSwitch::DISPLAYREPUTATIONTOOLTIP) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::DISPLAYGUILDTOOLTIP) << " = " << enabled(ServerSwitch::DISPLAYGUILDTOOLTIP) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::NPCTOOLTIPS) << " = " << enabled(ServerSwitch::NPCTOOLTIPS) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::OVERHEADTITLE) << " = " << enabled(ServerSwitch::OVERHEADTITLE) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::DISPLAYINVUNERABLE) << " = " << enabled(ServerSwitch::DISPLAYINVUNERABLE) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::DISPLAYRACE) << " = " << enabled(ServerSwitch::DISPLAYRACE) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::DISPLAYRACEPAPERDOLL) << " = " << enabled(ServerSwitch::DISPLAYRACEPAPERDOLL) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::ITEMDECAY) << " = " << enabled(ServerSwitch::ITEMDECAY) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::SCRIPTITEMSDECAYABLE) << " = " << enabled(ServerSwitch::SCRIPTITEMSDECAYABLE) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::BASEITEMSDECAYABLE) << " = " << enabled(ServerSwitch::BASEITEMSDECAYABLE) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::DISPLAYGUILDBUTTON) << " = " << enabled(ServerSwitch::DISPLAYGUILDBUTTON) << "\n" ;
    output << "\t" << ShortValue::nameFor(ShortValue::FISHINGSTAMINALOSS) << " = " << shortValues[ShortValue::FISHINGSTAMINALOSS] << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::ITEMDETECTSPEECH) << " = " << enabled(ServerSwitch::ITEMDETECTSPEECH) << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::MAXPLAYERPACKITEM) << " = " << ushortValues[UShortValue::MAXPLAYERPACKITEM] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::MAXPLAYERBANKITEM) << " = " << ushortValues[UShortValue::MAXPLAYERBANKITEM] << "\n" ;
    output << "\t" << UIntValue::nameFor(UIntValue::MAXPLAYERPACKWEIGHT) << " = " << uintValues[UIntValue::MAXPLAYERPACKWEIGHT] << "\n" ;
    output << "\t" << UIntValue::nameFor(UIntValue::MAXPLAYERBANKWEIGHT) << " = " << uintValues[UIntValue::MAXPLAYERBANKWEIGHT] << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::FORECENEWANIMATIONPACKET) << " = " << enabled(ServerSwitch::FORECENEWANIMATIONPACKET) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::MAPDIFF) << " = " << enabled(ServerSwitch::MAPDIFF) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::TOOLUSE) << " = " << enabled(ServerSwitch::TOOLUSE) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::TOOLBREAK) << " = " << enabled(ServerSwitch::TOOLBREAK) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::REPAIRLOSS) << " = " << enabled(ServerSwitch::REPAIRLOSS) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::COLORWEAPON) << " = " << enabled(ServerSwitch::COLORWEAPON) << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::MAXSAFETELEPORT) << " = " << ushortValues[UShortValue::MAXSAFETELEPORT] << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::SAFETELEPORT) << " = " << enabled(ServerSwitch::SAFETELEPORT) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::AWAKENPC) << " = " << enabled(ServerSwitch::AWAKENPC) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::GUILDDISCOUNT) << " = " << enabled(ServerSwitch::GUILDDISCOUNT) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::GUILDPREMIUM) << " = " << enabled(ServerSwitch::GUILDPREMIUM) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::YOUNGPLAYER) << " = " << enabled(ServerSwitch::YOUNGPLAYER) << "\n" ;
    
    output << "}\n\n" ;

    output << "[pets and followers]\n{\n";
    output << "\t" << UShortValue::nameFor(UShortValue::MAXCONTROLSLOT) << " = " << ushortValues[UShortValue::MAXCONTROLSLOT] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::MAXFOLLOWER) << " = " << ushortValues[UShortValue::MAXFOLLOWER] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::MAXPETOWNER) << " = " << ushortValues[UShortValue::MAXPETOWNER] << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::PETDIFFICULTY) << " = " << enabled(ServerSwitch::PETDIFFICULTY) << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::PETLOYALITYGAINSUCCESS) << " = " << ushortValues[UShortValue::PETLOYALITYGAINSUCCESS] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::PETLOYALITYLOSSFAILURE) << " = " << ushortValues[UShortValue::PETLOYALITYLOSSFAILURE] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::LOYALTYRATE) << " = " << timerSetting[TimerSetting::LOYALTYRATE] << "\n" ;
    
    output << "}\n\n" ;

    output << "[speedup]\n{\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::CHECKITEMS) << " = " << realNumbers[RealNumberConfig::CHECKITEMS] << "\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::CHECKBOATS) << " = " << realNumbers[RealNumberConfig::CHECKBOATS] << "\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::CHECKAI) << " = " << realNumbers[RealNumberConfig::CHECKAI] << "\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::CHECKSPAWN) << " = " << realNumbers[RealNumberConfig::CHECKSPAWN] << "\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::NPCMOVEMENT) << " = " << realNumbers[RealNumberConfig::NPCMOVEMENT] << "\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::NPCRUNNING) << " = " << realNumbers[RealNumberConfig::NPCRUNNING] << "\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::NPCFLEEING) << " = " << realNumbers[RealNumberConfig::NPCFLEEING] << "\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::NPCMOUNTMOVEMENT) << " = " << realNumbers[RealNumberConfig::NPCMOUNTMOVEMENT] << "\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::NPCMOUNTRUNNING) << " = " << realNumbers[RealNumberConfig::NPCMOUNTRUNNING] << "\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::NPCMOUNTFLEEING) << " = " << realNumbers[RealNumberConfig::NPCMOUNTFLEEING] << "\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::NPCSPELLCAST) << " = " << realNumbers[RealNumberConfig::NPCSPELLCAST] << "\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::GLOBALATTACK) << " = " << realNumbers[RealNumberConfig::GLOBALATTACK] << "\n" ;
     
    output << "}\n\n" ;

    output << "[message boards]\n{\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::POSTINGLEVEL) << " = " << ushortValues[UShortValue::POSTINGLEVEL] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::REMOVALLEVEL) << " = " << ushortValues[UShortValue::REMOVALLEVEL] << "\n";
    
    output << "}\n\n" ;

    output << "[escorts]\n{\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::ESCORTS) << " = " << this->enabled(ServerSwitch::ESCORTS) << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::ESCORTWAIT) << " = " << timerSetting[TimerSetting::ESCORTWAIT] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::ESCORTACTIVE) << " = " << timerSetting[TimerSetting::ESCORTACTIVE] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::ESCORTDONE) << " = " << timerSetting[TimerSetting::ESCORTDONE] << "\n" ;
    
    output << "}\n\n" ;

    output << "[worldlight]\n{\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::DUNGEONLIGHT) << " = " << ushortValues[UShortValue::DUNGEONLIGHT] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::BRIGHTLEVEL) << " = " << ushortValues[UShortValue::BRIGHTLEVEL] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::DARKLEVEL) << " = " << ushortValues[UShortValue::DARKLEVEL] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::SECONDSPERUOMINUTE) << " = " << ushortValues[UShortValue::SECONDSPERUOMINUTE] << "\n";
    output << "}\n\n" ;

    output << "[tracking]\n{\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::BASERANGE) << " = " << ushortValues[UShortValue::BASERANGE] << "\n";
    output << "\t" << TimerSetting::nameFor(TimerSetting::TRACKING) << " = " << timerSetting[TimerSetting::TRACKING] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::MAXTARGET) << " = " << ushortValues[UShortValue::MAXTARGET] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::MSGREDISPLAYTIME) << " = " << ushortValues[UShortValue::MSGREDISPLAYTIME] << "\n";
    
    output << "}\n\n" ;

    output << "[reputation]\n{\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::MURDERDECAY) << " = " << timerSetting[TimerSetting::MURDERDECAY] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::MAXKILL) << " = " << ushortValues[UShortValue::MAXKILL] << "\n";
    output << "\t" << TimerSetting::nameFor(TimerSetting::CRIMINAL) << " = " << timerSetting[TimerSetting::CRIMINAL] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::AGGRESSORFLAG) << " = " << timerSetting[TimerSetting::AGGRESSORFLAG] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::PERMAGREYFLAG) << " = " << timerSetting[TimerSetting::PERMAGREYFLAG] << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::STEALINGFLAG) << " = " << timerSetting[TimerSetting::STEALINGFLAG] << "\n" ;
    output << "}\n\n" ;

    output << "[resources]\n{\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::RESOURCEAREASIZE) << " = " << ushortValues[UShortValue::RESOURCEAREASIZE] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::MINECHECK) << " = " << ushortValues[UShortValue::MINECHECK] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::OREPERAREA) << " = " << ushortValues[UShortValue::OREPERAREA] << "\n";
    output << "\t" << TimerSetting::nameFor(TimerSetting::ORE) << " = " << timerSetting[TimerSetting::ORE] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::LOGPERAREA) << " = " << ushortValues[UShortValue::LOGPERAREA] << "\n";
    output << "\t" << TimerSetting::nameFor(TimerSetting::LOG) << " = " << timerSetting[TimerSetting::LOG] << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::FISHPERAREA) << " = " << ushortValues[UShortValue::FISHPERAREA] << "\n";
    output << "\t" << TimerSetting::nameFor(TimerSetting::FISH) << " = " << timerSetting[TimerSetting::FISH] << "\n" ;
    
    output << "}\n\n" ;

    output << "[hunger]\n{\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::HUNGER) << " = " << this->enabled(ServerSwitch::HUNGER) << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::HUNGERRATE) << " = " << timerSetting[TimerSetting::HUNGERRATE] << "\n" ;
    output << "\t" << ShortValue::nameFor(ShortValue::HUNGERDAMAGE) << " = " << shortValues[ShortValue::HUNGERDAMAGE] << "\n";
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::PETHUNGEROFFLINE) << " = " << this->enabled(ServerSwitch::PETHUNGEROFFLINE) << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::PETOFFLINETIMEOUT) << " = " << ushortValues[UShortValue::PETOFFLINETIMEOUT] << "\n";

    output << "}\n\n" ;

    output << "[thirst]\n{\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::THIRST) << " = " << this->enabled(ServerSwitch::THIRST) << "\n" ;
    output << "\t" << TimerSetting::nameFor(TimerSetting::THIRSTRATE) << " = " << timerSetting[TimerSetting::THIRSTRATE] << "\n" ;
    output << "\t" << ShortValue::nameFor(ShortValue::THIRSTDRAIN) << " = " << shortValues[ShortValue::THIRSTDRAIN] << "\n";
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::PETTHIRSTOFFLINE) << " = " << this->enabled(ServerSwitch::PETTHIRSTOFFLINE) << "\n" ;

    output << "}\n\n" ;

    output << "[combat]\n{\n" ;
    output << "\t" << ShortValue::nameFor(ShortValue::MAXRANGE) << " = " << shortValues[ShortValue::MAXRANGE] << "\n";
    output << "\t" << ShortValue::nameFor(ShortValue::MAXSPELLRANGE) << " = " << shortValues[ShortValue::MAXSPELLRANGE] << "\n";
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::DISPLAYHITMSG) << " = " << this->enabled(ServerSwitch::DISPLAYHITMSG) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::DISPLAYDAMAGENUMBERS) << " = " << this->enabled(ServerSwitch::DISPLAYDAMAGENUMBERS) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::MONSTERSVSANIMALS) << " = " << this->enabled(ServerSwitch::MONSTERSVSANIMALS) << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::ANIMALATTACKCHANCE) << " = " << ushortValues[UShortValue::ANIMALATTACKCHANCE] << "\n";
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::ANIMALSGUARDED) << " = " << this->enabled(ServerSwitch::ANIMALSGUARDED) << "\n" ;
    output << "\t" << ShortValue::nameFor(ShortValue::NPCDAMAGERATE) << " = " << shortValues[ShortValue::NPCDAMAGERATE] << "\n";
    output << "\t" << ShortValue::nameFor(ShortValue::NPCBASEFLEEAT) << " = " << shortValues[ShortValue::NPCBASEFLEEAT] << "\n";
    output << "\t" << ShortValue::nameFor(ShortValue::NPCBASEREATTACKAT) << " = " << shortValues[ShortValue::NPCBASEREATTACKAT] << "\n";
    output << "\t" << ShortValue::nameFor(ShortValue::ATTACKSTAMINA) << " = " << shortValues[ShortValue::ATTACKSTAMINA] << "\n";
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::ATTACKSPEEDFROMSTAMINA) << " = " << this->enabled(ServerSwitch::ATTACKSPEEDFROMSTAMINA) << "\n" ;
    output << "\t" << ShortValue::nameFor(ShortValue::ARCHERYTHITBONUS) << " = " << shortValues[ShortValue::ARCHERYTHITBONUS] << "\n";
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::ARCHERYDELAY) << " = " << realNumbers[RealNumberConfig::ARCHERYDELAY] << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::SHOOTONANIMALBACK) << " = " << this->enabled(ServerSwitch::SHOOTONANIMALBACK) << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::WEAPONDAMAGECHANCE) << " = " << ushortValues[UShortValue::WEAPONDAMAGECHANCE] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::MINWEAPONDAMAGE) << " = " << ushortValues[UShortValue::MINWEAPONDAMAGE] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::MAXWEAPONDAMAGE) << " = " << ushortValues[UShortValue::MAXWEAPONDAMAGE] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::ARMORDAMAGECHANCE) << " = " << ushortValues[UShortValue::ARMORDAMAGECHANCE] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::MINARMORDAMAGE) << " = " << ushortValues[UShortValue::MINARMORDAMAGE] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::MAXARMORDAMAGE) << " = " << ushortValues[UShortValue::MAXARMORDAMAGE] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::PARRYDAMAGECHANCE) << " = " << ushortValues[UShortValue::PARRYDAMAGECHANCE] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::MINPARRYDAMAGE) << " = " << ushortValues[UShortValue::MINPARRYDAMAGE] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::MAXPARRYDAMAGE) << " = " << ushortValues[UShortValue::MAXPARRYDAMAGE] << "\n";
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::ARMORCLASSBONUS) << " = " << this->enabled(ServerSwitch::ARMORCLASSBONUS) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::ALCHEMYBONUS) << " = " << this->enabled(ServerSwitch::ALCHEMYBONUS) << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::ALCHEMENYBONUSMODIFIER) << " = " << ushortValues[UShortValue::ALCHEMENYBONUSMODIFIER] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::BLOODEFFECTCHANCE) << " = " << ushortValues[UShortValue::BLOODEFFECTCHANCE] << "\n";

    output << "\t" << ServerSwitch::nameFor(ServerSwitch::INTERRUPTCASTING) << " = " << this->enabled(ServerSwitch::INTERRUPTCASTING) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::PETCOMBATTRAINING) << " = " << this->enabled(ServerSwitch::PETCOMBATTRAINING) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::HIRELINGTRAINING) << " = " << this->enabled(ServerSwitch::HIRELINGTRAINING) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::NPCCOMBAT) << " = " << this->enabled(ServerSwitch::NPCCOMBAT) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::DISPLAYRESISTSTATS) << " = " << this->enabled(ServerSwitch::DISPLAYRESISTSTATS) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::DISPLAYDAMAGETYPE) << " = " << this->enabled(ServerSwitch::DISPLAYDAMAGETYPE) << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::WEAPONDAMAGEBONUSTYPE) << " = " << ushortValues[UShortValue::WEAPONDAMAGEBONUSTYPE] << "\n";
    output << "}\n\n" ;

    output << "[magic]\n{\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::TRAVELBOATKEY) << " = " << this->enabled(ServerSwitch::TRAVELBOATKEY) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::TRAVELBURDEN) << " = " << this->enabled(ServerSwitch::TRAVELBURDEN) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::RUNEMULTI) << " = " << this->enabled(ServerSwitch::RUNEMULTI) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::SPELLWORLDTRAVEL) << " = " << this->enabled(ServerSwitch::SPELLWORLDTRAVEL) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::TRAVELAGRESSOR) << " = " << this->enabled(ServerSwitch::TRAVELAGRESSOR) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::MAGICSTATS) << " = " << this->enabled(ServerSwitch::MAGICSTATS) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::SPELLMOVING) << " = " << this->enabled(ServerSwitch::SPELLMOVING) << "\n" ;
    
    output << "}\n\n" ;

    output << "[start locations]\n{\n" ;
    output<< this->startLocation.describe() << "\n" ;
    
    output << "}\n\n" ;

    output << "[young start locations]\n{\n" ;
    output<< this->youngLocation.describe() << "\n" ;

    
    output << "}\n\n" ;

    output << "[startup]\n{\n" ;
    output << "\t" << ShortValue::nameFor(ShortValue::STARTGOLD) << " = " << shortValues[ShortValue::STARTGOLD] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::STARTPRIV) << " = " << ushortValues[UShortValue::STARTPRIV] << "\n";
    output << "}\n\n" ;

    output << "[gumps]\n{\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::TITLECOLOR) << " = " << ushortValues[UShortValue::TITLECOLOR] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::LEFTTEXTCOLOR) << " = " << ushortValues[UShortValue::LEFTTEXTCOLOR] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::RIGHTTEXTCOLOR) << " = " << ushortValues[UShortValue::RIGHTTEXTCOLOR] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::BUTTONCANCEL) << " = " << ushortValues[UShortValue::BUTTONCANCEL] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::BUTTONLEFT) << " = " << ushortValues[UShortValue::BUTTONLEFT] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::BUTTONRIGHT) << " = " << ushortValues[UShortValue::BUTTONRIGHT] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::BACKGROUNDPIC) << " = " << ushortValues[UShortValue::BACKGROUNDPIC] << "\n";
    
    output << "}\n\n" ;

    output << "[houses]\n{\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::TRACKHOUSESPERACCOUNT) << " = " << this->enabled(ServerSwitch::TRACKHOUSESPERACCOUNT) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::OWNCOOWNHOUSE) << " = " << this->enabled(ServerSwitch::OWNCOOWNHOUSE) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::COWNHOUSEACCOUNT) << " = " << this->enabled(ServerSwitch::COWNHOUSEACCOUNT) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::INHOUSEDECAY) << " = " << this->enabled(ServerSwitch::INHOUSEDECAY) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::PROTECTPRIVATEHOUSES) << " = " << this->enabled(ServerSwitch::PROTECTPRIVATEHOUSES) << "\n" ;
    output << "\t" << UShortValue::nameFor(UShortValue::MAXHOUSEOWNABLE) << " = " << ushortValues[UShortValue::MAXHOUSEOWNABLE] << "\n";
    output << "\t" << UShortValue::nameFor(UShortValue::MAXHOUSECOOWNABLE) << " = " << ushortValues[UShortValue::MAXHOUSECOOWNABLE] << "\n";

    output << "\t" << ServerSwitch::nameFor(ServerSwitch::COOWNERLOGOUT) << " = " << this->enabled(ServerSwitch::COOWNERLOGOUT) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::FRIENDLOGOUT) << " = " << this->enabled(ServerSwitch::FRIENDLOGOUT) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::GUESTLOGOUT) << " = " << this->enabled(ServerSwitch::GUESTLOGOUT) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::KEYLESSOWNER) << " = " << this->enabled(ServerSwitch::KEYLESSOWNER) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::KEYLESSCOOWNER) << " = " << this->enabled(ServerSwitch::KEYLESSCOOWNER) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::KEYLESSFRIEND) << " = " << this->enabled(ServerSwitch::KEYLESSFRIEND) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::KEYLESSGUEST) << " = " << this->enabled(ServerSwitch::KEYLESSGUEST) << "\n" ;
    
    output << "}\n\n" ;

    output << "[bulk order deeds]\n{\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::OFFERBODSFROMITEMSALES) << " = " << this->enabled(ServerSwitch::OFFERBODSFROMITEMSALES) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::OFFERBODSFROMCONTEXTMENU) << " = " << this->enabled(ServerSwitch::OFFERBODSFROMCONTEXTMENU) << "\n" ;
    output << "\t" << ServerSwitch::nameFor(ServerSwitch::BODSFROMCRAFTEDITEMSONLY) << " = " << this->enabled(ServerSwitch::BODSFROMCRAFTEDITEMSONLY) << "\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::GOLDREWARDMULTIPLER) << " = " << realNumbers[RealNumberConfig::GOLDREWARDMULTIPLER] << "\n" ;
    output << "\t" << RealNumberConfig::nameFor(RealNumberConfig::FAMEREWARDMULTIPLER) << " = " << realNumbers[RealNumberConfig::FAMEREWARDMULTIPLER] << "\n" ;
    
    output << "}\n\n" ;

    output << "[towns]\n{\n" ;
    output << "\t" << UIntValue::nameFor(UIntValue::POLLTIME) << " = " << uintValues[UIntValue::POLLTIME] << "\n" ;
    output << "\t" << UIntValue::nameFor(UIntValue::MAYORTIME) << " = " << uintValues[UIntValue::MAYORTIME] << "\n" ;
    output << "\t" << UIntValue::nameFor(UIntValue::TAXPERIOD) << " = " << uintValues[UIntValue::TAXPERIOD] << "\n" ;
    output << "\t" << UIntValue::nameFor(UIntValue::GUARDPAID) << " = " << uintValues[UIntValue::GUARDPAID] << "\n" ;
    output << "}\n\n" ;

    output << "[disabled assistant features]\n{\n" ;
    output << this->assistantFeature.describe() << "\n" ;
    
    output << "}\n\n" ;

    return true ;
}
//======================================================================
auto ServerConfig::directoryFor(dirlocation_t location) const -> const std::filesystem::path &{
    return directoryLocation[location] ;
}
//======================================================================
auto ServerConfig::unwindDirectoryFor(dirlocation_t location) const -> std::filesystem::path{
    return directoryLocation.unwindPath(location);
}

//======================================================================
auto ServerConfig::enabled(ServerSwitch::switch_t setting) const ->bool {
    return serverSwitch[setting];
}
//======================================================================
auto ServerConfig::setEnabled(ServerSwitch::switch_t setting, bool state) ->void {
    serverSwitch.setSetting(setting,state) ;
}
//======================================================================
auto ServerConfig::timer(TimerSetting::timer_t timer) const -> const std::uint16_t& {
    return timerSetting[timer];
}

//======================================================================
auto ServerConfig::dataForKeyword(const std::string &key) const -> std::optional<AllDataType>{
    auto ukey = util::upper(key) ;
    auto data = AllDataType() ;
    // Check switches
    auto bvalue = serverSwitch.valueFor(ukey);
    if (bvalue.has_value()){
        data.type = AllDataType::T_BOOL ;
        data.value = bvalue.value() ;
        return data ;
    }
    // Check short values
    auto s16value = shortValues.valueFor(ukey) ;
    if (s16value.has_value()){
        data.type = AllDataType::T_INT16 ;
        data.value = s16value.value() ;
        return data ;
    }
    // Check ushort values
    auto u16value = ushortValues.valueFor(ukey) ;
    if (u16value.has_value()){
        data.type = AllDataType::T_UINT16 ;
        data.value = u16value.value() ;
        return data ;
    }
    // Check timersetting values
    u16value = timerSetting.valueFor(ukey) ;
    if (u16value.has_value()){
        data.type = AllDataType::T_UINT16 ;
        data.value = u16value.value() ;
        return data ;
    }

    // Check uint values
    auto u32value = uintValues.valueFor(ukey) ;
    if (u32value.has_value()){
        data.type = AllDataType::T_UINT16 ;
        data.value = u16value.value() ;
        return data ;
    }
    // check real values
    auto r64value = realNumbers.valueFor(ukey) ;
    if (r64value.has_value()){
        data.type = AllDataType::T_DOUBLE ;
        data.value = r64value.value() ;
        return data ;
    }
   // check clients
    bvalue = enableClients.valueFor(ukey);
    if (bvalue.has_value()){
        data.type = AllDataType::T_BOOL ;
        data.value = bvalue.value() ;
        return data ;
    }
    // check feature
    bvalue = assistantFeature.valueFor(ukey);
    if (bvalue.has_value()){
        data.type = AllDataType::T_BOOL ;
        data.value = bvalue.value() ;
        return data ;
    }
    // check string
    auto svalue = serverString.valueFor(ukey) ;
    if (svalue.has_value()){
        data.type = AllDataType::T_STRING ;
        data.value = svalue.value() ;
        return data ;
    }
    //check directory
    svalue = directoryLocation.valueFor(ukey) ;
    if (svalue.has_value()){
        data.type = AllDataType::T_STRING ;
        data.value = svalue.value() ;
        return data ;
    }
    // Check execeptions (server/client fatures)
    if (ukey == "SERVERFEATURES"){
        data.type = AllDataType::T_UINT16;
        data.value = static_cast<std::uint16_t>(serverFeature.value());
        return data ;
    }
    if (ukey == "CLIENTFEATURES"){
        data.type = AllDataType::T_UINT32;
        data.value = static_cast<std::uint32_t>(clientFeature.value());
        return data ;
    }
    if (ukey == "SPAWNREGIONSFACETS"){
        data.type = AllDataType::T_UINT64 ;
        data.value = static_cast<std::uint64_t>(spawnFacet.value());
        return data ;
    }
    return {};
}
