// This script handles all functionality related to mining, grave digging

// The MineCheck setting determines where players are able to mine
// By default, only two options have any effect:
// 	0 - Mine everywhere. Like... literally EVERYWHERE
// 	1 - Mine only from mountain tiles, dungeon/cave floors, big rocks, etc.
const mineCheck = GetServerSetting( "MineCheck" );

// Also fetch server settings regarding tool use limits & tools breaking
const toolUseLimit = GetServerSetting( "ToolUseLimit" );
const toolUseBreak = GetServerSetting( "ToolUseBreak" );

// Valid item target IDs for mining
const validCaveFloorIDs = [ 0x053B, 0x053C, 0x053D, 0x053E, 0x053F, 0x0540, 0x0541, 0x0542, 0x0543,
	0x0544, 0x0545, 0x0546, 0x0547, 0x0548, 0x0549, 0x054A, 0x054B, 0x054C, 0x054D, 0x054E, 0x054F,
	0x0551, 0x0552, 0x0553, 0x056A
];
const validBoulderIDs = [ 0x134F, 0x1362 ];

// Valid item target IDs for grave digging
const validGraveIDs = [ 0x0ED3, 0x0EDF, 0x0EE0, 0x0EE1, 0x0EE8 ];

// Valid map target IDs for mining
const validMapMountainIDs = [
	0x00E4, 0x00E5, 0x00E6, 0x00E7,
	0x00F4, 0x00F5, 0x00F6, 0x00F7,
	0x0104, 0x0105, 0x0106, 0x0107,
	0x0110, 0x0111, 0x0112, 0x0113,
	0x0122, 0x0123, 0x0124, 0x0125,
	0x01D3, 0x01D4, 0x01D5, 0x01D6, 0x01D7, 0x01D8, 0x01D9, 0x01DA,
	0x021F, 0x0220, 0x0221, 0x0222, 0x0223, 0x0224, 0x0225, 0x0226, 0x0227, 0x0228, 0x0229, 0x022A, 0x022B, 0x022C, 0x022D, 0x022E, 0x022F, 0x0230,
	0x0235, 0x0236, 0x0237, 0x0238,
	0x06CD, 0x06CE, 0x06CF, 0x06D0, 0x06D1,
	0x06DA, 0x06DB, 0x06DC, 0x06DD,
	0x06EB, 0x06EC, 0x06ED, 0x06EE, 0x06EF, 0x06F0, 0x06F1, 0x06F2,
	0x06FB, 0x06FC, 0x06FD, 0x06FE,
	0x070D, 0x070E, 0x070F, 0x0710, 0x0711, 0x0712, 0x0713, 0x0714,
	0x071D, 0x071E, 0x071F, 0x0720,
	0x072B, 0x072C, 0x072D, 0x072E, 0x072F, 0x0730, 0x0731, 0x0732,
	0x073B, 0x073C, 0x073D, 0x073E,
	0x0749, 0x074A, 0x074B, 0x074C, 0x074D, 0x074E, 0x074F, 0x0750,
	0x0759, 0x075A, 0x075B, 0x075C
];
const validMapCaveFloorIDs = [
	0x0245, 0x0246, 0x0247, 0x0248, 0x0249, 0x024A, 0x024B, 0x024C, 0x024D, 0x024E, 0x024F,
	0x0250, 0x0251, 0x0252, 0x0253, 0x0254, 0x0255, 0x0256, 0x0257, 0x0258, 0x0259, 0x025A,
	0x025B, 0x025C, 0x025D, 0x025E, 0x025F, 0x0260, 0x0261, 0x0262, 0x0263, 0x0264, 0x0265,
	0x0266, 0x0267, 0x0268, 0x0269, 0x026A, 0x026B, 0x026C, 0x026D, 0x02BC, 0x02BD, 0x02BE,
	0x02BF, 0x02C0, 0x02C1, 0x02C2, 0x02C3, 0x02C4, 0x02C5, 0x02C6, 0x02C7, 0x02C8, 0x02C9,
	0x02CA, 0x02CB,
	0x063B, 0x063C, 0x063D, 0x063E,
	0x07EC, 0x07ED, 0x07EE, 0x07EF, 0x07F0, 0x07F1, 0x07F2, 0x07F3, 0x07F4,
	0x07F5, 0x07F6, 0x07F7, 0x07F8, 0x07F9, 0x07FA, 0x07FB, 0x07FC, 0x07FD,
	0x07FE, 0x07FF, 0x0800, 0x0801, 0x0802, 0x0803, 0x0804, 0x0805, 0x0806,
	0x0807, 0x0808, 0x0809, 0x080A, 0x080B, 0x080C, 0x080D, 0x080E, 0x080F,
	0x0810, 0x0811, 0x0812, 0x0813, 0x0814, 0x0815, 0x0816, 0x0817, 0x0818,
	0x0819, 0x081A, 0x081B, 0x081C, 0x081D, 0x081E, 0x081F, 0x0820, 0x0821,
	0x0822, 0x0823, 0x0824, 0x0825, 0x0826, 0x0827, 0x0828, 0x0829, 0x082A,
	0x082B, 0x082C, 0x082D, 0x082E, 0x082F, 0x0830, 0x0831, 0x0832, 0x0833,
	0x0834, 0x0835, 0x0836, 0x0837, 0x0838, 0x0839,
	0x2F13, 0x2F14, 0x2F15, 0x2F16, 0x2F17, 0x2F18, 0x2F19, 0x2F1A,
	0x2F62, 0x2F63, 0x2F64, 0x2F65, 0x2F66, 0x2F67, 0x2F68, 0x2F69,
	0x2F6A, 0x2F6B, 0x2F6C, 0x2F6D, 0x2F6E, 0x2F6F, 0x2F70, 0x2F71,
	0x2F72, 0x2F73, 0x2F74, 0x2F75, 0x2F76, 0x2F77, 0x2F78, 0x2F79,
	0x2F7A, 0x2F7B, 0x2F7C, 0x2F7D, 0x2F7E, 0x2F7F, 0x2F80, 0x2F81,
	0x2F82, 0x2F83, 0x2F84, 0x2F85, 0x2F86, 0x2F87, 0x2F88, 0x2F89,
	0x2F8A, 0x2F8B, 0x2F8C, 0x2F8D, 0x2F8E, 0x2F8F, 0x2F90, 0x2F91,
	0x2F92, 0x2F93, 0x2F94, 0x2F95, 0x2F96, 0x2F97, 0x2F98, 0x2F99,
	0x2F9A, 0x2F9B, 0x2F9C, 0x2F9D, 0x2F9E, 0x2F9F, 0x2FA0, 0x2FA1,
	0x2FA2, 0x2FA3, 0x2FA4, 0x2FA5, 0x2FA6, 0x2FA7, 0x2FA8, 0x2FA9,
	0x2FAA, 0x2FAB, 0x2FAC, 0x2FAD, 0x2FAE, 0x2FAF, 0x2FB0, 0x2FB1,
	0x2FB2, 0x2FB3, 0x2FB4, 0x2FB5,
];

