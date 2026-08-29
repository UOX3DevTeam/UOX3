/// <reference path="../definitions.d.ts" />
// @ts-check

// =============================================================================
// factions_sigils.js
// UOX3 Faction System - simple sigil capture and town control
// Script ID suggestion: 8502
// Attach this script to each sigil item.
// =============================================================================

const sigilCorruptionTime = parseInt( GetServerSetting( "FACTIONSIGILCORRUPTIONMINUTES" ), 10 ) * 60000;
const sigilReturnTimeDefault = parseInt( GetServerSetting( "FACTIONSIGILRETURNMINUTES" ), 10 ) * 60000;
const sigilPurificationTime = parseInt( GetServerSetting( "FACTIONSIGILPURIFICATIONHOURS" ), 10 ) * 3600000;
const sigilCaptureSilverReward = parseInt( GetServerSetting( "FACTIONSIGILSILVERREWARD" ), 10 );
const sigilCaptureKillPointReward = parseInt( GetServerSetting( "FACTIONSIGILKILLPOINTREWARD" ), 10 );
const sigilCaptureScoreReward = parseInt( GetServerSetting( "FACTIONSIGILSCOREREWARD" ), 10 );
const sigilCorruptionTimerId = 1;
const sigilReturnTimerId = 2;
const sigilDropSettleTimerId = 3;
const sigilPurificationTimerId = 4;
const sigilTownDefaults = {
	Britain: "TB",
	Trinsic: "TB",
	Moonglow: "COM",
	SkaraBrae: "COM",
	Yew: "MIN",
	Vesper: "MIN",
	Minoc: "SL",
	Magincia: "SL"
};
const sigilTownScriptId = 8509;
const sigilStrongholdScriptId = 8511;
const sigilPlayerDataScriptId = 8513;
let sigilIterateMode = "";
let sigilIterateTown = "";
let sigilIterateSocket = null;
let sigilIterateCount = 0;
let sigilIterateDelay = 0;
let sigilController = null;

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
	if( townName === "magincia" || townName === "cove" )
		return "Magincia";

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
	let factionKey = pChar.GetTag( "faction" );
	if( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" )
		return factionKey;
	return "";
}

function SigilOwnerFaction( iSigil )
{
	if( !ValidateObject( iSigil ) )
		return "";

	let townName = iSigil.GetTag( "sigil_town" );
	let currentOwner = iSigil.GetTag( "sigil_owner_faction" );
	if( SigilGetFactionKeyValid( currentOwner ) )
		return currentOwner;

	return sigilTownDefaults[townName] || "";
}

function SigilClearCorruption( iSigil )
{
	if( !ValidateObject( iSigil ) )
		return false;

	iSigil.SetTag( "sigil_corrupting_faction", "" );
	iSigil.SetTag( "sigil_corrupt_start", 0 );
	iSigil.SetTag( "sigil_corrupted", 0 );
	iSigil.SetTag( "sigil_corrupted_faction", "" );
	return true;
}

function SigilParseNumber( value, fallback )
{
	const parsed = parseInt( value, 10 );
	if( isNaN( parsed ) )
		return fallback;

	return parsed;
}

function SigilGetController()
{
	if( ValidateObject( sigilController ) )
		return sigilController;

	sigilController = null;
	sigilIterateMode = "controller";
	IterateOver( "ITEM" );
	sigilIterateMode = "";
	return sigilController;
}

function SigilReturnDelay( iSigil )
{
	if( !ValidateObject( iSigil ) )
		return sigilReturnTimeDefault;

	let delay = SigilParseNumber( iSigil.GetTag( "sigil_return_delay" ), sigilReturnTimeDefault );
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

	let delay = SigilReturnDelay( iSigil );
	iSigil.SetTag( "sigil_return_due", GetCurrentClock() + delay );
	iSigil.KillJSTimer( sigilReturnTimerId, 8502 );
	iSigil.StartTimer( delay, sigilReturnTimerId, 8502 );
	return true;
}

