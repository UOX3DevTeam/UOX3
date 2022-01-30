function SkillRegistration()
{
	RegisterSkill( 36, true );	// Taste Identification
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		// Fetch skill-specific skill delay to use on failure
		var pSkillDelay = Skills[36].skillDelay;
		pSock.CustomTarget( 0, GetDictionaryEntry( 6200, pSock.language )); // What would you like to taste?
		pSock.SetTimer( Timer.SOCK_SKILLDELAY, pSkillDelay * 1000 );
	}
	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ValidateObject( ourObj ) && ourObj.isItem && ValidateObject( pUser ))
	{
		if( ourObj.corpse )
		{
			pSock.SysMessage( GetDictionaryEntry( 6201, pSock.language )); // That's not something you can taste.
			return;
		}
		else if( ourObj.isItemHeld || ourObj.worldnumber != pUser.worldnumber || ourObj.instanceID != pUser.instanceID )
		{
			pSock.SysMessage( GetDictionaryEntry( 6202, pSock.language )); // You are too far away to taste that.
			return;
		}
		else if( !pUser.InRange( ourObj, 3 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 6202, pSock.language )); // You are too far away to taste that.
			return;
		}

		if( ourObj.type == 14 ) // Food Type
	    {
			if( pUser.CheckSkill( 36, 0, 1000 ))
			{
				if( ourObj.poison )
				{
					var tempMsg = GetDictionaryEntry( 6203, pSock.language );
					pSock.SysMessage( tempMsg.replace( /%s/gi, ourObj.name )); // You sense a hint of foulness about %s.
				}
				else
                {
                    pSock.SysMessage( GetDictionaryEntry( 6204, pSock.language )); // You detect nothing unusual about this substance.
                }
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 6205, pSock.language )); // You cannot discern anything about this substance.
				return;
			}
		}
		else if( ourObj.type == 19 ) // Potions
		{
			switch( ourObj.id )
			{
				case 0x0f06: // Black Potion
					pSock.SysMessage( GetDictionaryEntry( 6206, pSock.language )); // This potion may have been made from garlic.
					break;
				case 0x0f07: // Orange Potion
					pSock.SysMessage( GetDictionaryEntry( 6207, pSock.language )); // This potion may have been made from spider silk.
					break;
				case 0x0f08: // Blue Potion
					pSock.SysMessage( GetDictionaryEntry( 6208, pSock.language )); // This potion may have been made from bloodmoss.
					break;
				case 0x0f09: // White Potion
					pSock.SysMessage( GetDictionaryEntry( 6209, pSock.language )); // This potion may have been made from mandrake.
					break;
				case 0x0f0a: // Green Potion
					var tempMsg = GetDictionaryEntry( 6203, pSock.language ); // You sense a hint of foulness about %s.
					pSock.SysMessage( tempMsg.replace( /%s/gi, ourObj.name ));
					pSock.SysMessage( GetDictionaryEntry( 6210, pSock.language )); // This potion may have been made from nightshade.
					break;
				case 0x0f0b: // Red Potion
					pSock.SysMessage( GetDictionaryEntry( 6211, pSock.language )); // This potion may have been made from black pearl.
					break;
				case 0x0f0c: // Yellow Potion
					pSock.SysMessage( GetDictionaryEntry( 6212, pSock.language )); // This potion may have been made from ginseng.
					break;
				case 0x0f0d: // Purle Potion
					pSock.SysMessage( GetDictionaryEntry( 6213, pSock.language )); // This potion may have been made from sulfurous ash.
					break;
				default:
					pSock.SysMessage( GetDictionaryEntry( 6214, pSock.language )); // This potion's ingredients are unknown to you.
					break;
			}

			var tmpMsg = GetDictionaryEntry( 6215, pSock.language ); // You already know what kind of potion that is.
			pSock.SysMessage( tmpMsg.replace( /%s/gi, ourObj.name ));
			return;
		}
		else
			pSock.SysMessage( GetDictionaryEntry( 6201, pSock.language )); // That's not something you can taste.
			return;
	}
	else
		pSock.SysMessage( GetDictionaryEntry( 2094, pSock.language )); // You feel that such an action would be inappropriate.
		return;
}