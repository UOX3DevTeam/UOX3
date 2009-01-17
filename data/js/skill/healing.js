function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		if( pUser.skillsused.healing || pUser.skillsused.veterinary )
			socket.SysMessage( "You are too busy to do that." );
		else if( socket.GetTimer( 0 ) <= GetCurrentClock() )	// Skill timer
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
		if( mChar.InRange( ourObj, 2 ) && mChar.CanSee( ourObj ) )
		{
			if( ourObj.GetTag( "SK_BEINGHEALED" ) )
			{
				if( ourObj.GetTag( "SK_HEALTIMER" ) < GetCurrentClock() )
				{
					ourObj.SetTag( "SK_BEINGHEALED", false );
					ourObj.SetTag( "SK_HEALTIMER", 0 );
				}
				else
				{
					socket.SysMessage( "They are already being healed." );
					return;
				}
			}

			var healSkill;
			var skillNum;
			if( ourObj.isHuman || ourObj.id == 0x0192 || ourObj.id == 0x0193 || ourObj.id == 0x025f || ourObj.id == 0x0260 )
			{
				healSkill = mChar.baseskills.healing;
				skillNum  = 17;
			}
			else if( ourObj.isAnimal && ourObj.tamed && ourObj.owner )
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
			if( ourObj.dead )	// Resurrection	
			{
				if( healSkill >= 800 && anatSkill >= 800 )
				{
					if( bItem.amount > 1 )
						bItem.amount = bItem.amount-1;
					else
						bItem.Delete();

					if( ourObj.murderer )
						mChar.criminal = true;

					if( mChar.CheckSkill( skillNum, 800, 1000 ) && mChar.CheckSkill( 1, 800, 1000 ) )
					{
						var healTimer = 15000;
						SetSkillInUse( socket, mChar, ourObj, skillNum, healTimer, true );
						mChar.StartTimer( healTimer, 0, true );
					}
					else
						socket.SysMessage( GetDictionaryEntry( 1492, socket.Language ) );
				}
				else
					socket.SysMessage( GetDictionaryEntry( 1493, socket.Language ) );
			}
			else if( ourObj.poison > 0 )	// Cure Poison
			{
				if( healSkill >= 800 && anatSkill >= 800 )
				{
					if( bItem.amount > 1 )
						bItem.amount = bItem.amount-1;
					else
						bItem.Delete();

					if( ourObj.murderer || ourObj.criminal )
						mChar.criminal = true;
					if( mChar.CheckSkill( skillNum, 600, 1000 ) && mChar.CheckSkill( 1, 600, 1000 ) )
					{
						var healTimer;
						if( mChar.serial == ourObj.serial )
							healTimer = 6000;
						else
							healTimer = RandomNumber( 15000, 18000 );

						SetSkillInUse( socket, mChar, ourObj, skillNum, healTimer, true );
						mChar.StartTimer( healTimer, 1, true );
					}
					else
						socket.SysMessage( GetDictionaryEntry( 1494, socket.Language ) );
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 1495, socket.Language ) );
					socket.SysMessage( GetDictionaryEntry( 1496, socket.Language ) );
				}

			}
			else if( ourObj.health == ourObj.maxhp )
				socket.SysMessage( GetDictionaryEntry( 1497, socket.Language ) );
			else	// Heal
			{
				if( bItem.amount > 1 )
					bItem.amount = bItem.amount-1;
				else
					bItem.Delete();

				if( ourObj.murderer || ourObj.criminal )
					mChar.criminal = true;
				if( mChar.CheckSkill( skillNum, 0, 1000 ) )
				{
					var healTimer;
					if( mChar.serial == ourObj.serial )
						healTimer = parseInt((18 - (RandomNumber( 0, healSkill ) / 200)) * 1000);
					else
						healTimer = parseInt((7 - (RandomNumber( 0, healSkill ) / 330)) * 1000);

					SetSkillInUse( socket, mChar, ourObj, skillNum, healTimer, true );
					mChar.CheckSkill( 1, 0, 1000 );
					mChar.StartTimer( healTimer, 2, true );
				}
				else
				{
					ourObj.health = ourObj.health+1;

					if( skillNum == 17 ) // Healing
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

function SetSkillInUse( socket, mChar, ourObj, skillNum, healingTime, setVal )
{
	if( setVal )
	{
		if( socket )
			socket.SysMessage( "You begin to apply the bandages." );
		mChar.SetTag( "SK_HEALINGTYPE", skillNum );
	}
	else
	{
		mChar.SetTag( "SK_HEALINGTYPE", 0 );
		mChar.SetTag( "SK_HEALINGTARG", 0 );
	}

	if( skillNum == 17 )		// Healing
		mChar.skillsused.healing 	= setVal;
	else if( skillNum == 39 )	// Veterinary
		mChar.skillsused.veterinary 	= setVal;

	if( ValidateObject( ourObj ) )
	{
		ourObj.SetTag( "SK_BEINGHEALED", setVal );
		ourObj.SetTag( "SK_HEALINGTIME", healingTime );
		if( setVal )
			mChar.SetTag( "SK_HEALINGTARG", ourObj.serial );
	}
}

function onTimer( mChar, timerID )
{
	var skillNum	= mChar.GetTag( "SK_HEALINGTYPE" );
	var ourObj	= CalcCharFromSer( mChar.GetTag( "SK_HEALINGTARG" ) );
	var socket 	= mChar.socket;
	if( socket )
	{
		if( ValidateObject( ourObj ) && mChar.InRange( ourObj, 2 ) && mChar.CanSee( ourObj ) )
		{
			switch( timerID )
			{
			case 0:	// Resurrect
				ourObj.Resurrect();
				socket.SysMessage( GetDictionaryEntry( 1272, socket.Language ) );
				break;
			case 1:	// Cure Poison
				ourObj.SetPoisoned( 0 );
				ourObj.StaticEffect( 0x373A, 0, 15 );
				ourObj.SoundEffect( 0x01E0, true );
				socket.SysMessage( GetDictionaryEntry( 1274, socket.Language ) );
				var objSock = ourObj.socket;
				if( objSock )
					objSock.SysMessage( GetDictionaryEntry( 1273, objSock.Language ) );
				break;
			case 2:	// Heal
				var healSkill;
				if( skillNum == 17 )
					healSkill = mChar.skills.healing;
				else if( skillNum == 39 )
					healSkill = mChar.skills.veterinary;

				ourObj.Heal( (RandomNumber( 3, 10 ) + parseInt(mChar.skills.anatomy / 50) + RandomNumber( parseInt(healSkill / 50), parseInt(healSkill / 20) )), mChar );
				socket.SysMessage( GetDictionaryEntry( 1271, socket.Language ) );
				break;
			}
		}
		else
			socket.SysMessage( "You are no longer close enough to heal your target." );
	}
	SetSkillInUse( socket, mChar, ourObj, skillNum, 0, false );
}