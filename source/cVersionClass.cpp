//o-----------------------------------------------------------------------------------------------o
//|	File		-	cVersionClass.cpp
//|	Date		-	10/17/2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles version info for builds
//o-----------------------------------------------------------------------------------------------o
#include <string>
#include "cVersionClass.h"

//o-----------------------------------------------------------------------------------------------o
// product info
//o-----------------------------------------------------------------------------------------------o
const std::string VER				= "0.99";
const std::string BUILD				= "4u";
const std::string REALBUILD			= "0";
const std::string SVER				= "2.0";
const std::string CVER				= "2.0";
const std::string IVER				= "2.0";
const std::string PRODUCT			= "Ultima Offline eXperiment 3";
const std::string NAME				= "UOX3";
const std::string EMAIL				= "https://www.uox3.org/";
const std::string PROGRAMMERS		= "Countless! See UOX3 Hall of Fame in documentation!";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CVersionClass::CVersionClass()
{
}
//o-----------------------------------------------------------------------------------------------o
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