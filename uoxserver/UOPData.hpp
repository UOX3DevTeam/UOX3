// Copyright © 2021 Charles Kerr. All rights reserved.
// Created on:  6/1/21

#ifndef UOPData_hpp
#define UOPData_hpp

#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <fstream>

namespace UO {
	class UOPData {
	protected:
		struct UTableEntry {
			std::int64_t	offset ;
			std::int32_t	header_length ;
			std::int32_t	compressed_length ;
			std::int32_t	decompressed_length ;
			std::uint64_t	filehash ;
			std::uint32_t	data_block_hash ;
			std::int16_t	compression ;
			
		};
		enum UOPType {invalid = -1,multi=0,map,gump,art,sound} ;
		//************************************************************
		// Subclasses need to set these variables before loading a UOP
		UOPType _mytype ;
		std::size_t _maxhashes ;
		std::string _hashformat ;
		std::string _hashformat2 ;  // other then gump, this can be left alone
		//************************************************************

		virtual bool checkUOPFormat(std::uint32_t format) ;
		virtual bool checkUOPVersion(std::uint32_t version);

		// hash stuff
		std::map<std::uint64_t,std::uint32_t> _chunk_id1 ;
		std::map<std::uint64_t,std::uint32_t> _chunk_id2 ;
		
		std::string   buildHashString(const std::string& hashformat, std::size_t id);
		void buildHashes(std::size_t maxhashes,const std::string& hashkey, const std::string &hashkey2="");
		std::uint32_t getChunkID(std::uint64_t hashid);
		
		// UOP table information
		UTableEntry readTableEntry(std::ifstream &input);
		std::vector<UTableEntry> gatherTableEntry(std::ifstream &input,std::uint32_t maxentry );
		
		
		bool loadUOP(std::ifstream &input) ;
		
		virtual void processData(std::vector<unsigned char> &data, std::uint32_t chunkid);
		

		// Data manipulation/formatting
		bool decompress(std::vector<unsigned char> &source, std::vector<unsigned char> &dest);
		std::uint64_t HashLittle2(const std::string& s)  ;
		std::uint32_t HadAdler32(char* d, std::uintmax_t length ) ;

		
	public:
		UOPData() ;
	};
}

#endif /* UOPData_hpp */
