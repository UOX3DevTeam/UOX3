function SkillRegistration()
{
	RegisterSkill( 16, true );	// Evaluate Intel
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		pSock.CustomTarget( 0, GetDictionaryEntry( 858, pSock.language ));
	}

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ourObj && ourObj.isChar && pUser )
	{
		if( ourObj.dead )
		{
			pSock.SysMessage( GetDictionaryEntry( 1571, pSock.language )); // That does not appear to be a living being.
		}
		else if( ourObj == pUser )
		{
			pSock.SysMessage( GetDictionaryEntry( 9080, pSock.language )); // Hmm, that person looks really silly.
		}
		else if( !ourObj.InRange( pUser, 7 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 393, pSock.language )); // That is too far away.
		}
		else if( !pUser.CheckSkill( 16, 0, 1000 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 1504, pSock.language )); // You are not certain...
		}
		else
		{
			// Add in town crier when such NPCs are identifiable as town criers
			//if( ourObj == towncrier )
			//	pSock.SysMessage( GetDictionaryEntry( 9077, pSock.language )); // He looks smart enough to remember the news.  Ask him about it.
			if( ourObj.isShop )
			{
				pSock.SysMessage( GetDictionaryEntry( 9079, pSock.language )); // That person could probably calculate the cost of what you buy from them.
			}
			else
			{
				var ourInt = ourObj.intelligence;
				var intString;
				var outMessage;
				if( ourInt < 100 )
				{
					intString = GetDictionaryEntry(( parseInt( ourInt / 10 ) + 1558 ), pSock.language ); // slightly less intelligent than a rock
				}
				else
				{
					intString = GetDictionaryEntry( 1567, pSock.language ); // superhumanly intelligent in a manner you cannot comprehend
				}
				outMessage = GetDictionaryEntry( 1568, pSock.language ); // That person looks %s.
				pSock.SysMessage( outMessage.replace( /%s/gi, intString ));

				if( pUser.skills.evaluatingintel >= 760 )
				{
					var ourMana = ourObj.mana;
					var mentalStrength = parseInt( ourMana / 10 );
					var manaString = GetDictionaryEntry( 9081, pSock.language ); // This being is at %d percent mental strength.
					pSock.SysMessage( manaString.replace( /%d/gi, mentalStrength ));
				}
			}
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 9078, pSock.language )); // It looks smarter than a rock, but dumber than a piece of wood.
	}
}
