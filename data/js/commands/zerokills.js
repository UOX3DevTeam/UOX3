/// <reference path="../definitions.d.ts" />
// @ts-check
function CommandRegistration()
{
	RegisterCommand( "zerokills", 8, true );
}

function command_ZEROKILLS( socket, cmdString )
{
	socket.SysMessage( GetDictionaryEntry( 22, socket.language )); // Zeroing all player kills...
	var count = IterateOver( "CHARACTER" );
	socket.SysMessage( GetDictionaryEntry( 23, socket.language )); // All player kills are now 0.
}

/** @type { ( obj: Character | Item, mSock: Socket ) => boolean } */
function onIterate( toCheck )
{
	if( toCheck && toCheck.isChar )
	{
		toCheck.murdercount = 0;
		return true;
	}
	return false;
}

function _restorecontext_() {}
