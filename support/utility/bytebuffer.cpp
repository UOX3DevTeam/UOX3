//Copyright © 2022 Charles Kerr. All rights reserved.

#include "bytebuffer.hpp"
#include <iostream>
using namespace std::string_literals ;
//=========================================================
// bytebuffer exceptions
//=========================================================
//=========================================================
bytebuffer_bounds::bytebuffer_bounds(int offset, int amount, int size): offset(offset), amount(amount), buffersize(size), std::out_of_range(""){
	_msg = "Offset : "s + std::to_string(offset) + " Amount: "s+std::to_string(amount)+" exceeds buffer size of: "s+std::to_string(size);
}
//=========================================================
auto bytebuffer_bounds::what() const noexcept  ->const char*{
	return _msg.c_str() ;
}

//=========================================================
// bytebuffer_t
//=========================================================
//=========================================================
auto bytebuffer_t::exceeds(int offset,int bytelength) const ->bool {
	auto index = offset + bytelength ;
	return static_cast<int>(_bytedata.size()) < index ;
}
//=========================================================
auto bytebuffer_t::exceeds(int offset,int bytelength,bool expand) ->bool {
	auto rvalue = exceeds(offset,bytelength) ;
	if (expand && rvalue){
		this->size(offset+bytelength) ;
		rvalue = false ;
	}
	return rvalue ;
}
//=========================================================
bytebuffer_t::bytebuffer_t(int size,int reserve):_index(0){
	
	if ((reserve>0) && (size <=0)){
		_bytedata.reserve(reserve);
	}
	if (size>0){
		_bytedata.resize(size);
	}
}

//=========================================================
auto bytebuffer_t::size() const ->size_t {
	return _bytedata.size() ;
}
//=========================================================
auto bytebuffer_t::size(int value,std::uint8_t fill) ->void {
	_bytedata.resize(value, fill);
}
//=========================================================
auto bytebuffer_t::index() const ->int {
	return _index ;
}
//=========================================================
auto bytebuffer_t::index(int value) ->void {
	_index = value ;
}
//=========================================================
auto bytebuffer_t::raw() const ->const std::uint8_t* {
	return _bytedata.data();
}
//=========================================================
auto bytebuffer_t::raw()  -> std::uint8_t* {
	return _bytedata.data();
}
//=========================================================
auto bytebuffer_t::operator[](int index) const -> const std::uint8_t & {
	return _bytedata[index];
}
//=========================================================
auto bytebuffer_t::operator[](int index)  ->  std::uint8_t & {
	return _bytedata[index];
}
//=========================================================
auto bytebuffer_t::fill(std::uint8_t value, int offset,int length)->void {
	if (offset <0) {
		offset = _index ;
	}
	if (length <0) {
		length = static_cast<int>(_bytedata.size()) - offset ;
	}
	if (length >0){
		std::fill(_bytedata.begin()+offset, _bytedata.begin()+offset+length, value);
	}
}
//=========================================================
auto bytebuffer_t::log(std::ostream &output,strutil::radix_t radix,int entries_line)const ->void{
	strutil::dump(output,_bytedata.data(),_bytedata.size(),radix,entries_line);
	
}
