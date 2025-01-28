// Trolls, Cyclopes, Ophidians, Ogres, Ettins, Orcs, Harpies, Headlesses, Lizardmen, Ratmen, Humans
// , Human Players, Gargoyle players, Elf players, Juka, Meer, Pixies, Succubi, Centaur, Ethereal Warriors,
const validHealTargets = [ 0x0190,0x0191,0x025d,0x025e,0x029a,0x029b,0x0001,0x0002,0x0007,0x0011,0x0012,
		0x001e,0x001f,0x0021,0x0023,0x0024,0x0029,0x002a,0x002c,0x002d,0x0035,0x0036,0x004b,0x004c,0x0054,
		0x0065,0x007b,0x007c,0x007d,0x0080,0x0087,0x008a,0x008c,0x008e,0x008f,0x0095,0x00b5,0x00b6,0x00b8,
		0x00b9,0x00ba,0x02fc,0x02fd,0x02fe,0x0300,0x0302,0x0303,0x0304,0x0305 ];

// Gargoyles (all non-player ones), Corpsers, Dragons, Reapers, Swamp Tentacles, Slimes, Terrathans, Eagles, Birds, Giant Snakes, Spiders, Gorillas,
// Mongbats, Giant Scorpions, Snakes, Drakes, Frogs, Sea Serpents, Dolphins, Horses, Cats, Alligators, Pigs,
// Rabbits, Lava Lizards, Sheep, Chickens, Goats, Ostards, Bears, Grizzly Bears, Polar Bears, Cougars, Giant Rats,
// Cows, Dogs, Llamas, Walruses, Wolves (timber), Nightmares, Bulls, Great Harts, Hinds, Rats, Pack Horses, Pack Llamas,
// Quagmires, Bogthings, Boglings, Cranes, Beetles, Ridgebacks, Hiryus, Reptalons, Cu Sidhes, Hellcats
const validVetTargets = [ 0x0004,0x0005,0x0006,0x0008,0x000b,0x000c,0x0014,0x0015,0x0019,0x001c,0x001d,
		0x0025,0x0027,0x002f,0x0030,0x0031,0x0033,0x0034,0x003b,0x003c,0x003d,0x003e,0x003f,0x0040,0x0042,
		0x0043,0x0046,0x0047,0x0048,0x004d,0x0051,0x0058,0x0059,0x005a,0x005b,0x0062,0x0064,0x0067,0x0069,
		0x006a,0x0072,0x0074,0x0075,0x0076,0x0077,0x0078,0x0079,0x007a,0x007f,0x0082,0x0084,0x0090,0x0091,
		0x0096,0x0097,0x0098,0x009d,0x00a7,0x00a9,0x00ab,0x00b1,0x00b2,0x00b3,0x00bb,0x00bc,0x00be,0x00c2,
		0x00c3,0x00c8,0x00c9,0x00ca,0x00cb,0x00cc,0x00cd,0x00ce,0x00cf,0x00d0,0x00d1,0x00d2,0x00d3,0x00d4,
		0x00d5,0x00d6,0x00d8,0x00d9,0x00da,0x00db,0x00dc,0x00dd,0x00e1,0x00e2,0x00e4,0x00e8,0x00ea,0x00ed,
		0x00ee,0x00f3,0x00fd,0x00fe,0x0114,0x0115,0x0116,0x0117,0x0122,0x0123,0x0124,0x02f1,0x02f2,0x02f3,
		0x02f6,0x030b,0x030c,0x0315,0x0317,0x031a,0x031f ];

