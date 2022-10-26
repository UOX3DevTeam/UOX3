function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	if ( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( iUsed.id != 0x0de1 )
		return false;

	// Perform distance check
	var packOwner = GetPackOwner( iUsed, 0 );
	if( packOwner != null && packOwner.serial == pUser.serial )
	{
		//Make sure there are no other campfires in close vincinity
		var findCampSpot = FindItem( pUser.x, pUser.y, pUser.z, pUser.worldnumber, 0x0de3 );
		if( !findCampSpot || ( findCampSpot && !findCampSpot.InRange( pUser, 3 )))
		{
			if( pUser.CheckSkill( 10, 0, 1000 ))
			{
				if( iUsed.amount > 1 )
				{
					iUsed.amount -= 1;
					var eKindling = CreateDFNItem( pSock, pUser, "0x0de3", 1, "ITEM", false );
        			if( eKindling && eKindling.isItem )
        			{
						iUsed = eKindling;
        			}
				}
				iUsed.AddScriptTrigger( 5008 );
				iUsed.container = null;
				iUsed.Teleport( pUser.x, pUser.y, pUser.z, pUser.worldnumber );
				iUsed.movable = 2;
				iUsed.id = 0x0de3;
				iUsed.dir = 29;
				iUsed.decayable = false;

				// Successfully created campfire
				pUser.SysMessage( GetDictionaryEntry( 1765, pSock.language ));
				// You feel it would take a few moments to secure your camp.
				pUser.SysMessage( GetDictionaryEntry( 1790, pSock.language ));

				//Campfire burns for 30 seconds to secure camp
				iUsed.StartTimer( 30000, 1, true );
			}
			else // You fail to ignite the campfire.
			{
				pUser.SysMessage( GetDictionaryEntry( 1764, pSock.language ));
			}
		}
		else // There is not a spot nearby to place your campfire.
		{
			pUser.SysMessage( GetDictionaryEntry( 1789, pSock.language ));
		}
	}
	else // That item must be in your backpack before it can be used.
	{
		pUser.SysMessage( GetDictionaryEntry( 1763, pSock.language ));
	}
	return false;
}

function onTimer( timerObj, timerID )
{
	if( timerID == 1 )
	{
		timerObj.SetTag( "securecamp", true ); // mark campfire as secure
		AreaCharacterFunction( "searchForNearbyChars", timerObj, 12 );
		timerObj.StartTimer( 30000, 2, true );
	}
	else if( timerID == 2 )
	{
		timerObj.SetTag( "securecamp", null ); // campfire is no longer secure
		timerObj.id = 0x0de9; // turn campfire into embers
		timerObj.dir = 2; //Set light radius to 2
		timerObj.StartTimer( 15000, 3, true ); //Embers last for 15 seconds
	}
	else if( timerID == 3 )
	{
		timerObj.id = 0x0dea; // turn embers into burnt wood
		timerObj.decayable = true;
	}
	else if( timerID == 4 )
	{
		// Remove secure camp-tag from player
		timerObj.SetTag( "securecamp", null );
	}
}

function onCollide( pSock, pUser, iUsed )
{
	if( iUsed.id == 0x0de3 )
	{
		if( pUser.gender == 0 )
		{
			pUser.SoundEffect( 343, false );
		}
		else if( pUser.gender == 1 )
		{
			pUser.SoundEffect( 806, false );
		}
		pUser.TextMessage( "Ouch!", false );
		pUser.Damage( 2 );
	}
}

function searchForNearbyChars( campfire, trgChar )
{
	// Only apply secure-camp tag to players who are online, out of combat, !criminal
	if( trgChar.online && !trgChar.criminal && !trgChar.atWar && trgChar.attacker == null )
	{
		// The camp is now secure.
		trgChar.SysMessage( GetDictionaryEntry( 1791, trgChar.socket.language ));

		// Add player's serial as custom tag on campfire to mark it as secure for said character
		campfire.SetTag( trgChar.serial & 0x00FFFFFF, true );
	}
}