function SigilClearReturnTimer( iSigil )
{
	if( !ValidateObject( iSigil ) )
		return false;

	iSigil.SetTag( "sigil_return_due", 0 );
	iSigil.KillJSTimer( sigilReturnTimerId, 8502 );
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

	const homeX = parseInt( iSigil.GetTag( "sigil_home_x" ), 10 );
	const homeY = parseInt( iSigil.GetTag( "sigil_home_y" ), 10 );
	const homeZ = parseInt( iSigil.GetTag( "sigil_home_z" ), 10 );
	const homeWorld = parseInt( iSigil.GetTag( "sigil_home_world" ), 10 );
	const homeInstance = parseInt( iSigil.GetTag( "sigil_home_instance" ), 10 );
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

	const homeX = SigilParseNumber( iSigil.GetTag( "sigil_home_x" ), 0 );
	const homeY = SigilParseNumber( iSigil.GetTag( "sigil_home_y" ), 0 );
	const homeZ = SigilParseNumber( iSigil.GetTag( "sigil_home_z" ), 0 );
	const homeWorld = SigilParseNumber( iSigil.GetTag( "sigil_home_world" ), 0 );
	const homeInstance = SigilParseNumber( iSigil.GetTag( "sigil_home_instance" ), 0 );

	return ( iSigil.x == homeX && iSigil.y == homeY && iSigil.z == homeZ && iSigil.worldnumber == homeWorld && iSigil.instanceID == homeInstance );
}

function SigilIsNearHome( iSigil, range )
{
	if( !SigilHasHome( iSigil ) || iSigil.worldnumber != SigilParseNumber( iSigil.GetTag( "sigil_home_world" ), -1 ) ||
		iSigil.instanceID != SigilParseNumber( iSigil.GetTag( "sigil_home_instance" ), -1 ) )
		return false;

	const dx = Math.abs( iSigil.x - SigilParseNumber( iSigil.GetTag( "sigil_home_x" ), iSigil.x + range + 1 ) );
	const dy = Math.abs( iSigil.y - SigilParseNumber( iSigil.GetTag( "sigil_home_y" ), iSigil.y + range + 1 ) );
	return ( dx <= range && dy <= range );
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

	let pSock = pUser.socket;
	if( !pSock )
		return false;

	ShowSigilGump( pSock, pUser, iSigil );
	return false;
}

