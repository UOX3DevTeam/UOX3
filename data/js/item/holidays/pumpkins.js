// Other settings
const randomizePumpkinCountdown = false; // If true, add/remove +1/-1 seconds to explosion potion countdowns

function onCreateDFN( objMade, objType ) 
{
	var idList = [0x0C6B, 0x0C6A, 0x0C6C];

	if( objType == 0 )
	{
		var randomID = Math.floor( Math.random() * 3 );

		var pumpkinname = getRandomPumpkinName( objMade );

		objMade.name = pumpkinname;
		objMade.id = idList[randomID];

		if( objMade.id == 0x0C6A || objMade.id == 0x0C6B )
		{
			objMade.weight = Math.floor( Math.random() * ( 2500 - 1200 + 1 ) + 1200 );
		} 
		else if( objMade.id == 0x0C6C )
		{
			objMade.weight = objMade.id == 0x0C6C ? 300 : 600;
		}
	}
}

function getRandomPumpkinName( objMade ) 
{
	var pumpkinname = "";

	if( objMade.GetTag( "jackolantern" ) == 1 )
	{
		pumpkinname = "Jack O' Lantern";
	} 
	else if( objMade.GetTag( "pumpkin" ) == 1 )
	{
		pumpkinname = "pumpkin";
	}

	return pumpkinname;
}

// 2006 Pumpkins
// Missing: The Shadow Colored Jack O’ Lanterns, both big and small, are also useable. When used and targeted anywhere, they explode in a large 5×5 tile field of flames.
// Missing: The Shadow Colored Jack O’ Lanterns, both big and small, are also useable.When used and targeted anywhere, they explode in a large 5×5 tile field of flames.
// Using explosion potion effect.
// Missing: colors as well for the pumpkins

function onUseChecked( pUser, iUsed ) 
{
	var socket = pUser.socket;
	var itemOwner = GetPackOwner( iUsed, 0 );
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( itemOwner == null || itemOwner != pUser )
	{
		pUser.SysMessage( GetDictionaryEntry( 1763, pUser.socket.language )); // That item must be in your backpack before it can be used.
	}

	if( iUsed.GetTag("jackolantern") == 1 )
	{
		// Store potion on socket and player serial on potion, for later!
		socket.tempObj = iUsed;
		iUsed.more = pUser.serial;
		// Set radius of explosion
		iUsed.morex = pUser.skills.alchemy / 250;

		// Randomize countdown length, if enabled
		if( randomizePumpkinCountdown )
		{
			iUsed.speed = RandomNumber( iUsed.speed - 1, iUsed.speed + 1 );
		}
		// Item's speed forms the basis of the countdownTime
		var countdownTime = iUsed.speed * 1000;

		// Start the initial timer that shows the first number over the character/object's head
		iUsed.StartTimer( 200, 1, true );

		// Start timers with IDs from 2, and count until we reach item's speed + 1
		var iCount = 2;
		for( iCount = 2; iCount < ( iUsed.speed + 2 ); iCount++ )
		{
			iUsed.StartTimer(( iCount - 1) * 1000, iCount, true );
		}

		socket.CustomTarget( 0, GetDictionaryEntry( 1348, socket.language )); //Now would be a good time to throw it!
	}
	return false;
}

function onCallback0( socket, ourObj )
{
	var mChar = socket.currentChar;
	var iUsed = socket.tempObj;
	if( mChar && mChar.isChar && iUsed && iUsed.isItem )
	{
		var StrangeByte = socket.GetWord( 1 );
		if( StrangeByte == 0 && ourObj )
		{
			// We need a LineOfSight check
			if( mChar.CanSee( ourObj.x, ourObj.y, ourObj.z ))
			{
				iUsed.container = null;
				iUsed.Teleport( ourObj );
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 1646, socket.language )); // You cannot see that

				// Give player another chance to throw the potion before it blows up in their face
				socket.CustomTarget( 0, GetDictionaryEntry( 1348, socket.language )); //Now would be a good time to throw it!
				return;
			}
		}
		else
		{
			var x = socket.GetWord( 11 );
			var y = socket.GetWord( 13 );
			var z = socket.GetSByte( 16 );

			// If connected with a client lower than v7.0.9, manually add height of targeted tile
			if( socket.clientMajorVer <= 7 && socket.clientSubVer < 9 )
			{
				z += GetTileHeight( socket.GetWord( 17 ));
			}

			// We need a LineOfSight check
			if( mChar.CanSee( x, y, z ))
			{
				iUsed.container = null;
				iUsed.Teleport(x, y, z);
			}
			else 
			{
				socket.SysMessage( GetDictionaryEntry( 1646, socket.language )); // You cannot see that

				// Give player another chance to throw the pumpkin before it blows up in their face
				socket.CustomTarget( 0, GetDictionaryEntry( 1348, socket.language )); //Now would be a good time to throw it!
				return;
			}
		}

		iUsed.movable = 2;

		// Play moving effect of potion being thrown to potion's target location
		DoMovingEffect( mChar, iUsed, iUsed.id, 5, 0, false, 0, 0 );
	}
}

