// =============================================================================
// factions_sigils.js
// UOX3 Faction System - simple sigil capture and town control
// Script ID suggestion: 8502
// Attach this script to each sigil item.
// =============================================================================

var SigilCorruptionTime = 36000000;
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

function SigilGetFaction( pChar )
{
	if( !ValidateObject( pChar ) )
		return "";
	var factionKey = pChar.GetTag( "faction" );
	if( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" )
		return factionKey;
	return "";
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
	var playerFaction = SigilGetFaction( pUser );

	var myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 430, 310, 9200 );
	myGump.AddHTMLGump( 20, 15, 390, 25, 0, 0, "<CENTER><b>Faction Sigil</b></CENTER>" );
	myGump.AddHTMLGump( 20, 55, 390, 20, 0, 0, "Town: " + townName );
	myGump.AddHTMLGump( 20, 80, 390, 20, 0, 0, "Current Owner: " + currentOwner );

	if( corruptStart > 0 && corrupted != 1 )
	{
		var remaining = SigilCorruptionTime - ( GetCurrentClock() - corruptStart );
		if( remaining < 0 )
			remaining = 0;
		myGump.AddHTMLGump( 20, 105, 390, 20, 0, 0, "Corruption remaining: " + Math.ceil( remaining / 60000 ) + " minute(s)" );
	}
	else if( corrupted == 1 )
	{
		myGump.AddHTMLGump( 20, 105, 390, 20, 0, 0, "This town is controlled by " + currentOwner + "." );
	}

	if( playerFaction !== "" && playerFaction !== currentOwner )
	{
		myGump.AddButton( 25, 165, 0xFA5, 1, 0, 1 );
		myGump.AddHTMLGump( 65, 165, 280, 20, 0, 0, "Begin corruption for your faction" );
	}

	myGump.AddButton( 25, 260, 0xFA5, 1, 0, 0 );
	myGump.AddHTMLGump( 65, 260, 100, 20, 0, 0, "Close" );
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
		iSigil.SetTag( "sigil_owner_faction", playerFaction );
		iSigil.SetTag( "sigil_corrupt_start", GetCurrentClock() );
		iSigil.SetTag( "sigil_corrupted", 0 );
		iSigil.StartTimer( 60000, 1, 8502 );
		pUser.SysMessage( "You have begun corrupting this sigil." );
		BroadcastMessage( playerFaction + " has begun corrupting the sigil of " + iSigil.GetTag( "sigil_town" ) + "." );
	}
}

function onTimer( iSigil, timerID )
{
	if( !ValidateObject( iSigil ) )
		return;
	if( timerID != 1 )
		return;
	if( iSigil.GetTag( "sigil" ) != 1 )
		return;

	var corruptStart = iSigil.GetTag( "sigil_corrupt_start" );
	if( corruptStart > 0 && iSigil.GetTag( "sigil_corrupted" ) != 1 )
	{
		if( GetCurrentClock() - corruptStart >= SigilCorruptionTime )
		{
			iSigil.SetTag( "sigil_corrupted", 1 );
			iSigil.SetTag( "sigil_corrupt_start", 0 );
			BroadcastMessage( iSigil.GetTag( "sigil_owner_faction" ) + " has taken control of " + iSigil.GetTag( "sigil_town" ) + "." );
			return;
		}
		iSigil.StartTimer( 60000, 1, 8502 );
	}
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
