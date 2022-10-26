// Dungeon Trap script
// Last Updated: 10/11/2021
//
// Handles functionality of traps in dungeons like...
// 	...Spike Traps
//  ...Giant Spike Trap
//	...Saw Traps
// 	...Gas Traps
//	...Fire Column Trap
// 	...Stone Face Traps
//	...Mushroom Trap
// 	...Flamespurt Trap
//
// Like other traps, these can be disarmed by someone with sufficient
// Remove Trap skill, upon which they'll remain inactive for a set amount of time
//
// To be handled: axe traps

const inactiveTimer = 60; // After this amount of seconds, an inactive trap will reactivate

function onCollide( pSock, pChar, iTrap )
{
	if( !ValidateObject( pChar ) || !ValidateObject( iTrap ) || pChar.npc || ( !pChar.npc && ( pChar.dead || pChar.isGM || pChar.isCounselor )))
		return false;

	// Is trap in a disarmed/already active state? Do nothing
	var iMoreZPart1 = iTrap.GetMoreVar( "morez", 1 ); // Trap set
	if( iMoreZPart1 == 0 || iTrap.morey == 1 )
		return false;

	// Compare player's Remove Trap skill to the difficulty of the trap
	var iMoreZPart4 = iTrap.GetMoreVar( "morez", 4 ); // Maxskill
	if(( pChar.skills.removetrap / 10 >= 1000 ) || ( RandomNumber( 0, pChar.skills.removetrap / 10 ) > iMoreZPart4 ))
	{
		// Players with GM Remove Trap skill or higher can move past traps without triggering the traps
		// Anyone with less skill, but higher than trap difficulty have a chance of not triggering the trap
		pChar.TextMessage( GetDictionaryEntry( 9100, pSock.language ), false, 0x3b2, 0, pChar.serial ); // *avoids trap*
		return true;
	}

	var trapFX = 0;
	var trapSFX = 0;
	var trapFxLength = 0;
	switch( iTrap.id )
	{
		case 0x1108: // inactive wall spike trap
		case 0x111b: // inactive wall spike trap
			trapFxLength = ( !trapFxLength ? 0x10 : trapFxLength );
			// fallthrough
		case 0x119a: // inactive floor spike trap
		case 0x11a0: // inactive floor spike trap
			trapFxLength = ( !trapFxLength ? 0x09 : trapFxLength );
			trapSFX = ( !trapSFX ? 0x022c : trapSFX );

			iTrap.morex = 5; // 5 seconds
			iTrap.morey = 1; // trap active

			// Play sound effect of trap
			iTrap.SoundEffect( trapSFX, true );

			trapFX = iTrap.id + 1;

			// Play a trap effect
			DoStaticEffect( iTrap.x, iTrap.y, iTrap.z, trapFX, 0x0, trapFxLength, false );

			if( iTrap.id == 0x119a || iTrap.id == 0x11a0 )
			{
				iTrap.id += 2;
			}

			// Search for characters the trap can damage
			AreaCharacterFunction( "SearchForNearbyChars", iTrap, 0 );
			break;
		case 0x1d99: // inactive giant spike trap
			trapFxLength = ( !trapFxLength ? 0x2c : trapFxLength );
			trapSFX = ( !trapSFX ? 0x022c : trapSFX );

			iTrap.morex = 0; // 0 seconds
			iTrap.morey = 1; // trap active

			// Play sound effect of trap
			iTrap.SoundEffect( trapSFX, true );

			// Play a trap effect
			DoStaticEffect( iTrap.x, iTrap.y, iTrap.z, iTrap.id, 0x0, trapFxLength, false );

			// Search for characters the trap can damage
			AreaCharacterFunction( "SearchForNearbyChars", iTrap, 0 );
			break;
		case 0x1103: // inactive saw trap (north wall)
		case 0x1116: // inactive saw trap (west wall)
		case 0x11ac: // inactive saw trap (north floor)
		case 0x11b1: // inactive saw trap (west floor)
			trapSFX = ( !trapSFX ? 0x021c : trapSFX );
			// Set a duration for the trap
			iTrap.morex = 5; // 5 seconds

			// Activate the trap
			iTrap.id++;

			// Play sound effect of trap
			iTrap.SoundEffect( trapSFX, true );

			// Search for characters the trap can damage
			AreaCharacterFunction( "SearchForNearbyChars", iTrap, 0 );
			break;
		case 0x11c0: // inactive fire column trap
			// Activate the trap
			DoStaticEffect( iTrap.x, iTrap.y, iTrap.z, iTrap.id + 1, 0x10, 0x15, false );

			// Set a duration for the trap
			iTrap.morex = 5; // 5 seconds

			// Play sound effect of trap
			iTrap.SoundEffect( 0x0225, true );

			// Set a duration for the trap
			iTrap.morex = 5; // 5 seconds
			iTrap.SoundEffect( 0x004a, true );
			iTrap.StartTimer( 500, 0, true );
			return true;;
		case 0x1125: // inactive mushroom trap
			// Set a duration for the trap
			iTrap.morex = 2; // 2 seconds

			DoStaticEffect( iTrap.x, iTrap.y, iTrap.z, iTrap.id + 1, 0x10, 0x15, false );
			//iTrap.StartTimer( 1000, 1, true );
			break;
		case 0x113c: // inactive gas trap
		case 0x1147: // inactive gas trap
		case 0x11a8: // inactive gas trap
			// Activate the trap
			iTrap.id -= 2;

			// Play sound effect of trap
			iTrap.SoundEffect( 0x0231, true );

			// Set a duration for the trap
			iTrap.morex = 5; // 5 seconds

			// Search for characters the trap can damage
			AreaCharacterFunction( "SearchForNearbyChars", iTrap, 0 );
			break;
		case 0x10f5: // stone face (nw corner?)
		case 0x10fc: // stone face (north wall)
		case 0x110f: // stone face (west wall)
			// Set a duration for the trap
			iTrap.morex = 3; // 3 seconds
			break;
		default:
			return false;
	}

	// If a valid trap was detected, start timer
	iTrap.SoundEffect( 0x004a, true );
	iTrap.StartTimer( 1000, 0, true );
	return false;
}

