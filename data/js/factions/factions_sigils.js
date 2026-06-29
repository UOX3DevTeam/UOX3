// =============================================================================
// factions_sigils.js
// UOX3 Faction System - simple sigil capture and town control
// Script ID suggestion: 8502
// Attach this script to each sigil item.
// =============================================================================

var SigilCorruptionTime = 36000000;
var SigilReturnTimeDefault = 1800000;
var SigilCaptureSilverReward = 250;
var SigilCaptureKillPointReward = 10;
var SigilCaptureScoreReward = 1;
var SigilCorruptionTimerId = 1;
var SigilReturnTimerId = 2;
var SigilDropSettleTimerId = 3;
var SigilTownDefaults = {
	Britain: "TB",
	Trinsic: "TB",
	Moonglow: "COM",
	SkaraBrae: "COM",
	Yew: "MIN",
	Vesper: "MIN",
	Minoc: "SL",
	Cove: "SL"
};
var SigilTownScriptId = 8509;
var SigilStrongholdScriptId = 8511;
var SigilIterateMode = "";
var SigilIterateTown = "";
var SigilIterateSocket = null;
var SigilIterateCount = 0;
var SigilIterateDelay = 0;
var SigilController = null;

function SigilNormalizeTown( townName )
{
	townName = String( townName ).replace( /^\s+|\s+$/g, "" ).toLowerCase().replace( /\s+/g, "" );
	if( townName === "skarabrae" || townName === "skara" )
		return "SkaraBrae";
	if( townName === "britain" )
		return "Britain";
	if( townName === "trinsic" )
		return "Trinsic";
	if( townName === "moonglow" )
		return "Moonglow";
	if( townName === "yew" )
		return "Yew";
	if( townName === "vesper" )
		return "Vesper";
	if( townName === "minoc" )
		return "Minoc";
	if( townName === "cove" )
		return "Cove";

	return "";
}

function SigilDisplayTown( townName )
{
	if( townName === "SkaraBrae" )
		return "Skara Brae";

	return townName;
}

function SigilFactionName( factionKey )
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

function SigilCleanMessage( messageText )
{
	messageText = String( messageText ).replace( /^\s+|\s+$/g, "" );
	messageText = messageText.replace( /</g, "" ).replace( />/g, "" );
	if( messageText.length > 120 )
		messageText = messageText.substr( 0, 120 );

	return messageText;
}

