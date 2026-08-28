/// <reference path="../definitions.d.ts" />
// @ts-check

var LargeBODID = 5079;
var largeBODTag = "largeBODSerial";

var BODTypesToSkillNames = {
	1: "blacksmithing",
	2: "tailoring"
};

var BODSubtypeMinMax = {
	1: [1, 3],
	2: [1, 1]
};

var canCraftColouredWeapons = GetServerSetting( "CraftColouredWeapons" );

// Weapon CreateEntries Table
var weaponCreateEntries = [
	[20, 20, 20, 20, 20, 20, 20, 20, 20], // Dagger
	[40, 40, 40, 40, 40, 40, 40, 40, 40], // Mace
	[41, 41, 41, 41, 41, 41, 41, 41, 41], // Maul
	[21, 21, 21, 21, 21, 21, 21, 21, 21], // Cutlass
	[27, 27, 27, 27, 27, 27, 27, 27, 27], // Viking Sword
	[44, 44, 44, 44, 44, 44, 44, 44, 44], // Hammer Pick
	[26, 26, 26, 26, 26, 26, 26, 26, 26], // Longsword
	[42, 42, 42, 42, 42, 42, 42, 42, 42], // War Mace
	[32, 32, 32, 32, 32, 32, 32, 32, 32], // Double Axe
	[33, 33, 33, 33, 33, 33, 33, 33, 33], // Large Battle Axe
	[28, 28, 28, 28, 28, 28, 28, 28, 28], // Battle Axe
	[24, 24, 24, 24, 24, 24, 24, 24, 24], // Scimitar
	[31, 31, 31, 31, 31, 31, 31, 31, 31], // Two Handed Axe
	[43, 43, 43, 43, 43, 43, 43, 43, 43], // War Hammer
	[25, 25, 25, 25, 25, 25, 25, 25, 25], // Broadsword
	[29, 29, 29, 29, 29, 29, 29, 29, 29], // Axe
	[30, 30, 30, 30, 30, 30, 30, 30, 30], // Executioner's Axe
	[23, 23, 23, 23, 23, 23, 23, 23, 23], // Kryss
	[37, 37, 37, 37, 37, 37, 37, 37, 37], // War Fork
	[34, 34, 34, 34, 34, 34, 34, 34, 34], // War Axe
	[22, 22, 22, 22, 22, 22, 22, 22, 22], // Katana
	[35, 35, 35, 35, 35, 35, 35, 35, 35], // Short Spear
	[38, 38, 38, 38, 38, 38, 38, 38, 38], // Bardiche
	[36, 36, 36, 36, 36, 36, 36, 36, 36], // Spear
	[39, 39, 39, 39, 39, 39, 39, 39, 39]  // Halberd
];

var armorCreateEntries = [
	[1, 500, 600, 700, 800, 900, 1200, 1000, 1100],   // Buckler
	[2, 501, 601, 701, 801, 901, 1201, 1001, 1101],   // Bronze Shield
	[3, 502, 602, 702, 802, 902, 1202, 1002, 1102],   // Metal Shield
	[4, 503, 603, 703, 803, 903, 1203, 1003, 1103],   // Tear Kite Shield
	[5, 504, 604, 704, 804, 904, 1204, 1004, 1104],   // Metal Kite Shield
	[6, 505, 605, 705, 805, 905, 1205, 1005, 1105],   // Heater Shield
	[7, 506, 606, 706, 806, 906, 1206, 1006, 1106],   // Ringmail Gloves
	[8, 507, 607, 707, 807, 907, 1207, 1007, 1107],   // Ringmail Sleeves
	[9, 508, 608, 708, 808, 908, 1208, 1008, 1108],   // Ringmail Leggings
	[10, 509, 609, 709, 809, 909, 1209, 1009, 1109],  // Ringmail Tunic
	[11, 510, 610, 710, 810, 910, 1210, 1010, 1110],  // Chainmail Coif
	[12, 511, 611, 711, 811, 911, 1211, 1011, 1111],  // Chainmail Leggings
	[13, 512, 612, 712, 812, 912, 1212, 1012, 1112],  // Chainmail Tunic
	[14, 513, 613, 713, 813, 913, 1213, 1013, 1113],  // Platemail Gorget
	[15, 514, 614, 714, 814, 914, 1214, 1014, 1114],  // Platemail Gloves
	[16, 515, 615, 715, 815, 915, 1215, 1015, 1115],  // Platemail Arms
	[17, 516, 616, 716, 816, 916, 1216, 1016, 1116],  // Platemail Legs
	[18, 517, 617, 717, 817, 917, 1217, 1017, 1117],  // Platemail Tunic
	[19, 518, 618, 718, 818, 918, 1218, 1018, 1118],  // Platemail Female
	[45, 519, 619, 719, 819, 919, 1219, 1019, 1119],  // Helmet
	[46, 520, 620, 720, 820, 920, 1220, 1020, 1120],  // Bascinet
	[47, 521, 621, 721, 821, 921, 1221, 1021, 1121],  // Norse Helm
	[48, 522, 622, 722, 822, 922, 1222, 1022, 1122],  // Close Helm
	[49, 523, 623, 723, 823, 923, 1223, 1023, 1123]   // Plate Helm
];