const validMapSandTilesIDs = [
	0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
	0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33,
	0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,
	0x3E, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B,
	0x11E, 0x11F, 0x120, 0x121, 0x122, 0x123, 0x124, 0x125, 0x126, 0x127,
	0x128, 0x129, 0x12A, 0x12B, 0x12C, 0x12D, 0x192, 0x1A8, 0x1A9, 0x1AA,
	0x1AB, 0x1B9, 0x1BA, 0x1BB, 0x1BC, 0x1BD, 0x1BE, 0x1BF, 0x1C0, 0x1C1,
	0x1C2, 0x1C3, 0x1C4, 0x1C5, 0x1C6, 0x1C7, 0x1C8, 0x1C9, 0x1CA, 0x1CB,
	0x1CC, 0x1CD, 0x281, 0x282, 0x283, 0x284, 0x289, 0x28A, 0x28B, 0x28C,
	0x28D, 0x28E, 0x28F, 0x290, 0x359, 0x35A, 0x35B, 0x35C, 0x35D, 0x35E,
	0x35F, 0x35E, 0x349, 0x34A, 0x34B, 0x34C, 0x355, 0x356, 0x357, 0x358,
	0x359, 0x35A, 0x35B, 0x35C, 0x359, 0x35A, 0x35B, 0x35C,
	0x5A7, 0x5A8, 0x5A9, 0x5AA, 0x5AB, 0x5AC, 0x5AD, 0x5AE, 0x5AF,
	0x5B0, 0x5B1, 0x5B2, 0x653, 0x654, 0x655, 0x656, 0x657, 0x658,
	0x659, 0x65A, 0x657, 0x658, 0x659, 0x65A, 0x667, 0x668, 0x669, 0x66A
];

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		// Verify that the mining tool is equipped on player or in their backpack
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pUser.SysMessage( GetDictionaryEntry( 6019, socket.language )); // This must be in your backpack or equipped before it can be used.
		}
		else if( iUsed.type != 15 )
		{
			// Store a reference to the mining tool in a custom property on the itemOwner object
			itemOwner.miningTool = iUsed;

			var targMsg = GetDictionaryEntry( 446, socket.language ); // Where do you want to dig?
			socket.CustomTarget( 1, targMsg );
			return false;
		}
	}
	return true;
}

