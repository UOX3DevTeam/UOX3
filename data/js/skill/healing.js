// Main Sources
// 	Publish 71 - https://wiki.stratics.com/index.php?title=UO:Publish_Notes_from_2011-07-21

// Secondary Sources
// 	UOR era and earlier: https://web.archive.org/web/20010608154433/http://uo.stratics.com/content/skills/healing.shtml
// 	LBR era: https://web.archive.org/web/20020806222303/http://uo.stratics.com:80/content/skills/healing.shtml
// 	AoS era: https://web.archive.org/web/20030605153924/http://uo.stratics.com:80/content/skills/healing.shtml
// 	ML/SA era: https://web.archive.org/web/20080718013839/http://uo.stratics.com/content/skills/healing.php
// 	HS era: https://web.archive.org/web/20101229122433/http://uo.stratics.com/content/skills/healing.php

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

// If CoreShardEra is set to T2A or lower, skillgain from healing damage caps out at ~65.0
// Further increases in skill have to come from curing poison (60-80)
// and resurrecting (80-100+)
const coreShardEra = GetServerSetting( "CoreShardEra" );
const healingSkillGainCap = 650;
const cureSkillGainCap = 850;

// If set to 1, overrides the cap of 65.0 healing normally used when coreShardEra is set to T2A or lower
// and instead uses actual skill cap
const overrideCappedT2AHealing = 0;

// Set to specific distance value to override max range regardless of era
const overrideMaxBandageRange = 0;

// If set to 1, scales difficulty of healing based on % of target health lost and on target's overall maxhp
const useDifficultyScalingForHealing = 1;

// This script's ID
const healingScriptID = 4000;

// This is called by onUseBandageMacro event from global script, and skips manual target selection
function onUseCheckedTriggered( pUser, targChar, iUsed )
{
	if( ValidateObject( pUser ) && ValidateObject( targChar ) && targChar.isChar && ValidateObject( iUsed ) && iUsed.isItem )
	{
		// Temporarily store targChar in a custom property on pUser
		pUser.tempHealTarg = targChar;

		// Call onUseChecked directly
		onUseChecked( pUser, iUsed );
	}
	return;
}

