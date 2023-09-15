//

#include "uintvalue.hpp"
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
const std::map<std::string, UIntValue::item_t> UIntValue::NAMEITEMMAP{
    {"NETRCVTIMEOUT"s,NETRCVTIMEOUT},{"NETSNDTIMEOUT"s,NETSNDTIMEOUT},
    {"NETRETRYCOUNT"s,NETRETRYCOUNT},{"SAVESTIMER"s,SAVESTIMER},
    {"MAXCLIENTBYTESIN"s,MAXLIENTBYTESIN},{"MAXCLIENTBYTESOUT"s,MAXCLIENTBYTESOUT},
    {"NETTRAFFICTIMEBAN"s,NETTRAFFICTIMEBAN},{"SPAWNREGIONSFACETS"s,SPAWREGIONFACETS},
    {"POLLTIME"s,POLLTIME},{"MAYORTIME"s,MAYORTIME},{"TAXPERIOD"s,TAXPERIOD},
    {"GUARDSPAID"s,GUARDPAID},
    {"MAXPLAYERPACKWEIGHT"s,MAXPLAYERPACKWEIGHT},{"MAXPLAYERBANKWEIGHT"s,MAXPLAYERBANKWEIGHT}
};
//======================================================================
UIntValue::UIntValue() {
    this->reset();
}
//======================================================================
auto UIntValue::reset() ->void {
    values = std::vector<std::uint32_t>(NAMEITEMMAP.size(),0) ;
}

//======================================================================
auto UIntValue::operator[](item_t item) const -> const std::uint32_t &{
    return values.at(static_cast<size_t>(item)) ;
}
//======================================================================
auto UIntValue::operator[](item_t item) -> std::uint32_t &{
    return values.at(static_cast<size_t>(item)) ;
}

//======================================================================
auto UIntValue::setKeyValue(const std::string &key, const std::string &value) ->bool {
    auto rvalue = false ;
    auto iter = NAMEITEMMAP.find(key) ;
    if (iter != NAMEITEMMAP.end()){
        rvalue = true ;
        values.at(static_cast<size_t>(iter->second)) = static_cast<std::uint32_t>(std::stoul(value,nullptr,0));
    }
    return rvalue ;
}
//======================================================================
auto UIntValue::valueFor(const std::string &value) const -> std::optional<std::uint32_t> {
    auto iter = NAMEITEMMAP.find(value) ;
    if (iter == NAMEITEMMAP.end()){
        return {};
    }
    return  values.at(static_cast<size_t>(iter->second));
}
