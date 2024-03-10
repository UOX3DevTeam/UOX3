const scriptID = 19100;// this is the script id
const PlantDelayTimer = 5000;//82800000 Every 23 hours plant grows

function onUseChecked(pUser, iUsed)
{
	var gumpID = scriptID + 0xffff;
	var socket = pUser.socket;

	socket.tempObj = iUsed;
	socket.CloseGump(gumpID, 0);

	PlantBowlGump(pUser, iUsed);
}

function onCreateDFN(objMade, objType)
{
	if (objType == 0)
	{
		var plantInfo = objMade.GetTag("PlantInfo")

		if (!plantInfo) 
		{
			objMade.SetTag("PlantInfo", 0 + "," + 0 + "," + 0 + "," + 0);
			objMade.SetTag("Potions", 0 + "," + 0 + "," + 0 + "," + 0);
			return false;
		}
	}
}

function PlantBowlGump(pUser, iUsed)
{
	var PlantGump = new Gump;
	var socket = pUser.socket;
	var greaterPoison = iUsed.GetTag("GreaterPoison");
	var greaterCure = iUsed.GetTag("GreaterCure");
	var greaterHeal = iUsed.GetTag("GreaterHeal");
	var greaterStrength = iUsed.GetTag("GreaterStrength");

	DrawBackground(PlantGump);

	DrawPlant(PlantGump, iUsed);

	PlantGump.AddButton(71, 67, 0xD4, 0xD4, 1, 1, 1); // Reproduction menu
	PlantGump.AddPicture(59, 68, 0xD08);

	PlantGump.AddButton(71, 91, 0xD4, 0xD4, 2, 2, 2); // infestation
	PlantGump.AddPicture(8, 96, 0x372);
	AddPlus(PlantGump, iUsed);

	PlantGump.AddButton(71, 115, 0xD4, 0xD4, 3, 3, 3); // fungus
	PlantGump.AddPicture(58, 115, 0xD16);
	AddPlus(PlantGump, iUsed);

	PlantGump.AddButton(71, 139, 0xD4, 0xD4, 4, 4, 4); // poison
	PlantGump.AddPicture(59, 143, 0x1AE4);
	AddPlus(PlantGump, iUsed);

	PlantGump.AddButton(71, 163, 0xD4, 0xD4, 5, 5, 5); // disease
	PlantGump.AddPicture(55, 167, 0x1727);
	AddPlus(PlantGump, iUsed);

	PlantGump.AddButton(209, 67, 0xD4, 0xD4, 6, 6, 6); // water
	PlantGump.AddPicture(193, 67, 0x1F9D);
	AddWaterLevel(PlantGump, iUsed);

	PlantGump.AddButton(209, 91, 0xD4, 0xD4, 7, 7, 7); // poison potion
	PlantGump.AddPicture(201, 91, 0xF0A);
	PlantGump.AddText(196, 91, 0x835, greaterPoison);

	PlantGump.AddButton(209, 115, 0xD4, 0xD4, 8, 8, 8); // Cure potion
	PlantGump.AddPicture(201, 115, 0xF07);
	PlantGump.AddText(196, 115, 0x835, greaterCure);

	PlantGump.AddButton(209, 139, 0xD4, 0xD4, 9, 9, 9); // Heal potion
	PlantGump.AddPicture(201, 139, 0xF0C);
	PlantGump.AddText(196, 139, 0x835, greaterHeal);

	PlantGump.AddButton(209, 163, 0xD4, 0xD4, 10, 10, 10); // Strength potion
	PlantGump.AddPicture(201, 163, 0xF09);
	PlantGump.AddText(196, 163, 0x835, greaterStrength);

	PlantGump.AddGump(48, 47, 0xD2);
	if (iUsed.GetTag("PlantStage") >= 1)
	{
		PlantGump.AddText(54, 47, 0x835, iUsed.GetTag("PlantStage").toString());
	}

	PlantGump.AddGump(232, 47, 0xD2);
	AddGrowthIndicator(PlantGump, iUsed);

	PlantGump.AddButton(48, 183, 0xD2, 0xD2, 11, 11, 11); // Help
	PlantGump.AddText(54, 183, 0x835, "?");
	PlantGump.AddButton(232, 183, 0xD4, 0xD4, 12, 12, 12); // Empty the bowl
	PlantGump.AddPicture(219, 180, 0x15FD);
	PlantGump.Send(socket);
	PlantGump.Free();
}

function DrawBackground( PlantGump )
{
	PlantGump.AddBackground( 50, 50, 200, 150, 0xE10 );
	PlantGump.AddPicture( 45, 45, 0xCEF );
	PlantGump.AddPicture( 45, 118, 0xCF0 );
	PlantGump.AddPicture( 211, 45, 0xCEB );
	PlantGump.AddPicture( 211, 118, 0xCEC );
	return;
}

function AddPlus(PlantGump, iUsed)
{
	// Define positions and colors for each tag
	var tagPositions = {
		"infestation": { "position": 92, "color1": 0x35, "color2": 0x21 },
		"fungus": { "position": 116, "color1": 0x35, "color2": 0x21 },
		"poison": { "position": 140, "color1": 0x35, "color2": 0x21 },
		"disease": { "position": 164, "color1": 0x35, "color2": 0x21 }
	};

	// Loop through each tag and add "+" accordingly
	for (var tag in tagPositions) 
	{
		var positionInfo = tagPositions[tag];
		var tagValue = iUsed.GetTag(tag);
		var color = tagValue == 1 ? positionInfo.color1 : (tagValue == 2 ? positionInfo.color2 : null);
		if (color !== null) {
			PlantGump.AddText(95, positionInfo.position, color, "+");
		}
	}
}

function AddWaterLevel( PlantGump, iUsed )
{
	var addPlusMinus = "";
	var addHue = 0;
	switch( iUsed.GetTag( "water" ))
	{
		case 0: // severely under-watered.
			addHue = 0x21;
			addPlusMinus = "-";
			break;
		case 1: // slightly under-watered
			addHue = 0x35;
			addPlusMinus = "-";
			break;
		case 3: // slightly over-watered
			addHue = 0x35;
			addPlusMinus = "+";
			break;
		case 4: // severely over-watered
			addHue = 0x21;
			addPlusMinus = "+";
			break;
	}
	if ( addPlusMinus != 0 )
	{
		PlantGump.AddText( 196, 67, addHue, addPlusMinus );
	}
	return;
}

function HealthStatus(myPlant) 
{
	var status = myPlant.GetTag("PlantStage");
	var maxhealth = myPlant.maxhp + status * 2 + 10;
	var health = (myPlant.health / maxhealth) * 100;

	var plantHealth = 0;

	if (health < 33)
	{
		plantHealth = 10; // Dying
	}
	else if (health < 66) 
	{
		plantHealth = 11; // Wilted
	} 
	else if (health < 100)
	{
		plantHealth = 12; // Healthy
	} 
	else
	{
		plantHealth = 13; // Vibrant
	}

	myPlant.SetTag("PlantHealth", plantHealth);
}

function PlantDamage(iUsed)
{
	if (iUsed.health <= 0) 
	{
		Die(iUsed);
		return;
	}

	var damage = 0;
	var tags = ["infestation", "fungus", "poison", "disease"];
	var tag;
	for (var i = 0, len = tags.length; i < len; i++) 
	{
		tag = tags[i];
		if (iUsed.GetTag(tag) > 0)
		{
			damage += iUsed.GetTag(tag) * RandomNumber(3, 6);
		}
	}

	var waterLevel = iUsed.GetTag("water");
	if (waterLevel >= 3 || waterLevel <= 1) 
	{
		damage += Math.abs(2 - waterLevel) * RandomNumber(3, 6);
	}

	// Ensure health doesn't go below 0
	iUsed.health = Math.max(0, iUsed.health - damage);
}

function AddGrowthIndicator(PlantGump, iUsed )
{
	var plantInfo = iUsed.GetTag("PlantInfo")

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

	if (iUsed.GetTag("PlantStage") >= 1 && iUsed.GetTag("PlantStage") <= 9)
	{
		const gi = iUsed.GetTag("PlantHealth");
		
		if (!inValidLocation(iUsed)) 
		{
			PlantGump.AddText(239, 47, 0x21, "!");
		}
		else if (gi >= 10 && gi <= 11)
		{
			PlantGump.AddText(239, 47, 0x21, "-"); //Not Healthy
		}
		//else if(growthdelay)
		//PlantGump.AddText(239, 47, 0x35, "-");// delay
		else if (iUsed.GetTag("PlantStage") == 9)
		{
			PlantGump.AddText(239, 47, 0x3, "+"); // Blue + : The plant successfully grew.
		}// Green + : The plant successfully grew, and got an extra bonus growth from being planted in fertile dirt.
		else if (fertialeDirt == 1)
		{
			PlantGump.AddText(239, 47, 0x3F, "+"); // Double Grown
		}
		return;
	}
}

function Die(myPlant)
{
	myPlant.KillTimers();

	if (myPlant.GetTag( "PlantStage" ) >= 9 )
	{
		myPlant.SetTag("PlantStage", 20);//dead twigs
		myPlant.id = 0x1B9D;
		myPlant.colour = 0;
		myPlant.name = "twigs";
		myPlant.RemoveScriptTrigger(scriptID);
	}
	else
	{
		myPlant.SetTag("PlantStage", 14);//dirt
	}

	ResetPlant(myPlant);

	return;
}

function inValidLocation(myPlant) 
{
	if (myPlant.movable != 2 && myPlant.movable != 3)
	{
		return true; // Plant can be placed here
	} 
	else 
	{
		return false; // Plant cannot be placed here
	}
}

