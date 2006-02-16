#include "uox3.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "scriptc.h"
#include "ssection.h"

namespace UOX
{

CJSMapping *JSMapping = NULL;
//o--------------------------------------------------------------------------o
//|	File			-	CJSMapping.cpp
//|	Date			-	Feb 7, 2005
//|	Developers		-	giwo / EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	JavaScript File Mapping
//o--------------------------------------------------------------------------o
//| Modifications	-	Version History
//|						1.0			giwo		Feb 7, 2005
//|						Initial Implementation
//|
//|						1.1			giwo		Feb 8, 2005
//|						Added the ability to reload JS files on a per-script basis
//|
//|						1.2			giwo		Feb 28, 2005
//|						Added the ability to reload JS files on a per-section basis
//o--------------------------------------------------------------------------o

//o--------------------------------------------------------------------------o
//|	Class			-	CJSMapping
//|	Date			-	2/7/2005
//|	Developers		-	giwo / EviLDeD
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Base global class that holds within it an array of JS Mapping Sections
//o--------------------------------------------------------------------------o
CJSMapping::CJSMapping()
{
	Console.Print( "Loading JS Scripts\n" );
	ResetDefaults();
}
CJSMapping::~CJSMapping()
{
	Cleanup();
}

//o--------------------------------------------------------------------------o
//|	Function		-	CJSMapping::ResetDefaults()
//|	Date			-	2/7/2005
//|	Developers		-	giwo / EviLDeD
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Resets all parameters of the CJSMapping class to default
//o--------------------------------------------------------------------------o
void CJSMapping::ResetDefaults( void )
{
	for( size_t i = SCPT_NORMAL; i < SCPT_COUNT; ++i )
	{
		mapSection[i] = new CJSMappingSection( static_cast<SCRIPTTYPE>(i) );
	}

	envokeByID		= new CEnvoke( "object" );
	envokeByType	= new CEnvoke( "type" );

	Parse();
}

//o--------------------------------------------------------------------------o
//|	Function		-	CJSMapping::Cleanup()
//|	Date			-	2/7/2005
//|	Developers		-	giwo / EviLDeD
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Frees all memory used by CJSMapping
//o--------------------------------------------------------------------------o
void CJSMapping::Cleanup( void )
{
	for( size_t i = SCPT_NORMAL; i < SCPT_COUNT; ++i )
	{
		if( mapSection[1] != NULL )
		{
			delete mapSection[i];
			mapSection[i] = NULL;
		}
	}

	if( envokeByID != NULL )
	{
		delete envokeByID;
		envokeByID = NULL;
	}
	if( envokeByType != NULL )
	{
		delete envokeByType;
		envokeByType = NULL;
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	CJSMapping::Reload( UI16 scriptID )
//|	Date			-	2/7/2005
//|	Developers		-	giwo / EviLDeD
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Reloads the JS Scripts
//o--------------------------------------------------------------------------o
void CJSMapping::Reload( UI16 scriptID )
{
	if( scriptID != 0xFFFF )
	{
		Console.Print( "CMD: Attempting Reload of JavaScript (ScriptID %u)\n", scriptID );
		for( size_t i = SCPT_NORMAL; i < SCPT_COUNT; ++i )
		{
			if( mapSection[i]->IsInMap( scriptID ) )
			{
				mapSection[i]->Reload( scriptID );
				return;
			}
		}
		Console.Warning( 2, "Unable to locate specified JavaScript in the map (ScriptID %u)", scriptID );
	}
	else
	{
		Console.Print( "CMD: Loading JSE Scripts... \n" );
		Cleanup();
		ResetDefaults();
		envokeByID->Parse();
		envokeByType->Parse();
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	CJSMapping::Reload( SCRIPTTYPE sectionID )
//|	Date			-	2/28/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Reloads a specific section of the JS Scripts
//o--------------------------------------------------------------------------o
void CJSMapping::Reload( SCRIPTTYPE sectionID )
{
	Console.Print( "CMD: Attempting Reload of JavaScript (SectionID %u)\n", static_cast<int>(sectionID) );
	if( mapSection[sectionID] != NULL )
	{
		delete mapSection[sectionID];
		mapSection[sectionID] =  new CJSMappingSection( sectionID );

		Parse( sectionID );
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	CJSMapping::Parse()
//|	Date			-	2/7/2005
//|	Developers		-	giwo / EviLDeD
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Parses through jse_fileassociations.scp doling out the work
//|						to each CJSMappingSection Parse() routine.
//o--------------------------------------------------------------------------o
void CJSMapping::Parse( SCRIPTTYPE toParse )
{
	std::string scpFileName = cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS ) + "jse_fileassociations.scp";
	if( !FileExists( scpFileName ) )
	{
		Console.Error( 1, "Failed to open %s", scpFileName.c_str() );
		return;
	}

	Script *fileAssocData	= new Script( scpFileName, NUM_DEFS, false );
	if( fileAssocData != NULL )
	{
		if( toParse != SCPT_COUNT )
		{
			if( mapSection[toParse] != NULL )
				mapSection[toParse]->Parse( fileAssocData );
		}
		else
		{
			for( size_t i = SCPT_NORMAL; i < SCPT_COUNT; ++i )
			{
				if( mapSection[i] != NULL )
					mapSection[i]->Parse( fileAssocData );
			}
		}

		delete fileAssocData;
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	CJSMappingSection * CJSMapping::GetSection( SCRIPTTYPE toGet )
//|	Date			-	2/7/2005
//|	Developers		-	giwo / EviLDeD
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Returns a pointer to the specified JSMappingSection
//o--------------------------------------------------------------------------o
CJSMappingSection * CJSMapping::GetSection( SCRIPTTYPE toGet )
{
	if( mapSection[toGet] != NULL )
		return mapSection[toGet];

	return NULL;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI16 CJSMapping::GetScriptID( JSObject *toFind )
//|	Date			-	2/7/2005
//|	Developers		-	giwo / EviLDeD
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Returns the scriptID relating to the specified JSObject
//o--------------------------------------------------------------------------o
UI16 CJSMapping::GetScriptID( JSObject *toFind )
{
	UI16 retVal		= 0xFFFF;

	for( size_t i = SCPT_NORMAL; i < SCPT_COUNT; ++i )
	{
		retVal = mapSection[i]->GetScriptID( toFind );
		if( retVal != 0xFFFF )
			break;
	}
	return retVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	cScript * CJSMapping::GetScript( JSObject *toFind )
//|	Date			-	2/7/2005
//|	Developers		-	giwo / EviLDeD
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Returns the cScript relating to the specified JSObject
//o--------------------------------------------------------------------------o
cScript * CJSMapping::GetScript( JSObject *toFind )
{
	cScript *retVal		= NULL;
	cScript *toCheck	= NULL;

	for( size_t i = SCPT_NORMAL; i < SCPT_COUNT; ++i )
	{
		toCheck = mapSection[i]->GetScript( toFind );
		if( toCheck != NULL )
		{
			retVal = toCheck;
			break;
		}
	}
	return retVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	cScript * CJSMapping::GetScript( UI16 toFind )
//|	Date			-	2/7/2005
//|	Developers		-	giwo / EviLDeD
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Returns the cScript relating to the specified scriptID
//o--------------------------------------------------------------------------o
cScript * CJSMapping::GetScript( UI16 toFind )
{
	cScript *retVal		= NULL;
	cScript *toCheck	= NULL;

	for( size_t i = SCPT_NORMAL; i < SCPT_COUNT; ++i )
	{
		toCheck = mapSection[i]->GetScript( toFind );
		if( toCheck != NULL )
		{
			retVal = toCheck;
			break;
		}
	}
	return retVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CEnvoke * CJSMapping::GetEnvokeByID()
//|	Date			-	2/7/2005
//|	Developers		-	giwo / EviLDeD
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Returns a pointer to the CEnvoke class handling EnvokeByID
//o--------------------------------------------------------------------------o
CEnvoke * CJSMapping::GetEnvokeByID( void )
{
	return envokeByID;
}
//o--------------------------------------------------------------------------o
//|	Function		-	CEnvoke * CJSMapping::GetEnvokeByType()
//|	Date			-	2/7/2005
//|	Developers		-	giwo / EviLDeD
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Returns a pointer to the CEnvoke class handling EnvokeByType
//o--------------------------------------------------------------------------o
CEnvoke * CJSMapping::GetEnvokeByType( void )
{
	return envokeByType;
}

//o--------------------------------------------------------------------------o
//|	Class			-	CJSMappingSection
//|	Date			-	2/7/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Class containing the specified section of jse_fileassociations.scp
//o--------------------------------------------------------------------------o
CJSMappingSection::CJSMappingSection( SCRIPTTYPE sT )
{
	scriptType = sT;

	scriptIDMap.clear();
	scriptJSMap.clear();

	scriptIDIter = scriptIDMap.end();
}
CJSMappingSection::~CJSMappingSection()
{
	
	for( std::map< UI16, cScript * >::const_iterator sIter = scriptIDMap.begin(); sIter != scriptIDMap.end(); ++sIter )
	{
		cScript *toDelete = sIter->second;
		if( toDelete != NULL )
		{
			delete toDelete;
		}
	}

	scriptIDMap.clear();
	scriptJSMap.clear();
}

//o--------------------------------------------------------------------------o
//|	Function		-	CJSMappingSection::Parse( Script *fileAssocData )
//|	Date			-	2/7/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Parses through specified section of jse_fileassociations.scp
//|						populating the map with each entry.
//o--------------------------------------------------------------------------o
void CJSMappingSection::Parse( Script *fileAssocData )
{
	UI16 scriptID			= 0xFFFF;
	UString basePath		= cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS );
	ScriptSection *mSection = fileAssocData->FindEntry( ScriptNames[scriptType] );
	UI08 runTime			= 0;

	if( scriptType == SCPT_CONSOLE )
		runTime = 1;
	
	if( mSection != NULL )
	{
		size_t i = 0;
		UString data, fullPath;
		for( UString tag = mSection->First(); !mSection->AtEnd(); tag = mSection->Next() )
		{
			scriptID	= tag.toUShort();
			data		= mSection->GrabData();
			fullPath	= basePath + data;

			if( !FileExists( fullPath ) )
				Console.Error( 3, "SE mapping of %i to %s failed, file does not exist!", scriptID, data.c_str() );
			else
			{
				try
				{
					cScript *toAdd = new cScript( fullPath, runTime );
					if( toAdd != NULL )
					{
						scriptIDMap[scriptID]			= toAdd;
						scriptJSMap[toAdd->Object()]	= scriptID;
						++i;
					}
				}
				catch( std::runtime_error &e )
				{
					Console.Error( 2, "Compiling %s caused a construction failure (Details: %s)", fullPath.c_str(), e.what() );
				}
			}
		}
		Console.Print( "  o Loaded " );
		Console.TurnYellow();
		Console.Print( "%4u ", i );
		Console.TurnNormal();
		Console.Print( "scripts from section " );
		Console.TurnYellow();
		Console.Print( "%s\n", ScriptNames[scriptType].c_str() );
		Console.TurnNormal();
	}
	else
		Console.Warning( 2, "No JS file mappings found in section %s", ScriptNames[scriptType].c_str() );
}

//o--------------------------------------------------------------------------o
//|	Function		-	CJSMappingSection::Reload( UI16 toLoad )
//|	Date			-	2/8/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Reloads the specified JS file (by its scriptID)
//o--------------------------------------------------------------------------o
void CJSMappingSection::Reload( UI16 toLoad )
{
	std::string scpFileName = cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS ) + "jse_fileassociations.scp";
	if( !FileExists( scpFileName ) )
	{
		Console.Error( 1, "Failed to open %s", scpFileName.c_str() );
		return;
	}

	Script *fileAssocData	= new Script( scpFileName, NUM_DEFS, false );
	if( fileAssocData != NULL )
	{
		ScriptSection *mSection = fileAssocData->FindEntry( ScriptNames[scriptType] );
		if( mSection != NULL )
		{
			UI16 scriptID		= 0xFFFF;
			UString basePath	= cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS );
			UString data, fullPath;
			UI08 runTime = 0;
			if( scriptType == SCPT_CONSOLE )
				runTime = 1;
			for( UString tag = mSection->First(); !mSection->AtEnd(); tag = mSection->Next() )
			{
				scriptID		= tag.toUShort();
				if( scriptID == toLoad )
				{
					data		= mSection->GrabData();
					fullPath	= basePath + data;

					if( !FileExists( fullPath ) )
						Console.Error( 3, "SE mapping of %i to %s failed, file does not exist!", scriptID, data.c_str() );
					else
					{
						try
						{
							std::map< UI16, cScript * >::const_iterator iFind = scriptIDMap.find( toLoad );
							if( iFind != scriptIDMap.end() )
							{
								if( scriptIDMap[toLoad] != NULL )
								{
									JSObject *jsObj = scriptIDMap[toLoad]->Object();
									std::map< JSObject *, UI16 >::iterator jFind = scriptJSMap.find( jsObj );
									if( jFind != scriptJSMap.end() )
										scriptJSMap.erase( jFind );

									delete scriptIDMap[toLoad];
									scriptIDMap[toLoad] = NULL;
								}
							}
							cScript *toAdd = new cScript( fullPath, runTime );
							if( toAdd != NULL )
							{
								scriptIDMap[scriptID]			= toAdd;
								scriptJSMap[toAdd->Object()]	= scriptID;
								Console.Print( "Reload of JavaScript (ScriptID %u) Successful\n", toLoad );
							}
						}
						catch( std::runtime_error &e )
						{
							Console.Error( 2, "Compiling %s caused a construction failure (Details: %s)", fullPath.c_str(), e.what() );
						}
					}
					delete fileAssocData;
					return;
				}
			}
			Console.Warning( 2, "Unable to locate the specified JavaScript in the file (ScriptID %u)", toLoad );
		}
		else
			Console.Warning( 2, "No JS file mappings found in section %s", ScriptNames[scriptType].c_str() );
		delete fileAssocData;
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool CJSMappingSection::IsInMap( UI16 scriptID )
//|	Date			-	2/8/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Checks if the specified scriptID exists in the map
//o--------------------------------------------------------------------------o
bool CJSMappingSection::IsInMap( UI16 scriptID )
{
	bool retVal = false;

	std::map< UI16, cScript * >::const_iterator sIter = scriptIDMap.find( scriptID );
	if( sIter != scriptIDMap.end() )
		retVal = true;

	return retVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI16 CJSMappingSection::GetScriptID( JSObject *toFind )
//|	Date			-	2/7/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Finds the scriptID associated with the specified JSObject
//o--------------------------------------------------------------------------o
UI16 CJSMappingSection::GetScriptID( JSObject *toFind )
{
	UI16 retVal = 0xFFFF;

	std::map< JSObject *, UI16 >::const_iterator sIter = scriptJSMap.find( toFind );
	if( sIter != scriptJSMap.end() )
		retVal = sIter->second;

	return retVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	cScript * CJSMappingSection::GetScript( UI16 toFind )
//|	Date			-	2/7/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Finds the cScript associated with the specified scriptID
//o--------------------------------------------------------------------------o
cScript * CJSMappingSection::GetScript( UI16 toFind )
{
	cScript *retVal = NULL;

	std::map< UI16, cScript * >::const_iterator idIter = scriptIDMap.find( toFind );
	if( idIter != scriptIDMap.end() )
		retVal = idIter->second;

	return retVal;
}

//o--------------------------------------------------------------------------o
//|	Function		-	cScript * CJSMappingSection::GetScript( JSObject *toFind )
//|	Date			-	2/7/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Finds the cScript associated with the specified JSObject
//o--------------------------------------------------------------------------o
cScript * CJSMappingSection::GetScript( JSObject *toFind )
{
	UI16 scriptID = GetScriptID( toFind );
	return GetScript( scriptID );
}

//o--------------------------------------------------------------------------o
//|	Function		-	cScript * CJSMappingSection::First()
//|	Date			-	2/7/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Returns the first cScript in the scriptIDMap
//o--------------------------------------------------------------------------o
cScript * CJSMappingSection::First( void )
{
	scriptIDIter = scriptIDMap.begin();
	if( !Finished() )
		return scriptIDIter->second;

	return NULL;
}

//o--------------------------------------------------------------------------o
//|	Function		-	cScript * CJSMappingSection::Next()
//|	Date			-	2/7/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Returns the next cScript in the scriptIDMap
//o--------------------------------------------------------------------------o
cScript * CJSMappingSection::Next( void )
{
	if( !Finished() )
	{
		++scriptIDIter;
		if( !Finished() )
			return scriptIDIter->second;
	}
	return NULL;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool CJSMappingSection::Finished()
//|	Date			-	2/7/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Tells us when we have reached the end of the scriptIDMap
//o--------------------------------------------------------------------------o
bool CJSMappingSection::Finished( void )
{
	return (scriptIDIter == scriptIDMap.end());
}

//o--------------------------------------------------------------------------o
//|	Class			-	CEnvoke
//|	Date			-	2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Class containing the specified envoke file
//o--------------------------------------------------------------------------o
CEnvoke::CEnvoke( std::string eT )
{
	envokeType = eT;
	envokeList.clear();
}
CEnvoke::~CEnvoke()
{
	envokeList.clear();
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool CEnvoke::Check( UI16 envokeID ) const
//|	Date			-	2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Checks if specified envokeID is in the map
//o--------------------------------------------------------------------------o
bool CEnvoke::Check( UI16 envokeID ) const
{
	std::map< UI16, UI16 >::const_iterator p = envokeList.find( envokeID );
	return ( p != envokeList.end() );
}

//o--------------------------------------------------------------------------o
//|	Function		-	UI16 CEnvoke::GetScript( UI16 envokeID ) const
//|	Date			-	2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Returns specified envokeID
//o--------------------------------------------------------------------------o
UI16 CEnvoke::GetScript( UI16 envokeID ) const
{
	std::map< UI16, UI16 >::const_iterator p = envokeList.find( envokeID );
	if( p != envokeList.end() )
		return p->second;
	return 0xFFFF;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CEnvoke::Parse()
//|	Date			-	2/7/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Parses through specific envoke file mapping the scriptID to the envoke type
//o--------------------------------------------------------------------------o
void CEnvoke::Parse( void )
{
	envokeList.clear();

	std::ifstream envokefile;
	std::string filename = cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS ) + "jse_" + envokeType + "associations.scp";
	if( !FileExists( filename ) )
	{
		Console << "Unable to open " << filename << " for parsing" << myendl;
		return;
	}

	Script *fileAssocData	= new Script( filename, NUM_DEFS, false );
	if( fileAssocData != NULL )
	{
		ScriptSection *mSection = fileAssocData->FindEntry( "ENVOKE" );
		if( mSection != NULL )
		{
			UString tag, data;
			for( tag = mSection->First(); !mSection->AtEnd(); tag = mSection->Next() )
			{
				if( !tag.empty() && tag != "\n" && tag != "\r" )
				{
					data			= mSection->GrabData();
					UI16 envokeID	= tag.toUShort();
					UI16 scriptID	= data.toUShort();
					cScript *verify	= JSMapping->GetScript( scriptID );
					if( verify != NULL )
						envokeList[envokeID] = scriptID;
					else
						Console.Error( 2, "(ENVOKE) Item %s refers to scriptID %u which does not exist.", tag.c_str(), scriptID );
				}
			}
		}
		else
			Console.Warning( 2, "Envoke section not found, no hard id->script matching being done" );
		delete fileAssocData;
	}
}

}