function ShowSigilGump( pSock, pUser, iSigil )
{
	let townName = iSigil.GetTag( "sigil_town" );
	if( townName === "" || townName == 0 )
		townName = "Unknown";

	let currentOwner = iSigil.GetTag( "sigil_owner_faction" );
	if( currentOwner === "" || currentOwner == 0 )
		currentOwner = sigilTownDefaults[townName] || "None";

	let corruptStart = iSigil.GetTag( "sigil_corrupt_start" );
	let corrupted = iSigil.GetTag( "sigil_corrupted" );
	const corruptingFaction = iSigil.GetTag( "sigil_corrupting_faction" );
	const carrierFaction = iSigil.GetTag( "sigil_carrier_faction" );
	const returnDue = SigilParseNumber( iSigil.GetTag( "sigil_return_due" ), 0 );
	let purificationStart = SigilParseNumber( iSigil.GetTag( "sigil_purification_start" ), 0 );
	let playerFaction = SigilGetFaction( pUser );
	let strongholdStatus = "";
	let playerCanCorruptHere = false;
	if( playerFaction !== "" && playerFaction !== currentOwner )
	{
		playerCanCorruptHere = TriggerEvent( sigilStrongholdScriptId, "StrongholdIsObjectAtFactionStronghold", iSigil, playerFaction );
		strongholdStatus = TriggerEvent( sigilStrongholdScriptId, "StrongholdDistanceText", iSigil, playerFaction );
	}

	const myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 430, 350, 9200 );
	myGump.AddHTMLGump( 20, 15, 390, 25, 0, 0, "<CENTER><b>Faction Sigil</b></CENTER>" );
	myGump.AddHTMLGump( 20, 55, 390, 20, 0, 0, "Town: " + townName );
	myGump.AddHTMLGump( 20, 80, 390, 20, 0, 0, "Current Owner: " + currentOwner );

	if( corruptStart > 0 && corrupted != 1 )
	{
		let remaining = sigilCorruptionTime - ( GetCurrentClock() - corruptStart );
		if( remaining < 0 )
			remaining = 0;
		myGump.AddHTMLGump( 20, 105, 390, 20, 0, 0, "Corrupting Faction: " + corruptingFaction );
		myGump.AddHTMLGump( 20, 130, 390, 20, 0, 0, "Corruption remaining: " + Math.ceil( remaining / 60000 ) + " minute(s)" );
	}
	else if( purificationStart > 0 )
	{
		const purificationRemaining = Math.max( 0, sigilPurificationTime - ( GetCurrentClock() - purificationStart ) );
		myGump.AddHTMLGump( 20, 105, 390, 20, 0, 0, "Purifying for " + currentOwner + "." );
		myGump.AddHTMLGump( 20, 130, 390, 20, 0, 0, "Purification remaining: " + Math.ceil( purificationRemaining / 3600000 ) + " hour(s)" );
	}
	else if( corrupted == 1 )
	{
		myGump.AddHTMLGump( 20, 105, 390, 20, 0, 0, "Corrupted by " + iSigil.GetTag( "sigil_corrupted_faction" ) + ". Return it to " + townName + "." );
	}
	if( corrupted == 1 && purificationStart == 0 && iSigil.GetTag( "sigil_corrupted_faction" ) === playerFaction && SigilIsNearHome( iSigil, 3 ) )
	{
		myGump.AddButton( 25, 240, 0xFA5, 1, 0, 2 );
		myGump.AddHTMLGump( 65, 240, 300, 20, 0, 0, "Capture town and begin purification" );
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

	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	const sigilSerial = pSock.tempInt;
	const iSigil = CalcItemFromSer( sigilSerial );
	if( !ValidateObject( iSigil ) )
	{
		pUser.SysMessage( "That sigil is no longer available." );
		return;
	}

	let playerFaction = SigilGetFaction( pUser );
	if( playerFaction === "" )
	{
		pUser.SysMessage( "Only faction members may corrupt sigils." );
		return;
	}

	if( pButton == 1 )
	{
		if( !TriggerEvent( sigilStrongholdScriptId, "StrongholdIsObjectAtFactionStronghold", iSigil, playerFaction ) )
		{
			pUser.SysMessage( "You must bring this sigil to your faction stronghold first: " + TriggerEvent( sigilStrongholdScriptId, "StrongholdLastError" ) );
			return;
		}

		iSigil.SetTag( "sigil_corrupting_faction", playerFaction );
		iSigil.SetTag( "sigil_corruptor_serial", pUser.serial );
		iSigil.SetTag( "sigil_carrier_faction", "" );
		iSigil.SetTag( "sigil_carrier_serial", 0 );
		SigilClearReturnTimer( iSigil );
		iSigil.SetTag( "sigil_corrupt_start", GetCurrentClock() );
		iSigil.SetTag( "sigil_corrupted", 0 );
		iSigil.StartTimer( 60000, sigilCorruptionTimerId, 8502 );
		pUser.SysMessage( "You have begun corrupting this sigil." );
		BroadcastMessage( playerFaction + " has begun corrupting the sigil of " + iSigil.GetTag( "sigil_town" ) + "." );
	}
	else if( pButton == 2 )
	{
		const corruptedFaction = iSigil.GetTag( "sigil_corrupted_faction" );
		if( corruptedFaction !== playerFaction || !SigilIsNearHome( iSigil, 3 ) )
		{
			pUser.SysMessage( "Bring your faction's corrupted sigil to its correct town monolith." );
			return;
		}
		iSigil.SetTag( "sigil_owner_faction", playerFaction );
		iSigil.SetTag( "sigil_purification_start", GetCurrentClock() );
		iSigil.SetTag( "sigil_corrupted", 0 );
		iSigil.SetTag( "sigil_corrupted_faction", "" );
		SigilReturnHome( iSigil );
		iSigil.SetTag( "sigil_owner_faction", playerFaction );
		iSigil.SetTag( "sigil_purification_start", GetCurrentClock() );
		iSigil.StartTimer( 60000, sigilPurificationTimerId, 8502 );
		SigilAwardCapture( iSigil, playerFaction );
		TriggerEvent( sigilTownScriptId, "ApplySigilTownControl", iSigil );
		BroadcastMessage( SigilFactionName( playerFaction ) + " has taken control of " + SigilDisplayTown( iSigil.GetTag( "sigil_town" ) ) + "." );
	}
}

