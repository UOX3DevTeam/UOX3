//Copyright © 2023 Charles Kerr. All rights reserved.

#include "color.hpp"

#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std::string_literals;
namespace util {
    //=================================================================================
    namespace img {
        
        //=================================================================================
        auto color_t::load(std::uint16_t value) ->void {
            constexpr auto shift = 5 ;
            constexpr auto mask = 0x1f ;
            for (auto j = 0 ; j<3;j++){
                channels[j] = static_cast<std::uint8_t>(std::lround((((value>>(j*shift))&mask) * 255.0)/31.0));
            }
            channels[3] = 0;
            if (value & 0x8000){
                channels[3] = 255 ;
            }
        }
        //=================================================================================
        auto color_t::load(std::uint32_t value) ->void {
            constexpr auto shift = 8 ;
            constexpr auto mask = 0xff ;
            for (auto j = 0 ; j<4;j++){
                channels[j] = (value>>(j*shift))&mask;
            }
        }
        
        //=================================================================================
        auto color_t::convert16() const ->std::uint16_t {
            auto value = std::uint16_t(0);
            constexpr auto shift = 5 ;
            
            for (auto j=0 ; j<3;j++){
                auto temp = (channels[j] * 31.0)/255.0;
                value|= ((temp<0.9?0:lround(temp))<<(j*shift));
            }
            if (channels[static_cast<int>(colchan::alpha)]!=0){
                value |= 0x8000;
            }
            return value ;
        }
        //=================================================================================
        auto color_t::convert32() const ->std::uint32_t {
            auto value = std::uint32_t(0);
            constexpr auto shift = 8 ;
            
            for (auto j=0 ; j<8;j++){
                
                value|= channels[j]<<(j*shift);
            }
            return value ;
        }
        
        //=================================================================================
        color_t::color_t() {
            std::fill(channels.begin(),channels.end(),0);
        }
        //=================================================================================
        color_t::color_t(std::uint16_t value):color_t(){
            load(value) ;
        }
        //=================================================================================
        color_t::color_t(std::int16_t value):color_t(static_cast<std::uint16_t>(value)){
        }
        //=================================================================================
        color_t::color_t(std::uint32_t value):color_t(){
            load(value);
        }
        //=================================================================================
        color_t::color_t(std::int32_t value):color_t(static_cast<std::uint32_t>(value)){
        }
        //=================================================================================
        color_t::color_t(std::uint8_t red,std::uint8_t green, std::uint8_t blue,std::uint8_t alpha):color_t(){
            channels[static_cast<int>(colchan::red)] = red ;
            channels[static_cast<int>(colchan::green)] = green ;
            channels[static_cast<int>(colchan::blue)] = blue ;
            channels[static_cast<int>(colchan::alpha)] = alpha ;
        }
        //=================================================================================
        color_t::color_t(double red,double green, double blue,double alpha):color_t(){
            channels[static_cast<int>(colchan::red)] = static_cast<std::uint8_t>(std::lround(red*255.0)) ;
            channels[static_cast<int>(colchan::green)] = static_cast<std::uint8_t>(std::lround(green*255.0))  ;
            channels[static_cast<int>(colchan::blue)] = static_cast<std::uint8_t>(std::lround(blue*255.0))  ;
            channels[static_cast<int>(colchan::alpha)] = static_cast<std::uint8_t>(std::lround(alpha*255.0))  ;
        }
        
        //=================================================================================
        auto color_t::operator=(const std::uint16_t value) ->color_t& {
            load(value);
            return *this;
        }
        //=================================================================================
        auto color_t::operator=(const std::int16_t value) ->color_t& {
            return operator=(static_cast<const std::uint16_t>(value));
        }
        //=================================================================================
        auto color_t::operator=(const std::uint32_t value) ->color_t& {
            load(value);
            return *this;
        }
        //=================================================================================
        auto color_t::operator=(const std::int32_t value) ->color_t& {
            return operator=(static_cast<const std::uint32_t>(value));
            
        }
        
        //=================================================================================
        auto color_t::operator<(const color_t &value) const ->bool {
            return this->value<std::uint32_t>() < value.value<std::uint32_t>() ;
        }
        //=================================================================================
        auto color_t::operator==(const color_t &value) const ->bool {
            auto rvalue = true ;
            for (std::size_t j=0;j<channels.size();j++){
                if (channels[j] != value.channels[j]){
                    rvalue = false ;
                    break;
                }
            }
            return rvalue ;
        }
        //=================================================================================
        auto color_t::operator!=(const color_t &value) const ->bool {
            return !this->operator==(value);
        }
        
        //=================================================================================
        auto color_t::gray() const ->bool {
            return (channels[0]==channels[1]) && (channels[0] == channels[2]);
        }
        //=================================================================================
        auto color_t::opaque() const ->bool {
            return channels[static_cast<int>(colchan::alpha)] == 255 ;
        }
        //=================================================================================
        auto color_t::transparent() const ->bool {
            return channels[static_cast<int>(colchan::alpha)] == 0 ;
        }
        
        //=================================================================================
        auto color_t::operator[](colchan type) const ->const std::uint8_t& {
            return channels[static_cast<int>(type)];
        }
        //=================================================================================
        auto color_t::operator[](colchan type) -> std::uint8_t& {
            return channels[static_cast<int>(type)];
        }
        
        //=================================================================================
        auto color_t::hue(color_t* data,bool partial, colchan chan) const ->color_t {
            if ((partial && !gray()) || !opaque()){
                return *this ;
            }
            return *(data + this->operator[](colchan::red));
        }
    }
    
}
