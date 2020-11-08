function CommandRegistration()
{
	RegisterCommand( "buy", 1, true );
	RegisterCommand( "sell", 1, true );
	RegisterCommand( "removeshop", 2, true );
}

function command_BUY( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 233, socket.language );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
		socket.BuyFrom( ourObj );
}

function command_SELL( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 236, socket.language );
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
		socket.SellTo( ourObj );
}

function command_REMOVESHOP( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 262, socket.language );
	socket.CustomTarget( 2, targMsg );
}

function onCallback2( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		var txtMessage = GetDictionaryEntry( 367 );
		var buyLayer = ourObj.FindItemLayer( 0x1A );
		var boughtLayer = ourObj.FindItemLayer( 0x1B );
		var sellLayer = ourObj.FindItemLayer( 0x1C );
		if( buyLayer )
			buyLayer.Delete();
		if( boughtLayer )
			boughtLayer.Delete();
		if( sellLayer )
			sellLayer.Delete();
		socket.SysMessage( txtMessage );
	}
}
