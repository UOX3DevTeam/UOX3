/// <reference path="../../definitions.d.ts" />
// @ts-check
const textHue = 0x480;                 // Color of the text.
const blacksmithID = 4023;			   // Script ID used to identify and close this gump
const gumpDelay = 2000;                // Timer for the gump to reappear after crafting.
const ingotDelay = 200;                // Timer for the gump to reappear after selecting an ingot.
const repairDelay = 200;               // Timer for the gump to reappear after repairing an item
const craftGumpID = 4027;
const itemDetailsScriptID = 4026;

const itemsPerPage = 10;               // Number of craftable items shown per gump subpage
const displayUnlearnedRecipes = true;  // Show recipes player has not learned (if we add any later)
const coreShardEra = EraStringToNum( GetServerSetting( "CoreShardEra" ));

// If enabled, players can craft coloured variants of weapons, though unless the craftItems array
// is updated with specific create entries for the coloured weapon variants, they will just be
// regular weapons with ore colour applied
const allowColouredWeapons = GetServerSetting( "CraftColouredWeapons" );

const OreTypes = [
    { nameDict: 10291, hue: 0x0000, minMining:    0 }, // Iron
    { nameDict: 10203, hue: 0x0973, minMining:  650 }, // Dull Copper
    { nameDict: 10204, hue: 0x0966, minMining:  700 }, // Shadow Iron
    { nameDict: 10205, hue: 0x07dd, minMining:  750 }, // Copper
    { nameDict: 10206, hue: 0x06d6, minMining:  800 }, // Bronze
    { nameDict: 10207, hue: 0x08a5, minMining:  850 }, // Gold
    { nameDict: 10208, hue: 0x0979, minMining:  900 }, // Agapite
    { nameDict: 10209, hue: 0x089f, minMining:  950 }, // Verite
    { nameDict: 10210, hue: 0x08ab, minMining:  990 }  // Valorite
];

const ScaleTypes = [
    { nameDict: 20299, hue: 0x0663, minSkill:  0 }, // Red
    { nameDict: 20300, hue: 0x084d, minSkill:  0 }, // Yellow
    { nameDict: 20301, hue: 0x0455, minSkill:  0 }, // Black
	{ nameDict: 20302, hue: 0x0851, minSkill:  0 }, // Green
	{ nameDict: 20303, hue: 0x02c2, minSkill:  0 }, // White
	{ nameDict: 20304, hue: 0x0006, minSkill:  0 }  // Blue
    // add more as required
];

// Optional: if you later decide to make some blacksmith items recipe-locked, we will use this map:
// BlacksmithMap[buttonID] = { dictID, page, timerID, oreMake: [makeIDByOre], recipeID?, minEra?, maxEra? }
// o--------------------------------------------------------------------------o
// | Script        - blacksmithing.js                                         |
// | System        - Blacksmith Crafting Gump							      |
// o--------------------------------------------------------------------------o
// | Purpose       -                                                          |
// |   Provides the blacksmith crafting menu using the same data-driven       |
// |   system used by the tailoring script.                                   |
// |                                                                          |
// |   All craftable items are defined in tables (myPage, craftItems) and     |
// |   then mapped into a MasonryMap structure that controls:                 |
// |     - Which dictionary entry is shown per row                            |
// |     - Which "makeID" entry is used for each ore type                     |
// |     - Which page and timer ID to use when reopening the gump             |
// |     - Optional per-item recipe and era requirements                      |
// |     - Optional per-item custom names for display                         |
// |                                                                          |
// |   The script also handles:                                               |
// |     - iron selection (iron / colored ores) with skill requirements       |
// |     - Smelting iron items back into Granite                              |
// |     - Repairing stone armor and weapons at an anvil                      |
// |     - Tool wear and runic hammer handling                                |
// |     - A "Make Last" feature                                              |
// |     - A "Last Ten Masonry" list (optional)                               |
// o--------------------------------------------------------------------------o
// | Data Tables                                                              |
// o--------------------------------------------------------------------------o
// | myPage                                                                   |
// |   myPage[pageIndex] = [ dictID1, dictID2, ... ]                          |
// |     pageIndex 0 => Page 1: Metal Armor                                   |
// |     pageIndex 1 => Page 2: Helmets                                       |
// |     pageIndex 2 => Page 3: Shields                                       |
// |     pageIndex 3 => Page 4: Bladed weapons                                |
// |     pageIndex 4 => Page 5: Axes                                          |
// |     pageIndex 5 => Page 6: Polearms                                      |
// |     pageIndex 6 => Page 7: Bashing weapons                               |
// |                                                                          |
// |   Each entry is a dictionary ID that will be used to look up the text    |
// |   for that row, unless a customName is defined for that button in        |
// |   MasonryMap.                                                            |
// |                                                                          |
// | craftItems                                                               |
// |   craftItems[ironIndex][pageIndex][itemIndex] = makeID                   |
// |     oreIndex 0 = Iron                                                    |
// |     oreIndex 1 = Dull Copper                                             |
// |     oreIndex 2 = Shadow Iron                                             |
// |     oreIndex 3 = Copper                                                  |
// |     oreIndex 4 = Bronze                                                  |
// |     oreIndex 5 = Gold                                                    |
// |     oreIndex 6 = Agapite												  |
// |     oreIndex 7 = Verite												  |
// |     oreIndex 8 = Valorite												  |
// |                                                                          |
// |   For each granite type and page, this holds the createEntry ID used by  |
// |   MakeItem when the player crafts that item. The same index positions    |
// |   on each page line up with the matching entries in myPage.              |
// o--------------------------------------------------------------------------o
// | BlacksmithMap                                                            |
// o--------------------------------------------------------------------------o
// | BlacksmithMap is built automatically from myPage and craftItems.         |
// |                                                                          |
// |   BlacksmithMap[buttonID] = {                                            |
// |       dictID    : number,     // Base dictionary entry for the row       |
// |       page      : number,     // Main page (1..7, or 999 for Last Ten)   |
// |       timerID   : number,     // Timer ID to reopen same page            |
// |       oreMake : number[],	   // oreMake[oreIndex] = makeID			  |
// |       customName: string?,    // Optional override for display text      |
// |       recipeID  : number?,    // Optional recipe requirement             |
// |       minEra    : string?,    // Optional minimum shard era              |
// |       maxEra    : string?     // Optional maximum shard era              |
// |   };                                                                     |
// |                                                                          |
// | Button ID mapping (same as original script):                             |
// |   Page 1 (Metal Armors) : 100..108                                       |
// |   Page 2 (Helmets)   : 200..205                                          |
// |   Page 3 (Shields)     : 300..305                                        |
// |   Page 4 (Bladed) : 400..407                                             |
// |   Page 5 (Axes) : 500..506                                               |
// |   Page 6 (PoleArms) : 600..604											  |
// |   Page 7 (Bashing): 700..704										      |
// |                                                                          |
// | Custom Names                                                             |
// |   To override the display name for a specific row, set customName after  |
// |   the BlacksmithMap has been initialized, for example:                   |
// |                                                                          |
// |       BlacksmithMap[100].customName = "Hump vase";		                  |
// |                                                                          |
// |   PageX() will use this order of preference for text:                    |
// |     1. entry.customName (if set)                                         |
// |     2. GetDictionaryEntry(entry.dictID)                                  |
// |     3. A fallback "[Unnamed Item: buttonID]"                             |
// |                                                                          |
// | Recipes                                                                  |
// |   If recipeID is set on a MasonryMap entry, onGumpPress will call:       |
// |       TriggerEvent(4022, "NeedRecipe", pUser, recipeID)                  |
// |   to check if the player has learned that recipe. If not, the craft      |
// |   attempt is blocked and a message is shown.                             |
// |                                                                          |
// | Era Gating                                                               |
// |   The script reads the shard era using:                                  |
// |   const coreShardEra = EraStringToNum(GetServerSetting("CoreShardEra")); |
// |                                                                          |
// |   If an entry defines minEra or maxEra (strings like "lbr","aos","ml",   |
// |   "sa","hs","tol"), eraOK(entry) will ensure the current server era is   |
// |   within that range before allowing craft or display.                    |
// o--------------------------------------------------------------------------o
// | Notes                                                                    |
// o--------------------------------------------------------------------------o
// | - To add new blacksmith items, update myPage and craftItems, then        |
// |   optionally decorate their BlacksmithMap entries with customName,       |
// |   recipeID, minEra, and maxEra.                                          |
// o--------------------------------------------------------------------------o

