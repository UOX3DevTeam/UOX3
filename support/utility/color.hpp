//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef color_hpp
#define color_hpp

#include <cstdint>
#include <string>
#include <array>

namespace util {
    //=================================================================================
    namespace img {
        enum class colchan{blue=0,green,red,alpha};
        class color_t {
            std::array<std::uint8_t,4> channels ;
            
            auto load(std::uint16_t value) ->void ;
            auto load(std::uint32_t value) ->void ;
            auto convert16() const ->std::uint16_t ;
            auto convert32() const ->std::uint32_t ;
        public:
            color_t(char value) = delete;
            color_t(std::uint8_t value) = delete;
            color_t(std::int8_t value) = delete;
            color_t(std::uint64_t value) = delete;
            color_t(std::int64_t value) = delete;
            
            color_t();
            
            color_t(std::uint16_t value);
            color_t(std::int16_t value);
            color_t(std::uint32_t value);
            color_t(std::int32_t value);
            color_t(std::uint8_t red,std::uint8_t green, std::uint8_t blue,std::uint8_t alpha) ;
            color_t(double red,double green, double blue,double alpha) ;
            
            auto operator=(const char& ) ->color_t& = delete ;
            auto operator=(const std::uint8_t ) ->color_t& = delete ;
            auto operator=(const std::int8_t ) ->color_t& = delete ;
            auto operator=(const std::uint64_t ) ->color_t& = delete ;
            auto operator=(const std::int64_t ) ->color_t& = delete ;
            
            auto operator=(const std::uint16_t value) ->color_t& ;
            auto operator=(const std::int16_t value) ->color_t& ;
            auto operator=(const std::uint32_t value) ->color_t& ;
            auto operator=(const std::int32_t value) ->color_t& ;
            
            auto operator<(const color_t &value) const ->bool ;
            auto operator==(const color_t &value) const ->bool ;
            auto operator!=(const color_t &value) const ->bool ;
            
            auto gray() const ->bool ;
            auto opaque() const ->bool ;
            auto transparent() const ->bool ;
            
            auto operator[](colchan type) const ->const std::uint8_t& ;
            auto operator[](colchan type) -> std::uint8_t& ;
            
            auto hue(color_t* data,bool partial, colchan chan=colchan::red) const ->color_t ;
            
            template <typename T>
            typename std::enable_if_t<std::is_integral_v<T>&& (std::is_same_v<T,std::uint16_t> || std::is_same_v<T,std::int16_t> || std::is_same_v<T,std::uint32_t> || std::is_same_v<T,std::int32_t>),T>  value() const {
                
                if (sizeof(T)==4){
                    return static_cast<T>(convert16());
                }
                return static_cast<T>(convert32());
            }
            
        };
    }
}
#endif /* color_hpp */
