//

#ifndef serverstring_hpp
#define serverstring_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

// SERVERNAME Name of the server to be greated with
// PUBLICIP The ip that will be used to relay to when connected by a client not on your network
// COMMANDPREFIX The character to symbol a command is being used in the client talk line
// SHARDKEY secret shard key can be defined, which can be used as a way to restrict which clients can connect to the shard
// SERVERIP ip used to relay to if on network (not sure this is used anymore?)
//======================================================================
struct ServerString {
    enum item_t {
        SERVERNAME=0,PUBLICIP,COMMANDPREFIX,SHARDKEY,SERVERIP
    };
    static const std::map<std::string,item_t> ITEMNAMEMAP ;
    static auto nameFor(item_t setting)-> const std::string & ;
    std::vector<std::string> item;
    ServerString() ;
    auto reset() ->void ;
    auto operator[](item_t index) const -> const std::string & ;
    auto operator[](item_t index)  ->  std::string & ;
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    auto valueFor(const std::string &keyword) const ->std::optional<std::string> ;
};

#endif /* serverstring_hpp */
