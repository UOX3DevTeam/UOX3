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

namespace UOX
{


#if defined(_MSC_VER)
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif

class CVersionClass  
{
public:
						CVersionClass();
						~CVersionClass();
	static std::string	GetVersion( void );
	static std::string	GetBuild( void );
	static std::string	GetRealBuild( void );
	static std::string	GetScriptVersion( void );
	static std::string	GetCharacterVersion( void );
	static std::string	GetItemVersion( void );
	static std::string	GetProductName( void );
	static std::string	GetName( void );
	static std::string	GetEmail( void );
	static std::string	GetProgrammers( void );
};

}

#endif // __CVERSIONCLASS_H__
