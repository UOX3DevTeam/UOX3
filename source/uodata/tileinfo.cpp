//Copyright © 2023 Charles Kerr. All rights reserved.

#include "tileinfo.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <vector>

#include "utility/strutil.hpp"
using namespace std::string_literals ;

namespace uo{
    //======================================================================
    // BaseTileInfo
    //======================================================================
    //======================================================================
    auto BaseTileInfo::loadName(std::istream &input) ->void {
        auto buffer = std::vector<char>(uo::INFOSTRINGSIZE+1,0);
        input.read(buffer.data(), buffer.size()-1);
        buffer[input.gcount()]=0 ;
        name = buffer.data() ;
    }
    //======================================================================
    auto BaseTileInfo::describe() const ->std::string {
        return this->name ;
    }

    //======================================================================
    //======================================================================
    // TerrainInfo
    //======================================================================
    
    //======================================================================
    TerrainInfo::TerrainInfo():BaseTileInfo(){
        type = TileType::TERRAIN;
        textureID = 0 ;
    }
    //======================================================================
    TerrainInfo::TerrainInfo(std::istream &input, bool largeFlag):TerrainInfo(){
        load(input, largeFlag);
    }
    //======================================================================
    auto TerrainInfo::load(std::istream &input,bool largeFlag) ->void{
        auto flagsize = (largeFlag? 8: 4) ;
        input.read(reinterpret_cast<char*>(&flag.value),flagsize) ;
        input.read(reinterpret_cast<char*>(&textureID),2) ;  // textureID is 2 bytes in file
        this->loadName(input) ;
    }
    //======================================================================
    auto TerrainInfo::describe() const ->std::string {
        return BaseTileInfo::describe() +","s+ util::format("0x%04x",textureID) ;
    }

    //======================================================================
    // ArtInfo
    //======================================================================
    //======================================================================
    ArtInfo::ArtInfo() : BaseTileInfo() {
        type = TileType::ART ;
        weight = 0 ;
        quality = 0  ;
        miscData = 0  ;
        unknown2 = 0  ;
        quantity = 0  ;
        animid = 0  ;
        unknown3 = 0  ;
        hue = 0  ;
        stackOffset = 0  ;
        height = 0 ;
    }
    //======================================================================
    ArtInfo::ArtInfo(std::istream &input, bool largeFlag):ArtInfo(){
        load(input, largeFlag);
    }
    
    //======================================================================
    auto ArtInfo::load(std::istream &input,bool largeFlag) ->void{
        auto flagsize = (largeFlag? 8: 4) ;
        input.read(reinterpret_cast<char*>(&flag.value),flagsize) ;
        
        input.read(reinterpret_cast<char*>(&weight),1) ;  //1 bytes in file
        input.read(reinterpret_cast<char*>(&quality),1) ;  //1 bytes in file
        input.read(reinterpret_cast<char*>(&miscData),2) ;  //2 bytes in file
        input.read(reinterpret_cast<char*>(&unknown2),1) ;  //1 bytes in file
        input.read(reinterpret_cast<char*>(&quantity),1) ;  //1 bytes in file
        input.read(reinterpret_cast<char*>(&animid),2) ;  //2 bytes in file
        input.read(reinterpret_cast<char*>(&unknown3),1) ;  //1 bytes in file
        input.read(reinterpret_cast<char*>(&hue),1) ;  //1 bytes in file
        input.read(reinterpret_cast<char*>(&stackOffset),2) ;  //2 bytes in file
        input.read(reinterpret_cast<char*>(&height),1) ;  //1 bytes in file
        
        this->loadName(input) ;
    }
    //======================================================================
    auto ArtInfo::describe() const ->std::string  {
        return BaseTileInfo::describe() + ","s + std::to_string(static_cast<std::uint16_t>(weight))+ "," + std::to_string(static_cast<std::uint16_t>(quality)) + "," + std::to_string(miscData) + "," + util::format("0x%02x",unknown2) + "," + std::to_string(static_cast<std::uint16_t>(quantity)) + "," + util::format("0x%04x",animid) + "," + util::format("0x%02x",unknown3)  + "," + std::to_string(static_cast<std::uint16_t>(hue))  + "," + std::to_string(stackOffset) + "," + std::to_string(static_cast<std::uint16_t>(height));
    }