function SigilGetFaction( pChar )
{
	if( !ValidateObject( pChar ) )
		return "";
	var factionKey = pChar.GetTag( "faction" );
	if( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" )
		return factionKey;
	return "";
}

function SigilOwnerFaction( iSigil )
{
	if( !ValidateObject( iSigil ) )
		return "";

	var townName = iSigil.GetTag( "sigil_town" );
	var currentOwner = iSigil.GetTag( "sigil_owner_faction" );
	if( SigilGetFactionKeyValid( currentOwner ) )
		return currentOwner;

	return SigilTownDefaults[townName] || "";
}

function SigilClearCorruption( iSigil )
{
	if( !ValidateObject( iSigil ) )
		return false;

	iSigil.SetTag( "sigil_corrupting_faction", "" );
	iSigil.SetTag( "sigil_corrupt_start", 0 );
	iSigil.SetTag( "sigil_corrupted", 0 );
	return true;
}

function SigilParseNumber( value, fallback )
{
	var parsed = parseInt( value, 10 );
	if( isNaN( parsed ) )
		return fallback;

	return parsed;
}

function SigilGetController()
{
	if( ValidateObject( SigilController ) )
		return SigilController;

	SigilController = null;
	SigilIterateMode = "controller";
	IterateOver( "ITEM" );
	SigilIterateMode = "";
	return SigilController;
}

function SigilReturnDelay( iSigil )
{
	if( !ValidateObject( iSigil ) )
		return SigilReturnTimeDefault;

	var delay = SigilParseNumber( iSigil.GetTag( "sigil_return_delay" ), SigilReturnTimeDefault );
	if( delay < 60000 )
		delay = 60000;

	return delay;
}

function SigilStartReturnTimer( iSigil )
{
	if( !ValidateObject( iSigil ) || iSigil.GetTag( "sigil" ) != 1 )
		return false;
	if( !SigilHasHome( iSigil ) )
		return false;
	if( SigilParseNumber( iSigil.GetTag( "sigil_corrupt_start" ), 0 ) > 0 )
		return false;

	var delay = SigilReturnDelay( iSigil );
	iSigil.SetTag( "sigil_return_due", GetCurrentClock() + delay );
	iSigil.KillJSTimer( SigilReturnTimerId, 8502 );
	iSigil.StartTimer( delay, SigilReturnTimerId, 8502 );
	return true;
}

function SigilClearReturnTimer( iSigil )
{
	if( !ValidateObject( iSigil ) )
		return false;

	iSigil.SetTag( "sigil_return_due", 0 );
	iSigil.KillJSTimer( SigilReturnTimerId, 8502 );
	return true;
}

function SigilSetHome( iSigil )
{
	if( !ValidateObject( iSigil ) || iSigil.GetTag( "sigil" ) != 1 )
		return false;

	iSigil.SetTag( "sigil_home_x", iSigil.x );
	iSigil.SetTag( "sigil_home_y", iSigil.y );
	iSigil.SetTag( "sigil_home_z", iSigil.z );
	iSigil.SetTag( "sigil_home_world", iSigil.worldnumber );
	iSigil.SetTag( "sigil_home_instance", iSigil.instanceID );
	iSigil.SetTag( "sigil_home_set", 1 );
	return true;
}

function SigilHasHome( iSigil )
{
	if( !ValidateObject( iSigil ) || iSigil.GetTag( "sigil" ) != 1 )
		return false;

	return ( iSigil.GetTag( "sigil_home_set" ) == 1 );
}

function SigilReturnHome( iSigil )
{
	if( !SigilHasHome( iSigil ) )
		return false;

	var homeX = parseInt( iSigil.GetTag( "sigil_home_x" ), 10 );
	var homeY = parseInt( iSigil.GetTag( "sigil_home_y" ), 10 );
	var homeZ = parseInt( iSigil.GetTag( "sigil_home_z" ), 10 );
	var homeWorld = parseInt( iSigil.GetTag( "sigil_home_world" ), 10 );
	var homeInstance = parseInt( iSigil.GetTag( "sigil_home_instance" ), 10 );
	if( isNaN( homeX ) || isNaN( homeY ) || isNaN( homeZ ) || isNaN( homeWorld ) || isNaN( homeInstance ) )
		return false;

	SigilClearCorruption( iSigil );
	SigilClearReturnTimer( iSigil );
	iSigil.SetTag( "sigil_carrier_faction", "" );
	iSigil.SetTag( "sigil_carrier_serial", 0 );
	iSigil.SetTag( "sigil_stolen_at", 0 );
	iSigil.container = null;
	iSigil.SetLocation( homeX, homeY, homeZ, homeWorld, homeInstance );
	return true;
}

function SigilIsAtHome( iSigil )
{
	if( !SigilHasHome( iSigil ) )
		return false;

	var homeX = SigilParseNumber( iSigil.GetTag( "sigil_home_x" ), 0 );
	var homeY = SigilParseNumber( iSigil.GetTag( "sigil_home_y" ), 0 );
	var homeZ = SigilParseNumber( iSigil.GetTag( "sigil_home_z" ), 0 );
	var homeWorld = SigilParseNumber( iSigil.GetTag( "sigil_home_world" ), 0 );
	var homeInstance = SigilParseNumber( iSigil.GetTag( "sigil_home_instance" ), 0 );

	return ( iSigil.x == homeX && iSigil.y == homeY && iSigil.z == homeZ && iSigil.worldnumber == homeWorld && iSigil.instanceID == homeInstance );
}

function SigilMatchesTown( iSigil, townName )
{
	if( !ValidateObject( iSigil ) || iSigil.GetTag( "sigil" ) != 1 )
		return false;
	if( townName === "" )
		return true;

	return SigilNormalizeTown( iSigil.GetTag( "sigil_town" ) ) === townName;
}

function onUseChecked( pUser, iSigil )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iSigil ) )
		return true;
	if( iSigil.GetTag( "sigil" ) != 1 )
		return true;

	var pSock = pUser.socket;
	if( !pSock )
		return false;

	ShowSigilGump( pSock, pUser, iSigil );
	return false;
}