var clothCreateEntries = [
	[130], [131], [132], [133], [134], [135], [136], [137], [138], [139],
	[140], [141], [142], [143], [144], [145], [146], [147], [148], [149],
	[150], [151], [152], [153], [154], [155], [156], [157], [158], [160]
];

var LargeBlacksmithWeaponGroups = {
	swords: [14, 3, 20, 6, 11, 4],
	axes: [15, 10, 8, 16, 9, 12],
	maces: [19, 5, 1, 2, 13, 7],
	fencing: [0, 21, 23, 18, 17],
	polearms: [22, 24]
};

var LargeBlacksmithArmorGroups = {
	ring: [6, 7, 8, 9],
	chain: [10, 11, 12],
	plate: [13, 14, 15, 16, 17]
};

var LargeTailorGroups = {
	hats: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11],
	shirts: [12, 13, 14, 15],
	outerwear: [16, 19, 20],
	dresses: [17, 18],
	legs: [22, 23, 24],
	sashesAprons: [25, 26, 27],
	misc: [28, 29]
};

/** @type { ( thingCreated: BaseObject, thingType: 0 | 1 ) => void } */
function onCreateDFN( objMade, objType )
{
	if( !ValidateObject( objMade ) || !objMade.isItem )
		return;

	if( objType != 0 )
		return;

	var bodType = objMade.GetTag( "bodType" );
	if( !BODSubtypeMinMax[bodType] )
		return;

	if( !objMade.GetTag( "bodSubtype" ) )
	{
		var bodSubtypeRange = BODSubtypeMinMax[bodType];
		objMade.SetTag( "bodSubtype", RandomNumber( bodSubtypeRange[0], bodSubtypeRange[1] ) );
	}

	if( !objMade.GetTag( "init" ) )
		objMade.SetTag( "init", 0 );

	if( !objMade.GetTag( "amountCur" ) )
		objMade.SetTag( "amountCur", 0 );

	if( !objMade.GetTag( "goldValue" ) )
		objMade.SetTag( "goldValue", 0 );

	if( !objMade.GetTag( "fameValue" ) )
		objMade.SetTag( "fameValue", 0 );

	if( !objMade.GetTag( "qualityValue" ) )
		objMade.SetTag( "qualityValue", 0 );

	if( !objMade.GetTag( "complete" ) )
		objMade.SetTag( "complete", 0 );

	objMade.Refresh();
}

/** @type { ( pUser: Character, largeBOD: Item ) => boolean } */
function onUseChecked( pUser, largeBOD )
{
	var socket = pUser.socket;
	if( socket == null )
		return false;

	if( !IsLargeBODAccessibleByUser( pUser, largeBOD ) )
	{
		socket.SysMessage( GetDictionaryEntry( 17259, socket.language ) );
		return false;
	}

	EnsureLargeBODDefinition( pUser, largeBOD );
	InitLargeBODTags( largeBOD );

	socket.CloseGump( LargeBODID + 0xffff, 0 );
	LargeBODGump( pUser, largeBOD );

	return false;
}

