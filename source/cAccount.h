//o--------------------------------------------------------------------------o
//|	Function/Class-	cAccounts
//|	Date					-	02/11/2002
//o--------------------------------------------------------------------------o
//|	Description		-	Class that handles the accounts portion of server
//|									operation.
//o--------------------------------------------------------------------------o
#include <map>
#include <string>

#if !defined(AFX_CACCOUNT_H__6FD26C86_F155_11D3_9002_00104B73C455__INCLUDED_)
#define AFX_CACCOUNT_H__6FD26C86_F155_11D3_9002_00104B73C455__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace std;

// EvilDeD: 021802: Iterator typedefs, I did this change only for readability
typedef map< string, ACTREC *>::const_iterator CONST_ACTREC_ITERATOR;
typedef map< string, ACTREC *>::iterator ACTREC_ITERATOR;
typedef map< int, ACTREC *>::iterator ACTIREC_ITERATOR;
typedef map< int, ACTREC *>::const_iterator CONST_ACTIREC_ITERATOR;
typedef map< string, AAREC *>::iterator AAREC_ITERATOR;
typedef map< string, AAREC *>::const_iterator CONST_AAREC_ITERATOR;
// 
typedef map< string, ACTREC *> ACTRECMAP;
typedef map< int, ACTREC *> ACTIRECMAP;
typedef map< string, AAREC *> AARECMAP;
typedef vector< ACTREC* > ACTRECVEC;
typedef vector< CChar * > CCHARVEC;
//

enum ActFlags 
{
	ALLOWED		= 0x00000000,
	BANNED		= 0x00000001,
	TIMELIMIT	= 0x00000002,
	LOGACTIONS	= 0x00000003
};

class cAccounts  
{
public:
	cAccounts();
	cAccounts( const char *accountsfile );
	~cAccounts();

	virtual bool 	CheckAccountsStamp( void );
	virtual long	LoadAccessList( void );
	virtual long	LoadAccounts( void );
	virtual long	SaveAccounts( void );
	virtual long	SaveAccounts( string AccessPath, string AccountsPath );
	virtual bool	UnloadAccounts( void );

	bool AddCharacterToAccount( SI32 accountid, CChar *object );
	bool RemoveCharacterFromAccount( SI32 accountid, UI08 slot );
	bool RemoveCharacterFromAccount( ACTREC *toRemove, UI08 slot );
	bool ConvertAccounts( const char *filename );
	void AddAccount( string username, string password, string contact );
	void DeleteAccount( string username );
	const char *CheckAccountsVersion( const char *filename );
	long GetAccountCount( void );

	ACTREC *NextAccount( void );
	ACTREC *FirstAccount( void );
	bool	 FinishedAccounts( void );
	
	virtual AAREC		*GetAccessBlock( const char *username );
	virtual ACTREC	*GetAccount( const char *username );
	virtual ACTREC	*GetAccountByID( SI32 id );

	void			SetFilePath( const char *filepath );
	const char *	GetFilePath( void );

	bool			IPExists( UI08 ip1, UI08 ip2, UI08 ip3, UI08 ip4 ) const;

	SI32			Count( void ) const;
private:
	CONST_ACTREC_ITERATOR		gCI;
	long			AccountCount;
	virtual UI32	ReadToken( FILE *openfile );
	
	ACTRECMAP		actMap;
	ACTIRECMAP	actByNum;
	AARECMAP		aarMap;
	// EviLDeD: 021802: This will be supported dont remove please.
	ACTRECVEC		actDeletionVector;
	CCHARVEC		ccharDeletionVector;
	//

	string			PathToFile;
	SI32			highestAccountNumber;
	void			PostLoadProcessing( void );
protected:
	bool			isReloaded;
	bool			isBinary;
	bool			isValid;
	char *			szAccountsVersion;
	string			ErrDesc;
	UI32			ErrNum;
};

#endif // !defined(AFX_CACCOUNT_H__6FD26C86_F155_11D3_9002_00104B73C455__INCLUDED_)
