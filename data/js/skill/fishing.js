const mapWaterTiles = [ 0x00a8,0x00a9,0x00aa,0x00ab ];
const staticWaterTiles = [
	0x1797, 0x179c,
	0x346e, 0x3485,
	0x3490, 0x34ab,
	0x34b5, 0x34bb,
	0x34bd, 0x34c0,
	0x34c2, 0x34c5,
	0x34c7, 0x34ca,
	0x34d1, 0x34d5,
	0x55f0, 0x5607,
	0x5612, 0x562d,
	0x5637, 0x563d,
	0x563f, 0x5642,
	0x5644, 0x5647,
	0x5649, 0x564c,
	0x5653, 0x565e
];

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;

	if( socket && ValidateObject( iUsed ) && iUsed.isItem )
	{
		if( !DoItemChecks( socket, pUser, iUsed ))
			return false;

		// Check that fishing ability is not on cooldown
		if( socket.GetTimer( Timer.SOCK_FISHING ) > GetCurrentClock() )
		{
			socket.SysMessage( GetDictionaryEntry( 467, socket.language )); // You are already fishing.
			return false;
		}

		// Reveal user if they are hidden
		if( pUser.visible == 1 || pUser.visible == 2 )
		{
			pUser.visible = 0;
		}

		// Store some info for later
		pUser.SetTempTag( "fishingToolSerial", ( iUsed.serial ).toString() );
		iUsed.more = pUser.serial;

		socket.CustomTarget( 1, GetDictionaryEntry( 9310, socket.language )); // What water do you want to fish in?
	}
	return false;
}

function DoItemChecks( socket, pUser, iUsed )
{
	// Verify that fishing pole still exists
	if( !ValidateObject( iUsed ))
	{
		socket.SysMessage( GetDictionaryEntry( 9319, socket.language )); // Unable to detect item that initiated action. Does it still exist?
		return false;
	}

	// Check to see if item is locked down
	if( iUsed.movable == 2 || iUsed.movable == 3 )
	{
		socket.SysMessage( GetDictionaryEntry( 774, socket.language )); // That is locked down and you cannot use it
		return false;
	}

	// Check that item is in player's possession
	var itemOwner = GetPackOwner( iUsed, 0 );
	if( itemOwner == null || itemOwner.serial != pUser.serial )
	{
		pUser.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
		return false;
	}

	// Check if player is mounted
	if( pUser.isonhorse )
	{
		pUser.SysMessage( GetDictionaryEntry( 2713, socket.language )); // You cannot use this while riding a mount.
		return false;
	}
	return true;
}

