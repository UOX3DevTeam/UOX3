/* Copyright © 2000 
Michael Gradman and Corwin Joy 

Permission to use, copy, modify, distribute and sell this software and 
its documentation for any purpose is hereby granted without fee, provided 
that the above copyright notice appears in all copies and that both that 
copyright notice and this permission notice appear in supporting documentation. 
Corwin Joy and Michael Gradman make no representations about the suitability 
of this software for any purpose. 
It is provided "as is" without express or implied warranty.
*/ 
// misc. tstring utilities for our DB abstractions
// Initial: 9/7/2000 - MG
// Edited: 12/19/2000 - MG - added namespaces

#ifndef DTL_STRING_UTIL_H
#define DTL_STRING_UTIL_H

#include "dtl_config.h"
#include "DBConnection.h"

#include "std_warn_off.h"
#include "clib_fwd.h"
#include <string>
#include <vector>
#include <functional>

#include <set>
#include "std_warn_on.h"
BEGIN_DTL_NAMESPACE

// case insensitive comparison for tstring's
class NonCaseSensitiveEq : public STD_::binary_function<tstring, tstring, bool>
{
public:
	bool operator()(const tstring &str1, const tstring &str2) const
	{
       return (std_tstricmp(str1.c_str(), str2.c_str()) == 0);
	}
};

class NonCaseSensitiveLt: public STD_::binary_function<tstring, tstring, bool>
{
public:
	bool operator()(const tstring &str1, const tstring &str2) const
	{
       return (std_tstricmp(str1.c_str(), str2.c_str()) < 0);
	}
};

// construct a delimited list out of the strings in the container, each
// having the suffix tacked on to it
// (should also work for TCHAR *'s)

// Disabled since it mysteriously crashes Borland 6 -- and we don't
#if 1
template <typename StrContainer> void
	WriteDelimitedList(const StrContainer &container,
	tstring prefix,tstring suffix,
	tstring delimiter, tstring &delimitedList)
{
	// add enough space to minimize reallocations
	delimitedList.reserve(delimitedList.capacity() + 50 * delimitedList.size());

	for (typename StrContainer::const_iterator str_it = container.begin();
			str_it != container.end(); str_it++)
	{
		if (str_it != container.begin())
		{
			// only add delimiter if this isn't the first item
		    delimitedList += delimiter;
		}
	
		// tack item on to end of list along with the suffix
		delimitedList += prefix;
		delimitedList += *str_it;
		delimitedList += suffix;
	}
}
#endif

#if 0
// specialize for sets
void
	WriteDelimitedList(const STD_::set<tstring> &container,
	tstring prefix,tstring suffix,
	tstring delimiter, tstring &delimitedList);

// specialize for vector
void
	WriteDelimitedList(const STD_::vector<tstring> &container,
	tstring prefix,tstring suffix,
	tstring delimiter, tstring &delimitedList);

#endif

// construct a delimited list out of the strings in the container, each
// having the suffix tacked on to it
// (should also work for TCHAR *'s)
template <typename StrContainer> tstring
	RawMakeDelimitedList(const StrContainer &container,
	tstring prefix, tstring suffix,
	tstring delimiter)
{
	tstring delimitedList;
	delimitedList.reserve(128); // reserve size of 128 for speed in concatenation
        WriteDelimitedList(container, prefix, suffix, delimiter, delimitedList);

	return delimitedList;
}

template <typename StrContainer> tstring
	MakeDelimitedList(const StrContainer &container,
	tstring prefix, tstring suffix,
	tstring delimiter)
{
	return RawMakeDelimitedList(container, prefix, suffix, delimiter);
}

template <typename StrContainer> tstring
	MakeDelimitedList(const StrContainer &container, 
	tstring suffix,
	tstring delimiter)
{
	return RawMakeDelimitedList(container, _TEXT(""), suffix, delimiter);
}

template <typename StrContainer> tstring
MakeDelimitedList(const StrContainer &container, tstring suffix)
{
	return RawMakeDelimitedList(container, _TEXT(""), suffix, _TEXT(", "));
}

template <typename StrContainer> tstring
	MakeDelimitedList(const StrContainer &container)
{
	return RawMakeDelimitedList(container, _TEXT(""), _TEXT(""), _TEXT(", "));
}




template <typename StrContainer> void
	WriteDelimitedList(const StrContainer &container, tstring &out)
{
	WriteDelimitedList(container, _TEXT(""), _TEXT(""), _TEXT(", "), out);
}

template <typename StrContainer> void
WriteDelimitedList(const StrContainer &container, tstring suffix, tstring &out)
{
	WriteDelimitedList(container, _TEXT(""), suffix, _TEXT(", "), out);
}



// finds the number of occurrences of the character in the tstring
unsigned int numOfOccurrences(TCHAR c, tstring inString);

// parse a comma-separated list of strings, packaging the result in a STD_::vector
// of the strings
STD_::vector<tstring> ParseCommaDelimitedList(tstring commaList);

// Use values from isspace in the "C" locale (see isspace(3)):
extern const TCHAR* spaces; // = _TEXT(" \t\r\n\v\f");

inline
void ltrim (tstring& s) { s.erase (0, s.find_first_not_of (spaces)); }
// Front of string is adjusted to non-space, hence TCHAR*&
inline
void ltrim (TCHAR*& s) { while (std_tisspace (*s)) ++s; }
inline
void rtrim (tstring& s) { s.erase (s.find_last_not_of (spaces) + 1); }
// Only end of string is adjusted to non-space, hence TCHAR*
void rtrim (TCHAR* s); // to complex for inline
inline
void trim (tstring& s) { ltrim (s); rtrim (s); }
inline
void trim (TCHAR*& s) { ltrim (s); rtrim (s); }


// returns str with all spaces removed
tstring EliminateSpaces(const tstring &str);

#ifndef _MSC_VER
int stricmp(const TCHAR *s1, const TCHAR *s2);
int strnicmp(const TCHAR *s1, const TCHAR *s2, size_t len );
#endif

// transforms a SQL Query to lowercase, with special care for quotes
tstring QueryToLowercase(const tstring &Query, const tstring &QuoteChar);

// transforms a SQL Query to uppercase, with special care for quotes
tstring QueryToUppercase(const tstring &Query, const tstring &QuoteChar);

// caseifies a SQL Query based on the dtl_config flags
tstring QueryCaseify(const tstring &Query, 
					 const DBConnection &dbc = DBConnection::GetDefaultConnection());

template<class Iter> void lower_caseify(Iter first, Iter last)
{
	STD_::transform(first, last, first, std_ttolower);
}

template<class Iter> void upper_caseify(Iter first, Iter last)
{
	STD_::transform(first, last, first, std_ttoupper);
}

END_DTL_NAMESPACE

#endif
