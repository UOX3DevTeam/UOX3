// *******************************************************************
// trigger.cpp (Trigger processing module)
//
// The trigger is activated here.  When the users doubleclicks on a triggered
// item or a triggered NPC, this routine will load the trigger from the
// triggers.scp (for dynamic), ntrigrs.scp (for NPCs) file or wtrigrs.scp
// (static world objects) and process the tokens for the trigger.
// NOTE: By using this source you are agreeing to all the terms of the GNU
// agreement.  Any use of this code, sniplets of this code, concepts of
// this code or ideas derived from this code requires that any program(s)
// and source containing any of the above mentioned states of the code, must
// be released in its entirety.
//
// DO NOT MODIFY THIS CODE UNLESS you are sure you know what your doing.
// The trigger system is not designed for making tokens that do just one
// thing.  Remember, if it can't be used for multiple things, it shouldnt
// be here. Do not distribute your changes unless its either by consent of
// the author(Zadius) or if the change and/or addition is by popular
// demand and it has been thouroughly tested.
// 
// Zadius 11-4-98 (linkage@xmission.com)
// *******************************************************************


#include <errno.h>
#include "uox3.h"
#include "stream.h"

//o---------------------------------------------------------------------------o
//|	Function	-	void readscript( FILE *toReadFrom, char *temp )
//|	Date		-	1st March, 2000
//|	Programmer	-	Abaddon
//o---------------------------------------------------------------------------o
//|	Purpose		-	Reads lines from the toReadFrom until it finds a
//|					non-commented, usable line. Commented lines are
//|					preceeded with // chars.
//o---------------------------------------------------------------------------o
void readscript( FILE *toReadFrom, char *temp ) // Read a line from the opened script file
{
	if( temp == NULL )
		return;
	if( toReadFrom == NULL )
	{
		strcpy( temp, "EOF" );
		return;
	}
	bool valid = false;
	temp[0] = 0;
	while( !valid )
	{
		int i = 0;
		if( feof( toReadFrom ) ) 
			return;
		char c = (char)fgetc( toReadFrom );
		while( c != 10 && i < 1024 )	// don't overflow our buffer
		{
			if( c != 13 )// && c!=0)
			{
				temp[i] = c;
				i++;
			}
			if( feof( toReadFrom ) ) 
				return;
			c = (char)fgetc( toReadFrom );
			if( c == 9 ) c = 32;	// if we actually have a tab, then replace it with a space!
		}
		temp[i] = 0;
		valid = true;
		if( temp[0] == '/' && temp[1] == '/' ) 
			valid = false;
		if( temp[0] == '{') 
			valid = false;
		if( temp[0] == 0) 
			valid = false;
	}
}

void read2( FILE *toRead, char *script1, char *script2 ) // Read line from script
{
	if( script1 == NULL || script2 == NULL )
		return;
	char temp[1024];
	readscript( toRead, temp );
	int i = 0;
	script1[0] = 0;
	script2[0] = 0;
	while( temp[i] != 0 && temp[i] != ' ' && temp[i] != '=' )
	{
		i++;
	}
	strncpy( script1, temp, i );
	script1[i] = 0;
	if( script1[0] != '}' && temp[i] != 0 ) 
		strcpy( script2, temp + i + 1 );
	return;
}

CItem *FindItemInPack( const char *itemType, CChar *toSearch );

Triggers::Triggers()
{
	ParseScript();
	ParseEnvoke();	// do envoke second, so script builds the script list
}
Triggers::~Triggers()
{
	envokeList.erase( envokeList.begin(), envokeList.end() );
}

void Triggers::Cleanup()
{//this MUST be done BEFORE deleteing Triggers, because ~cScript uses Triggers->Whatever()
	for( std::map< UI16, cScript *>::iterator p = scriptTriggers.begin(); p != scriptTriggers.end(); p++ )
	{
		cScript *pScript = p->second;
		delete pScript;
	}
}

bool Triggers::CheckEnvoke( UI16 itemID ) const
{
	std::map< UI16, UI16 >::const_iterator p = envokeList.find( itemID );
	if( p != envokeList.end() )
		return true;
	return false;
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
	char buf[MAX_PATH];

	sprintf( buf, "%sjse_objectassociations.scp", cwmWorldState->ServerData()->GetScriptsDirectory() );

	envokefile.open( buf );
	if( !envokefile.is_open() )
	{
		Console << "Unable to open " << buf << " for parsing" << myendl;
		return;
	}

	envokeList.erase( envokeList.begin(), envokeList.end() );
	char itemIDString[20], scriptIDString[20];
	while( !envokefile.eof() && !envokefile.fail() )
	{
		envokefile.getline( buf, 1024 );
		if( buf[0] == 0 || buf[0] == '/' || buf[0] == 10 || buf[0] == 13 )
			continue;
		std::string s( buf );
		std::istringstream ss( s );
		ss >> itemIDString >> scriptIDString;
		UI16 itemID = (UI16)(makeNum( itemIDString ));
		UI16 scriptID = (UI16)(makeNum( scriptIDString ));
		cScript *verify = GetScript( scriptID );
		if( verify != NULL )
			envokeList[itemID] = scriptID;
		else
			Console << "Error with envoke, itemID " << itemID << " refers to scriptID " << scriptID << " which doesn't exist" << myendl;
	}
	envokefile.close();
}

