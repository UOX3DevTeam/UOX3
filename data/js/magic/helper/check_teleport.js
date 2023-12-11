// Helper function to use when teleporting
function CheckTeleport( targPlayer, targLocX, targLocY, targLocZ, targWorld, targInstanceID )
{
	// Teleport player's followers
	var followerList = targPlayer.GetFollowerList();
	for( var i = 0; i < followerList.length; i++ )
	{
		var tempFollower = followerList[i];
		// Only teleport player's pets if they are set to follow
		if( ValidateObject( tempFollower ) && tempFollower.wandertype == 1 && tempFollower.InRange( timerObj, 24 ))
		{
			tempFollower.Teleport( targLocX, targLocY, targLocZ, targWorld, targInstanceID );
			tempFollower.Follow( targPlayer );
		}
	}

	// Teleport player
	targPlayer.Teleport( targLocX, targLocY, targLocZ, targWorld, targInstanceID );
}