function GrowthCheck( myPlant, pUser )
{
	var waterLevel = myPlant.GetTag("water");
	var infestsationLevel = myPlant.GetTag("infestation");
	var fungusLevel = myPlant.GetTag("fungus");
	var poisonLevel = myPlant.GetTag("poison");
	var diseaseLevel = myPlant.GetTag("disease");

	var greaterPoison = myPlant.GetTag("GreaterPoison");
	var greaterCure = myPlant.GetTag("GreaterCure");
	var greaterStrength = myPlant.GetTag("GreaterStrength");

	var plantInfo = myPlant.GetTag("PlantInfo")

	if (!plantInfo) 
		return false;

	var infoLength = plantInfo.split(",");
	if (infoLength.length != 4)
		return false;

	var plantType = parseInt(infoLength[0]);
	var PlantName = parseInt(infoLength[1]);
	var plantColor = parseInt(infoLength[2]);
	var fertialeDirt = parseInt(infoLength[3]);

	var chanceType = 0;
	switch (plantType) 
	{
		case 1:
			chanceType = 1;
			break;
		case 27:
			chanceType = 27;
			break;
		case 28:
			chanceType = 28;
			break;
		case 30:
			chanceType = 30;
			break;
		default:
			chanceType = 0;
			break;
	}

    var chanceColor = 0
	switch( myPlant.GetTag( "PlantColor" )) 
	{
		case 21:
			chanceColor = 21; // Bright Red
			break;
		case 5:
			chanceColor = 5;  // Bright Blue
			break;
		case 38:
			chanceColor = 38; // Bright Yellow
			break;
		case 10:
			chanceColor = 10; // Bright Purple
			break;
		case 42:
			chanceColor = 42; // Bright Green
			break;
		case 43:
			chanceColor = 43; // Bright Orange
			break;
		default:
			chanceColor = 0;
			break;
	}
	var infestationChance = 0.30 - greaterStrength * 0.075 + (waterLevel - 2) * 0.10;

	// Flowers have a 10% higher chance to become infected
	if (chanceType == 1 || chanceType == 27 || chanceType == 30)
	{
		infestationChance += 0.10;
	}

	// For plants with a bright color, the chance is another 10% higher
	if (chanceColor == 21 || chanceColor == 5 || chanceColor == 38 || chanceColor == 10 || chanceColor == 42 || chanceColor == 43) 
	{
		infestationChance += 0.10;
	}

	if( infestationChance >= Math.random())
	{
		myPlant.SetTag( "infestation", infestsationLevel + 1 );
	}

	// Check if the plant gets infected with fungus
	var fungusChance = 0.15 - greaterStrength * 0.075 + (waterLevel - 2) * 0.10

	if (fungusChance >= Math.random()) 
	{
		myPlant.SetTag( "fungus", fungusLevel + 1 );
	}

	if ( waterLevel > 0 && waterLevel >= 2 || 0.9 >= Math.random())
	{
		myPlant.SetTag("water", waterLevel - 1);
	}

	//Too many poison and cure potions will cause disease and poison
	if (greaterPoison > 0)
	{
		myPlant.SetTag("GreaterPoison", greaterPoison - 1);
		myPlant.SetTag("poison", poisonLevel + greaterPoison);
	}

	if (greaterCure > 0) 
	{
		myPlant.SetTag("GreaterCure", greaterCure - 1);
		myPlant.SetTag("disease", diseaseLevel + greaterCure);
	}

	if (greaterStrength > 0)
	{
		myPlant.SetTag("GreaterStrength", greaterStrength - 1);
	}

	return;
}

function ApplyPotions(myPlant)
{
	var waterLevel = myPlant.GetTag("water");
	var infestsationLevel = myPlant.GetTag("infestation");
	var fungusLevel = myPlant.GetTag("fungus");
	var poisonLevel = myPlant.GetTag("poison");
	var diseaseLevel = myPlant.GetTag("disease");

	var greaterPoison = myPlant.GetTag("GreaterPoison");
	var greaterCure = myPlant.GetTag("GreaterCure");
	var greaterHeal = myPlant.GetTag("GreaterHeal");
	var greaterStrength = myPlant.GetTag("GreaterStrength");

	//Poison potions kill off infestations
	if (greaterPoison >= infestsationLevel) 
	{
		myPlant.SetTag("GreaterPoison", greaterPoison - infestsationLevel);
		myPlant.SetTag("infestation", 0);
	}
	else
	{
		myPlant.SetTag("GreaterPoison", 0);
		myPlant.SetTag("infestation", infestsationLevel - greaterPoison);
	}

	//cure potions kill off fungus
	if (greaterCure >= fungusLevel)
	{
		myPlant.SetTag("GreaterCure", greaterCure - fungusLevel);
		myPlant.SetTag("fungus", 0);
	}
	else 
	{
		myPlant.SetTag("GreaterCure", 0);
		myPlant.SetTag("fungus", fungusLevel - greaterCure);
	}

	//heal potions kill off poison
	if (greaterHeal >= diseaseLevel) 
	{
		myPlant.SetTag("GreaterHeal", greaterHeal - poisonLevel);
		myPlant.SetTag("poison", 0);
	}
	else
	{
		myPlant.SetTag("GreaterHeal", 0);
		myPlant.SetTag("poison", poisonLevel - greaterHeal);
	}

	//heal potions kill off disease
	if (greaterHeal >= diseaseLevel)
	{
		myPlant.SetTag("GreaterHeal", greaterHeal - diseaseLevel);
		myPlant.SetTag("disease", 0);
	}
	else 
	{
		myPlant.SetTag("GreaterHeal", 0);
		myPlant.SetTag("disease", diseaseLevel - greaterHeal);
	}

	if (infestsationLevel == 0 || fungusLevel == 0 || poisonLevel == 0 || diseaseLevel == 0 || waterLevel != 2)
	{
		if (greaterHeal > 0) 
		{
			myPlant.health = myPlant.health + greaterHeal * 7;
		}
		else
			myPlant.health = myPlant.health + 2;
	}

	if (greaterHeal > 0) 
	{
		myPlant.SetTag("GreaterHeal", greaterHeal - 1);
	}
}

function onTimer( myPlant, timerID )
{
	if ( !ValidateObject( myPlant ))
		return;

	var stage = myPlant.GetTag("PlantStage");//Starts at stage 1
	var Seeds = myPlant.GetTag("Seeds")
	var CrossedPlants = myPlant.GetTag("PlantCross")

	if (!CrossedPlants || !Seeds)
		return false;

	var Crossed = CrossedPlants.split(",");
	var Seedlength = Seeds.split(",");

	if (Crossed.length != 3 || Seedlength.length != 3)
		return false;

	var Pollinated = parseInt(Crossed[0]);
	var SeedBreed = parseInt(Crossed[1]);
	var crossAble = parseInt(Crossed[2]);
	var availableSeeds = parseInt(Seedlength[0]);
	var remainingSeeds = parseInt(Seedlength[1]);
	var hueSeeds = parseInt(Seedlength[2]);

	if (timerID == 1 )
	{
		if (!inValidLocation(myPlant)) // Checks make sure the plant is lockdown
		{
			return;
		}
		else 
		{
			if (myPlant.GetTag("PlantHealth") != 10 && myPlant.GetTag("PlantHealth") != 11)//wilted or dying
			{
				if (myPlant.GetTag("FertialeDirt") && stage <= 5)
				{
					//double growth to stage 5
					myPlant.SetTag("PlantStage", stage + 2);
				}
				else if (stage < 9) 
				{
					// Continue to the next stage if it does not have fertial dirt
					myPlant.SetTag("PlantStage", stage + 1);
				}
				else 
				{
					//Produce Seeds 
					if (remainingSeeds > 0 && Pollinated == 1)
					{
						var rseeds = remainingSeeds - 1;
						var aseeds = availableSeeds + 1;
						myPlant.SetTag("Seeds", aseeds + "," + rseeds + "," + hueSeeds);
					}					
				}

				if (stage >= 2 && stage <= 3)
				{
					myPlant.id = 0x1600;
				}
				else if (stage == 9)
				{
					PlantBowl(myPlant);
				}
			}

			if (stage >= 9)
			{
				if (Pollinated == 0)
				{
					myPlant.SetTag("PlantCross", 1 + "," + SeedBreed + "," + crossAble);
				}

			}

			ApplyPotions(myPlant)
			HealthStatus(myPlant);
			//PlantDamage(myPlant);
			GrowthCheck(myPlant);
			myPlant.Refresh();
		}
	}
	myPlant.StartTimer(PlantDelayTimer, 1, scriptID);
}

