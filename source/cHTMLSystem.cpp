// HTML Template

#include "uox3.h"
#include "cHTMLSystem.h"
#include "cVersionClass.h"
#include "cGuild.h"
#include "townregion.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "gump.h"
#include "scriptc.h"
#include "CPacketSend.h"
#include "ObjectFactory.h"

cHTMLTemplates *HTMLTemplates;

cHTMLTemplate::cHTMLTemplate() : UpdateTimer( 60 ), Loaded( false ), Type( ETT_INVALIDTEMPLATE ), ScheduledUpdate( 0 )
{
	Name			= "";
	Content			= "";
	OutputFile.reserve( MAX_PATH );
	InputFile.reserve( MAX_PATH );
}

cHTMLTemplate::~cHTMLTemplate()
{

}

std::string GetUptime( void )
{
	UI32 total	= (cwmWorldState->GetUICurrentTime() - cwmWorldState->GetStartTime() ) / 1000;
	UI32 ho		= total / 3600;
	total		-= ho * 3600;
	UI32 mi		= total / 60;
	total		-= mi * 60;
	UI32 se		= total;
	total		= 0;
	std::string builtString = "";
	if( ho < 10 )
	{
		builtString += std::string("0");
	}
	builtString += strutil::number( ho ) + ":";
	if( mi < 10 )
	{
		builtString += std::string("0");
	}
	builtString += strutil::number( mi ) + std::string(":");
	if( se < 10 )
	{
		builtString += std::string("0");
	}
	builtString += strutil::number( se );
	return builtString;
}

bool CountNPCFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	bool retVal = true;
	if( ValidateObject( a ) )
	{
		CChar *j = static_cast< CChar * >(a);
		if( j->IsNpc() )
			++b;
	}
	return retVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void cHTMLTemplate::Process( void )