function DeactivateTrap( iTrap )
{
	iTrap.StartTimer( inactiveTimer * 1000, 125, true );
}

function onTimer( timerObj, timerID )
{
	if( !ValidateObject( timerObj ))
		return;

	// Fetch trap duration from trap's morex value
	var trapDuration = timerObj.morex;

	if( timerID >= 0 && timerID < trapDuration )
	{
		var trapFxLength = 0;
		switch( timerObj.id )
		{
			case 0x1108: // inactive spike trap
			case 0x111b: // inactive spike trap
				trapFxLength = ( !trapFxLength ? 0x10 : trapFxLength );
			case 0x119c: // inactive spike trap
			case 0x11a2: // inactive spike trap
				trapFxLength = ( !trapFxLength ? 0x09 : trapFxLength );

				// Play sound effect of trap
				timerObj.SoundEffect( 0x022c, true );

				// Play a trap effect
				if( timerObj.id == 0x119c || timerObj.id == 0x11a2 )
				{
					DoStaticEffect( timerObj.x, timerObj.y, timerObj.z, timerObj.id - 1, 0x10, trapFxLength, false );
				}
				else
				{
					DoStaticEffect( timerObj.x, timerObj.y, timerObj.z, timerObj.id + 1, 0x10, trapFxLength, false );
				}
				break;
			case 0x11c0: // fire column trap
				timerObj.id = 0x11c5;
				timerObj.dir = 1;
				// fallthrough
			case 0x11c5: // fire column trap
				DoStaticEffect( timerObj.x, timerObj.y, timerObj.z + 1, 0x3709, 0x09, 0x1e, false );
				break;
			case 0x10f5: // stone face trap (initial)
			case 0x10fc: // stone face trap (initial)
			case 0x110f: // stone face trap (initial)
				timerObj.id += 3;
				// fallthrough
			case 0x10f8: // stone face trap (active)
			case 0x10ff: // stone face trap (active)
			case 0x1112: // stone face trap (active)
				if( timerID == 0 )
				{
					timerObj.SoundEffect( 0x54, true ); // Play only once
				}
				DoStaticEffect( timerObj.x, timerObj.y, timerObj.z, timerObj.id - 1, 30, 0x15, false );
				break;
			case 0x1104: // saw trap
			case 0x1117: // saw trap
			case 0x11ad: // saw trap
			case 0x11b2: // saw trap
				if( timerID == 2 )
				{
					timerObj.SoundEffect( 0x021c, true ); // Repeat sound effect half ways
				}
				break;
			case 0x1125: // mushroom trap
				DoStaticEffect( timerObj.x, timerObj.y, timerObj.z, timerObj.id + 1, 0x10, 0x15, false );
				break;
			default:
				break;
		}

		// Deal damage periodically; every time timerID is below the trapDuration
		AreaCharacterFunction( "SearchForNearbyChars", timerObj, 0 );
		timerObj.StartTimer( 1000, timerID + 1, true );
	}
	else if( timerID == trapDuration )
	{
		switch( timerObj.id )
		{
			//case 0x1108: // spike trap
			//case 0x111b: // spike trap
			case 0x119c: // spike trap
			case 0x11a2: // spike trap
				timerObj.id -= 2;
				break;
			case 0x1104: // saw trap (north wall)
			case 0x1117: // saw trap (west wall)
			case 0x11ad: // saw trap (north floor)
			case 0x11b2: // saw trap (west floor)
				timerObj.id--;
				break;
			case 0x113d: // gas trap
			case 0x113a: // gas trap
			case 0x1145: // gas trap
			case 0x11a6: // gas trap
				timerObj.id += 2;
				//timerObj.visible = 3;
				break;
			case 0x11c5: // fire column trap
				timerObj.id = 0x11c0;
				break;
			case 0x10f8: // stone face trap (nw corner)
			case 0x10ff: // stone face trap (north wall)
			case 0x1112: // stone face trap (west wall)
				timerObj.id -= 3;
				break;
			case 0x1125: // mushroom trap
				AreaCharacterFunction( "SearchForNearbyChars", timerObj, 0 );
				break;
			default:
				break;
		}

		// Reset trap
		timerObj.morey = 0;
	}
	else if( timerID == 125 )
	{
		// The trap was previously disarmed. Time to reactivate it
		// by setting first part of morez value back to 1!
		timerObj.SetMoreVar( "morez", 1, 1 );
	}
}