function onUseChecked( pUser, iUsed )
{
	if( ValidateObject( pUser ) && ValidateObject( iUsed ) && iUsed.isItem )
	{
		let socket = pUser.socket;
		if( socket == null )
			return false;

		let pLanguage = socket.language;
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
			socket.SysMessage( GetDictionaryEntry( 1971, pLanguage )); // You are too busy to do that.
		}
		else if( socket.GetTimer( Timer.SOCK_SKILLDELAY ) <= GetCurrentClock() )
		{
			socket.tempObj = iUsed;
			socket.SetTimer( Timer.SOCK_SKILLDELAY, 5000 ); // Reset the skill timer

			let healTarg = pUser.tempHealTarg;
			if( healTarg !== undefined && ValidateObject( healTarg ))
			{
				// We already have our target, hit the callback directly
				delete pUser.tempHealTarg;
				onCallback1( socket, healTarg );
			}
			else
			{
				let targMsg = GetDictionaryEntry( 472, pLanguage ); // Who will you use the bandages on?
				socket.CustomTarget( 1, targMsg );
			}
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
	let bItem = socket.tempObj;
	let mChar = socket.currentChar;

	socket.tempObj = null;

	if( ValidateObject( bItem ) && bItem.isItem && ValidateObject( ourObj ) && ourObj.isChar && ValidateObject( mChar ) && mChar.isChar )
	{
		let maxRange = ( overrideMaxBandageRange ? overrideMaxBandageRange : ( coreShardEra >= EraStringToNum( "se" ) ? 2 : 1 )); // Range increased to 2 with SE expansion
		if( mChar.InRange( ourObj, maxRange ) && mChar.CanSee( ourObj ) && Math.abs( mChar.z - ourObj.z ) < 4 )
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

			let healSkill;
			let skillNum;
			let buffIcon;
			let priCliloc;
			let scndCliloc;
			if( IsTargetHealable( ourObj, false ))
			{
				// Target can be healed with Healing skill
				healSkill = mChar.baseskills.healing;
				skillNum = 17;
				buffIcon = 1069;
				priCliloc = 1002082;
				scndCliloc = 1151400;
			}
			else if( IsTargetHealable( ourObj, true ) || ( ourObj.tamed && ourObj.owner ))
			{
				// Target can be healed with Veterinary skill
				healSkill = mChar.baseskills.veterinary;
				skillNum = 39;
				buffIcon = 1101;
				priCliloc = 1002167;
				scndCliloc = 1151400;
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 6014, socket.language )); // You can't heal that!
				return;
			}

			let iMulti = FindMulti( ourObj );
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
			let healTimer = 0;
			let healTimerID = -1;
			let anatSkill = mChar.baseskills.anatomy;
			let loreSkill = mChar.baseskills.animallore;
			let triggerBonusCure = false;
			let affectedByMortalStrike = ourObj.GetTempTag( "blockHeal" );
			if( ourObj.dead || ourObj.GetTag( "isPetDead" )) // Resurrection
			{
				// Resurrection of bonded pet
				if( ourObj.GetTag( "isBondedPet" ))
				{
					// Must have at least 80.0 Vet and 80.0 Lore
					if( healSkill >= 800 && loreSkill >= 800 )
					{
						let isOwner = ourObj.owner;
						let isFriend = false;
						let friendList = ourObj.GetFriendList();

						for( let i = 0; i < friendList.length; i++ )
						{
							if( ValidateObject( friendList[i] ) && friendList[i] == mChar )
							{
								isFriend = true;
								break;
							}
						}
						if( isOwner || isFriend )
						{
							// Resurrecting takes between 8 - 10 seconds depending on dexterity
							if( coreShardEra >= EraStringToNum( "uor" ))
							{
								// UOR introduced dexterity as a modifier for healing timer
								healTimer = 10000 - (( mChar.dexterity / 50 ) * 1000 );
							}
							else// if( coreShardEra >= EraStringToNum( "t2a" ))
							{
								// T2A (Publish May 25, 1999), resurrecting "now" takes 10 seconds flat
								healTimer = 10000;
							}
							/*else
							{
								// Did original UO have a different resurrect timer than T2A?
								// ???
							}*/			

							healTimerID = 0; // Resurrect
						}
						else
						{
							socket.SysMessage( GetDictionaryEntry( 19304, socket.language ));// Only pets bonded with their owner can be resurrected..
						}
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 19303, socket.language ));// You are not skilled enough to resurrect that pet.
						return;
					}
				}
				else
				{
					if( healSkill >= 800 && anatSkill >= 800 )
					{
						// Resurrecting takes between 8 - 10 seconds depending on dexterity
						if( coreShardEra >= EraStringToNum( "uor" ))
						{
							// UOR introduced dexterity as a modifier for healing timer
							healTimer = 10000 - (( mChar.dexterity / 50 ) * 1000 );
						}
						else// if( coreShardEra >= EraStringToNum( "t2a" ))
						{
							// T2A (Publish May 25, 1999), resurrecting "now" takes 10 seconds flat
							healTimer = 10000;
						}
						/*else
						{
							// Did original UO have a different resurrect timer than T2A?
							// ???
						}*/

						healTimerID = 0; // Resurrect
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 1493, socket.language )); // You are not skilled enough to resurrect.
						return;
					}
				}
			}
			else if( ourObj.poison > 0 || ourObj.GetTempTag( "doBleed" ))	// Cure Poison/Bleed
			{
				if( mChar.serial == ourObj.serial && ourObj.health < ourObj.maxhp
					&& !affectedByMortalStrike && coreShardEra >= EraStringToNum( "hs" ) && healSkill >= 800 && anatSkill >= 800 )
				{
					// When targeting self, trigger bonus attempt to remove poison/bleed at half normal heal duration.
					// If successful, will do a reduced bonus heal at end of normal duration
					// If not successful, will just do normal cure attempt at end of normal duration
					triggerBonusCure = true;
				}

				if( healSkill >= 600 && anatSkill >= 600 )
				{
					if( mChar.serial == ourObj.serial )
					{
						if( coreShardEra >= EraStringToNum( "tol" ))
						{
							// TOL: curing self: 8 to 11 seconds
							healTimer = 11000 - (( mChar.dexterity / 40 ) * 1000 );
						}
						else if( coreShardEra >= EraStringToNum( "hs" ))
						{
							// HS: curing self: 10 to 15 seconds
							healTimer = 15000 - (( mChar.dexterity / 24 ) * 1000 );
						}
						else if( coreShardEra >= EraStringToNum( "aos" ))
						{
							// AOS: Curing Self takes 9 to 15 seconds depending on dexterity
							healTimer = 15000 - (( mChar.dexterity / 20 ) * 1000 );
						}
						else if( coreShardEra >= EraStringToNum( "uor" ))
						{
							// UOR: Curing Self takes 13 to 18 seconds depending on dexterity
							healTimer = 18000 - (( mChar.dexterity / 24 ) * 1000 );
						}
						else
						{
							// T2A: Curing self takes 18 seconds flat
							healTimer = 18000;
						}
					}
					else
					{
						if( coreShardEra >= EraStringToNum( "hs" ))
						{
							// HS: curing others: 3-5 seconds depending on dexterity
							healTimer = 5000 - (( mChar.dexterity / 40 ) * 1000 );
						}
						else if( coreShardEra >= EraStringToNum( "uor" ))
						{
							// UOR and later: Curing others takes 4 to 6 seconds depending on dexterity
							healTimer = 6000 - (( mChar.dexterity / 50 ) * 1000 );
						}
						else if( coreShardEra >= EraStringToNum( "t2a" ))
						{
							// T2A: Cure others takes 6 seconds flat
							healTimer = 6000;
						}
						else
						{
							// Original UO
							healTimer = 18000; // Curing self/others takes 18 seconds
						}
					}

					// Cut cure timer in two if we're trying to "cure" a bleed (which only lasts 10 seconds)
					if( ourObj.GetTempTag( "doBleed" ))
					{
						healTimer /= 2;
					}

					healTimerID = 1; // Cure
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 1495, socket.language )); // You are not skilled enough to cure poison.
					socket.SysMessage( GetDictionaryEntry( 1496, socket.language )); // The poison in your target's system counters the bandage's effect.
					return;
				}
			}
			else if( ourObj.health == ourObj.maxhp )
			{
				socket.SysMessage( GetDictionaryEntry( 1497, socket.language )); // That being is undamaged.
				return;
			}

			// Mortal Strike effect prevents healing attempts
			if( healTimerID == -1 && affectedByMortalStrike )
			{
				if( ourObj != mChar && ourObj.socket )
				{
					socket.SysMessage( "You cannot heal that target in their current state." );
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 9058, socket.language ));// You can not heal yourself in your current state.
				}
				return;
			}

			if( healTimerID == -1 ) // Not already ressing/curing, let's heal
			{
				if( mChar.serial == ourObj.serial )
				{
					// Healing self
					if( coreShardEra >= EraStringToNum( "hs" ))
					{
						// HS and beyond, healing self takes 5 - 8 seconds (max duration reduced with Publish 71)
						healTimer = 8000 - (( mChar.dexterity / 50 ) * 1000 );
					}
					else if( coreShardEra >= EraStringToNum( "ml" ))
					{
						// ML and beyond: Healing self takes 4 to 11 seconds
						healTimer = 11000 - (( mChar.dexterity / 17.12 ) * 1000 )
					}
					else if( coreShardEra >= EraStringToNum( "se" ))
					{
						// SE: Healing self takes 5 to 10 seconds (Publish 21)
						healTimer = 10000 - (( mChar.dexterity / 24 ) * 1000 );
					}
					else if( coreShardEra >= EraStringToNum( "aos" ))
					{
						// AOS: Healing self takes 9 to 16 seconds
						healTimer = ( 9.4 + ( 0.6 * (( 120 - mChar.dexterity ) / 10 ))) * 1000;
					}
					else if( coreShardEra >= EraStringToNum( "uor" ))
					{
						// T2A to LBR: Healing self takes 11 to 15 seconds
						healTimer = ( 11 + ( 0.4 * (( 100 - mChar.dexterity ) / 10 ))) * 1000;
					}
					else
					{
						// Original UO/T2A: Healing self takes 15 seconds flat
						healTimer = 15000;
					}
				}
				else
				{
					// Healing others
					if( coreShardEra >= EraStringToNum( "ml" ))
					{
						// ML and beyond: healing others takes 2 to 4 seconds (4 - ( dex / 60 ))
						healTimer = 4000 - (( mChar.dexterity / 60 ) * 1000 );
					}
					else if( coreShardEra >= EraStringToNum( "uor" ))
					{
						// AoS and earlier: Healing others takes 3 to 5 seconds, depending on dexterity
						healTimer = 5000 - (( mChar.dexterity / 50 ) * 1000 );
					}
					else if( coreShardEra >= EraStringToNum( "t2a" ))
					{
						// t2a: Healing others takes 5 seconds flat (Publish May 25, 1999)
						healTimer = 5000;
					}
					else
					{
						// Original UO
						healTimer = 15000; // Healing self/others takes 15 seconds flat
					}
				}

				healTimerID = 2; // Heal
			}

			// Section below is SHARED between resurrect/cure/heal

			// Consume some bandages
			ConsumeBandage( bItem, 1 );

			// Flag healer as criminal if target is murderer/criminal
			FlagHealer( mChar, ourObj );

			// Inform the target
			if( ourObj != mChar && ourObj.socket )
			{
				let tempMsg = GetDictionaryEntry( 6016, ourObj.socket.language ); // %s is attempting to heal you.
				ourObj.SysMessage( tempMsg.replace( /%s/gi, mChar.name ));
			}

			mChar.AddScriptTrigger( 4014 ); // Add healing_slip.js script
			SetSkillInUse( socket, mChar, ourObj, skillNum, healTimer, true );

			// Add buff to target or yourself
			let seconds = Math.round( healTimer / 1000 );
			if( ourObj != mChar && ourObj.socket )
			{
				TriggerEvent( 2204, "AddBuff", ourObj, buffIcon, priCliloc, scndCliloc, seconds, " " + ourObj.name );
			}
			else
			{
				TriggerEvent( 2204, "AddBuff", mChar, buffIcon, priCliloc, scndCliloc, seconds, " " + ourObj.name );
			}

			if( triggerBonusCure )
			{
				mChar.SetTempTag( "triggerBonusCure", true );

				// Start a timer to attempt poison curing at half the regular time to cure
				mChar.StartTimer( healTimer / 2, 1, healingScriptID );
			}
			else
			{
				mChar.SetTempTag( "triggerBonusCure", null ); // Just making sure we clear this
			}

			mChar.StartTimer( healTimer, healTimerID, healingScriptID );
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 1498, socket.language )); // You are not close enough to apply the bandages.
		}
	}
}