function onCallback1( socket, ourObj )
{
	if( socket == null )
		return;

	var mChar = socket.currentChar;
	if( ValidateObject( mChar ) && mChar.isChar )
	{
		if( mChar.skillsused.mining )
		{
			socket.SysMessage( GetDictionaryEntry( 1971, socket.language )); // You are too busy to do that.
			return;
		}

		// Add a 1 second delay before player can use another skill
		socket.SetTimer( Timer.SOCK_SKILLDELAY, GetServerSetting( "SkillDelay" ) * 1000 );

		// Verify player has access to mining tool still
		if( !CheckForMiningTool( socket, mChar ))
			return;

		let treasureMapIDs = ["treasuremaplvl0", "treasuremaplvl1", "treasuremaplvl2", "treasuremaplvl3", "treasuremaplvl4", "treasuremaplvl5", "treasuremaplvl6" ];

		if( ValidateObject( ourObj)  && ourObj.isItem && treasureMapIDs.indexOf( ourObj.sectionID ) != -1 ) 
		{
			socket.tempObj2 = ourObj;
			TriggerEvent( 5400, "TreasureDigging", mChar );
			return;
		}

		var targX = socket.GetWord( 11 );
		var targY = socket.GetWord( 13 );
		var targZ = socket.GetSByte( 16 );
		var distX = Math.abs( mChar.x - targX );
		var distY = Math.abs( mChar.y - targY );
		var distZ = Math.abs( mChar.z - targZ );

		if( distX > 5 || distY > 5 || distZ > 35 )
		{
			socket.SysMessage( GetDictionaryEntry( 799, socket.language )); // You are too far away to mine that!
			return;
		}

		mChar.miningTargX = targX;
		mChar.miningTargY = targY;
		mChar.miningTargZ = targZ;

		// Check to see what player targeted
		if( mineCheck == 0 )
		{
			// Mine everywhere! Don't check.
			Mining( socket, mChar, targX, targY );
		}
		else
		{
			// Mine dungeon floors, cave floors, mountain, rocks
			var tileID = 0;
			var staticTile = true;
			if( socket.GetByte( 1 ))
			{
				tileID = socket.GetWord( 17 );
				if( !tileID )
				{
					tileID = GetTileIDAtMapCoord( socket.GetWord( 11 ), socket.GetWord( 13 ), mChar.worldnumber );
					staticTile = false;
				}
			}
			else if( ValidateObject( ourObj ) && ourObj.isItem )
			{
				// Make sure targeted object is in same world & instance as player
				if( ourObj.worldnumber != mChar.worldnumber || ourObj.instanceID != mChar.instanceID )
					return;

				tileID = ourObj.id;
			}

			var validTileIDFound = false;
			if( tileID != 0 )
			{
				// Replace the hard checks for IDs here with either arrays of IDs at top of script, or
				// IDs combined with names
				if( staticTile == true || ( ourObj && ourObj.isItem )) // tileID is from a dynamic or static item
				{
					if( validCaveFloorIDs.indexOf( tileID ) != -1 || validBoulderIDs.indexOf( tileID ) != -1 ) // Cave Floors or Boulders
					{
						// Mining
						validTileIDFound = true;
						Mining( socket, mChar, targX, targY );
					}
					else if( validGraveIDs.indexOf( tileID ) != -1 ) // Grave digging
					{
						// GraveDigging
						validTileIDFound = true;
						GraveDigging( socket, mChar, targX, targY );
					}
				}
				else if( staticTile == false ) // tileID is from a map tile
				{
					if( validMapMountainIDs.indexOf( tileID ) != -1 || validMapCaveFloorIDs.indexOf( tileID ) != -1 ) // Mountains or Cave Floors
					{
						validTileIDFound = true;
						Mining( socket, mChar, targX, targY );
					}
					else if( validMapSandTilesIDs.indexOf( tileID ) != -1 && mChar.GetTag( "GatheringSand" ) == 1 )
					{
						validTileIDFound = true;
						SandMining( socket, mChar, targX, targY );
					}
				}
			}

			if( !validTileIDFound )
			{
				socket.SysMessage( GetDictionaryEntry( 801, socket.language )); // You cannot mine there
			}
		}
	}
}

