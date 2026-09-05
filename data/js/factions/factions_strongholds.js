/// <reference path="../definitions.d.ts" />
// @ts-check

// =============================================================================
// factions_strongholds.js
// UOX3 Faction System - faction stronghold locations
// Script ID suggestion: 8511
// =============================================================================

let strongholdController = null;
let strongholdLastErrorText = "";
const strongholdDefaultRange = 12;
const strongholdFactionKeys = [ "TB", "COM", "MIN", "SL" ];

function StrongholdIsFactionValid( factionKey )
{
	return ( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" );
}

function StrongholdFactionName( factionKey )
{
	if( factionKey === "TB" )
		return "True Britannians";
	if( factionKey === "COM" )
		return "Council of Mages";
	if( factionKey === "MIN" )
		return "Minax";
	if( factionKey === "SL" )
		return "Shadowlords";

	return factionKey;
}

function StrongholdSetLastError( errorText )
{
	strongholdLastErrorText = errorText;
	return false;
}

function StrongholdLastError()
{
	return strongholdLastErrorText;
}

function StrongholdGetController()
{
	if( ValidateObject( strongholdController ) )
		return strongholdController;

	strongholdController = null;
	IterateOver( "ITEM" );
	return strongholdController;
}

function onIterate( toCheck )
{
	if( ValidateObject( toCheck ) && toCheck.isItem && toCheck.GetTag( "faction_controller" ) == 1 )
	{
		strongholdController = toCheck;
		return true;
	}

	return false;
}

function StrongholdTag( factionKey, tagName )
{
	return "stronghold_" + factionKey + "_" + tagName;
}

function StrongholdParseNumber( value, fallback )
{
	const parsed = parseInt( value, 10 );
	if( isNaN( parsed ) )
		return fallback;

	return parsed;
}

function StrongholdSet( factionKey, pChar, range )
{
	strongholdLastErrorText = "";
	factionKey = String( factionKey ).toUpperCase();
	if( !StrongholdIsFactionValid( factionKey ) )
		return StrongholdSetLastError( "Invalid faction." );
	if( !ValidateObject( pChar ) )
		return StrongholdSetLastError( "Invalid character." );

	const ctrl = StrongholdGetController();
	if( !ValidateObject( ctrl ) )
		return StrongholdSetLastError( "Faction controller was not found. Run 'factionsetup or create a faction controller first." );

	range = StrongholdParseNumber( range, strongholdDefaultRange );
	if( range < 1 )
		range = 1;
	if( range > 100 )
		range = 100;

	ctrl.SetTag( StrongholdTag( factionKey, "x" ), pChar.x );
	ctrl.SetTag( StrongholdTag( factionKey, "y" ), pChar.y );
	ctrl.SetTag( StrongholdTag( factionKey, "z" ), pChar.z );
	ctrl.SetTag( StrongholdTag( factionKey, "world" ), pChar.worldnumber );
	ctrl.SetTag( StrongholdTag( factionKey, "instance" ), pChar.instanceID );
	ctrl.SetTag( StrongholdTag( factionKey, "range" ), range );
	ctrl.SetTag( StrongholdTag( factionKey, "configured" ), 1 );
	ctrl.SetTag( StrongholdTag( factionKey, "set_at" ), GetCurrentClock() );
	return true;
}

function StrongholdSetLocation( factionKey, x, y, z, worldNum, instanceId, range )
{
	strongholdLastErrorText = "";
	factionKey = String( factionKey ).toUpperCase();
	if( !StrongholdIsFactionValid( factionKey ) )
		return StrongholdSetLastError( "Invalid faction." );

	const ctrl = StrongholdGetController();
	if( !ValidateObject( ctrl ) )
		return StrongholdSetLastError( "Faction controller was not found. Run 'factionsetup or create a faction controller first." );

	x = StrongholdParseNumber( x, 0 );
	y = StrongholdParseNumber( y, 0 );
	z = StrongholdParseNumber( z, 0 );
	worldNum = StrongholdParseNumber( worldNum, 0 );
	instanceId = StrongholdParseNumber( instanceId, 0 );
	range = StrongholdParseNumber( range, strongholdDefaultRange );
	if( range < 1 )
		range = 1;
	if( range > 100 )
		range = 100;

	ctrl.SetTag( StrongholdTag( factionKey, "x" ), x );
	ctrl.SetTag( StrongholdTag( factionKey, "y" ), y );
	ctrl.SetTag( StrongholdTag( factionKey, "z" ), z );
	ctrl.SetTag( StrongholdTag( factionKey, "world" ), worldNum );
	ctrl.SetTag( StrongholdTag( factionKey, "instance" ), instanceId );
	ctrl.SetTag( StrongholdTag( factionKey, "range" ), range );
	ctrl.SetTag( StrongholdTag( factionKey, "configured" ), 1 );
	ctrl.SetTag( StrongholdTag( factionKey, "set_at" ), GetCurrentClock() );
	return true;
}

function StrongholdIsConfigured( factionKey )
{
	factionKey = String( factionKey ).toUpperCase();
	if( !StrongholdIsFactionValid( factionKey ) )
		return false;

	const ctrl = StrongholdGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	if( ctrl.GetTag( StrongholdTag( factionKey, "configured" ) ) == 1 )
		return true;
	if( StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "set_at" ) ), 0 ) > 0 )
		return true;

	return StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "range" ) ), 0 ) > 0;
}

