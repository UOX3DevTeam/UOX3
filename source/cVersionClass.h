//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	10/17/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
#ifndef __CVERSIONCLASS_H__
#define __CVERSIONCLASS_H__

#include <string.h>

#if defined(_MSC_VER)
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif

class cVersionClass  
{
public:
	cVersionClass();
	virtual ~cVersionClass();
	// Need to set our values for the system. Pair matching Get routine as well(script modification connection)
	virtual void SetVersion(char *szVersion);
	char *GetVersion(void);
	virtual void SetBuild(char *szBuild);
	char *GetBuild(void);
	virtual void SetRealBuild(char *szRealBuild);
	char *GetRealBuild(void);
	virtual void SetScriptVersion(char *szScriptVersion);
	char *GetScriptVersion(void);
	virtual void SetCharacterVersion(char *szCharacterVersion);
	char *GetCharacterVersion(void);
	virtual void SetItemVersion(char *szItemVersion);
	char *GetItemVersion(void);
	virtual void SetProductName(char *szProduct);
	char *GetProductName(void);
	virtual void SetName(char *szName);
	char *GetName(void);
	virtual void SetEmail(char *szEmail);
	char *GetEmail(void);
	virtual void SetProgrammers(char *szProgrammers);
	char *GetProgrammers(void);

public:
	char szIVersion[32];
	char szIBuild[32];
	char szIRealBuild[32];
	char szIScriptsVersion[32];
	char szICharactersVersion[32];
	char szIItemsVersion[32];
	char szIProductName[80];
	char szIEmail[128];
	char szIProgrammers[128];
	char szIName[80];
};

#endif // __CVERSIONCLASS_H__
