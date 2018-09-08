function SkillRegistration()
{
	RegisterSkill( 15, true );	// Enticement
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
		pSock.CustomTarget( 0, GetDictionaryEntry( 862, pSock.Language ) );

	return true;
}

function onCallback0( pSock, ourObj )
{
	if( ValidateObject( ourObj ) && ourObj.isChar )
	{
		if( !ourObj.npc )
			pSock.SysMessage( GetDictionaryEntry( 1445, pSock.Language ) );
		else if( ourObj.aitype == 17 || ourObj.aitype == 4 || !ourObj.vulnerable )
			pSock.SysMessage( GetDictionaryEntry( 1642, pSock.Language ) );
		else
		{
			var pUser = pSock.currentChar;
			if( ValidateObject( pUser ) )
			{
				var myInstrument = GetInstrument( pUser );
				if( ValidateObject( myInstrument ) )
				{
					pSock.tempObj = ourObj;
					pSock.CustomTarget( 1, GetDictionaryEntry( 1446, pSock.Language ) );
					PlayInstrument( pSock, myInstrument, true );
				}
				else
					pSock.SysMessage( GetDictionaryEntry( 1444, pSock.Language ) );
			}
		}
	}
}

function onCallback1( pSock, toFollow )
{
	if( ValidateObject( toFollow ) )
	{
		if( toFollow.aitype == 17 || toFollow.aitype == 4 || !toFollow.vulnerable )
			pSock.SysMessage( GetDictionaryEntry( 1642, pSock.Language ) );
		else
		{
			var pUser = pSock.currentChar;
			if( ValidateObject( pUser ) )
			{
				var myInstrument = GetInstrument( pUser );
				if( ValidateObject( myInstrument ) )
				{
					if( pUser.CheckSkill( 29, 0, 1000 ) && pUser.CheckSkill( 15, 0, 1000 ) )
					{
						PlayInstrument( pSock, myInstrument, true );
						var pFollower = pSock.tempObj;
						if( ValidateObject( pFollower ) )
						{
							pUser.criminal = true;
							pFollower.Follow( toFollow );
							pSock.SysMessage( GetDictionaryEntry( 1447, pSock.Language ) );
						}
					}
					else
					{
						PlayInstrument( pSock, myInstrument, false );
						pSock.SysMessage( GetDictionaryEntry( 1448, pSock.Language ) );
					}
				}
				else
					pSock.SysMessage( GetDictionaryEntry( 1438, pSock.Language ) );
			}
		}
	}
	pSock.tempObj = null;
}

function GetInstrument( pUser )
{
	if( ValidateObject( pUser.pack ) )
	{
		for( var toCheck = pUser.pack.FirstItem(); !pUser.pack.FinishedItems(); toCheck = pUser.pack.NextItem() )
		{
			if( ValidateObject( toCheck ) )
			{
				switch( toCheck.id )
				{
				case 0x0E9C:
				case 0x0E9D:
				case 0x0E9E:
				case 0x0EB1:
				case 0x0EB2:
				case 0x0EB3:
				case 0x0EB4:
				case 0x2805:
				case 0x2807:
					return toCheck;
				default:
					break;
				}
			}
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
			soundID = 0x0038;
		else
			soundID = 0x0039;
		break;
	case 0x0E9D:	// Tambourine
	case 0x0E9E:
		if( wellPlayed )
			soundID = 0x0052;
		else
			soundID = 0x0053;
		break;
	case 0x0EB1:	// Standing Harp
		if( wellPlayed )
			soundID = 0x0043;
		else
			soundID = 0x0044;
		break;
	case 0x0EB2:	// Harp
		if( wellPlayed )
			soundID = 0x0045;
		else
			soundID = 0x0046;
		break;
	case 0x0EB3:	// Lute
	case 0x0EB4:
		if( wellPlayed )
			soundID = 0x004C;
		else
			soundID = 0x004D;
		break;
	case 0x2805:	//Bamboo Flute
	case 0x2807:
		if( wellPlayed )
			soundID = 0x504;
		else
			soundID = 0x503;
		break;
	default:
		return;
	}
	pSock.SoundEffect( soundID, true );
}

