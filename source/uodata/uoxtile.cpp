//

#include "uoxtile.hpp"

#include <iostream>
#include <stdexcept>
#include <fstream>

using namespace std::string_literals ;

//=========================================================================================

//=========================================================================================
// CTile
//=========================================================================================

//=========================================================================================
CTile::CTile():uo::arttile_t(){
    
}
//=========================================================================================
CTile::CTile(std::istream &input,bool is64):uo::arttile_t(input,is64){
    
}

//=========================================================================================
auto CTile::Unknown1() const -> std::uint16_t {
    return misc_data;
}
//=========================================================================================
auto CTile::Unknown2() const -> std::uint8_t {
    return  unknown2;
}
//=========================================================================================
auto CTile::Unknown3() const -> std::uint8_t{
    return unknown3;
}
//=========================================================================================
auto CTile::Unknown4() const -> std::uint8_t {
    return static_cast<std::uint8_t>(stacking_offset&0xFF);
}
//=========================================================================================
auto CTile::Unknown5() const -> std::uint8_t {
    return static_cast<std::uint8_t>(((stacking_offset>>8)&0xFF));

}
//=========================================================================================
auto CTile::Hue() const -> std::uint8_t {
    return hue;
}
//=========================================================================================
auto CTile::Quantity() const -> std::uint8_t {
    return quantity;
}
//=========================================================================================
auto CTile::Animation() const -> std::uint16_t {
    return animid;
}
//=========================================================================================
auto CTile::Weight() const -> std::uint8_t{
    return weight;
}
//=========================================================================================
auto CTile::Layer() const -> std::int8_t {
    return quality;
}
//=========================================================================================
auto CTile::Height() const -> std::int8_t {
    return static_cast<std::int8_t>(height);
}
//=========================================================================================
auto CTile::ClimbHeight( bool trueHeight) const -> std::int8_t{
    if( flag.test( TF_CLIMBABLE ) && !trueHeight )
    {
        return static_cast<std::int8_t>(height) / 2;
    }
    return static_cast<std::int8_t>(height);
}
//=========================================================================================
auto CTile::Unknown1( std::uint16_t newVal ) ->void {
    misc_data = newVal;
}
//=========================================================================================
auto CTile::Unknown2( std::uint8_t newVal ) ->void {
    unknown2 = newVal;
}
//=========================================================================================
auto CTile::Unknown3( std::uint8_t newVal ) ->void {
    unknown3 = newVal;
}
//=========================================================================================
auto CTile::Unknown4( std::uint8_t newVal ) ->void {
    stacking_offset = ((stacking_offset & 0xFF00)|newVal) ;
}
//=========================================================================================
auto CTile::Unknown5( std::uint8_t newVal ) ->void {
    stacking_offset = ((stacking_offset&0xFF) | (static_cast<std::uint16_t>(newVal)<<8));
}
//=========================================================================================
auto CTile::Animation( std::uint16_t newVal ) ->void {
    animid = newVal;
}
//=========================================================================================
auto CTile::Weight( std::uint8_t newVal ) ->void {
    weight = newVal;
}
//=========================================================================================
auto CTile::Layer( std::int8_t newVal ) ->void {
    quality = newVal;
}
//=========================================================================================
auto CTile::Height( std::int8_t newVal ) ->void {
    height = static_cast<std::uint8_t>(newVal);
}
//=========================================================================================
auto CTile::Hue( std::uint8_t newVal ) ->void {
    hue = newVal;
}
//=========================================================================================
auto CTile::Quantity( std::uint8_t newVal ) ->void {
    quantity = newVal;
}

//=========================================================================================
// CLand
//=========================================================================================

//=========================================================================================
CLand::CLand():uo::landtile_t(){
    
}
//=========================================================================================
CLand::CLand(std::istream &input,bool is64):uo::landtile_t(input,is64){
    
}