function onCallback1( socket, ourObj )
{
	var mChar = socket.currentChar;
	if( !ValidateObject( mChar ) || !mChar.isChar )
		return;

	// Abort if player cancelled the targeting cursor
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	// Retrieve fishing tool that was used to initiate the fishing action
	var fishingTool = CalcItemFromSer( mChar.GetTempTag( "fishingToolSerial" ));

	// Let's repeat some of the same checks as before to make sure this process is still valid
	if( !DoItemChecks( socket, mChar, fishingTool ))
		return;

	// Reveal player if they're hidden
	if( mChar.visible == 1 || mChar.visible == 2 )
	{
		mChar.visible = 0;
	}

	// Fetch coordinates for where player targeted
	var worldNum = mChar.worldNumber;
	var targX = socket.GetWord( 11 );
	var targY = socket.GetWord( 13 );
	var targZ = socket.GetSByte( 16 );

	var shallowWaterFlag = false;
	var validWaterTile = false;
	var mapTileID = 0;
	var tileID = socket.GetWord( 17 );
	if( tileID == 0 && socket.GetByte( 1 ))
	{
		// Player targeted a map tile
		mapTileID = GetTileIDAtMapCoord( targX, targY, worldNum );
		validWaterTile = ( mapWaterTiles.indexOf( mapTileID ) > -1 );
	}
	else if( tileID != 0 && socket.GetByte( 1 ))
	{
		// Player targeted a static object! Check that it's a valid water tile:
		for( let i = 0; !validWaterTile && i < staticWaterTiles.length; i += 2 )
		{
			if( tileID >= staticWaterTiles[i] && tileID <= staticWaterTiles[i + 1] )
			{
				// Also confirm, server-side, that there's an actual water tile at the specified location. Don't trust the client.
				var validWaterTile = CheckStaticFlag( targX, targY, targZ, mChar.worldnumber, 7 ); // Is there a tile with TF_WET flag?
				if( validWaterTile )
				{
					shallowWaterFlag = true;
				}
			}
		}
	}
	else if( ValidateObject( ourObj ))
	{
		// Player targeted a dynamic object
		if( ourObj.isItem && ourObj.movable > 1 ) // Don't allow fishing in portable water, if any exists
		{
			tileID = ourObj.id;
			for( let i = 0; !validWaterTile && i < staticWaterTiles.length; i += 2 )
			{
				if( tileID >= staticWaterTiles[i] && tileID <= staticWaterTiles[i + 1] )
				{
					validWaterTile = true;
					shallowWaterFlag = true;
				}
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 846, socket.language )); // You can't fish there!
			CleanUp( socket, mChar );
			return;
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 846, socket.language )); // You can't fish there!
		CleanUp( socket, mChar );
		return;
	}

	if( !validWaterTile )
	{
		// No valid water tile was targeted, no fish to be found
		socket.SysMessage( GetDictionaryEntry( 9337, socket.language )); // You need water to fish in!
		CleanUp( socket, mChar );
		return;
	}

	// Verify that player is within distance of the targeted area
	var maxDistance = ( fishingTool.id == 0x0dca ? 6 : 5 );
	if( !CheckDistance( socket, mChar, maxDistance ))
	{
		CleanUp( socket, mChar );
		return;
	}

	// Do a line of sight check to the targeted water tile location
	if( !mChar.CanSee( targX, targY, targZ ))
	{
		socket.SysMessage( GetDictionaryEntry( 1646, socket.language )); // You cannot see that
		CleanUp( socket, mChar );
		return;
	}

	// In case player targeted a map tile, detect if there's non-water based tiles within 16 tiles to determine
	// if player is fishing in shallow or deep water
	if( !shallowWaterFlag && validWaterTile )
	{
		// A valid water tile was detected. Now, time to check if we're attempting to fish in shallow or deep water!
		var xOffset = 0;
		var yOffset = 0;
		var mapTileIDToCheck = 0;
		for( xOffset = targX - 15; !shallowWaterFlag && xOffset < targX + 15; ++xOffset )
		{
			for( yOffset = targY - 15; !shallowWaterFlag && yOffset < targY + 15; ++yOffset )
			{
				// Fetch ID of map tile at various places surrounding the target location
				mapTileIDToCheck = GetTileIDAtMapCoord( targX, targY, mChar.worldnumber );

				// If a non-water map tile is found, player is attempting to fish in shallow water
				shallowWaterFlag = ( mapWaterTiles.indexOf( mapTileIDToCheck ) == -1 );
			}
		}
	}

	if( shallowWaterFlag )
	{
		if( fishingTool.id == 0x0dca ) // special fishing net
		{
			socket.SysMessage( GetDictionaryEntry( 9317, socket.language )); // You can only use this net in deep water!
			return;
		}

		// Shallow water fishing
		// TODO: Skill checks easier in shallow water than deep water
		mChar.SetTempTag( "IsDeepSeaFishing", false );
		FishingWithPole( socket, mChar, fishingTool, targX, targY, targZ );
		return;
	}
	else
	{
		// Deep water fishing
		// TODO: Skill checks more difficult in deep water than shallow water
		mChar.SetTempTag( "IsDeepSeaFishing", true );
		if( fishingTool.id == 0x0dca ) // special fishing net
		{
			FishingWithNet( socket, mChar, fishingTool, targX, targY, targZ );
			return;
		}

		FishingWithPole( socket, mChar, fishingTool, targX, targY, targZ ); // Replace with deep water fishing when implemented
		return;
	}
}

function FishingWithNet( socket, mChar, fishingNet, targX, targY, targZ )
{
	fishingNet.movable = 2;
	fishingNet.visible = 3;
	fishingNet.container = null;

	// Turn player towards targeted location, and play the "cast out" animation
	mChar.TurnToward( targX, targY );
	mChar.DoAction( 0x00, 0x03 );

	DoMovingEffect( mChar.x, mChar.y, mChar.z + 5, targX, targY, targZ + 1, fishingNet.id, 0x01, 0x00, false );
	DoStaticEffect( targX, targY, targZ, 0x352D, 0x3, 0x10, false );
	fishingNet.SetLocation( targX, targY, targZ + 3 );
	fishingNet.visible = 0;
	mChar.SoundEffect( 0x364, true );

	socket.SysMessage( GetDictionaryEntry( 9318, socket.language )); // You plunge the net into the sea...

	fishingNet.morex = 1;
	fishingNet.StartTimer( 1000, 10, true );
}

