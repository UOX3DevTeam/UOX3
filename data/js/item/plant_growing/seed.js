function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem ) 
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner != pUser )
		{
			socket.SysMessage( GetDictionaryEntry( 1763, pUser.socket.language )); //That item must be in your backpack before it can be used.
		}
		else 
		{
			pUser.socket.tempObj = iUsed;
			pUser.CustomTarget( 0, GetDictionaryEntry( 19137, pSock.language ));//Choose a bowl of dirt to plant this seed in.
			return false;
		}
	}
	return true;
}

function onCallback0( pSock, myTarget )
{
	const PlantDelayTimer = 82800000;//82800000 Every 23 hours plant grows
	var iUsed = pSock.tempObj;
	var pUser = pSock.currentChar;

	// That must be in your pack for you to use it.
	if( !pSock.GetWord( 1 ) && myTarget.isItem )
	{
		var plantStage = myTarget.GetTag("PlantStage");
		var plantInfo = myTarget.GetTag( "PlantInfo" );
		var waterLevel = myTarget.GetTag("water");

		if( !plantInfo ) 
			return false;

		var infoLength = plantInfo.split( "," );
		if( infoLength.length != 4 )
			return false;

		var plantType = parseInt( infoLength[0] );
		var PlantName = parseInt( infoLength[1] );
		var plantColor = parseInt( infoLength[2] );
		var fertialeDirt = parseInt( infoLength[3] );
		var itemOwner = GetPackOwner( myTarget, 0 );

		if( itemOwner == null || itemOwner != pUser )
		{
			pSock.SysMessage( GetDictionaryEntry( 1763, pSock.language )); //That item must be in your backpack before it can be used.
		}
		else
		{
			if( ValidateObject( myTarget ) && myTarget.id == 0x1602 || myTarget.sectionID == "plantbowlOfdirt" ) //checking to make sure its a full bowl of dirt
			{
				if( plantStage >= 18 ) // FullGrownPlant
				{
					// You must use a seed on some prepared soil!
					pSock.SysMessage( GetDictionaryEntry( 19138, pSock.language ));//You must use a seed on some prepared soil!
				}
				else if( plantStage != 14 && PlantName ) // BowlOfDirt or PlantName
				{
					var tempMsg = GetDictionaryEntry( 19139, socket.language );
					pSock.SysMessage( tempMsg.replace( /%s/gi, PlantName ));//This bowl of dirt already has a seed %s in it!
					// This bowl of dirt already has a seed of %s in it!
				}
				else if( waterLevel < 2 ) // not enough water
				{
					pSock.SysMessage( GetDictionaryEntry( 19140, pSock.language ));//The dirt needs to be softened first.
				}
				else
				{
					const seedToPlantType = {
						"CampionFlowerSeed": { type: 1, crossable: 1 },
						"PoppieSeed": { type: 2, crossable: 1 },
						"SnowdropSeed": { type: 3, crossable: 1 },
						"BulrusheSeed": { type: 4, crossable: 1 },
						"LilieSeed": { type: 5, crossable: 1 },
						"PampasGrassSeed": { type: 6, crossable: 1 },
						"RusheSeed": { type: 7, crossable: 1 },
						"ElephantEarPlantrSeed": { type: 8, crossable: 1 },
						"FernSeed": { type: 9, crossable: 1 },
						"PonytailPalmSeed": { type: 10, crossable: 1 },
						"SmallPalmSeed": { type: 11, crossable: 1 },
						"CenturyPlantSeed": { type: 12, crossable: 1 },
						"WaterPlantSeed": { type: 13, crossable: 1 },
						"SnakePlantSeed": { type: 14, crossable: 1 },
						"PricklyPearCactusSeed": { type: 15, crossable: 1 },
						"BarrelCactusSeed": { type: 16, crossable: 1 },
						"TribarrelCactusSeed": { type: 17, crossable: 1 },
						"CommonGreenBonsaiSeed": { type: 18, crossable: 0 },
						"CommonPinkBonsaiSeed": { type: 19, crossable: 0 },
						"UncommonGreenBonsai": { type: 20, crossable: 0 },
						"UncommonPinkBonsai": { type: 21, crossable: 0 },
						"RareGreenBonsai": { type: 22, crossable: 0 },
						"RarePinkBonsai": { type: 23, crossable: 0 },
						"ExceptionalBonsai": { type: 24, crossable: 0 },
						"ExoticBonsai": { type: 25, crossable: 0 },
						"Cactus": { type: 26, crossable: 0 },
						"FlaxFlowers": { type: 27, crossable: 0 },
						"FoxgloveFlowers": { type: 28, crossable: 0 },
						"HopsEast": { type: 29, crossable: 0 },
						"OrfluerFlowers": { type: 30, crossable: 0 },
						"CypressTwisted": { type: 31, crossable: 0 },
						"HedgeShort": { type: 32, crossable: 0 },
						"JuniperBush": { type: 33, crossable: 0 },
						"SnowdropPatch": { type: 34, crossable: 0 },
						"Cattails": { type: 35, crossable: 0 },
						"PoppyPatch": { type: 36, crossable: 0 },
						"SpiderTree": { type: 37, crossable: 0 },
						"WaterLily": { type: 38, crossable: 0 },
						"CypressStraight": { type: 39, crossable: 0 },
						"HedgeTall": { type: 40, crossable: 0 },
						"HopsSouth": { type: 41, crossable: 0 },
						"SugarCanes": { type: 42, crossable: 0 }
					};

					// Loop through the keys of seedToPlantType to find the matching tag
					for( var tagName in seedToPlantType )
					{
						if( parseInt( iUsed.GetTag( tagName )) == 1 ) 
						{
							var tplantType = seedToPlantType[tagName];
							if( !isNaN( tplantType.type ))
							{
								myTarget.SetTag( "PlantInfo", tplantType.type + "," + iUsed.name + "," + iUsed.colour + "," + fertialeDirt );
								myTarget.SetTag( "PlantCross", 0 + "," + tplantType.type + "," + tplantType.crossable );
								break;
							}
							else 
							{
								// Handle the case where the plantType is not a valid number
								pSock.SysMessage( "Invalid plantType:", tplantType.type, "Page and GM" );
								break;
							}
						}
					}

					myTarget.StartTimer( PlantDelayTimer, 1, 19100 );
					myTarget.SetTag( "PlantStage", 1 );//Seed
					objMade.SetTag( "Seed", 0 + "," + 8 + "," + iUsed.colour );//Min/max/color
					pSock.SysMessage( GetDictionaryEntry( 19141, pSock.language ));//You plant the seed in the bowl of dirt.

					if( iUsed.amount > 1 )
						iUsed.amount--;
					else
						iUsed.Delete();
				}
			}
			else
			{
				pUser.SysMessage( GetDictionaryEntry( 19142, pSock.language ));//You must use a seed on a bowl of dirt!
				return false;
			}
		}
		return false;
	}
	return false;
}