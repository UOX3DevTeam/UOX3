//o--------------------------------------------------------------------------o
//|	Function/Class	-	
//|	Date			-	10/17/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns			-
//o--------------------------------------------------------------------------o
//|	Notes			-	
//o--------------------------------------------------------------------------o
#include <string>
#include "cVersionClass.h"

namespace UOX
{

//o---------------------------------------------------------------------------o
// product info
//o---------------------------------------------------------------------------o
const std::string VER				= "0.98-3";
const std::string BUILD				= "3r";
const std::string REALBUILD			= "0";
const std::string SVER				= "2.0";
const std::string CVER				= "2.0";
const std::string IVER				= "2.0";
const std::string PRODUCT			= "Ultima Offline eXperiment 3";
const std::string NAME				= "The Official DevTeam";
const std::string EMAIL				= "http://uox3.sourceforge.net/";
const std::string PROGRAMMERS		= "UOX3 DevTeam";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CVersionClass::CVersionClass()
{
}
//o---------------------------------------------------------------------------o
CVersionClass::~CVersionClass()
{

}

std::string CVersionClass::GetVersion( void )
{
	return VER;
}

std::string CVersionClass::GetBuild( void )
{
	return BUILD;
}

std::string CVersionClass::GetRealBuild( void )
{
	return REALBUILD;
}

std::string CVersionClass::GetScriptVersion( void )
{
	return SVER;
}

std::string CVersionClass::GetCharacterVersion( void )
{
	return CVER;
}

std::string CVersionClass::GetItemVersion( void )
{
	return IVER;
}

std::string CVersionClass::GetProductName( void )
{
	return PRODUCT;
}

std::string CVersionClass::GetName( void )
{
	return NAME;
}

std::string CVersionClass::GetEmail( void )
{
	return EMAIL;
}

std::string CVersionClass::GetProgrammers( void )
{
	return PROGRAMMERS;
}

}