function CheckForMiningTool( socket, mChar )
{
	// Verify that mining tool still exists
	if( !ValidateObject( mChar.miningTool ))
	{
		socket.SysMessage( GetDictionaryEntry( 9187, socket.language )); // Tool used to initiate digging can no longer be found!
		return false;
	}

	// Make sure player still has access to mining tool - needs to be equipped or in backpack
	var itemOwner = GetPackOwner( mChar.miningTool, 0 );
	if( itemOwner == null || itemOwner.serial != mChar.serial )
	{
		socket.SysMessage( GetDictionaryEntry( 1971, socket.language )); // This must be in your backpack or equipped before it can be used.
		return false;
	}
	return true;
}

function Mining( socket, mChar, targX, targY )
{
	var mResource = ResourceRegion( targX, targY, mChar.worldnumber );
	RegenerateOre( mResource, socket );
	if( mResource.oreAmount <= 0 )
	{
		mChar.TextMessage( GetDictionaryEntry( 802, socket.language ), false, 0x3b2, 0, mChar.serial ); // There is no metal to mine here.
		return;
	}

	if( mChar.visible == 1 || mChar.visible == 2 )
	{
		mChar.visible = 0;
	}

	mChar.TurnToward( targX, targY );

	if( mChar.bodyType == 2 ) // Gargoyle
	{
		mChar.DoAction( 0x0, 0x3 );
	}
	else if( mChar.isonhorse ) // Mounted
	{
		mChar.DoAction( 0x1A );
	}
	else // On foot
	{
		mChar.DoAction( 0x0B );
	}

	mChar.SoundEffect( 0x0125, true );

	mChar.skillsused.mining = true;
	mChar.miningType = 1; // Regular Mining
	mChar.StartTimer( 1500, 1, true );
}

// Handle functionality of GraveDigging feature
function GraveDigging( socket, mChar, targX, targY )
{
	var mResource = ResourceRegion( targX, targY, mChar.worldnumber );
	RegenerateOre( mResource, socket );
	if( mResource.oreAmount <= 0 )
	{
		mChar.TextMessage( GetDictionaryEntry( 813, socket.language ), false, 0x3b2, 0, mChar.serial ); // This grave seems to be empty
		return;
	}

	// Lower player's karma, but only down to -2 tier
	var karmaChange = 0;
	var karmaLossThreshold = -2000;
	var curKarma = mChar.karma;
	if( curKarma > karmaLossThreshold )
	{
		karmaChange = Math.abs( Math.round(( curKarma - karmaLossThreshold ) / 50 ));
		mChar.karma -= karmaChange;
	}

	if( karmaChange != 0 )
	{
		if( karmaChange <= 25 )
		{
			socket.SysMessage( GetDictionaryEntry( 1368, socket.language )); // You have lost a little karma.
		}
		else if( karmaChange <= 50 )
		{
			socket.SysMessage( GetDictionaryEntry( 1370, socket.language )); // You have lost some karma.
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 1372, socket.language )); // You have lost a lot of karma.
		}
	}

	// Do action and sound
	if( mChar.visible == 1 || mChar.visible == 2 )
	{
		mChar.visible = 0;
	}

	if( mChar.id == 0x029A || mChar.id == 0x029B ) // Gargoyle
	{
		mChar.DoAction( 0x0, 0x3 );
	}
	else if( mChar.isonhorse )
	{
		mChar.DoAction( 0x1A );
	}
	else
	{
		mChar.DoAction( 0x0B );
	}

	mChar.SoundEffect( 0x0125, true );

	mChar.skillsused.mining = true;
	mChar.miningType = 2; // Grave Digging
	mChar.StartTimer( 1500, 0, true );
}

