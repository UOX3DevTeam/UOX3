function onUse( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		var pHunger = pUser.hunger;
		if( pHunger < 6 )
		{
			pUser.SoundEffect( (0x003A + RandomNumber( 0, 2 )), true );
			if( pHunger >= 0 )
				socket.SysMessage( GetDictionaryEntry( (408 + pHunger), socket.Language ) ); //You eat the food, but are still extremely hungry.
			else
				socket.SysMessage( GetDictionaryEntry( 415, socket.Language ) ); //You are simply too full to eat any more!

			iPoison = iUsed.poison;
			if( iPoison && pUser.poison < iPoison )
			{
				socket.SysMessage( GetDictionaryEntry( (416 + RandomNumber( 0, 2 )), socket.Language ) ); //You feel disoriented and nauseous.
				pUser.SoundEffect( 0x0246, true );
				pUser.poison = iPoison;
				pUser.SetTimer( 5, 180000 );	// Set the Poison Timer (Need to use uox.ini eventually)
			}

			if( iUsed.amount > 1 )
				iUsed.amount = (iUsed.amount-1);
			else
				iUsed.Delete();

			pUser.hunger = (pHunger+1);
		}
		else
			socket.SysMessage( GetDictionaryEntry( 407, socket.Language ) ); //You are simply too full to eat any more!
	}
	return false;
}
