/// <reference path="../../definitions.d.ts" />
// @ts-check

var craftMapRegistry = {};
var craftMapRegistryLoaded = {};
var craftMapRegistryLoadError = {};
var craftResourceMap = null;
var craftResourceMapLoaded = false;
var craftResourceMapLoadError = false;

/** @type { () => object|null } */
function LoadCraftResourceMap()
{
	if( craftResourceMapLoaded )
		return craftResourceMap;

	craftResourceMap = null;
	craftResourceMapLoaded = false;
	craftResourceMapLoadError = false;

	var resourceMapFile = new UOXCFile();
	resourceMapFile.Open( "resourcemap.json", "r", "crafting", true );

	if( resourceMapFile == null || resourceMapFile.Length() < 0 )
	{
		Console.Error( "CraftMap system: Unable to open js/jsdata/crafting/resourcemap.json" );
		craftResourceMapLoadError = true;
		return null;
	}

	var fileText = "";
	while( !resourceMapFile.EOF() )
	{
		var rawLine = resourceMapFile.ReadUntil( "\n" );
		if( rawLine != null && typeof( rawLine ) != "undefined" )
		{
			fileText += rawLine;
		}
	}

	resourceMapFile.Close();
	resourceMapFile.Free();

	fileText = SanitizeCraftMapJsonText( fileText );

	try
	{
		craftResourceMap = JSON.parse( fileText );
	}
	catch( error )
	{
		Console.Error( "CraftMap system: Failed to parse resourcemap.json: " + error );
		craftResourceMapLoadError = true;
		return null;
	}

	if( !IsCraftMapArrayValue( craftResourceMap ) )
	{
		Console.Error( "CraftMap system: resourcemap.json must contain a JSON array." );
		craftResourceMapLoadError = true;
		return null;
	}

	craftResourceMapLoaded = true;
	craftResourceMapLoadError = false;

	Console.Print( "CraftMap system: Loaded " + craftResourceMap.length + " resource maps.\n" );

	return craftResourceMap;
}

/** @type { ( resourceSet: string, pUser: Character ) => object|null } */
function GetCraftResourceSelection( resourceSet, pUser )
{
	var resourceMap = LoadCraftResourceMap();

	if( !resourceMap )
		return null;

	for( var i = 0; i < resourceMap.length; i++ )
	{
		var entry = resourceMap[i];

		if( entry.resourceSet == resourceSet )
			return BuildCraftResourceSelection( entry, pUser );
	}

	return null;
}

/** @type { ( resourceSet: string ) => object|null } */
function GetCraftResourceList( resourceSet )
{
	var resourceMap = LoadCraftResourceMap();

	if( !resourceMap )
		return null;

	for( var i = 0; i < resourceMap.length; i++ )
	{
		if( resourceMap[i].resourceSet == resourceSet )
			return resourceMap[i];
	}

	return null;
}

/** @type { ( resourceEntry: object, pUser: Character ) => object|null } */
function BuildCraftResourceSelection( resourceEntry, pUser )
{
	if( !resourceEntry || !resourceEntry.items || !IsCraftMapArrayValue( resourceEntry.items ) )
		return null;

	var selectedIndex = resourceEntry.defaultIndex || 0;

	if( resourceEntry.tempTag )
	{
		var tempIndex = pUser.GetTempTag( resourceEntry.tempTag );
		if( tempIndex || tempIndex == 0 )
			selectedIndex = tempIndex;
	}

	var selectedResource = null;

	for( var i = 0; i < resourceEntry.items.length; i++ )
	{
		var itemEntry = resourceEntry.items[i];

		if( typeof itemEntry.index == "undefined" )
		{
			selectedResource = itemEntry;
			break;
		}

		if( itemEntry.index == selectedIndex )
		{
			selectedResource = itemEntry;
			break;
		}
	}

	if( !selectedResource )
		selectedResource = resourceEntry.items[0];

	return {
		dictID: selectedResource.dictID || resourceEntry.dictID,
		amount: CountCraftResourceAmount( selectedResource, pUser ),
		resource: selectedResource
	};
}

/** @type { ( resourceEntry: object, pUser: Character ) => number } */
function CountCraftResourceAmount( resourceEntry, pUser )
{
	if( !resourceEntry )
		return 0;

	if( resourceEntry.items && IsCraftMapArrayValue( resourceEntry.items ) )
	{
		var totalAmount = 0;

		for( var i = 0; i < resourceEntry.items.length; i++ )
		{
			totalAmount += CountCraftResourceAmount( resourceEntry.items[i], pUser );
		}

		return totalAmount;
	}

	return pUser.ResourceCount( resourceEntry.itemID, resourceEntry.hue || 0 );
}

/** @type { () => void } */
function ReloadCraftResourceMap()
{
	craftResourceMap = null;
	craftResourceMapLoaded = false;
	craftResourceMapLoadError = false;

	LoadCraftResourceMap();
}

