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

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdio>
#include <iosfwd>
#include <algorithm>
#include <cctype>
#include <vector>
#include <map>

#ifdef __LINUX__
	#include <dirent.h>
	#define strnicmp(a,b,c) strnbasecmp(a,b,c)
	#define _stat stat
	#define _mkdir mkdir
	#define _rmdir rmdir
#else
	#include <direct.h>
	#define _mkdir(s1,s2) _mkdir(s1)
#endif

#include "uox3.h"

//#ifdef __UOX3_DTL__
//	#include "DTL.h"
//	#include "variant_cc.h"
//#endif

/* Enums */
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
	UI16 wAccountIndex;
	UI16 wFlags;
	UI16 wTimeBan;
	UI32 dwInGame;
	UI32 dwLastIP;
	UI32 dwCharacters[5];
	CChar	*lpCharacters[5];	
} ACCOUNTSBLOCK,*LPACCOUNTSBLOCK;

//o--------------------------------------------------------------------------o
//|	Class/Struct	-	class cDBAccountClass
//|	Date					-	1/20/2003 12:52:32 PM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	For use with the DTL libraries for DB connections, it 
//|									seemed easier to just make a duplicate so to speak ok the
//|									cAccountClass without the member functions. This is basically
//|									a copy of the __ACCOUNTSADM_BLOCK__ structure. As the DTL
//|									is streamed, and seemed to required a transfer class for
//|									record col storage, this class was created to interface
//|									with the DTL
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
#ifdef __UOX3_DTL__
class cDBAccountClass
{
public:
	std::string sUsername;
	std::string sPassword;
	std::string sPath;
	std::string sContact;
	UI32 dwCommentID;
	UI32 dwAccountID;
	UI32 dwFlags;
	UI32 dwTimeBan;
	UI32 dwInGame;
	UI32 dwLastIP;
	UI32 dwCharacter1;
	UI32 dwCharacter2;
	UI32 dwCharacter3;
	UI32 dwCharacter4;
	UI32 dwCharacter5;
	// Constructor
	cDBAccountClass(std::string &sUUsername,std::string &sUPassword,std::string &sUPath,
									std::string &sUContact,UI32 dwUCommentID, UI32 dwUAccountIndex,
									UI32 dwUFlags,UI32 dwUTimeBan,UI32 dwUInGame,UI32 dwULastIP,
									UI32 dwUChar1,UI32 dwUChar2,UI32 dwUChar3,UI32 dwUChar4,
									UI32 dwUChar5) :
									sUsername(sUUsername),sPassword(sUPassword),sPath(sUPath),
									sContact(sUContact),dwCommentID(dwUCommendID),wAccountID(dwUAccountIndex),
									dwFlags(dwUFlags),dwTimeBan(dwUTimeBan),dwInGame(dwUInGame),
									dwLastIP(dwULastIP),dwCharacter1(dwUChar1),dwCharacter2(dwUChar2),
									dwCharacter3(dwUChar3),dwCharacter4(dwUChar4),dwCharacter5(dwUChar5) { }

};
//
template<> class dtl::DefaultBCA<cDBAccountClass>
{
public:
	void operator() (BoundIOs &cols,cDBAccountClass &rowbuf)
	{
		cols["fAccountID"] == rowbuf.dwAccountID;
		cols["fCommentID"] == rowbuf.dwCommentID;
		cols["fUsername"] == rowbuf.sUsername;
		cols["fPassword"] == rowbuf.sPassword;
		cols["fPath"] == rowbuf.sPath;
		cols["fFlags"] == rowbuf.dwFlags;
		cols["fTimeBan"] == rowbuf.dwTimeBan;
		cols["fInGame"] == rowbuf.ddwInGame;
		cols["fCharacter1"] == rowbuf.dwCharacter1;
		cols["fCharacter2"] == rowbuf.dwCharacter2;
		cols["fCharacter3"] == rowbuf.dwCharacter3;
		cols["fCharacter4"] == rowbuf.dwCharacter4;
		cols["fCharacter5"] == rowbuf.dwCharacter5;
		cols["fContact"] == rowbuf.sContact;
	}
};
//
#endif

// Class typdefs to help simplify the use of map STL
typedef std::map<std::string,ACCOUNTSBLOCK> MAPUSERNAME;
typedef std::map<UI16,ACCOUNTSBLOCK> MAPUSERNAMEID;
typedef std::map<std::string,ACCOUNTSBLOCK>::iterator MAPUSERNAME_ITERATOR;
typedef std::map<std::string,ACCOUNTSBLOCK>::const_iterator MAPUSERNAME_CITERATOR;
typedef std::map<UI16,ACCOUNTSBLOCK>::iterator MAPUSERNAMEID_ITERATOR;
typedef std::map<UI16,ACCOUNTSBLOCK>::const_iterator MAPUSERNAMEID_CITERATOR;
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
	UI16 CreateAccountSystem(void);
	UI16 AddAccount(std::string sUsername, std::string sPassword, std::string sContact="NONE", UI16 wAttributes=0x0000);
	bool DelAccount(std::string sUsername);
	bool DelAccount(UI16 wAccountID);
	bool ModAccount(std::string sUsername,UI32 dwFlags,ACCOUNTSBLOCK &actbBlock);
	bool ModAccount(UI16 wAccountID,UI32 dwFlags,ACCOUNTSBLOCK &actbBlock);
	bool SetPath(std::string sPath);
	std::string GetPath(void);
	UI16 Save(void);
	UI16 Load(void);
	UI16 size(void);
	bool clear(void);
	bool isUser(std::string sUsername);
	bool AddCharacter(UI16 wAccountID, CChar *lpObject);
	bool AddCharacter(UI16 wAccountID,UI32 dwCharacterID, CChar *lpObject);
	bool DelCharacter(UI16 wAccountID, int nSlot);
	bool TransCharacter(UI16 wSAccountID,UI16 wSSlot,UI16 wDAccountID);
	bool GetAccountByName(std::string sUsername,ACCOUNTSBLOCK& actbBlock);
	bool GetAccountByID(UI16 wAccountID,ACCOUNTSBLOCK& actbBlock);
	long atol(const char * lpszValue);
	long atol(std::string sValue);
	MAPUSERNAMEID_ITERATOR& begin(void);
	MAPUSERNAMEID_ITERATOR& end(void);
	MAPUSERNAMEID_ITERATOR& last(void);
	// Member variables
	UI16 m_wHighestAccount;
	std::string m_sAccountsDirectory;
	std::string& PathFix(std::string& sPath);
	MAPUSERNAME m_mapUsernameMap;
	MAPUSERNAMEID m_mapUsernameIDMap;
	MAPUSERNAMEID_ITERATOR I;

#ifdef __UOX3_DTL__
	std::vector<cDBAccountClass> m_vecDTLAccountVector;
#endif

private:
	// Member Functions
	void WriteAccountsHeader(std::fstream &fsOut);
	void WriteAccessHeader(std::fstream &fsOut);
	void WriteOrphanHeader(std::fstream &fsOut);
	void WriteUADHeader(std::fstream &fsOut,ACCOUNTSBLOCK& actbTemp);
};

#endif // __CACCOUNTCLASS_H__