function CommandRegistration()
{
	RegisterCommand( "glow", 2, true );
	RegisterCommand( "unglow", 2, true );
}

function command_GLOW( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 258 );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( ourObj.isItem )
		ourObj.Glow( socket );
}

function command_UNGLOW( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 259 );
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	if( ourObj.isItem )
		ourObj.UnGlow( socket );
}