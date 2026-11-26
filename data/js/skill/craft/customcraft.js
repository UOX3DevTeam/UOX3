/// <reference path="../../definitions.d.ts" />
// @ts-check

var textHue = 0x480;
var customCraftID = 4040;
var craftMapRegistryID = 4038;
var itemDetailsScriptID = 4026;
var itemsPerPage = 10;
var displayUnlearnedRecipes = true;
var coreShardEra = EraStringToNum( GetServerSetting( "CoreShardEra" ) );

var customCraftDefinitions = null;
var customCraftDefinitionsLoaded = false;
var customCraftDefinitionsLoadError = false;
var CustomCraftMap = {};
var loadedCustomCraftName = "";

/** @type { () => object|null } */
function LoadCustomCraftDefinitions()
{
	if( customCraftDefinitionsLoaded )
		return customCraftDefinitions;

	customCraftDefinitions = null;
	customCraftDefinitionsLoaded = false;
	customCraftDefinitionsLoadError = false;

	var customCraftFile = new UOXCFile();
	customCraftFile.Open( "customcrafts.json", "r", "crafting", true );

	if( customCraftFile == null || customCraftFile.Length() < 0 )
	{
		Console.Error( "CustomCraft system: Unable to open js/jsdata/crafting/customcrafts.json" );
		customCraftDefinitionsLoadError = true;
		return null;
	}

	var fileText = "";
	while( !customCraftFile.EOF() )
	{
		var rawLine = customCraftFile.ReadUntil( "\n" );
		if( rawLine != null && typeof( rawLine ) != "undefined" )
		{
			fileText += rawLine;
		}
	}

	customCraftFile.Close();
	customCraftFile.Free();

	fileText = SanitizeCustomCraftJsonText( fileText );

	try
	{
		customCraftDefinitions = JSON.parse( fileText );
	}
	catch( error )
	{
		Console.Error( "CustomCraft system: Failed to parse customcrafts.json: " + error );
		customCraftDefinitionsLoadError = true;
		return null;
	}

	if( !IsCustomCraftArrayValue( customCraftDefinitions ))
	{
		Console.Error( "CustomCraft system: customcrafts.json must contain a JSON array." );
		customCraftDefinitionsLoadError = true;
		return null;
	}

	customCraftDefinitionsLoaded = true;
	Console.Print( "CustomCraft system: Loaded " + customCraftDefinitions.length + " custom craft definitions.\n" );

	return customCraftDefinitions;
}

/** @type { ( craftName: string ) => object|null } */
function GetCustomCraftDefinition( craftName )
{
	var definitions = LoadCustomCraftDefinitions();

	if( !definitions )
		return null;

	for( var i = 0; i < definitions.length; i++ )
	{
		if( definitions[i] && definitions[i].craft == craftName )
			return definitions[i];
	}

	return null;
}

/** @type { ( pUser: Character ) => object|null } */
function GetActiveCustomCraftDefinition( pUser )
{
	var craftName = pUser.GetTempTag( "CUSTOMCRAFT" );

	if( !craftName )
		return null;

	return GetCustomCraftDefinition( craftName );
}

/** @type { ( craftDefinition: object ) => boolean } */
function LoadCustomCraftMap( craftDefinition )
{
	CustomCraftMap = {};
	loadedCustomCraftName = "";

	if( !craftDefinition )
		return false;

	var mapFile = craftDefinition.mapFile || craftDefinition.craft;
	var craftEntries = TriggerEvent( craftMapRegistryID, "CraftMapRegistry", mapFile );

	if( !craftEntries || !IsCustomCraftArrayValue( craftEntries ))
	{
		Console.Warning( "CustomCraft system: Unable to load custom craft map data for " + mapFile + "." );
		return false;
	}

	for( var i = 0; i < craftEntries.length; i++ )
	{
		var entry = craftEntries[i];

		if( !entry || typeof entry.makeID == "undefined" )
			continue;

		if( entry.skill === undefined && typeof craftDefinition.skillID != "undefined" )
			entry.skill = craftDefinition.skillID;

		CustomCraftMap[entry.makeID] = entry;
	}

	Console.Print( "CustomCraft system: Loaded " + craftEntries.length + " entries for " + mapFile + ".\n" );
	loadedCustomCraftName = craftDefinition.craft;
	return true;
}

