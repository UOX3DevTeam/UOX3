//

#include "serverdata.hpp"

#include <algorithm>
#include <stdexcept>

#include "utility/strutil.hpp"

using namespace std::string_literals ;
//======================================================================
// ClientFeature
//======================================================================
//======================================================================
enum features_t {
    CHAT = 0, // 0x01
    UOR,      // 0x02
    TD,       // 0x04
    LBR,      // 0x08 - Enables LBR features: mp3s instead of midi, show new LBR monsters
    AOS, // 0x10 - Enable AoS monsters/map, AoS skills, Necro/Pala/Fight book stuff - works
    // for 4.0+
    SIXCHARS,   // 0x20 - Enable sixth character slot
    SE,         // 0x40
    ML,         // 0x80 - Elven race, new spells, skills + housing tiles
    EIGHTAGE,   // 0x100 - Splash screen for 8th age
    NINTHAGE,   // 0x200 - Splash screen for 9th age
    TENTHAGE,   // 0x400 - Splash screen for 10th age - crystal/shadow house tiles
    UNKNOWN1,   // 0x800 - Increased housing/bank storage (6.0.3.0 or earlier)
    SEVENCHARS, // 0x1000 - Enable seventh character slot
    // CF_BIT_KRFACES,        // 0x2000 - KR release (6.0.0.0)
    // CF_BIT_TRIAL,            // 0x4000 - Trial account
    EXPANSION = 15, // 0x8000 - Live account
    SA, // 0x10000 - Enable SA features: gargoyle race, spells, skills, housing tiles -
    // clients 6.0.14.2+
    HS, // 0x20000 - Enable HS features: boats, new movementtype? ++
    GOTHHOUSE,    // 0x40000
    RUSTHOUSE,    // 0x80000
    JUNGLEHOUSE,  // 0x100000 - Enable Jungle housing tiles
    SHADOWHOUSE,  // 0x200000 - Enable Shadowguard housing tiles
    OLHOUSE,     // 0x400000 - Enable Time of Legends features
    ENDLESSHOUSE, // 0x800000 - Enable Endless Journey account
};

const std::map<std::string,ClientFeature::features_t> ClientFeature::FEATURENAMEMAP{
    {"CF_BIT_CHAT"s,ClientFeature::CHAT}, {"CF_BIT_UOR"s,ClientFeature::UOR},
    {"CF_BIT_TD"s,ClientFeature::TD}, {"CF_BIT_LBR"s,ClientFeature::LBR},
    {"CF_BIT_AOS"s,ClientFeature::AOS}, {"CF_BIT_SIXCHARS"s,ClientFeature::SIXCHARS},
    {"CF_BIT_SE"s,ClientFeature::SE}, {"CF_BIT_ML"s,ClientFeature::ML},
    {"CF_BIT_EIGHTAGE"s,ClientFeature::EIGHTAGE}, {"CF_BIT_NINTHAGE"s,ClientFeature::NINTHAGE},
    {"CF_BIT_TENTHAGE"s,ClientFeature::TENTHAGE}, {"CF_BIT_UNKNOWN1"s,ClientFeature::UNKNOWN1},
    {"CF_BIT_SEVENCHARS"s,ClientFeature::SEVENCHARS}, {"CF_BIT_KRFACES"s,ClientFeature::KRFACES},
    {"CF_BIT_TRIAL"s,ClientFeature::TRIAL}, {"CF_BIT_EXPANSION"s,ClientFeature::EXPANSION},
    {"CF_BIT_SA"s,ClientFeature::SA}, {"CF_BIT_HS"s,ClientFeature::HS},
    {"CF_BIT_GOTHHOUSE"s,ClientFeature::GOTHHOUSE}, {"CF_BIT_RUSTHOUSE"s,ClientFeature::RUSTHOUSE},
    {"CF_BIT_JUNGLEHOUSE"s,ClientFeature::JUNGLEHOUSE}, {"CF_BIT_SHADOWHOUSE"s,ClientFeature::SHADOWHOUSE},
    {"CF_BIT_TOLHOUSE"s,ClientFeature::OLHOUSE}, {"CF_BIT_ENDLESSHOUSE"s,ClientFeature::ENDLESSHOUSE}
};
//======================================================================
auto ClientFeature::size() ->size_t{
    return FEATURENAMEMAP.size();
}
//======================================================================
auto ClientFeature::value() const -> std::uint32_t {
    return static_cast<std::uint32_t>(databits.to_ulong()) ;
}
//======================================================================
auto ClientFeature::set(features_t feature,bool value) ->void {
     databits.set(feature,value) ;
}
//======================================================================
auto ClientFeature::set(const std::string &feature,bool value) ->bool {
    auto rvalue = false ;
    auto iter = FEATURENAMEMAP.find(feature);
    if (iter != FEATURENAMEMAP.end()){
        databits.set(iter->second,value) ;
        rvalue = true ;
    }
    return rvalue ;
}
//======================================================================
auto ClientFeature::test(features_t feature) const ->bool {
    return databits.test(feature) ;
}
//======================================================================
auto ClientFeature::test(const std::string &feature) const ->bool {
    auto iter = FEATURENAMEMAP.find(feature);
    if (iter != FEATURENAMEMAP.end()){
        return databits.test(iter->second) ;
    }
    throw std::runtime_error("Unable to find client feature: "s+feature);
}

