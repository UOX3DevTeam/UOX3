//Copyright © 2023 Charles Kerr. All rights reserved.

#include "image.hpp"

#include <iostream>
#include <algorithm>
#include <stdexcept>

using namespace std::string_literals;
namespace util {
    //=================================================================================
    namespace img {
        //============================================================================
        struct bmpsig{
            constexpr static auto size = 14 ;
            std::uint16_t signature ;
            std::uint32_t filesize ;
            std::uint32_t placeholder;
            std::uint32_t offset ;
            [[maybe_unused]] auto load(std::ifstream &input) ->bmpsig& {
                input.read(reinterpret_cast<char*>(&signature),sizeof(signature));
                input.read(reinterpret_cast<char*>(&filesize),sizeof(filesize));
                input.read(reinterpret_cast<char*>(&placeholder),sizeof(placeholder));
                input.read(reinterpret_cast<char*>(&offset),sizeof(offset));
                return *this;
            }
            bmpsig():filesize(14+40),placeholder(0),offset(14+40),signature(0x4d42){
            }
            bmpsig(std::ifstream &input){
                load(input) ;
            }
            auto save(std::ofstream &output) ->void {
                output.write(reinterpret_cast<char*>(&signature),sizeof(signature));
                output.write(reinterpret_cast<char*>(&filesize),sizeof(filesize));
                output.write(reinterpret_cast<char*>(&placeholder),sizeof(placeholder));
                output.write(reinterpret_cast<char*>(&offset),sizeof(offset));
            }
        };
        
        //============================================================================
        struct dibheader{
            constexpr static auto size = 40 ;
            std::uint32_t header_size ;
            std::int32_t width ;
            std::int32_t height ;
            std::uint16_t num_planes ;
            std::uint16_t bits_pixel ;
            std::uint32_t compression ;
            std::uint32_t image_size ;
            std::uint32_t horiz_resolution ;
            std::uint32_t vert_resolution ;
            std::uint32_t color_count ;
            std::uint32_t important_count ;
            [[maybe_unused]] auto load(std::ifstream &input) ->dibheader& {
                input.read(reinterpret_cast<char*>(&header_size),sizeof(header_size));
                if (header_size < dibheader::size){
                    throw std::runtime_error("Unsupported dib header format");
                }
                input.read(reinterpret_cast<char*>(&width),sizeof(width));
                input.read(reinterpret_cast<char*>(&height),sizeof(height));
                input.read(reinterpret_cast<char*>(&num_planes),sizeof(num_planes));
                input.read(reinterpret_cast<char*>(&bits_pixel),sizeof(bits_pixel));
                input.read(reinterpret_cast<char*>(&compression),sizeof(compression));
                input.read(reinterpret_cast<char*>(&image_size),sizeof(image_size));
                input.read(reinterpret_cast<char*>(&horiz_resolution),sizeof(horiz_resolution));
                input.read(reinterpret_cast<char*>(&vert_resolution),sizeof(vert_resolution));
                input.read(reinterpret_cast<char*>(&color_count),sizeof(color_count));
                input.read(reinterpret_cast<char*>(&important_count),sizeof(important_count));
                return *this;
            }
            auto save(std::ofstream &output) ->void {
                output.write(reinterpret_cast<char*>(&header_size),sizeof(header_size));
                output.write(reinterpret_cast<char*>(&width),sizeof(width));
                output.write(reinterpret_cast<char*>(&height),sizeof(height));
                output.write(reinterpret_cast<char*>(&num_planes),sizeof(num_planes));
                output.write(reinterpret_cast<char*>(&bits_pixel),sizeof(bits_pixel));
                output.write(reinterpret_cast<char*>(&compression),sizeof(compression));
                output.write(reinterpret_cast<char*>(&image_size),sizeof(image_size));
                output.write(reinterpret_cast<char*>(&horiz_resolution),sizeof(horiz_resolution));
                output.write(reinterpret_cast<char*>(&vert_resolution),sizeof(vert_resolution));
                output.write(reinterpret_cast<char*>(&color_count),sizeof(color_count));
                output.write(reinterpret_cast<char*>(&important_count),sizeof(important_count));
                
            }
            //Print resolution of the image,
            //72 DPI × 39.3701 inches per metre yields 2834.6472
            dibheader():header_size(size),width(0),height(0),num_planes(1),bits_pixel(32),compression(0),image_size(0),horiz_resolution(0xb13),vert_resolution(0xb13),color_count(0),important_count(0) {
            }
            dibheader(std::ifstream &input){
                load(input) ;
            }
        };
        
        
        
        //=================================================================================
        image_t::image_t(std::size_t height,std::size_t width,const color_t &fillcolor){
            auto data = std::vector(height,std::vector(width,fillcolor));
        }
        
        //=================================================================================
        auto image_t::empty() const ->bool{
            return data.empty();
        }
        
        //=================================================================================
        auto image_t::size() const ->std::pair<std::size_t,std::size_t> {
            auto height = static_cast<std::size_t>(data.size());
            auto width = std::size_t(0);
            if (height>0){
                width = static_cast<std::size_t>(data.at(0).size());
            }
            return std::make_pair(height,width);
        }
        //=================================================================================
        auto image_t::size(std::size_t height, std::size_t width,const color_t &fillcolor) ->image_t&{
            
            data.resize(height,std::vector<color_t>(width,fillcolor));
            std::for_each(data.begin(),data.end(),[width,&fillcolor](std::vector<color_t> &row){
                row.resize(width,fillcolor);
            });
            return *this ;
        }
        
        //=================================================================================
        auto image_t::color(std::size_t row,std::size_t col) const ->const color_t& {
            return data.at(row).at(col);
        }
        //=================================================================================
        auto image_t::color(std::size_t row,std::size_t col)  -> color_t& {
            return data.at(row).at(col);
        }
        
