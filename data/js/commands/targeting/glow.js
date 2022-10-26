// These commands let GMs add "glow" (light) effects to objects, including carried ones

function CommandRegistration()
{
	RegisterCommand( "glow", 2, true );
	RegisterCommand( "unglow", 2, true );
}

function command_GLOW( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 258, socket.language ); // Select item to make glowing.
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isItem )
	{
		ourObj.Glow( socket );
	}
}

function command_UNGLOW( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 259, socket.language ); // Select item to deactivate glowing.
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isItem )
	{
		ourObj.UnGlow( socket );
	}
}