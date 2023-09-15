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
    return rvalue ;
}
//======================================================================
auto ServerConfig::reloadKeyValue(const std::string &key, const std::string &value)->bool {
    auto rvalue = true ;
    // We dont reload the entire config file
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
    directoryLocation.reset();
    enableClients.reset() ;
    clientFeature = ClientFeature() ;
    serverFeature = ServerFeature() ;
    assistantFeature = AssistantFeature() ;
    startLocation.reset();
    youngLocation.reset();
    serverSwitch.reset();
    serverString.reset();
    ruleSets.reset();
    timerSetting.reset();
    realNumbers.reset();
    ushortValues.reset();
    uintValues.reset();
    
    // Strings
    // Done in reset
    // Switches
    // Done in reset
    // Timers
    // Done in reset

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
auto ServerConfig::reloadConfig() ->void{
    auto input = std::ifstream(configFile.string());
    if (!input.is_open()){
        throw std::runtime_error("Unable to open UOX3 configuration file: "s + configFile.string());
    }
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
                    reloadKeyValue(key, value);
                }
            }
        }
    }
    postCheck();
}
//======================================================================
auto ServerConfig::writeConfig(const std::filesystem::path &config) const ->bool {
    auto output = std::ofstream(config.string()) ;
    if (!output.is_open()){
        //throw std::runtime_error("Unable to open: "s+config.string());
        return false ;
    }
    // Now if we stored by "type", we could automate this, but we dont
    output << "//========================================================================\n" ;
    output << "// UOX3 Initialization File. V" ;
    output << (static_cast<std::uint16_t>(1) << 8 | static_cast<std::uint16_t>(2)) <<"\n" ;
    output << "//========================================================================\n\n" ;
    output << "[system]\n{\n" ;
    output << "\tSERVERNAME = " << serverString[ServerString::SERVERNAME] << "\n" ;
    output << "\tEXTERNALIP = " << serverString[ServerString::PUBLICIP] << "\n" ;
//    output << "\tPORT = " << ServerPort() <<"n" ;
    output << "\tSECRETSHARDKEY = " << serverString[ServerString::SHARDKEY] << "\n" ;
//    output << "\tSERVERLANGUAGE =" << ServerLanguage() << "n" ;
//    output << "\tNETRCVTIMEOUT =" << ServerNetRcvTimeout() << "n" ;
//    output << "\tNETSNDTIMEOUT =" << ServerNetSndTimeout() << "n" ;
//    output << "\tNETRETRYCOUNT =" << ServerNetRetryCount() << "n" ;
    output << "\tCONSOLELOG = " << this->enabled(ServerSwitch::CONSOLELOG) << "\n" ;
    output << "\tNETWORKLOG = " << this->enabled(ServerSwitch::NETWORKLOG) << "\n" ;
    output << "\tSPEECHLOG = " << this->enabled(ServerSwitch::SPEECHLOG) << "\n" ;
    output << "\tCOMMANDPREFIX = " << serverString[ServerString::COMMANDPREFIX] << "\n" ;
    output << "\tANNOUNCEWORLDSAVES = " << this->enabled(ServerSwitch::ANNOUNCESAVE) << "\n" ;
    output << "\tJOINPARTMSGS = " << this->enabled(ServerSwitch::ANNOUNCEJOINPART) << "\n" ;
    output << "\tBACKUPSENABLED = " << this->enabled(ServerSwitch::BACKUP) << "\n" ;
    //output << "\tBACKUPSAVERATIO = " << BackupRatio() << "n" ;
    //output << "\tSAVESTIMER = " << ServerSavesTimerStatus() << "n" ;
//    output << "\tACCOUNTISOLATION = " << "1" << "n" ;
    output << "\tUOGENABLED = " << this->enabled(ServerSwitch::UOG) << "\n" ;
    output << "\tFREESHARDSERVERPOLL = " << this->enabled(ServerSwitch::FREESHARD) << "\n" ;
    output << "\tRANDOMSTARTINGLOCATION = " << this->enabled(ServerSwitch::RANDOMSTART) << "\n" ;
    output << "\tASSISTANTNEGOTIATION = " << this->enabled(ServerSwitch::ASSISTANTNEGOTIATION) << "\n" ;
    output << "\tKICKONASSISTANTSILENCE = " << this->enabled(ServerSwitch::KICKONASSISTANTSILENCE) << "\n" ;
    output << "\tCLASSICUOMAPTRACKER = " << this->enabled(ServerSwitch::CUOMAPTRACKER) << "\n" ;
//    output << "\tJSENGINESIZE = " << static_cast<std::uint16_t>(GetJSEngineSize()) <<< "\n" ;
    output << "\tUSEUNICODEMESSAGES = " << this->enabled(ServerSwitch::UNICODEMESSAGE) << "\n" ;
    output << "\tCONTEXTMENUS = " << this->enabled(ServerSwitch::CONTEXTMENU) << "\n" ;
//    output << "\tSYSMESSAGECOLOUR = " << SysMsgColour() << "\n" ;
//    output << "\tMAXCLIENTBYTESIN = " << static_cast<std::uint32_t>(MaxClientBytesIn()) << "\n" ;
//    output << "\tMAXCLIENTBYTESOUT = " << static_cast<std::uint32_t>(MaxClientBytesOut()) << "\n" ;
//    output << "\tNETTRAFFICTIMEBAN = " << static_cast<std::uint32_t>(NetTrafficTimeban()) << "\n" ;
//    output << "\tAPSPERFTHRESHOLD = " << static_cast<std::uint16_t>(APSPerfThreshold()) << "\n" ;
//    output << "\tAPSINTERVAL = " << static_cast<std::uint16_t>(APSInterval()) << "\n" ;
//    output << "\tAPSDELAYSTEP = " << static_cast<std::uint16_t>(APSDelayStep()) << "\n" ;
//    output << "\tAPSDELAYMAXCAP = " << static_cast<std::uint16_t>(APSDelayMaxCap()) << "\n" ;
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
//    output << "\tSKILLLEVEL = " << static_cast<std::uint16_t>(SkillLevel()) << "\n" ;
//    output << "\tSKILLCAP = " << ServerSkillTotalCapStatus() << "\n" ;
//    output << "\tSKILLDELAY = " << static_cast<std::uint16_t>(ServerSkillDelayStatus()) << "\n" ;
//    output << "\tSTATCAP = " << ServerStatCapStatus() << "\n" ;
    output << "\tSTATSAFFECTSKILLCHECKS = " << this->enabled(ServerSwitch::STATIMPACTSKILL) << "\n" ;
    output << "\tEXTENDEDSTARTINGSTATS = " << this->enabled(ServerSwitch::EXTENDEDSTATS) << "\n" ;
    output << "\tEXTENDEDSTARTINGSKILLS = " << this->enabled(ServerSwitch::EXTENDEDSKILLS) <<  "\n" ;
//    output << "\tMAXSTEALTHMOVEMENTS = " << MaxStealthMovement() << "\n" ;
//    output << "\tMAXSTAMINAMOVEMENTS = " << MaxStaminaMovement() << "\n" ;
    output << "\tSNOOPISCRIME = " << this->enabled(ServerSwitch::SNOOPISCRIME) << "\n" ;
    output << "\tSNOOPAWARENESS = " << this->enabled(ServerSwitch::SNOOPAWARE) << "\n" ;
    output << "\tARMORAFFECTMANAREGEN = " << this->enabled(ServerSwitch::ARMORIMPACTSMANA) << "\n" ;
    output << "}\n\n" ;

    output << "[timers]\n{\n" ;
//    output << "\tCORPSEDECAYTIMER = " << SystemTimer(tSERVER_CORPSEDECAY) << "\n" ;
//    output << "\tNPCCORPSEDECAYTIMER = " << SystemTimer(tSERVER_NPCCORPSEDECAY) << "\n" ;
//    output << "\tWEATHERTIMER = " << SystemTimer(tSERVER_WEATHER) << "\n" ;
//    output << "\tSHOPSPAWNTIMER = " << SystemTimer(tSERVER_SHOPSPAWN) << "\n" ;
//    output << "\tDECAYTIMER = " << SystemTimer(tSERVER_DECAY) << "\n" ;
//    output << "\tDECAYTIMERINHOUSE = " << SystemTimer(tSERVER_DECAYINHOUSE) << "\n" ;
//    output << "\tINVISIBILITYTIMER = " << SystemTimer(tSERVER_INVISIBILITY) << "\n" ;
//    output << "\tOBJECTUSETIMER = " << SystemTimer(tSERVER_OBJECTUSAGE) << "\n" ;
//    output << "\tGATETIMER = " << SystemTimer(tSERVER_GATE) << "\n" ;
//    output << "\tPOISONTIMER = " << SystemTimer(tSERVER_POISON) << "\n" ;
//    output << "\tLOGINTIMEOUT = " << SystemTimer(tSERVER_LOGINTIMEOUT) << "\n" ;
//    output << "\tHITPOINTREGENTIMER = " << SystemTimer(tSERVER_HITPOINTREGEN) << "\n" ;
//    output << "\tSTAMINAREGENTIMER = " << SystemTimer(tSERVER_STAMINAREGEN) << "\n" ;
//    output << "\tMANAREGENTIMER = " << SystemTimer(tSERVER_MANAREGEN) <<< "\n" ;
//    output << "\tBASEFISHINGTIMER = " << SystemTimer(tSERVER_FISHINGBASE) << "\n" ;
//    output << "\tRANDOMFISHINGTIMER = " << SystemTimer(tSERVER_FISHINGRANDOM) << "\n" ;
//    output << "\tSPIRITSPEAKTIMER = " << SystemTimer(tSERVER_SPIRITSPEAK) << "\n" ;
//    output << "\tPETOFFLINECHECKTIMER = " << SystemTimer(tSERVER_PETOFFLINECHECK) << "\n" ;
//    output << "\tNPCFLAGUPDATETIMER = " << SystemTimer(tSERVER_NPCFLAGUPDATETIMER) << "\n" ;
//    output << "\tBLOODDECAYTIMER = " << SystemTimer(tSERVER_BLOODDECAY) << "\n" ;
//    output << "\tBLOODDECAYCORPSETIMER = " << SystemTimer(tSERVER_BLOODDECAYCORPSE) << "\n" ;
    output << "}\n\n" ;

    output << "//========================================================================\n" ;
    output << "// Supported era values: core, uo, t2a, uor, td, lbr, aos, se, ml, sa, hs, tol\n" ;
    output << "// Note: A value of 'core' inherits whatever is set in CORESHARDERA\n";
    output << "//========================================================================\n" ;
    output << "[expansion settings]\n{\n" ;
    // Becaause we care about the order"
    output << "\tCORESHARDERA = " << ruleSets[Expansion::SHARD].describe() << "\n" ;
    output << "\tARMORCALCULATION = " << ruleSets[Expansion::ARMOR].describe() << "\n" ;
    output << "\tSTRENGTHDAMAGEBONUS = " << ruleSets[Expansion::STRENGTH].describe() << "\n" ;
    output << "\tTACTICSDAMAGEBONUS = " <<ruleSets[Expansion::TATIC].describe() << "\n" ;
    output << "\tANATOMYDAMAGEBONUS = "  << ruleSets[Expansion::DAMAGE].describe() << "\n" ;
    output << "\tLUMBERJACKDAMAGEBONUS = " << ruleSets[Expansion::LUMBERJACK].describe() << "\n" ;
    output << "\tRACIALDAMAGEBONUS = " << ruleSets[Expansion::RACIAL].describe() << "\n" ;
    output << "\tDAMAGEBONUSCAP = " << ruleSets[Expansion::DAMAGE].describe() << "\n" ;
    output << "\tSHIELDPARRY = " << ruleSets[Expansion::SHIELD].describe() << "\n" ;
    output << "\tWEAPONPARRY ="  << ruleSets[Expansion::WEAPON].describe() << "\n" ;
    output << "\tWRESTLINGPARRY = " << ruleSets[Expansion::WRESTLING].describe() << "\n" ;
    output << "\tCOMBATHITCHANCE = " << ruleSets[Expansion::COMBAT].describe() << "\n" ;
    
    output << "}\n\n" ;

    output <<"[settings]\n{\n" ;

    output << "\tLOOTDECAYSWITHCORPSE = " << this->enabled(ServerSwitch::CORPSELOOTDECAY) << "\n" ;
    output << "\tGUARDSACTIVE = " << this->enabled(ServerSwitch::GUARDSACTIVE) << "\n" ;
    output << "\tDEATHANIMATION = " << this->enabled(ServerSwitch::DEATHANIMATION) << "\n" ;
//    output << "\tAMBIENTSOUNDS = " << WorldAmbientSounds() << "\n" ;
    output << "\tAMBIENTFOOTSTEPS = " << this->enabled(ServerSwitch::AMBIENTFOOTSTEPS) << "\n" ;
    output << "\tINTERNALACCOUNTCREATION = " << this->enabled(ServerSwitch::AUTOACCOUNT) << "\n" ;
    output << "\tSHOWOFFLINEPCS = " << this->enabled(ServerSwitch::SHOWOFFLINEPCS) << "\n" ;
    output << "\tROGUESENABLED = " << this->enabled(ServerSwitch::ROGUEENABLE) << "\n" ;
    output << "\tPLAYERPERSECUTION = " << this->enabled(ServerSwitch::PLAYERPERSECUTION) << "\n" ;
//    output << "\tACCOUNTFLUSH = " << AccountFlushTimer() << "\n" ;
    output << "\tHTMLSTATUSENABLED = " << this->enabled(ServerSwitch::HTMLSTAT) << "\n" ;
    output << "\tSELLBYNAME = " << this->enabled(ServerSwitch::SELLBYNAME) << "\n" ;
//    output << "\tSELLMAXITEMS = " << SellMaxItemsStatus() << "\n" ;
//    output << "\tGLOBALRESTOCKMULTIPLIER = " << GlobalRestockMultiplier() << "\n" ;
//    output << "\tBANKBUYTHRESHOLD = " << BuyThreshold() << "\n" ;
    output << "\tTRADESYSTEM = " << this->enabled(ServerSwitch::TRADESYSTEM) << "\n" ;
    output << "\tRANKSYSTEM = " << this->enabled(ServerSwitch::RANKSYSTEM) << "\n" ;
    output << "\tDISPLAYMAKERSMARK = " << this->enabled(ServerSwitch::MAKERMARK) << "\n" ;
    output << "\tCUTSCROLLREQUIREMENTS = " << this->enabled(ServerSwitch::CUTSCROLLREQ) << "\n" ;
    output << "\tNPCTRAININGENABLED = " << this->enabled(ServerSwitch::NPCTRAINING) << "\n" ;
    output << "\tHIDEWHILEMOUNTED = " << this->enabled(ServerSwitch::HIDEWHILEMOUNTED) << "\n" ;
    output << "\tWEIGHTPERSTR = " <<  realNumbers[RealNumberConfig::WEIGHTSTR] << "\n" ;
//    output << "\tPOLYDURATION = " << SystemTimer(tSERVER_POLYMORPH) << "\n" ;
    output << "\tCLIENTFEATURES = " << this->clientFeature.value() << "\n" ;
    output << "\tSERVERFEATURES = " << this->serverFeature.value()  << "\n" ;
//    output << "\tSPAWNREGIONSFACETS = " << getSpawnRegionsFacetStatus() << "\n" ;
    output << "\tOVERLOADPACKETS = " << this->enabled(ServerSwitch::OVERLOADPACKETS) << "\n" ;
    output << "\tADVANCEDPATHFINDING = " << this->enabled(ServerSwitch::ADVANCEDPATHFINDING) << "\n" ;
    output << "\tLOOTINGISCRIME = " << this->enabled(ServerSwitch::LOOTINGISCRIME) << "\n" ;
    output << "\tBASICTOOLTIPSONLY = " << this->enabled(ServerSwitch::BASICTOOLTIPSONLY) << "\n" ;
    output << "\tSHOWREPUTATIONTITLEINTOOLTIP = " << this->enabled(ServerSwitch::DISPLAYREPUTATIONTOOLTIP) << "\n" ;
    output << "\tSHOWGUILDINFOINTOOLTIP = " << this->enabled(ServerSwitch::DISPLAYGUILDTOOLTIP) << "\n" ;
    output << "\tSHOWNPCTITLESINTOOLTIPS = " << this->enabled(ServerSwitch::NPCTOOLTIPS) << "\n" ;
    output << "\tSHOWNPCTITLESOVERHEAD = " << this->enabled(ServerSwitch::OVERHEADTITLE) << "\n" ;
    output << "\tSHOWINVULNERABLETAGOVERHEAD = " << this->enabled(ServerSwitch::DISPLAYINVUNERABLE) << "\n" ;
    output << "\tSHOWRACEWITHNAME = " << this->enabled(ServerSwitch::DISPLAYRACE) << "\n" ;
    output << "\tSHOWRACEINPAPERDOLL = " << this->enabled(ServerSwitch::DISPLAYRACEPAPERDOLL) << "\n" ;
    output << "\tGLOBALITEMDECAY = " << this->enabled(ServerSwitch::ITEMDECAY) << "\n" ;
    output << "\tSCRIPTITEMSDECAYABLE = " << this->enabled(ServerSwitch::SCRIPTITEMSDECAYABLE) << "\n" ;
    output << "\tBASEITEMSDECAYABLE = " << this->enabled(ServerSwitch::BASEITEMSDECAYABLE) << "\n" ;
    output << "\tPAPERDOLLGUILDBUTTON = " << this->enabled(ServerSwitch::DISPLAYGUILDBUTTON) << "\n" ;
//    output << "\tFISHINGSTAMINALOSS = " << FishingStaminaLoss() << "\n" ;
    output << "\tITEMSDETECTSPEECH = " << this->enabled(ServerSwitch::ITEMDETECTSPEECH) << "\n" ;
//    output << "\tMAXPLAYERPACKITEMS = " << MaxPlayerPackItems() << "\n" ;
//    output << "\tMAXPLAYERBANKITEMS = " << MaxPlayerBankItems() << "\n" ;
//    output << "\tMAXPLAYERPACKWEIGHT = " << MaxPlayerPackWeight() << "\n" ;
//    output << "\tMAXPLAYERBANKWEIGHT = " << MaxPlayerBankWeight() << "\n" ;
    output << "\tFORCENEWANIMATIONPACKET = " << this->enabled(ServerSwitch::FORECENEWANIMATIONPACKET) << "\n" ;
    output << "\tMAPDIFFSENABLED = " << this->enabled(ServerSwitch::MAPDIFF) << "\n" ;
    output << "\tTOOLUSELIMIT = " << this->enabled(ServerSwitch::TOOLUSE) << "\n" ;
    output << "\tTOOLUSEBREAK = " << this->enabled(ServerSwitch::TOOLBREAK) << "\n" ;
    output << "\tITEMREPAIRDURABILITYLOSS = " << this->enabled(ServerSwitch::REPAIRLOSS) << "\n" ;
    output << "\tCRAFTCOLOUREDWEAPONS = " << this->enabled(ServerSwitch::COLORWEAPON) << "\n" ;
//    output << "\tMAXSAFETELEPORTSPERDAY = " << static_cast<std::uint16_t>(MaxSafeTeleportsPerDay()) << "\n" ;
    output << "\tTELEPORTTONEARESTSAFELOCATION = " << this->enabled(ServerSwitch::SAFETELEPORT) << "\n" ;
    output << "\tALLOWAWAKENPCS = " << this->enabled(ServerSwitch::AWAKENPC) << "\n" ;
    output << "\tENABLENPCGUILDDISCOUNTS = " << this->enabled(ServerSwitch::GUILDDISCOUNT) << "\n" ;
    output << "\tENABLENPCGUILDPREMIUMS = " << this->enabled(ServerSwitch::GUILDPREMIUM) << "\n" ;
    output << "\tYOUNGPLAYERSYSTEM = " << this->enabled(ServerSwitch::YOUNGPLAYER) << "\n" ;
    
    output << "}\n\n" ;

    output << "[pets and followers]\n{\n";
//    output << "\tMAXCONTROLSLOTS = " << static_cast<std::uint16_t>(MaxControlSlots()) << "\n" ;
//    output << "\tMAXFOLLOWERS = " << static_cast<std::uint16_t>(MaxFollowers()) << "\n" ;
//    output << "\tMAXPETOWNERS = " << static_cast<std::uint16_t>(MaxPetOwners()) << "\n" ;
    output << "\tCHECKPETCONTROLDIFFICULTY = " << this->enabled(ServerSwitch::PETDIFFICULTY) << "\n" ;
//    output << "\tPETLOYALTYGAINONSUCCESS = " << static_cast<std::uint16_t>(GetPetLoyaltyGainOnSuccess()) << "\n" ;
//    output << "\tPETLOYALTYLOSSONFAILURE = " << static_cast<std::uint16_t>(GetPetLoyaltyLossOnFailure()) << "\n" ;
//    output << "\tPETLOYALTYRATE =" << SystemTimer(tSERVER_LOYALTYRATE) << "\n" ;
    
    output << "}\n\n" ;

    output << "[speedup]\n{\n" ;
    output << "\tCHECKITEMS = " << realNumbers[RealNumberConfig::CHECKITEMS] << "\n" ;
    output << "\tCHECKBOATS = " << realNumbers[RealNumberConfig::CHECKBOATS] << "\n" ;
    output << "\tCHECKNPCAI = " << realNumbers[RealNumberConfig::CHECKAI] << "\n" ;
    output << "\tCHECKSPAWNREGIONS = " << realNumbers[RealNumberConfig::CHECKSPAWN] << "\n" ;
    output << "\tNPCMOVEMENTSPEED = " << realNumbers[RealNumberConfig::NPCMOVEMENT] << "\n" ;
    output << "\tNPCRUNNINGSPEED = " << realNumbers[RealNumberConfig::NPCRUNNING] << "\n" ;
    output << "\tNPCFLEEINGSPEED = " << realNumbers[RealNumberConfig::NPCFLEEING] << "\n" ;
    output << "\tNPCMOUNTEDWALKINGSPEED = " << realNumbers[RealNumberConfig::NPCMOUNTMOVEMENT] << "\n" ;
    output << "\tNPCMOUNTEDRUNNINGSPEED = " << realNumbers[RealNumberConfig::NPCMOUNTRUNNING] << "\n" ;
    output << "\tNPCMOUNTEDFLEEINGSPEED = " << realNumbers[RealNumberConfig::NPCMOUNTFLEEING] << "\n" ;
    output << "\tNPCSPELLCASTSPEED = " << realNumbers[RealNumberConfig::NPCSPELLCAST] << "\n" ;
    output << "\tGLOBALATTACKSPEED = " << realNumbers[RealNumberConfig::GLOBALATTACK] << "\n" ;
    
    output << "}\n\n" ;

    output << "[message boards]\n{\n" ;
//    output << "\tPOSTINGLEVEL = " << static_cast<std::uint16_t>(MsgBoardPostingLevel()) << "\n" ;
//    output << "\tREMOVALLEVEL = " << static_cast<std::uint16_t>(MsgBoardPostRemovalLevel()) << "\n" ;
    
    output << "}\n\n" ;

    output << "[escorts]\n{\n" ;
    output << "\tESCORTENABLED = " << this->enabled(ServerSwitch::ESCORTS) << "\n" ;
//    output << "\tESCORTINITEXPIRE = " << SystemTimer(tSERVER_ESCORTWAIT) << "\n" ;
//    output << "\tESCORTACTIVEEXPIRE = " << SystemTimer(tSERVER_ESCORTACTIVE) << "\n" ;
//    output << "\tESCORTDONEEXPIRE = " << SystemTimer(tSERVER_ESCORTDONE) << "\n" ;
    
    output << "}\n\n" ;

    output << "[worldlight]\n{\n" ;
//    output << "\tDUNGEONLEVEL = " << static_cast<std::uint16_t>(dungeonLightLevel()) << "\n" ;
//    output << "\tBRIGHTLEVEL = " << static_cast<std::uint16_t>(worldLightBrightLevel()) << "\n" ;
//    output << "\tDARKLEVEL = " << static_cast<std::uint16_t>(worldLightDarkLevel()) << "\n" ;
//    output << "\tSECONDSPERUOMINUTE = " << ServerSecondsPerUOMinute() << "\n" ;
    
    output << "}\n\n" ;

    output << "[tracking]\n{\n" ;
//    output << "\tBASERANGE = " << TrackingBaseRange() << "\n" ;
//    output << "\tBASETIMER = " << TrackingBaseTimer() << "\n" ;
//    output << "\tMAXTARGETS = " << static_cast<std::uint16_t>(TrackingMaxTargets()) << "\n" ;
//    output << "\tMSGREDISPLAYTIME = " << TrackingRedisplayTime() << "\n" ;
    
    output << "}\n\n" ;

    output << "[reputation]\n{\n" ;
//    output << "\tMURDERDECAYTIMER = " << SystemTimer(tSERVER_MURDERDECAY) << "\n" ;
//    output << "\tMAXKILLS = " << RepMaxKills() << "\n" ;
//    output << "\tCRIMINALTIMER = " << SystemTimer(tSERVER_CRIMINAL) << "\n" ;
//    output << "\tAGGRESSORFLAGTIMER = " << SystemTimer(tSERVER_AGGRESSORFLAG) << "\n" ;
//    output << "\tPERMAGREYFLAGTIMER = " << SystemTimer(tSERVER_PERMAGREYFLAG) << "\n" ;
//    output << "\tSTEALINGFLAGTIMER = " << SystemTimer(tSERVER_STEALINGFLAG) << "\n" ;
    
    output << "}\n\n" ;

    output << "[resources]\n{\n" ;
//    output << "\tRESOURCEAREASIZE = " << ResourceAreaSize() << "\n" ;
//    output << "\tMINECHECK = " << static_cast<std::uint16_t>(MineCheck()) << "\n" ;
//    output << "\tOREPERAREA = " << ResOre() << "\n" ;
 //   output << "\tORERESPAWNTIMER = " << ResOreTime() << "\n" ;
//    output << "\tLOGSPERAREA = " << ResLogs() << "\n" ;
//    output << "\tLOGSRESPAWNTIMER = " << ResLogTime() << "\n" ;
//    output << "\tFISHPERAREA = " << ResFish() << "\n" ;
//    output << "\tFISHRESPAWNTIMER = " << ResFishTime() << "\n" ;
    
    output << "}\n\n" ;

    output << "[hunger]\n{\n" ;
//    output << "\tHUNGERENABLED = " << this->enabled(ServerSwitch::HUNGER) << "\n" ;
//    output << "\tHUNGERRATE = " << SystemTimer(tSERVER_HUNGERRATE) << "\n" ;
//    output << "\tHUNGERDMGVAL = " << HungerDamage() << "\n" ;
    output << "\tPETHUNGEROFFLINE = " << this->enabled(ServerSwitch::PETHUNGEROFFLINE) << "\n" ;
//    output << "\tPETOFFLINETIMEOUT = " << PetOfflineTimeout() << "\n" ;
    
    output << "}\n\n" ;

    output << "[thirst]\n{\n" ;
    output << "\tTHIRSTENABLED = " << this->enabled(ServerSwitch::THIRST) << "\n" ;
 //   output << "\tTHIRSTRATE = " << SystemTimer(tSERVER_THIRSTRATE) << "\n" ;
 //   output << "\tTHIRSTDRAINVAL = " << ThirstDrain() << "\n" ;
    output << "\tPETTHIRSTOFFLINE = " << this->enabled(ServerSwitch::PETTHIRSTOFFLINE) << "\n" ;
    
    output << "}\n\n" ;

    output << "[combat]\n{\n" ;
//    output << "\tMAXRANGE = " << CombatMaxRange() << "\n" ;
//    output << "\tSPELLMAXRANGE = " << CombatMaxSpellRange() << "\n" ;
    output << "\tDISPLAYHITMSG = " << this->enabled(ServerSwitch::DISPLAYHITMSG) << "\n" ;
    output << "\tDISPLAYDAMAGENUMBERS = " << this->enabled(ServerSwitch::DISPLAYDAMAGENUMBERS) << "\n" ;
    output << "\tMONSTERSVSANIMALS = " << this->enabled(ServerSwitch::MONSTERSVSANIMALS) << "\n" ;
//    output << "\tANIMALATTACKCHANCE = " << static_cast<std::uint16_t>(CombatAnimalsAttackChance()) << "\n" ;
    output << "\tANIMALSGUARDED = " << this->enabled(ServerSwitch::ANIMALSGUARDED) << "\n" ;
 //   output << "\tNPCDAMAGERATE = " << CombatNpcDamageRate() << "\n" ;
 //   output << "\tNPCBASEFLEEAT = " << CombatNPCBaseFleeAt() << "\n" ;
//    output << "\tNPCBASEREATTACKAT = " << CombatNPCBaseReattackAt() << "\n" ;
//    output << "\tATTACKSTAMINA = " << CombatAttackStamina() << "\n" ;
    output << "\tATTACKSPEEDFROMSTAMINA = " << this->enabled(ServerSwitch::ATTACKSPEEDFROMSTAMINA) << "\n" ;
//    output << "\tARCHERYHITBONUS = " << static_cast<std::int16_t>(CombatArcheryHitBonus()) << "\n" ;
    output << "\tARCHERYSHOOTDELAY = " << realNumbers[RealNumberConfig::ARCHERYDELAY] << "\n" ;
    output << "\tSHOOTONANIMALBACK = " << this->enabled(ServerSwitch::SHOOTONANIMALBACK) << "\n" ;
//    output << "\tWEAPONDAMAGECHANCE = " << static_cast<std::uint16_t>(CombatWeaponDamageChance()) << "\n" ;
//    output << "\tWEAPONDAMAGEMIN = " << static_cast<std::uint16_t>(CombatWeaponDamageMin()) << "\n" ;
//    output << "\tWEAPONDAMAGEMAX = " << static_cast<std::uint16_t>(CombatWeaponDamageMax()) << "\n" ;
//    output << "\tARMORDAMAGECHANCE = " << static_cast<std::uint16_t>(CombatArmorDamageChance()) << "\n" ;
//    output << "\tARMORDAMAGEMIN = " << static_cast<std::uint16_t>(CombatArmorDamageMin()) << "\n" ;
//    output << "\tARMORDAMAGEMAX=" << static_cast<std::uint16_t>(CombatArmorDamageMax()) << "\n" ;
//    output << "\tPARRYDAMAGECHANCE = " << static_cast<std::uint16_t>(CombatParryDamageChance()) << "\n" ;
//    output << "\tPARRYDAMAGEMIN = " << static_cast<std::uint16_t>(CombatParryDamageMin()) << "\n" ;
 //   output << "\tPARRYDAMAGEMAX = " << static_cast<std::uint16_t>(CombatParryDamageMax()) << "\n" ;
    output << "\tARMORCLASSDAMAGEBONUS = " << this->enabled(ServerSwitch::ARMORCLASSBONUS) << "\n" ;
    output << "\tALCHEMYBONUSENABLED = " << this->enabled(ServerSwitch::ALCHEMYBONUS) << "\n" ;
//    output << "\tALCHEMYBONUSMODIFIER = " << static_cast<std::uint16_t>(AlchemyDamageBonusModifier()) << "\n" ;
//    output << "\tBLOODEFFECTCHANCE = " << static_cast<std::uint16_t>(CombatBloodEffectChance()) << "\n" ;
    output << "\tITEMSINTERRUPTCASTING = " << this->enabled(ServerSwitch::INTERRUPTCASTING) << "\n" ;
    output << "\tPETCOMBATTRAINING = " << this->enabled(ServerSwitch::PETCOMBATTRAINING) << "\n" ;
    output << "\tHIRELINGCOMBATTRAINING = " << this->enabled(ServerSwitch::HIRELINGTRAINING) << "\n" ;
    output << "\tNPCCOMBATTRAINING = "  << this->enabled(ServerSwitch::NPCCOMBAT) << "\n" ;
    output << "\tSHOWITEMRESISTSTATS = " << this->enabled(ServerSwitch::DISPLAYRESISTSTATS) << "\n" ;
    output << "\tSHOWWEAPONDAMAGETYPES = " << this->enabled(ServerSwitch::DISPLAYDAMAGETYPE) << "\n" ;
//    output << "\tWEAPONDAMAGEBONUSTYPE = " << static_cast<std::uint16_t>(WeaponDamageBonusType()) << "\n" ;
    
    output << "}\n\n" ;

    output << "[magic]\n{\n" ;
    output << "\tTRAVELSPELLSFROMBOATKEYS = " << this->enabled(ServerSwitch::TRAVELBOATKEY) << "\n" ;
    output << "\tTRAVELSPELLSWHILEOVERWEIGHT = " << this->enabled(ServerSwitch::TRAVELBURDEN) << "\n" ;
    output << "\tMARKRUNESINMULTIS = " << this->enabled(ServerSwitch::RUNEMULTI) << "\n" ;
    output << "\tTRAVELSPELLSBETWEENWORLDS = " << this->enabled(ServerSwitch::SPELLWORLDTRAVEL) << "\n" ;
    output << "\tTRAVELSPELLSWHILEAGGRESSOR = " << this->enabled(ServerSwitch::TRAVELAGRESSOR) << "\n" ;
    output << "\tHIDESTATSFORUNKNOWNMAGICITEMS = " << this->enabled(ServerSwitch::MAGICSTATS) << "\n" ;
    output << "\tCASTSPELLSWHILEMOVING = " << this->enabled(ServerSwitch::SPELLMOVING) << "\n" ;
    
    output << "}\n\n" ;

    output << "[start locations]\n{\n" ;
    output<< this->startLocation.describe() << "\n" ;
    
    output << "}\n\n" ;

    output << "[young start locations]\n{\n" ;
    output<< this->youngLocation.describe() << "\n" ;

    
    output << "}\n\n" ;

    output << "[startup]\n{\n" ;
//    output << "\tSTARTGOLD = " << ServerStartGold() << "\n" ;
//    output << "\tSTARTPRIVS = " << serverStartPrivs() << "\n" ;
    
    output << "}\n\n" ;

    output << "[gumps]\n{\n" ;
//    output << "\tTITLECOLOUR = " << TitleColour() << "\n" ;
//    output << "\tLEFTTEXTCOLOUR = " << LeftTextColour() << "\n" ;
//    output << "\tRIGHTTEXTCOLOUR = " << RightTextColour() << "\n" ;
//    output << "\tBUTTONCANCEL = " << ButtonCancel() << "\n" ;
//    output << "\tBUTTONLEFT = " << ButtonLeft() << "\n" ;
//    output << "\tBUTTONRIGHT = " << ButtonRight() << "\n" ;
//    output << "\tBACKGROUNDPIC = " << BackgroundPic() << "\n" ;
    
    output << "}\n\n" ;

    output << "[houses]\n{\n" ;
    output << "\tTRACKHOUSESPERACCOUNT = " << this->enabled(ServerSwitch::TRACKHOUSESPERACCOUNT) << "\n" ;
    output << "\tCANOWNANDCOOWNHOUSES = " << this->enabled(ServerSwitch::OWNCOOWNHOUSE) << "\n" ;
    output << "\tCOOWNHOUSESONSAMEACCOUNT ="  << this->enabled(ServerSwitch::COWNHOUSEACCOUNT) << "\n" ;
    output << "\tITEMDECAYINHOUSES = " << this->enabled(ServerSwitch::INHOUSEDECAY) << "\n" ;
    output << "\tPROTECTPRIVATEHOUSES = " << this->enabled(ServerSwitch::PROTECTPRIVATEHOUSES) << "\n" ;
//    output << "\tMAXHOUSESOWNABLE = " << MaxHousesOwnable() << "\n" ;
//    output << "\tMAXHOUSESCOOWNABLE = " << MaxHousesCoOwnable() << "\n" ;
    output << "\tSAFECOOWNERLOGOUT = " << this->enabled(ServerSwitch::COOWNERLOGOUT) << "\n" ;
    output << "\tSAFEFRIENDLOGOUT = " << this->enabled(ServerSwitch::FRIENDLOGOUT) << "\n" ;
    output << "\tSAFEGUESTLOGOUT = " << this->enabled(ServerSwitch::GUESTLOGOUT) << "\n" ;
    output << "\tKEYLESSOWNERACCESS = " << this->enabled(ServerSwitch::KEYLESSOWNER) << "\n" ;
    output << "\tKEYLESSCOOWNERACCESS = " << this->enabled(ServerSwitch::KEYLESSCOOWNER) << "\n" ;
    output << "\tKEYLESSFRIENDACCESS = " << this->enabled(ServerSwitch::KEYLESSFRIEND) << "\n" ;
    output << "\tKEYLESSGUESTACCESS = " << this->enabled(ServerSwitch::KEYLESSGUEST) << "\n" ;
    
    output << "}\n\n" ;

    output << "[bulk order deeds]\n{\n" ;
    output << "\tOFFERBODSFROMITEMSALES = " << this->enabled(ServerSwitch::OFFERBODSFROMITEMSALES) << "\n" ;
    output << "\tOFFERBODSFROMCONTEXTMENU = " << this->enabled(ServerSwitch::OFFERBODSFROMCONTEXTMENU) << "\n" ;
    output << "\tBODSFROMCRAFTEDITEMSONLY = " << this->enabled(ServerSwitch::BODSFROMCRAFTEDITEMSONLY) << "\n" ;
//    output << "\tBODGOLDREWARDMULTIPLIER = " << BODGoldRewardMultiplier() << "\n" ;
//    output << "\tBODFAMEREWARDMULTIPLIER = " << BODFameRewardMultiplier() << "\n" ;
    
    output << "}\n\n" ;

    output << "[towns]\n{\n" ;
//    output << "\tPOLLTIME = " << TownNumSecsPollOpen() << "\n" ;
//    output << "\tMAYORTIME = " << TownNumSecsAsMayor() << "\n" ;
//    output << "\tTAXPERIOD = " << TownTaxPeriod() << "\n" ;
//    output << "\tGUARDSPAID = " << TownGuardPayment() << "\n" ;
    
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
    auto bvalue = serverSwitch.valueFor(ukey);
    if (bvalue.has_value()){
        data.type = AllDataType::T_BOOL ;
        data.value = bvalue.value() ;
        return data ;
    }
    bvalue = enableClients.valueFor(ukey);
    if (bvalue.has_value()){
        data.type = AllDataType::T_BOOL ;
        data.value = bvalue.value() ;
        return data ;
    }
    bvalue = assistantFeature.valueFor(ukey);
    if (bvalue.has_value()){
        data.type = AllDataType::T_BOOL ;
        data.value = bvalue.value() ;
        return data ;
    }
    auto svalue = serverString.valueFor(ukey) ;
    if (svalue.has_value()){
        data.type = AllDataType::T_STRING ;
        data.value = svalue.value() ;
        return data ;
    }
    svalue = directoryLocation.valueFor(ukey) ;
    if (svalue.has_value()){
        data.type = AllDataType::T_STRING ;
        data.value = svalue.value() ;
        return data ;
    }
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
    return {};
}
