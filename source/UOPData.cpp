// 
// Created on:  4/30/21

#include "UOPData.hpp"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "zlib.h"
//+++++++++++++++++++++++++++++++++++++++++++++++++++
//
//		Methods for UOPData
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++

//=============================================================================
const std::array<std::string, 2> UOPData::_hashformats = {
	"build/multicollection/#.bin",
	"build/map?legacymul/#.dat"
};
//=============================================================================
const std::array<std::int32_t, 2> UOPData::_fixedwidths = {6,8} ;
//=============================================================================
const std::array<std::size_t, 2> UOPData::_maxhashes = {0x10000,200} ;

//=============================================================================
UOPData::UOPData() {
	_mytype = invalid ;
}

//=============================================================================
std::uint64_t UOPData::HashLittle2(const std::string& s) {
	
	std::uint32_t length = static_cast<std::uint32_t>(s.size()) ;
	std::uint32_t a ;
	std::uint32_t b ;
	std::uint32_t c ;
	
	c = 0xDEADBEEF + static_cast<std::uint32_t>(length) ;
	a = c;
	b = c ;
	std::uint32_t k = 0 ;
	std::uint32_t l = 0 ;
	
	while (length > 12){
		a += (s[k++]);
		a += (s[k++] << 8);
		a += (s[k++] << 16);
		a += (s[k++] << 24);
		b += (s[k++]);
		b += (s[k++] << 8);
		b += (s[k++] << 16);
		b += (s[k++] << 24);
		c += (s[k++]);
		c += (s[k++] << 8);
		c += (s[k++] << 16);
		c += (s[k++] << 24);
		
		a -= c; a ^= c << 4 | c >> 28; c += b;
		b -= a; b ^= a << 6 | a >> 26; a += c;
		c -= b; c ^= b << 8 | b >> 24; b += a;
		a -= c; a ^= c << 16 | c >> 16; c += b;
		b -= a; b ^= a << 19 | a >> 13; a += c;
		c -= b; c ^= b << 4 | b >> 28; b += a;
		
		length -= 12 ;
	}
	
	// Notice the lack of breaks!  we actually want it to fall through
	switch (length) {
		case 12: {
			l = k + 11;
			c += (s[l] << 24);
		}
		case 11: {
			l = k + 10;
			c += (s[l] << 16);
		}
		case 10: {
			l = k + 9;
			c += (s[l] << 8);
		}
		case 9: {
			l = k + 8;
			c += (s[l]);
		}
		case 8: {
			l = k + 7;
			b += (s[l] << 24);
		}
		case 7: {
			l = k + 6;
			b += (s[l] << 16);
		}
		case 6: {
			l = k + 5;
			b += (s[l] << 8);
		}
		case 5: {
			l = k + 4;
			b += (s[l]);
		}
		case 4: {
			l = k + 3;
			a += (s[l] << 24);
		}
		case 3: {
			l = k + 2;
			a += (s[l] << 16);
		}
		case 2: {
			l = k + 1;
			a += (s[l] << 8);
		}
		case 1: {
			a += (s[k]);
			c ^= b; c -= b << 14 | b >> 18;
			a ^= c; a -= c << 11 | c >> 21;
			b ^= a; b -= a << 25 | a >> 7;
			c ^= b; c -= b << 16 | b >> 16;
			a ^= c; a -= c << 4 | c >> 28;
			b ^= a; b -= a << 14 | a >> 18;
			c ^= b; c -= b << 24 | b >> 8;
			break;
		}
			
		default:
			break;
	}
	
	return (static_cast<std::uint64_t>(b) << 32) | static_cast<std::uint64_t>(c) ;
}

//=============================================================================
std::uint32_t UOPData::HadAdler32(char* d, std::uintmax_t length )  {
	std::uint32_t a = 1 ;
	std::uint32_t b = 0 ;
	for (std::uintmax_t i = 0 ; i < length; i++){
		a = (a + (d[i] % 65521)) ;
		b = (b + a) % 65521 ;
	}
	return (b<<16) | a ;
}

//=============================================================================
std::string UOPData::fixedWidth(std::size_t value,int digitsCount ){
	std::ostringstream os;
	os<< std::setfill('0') << std::setw(digitsCount) << value;
	return os.str();
}

//=============================================================================
std::string UOPData::customizeHashString(const std::string &hashstring) {
	return std::string(hashstring) ;
}
//=============================================================================
void UOPData::buildHash(const std::string& hashkey, std::size_t maxhashes, int digitscount) {
	auto hashstring = hashkey ;
	auto secondstring = std::string("");
	for (std::size_t i = 0 ; i < maxhashes ; ++i) {
		auto index = fixedWidth(i,digitscount) ;
		auto hash = std::string(hashstring);
		auto pos = hash.find("#") ;
		hash.replace(pos, 1, index);
		auto hashvalue = HashLittle2(hash);
		_chunk_id1.insert_or_assign(hashvalue, i);
	}
}
//=============================================================================
std::uint32_t UOPData::getChunkID(std::uint64_t hashid){
	auto iter = _chunk_id1.find(hashid) ;
	if (iter != _chunk_id1.end()){
		return iter->second ;
	}
	throw std::out_of_range(std::string("UOP: No entry found for hash id: )" + std::to_string(hashid)));
}

