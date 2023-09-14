//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef serverconfig_hpp
#define serverconfig_hpp

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "directorylocation.hpp"
#include "clientenable.hpp"
#include "clientfeature.hpp"
#include "serverfeature.hpp"
#include "assistantfeature.hpp"
#include "startlocconfig.hpp"
#include "serverswitch.hpp"
#include "serverstring.hpp"
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
    
    ClientEnable enableClients ;
    ClientFeature clientFeature ;
    ServerFeature serverFeature ;
    AssistantFeature assistantFeature ;
    StartLocConfig startLocation;
    StartLocConfig youngLocation;
    ServerSwitch serverSwitch;
    ServerString serverString;
    
};

#endif /* serverconfig_hpp */
