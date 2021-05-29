// 
// Created on:  4/30/21

#ifndef UOPData_hpp
#define UOPData_hpp

#include <cstdint>
#include <string>
#include <map>

class UOPData {
protected:
	struct UTableEntry {
		std::int64_t m_Offset ;
		std::int32_t m_HeaderLength ;
		std::int32_t m_Size ;
		std::uint64_t m_Identifier ;
		std::uint32_t m_Hash ;
	};
	
	std::uint64_t HashLittle2(const std::string& s)  ;
	std::uint32_t HadAdler32(char* d, std::uintmax_t length ) ;
	std::string fixedWidth(std::size_t value,int digitsCount=8 );
	
	// we have some max's we use to improve performance
	std::size_t maxhashes ;
	virtual std::uint32_t getMaxHash() ;
	std::map<std::uint64_t,std::uint32_t> chunkids ;
	
public:
	UOPData() ;
};

#endif /* UOPData_hpp */
