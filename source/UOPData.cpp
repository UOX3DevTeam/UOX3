// Copyright � 2021 Charles Kerr. All rights reserved.
// Created on:  6/1/21

#include "UOPData.hpp"

#include <stdexcept>
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <cmath>


#include <zlib.h>		// We want the global first, and local as a backup.  On macOS we use the system zlib

using namespace std::string_literals ;

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
			_chunk_id1.insert_or_assign(hashvalue, static_cast<std::uint32_t>(i));
			if (!hashkey2.empty()){
				auto hash = buildHashString(hashkey2, i);
				auto hashvalue = HashLittle2(hash);
				_chunk_id2.insert_or_assign(hashvalue, static_cast<std::uint32_t>(i));
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
		for (size_t i = 0; i<maxentry;i++){
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
			[[fallthrough]];
			case 11: {
				l = k + 10;
				c += (s[l] << 16);
			}
			[[fallthrough]];
			case 10: {
				l = k + 9;
				c += (s[l] << 8);
			}
			[[fallthrough]];
			case 9: {
				l = k + 8;
				c += (s[l]);
			}
			[[fallthrough]];
			case 8: {
				l = k + 7;
				b += (s[l] << 24);
			}
			[[fallthrough]];
			case 7: {
				l = k + 6;
				b += (s[l] << 16);
			}
			[[fallthrough]];
			case 6: {
				l = k + 5;
				b += (s[l] << 8);
			}
			[[fallthrough]];
			case 5: {
				l = k + 4;
				b += (s[l]);
			}
			[[fallthrough]];
			case 4: {
				l = k + 3;
				a += (s[l] << 24);
			}
			[[fallthrough]];
			case 3: {
				l = k + 2;
				a += (s[l] << 16);
			}
			[[fallthrough]];
			case 2: {
				l = k + 1;
				a += (s[l] << 8);
			}
			[[fallthrough]];
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


//=================================================================================================
// all code above can be deleted


//===========================================================
// uopindex_t
//===========================================================
//===========================================================
auto uopindex_t::hashLittle2(const std::string& s) ->std::uint64_t{
	std::uint32_t length = static_cast<std::uint32_t>(s.size()) ;
	std::uint32_t a ;
	std::uint32_t b ;
	std::uint32_t c ;
	
	c = 0xDEADBEEF + static_cast<std::uint32_t>(length) ;
	a = c;
	b = c ;
	int k = 0 ;
	
	while (length > 12){
		a += (s[k]);
		a += (s[k+1] << 8);
		a += (s[k+2] << 16);
		a += (s[k+3] << 24);
		b += (s[k+4]);
		b += (s[k+5] << 8);
		b += (s[k+6] << 16);
		b += (s[k+7] << 24);
		c += (s[k+8]);
		c += (s[k+9] << 8);
		c += (s[k+10] << 16);
		c += (s[k+11] << 24);
		
		a -= c; a ^= c << 4 | c >> 28; c += b;
		b -= a; b ^= a << 6 | a >> 26; a += c;
		c -= b; c ^= b << 8 | b >> 24; b += a;
		a -= c; a ^= c << 16 | c >> 16; c += b;
		b -= a; b ^= a << 19 | a >> 13; a += c;
		c -= b; c ^= b << 4 | b >> 28; b += a;
		
		length -= 12 ;
		k += 12;
	}
	
	if (length != 0){
		// Notice the lack of breaks!  we actually want it to fall through
		switch (length) {
			case 12:
				c += (s[k+11] << 24);
				[[fallthrough]];
			case 11:
				c += (s[k+10] << 16);
				[[fallthrough]];
			case 10:
				c += (s[k+9] << 8);
				[[fallthrough]];
			case 9:
				c += (s[k+8]);
				[[fallthrough]];
			case 8:
				b += (s[k+7] << 24);
				[[fallthrough]];
			case 7:
				b += (s[k+6] << 16);
				[[fallthrough]];
			case 6:
				b += (s[k+5] << 8);
				[[fallthrough]];
			case 5:
				b += (s[k+4]);
				[[fallthrough]];
			case 4:
				a += (s[k+3] << 24);
				[[fallthrough]];
			case 3:
				a += (s[k+2] << 16);
				[[fallthrough]];
			case 2:
				a += (s[k+1] << 8);
				[[fallthrough]];
			case 1: {
				a += (s[k]);
				c ^= b;
				c -= (b << 14) | (b >> 18);
				a ^= c;
				a -= (c << 11) | (c >> 21);
				b ^= a;
				b -= (a << 25) | (a >> 7);
				c ^= b;
				c -= (b << 16) | (b >> 16);
				a ^= c;
				a -= (c << 4)  | (c >> 28);
				b ^= a;
				b -= (a << 14) | (a >> 18);
				c ^= b;
				c -= (b << 24) | (b >> 8);
				break;
			}
				
			default:
				break;
		}
		
	}
	
	return (static_cast<std::uint64_t>(b) << 32) | static_cast<std::uint64_t>(c) ;
	
}
//===========================================================
auto uopindex_t::hashAdler32(const std::vector<std::uint8_t> &data) ->std::uint32_t {
	std::uint32_t a = 1 ;
	std::uint32_t b = 0 ;
	for (const auto &entry : data ) {
		a = (a + static_cast<std::uint32_t>(entry)) % 65521;
		b = (b + a) % 65521 ;
	}
	return (b<<16)| a ;
	
}

//===========================================================
auto uopindex_t::load(const std::string &hashstring, size_t max_index) ->void{
	hashes.clear();
	hashes.reserve(max_index);
	if (!hashstring.empty() && (max_index>0)){
		for (size_t i=0 ; i<= max_index;i++){
			auto formatted = format(hashstring, i);
			hashes.push_back(hashLittle2(formatted));
		}
	}
}
//===========================================================
uopindex_t::uopindex_t(const std::string &hashstring, size_t max_index){
	if (!hashstring.empty() && (max_index!=0)) {
		load(hashstring,max_index);
	}
}
//===========================================================
auto uopindex_t::operator[](std::uint64_t hash) const -> std::size_t{
	auto iter = std::find(hashes.cbegin(),hashes.cend(),hash);
	if (iter != hashes.cend()){
		return std::distance(hashes.cbegin(), iter);
	}
	return std::numeric_limits<std::size_t>::max();
	
}
//===========================================================
auto uopindex_t::clear() ->void {
	hashes.clear();
}



//=========================================================
// table_entry
//=========================================================
//=========================================================
uopfile::table_entry::table_entry(){
	offset = 0 ;
	header_length = _entry_size ;
	compressed_length = 0 ;
	decompressed_length = 0 ;
	identifer = 0;
	data_block_hash = 0 ;
	compression = 0 ;
}
//===============================================================
auto 	uopfile::table_entry::load(std::istream &input) ->uopfile::table_entry & {
	input.read(reinterpret_cast<char*>(&offset),sizeof(offset));
	input.read(reinterpret_cast<char*>(&header_length),sizeof(header_length));
	input.read(reinterpret_cast<char*>(&compressed_length),sizeof(compressed_length));
	input.read(reinterpret_cast<char*>(&decompressed_length),sizeof(decompressed_length));
	input.read(reinterpret_cast<char*>(&identifer),sizeof(identifer));
	input.read(reinterpret_cast<char*>(&data_block_hash),sizeof(data_block_hash));
	input.read(reinterpret_cast<char*>(&compression),sizeof(compression));
	return *this ;
}
//===============================================================
auto	uopfile::table_entry::save(std::ostream &output) ->uopfile::table_entry & {
	output.write(reinterpret_cast<char*>(&offset),sizeof(offset));
	output.write(reinterpret_cast<char*>(&header_length),sizeof(header_length));
	output.write(reinterpret_cast<char*>(&compressed_length),sizeof(compressed_length));
	output.write(reinterpret_cast<char*>(&decompressed_length),sizeof(decompressed_length));
	output.write(reinterpret_cast<char*>(&identifer),sizeof(identifer));
	output.write(reinterpret_cast<char*>(&data_block_hash),sizeof(data_block_hash));
	output.write(reinterpret_cast<char*>(&compression),sizeof(compression));
	return *this ;
}



/************************************************************************
 zlib wrappers for compression
 ***********************************************************************/
//=============================================================================
auto uopfile::zdecompress(const std::vector<uint8_t> &source, std::size_t decompressed_size) const ->std::vector<uint8_t>{
	// uLongf is from zlib.h
	auto srcsize = static_cast<uLongf>(source.size()) ;
	auto destsize = static_cast<uLongf>(decompressed_size);
	std::vector<uint8_t> dest(decompressed_size,0);
	auto status = uncompress2(dest.data(), &destsize, source.data(), &srcsize);
	if (status != Z_OK){
		dest.clear() ;
		dest.resize(0) ;
		return dest ;
	}
	dest.resize(destsize);
	return dest ;
}
//=============================================================================
auto uopfile::zcompress(const std::vector<uint8_t> &source) const ->std::vector<uint8_t>{
	auto size = compressBound(static_cast<uLong>(source.size()));
	std::vector<uint8_t> rdata(size,0);
	auto status = compress2(reinterpret_cast<Bytef*>(rdata.data()), &size, reinterpret_cast<const Bytef*>(source.data()), static_cast<uLongf>(source.size()),Z_DEFAULT_COMPRESSION);
	if (status != Z_OK){
		rdata.clear();
		return rdata ;
	}
	rdata.resize(size) ;
	return rdata;
}


//=============================================================================
auto uopfile::isUOP(const std::string &filepath) const ->bool {
	std::ifstream input(filepath, std::ios::binary);
	if (input.is_open()){
		// Make sure this is a format and version we understand
		std::uint32_t sig  = 0 ;
		std::uint32_t version = 0 ;
		input.read(reinterpret_cast<char*>(&sig),sizeof(sig));
		input.read(reinterpret_cast<char*>(&version),sizeof(version));
		input.seekg(4,std::ios::cur);
		if ((version <= _uop_version) && (sig == _uop_identifer)){
			return true;
		}
		
	}
	return false ;
	
}

//===============================================================
//===============================================================
auto uopfile::nonIndexHash(std::uint64_t hash, std::size_t entry, std::vector<std::uint8_t> &data) ->bool{
	auto fill = std::cerr.fill() ;
	
	std::cerr << "Hashlookup failed for entry "s << entry << " with a hash of " <<std::showbase << std::hex << std::setfill('0') << std::setw(16)<<hash<<std::dec <<std::noshowbase <<std::setfill(fill)<<std::setw(0)<<std::endl;
	return false;
}

//===============================================================
auto uopfile::loadUOP(const std::string &filepath, std::size_t max_hashindex , const std::string &hashformat1, const std::string &hashformat2 )->bool{
	std::ifstream input(filepath, std::ios::binary);
	if (!input.is_open()){
		return false ;
	}
	// Make sure this is a format and version we understand
	std::uint32_t sig  = 0 ;
	std::uint32_t version = 0 ;
	input.read(reinterpret_cast<char*>(&sig),sizeof(sig));
	input.read(reinterpret_cast<char*>(&version),sizeof(version));
	input.seekg(4,std::ios::cur);
	if ((version > _uop_version) || (sig != _uop_identifer)){
		return false ;
	}
	auto hashstorage1 = uopindex_t(hashformat1,max_hashindex);
	auto hashstorage2 = uopindex_t(hashformat2,max_hashindex);
	
	std::uint64_t table_offset = 0;
	std::uint32_t tablesize = 0 ;
	std::uint32_t maxentry = 0 ;
	input.read(reinterpret_cast<char*>(&table_offset),sizeof(table_offset));
	input.read(reinterpret_cast<char*>(&tablesize),sizeof(tablesize));
	input.read(reinterpret_cast<char*>(&maxentry),sizeof(maxentry));
	
	// Read the table entries
	input.seekg(table_offset,std::ios::beg) ;
	std::vector<table_entry> entries ;
	entries.reserve(maxentry);
	while ((table_offset!= 0) && (!input.eof()) && input.good()){
		input.read(reinterpret_cast<char*>(&tablesize),sizeof(tablesize));
		input.read(reinterpret_cast<char*>(&table_offset),sizeof(table_offset));
		for (std::uint32_t i=0 ;i < tablesize;i++){
			table_entry entry ;
			entry.load(input);
			entries.push_back(entry);
		}
		if ((table_offset!=0) && (!input.eof()) && input.good()){
			input.seekg(table_offset,std::ios::beg);
		}
	}
	auto current_entry = 0 ;
	//std::cout <<"Number of entries: " << entries.size()<<std::endl;
	for (auto &entry : entries){
		// Now loop through entries
		if ((entry.identifer != 0 ) && (entry.compressed_length != 0)) {
			input.seekg(entry.offset+entry.header_length,std::ios::beg) ;
			auto size = (entry.compression==0)?entry.decompressed_length : entry.compressed_length ;
			std::vector<std::uint8_t> uopdata(size,0);
			input.read(reinterpret_cast<char*>(uopdata.data()),size);
			if (entry.compression == 1){
				uopdata = zdecompress(uopdata, entry.decompressed_length);
			}
			
			// First see if we should even do anything with this hash
			if (processHash(entry.identifer, current_entry, uopdata)) {
				// Yes, we should!
				// Can we find an index?
				
				
				auto 	index = hashstorage1[entry.identifer];
				if (index == std::numeric_limits<std::size_t>::max()){
					
					index = hashstorage2[entry.identifer];
				}
				if (index == std::numeric_limits<std::size_t>::max()){
					
					if (!nonIndexHash(entry.identifer, current_entry, uopdata)){
						return false ;
					}
				}
				
				processEntry(current_entry, index, uopdata);
			}
		}
		current_entry++ ;
	}
	return endUOPProcessing();
}
//==============================================================================
auto uopfile::writeUOP(const std::string &filepath) ->bool {
	static constexpr std::int32_t table_size = 100 ;
	static constexpr std::int64_t first_table = 0x200 ;
	static constexpr std::uint32_t timestamp = 0xFD23EC43 ;
	static constexpr char pad = 0 ;
	static constexpr std::int32_t zero = 0 ;
	static constexpr std::int64_t bigzero = 0 ;
	static constexpr std::int32_t one = 1 ;
	auto blanktable = std::vector<char>(table_entry::_entry_size*table_size,0) ;
	auto compress = static_cast<std::uint16_t>(writeCompress());
	std::vector<unsigned char> emptyTableEntry(table_entry::_entry_size,0);
	auto number_of_entries = entriesToWrite() ;
	
	// First can we even open the file
	auto output = std::ofstream(filepath,std::ios::binary);
	if (!output.is_open()){
		return false ;
	}
	// write out the signature and version
	output.write(reinterpret_cast<const char*>(&_uop_identifer), sizeof(_uop_identifer));
	output.write(reinterpret_cast<const char*>(&_uop_version), sizeof(_uop_version));
	output.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
	
	output.write(reinterpret_cast<const char*>(&first_table),sizeof(first_table));
	output.write(reinterpret_cast<const char*>(&table_size),sizeof(table_size));
	output.write(reinterpret_cast<char*>(&number_of_entries),sizeof(number_of_entries));
	output.write(reinterpret_cast<const char*>(&one),sizeof(one));
	output.write(reinterpret_cast<const char*>(&one),sizeof(one));
	output.write(reinterpret_cast<const char*>(&zero),sizeof(zero));
	for (auto i = 0x28; i < first_table;++i){
		output.write(&pad,sizeof(pad));
	}
	auto number_tables = number_of_entries/table_size + (((number_of_entries%table_size) > 0)?1:0);
	
	auto tables = std::vector<table_entry>(table_size);
	// We are going to write place holders for our table,
	// and then the data
	for (auto i = 0 ; i< number_tables ; ++i){
		std::uint64_t current_table = output.tellp() ;
		auto idxStart = i * table_size ;
		auto idxEnd = std::min(((i+1) * table_size),number_of_entries) ;
		int delta = idxEnd - idxStart ;
		output.write(reinterpret_cast<char*>(&delta),sizeof(delta)); // files are in this block
		output.write(reinterpret_cast<const char*>(&bigzero),sizeof(bigzero)); // next table, fill in later
		// we need to write out a dummy table
		output.write(blanktable.data(),blanktable.size());
		// now we will create our table in memory, and write out data at the same time
		// data
		int data_entry = 0 ;
		for (int j= idxStart;j<idxEnd; ++j,++data_entry){
			auto rawdata = entryForWrite(j);
			
			unsigned int sizeDecompressed = static_cast<unsigned int>(rawdata.size()) ;
			unsigned int sizeOut = sizeDecompressed ;
			if ((compress != 0) && (sizeDecompressed>0)){
				
				auto dataout = this->zcompress(rawdata) ;
				sizeOut = static_cast<unsigned int>(dataout.size()) ;
				rawdata = dataout ;
			}
			tables[data_entry].offset = output.tellp() ;
			tables[data_entry].compression = compress ;
			tables[data_entry].compressed_length = sizeOut ;
			tables[data_entry].decompressed_length = sizeDecompressed ;
			auto hashkey = writeHash(data_entry + i*table_size);
			tables[data_entry].identifer =  uopindex_t::hashLittle2(hashkey);
			
			if (sizeDecompressed>0){
				tables[data_entry].data_block_hash = uopindex_t::hashAdler32(rawdata);
				// write out the data
				output.write(reinterpret_cast<char*>(rawdata.data()),rawdata.size());
			}
		}
		std::uint64_t nextTable = output.tellp() ;
		// Go back and fix the table header
		if (i < number_tables -1){
			output.seekp(current_table + 4,std::ios::beg);
			output.write(reinterpret_cast<char*>(&nextTable),sizeof(nextTable));
			
		}
		else {
			output.seekp(current_table + 12,std::ios::beg); // We need to fix the next table address
		}
		auto table_entry = 0 ;
		for (int j=idxStart;j<idxEnd;++j,++table_entry ){
			output.write(reinterpret_cast<char*>(&tables[table_entry].offset),sizeof(tables[table_entry].offset));
			output.write(reinterpret_cast<const char*>(&zero),sizeof(zero));
			output.write(reinterpret_cast<char*>(&tables[table_entry].compressed_length),sizeof(tables[table_entry].compressed_length));
			output.write(reinterpret_cast<char*>(&tables[table_entry].decompressed_length),sizeof(tables[table_entry].decompressed_length));
			output.write(reinterpret_cast<char*>(&tables[table_entry].identifer),sizeof(tables[table_entry].identifer));
			output.write(reinterpret_cast<char*>(&tables[table_entry].data_block_hash),sizeof(tables[table_entry].data_block_hash));
			output.write(reinterpret_cast<char*>(&tables[table_entry].compression),sizeof(tables[table_entry].compression));
			
		}
		// Fill the remainder with entry entries
		for (;table_entry < table_size;++table_entry){
			output.write(reinterpret_cast<const char*>(emptyTableEntry.data()),emptyTableEntry.size());
		}
		output.seekp(nextTable,std::ios::beg);
	}
	return true ;
}