//======================================================================
// ServerFeature
//======================================================================
//======================================================================
const std::map<std::string,ServerFeature::features_t> ServerFeature::FEATURENAMEMAP{
    {"SF_BIT_UNKNOWN1"s,ServerFeature::UNKNOWN1}, {"SF_BIT_IGR"s,ServerFeature::IGR},
    {"SF_BIT_ONECHAR"s,ServerFeature::ONECHAR}, {"SF_BIT_CONTEXTMENUS"s,ServerFeature::CONTEXTMENUS},
    {"SF_BIT_LIMITCHAR"s,ServerFeature::LIMITCHAR}, {"SF_BIT_AOS"s,ServerFeature::AOS},
    {"SF_BIT_SIXCHARS"s,ServerFeature::SIXCHARS}, {"SF_BIT_SE"s,ServerFeature::SE},
    {"SF_BIT_ML"s,ServerFeature::ML}, {"SF_BIT_UNKNOWN2"s,ServerFeature::UNKNOWN2},
    {"SF_BIT_SEND3DTYPE"s,ServerFeature::SEND3DTYPE}, {"SF_BIT_UNKNOWN4"s,ServerFeature::UNKNOWN4},
    {"SF_BIT_SEVENCHARS"s,ServerFeature::SEVENCHARS}, {"SF_BIT_UNKNOWN5"s,ServerFeature::UNKNOWN5},
    {"SF_BIT_NEWMOVE"s,ServerFeature::NEWMOVE}, {"SF_BIT_FACTIONAREAS"s,ServerFeature::FACTIONAREAS}
};
//======================================================================
auto ServerFeature::size() ->size_t{
    return FEATURENAMEMAP.size();
}
//======================================================================
auto ServerFeature::value() const -> std::uint16_t {
    return static_cast<std::uint16_t>(databits.to_ulong()) ;
}
//======================================================================
auto ServerFeature::set(features_t feature,bool value) ->void {
     databits.set(feature,value) ;
}
//======================================================================
auto ServerFeature::set(const std::string &feature,bool value) ->bool {
    auto rvalue = false ;
    auto iter = FEATURENAMEMAP.find(feature);
    if (iter != FEATURENAMEMAP.end()){
        databits.set(iter->second,value) ;
        rvalue = true ;
    }
    return rvalue ;
}
//======================================================================
auto ServerFeature::test(features_t feature) const ->bool {
    return databits.test(feature) ;
}
//======================================================================
auto ServerFeature::test(const std::string &feature) const ->bool {
    auto iter = FEATURENAMEMAP.find(feature);
    if (iter != FEATURENAMEMAP.end()){
        return databits.test(iter->second) ;
    }
    throw std::runtime_error("Unable to find server feature: "s+feature);
}