function ShowSigilGump( pSock, pUser, iSigil )
{
	var townName = iSigil.GetTag( "sigil_town" );
	if( townName === "" || townName == 0 )
		townName = "Unknown";

	var currentOwner = iSigil.GetTag( "sigil_owner_faction" );
	if( currentOwner === "" || currentOwner == 0 )
		currentOwner = SigilTownDefaults[townName] || "None";

	var corruptStart = iSigil.GetTag( "sigil_corrupt_start" );
	var corrupted = iSigil.GetTag( "sigil_corrupted" );
	var corruptingFaction = iSigil.GetTag( "sigil_corrupting_faction" );
	var carrierFaction = iSigil.GetTag( "sigil_carrier_faction" );
	var returnDue = SigilParseNumber( iSigil.GetTag( "sigil_return_due" ), 0 );
	var playerFaction = SigilGetFaction( pUser );
	var strongholdStatus = "";
	var playerCanCorruptHere = false;
	if( playerFaction !== "" && playerFaction !== currentOwner )
	{
		playerCanCorruptHere = TriggerEvent( SigilStrongholdScriptId, "StrongholdIsObjectAtFactionStronghold", iSigil, playerFaction );
		strongholdStatus = TriggerEvent( SigilStrongholdScriptId, "StrongholdDistanceText", iSigil, playerFaction );
	}

	var myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 430, 350, 9200 );
	myGump.AddHTMLGump( 20, 15, 390, 25, 0, 0, "<CENTER><b>Faction Sigil</b></CENTER>" );
	myGump.AddHTMLGump( 20, 55, 390, 20, 0, 0, "Town: " + townName );
	myGump.AddHTMLGump( 20, 80, 390, 20, 0, 0, "Current Owner: " + currentOwner );

	if( corruptStart > 0 && corrupted != 1 )
	{
		var remaining = SigilCorruptionTime - ( GetCurrentClock() - corruptStart );
		if( remaining < 0 )
			remaining = 0;
		myGump.AddHTMLGump( 20, 105, 390, 20, 0, 0, "Corrupting Faction: " + corruptingFaction );
		myGump.AddHTMLGump( 20, 130, 390, 20, 0, 0, "Corruption remaining: " + Math.ceil( remaining / 60000 ) + " minute(s)" );
	}
	else if( corrupted == 1 )
	{
		myGump.AddHTMLGump( 20, 105, 390, 20, 0, 0, "This town is controlled by " + currentOwner + "." );
	}
	else if( carrierFaction !== "" && carrierFaction != 0 )
	{
		myGump.AddHTMLGump( 20, 105, 390, 20, 0, 0, "Last Carried By: " + carrierFaction );
		if( returnDue > GetCurrentClock() )
			myGump.AddHTMLGump( 20, 130, 390, 20, 0, 0, "Return time: " + Math.ceil( ( returnDue - GetCurrentClock() ) / 60000 ) + " minute(s)" );
	}

	if( playerFaction !== "" && playerFaction !== currentOwner && corruptStart == 0 )
	{
		myGump.AddHTMLGump( 20, 165, 390, 20, 0, 0, "Your Stronghold: " + strongholdStatus );
		if( playerCanCorruptHere )
		{
			myGump.AddButton( 25, 195, 0xFA5, 1, 0, 1 );
			myGump.AddHTMLGump( 65, 195, 280, 20, 0, 0, "Begin corruption for your faction" );
		}
		else
		{
			myGump.AddHTMLGump( 20, 195, 390, 40, 0, 0, "Bring this sigil to your faction stronghold to corrupt it." );
		}
	}

	myGump.AddButton( 25, 300, 0xFA5, 1, 0, 0 );
	myGump.AddHTMLGump( 65, 300, 100, 20, 0, 0, "Close" );
	myGump.Send( pSock );
	myGump.Free();

	pSock.tempInt = iSigil.serial;
}

