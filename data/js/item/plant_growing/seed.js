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
					// You plant the seed in the bowl of dirt.
					/*pUser.SysMessage("You plant the seed in the bowl of dirt.");
					if (parseInt(iUsed.GetTag("CampionFlowerSeed")) == 1) {
						myTarget.SetTag("planttype", 1);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("PoppieSeed")) == 1) {
						myTarget.SetTag("planttype", 2);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("SnowdropSeed")) == 1) {
						myTarget.SetTag("planttype", 3);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("BulrusheSeed")) == 1) {
						myTarget.SetTag("planttype", 4);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("LilieSeed")) == 1) {
						myTarget.SetTag("planttype", 5);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("PampasGrassSeed")) == 1) {
						myTarget.SetTag("planttype", 6);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("RusheSeed")) == 1) {
						myTarget.SetTag("planttype", 7);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("ElephantEarPlantrSeed")) == 1) {
						myTarget.SetTag("planttype", 8);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("FernSeed")) == 1) {
						myTarget.SetTag("planttype", 9);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("PonytailPalmSeed")) == 1) {
						myTarget.SetTag("planttype", 10);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("SmallPalmSeed")) == 1) {
						myTarget.SetTag("planttype", 11);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("CenturyPlantSeed")) == 1) {
						myTarget.SetTag("planttype", 12);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("WaterPlantSeed")) == 1) {
						myTarget.SetTag("planttype", 13);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("SnakePlantSeed")) == 1) {
						myTarget.SetTag("planttype", 14);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("PricklyPearCactusSeed")) == 1) {
						myTarget.SetTag("planttype", 15);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("BarrelCactusSeed")) == 1) {
						myTarget.SetTag("planttype", 16);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("TribarrelCactusSeed")) == 1) {
						myTarget.SetTag("planttype", 17);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("CommonGreenBonsaiSeed")) == 1) {
						myTarget.SetTag("planttype", 18);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("CommonPinkBonsaiSeed")) == 1) {
						myTarget.SetTag("planttype", 19);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("UncommonGreenBonsai")) == 1) {
						myTarget.SetTag("planttype", 20);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("UncommonPinkBonsai")) == 1) {
						myTarget.SetTag("planttype", 21);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("RareGreenBonsai")) == 1) {
						myTarget.SetTag("planttype", 22);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("RarePinkBonsai")) == 1) {
						myTarget.SetTag("planttype", 23);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("ExceptionalBonsai")) == 1) {
						myTarget.SetTag("planttype", 24);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("ExoticBonsai")) == 1) {
						myTarget.SetTag("planttype", 25);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("Cactus")) == 1) {
						myTarget.SetTag("planttype", 26);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("FlaxFlowers")) == 1) {
						myTarget.SetTag("planttype", 27);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("FoxgloveFlowers")) == 1) {
						myTarget.SetTag("planttype", 28);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("HopsEast")) == 1) {
						myTarget.SetTag("planttype", 29);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("OrfluerFlowers")) == 1) {
						myTarget.SetTag("planttype", 30);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("CypressTwisted")) == 1) {
						myTarget.SetTag("planttype", 31);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("HedgeShort")) == 1) {
						myTarget.SetTag("planttype", 32);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("JuniperBush")) == 1) {
						myTarget.SetTag("planttype", 33);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("SnowdropPatch")) == 1) {
						myTarget.SetTag("planttype", 34);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("Cattails")) == 1) {
						myTarget.SetTag("planttype", 35);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("PoppyPatch")) == 1) {
						myTarget.SetTag("planttype", 36);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("SpiderTree")) == 1) {
						myTarget.SetTag("planttype", 37);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("WaterLily")) == 1) {
						myTarget.SetTag("planttype", 38);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("CypressStraight")) == 1) {
						myTarget.SetTag("planttype", 39);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("HedgeTall")) == 1) {
						myTarget.SetTag("planttype", 40);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("HopsSouth")) == 1) {
						myTarget.SetTag("planttype", 41);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}
					else if (parseInt(iUsed.GetTag("SugarCanes")) == 1) {
						myTarget.SetTag("planttype", 42);
						myTarget.SetTag("plantname", iUsed.name);
						myTarget.SetTag("plantcolor", iUsed.colour);
					}*/

					// Define a mapping between seed names and plant types
					const seedToPlantType = {
						"CampionFlowerSeed": 1,
						"PoppieSeed": 2,
						"SnowdropSeed": 3,
						"BulrusheSeed": 4,
						"LilieSeed": 5,
						"PampasGrassSeed": 6,
						"RusheSeed": 7,
						"ElephantEarPlantrSeed": 8,
						"FernSeed": 9,
						"PonytailPalmSeed": 10,
						"SmallPalmSeed": 11,
						"CenturyPlantSeed": 12,
						"WaterPlantSeed": 13,
						"SnakePlantSeed": 14,
						"PricklyPearCactusSeed": 15,
						"BarrelCactusSeed": 16,
						"TribarrelCactusSeed": 17,
						"CommonGreenBonsaiSeed": 18,
						"CommonPinkBonsaiSeed": 19,
						"UncommonGreenBonsai": 20,
						"UncommonPinkBonsai": 21,
						"RareGreenBonsai": 22,
						"RarePinkBonsai": 23,
						"ExceptionalBonsai": 24,
						"ExoticBonsai": 25,
						"Cactus": 26,
						"FlaxFlowers": 27,
						"FoxgloveFlowers": 28,
						"HopsEast": 29,
						"OrfluerFlowers": 30,
						"CypressTwisted": 31,
						"HedgeShort": 32,
						"JuniperBush": 33,
						"SnowdropPatch": 34,
						"Cattails": 35,
						"PoppyPatch": 36,
						"SpiderTree": 37,
						"WaterLily": 38,
						"CypressStraight": 39,
						"HedgeTall": 40,
						"HopsSouth": 41,
						"SugarCanes": 42
					};

					// You plant the seed in the bowl of dirt.
					pUser.SysMessage("You plant the seed in the bowl of dirt.");

					// Loop through the keys of seedToPlantType to find the matching tag
					for (var tagName in seedToPlantType) {
						if (parseInt(iUsed.GetTag(tagName)) === 1)
						{
							var plantType = seedToPlantType[tagName];
							if (!isNaN(plantType)) 
							{ // Check if plantType is a valid number
								myTarget.SetTag("PlantType", plantType);
								myTarget.SetTag("PlantName", iUsed.name); // Name of plant it is from dfn files
								myTarget.SetTag("PlantColor", iUsed.colour); // Colour of plant it is from dfn files
								break; // Exit the loop once a match is found
							}
							else 
							{
								// Handle the case where the plantType is not a valid number
								pUser.SysMessage("Invalid plantType:", plantType, "Page and GM");
							}
						}
					}
					const PlantDelayTimer = 5000;//82800000 Every 23 hours plant grows

					myTarget.StartTimer(PlantDelayTimer, 1, 19100);
					myTarget.SetTag("PlantStage", 1);
					myTarget.SetTag("Seeds", 0 + "," + 8 + "," + iUsed.colour);
					myTarget.SetTag("PlantCross", 0 + "," + plantType);

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