/** @type { ( craftName: string ) => object|null } */
function CraftMapRegistry( craftName )
{
	craftName = NormalizeCraftMapName( craftName );

	if( craftName == "" )
	{
		Console.Warning( "CraftMap system: Missing craft map name." );
		return null;
	}

	if( !craftMapRegistryLoaded[craftName] )
	{
		LoadCraftMapRegistry( craftName );
	}

	return craftMapRegistry[craftName] || null;
}

/** @type { ( makeID: number ) => object|null } */
function GetCraftMapEntryByMakeID( makeID )
{
	makeID = parseInt( makeID, 10 );

	if( isNaN( makeID ))
		return null;

	var craftNames = [
		"alchemy",
		"blacksmithing",
		"carpentry",
		"cartography",
		"cooking",
		"fletching",
		"glassblowing",
		"masonry",
		"tailoring",
		"tinkering"
	];

	for( var i = 0; i < craftNames.length; i++ )
	{
		var craftMap = CraftMapRegistry( craftNames[i] );

		if( !craftMap )
			continue;

		for( var j = 0; j < craftMap.length; j++ )
		{
			var entry = craftMap[j];

			if( !entry )
				continue;

			if( entry.makeID == makeID )
				return entry;

			if( entry.oreMake && CraftMapArrayHasValue( entry.oreMake, makeID ))
				return entry;

			if( entry.graniteMake && CraftMapArrayHasValue( entry.graniteMake, makeID ))
				return entry;
		}
	}

	return null;
}

/** @type { ( valueList: any, valueToFind: number ) => boolean } */
function CraftMapArrayHasValue( valueList, valueToFind )
{
	if( !IsCraftMapArrayValue( valueList ))
		return false;

	for( var i = 0; i < valueList.length; i++ )
	{
		if( parseInt( valueList[i], 10 ) == valueToFind )
			return true;
	}

	return false;
}

/** @type { ( craftName: string ) => void } */
function ReloadCraftMapRegistry( craftName )
{
	craftName = NormalizeCraftMapName( craftName );

	if( craftName == "" )
	{
		craftMapRegistry = {};
		craftMapRegistryLoaded = {};
		craftMapRegistryLoadError = {};
		return;
	}

	craftMapRegistry[craftName] = null;
	craftMapRegistryLoaded[craftName] = false;
	craftMapRegistryLoadError[craftName] = false;

	LoadCraftMapRegistry( craftName );
}

/** @type { ( craftName: string ) => void } */
function LoadCraftMapRegistry( craftName )
{
	craftName = NormalizeCraftMapName( craftName );

	craftMapRegistry[craftName] = null;
	craftMapRegistryLoaded[craftName] = false;
	craftMapRegistryLoadError[craftName] = false;

	var fileName = craftName + ".json";

	var craftMapFile = new UOXCFile();
	craftMapFile.Open( fileName, "r", "crafting", true );

	if( craftMapFile == null || craftMapFile.Length() < 0 )
	{
		Console.Error( "CraftMap system: Unable to open js/jsdata/crafting/" + fileName );
		craftMapRegistryLoadError[craftName] = true;
		return;
	}

	var fileText = "";
	while( !craftMapFile.EOF() )
	{
		var rawLine = craftMapFile.ReadUntil( "\n" );
		if( rawLine != null && typeof( rawLine ) != "undefined" )
		{
			fileText += rawLine;
		}
	}

	craftMapFile.Close();
	craftMapFile.Free();

	fileText = SanitizeCraftMapJsonText( fileText );

	if( fileText == "" )
	{
		Console.Error( "CraftMap system: " + fileName + " is empty after sanitizing." );
		craftMapRegistryLoadError[craftName] = true;
		return;
	}

	var parsedCraftMap = null;

	try
	{
		parsedCraftMap = JSON.parse( fileText );
	}
	catch( error )
	{
		Console.Error( "CraftMap system: Failed to parse " + fileName + ": " + error );
		craftMapRegistryLoadError[craftName] = true;
		return;
	}

	if( !IsCraftMapArrayValue( parsedCraftMap ) )
	{
		Console.Error( "CraftMap system: " + fileName + " must contain a JSON array." );
		craftMapRegistryLoadError[craftName] = true;
		return;
	}

	craftMapRegistry[craftName] = [];

	for( var i = 0; i < parsedCraftMap.length; i++ )
	{
		var entry = parsedCraftMap[i];

		if( !ValidateCraftMapEntry( entry, craftName, i ) )
		{
			continue;
		}

		craftMapRegistry[craftName].push( entry );
	}

	Console.Print( "CraftMap system: Loaded " + craftMapRegistry[craftName].length + " entries from " + fileName + ".\n" );

	craftMapRegistryLoaded[craftName] = true;
	craftMapRegistryLoadError[craftName] = false;
}

