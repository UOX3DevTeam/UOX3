//o------------------------------------------------------------------------------------------------o
//|	File		-	regions.h
//o------------------------------------------------------------------------------------------------o
//|	Notes		-	Region class  Added 3/6/1999 To lookup items by region
//|						This should help for now. BTW- my first attempt at C++
//|						So forgive any newbie mistakes :)
//|					Side note, I wanted regions to be more generic, but
//|						now that I have to do this, time doesn't allow for it yet.
//o------------------------------------------------------------------------------------------------o

#ifndef __Region_h
#define __Region_h

#include <algorithm>
#include <memory>
#include <vector>
#include <utility>

#include "mapstuff.h"

constexpr auto MapColSize = static_cast<std::int16_t>(32);
constexpr auto MapRowSize = static_cast<std::int16_t>(128);

constexpr auto UpperX = static_cast<std::int16_t>( 7168 / MapColSize );
constexpr auto UpperY = static_cast<std::int16_t>( 4096 / MapRowSize );

//========================================================================
struct MapResource_st
{
    std::int16_t	oreAmt;
    std::uint32_t	oreTime;
    std::int16_t	logAmt;
    std::uint32_t	logTime;
    std::int16_t	fishAmt;
    std::uint32_t	fishTime;
    
    MapResource_st(std::int16_t defOre = 0, std::int16_t defLog = 0, std::int16_t defFish = 0, std::uint32_t defOreTime = 0, std::uint32_t defLogTIme = 0, std::uint32_t defFishTIme=0) : oreAmt( defOre ), oreTime( defOreTime ), logAmt( defLog ), logTime( defLogTIme ), fishAmt( defFish ), fishTime( defFishTIme ) {
    }
};

//========================================================================
class CMapRegion {
private:
    GenericList<CItem *>	itemData;
    GenericList<CChar *>	charData;
    RegionSerialList 		regionSerialData;
    bool hasRegionChanged = false;
    
public:
    auto GetItemList() -> GenericList<CItem *> *;
    auto GetCharList() ->GenericList<CChar *> * ;
    auto GetRegionSerialList()->RegionSerialList* ;
    auto HasRegionChanged() const -> bool;
    auto HasRegionChanged( bool newVal ) ->void;
    
    CMapRegion()=default;
    ~CMapRegion()=default ;
    
    auto SaveToDisk( std::ostream& writeDestination ) ->void;
    auto SaveToVectors() -> std::vector<std::vector<std::pair<std::string,std::string>>>;
};
//========================================================================
class CMapWorld {
private:
    std::int16_t							upperArrayX;
    std::int16_t							upperArrayY;
    std::uint16_t							resourceX;
    std::uint16_t							resourceY;
    std::vector<CMapRegion>			mapRegions;
    std::vector<MapResource_st>		mapResources;
public:
    CMapWorld();
    CMapWorld( std::uint8_t worldNum );
    ~CMapWorld()=default;
    
    auto GetMapRegion( std::int16_t xOffset, std::int16_t yOffset ) ->CMapRegion * ;
    auto GetMapRegions()->std::vector<CMapRegion> * ; // Why isn't this a refernce return?
    
    MapResource_st&	GetResource( std::int16_t x, std::int16_t y );
    
    auto LoadResources( std::uint8_t worldNum ) ->void;
    auto SaveResources( std::uint8_t worldNUm ) ->void ;
};

//========================================================================
class CMapHandler {
private:
    
    std::vector<std::unique_ptr<CMapWorld>>	 mapWorlds;
    CMapRegion		overFlow;
    
    auto LoadFromDisk( std::istream& readDestination, SI32 baseValue, SI32 fileSize, std::uint32_t maxSize ) ->void ;
public:
    CMapHandler() = default;
    ~CMapHandler() = default;
    
    auto Startup() -> void;
    auto SaveTest() ->void ;
    auto Save() -> void;
    auto Load() -> void;
    
    auto AddItem( CItem *nItem ) ->bool;
    auto RemoveItem( CItem *nItem ) ->bool;
    
    auto AddChar( CChar *toAdd ) -> bool;
    auto RemoveChar( CChar *toRemove )->bool;
    
    auto ChangeRegion( CItem *nItem, std::int16_t x, std::int16_t y, std::uint8_t worldNum )->bool;
    auto ChangeRegion( CChar *nChar, std::int16_t x, std::int16_t y, std::uint8_t worldNum )->bool;
    
    auto GetMapRegion( CBaseObject *mObj ) ->CMapRegion *;
    auto GetMapRegion( std::int16_t xOffset, std::int16_t yOffset, std::uint8_t worldNumber )->CMapRegion *;
    
    auto GetGridX( std::int16_t x )->std::int16_t;
    auto GetGridY( std::int16_t y ) ->std::int16_t;
    
    auto PopulateList( std::int16_t x, std::int16_t y, std::uint8_t worldNumber ) -> std::vector<CMapRegion *>;
    auto PopulateList( CBaseObject *mObj ) -> std::vector<CMapRegion *>;
    
    auto GetMapWorld( std::uint8_t worldNum ) ->CMapWorld *  ;
    
    auto GetResource( std::int16_t x, std::int16_t y, std::uint8_t worldNum )->MapResource_st* ;
};

extern CMapHandler *MapRegion;

#endif