function SandMining( socket, mChar, targX, targY ) 
{
	var mResource = ResourceRegion( targX, targY, mChar.worldnumber );
	RegenerateOre( mResource, socket );
	if( mResource.oreAmount <= 0 )
	{
		mChar.TextMessage( GetDictionaryEntry( 9412, mChar.socket.language ), false, 0x3b2, 0, mChar.serial); // There is no sand here to mine.
		return;
	}

	if( mChar.visible == 1 || mChar.visible == 2 )
	{
		mChar.visible = 0;
	}

	mChar.TurnToward( targX, targY );

	if( mChar.bodyType == 2 ) // Gargoyle
	{
		mChar.DoAction(0x0, 0x3);
	}
	else if( mChar.isonhorse ) // Mounted
	{
		mChar.DoAction( 0x1A );
	}
	else // On foot
	{
		mChar.DoAction( 0x0B );
	}

	var randomNumber = Math.random();
	var value = ( randomNumber < 0.5 ) ? 0x0125 : 0x0126;
	mChar.SoundEffect( value, true );

	mChar.skillsused.mining = true;
	mChar.miningType = 3; // Sand Mining
	mChar.StartTimer( 1500, 1, true );
}

function onTimer( mChar, timerID )
{
	switch( timerID )
	{
		case 0: // Mining action has played out, let there be ore! (maybe)
			mChar.skillsused.mining = false;
			var socket = mChar.socket;
			if( socket != null )
			{
				// Make sure player is still within range of the targeted area
				var targX = mChar.miningTargX;
				var targY = mChar.miningTargY;
				var targZ = mChar.miningTargZ;
				var distX = Math.abs( mChar.x - targX );
				var distY = Math.abs( mChar.y - targY );
				var distZ = Math.abs( mChar.z - targZ );

				if( distX > 5 || distY > 5 || distZ > 35 )
				{
					mChar.socket.SysMessage( GetDictionaryEntry( 799, mChar.socket.language )); // You are too far away to mine that!
					return;
				}

				// Verify player has access to mining tool still
				if( !CheckForMiningTool( socket, mChar ))
					return;

				var miningType = mChar.miningType;
				var mResource = ResourceRegion( targX, targY, mChar.worldnumber );
				if( mResource.oreAmount <= 0 )
				{
					if( miningType == 1 )
					{
						mChar.TextMessage( GetDictionaryEntry( 802, socket.language ), false, 0x3b2, 0, mChar.serial ); // There is no metal to mine here.
					}
					else
					{
						mChar.TextMessage( GetDictionaryEntry( 813, socket.language ), false, 0x3b2, 0, mChar.serial ); // This grave seems to be empty
					}
					return;
				}

				if( miningType == 1 )
				{
					// Regular mining
					if( mChar.CheckSkill( 45, 0, 1000 ))
					{
						// Remove some ore from the ore resource
						mResource.oreAmount = mResource.oreAmount - 1;

						if( mChar.GetTag( "GatheringStone" ) == 1 ) 
						{
							var mItem;
							var totalChance = 0;

							for( mItem = mChar.FirstItem(); !mChar.FinishedItems(); mItem = mChar.NextItem() ) 
							{
								if( !ValidateObject( mItem ))
									continue;

								var stoneminingBonus = parseInt( mItem.GetTag( "stoneminingBonus" ));

								// Check if stoneminingBonus is a number
								if( !isNaN( stoneminingBonus ))
								{
									totalChance += stoneminingBonus;
								}
							}

							// Always add a 15% chance
							totalChance += 15;

							var chance = totalChance || 15;

							if( chance > Math.random() * 100 )
							{
								// Create granite in player's backpack
								MakeGranite( socket, mChar );
							}
						}

						// Create ore in player's backpack
						MakeOre( socket, mChar );
					}
					else
					{
						mChar.TextMessage( GetDictionaryEntry( 803, socket.language ), false, 0x3b2, 0, mChar.serial ); // You sifted through the dirt and rocks, but found nothing usable.
						if( RandomNumber( 0, 1 ))	// 50% chance to destroy some resources
						{
							mResource.oreAmount = mResource.oreAmount - 1;
						}
					}
				}
				else if( miningType == 2 )
				{
					// Grave Digging - piggyback on ore resource system
					if( mChar.CheckSkill( 45, 0, 800 ))
					{
						// Remove some "ore" from the ore resource 50% of the time when digging graves
						if( RandomNumber( 0, 1 ))
						{
							mResource.oreAmount = mResource.oreAmount - 1;
						}

						// Handle outcome of grave digging
						HandleGraveDigOutcome( socket, mChar );
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 9186, socket.language )); // You find nothing in the grave.
						if( RandomNumber( 0, 4 )) // 20% chance to destroy some resources
						{
							mResource.oreAmount = mResource.oreAmount - 1;
						}
					}
				}
				else if( miningType == 3 )
				{
					// Sand mining
					if( mChar.CheckSkill( 45, 0, 1000 ))
					{
						// Remove some ore from the ore resource
						mResource.oreAmount = mResource.oreAmount - 1;

						// Create sand in player's backpack
						MakeSand( socket, mChar );
					}
					else
					{
						mChar.TextMessage( GetDictionaryEntry( 9405, socket.language ), false, 0x3b2, 0, mChar.serial ); // You dig for a while but fail to find any of sufficient quality for glassblowing.
						if( RandomNumber( 0, 1 ))	// 50% chance to destroy some resources
						{
							mResource.oreAmount = mResource.oreAmount - 1;
						}
					}
				}
			}
			break;
		case 1: // Second mining action playing
			if( mChar.visible == 1 || mChar.visible == 2 )
			{
				mChar.visible = 0;
			}

			if( mChar.id == 0x029A || mChar.id == 0x029B ) // Gargoyle
			{
				mChar.DoAction( 0x0, 0x3 );
			}
			else if( mChar.isonhorse )
			{
				mChar.DoAction( 0x1A );
			}
			else
			{
				mChar.DoAction( 0x0B );
			}

			mChar.SoundEffect( 0x0125, true );

			mChar.StartTimer( 1500, 0, true );
			break;
	}
}

