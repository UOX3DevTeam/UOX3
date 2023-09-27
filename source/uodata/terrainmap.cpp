//

#include "terrainmap.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "hash.hpp"
#include "mapsize.hpp"
#include "utility/strutil.hpp"
#include "tileinfo.hpp"

#include "uop.hpp"

using namespace std::string_literals ;

namespace uo {

//======================================================================
//======================================================================
//  TerrainDiff
//======================================================================
    //======================================================================
    TerrainDiff::TerrainDiff() {
        diffEntry.clear();
        data = nullptr ;
    }
    //======================================================================
    auto TerrainDiff::load(const std::filesystem::path &diffList, const std::filesystem::path &diff) ->bool {
        // map the diff
        auto offset = std::uint64_t(0) ;
        auto blockNumber = 0 ;
        
        data = reinterpret_cast<std::uint8_t*>(diffmap.map(diff)) ;
        if (data == nullptr){
            return false ;
        }
        auto input = std::ifstream(diff.string(),std::ios::binary);
        if (!input.is_open()){
            return false ;
        }
        while (input.good() && !input.eof()){
            input.read(reinterpret_cast<char*>(&blockNumber),4) ;
            if (input.gcount()==4){
                diffEntry.insert_or_assign(blockNumber,offset);
                offset += 196 ;
            }
        }
        return true ;
    }
    //======================================================================
    auto TerrainDiff::size() const -> size_t {
        return diffEntry.size();
    }
    //======================================================================
    auto TerrainDiff::dataForBlock(int block) const -> const std::uint8_t* {
        auto iter = diffEntry.find(block) ;
        if (iter != diffEntry.end()){
            return  data + iter->second ;
        }
        return nullptr;
    }

    //======================================================================
    //  TerrainMap
    //======================================================================
    //======================================================================
    const std::string TerrainMap::hashformat = "build/map%ilegacymul/%.08u.dat";
    //======================================================================
    TerrainMap::TerrainMap():mapHeight(0),data(nullptr),valid(false){
        info = nullptr ;

    }
    //======================================================================
    auto TerrainMap::load(const std::filesystem::path &mappath,int mapNumber, int mapHeight, int mapWidth) ->bool {
        
        this->mapHeight = mapHeight;
        if (this->mapHeight == 0){
            this->mapHeight = uo::MapSize::MAPSIZE[mapNumber].height ;
        }
        this->mapWidth = mapWidth;
        if (this->mapWidth == 0){
            this->mapWidth = uo::MapSize::MAPSIZE[mapNumber].width ;
        }
        // First determine if this is a uop file or mul file
        if (!std::filesystem::exists(mappath)){
            return false ;
        }
        // Go head and map this
        data = reinterpret_cast<std::uint8_t *>(datamap.map(mappath));
        if (data == nullptr){
            return false ;
        }
        // Now, are we mul or uop ?
        if (uop::valid(data)){
            auto hashToIDMap = std::map<std::uint64_t, int>() ;
            for (auto i=0 ; i < 300;i++){
                auto hashString = util::format("build/map%ilegacymul/%.08u.dat", mapNumber, i );
                hashToIDMap.insert_or_assign(uop::hashLittle2(hashString),i) ;
            }
            auto offsets = uop::allEntries(data);
            for (const auto &[dummy,entry]:offsets){
                if (entry.valid()){
                    auto iter = hashToIDMap.find(entry.hash) ;
                    if (iter != hashToIDMap.end()){
                        // We found this entry
                        uopOffsets.insert_or_assign(iter->second ,entry.file_offset+entry.header_length) ;
                    }
                }
            }
        }
        valid = true ;
        return true ;
    }
    //======================================================================
    auto TerrainMap::dataForBlock(int block) const -> const std::uint8_t * {
        // first determine if a uop or not (we dont worry about diffs on uop)
        auto offsetdata = static_cast<const std::uint8_t*>(nullptr);
        if (uopOffsets.empty()){
            // This is a mul file
            offsetdata = diff.dataForBlock(block) ;
            if (!offsetdata){
                // we had no diff for this
                offsetdata = data + (block * 196) ;
            }
        }
        else {
            // we need to figure out what uop block this would be
            auto [uopblock,blockoffset] = MapSize::uopBlockFor(block) ;
            offsetdata = data + uopOffsets.at(uopblock) + blockoffset*196 ;
        }
        return offsetdata ;
    }
    //======================================================================
    auto TerrainMap::sizeDiff() const ->size_t {
        return diff.size() ;
    }

    //======================================================================
    auto TerrainMap::setInfo(const TileInfo *info) -> void {
        this->info = info;
    }
    //======================================================================
    auto TerrainMap::tileFor(int x, int y) const -> UOTile {
        if (!valid) {
            throw UODataError("UO terrain map not loaded");
        }
        auto [block,xoffset,yoffset] = MapSize::blockFor(x, y, mapHeight) ;
        auto uodata = dataForBlock(block) ;
        uodata += 4 + yoffset*24 + xoffset*3 ;
        if ((uodata+3) > data + datamap.length) {
            throw UODataError("XY exceeds terrain map data range");
        }
        // Data is now pointing to where we need to be
        auto uotile = UOTile(TERRAIN);
        auto alt = std::int8_t(0) ;
        std::copy(uodata,uodata+2,reinterpret_cast<std::uint8_t*>(&uotile.tileid));
        std::copy(uodata+2,uodata+3,reinterpret_cast<std::uint8_t*>(&alt));
        uotile.altitude = static_cast<int>(alt) ;
        return uotile ;
    }
    //======================================================================
    auto TerrainMap::loadDiff(const std::filesystem::path &diffl, const std::filesystem::path diff) -> bool{
        if (std::filesystem::exists(diffl) && std::filesystem::exists(diff)){
            return this->diff.load(diffl, diff) ;
        }
        return false ;
    }

}


