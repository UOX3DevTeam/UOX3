#include "uox3.h"
#include "cStringTools.h"

cStringTools::cStringTools()
{
}

cStringTools::~cStringTools()
{
}

//o--------------------------------------------------------------------------o
//|	Function		-	R64 cStringTools::makeR64( std::string sValue ) 
//|	Date			-	4/02/2003
//|	Developers		-	EviLDeD / Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handy conversion member to quickly convert from a string to a double
//o--------------------------------------------------------------------------o
R64 cStringTools::makeR64( std::string sValue ) 
{
	R64 lnDouble = 0;
	std::istringstream ssHold(sValue.c_str());
	ssHold >> lnDouble;
	return lnDouble;
}
R64 cStringTools::makeR64( const char *lpszValue )
{
	// Make sure that there is a valid value sent in
	if(!lpszValue)
		return 0;
	// Convert to string
	std::string sTemp( lpszValue );
	// Call converstion member function and return response.
	return makeR64(sTemp);
}

//o--------------------------------------------------------------------------o
//|	Function		-	R64 cStringTools::makeR64( std::string sValue ) 
//|	Date			-	4/02/2003
//|	Developers		-	EviLDeD / Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handy conversion member to quickly convert from a string to a float
//o--------------------------------------------------------------------------o
R32 cStringTools::makeR32( std::string sValue )
{
	R32 lnFloat = 0;
	std::istringstream ssHold(sValue.c_str());
	ssHold >> lnFloat;
	return lnFloat;
}
R32 cStringTools::makeR32( const char *lpszValue )
{
	// Make sure that there is a valid value sent in
	if(!lpszValue)
		return 0;
	// Convert to string
	std::string sTemp( lpszValue );
	// Call converstion member function and return response.
	return makeR32(sTemp);
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI32 cStringTools::makeUI32( std::string sValue )
//|	Date			-	4/02/2003
//|	Developers		-	EviLDeD / Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handy conversion member to quickly convert from a string to a UI32
//o--------------------------------------------------------------------------o
UI32 cStringTools::makeUI32( std::string sValue )
{
	// Ok we need to run a conversion from hex to dec for storage.
	if(!sValue.length())
	{
		return 0;
	}
	// Declare variables
	UI32 lnLong = 0;
	if(!strnicmp(sValue.c_str(),"0X",sizeof(char)*2)||!strnicmp(sValue.c_str(),"X",sizeof(char)*1))
	{
		// Hexidecimal value passed in. Formate either 0x, or x (Is not case sensitive)
		std::istringstream ssHold(sValue.c_str()+2);
		ssHold >> std::hex >> lnLong >> std::dec;
	}
	else if(!strnicmp(sValue.c_str(),"0",sizeof(char)*1))
	{
		// Octal value was passed in. Not sure why we would ever do this, but for those willing. ;)
		std::istringstream ssHold(sValue.c_str());
		ssHold >> std::oct >> lnLong >> std::dec;
	}
	else
	{
		// For some reason the value passed in to be converted was a decimal number.
		std::istringstream ssHold(sValue.c_str());
		ssHold >> std::dec >> lnLong;
	}
	return lnLong;
}
UI32 cStringTools::makeUI32( const char *lpszValue )
{
	// Make sure that there is a valid value sent in
	if(!lpszValue)
		return 0;
	// Convert to string
	std::string sTemp( lpszValue );
	// Call converstion member function and return response.
	return makeUI32(sTemp);
}

//o--------------------------------------------------------------------------o
//|	Function		-	SI32 cStringTools::makeSI32( std::string sValue )
//|	Date			-	4/02/2003
//|	Developers		-	EviLDeD / Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handy conversion member to quickly convert from a string to a SI32
//o--------------------------------------------------------------------------o
SI32 cStringTools::makeSI32( std::string sValue )
{
	// Ok we need to run a conversion from hex to dec for storage.
	if(!sValue.length())
	{
		return 0;
	}
	// Declare variables
	SI32 lnLong = 0;
	if(!strnicmp(sValue.c_str(),"0X",sizeof(char)*2)||!strnicmp(sValue.c_str(),"X",sizeof(char)*1))
	{
		// Hexidecimal value passed in. Formate either 0x, or x (Is not case sensitive)
		std::istringstream ssHold(sValue.c_str()+2);
		ssHold >> std::hex >> lnLong >> std::dec;
	}
	else if(!strnicmp(sValue.c_str(),"0",sizeof(char)*1))
	{
		// Octal value was passed in. Not sure why we would ever do this, but for those willing. ;)
		std::istringstream ssHold(sValue.c_str());
		ssHold >> std::oct >> lnLong >> std::dec;
	}
	else
	{
		// For some reason the value passed in to be converted was a decimal number.
		std::istringstream ssHold(sValue.c_str());
		ssHold >> std::dec >> lnLong;
	}
	return lnLong;
}
SI32 cStringTools::makeSI32( const char *lpszValue )
{
	// Make sure that there is a valid value sent in
	if(!lpszValue)
		return 0;
	// Convert to string
	std::string sTemp( lpszValue );
	// Call converstion member function and return response.
	return makeSI32(sTemp);
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI16 cStringTools::makeUI16( std::string sValue )
//|	Date			-	4/02/2003
//|	Developers		-	EviLDeD / Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handy conversion member to quickly convert from a string to a UI16
//o--------------------------------------------------------------------------o
UI16 cStringTools::makeUI16( std::string sValue )
{
	// Ok we need to run a conversion from hex to dec for storage.
	if(!sValue.length())
	{
		return 0;
	}
	// Declare variables
	UI16 lnShort = 0;
	if(!strnicmp(sValue.c_str(),"0X",sizeof(char)*2)||!strnicmp(sValue.c_str(),"X",sizeof(char)*1))
	{
		// Hexidecimal value passed in. Formate either 0x, or x (Is not case sensitive)
		std::istringstream ssHold(sValue.c_str()+2);
		ssHold >> std::hex >> lnShort >> std::dec;
	}
	else if(!strnicmp(sValue.c_str(),"0",sizeof(char)*1))
	{
		// Octal value was passed in. Not sure why we would ever do this, but for those willing. ;)
		std::istringstream ssHold(sValue.c_str());
		ssHold >> std::oct >> lnShort >> std::dec;
	}
	else
	{
		// For some reason the value passed in to be converted was a decimal number.
		std::istringstream ssHold(sValue.c_str());
		ssHold >> std::dec >> lnShort;
	}
	return lnShort;
}
UI16 cStringTools::makeUI16( const char *lpszValue )
{
	// Make sure that there is a valid value sent in
	if(!lpszValue)
		return 0;
	// Convert to string
	std::string sTemp( lpszValue );
	// Call converstion member function and return response.
	return makeUI16(sTemp);
}

//o--------------------------------------------------------------------------o
//|	Function		-	SI16 cStringTools::makeSI16( std::string sValue )
//|	Date			-	4/02/2003
//|	Developers		-	EviLDeD / Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handy conversion member to quickly convert from a string to SI16
//o--------------------------------------------------------------------------o
SI16 cStringTools::makeSI16( std::string sValue )
{
	// Ok we need to run a conversion from hex to dec for storage.
	if(!sValue.length())
	{
		return 0;
	}
	// Declare variables
	SI16 lnShort = 0;
	if(!strnicmp(sValue.c_str(),"0X",sizeof(char)*2)||!strnicmp(sValue.c_str(),"X",sizeof(char)*1))
	{
		// Hexidecimal value passed in. Formate either 0x, or x (Is not case sensitive)
		std::istringstream ssHold(sValue.c_str()+2);
		ssHold >> std::hex >> lnShort >> std::dec;
	}
	else if(!strnicmp(sValue.c_str(),"0",sizeof(char)*1))
	{
		// Octal value was passed in. Not sure why we would ever do this, but for those willing. ;)
		std::istringstream ssHold(sValue.c_str());
		ssHold >> std::oct >> lnShort >> std::dec;
	}
	else
	{
		// For some reason the value passed in to be converted was a decimal number.
		std::istringstream ssHold(sValue.c_str());
		ssHold >> std::dec >> lnShort;
	}
	return lnShort;
}
SI16 cStringTools::makeSI16( const char *lpszValue )
{
	// Make sure that there is a valid value sent in
	if(!lpszValue)
		return 0;
	// Convert to string
	std::string sTemp( lpszValue );
	// Call converstion member function and return response.
	return makeSI16(sTemp);
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI08 cStringTools::makeUI08( std::string sValue )
//|	Date			-	4/02/2003
//|	Developers		-	EviLDeD / Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handy conversion member to quickly convert from a string to a UI08
//o--------------------------------------------------------------------------o
//| Modifications	-	char's are special cases as you cannot convert a string to
//|						them directly, thus we simply typecast our short conversion func
//o--------------------------------------------------------------------------o
UI08 cStringTools::makeUI08( std::string sValue )
{
	return static_cast<UI08>(makeUI16( sValue ));
}
UI08 cStringTools::makeUI08( const char *lpszValue )
{
	// Make sure that there is a valid value sent in
	if(!lpszValue)
		return 0;
	// Convert to string
	std::string sTemp( lpszValue );
	// Call converstion member function and return response.
	return makeUI08(sTemp);
}

//o--------------------------------------------------------------------------o
//|	Function		-	SI08 cStringTools::makeSI08( std::string sValue )
//|	Date			-	4/02/2003
//|	Developers		-	EviLDeD / Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handy conversion member to quickly convert from a string to a SI08
//o--------------------------------------------------------------------------o
//| Modifications	-	char's are special cases as you cannot convert a string to
//|						them directly, thus we simply typecast our short conversion func
//o--------------------------------------------------------------------------o
SI08 cStringTools::makeSI08( std::string sValue )
{
	return static_cast<SI08>(makeSI16( sValue ));
}
SI08 cStringTools::makeSI08( const char *lpszValue )
{
	// Make sure that there is a valid value sent in
	if(!lpszValue)
		return 0;
	// Convert to string
	std::string sTemp( lpszValue );
	// Call converstion member function and return response.
	return makeSI08(sTemp);
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cStringTools::GetToken( const char *data, SI32 num, char *tokenStr )
//|	Date			-	Unknown
//|	Developers		-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Grabs token number "num" from string "data" and sets it to "tokenStr"
//o--------------------------------------------------------------------------o
void cStringTools::GetToken( const char *data, SI32 num, char *tokenStr )
{
	memset( tokenStr, 0, 255 );
	SI32 i = 0;
	while( num != 0 )
	{
		if( data[i] == 0 )
			num--;
		else
		{
			if( data[i] == ' ' && i != 0 && data[i-1] != ' ' )
				num--;
			i++;
		}
	}
	SI32 j = 0;
	while( num != -1 )
	{
		if( data[i] == 0 )
			num--;
		else
		{
			if( data[i] == ' ' && i != 0 && data[i-1] != ' ')
				num--;
			else
			{
				tokenStr[j] = data[i];
				j++;
			}
			i++;
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cStringTools::safeCopy(char *dest, const char *src, UI32 maxLen )
//|	Date			-	Unknown
//|	Developers		-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Safely copy a string that might be longer than the 
//|									destination Will truncate if needed, but will never copy 
//|									over too much to avoid possible crashes.
//o--------------------------------------------------------------------------o
void cStringTools::safeCopy(char *dest, const char *src, UI32 maxLen )
{
	assert( src );
	assert( dest );
	assert( maxLen );
	
	strncpy( dest, src, maxLen );
	dest[maxLen - 1] = '\0';
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cStringTools::makeString( int i, char *string )
//|	Date			-	Unknown
//|	Developers		-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Wrapping of the stdlib num-2-str functions
//o--------------------------------------------------------------------------o
void cStringTools::makeString( int i, char *string )
{
#if !defined(__unix__)
	itoa( i, string, 10 );
#else
	sprintf(string, "%d", i );
#endif
}