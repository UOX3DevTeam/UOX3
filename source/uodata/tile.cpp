//

#include "tile.hpp"

#include <iostream>
#include <stdexcept>
#include <algorithm>
using namespace std::string_literals ;
namespace uo{

//=========================================================================================
const std::array<std::string,64> flag_t::flagnames{
        "background"s,"holdable"s,"transparent"s,"translucent"s,
        "wall"s,"damaging"s,"impassable"s,"wet"s,
        "bit9"s,"surface"s,"bridge"s,"generic"s,
        "window"s,"noshoot"s,"article_a"s,"article_an"s,
        "article_the"s,"foilage"s,"paritalhue"s,"nohouse"s,
        "map"s,"container"s,"wearable"s,"lightsource"s,
        "animation"s,"hoverover"s,"nodiagnol"s,"armor"s,
        "roof"s,"door"s,"stairback"s,"stairright"s,
        "alphablend"s,"usenewart"s,"artused"s,"bit36"s,
        "noshadow"s,"pixelbleed"s,"playanimonce"s,"bit40"s,
        "multimovable"s,"bit42"s,"bit43"s,"bit44"s,
        "bit45"s,"bit46"s,"bit47"s,"bit48"s,
        "bit49"s,"bit50"s,"bit51"s,"bit52"s,
        "bit53"s,"bit54"s,"bit55"s,"bit56"s,
        "bit57"s,"bit58"s,"bit59"s,"bit60"s,
        "bit61"s,"bit62"s,"bit63"s,"bit64"s,
    };
    
    
    //=========================================================================================
    auto flag_t::test(std::size_t bit) const ->bool {
        return flag_value.test(bit) ;
    }
    //=========================================================================================
    auto flag_t::set(std::size_t bit,bool value) ->void {
        if (value){
            flag_value.set(bit) ;
        }
        else {
            flag_value.reset(bit) ;
        }
    }
    //=========================================================================================
    auto flag_t::set(std::size_t bit) ->void {
        flag_value.set(bit) ;
    }
    //=========================================================================================
    auto flag_t::clear(std::size_t bit) ->void {
        flag_value.reset(bit);
    }
    //=========================================================================================
    auto flag_t::operator=(std::uint64_t value) ->flag_t& {
        flag_value= value ;
        return *this;
    }
    //=========================================================================================
    auto flag_t::value() const -> std::uint64_t {
        return flag_value.to_ullong() ;
    }
    //=========================================================================================
    auto flag_t::load(std::istream &input,bool is64) -> flag_t&{
        auto value = std::uint64_t(0);
        auto size = 8 ;
        if (!is64){
            size = 4 ;
        }
        input.read(reinterpret_cast<char*>(&value),size);
        flag_value = value ;
        return *this;
    }
    //====================================================================================
    // tile_t
    //====================================================================================
    //=========================================================================================
    auto tile_t::loadName(std::istream &input) ->void {
        constexpr auto info_string_size = 20 ;
        auto buffer = std::array<char,info_string_size+1>() ;
        buffer[info_string_size] = 0 ;
        input.read(buffer.data(),info_string_size) ;
        name = buffer.data();
    }

    //====================================================================================
    // landtile_t
    //====================================================================================
    //====================================================================================
    landtile_t::landtile_t(std::istream &input,bool is64):landtile_t() {
        this->load(input,is64);
    }
    //====================================================================================
    auto landtile_t::load(std::istream &input,bool is64) ->landtile_t&{
        flag.load(input,is64);
        input.read(reinterpret_cast<char*>(&texture),sizeof(texture));
        loadName(input);
        return *this;

    }
    //====================================================================================
    // arttile_t
    //====================================================================================
    arttile_t::arttile_t() : tile_t(),weight(0),quality(0),misc_data(0),unknown2(),quantity(0),animid(0),unknown3(0),hue(0),stacking_offset(),height(0){
        
    }
    //====================================================================================
    arttile_t::arttile_t(std::istream &input,bool is64):arttile_t() {
        this->load(input,is64);
    }
    //====================================================================================
    auto arttile_t::load(std::istream &input,bool is64) ->arttile_t&{
        flag.load(input,is64);
        input.read(reinterpret_cast<char*>(&weight),sizeof(weight));
        input.read(reinterpret_cast<char*>(&quality),sizeof(quality));
        input.read(reinterpret_cast<char*>(&misc_data),sizeof(misc_data));
        input.read(reinterpret_cast<char*>(&unknown2),sizeof(unknown2));
        input.read(reinterpret_cast<char*>(&quantity),sizeof(quantity));
        input.read(reinterpret_cast<char*>(&animid),sizeof(animid));
        input.read(reinterpret_cast<char*>(&unknown3),sizeof(unknown3));
        input.read(reinterpret_cast<char*>(&hue),sizeof(hue));
        input.read(reinterpret_cast<char*>(&stacking_offset),sizeof(stacking_offset));
        input.read(reinterpret_cast<char*>(&height),sizeof(height));
        loadName(input);
        return *this;

    }
    //====================================================================================
    // General methods
    //====================================================================================
    //====================================================================================
    auto uses64BitFlag(const std::filesystem::path &path) ->bool{
        constexpr auto hsSize = 3188736LL ;
        if (!std::filesystem::exists(path)){
            throw std::runtime_error("Tiledata mul not located: "s+path.string());
        }
        return std::filesystem::file_size(path) >= hsSize;
    }

}
