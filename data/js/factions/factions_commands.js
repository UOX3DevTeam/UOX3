// =============================================================================
// factions_commands.js
// UOX3 Faction System - GM testing commands
// COMMAND_SCRIPTS ID suggestion: 1071
// =============================================================================

var CommandMaxFactionSilver = 100000;
var CommandFactionCombatScriptId = 8501;
var CommandFactionSigilScriptId = 8502;
var CommandFactionElectionScriptId = 8508;
var CommandFactionTownScriptId = 8509;
var CommandFactionStrongholdScriptId = 8511;
var CommandFactionGuardCost = 250;
var CommandFactionVendorCost = 500;
var CommandFactionAdminHealthButton = 9001;
var CommandFactionAdminSetupButton = 9002;
var CommandFactionAdminDedupeButton = 9003;
var CommandFactionAdminSyncTownsButton = 9004;
var CommandFactionAdminTownStatusButton = 9005;
var CommandFactionAdminTreasuryButton = 9006;
var CommandFactionAdminTaxStartButton = 9007;
var CommandFactionAdminTaxStopButton = 9008;
var CommandFactionAdminTaxStatusButton = 9009;
var CommandFactionAdminSigilsButton = 9010;
var CommandFactionAdminStrongholdsButton = 9011;
var CommandFactionAdminRemoveConfirmButton = 9012;
var CommandFactionAdminRemoveNowButton = 9013;
var CommandFactionAdminBackButton = 9014;

var CommandFactionSetupItems = [
	[ "JOIN_STONE_TB", -4, -2 ],
	[ "FACTION_STONE_TB", -4, -1 ],
	[ "JOIN_STONE_COM", -1, -2 ],
	[ "FACTION_STONE_COM", -1, -1 ],
	[ "JOIN_STONE_MIN", 2, -2 ],
	[ "FACTION_STONE_MIN", 2, -1 ],
	[ "JOIN_STONE_SL", 5, -2 ],
	[ "FACTION_STONE_SL", 5, -1 ],
	[ "FACTION_SIGIL_BRITAIN", -4, 2 ],
	[ "FACTION_SIGIL_TRINSIC", -2, 2 ],
	[ "FACTION_SIGIL_MOONGLOW", 0, 2 ],
	[ "FACTION_SIGIL_SKARABRAE", 2, 2 ],
	[ "FACTION_SIGIL_YEW", 4, 2 ],
	[ "FACTION_SIGIL_VESPER", -3, 4 ],
	[ "FACTION_SIGIL_MINOC", -1, 4 ],
	[ "FACTION_SIGIL_COVE", 1, 4 ]
];

var CommandFactionRegionChecks = [
	[ "Britain", 3 ],
	[ "Trinsic", 9 ],
	[ "Moonglow", 13 ],
	[ "Skara Brae", 8 ],
	[ "Yew", 7 ],
	[ "Vesper", 11 ],
	[ "Minoc", 12 ],
	[ "Cove", 10 ]
];

var CommandFactionVendorTypes = [
	"REAGENT",
	"BOARD",
	"BOTTLE",
	"EQUIPMENT",
	"HORSE"
];

var CommandFactionRoles = [
	"sheriff",
	"finance",
	"commander"
];

var CommandFindPlayerName = "";
var CommandFindPlayerResult = null;
var CommandRoleListSocket = null;
var CommandClearRoleName = "";
var CommandClearRoleFaction = "";
var CommandClearRoleExcept = 0;
var CommandBroadcastFaction = "";
var CommandBroadcastMessage = "";
var CommandBroadcastCount = 0;
var CommandHealthScanActive = false;
var CommandHealthControllerCount = 0;
var CommandHealthFactionStoneCount = 0;
var CommandHealthJoinStoneCount = 0;
var CommandHealthSigilCount = 0;
var CommandHealthSigilHomeCount = 0;
var CommandSetupScanActive = false;
var CommandSetupFound = {};
var CommandSetupRanks = {};
var CommandDedupeActive = false;
var CommandDedupeKept = {};
var CommandDedupeRanks = {};
var CommandDedupeRemovedCount = 0;
var CommandRemoveFactionsMode = "";
var CommandRemoveFactionsItems = 0;
var CommandRemoveFactionsNpcs = 0;
var CommandRemoveFactionsPlayers = 0;
var CommandRemoveFactionsRegions = 0;

function CommandRegistration()
{
	RegisterCommand( "factionsilver", 5, true );
	RegisterCommand( "factioncontroller", 5, true );
	RegisterCommand( "factionsetup", 5, true );
	RegisterCommand( "factionregioncheck", 5, true );
	RegisterCommand( "factiontowns", 0, true );
	RegisterCommand( "factiontownset", 5, true );
	RegisterCommand( "factionstatus", 0, true );
	RegisterCommand( "factionadmin", 5, true );
	RegisterCommand( "factionhealth", 5, true );
	RegisterCommand( "factiondedupe", 5, true );
	RegisterCommand( "removefactions", 5, true );
	RegisterCommand( "factiontownsync", 5, true );
	RegisterCommand( "factiontownnpcs", 5, true );
	RegisterCommand( "factiontownclear", 5, true );
	RegisterCommand( "factiontreasury", 0, true );
	RegisterCommand( "factiontreasurygrant", 5, true );
	RegisterCommand( "factiontreasuryset", 5, true );
	RegisterCommand( "factiontaxcycle", 5, true );
	RegisterCommand( "factiontaxrate", 5, true );
	RegisterCommand( "factiontaxstart", 5, true );
	RegisterCommand( "factiontaxstop", 5, true );
	RegisterCommand( "factiontaxstatus", 5, true );
	RegisterCommand( "factionstronghold", 5, true );
	RegisterCommand( "factionstrongholds", 5, true );
	RegisterCommand( "factionsigilhome", 5, true );
	RegisterCommand( "factionsigilreturn", 5, true );
	RegisterCommand( "factionsigilreturntime", 5, true );
	RegisterCommand( "factionsigils", 5, true );
	RegisterCommand( "factionscore", 0, true );
	RegisterCommand( "factionscorereset", 5, true );
	RegisterCommand( "factionkills", 0, true );
	RegisterCommand( "factionkillsreset", 5, true );
	RegisterCommand( "factionnotice", 0, true );
	RegisterCommand( "factionnoticeclear", 0, true );
	RegisterCommand( "factionalert", 0, true );
	RegisterCommand( "factionrole", 5, true );
	RegisterCommand( "factionroleclear", 5, true );
	RegisterCommand( "factionroles", 5, true );
	RegisterCommand( "factionappoint", 0, true );
	RegisterCommand( "factionguard", 0, true );
	RegisterCommand( "factionvendor", 0, true );
	RegisterCommand( "factionitemcheck", 5, true );
	RegisterCommand( "factionitemcleanup", 5, true );
	RegisterCommand( "spawnfitem", 5, true );
	RegisterCommand( "spawntrapdeed", 5, true );

	RegisterCommand( "electionstart", 5, true );
	RegisterCommand( "electionvote", 5, true );
	RegisterCommand( "electionend", 5, true );
	RegisterCommand( "electionstatus", 0, true );
	RegisterCommand( "electionreset", 5, true );
}

