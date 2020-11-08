function SkillRegistration()
{
	RegisterSkill( 22, true );	// Provocation
}

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
				pSock.SysMessage( GetDictionaryEntry( 1799, pSock.language ) );
			}
			else
			{
		pSock.CustomTarget( 0, GetDictionaryEntry( 861, pSock.language ) );
			}
		}
	}

	return true;
}

function onCallback0( pSock, ourObj )
{
	if( ValidateObject( ourObj ) && ourObj.isChar )
	{
		if( !ourObj.npc )
			pSock.SysMessage( GetDictionaryEntry( 1442, pSock.language ) );
		else if( ourObj.aitype == 17 || ourObj.aitype == 4 || !ourObj.vulnerable )
			pSock.SysMessage( GetDictionaryEntry( 830, pSock.language ) );
		else
		{
			var ourObjRegion = ourObj.region;
			if( ourObjRegion )
			{
				if( ourObjRegion.isSafeZone )
				{
					// Target is in a safe zone where no aggressive actions can be taken, disallow
					pSock.SysMessage( GetDictionaryEntry( 1799, pSock.language ) );
					return;
				}
			}
			var pUser = pSock.currentChar;
			if( ValidateObject( pUser ) )
			{
				var myInstrument = GetInstrument( pUser );
				if( ValidateObject( myInstrument ) )
				{
					pSock.tempObj = ourObj;
					pSock.CustomTarget( 1, GetDictionaryEntry( 1443, pSock.language ) );
					PlayInstrument( pSock, myInstrument, true );
				}
				else
					pSock.SysMessage( GetDictionaryEntry( 1438, pSock.language ) );
			}
		}
	}
}

function onCallback1( pSock, toAttack )
{
	if( ValidateObject( toAttack ) )
	{
		if( toAttack.aitype == 17 || toAttack.aitype == 4 || !toAttack.vulnerable )
			pSock.SysMessage( GetDictionaryEntry( 1730, pSock.language ) );
		else
		{
			var pUser = pSock.currentChar;
			if( ValidateObject( pUser ) )
			{
				var pAttacker = pSock.tempObj;
				if( ValidateObject( pAttacker ) )
				{
					if( pAttacker == toAttack )
						pSock.SysMessage( GetDictionaryEntry( 1449, pSock.language ) );
					else
					{
						var myInstrument = GetInstrument( pUser );
						if( ValidateObject( myInstrument ) )
						{
							if( pUser.CheckSkill( 29, 0, 1000 ) )	// Musicianship
							{
								PlayInstrument( pSock, myInstrument, true );
								var willAttack;
								if( pUser.CheckSkill( 22, pAttacker.skillToProv, 1200 ) )	// Provocation
								{
									willAttack = toAttack;
									if( toAttack.innocent )
										pUser.criminal = true;
									pSock.SysMessage( GetDictionaryEntry( 1450, pSock.language ) );
								}
								else
									willAttack = pUser;

								BeginAttack( pAttacker, willAttack, true );
								BeginAttack( willAttack, pAttacker, false );
								pAttacker.EmoteMessage( "You see "+pAttacker.name+" attacking "+willAttack.name+"!" );
							}
							else
							{
								PlayInstrument( pSock, myInstrument, false );
								pSock.SysMessage( GetDictionaryEntry( 1452, pSock.language ) );
							}
						}
						else
							pSock.SysMessage( GetDictionaryEntry( 1438, pSock.language ) );
					}
				}
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
	default:
		return;
	}
	pSock.SoundEffect( soundID, true );
}

function BeginAttack( charOne, charTwo, attackFirst )
{
	charOne.attackFirst = attackFirst;
	charOne.attacker = charTwo;
	charOne.target = charTwo;

	if( charOne.visible == 1 || charOne.visible == 2 )
	{
		charOne.visible = 0;
		charOne.stealth = -1;
	}
	if( charOne.isMeditating )
		charOne.isMeditating = false;
	if( charOne.npc )
	{
		if( !charOne.atWar )
			charOne.atWar = true;
	}
}