function FishingWithPole( socket, mChar, fishingPole, targX, targY, targZ )
{
	// Check that fishing ability is not on cooldown
	if( socket.GetTimer( Timer.SOCK_FISHING ) > GetCurrentClock() )
	{
		socket.SysMessage( GetDictionaryEntry( 467, socket.language )); // You are already fishing.
		return false;
	}

	// Handle stamina loss from fishing based on UOX.INI setting
	var fishingStaminaLoss = GetServerSetting( "FishingStaminaLoss" );
	if( mChar.stamina - 2 < 2 )
	{
		mChar.SysMessage( GetDictionaryEntry( 845, socket.language )); // You are too tired to fish.
		return;
	}

	// Turn player towards targeted location, and play the "cast out" animation
	mChar.TurnToward( targX, targY );
	mChar.DoAction( 0x00, 0x03 );

	// Store the targeted location in socket variables
	socket.clickX = targX;
	socket.clickY = targY;
	socket.clickZ = targZ;

	// Set fishing as the current skill in use
	var baseFishingTimer = GetServerSetting( "BaseFishingTimer" );
	var randomFishingTimer = GetServerSetting( "RandomFishingTimer" );
	var fishingTimer = Math.max( 1.5, baseFishingTimer + RandomNumber( 0, randomFishingTimer )); // Minimum 1500 aka 1.5 seconds

	// Set timer for fishing skill, during which player cannot fish again
	socket.SetTimer( Timer.SOCK_FISHING, fishingTimer * 1000 );

	// Set timer for skill use, to disallow using other skills while fishing
	socket.SetTimer( Timer.SOCK_SKILLDELAY, fishingTimer * 1000 );

	// Start JS timer to handle various effects around fishing
	socket.tempInt2 = ( fishingTimer * 1000 ) - 1000;

	// mChar.StartTimer( 1000, 3, true );
	fishingPole.StartTimer( 1000, 3, true );
	mChar.stamina -= fishingStaminaLoss
}

function CheckDistance( socket, mChar, maxDistance )
{
	var targX = socket.GetWord( 11 );
	var targY = socket.GetWord( 13 );
	var targZ = socket.GetSByte( 16 );
	var distX = Math.abs( mChar.x - targX );
	var distY = Math.abs( mChar.y - targY );
	var distZ = Math.abs( mChar.z - targZ );

	if( mChar.z < targZ && ( targZ - mChar.z >= 5 )) // Allows player to stand on shorelines that are "below" the ocean!
	{
		socket.SysMessage( GetDictionaryEntry( 844, socket.language )); // You cannot fish above you!
		return false;
	}

	if( distX > maxDistance || distY > maxDistance || distZ > 30 )
	{
		socket.SysMessage( GetDictionaryEntry( 9313, socket.language )); // You need to be closer to the water to fish!
		return false;
	}
	else
	{
		return true;
	}
}

// Weighted randomizer for quick and easy distribution of fishing resources based on difficulty
function weightedRandom( min, max )
{
 	return Math.round( max / ( Math.random() * max + min ));
}