/** @param {Character} pUser @param {Item} largeBOD @returns {void} */
function EnsureLargeBODDefinition( pUser, largeBOD )
{
	if( !ValidateObject( pUser ) || !ValidateObject( largeBOD ) )
		return;

	if( largeBOD.GetTag( "init" ) && largeBOD.GetTag( "entryCount" ) > 0 )
		return;

	var bodType = largeBOD.GetTag( "bodType" );
	if( !BODSubtypeMinMax[bodType] )
		return;

	var bodSubtype = largeBOD.GetTag( "bodSubtype" );
	if( !bodSubtype )
	{
		var bodSubtypeRange = BODSubtypeMinMax[bodType];
		bodSubtype = RandomNumber( bodSubtypeRange[0], bodSubtypeRange[1] );
		largeBOD.SetTag( "bodSubtype", bodSubtype );
	}

	var playerSkill = pUser.skills[BODTypesToSkillNames[bodType]];
	var largeBODDef = BuildLargeBODDef( bodType, bodSubtype, playerSkill );
	if( largeBODDef == null )
		return;

	WriteLargeBODDefinition( largeBOD, largeBODDef );
}

/** @param {Item} largeBOD @param {{ bodType: number, bodSubtype: number, amountMax: number, reqExceptional: boolean, materialColor: number, entries: Array<{ itemName: string, graphicID: number, bodSectionID: string }> }} largeBODDef @returns {void} */
function WriteLargeBODDefinition( largeBOD, largeBODDef )
{
	var oldEntryCount = largeBOD.GetTag( "entryCount" );
	var entryIndex;

	for( entryIndex = 0; entryIndex < oldEntryCount; entryIndex++ )
	{
		largeBOD.SetTag( "entry" + entryIndex + "_itemName", null );
		largeBOD.SetTag( "entry" + entryIndex + "_graphicID", null );
		largeBOD.SetTag( "entry" + entryIndex + "_bodSectionID", null );
		largeBOD.SetTag( "entry" + entryIndex + "_amount", null );
	}

	largeBOD.SetTag( "bodType", largeBODDef.bodType );
	largeBOD.SetTag( "bodSubtype", largeBODDef.bodSubtype );
	largeBOD.SetTag( "amountMax", largeBODDef.amountMax );
	largeBOD.SetTag( "reqExceptional", largeBODDef.reqExceptional );
	largeBOD.SetTag( "materialColor", largeBODDef.materialColor );
	largeBOD.SetTag( "entryCount", largeBODDef.entries.length );
	largeBOD.SetTag( "amountCur", 0 );
	largeBOD.SetTag( "goldValue", 0 );
	largeBOD.SetTag( "fameValue", 0 );
	largeBOD.SetTag( "qualityValue", 0 );
	largeBOD.SetTag( "complete", 0 );

	for( entryIndex = 0; entryIndex < largeBODDef.entries.length; entryIndex++ )
	{
		var entry = largeBODDef.entries[entryIndex];
		largeBOD.SetTag( "entry" + entryIndex + "_itemName", entry.itemName );
		largeBOD.SetTag( "entry" + entryIndex + "_graphicID", entry.graphicID );
		largeBOD.SetTag( "entry" + entryIndex + "_bodSectionID", entry.bodSectionID );
		largeBOD.SetTag( "entry" + entryIndex + "_amount", 0 );
	}

	largeBOD.SetTag( "init", 1 );
	largeBOD.Refresh();
}

/** @param {Character} pUser @param {Item} largeBOD @returns {boolean} */
function IsLargeBODAccessibleByUser( pUser, largeBOD )
{
	if( !ValidateObject( pUser ) || !ValidateObject( largeBOD ) )
		return false;

	var packOwner = GetPackOwner( largeBOD, 0 );
	var inBackpack = ( ValidateObject( packOwner ) && packOwner.serial == pUser.serial );
	var lockedDown = IsLockedDownItem( largeBOD );

	return ( inBackpack || lockedDown );
}

/** @param {Item} item @returns {boolean} */
function IsLockedDownItem( item )
{
	if( !ValidateObject( item ) )
		return false;

	return ( item.movable == 2 || item.movable == 3 );
}

/** @param {Item} largeBOD @returns {void} */
function InitLargeBODTags( largeBOD )
{
	if( !largeBOD.GetTag( "goldValue" ) )
		largeBOD.SetTag( "goldValue", 0 );

	if( !largeBOD.GetTag( "fameValue" ) )
		largeBOD.SetTag( "fameValue", 0 );

	if( !largeBOD.GetTag( "qualityValue" ) )
		largeBOD.SetTag( "qualityValue", 0 );

	if( !largeBOD.GetTag( "amountCur" ) )
		largeBOD.SetTag( "amountCur", 0 );

	if( !largeBOD.GetTag( "complete" ) )
		largeBOD.SetTag( "complete", 0 );

	var entryCount = largeBOD.GetTag( "entryCount" );
	var entryIndex;

	for( entryIndex = 0; entryIndex < entryCount; entryIndex++ )
	{
		var entryAmountTag = "entry" + entryIndex + "_amount";
		if( !largeBOD.GetTag( entryAmountTag ) )
			largeBOD.SetTag( entryAmountTag, 0 );
	}

	largeBOD.Refresh();
}

