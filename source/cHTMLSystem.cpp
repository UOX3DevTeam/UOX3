// HTML Template

// Code added to update when shutting down  -avtotar (16/09/02) 
// Slight modification on avotars fix (19/09/02)

#include "cHTMLSystem.h"
#include "cVersionClass.h"

extern cVersionClass CVC;

cHTMLTemplate::cHTMLTemplate()
{
	
}

cHTMLTemplate::~cHTMLTemplate()
{
	
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::Process
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Parses a template
//o---------------------------------------------------------------------------o
//o--------------------------------------------------------------------------o
//|	Function			-	void cHTMLTemplate::Process( void )
//|	Date					-	1/18/2003 4:43:17 AM
//|	Developers		-	DarkStorm / EviLDeD
//|	Organization	-UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void cHTMLTemplate::Process( void )
{
	// Only read the Status Page if it's not already loaded
	if( !Loaded )
		LoadTemplate();

	// Parse the Content...
	std::string ParsedContent = Content;

	//***************************************/
	// Replacing Placeholders
	//***************************************/

	int Pos;

	// Account-Count
	char AccountCount[32];
	sprintf(AccountCount, "%d", Accounts->size());
	for( Pos = ParsedContent.find("%accounts"); Pos >= 0; Pos = ParsedContent.find( "%accounts" ) )
	{
		ParsedContent.replace( Pos, 9, AccountCount );
	}

	// Version
	std::string Version = CVC.GetVersion();
	Version += "(";
	Version += CVC.GetBuild();
	Version += ")";
	Version += " [";
	Version += OS_STR;
	Version += "]";

	for( Pos = ParsedContent.find( "%version" ); Pos >= 0; Pos = ParsedContent.find( "%version" ) )
	{
		ParsedContent.replace( Pos, 8, Version );
	}
	// Character Count
	char CharacterCount[32];
	sprintf( CharacterCount, "%d", chars.Count() );
	for( Pos = ParsedContent.find( "%charcount" ); Pos >= 0; Pos = ParsedContent.find( "%charcount" ) )
	{
		ParsedContent.replace( Pos, 10, CharacterCount );
	}

	// Item Count
	char ItemCount[32];
	sprintf( ItemCount, "%d", items.Count() );
	for( Pos = ParsedContent.find("%itemcount"); Pos >= 0; Pos = ParsedContent.find( "%itemcount" ) )
	{
		ParsedContent.replace( Pos, 10, ItemCount );
	}

	// Connection Count (GMs, Counselors, Player)
	UI32 gm = 0, cns = 0, ccount = 0;
	
	// Get all Network Connections
	Network->PushConn();
	cSocket *tSock = NULL;
	CChar *tChar = NULL;
	for( tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		tChar = tSock->CurrcharObj();
		try
		{
			if( tChar == NULL )
					continue;
		
			if( tChar->IsGM() )
				gm++;
			else if( tChar->IsCounselor() )
				cns++;
			else 
				ccount++;
		}
		catch(...)
		{
			continue;
		}
	}
	Network->PopConn();

	// GMs
	char GMCount[32];
	sprintf( GMCount, "%d", gm );
	for( Pos = ParsedContent.find( "%online_gms" ); Pos >= 0; Pos = ParsedContent.find( "%online_gms" ) )
	{
		(keeprun)?ParsedContent.replace( Pos, 11, GMCount ):ParsedContent.replace( Pos, 11, "0" );
	}

	// Counselor
	char CounsiCount[32];
	sprintf( CounsiCount, "%d", cns );
	for( Pos = ParsedContent.find( "%online_couns" ); Pos >= 0; Pos = ParsedContent.find( "%online_couns" ) )
	{
		(keeprun)?ParsedContent.replace( Pos, 13, CounsiCount ):ParsedContent.replace( Pos, 13, "0");
	}

	// Player
	char PlayerCount[32];
	sprintf( PlayerCount, "%d", ccount );
	for( Pos = ParsedContent.find( "%online_player" ); Pos >= 0; Pos = ParsedContent.find( "%online_player" ) )
	{
		(keeprun)?ParsedContent.replace( Pos, 14, PlayerCount ):ParsedContent.replace( Pos, 14, "0" );
	}

	// Total
	char AllCount[32];
	sprintf( AllCount, "%d", (ccount + gm + cns) );
	for( Pos = ParsedContent.find( "%online_all" ); Pos >= 0; Pos = ParsedContent.find( "%online_all" ) )
	{
		(keeprun)?ParsedContent.replace( Pos, 11, AllCount ):ParsedContent.replace( Pos, 11, "0" );
	}

	//RealTime( time_str )
	char time_str[80];
	RealTime( time_str );
	for( Pos = ParsedContent.find( "%time" ); Pos >= 0; Pos = ParsedContent.find( "%time" ) )
	{
		(keeprun)?ParsedContent.replace( Pos, 5, time_str ):ParsedContent.replace( Pos, 5, "Down" );
	}

	// IP(s) + PORT(s)
	UI16 ServerCount = (UI16)cwmWorldState->ServerData()->GetServerCount();
	if( ServerCount > 0 ) 
	{
		for( UI16 i = 0; i < ServerCount; i++ )
		{
			physicalServer *mServ = cwmWorldState->ServerData()->GetServerEntry( i );
			char ipToken[8]; // i think we'll never get higher than 2 digits, anyway...
			sprintf( ipToken, "%%ip%i", i+1 );

			if( mServ != NULL )	
			{
				for( Pos = ParsedContent.find( ipToken ); Pos >= 0; Pos = ParsedContent.find( ipToken ) )
				{
					(keeprun)?ParsedContent.replace( Pos, strlen(ipToken), mServ->getIP().c_str()):ParsedContent.replace( Pos, strlen(ipToken), "Down" );
				}
			}
			
			char portToken[10]; // i think we'll never get higher than 2 digits, anyway...
			sprintf( portToken, "%%port%i", i+1 );

			if( mServ != NULL )	
			{
				for( Pos = ParsedContent.find( portToken ); Pos >= 0; Pos = ParsedContent.find( portToken ) )
				{
					char myPort[5];
					sprintf( myPort, "%i", mServ->getPort() );
					(keeprun)?ParsedContent.replace( Pos, strlen( portToken ), myPort ):ParsedContent.replace( Pos, strlen( portToken ), "NA" );
				}
			}
			char serverToken[10]; // i think we'll never get higher than 2 digits, anyway...
			sprintf( serverToken, "%%server%i", i+1 );

			if( mServ != NULL )	
			{
				for( Pos = ParsedContent.find( serverToken ); Pos >= 0; Pos = ParsedContent.find( serverToken ) )
				{
					(keeprun)?ParsedContent.replace( Pos, strlen( serverToken ), mServ->getName().c_str() ):ParsedContent.replace( Pos, strlen( serverToken ), "Down" );
				}
			}
		}
	}
	// PLAYERLIST
	for( Pos = ParsedContent.find( "%playerlist%" ); Pos >= 0; Pos = ParsedContent.find( "%playerlist%" ) )
	{
		UI32 SecondPos = ParsedContent.find( "%playerlist%", Pos+1 );
		std::string myInline = ParsedContent.substr( Pos, SecondPos - Pos + 12 );
		std::string PlayerList;

		Network->PushConn();
		for( tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
		{ 
			try
			{
				if( tSock != NULL )
				{
					CChar *tChar = tSock->CurrcharObj();
					if( tChar != NULL )
					{
						std::string parsedInline = myInline;
						parsedInline.replace( 0, 12, "" );
						parsedInline.replace( parsedInline.length()-12, 12, "" );

	//					Tokens for the PlayerList
	//					%playername
	//					%playertitle
	//					%playerip
	//					%playeraccount
	//					%playerx
	//					%playery
	//					%playerz
	//					%playerrace
	//					%playerregion

						// PlayerName
						SI32 sPos;
						for( sPos = parsedInline.find( "%playername" ); sPos >= 0; sPos = parsedInline.find( "%playername" ) )
						{
							(keeprun)?parsedInline.replace( sPos, 11, tChar->GetName() ):parsedInline.replace( sPos, 11, "" );
						}

						// PlayerTitle
						for( sPos = parsedInline.find( "%playertitle" ); sPos >= 0; sPos = parsedInline.find( "%playertitle" ) )
						{
							(keeprun)?parsedInline.replace( sPos, 12, tChar->GetTitle() ):parsedInline.replace( sPos, 12, "" );
						}

						// PlayerIP
						for( sPos = parsedInline.find( "%playerip" ); sPos >= 0; sPos = parsedInline.find( "%playerip" ) )
						{
							cSocket *mySock = calcSocketObjFromChar( tChar );
							char ClientIP[32];
							sprintf( ClientIP, "%i.%i.%i.%i", mySock->ClientIP4(), mySock->ClientIP3(), mySock->ClientIP3(), mySock->ClientIP1() );
							(keeprun)?parsedInline.replace( sPos, 9, ClientIP ):parsedInline.replace( sPos, 9, "" );
						}

						// PlayerAccount
						for( sPos = parsedInline.find( "%playeraccount" ); sPos >= 0; sPos = parsedInline.find( "%playeraccount" ) )
						{
							ACCOUNTSBLOCK toScan;
							toScan=tChar->GetAccount();
							if( toScan.wAccountIndex!=AB_INVALID_ID)
								(keeprun)?parsedInline.replace( sPos, 14, toScan.sUsername):parsedInline.replace( sPos, 14, "" );
						}

						// PlayerX
						for( sPos = parsedInline.find( "%playerx" ); sPos >= 0; sPos = parsedInline.find( "%playerx" ) )
						{
							char myX[5];
							sprintf( myX, "%i", tChar->GetX() );
							(keeprun)?parsedInline.replace( sPos, 8, myX ):parsedInline.replace( sPos, 8, "" );
						}

						// PlayerY
						for( sPos = parsedInline.find( "%playery" ); sPos >= 0; sPos = parsedInline.find( "%playery" ) )
						{
							char myY[5];
							sprintf( myY, "%i", tChar->GetY() );
							(keeprun)?parsedInline.replace( sPos, 8, myY ):parsedInline.replace( sPos, 8, myY );
						}

						// PlayerZ
						for( sPos = parsedInline.find( "%playerz" ); sPos >= 0; sPos = parsedInline.find( "%playerz" ) )
						{
							char myZ[3];
							sprintf( myZ, "%i", tChar->GetZ() );
							(keeprun)?parsedInline.replace( sPos, 8, myZ ):parsedInline.replace( sPos, 8, "" );
						}

						// PlayerRace -- needs testing
						for( sPos = parsedInline.find( "%playerrace" ); sPos >= 0; sPos = parsedInline.find( "%playerrace" ) )
						{
							RACEID myRace = tChar->GetRace();
							const char *rName = Races->Name( myRace );
							UI32 raceLenName = strlen( rName );
							char *myRaceName = new char[ raceLenName + 1 ];
							strcpy( myRaceName, rName );

							if( myRaceName != NULL ) 
								(keeprun)?parsedInline.replace( sPos, 11, myRaceName ):parsedInline.replace( sPos, 11, "");
							delete [] myRaceName;
						}

						// PlayerRegion
						for( sPos = parsedInline.find( "%playerregion" ); sPos >= 0; sPos = parsedInline.find( "%playerregion" ) )
						{
							(keeprun)?parsedInline.replace( sPos, 13, region[tChar->GetRegion()]->GetName() ):parsedInline.replace( sPos, 13, "");
						}

						PlayerList += parsedInline;
					}
				}
			}
			catch(...)
			{
				Console << "| EXCEPTION: Invalid character/socket pointer found. Ignored." << myendl;
			}
		}
		Network->PopConn();	

		(keeprun)?ParsedContent.replace( Pos, myInline.length(), PlayerList ):ParsedContent.replace( Pos, myInline.length(), "");
	}

	// GuildCount
	char GuildCount[32];
	sprintf( GuildCount, "%d", GuildSys->NumGuilds() );
	for( Pos = ParsedContent.find( "%guildcount" ); Pos >= 0; Pos = ParsedContent.find( "%guildcount" ) )
	{
		(keeprun)?ParsedContent.replace( Pos, 11, PlayerCount ):ParsedContent.replace( Pos, 11, "" );
	}

	// GUILDLIST
	for( Pos = ParsedContent.find( "%guildlist%" ); Pos >= 0; Pos = ParsedContent.find( "%guildlist%" ) )
	{
		UI32 SecondPos = ParsedContent.find( "%guildlist%", Pos+1 );
		std::string myInline = ParsedContent.substr( Pos, SecondPos - Pos + 11 );
		std::string GuildList;

	
		for( SI16 i = 0; i < (SI16)GuildSys->NumGuilds(); i++ ) 
		{
			std::string parsedInline = myInline;
			parsedInline.replace( 0, 11, "" );
			parsedInline.replace( parsedInline.length()-11, 11, "" );

			//parsedInline += "Yeah it worked";

//			Tokens for the GuildList
//			%guildid
//			%guildname

			// GuildID
			SI32 sPos;
			CGuild *myGuild = GuildSys->Guild( i );

			char GuildID[6];
			sprintf( GuildID, "%d", i );
			for( sPos = parsedInline.find( "%guildid" ); sPos >= 0; sPos = parsedInline.find( "%guildid" ) )
			{
				(keeprun)?parsedInline.replace( sPos, 8, GuildID ):parsedInline.replace( sPos, 8, "" );
			}

			// GuildName
			for( sPos = parsedInline.find( "%guildname" ); sPos >= 0; sPos = parsedInline.find( "%guildname" ) )
			{
				(keeprun)?parsedInline.replace( sPos, 10, myGuild->Name() ):parsedInline.replace( sPos, 10, "" );
			}

			GuildList += parsedInline;
		}

		(keeprun)?ParsedContent.replace( Pos, myInline.length(), GuildList ):ParsedContent.replace( Pos, myInline.length(), "" );
	}

	//NPCCount
	char npcs[6];
	UI32 npccount = 0;

	if( npccount == 0 )
	{
		for( UI32 a = 0; a < chars.Count(); a++ )
		{
			if( chars[a].isFree() )
				continue;
			if( chars[a].IsNpc() )
				npccount++;
		}
	}

	sprintf(npcs, "%i", npccount);

	for( Pos = ParsedContent.find( "%npcs" ); Pos >= 0; Pos = ParsedContent.find( "%npcs" ) )
	{
		(keeprun)?ParsedContent.replace( Pos, 5, npcs ):ParsedContent.replace( Pos, 5, "0" );
	}

	// Performance Dump
	R64 eps = 0.00000000001;
	for( Pos = ParsedContent.find( "%performance" ); Pos >= 0; Pos = ParsedContent.find( "%performance" ) )
	{
		std::string performance;
		std::ostringstream myStream( performance );
		if(keeprun)
		{
			myStream << "Network code: " << (R32)((R32)networkTime/(R32)networkTimeCount) << "msec [" << networkTimeCount << " samples] <BR>";
			myStream << "Timer code: " << (R32)((R32)timerTime/(R32)timerTimeCount) << "msec [" << timerTimeCount << " samples] <BR>";
			myStream << "Auto code: " << (R32)((R32)autoTime/(R32)autoTimeCount) << "msec [" << autoTimeCount << " samples] <BR>";
			myStream << "Loop Time: " << (R32)((R32)loopTime/(R32)loopTimeCount) << "msec [" << loopTimeCount << " samples] <BR>";
			if( !( loopTime < eps ||  loopTimeCount < eps ) )
				myStream << "Simulation Cycles: " << (1000.0*(1.0/(R32)((R32)loopTime/(R32)loopTimeCount))) << " per sec <BR>";
			else 
				myStream << "Simulation Cycles: Greater than 10000 <BR> ";
		}
		else
		{
			myStream << "Network code: 0" << "<BR>";
			myStream << "Timer code: 0" << "<BR>";
			myStream << "Auto code: 0" << "<BR>";
			myStream << "Loop Time: 0" << "<BR>";
			myStream << "Simulation Cycles: 0<BR>";
		}
		ParsedContent.replace(Pos, 12, myStream.str());
	}

	// Uptime
	for( Pos = ParsedContent.find( "%uptime" ); Pos >= 0; Pos = ParsedContent.find( "%uptime" ) )
	{
		std::string uptime;
		
		char sh[3], sm[3], ss[3];
		std::ostringstream myStream( uptime );

		UI32 total, hr, min, sec;

		total = (uiCurrentTime - starttime ) / CLOCKS_PER_SEC;
		hr = total / 3600;
		if( hr < 10 && hr <= 60 ) 
			sprintf( sh,"0%i",hr );
		else 
			sprintf( sh, "%i", hr);
		total -= hr * 3600;
		min = total / 60;
		if( min < 10 && min <= 60 ) 
			sprintf( sm, "0%i", min );
		else 
			sprintf( sm, "%i", min );
		total -= min*60;
		sec = total;
		if( sec < 10 && sec <= 60 ) 
			sprintf( ss, "0%i", sec );
		else 
			sprintf( ss, "%i", sec );
		myStream << sh << ":" << sm << ":" << ss;		

		ParsedContent.replace(Pos, 7, myStream.str());
	}

	// Simulation Cycles (whatever that may be...)
	for( Pos = ParsedContent.find( "%simcycles" ); Pos >= 0; Pos = ParsedContent.find( "%simcycles" ) )
	{
		std::string simcycles;
		std::ostringstream myStream( simcycles );

		if(keeprun)
		{
			if( !( loopTime < eps ||  loopTimeCount < eps ) )
				myStream << "Simulation Cycles: " << (1000.0*(1.0/(R32)((R32)loopTime/(R32)loopTimeCount))) << " per sec <BR>";
			else 
				myStream << "Simulation Cycles: Greater than 10000 <BR> ";
		}
		else
		{
			myStream << "Simulation Cycles: 0<BR> ";
		}
		ParsedContent.replace(Pos, 10, myStream.str());
	}

	// Update Time in SECONDS
	for( Pos = ParsedContent.find( "%updatetime" ); Pos >= 0; Pos = ParsedContent.find( "%updatetime" ) )
	{
		char strUpdateTimer[32]; // Could be a big value...
		sprintf( strUpdateTimer, "%i", UpdateTimer );

		(keeprun)?ParsedContent.replace( Pos, 11, strUpdateTimer ):ParsedContent.replace( Pos, 11, "0" );
	}

	//***************************************/
	// End Replacing Placeholders
	//***************************************/	

	// Print the Content out to the new file...
	std::ofstream Output( OutputFile );
	if( !Output.is_open() ) 
	{
		Console.Error( 1, " Couldn't open the template file %s for writing", OutputFile );
		return;
	}

	Output << ParsedContent;

	Output.close();
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::Poll
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Updates the page if needed
//o---------------------------------------------------------------------------o
void cHTMLTemplate::Poll( bool Force )
{
	if( ( Force ) || ( ScheduledUpdate < uiCurrentTime ) )
	{
		Process();
		ScheduledUpdate = BuildTimeValue( (R32)UpdateTimer );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::LoadTemplate
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Loads the Template into memory
//o---------------------------------------------------------------------------o
void cHTMLTemplate::LoadTemplate( void )
{
		Content = "";

		std::ifstream InputFile1( InputFile );

		if( !InputFile1.is_open() )
		{
			Console.Error( 1, "Couldn't open HTML Template File %s", InputFile );
			return;
		}
		
		while( !InputFile1.eof() )
		{
			std::string Line;
			std::getline( InputFile1, Line );
			Content += Line;
		}

		InputFile1.close();

		Loaded = true;
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::UnloadTemplate
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Unloads the Template (i.e. for reloading)
//o---------------------------------------------------------------------------o
void cHTMLTemplate::UnloadTemplate( void )
{
	Loaded = false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::Load
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Loads the HTML Template from a ScriptSection
//o---------------------------------------------------------------------------o
void cHTMLTemplate::Load( ScriptSection *found )
{
	Type = ETT_PERIODIC;
				
	const char *tag = NULL;
	const char *data = NULL;

	for( tag = found->First(); !found->AtEnd(); tag = found->Next() )
	{
		data = found->GrabData();
		
		if( !strcmp( tag, "UPDATE" ) ) 
		{ 
			UpdateTimer = makeNum( data );
		}
		else if( !strcmp( tag, "TYPE" ) )
		{
			if( !strcmp( data, "PERIODIC" ) )
				Type = ETT_PERIODIC;
			else if( !strcmp( data, "OFFLINE" ) )
				Type = ETT_OFFLINE;
			else if( !strcmp( data, "PLAYER" ) )
				Type = ETT_PLAYER;
			else if( !strcmp( data, "GUILD" ) )
				Type = ETT_GUILD;
		}
		else if( !strcmp( tag, "INPUT" ) )
		{
			sprintf( InputFile, "%s", data );
		}
		else if( !strcmp( tag, "OUTPUT" ) )
		{
			sprintf( OutputFile, "%s", data );
		}
		else if( !strcmp( tag, "NAME" ) )
		{
			Name = data;
		}
	}
		
	ScheduledUpdate = 0;
	Loaded = false;
}

cHTMLTemplates::cHTMLTemplates()
{
}

cHTMLTemplates::~cHTMLTemplates()
{
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplates::Load
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Loads the HTML Templates from the scripts
//o---------------------------------------------------------------------------o
void cHTMLTemplates::Load( void )
{

	VECSCRIPTLIST *toWalk = FileLookup->GetFiles( html_def );
	if( toWalk == NULL )
		return;

	for( UI32 i = 0; i < toWalk->size(); i++ )
	{
		if( (*toWalk)[i] != NULL )
		{
			Script *toCheck = (*toWalk)[i];
			if( toWalk == NULL )
				continue;
			
			SI32 NumEntries = toCheck->NumEntries();
			if( NumEntries == 0 )
				continue; 

			for( ScriptSection *found = toCheck->FirstEntry(); found != NULL; found = toCheck->NextEntry() )
			{
				cHTMLTemplate *Template = new cHTMLTemplate();
				Template->Load( found );
				Templates.push_back( Template );
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplates::Unload
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Unloads all Templates
//o---------------------------------------------------------------------------o
void cHTMLTemplates::Unload( void )
{
	if( Templates.size() < 1 )
		return;

	for( UI32 i = 0; i < Templates.size(); i++ )
	{
		delete( Templates[ i ] );
	}

	Templates.clear();
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplates::Poll
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Polls the templates for updates
//o---------------------------------------------------------------------------o
void cHTMLTemplates::Poll( UI08 TemplateType )
{
	if( Templates.size() < 1 )
		return;

	for( UI32 i = 0; i < Templates.size(); i++ )
	{
		if( Templates[ i ]->GetTemplateType() != TemplateType )
			continue;

		if( Templates[ i ] != NULL )
			Templates[ i ]->Poll();
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplates::TemplateInfoGump
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Shows an information gump about current templates
//o---------------------------------------------------------------------------o
void cHTMLTemplates::TemplateInfoGump( cSocket *mySocket )
{
	CGump InfoGump = CGump( false, false );

	InfoGump.SetSerial( 1 ); // Serial for the HTML Status Callback

	InfoGump.StartPage();
	// Add an exit button
	InfoGump.AddButton( 40, 300, (UI16)cwmWorldState->ServerData()->GetButtonCancel(), (UI16)( cwmWorldState->ServerData()->GetButtonCancel() + 1 ), 1, 0, 0 );

	InfoGump.AddBackground( 0, 0, cwmWorldState->ServerData()->GetBackgroundPic(), 350, 350 );

	// 10 Max Templates per page
	UI32 Entries = 0; // Entries per page
	UI32 CurrentPage = 0; // Page

	for( UI32 i = 0; i < Templates.size(); i++ )
	{
		if( Entries == 0 )
		{
			// We should add a next button if we're not starting the first page
			if( CurrentPage != 0 )
				InfoGump.AddButton( 300, 250, (UI16)cwmWorldState->ServerData()->GetButtonRight(), (UI16)( cwmWorldState->ServerData()->GetButtonRight() + 1 ), 0, (UI16)( CurrentPage + 1 ), 0 );

			CurrentPage = InfoGump.StartPage();
		}

		// If we're not on the first page add the "back" button
		if( CurrentPage > 1 )
			InfoGump.AddButton( 30, 250, (UI16)cwmWorldState->ServerData()->GetButtonLeft(), (UI16)( cwmWorldState->ServerData()->GetButtonLeft() + 1 ), 0, (UI16)( CurrentPage - 1 ), 0 );

		Entries++;

		// ~25 pixel per entry
		char tmpName[256];
		sprintf( tmpName, "%s (%i)", Templates[ i ]->GetName().c_str(), i );

		InfoGump.AddText( 40, (UI16)( 40 + (Entries-1)*25 ), cwmWorldState->ServerData()->GetLeftTextColour(), tmpName );
		
		if( Entries == 5 )
			Entries = 0;
	}

	InfoGump.Send( mySocket );
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplates::GetName
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Returns the name of the Template
//o---------------------------------------------------------------------------o
std::string cHTMLTemplate::GetName( void )
{
	return Name; 
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplates::GetOutput
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Gets the Output Filename
//o---------------------------------------------------------------------------o
std::string cHTMLTemplate::GetOutput( void )
{
	return OutputFile; 
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplates::GetInput
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Gets the Input Filename
//o---------------------------------------------------------------------------o
std::string cHTMLTemplate::GetInput( void )
{
	return InputFile;
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplates::TemplateInfoGump
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Gets the next scheduled Update time
//o---------------------------------------------------------------------------o
UI32 cHTMLTemplate::GetScheduledUpdate( void )
{
	return ScheduledUpdate; 
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplates::GetUpdateTimer
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Gets the Update timer
//o---------------------------------------------------------------------------o
UI32 cHTMLTemplate::GetUpdateTimer( void )
{
	return UpdateTimer;
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplate::GetTemplateType
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Gets the Template Type
//o---------------------------------------------------------------------------o
UI08 cHTMLTemplate::GetTemplateType( void )
{
	return Type;   
}

//o---------------------------------------------------------------------------o
//|	Function	-	cHTMLTemplates::Refresh
//|	Programmer	-	Dark-Storm
//o---------------------------------------------------------------------------o
//|	Purpose		-	Refreshs a single template
//o---------------------------------------------------------------------------o
bool cHTMLTemplates::Refresh( UI32 TemplateID )
{
	if( Templates.size() < 1 )
		return false;

	if( TemplateID >= Templates.size() )
		return false;

	if( Templates[ TemplateID ] == NULL )
		return false;

	Templates[ TemplateID ]->Poll( true );
	
	return true;
}

