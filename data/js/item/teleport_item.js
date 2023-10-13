// Generic teleport script that uses morex, morey, morez, more and more0 on item
// to determine teleport destination for players who double-click item

function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	if( pSock == null || !ValidateObject( iUsed ) || !pUser.CanSee( iUsed ))
		return false;

	if( pUser.InRange( iUsed, 3 ))
	{
		var x = 0;
		var y = 0;
		var z = 0;
		var worldNum = 0;
		var instanceID = 0;
		if( iUsed.morex != 0 && iUsed.morey != 0 )
		{
			x = iUsed.morex;
			y = iUsed.morey;
			z = iUsed.morez;
			worldNum = iUsed.more;
			instanceID = iUsed.more0;

			// Teleport player's pets
			var followerList = pUser.GetFollowerList();
			for( var i = 0; i < followerList.length; i++ )
			{
				var tempFollower = followerList[i];

				// Only teleport pets if set to follow and within range
				if( ValidateObject( tempFollower ) && tempFollower.wandertype == 1 && tempFollower.InRange( pChar, 24 ))
				{
					tempFollower.Teleport( x, y, z, worldNum, instanceID );
					tempFollower.Follow( pUser );
				}
			}

			// Teleport player
			pUser.Teleport( x, y, z, worldNum, instanceID );
		}
		else
		{
			// Invalid teleport item
			Console.Warning( "Invalid setup of item using teleport_item.js (" + this.script_id + ")" );
			pSock.SysMessage( "Invalid setup of item using teleport_item.js (" + this.script_id + ")" );
		}
	}
	else
	{
		// Too far away
		pSock.SysMessage( GetDictionaryEntry( 389, pSock.language )); // That is too far away and you cannot reach it.
	}
	return false;
}

function _restorecontext_() {}