CItem *FindItemInPack( const char *itemType, CChar *toSearch )
{
	char tempSearch[32];
	CItem *tPack = getPack( toSearch );
	if( tPack != NULL )
	{
		for( CItem *tItem = tPack->FirstItemObj(); !tPack->FinishedItems(); tItem = tPack->NextItemObj() )
		{
			if( tItem != NULL )
			{
				sprintf( tempSearch, "x%x", tItem->GetID() );
				if( strstr( itemType, tempSearch ) )
					return tItem;
			}
		}
	}
	return NULL;
}


//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	09/24/2002
//|	Developer(s)	-	Unknown
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Parse a JSE Script
//|									
//|	Modification	-	09/24/2002	-	Avtotar - Fixed JSE path usage, to get the 
//|									path to JS file from the uox.ini file
//o--------------------------------------------------------------------------o
//|	Returns				-	NA
//o--------------------------------------------------------------------------o	
void Triggers::ParseScript( void )
{
	char scpFileName[MAX_PATH];
	sprintf( scpFileName, "%sjse_fileassociations.scp", cwmWorldState->ServerData()->GetScriptsDirectory() );
	FILE *data = fopen( scpFileName, "r" );
	if( data == NULL )
	{
		Console.Error( 1, "Failed to open %s", scpFileName );
		return;
	}
	std::map< UI16, cScript *>::iterator p;
	char script1[MAX_PATH], script2[MAX_PATH], temp[MAX_PATH], fullpath[MAX_PATH];
	while( !feof( data ) )
	{
		readscript( data, temp );
		if( !stricmp( temp, "SECTION SCRIPT_LIST" ) )
		{
			bool endSection = false;
			UI16 triggerNumber;
			while( !feof( data ) && !endSection )
			{
				read2( data, script1, script2 );
				if( script1[0] == '}' )
					endSection = true;
				else
				{
					triggerNumber = (UI16)makeNum( script1 );
					p = scriptTriggers.find( triggerNumber );
					if( p != scriptTriggers.end() )
						scriptTriggers.erase( p );
					// Sept 24, 2002 - Avtotar - Fixed path up so that path from uox.ini to the JS dir is not used.
					sprintf( fullpath, "%s%s", cwmWorldState->ServerData()->GetScriptsDirectory(), script2);
					// let's add a verification of the file existing, shall we?
					FILE *toTest = fopen( fullpath, "r" );
					if( toTest == NULL )
					{	// doesn't exist
						Console.Error( 3, "SE mapping of %i to %s failed, %s does not exist!", triggerNumber, script2, script2 );
					}
					else
					{
						fclose( toTest );	// exists
						cScript *toAdd = new cScript( fullpath );
						scriptTriggers[triggerNumber] = toAdd;
						if( toAdd != NULL )
							RegisterObject( toAdd->Object(), triggerNumber );
					}
				}
			}
		}
	}
	fclose( data );
}


cScript *Triggers::GetScript( UI16 triggerNumber )
{
	std::map< UI16, cScript *>::iterator p = scriptTriggers.find( triggerNumber );
	if( p != scriptTriggers.end() )	//	Script was found
		return p->second;
	else	//	Account was not found
		return NULL;

}

void Triggers::ReloadJS( void )
{
	std::map< UI16, cScript *>::iterator p;
	for( p = scriptTriggers.begin(); p != scriptTriggers.end(); p++ )
		delete p->second;
	scriptTriggers.erase( scriptTriggers.begin(), scriptTriggers.end() );	// erase it all
	ParseScript();
}

void Triggers::PeriodicTriggerCheck( void )
{
	for( UI32 i = 0; i < periodicTriggers.size(); i++ )
	{
		if( uiCurrentTime >= (UI32)periodicTriggers[i].nextRun )
		{
			if( periodicTriggers[i].trigScript != NULL )
				periodicTriggers[i].trigScript->OnSystemSlice();
		}
	}
}

void Triggers::PushPeriodicTrigger( cScript *trigScript, SI32 nextRun )
{
	periodicTriggers.push_back( periodicTrigger( trigScript, nextRun ) );
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

UI16 Triggers::FindObject( JSObject *toFind )
{
	std::map< JSObject *, UI16 >::iterator tFnd = scriptToTriggerMapping.find( toFind );
	if( tFnd != scriptToTriggerMapping.end() )
		return tFnd->second;
	return 0xFFFF;
}
