// *******************************************************************
// trigger.cpp
// *******************************************************************


#include "uox3.h"
#include "trigger.h"
#include "cScript.h"
#include "scriptc.h"
#include "ssection.h"

namespace UOX
{

CTrigger *Trigger = NULL;

CTrigger::CTrigger()
{
	ParseScript();

	envokeByID		= new CEnvoke( "object" );
	envokeByType	= new CEnvoke( "type" );

	magicScripts	= new CTriggerScripts( SCPT_MAGIC );
	commandScripts	= new CTriggerScripts( SCPT_COMMAND );
}
CTrigger::~CTrigger()
{
	delete envokeByID;
	delete envokeByType;

	delete magicScripts;
	delete commandScripts;
}

void CTrigger::Cleanup()
{//this MUST be done BEFORE deleteing CTrigger, because ~cScript uses Trigger->Whatever()
	for( std::map< UI16, cScript *>::const_iterator p = scriptTriggers.begin(); p != scriptTriggers.end(); ++p )
	{
		cScript *pScript = p->second;
		if( pScript != NULL )
		{
			pScript->Cleanup();
			delete pScript;
		}
	}

	scriptTriggers.clear();
	scriptToTriggerMapping.clear();
}

void CTrigger::ReloadJS( void )
{
	Cleanup();

	ParseScript();

	envokeByID->Parse();
	envokeByType->Parse();

	magicScripts->Parse();
	commandScripts->Parse();
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	
//|	Date			-	09/24/2002
//|	Developer(s)	-	Unknown
//|	Company/Team	-	UOX3 DevTeam
//|	Status			-	
//o--------------------------------------------------------------------------o
//|	Description		-	Parse a JSE Script
//|									
//|	Modification	-	09/24/2002	-	Avtotar - Fixed JSE path usage, to get the 
//|									path to JS file from the uox.ini file
//o--------------------------------------------------------------------------o
//|	Returns			-	NA
//o--------------------------------------------------------------------------o	
void CTrigger::ParseScript( void )
{
	std::string scpFileName = cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS ) + "jse_fileassociations.scp";
	if( !FileExists( scpFileName ) )
	{
		Console.Error( 1, "Failed to open %s", scpFileName.c_str() );
		return;
	}

	UI16 trigNum			= 0xFFFF;
	UString basePath		= cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS );
	Script *fileAssocData	= new Script( scpFileName, NUM_DEFS, false );
	if( fileAssocData != NULL )
	{
		std::map< UI16, cScript *>::iterator p;
		ScriptSection *mSection = fileAssocData->FindEntry( "SCRIPT_LIST" );
		if( mSection != NULL )
		{
			UString tag, data;
			for( tag = mSection->First(); !mSection->AtEnd(); tag = mSection->Next() )
			{
				trigNum	= tag.toUShort();
				data	= mSection->GrabData();
				p		= scriptTriggers.find( trigNum );
				if( p != scriptTriggers.end() )
					scriptTriggers.erase( p );
				UString fullpath = basePath + data;
				if( !FileExists( fullpath ) )
					Console.Error( 3, "SE mapping of %i to %s failed, file does not exist!", trigNum, data.c_str() );
				else
				{
					try
					{
						cScript *toAdd			= new cScript( fullpath, SCPT_NORMAL );
						scriptTriggers[trigNum]	= toAdd;
						if( toAdd != NULL )
							RegisterObject( toAdd->Object(), trigNum );
					}
					catch( std::runtime_error &e )
					{
						Console.Error( 2, "Compiling %s caused a construction failure (Details: %s)", fullpath.c_str(), e.what() );
					}
				}
			}
		}
		else
			Console.Warning( 2, "No JS file mappings found" );
		delete fileAssocData;
	}
}

cScript *CTrigger::GetScript( UI16 triggerNumber )
{
	std::map< UI16, cScript *>::const_iterator p = scriptTriggers.find( triggerNumber );
	if( p != scriptTriggers.end() )	//	Script was found
		return p->second;
	else	//	Account was not found
		return NULL;

}

cScript *CTrigger::GetAssociatedScript( JSObject *toFind )
{
	std::map< JSObject *, UI16 >::const_iterator tFnd = scriptToTriggerMapping.find( toFind );
	if( tFnd != scriptToTriggerMapping.end() )
	{
		std::map< UI16, cScript *>::const_iterator p = scriptTriggers.find( tFnd->second );
		if( p != scriptTriggers.end() )	//	Script was found
			return p->second;
		else	//	Script was not found
			return NULL;
	}
	else
	{
		// Check command stuff here
		cScript *tScript = NULL;

		tScript = commandScripts->GetScript( toFind );
		if( tScript != NULL )
			return tScript;

		tScript = magicScripts->GetScript( toFind );
		if( tScript != NULL )
			return tScript;
	}
	return NULL;
}

UI16 CTrigger::GetScriptNumber( JSObject *toFind )
{
	std::map< JSObject *, UI16 >::const_iterator tFnd = scriptToTriggerMapping.find( toFind );
	if( tFnd != scriptToTriggerMapping.end() )
	{
		std::map< UI16, cScript *>::const_iterator p = scriptTriggers.find( tFnd->second );
		if( p != scriptTriggers.end() )	//	Script was found
			return p->first;
	}
	return 0;
}


