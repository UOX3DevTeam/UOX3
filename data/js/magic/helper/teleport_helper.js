// Helper function to use when teleporting
function TeleportHelper( mChar, targX, targY, targZ, targWorld, targInstanceID, includePets )
{
	if( includePets )
	{
		// Teleport player's followers
		var followerList = mChar.GetFollowerList();
		for( var i = 0; i < followerList.length; i++ )
		{
			var tempFollower = followerList[i];
			// Only teleport player's pets if they are set to follow
			if( ValidateObject( tempFollower ) && tempFollower.wandertype == 1 && tempFollower.InRange( timerObj, 24 ))
			{
				tempFollower.Teleport( targX, targY, targZ, targWorld, targInstanceID );
				tempFollower.Follow( mChar );
			}
		}
	}

	// Teleport player
	mChar.Teleport( targX, targY, targZ, targWorld, targInstanceID );
}