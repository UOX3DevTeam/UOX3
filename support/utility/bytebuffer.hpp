//Copyright © 2022 Charles Kerr. All rights reserved.

#ifndef bytebuffer_hpp
#define bytebuffer_hpp


#include <ostream>
#include <cstdint>
#include <vector>
#include <list>
#include <deque>
#include <cstdlib>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <string>

#include "strutil.hpp"
//=========================================================
// bytebuffer_bounds
//=========================================================
//=========================================================
//=========================================================
struct bytebuffer_bounds : public std::out_of_range {
	int offset ;
	int amount ;
	int buffersize ;
	std::string _msg ;
	explicit bytebuffer_bounds(int offset, int amount, int size);
	auto what() const noexcept   ->const char*  override;
};
//=========================================================
// bytebuffer_t
//=========================================================
//=========================================================
class bytebuffer_t {
protected:
	mutable int _index ;
	std::vector<std::uint8_t> _bytedata ;
	auto exceeds(int offset,int bytelength) const ->bool ;
	auto exceeds(int offset,int bytelength,bool expand) ->bool ;
public:
	bytebuffer_t(int size=0,int reserve=0);
	
	auto size() const ->size_t ;
	auto size(int value,std::uint8_t fill = 0) ->void ;
	
	auto index() const ->int ;
	auto index(int value) ->void ;
	
	auto raw() const ->const std::uint8_t* ;
	auto raw()  -> std::uint8_t* ;
	
	auto operator[](int index) const -> const std::uint8_t & ;
	auto operator[](int index)  ->  std::uint8_t & ;
	
	auto fill(std::uint8_t value, int offset,int length)->void ;
	
