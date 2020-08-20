//	UOPInterface.hpp
//
//	Created by Charles Kerr
//
//

#ifndef UOPInterface_hpp
#define UOPInterface_hpp

#include <string>
#include <cstdint>
#include <fstream>
#include <tuple>

// All of this was lifted from UOFiddler, and then converted to C++
// So hats off to them, and the folks that also got the information from
// These hash functions are said to have come from
// http://burtleburtle.net/bob/c/lookup3.c
//

namespace  UOP
{

enum class FileType
{
    ArtLegacyMul,
    GumpartLegacyMul,
    MapLegacyMul,
    SoundLegacyMul
};

struct IdxEntry
{
    std::int32_t m_Id;
    std::int32_t m_Offset;
    std::int32_t m_Size;
    std::int32_t m_Extra;
};

struct TableEntry
{
    std::int64_t m_Offset;
    std::int32_t m_HeaderLength;
    std::int32_t m_Size;
    std::uint64_t m_Identifier;
    std::uint32_t m_Hash;
};

std::uint64_t HashLittle2( std::string s );
std::uint32_t HadAdler32( char* d, std::uintmax_t length  );    
    
std::int32_t readInt32( std::ifstream& input );
std::uint32_t readUInt32( std::ifstream& input );
std::int16_t readInt16( std::ifstream& input );
std::uint16_t readUInt16( std::ifstream& input );
std::int8_t readInt8( std::ifstream& input );
std::uint8_t readUInt8( std::ifstream& input );
std::int64_t readInt64( std::ifstream& input );
std::uint64_t readUInt64( std::ifstream& input );
    
std::string GetHashFormat( UOP::FileType type, int typeIndex, std::int32_t& maxID );
std::string ApplyIndexToHash( const std::string &hash, std::uint64_t index );
// Need a way to get a fixed width of a number in a string
std::string ToString( std::uint64_t value, int digitsCount = 8 );
/*
    void ToUOP( const std::string& inFile, const std::string& inFileIdx, const std::string& outFile, FileType type, std::int32_t typeIndex);
    */
void FromUOP( const std::string& inFile, const std::string& outFile, const std::string& outFileIdx, UOP::FileType type, std::int32_t typeIndex );
std::tuple<char*,std::uintmax_t> mapUOP( const std::string& uopfile, int mapnumber );

};

#endif /* UOPInterface_hpp */