function onUseCheckedTriggered( pUser, targChar, iUsed )
{
	if( pUser && iUsed && iUsed.isItem )
	{
		var socket = pUser.socket;
		var pLanguage = socket.language;
		if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			if( socket != null )
			{
				socket.SysMessage( GetDictionaryEntry( 774, pLanguage )); // That is locked down and you cannot use it.
			}
			return;
		}

		if( pUser.visible == 1 || pUser.visible == 2 ) // Using bandages will now unhide you. February_2,_1999
		{
			pUser.visible = 0;
		}

		if( socket != null )
		{
			if( pUser.skillsused.healing || pUser.skillsused.veterinary )
			{
				socket.SysMessage( GetDictionaryEntry( 1971, pLanguage )); // You are too busy to do that.
			}
			else if( socket.GetTimer( Timer.SOCK_SKILLDELAY ) <= GetCurrentClock() )
			{
				socket.tempObj = iUsed;
				socket.SetTimer( Timer.SOCK_SKILLDELAY, 5000 );
				onCallback1( socket, targChar );
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 473, pLanguage )); // You must wait a few moments before using another skill.
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
		var pLanguage = socket.language;
		if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, pLanguage )); // That is locked down and you cannot use it.
			return;
		}

		if( pUser.visible == 1 || pUser.visible == 2 ) // Using bandages will now unhide you. February_2,_1999
		{
			pUser.visible = 0;
		}

		if( pUser.skillsused.healing || pUser.skillsused.veterinary )
		{
			socket.SysMessage( GetDictionaryEntry(1971, pLanguage)); // You are too busy to do that.
		}
		else if( socket.GetTimer( Timer.SOCK_SKILLDELAY ) <= GetCurrentClock() )	// Skill timer
		{
			socket.tempObj = iUsed;
			var targMsg = GetDictionaryEntry( 472, pLanguage ); // Who will you use the bandages on?
			socket.CustomTarget( 1, targMsg );
			socket.SetTimer( Timer.SOCK_SKILLDELAY, 5000 ); // Reset the skill timer
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 473, pLanguage )); // You must wait a few moments before using another skill.
		}
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
			if( ourObj.GetTempTag( "SK_BEINGHEALED" ))
			{
				if( ourObj.GetTempTag( "SK_HEALTIMER" ) < GetCurrentClock() )
				{
					ourObj.SetTempTag( "SK_BEINGHEALED", false );
					ourObj.SetTempTag( "SK_HEALTIMER", 0 );
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 6013, socket.language )); // They are already being healed.
					return;
				}
			}

			var healSkill;
			var skillNum;
			var buffIcon;
			var priCliloc;
			var scndCliloc;
			if( IsTargetHealable( ourObj, false ))
			{
				// Target can be healed with Healing skill
				healSkill = mChar.baseskills.healing;
				skillNum  = 17;
				buffIcon = 1069;
				priCliloc = 1002082;
				scndCliloc = 1151400;
			}
			else if( IsTargetHealable( ourObj, true ) || ( ourObj.tamed && ourObj.owner ))
			{
				// Target can be healed with Veterinary skill
				healSkill = mChar.baseskills.veterinary;
				skillNum  = 39;
				buffIcon = 1101;
				priCliloc = 1002167;
				scndCliloc = 1151400;
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 6014, socket.language )); // You can't heal that!
				return;
			}

			var iMulti = FindMulti( ourObj );
			if( iMulti )
			{
				if( iMulti.IsInMulti( ourObj ))
				{
					if( !iMulti.IsOnOwnerList( ourObj ) && !iMulti.IsOnOwnerList( mChar ))
					{
						socket.SysMessage( GetDictionaryEntry( 6015, socket.language )); // Your target is in another character's house, healing attempt aborted.
						return;
					}
				}
			}
			var anatSkill = mChar.baseskills.anatomy;
			if( ourObj.dead ) // Resurrection
			{
				if( healSkill >= 800 && anatSkill >= 800 )
				{
					// Consume some bandages
					if( bItem.amount > 1 )
					{
						bItem.amount = bItem.amount - 1;
					}
					else
					{
						bItem.Delete();
					}

					// Flag healer as criminal if target is murderer/criminal
					if( ourObj.murderer || ourObj.criminal )
					{
						mChar.criminal = true;
					}

					// Resurrecting takes between 8 - 10 seconds depending on dexterity
					var healTimer = 10000 - (( mChar.dexterity / 50 ) * 1000 );
					SetSkillInUse( socket, mChar, ourObj, skillNum, healTimer, true );
					mChar.StartTimer( healTimer, 0, true );
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 1493, socket.language )); // You are not skilled enough to resurrect.
				}
			}
			else if( ourObj.poison > 0 )	// Cure Poison
			{
				if( healSkill >= 600 && anatSkill >= 600 )
				{
					// Consume some bandages
					if( bItem.amount > 1 )
					{
						bItem.amount = bItem.amount - 1;
					}
					else
					{
						bItem.Delete();
					}

					// Flag healer as criminal if target is murderer/criminal
					if( ourObj.murderer || ourObj.criminal )
					{
						mChar.criminal = true;
					}

					var healTimer;
					if( mChar.serial == ourObj.serial )
					{
						// Curing Self takes 13 to 18 seconds depending on dexterity
						healTimer = 18000 - (( mChar.dexterity / 20 ) * 1000);
					}
					else
					{
						// Curing others takes 4 to 6 seconds depending on dexterity
						healTimer = 6000 - (( mChar.dexterity / 50 ) * 1000);
					}

					SetSkillInUse( socket, mChar, ourObj, skillNum, healTimer, true );
					mChar.StartTimer( healTimer, 1, true );
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 1495, socket.language )); // You are not skilled enough to cure poison.
					socket.SysMessage( GetDictionaryEntry( 1496, socket.language )); // The poison in your target's system counters the bandage's effect.
				}

			}
			else if( ourObj.health == ourObj.maxhp )
			{
				socket.SysMessage( GetDictionaryEntry( 1497, socket.language )); // That being is undamaged.
			}
			else // Heal
			{

				if( ourObj.GetTempTag( "blockHeal" ) == true )
				{
					if( ourObj != mChar && ourObj.socket )
					{
						socket.SysMessage( "You cannot heal that target in their current state." );
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 9058, socket.language ));// You can not heal yourself in your current state.
					}
				}
				else if( ourObj.GetTempTag( "doBleed" ) == true )
				{
					if( ourObj != mChar && ourObj.socket ) 
					{
						socket.SysMessage( "You bind the wound and stop the bleeding" );
					}
					else
					{
						socket.SysMessage( "The bleeding wounds have healed, you are no longer bleeding!" );
					}
					ourObj.KillJSTimer( 9300, 7001 );
					ourObj.SetTempTag( "doBleed", null );
				}

				// Consume some bandages
				if( bItem.amount > 1 )
				{
					bItem.amount = bItem.amount - 1;
				}
				else
				{
					bItem.Delete();
				}

				// Flag healer as criminal if target is murderer/criminal
				if( ourObj.murderer || ourObj.criminal )
				{
					mChar.criminal = true;
				}

				// Inform the target
				if( ourObj != mChar && ourObj.socket )
				{
					var tempMsg = GetDictionaryEntry( 6016, ourObj.socket.language ); // %s is attempting to heal you.
					ourObj.SysMessage( tempMsg.replace( /%s/gi, mChar.name ));
				}

				var healTimer;
				if( mChar.serial == ourObj.serial )
				{
					// Healing self takes 11 to 15 seconds, depending on dexterity
					healTimer = 15000 - (( mChar.dexterity / 20 ) * 1000 );
				}
				else
				{
					// Healing others takes 3 to 5 seconds, depending on dexterity
					healTimer = 5000 - (( mChar.dexterity / 50 ) * 1000 );
				}

				mChar.AddScriptTrigger( 4014 ); // Add healing_slip.js script
				SetSkillInUse( socket, mChar, ourObj, skillNum, healTimer, true );

				var seconds = Math.round(healTimer / 1000);
				// Add buff to target or yourself
				if (ourObj != mChar && ourObj.socket)
				{
					TriggerEvent( 2204, "AddBuff", ourObj, buffIcon, priCliloc, scndCliloc, seconds, " " + ourObj.name );
				}
				else
				{
					TriggerEvent( 2204, "AddBuff", mChar, buffIcon, priCliloc, scndCliloc, seconds, " " + ourObj.name );
				}

				mChar.StartTimer( healTimer, 2, true );
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 1498, socket.language )); // You are not close enough to apply the bandages.
		}
	}
}

