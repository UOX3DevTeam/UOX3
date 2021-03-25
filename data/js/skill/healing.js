function onUseCheckedTriggered( pUser, targChar, iUsed )
{
	if( pUser && iUsed && iUsed.isItem )
	{
		if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			pSock.SysMessage( GetDictionaryEntry( 774, pLanguage ) ); // That is locked down and you cannot use it.
			return;
		}

		var socket = pUser.socket;
		if( socket )
		{
			if( pUser.skillsused.healing || pUser.skillsused.veterinary )
			{
				socket.SysMessage( "You are too busy to do that." );
			}
			else if( socket.GetTimer( 0 ) <= GetCurrentClock() )
			{
				socket.tempObj = iUsed;
				socket.SetTimer( 0, 5000 );
				onCallback1( socket, targChar );
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 473, socket.language ) );
			}
		}
	}
	return true;
}
function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			pSock.SysMessage( GetDictionaryEntry( 774, pLanguage ) ); // That is locked down and you cannot use it.
			return;
		}

		if( pUser.skillsused.healing || pUser.skillsused.veterinary )
			socket.SysMessage( "You are too busy to do that." );
		else if( socket.GetTimer( 0 ) <= GetCurrentClock() )	// Skill timer
		{
			socket.tempObj = iUsed;
			var targMsg = GetDictionaryEntry( 472, socket.language );
			socket.CustomTarget( 1, targMsg );
			socket.SetTimer( 0, 5000 );		// Reset the skill timer
		}
		else
			socket.SysMessage( GetDictionaryEntry( 473, socket.language ) );
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
		if( mChar.InRange( ourObj, 2 ) && mChar.CanSee( ourObj ) && Math.abs( mChar.z - ourObj.z ) < 4 )
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

			var iMulti = FindMulti( ourObj );
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
			}
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
						var healTimer = 10000;
						SetSkillInUse( socket, mChar, ourObj, skillNum, healTimer, true );
						mChar.StartTimer( healTimer, 0, true );
					}
					else
						socket.SysMessage( GetDictionaryEntry( 1492, socket.language ) );
				}
				else
					socket.SysMessage( GetDictionaryEntry( 1493, socket.language ) );
			}
			else if( ourObj.poison > 0 )	// Cure Poison
			{
				if( healSkill >= 600 && anatSkill >= 600 )
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
						if( mChar.serial == ourObj.serial ) // Curing yourself has a 9 to 15 second delay, depending on dexterity
							healTimer = 9400 + (( 0.6 * (( 120 - mChar.dexterity ) / 10 )) * 1000);
						else
							healTimer = 6000; // Curing others has a 6 second delay

						SetSkillInUse( socket, mChar, ourObj, skillNum, healTimer, true );
						mChar.StartTimer( healTimer, 1, true );
					}
					else
						socket.SysMessage( GetDictionaryEntry( 1494, socket.language ) );
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 1495, socket.language ) );
					socket.SysMessage( GetDictionaryEntry( 1496, socket.language ) );
				}

			}
			else if( ourObj.health == ourObj.maxhp )
				socket.SysMessage( GetDictionaryEntry( 1497, socket.language ) );
			else	// Heal
			{
				if( bItem.amount > 1 )
					bItem.amount = bItem.amount-1;
				else
					bItem.Delete();

				if( ourObj.murderer || ourObj.criminal )
					mChar.criminal = true;
				if( ourObj != mChar && ourObj.socket )
					ourObj.SysMessage( mChar.name+" is attempting to heal you." );
				if( mChar.CheckSkill( skillNum, 0, 1000 ) )
				{
					var healTimer;
					if( mChar.serial == ourObj.serial ) // Healing yourself has a 9 to 16 second delay, depending on your dexterity.
						healTimer = 9400 + (( 0.6 * (( 120 - mChar.dexterity ) / 10 )) * 1000);
					else
						healTimer = 5000; // Healing others has a 5 second delay

					SetSkillInUse( socket, mChar, ourObj, skillNum, healTimer, true );
					mChar.CheckSkill( 1, 0, 1000 );
					mChar.StartTimer( healTimer, 2, true );
				}
				else
				{
					ourObj.health = ourObj.health+1;

					if( skillNum == 17 ) // Healing
						socket.SysMessage( GetDictionaryEntry( 1499, socket.language ) );
					else
						socket.SysMessage( GetDictionaryEntry( 1500, socket.language ) );
				}
			}
		}
		else
			socket.SysMessage( GetDictionaryEntry( 1498, socket.language ) );
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
		if( mChar.dead )
		{
			socket.SysMessage( GetDictionaryEntry( 330, socket.language ) );
		}
		else if( ValidateObject( ourObj ) && mChar.InRange( ourObj, 2 ) && mChar.CanSee( ourObj ) )
		{
			switch( timerID )
			{
			case 0:	// Resurrect
				ourObj.Resurrect();
				socket.SysMessage( GetDictionaryEntry( 1272, socket.language ) );
				break;
			case 1:	// Cure Poison
				ourObj.SetPoisoned( 0, 0 );
				ourObj.StaticEffect( 0x373A, 0, 15 );
				ourObj.SoundEffect( 0x01E0, true );
				socket.SysMessage( GetDictionaryEntry( 1274, socket.language ) );
				var objSock = ourObj.socket;
				if( objSock )
					objSock.SysMessage( GetDictionaryEntry( 1273, objSock.language ) );
				break;
			case 2:	// Heal
				var healSkill;
				if( skillNum == 17 )
					healSkill = mChar.skills.healing;
				else if( skillNum == 39 )
					healSkill = mChar.skills.veterinary;

				ourObj.Heal( (RandomNumber( 3, 10 ) + parseInt(mChar.skills.anatomy / 50) + RandomNumber( parseInt(healSkill / 50), parseInt(healSkill / 20) )), mChar );
				socket.SysMessage( GetDictionaryEntry( 1271, socket.language ) );
				break;
			}
		}
		else
			socket.SysMessage( "You are no longer close enough to heal your target." );
	}
	SetSkillInUse( socket, mChar, ourObj, skillNum, 0, false );
}