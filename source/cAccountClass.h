//o--------------------------------------------------------------------------o
//|	File					-	cAccountClass.h
//|	Date					-	12/6/2002 4:33:44 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Header file for the cAccountClass class. See xRFC0004.txt
//|									for more details.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
#ifndef __CACCOUNTCLASS_H__
#define __CACCOUNTCLASS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786 )

#ifdef __linux__
	#define __unix__
#endif


#if __LINUX__
#include <dirent.h>
/*
#include <glob.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
*/
#define strnicmp(a,b,c) strnbasecmp(a,b,c)
#define _stat stat
#define _mkdir mkdir
#define _rmdir rmdir

#else
/*
#include <winsock2.h>
#include <windows.h>
*/
#include <direct.h>
/*
#include <io.h>
#include <time.h>
#include <sys/timeb.h>
*/
#define _mkdir(s1,s2) _mkdir(s1)
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdio>
#include <ios>
#include <map>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "CChar.h"
#include "cVersionClass.h"

// Enums
enum __ACCOUNTBBLOCK_FLAGS__
{
	AB_USERNAME=1,
	AB_PASSWORD=2,
	AB_FLAGS=4,
	AB_PATH=8,
	AB_TIMEBAN=16,
	AB_CONTACT=32,
	AB_CHARACTER1=64,
	AB_CHARACTER2=128,
	AB_CHARACTER3=256,
	AB_CHARACTER4=512,
	AB_CHARACTER5=1024,
	AB_ALL=2047
};
//
enum __ACCOUNTBLOCK_ERRORS__
{
	AB_INVALID_ID=0xffff
};
//
enum __ACCOUNTBLOCK_FLAGS__
{
	AB_BLAGS_NONE=0x0000,
	AB_FLAGS_BANNED=0x0001,
	AB_FLAGS_UNUSED0=0x0002,
	AB_FLAGS_PUBLIC=0x0004,
	AB_FLAGS_ONLINE=0x0008,
	AB_FLAGS_UNUSED1=0x0010,
	AB_FLAGS_UNUSED2=0x0020,
	AB_FLAGS_UNUSED3=0x0040,
	AB_FLAGS_UNUSED4=0x0080,
	AB_FLAGS_UNUSED5=0x0100,
	AB_FLAGS_UNUSED6=0x0200,
	AB_FLAGS_UNUSED8=0x0400,
	AB_FLAGS_UNUSED9=0x0800,
	AB_FLAGS_XGM=0x1000,
	AB_FLAGS_SEER=0x2000,
	AB_FLAGS_COUNSELOR=0x4000,
	AB_FLAGS_GM=0x8000,
	AB_FLAGS_ALL=0xFFFF
};
//

//o--------------------------------------------------------------------------o
//|	Class/Struct	-	typedef struct __ACCOUNTSADM_BLOCK__
//|	Date					-	12/6/2002 5:46:10 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	All the data has been moved to once again stored in the
//|									accounts.adm file. So changes to this typedef were needed
//|									to reflect the members being moved back, and the inclusion
//|									of PATH, and FLAGS to cover the storage locations.
//|									
//|									sUsername: username for this account
//|									sPassword: password for this account
//|									aPath    : path to where the username.uad file will be stored
//|									sContact : typically the email address for this account
//|									wAccountIndex: which block is this accounts.
//|									wFlags   : flags controlling the allowed accounts functions
//|									dwLastIP : ip address of the last connection from this account
//|									dwCharacters[5]: straight ID number for this account
//|									lpCharacters[5]: 
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
typedef struct __ACCOUNTSADM_BLOCK__
{
	std::string sUsername;
	std::string sPassword;
	std::string sPath;
	std::string sContact;
	WORD wAccountIndex;
	WORD wFlags;
	WORD wTimeBan;
	DWORD dwInGame;
	DWORD dwLastIP;
	DWORD dwCharacters[5];
	CChar	*lpCharacters[5];	
} ACCOUNTSBLOCK,*LPACCOUNTSBLOCK;