    //======================================================================
    // TileInfo
    //======================================================================
    //======================================================================
    TileInfo::TileInfo(const std::filesystem::path &path) {
        if (!path.empty()){
            load(path);
        }
    }
    //======================================================================
    auto TileInfo::load(const std::filesystem::path &path) ->bool {
        if (path.empty()){
            throw std::runtime_error("Path for tile information is empty");
        }
        auto filesize = std::filesystem::file_size(path) ;
        auto largeFlag = filesize >= uo::TILEDATASIZE ;
        auto input = std::ifstream(path.string(),std::ios::binary);
        if (!input.is_open()){
            throw std::runtime_error("Unable to open tile information file: "s+path.string());
        }
        artInfo.clear();
        terrainInfo.clear();
        auto header = 0 ;
        for (auto count =0 ; count < uo::NUMTERRAINBLOCK * uo::ENTRIESPERBLOCK;++count) {
            if (largeFlag){
                if (((count % uo::ENTRIESPERBLOCK == 0) && (count>0) ) || (count ==1)) {
                    input.read(reinterpret_cast<char*>(&header),sizeof(header));
                }
            }
            else if (count % uo::ENTRIESPERBLOCK == 0) {
                input.read(reinterpret_cast<char*>(&header),sizeof(header));
            }
            terrainInfo.push_back(TerrainInfo(input,largeFlag)) ;
        }
        auto count = 0 ;
        do {
            if (count % uo::ENTRIESPERBLOCK == 0) {
                input.read(reinterpret_cast<char*>(&header),sizeof(header));
            }
            artInfo.push_back(ArtInfo(input,largeFlag));
            count++;
        } while(( !input.eof() ) && input.good() );
        return true ;
    }
    //======================================================================
    auto TileInfo::loadOverride(const std::filesystem::path &path) ->void {
        auto input = std::ifstream(path.string()) ;
        if (input.is_open()){
            loadOverride(input) ;
        }
    }
    
