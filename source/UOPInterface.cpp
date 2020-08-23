//	UOPInterface.cpp
//
//	Created by Charles Kerr
//
//

#include "UOPInterface.hpp"
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <map>
#include <cmath>
#include <vector>

///+++++++++++++++++++++++++++++++++++
std::tuple<std::uintmax_t,char*> UOP::loadUOP(const std::string& uopname,  std::uint16_t mapnum){

    // we have some max's we use to improve performance
    constexpr std::uint32_t maxhashes = 200;
    
    char* mapdata = nullptr ;
    std::map<std::uint64_t,std::uint32_t> chunkids ;
    // add the number to our has format
    
    std::fstream uopstream(uopname,std::ifstream::binary|std::ifstream::in);
    if (!uopstream.is_open()){
        throw std::runtime_error(std::string("Unable to open uop file: ")+uopname);
    }
    std::uint32_t wordword = 0 ;
    uopstream.read(reinterpret_cast<char*>(&wordword),sizeof(wordword));
    if (wordword != 0x50594D) {
        uopstream.close();
        throw std::runtime_error(std::string("Unkown UOP format - ")+uopname);
    }
    // If we cared, we would get the version, and format
    uopstream.seekg(8,std::ifstream::cur);
    
    auto hashwithmap = std::string(hashformat).replace(9, 1, std::to_string(mapnum)) ;
    for (std::uint32_t i = 0 ; i < maxhashes ; ++i) {
        auto index = UOP::fixedWidth(i) ;
        auto hash = std::string(hashwithmap);
        auto pos = hash.find("#") ;
        hash.replace(pos, 1, index);
        auto hashvalue = UOP::HashLittle2(hash);
        chunkids.insert_or_assign(hashvalue, i);
        
    }
    // Hashes made
    std::int64_t table = 0;
    uopstream.read(reinterpret_cast<char*>(&table),sizeof(table));
    uopstream.seekg(table,std::ifstream::beg);
    std::uint32_t entries ;
    uopstream.read(reinterpret_cast<char*>(&entries),sizeof(entries));
    // we are not going to loop, but we do the next read to align
    uopstream.read(reinterpret_cast<char*>(&table),sizeof(table));
    std::vector<UOP::UTableEntry> offsets(entries);
    
    for (std::uint32_t i = 0 ; i< entries;++i){
        uopstream.read(reinterpret_cast<char*>(&offsets[i].m_Offset),sizeof(offsets[i].m_Offset));
        // header length
        uopstream.read(reinterpret_cast<char*>(&offsets[i].m_HeaderLength),sizeof(offsets[i].m_HeaderLength));
        uopstream.read(reinterpret_cast<char*>(&offsets[i].m_Size),sizeof(offsets[i].m_Size));  // compressed size

        uopstream.read(reinterpret_cast<char*>(&wordword),sizeof(wordword)); // decompressed size, not used

        uopstream.read(reinterpret_cast<char*>(&offsets[i].m_Identifier),sizeof(offsets[i].m_Identifier)); // filename hash (HashLittle2)

        uopstream.read(reinterpret_cast<char*>(&offsets[i].m_Hash),sizeof(offsets[i].m_Hash)); // data hash (Adler32)
        std::int16_t compression ;
        uopstream.read(reinterpret_cast<char*>(&compression),sizeof(compression)); // compression method (0 = none, 1 = zlib)
    }
    
    // We loop through now and get the max file size
    std::uintmax_t filesize = 0 ;
    for (std::uint32_t i = 0 ; i<entries ; ++i){
        if (offsets[i].m_Offset != 0){
            auto hashiter = chunkids.find(offsets[i].m_Identifier);
            if (hashiter == chunkids.end()){
                uopstream.close();
                
                throw std::runtime_error(std::string("hash not found in uop file ")+uopname);
            }
            auto chunkid = hashiter->second ;
            filesize = std::max<std::uintmax_t>(filesize,(0xC4000*chunkid) + static_cast<uintmax_t>(offsets[i].m_Size));
        }
    }
    // we now have the file size
    
    mapdata = new char[filesize] ;
    
    for (std::uint32_t i = 0 ; i<entries ; ++i){
        if (offsets[i].m_Offset != 0){
            auto hashiter = chunkids.find(offsets[i].m_Identifier);
            auto offset = (hashiter->second)*0xC4000;
            
            uopstream.seekg(offsets[i].m_Offset +offsets[i].m_HeaderLength,std::ifstream::beg);
            uopstream.read(reinterpret_cast<char*>(mapdata+offset),offsets[i].m_Size);
        }
    }
    uopstream.close();
    
    return std::tuple(filesize,mapdata);

}


