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
const std::string VER				= "0.98-1";
const std::string BUILD				= "18";
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
cVersionClass::cVersionClass()
{
}
//o---------------------------------------------------------------------------o
cVersionClass::~cVersionClass()
{

}

std::string cVersionClass::GetVersion( void )
{
	return VER;
}

std::string cVersionClass::GetBuild( void )
{
	return BUILD;
}

std::string cVersionClass::GetRealBuild( void )
{
	return REALBUILD;
}

std::string cVersionClass::GetScriptVersion( void )
{
	return SVER;
}

std::string cVersionClass::GetCharacterVersion( void )
{
	return CVER;
}

std::string cVersionClass::GetItemVersion( void )
{
	return IVER;
}

std::string cVersionClass::GetProductName( void )
{
	return PRODUCT;
}

std::string cVersionClass::GetName( void )
{
	return NAME;
}

std::string cVersionClass::GetEmail( void )
{
	return EMAIL;
}

std::string cVersionClass::GetProgrammers( void )
{
	return PROGRAMMERS;
}

}