const myPage = [
	// Page 1 - Metal Armors
	[10217, 10218, 10219, 10220, 10221, 10222, 10223, 10224, 10225, 10226, 10227, 10228, 10229, 0, 0, 0, 0 ,0],
	// Page 2 - Helmets
	[10230, 10231, 10232, 10233, 10234],
	// Page 3 - Shields
	[10235, 10236, 10237, 10238, 10239, 10293],
	 // Page 4 - Bladed
	[10240, 10241, 10242, 10243, 10244, 10245, 10246, 10247],
	// Page 5 - Axes
	[10248, 10249, 10250, 10251, 10252, 10253, 10254],
	// Page 6 - PoleArms
	[10255, 10256, 10257, 10258, 10259],
	// Page 7 - Bashing
	[10260, 10261, 10262, 10263, 10264]
];

const craftItems = [
	// Iron
	[
		// Metal Armors
		[ 7, 9, 8, 10, 11, 12, 13, 16, 15, 14, 17, 18, 19, 367, 368, 369, 370, 371 ],
		// Helmets
		[ 46, 48, 45, 47, 49 ],
		// Shields
		[ 1, 2, 6, 3, 5, 4 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Dull Copper
	[
		// Metal Armors
		[ 506, 508, 507, 509, 510, 511, 512, 515, 514, 513, 516, 517, 518 ],
		// Helmets
		[ 520, 522, 519, 521, 523 ],
		// Shields
		[ 500, 501, 505, 502, 504, 503 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Shadow Iron
	[
		// Metal Armors
		[ 606, 608, 607, 609, 610, 611, 612, 615, 614, 613, 616, 617, 618 ],
		// Helmets
		[ 620, 622, 619, 621, 623 ],
		// Shields
		[ 600, 601, 605, 602, 604, 603 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Copper
	[
		// Metal Armors
		[ 706, 708, 707, 709, 710, 711, 7012, 715, 714, 713, 716, 717, 718 ],
		// Helmets
		[ 720, 722, 719, 721, 723 ],
		// Shields
		[ 700, 701, 705, 702, 704, 703 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Bronze
	[
		// Metal Armors
		[ 806, 808, 807, 809, 810, 811, 812, 815, 814, 813, 816, 817, 818 ],
		// Helmets
		[ 820, 822, 819, 821, 823 ],
		// Shields
		[ 800, 801, 805, 802, 804, 803 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Gold
	[
		// Metal Armors
		[ 906, 908, 907, 909, 910, 911, 912, 915, 914, 913, 916, 917, 918 ],
		// Helmets
		[ 920, 922, 919, 921, 923 ],
		// Shields
		[ 900, 901, 905, 902, 904, 903 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Agapite
	[
		// Metal Armors
		[ 1206, 1208, 1207, 1209, 1210, 1211, 1212, 1215, 1214, 1213, 1216, 1217, 1218 ],
		// Helmets
		[ 1220, 1222, 1219, 1221, 1223 ],
		// Shields
		[ 1200, 1201, 1205, 1202, 1204, 1203 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Verite
	[
		// Metal Armors
		[ 1006, 1008, 1007, 1009, 1010, 1011, 1012, 1015, 1014, 1013, 1016, 1017, 1018 ],
		// Helmets
		[ 1020, 1022, 1019, 1021, 1023 ],
		// Shields
		[ 1000, 1001, 1005, 1002, 1004, 1003 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	],

	// Valorite
	[
		// Metal Armors
		[ 1106, 1108, 1107, 1109, 1110, 1111, 1112, 1115, 1114, 1113, 1116, 1117, 1118 ],
		// Helmets
		[ 1120, 1122, 1119, 1121, 1123 ],
		// Shields
		[ 1100, 1101, 1105, 1102, 1104, 1103 ],
		// Bladed
		[ 25, 21, 20, 22, 23, 26, 24, 27 ],
		// Axes
		[ 29, 28, 32, 30, 33, 31, 34 ],
		// Polearms
		[ 38, 39, 35, 36, 37 ],
		// Bashing
		[ 44, 40, 41, 42, 43 ]
	]
];

// BlacksmithMap[buttonID] = {
//     dictID: <dictionaryID>,
//     page: <pageNumber>,
//     timerID: <timerID>,
//     oreMake: [ makeIDForIron, makeIDForDullCopper, ... ], // index is oreID (0..8)
//     // Optional later:
//     // recipeID: <recipeID>,
//     // minEra: "lbr" / "aos" / "ml" / "sa" / "hs" / "tol",
//     // maxEra: ...
// };

const BlacksmithMap = {};

(function initBlacksmithMap()
{
	// oreIndex: 0 = iron, 1 = dull copper, ... 8 = valorite
	for( var oreIndex = 0; oreIndex < craftItems.length; oreIndex++ )
	{
		var oreRows = craftItems[oreIndex];

		// pageIdx: 0..6 => pages 1..7
		for( var pageIdx = 0; pageIdx < myPage.length; pageIdx++ )
		{
			var dictList = myPage[pageIdx];
			var makeList = oreRows[pageIdx];

			for( var i = 0; i < dictList.length && i < makeList.length; i++ )
			{
				// Old script uses:
				// page 1 => 100..112
				// page 2 => 200..204
				// page 3 => 300..305
				// etc.
				var buttonID = ( ( pageIdx + 1 ) * 100 ) + i;
				var dictID = dictList[i];
				var makeID = makeList[i];

				if( !BlacksmithMap[buttonID] )
				{
					BlacksmithMap[buttonID] = {
						dictID: dictID,
						page: pageIdx + 1,
						timerID: pageIdx + 1,
						oreMake: [],
						// recipeID: undefined,
						// minEra: undefined,
						// maxEra: undefined
						skill: 7,                // blacksmithing skill ID
						harvest: [10015],        // ingots dict
						harvest2: [],             // optional second resource
						harvest3: [],             // optional second resource
						harvest4: []             // optional second resource
					};
				}

				BlacksmithMap[buttonID].oreMake[oreIndex] = makeID;
			}
		}
	}
})();

// 3) AFTER initBlacksmithMap, you can override entries:
// BlacksmithMap[400].customName = "Elven Broadsword";
// BlacksmithMap[400].recipeID = 5101;   // if you want it recipe-locked
// BlacksmithMap[400].minEra = "ml";     // if you want it ML and later only
BlacksmithMap[113].customName = "Dragon Sleeves";
BlacksmithMap[113].useScales = true;
BlacksmithMap[114].customName = "Dragon Breast Plate";
BlacksmithMap[114].useScales = true;
BlacksmithMap[115].customName = "Dragon Gloves";
BlacksmithMap[115].useScales = true;
BlacksmithMap[116].customName = "Dragon Helmet";
BlacksmithMap[116].useScales = true;
BlacksmithMap[117].customName = "Dragon leggings";
BlacksmithMap[117].useScales = true;

function PageX( socket, pUser, pageNum )
{
	if( !ValidateObject( pUser ))
		return;

	// Pages 1 - 7: normal crafting pages
	// Page 999: optional "Last Ten Blacksmith" (if you decide to use it later)

	var subPage = pUser.GetTempTag( "subPage" ) || 1;
	var pageItems = [];

	if( pageNum == 999 )
	{
		var lastTenRaw = pUser.GetTempTag( "LastTenBlacksmith" ) || "";
		var split = lastTenRaw.split( "," );
		for( var i = 0; i < split.length; i++ )
		{
			var val = parseInt( split[i] );
			if( !isNaN( val ) && BlacksmithMap[val] )
				pageItems.push( val ); // here val is the buttonID itself
		}
	}
	else
	{
		// Build list of buttonIDs for this page from BlacksmithMap
		for( var buttonID in BlacksmithMap )
		{
			var data = BlacksmithMap[buttonID];
			if( data.page == pageNum && eraOK( data ))
			{
				// If we later add recipes and want to hide unknown ones:
				var needsRecipe = data.recipeID;
				var showAll = displayUnlearnedRecipes;
				if( !needsRecipe || showAll || HasLearnedRecipe( pUser, needsRecipe ))
				{
					pageItems.push( parseInt( buttonID ) );
				}
			}
		}

		// Sort by buttonID to keep consistent ordering
		pageItems.sort( function( a, b ){ return a - b; } );
	}

	if( pageItems.length == 0 )
	{
		var emptyGump = new Gump;
		TriggerEvent( craftGumpID, "CraftingGumpMenu", emptyGump, socket );
		emptyGump.AddPage( 1 );
		emptyGump.AddText( 220, 60, textHue, "No items available on this page." );
		emptyGump.Send( socket );
		emptyGump.Free();
		return;
	}

	var totalSubPages = Math.ceil( pageItems.length / itemsPerPage );

	if( subPage < 1 )
		subPage = 1;
	if( subPage > totalSubPages )
		subPage = totalSubPages;

	pUser.SetTempTag( "page", pageNum );
	pUser.SetTempTag( "subPage", subPage );

	var startIndex = ( subPage - 1 ) * itemsPerPage;
	var endIndex = Math.min( startIndex + itemsPerPage, pageItems.length );

	if( startIndex >= pageItems.length )
	{
		subPage = 1;
		startIndex = 0;
		endIndex = Math.min( itemsPerPage, pageItems.length );
		pUser.SetTempTag( "subPage", subPage );
	}

	var resourceHue = pUser.GetTempTag( "resourceHue" ) | 0;
	var blacksmithMenu = new Gump;
	TriggerEvent( craftGumpID, "CraftingGumpMenu", blacksmithMenu, socket );
	blacksmithMenu.AddPage( 1 );

	var drawIndex = 0;

	for( var i = startIndex; i < endIndex; i++ )
	{
		var buttonID = pageItems[i];
		var data = BlacksmithMap[buttonID];

		// Do not show weapons when colored ingots selected and colored weapons are disabled
		if( !allowColouredWeapons && resourceHue > 0 && data.page > 3 )
			continue;

		var entryText = "";
		if( data.customName )
		{
			entryText = data.customName;
		}
		else if( data.dictID )
		{
			entryText = GetDictionaryEntry( data.dictID, socket.language );
			if( !entryText || entryText === "" )
				entryText = "[Missing EntryID: " + data.dictID + "]";
		}
		else
		{
			entryText = "[Unnamed Item: " + buttonID + "]";
		}

		// Same layout as tailoring: button, text, details button
		blacksmithMenu.AddButton( 220, 60 + ( drawIndex * 20 ), 4005, 4007, 1, 0, buttonID );
		blacksmithMenu.AddText( 255, 60 + ( drawIndex * 20 ), textHue, entryText );
		blacksmithMenu.AddButton( 480, 60 + ( drawIndex * 20 ), 4011, 4012, 1, 0, 2000 + buttonID );

		drawIndex++;
	}

	// Prev subpage
	if( subPage > 1 )
	{
		blacksmithMenu.AddButton( 220, 260, 4014, 4015, 1, 0, 8000 + ( subPage - 1 ));
		blacksmithMenu.AddHTMLGump( 255, 263, 100, 18, 0, 0,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" );
	}

	// Next subpage
	if( subPage < totalSubPages )
	{
		blacksmithMenu.AddButton( 370, 260, 4005, 4007, 1, 0, 9000 + ( subPage + 1 ));
		blacksmithMenu.AddHTMLGump( 405, 263, 100, 18, 0, 0,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" );
	}

	blacksmithMenu.Send( socket );
	blacksmithMenu.Free();
}

function Page8( socket, pUser )
{
    // Ingot choices page
    var myGump = new Gump();
    pUser.SetTempTag( "page", 8 );

    // Draw standard crafting frame (title, buttons, notices etc.)
    TriggerEvent( craftGumpID, "CraftingGumpMenu", myGump, socket );

    // Count ingots by hue
    var iron       = pUser.ResourceCount( 0x1BF2 );
    var dullcopper = pUser.ResourceCount( 0x1BF2, 0x0973 );
    var shadowiron = pUser.ResourceCount( 0x1BF2, 0x0966 );
    var copper     = pUser.ResourceCount( 0x1BF2, 0x07dd );
    var bronze     = pUser.ResourceCount( 0x1BF2, 0x06d6 );
    var gold       = pUser.ResourceCount( 0x1BF2, 0x08a5 );
    var agapite    = pUser.ResourceCount( 0x1BF2, 0x0979 );
    var verite     = pUser.ResourceCount( 0x1BF2, 0x089f );
    var valorite   = pUser.ResourceCount( 0x1BF2, 0x08ab );

    var counts = [ iron, dullcopper, shadowiron, copper, bronze, gold, agapite, verite, valorite ];

    var lines = [];
    for( var i = 0; i < OreTypes.length; i++ )
    {
        var oreInfo = OreTypes[i];
        var label   = GetDictionaryEntry( oreInfo.nameDict, socket.language );
        lines.push( label + " (" + counts[i].toString() + ")" );
    }

    for( var j = 0; j < lines.length; j++ )
    {
        var index = j % 10;

        if( index == 0 )
        {
            if( j > 0 )
            {
                myGump.AddButton( 370, 260, 4005, 4007, 0, ( j / 10 ) + 1, 0 );
                myGump.AddHTMLGump( 405, 263, 100, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" ); // NEXT PAGE
            }

            myGump.AddPage(( j / 10 ) + 1 );

            if( j > 0 )
            {
                myGump.AddButton( 220, 260, 4014, 4015, 0, j / 10, 0 );
                myGump.AddHTMLGump( 255, 263, 100, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" ); // PREV PAGE
            }
        }

        myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, 1000 + j );
        myGump.AddText(   255, 60 + ( index * 20 ), textHue, lines[j] );
    }

    myGump.Send( socket );
    myGump.Free();
}

function Page30( socket, pUser )
{
    var myGump = new Gump();
    pUser.SetTempTag( "page", 30 ); // page index for scales (use any unused index)

    // Base crafting frame (same as other pages)
    TriggerEvent( craftGumpID, "CraftingGumpMenu", myGump, socket );

    // Count scales in backpack by hue (update IDs/hues as needed)
    // If all scales share the same item ID but different hues, adjust this.
    var counts = [];

    for( var i = 0; i < ScaleTypes.length; i++ )
    {
        var scaleInfo = ScaleTypes[i];

        // Example: if all scales share a single ID (replace 0x26B4 with your scale item ID)
        // counts[i] = pUser.ResourceCount( 0x26B4, scaleInfo.hue );

        // If you have different IDs per scale type, change the above line as needed.
        counts[i] = pUser.ResourceCount( 0x26b4, scaleInfo.hue );; // placeholder so script parses; you’ll replace with real logic
    }

    var lines = [];
    for( var j = 0; j < ScaleTypes.length; j++ )
    {
        var sInfo = ScaleTypes[j];
        var label = GetDictionaryEntry( sInfo.nameDict, socket.language );
        lines.push( label + " (" + counts[j].toString() + ")" );
    }

    for( var k = 0; k < lines.length; k++ )
    {
        var index = k % 10;

        if( index == 0 )
        {
            if( k > 0 )
            {
                myGump.AddButton( 370, 260, 4005, 4007, 0, ( k / 10 ) + 1, 0 );
                myGump.AddHTMLGump( 405, 263, 100, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" ); // NEXT PAGE
            }

            myGump.AddPage(( k / 10 ) + 1 );

            if( k > 0 )
            {
                myGump.AddButton( 220, 260, 4014, 4015, 0, k / 10, 0 );
                myGump.AddHTMLGump( 255, 263, 100, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" ); // PREV PAGE
            }
        }

        // Use a separate button range from ores to avoid conflicts: 1100+
        myGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, 1100 + k );
        myGump.AddText(   255, 60 + ( index * 20 ), textHue, lines[k] );
    }

    myGump.Send( socket );
    myGump.Free();
}

function FindNearbyAnvils( pUser, trgItem, pSock )
{
	if( !ValidateObject( trgItem ) || !trgItem.isItem )
		return false;

	return ( trgItem.id == 0x0faf || trgItem.id == 0x0fb0 || trgItem.id == 0x2dd5 || trgItem.id == 0x2dd6 );
}

function FindNearbyForges( pUser, trgItem, pSock )
{
	if( !ValidateObject( trgItem ) || !trgItem.isItem )
		return false;

	return (( trgItem.id >= 0x197a && trgItem.id <= 0x19a9 ) || trgItem.id == 0x0Fb1 || trgItem.id == 0x2db0 || trgItem.id == 0x2dd8 );
}

function SmeltTarget( pSock )
{
	pSock.CustomTarget( 1, GetDictionaryEntry( 440, pSock.language )); // What item would you like to smelt?
}

// Armor and weapons can be smelted back into ingots.
/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback1( pSock, ourObj )
{
	// Smelt item, get ingots in return
	var mChar = pSock.currentChar;

	if( !ValidateObject( ourObj ) || !ourObj.isItem )
	{
		// Targeted object is not an item that can be smelted
		mChar.SetTempTag( "prevActionResult", "CANTSMELT" );
		mChar.StartTimer( ingotDelay, 1, true );
		return;
	}

	var nearbyAnvil = AreaItemFunction( "FindNearbyAnvils", mChar, 3, pSock );
	var nearbyForge = AreaItemFunction( "FindNearbyForges", mChar, 3, pSock );
	if( nearbyForge == 0 || nearbyAnvil == 0)
	{
		// No forge nearby
		mChar.SetTempTag( "prevActionResult", "NOFORGEORANVIL" );
		mChar.StartTimer( ingotDelay, 1, true );
		return;
	}

	var creatorSerial = ourObj.creator;
	var entryMadeFrom = ourObj.entryMadeFrom;
	var createEntry;
	if( entryMadeFrom != null && entryMadeFrom != 0 )
	{
		createEntry = CreateEntries[entryMadeFrom];
	}
	if( createEntry != null && createEntry.id != ourObj.id )
	{
		createEntry = null;
	}

	var resourceName = "iron ingot";
	var resourceAmount = 0;
	var maxResourceAmount = 1;
	var resourceHue = ourObj.colour;

	if( creatorSerial == -1 || entryMadeFrom == 0 || createEntry == null )
	{
		// Not a player-crafted item, return 1 ingot if item is made of metal
		var materialType = TriggerEvent( 2506, "GetItemMaterialType", ourObj );
		if( materialType == "metal" )
		{
			resourceAmount = 1;
		}
	}
	else
	{
		if( createEntry.avgMinSkill > mChar.skills.mining )
		{
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "NOSMELTSKILL" );
			mChar.StartTimer( gumpDelay, 1, true );
			return;
		}

		// Loop through resources used to craft item, see how many ingots were used
		var resourcesUsed = createEntry.resources;
		for( var i = 0; i < resourcesUsed.length; i++ )
		{
			var resource = resourcesUsed[i];
			var amountNeeded = resource[0];
			var colorNeeded = resource[1];
			var resourceIDs = resource[2];

			// Loop through list of resourceIDs that were valid for crafting this item, see if ANY
			// were a match for the resource we're trying to return
			for( var j = 0; j <= resourceIDs.length; j++ )
			{
				// If both resource needed matches up, and resource color matches up, go for it
				if( resourceIDs[j] == 0x1bf2 && colorNeeded == resourceHue )
				{
					maxResourceAmount = amountNeeded;
					break;
				}
			}
		}

		if( maxResourceAmount > 1 )
		{
			// Calculate amount of resources returned based on player's mining skill, item's wear and tear,
			// and amount of resources that went into making the item in the first place
			if ( ourObj.health >= 1 || ourObj.usesLeft >= 1 ) 
			{
				var healthPercentage = 0;
				if( ourObj.health >= 1 )
				{
					healthPercentage = Math.floor( ( ourObj.health * 100) / ourObj.maxhp );
				}

				var usesPercentage = 0;
				if( ourObj.usesLeft >= 1 ) 
				{
					usesPercentage = Math.floor( ( ourObj.usesLeft * 100 ) / ourObj.maxUses );
				}

				var itemPercentage = usesPercentage > 0 ? Math.min( healthPercentage, usesPercentage ) : healthPercentage;

				resourceAmount = Math.floor( ( maxResourceAmount * itemPercentage ) / 100 );
			}

			// Halve the amount of resources returned
			resourceAmount = Math.max( Math.floor( resourceAmount / 2 ), 1 );

			// Fetch player's Mining skill
			var playerSkill = mChar.skills.mining;

			// Based on player's Mining skill, return between 1 to maxResourceAmount
			resourceAmount = Math.min( Math.max( Math.floor( resourceAmount * ( playerSkill / 1000 )), 1 ), resourceAmount );
		}
		else
		{
			resourceAmount = 1;
		}
	}

	if( resourceAmount == 0 )
	{
		// Targeted object is not an item that can be smelted
		mChar.SetTempTag( "prevActionResult", "CANTSMELT" );
		mChar.StartTimer( ingotDelay, 1, true );
		return;
	}

	if( ourObj.isDyeable )
	{
		// Dyeable items should return regular iron ingots
		resourceHue = 0;
	}

	switch( resourceHue )
	{
		case 0: // Iron Ingot
		default:
			break;
		case 0x0973: // Dull Copper
			resourceName = "dull copper ingot";
			break;
		case 0x0966: // Shadow Iron
			resourceName = "shadow iron ingot";
			break;
		case 0x07dd: // Copper
			resourceName = "copper ingot";
			break;
		case 0x06d6: // Bronze
			resourceName = "bronze ingot";
			break;
		case 0x08a5: // Gold
			resourceName = "gold ingot";
			break;
		case 0x0979: // Agapite
			resourceName = "agapite ingot";
			break;
		case 0x089f: // Verite
			resourceName = "verite ingot";
			break;
		case 0x08ab: // Valorite
			resourceName = "valorite ingot";
			break;
	}

	// Delete the melted item
	ourObj.Delete();

	// Run a generic skill check to give player a chance to increase their mining skill
	mChar.CheckSkill( 45, 0, mChar.skillCaps.mining );

	var newResource = CreateDFNItem( pSock, mChar, "0x1bf2", resourceAmount, "ITEM", true, resourceHue );
	newResource.name = resourceName;

	mChar.SetTempTag( "ingotsFromSmelting", resourceAmount );
	mChar.SetTempTag( "prevActionResult", "SMELTITEMSUCCESS" );
	mChar.StartTimer( gumpDelay, 1, true );
}

function RepairTarget( pSock )
{
	pSock.CustomTarget( 2, GetDictionaryEntry( 485, pSock.language )); // What item would you like to repair?
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback2( pSock, ourObj )
{
	// Repair Item
	var mChar = pSock.currentChar;

	// Don't continue if character is invalid, or worse... dead!
	if( !ValidateObject( mChar ) || mChar.dead )
		return;

	var bItem = pSock.tempObj;
	var anvil = AreaItemFunction( "FindNearbyAnvils", mChar, 3, pSock );
	var gumpID = blacksmithID + 0xffff;
	pSock.tempObj = null;

	if( ValidateObject( mChar ) && mChar.isChar && ValidateObject( bItem ) && bItem.isItem )
	{
		if( !ValidateObject( ourObj ) || !ourObj.isItem
			|| TriggerEvent( 2506, "GetItemMaterialType", ourObj ) != "metal"
			|| !CheckTileFlag( ourObj.id, 22 )) // TF_WEARABLE
		{
			// Targeted object is not an item that can be repaired
			pSock.tempObj = bItem;
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "CANTREPAIR" );
			mChar.StartTimer( repairDelay, 1, true );
			return;
		}

		if( anvil == 0 )
		{
			// No anvil nearby
			pSock.tempObj = bItem;
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "NOANVIL" );
			mChar.StartTimer( repairDelay, 1, true );
			return;
		}

		var itemDurabilityLossEnabled = GetServerSetting( "ItemRepairDurabilityLoss" );
		var repairID = ourObj.id;
		var ownerObj = GetPackOwner( ourObj, 0 );
		if( ownerObj && mChar.serial == ownerObj.serial )
		{
			var maxHitpoints = ourObj.maxhp;
			var currentHitpoints = ourObj.health;
			if( currentHitpoints < maxHitpoints )
			{
				// Get base repair difficulty based on amount of HP missing and max hitpoints
				var deltaHP = maxHitpoints - currentHitpoints;
				var repairDifficulty = (( deltaHP / maxHitpoints ) * 1000 );
				var minDifficulty = repairDifficulty - 250;
				var skillBonus = 0;
				var repairSkill = mChar.skills.blacksmithing;
				if( minDifficulty < 0 )
				{
					// If minDifficulty is negative, add the negative value as a bonus to player's skill
					skillBonus = minDifficulty * -1;
					minDifficulty = 0;
				}
				else if( minDifficulty > repairSkill )
				{
					// Player skill below minimum repair difficulty, Too difficult to make the attempt!
					pSock.tempObj = bItem;
					pSock.CloseGump( gumpID, 0 );
					mChar.SetTempTag( "prevActionResult", "CANTREPAIR" );
					mChar.StartTimer( repairDelay, 1, true );
					return;
				}
				var maxDifficulty = Math.min( repairDifficulty + 250, mChar.skillCaps.blacksmithing );

				// Allow repair if random number between min and base difficulty is under player's skill
				if( RandomNumber( minDifficulty, 1000 ) < ( Math.max( repairSkill + skillBonus, 999 )))
				{
					// Give player a chance every now and then to gain skill from repairing
					if( RandomNumber( 1, 5 ) == 1 )
					{
						// Run a skill-check, which might trigger a skill-gain if player passes
						mChar.CheckSkill( 8, minDifficulty, maxDifficulty ); // Skill 8 = blacksmithing
					}

					// Reduce object's max durability by 1
					if( itemDurabilityLossEnabled )
					{
						ourObj.maxhp -= 1;
					}

					// Repair item here
					ourObj.health = ourObj.maxhp;
					pSock.SoundEffect( 0x002A, true );

					// Reopen gump after a short delay
					pSock.CloseGump( gumpID, 0 );
					mChar.SetTempTag( "prevActionResult", "REPAIRSUCCESS" );
					mChar.StartTimer( repairDelay, 1, true );

					// GM skill (100.0 skillpoints)
					// 	Item with 51 HP max
					// 		item with 2 hp left - 99.65% chance to repair
					// 		item with 25 hp left - 99.86% chance to repair
					// 		item with 40 hp left - 99.9% chance to repair

					// Expert Smith (71.5 skill points)
					//	Item with 51 HP max
					// 		item with 2 hp left - 1.45% chance to repair
					// 		item with 25 hp left - 61.49% chance to repair
					// 		item with 40 hp left - 74.9% chance to repair
					// 		item with 48 hp left - 90.6% chance to repair

					// Apprentice Smith (51.5 skill points)
					// 	Item with 51 HP max
					// 		item with 2 hp left - 0% chance to repair
					// 		item with 25 hp left - 34.5% chance to repair
					// 		item with 40 hp left - 54.9% chance to repair
					// 		item with 48 hp left - 70.6% chance to repair
				}
				else
				{
					// Failed to repair item - decrease item health!
					if( repairSkill >= 1000 ) // GM Smith
					{
						ourObj.health -= 1;
					}
					else if( repairSkill >= 715 ) // Expert Smith
					{
						ourObj.health -= 2;
					}
					else // Below Expert Smith
					{
						ourObj.health -= 3;
					}

					if( ourObj.health <= 0 )
					{
						// Item has been destroyed!
						pSock.SysMessage( GetDictionaryEntry( 311, pSock.language ).replace(/%s/gi, ourObj.name )); // Your %s has been destroyed.
						ourObj.Delete();
					}

					pSock.tempObj = bItem;
					pSock.CloseGump( gumpID, 0 );
					mChar.SetTempTag( "prevActionResult", "FAILREPAIR" );
					mChar.StartTimer( repairDelay, 1, true );
				}
			}
			else
			{
				pSock.tempObj = bItem;
				pSock.CloseGump( gumpID, 0 );
				mChar.SetTempTag( "prevActionResult", "FULLREPAIR" );
				mChar.StartTimer( repairDelay, 1, true );
			}
		}
		else
		{
			pSock.tempObj = bItem;
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "CHECKPACK" );
			mChar.StartTimer( repairDelay, 1, true );
		}
	}
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( pUser, timerID )
{
	if( !ValidateObject( pUser ))
		return;

	var socket = pUser.socket;

	if( timerID >= 1 && timerID <= 7 )
	{
		PageX( socket, pUser, timerID ); // Pages 1 - 7
	}
	else if( timerID == 8 )
	{
		Page8( socket, pUser );          // Ingot selection
	}
	else if( timerID == 30 )
	{
		Page30( socket, pUser );          // Scale selection
	}
	else if( timerID == 999 )
	{
		PageX( socket, pUser, 999 );     // Last Ten Blacksmith (if used)
	}
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	var usedMakeLast = false;

	if( !ValidateObject( pUser ) || pUser.dead )
		return;

	var bItem = pSock.tempObj;
	if( !ValidateObject( bItem ) || !pUser.InRange( bItem, 3 ))
	{
		pSock.SysMessage( GetDictionaryEntry( 461, pSock.language )); // You are too far away.
		return;
	}

	if( bItem.movable == 3 )
	{
		pSock.SysMessage( GetDictionaryEntry( 6031, pSock.language )); // Locked down resources cannot be used!
		return;
	}

	var iPackOwner = GetPackOwner( bItem, 0 );
	if( ValidateObject( iPackOwner ))
	{
		if( iPackOwner.serial != pUser.serial )
		{
			pSock.SysMessage( GetDictionaryEntry( 6032, pSock.language )); // That resource is in someone else's backpack!
			return;
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 6022, pSock.language )); // This has to be in your backpack before you can use it.
		return;
	}

	var gumpID = blacksmithID + 0xffff;

	// Close / Exit
	if( pButton == 0 )
	{
		pUser.SetTempTag( "prevActionResult", null );
		pUser.SetTempTag( "MAKELAST", null );
		pSock.CloseGump( gumpID, 0 );
		return;
	}

	// Repair Item
	if( pButton == 49 )
	{
		RepairTarget( pSock );
		return;
	}

	// Select Materials (ingots)
	if( pButton == 50 )
	{
		pSock.CloseGump( gumpID, 0 );
		Page8( pSock, pUser );
		return;
	}

	// Select Materials (scales)
	if( pButton == 51 )
	{
		pSock.CloseGump( gumpID, 0 );
		Page30( pSock, pUser );
		return;
	}

	// Smelt Item
	if( pButton == 52 )
	{
		SmeltTarget( pSock );
		return;
	}

	// Page buttons (1..7)
	if( pButton >= 1 && pButton <= 7 )
	{
		pUser.SetTempTag( "page", pButton );
		pUser.SetTempTag( "subPage", 1 );
		pSock.CloseGump( gumpID, 0 );
		PageX( pSock, pUser, pButton );
		return;
	}

	// Handle ore selection buttons (1000–1008) BEFORE the main switch
	if( pButton >= 1000 && pButton <= 1008 )
	{
		var index = pButton - 1000;            // 0..8
		var oreInfo = OreTypes[index];
		var miningSkill = pUser.skills.mining; // you are already using skills.mining in smelting logic

		if( miningSkill < oreInfo.minMining )
		{
			// Not enough mining skill for this ore
			pSock.CloseGump( gumpID, 0 );
			pUser.SetTempTag( "prevActionResult", "FAILED" );
			// Re-open ore page after a short delay
			pUser.StartTimer( ingotDelay, 8, true );
			return;
		}

		// Set selected ore and hue
		pUser.SetTempTag( "ORE", index );
		pUser.SetTempTag( "resourceHue", oreInfo.hue );
		pUser.SetTempTag( "MAKELAST", null );
		pUser.SetTempTag( "prevActionResult", null );

		// Re-open ore page so they see the new counts/selection
		pSock.CloseGump( gumpID, 0 );
		pUser.StartTimer( ingotDelay, 8, true );
		return;
	}

	// Handle scale selection buttons (1100–11XX) BEFORE the main switch
	if (pButton >= 1100 && pButton < 1100 + ScaleTypes.length)
	{
		var sIndex = pButton - 1100;        // 0..ScaleTypes.length-1
		var sInfo = ScaleTypes[sIndex];
		var smithSkill = pUser.skills.blacksmithing; // or mining / whatever you want to gate on

		if( smithSkill < sInfo.minSkill )
		{
			// Not enough skill for this scale type
			pSock.CloseGump( gumpID, 0 );
			pUser.SetTempTag( "prevActionResult", "FAILED" );
			pUser.StartTimer( ingotDelay, 30, true ); // reopen scale page after delay
			return;
		}

		// Store selection
		pUser.SetTempTag( "Scale", sIndex );
		pUser.SetTempTag( "resourceHue", sInfo.hue );
		pUser.SetTempTag( "MAKELAST", null );
		pUser.SetTempTag( "prevActionResult", null );

		// If you want a separate "make last" for scales, you could also SetTempTag("SCALEMAKELAST", ...)
		// For now we just store the current selection.

		pSock.CloseGump( gumpID, 0 );
		pUser.StartTimer( ingotDelay, 30, true ); // reopen PageScales to show updated selection/notice
		return;
	}

	// Last Ten page (if you wire it into the gump)
	if( pButton == 11000 )
	{
		pUser.SetTempTag( "page", 999 );
		pUser.SetTempTag( "subPage", 1 );
		PageX( pSock, pUser, 999 );
		return;
	}

	// Subpage navigation (8000 = prev, 9000 = next)
	if( pButton >= 8000 && pButton < 9000 )
	{
		var prevSub = pButton - 8000;
		var curPage = pUser.GetTempTag( "page" ) || 1;
		pUser.SetTempTag( "subPage", prevSub );
		PageX( pSock, pUser, curPage );
		return;
	}

	if( pButton >= 9000 && pButton < 10000 )
	{
		var nextSub = pButton - 9000;
		var curPage2 = pUser.GetTempTag( "page" ) || 1;
		pUser.SetTempTag( "subPage", nextSub );
		PageX( pSock, pUser, curPage2 );
		return;
	}

	// Handle "Make Last"
	if(( pButton >= 100 && pButton <= 799 ) || pButton == 5000 )
	{
		if( pButton == 5000 )
		{
			pButton = pUser.GetTempTag( "MAKELAST" );
			usedMakeLast = true;
		}
		else
		{
			pUser.SetTempTag( "MAKELAST", pButton );
		}
	}

	// Item detail buttons (2000 + buttonID)
	if( pButton >= 2000 && pButton < 3000 )
	{
		var detailButtonID = pButton - 2000;
		var entry = BlacksmithMap[detailButtonID];
		if( entry )
		{
			// For details we just pass the iron version (ore index 0) to 4026
			var ironMakeID = entry.oreMake[0] || 0;
			if( ironMakeID > 0 )
			{
				// Masonry uses Carpentry skill
				pUser.SetTempTag( "Skill", entry.skill | 0 );

				// Clear old harvests
				pUser.SetTempTag( "Harvest", null );
				pUser.SetTempTag( "Harvest2", null );
				pUser.SetTempTag( "Harvest3", null );
				pUser.SetTempTag( "Harvest4", null );

				// Clear old harvest names
				pUser.SetTempTag( "HarvestName", null );
				pUser.SetTempTag( "Harvest2Name", null );
				pUser.SetTempTag( "Harvest3Name", null );
				pUser.SetTempTag( "Harvest4Name", null );

				 // If this entry uses dragon scales, override resource label
                if( entry.useScales )
                {
                    // Use currently selected scale type (from Page30 selection)
                    var sIndex = pUser.GetTempTag( "Scale" );
                    if( sIndex < 0 || sIndex >= ScaleTypes.length )
                        sIndex = 0;

                    var sInfo  = ScaleTypes[sIndex];
                    var sLabel = GetDictionaryEntry( sInfo.nameDict, pSock.language );
                    pUser.SetTempTag( "HarvestName", sLabel );
                }
                else
                {
                    // Normal ingot-based items: use the harvest setup as before
                    if( entry.harvest && entry.harvest.length > 0 )
                    {
                        if( entry.harvest.length >= 1 )
                            pUser.SetTempTag( "Harvest", entry.harvest[0] );
                        if( entry.harvest.length >= 2 )
                            pUser.SetTempTag( "Harvest2", entry.harvest[1] );
                        if( entry.harvest.length >= 3 )
                            pUser.SetTempTag( "Harvest3", entry.harvest[2] );
                        if( entry.harvest.length >= 4 )
                            pUser.SetTempTag( "Harvest4", entry.harvest[3] );
                    }

                    // OPTIONAL custom names – these override the dictionary string
                    if( entry.harvestNames && entry.harvestNames.length > 0 )
                    {
                        if( entry.harvestNames.length >= 1 )
                            pUser.SetTempTag( "HarvestName", entry.harvestNames[0] );
                        if( entry.harvestNames.length >= 2 )
                            pUser.SetTempTag( "Harvest2Name", entry.harvestNames[1] );
                        if( entry.harvestNames.length >= 3 )
                            pUser.SetTempTag( "Harvest3Name", entry.harvestNames[2] );
                        if( entry.harvestNames.length >= 4 )
                            pUser.SetTempTag( "Harvest4Name", entry.harvestNames[3] );
                    }
                }

				if( entry.recipeID && entry.recipeID > 0 )
					pUser.SetTempTag( "needRecipeID", entry.recipeID );
				else
					pUser.SetTempTag( "needRecipeID", 0 );

				pUser.SetTempTag( "ITEMDETAILS", ironMakeID );
				TriggerEvent( itemDetailsScriptID, "ItemDetailGump", pUser );
			}
		}
		return;
	}

	// If this is a craft button in our map:
	if( BlacksmithMap[pButton] != undefined )
	{
		var entry2 = BlacksmithMap[pButton];
		var oreID;
		if( entry2.useScales )
		{
			// For dragon armor, index into oreMake[] by selected scale index
			oreID = pUser.GetTempTag( "Scale" );
		}
		else
		{
			oreID = pUser.GetTempTag( "ORE" );
		}
		var resourceHue = pUser.GetTempTag( "resourceHue" );

		// Ensure oreID within range
		if( oreID < 0 || oreID >= craftItems.length )
			oreID = 0;

		// Era / recipe gating
		if( !eraOK( entry2 ))
		{
			pSock.SysMessage( "That item is not available in this era." );
			return;
		}

		if( entry2.recipeID && !TriggerEvent( 4022, "NeedRecipe", pUser, entry2.recipeID ))
		{
			pSock.SysMessage( "You must learn that recipe from a scroll." );
			return;
		}

		// Check for nearby anvil
		var anvil = AreaItemFunction( "FindNearbyAnvils", pUser, 2, pSock );
		if( anvil == 0 )
		{
			pUser.SetTempTag( "prevActionResult", "NOANVIL" );
			pUser.StartTimer( gumpDelay, entry2.timerID, true );
			return;
		}

		// No colored weapons if disabled and using non-iron ingots
		if( !allowColouredWeapons && resourceHue > 0 && entry2.page > 3 )
		{
			pSock.SysMessage( "You cannot use colored ingots for weapons on this shard." );
			return;
		}

		var makeID = entry2.oreMake[oreID];
		if( !makeID || makeID == 0 )
		{
			// Fallback to iron version if for some reason we did not get a specific ore entry
			makeID = entry2.oreMake[0];
		}

		if( !makeID || makeID == 0 )
		{
			pSock.SysMessage( "That item is not properly configured." );
			return;
		}

		// Runic hammer bonus logic (unchanged from your original)
		pUser.AddScriptTrigger( 4033 );

		MakeItem( pSock, pUser, makeID, resourceHue );

		// Tool wear
		var toolUseLimit = GetServerSetting( "ToolUseLimit" );
		var toolUseBreak = GetServerSetting( "ToolUseBreak" );

		var runicHammer = pUser.FindItemLayer( 0x01 ); // Right Hand
		if( ValidateObject( runicHammer ) && runicHammer.GetTag( "runicHammer" ) && runicHammer.usesLeft > 0 )
		{
			pUser.SetTempTag( "usedRunicHammer", true );
			pUser.SetTempTag( "runicHammerType", runicHammer.color );

			if( toolUseLimit && runicHammer != bItem )
			{
				runicHammer.usesLeft -= 1;
				if( runicHammer.usesLeft == 0 && toolUseBreak )
				{
					runicHammer.Delete();
					pSock.SysMessage( GetDictionaryEntry( 10202, pSock.language ));
				}
			}
		}

		if( toolUseLimit )
		{
			bItem.usesLeft -= 1;
			if( bItem.usesLeft == 0 && toolUseBreak )
			{
				bItem.Delete();
				pSock.SysMessage( GetDictionaryEntry( 10202, pSock.language ));
			}
		}

		// Track in last ten list for blacksmith
		AddToLastTenBlacksmith( pUser, pButton );

		// Reopen page after delay
		pUser.StartTimer( gumpDelay, entry2.timerID, true );
		return;
	}

	// Any other buttons fall through and do nothing
}

function AddToLastTenBlacksmith( pUser, buttonID )
{
	var raw = pUser.GetTempTag( "LastTenBlacksmith" ) || "";
	var list = raw.split( "," );

	// Remove if already in list
	for( var i = 0; i < list.length; i++ )
	{
		if( parseInt( list[i] ) == buttonID )
		{
			list.splice( i, 1 );
			break;
		}
	}

	var newList = [buttonID];
	for( var j = 0; j < list.length && newList.length < 10; j++ )
	{
		var entry = parseInt( list[j] );
		if( !isNaN( entry ) && entry > 0 )
			newList.push( entry );
	}

	pUser.SetTempTag( "LastTenBlacksmith", newList.join( "," ) );
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

function eraOK( entry )
{
	// Optional per-entry gates. Strings like "lbr","aos","ml","sa","hs","tol".
	// If not present, the entry is valid for all eras.
	if( entry.minEra && coreShardEra < EraStringToNum( entry.minEra ))
		return false;
	if( entry.maxEra && coreShardEra > EraStringToNum( entry.maxEra ))
		return false;
	return true;
}