/** @param {number} playerSkill @returns {{ amountMax: number, reqExceptional: boolean }} */
function GetLargeAmountMaximumAndExceptional( playerSkill )
{
	var amountMaximum = 0;
	var requiresExceptional = false;

	if( playerSkill >= 701 )
	{
		if( (( playerSkill + 800 ) / 2 ) > RandomNumber( 0, 1000 ) )
			requiresExceptional = true;

		var valuesOverSeventy = [10, 15, 20, 20];
		amountMaximum = valuesOverSeventy[Math.floor( Math.random() * valuesOverSeventy.length )];
	}
	else if( playerSkill >= 501 )
	{
		var valuesOverFifty = [10, 15, 15, 20];
		amountMaximum = valuesOverFifty[Math.floor( Math.random() * valuesOverFifty.length )];
	}
	else
	{
		var valuesLow = [10, 10, 15, 20];
		amountMaximum = valuesLow[Math.floor( Math.random() * valuesLow.length )];
	}

	return {
		amountMax: amountMaximum,
		reqExceptional: requiresExceptional
	};
}

/** @param {number} bodType @param {number} bodSubtype @returns {{ sourceTable: number[][], rowIndices: number[] }|null} */
function GetLargeGroupRowIndices( bodType, bodSubtype )
{
	if( bodType == 1 )
	{
		if( bodSubtype == 1 || bodSubtype == 3 )
		{
			var weaponKeys = Object.keys( LargeBlacksmithWeaponGroups );
			if( weaponKeys.length == 0 )
				return null;

			var pickedWeaponGroup = weaponKeys[Math.floor( Math.random() * weaponKeys.length )];
			return {
				sourceTable: weaponCreateEntries,
				rowIndices: LargeBlacksmithWeaponGroups[pickedWeaponGroup]
			};
		}
		else if( bodSubtype == 2 )
		{
			var armorKeys = Object.keys( LargeBlacksmithArmorGroups );
			if( armorKeys.length == 0 )
				return null;

			var pickedArmorGroup = armorKeys[Math.floor( Math.random() * armorKeys.length )];
			return {
				sourceTable: armorCreateEntries,
				rowIndices: LargeBlacksmithArmorGroups[pickedArmorGroup]
			};
		}
	}
	else if( bodType == 2 )
	{
		var tailorKeys = Object.keys( LargeTailorGroups );
		if( tailorKeys.length == 0 )
			return null;

		var pickedTailorGroup = tailorKeys[Math.floor( Math.random() * tailorKeys.length )];
		return {
			sourceTable: clothCreateEntries,
			rowIndices: LargeTailorGroups[pickedTailorGroup]
		};
	}

	return null;
}

/** @param {number[][]} sourceTable @param {number} bodType @param {number} playerSkill @returns {number} */
function GetLargeMaterialIndex( sourceTable, bodType, playerSkill )
{
	if( !sourceTable || sourceTable.length == 0 || !sourceTable[0] )
		return 0;

	var maxMaterialIndex = sourceTable[0].length - 1;
	var materialIndex = 0;

	if( bodType == 1 && canCraftColouredWeapons )
	{
		materialIndex = RandomNumber( 0, maxMaterialIndex );

		if( playerSkill < 700 )
		{
			materialIndex = 0;
		}
		else
		{
			var minimumSkillForMaterial = 1000 - ( 50 * ( maxMaterialIndex - materialIndex ) );
			if( playerSkill < minimumSkillForMaterial )
				materialIndex = 0;
		}
	}

	return materialIndex;
}