    //======================================================================
    auto TileInfo::loadOverride(std::istream &input) ->void {
        auto tileid = 0 ;
        
        auto buffer = std::vector<char>(4096,0) ;
        while (input.good() && !input.eof()){
            input.getline(buffer.data(), buffer.size()-1);;
            if (input.gcount()>0){
                buffer[input.gcount()]=0 ;
                std::string line = buffer.data();
                line = util::trim(util::strip(line,"//"));
                if (!line.empty()){
                    if (line[0]=='['){
                        auto cont = util::contentsOf(line, "[", "]");
                        auto [num,type] = util::split(cont,":");
                        tileid = std::stoi(num,nullptr,0);
                    }
                    else if (line[0] != '{' && line[0] != '}'){
                        auto [key,value] = util::split(line,"=");
                        auto lkey = util::lower(key);
                        if (lkey=="height") {
                            this->artInfo.at(tileid).height = std::stoi(value,nullptr,0);
                        }
                        else if (lkey=="weight") {
                            this->artInfo.at(tileid).weight = std::stoi(value,nullptr,0);
                        }
                        else if (lkey=="layer") {
                            this->artInfo.at(tileid).quality = std::stoi(value,nullptr,0);
                        }
                        else if (lkey=="hue") {
                            this->artInfo.at(tileid).hue = std::stoi(value,nullptr,0);
                        }
                        else if (lkey=="animation") {
                            this->artInfo.at(tileid).animid = std::stoi(value,nullptr,0);
                        }
                        else if (lkey=="name") {
                            this->artInfo.at(tileid).name = std::stoi(value,nullptr,0);
                        }
                        else if (lkey == "quantity") {
                            this->artInfo.at(tileid).quantity = std::stoi(value, nullptr, 0);
                        }
                        else {
                            if (std::stoi(value) > 0) {
                                this->artInfo.at(tileid).flag.setFlag(Flag::maskForName(lkey));
                            }
                            else {
                                this->artInfo.at(tileid).flag.clearFlag(Flag::maskForName(lkey));
                            }
                        }
                    }
                }
            }
        }
    }
    //======================================================================
    auto TileInfo::loadUOX3Override(const std::filesystem::path &path) ->void {
        auto input = std::ifstream(path.string()) ;
        if (!input.is_open()){
            throw std::runtime_error("Unable to open art tile overrides: "s+path.string());
        }
        this->loadUOX3Override(input);
    }
    //======================================================================
    auto TileInfo::loadUOX3Override(std::istream &input) ->void {
        auto buffer = std::vector<char>(4096,0);
        auto tilenumber = static_cast<std::uint32_t>(0) ;
        bool inSection = false ;
        while(input.good() && !input.eof()){
            input.getline(buffer.data(),buffer.size()-1);
            if (input.gcount()>0){
                buffer[input.gcount()] = 0 ; // Make a null terminator to end the string
                std::string line = buffer.data() ;
                line = util::trim(util::strip(line,"//"));
                if (!line.empty()){
                    if (line[0] == '[' && line[line.size()-1] == ']'){
                        inSection = false ;
                        // It is a section header
                        auto [header,num] = util::split(util::contentsOf(line, "[", "]")," ");
                        if (util::upper(header)=="TILE"){
                            tilenumber = std::stoul(num,nullptr,0) ;
                        }
                    }
                    else if (line[0] != '{'){
                        inSection = true ;
                    }
                    else if (line[0] != '}'){
                        inSection = false ;
                    }
                    else if (inSection){
                        auto [key,value] = util::split(line,"=");
                        // First we check the attributes
                        if (!checkUOX3Attribute(tilenumber, util::upper(key), value)){
                            checkUOX3Flag(tilenumber, util::upper(key), value);
                        }
                    }
                }
            }
        }
    }
    //======================================================================
    auto TileInfo::checkUOX3Attribute(std::uint32_t tilenumber,const std::string &key, const std::string &value) ->bool {
        auto rvalue = true ;
        if (key == "WEIGHT"){
            artInfo.at(tilenumber).weight = static_cast<std::uint8_t>(std::stoul(value,nullptr,0));
        }
        else if (key == "HEIGHT"){
            artInfo.at(tilenumber).height = static_cast<std::uint8_t>(std::stoul(value,nullptr,0));
        }
        else if (key == "LAYER"){
            artInfo.at(tilenumber).quality = static_cast<std::uint8_t>(std::stoul(value,nullptr,0));
        }
        else if (key == "ANIMATION"){
            artInfo.at(tilenumber).animid = static_cast<std::uint16_t>(std::stoul(value,nullptr,0));
        }
        else if (key == "HUE"){
            artInfo.at(tilenumber).hue = static_cast<std::uint8_t>(std::stoul(value,nullptr,0));
        }
        else if (key == "QUANTITY"){
            artInfo.at(tilenumber).quantity = static_cast<std::uint8_t>(std::stoul(value,nullptr,0));
        }
        else if (key == "NAME"){
            artInfo.at(tilenumber).name = value;
        }
        else {
            rvalue = false ;
        }
        return rvalue ;
    }
    //======================================================================
    auto TileInfo::checkUOX3Flag(std::uint32_t tilenumber,const std::string &key, const std::string &value) ->bool {
        auto rvalue = false ;
        try {
            auto bit = bitForName(key) ;
            auto state = static_cast<bool>(std::stoi(key,nullptr,0));
            artInfo.at(tilenumber).flag.set(bit, state);
            rvalue = true ;
        }
        catch(...){
            // nothing to do here, not a flag
        }
        return rvalue ;
    }

}
