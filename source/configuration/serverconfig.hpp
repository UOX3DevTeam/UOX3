//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef serverconfig_hpp
#define serverconfig_hpp

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "directorylocation.hpp"
#include "clientenable.hpp"
#include "clientfeature.hpp"
#include "serverfeature.hpp"
#include "assistantfeature.hpp"
#include "startlocconfig.hpp"
#include "serverswitch.hpp"
#include "serverstring.hpp"
#include "expansion.hpp"
#include "realnumberconfig.hpp"
#include "timersetting.hpp"
#include "ushortvalue.hpp"
#include "uintvalue.hpp"
#include "shortvalue.hpp"

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
    [[maybe_unused]] auto reloadKeyValue(const std::string &key, const std::string &value) ->bool ;
    
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
    auto reloadConfig() ->void;
    auto writeConfig(const std::filesystem::path &config) const ->bool ;
    auto reset() ->void ;
    
    auto directoryFor(dirlocation_t location) const -> const std::filesystem::path &;
    auto unwindDirectoryFor(dirlocation_t location) const -> std::filesystem::path ;
    auto enabled(ServerSwitch::switch_t setting) const ->bool ;
    auto setEnabled(ServerSwitch::switch_t setting, bool state) ->void ;
    
    auto timer(TimerSetting::timer_t timer) const -> const std::uint16_t& ;
    
    auto dataForKeyword(const std::string &key) const -> std::optional<AllDataType>;
    
    ClientEnable enableClients ;
    ClientFeature clientFeature ;
    ServerFeature serverFeature ;
    AssistantFeature assistantFeature ;
    StartLocConfig startLocation;
    StartLocConfig youngLocation;
    ServerSwitch serverSwitch;
    ServerString serverString;
    Expansion ruleSets ;
    TimerSetting timerSetting ;
    RealNumberConfig realNumbers ;
    UShortValue ushortValues ;
    UIntValue uintValues;
    ShortValue shortValues;
};

#endif /* serverconfig_hpp */
