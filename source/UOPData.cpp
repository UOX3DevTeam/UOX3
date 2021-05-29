// 
// Created on:  4/30/21

#include "UOPData.hpp"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <algorithm>
//+++++++++++++++++++++++++++++++++++++++++++++++++++
//
//		Methods for UOPData
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UOPData::UOPData() {
	maxhashes=getMaxHash() ;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
std::uint32_t UOPData::getMaxHash(){
	return 200 ;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
		case 11: {
			l = k + 10;
			c += (s[l] << 16);
		}
		case 10: {
			l = k + 9;
			c += (s[l] << 8);
		}
		case 9: {
			l = k + 8;
			c += (s[l]);
		}
		case 8: {
			l = k + 7;
			b += (s[l] << 24);
		}
		case 7: {
			l = k + 6;
			b += (s[l] << 16);
		}
		case 6: {
			l = k + 5;
			b += (s[l] << 8);
		}
		case 5: {
			l = k + 4;
			b += (s[l]);
		}
		case 4: {
			l = k + 3;
			a += (s[l] << 24);
		}
		case 3: {
			l = k + 2;
			a += (s[l] << 16);
		}
		case 2: {
			l = k + 1;
			a += (s[l] << 8);
		}
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

//+++++++++++++++++++++++++++++++++++++++++++
std::uint32_t UOPData::HadAdler32(char* d, std::uintmax_t length )  {
	std::uint32_t a = 1 ;
	std::uint32_t b = 0 ;
	for (std::uintmax_t i = 0 ; i < length; i++){
		a = (a + (d[i] % 65521)) ;
		b = (b + a) % 65521 ;
	}
	return (b<<16) | a ;
}

//+++++++++++++++++++++++++++++++++++
std::string UOPData::fixedWidth(std::size_t value,int digitsCount ){
	std::ostringstream os;
	os<< std::setfill('0') << std::setw(digitsCount) << value;
	return os.str();
}
