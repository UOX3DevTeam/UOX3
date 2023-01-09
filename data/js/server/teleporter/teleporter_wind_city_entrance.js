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
	var petList = pChar.GetPetList();
	for( var i = 0; i < petList.length; i++ )
	{
		var tempPet = petList[i];
		if( ValidateObject( tempPet ) && tempPet.InRange( pChar, 24 ))
		{
			tempPet.Teleport( 5166, 244, 15 );
			tempPet.Follow( pChar );
		}
	}

	// Teleport player to Wind
	pChar.Teleport( 5166, 244, 15 );
	return true;
}