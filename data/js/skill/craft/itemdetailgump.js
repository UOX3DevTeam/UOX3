/// <reference path="../../definitions.d.ts" />
// @ts-check
const textHue = 0x480;				// Color of the text.
const Blacksmithing = 4023;
const Carpentry = 4025;
const Alchemy = 4028;
const Fletching = 4029;
const Tailoring = 4030;
const Tinkering = 4032;
const itemDetailID = 4026; // This script
const Cooking = 4034;
const Cartography = 4035;
const Glassblowing = 4036;
const Masonry = 4037;
const CustomCraft = 4040;

const exceptionalWearablesOnly = true;

function ItemDetailGump( pUser )
{
	var skillNames = [
	"alchemy",
	"anatomy",
	"animallore",
	"itemid",
	"armslore",
	"parrying",
	"begging",
	"blacksmithing",
	"bowcraft",
	"peacemaking",
	"camping",
	"carpentry",
	"cartography",
	"cooking",
	"detectinghidden",
	"enticement",
	"evaluatingintel",
	"healing",
	"fishing",
	"forensics",
	"herding",
	"hiding",
	"provocation",
	"inscription",
	"lockpicking",
	"magery",
	"magicresistance",
	"tactics",
	"snooping",
	"musicianship",
	"poisoning",
	"archery",
	"spiritspeak",
	"stealing",
	"tailoring",
	"taming",
	"tasteid",
	"tinkering",
	"tracking",
	"veterinary",
	"swordsmanship",
	"macefighting",
	"fencing",
	"wrestling",
	"lumberjacking",
	"mining",
	"meditation",
	"stealth",
	"removetrap",
	"necromancy",
	"focus",
	"chivalry",
	"bushido",
	"ninjitsu",
	"spellweaving ",
	"mysticism ",
	"imbuing",
	"throwing "
	];

	var socket = pUser.socket;
	var itemGump = new Gump;
	var createEntry = null;
	// Now supports both dict-based and custom harvest names
	// harvestResource[i] = { id: number, name: string }
	var harvestResource = [];
	var mainSkill;

	var detailTag   = pUser.GetTempTag( "ITEMDETAILS" );
	var skillTag    = pUser.GetTempTag( "Skill" );
	var harvestTag = pUser.GetTempTag( "Harvest" );
	var harvest2Tag = pUser.GetTempTag( "Harvest2" );
	var harvest3Tag = pUser.GetTempTag( "Harvest3" );
	var harvest4Tag = pUser.GetTempTag( "Harvest4" );

	// NEW: optional custom harvest names
	var harvestNameTag   = pUser.GetTempTag( "HarvestName" );
	var harvest2NameTag  = pUser.GetTempTag( "Harvest2Name" );
	var harvest3NameTag  = pUser.GetTempTag( "Harvest3Name" );
	var harvest4NameTag  = pUser.GetTempTag( "Harvest4Name" );

	var recipeID = pUser.GetTempTag( "needRecipeID" );

	if (detailTag !== null)
	{
		try
		{
			createEntry = CreateEntries[detailTag];
		}
		catch (error)
		{
			createEntry = null;
		}
	}

	if( skillTag >= 0 && skillTag < skillNames.length )
	{
		mainSkill = parseInt( pUser.skills[ skillNames[skillTag] ] );
	}
	else
	{// if all fails fallback to alchemy
		mainSkill = parseInt( pUser.skills.alchemy );
	}

		// Helper to build harvest entries with both id + optional custom name
	function makeHarvestObj(idTag, nameTag)
	{
		// nothing set at all
		var hasName = !!(nameTag && nameTag.length);
		if (idTag === null && !hasName)
			return null;

		var idNum = 0;
		if (idTag !== null)
		{
			var n = parseInt(idTag, 10);
			if (!isNaN(n) && n > 0)
				idNum = n;
		}

		// if no valid dict id and no name, skip
		if (idNum === 0 && !hasName)
			return null;

		return {
			id: idNum,           // 0 = "no dict"
			name: hasName ? nameTag : ""
		};
	}

	// If harvest info is provided, rebuild harvestResource array
	if( harvestTag !== null || harvest2Tag !== null || harvest3Tag !== null || harvest4Tag !== null
		|| harvestNameTag !== null || harvest2NameTag !== null || harvest3NameTag !== null || harvest4NameTag !== null )
	{
		var h1 = makeHarvestObj( harvestTag,  harvestNameTag  );
		var h2 = makeHarvestObj( harvest2Tag, harvest2NameTag );
		var h3 = makeHarvestObj( harvest3Tag, harvest3NameTag );
		var h4 = makeHarvestObj( harvest4Tag, harvest4NameTag );

		if( h1 ) harvestResource.push( h1 );
		if( h2 ) harvestResource.push( h2 );
		if( h3 ) harvestResource.push( h3 );
		if( h4 ) harvestResource.push( h4 );
	}

	if( createEntry == null )
	{
		ItemDetailsGump( itemGump, pUser );

		itemGump.AddHTMLGump( 170, 130, 320, 20, false, false, "<basefont color=#ffffff>Missing CreateEntry</basefont>" );
		itemGump.AddHTMLGump( 170, 150, 320, 20, false, false, "<basefont color=#ffffff>CreateEntry ID " + detailTag + " was not found.</basefont>" );
		itemGump.AddHTMLGump( 170, 170, 320, 40, false, false, "<basefont color=#ffffff>Check the makeID in the crafting JSON file. and make sure you add it to create dfn files</basefont>" );

		itemGump.Send( socket );
		itemGump.Free();
		return;
	}

	// Recipe flags
	var needsRecipe = ( recipeID > 0 );
	var hasRecipe = false;
	if( needsRecipe )
	{
		hasRecipe = HasLearnedRecipe( pUser, recipeID );
	}

	// Fetch properties of create entry
	var createName = createEntry.name; // name of the create entry
	var createID = createEntry.id; // section id of item to craft
	var addItem = createEntry.addItem; // section header of item to craft
	var sound = createEntry.sound; // sound to play when crafting item
	var delay = createEntry.delay; // how long it takes to craft the item
	var spell = createEntry.spell; // spell requirement to craft the item
	var resources = createEntry.resources; // list of resources needed
	var skills = createEntry.skills; // list of skill requirements
	var avgMinSkill = createEntry.avgMinSkill; // average min skill required
	var avgMaxSkill = createEntry.avgMaxSkill; // average max skill required
	var minRank = createEntry.minRank;
	var maxRank = createEntry.maxRank;

	// List out resource requirements
	for( var i = 0; i < resources.length; i++ )
	{
		var resource = resources[i]
		var amountNeeded = resource[0];
		var resourceColour = resource[1];
		var resourceIDs = resource[2];
	}
	ItemDetailsGump( itemGump, pUser );
	itemGump.AddText( 330, 40, textHue, createName );
	itemGump.AddPicture( 20, 50, createID );
	var minSkillReq = 0;
	var maxSkillReq = 0;

	// List out skill requirements
	var primaryCraftSkill = -1;
	var primaryCraftSkillVal = -1;
	var exceptionalChance = 0;
	var rankSum = 0;
	var rank = 0;
	var rndNum1 = 0;
	var supportSkillTooLow = false;
	for( var i = 0; i < skills.length; i++ )
	{
		var skillReq = skills[i];
		var skillNumber = skillReq[0];
		var minSkill = skillReq[1];
		var maxSkill = skillReq[2];

		itemGump.AddHTMLGump( 170, 132 + ( i * 20 ), 200, 18, false, false, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 15000 + skills[i][0], socket.language ) + "</basefont> </center>" );
		itemGump.AddText( 430, 132 + ( i * 20 ), textHue, skills[i][1]/10 );

		if( i == 0 )
		{
			primaryCraftSkill = Skills[skillNumber];
			primaryCraftSkillVal = ( pUser.skills[skillNumber] / 10 );
			minSkillReq = minSkill;
			maxSkillReq = maxSkill;
		}
		else
		{
			if( pUser.skills[skillNumber] < minSkill )
			{
				// Support skill doesn't meet minimum requirements!
				supportSkillTooLow = true;
				break;
			}
		}

		if( GetServerSetting( "RankSystem" ))
		{
			// Rough estimate of exceptional chance based on rank system
			var rankRange = maxRank - minRank;
			var skillRange = pUser.skills[skillNumber] - minSkill;
			if( skillRange < 0 )
			{
				skillRange = minRank * 10;
			}
			else if( skillRange >= 1000 )
			{
				skillRange = maxRank * 10;
			}

			// Calculate estimated exceptional chance for current skill with the help of a few
			// constants that represent the average of some random numbers used in source code
			// calculations, modified by the SkillLevel setting in UOX.INI, which is a modifier
			// for how hard it is to craft items of exceptional quality (1 = easy, 10 = hard)
			exceptionalChance += 499.5 - (( 499.5 - skillRange ) / ( 11 - parseInt( GetServerSetting( "SkillLevel" )))); // 436 at skill level 10
		}
	}

	// Calculate success rate of crafting based on primary skill
	var chance = (( mainSkill - minSkillReq ) / ( maxSkillReq - minSkillReq ));

	if( GetServerSetting( "RankSystem" ))
	{
		// Estimate for exceptional chance, based on average exceptionalChance of all skills required
		exceptionalChance = ( exceptionalChance / skills.length ) / 10;

		// Modify exceptionalChance by base success rate of crafting
		exceptionalChance *= chance;
	}

	// Give player a minimum chance of crafting at 50% since they met skill requirement
	var chance = Math.max( 0.5, chance );

	if( GetServerSetting( "StatsAffectSkillChecks" ))
	{
		// Modify base chance of success with bonuses from stats, if this feature is enabled in ini
		chance += (( pUser.strength * ( primaryCraftSkill.strength / 10 )) / 10000 );
		chance += (( pUser.dexterity * ( primaryCraftSkill.dexterity / 10 )) / 10000 );
		chance += (( pUser.intelligence * ( primaryCraftSkill.intelligence / 10 )) / 10000 );
	}

	var maxHarvest = harvestResource.length;
	if( resources.length < maxHarvest )
		maxHarvest = resources.length;

	// MATERIALS list, now supporting custom names
	for( var i = 0; i < maxHarvest; i++ )
	{
		var hObj = harvestResource[i]; // { id, name }
		var label = "";

		if( hObj )
		{
			var dictText = "";
			if( hObj.id && hObj.id > 0 )
				dictText = GetDictionaryEntry( hObj.id, socket.language ) || "";

			if( hObj.name && hObj.name.length > 0 )
			{
				// Both set: "Custom (Dict)"
				if( dictText && dictText.length > 0 )
					label = hObj.name + " (" + dictText + ")";
				else
					label = hObj.name;
			}
			else
			{
				// Only dict
				label = dictText;
			}

			// Optional safety fallback if both are empty
			if( !label || label.length === 0 )
			{
				if( hObj.id && hObj.id > 0 )
					label = "Resource " + hObj.id;
				else
					label = "Resource";
			}
		}

		itemGump.AddText( 170, 219 + ( i * 20 ), textHue, label );
		itemGump.AddText( 430, 219 + ( i * 20 ), textHue, resources[i][0] );
	}

	if( supportSkillTooLow || chance < 0.0 || primaryCraftSkillVal < ( minSkillReq / 10 ))
	{
		chance = 0;
		exceptionalChance = 0;
	}
	else if( chance > 1.0 )
	{
		chance = 1.0; // Cap chance at 100%
	}

	itemGump.AddText( 430, 80, textHue, ( chance * 100 ).toFixed( 1 ) + "%" ); // Success Chance:
	if( !exceptionalWearablesOnly || CheckTileFlag( createID, 22 )) // TF_WEARABLE
	{
		if( exceptionalChance == 0 )
		{
			itemGump.AddText( 430, 100, textHue, "0%" ); // Exceptional Chance:
		}
		else
		{
			itemGump.AddText( 430, 100, textHue, Math.min( 100, Math.max( 0, ( exceptionalChance - 5 ))).toFixed( 0 ) + "%" + " - " + Math.min( 100, ( exceptionalChance + 5 )).toFixed( 0 ) + "%" ); // Exceptional Chance:
		}
	}
	else if( exceptionalWearablesOnly || !CheckTileFlag( createID, 22 )) // TF_WEARABLE?
	{
		itemGump.AddText( 430, 100, textHue, "-" ); // No chance of exceptional, not a wearable item!
	}

	if( needsRecipe )
	{
		var recipeMsg;
		if( hasRecipe )
		{
			recipeMsg = "<basefont color=#00ff00>You have learned this recipe.</basefont>";
		}
		else
		{
			recipeMsg = "<basefont color=#ff0000>You have not learned this recipe.</basefont>";
		}

		// OTHER box starts at y=302, you already use 302+20 for the color note (dict 10006),
		// so 302+40 (342) is a safe line under that.
		itemGump.AddHTMLGump( 170, 342, 310, 18, false, false, recipeMsg );
	}
	itemGump.Send( socket );
	itemGump.Free();
}

