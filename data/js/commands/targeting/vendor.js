// This command is a shortcut for the "vendor buy/vendor sell" voice commands,
// and to turn shopkeeper NPCs into normal NPCs

function CommandRegistration()
{
	RegisterCommand( "buy", 1, true );
	RegisterCommand( "sell", 1, true );
	RegisterCommand( "removeshop", 2, true );
}

function command_BUY( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 233, socket.language ); // Select the shopkeeper you'd like to buy from.
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
		socket.BuyFrom( ourObj );
}

function command_SELL( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 236, socket.language ); // Select the NPC to sell to.
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		socket.SellTo( ourObj );
	}
}

function command_REMOVESHOP( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 262, socket.language ); // Select shopkeeper to remove shop packs from.
	socket.CustomTarget( 2, targMsg );
}

function onCallback2( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		var txtMessage = GetDictionaryEntry( 367 ); // The character's vendor packs have been removed.
		var buyLayer = ourObj.FindItemLayer( 0x1A );
		var boughtLayer = ourObj.FindItemLayer( 0x1B );
		var sellLayer = ourObj.FindItemLayer( 0x1C );
		if( buyLayer )
		{
			buyLayer.Delete();
		}
		if( boughtLayer )
		{
			boughtLayer.Delete();
		}
		if( sellLayer )
		{
			sellLayer.Delete();
		}
		socket.SysMessage( txtMessage );
	}
}