function CommandIsFactionValid( factionKey )
{
	return ( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" );
}

function CommandIsVendorTypeValid( vendorType )
{
	for( var typeIndex = 0; typeIndex < CommandFactionVendorTypes.length; typeIndex++ )
	{
		if( CommandFactionVendorTypes[typeIndex] === vendorType )
			return true;
	}

	return false;
}

function CommandIsRoleValid( roleName )
{
	for( var roleIndex = 0; roleIndex < CommandFactionRoles.length; roleIndex++ )
	{
		if( CommandFactionRoles[roleIndex] === roleName )
			return true;
	}

	return false;
}

function CommandRoleDisplayName( roleName )
{
	if( roleName === "" || roleName == 0 )
		return "None";
	if( roleName === "sheriff" )
		return "Sheriff";
	if( roleName === "finance" )
		return "Finance Minister";
	if( roleName === "commander" )
		return "Commander";

	return roleName;
}

function CommandFactionUsageName( factionKey )
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

function CommandRankName( rank )
{
	var rankNames = [
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

	rank = parseInt( rank, 10 );
	if( isNaN( rank ) || rank < 0 || rank >= rankNames.length )
		rank = 0;

	return rankNames[rank];
}

function CommandIsStaff( pUser )
{
	if( !ValidateObject( pUser ) )
		return false;

	return ( pUser.isGM || pUser.commandlevel >= 5 );
}

function CommandClampSilver( amount )
{
	if( amount < 0 )
		amount = 0;
	if( amount > CommandMaxFactionSilver )
		amount = CommandMaxFactionSilver;

	return amount;
}

function CommandFindPlayer( playerText )
{
	CommandFindPlayerResult = null;
	CommandFindPlayerName = String( playerText ).replace( /^\s+|\s+$/g, "" ).toLowerCase();
	if( CommandFindPlayerName === "" )
		return null;

	var serialNum = parseInt( CommandFindPlayerName, 10 );
	if( !isNaN( serialNum ) )
	{
		var serialChar = CalcCharFromSer( serialNum );
		if( ValidateObject( serialChar ) && serialChar.isChar && !serialChar.npc )
		{
			CommandFindPlayerResult = null;
			CommandFindPlayerName = "";
			return serialChar;
		}
	}

	IterateOver( "CHARACTER" );
	var foundPlayer = CommandFindPlayerResult;
	CommandFindPlayerResult = null;
	CommandFindPlayerName = "";
	return foundPlayer;
}

function CommandSetFactionRole( pChar, roleName, factionKey )
{
	if( !ValidateObject( pChar ) || !CommandIsRoleValid( roleName ) || !CommandIsFactionValid( factionKey ) )
		return false;

	CommandClearFactionRoleHolders( roleName, factionKey, pChar.serial );
	pChar.SetTag( "faction_role", roleName );
	pChar.SetTag( "faction_role_faction", factionKey );
	pChar.SetTag( "faction_role_set_at", GetCurrentClock() );
	if( roleName === "commander" )
		pChar.SetTag( "faction_commander", "1" );

	return true;
}

function CommandClearFactionRole( pChar )
{
	if( !ValidateObject( pChar ) )
		return false;

	pChar.SetTag( "faction_role", "" );
	pChar.SetTag( "faction_role_faction", "" );
	pChar.SetTag( "faction_role_set_at", 0 );
	pChar.SetTag( "faction_commander", "0" );
	return true;
}

function CommandClearFactionRoleHolders( roleName, factionKey, exceptSerial )
{
	if( !CommandIsRoleValid( roleName ) || !CommandIsFactionValid( factionKey ) )
		return 0;

	CommandClearRoleName = roleName;
	CommandClearRoleFaction = factionKey;
	CommandClearRoleExcept = exceptSerial;
	var clearCount = IterateOver( "CHARACTER" );
	CommandClearRoleName = "";
	CommandClearRoleFaction = "";
	CommandClearRoleExcept = 0;
	return clearCount;
}

function CommandHasFactionRole( pChar, roleName, factionKey )
{
	if( !ValidateObject( pChar ) || !CommandIsRoleValid( roleName ) || !CommandIsFactionValid( factionKey ) )
		return false;

	if( pChar.GetTag( "faction" ) !== factionKey )
		return false;
	if( roleName === "commander" && pChar.GetTag( "faction_commander" ) == "1" )
		return true;
	if( pChar.GetTag( "faction_role" ) !== roleName )
		return false;

	return pChar.GetTag( "faction_role_faction" ) === factionKey;
}

function CommandCanAppointFactionRole( pUser, factionKey )
{
	if( CommandIsStaff( pUser ) )
		return true;

	return CommandHasFactionRole( pUser, "commander", factionKey );
}

function onIterate( toCheck )
{
	if( CommandFindPlayerName !== "" )
	{
		if( ValidateObject( toCheck ) && toCheck.isChar && !toCheck.npc )
		{
			var checkName = String( toCheck.name ).toLowerCase();
			if( checkName === CommandFindPlayerName )
			{
				CommandFindPlayerResult = toCheck;
				return true;
			}
		}

		return false;
	}

	if( CommandRoleListSocket != null )
	{
		if( ValidateObject( toCheck ) && toCheck.isChar && !toCheck.npc )
		{
			var roleName = toCheck.GetTag( "faction_role" );
			var roleFaction = toCheck.GetTag( "faction_role_faction" );
			if( CommandIsRoleValid( roleName ) && CommandIsFactionValid( roleFaction ) )
			{
				CommandRoleListSocket.SysMessage( toCheck.name + ": " + CommandFactionUsageName( roleFaction ) + " " + CommandRoleDisplayName( roleName ) );
				return true;
			}
		}

		return false;
	}

	if( CommandClearRoleName !== "" )
	{
		if( ValidateObject( toCheck ) && toCheck.isChar && !toCheck.npc && toCheck.serial != CommandClearRoleExcept )
		{
			if( toCheck.GetTag( "faction_role" ) === CommandClearRoleName && toCheck.GetTag( "faction_role_faction" ) === CommandClearRoleFaction )
			{
				toCheck.SetTag( "faction_role", "" );
				toCheck.SetTag( "faction_role_faction", "" );
				toCheck.SetTag( "faction_role_set_at", 0 );
				if( CommandClearRoleName === "commander" )
					toCheck.SetTag( "faction_commander", "0" );
				return true;
			}
		}

		return false;
	}

	if( CommandBroadcastFaction !== "" )
	{
		if( ValidateObject( toCheck ) && toCheck.isChar && !toCheck.npc && toCheck.GetTag( "faction" ) === CommandBroadcastFaction )
		{
			if( toCheck.socket != null )
			{
				toCheck.SysMessage( CommandBroadcastMessage );
				CommandBroadcastCount++;
			}
		}

		return false;
	}

	if( CommandHealthScanActive )
	{
		if( ValidateObject( toCheck ) && toCheck.isItem )
		{
			if( toCheck.GetTag( "faction_controller" ) == 1 )
				CommandHealthControllerCount++;
			if( toCheck.GetTag( "faction_stone" ) == 1 )
				CommandHealthFactionStoneCount++;
			if( toCheck.GetTag( "join_stone" ) == 1 )
				CommandHealthJoinStoneCount++;
			if( toCheck.GetTag( "sigil" ) == 1 )
			{
				CommandHealthSigilCount++;
				if( toCheck.GetTag( "sigil_home_set" ) == 1 )
					CommandHealthSigilHomeCount++;
			}
		}

		return false;
	}

	if( CommandSetupScanActive )
	{
		if( ValidateObject( toCheck ) && toCheck.isItem )
		{
			var setupKey = CommandDedupeItemKey( toCheck );
			if( setupKey !== "" )
			{
				var setupRank = CommandDedupeItemRank( toCheck );
				if( typeof CommandSetupFound[setupKey] == "undefined" || !ValidateObject( CommandSetupFound[setupKey] ) || setupRank > CommandSetupRanks[setupKey] )
				{
					CommandSetupFound[setupKey] = toCheck;
					CommandSetupRanks[setupKey] = setupRank;
				}
			}
		}

		return false;
	}

	if( CommandDedupeActive )
	{
		if( ValidateObject( toCheck ) && toCheck.isItem )
		{
			var dedupeKey = CommandDedupeItemKey( toCheck );
			if( dedupeKey !== "" )
			{
				var dedupeRank = CommandDedupeItemRank( toCheck );
				if( typeof CommandDedupeKept[dedupeKey] != "undefined" && ValidateObject( CommandDedupeKept[dedupeKey] ) )
				{
					if( dedupeRank > CommandDedupeRanks[dedupeKey] )
					{
						CommandDedupeKept[dedupeKey].Delete();
						CommandDedupeKept[dedupeKey] = toCheck;
						CommandDedupeRanks[dedupeKey] = dedupeRank;
						CommandDedupeRemovedCount++;
					}
					else
					{
						toCheck.Delete();
						CommandDedupeRemovedCount++;
					}
				}
				else
				{
					CommandDedupeKept[dedupeKey] = toCheck;
					CommandDedupeRanks[dedupeKey] = dedupeRank;
				}
			}
		}

		return false;
	}

	if( CommandRemoveFactionsMode === "items" )
	{
		if( CommandRemoveFactionItem( toCheck ) )
			return true;

		return false;
	}

	if( CommandRemoveFactionsMode === "chars" )
	{
		if( CommandRemoveFactionChar( toCheck ) )
			return true;

		return false;
	}

	return false;
}

function command_FACTIONCONTROLLER( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var controller = CreateDFNItem( pSock, pUser, "FACTION_CONTROLLER", 1, "ITEM", true );
	if( ValidateObject( controller ) )
	{
		pSock.SysMessage( "Faction controller created." );
		TriggerEvent( CommandFactionElectionScriptId, "RegisterController", controller );
		TriggerEvent( CommandFactionTownScriptId, "RegisterController", controller );
		TriggerEvent( CommandFactionTownScriptId, "SyncTownControl" );
	}
	else
	{
		pSock.SysMessage( "Unable to create faction controller." );
	}
}

function CommandPlaceFactionItem( pSock, pUser, sectionId, xOffset, yOffset )
{
	var newItem = CreateDFNItem( pSock, pUser, sectionId, 1, "ITEM", false );
	if( !ValidateObject( newItem ) )
		return null;

	newItem.SetLocation( pUser.x + xOffset, pUser.y + yOffset, pUser.z, pUser.worldnumber, pUser.instanceID );
	return newItem;
}

function CommandSetupScanItems()
{
	CommandSetupFound = {};
	CommandSetupRanks = {};
	CommandSetupScanActive = true;
	IterateOver( "ITEM" );
	CommandSetupScanActive = false;
}

function CommandSetupSectionKey( sectionId )
{
	if( sectionId === "JOIN_STONE_TB" )
		return "join_stone_TB";
	if( sectionId === "JOIN_STONE_COM" )
		return "join_stone_COM";
	if( sectionId === "JOIN_STONE_MIN" )
		return "join_stone_MIN";
	if( sectionId === "JOIN_STONE_SL" )
		return "join_stone_SL";

	if( sectionId === "FACTION_STONE_TB" )
		return "faction_stone_TB";
	if( sectionId === "FACTION_STONE_COM" )
		return "faction_stone_COM";
	if( sectionId === "FACTION_STONE_MIN" )
		return "faction_stone_MIN";
	if( sectionId === "FACTION_STONE_SL" )
		return "faction_stone_SL";

	if( sectionId === "FACTION_SIGIL_BRITAIN" )
		return "sigil_Britain";
	if( sectionId === "FACTION_SIGIL_TRINSIC" )
		return "sigil_Trinsic";
	if( sectionId === "FACTION_SIGIL_MOONGLOW" )
		return "sigil_Moonglow";
	if( sectionId === "FACTION_SIGIL_SKARABRAE" )
		return "sigil_SkaraBrae";
	if( sectionId === "FACTION_SIGIL_YEW" )
		return "sigil_Yew";
	if( sectionId === "FACTION_SIGIL_VESPER" )
		return "sigil_Vesper";
	if( sectionId === "FACTION_SIGIL_MINOC" )
		return "sigil_Minoc";
	if( sectionId === "FACTION_SIGIL_COVE" )
		return "sigil_Cove";

	return "";
}

function CommandSetupFoundObject( setupKey )
{
	if( setupKey === "" )
		return null;
	if( typeof CommandSetupFound[setupKey] == "undefined" )
		return null;
	if( !ValidateObject( CommandSetupFound[setupKey] ) )
		return null;

	return CommandSetupFound[setupKey];
}

function command_FACTIONSETUP( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var setupText = String( cmdString ).replace( /^\s+|\s+$/g, "" ).toLowerCase();
	var forceSetup = ( setupText === "force" );
	var createdCount = 0;
	var skippedCount = 0;
	var controller = null;

	if( !forceSetup )
		CommandSetupScanItems();

	if( !forceSetup )
		controller = CommandSetupFoundObject( "controller" );

	if( !ValidateObject( controller ) )
	{
		controller = CreateDFNItem( pSock, pUser, "FACTION_CONTROLLER", 1, "ITEM", true );
		if( ValidateObject( controller ) )
		{
			createdCount++;
			pSock.SysMessage( "Faction controller created in your backpack." );
		}
		else
		{
			pSock.SysMessage( "Unable to create faction controller." );
			return;
		}
	}
	else
	{
		skippedCount++;
		pSock.SysMessage( "Existing faction controller found; skipping controller creation." );
	}

	TriggerEvent( CommandFactionElectionScriptId, "RegisterController", controller );
	TriggerEvent( CommandFactionTownScriptId, "RegisterController", controller );

	for( var itemIndex = 0; itemIndex < CommandFactionSetupItems.length; itemIndex++ )
	{
		var setupItem = CommandFactionSetupItems[itemIndex];
		var setupKey = CommandSetupSectionKey( setupItem[0] );
		if( !forceSetup && ValidateObject( CommandSetupFoundObject( setupKey ) ) )
		{
			skippedCount++;
			continue;
		}

		var newItem = CommandPlaceFactionItem( pSock, pUser, setupItem[0], setupItem[1], setupItem[2] );
		if( ValidateObject( newItem ) )
		{
			if( newItem.GetTag( "sigil" ) == 1 )
				TriggerEvent( CommandFactionSigilScriptId, "SigilSetHome", newItem );
			createdCount++;
		}
		else
			pSock.SysMessage( "Unable to create " + setupItem[0] + "." );
	}

	if( forceSetup )
		pSock.SysMessage( "Faction setup force-placed " + createdCount + " setup item(s) around you." );
	else
		pSock.SysMessage( "Faction setup created " + createdCount + " missing setup item(s), skipped " + skippedCount + " existing item(s)." );

	if( TriggerEvent( CommandFactionTownScriptId, "SyncTownControl" ) )
		pSock.SysMessage( "Faction town ownership synced." );
	else
		pSock.SysMessage( "Faction setup placed, but town sync failed: " + TriggerEvent( CommandFactionTownScriptId, "TownLastError" ) );

	CommandSetupFound = {};
	CommandSetupRanks = {};
}

function command_FACTIONREGIONCHECK( pSock, cmdString )
{
	for( var checkIndex = 0; checkIndex < CommandFactionRegionChecks.length; checkIndex++ )
	{
		var regionCheck = CommandFactionRegionChecks[checkIndex];
		var townRegion = GetTownRegion( regionCheck[1] );
		if( townRegion != null && typeof townRegion.id != "undefined" )
			pSock.SysMessage( regionCheck[0] + " region " + regionCheck[1] + ": loaded as " + townRegion.name );
		else
			pSock.SysMessage( regionCheck[0] + " region " + regionCheck[1] + ": NOT LOADED" );
	}
}

function command_FACTIONTOWNS( pSock, cmdString )
{
	TriggerEvent( CommandFactionTownScriptId, "ShowTownStatus", pSock );
}

function command_FACTIONTOWNSYNC( pSock, cmdString )
{
	if( TriggerEvent( CommandFactionTownScriptId, "SyncTownControl" ) )
		pSock.SysMessage( "Faction town ownership synced to town regions." );
	else
		pSock.SysMessage( "Unable to sync faction town ownership." );
}

function command_FACTIONTOWNSET( pSock, cmdString )
{
	cmdString = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	var parts = cmdString.length > 0 ? cmdString.split( /\s+/ ) : [];
	if( parts.length < 2 )
	{
		pSock.SysMessage( "Usage: 'factiontownset <town> <TB|COM|MIN|SL>" );
		return;
	}

	var factionKey = parts[parts.length - 1].toUpperCase();
	var townParts = [];
	for( var partIndex = 0; partIndex < parts.length - 1; partIndex++ )
		townParts.push( parts[partIndex] );

	var townName = townParts.join( " " );
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "Usage: 'factiontownset <town> <TB|COM|MIN|SL>" );
		return;
	}

	if( TriggerEvent( CommandFactionTownScriptId, "ApplyTownControl", townName, factionKey, 0 ) )
		pSock.SysMessage( "Faction town ownership updated." );
	else
		pSock.SysMessage( "Unable to update that faction town: " + TriggerEvent( CommandFactionTownScriptId, "TownLastError" ) );
}

