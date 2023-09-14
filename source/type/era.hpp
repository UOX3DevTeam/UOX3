//

#ifndef era_hpp
#define era_hpp

#include <cstdint>
#include <iostream>
#include <map>
#include <string>

//======================================================================
struct Era {
    //Core (used to inherit CoreShardEra setting)
    //UO - Original UO release
    //T2A - The Second Age
    //UOR - Rennaissance
    //TD - Third Dawn
    //LBR - Lord Blackthorn's Revenge
    //AoS - Age of Shadows
    //SE - Samurai Empire
    //ML - Mondain's Legacy
    //SA - Stygian Abyss
    //HS - High Seas
    //ToL - Time of Legends
    enum era_t{
        CORE, UO, T2A, UOR,
        TD, LBR, AOS, SE,
        ML, SA, HS, TOL
    };
    static const std::map<std::string, era_t> ERANAMEMAP ;
    static auto eraFromName(const std::string &name) -> era_t ;
    static auto nameFromEra(era_t era) -> const std::string& ;
    static auto size()  -> size_t ;
    static auto latest()  -> era_t ;

    era_t value ;
    
    Era() ;
    Era(era_t era);
    Era(const std::string &eraname);
    
    auto describe() const -> const std::string& ;
    
    auto operator==(const Era &value) const ->bool ;
    auto operator!=(const Era &value) const ->bool ;
    auto operator<(const Era &value) const ->bool ;
    auto operator>(const Era &value) const ->bool ;
    auto operator<=(const Era &value) const ->bool ;
    auto operator>=(const Era &value) const ->bool ;
};

#endif /* era_hpp */