function onTimer( fishingTool, timerID )
{
	if( !ValidateObject( fishingTool ))
		return;

	var mChar = CalcCharFromSer( fishingTool.more );
	if( !ValidateObject( mChar ))
		return;

	var socket = null;
	socket = mChar.socket;
	if( socket == null )
		return;

	var fishingTimer = 0;
	fishingTimer = socket.tempInt2;

	// Reveal player if they are hidden
	if( mChar.visible == 1 || mChar.visible == 2 )
	{
		mChar.visible = 0;
	}

	var targX = socket.clickX;
	var targY = socket.clickY;
	var targZ = socket.clickZ;
	var maxDistance = ( fishingTool.id == 0x0dca ? 6 : 5 );

	switch( timerID )
	{
		case 0:
			if( socket )
			{
				socket.clickX = 0;
				socket.clickY = 0;
				socket.clickZ = 0;
				socket.tempInt2 = 0;

				if( !CheckDistance( socket, mChar, maxDistance ))
					return;

				// If player has a SOS (message in a bottle) at the top level of their inventory, and they fish near the location indicated by some sextant coordinates on the SOS,
				// they will try to retrieve sunken treasure. They can fish up bones, artifacts from the sunken ship or the treasure itself. If the treasure is fished up, the relevant SOS is deleted.
				if( mChar.skills.fishing >= 650 )
				{
					var mCharPack = mChar.pack;
					var mItem;
					for( mItem = mCharPack.FirstItem(); !mCharPack.FinishedItems(); mItem = mCharPack.NextItem() )
					{
						if( ValidateObject( mItem ))
						{
							if( mItem.id == 0x14ee && mItem.morex != 0 && mItem.morey != 0 && mItem.GetTag( "treasureLevel" ) != 0 ) // waterstained SOS
							{
								if( DistanceBetween( targX, targY, mItem.morex, mItem.morey ) <= 4 ) // Within 4 tiles of the shipwreck location, allow
								{
									ShipwreckFishing( mChar, mItem, targX, targY, targZ )
									return;
								}
							}
						}
					}
				}

				// Double check there's still enough resources left here! Other players might be fishing too
				var mResource = ResourceRegion( targX, targY, mChar.worldnumber );
				RegenerateFish( mResource, socket );
				if( mResource.fishAmount <= 0 )
				{
					socket.SysMessage( GetDictionaryEntry( 9311, socket.language )); // The fish don't seem to be biting here.
					return;
				}

				// NOTE: Should the skill check be moved to after system has determined what to fish up? That way, can run different skill checks
				// depending on the type of item being fished up, and prevent highly skilled fishermen from gaining more skill from fishing up boots
				var isDeepSeaFishing = mChar.GetTempTag( "IsDeepSeaFishing" );
				if(( isDeepSeaFishing && mChar.CheckSkill( 18, 0, 1000 )) || ( !isDeepSeaFishing && mChar.CheckSkill( 18, 0, 750 )))// Player cannot gain skill in shallow water if skill is over 75.0
				{
					// Do another effect on water to simulate fish being caught!
					mChar.TurnToward( targX, targY );

					// Play cast animation in reverse to catch fish (for non-gargoyle characters only)
					if( mChar.gender != 4 && mChar.gender != 5 )
					{
						mChar.DoAction( 0x0b, null, 7, 0, true );
					}

					// Play another splash effect with sound
					mChar.SoundEffect( 0x364, true );
					DoStaticEffect( targX, targY, targZ, 0x352D, 0x3, 0x10, false );

					var itemCaught = null;
					var fishType = "randomfish";

					var fishingSkill = mChar.skills.fishing;
					if( fishingSkill < 25 )
					{
						switch( weightedRandom( 1, 2 ))
						{
							case 1: // random fish
								fishType = "randomfish";
								break;
							case 2: // random footwear
								fishType = "randomfootwear";
								break;
						}
					}
					else if( fishingSkill >= 250 && fishingSkill < 800 )
					{
						// boots, regular fish, special fishing net
						switch( weightedRandom( 1, 4 ))
						{
							case 1: // random fish
							case 2:
								fishType = "randomfish";
								break;
							case 3: // random footwear
								fishType = "randomfootwear";
								break;
							case 4: // special fishing net
								fishType = "specialfishingnet";
								break;
						}
					}
					else if( fishingSkill >= 800 && fishingSkill < 900 )
					{
						// boots, regular fish, special fishing net, magic fish, big fish (deep water only),
						switch( weightedRandom( 1, 6 ))
						{
							case 1: // random fish
							case 2:
								fishType = "randomfish";
								break;
							case 3: // random footwear
								fishType = "randomfootwear";
								break;
							case 4: // special fishing net
								fishType = "specialfishingnet";
								break;
							case 5: // random magic fish
								fishType = "randommagicfish";
								break;
							case 6: // big fish
								if( isDeepSeaFishing )
								{
									fishType = "big_fish";
								}
								else
								{
									fishType = "randomfish";
								}
								break;
						}
					}
					else if( fishingSkill >= 900 && fishingSkill < 1000 )
					{
						// boots, regular fish, special fishing net, magic fish, big fish (deep water only), treasure map
						switch( weightedRandom( 1, 6 ))
						{
							case 1: // random fish
							case 2:
								fishType = "randomfish";
								break;
							case 3: // random footwear
								fishType = "randomfootwear";
								break;
							case 4: // special fishing net
								fishType = "specialfishingnet";
								break;
							case 5: // random magic fish
								fishType = "randommagicfish";
								break;
							case 6: // big fish
								if( isDeepSeaFishing )
								{
									fishType = "big_fish";
								}
								else
								{
									fishType = "randomfish";
								}
								break;
							/*case 6: // treasure map
								fishType = "treasuremap";

								// Spawn sea serpent with this item as part of its loot
								var nSpawned = SpawnNPC( npcToSpawn, targX, targY, targZ, mChar.worldnumber, mChar.instanceID );
								if( ValidateObject( nSpawned ))
								{
									var itemCaught = CreateDFNItem( null, nSpawned, fishType, 1, "ITEM", true );
								}
								return;*/
						}
					}
					else if( fishingSkill >= 1000 )
					{
						// boots, regular fish, special fishing net, magic fish, big fish (deep water only), treasure map, SOS (message in a bottle)
						switch( weightedRandom( 1, 8 ))
						{
							case 1: // random fish
							case 2:
								fishType = "randomfish";
								break;
							case 3: // random footwear
								fishType = "randomfootwear";
								break;
							case 4: // special fishing net
								fishType = "specialfishingnet";
								break;
							case 5: // random magic fish
								fishType = "randommagicfish";
								break;
							case 7:
							case 6: // big fish
								if( isDeepSeaFishing )
								{
									fishType = "big_fish";
								}
								else
								{
									fishType = "randomfish";
								}
								break;
							/*case 7: // treasure map
								fishType = "treasuremap";

								// Spawn sea serpent with this item as part of its loot
								var nSpawned = SpawnNPC( npcToSpawn, targX, targY, targZ, mChar.worldnumber, mChar.instanceID );
								if( ValidateObject( nSpawned ))
								{
									var itemCaught = CreateDFNItem( null, nSpawned, fishType, 1, "ITEM", true );
								}
								return;*/
							case 8: // message in a bottle
							{
								fishType = "messageinabottle";

								// Spawn sea serpent with this item as part of its loot
								var nSpawned = SpawnNPC( npcToSpawn, targX, targY, targZ, mChar.worldnumber, mChar.instanceID );
								if( ValidateObject( nSpawned ))
								{
									var itemCaught = CreateDFNItem( null, nSpawned, fishType, 1, "ITEM", true );
								}
								return;
							}
						}
					}

					if( fishType == "randomfootwear" )
					{
						// The higher the player's fishing skill, the less likely they should be to get
						// random footwear, but instead get regular fish
						if( mChar.skills.fishing > RandomNumber( 1, 100 ))
						{
							fishType == "randomfish";
						}
					}

					// Create an item and consume a fish resource
					mResource.fishAmount -= 1;
					var itemCaught = CreateDFNItem( mChar.socket, mChar, fishType, 1, "ITEM", true );
					switch( fishType )
					{
						case "bigfish":
						{
							let fishWeight = RandomNumber( 3, 200 ) * 100;
							itemCaught.weight = fishWeight;
							itemCaught.SetTag( "caughtBy", mChar.name );
							break;
						}
						default:
							break;
					}

					// Fishermen with skill above 80.0 have a chance to pull up sea serpents regardless of success or failure
					if( mChar.skills.fishing >= 800 )
					{
						var npcToSpawn = "seaserpent";
						if( isDeepSeaFishing )
						{
							npcToSpawn = "deepseaserpent";
						}

						let rndNum = RandomNumber( 1, 100 );
						if( rndNum <= 2 ) // 2% chance of fishing sea serpent on success
						{
							var catchText = GetDictionaryEntry( 9316, socket.language ); // You pull out an item along with a monster : %s
							socket.SysMessage( catchText.replace( /%s/gi, itemCaught.name ));

							// Spawn Sea Serpent or Deep Sea Serpent
							var nSpawned = SpawnNPC( npcToSpawn, targX, targY, targZ, mChar.worldnumber, mChar.instanceID );
						}
						else
						{
							if( fishType == "bigfish" )
							{
								socket.SysMessage( GetDictionaryEntry( 9336, socket.language )); // Your fishing pole bends as you pull a big fish from the depths!
							}
							else
							{
								var catchText = GetDictionaryEntry( 9315, socket.language ); // You pull out an item : %s
								socket.SysMessage( catchText.replace( /%s/gi, itemCaught.name ));
							}
						}
					}
					else
					{
						// Low skilled fisherman, in either shallow or deep water
						var catchText = GetDictionaryEntry( 9315, socket.language ); // You pull out an item : %s
						socket.SysMessage( catchText.replace( /%s/gi, itemCaught.name ));
					}

					// Play moving effect for item caught
					if( itemCaught != null )
					{
						DoMovingEffect( targX, targY, targZ, mChar.x, mChar.y, mChar.z + 5, itemCaught.id, 0x01, 0x00, false );
					}
					break;
				}
				else
				{
					let rndNum = RandomNumber( 1, 1000 );
					// 2.5% chance of fishing sea serpent from deep sea on failure, and 0.5% chance to fish one from shallow water on failure
					if( isDeepSeaFishing && rndNum <= 25 || !isDeepSeaFishing && rndNum <= 5 )
					{
						// Spawn Sea Serpent
						socket.SysMessage( GetDictionaryEntry( 9339, socket.language )); // You pull out a sea serpent!
						var serpentToSpawn = "seaserpent";
						if( isDeepSeaFishing )
						{
							// 5% chance that the serpent being fished up is of the deep sea variant
							serpentToSpawn = ( RandomNumber( 1, 100 ) <= 5 ) ? "deepseaserpent" : "seaserpent";
						}
						var nSpawned = SpawnNPC( serpentToSpawn, targX, targY, targZ, mChar.worldnumber, mChar.instanceID );
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 9314, socket.language )); // You fish a while, but fail to catch anything.
					}

					if( RandomNumber( 0, 1 ))	// 50% chance to destroy some resources
					{
						mResource.fishAmount = mResource.fishAmount - 1;
					}
				}
			}
			break;
		case 1:
			if( socket )
			{
				if( !CheckDistance( socket, mChar, maxDistance ))
				{
					mChar.skillsused.fishing = false;
					return;
				}
			}
			mChar.DoAction( 0x22 );
			fishingTool.StartTimer( fishingTimer, 0, true );
			break;
		case 2:
			mChar.DoAction( 0x22 );
			fishingTimer = Math.round(( fishingTimer / 2 ));
			socket.tempInt2 = fishingTimer;
			fishingTool.StartTimer( fishingTimer, 1, true );
			break;
		case 3:
			mChar.SoundEffect( 0x027, true );
			DoStaticEffect( targX, targY, targZ, 0x352D, 0x3, 0x10, false )
			fishingTimer = Math.round( fishingTimer / 3 );
			socket.tempInt2 = fishingTimer * 2;
			fishingTool.StartTimer( fishingTimer, 2, true );
			break;
		case 10: // Fishing net initial timer
			if( ValidateObject( fishingTool ))
			{
				if( fishingTool.morex == 14 )
				{
					socket.clickX = null;
					socket.clickY = null;
					socket.clickZ = null;
					if( fishingTool.GetTag( "fabledNet" ))
					{
						FabledDeepSeaFishingResult( socket, mChar, fishingTool );
					}
					else
					{
						DeepSeaFishingResult( socket, mChar, fishingTool );
					}
				}
				else if( fishingTool.morex < 14 )
				{
					if( fishingTool.morex % 2 == 0 )
					{
						fishingTool.z--;
					}

					for( let j = 0; j < 3; j++ )
					{
						let effectX = fishingTool.x + ( RandomNumber( -2, 2 ));
						let effectY = fishingTool.y + ( RandomNumber( -2, 2 ));
						DoStaticEffect( effectX, effectY, targZ, 0x352D, 0x3, 0x10, false );
					}
					if( RandomNumber( 0, 1 ))
					{
						fishingTool.SoundEffect( 0x364, true );
					}
					fishingTool.morex++;
					fishingTool.StartTimer( 1500, 10, true );
				}
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 9319, socket.language )); // Unable to detect item that initiated action. Does it still exist?
			}
			break;
	}
}