// Class typdefs to help simplify the use of map STL
typedef std::map<std::string,ACCOUNTSBLOCK> MAPUSERNAME;
typedef std::map<WORD,ACCOUNTSBLOCK> MAPUSERNAMEID;
typedef std::map<std::string,ACCOUNTSBLOCK>::iterator MAPUSERNAME_ITERATOR;
typedef std::map<std::string,ACCOUNTSBLOCK>::const_iterator MAPUSERNAME_CITERATOR;
typedef std::map<WORD,ACCOUNTSBLOCK>::iterator MAPUSERNAMEID_ITERATOR;
typedef std::map<WORD,ACCOUNTSBLOCK>::const_iterator MAPUSERNAMEID_CITERATOR;
//o--------------------------------------------------------------------------o
//|	Class/Struct	-	class cAccountClass
//|	Date					-	12/6/2002 5:46:02 AM
//|	Developers		-	Matthew J. Randall / mrandall (mrandall@adtastik.net)
//|	Organization	-	MyndTrip Technologies / MyndTrip Studios / EAWare
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
class cAccountClass  
{
public:
	// Construction/Destruction
	cAccountClass();
	cAccountClass(std::string sAccountsPath);
	virtual ~cAccountClass();
	// Operator overloads
	cAccountClass& operator++();
	cAccountClass& operator--(int);
	//cAccountClass& operator+=(int);
	//cAccountClass& operator>>(ACCOUNTSBLOCK& actbRef);
	// Member Functions
	WORD CreateAccountSystem(void);
	WORD AddAccount(std::string sUsername, std::string sPassword, std::string sContact="NONE", WORD wAttributes=0x0000);
	BOOL DelAccount(std::string sUsername);
	BOOL DelAccount(WORD wAccountID);
	BOOL ModAccount(std::string sUsername,DWORD dwFlags,ACCOUNTSBLOCK &actbBlock);
	BOOL ModAccount(WORD wAccountID,DWORD dwFlags,ACCOUNTSBLOCK &actbBlock);
	BOOL SetPath(std::string sPath);
	std::string GetPath(void);
	WORD Save(void);
	WORD Load(void);
	WORD size(void);
	BOOL clear(void);
	BOOL isUser(std::string sUsername);
	BOOL AddCharacter(WORD wAccountID, CChar *lpObject);
	BOOL AddCharacter(WORD wAccountID,DWORD dwCharacterID, CChar *lpObject);
	BOOL DelCharacter(WORD wAccountID, int nSlot);
	BOOL TransCharacter(WORD wSAccountID,WORD wSSlot,WORD wDAccountID);
	BOOL GetAccountByName(std::string sUsername,ACCOUNTSBLOCK& actbBlock);
	BOOL GetAccountByID(WORD wAccountID,ACCOUNTSBLOCK& actbBlock);
	long atol(const char * lpszValue);
	long atol(std::string sValue);
	MAPUSERNAMEID_ITERATOR& Begin(void);
	MAPUSERNAMEID_ITERATOR& End(void);
	MAPUSERNAMEID_ITERATOR& Last(void);
	// Member variables
	WORD m_wHighestAccount;
	std::string m_sAccountsDirectory;
	std::string& PathFix(std::string& sPath);
	MAPUSERNAME m_mapUsernameMap;
	MAPUSERNAMEID m_mapUsernameIDMap;
	MAPUSERNAMEID_ITERATOR I;
private:
	// Member Functions
	VOID WriteAccountsHeader(std::fstream &fsOut);
	VOID WriteAccessHeader(std::fstream &fsOut);
	VOID WriteOrphanHeader(std::fstream &fsOut);
	VOID WriteUADHeader(std::fstream &fsOut,ACCOUNTSBLOCK& actbTemp);
};

#endif // __CACCOUNTCLASS_H__