//========================================================================
// FILE: mstring.cpp
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
#include "uox3.h"
#include "mstring.h"
//========================================================================
// Begin Constructor
//##ModelId=3C5D92DB01FF
mstring::mstring() : string()
{
}
// End of Constructor
//========================================================================
// Begin Constructor (const string&)
//##ModelId=3C5D92DB0209
mstring::mstring(const string& sStr) : string(static_cast<string>(sStr))
{ 
}
// End of Constructor (const string&)
//========================================================================
// Constructor that is populated from the null terminated char string
//##ModelId=3C5D92DB0231
mstring::mstring(char* szStr) : string(szStr)
{
}
// End of Constructor
//========================================================================
// Begin Constructor (char*, UI32)
//##ModelId=3C5D92DB0245
mstring::mstring(char* szStr, UI32 uiLen) : string(szStr, uiLen)
{
	// Creates a string  using the char string and the number of characters specified
}
// End of Constructor (char *, UI32)
//========================================================================
// Begin Constructor (const string&, UI32, UI32)
//##ModelId=3C5D92DB0213
mstring::mstring(const string& clString, UI32 uiIdx, UI32 uiLength)  : string(static_cast<string>(clString), uiIdx, uiLength)
{
}
// End of Constructor (const string&, UI32, UI32)
//========================================================================
// Begin Destructor
//##ModelId=3C5D92DB025A
mstring::~mstring()
{
}
// End of Destrcutor
//========================================================================
// Begin appendNum (UI08,UI16)
//##ModelId=3C5D92DB03A3
mstring& mstring::appendNum(UI08 uiValue, UI16 uiBase) 
{
	// Convert a byte	
	return appendNum(static_cast<UI32>(uiValue) , uiBase);
}
// End of appendNum (UI08,UI16)
//========================================================================
// Begin appendNum (UI16, UI16)
//##ModelId=3C5D92DB03C1
mstring& mstring::appendNum(UI16 uiValue, UI16 uiBase)
{
	// Convert a 16 bit integer
	return appendNum(static_cast<UI32>(uiValue), uiBase);
}
// End of appendNum (UI16,UI16)
//========================================================================
// Begin appendNum(UI32,UI16)
//##ModelId=3C5D92DB03DF
mstring& mstring::appendNum(UI32 uiValue, UI16 uiBase)
{
	// Convert a 32 bit integer
	// Basically we make a stream, pop the value onto it, and pop it back off
	// We have to use the older strstream as gcc doesn't support the new ones
	// Ok, gcc 3 does use sstream, so we will conver it over
	
	stringstream   strmQueue;
	mstring sTemp;
	
	// I belive this could be done cleaner with STL algorithms!!!!
	// Put it into the stream
	strmQueue <<(uiValue);
	
	// Ok, now read it back from the stream into our string
	switch (uiBase)
	{
			// Decimal case
		case 10:
			strmQueue >> dec>> sTemp;
			append(sTemp);
			break;
			// Hex case	
		case 16:
			strmQueue >> hex >> sTemp;
			append(sTemp);
			break;
			// Octal case
		case 8:
			strmQueue >> oct >> sTemp;
			append(sTemp);
			break;
			// We dont support anything else
		default:
			break;
	}
	// Return our object
	(*this) = (*this) + sTemp;
	return *this;
}
// End of appendNum (UI32,UI16)
//========================================================================
// Begin appendNum(SI08,UI16)
//##ModelId=3C5D92DC000D
mstring & mstring::appendNum(SI08 siValue, UI16 uiBase) 
{
	// Set a signed eight bit value
	return appendNum(static_cast<SI32>(siValue) , uiBase);
}
// End of setNum (SI08,UI16)
//========================================================================
// Begin appendNum (SI16, UI16)
//##ModelId=3C5D92DC005C
mstring & mstring::appendNum(SI16 siValue, UI16 uiBase) 
{
	// Set a signed 16 bit value
	return appendNum(static_cast<SI32>(siValue), uiBase);
}
// End of appendNum (SI16, UI16)
//========================================================================
// Begi appendNum (SI32,UI16)
//##ModelId=3C5D92DC007A
mstring & mstring::appendNum(SI32 siValue, UI16 uiBase)
{
	// Set a signed 32 bit value
	// Basically we make a stream, pop the value onto it, and pop it back off
	// We have to use the older strstream as gcc doesn't support the new ones (will with 3.0)
	// Ok, gcc 3 does use sstream, so we will conver it over
	
	stringstream   strmQueue;
	
	
	// Put in into the stream
	strmQueue <<(siValue);
	mstring sTemp;
	
	// Ok, now read it back from the stream into our string
	switch (uiBase)
	{
			// Decimal case
		case 10:
			strmQueue >> dec >> sTemp;
			// >> *this;
			append(sTemp);
			break;
			// Hex case	
		case 16:
			strmQueue >> hex >> sTemp;
			append(sTemp);
			break;
			// Octal case
		case 8:
			strmQueue >> oct >> sTemp;
			append(sTemp);
			break;
			// We dont support anything else
		default:
			break;
	}
	// Return our object
	return *this;
}
// End of appendNum (SI32,UI16)
//========================================================================
// Begin setNum (UI08,UI16)
//##ModelId=3C5D92DB0303
mstring& mstring::setNum(UI08 uiValue, UI16 uiBase) 
{
	// Convert a byte	
	return setNum(static_cast<UI32>(uiValue) , uiBase);
}
// End of setNum (UI08,UI16)
//========================================================================
// Begin setNum (UI16, UI16)
//##ModelId=3C5D92DB0321
mstring& mstring::setNum(UI16 uiValue, UI16 uiBase)
{
	// Convert a 16 bit integer
	return setNum(static_cast<UI32>(uiValue), uiBase);
}
// End of setNum (UI16,UI16)
//========================================================================
// Begin setNum(UI32,UI16)
//##ModelId=3C5D92DB0336
mstring& mstring::setNum(UI32 uiValue, UI16 uiBase)
{
	// Convert a 32 bit integer
	// Basically we make a stream, pop the value onto it, and pop it back off
	// We have to use the older strstream as gcc doesn't support the new ones
	// Ok, gcc 3 does use sstream, so we will conver it over
	stringstream   strmQueue;
	
	// Put it into the stream
	strmQueue <<(uiValue);
	
	// Ok, now read it back from the stream into our string
	switch (uiBase)
	{
			// Decimal case
		case 10:
			strmQueue >> dec>> *this;
			break;
			// Hex case	
		case 16:
			strmQueue >> hex >> *this;
			break;
			// Octal case
		case 8:
			strmQueue >> oct >> *this;
			break;
			// We dont support anything else
		default:
			break;
	}
	// Return our object
	return *this;
}
// End of setNum (UI32,UI16)
//========================================================================
// Begin setNum(SI08,UI16)
//##ModelId=3C5D92DB0354
mstring & mstring::setNum(SI08 siValue, UI16 uiBase) 
{
	// Set a signed eight bit value
	return setNum(static_cast<SI32>(siValue) , uiBase);
}
// End of setNum (SI08,UI16)
//========================================================================
// Begin setNum (SI16, UI16)
//##ModelId=3C5D92DB0371
mstring & mstring::setNum(SI16 siValue, UI16 uiBase) 
{
	// Set a signed 16 bit value
	return setNum(static_cast<SI32>(siValue), uiBase);
}
// End of setNum (SI16, UI16)
//========================================================================
// Begi setNum (SI32,UI16)
//##ModelId=3C5D92DB0385
mstring & mstring::setNum(SI32 siValue, UI16 uiBase)
{
	// Set a signed 32 bit value
	// Basically we make a stream, pop the value onto it, and pop it back off
	// We have to use the older strstream as gcc doesn't support the new ones (will with 3.0)
	// Ok, gcc 3 does use sstream, so we will conver it over
	stringstream   strmQueue;
	
	// Put in into the stream
	strmQueue <<(siValue);
	
	// Ok, now read it back from the stream into our string
	switch (uiBase)
	{
			// Decimal case
		case 10:
			strmQueue >> dec >> *this;
			
			break;
			// Hex case	
		case 16:
			strmQueue >> hex >> *this;
			break;
			// Octal case
		case 8:
			strmQueue >> oct >> *this;
			break;
			// We dont support anything else
		default:
			break;
	}
	// Return our object
	return *this;
}
// End of setNum (SI32,UI16)
//========================================================================
// Begin toUI32
//##ModelId=3C5D92DC0098
UI32 mstring::toUI32(UI16 uiBase)
{
	// Make a stream for our conversion
	stringstream strmQueue;
	// Our return value
	UI32 uiReturn =0;
	// Take our value and put it into the stream
	strmQueue <<(*this);
	// Pull it out
	switch (uiBase)
	{
			// Decimal case
		case 10:
			strmQueue >>dec>> uiReturn;
			break;
			// Hex case
		case 16:
			strmQueue >> hex>> uiReturn;
			break;
			// Octal case
		case 8:
			strmQueue >> oct >> uiReturn;
			break;
			// We dont support anything else
		default:
			break;
	}
	
	// Convert the value in the a 32 bit unsigned integer
	return uiReturn;
}
// End of toUI32
//========================================================================
// Begin toUI16
//##ModelId=3C5D92DC00B6
UI16 mstring::toUI16(UI16 uiBase)
{
	// Return a 16 bit unsigned
	return static_cast<UI16>(toUI32(uiBase));
}
// End of toUI16
//========================================================================
// Begin toUI08
//##ModelId=3C5D92DC00CA
UI08 mstring::toUI08(UI16 uiBase)
{
	return static_cast<UI08>(toUI32(uiBase));
}
// End of toUI08
//========================================================================
// Begin toSI32
//##ModelId=3C5D92DC00DE
SI32 mstring::toSI32(UI16 uiBase)
{
	// Make a stream for our conversion
	// Ok, gcc 3 does use sstream, so we will conver it over
	stringstream   strmQueue;
	// Our return value
	SI32 siReturn =0;
	// Take our value and put it into the stream
	strmQueue <<(*this);
	// Pull it out
	switch (uiBase)
	{
			// Decimal case
		case 10:
			strmQueue >> dec >> siReturn;
			break;
			// Hex case
		case 16:
			strmQueue >> hex>> siReturn;
			break;
			// Octal case
		case 8:
			strmQueue >> oct >> siReturn;
			break;
			// We dont support anything else
		default:
			break;
	}
	return siReturn;
}
// End of toSI32
//========================================================================
// Begin toSI16
//##ModelId=3C5D92DC011A
SI16 mstring::toSI16(UI16 uiBase)
{
	return static_cast<SI16>(toSI32(uiBase));
}
// End of toSI16
//========================================================================
// Begin toSI08
//##ModelId=3C5D92DC012F
SI08 mstring::toSI08(UI16 uiBase)
{
	return static_cast<SI08>(toSI32(uiBase));
}
// End of toSI08
//========================================================================
// Begin local split
//##ModelId=3C5D92DB0295
vector<mstring> mstring::split(const mstring& sSep, const UI16 uiAmount)
{
	return split(sSep, *this, uiAmount);
}
//========================================================================
// Begin split (const mstring&, const mstring&)
//##ModelId=3C5D92DC01D8
vector<mstring> mstring::split(const mstring & clSep,  const mstring & clString, const UI16 uiAmount)
{
	vector<mstring> vecStrings;
	
	mstring myString;
	myString = clString;
	myString = trim(myString);
	UI32 uiCount =1;
	// Search for the sperator, keep taking out substrings
	UI32 uiStart = 0;
	UI32 uiStop = 0;
	
	if (uiAmount != 0)
		vecStrings.reserve(uiAmount);
	
	while ((uiStop = myString.find(clSep, uiStart)) != string::npos)
	{
		if ((uiCount < uiAmount)  || (uiAmount == 0))
		{
			mstring sTemp;
			if (uiStart != uiStop)
			{
				sTemp = myString.substr(uiStart, uiStop - uiStart);
				sTemp = trim(sTemp);
			}
			vecStrings.push_back(sTemp);
			uiCount++;
			uiStart = uiStop +clSep.size();
			if (uiStart == myString.size())
				break;
		}
		else
			break;
	}
	// ok, we have to push our remaining back on
	if (uiStart != myString.size())
		myString = myString.substr(uiStart);
	else
		myString.erase();
	
	myString = trim(myString);
	vecStrings.push_back(myString);
	if (uiAmount !=0)
	{
		for (UI32 uiPatch = vecStrings.size(); uiPatch < uiAmount; uiPatch++)
		{
			mstring sPatch;
			vecStrings.push_back(sPatch);
		}
	}
	return vecStrings;
}
// End of split (const mstring&, const mstring&)
//========================================================================
// Begin local split
//##ModelId=3C5D92DB02B3
vector<mstring> mstring::split(char* ptrSep, const UI16 uiAmount)
{
	return split(ptrSep, *this, uiAmount);
}
// End of local split
//========================================================================
// Begin split (char *, const mstring)
//##ModelId=3C5D92DC0200
vector<mstring> mstring::split(char * ptrSep,  const mstring clString, const UI16 uiAmount)
{
	mstring clSep(ptrSep);
	return split(clSep, clString, uiAmount);
}
// End of split (char *, const mstring)


