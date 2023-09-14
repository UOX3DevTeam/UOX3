//

#include "serverstring.hpp"
#include <stdexcept>


using namespace std::string_literals ;

//======================================================================
const std::map<std::string,ServerString::item_t> ServerString::ITEMNAMEMAP{
    {"SERVERNAME"s,SERVERNAME},{"EXTERNALIP"s,PUBLICIP},{"COMMANDPREFIX"s,COMMANDPREFIX},{"SECRETSHARDKEY"s,SHARDKEY},
    {"SERVERIP"s,SERVERIP}
};
//======================================================================
ServerString::ServerString(){
    reset();
}
//======================================================================
auto ServerString::reset() ->void {
    item = std::vector<std::string>(ITEMNAMEMAP.size(),std::string());
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