function ShipwreckFishing( mChar, mItem, targX, targY, targZ )
{
	// Do another effect on water to simulate fish being caught!
	mChar.TurnToward( targX, targY );

	// Play cast animation in reverse to catch fish (for non-gargoyle characters only)
	if( mChar.gender != 4 && mChar.gender != 5 )
	{
		mChar.DoAction( 0x0b, null, 7, 0, true );
	}

	// Play another splash effect with sound
	mChar.SoundEffect( 0x364, true );
	DoStaticEffect( targX, targY, targZ, 0x352D, 0x3, 0x10, false );

	var itemCaught = null;

	// Can't let the player fish up the treasure on the first attempt... we should set a minimum amount
	// of tries before there's a chance to fish up the chest, then increase the chance with each "failure"
	// until a max cap on amount of tries is reached, where fishing up the treasure is guaranteed
	var chanceToFishTreasure = 0;
	var fishingTries = mItem.GetTag( "fishingTries" );
	if( fishingTries >= 20 )
	{
		chanceToFishTreasure = 100;
	}
	else if( fishingTries >= 5 )
	{
		chanceToFishTreasure = 20 + ( 4 * fishingTries );
	}

	var shipwreckItemType;
	var treasureFound = false;
	var rndNum = RandomNumber( 1, 100 );
	if( chanceToFishTreasure >= rndNum )
	{
		// Treasure!
		var treasureLevel = mItem.GetTag( "treasureLevel" );
		if( treasureLevel >= 1 && treasureLevel <= 4 )
		{
			switch( treasureLevel )
			{
				case 1: // Level 1 treasure
					shipwreckItemType = "shipwreck_treasure_1";
					break;
				case 2: // Level 2 treasure
					shipwreckItemType = "shipwreck_treasure_2";
					break;
				case 3: // Level 3 treasure
					shipwreckItemType = "shipwreck_treasure_3";
					break;
				case 4: // Level 4 treasure
					shipwreckItemType = "shipwreck_treasure_4";
					break;
			}
		}

		treasureFound = true;
	}
	else
	{
		// Let's fish up some treasures
		switch( weightedRandom( 1, 6 ))
		{
			case 1: // Bones
				shipwreckItemType = "randombones";
				break;
			case 2: // Hats
				shipwreckItemType = "randomhats";
				break;
			case 3: // Pillows
				shipwreckItemType = "randompillows";
				break;
			case 4: // Shells
				shipwreckItemType = "randomshells";
				break;
			case 5: // Misc (Barrel staves, unfinished barrel, stool, candelabra, broken clock, globe)
				shipwreckItemType = "randommisc";
				break;
			case 6: // Paintings
				shipwreckItemType = "randompaintings";
				break;
		}
	}

	// Create the shipwreck item
	var itemCaught = CreateDFNItem( mChar.socket, mChar, shipwreckItemType, 1, "ITEM", true );
	if( ValidateObject( itemCaught ))
	{
		if( treasureFound )
		{
			mchar.socket.SysMessage( GetDictionaryEntry( 9335, mChar.socket.language )); // You pull up a heavy chest from the depths of the ocean!

			// Treasure found - remove the SOS message!
			mItem.Delete();
		}
		else
		{
			var catchText = GetDictionaryEntry( 9315, mChar.socket.language ); // You pull out an item : %s
			mChar.socket.SysMessage( catchText.replace( /%s/gi, itemCaught.name ));

			mItem.SetTag( "fishingTries", fishingTries + 1 );
		}
	}
}