function DrawPlant( PlantGump, iUsed )
{
	if( iUsed.GetTag( "PlantStage" ) >= 0 && iUsed.GetTag( "PlantStage") < 9 )
	{
		PlantGump.AddGump( 110, 85, 0x589 );

		PlantGump.AddPicture( 122, 94, 0x914 );
		PlantGump.AddPicture( 135, 94, 0x914 );
		PlantGump.AddPicture( 120, 112, 0x914 );
		PlantGump.AddPicture( 135, 112, 0x914 );

		if( iUsed.GetTag( "PlantStage") >= 2 )
		{
			PlantGump.AddPicture( 127, 112, 0xC62 );
		}
		if( iUsed.GetTag( "PlantStage" ) == 3 || iUsed.GetTag( "PlantStage" ) == 4 )
		{
			PlantGump.AddPicture(129, 85, 0xC7E);
		}
		if( iUsed.GetTag( "PlantStage" ) >= 4 )
		{
			PlantGump.AddPicture( 121, 117, 0xC62 );
			PlantGump.AddPicture( 133, 117, 0xC62 );
		}
		if( iUsed.GetTag("PlantStage") >= 5)
		{
			PlantGump.AddPicture( 110, 100, 0xC62 );
			PlantGump.AddPicture( 140, 100, 0xC62 );
			PlantGump.AddPicture( 110, 130, 0xC62 );
			PlantGump.AddPicture( 140, 130, 0xC62 );
		}
		if( iUsed.GetTag( "PlantStage" ) > 6 )
		{
			PlantGump.AddPicture( 105, 115, 0xC62 );
			PlantGump.AddPicture( 145, 115, 0xC62 );
			PlantGump.AddPicture( 125, 90, 0xC62 );
			PlantGump.AddPicture( 125, 135, 0xC62 );
		}
	}
	else
	{
		  PlantGump( PlantGump, iUsed );
	}

	if( iUsed.GetTag( "PlantStage" ) != 14 ) //BowlOfDirt
	{
		switch( iUsed.GetTag( "PlantHealth" ))
		{
			case 10:
					PlantGump.AddPicture( 92, 167, 0x1B9D );
					PlantGump.AddPicture( 161, 167, 0x1B9D );
					PlantGump.AddHTMLGump( 136, 167, 42, 20, false, false, "<basefont color=#00FC00>" + "dying" + "</basefont>" );
					break;// Dying
			case 11:
					PlantGump.AddPicture( 91, 164, 0x18E6 );
					PlantGump.AddPicture( 161, 164, 0x18E6 );
					PlantGump.AddHTMLGump( 132, 167, 42, 20, false, false, "<basefont color=#00C207>" + "Wilted" + "</basefont>" );
					break;// Wilted
			case 12:
					PlantGump.AddPicture( 96, 168, 0xC61 );
					PlantGump.AddPicture( 162, 168, 0xC61 );
					PlantGump.AddHTMLGump( 129, 167, 42, 20, false, false, "<basefont color=#008200>" + "Healthy" + "</basefont>" );
					break;// Healthy
			case 13:
					PlantGump.AddPicture( 93, 162, 0x1A99 );
					PlantGump.AddPicture( 162, 162, 0x1A99 );
					PlantGump.AddHTMLGump( 129, 167, 42, 20, false, false, "<basefont color=#0083E0>" + "Vibrant" + "</basefont>" );
					break;// Vibrant
		}
	}
	return;
}

function PlantGump( PlantGump, iUsed )
{
	var plantInfo = iUsed.GetTag("PlantInfo")

	if (!plantInfo) {
		return false;
	}

	var infoLength = plantInfo.split(",");
	if (infoLength.length != 4)
		return false;

	var plantType = parseInt(infoLength[0]);
	var PlantName = parseInt(infoLength[1]);
	var plantColor = parseInt(infoLength[2]);
	var fertialeDirt = parseInt(infoLength[3]);

	var plantArray = {
		"3203": [0, 0], // 0x0c83
		"3206": [0, 0], // 0x0c86
		"3208": [0, 0], // 0x0c88
		"3220": [-15, 0],  // 0xC94
		"3211": [0, 0], // 0xC8B
		"3237": [-8, 0], // 0xCA5
		"3239": [-10, 0], // 0xCA7
		"3223": [-20, 0], // 0xC97
		"3231": [-20, 0], // 0xC9F
		"3238": [-16, -5], // 0xCA6
		"3228": [-5, -10], // 0xC9C
		"3377": [0, -27], // 0xD31
		"3332": [0, 10], // 0xD04
		"3241": [0, 0], // 0xCA9
		"3372": [0, 10], // 0xD2C
		"3366": [0, 10], // 0xD26
		"3367": [0, 10], // 0xD27
		"10460": [-5, 5], //0x28DC
		"10463": [-5, 5], // 0x28DF
		"10461": [-5, 5], // 0x28DD
		"10464": [-5, 5], // 0x28E0
		"10462": [-5, 5], // 0x28DE
		"10465": [-5, 5], // 0x28E1
		"10466": [-5, 5], // 0x28E2
		"10467": [-5, 5], // 0x28E3
		"3365": [0, 0], // 0x0D25
		"6810": [5, 10], // 0x1A9A
		"3204": [0, 0], // 0x0C84
		"6815": [5, 25], // 0x1A9F
		"3265": [0, 0], // 0x0CC1
		"3326": [-45, -35], // 0x0CFE
		"3215": [0, 0], // 0x0C8F
		"3272": [0, 0], // 0x0CC8
		"3214": [-20, 0], // 0x0C8E
		"3255": [0, 0], // 0x0CB7
		"3262": [-20, 0], // 0x0CBE
		"3273": [0, 0], // 0x0CC9
		"3521": [-15, 15], // 0x0DC1
		"3323": [-45, -30], // 0x0CFB
		"3512": [0, -20], // 0x0DB8
		"6817": [10, -25], // 0x1AA1
		"9324": [-25, -20] // 0x246C
	};

	var CampionFlowers = 3203;
	var Poppies = 3206;
	var Snowdrops = 3208;
	var Bulrushes = 3220;
	var Lilies = 3211;
	var PampasGrass = 3237;
	var Rushes = 3239;
	var ElephantEarPlant = 3223;
	var Fern = 3231;
	var PonytailPalm = 3238;
	var SmallPalm = 3228;
	var CenturyPlant = 3377;
	var WaterPlant = 3332;
	var SnakePlant = 3241;
	var PricklyPearCactus = 3372;
	var BarrelCactus = 3366;
	var TribarrelCactus = 3367;
	var CommonGreenBonsai = 10460;
	var CommonPinkBonsai = 10463;
	var UncommonGreenBonsai = 10461;
	var UncommonPinkBonsai = 10464;
	var RareGreenBonsai = 10462;
	var RarePinkBonsai = 10465;
	var ExceptionalBonsai = 10466;
	var ExoticBonsai = 10467;
	var Cactus = 3365;
	var FlaxFlowers = 6810;
	var FoxgloveFlowers = 3204;
	var HopsEast = 6815;
	var OrfluerFlowers = 3265;
	var CypressTwisted = 3326;
	var HedgeShort = 3215;
	var JuniperBush = 3272;
	var SnowdropPatch = 3214;
	var Cattails = 3255;
	var PoppyPatch = 3262;
	var SpiderTree = 3273;
	var WaterLily = 3521;
	var CypressStraight = 3323;
	var HedgeTall = 3512;
	var HopsSouth = 6817;
	var SugarCanes = 9324;

	var plantID = 0;
	var plantName = "";
	switch (plantType)
	{
		case 1:
			plantID = CampionFlowers;
			plantName = "Campion Flowers";
			break;
		case 2:
			plantID = Poppies;
			plantName = "Poppies";
			break;
		case 3:
			plantID = Snowdrops;
			plantName = "Snowdrops";
			break;
		case 4:
			plantID = Bulrushes;
			plantName = "Bulrushes";
			break;
		case 5:
			plantID = Lilies;
			plantName = "Lilies";
			break;
		case 6:
			plantID = PampasGrass;
			plantName = "Pampas Grass";
			break;
		case 7:
			plantID = Rushes;
			plantName = "Rushes";
			break;
		case 8:
			plantID = ElephantEarPlant;
			plantName = "Elephant Ear Plant";
			break;
		case 9:
			plantID = Fern;
			plantName = "Fern";
			break;
		case 10:
			plantID = PonytailPalm;
			plantName = "Ponytail Palm";
			break;
		case 11:
			plantID = SmallPalm;
			plantName = "Small Palm";
			break;
		case 12:
			plantID = CenturyPlant;
			plantName = "Century Plant";
			break;
		case 13:
			plantID = WaterPlant;
			plantName = "Water Plant";
			break;
		case 14:
			plantID = SnakePlant;
			plantName = "Snake Plant";
			break;
		case 15:
			plantID = PricklyPearCactus;
			plantName = "Prickly Pear Cactus";
			break;
		case 16:
			plantID = BarrelCactus;
			plantName = "Barrel Cactus";
			break;
		case 17:
			plantID = TribarrelCactus;
			plantName = "Tribarrel Cactus";
			break;
		case 18:
			plantID = CommonGreenBonsai;
			plantName = "Common Green Bonsai";
			break;
		case 19:
			plantID = CommonPinkBonsai;
			plantName = "Common Pink Bonsai";
			break;
		case 20:
			plantID = UncommonGreenBonsai;
			plantName = "Uncommon Green Bonsai";
			break;
		case 21:
			plantID = UncommonPinkBonsai;
			plantName = "Uncommon Pink Bonsai";
			break;
		case 22:
			plantID = RareGreenBonsai;
			plantName = "Rare Green Bonsai";
			break;
		case 23:
			plantID = RarePinkBonsai;
			plantName = "Rare Pink Bonsai";
			break;
		case 24:
			plantID = ExceptionalBonsai;
			plantName = "Exceptional Bonsai";
			break;
		case 25:
			plantID = ExoticBonsai;
			plantName = "Exotic Bonsai";
			break;
		case 26:
			plantID = Cactus;
			plantName = "Cactus";
			break;
		case 27:
			plantID = FlaxFlowers;
			plantName = "Flax Flowers";
			break;
		case 28:
			plantID = FoxgloveFlowers;
			plantName = "Foxglove Flowers";
			break;
		case 29:
			plantID = HopsEast;
			plantName = "Hops East";
			break;
		case 30:
			plantID = OrfluerFlowers;
			plantName = "Orfluer Flowers";
			break;
		case 31:
			plantID = CypressTwisted;
			plantName = "Cypress Twisted";
			break;
		case 32:
			plantID = HedgeShort;
			plantName = "Hedge Short";
			break;
		case 33:
			plantID = JuniperBush;
			plantName = "Juniper Bush";
			break;
		case 34:
			plantID = SnowdropPatch;
			plantName = "Snowdrop Patch";
			break;
		case 35:
			plantID = Cattails;
			plantName = "Cattails";
			break;
		case 36:
			plantID = PoppyPatch;
			plantName = "Poppy Patch";
			break;
		case 37:
			plantID = SpiderTree;
			plantName = "Spider Tree";
			break;
		case 38:
			plantID = WaterLily;
			plantName = "Water Lily";
			break;
		case 39:
			plantID = CypressStraight;
			plantName = "Cypress Straight";
			break;
		case 40:
			plantID = HedgeTall;
			plantName = "Hedge Tall";
			break;
		case 41:
			plantID = HopsSouth;
			plantName = "Hops South";
			break;
		case 42:
			plantID = SugarCanes;
			plantName = "Sugar Canes";
			break;
	}
	if ( plantID == CypressTwisted || plantID == CypressStraight)
	{ // The large images for these trees trigger a client crash, so use a smaller, generic tree for gump.
		PlantGump.AddPictureColor( 130 + plantArray[plantID][0], 96 + plantArray[plantID][1], 0x0CCA, plantColor );
	}
	else
	{
		PlantGump.AddPictureColor( 130 + plantArray[plantID][0], 96 + plantArray[plantID][1], plantID, plantColor );
	}
	iUsed.Refresh();
	return;
}