        //=================================================================================
        auto image_t::invert() ->image_t& {
            std::reverse(data.begin(), data.end());
            return *this;
        }
        //=================================================================================
        auto image_t::flip() ->image_t& {
            std::for_each(data.begin(),data.end(),[](std::vector<color_t> &row){
                std::reverse(row.begin(), row.end());
            });
            return *this;
        }
        
        //=================================================================================
        auto image_t::copy(const image_t &image, std::size_t offrow,std::size_t offcol,std::size_t height,std::size_t width, std::size_t destrow,std::size_t destcol,bool transparent) ->image_t& {
            auto [myheight,mywidth] = this->size();
            auto [sheight, swidth] = image.size();
            for (std::size_t row = 0;row < height;row++){
                if ((row+destrow) < myheight){
                    if (row+offrow < sheight){
                        for (std::size_t col =0;col < width;col++){
                            if (col+destcol < width){
                                if (col + offcol < swidth){
                                    auto color = image.color(offrow=row, offcol+col);
                                    if (!color.transparent() || transparent ){
                                        this->color(destrow+row, destcol+col) = color;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            return *this;
        }
        
        //=================================================================================
        auto image_t::palette() const ->std::set<color_t> {
            auto rvalue = std::set<color_t>();
            for (const auto &row:data){
                for (const auto &color:row){
                    rvalue.insert(color);
                }
            }
            return rvalue ;
        }
        
        //=================================================================================
        auto image_t::save(std::ofstream &output) ->void {
            auto header = dibheader() ;
            auto [height,width] = this->size();
            header.width = static_cast<std::int32_t>(width) ;
            header.height = static_cast<std::int32_t>(height) ;
            header.bits_pixel = 32 ;
            
            header.image_size = static_cast<std::uint32_t>(height*width*4);
            auto sig = bmpsig() ;
            
            sig.filesize = static_cast<std::uint32_t>(bmpsig::size + dibheader::size + header.image_size) ;
            sig.save(output);
            header.save(output);
            for (auto iter = data.rbegin();iter != data.rend();iter++){
                for (const auto &color:*iter){
                    auto value = color.value<std::uint32_t>();
                    output.write(reinterpret_cast<const char*>(&value),sizeof(value));
                }
            }
        }
        //=================================================================================
        auto image_t::save(const std::filesystem::path &outpath) ->bool{
            auto output = std::ofstream(outpath.string(),std::ios::binary);
            auto rvalue = false ;
            if (output.is_open()){
                rvalue = true ;
                save(output);
            }
            return rvalue ;
        }
        
        
        //=================================================================================
        auto image_t::load(const std::filesystem::path &bmppath) ->image_t& {
            auto input = std::ifstream(bmppath.string(),std::ios::binary);
            if (!input.is_open()){
                throw std::runtime_error("Unable to open: "s + bmppath.string());
            }
            return load(input);
            
        }
        //=================================================================================
        auto image_t::load(std::ifstream &input) ->image_t& {
            auto sig = bmpsig(input);
            auto current = static_cast<std::uint32_t>(input.tellg());
            auto header = dibheader(input);
            if (header.compression != 0){
                throw std::runtime_error("Unsupported bmp file compression method");
            }
            if (header.bits_pixel<8){
                throw std::runtime_error("Unsupported bmp pixel size");
            }
            // Does this have a palette?
            auto headerend = current + header.header_size;
            
            auto palettesize = header.color_count ;
            if (palettesize == 0){
                palettesize = sig.offset - headerend ;
            }
            input.seekg(headerend,std::ios::beg);
            
            auto count = palettesize/4 ;
            auto palette_data = std::array<std::uint8_t,4>() ;
            auto palette = std::vector<color_t>() ;
            for (std::uint32_t j= 0 ; j<count;j++){
                input.read(reinterpret_cast<char*>(palette_data.data()),palette_data.size());
                palette.push_back(color_t(palette_data[2],palette_data[1],palette_data[0],palette_data[3]));
                
            }
            input.seekg(sig.offset,std::ios::beg);
            data = std::vector<std::vector<color_t>>(header.height,std::vector<color_t>(header.width,color_t()));
            auto temp =(header.width * (header.bits_pixel/8)) ;
            auto pad = (temp%4==0?0:4-temp%4);
            auto padvalue = std::uint32_t(0) ;
            auto colvalue = std::uint32_t(0);
            auto color = color_t() ;
            for (std::int32_t row = 0 ; row < header.height;row++){
                for (std::int32_t col = 0 ; col < header.width;col++){
                    switch (header.bits_pixel){
                        case 8: {
                            input.read(reinterpret_cast<char*>(&colvalue),1);
                            color  = palette.at(colvalue);
                            colvalue = 0 ;
                            break;
                        }
                        case 16: {
                            input.read(reinterpret_cast<char*>(&colvalue),1);
                            colvalue |= 0x8000;
                            color = static_cast<std::uint16_t>(colvalue);
                            colvalue = 0 ;
                            break;
                        }
                        case 24: {
                            input.read(reinterpret_cast<char*>(palette_data.data()),3); // we are reusing this varable
                            color = color_t(palette_data.at(0),palette_data.at(1),palette_data.at(2),255);
                            
                            break;
                        }
                            
                        case 32: {
                            input.read(reinterpret_cast<char*>(&colvalue),1);
                            
                            color = colvalue;
                            colvalue = 0 ;
                            
                            break;
                        }
                            
                    }
                    data.at(data.size()-row -1).at(col) = color ;
                }
                
                input.read(reinterpret_cast<char*>(&padvalue),pad);
            }
            
            return *this;
        }
        
    }
}
