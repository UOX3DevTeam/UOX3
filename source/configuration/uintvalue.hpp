//

#ifndef uintvalue_hpp
#define uintvalue_hpp
#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

// NETRCVTIMEOUT Used to be hardcoded as 2 seconds (2 * 1000ms) for some really laggy nets this would drop alot of packets, and disconnect people.
// NETSNDTIMEOUT 04/03/2004 - Not used at this time.
// NETRETRYCOUNT04/03/2004 - Used to set the number of times a network recieve will be attempted before it throws an error
// SAVESTIMER Number of seconds between world saves
// MAXCLIENTBYTESIN Max bytes that can be received from a client in a 10-second window before client is warned/kicked for excessive data use
// MAXCLIENTBYTESOUT  Max bytes that can be sent to a client in a 10-second window before client is warned/kicked for excessive data use
// NETTRAFFICTIMEBAN Duration in minutes that player will be banned for if they exceed their
// POLLTIME Time (in seconds) for which a town voting poll is open
// MAYORTIME Time (in seconds) that a PC would be a mayor
// TAXPERIOD Time (in seconds) between periods of taxes for PCs
// GUARDPAID Time (in seconds) between payments for guards
// MAXPLAYERPACKWEIGHT The max weight capacity of a player's backpack (including subcontainers)
// MAXPLAYERBANKWEIGHT The max weight capacity of a player's bankbox (including subcontainers)
//======================================================================
struct UIntValue {
    enum item_t {
        NETRCVTIMEOUT,NETSNDTIMEOUT,NETRETRYCOUNT,SAVESTIMER,
        MAXCLIENTBYTESIN,MAXCLIENTBYTESOUT,NETTRAFFICTIMEBAN,POLLTIME,
        MAYORTIME,TAXPERIOD,GUARDPAID,MAXPLAYERPACKWEIGHT, 
        MAXPLAYERBANKWEIGHT
    };
    static const std::map<std::string, item_t> NAMEITEMMAP ;
    static auto nameFor(item_t setting)-> const std::string & ;

    std::vector<std::uint32_t> values ;
    
    UIntValue() ;
    auto reset() ->void ;
    
    auto operator[](item_t item) const -> const std::uint32_t &;
    auto operator[](item_t item) -> std::uint32_t &;
    
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    auto valueFor(const std::string &value) const -> std::optional<std::uint32_t> ;
};


#endif /* uintvalue_hpp */
