//========================================================================
//FILE: mstring.h
//========================================================================
//	Copyright (c) 2001 by Sheppard Norfleet and Charles Kerr
//  All Rights Reserved
// 
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided the following conditions are met:
//
//	Redistributions of source code must retain the above copyright notice,
//	this list of conditions and the following disclaimer. Redistributions
//	in binary form must reproduce the above copyright notice, this list of
//	conditions and the following disclaimer in the documentation and/or
//	other materials provided with the distribution.
//
//	Neither the name of the SWORDS  nor the names of its contributors may
//	be used to endorse or promote products derived from this software
//	without specific prior written permission. 
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  `AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//	A PARTICULAR PURPOSE ARE DISCLAIMED. .
//========================================================================
//Directives
#ifndef	IN_MSTRING_H
#define IN_MSTRING_H
//========================================================================
//Platform Specs
//========================================================================
//System Files
#include <iosfwd>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
using namespace std;

//========================================================================
// Third party includes

// Forward class declaration
class mstring ;

// SWORDS Includes
#define MYWHITESPACE " \t\v\f\0 "
#define MYNONWHITESPACE "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-=`~!@#$%^&*()_+|,<.>/?;:'[{]}\"\\"

//#include "stream.h" 
///CLASSDEF:



//##ModelId=3C5D92DB01D7
class mstring : public string
{
public:
//	Constructors
	/// Blank Constructor
	//##ModelId=3C5D92DB01FF
	mstring() ;
	/// Constructor that is a copy of the existing string
	//##ModelId=3C5D92DB0209
	mstring(const string&) ;
	
	// Constructor that initialise based on size (broken on msvc6)
	//mstring(const string& clString, UI32 uiIdx) ;
	//##ModelId=3C5D92DB0213
	mstring(const string& clString, UI32 uiIdx, UI32 uiLength) ;
	///Constructor that is populated from the null terminated char string
	//##ModelId=3C5D92DB0231
	mstring(char*);
	///Creates a string  using the char string and the number of characters specified
	//##ModelId=3C5D92DB0245
	mstring(char*,UI32);
	///Destructor
	//##ModelId=3C5D92DB025A
	~mstring();
	///	Retrieve the unicode version
	//	this is mainly for future growth.  Since internally we dont keep it in unicode.
	//	We could and should, and hopefully this class will be expanded to do that (or the string class 
	//	will grow to do it for us.  Anyway, just convert the character representation to high byte
	//	of 0.

	//##ModelId=3C5D92DB0263
	char* unicode();
	
	///The reverse, take in a unicode character array
	//##ModelId=3C5D92DB026D
	void unicode(const char* szBuffer, UI32 uiLen) ;
	
	// formatting methods
	
	//##ModelId=3C5D92DB028B
	void	trim() ;
	//##ModelId=3C5D92DB0295
	vector<mstring> split(const mstring & clSep, const UI16 uiAmount=0) ;
	//##ModelId=3C5D92DB02B3
	vector<mstring> split( char* szSep,const UI16 uiAmount=0) ;
	//##ModelId=3C5D92DB02C7
	vector<mstring> splitAny(const mstring & clSep,const UI16 uiAmount=0) ;
	//##ModelId=3C5D92DB02E5
	vector<mstring> splitAny( char* szSep,const UI16 uiAmount=0) ;
	//##ModelId=3C5D92DB02FA
	void  cleanup();

	//Methods to set binary numbers to a string
	///  Convert a byte	
	//##ModelId=3C5D92DB0303
	mstring & setNum(UI08 uiValue, UI16 uiBase=10) ;
	/// Convert a 16 bit integer
	//##ModelId=3C5D92DB0321
	mstring & setNum(UI16 uiValue, UI16 uiBase=10) ;
	/// Convert a 32 bit integer
	//##ModelId=3C5D92DB0336
	mstring & setNum(UI32 uiValue, UI16 uiBase=10) ;
	//##ModelId=3C5D92DB0354
	mstring & setNum(SI08 uiValue, UI16 uiBase=10) ;
	//##ModelId=3C5D92DB0371
	mstring & setNum(SI16 uiValue, UI16 uiBase=10) ;
	//##ModelId=3C5D92DB0385
	mstring & setNum(SI32 uiValue, UI16 uiBase=10) ;	
	//##ModelId=3C5D92DB03A3
	mstring & appendNum(UI08 uiValue, UI16 uiBase=10) ;
	/// Convert a 16 bit integer
	//##ModelId=3C5D92DB03C1
	mstring & appendNum(UI16 uiValue, UI16 uiBase=10) ;
	/// Convert a 32 bit integer
	//##ModelId=3C5D92DB03DF
	mstring & appendNum(UI32 uiValue, UI16 uiBase=10) ;
	//##ModelId=3C5D92DC000D
	mstring & appendNum(SI08 uiValue, UI16 uiBase=10) ;
	//##ModelId=3C5D92DC005C
	mstring & appendNum(SI16 uiValue, UI16 uiBase=10) ;
	//##ModelId=3C5D92DC007A
	mstring & appendNum(SI32 uiValue, UI16 uiBase=10) ;	
	//Methods to convert the string value to binary number	
	//##ModelId=3C5D92DC0098
	UI32 toUI32(UI16 uiBase=10);
	//##ModelId=3C5D92DC00B6
	UI16 toUI16(UI16 uiBase=10);
	//##ModelId=3C5D92DC00CA
	UI08 toUI08(UI16 uiBase=10);
	
	//##ModelId=3C5D92DC00DE
	SI32 toSI32(UI16 uiBase=10);
	//##ModelId=3C5D92DC011A
	SI16 toSI16(UI16 uiBase=10);
	//##ModelId=3C5D92DC012F
	SI08 toSI08(UI16 uiBase=10);

	//##ModelId=3C5D92DC0188
	string lower();

	//##ModelId=3C5D92DC0192
	string upper();

	//##ModelId=3C5D92DC019C
	bool  compare(string);

	//##ModelId=3C5D92DC01B0
	string pop(char*  szSep);
	//##ModelId=3C5D92DC01BA
	string pop(string sSep);

	//Static members
	//##ModelId=3C5D92DC01D8
	static vector<mstring> split(const mstring & clSep, const mstring & clString,const UI16 uiAmount=0) ;
	//##ModelId=3C5D92DC0200
	static vector<mstring> split( char* szSep, const mstring clString,const UI16 uiAmount=0) ;
	//##ModelId=3C5D92DC032D
	static vector<mstring> splitAny(const mstring & clSep, const mstring & clString,const UI16 uiAmount=0) ;
	//##ModelId=3C5D92DC0355
	static vector<mstring> splitAny( char* szSep, const mstring clString,const UI16 uiAmount=0) ;

	//##ModelId=3C5D92DC0373
	static mstring  trim(mstring&);
	
	//##ModelId=3C5D92DC039B
	static mstring  cleanup(mstring);

	//##ModelId=3C5D92DC03AF
	static string	lower(string&) ;
	//##ModelId=3C5D92DC03C3
	static string   upper(string&) ;


};
	
bool nocase_compare(SI08 szA, SI08 szB) ;
//========================================================================
//========================================================================
//=========================  End of mstring.h  ===========================
//========================================================================
//========================================================================
#endif

