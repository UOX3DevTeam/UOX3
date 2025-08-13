/// <reference path="../definitions.d.ts" />
// @ts-check
// This command deletes _ALL_ items in the world that matches specified ID

function CommandRegistration()
{
	RegisterCommand( "delid", 8, true );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_DELID( socket, cmdString )
{
	if( cmdString )
	{
		var count = 0;
		idToDelete = parseInt( cmdString );
		if( idToDelete )
		{
			count = IterateOver( "ITEM" );
		}

		var tempMsg = GetDictionaryEntry( 8011, socket.language ); // Deleted %i items
		socket.SysMessage( tempMsg.replace( /%i/gi, count.toString() ));
	}
}

/** @type { ( obj: BaseObject, mSock: Socket ) => boolean } */
function onIterate( toCheck )
{
	if( toCheck && toCheck.isItem )
	{
		if( toCheck.id == idToDelete )
		{
			toCheck.Delete();
			return true;
		}
	}
	return false;
}
