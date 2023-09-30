//

#include "uomgr.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "utility/strutil.hpp"
using namespace std::string_literals ;

//======================================================================
namespace uo {
    
    //======================================================================
    auto UOMgr::loadDefinition(const std::filesystem::path &uodir, const DFNDef &def,bool applyDiff,bool log)->void{
        if (def.number <= world.size()){
            world.resize(def.number+1) ;
        }
        world.at(def.number).setInfo(&tileInfo);
        world.at(def.number).setValid(true);
        if (log){
            std::clog << "Processing UO map " << def.number<<" data ";
        }
        auto status = false ;
        auto file = uodir/def.mulpath ;
        if (std::filesystem::exists(uodir/def.uoppath)) {
            file = uodir/def.uoppath ;
        }
        
        status = world.at(def.number).loadTerrain(def.number,file , log, def.height, def.width);
        if (!status) {
            if (log){
                std::clog <<"- FAILED\n";
            }
            world.at(def.number) = UOMap(&tileInfo);
            return;
        }
        status = world.at(def.number).loadArt(uodir/def.staidxpath, uodir/def.stamulpath, def.height, def.number);
        if (!status) {
            if (log){
                std::clog <<"- FAILED\n";
            }
            world.at(def.number) = UOMap(&tileInfo);
            return;
        }
        // do we apply diffs?
        if (applyDiff) {
            if (!def.mapdiffpath.empty() && !def.mapdifflpath.empty()){
                world.at(def.number).loadTerrainDiff(uodir/def.mapdifflpath, uodir/def.mapdiffpath) ;
            }
            if (!def.stadiffpath.empty() && !def.stadifflpath.empty()  && def.stadiffipath.empty()){
                world.at(def.number).loadArtDiff(uodir/def.stadifflpath, uodir/def.stadiffipath, uodir/def.stadiffpath) ;
            }
        }
        if (log) {
            std::clog <<"- Completed\n";
        }
    }
    
    //======================================================================
    auto UOMgr::loadDefinition(const std::filesystem::path &uodir, const std::filesystem::path &mapdfn,bool applyDiff,bool log) ->void {
        world.clear() ;
        auto input = std::ifstream((mapdfn/std::filesystem::path("maps.dfn")).string());
        if (!input.is_open()){
            throw std::runtime_error("Unable to process uo map information: "s + (mapdfn/std::filesystem::path("maps.dfn")).string());
        }
        auto buffer = std::vector<char>(4096,0) ;
        auto dfnData = DFNDef() ;
        auto inSection = false ;
        while (input.good() && !input.eof()){
            input.getline(buffer.data(),buffer.size()-1);
            if (input.gcount()>0){
                buffer[input.gcount()] = 0 ;
                std::string line = buffer.data() ;
                line = util::trim(util::strip(line,"//"));
                if (!line.empty()){
                    if (line[0] == '[' && line[line.size()-1]==']'){
                        // Section header
                        auto [key,number] = util::split(util::contentsOf(line, "[", "]")," ") ;
                        if (util::upper(key) == "MAP"){
                            dfnData = DFNDef(std::stoi(number,nullptr,0));
                            inSection = true ;
                        }
                    }
                    else if (inSection){
                        if (line[0] == '}'){
                            inSection = false ;
                            
                            loadDefinition(uodir, dfnData, applyDiff, log);
                        }
                        else if (line[0] != '{'){
                            auto [lkey,value] = util::split(line,"=") ;
                            auto key = util::upper(lkey) ;
                            if (key == "MAP"){
                                dfnData.mulpath = std::filesystem::path(value) ;
                            }
                            else if (key == "MAPUOPWRAP") {
                                dfnData.uoppath = std::filesystem::path(value) ;
                            }
                            else if (key == "STATICS") {
                                dfnData.stamulpath = std::filesystem::path(value) ;
                            }
                            else if (key == "STAIDX") {
                                dfnData.staidxpath = std::filesystem::path(value) ;
                            }
                            else if (key == "MAPDIFF") {
                                dfnData.mapdiffpath = std::filesystem::path(value) ;
                            }
                            else if (key == "MAPDIFFLIST") {
                                dfnData.mapdifflpath = std::filesystem::path(value) ;
                            }
                            else if (key == "STATICSDIFF") {
                                dfnData.stadiffpath = std::filesystem::path(value) ;
                            }
                            else if (key == "STATICSDIFFLIST") {
                                dfnData.stadifflpath = std::filesystem::path(value) ;
                            }
                            else if (key == "STATICSDIFFINDEX") {
                                dfnData.stadiffipath = std::filesystem::path(value) ;
                            }
                            else if (key == "X") {
                                dfnData.width = std::stoi(value,nullptr,0) ;
                            }
                            else if (key == "Y") {
                                dfnData.height =std::stoi(value,nullptr,0) ;
                            }
                        }
                    }
                    
                }
            }
        }
        if (world.empty()){
            throw std::runtime_error("Did not successfully load any UO maps");
        }
    }
    
