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

Triggers *Trigger = NULL;

Triggers::Triggers()
{
	commandScpIterator	= commandScripts.end();
	magicScpIterator	= magicScripts.end();
	
	ParseScript();
	ParseEnvoke();	// do envoke second, so script builds the script list
	ParseCommands();
	ParseMagic();
}
Triggers::~Triggers()
{
	scriptTriggers.clear();
	commandScripts.clear();
	envokeList.clear();
	magicScripts.clear();
}

void Triggers::Cleanup()
{//this MUST be done BEFORE deleteing Triggers, because ~cScript uses Triggers->Whatever()
	std::map< JSObject *, cScript *>::iterator mIter;
	std::vector< cScript * > toDestroy;
	for( mIter = commandScripts.begin(); mIter != commandScripts.end(); ++mIter )
	{
		cScript *qScript = mIter->second;
		if( qScript != NULL )
		{
			toDestroy.push_back( qScript );
		}
	}
	for( mIter = magicScripts.begin(); mIter != magicScripts.end(); ++mIter )
	{
		cScript *rScript = mIter->second;
		if( rScript != NULL )
		{
			toDestroy.push_back( rScript );
		}
	}
	for( size_t i = 0; i < toDestroy.size(); ++i )
	{
		toDestroy[i]->Cleanup();
		delete toDestroy[i];
	}
	toDestroy.resize( 0 );
	magicScripts.clear();
	commandScripts.clear();
	for( std::map< UI16, cScript *>::const_iterator p = scriptTriggers.begin(); p != scriptTriggers.end(); ++p )
	{
		cScript *pScript = p->second;
		if( pScript != NULL )
		{
			pScript->Cleanup();
			delete pScript;
		}
	}
}

bool Triggers::CheckEnvoke( UI16 itemID ) const
{
	std::map< UI16, UI16 >::const_iterator p = envokeList.find( itemID );
	return ( p != envokeList.end() );
}

UI16 Triggers::GetScriptFromEnvoke( UI16 itemID ) const
{
	std::map< UI16, UI16 >::const_iterator p = envokeList.find( itemID );
	if( p != envokeList.end() )
		return p->second;
	return 0xFFFF;
}

void Triggers::ParseEnvoke( void )
{
	std::ifstream envokefile;
	std::string filename = cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS ) + "jse_objectassociations.scp";
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
					UI16 itemID		= tag.toUShort();
					UI16 scriptID	= data.toUShort();
					cScript *verify	= GetScript( scriptID );
					if( verify != NULL )
						envokeList[itemID] = scriptID;
					else
						Console.Error( 2,"(ENVOKE) itemID 0x%x refers for scriptID 0x%x which does not exist.", itemID, scriptID ); //,myendl;
				}
			}
		}
		else
			Console.Warning( 2, "Envoke section not found, no hard id->script matching being done" );
		delete fileAssocData;
	}
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
void Triggers::ParseScript( void )
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
				{	// doesn't exist
					Console.Error( 3, "SE mapping of %i to %s failed, file does not exist!", trigNum, data.c_str() );
				}
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

void Triggers::ParseCommands( void )
{
	std::string scpFileName = cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS ) + "jse_fileassociations.scp";
	if( !FileExists( scpFileName ) )
	{
		Console.Error( 1, "Failed to open %s", scpFileName.c_str() );
		return;
	}

	UString basePath		= cwmWorldState->ServerData()->Directory( CSDDP_SCRIPTS );
	Script *fileAssocData	= new Script( scpFileName, NUM_DEFS, false );
	if( fileAssocData != NULL )
	{
		ScriptSection *mSection = fileAssocData->FindEntry( "COMMAND_SCRIPTS" );
		if( mSection != NULL )
		{
#ifdef _DEBUG
			Console.Print( "Preparing for command registration\n" );
#endif
			UString tag, data;
			for( tag = mSection->First(); !mSection->AtEnd(); tag = mSection->Next() )
			{
				data				= mSection->GrabData();
				UString fullpath	= basePath + data;
				if( !FileExists( fullpath ) )
				{	// doesn't exist
					Console.Error( 3, "Command SE loading of %s failed, file does not exist!", data.c_str() );
				}
				else
				{
					try
					{
#ifdef _DEBUG
						Console.Print( "Registering file %s\n", fullpath.c_str() );
#endif
						cScript *toAdd			= new cScript( fullpath, SCPT_COMMAND );
						if( toAdd != NULL )
							RegisterCommandObject( toAdd->Object(), toAdd );
					}
					catch( std::runtime_error &e )
					{
						Console.Error( 2, "Compiling %s caused a construction failure (Details: %s)", fullpath.c_str(), e.what() );
					}
				}
			}
		}
		else
			Console.Warning( 2, "No JS command mappings found" );
		delete fileAssocData;
	}
}