function PlantBowl( iUsed )
{
	var plantInfo = iUsed.GetTag("PlantInfo")

	if (!plantInfo) {
		return false;
	}

	var infoLength = plantInfo.split(",");
	if (infoLength.length != 4)
		return false;

	var plantType = parseInt(infoLength[0]);
	var PlantName = parseInt(infoLength[1]);
	var plantColor = parseInt(infoLength[2]);
	var fertialeDirt = parseInt(infoLength[3]);

	var CampionFlowers = 3203;
	var Poppies = 3206;
	var Snowdrops = 3208;
	var Bulrushes = 3220;
	var Lilies = 3211;
	var PampasGrass = 3237;
	var Rushes = 3239;
	var ElephantEarPlant = 3223;
	var Fern = 3231;
	var PonytailPalm = 3238;
	var SmallPalm = 3228;
	var CenturyPlant = 3377;
	var WaterPlant = 3332;
	var SnakePlant = 3241;
	var PricklyPearCactus = 3372;
	var BarrelCactus = 3366;
	var TribarrelCactus = 3367;
	var CommonGreenBonsai = 10460;
	var CommonPinkBonsai = 10463;
	var UncommonGreenBonsai = 10461;
	var UncommonPinkBonsai = 10464;
	var RareGreenBonsai = 10462;
	var RarePinkBonsai = 10465;
	var ExceptionalBonsai = 10466;
	var ExoticBonsai = 10467;
	var Cactus = 3365;
	var FlaxFlowers = 6810;
	var FoxgloveFlowers = 3204;
	var HopsEast = 6815;
	var OrfluerFlowers = 3265;
	var CypressTwisted = 3326;
	var HedgeShort = 3215;
	var JuniperBush = 3272;
	var SnowdropPatch = 3214;
	var Cattails = 3255;
	var PoppyPatch = 3262;
	var SpiderTree = 3273;
	var WaterLily = 3521;
	var CypressStraight = 3323;
	var HedgeTall = 3512;
	var HopsSouth = 6817;
	var SugarCanes = 9324;

	var plantID = 0;
	var plantName = "";
	switch(plantType)
	{
		case 1:
			plantID = CampionFlowers;
			plantName = "Campion Flowers";
			break;
		case 2:
			plantID = Poppies;
			plantName = "Poppies";
			break;
		case 3:
			plantID = Snowdrops;
			plantName = "Snowdrops";
			break;
		case 4:
			plantID = Bulrushes;
			plantName = "Bulrushes";
			break;
		case 5:
			plantID = Lilies;
			plantName = "Lilies";
			break;
		case 6:
			plantID = PampasGrass;
			plantName = "Pampas Grass";
			break;
		case 7:
			plantID = Rushes;
			plantName = "Rushes";
			break;
		case 8:
			plantID = ElephantEarPlant;
			plantName = "Elephant Ear Plant";
			break;
		case 9:
			plantID = Fern;
			plantName = "Fern";
			break;
		case 10:
			plantID = PonytailPalm;
			plantName = "Ponytail Palm";
			break;
		case 11:
			plantID = SmallPalm;
			plantName = "Small Palm";
			break;
		case 12:
			plantID = CenturyPlant;
			plantName = "Century Plant";
			break;
		case 13:
			plantID = WaterPlant;
			plantName = "Water Plant";
			break;
		case 14:
			plantID = SnakePlant;
			plantName = "Snake Plant";
			break;
		case 15:
			plantID = PricklyPearCactus;
			plantName = "Prickly Pear Cactus";
			break;
		case 16:
			plantID = BarrelCactus;
			plantName = "Barrel Cactus";
			break;
		case 17:
			plantID = TribarrelCactus;
			plantName = "Tribarrel Cactus";
			break;
		case 18:
			plantID = CommonGreenBonsai;
			plantName = "Common Green Bonsai";
			break;
		case 19:
			plantID = CommonPinkBonsai;
			plantName = "Common Pink Bonsai";
			break;
		case 20:
			plantID = UncommonGreenBonsai;
			plantName = "Uncommon Green Bonsai";
			break;
		case 21:
			plantID = UncommonPinkBonsai;
			plantName = "Uncommon Pink Bonsai";
			break;
		case 22:
			plantID = RareGreenBonsai;
			plantName = "Rare Green Bonsai";
			break;
		case 23:
			plantID = RarePinkBonsai;
			plantName = "Rare Pink Bonsai";
			break;
		case 24:
			plantID = ExceptionalBonsai;
			plantName = "Exceptional Bonsai";
			break;
		case 25:
			plantID = ExoticBonsai;
			plantName = "Exotic Bonsai";
			break;
		case 26:
			plantID = Cactus;
			plantName = "Cactus";
			break;
		case 27:
			plantID = FlaxFlowers;
			plantName = "Flax Flowers";
			break;
		case 28:
			plantID = FoxgloveFlowers;
			plantName = "Foxglove Flowers";
			break;
		case 29:
			plantID = HopsEast;
			plantName = "Hops East";
			break;
		case 30:
			plantID = OrfluerFlowers;
			plantName = "Orfluer Flowers";
			break;
		case 31:
			plantID = CypressTwisted;
			plantName = "Cypress Twisted";
			break;
		case 32:
			plantID = HedgeShort;
			plantName = "Hedge Short";
			break;
		case 33:
			plantID = JuniperBush;
			plantName = "Juniper Bush";
			break;
		case 34:
			plantID = SnowdropPatch;
			plantName = "Snowdrop Patch";
			break;
		case 35:
			plantID = Cattails;
			plantName = "Cattails";
			break;
		case 36:
			plantID = PoppyPatch;
			plantName = "Poppy Patch";
			break;
		case 37:
			plantID = SpiderTree;
			plantName = "Spider Tree";
			break;
		case 38:
			plantID = WaterLily;
			plantName = "Water Lily";
			break;
		case 39:
			plantID = CypressStraight;
			plantName = "Cypress Straight";
			break;
		case 40:
			plantID = HedgeTall;
			plantName = "Hedge Tall";
			break;
		case 41:
			plantID = HopsSouth;
			plantName = "Hops South";
			break;
		case 42:
			plantID = SugarCanes;
			plantName = "Sugar Canes";
			break;
	}

	iUsed.id = plantID;
	iUsed.name = plantName;
	iUsed.movable = 1;
	iUsed.colour = plantColor;
	iUsed.Refresh();
}

function CodexOFWisdomPacket(socket, topicID)
{
	var helpPacket = new Packet; // Create new packet stream
	helpPacket.ReserveSize( 11 ); // Reserve packet size of 11, which is optimal for packet 0xBF in this case
	helpPacket.WriteByte( 0, 0xBF ); // Write packetID (0xBF) at position 0
	helpPacket.WriteShort( 1, 11 ); // Write total packet length at position 1 (0+WriteByte, or 0+1)
	helpPacket.WriteShort( 3, 0x17 ); // Write subcommand 0x17 (Codex of Wisdom) at position 3 (1+WriteShort, or 0+2)
	helpPacket.WriteByte( 5, 0x01 ); // Write unknown 0x01 at position 5 (3+WriteShort, or 3+2)
	helpPacket.WriteLong( 6, topicID); // Write topicID (???) at position 6 (5+WriteByte, or 5+2)
	helpPacket.WriteByte( 10, 1 ); // Write 0 or 1 at position 10 (6+WriteLong, or 6+4)
	socket.Send( helpPacket );
	helpPacket.Free();
}

