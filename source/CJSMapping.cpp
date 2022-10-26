#include "uox3.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "scriptc.h"
#include "ssection.h"
#include "StringUtility.hpp"
#include <memory>

using namespace std::string_literals;

CJSMapping *JSMapping = nullptr;
//o------------------------------------------------------------------------------------------------o
//|	File		-	CJSMapping.cpp
//|	Date		-	Feb 7, 2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	JavaScript File Mapping
//o------------------------------------------------------------------------------------------------o
//| Changes		-	Version History
//|					1.0			 		Feb 7, 2005
//|					Initial Implementation
//|
//|					1.1			 		Feb 8, 2005
//|					Added the ability to reload JS files on a per-script basis
//|
//|					1.2			 		Feb 28, 2005
//|					Added the ability to reload JS files on a per-section basis
//o------------------------------------------------------------------------------------------------o

//o------------------------------------------------------------------------------------------------o
//|	Class		-	CJSMapping
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Base global class that holds within it an array of JS Mapping Sections
//o------------------------------------------------------------------------------------------------o
/*
CJSMapping::CJSMapping()
{
}
 */
CJSMapping::~CJSMapping()
{
	//Cleanup();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::ResetDefaults()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Resets all parameters of the CJSMapping class to default
//o------------------------------------------------------------------------------------------------o
void CJSMapping::ResetDefaults( void )
{
	for( size_t i = SCPT_NORMAL; i < SCPT_COUNT; ++i )
	{
		mapSection[i] = new CJSMappingSection( static_cast<SCRIPTTYPE>( i ));
	}

	envokeById		= new CEnvoke( "object" );
	envokeByType	= new CEnvoke( "type" );

	Parse();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::Cleanup()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Frees all memory used by CJSMapping
//o------------------------------------------------------------------------------------------------o
void CJSMapping::Cleanup( void )
{
	for( size_t i = SCPT_NORMAL; i < SCPT_COUNT; ++i )
	{
		if( mapSection[1] != nullptr )
		{
			delete mapSection[i];
			mapSection[i] = nullptr;
		}
	}

	if( envokeById != nullptr )
	{
		delete envokeById;
		envokeById = nullptr;
	}
	if( envokeByType != nullptr )
	{
		delete envokeByType;
		envokeByType = nullptr;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::Reload()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reloads the JS Scripts
//o------------------------------------------------------------------------------------------------o
void CJSMapping::Reload( UI16 scriptId )
{
	if( scriptId != 0xFFFF )
	{
		Console.Print( oldstrutil::format( "CMD: Attempting Reload of JavaScript (ScriptId %u)\n", scriptId ));
		for( size_t i = SCPT_NORMAL; i < SCPT_COUNT; ++i )
		{
			if( mapSection[i]->IsInMap( scriptId ))
			{
				mapSection[i]->Reload( scriptId );
				return;
			}
		}
		Console.Warning( oldstrutil::format( "Unable to locate specified JavaScript in the map (ScriptId %u)", scriptId ));
	}
	else
	{
		Console.Print( oldstrutil::format( "CMD: Loading JSE Scripts... \n" ));
		Cleanup();
		ResetDefaults();
		envokeById->Parse();
		envokeByType->Parse();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::Reload()
//|	Date		-	2/28/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reloads a specific section of the JS Scripts
//o------------------------------------------------------------------------------------------------o
void CJSMapping::Reload( SCRIPTTYPE sectionId )
{
	Console.Print( oldstrutil::format( "CMD: Attempting Reload of JavaScript (SectionId %u)\n", static_cast<SI32>( sectionId )));
	if( mapSection[sectionId] != nullptr )
	{
		delete mapSection[sectionId];
		mapSection[sectionId] =  new CJSMappingSection( sectionId );

		Parse( sectionId );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::Parse()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Parses through jse_fileassociations.scp doling out the work
//|						to each CJSMappingSection Parse() routine.
//o------------------------------------------------------------------------------------------------o
void CJSMapping::Parse( SCRIPTTYPE toParse )
{
	Console.Print( "Loading JS Scripts\n" );

	std::string scpFileName = cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS ) + "jse_fileassociations.scp";
	if( !FileExists( scpFileName ))
	{
		Console.Error( oldstrutil::format( "Failed to open %s", scpFileName.c_str() ));
		return;
	}

	Script *fileAssocData = new Script( scpFileName, NUM_DEFS, false );
	if( fileAssocData != nullptr )
	{
		if( toParse != SCPT_COUNT )
		{
			if( mapSection[toParse] != nullptr )
			{
				mapSection[toParse]->Parse( fileAssocData );
			}
		}
		else
		{
			for( size_t i = SCPT_NORMAL; i < SCPT_COUNT; ++i )
			{
				if( mapSection[i] != nullptr )
				{
					mapSection[i]->Parse( fileAssocData );
				}
			}
		}

		delete fileAssocData;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::GetSection()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a pointer to the specified JSMappingSection
//o------------------------------------------------------------------------------------------------o
CJSMappingSection * CJSMapping::GetSection( SCRIPTTYPE toGet )
{
	if( mapSection[toGet] != nullptr )
		return mapSection[toGet];

	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::GetScriptId()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the scriptId relating to the specified JSObject
//o------------------------------------------------------------------------------------------------o
UI16 CJSMapping::GetScriptId( JSObject *toFind )
{
	UI16 retVal	= 0xFFFF;

	for( size_t i = SCPT_NORMAL; i < SCPT_COUNT; ++i )
	{
		retVal = mapSection[i]->GetScriptId( toFind );
		if( retVal != 0xFFFF )
			break;
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::GetScript()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the cScript relating to the specified JSObject
//o------------------------------------------------------------------------------------------------o
cScript * CJSMapping::GetScript( JSObject *toFind )
{
	cScript *retVal		= nullptr;
	cScript *toCheck	= nullptr;

	for( size_t i = SCPT_NORMAL; i < SCPT_COUNT; ++i )
	{
		toCheck = mapSection[i]->GetScript( toFind );
		if( toCheck != nullptr )
		{
			retVal = toCheck;
			break;
		}
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::GetScript()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the cScript relating to the specified scriptId
//o------------------------------------------------------------------------------------------------o
cScript * CJSMapping::GetScript( UI16 toFind )
{
	cScript *retVal		= nullptr;
	cScript *toCheck	= nullptr;

	for( size_t i = SCPT_NORMAL; i < SCPT_COUNT; ++i )
	{
		toCheck = mapSection[i]->GetScript( toFind );
		if( toCheck != nullptr )
		{
			retVal = toCheck;
			break;
		}
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::GetEnvokeById()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a pointer to the CEnvoke class handling EnvokeByID
//o------------------------------------------------------------------------------------------------o
CEnvoke * CJSMapping::GetEnvokeById( void )
{
	return envokeById;
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMapping::GetEnvokeByType()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a pointer to the CEnvoke class handling EnvokeByType
//o------------------------------------------------------------------------------------------------o
CEnvoke * CJSMapping::GetEnvokeByType( void )
{
	return envokeByType;
}

//o------------------------------------------------------------------------------------------------o
//|	Class		-	CJSMappingSection::CJSMappingSection()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Class containing the specified section of jse_fileassociations.scp
//o------------------------------------------------------------------------------------------------o
CJSMappingSection::CJSMappingSection( SCRIPTTYPE sT )
{
	scriptType = sT;

	scriptIdMap.clear();
	scriptJSMap.clear();

	scriptIdIter = scriptIdMap.end();
}
CJSMappingSection::~CJSMappingSection()
{

	for( std::map<UI16, cScript *>::const_iterator sIter = scriptIdMap.begin(); sIter != scriptIdMap.end(); ++sIter )
	{
		cScript *toDelete = sIter->second;
		if( toDelete != nullptr )
		{
			delete toDelete;
		}
	}

	scriptIdMap.clear();
	scriptJSMap.clear();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::Parse()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Parses through specified section of jse_fileassociations.scp
//|						populating the map with each entry.
//o------------------------------------------------------------------------------------------------o
auto CJSMappingSection::Parse( Script *fileAssocData ) -> void
{
	auto basePath	= cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS );
	auto mSection	= fileAssocData->FindEntry( ScriptNames[scriptType] );
	UI08 runTime	= 0;

	if( scriptType == SCPT_CONSOLE )
	{
		runTime = 1;
	}

	if( mSection )
	{
		UI16 scriptId = 0xFFFF;
		size_t i = 0;
		for( const auto &sec :mSection->collection() )
		{
			auto tag = sec->tag;
			auto data = sec->data;

			scriptId		= static_cast<UI16>( std::stoul( tag, nullptr, 0 ));
			auto fullPath	= basePath + data;

			if( !FileExists( fullPath ))
			{
				Console.Error( oldstrutil::format( "SE mapping of %i to %s failed, file does not exist!", scriptId, data.c_str() ));
			}
			else
			{
				try
				{
					auto toAdd = new cScript( fullPath, runTime );
					if( toAdd )
					{
						scriptIdMap[scriptId]			= toAdd;
						scriptJSMap[toAdd->Object()]	= scriptId;
						++i;
					}
				}
				catch( std::runtime_error &e )
				{
					Console.Error( oldstrutil::format( "Compiling %s caused a construction failure (Details: %s)", fullPath.c_str(), e.what() ));
				}
			}
		}
		Console.Print( "  o Loaded " );
		Console.TurnYellow();
		Console.Print( oldstrutil::format( "%4u ", i ));
		Console.TurnNormal();
		Console.Print( "scripts from section " );
		Console.TurnYellow();
		Console.Print( oldstrutil::format( "%s\n", ScriptNames[scriptType].c_str() ));
		Console.TurnNormal();
	}
	else
	{
		Console.Warning( oldstrutil::format( "No JS file mappings found in section %s", ScriptNames[scriptType].c_str() ));
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::Reload()
//|	Date		-	2/8/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reloads the specified JS file (by its scriptId)
//o------------------------------------------------------------------------------------------------o
auto CJSMappingSection::Reload( UI16 toLoad ) -> void
{
	auto scpFileName = cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS ) + "jse_fileassociations.scp"s;
	if( !FileExists( scpFileName ))
	{
		Console.Error( oldstrutil::format( "Failed to open %s", scpFileName.c_str() ));
		return;
	}

	auto fileAssocData = std::make_unique<Script>( scpFileName, NUM_DEFS, false );
	if( fileAssocData )
	{
		auto mSection = fileAssocData->FindEntry( ScriptNames[scriptType] );
		if( mSection )
		{
			UI16 scriptId = 0xFFFF;
			auto basePath = cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS );
			std::string data, fullPath;
			UI08 runTime = 0;
			if( scriptType == SCPT_CONSOLE )
			{
				runTime = 1;
			}
			for( const auto &sec : mSection->collection() )
			{
				auto tag = sec->tag;
				scriptId		= static_cast<UI16>( std::stoul( tag, nullptr, 0 ));
				if( scriptId == toLoad )
				{
					data		= sec->data;
					fullPath	= basePath + data;

					if( !FileExists( fullPath ))
					{
						Console.Error( oldstrutil::format( "SE mapping of %i to %s failed, file does not exist!", scriptId, data.c_str() ));
					}
					else
					{
						try
						{
							std::map<UI16, cScript *>::const_iterator iFind = scriptIdMap.find( toLoad );
							if( iFind != scriptIdMap.end() )
							{
								if( scriptIdMap[toLoad] )
								{
									JSObject *jsObj = scriptIdMap[toLoad]->Object();
									std::map<JSObject *, UI16>::iterator jFind = scriptJSMap.find( jsObj );
									if( jFind != scriptJSMap.end() )
									{
										scriptJSMap.erase( jFind );
									}

									delete scriptIdMap[toLoad];
									scriptIdMap[toLoad] = nullptr;
								}
							}
							auto toAdd = new cScript( fullPath, runTime );
							if( toAdd )
							{
								scriptIdMap[scriptId]			= toAdd;
								scriptJSMap[toAdd->Object()]	= scriptId;
								Console.Print( oldstrutil::format( "Reload of JavaScript (ScriptId %u) Successful\n", toLoad ));
							}
						}
						catch( std::runtime_error &e )
						{
							Console.Error( oldstrutil::format( "Compiling %s caused a construction failure (Details: %s)", fullPath.c_str(), e.what() ));
						}
					}
					return;
				}
			}
			Console.Warning( oldstrutil::format( "Unable to locate the specified JavaScript in the file (ScriptId %u)", toLoad ));
		}
		else
		{
			Console.Warning( oldstrutil::format( "No JS file mappings found in section %s", ScriptNames[scriptType].c_str() ));
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::IsInMap()
//|	Date		-	2/8/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if the specified scriptId exists in the map
//o------------------------------------------------------------------------------------------------o
bool CJSMappingSection::IsInMap( UI16 scriptId )
{
	bool retVal = false;

	std::map<UI16, cScript *>::const_iterator sIter = scriptIdMap.find( scriptId );
	if( sIter != scriptIdMap.end() )
	{
		retVal = true;
	}

	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::GetScriptId()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the scriptId associated with the specified JSObject
//o------------------------------------------------------------------------------------------------o
UI16 CJSMappingSection::GetScriptId( JSObject *toFind )
{
	UI16 retVal = 0xFFFF;

	std::map<JSObject *, UI16>::const_iterator sIter = scriptJSMap.find( toFind );
	if( sIter != scriptJSMap.end() )
	{
		retVal = sIter->second;
	}

	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::GetScript()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the cScript associated with the specified scriptId
//o------------------------------------------------------------------------------------------------o
cScript * CJSMappingSection::GetScript( UI16 toFind )
{
	cScript *retVal = nullptr;

	std::map<UI16, cScript *>::const_iterator idIter = scriptIdMap.find( toFind );
	if( idIter != scriptIdMap.end() )
	{
		retVal = idIter->second;
	}

	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::GetScript()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds the cScript associated with the specified JSObject
//o------------------------------------------------------------------------------------------------o
cScript * CJSMappingSection::GetScript( JSObject *toFind )
{
	UI16 scriptId = GetScriptId( toFind );
	return GetScript( scriptId );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::First()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the first cScript in the scriptIdMap
//o------------------------------------------------------------------------------------------------o
cScript * CJSMappingSection::First( void )
{
	scriptIdIter = scriptIdMap.begin();
	if( !Finished() )
		return scriptIdIter->second;

	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::Next()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the next cScript in the scriptIdMap
//o------------------------------------------------------------------------------------------------o
cScript * CJSMappingSection::Next( void )
{
	if( !Finished() )
	{
		++scriptIdIter;
		if( !Finished() )
			return scriptIdIter->second;
	}
	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJSMappingSection::Finished()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Tells us when we have reached the end of the scriptIdMap
//o------------------------------------------------------------------------------------------------o
bool CJSMappingSection::Finished( void )
{
	return ( scriptIdIter == scriptIdMap.end() );
}

//o------------------------------------------------------------------------------------------------o
//|	Class		-	CEnvoke::CEnvoke()
//|	Date		-	2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Class containing the specified envoke file
//o------------------------------------------------------------------------------------------------o
CEnvoke::CEnvoke( const std::string &eT )
{
	envokeType = eT;
	envokeList.clear();
}
CEnvoke::~CEnvoke()
{
	envokeList.clear();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CEnvoke::Check()
//|	Date		-	2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if specified envokeId is in the map
//o------------------------------------------------------------------------------------------------o
bool CEnvoke::Check( UI16 envokeId ) const
{
	std::map<UI16, UI16>::const_iterator p = envokeList.find( envokeId );
	return ( p != envokeList.end() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CEnvoke::GetScript()
//|	Date		-	2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns specified envokeId
//o------------------------------------------------------------------------------------------------o
UI16 CEnvoke::GetScript( UI16 envokeId ) const
{
	std::map<UI16, UI16>::const_iterator p = envokeList.find( envokeId );
	if( p != envokeList.end() )
		return p->second;

	return 0xFFFF;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CEnvoke::Parse()
//|	Date		-	2/7/2005
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Parses through specific envoke file mapping the scriptId to the envoke type
//o------------------------------------------------------------------------------------------------o
auto CEnvoke::Parse() -> void
{
	envokeList.clear();

	auto filename = cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS ) + "jse_" + envokeType + "associations.scp";
	if( !FileExists( filename ))
	{
		Console << "Unable to open " << filename << " for parsing" << myendl;
		return;
	}

	auto fileAssocData	= new Script( filename, NUM_DEFS, false );
	if( fileAssocData )
	{
		auto mSection = fileAssocData->FindEntry( "ENVOKE" );
		if( mSection )
		{
			for( const auto &sec:mSection->collection() )
			{
				auto tag = sec->tag;
				if( !tag.empty() && tag != "\n" && tag != "\r" )
				{
					auto data = sec->data;
					auto envokeId	= static_cast<UI16>( std::stoul( tag, nullptr, 0 ));
					auto scriptId	= static_cast<UI16>( std::stoul( data, nullptr, 0 ));
					auto verify	= JSMapping->GetScript( scriptId );
					if( verify )
					{
						envokeList[envokeId] = scriptId;
					}
					else
					{
						Console.Error( oldstrutil::format( "(ENVOKE) Item %s refers to scriptId %u which does not exist.", tag.c_str(), scriptId ));
					}
				}
			}
		}
		else
		{
			Console.Warning( "Envoke section not found, no hard id->script matching being done" );
		}
		delete fileAssocData;
	}
}
