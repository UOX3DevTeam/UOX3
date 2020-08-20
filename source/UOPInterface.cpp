//	UOPInterface.cpp
//
//	Created by Charles Kerr
//
//

#include "UOPInterface.hpp"
#include <stdexcept>
#include <vector>
#include <map>
#include <fstream>
#include <cstring>
#include <filesystem>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iostream>

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::uint64_t UOP::HashLittle2( std::string s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Static hash
//o-----------------------------------------------------------------------------------------------o
std::uint64_t UOP::HashLittle2( std::string s )
{
    std::int32_t length = static_cast<std::int32_t>( s.size() );
    std::uint32_t a;
    std::uint32_t b;
    std::uint32_t c;
    
    c = 0xDEADBEEF + static_cast<std::uint32_t>( length );
    a = c;
    b = c;
    std::int32_t k = 0;
    
    while( length > 12 )
	{
        a += s[k];
        
        a += static_cast<std::uint32_t>( s[k+1] ) << 8;
        a += static_cast<std::uint32_t>( s[k+2] ) << 16;
        a += static_cast<std::uint32_t>( s[k+3] ) << 24;
        b += s[k+4];
        b += static_cast<std::uint32_t>( s[k+5] ) << 8;
        b += static_cast<std::uint32_t>( s[k+6] ) << 16;
        b += static_cast<std::uint32_t>( s[k+7] ) << 24;
        c += s[k+8];
        c += static_cast<std::uint32_t>( s[k+9] ) << 8;
        c += static_cast<std::uint32_t>( s[k+10] ) << 16;
        c += static_cast<std::uint32_t>( s[k+11] ) << 24;
        
        a -= c; a ^= c << 4 | c >> 28; c += b;
        b -= a; b ^= a << 6 | a >> 26; a += c;
        c -= b; c ^= b << 8 | b >> 24; b += a;
        a -= c; a ^= c << 16 | c >> 16; c += b;
        b -= a; b ^= a << 19 | a >> 13; a += c;
        c -= b; c ^= b << 4 | b >> 28; b += a;
        
        length -= 12;
        k += 12;
    }
    
    // Notice the lack of breaks!  we actually want it to fall through
    switch( length )
	{
        case 12:
		{
            c += static_cast<std::uint32_t>( s[k + 11] ) << 24;
        }
        case 11:
		{
            c += static_cast<std::uint32_t>( s[k + 10] ) << 16;
        }
        case 10:
		{
            c += static_cast<std::uint32_t>( s[k + 9] ) << 8;
        }
        case 9:
		{
            c += static_cast<std::uint32_t>( s[k + 8] );
        }
        case 8:
		{
            b += static_cast<std::uint32_t>( s[k + 7] ) << 24;
        }
        case 7:
		{
            b += static_cast<std::uint32_t>( s[k + 6] ) << 16;
        }
        case 6:
		{
            b += static_cast<std::uint32_t>( s[k + 5] ) << 8;
        }
        case 5:
		{
            b += static_cast<std::uint32_t>( s[k + 4] )  ;
        }
        case 4:
		{
            a += static_cast<std::uint32_t>( s[k + 3] ) << 24;
        }
        case 3:
		{
            a += static_cast<std::uint32_t>( s[k + 2] ) << 16;
        }
        case 2:
		{
            a += static_cast<std::uint32_t>( s[k + 1] ) << 8;
        }
        case 1:
		{
            a += static_cast<std::uint32_t>( s[k ] )  ;
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
    
    return ( static_cast<std::uint64_t>( b ) << 32 ) | static_cast<std::uint64_t>( c );
    
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
std::uint32_t UOP::HadAdler32( char* d, std::uintmax_t length )
{
    std::uint32_t a = 1;
    std::uint32_t b = 0;
    for( auto i = 0; i < length; i++ )
	{
        a = ( a + ( d[i] % 65521 ));
        b = ( b + a ) % 65521;
    }
    return ( b << 16 ) | a;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
std::string UOP::ToString (std::uint64_t value, int digitsCount )
{
    std::ostringstream os;
    os << std::setfill( '0' ) << std::setw( digitsCount ) << value;
    return os.str();
}

std::string UOP::ApplyIndexToHash( const std::string &hash, std::uint64_t index )
{
    auto sub = UOP::ToString( index );
    auto temp = hash;
    auto pos = temp.find( "#" );
    if( pos != std::string::npos )
	{
        return temp.replace( pos, 1, sub );
    }
    return temp;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string UOP::GetHashFormat( UOP::FileType type, int typeIndex, std::int32_t& maxId )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Hash filename formats (remember: lower case!)
//o-----------------------------------------------------------------------------------------------o
std::string UOP::GetHashFormat( UOP::FileType type, int typeIndex, std::int32_t& maxId )
{
    /*
     * MaxID is only used for constructing a lookup table.
     * Decrease to save some possibly unneeded computation.
     */
    maxId = 0x7FFFF;
    switch ( type )
    {
        case UOP::FileType::ArtLegacyMul:
        {
            maxId = 0x13FDC; //  requires this exact index length to recognize UOHS art files
            return std::string( "build/artlegacymul/#.tga" );
        }
        case UOP::FileType::GumpartLegacyMul:
        {
            // MaxID = 0xEF3C on 7.0.8.2
            return std::string( "build/gumpartlegacymul/#.tga" );
        }
        case UOP::FileType::MapLegacyMul:
        {
            // MaxID = 0x71 on 7.0.8.2 for Fel/Tram
            return std::string( "build/map") + std::to_string( typeIndex ) + std::string( "legacymul/#.dat" );
            
        }
        case UOP::FileType::SoundLegacyMul:
        {
            // MaxID = 0x1000 on 7.0.8.2
            return std::string( "build/soundlegacymul/#.dat" );
        }
        default:
        {
            throw std::runtime_error( "Unknown file type to get hash!" );
        }
    }
    
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string UOP::GetHashFormat( UOP::FileType type, int typeIndex, std::int32_t& maxId )
//|					std::uint64_t UOP::readUInt64( std::ifstream& input )
//|					std::int32_t UOP::readInt32( std::ifstream& input )
//|					std::uint32_t UOP::readUInt32( std::ifstream& input )
//|					std::int16_t UOP::readInt16( std::ifstream& input )
//|					std::uint16_t UOP::readUInt16( std::ifstream& input )
//|					std::int8_t UOP::readInt8( std::ifstream& input )
//|					std::uint8_t UOP::readUInt8( std::ifstream& input )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Some stream accessors to make it easier to match C# code
//o-----------------------------------------------------------------------------------------------o
std::int64_t UOP::readInt64( std::ifstream& input )
{
    std::int64_t temp;
    input.read( reinterpret_cast<char*>( &temp ), sizeof( temp ));
    return temp;
}
std::uint64_t UOP::readUInt64( std::ifstream& input )
{
    std::uint64_t temp;
    input.read( reinterpret_cast<char*>( &temp ), sizeof( temp ));
    return temp;
}
std::int32_t UOP::readInt32( std::ifstream& input )
{
    std::int32_t temp;
    input.read( reinterpret_cast<char*>( &temp ), sizeof( temp ));
    return temp;
}
std::uint32_t UOP::readUInt32( std::ifstream& input )
{
    std::uint32_t temp;
    input.read( reinterpret_cast<char*>( &temp ), sizeof( temp ));
    return temp;
}
std::int16_t UOP::readInt16( std::ifstream& input )
{
    std::int16_t temp;
    input.read( reinterpret_cast<char*>( &temp ), sizeof( temp ));
    return temp;
}
std::uint16_t UOP::readUInt16( std::ifstream& input )
{
    std::int16_t temp;
    input.read( reinterpret_cast<char*>( &temp ), sizeof( temp ));
    return temp;
}
std::int8_t UOP::readInt8( std::ifstream& input )
{
    std::int8_t temp;
    input.read( reinterpret_cast<char*>( &temp ), sizeof( temp ));
    return temp;
}
std::uint8_t UOP::readUInt8( std::ifstream& input )
{
    std::uint8_t temp;
    input.read( reinterpret_cast<char*>( &temp ), sizeof( temp ));
    return temp;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++
std::tuple<char*,std::uintmax_t> UOP::mapUOP( const std::string& uopfile, int mapnumber )
{
    std::uintmax_t mulsize = 0;
    std::map<std::uint64_t, std::int32_t> chunkIds;
    std::int32_t maxId =0;
    auto format = UOP::GetHashFormat( UOP::FileType::MapLegacyMul, mapnumber, maxId );
    // Setup the hashes
    for( auto i = 0; i < maxId; ++i )
	{
        auto aformat = UOP::ApplyIndexToHash( format, i );
        auto hash = UOP::HashLittle2( aformat );
        
        try {
            chunkIds.insert( std::make_pair( hash, i ));
        }
        catch ( const std::exception &e ) {
            throw std::runtime_error( std::string( "Duplicate hash on entry: " ) + std::to_string( i ) + std::string( "Error was: " ) +e.what() );
        }
    }
    
    // we have the index hashes setup
    std::ifstream uopstream;
    uopstream.open( uopfile, std::ifstream::in | std::ifstream::binary );
    if( !uopstream.is_open() )
	{
        throw std::runtime_error( std::string( "Unable to open: " ) + uopfile );
    }
    auto magic = UOP::readInt32( uopstream );
    if( magic != 0x50594D )
	{
        throw std::runtime_error( std::string( "Not recognized UOP format: " ) + uopfile );
    }
    UOP::readInt32( uopstream ); //Version?
    UOP::readInt32( uopstream ); // format Timestamp?
    std::int64_t nextTable = UOP::readInt64( uopstream );
    do {
        uopstream.seekg( nextTable,std::ifstream::beg );
        const int entries = UOP::readInt32( uopstream );
        nextTable = UOP::readInt64( uopstream );
        if( entries > 0 )
		{
            auto offsets = new TableEntry[entries];
            for( auto i = 0; i < entries; ++i )
			{
                offsets[i].m_Offset = UOP::readInt64( uopstream ); // header length
                offsets[i].m_HeaderLength = UOP::readInt32( uopstream );
                offsets[i].m_Size  = UOP::readInt32( uopstream ); // compressed size
                UOP::readInt32( uopstream ); // decompressed size
                offsets[i].m_Identifier = UOP::readInt64( uopstream ); // filename hash ( HashLittle2 )
                offsets[i].m_Hash = UOP::readInt32( uopstream );  // data hash ( Adler32 )
                UOP::readInt16( uopstream ); // compression method ( 0 = none, 1 = zlib )
            }
            for( auto i = 0; i < entries; ++i )
			{
                if( offsets[i].m_Offset != 0 )
				{
                    auto iter = chunkIds.find( offsets[i].m_Identifier );
                    if( iter == chunkIds.end() )
					{
                        uopstream.close();
                        throw std::runtime_error( std::string( "Unknown identifer : " ) + std::to_string( i ) + std::string( " : " ) + 
							std::to_string( offsets[i].m_Identifier ) + std::string( " encoutered on: " ) + uopfile );
                    }
                    auto chunkId = iter->second;
                    mulsize = std::max<std::uint64_t>( mulsize, chunkId * 0xC4000 + offsets[i].m_Size );
                }
            }
			delete[] offsets;
        }
    } while( nextTable != 0 );

    // At this point, we have the mulsize we need
    if( mulsize == 0 )
	{
        uopstream.close();
        throw std::runtime_error( std::string( "Could not determine size to write data to for " ) + uopfile );
    }
    auto muldata = new char[mulsize];
    uopstream.seekg( 12,std::ifstream::beg );
    nextTable = UOP::readInt64( uopstream );
    
    do {
        // Table header
        uopstream.seekg( nextTable,std::ifstream::beg );
        const int nentries = UOP::readInt32( uopstream );
        nextTable = UOP::readInt64( uopstream );
        if( nentries > 0 )
		{
            auto offsets = new TableEntry[nentries];
            for( auto i = 0; i < nentries; ++i )
			{
                offsets[i].m_Offset = UOP::readInt64( uopstream ); // header length
                offsets[i].m_HeaderLength = UOP::readInt32( uopstream );
                offsets[i].m_Size  = UOP::readInt32( uopstream ); // compressed size
                UOP::readInt32( uopstream ); // decompressed size
                offsets[i].m_Identifier = UOP::readInt64( uopstream ); // filename hash (HashLittle2)
                offsets[i].m_Hash = UOP::readInt32( uopstream );  // data hash (Adler32)
                UOP::readInt16( uopstream ); // compression method (0 = none, 1 = zlib)
            }
            for( auto i = 0; i < nentries; ++i )
			{
                if( offsets[i].m_Offset != 0 )
				{
                    auto iter = chunkIds.find( offsets[i].m_Identifier );
                    if( iter == chunkIds.end() )
					{
                        uopstream.close();
                        if( muldata != nullptr )
						{
                            delete[] muldata;
                            muldata= nullptr;
                        }
                        throw std::runtime_error( std::string( "Unknown identifer : " ) + std::to_string( i ) + std::string( " : " ) + 
							std::to_string( offsets[i].m_Identifier ) + std::string( " encoutered on: " ) + uopfile );
                    }
                    auto chunkId = iter->second;
                    uopstream.seekg( offsets[i].m_Offset + offsets[i].m_HeaderLength, std::ifstream::beg );
                    uopstream.read( muldata + ( chunkId * 0xC4000 ), offsets[i].m_Size );
                }
            }
			delete[] offsets;
        }
    } while( nextTable != 0 );
    uopstream.close();
    return std::make_tuple( muldata,mulsize );
}