function StrongholdInfoText( factionKey )
{
	factionKey = String( factionKey ).toUpperCase();
	if( !StrongholdIsFactionValid( factionKey ) )
		return "Invalid faction.";

	const ctrl = StrongholdGetController();
	if( !ValidateObject( ctrl ) )
		return StrongholdFactionName( factionKey ) + ": faction controller not found.";
	if( !StrongholdIsConfigured( factionKey ) )
		return StrongholdFactionName( factionKey ) + ": not configured.";

	let x = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "x" ) ), 0 );
	let y = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "y" ) ), 0 );
	let z = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "z" ) ), 0 );
	let worldNum = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "world" ) ), 0 );
	let instanceId = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "instance" ) ), 0 );
	let range = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "range" ) ), strongholdDefaultRange );

	return StrongholdFactionName( factionKey ) + ": " + x + "," + y + "," + z + " world " + worldNum + " instance " + instanceId + " range " + range;
}

function StrongholdDistanceText( obj, factionKey )
{
	factionKey = String( factionKey ).toUpperCase();
	if( !StrongholdIsFactionValid( factionKey ) )
		return "Invalid faction.";
	if( !ValidateObject( obj ) )
		return "Invalid object.";
	if( !StrongholdIsConfigured( factionKey ) )
		return "No stronghold configured.";

	const ctrl = StrongholdGetController();
	let x = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "x" ) ), 0 );
	let y = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "y" ) ), 0 );
	let worldNum = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "world" ) ), 0 );
	let instanceId = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "instance" ) ), 0 );
	let range = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "range" ) ), strongholdDefaultRange );

	if( obj.worldnumber != worldNum || obj.instanceID != instanceId )
		return "Wrong map/instance.";

	const dx = obj.x - x;
	const dy = obj.y - y;
	const distance = Math.ceil( Math.sqrt( dx * dx + dy * dy ) );
	if( distance <= range )
		return "At stronghold.";

	return distance + " tile(s) away. Range is " + range + ".";
}

function StrongholdIsObjectAtFactionStronghold( obj, factionKey )
{
	strongholdLastErrorText = "";
	factionKey = String( factionKey ).toUpperCase();
	if( !StrongholdIsFactionValid( factionKey ) )
		return StrongholdSetLastError( "Invalid faction." );
	if( !ValidateObject( obj ) )
		return StrongholdSetLastError( "Invalid object." );

	const ctrl = StrongholdGetController();
	if( !ValidateObject( ctrl ) )
		return StrongholdSetLastError( "Faction controller was not found." );
	if( !StrongholdIsConfigured( factionKey ) )
		return StrongholdSetLastError( StrongholdFactionName( factionKey ) + " stronghold is not configured." );

	let x = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "x" ) ), 0 );
	let y = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "y" ) ), 0 );
	let worldNum = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "world" ) ), 0 );
	let instanceId = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "instance" ) ), 0 );
	let range = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "range" ) ), strongholdDefaultRange );

	if( obj.worldnumber != worldNum || obj.instanceID != instanceId )
		return StrongholdSetLastError( "The sigil is not in your faction stronghold map/instance." );

	const dx = obj.x - x;
	const dy = obj.y - y;
	if( ( dx * dx ) + ( dy * dy ) > ( range * range ) )
		return StrongholdSetLastError( "The sigil must be within " + range + " tile(s) of your faction stronghold." );

	return true;
}

function ShowStrongholdStatus( pSock )
{
	if( pSock == null )
		return false;

	for( let i = 0; i < strongholdFactionKeys.length; i++ )
		pSock.SysMessage( StrongholdInfoText( strongholdFactionKeys[i] ) );

	return true;
}
