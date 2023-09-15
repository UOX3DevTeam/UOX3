//

#ifndef shortvalue_hpp
#define shortvalue_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

//======================================================================
struct ShortValue {
    enum item_t {
        SAVERATIO,MAXSTEALTHMOVEMENT,
        MAXSTAMINIAMOVEMENT,AMBIENTSOUND,
        MAXSELLITEM,BANKBUYTHRESHOLD,
        FISHINGSTAMINALOSS,OREPERAREA,
        FISHPERAREA,HUNGERDAMAGE,
        THIRSTDRAIN,MAXRANGE,
        MAXSPELLRANGE,NPCDAMAGERATE,
        NPCBASEFLEEAT,NPCBASEREATTACKAT,
        ATTACKSTAMINA,ARCHERYTHITBONUS,
        WEAPONDAMAGECHANCE,STARTGOLD
    };
    static const std::map<std::string, item_t> NAMEITEMMAP ;
    
    std::vector<std::int16_t> values ;
    
    ShortValue() ;
    auto reset() ->void ;
    
    auto operator[](item_t item) const -> const std::int16_t &;
    auto operator[](item_t item) -> std::int16_t &;
    
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    auto valueFor(const std::string &value) const -> std::optional<std::int16_t> ;
};



#endif /* shortvalue_hpp */
