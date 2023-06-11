// Teleporter between Orc Caves levels 2 and 3
function onCollide( pSock, pChar, iObject )
{
	if( !ValidateObject( pChar ) || !pChar.isChar || pChar.npc )
		return false;

	// Freeze player to avoid them dropping to Z 0 if they take another step too quickly after teleporting
	pChar.frozen = true;
	pChar.Refresh();

	if( pChar.x == 5363 || pChar.x == 5364 )
	{
		pChar.StartTimer( 100, 0, true );
	}
	else // pChar.x == 5272 || pChar.x == 5273
	{
		pChar.StartTimer( 100, 1, true );
	}

	return true;
}

function onTimer( timerObj, timerID )
{
	if( timerID == 0 )
	{
		// Teleport player's pets to Orc Caves Level 3
		var followerList = timerObj.GetFollowerList();
		for( var i = 0; i < followerList.length; i++ )
		{
			var tempFollower = followerList[i];
			// Only teleport player's pets if they're set to follow and are within range
			if( ValidateObject( tempFollower ) && tempFollower.wandertype == 1 && tempFollower.InRange( timerObj, 24 ))
			{
				tempFollower.Teleport(( timerObj.x == 5363 ? 5272 : 5273 ), 2041, 3 );
				tempFollower.Follow( timerObj );
			}
		}

		// Teleport player to Orc Caves Level 3
		timerObj.Teleport(( timerObj.x == 5363 ? 5272 : 5273 ), 2041, 3 );
		timerObj.StartTimer( 50, 2, true );
	}
	else if( timerID == 1 )
	{
		// Teleport player's pets to Orc Caves Level 2
		var followerList = timerObj.GetFollowerList();
		for( var i = 0; i < followerList.length; i++ )
		{
			var tempFollower = followerList[i];
			if( ValidateObject( tempFollower ) && tempFollower.wandertype == 1 && tempFollower.InRange( timerObj, 24 ))
			{
				tempFollower.Teleport(( timerObj.x == 5272 ? 5363 : 5364 ), 1290, 3 );
				tempFollower.Follow( timerObj );
			}
		}

		// Teleport player to Orc Caves Level 2
		timerObj.Teleport(( timerObj.x == 5272 ? 5363 : 5364 ), 1290, 3 );
		timerObj.StartTimer( 50, 2, true );
	}
	else if( timerID == 2 )
	{
		// Unfreeze player
		timerObj.frozen = false;
		timerObj.Refresh();
	}
}