function DeepSeaFishingResult( socket, mChar, fishingNet )
{
	var targX = fishingNet.x;
	var targY = fishingNet.y;

	var spawnCount = fishingNet.morey + RandomNumber( Math.round( fishingNet.morez / 2 ), Math.round( fishingNet.morez ));
	for( var i = 0; i < spawnCount; i++ )
	{
		// Find a valid spawn location
		for( let j = 0; j < 20; j++ )
		{
			let spawnX = targX + RandomNumber( -4, 4 );
			let spawnY = targY + RandomNumber( -4, 4 );
			let spawnZ = GetMapElevation( spawnX, spawnY, mChar.worldnumber );

			let mapTileID = GetTileIDAtMapCoord( spawnX, spawnY, mChar.worldnumber );
			if( mapWaterTiles.indexOf( mapTileID ) == -1 )
				continue;

			if( FindMulti( spawnX, spawnY, spawnZ, mChar.worldnumber, mChar.instanceID ))
				continue;

			let rndNum = weightedRandom( 1, 4 );
			switch( rndNum )
			{
				case 1: // Sea Serpent
					var nSpawned = SpawnNPC( "seaserpent", spawnX, spawnY, spawnZ, mChar.worldnumber, mChar.instanceID );
					break;
				case 2: // Water Elemental
					var nSpawned = SpawnNPC( "waterele", spawnX, spawnY, spawnZ, mChar.worldnumber, mChar.instanceID );
					break;
				case 3: // Deep Sea Serpent
					var nSpawned = SpawnNPC( "deepseaserpent", spawnX, spawnY, spawnZ, mChar.worldnumber, mChar.instanceID );
					break;
				case 4: // Kraken
				{
					var nSpawned = SpawnNPC( "kraken", spawnX, spawnY, spawnZ, mChar.worldnumber, mChar.instanceID );
					if( ValidateObject( nSpawned ))
					{
						// Chance of spawning a SOS message in a bottle in kraken's inventory
						var newLoot;
						switch( RandomNumber( 1, 10 ))
						{
							case 1: // message in a bottle, 10% chance to spawn as loot
								newLoot = CreateDFNItem( socket, mChar, "messageinabottle", 1, "ITEM", true );
								break;
							default:
								break;
						}

					}
					break;
				}
				default:
					break;
			}

			if( ValidateObject( nSpawned ) && ValidateObject( mChar ) && !mChar.dead && mChar.online  )
			{
				nSpawned.target = mChar;
			}
			break;
		}
	}
	fishingNet.Delete();
}