//======================================================================
// StartLocation
//======================================================================

StartLocation::StartLocation(const std::string &line):StartLocation(){
    auto values = util::parse(line,",") ;
    switch(values.size()){
            
            
        default:
        case 8:
            clilocDesc = static_cast<std::uint32_t>(std::stoul(values[7])) ;
            [[fallthrough]];

        case 7:
            instanceID = static_cast<std::uint16_t>(std::stoul(values[6])) ;
            [[fallthrough]];
       case 6:
            world = static_cast<std::int16_t>(std::stoi(values[5])) ;
            [[fallthrough]];

        case 5:
            z = static_cast<std::int16_t>(std::stoi(values[4])) ;
            [[fallthrough]];

        case 4:
            y = static_cast<std::int16_t>(std::stoi(values[3])) ;
            [[fallthrough]];

        case 3:
            x = static_cast<std::int16_t>(std::stoi(values[2])) ;
            [[fallthrough]];
        case 2:
            oldDescription = values[1] ;
            newDescription = values[1] ;
            [[fallthrough]];

        case 1:
            oldTown = values[0] ;
            newTown = values[0] ;
            [[fallthrough]];
        case 0:
            break;
    }
}


//======================================================================
// ServerData
//======================================================================


//======================================================================
std::map<std::string,std::variant<R32,R64,bool,std::string,std::uint8_t,std::int8_t,std::uint16_t,std::int16_t,std::uint32_t,std::int32_t,std::uint64_t,std::int64_t>> ServerData::dataValue ;

const std::map<ServerData::timer_t,std::string> ServerData::TIMERNAMEMAP{
    {ServerData::ERROR,"ERROR"s},{ServerData::CORPSEDECAY,"CORPSEDECAY"},{ServerData::WEATHER,"WEATHER"s},
    {ServerData::SHOPSPAWN,"SHOPSPAWN"s},{ServerData::DECAY,"DECAY"s},{ServerData::DECAYINHOUSE,"DECAYINHOUSE"s},
    {ServerData::INVISIBLITY,"INVISIBLITY"s},{ServerData::OBJECTUSAGE,"OBJECTUSAGE"s},{ServerData::GATE,"GATE"s},
    {ServerData::POISON,"POISON"s},{ServerData::LOGINTIMEOUT,"LOGINTIMEOUT"s},{ServerData::HITPOINTREGEN,"HITPOINTREGEN"s},
    {ServerData::STAMINAREGEN,"STAMINAREGEN"s},{ServerData::MANAREGEN,"MANAREGEN"s},{ServerData::FISHINGBASE,"FISHINGBASE"s},
    {ServerData::FISHINGRANDOM,"FISHINGRANDOM"s},{ServerData::SPIRITSPEAK,"SPIRITSPEAK"s},{ServerData::HUNGERRATE,"HUNGERRATE"s},
    {ServerData::THIRSTRATE,"THIRSTRATE"s},{ServerData::POLYMORPH,"POLYMORPH"s},{ServerData::ESCORTWAIT,"ESCORTWAIT"s},
    {ServerData::ESCORTACTIVE,"ESCORTACTIVE"s},{ServerData::ESCORTDONE,"ESCORTDONE"s},{ServerData::MURDERDECAY,"MURDERDECAY"s},
    {ServerData::CRIMINAL,"CRIMINAL"s},{ServerData::STEALINGFLAG,"STEALINGFLAG"s},{ServerData::AGGRESSORFLAG,"AGGRESSORFLAG"s},
    {ServerData::PERMAGREYFLAG,"PREMAGREYFLAG"s},{ServerData::COMBATIGNORE,"COMBATIGNORE"s},{ServerData::POTION,"POTION"s},
    {ServerData::PETOFFLINECHECK,"PETOFFLINECHECK"s},{ServerData::NPCFLAGUPDATETIMER,"NPCFLAGUPDATETIMER"s},{ServerData::BLOODDECAY,"BLOODDECAY"s},
    {ServerData::BLOODDECAYCORPSE,"BLOODDECAYCORPSE"s},{ServerData::NPCCORPSEDECAY,"NPCCORPSEDECAY"s},{ServerData::LOYALITYRATE,"LOYALITYRATE"s}
};
//======================================================================
auto ServerData::timerForName(const std::string &name) const -> timer_t {
    auto uname = util::upper(name) ;
    auto iter = std::find_if(TIMERNAMEMAP.begin(),TIMERNAMEMAP.end(),[&uname](const std::pair<timer_t,std::string> &entry){
        return entry.second == uname ;
    });
    if (iter != TIMERNAMEMAP.end()){
        return iter->first ;
    }
    return timer_t::ERROR ;
}
//======================================================================
auto ServerData::shared() ->ServerData&{
    static ServerData instance ;
    return instance ;
}
//======================================================================
ServerData::ServerData() {
    this->reset();
}
//======================================================================
auto ServerData::timerCount() const -> size_t{
    return TIMERNAMEMAP.size() ;
}
//======================================================================
auto ServerData::reset() ->void {
    dataValue.clear();
}
//======================================================================
// Accessors
//======================================================================
auto ServerData::serverName() const -> const std::string & {
    return getValue<std::string>("SERVERNAME"s);
}
//======================================================================
auto ServerData::setServerName(const std::string &name) -> void {
    setValue("SERVERNAME"s, name) ;
}

