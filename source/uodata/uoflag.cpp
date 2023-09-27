//Copyright © 2023 Charles Kerr. All rights reserved.

#include "uoflag.hpp"

#include <stdexcept>
#include <algorithm>
#include <map>
using namespace std::string_literals ;
namespace uo{
    //======================================================================
    static std::map<std::string,flag_t> NAMEBITMAP {
        {"ATFLOORLEVEL"s,flag_t::FLOORLEVEL},{"HOLDABLE"s,flag_t::HOLDABLE},
        {"SIGNGUILDBANNER"s,flag_t::TRANSPARENT},{"WEBDIRTBLOOD"s,flag_t::TRANSLUCENT},
        {"WALLVERTTILE"s,flag_t::WALL},{"DAMAGING"s,flag_t::DAMAGING},
        {"BLOCKING"s,flag_t::BLOCKING},{"LIQUIDWET"s,flag_t::WET},
        {"UNKNOWNFLAG1"s,flag_t::UNKNOWN1},{"STANDABLE"s,flag_t::SURFACE},
        {"CLIMBABLE"s,flag_t::CLIMBABLE},{"STACKABLE"s,flag_t::STACKABLE},
        {"WINDOWARCHDOOR"s,flag_t::WINDOW},{"CANNOTSHOOTTHRU"s,flag_t::NOSHOOT},
        {"DISPLAYASA"s,flag_t::DISPLAYA},{"DISPLAYASAN"s,flag_t::DISPLAYAN},
        {"DESCRIPTIONTILE"s,flag_t::DESCRIPTION},{"FADEWITHTRANS"s,flag_t::FOLIAGE},
        {"PARTIALHUE"s,flag_t::PARTIALHUE},{"UNKNOWNFLAG2"s,flag_t::UNKNOWN2},
        {"MAP"s,flag_t::MAP},{"CONTAINER"s,flag_t::CONTAINER},
        {"EQUIPABLE"s,flag_t::WEARABLE},{"LIGHTSOURCE"s,flag_t::LIGHT},
        {"ANIMATED"s,flag_t::ANIMATED},{"NODIAGONAL"s,flag_t::NODIAGONAL},
        {"UNKNOWNFLAG3"s,flag_t::UNKNOWN3},{"WHOLEBODYITEM"s,flag_t::ARMOR},
        {"WALLROOFWEAP"s,flag_t::ROOF},{"DOOR"s,flag_t::DOOR},
        {"CLIMBABLEBIT1"s,flag_t::STAIRBACK},{"CLIMBABLEBIT2"s,flag_t::STAIRRIGHT}
    };
    //======================================================================
    auto uobitForName(const std::string &name) -> flag_t {
        auto iter = NAMEBITMAP.find(name) ;
        if (iter == NAMEBITMAP.end()){
            throw std::runtime_error("No flag bit found for: "s+name);
        }
        return iter->second ;
    }

    //=================================================================================
    // Define flagnames
    //=================================================================================
    const std::unordered_map<std::string,unsigned long long> Flag::flagname_masks{
        {"background"s,Flag::BACKGROUND},{"weapon"s,Flag::WEAPON},{"transparent"s,Flag::TRANSPARENT},{"translucent"s,Flag::TRANSLUCENT},
        {"wall"s,Flag::WALL},{"damaging"s,Flag::DAMAGING},{"impassable"s,Flag::IMPASSABLE},{"wet"s,Flag::WET},
        {"bit9"s,Flag::BIT9},{"surface"s,Flag::SURFACE},{"climbable"s,Flag::CLIMBABLE},{"stackable"s,Flag::STACKABLE},
        {"window"s,Flag::WINDOW},{"noshoot"s,Flag::NOSHOOT},{"articlea"s,Flag::ARTICLEA},{"articlean"s,Flag::ARTICLEAN},
        {"articlethe"s,Flag::ARTICLETHE},{"foliage"s,Flag::FOLIAGE},{"partialhue"s,Flag::PARTIALHUE},{"nohouse"s,Flag::NOHOUSE},
        {"map"s,Flag::MAP},{"container"s,Flag::CONTAINER},{"wearable"s,Flag::WEARABLE},{"lightsource"s,Flag::LIGHTSOURCE},
        {"animation"s,Flag::ANIMATION},{"hoverover"s,Flag::HOVEROVER},{"nodiagnol"s,Flag::NODIAGNOL},{"armor"s,Flag::ARMOR},
        {"roof"s,Flag::ROOF},{"door"s,Flag::DOOR},{"stairback"s,Flag::STAIRBACK},{"stairright"s,Flag::STAIRRIGHT},
        {"alphablend"s,Flag::ALPHABLEND},{"usenewart"s,Flag::USENEWART},{"artused"s,Flag::ARTUSED},{"bit36"s,Flag::BIT36},
        {"noshadow"s,Flag::NOSHADOW},{"pixelbleed"s,Flag::PIXELBLEED},{"playanimonce"s,Flag::PLAYANIMONCE},{"bit40"s,Flag::BIT40},
        {"multimovable"s,Flag::MULTIMOVABLE},{"bit42"s,Flag::BIT42},{"bit43"s,Flag::BIT43},{"bit44"s,Flag::BIT44},
        {"bit45"s,Flag::BIT45},{"bit46"s,Flag::BIT46},{"bit47"s,Flag::BIT47},{"bit48"s,Flag::BIT48},
        {"bit49"s,Flag::BIT49},{"bit50"s,Flag::BIT50},{"bit51"s,Flag::BIT51},{"bit52"s,Flag::BIT52},
        {"bit53"s,Flag::BIT53},{"bit54"s,Flag::BIT54},{"bit55"s,Flag::BIT55},{"bit56"s,Flag::BIT56},
        {"bit57"s,Flag::BIT57},{"bit58"s,Flag::BIT58},{"bit59"s,Flag::BIT59},{"bit60"s,Flag::BIT60},
        {"bit61"s,Flag::BIT61},{"bit62"s,Flag::BIT62},{"bit63"s,Flag::BIT63},{"bit64"s,Flag::BIT64}
    };
    //=================================================================================
    // Define Flag::maskForName
    //=================================================================================
    //=================================================================================
    auto Flag::maskForName(const std::string &flag_name) ->unsigned long long {
        auto iter = flagname_masks.find(flag_name) ;
        if (iter != flagname_masks.end()){
            return iter->second ;
        }
        throw std::out_of_range("Flag name: "s+flag_name+" not valid"s);
    }
    