function onGumpPress( pSock, pButton, gumpData )
{
	if( pButton == 0 )
		return;

	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var sigilSerial = pSock.tempInt;
	var iSigil = CalcItemFromSer( sigilSerial );
	if( !ValidateObject( iSigil ) )
	{
		pUser.SysMessage( "That sigil is no longer available." );
		return;
	}

	var playerFaction = SigilGetFaction( pUser );
	if( playerFaction === "" )
	{
		pUser.SysMessage( "Only faction members may corrupt sigils." );
		return;
	}

	if( pButton == 1 )
	{
		if( !TriggerEvent( SigilStrongholdScriptId, "StrongholdIsObjectAtFactionStronghold", iSigil, playerFaction ) )
		{
			pUser.SysMessage( "You must bring this sigil to your faction stronghold first: " + TriggerEvent( SigilStrongholdScriptId, "StrongholdLastError" ) );
			return;
		}

		iSigil.SetTag( "sigil_corrupting_faction", playerFaction );
		iSigil.SetTag( "sigil_corruptor_serial", pUser.serial );
		iSigil.SetTag( "sigil_carrier_faction", "" );
		iSigil.SetTag( "sigil_carrier_serial", 0 );
		SigilClearReturnTimer( iSigil );
		iSigil.SetTag( "sigil_corrupt_start", GetCurrentClock() );
		iSigil.SetTag( "sigil_corrupted", 0 );
		iSigil.StartTimer( 60000, SigilCorruptionTimerId, 8502 );
		pUser.SysMessage( "You have begun corrupting this sigil." );
		BroadcastMessage( playerFaction + " has begun corrupting the sigil of " + iSigil.GetTag( "sigil_town" ) + "." );
	}
}

function onTimer( iSigil, timerID )
{
	if( !ValidateObject( iSigil ) )
		return;
	if( iSigil.GetTag( "sigil" ) != 1 )
		return;

	if( timerID == SigilReturnTimerId )
	{
		SigilCheckReturnTimer( iSigil );
		return;
	}

	if( timerID == SigilDropSettleTimerId )
	{
		SigilCheckDropSettled( iSigil );
		return;
	}

	if( timerID != SigilCorruptionTimerId )
		return;

	var corruptStart = iSigil.GetTag( "sigil_corrupt_start" );
	if( corruptStart > 0 && iSigil.GetTag( "sigil_corrupted" ) != 1 )
	{
		var activeFaction = iSigil.GetTag( "sigil_corrupting_faction" );
		if( !TriggerEvent( SigilStrongholdScriptId, "StrongholdIsObjectAtFactionStronghold", iSigil, activeFaction ) )
		{
			iSigil.SetTag( "sigil_corrupt_start", 0 );
			iSigil.SetTag( "sigil_corrupting_faction", "" );
			iSigil.SetTag( "sigil_corrupted", 0 );
			BroadcastMessage( "The corruption of the " + iSigil.GetTag( "sigil_town" ) + " sigil has failed because it left the stronghold." );
			return;
		}

		if( GetCurrentClock() - corruptStart >= SigilCorruptionTime )
		{
			var corruptingFaction = activeFaction;
			if( SigilGetFactionKeyValid( corruptingFaction ) )
				iSigil.SetTag( "sigil_owner_faction", corruptingFaction );

			iSigil.SetTag( "sigil_corrupted", 1 );
			iSigil.SetTag( "sigil_corrupt_start", 0 );
			iSigil.SetTag( "sigil_corrupting_faction", "" );
			SigilAwardCapture( iSigil, corruptingFaction );
			TriggerEvent( SigilTownScriptId, "ApplySigilTownControl", iSigil );
			BroadcastMessage( SigilFactionName( iSigil.GetTag( "sigil_owner_faction" ) ) + " has taken control of " + SigilDisplayTown( iSigil.GetTag( "sigil_town" ) ) + "." );
			return;
		}
		iSigil.StartTimer( 60000, SigilCorruptionTimerId, 8502 );
	}
}

