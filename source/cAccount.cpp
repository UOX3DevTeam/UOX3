//o--------------------------------------------------------------------------o
//|	Function/Class-	cAccounts
//|	Date					-	02/11/2002
//| Last Update: DarkStorm, 02/21/2002
//| Some small fixes
//| We should think about a rewrite of SaveAccounts, that function 
//| Does almost everything twice
//o--------------------------------------------------------------------------o
//|	Description		-	Class that handles the accounts portion of server
//|									operation.
//o--------------------------------------------------------------------------o
#include "uox3.h"
#include <fstream>

using namespace std;

// Added MakeDirectory (DarkStorm)
// DarkStorm, 2/22/2002:
// please leave that commented out 
// just left it here for reference or educational purposes
// we're now using makeDirectory in <utilsys.h> !!
//
// EviLDeD: no were not. As this new class does not return proper directory creations values its not usable.
// and is what part of the accounts problems are. 
// EviLDeD: DarkStorm I ask that you stop just putting things in that you dont understand completly or ask.

bool makeDirectory( const char *Path )
{
	SI32 Result;

#ifdef __NT__
	if( !CreateDirectory( Path, NULL ) )
		{
		// directory was not crated, check to see if its becuase it alrady exists. 
		// if se we build the path, we just dont create the directory.
		Result = GetLastError();
		if( Result != ERROR_ALREADY_EXISTS )
		{
			return false;
		}
		// if the directory already exists we need to tell the calling code that we made the directory
		return true;
	}
#else
	// Removed the Permissions as they should be set
	// automatically
	if( mkdir( Path, (mode_t)0777 ) == -1 )
	{
		Result = errno;
		if( Result != EEXIST )
		{
			return false;
		}
		// if the directory already exists we need to tell the calling code that we made the directory
		return true;
	}
#endif

	return true;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//	Extentions: (.adm)-text accounts file  (.abs)-binary accounts file

cAccounts::cAccounts() : AccountCount( 0 ), highestAccountNumber( -1 ), isReloaded( false ), isBinary( false ), isValid( false )
{
	PathToFile.erase();
	isReloaded = isBinary = isValid = false;
}

cAccounts::cAccounts( const char *accountsfile ) : AccountCount( 0 ), highestAccountNumber( -1 ), isReloaded( false ), isBinary( false ), isValid( false )
{
	PathToFile = accountsfile;
}

cAccounts::~cAccounts()
{

}

//o--------------------------------------------------------------------------
//|	Function		-	bool ConvertAccounts(const char *filename)
//|	Date			-	March 8, 2000
//|	Programmer		-	EviLDeD
//|	Modified		-
//|
//|	Modification	-	02/03/2002 - Prefix accounts.adm file with accounts path
//o--------------------------------------------------------------------------
//|	Purpose			-	This function is does basically what the name implies
//|						It reads in a accounts format, converts that format into
//|						the format for use with the server. At this stage this
//|						function will convert only between text versions 1.0
//|						and the new 2.0 Script/Accounts/Item format
//o--------------------------------------------------------------------------
bool cAccounts::ConvertAccounts( const char *filename )
{
	ifstream ifsSource;
	ifstream ifsDestination;
	char szInBuffer[128], szOutBuffer[1024];
	const char *path = cwmWorldState->ServerData()->GetAccountsDirectory();
	char buffer[512];
	UI32 slen = strlen( path );
	if( path[slen-1] == '\\' || path[slen-1] == '/' )
		sprintf( buffer, "%saccounts.adm", path );
	else
		sprintf( buffer, "%s/accounts.adm", path );
	memset( szOutBuffer, 0x00, 1024 );
	ifsSource.open( buffer, ios::in );
	if( !ifsSource.is_open() )
	{
		ErrNum = 0x00000100;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
		ErrDesc = "Unable to open the specified file for input";
		return false;
	}
	ifsDestination.open( filename, ios::out );
	if( !ifsDestination.is_open() )
	{
		ErrNum = 0x00000200;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
		ErrDesc = "Unable to open the specified file for output";
		return false;
	}
	
	while( !ifsSource.eof() )
	{
		ifsSource.getline( szInBuffer, 127 );
		if( szInBuffer == NULL )	
			continue;
	}
	ifsSource.getline( szInBuffer, 127 );
	//if(szLine==NULL)
	//{	//	Assume that this accounts file is version 1
	//	ErrNum = 0x00000001;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
	//	ErrDesc = "Unable to determine version of Accounts file";
	//	return "1.0";
	//}	
	// Im going to assume that old UOX still has accounts.adm in the rood dir.


	return false;
}

//o--------------------------------------------------------------------------
//|	Function		-	const char *CheckAccountsVersion(const char *filename)
//|	Date				-	March 8, 2000
//|	Programmer	-	EviLDeD
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	This function attempts to open and read in the first line
//|								of the accounts.adm file to determine the version that the
//|								file is in. This is primarily intended to differentiate 
//|								between account files to determine format
//o--------------------------------------------------------------------------
const char *cAccounts::CheckAccountsVersion( const char *filename )
{
	ifstream isFile;
	char szLine[128];

	memset( szLine, 0x00, 128 );
	if( filename == NULL )
		isFile.open( "./accounts.adm", ios::in );
	else
		isFile.open(filename,ios::in);
	if(!isFile.is_open())
	{
		ErrNum = 0x00000100;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
		ErrDesc = "Unable to open the specified file";
		return "1.0";
	}
	isFile.getline( szLine, 127 );
	if( szLine == NULL )
	{	//	Assume that this accounts file is version 1
		ErrNum = 0x00000001;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
		ErrDesc = "Unable to determine version of Accounts file";
		return "1.0";
	}

	char *p; //,strUOXVersion[15],strBuildNumber[8];
	if((p=strtok(szLine,"-"))==NULL)
	{
		ErrNum = 0x00000001;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
		ErrDesc = "Unable to determine version of Accounts file";
		return "1.0";
	}
	if( strncmp( p, "AV", ( 2 * sizeof( char ) ) ) )
	{
		ErrNum = 0x00000002;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
		ErrDesc = "Expected token[AV] was not found.";
		return "1.0";
	}
	strcpy(szAccountsVersion,(szLine+2));
	isFile.close();
	return szAccountsVersion;
}

UI32 cAccounts::ReadToken( FILE *openfile )
{
	return 0;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	long LoadAccessList( void )
//|	Date					-	02/07/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 Development Team
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Load/compress and store this data in memory for fast look
//|									up when players attempt to log in. flag bits are used
//|									to handle banning, xgm clients, etc. There is no reason 
//|									to load in character information and objects into the 
//|									world untill a GM in game requests apaper doll, or someone
//|									actually loggs in.
//o--------------------------------------------------------------------------o
//|	Returns				- [LONG] count of access blocks
//o--------------------------------------------------------------------------o
long cAccounts::LoadAccessList( void )
{

	ifstream ifsFile;
	char szLine[128];
	char buffer[MAX_PATH];
	const char *path = cwmWorldState->ServerData()->GetAccessDirectory();
	UI32 slen = strlen( path );
	if( path[slen-1] == '\\' || path[slen-1] == '/' )
		sprintf( buffer, "%saccess.adm", path );
	else
		sprintf( buffer, "%s/access.adm", path );
	ifsFile.open( buffer , ios::in );
	if( !ifsFile.is_open() )
	{
		ErrNum = 0x00000100;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
		ErrDesc = "Unable to open the specified file";
		return -1;
	}
	bool braces[3] = { false, false, false };
	long aclidx = -1;	// when we enter our first area, it will become 0
	ifsFile.getline( szLine, 127 );
	AAREC *aarPointer = NULL;
	while( !ifsFile.eof() )
	{
		if( szLine[0] == '/' )
		{
			ifsFile.getline( szLine, 127 );
			continue;
		}
		else if( !strnicmp( szLine, "SECTION ACCESS", strlen( "SECTION ACCESS" ) ) )
		{
			aarPointer = new AAREC;	
			memset( aarPointer, 0x00, sizeof( AAREC ) );
			if( strlen( szLine ) != strlen( "SECTION ACCESS" ) )
				aarPointer->id = aclidx = makeNum( &szLine[strlen( "SECTION ACCESS" )] );
			else	
				aclidx = -1;

			if( aclidx >= highestAccountNumber )
				highestAccountNumber = aclidx+1;

			braces[0] = true;
			ifsFile.getline( szLine, 127 );
			continue;	// get next line
		}
		else if( braces[0] && szLine[0] == '{' ) 
		{
			if( !braces[1] )
			{
				braces[1] = true;
				ifsFile.getline( szLine, 127 );
				continue;
			}
			else
			{
				ifsFile.close();
				ErrNum = 0x00000001;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
				ErrDesc = "Another '{' was encountered before '}'";
				return -1;
			}
		}
		else if( braces[0] && braces[1] && szLine[0] == '}' )
		{
			braces[0] = braces[1] = false;
			aarPointer->id = aclidx;
			string keystring( aarPointer->Info.username );
			AAREC *alreadyExists = GetAccessBlock( aarPointer->Info.username );
			if( alreadyExists != NULL )
			{
				delete alreadyExists;
			}
			aarMap[keystring] = aarPointer;
			ifsFile.getline( szLine, 127 );
			continue;
		}
		else if( !braces[0] && ( braces[1] || braces[2] ) )
		{
			ifsFile.close();
			ErrNum = 0x00000001;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
			ErrDesc = "Invalid block encountered";
			return -1;
		}

		//
		char *token[2] = { NULL, NULL };
		token[0] = strtok( szLine, " " );
		token[1] = strtok( NULL, "\n" );
		if( token[1] == NULL )	// no data!
		{
			ifsFile.getline( szLine, 127 );
			continue;
		}
		else if( braces[0] && braces[1] )
		{
			if( !stricmp( token[0], "NAME" ) )
				strcpy( aarPointer->Info.username, token[1] );

			else if( !stricmp( token[0], "PASS" ) )
				strcpy( aarPointer->Info.password, token[1] );

			else if( !stricmp( token[0], "FLAGS" ) )
				aarPointer->bFlags = (UI16)atol( token[1] );

			else if( !stricmp( token[0], "BAN" ) )
			{
				if(!stricmp(token[1],"ON") ) // bit0
					aarPointer->bFlags|=0x01;
				else
					aarPointer->bFlags&=0xFFFE; 
			}
			else if( !stricmp( token[0], "XGMCLIENT" ) )
			{
				if(!stricmp( token[1], "ON" ) ) // bit1
					aarPointer->bFlags|=0x02;
				else
					aarPointer->bFlags&=0xFFFD; 
			}
			else if( !stricmp( token[0], "PUBLIC" ) )
			{
				if( !stricmp( token[1], "ON" ) ) //bit2
					aarPointer->bFlags|=0x04;
				else
					aarPointer->bFlags&=0xFFFB; 
			}
			else if( !stricmp( token[0], "COMMENT" ) )
			{
				strcpy( aarPointer->Info.comment, token[1] );
			}
			else if( !stricmp( token[0], "ID" ) )
			{
				if( ( aarPointer->id = atol( token[1] ) ) == 0L )
					aarPointer->id = -1;
			}
			else if( !stricmp( token[0], "PATH" ) )
			{
				strcpy( aarPointer->Info.path, token[1] );
			}
			ifsFile.getline( szLine, 127 );
		}
		//else
		//	ifsFile.getline( szLine, 127 );
	}

	return aarMap.size();
}


//o--------------------------------------------------------------------------o
//|	Function/Class-	BOOL cAccounts::CheckAccountsStamp( void )
//|	Date					-	09/20/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Do a quick open, read, and close of the accounts.adm
//|									file. If this file does not exist then ignore. If the
//|									file exists it will check the first line, and compare it
//|									to the datestamp that is associated with the file. If
//|									different then we reload the acccounts.adm file for new
//|									accounts.
//o--------------------------------------------------------------------------o
//|	Returns				- [TRUE] if accounts.adm need to be read [FALSE] otherwise
//o--------------------------------------------------------------------------o	
BOOL cAccounts::CheckAccountsStamp( void )
{
	char szLine[128];
	FILE *fpFile=NULL;
#ifdef __LINUX__
	return FALSE;
#else
	// open the accounts.adm and grab the first line
	if((fpFile=fopen(PathToFile.c_str(),"r"))==NULL)
	{
		ErrNum = 0x00000100;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
		ErrDesc = "Unable to open the specified file";
		return FALSE;
	}
	// Need an open file handle to use this funtion	
	FILETIME ftFileTime;
	FILETIME ftStampTime;
	if(!GetFileTime(fpFile,NULL,NULL,&ftFileTime))
	{
		// Ok there are an error getting the last written date. Assume default operation.
		fclose(fpFile);
		return FALSE;
	}
	// Get the first line and close
	fgets( szLine, 127, fpFile );
	fclose(fpFile);
	// Process the timestamp line and then compare
	char *c,*l,*r;
	c=l=r=NULL;
	c=strtok(szLine,"-");
	if(strcmp("DS",strupr(c))!=0)
	{
		// ok this is not a datestamp
		return FALSE;
	}
	l=strtok(NULL,"-");
	r=strtok(NULL,"\n");
	ftStampTime.dwHighDateTime=atoi(l);
	ftStampTime.dwLowDateTime =atoi(r);
	if(CompareFileTime(&ftFileTime,&ftStampTime)!=0)
		return TRUE;
	else
		return FALSE;
#endif
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	long cAccounts::LoadAccounts( void )
//|	Date					-	02/06/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 Development Team
//|	Status				-	
//|									
//|	Modification	-	02/11/2002	-	EviLDeD: Since saves have changed to output
//|									new format, we need only look for an old version and 
//|									convert it to the new version. Or load the new version.
//|									
//|	Modification	-	09/20/2002	-	Check accounts.adm for datestamp. If date-
//|									stamp is greater than laststamp, then the accounts.adm
//|									file will be read in and parsed. This should help make it
//|									significantly easier to create new accounts.
//o--------------------------------------------------------------------------o
//|	Description		-	This function handles the meat of accounts processing.
//|	(obsolete)			[mode] : (0) Text, (1) Binary, (2) Detect
//o--------------------------------------------------------------------------o
//|	Returns				- [LONG] 0-Successfull, <0 Failure
//o--------------------------------------------------------------------------o	
long cAccounts::LoadAccounts( void )
{	
	char szLine[128];
	ifstream ifsFile;
	// temp storage from old file as the new struct is different now.
	int actidx;
	ACTREC *actPointer = NULL;
	AAREC *aarPointer=NULL;
	highestAccountNumber = 0;
	bool braces[3]={false,false,false}; 
	// Ok the first thing in that we have to do is check for the new system first.
	// So we will look for the access.adm file. If it exists we know that its new format
	// otherwise its the old format so we will have to adapt.
	if(LoadAccessList()<0 && CheckAccountsStamp()==FALSE)
	{	
		// There was no access.adm file so we should read in old format

		if( PathToFile.empty() )
		{
			ErrNum = 0x00000001;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
			ErrDesc = "Path to accounts file is invalid, or empty";
			return -1;
		}
		ifsFile.open( PathToFile.c_str(), ios::in );
		if( !ifsFile.is_open() )
		{
			ErrNum = 0x00000100;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
			ErrDesc = "Unable to open the specified file";
			return -1;
		}
		ifsFile.getline( szLine, 127 );
		while( !ifsFile.eof() )
		{
			if( '/' == szLine[0] || ' ' == szLine[0] )
			{
				ifsFile.getline( szLine, 127 );
				continue;
			}
			if( !strnicmp( szLine, "SECTION ACCOUNT", strlen( "SECTION ACCOUNT" ) ) )
			{
				try
				{
					actPointer = new ACTREC;	// only create a new account when we enter a new area
					aarPointer = new AAREC;
				}
				catch(...)
				{
					// Need to call from globalheap
					Console << "|\n| ERROR: The memory allocation was bad.\n";
					return -1;
				}

				// Associate the AAREC to the ACTREC
				actPointer->lpaarHolding=aarPointer;
				// Now Lets parse the file into the correct members
				actPointer->inworld = INVALIDSERIAL;
				if( strlen( szLine ) != strlen( "SECTION ACCOUNT" ) )
					actPointer->lpaarHolding->id= actidx = makeNum( &szLine[strlen( "SECTION ACCOUNT" )] );
				else
					actidx = -1;

				if( actidx >= highestAccountNumber )
					highestAccountNumber = actidx+1;

				braces[0] = true;
				ifsFile.getline( szLine, 127 );
				continue;	// get next line
			}
			if( braces[0] && szLine[0] == '{' ) 
			{
				if( !braces[1] )
				{
					braces[1] = true;
					ifsFile.getline( szLine, 127 );
					continue;
				}
				else
				{
					ifsFile.close();
					ErrNum = 0x00000001;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
					ErrDesc = "Another '{' was encountered before '}'";
					return -1;
				}
			}
			if( braces[0] && braces[1] && szLine[0] == '}' )
			{
				braces[0] = braces[1] = false;
				actPointer->lpaarHolding->id= actidx;
				string keystring( actPointer->lpaarHolding->Info.username );
				ACTREC *alreadyExists = GetAccount( actPointer->lpaarHolding->Info.username );
				if( alreadyExists != NULL )
				{
					// Ok this record is already in memory no need to re read it.
					actPointer->charactercount = alreadyExists->charactercount;
					for(int uu=0;uu<5;uu++)
					{
						actPointer->characters[uu] = alreadyExists->characters[uu];
						//if(actPointer->lpaarHolding->id==0)
						//	actPointer->characters[uu]->SetCommandLevel( GMCMDLEVEL );
					}
					//actPointer->characters[1] = alreadyExists->characters[1];
					//actPointer->characters[2] = alreadyExists->characters[2];
					//actPointer->characters[3] = alreadyExists->characters[3];
					//actPointer->characters[4] = alreadyExists->characters[4];
				}
				else
				{
					// If we don't already exist in memory then we need to copy our account
					// information into each character object. 
					actPointer->lpaarHolding = aarPointer;
					for(int jj=0;jj<5;jj++)
						if(actPointer->characters[jj]!=NULL)
							actPointer->characters[jj]->SetAccountObj( actPointer );
				}
				if(alreadyExists)
					delete alreadyExists;
				// Just as a safty precation make sure if this is superGM (account0) that GM flags are set by default each save
				if(actPointer->lpaarHolding->id==0)
					actPointer->lpaarHolding->bFlags|=0x8000;
				// Just in case this didn't happen in the conditions above we'll do it again for safty
				actPointer->lpaarHolding=aarPointer;
				actMap[keystring] = actPointer;
				// Make sure to place a copy of the access lookup map
				aarMap[keystring] = actPointer->lpaarHolding;
				// Don't we have to set the byNum Map too?
				actByNum[ actPointer->lpaarHolding->id ] = actPointer;

				ifsFile.getline( szLine, 127 );
				continue;
			}
			if( !braces[0] && ( braces[1] || braces[2] ) )
			{
				ifsFile.close();
				ErrNum = 0x00000001;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
				ErrDesc = "Invalid block encountered";
				return -1;
			}
			char *token[2] = { NULL, NULL };
			token[0] = strtok( szLine, " " );	token[1] = strtok( NULL, "\n" );
			if( token[1] == NULL )	// no data!
			{
				ifsFile.getline( szLine, 127 );
				continue;
			}
			if( braces[0] && braces[1] )
			{
				if( !stricmp( token[0], "TIMEBAN" ) )
				{	//	This will be downgraded, and removed
					actPointer->ban_duration = atol( token[1] );
				}
				else if( !stricmp( token[0], "LASTIP" ) )
				{
					actPointer->ipaddress[0] = inet_addr( (const char*)token[1] );
				}
				else if( !stricmp( token[0], "ID" ) )
				{
					if( ( actPointer->lpaarHolding->id = atol( token[1] ) ) == 0L )
						actPointer->lpaarHolding->id = -1;
					else
						actPointer->lpaarHolding->bFlags|=0x8000;	// Set the GM BIT 
				}
				else if( !stricmp( token[0], "CONTACT" ) || !stricmp( token[0], "EMAIL" ))
				{
					if(strlen(token[1])<=0)
						strcpy( actPointer->lpaarHolding->Info.comment, "NA" );
					else
						strcpy( actPointer->lpaarHolding->Info.comment, token[1] );
				}
				else if( !stricmp( token[0], "BANDURATION" ) )
				{	// This is the new conversion from TIMEBAN
					actPointer->ban_duration = atol( token[1] );
				}
				else if( !strnicmp( token[0], "CHARACTER-", strlen("CHARACTER-")) )
				{
					char *tl=NULL; 
					char *tr=NULL;
					UI32 serialid,charslot;
					// parse off the character that were going to copy.
					tl=strtok(token[0],"-");
					charslot=makeNum( strtok(NULL,"\n") );
					serialid=makeNum( strtok(token[1]," ") );	// We really only need this
					tr=strtok(token[1],"\n");// but might as well parse out the charname if it exists in cases someone wants to use it later
					if( serialid != INVALIDSERIAL)
					{
						actPointer->characters[charslot-1] = calcCharObjFromSer( serialid );
						// Need to set the account pointer back to this record to so inserver lookups aren't broken
						if(actPointer->characters[charslot-1]!=NULL)
						{
							actPointer->characters[charslot-1]->SetAccountObj( actPointer );
						}
						actPointer->charactercount++;
					}
				}
				else if( !stricmp( token[0], "NAME" ) )
					strcpy( actPointer->lpaarHolding->Info.username, strlwr(token[1]) );
				else if( !stricmp( token[0], "PASS" ) )
					strcpy( actPointer->lpaarHolding->Info.password, token[1] );
				else if( !stricmp( token[0], "BAN" ) )
				{
					if( makeNum( token[1] ) > 0)
						actPointer->lpaarHolding->bFlags|=0x0001; //tban=true;
					else
						actPointer->lpaarHolding->bFlags&=0xFFFE; //tban=false;
				}
				else if( !stricmp( token[0], "PUBLIC" ) )
				{
					if( !stricmp( token[1], "ON" ) )
						actPointer->lpaarHolding->bFlags|=0x0004;	//tpub=true;
					else
						actPointer->lpaarHolding->bFlags&=0xFFFB;	//tpub=false;
				}
				else if( !stricmp( token[0], "PATH" ) )
				{
					strcpy( actPointer->lpaarHolding->Info.path, token[1] );
				}
				else if( !stricmp( token[0], "XGMCLIENT" ) )
				{
					if(!stricmp( token[1], "ON" ) )
						actPointer->lpaarHolding->bFlags|=0x0002; //txgm=true;
					else
						actPointer->lpaarHolding->bFlags&=0xFFFD;	// False
				}
				ifsFile.getline( szLine, 127 );
			}
		} // while loop
	}
	else
	{
		// There was an access.adm found so now we just need to create the actrec
		// structures, associate the access map to it, and load in the ActInfo.uad
		// file data to complete the in memory storage of access/account blocks.
		AAREC_ITERATOR p;
		AAREC *temp=NULL;
		for(p=aarMap.begin();p!=aarMap.end();p++)
		{
			ACTREC *acr= new ACTREC;
			temp = p->second;
			char tbuffer[MAX_PATH];
			memset(tbuffer,0x00,sizeof(tbuffer));
			// Expects that the path value contains path and filename to actinfo.uad ins the users directory
			UI32 slen = strlen( temp->Info.path );
			if( temp->Info.path[slen-1] == '\\' || temp->Info.path[ slen -1] == '/' )
				sprintf(tbuffer,"%s%s.uad",temp->Info.path,temp->Info.username);
			else
				sprintf(tbuffer,"%s/%s.uad",temp->Info.path,temp->Info.username);
			// We have the path to the file now open and parse it into memory
			ifstream ins;
			ins.open(tbuffer);
			if( !ins.is_open() )
			{
				ErrNum = 0x00000100;	// [00]-Unknown [00]-Memory [00]-i/o [00]-syntax =0x[00][00][00][00]
				ErrDesc = "Unable to open the specified file";
				return -1;
			}
			memset(tbuffer,0x00,sizeof(tbuffer));
			// Try to read in the first line
			ins.getline(tbuffer,127);
			while(!ins.eof())
			{
				// We loop through and gather all the data here
				if( '/' == tbuffer[0] || ' ' == tbuffer[0] || 0x0D == tbuffer[0])
				{
					ins.getline(tbuffer,127);
					continue;
				}
				char *r,*l;
				l=strtok(tbuffer," ");	r=strtok(NULL,"\0");
				if(l==NULL)
				{
					ins.getline(tbuffer,127);
					continue;
				}
				else if(stricmp("BANTIME",l)==0)
				{
					time_t ban_t = atol(r);
					if( difftime( ban_t, time(NULL) ) > 0 )
					{
						// the timeban has expired so set this in memory to 0 so that next load it will unban the account
						acr->ban_duration=0l;
					}
					else
					{
						// The ban has not yet expired so we put the dureation into memory
						acr->ban_duration=ban_t;
					}
				}
				// Do we really need to
				else if( stricmp("LASTIP",l)==0)
				{
					sockaddr_in ineta;
					ineta.sin_addr.s_addr = inet_addr( r );
					acr->ipaddress[0]=ineta.sin_addr.s_addr;
				}
				else if( stricmp( "ID", l)==0)
				{
					temp->id=makeNum( r );
				}
				else if( strnicmp("CHARACTER-",l,strlen("CHARACTER-"))==0)
				{
					char *tl=NULL; 
					char *tr=NULL;
					char *trr=NULL;
					// parse off the character that were going to copy.
					tl=strtok(l,"-");
					tr=strtok(NULL," ");
					trr=strtok(r," ");
					ACTREC *alreadyExists = GetAccount( temp->Info.username );
					if( alreadyExists != NULL )
					{
						acr->charactercount = alreadyExists->charactercount;
						acr->characters[ makeNum(tr)-1 ] = alreadyExists->characters[ makeNum(tr)-1 ];
					}
					else
					{
						// If we get here they are no characters or they have to be looked upusing thier ID
						if( (acr->characters[ makeNum( tr )-1 ] = calcCharObjFromSer( makeNum( trr ) ) ) != NULL )
						{
							// If we don't already exist in memory then we need to copy our account
							// information into each character object. And increment our charcounter
							acr->lpaarHolding = temp;
							acr->characters[ makeNum( tr )-1 ]->SetAccountObj( acr );
							acr->charactercount++;
						}
					}
				}
				ins.getline(tbuffer,127);
			}

			// Make sure to copy the access record pointer into the account record
			acr->lpaarHolding=temp;
			// insert the account record into the account map.
			actMap[temp->Info.username] = acr;
			// Make sure to place a copy into the Access lookup map
			aarMap[temp->Info.username] = temp;
			// We need to add this to out lookup by number map.
			actByNum[temp->id] = acr;
			// were done with this file close it.
			ins.close();
		}
	}
	AccountCount = actMap.size();
	PostLoadProcessing();
	return 0;
}

const char * cAccounts::GetFilePath( void )
{
	if( !PathToFile.empty() )
		return PathToFile.c_str();
	return NULL;
}

void cAccounts::SetFilePath( const char *filepath )
{
	if( !FileExists( filepath ) )
		return;
	isBinary = isValid = false;
	PathToFile.erase();
	PathToFile = filepath;
	if( isReloaded )
	LoadAccounts();
}

bool cAccounts::UnloadAccounts( void )
{
	ACTREC_ITERATOR p;
	ACTIREC_ITERATOR j;
	AAREC_ITERATOR k;

	for( p = actMap.begin(); p != actMap.end(); p++ )
		delete p->second;

	// Should have been cleaning out the actbyNum map too it also has a copy of the account data in it :)
	for( j = actByNum.begin(); j != actByNum.end(); j++ )
		delete j->second;

	// While here clear up the aarMap to. as it contains login data
	for( k = aarMap.begin(); k != aarMap.end(); k++ )
		delete k->second;

	actMap.clear();
	actByNum.clear();
	aarMap.clear();

	return true;
}

long cAccounts::GetAccountCount( void )
{
	return AccountCount;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	AAREC  *cAccounts::GetAccessBlock( const char *username)
//|	Date					-	02/13/2002
//|	Developer(s)	-	EviLDeD / Unknown
//|	Company/Team	-	UOX3 Community Development Project
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Get the account object indicated by username
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o	
AAREC  *cAccounts::GetAccessBlock( const char *username)
{
	string s1( username );
	map< string, AAREC *>::iterator p = aarMap.find( s1 );
	if( p != aarMap.end() )
	{	//	Account was found
		return p->second;
	}
	else
	{	//	Account was not found
		return NULL;
	}
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	ACTREC *cAccounts::GetAccount( const char *username )
//|	Date					-	02/13/2002
//|	Developer(s)	-	EviLDeD / Unknown
//|	Company/Team	-	UOX3 Community Development Project
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Get the account object indicated by username
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o	
ACTREC *cAccounts::GetAccount( const char *username )
{
	string s1( strlwr((char*)username) );
	ACTREC_ITERATOR p = actMap.find( s1 );
	if( p != actMap.end() )
	{	//	Account was found
		return p->second;
	}
	else
	{	//	Account was not found
		return NULL;
	}
}


//o--------------------------------------------------------------------------o
//|	Function/Class-	long cAccounts::SaveAccounts( const char *filename, int mode )
//|	Date					-	02/09/2002
//|	Developer(s)	-	EviLDeD/UnKnown
//|	Company/Team	-	UOX3 Community Development Project
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Save out the current access, and accounts information
//|									to disk. Access.adm contains the basic connection info
//|									required to log in as well as take care of special 
//|									handling of clients IE: bans, XGM client etc. The 
//|									remaining information that is stored in the traditional
//|									accounts.adm file will now be stored in the accounts
//|									respective account path found inthe access.adm file. 
//|									This new file name will be actinfo.uad. This function
//|									will create account subdirectories if they do not exist.
//|									
//|	Modification	-	02/11/2002	-	EviLDeD: Decided to remove the multi saves
//|									seeing as no one was using them anyhow, so just output
//|									new format.
//|									
//|	Modification	-	02/21/2002	-	EviLDeD: There were some issues with accounts
//|									paths. So thosre will be fixed and DarkStorm suggested that
//|									lowercase be used instead of a mixture. So strlwr() was 
//|									implented.
//|									
//|	Modification	-	02/21/2002	-	EvilDeD: Also moved comments to username.uad
//|									and replaced COMMENTS in access.adm with EMAIL. The email
//|									was stripped off if the comment field had a valid email.
//|									
//|	Modification	-	02/22/2002	-	DarkStorm: Removed strlwr for the AccountsPath
//|									the Pathnames might be mixed lwr/upr case in the ini
//|									that would lead to serious *nix problems!
//|									
//|	Modification	-	04/11/2002	-	EviLDeD: Changed the directory lettering
//|									to use 3 chracters instead of 2. Make sure that characters
//|									have 3 or more characters to thier usernames.
//|
//o--------------------------------------------------------------------------o
//|	Returns				- [LONG] Count of number of accounts writen to the file.
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
long cAccounts::SaveAccounts( void )
{
	return SaveAccounts( cwmWorldState->ServerData()->GetAccessDirectory(), cwmWorldState->ServerData()->GetAccountsDirectory() );
}

long cAccounts::SaveAccounts( string AccessPath, string AccountsPath )
{	
	ofstream outStream;
	char TempString[MAX_PATH];
	ACTREC *CurrentAccount = NULL;
	ACTIREC_ITERATOR iter;

	// Please let us use lowercase names here or we'll run into the 
	// linux trap (case sensitive names)
	string sRealAccessPath = cwmWorldState->ServerData()->GetAccessDirectory();
	string sRealAccountsPath = cwmWorldState->ServerData()->GetAccountsDirectory();
	string sAccessPath = AccessPath;
	string sAccountsPath= AccountsPath;
	string sBasePath;
	if( AccessPath.size() <= 0 )
	{
		// were gonna use the default path then. kinda of error checking.
		sAccessPath = sRealAccessPath;
	}
	if( AccountsPath.size() <= 0 )
	{
		// were gonna use the default path to accounts then
		sAccountsPath = sRealAccountsPath;
	}
	// Create our directory paths to use for this call.
	if( sAccessPath[ sAccessPath.size() - 1 ] != '/' && sAccessPath[ sAccessPath.size() - 1 ] != '\\' )
		sAccessPath += '/'; 

	sAccessPath += "access.adm"; 



	if( sAccountsPath[ sAccountsPath.size() - 1 ] != '/' && sAccountsPath[ sAccountsPath.size() - 1 ] != '\\' )
		sAccountsPath += '/'; // I think it's sufficient to add a / here
	sBasePath = sAccountsPath;	
	
	outStream.open( sAccessPath.c_str(), ios::out );

	// Error handling
	if( !outStream.is_open() )
	{
		Console.Error( 1, "| ERROR: Failed to open the shared access  file %s for writing", sAccessPath.c_str() );
		Console.Error( 1, "| ERROR: Character information will not be saved" );
		return -1;
	}

	outStream << "//AV2.0" << "-UV" << VER << "-BD" << BUILD << "-DS" << time(NULL) << "-ED" << REALBUILD << "\n";
	outStream << "//------------------------------------------------------------------------------\n";
	outStream << "//access.adm[TEXT] : UOX3 uses this file for shared accounts access between servers\n";
	outStream << "//\n";
	outStream << "//   Format: \n";
	outStream << "//      SECTION ACCESS 0\n";
	outStream << "//      {\n";
	outStream << "//         NAME username\n";
	outStream << "//         PASS password\n";
	outStream << "//         PATH c:/uox3/Accounts/path2userdata/\n";
	outStream << "//         FLAGS 0x0008\n";
	outStream << "//         EMAIL NONE\n";
	outStream << "//      }\n";
	outStream << "//\n";
	outStream << "//   FLAGS: \n";
	outStream << "//      Bit:  0) Banned  1) XGMClient    2) Public   3)\n";
	outStream << "//            4)         5)              6)          7)\n";
	outStream << "//            8)         9)             10)         11)\n";
	outStream << "//           12)        13)             14)         15) GM Account\n";
	outStream << "//------------------------------------------------------------------------------\n";
	
	// Walk all existing accounts and save the information about them to access.adm  and to the account-files

	for( iter = actByNum.begin(); iter != actByNum.end(); iter++ )
	{
		CurrentAccount = iter->second;

		strlwr( CurrentAccount->lpaarHolding->Info.username );
		// EviLDeD: gonna make username case insensitive so this was just natural to do.

		outStream << "SECTION ACCESS " << CurrentAccount->lpaarHolding->id << "\n";
		outStream << "{\n";
		outStream << "NAME " << CurrentAccount->lpaarHolding->Info.username << "\n";
		outStream << "PASS " << CurrentAccount->lpaarHolding->Info.password << "\n";
		
		// Determine where we're going to store the account-information
		// 0: Each Account has an own directory
		// 1: Each Account is stored in a Directory 'AB' where ab are the first 2 chars of the account-name
		// 2: Each Account-file is stored directly in the Account-directory

		switch( 1 /*cwmWorldState->ServerData()->GetAccountIsolationValue()*/ )
		{
			case 0:	
				sAccountsPath += CurrentAccount->lpaarHolding->Info.username;
				break;

			case 1:	
				// Get the first three characters of the username

				char TwoChars[4];
				memset( TwoChars, 0x00,( sizeof(char)*4 ) );
				memcpy( TwoChars, CurrentAccount->lpaarHolding->Info.username,( sizeof(char)*3 ) );

				// Fix for linux, directory names are case sensitive

				strlwr( TwoChars );

				// Build the path for the Directory we'll save our information in
				sAccountsPath += TwoChars;
				sAccountsPath += "/";

				break;
		} // switch( )

		if( !makeDirectory( sAccountsPath.c_str() ) )

		{
			Console.Error( 1, "| ERROR: Failed to create directory", sAccountsPath.c_str() );
			Console.Error( 1, "| ERROR: Accounts Path Information will not be saved" );
			outStream << "PATH ERROR\n";
			outStream << "FLAGS 0x0001\n";
			outStream << "}\n";
			outStream.close();
			return -1;
		}

		outStream << "PATH " << sAccountsPath << "\n";
		strcpy( CurrentAccount->lpaarHolding->Info.path, sAccountsPath.c_str() );	

		// HEX-ify the flags and save them
		sprintf( TempString, "0x%04X", CurrentAccount->lpaarHolding->bFlags );
		outStream << "FLAGS " << TempString << "\n";

		char *l, *email;

		l = strtok(CurrentAccount->lpaarHolding->Info.comment,": ");
		email = strtok( NULL, " " );

		if ( email ) 
			outStream << "EMAIL " << email << "\n";
		else
			outStream << "EMAIL NONE\n";

		outStream << "}\n\n";

		// Build the correct Filename here
		//AccountPath = AccountsPath+AccountPath;
		string sWorking = sAccountsPath + CurrentAccount->lpaarHolding->Info.username;
		sWorking += ".uad";
		sAccountsPath = sBasePath;	// Reset the accounts path for the next run.

		// Then open the file and write out all information
		ofstream AccountStream( sWorking.c_str(), ios::out );

		AccountStream << "//AI2.1" << "-UV" << VER << "-BD" << BUILD << "-DS" << time(NULL) << "-ED" << REALBUILD << "\n";
		AccountStream << "//------------------------------------------------------------------------------\n";
		AccountStream << "// UAD Path: " << sAccountsPath << "\n";
		AccountStream << "//------------------------------------------------------------------------------\n";
		AccountStream << "// UOX3 uses this file to store any extra administration info\n";
		AccountStream << "//------------------------------------------------------------------------------\n";
		AccountStream << "ID " << CurrentAccount->lpaarHolding->id << "\n";
		AccountStream << "BANTIME " << CurrentAccount->ban_duration << "\n";
		AccountStream << "LASTIP " << (int)(CurrentAccount->ipaddress[1]>>24) << "." << (int)(CurrentAccount->ipaddress[1]>>16) << "." << (int)(CurrentAccount->ipaddress[1]>>8) << "." << (int)(CurrentAccount->ipaddress[1]%256) << "\n";
		
		for( UI08 i = 0; i < 5; i++ )
			AccountStream << "CHARACTER-" << (i+1) << " " << (long)( CurrentAccount->characters[i] != NULL ? CurrentAccount->characters[i]->GetSerial() : (INVALIDSERIAL) ) << " [" << (char*)( CurrentAccount->characters[ i ] != NULL ? CurrentAccount->characters[ i ]->GetName() : "INVALID" ) << "]\n"; 
		
		AccountStream << "\n";
		
		// EviLDeD: 022102: Ok by request I moved this into username.uad, and placed just EMAIL into the access.adm.
		AccountStream << "COMMENT " << CurrentAccount->lpaarHolding->Info.comment << "\n";
		AccountStream.close();
	}
	outStream.flush();
	outStream.close();


	// We return the size of the map. Actual number of accounts in the map
	return (long)actMap.size();
}

ACTREC *cAccounts::GetAccountByID( SI32 id )
{
	if( id == -1 )
		return NULL;
	ACTIREC_ITERATOR iter = actByNum.find( id );
	if( iter != actByNum.end() )
		return iter->second;
	else
		return NULL;
}

ACTREC * cAccounts::FirstAccount( void )
{
	gCI = actMap.begin();
	if( FinishedAccounts() )
		return NULL;
	return gCI->second;
}

ACTREC * cAccounts::NextAccount( void )
{
	if( FinishedAccounts() )
		return NULL;
	gCI++;
	if( FinishedAccounts() )
		return NULL;
	return gCI->second;
}

bool cAccounts::FinishedAccounts( void )
{
	return ( gCI == actMap.end() );
}

bool cAccounts::AddCharacterToAccount( SI32 accountid, CChar *object)
{
	bool exitvalue = false;
	ACTREC *holding = GetAccountByID( accountid );
	if( holding == NULL || object == NULL ) 
	{
		ErrDesc = "Unable to add character to account.";
		ErrNum = 0xFFFFFFFF;
		return exitvalue;
	}

	for( UI08 i = 0; i < 5; i++ ) 
	{
		if( holding->characters[i] == NULL )
		{	// Ok this is an empty slot we can use this one
			holding->characters[i] = object;
			holding->charactercount++;
			return true;
		}
	}
	return exitvalue;
}


//o--------------------------------------------------------------------------o
//|	Function/Class-	void cAccounts::AddAccount( string username, string password, string contact )
//|	Date					-	02/20/2002
//|	Developer(s)	-	EviLDeD/Unkown
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	AddAccount is the command that is called when a shard has 
//|									selected that it allow autoaccount creation: 
//|									ie:. INTERNALACCOUNTS 1 . 
//|									
//|	Modification	-	02/20/2002	-	Modified this to use the new accounts isolation
//|									system. 
//|									
//|	Modification	-	02/21/2002	-	Actualy implemented the new isolation system
//|									instead if just commenting about it ;-P
//|									
//|	Modification	-	04/22/2002	-	Ok sO I missed some things! Append to access.adm
//|									and create the actual account dir, AND create the username.uad file
//o--------------------------------------------------------------------------o
//|	Returns				- N/A
//o--------------------------------------------------------------------------o	
void cAccounts::AddAccount( string username, string password, string contact )
{
	ACTREC *toAdd = new ACTREC;
	AAREC *toAddAA = new AAREC;
	memset( toAdd, 0x00, sizeof( ACTREC ) );
	memset( toAddAA, 0x00, sizeof( AAREC ) );
	
	// Assign the AAREC storage to the accounts record.
	toAdd->lpaarHolding=toAddAA;
	toAdd->inworld = INVALIDSERIAL;
	toAdd->lpaarHolding->id = highestAccountNumber++;

	strcpy( toAdd->lpaarHolding->Info.username, username.c_str() );
	strcpy( toAdd->lpaarHolding->Info.password, password.c_str() );
	strcpy( toAdd->lpaarHolding->Info.comment, contact.c_str() );

	// ok well we need to append data to the end of the access.adm migth as well do it now
	string sAccessPath = cwmWorldState->ServerData()->GetAccessDirectory();
	if( sAccessPath[sAccessPath.size()-1] != '/' && sAccessPath[sAccessPath.size()-1] != '\\' )
		sAccessPath += "/access.adm";
	else
		sAccessPath += "access.adm";
	ofstream ofsOut;
	ofsOut.open( sAccessPath.c_str(), ios::out|ios::app );

	if(!ofsOut.is_open())
	{
		// file was not found so account cannot be created.
		return;
	}
	strlwr( toAdd->lpaarHolding->Info.username );
	toAdd->lpaarHolding->id = aarMap.size();
	ofsOut << "SECTION ACCESS " << toAdd->lpaarHolding->id << "\n";
	ofsOut << "{\n";
	ofsOut << "NAME " << toAdd->lpaarHolding->Info.username << "\n";
	ofsOut << "PASS " << toAdd->lpaarHolding->Info.password << "\n";

	// We need to create the path for the accounts and store it to or this will memory error
	string sAccountPath = cwmWorldState->ServerData()->GetAccountsDirectory();

	switch( 1 /*cwmWorldState->ServerData()->GetAccountIsolationValue()*/ )
	{
		case 0:	
			sAccountPath += toAdd->lpaarHolding->Info.username;
			break;

		case 1:	
			// Get the first two characters of the username

			char TwoChars[4];
			memset( TwoChars, 0x00,( sizeof(char)*4 ) );
			memcpy( TwoChars, toAdd->lpaarHolding->Info.username,( sizeof(char)*3 ) );

			// Fix for linux, directory names are case sensitive
			// dE, De, DE etc.
			strlwr( TwoChars );

			// Build the path for the Directory we'll save our information in
			sAccountPath += TwoChars;
			sAccountPath += "/";
			
			break;
	} // switch( )

	strcpy( toAdd->lpaarHolding->Info.path, sAccountPath.c_str() );

	if( !makeDirectory( sAccountPath.c_str() ) ) 
	{
			Console.Error( 1, "| ERROR: Failed to create directory", sAccountPath.c_str() );
			Console.Error( 1, "| ERROR: Accounts Path Information will not be saved" );
			ofsOut << "PATH ERROR\n";
			ofsOut << "FLAGS 0x0001\n";
			ofsOut << "}\n";
			ofsOut.close();
			return;
	}

	ofsOut << "PATH " << sAccountPath << "\n";
	strcpy( toAdd->lpaarHolding->Info.path, sAccountPath.c_str() );	

	// HEX-ify the flags and save them
	char TempString[128];
	sprintf( TempString, "0x%04X", toAdd->lpaarHolding->bFlags );
	ofsOut << "FLAGS " << TempString << "\n";

	char *l, *email;
	l = strtok(toAdd->lpaarHolding->Info.comment,": ");
	email = strtok( NULL, " " );

	if ( email ) 
		ofsOut << "EMAIL " << email << "\n";
	else
		ofsOut << "EMAIL NONE\n";

	ofsOut << "}\n\n";

	// Build the correct Filename here
	//AccountPath = AccountsPath+AccountPath;
	string sWorking = sAccountPath + toAdd->lpaarHolding->Info.username;
	sWorking += ".uad";

	ofstream AccountStream( sWorking.c_str(), ios::out );

	AccountStream << "//AI2.1" << "-UV" << VER << "-BD" << BUILD << "-DS" << time(NULL) << "-ED" << REALBUILD << "\n";
	AccountStream << "//------------------------------------------------------------------------------\n";
	AccountStream << "// UAD Path: " << sAccountPath << "\n";
	AccountStream << "//------------------------------------------------------------------------------\n";
	AccountStream << "// UOX3 uses this file to store any extra administration info\n";
	AccountStream << "//------------------------------------------------------------------------------\n";
	AccountStream << "ID " << toAdd->lpaarHolding->id << "\n";
	AccountStream << "BANTIME " << toAdd->ban_duration << "\n";
	AccountStream << "LASTIP " << (int)(toAdd->ipaddress[1]>>24) << "." << (int)(toAdd->ipaddress[1]>>16) << "." << (int)(toAdd->ipaddress[1]>>8) << "." << (int)(toAdd->ipaddress[1]%256) << "\n";
		
	for( UI08 i = 0; i < 5; i++ )
		AccountStream << "CHARACTER-" << (i+1) << " -1 [INVALID]" << "]\n"; 
		
	AccountStream << "\n";
		
	// EviLDeD: 022102: Ok by request I moved this into username.uad, and placed just EMAIL into the access.adm.
	AccountStream << "COMMENT " << toAdd->lpaarHolding->Info.comment << "\n";
	AccountStream.close();

	ofsOut.flush();
	ofsOut.close();

	// Add this account to the map.
	actMap[username] = toAdd;
	aarMap[username] = toAddAA;
	actByNum[toAdd->lpaarHolding->id] = toAdd;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	void cAccounts::DeleteAccount( string username )
//|	Date					-	2000
//|	Developer(s)	-	EviLDeD/Unknown
//|	Company/Team	-	UOX3 Community Development Project
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Remove an account from the internalmap, and stop allowing
//|									connection to the server.
//|									
//|	Modification	-	02/18/2002	-	Added a actDeletionVec vector to simplify 
//|									item removal when we remove an account. Basicaly this
//|									will be for the Maintinance thread to use to look for 
//|									item serial numbers, and validate if they should be there.
//|									
//|	Modification	-	02/18/2002	-	Also made sure that all memory that is
//|									tied to those maps we're using was freed properly.
//|									and added the aarMap support.
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
void cAccounts::DeleteAccount( string username )
{
	ACTREC_ITERATOR p;
	ACTIREC_ITERATOR i;
	AAREC_ITERATOR j;
	p = actMap.find( username.c_str() );
	if( p != actMap.end() )
	{
		// We are going to save a copy of this information so we can look up 
		// account/characetr items and remove them from the world. 
		actDeletionVector.push_back( (ACTREC*)p->second );
		// Now remove it :) and its memory
		delete p->second;
		actMap.erase( p );
		i = actByNum.find( p->second->lpaarHolding->id );
		if( i != actByNum.end() )
		{
			// Ok remove this entery. remeber that we have to remove them from the actByNum map
			delete i->second;
			actByNum.erase( i );
		}
		j=aarMap.find( username.c_str() );
		if( j != aarMap.end() )
		{
			// Ok remove the aarMap copy too
			delete j->second;
			aarMap.erase( j );
		}
	}
}

bool cAccounts::IPExists( UI08 ip1, UI08 ip2, UI08 ip3, UI08 ip4 ) const
{
	ACTREC *temp = NULL;
	UI32 targIP = calcserial( ip1, ip2, ip3, ip4 );
	CONST_ACTREC_ITERATOR p; 
	for( p = actMap.begin(); p != actMap.end(); p++ )
	{
		temp = p->second;
		if( temp->ipaddress[0] == targIP || temp->ipaddress[1] == targIP )
			return true;
	}
	return false;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	bool cAccounts::RemoveCharacterFromAccount( SI32 accountid, UI08 slot )
//|	Date					-	02/18/2002
//|	Developer(s)	-	Unkown
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
bool cAccounts::RemoveCharacterFromAccount( SI32 accountid, UI08 slot )
{
	ACTREC *temp = NULL;
	temp = GetAccountByID( accountid );
	return RemoveCharacterFromAccount( temp, slot );
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	bool cAccounts::RemoveCharacterFromAccount( ACTREC *toRemove, UI08 slot )
//|	Date					-	02/18/2002
//|	Developer(s)	-	Unknown
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Remove the actual character object from the charcaters
//|									vector in the actMap object.
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
bool cAccounts::RemoveCharacterFromAccount( ACTREC *toRemove, UI08 slot )
{
	if( slot >= 5 )
		return false;
	// EviLDeD: 021802: put this on the deletion vector so we can clean it up later with a maint. thread.
	ccharDeletionVector.push_back( (CChar*)toRemove->characters[slot] );
	//
	toRemove->charactercount--;
	for( UI08 j = slot; j < 4; j++ )
		toRemove->characters[j] = toRemove->characters[j+1];
	toRemove->characters[4] = NULL;
	return true;
}

SI32 cAccounts::Count( void ) const
{
	return actMap.size();
}

void cAccounts::PostLoadProcessing( void )
{
	ACTREC *pAcct = NULL;
	ACTREC_ITERATOR b;
	for( b = actMap.begin(); b != actMap.end(); b++ )
	{
		pAcct = b->second;
		if( pAcct != NULL )
		{
			if( pAcct->lpaarHolding->id == -1 )
				pAcct->lpaarHolding->id = highestAccountNumber++;
			actByNum[pAcct->lpaarHolding->id] = pAcct;
		}
	}
}