    //=================================================================================
    // Define Flag::flagNameForBit
    //=================================================================================
    //=================================================================================
    auto Flag::flagNameForBit(int bit) ->const std::string& {
        unsigned long long mask = 1 ;
        mask = mask<<(bit-1) ;
        auto iter = std::find_if(flagname_masks.begin(),flagname_masks.end(),[mask](const std::pair<std::string ,unsigned long long> &value){
            return mask == std::get<1>(value);
        });
        if (iter != flagname_masks.end()){
            return iter->first;
        }
        throw std::out_of_range("Bit value not between 1 and 64 inclusive: "s + std::to_string(bit));
    }
    //=================================================================================
    // outputs a header for each flag value
    //=================================================================================
    //=================================================================================
    auto Flag::header(std::ostream &output , const std::string &sep)  ->void{
        for (auto j=1;j<=64;j++){
            output <<flagNameForBit(j);
            if (j!= 64){
                output<<sep;
            }
        }
    }
    //=================================================================================
    // UOX3 compatability
    // test a bit number
    // set a bit number
    //=================================================================================
    //=================================================================================
    auto Flag::test(flag_t bit) const ->bool {
        auto mask = std::uint64_t(1) ;
        mask = mask << static_cast<int>(bit) ;
        return this->value & mask ;
    }
    //=================================================================================
   auto Flag::set(flag_t bit, bool state) ->void {
       auto mask = std::uint64_t(1) ;
       mask = mask << static_cast<int>(bit) ;
       if (state) {
           this->value |= mask ;
       }
       else {
           this->value &= (~mask);
       }
    }

    //=================================================================================
    // Constructor
    //=================================================================================
    //=================================================================================
    Flag::Flag(unsigned long long flag_value):value(flag_value){}
    
    //=================================================================================
    // Constructor
    //=================================================================================
    //=================================================================================
    Flag::Flag(const std::string &value,const std::string &sep):Flag() {
        if (!value.empty()){
            auto data = value ;
            do {
                
                auto end = data.find_first_of(sep);
                if (end != std::string::npos){
                    auto svalue = data.substr(0,end) ;
                    data = data.substr(end+sep.size()) ;
                    this->value |= maskForName(svalue) ;
                }
                else {
                    this->value |= maskForName(data) ;
                    data = "";
                }
            }
            while (!data.empty());
        }
    }
    //=================================================================================
    // Constructor
    //=================================================================================
    //=================================================================================
    Flag::Flag(const std::vector<std::string> &values):Flag(){
        auto bit = 0 ;
        unsigned long long mask = 1 ;
        for (const auto &entry:values){
            auto value = false ;
            if (!entry.empty()){
                value = static_cast<bool>(std::stoi(entry)) ;
            }
            if (value){
                this->value|= (mask<<bit);
            }
            bit++;
        }
    }
    
    //=================================================================================
    // check if the value has the mask.
    //=================================================================================
    //=================================================================================
    auto Flag::hasFlag(unsigned long long mask) const -> bool {
        return mask&value ;
    }
    //=================================================================================
    // Set the mask in the flag.
    //=================================================================================
    //=================================================================================
    auto Flag::setFlag(unsigned long long mask) ->Flag& {
        value |= mask ;
        return *this;
    }
    //=================================================================================
    // Clear the mask in the flag.
    //=================================================================================
    //=================================================================================
    auto Flag::clearFlag(unsigned long long mask) ->Flag& {
        value &= (~mask);
        return *this;
    }
    //=================================================================================
    // Text representation
    //=================================================================================
    //=================================================================================
    auto Flag::description(const std::string &sep) const ->std::string {
        
        unsigned long long bit = 1 ;
        auto text = std::string() ;
        for (auto j=0;j<64;j++) {
            auto mask = bit<<j ;
            if (mask & this->value){
                if (!text.empty()){
                    text += sep ;
                }
                text += flagNameForBit(j+1);
            }
        }
        return text ;
    }
    //=================================================================================
    // outputs a entry for each flag value
    //=================================================================================
    //=================================================================================
    auto Flag::column(std::ostream &output , const std::string &sep) const ->void{
        unsigned long long mask = 0 ;
        for (auto j=0;j<64;j++){
            if (value&(mask<<j)){
                output <<1;
            }
            if (j<(64-1)){
                output<<sep;
            }
        }
    }
    //=================================================================================
    // operator =
    //=================================================================================
    //=================================================================================
    auto Flag::operator=(unsigned long long value) ->Flag& {
        this->value = value;
        return *this;
    }
    //=================================================================================
    auto Flag::operator=(long long value) ->Flag& {
        this->value = static_cast<unsigned long long>(value);
        return *this;
    }
    //=================================================================================
    auto Flag::operator=(unsigned int value) ->Flag& {
        this->value = static_cast<unsigned long long>(value);
        return *this;
    }
    //=================================================================================
    auto Flag::operator=(int value) ->Flag& {
        this->value = static_cast<unsigned long long>(value);
        return *this;
    }
}