/** @param {number} bodType @param {number} bodSubtype @param {number} playerSkill @returns {{ bodType: number, bodSubtype: number, amountMax: number, reqExceptional: boolean, materialColor: number, entries: Array<{ itemName: string, graphicID: number, bodSectionID: string }> }|null} */
function BuildLargeBODDef( bodType, bodSubtype, playerSkill )
{
	var groupInfo = GetLargeGroupRowIndices( bodType, bodSubtype );
	if( groupInfo == null )
		return null;

	var sourceTable = groupInfo.sourceTable;
	var rowIndices = groupInfo.rowIndices;
	if( !sourceTable || !rowIndices || rowIndices.length == 0 )
		return null;

	var amountData = GetLargeAmountMaximumAndExceptional( playerSkill );
	var materialIndex = GetLargeMaterialIndex( sourceTable, bodType, playerSkill );

	if( typeof sourceTable[rowIndices[0]] == "undefined" )
		return null;

	var firstCreateIndex = sourceTable[rowIndices[0]][materialIndex];
	var firstCreateEntry = CreateEntries[firstCreateIndex];
	if( !firstCreateEntry )
		return null;

	var materialColor = 0;
	if( firstCreateEntry.resources && firstCreateEntry.resources[0] )
		materialColor = firstCreateEntry.resources[0][1];

	var entries = [];
	var rowIndexPosition;

	for( rowIndexPosition = 0; rowIndexPosition < rowIndices.length; rowIndexPosition++ )
	{
		var rowIndex = rowIndices[rowIndexPosition];
		if( typeof sourceTable[rowIndex] == "undefined" )
			continue;

		var createIndex = sourceTable[rowIndex][materialIndex];
		var createEntry = CreateEntries[createIndex];
		if( !createEntry )
			continue;

		entries.push({
			itemName: createEntry.name ? createEntry.name : "Unknown item",
			graphicID: createEntry.id ? createEntry.id : 0,
			bodSectionID: createEntry.addItem ? createEntry.addItem : ""
		});
	}

	if( entries.length == 0 )
		return null;

	return {
		bodType: bodType,
		bodSubtype: bodSubtype,
		amountMax: amountData.amountMax,
		reqExceptional: amountData.reqExceptional,
		materialColor: materialColor,
		entries: entries
	};
}

/** @param {Character} pUser @param {Item} largeBOD @returns {void} */
function LargeBODGump( pUser, largeBOD )
{
	var socket = pUser.socket;
	if( socket == null )
		return;

	StoreLargeBOD( pUser, largeBOD );

	var amountMaximum = largeBOD.GetTag( "amountMax" );
	var requiresExceptional = largeBOD.GetTag( "reqExceptional" );
	var materialColor = largeBOD.GetTag( "materialColor" );
	var entryCount = largeBOD.GetTag( "entryCount" );

	var largeBODGump = new Gump();
	largeBODGump.AddPage( 0 );

	var extraHeight = 0;
	if( requiresExceptional || materialColor > 0 )
	{
		extraHeight += 24;
		if( requiresExceptional )
			extraHeight += 24;
		if( materialColor > 0 )
			extraHeight += 24;
	}

	var totalHeight = 218 + extraHeight + ( entryCount * 24 );

	largeBODGump.AddBackground( 50, 10, 455, totalHeight, 5054 );
	largeBODGump.AddTiledGump( 58, 20, 438, totalHeight - 18, 2624 );
	largeBODGump.AddCheckerTrans( 58, 20, 438, totalHeight - 18 );

	largeBODGump.AddGump( 45, 5, 10460 );
	largeBODGump.AddGump( 480, 5, 10460 );
	largeBODGump.AddGump( 45, totalHeight - 15, 10460 );
	largeBODGump.AddGump( 480, totalHeight - 15, 10460 );

	largeBODGump.AddHTMLGump( 225, 25, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17285, socket.language ) + "</basefont>" );
	largeBODGump.AddHTMLGump( 75, 48, 250, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17251, socket.language ) + "</basefont>" );
	largeBODGump.AddText( 275, 48, 1152, String( amountMaximum ) );

	largeBODGump.AddHTMLGump( 75, 72, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17253, socket.language ) + "</basefont>" );
	largeBODGump.AddHTMLGump( 275, 76, 200, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17252, socket.language ) + "</basefont>" );

	var entryIndex;
	var yPosition = 96;

	for( entryIndex = 0; entryIndex < entryCount; entryIndex++ )
	{
		var itemName = largeBOD.GetTag( "entry" + entryIndex + "_itemName" );
		var entryAmount = largeBOD.GetTag( "entry" + entryIndex + "_amount" );

		largeBODGump.AddHTMLGump( 75, yPosition, 210, 20, false, false, "<basefont color=#ffffff>" + itemName + "</basefont>" );
		largeBODGump.AddText( 275, yPosition, 0x480, String( entryAmount ) );
		yPosition += 24;
	}

	if( requiresExceptional || materialColor > 0 )
	{
		largeBODGump.AddHTMLGump( 75, yPosition, 200, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17255, socket.language ) + "</basefont>" );
		yPosition += 24;
	}

	if( requiresExceptional )
	{
		largeBODGump.AddHTMLGump( 75, yPosition, 300, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17256, socket.language ) + "</basefont>" );
		yPosition += 24;
	}

	if( materialColor > 0 )
	{
		var materialName = GetMaterialNameFromHue( materialColor );
		if( materialName != "" )
		{
			largeBODGump.AddHTMLGump( 75, yPosition, 300, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17257, socket.language ) + " " + materialName + " material</basefont>" );
			yPosition += 24;
		}
	}

	largeBODGump.AddButton( 125, yPosition, 4005, 4007, 1, 0, 2 );
	largeBODGump.AddHTMLGump( 160, yPosition, 300, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17276, socket.language ) + "</basefont>" );
	yPosition += 24;

	largeBODGump.AddButton( 125, yPosition, 4005, 4007, 1, 0, 1 );
	largeBODGump.AddHTMLGump( 160, yPosition, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10290, socket.language ) + "</basefont>" );

	largeBODGump.Send( socket );
	largeBODGump.Free();
}