	auto log(std::ostream &output,strutil::radix_t radix=strutil::radix_t::hex,int entries_line = 8)const ->void;
	//=========================================================
	// we need to read :integral/floating, vectors/list/strings
	template <typename T>
	auto read(int offset=-1,int amount=-1,bool reverse=true) const ->T {
		if (offset < 0){
			offset = _index ;
		}
		if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>){
			// we ignore amount for integrals and floating point
			auto size = static_cast<int>(sizeof(T)) ;
			if constexpr (std::is_same_v<T,bool>){
				size = 1 ;
			}
			if (exceeds(offset,size)){
				
				throw bytebuffer_bounds(offset,size,static_cast<int>(_bytedata.size()));
			}
			T value(0);
			std::copy(_bytedata.data()+offset,_bytedata.data()+offset+size,reinterpret_cast<std::uint8_t*>(&value));
			if (reverse  && (size>1) ){
				std::reverse(reinterpret_cast<std::uint8_t*>(&value),reinterpret_cast<std::uint8_t*>(&value)+size) ;
			}
			_index = offset + size;
			return value ;
		}
		else if constexpr (std::is_class_v<T> ){
			if constexpr (std::is_integral_v<typename T::value_type>){
				// It is a supported container (hopefully) they want back!
				// what is the size of the entry in the container
				auto entry_size = static_cast<int>(sizeof(typename T::value_type));
				if (amount <0){
					// what we do is calcualate how many entries are left in the buffer
					amount = (static_cast<int>(_bytedata.size()) - offset )/entry_size ;
				}
				auto requested_size =  entry_size * amount ;
				// are we exceeding that?
				if (exceeds(offset, requested_size)){
					throw bytebuffer_bounds(offset,requested_size,static_cast<int>(_bytedata.size()));
				}
				// we need to loop through and read a "character" at a time
				typename T::value_type character =0 ;
				T rvalue ;
				
				for (auto i=0 ; i< amount ;++i){
					std::copy(_bytedata.data()+offset + i*entry_size,_bytedata.data()+offset + (i+1)*entry_size,reinterpret_cast<std::uint8_t*>(&character)) ;
					// should we "reverse it" ?
					if ((entry_size >1) && reverse) {
						std::reverse(reinterpret_cast<std::uint8_t*>(&character),reinterpret_cast<std::uint8_t*>(&character)+entry_size) ;
					}
					// If this is a string, we stop at a null terminator.  Do we really want to do this?
					if constexpr (std::is_same_v<std::string,T> || std::is_same_v<std::wstring,T> || std::is_same_v<std::u16string,T> || std::is_same_v<std::u32string,T>){
						if (character == 0) {
							break;
						}
					}
					rvalue.push_back(character) ;
				}
				_index = offset+ requested_size ;
				return rvalue ;
			}
		}
		
	}
	//=========================================================
	// This reads into the buffer supplied, only for integral types ;
	template<typename T>
	auto read(int offset, T *value,int amount = -1,bool reverse = true) const ->void {
		if (offset <0){
			offset = _index ;
		}
		if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>){
			auto entry_size = static_cast<int>(sizeof(T)) ;
			if constexpr (std::is_same_v<T,bool>){
				entry_size = 1 ;
			}
			auto size = amount * entry_size ;
			if (amount <0){
				size = entry_size ;
				amount = 1 ;
			}
			if (amount >0){
				if (exceeds(offset,size)){
					throw bytebuffer_bounds(offset,size,static_cast<int>(_bytedata.size()));
				}
				// We now get to read
				T input ;
				for (auto i=0 ; i< amount ; ++i){
					std::copy(_bytedata.begin()+offset + (i*entry_size),_bytedata.begin()+offset+((i+1)*entry_size),reinterpret_cast<std::uint8_t*>(&input)) ;
					if (reverse) {
						std::reverse(reinterpret_cast<std::uint8_t*>(&input), reinterpret_cast<std::uint8_t*>(&input)+entry_size) ;
					}
					// Now put it into the data stream
					std::copy(reinterpret_cast<std::uint8_t*>(&input),reinterpret_cast<std::uint8_t*>(&input)+entry_size ,value+(i*entry_size)) ;
					
				}
				_index = offset + amount * entry_size ;
			}
		}
	}
	//=========================================================
	template<typename T>
	auto write(int offset,const T *value,int amount=-1,bool reverse = true,bool expand = true)->bytebuffer_t&{
		if (offset <0){
			offset = _index ;
		}
		if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>){
			auto entry_size = static_cast<int>(sizeof(T)) ;
			if constexpr (std::is_same_v<T,bool>){
				entry_size = 1 ;
			}
			auto size = amount * entry_size ;
			if (amount <0){
				size = entry_size ;
				amount = 1 ;
			}
			if (amount >0){
				if (exceeds(offset,size,expand)){
					throw bytebuffer_bounds(offset,size,static_cast<int>(_bytedata.size()));
				}
				// we need to write it
				for (auto i= 0 ; i< amount ; ++i){
					auto input = value[i] ;
					if (reverse && (entry_size >1)){
						std::reverse(reinterpret_cast<std::uint8_t*>(&input),reinterpret_cast<std::uint8_t*>(&input)+entry_size) ;
					}
					std::copy(reinterpret_cast<std::uint8_t*>(&input),reinterpret_cast<std::uint8_t*>(&input)+entry_size,_bytedata.begin()+offset + (i*entry_size));
				}
				_index = offset + (entry_size * amount);
			}
			return *this ;  // we put this here, versue at the end, for we want a compile error if a type not caught with if constexpr. So a return in each if constexpr at the top level
		}
	}

	//=========================================================
	template<typename T>
	auto write(int offset,const T &value,int amount=-1,bool reverse = true,bool expand = true)->bytebuffer_t&{
		if (offset <0){
			offset = _index ;
		}
		 
		if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>){
			// we ignore amount for integrals and floating point
			auto size = static_cast<int>(sizeof(T)) ;
			if constexpr (std::is_same_v<T,bool>){
				size = 1 ;
			}
			if (exceeds(offset,size,expand)){
				
				throw bytebuffer_bounds(offset,size,static_cast<int>(_bytedata.size()));
			}
			// we need to write it
			T temp = value ;
			if (reverse) {
				std::reverse(reinterpret_cast<std::uint8_t*>(&temp),reinterpret_cast<std::uint8_t*>(&temp)+size);
			}
			std::copy(reinterpret_cast<std::uint8_t*>(&temp),reinterpret_cast<std::uint8_t*>(&temp)+size,_bytedata.begin()+offset);
			
			_index = offset + size ;
			return *this ;  // we put this here, versue at the end, for we want a compile error if a type not caught with if constexpr. So a return in each if constexpr at the top level
		}
		else if constexpr (std::is_class_v< T>){
			if constexpr (std::is_integral_v<typename T::value_type>){
				// It is a supported container (hopefully) they want back!
				// what is the size of the entry in the container
				auto entry_size = static_cast<int>(sizeof(typename T::value_type));
				auto container_size = static_cast<int>(value.size()) ;
				auto fill_size = 0 ;
				auto write_size = container_size ;
				if (amount <0){
					amount = container_size ;
				}
				else {
					write_size = amount ;
					if (amount > container_size){
						fill_size = amount - container_size ;
						write_size = container_size ;
					}
				}
				auto requested_size =  (write_size + fill_size) * entry_size;
				// are we exceeding that?
				if (exceeds(offset, requested_size,expand)){
					throw bytebuffer_bounds(offset,requested_size,static_cast<int>(_bytedata.size()));
				}
				// Ok, so now we get to go and do our thing
				for (auto i=0 ; i< write_size;++i){
					auto entry = value[i] ;
					if (reverse && (entry_size > 1)) {
						std::reverse(reinterpret_cast<std::uint8_t*>(&entry),reinterpret_cast<std::uint8_t*>(&entry)+entry_size) ;
					}
					std::copy(reinterpret_cast<std::uint8_t*>(&entry),reinterpret_cast<std::uint8_t*>(&entry)+entry_size,_bytedata.data()+offset+i*entry_size);
					
				}
				_index = offset + (write_size *entry_size);
				
				// Now we need to do the fill if anyway
				if (fill_size>0){
					std::fill(_bytedata.data()+_index,_bytedata.data()+_index+(fill_size*entry_size),0);
				}
				_index += fill_size * entry_size ;
			}
			return *this ;
		}
		else if constexpr (std::is_array_v<T>){
			if constexpr (std::is_integral_v<typename std::remove_extent<T>::type> || std::is_floating_point_v<typename std::remove_extent<T>::type>){
				// It is an array!
				auto entry_size = static_cast<int>(sizeof(typename std::remove_extent<T>::type));
				if (amount <0){
					amount = 1 ;
				}
				auto requested_size = amount * entry_size ;
			
				if (exceeds(offset,requested_size,expand)){
					throw bytebuffer_bounds(offset,requested_size,static_cast<int>(_bytedata.size()));
				}
				// We need to check and loop through if we are reversing ;
				
				// we need to write it
				for (auto i= 0 ; i< amount ; ++i){
					auto input = value[i] ;
					if (reverse && (entry_size >1)){
						std::reverse(reinterpret_cast<std::uint8_t*>(&input),reinterpret_cast<std::uint8_t*>(&input)+entry_size) ;
					}
					std::copy(reinterpret_cast<std::uint8_t*>(&input),reinterpret_cast<std::uint8_t*>(&input)+entry_size,_bytedata.begin()+offset + (i*entry_size));
				}
				_index = offset + (entry_size * amount);
				return *this ;
			
			}
		}
		
	}
};
#endif /* bytebuffer_hpp */
