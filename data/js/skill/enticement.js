function SkillRegistration()
{
	RegisterSkill( 15, true );	// Enticement
}

const useLoSCheckForEnticement = true;

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		var pRegion = pUser.region;
		if( pRegion )
		{
			if( pRegion.isSafeZone )
			{
				// Player is in a safe zone where no aggressive actions can be taken, disallow
				pSock.SysMessage( GetDictionaryEntry( 1799, pSock.language ));
			}
			else
			{
				pSock.CustomTarget( 0, GetDictionaryEntry( 862, pSock.language ), 1 ); // Whom do you wish to entice?
			}
		}
	}
	return true;
}

// Enticement - First Target
function onCallback0( pSock, ourObj )
{
	if( ValidateObject( ourObj ) && ourObj.isChar )
	{
		if( !ourObj.npc )
		{
			pSock.SysMessage( GetDictionaryEntry( 1445, pSock.language )); // You cannot entice other players.
		}
		else if( ourObj.aitype == 17 || ourObj.aitype == 4 || !ourObj.vulnerable )
		{
			pSock.SysMessage( GetDictionaryEntry( 1642, pSock.language )); // You cannot entice vendors or invulnerable folk
		}
		else
		{
			var ourObjRegion = ourObj.region;
			if( ourObjRegion )
			{
				if( ourObjRegion.isSafeZone )
				{
					// Target is in a safe zone where no aggressive actions can be taken, disallow
					pSock.SysMessage( GetDictionaryEntry( 1799, pSock.language )); // Hostile actions are not permitted in this safe area.
					return;
				}
			}
			var pUser = pSock.currentChar;

			// Can player see the target?
			if( useLoSCheckForEnticement && !pUser.CanSee( ourObj ))
			{
				pSock.SysMessage( GetDictionaryEntry( 1646, pSock.language )); // You cannot see that
				return;
			}

			if( ValidateObject( pUser ))
			{
				// TODO: At some point, base range of all bard abilities was 8, with increase of 1 tile per 15 points of skill in the ability
				if( !pUser.InRange( ourObj, 10 ))
				{
					pSock.SysMessage( GetDictionaryEntry( 393, pSock.language )); // That is too far away.
					return;
				}

				var myInstrument = GetInstrument( pUser );
				if( ValidateObject( myInstrument ))
				{
					pSock.tempObj = ourObj;
					pSock.CustomTarget( 1, GetDictionaryEntry( 1446, pSock.language ), 1 ); // You play your music, luring them near. Whom do you wish them to follow?
					PlayInstrument( pSock, myInstrument, true );
				}
				else
				{
					pSock.SysMessage( GetDictionaryEntry( 1444, pSock.language )); // You do not have an instrument to play on!
				}
			}
		}
	}
}

// Enticement - Second Target
function onCallback1( pSock, toFollow )
{
	if( ValidateObject( toFollow ))
	{
		if( toFollow.aitype == 17 || toFollow.aitype == 4 || !toFollow.vulnerable )
		{
			pSock.SysMessage( GetDictionaryEntry( 1642, pSock.language )); // You cannot entice vendors or invulnerable folk
		}
		else
		{
			var pFollower = pSock.tempObj;
			if( !ValidateObject( pFollower ))
				return;

			var pUser = pSock.currentChar;
			if( !ValidateObject( pUser ))
				return;

			// TODO: At some point, base range of all bard abilities was 8, with increase of 1 tile per 15 points of skill in the ability
			if( !pFollower.InRange( toFollow, 10 ))
			{
				// The two NPCs involved are too far away from one another
				pSock.SysMessage( GetDictionaryEntry( 393, pSock.language )); // That is too far away.
				return;
			}

			// Ensure target can see secondary target
			if( !pFollower.CanSee( toFollow ))
			{
				// No line of sight
				pFollower.TextMessage( GetDictionaryEntry( 1669, pSock.language ), false, 0x3b2, 0, pUser.serial ); // Cannot see target!
				return;
			}

			var myInstrument = GetInstrument( pUser );
			if( ValidateObject( myInstrument ))
			{
				if( pUser.CheckSkill( 29, 0, pUser.skillCaps.musicianship ) && pUser.CheckSkill( 15, 0, pUser.skillCaps.enticement ))
				{
					PlayInstrument( pSock, myInstrument, true );
					if( ValidateObject( pFollower ))
					{
						pUser.criminal = true;
						pFollower.Follow( toFollow );
						pSock.SysMessage( GetDictionaryEntry( 1447, pSock.language )); // You play your hypnotic music, luring them near your target.
					}
				}
				else
				{
					PlayInstrument( pSock, myInstrument, false );
					pSock.SysMessage( GetDictionaryEntry( 1448, pSock.language )); // Your music fails to attract them.
				}
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 1438, pSock.language )); // You do not have an instrument to play on!
			}
		}
	}
	pSock.tempObj = null;
}

function GetInstrument( pUser )
{
	// Fetch last instrument player played (set in musicianship.js)
	var lastInstrument = CalcItemFromSer( parseInt( pUser.GetTempTag( "lastInstrument" )));
	if( ValidateObject( lastInstrument ))
	{
		if( ValidateObject( lastInstrument.container ))
		{
			// Instrument is in a container. This should only work if item is inside player pack somewhere
			var rootCont = FindRootContainer( lastInstrument, 0 );
			if( ValidateObject( rootCont ) && rootCont == pUser.pack )
			{
				// Found instrument in player's pack somewhere!
				return lastInstrument;
			}
		}
		else if( pUser.InRange( lastInstrument, 3 ))
		{
			// Player is within range of the instrument!
			return lastInstrument;
		}
	}

	return null;
}

function PlayInstrument( pSock, myInstrument, wellPlayed )
{
	var soundID;
	switch( myInstrument.id )
	{
	case 0x0E9C:	// Drum
		if( wellPlayed )
		{
			soundID = 0x0038;
		}
		else
		{
			soundID = 0x0039;
		}
		break;
	case 0x0E9D:	// Tambourine
	case 0x0E9E:
		if( wellPlayed )
		{
			soundID = 0x0052;
		}
		else
		{
			soundID = 0x0053;
		}
		break;
	case 0x0EB1:	// Standing Harp
		if( wellPlayed )
		{
			soundID = 0x0043;
		}
		else
		{
			soundID = 0x0044;
		}
		break;
	case 0x0EB2:	// Harp
		if( wellPlayed )
		{
			soundID = 0x0045;
		}
		else
		{
			soundID = 0x0046;
		}
		break;
	case 0x0EB3:	// Lute
	case 0x0EB4:
		if( wellPlayed )
		{
			soundID = 0x004C;
		}
		else
		{
			soundID = 0x004D;
		}
		break;
	case 0x2805:	// Bamboo Flute
	case 0x2807:
		if( wellPlayed )
		{
			soundID = 0x504;
		}
		else
		{
			soundID = 0x503;
		}
		break;
	default:
		return;
	}
	pSock.SoundEffect( soundID, true );
}