function CommandShowFactionStatusGump( pSock, pUser, factionKey )
{
	if( !pSock || !ValidateObject( pUser ) || !CommandIsFactionValid( factionKey ) )
		return false;

	var playerFaction = pUser.GetTag( "faction" );
	var isMember = CommandIsFactionValid( playerFaction );
	var displayFactionName = CommandFactionUsageName( factionKey );
	var y = 0;

	var myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 470, CommandIsStaff( pUser ) ? 430 : 390, 9200 );
	myGump.AddHTMLGump( 20, 15, 430, 25, 0, 0, "<CENTER><b>Faction Status</b></CENTER>" );
	myGump.AddHTMLGump( 25, 50, 420, 20, 0, 0, "Faction: " + displayFactionName );

	y = 80;
	if( isMember )
	{
		myGump.AddHTMLGump( 25, y, 420, 20, 0, 0, "Your Faction: " + CommandFactionUsageName( playerFaction ) );
		y += 25;
		myGump.AddHTMLGump( 25, y, 420, 20, 0, 0, "Rank: " + CommandRankName( pUser.GetTag( "faction_rank" ) ) );
		y += 25;
		myGump.AddHTMLGump( 25, y, 420, 20, 0, 0, "Kill Points: " + pUser.GetTag( "faction_kp" ) + ", Silver: " + pUser.GetTag( "faction_silver" ) );
		y += 25;
		var roleName = pUser.GetTag( "faction_role" );
		if( roleName === "" && pUser.GetTag( "faction_commander" ) == "1" )
			roleName = "commander";
		myGump.AddHTMLGump( 25, y, 420, 20, 0, 0, "Role: " + CommandRoleDisplayName( roleName ) );
		y += 30;
	}
	else
	{
		myGump.AddHTMLGump( 25, y, 420, 35, 0, 0, "You are not currently enlisted in a faction." );
		y += 45;
	}

	myGump.AddHTMLGump( 25, y, 420, 20, 0, 0, TriggerEvent( CommandFactionSigilScriptId, "FactionScoreText", factionKey ) );
	y += 25;
	myGump.AddHTMLGump( 25, y, 420, 20, 0, 0, TriggerEvent( CommandFactionCombatScriptId, "FactionKillStatsText", factionKey ) );
	y += 25;
	myGump.AddHTMLGump( 25, y, 420, 35, 0, 0, TriggerEvent( CommandFactionSigilScriptId, "FactionNoticeText", factionKey ) );
	y += 40;
	myGump.AddHTMLGump( 25, y, 420, 45, 0, 0, "Controlled Towns: " + TriggerEvent( CommandFactionTownScriptId, "TownControlledByFactionList", factionKey ) );
	y += 50;

	if( CommandIsStaff( pUser ) )
	{
		myGump.AddHTMLGump( 25, y, 420, 20, 0, 0, TriggerEvent( CommandFactionTownScriptId, "TownTreasuryByFactionText", factionKey ) );
		y += 25;
	}

	myGump.AddButton( 25, y + 10, 0xFA5, 1, 0, 0 );
	myGump.AddHTMLGump( 65, y + 10, 100, 20, 0, 0, "Close" );
	myGump.Send( pSock );
	myGump.Free();
	return true;
}

function CommandHealthScanItems()
{
	CommandHealthControllerCount = 0;
	CommandHealthFactionStoneCount = 0;
	CommandHealthJoinStoneCount = 0;
	CommandHealthSigilCount = 0;
	CommandHealthSigilHomeCount = 0;
	CommandHealthScanActive = true;
	IterateOver( "ITEM" );
	CommandHealthScanActive = false;
}

function CommandDedupeItemKey( iItem )
{
	if( !ValidateObject( iItem ) || !iItem.isItem )
		return "";

	if( iItem.GetTag( "faction_controller" ) == 1 )
		return "controller";

	if( iItem.GetTag( "faction_stone" ) == 1 )
	{
		var factionStoneFaction = iItem.GetTag( "stone_faction" );
		if( CommandIsFactionValid( factionStoneFaction ) )
			return "faction_stone_" + factionStoneFaction;
		return "";
	}

	if( iItem.GetTag( "join_stone" ) == 1 )
	{
		var joinStoneFaction = iItem.GetTag( "stone_faction" );
		if( CommandIsFactionValid( joinStoneFaction ) )
			return "join_stone_" + joinStoneFaction;
		return "";
	}

	if( iItem.GetTag( "sigil" ) == 1 )
	{
		var sigilTown = iItem.GetTag( "sigil_town" );
		if( sigilTown !== "" && sigilTown != 0 )
			return "sigil_" + sigilTown;
		return "";
	}

	return "";
}

function CommandDedupeItemRank( iItem )
{
	if( !ValidateObject( iItem ) || !iItem.isItem )
		return 0;

	var rankScore = 0;

	if( iItem.GetTag( "faction_controller" ) == 1 )
	{
		if( iItem.GetTag( "faction_town_tax_enabled" ) == 1 )
			rankScore += 100;
		if( parseInt( iItem.GetTag( "faction_town_last_tax_income" ), 10 ) > 0 )
			rankScore += 50;
		if( parseInt( iItem.GetTag( "faction_town_tax_next" ), 10 ) > 0 )
			rankScore += 25;
		if( parseInt( iItem.GetTag( "election_cycle" ), 10 ) > 0 )
			rankScore += 25;
		return rankScore;
	}

	if( iItem.GetTag( "sigil" ) == 1 )
	{
		if( iItem.GetTag( "sigil_home_set" ) == 1 )
			rankScore += 25;
		if( parseInt( iItem.GetTag( "sigil_corrupt_start" ), 10 ) > 0 )
			rankScore += 50;
		if( iItem.GetTag( "sigil_corrupted" ) == 1 )
			rankScore += 50;
		if( CommandIsFactionValid( iItem.GetTag( "sigil_owner_faction" ) ) )
			rankScore += 10;
		if( CommandIsFactionValid( iItem.GetTag( "sigil_carrier_faction" ) ) )
			rankScore += 75;
		return rankScore;
	}

	return rankScore;
}

