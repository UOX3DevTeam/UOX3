//

#include "artmap.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>

#include "mapsize.hpp"
#include "tileinfo.hpp"
#include "uotile.hpp"

using namespace std::string_literals ;

//======================================================================
namespace uo {
    //======================================================================
    //  ArtBlock
    //======================================================================
    //        std::array<std::array<std::vector<UOTile>,8>,8> data ;
    //        TileInfo *info ;
    
    //======================================================================
    ArtBlock::ArtBlock() {
        auto temp = std::vector<UOTile>() ;
        auto temparray =std::array<std::vector<UOTile>,8>();
        std::fill(data.begin(),data.end(),temparray);
    }
    //======================================================================
    ArtBlock::ArtBlock(const std::uint8_t *ptr, std::uint64_t length,const TileInfo *info ):ArtBlock(){
        load(ptr,length,info) ;
    }
    
    
    //======================================================================
    auto ArtBlock::load(const std::uint8_t *ptr, std::uint64_t length,const TileInfo *info ) -> void {
        auto processed = std::uint64_t(0) ;
        auto offsetx = std::uint8_t(0) ;
        auto offsety = std::uint8_t(0) ;
        auto alt = std::int8_t(0) ;
        auto tile = UOTile(ART) ;
        while (processed < length){
            std::copy(ptr + processed,ptr + processed+2,reinterpret_cast<std::uint8_t*>(&tile.tileid));
            if (info != nullptr){
                tile.info = &info->artInfo.at(tile.tileid);
            }
            processed += 2 ;
            std::copy(ptr + processed,ptr + processed+1,reinterpret_cast<std::uint8_t*>(&offsetx));
            processed += 1 ;
            std::copy(ptr + processed,ptr + processed+1,reinterpret_cast<std::uint8_t*>(&offsety));
            processed += 1 ;
            std::copy(ptr + processed,ptr + processed+1,reinterpret_cast<std::uint8_t*>(&alt));
            tile.altitude = static_cast<int>(alt) ;
            processed += 1 ;
            std::copy(ptr + processed,ptr + processed+2,reinterpret_cast<std::uint8_t*>(&tile.hue));
            processed += 2 ;
            
            data.at(offsetx).at(offsety).push_back(tile) ;
        }
    }
    //======================================================================
    auto ArtBlock::tileAt(int offsetx,int offsety) const -> const std::vector<UOTile>& {
        return data.at(offsetx).at(offsety);
    }
    //======================================================================
    auto ArtBlock::tileAt(int offsetx, int offsety) -> std::vector<UOTile> & {
        return data.at(offsetx).at(offsety);
    }
    
    //======================================================================
    //  ArtMap
    //======================================================================
    