function onTimer( iSigil, timerID )
{
	if( !ValidateObject( iSigil ) )
		return;
	if( iSigil.GetTag( "sigil" ) != 1 )
		return;

	if( timerID == sigilReturnTimerId )
	{
		SigilCheckReturnTimer( iSigil );
		return;
	}

	if( timerID == sigilDropSettleTimerId )
	{
		SigilCheckDropSettled( iSigil );
		return;
	}
	if( timerID == sigilPurificationTimerId )
	{
		let purificationStart = SigilParseNumber( iSigil.GetTag( "sigil_purification_start" ), 0 );
		if( purificationStart > 0 && GetCurrentClock() - purificationStart < sigilPurificationTime )
			iSigil.StartTimer( 60000, sigilPurificationTimerId, 8502 );
		else if( purificationStart > 0 )
		{
			iSigil.SetTag( "sigil_purification_start", 0 );
			BroadcastMessage( "The sigil of " + iSigil.GetTag( "sigil_town" ) + " has been purified." );
		}
		return;
	}

	if( timerID != sigilCorruptionTimerId )
		return;

	let corruptStart = iSigil.GetTag( "sigil_corrupt_start" );
	if( corruptStart > 0 && iSigil.GetTag( "sigil_corrupted" ) != 1 )
	{
		const activeFaction = iSigil.GetTag( "sigil_corrupting_faction" );
		if( !TriggerEvent( sigilStrongholdScriptId, "StrongholdIsObjectAtFactionStronghold", iSigil, activeFaction ) )
		{
			iSigil.SetTag( "sigil_corrupt_start", 0 );
			iSigil.SetTag( "sigil_corrupting_faction", "" );
			iSigil.SetTag( "sigil_corrupted", 0 );
			BroadcastMessage( "The corruption of the " + iSigil.GetTag( "sigil_town" ) + " sigil has failed because it left the stronghold." );
			return;
		}

		if( GetCurrentClock() - corruptStart >= sigilCorruptionTime )
		{
			const corruptingFaction = activeFaction;
			iSigil.SetTag( "sigil_corrupted", 1 );
			iSigil.SetTag( "sigil_corrupted_faction", corruptingFaction );
			iSigil.SetTag( "sigil_corrupt_start", 0 );
			iSigil.SetTag( "sigil_corrupting_faction", "" );
			BroadcastMessage( SigilFactionName( corruptingFaction ) + " has corrupted the sigil of " + SigilDisplayTown( iSigil.GetTag( "sigil_town" ) ) + ". It must be returned to the town monolith." );
			return;
		}
		iSigil.StartTimer( 60000, sigilCorruptionTimerId, 8502 );
	}
}

