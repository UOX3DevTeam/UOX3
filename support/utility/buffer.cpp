//Copyright © 2023 Charles Kerr. All rights reserved.

#include "buffer.hpp"

#include <iostream>
#include <stdexcept>

using namespace std::string_literals ;

//=========================================================================================
namespace util {
    //=================================================================================
    // Constructors
    //==============================================================================
    
    //=================================================================================
    buffer_t::buffer_t(): write_data(nullptr),read_data(nullptr),current_position(0),length(0),owning(false),is_expandable(true){

    }

    //=================================================================================
    buffer_t::buffer_t(const void *ptr, std::size_t size, bool consume):buffer_t(){
        if ((ptr==nullptr) || (size==0)){
            throw std::runtime_error("buffer_t initialization with null data");
        }
        if (consume){
            owning=true ;
            data = std::vector<std::uint8_t>(size,0) ;
            std::copy(static_cast<const std::uint8_t*>(ptr),static_cast<const std::uint8_t*>(ptr)+size,data.begin());
            ptr = static_cast<const void*>(data.data()) ;
            write_data = static_cast<void*>(data.data());
            size = data.size();
        }
        read_data = ptr;
        length = size ;
    }
    //=================================================================================
    buffer_t::buffer_t(void *ptr, std::size_t size, bool consume):buffer_t(){
        if ((ptr==nullptr) || (size==0)){
            throw std::runtime_error("buffer_t initialization with null data");
        }
        if (consume){
            owning=true ;
            data = std::vector<std::uint8_t>(size,0) ;
            std::copy(static_cast<std::uint8_t*>(ptr),static_cast<std::uint8_t*>(ptr)+size,data.begin());
            ptr = static_cast<void*>(data.data()) ;
            size = data.size();
        }
        write_data = ptr ;
        read_data = const_cast<const void *>(ptr);
        length = size ;
    }
    //=================================================================================
    buffer_t::buffer_t( std::size_t size):buffer_t(){
        data = std::vector<std::uint8_t>(size,0) ;
        write_data = static_cast<void*>(data.data()) ;
        read_data = static_cast<const void*>(data.data());
        length = data.size();
        owning = true ;
        
    }

    //=================================================================================
    // Size/position related
    //=================================================================================
    
    //=================================================================================
    auto buffer_t::size() const ->std::size_t {
        return length;
    }
    //=================================================================================
    auto buffer_t::remaining() const ->std::size_t {
        return length-current_position ;
    }
    //=================================================================================
    auto buffer_t::at() const ->std::size_t{
        return current_position;
    }
    //=================================================================================
    auto buffer_t::at(std::size_t position)  -> buffer_t&{
        if (position > length){
            throw std::out_of_range("Index position exceeds buffer length");
        }
        current_position = position ;
        return *this ;
    }
    //=================================================================================
    auto buffer_t::resize(std::size_t size) ->buffer_t& {
        if (!owning) {
            throw std::runtime_error("Unable to resize buffer, not the data owner");
        }
        data.resize(size,0);
        if (current_position > data.size()){
            current_position = data.size();
        }
        write_data = static_cast<void*>(data.data());
        read_data = static_cast<const void*>(data.data());
        length = data.size() ;
        return *this ;
    }
    
    //=================================================================================
    auto buffer_t::expandable(bool value) ->buffer_t{
        is_expandable  = value ;
        return *this ;
    }
    
    //=================================================================================
    auto buffer_t::expandable() const ->bool{
        return is_expandable;
    }

    //=================================================================================
    // Access related
    //=================================================================================
    
    //=================================================================================
    auto buffer_t::raw() ->void* {
        return write_data;
    }
    //=================================================================================
    auto buffer_t::raw() const ->const void* {
        return read_data ;
    }

}
