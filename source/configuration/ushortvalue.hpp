//

#ifndef ushortvalue_hpp
#define ushortvalue_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

//======================================================================
struct UShortValue {
    enum item_t {
        PORT,LANGUAGE,JSENGINE,SYSMESSAGECOLOR,
        APSPERFTHRESHOLD,APSINTERNAL,APSDELAYSTEP,APSDELAYMAX,
        SKILLLEVEL,SKILLCAP,SKILLDELAY,STATCAP,
        MAXPLAYERPACKITEM,MAXPLAYERBANKITEM,MAXSAFETELEPORT,
        MAXCONTROLSLOT,MAXFOLLOWER,MAXPETOWNER,
        PETLOYALITYGAINSUCCESS,PETLOYALITYGAINFAILURE,
        POSTINGLEVEL,REMOVALLEVEL,DUNGEONLIGHT,
        BRIGHTLEVEL,DARKLEVEL,SECONDSPERUOMINUTE,BASERANGE,
        MAXTARGET,MSGREDISPLAYTIME,MAXKILL,RESOURCEAREASIZE,
        MINECHECK,LOGSPERAREA,PETOFFLINETIMEOUT,ANIMALATTACKCHANCE,MINWEAPONDAMAGE,
        MAXWEAPONDAMAGE,ARMORDAMAGECHANCE,MINARMORDAMAGE,MAXARMORDAMAGE,
        PARRYDAMAGECHANCE,MINPARRYDAMAGE,MAXPARRYDAMAGE,ALCHEMENYBONUSMODIFIER,
        BLOODEFFECTCHANCE,WEAPONDAMAGEBONUSTYPE,STARTPRIV,
        TITLECOLOR,LEFTTEXTCOLOR,RIGHTTEXTCOLOR,
        BUTTONCANCEL,BUTTONLEFT,BUTTONRIGHT,BACKGROUNDPIC,
        MAXHOUSEOWNABLE,MAXHOUSECOOWNABLE
    };
    static const std::map<std::string, item_t> NAMEITEMMAP ;
    
    std::vector<std::uint16_t> values ;
    
    UShortValue() ;
    auto reset() ->void ;
    
    auto operator[](item_t item) const -> const std::uint16_t &;
    auto operator[](item_t item) -> std::uint16_t &;
    
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    auto valueFor(const std::string &value) const -> std::optional<std::uint16_t> ;
};



#endif /* ushortvalue_hpp */