function CommandDedupeFactionSetup()
{
	CommandDedupeKept = {};
	CommandDedupeRanks = {};
	CommandDedupeRemovedCount = 0;
	CommandDedupeActive = true;
	IterateOver( "ITEM" );
	CommandDedupeActive = false;
	CommandDedupeKept = {};
	CommandDedupeRanks = {};
	return CommandDedupeRemovedCount;
}

function CommandIsFactionSetupItem( iItem )
{
	if( !ValidateObject( iItem ) || !iItem.isItem )
		return false;

	if( iItem.GetTag( "faction_controller" ) == 1 )
		return true;
	if( iItem.GetTag( "faction_stone" ) == 1 )
		return true;
	if( iItem.GetTag( "join_stone" ) == 1 )
		return true;
	if( iItem.GetTag( "sigil" ) == 1 )
		return true;

	return false;
}

function CommandIsFactionItem( iItem )
{
	if( !ValidateObject( iItem ) || !iItem.isItem )
		return false;

	if( CommandIsFactionSetupItem( iItem ) )
		return true;
	if( iItem.GetTag( "faction_item" ) == 1 )
		return true;
	if( iItem.GetTag( "faction_horse_deed" ) == 1 )
		return true;
	if( iItem.GetTag( "trap_deed" ) == 1 )
		return true;
	if( iItem.GetTag( "trap_placed" ) == 1 )
		return true;
	if( CommandIsFactionValid( iItem.GetTag( "item_faction" ) ) )
		return true;
	if( CommandIsFactionValid( iItem.GetTag( "trap_faction" ) ) )
		return true;

	return false;
}

function CommandRemoveFactionItem( iItem )
{
	if( !CommandIsFactionItem( iItem ) )
		return false;

	iItem.Delete();
	CommandRemoveFactionsItems++;
	return true;
}

function CommandClearFactionPlayerTags( pChar )
{
	if( !ValidateObject( pChar ) || !pChar.isChar || pChar.npc )
		return false;

	var hadFactionData = false;
	var factionTags = [
		"faction",
		"faction_join_time",
		"faction_kp",
		"faction_silver",
		"faction_rank",
		"faction_leave_time",
		"faction_commander",
		"faction_role",
		"faction_role_faction",
		"faction_role_set_at",
		"faction_kp_decay_time",
		"elec_voted_TB",
		"elec_voted_COM",
		"elec_voted_MIN",
		"elec_voted_SL"
	];

	for( var tagIndex = 0; tagIndex < factionTags.length; tagIndex++ )
	{
		if( pChar.GetTag( factionTags[tagIndex] ) !== "" && pChar.GetTag( factionTags[tagIndex] ) != 0 )
			hadFactionData = true;
		pChar.SetTag( factionTags[tagIndex], null );
	}

	pChar.SetTempTag( "placing_trap_type", null );
	pChar.SetTempTag( "placing_trap_faction", null );
	pChar.SetTempTag( "placing_trap_deed", null );
	pChar.SetTempTag( "trap_scan_type", null );
	pChar.SetTempTag( "trap_scan_faction", null );

	if( pChar.HasScriptTrigger( CommandFactionCombatScriptId ) )
		pChar.RemoveScriptTrigger( CommandFactionCombatScriptId );

	if( hadFactionData )
		CommandRemoveFactionsPlayers++;

	return hadFactionData;
}

function CommandIsFactionNpc( pChar )
{
	if( !ValidateObject( pChar ) || !pChar.isChar || !pChar.npc )
		return false;

	if( pChar.GetTag( "faction_spawned" ) == 1 )
		return true;
	if( pChar.GetTag( "faction_horse" ) == 1 )
		return true;
	if( pChar.GetTag( "faction_mount" ) == 1 )
		return true;
	if( CommandIsFactionValid( pChar.GetTag( "faction" ) ) && pChar.GetTag( "faction_town" ) !== "" )
		return true;
	if( CommandIsFactionValid( pChar.GetTag( "item_faction" ) ) )
		return true;
	if( CommandIsFactionValid( pChar.GetTag( "mount_faction" ) ) )
		return true;

	return false;
}

function CommandRemoveFactionChar( pChar )
{
	if( !ValidateObject( pChar ) || !pChar.isChar )
		return false;

	if( pChar.npc )
	{
		if( !CommandIsFactionNpc( pChar ) )
			return false;

		var ownerChar = pChar.owner;
		if( ValidateObject( ownerChar ) && ownerChar.isChar && !ownerChar.npc )
		{
			ownerChar.RemoveFollower( pChar );
			if( ownerChar.controlSlotsUsed >= pChar.controlSlots )
				ownerChar.controlSlotsUsed = ownerChar.controlSlotsUsed - pChar.controlSlots;
			else
				ownerChar.controlSlotsUsed = 0;
		}

		pChar.Delete();
		CommandRemoveFactionsNpcs++;
		return true;
	}

	CommandClearFactionPlayerTags( pChar );
	return false;
}

function CommandResetFactionTownRegions()
{
	var resetCount = 0;
	for( var regionIndex = 0; regionIndex < CommandFactionRegionChecks.length; regionIndex++ )
	{
		var regionInfo = CommandFactionRegionChecks[regionIndex];
		var townRegion = GetTownRegion( regionInfo[1] );
		if( townRegion != null && typeof townRegion.id != "undefined" )
		{
			townRegion.owner = "The Town";
			townRegion.isGuarded = true;
			resetCount++;
		}
	}

	CommandRemoveFactionsRegions = resetCount;
	return resetCount;
}

function CommandRemoveFactionsFromShard()
{
	CommandRemoveFactionsItems = 0;
	CommandRemoveFactionsNpcs = 0;
	CommandRemoveFactionsPlayers = 0;
	CommandRemoveFactionsRegions = 0;

	CommandResetFactionTownRegions();

	CommandRemoveFactionsMode = "chars";
	IterateOver( "CHARACTER" );
	CommandRemoveFactionsMode = "items";
	IterateOver( "ITEM" );
	CommandRemoveFactionsMode = "";

	return CommandRemoveFactionsItems + CommandRemoveFactionsNpcs + CommandRemoveFactionsPlayers;
}

function CommandHealthLine( ok, label, value )
{
	return ( ok ? "[OK] " : "[WARN] " ) + label + ": " + value;
}

function CommandHealthRegionSummary()
{
	var loadedCount = 0;
	var missingText = "";

	for( var regionIndex = 0; regionIndex < CommandFactionRegionChecks.length; regionIndex++ )
	{
		var regionInfo = CommandFactionRegionChecks[regionIndex];
		var townRegion = GetTownRegion( regionInfo[1] );
		if( townRegion != null && typeof townRegion.id != "undefined" )
		{
			loadedCount++;
		}
		else
		{
			if( missingText !== "" )
				missingText += ", ";
			missingText += regionInfo[0];
		}
	}

	return {
		loaded: loadedCount,
		total: CommandFactionRegionChecks.length,
		missing: missingText
	};
}

function CommandHealthStrongholdSummary()
{
	var factionKeys = [ "TB", "COM", "MIN", "SL" ];
	var configuredCount = 0;
	var missingText = "";

	for( var factionIndex = 0; factionIndex < factionKeys.length; factionIndex++ )
	{
		var factionKey = factionKeys[factionIndex];
		if( TriggerEvent( CommandFactionStrongholdScriptId, "StrongholdIsConfigured", factionKey ) )
		{
			configuredCount++;
		}
		else
		{
			if( missingText !== "" )
				missingText += ", ";
			missingText += factionKey;
		}
	}

	return {
		configured: configuredCount,
		total: factionKeys.length,
		missing: missingText
	};
}

function CommandShowFactionHealthGump( pSock, pUser )
{
	if( !pSock || !ValidateObject( pUser ) )
		return false;

	CommandHealthScanItems();
	var regionSummary = CommandHealthRegionSummary();
	var strongholdSummary = CommandHealthStrongholdSummary();
	var y = 50;

	var myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 560, 500, 9200 );
	myGump.AddHTMLGump( 20, 15, 520, 25, 0, 0, "<CENTER><b>Faction Health</b></CENTER>" );

	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, CommandHealthLine( CommandHealthControllerCount === 1, "Faction controllers", CommandHealthControllerCount + " found, expected 1" ) );
	y += 25;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, CommandHealthLine( CommandHealthFactionStoneCount === 4, "Faction stones", CommandHealthFactionStoneCount + " found, expected 4" ) );
	y += 25;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, CommandHealthLine( CommandHealthJoinStoneCount === 4, "Join stones", CommandHealthJoinStoneCount + " found, expected 4" ) );
	y += 25;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, CommandHealthLine( CommandHealthSigilCount === 8, "Sigils", CommandHealthSigilCount + " found, expected 8" ) );
	y += 25;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, CommandHealthLine( CommandHealthSigilHomeCount === 8, "Sigil homes", CommandHealthSigilHomeCount + " set, expected 8" ) );
	y += 30;

	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, CommandHealthLine( regionSummary.loaded === regionSummary.total, "Town regions", regionSummary.loaded + "/" + regionSummary.total + " loaded" ) );
	y += 25;
	if( regionSummary.missing !== "" )
	{
		myGump.AddHTMLGump( 45, y, 490, 35, 0, 0, "Missing regions: " + regionSummary.missing );
		y += 40;
	}

	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, CommandHealthLine( strongholdSummary.configured === strongholdSummary.total, "Strongholds", strongholdSummary.configured + "/" + strongholdSummary.total + " configured" ) );
	y += 25;
	if( strongholdSummary.missing !== "" )
	{
		myGump.AddHTMLGump( 45, y, 490, 35, 0, 0, "Missing strongholds: " + strongholdSummary.missing );
		y += 40;
	}

	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, TriggerEvent( CommandFactionTownScriptId, "TownTaxStatusText" ) );
	y += 30;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, TriggerEvent( CommandFactionSigilScriptId, "FactionScoreText", "TB" ) );
	y += 22;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, TriggerEvent( CommandFactionSigilScriptId, "FactionScoreText", "COM" ) );
	y += 22;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, TriggerEvent( CommandFactionSigilScriptId, "FactionScoreText", "MIN" ) );
	y += 22;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, TriggerEvent( CommandFactionSigilScriptId, "FactionScoreText", "SL" ) );
	y += 30;

	myGump.AddHTMLGump( 25, y, 510, 40, 0, 0, "Run 'factiondedupe to remove duplicate setup pieces. Run 'factionsetup only for missing stones/sigils, then 'factionsigilhome all if homes are missing." );
	y += 50;
	myGump.AddButton( 25, y, 0xFA5, 1, 0, 0 );
	myGump.AddHTMLGump( 65, y, 100, 20, 0, 0, "Close" );
	myGump.Send( pSock );
	myGump.Free();
	return true;
}

