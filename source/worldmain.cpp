// WorldMain.cpp: implementation of the CWorldMain class.
//
//////////////////////////////////////////////////////////////////////

#include "uox3.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CWorldMain::CWorldMain()
{
	//	December 24, 2000 - EviLDeD
	//	Modify the constructor to load in the uox3.ini file automatically
	announce( false );

	isSaving = false;
	sData = new cServerData();
}

CWorldMain::~CWorldMain()
{
	delete sData;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	?/?/1997
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//|									
//|	Modification	-	10/21/2002	-	Xuri fix for archiving. Now it wont always
//|									archive :)
//o--------------------------------------------------------------------------o
//|	Returns				-	N/A
//o--------------------------------------------------------------------------o	
void CWorldMain::savenewworld( bool x )
{
	static unsigned int save_counter = 0;
//	tempeffectsoff();

	cSpawnRegion *spawnReg = NULL;

	for( UI32 i = 1; i < totalspawnregions; i++ )
	{
		spawnReg = spawnregion[i];

		if( spawnReg == NULL )
			continue;
		else
			spawnReg->checkSpawned();
	}
	
	ncspawnsp.GarbageCollect();
	nspawnsp.GarbageCollect();
	ncharsp.GarbageCollect();
	nitemsp.GarbageCollect();
	worldSaveProgress = 1;
	if( !Saving() )
	{
		Console.PrintSectionBegin();
		if( announce() )
		{
			sysbroadcast( Dictionary->GetEntry( 1615 ) );
			SpeechSys->Poll();
		}
		Network->ClearBuffers();

		if( x )
			Console << "| Starting manual world data save...." << myendl;
		else 
			Console << "| Starting automatic world data save...." << myendl;
		
		if( ServerData()->GetServerBackupStatus() && strlen( ServerData()->GetBackupDirectory() ) > 1 )
		{
			save_counter++;
			if(( save_counter % ServerData()->GetBackupRatio() ) == 0 )
			{
				Console << "| Archiving world files." << myendl;
				fileArchive();
			}
		}
		Console << "Saving Misc. data... ";
		//Accounts->SaveAccounts();
		ServerData()->save();
		savelog( "Server data save\n", "server.log" );
		RegionSave();
		Console.PrintDone();
		MapRegion->Save(); 
		GuildSys->Save();
		JailSys->WriteData();
		Skills->SaveResources();
		SaveEffects();

		if( announce() )
			sysbroadcast("World Save Complete.");

		//	If accounts are to be loaded then they should be loaded
		//	all the time if using the web interface
		if( ServerData()->GetExternalAccountStatus()  )
		{
			Accounts->Load();
		}
		Accounts->Save();
		worldSaveProgress = 2;
		Console << "World save complete." << myendl;
		Console.PrintSectionBegin();
	}

//	tempeffectson();
	uiCurrentTime = getclock();
}

bool CWorldMain::announce( void )
{
	return DisplayWorldSaves;
}

void CWorldMain::announce( bool choice )
{
	DisplayWorldSaves = choice;
}

bool CWorldMain::Saving( void )
{
	return isSaving;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	void CWorldMain::RegionSave( void )
//|	Date					-	Unknown
//|	Developer(s)	-	Abaddon
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
void CWorldMain::RegionSave( void )
{
	char regionsFile[MAX_PATH];
	sprintf(regionsFile, "%s%s", cwmWorldState->ServerData()->GetSharedDirectory(), "regions.wsc");
	FILE *regions = fopen( regionsFile, "w" );
	if( regions != NULL )
	{
		for( int regionCounter = 0; regionCounter < 256; regionCounter++ )
		{
			if( region[regionCounter] != NULL )
				region[regionCounter]->Save( regions );
		}
		fclose( regions );
	}
}

cServerData *CWorldMain::ServerData( void )
{
	return sData;
}

/*
Below are the tables to be writen to saveidx.wsc
these are lengths of all the data fields that are used in binary 
saves.  These tables should be updated when any tag is changed.
These tables provide useful info for 3rd party tools... so they can
easily skip unknown data in world save files.

IMPORTANT:
The first 64 tags for BOTH items and characters are RESERVED for 
cBaseObject values and should not be used for item/char specific stuff.
The first 64 values in char and item tables should always match!

Length of 0xFF(255) implies NULL terminated string.
-The exception is 21 and 22, which are used for tags, 21 is a null 
 string for the tag name, followed by a null string, and 22 is a null
 string for tag name, followed by a 4 byte (long) JSVal

~Bryan "Zippy" Pass (1/9/02)
*/

const UI08 CharSaveTable[256] = 
{// 0	1	2	3	4	5	6	7  
	255,255,4,	7,	4,	2,	2,	2,	//0 (0)
	4,	12,	6,	4,	4,	4,	2,	1,	//8 (8)
	1,	4,	2,	2,	16,	255,255,0,	//16 (10)
	0,	0,	0,	0,	0,	0,	0,	0,	//24 (18)
	0,	0,	0,	0,	0,	0,	0,	0,	//32 (20)
	0,	0,	0,	0,	0,	0,	0,	0,	//40 (28)
	0,	0,	0,	0,	0,	0,	0,	0,	//48 (30)
	0,	0,	0,	0,	0,	0,	0,	0,	//56 (38)
//indexes above this line are reserved for cBaseObject
	4,	4,255,255,255,	2,	4,	2,	//64 (40)
	2,	1,	1,	8,	4,	4,	2,	4,	//72 (48)
	8,	4,	4,	4,	8,	4,	1,	9,	//80 (50)
	1,	1,	1,	1,	5,	1,	4,	6,	//88 (58)
	3,	3,	4,	2,	4,	4,	1+2*TRUESKILLS,1+2*ALLSKILLS,//96 (60)
	1+TRUESKILLS,	4,	2,	2,	2,	0,	0,	0,	//104 (68)
	0,	0,	0,	0,	0,	0,	0,	0,	//112 (70)
	0,	0,	0,	0,	0,	0,	0,	0,	//120 (78)
	0,	0,	0,	0,	0,	0,	0,	0,	//128 (80)
	0,	0,	0,	0,	0,	0,	0,	0,	//136 (88)
	0,	0,	0,	0,	0,	0,	0,	0,	//144 (90)
	0,	0,	0,	0,	0,	0,	0,	0,	//152 (98)
	0,	0,	0,	0,	0,	0,	0,	0,	//160 (a0)
	0,	0,	0,	0,	0,	0,	0,	0,	//168 (a8)
	0,	0,	0,	0,	0,	0,	0,	0,	//176 (b0)
	0,	0,	0,	0,	0,	0,	0,	0,	//184 (b8)
	0,	0,	0,	0,	0,	0,	0,	0,	//192 (c0)
	0,	0,	0,	0,	0,	0,	0,	0,	//200 (c8)
	0,	0,	0,	0,	0,	0,	0,	0,	//208 (d0)
	0,	0,	0,	0,	0,	0,	0,	0,	//216 (d8)
	0,	0,	0,	0,	0,	0,	0,	0,	//224 (e0)
	0,	0,	0,	0,	0,	0,	0,	0,	//232 (e8)
	0,	0,	0,	0,	0,	0,	0,	0,	//240 (f0)
	0,	0,	0,	0,	0,	0,	0,	0,	//248 (f8)
	//ff
};

const UI08 ItemSaveTable[256] = 
{// 0	1	2	3	4	5	6	7  
	255,255,4,	7,	4,	2,	2,	2,	//0 (0)
	4,	12,	6,	4,	4,	4,	2,	1,	//8 (8)
	1,	4,	2,	2,	16,	255,255,0,	//16 (10)
	0,	0,	0,	0,	0,	0,	0,	0,	//24 (18)
	0,	0,	0,	0,	0,	0,	0,	0,	//32 (20)
	0,	0,	0,	0,	0,	0,	0,	0,	//40 (28)
	0,	0,	0,	0,	0,	0,	0,	0,	//48 (30)
	0,	0,	0,	0,	0,	0,	0,	0,	//56 (38)
//indexes above this line are reserved for cBaseObject
	5,	8,255,	4,	4,255,	2,	1,	//64 (40)
	4, 12,	4,	1,	2,	3,	8,	1,	//72 (48)
	2,	5,	4,	7,	4,	1,	4,	1,	//80 (50)
//88 to 95 used for multis
	4,	4,	4,	2,	0,	0,	0,	0,	//88 (58)
//-------------------------------------------------
	0,	0,	0,	0,	0,	0,	0,	0,	//96 (60)
	0,	0,	0,	0,	0,	0,	0,	0,	//104 (68)
	0,	0,	0,	0,	0,	0,	0,	0,	//112 (70)
	0,	0,	0,	0,	0,	0,	0,	0,	//120 (78)
	0,	0,	0,	0,	0,	0,	0,	0,	//128 (80)
	0,	0,	0,	0,	0,	0,	0,	0,	//136 (88)
	0,	0,	0,	0,	0,	0,	0,	0,	//144 (90)
	0,	0,	0,	0,	0,	0,	0,	0,	//152 (98)
	0,	0,	0,	0,	0,	0,	0,	0,	//160 (a0)
	0,	0,	0,	0,	0,	0,	0,	0,	//168 (a8)
	0,	0,	0,	0,	0,	0,	0,	0,	//176 (b0)
	0,	0,	0,	0,	0,	0,	0,	0,	//184 (b8)
	0,	0,	0,	0,	0,	0,	0,	0,	//192 (c0)
	0,	0,	0,	0,	0,	0,	0,	0,	//200 (c8)
	0,	0,	0,	0,	0,	0,	0,	0,	//208 (d0)
	0,	0,	0,	0,	0,	0,	0,	0,	//216 (d8)
	0,	0,	0,	0,	0,	0,	0,	0,	//224 (e0)
	0,	0,	0,	0,	0,	0,	0,	0,	//232 (e8)
	0,	0,	0,	0,	0,	0,	0,	0,	//240 (f0)
	0,	0,	0,	0,	0,	0,	0,	0,	//248 (f8)
	//ff
};

