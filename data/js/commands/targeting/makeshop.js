// This command turns a targeted NPC into a shopkeeper

function CommandRegistration()
{
	RegisterCommand( "makeshop", 2, true );
}

function command_MAKESHOP( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 232, socket.language ); // Select the character to add shopkeeper buy containers to.
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( ValidateObject( ourObj ))
	{
		if( !ourObj.isChar )
		{
			socket.SysMessage( GetDictionaryEntry( 1069, socket.language ) ); // Target character not found...
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 1068, socket.language ) ); // The buy containers have been added.
			ourObj.isShop = true;
		}
	}
}
