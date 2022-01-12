// 
// Created on:  4/30/21

#include "MultiMul.hpp"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <filesystem>
#include <algorithm>

#include "IDXMul.hpp"

using namespace UO ;

const std::string MultiMul::_uopformat = "build/multicollection/%06d.bin";

//+++++++++++++++++++++++++++++++++++++++++++++++++++
//
//		Methods for MultiMul
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++
//=========================================================================
std::size_t multi_structure::memoryComponent() const {
	std::size_t rvalue = 0 ;
	for (auto &entry : _items) {
		rvalue = rvalue + entry.second.size() * sizeof(multi_component) ;
	}
	return rvalue ;
}


//=========================================================================
MultiMul::MultiMul() : UOPData() {
	_mytype = UOPData::multi ;
	_hashformat = _uopformat;
	_maxhashes = 0x10000 ;
}
//=========================================================================
MultiMul::MultiMul(const std::string &mulpath, const std::string &idxpath):MultiMul() {
	
	load(mulpath,idxpath);
}
//=========================================================================
MultiMul::MultiMul(const std::string &uoppath) : MultiMul() {
	load(uoppath);
}
//=============================================================================
void MultiMul::load(const std::string &uoppath){
	std::ifstream uopstream(uoppath,std::ifstream::binary|std::ifstream::in);
	if (!uopstream.is_open()){
		throw std::runtime_error(std::string("Unable to open uop file: ")+uoppath);
	}
	loadUOP(uopstream);
	uopstream.close();

}

//=============================================================================
void MultiMul::processData(std::vector<unsigned char> &data, std::uint32_t chunkid){
	multi_structure structure ;
	std::size_t offset = 4 ;  // skip the first 32 bit word 
	std::uint32_t count ;
	count = (*reinterpret_cast<std::uint32_t *>((data.data()+offset))) ;
	offset = offset + 4 ;
	for (unsigned int i = 0 ; i < count ; i++){
		multi_component component ;
		component.visible = 0 ;
		component.tileid  = (*reinterpret_cast<std::uint16_t *>((data.data()+offset))) ;
		offset = offset + 2 ;
		component.xoffset = (*reinterpret_cast<std::int16_t *>((data.data()+offset))) ;
		offset = offset + 2 ;
		component.yoffset = (*reinterpret_cast<std::int16_t *>((data.data()+offset))) ;
		offset = offset + 2 ;
		component.zoffset = (*reinterpret_cast<std::int16_t *>((data.data()+offset))) ;
		offset = offset + 2 ;
		auto flags = (*reinterpret_cast<std::uint16_t *>((data.data()+offset))) ;
		offset = offset + 2 ;
		switch (flags) {
			default:
			case 0: 					// background
				component.visible = 1 ;
				break;
			case 1:					// none
				break;
			case 257:					// generic
				component.visible = 0x800;
				break;
		}
		auto clilocs = (*reinterpret_cast<std::uint32_t *>((data.data()+offset))) ;
		offset = offset + 4 ;
		if (clilocs != 0 ) {
			offset = offset + (clilocs * 4);
		}

		// Add max bounds of multi
		structure._maxX = std::max( static_cast<int>( component.xoffset ), structure._maxX );
		structure._minX = std::min( static_cast<int>( component.xoffset ), structure._minX );
		structure._maxY = std::max( static_cast<int>( component.yoffset ), structure._maxY );
		structure._minY = std::min( static_cast<int>( component.yoffset ), structure._minY );

		structure.addItem(component);

	}
	_entries.insert_or_assign(chunkid, structure);
}


//=========================================================================
void MultiMul::load(const std::string &mulpath, const std::string &idxpath){
	
	bool useHS = false ;
	
	auto mulsize = std::filesystem::file_size(std::filesystem::path(mulpath));
	if (mulsize >= hssize) {
		useHS = true ;
	}
	auto idx = IDXMul(idxpath);
	auto size = idx.size() ;
	std::ifstream mul(mulpath, std::ios::binary | std::ios::in);
	if (!mul.is_open()) {
		throw std::runtime_error(std::string("Unable to open: ")+ mulpath) ;
	}
	for ( auto i = 0 ; i < size ; i++) {
		auto entry = idx[i] ;
		if (entry.valid()) {
			mul.seekg(entry.offset,std::ios::beg) ;
			auto length = entry.length ;
			multi_structure entry= multi_structure();
			bool firsttime = true ;
			while (length > 0) {
				multi_component item ;
				
				short temp =0;
				unsigned int 	dummy =0;
				// Read in Tileid
				mul.read(reinterpret_cast<char*>(&temp),2);
				item.tileid = static_cast<unsigned short>(temp);
				length = length-2 ;
				
				// Read in xoffset
				mul.read(reinterpret_cast<char*>(&temp),2);
				item.xoffset = static_cast<unsigned short>(temp);
				length = length-2 ;

				// Read in yoffset
				mul.read(reinterpret_cast<char*>(&temp),2);
				item.yoffset = static_cast<unsigned short>(temp);
				length = length-2 ;

				// Read in Zoffset
				mul.read(reinterpret_cast<char*>(&temp),2);
				item.zoffset = static_cast<unsigned short>(temp);
				length = length-2 ;

				// Read in flags ;
				mul.read(reinterpret_cast<char*>(&dummy),4);
				item.visible = dummy ;
				length = length-4 ;
				if (useHS) {
					mul.read(reinterpret_cast<char*>(&dummy),4);
					length = length-4;

				}
				
				if (firsttime) {
					entry._maxX = item.xoffset ;
					entry._minX = item.xoffset ;
					entry._maxY = item.yoffset ;
					entry._minY = item.yoffset;
					firsttime = false;
				}
				else {
					
					entry._maxX = std::max(static_cast<int>(item.xoffset),entry._maxX);
					entry._minX = std::min(static_cast<int>(item.xoffset),entry._minX);
					entry._maxY = std::max(static_cast<int>(item.yoffset),entry._maxY);
					entry._minY = std::min(static_cast<int>(item.yoffset),entry._minY);
				}
				
				entry.addItem(item);
			}
			
			_entries.insert_or_assign(i, entry);
			
		}
	}
}
//======================================================================
bool MultiMul::exists(int id){
	auto iter = _entries.find(id) ;
	if (iter == _entries.end()) {
		return false ;
	}
	return true ;
	
}
//======================================================================
const multi_structure& MultiMul::entry(int id) const {
	auto iter = _entries.find(id) ;
	if (iter != _entries.end()) {
		return iter->second ;
	}
	else {
		throw std::runtime_error(std::string("Multi id: ") + std::to_string(id) + std::string(" not found"));
	}
}
//======================================================================
std::size_t MultiMul::amount() const {
	return _entries.size() ;
}

//======================================================================
std::size_t MultiMul::memoryMulti() const {
	std::size_t rvalue = 0 ;
	for (auto &entry : _entries) {
		rvalue = rvalue + entry.second.memoryComponent() ;
	}
	return rvalue ;
}

//======================================================================
std::size_t MultiMul::size() const {
	return _entries.size() ;
}
