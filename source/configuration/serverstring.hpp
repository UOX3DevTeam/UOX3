//

#ifndef serverstring_hpp
#define serverstring_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <vector>

//======================================================================
struct ServerString {
    enum item_t {
        SERVERNAME=0,PUBLICIP,COMMANDPREFIX,SHARDKEY,SERVERIP
    };
    static const std::map<std::string,item_t> ITEMNAMEMAP ;
    std::vector<std::string> item;
    ServerString() ;
    auto reset() ->void ;
    auto operator[](item_t index) const -> const std::string & ;
    auto operator[](item_t index)  ->  std::string & ;
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    auto valueFor(const std::string &keyword) const ->std::optional<std::string> ;
};

#endif /* serverstring_hpp */
