//

#include "serverswitch.hpp"

#include <algorithm>
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
const std::map<std::string, ServerSwitch::switch_t> ServerSwitch::NAMESWITCHMAP{
    {"UOGENABLED"s,UOG},                                    {"FREESHARDSERVERPOLL"s,FREESHARD},
    {"CONSOLELOG"s,CONSOLELOG},                             {"NETWORKLOG"s,NETWORKLOG},
    {"SPEECHLOG"s,SPEECHLOG},                               {"ANNOUNCEWORLDSAVES"s,ANNOUNCESAVE},
    {"JOINPARTMSGS"s,ANNOUNCEJOINPART},                     {"BACKUPSENABLED"s,BACKUP},
    {"SHOOTONANIMALBACK"s,SHOOTONANIMALBACK},               {"NPCTRAININGENABLED"s,NPCTRAINING},
    {"LOOTDECAYSWITHCORPSE"s,CORPSELOOTDECAY},              {"GUARDSACTIVE"s,GUARDSACTIVE},
    {"DEATHANIMATION"s,DEATHANIMATION},                     {"AMBIENTFOOTSTEPS"s,AMBIENTFOOTSTEPS},
    {"INTERNALACCOUNTCREATION"s,AUTOACCOUNT},               {"SHOWOFFLINEPCS"s,SHOWOFFLINEPCS},
    {"ROGUESENABLED"s,ROGUEENABLE},                         {"SNOOPISCRIME"s,SNOOPISCRIME},
    {"PLAYERPERSECUTION"s,PLAYERPERSECUTION},               {"SELLBYNAME"s,SELLBYNAME},
    {"TRADESYSTEM"s,TRADESYSTEM},                           {"RANKSYSTEM"s,RANKSYSTEM},
    {"CUTSCROLLREQUIREMENTS"s,CUTSCROLLREQ},                {"DISPLAYHITMSG"s,DISPLAYHITMSG},
    {"ESCORTENABLED"s,ESCORTS},                             {"MONSTERSVSANIMALS"s,MONSTERSVSANIMALS},
    {"PETHUNGEROFFLINE"s,PETHUNGEROFFLINE},                 {"HIDEWHILEMOUNTED"s,HIDEWHILEMOUNTED},
    {"OVERLOADPACKETS"s,OVERLOADPACKETS},                   {"ARMORAFFECTMANAREGEN"s,ARMORIMPACTSMANA},
    {"ANIMALSGUARDED"s,ANIMALSGUARDED},                     {"ADVANCEDPATHFINDING"s,ADVANCEDPATHFINDING},
    {"LOOTINGISCRIME"s,LOOTINGISCRIME},                     {"BASICTOOLTIPSONLY"s,BASICTOOLTIPSONLY},
    {"GLOBALITEMDECAY"s,ITEMDECAY},                         {"SCRIPTITEMSDECAYABLE"s,SCRIPTITEMSDECAYABLE},
    {"BASEITEMSDECAYABLE"s,BASEITEMSDECAYABLE},             {"ITEMDECAYINHOUSES"s,INHOUSEDECAY},
    {"PAPERDOLLGUILDBUTTON"s,DISPLAYGUILDBUTTON},           {"ATTACKSPEEDFROMSTAMINA"s,ATTACKSPEEDFROMSTAMINA},
    {"DISPLAYDAMAGENUMBERS"s,DISPLAYDAMAGENUMBERS},         {"EXTENDEDSTARTINGSTATS"s,EXTENDEDSTATS},
    {"EXTENDEDSTARTINGSKILLS"s,EXTENDEDSKILLS},             {"ASSISTANTNEGOTIATION"s,ASSISTANTNEGOTIATION},
    {"KICKONASSISTANTSILENCE"s,KICKONASSISTANTSILENCE},     {"CLASSICUOMAPTRACKER",CUOMAPTRACKER},
    {"PROTECTPRIVATEHOUSES"s,PROTECTPRIVATEHOUSES},         {"TRACKHOUSESPERACCOUNT"s,TRACKHOUSESPERACCOUNT},
    {"CANOWNANDCOOWNHOUSES"s,OWNCOOWNHOUSE},                {"COOWNHOUSESONSAMEACCOUNT"s,COWNHOUSEACCOUNT},
    {"ITEMSDETECTSPEECH"s,ITEMDETECTSPEECH},                {"FORCENEWANIMATIONPACKET"s,FORECENEWANIMATIONPACKET},
    {"MAPDIFFSENABLED"s,MAPDIFF},                           {"ARMORCLASSDAMAGEBONUS"s,ARMORCLASSBONUS},
    {"ALCHEMYBONUSENABLED",ALCHEMYBONUS},                   {"PETTHIRSTOFFLINE"s,PETTHIRSTOFFLINE},
    {"USEUNICODEMESSAGES"s,UNICODEMESSAGE},                 {"HUNGERENABLED"s,HUNGER},
    {"THIRSTENABLED"s,THIRST},                              {"TRAVELSPELLSFROMBOATKEYS"s,TRAVELBOATKEY},
    {"TRAVELSPELLSWHILEOVERWEIGHT"s,TRAVELBURDEN},          {"MARKRUNESINMULTIS"s,RUNEMULTI},
    {"TRAVELSPELLSBETWEENWORLDS"s,SPELLWORLDTRAVEL},         {"TRAVELSPELLSWHILEAGGRESSOR"s,TRAVELAGRESSOR},
    {"CONTEXTMENUS"s,CONTEXTMENU},                          {"CHECKPETCONTROLDIFFICULTY"s,PETDIFFICULTY},
    {"SHOWNPCTITLESINTOOLTIPS",NPCTOOLTIPS},                {"ITEMSINTERRUPTCASTING"s,INTERRUPTCASTING},
    {"STATSAFFECTSKILLCHECKS"s,STATIMPACTSKILL},            {"TOOLUSELIMIT"s,TOOLUSE},
    {"TOOLUSEBREAK"s,TOOLBREAK},                            {"ITEMREPAIRDURABILITYLOSS"s,REPAIRLOSS},
    {"HIDESTATSFORUNKNOWNMAGICITEMS"s,MAGICSTATS},          {"CRAFTCOLOUREDWEAPONS"s,COLORWEAPON},
    {"TELEPORTTONEARESTSAFELOCATION"s,SAFETELEPORT},        {"ALLOWAWAKENPCS"s,AWAKENPC},
    {"DISPLAYMAKERSMARK"s,MAKERMARK},                       {"SHOWNPCTITLESOVERHEAD"s,OVERHEADTITLE},
    {"SHOWINVULNERABLETAGOVERHEAD"s,DISPLAYINVUNERABLE},    {"PETCOMBATTRAINING"s,PETCOMBATTRAINING},
    {"HIRELINGCOMBATTRAINING"s,HIRELINGTRAINING},           {"NPCCOMBATTRAINING"s,NPCCOMBAT},
    {"SHOWITEMRESISTSTATS"s,DISPLAYRESISTSTATS},            {"SHOWWEAPONDAMAGETYPES"s,DISPLAYDAMAGETYPE},
    {"SHOWRACEWITHNAME"s,DISPLAYRACE},                      {"SHOWRACEINPAPERDOLL"s,DISPLAYRACEPAPERDOLL},
    {"CASTSPELLSWHILEMOVING"s,SPELLMOVING},                 {"SHOWREPUTATIONTITLEINTOOLTIP"s,DISPLAYREPUTATIONTOOLTIP},
    {"SHOWGUILDINFOINTOOLTIP"s,DISPLAYGUILDTOOLTIP},        {"SAFECOOWNERLOGOUT",COOWNERLOGOUT},
    {"SAFEFRIENDLOGOUT",FRIENDLOGOUT},                      {"SAFEGUESTLOGOUT"s,GUESTLOGOUT},
    {"KEYLESSGUESTACCESS"s,KEYLESSGUEST},                   {"KEYLESSOWNERACCESS"s,KEYLESSOWNER},
    {"KEYLESSCOOWNERACCESS"s,KEYLESSCOOWNER},               {"KEYLESSFRIENDACCESS"s,KEYLESSFRIEND},
    {"OFFERBODSFROMITEMSALES"s,OFFERBODSFROMITEMSALES},     {"OFFERBODSFROMCONTEXTMENU"s,OFFERBODSFROMCONTEXTMENU},
    {"BODSFROMCRAFTEDITEMSONLY"s,BODSFROMCRAFTEDITEMSONLY}, {"ENABLENPCGUILDDISCOUNTS"s,GUILDDISCOUNT},
    {"ENABLENPCGUILDPREMIUMS"s,GUILDPREMIUM},               {"SNOOPAWARENESS",SNOOPAWARE},
    {"YOUNGPLAYERSYSTEM"s,YOUNGPLAYER},                     {"RANDOMSTARTINGLOCATION"s,RANDOMSTART},
    {"HTMLSTATUSENABLED"s,HTMLSTAT}
    
};
//======================================================================
auto ServerSwitch::nameFor(switch_t setting)-> const std::string &{
    auto iter = std::find_if(NAMESWITCHMAP.begin(),NAMESWITCHMAP.end(),[setting](const std::pair<std::string,switch_t> &entry){
        return setting == entry.second;
    });
    if (iter == NAMESWITCHMAP.end()){
        throw std::runtime_error("No name was found for switch_t setting: "s+std::to_string(static_cast<int>(setting)));
    }
    return iter->first ;
}
//======================================================================
ServerSwitch::ServerSwitch() {
    this->reset();
}
//======================================================================
auto ServerSwitch::reset() ->void {
    enabledSwitch = std::vector<bool>(NAMESWITCHMAP.size(),false);
    this->setSetting(UNICODEMESSAGE,true);
    this->setSetting(YOUNGPLAYER,true);
    this->setSetting(CORPSELOOTDECAY,true);
    this->setSetting(STATIMPACTSKILL,true);
    this->setSetting(HUNGER,true);
    this->setSetting(THIRST,false);
    this->setSetting(ARMORIMPACTSMANA,true);
    this->setSetting(SNOOPISCRIME,false);
    this->setSetting(SNOOPAWARE,false);
    this->setSetting(DEATHANIMATION,true);
    this->setSetting(SHOWOFFLINEPCS,true);
    this->setSetting(DISPLAYHITMSG,true);
    this->setSetting(DISPLAYDAMAGENUMBERS,true);
    this->setSetting(ATTACKSPEEDFROMSTAMINA,true);
    this->setSetting(NPCTRAINING,true);
    this->setSetting(HIDEWHILEMOUNTED,true);
    this->setSetting(OVERLOADPACKETS,true);
    this->setSetting(ADVANCEDPATHFINDING,true);
    this->setSetting(LOOTINGISCRIME,true);
    this->setSetting(UOG,true);
    this->setSetting(FREESHARD,true);
    this->setSetting(CONTEXTMENU,true);
    this->setSetting(MONSTERSVSANIMALS,true);
    this->setSetting(ANIMALSGUARDED,false);
    this->setSetting(SHOOTONANIMALBACK,false);
    this->setSetting(SELLBYNAME,false);
    this->setSetting(RANKSYSTEM,true);
    this->setSetting(MAKERMARK,true);
    this->setSetting(ARMORCLASSBONUS,false);
    this->setSetting(ALCHEMYBONUS,false);
    this->setSetting(PETCOMBATTRAINING,true);
    this->setSetting(HIRELINGTRAINING,true);
    this->setSetting(NPCTRAINING,false);
    this->setSetting(PETDIFFICULTY,true);
    this->setSetting(INTERRUPTCASTING,true);
    this->setSetting(GUARDSACTIVE,true);
    this->setSetting(ANNOUNCESAVE,true);
    this->setSetting(ANNOUNCEJOINPART,true);
    this->setSetting(CONSOLELOG,true);
    this->setSetting(NETWORKLOG,false);
    this->setSetting(SPEECHLOG,false);
    this->setSetting(ROGUEENABLE,true);
    this->setSetting(PETHUNGEROFFLINE,true);
    this->setSetting(ITEMDETECTSPEECH,false);
    this->setSetting(FORECENEWANIMATIONPACKET,true);
    this->setSetting(MAPDIFF,false);
    this->setSetting(TRAVELBOATKEY,true);
    this->setSetting(TRAVELBURDEN,false);
    this->setSetting(RUNEMULTI,true);
    this->setSetting(SPELLWORLDTRAVEL,false);
    this->setSetting(TRAVELAGRESSOR,false);
    this->setSetting(SPELLMOVING,false);
    this->setSetting(TOOLUSE,true);
    this->setSetting(TOOLBREAK,true);
    this->setSetting(REPAIRLOSS,true);
    this->setSetting(MAGICSTATS,true);
    this->setSetting(COLORWEAPON,false);
    this->setSetting(SAFETELEPORT,false);
    this->setSetting(AWAKENPC,true);
    this->setSetting(DISPLAYRESISTSTATS,false);
    this->setSetting(DISPLAYDAMAGETYPE,true);
    this->setSetting(DISPLAYREPUTATIONTOOLTIP,true);
    this->setSetting(DISPLAYGUILDTOOLTIP,true);
    this->setSetting(GUILDDISCOUNT,true);
    this->setSetting(GUILDPREMIUM,true);
    this->setSetting(CUTSCROLLREQ,true);
    this->setSetting(PLAYERPERSECUTION,false);
    this->setSetting(ESCORTS,true);
    this->setSetting(BASICTOOLTIPSONLY,false);
    this->setSetting(NPCTOOLTIPS,true);
    this->setSetting(OVERHEADTITLE,true);
    this->setSetting(DISPLAYINVUNERABLE,false);
    this->setSetting(DISPLAYRACE,true);
    this->setSetting(DISPLAYRACEPAPERDOLL,true);
    this->setSetting(ITEMDECAY,true);
    this->setSetting(SCRIPTITEMSDECAYABLE,true);
    this->setSetting(BASEITEMSDECAYABLE,false);
    this->setSetting(AMBIENTFOOTSTEPS,false);
    this->setSetting(INHOUSEDECAY,true);
    this->setSetting(PROTECTPRIVATEHOUSES,true);
    this->setSetting(TRACKHOUSESPERACCOUNT,true);
    this->setSetting(OWNCOOWNHOUSE,true);
    this->setSetting(COWNHOUSEACCOUNT,true);
    this->setSetting(COOWNERLOGOUT,true);
    this->setSetting(FRIENDLOGOUT,true);
    this->setSetting(GUESTLOGOUT,true);
    this->setSetting(KEYLESSOWNER,true);
    this->setSetting(KEYLESSCOOWNER,true);
    this->setSetting(KEYLESSFRIEND,true);
    this->setSetting(KEYLESSGUEST,false);
    this->setSetting(OFFERBODSFROMITEMSALES,true);
    this->setSetting(OFFERBODSFROMCONTEXTMENU,false);
    this->setSetting(BODSFROMCRAFTEDITEMSONLY,false);
    this->setSetting(EXTENDEDSTATS,true);
    this->setSetting(EXTENDEDSKILLS,true);
    this->setSetting(RANDOMSTART,false);
    
}
//======================================================================
auto ServerSwitch::size() const ->size_t {
    return enabledSwitch.size();
}
//======================================================================
auto ServerSwitch::operator[](switch_t setting) const ->bool {
    return enabledSwitch.at(setting) ;
}
//======================================================================
auto ServerSwitch::setSetting(switch_t setting, bool state) ->void {
    enabledSwitch.at(setting) = state ;
}
//======================================================================
auto ServerSwitch::setKeyValue(const std::string &key, const std::string &value) ->bool {
    auto rvalue = false ;
    auto iter = NAMESWITCHMAP.find(key) ;
    if (iter != NAMESWITCHMAP.end()){
        rvalue = true ;
        // We cant just cast, and take anything not 0 as true.  Because HTML status used -1.
        // This should be fixed, but until then, we check for 0/-1 as false, anything else is true
        enabledSwitch.at(iter->second) = (std::stoi(value,nullptr,0)!=0) && (std::stoi(value,nullptr,0)!= -1);
    }
    return rvalue ;
}
//======================================================================
auto ServerSwitch::save(std::ostream &output) const ->void{
    for (const auto &[name,value]:NAMESWITCHMAP){
        output <<"\t"<<name<<" = " << (enabledSwitch.at(value) == true ? 1:0)<<"\n";
    }
}
//======================================================================
auto ServerSwitch::valueFor(const std::string &keyword) const ->std::optional<bool> {
    auto iter  = NAMESWITCHMAP.find(keyword) ;
    if (iter != NAMESWITCHMAP.end()){
        return enabledSwitch.at(iter->second);
    }
    return {} ;
}
