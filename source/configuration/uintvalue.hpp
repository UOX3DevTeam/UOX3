//

#ifndef uintvalue_hpp
#define uintvalue_hpp
#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

//======================================================================
struct UIntValue {
    enum item_t {
        NETRCVTIMEOUT,NETSNDTIMEOUT,NETRETRYCOUNT,SAVESTIMER,
        MAXLIENTBYTESIN,MAXCLIENTBYTESOUT,NETTRAFFICTIMEBAN,SPAWREGIONFACETS,
        POLLTIME,MAYORTIME,TAXPERIOD,GUARDPAID,
        MAXPLAYERPACKWEIGHT,MAXPLAYERBANKWEIGHT
    };
    static const std::map<std::string, item_t> NAMEITEMMAP ;
    
    std::vector<std::uint32_t> values ;
    
    UIntValue() ;
    auto reset() ->void ;
    
    auto operator[](item_t item) const -> const std::uint32_t &;
    auto operator[](item_t item) -> std::uint32_t &;
    
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    auto valueFor(const std::string &value) const -> std::optional<std::uint32_t> ;
};


#endif /* uintvalue_hpp */