void CTrigger::RegisterObject( JSObject *toReg, UI16 tNum )
{
	scriptToTriggerMapping[toReg] = tNum;
}
void CTrigger::UnregisterObject( JSObject *toUnreg )
{
	std::map< JSObject *, UI16 >::iterator toFind = scriptToTriggerMapping.find( toUnreg );
	if( toFind != scriptToTriggerMapping.end() )
		scriptToTriggerMapping.erase( toFind );
}

CEnvoke * CTrigger::GetEnvokeByID( void )
{
	return envokeByID;
}

CEnvoke * CTrigger::GetEnvokeByType( void )
{
	return envokeByType;
}

CTriggerScripts * CTrigger::GetMagicScripts( void )
{
	return magicScripts;
}

CTriggerScripts * CTrigger::GetCommandScripts( void )
{
	return commandScripts;
}

CEnvoke::CEnvoke( std::string eT )
{
	envokeType = eT;
}

CEnvoke::~CEnvoke()
{
	envokeList.clear();
}

bool CEnvoke::Check( UI16 envokeID ) const
{
	std::map< UI16, UI16 >::const_iterator p = envokeList.find( envokeID );
	return ( p != envokeList.end() );
}

UI16 CEnvoke::GetScript( UI16 envokeID ) const
{
	std::map< UI16, UI16 >::const_iterator p = envokeList.find( envokeID );
	if( p != envokeList.end() )
		return p->second;
	return 0xFFFF;
}

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
					cScript *verify	= Trigger->GetScript( scriptID );
					if( verify != NULL )
						envokeList[envokeID] = scriptID;
					else
						Console.Error( 2,"(ENVOKE) Item %s refers to scriptID %u which does not exist.", tag, scriptID ); //,myendl;
				}
			}
		}
		else
			Console.Warning( 2, "Envoke section not found, no hard id->script matching being done" );
		delete fileAssocData;
	}
}

CTriggerScripts::CTriggerScripts( SCRIPTTYPE sT )
{
	scriptIterator = scriptMapping.end();
	scriptType = sT;
	Parse();
}

CTriggerScripts::~CTriggerScripts()
{
	for( std::map< JSObject *, cScript *>::const_iterator mIter = scriptMapping.begin(); mIter != scriptMapping.end(); ++mIter )
	{
		cScript *rScript = mIter->second;
		if( rScript != NULL )
		{
			rScript->Cleanup();
			delete rScript;
			rScript = NULL;
		}
	}
	scriptMapping.clear();
}

void CTriggerScripts::Parse( void )
{
	UString basePath		= cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS );
	std::string scpFileName = basePath + "jse_fileassociations.scp";
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
#ifdef _DEBUG
			Console.Print( "Preparing for script registration\n" );
#endif
			UString tag, data;
			for( tag = mSection->First(); !mSection->AtEnd(); tag = mSection->Next() )
			{
				data				= mSection->GrabData();
				UString fullpath	= basePath + data;
				if( !FileExists( fullpath ) )
					Console.Error( 3, "SE loading of %s failed, file does not exist!", data.c_str() );
				else
				{
					try
					{
#ifdef _DEBUG
						Console.Print( "Registering file %s\n", fullpath.c_str() );
#endif
						cScript *toAdd			= new cScript( fullpath, scriptType );
						if( toAdd != NULL )
							RegisterObject( toAdd->Object(), toAdd );
					}
					catch( std::runtime_error &e )
					{
						Console.Error( 2, "Compiling %s caused a construction failure (Details: %s)", fullpath.c_str(), e.what() );
					}
				}
			}
		}
		else
			Console.Warning( 2, "No JS mappings found for %s", ScriptNames[scriptType] );
		delete fileAssocData;
	}
}

void CTriggerScripts::RegisterObject( JSObject *toReg, cScript *tNum )
{
	scriptMapping[toReg] = tNum;
}

void CTriggerScripts::UnregisterObject( JSObject *toUnreg )
{
	std::map< JSObject *, cScript * >::iterator toFind = scriptMapping.find( toUnreg );
	if( toFind != scriptMapping.end() )
		scriptMapping.erase( toFind );
}

cScript * CTriggerScripts::GetScript( JSObject *toFind )
{
	std::map< JSObject *, cScript * >::const_iterator cFnd = scriptMapping.find( toFind );
	if( cFnd != scriptMapping.end() )
		return cFnd->second;
	return NULL;
}

cScript * CTriggerScripts::First( void )
{
	scriptIterator = scriptMapping.begin();
	if( scriptIterator != scriptMapping.end() )
		return scriptIterator->second;
	else
		return NULL;
}
cScript * CTriggerScripts::Next( void )
{
	if( scriptIterator != scriptMapping.end() )
	{
		++scriptIterator;
		if( scriptIterator != scriptMapping.end() )
			return scriptIterator->second;
		else
			return NULL;
	}
	else
		return NULL;
}
bool CTriggerScripts::Finished( void )
{
	return( scriptIterator == scriptMapping.end() );
}

}

