// =============================================================================
// factions_stones.js
// UOX3 Faction System - faction stones and join stones
// Script ID suggestion: 8505
// =============================================================================

var StoneFactionNames = {
	TB: "True Britannians",
	COM: "Council of Mages",
	MIN: "Minax",
	SL: "Shadowlords"
};

var StoneRankNames = [
	"Soldier",
	"Scout",
	"Corporal",
	"Sergeant",
	"Lieutenant",
	"Captain",
	"Major",
	"Colonel",
	"General",
	"Commander"
];

var StoneRankPoints = [ 0, 5, 10, 20, 40, 80, 160, 320, 640, 1280 ];
var StoneLeaveDelay = 259200000;
var StoneSigilScriptId = 8502;
var StoneCoreScriptId = 8500;
var StoneElectionScriptId = 8508;
var StoneTownScriptId = 8509;
var StonePlayerDataScriptId = 8513;

function StoneIsFactionValid( factionKey )
{
	return ( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" );
}

function StoneFactionName( factionKey )
{
	if( !StoneIsFactionValid( factionKey ) )
		return "Unknown Faction";

	return StoneFactionNames[factionKey];
}

function StoneGetFaction( pChar )
{
	if( !ValidateObject( pChar ) )
		return "";

	var factionKey = TriggerEvent( StonePlayerDataScriptId, "GetFactionValue", pChar, "faction", pChar.GetTag( "faction" ) );
	if( StoneIsFactionValid( factionKey ) )
		return factionKey;

	return "";
}

function StoneGetRankName( pChar )
{
	var rank = TriggerEvent( StonePlayerDataScriptId, "GetFactionValue", pChar, "rank", pChar.GetTag( "faction_rank" ) );
	if( rank < 0 || rank >= StoneRankNames.length )
		rank = 0;

	return StoneRankNames[rank];
}

function StoneRoleName( pChar )
{
	if( !ValidateObject( pChar ) )
		return "None";

	var factionData = TriggerEvent( StonePlayerDataScriptId, "ReadFactionPlayerData", pChar );
	var roleName = factionData.role;
	if( roleName === "commander" || factionData.commander )
		return "Commander";
	if( roleName === "sheriff" )
		return "Sheriff";
	if( roleName === "finance" )
		return "Finance Minister";

	return "None";
}

function StoneUpdateRank( pChar )
{
	var factionData = TriggerEvent( StonePlayerDataScriptId, "ReadFactionPlayerData", pChar );
	var killPoints = factionData.killPoints;
	var rank = 0;
	for( var rankIndex = StoneRankPoints.length - 1; rankIndex >= 0; rankIndex-- )
	{
		if( killPoints >= StoneRankPoints[rankIndex] )
		{
			rank = rankIndex;
			break;
		}
	}
	factionData.rank = rank;
	TriggerEvent( StonePlayerDataScriptId, "WriteFactionPlayerData", pChar, factionData );
}

function StoneJoinFaction( pChar, factionKey )
{
	return TriggerEvent( StoneCoreScriptId, "JoinFaction", pChar, factionKey );
}

function StoneLeaveFaction( pChar )
{
	return TriggerEvent( StoneCoreScriptId, "LeaveFaction", pChar );
}

function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ) )
		return false;

	var pSock = pUser.socket;
	if( !pSock )
		return false;

	var stoneFaction = iUsed.GetTag( "stone_faction" );
	if( !StoneIsFactionValid( stoneFaction ) )
	{
		pUser.SysMessage( "This stone has no faction assigned." );
		return false;
	}

	if( iUsed.GetTag( "join_stone" ) == 1 )
	{
		ShowJoinStoneGump( pSock, pUser, stoneFaction );
		return false;
	}

	if( iUsed.GetTag( "faction_stone" ) == 1 )
	{
		ShowFactionStoneGump( pSock, pUser, stoneFaction );
		return false;
	}

	return true;
}