//=============================================================================
bool UOPData::checkUOPFormat(std::uint32_t format){
	if (format != 0x50594D) {
		return false ;
	}
	return true ;

}
//=============================================================================
bool UOPData::checkUOPVersion(std::uint32_t version){
	if (version > 5) {
		return false ;
	}
	return true ;
}
//=============================================================================
UOPData::UTableEntry UOPData::readTableEntry(std::ifstream &input){
	UTableEntry entry ;
	
	input.read(reinterpret_cast<char*>(&entry.offset),8);
	input.read(reinterpret_cast<char*>(&entry.header_length),4);
	input.read(reinterpret_cast<char*>(&entry.compressed_length),4);
	input.read(reinterpret_cast<char*>(&entry.decompressed_length),4);
	input.read(reinterpret_cast<char*>(&entry.filehash),8);
	input.read(reinterpret_cast<char*>(&entry.data_block_hash),4);
	input.read(reinterpret_cast<char*>(&entry.compression),2);

	return entry ;

}
//=============================================================================
bool UOPData::decompress(std::vector<unsigned char> &source, std::vector<unsigned char> &dest){
	
	auto srcsize = static_cast<unsigned long>( source.size() );
	auto destsize = static_cast<unsigned long>( dest.size() );
	auto status = uncompress2(dest.data(), &destsize, source.data(), &srcsize);
	if (status != Z_OK){
		return false ;
	}
	return true ;
}
//=============================================================================
void UOPData::processData(std::vector<unsigned char> &data, std::uint32_t chunkid){
	
}
//=============================================================================
bool UOPData::loadUOP(std::ifstream &input){
	// Read the Header information
	std::uint32_t uidata =0;
	std::uint64_t ulldata = 0 ;
	
	input.read(reinterpret_cast<char*>(&uidata), 4) ;
	if (!checkUOPFormat(uidata)) {
		return false ;
	}
	input.read(reinterpret_cast<char*>(&uidata), 4) ;
	if (!checkUOPVersion(uidata)) {
		return false ;
	}
	// Deal with the hashes
	auto hashstring = _hashformats[_mytype] ;
	auto maxhash = _maxhashes[_mytype] ;
	auto digitcount = _fixedwidths[_mytype] ;
	hashstring = customizeHashString(hashstring) ;
	buildHash(hashstring, maxhash, digitcount) ;
	
	//strip off the time format
	input.read(reinterpret_cast<char*>(&uidata), 4) ;

	// Starting address
	input.read(reinterpret_cast<char*>(&ulldata), 8) ;
	auto startaddress = ulldata ;
	
	input.read(reinterpret_cast<char*>(&uidata), 4) ;
	//auto blocksize = static_cast<std::int32_t>(uidata) ;
	
	input.read(reinterpret_cast<char*>(&uidata), 4) ;
	//auto totalsize = static_cast<std::int32_t>(uidata) ;
	
	input.seekg(startaddress,std::ios::beg);
	
	std::uint64_t nextblock = 0 ;
	do {
		input.read(reinterpret_cast<char*>(&uidata), 4) ;
		auto blockfilecount = uidata ;
		
		input.read(reinterpret_cast<char*>(&ulldata), 8) ;
		nextblock  = ulldata ;
		auto index = 0 ;
		do {
			auto entry = readTableEntry(input) ;
			index++ ;
			if (entry.offset == 0) {
				continue ;
			}
			else if (_mytype == UOPData::multi) {
				if ((entry.decompressed_length==0) || (entry.filehash == 0x126D1E99DDEDEE0A)) {
					continue ;
				}
			}
			std::uint32_t chunkid = 0;
			try
			{
				chunkid = getChunkID(entry.filehash);
			}
			catch( ... )
			{
				break;

			}
			auto position = input.tellg();
			input.seekg(entry.offset + entry.header_length,std::ios::beg) ;
			std::vector<unsigned char> sourcedata ;
			sourcedata.resize(entry.compressed_length,0) ;
			input.read(reinterpret_cast<char*>(sourcedata.data()), entry.compressed_length) ;
			if (input.gcount() != entry.compressed_length) {
				continue ;
			}
			std::vector<unsigned char> data ;

			// We have the source data, do we need to decompress it?
			if (entry.compression == 1){
				std::vector<unsigned char> decompress_data;
				decompress_data.resize(entry.decompressed_length,0);
				decompress(sourcedata, decompress_data) ;
				data = decompress_data;
			}
			else {
				data = sourcedata ;
			}
			processData(data,chunkid);
			
			input.seekg(position,std::ios::beg);
			
		}while (index < blockfilecount);
		input.seekg(nextblock,std::ios::beg) ;
	}while(input.good());
	
	_chunk_id1.clear();
	
	return true ;
}