//|	Date		-	1/18/2003 4:43:17 AM
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-
//|
//|	Changes		-	08062003 -  For the record this is some of the
//|									most fucked up shit I have EVER SEEN!! Written to truely
//|									handle multiple Templates.
//o-----------------------------------------------------------------------------------------------o
void cHTMLTemplate::Process( void )
{
	// Need to check to see if the server is actually running, of so we do not want to process the offline template.
	if(cwmWorldState->GetKeepRun() && this->GetTemplateType() == ETT_OFFLINE )
		return;

	// Only read the Status Page if it's not already loaded
	if( !Loaded )
		LoadTemplate();

	// Parse the Content...
	std::string ParsedContent = Content;

	// Replacing Placeholders

	// Account-Count
	std::string AccountCount	= strutil::number( (Accounts->size()) );
	size_t Pos				= ParsedContent.find( "%accounts" );
	while( Pos != std::string::npos )
	{
		ParsedContent.replace( Pos, 9, AccountCount );
		Pos = ParsedContent.find( "%accounts" );
	}

	// Version
	std::string Version = CVersionClass::GetVersion();
	Version += ".";
	Version += CVersionClass::GetBuild();
	Version += " [";
	Version += OS_STR;
	Version += "]";

	Pos = ParsedContent.find( "%version" );
	while( Pos != std::string::npos )
	{
		ParsedContent.replace( Pos, 8, Version );
		Pos = ParsedContent.find( "%version" );
	}
	// Character Count
	std::string CharacterCount	= strutil::number( ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ) );
	Pos						= ParsedContent.find( "%charcount" );
	while( Pos != std::string::npos )
	{
		ParsedContent.replace( Pos, 10, CharacterCount );
		Pos = ParsedContent.find( "%charcount" );
	}

	// Item Count
	std::string ItemCount	= strutil::number( ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ) );
	Pos					= ParsedContent.find( "%itemcount" );
	while( Pos != std::string::npos )
	{
		ParsedContent.replace( Pos, 10, ItemCount );
		Pos = ParsedContent.find( "%itemcount" );
	}

	// Connection Count (GMs, Counselors, Player)
	UI32 gm = 0, cns = 0, ccount = 0;
	CSocket *tSock	= nullptr;
	CChar *tChar	= nullptr;

	// Get all Network Connections
	{
		//std::scoped_lock lock(Network->internallock);
		Network->pushConn();
		for( tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
		{
			tChar = tSock->CurrcharObj();
			if( !ValidateObject( tChar ) )
				continue;

			if( tChar->IsGM() )
				++gm;
			else if( tChar->IsCounselor() )
				++cns;
			else
				++ccount;
		}
		Network->popConn();
	}

	// GMs
	std::string GMCount = strutil::number( gm );
	Pos				= ParsedContent.find( "%online_gms" );
	while( Pos != std::string::npos )
	{
		(cwmWorldState->GetKeepRun())?ParsedContent.replace( Pos, 11, GMCount ):ParsedContent.replace( Pos, 11, "0" );
		Pos = ParsedContent.find( "%online_gms" );
	}

	// Counselor
	std::string CounsiCount	= strutil::number( cns );
	Pos					= ParsedContent.find( "%online_couns" );
	while( Pos != std::string::npos )
	{
		(cwmWorldState->GetKeepRun())?ParsedContent.replace( Pos, 13, CounsiCount ):ParsedContent.replace( Pos, 13, "0");
		Pos = ParsedContent.find( "%online_couns" );
	}

	// Player
	std::string PlayerCount	= strutil::number( ccount );
	Pos					= ParsedContent.find( "%online_player" );
	while( Pos != std::string::npos )
	{
		(cwmWorldState->GetKeepRun())?ParsedContent.replace( Pos, 14, PlayerCount ):ParsedContent.replace( Pos, 14, "0" );
		Pos = ParsedContent.find( "%online_player" );
	}

	// Total
	std::string AllCount	= strutil::number( (ccount + gm + cns) );
	Pos					= ParsedContent.find( "%online_all" );
	while( Pos != std::string::npos )
	{
		(cwmWorldState->GetKeepRun())?ParsedContent.replace( Pos, 11, AllCount ):ParsedContent.replace( Pos, 11, "0" );
		Pos = ParsedContent.find( "%online_all" );
	}

	// Time
	char time_str[256];
	RealTime( time_str );
	Pos = ParsedContent.find( "%time" );
	while( Pos != std::string::npos )
	{
		(cwmWorldState->GetKeepRun())?ParsedContent.replace( Pos, 5, time_str ):ParsedContent.replace( Pos, 5, "Down" );
		Pos = ParsedContent.find( "%time" );
	}

	// 24Time
	RealTime24( time_str );
	Pos = ParsedContent.find( "%24time" );
	while( Pos != std::string::npos )
	{
		(cwmWorldState->GetKeepRun())?ParsedContent.replace( Pos, 7, time_str ):ParsedContent.replace( Pos, 7, "Down" );
		Pos = ParsedContent.find( "%24time" );
	}

	// Timestamp
	time_t currTime;
	time( &currTime );
	currTime = mktime( gmtime( &currTime ) );
	std::string timestamp = strutil::number( currTime );
	Pos = ParsedContent.find( "%tstamp" );
	while( Pos != std::string::npos )
	{
		(cwmWorldState->GetKeepRun())?ParsedContent.replace( Pos, 7, timestamp ):ParsedContent.replace( Pos, 7, "Down" );
		Pos = ParsedContent.find( "%tstamp" );
	}

	// IP(s) + PORT(s)
	UI16 ServerCount = cwmWorldState->ServerData()->ServerCount();
	if( ServerCount > 0 )
	{
		for( UI16 i = 0; i < ServerCount; ++i )
		{
			physicalServer *mServ = cwmWorldState->ServerData()->ServerEntry( i );
			auto ipToken = strutil::format( "%%ip%i", i+1 );

			if( mServ != nullptr )
			{
				Pos = ParsedContent.find( ipToken );
				while( Pos != std::string::npos )
				{
					(cwmWorldState->GetKeepRun())?ParsedContent.replace( Pos, ipToken.size(), mServ->getIP().c_str()):ParsedContent.replace( Pos, ipToken.size(), "Down" );
					Pos = ParsedContent.find( ipToken );
				}
			}

			// i think we'll never get higher than 2 digits, anyway...
			auto portToken = strutil::format( "%%port%i", i+1 );

			if( mServ != nullptr )
			{
				Pos = ParsedContent.find( portToken );
				while( Pos != std::string::npos )
				{

					auto myPort = std::to_string(mServ->getPort());

					// Both paths do the same. Something seems missing
					//if (cwmWorldState->GetKeepRun()){
						ParsedContent.replace(Pos, portToken.size(), myPort);
					//}
					//else {
						//ParsedContent.replace(Pos, portToken.size(), myPort);
					//}
					Pos = ParsedContent.find( portToken );
				}
			}

			auto serverToken= strutil::format( "%%server%i", i+1 );

			if( mServ != nullptr )
			{
				Pos = ParsedContent.find( serverToken );
				while( Pos != std::string::npos )
				{
					if (cwmWorldState->GetKeepRun()) {
						ParsedContent.replace(Pos, serverToken.size(), mServ->getName());
					}
					else {
						ParsedContent.replace(Pos, serverToken.size(), "Down");
					}
					Pos = ParsedContent.find( serverToken );
				}
			}
		}
	}
	// PLAYERLIST
	Pos = ParsedContent.find( "%playerlist%" );
	while( Pos != std::string::npos )
	{
		size_t SecondPos		= ParsedContent.find( "%playerlist%", Pos+1 );
		if( SecondPos == std::string::npos )	// there's no closing part!
			break;
		std::string myInline	= ParsedContent.substr( Pos, SecondPos - Pos + 12 );
		std::string PlayerList;
		{
			//std::scoped_lock lock(Network->internallock);

			Network->pushConn();
			for( tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
			{
				try
				{
					if( tSock != nullptr )
					{
						CChar *tChar = tSock->CurrcharObj();
						if( ValidateObject( tChar ) )
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
							size_t sPos = parsedInline.find( "%playername" );
							while( sPos != std::string::npos )
							{
								(cwmWorldState->GetKeepRun())?parsedInline.replace( sPos, 11, tChar->GetName() ):parsedInline.replace( sPos, 11, "" );
								sPos = parsedInline.find( "%playername" );
							}

							// PlayerTitle
							sPos = parsedInline.find( "%playertitle" );
							while( sPos != std::string::npos )
							{
								(cwmWorldState->GetKeepRun())?parsedInline.replace( sPos, 12, tChar->GetTitle() ):parsedInline.replace( sPos, 12, "" );
								sPos = parsedInline.find( "%playertitle" );
							}

							// PlayerIP
							sPos = parsedInline.find( "%playerip" );
							while( sPos != std::string::npos )
							{
								CSocket *mySock = tChar->GetSocket();

								auto ClientIP = strutil::format("%i.%i.%i.%i", mySock->ClientIP4(), mySock->ClientIP3(), mySock->ClientIP3(), mySock->ClientIP1() );
								(cwmWorldState->GetKeepRun())?parsedInline.replace( sPos, 9, ClientIP ):parsedInline.replace( sPos, 9, "" );
								sPos = parsedInline.find( "%playerip" );
							}

							// PlayerAccount
							sPos = parsedInline.find( "%playeraccount" );
							while( sPos != std::string::npos )
							{
								CAccountBlock& toScan = tChar->GetAccount();
								if( toScan.wAccountIndex != AB_INVALID_ID )
									(cwmWorldState->GetKeepRun())?parsedInline.replace( sPos, 14, toScan.sUsername):parsedInline.replace( sPos, 14, "" );
								sPos = parsedInline.find( "%playeraccount" );
							}

							// PlayerX
							sPos = parsedInline.find( "%playerx" );
							while( sPos != std::string::npos )
							{
								std::string myX = strutil::number( tChar->GetX() );
								(cwmWorldState->GetKeepRun())?parsedInline.replace( sPos, 8, myX ):parsedInline.replace( sPos, 8, "" );
								sPos = parsedInline.find( "%playerx" );
							}

							// PlayerY
							sPos = parsedInline.find( "%playery" );
							while( sPos != std::string::npos )
							{
								std::string myY = strutil::number( tChar->GetY() );
								(cwmWorldState->GetKeepRun())?parsedInline.replace( sPos, 8, myY ):parsedInline.replace( sPos, 8, "" );
								sPos = parsedInline.find( "%playery" );
							}

							// PlayerZ
							sPos = parsedInline.find( "%playerz" );
							while( sPos != std::string::npos )
							{
								std::string myZ = strutil::number( tChar->GetZ() );
								(cwmWorldState->GetKeepRun())?parsedInline.replace( sPos, 8, myZ ):parsedInline.replace( sPos, 8, "" );
								sPos = parsedInline.find( "%playerz" );
							}

							// PlayerRace -- needs testing
							sPos = parsedInline.find( "%playerrace" );
							while( sPos != std::string::npos )
							{
								RACEID myRace			= tChar->GetRace();
								const std::string rName	= Races->Name( myRace );
								size_t raceLenName		= rName.length();

								if( raceLenName > 0 )
									(cwmWorldState->GetKeepRun())?parsedInline.replace( sPos, 11, rName ):parsedInline.replace( sPos, 11, "");
								sPos = parsedInline.find( "%playerrace" );
							}

							// PlayerRegion
							sPos = parsedInline.find( "%playerregion" );
							while( sPos != std::string::npos )
							{
								(cwmWorldState->GetKeepRun())?parsedInline.replace( sPos, 13, tChar->GetRegion()->GetName() ):parsedInline.replace( sPos, 13, "");
								sPos = parsedInline.find( "%playerregion" );
							}

							PlayerList += parsedInline;
						}
					}
				}
				catch( ... )
				{
					Console << "| EXCEPTION: Invalid character/socket pointer found. Ignored." << myendl;
				}
			}
			Network->popConn();
		}

		(cwmWorldState->GetKeepRun())?ParsedContent.replace( Pos, myInline.length(), PlayerList ):ParsedContent.replace( Pos, myInline.length(), "");
		Pos = ParsedContent.find( "%playerlist%" );
	}

	// GuildCount
	std::string GuildCount	= strutil::number( (SI32)GuildSys->NumGuilds() );
	Pos					= ParsedContent.find( "%guildcount" );
	while( Pos != std::string::npos )
	{
		(cwmWorldState->GetKeepRun())?ParsedContent.replace( Pos, 11, GuildCount ):ParsedContent.replace( Pos, 11, "" );
		Pos = ParsedContent.find( "%guildcount" );
	}

	// GUILDLIST
	Pos = ParsedContent.find( "%guildlist%" );
	while( Pos != std::string::npos )
	{
		size_t SecondPos		= ParsedContent.find( "%guildlist%", Pos+1 );
		if( SecondPos == std::string::npos )	// can't find closing
			break;
		std::string myInline	= ParsedContent.substr( Pos, SecondPos - Pos + 11 );
		std::string GuildList;

		for( SI16 i = 0; i < (SI16)GuildSys->NumGuilds(); ++i )
		{
			std::string parsedInline = myInline;
			parsedInline.replace( 0, 11, "" );
			parsedInline.replace( parsedInline.length()-11, 11, "" );

			//			Tokens for the GuildList
			//			%guildid
			//			%guildname

			// GuildID
			size_t sPos;
			CGuild *myGuild = GuildSys->Guild( i );

			std::string GuildID	= strutil::number( i );
			sPos			= parsedInline.find( "%guildid" );
			while( sPos != std::string::npos )
			{
				(cwmWorldState->GetKeepRun())?parsedInline.replace( sPos, 8, GuildID ):parsedInline.replace( sPos, 8, "" );
				sPos = parsedInline.find( "%guildid" );
			}

			// GuildName
			sPos = parsedInline.find( "%guildname" );
			while( sPos != std::string::npos )
			{
				(cwmWorldState->GetKeepRun())?parsedInline.replace( sPos, 10, myGuild->Name() ):parsedInline.replace( sPos, 10, "" );
				sPos = parsedInline.find( "%guildname" );
			}

			GuildList += parsedInline;
		}

		(cwmWorldState->GetKeepRun())?ParsedContent.replace( Pos, myInline.length(), GuildList ):ParsedContent.replace( Pos, myInline.length(), "" );
		Pos = ParsedContent.find( "%guildlist%" );
	}

	//NPCCount
	UI32 npccount = 0;
	UI32 b		= 0;
	ObjectFactory::getSingleton().IterateOver( OT_CHAR, b, nullptr, &CountNPCFunctor );
	npccount	= b;

	std::string npcs	= strutil::number( npccount );
	Pos				= ParsedContent.find( "%npcs" );
	while( Pos != std::string::npos )
	{
		(cwmWorldState->GetKeepRun())?ParsedContent.replace( Pos, 5, npcs ):ParsedContent.replace( Pos, 5, "0" );
		Pos = ParsedContent.find( "%npcs" );
	}

	// Performance Dump
	R64 eps	= 0.00000000001;
	Pos		= ParsedContent.find( "%performance" );
	while( Pos != std::string::npos )
	{
		std::string performance;
		std::ostringstream myStream( performance );
		if( cwmWorldState->GetKeepRun() )
		{
			UI32 networkTimeCount	= cwmWorldState->ServerProfile()->NetworkTimeCount();
			UI32 timerTimeCount		= cwmWorldState->ServerProfile()->TimerTimeCount();
			UI32 autoTimeCount		= cwmWorldState->ServerProfile()->AutoTimeCount();
			UI32 loopTimeCount		= cwmWorldState->ServerProfile()->LoopTimeCount();
			myStream << "Network code: " << (R32)((R32)cwmWorldState->ServerProfile()->NetworkTime()/(R32)networkTimeCount) << "msec [" << networkTimeCount << " samples] <BR>";
			myStream << "Timer code: " << (R32)((R32)cwmWorldState->ServerProfile()->TimerTime()/(R32)timerTimeCount) << "msec [" << timerTimeCount << " samples] <BR>";
			myStream << "Auto code: " << (R32)((R32)cwmWorldState->ServerProfile()->AutoTime()/(R32)autoTimeCount) << "msec [" << autoTimeCount << " samples] <BR>";
			myStream << "Loop Time: " << (R32)((R32)cwmWorldState->ServerProfile()->LoopTime()/(R32)loopTimeCount) << "msec [" << loopTimeCount << " samples] <BR>";
			if( !( cwmWorldState->ServerProfile()->LoopTime() < eps ||  loopTimeCount < eps ) )
				myStream << "Simulation Cycles: " << (1000.0*(1.0/(R32)((R32)cwmWorldState->ServerProfile()->LoopTime()/(R32)loopTimeCount))) << " per sec <BR>";
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
		ParsedContent.replace( Pos, 12, myStream.str() );
		Pos = ParsedContent.find( "%performance" );
	}

	// Uptime
	Pos = ParsedContent.find( "%uptime" );
	while( Pos != std::string::npos )
	{
		std::string builtString = GetUptime();
		ParsedContent.replace( Pos, 7, builtString );
		Pos = ParsedContent.find( "%uptime" );
	}

	// Simulation Cycles (whatever that may be...)
	Pos = ParsedContent.find( "%simcycles" );
	while( Pos != std::string::npos )
	{
		std::string simcycles;
		std::ostringstream myStream( simcycles );

		if( cwmWorldState->GetKeepRun() )
		{
			if( !( cwmWorldState->ServerProfile()->LoopTime() < eps ||  cwmWorldState->ServerProfile()->LoopTimeCount() < eps ) )
				myStream << "Simulation Cycles: " << (1000.0*(1.0/(R32)((R32)cwmWorldState->ServerProfile()->LoopTime()/(R32)cwmWorldState->ServerProfile()->LoopTimeCount()))) << " per sec <BR>";
			else
				myStream << "Simulation Cycles: Greater than 10000 <BR> ";
		}
		else
		{
			myStream << "Simulation Cycles: 0<BR> ";
		}
		ParsedContent.replace( Pos, 10, myStream.str() );
		Pos = ParsedContent.find( "%simcycles" );
	}

	// Update Time in SECONDS
	Pos = ParsedContent.find( "%updatetime" );
	while( Pos != std::string::npos )
	{
		std::string strUpdateTimer = strutil::number( UpdateTimer );
		(cwmWorldState->GetKeepRun())?ParsedContent.replace( Pos, 11, strUpdateTimer ):ParsedContent.replace( Pos, 11, "0" );
		Pos = ParsedContent.find( "%updatetime" );
	}

	// End Replacing Placeholders

	// Print the Content out to the new file...
	std::ofstream Output;
	Output.open( OutputFile.c_str(), std::ios::out );
	if( Output.is_open() )
	{
		Output << ParsedContent;
		Output.close();
	}
	else
		Console.error( strutil::format(" Couldn't open the template file %s for writing", OutputFile.c_str()) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Poll( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates the page if needed
//o-----------------------------------------------------------------------------------------------o
void cHTMLTemplate::Poll( void )
{
	if( ScheduledUpdate < cwmWorldState->GetUICurrentTime() || !cwmWorldState->GetKeepRun() )
	{
		Process();
		ScheduledUpdate = BuildTimeValue( (R32)UpdateTimer );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadTemplate( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the Template into memory
//|
//|	Changes		-	08062003 - Updated this member function to actually handle
//|									loading the different templates for use later.
//o-----------------------------------------------------------------------------------------------o
void cHTMLTemplate::LoadTemplate( void )
{
	Content = "";

	std::ifstream InputFile1( InputFile.c_str() );

	if( !InputFile1.is_open() )
	{
		Console.error( strutil::format("Couldn't open HTML Template File %s", InputFile.c_str()) );
		return;
	}

	while( !InputFile1.eof() && !InputFile1.fail() )
	{
		std::string Line;
		std::getline( InputFile1, Line );
		Content += Line;
	}

	InputFile1.close();

	Loaded = true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void UnloadTemplate( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Unloads the Template (i.e. for reloading)
//|
//|	Changes		-	08062003 - Updated to properly unload a template
//|								and to unload the correect template, instead of just the
//|								status template.
//o-----------------------------------------------------------------------------------------------o
void cHTMLTemplate::UnloadTemplate( void )
{
	Content="";
	Loaded = false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Load( ScriptSection *found )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the HTML Template from a ScriptSection
//o-----------------------------------------------------------------------------------------------o
void cHTMLTemplate::Load( ScriptSection *found )
{
	std::string tag, data, UTag, UData, fullPath;

	for( tag = found->First(); !found->AtEnd(); tag = found->Next() )
	{
		data = found->GrabData();
		UTag = strutil::toupper( tag );

		if( UTag == "UPDATE" )
		{
			UpdateTimer = static_cast<UI32>(std::stoul(data, nullptr, 0));
		}
		else if( UTag == "TYPE" )
		{
			UData = strutil::toupper( data );
			if( UData == "STATUS" )
			{
				Type = ETT_ONLINE;
			}
			else if( UData == "OFFLINE" )
			{
				Type = ETT_OFFLINE;
			}
			else if( UData == "PLAYER" )
			{
				Type = ETT_PLAYER;
			}
			else if( UData == "GUILD" )
			{
				Type = ETT_GUILD;
			}
			else if( UData == "GMSTATUS" )
			{
				Type = ETT_GMSTATUS;
			}
		}
		else if( UTag == "INPUT" )
		{
			fullPath = cwmWorldState->ServerData()->Directory( CSDDP_DEFS ) + "html/" + data;
			//LOOKATME
			InputFile = strutil::trim(fullPath).substr( 0, MAX_PATH - 1 );
		}
		else if( UTag == "OUTPUT" )
		{
			fullPath = cwmWorldState->ServerData()->Directory( CSDDP_HTML ) + data;
			//LOOKATME
			OutputFile = strutil::trim(fullPath).substr( 0, MAX_PATH - 1 );
		}
		else if( UTag == "NAME" )
			Name = data;
	}

	ScheduledUpdate = 0;
	Loaded = false;
}

cHTMLTemplates::cHTMLTemplates()
{
	Templates.resize( 0 );
}

cHTMLTemplates::~cHTMLTemplates()
{
	Unload();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Load( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the HTML Templates from the scripts
//o-----------------------------------------------------------------------------------------------o
void cHTMLTemplates::Load( void )
{
	for( Script *toCheck = FileLookup->FirstScript( html_def ); !FileLookup->FinishedScripts( html_def ); toCheck = FileLookup->NextScript( html_def ) )
	{
		if( toCheck != nullptr )
		{
			size_t NumEntries = toCheck->NumEntries();
			if( NumEntries == 0 )
				continue;

			for( ScriptSection *found = toCheck->FirstEntry(); found != nullptr; found = toCheck->NextEntry() )
			{
				cHTMLTemplate *Template = new cHTMLTemplate();
				Template->Load( found );
				Templates.push_back( Template );
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Unload( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Unloads all Templates
//o-----------------------------------------------------------------------------------------------o
void cHTMLTemplates::Unload( void )
{
	if( Templates.empty() )
		return;

	for( size_t i = 0; i < Templates.size(); ++i )
	{
		delete Templates[ i ];
		Templates[i] = nullptr;
	}
	Templates.clear();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void Poll( ETemplateType nTemplateID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Polls the templates for updates
//o-----------------------------------------------------------------------------------------------o
void cHTMLTemplates::Poll( ETemplateType nTemplateID )
{
	std::vector< cHTMLTemplate* >::const_iterator tIter;
	for( tIter = Templates.begin(); tIter != Templates.end(); ++tIter )
	{
		cHTMLTemplate *toPoll = (*tIter);
		if( toPoll != nullptr )
		{
			if( nTemplateID == -1 || toPoll->GetTemplateType() == nTemplateID )
				toPoll->Poll();
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void TemplateInfoGump( CSocket *mySocket )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Shows an information gump about current templates
//o-----------------------------------------------------------------------------------------------o
void cHTMLTemplates::TemplateInfoGump( CSocket *mySocket )
{
	CGump InfoGump = CGump( false, false );

	InfoGump.SetSerial( 1 ); // Serial for the HTML Status Callback

	InfoGump.StartPage();
	// Add an exit button
	InfoGump.AddButton( 40, 300, cwmWorldState->ServerData()->ButtonCancel(), static_cast<UI16>( cwmWorldState->ServerData()->ButtonCancel() + 1 ), 1, 0, 0 );

	InfoGump.AddBackground( 0, 0, cwmWorldState->ServerData()->BackgroundPic(), 350, 350 );

	// 10 Max Templates per page
	UI32 Entries = 0; // Entries per page
	UI16 CurrentPage = 0; // Page

	for( UI32 i = 0; i < Templates.size(); ++i )
	{
		if( Entries == 0 )
		{
			// We should add a next button if we're not starting the first page
			if( CurrentPage != 0 )
				InfoGump.AddButton( 300, 250, cwmWorldState->ServerData()->ButtonRight(), static_cast<UI16>( cwmWorldState->ServerData()->ButtonRight() + 1 ), 0, static_cast<UI16>( CurrentPage + 1 ), 0 );

			CurrentPage = InfoGump.StartPage();
		}

		// If we're not on the first page add the "back" button
		if( CurrentPage > 1 )
			InfoGump.AddButton( 30, 250, cwmWorldState->ServerData()->ButtonLeft(), static_cast<UI16>( cwmWorldState->ServerData()->ButtonLeft() + 1 ), 0, static_cast<UI16>( CurrentPage - 1 ), 0 );

		++Entries;

		// ~25 pixel per entry

		InfoGump.AddText( 40, static_cast<UI16>( 40 + (Entries-1)*25 ), cwmWorldState->ServerData()->LeftTextColour(), strutil::format( "%s (%i)", Templates[ i ]->GetName().c_str(), i ) );

		if( Entries == 5 )
			Entries = 0;
	}

	InfoGump.Send( mySocket );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string GetName( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns the name of the Template
//o-----------------------------------------------------------------------------------------------o
std::string cHTMLTemplate::GetName( void ) const
{
	return Name;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string GetOutput( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the Output Filename
//o-----------------------------------------------------------------------------------------------o
std::string cHTMLTemplate::GetOutput( void ) const
{
	return OutputFile;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string GetInput( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the Input Filename
//o-----------------------------------------------------------------------------------------------o
std::string cHTMLTemplate::GetInput( void ) const
{
	return InputFile;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetScheduledUpdate( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the next scheduled Update time
//o-----------------------------------------------------------------------------------------------o
UI32 cHTMLTemplate::GetScheduledUpdate( void ) const
{
	return ScheduledUpdate;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetUpdateTimer( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the Update timer
//o-----------------------------------------------------------------------------------------------o
UI32 cHTMLTemplate::GetUpdateTimer( void ) const
{
	return UpdateTimer;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	ETemplateType GetTemplateType( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets the Template Type
//o-----------------------------------------------------------------------------------------------o
ETemplateType cHTMLTemplate::GetTemplateType( void ) const
{
	return Type;
}

