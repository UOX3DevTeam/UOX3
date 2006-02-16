#include "uox3.h"
#include "commands.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "CPacketSend.h"

#undef DBGFILE
#define DBGFILE "commands.cpp"

namespace UOX
{

cCommands *Commands = NULL;

cCommands::cCommands()
{
	CommandReset();
}

cCommands::~cCommands()
{
	CommandMap.clear();
	TargetMap.clear();
	JSCommandMap.clear();
	for( size_t clearIter = 0; clearIter < clearance.size(); ++clearIter )
	{
		delete clearance[clearIter];
		clearance[clearIter] = NULL;
	}
	clearance.clear();
}
//o---------------------------------------------------------------------------o
//|	Function		-	UI08 NumArguments( void )
//|	Date			-	3/12/2003
//|	Programmer		-	Zane
//|	Modified		-	4/2/2003 - Reduced to a UI08 - Zane
//o---------------------------------------------------------------------------o
//|	Purpose			-	Number of arguments in a command
//o---------------------------------------------------------------------------o
UI08 cCommands::NumArguments( void )
{
	return static_cast<UI08>(commandString.sectionCount( " " ) + 1 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 cCommands::Argument( UI08 argNum )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Grabs argument argNum and converts it to an integer
//o---------------------------------------------------------------------------o
SI32 cCommands::Argument( UI08 argNum )
{
	SI32 retVal = 0;
	UString tempString = CommandString( argNum + 1, argNum + 1 );
	if( !tempString.empty() )
		retVal = tempString.toLong();

	return retVal;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UString cCommands::CommandString( UI08 section, UI08 end )
//|	Date		-	4/02/2003
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets the Comm value
//o---------------------------------------------------------------------------o
UString cCommands::CommandString( UI08 section, UI08 end )
{
	UString retString;
	if( end != 0 )
		retString = commandString.section( " ", section - 1, end - 1 );
	else
		retString = commandString.section( " ", section - 1 );
	return retString;
}
void cCommands::CommandString( UString newValue )
{
	commandString = newValue;
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cCommands::Command( CSocket *s )
//|	Date			-	
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Handles commands sent from client
//o--------------------------------------------------------------------------o
//| Modifications	-	25 June, 2003
//|						Made it accept a CPITalkRequest, allowing to remove
//|						the need for Offset and unicode decoding
//o--------------------------------------------------------------------------o
void cCommands::Command( CSocket *s, CChar *mChar, UString text )
{
	CommandString( text.simplifyWhiteSpace() );
	if( NumArguments() < 1 )
		return;
	UString command = CommandString( 1, 1 ).upper();	// discard the leading '

	JSCOMMANDMAP_ITERATOR toFind = JSCommandMap.find( command );
	if( toFind != JSCommandMap.end() )
	{
		if( toFind->second.isEnabled )
		{
			bool plClearance = ( mChar->GetCommandLevel() >= toFind->second.cmdLevelReq || mChar->GetAccount().wAccountIndex == 0 );
			// from now on, account 0 ALWAYS has admin access, regardless of command level
			if( !plClearance )
			{
				Log( command, mChar, NULL, "Insufficient clearance" );
				s->sysmessage( 337 );
				return;
			}
			cScript *toExecute = JSMapping->GetScript( toFind->second.scriptID );
			if( toExecute != NULL )
			{	// All commands that execute are of the form: command_commandname (to avoid possible clashes)
#if defined( UOX_DEBUG_MODE )
				Console.Print( "Executing JS command %s\n", command.c_str() );
#endif
				toExecute->executeCommand( s, "command_" + command, CommandString( 2 ) );
			}
			Log( command, mChar, NULL, "Cleared" );
			return;
		}
	}

	TARGETMAP_ITERATOR findTarg = TargetMap.find( command );
	if( findTarg != TargetMap.end() )
	{
		bool plClearance = ( mChar->GetCommandLevel() >= findTarg->second.cmdLevelReq || mChar->GetAccount().wAccountIndex == 0 );
		if( !plClearance )
		{
			Log( command, mChar, NULL, "Insufficient clearance" );
			s->sysmessage( 337 );
			return;
		}
		Log( command, mChar, NULL, "Cleared" );
		switch( findTarg->second.cmdType )
		{
			case CMD_TARGET:
				s->target( 0, findTarg->second.targID, findTarg->second.dictEntry );
				break;
			case CMD_TARGETXYZ:
				if( NumArguments() == 4 )
				{
					s->ClickX( static_cast<SI16>(Argument( 1 )) );
					s->ClickY( static_cast<SI16>(Argument( 2 )) );
					s->ClickZ( static_cast<SI08>(Argument( 3 )) );
					s->target( 0, findTarg->second.targID, findTarg->second.dictEntry );
				}
				else
					s->sysmessage( 340 );
				break;
			case CMD_TARGETINT:
				if( NumArguments() == 2 )
				{
					s->TempInt( Argument( 1 ) );
					s->target( 0, findTarg->second.targID, findTarg->second.dictEntry );
				}
				else
					s->sysmessage( 338 );
				break;
			case CMD_TARGETTXT:
				if( NumArguments() > 1 )
				{
					s->XText( CommandString( 2 ) );
					s->target( 0, findTarg->second.targID, findTarg->second.dictEntry );
				}
				else
					s->sysmessage( "This command requires more arguments!" );
				break;
		}
	}
	else
	{
		COMMANDMAP_ITERATOR toFind = CommandMap.find( command );
		if( toFind == CommandMap.end() )
		{
			cScript *toGrab = JSMapping->GetScript( mChar->GetScriptTrigger() );
			if( toGrab == NULL || !toGrab->OnCommand( s ) )
				s->sysmessage( 336 );
			return;
		}
		else
		{
			bool plClearance = ( mChar->GetCommandLevel() >= toFind->second.cmdLevelReq || mChar->GetAccount().wAccountIndex == 0 );
			// from now on, account 0 ALWAYS has admin access, regardless of command level
			if( !plClearance )
			{
				Log( command, mChar, NULL, "Insufficient clearance" );
				s->sysmessage( 337 );
				return;
			}
			Log( command, mChar, NULL, "Cleared" );

			switch( toFind->second.cmdType )
			{
				case CMD_FUNC:
					(*((CMD_EXEC)toFind->second.cmd_extra)) ();
					break;
				case CMD_SOCKFUNC:
					(*((CMD_SOCKEXEC)toFind->second.cmd_extra)) (s);
					break;
				default:
					s->sysmessage( 346 );
					break;
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::Load( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Load the command table
//o---------------------------------------------------------------------------o
void cCommands::Load( void )
{
	SI16 commandCount = 0;
	ScriptSection *commands = FileLookup->FindEntry( "COMMAND_OVERRIDE", command_def );
	if( commands == NULL )
	{
		InitClearance();
		return;
	}

	UString tag;
	UString data;
	UString UTag;

	STRINGLIST	badCommands;
	for( tag = commands->First(); !commands->AtEnd(); tag = commands->Next() )
	{
		data						= commands->GrabData();
		COMMANDMAP_ITERATOR toFind	= CommandMap.find( tag );
		TARGETMAP_ITERATOR findTarg	= TargetMap.find( tag );
		if( toFind == CommandMap.end() && findTarg == TargetMap.end() )
			badCommands.push_back( tag ); // make sure we don't index into array at -1
		else
		{
			++commandCount;
			if( toFind != CommandMap.end() )
				toFind->second.cmdLevelReq		= data.toUByte();
			else if( findTarg != TargetMap.end() )
				findTarg->second.cmdLevelReq	= data.toUByte();
		}
		// check for commands here
	}
	if( !badCommands.empty() )
	{
		Console << myendl;
		STRINGLIST_CITERATOR tablePos = badCommands.begin();
		while( tablePos != badCommands.end() )
		{
			Console << "Invalid command '" << (*tablePos).c_str() << "' found in commands.dfn!" << myendl;
			++tablePos;
		}
	}
	
	Console << "   o Loading command levels" << myendl;
	ScriptSection *cmdClearance = FileLookup->FindEntry( "COMMANDLEVELS", command_def );
	if( cmdClearance == NULL )
		InitClearance();
	else
	{
		size_t currentWorking;
		for( tag = cmdClearance->First(); !cmdClearance->AtEnd(); tag = cmdClearance->Next() )
		{
			data			= cmdClearance->GrabData();
			currentWorking	= clearance.size();
			clearance.push_back( new commandLevel_st );
			clearance[currentWorking]->name			= tag;
			clearance[currentWorking]->commandLevel = data.toUByte();
		}
		std::vector< commandLevel_st * >::const_iterator cIter;
		for( cIter = clearance.begin(); cIter != clearance.end(); ++cIter )
		{
			commandLevel_st *ourClear = (*cIter);
			if( ourClear == NULL )
				continue;
			cmdClearance = FileLookup->FindEntry( ourClear->name, command_def );
			if( cmdClearance == NULL )
				continue;
			for( tag = cmdClearance->First(); !cmdClearance->AtEnd(); tag = cmdClearance->Next() )
			{
				UTag = tag.upper();
				data = cmdClearance->GrabData();
				if( UTag == "NICKCOLOUR" )
					ourClear->nickColour = data.toUShort();
				else if( UTag == "DEFAULTPRIV" )
					ourClear->defaultPriv = data.toUShort();
				else if( UTag == "BODYID" )
					ourClear->targBody = data.toUShort();
				else if( UTag == "ALLSKILL" )
					ourClear->allSkillVals = data.toUShort();
				else if( UTag == "BODYCOLOUR" )
					ourClear->bodyColour = data.toUShort();
				else if( UTag == "STRIPHAIR" )
					ourClear->stripOff |= 0x02;
				else if( UTag == "STRIPITEMS" )
					ourClear->stripOff |= 0x04;
				else
					Console << myendl << "Unknown tag in " << ourClear->name << ": " << tag << " with data of " << data << myendl;
			}
		}
	}

	// Now we'll load our JS commands, what fun!
	CJSMappingSection *commandSection = JSMapping->GetSection( SCPT_COMMAND );
	for( cScript *ourScript = commandSection->First(); !commandSection->Finished(); ourScript = commandSection->Next() )
	{
		if( ourScript != NULL )
			ourScript->ScriptRegistration( "Command" );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::Log( std::string command, CChar *player1, CChar *player2, std::string extraInfo )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Writes toLog to a file
//o---------------------------------------------------------------------------o
void cCommands::Log( std::string command, CChar *player1, CChar *player2, std::string extraInfo )
{
	std::string logName	= cwmWorldState->ServerData()->Directory( CSDDP_LOGS ) + "command.log";
	std::ofstream logDestination;
	logDestination.open( logName.c_str(), std::ios::out | std::ios::app );
	if( !logDestination.is_open() )
	{
		Console.Error( 1, "Unable to open command log file %s!", logName.c_str() );
		return;
	}
	char dateTime[1024];
	RealTime( dateTime );

	logDestination << "[" << dateTime << "] ";
	logDestination << player1->GetName() << " (serial: " << std::hex << player1->GetSerial() << " ) ";
	logDestination << "used command <" << command << "> ";
	if( ValidateObject( player2 ) )
		logDestination << "on player " << player2->GetName() << " (serial: " << player2->GetSerial() << " ) ";
	logDestination << "Extra Info: " << extraInfo << std::endl;
	logDestination.close();
}

//o---------------------------------------------------------------------------o
//|	Function	-	commandLevel_st *cCommands::GetClearance( UString clearName )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the command level of a character
//o---------------------------------------------------------------------------o
commandLevel_st *cCommands::GetClearance( UString clearName )
{
	if( clearance.empty() )
		return NULL;
	commandLevel_st *clearPointer;
	std::vector< commandLevel_st * >::const_iterator clearIter;
	for( clearIter = clearance.begin(); clearIter != clearance.end(); ++clearIter )
	{
		clearPointer = (*clearIter);
		if( clearName.upper() == clearPointer->name )
			return clearPointer;
	}
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI16 cCommands::GetColourByLevel( UI08 commandLevel )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get a players nick color based on his command level
//o---------------------------------------------------------------------------o
UI16 cCommands::GetColourByLevel( UI08 commandLevel )
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

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::InitClearance( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Initialize command levels
//o---------------------------------------------------------------------------o
void cCommands::InitClearance( void )
{
	clearance.push_back( new commandLevel_st );	// 0 -> 2
	clearance.push_back( new commandLevel_st );
	clearance.push_back( new commandLevel_st );

	clearance[0]->name = "GM";
	clearance[1]->name = "COUNSELOR";
	clearance[2]->name = "PLAYER";

	clearance[0]->commandLevel = 2;
	clearance[1]->commandLevel = 1;
	clearance[2]->commandLevel = 0;

	clearance[0]->targBody = 0x03DB;
	clearance[1]->targBody = 0x03DB;
	clearance[2]->targBody = 0;

	clearance[0]->bodyColour = 0x8021;
	clearance[1]->bodyColour = 0x8002;

	clearance[0]->defaultPriv = 0xFD;
	clearance[1]->defaultPriv = 0x8DB6;
	clearance[2]->defaultPriv = 0;

	clearance[0]->nickColour = 0x03;
	clearance[1]->nickColour = 0x03;
	clearance[2]->nickColour = 0x005A;

	clearance[0]->allSkillVals = 1000;
	clearance[1]->allSkillVals = 0;
	clearance[2]->allSkillVals = 0;

	clearance[0]->stripOff |= 0x06;	// Strip Everything
	clearance[1]->stripOff |= 0x06;	// Strip Everything
}

//o---------------------------------------------------------------------------o
//|	Function	-	commandLevel_st *cCommands::GetClearance( UI08 commandLevel )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Geta characters command level
//o---------------------------------------------------------------------------o
commandLevel_st *cCommands::GetClearance( UI08 commandLevel )
{
	size_t clearanceSize = clearance.size();
	if( clearanceSize == 0 )
		return NULL;
	if( commandLevel > clearance[0]->commandLevel )
		return clearance[0];

	for( size_t counter = 0; counter < ( clearanceSize - 1 ); ++counter )
	{
		if( commandLevel <= clearance[counter]->commandLevel && commandLevel > clearance[counter+1]->commandLevel )
			return clearance[counter];
	}
	return clearance[clearanceSize - 1];
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool cCommands::CommandExists( const std::string cmdName )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if a command is valid
//o---------------------------------------------------------------------------o
bool cCommands::CommandExists( const std::string cmdName )
{
	COMMANDMAP_ITERATOR toFind = CommandMap.find( cmdName );
	return ( toFind != CommandMap.end() );
}

//o---------------------------------------------------------------------------o
//|	Function	-	const std::string cCommands::FirstCommand( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get first command in cmd_table
//o---------------------------------------------------------------------------o
const std::string cCommands::FirstCommand( void )
{
	cmdPointer = CommandMap.begin();
	if( FinishedCommandList() )
		return "";
	return cmdPointer->first;
}

//o---------------------------------------------------------------------------o
//|	Function	-	const std::string cCommands::NextCommand( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get next command in cmd_table
//o---------------------------------------------------------------------------o
const std::string cCommands::NextCommand( void )
{
	if( FinishedCommandList() )
		return "";
	++cmdPointer;
	if( FinishedCommandList() )
		return "";
	return cmdPointer->first;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool cCommands::FinishedCommandList( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get end of cmd_table
//o---------------------------------------------------------------------------o
bool cCommands::FinishedCommandList( void )
{
	return ( cmdPointer == CommandMap.end() );
}

//o---------------------------------------------------------------------------o
//|	Function	-	cmdtable_mapentry *cCommands::CommandDetails( const std::string cmdName )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get command info
//o---------------------------------------------------------------------------o
CommandMapEntry *cCommands::CommandDetails( const std::string cmdName )
{
	if( !CommandExists( cmdName ) )
		return NULL;
	COMMANDMAP_ITERATOR toFind = CommandMap.find( cmdName );
	if( toFind == CommandMap.end() )
		return NULL;
	return &(toFind->second);
}

}
