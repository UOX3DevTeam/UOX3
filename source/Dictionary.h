// Dictionary.h: interface for the CDictionary class.
//
//////////////////////////////////////////////////////////////////////
//#include "uox3.h"
#include <vector>
#include <map>

#if !defined(AFX_DICTIONARY_H__61EDE933_2045_11D4_9032_00104B73C455__INCLUDED_)
#define AFX_DICTIONARY_H__61EDE933_2045_11D4_9032_00104B73C455__INCLUDED_

using namespace std;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace std;

typedef map<long, string> ourDict;

const SI32 dictCANTOPEN = -1;
const SI32 dictDUPESECTION = -2;
const SI32 dictDUPEOPENBRACE = -3;
const SI32 dictNOOPENBRACE = -4;

class CDictionary
{
public:
					CDictionary();
					CDictionary( const char *filepath, const char *language );
	virtual			~CDictionary();
	const char *	operator[]( SI32 Num );
	const char *	GetEntry( SI32 Num );

public:
	void			ShowList( void );
	SI32			LoadDictionary( void );
	SI32			NumberOfEntries( void );

private:
	bool IsValid;
	char PathToDictionary[512];
	char Language[8];
	map< long, string > Text2;
};

class CDictionaryContainer
{
private:
	CDictionary		*dictList[TOTAL_LANGUAGES];
	UnicodeTypes	defaultLang;
public:
					CDictionaryContainer();
					CDictionaryContainer( const char *filepath );
	virtual 		~CDictionaryContainer();
	SI32			LoadDictionary( void );
	const char *	operator[]( SI32 Num );	// only default lang
	const char *	GetEntry( SI32 Num, UnicodeTypes toDisp = ZERO );
	void			SetDefaultLang( UnicodeTypes newType );
};

#endif // !defined(AFX_DICTIONARY_H__61EDE933_2045_11D4_9032_00104B73C455__INCLUDED_)