function MakeOre( socket, mChar )
{
	var mRegion = mChar.region;
	delete mChar.miningTargX;
	delete mChar.miningTargY;
	delete mChar.miningTargZ;

	var getSkill = mChar.skills.mining;

	// Find base chance of finding any ore in town region
	var findOreChance = RandomNumber( 0, mRegion.GetOreChance() );

	var sumChance = 0;
	var oreFound = false;
	var orePref;
	var oreData = 0;

	for( var oreType = 0; oreType < mRegion.numOrePrefs; oreType++ )
	{
		// Get the townregion's preference for specified ore type
		orePref = mRegion.GetOrePref( oreType );
		if( orePref == null )
			continue;

		/* Overview of data contained in orePref:
			var oreIndex = orePref[0];		// Array with data on specified ore preference
			var orePrefChance = orePref[1];	// Chance of finding ore type in given town region

			var oreName = oreIndex[0];      // name of ore type internal in mining system
			var oreColor = oreIndex[1];		// color of ore
			var minSkill = oreIndex[2];		// minimum skill to mine ore
			var ingotName = oreIndex[3];    // name of dug up ore, and of ingot from smelted ore
			var makeMenu = oreIndex[4];		// make-menu entry for crafting something from ingot
			var oreChance = oreIndex[5];	// default global chance of finding ore type
			var scriptID = oreIndex[6];		// script ID attached to ore*/

		oreData = orePref[0]; // oreIndex
		if( oreData == null )
			continue;

		sumChance += orePref[1]; // default chance of finding ore type;
		if( sumChance > findOreChance )
		{
			if( getSkill >= oreData[2] ) // oreData[2] is minSkill to mine ore )
			{
				var amtToMake = 1;
				if( mRegion.chanceBigOre >= RandomNumber( 1, 100 ))
				{
					amtToMake = 5;
				}

				// Randomize the size of ore oreData
				var oreId = RandomNumber( 0x19B7, 0x19BA );

				var oreName = oreData[3].toLowerCase() + " ore"; // oreData[3] is name of actual ore dug up & ingot created from it
				var oreItem = CreateBlankItem( socket, mChar, amtToMake, oreName, oreId, oreData[1], "ITEM", true ); // oreData[1] is oreColor
				if( ValidateObject( oreItem ))
				{
					oreItem.name = oreName;
					oreItem.sectionID = "ore";
					if( oreData[6] != 0 )
					{
						oreItem.AddScriptTrigger( oreData[6] ); // oreData[6] is scriptID
					}
					if( oreItem.container != null )
					{
						var dictmsg = GetDictionaryEntry( 982, socket.language ); // You place some %s in your pack
						//socket.SysMessage( dictmsg.replace( /%s/gi, oreName ));
						mChar.TextMessage( dictmsg.replace( /%s/gi, oreName ), false, 0x3b2, 0, mChar.serial ); // You place some %s in your pack
					}
				}
				oreFound = true;
				break;
			}
		}
	}

	if( !oreFound )
	{
		// No ore was found, what about gems?
		if( getSkill >= 850 )
		{
			var randomGem = CreateDFNItem( socket, mChar, "digginggems", 1, "ITEM", true );
			if( Validateobject( randomGem ) && ValidateObject( randomGem.container ))
			{
				mChar.TextMessage( GetDictionaryEntry( 983, socket.language ), false, 0x3b2, 0, mChar.serial ); // You place a gem in your pack.
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 1772, socket.language )); // You do not have enough skill to mine that ore!
		}
	}

	HandleToolUse( socket, mChar );
}

