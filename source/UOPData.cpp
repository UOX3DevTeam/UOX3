// Copyright © 2021 Charles Kerr. All rights reserved.
// Created on:  6/1/21

#include "UOPData.hpp"

#include <stdexcept>
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <zlib.h>		// We want the global first, and local as a backup.  On macOS we use the system zlib

using namespace std::string_literals;

//o------------------------------------------------------------------------------------------------o
// uopindex_t
//o------------------------------------------------------------------------------------------------o

auto UopIndex_st::HashLittle2( const std::string& s ) -> std::uint64_t
{
	std::uint32_t length = static_cast<std::uint32_t>( s.size() );
	std::uint32_t a;
	std::uint32_t b;
	std::uint32_t c;

	c = 0xDEADBEEF + static_cast<std::uint32_t>( length );
	a = c;
	b = c;
	int k = 0;

	while( length > 12 )
	{
		a += ( s[k] );
		a += ( s[k + 1] << 8 );
		a += ( s[k + 2] << 16 );
		a += ( s[k + 3] << 24 );
		b += ( s[k + 4] );
		b += ( s[k + 5] << 8 );
		b += ( s[k + 6] << 16 );
		b += ( s[k + 7] << 24 );
		c += ( s[k + 8]);
		c += ( s[k + 9] << 8 );
		c += ( s[k + 10] << 16 );
		c += ( s[k + 11] << 24 );

		a -= c; a ^= c << 4 | c >> 28; c += b;
		b -= a; b ^= a << 6 | a >> 26; a += c;
		c -= b; c ^= b << 8 | b >> 24; b += a;
		a -= c; a ^= c << 16 | c >> 16; c += b;
		b -= a; b ^= a << 19 | a >> 13; a += c;
		c -= b; c ^= b << 4 | b >> 28; b += a;

		length -= 12;
		k += 12;
	}

	if( length != 0 )
	{
		// Notice the lack of breaks!  we actually want it to fall through
		switch( length )
		{
			case 12:
				c += ( s[k + 11] << 24 );
				[[fallthrough]];
			case 11:
				c += ( s[k + 10] << 16 );
				[[fallthrough]];
			case 10:
				c += ( s[k + 9] << 8 );
				[[fallthrough]];
			case 9:
				c += ( s[k + 8] );
				[[fallthrough]];
			case 8:
				b += ( s[k + 7] << 24 );
				[[fallthrough]];
			case 7:
				b += ( s[k + 6] << 16 );
				[[fallthrough]];
			case 6:
				b += ( s[k + 5] << 8 );
				[[fallthrough]];
			case 5:
				b += ( s[k + 4] );
				[[fallthrough]];
			case 4:
				a += ( s[k + 3] << 24 );
				[[fallthrough]];
			case 3:
				a += ( s[k + 2] << 16 );
				[[fallthrough]];
			case 2:
				a += ( s[k + 1] << 8 );
				[[fallthrough]];
			case 1:
			{
				a += ( s[k] );
				c ^= b;
				c -= ( b << 14 ) | ( b >> 18 );
				a ^= c;
				a -= ( c << 11 ) | ( c >> 21 );
				b ^= a;
				b -= ( a << 25 ) | ( a >> 7 );
				c ^= b;
				c -= ( b << 16 ) | ( b >> 16 );
				a ^= c;
				a -= ( c << 4 )  | ( c >> 28 );
				b ^= a;
				b -= ( a << 14 ) | ( a >> 18 );
				c ^= b;
				c -= ( b << 24 ) | ( b >> 8 );
				break;
			}
			default:
				break;
		}
	}
	return ( static_cast<std::uint64_t>( b ) << 32 ) | static_cast<std::uint64_t>( c );
}
//===========================================================
auto UopIndex_st::HashAdler32( const std::vector<std::uint8_t> &data ) -> std::uint32_t
{
	std::uint32_t a = 1;
	std::uint32_t b = 0;
	for( const auto &entry : data )
	{
		a = ( a + static_cast<std::uint32_t>( entry )) % 65521;
		b = ( b + a ) % 65521;
	}
	return ( b << 16 ) | a;
}

//===========================================================
auto UopIndex_st::LoadHashes(const std::string &hashstring, size_t max_index) -> void
{
	hashes.clear();
	hashes.reserve(max_index);
	if( !hashstring.empty() && ( max_index > 0 ))
	{
		for( size_t i = 0; i <= max_index; i++ )
		{
			auto formatted = format( hashstring, i );
			hashes.push_back( HashLittle2( formatted ));
		}
	}
}
//===========================================================
UopIndex_st::UopIndex_st( const std::string &hashstring, size_t max_index )
{
	if( !hashstring.empty() && ( max_index != 0 ))
	{
		LoadHashes( hashstring, max_index );
	}
}
//===========================================================
auto UopIndex_st::operator[]( std::uint64_t hash ) const -> std::size_t
{
	auto iter = std::find( hashes.cbegin(), hashes.cend(), hash );
	if( iter != hashes.cend() )
	{
		return std::distance( hashes.cbegin(), iter );
	}
	return std::numeric_limits<std::size_t>::max();
}
//===========================================================
auto UopIndex_st::clear() -> void
{
	hashes.clear();
}