function onTimer( timerObj, timerID ) 
{
	var countdown = 0;
	countdown = ( timerObj.speed + 1 ) - timerID;
	var packOwner = GetPackOwner( timerObj, 0 );

	// If timerID equals object speed + 1, time to explode!
	if( timerID == timerObj.speed + 1 )
	{
		if( timerObj.container == null )
		{
			DoStaticEffect(timerObj.x, timerObj.y, timerObj.z, 0x36B0, 0x09, 0x0d, false );
			timerObj.SoundEffect(0x0207, true);

			var explosionCounter = AreaCharacterFunction( "ApplyExplosionDamage", timerObj, 4 );
		}
		else
		{
			// Oops! Player is still holding the pumpkin... explode player for damage!
			if( packOwner != null && packOwner.isChar )
			{
				packOwner.StaticEffect( 0x36b0, 0x09, 0x0d );
				packOwner.SoundEffect( 0x0207, true );

				// Deal damage to player holding the pumpkin
				packOwner.Damage( RandomNumber( timerObj.lodamage, timerObj.hidamage ), 5 );
			}
		}
		timerObj.Delete();
		return;
	}

	// As long as timerID is lower than object speed, display a countdown message
	if( timerID <= timerObj.speed ) 
	{
		if( timerObj.container == null )
		{
			// Player threw pumpkin, let's show countdown above pumpkin - to everyone nearby
			timerObj.TextMessage( countdown.toString(), true, 0x0026, -1, -1, -1, 6 );
		}
		else
		{
			// Player has not thrown pumpkin yet - show timer above character's head, but only to player
			if( packOwner != null && packOwner.isChar )
			{
				packOwner.TextMessage(countdown.toString(), false, 0x0026, -1, -1, -1, 6);
			}
		}
	}
}

function ApplyExplosionDamage( timerObj, targetChar )
{
	var sourceChar = CalcCharFromSer( timerObj.more );
	if( ValidateObject( sourceChar ))
	{
		// Don't damage offline players
		if( !targetChar.npc && !targetChar.online )
			return;

		// Don't damage Young players
		if( GetServerSetting( "YoungPlayerStatus" ))
		{
			// Don't damage a Young player, or a Young player's pets
			if(( !targetChar.npc && targetChar.account.isYoung )
				|| ( targetChar.npc && ValidateObject( targetChar.owner ) && !targetChar.owner.npc && targetChar.owner.account.isYoung ))
			{
				return;
			}

			// Don't let Young players damage other players, or the pets of other players
			if(( !sourceChar.npc && sourceChar.account.isYoung && !targetChar.npc )
				|| ( targetChar.npc && ValidateObject(targetChar.owner) && !targetChar.owner.npc ))
			{
				return;
			}
		}

		// Check for Facet Ruleset
		if( !TriggerEvent( 2507, "FacetRuleExplosionDamage", sourceChar, targetChar ))
		{
			return;
		}

		// Ignore targets that are in safe zones
		var targetRegion = targetChar.region;
		if( targetRegion.isSafeZone )
		{
			sourceChar.SysMessage(GetDictionaryEntry( 2756, sourceChar.socket.language )); // Your target is in a safe zone!
			return;
		}

		// Don't allow a Z difference greater than 5
		if( Math.abs( targetChar.z - timerObj.z ) > 5 )
			return;

		// Ignore characters that are not in Line of Sight of the potion
		if( !targetChar.CanSee( timerObj ))
			return;

		// Deal damage, and do criminal check for source character!
		targetChar.Damage( RandomNumber( timerObj.lodamage, timerObj.hidamage ), 5, sourceChar, true );

		// If target is an NPC, make them attack the person who threw the potion!
		if( targetChar.npc && targetChar.target == null && targetChar.atWar == false )
		{
			targetChar.target = sourceChar;
			targetChar.atWar = true;
			targetChar.attacker = sourceChar;
		}
	}
	else
	{
		// Source character not found - apply damage on general basis
		targetChar.Damage( RandomNumber( timerObj.lodamage, timerObj.hidamage ), 5 );
	}
}

function onPickup( iPickedUp, pGrabber, containerObj )
{
	var pSock = pGrabber.socket;
	var idList = [0x4de, 0x4df];
	var randomID = Math.floor( Math.random() * 2 );

	switch( pSock.pickupSpot )
	{
		case 0: //nowhere
			return true;
			break;
		case 1: //ground
			if( iPickedUp.GetTag( "jackolantern" ) == 1 || iPickedUp.GetTag( "pumpkin" ) == 1 )
			{
				// Define a probability value (between 0 and 1) for the chance of spawning.
				var spawnProbability = 0.5; // This represents a 50% chance.

				// Generate a random number between 0 and 1.
				var randomChance = Math.random();

				// Check if the randomChance is less than or equal to the spawnProbability.
				if( randomChance <= spawnProbability && iPickedUp.GetTag( "Named" ) == 0 )
				{
					var nSpawned = SpawnNPC( "killerpumpkin", iPickedUp.x, iPickedUp.y, iPickedUp.z, iPickedUp.worldnumber, iPickedUp.instanceID );
					nSpawned.id = idList[randomID];
					iPickedUp.Delete();
				}
				else if( iPickedUp.GetTag( "Named" ) == 0 )
				{
					iPickedUp.name = pGrabber.name + " Pumpkin";
					iPickedUp.SetTag( "Named", 1 );
				}
			}
			return true;
			break;
		case 2: //ownpack
			return true;
			break;
		case 3: //otherpack
			return true;
			break;
		case 4: //paperdoll
			return true;
			break;
		case 5: //bank
			return true;
			break;
		default:
			return true;
			break;
	}
}