//========================================================================
// Begin local split
//##ModelId=3C5D92DB02C7
vector<mstring> mstring::splitAny(const mstring& sSep, const UI16 uiAmount)
{
	return splitAny(sSep, *this, uiAmount);
}
//========================================================================
// Begin split (const mstring&, const mstring&)
//##ModelId=3C5D92DC032D
vector<mstring> mstring::splitAny(const mstring & clSep,  const mstring & clString, const UI16 uiAmount)
{
	vector<mstring> vecStrings;
	
	mstring myString;
	myString = clString;
	myString = trim(myString);
	UI32 uiCount =1;
	// Search for the sperator, keep taking out substrings
	UI32 uiStart = 0;
	UI32 uiStop = 0;
	
	if (uiAmount != 0)
		vecStrings.reserve(uiAmount);
	
	while ((uiStop = myString.find_first_of(clSep, uiStart)) != string::npos)
	{
		if ((uiCount < uiAmount)  || (uiAmount == 0))
		{
			mstring sTemp;
			if (uiStart != uiStop)
			{
				sTemp = myString.substr(uiStart, uiStop);
				sTemp = trim(sTemp);
			}
			vecStrings.push_back(sTemp);
			uiCount++;
			uiStart = myString.find_first_not_of(clSep, uiStop);
			if (uiStart == string::npos)
				break;
		}
		else
			break;
	}
	// ok, we have to push our remaining back on
	if (uiStart != string::npos)
		myString = myString.substr(uiStart);
	else
		myString.erase();
	
	myString = trim(myString);
	vecStrings.push_back(myString);
	if (uiAmount !=0)
	{
		for (UI32 uiPatch = vecStrings.size() -1; uiPatch < uiAmount; uiPatch++)
		{
			mstring sPatch;
			vecStrings.push_back(sPatch);
		}
	}
	return vecStrings;
}
// End of split (const mstring&, const mstring&)
//========================================================================
// Begin local split
//##ModelId=3C5D92DB02E5
vector<mstring> mstring::splitAny(char* ptrSep, const UI16 uiAmount)
{
	return splitAny(ptrSep, *this, uiAmount);
}
// End of local split
//========================================================================
// Begin split (char *, const mstring)
//##ModelId=3C5D92DC0355
vector<mstring> mstring::splitAny(char * ptrSep,  const mstring clString, const UI16 uiAmount)
{
	mstring clSep(ptrSep);
	return splitAny(clSep, clString, uiAmount);
}
// End of split (char *, const mstring)