function MakeGranite( socket, mChar ) 
{
	var iSpawned = null;
	iSpawned = CreateDFNItem( socket, mChar, "randomgranite", 1, "ITEM", true );
	if( ValidateObject( iSpawned ))
	{
		socket.SysMessage( GetDictionaryEntry( 9406, socket.language )); // You carefully extract some workable stone from the ore vein!
	}

	HandleToolUse( socket, mChar );
}

function MakeSand(socket, mChar) 
{
	const coreShardEra = GetServerSetting( "CoreShardEra" );
	var rand = RandomNumber( 0, 1 );
	var iSpawned = null;
	iSpawned = CreateDFNItem( socket, mChar, "sand", 1, "ITEM", true );

	if( ValidateObject( iSpawned ))
	{
		if( EraStringToNum( coreShardEra ) <= EraStringToNum( "sa" ))
		{
			switch( rand )
			{
				case 0: iSpawned.id = 0x11EA;
				case 1: iSpawned.id = 0x11EB;
			}
			iSpawned.pileable = 0;
			iSpawned.colour = 0;

		}
		socket.SysMessage( GetDictionaryEntry( 9407, socket.language )); // You carefully dig up sand of sufficient quality for glassblowing.
	}

	HandleToolUse( socket, mChar );
}

function HandleGraveDigOutcome( socket, mChar )
{
	var npcToSpawn = null;
	var iSpawned = null;
	var graveDigOutcome = WeightedRandom( 1, 7 );
	switch( graveDigOutcome )
	{
		case 1: // Spawn nothing, found nothing
			mChar.TextMessage( GetDictionaryEntry( 803, socket.language ), false, 0x3b2, 0, mChar.serial ); // You sifted through the dirt and rocks, but found nothing usable.
			break;
		case 2: // Spawn random bones
			iSpawned = CreateDFNItem( socket, mChar, "randombones", 1, "ITEM", true );
			if( ValidateObject( iSpawned ))
			{
				mChar.TextMessage( GetDictionaryEntry( 812, socket.language ), false, 0x3b2, 0, mChar.serial ); // You have unearthed some old bones and placed them in your pack.
			}
			break;
		case 3: // Spawn low level undead from weakundeadlist
			npcToSpawn = "weakundeadlist";
			mChar.TextMessage( GetDictionaryEntry( 806, socket.language ), false, 0x3b2, 0, mChar.serial ); // You have disturbed the rest of a vile undead creature.
			break;
		case 4: // Spawn random armors from diggingarmor list
			iSpawned = CreateDFNItem( socket, mChar, "diggingarmor", 1, "ITEM", true );
			if( ValidateObject( iSpawned ))
			{
				if( iSpawned.id >= 7026 && iSpawned.id <= 7035 )
				{
					mChar.TextMessage( GetDictionaryEntry( 807, socket.language ), false, 0x3b2, 0, mChar.serial ); // You have found an old piece of armour and placed it in your pack.
				}
				else
				{
					mChar.TextMessage( GetDictionaryEntry( 807, socket.language ), false, 0x3b2, 0, mChar.serial ); // You unearthed an old shield and placed it in your pack.
				}
			}
			break;
		case 5: // Spawn random treasure (gems/gold)
			if( RandomNumber( 0, 1 ))
			{
				// Randomly create a gem and place in backpack
				iSpawned = CreateDFNItem( socket, mChar, "digginggems", 1, "ITEM", true );
				if( ValidateObject( iSpawned ))
				{
					mChar.TextMessage( GetDictionaryEntry( 809, socket.language ), false, 0x3b2, 0, mChar.serial ); // You place a gem in your pack
				}
			}
			else
			{
				// Create between 1 and 15 gold and place in backpack
				var goldAmount = WeightedRandom( 1, 15 );
				iSpawned = CreateDFNItem( socket, mChar, "0x0EED", goldAmount, "ITEM", true );
				if( ValidateObject( iSpawned ))
				{
					var dictMsg = GetDictionaryEntry( 810, socket.language ); // You unearthed %i gold coins.
					if( goldAmount == 1 )
					{
						mChar.SoundEffect( 0x0035, false );
					}
					else if( goldAmount < 6 )
					{
						mChar.SoundEffect( 0x0036, false );
					}
					else
					{
						mChar.SoundEffect( 0x0037, false );
					}
					mChar.TextMessage( dictMsg.replace( /%i/gi, goldAmount ), false, 0x3b2, 0, mChar.serial ); // You unearthed %i gold coins.
				}
			}
			break;
		case 6: // Spawn random weapons from diggingweapons list
			iSpawned = CreateDFNItem( socket, mChar, "diggingweapons", 1, "ITEM", true );
			if( ValidateObject( iSpawned ))
			{
				//socket.SysMessage( GetDictionaryEntry( 811, socket.language )); // You unearthed a old weapon and placed it in your pack.
				mChar.TextMessage( GetDictionaryEntry( 811, socket.language ), false, 0x3b2, 0, mChar.serial ); // You unearthed a old weapon and placed it in your pack.
			}
			break;
		case 7: // Spawn Medium-to-High level undeads based on player's fame
			if( mChar.fame < 1000 )
			{
				npcToSpawn = "allundeadlist";
			}
			else
			{
				npcToSpawn = "strongundeadlist";
			}
			mChar.TextMessage( GetDictionaryEntry( 806, socket.language ), false, 0x3b2, 0, mChar.serial ); // You have disturbed the rest of a vile undead creature.
			break;
		default:
			break;
	}

	// Spawn selected creature, if there is one to spawn
	if( npcToSpawn != null )
	{
		var targX = mChar.miningTargX;
		var targY = mChar.miningTargY;
		var targZ = mChar.miningTargZ;
		var nSpawned = SpawnNPC( npcToSpawn, targX, targY, targZ, mChar.worldnumber, mChar.instanceID, true );
	}
}