function CommandAddAdminButton( myGump, x, y, buttonId, labelText )
{
	myGump.AddButton( x, y, 0xFA5, 0xFA7, 1, 0, buttonId );
	myGump.AddHTMLGump( x + 40, y, 180, 20, 0, 0, labelText );
}

function CommandShowFactionAdminGump( pSock, pUser )
{
	if( !pSock || !ValidateObject( pUser ) )
		return false;

	CommandHealthScanItems();
	var regionSummary = CommandHealthRegionSummary();
	var strongholdSummary = CommandHealthStrongholdSummary();

	var myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 520, 430, 9200 );
	myGump.AddHTMLGump( 20, 15, 480, 25, 0, 0, "<CENTER><b>Faction Admin</b></CENTER>" );
	myGump.AddHTMLGump( 25, 50, 470, 20, 0, 0, "Setup: controllers " + CommandHealthControllerCount + "/1, stones " + CommandHealthFactionStoneCount + "/4, join stones " + CommandHealthJoinStoneCount + "/4, sigils " + CommandHealthSigilCount + "/8" );
	myGump.AddHTMLGump( 25, 75, 470, 20, 0, 0, "Regions: " + regionSummary.loaded + "/" + regionSummary.total + ", Strongholds: " + strongholdSummary.configured + "/" + strongholdSummary.total + ", Sigil homes: " + CommandHealthSigilHomeCount + "/8" );
	myGump.AddHTMLGump( 25, 100, 470, 20, 0, 0, TriggerEvent( CommandFactionTownScriptId, "TownTaxStatusText" ) );

	CommandAddAdminButton( myGump, 30, 140, CommandFactionAdminHealthButton, "Health Gump" );
	CommandAddAdminButton( myGump, 30, 170, CommandFactionAdminSetupButton, "Safe Setup" );
	CommandAddAdminButton( myGump, 30, 200, CommandFactionAdminDedupeButton, "Dedupe Setup" );
	CommandAddAdminButton( myGump, 30, 230, CommandFactionAdminSyncTownsButton, "Sync Towns" );
	CommandAddAdminButton( myGump, 30, 260, CommandFactionAdminTownStatusButton, "Town Status" );
	CommandAddAdminButton( myGump, 30, 290, CommandFactionAdminTreasuryButton, "Treasury" );

	CommandAddAdminButton( myGump, 275, 140, CommandFactionAdminTaxStartButton, "Start Tax Timer" );
	CommandAddAdminButton( myGump, 275, 170, CommandFactionAdminTaxStopButton, "Stop Tax Timer" );
	CommandAddAdminButton( myGump, 275, 200, CommandFactionAdminTaxStatusButton, "Tax Status" );
	CommandAddAdminButton( myGump, 275, 230, CommandFactionAdminSigilsButton, "Sigils" );
	CommandAddAdminButton( myGump, 275, 260, CommandFactionAdminStrongholdsButton, "Strongholds" );
	CommandAddAdminButton( myGump, 275, 320, CommandFactionAdminRemoveConfirmButton, "Remove Factions" );

	myGump.AddButton( 30, 380, 0xFA5, 0xFA7, 1, 0, 0 );
	myGump.AddHTMLGump( 70, 380, 100, 20, 0, 0, "Close" );
	myGump.Send( pSock );
	myGump.Free();
	return true;
}

function CommandShowRemoveFactionsConfirmGump( pSock, pUser )
{
	if( !pSock || !ValidateObject( pUser ) )
		return false;

	var myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 430, 230, 9200 );
	myGump.AddHTMLGump( 20, 15, 390, 25, 0, 0, "<CENTER><b>Remove Factions?</b></CENTER>" );
	myGump.AddHTMLGump( 25, 55, 380, 70, 0, 0, "This deletes faction setup items, faction items, faction NPCs, faction mounts, traps, and clears player faction tags." );
	myGump.AddHTMLGump( 25, 130, 380, 25, 0, 0, "Use this only when resetting the faction test install." );
	myGump.AddButton( 45, 175, 0xFA5, 0xFA7, 1, 0, CommandFactionAdminBackButton );
	myGump.AddHTMLGump( 85, 175, 100, 20, 0, 0, "Cancel" );
	myGump.AddButton( 240, 175, 0xFA5, 0xFA7, 1, 0, CommandFactionAdminRemoveNowButton );
	myGump.AddHTMLGump( 280, 175, 120, 20, 0, 0, "Remove Now" );
	myGump.Send( pSock );
	myGump.Free();
	return true;
}

function command_FACTIONSTATUS( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var factionKey = pUser.GetTag( "faction" );
	var requestedFaction = String( cmdString ).replace( /^\s+|\s+$/g, "" ).toUpperCase();
	if( CommandIsStaff( pUser ) && CommandIsFactionValid( requestedFaction ) )
		factionKey = requestedFaction;

	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "You are not currently in a faction." );
		return;
	}

	CommandShowFactionStatusGump( pSock, pUser, factionKey );
}

function command_FACTIONADMIN( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	CommandShowFactionAdminGump( pSock, pUser );
}

function command_FACTIONHEALTH( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	CommandShowFactionHealthGump( pSock, pUser );
}

function command_FACTIONDEDUPE( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var removedCount = CommandDedupeFactionSetup();
	pSock.SysMessage( "Removed " + removedCount + " duplicate faction setup item(s)." );
	CommandShowFactionHealthGump( pSock, pUser );
}

function command_REMOVEFACTIONS( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	CommandRemoveFactionsFromShard();
	pSock.SysMessage( "Faction system removed from shard." );
	pSock.SysMessage( "Deleted " + CommandRemoveFactionsItems + " faction item(s), " + CommandRemoveFactionsNpcs + " faction NPC/mount(s), cleared " + CommandRemoveFactionsPlayers + " player(s), reset " + CommandRemoveFactionsRegions + " town region(s)." );
}

function onGumpPress( pSock, buttonID, gumpData )
{
	if( buttonID == 0 )
		return;

	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	if( buttonID == CommandFactionAdminHealthButton )
	{
		CommandShowFactionHealthGump( pSock, pUser );
		return;
	}

	if( buttonID == CommandFactionAdminSetupButton )
	{
		command_FACTIONSETUP( pSock, "" );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == CommandFactionAdminDedupeButton )
	{
		var removedCount = CommandDedupeFactionSetup();
		pSock.SysMessage( "Removed " + removedCount + " duplicate faction setup item(s)." );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == CommandFactionAdminSyncTownsButton )
	{
		command_FACTIONTOWNSYNC( pSock, "" );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == CommandFactionAdminTownStatusButton )
	{
		TriggerEvent( CommandFactionTownScriptId, "ShowTownStatus", pSock );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == CommandFactionAdminTreasuryButton )
	{
		TriggerEvent( CommandFactionTownScriptId, "ShowTownTreasury", pSock, "" );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == CommandFactionAdminTaxStartButton )
	{
		if( TriggerEvent( CommandFactionTownScriptId, "StartTownTaxTimer", 60 ) )
			pSock.SysMessage( "Faction town tax timer started." );
		else
			pSock.SysMessage( "Unable to start faction town tax timer." );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == CommandFactionAdminTaxStopButton )
	{
		command_FACTIONTAXSTOP( pSock, "" );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == CommandFactionAdminTaxStatusButton )
	{
		TriggerEvent( CommandFactionTownScriptId, "ShowTownTaxStatus", pSock );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == CommandFactionAdminSigilsButton )
	{
		TriggerEvent( CommandFactionSigilScriptId, "ShowSigilStatus", pSock, "" );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == CommandFactionAdminStrongholdsButton )
	{
		TriggerEvent( CommandFactionStrongholdScriptId, "ShowStrongholdStatus", pSock );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == CommandFactionAdminRemoveConfirmButton )
	{
		CommandShowRemoveFactionsConfirmGump( pSock, pUser );
		return;
	}

	if( buttonID == CommandFactionAdminRemoveNowButton )
	{
		command_REMOVEFACTIONS( pSock, "" );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == CommandFactionAdminBackButton )
	{
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}
}

function command_FACTIONSTRONGHOLD( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	cmdString = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	var parts = cmdString.length > 0 ? cmdString.split( /\s+/ ) : [];
	if( parts.length < 1 )
	{
		pSock.SysMessage( "Usage: 'factionstronghold <TB|COM|MIN|SL> [range]" );
		return;
	}

	var factionKey = parts[0].toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "Usage: 'factionstronghold <TB|COM|MIN|SL> [range]" );
		return;
	}

	var range = 12;
	if( parts.length > 1 )
	{
		range = parseInt( parts[1], 10 );
		if( isNaN( range ) )
		{
			pSock.SysMessage( "Stronghold range must be a number." );
			return;
		}
	}

	if( TriggerEvent( CommandFactionStrongholdScriptId, "StrongholdSet", factionKey, pUser, range ) )
		pSock.SysMessage( CommandFactionUsageName( factionKey ) + " stronghold set at your current location." );
	else
		pSock.SysMessage( "Unable to set faction stronghold: " + TriggerEvent( CommandFactionStrongholdScriptId, "StrongholdLastError" ) );
}