/** @param {number} hue @returns {string} */
function GetMaterialNameFromHue( hue )
{
	switch( hue )
	{
		case 0: return "iron";
		case 2419: return "dull copper";
		case 2406: return "shadow iron";
		case 2414: return "copper";
		case 1750: return "bronze";
		case 2213: return "gold";
		case 2425: return "agapite";
		case 2207: return "verite";
		case 2219: return "valorite";
		default: return "";
	}
}

/** @type { ( socket: Socket, pButton: number, gumpData: GumpData ) => void } */
function onGumpPress( socket, pButton, gumpData )
{
	if( socket == null )
		return;

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	switch( pButton )
	{
		case 0:
		case 1:
			ClearStoredLargeBOD( pUser );
			break;

		case 2:
		{
			var largeBOD = GetStoredLargeBOD( pUser );
			if( !ValidateObject( largeBOD ) )
			{
				ClearStoredLargeBOD( pUser );
				return;
			}

			BeginLargeBODCombine( pUser, largeBOD );
			break;
		}
	}
}

/** @param {Character} pUser @param {Item} largeBOD @returns {void} */
function StoreLargeBOD( pUser, largeBOD )
{
	if( !ValidateObject( pUser ) || !ValidateObject( largeBOD ) )
		return;

	pUser.SetTempTag( largeBODTag, largeBOD.serial );
}

/** @param {Character} pUser @returns {void} */
function ClearStoredLargeBOD( pUser )
{
	if( !ValidateObject( pUser ) )
		return;

	pUser.SetTempTag( largeBODTag, null );
}

/** @param {Character} pUser @returns {Item|null} */
function GetStoredLargeBOD( pUser )
{
	if( !ValidateObject( pUser ) )
		return null;

	var largeBODSerial = pUser.GetTempTag( largeBODTag );
	if( !largeBODSerial )
		return null;

	var largeBOD = CalcItemFromSer( largeBODSerial );
	if( !ValidateObject( largeBOD ) )
		return null;

	return largeBOD;
}

/** @param {Character} pUser @param {Item} largeBOD @returns {void} */
function BeginLargeBODCombine( pUser, largeBOD )
{
	var socket = pUser.socket;
	if( socket == null )
		return;

	if( !IsLargeBODAccessibleByUser( pUser, largeBOD ) )
	{
		socket.SysMessage( GetDictionaryEntry( 17259, socket.language ) );
		ClearStoredLargeBOD( pUser );
		return;
	}

	if( IsLargeBODComplete( largeBOD ) )
	{
		pUser.TextMessage( GetDictionaryEntry( 17283, socket.language ), false, 0x3b2, 0, pUser.serial );
		ClearStoredLargeBOD( pUser );
		return;
	}

	StoreLargeBOD( pUser, largeBOD );
	pUser.CustomTarget( 0 );
}

