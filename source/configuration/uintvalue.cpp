//

#include "uintvalue.hpp"

#include <algorithm>
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
const std::map<std::string, UIntValue::item_t> UIntValue::NAMEITEMMAP{
    {"NETRCVTIMEOUT"s,NETRCVTIMEOUT},{"NETSNDTIMEOUT"s,NETSNDTIMEOUT},
    {"NETRETRYCOUNT"s,NETRETRYCOUNT},{"SAVESTIMER"s,SAVESTIMER},
    {"MAXCLIENTBYTESIN"s,MAXCLIENTBYTESIN},{"MAXCLIENTBYTESOUT"s,MAXCLIENTBYTESOUT},
    {"NETTRAFFICTIMEBAN"s,NETTRAFFICTIMEBAN},{"POLLTIME"s,POLLTIME},
    {"MAYORTIME"s,MAYORTIME},{"TAXPERIOD"s,TAXPERIOD},
    {"GUARDSPAID"s,GUARDPAID},{"MAXPLAYERPACKWEIGHT"s,MAXPLAYERPACKWEIGHT}
    ,{"MAXPLAYERBANKWEIGHT"s,MAXPLAYERBANKWEIGHT}
};
//======================================================================
auto UIntValue::nameFor(item_t setting)-> const std::string &{
    auto iter = std::find_if(NAMEITEMMAP.begin(),NAMEITEMMAP.end(),[setting](const std::pair<std::string,item_t> &entry){
        return setting == entry.second;
    });
    if (iter == NAMEITEMMAP.end()){
        throw std::runtime_error("No name was found for item_t setting: "s+std::to_string(static_cast<int>(setting)));
    }
    return iter->first ;
}

//======================================================================
UIntValue::UIntValue() {
    this->reset();
}
//======================================================================
auto UIntValue::reset() ->void {
    values = std::vector<std::uint32_t>(NAMEITEMMAP.size(),0) ;
    this->operator[](NETRCVTIMEOUT) = 3 ;
    this->operator[](NETSNDTIMEOUT) = 3 ;
    this->operator[](NETRETRYCOUNT) = 3 ;
    this->operator[](SAVESTIMER) = 600 ;
    this->operator[](MAXCLIENTBYTESIN) = 25000 ;
    this->operator[](MAXCLIENTBYTESOUT) = 100000 ;
    this->operator[](NETTRAFFICTIMEBAN) = 30 ;
//    this->operator[](SPAWREGIONFACETS) = 0 ;
    this->operator[](POLLTIME) = 3600 ;
    this->operator[](MAYORTIME) = 36000 ;
    this->operator[](TAXPERIOD) = 1800 ;
    this->operator[](GUARDPAID) = 3600 ;
    this->operator[](MAXPLAYERPACKWEIGHT) = 40000 ;
    this->operator[](MAXPLAYERBANKWEIGHT) = 160000 ;
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