function ShowJoinStoneGump( pSock, pUser, stoneFaction )
{
	var myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 430, 260, 9200 );
	myGump.AddHTMLGump( 20, 15, 390, 25, 0, 0, "<CENTER><b>" + StoneFactionName( stoneFaction ) + "</b></CENTER>" );

	if( StoneGetFaction( pUser ) !== "" )
	{
		myGump.AddHTMLGump( 20, 55, 390, 80, 0, 0, "You are already a faction member. Leave your current faction before joining another." );
	}
	else
	{
		myGump.AddHTMLGump( 20, 55, 390, 90, 0, 0, "Join the " + StoneFactionName( stoneFaction ) + " to fight for town control, earn faction silver and gain faction rank." );
		myGump.AddButton( 25, 155, 0xFA5, 1, 0, 1 );
		myGump.AddHTMLGump( 65, 155, 260, 20, 0, 0, "Join this faction" );
	}

	myGump.AddButton( 25, 215, 0xFA5, 1, 0, 0 );
	myGump.AddHTMLGump( 65, 215, 100, 20, 0, 0, "Close" );
	myGump.Send( pSock );
	myGump.Free();

	pSock.xText = "join";
	pSock.xText2 = stoneFaction;
}

function ShowFactionStoneGump( pSock, pUser, stoneFaction )
{
	var playerFaction = StoneGetFaction( pUser );
	var myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 440, 400, 9200 );
	myGump.AddHTMLGump( 20, 15, 400, 25, 0, 0, "<CENTER><b>Faction Stone</b></CENTER>" );
	myGump.AddHTMLGump( 20, 45, 400, 20, 0, 0, "Stone Faction: " + StoneFactionName( stoneFaction ) );

	if( playerFaction === "" )
	{
		myGump.AddHTMLGump( 20, 85, 400, 60, 0, 0, "You are not in a faction. Use a join stone to enlist." );
	}
	else
	{
		StoneUpdateRank( pUser );
		var factionData = TriggerEvent( StonePlayerDataScriptId, "ReadFactionPlayerData", pUser );
		myGump.AddHTMLGump( 20, 85, 400, 20, 0, 0, "Your Faction: " + StoneFactionName( playerFaction ) );
		myGump.AddHTMLGump( 20, 110, 400, 20, 0, 0, "Rank: " + StoneGetRankName( pUser ) );
		myGump.AddHTMLGump( 20, 135, 400, 20, 0, 0, "Kill Points: " + factionData.killPoints );
		myGump.AddHTMLGump( 20, 160, 400, 20, 0, 0, "Silver: " + factionData.silver );
		myGump.AddHTMLGump( 20, 185, 400, 20, 0, 0, "Role: " + StoneRoleName( pUser ) );
		myGump.AddHTMLGump( 20, 210, 400, 20, 0, 0, TriggerEvent( StoneSigilScriptId, "FactionScoreText", stoneFaction ) );
		myGump.AddHTMLGump( 20, 235, 400, 35, 0, 0, TriggerEvent( StoneSigilScriptId, "FactionNoticeText", stoneFaction ) );
		myGump.AddHTMLGump( 20, 275, 400, 35, 0, 0, "Controlled Towns: " + TriggerEvent( StoneTownScriptId, "TownControlledByFactionList", stoneFaction ) );

		if( playerFaction === stoneFaction )
		{
			myGump.AddButton( 25, 315, 0xFA5, 1, 0, 3 );
			myGump.AddHTMLGump( 65, 315, 250, 20, 0, 0, "Faction election" );
			myGump.AddButton( 25, 345, 0xFA5, 1, 0, 2 );
			myGump.AddHTMLGump( 65, 345, 250, 20, 0, 0, "Leave this faction" );
		}
	}

	myGump.AddButton( 25, 370, 0xFA5, 1, 0, 0 );
	myGump.AddHTMLGump( 65, 370, 100, 20, 0, 0, "Close" );
	myGump.Send( pSock );
	myGump.Free();

	pSock.xText = "stone";
	pSock.xText2 = stoneFaction;
}

function onGumpPress( pSock, pButton, gumpData )
{
	if( pButton == 0 )
		return;

	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var action = pSock.xText;
	var factionKey = pSock.xText2;

	if( pButton == 1 && action === "join" )
	{
		StoneJoinFaction( pUser, factionKey );
		return;
	}

	if( pButton == 2 && action === "stone" )
	{
		StoneLeaveFaction( pUser );
		return;
	}

	if( pButton == 3 && action === "stone" )
	{
		if( StoneGetFaction( pUser ) !== factionKey )
		{
			pUser.SysMessage( "You may only use your own faction election stone." );
			return;
		}

		TriggerEvent( StoneElectionScriptId, "ShowElectionGump", pSock, pUser, factionKey );
		return;
	}
}