/** @type { ( entry: object, craftName: string, entryIndex: number ) => boolean } */
function ValidateCraftMapEntry( entry, craftName, entryIndex )
{
	if( !entry || typeof entry != "object" || IsCraftMapArrayValue( entry ) )
	{
		Console.Warning( "CraftMap system: Entry " + entryIndex + " in '" + craftName + "' is not a valid object." );
		return false;
	}

	var hasMakeID = (typeof entry.makeID != "undefined" && !isNaN(parseInt(entry.makeID, 10)));
	var hasButtonID = (typeof entry.buttonID != "undefined" && !isNaN(parseInt(entry.buttonID, 10)));
	var hasOreMake = (typeof entry.oreMake != "undefined" && IsCraftMapArrayValue(entry.oreMake));
	var hasGraniteMake = (typeof entry.graniteMake != "undefined" && IsCraftMapArrayValue(entry.graniteMake));

	if (!hasMakeID && !hasButtonID)
	{
		Console.Warning("CraftMap system: Entry " + entryIndex + " in '" + craftName + "' is missing valid makeID or buttonID.");
		return false;
	}

	if (hasButtonID && !hasMakeID && !hasOreMake && !hasGraniteMake)
	{
		Console.Warning("CraftMap system: Entry " + entryIndex + " in '" + craftName + "' has buttonID but no makeID, oreMake, or graniteMake.");
		return false;
	}

	if( typeof entry.page == "undefined" || isNaN( parseInt( entry.page, 10 ) ) )
	{
		Console.Warning( "CraftMap system: Entry " + entryIndex + " in '" + craftName + "' is missing valid page." );
		return false;
	}

	if( typeof entry.timerID == "undefined" || isNaN( parseInt( entry.timerID, 10 ) ) )
	{
		entry.timerID = entry.page;
	}

	if( typeof entry.dictID != "undefined" && isNaN( parseInt( entry.dictID, 10 ) ) )
	{
		Console.Warning( "CraftMap system: Entry " + entryIndex + " in '" + craftName + "' has invalid dictID." );
		return false;
	}

	if( typeof entry.harvest == "undefined" )
	{
		entry.harvest = [];
	}

	if( !IsCraftMapArrayValue( entry.harvest ) )
	{
		Console.Warning( "CraftMap system: Entry " + entryIndex + " in '" + craftName + "' has invalid harvest list." );
		return false;
	}

	if( typeof entry.harvestNames != "undefined" && !IsCraftMapArrayValue( entry.harvestNames ) )
	{
		Console.Warning( "CraftMap system: Entry " + entryIndex + " in '" + craftName + "' has invalid harvestNames list." );
		return false;
	}

	if( hasMakeID )
		entry.makeID = parseInt( entry.makeID, 10 );

	if( hasButtonID )
		entry.buttonID = parseInt( entry.buttonID, 10 );

	entry.page = parseInt( entry.page, 10 );
	entry.timerID = parseInt( entry.timerID, 10 );

	if( typeof entry.dictID != "undefined" )
	{
		entry.dictID = parseInt( entry.dictID, 10 );
	}

	return true;
}

/** @type { ( craftName: string ) => string } */
function NormalizeCraftMapName( craftName )
{
	if( craftName == null || typeof( craftName ) == "undefined" )
	{
		return "";
	}

	craftName = String( craftName );
	craftName = craftName.toLowerCase();
	craftName = craftName.replace( /[^a-z0-9_]/g, "" );

	return craftName;
}

/** @type { ( value: any ) => boolean } */
function IsCraftMapArrayValue( value )
{
	return Object.prototype.toString.call( value ) == "[object Array]";
}

/** @type { ( text: string ) => string } */
function TrimCraftMapString( text )
{
	if( text == null || typeof( text ) == "undefined" )
	{
		return "";
	}

	return text.replace( /^\s+|\s+$/g, "" );
}

/** @type { ( text: string ) => string } */
function SanitizeCraftMapJsonText( text )
{
	if( text == null || typeof( text ) == "undefined" )
	{
		return "";
	}

	text = String( text );

	if( text.length > 0 && text.charCodeAt( 0 ) == 65279 )
	{
		text = text.substring( 1 );
	}

	text = text.split( "\r\n" ).join( "\n" );
	text = text.split( "\r" ).join( "\n" );
	text = text.split( String.fromCharCode( 160 ) ).join( " " );
	text = text.split( String.fromCharCode( 255 ) ).join( "" );
	text = text.split( "\t" ).join( " " );

	text = TrimCraftMapString( text );

	var lastBracket = text.lastIndexOf( "]" );
	if( lastBracket >= 0 )
	{
		text = text.substring( 0, lastBracket + 1 );
	}

	return TrimCraftMapString( text );
}