function command_FACTIONSTRONGHOLDS( pSock, cmdString )
{
	if( !TriggerEvent( CommandFactionStrongholdScriptId, "ShowStrongholdStatus", pSock ) )
		pSock.SysMessage( "Unable to show faction strongholds." );
}

function CommandSigilTownArg( cmdString )
{
	var townName = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	if( townName.toLowerCase() === "all" )
		return "";

	return townName;
}

function command_FACTIONSIGILHOME( pSock, cmdString )
{
	var townName = CommandSigilTownArg( cmdString );
	var count = TriggerEvent( CommandFactionSigilScriptId, "SigilRegisterHome", townName );
	if( count > 0 )
		pSock.SysMessage( "Registered home location for " + count + " faction sigil(s)." );
	else
		pSock.SysMessage( "No faction sigils matched. Usage: 'factionsigilhome <town|all>" );
}

function command_FACTIONSIGILRETURN( pSock, cmdString )
{
	var townName = CommandSigilTownArg( cmdString );
	var count = TriggerEvent( CommandFactionSigilScriptId, "SigilReturn", townName );
	if( count > 0 )
		pSock.SysMessage( "Returned " + count + " faction sigil(s) home." );
	else
		pSock.SysMessage( "No faction sigils with registered homes matched. Usage: 'factionsigilreturn <town|all>" );
}

function command_FACTIONSIGILS( pSock, cmdString )
{
	var townName = CommandSigilTownArg( cmdString );
	var count = TriggerEvent( CommandFactionSigilScriptId, "ShowSigilStatus", pSock, townName );
	if( count == 0 )
		pSock.SysMessage( "No faction sigils matched. Usage: 'factionsigils [town|all]" );
}

function command_FACTIONSIGILRETURNTIME( pSock, cmdString )
{
	var cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	if( cmdText === "" )
	{
		TriggerEvent( CommandFactionSigilScriptId, "ShowSigilReturnTime", pSock );
		return;
	}

	var minutes = parseInt( cmdText, 10 );
	if( isNaN( minutes ) || minutes < 1 )
	{
		pSock.SysMessage( "Usage: 'factionsigilreturntime [minutes]" );
		return;
	}

	var count = TriggerEvent( CommandFactionSigilScriptId, "SigilSetReturnTime", minutes );
	if( count > 0 )
		pSock.SysMessage( "Faction sigil return time set to " + minutes + " minute(s) for " + count + " sigil(s)." );
	else
		pSock.SysMessage( "No faction sigils found." );
}

function command_FACTIONSCORE( pSock, cmdString )
{
	if( !TriggerEvent( CommandFactionSigilScriptId, "ShowFactionScore", pSock ) )
		pSock.SysMessage( "Unable to show faction score." );
}

function command_FACTIONSCORERESET( pSock, cmdString )
{
	if( TriggerEvent( CommandFactionSigilScriptId, "ResetFactionScore" ) )
		pSock.SysMessage( "Faction score has been reset." );
	else
		pSock.SysMessage( "Unable to reset faction score." );
}

function command_FACTIONKILLS( pSock, cmdString )
{
	if( !TriggerEvent( CommandFactionCombatScriptId, "ShowFactionKillStats", pSock ) )
		pSock.SysMessage( "Unable to show faction kill stats." );
}

function command_FACTIONKILLSRESET( pSock, cmdString )
{
	if( TriggerEvent( CommandFactionCombatScriptId, "ResetFactionKillStats" ) )
		pSock.SysMessage( "Faction kill stats have been reset." );
	else
		pSock.SysMessage( "Unable to reset faction kill stats." );
}

function CommandFactionFromCommanderArgs( pUser, parts )
{
	if( parts.length > 0 && CommandIsFactionValid( String( parts[0] ).toUpperCase() ) && CommandIsStaff( pUser ) )
		return String( parts[0] ).toUpperCase();

	return pUser.GetTag( "faction" );
}

function CommandRequireCommander( pSock, pUser, factionKey )
{
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "You are not in a faction." );
		return false;
	}

	if( CommandIsStaff( pUser ) )
		return true;
	if( CommandHasFactionRole( pUser, "commander", factionKey ) )
		return true;

	pSock.SysMessage( "Only your faction Commander may use that command." );
	return false;
}

function CommandBroadcastToFaction( factionKey, messageText )
{
	CommandBroadcastFaction = factionKey;
	CommandBroadcastMessage = messageText;
	CommandBroadcastCount = 0;
	IterateOver( "CHARACTER" );
	var sentCount = CommandBroadcastCount;
	CommandBroadcastFaction = "";
	CommandBroadcastMessage = "";
	CommandBroadcastCount = 0;
	return sentCount;
}

function command_FACTIONNOTICE( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	var parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	var factionKey = CommandFactionFromCommanderArgs( pUser, parts );
	if( !CommandRequireCommander( pSock, pUser, factionKey ) )
		return;

	if( CommandIsStaff( pUser ) && parts.length > 0 && CommandIsFactionValid( String( parts[0] ).toUpperCase() ) )
		parts.shift();

	var messageText = parts.join( " " ).replace( /^\s+|\s+$/g, "" );
	if( messageText === "" )
	{
		pSock.SysMessage( "Usage: 'factionnotice [TB|COM|MIN|SL] <message>" );
		return;
	}

	if( TriggerEvent( CommandFactionSigilScriptId, "SetFactionNotice", factionKey, messageText, pUser.name ) )
	{
		pSock.SysMessage( "Faction notice updated." );
		CommandBroadcastToFaction( factionKey, CommandFactionUsageName( factionKey ) + " notice: " + messageText );
	}
	else
	{
		pSock.SysMessage( "Unable to update faction notice." );
	}
}

function command_FACTIONNOTICECLEAR( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var parts = String( cmdString ).replace( /^\s+|\s+$/g, "" ).split( /\s+/ );
	if( parts.length == 1 && parts[0] === "" )
		parts = [];

	var factionKey = CommandFactionFromCommanderArgs( pUser, parts );
	if( !CommandRequireCommander( pSock, pUser, factionKey ) )
		return;

	if( TriggerEvent( CommandFactionSigilScriptId, "ClearFactionNotice", factionKey ) )
	{
		pSock.SysMessage( "Faction notice cleared." );
		CommandBroadcastToFaction( factionKey, CommandFactionUsageName( factionKey ) + " notice cleared." );
	}
	else
	{
		pSock.SysMessage( "Unable to clear faction notice." );
	}
}

function command_FACTIONALERT( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	var parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	var factionKey = CommandFactionFromCommanderArgs( pUser, parts );
	if( !CommandRequireCommander( pSock, pUser, factionKey ) )
		return;

	if( CommandIsStaff( pUser ) && parts.length > 0 && CommandIsFactionValid( String( parts[0] ).toUpperCase() ) )
		parts.shift();

	var townName = parts.join( " " ).replace( /^\s+|\s+$/g, "" );
	if( townName === "" )
		townName = CommandCurrentFactionTown( pUser );
	if( townName === "" )
	{
		pSock.SysMessage( "Usage: 'factionalert [TB|COM|MIN|SL] <town>" );
		return;
	}

	var townOwner = TriggerEvent( CommandFactionTownScriptId, "TownGetOwner", townName );
	if( townOwner === "" )
	{
		pSock.SysMessage( "That is not a known faction town." );
		return;
	}

	if( townOwner !== "" && townOwner !== factionKey && !CommandIsStaff( pUser ) )
	{
		pSock.SysMessage( "You may only alert towns controlled by your faction." );
		return;
	}

	if( !CommandIsStaff( pUser ) )
	{
		if( TriggerEvent( CommandFactionSigilScriptId, "FactionScoreValue", factionKey ) < 1 )
		{
			pSock.SysMessage( "Your faction needs at least 1 score to issue a town alert." );
			return;
		}

		if( !TriggerEvent( CommandFactionSigilScriptId, "SpendFactionScore", factionKey, 1 ) )
		{
			pSock.SysMessage( "Unable to spend faction score for that alert." );
			return;
		}
	}

	var sentCount = CommandBroadcastToFaction( factionKey, "Faction alert: " + townName + " needs " + CommandFactionUsageName( factionKey ) + " forces." );
	pSock.SysMessage( "Faction alert sent to " + sentCount + " online faction member(s)." );
}

function CommandCurrentFactionTown( pUser )
{
	return TriggerEvent( CommandFactionTownScriptId, "TownNameForObject", pUser );
}

function CommandCanPlaceFactionNpc( pSock, pUser, factionKey )
{
	var townOwner = TriggerEvent( CommandFactionTownScriptId, "TownOwnerForObject", pUser );
	if( townOwner === "" )
	{
		pSock.SysMessage( "You must be in a faction-controlled town to place faction NPCs." );
		return false;
	}

	if( townOwner !== "" && townOwner !== factionKey )
	{
		pSock.SysMessage( "Only " + CommandFactionUsageName( townOwner ) + " may place faction NPCs in this town." );
		return false;
	}

	return true;
}

function CommandSpendTownTreasury( pSock, pUser, townName, cost, spendText )
{
	if( CommandIsStaff( pUser ) )
		return true;

	var treasury = TriggerEvent( CommandFactionTownScriptId, "TownGetTreasury", townName );
	if( treasury < cost )
	{
		pSock.SysMessage( "The " + townName + " treasury needs " + cost + " silver for " + spendText + ". Current treasury: " + treasury + "." );
		return false;
	}

	if( !TriggerEvent( CommandFactionTownScriptId, "TownSpendTreasury", townName, cost ) )
	{
		pSock.SysMessage( "Unable to spend from the town treasury." );
		return false;
	}

	pSock.SysMessage( "Spent " + cost + " silver from the " + townName + " treasury." );
	return true;
}

