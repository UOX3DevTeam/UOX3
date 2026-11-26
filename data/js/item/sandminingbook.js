/// <reference path="../definitions.d.ts" />
// @ts-check
/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;
	var itemOwner = GetPackOwner( iUsed, 0 );

	if( pSocket && iUsed && iUsed.isItem )
	{
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pSocket.SysMessage( GetDictionaryEntry( 1763, pSocket.language )); // That item must be in your backpack before it can be used.
		}
		else if( pUser.skills[45] < 1000 )
		{
			pSocket.SysMessage( GetDictionaryEntry( 9408, pSocket.language )); // Only a Grandmaster Miner can learn from this book.
		}
		else if( pUser.GetTag( "GatheringSand" ) == 1 )
		{
			pSocket.SysMessage( GetDictionaryEntry( 9409, pSocket.language )); // You have already learned this information.
		}
		else if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			pSocket.SysMessage( GetDictionaryEntry( 774, pSocket.language )); //That is locked down and you cannot use it
		}
		else 
		{
			pUser.SetTag( "GatheringSand", 1 );
			pSocket.SysMessage( GetDictionaryEntry( 9410, pSocket.language )); // You have learned how to mine fine sand.  Target sand areas when mining to look for fine sand.
			iUsed.Delete();
		}
	}
	return false;
}
