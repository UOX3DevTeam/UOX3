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

#if defined(_MSC_VER)
#pragma warning(disable : 4786 )
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif

#include <iosfwd>

#if UOX_PLATFORM != PLATFORM_WIN32
	#include <dirent.h>
	#define _stat stat
	#define _mkdir mkdir
	#define _rmdir rmdir
#else
	#include <direct.h>
	#define _mkdir(s1,s2) _mkdir(s1)
#endif

namespace UOX
{

// Enums

//
enum CAccountBlock_Errors
{
	AB_INVALID_ID = 0xFFFF
};
//
enum CAccountBlock_Flags
{
	AB_FLAGS_BANNED		=	0,
	AB_FLAGS_SUSPENDED	=	1,
	AB_FLAGS_PUBLIC		=	2,
	AB_FLAGS_ONLINE		=	3,
	AB_FLAGS_CHARACTER1	=	4,
	AB_FLAGS_CHARACTER2	=	5,
	AB_FLAGS_CHARACTER3	=	6,
	AB_FLAGS_CHARACTER4	=	7,
	AB_FLAGS_CHARACTER5	=	8,
	AB_FLAGS_CHARACTER6	=	9,
	AB_FLAGS_UNUSED8	=	10,
	AB_FLAGS_UNUSED9	=	11,
	AB_FLAGS_UNUSED10	=	12,
	AB_FLAGS_SEER		=	13,
	AB_FLAGS_COUNSELOR	=	14,
	AB_FLAGS_GM			=	15,
	AB_FLAGS_ALL		=	16
};
//

//o--------------------------------------------------------------------------o
//|	Class/Struct	-	typedef struct CAccountBlock
//|	Date			-	12/6/2002 5:46:10 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
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
typedef struct CAccountBlock
{
#if _NOACTCOPY_
private:
	CAccountBlock::CAccountBlock( const CAccountBlock& );
	CAccountBlock& CAccountBlock::operator=( const CAccountBlock& );
#endif
public:
	CAccountBlock( void ) : sUsername( "" ), sPassword( "" ), sPath( "" ), sContact( "" ),
		wAccountIndex( 0xFFFF ), wTimeBan( 0x0000 ), dwInGame( INVALIDSERIAL ),
		dwLastIP( 0x00000000 ), bChanged( false )
	{
		for( UI08 i = 0; i < 6; ++i )
		{
			dwCharacters[i] = 0xFFFFFFFF;
			lpCharacters[i] = NULL;
		}
	};
	void reset( void )
	{
		sUsername		= "";
		sPassword		= "";
		sContact		= "";
		sPath			= "";
		wAccountIndex	= 0xFFFF;
		wTimeBan		= 0x0000;
		dwInGame		= 0xFFFFFFFF;
		dwLastIP		= 0x00000000;
		bChanged		= false;
		for( UI08 i = 0; i < 6; ++i )
		{
			dwCharacters[i] = 0xFFFFFFFF;
			lpCharacters[i] = NULL;
		}
		wFlags.reset();
	};
	std::string					sUsername;
	std::string					sPassword;
	std::string					sPath;
	std::string					sContact;
	UI16						wAccountIndex;
	std::bitset< AB_FLAGS_ALL >	wFlags;
	UI16						wTimeBan;
	UI32						dwInGame;
	UI32						dwLastIP;
	bool						bChanged;
	UI32						dwCharacters[6];
	CChar *						lpCharacters[6];
} CAccountBlock;

// Class typdefs to help simplify the use of map STL
typedef std::map< std::string, CAccountBlock * >					MAPUSERNAME;
typedef std::map< UI16, CAccountBlock >								MAPUSERNAMEID;
typedef std::map< std::string, CAccountBlock * >::iterator			MAPUSERNAME_ITERATOR;
typedef std::map< std::string, CAccountBlock * >::const_iterator	MAPUSERNAME_CITERATOR;
typedef std::map< UI16, CAccountBlock >::iterator					MAPUSERNAMEID_ITERATOR;
typedef std::map< UI16, CAccountBlock >::const_iterator				MAPUSERNAMEID_CITERATOR;
//o--------------------------------------------------------------------------o
//|	Class/Struct	-	class cAccountClass
//|	Date			-	12/6/2002 5:46:02 AM
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
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
	~cAccountClass();
	// Operator overloads
	cAccountClass& operator++();
	cAccountClass& operator--(int);
	UI16					CreateAccountSystem( void );
	UI16					ImportAccounts( void );
	void					WriteAccountSection( CAccountBlock& actbTemp, std::fstream& fsOut );
	UI16					AddAccount( std::string sUsername, std::string sPassword, std::string sContact="NONE", UI16 wAttributes=0x0000 );
	bool					DelAccount( std::string sUsername );
	bool					DelAccount( UI16 wAccountID );
	bool					SetPath( std::string sPath );
	std::string				GetPath( void );
	UI16					Save( bool bForceLoad = false );
	UI16					Load( void );
	size_t					size( void );
	bool					clear( void );
	bool					isUser( std::string sUsername );
	bool					AddCharacter( UI16 wAccountID, CChar *lpObject );
	bool					AddCharacter( UI16 wAccountID, UI32 dwCharacterID, CChar *lpObject );
	bool					DelCharacter( UI16 wAccountID, UI08 nSlot );
	bool					TransCharacter( UI16 wSAccountID, UI16 wSSlot, UI16 wDAccountID );
	CAccountBlock&			GetAccountByName( std::string sUsername );
	CAccountBlock&			GetAccountByID( UI16 wAccountID );
	MAPUSERNAMEID_ITERATOR&	begin( void );
	MAPUSERNAMEID_ITERATOR&	end( void );
	MAPUSERNAMEID_ITERATOR&	last( void );
	// Member variables
	MAPUSERNAMEID_ITERATOR I;
private:
	// Member Functions
	void WriteAccountsHeader( std::fstream &fsOut );
	void WriteAccessHeader( std::fstream &fsOut );
	void WriteOrphanHeader( std::fstream &fsOut );
	void WriteUADHeader( std::fstream &fsOut, CAccountBlock& actbTemp );
	void WriteImportHeader( std::fstream &fsOut );

	CAccountBlock	actbInvalid;
	MAPUSERNAME		m_mapUsernameMap;
	MAPUSERNAMEID	m_mapUsernameIDMap;
	UI16			m_wHighestAccount;
	std::string		m_sAccountsDirectory;
};

extern cAccountClass *Accounts;

}

#endif // __CACCOUNTCLASS_H__