function SigilCheckReturnTimer( iSigil )
{
	if( !ValidateObject( iSigil ) || iSigil.GetTag( "sigil" ) != 1 )
		return false;
	if( SigilParseNumber( iSigil.GetTag( "sigil_corrupt_start" ), 0 ) > 0 )
		return false;

	var carrierFaction = iSigil.GetTag( "sigil_carrier_faction" );
	if( SigilGetFactionKeyValid( carrierFaction ) && TriggerEvent( SigilStrongholdScriptId, "StrongholdIsObjectAtFactionStronghold", iSigil, carrierFaction ) )
	{
		SigilStartReturnTimer( iSigil );
		return true;
	}

	var townName = iSigil.GetTag( "sigil_town" );
	if( SigilReturnHome( iSigil ) )
		BroadcastMessage( "The sigil of " + townName + " has returned home." );

	return true;
}

function SigilAwardCapture( iSigil, factionKey )
{
	if( !ValidateObject( iSigil ) || !SigilGetFactionKeyValid( factionKey ) )
		return false;

	var ctrl = SigilGetController();
	if( ValidateObject( ctrl ) )
	{
		ctrl.SetTag( "score_" + factionKey, SigilParseNumber( ctrl.GetTag( "score_" + factionKey ), 0 ) + SigilCaptureScoreReward );
		ctrl.SetTag( "captures_" + factionKey, SigilParseNumber( ctrl.GetTag( "captures_" + factionKey ), 0 ) + 1 );
		ctrl.SetTag( "last_capture_town_" + factionKey, iSigil.GetTag( "sigil_town" ) );
		ctrl.SetTag( "last_capture_time_" + factionKey, GetCurrentClock() );
	}

	var corruptorSerial = SigilParseNumber( iSigil.GetTag( "sigil_corruptor_serial" ), 0 );
	var corruptor = CalcCharFromSer( corruptorSerial );
	if( ValidateObject( corruptor ) && corruptor.GetTag( "faction" ) === factionKey )
	{
		var silver = SigilParseNumber( corruptor.GetTag( "faction_silver" ), 0 ) + SigilCaptureSilverReward;
		if( silver > 100000 )
			silver = 100000;
		corruptor.SetTag( "faction_silver", silver );

		var killPoints = SigilParseNumber( corruptor.GetTag( "faction_kp" ), 0 ) + SigilCaptureKillPointReward;
		corruptor.SetTag( "faction_kp", killPoints );
		corruptor.SetTag( "faction_captures", SigilParseNumber( corruptor.GetTag( "faction_captures" ), 0 ) + 1 );
		SigilUpdateRank( corruptor );
		if( corruptor.socket != null )
			corruptor.SysMessage( "You earned " + SigilCaptureSilverReward + " faction silver and " + SigilCaptureKillPointReward + " kill points for capturing " + SigilDisplayTown( iSigil.GetTag( "sigil_town" ) ) + "." );
	}

	iSigil.SetTag( "sigil_corruptor_serial", 0 );
	return true;
}