function IsTargetHealable( ourObj, useVetSkill )
{
	var id = ourObj.id;
	if( useVetSkill )
	{
		return ( validVetTargets.indexOf( id ) != -1 );
	}
	else
	{
		return ( validHealTargets.indexOf( id ) != -1 );
	}
}

function SetSkillInUse( socket, mChar, ourObj, skillNum, healingTime, setVal )
{
	if( setVal )
	{
		if( socket )
		{
			socket.SysMessage( GetDictionaryEntry( 6017, socket.language )); // You begin to apply the bandages.
		}
		mChar.SetTempTag( "SK_HEALINGTYPE", skillNum );
	}
	else
	{
		mChar.SetTempTag( "SK_HEALINGTYPE", 0 );
		mChar.SetTempTag( "SK_HEALINGTARG", 0 );
	}

	if( skillNum == 17 )		// Healing
	{
		mChar.skillsused.healing = setVal;
	}
	else if( skillNum == 39 )	// Veterinary
	{
		mChar.skillsused.veterinary = setVal;
	}

	if( ValidateObject( ourObj ))
	{
		ourObj.SetTempTag( "SK_BEINGHEALED", setVal );
		ourObj.SetTempTag( "SK_HEALINGTIME", healingTime );
		if( setVal )
		{
			mChar.SetTempTag( "SK_HEALINGTARG", ourObj.serial );
		}
	}
}

