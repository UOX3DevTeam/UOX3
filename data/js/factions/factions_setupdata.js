/// <reference path="../definitions.d.ts" />
// @ts-check

// =============================================================================
// factions_setupdata.js
// UOX3 Faction System - static setup data loader
// Script ID: 8514
// =============================================================================

const factionSetupDataFile = "faction_setup.json";
const factionSetupDataFolder = "factions";
let factionSetupCachedData = null;
let factionSetupLoaded = false;
let factionSetupLastErrorText = "";

function SetupDataLastError()
{
	return factionSetupLastErrorText;
}

function SetupDataSetLastError( errorText )
{
	factionSetupLastErrorText = errorText;
	return null;
}

function SetupDataSanitizeJsonText( fileText )
{
	if( fileText == null || typeof fileText == "undefined" )
		return "";

	fileText = String( fileText );

	if( fileText.length > 0 && fileText.charCodeAt( 0 ) == 65279 )
		fileText = fileText.substring( 1 );

	fileText = fileText.split( "\r\n" ).join( "\n" );
	fileText = fileText.split( "\r" ).join( "\n" );
	fileText = fileText.split( String.fromCharCode( 160 ) ).join( " " );
	fileText = fileText.split( String.fromCharCode( 255 ) ).join( "" );
	fileText = fileText.split( "\t" ).join( " " );
	fileText = fileText.replace( /^\s+|\s+$/g, "" );

	const lastBrace = fileText.lastIndexOf( "}" );
	if( lastBrace >= 0 )
		fileText = fileText.substring( 0, lastBrace + 1 );

	return fileText.replace( /^\s+|\s+$/g, "" );
}

function SetupDataReadFile()
{
	let setupFile = new UOXCFile();
	setupFile.Open( factionSetupDataFile, "r", factionSetupDataFolder, true );

	if( setupFile == null || setupFile.Length() < 0 )
		return SetupDataSetLastError( "Unable to open js/jsdata/factions/" + factionSetupDataFile + "." );

	let fileText = "";
	while( !setupFile.EOF() )
	{
		const rawLine = setupFile.ReadUntil( "\n" );
		if( rawLine != null && typeof rawLine != "undefined" )
			fileText += rawLine;
	}

	setupFile.Close();
	setupFile.Free();
	return SetupDataSanitizeJsonText( fileText );
}

function SetupDataValidateLocation( location )
{
	if( location == null || typeof location != "object" )
		return false;
	if( typeof location.x == "undefined" || typeof location.y == "undefined" || typeof location.z == "undefined" )
		return false;

	return true;
}

function SetupDataNormalizeLocation( location, defaults )
{
	if( defaults == null )
		defaults = {};

	location.x = parseInt( location.x, 10 );
	location.y = parseInt( location.y, 10 );
	location.z = parseInt( location.z, 10 );
	location.world = parseInt( typeof location.world == "undefined" ? defaults.world : location.world, 10 );
	location.instance = parseInt( typeof location.instance == "undefined" ? defaults.instance : location.instance, 10 );

	if( isNaN( location.x ) || isNaN( location.y ) || isNaN( location.z ) )
		return false;
	if( isNaN( location.world ) )
		location.world = 0;
	if( isNaN( location.instance ) )
		location.instance = 0;

	return true;
}

function SetupDataNormalizeConfig( setupData )
{
	if( !setupData || typeof setupData != "object" )
		return SetupDataSetLastError( "Faction setup data must be one JSON object." );
	if( !setupData.factions || typeof setupData.factions != "object" )
		return SetupDataSetLastError( "Faction setup data is missing factions." );
	if( !setupData.towns || typeof setupData.towns != "object" )
		return SetupDataSetLastError( "Faction setup data is missing towns." );

	let defaults = {
		world: parseInt( setupData.world, 10 ),
		instance: parseInt( setupData.instance, 10 )
	};
	if( isNaN( defaults.world ) )
		defaults.world = 0;
	if( isNaN( defaults.instance ) )
		defaults.instance = 0;

	for( let factionKey in setupData.factions )
	{
		if( !setupData.factions.hasOwnProperty( factionKey ) )
			continue;

		const factionData = setupData.factions[factionKey];
		if( SetupDataValidateLocation( factionData.joinStone ) )
			SetupDataNormalizeLocation( factionData.joinStone, defaults );
		if( SetupDataValidateLocation( factionData.factionStone ) )
			SetupDataNormalizeLocation( factionData.factionStone, defaults );
		if( factionData.stronghold && SetupDataValidateLocation( factionData.stronghold.center ) )
		{
			SetupDataNormalizeLocation( factionData.stronghold.center, defaults );
			factionData.stronghold.range = parseInt( factionData.stronghold.range, 10 );
			if( isNaN( factionData.stronghold.range ) || factionData.stronghold.range < 1 )
				factionData.stronghold.range = 12;
		}
	}

	for( let townName in setupData.towns )
	{
		if( !setupData.towns.hasOwnProperty( townName ) )
			continue;

		const townData = setupData.towns[townName];
		if( townData.sigil && SetupDataValidateLocation( townData.sigil ) )
			SetupDataNormalizeLocation( townData.sigil, defaults );
		if( townData.townStone && SetupDataValidateLocation( townData.townStone ) )
			SetupDataNormalizeLocation( townData.townStone, defaults );
	}

	factionSetupLastErrorText = "";
	return setupData;
}

function LoadFactionSetupData()
{
	factionSetupCachedData = null;
	factionSetupLoaded = false;
	factionSetupLastErrorText = "";

	let fileText = SetupDataReadFile();
	if( fileText == null || fileText === "" )
	{
		if( factionSetupLastErrorText === "" )
			SetupDataSetLastError( "Faction setup data file is empty." );
		return null;
	}

	let setupData = null;
	try
	{
		setupData = JSON.parse( fileText );
	}
	catch( error )
	{
		return SetupDataSetLastError( "Unable to parse " + factionSetupDataFile + " (" + fileText.length + " chars, starts '" + fileText.substr( 0, 1 ) + "', ends '" + fileText.substr( fileText.length - 1, 1 ) + "'): " + error );
	}

	setupData = SetupDataNormalizeConfig( setupData );
	if( setupData == null )
		return null;

	factionSetupCachedData = setupData;
	factionSetupLoaded = true;
	return factionSetupCachedData;
}

function ReloadFactionSetupData()
{
	return LoadFactionSetupData();
}

function FactionSetupData()
{
	if( !factionSetupLoaded || factionSetupCachedData == null )
		return LoadFactionSetupData();

	return factionSetupCachedData;
}
