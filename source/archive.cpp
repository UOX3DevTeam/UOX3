#include "uox3.h"
#include "cAccountClass.h"
#ifdef __LINUX__
	#include <dirent.h>
#else
	#include <direct.h>
#endif

const SI32 BUFFERSIZE = 16384;

bool fileCopy( const char *sourceFile, const char *targetFile )
{
	if( sourceFile == NULL || targetFile == NULL )
		return false;
	FILE *source = fopen( sourceFile, "rb" );	
	if( source == NULL )
		return false;
	FILE *target = fopen( targetFile, "wb" );
	if( target == NULL )
	{
		fclose( source );
		return false;
	}
	UI08 buffer[BUFFERSIZE];
	while( !feof( source ) )
	{
		SI32 amtRead = fread( buffer, 1, BUFFERSIZE, source );
		SI32 amtWritten = fwrite( buffer, 1, amtRead, target );
		if( amtWritten != amtRead )
		{
			fclose( source );
			fclose( target );
			return false;
		}
	}
	fclose( source );
	fclose( target );
	return true;
}
   
//o---------------------------------------------------------------------------o
//|   Function    :  void fileArchive( void )
//|   Date        :  11th April, 2002
//|   Programmer  :  duckhead
//o---------------------------------------------------------------------------o
//|   Purpose     :  Makes a backup copy of a file in the shared directory.
//|                  puts the copy in the backup directory
//o---------------------------------------------------------------------------o
static void backupFile(char * filename, char * backupDir)
{
        char backupFromFileName[MAX_PATH];
        char backupToFileName[MAX_PATH];
	
	sprintf(backupFromFileName, "%s%s", cwmWorldState->ServerData()->GetSharedDirectory(), filename);
	sprintf(backupToFileName, "%s%s", backupDir, filename);

	fileCopy( backupFromFileName, backupToFileName );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void fileArchive( void )
//|   Date        :  24th September, 2001
//|   Programmer  :  Abaddon (rewritten for 0.95)
//o---------------------------------------------------------------------------o
//|   Purpose     :  Makes a backup copy of the current world state
//o---------------------------------------------------------------------------o
void fileArchive( void )
{
	Console << "Beginning backup... ";
	time_t mytime;
	time( &mytime );
	tm *ptime = localtime( &mytime );			// get local time
	const char *timenow = asctime( ptime );   // convert it to a string

	char timebuffer[256];
	strcpy( timebuffer, timenow );
    
	// overwriting the characters that aren't allowed in paths
	for( UI32 a = 0; a < strlen( timebuffer ); a++ )
	{
		if( isspace( timebuffer[a] ) || timebuffer[a] == ':' )
			timebuffer[a]='-';
	}

	char filename1[MAX_PATH]; //, filename2[MAX_PATH];

	sprintf( filename1, "%s/%s", cwmWorldState->ServerData()->GetBackupDirectory(), timebuffer );
	int makeResult = _mkdir( filename1),0777 );
	if( makeResult != 0 )
	{
		Console << "Cannot create backup directory, please check available disk space" << myendl;
		return;
	}

	sprintf( filename1, "%s%s/accounts", cwmWorldState->ServerData()->GetBackupDirectory(), timebuffer );
	makeResult = _mkdir( filename1),0777 );
	if( makeResult != 0 )
		Console << "Cannot create accounts backup directory, please check available disk space" << myendl;
	//else if ( Accounts->SaveAccounts( (std::string)filename1, (std::string)filename1 ) == -1 )
		//Console << "Cannot save accounts for backup" << myendl;
	Console << "NOTICE: Accounts not backed up. Archiving will change. Sorry for the trouble." << myendl;

	char backupDir[MAX_PATH];

	sprintf( backupDir, "%s%s/", 
		 cwmWorldState->ServerData()->GetBackupDirectory(), timebuffer );
	backupFile("house.wsc", backupDir);

	// effect backups
	backupFile("effects.wsc", backupDir);

	const SI32 AreaX = UpperX / 8;	// we're storing 8x8 grid arrays together
	const SI32 AreaY = UpperY / 8;
	char backupPath[MAX_PATH + 1];

	sprintf(backupPath, "%s%s/", 
		cwmWorldState->ServerData()->GetSharedDirectory(), timebuffer );

	for( SI32 counter1 = 0; counter1 < AreaX; counter1++ )	// move left->right
	{
		for( SI32 counter2 = 0; counter2 < AreaY; counter2++ )	// move up->down
		{
			sprintf( filename1, "%i.%i.wsc", counter1, counter2 );
			backupFile(filename1, backupDir);
		}
	}
	
	backupFile("overflow.wsc", backupDir);
	backupFile("jails.wsc", backupDir);
	backupFile("guilds.wsc", backupDir);
	backupFile("regions.wsc", backupDir);

	Console << "Finished backup" << myendl;
	return;
}
