// Copyright © 2021 Charles Kerr. All rights reserved.
// Created on:  6/1/21

#include "UOPData.hpp"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include "zlib.h"		// Not <> as we are using our local one
//+++++++++++++++++++++++++++++++++++++++++++++++++++
//
//		Methods for UOPData
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++
namespace UO {
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
	UOPData::UOPData() {
		_mytype = invalid ;
	}
	
	//=============================================================================
	std::string UOPData::buildHashString(const std::string& hashformat, std::size_t id) {
		char buffer[200] ;
		std::snprintf(buffer,200, hashformat.c_str(),id);
		return std::string(buffer);
	}
	//=============================================================================
	void UOPData::buildHashes(std::size_t maxhashes,const std::string& hashkey, const std::string &hashkey2) {
		_chunk_id1.clear();
		_chunk_id2.clear();
		for (std::size_t i = 0 ; i < maxhashes ; ++i) {
			auto hash = buildHashString(hashkey, i);
			auto hashvalue = HashLittle2(hash);
			_chunk_id1.insert_or_assign(hashvalue, i);
			if (!hashkey2.empty()){
				auto hash = buildHashString(hashkey2, i);
				auto hashvalue = HashLittle2(hash);
				_chunk_id2.insert_or_assign(hashvalue, i);
			}
		}
	}

	//=============================================================================
	std::uint32_t UOPData::getChunkID(std::uint64_t hashid){
		auto iter = _chunk_id1.find(hashid) ;
		if (iter != _chunk_id1.end()){
			return iter->second ;
		}
		else {
			auto iter = _chunk_id2.find(hashid) ;
			if (iter != _chunk_id2.end()){
				return iter->second ;
			}
		}
		throw std::out_of_range(std::string("UOP: No entry found for hash id: )" + std::to_string(hashid)));
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
	std::vector<UOPData::UTableEntry> UOPData::gatherTableEntry(std::ifstream &input,std::uint32_t maxentry){
		std::vector<UTableEntry> rvalue ;
		rvalue.reserve(maxentry);
		for (auto i = 0; i<maxentry;i++){
			rvalue.push_back(readTableEntry(input));
		}
		return rvalue ;
	}
	
	//============================================================================
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
		// Build the hashes (gumps take two hashes, since the number of padding changed)
		buildHashes(_maxhashes, _hashformat,_hashformat2) ;
		// hashes are built

		//strip off the time format
		input.read(reinterpret_cast<char*>(&uidata), 4) ;
		
		// table address
		input.read(reinterpret_cast<char*>(&ulldata), 8) ;
		auto tableaddress = ulldata ;
		input.seekg(tableaddress,std::ios::beg);
		do {
			//strip number of entries in this table
			input.read(reinterpret_cast<char*>(&uidata), 4) ;
			auto numentries = uidata ;

			input.read(reinterpret_cast<char*>(&ulldata), 8) ;
			tableaddress = ulldata ;
			std::vector<unsigned char> sourcedata;
			std::vector<unsigned char> destdata ;
			auto entries = gatherTableEntry(input, numentries);
			for (auto entry: entries) {
				// For each entry, process it
				// if the offset is zero, we skip it
				if (entry.offset == 0){
					continue ;
				}
				if (_mytype == multi) {
					// We want to skip housing.bin
					if ((entry.decompressed_length==0) || (entry.filehash == 0x126D1E99DDEDEE0A)) {
						continue ;
					}
				}
			
				// get the chunkid
				std::uint32_t chunkid = 0 ;
				try {
					chunkid = getChunkID(entry.filehash) ;
				}
				catch(...) {
					// We couldn't find the hash, so did we build enough of them?
					break;
				}
				// we need to read in the data ;
				input.seekg(entry.offset+entry.header_length,std::ios::beg);

				sourcedata.clear();
				sourcedata.resize(entry.compressed_length,0);
				input.read(reinterpret_cast<char*>(sourcedata.data()),entry.compressed_length);
				if (entry.compression == 1){
					destdata.clear() ;
					destdata.resize(entry.decompressed_length,0);
					decompress(sourcedata, destdata) ;
					sourcedata = destdata ;
				}
				processData(sourcedata, chunkid);
			}
			input.seekg(tableaddress,std::ios::beg);
		}while( (tableaddress != 0) && input.good());
		return true ;
	}
	
	//=============================================================================
	void UOPData::processData(std::vector<unsigned char> &data, std::uint32_t chunkid){
		
	}
	
	//=============================================================================
	bool UOPData::decompress(std::vector<unsigned char> &source, std::vector<unsigned char> &dest){
		
		auto srcsize = static_cast<unsigned long>(source.size()) ;
		auto destsize = static_cast<unsigned long>(dest.size());
		auto status = uncompress2(dest.data(), &destsize, source.data(), &srcsize);
		if (status != Z_OK){
			return false ;
		}
		return true ;
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

}