//=========================================================
// TableEntry_st
//=========================================================
//=========================================================
UopFile::TableEntry_st::TableEntry_st()
{
	offset = 0;
	headerLength = _entry_size;
	compressedLength = 0;
	decompressedLength = 0;
	identifer = 0;
	dataBlockHash = 0;
	compression = 0;
}
//===============================================================
auto UopFile::TableEntry_st::Load( std::istream &input ) -> UopFile::TableEntry_st &
{
	input.read( reinterpret_cast<char*>( &offset), sizeof( offset ));
	input.read( reinterpret_cast<char*>( &headerLength), sizeof( headerLength ));
	input.read( reinterpret_cast<char*>( &compressedLength), sizeof( compressedLength ));
	input.read( reinterpret_cast<char*>( &decompressedLength), sizeof( decompressedLength ));
	input.read( reinterpret_cast<char*>( &identifer), sizeof( identifer ));
	input.read( reinterpret_cast<char*>( &dataBlockHash), sizeof( dataBlockHash ));
	input.read( reinterpret_cast<char*>( &compression), sizeof( compression ));
	return *this;
}
//===============================================================
auto UopFile::TableEntry_st::Save( std::ostream &output ) -> UopFile::TableEntry_st &
{
	output.write( reinterpret_cast<char*>( &offset ), sizeof( offset ));
	output.write( reinterpret_cast<char*>( &headerLength ), sizeof( headerLength ));
	output.write( reinterpret_cast<char*>( &compressedLength ), sizeof( compressedLength ));
	output.write( reinterpret_cast<char*>( &decompressedLength ), sizeof( decompressedLength ));
	output.write( reinterpret_cast<char*>( &identifer ), sizeof( identifer ));
	output.write( reinterpret_cast<char*>( &dataBlockHash ), sizeof( dataBlockHash ));
	output.write( reinterpret_cast<char*>( &compression ), sizeof( compression ));
	return *this;
}


/************************************************************************
 zlib wrappers for compression
 ***********************************************************************/
//=============================================================================
auto UopFile::zdecompress( const std::vector<uint8_t> &source, std::size_t decompressed_size) const -> std::vector<uint8_t>
{
	// uLongf is from zlib.h
	auto srcsize = static_cast<uLongf>( source.size() );
	auto destsize = static_cast<uLongf>( decompressed_size );
	std::vector<uint8_t> dest( decompressed_size, 0 );
	auto status = uncompress2( dest.data(), &destsize, source.data(), &srcsize );
	if( status != Z_OK )
	{
		dest.clear();
		dest.resize( 0 );
		return dest;
	}
	dest.resize( destsize );
	return dest;
}
//=============================================================================
auto UopFile::zcompress( const std::vector<uint8_t> &source ) const -> std::vector<uint8_t>
{
	auto size = compressBound( static_cast<uLong>( source.size() ));
	std::vector<uint8_t> rdata( size, 0 );
	auto status = compress2( reinterpret_cast<Bytef*>( rdata.data() ), &size, reinterpret_cast<const Bytef*>( source.data() ), static_cast<uLongf>( source.size() ), Z_DEFAULT_COMPRESSION );
	if( status != Z_OK )
	{
		rdata.clear();
		return rdata;
	}
	rdata.resize( size );
	return rdata;
}

//=============================================================================
auto UopFile::IsUop( const std::string &filepath ) const -> bool
{
	std::ifstream input( filepath, std::ios::binary );
	if( input.is_open() )
	{
		// Make sure this is a format and version we understand
		std::uint32_t sig  = 0;
		std::uint32_t version = 0;
		input.read( reinterpret_cast<char*>( &sig ), sizeof( sig ));
		input.read( reinterpret_cast<char*>( &version ), sizeof( version ));
		input.seekg( 4, std::ios::cur );
		if(( version <= _uop_version ) && ( sig == _uop_identifer ))
		{
			return true;
		}
	}
	return false;
}

//===============================================================
//===============================================================
auto UopFile::NonIndexHash( std::uint64_t hash, std::size_t entry, [[maybe_unused]] std::vector<std::uint8_t> &data ) -> bool
{
	auto fill = std::cerr.fill();

	std::cerr << "Hashlookup failed for entry "s << entry << " with a hash of " << std::showbase 
		<< std::hex << std::setfill( '0' ) << std::setw( 16 ) << hash << std::dec << std::noshowbase 
		<< std::setfill( fill ) << std::setw( 0 ) << std::endl;
	return false;
}

