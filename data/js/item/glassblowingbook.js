function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	var itemOwner = GetPackOwner( iUsed, 0 );

	if( socket && iUsed && iUsed.isItem )
	{
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			socket.SysMessage(GetDictionaryEntry( 1763, pSock.language )); // That item must be in your backpack before it can be used.
			return false;
		}
		else if( pUser.skills[0] < 1000 )
		{
			socket.SysMessage( GetDictionaryEntry( 6301, socket.Language ) ); // Only a Grandmaster Alchemist can learn from this book.
			return false;
		}
		else if( pUser.GetTag( "GlassBlowing" ) == 1 )
		{
			socket.SysMessage( GetDictionaryEntry( 6302, socket.Language ) ); // You have already learned this information.
			return false;
		}
		else if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.Language )); //That is locked down and you cannot use it
			return false;
		}
		else 
		{
			pUser.SetTag( "GlassBlowing", 1 );
			socket.SysMessage( GetDictionaryEntry( 6303, socket.Language )); // You have learned to mine for stones.  Target mountains when mining to find stones.
			iUsed.Delete();
		}
	}
	return false;
}