    const std::vector<UOTile> ArtMap::emptyTile = std::vector<UOTile>()  ;
    //======================================================================
    ArtMap::ArtMap(const TileInfo * info){
        this->info = info ;
        this->mapHeight = 0 ;
        data = nullptr ;
        
    }
    //======================================================================
    ArtMap::ArtMap(int mapHeight, const TileInfo * info,int mapNumber):ArtMap(info){
        this->mapHeight = mapHeight ;
        if (mapHeight == 0){
            this->mapHeight = MapSize::MAPSIZE.at(mapNumber).height ;
        }
    }
    //======================================================================
    auto ArtMap::load(const std::filesystem::path &idxpath, const std::filesystem::path &mulpath,int mapHeight,int mapNumber) ->bool {
        this->mapHeight = mapHeight ;
        if (mapHeight == 0){
            this->mapHeight = MapSize::MAPSIZE.at(mapNumber).height ;
        }
        
        auto idx = std::ifstream(idxpath.string(),std::ios::binary) ;
        if (!idx.is_open()){
            return false ;
        }
        data = reinterpret_cast<std::uint8_t *>(stamap.map(mulpath));
        if (data == nullptr){
            return false ;
        }
        auto blocknum = 0 ;
        auto blkoffset = std::uint32_t(0) ;
        auto blklength = std::uint32_t(0) ;
        auto extra = std::uint32_t(0) ;
        while (idx.good() && !idx.eof()){
            idx.read(reinterpret_cast<char*>(&blkoffset),4);
            idx.read(reinterpret_cast<char*>(&blklength),4);
            idx.read(reinterpret_cast<char*>(&extra),4);
            if (idx.gcount() == 4) {
                // We use 0x0xFFFFFFFE versus 0x0xFFFFFFFF due to some older corrupted modified staidx
                // that is also why we dont use the extra, it also indicates if there is a block here or not
                if (blkoffset < 0xFFFFFFFE && (blklength != 0) && (blklength < 0xFFFFFFFE)){
                    // This is a block that has some data, so we will stick it in the offsets
                    offsets.insert_or_assign(blocknum,std::make_pair(static_cast<std::uint64_t>(blkoffset),static_cast<std::uint64_t>(blklength))) ;
                }
                blocknum++ ;
            }
        }
        return true ;
    }
    //======================================================================
    auto ArtMap::loadDiff(const std::filesystem::path &difflpath, const std::filesystem::path &diffipath, const std::filesystem::path &diffpath)->bool{
        if (!std::filesystem::exists(difflpath) || !std::filesystem::exists(diffipath) || !std::filesystem::exists(diffpath)){
            return false ;
        }
        auto diffl = std::ifstream(difflpath.string(),std::ios::binary) ;
        if (!diffl.is_open()){
            return false ;
        }
        auto diffi = std::ifstream(diffipath.string(),std::ios::binary) ;
        if (!diffi.is_open()){
            return false ;
        }
        auto diff = std::ifstream(diffpath.string(),std::ios::binary) ;
        if (!diff.is_open()){
            return false ;
        }
        auto blocknum = std::int32_t(0) ;
        
        auto blkoffset = std::uint32_t(0) ;
        auto blklength = std::uint32_t(0);
        auto extra = std::uint32_t(0) ;
        diffSize = 0 ;
        while (diffl.good() && !diffl.eof()){
            diffl.read(reinterpret_cast<char*>(&blocknum),4);
            if (diffl.gcount() == 4){
                diffi.read(reinterpret_cast<char*>(&blkoffset),4);
                diffi.read(reinterpret_cast<char*>(&blklength),4);
                diffi.read(reinterpret_cast<char*>(&extra),4);
                if (diffi.gcount() == 4){
                    diffSize++ ;
                    // We use 0x0xFFFFFFFE versus 0x0xFFFFFFFF due to some older corrupted modified staidx
                    // that is also why we dont use the extra, it also indicates if there is a block here or not
                    if (blkoffset < 0xFFFFFFFE && (blklength != 0) && (blklength < 0xFFFFFFFE)){
                        // This is a block that has some data, we will read the data, and go for it
                        diff.seekg(blkoffset,std::ios::beg) ;
                        auto temp = std::vector<std::uint8_t>(blklength,0);
                        diff.read(reinterpret_cast<char*>(temp.data()),temp.size());
                        diffBlock.insert_or_assign(blocknum,ArtBlock(temp.data(), static_cast<std::uint64_t>(temp.size()), this->info));
                    }
                    else {
                        // We need to remove the block from the offsets
                        auto iter = offsets.find(blocknum) ;
                        if (iter != offsets.end()){
                            offsets.erase(iter) ;
                        }
                    }
                }
            }
        }
        return true ;
        
    }
    //======================================================================
    auto ArtMap::setTileInfo(const TileInfo *info) ->void {
        this->info = info ;
    }
    //======================================================================
    auto ArtMap::tilesAt(int x, int y) const -> const std::vector<UOTile>& {
        //first get the block and offset
        auto [blocknum,xoff,yoff] = MapSize::blockFor(x, y, this->mapHeight);
        auto iter = previousBlock.find(blocknum) ;
        if (iter != previousBlock.end()){
            // We all read got this block, lets just return what we need
            return iter->second.tileAt(xoff, yoff);
        }
        // It didn't exist, so lets see if it is in our diffs
        auto diter = diffBlock.find(blocknum) ;
        if (diter != diffBlock.end()){
            // We should move this
            previousBlock.insert_or_assign(blocknum,diter->second) ;
            return diter->second.tileAt(xoff, yoff);
        }
        // Ok, so maybe we haven't touchd this block
        auto oiter = offsets.find(blocknum) ;
        if (oiter!= offsets.end()){
            previousBlock.insert_or_assign(blocknum,ArtBlock(data + oiter->second.first,static_cast<std::uint64_t>(oiter->second.second),info)) ;
            // We get it again, because we are passing reference, and this is the one that is hanging around
            iter = previousBlock.find(blocknum) ;
            return iter->second.tileAt(xoff, yoff);
        }
        return emptyTile ;
    }
    
}

