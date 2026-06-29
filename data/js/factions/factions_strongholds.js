// =============================================================================
// factions_strongholds.js
// UOX3 Faction System - faction stronghold locations
// Script ID suggestion: 8511
// =============================================================================

var StrongholdController = null;
var StrongholdLastErrorText = "";
var StrongholdDefaultRange = 12;
var StrongholdFactionKeys = [ "TB", "COM", "MIN", "SL" ];

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
	StrongholdLastErrorText = errorText;
	return false;
}

function StrongholdLastError()
{
	return StrongholdLastErrorText;
}

function StrongholdGetController()
{
	if( ValidateObject( StrongholdController ) )
		return StrongholdController;

	StrongholdController = null;
	IterateOver( "ITEM" );
	return StrongholdController;
}

function onIterate( toCheck )
{
	if( ValidateObject( toCheck ) && toCheck.isItem && toCheck.GetTag( "faction_controller" ) == 1 )
	{
		StrongholdController = toCheck;
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
	var parsed = parseInt( value, 10 );
	if( isNaN( parsed ) )
		return fallback;

	return parsed;
}

function StrongholdSet( factionKey, pChar, range )
{
	StrongholdLastErrorText = "";
	factionKey = String( factionKey ).toUpperCase();
	if( !StrongholdIsFactionValid( factionKey ) )
		return StrongholdSetLastError( "Invalid faction." );
	if( !ValidateObject( pChar ) )
		return StrongholdSetLastError( "Invalid character." );

	var ctrl = StrongholdGetController();
	if( !ValidateObject( ctrl ) )
		return StrongholdSetLastError( "Faction controller was not found. Run 'factionsetup or create a faction controller first." );

	range = StrongholdParseNumber( range, StrongholdDefaultRange );
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
	StrongholdLastErrorText = "";
	factionKey = String( factionKey ).toUpperCase();
	if( !StrongholdIsFactionValid( factionKey ) )
		return StrongholdSetLastError( "Invalid faction." );

	var ctrl = StrongholdGetController();
	if( !ValidateObject( ctrl ) )
		return StrongholdSetLastError( "Faction controller was not found. Run 'factionsetup or create a faction controller first." );

	x = StrongholdParseNumber( x, 0 );
	y = StrongholdParseNumber( y, 0 );
	z = StrongholdParseNumber( z, 0 );
	worldNum = StrongholdParseNumber( worldNum, 0 );
	instanceId = StrongholdParseNumber( instanceId, 0 );
	range = StrongholdParseNumber( range, StrongholdDefaultRange );
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

	var ctrl = StrongholdGetController();
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

	var ctrl = StrongholdGetController();
	if( !ValidateObject( ctrl ) )
		return StrongholdFactionName( factionKey ) + ": faction controller not found.";
	if( !StrongholdIsConfigured( factionKey ) )
		return StrongholdFactionName( factionKey ) + ": not configured.";

	var x = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "x" ) ), 0 );
	var y = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "y" ) ), 0 );
	var z = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "z" ) ), 0 );
	var worldNum = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "world" ) ), 0 );
	var instanceId = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "instance" ) ), 0 );
	var range = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "range" ) ), StrongholdDefaultRange );

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

	var ctrl = StrongholdGetController();
	var x = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "x" ) ), 0 );
	var y = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "y" ) ), 0 );
	var worldNum = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "world" ) ), 0 );
	var instanceId = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "instance" ) ), 0 );
	var range = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "range" ) ), StrongholdDefaultRange );

	if( obj.worldnumber != worldNum || obj.instanceID != instanceId )
		return "Wrong map/instance.";

	var dx = obj.x - x;
	var dy = obj.y - y;
	var distance = Math.ceil( Math.sqrt( dx * dx + dy * dy ) );
	if( distance <= range )
		return "At stronghold.";

	return distance + " tile(s) away. Range is " + range + ".";
}

function StrongholdIsObjectAtFactionStronghold( obj, factionKey )
{
	StrongholdLastErrorText = "";
	factionKey = String( factionKey ).toUpperCase();
	if( !StrongholdIsFactionValid( factionKey ) )
		return StrongholdSetLastError( "Invalid faction." );
	if( !ValidateObject( obj ) )
		return StrongholdSetLastError( "Invalid object." );

	var ctrl = StrongholdGetController();
	if( !ValidateObject( ctrl ) )
		return StrongholdSetLastError( "Faction controller was not found." );
	if( !StrongholdIsConfigured( factionKey ) )
		return StrongholdSetLastError( StrongholdFactionName( factionKey ) + " stronghold is not configured." );

	var x = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "x" ) ), 0 );
	var y = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "y" ) ), 0 );
	var worldNum = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "world" ) ), 0 );
	var instanceId = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "instance" ) ), 0 );
	var range = StrongholdParseNumber( ctrl.GetTag( StrongholdTag( factionKey, "range" ) ), StrongholdDefaultRange );

	if( obj.worldnumber != worldNum || obj.instanceID != instanceId )
		return StrongholdSetLastError( "The sigil is not in your faction stronghold map/instance." );

	var dx = obj.x - x;
	var dy = obj.y - y;
	if( ( dx * dx ) + ( dy * dy ) > ( range * range ) )
		return StrongholdSetLastError( "The sigil must be within " + range + " tile(s) of your faction stronghold." );

	return true;
}

function ShowStrongholdStatus( pSock )
{
	if( pSock == null )
		return false;

	for( var i = 0; i < StrongholdFactionKeys.length; i++ )
		pSock.SysMessage( StrongholdInfoText( StrongholdFactionKeys[i] ) );

	return true;
}