//======================================================================
auto ServerData::secretShardKey() const -> const std::string & {
    return getValue<std::string>("SECRETSHARDKEY"s);
}
//======================================================================
auto ServerData::setSecretShardKey(const std::string &shardkey) ->void {
    setValue("SECRETSHARDKEY"s, shardkey) ;
}

//======================================================================
auto ServerData::publicIP() const ->const std::string & {
    return getValue<std::string>("EXTERNALIP"s);
}

//======================================================================
auto ServerData::setPublicIP(const std::string &ipvalue) ->void {
    setValue("EXTERNALIP"s, ipvalue) ;
}
    
//======================================================================
auto ServerData::jsengineSize() const -> std::uint16_t {
    return getValue<std::uint16_t>("JSENGINESIZE"s);
}
//======================================================================
auto ServerData::setJSEngineSize(std::uint16_t value) ->void {
    setValue("JSENGINESIZE", value);
}
//======================================================================
auto ServerData::useUnicodeMessages() const -> bool {
    return getValue<bool>("USEUNICODEMESSAGES"s);
}
//======================================================================
auto ServerData::setUseUnicodeMessages(bool value) ->void {
    setValue("USEUNICODEMESSAGES"s, value);
}
//======================================================================
auto ServerData::serverLanguage() const -> std::uint16_t{
    return getValue<std::uint16_t>("SERVERLANGUAGE"s);

}
//======================================================================
auto ServerData::setServerLanguage(std::uint16_t value) ->void{
    setValue("SERVERLANGUAGE", value);
}
//======================================================================
auto ServerData::lightDarkLevel() const -> lightlevel_t {
    return getValue<lightlevel_t>("SERVERLANGUAGE"s);
}
//======================================================================
auto ServerData::setLightDarkLevel(lightlevel_t level) ->void {
    setValue("SERVERLANGUAGE", level);
}

//======================================================================
auto ServerData::lightBrightLevel() const -> lightlevel_t{
    return getValue<lightlevel_t>("SERVERLANGUAGE"s);
}
//======================================================================
auto ServerData::setLightBrightLevel(lightlevel_t level) ->void {
    setValue("SERVERLANGUAGE", level);
}

//======================================================================
auto ServerData::lightCurrentLevel() const -> lightlevel_t{
    return getValue<lightlevel_t>("SERVERLANGUAGE"s);
}
//======================================================================
auto ServerData::setLightCurrentLevel(lightlevel_t level) ->void {
    setValue("SERVERLANGUAGE", level);
}