//=========================================================================================
auto CLand::TextureId() const -> std::uint16_t{
    return texture;
}
//=========================================================================================
auto CLand::TextureId( std::uint16_t newVal ) -> void{
    texture = newVal;
}
//=========================================================================================
auto CLand::IsRoadId() const -> bool{
    auto rValue = false;
    for( std::size_t j = 0; j < roadIds.size(); j += 2 )
    {
        if( texture >= roadIds[j] && texture <= roadIds[j + 1] )
        {
            rValue = true;
            break;
        }
    }
    return rValue;

}

//=========================================================================================
// TileInfo
//=========================================================================================

//=========================================================================================
auto TileInfo::processTerrain( std::istream &input,bool is64) -> void {
    for (auto i = 0 ; i < 0x4000;i++){
        // We need to know when to skip header
        if (is64){
            if(((( i & 0x1F ) == 0 ) && ( i > 0 )) || ( i == 1 ))
            {
                input.seekg(4,std::ios::cur);
            }
        }
        else
        {
            if(( i & 0x1f ) == 0 )
            {
                input.seekg(4,std::ios::cur);
            }
        }
        terrainData[i]= CLand(input,is64);
    }
}
//=========================================================================================
auto TileInfo::processArt(std::istream &input,bool is64) -> void {
    for (auto i = 0 ; i < 0xFFFF;i++){
        // We need to know when to skip header
        if(( i & 0x1f ) == 0 ) {
            input.seekg(4,std::ios::cur);
        }
        artData[i]= CTile(input,is64);
    }

}
//=========================================================================================
TileInfo::TileInfo(){
    terrainData.resize(0x4000);
    artData.resize(0xFFFF);
}
//=========================================================================================
TileInfo::TileInfo( const std::string &filename):TileInfo(){
    if (!filename.empty()){
        LoadTiles(filename);
    }
}
//=========================================================================================
auto TileInfo::LoadTiles( const std::string &filename ) -> bool{
    auto rvalue = false ;
    try{
        auto is64 = uo::uses64BitFlag(std::filesystem::path(filename));
        auto input = std::ifstream(filename,std::ios::binary);
        if (input.is_open()){
            rvalue = true ;
            processTerrain(input, is64);
            processArt(input, is64);
        }
    }
    catch(const std::exception &e){
        //std::cerr << e.what()<<std::endl;
        rvalue = false ;
    }
    return rvalue ;
}
//=========================================================================================
auto TileInfo::TerrainInfo( std::uint16_t tileId ) const -> const CLand&{
    return terrainData.at(tileId);
}
//=========================================================================================
auto TileInfo::TerrainInfo( std::uint16_t tileId ) -> CLand&{
    return terrainData.at(tileId);

}
//=========================================================================================
auto TileInfo::ArtInfo( std::uint16_t tileId ) -> CTile&{
    return artData.at(tileId);
}
//=========================================================================================
auto TileInfo::ArtInfo( std::uint16_t tileId ) const -> const CTile&{
    return artData.at(tileId);
}

//=========================================================================================
auto TileInfo::SizeTerrain() const -> std::size_t{
    return terrainData.size()*sizeof(CLand);
}
//=========================================================================================
auto TileInfo::SizeArt() const -> std::size_t{
    return artData.size()*sizeof(CTile);
}

//=========================================================================================
auto TileInfo::CollectionTerrain() const -> const std::vector<CLand>&{
    return terrainData;
}
//=========================================================================================
auto TileInfo::CollectionTerrain() -> std::vector<CLand>&{
    return terrainData;
}
//=========================================================================================
auto TileInfo::CollectionArt() const -> const std::vector<CTile>&{
    return artData;
}
//=========================================================================================
auto TileInfo::CollectionArt() -> std::vector<CTile>&{
    return artData;
}
//=========================================================================================
auto TileInfo::TotalMemory() const -> size_t{
    return (artData.size()*sizeof(CTile))+ (terrainData.size()*sizeof(CLand)) ;
}
