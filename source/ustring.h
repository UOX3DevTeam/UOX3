#ifndef __STRING_H
#define __STRING_H
// Ssytem Includes
#include <string>
#include <cstdarg>
#include <cstdio> 
#include "stream.h"
#include <algorithm>


typedef std::string stdstring ;

namespace UOX
{

// Defines

#define MYWHITESPACE " \t\v\f\0 "
#define COMMENTTAG "//"

//#define MYNONWHITESPACE "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-=`~!@#$%^&*()_+|,<.>/?;:'[{]}\"\\"
//! String class with extensions
/*! This string class mimics many of the capaiblities provided by QString in the 
	Trolltech QT library.  It has methods to handle number conversion's, convert
	to upper and lower, spit a string into sections based on delimitors, and 
	remove comment strings.
*/
class UString : public stdstring
{
public:
	//! Base constructor 
	UString() ;
	//! Create a UString from another string
	UString(const std::string& str) ;
	//! Create a UString from another string, starting at the specified index
	UString(const std::string& str, std::string::size_type str_idx) ;
	//! Create a UString from another string, starting at the specified index and for the specified number of characters 
	UString(const std::string& str, std::string::size_type str_idx, std::string::size_type str_num) ;
	//! Create a UString from a char delimited string
	UString(const char* cstr) ;
	//! Create a UString from a char delimted string, using the specified number of characters
	UString(const char* chars, std::string::size_type chars_len) ;
	//! Create a Ustring that repeats the character specified
	UString(std::string::size_type num, char c) ;
	//! Remove all leading, trailing white space, and all redundant white space within the string
	UString simplifyWhiteSpace() const ;
	//! Remove all trailing and leading white space
	UString stripWhiteSpace() const ;
	//! Using the specified sep as a delimitor, return the specifed section of the string
	UString section(std::string sep, int start, int stop=std::string::npos) ;
	//! Overloaded member of the above
	UString section(const char* sep, int start, int stop=std::string::npos) ;

	//!  How many "sections" are in this string ?
	int sectionCount(std::string sep) ;
	//!  How many "sections" are in this string ?
	int sectionCount(const char* sep) ;

	unsigned char toUByte( bool *ok = 0, int base = -1 ) const;
	char toByte( bool *ok = 0, int base = 10 ) const;

	unsigned short toUShort ( bool * ok = 0, int base = -1 ) const;
	short toShort ( bool * ok = 0, int base = 10 ) const ;

	int toInt ( bool * ok = 0, int base = -1 ) const;
	unsigned int toUInt ( bool * ok = 0, int base = -1 ) const;

	long toLong ( bool * ok = 0, int base = -1 ) const;
	unsigned long toULong ( bool * ok = 0, int base = -1 ) const;

	float toFloat(bool * ok=0) const ;
	double toDouble(bool * ok=0) const ;

	UString  setNum ( short n, int base = 10 );
	UString  setNum ( unsigned short n, int base = 10 );
	UString  setNum ( int n, int base = 10 );
	UString  setNum ( unsigned int n, int base = 10 );
	UString  setNum ( long n, int base = 10 );
	UString  setNum ( unsigned long n, int base = 10 );
	UString  setNum ( float n ) ;
	UString setNum(double n) ;

	UString lower () const;
	UString upper () const;

	UString removeComment() const ;
	UString comment() const ;

	static std::string& replaceSlash( std::string& data );
	UString fixDirectory()  ;

	UString operator+(const char * ) ;
	bool operator==( const char *input );

	static UString number ( long n, int base = 10 ) ;
	static UString number ( unsigned long n, int base = 10 ) ;
	static UString number ( int n, int base = 10 )  ;
	static UString number ( unsigned int n, int base = 10 )  ;
	static UString number (float n) ;
	static UString number (double n) ;
	//static UString sprintf(std::string format,...) ;
	static UString sprintf(const char* format,...) ;
};

}

#endif