function onGumpPress(socket, button, PlantGump)
{
	var pUser = socket.currentChar;
	var iUsed = socket.tempObj;
	var gumpID = scriptID + 0xffff;
	switch( button )
	{
		case 0:
			socket.CloseGump( gumpID, 0 );
			break;// abort and do nothing
		case 1:// Reproduction menu
			if (iUsed.GetTag("PlantStage") >= 1)
			{
				ReproductionGump(pUser, iUsed);
			}
			else
			{
				pUser.SysMessage("You need to plant a seed in the bowl first");
				onUseChecked(pUser, iUsed);
			}
			break;
		case 2:// infestation
			onUseChecked(pUser, iUsed);
			CodexOFWisdomPacket(socket, 54);
			break;
		case 3:// fungus
			onUseChecked(pUser, iUsed);
			CodexOFWisdomPacket(socket, 56);
			break;
		case 4:// poison
			onUseChecked(pUser, iUsed);
			CodexOFWisdomPacket(socket, 58);
			break;
		case 5:// disease
			onUseChecked(pUser, iUsed);
			CodexOFWisdomPacket(socket, 60);
			break;
		case 6:// water
			onUseChecked(pUser, iUsed);
			break;
		case 7:// poison potion
			addPotion(pUser, iUsed, 7);
			onUseChecked(pUser, iUsed);
			break;
		case 8:// Cure potion
			addPotion(pUser, iUsed, 8);
			onUseChecked(pUser, iUsed);
			break;
		case 9:// Heal potion
			addPotion(pUser, iUsed, 9);
			onUseChecked(pUser, iUsed);
			break;
		case 10:// Strength potion
			addPotion(pUser, iUsed, 10);
			onUseChecked(pUser, iUsed);
			break;
		case 11:// Help
			onUseChecked(pUser, iUsed);
			CodexOFWisdomPacket(socket, 48);
			break;
		case 12:// Empty the bowl
			socket.CloseGump(gumpID, 0);
			EmptyBowlGump(pUser, iUsed);
            //TriggerEvent( 5040, "EmptyBowl", pUser, iUsed );
			break;
		case 13:// Empty Bowl Help
			EmptyBowlGump(pUser, iUsed);
			CodexOFWisdomPacket(socket, 71);
			break;
		case 14:// Cancel Plant Bowl Gump
			onUseChecked(pUser, iUsed);
			break;
		case 15:// Okay Empty Bowl
			iUsed.KillTimers();
			ResetPlant(iUsed);
			CreateDFNItem(pUser.socket, pUser, "emptyplantbowl", 1, "ITEM", true, 0);
			iUsed.Delete();
			break;
		case 16:// Main Plant Gump
			onUseChecked(pUser, iUsed);
			break;
		case 17:// Set to decorative
			SetToDecorativeGump(pUser, iUsed);
			break;
		case 18:// Pollination
			CodexOFWisdomPacket(socket, 67);
			break;
		case 19:// Resources
			CodexOFWisdomPacket(socket, 69);
			break;
		case 20:// Seeds
			CodexOFWisdomPacket(socket, 68);
			break;
		case 21:// Gather pollen
			PollinatePlant(pUser, iUsed);
			onUseChecked(pUser, iUsed);
			break;
		case 22:// Gather Resources
			onUseChecked(pUser, iUsed);
			break;
		case 23:// Gather seeds
			GatherSeeds(pUser, iUsed);
			break;
		case 24:// Cancel set decorative
			ReproductionGump(pUser, iUsed);
			break;
		case 25:
			CodexOFWisdomPacket(socket, 70);
			break;
		case 26:
			iUsed.KillTimers();
			ResetPlant(iUsed);
			iUsed.RemoveScriptTrigger(scriptID);
			socket.CloseGump(gumpID, 0);
			socket.SysMessage("You prune the plant. This plant will no longer produce resources or seeds, but will require no upkeep.");
			iUsed.Refresh();
			break;
	default:		break;
	}
}

function ResetPlant(iUsed)
{
	iUsed.SetTag("infestation", null);
	iUsed.SetTag("fungus", null);
	iUsed.SetTag("poison", null);
	iUsed.SetTag("disease", null);
	iUsed.SetTag("water", null);
	iUsed.SetTag("PlantType", null);
	iUsed.SetTag("PlantStage", null);
	iUsed.SetTag("PlantColor", null);
	iUsed.SetTag("PlantHealth", null);
	iUsed.SetTag("Seeds", null);
	iUsed.SetTag("PlantCross", null);
	iUsed.SetTag("GreaterPoison", null);
	iUsed.SetTag("GreaterCure", null);
	iUsed.SetTag("GreaterHeal", null);
	iUsed.SetTag("GreaterStrength", null);
}

function PollinatePlant(pUser, iUsed)
{
	pUser.socket.tempObj = iUsed;
	var socket = pUser.socket;
	var status = iUsed.GetTag("PlantStage");
	var CrossedPlants = iUsed.GetTag("PlantCross")

	if (!CrossedPlants)
	{
		pSock.SysMessage("You cannot gather pollen from a mutated plant!");
		return false;
	}
	else if (status < 7)
	{
		socket.SysMessage("Too early to gather pollen");
		return false;
	}
	else if (iUsed.GetTag("PlantHealth") == 10 && iUsed.GetTag("PlantHealth") == 11)//wilted or dying
	{
		socket.SysMessage("You cannot gather pollen from an unhealthy plant!");
		return false;
	}
	else
		pUser.CustomTarget(1, "Target the plant you wish to cross-pollinate to.");
}

function onCallback1(pSock, myTarget)
{
	var iUsed = pSock.tempObj;
	var pUser = pSock.currentChar;
	var status = iUsed.GetTag("PlantStage");
	var iCrossedPlants = iUsed.GetTag("PlantCross")

	if (!iCrossedPlants)
	{
		pSock.SysMessage("You cannot gather pollen from a mutated plant!");
		return false;
	}
	if (status < 7) 
	{
		pSock.SysMessage("Too early to gather pollen");
		return false;
	}
	else if (iUsed.GetTag("PlantHealth") == 10 && iUsed.GetTag("PlantHealth") == 11)//wilted or dying
	{
		pSock.SysMessage("You cannot gather pollen from an unhealthy plant!");
		return false;
	}
	else if (status > 7 && status < 9) 
	{
		pSock.SysMessage("You can only pollinate other specially grown plants!");
		return false;
	}
	else
	{
		var tstatus = myTarget.GetTag("PlantStage");
		var tCrossedPlants = myTarget.GetTag("PlantCross")

		if (!tCrossedPlants)
		{
			pSock.SysMessage("You cannot gather pollen from a mutated plant!");
			return false;
		}

		var Crossed = tCrossedPlants.split(",");
		if (Crossed.length != 3)
			return false;

		var Pollinated = parseInt(Crossed[0]);
		var SeedBreed = parseInt(Crossed[1]);
		var crossAble = parseInt(Crossed[2]);

		if (tstatus < 7)
		{
			pSock.SysMessage("This plant is not in the flowering stage. You cannot pollinate it!");
			return false;
		}
		else if (myTarget.GetTag("PlantHealth") == 10 && myTarget.GetTag("PlantHealth") == 11)//wilted or dying
		{
			pSock.SysMessage("You cannot pollinate an unhealthy plant!");
			return false;
		}
		else if (tstatus > 7 && tstatus < 9)
		{
			pSock.SysMessage("You can only pollinate other specially grown plants!");
			return false;
		}
		else if (crossAble == 0) 
		{
			pSock.SysMessage("You cannot cross-pollinate with a mutated plant!");
			return false;
		}
		else if (Pollinated == 1) 
		{
			pSock.SysMessage("This plant has already been pollinated!");
		}
		else if (myTarget == iUsed)
		{
			CrossPollinateTable(myTarget, iUsed, pSock);
			SeedColorsSet(myTarget, iUsed);
			pSock.SysMessage("You pollinate the plant with its own pollen.");
		}
		else
		{
			CrossPollinateTable(myTarget, iUsed, pSock);
			SeedColorsSet(myTarget, iUsed);
			pSock.SysMessage("You successfully cross - pollinate the plant.");
		}
	}
}