function ItemDetailsGump( itemGump, pUser )
{
	var socket = pUser.socket;
	itemGump.AddPage( 0 );
	itemGump.AddBackground( 0, 0, 530, 417, 5054 );
	itemGump.AddTiledGump( 10, 10, 510, 22, 2624 );
	itemGump.AddTiledGump( 10, 37, 150, 148, 2624 );
	itemGump.AddTiledGump( 165, 37, 355, 90, 2624 );
	itemGump.AddTiledGump( 10, 190, 155, 22, 2624 );
	itemGump.AddTiledGump( 10, 240, 150, 57, 2624 );
	itemGump.AddTiledGump( 165, 132, 355, 80, 2624 );
	itemGump.AddTiledGump( 10, 325, 150, 57, 2624 );
	itemGump.AddTiledGump( 165, 217, 355, 80, 2624 );
	itemGump.AddTiledGump( 165, 302, 355, 80, 2624 );
	itemGump.AddTiledGump( 10, 387, 510, 22, 2624 );
	itemGump.AddCheckerTrans( 10, 10, 510, 399 );
	itemGump.AddHTMLGump( 170, 40, 150, 20, false, false, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 10000, socket.language ) + "</basefont> </center>" );  // ITEM

	itemGump.AddHTMLGump( 10, 217, 150, 22, false, false, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 10001, socket.language ) + "</basefont> </center>" );  //<CENTER>MATERIALS</CENTER>
	itemGump.AddHTMLGump( 10, 302, 150, 22, false, false, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 10002, socket.language ) + "</basefont> </center>" );  // <CENTER>OTHER</CENTER>
	itemGump.AddHTMLGump( 170, 80, 250, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10003, socket.language ) + "</basefont>" );  // Success Chance:
	if( GetServerSetting( "RankSystem" ))
	{
		itemGump.AddHTMLGump( 170, 100, 250, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10004, socket.language ) + "</basefont>" );  // Exceptional Chance:
	}
	itemGump.AddButton( 15, 387, 0xfa5, 1, 0, 1 );
	itemGump.AddHTMLGump( 50, 390, 150, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10005, socket.language ) + "</basefont>" );  // BACK
	itemGump.AddHTMLGump( 170, ( 302 + 20 ), 310, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10006, socket.language ) + "</basefont>" );  // * The item retains the color of this material
	itemGump.AddText(  500, 219, textHue, "*"  );
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;

	// Don't continue if character is invalid, or worse... dead!
	if( !ValidateObject( pUser ) || pUser.dead )
		return;

	var bItem = pSock.tempObj;
	var gumpID = itemDetailID + 0xffff;
	switch( pButton )
	{
		case 0:
			pUser.SetTempTag( "ITEMDETAILS", null )
			pUser.SetTempTag( "CRAFT", null )
			pSock.CloseGump( gumpID, 0 );
			break;
		case 1:
			switch( pUser.GetTempTag( "CRAFT" ))
			{
				case 1:
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag("page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
						case 5: // Page 5
						case 6: // Page 6
						case 7: // Page 7
						case 8: // Page 8
						case 9: // Page 9
						case 10: // Page 10
							TriggerEvent( Carpentry, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Carpentry, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 2:
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag("page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
							TriggerEvent( Alchemy, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Alchemy, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 3:
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag("page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
							TriggerEvent( Fletching, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Fletching, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 4: // Tailoring
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag("page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
						case 5: // Page 5
						case 6: // Page 6
						case 7: // Page 7
						case 8: // Page 8
							TriggerEvent( Tailoring, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Tailoring, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 5: // Blacksmithing
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag( "page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
						case 5: // Page 5
						case 6: // Page 6
						case 7: // Page 7
							TriggerEvent( Blacksmithing, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Blacksmithing, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 6: // Cooking
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag("page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
							TriggerEvent( Cooking, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Cooking, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 7: // Tinkering
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag( "page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
						case 5: // Page 5
						case 6: // Page 6
						case 7: // Page 7
						case 8: // Page 8
						case 9: // Page 9
							TriggerEvent( Tinkering, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Tinkering, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 8: // Cartography
					pUser.SetTempTag( "ITEMDETAILS", null );
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag( "page" ))
					{
						case 1: // Page 1
							TriggerEvent( Cartography, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Cartography, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 9: // Glassblowing
					pUser.SetTempTag( "ITEMDETAILS", null );
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag( "page" ))
					{
						case 1: // Page 1
							TriggerEvent( Glassblowing, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Glassblowing, "PageX", pSock, pUser, 1 );
					}
					break;
				case 10: // masonry
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag("page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
						case 5: // Page 5
						case 6: // Page 6
						case 7: // Page 7
						case 8: // Page 8
						case 9: // Page 9
							TriggerEvent( Masonry, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Masonry, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 100: // Custom Craft
					pUser.SetTempTag( "ITEMDETAILS", null );
					pSock.CloseGump( gumpID, 0 );
					TriggerEvent( CustomCraft, "PageX", pSock, pUser, pUser.GetTempTag( "page" ) || 1 );
					break;
			}
	}
}

function HasLearnedRecipe( pUser, recipeID )
{
	var myData = TriggerEvent( 4022, "ReadRecipeID", pUser );
	if( !myData || myData.length == 0 )
		return false;

	for( var i = 0; i < myData.length; i++ )
	{
		var data = myData[i].split( "," );
		if( data[0] == recipeID )
			return true;
	}
	return false;
}
