function SkillRegistration()
{
	RegisterSkill( 9, true );	// Peacemaking
}

const useLoSCheckForPeacemaking = true;

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		var myInstrument = GetInstrument( pUser );
		if( ValidateObject( myInstrument ) )
		{
			if( pUser.CheckSkill( 29, 0, pUser.skillCaps.peacemaking ))
			{
				PlayInstrument( pSock, myInstrument, true );

				// TODO: At some point, base range of all bard abilities was 8, with increase of 1 tile per 15 points of skill in the ability
				AreaCharacterFunction( "PeaceMakeArea", pUser, 15 );
			}
			else
			{
				PlayInstrument( pSock, myInstrument, false );
				pSock.SysMessage( GetDictionaryEntry( 1441, pSock.language ) ); // You attempt to calm everyone, but fail.

				// Fetch skill-specific skill delay
				var skillDelay = Skills[9].skillDelay;
				pSock.SetTimer( Timer.SOCK_SKILLDELAY, skillDelay * 1000 ); // Full skill delay for failed used of skill
			}
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 1438, pSock.language ) ); // You do not have an instrument to play on!
		}
	}
	return true;
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
		case 0x2805:	//Bamboo Flute
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

function PeaceMakeArea( pUser, targChar )
{
	if( pUser == targChar )
		return;

	if( useLoSCheckForPeacemaking && !pUser.CanSee( targChar ))
		return;

	var targSock = targChar.socket;

	// Fetch skill-specific skill delay
	var skillDelay = Skills[9].skillDelay;

	// TODO: At some point, musicianship skill started giving 1% success bonus per point of musicianship over 100.0
	// for attempts at Discordance, Targeted Peacemaking and Provocation

	if( pUser.CheckSkill( 9, targChar.skillToPeace, 1200 ))
	{
		if( targSock )
		{
			targSock.SysMessage( GetDictionaryEntry( 1440, targSock.language )); // You hear some lovely music, and forget about fighting.
		}
		targChar.setPeace = 60; // At some point this was set to 2-10 seconds only for creatures?

		// Half skill delay on successful use of peacemaking
		pUser.socket.SetTimer( Timer.SOCK_SKILLDELAY, ( skillDelay * 1000 ) / 2 );
		return true;
	}
}

function _restorecontext_() {}