/** @type { ( socket: Socket, pUser: Character, pageNum: number ) => void } */
function PageX( socket, pUser, pageNum )
{
	if( !socket || !ValidateObject( pUser ))
		return;

	var craftDefinition = GetActiveCustomCraftDefinition( pUser );

	if( !craftDefinition )
	{
		socket.SysMessage( "Custom craft definition was not found." );
		return;
	}

	if( loadedCustomCraftName != craftDefinition.craft || !CustomCraftMap || Object.keys( CustomCraftMap ).length == 0 )
	{
		if( !LoadCustomCraftMap( craftDefinition ))
		{
			socket.SysMessage( "Custom craft map failed to load." );
			return;
		}
	}

	var pageItems = [];

	if( pageNum == 999 )
	{
		var lastTenRaw = pUser.GetTempTag( "LastTenCustomCraft_" + craftDefinition.craft ) || "";
		var split = lastTenRaw.split( "," );

		for( var i = 0; i < split.length; i++ )
		{
			var value = parseInt( split[i], 10 );
			if( !isNaN( value ) && CustomCraftMap[value] )
				pageItems.push( value );
		}
	}
	else
	{
		for( var makeIDStr in CustomCraftMap )
		{
			if( !CustomCraftMap.hasOwnProperty( makeIDStr ))
				continue;

			var makeID = parseInt( makeIDStr, 10 );
			var data = CustomCraftMap[makeID];

			if( data.page == pageNum && EraOK( data ))
			{
				var needsRecipe = data.recipeID;
				var showAll = displayUnlearnedRecipes;

				if( !needsRecipe || showAll || HasLearnedRecipe( pUser, needsRecipe ))
					pageItems.push( makeID );
			}
		}

		pageItems.sort( function( a, b )
		{
			var entryA = CustomCraftMap[a];
			var entryB = CustomCraftMap[b];

			if( entryA && entryB )
			{
				if( entryA.sortOrder != undefined && entryB.sortOrder != undefined )
					return entryA.sortOrder - entryB.sortOrder;

				if( entryA.dictID && entryB.dictID )
					return entryA.dictID - entryB.dictID;
			}

			return a - b;
		});
	}

	var subPage = pUser.GetTempTag( "subPage" ) || 1;
	var totalSubPages = Math.ceil( pageItems.length / itemsPerPage );

	if( totalSubPages < 1 )
		totalSubPages = 1;

	if( subPage < 1 )
		subPage = 1;

	if( subPage > totalSubPages )
		subPage = totalSubPages;

	pUser.SetTempTag( "page", pageNum );
	pUser.SetTempTag( "subPage", subPage );

	var startIndex = ( subPage - 1 ) * itemsPerPage;
	var endIndex = Math.min( startIndex + itemsPerPage, pageItems.length );

	var customGump = new Gump();
	BuildCustomCraftFrame( customGump, socket, pUser, craftDefinition, pageNum );

	for( var j = startIndex; j < endIndex; j++ )
	{
		var index = j - startIndex;
		var makeEntryID = pageItems[j];
		var entry = CustomCraftMap[makeEntryID];
		var entryText = GetCustomCraftEntryText( entry, socket );

		customGump.AddButton( 220, 60 + ( index * 20 ), 4005, 4007, 1, 0, makeEntryID );
		customGump.AddText( 255, 60 + ( index * 20 ), textHue, entryText );
		customGump.AddButton( 480, 60 + ( index * 20 ), 4011, 4012, 1, 0, 20000 + makeEntryID );
	}

	if( pageItems.length == 0 )
		customGump.AddText( 220, 60, textHue, "No items available on this page." );

	if( subPage > 1 )
	{
		customGump.AddButton( 220, 260, 4014, 4015, 1, 0, 8000 + ( subPage - 1 ));
		customGump.AddHTMLGump( 255, 263, 100, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" );
	}

	if( subPage < totalSubPages )
	{
		customGump.AddButton( 370, 260, 4005, 4007, 1, 0, 9000 + ( subPage + 1 ));
		customGump.AddHTMLGump( 405, 263, 100, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" );
	}

	customGump.Send( socket );
	customGump.Free();
}

/** @type { ( customGump: Gump, socket: Socket, pUser: Character, craftDefinition: object, pageNum: number ) => void } */
function BuildCustomCraftFrame( customGump, socket, pUser, craftDefinition, pageNum )
{
	customGump.AddPage( 0 );
	customGump.AddBackground( 0, 0, 530, 437, 5054 );
	customGump.AddTiledGump( 10, 10, 510, 22, 2624 );
	customGump.AddTiledGump( 10, 292, 150, 45, 2624 );
	customGump.AddTiledGump( 165, 292, 355, 45, 2624 );
	customGump.AddTiledGump( 10, 342, 510, 85, 2624 );
	customGump.AddTiledGump( 10, 37, 200, 250, 2624 );
	customGump.AddTiledGump( 215, 37, 305, 250, 2624 );
	customGump.AddCheckerTrans( 10, 10, 510, 417 );

	var menuName = craftDefinition.menuName || craftDefinition.craft || "Custom Craft";

	customGump.AddHTMLGump( 0, 12, 530, 20, 0, 0, "<center><basefont color=#ffffff>" + menuName + "</basefont></center>" );
	customGump.AddHTMLGump( 0, 37, 220, 22, 0, 0, "<center><basefont color=#ffffff>" + GetDictionaryEntry( 10286, socket.language ) + "</basefont></center>" );
	customGump.AddHTMLGump( 230, 37, 280, 22, 0, 0, "<center><basefont color=#ffffff>" + GetDictionaryEntry( 10287, socket.language ) + "</basefont></center>" );
	customGump.AddHTMLGump( 10, 302, 150, 25, 0, 0, "<center><basefont color=#ffffff>" + GetDictionaryEntry( 10288, socket.language ) + "</basefont></center>" );

	var categories = craftDefinition.categories || [];

	for( var i = 0; i < categories.length; i++ )
	{
		var category = categories[i];
		var categoryPage = category.page || ( i + 1 );
		var categoryText = category.label || ( "Page " + categoryPage );

		customGump.AddButton( 20, 60 + ( i * 20 ), 4005, 4007, 1, 0, 10000 + categoryPage );
		customGump.AddText( 55, 60 + ( i * 20 ), textHue, categoryText );
	}

	if( craftDefinition.showLastTen )
	{
		customGump.AddButton( 20, 250, 4005, 4007, 1, 0, 10999 );
		customGump.AddText( 55, 250, textHue, "Last Ten" );
	}

	if( craftDefinition.allowRepair )
	{
		customGump.AddButton( 20, 345, 4005, 4007, 1, 0, 90001 );
		customGump.AddText( 55, 345, textHue, GetCustomCraftText( craftDefinition, "repairText", "Repair Item" ) );
	}

	if( craftDefinition.allowRecycle )
	{
		customGump.AddButton( 20, 365, 4005, 4007, 1, 0, 90002 );
		customGump.AddText( 55, 365, textHue, GetCustomCraftText( craftDefinition, "recycleText", "Recycle Item" ) );
	}
}

/** @type { ( entry: object, socket: Socket ) => string } */
function GetCustomCraftEntryText( entry, socket )
{
	if( !entry )
		return "[Missing Entry]";

	if( entry.customName )
		return entry.customName;

	if( entry.dictID )
	{
		var dictText = GetDictionaryEntry( entry.dictID, socket.language );
		if( dictText && dictText != "" )
			return dictText;
	}

	if( entry.label )
		return entry.label;

	return "[Unnamed Item: " + entry.makeID + "]";
}

/** @type { ( pSock: Socket, pButton: number, gumpData: any ) => void } */
function onGumpPress( pSock, pButton, gumpData )
{
	if( !pSock )
		return;

	var pUser = pSock.currentChar;

	if( !ValidateObject( pUser ))
		return;

	var craftDefinition = GetActiveCustomCraftDefinition( pUser );

	if( !craftDefinition )
		return;

	if( loadedCustomCraftName != craftDefinition.craft || !CustomCraftMap || Object.keys( CustomCraftMap ).length == 0 )
	{
		if( !LoadCustomCraftMap( craftDefinition ))
			return;
	}

	if( pButton == 0 )
	{
		pSock.CloseGump( customCraftID + 0xffff, 0 );
		return;
	}

	if( craftDefinition.allowRepair && pButton == 90001 )
	{
		pSock.CustomTarget( 0, GetCustomCraftText( craftDefinition, "repairTargetText", "Select the item you want to repair." ) );
		return;
	}

	if( craftDefinition.allowRecycle && pButton == 90002 )
	{
		pSock.CustomTarget( 1, GetCustomCraftText( craftDefinition, "recycleTargetText", "Select the item you want to recycle." ) );
		return;
	}

	if( pButton >= 10000 && pButton < 11000 )
	{
		if( pButton == 10999 )
			PageX( pSock, pUser, 999 );
		else
			PageX( pSock, pUser, pButton - 10000 );

		return;
	}

	if( pButton >= 8000 && pButton < 9000 )
	{
		pUser.SetTempTag( "subPage", pButton - 8000 );
		PageX( pSock, pUser, pUser.GetTempTag( "page" ) || 1 );
		return;
	}

	if( pButton >= 9000 && pButton < 10000 )
	{
		pUser.SetTempTag( "subPage", pButton - 9000 );
		PageX( pSock, pUser, pUser.GetTempTag( "page" ) || 1 );
		return;
	}

	if( pButton >= 20000 )
	{
		ShowCustomCraftItemDetails( pSock, pUser, pButton - 20000 );
		return;
	}

	var entry = CustomCraftMap[pButton];

	if( !entry )
		return;

	if( !EraOK( entry ))
	{
		pSock.SysMessage( "That item is not available in this era." );
		return;
	}

	if( entry.recipeID && !HasLearnedRecipe( pUser, entry.recipeID ))
	{
		TriggerEvent( 4022, "NeedRecipe", pSock, entry.recipeID );
		return;
	}

	pUser.SetTempTag( "MAKELAST", pButton );
	StoreLastTenCustomCraft( pUser, craftDefinition, pButton );

	pSock.CloseGump( customCraftID + 0xffff, 0 );
	MakeItem( pSock, pUser, pButton );
	pUser.StartTimer( 2000, entry.timerID || entry.page || 1, true );
}

/** @type { ( pSock: Socket, pUser: Character, makeID: number ) => void } */
function ShowCustomCraftItemDetails( pSock, pUser, makeID )
{
	var entry = CustomCraftMap[makeID];

	if( !entry )
		return;

	pUser.SetTempTag( "ITEMDETAILS", makeID );
	pUser.SetTempTag( "Skill", entry.skill || 0 );
	pUser.SetTempTag( "needRecipeID", entry.recipeID || 0 );

	ClearCustomCraftHarvestTags( pUser );

	if( entry.harvest && IsCustomCraftArrayValue( entry.harvest ))
	{
		for( var i = 0; i < entry.harvest.length && i < 4; i++ )
		{
			var tagName = ( i == 0 ? "Harvest" : "Harvest" + ( i + 1 ));
			pUser.SetTempTag( tagName, entry.harvest[i] );
		}
	}

	if( entry.harvestNames && IsCustomCraftArrayValue( entry.harvestNames ))
	{
		for( var j = 0; j < entry.harvestNames.length && j < 4; j++ )
		{
			var nameTag = ( j == 0 ? "HarvestName" : "Harvest" + ( j + 1 ) + "Name" );
			pUser.SetTempTag( nameTag, entry.harvestNames[j] );
		}
	}

	TriggerEvent( itemDetailsScriptID, "ItemDetailGump", pUser );
}

/** @type { ( pUser: Character, craftDefinition: object, makeID: number ) => void } */
function StoreLastTenCustomCraft( pUser, craftDefinition, makeID )
{
	if( !craftDefinition || !craftDefinition.showLastTen )
		return;

	var tagName = "LastTenCustomCraft_" + craftDefinition.craft;
	var rawValue = pUser.GetTempTag( tagName ) || "";
	var split = rawValue.split( "," );
	var newList = [ makeID.toString() ];

	for( var i = 0; i < split.length; i++ )
	{
		var value = parseInt( split[i], 10 );

		if( !isNaN( value ) && value != makeID && newList.length < 10 )
			newList.push( value.toString() );
	}

	pUser.SetTempTag( tagName, newList.join( "," ) );
}

/** @type { ( timerObj: Character, timerID: number ) => void } */
function onTimer( timerObj, timerID )
{
	if( !ValidateObject( timerObj ))
		return;

	var socket = timerObj.socket;

	if( !socket )
		return;

	PageX( socket, timerObj, timerID );
}

/** @type { ( entry: object ) => boolean } */
function EraOK( entry )
{
	if( !entry )
		return false;

	if( entry.minEra && coreShardEra < EraStringToNum( entry.minEra ))
		return false;

	if( entry.maxEra && coreShardEra > EraStringToNum( entry.maxEra ))
		return false;

	return true;
}

/** @type { ( pUser: Character, recipeID: number ) => boolean } */
function HasLearnedRecipe( pUser, recipeID )
{
	if( !recipeID || recipeID <= 0 )
		return true;

	return TriggerEvent( 4022, "NeedRecipe", pUser.socket, recipeID );
}

/** @type { ( pUser: Character ) => void } */
function ClearCustomCraftHarvestTags( pUser )
{
	pUser.SetTempTag( "Harvest", null );
	pUser.SetTempTag( "Harvest2", null );
	pUser.SetTempTag( "Harvest3", null );
	pUser.SetTempTag( "Harvest4", null );
	pUser.SetTempTag( "HarvestName", null );
	pUser.SetTempTag( "Harvest2Name", null );
	pUser.SetTempTag( "Harvest3Name", null );
	pUser.SetTempTag( "Harvest4Name", null );
}

/** @type { ( value: any ) => boolean } */
function IsCustomCraftArrayValue( value )
{
	return Object.prototype.toString.call( value ) == "[object Array]";
}

/** @type { ( text: string ) => string } */
function SanitizeCustomCraftJsonText( text )
{
	if( text == null || typeof( text ) == "undefined" )
		return "";

	text = String( text );

	if( text.length > 0 && text.charCodeAt( 0 ) == 65279 )
		text = text.substring( 1 );

	text = text.split( "\r\n" ).join( "\n" );
	text = text.split( "\r" ).join( "\n" );
	text = text.split( String.fromCharCode( 160 ) ).join( " " );
	text = text.split( String.fromCharCode( 255 ) ).join( "" );
	text = text.split( "\t" ).join( " " );

	text = TrimCustomCraftString( text );

	var lastBracket = text.lastIndexOf( "]" );
	if( lastBracket >= 0 )
		text = text.substring( 0, lastBracket + 1 );

	return TrimCustomCraftString( text );
}

/** @type { ( text: string ) => string } */
function TrimCustomCraftString( text )
{
	if( text == null || typeof( text ) == "undefined" )
		return "";

	return text.replace( /^\s+|\s+$/g, "" );
}

/** @type { () => void } */
function ReloadCustomCraftDefinitions()
{
	customCraftDefinitions = null;
	customCraftDefinitionsLoaded = false;
	customCraftDefinitionsLoadError = false;
	CustomCraftMap = {};
	loadedCustomCraftName = "";

	LoadCustomCraftDefinitions();
}

/** @type { ( craftDefinition: object, keyName: string, fallbackText: string ) => string } */
function GetCustomCraftText( craftDefinition, keyName, fallbackText )
{
	if( craftDefinition && craftDefinition[keyName] )
		return craftDefinition[keyName];

	return fallbackText;
}

/** @type { ( pSock: Socket, pUser: Character, targetObj: Item, craftDefinition: object ) => boolean } */
function CheckCustomCraftTargetItem( pSock, pUser, targetObj, craftDefinition )
{
	if( !ValidateObject( targetObj ) || !targetObj.isItem )
	{
		pSock.SysMessage( GetCustomCraftText( craftDefinition, "invalidTargetText", "That is not a valid item." ));
		return false;
	}

	var ownerObj = GetPackOwner( targetObj, 0 );
	if( !ValidateObject( ownerObj ) || ownerObj.serial != pUser.serial )
	{
		pSock.SysMessage( GetCustomCraftText( craftDefinition, "mustBeInPackText", "That item must be in your backpack." ));
		return false;
	}

	return true;
}

/** @type { ( pUser: Character, stationIDs: any ) => boolean } */
function HasNearbyCustomCraftStation( pUser, stationIDs )
{
	if( !stationIDs || !IsCustomCraftArrayValue( stationIDs ))
		return true;

	var nearbyStation = AreaItemFunction( "FindNearbyCustomCraftStation", pUser, 3, stationIDs );
	return nearbyStation != 0;
}

/** @type { ( pUser: Character, trgItem: Item, stationIDs: any ) => boolean } */
function FindNearbyCustomCraftStation( pUser, trgItem, stationIDs )
{
	if( !ValidateObject( trgItem ) || !trgItem.isItem )
		return false;

	for( var i = 0; i < stationIDs.length; i++ )
	{
		if( trgItem.id == stationIDs[i] )
			return true;
	}

	return false;
}

/** @type { ( pSock: Socket, pUser: Character, targetObj: Item, craftDefinition: object ) => void } */
function RepairCustomCraftItem( pSock, pUser, targetObj, craftDefinition )
{
	if( !CheckCustomCraftTargetItem( pSock, pUser, targetObj, craftDefinition ))
		return;

	if( craftDefinition.repairStationIDs && !HasNearbyCustomCraftStation( pUser, craftDefinition.repairStationIDs ))
	{
		pSock.SysMessage( GetCustomCraftText( craftDefinition, "repairNoStationText", "You need to be near the proper crafting station to repair that." ));
		RestartCustomCraftGump( pUser, craftDefinition );
		return;
	}

	if( craftDefinition.repairMaterialType )
	{
		var materialType = TriggerEvent( 2506, "GetItemMaterialType", targetObj );
		if( materialType != craftDefinition.repairMaterialType )
		{
			pSock.SysMessage( GetCustomCraftText( craftDefinition, "repairWrongMaterialText", "That item cannot be repaired with this craft." ));
			RestartCustomCraftGump( pUser, craftDefinition );
			return;
		}
	}

	if( targetObj.maxhp <= 0 )
	{
		pSock.SysMessage( GetCustomCraftText( craftDefinition, "repairInvalidDurabilityText", "That item cannot be repaired." ));
		RestartCustomCraftGump( pUser, craftDefinition );
		return;
	}

	if( targetObj.health >= targetObj.maxhp )
	{
		pSock.SysMessage( GetCustomCraftText( craftDefinition, "repairFullText", "That item is already fully repaired." ));
		RestartCustomCraftGump( pUser, craftDefinition );
		return;
	}

	var repairSkillID = craftDefinition.repairSkillID;
	var repairSkillName = craftDefinition.repairSkillName;
	var repairSkill = 0;
	var repairSkillCap = 1000;

	if( typeof repairSkillID != "undefined" )
	{
		repairSkill = pUser.skills[repairSkillID];
		repairSkillCap = pUser.skillCaps[repairSkillID];
	}
	else if( repairSkillName )
	{
		repairSkill = pUser.skills[repairSkillName];
		repairSkillCap = pUser.skillCaps[repairSkillName];
	}

	var minSkill = craftDefinition.repairMinSkill || 0;
	if( repairSkill < minSkill )
	{
		pSock.SysMessage( GetCustomCraftText( craftDefinition, "repairTooDifficultText", "You are not skilled enough to repair that." ));
		RestartCustomCraftGump( pUser, craftDefinition );
		return;
	}

	var missingHP = targetObj.maxhp - targetObj.health;
	var repairDifficulty = Math.floor(( missingHP * 1000 ) / targetObj.maxhp );
	var minDifficulty = repairDifficulty - 250;

	if( minDifficulty < 0 )
		minDifficulty = 0;

	var maxDifficulty = repairDifficulty + 250;
	if( maxDifficulty > repairSkillCap )
		maxDifficulty = repairSkillCap;

	if( RandomNumber( minDifficulty, 1000 ) < repairSkill )
	{
		if( typeof repairSkillID != "undefined" )
			pUser.CheckSkill( repairSkillID, minDifficulty, maxDifficulty );

		if( GetServerSetting( "ItemRepairDurabilityLoss" ))
			targetObj.maxhp -= 1;

		targetObj.health = targetObj.maxhp;

		if( craftDefinition.repairSound )
			pSock.SoundEffect( craftDefinition.repairSound, true );

		pSock.SysMessage( GetCustomCraftText( craftDefinition, "repairSuccessText", "You repair the item." ));
	}
	else
	{
		var damageLoss = craftDefinition.repairFailureDamage || 1;
		targetObj.health -= damageLoss;

		if( targetObj.health <= 0 )
		{
			pSock.SysMessage( GetCustomCraftText( craftDefinition, "repairDestroyedText", "The item has been destroyed." ));
			targetObj.Delete();
		}
		else
		{
			pSock.SysMessage( GetCustomCraftText( craftDefinition, "repairFailedText", "You fail to repair the item." ));
		}
	}

	RestartCustomCraftGump( pUser, craftDefinition );
}

/** @type { ( pSock: Socket, pUser: Character, targetObj: Item, craftDefinition: object ) => void } */
function RecycleCustomCraftItem( pSock, pUser, targetObj, craftDefinition )
{
	if( !CheckCustomCraftTargetItem( pSock, pUser, targetObj, craftDefinition ))
		return;

	if( craftDefinition.recycleStationIDs && !HasNearbyCustomCraftStation( pUser, craftDefinition.recycleStationIDs ))
	{
		pSock.SysMessage( GetCustomCraftText( craftDefinition, "recycleNoStationText", "You need to be near the proper crafting station to recycle that." ));
		RestartCustomCraftGump( pUser, craftDefinition );
		return;
	}

	if( craftDefinition.recycleMaterialType )
	{
		var materialType = TriggerEvent( 2506, "GetItemMaterialType", targetObj );
		if( materialType != craftDefinition.recycleMaterialType )
		{
			pSock.SysMessage( GetCustomCraftText( craftDefinition, "recycleWrongMaterialText", "That item cannot be recycled with this craft." ));
			RestartCustomCraftGump( pUser, craftDefinition );
			return;
		}
	}

	var recycleResourceID = craftDefinition.recycleResourceID || 0;
	if( recycleResourceID == 0 )
	{
		pSock.SysMessage( GetCustomCraftText( craftDefinition, "recycleNoResourceText", "This craft does not define a recycled resource." ));
		RestartCustomCraftGump( pUser, craftDefinition );
		return;
	}

	var resourceAmount = craftDefinition.recycleResourceAmount || 1;
	var resourceHue = craftDefinition.recycleResourceHue || 0;

	if( craftDefinition.recycleUseTargetHue )
		resourceHue = targetObj.colour;

	if( craftDefinition.recycleFromCreateEntry )
	{
		var createEntry = null;

		if( targetObj.entryMadeFrom != null && targetObj.entryMadeFrom != 0 )
			createEntry = CreateEntries[targetObj.entryMadeFrom];

		if( createEntry && createEntry.resources && createEntry.resources.length > 0 )
		{
			resourceAmount = Math.max( 1, Math.floor( createEntry.resources[0][0] / 2 ));
		}
	}

	targetObj.Delete();

	var newResource = CreateDFNItem( pSock, pUser, recycleResourceID.toString(), resourceAmount, "ITEM", true, resourceHue );

	if( ValidateObject( newResource ) && craftDefinition.recycleResourceName )
		newResource.name = craftDefinition.recycleResourceName;

	pSock.SysMessage( GetCustomCraftText( craftDefinition, "recycleSuccessText", "You recycle the item." ));
	RestartCustomCraftGump( pUser, craftDefinition );
}

/** @type { ( pUser: Character, craftDefinition: object ) => void } */
function RestartCustomCraftGump( pUser, craftDefinition )
{
	if( !ValidateObject( pUser ))
		return;

	var timerID = pUser.GetTempTag( "page" ) || 1;
	var delay = craftDefinition.actionDelay || 200;

	pUser.StartTimer( delay, timerID, true );
}

/** @type { ( pSock: Socket, pUser: Character, targetObj: any, targetID: number ) => void } */
function onCallback0( pSock, pUser, targetObj, targetID )
{
	var craftDefinition = GetActiveCustomCraftDefinition( pUser );

	if( !craftDefinition || !craftDefinition.allowRepair )
		return;

	RepairCustomCraftItem( pSock, pUser, targetObj, craftDefinition );
}

/** @type { ( pSock: Socket, pUser: Character, targetObj: any, targetID: number ) => void } */
function onCallback1( pSock, pUser, targetObj, targetID )
{
	var craftDefinition = GetActiveCustomCraftDefinition( pUser );

	if( !craftDefinition || !craftDefinition.allowRecycle )
		return;

	RecycleCustomCraftItem( pSock, pUser, targetObj, craftDefinition );
}