function CommandCanUseFactionNpcCommand( pSock, pUser, factionKey, requiredRole )
{
	if( CommandIsStaff( pUser ) )
		return true;

	if( CommandHasFactionRole( pUser, requiredRole, factionKey ) )
		return true;

	pSock.SysMessage( "Only a " + CommandRoleDisplayName( requiredRole ) + " of " + CommandFactionUsageName( factionKey ) + " may use that command." );
	return false;
}

function CommandSpawnFactionNpc( pSock, pUser, sectionId, factionKey, npcType, vendorType )
{
	var newNpc = SpawnNPC( sectionId, pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
	if( !ValidateObject( newNpc ) )
		return null;

	var townName = CommandCurrentFactionTown( pUser );
	if( !TriggerEvent( CommandFactionTownScriptId, "TownTagFactionNpc", newNpc, factionKey, townName, npcType, vendorType ) )
		newNpc.SetTag( "faction_spawned", 1 );

	return newNpc;
}

function CommandTownNameFromArgs( pSock, pUser, cmdString, allowAll, requireTown )
{
	var townName = "";
	if( cmdString != null )
		townName = String( cmdString ).replace( /^\s+|\s+$/g, "" );

	if( allowAll && townName.toLowerCase() === "all" )
		return "";

	if( townName === "" )
		townName = CommandCurrentFactionTown( pUser );

	if( townName === "" && requireTown )
	{
		pSock.SysMessage( "Stand in a faction town, specify a town name, or use all." );
		return null;
	}

	return townName;
}

function command_FACTIONTOWNNPCS( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var townName = CommandTownNameFromArgs( pSock, pUser, cmdString, true, false );
	TriggerEvent( CommandFactionTownScriptId, "ShowTownNpcStatus", pSock, townName );
}

function command_FACTIONTOWNCLEAR( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var townName = CommandTownNameFromArgs( pSock, pUser, cmdString, true, true );
	if( townName == null )
		return;

	var removedCount = TriggerEvent( CommandFactionTownScriptId, "TownClearFactionNpcs", townName, "" );
	pSock.SysMessage( "Removed " + removedCount + " managed faction NPC(s)." );
}

function command_FACTIONTREASURY( pSock, cmdString )
{
	var townName = CommandSigilTownArg( cmdString );
	if( !TriggerEvent( CommandFactionTownScriptId, "ShowTownTreasury", pSock, townName ) )
		pSock.SysMessage( "Unable to show faction town treasury." );
}

function command_FACTIONTREASURYGRANT( pSock, cmdString )
{
	var cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	var parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	if( parts.length < 2 )
	{
		pSock.SysMessage( "Usage: 'factiontreasurygrant <town> <amount>" );
		return;
	}

	var amount = parseInt( parts[parts.length - 1], 10 );
	if( isNaN( amount ) )
	{
		pSock.SysMessage( "Usage: 'factiontreasurygrant <town> <amount>" );
		return;
	}

	parts.pop();
	var townName = parts.join( " " );
	if( TriggerEvent( CommandFactionTownScriptId, "TownAddTreasury", townName, amount ) )
		pSock.SysMessage( "Faction town treasury updated." );
	else
		pSock.SysMessage( "Unable to update that faction town treasury." );
}

function command_FACTIONTREASURYSET( pSock, cmdString )
{
	var cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	var parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	if( parts.length < 2 )
	{
		pSock.SysMessage( "Usage: 'factiontreasuryset <town> <amount>" );
		return;
	}

	var amount = parseInt( parts[parts.length - 1], 10 );
	if( isNaN( amount ) )
	{
		pSock.SysMessage( "Usage: 'factiontreasuryset <town> <amount>" );
		return;
	}

	parts.pop();
	var townName = parts.join( " " );
	if( TriggerEvent( CommandFactionTownScriptId, "TownSetTreasury", townName, amount ) )
		pSock.SysMessage( "Faction town treasury set." );
	else
		pSock.SysMessage( "Unable to set that faction town treasury." );
}

function command_FACTIONTAXCYCLE( pSock, cmdString )
{
	var totalIncome = TriggerEvent( CommandFactionTownScriptId, "RunTownTaxCycle" );
	if( totalIncome >= 0 )
		pSock.SysMessage( "Faction town tax cycle complete. Total income: " + totalIncome + " silver." );
	else
		pSock.SysMessage( "Unable to run faction town tax cycle." );
}

function command_FACTIONTAXRATE( pSock, cmdString )
{
	var cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	var parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	if( parts.length < 2 )
	{
		pSock.SysMessage( "Usage: 'factiontaxrate <town> <amount>" );
		return;
	}

	var amount = parseInt( parts[parts.length - 1], 10 );
	if( isNaN( amount ) )
	{
		pSock.SysMessage( "Usage: 'factiontaxrate <town> <amount>" );
		return;
	}

	parts.pop();
	var townName = parts.join( " " );
	if( TriggerEvent( CommandFactionTownScriptId, "TownSetTaxRate", townName, amount ) )
		pSock.SysMessage( "Faction town tax rate set." );
	else
		pSock.SysMessage( "Unable to set that faction town tax rate." );
}

function command_FACTIONTAXSTART( pSock, cmdString )
{
	var cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	var minutes = 0;
	if( cmdText !== "" )
	{
		minutes = parseInt( cmdText, 10 );
		if( isNaN( minutes ) || minutes < 1 )
		{
			pSock.SysMessage( "Usage: 'factiontaxstart [minutes]" );
			return;
		}
	}

	if( TriggerEvent( CommandFactionTownScriptId, "StartTownTaxTimer", minutes ) )
		pSock.SysMessage( "Faction town tax timer started." );
	else
		pSock.SysMessage( "Unable to start faction town tax timer." );
}

function command_FACTIONTAXSTOP( pSock, cmdString )
{
	if( TriggerEvent( CommandFactionTownScriptId, "StopTownTaxTimer" ) )
		pSock.SysMessage( "Faction town tax timer stopped." );
	else
		pSock.SysMessage( "Unable to stop faction town tax timer." );
}

function command_FACTIONTAXSTATUS( pSock, cmdString )
{
	if( !TriggerEvent( CommandFactionTownScriptId, "ShowTownTaxStatus", pSock ) )
		pSock.SysMessage( "Unable to show faction town tax timer status." );
}

function command_FACTIONROLE( pSock, cmdString )
{
	var cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	var parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	if( parts.length < 3 )
	{
		pSock.SysMessage( "Usage: 'factionrole <player> <sheriff|finance|commander> <TB|COM|MIN|SL>" );
		return;
	}

	var factionKey = parts[parts.length - 1].toUpperCase();
	var roleName = parts[parts.length - 2].toLowerCase();
	var playerParts = [];
	for( var partIndex = 0; partIndex < parts.length - 2; partIndex++ )
		playerParts.push( parts[partIndex] );

	if( !CommandIsRoleValid( roleName ) || !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "Usage: 'factionrole <player> <sheriff|finance|commander> <TB|COM|MIN|SL>" );
		return;
	}

	var targetChar = CommandFindPlayer( playerParts.join( " " ) );
	if( !ValidateObject( targetChar ) )
	{
		pSock.SysMessage( "Unable to find that player." );
		return;
	}

	if( targetChar.GetTag( "faction" ) !== factionKey )
	{
		pSock.SysMessage( targetChar.name + " must be a member of " + CommandFactionUsageName( factionKey ) + " before receiving that role." );
		return;
	}

	CommandSetFactionRole( targetChar, roleName, factionKey );
	pSock.SysMessage( targetChar.name + " is now " + CommandFactionUsageName( factionKey ) + " " + CommandRoleDisplayName( roleName ) + "." );
	if( targetChar.socket != null )
		targetChar.SysMessage( "You are now " + CommandFactionUsageName( factionKey ) + " " + CommandRoleDisplayName( roleName ) + "." );
}

function command_FACTIONROLECLEAR( pSock, cmdString )
{
	var targetChar = CommandFindPlayer( cmdString );
	if( !ValidateObject( targetChar ) )
	{
		pSock.SysMessage( "Usage: 'factionroleclear <player>" );
		return;
	}

	CommandClearFactionRole( targetChar );
	pSock.SysMessage( "Faction role cleared for " + targetChar.name + "." );
	if( targetChar.socket != null )
		targetChar.SysMessage( "Your faction role has been cleared." );
}

function command_FACTIONROLES( pSock, cmdString )
{
	CommandRoleListSocket = pSock;
	var roleCount = IterateOver( "CHARACTER" );
	CommandRoleListSocket = null;

	if( roleCount == 0 )
		pSock.SysMessage( "No faction roles are assigned." );
}

