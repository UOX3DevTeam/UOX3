#if defined(_MSC_VER) 
	#if _MSC_VER < 1300
		#pragma warning( disable : 4786 )
		#pragma warning( disable : 4514 )
		#pragma warning( disable : 4097 )
		#define UOXVALIST va_list
		#define NOSIZETYPE
	#else
		#define UOXVALIST std::va_list
	#endif
#else
	#define UOXVALIST std::va_list
#endif

#include "ustring.h" 
#include "Prerequisites.h"

namespace UOX
{

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString::UString(): stdstring() 
{
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString::UString(const std::string& str) : stdstring(str)
{
}
#if !defined( NOSIZETYPE )
UString::UString( const std::string& str, std::string::size_type str_idx ) : stdstring( str, str_idx )
{
} 
#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString::UString(const std::string& str, std::string::size_type str_idx, 
				 std::string::size_type str_num) : stdstring(str,str_idx,str_num)
{
} 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString::UString(const char* cstr) : stdstring(cstr)
{
} 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString::UString(const char* chars, std::string::size_type chars_len) :
stdstring(chars,chars_len)
{
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString::UString(std::string::size_type num, char c) : stdstring(num,c)
{
} 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString UString::stripWhiteSpace() const
{
	// Eliminate any leading white space
	UString data  = static_cast<UString>((*this));
	size_t offset = (*this).find_first_not_of(MYWHITESPACE) ;
	if( offset != std::string::npos )
	{
		data = (*this).substr(offset) ;
	}
	offset = data.find_last_not_of(MYWHITESPACE) ;
	if( offset!=std::string::npos )
	{
		data = data.substr(0,offset+1) ;
	}
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
UString UString::simplifyWhiteSpace() const
{
	UString working = (*this).stripWhiteSpace() ;
	UString data ="" ;
	//std::string::iterator iter ;
	//iter = working.begin() ;
	bool first = false ;
	char value[2] ;
	value[1] = 0 ;
	for ( unsigned int i = 0 ; i < working.length() ;i++ )
	{
		value[0] = working[i] ;
		std::string  test = value ;
		if( test.find_first_of(MYWHITESPACE) != std::string::npos )
		{
			if( !first )
			{
				data = (stdstring)(data) + test;
				first = true ;
			}

		}
		else
		{
			first = false ;
			data = data + value ;
		}
	}

	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString UString::section(const char* sep,int start,int stop)
{
	std::string temp = sep ;
	return (section(temp,start,stop) );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString UString::section(std::string sep,int start,int stop)
{
	UString data ;
	int count = -1 ;
	size_t startoffset = 0 ;
	size_t stopoffset = 0 ;
	bool match = false ;
	stopoffset = (*this).find(sep,startoffset);
	while ( startoffset != std::string::npos )
	{
		count++ ;
		if( count == start )
		{
			match = true ;
			// We dont return the seperator, so jump over it
			size_t tempoffset = startoffset ;
			while ( stopoffset != std::string::npos )
			{
				if( count == stop )
				{
					break ;
				}
				else
				{
					tempoffset = stopoffset + sep.length() ;
					stopoffset = (*this).find(sep,tempoffset) ;
					count++ ;
				}
			}
			break ;

		}
		else
		{
			if( stopoffset != std::string::npos )
				startoffset = stopoffset+ sep.length();
			else
				startoffset	= static_cast<size_t>(std::string::npos) ;
			stopoffset = (*this).find(sep,startoffset) ;

		}
	}
	if( match )
	{
		size_t length = stopoffset - startoffset ;
		if( stopoffset == std::string::npos )
			length = (*this).length() - startoffset ;

		data = (*this).substr(startoffset,length) ;
	}
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString  UString::setNum ( float n )
{
	std::stringstream input ;
	input << n ;
	UString data ;
	input >> data ;
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString  UString::setNum ( double n )
{
	std::stringstream input ;
	input << n ;
	UString data ;
	input >> data ;
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString  UString::setNum ( short n, int base  )
{
	std::stringstream input ;
	input << n ;
	UString data ;
	switch ( base )
	{
		case 10:
			input >> std::dec >> data ;
			break ;
		case 16:
			input.fill( '0' );
			input.width( 4 );
			input.setf( std::ios::uppercase );
			input >> std::hex >> std::right >> data ;
			break ;
		case 8:
			input >> std::oct>> data ;
			break ;
	}
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString  UString::setNum ( unsigned short n, int base  )
{
	std::stringstream input ;
	input << n ;
	UString data ;
	switch ( base )
	{
		case 10:
			input >> std::dec >> data ;
			break ;
		case 16:
			input.fill( '0' );
			input.width( 4 );
			input.setf( std::ios::uppercase );
			input >> std::hex >> std::right >> data ;
			break ;
		case 8:
			input >> std::oct>> data ;
			break ;
	}
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString  UString::setNum ( int n, int base  )
{
	std::stringstream input ;
	input << n ;
	UString data ;
	switch ( base )
	{
		case 10:
			input >> std::dec >> data ;
			break ;
		case 16:
			input.fill( '0' );
			input.width( 4 );
			input.setf( std::ios::uppercase );
			input >> std::hex >> std::right >> data ;
			break ;
		case 8:
			input >> std::oct>> data ;
			break ;
	}
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString  UString::setNum ( unsigned int n, int base  )
{
	std::stringstream input ;
	input << n ;
	UString data ;
	switch ( base )
	{
		case 10:
			input >> std::dec >> data ;
			break ;
		case 16:
			input.fill( '0' );
			input.width( 4 );
			input.setf( std::ios::uppercase );
			input >> std::hex >> std::right >> data ;
			break ;
		case 8:
			input >> std::oct>> data ;
			break ;
	}
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString  UString::setNum ( long n, int base  )
{
	std::stringstream input ;
	input << n ;
	UString data ;
	switch ( base )
	{
		case 10:
			input >> std::dec >> data ;
			break ;
		case 16:
			input.fill( '0' );
			input.width( 4 );
			input.setf( std::ios::uppercase );
			input >> std::hex >> std::right >> data ;
			break ;
		case 8:
			input >> std::oct>> data ;
			break ;
	}
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString  UString::setNum ( unsigned long n, int base  )
{
	std::stringstream input ;
	input << n ;
	UString data ;
	switch ( base )
	{
		case 10:
			input >> std::dec >> data ;
			break ;
		case 16:
			input.fill( '0' );
			input.width( 4 );
			input.setf( std::ios::uppercase );
			input >> std::hex >> std::right >> data ;
			break ;
		case 8:
			input >> std::oct>> data ;
			break ;
	}
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//UString & UString::setNum ( float n, char f = 'g', int prec = 6 ) ;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString UString::number (float n)
{
	UString data ;
	std::stringstream input ;
	input << n ;
	input >> data ;
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString UString::number (double n)
{
	UString data ;
	std::stringstream input ;
	input << n ;
	input >> data ;
	return ( data );
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString UString::number ( long n, int base  ) 
{
	UString data;
	std::stringstream input;
	switch( base )
	{
	case 10:
		input << std::dec << n;
		break;
	case 16:
		input << std::hex << n;
		break;
	case 8:
		input << std::oct << n;
		break;
	}
	input >> data;
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString UString::number ( unsigned long n, int base )
{
	UString data;
	std::stringstream input;
	switch( base )
	{
	case 10:
		input << std::dec << n;
		break;
	case 16:
		input << std::hex << n;
		break;
	case 8:
		input << std::oct << n;
		break;
	}
	input >> data;
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString UString::number ( int n, int base  )
{
	UString data;
	std::stringstream input;
	switch( base )
	{
	case 10:
		input << std::dec << n;
		break;
	case 16:
		input << std::hex << n;
		break;
	case 8:
		input << std::oct << n;
		break;
	}
	input >> data;
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
UString UString::number ( unsigned int n, int base  ) 
{
	UString data;
	std::stringstream input;
	switch( base )
	{
	case 10:
		input << std::dec << n;
		break;
	case 16:
		input << std::hex << n;
		break;
	case 8:
		input << std::oct << n;
		break;
	}
	input >> data;
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString UString::lower() const
{
	UString sReturn = *this; 
	std::transform(sReturn.begin(), sReturn.end(), sReturn.begin(), ::tolower);
	return ( sReturn );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString UString::upper() const
{
	UString sReturn = *this;

	std::transform(sReturn.begin(), sReturn.end(), sReturn.begin(), ::toupper);
	return ( sReturn );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
float UString::toFloat(bool * ok) const
{
	float data ;
	std::stringstream input ;
	input << *this ;
	input >> data ;
	if( ok != NULL )
		(*ok) = input.good() ;
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
double UString::toDouble(bool * ok)  const  
{
	double data ;
	std::stringstream input ;
	input << *this ;
	input >> data ;
	if( ok != NULL )
		(*ok) = input.good() ;
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
char UString::toByte( bool *ok, int base ) const
{
	return static_cast< char >( toShort( ok, base ) );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned char UString::toUByte( bool *ok, int base ) const
{
	return static_cast< unsigned char >( toUShort( ok, base ) );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
short UString::toShort ( bool * ok , int base  ) const 
{
	short data ;
	std::stringstream input ;
	switch ( base )
	{
		case -1:
			size_t offset ;
			if( (offset = (*this).find("0X")) != std::string::npos )
			{
				input << std::hex <<(*this).substr(offset+2) ;
			}
			else if( (offset = (*this).find("0x")) != std::string::npos )
			{
				input << std::hex <<(*this).substr(offset+2) ;
			}
			else
				input << std::dec <<(*this)	;
			break ;
		case 10:
			input <<std::dec << (*this) ;
			break ;
		case 16:
			input << std::hex << (*this) ;
			break ;
		case 8:
			input << std::oct << (*this) ;
			break ;
	}
	input >> data ;
	if( ok != NULL )
		(*ok) = input.good() ;

	return ( data );

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned short UString::toUShort ( bool * ok , int base  ) const
{
	unsigned short data ;
	std::stringstream input ;
	switch ( base )
	{
		case -1:
			size_t offset ;
			if( (offset = (*this).find("0X")) != std::string::npos )
			{
				input << std::hex <<(*this).substr(offset+2) ;
			}
			else if( (offset = (*this).find("0x")) != std::string::npos )
			{
				input << std::hex <<(*this).substr(offset+2) ;
			}
			else
				input << std::dec <<(*this)	;
			break ;
		case 10:
			input <<std::dec << (*this) ;
			break ;
		case 16:
			input << std::hex << (*this) ;
			break ;
		case 8:
			input << std::oct << (*this) ;
			break ;
	}
	input >> data ;
	if( ok != NULL )
		(*ok) = input.good() ;


	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int UString::toInt ( bool * ok , int base  ) const
{
	int data ;
	std::stringstream input ;
	switch ( base )
	{
		case -1:
			size_t offset ;
			if( (offset = (*this).find("0X")) != std::string::npos )
			{
				input << std::hex <<(*this).substr(offset+2) ;
			}
			else if( (offset = (*this).find("0x")) != std::string::npos )
			{
				input << std::hex <<(*this).substr(offset+2) ;
			}
			else
				input << std::dec <<(*this)	;
			break ;
		case 10:
			input <<std::dec << (*this) ;
			break ;
		case 16:
			input << std::hex << (*this) ;
			break ;
		case 8:
			input << std::oct << (*this) ;
			break ;
	}
	input >> data ;
	if( ok != NULL )
		(*ok) = input.good() ;

	return ( data );

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned int UString::toUInt ( bool * ok , int base ) const
{
	unsigned int data ;
	std::stringstream input ;
	switch ( base )
	{
		case -1:
			size_t offset ;
			if( (offset = (*this).find("0X")) != std::string::npos )
			{
				input << std::hex <<(*this).substr(offset+2) ;
			}
			else if( (offset = (*this).find("0x")) != std::string::npos )
			{
				input << std::hex <<(*this).substr(offset+2) ;
			}
			else
				input << std::dec <<(*this)	;
			break ;
		case 10:
			input <<std::dec << (*this) ;
			break ;
		case 16:
			input << std::hex << (*this) ;
			break ;
		case 8:
			input << std::oct << (*this) ;
			break ;
	}
	input >> data ;
	if( ok != NULL )
		(*ok) = input.good() ;


	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
long UString::toLong ( bool * ok , int base  ) const
{
	long data ;
	std::stringstream input ;
	switch ( base )
	{
		case -1:
			size_t offset ;
			if( (offset = (*this).find("0X")) != std::string::npos )
			{
				input << std::hex <<(*this).substr(offset+2) ;
			}
			else if( (offset = (*this).find("0x")) != std::string::npos )
			{
				input << std::hex <<(*this).substr(offset+2) ;
			}
			else
				input << std::dec <<(*this)	;
			break ;
		case 10:
			input <<std::dec << (*this) ;
			break ;
		case 16:
			input << std::hex << (*this) ;
			break ;
		case 8:
			input << std::oct << (*this) ;
			break ;
	}
	input >> data ;
	if( ok != NULL )
		(*ok) = input.good() ;

	return ( data );

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
unsigned long UString::toULong ( bool * ok , int base  ) const
{
	unsigned long data ;
	std::stringstream input ;
	switch ( base )
	{
		case -1:
			size_t offset ;
			if( (offset = (*this).find("0X")) != std::string::npos )
			{
				input << std::hex <<(*this).substr(offset+2) ;
			}
			else if( (offset = (*this).find("0x")) != std::string::npos )
			{
				input << std::hex <<(*this).substr(offset+2) ;
			}
			else
				input << std::dec <<(*this)	;
			break ;
		case 10:
			input <<std::dec << (*this) ;
			break ;
		case 16:
			input << std::hex << (*this) ;
			break ;
		case 8:
			input << std::oct << (*this) ;
			break ;
	}
	input >> data ;
	if( ok != NULL )
		(*ok) = input.good() ;

	return ( data );

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString UString::comment() const
{
	UString data ;
	std::string sep = COMMENTTAG ;
	size_t offset = find(sep) ;
	if( offset != std::string::npos )
	{
		offset = offset + sep.length() ;
		data = (*this).substr(offset) ;
	}
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString UString::removeComment() const
{
	UString data = *this;
	std::string sep = COMMENTTAG ;
	size_t offset = find(sep) ;
	if( offset != std::string::npos )
	{
		data = (*this).substr(0,offset) ;
	}
	return ( data );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
std::string& UString::replaceSlash( std::string& data )
{
	std::string value = "\\" ;
	std::string::size_type index = 0 ;

	while ( (index = data.find( value, index ) ) != std::string::npos )
	{
		data = data.replace(index,1,"/") ;
	}
	return ( data );

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString UString::fixDirectory() 
{
	UString temp = *this;
	temp = temp.stripWhiteSpace() ;
	temp = replaceSlash(temp) ;
	if( temp.find_last_of("/") != temp.size()-1 )
	{
		temp = temp+"/" ;
	}
	return (temp );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString UString::operator+(const char * input ) 
{
	UString temp(input) ;
	UString base = *this ;
//	return base + temp ;
	return (stdstring)base + (stdstring)temp;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UString UString::sprintf(const char* format,...)
{
	char buffer[2048] ;	// yes, we are limiting to 2k buffers 
	memset(buffer,0,2048) ;

	UOXVALIST marker;
	va_start( marker, format ) ;
#if UOX_PLATFORM == PLATFORM_WIN32
	_vsnprintf( buffer, 2048, format, marker );
#else
	vsnprintf( buffer, 2048, format, marker );
#endif
	va_end( marker );			   // Reset variable arguments.  
	UString status = buffer ;


	return ( status );
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int UString::sectionCount(std::string sep) 
{
	int count = -1 ;

	UString temp =(*this).simplifyWhiteSpace()  ;

	size_t index = temp.find(sep) ;
	while ( index != std::string::npos )
	{
		count++ ;
		index = temp.find(sep,index+1) ;
	}

	count++ ;
	return ( count );
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int UString::sectionCount(const char* sep)
{
	return ( sectionCount(std::string(sep)) );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool UString::operator==( const char *input )
{
	bool rvalue = false;
	std::string temp( input );
	if( temp == *this )
	{
		rvalue = true;
	}
	return rvalue;
}

}
