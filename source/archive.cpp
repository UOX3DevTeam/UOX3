#include "uox3.h"
#include "cAccount.h"
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

	int chResult = _chdir( cwmWorldState->ServerData()->GetBackupDirectory() );
	if( chResult != 0 )
	{
		Console << "Cannot find backup directory, please check server.scp" << myendl;
		return;
	}

	int makeResult = _mkdir( timebuffer );
	if( makeResult != 0 )
	{
		Console << "Cannot create backup directory, please check available disk space" << myendl;
		_chdir( cwmWorldState->ServerData()->GetRootDirectory() );
		return;
	}

	char filename1[MAX_PATH], filename2[MAX_PATH];

	sprintf( filename1, "%s%s/accounts", cwmWorldState->ServerData()->GetBackupDirectory(), timebuffer );
	makeResult = _mkdir( filename1 );
	if( makeResult != 0 )
		Console << "Cannot create accounts backup directory, please check available disk space" << myendl;
	else if ( Accounts->SaveAccounts( (string)filename1, (string)filename1 ) == -1 )
		Console << "Cannot save accounts for backup" << myendl;

	sprintf( filename1, "%shouse.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );
	sprintf( filename2, "%s%s/house.wsc", cwmWorldState->ServerData()->GetBackupDirectory(), timebuffer );

	fileCopy( filename1, filename2 );

	// effect backups
	sprintf( filename1, "%seffects.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );
	sprintf( filename2, "%s%s/effects.wsc", cwmWorldState->ServerData()->GetBackupDirectory(), timebuffer );

	fileCopy( filename1, filename2 );

	const SI32 AreaX = UpperX / 8;	// we're storing 8x8 grid arrays together
	const SI32 AreaY = UpperY / 8;

	for( SI32 counter1 = 0; counter1 < AreaX; counter1++ )	// move left->right
	{
		for( SI32 counter2 = 0; counter2 < AreaY; counter2++ )	// move up->down
		{
			sprintf( filename1, "%s%i.%i.wsc", cwmWorldState->ServerData()->GetSharedDirectory(), counter1, counter2 );				// let's name our file
			sprintf( filename2, "%s%s/%i.%i.wsc", cwmWorldState->ServerData()->GetBackupDirectory(), timebuffer, counter1, counter2 );	// let's name our file
			fileCopy( filename1, filename2 );
		}
	}
	sprintf( filename1, "%soverflow.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );
	sprintf( filename2, "%s%s/overflow.wsc", cwmWorldState->ServerData()->GetSharedDirectory(), timebuffer );
	fileCopy( filename1, filename2 );

	sprintf( filename1, "%sjails.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );
	sprintf( filename2, "%s%s/jails.wsc", cwmWorldState->ServerData()->GetSharedDirectory(), timebuffer );
	fileCopy( filename1, filename2 );

	sprintf( filename1, "%sguilds.wsc", cwmWorldState->ServerData()->GetRootDirectory() );
	sprintf( filename2, "%s%s/guilds.wsc", cwmWorldState->ServerData()->GetSharedDirectory(), timebuffer );
	fileCopy( filename1, filename2 );

	sprintf( filename1, "%sregions.wsc", cwmWorldState->ServerData()->GetRootDirectory() );
	sprintf( filename2, "%s%s/regions.wsc", cwmWorldState->ServerData()->GetSharedDirectory(), timebuffer );
	fileCopy( filename1, filename2 );

	Console << "Finished backup" << myendl;
	_chdir( cwmWorldState->ServerData()->GetRootDirectory() );
	return;
}
