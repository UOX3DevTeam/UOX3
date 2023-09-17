//

#include "serverstring.hpp"

#include <algorithm>
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
const std::map<std::string,ServerString::item_t> ServerString::ITEMNAMEMAP{
    {"SERVERNAME"s,SERVERNAME},{"EXTERNALIP"s,PUBLICIP},{"COMMANDPREFIX"s,COMMANDPREFIX},{"SECRETSHARDKEY"s,SHARDKEY},
    {"SERVERIP"s,SERVERIP}
};
//======================================================================
auto ServerString::nameFor(item_t setting)-> const std::string &{
    auto iter = std::find_if(ITEMNAMEMAP.begin(),ITEMNAMEMAP.end(),[setting](const std::pair<std::string,item_t> &entry){
        return setting == entry.second;
    });
    if (iter == ITEMNAMEMAP.end()){
        throw std::runtime_error("No name was found for item_t setting: "s+std::to_string(static_cast<int>(setting)));
    }
    return iter->first ;
}
//======================================================================
ServerString::ServerString(){
    reset();
}
//======================================================================
auto ServerString::reset() ->void {
    item = std::vector<std::string>(ITEMNAMEMAP.size(),std::string());
    this->operator[](SERVERIP) = "127.0.0.1";
    this->operator[](PUBLICIP) = "127.0.0.1";
    this->operator[](SERVERNAME) = "My UOX3 Shard";
    this->operator[](SHARDKEY) = "None";
    this->operator[](COMMANDPREFIX) = "\\" ;
}
//======================================================================
auto ServerString::operator[](item_t index) const -> const std::string & {
    return item.at(index);
}
//======================================================================
auto ServerString::operator[](item_t index)  ->  std::string & {
    return item.at(index);
}
//======================================================================
auto ServerString::setKeyValue(const std::string &key, const std::string &value) ->bool {
    auto rvalue = false ;
    auto iter = ITEMNAMEMAP.find(key) ;
    if (iter != ITEMNAMEMAP.end()){
        rvalue = true ;
        item.at(iter->second) = value ;
    }
    return rvalue;
}
//======================================================================
auto ServerString::valueFor(const std::string &keyword) const ->std::optional<std::string> {
    auto iter  = ITEMNAMEMAP.find(keyword) ;
    if (iter != ITEMNAMEMAP.end()){
        return item.at(iter->second);
    }
    return {} ;
}
