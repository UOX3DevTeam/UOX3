/// <reference path="../../definitions.d.ts" />
// @ts-check

var craftMapRegistry = {};
var craftMapRegistryLoaded = {};
var craftMapRegistryLoadError = {};

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

	if( typeof entry.makeID == "undefined" || isNaN( parseInt( entry.makeID, 10 ) ) )
	{
		Console.Warning( "CraftMap system: Entry " + entryIndex + " in '" + craftName + "' is missing valid makeID." );
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

	entry.makeID = parseInt( entry.makeID, 10 );
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