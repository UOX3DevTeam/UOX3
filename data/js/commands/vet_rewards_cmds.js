/// <reference path="../../../definitions.d.ts" />
// @ts-check

function CommandRegistration()
{
	RegisterCommand( "vetreward", 0, true );
	RegisterCommand( "vetrewardtest", 5, true );
	RegisterCommand( "clearvetrewards", 5, true );
	RegisterCommand( "reloadvetrewards", 5, true );
}

function command_VETREWARD( socket, cmdString )
{
	if( socket == null )
	{
		return;
	}

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ) )
	{
		return;
	}

	TriggerEvent( 7602, "VetRewardMenu", pUser );
}

function command_VETREWARDTEST( socket, cmdString )
{
	if( socket == null )
	{
		return;
	}

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ) )
	{
		return;
	}

	if( cmdString == null || cmdString == "" )
	{
		socket.SysMessage( "Usage: 'vetrewardtest level" );
		return;
	}

	var testLevel = parseInt( cmdString, 10 );
	if( isNaN( testLevel ) || testLevel < 0 )
	{
		testLevel = 0;
	}

	if( TriggerEvent( 7602, "SetVeteranRewardTestLevel", pUser, testLevel ) )
	{
		socket.SysMessage( "Veteran reward test level set to " + testLevel + "." );
		TriggerEvent( 7602, "VetRewardMenu", pUser );
	}
	else
	{
		socket.SysMessage( "Unable to save veteran reward test level." );
	}
}

function command_CLEARVETREWARDS( socket, cmdString )
{
	if( socket == null )
	{
		return;
	}

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ) )
	{
		return;
	}

	if( TriggerEvent( 7602, "ClearVeteranRewardSave", pUser ) )
	{
		socket.SysMessage( "Veteran reward save was cleared for this character." );
	}
	else
	{
		socket.SysMessage( "Unable to clear veteran reward save." );
	}
}

function command_RELOADVETREWARDS( socket, cmdString )
{
	TriggerEvent( 7602, "ReloadVeteranRewardTable" );

	if( socket != null )
	{
		socket.SysMessage( "Veteran reward table reloaded." );
	}
}