function HandleToolUse( socket, mChar )
{
	var miningTool = mChar.miningTool;
	if( !ValidateObject( miningTool ) || !toolUseLimit )
		return;

	if( miningTool.usesLeft > 0 )
	{
		miningTool.usesLeft -= 1;
		if( miningTool.usesLeft == 0 )
		{
			socket.SysMessage( GetDictionaryEntry( 10202, socket.language )); // You have worn out your tool!
			if( toolUseBreak )
			{
				// Clean up mining tool reference and delete tool
				delete mChar.miningTool;
				miningTool.Delete();
			}
		}
	}
}

function RegenerateOre( mResource, socket)
{
	var oreCeiling	= ResourceAmount( "ORE" );
	var oreTimer	= ResourceTime( "ORE" );
	var currentTime	= GetCurrentClock();

	if( mResource.oreTime <= currentTime )
	{
		for( var i = 0; i < oreCeiling; ++i )
		{
			if(( mResource.oreTime + ( i * oreTimer * 1000 )) <= currentTime && mResource.oreAmount < oreCeiling )
			{
				mResource.oreAmount = mResource.oreAmount + 1;
			}
			else
			{
				break;
			}
		}
		mResource.oreTime = ( currentTime + ( 1000 * oreTimer ));
	}
}

// Weighted randomizer for quick and easy distribution of rewards based on difficulty
function WeightedRandom( min, max )
{
 	return Math.round( max / ( Math.random() * max + min ));
}

function _restorecontext_() {}