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
#include "cVersionClass.h"

//o---------------------------------------------------------------------------o
// product info
//o---------------------------------------------------------------------------o
#define VER "0.97.02"
#define BUILD "1j"
#define REALBUILD "0"
#define SVER "2.0"
#define CVER "2.0"
#define IVER "2.0"
#define PRODUCT "Ultima Offline eXperiment 3"
#define NAME "The Official DevTeam"
#define EMAIL "http://uox3.sourceforge.net/"
#define PROGRAMMERS "UOX3 DevTeam"

cVersionClass CVC;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
cVersionClass::cVersionClass()
{
	SetVersion(VER);
	SetBuild(BUILD);
	SetRealBuild(REALBUILD);
	SetScriptVersion(SVER);
	SetItemVersion(CVER);
	SetCharacterVersion(IVER);
	SetProductName(PRODUCT);
	SetName(NAME);
	SetProgrammers(EMAIL);
	SetEmail(PROGRAMMERS);
}
//o---------------------------------------------------------------------------o
cVersionClass::~cVersionClass()
{

}

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
void cVersionClass::SetVersion(char *szVersion)
{
	strncpy(szIVersion,szVersion,sizeof(char)*32);
}
//
char *cVersionClass::GetVersion(void)
{
	return szIVersion;
}

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
void cVersionClass::SetBuild(char *szBuild)
{
	strncpy(szIBuild,szBuild,sizeof(char)*32);
}
//
char *cVersionClass::GetBuild(void)
{
	return szIBuild;
}

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
void cVersionClass::SetRealBuild(char *szRealBuild)
{
	strncpy(szIRealBuild,szRealBuild,sizeof(char)*32);
}
//
char *cVersionClass::GetRealBuild(void)
{
	return szIRealBuild;
}

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
void cVersionClass::SetScriptVersion(char *szScriptVersion)
{
	strncpy(szIScriptsVersion,szScriptVersion,sizeof(char)*32);
}
//
char *cVersionClass::GetScriptVersion(void)
{
	return szIScriptsVersion;
}

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
void cVersionClass::SetCharacterVersion(char *szCharacterVersion)
{
	strncpy(szICharactersVersion,szCharacterVersion,sizeof(char)*32);
}
//
char *cVersionClass::GetCharacterVersion(void)
{
	return szICharactersVersion;
}

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
void cVersionClass::SetItemVersion(char *szItemVersion)
{
	strncpy(szIItemsVersion,szItemVersion,sizeof(char)*32);
}
//
char *cVersionClass::GetItemVersion(void)
{
	return szIItemsVersion;
}

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
void cVersionClass::SetProductName(char *szProduct)
{
	strncpy(szIProductName,szProduct,sizeof(char)*32);
}
//
char *cVersionClass::GetProductName(void)
{
	return szIProductName;
}

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
void cVersionClass::SetName(char *szName)
{
	strncpy(szIName,szName,sizeof(char)*32);
}
//
char *cVersionClass::GetName(void)
{
	return szIName;
}

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
void cVersionClass::SetEmail(char *szEmail)
{
	strncpy(szIEmail,szEmail,sizeof(char)*32);
}
//
char *cVersionClass::GetEmail(void)
{
	return szIEmail;
}

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
void cVersionClass::SetProgrammers(char *szProgrammers)
{
	strncpy(szIProgrammers,szProgrammers,sizeof(char)*32);
}
//
char *cVersionClass::GetProgrammers(void)
{
	return szIProgrammers;
}
