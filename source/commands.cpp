#include "uox3.h"
#include "commands.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "CPacketSend.h"
#include "StringUtility.hpp"


CCommands *Commands			= nullptr;

auto CCommands::Startup() -> void
{
	CommandReset();
}

CCommands::~CCommands()
{
	CommandMap.clear();
	TargetMap.clear();
	JSCommandMap.clear();
	for( size_t clearIter = 0; clearIter < clearance.size(); ++clearIter )
	{
		delete clearance[clearIter];
		clearance[clearIter] = nullptr;
	}
	clearance.clear();
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::NumArguments()
//|	Date		-	3/12/2003
//|	Changes		-	4/2/2003 - Reduced to a UI08
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Number of arguments in a command
//o------------------------------------------------------------------------------------------------o
UI08 CCommands::NumArguments( void )
{
	auto secs = oldstrutil::sections( commandString, " ");
	return static_cast<UI08>( secs.size() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::Argument()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Grabs argument argNum and converts it to an integer
//o------------------------------------------------------------------------------------------------o
SI32 CCommands::Argument( UI08 argNum )
{
	SI32 retVal = 0;
	std::string tempString = CommandString( argNum + 1, argNum + 1 );
	if( !tempString.empty() )
	{
		try
		{
			retVal = std::stoi( tempString, nullptr, 0 );
		} 
		catch( const std::invalid_argument & e )
		{
			Console.Error( oldstrutil::format( "[%s] Unable to convert command argument ('%s') to integer.", e.what(), tempString.c_str() ));
		}
	}

	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::CommandString()
//|	Date		-	4/02/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Comm value
//o------------------------------------------------------------------------------------------------o
std::string CCommands::CommandString( UI08 section, UI08 end )
{
	std::string retString;
	if( end != 0 )
	{
		retString = oldstrutil::extractSection( commandString, " ", section - 1, end - 1 );
	}
	else
	{
		retString = oldstrutil::extractSection( commandString, " ", section - 1 );
	}
	return retString;
}
void CCommands::CommandString( std::string newValue )
{
	commandString = newValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::Command()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles commands sent from client
//o------------------------------------------------------------------------------------------------o
//| Changes		-	25 June, 2003
//|						Made it accept a CPITalkRequest, allowing to remove
//|						the need for Offset and unicode decoding
//o------------------------------------------------------------------------------------------------o
void CCommands::Command( CSocket *s, CChar *mChar, std::string text, bool checkSocketAccess )
{
	CommandString( oldstrutil::simplify( text ));
	if( NumArguments() < 1 )
		return;
	
	// Discard the leading command prefix
	std::string command = oldstrutil::upper( CommandString( 1, 1 ));

	JSCOMMANDMAP_ITERATOR toFind = JSCommandMap.find( command );
	if( toFind != JSCommandMap.end() )
	{
		if( toFind->second.isEnabled )
		{
			bool plClearance = false;
			if( checkSocketAccess )
			{
				plClearance = ( s->CurrcharObj()->GetCommandLevel() >= toFind->second.cmdLevelReq || s->CurrcharObj()->GetAccount().wAccountIndex == 0 );
			}
			else
			{
				plClearance = ( mChar->GetCommandLevel() >= toFind->second.cmdLevelReq || mChar->GetAccount().wAccountIndex == 0 );
			}
			// from now on, account 0 ALWAYS has admin access, regardless of command level
			if( !plClearance )
			{
				if( checkSocketAccess )
				{
					Log( command, s->CurrcharObj(), nullptr, "Insufficient clearance" );
				}
				else
				{
					Log( command, mChar, nullptr, "Insufficient clearance" );
				}
				s->SysMessage( 337 ); // Access denied.
				return;
			}
			cScript *toExecute = JSMapping->GetScript( toFind->second.scriptId );
			if( toExecute != nullptr )
			{	// All commands that execute are of the form: command_commandname (to avoid possible clashes)
#if defined( UOX_DEBUG_MODE )
				Console.Print( oldstrutil::format( "Executing JS command %s\n", command.c_str() ));
#endif
				toExecute->executeCommand( s, "command_" + command, CommandString( 2 ));
			}
			if( checkSocketAccess )
			{
				Log( command, s->CurrcharObj(), nullptr, "Cleared" );
			}
			else
			{
				Log( command, mChar, nullptr, "Cleared" );
			}
			return;
		}
	}

	TARGETMAP_ITERATOR findTarg = TargetMap.find( command );
	if( findTarg != TargetMap.end() )
	{
		bool plClearance = false;
		if( checkSocketAccess )
		{
			plClearance = ( s->CurrcharObj()->GetCommandLevel() >= findTarg->second.cmdLevelReq || s->CurrcharObj()->GetAccount().wAccountIndex == 0 );
		}
		else
		{
			plClearance = ( mChar->GetCommandLevel() >= findTarg->second.cmdLevelReq || mChar->GetAccount().wAccountIndex == 0 );
		}
		if( !plClearance )
		{
			if( checkSocketAccess )
			{
				Log( command, s->CurrcharObj(), nullptr, "Insufficient clearance" );
			}
			else
			{
				Log( command, mChar, nullptr, "Insufficient clearance" );
			}
			s->SysMessage( 337 ); // Access denied.
			return;
		}
		if( checkSocketAccess )
		{
			Log( command, s->CurrcharObj(), nullptr, "Cleared" );
		}
		else
		{
			Log( command, mChar, nullptr, "Cleared" );
		}
		switch( findTarg->second.cmdType )
		{
			case CMD_TARGET:
				s->SendTargetCursor( 0, findTarg->second.targId, 0, findTarg->second.dictEntry );
				break;
			case CMD_TARGETXYZ:
				if( NumArguments() == 4 )
				{
					s->ClickX( static_cast<SI16>( Argument( 1 )));
					s->ClickY( static_cast<SI16>( Argument( 2 )));
					s->ClickZ( static_cast<SI08>( Argument( 3 )));
					s->SendTargetCursor( 0, findTarg->second.targId, 0, findTarg->second.dictEntry );
				}
				else
				{
					s->SysMessage( 340 ); // This command takes three numbers as arguments.
				}
				break;
			case CMD_TARGETINT:
				if( NumArguments() == 2 )
				{
					s->TempInt( Argument( 1 ));
					s->SendTargetCursor( 0, findTarg->second.targId, 0, findTarg->second.dictEntry );
				}
				else
				{
					s->SysMessage( 338 ); // This command takes one number as an argument.
				}
				break;
			case CMD_TARGETTXT:
				if( NumArguments() > 1 )
				{
					s->XText( CommandString( 2 ));
					s->SendTargetCursor( 0, findTarg->second.targId, 0, findTarg->second.dictEntry );
				}
				else
				{
					s->SysMessage( 9026 ); // This command requires more arguments!
				}
				break;
		}
	}
	else
	{
		COMMANDMAP_ITERATOR toFind = CommandMap.find( command );
		if( toFind == CommandMap.end() )
		{
			bool cmdHandled = false;
			std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
			for( auto scriptTrig : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					// -1 == event doesn't exist, or returned -1
					// 0 == script returned false, 0, or nothing
					// 1 == script returned true or 1, don't execute hard code
					if( toExecute->OnCommand( s, command ) == 1 )
					{
						cmdHandled = true;
					}
				}
			}

			if( !cmdHandled )
			{
				s->SysMessage( 336 ); // Unrecognized command.
			}
			return;
		}
		else
		{
			bool plClearance = false;
			if( checkSocketAccess )
			{
				plClearance = ( s->CurrcharObj()->GetCommandLevel() >= toFind->second.cmdLevelReq || s->CurrcharObj()->GetAccount().wAccountIndex == 0 );
			}
			else
			{
				plClearance = ( mChar->GetCommandLevel() >= toFind->second.cmdLevelReq || mChar->GetAccount().wAccountIndex == 0 );
			}
			// from now on, account 0 ALWAYS has admin access, regardless of command level
			if( !plClearance )
			{
				if( checkSocketAccess )
				{
					Log( command, s->CurrcharObj(), nullptr, "Insufficient clearance" );
				}
				else
				{
					Log( command, mChar, nullptr, "Insufficient clearance" );
				}
				s->SysMessage( 337 ); // Access denied.
				return;
			}
			if( checkSocketAccess )
			{
				Log( command, s->CurrcharObj(), nullptr, "Cleared" );
			}
			else
			{
				Log( command, mChar, nullptr, "Cleared" );
			}

			switch( toFind->second.cmdType )
			{
				case CMD_FUNC:
					(*(( CMD_EXEC )toFind->second.cmd_extra )) ();
					break;
				case CMD_SOCKFUNC:
					(*(( CMD_SOCKEXEC )toFind->second.cmd_extra )) ( s );
					break;
				default:
					s->SysMessage( 346 ); // BUG: Command has a bad command type set!
					break;
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::Load()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load the command table
//o------------------------------------------------------------------------------------------------o
void CCommands::Load( void )
{
	SI16 commandCount = 0;
	CScriptSection *commands = FileLookup->FindEntry( "COMMAND_OVERRIDE", command_def );
	if( commands == nullptr )
	{
		InitClearance();
		return;
	}

	std::string tag;
	std::string data;
	std::string UTag;

	std::vector<std::string>		badCommands;
	for( tag = commands->First(); !commands->AtEnd(); tag = commands->Next() )
	{
		data						= commands->GrabData();
		COMMANDMAP_ITERATOR toFind	= CommandMap.find( tag );
		TARGETMAP_ITERATOR findTarg	= TargetMap.find( tag );
		if( toFind == CommandMap.end() && findTarg == TargetMap.end() )
		{
			badCommands.push_back( tag ); // make sure we don't index into array at -1
		}
		else
		{
			++commandCount;
			if( toFind != CommandMap.end() )
			{
				toFind->second.cmdLevelReq		= static_cast<UI08>( std::stoul( data, nullptr, 0 ));
			}
			else if( findTarg != TargetMap.end() )
			{
				findTarg->second.cmdLevelReq	= static_cast<UI08>( std::stoul( data, nullptr, 0 ));
			}
		}
		// check for commands here
	}
	if( !badCommands.empty() )
	{
		Console << myendl;
		std::for_each( badCommands.begin(), badCommands.end(), []( const std::string &entry )
		{
			Console << "Invalid command '" << entry.c_str() << "' found in commands.dfn!" << myendl;
		});
	}

	Console << "   o Loading command levels";
#if defined( UOX_DEBUG_MODE )
	Console << myendl;
#endif
	CScriptSection *cmdClearance = FileLookup->FindEntry( "COMMANDLEVELS", command_def );
	if( cmdClearance == nullptr )
	{
		InitClearance();
	}
	else
	{
		size_t currentWorking;
		for( const auto &sec : cmdClearance->collection() )
		{
			tag = sec->tag;
			data = sec->data;
			currentWorking = clearance.size();
			clearance.push_back( new CommandLevel_st );
			clearance[currentWorking]->name			= tag;
			clearance[currentWorking]->commandLevel = static_cast<UI08>( std::stoul( data, nullptr, 0 ));
		}
		std::vector<CommandLevel_st *>::const_iterator cIter;
		for( cIter = clearance.begin(); cIter != clearance.end(); ++cIter )
		{
			CommandLevel_st *ourClear = ( *cIter );
			if( ourClear )
			{
				cmdClearance = FileLookup->FindEntry( ourClear->name, command_def );
				if( cmdClearance )
				{
					for( const auto &sec : cmdClearance->collection() )
					{
						tag = sec->tag;
						data = sec->data;
						UTag = oldstrutil::upper( tag );
						if( UTag == "NICKCOLOUR" )
						{
							ourClear->nickColour = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
						}
						else if( UTag == "TITLE" )
						{
							ourClear->title = data;
						}
						else if( UTag == "DEFAULTPRIV" )
						{
							ourClear->defaultPriv = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
						}
						else if( UTag == "BODYID" )
						{
							ourClear->targBody = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
						}
						else if( UTag == "ALLSKILL" )
						{
							ourClear->allSkillVals = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
						}
						else if( UTag == "BODYCOLOUR" )
						{
							ourClear->bodyColour = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
						}
						else if( UTag == "STRIPHAIR" )
						{
							ourClear->stripOff.set( BIT_STRIPHAIR, true );
						}
						else if( UTag == "STRIPITEMS" )
						{
							ourClear->stripOff.set( BIT_STRIPITEMS, true );
						}
						else
						{
							Console << myendl << "Unknown tag in " << ourClear->name << ": " << tag << " with data of " << data << myendl;
						}
					}
				}
			}
		}
	}

	// Now we'll load our JS commands, what fun!
	CJSMappingSection *commandSection = JSMapping->GetSection( SCPT_COMMAND );
	for( cScript *ourScript = commandSection->First(); !commandSection->Finished(); ourScript = commandSection->Next() )
	{
		if( ourScript != nullptr )
		{
			ourScript->ScriptRegistration( "Command" );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::Log()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes toLog to a file
//o------------------------------------------------------------------------------------------------o
void CCommands::Log( const std::string &command, CChar *player1, CChar *player2, const std::string &extraInfo )
{
	std::string logName	= cwmWorldState->ServerData()->Directory( CSDDP_LOGS ) + "command.log";
	std::ofstream logDestination;
	logDestination.open( logName.c_str(), std::ios::out | std::ios::app );
	if( !logDestination.is_open() )
	{
		Console.Error( oldstrutil::format( "Unable to open command log file %s!", logName.c_str() ));
		return;
	}
	char dateTime[1024];
	RealTime( dateTime );

	logDestination << "[" << dateTime << "] ";
	logDestination << player1->GetName() << " (serial: " << std::hex << player1->GetSerial() << ") ";
	logDestination << "used command <" << command << ( CommandString( 2 ) != "" ? " " : "" ) << CommandString( 2 ) << "> ";
	if( ValidateObject( player2 ))
	{
		logDestination << "on player " << player2->GetName() << " (serial: " << player2->GetSerial() << " )";
	}
	logDestination << "Extra Info: " << extraInfo << std::endl;
	logDestination.close();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	*CCommands::GetClearance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the command level of a character
//o------------------------------------------------------------------------------------------------o
CommandLevel_st *CCommands::GetClearance( std::string clearName )
{
	if( clearance.empty() )
	{
		return nullptr;
	}
	CommandLevel_st *clearPointer;
	std::vector<CommandLevel_st *>::const_iterator clearIter;
	for( clearIter = clearance.begin(); clearIter != clearance.end(); ++clearIter )
	{
		clearPointer = ( *clearIter );
		if( oldstrutil::upper( clearName ) == clearPointer->name )
		{
			return clearPointer;
		}
	}
	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::GetColourByLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get a players nick color based on his command level
//o------------------------------------------------------------------------------------------------o
UI16 CCommands::GetColourByLevel( UI08 commandLevel )
{
	size_t clearanceSize = clearance.size();
	if( clearanceSize == 0 )
		return 0x005A;

	if( commandLevel > clearance[0]->commandLevel )
		return clearance[0]->nickColour;

	for( size_t counter = 0; counter < ( clearanceSize - 1 ); ++counter )
	{
		if( commandLevel <= clearance[counter]->commandLevel && commandLevel > clearance[counter+1]->commandLevel )
			return clearance[counter]->nickColour;
	}
	return clearance[clearanceSize - 1]->nickColour;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::InitClearance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initialize command levels
//o------------------------------------------------------------------------------------------------o
void CCommands::InitClearance( void )
{
	clearance.push_back( new CommandLevel_st );	// 0 -> 3
	clearance.push_back( new CommandLevel_st );
	clearance.push_back( new CommandLevel_st );
	clearance.push_back( new CommandLevel_st );

	clearance[0]->name = "ADMIN";
	clearance[1]->name = "GM";
	clearance[2]->name = "COUNSELOR";
	clearance[3]->name = "PLAYER";

	clearance[0]->title = "Admin";
	clearance[1]->title = "GM";
	clearance[2]->title = "Counselor";
	clearance[3]->title = "";

	clearance[0]->commandLevel = 5;
	clearance[1]->commandLevel = 2;
	clearance[2]->commandLevel = 1;
	clearance[3]->commandLevel = 0;

	clearance[0]->targBody = 0x03DB;
	clearance[1]->targBody = 0x03DB;
	clearance[2]->targBody = 0x03DB;
	clearance[3]->targBody = 0;

	clearance[0]->bodyColour = 0x8001;
	clearance[1]->bodyColour = 0x8021;
	clearance[2]->bodyColour = 0x8002;

	clearance[0]->defaultPriv = 0x786F;
	clearance[1]->defaultPriv = 0x786F;
	clearance[2]->defaultPriv = 0x0094;
	clearance[3]->defaultPriv = 0;

	clearance[0]->nickColour = 0x1332;
	clearance[1]->nickColour = 0x03;
	clearance[2]->nickColour = 0x03;
	clearance[3]->nickColour = 0x005A;

	clearance[0]->allSkillVals = 1000;
	clearance[1]->allSkillVals = 1000;
	clearance[2]->allSkillVals = 0;
	clearance[3]->allSkillVals = 0;

	// Strip Everything for Admins, GMs and Counselors
	clearance[0]->stripOff.set( BIT_STRIPHAIR, true );
	clearance[0]->stripOff.set( BIT_STRIPITEMS, true );
	clearance[1]->stripOff.set( BIT_STRIPHAIR, true );
	clearance[1]->stripOff.set( BIT_STRIPITEMS, true );
	clearance[2]->stripOff.set( BIT_STRIPHAIR, true );
	clearance[2]->stripOff.set( BIT_STRIPITEMS, true );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::GetClearance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Geta characters command level
//o------------------------------------------------------------------------------------------------o
CommandLevel_st *CCommands::GetClearance( UI08 commandLevel )
{
	size_t clearanceSize = clearance.size();
	if( clearanceSize == 0 )
		return nullptr;

	if( commandLevel > clearance[0]->commandLevel )
		return clearance[0];

	for( size_t counter = 0; counter < ( clearanceSize - 1 ); ++counter )
	{
		if( commandLevel <= clearance[counter]->commandLevel && commandLevel > clearance[counter+1]->commandLevel )
			return clearance[counter];
	}
	return clearance[clearanceSize - 1];
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::CommandExists()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if a command is valid
//o------------------------------------------------------------------------------------------------o
bool CCommands::CommandExists( const std::string& cmdName )
{
	COMMANDMAP_ITERATOR toFind = CommandMap.find( cmdName );
	return ( toFind != CommandMap.end() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::FirstCommand()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get first command in cmd_table
//o------------------------------------------------------------------------------------------------o
const std::string CCommands::FirstCommand( void )
{
	cmdPointer = CommandMap.begin();
	if( FinishedCommandList() )
		return "";

	return cmdPointer->first;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::NextCommand()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get next command in cmd_table
//o------------------------------------------------------------------------------------------------o
const std::string CCommands::NextCommand( void )
{
	if( FinishedCommandList() )
		return "";

	++cmdPointer;
	if( FinishedCommandList() )
		return "";

	return cmdPointer->first;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::FinishedCommandList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get end of cmd_table
//o------------------------------------------------------------------------------------------------o
bool CCommands::FinishedCommandList( void )
{
	return ( cmdPointer == CommandMap.end() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCommands::CommandDetails()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get command info
//o------------------------------------------------------------------------------------------------o
CommandMapEntry_st *CCommands::CommandDetails( const std::string& cmdName )
{
	if( !CommandExists( cmdName ))
		return nullptr;

	COMMANDMAP_ITERATOR toFind = CommandMap.find( cmdName );
	if( toFind == CommandMap.end() )
		return nullptr;

	return &( toFind->second );
}