function FabledDeepSeaFishingResult( socket, mChar, fishingNet )
{
	var targX = fishingNet.x;
	var targY = fishingNet.y;

	var spawnCount = fishingNet.morey + RandomNumber( Math.round( fishingNet.morez / 2 ), Math.round( fishingNet.morez ));
	for( var i = 0; i < spawnCount; i++ )
	{
		// Find a valid spawn location
		for( let j = 0; j < 20; j++ )
		{
			let spawnX = targX + RandomNumber( -4, 4 );
			let spawnY = targY + RandomNumber( -4, 4 );
			let spawnZ = GetMapElevation( spawnX, spawnY, mChar.worldnumber );

			let mapTileID = GetTileIDAtMapCoord( spawnX, spawnY, mChar.worldnumber );
			if( mapWaterTiles.indexOf( mapTileID ) == -1 )
				continue;

			if( FindMulti( spawnX, spawnY, spawnZ, mChar.worldnumber, mChar.instanceID ))
				continue;

			let rndNum = weightedRandom( 1, 4 );
			switch( rndNum )
			{
				case 1: // Sea Serpent
					var nSpawned = SpawnNPC( "seaserpent", spawnX, spawnY, spawnZ, mChar.worldnumber, mChar.instanceID );
					break;
				case 2: // Water Elemental
					var nSpawned = SpawnNPC( "waterele", spawnX, spawnY, spawnZ, mChar.worldnumber, mChar.instanceID );
					break;
				case 3: // Deep Sea Serpent
					var nSpawned = SpawnNPC( "deepseaserpent", spawnX, spawnY, spawnZ, mChar.worldnumber, mChar.instanceID );
					break;
				case 4: // Kraken
				{
					var specialNpcSpawned = false;
					let rndNum2 = weightedRandom( 1, ( specialNpcSpawned ? 1 : 3 ));
					switch( rndNum2 )
					{
						case 1: // Kraken
						{
							var nSpawned = SpawnNPC( "kraken", spawnX, spawnY, spawnZ, mChar.worldnumber, mChar.instanceID );
							if( ValidateObject( nSpawned ))
							{
								// Chance of spawning a SOS message in a bottle in kraken's inventory
								var newLoot;
								switch( weightedRandom( 1, 2 ))
								{
									case 1: // message in a bottle
										newLoot = CreateDFNItem( socket, mChar, "messageinabottle", 1, "ITEM", true );
										break;
									case 2: // rope
										newLoot = CreateDFNItem( socket, mChar, "0x14f8", 1, "ITEM", true );
										break;
								}

							}
							break;
						}
						/*case 2: // Leviathan
							var nSpawned = SpawnNPC( "leviathan", spawnX, spawnY, spawnZ, mChar.worldnumber, mChar.instanceID );
							specialNpcSpawned = true;
							break;
						case 3: // Osiredon the Scalis Enforcer
							var nSpawned = SpawnNPC( "scalisenforcer", spawnX, spawnY, spawnZ, mChar.worldnumber, mChar.instanceID );
							specialNpcSpawned true;
							break;*/
					}
					break;
				}
				default:
					break;
			}

			if( ValidateObject( nSpawned ) && ValidateObject( mChar ) && !mChar.dead && mChar.online )
			{
				nSpawned.target = mChar;
			}
			break;
		}
	}
	fishingNet.Delete();
}

function RegenerateFish( mResource, socket )
{
	var fishCeiling = ResourceAmount( "FISH" );
	var fishTimer 	= ResourceTime( "FISH" );
	var currentTime = GetCurrentClock();

	if( mResource.fishTime <= currentTime )
	{
		for( var i = 0; i < fishCeiling; ++i )
		{
			if((( mResource.fishTime + ( i * fishTimer * 1000 )) / 1000 ) <= ( currentTime / 1000 ) && mResource.fishAmount < fishCeiling )
			{
				mResource.fishAmount = mResource.fishAmount + 1;
			}
			else
				break;
		}
		mResource.fishTime = ( currentTime + ( 1000 * parseInt( fishTimer ))) / 1000;
	}
}

function CleanUp( socket, mChar )
{
	socket.tempInt2 = 0;
	socket.clickX = 0;
	socket.clickY = 0;
	socket.clickZ = 0;
	mChar.SetTempTag( "IsDeepSeaFishing", null );
}