//===============================================================
auto UopFile::LoadUop( const std::string &filepath, std::size_t max_hashindex, const std::string &hashformat1, const std::string &hashformat2 ) -> bool
{
	std::ifstream input( filepath, std::ios::binary );
	if( !input.is_open() )
	{
		return false;
	}

	// Make sure this is a format and version we understand
	std::uint32_t sig  = 0;
	std::uint32_t version = 0;
	input.read( reinterpret_cast<char*>( &sig ), sizeof( sig ));
	input.read( reinterpret_cast<char*>( &version ), sizeof( version ));
	input.seekg( 4, std::ios::cur );
	if(( version > _uop_version ) || ( sig != _uop_identifer ))
	{
		return false;
	}
	auto hashstorage1 = UopIndex_st( hashformat1, max_hashindex );
	auto hashstorage2 = UopIndex_st( hashformat2, max_hashindex );

	std::uint64_t table_offset = 0;
	std::uint32_t tablesize = 0;
	std::uint32_t maxentry = 0;
	input.read( reinterpret_cast<char*>( &table_offset ), sizeof( table_offset ));
	input.read( reinterpret_cast<char*>( &tablesize ), sizeof( tablesize ));
	input.read( reinterpret_cast<char*>( &maxentry ), sizeof( maxentry ));

	// Read the table entries
	input.seekg( table_offset, std::ios::beg );
	std::vector<TableEntry_st> entries;
	entries.reserve( maxentry );
	while(( table_offset != 0 ) && ( !input.eof() ) && input.good() )
	{
		input.read( reinterpret_cast<char*>( &tablesize ), sizeof( tablesize ));
		input.read( reinterpret_cast<char*>( &table_offset ), sizeof( table_offset ));
		for( std::uint32_t i = 0; i < tablesize; i++ )
		{
			TableEntry_st entry;
			entry.Load( input );
			entries.push_back( entry );
		}
		if(( table_offset != 0 ) && ( !input.eof() ) && input.good() )
		{
			input.seekg( table_offset, std::ios::beg );
		}
	}
	auto current_entry = 0;
	//std::cout <<"Number of entries: " << entries.size()<<std::endl;
	for( auto &entry : entries )
	{
		// Now loop through entries
		if(( entry.identifer != 0 ) && ( entry.compressedLength != 0 ))
		{
			input.seekg( entry.offset + entry.headerLength, std::ios::beg );
			auto size = ( entry.compression == 0 ) ? entry.decompressedLength : entry.compressedLength;
			std::vector<std::uint8_t> uopdata( size, 0 );
			input.read( reinterpret_cast<char*>( uopdata.data() ), size );
			if( entry.compression == 1 )
			{
				uopdata = zdecompress( uopdata, entry.decompressedLength );
			}

			// First see if we should even do anything with this hash
			if( ProcessHash( entry.identifer, current_entry, uopdata ))
			{
				// Yes, we should!
				// Can we find an index?
				auto index = hashstorage1[entry.identifer];
				if( index == std::numeric_limits<std::size_t>::max() )
				{
					index = hashstorage2[entry.identifer];
				}
				if( index == std::numeric_limits<std::size_t>::max() )
				{
					if( !NonIndexHash( entry.identifer, current_entry, uopdata ))
					{
						return false;
					}
				}
				ProcessEntry( current_entry, index, uopdata );
			}
		}
		current_entry++;
	}
	return EndUopProcessing();
}
//==============================================================================
auto UopFile::WriteUop( const std::string &filepath ) -> bool
{
	static constexpr std::int32_t tableSize = 100;
	static constexpr std::int64_t firstTable = 0x200;
	static constexpr std::uint32_t timeStamp = 0xFD23EC43;
	static constexpr char pad = 0;
	static constexpr std::int32_t zero = 0;
	static constexpr std::int64_t bigZero = 0;
	static constexpr std::int32_t one = 1;
	auto blankTable = std::vector<char>( TableEntry_st::_entry_size * tableSize, 0 );
	auto compress = static_cast<std::uint16_t>( WriteCompress() );
	std::vector<unsigned char> emptyTableEntry( TableEntry_st::_entry_size, 0 );
	auto numberOfEntries = EntriesToWrite();

	// First can we even open the file
	auto output = std::ofstream( filepath, std::ios::binary );
	if( !output.is_open() )
	{
		return false;
	}
	// write out the signature and version
	output.write( reinterpret_cast<const char*>( &_uop_identifer ), sizeof( _uop_identifer ));
	output.write( reinterpret_cast<const char*>( &_uop_version ), sizeof( _uop_version ));
	output.write( reinterpret_cast<const char*>( &timeStamp ), sizeof( timeStamp ));
	output.write( reinterpret_cast<const char*>( &firstTable ), sizeof( firstTable ));
	output.write( reinterpret_cast<const char*>( &tableSize ), sizeof( tableSize ));
	output.write( reinterpret_cast<char*>( &numberOfEntries), sizeof( numberOfEntries ));
	output.write( reinterpret_cast<const char*>( &one ), sizeof( one ));
	output.write( reinterpret_cast<const char*>( &one ), sizeof( one ));
	output.write( reinterpret_cast<const char*>( &zero ), sizeof( zero ));
	for( auto i = 0x28; i < firstTable; ++i )
	{
		output.write( &pad, sizeof( pad ));
	}
	auto numerTables = numberOfEntries / tableSize + ((( numberOfEntries % tableSize ) > 0 ) ? 1 : 0 );
	auto tables = std::vector<TableEntry_st>( tableSize );
	// We are going to write place holders for our table,
	// and then the data
	for( auto i = 0; i< numerTables; ++i )
	{
		std::uint64_t current_table = output.tellp();
		auto idxStart = i * tableSize;
		auto idxEnd = std::min((( i + 1 ) * tableSize ), numberOfEntries );
		int delta = idxEnd - idxStart;
		output.write( reinterpret_cast<char*>( &delta ), sizeof( delta )); // files are in this block
		output.write( reinterpret_cast<const char*>( &bigZero ), sizeof( bigZero )); // next table, fill in later
		// we need to write out a dummy table
		output.write( blankTable.data(), blankTable.size() );
		// now we will create our table in memory, and write out data at the same time
		// data
		int data_entry = 0;
		for( int j = idxStart; j < idxEnd; ++j, ++data_entry )
		{
			auto rawdata = EntryForWrite( j );
			unsigned int sizeDecompressed = static_cast<unsigned int>( rawdata.size() );
			unsigned int sizeOut = sizeDecompressed;
			if(( compress != 0 ) && ( sizeDecompressed > 0 ))
			{
				auto dataout = this->zcompress( rawdata );
				sizeOut = static_cast<unsigned int>( dataout.size() );
				rawdata = dataout;
			}
			tables[data_entry].offset = output.tellp();
			tables[data_entry].compression = compress;
			tables[data_entry].compressedLength = sizeOut;
			tables[data_entry].decompressedLength = sizeDecompressed;
			auto hashkey = WriteHash(data_entry + i*tableSize);
			tables[data_entry].identifer =  UopIndex_st::HashLittle2( hashkey );

			if( sizeDecompressed > 0 )
			{
				tables[data_entry].dataBlockHash = UopIndex_st::HashAdler32( rawdata );
				// write out the data
				output.write( reinterpret_cast<char*>( rawdata.data() ), rawdata.size() );
			}
		}
		std::uint64_t nextTable = output.tellp();
		// Go back and fix the table header
		if( i < numerTables -1 )
		{
			output.seekp( current_table + 4, std::ios::beg );
			output.write( reinterpret_cast<char*>( &nextTable ), sizeof( nextTable ));
		}
		else
		{
			output.seekp( current_table + 12, std::ios::beg ); // We need to fix the next table address
		}
		auto tableEntry = 0;
		for( int j = idxStart; j < idxEnd; ++j, ++tableEntry )
		{
			output.write( reinterpret_cast<char*>( &tables[tableEntry].offset ), sizeof(tables[tableEntry].offset));
			output.write( reinterpret_cast<const char*>( &zero ), sizeof( zero ));
			output.write( reinterpret_cast<char*>( &tables[tableEntry].compressedLength ), sizeof( tables[tableEntry].compressedLength ));
			output.write( reinterpret_cast<char*>( &tables[tableEntry].decompressedLength ), sizeof( tables[tableEntry].decompressedLength ));
			output.write( reinterpret_cast<char*>( &tables[tableEntry].identifer ), sizeof( tables[tableEntry].identifer ));
			output.write( reinterpret_cast<char*>( &tables[tableEntry].dataBlockHash ), sizeof( tables[tableEntry].dataBlockHash ));
			output.write( reinterpret_cast<char*>( &tables[tableEntry].compression ), sizeof( tables[tableEntry].compression ));
		}
		// Fill the remainder with entry entries
		for( ; tableEntry < tableSize; ++tableEntry )
		{
			output.write( reinterpret_cast<const char*>( emptyTableEntry.data() ), emptyTableEntry.size() );
		}
		output.seekp( nextTable, std::ios::beg );
	}
	return true;
}