//============================================================================
/*
 Hash routines for the UOP file
 */

//==========================================================================
//
//          Static hash
//

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
std::uint64_t UOP::HashLittle2(const std::string& s) {
    
    std::uint32_t length = static_cast<std::uint32_t>(s.size()) ;
    std::uint32_t a ;
    std::uint32_t b ;
    std::uint32_t c ;
    
    c = 0xDEADBEEF + static_cast<std::uint32_t>(length) ;
    a = c;
    b = c ;
    std::uint32_t k = 0 ;
    
    while (length > 12){
        a += static_cast<std::uint32_t>(s[k]) ;
        
        a += static_cast<std::uint32_t>(s[k+1]) << 8 ;
        a += static_cast<std::uint32_t>(s[k+2]) << 16 ;
        a += static_cast<std::uint32_t>(s[k+3]) << 24 ;
        b += static_cast<std::uint32_t>(s[k+4]) ;
        b += static_cast<std::uint32_t>(s[k+5]) << 8 ;
        b += static_cast<std::uint32_t>(s[k+6]) << 16 ;
        b += static_cast<std::uint32_t>(s[k+7]) << 24 ;
        c += static_cast<std::uint32_t>(s[k+8]) ;
        c += static_cast<std::uint32_t>(s[k+9]) << 8 ;
        c += static_cast<std::uint32_t>(s[k+10]) << 16 ;
        c += static_cast<std::uint32_t>(s[k+11]) << 24 ;
        
        a -= c; a ^= c << 4 | c >> 28; c += b;
        b -= a; b ^= a << 6 | a >> 26; a += c;
        c -= b; c ^= b << 8 | b >> 24; b += a;
        a -= c; a ^= c << 16 | c >> 16; c += b;
        b -= a; b ^= a << 19 | a >> 13; a += c;
        c -= b; c ^= b << 4 | b >> 28; b += a;
        
        length -= 12 ;
        k += 12 ;
    }
    
    // Notice the lack of breaks!  we actually want it to fall through
    switch (length) {
        case 12: {
            c += static_cast<std::uint32_t>(s[k + 11]) << 24;
        }
        case 11: {
            c += static_cast<std::uint32_t>(s[k + 10]) << 16;
        }
        case 10: {
            c += static_cast<std::uint32_t>(s[k + 9]) << 8;
        }
        case 9: {
            c += static_cast<std::uint32_t>(s[k + 8]) ;
        }
        case 8: {
            b += static_cast<std::uint32_t>(s[k + 7]) << 24 ;
        }
        case 7: {
            b += static_cast<std::uint32_t>(s[k + 6]) << 16 ;
        }
        case 6: {
            b += static_cast<std::uint32_t>(s[k + 5]) << 8 ;
        }
        case 5: {
            b += static_cast<std::uint32_t>(s[k + 4])  ;
        }
        case 4: {
            a += static_cast<std::uint32_t>(s[k + 3]) << 24 ;
        }
        case 3: {
            a += static_cast<std::uint32_t>(s[k + 2]) << 16 ;
        }
        case 2: {
            a += static_cast<std::uint32_t>(s[k + 1]) << 8 ;
        }
        case 1: {
            a += static_cast<std::uint32_t>(s[k ])  ;
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

//+++++++++++++++++++++++++++++++++++++++++++
std::uint32_t UOP::HadAdler32(char* d, std::uintmax_t length )  {
    std::uint32_t a = 1 ;
    std::uint32_t b = 0 ;
    for (std::uintmax_t i = 0 ; i < length; i++){
        a = (a + (d[i] % 65521)) ;
        b = (b + a) % 65521 ;
    }
    return (b<<16) | a ;
}

//+++++++++++++++++++++++++++++++++++
std::string UOP::fixedWidth(std::uint32_t value,int digitsCount ){
    std::ostringstream os;
    os<<std::setfill('0')<<std::setw(digitsCount)<<value;
    return os.str();
}
