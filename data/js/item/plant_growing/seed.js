function onUseChecked( pUser, iUsed )
{
	var itemOwner = GetPackOwner( iUsed, 0 );
	if( itemOwner == null || itemOwner != pUser )
	{
		pUser.SysMessage( GetDictionaryEntry( 1763, pUser.socket.language )); //That item must be in your backpack before it can be used.
	}
	else
	{
		pUser.socket.tempObj = iUsed;
		pUser.CustomTarget(0, "Choose a bowl of dirt to plant this seed in." );
	}
	return false;
}

function onCallback0(pSock, myTarget)
{
	var iUsed = pSock.tempObj;
	var pUser = pSock.currentChar;

	// That must be in your pack for you to use it.
	if (!pSock.GetWord(1) && myTarget.isItem)
	{
		var plantInfo = myTarget.GetTag("PlantInfo")

		if (!plantInfo) 
		{
			return false;
		}

		var infoLength = plantInfo.split(",");
		if (infoLength.length != 4)
			return false;

		var plantType = parseInt(infoLength[0]);
		var PlantName = parseInt(infoLength[1]);
		var plantColor = parseInt(infoLength[2]);
		var fertialeDirt = parseInt(infoLength[3]);
		var itemOwner = GetPackOwner(myTarget, 0);

		if (itemOwner == null || itemOwner != pUser)
		{
			pUser.SysMessage(GetDictionaryEntry(1763, pSock.language)); //That item must be in your backpack before it can be used.
		}
		else
		{
			if (ValidateObject(myTarget) && myTarget.id == 0x1602) //checking to make sure its a full bowl of dirt
			{
				if (myTarget.GetTag("PlantStage") >= 18) // FullGrownPlant
				{
					// You must use a seed on some prepared soil!
					pSock.SysMessage("You must use a seed on some prepared soil!");
				}
				else if (myTarget.GetTag("PlantStage") != 14 && iUsed.GetTag("PlantName")) // BowlOfDirt or PlantName
				{
					pUser.SysMessage("This bowl of dirt already has a seed " + myTarget.GetTag("PlantName") + " in it!");
					// This bowl of dirt already has a seed of %s in it!
				}
				else if (myTarget.GetTag("water") < 2) // not enough water
				{
					pSock.SysMessage("The dirt needs to be softened first.");
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

					// You plant the seed in the bowl of dirt.
					pUser.SysMessage("You plant the seed in the bowl of dirt.");

					// Loop through the keys of seedToPlantType to find the matching tag
					for (var tagName in seedToPlantType) {
						if (parseInt(iUsed.GetTag(tagName)) === 1)
						{
							var tplantType = seedToPlantType[tagName];
							if (!isNaN(tplantType.type))
							{
								myTarget.SetTag("PlantInfo", tplantType.type + "," + iUsed.name + "," + iUsed.colour + "," + fertialeDirt);
								myTarget.SetTag("PlantCross", 0 + "," + tplantType.type + "," + tplantType.crossable);
								break;
							}
							else 
							{
								// Handle the case where the plantType is not a valid number
								pUser.SysMessage("Invalid plantType:", tplantType.type, "Page and GM");
							}
						}
					}
					const PlantDelayTimer = 5000;//82800000 Every 23 hours plant grows

					myTarget.StartTimer(PlantDelayTimer, 1, 19100);
					myTarget.SetTag("PlantStage", 1);
					myTarget.SetTag("Seeds", 0 + "," + 8 + "," + iUsed.colour);

					if (iUsed.amount > 1)
						iUsed.amount--;
					else
						iUsed.Delete();
				}
			}
			else
				pUser.SysMessage("You must use a seed on a bowl of dirt!");//You must use a seed on a bowl of dirt!
			return false;
		}
		return false;
	}
	return false;
}