function SigilCheckReturnTimer( iSigil )
{
	if( !ValidateObject( iSigil ) || iSigil.GetTag( "sigil" ) != 1 )
		return false;
	if( SigilParseNumber( iSigil.GetTag( "sigil_corrupt_start" ), 0 ) > 0 )
		return false;

	const carrierFaction = iSigil.GetTag( "sigil_carrier_faction" );
	if( SigilGetFactionKeyValid( carrierFaction ) && TriggerEvent( sigilStrongholdScriptId, "StrongholdIsObjectAtFactionStronghold", iSigil, carrierFaction ) )
	{
		SigilStartReturnTimer( iSigil );
		return true;
	}

	let townName = iSigil.GetTag( "sigil_town" );
	if( SigilReturnHome( iSigil ) )
		BroadcastMessage( "The sigil of " + townName + " has returned home." );

	return true;
}

function SigilAwardCapture( iSigil, factionKey )
{
	if( !ValidateObject( iSigil ) || !SigilGetFactionKeyValid( factionKey ) )
		return false;

	const ctrl = SigilGetController();
	if( ValidateObject( ctrl ) )
	{
		ctrl.SetTag( "score_" + factionKey, SigilParseNumber( ctrl.GetTag( "score_" + factionKey ), 0 ) + sigilCaptureScoreReward );
		ctrl.SetTag( "captures_" + factionKey, SigilParseNumber( ctrl.GetTag( "captures_" + factionKey ), 0 ) + 1 );
		ctrl.SetTag( "last_capture_town_" + factionKey, iSigil.GetTag( "sigil_town" ) );
		ctrl.SetTag( "last_capture_time_" + factionKey, GetCurrentClock() );
	}

	const corruptorSerial = SigilParseNumber( iSigil.GetTag( "sigil_corruptor_serial" ), 0 );
	const corruptor = CalcCharFromSer( corruptorSerial );
	if( ValidateObject( corruptor ) && TriggerEvent( sigilPlayerDataScriptId, "GetFactionValue", corruptor, "faction", corruptor.GetTag( "faction" ) ) === factionKey )
	{
		const factionData = TriggerEvent( sigilPlayerDataScriptId, "ReadFactionPlayerData", corruptor );
		let silver = SigilParseNumber( factionData.silver, 0 ) + sigilCaptureSilverReward;
		if( silver > 100000 )
			silver = 100000;
		factionData.silver = silver;

		let killPoints = SigilParseNumber( factionData.killPoints, 0 ) + sigilCaptureKillPointReward;
		factionData.killPoints = killPoints;
		factionData.captures = SigilParseNumber( factionData.captures, 0 ) + 1;
		factionData.rank = SigilRankForPoints( killPoints );
		TriggerEvent( sigilPlayerDataScriptId, "WriteFactionPlayerData", corruptor, factionData );
		if( corruptor.socket != null )
			corruptor.SysMessage( "You earned " + sigilCaptureSilverReward + " faction silver and " + sigilCaptureKillPointReward + " kill points for capturing " + SigilDisplayTown( iSigil.GetTag( "sigil_town" ) ) + "." );
	}

	iSigil.SetTag( "sigil_corruptor_serial", 0 );
	return true;
}