function SigilUpdateRank( pChar )
{
	if( !ValidateObject( pChar ) )
		return false;

	var rankPoints = [ 0, 5, 10, 20, 40, 80, 160, 320, 640, 1280 ];
	var killPoints = SigilParseNumber( pChar.GetTag( "faction_kp" ), 0 );
	var rank = 0;
	for( var i = rankPoints.length - 1; i >= 0; i-- )
	{
		if( killPoints >= rankPoints[i] )
		{
			rank = i;
			break;
		}
	}

	pChar.SetTag( "faction_rank", rank );
	return true;
}

function SigilCheckDropSettled( iSigil )
{
	if( !ValidateObject( iSigil ) || iSigil.GetTag( "sigil" ) != 1 )
		return false;
	if( SigilParseNumber( iSigil.GetTag( "sigil_corrupt_start" ), 0 ) > 0 )
		return false;

	if( SigilIsAtHome( iSigil ) )
	{
		SigilClearReturnTimer( iSigil );
		iSigil.SetTag( "sigil_carrier_faction", "" );
		iSigil.SetTag( "sigil_carrier_serial", 0 );
		iSigil.SetTag( "sigil_stolen_at", 0 );
		return true;
	}

	return SigilStartReturnTimer( iSigil );
}

function onPickup( iSigil, pGrabber, containerObj )
{
	if( !ValidateObject( iSigil ) || iSigil.GetTag( "sigil" ) != 1 )
		return true;

	var grabberFaction = SigilGetFaction( pGrabber );
	if( grabberFaction === "" )
	{
		if( ValidateObject( pGrabber ) )
			pGrabber.SysMessage( "Only faction members may take faction sigils." );
		return false;
	}

	if( !SigilHasHome( iSigil ) )
		SigilSetHome( iSigil );

	iSigil.SetTag( "sigil_carrier_faction", grabberFaction );
	iSigil.SetTag( "sigil_carrier_serial", pGrabber.serial );
	iSigil.SetTag( "sigil_stolen_at", GetCurrentClock() );
	SigilStartReturnTimer( iSigil );
	return true;
}

function onDrop( iSigil, pDropper )
{
	if( !ValidateObject( iSigil ) || iSigil.GetTag( "sigil" ) != 1 )
		return 1;

	if( ValidateObject( pDropper ) )
		iSigil.SetTag( "sigil_carrier_faction", SigilGetFaction( pDropper ) );

	iSigil.StartTimer( 1000, SigilDropSettleTimerId, 8502 );
	return 1;
}

function onClick( pSock, iSigil )
{
	if( !ValidateObject( iSigil ) )
		return false;
	if( iSigil.GetTag( "sigil" ) != 1 )
		return false;

	pSock.SysMessage( "Faction Sigil [" + iSigil.GetTag( "sigil_town" ) + "]" );
	return true;
}

