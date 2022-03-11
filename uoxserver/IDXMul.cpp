// 
// Created on:  4/28/21

#include "IDXMul.hpp"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <fstream>
//+++++++++++++++++++++++++++++++++++++++++++++++++++
//
//		Methods for IDXMul
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++
//==========================================================================
IDXMul::IDXMul() {
}
//==========================================================================
IDXMul::IDXMul(const std::string &filepath) {
	load(filepath);
}

//==========================================================================
void IDXMul::load(const std::string &filepath){
	std::ifstream input(filepath,std::fstream::in | std::fstream::binary);
	if (!input.is_open()) {
		throw std::runtime_error(std::string("Unable to open: ")+filepath);
	}
	while(!input.eof()) {
		entry value ;
		input.read(reinterpret_cast<char*>(&value.offset),4);
		if (input.gcount()!= 4) {
			break;
		}
		input.read(reinterpret_cast<char*>(&value.length),4);
		if (input.gcount()!= 4) {
			break;
		}
		input.read(reinterpret_cast<char*>(&value.extra),4);
		if (input.gcount()!= 4) {
			break;
		}
		_entries.push_back(value);
	
		
	}
}
//==========================================================================
std::size_t IDXMul::size() const {
	return _entries.size();
}
//==========================================================================
const IDXMul::entry& IDXMul::operator[](std::size_t index) const {
	return _entries[index];
}
