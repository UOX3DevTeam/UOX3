//Copyright © 2023 Charles Kerr. All rights reserved.

#ifndef image_hpp
#define image_hpp

#include <cstdint>
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <filesystem>

#include "color.hpp"
//=================================================================================
namespace util {
    namespace img {
        
        class image_t{
            std::vector<std::vector<color_t>> data ;
            
        public:
            image_t()=default ;
            image_t(std::size_t height,std::size_t width,const color_t &fillcolor=color_t(0));
            
            auto empty() const ->bool ;
            auto size() const ->std::pair<std::size_t,std::size_t> ;
            [[maybe_unused]] auto size(std::size_t height, std::size_t width,const color_t &fillcolor = color_t(0)) ->image_t&;
            
            auto color(std::size_t row,std::size_t col) const ->const color_t& ;
            auto color(std::size_t row,std::size_t col)  -> color_t& ;
            
            [[maybe_unused]] auto invert() ->image_t& ;
            [[maybe_unused]] auto flip() ->image_t& ;
            
            [[maybe_unused]] auto copy(const image_t &image, std::size_t offrow,std::size_t offcol,std::size_t height,std::size_t width, std::size_t destrow,std::size_t destcol,bool transparent=false) ->image_t& ;
            
            auto palette() const ->std::set<color_t> ;
            
            auto save(std::ofstream &output) ->void ;
            auto save(const std::filesystem::path &outpath) ->bool ;
            
            [[maybe_unused]] auto load(std::ifstream &input) ->image_t& ;
            [[maybe_unused]] auto load(const std::filesystem::path &bmppath) ->image_t& ;
        };
    }
}
#endif /* image_hpp */