function CrossPollinateTable(myTarget, iUsed, pSock)
{
	var iinfo = iUsed.GetTag("PlantInfo")
	var tinfo = myTarget.GetTag("PlantInfo");

	if (!iinfo || !tinfo) 
	{
		return false;
	}

	var icrossLength = iinfo.split(",");
	var tcrossLength = tinfo.split(",");
	if (icrossLength.length != 4 || tcrossLength.length != 4)
		return false;

	var cross = parseInt(icrossLength[0]);
	var cross2 = parseInt(tcrossLength[0]);

	var setcross = 0;

	if (cross == 1 && cross2 == 1)
		setcross = 1; // CampionFlowerSeed

	if (cross == 2 && cross2 == 2)//Poppies + Poppies
		setcross = 2;// PoppieSeed

	if (cross == 1 && cross2 == 3)//Campion Flowers + Snowdrops
		setcross = 2;// PoppieSeed

	if (cross == 3 && cross2 == 3)//Snowdrops + Snowdrops
		setcross = 3;//SnowdropSeed

	if (cross == 2 && cross2 == 4)//Poppies + Bulrushes
		setcross = 3;//SnowdropSeed

	if (cross == 1 && cross2 == 5)//Campion Flowers + Lilies
		setcross = 3;//SnowdropSeed

	if (cross == 4 && cross2 == 4)//Bulrushes + Bulrushes
		setcross = 4;//BulrusheSeed

	if (cross == 3 && cross2 == 5)//Snowdrops + Lilies
		setcross = 4;//BulrusheSeed

	if (cross == 2 && cross2 == 6)//Poppies + Pampas Grass
		setcross = 4;//BulrusheSeed

	if (cross == 1 && cross2 == 7)//Campion Flowers + Rushes
		setcross = 4;//BulrusheSeed

	if (cross == 5 && cross2 == 5)//Lilies + Lilies
		setcross = 5;//LilieSeed

	if (cross == 4 && cross2 == 6)//Bulrushes + Pampas Grass
		setcross = 5;//LilieSeed

	if (cross == 3 && cross2 == 7)//Snowdrops + Rushes
		setcross = 5;//LilieSeed

	if (cross == 2 && cross2 == 8)//Poppies + Elephant Ear Plant
		setcross = 5;//LilieSeed

	if (cross == 1 && cross2 == 9)//Campion Flowers + Fern
		setcross = 5;//LilieSeed

	if (cross == 6 && cross2 == 6)//Pampas Grass + Pampas Grass
		setcross = 6;//PampasGrassSeed

	if (cross == 5 && cross2 == 7)//Lilies + Rushes
		setcross = 6;//PampasGrassSeed

	if (cross == 4 && cross2 == 8)//Bulrushes + Elephant Ear Plant
		setcross = 6;//PampasGrassSeed

	if (cross == 3 && cross2 == 9)//Snowdrops + Fern
		setcross = 6;//PampasGrassSeed

	if (cross == 2 && cross2 == 10)//Poppies + Ponytail Palm
		setcross = 6;//PampasGrassSeed

	if (cross == 1 && cross2 == 11)//Campion Flowers + Small Palm
		setcross = 6;//PampasGrassSeed

	if (cross == 7 && cross2 == 7)//Rushes + Rushes
		setcross = 7;//RusheSeed

	if (cross == 6 && cross2 == 8)//Pampas Grass + Elephant Ear Plant
		setcross = 7;//RusheSeed

	if (cross == 5 && cross2 == 9)//Lilies + Fern
		setcross = 7;//RusheSeed

	if (cross == 4 && cross2 == 10)//Bulrushes + Ponytail Palm
		setcross = 7;//RusheSeed

	if (cross == 3 && cross2 == 11)//Snowdrops + Small Palm
		setcross = 7;//RusheSeed

	if (cross == 2 && cross2 == 12)//Poppies + Century Plant
		setcross = 7;//RusheSeed

	if (cross == 1 && cross2 == 13)//Campion Flowers + Water Plants
		setcross = 7;//RusheSeed

	if (cross == 8 && cross2 == 8)//Elephant Ear Plant + Elephant Ear Plant
		setcross = 8;//ElephantEarPlantrSeed

	if (cross == 7 && cross2 == 9)//Rushes + Fern
		setcross = 8;//ElephantEarPlantrSeed

	if (cross == 6 && cross2 == 10)//Pampas Grass + Ponytail Palm
		setcross = 8;//ElephantEarPlantrSeed

	if (cross == 5 && cross2 == 11)//Lilies + Small Palm
		setcross = 8;//ElephantEarPlantrSeed

	if (cross == 4 && cross2 == 12)//Bulrushes + Century Plant
		setcross = 8;//ElephantEarPlantrSeed

	if (cross == 3 && cross2 == 13)//Snowdrops + Water Plants
		setcross = 8;//ElephantEarPlantrSeed

	if (cross == 2 && cross2 == 14)//Poppies + Snake Plant
		setcross = 8;//ElephantEarPlantrSeed

	if (cross == 1 && cross2 == 15)//Campion Flowers + Prickly Pear Cactus
		setcross = 8;//ElephantEarPlantrSeed

	if (cross == 9 && cross2 == 9)//Fern + Fern
		setcross = 9;//FernSeed

	if (cross == 8 && cross2 == 10)//Elephant Ear Plant + Ponytail Palm
		setcross = 9;//FernSeed

	if (cross == 7 && cross2 == 11)//Rushes + Small Palm
		setcross = 9;//FernSeed

	if (cross == 6 && cross2 == 12)//Pampas Grass + Century Plant
		setcross = 9;//FernSeed

	if (cross == 5 && cross2 == 13)//Lilies + Water Plants
		setcross = 9;//FernSeed

	if (cross == 4 && cross2 == 14)//Bulrushes + Snake Plant
		setcross = 9;//FernSeed

	if (cross == 3 && cross2 == 15)//Snowdrops + Prickly Pear Cactus
		setcross = 9;//FernSeed

	if (cross == 2 && cross2 == 16)//Poppies + Barrel Cactus
		setcross = 9;//FernSeed

	if (cross == 1 && cross2 == 17)//Campion Flowers + Tribarrel Cactus
		setcross = 9;//FernSeed

	if (cross == 10 && cross2 == 10)//Ponytail Palm + Ponytail Palm
		setcross = 10;//PonytailPalmSeed

	if (cross == 9 && cross2 == 11)//Fern + Small Palm
		setcross = 10;//PonytailPalmSeed

	if (cross == 8 && cross2 == 12)//Elephant Ear Plant + Century Plant
		setcross = 10;//PonytailPalmSeed

	if (cross == 7 && cross2 == 13)//Rushes + Water Plants
		setcross = 10;//PonytailPalmSeed

	if (cross == 6 && cross2 == 14)//Pampas Grass + Snake Plant
		setcross = 10;//PonytailPalmSeed

	if (cross == 5 && cross2 == 15)//Lilies + Prickly Pear Cactus
		setcross = 10;//PonytailPalmSeed

	if (cross == 4 && cross2 == 16)//Bulrushes + Barrel Cactus
		setcross = 10;//PonytailPalmSeed

	if (cross == 3 && cross2 == 17)//Snowdrops + Tribarrel Cactus
		setcross = 10;//PonytailPalmSeed

	if (cross == 11 && cross2 == 11)//Small Palm + Small Palm
		setcross = 11;//SmallPalmSeed

	if (cross == 10 && cross2 == 12)//Ponytail Palm + Century Plant
		setcross = 11;//SmallPalmSeed

	if (cross == 9 && cross2 == 13)//Fern + Water Plants
		setcross = 11;//SmallPalmSeed

	if (cross == 8 && cross2 == 14)//Elephant Ear Plant + Snake Plant
		setcross = 11;//SmallPalmSeed

	if (cross == 7 && cross2 == 15)//Rushes + Prickly Pear Cactus
		setcross = 11;//SmallPalmSeed

	if (cross == 6 && cross2 == 16)//Pampas Grass + Barrel Cactus
		setcross = 11;//SmallPalmSeed

	if (cross == 5 && cross2 == 17)//Lilies + Tribarrel Cactus
		setcross = 11;//SmallPalmSeed

	if (cross == 12 && cross2 == 12)//Century Plant + Century Plant
		setcross = 12;//CenturyPlantSeed

	if (cross == 11 && cross2 == 13)//Small Palm + Water Plants
		setcross = 12;//CenturyPlantSeed

	if (cross == 10 && cross2 == 14)//Ponytail Palm + Snake Plant
		setcross = 12;//CenturyPlantSeed

	if (cross == 9 && cross2 == 15)//Fern + Prickly Pear Cactus
		setcross = 12;//CenturyPlantSeed

	if (cross == 8 && cross2 == 16)//Elephant Ear Plant + Barrel Cactus
		setcross = 12;//CenturyPlantSeed

	if (cross == 7 && cross2 == 17)//Rushes + Tribarrel Cactus
		setcross = 12;//CenturyPlantSeed

	if (cross == 13 && cross2 == 13)//Water Plants + Water Plants
		setcross = 13;//WaterPlantSeed

	if (cross == 12 && cross2 == 14)//Century Plant + Snake Plant
		setcross = 13;//WaterPlantSeed

	if (cross == 11 && cross2 == 15)//Small Palm + Prickly Pear Cactus
		setcross = 13;//WaterPlantSeed

	if (cross == 10 && cross2 == 16)//Ponytail Palm + Barrel Cactus
		setcross = 13;//WaterPlantSeed

	if (cross == 9 && cross2 == 17)//Fern + Tribarrel Cactus
		setcross = 13;//WaterPlantSeed

	if (cross == 14 && cross2 == 14)//Snake Plant + Snake Plant
		setcross = 14;//SnakePlantSeed

	if (cross == 13 && cross2 == 15)//Water Plants + Prickly Pear Cactus
		setcross = 14;//SnakePlantSeed

	if (cross == 12 && cross2 == 16)//Century Plant + Barrel Cactus
		setcross = 14;//SnakePlantSeed

	if (cross == 11 && cross2 == 17)//Small Palm + Tribarrel Cactus
		setcross = 14;//SnakePlantSeed

	if (cross == 15 && cross2 == 15)//Prickly Pear Cactus + Prickly Pear Cactus
		setcross = 15;//PricklyPearCactusSeed

	if (cross == 14 && cross2 == 16)//Snake Plant + Barrel Cactus
		setcross = 15;//PricklyPearCactusSeed

	if (cross == 13 && cross2 == 17)//Water Plants + Tribarrel Cactus
		setcross = 15;//PricklyPearCactusSeed

	if (cross == 16 && cross2 == 16)//Barrel Cactus + Barrel Cactus
		setcross = 16;//BarrelCactusSeed

	if (cross == 15 && cross2 == 17)//Prickly Pear Cactus + Tribarrel Cactus
		setcross = 16;//BarrelCactusSeed

	if (cross == 17 && cross2 == 17)//Tribarrel Cactus + Tribarrel Cactus
		setcross = 17;//TribarrelCactusSeed

	//pSock.SysMessage("You Set the Seed " + setcross);
	myTarget.SetTag("PlantCross", 1 + "," + setcross + "," + 1);
}

