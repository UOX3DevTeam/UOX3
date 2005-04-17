function onUse( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		if( socket.GetTimer( 0 ) <= GetCurrentClock() )	// Skill timer
		{
			socket.tempObj = iUsed;
			var targMsg = GetDictionaryEntry( 472, socket.Language );
			socket.CustomTarget( 1, targMsg );
			socket.SetTimer( 0, 5000 );		// Reset the skill timer
		}
		else
			socket.SysMessage( GetDictionaryEntry( 473, socket.Language ) );
	}
	return false;
}

function onCallback1( socket, ourObj )
{
	var bItem = socket.tempObj;
	var mChar = socket.currentChar;

	socket.tempObj = null;

	if( bItem && bItem.isItem && ourObj && ourObj.isChar && mChar && mChar.isChar )
	{
		if( mChar.InRange( ourObj, 2 ) )
		{
			var healSkill;
			var skillNum;
			if( ourObj.id == 0x0190 || ourObj.id == 0x0191 || ourObj.id == 0x0192 || ourObj.id == 0x0193 )
			{
				healSkill = mChar.baseskills.healing;
				skillNum  = 21;
			}
			else if( ourObj.tamed && ourObj.owner )
			{
				healSkill = mChar.baseskills.veterinary;
				skillNum  = 39;
			}
			else
			{
				socket.SysMessage( "You can't heal that!" );
				return;
			}
/*			// Commented out until the multi-functions actually work.
			var iMulti = FindMulti( ourObj.x, ourObj.y, ourObj.z, 0 );
			if( iMulti )
			{
				if( iMulti.IsInMulti( ourObj ) )
				{
					if( !iMulti.IsOnOwnerList( ourObj ) && !iMulti.IsOnOwnerList( mChar ) )
					{
						socket.SysMessage( "Your target is in another character's house, healing attempt aborted." );
						return;
					}
				}
			}*/
			var anatSkill = mChar.baseskills.anatomy;
			if( ourObj.dead )
			{
				if( healSkill >= 800 && anatSkill >= 800 )
				{
					if( ourObj.murderer )
						mChar.criminal = true;
					if( mChar.CheckSkill( skillNum, 800, 1000 ) && mChar.CheckSkill( 1, 800, 1000 ) )
						DoTempEffect( 0, mChar, ourObj, 23, skillNum, 0, 0, bItem );
					else
					{
						socket.SysMessage( GetDictionaryEntry( 1492, socket.Language ) );
						bItem.amount = bItem.amount-1;
					}
				}
				else
					socket.SysMessage( GetDictionaryEntry( 1493, socket.Language ) );
			}
			else if( ourObj.poison > 0 )
			{
				if( healSkill >= 800 && anatSkill >= 800 )
				{
					if( ourObj.murderer || ourObj.criminal )
						mChar.criminal = true;
					if( mChar.CheckSkill( skillNum, 600, 1000 ) && mChar.CheckSkill( 1, 600, 1000 ) )
						DoTempEffect( 0, mChar, ourObj, 24, skillNum, 0, 0, bItem );
					else
					{
						socket.SysMessage( GetDictionaryEntry( 1494, socket.Language ) );
						bItem.amount = bItem.amount-1;
					}
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 1495, socket.Language ) );
					socket.SysMessage( GetDictionaryEntry( 1496, socket.Language ) );
				}

			}
			else if( ourObj.health == ourObj.maxhp )
				socket.SysMessage( GetDictionaryEntry( 1497, socket.Language ) );
			else
			{
				if( ourObj.murderer || ourObj.criminal )
					mChar.criminal = true;
				if( mChar.CheckSkill( skillNum, 0, 1000 ) )
				{
					mChar.CheckSkill( 1, 0, 1000 );
					DoTempEffect( 0, mChar, ourObj, 22, skillNum, 0, 0, bItem );
				}
				else
				{
					ourObj.health = ourObj.health+1;

					if( skillNum == 21 ) // Healing
						socket.SysMessage( GetDictionaryEntry( 1499, socket.Language ) );
					else
						socket.SysMessage( GetDictionaryEntry( 1500, socket.Language ) );
				}
			}
		}
		else
			socket.SysMessage( GetDictionaryEntry( 1498, socket.Language ) );
	}
}

