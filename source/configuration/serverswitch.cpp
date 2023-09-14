//

#include "serverswitch.hpp"
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
    {"CANOWNANDCOOWNHOUSES"s,OWNCOOWNHOUSE},                {"COOWNHOUSEONESAMEACCOUNT"s,OWNCOOWNHOUSE},
    {"ITEMSDETECTSPEECH"s,ITEMDETECTSPEECH},                {"FORCENEWANIMATIONPACKET"s,FORECENEWANIMATIONPACKET},
    {"MAPDIFFSENABLED"s,MAPDIFF},                           {"ARMORCLASSDAMAGEBONUS"s,ARMORCLASSBONUS},
    {"ALCHEMYBONUSENABLED",ALCHEMYBONUS},                   {"PETTHIRSTOFFLINE"s,PETTHIRSTOFFLINE},
    {"USEUNICODEMESSAGES"s,UNICODEMESSAGE},                 {"HUNGERENABLED"s,HUNGER},
    {"THIRSTENABLED"s,THIRST},                              {"TRAVELSPELLSFROMBOATKEYS"s,TRAVELBOATKEY},
    {"TRAVELSPELLSWHILEOVERWEIGHT"s,TRAVELBURDEN},          {"MARKRUNESINMULTIS"s,RUNEMULTI},
    {"TRAVELSPELLSBETWEENWORLD"s,SPELLWORLDTRAVEL},         {"TRAVELSPELLSWHILEAGGRESSOR"s,TRAVELAGRESSOR},
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
    {"YOUNGPLAYERSYSTEM"s,YOUNGPLAYER},                     {"RANDOMSTARTINGLOCATION"s,RANDOMSTART}
    
};
//======================================================================
ServerSwitch::ServerSwitch() {
    this->reset();
}
//======================================================================
auto ServerSwitch::reset() ->void {
    enabledSwitch = std::vector<bool>(NAMESWITCHMAP.size(),false);
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
        enabledSwitch.at(iter->second) = static_cast<bool>(std::stoi(value,nullptr,0));
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