function SeedColorsSet(myTarget, iUsed)
{
	var Seeds = iUsed.GetTag("Seeds");
	var plantInfo = iUsed.GetTag("PlantInfo")
	var tplantInfo = myTarget.GetTag("PlantInfo")

	if (!Seeds || !plantInfo || !tplantInfo)
		return false;

	var infoLength = plantInfo.split(",");
	var tinfoLength = tplantInfo.split(",");
	var Seedlength = Seeds.split(",");

	if (Seedlength.length != 3 || infoLength.length != 4 || tinfoLength.length != 4)
		return false;

	var iplantColor = parseInt(infoLength[2]);
	var tplantColor = parseInt(tinfoLength[2]);

	var availableSeeds = parseInt(Seedlength[0]);
	var remainingSeeds = parseInt(Seedlength[1]);
	var hueSeeds = parseInt(Seedlength[2]);

	// Define color combination object
	var colorMap = {
		"0_0": 0, // Plain + Plain

		"1645_1645": 33,    // Red + Red = Bright Red
		"1645_1341": 13,    // Red + Blue = Purple
		"1645_2213": 1135,  // Red + Yellow = Orange
		"1645_13": 1645,    // Red + Purple = Red
		"1645_1435": 1645,  // Red + Green = Red
		"1645_1135": 1645,  // Red + Orange = Red
		"1645_0": 0,        // Red + Plain = Plain

		"1341_1341": 5,     // Blue + Blue = Bright Blue
		"1341_1645": 13,    // Blue + Red = Purple
		"1341_2213": 1435,  // Blue + Yellow = Green
		"1341_13": 1341,    // Blue + Purple = Blue
		"1341_1435": 1341,  // Blue + Green = Blue
		"1341_1135": 1341,  // Blue + Orange = Blue
		"1341_0": 0,        // Blue + Plain = Plain

		"2213_2213": 56,    // Yellow + Yellow = Bright Yellow
		"2213_1645": 1135,  // Yellow + Red = Orange
		"2213_1341": 1435,  // Yellow + Blue = Green
		"2213_13": 2213,    // Yellow + Purple = Yellow
		"2213_1435": 2213,  // Yellow + Green = Yellow
		"2213_1135": 2213,  // Yellow + Orange = Yellow
		"2213_0": 0,        // Yellow + Plain = Plain

		"13_1645": 1645,    // Purple + Red = Red
		"13_1341": 1341,    // Purple + Blue = Blue
		"13_2213": 2213,    // Purple + Yellow = Yellow
		"13_13": 16,        // Purple + Purple = Bright Purple
		"13_1435": 1341,    // Purple + Green = Blue
		"13_1135": 1645,    // Purple + Orange = Red
		"13_0": 0,          // Purple + Plain = Plain

		"1435_1645": 1645,  // Green + Red = Red
		"1435_1341": 1341,  // Green + Blue = Blue
		"1435_2213": 2213,  // Green + Yellow = Yellow
		"1435_13": 1341,    // Green + Purple = Blue
		"1435_1435": 66,    // Green + Green = Bright Green
		"1435_1135": 2213,  // Green + Orange = Yellow
		"1435_0": 0,        // Green + Plain = Plain

		"1135_1645": 1645,  // Orange + Red = Red
		"1135_1341": 1341,  // Orange + Blue = Blue
		"1135_2213": 2213,  // Orange + Yellow = Yellow
		"1135_13": 1645,    // Orange + Purple = Red
		"1135_1435": 2213,  // Orange + Green = Yellow
		"1135_1135": 43,    // Orange + Orange = Bright Orange
		"1135_0": 0,        // Orange + Plain = Plain

		"1117_1117": 1117,  // Black + Black
		"1117_0": 1117,     // Black + plain

		"1153_1153": 1153,  // White + White
		"1153_0": 1153,     // Black + plain

		"1166_1166": 1166,  // Pink + Pink
		"1166_0": 1166,     // Pink + plain

		"1158_1158": 1158,  // Magenta + Magenta
		"1158_0": 1158,     // Magenta + plain

		"1173_1173": 1173,  // Aqua + Aqua
		"1173_0": 1173,     // Aqua + plain

		"1161_1161": 1173,  // FireRed + FireRed
		"1161_0": 1173,     // FireRed + plain
	};

	// Build key for the color combination
	var key = iplantColor.toString() + "_" + tplantColor.toString();

	// Get hue from the object
	var sethue = colorMap[key] || 0;

	myTarget.SetTag("Seeds", availableSeeds + "," + remainingSeeds + "," + sethue);
}


function GatherSeeds(pUser, iUsed)
{
	var socket = pUser.socket;

	var Seeds = iUsed.GetTag("Seeds")

	if (!Seeds) 
	{
		return false;
	}

	var Seedlength = Seeds.split(",");
	if (Seedlength.length != 3)
		return false;

	var availableSeeds = parseInt(Seedlength[0]);
	var remainingSeeds = parseInt(Seedlength[1]);
	var hueSeeds = parseInt(Seedlength[2]);

	var CrossedPlants = iUsed.GetTag("PlantCross").split( "," );
	var Pollinated = parseInt(CrossedPlants[0]);
	var SeedBreed = parseInt(CrossedPlants[1]);
	var crossAble = parseInt(Crossed[2]);

	if (availableSeeds == 0)
	{
		socket.SysMessage("This plant has no seeds to gather!");
	}
	else 
	{
		var seedType = "";
		switch (SeedBreed)
		{
			case 1:
				seedType = "CampionFlowerSeed";
				break;
			case 2:
				seedType = "PoppiesSeed";
				break;
			case 3:
				seedType = "SnowdropSeed";
				break;
			case 4:
				seedType = "BulrusheSeed";
				break;
			case 5:
				seedType = "LilieSeed";
				break;
			case 6:
				seedType = "PampasGrassSeed";
				break;
			case 7:
				seedType = "RusheSeed";
				break;
			case 8:
				seedType = "ElephantEarPlantrSeed";
				break;
			case 9:
				seedType = "FernSeed";
				break;
			case 10:
				seedType = "PonytailPalmSeed";
				break;
			case 11:
				seedType = "SmallPalmSeed";
				break;
			case 12:
				seedType = "CenturyPlantSeed";
				break;
			case 13:
				seedType = "WaterPlantSeed";
				break;
			case 14:
				seedType = "SnakePlantSeed";
				break;
			case 15:
				seedType = "PricklyPearCactusSeed";
				break;
			case 16:
				seedType = "BarrelCactusSeed";
				break;
			case 17:
				seedType = "TribarrelCactusSeed";
				break;
			case 18:
				seedType = "CommonGreenBonsaiSeed";
				break;
			case 19:
				seedType = "CommonPinkBonsaiSeed";
				break;
			case 20:
				seedType = "UncommonGreenBonsaiSeed";
				break;
			case 21:
				seedType = "UncommonPinkBonsaiSeed";
				break;
			case 22:
				seedType = "RareGreenBonsai";
				break;
			case 23:
				seedType = "RarePinkBonsai";
				break;
			case 24:
				seedType = "ExceptionalBonsai";
				break;
			case 25:
				seedType = "ExoticBonsai";
				break;
			case 26:
				seedType = "Cactus";
				break;
			case 27:
				seedType = "FlaxFlowers";
				break;
			case 28:
				seedType = "FoxgloveFlowers";
				break;
			case 29:
				seedType = "HopsEast";
				break;
			case 30:
				seedType = "OrfluerFlowers";
				break;
			case 31:
				seedType = "CypressTwisted";
				break;
			case 32:
				seedType = "HedgeShort";
				break;
			case 33:
				seedType = "JuniperBush";
				break;
			case 34:
				seedType = "SnowdropPatch";
				break;
			case 35:
				seedType = "Cattails";
				break;
			case 36:
				seedType = "PoppyPatch";
				break;
			case 37:
				seedType = "SpiderTree";
				break;
			case 38:
				seedType = "WaterLily";
				break;
			case 39:
				seedType = "CypressStraight";
				break;
			case 40:
				seedType = "HedgeTall";
				break;
			case 41:
				seedType = "HopsSouth";
				break;
			case 42:
				seedType = "SugarCanes";
				break;
		}
		if (seedType != null)
		{
			CreateDFNItem(pUser.socket, pUser, seedType, 1, "ITEM", true, hueSeeds);

			socket.SysMessage("You gather seeds from the plant.")

			if (availableSeeds > 0)
			{
				var rseeds = availableSeeds - 1;
				iUsed.SetTag("Seeds", rseeds + "," + remainingSeeds + "," + hueSeeds);
			}
		}
	}
	onUseChecked(pUser, iUsed);
}

function EmptyBowlGump(pUser, iUsed)
{
	var socket = pUser.socket;
	var gumpID = scriptID + 0xffff;
	var EmptyBowlGump = new Gump;
	socket.tempObj = iUsed;

	EmptyBowlGump.AddBackground(50, 50, 200, 150, 0xE10);

	EmptyBowlGump.AddPicture(45, 45, 0xCEF);
	EmptyBowlGump.AddPicture(45, 118, 0xCF0);

	EmptyBowlGump.AddPicture(211, 45, 0xCEB);
	EmptyBowlGump.AddPicture(211, 118, 0xCEC);

	EmptyBowlGump.AddText(90, 70, 0x44, "Empty the bowl?");

	EmptyBowlGump.AddPicture(90, 100, 0x1602);
	EmptyBowlGump.AddGump(140, 102, 0x15E1);
	EmptyBowlGump.AddPicture(160, 100, 0x15FD);

	if (iUsed.GetTag("PlantStage") != 14 && iUsed.GetTag("PlantStage") < 2 ) //BowlOfDirt
		EmptyBowlGump.AddPicture(156, 130, 0xDCF); // Seed

	EmptyBowlGump.AddButton(98, 150, 0x47E, 0x480, 14, 14, 14); // Cancel

	EmptyBowlGump.AddButton(138, 151, 0xD2, 0xD2, 13, 13, 13); // Help
	EmptyBowlGump.AddText(143, 151, 0x835, "?");

	EmptyBowlGump.AddButton(168, 150, 0x481, 0x483, 15, 15, 15); // Ok
    EmptyBowlGump.Send(socket);
	EmptyBowlGump.Free();
}

function addPotion(pUser, iUsed, button)
{
	var itemOwner = GetPackOwner(iUsed, 0);
	if (itemOwner == null || itemOwner != pUser)
	{
		pUser.SysMessage(GetDictionaryEntry(1763, pUser.socket.language)); //That item must be in your backpack before it can be used.
	}
	else
	{
		pUser.socket.tempObj = iUsed;
		pUser.SetTempTag("ButtonPushed", button);// 7 = poison, 8 = cure, 9 = heal, 10 = strength
		pUser.CustomTarget(0);
	}
}

