//
//  StringUtility.cpp
//  Infinity
//
//

#include "StringUtility.hpp"
#include <cctype>
#include <regex>
#include <locale>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <filesystem>
#include <codecvt>
using namespace std::string_literals;

namespace oldstrutil
{
	//=====================================================================
	// Lowercase the string
	std::string lower( const std::string &value )
	{
		auto rValue = value;
		std::transform( rValue.begin(), rValue.end(), rValue.begin(), 
				   []( unsigned char c ){ return std::tolower( c ); } );
		return rValue;
	}

	//=====================================================================
	// Uppercase the string
	std::string upper( const std::string &value )
	{
		auto rValue = value;
		std::transform( rValue.begin(), rValue.end(), rValue.begin(),
				   []( unsigned char c ){ return std::toupper( c ); } );
		return rValue;
	}

	//=====================================================================
	// Remove leading whitespace
	std::string ltrim( const std::string &value )
	{
		auto count = 0;
		for( auto it = value.begin(); it != value.end(); it++ )
		{
			if( !isspace( *it ))
			{
				return value.substr( count );
			}
			else
			{
				count++;
			}
		}

		return std::string();
	}

	//=====================================================================
	// Remove trailing whitespace
	std::string rtrim( const std::string &value )
	{
		auto count = 0;
		for( auto it = value.rbegin(); it != value.rend(); it++ )
		{
			if( !isspace( *it ))
			{
				return value.substr( 0, value.size() - count );
			}
			else
			{
				count++;
			}
		}

		return std::string();
	}

	//=====================================================================
	// Remove leading/trailing whitespace
	std::string trim( const std::string &value )
	{
		return rtrim( ltrim( value ));
	}

	//=====================================================================
	// Remove leading/trailing whitepace, and reduce all other whitespace to one " "
	std::string simplify( const std::string &value )
	{
		auto tvalue = trim( value );
		auto rValue = ""s;
		auto toggle = false;
		for( auto &c : tvalue )
		{
			if( isspace( c ))
			{
				if( !toggle )
				{
					rValue = rValue + c;
					toggle = true;
				}
			}
			else
			{
				toggle = false;
				rValue = rValue + c;
			}
		}
		return rValue;
	}

	//=====================================================================
	// Removing everything after a delimiter. Normally "//" for comments
	std::string removeTrailing( const std::string& value, const std::string& delim )
	{
		auto loc = value.find( delim );
		if( loc == std::string::npos )
		{
			return value;
		}
		return value.substr( 0, loc );
	}

	//=====================================================================
	// Return the values between two delimitors.
	std::string contents( const std::string &value, const std::string& startdelim, const std::string &enddelim, std::string::size_type location )
	{
		auto loc = value.find_first_of( startdelim, location );
		if( loc == std::string::npos )
		{
			return std::string();
		}
		auto start_search = loc + startdelim.size();
		if( start_search >= value.size() )
		{
			return std::string();
		}
		auto end = value.find_first_of( enddelim, start_search );
		if( end == std::string::npos )
		{
			return std::string();
		}
		return value.substr( start_search, end - start_search );
	}

	//=====================================================================
	// Split a string based on a separator
	std::tuple<std::string, std::string> split( const std::string &value, const std::string &sep )
	{
		auto loc = value.find( sep );
		auto next = loc + sep.size();
		if(( loc == std::string::npos ) || ( next >= value.size() ))
		{
			return std::make_tuple( trim( value ), ""s );
		}
		else
		{
			return std::make_tuple( trim( value.substr( 0, loc )), trim( value.substr( next )));
		}
	}

	//+++++++++++++++++++++++++++++++++++++
	std::string format( std::size_t maxsize, const std::string fmtstring, ... )
	{

		std::va_list argptr;
		va_start( argptr, fmtstring );
		auto test = oldstrutil::format( fmtstring, argptr );
		if( maxsize > 0 )
		{
			if( test.size() > maxsize )
			{
				test = test.substr( 0, maxsize );
			}
		}
		return test;
	}

	//++++++++++++++++++++++++++++++++++++++
	std::string format( std::size_t maxsize, const std::string fmtstring, va_list& list )
	{
		auto temp = oldstrutil::format( fmtstring, list );
		if( maxsize > 0 )
		{
			if( temp.size() > maxsize )
			{
				temp = temp.substr( 0, maxsize );
			}
		}
		return temp;
	}

	//+++++++++++++++++++++++++++++++++++++
	std::string format( const std::string fmtstring, ... )
	{
		std::va_list argptr;
		va_start( argptr, fmtstring );
		return oldstrutil::format( fmtstring, argptr );
	}

	//++++++++++++++++++++++++++++++++++++++
	std::string format( const std::string fmtstring, va_list& list )
	{
		constexpr std::uintmax_t buffersize = 4096;
		char msg[buffersize];
		std::memset( msg, 0, buffersize );
		std::vsnprintf( msg, buffersize, fmtstring.c_str(), list );
		va_end( list );
		return std::string( msg );
	}

	//++++++++++++++++++++++++++++++++++++++++
	std::string formatMessage( const std::string& uformat, const std::string& data )
	{
		auto values = oldstrutil::sections( data, "," );
		auto format = uformat;
		auto pos = format.find_first_of( "%" );
		if( pos == std::string::npos )
		{
			return format;
		}
		size_t index = 0;
		while( pos != std::string::npos )
		{
			if( index >= values.size() )
			{
				break;
			}
			auto temp = values[index];
			index++;
			// now, we assume the formatting was done before the string, so basically we
			// replace the format specifier, with the string
			format.replace( pos, 2, temp );
			pos = format.find_first_of( "%" );
		}
		return format;
	}

