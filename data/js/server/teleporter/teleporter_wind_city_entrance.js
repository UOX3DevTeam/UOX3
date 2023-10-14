// Teleporter into the City of Wind, which comes with a skill requirement
// Only characters with 71.5 Magery or above can enter the city

function onCollide( pSock, pChar, iObject )
{
	if( !ValidateObject( pChar ) || !pChar.isChar || pChar.npc )
		return false;

	// Check if character has the required skillpoints in Magery to enter
	if( pChar.skills.magery < 715 && !pChar.isGM && !pChar.isCounselor )
	{
		pSock.SysMessage( GetDictionaryEntry( 9107, pSock.language )); // You are not worthy of entrance to the city of Wind!
		return false;
	}

	// Teleport player's pets to Wind
	var followerList = pChar.GetFollowerList();
	for( var i = 0; i < followerList.length; i++ )
	{
		var tempFollower = followerList[i];
		// Only teleport player's pets if they're set to follow and within range
		if( ValidateObject( tempFollower ) && tempFollower.wandertype == 1 && tempFollower.InRange( pChar, 24 ))
		{
			tempFollower.Teleport( 5166, 244, 15 );
			tempFollower.Follow( pChar );
		}
	}

	// Teleport player to Wind
	pChar.Teleport( 5166, 244, 15 );
	return true;
}