//========================================================================
// Begin unicode (const char*, UI32)
//##ModelId=3C5D92DB026D
void mstring::unicode(const char* szBuffer, UI32 uiLen)
{
	// right now we throw away the top byte of each value, assume latin8
	char* ptrBuffer;
	
	ptrBuffer = new char[uiLen/2];
	
	// Ok, now just loop through and copy them over.
	for (UI32 uiIndex = 0; uiIndex < uiLen; uiIndex++)
	{
		ptrBuffer[uiIndex] = szBuffer[uiIndex*2];
	}
	
	// We now have the buffer, have to assign to the internal string
	erase();
	
	assign(ptrBuffer, uiLen/2);
	
	delete ptrBuffer;
	ptrBuffer = NULL;
}
// End of unicode (const char *,UI32)
//========================================================================
// Begin unicode
//##ModelId=3C5D92DB0263
char* mstring::unicode()
{
	// We return a unicode character array, just add zero to the top byte
	UI32 uiSize = size();
	char* ptrBuffer;
	
	ptrBuffer = new char[uiSize*2];
	
	for (UI32 uiIndex = 0; uiIndex < uiSize; uiIndex++)
	{
		ptrBuffer[uiIndex*2] = c_str()[uiIndex];
		ptrBuffer[uiIndex*2 + 1] = 0;
	}
	
	return ptrBuffer;
}
// End of unicode
//========================================================================
// local trim
//##ModelId=3C5D92DB028B
void mstring::trim()
{
	trim(*this);
}
//========================================================================
// Begin trim
//##ModelId=3C5D92DC0373
mstring mstring::trim(mstring& clInput)
{
	SI32 siStart = clInput.find_first_not_of(MYWHITESPACE);
	SI32 siStop = clInput.find_last_not_of(MYWHITESPACE);
	if ((siStart != string::npos) && (siStop != string::npos))
	{
		clInput = clInput.substr(siStart, (siStop - siStart) + 1);
	}
	
	return clInput;
}
// End of trim
//========================================================================
// Begin cleanup
//##ModelId=3C5D92DB02FA
void mstring::cleanup()
{
	(*this) = cleanup(*this);
}