function SigilGetFactionKeyValid( factionKey )
{
	return ( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" );
}

function SigilRegisterHome( townName )
{
	SigilIterateMode = "home";
	SigilIterateTown = SigilNormalizeTown( townName );
	SigilIterateCount = 0;
	IterateOver( "ITEM" );
	var count = SigilIterateCount;
	SigilIterateMode = "";
	SigilIterateTown = "";
	SigilIterateCount = 0;
	return count;
}

function SigilReturn( townName )
{
	SigilIterateMode = "return";
	SigilIterateTown = SigilNormalizeTown( townName );
	SigilIterateCount = 0;
	IterateOver( "ITEM" );
	var count = SigilIterateCount;
	SigilIterateMode = "";
	SigilIterateTown = "";
	SigilIterateCount = 0;
	return count;
}

function ShowSigilStatus( pSock, townName )
{
	SigilIterateMode = "status";
	SigilIterateTown = SigilNormalizeTown( townName );
	SigilIterateSocket = pSock;
	SigilIterateCount = 0;
	IterateOver( "ITEM" );
	var count = SigilIterateCount;
	SigilIterateMode = "";
	SigilIterateTown = "";
	SigilIterateSocket = null;
	SigilIterateCount = 0;
	return count;
}

function SigilSetReturnTime( minutes )
{
	minutes = SigilParseNumber( minutes, 0 );
	if( minutes < 1 )
		return 0;

	SigilIterateMode = "delay";
	SigilIterateTown = "";
	SigilIterateDelay = minutes * 60000;
	SigilIterateCount = 0;
	IterateOver( "ITEM" );
	var count = SigilIterateCount;
	SigilIterateMode = "";
	SigilIterateDelay = 0;
	SigilIterateCount = 0;
	return count;
}

function ShowSigilReturnTime( pSock )
{
	if( pSock == null )
		return false;

	pSock.SysMessage( "Default sigil return time: " + Math.ceil( SigilReturnTimeDefault / 60000 ) + " minute(s)." );
	SigilIterateMode = "delaystatus";
	SigilIterateTown = "";
	SigilIterateSocket = pSock;
	SigilIterateCount = 0;
	IterateOver( "ITEM" );
	var count = SigilIterateCount;
	SigilIterateMode = "";
	SigilIterateSocket = null;
	SigilIterateCount = 0;
	if( count == 0 )
		pSock.SysMessage( "No faction sigils found." );

	return true;
}

function ShowFactionScore( pSock )
{
	if( pSock == null )
		return false;

	var ctrl = SigilGetController();
	if( !ValidateObject( ctrl ) )
	{
		pSock.SysMessage( "Faction controller was not found." );
		return false;
	}

	var factionKeys = [ "TB", "COM", "MIN", "SL" ];
	for( var i = 0; i < factionKeys.length; i++ )
	{
		var factionKey = factionKeys[i];
		var score = SigilParseNumber( ctrl.GetTag( "score_" + factionKey ), 0 );
		var captures = SigilParseNumber( ctrl.GetTag( "captures_" + factionKey ), 0 );
		var lastTown = ctrl.GetTag( "last_capture_town_" + factionKey );
		if( lastTown === "" || lastTown == 0 )
			lastTown = "None";

		pSock.SysMessage( SigilFactionName( factionKey ) + ": score " + score + ", captures " + captures + ", last capture " + SigilDisplayTown( lastTown ) );
	}

	return true;
}

function FactionScoreText( factionKey )
{
	if( !SigilGetFactionKeyValid( factionKey ) )
		return "Score: 0, Captures: 0";

	var ctrl = SigilGetController();
	if( !ValidateObject( ctrl ) )
		return "Score: 0, Captures: 0";

	return "Score: " + SigilParseNumber( ctrl.GetTag( "score_" + factionKey ), 0 ) + ", Captures: " + SigilParseNumber( ctrl.GetTag( "captures_" + factionKey ), 0 );
}

function FactionScoreValue( factionKey )
{
	if( !SigilGetFactionKeyValid( factionKey ) )
		return 0;

	var ctrl = SigilGetController();
	if( !ValidateObject( ctrl ) )
		return 0;

	return SigilParseNumber( ctrl.GetTag( "score_" + factionKey ), 0 );
}

function SpendFactionScore( factionKey, amount )
{
	if( !SigilGetFactionKeyValid( factionKey ) )
		return false;

	var ctrl = SigilGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	amount = SigilParseNumber( amount, 0 );
	if( amount < 1 )
		return true;

	var score = SigilParseNumber( ctrl.GetTag( "score_" + factionKey ), 0 );
	if( score < amount )
		return false;

	ctrl.SetTag( "score_" + factionKey, score - amount );
	return true;
}

function SetFactionNotice( factionKey, messageText, setterName )
{
	if( !SigilGetFactionKeyValid( factionKey ) )
		return false;

	var ctrl = SigilGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	messageText = SigilCleanMessage( messageText );
	if( messageText === "" )
		return false;

	ctrl.SetTag( "notice_" + factionKey, messageText );
	ctrl.SetTag( "notice_by_" + factionKey, SigilCleanMessage( setterName ) );
	ctrl.SetTag( "notice_time_" + factionKey, GetCurrentClock() );
	return true;
}

function ClearFactionNotice( factionKey )
{
	if( !SigilGetFactionKeyValid( factionKey ) )
		return false;

	var ctrl = SigilGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	ctrl.SetTag( "notice_" + factionKey, "" );
	ctrl.SetTag( "notice_by_" + factionKey, "" );
	ctrl.SetTag( "notice_time_" + factionKey, 0 );
	return true;
}

function FactionNoticeText( factionKey )
{
	if( !SigilGetFactionKeyValid( factionKey ) )
		return "Notice: None";

	var ctrl = SigilGetController();
	if( !ValidateObject( ctrl ) )
		return "Notice: None";

	var noticeText = ctrl.GetTag( "notice_" + factionKey );
	if( noticeText === "" || noticeText == 0 )
		return "Notice: None";

	return "Notice: " + noticeText;
}

function ResetFactionScore()
{
	var ctrl = SigilGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	var factionKeys = [ "TB", "COM", "MIN", "SL" ];
	for( var i = 0; i < factionKeys.length; i++ )
	{
		var factionKey = factionKeys[i];
		ctrl.SetTag( "score_" + factionKey, 0 );
		ctrl.SetTag( "captures_" + factionKey, 0 );
		ctrl.SetTag( "last_capture_town_" + factionKey, "" );
		ctrl.SetTag( "last_capture_time_" + factionKey, 0 );
	}

	return true;
}

function onIterate( toCheck )
{
	if( SigilIterateMode === "controller" )
	{
		if( ValidateObject( toCheck ) && toCheck.isItem && toCheck.GetTag( "faction_controller" ) == 1 )
		{
			SigilController = toCheck;
			return true;
		}

		return false;
	}

	if( !SigilMatchesTown( toCheck, SigilIterateTown ) )
		return false;

	if( SigilIterateMode === "home" )
	{
		if( SigilSetHome( toCheck ) )
			SigilIterateCount++;
		return false;
	}

	if( SigilIterateMode === "return" )
	{
		if( SigilReturnHome( toCheck ) )
			SigilIterateCount++;
		return false;
	}

	if( SigilIterateMode === "status" )
	{
		SigilIterateCount++;
		if( SigilIterateSocket != null )
		{
			var homeText = SigilHasHome( toCheck ) ? "home set" : "home missing";
			var carrierFaction = toCheck.GetTag( "sigil_carrier_faction" );
			var returnDue = SigilParseNumber( toCheck.GetTag( "sigil_return_due" ), 0 );
			var carriedText = "";
			if( carrierFaction !== "" && carrierFaction != 0 )
				carriedText = ", carried by " + carrierFaction;
			if( returnDue > GetCurrentClock() )
				carriedText += ", returns in " + Math.ceil( ( returnDue - GetCurrentClock() ) / 60000 ) + " min";
			else
				carriedText += ", no return timer";

			SigilIterateSocket.SysMessage( SigilDisplayTown( toCheck.GetTag( "sigil_town" ) ) + ": owner " + SigilOwnerFaction( toCheck ) + ", at " + toCheck.x + "," + toCheck.y + "," + toCheck.z + ", " + homeText + carriedText );
		}
		return false;
	}

	if( SigilIterateMode === "delay" )
	{
		toCheck.SetTag( "sigil_return_delay", SigilIterateDelay );
		if( SigilParseNumber( toCheck.GetTag( "sigil_return_due" ), 0 ) > 0 )
			SigilStartReturnTimer( toCheck );
		SigilIterateCount++;
		return false;
	}

	if( SigilIterateMode === "delaystatus" )
	{
		SigilIterateCount++;
		if( SigilIterateSocket != null )
			SigilIterateSocket.SysMessage( SigilDisplayTown( toCheck.GetTag( "sigil_town" ) ) + ": return time " + Math.ceil( SigilReturnDelay( toCheck ) / 60000 ) + " minute(s)." );
		return false;
	}

	return false;
}
