//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef serverconfig_hpp
#define serverconfig_hpp

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "assistantfeature.hpp"
#include "clientenable.hpp"
#include "clientfeature.hpp"
#include "directorylocation.hpp"
#include "expansion.hpp"
#include "realnumberconfig.hpp"
#include "serverfeature.hpp"
#include "serverswitch.hpp"
#include "serverstring.hpp"
#include "shortvalue.hpp"
#include "spawnfacet.hpp"
#include "startlocconfig.hpp"
#include "timersetting.hpp"
#include "ushortvalue.hpp"
#include "uintvalue.hpp"

//======================================================================
// AllDataType
//======================================================================
using alldata_t = std::variant<std::string,bool,std::uint8_t,std::int8_t,std::uint64_t,std::int64_t,std::uint16_t,std::int16_t,std::uint32_t,std::int32_t,float,double> ;

//======================================================================
struct AllDataType {
    enum datatype_t {
        T_NONE,T_STRING, T_BOOL, T_UINT8, T_INT8, T_UINT64, T_INT64,
        T_UINT16, T_INT16, T_UINT32, T_INT32, T_FLOAT, T_DOUBLE
    };
    datatype_t type ;
    alldata_t value ;
    AllDataType() { type = T_NONE ;}
};

//======================================================================
// ServerConfig
//======================================================================
//======================================================================
class ServerConfig {
    
private:
    // Because singleton, we hide this from others
    ServerConfig() ;
    static const std::vector<StartLocation> fallbackStartLocation ;
    static const std::vector<StartLocation> fallbackYoungLocation ;
    
    [[maybe_unused]] auto loadKeyValue(const std::string &key, const std::string &value) ->bool ;
    
    auto postCheck() ->void ;
    
    std::filesystem::path configFile ;
    DirectoryLocation directoryLocation ;
public:
    // Make this a singleton
    ServerConfig(const ServerConfig&) = delete ;
    ServerConfig(ServerConfig&&) = delete ;
    auto operator=(const ServerConfig&) ->ServerConfig& = delete ;
    auto operator=(ServerConfig&&)->ServerConfig& = delete ;
    static auto shared() ->ServerConfig& ;
    
    auto loadConfig(const std::filesystem::path &config) ->void ;
    auto writeConfig(const std::filesystem::path &config) const ->bool ;
    auto reset() ->void ;
    
    auto directoryFor(dirlocation_t location) const -> const std::filesystem::path &;
    auto unwindDirectoryFor(dirlocation_t location) const -> std::filesystem::path ;
    auto enabled(ServerSwitch::switch_t setting) const ->bool ;
    auto setEnabled(ServerSwitch::switch_t setting, bool state) ->void ;
    
    auto timer(TimerSetting::timer_t timer) const -> const std::uint16_t& ;
    
    auto dataForKeyword(const std::string &key) const -> std::optional<AllDataType>;
    
    AssistantFeature assistantFeature ;
    ClientEnable enableClients ;
    ClientFeature clientFeature ;
    Expansion ruleSets ;
    RealNumberConfig realNumbers ;
    ServerFeature serverFeature ;
    ServerString serverString;
    ServerSwitch serverSwitch;
    ShortValue shortValues;
    SpawnFacet spawnFacet ;
    StartLocConfig startLocation;
    StartLocConfig youngLocation;
    TimerSetting timerSetting ;
    UShortValue ushortValues ;
    UIntValue uintValues;
};

#endif /* serverconfig_hpp */