//========================================================================
// Begin cleanup
//##ModelId=3C5D92DC039B
mstring mstring::cleanup(mstring clInput)
{
	// First thing is get rid of trailing/leading blanks
	clInput = trim(clInput);
	
	string::size_type siStart = 0;
	string::size_type siStop;
	
	while ((siStart = clInput.find_first_of(MYWHITESPACE, siStart))!= string::npos)
	{
		siStop  = clInput.find_first_not_of(MYWHITESPACE, siStart);
		if (siStop == string::npos)
			siStop = clInput.size() - siStart;
		clInput = clInput.replace(siStart, siStop, " ");
		siStart++;
	}
	
	return clInput;
}
// End of cleanup

//========================================================================
//##ModelId=3C5D92DC01B0
string mstring::pop(char* szSep)
{
	string sSep(szSep);
	return pop(sSep);
}
//========================================================================
//##ModelId=3C5D92DC01BA
string mstring::pop(string sSep)
{
	// find a sperator, return the porting in front, and reset the string
	
	SI32 siFind = 0;
	string sReturn;
	if ((siFind = (*this).find(sSep)) != string::npos)
	{
		// Found it
		sReturn = (*this).substr(0, siFind);
		// reset ourselves
		if ((siFind + 1) < static_cast<SI32>((*this).size()))
		{
			(*this) = (*this).substr(siFind + 1);
		}
		else
			(*this).erase();
	}
	else
		sReturn.erase();
	
	return sReturn;
}
//========================================================================
// Begin lower()
//##ModelId=3C5D92DC0188
string mstring::lower()
{
	return lower((*this));
}

//==========================================================================
// Begin upper()
//##ModelId=3C5D92DC0192
string mstring::upper()
{
	return upper((*this));
}

//=========================================================================
// Begin static lower
//##ModelId=3C5D92DC03AF
string mstring::lower(string& sTemp)
{
	string sReturn; 
	sReturn = sTemp;
	transform(sReturn.begin(), sReturn.end(), sReturn.begin(), ::tolower);
	
	return sReturn;
}

//=========================================================================
// Begin static upper
//##ModelId=3C5D92DC03C3
string mstring::upper(string& sTemp)
{
	string sReturn;
	sReturn = sTemp;
	transform(sReturn.begin(), sReturn.end(), sReturn.begin(), ::toupper);
	return sReturn;
}

//=======================================================================
//##ModelId=3C5D92DC019C
bool mstring::compare(string sTemp)
{
	string sTemp1;
	sTemp1 = (*this);
	return equal(sTemp.begin(), sTemp.end(), sTemp1.begin(), nocase_compare);
}

//=======================================================================
bool nocase_compare(SI08 szA, SI08 szB)
{
	return toupper(szA) == toupper(szB);
}

//========================================================================
//========================================================================
//======================  End of mstring.cpp  ============================
//========================================================================
//========================================================================
