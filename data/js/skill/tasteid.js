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
		pSock.CustomTarget( 0, "What would you like to taste?" ); // What would you like to taste?
		pSock.SetTimer( Timer.SOCK_SKILLDELAY, pSkillDelay * 1000 );
	}
	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ValidateObject( ourObj ) && ourObj.isItem && ValidateObject( pUser ) )
	{
		if( ourObj.corpse || ourObj.dead )
		{
			pSock.SysMessage( "That's not something you can taste." ); // That's not something you can taste.
			return;
		}
		else if( ourObj.isItemHeld || ourObj.worldnumber != pUser.worldnumber || ourObj.instanceID != pUser.instanceID )
		{
			pSock.SysMessage( "You are too far away to taste that." ); // You are too far away to taste that.
			return;
		}
		else if( !pUser.InRange( ourObj, 3 ) )
		{
			pSock.SysMessage( "You are too far away to taste that." ); // You are too far away to taste that.
			return;
		}

		if( ourObj.type == 14 ) // Food Type
	    {
			if( pUser.CheckSkill( 36, 0, 1000 ) )
			{
				if( ourObj.poison )
				{
					pSock.SysMessage( "It appears to have poison smeared on it." ); // It appears to have poison smeared on it.
				}
				else
                {
                    pSock.SysMessage( "You detect nothing unusual about this substance." ); // You detect nothing unusual about this substance.
                }
			}
			else
			{
				pSock.SysMessage( "You cannot discern anything about this substance." ); // You cannot discern anything about this substance.
				return;
			}
		}
		else if( ourObj.morey >= 1) // Potions
		{
			pSock.SysMessage( "You already know what kind of potion that is." ); //  You already know what kind of potion that is.
			pSock.SysMessage( ourObj.name );
			return;
		}
		else
			pSock.SysMessage( "That's not something you can taste." ); // That's not something you can taste.
			return;
	}
	else
		pSock.SysMessage( "You feel that such an action would be inappropriate." ); // You feel that such an action would be inappropriate.
		return;
}