    //======================================================================
    auto UOMgr::load(const std::filesystem::path &uodir, const std::filesystem::path &mapdef,bool log,bool applyDiff)->void {
        // First we load the tiledata
        if (log){
            std::clog <<"Loading UO tile information";
        }
        if (!tileInfo.load(uodir/std::filesystem::path("tiledata.mul"))){
            
            throw std::runtime_error("Unable to open: "s + (uodir/std::filesystem::path("tiledata.mul")).string()) ;
        }
        if (log){
            std::clog <<" - Terrain: "<<tileInfo.terrainInfo.size()<<" Art: " << tileInfo.artInfo.size()<<"\n";
        }
        // Load the overrides
        tileInfo.loadUOX3Override(mapdef/std::filesystem::path("tiles.dfn"));
        // Now load the maps
        loadDefinition(uodir,mapdef,applyDiff,log) ;
        // And load the multi s
        auto status = false ;
        if (log) {
            std::clog <<"Loading Multi data";
        }
        if (std::filesystem::exists(uodir/std::filesystem::path("MultiCollection.uop"))){
            status = multiCollection.load(uodir/std::filesystem::path("MultiCollection.uop"), &tileInfo);
        }
        if (!status){
            status = multiCollection.load(uodir/std::filesystem::path("multi.idx"),uodir/std::filesystem::path("multi.mul"), &tileInfo);
            
        }
        if (log) {
            if (status){
                std::clog << " - Entries: " << multiCollection.size()<<"\n";
            }
            else {
                std::clog <<"- FAILED\n" ;
            }
        }
        
    }
    //======================================================================
    auto UOMgr::terrainSize() const -> size_t {
        return tileInfo.terrainInfo.size();
    }
    //======================================================================
    auto UOMgr::terrain(size_t tileid) const -> const TerrainInfo& {
        return tileInfo.terrainInfo.at(tileid) ;
    }
    //======================================================================
    auto UOMgr::terrain(size_t tileid)  ->  TerrainInfo& {
        return tileInfo.terrainInfo.at(tileid) ;
    }
    //======================================================================
    auto UOMgr::artSize() const -> size_t {
        return tileInfo.artInfo.size();
    }
    //======================================================================
    auto UOMgr::art(size_t tileid) const -> const ArtInfo& {
        return tileInfo.artInfo.at(tileid) ;
    }
    //======================================================================
    auto UOMgr::art(size_t tileid)  ->  ArtInfo& {
        return tileInfo.artInfo.at(tileid) ;
    }
    // World information
    //======================================================================
    auto UOMgr::artTileAt(int world, int x, int y) const ->const std::vector<UOTile>& {
        if (world < this->world.size()){
            return this->world.at(world).tilesAt(x, y);
        }
        throw std::runtime_error("No world information for: "+std::to_string(world));
    }
    //======================================================================
    auto UOMgr::terrainTileAt(int world, int x, int y) const -> UOTile {
        if (world < this->world.size()){
            return this->world.at(world).tileTerrainFor(x,y) ;
        }
        throw std::runtime_error("No world information for: "+std::to_string(world));
        
    }
    //======================================================================
    auto UOMgr::existMap(int mapnumber) const ->bool {
        if (mapnumber <world.size()){
            return world.at(mapnumber).isValid() ; ;
        }
        return false ;
    }
    //======================================================================
    auto UOMgr::validLocation(int x,int y,int mapnumber) const ->bool {
        if (mapnumber<world.size()) {
            return world.at(mapnumber).validLocation(x, y);
        }
        return false ;
    }
    //======================================================================
    auto UOMgr::sizeOfMap(int mapnumber) const -> std::pair<int,int> {
        if (mapnumber < this->world.size()){
            return this->world.at(mapnumber).size();
        }
        return std::make_pair(0,0);
    }
    //======================================================================
    auto UOMgr::size() const -> size_t {
        return this->world.size();
    }
    //======================================================================
    auto UOMgr::diffCountForMap(int mapnumber) const ->std::pair<int,int> {
        if (mapnumber < this->world.size()){
            return std::make_pair(static_cast<int>(this->world.at(mapnumber).sizeArtDiff()),static_cast<int>(this->world.at(mapnumber).sizeTerrainDiff()));
        }
        return std::make_pair(0,0);
    }
    
    //======================================================================
    auto UOMgr::sizeMulti() const-> size_t {
        return multiCollection.size();
    }
    //======================================================================
    auto UOMgr::multiFor(int multiid) const -> const MultiEntry& {
        return multiCollection.entryFor(multiid);
    }
    //======================================================================
    auto UOMgr::multiExists(int multiid)const ->bool {
        return multiCollection.exists(multiid) ;
    }
}
