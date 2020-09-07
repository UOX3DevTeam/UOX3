//	UOPInterface.hpp
//
//
//

#ifndef UOPInterface_hpp
#define UOPInterface_hpp

#include <string>
#include <cstdint>
#include <tuple>

// All of this was lifted from UOFiddler, and then converted to C++
// So hats off to them, and the folks that also got the information from
// These hash functions are said to have come from
// http://burtleburtle.net/bob/c/lookup3.c
//

namespace  UOP
{

    struct UTableEntry {
        std::int64_t m_Offset ;
        std::int32_t m_HeaderLength ;
        std::int32_t m_Size ;
        std::uint64_t m_Identifier ;
        std::uint32_t m_Hash ;
    };
    std::uint64_t HashLittle2(const std::string& s)  ;
    std::uint32_t HadAdler32(char* d, std::uintmax_t length ) ;
    static constexpr auto hashformat = "build/map?legacymul/#.dat";
    std::string fixedWidth(std::uint32_t value,int digitsCount=8 );
    std::tuple<std::uintmax_t,char*> loadUOP(const std::string& uopname, std::uint16_t mapnum);

};

#endif /* UOPInterface_hpp */
