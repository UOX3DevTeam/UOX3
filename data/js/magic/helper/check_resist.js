// Helper function to calculate spell resist
function CheckResist( sourceChar, targetChar, spellCircle )
{
	// Perform a skill check for magic resistance
	var resistSuccess = targetChar.CheckSkill( 26, 80*spellCircle, 800+(80*spellCircle) );
	var targetSocket = targetChar.socket;

	if( ValidateObject( sourceChar ) )
	{
		// Check which is higher between user's normal resist chance and a fallback value
		// To ensure user always has a chance of resisting, however small their resist skill (except at 0)
		var defaultChance = targetChar.skills.magicresistance / 5;
		var resistChance = targetChar.skills.magicresistance - (((sourceChar.skills.magery - 20) / 5) + ( spellCircle * 5 ));
		if( defaultChance > resistChance )
		{
			resistChance = defaultChance;
		}

		if( RandomNumber( 1, 100) < resistChance / 10 )
		{
			if( targetSocket )
			{
				targetSocket.SysMessage( GetDictionaryEntry( 699, targetSocket.language ));
			}
			resistSuccess = true;
		}
		else
		{
			resistSuccess = false;
		}
	}
	else
	{
		if( resistSuccess )
		{
			if( targetSocket )
			{
				targetSocket.SysMessage( GetDictionaryEntry( 699, targetSocket.language ));
			}
		}
	}
	return resistSuccess;
}