function ConsumeBandage( bItem, amt )
{
	if( bItem.amount > amt )
	{
		bItem.amount = bItem.amount - amt;
	}
	else
	{
		bItem.Delete();
	}
}

function FlagHealer( mChar, ourObj )
{
	if( ourObj.murderer || ourObj.criminal )
	{
		mChar.criminal = true;
	}
}

function IsTargetHealable( ourObj, useVetSkill )
{
	let id = ourObj.id;
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
		ourObj.SetTempTag( "SK_HEALTIMER", healingTime );
		if( setVal )
		{
			mChar.SetTempTag( "SK_HEALINGTARG", ourObj.serial );
		}
	}
}

function onTimer( mChar, timerID )
{
	if( !ValidateObject( mChar ))
		return;

	let skillNum = mChar.GetTempTag( "SK_HEALINGTYPE" );
	let ourObj = CalcCharFromSer( mChar.GetTempTag( "SK_HEALINGTARG" ));
	if( !ValidateObject( ourObj ))
	{
		SetSkillInUse( socket, mChar, null, skillNum, 0, false );
		return;
	}

	let primarySkill = 0;
	let secondarySkill = 0;
	let primarySkillCap = 0;
	let secondarySkillCap = 0;
	switch( skillNum )
	{
		case 17:
			primarySkill = mChar.skills.healing;
			secondarySkill = mChar.skills.anatomy;
			primarySkillCap = mChar.skillCaps.healing;
			secondarySkillCap = mChar.skillCaps.anatomy;
			break;
		case 39:
			primarySkill = mChar.skills.veterinary;
			secondarySkill = mChar.skills.animallore;
			primarySkillCap = mChar.skillCaps.veterinary;
			secondarySkillCap = mChar.skillCaps.animallore;
			break;
	}

	let socket = mChar.socket;
	if( socket != null )
	{
		if( mChar.dead )
		{
			socket.SysMessage( GetDictionaryEntry( 9083, socket.language )); // You were unable to finish your work before you died.
			return;
		}

		let maxRange = ( overrideMaxBandageRange ? overrideMaxBandageRange : ( coreShardEra >= EraStringToNum( "se" ) ? 2 : 1 )); // Range increased to 2 with SE expansion
		if( ourObj.dead && timerID != 0 )
		{
			socket.SysMessage( GetDictionaryEntry( 9086, socket.language )); // You cannot heal that which is not alive.
		}
		else if( mChar.InRange( ourObj, maxRange ) && mChar.CanSee( ourObj ))
		{
			// Retrieve amount of times character's hands slipped during healing
			let slipCount = mChar.GetTempTag( "slipCount" );

			if( mChar.GetTempTag( "bonusCureLevel" ))
			{
				// Poison/Bleed was cured with bonus attempt already!
				// Proceed to do a small heal as a reward instead of continuing with main cure attempt
				timerID = 2;
			}

			switch ( timerID )
			{
				case 0:	// Resurrect
					if( !ourObj.dead && !ourObj.GetTag( "isPetDead" ) )
					{
						socket.SysMessage( GetDictionaryEntry( 9085, socket.language )); // The target is not dead.
						break;
					}

					// Base 25% chance to resurrect at 80.0 healing and 80.0 anatomy
					// Base 65% chance to resurrect at 100.0 healing and 100.0 anatomy
					// Base 100% (clamped) chance to resurrect at 120.0 healing and 120.0 anatomy
					let chanceToRes = 0.25 + (((( primarySkill + secondarySkill ) / 2 ) - 800 ) * 0.002 );

					// Reduce chance by 2% per number of times healer's fingers "slipped" during bandaging
					chanceToRes -= ( slipCount * 0.02 );

					// Clamp at 0.0 - 1.0
					chanceToRes = Math.max( 0.0, Math.min( 1.0, chanceToRes ));

					let resResult = -1;
					if( chanceToRes >= RandomNumber( 0.0, 1.0 ))
					{
						// Arise, and live!
						let iMulti = ourObj.multi;
						if( ValidateObject( iMulti ) && ( !iMulti.IsOnOwnerList( mChar ) && !iMulti.IsOnFriendList( mChar )) &&
							( !iMulti.IsOnOwnerList( ourObj ) && !iMulti.IsOnFriendList( ourObj )))
						{
							// Don't resurrect dead players in multis not owned by either target or healer
							socket.SysMessage( GetDictionaryEntry( 9087, socket.language )); // Target cannot be resurrected at that location.
							mChar.RemoveScriptTrigger( 4014 ); // Remove healing_slip.js script
							mChar.SetTempTag( "slipCount", null );
							return;
						}
						else
						{
							if ( ourObj.GetTag( "isPetDead" ))
							{
								onPetResurrect( socket, ourObj );
								ourObj.StaticEffect( 0x376A, 10, 16 );
								ourObj.SoundEffect( 0x214, true );
								socket.SysMessage( GetDictionaryEntry( 19306, socket.language )); // You successfully resurrected the pet!
								resResult = 1;
								mChar.RemoveScriptTrigger( 4014 ); // Remove healing_slip.js script
								mChar.SetTempTag( "slipCount", null );
							}
							else
							{
								ourObj.Resurrect();
								ourObj.StaticEffect( 0x376A, 10, 16 );
								ourObj.SoundEffect( 0x214, true );
								socket.SysMessage( GetDictionaryEntry( 1272, socket.language )); // You successfully resurrected the patient!
								resResult = 1;
								mChar.RemoveScriptTrigger( 4014 ); // Remove healing_slip.js script
								mChar.SetTempTag( "slipCount", null );
							}
						}
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 9084, socket.language )); // You are unable to resurrect your patient.
					}

					// Run skill checks for chance to gain skill, but override the check to match up with the manual check done above, so a success is still a success, and a failure is still a failure
					mChar.CheckSkill( skillNum, 800, primarySkillCap, false, resResult ); // Primary skill
					mChar.CheckSkill(( skillNum == 17 ? 1 : 2 ), 800, secondarySkillCap, false, resResult ); // Secondary skill

					break;
				case 1:	// Cure Poison
					let chanceToCure = 0;
					let poisonLvl = ourObj.poison;
					let cureBleed = ourObj.GetTempTag( "doBleed" );
					let triggerBonusCure = mChar.GetTempTag( "triggerBonusCure" );
					if( triggerBonusCure )
					{
						// Chance for a "half heal duration" cure
						chanceToCure = (((( primarySkill + secondarySkill ) / 10 ) - 120 ) * 25 ) / (( cureBleed ? 3 : Math.max( 1, poisonLvl )) * 20 );
					}
					else
					{
						// Base 80% chance to cure level 1 poison at 60.0 healing and 60.0 anatomy
						// Base 92% chance at 100.0 healing and 100.0 anatomy
						// Base 100% (clamped) chance at 120.0 healing and 120.0 anatomy
						chanceToCure = 0.80 + (((( primarySkill + secondarySkill ) / 2 ) - 600 ) * 0.00035 );

						// Reduce chance by 10% per poison level on target
						if( useDifficultyScalingForHealing )
						{
							chanceToCure -= ((( cureBleed ? 3 : Math.max( 1, poisonLvl )) - 1 ) * 0.1 );
						}
					}

					// Reduce chance by 2% per number of times healer's fingers "slipped" during bandaging
					chanceToCure -= ( slipCount * 0.02 );

					// Clamp at 0.0 - 1.0
					chanceToCure = Math.max( 0.0, Math.min( 1.0, chanceToCure ));

					let cureResult = -1;
					if( chanceToCure >= RandomNumber( 0.0, 1.0 ))
					{
						// Cured!
						cureResult = 1;
						if( triggerBonusCure )
						{
							// Update combined cure/heal tag to reflect level of poison that was cured
							// we'll use this later in the bonus heal-portion
							mChar.SetTempTag( "triggerBonusCure", null );
							mChar.SetTempTag( "bonusCureLevel", ( cureBleed ? 3 : poisonLvl ));
						}
						else
						{
							// Remove these, as this was the final cure attempt
							mChar.RemoveScriptTrigger( 4014 ); // Remove healing_slip.js script
							mChar.SetTempTag( "slipCount", null );
						}

						if( cureBleed )
						{
							// Cure the Bleed!
							if( ourObj != mChar && ourObj.socket )
							{
								ourObj.socket.SysMessage( GetDictionaryEntry( 8259, ourObj.socket.language )); // The bleeding wounds have healed, you are no longer bleeding!
							}
							else
							{
								socket.SysMessage( GetDictionaryEntry( 8258, socket.language )); // You bind the wound and stop the bleeding
							}
							ourObj.KillJSTimer( 9300, 7001 ); // Stop the bleed timer on target
							ourObj.SetTempTag( "doBleed", null );
						}
						else
						{
							ourObj.SetPoisoned( 0, 0 );
							ourObj.StaticEffect( 0x373A, 0, 15 );
							ourObj.SoundEffect( 0x01E0, true );
							socket.SysMessage( GetDictionaryEntry( 1274, socket.language )); // You have cured the poison.
							let objSock = ourObj.socket;
							if( objSock && objSock != mChar.socket ) // Don't spam the player
							{
								objSock.SysMessage( GetDictionaryEntry( 1273, objSock.language )); // You have been cured of poison.
							}
						}
					}
					else
					{
						if( !triggerBonusCure )
						{
							if( cureBleed )
							{
								socket.SysMessage( GetDictionaryEntry( 9089, socket.language )); // You finish applying the bandages, but they barely help.
							}
							else
							{
								socket.SysMessage( GetDictionaryEntry( 1494, socket.language )); // You fail to counter the poison.
							}
						}
						mChar.SetTempTag( "triggerBonusCure", null );
					}

					// Run skill checks for chance to gain skill, but override the check to match up with the manual check done above, so a success is still a success, and a failure is still a failure
					let useSkillGainCaps = ( !overrideCappedT2AHealing && coreShardEra <= EraStringToNum( "t2a" ));
					mChar.CheckSkill( skillNum, 600, ( useSkillGainCaps ? cureSkillGainCap : primarySkillCap ), false, cureResult ); // Primary skill
					mChar.CheckSkill(( skillNum == 17 ? 1 : 2 ), 600, ( useSkillGainCaps ? cureSkillGainCap : secondarySkillCap ), false, cureResult ); // Secondary skill
					break;
				case 2:	// Heal
					let upperBound = 1000;
					let healthLoss = ( ourObj.maxhp - ourObj.health );
					if( healthLoss == 0 )
					{
						socket.SysMessage( GetDictionaryEntry( 1497, socket.language )); // That being is undamaged.
						mChar.SetTempTag( "bonusCureLevel", null );
						break;
					}

					if( !overrideCappedT2AHealing && coreShardEra <= EraStringToNum( "t2a" ))
					{
						// T2A and earlier, skill-gain from normal healing caps out at around ~65.0
						upperBound = healingSkillGainCap; // 650
					}
					else
					{
						// UOR and later, skill gain from normal healing caps out at actual skill cap
						upperBound = primarySkillCap;
					}

					if( useDifficultyScalingForHealing )
					{
						// Scale difficulty based on %loss of health on target + maxHP
						// i.e. requires higher and higher amount of health lost in order for healer to gain skill
						let normalizedHealthLoss = healthLoss / ourObj.maxhp; // get a value between 0.0 and 1.0
						let idealHealAmtPerSkill = Math.max( 10, primarySkill * 0.155 );
						let healTrainingEffectiveness = Math.min( 1.0, ourObj.maxhp / idealHealAmtPerSkill );
						upperBound = Math.min( upperBound, Math.round( normalizedHealthLoss * primarySkillCap * healTrainingEffectiveness ));
					}

					// Perform skill check
					if( mChar.CheckSkill( skillNum, 0, upperBound ))
					{
						let mItem;
						let healBonus = 0;
						for( mItem = mChar.FirstItem(); !mChar.FinishedItems(); mItem = mChar.NextItem() )
						{
							if( !ValidateObject( mItem ))
								continue;

							healBonus += parseInt( mItem.GetTag( "healingBonus" ));
						}

						// Increase karma when healing innocent/neutral characters
						if( ourObj != mChar && ( ourObj.innocent || ourObj.neutral ))
						{
							mChar.karma++;
						}

						// Are we healing using the Healing skill, or using the Veterinary skill?
						if( skillNum == 17 ) // Healing
						{
							// Perform generic Anatomy skill check to allow skill increase
							mChar.CheckSkill( 1, 0, secondarySkillCap );
						}
						else if( skillNum == 39 ) // Veterinary
						{
							// Perform generic Animal Lore skill check to allow skill increase
							mChar.CheckSkill( 2, 0, secondarySkillCap );
						}

						if( slipCount > 5 )
						{
							// If hands slip more than 5 times, fail at healing
							socket.SysMessage( GetDictionaryEntry( 9089, socket.language )); // You finish applying the bandages, but they barely help.
							ourObj.Heal( 1, mChar );
						}
						else
						{
							// Minimum amount healed = Anatomy(or Animal Lore)/5 + Healing/5 + 3  Maximum amount healed = Anatomy(or Animal Lore)/5 + Healing/2 + 10
							let minValue = 0;
							let maxValue = 0;
							if( coreShardEra >= EraStringToNum( "se" ))
							{
								// SE and above, max heal at GM level is 36-60
								minValue = Math.round(( secondarySkill / 80 ) + ( primarySkill / 50 ) + 4 );
								maxValue = Math.round(( secondarySkill / 60 ) + ( primarySkill / 25 ) + 4 );
							}
							else
							{
								// AoS and below, max heal at GM level is 43-80
								minValue = Math.round(( secondarySkill / 50 ) + ( primarySkill / 50 ) + 3 );
								maxValue = Math.round(( secondarySkill / 50 ) + ( primarySkill / 20 ) + 10 );
							}
							let healAmt = RandomNumber( minValue, maxValue );

							// Reduce the amount healed with each slip caused by damage taken while healing
							// T2A and earlier, this amount is reduced by dexterity stat
							// UOR and later, amount is reduced by dexterity stat & healing skill
							let slipModifier = ( coreShardEra >= EraStringToNum( "uor" ) ? ( Math.round( primarySkill / 10 ) + mChar.dexterity ) : ( mChar.dexterity )) / 750;
							for( let i = 0; i < slipCount; i++ )
							{
								// Reduce healing done by a percentage (35%) modified by healer's Healing skills and Dexterity for each slip up
								healAmt -= Math.round( healAmt * ( 0.35 - slipModifier ));

								// Example: Healing reduction per slip, based a 35% percentage reduction, adjusted by 100.0 Healing and 100 Dexterity
								// 80 > 74 > 68 > 63 > 58

								// Example: Healing reduction based on flat 35% percentage per slip
								// 80 > 52 > 34 > 22 > 14
							}

							if( ourObj == mChar && coreShardEra >= EraStringToNum( "hs" ))
							{
								let poisonLvlCured = mChar.GetTempTag( "bonusCureLevel" );
								if( poisonLvlCured > 0 )
								{
									// Reduce healing amount proportional to level of poison cured since we did a combined curing/healing action
									healAmt /= poisonLvlCured;
								}
							}

							if( healAmt <= 1 )
							{
								// Always heal at least 1 hp, if possible
								ourObj.Heal( 1, mChar );
								socket.SysMessage( GetDictionaryEntry( 9089, socket.language )); // You finish applying the bandages, but they barely help.
							}
							else
							{
								ourObj.Heal( healAmt * Math.max( 1, ( 1 + ( healBonus / 100 ))), mChar );
								socket.SysMessage( GetDictionaryEntry( 1271, socket.language )); // You apply the bandages and the patient looks a bit healthier.
							}
						}
					}
					else
					{
						// Skill Check failed
						socket.SysMessage( GetDictionaryEntry( 9089, socket.language )); // You finish applying the bandages, but they barely help.
					}

					mChar.SetTempTag( "bonusCureLevel", null );
					mChar.RemoveScriptTrigger( 4014 ); // Remove healing_slip.js script
					mChar.SetTempTag( "slipCount", null );
					break;
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 6018, socket.language )); // You are no longer close enough to heal your target.
		}
	}
	if( !mChar.GetTempTag( "bonusCureLevel" ))
	{
		SetSkillInUse( socket, mChar, ourObj, skillNum, 0, false );
	}
}

function onPetResurrect( socket, deadPet )
{
	var petsAI = deadPet.GetTag( "PetsAI" );
	var petsHue = deadPet.GetTag( "PetsHue" );

	TriggerEvent( 3108, "SendNpcGhostMode", socket, 0, deadPet.serial, 0  );
	deadPet.aitype = petsAI;
	deadPet.colour = petsHue;
	deadPet.target = null;
	deadPet.atWar = false;
	deadPet.attacker = null;
	deadPet.SetTag( "isPetDead", false );
}