// Dictionary.cpp: implementation of the CDictionary class.
//
//////////////////////////////////////////////////////////////////////

#include "uox3.h"
#include <vector>
#include <fstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDictionary::CDictionary()
{
	strcpy( Language, "ZRO" );
	strcpy( PathToDictionary, "./dictionary.ZRO" );
}

CDictionary::~CDictionary()
{
}

CDictionary::CDictionary( const char *filepath, const char *language )
{
	CDictionary();	//	Call default constructor for this Class

	if( language == NULL )
		strcpy( Language, "ZRO" );
	else
		strcpy( Language, language );

	//	Load the path
	if( filepath == NULL )
		strcpy( PathToDictionary, "dictionary.ZRO" );
	else
		strcpy( PathToDictionary, filepath );
}

const UI16 DICT_MAX_TEXTBUFFERSIZE = 1024;

SI32 CDictionary::LoadDictionary( void )
{
	std::ifstream ifsFile;
	char szTxtBfr[DICT_MAX_TEXTBUFFERSIZE];
	long count = 0;

	ifsFile.open( PathToDictionary, std::ios::in );
	if( !ifsFile.is_open() )
	{
		Console.Warning( 2, "Failed to open dictionary.%s", Language );
		return dictCANTOPEN;
	}
	bool isSection[3] = { false, false, false };
	while( !ifsFile.eof() && !ifsFile.fail() )
	{
		ifsFile.getline( szTxtBfr, DICT_MAX_TEXTBUFFERSIZE );
		if( szTxtBfr[0] == '/' && szTxtBfr[1] == '/' )
			continue;	//	Deal with comments through out the text.
		else if( isSection[0] == false && !strnicmp( szTxtBfr, "SECTION DICTIONARY", 18 ) )
		{
			isSection[0] = true;
		}
		else if( isSection[0] == true && !strnicmp( szTxtBfr, "SECTION DICTIONARY", 18 ) )
		{	//	This is a serious error condition seeing as there should never encounter another SECTION header
			isSection[0] = isSection[1] = isSection[2] = false;
			return dictDUPESECTION;
		}
		else if( szTxtBfr[0] == '{' && isSection[1] )
		{	//	Not a good idea either if there is more than one start { encountered
			isSection[0] = isSection[1] = isSection[2] = false;
			return dictDUPEOPENBRACE;
		}
		else if( szTxtBfr[0] == '}' && !isSection[1] )
		{	//	Not a good idea either if there is a } found before a {
			isSection[0] = isSection[1] = isSection[2] = false;
			return dictNOOPENBRACE;
		}
		else if( szTxtBfr[0] == '{' && isSection[0] )
		{
			isSection[1] = true;
			continue;
		}
		else if( szTxtBfr[0] == '}' && isSection[0] && isSection[1] )
		{
			isSection[2] = true;
		}
		else if( isSection[1] && !isSection[2] )
		{	//	Ok read in the lines from the file and insert them into the vector
			char *tok[2];
			tok[0] = strtok( szTxtBfr, "=" );
			tok[1] = strtok( NULL, "\n" );
			if( tok[0] == NULL || tok[1] == NULL )	// malformed
			{
				if( tok[0] != NULL && tok[0][0] != '/' )
					Console.Error( 2, "Bad dictionary entry! (%s) in dictionary %s", tok[0], PathToDictionary );
				continue;
			}
			Text2[atol(tok[0])] = tok[1];
			count++;
		}
		else if( isSection[1] && isSection[2] )
			break;	//	Done reading for now, as we only support one dictionary. I left the ability here though to add more if code requires it
	}
	ifsFile.close();
	return count;
}

void CDictionary::ShowList( void )
{
	Console << "[Testing]" << myendl;
	std::map< long, std::string >::iterator toFind = Text2.begin();
	while( toFind != Text2.end() )
	{
		Console << (SI32)(toFind->first) << ") " << toFind->second.c_str() << myendl;
		toFind++;
	}
}

const char *CDictionary::operator[]( SI32 Num )
{
	return GetEntry( Num );
}

SI32 CDictionary::NumberOfEntries( void )
{
	return Text2.size();
}

const char *CDictionary::GetEntry( SI32 Num )
{
	std::map< long, std::string >::iterator toFind = Text2.find( Num );
	if( toFind != Text2.end() )
		return toFind->second.c_str();
	else
		Console.Warning( 2, "WARNING: Dictionary Reference %i not found in \"%s\"\n", Num, PathToDictionary );
	return NULL;
}

CDictionaryContainer::CDictionaryContainer()
{
	char buildName[MAX_PATH];
	for( int i = (int)DL_UNKNOWN; i < (int)DL_COUNT; i++ )
	{
		sprintf( buildName, "%sdictionary.%s", cwmWorldState->ServerData()->GetDictionaryDirectory(), DistinctLanguageNames[i].c_str() );
		dictList[i] = new CDictionary( buildName, DistinctLanguageNames[i].c_str() );
	}
}

CDictionaryContainer::CDictionaryContainer( const char *filepath )
{
	char buildName[MAX_PATH];
	for( int i = (int)DL_UNKNOWN; i < (int)DL_COUNT; i++ )
	{
		sprintf( buildName, "%sdictionary.%s", filepath, DistinctLanguageNames[i].c_str() );
		dictList[i] = new CDictionary( buildName, DistinctLanguageNames[i].c_str() );
	}
}

CDictionaryContainer::~CDictionaryContainer()
{
	for( int i = (int)DL_UNKNOWN; i < (int)DL_COUNT; i++ )
		delete dictList[i];
}

SI32 CDictionaryContainer::LoadDictionary( void )
{
	UI32 j = 0, k = 0;
	bool oneValid = false;
	for( int i = (int)DL_UNKNOWN; i < (int)DL_COUNT; i++ )
	{
		k = dictList[i]->LoadDictionary();
		if( k == 0 )
			oneValid = true;
		else
			j += k;
	}
	if( j > 0 )
		Console << "Loading dictionaries...        ";
	if( oneValid )
		return 0;
	else
		return j;
}

const char *CDictionaryContainer::operator[]( SI32 Num )
{
	return GetEntry( Num );
}

const char *CDictionaryContainer::GetEntry( SI32 Num, UnicodeTypes toDisp )
{
	const char *RetVal = dictList[LanguageCodesLang[toDisp]]->GetEntry( Num );
	if( RetVal == NULL && toDisp != ZERO )//if we are using a diff language, and the entry wasn't found, try the ZERO before we flat out return null
		RetVal = dictList[LanguageCodesLang[ZERO]]->GetEntry( Num );

	return RetVal;
	//return dictList[LanguageCodesLang[toDisp]]->GetEntry( Num );
}

void CDictionaryContainer::SetDefaultLang( UnicodeTypes newType )
{
	defaultLang = newType;
}
