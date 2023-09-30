//

#ifndef artmap_hpp
#define artmap_hpp

#include <array>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>

#include "utility/filemap.hpp"
#include "uotile.hpp"
//======================================================================
namespace uo {
    
    struct TileInfo;
    
    //======================================================================
    //  ArtBlock
    //======================================================================
    struct ArtBlock{
        std::array<std::array<std::vector<UOTile>,8>,8> data ;
        ArtBlock() ;
        ArtBlock(const std::uint8_t *ptr, std::uint64_t length,const TileInfo *info ) ;
        
        
        auto load(const std::uint8_t *ptr, std::uint64_t length,const TileInfo *info ) -> void ;
        auto tileAt(int offsetx,int offsety) const -> const std::vector<UOTile>& ;
        auto tileAt(int offsetx, int offsety) -> std::vector<UOTile> & ;
    };
    //======================================================================
    //  ArtMap
    //======================================================================
    class ArtMap{
        int mapHeight ;
        const TileInfo * info ;
        static const std::vector<UOTile> emptyTile ;
        size_t diffSize = 0 ;
        mutable std::unordered_map<int,std::pair<std::uint64_t,std::uint64_t>> offsets ;
        mutable std::unordered_map<int,ArtBlock> diffBlock ;
        mutable std::unordered_map<int,ArtBlock> previousBlock ;
        
        util::filemap_t stamap ;
        std::uint8_t *data ;
    public:
        ArtMap(const TileInfo * info=nullptr);
        ArtMap(int mapHeight, const TileInfo * info,int mapNumber);
        
        auto load(const std::filesystem::path &idxpath, const std::filesystem::path &mulpath,int mapHeight,int mapNumber) ->bool ;
        auto loadDiff(const std::filesystem::path &difflpath, const std::filesystem::path &diffipath, const std::filesystem::path &diffpath)->bool;
        auto sizeDiff() const ->size_t {return diffSize;}
        auto setTileInfo(const TileInfo *info) ->void ;
        auto tilesAt(int x, int y) const -> const std::vector<UOTile>& ;
    };
    
}

#endif /* artmap_hpp */