function SigilUpdateRank( pChar )
{
	if( !ValidateObject( pChar ) )
		return false;

	const factionData = TriggerEvent( sigilPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	factionData.rank = SigilRankForPoints( factionData.killPoints );
	return TriggerEvent( sigilPlayerDataScriptId, "WriteFactionPlayerData", pChar, factionData );
}

function SigilRankForPoints( killPoints )
{
	const rankPoints = [ 0, 5, 10, 20, 40, 80, 160, 320, 640, 1280 ];
	killPoints = SigilParseNumber( killPoints, 0 );
	let rank = 0;
	for( let i = rankPoints.length - 1; i >= 0; i-- )
	{
		if( killPoints >= rankPoints[i] )
		{
			rank = i;
			break;
		}
	}

	return rank;
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
	if( SigilParseNumber( iSigil.GetTag( "sigil_purification_start" ), 0 ) > 0 )
	{
		if( ValidateObject( pGrabber ) )
			pGrabber.SysMessage( "That sigil is locked to the town monolith while it purifies." );
		return false;
	}

	let grabberFaction = SigilGetFaction( pGrabber );
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

	iSigil.StartTimer( 1000, sigilDropSettleTimerId, 8502 );
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
	sigilIterateMode = "home";
	sigilIterateTown = SigilNormalizeTown( townName );
	sigilIterateCount = 0;
	IterateOver( "ITEM" );
	let count = sigilIterateCount;
	sigilIterateMode = "";
	sigilIterateTown = "";
	sigilIterateCount = 0;
	return count;
}

function SigilReturn( townName )
{
	sigilIterateMode = "return";
	sigilIterateTown = SigilNormalizeTown( townName );
	sigilIterateCount = 0;
	IterateOver( "ITEM" );
	let count = sigilIterateCount;
	sigilIterateMode = "";
	sigilIterateTown = "";
	sigilIterateCount = 0;
	return count;
}

function ShowSigilStatus( pSock, townName )
{
	sigilIterateMode = "status";
	sigilIterateTown = SigilNormalizeTown( townName );
	sigilIterateSocket = pSock;
	sigilIterateCount = 0;
	IterateOver( "ITEM" );
	let count = sigilIterateCount;
	sigilIterateMode = "";
	sigilIterateTown = "";
	sigilIterateSocket = null;
	sigilIterateCount = 0;
	return count;
}

function SigilSetReturnTime( minutes )
{
	minutes = SigilParseNumber( minutes, 0 );
	if( minutes < 1 )
		return 0;

	sigilIterateMode = "delay";
	sigilIterateTown = "";
	sigilIterateDelay = minutes * 60000;
	sigilIterateCount = 0;
	IterateOver( "ITEM" );
	let count = sigilIterateCount;
	sigilIterateMode = "";
	sigilIterateDelay = 0;
	sigilIterateCount = 0;
	return count;
}

function ShowSigilReturnTime( pSock )
{
	if( pSock == null )
		return false;

	pSock.SysMessage( "Default sigil return time: " + Math.ceil( sigilReturnTimeDefault / 60000 ) + " minute(s)." );
	sigilIterateMode = "delaystatus";
	sigilIterateTown = "";
	sigilIterateSocket = pSock;
	sigilIterateCount = 0;
	IterateOver( "ITEM" );
	let count = sigilIterateCount;
	sigilIterateMode = "";
	sigilIterateSocket = null;
	sigilIterateCount = 0;
	if( count == 0 )
		pSock.SysMessage( "No faction sigils found." );

	return true;
}

function ShowFactionScore( pSock )
{
	if( pSock == null )
		return false;

	const ctrl = SigilGetController();
	if( !ValidateObject( ctrl ) )
	{
		pSock.SysMessage( "Faction controller was not found." );
		return false;
	}

	const factionKeys = [ "TB", "COM", "MIN", "SL" ];
	for( let i = 0; i < factionKeys.length; i++ )
	{
		let factionKey = factionKeys[i];
		const score = SigilParseNumber( ctrl.GetTag( "score_" + factionKey ), 0 );
		let captures = SigilParseNumber( ctrl.GetTag( "captures_" + factionKey ), 0 );
		let lastTown = ctrl.GetTag( "last_capture_town_" + factionKey );
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

	const ctrl = SigilGetController();
	if( !ValidateObject( ctrl ) )
		return "Score: 0, Captures: 0";

	return "Score: " + SigilParseNumber( ctrl.GetTag( "score_" + factionKey ), 0 ) + ", Captures: " + SigilParseNumber( ctrl.GetTag( "captures_" + factionKey ), 0 );
}

function FactionScoreValue( factionKey )
{
	if( !SigilGetFactionKeyValid( factionKey ) )
		return 0;

	const ctrl = SigilGetController();
	if( !ValidateObject( ctrl ) )
		return 0;

	return SigilParseNumber( ctrl.GetTag( "score_" + factionKey ), 0 );
}

function SpendFactionScore( factionKey, amount )
{
	if( !SigilGetFactionKeyValid( factionKey ) )
		return false;

	const ctrl = SigilGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	amount = SigilParseNumber( amount, 0 );
	if( amount < 1 )
		return true;

	const score = SigilParseNumber( ctrl.GetTag( "score_" + factionKey ), 0 );
	if( score < amount )
		return false;

	ctrl.SetTag( "score_" + factionKey, score - amount );
	return true;
}

function SetFactionNotice( factionKey, messageText, setterName )
{
	if( !SigilGetFactionKeyValid( factionKey ) )
		return false;

	const ctrl = SigilGetController();
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

	const ctrl = SigilGetController();
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

	const ctrl = SigilGetController();
	if( !ValidateObject( ctrl ) )
		return "Notice: None";

	let noticeText = ctrl.GetTag( "notice_" + factionKey );
	if( noticeText === "" || noticeText == 0 )
		return "Notice: None";

	return "Notice: " + noticeText;
}

function ResetFactionScore()
{
	const ctrl = SigilGetController();
	if( !ValidateObject( ctrl ) )
		return false;

	const factionKeys = [ "TB", "COM", "MIN", "SL" ];
	for( let i = 0; i < factionKeys.length; i++ )
	{
		let factionKey = factionKeys[i];
		ctrl.SetTag( "score_" + factionKey, 0 );
		ctrl.SetTag( "captures_" + factionKey, 0 );
		ctrl.SetTag( "last_capture_town_" + factionKey, "" );
		ctrl.SetTag( "last_capture_time_" + factionKey, 0 );
	}

	return true;
}

function onIterate( toCheck )
{
	if( sigilIterateMode === "controller" )
	{
		if( ValidateObject( toCheck ) && toCheck.isItem && toCheck.GetTag( "faction_controller" ) == 1 )
		{
			sigilController = toCheck;
			return true;
		}

		return false;
	}

	if( !SigilMatchesTown( toCheck, sigilIterateTown ) )
		return false;

	if( sigilIterateMode === "home" )
	{
		if( SigilSetHome( toCheck ) )
			sigilIterateCount++;
		return false;
	}

	if( sigilIterateMode === "return" )
	{
		if( SigilReturnHome( toCheck ) )
			sigilIterateCount++;
		return false;
	}

	if( sigilIterateMode === "status" )
	{
		sigilIterateCount++;
		if( sigilIterateSocket != null )
		{
			const homeText = SigilHasHome( toCheck ) ? "home set" : "home missing";
			const carrierFaction = toCheck.GetTag( "sigil_carrier_faction" );
			const returnDue = SigilParseNumber( toCheck.GetTag( "sigil_return_due" ), 0 );
			let carriedText = "";
			if( carrierFaction !== "" && carrierFaction != 0 )
				carriedText = ", carried by " + carrierFaction;
			if( returnDue > GetCurrentClock() )
				carriedText += ", returns in " + Math.ceil( ( returnDue - GetCurrentClock() ) / 60000 ) + " min";
			else
				carriedText += ", no return timer";

			sigilIterateSocket.SysMessage( SigilDisplayTown( toCheck.GetTag( "sigil_town" ) ) + ": owner " + SigilOwnerFaction( toCheck ) + ", at " + toCheck.x + "," + toCheck.y + "," + toCheck.z + ", " + homeText + carriedText );
		}
		return false;
	}

	if( sigilIterateMode === "delay" )
	{
		toCheck.SetTag( "sigil_return_delay", sigilIterateDelay );
		if( SigilParseNumber( toCheck.GetTag( "sigil_return_due" ), 0 ) > 0 )
			SigilStartReturnTimer( toCheck );
		sigilIterateCount++;
		return false;
	}

	if( sigilIterateMode === "delaystatus" )
	{
		sigilIterateCount++;
		if( sigilIterateSocket != null )
			sigilIterateSocket.SysMessage( SigilDisplayTown( toCheck.GetTag( "sigil_town" ) ) + ": return time " + Math.ceil( SigilReturnDelay( toCheck ) / 60000 ) + " minute(s)." );
		return false;
	}

	return false;
}