function onCallback0(pSock, myTarget)
{
	var iUsed = pSock.tempObj;
	var pUser = pSock.currentChar;
	var buttonPushed = pUser.GetTempTag("ButtonPushed"); // 7 = poison, 8 = cure, 9 = heal, 10 = strength

	//Define maximum potion count
	var maxPotionCount = 4; // Change this value to the desired maximum potion count

	// Check if the target is an item
	if (myTarget.isItem)
	{
		// Check if the item is in the user's backpack
		var itemOwner = GetPackOwner(myTarget, 0);
		if (itemOwner !== pUser)
		{
			pUser.SysMessage(GetDictionaryEntry(1763, pSock.language)); //That item must be in your backpack before it can be used.
			return false;
		}

		// Define potion types
		var potionTypes = {
			"greaterstrengthpotion": { "type": "GreaterStrength", "button": 10 },
			"0x0F09-b": { "type": "GreaterStrength", "button": 10 },
			"greaterpoisonpotion": { "type": "GreaterPoison", "button": 7 },
			"0x0F0A-c": { "type": "GreaterPoison", "button": 7 },
			"greatercurepotion": { "type": "GreaterCure", "button": 8 },
			"0x0F07-c": { "type": "GreaterCure", "button": 8 },
			"greaterhealpotion": { "type": "GreaterHeal", "button": 9 },
			"0x0F0C-c": { "type": "GreaterHeal", "button": 9 }
		};

		// Check if the sectionID is a valid potion type
		var potionInfo = potionTypes[myTarget.sectionID];
		if (potionInfo) 
		{
			// Check if the button pushed matches the potion type
			if (buttonPushed === potionInfo.button)
			{
				// Increase potion count if within maximum limit
				var potionCount = iUsed.GetTag(potionInfo.type);
				if (potionCount < maxPotionCount)
				{
					iUsed.SetTag(potionInfo.type, potionCount >= 0 ? potionCount + 1 : 1);
				} 
				else
				{
					pUser.SysMessage("The plant is already soaked with this type of potion!");
					return false;
				}

				// Call onUseChecked
				onUseChecked(pUser, iUsed);

				// Decrease target amount or delete if amount is 1
				if (myTarget.amount > 1)
					myTarget.amount--;
				else
					myTarget.Delete();
			}
			else
			{
				pUser.SysMessage("You don't have any strong potions of that type in your pack.");
			}
		}
		else 
		{
			pUser.SysMessage("You don't have any strong potions of that type in your pack.");
		}
	}
	return false;
}

function ReproductionGump(pUser, iUsed)
{
	var socket = pUser.socket;
	var ReproductionGump = new Gump;
	socket.tempObj = iUsed;

	ReproductionGump.AddBackground(50, 50, 200, 150, 0xE10);

	ReproductionGump.AddGump(60, 90, 0xE17);
	ReproductionGump.AddGump(120, 90, 0xE17);

	ReproductionGump.AddGump(60, 145, 0xE17);
	ReproductionGump.AddGump(120, 145, 0xE17);

	ReproductionGump.AddPicture(45, 45, 0xCEF);
	ReproductionGump.AddPicture(45, 118, 0xCF0);

	ReproductionGump.AddPicture(211, 45, 0xCEB);
	ReproductionGump.AddPicture(211, 118, 0xCEC);

	ReproductionGump.AddButton(70, 67, 0xD4, 0xD4, 16, 16, 16); // Main menu
	ReproductionGump.AddPicture(57, 65, 0x1600);

	ReproductionGump.AddText(108, 67, 0x835, "Reproduction");

	if (iUsed.GetTag("PlantStage") == 9)
	{
		ReproductionGump.AddButton(212, 67, 0xD4, 0xD4, 17, 17, 17); // Set to decorative
		ReproductionGump.AddPicture(202, 68, 0xC61);
		ReproductionGump.AddText(216, 66, 0x21, "/");
	}

	ReproductionGump.AddButton(80, 116, 0xD4, 0xD4, 18, 18, 18); // Pollination
	ReproductionGump.AddPicture(66, 117, 0x1AA2);
	AddPollinationState(ReproductionGump, iUsed, 106, 116);

	ReproductionGump.AddButton(128, 116, 0xD4, 0xD4, 19, 19, 19); // Resources
	ReproductionGump.AddPicture(113, 120, 0x1021);
	//AddResourcesState(ReproductionGump, iUsed, 149, 116);

	ReproductionGump.AddButton(177, 116, 0xD4, 0xD4, 20, 20, 20); // Seeds
	ReproductionGump.AddPicture(160, 121, 0xDCF);
	AddSeedsState(ReproductionGump, iUsed,199, 116);

	ReproductionGump.AddButton(70, 163, 0xD2, 0xD2, 21, 21, 21); // Gather pollen
	ReproductionGump.AddPicture(56, 164, 0x1AA2);

	ReproductionGump.AddButton(138, 163, 0xD2, 0xD2, 22, 22, 22); // Gather resources
	ReproductionGump.AddPicture(123, 167, 0x1021);

	ReproductionGump.AddButton(212, 163, 0xD2, 0xD2, 23, 23, 23); // Gather seeds
	ReproductionGump.AddPicture(195, 168, 0xDCF);
	ReproductionGump.Send(socket);
	ReproductionGump.Free();
}

function AddResourcesState(ReproductionGump, iUsed, x, y)
{
	var Seeds = iUsed.GetTag("Seeds")

	if (!Seeds)
	{
		return false;
	}

	var Seedlength = Seeds.split(",");
	if (Seedlength.length != 3)
		return false;

	var availableSeeds = parseInt(Seedlength[0]);
	var remainingSeeds = parseInt(Seedlength[1]);
	var hueSeeds = parseInt(Seedlength[2]);

	if (availableSeeds == 0 && remainingSeeds == 0)
	{
		ReproductionGump.AddText(x + 5, y, 0x21, "X");
	}
	else
	{
		ReproductionGump.AddText(x, y, plantColor, availableSeeds + "/" + remainingSeeds);
	}
	return;
}

function AddSeedsState(ReproductionGump, iUsed, x, y)
{
	var Seeds = iUsed.GetTag("Seeds")

	if (!Seeds) 
	{
		return false;
	}

	var Seedlength = Seeds.split(",");
	if (Seedlength.length != 3)
		return false;

	var availableSeeds = parseInt(Seedlength[0]);
	var remainingSeeds = parseInt(Seedlength[1]);
	var hueSeeds = parseInt(Seedlength[2]);

	var plantColor = iUsed.GetTag("PlantColor");

	if (availableSeeds == 0 && remainingSeeds == 0)
	{
		ReproductionGump.AddText(x + 5, y, 0x21, "X");
	}
	else
	{
		ReproductionGump.AddText(x, y, hueSeeds, availableSeeds + "/" + remainingSeeds);
	}
}

function AddPollinationState(ReproductionGump, iUsed, x, y)
{
	var status = parseInt(iUsed.GetTag("PlantStage"));

	var tCrossedPlants = iUsed.GetTag("PlantCross")

	if (!tCrossedPlants)
	{
		return false;
	}

	var Crossed = tCrossedPlants.split(",");
	if (Crossed.length != 2)
		return false;

	var Pollinated = parseInt(Crossed[0]);
	var SeedBreed = parseInt(Crossed[1]);

	if (status < 7)
	{
		ReproductionGump.AddText(x, y, 0x35, "-");
	}
	else if (status >= 7 && Pollinated == 0)
	{
		ReproductionGump.AddText(x, y, 0x21, "!");
	}
	else
	{
		ReproductionGump.AddText(x, y, 0x3F, "+");
	}
}

function SetToDecorativeGump(pUser, iUsed)
{
	var socket = pUser.socket;
	var SetToDecorativeGump = new Gump;
	socket.tempObj = iUsed;

	SetToDecorativeGump.AddBackground(50, 50, 200, 150, 0xE10);

	SetToDecorativeGump.AddPicture(25, 45, 0xCEB);
	SetToDecorativeGump.AddPicture(25, 118, 0xCEC);

	SetToDecorativeGump.AddPicture(227, 45, 0xCEF);
	SetToDecorativeGump.AddPicture(227, 118, 0xCF0);

	SetToDecorativeGump.AddText(115, 85, 0x44, "Set plant");
	SetToDecorativeGump.AddText(82, 105, 0x44, "to decorative mode?");

	SetToDecorativeGump.AddButton(98, 140, 0x47E, 0x480, 24, 24, 24); // Cancel

	SetToDecorativeGump.AddButton(138, 141, 0xD2, 0xD2, 25, 25, 25); // Help
	SetToDecorativeGump.AddText(143, 141, 0x835, "?");

	SetToDecorativeGump.AddButton(168, 140, 0x481, 0x483, 26, 26, 26); // Ok
	SetToDecorativeGump.Send(socket);
	SetToDecorativeGump.Free();
}

function onTooltip(myPlant) 
{
	var tooltipText = "";
	var status = parseInt(myPlant.GetTag("PlantStage"));
	var waterLevel = myPlant.GetTag("water");

	var waterStatus = {
		1: "Hard",
		2: "Soft",
		3: "Squishy",
		4: "Sopping wet"
	};

	var water = waterStatus[waterLevel] || "Hard";

	if (status === 0) 
	{
		tooltipText = "A bowl of " + water + " dirt";
	}
	else
	{
		tooltipText = "Plant Stage: " + status + " : " + myPlant.health;
	}

	return tooltipText;
}