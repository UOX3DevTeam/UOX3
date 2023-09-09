//Copyright © 2023 Charles Kerr. All rights reserved.

#include "serverconfig.hpp"

#include <fstream>
#include <stdexcept>
#include "utility/strutil.hpp"
#include <vector>

using namespace std::string_literals ;

//======================================================================
ServerConfig::ServerConfig(){
    
}
//======================================================================
auto ServerConfig::loadKeyValue(const std::string &key, const std::string &value)->bool {
    auto rvalue = true ;
    if (!directoryLocation.setLocation(key, value)){
        std::clog <<"Unhanded key/value: "<<key<<" = " << value << std::endl;
        rvalue = false ;
    }
    return rvalue ;
}
//======================================================================
auto ServerConfig::reloadKeyValue(const std::string &key, const std::string &value)->bool {
    auto rvalue = true ;
    // We dont reload the entire config file
    return rvalue ;
}

//======================================================================
auto ServerConfig::shared() ->ServerConfig&{
    static ServerConfig instance ;
    return instance ;
}
//======================================================================
auto ServerConfig::loadConfig(const std::filesystem::path &config) ->void {
    auto input = std::ifstream(config.string());
    if (!input.is_open()){
        throw std::runtime_error("Unable to open UOX3 configuration file: "s + config.string());
    }
    configFile = config ;
    auto buffer = std::vector<char>(4096,0) ;
    while(input.good() && !input.eof()){
        input.getline(buffer.data(),buffer.size()-1);
        if (input.gcount()>0){
            buffer[input.gcount()] = 0 ;
            std::string line = buffer.data() ;
            line = util::trim(util::strip(line,"//"));
            if (!line.empty()){
                if (line[0]!='[' && line[0]!='{' && line[0]!='}'){
                    auto [lkey,value] = util::split(line,"=");
                    auto key = util::upper(lkey) ;
                    loadKeyValue(key, value);
                }
            }
        }
    }
    directoryLocation.postLoad() ;
}
//======================================================================
auto ServerConfig::reloadConfig() ->void{
    auto input = std::ifstream(configFile.string());
    if (!input.is_open()){
        throw std::runtime_error("Unable to open UOX3 configuration file: "s + configFile.string());
    }
    auto buffer = std::vector<char>(4096,0) ;
    while(input.good() && !input.eof()){
        input.getline(buffer.data(),buffer.size()-1);
        if (input.gcount()>0){
            buffer[input.gcount()] = 0 ;
            std::string line = buffer.data() ;
            line = util::trim(util::strip(line,"//"));
            if (!line.empty()){
                if (line[0]!='[' && line[0]!='{' && line[0]!='}'){
                    auto [lkey,value] = util::split(line,"=");
                    auto key = util::upper(lkey) ;
                    reloadKeyValue(key, value);
                }
            }
        }
    }

}
//======================================================================
auto ServerConfig::directoryFor(dirlocation_t location) const -> const std::filesystem::path &{
    return directoryLocation[location] ;
}
