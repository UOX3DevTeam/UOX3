// Dictionary.h: interface for the CDictionary class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#if defined(_MSC_VER)
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif

namespace UOX
{

class CDictionary
{
public:
					CDictionary();
					CDictionary( std::string filepath, std::string language );
					~CDictionary();
	std::string 	operator[]( SI32 Num );
	std::string 	GetEntry( SI32 Num );

public:
	void			ShowList( void );
	SI32			LoadDictionary( void );
	size_t			NumberOfEntries( void );
	void			SetValid( bool newVal );
	bool			GetValid( void );

private:
	bool IsValid;
	std::string PathToDictionary;
	std::string Language;
	std::map< long, std::string > Text2;
};

class CDictionaryContainer
{
private:
	CDictionary		*dictList[DL_COUNT];
	UnicodeTypes	defaultLang;
public:
					CDictionaryContainer();
					CDictionaryContainer( std::string filepath );
			 		~CDictionaryContainer();
	SI32			LoadDictionary( void );
	std::string 	operator[]( SI32 Num );	// only default lang
	std::string 	GetEntry( SI32 Num, UnicodeTypes toDisp = ZERO );
	void			SetDefaultLang( UnicodeTypes newType );
};

extern CDictionaryContainer *Dictionary;

}

#endif
