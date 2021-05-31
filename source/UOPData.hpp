// 
// Created on:  4/30/21

#ifndef UOPData_hpp
#define UOPData_hpp

#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <array>
#include <fstream>

class UOPData {
protected:
	struct UTableEntry {
		std::int64_t offset ;
		std::int32_t header_length ;
		std::int32_t compressed_length ;
		std::int32_t decompressed_length ;
		std::uint64_t filehash ;
		std::uint32_t data_block_hash ;
		std::int16_t compression ;

	};
	enum UOPType {invalid = -1,multi=0,map} ;
	UOPType _mytype ;
	static const std::array<std::string, 2> _hashformats ;
	static const std::array<std::int32_t, 2> _fixedwidths ;
	static const std::array<std::size_t, 2> _maxhashes ;

	std::map<std::uint64_t,std::uint32_t> _chunk_id1 ;
	std::vector<unsigned char> _uopdata ;

	
	std::uint64_t HashLittle2(const std::string& s)  ;
	std::uint32_t HadAdler32(char* d, std::uintmax_t length ) ;
	std::string fixedWidth(std::size_t value,int digitsCount=8 );
	
	virtual std::string customizeHashString(const std::string &hashstring) ;
	void buildHash(const std::string& hashkey, std::size_t maxhashes, int digitsCount);
	std::uint32_t getChunkID(std::uint64_t hashid);
	
	virtual bool checkUOPFormat(std::uint32_t format) ;
	virtual bool checkUOPVersion(std::uint32_t version);
	virtual void processData(std::vector<unsigned char> &data, std::uint32_t chunkid);
	
	bool decompress(std::vector<unsigned char> &source, std::vector<unsigned char> &dest);
	
	bool loadUOP(std::ifstream &input) ;
	UTableEntry readTableEntry(std::ifstream &input);
	
	
public:
	UOPData() ;
};

#endif /* UOPData_hpp */