/** @param {Item} largeBOD @returns {boolean} */
function IsLargeBODComplete( largeBOD )
{
	var amountMaximum = largeBOD.GetTag( "amountMax" );
	var entryCount = largeBOD.GetTag( "entryCount" );
	var entryIndex;

	for( entryIndex = 0; entryIndex < entryCount; entryIndex++ )
	{
		if( largeBOD.GetTag( "entry" + entryIndex + "_amount" ) < amountMaximum )
			return false;
	}

	return true;
}

/** @type { ( socket: Socket, myTarget: BaseObject | null ) => void } */
function onCallback0( socket, myTarget )
{
	if( socket == null )
		return;

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var largeBOD = GetStoredLargeBOD( pUser );
	var gumpID = LargeBODID + 0xffff;

	ClearStoredLargeBOD( pUser );

	if( !ValidateObject( largeBOD ) )
		return;

	if( !IsLargeBODAccessibleByUser( pUser, largeBOD ) )
		return;

	var targetWasCancelled = ( socket.GetByte( 11 ) == 255 );
	if( targetWasCancelled || !ValidateObject( myTarget ) )
	{
		socket.CloseGump( gumpID, 0 );
		LargeBODGump( pUser, largeBOD );
		return;
	}

	if( !myTarget.isItem )
	{
		socket.SysMessage( GetDictionaryEntry( 17277, socket.language ) );
		socket.CloseGump( gumpID, 0 );
		LargeBODGump( pUser, largeBOD );
		return;
	}

	/** @type {Item} */
	var smallBOD = myTarget;

	if( typeof smallBOD.sectionID != "string" || smallBOD.sectionID.indexOf( "smallbod_" ) != 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 17277, socket.language ) );
		socket.CloseGump( gumpID, 0 );
		LargeBODGump( pUser, largeBOD );
		return;
	}

	var packOwner = GetPackOwner( smallBOD, 0 );
	if( !ValidateObject( packOwner ) || packOwner.serial != pUser.serial )
	{
		socket.SysMessage( GetDictionaryEntry( 17259, socket.language ) );
		socket.CloseGump( gumpID, 0 );
		LargeBODGump( pUser, largeBOD );
		return;
	}

	CombineSmallIntoLarge( pUser, socket, largeBOD, smallBOD );

	socket.CloseGump( gumpID, 0 );
	LargeBODGump( pUser, largeBOD );
}