function SearchForNearbyChars( iTrap, trgChar )
{
	if( ValidateObject( trgChar ) && (( !trgChar.npc && trgChar.online && !trgChar.dead ) || trgChar.npc ) && trgChar.vulnerable )
	{
		// Trap has a valid target - apply damage!
		ApplyTrapDamage( iTrap, trgChar );
	}
}

function ApplyTrapDamage( iTrap, trgChar )
{
	switch( iTrap.id )
	{
		case 0x1108: // spike trap
		case 0x111b: // spike trap
		case 0x119c: // spike trap
		case 0x11a2: // spike trap
		case 0x1d99: // giant spike trap
		case 0x1104: // saw trap (north wall)
		case 0x1117: // saw trap (west wall)
		case 0x11ad: // saw trap (north floor)
		case 0x11b2: // saw trap (west floor)
		case 0x11c5: // fire column trap
		case 0x10f8: // stone face trap
		case 0x10ff: // stone face trap
		case 0x1112: // stone face trap
			// Fetch Trap Damage Multiplier from morez
			var trapDamageMultiplier = iTrap.GetMoreVar( "morez", 2 );

			// Apply damage to character
			trgChar.Damage( RandomNumber( iTrap.lodamage, iTrap.hidamage ) * parseInt( trapDamageMultiplier )); // Deal 6 to 36 damage

			// Make character react to damage
			if( trgChar.gender == 0 )
			{
				trgChar.SoundEffect( 343, false );
			}
			else if( trgChar.gender == 1 )
			{
				trgChar.SoundEffect( 806, false );
			}
			trgChar.TextMessage( "Ouch!", false );
			break;
		case 0x113d: // gas trap
		case 0x113a: // gas trap
		case 0x1145: // gas trap
		case 0x11a6: // gas trap
		case 0x1125: // mushroom trap
			// Fetch poison level from trap
			var poisonLevel = iTrap.GetMoreVar( "morez", 2 );
			trgChar.SetPoisoned( poisonLevel, 180*1000 );
			break;
		default:
			break;
	}
}

// Used by the flameSpurtTrap
function onMoveDetect( iTrap, pChar, rangeToChar, oldCharX, oldCharY )
{
	if( !ValidateObject( pChar ) || !pChar.isChar || pChar.npc || pChar.dead || pChar.isGM || pChar.isCounselor || pChar.visible == 3 )
		return;

	if( iTrap.id != 0x11c4 )
		return;

	// Grab some info from the trap
	var maxRange = iTrap.GetMoreVar( "more", 1 );
	if( maxRange < 0x04 )
	{
		maxRange = 0x04; // Min range for this trap to work correctly
	}
	var oldRangeX = Math.abs( iTrap.x - oldCharX );
	var oldRangeY = Math.abs( iTrap.y - oldCharY );
	var flameItem = CalcItemFromSer( parseInt( iTrap.GetTempTag( "flameItem" )));

	// Handle player walking out of range
	if( rangeToChar >= maxRange )
	{
		// Destroy the associated flame item, if any exists
		if( ValidateObject( flameItem ))
		{
			flameItem.Delete();
			iTrap.SetTempTag( "flameItem", null );
		}
	}
	// Handle what happens when player enters a range of 3 tiles from the trap
	if( rangeToChar <= 0x03 && !ValidateObject( flameItem ))
	{
		flameItem = CreateDFNItem( null, null, "flameSpurtTrap_FX", 1, "ITEM", false );
		if( ValidateObject( flameItem ))
		{
			iTrap.SetTempTag( "flameItem", (flameItem.serial).toString() );
			flameItem.dir = 1;
			flameItem.decaytime = 10;
			flameItem.Teleport( iTrap );
			flameItem.SoundEffect( 0x011d, true );
		}
	}
	if( rangeToChar == 0x01 && ValidateObject( flameItem ))
	{
		pChar.Damage( RandomNumber( 0x01, 0x0a )); // Deal 1 to 10 damage
		pChar.SoundEffect( 0x014c, true );
		pChar.DoAction( 0x14 );
	}
	if( rangeToChar == 0x00 && ValidateObject( flameItem ))
	{
		pChar.Damage( RandomNumber( 0x01, 0x1e )); // Deal 1 to 30 damage
		pChar.SoundEffect( 0x014c, true );
	}
	return true;
}