void Triggers::ParseMagic( void )
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
		ScriptSection *mSection = fileAssocData->FindEntry( "MAGIC_SCRIPTS" );
		if( mSection != NULL )
		{
#ifdef _DEBUG
			Console.Print( "Preparing for magic spell registration\n" );
#endif
			UString tag, data;
			for( tag = mSection->First(); !mSection->AtEnd(); tag = mSection->Next() )
			{
				data				= mSection->GrabData();
				UString fullpath	= basePath + data;
				if( !FileExists( fullpath ) )
				{	// doesn't exist
					Console.Error( 3, "Magic SE loading of %s failed, file does not exist!", data.c_str() );
				}
				else
				{
					try
					{
#ifdef _DEBUG
						Console.Print( "Registering file %s\n", fullpath.c_str() );
#endif
						cScript *toAdd			= new cScript( fullpath, SCPT_MAGIC );
						if( toAdd != NULL )
							RegisterMagicObject( toAdd->Object(), toAdd );
					}
					catch( std::runtime_error &e )
					{
						Console.Error( 2, "Compiling %s caused a construction failure (Details: %s)", fullpath.c_str(), e.what() );
					}
				}
			}
		}
		else
			Console.Warning( 2, "No JS magic mappings found" );
		delete fileAssocData;
	}
}

cScript *Triggers::GetScript( UI16 triggerNumber )
{
	std::map< UI16, cScript *>::const_iterator p = scriptTriggers.find( triggerNumber );
	if( p != scriptTriggers.end() )	//	Script was found
		return p->second;
	else	//	Account was not found
		return NULL;

}

cScript *Triggers::GetAssociatedScript( JSObject *toFind )
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
		std::map< JSObject *, cScript * >::const_iterator cFnd = commandScripts.find( toFind );
		if( cFnd != commandScripts.end() )
			return cFnd->second;

		// Check magic stuff here
		std::map< JSObject *, cScript * >::const_iterator mFnd = magicScripts.find( toFind );
		if( mFnd != magicScripts.end() )
			return mFnd->second;
	}
	return NULL;
}

void Triggers::ReloadJS( void )
{
	Cleanup();
	scriptTriggers.clear();	// erase it all
	commandScripts.clear();
	magicScripts.clear();
	ParseScript();
	ParseCommands();
	ParseMagic();
}


void Triggers::RegisterObject( JSObject *toReg, UI16 tNum )
{
	scriptToTriggerMapping[toReg] = tNum;
}
void Triggers::UnregisterObject( JSObject *toUnreg )
{
	std::map< JSObject *, UI16 >::iterator toFind = scriptToTriggerMapping.find( toUnreg );
	if( toFind != scriptToTriggerMapping.end() )
		scriptToTriggerMapping.erase( toFind );
}

void Triggers::RegisterCommandObject( JSObject *toReg, cScript *tNum )
{
	commandScripts[toReg] = tNum;
}
void Triggers::RegisterMagicObject( JSObject *toReg, cScript *tNum )
{
	magicScripts[toReg] = tNum;
}
void Triggers::UnregisterCommandObject( JSObject *toUnreg )
{
	std::map< JSObject *, cScript * >::iterator toFind = commandScripts.find( toUnreg );
	if( toFind != commandScripts.end() )
		commandScripts.erase( toFind );
}

void Triggers::UnregisterMagicObject( JSObject *toUnreg )
{
	std::map< JSObject *, cScript * >::iterator toFind = magicScripts.find( toUnreg );
	if( toFind != magicScripts.end() )
		magicScripts.erase( toFind );
}

cScript * Triggers::FirstCommand( void )
{
	commandScpIterator = commandScripts.begin();
	if( commandScpIterator != commandScripts.end() )
		return commandScpIterator->second;
	else
		return NULL;
}
cScript * Triggers::NextCommand( void )
{
	if( commandScpIterator != commandScripts.end() )
	{
		++commandScpIterator;
		if( commandScpIterator != commandScripts.end() )
			return commandScpIterator->second;
		else
			return NULL;
	}
	else
		return NULL;
}
bool Triggers::FinishedCommands( void )
{
	return( commandScpIterator == commandScripts.end() );
}

cScript * Triggers::FirstSpell( void )
{
	magicScpIterator = magicScripts.begin();
	if( magicScpIterator != magicScripts.end() )
		return magicScpIterator->second;
	else
		return NULL;
}
cScript * Triggers::NextSpell( void )
{
	if( magicScpIterator != magicScripts.end() )
	{
		++magicScpIterator;
		if( magicScpIterator != magicScripts.end() )
			return magicScpIterator->second;
		else
			return NULL;
	}
	else
		return NULL;
}
bool Triggers::FinishedSpells( void )
{
	return( magicScpIterator == magicScripts.end() );
}

}