/** @param {Character} pUser @param {Socket} socket @param {Item} largeBOD @param {Item} smallBOD @returns {void} */
function CombineSmallIntoLarge( pUser, socket, largeBOD, smallBOD )
{
	if( socket == null )
		return;

	var amountMaximumLarge = largeBOD.GetTag( "amountMax" );
	var requiresExceptionalLarge = largeBOD.GetTag( "reqExceptional" );
	var materialColorLarge = largeBOD.GetTag( "materialColor" );
	var bodTypeLarge = largeBOD.GetTag( "bodType" );

	var amountMaximumSmall = smallBOD.GetTag( "amountMax" );
	var amountCurrentSmall = smallBOD.GetTag( "amountCur" );
	var requiresExceptionalSmall = smallBOD.GetTag( "reqExceptional" );
	var materialColorSmall = smallBOD.GetTag( "materialColor" );
	var bodTypeSmall = smallBOD.GetTag( "bodType" );

	if( bodTypeSmall != bodTypeLarge )
	{
		socket.SysMessage( GetDictionaryEntry( 17272, socket.language ) );
		return;
	}

	if( requiresExceptionalLarge && !requiresExceptionalSmall )
	{
		socket.SysMessage( GetDictionaryEntry( 17278, socket.language ) );
		return;
	}

	if( materialColorLarge > 0 && materialColorLarge != materialColorSmall )
	{
		socket.SysMessage( GetDictionaryEntry( 17279, socket.language ) );
		return;
	}

	if( amountMaximumLarge != amountMaximumSmall )
	{
		socket.SysMessage( GetDictionaryEntry( 17280, socket.language ) );
		return;
	}

	if( amountCurrentSmall < amountMaximumSmall )
	{
		socket.SysMessage( GetDictionaryEntry( 17281, socket.language ) );
		return;
	}

	var entryCount = largeBOD.GetTag( "entryCount" );
	var entryIndex;
	var allEntriesAreFull = true;

	for( entryIndex = 0; entryIndex < entryCount; entryIndex++ )
	{
		if( largeBOD.GetTag( "entry" + entryIndex + "_amount" ) < amountMaximumLarge )
		{
			allEntriesAreFull = false;
			break;
		}
	}

	if( allEntriesAreFull )
	{
		socket.SysMessage( GetDictionaryEntry( 17283, socket.language ) );
		return;
	}

	var smallBODItemName = smallBOD.GetTag( "itemName" );
	var matchingEntryIndex = -1;

	for( entryIndex = 0; entryIndex < entryCount; entryIndex++ )
	{
		var entryItemName = largeBOD.GetTag( "entry" + entryIndex + "_itemName" );

		if( entryItemName == smallBODItemName )
		{
			matchingEntryIndex = entryIndex;
			break;
		}
	}

	if( matchingEntryIndex == -1 )
	{
		socket.SysMessage( GetDictionaryEntry( 17282, socket.language ) );
		return;
	}

	var currentEntryAmount = largeBOD.GetTag( "entry" + matchingEntryIndex + "_amount" );
	if( currentEntryAmount >= amountMaximumLarge )
	{
		socket.SysMessage( GetDictionaryEntry( 17283, socket.language ) );
		return;
	}

	currentEntryAmount += amountCurrentSmall;
	if( currentEntryAmount > amountMaximumLarge )
		currentEntryAmount = amountMaximumLarge;

	largeBOD.SetTag( "entry" + matchingEntryIndex + "_amount", currentEntryAmount );

	var goldLarge = largeBOD.GetTag( "goldValue" );
	var fameLarge = largeBOD.GetTag( "fameValue" );
	var qualityLarge = largeBOD.GetTag( "qualityValue" );

	var goldSmall = smallBOD.GetTag( "goldValue" );
	var fameSmall = smallBOD.GetTag( "fameValue" );
	var qualitySmall = smallBOD.GetTag( "qualityValue" );

	largeBOD.SetTag( "goldValue", goldLarge + goldSmall );
	largeBOD.SetTag( "fameValue", fameLarge + fameSmall );
	largeBOD.SetTag( "qualityValue", qualityLarge + qualitySmall );

	UpdateLargeBODCurrentAmount( largeBOD );

	var nowAllEntriesAreFull = true;
	for( entryIndex = 0; entryIndex < entryCount; entryIndex++ )
	{
		if( largeBOD.GetTag( "entry" + entryIndex + "_amount" ) < amountMaximumLarge )
		{
			nowAllEntriesAreFull = false;
			break;
		}
	}

	if( nowAllEntriesAreFull )
		largeBOD.SetTag( "complete", 1 );
	else
		largeBOD.SetTag( "complete", 0 );

	largeBOD.Refresh();
	smallBOD.Delete();

	socket.SysMessage( GetDictionaryEntry( 17284, socket.language ) );
}

/** @param {Item} largeBOD @returns {void} */
function UpdateLargeBODCurrentAmount( largeBOD )
{
	var entryCount = largeBOD.GetTag( "entryCount" );
	var totalAmountCurrent = 0;
	var entryIndex;

	for( entryIndex = 0; entryIndex < entryCount; entryIndex++ )
		totalAmountCurrent += largeBOD.GetTag( "entry" + entryIndex + "_amount" );

	largeBOD.SetTag( "amountCur", totalAmountCurrent );
}

/** @type { ( largeBOD: BaseObject, pSocket: Socket ) => string } */
function onTooltip( largeBOD, pSocket )
{
	if( !ValidateObject( largeBOD ) || !largeBOD.isItem )
		return "";

	var amountMaximum = largeBOD.GetTag( "amountMax" );
	var entryCount = largeBOD.GetTag( "entryCount" );

	if( amountMaximum <= 0 || entryCount <= 0 )
		return "";

	var tooltipText = "Large bulk order\n";
	tooltipText += "Amount to make per item: " + amountMaximum;

	var entryIndex;
	for( entryIndex = 0; entryIndex < entryCount; entryIndex++ )
	{
		var itemName = largeBOD.GetTag( "entry" + entryIndex + "_itemName" );
		var entryAmount = largeBOD.GetTag( "entry" + entryIndex + "_amount" );
		tooltipText += "\n" + itemName + " : " + entryAmount + "/" + amountMaximum;
	}

	return tooltipText;
}