function command_FACTIONAPPOINT( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	var parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	if( parts.length < 2 )
	{
		pSock.SysMessage( "Usage: 'factionappoint <player> <sheriff|finance> [TB|COM|MIN|SL]" );
		return;
	}

	var factionKey = "";
	var roleIndex = parts.length - 1;
	var maybeFaction = parts[parts.length - 1].toUpperCase();
	if( CommandIsFactionValid( maybeFaction ) )
	{
		factionKey = maybeFaction;
		roleIndex = parts.length - 2;
	}

	var roleName = parts[roleIndex].toLowerCase();
	var playerParts = [];
	for( var partIndex = 0; partIndex < roleIndex; partIndex++ )
		playerParts.push( parts[partIndex] );

	if( roleName !== "sheriff" && roleName !== "finance" )
	{
		pSock.SysMessage( "Usage: 'factionappoint <player> <sheriff|finance> [TB|COM|MIN|SL]" );
		return;
	}

	if( factionKey === "" )
		factionKey = pUser.GetTag( "faction" );

	if( !CommandIsFactionValid( factionKey ) && !CommandIsStaff( pUser ) )
	{
		pSock.SysMessage( "You are not in a faction." );
		return;
	}

	var targetChar = CommandFindPlayer( playerParts.join( " " ) );
	if( !ValidateObject( targetChar ) )
	{
		pSock.SysMessage( "Unable to find that player." );
		return;
	}

	if( factionKey === "" && CommandIsStaff( pUser ) )
		factionKey = targetChar.GetTag( "faction" );

	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "Unable to determine appointment faction." );
		return;
	}

	if( !CommandCanAppointFactionRole( pUser, factionKey ) )
	{
		pSock.SysMessage( "Only your faction Commander may appoint that role." );
		return;
	}

	if( targetChar.GetTag( "faction" ) !== factionKey )
	{
		pSock.SysMessage( targetChar.name + " must be a member of your faction." );
		return;
	}

	CommandSetFactionRole( targetChar, roleName, factionKey );
	pSock.SysMessage( targetChar.name + " is now " + CommandFactionUsageName( factionKey ) + " " + CommandRoleDisplayName( roleName ) + "." );
	if( targetChar.socket != null )
		targetChar.SysMessage( "You are now " + CommandFactionUsageName( factionKey ) + " " + CommandRoleDisplayName( roleName ) + "." );
}

function command_FACTIONGUARD( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var factionKey = String( cmdString ).replace( /^\s+|\s+$/g, "" ).toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "Usage: 'factionguard <TB|COM|MIN|SL>" );
		return;
	}

	if( !CommandCanUseFactionNpcCommand( pSock, pUser, factionKey, "sheriff" ) )
		return;

	if( !CommandCanPlaceFactionNpc( pSock, pUser, factionKey ) )
		return;

	var townName = CommandCurrentFactionTown( pUser );
	if( !CommandSpendTownTreasury( pSock, pUser, townName, CommandFactionGuardCost, "a faction guard" ) )
		return;

	var guard = CommandSpawnFactionNpc( pSock, pUser, "FACTION_GUARD_" + factionKey, factionKey, "guard", "" );
	if( ValidateObject( guard ) )
	{
		pSock.SysMessage( CommandFactionUsageName( factionKey ) + " faction guard created." );
	}
	else
	{
		if( !CommandIsStaff( pUser ) )
			TriggerEvent( CommandFactionTownScriptId, "TownAddTreasury", townName, CommandFactionGuardCost );
		pSock.SysMessage( "Unable to create faction guard." );
	}
}

function command_FACTIONVENDOR( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var parts = String( cmdString ).replace( /^\s+|\s+$/g, "" ).toUpperCase().split( /\s+/ );
	var factionKey = "";
	var vendorType = "";

	if( parts.length === 2 )
	{
		if( CommandIsFactionValid( parts[0] ) )
		{
			factionKey = parts[0];
			vendorType = parts[1];
		}
		else if( CommandIsFactionValid( parts[1] ) )
		{
			factionKey = parts[1];
			vendorType = parts[0];
		}
	}

	if( !CommandIsFactionValid( factionKey ) || !CommandIsVendorTypeValid( vendorType ) )
	{
		pSock.SysMessage( "Usage: 'factionvendor <TB|COM|MIN|SL> <REAGENT|BOARD|BOTTLE|EQUIPMENT|HORSE>" );
		return;
	}

	if( !CommandCanUseFactionNpcCommand( pSock, pUser, factionKey, "finance" ) )
		return;

	if( !CommandCanPlaceFactionNpc( pSock, pUser, factionKey ) )
		return;

	var townName = CommandCurrentFactionTown( pUser );
	if( !CommandSpendTownTreasury( pSock, pUser, townName, CommandFactionVendorCost, "a faction vendor" ) )
		return;

	var vendor = CommandSpawnFactionNpc( pSock, pUser, "FACTION_" + vendorType + "_VENDOR_" + factionKey, factionKey, "vendor", vendorType );
	if( ValidateObject( vendor ) )
	{
		pSock.SysMessage( CommandFactionUsageName( factionKey ) + " " + vendorType.toLowerCase() + " faction vendor created." );
	}
	else
	{
		if( !CommandIsStaff( pUser ) )
			TriggerEvent( CommandFactionTownScriptId, "TownAddTreasury", townName, CommandFactionVendorCost );
		pSock.SysMessage( "Unable to create faction vendor." );
	}
}

function command_FACTIONITEMCHECK( pSock, cmdString )
{
	if( !TriggerEvent( 8507, "ShowFactionItemCheck", pSock ) )
		pSock.SysMessage( "Unable to check faction items." );
}

function command_FACTIONITEMCLEANUP( pSock, cmdString )
{
	var cleanedCount = TriggerEvent( 8507, "CleanupInvalidFactionItems" );
	pSock.SysMessage( "Removed " + cleanedCount + " invalid faction item(s) or mount(s)." );
}

function command_FACTIONSILVER( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var parts = cmdString.split( " " );
	var action = "";
	var amount = 0;

	if( parts.length >= 1 )
		action = parts[0].toLowerCase();
	if( parts.length >= 2 )
		amount = Number( parts[1] );

	if( action !== "add" && action !== "set" )
	{
		pSock.SysMessage( "Usage: 'factionsilver <add|set> <amount>" );
		return;
	}

	if( isNaN( amount ) )
	{
		pSock.SysMessage( "Amount must be a number." );
		return;
	}

	var currentSilver = pUser.GetTag( "faction_silver" );
	var newSilver = currentSilver;

	if( action === "add" )
		newSilver = currentSilver + amount;
	else
		newSilver = amount;

	newSilver = CommandClampSilver( newSilver );

	pUser.SetTag( "faction_silver", newSilver );
	pSock.SysMessage( "Faction silver set to " + newSilver + "." );
}

function CommandCreateFactionItem( pSock, pUser, itemType, factionKey )
{
	var sectionId = "";
	if( itemType === "ROBE" )
		sectionId = "FACTION_ROBE_" + factionKey;
	else if( itemType === "SHIELD" )
		sectionId = "FACTION_SHIELD_" + factionKey;
	else if( itemType === "HORSE" )
		sectionId = "FACTION_WAR_HORSE_DEED";
	else
		return null;

	var newItem = CreateDFNItem( pSock, pUser, sectionId, 1, "ITEM", true );
	if( ValidateObject( newItem ) )
	{
		newItem.SetTag( "faction_item", 1 );
		newItem.SetTag( "item_faction", factionKey );
		if( itemType === "HORSE" )
		{
			newItem.SetTag( "faction_horse_deed", 1 );
			newItem.name = factionKey + " Faction War Horse Deed";
		}
	}

	return newItem;
}

function command_SPAWNFITEM( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var parts = cmdString.toUpperCase().split( " " );
	var itemType = parts[0];
	var factionKey = parts[1];

	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "Usage: 'spawnfitem <robe|shield|horse> <TB|COM|MIN|SL>" );
		return;
	}

	var newItem = CommandCreateFactionItem( pSock, pUser, itemType, factionKey );
	if( !ValidateObject( newItem ) )
	{
		pSock.SysMessage( "Unable to create faction item." );
		return;
	}

	pSock.SysMessage( "Faction item created." );
}

function command_SPAWNTRAPDEED( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var trapType = cmdString.toUpperCase();
	if( trapType !== "EXPLOSION" && trapType !== "GAS" && trapType !== "SAW" && trapType !== "SPIKE" )
	{
		pSock.SysMessage( "Usage: 'spawntrapdeed <EXPLOSION|GAS|SAW|SPIKE>" );
		return;
	}

	var deed = CreateDFNItem( pSock, pUser, "FACTION_TRAP_DEED", 1, "ITEM", true );
	if( ValidateObject( deed ) )
	{
		deed.SetTag( "trap_deed", 1 );
		deed.SetTag( "trap_deed_type", trapType );
		deed.name = "Faction " + trapType + " Trap Deed";
		pSock.SysMessage( "Trap deed created." );
	}
	else
	{
		pSock.SysMessage( "Unable to create trap deed." );
	}
}

function command_ELECTIONSTART( pSock, cmdString )
{
	var factionKey = cmdString.toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "Usage: 'electionstart <TB|COM|MIN|SL>" );
		return;
	}

	if( TriggerEvent( CommandFactionElectionScriptId, "StartElection", factionKey ) )
		pSock.SysMessage( "Election started for " + factionKey + "." );
	else
		pSock.SysMessage( "Could not start election." );
}

function command_ELECTIONVOTE( pSock, cmdString )
{
	var factionKey = cmdString.toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "Usage: 'electionvote <TB|COM|MIN|SL>" );
		return;
	}

	if( TriggerEvent( CommandFactionElectionScriptId, "BeginVoting", factionKey ) )
		pSock.SysMessage( "Voting phase started for " + factionKey + "." );
	else
		pSock.SysMessage( "Could not start voting phase." );
}

function command_ELECTIONEND( pSock, cmdString )
{
	var factionKey = cmdString.toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "Usage: 'electionend <TB|COM|MIN|SL>" );
		return;
	}

	if( TriggerEvent( CommandFactionElectionScriptId, "ConcludeElection", factionKey ) )
		pSock.SysMessage( "Election concluded for " + factionKey + "." );
	else
		pSock.SysMessage( "Could not conclude election." );
}

function command_ELECTIONSTATUS( pSock, cmdString )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var factionKey = pUser.GetTag( "faction" );
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "You are not in a faction." );
		return;
	}

	TriggerEvent( CommandFactionElectionScriptId, "ShowElectionStatus", pSock, factionKey );
}

function command_ELECTIONRESET( pSock, cmdString )
{
	var factionKey = cmdString.toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( "Usage: 'electionreset <TB|COM|MIN|SL>" );
		return;
	}

	if( TriggerEvent( CommandFactionElectionScriptId, "ResetElection", factionKey ) )
		pSock.SysMessage( "Election state for " + factionKey + " has been reset." );
	else
		pSock.SysMessage( "Could not reset election." );
}
