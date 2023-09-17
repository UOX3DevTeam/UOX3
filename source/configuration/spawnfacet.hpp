//

#ifndef spawnfacet_hpp
#define spawnfacet_hpp

#include <bitset>
#include <cstdint>
#include <iostream>
#include <string>

//======================================================================
// We wrappered this, in case we ever go to std::vector<bool>
//======================================================================
struct SpawnFacet {
    static const std::string name ;
    std::bitset<64>  facets;    // Used to determine which facets to enable spawn regions for
    SpawnFacet() ;
    auto set(size_t index, bool value) -> void;
    auto test(size_t index) const ->bool ;
    auto reset() ->void ;
    auto value() const -> std::uint64_t ;
    auto operator=(std::uint64_t value) -> SpawnFacet& ;
    auto operator[](size_t index) const -> bool ;
    
    auto setKeyValue(const std::string &key, const std::string &value) ->bool ;
    
};

#endif /* spawnfacet_hpp */