function onTimer( mChar, timerID )
{
	var skillNum = mChar.GetTempTag( "SK_HEALINGTYPE" );
	var ourObj = CalcCharFromSer( mChar.GetTempTag( "SK_HEALINGTARG" ));
	var socket = mChar.socket;
	if( socket != null )
	{
		if( mChar.dead )
		{
			socket.SysMessage( GetDictionaryEntry( 9083, socket.language )); // You were unable to finish your work before you died.
			return;
		}

		if( !ValidateObject( ourObj ))
		{
			return;
		}
		else if( ourObj.dead && timerID != 0 )
		{
			socket.SysMessage( GetDictionaryEntry( 9086, socket.language )); // You cannot heal that which is not alive.
		}
		else if( mChar.InRange( ourObj, 2 ) && mChar.CanSee( ourObj ))
		{
			switch ( timerID )
			{
				case 0:	// Resurrect
					if( !ourObj.dead )
					{
						socket.SysMessage( GetDictionaryEntry( 9085, socket.language )); // The target is not dead.
					}
					else if( mChar.CheckSkill( skillNum, 800, 1000 ) && mChar.CheckSkill( 1, 800, 1000 ))
					{
						var iMulti = ourObj.multi;
						if( ValidateObject( iMulti ) && ( !iMulti.IsOnOwnerList( mChar ) && !iMulti.IsOnFriendList( mChar )) &&
							( !iMulti.IsOnOwnerList( ourObj ) && !iMulti.IsOnFriendList( ourObj )))
						{
							// Don't resurrect dead players in multis not owned by either target or healer
							socket.SysMessage( GetDictionaryEntry( 9087, socket.language )); // Target cannot be resurrected at that location.
							return;
						}
						else
						{
							ourObj.Resurrect();
							ourObj.StaticEffect( 0x376A, 10, 16 );
							ourObj.SoundEffect( 0x214, true );
							socket.SysMessage( GetDictionaryEntry( 1272, socket.language )); // You successfully resurrected the patient!
						}
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 9084, socket.language )); // You are unable to resurrect your patient.
					}
					break;
				case 1:	// Cure Poison
					if( mChar.CheckSkill( skillNum, 600, 1000 ) && (( skillNum == 17 && mChar.CheckSkill( 1, 600, 1000 )) || ( skillNum == 39 && mChar.CheckSkill( 2, 600, 1000 ))))
					{
						ourObj.SetPoisoned( 0, 0 );
						ourObj.StaticEffect( 0x373A, 0, 15 );
						ourObj.SoundEffect( 0x01E0, true );
						socket.SysMessage( GetDictionaryEntry( 1274, socket.language )); // You have cured the poison.
						var objSock = ourObj.socket;
						if( objSock )
						{
							objSock.SysMessage( GetDictionaryEntry( 1273, objSock.language )); // You have been cured of poison.
						}
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 1494, socket.language )); // You fail to counter the poison.
					}
					break;
				case 2:	// Heal
					var healthLoss = ( ourObj.maxhp - ourObj.health );
					if( healthLoss == 0 )
					{
						socket.SysMessage( GetDictionaryEntry( 1497, socket.language )); // That being is undamaged.
					}
					else if( mChar.CheckSkill( skillNum, 0, healthLoss * 10 )) // Requires higher and higher amount of health lost in order for healer to gain skill
					{
						var mItem;
						var healBonus;
						for( mItem = mChar.FirstItem(); !mChar.FinishedItems(); mItem = mChar.NextItem() ) 
						{
							if( !ValidateObject( mItem ))
								continue;

							healBonus += parseInt( mItem.GetTag( "healingBonus" ) );
						}

						// Increase karma when healing innocent/neutral characters
						if( ourObj != mChar && ( ourObj.innocent || ourObj.neutral ))
						{
							mChar.karma++;
						}

						// Are we healing using the Healing skill, or using the Veterinary skill?
						var healSkill;
						var secondarySkill;
						if( skillNum == 17 ) // Healing
						{
							healSkill = mChar.skills.healing;
							secondarySkill = mChar.skills.anatomy;

							// Perform generic Anatomy skill check to allow skill increase
							mChar.CheckSkill( 1, 0, 1000 );
						}
						else if( skillNum == 39 ) // Veterinary
						{
							healSkill = mChar.skills.veterinary;
							secondarySkill = mChar.skills.animallore;

							// Perform generic Animal Lore skill check to allow skill increase
							mChar.CheckSkill( 2, 0, 1000 );
						}

						// Retrieve amount of times character's hands slipped during healing
						mChar.RemoveScriptTrigger( 4014 ); // Remove healing_slip.js script
						var slipCount = mChar.GetTempTag( "slipCount" );
						if( slipCount > 5 )
						{
							// If hands slip more than 5 times, fail at healing
							socket.SysMessage( GetDictionaryEntry( 9089, socket.language )); // You finish applying the bandages, but they barely help.
							ourObj.Heal( 1, mChar );
						}
						else
						{
							// Minimum amount healed = Anatomy(or Animal Lore)/5 + Healing/5 + 3  Maximum amount healed = Anatomy(or Animal Lore)/5 + Healing/2 + 10
							var minValue = Math.round(( secondarySkill / 50 ) + ( healSkill / 50 ) + 3 );
							var maxValue = Math.round(( secondarySkill / 50 ) + ( healSkill / 20 ) + 10 );
							var healAmt = RandomNumber( minValue, maxValue );

							// Reduce the amount healed with each slip caused by damage taken while healing
							for( var i = 0; i < slipCount; i++ )
							{
								// Reduce health by a percentage (35%) modified by healer's Healing skills and Dexterity for each slip up
								healAmt -= Math.round( healAmt * ( 0.35 - (( Math.round( healSkill / 10 ) + mChar.dexterity ) / 750 )));

								// Example: Healing reduction per slip, based a 35% percentage reduction, adjusted by 100.0 Healing and 100 Dexterity
								// 80 > 74 > 68 > 63 > 58

								// Example: Health reduction based on flat 35% percentage per slip
								// 80 > 52 > 34 > 22 > 14
							}

							if( healAmt <= 1 )
							{
								// Always heal at least 1 hp, if possible
								ourObj.Heal( 1, mChar );
								socket.SysMessage( GetDictionaryEntry( 9089, socket.language )); // You finish applying the bandages, but they barely help.
							}
							else
							{
								ourObj.Heal(healAmt * (healBonus / 100), mChar );
								socket.SysMessage( GetDictionaryEntry( 1271, socket.language )); // You apply the bandages and the patient looks a bit healthier.
							}
						}
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 9089, socket.language )); // You finish applying the bandages, but they barely help.
						ourObj.Heal( 1, mChar );
					}
					break;
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 6018, socket.language )); // You are no longer close enough to heal your target.
		}
	}
	SetSkillInUse( socket, mChar, ourObj, skillNum, 0, false );
}