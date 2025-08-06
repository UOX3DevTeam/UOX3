/// <reference path="../definitions.d.ts" />
// @ts-check
function CommandRegistration()
{
	RegisterCommand( "cnsyell", 4, true );
	RegisterCommand( "seeryell", 6, true );
	RegisterCommand( "gmyell", 8, true );
	RegisterCommand( "adminyell", 10, true );
	RegisterCommand( "yell", 8, true );
}

// Yell so all Counselors can hear it
function command_CNSYELL( socket, cmdString )
{
	if( cmdString )
	{
		Yell( socket, cmdString, GetCommandLevelVal( "CNS" ));
	}
}

// Yell so all Seers can hear it
function command_GMYELL( socket, cmdString )
{
	if( cmdString )
	{
		Yell( socket, cmdString, GetCommandLevelVal( "SEER" ));
	}
}

// Yell so all GMs can hear it
function command_GMYELL( socket, cmdString )
{
	if( cmdString )
	{
		Yell( socket, cmdString, GetCommandLevelVal( "GM" ));
	}
}

// Yell so all Admins can hear it
function command_ADMINYELL( socket, cmdString )
{
	if( cmdString )
	{
		Yell( socket, cmdString, GetCommandLevelVal( "ADMIN" ));
	}
}

// Yell so _everyone_ can hear it
function command_YELL( socket, cmdString )
{
	if( cmdString )
	{
		Yell( socket, cmdString, 0 );
	}
}
