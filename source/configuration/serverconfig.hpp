//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef serverconfig_hpp
#define serverconfig_hpp

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>

#include "directorylocation.hpp"
#include "clientenable.hpp"
#include "clientfeature.hpp"
#include "serverfeature.hpp"
#include "assistantfeature.hpp"
#include "startlocconfig.hpp"
#include "serverswitch.hpp"
//======================================================================
class ServerConfig {
    
private:
    // Because singleton, we hide this from others 
    ServerConfig() ;
    
    [[maybe_unused]] auto loadKeyValue(const std::string &key, const std::string &value) ->bool ;
    [[maybe_unused]] auto reloadKeyValue(const std::string &key, const std::string &value) ->bool ;
    
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
    auto reset() ->void ;
    
    auto directoryFor(dirlocation_t location) const -> const std::filesystem::path &;
    auto dumpPaths() const ->void ;
    auto enabled(ServerSwitch::switch_t setting) const ->bool ;
    auto setEnabled(ServerSwitch::switch_t setting, bool state) ->void ;
    
    ClientEnable enableClients ;
    ClientFeature clientFeature ;
    ServerFeature serverFeature ;
    AssistantFeature assistantFeature ;
    StartLocConfig startLocation;
    StartLocConfig youngLocation;
    ServerSwitch serverSwitch;
    
};

#endif /* serverconfig_hpp */