	//++++++++++++++++++++++++++++++++++++++++++
	std::vector<std::string> sections( const std::string& value, const std::string& sep, std::string::size_type start, std::string::size_type end )
	{
		std::vector<std::string> rValue;

		auto pos = value.find( sep, start );
		while( pos < end )
		{
			if(( pos - start ) > 0 )
			{
				auto potential = value.substr( start, pos - start );
				rValue.push_back( potential );
				start = pos+sep.size();
			}
			else if( pos == start )
			{
				rValue.push_back( "" );
				start += sep.size();
			}
			pos = value.find( sep, start );
		}
		rValue.push_back( value.substr( start, pos - start ));
		return rValue;
	}

	//++++++++++++++++++++++++++++++++++++++++++
	std::uintmax_t sectionCount( const std::string& value, const std::string& sep, std::string::size_type start, std::string::size_type end )
	{
		return oldstrutil::sections( value, sep, start, end ).size();
	}

	//++++++++++++++++++++++++++++++++++++++++++
	std::string indexSection( const std::string& value, std::uintmax_t sectionindex, const std::string& sep, std::string::size_type start, std::string::size_type end )
	{
		auto sec = oldstrutil::sections( value, sep, start, end );
		if( sec.size() <= sectionindex )
		{
			return sec[sectionindex];
		}
		throw std::runtime_error( oldstrutil::format( "Section index %ul exceeded size %ul from sections %s", sectionindex, sec.size(), value.c_str() ));
	}

	//+++++++++++++++++++++++++++++++++++++++++
	std::string extractSection( const std::string& value, const std::string& sep, std::string::size_type start, std::string::size_type stop )
	{
		std::string data;
		size_t count = 0;
		size_t startoffset = 0;
		size_t stopoffset = 0;
		bool match = false;
		stopoffset = value.find( sep, startoffset );
		while( startoffset != std::string::npos )
		{
			if( count == start )
			{
				match = true;
				// We dont return the seperator, so jump over it
				size_t tempoffset = startoffset;
				while ( stopoffset != std::string::npos )
				{
					if( count == stop )
					{
						break;
					}
					else
					{
						tempoffset = stopoffset + sep.length();
						stopoffset = value.find( sep, tempoffset );
						count++;
					}
				}
				break;

			}
			else
			{
				if( stopoffset != std::string::npos )
				{
					startoffset = stopoffset + sep.length();
				}
				else
				{
					startoffset	= static_cast<size_t>( std::string::npos );
				}
				stopoffset = value.find( sep, startoffset );
			}
			count++;
		}
		if( match )
		{
			size_t length = stopoffset - startoffset;
			if( stopoffset == std::string::npos )
			{
				length = value.length() - startoffset;
			}
			data = value.substr( startoffset, length );
		}
		return data;
	}

	//++++++++++++++++++++++++++++++++++++++++++
	std::vector<std::string> breakSize( std::size_t maxsize, const std::string& input )
	{
		auto working = input;
		std::vector<std::string> total;
		while( working.size() > maxsize )
		{
			auto portion = working.substr( 0, maxsize );
			working.replace( 0, maxsize, "" );
			total.push_back( portion );
		}
		if( !working.empty() )
		{
			total.push_back( working );
		}
		return total;
	}

	//++++++++++++++++++++++++++++++++++++++++++
	std::string fixDirectory( const std::string& base )
	{
		std::string value = "\\";
		std::string::size_type index = 0;
		auto data = trim( base );
		while(( index = data.find( value, index )) != std::string::npos )
		{
			data = data.replace( index, 1, "/" );
		}
		if( data.find_last_of( "/" ) != data.size() - 1 )
		{
			data = data + "/";
		}

		return data;
	}

	//+++++++++++++++++++++++++++++++++++++++++++
	std::string replaceSlash( std::string& data )
	{
		std::string value = "\\";
		std::string::size_type index = 0;

		while(( index = data.find( value, index )) != std::string::npos )
		{
			data = data.replace( index, 1, "/" );
		}
		return data;
	}

	std::string number( char n, int base )
	{
		std::stringstream conversion;
		auto value = static_cast<std::int16_t>( n );

		switch( base )
		{
			case 10:
				conversion << std::dec << value;
				break;
			case 16:
				conversion << std::hex << value << std::dec;
				break;
			case 8:
				conversion << std::oct << value << std::dec;
				break;
			default:
				conversion << std::dec << value;
				break;
		}

		return conversion.str();
	}
	std::string number( unsigned char n, int base )
	{
		std::stringstream conversion;
		auto value = static_cast<std::uint16_t>( n );

		switch( base )
		{
			case 10:
				conversion << std::dec << value;
				break;
			case 16:
				conversion << std::hex << value << std::dec;
				break;
			case 8:
				conversion << std::oct << value << std::dec;
				break;
			default:
				conversion << std::dec << value;
				break;
		}

		return conversion.str();
	}

#if defined( _MSC_VER )
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
	// Decode a UTF8 string into a wstring
	std::wstring stringToWstring( const std::string& t_str )
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;

		// use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
		try
		{
			auto rValue = converter.from_bytes( t_str );
			return rValue;
		}
		catch( ... )
		{
			return L"";
		}
	}
#if defined( _MSC_VER )
#pragma warning(pop)
#endif

	// "Encode" a wstring into UTF8, while retaining any special wide characters
	std::string wStringToString( const std::wstring& t_str )
	{
		const size_t stringLen = t_str.size();

		std::string resultString;
		for( size_t i = 0; i < stringLen; ++i )
		{
			resultString.push_back( static_cast<char>( t_str[i] ));
		}

		return resultString;
	}

	// Convert from string to wstring and back to string
	std::string stringToWstringToString( const std::string& t_str )
	{
		return wStringToString( stringToWstring( t_str ));
	}
}