//======================================================================
auto ServerData::lightDungeonLevel() const -> lightlevel_t{
    return getValue<lightlevel_t>("SERVERLANGUAGE"s);
}
//======================================================================
auto ServerData::setLightDungeonLevel(lightlevel_t level) ->void {
    setValue("SERVERLANGUAGE", level);
}

//======================================================================
auto ServerData::clientSupport4000() const -> bool  {
    return getValue<bool>("CLIENTSUPPORT4000"s);
}
//======================================================================
auto ServerData::setClientSupport4000(bool value) ->void  {
    setValue("CLIENTSUPPORT4000"s, value);
}

//======================================================================
auto ServerData::clientSupport5000() const -> bool  {
    return getValue<bool>("CLIENTSUPPORT5000"s);
}
//======================================================================
auto ServerData::setClientSupport5000(bool value) ->void  {
    setValue("CLIENTSUPPORT5000"s, value);
}

//======================================================================
auto ServerData::clientSupport6000() const -> bool  {
    return getValue<bool>("CLIENTSUPPORT6000"s);
}
//======================================================================
auto ServerData::setClientSupport6000(bool value) ->void  {
    setValue("CLIENTSUPPORT6000"s, value);
}

//======================================================================
auto ServerData::clientSupport6050() const -> bool  {
    return getValue<bool>("CLIENTSUPPORT6050"s);
}
//======================================================================
auto ServerData::setClientSupport6050(bool value) ->void  {
    setValue("CLIENTSUPPORT6050"s, value);
}

//======================================================================
auto ServerData::clientSupport7000() const -> bool  {
    return getValue<bool>("CLIENTSUPPORT7000"s);
}
//======================================================================
auto ServerData::setClientSupport7000(bool value) ->void  {
    setValue("CLIENTSUPPORT7000"s, value);
}

//======================================================================
auto ServerData::clientSupport7090() const -> bool  {
    return getValue<bool>("CLIENTSUPPORT7090"s);
}
//======================================================================
auto ServerData::setClientSupport7090(bool value) ->void  {
    setValue("CLIENTSUPPORT7090"s, value);
}

//======================================================================
auto ServerData::clientSupport70160() const -> bool  {
    return getValue<bool>("CLIENTSUPPORT70160"s);
}
//======================================================================
auto ServerData::setClientSupport70160(bool value) ->void  {
    setValue("CLIENTSUPPORT70160"s, value);
}

//======================================================================
auto ServerData::clientSupport70240() const -> bool  {
    return getValue<bool>("CLIENTSUPPORT70240"s);
}
//======================================================================
auto ServerData::setClientSupport70240(bool value) ->void  {
    setValue("CLIENTSUPPORT70240"s, value);
}

//======================================================================
auto ServerData::clientSupport70300() const -> bool  {
    return getValue<bool>("CLIENTSUPPORT70300"s);
}
//======================================================================
auto ServerData::setClientSupport70300(bool value) ->void  {
    setValue("CLIENTSUPPORT70300"s, value);
}

//======================================================================
auto ServerData::clientSupport70331() const -> bool  {
    return getValue<bool>("CLIENTSUPPORT70331"s);
}
//======================================================================
auto ServerData::setClientSupport70331(bool value) ->void  {
    setValue("CLIENTSUPPORT70331"s, value);
}

//======================================================================
auto ServerData::clientSupport704565() const -> bool  {
    return getValue<bool>("CLIENTSUPPORT704565"s);
}
//======================================================================
auto ServerData::setClientSupport704565(bool value) ->void  {
    setValue("CLIENTSUPPORT704565"s, value);
}

//======================================================================
auto ServerData::clientSupport70610() const -> bool  {
    return getValue<bool>("CLIENTSUPPORT70610"s);
}
//======================================================================
auto ServerData::setClientSupport70610(bool value) ->void {
    setValue("CLIENTSUPPORT70610"s, value);
}
