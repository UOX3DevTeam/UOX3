/// <reference path="../definitions.d.ts" />
// @ts-check

// =============================================================================
// factions_commands.js
// UOX3 Faction System - GM testing commands
// COMMAND_SCRIPTS ID: 1072
// =============================================================================

const commandMaxFactionSilver = parseInt( GetServerSetting( "FACTIONMAXSILVER" ), 10 );
const commandFactionCombatScriptId = 8501;
const commandFactionSigilScriptId = 8502;
const commandFactionElectionScriptId = 8508;
const commandFactionTownScriptId = 8509;
const commandFactionStrongholdScriptId = 8511;
const commandFactionNpcScriptId = 8512;
const commandFactionPlayerDataScriptId = 8513;
const commandFactionSetupDataScriptId = 8514;
const commandFactionGuardCost = parseInt( GetServerSetting( "FACTIONGUARDCOST" ), 10 );
const commandFactionVendorCost = parseInt( GetServerSetting( "FACTIONVENDORCOST" ), 10 );
const commandFactionAdminHealthButton = 9001;
const commandFactionAdminSetupButton = 9002;
const commandFactionAdminDedupeButton = 9003;
const commandFactionAdminSyncTownsButton = 9004;
const commandFactionAdminTownStatusButton = 9005;
const commandFactionAdminTreasuryButton = 9006;
const commandFactionAdminTaxStartButton = 9007;
const commandFactionAdminTaxStopButton = 9008;
const commandFactionAdminTaxStatusButton = 9009;
const commandFactionAdminSigilsButton = 9010;
const commandFactionAdminStrongholdsButton = 9011;
const commandFactionAdminRemoveConfirmButton = 9012;
const commandFactionAdminRemoveNowButton = 9013;
const commandFactionAdminBackButton = 9014;

const commandFactionSetupItems = [
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
	[ "FACTION_SIGIL_MAGINCIA", 1, 4 ]
];

const commandFactionRegionChecks = [
	[ "Britain", 3 ],
	[ "Trinsic", 9 ],
	[ "Moonglow", 13 ],
	[ "Skara Brae", 8 ],
	[ "Yew", 7 ],
	[ "Vesper", 11 ],
	[ "Minoc", 12 ],
	[ "Magincia", 15 ]
];

const commandFactionVendorTypes = [
	"REAGENT",
	"BOARD",
	"BOTTLE",
	"ORE",
	"EQUIPMENT",
	"HORSE"
];

function CommandFactionVendorPurchaseCost( vendorType )
{
	if( vendorType === "BOARD" || vendorType === "ORE" )
		return 3000;
	return commandFactionVendorCost;
}

const commandFactionRoles = [
	"sheriff",
	"finance",
	"commander"
];

let commandFindPlayerName = "";
let commandFindPlayerResult = null;
let commandFindNpcName = "";
let commandFindNpcResult = null;
let commandRoleListSocket = null;
let commandClearRoleName = "";
let commandClearRoleFaction = "";
let commandClearRoleExcept = 0;
let commandClearRoleTown = "";
let commandBroadcastFaction = "";
let commandBroadcastMessage = "";
let commandBroadcastCount = 0;
let commandHealthScanActive = false;
let commandHealthControllerCount = 0;
let commandHealthFactionStoneCount = 0;
let commandHealthJoinStoneCount = 0;
let commandHealthSigilCount = 0;
let commandHealthSigilHomeCount = 0;
let commandHealthTownstoneCount = 0;
let commandHealthFactionNpcCounts = {};
let commandSetupScanActive = false;
let commandSetupFound = {};
let commandSetupRanks = {};
let commandDedupeActive = false;
let commandDedupeKept = {};
let commandDedupeRanks = {};
let commandDedupeRemovedCount = 0;
let commandRemoveFactionsMode = "";
let commandRemoveFactionsItems = 0;
let commandRemoveFactionsNpcs = 0;
let commandRemoveFactionsPlayers = 0;
let commandRemoveFactionsRegions = 0;
let commandLeaderboardFaction = "";
let commandLeaderboardEntries = [];
let commandDataMigrateActive = false;
let commandDataMigrateCount = 0;

function CommandRegistration()
{
	RegisterCommand( "factionsilver", 5, true );
	RegisterCommand( "factioncontroller", 5, true );
	RegisterCommand( "factionsetup", 5, true );
	RegisterCommand( "factionkp", 5, true );
	RegisterCommand( "factionregioncheck", 5, true );
	RegisterCommand( "factiontowns", 0, true );
	RegisterCommand( "factiontownset", 5, true );
	RegisterCommand( "factionstatus", 0, true );
	RegisterCommand( "factionranks", 0, true );
	RegisterCommand( "factionleaderboard", 0, true );
	RegisterCommand( "factionleaders", 0, true );
	RegisterCommand( "factionadmin", 5, true );
	RegisterCommand( "factionhealth", 5, true );
	RegisterCommand( "factiondedupe", 5, true );
	RegisterCommand( "factiondatamigrate", 5, true );
	RegisterCommand( "removefactions", 5, true );
	RegisterCommand( "factiontownsync", 5, true );
	RegisterCommand( "factiontownnpcs", 5, true );
	RegisterCommand( "factiontownclear", 5, true );
	RegisterCommand( "factiontownlimit", 5, true );
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
	RegisterCommand( "factioncapture", 5, true );
	RegisterCommand( "factionnotice", 0, true );
	RegisterCommand( "factionnoticeclear", 0, true );
	RegisterCommand( "factionalert", 0, true );
	RegisterCommand( "factionrole", 5, true );
	RegisterCommand( "factionroleclear", 5, true );
	RegisterCommand( "factionroles", 5, true );
	RegisterCommand( "factionappoint", 0, true );
	RegisterCommand( "factionguard", 0, true );
	RegisterCommand( "factionvendor", 0, true );
	RegisterCommand( "factionnpc", 5, true );
	RegisterCommand( "factionnpcclear", 5, true );
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
	for( let typeIndex = 0; typeIndex < commandFactionVendorTypes.length; typeIndex++ )
	{
		if( commandFactionVendorTypes[typeIndex] === vendorType )
			return true;
	}

	return false;
}

function CommandIsRoleValid( roleName )
{
	for( let roleIndex = 0; roleIndex < commandFactionRoles.length; roleIndex++ )
	{
		if( commandFactionRoles[roleIndex] === roleName )
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
	const rankNames = [
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

function CommandRankPoints( rank )
{
	const rankPoints = [ 0, 5, 10, 20, 40, 80, 160, 320, 640, 1280 ];

	rank = parseInt( rank, 10 );
	if( isNaN( rank ) || rank < 0 || rank >= rankPoints.length )
		rank = 0;

	return rankPoints[rank];
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
	if( amount > commandMaxFactionSilver )
		amount = commandMaxFactionSilver;

	return amount;
}

function CommandClampNonNegative( amount )
{
	amount = parseInt( amount, 10 );
	if( isNaN( amount ) || amount < 0 )
		return 0;

	return amount;
}

function CommandUpdateFactionRank( pChar )
{
	if( !ValidateObject( pChar ) )
		return 0;

	let killPoints = CommandClampNonNegative( TriggerEvent( commandFactionPlayerDataScriptId, "GetFactionValue", pChar, "killPoints", pChar.GetTag( "faction_kp" ) ) );
	let rank = 0;
	for( let rankIndex = 9; rankIndex >= 0; rankIndex-- )
	{
		if( killPoints >= CommandRankPoints( rankIndex ) )
		{
			rank = rankIndex;
			break;
		}
	}

	TriggerEvent( commandFactionPlayerDataScriptId, "SetFactionValue", pChar, "rank", rank );
	return rank;
}

function CommandParsePlayerAmountArgs( pSock, cmdString, usageText )
{
	let cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	let parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	if( parts.length < 3 )
	{
		pSock.SysMessage( usageText );
		return null;
	}

	let action = parts[0].toLowerCase();
	if( action !== "add" && action !== "set" )
	{
		pSock.SysMessage( usageText );
		return null;
	}

	let amount = parseInt( parts[parts.length - 1], 10 );
	if( isNaN( amount ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25126, pSock.language ) );
		return null;
	}

	const playerParts = [];
	for( let partIndex = 1; partIndex < parts.length - 1; partIndex++ )
		playerParts.push( parts[partIndex] );

	let targetChar = CommandFindPlayer( playerParts.join( " " ) );
	if( !ValidateObject( targetChar ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25127, pSock.language ) );
		return null;
	}

	return {
		action: action,
		targetChar: targetChar,
		amount: amount
	};
}

function CommandFindPlayer( playerText )
{
	commandFindPlayerResult = null;
	commandFindPlayerName = String( playerText ).replace( /^\s+|\s+$/g, "" ).toLowerCase();
	if( commandFindPlayerName === "" )
		return null;

	const serialNum = parseInt( commandFindPlayerName, 10 );
	if( !isNaN( serialNum ) )
	{
		const serialChar = CalcCharFromSer( serialNum );
		if( ValidateObject( serialChar ) && serialChar.isChar && !serialChar.npc )
		{
			commandFindPlayerResult = null;
			commandFindPlayerName = "";
			return serialChar;
		}
	}

	IterateOver( "CHARACTER" );
	const foundPlayer = commandFindPlayerResult;
	commandFindPlayerResult = null;
	commandFindPlayerName = "";
	return foundPlayer;
}

function CommandFindNpc( npcText )
{
	commandFindNpcResult = null;
	commandFindNpcName = String( npcText ).replace( /^\s+|\s+$/g, "" ).toLowerCase();
	if( commandFindNpcName === "" )
		return null;

	const serialNum = parseInt( commandFindNpcName, 10 );
	if( !isNaN( serialNum ) )
	{
		const serialChar = CalcCharFromSer( serialNum );
		if( ValidateObject( serialChar ) && serialChar.isChar && serialChar.npc )
		{
			commandFindNpcResult = null;
			commandFindNpcName = "";
			return serialChar;
		}
	}

	IterateOver( "CHARACTER" );
	const foundNpc = commandFindNpcResult;
	commandFindNpcResult = null;
	commandFindNpcName = "";
	return foundNpc;
}

function CommandSetFactionRole( pChar, roleName, factionKey, townName )
{
	if( !ValidateObject( pChar ) || !CommandIsRoleValid( roleName ) || !CommandIsFactionValid( factionKey ) )
		return false;

	townName = roleName === "commander" ? "" : TriggerEvent( commandFactionTownScriptId, "TownNormalizeName", townName );
	if( roleName !== "commander" && TriggerEvent( commandFactionTownScriptId, "TownGetDefault", townName ) == null )
		return false;

	CommandClearFactionRoleHolders( roleName, factionKey, pChar.serial, townName );
	let factionData = TriggerEvent( commandFactionPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	factionData.role = roleName;
	factionData.roleFaction = factionKey;
	factionData.roleTown = townName;
	factionData.roleSetAt = GetCurrentClock();
	factionData.commander = ( roleName === "commander" );
	TriggerEvent( commandFactionPlayerDataScriptId, "WriteFactionPlayerData", pChar, factionData );

	return true;
}

function CommandClearFactionRole( pChar )
{
	if( !ValidateObject( pChar ) )
		return false;

	let factionData = TriggerEvent( commandFactionPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	factionData.role = "";
	factionData.roleFaction = "";
	factionData.roleTown = "";
	factionData.roleSetAt = 0;
	factionData.commander = false;
	TriggerEvent( commandFactionPlayerDataScriptId, "WriteFactionPlayerData", pChar, factionData );
	return true;
}

function CommandClearFactionRoleHolders( roleName, factionKey, exceptSerial, townName )
{
	if( !CommandIsRoleValid( roleName ) || !CommandIsFactionValid( factionKey ) )
		return 0;

	commandClearRoleName = roleName;
	commandClearRoleFaction = factionKey;
	commandClearRoleExcept = exceptSerial;
	commandClearRoleTown = String( townName || "" );
	const clearCount = IterateOver( "CHARACTER" );
	commandClearRoleName = "";
	commandClearRoleFaction = "";
	commandClearRoleExcept = 0;
	commandClearRoleTown = "";
	return clearCount;
}

function CommandHasFactionRole( pChar, roleName, factionKey )
{
	if( !ValidateObject( pChar ) || !CommandIsRoleValid( roleName ) || !CommandIsFactionValid( factionKey ) )
		return false;

	let factionData = TriggerEvent( commandFactionPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	if( factionData.faction !== factionKey )
		return false;
	if( roleName === "commander" && factionData.commander )
		return true;
	if( factionData.role !== roleName )
		return false;

	return factionData.roleFaction === factionKey;
}

function CommandCanAppointFactionRole( pUser, factionKey )
{
	if( CommandIsStaff( pUser ) )
		return true;

	return CommandHasFactionRole( pUser, "commander", factionKey );
}

function onIterate( toCheck )
{
	if( commandFindPlayerName !== "" )
	{
		if( ValidateObject( toCheck ) && toCheck.isChar && !toCheck.npc )
		{
			let checkName = String( toCheck.name ).toLowerCase();
			if( checkName === commandFindPlayerName )
			{
				commandFindPlayerResult = toCheck;
				return true;
			}
		}

		return false;
	}

	if( commandFindNpcName !== "" )
	{
		if( ValidateObject( toCheck ) && toCheck.isChar && toCheck.npc )
		{
			let npcName = String( toCheck.name ).toLowerCase();
			if( npcName === commandFindNpcName )
			{
				commandFindNpcResult = toCheck;
				return true;
			}
		}

		return false;
	}

	if( commandRoleListSocket != null )
	{
		if( ValidateObject( toCheck ) && toCheck.isChar && !toCheck.npc )
		{
			const roleData = TriggerEvent( commandFactionPlayerDataScriptId, "ReadFactionPlayerData", toCheck );
			let roleName = roleData.role;
			let roleFaction = roleData.roleFaction;
			if( CommandIsRoleValid( roleName ) && CommandIsFactionValid( roleFaction ) )
			{
				const townText = roleData.roleTown !== "" ? " of " + roleData.roleTown : "";
				commandRoleListSocket.SysMessage( GetDictionaryEntry( 25128, commandRoleListSocket.language ).replace( /%s/, String( toCheck.name ) ).replace( /%s/, String( CommandFactionUsageName( roleFaction ) ) ).replace( /%s/, String( CommandRoleDisplayName( roleName ) ) ).replace( /%s/, String( townText ) ) );
				return true;
			}
		}

		return false;
	}

	if( commandClearRoleName !== "" )
	{
		if( ValidateObject( toCheck ) && toCheck.isChar && !toCheck.npc && toCheck.serial != commandClearRoleExcept )
		{
			const clearRoleData = TriggerEvent( commandFactionPlayerDataScriptId, "ReadFactionPlayerData", toCheck );
			if( clearRoleData.role === commandClearRoleName && clearRoleData.roleFaction === commandClearRoleFaction &&
				( commandClearRoleName === "commander" || clearRoleData.roleTown === commandClearRoleTown ) )
			{
				CommandClearFactionRole( toCheck );
				return true;
			}
		}

		return false;
	}

	if( commandBroadcastFaction !== "" )
	{
		if( ValidateObject( toCheck ) && toCheck.isChar && !toCheck.npc && toCheck.GetTag( "faction" ) === commandBroadcastFaction )
		{
			if( toCheck.socket != null )
			{
				toCheck.SysMessage( commandBroadcastMessage );
				commandBroadcastCount++;
			}
		}

		return false;
	}

	if( commandLeaderboardFaction !== "" )
	{
		if( ValidateObject( toCheck ) && toCheck.isChar && !toCheck.npc && toCheck.GetTag( "faction" ) === commandLeaderboardFaction )
		{
			let factionData = TriggerEvent( commandFactionPlayerDataScriptId, "ReadFactionPlayerData", toCheck );
			let killPoints = parseInt( factionData.killPoints, 10 );
			let silver = parseInt( factionData.silver, 10 );
			let rank = parseInt( factionData.rank, 10 );
			let captures = parseInt( factionData.captures, 10 );
			if( isNaN( killPoints ) )
				killPoints = 0;
			if( isNaN( silver ) )
				silver = 0;
			if( isNaN( rank ) )
				rank = 0;
			if( isNaN( captures ) )
				captures = 0;

			commandLeaderboardEntries.push({
				name: String( toCheck.name ),
				killPoints: killPoints,
				silver: silver,
				rank: rank,
				captures: captures,
				online: toCheck.online
			});
		}

		return false;
	}

	if( commandDataMigrateActive )
	{
		if( ValidateObject( toCheck ) && toCheck.isChar && !toCheck.npc )
		{
			if( TriggerEvent( commandFactionPlayerDataScriptId, "MigrateFactionTagsToFile", toCheck ) )
			{
				commandDataMigrateCount++;
				return true;
			}
		}

		return false;
	}

	if( commandHealthScanActive )
	{
		if( ValidateObject( toCheck ) && toCheck.isItem )
		{
			if( toCheck.GetTag( "faction_controller" ) == 1 )
				commandHealthControllerCount++;
			if( toCheck.GetTag( "faction_stone" ) == 1 )
				commandHealthFactionStoneCount++;
			if( toCheck.GetTag( "join_stone" ) == 1 )
				commandHealthJoinStoneCount++;
			if( toCheck.GetTag( "sigil" ) == 1 )
			{
				commandHealthSigilCount++;
				if( toCheck.GetTag( "sigil_home_set" ) == 1 )
					commandHealthSigilHomeCount++;
			}
			if( toCheck.GetTag( "faction_townstone" ) == 1 )
				commandHealthTownstoneCount++;
		}
		else if( ValidateObject( toCheck ) && toCheck.isChar && toCheck.npc && toCheck.GetTag( "faction_npc" ) == 1 )
		{
			let npcFaction = toCheck.GetTag( "npc_faction" );
			if( !CommandIsFactionValid( npcFaction ) )
				npcFaction = toCheck.GetTag( "faction" );
			if( CommandIsFactionValid( npcFaction ) )
				commandHealthFactionNpcCounts[npcFaction]++;
		}

		return false;
	}

	if( commandSetupScanActive )
	{
		if( ValidateObject( toCheck ) && toCheck.isItem )
		{
			let setupKey = CommandDedupeItemKey( toCheck );
			if( setupKey !== "" )
			{
				const setupRank = CommandDedupeItemRank( toCheck );
				if( typeof commandSetupFound[setupKey] == "undefined" || !ValidateObject( commandSetupFound[setupKey] ) || setupRank > commandSetupRanks[setupKey] )
				{
					commandSetupFound[setupKey] = toCheck;
					commandSetupRanks[setupKey] = setupRank;
				}
			}
		}

		return false;
	}

	if( commandDedupeActive )
	{
		if( ValidateObject( toCheck ) && toCheck.isItem )
		{
			const dedupeKey = CommandDedupeItemKey( toCheck );
			if( dedupeKey !== "" )
			{
				const dedupeRank = CommandDedupeItemRank( toCheck );
				if( typeof commandDedupeKept[dedupeKey] != "undefined" && ValidateObject( commandDedupeKept[dedupeKey] ) )
				{
					if( dedupeRank > commandDedupeRanks[dedupeKey] )
					{
						commandDedupeKept[dedupeKey].Delete();
						commandDedupeKept[dedupeKey] = toCheck;
						commandDedupeRanks[dedupeKey] = dedupeRank;
						commandDedupeRemovedCount++;
					}
					else
					{
						toCheck.Delete();
						commandDedupeRemovedCount++;
					}
				}
				else
				{
					commandDedupeKept[dedupeKey] = toCheck;
					commandDedupeRanks[dedupeKey] = dedupeRank;
				}
			}
		}

		return false;
	}

	if( commandRemoveFactionsMode === "items" )
	{
		if( CommandRemoveFactionItem( toCheck ) )
			return true;

		return false;
	}

	if( commandRemoveFactionsMode === "chars" )
	{
		if( CommandRemoveFactionChar( toCheck ) )
			return true;

		return false;
	}

	return false;
}

function command_FACTIONCONTROLLER( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let controller = CreateDFNItem( pSock, pUser, "FACTION_CONTROLLER", 1, "ITEM", true );
	if( ValidateObject( controller ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25129, pSock.language ) );
		TriggerEvent( commandFactionElectionScriptId, "RegisterController", controller );
		TriggerEvent( commandFactionTownScriptId, "RegisterController", controller );
		TriggerEvent( commandFactionTownScriptId, "SyncTownControl" );
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 25130, pSock.language ) );
	}
}

function CommandPlaceFactionItem( pSock, pUser, sectionId, xOffset, yOffset )
{
	const newItem = CreateDFNItem( pSock, pUser, sectionId, 1, "ITEM", false );
	if( !ValidateObject( newItem ) )
		return null;

	newItem.SetLocation( pUser.x + xOffset, pUser.y + yOffset, pUser.z, pUser.worldnumber, pUser.instanceID );
	return newItem;
}

function CommandPlaceFactionItemAt( pSock, pUser, sectionId, location )
{
	const newItem = CreateDFNItem( pSock, pUser, sectionId, 1, "ITEM", false );
	if( !ValidateObject( newItem ) )
		return null;

	let worldNum = typeof location.world == "undefined" ? 0 : parseInt( location.world, 10 );
	let instanceId = typeof location.instance == "undefined" ? 0 : parseInt( location.instance, 10 );
	if( isNaN( worldNum ) )
		worldNum = 0;
	if( isNaN( instanceId ) )
		instanceId = 0;

	newItem.SetLocation( parseInt( location.x, 10 ), parseInt( location.y, 10 ), parseInt( location.z, 10 ), worldNum, instanceId );
	return newItem;
}

function CommandSetupOptions( cmdString )
{
	const setupText = String( cmdString ).replace( /^\s+|\s+$/g, "" ).toLowerCase();
	let parts = setupText.length > 0 ? setupText.split( /\s+/ ) : [];
	const options = {
		force: false,
		local: false,
		relocate: false
	};

	for( let partIndex = 0; partIndex < parts.length; partIndex++ )
	{
		if( parts[partIndex] === "force" )
			options.force = true;
		else if( parts[partIndex] === "local" )
			options.local = true;
		else if( parts[partIndex] === "relocate" )
			options.relocate = true;
	}

	return options;
}

function CommandSetupCreateController( pSock, pUser, forceSetup, stats )
{
	let controller = null;
	if( !forceSetup )
		controller = CommandSetupFoundObject( "controller" );

	if( !ValidateObject( controller ) )
	{
		controller = CreateDFNItem( pSock, pUser, "FACTION_CONTROLLER", 1, "ITEM", true );
		if( ValidateObject( controller ) )
		{
			stats.created++;
			pSock.SysMessage( GetDictionaryEntry( 25131, pSock.language ) );
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 25130, pSock.language ) );
			return null;
		}
	}
	else
	{
		stats.skipped++;
		pSock.SysMessage( GetDictionaryEntry( 25132, pSock.language ) );
	}

	TriggerEvent( commandFactionElectionScriptId, "RegisterController", controller );
	TriggerEvent( commandFactionTownScriptId, "RegisterController", controller );
	return controller;
}

function CommandMoveFactionSetupItem( setupItem, location )
{
	let worldNum = typeof location.world == "undefined" ? 0 : parseInt( location.world, 10 );
	let instanceId = typeof location.instance == "undefined" ? 0 : parseInt( location.instance, 10 );
	if( isNaN( worldNum ) )
		worldNum = 0;
	if( isNaN( instanceId ) )
		instanceId = 0;

	setupItem.SetLocation( parseInt( location.x, 10 ), parseInt( location.y, 10 ), parseInt( location.z, 10 ), worldNum, instanceId );
	if( setupItem.GetTag( "sigil" ) == 1 )
		TriggerEvent( commandFactionSigilScriptId, "SigilSetHome", setupItem );
}

function CommandTagFactionTownstone( townstone, townName, townData )
{
	if( !ValidateObject( townstone ) )
		return false;

	townstone.name = "Faction Townstone of " + townName;
	townstone.SetTag( "faction_townstone", 1 );
	townstone.SetTag( "faction_town", townName );
	townstone.SetTag( "faction_town_region", townData.region );
	townstone.SetTag( "faction_town_owner", townData.owner );
	townstone.AddScriptTrigger( 8510 );
	return true;
}

function CommandSetupPlaceSection( pSock, pUser, sectionId, location, forceSetup, relocateSetup, stats )
{
	if( location == null || typeof location != "object" )
	{
		pSock.SysMessage( GetDictionaryEntry( 25133, pSock.language ).replace( /%s/, String( sectionId ) ) );
		stats.failed++;
		return null;
	}

	let setupKey = CommandSetupSectionKey( sectionId );
	const existingItem = CommandSetupFoundObject( setupKey );
	if( !forceSetup && ValidateObject( existingItem ) )
	{
		if( relocateSetup )
		{
			CommandMoveFactionSetupItem( existingItem, location );
			stats.relocated++;
		}
		else
			stats.skipped++;
		return existingItem;
	}

	const newItem = CommandPlaceFactionItemAt( pSock, pUser, sectionId, location );
	if( ValidateObject( newItem ) )
	{
		if( newItem.GetTag( "sigil" ) == 1 )
			TriggerEvent( commandFactionSigilScriptId, "SigilSetHome", newItem );
		stats.created++;
		return newItem;
	}

	pSock.SysMessage( GetDictionaryEntry( 25134, pSock.language ).replace( /%s/, String( sectionId ) ) );
	stats.failed++;
	return null;
}

function CommandSetupPlaceTownstone( pSock, pUser, townName, townData, forceSetup, relocateSetup, stats )
{
	if( townData.townStone == null || typeof townData.townStone != "object" )
		return null;

	let setupKey = "townstone_" + townName;
	const existingItem = CommandSetupFoundObject( setupKey );
	if( !forceSetup && ValidateObject( existingItem ) )
	{
		CommandTagFactionTownstone( existingItem, townName, townData );
		if( relocateSetup )
		{
			CommandMoveFactionSetupItem( existingItem, townData.townStone );
			stats.relocated++;
		}
		else
			stats.skipped++;
		return existingItem;
	}

	const newItem = CommandPlaceFactionItemAt( pSock, pUser, "townstone", townData.townStone );
	if( ValidateObject( newItem ) )
	{
		CommandTagFactionTownstone( newItem, townName, townData );
		stats.created++;
		return newItem;
	}

	pSock.SysMessage( GetDictionaryEntry( 25135, pSock.language ).replace( /%s/, String( townName ) ) );
	stats.failed++;
	return null;
}

function CommandFactionSectionId( prefix, factionKey )
{
	return prefix + "_" + factionKey;
}

function CommandSetupFromData( pSock, pUser, forceSetup, relocateSetup, stats )
{
	let setupData = TriggerEvent( commandFactionSetupDataScriptId, "ReloadFactionSetupData" );
	if( setupData == null )
	{
		pSock.SysMessage( GetDictionaryEntry( 25136, pSock.language ).replace( /%s/, String( TriggerEvent( commandFactionSetupDataScriptId, "SetupDataLastError" ) ) ) );
		return false;
	}

	const factionOrder = [ "TB", "COM", "MIN", "SL" ];
	for( let factionIndex = 0; factionIndex < factionOrder.length; factionIndex++ )
	{
		let factionKey = factionOrder[factionIndex];
		let factionData = setupData.factions[factionKey];
		if( factionData == null )
		{
			pSock.SysMessage( GetDictionaryEntry( 25137, pSock.language ).replace( /%s/, String( factionKey ) ) );
			stats.failed++;
			continue;
		}

		CommandSetupPlaceSection( pSock, pUser, CommandFactionSectionId( "JOIN_STONE", factionKey ), factionData.joinStone, forceSetup, relocateSetup, stats );
		CommandSetupPlaceSection( pSock, pUser, CommandFactionSectionId( "FACTION_STONE", factionKey ), factionData.factionStone, forceSetup, relocateSetup, stats );

		if( factionData.stronghold && factionData.stronghold.center )
		{
			const center = factionData.stronghold.center;
			if( TriggerEvent( commandFactionStrongholdScriptId, "StrongholdSetLocation", factionKey, center.x, center.y, center.z, center.world, center.instance, factionData.stronghold.range ) )
				stats.strongholds++;
			else
				pSock.SysMessage( GetDictionaryEntry( 25138, pSock.language ).replace( /%s/, String( factionKey ) ).replace( /%s/, String( TriggerEvent( commandFactionStrongholdScriptId, "StrongholdLastError" ) ) ) );
		}
	}

	for( let townName in setupData.towns )
	{
		if( !setupData.towns.hasOwnProperty( townName ) )
			continue;

		const townData = setupData.towns[townName];
		if( townData.enabled === false )
		{
			stats.skipped++;
			continue;
		}

		CommandSetupPlaceSection( pSock, pUser, townData.section, townData.sigil, forceSetup, relocateSetup, stats );
		CommandSetupPlaceTownstone( pSock, pUser, townName, townData, forceSetup, relocateSetup, stats );
	}

	return true;
}

function CommandSetupLocal( pSock, pUser, forceSetup, stats )
{
	for( let itemIndex = 0; itemIndex < commandFactionSetupItems.length; itemIndex++ )
	{
		const setupItem = commandFactionSetupItems[itemIndex];
		let setupKey = CommandSetupSectionKey( setupItem[0] );
		if( !forceSetup && ValidateObject( CommandSetupFoundObject( setupKey ) ) )
		{
			stats.skipped++;
			continue;
		}

		const newItem = CommandPlaceFactionItem( pSock, pUser, setupItem[0], setupItem[1], setupItem[2] );
		if( ValidateObject( newItem ) )
		{
			if( newItem.GetTag( "sigil" ) == 1 )
				TriggerEvent( commandFactionSigilScriptId, "SigilSetHome", newItem );
			stats.created++;
		}
		else
		{
			stats.failed++;
			pSock.SysMessage( GetDictionaryEntry( 25134, pSock.language ).replace( /%s/, String( setupItem[0] ) ) );
		}
	}

	return true;
}

function CommandSetupScanItems()
{
	commandSetupFound = {};
	commandSetupRanks = {};
	commandSetupScanActive = true;
	IterateOver( "ITEM" );
	commandSetupScanActive = false;
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
	if( sectionId === "FACTION_SIGIL_MAGINCIA" )
		return "sigil_Magincia";

	return "";
}

function CommandSetupFoundObject( setupKey )
{
	if( setupKey === "" )
		return null;
	if( typeof commandSetupFound[setupKey] == "undefined" )
		return null;
	if( !ValidateObject( commandSetupFound[setupKey] ) )
		return null;

	return commandSetupFound[setupKey];
}

function command_FACTIONSETUP( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	const setupOptions = CommandSetupOptions( cmdString );
	const stats = {
		created: 0,
		skipped: 0,
		failed: 0,
		relocated: 0,
		strongholds: 0
	};

	if( !setupOptions.force )
		CommandSetupScanItems();

	let controller = CommandSetupCreateController( pSock, pUser, setupOptions.force, stats );
	if( !ValidateObject( controller ) )
	{
		commandSetupFound = {};
		commandSetupRanks = {};
		return;
	}

	const setupOk = setupOptions.local ? CommandSetupLocal( pSock, pUser, setupOptions.force, stats ) : CommandSetupFromData( pSock, pUser, setupOptions.force, setupOptions.relocate, stats );
	if( !setupOk )
	{
		commandSetupFound = {};
		commandSetupRanks = {};
		return;
	}

	if( setupOptions.force && setupOptions.local )
		pSock.SysMessage( GetDictionaryEntry( 25139, pSock.language ).replace( /%s/, String( stats.created ) ) );
	else if( setupOptions.force )
		pSock.SysMessage( GetDictionaryEntry( 25140, pSock.language ).replace( /%s/, String( stats.created ) ) );
	else if( setupOptions.local )
		pSock.SysMessage( GetDictionaryEntry( 25141, pSock.language ).replace( /%s/, String( stats.created ) ).replace( /%s/, String( stats.skipped ) ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25142, pSock.language ).replace( /%s/, String( stats.created ) ).replace( /%s/, String( stats.relocated ) ).replace( /%s/, String( stats.skipped ) ).replace( /%s/, String( stats.strongholds ) ) );
	if( stats.failed > 0 )
		pSock.SysMessage( GetDictionaryEntry( 25143, pSock.language ).replace( /%s/, String( stats.failed ) ) );

	if( TriggerEvent( commandFactionTownScriptId, "SyncTownControl" ) )
		pSock.SysMessage( GetDictionaryEntry( 25144, pSock.language ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25145, pSock.language ).replace( /%s/, String( TriggerEvent( commandFactionTownScriptId, "TownLastError" ) ) ) );

	commandSetupFound = {};
	commandSetupRanks = {};
}

function command_FACTIONREGIONCHECK( pSock, cmdString )
{
	for( let checkIndex = 0; checkIndex < commandFactionRegionChecks.length; checkIndex++ )
	{
		const regionCheck = commandFactionRegionChecks[checkIndex];
		const townRegion = GetTownRegion( regionCheck[1] );
		if( townRegion != null && typeof townRegion.id != "undefined" )
			pSock.SysMessage( GetDictionaryEntry( 25146, pSock.language ).replace( /%s/, String( regionCheck[0] ) ).replace( /%s/, String( regionCheck[1] ) ).replace( /%s/, String( townRegion.name ) ) );
		else
			pSock.SysMessage( GetDictionaryEntry( 25147, pSock.language ).replace( /%s/, String( regionCheck[0] ) ).replace( /%s/, String( regionCheck[1] ) ) );
	}
}

function command_FACTIONTOWNS( pSock, cmdString )
{
	TriggerEvent( commandFactionTownScriptId, "ShowTownStatus", pSock );
}

function command_FACTIONTOWNSYNC( pSock, cmdString )
{
	if( TriggerEvent( commandFactionTownScriptId, "SyncTownControl" ) )
		pSock.SysMessage( GetDictionaryEntry( 25148, pSock.language ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25149, pSock.language ) );
}

function command_FACTIONTOWNSET( pSock, cmdString )
{
	cmdString = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	let parts = cmdString.length > 0 ? cmdString.split( /\s+/ ) : [];
	if( parts.length < 2 )
	{
		pSock.SysMessage( GetDictionaryEntry( 25150, pSock.language ) );
		return;
	}

	let factionKey = parts[parts.length - 1].toUpperCase();
	const townParts = [];
	for( let partIndex = 0; partIndex < parts.length - 1; partIndex++ )
		townParts.push( parts[partIndex] );

	let townName = townParts.join( " " );
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25150, pSock.language ) );
		return;
	}

	if( TriggerEvent( commandFactionTownScriptId, "ApplyTownControl", townName, factionKey, 0 ) )
		pSock.SysMessage( GetDictionaryEntry( 25151, pSock.language ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25152, pSock.language ).replace( /%s/, String( TriggerEvent( commandFactionTownScriptId, "TownLastError" ) ) ) );
}

function CommandShowFactionStatusGump( pSock, pUser, factionKey )
{
	if( !pSock || !ValidateObject( pUser ) || !CommandIsFactionValid( factionKey ) )
		return false;

	const playerFaction = pUser.GetTag( "faction" );
	const isMember = CommandIsFactionValid( playerFaction );
	const displayFactionName = CommandFactionUsageName( factionKey );
	let y = 0;

	const myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 470, CommandIsStaff( pUser ) ? 430 : 390, 9200 );
	myGump.AddHTMLGump( 20, 15, 430, 25, 0, 0, "<CENTER><b>Faction Status</b></CENTER>" );
	myGump.AddHTMLGump( 25, 50, 420, 20, 0, 0, "Faction: " + displayFactionName );

	y = 80;
	if( isMember )
	{
		const playerData = TriggerEvent( commandFactionPlayerDataScriptId, "ReadFactionPlayerData", pUser );
		myGump.AddHTMLGump( 25, y, 420, 20, 0, 0, "Your Faction: " + CommandFactionUsageName( playerFaction ) );
		y += 25;
		myGump.AddHTMLGump( 25, y, 420, 20, 0, 0, "Rank: " + CommandRankName( playerData.rank ) );
		y += 25;
		myGump.AddHTMLGump( 25, y, 420, 20, 0, 0, "Kill Points: " + playerData.killPoints + ", Silver: " + playerData.silver );
		y += 25;
		let roleName = playerData.role;
		if( roleName === "" && playerData.commander )
			roleName = "commander";
		myGump.AddHTMLGump( 25, y, 420, 20, 0, 0, "Role: " + CommandRoleDisplayName( roleName ) );
		y += 30;
	}
	else
	{
		myGump.AddHTMLGump( 25, y, 420, 35, 0, 0, "You are not currently enlisted in a faction." );
		y += 45;
	}

	myGump.AddHTMLGump( 25, y, 420, 20, 0, 0, TriggerEvent( commandFactionSigilScriptId, "FactionScoreText", factionKey ) );
	y += 25;
	myGump.AddHTMLGump( 25, y, 420, 20, 0, 0, TriggerEvent( commandFactionCombatScriptId, "FactionKillStatsText", factionKey ) );
	y += 25;
	myGump.AddHTMLGump( 25, y, 420, 35, 0, 0, TriggerEvent( commandFactionSigilScriptId, "FactionNoticeText", factionKey ) );
	y += 40;
	myGump.AddHTMLGump( 25, y, 420, 45, 0, 0, "Controlled Towns: " + TriggerEvent( commandFactionTownScriptId, "TownControlledByFactionList", factionKey ) );
	y += 50;

	if( CommandIsStaff( pUser ) )
	{
		myGump.AddHTMLGump( 25, y, 420, 20, 0, 0, TriggerEvent( commandFactionTownScriptId, "TownTreasuryByFactionText", factionKey ) );
		y += 25;
	}

	myGump.AddButton( 25, y + 10, 0xFA5, 1, 0, 0 );
	myGump.AddHTMLGump( 65, y + 10, 100, 20, 0, 0, "Close" );
	myGump.Send( pSock );
	myGump.Free();
	return true;
}

function CommandShowFactionRanksGump( pSock, pUser )
{
	if( !pSock || !ValidateObject( pUser ) )
		return false;

	const rankData = TriggerEvent( commandFactionPlayerDataScriptId, "ReadFactionPlayerData", pUser );
	const playerFaction = rankData.faction;
	let currentRank = parseInt( rankData.rank, 10 );
	let currentPoints = parseInt( rankData.killPoints, 10 );
	if( isNaN( currentRank ) )
		currentRank = -1;
	if( isNaN( currentPoints ) )
		currentPoints = 0;

	const myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 430, 360, 9200 );
	myGump.AddHTMLGump( 20, 15, 390, 25, 0, 0, "<CENTER><b>Faction Ranks</b></CENTER>" );

	if( CommandIsFactionValid( playerFaction ) )
		myGump.AddHTMLGump( 25, 45, 380, 20, 0, 0, CommandFactionUsageName( playerFaction ) + ": " + CommandRankName( currentRank ) + " (" + currentPoints + " kill points)" );
	else
		myGump.AddHTMLGump( 25, 45, 380, 20, 0, 0, "You are not currently enlisted in a faction." );

	let y = 75;
	for( let rankIndex = 0; rankIndex < 10; rankIndex++ )
	{
		const prefix = rankIndex === currentRank ? "> " : "";
		myGump.AddHTMLGump( 35, y, 360, 20, 0, 0, prefix + CommandRankName( rankIndex ) + ": " + CommandRankPoints( rankIndex ) + " kill points" );
		y += 24;
	}

	myGump.AddButton( 25, 325, 0xFA5, 1, 0, 0 );
	myGump.AddHTMLGump( 65, 325, 100, 20, 0, 0, "Close" );
	myGump.Send( pSock );
	myGump.Free();
	return true;
}

function CommandCollectFactionLeaderboard( factionKey )
{
	commandLeaderboardFaction = factionKey;
	commandLeaderboardEntries = [];
	IterateOver( "CHARACTER" );
	commandLeaderboardFaction = "";

	commandLeaderboardEntries.sort(function( leftEntry, rightEntry )
	{
		if( rightEntry.killPoints !== leftEntry.killPoints )
			return rightEntry.killPoints - leftEntry.killPoints;
		if( rightEntry.rank !== leftEntry.rank )
			return rightEntry.rank - leftEntry.rank;
		if( rightEntry.captures !== leftEntry.captures )
			return rightEntry.captures - leftEntry.captures;
		return leftEntry.name.toLowerCase() > rightEntry.name.toLowerCase() ? 1 : -1;
	});

	return commandLeaderboardEntries;
}

function CommandShowFactionLeaderboardGump( pSock, pUser, factionKey )
{
	if( !pSock || !ValidateObject( pUser ) || !CommandIsFactionValid( factionKey ) )
		return false;

	const entries = CommandCollectFactionLeaderboard( factionKey );
	const myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 520, 390, 9200 );
	myGump.AddHTMLGump( 20, 15, 480, 25, 0, 0, "<CENTER><b>Faction Leaderboard</b></CENTER>" );
	myGump.AddHTMLGump( 25, 45, 470, 20, 0, 0, CommandFactionUsageName( factionKey ) + " Members" );
	myGump.AddHTMLGump( 35, 75, 30, 20, 0, 0, "#" );
	myGump.AddHTMLGump( 70, 75, 175, 20, 0, 0, "Name" );
	myGump.AddHTMLGump( 250, 75, 105, 20, 0, 0, "Rank" );
	myGump.AddHTMLGump( 360, 75, 55, 20, 0, 0, "KP" );
	myGump.AddHTMLGump( 420, 75, 70, 20, 0, 0, "Caps" );

	let y = 100;
	let maxRows = entries.length;
	if( maxRows > 10 )
		maxRows = 10;

	if( maxRows === 0 )
	{
		myGump.AddHTMLGump( 35, y, 450, 25, 0, 0, "No faction members found." );
	}
	else
	{
		for( let entryIndex = 0; entryIndex < maxRows; entryIndex++ )
		{
			const entry = entries[entryIndex];
			const onlineMark = entry.online ? "*" : "";
			myGump.AddHTMLGump( 35, y, 30, 20, 0, 0, String( entryIndex + 1 ) );
			myGump.AddHTMLGump( 70, y, 175, 20, 0, 0, onlineMark + entry.name );
			myGump.AddHTMLGump( 250, y, 105, 20, 0, 0, CommandRankName( entry.rank ) );
			myGump.AddHTMLGump( 360, y, 55, 20, 0, 0, String( entry.killPoints ) );
			myGump.AddHTMLGump( 420, y, 70, 20, 0, 0, String( entry.captures ) );
			y += 24;
		}
	}

	myGump.AddHTMLGump( 35, 345, 250, 20, 0, 0, "* online" );
	myGump.AddButton( 390, 345, 0xFA5, 1, 0, 0 );
	myGump.AddHTMLGump( 430, 345, 70, 20, 0, 0, "Close" );
	myGump.Send( pSock );
	myGump.Free();
	return true;
}

function CommandHealthScanItems()
{
	commandHealthControllerCount = 0;
	commandHealthFactionStoneCount = 0;
	commandHealthJoinStoneCount = 0;
	commandHealthSigilCount = 0;
	commandHealthSigilHomeCount = 0;
	commandHealthTownstoneCount = 0;
	commandHealthFactionNpcCounts = { TB: 0, COM: 0, MIN: 0, SL: 0 };
	commandHealthScanActive = true;
	IterateOver( "ITEM" );
	IterateOver( "CHARACTER" );
	commandHealthScanActive = false;
}

function CommandHealthFactionNpcSummary()
{
	return "Faction NPCs: TB " + commandHealthFactionNpcCounts.TB + ", COM " + commandHealthFactionNpcCounts.COM + ", MIN " + commandHealthFactionNpcCounts.MIN + ", SL " + commandHealthFactionNpcCounts.SL;
}

function CommandDedupeItemKey( iItem )
{
	if( !ValidateObject( iItem ) || !iItem.isItem )
		return "";

	if( iItem.GetTag( "faction_controller" ) == 1 )
		return "controller";

	if( iItem.GetTag( "faction_stone" ) == 1 )
	{
		const factionStoneFaction = iItem.GetTag( "stone_faction" );
		if( CommandIsFactionValid( factionStoneFaction ) )
			return "faction_stone_" + factionStoneFaction;
		return "";
	}

	if( iItem.GetTag( "join_stone" ) == 1 )
	{
		const joinStoneFaction = iItem.GetTag( "stone_faction" );
		if( CommandIsFactionValid( joinStoneFaction ) )
			return "join_stone_" + joinStoneFaction;
		return "";
	}

	if( iItem.GetTag( "sigil" ) == 1 )
	{
		const sigilTown = iItem.GetTag( "sigil_town" );
		if( sigilTown !== "" && sigilTown != 0 )
			return "sigil_" + sigilTown;
		return "";
	}

	if( iItem.GetTag( "faction_townstone" ) == 1 )
	{
		const factionTownstoneTown = iItem.GetTag( "faction_town" );
		if( factionTownstoneTown !== "" && factionTownstoneTown != 0 )
			return "townstone_" + factionTownstoneTown;
		return "";
	}

	return "";
}

function CommandDedupeItemRank( iItem )
{
	if( !ValidateObject( iItem ) || !iItem.isItem )
		return 0;

	let rankScore = 0;

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
	commandDedupeKept = {};
	commandDedupeRanks = {};
	commandDedupeRemovedCount = 0;
	commandDedupeActive = true;
	IterateOver( "ITEM" );
	commandDedupeActive = false;
	commandDedupeKept = {};
	commandDedupeRanks = {};
	return commandDedupeRemovedCount;
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
	if( iItem.GetTag( "faction_townstone" ) == 1 )
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
	commandRemoveFactionsItems++;
	return true;
}

function CommandClearFactionPlayerTags( pChar )
{
	if( !ValidateObject( pChar ) || !pChar.isChar || pChar.npc )
		return false;

	let hadFactionData = TriggerEvent( commandFactionPlayerDataScriptId, "ClearFactionPlayerData", pChar );
	const factionTags = [
		"faction",
		"faction_join_time",
		"faction_kp",
		"faction_silver",
		"faction_rank",
		"faction_captures",
		"faction_leave_time",
		"faction_commander",
		"faction_role",
		"faction_role_faction",
		"faction_role_town",
		"faction_role_set_at",
		"faction_kp_decay_time",
		"elec_voted_TB",
		"elec_voted_COM",
		"elec_voted_MIN",
		"elec_voted_SL"
	];

	for( let tagIndex = 0; tagIndex < factionTags.length; tagIndex++ )
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

	if( pChar.HasScriptTrigger( commandFactionCombatScriptId ) )
		pChar.RemoveScriptTrigger( commandFactionCombatScriptId );

	if( hadFactionData )
		commandRemoveFactionsPlayers++;

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

function CommandClearGenericFactionNpcTags( pChar )
{
	if( !ValidateObject( pChar ) || !pChar.isChar || !pChar.npc )
		return false;

	let hadFactionData = false;
	const factionTags = [
		"npc_faction",
		"faction_npc",
		"npc_faction_passive",
		"npc_faction_require_town_control",
		"npc_faction_no_reward",
		"npc_faction_kp",
		"npc_faction_silver"
	];

	for( let tagIndex = 0; tagIndex < factionTags.length; tagIndex++ )
	{
		if( pChar.GetTag( factionTags[tagIndex] ) !== "" && pChar.GetTag( factionTags[tagIndex] ) != 0 )
			hadFactionData = true;
		pChar.SetTag( factionTags[tagIndex], null );
	}

	if( pChar.GetTag( "faction_spawned" ) != 1 && pChar.GetTag( "faction_town" ) === "" && CommandIsFactionValid( pChar.GetTag( "faction" ) ) )
	{
		hadFactionData = true;
		pChar.SetTag( "faction", null );
	}

	if( pChar.HasScriptTrigger( commandFactionNpcScriptId ) )
	{
		pChar.RemoveScriptTrigger( commandFactionNpcScriptId );
		hadFactionData = true;
	}

	return hadFactionData;
}

function CommandRemoveFactionChar( pChar )
{
	if( !ValidateObject( pChar ) || !pChar.isChar )
		return false;

	if( pChar.npc )
	{
		if( !CommandIsFactionNpc( pChar ) )
		{
			if( CommandClearGenericFactionNpcTags( pChar ) )
			{
				commandRemoveFactionsNpcs++;
				return true;
			}

			return false;
		}

		const ownerChar = pChar.owner;
		if( ValidateObject( ownerChar ) && ownerChar.isChar && !ownerChar.npc )
		{
			ownerChar.RemoveFollower( pChar );
			if( ownerChar.controlSlotsUsed >= pChar.controlSlots )
				ownerChar.controlSlotsUsed = ownerChar.controlSlotsUsed - pChar.controlSlots;
			else
				ownerChar.controlSlotsUsed = 0;
		}

		pChar.Delete();
		commandRemoveFactionsNpcs++;
		return true;
	}

	CommandClearFactionPlayerTags( pChar );
	return false;
}

function CommandResetFactionTownRegions()
{
	let resetCount = 0;
	for( let regionIndex = 0; regionIndex < commandFactionRegionChecks.length; regionIndex++ )
	{
		const regionInfo = commandFactionRegionChecks[regionIndex];
		const townRegion = GetTownRegion( regionInfo[1] );
		if( townRegion != null && typeof townRegion.id != "undefined" )
		{
			townRegion.owner = "The Town";
			townRegion.isGuarded = true;
			resetCount++;
		}
	}

	commandRemoveFactionsRegions = resetCount;
	return resetCount;
}

function CommandRemoveFactionsFromShard()
{
	commandRemoveFactionsItems = 0;
	commandRemoveFactionsNpcs = 0;
	commandRemoveFactionsPlayers = 0;
	commandRemoveFactionsRegions = 0;

	CommandResetFactionTownRegions();

	commandRemoveFactionsMode = "chars";
	IterateOver( "CHARACTER" );
	commandRemoveFactionsMode = "items";
	IterateOver( "ITEM" );
	commandRemoveFactionsMode = "";

	return commandRemoveFactionsItems + commandRemoveFactionsNpcs + commandRemoveFactionsPlayers;
}

function CommandHealthLine( ok, label, value )
{
	return ( ok ? "[OK] " : "[WARN] " ) + label + ": " + value;
}

function CommandHealthRegionSummary()
{
	let loadedCount = 0;
	let missingText = "";

	for( let regionIndex = 0; regionIndex < commandFactionRegionChecks.length; regionIndex++ )
	{
		const regionInfo = commandFactionRegionChecks[regionIndex];
		const townRegion = GetTownRegion( regionInfo[1] );
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
		total: commandFactionRegionChecks.length,
		missing: missingText
	};
}

function CommandHealthStrongholdSummary()
{
	const factionKeys = [ "TB", "COM", "MIN", "SL" ];
	let configuredCount = 0;
	let missingText = "";

	for( let factionIndex = 0; factionIndex < factionKeys.length; factionIndex++ )
	{
		let factionKey = factionKeys[factionIndex];
		if( TriggerEvent( commandFactionStrongholdScriptId, "StrongholdIsConfigured", factionKey ) )
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
	const regionSummary = CommandHealthRegionSummary();
	const strongholdSummary = CommandHealthStrongholdSummary();
	let y = 50;

	const myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 560, 525, 9200 );
	myGump.AddHTMLGump( 20, 15, 520, 25, 0, 0, "<CENTER><b>Faction Health</b></CENTER>" );

	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, CommandHealthLine( commandHealthControllerCount === 1, "Faction controllers", commandHealthControllerCount + " found, expected 1" ) );
	y += 25;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, CommandHealthLine( commandHealthFactionStoneCount === 4, "Faction stones", commandHealthFactionStoneCount + " found, expected 4" ) );
	y += 25;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, CommandHealthLine( commandHealthJoinStoneCount === 4, "Join stones", commandHealthJoinStoneCount + " found, expected 4" ) );
	y += 25;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, CommandHealthLine( commandHealthSigilCount === 8, "Sigils", commandHealthSigilCount + " found, expected 8" ) );
	y += 25;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, CommandHealthLine( commandHealthSigilHomeCount === 8, "Sigil homes", commandHealthSigilHomeCount + " set, expected 8" ) );
	y += 25;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, CommandHealthLine( commandHealthTownstoneCount === 8, "Townstones", commandHealthTownstoneCount + " found, expected 8" ) );
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

	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, TriggerEvent( commandFactionTownScriptId, "TownTaxStatusText" ) );
	y += 30;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, CommandHealthFactionNpcSummary() );
	y += 30;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, TriggerEvent( commandFactionSigilScriptId, "FactionScoreText", "TB" ) );
	y += 22;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, TriggerEvent( commandFactionSigilScriptId, "FactionScoreText", "COM" ) );
	y += 22;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, TriggerEvent( commandFactionSigilScriptId, "FactionScoreText", "MIN" ) );
	y += 22;
	myGump.AddHTMLGump( 25, y, 510, 20, 0, 0, TriggerEvent( commandFactionSigilScriptId, "FactionScoreText", "SL" ) );
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
	const regionSummary = CommandHealthRegionSummary();
	const strongholdSummary = CommandHealthStrongholdSummary();

	const myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 520, 455, 9200 );
	myGump.AddHTMLGump( 20, 15, 480, 25, 0, 0, "<CENTER><b>Faction Admin</b></CENTER>" );
	myGump.AddHTMLGump( 25, 50, 470, 20, 0, 0, "Setup: controllers " + commandHealthControllerCount + "/1, stones " + commandHealthFactionStoneCount + "/4, join stones " + commandHealthJoinStoneCount + "/4, sigils " + commandHealthSigilCount + "/8" );
	myGump.AddHTMLGump( 25, 75, 470, 20, 0, 0, "Regions: " + regionSummary.loaded + "/" + regionSummary.total + ", Strongholds: " + strongholdSummary.configured + "/" + strongholdSummary.total + ", Sigil homes: " + commandHealthSigilHomeCount + "/8, townstones " + commandHealthTownstoneCount + "/8" );
	myGump.AddHTMLGump( 25, 100, 470, 20, 0, 0, TriggerEvent( commandFactionTownScriptId, "TownTaxStatusText" ) );
	myGump.AddHTMLGump( 25, 125, 470, 20, 0, 0, CommandHealthFactionNpcSummary() );

	CommandAddAdminButton( myGump, 30, 155, commandFactionAdminHealthButton, "Health Gump" );
	CommandAddAdminButton( myGump, 30, 185, commandFactionAdminSetupButton, "Safe Setup" );
	CommandAddAdminButton( myGump, 30, 215, commandFactionAdminDedupeButton, "Dedupe Setup" );
	CommandAddAdminButton( myGump, 30, 245, commandFactionAdminSyncTownsButton, "Sync Towns" );
	CommandAddAdminButton( myGump, 30, 275, commandFactionAdminTownStatusButton, "Town Status" );
	CommandAddAdminButton( myGump, 30, 305, commandFactionAdminTreasuryButton, "Treasury" );

	CommandAddAdminButton( myGump, 275, 155, commandFactionAdminTaxStartButton, "Start Tax Timer" );
	CommandAddAdminButton( myGump, 275, 185, commandFactionAdminTaxStopButton, "Stop Tax Timer" );
	CommandAddAdminButton( myGump, 275, 215, commandFactionAdminTaxStatusButton, "Tax Status" );
	CommandAddAdminButton( myGump, 275, 245, commandFactionAdminSigilsButton, "Sigils" );
	CommandAddAdminButton( myGump, 275, 275, commandFactionAdminStrongholdsButton, "Strongholds" );
	CommandAddAdminButton( myGump, 275, 335, commandFactionAdminRemoveConfirmButton, "Remove Factions" );

	myGump.AddButton( 30, 405, 0xFA5, 0xFA7, 1, 0, 0 );
	myGump.AddHTMLGump( 70, 405, 100, 20, 0, 0, "Close" );
	myGump.Send( pSock );
	myGump.Free();
	return true;
}

function CommandShowRemoveFactionsConfirmGump( pSock, pUser )
{
	if( !pSock || !ValidateObject( pUser ) )
		return false;

	const myGump = new Gump();
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 430, 230, 9200 );
	myGump.AddHTMLGump( 20, 15, 390, 25, 0, 0, "<CENTER><b>Remove Factions?</b></CENTER>" );
	myGump.AddHTMLGump( 25, 55, 380, 70, 0, 0, "This deletes faction setup items, faction items, faction NPCs, faction mounts, traps, and clears player faction tags." );
	myGump.AddHTMLGump( 25, 130, 380, 25, 0, 0, "Use this only when resetting the faction test install." );
	myGump.AddButton( 45, 175, 0xFA5, 0xFA7, 1, 0, commandFactionAdminBackButton );
	myGump.AddHTMLGump( 85, 175, 100, 20, 0, 0, "Cancel" );
	myGump.AddButton( 240, 175, 0xFA5, 0xFA7, 1, 0, commandFactionAdminRemoveNowButton );
	myGump.AddHTMLGump( 280, 175, 120, 20, 0, 0, "Remove Now" );
	myGump.Send( pSock );
	myGump.Free();
	return true;
}

function command_FACTIONSTATUS( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let factionKey = pUser.GetTag( "faction" );
	const requestedFaction = String( cmdString ).replace( /^\s+|\s+$/g, "" ).toUpperCase();
	if( CommandIsStaff( pUser ) && CommandIsFactionValid( requestedFaction ) )
		factionKey = requestedFaction;

	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25153, pSock.language ) );
		return;
	}

	CommandShowFactionStatusGump( pSock, pUser, factionKey );
}

function command_FACTIONRANKS( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	CommandShowFactionRanksGump( pSock, pUser );
}

function command_FACTIONLEADERBOARD( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	const requestedFaction = String( cmdString ).replace( /^\s+|\s+$/g, "" ).toUpperCase();
	let factionKey = pUser.GetTag( "faction" );
	if( CommandIsFactionValid( requestedFaction ) )
		factionKey = requestedFaction;

	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25154, pSock.language ) );
		return;
	}

	CommandShowFactionLeaderboardGump( pSock, pUser, factionKey );
}

function command_FACTIONLEADERS( pSock, cmdString )
{
	command_FACTIONLEADERBOARD( pSock, cmdString );
}

function command_FACTIONADMIN( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	CommandShowFactionAdminGump( pSock, pUser );
}

function command_FACTIONHEALTH( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	CommandShowFactionHealthGump( pSock, pUser );
}

function command_FACTIONDEDUPE( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	const removedCount = CommandDedupeFactionSetup();
	pSock.SysMessage( GetDictionaryEntry( 25155, pSock.language ).replace( /%s/, String( removedCount ) ) );
	CommandShowFactionHealthGump( pSock, pUser );
}

function command_FACTIONDATAMIGRATE( pSock, cmdString )
{
	commandDataMigrateActive = true;
	commandDataMigrateCount = 0;
	IterateOver( "CHARACTER" );
	commandDataMigrateActive = false;
	pSock.SysMessage( GetDictionaryEntry( 25156, pSock.language ).replace( /%s/, String( commandDataMigrateCount ) ) );
}

function command_REMOVEFACTIONS( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	CommandRemoveFactionsFromShard();
	pSock.SysMessage( GetDictionaryEntry( 25157, pSock.language ) );
	pSock.SysMessage( GetDictionaryEntry( 25158, pSock.language ).replace( /%s/, String( commandRemoveFactionsItems ) ).replace( /%s/, String( commandRemoveFactionsNpcs ) ).replace( /%s/, String( commandRemoveFactionsPlayers ) ).replace( /%s/, String( commandRemoveFactionsRegions ) ) );
}

function onGumpPress( pSock, buttonID, gumpData )
{
	if( buttonID == 0 )
		return;

	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	if( buttonID == commandFactionAdminHealthButton )
	{
		CommandShowFactionHealthGump( pSock, pUser );
		return;
	}

	if( buttonID == commandFactionAdminSetupButton )
	{
		command_FACTIONSETUP( pSock, "" );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == commandFactionAdminDedupeButton )
	{
		const removedCount = CommandDedupeFactionSetup();
		pSock.SysMessage( GetDictionaryEntry( 25155, pSock.language ).replace( /%s/, String( removedCount ) ) );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == commandFactionAdminSyncTownsButton )
	{
		command_FACTIONTOWNSYNC( pSock, "" );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == commandFactionAdminTownStatusButton )
	{
		TriggerEvent( commandFactionTownScriptId, "ShowTownStatus", pSock );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == commandFactionAdminTreasuryButton )
	{
		TriggerEvent( commandFactionTownScriptId, "ShowTownTreasury", pSock, "" );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == commandFactionAdminTaxStartButton )
	{
		if( TriggerEvent( commandFactionTownScriptId, "StartTownTaxTimer", 60 ) )
			pSock.SysMessage( GetDictionaryEntry( 25159, pSock.language ) );
		else
			pSock.SysMessage( GetDictionaryEntry( 25160, pSock.language ) );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == commandFactionAdminTaxStopButton )
	{
		command_FACTIONTAXSTOP( pSock, "" );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == commandFactionAdminTaxStatusButton )
	{
		TriggerEvent( commandFactionTownScriptId, "ShowTownTaxStatus", pSock );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == commandFactionAdminSigilsButton )
	{
		TriggerEvent( commandFactionSigilScriptId, "ShowSigilStatus", pSock, "" );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == commandFactionAdminStrongholdsButton )
	{
		TriggerEvent( commandFactionStrongholdScriptId, "ShowStrongholdStatus", pSock );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == commandFactionAdminRemoveConfirmButton )
	{
		CommandShowRemoveFactionsConfirmGump( pSock, pUser );
		return;
	}

	if( buttonID == commandFactionAdminRemoveNowButton )
	{
		command_REMOVEFACTIONS( pSock, "" );
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}

	if( buttonID == commandFactionAdminBackButton )
	{
		CommandShowFactionAdminGump( pSock, pUser );
		return;
	}
}

function command_FACTIONSTRONGHOLD( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	cmdString = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	let parts = cmdString.length > 0 ? cmdString.split( /\s+/ ) : [];
	if( parts.length < 1 )
	{
		pSock.SysMessage( GetDictionaryEntry( 25161, pSock.language ) );
		return;
	}

	let factionKey = parts[0].toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25161, pSock.language ) );
		return;
	}

	let range = 12;
	if( parts.length > 1 )
	{
		range = parseInt( parts[1], 10 );
		if( isNaN( range ) )
		{
			pSock.SysMessage( GetDictionaryEntry( 25162, pSock.language ) );
			return;
		}
	}

	if( TriggerEvent( commandFactionStrongholdScriptId, "StrongholdSet", factionKey, pUser, range ) )
		pSock.SysMessage( GetDictionaryEntry( 25163, pSock.language ).replace( /%s/, String( CommandFactionUsageName( factionKey ) ) ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25164, pSock.language ).replace( /%s/, String( TriggerEvent( commandFactionStrongholdScriptId, "StrongholdLastError" ) ) ) );
}

function command_FACTIONSTRONGHOLDS( pSock, cmdString )
{
	if( !TriggerEvent( commandFactionStrongholdScriptId, "ShowStrongholdStatus", pSock ) )
		pSock.SysMessage( GetDictionaryEntry( 25165, pSock.language ) );
}

function CommandSigilTownArg( cmdString )
{
	let townName = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	if( townName.toLowerCase() === "all" )
		return "";

	return townName;
}

function command_FACTIONSIGILHOME( pSock, cmdString )
{
	let townName = CommandSigilTownArg( cmdString );
	let count = TriggerEvent( commandFactionSigilScriptId, "SigilRegisterHome", townName );
	if( count > 0 )
		pSock.SysMessage( GetDictionaryEntry( 25166, pSock.language ).replace( /%s/, String( count ) ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25167, pSock.language ) );
}

function command_FACTIONSIGILRETURN( pSock, cmdString )
{
	let townName = CommandSigilTownArg( cmdString );
	let count = TriggerEvent( commandFactionSigilScriptId, "SigilReturn", townName );
	if( count > 0 )
		pSock.SysMessage( GetDictionaryEntry( 25168, pSock.language ).replace( /%s/, String( count ) ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25169, pSock.language ) );
}

function command_FACTIONSIGILS( pSock, cmdString )
{
	let townName = CommandSigilTownArg( cmdString );
	let count = TriggerEvent( commandFactionSigilScriptId, "ShowSigilStatus", pSock, townName );
	if( count == 0 )
		pSock.SysMessage( GetDictionaryEntry( 25170, pSock.language ) );
}

function command_FACTIONSIGILRETURNTIME( pSock, cmdString )
{
	let cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	if( cmdText === "" )
	{
		TriggerEvent( commandFactionSigilScriptId, "ShowSigilReturnTime", pSock );
		return;
	}

	let minutes = parseInt( cmdText, 10 );
	if( isNaN( minutes ) || minutes < 1 )
	{
		pSock.SysMessage( GetDictionaryEntry( 25171, pSock.language ) );
		return;
	}

	let count = TriggerEvent( commandFactionSigilScriptId, "SigilSetReturnTime", minutes );
	if( count > 0 )
		pSock.SysMessage( GetDictionaryEntry( 25172, pSock.language ).replace( /%s/, String( minutes ) ).replace( /%s/, String( count ) ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25173, pSock.language ) );
}

function command_FACTIONSCORE( pSock, cmdString )
{
	if( !TriggerEvent( commandFactionSigilScriptId, "ShowFactionScore", pSock ) )
		pSock.SysMessage( GetDictionaryEntry( 25174, pSock.language ) );
}

function command_FACTIONSCORERESET( pSock, cmdString )
{
	if( TriggerEvent( commandFactionSigilScriptId, "ResetFactionScore" ) )
		pSock.SysMessage( GetDictionaryEntry( 25175, pSock.language ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25176, pSock.language ) );
}

function command_FACTIONKILLS( pSock, cmdString )
{
	if( !TriggerEvent( commandFactionCombatScriptId, "ShowFactionKillStats", pSock ) )
		pSock.SysMessage( GetDictionaryEntry( 25177, pSock.language ) );
}

function command_FACTIONKILLSRESET( pSock, cmdString )
{
	if( TriggerEvent( commandFactionCombatScriptId, "ResetFactionKillStats" ) )
		pSock.SysMessage( GetDictionaryEntry( 25178, pSock.language ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25179, pSock.language ) );
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
		pSock.SysMessage( GetDictionaryEntry( 25105, pSock.language ) );
		return false;
	}

	if( CommandIsStaff( pUser ) )
		return true;
	if( CommandHasFactionRole( pUser, "commander", factionKey ) )
		return true;

	pSock.SysMessage( GetDictionaryEntry( 25180, pSock.language ) );
	return false;
}

function CommandBroadcastToFaction( factionKey, messageText )
{
	commandBroadcastFaction = factionKey;
	commandBroadcastMessage = messageText;
	commandBroadcastCount = 0;
	IterateOver( "CHARACTER" );
	const sentCount = commandBroadcastCount;
	commandBroadcastFaction = "";
	commandBroadcastMessage = "";
	commandBroadcastCount = 0;
	return sentCount;
}

function command_FACTIONNOTICE( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	let parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	let factionKey = CommandFactionFromCommanderArgs( pUser, parts );
	if( !CommandRequireCommander( pSock, pUser, factionKey ) )
		return;

	if( CommandIsStaff( pUser ) && parts.length > 0 && CommandIsFactionValid( String( parts[0] ).toUpperCase() ) )
		parts.shift();

	let messageText = parts.join( " " ).replace( /^\s+|\s+$/g, "" );
	if( messageText === "" )
	{
		pSock.SysMessage( GetDictionaryEntry( 25181, pSock.language ) );
		return;
	}

	if( TriggerEvent( commandFactionSigilScriptId, "SetFactionNotice", factionKey, messageText, pUser.name ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25182, pSock.language ) );
		CommandBroadcastToFaction( factionKey, CommandFactionUsageName( factionKey ) + " notice: " + messageText );
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 25183, pSock.language ) );
	}
}

function command_FACTIONNOTICECLEAR( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let parts = String( cmdString ).replace( /^\s+|\s+$/g, "" ).split( /\s+/ );
	if( parts.length == 1 && parts[0] === "" )
		parts = [];

	let factionKey = CommandFactionFromCommanderArgs( pUser, parts );
	if( !CommandRequireCommander( pSock, pUser, factionKey ) )
		return;

	if( TriggerEvent( commandFactionSigilScriptId, "ClearFactionNotice", factionKey ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25184, pSock.language ) );
		CommandBroadcastToFaction( factionKey, CommandFactionUsageName( factionKey ) + " notice cleared." );
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 25185, pSock.language ) );
	}
}

function command_FACTIONALERT( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	let parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	let factionKey = CommandFactionFromCommanderArgs( pUser, parts );
	if( !CommandRequireCommander( pSock, pUser, factionKey ) )
		return;

	if( CommandIsStaff( pUser ) && parts.length > 0 && CommandIsFactionValid( String( parts[0] ).toUpperCase() ) )
		parts.shift();

	let townName = parts.join( " " ).replace( /^\s+|\s+$/g, "" );
	if( townName === "" )
		townName = CommandCurrentFactionTown( pUser );
	if( townName === "" )
	{
		pSock.SysMessage( GetDictionaryEntry( 25186, pSock.language ) );
		return;
	}

	let townOwner = TriggerEvent( commandFactionTownScriptId, "TownGetOwner", townName );
	if( townOwner === "" )
	{
		pSock.SysMessage( GetDictionaryEntry( 25187, pSock.language ) );
		return;
	}

	if( townOwner !== "" && townOwner !== factionKey && !CommandIsStaff( pUser ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25188, pSock.language ) );
		return;
	}

	if( !CommandIsStaff( pUser ) )
	{
		if( TriggerEvent( commandFactionSigilScriptId, "FactionScoreValue", factionKey ) < 1 )
		{
			pSock.SysMessage( GetDictionaryEntry( 25189, pSock.language ) );
			return;
		}

		if( !TriggerEvent( commandFactionSigilScriptId, "SpendFactionScore", factionKey, 1 ) )
		{
			pSock.SysMessage( GetDictionaryEntry( 25190, pSock.language ) );
			return;
		}
	}

	const sentCount = CommandBroadcastToFaction( factionKey, "Faction alert: " + townName + " needs " + CommandFactionUsageName( factionKey ) + " forces." );
	pSock.SysMessage( GetDictionaryEntry( 25191, pSock.language ).replace( /%s/, String( sentCount ) ) );
}

function CommandCurrentFactionTown( pUser )
{
	return TriggerEvent( commandFactionTownScriptId, "TownNameForObject", pUser );
}

function CommandCanPlaceFactionNpc( pSock, pUser, factionKey )
{
	let townOwner = TriggerEvent( commandFactionTownScriptId, "TownOwnerForObject", pUser );
	if( townOwner === "" )
	{
		pSock.SysMessage( GetDictionaryEntry( 25192, pSock.language ) );
		return false;
	}

	if( townOwner !== "" && townOwner !== factionKey )
	{
		pSock.SysMessage( GetDictionaryEntry( 25193, pSock.language ).replace( /%s/, String( CommandFactionUsageName( townOwner ) ) ) );
		return false;
	}

	return true;
}

function CommandCanPlaceFactionNpcType( pSock, pUser, factionKey, npcType )
{
	let townName = CommandCurrentFactionTown( pUser );
	const limitError = TriggerEvent( commandFactionTownScriptId, "TownCanPlaceFactionNpc", townName, factionKey, npcType );
	if( limitError !== "" )
	{
		pSock.SysMessage( limitError );
		return false;
	}

	return true;
}

function CommandSpendTownTreasury( pSock, pUser, townName, cost, spendText )
{
	if( CommandIsStaff( pUser ) )
		return true;

	const treasury = TriggerEvent( commandFactionTownScriptId, "TownGetTreasury", townName );
	if( treasury < cost )
	{
		pSock.SysMessage( GetDictionaryEntry( 25194, pSock.language ).replace( /%s/, String( townName ) ).replace( /%s/, String( cost ) ).replace( /%s/, String( spendText ) ).replace( /%s/, String( treasury ) ) );
		return false;
	}

	if( !TriggerEvent( commandFactionTownScriptId, "TownSpendTreasury", townName, cost ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25195, pSock.language ) );
		return false;
	}

	pSock.SysMessage( GetDictionaryEntry( 25196, pSock.language ).replace( /%s/, String( cost ) ).replace( /%s/, String( townName ) ) );
	return true;
}

function CommandCanUseFactionNpcCommand( pSock, pUser, factionKey, requiredRole )
{
	if( CommandIsStaff( pUser ) )
		return true;

	if( CommandHasFactionRole( pUser, requiredRole, factionKey ) )
		return true;

	pSock.SysMessage( GetDictionaryEntry( 25197, pSock.language ).replace( /%s/, String( CommandRoleDisplayName( requiredRole ) ) ).replace( /%s/, String( CommandFactionUsageName( factionKey ) ) ) );
	return false;
}

function CommandSpawnFactionNpc( pSock, pUser, sectionId, factionKey, npcType, vendorType )
{
	const newNpc = SpawnNPC( sectionId, pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID );
	if( !ValidateObject( newNpc ) )
		return null;

	let townName = CommandCurrentFactionTown( pUser );
	if( !TriggerEvent( commandFactionTownScriptId, "TownTagFactionNpc", newNpc, factionKey, townName, npcType, vendorType ) )
		newNpc.SetTag( "faction_spawned", 1 );

	return newNpc;
}

function CommandTownNameFromArgs( pSock, pUser, cmdString, allowAll, requireTown )
{
	let townName = "";
	if( cmdString != null )
		townName = String( cmdString ).replace( /^\s+|\s+$/g, "" );

	if( allowAll && townName.toLowerCase() === "all" )
		return "";

	if( townName === "" )
		townName = CommandCurrentFactionTown( pUser );

	if( townName === "" && requireTown )
	{
		pSock.SysMessage( GetDictionaryEntry( 25198, pSock.language ) );
		return null;
	}

	return townName;
}

function command_FACTIONTOWNNPCS( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let townName = CommandTownNameFromArgs( pSock, pUser, cmdString, true, false );
	TriggerEvent( commandFactionTownScriptId, "ShowTownNpcStatus", pSock, townName );
}

function command_FACTIONTOWNCLEAR( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let townName = CommandTownNameFromArgs( pSock, pUser, cmdString, true, true );
	if( townName == null )
		return;

	const removedCount = TriggerEvent( commandFactionTownScriptId, "TownClearFactionNpcs", townName, "" );
	pSock.SysMessage( GetDictionaryEntry( 25199, pSock.language ).replace( /%s/, String( removedCount ) ) );
}

function command_FACTIONTOWNLIMIT( pSock, cmdString )
{
	let cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	let parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	if( parts.length < 2 )
	{
		pSock.SysMessage( GetDictionaryEntry( 25200, pSock.language ) );
		return;
	}

	const npcType = parts[0].toLowerCase();
	if( npcType !== "guard" && npcType !== "vendor" )
	{
		pSock.SysMessage( GetDictionaryEntry( 25200, pSock.language ) );
		return;
	}

	let amountText = parts[1].toLowerCase();
	if( amountText === "default" || amountText === "ini" || amountText === "clear" )
	{
		if( TriggerEvent( commandFactionTownScriptId, "TownClearNpcLimit", npcType ) )
			pSock.SysMessage( GetDictionaryEntry( 25201, pSock.language ).replace( /%s/, String( npcType ) ) );
		else
			pSock.SysMessage( GetDictionaryEntry( 25202, pSock.language ) );
		return;
	}

	let amount = parseInt( amountText, 10 );
	if( isNaN( amount ) || amount < 0 )
	{
		pSock.SysMessage( GetDictionaryEntry( 25200, pSock.language ) );
		return;
	}

	if( TriggerEvent( commandFactionTownScriptId, "TownSetNpcLimit", npcType, amount ) )
		pSock.SysMessage( GetDictionaryEntry( 25203, pSock.language ).replace( /%s/, String( npcType ) ).replace( /%s/, String( amount ) ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25204, pSock.language ) );
}

function command_FACTIONTREASURY( pSock, cmdString )
{
	let townName = CommandSigilTownArg( cmdString );
	if( !TriggerEvent( commandFactionTownScriptId, "ShowTownTreasury", pSock, townName ) )
		pSock.SysMessage( GetDictionaryEntry( 25205, pSock.language ) );
}

function command_FACTIONTREASURYGRANT( pSock, cmdString )
{
	let cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	let parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	if( parts.length < 2 )
	{
		pSock.SysMessage( GetDictionaryEntry( 25206, pSock.language ) );
		return;
	}

	let amount = parseInt( parts[parts.length - 1], 10 );
	if( isNaN( amount ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25206, pSock.language ) );
		return;
	}

	parts.pop();
	let townName = parts.join( " " );
	if( TriggerEvent( commandFactionTownScriptId, "TownAddTreasury", townName, amount ) )
		pSock.SysMessage( GetDictionaryEntry( 25207, pSock.language ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25208, pSock.language ) );
}

function command_FACTIONTREASURYSET( pSock, cmdString )
{
	let cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	let parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	if( parts.length < 2 )
	{
		pSock.SysMessage( GetDictionaryEntry( 25209, pSock.language ) );
		return;
	}

	let amount = parseInt( parts[parts.length - 1], 10 );
	if( isNaN( amount ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25209, pSock.language ) );
		return;
	}

	parts.pop();
	let townName = parts.join( " " );
	if( TriggerEvent( commandFactionTownScriptId, "TownSetTreasury", townName, amount ) )
		pSock.SysMessage( GetDictionaryEntry( 25210, pSock.language ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25211, pSock.language ) );
}

function command_FACTIONTAXCYCLE( pSock, cmdString )
{
	const totalIncome = TriggerEvent( commandFactionTownScriptId, "RunTownTaxCycle" );
	if( totalIncome >= 0 )
		pSock.SysMessage( GetDictionaryEntry( 25212, pSock.language ).replace( /%s/, String( totalIncome ) ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25213, pSock.language ) );
}

function command_FACTIONTAXRATE( pSock, cmdString )
{
	let cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	let parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	if( parts.length < 2 )
	{
		pSock.SysMessage( GetDictionaryEntry( 25214, pSock.language ) );
		return;
	}

	let amount = parseInt( parts[parts.length - 1], 10 );
	if( isNaN( amount ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25214, pSock.language ) );
		return;
	}

	parts.pop();
	let townName = parts.join( " " );
	if( TriggerEvent( commandFactionTownScriptId, "TownSetTaxRate", townName, amount, true ) )
		pSock.SysMessage( GetDictionaryEntry( 25215, pSock.language ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25216, pSock.language ) );
}

function command_FACTIONTAXSTART( pSock, cmdString )
{
	let cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	let minutes = 0;
	if( cmdText !== "" )
	{
		minutes = parseInt( cmdText, 10 );
		if( isNaN( minutes ) || minutes < 1 )
		{
			pSock.SysMessage( GetDictionaryEntry( 25217, pSock.language ) );
			return;
		}
	}

	if( TriggerEvent( commandFactionTownScriptId, "StartTownTaxTimer", minutes ) )
		pSock.SysMessage( GetDictionaryEntry( 25159, pSock.language ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25160, pSock.language ) );
}

function command_FACTIONTAXSTOP( pSock, cmdString )
{
	if( TriggerEvent( commandFactionTownScriptId, "StopTownTaxTimer" ) )
		pSock.SysMessage( GetDictionaryEntry( 25218, pSock.language ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25219, pSock.language ) );
}

function command_FACTIONTAXSTATUS( pSock, cmdString )
{
	if( !TriggerEvent( commandFactionTownScriptId, "ShowTownTaxStatus", pSock ) )
		pSock.SysMessage( GetDictionaryEntry( 25220, pSock.language ) );
}

function command_FACTIONROLE( pSock, cmdString )
{
	let cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	let parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	if( parts.length < 3 )
	{
		pSock.SysMessage( GetDictionaryEntry( 25221, pSock.language ) );
		return;
	}

	let factionIndex = parts.length - 1;
	let factionKey = parts[factionIndex].toUpperCase();
	let townName = "";
	if( !CommandIsFactionValid( factionKey ) && parts.length >= 4 )
	{
		townName = parts[parts.length - 1];
		factionIndex--;
		factionKey = parts[factionIndex].toUpperCase();
	}
	let roleName = parts[factionIndex - 1].toLowerCase();
	const playerParts = [];
	for( let partIndex = 0; partIndex < factionIndex - 1; partIndex++ )
		playerParts.push( parts[partIndex] );

	if( !CommandIsRoleValid( roleName ) || !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25221, pSock.language ) );
		return;
	}
	if( roleName !== "commander" && townName === "" )
	{
		pSock.SysMessage( GetDictionaryEntry( 25222, pSock.language ) );
		return;
	}

	let targetChar = CommandFindPlayer( playerParts.join( " " ) );
	if( !ValidateObject( targetChar ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25127, pSock.language ) );
		return;
	}

	if( targetChar.GetTag( "faction" ) !== factionKey )
	{
		pSock.SysMessage( GetDictionaryEntry( 25223, pSock.language ).replace( /%s/, String( targetChar.name ) ).replace( /%s/, String( CommandFactionUsageName( factionKey ) ) ) );
		return;
	}

	if( !CommandSetFactionRole( targetChar, roleName, factionKey, townName ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25224, pSock.language ) );
		return;
	}
	pSock.SysMessage( GetDictionaryEntry( 25225, pSock.language ).replace( /%s/, String( targetChar.name ) ).replace( /%s/, String( CommandFactionUsageName( factionKey ) ) ).replace( /%s/, String( CommandRoleDisplayName( roleName ) ) ).replace( /%s/, String( ( townName !== "" ? " of " + townName : "" ) ) ) );
	if( targetChar.socket != null )
		targetChar.SysMessage( GetDictionaryEntry( 25226, ( targetChar.socket == null ? 0 : targetChar.socket.language ) ).replace( /%s/, String( CommandFactionUsageName( factionKey ) ) ).replace( /%s/, String( CommandRoleDisplayName( roleName ) ) ) );
}

function command_FACTIONROLECLEAR( pSock, cmdString )
{
	let targetChar = CommandFindPlayer( cmdString );
	if( !ValidateObject( targetChar ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25227, pSock.language ) );
		return;
	}

	CommandClearFactionRole( targetChar );
	pSock.SysMessage( GetDictionaryEntry( 25228, pSock.language ).replace( /%s/, String( targetChar.name ) ) );
	if( targetChar.socket != null )
		targetChar.SysMessage( GetDictionaryEntry( 25229, ( targetChar.socket == null ? 0 : targetChar.socket.language ) ) );
}

function command_FACTIONROLES( pSock, cmdString )
{
	commandRoleListSocket = pSock;
	let roleCount = IterateOver( "CHARACTER" );
	commandRoleListSocket = null;

	if( roleCount == 0 )
		pSock.SysMessage( GetDictionaryEntry( 25230, pSock.language ) );
}

function command_FACTIONAPPOINT( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	let parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	if( parts.length < 2 )
	{
		pSock.SysMessage( GetDictionaryEntry( 25231, pSock.language ) );
		return;
	}

	let factionKey = pUser.GetTag( "faction" );
	let roleIndex = parts.length - 1;
	let townName = TriggerEvent( commandFactionTownScriptId, "TownNameForObject", pUser );
	const maybeTown = TriggerEvent( commandFactionTownScriptId, "TownNormalizeName", parts[parts.length - 1] );
	if( TriggerEvent( commandFactionTownScriptId, "TownGetDefault", maybeTown ) != null )
	{
		townName = maybeTown;
		roleIndex = parts.length - 2;
	}

	let roleName = parts[roleIndex].toLowerCase();
	const playerParts = [];
	for( let partIndex = 0; partIndex < roleIndex; partIndex++ )
		playerParts.push( parts[partIndex] );

	if( roleName !== "sheriff" && roleName !== "finance" )
	{
		pSock.SysMessage( GetDictionaryEntry( 25231, pSock.language ) );
		return;
	}

	if( !CommandIsFactionValid( factionKey ) && !CommandIsStaff( pUser ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25105, pSock.language ) );
		return;
	}

	let targetChar = CommandFindPlayer( playerParts.join( " " ) );
	if( !ValidateObject( targetChar ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25127, pSock.language ) );
		return;
	}

	if( factionKey === "" && CommandIsStaff( pUser ) )
		factionKey = targetChar.GetTag( "faction" );

	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25232, pSock.language ) );
		return;
	}

	if( !CommandCanAppointFactionRole( pUser, factionKey ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25233, pSock.language ) );
		return;
	}
	if( townName === "" || TriggerEvent( commandFactionTownScriptId, "TownGetOwner", townName ) !== factionKey )
	{
		pSock.SysMessage( GetDictionaryEntry( 25234, pSock.language ) );
		return;
	}

	if( targetChar.GetTag( "faction" ) !== factionKey )
	{
		pSock.SysMessage( GetDictionaryEntry( 25235, pSock.language ).replace( /%s/, String( targetChar.name ) ) );
		return;
	}
	if( targetChar.GetTag( "faction_commander" ) == 1 )
	{
		pSock.SysMessage( GetDictionaryEntry( 25236, pSock.language ) );
		return;
	}

	if( !CommandSetFactionRole( targetChar, roleName, factionKey, townName ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25237, pSock.language ) );
		return;
	}
	pSock.SysMessage( GetDictionaryEntry( 25238, pSock.language ).replace( /%s/, String( targetChar.name ) ).replace( /%s/, String( CommandFactionUsageName( factionKey ) ) ).replace( /%s/, String( CommandRoleDisplayName( roleName ) ) ).replace( /%s/, String( townName ) ) );
	if( targetChar.socket != null )
		targetChar.SysMessage( GetDictionaryEntry( 25239, ( targetChar.socket == null ? 0 : targetChar.socket.language ) ).replace( /%s/, String( CommandFactionUsageName( factionKey ) ) ).replace( /%s/, String( CommandRoleDisplayName( roleName ) ) ).replace( /%s/, String( townName ) ) );
}

function command_FACTIONGUARD( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let factionKey = String( cmdString ).replace( /^\s+|\s+$/g, "" ).toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25240, pSock.language ) );
		return;
	}

	if( !CommandCanUseFactionNpcCommand( pSock, pUser, factionKey, "sheriff" ) )
		return;

	if( !CommandCanPlaceFactionNpc( pSock, pUser, factionKey ) )
		return;
	if( !CommandCanPlaceFactionNpcType( pSock, pUser, factionKey, "guard" ) )
		return;

	let townName = CommandCurrentFactionTown( pUser );
	if( !CommandSpendTownTreasury( pSock, pUser, townName, commandFactionGuardCost, "a faction guard" ) )
		return;

	const guard = CommandSpawnFactionNpc( pSock, pUser, "FACTION_GUARD_" + factionKey, factionKey, "guard", "" );
	if( ValidateObject( guard ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25241, pSock.language ).replace( /%s/, String( CommandFactionUsageName( factionKey ) ) ) );
	}
	else
	{
		if( !CommandIsStaff( pUser ) )
			TriggerEvent( commandFactionTownScriptId, "TownAddTreasury", townName, commandFactionGuardCost );
		pSock.SysMessage( GetDictionaryEntry( 25242, pSock.language ) );
	}
}

function command_FACTIONVENDOR( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let parts = String( cmdString ).replace( /^\s+|\s+$/g, "" ).toUpperCase().split( /\s+/ );
	let factionKey = "";
	let vendorType = "";

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
		pSock.SysMessage( GetDictionaryEntry( 25243, pSock.language ) );
		return;
	}

	if( !CommandCanUseFactionNpcCommand( pSock, pUser, factionKey, "finance" ) )
		return;

	if( !CommandCanPlaceFactionNpc( pSock, pUser, factionKey ) )
		return;
	if( !CommandCanPlaceFactionNpcType( pSock, pUser, factionKey, "vendor" ) )
		return;

	let townName = CommandCurrentFactionTown( pUser );
	const vendorCost = CommandFactionVendorPurchaseCost( vendorType );
	if( !CommandSpendTownTreasury( pSock, pUser, townName, vendorCost, "a faction vendor" ) )
		return;

	const vendor = CommandSpawnFactionNpc( pSock, pUser, "FACTION_" + vendorType + "_VENDOR_" + factionKey, factionKey, "vendor", vendorType );
	if( ValidateObject( vendor ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25244, pSock.language ).replace( /%s/, String( CommandFactionUsageName( factionKey ) ) ).replace( /%s/, String( vendorType.toLowerCase() ) ) );
	}
	else
	{
		if( !CommandIsStaff( pUser ) )
			TriggerEvent( commandFactionTownScriptId, "TownAddTreasury", townName, vendorCost );
		pSock.SysMessage( GetDictionaryEntry( 25245, pSock.language ) );
	}
}

function CommandFactionNpcUsage( pSock )
{
	pSock.SysMessage( GetDictionaryEntry( 25246, pSock.language ) );
	pSock.SysMessage( GetDictionaryEntry( 25247, pSock.language ) );
}

function command_FACTIONNPC( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	let parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	let factionIndex = -1;
	let factionKey = "";
	for( let partIndex = 0; partIndex < parts.length; partIndex++ )
	{
		const possibleFaction = parts[partIndex].toUpperCase();
		if( CommandIsFactionValid( possibleFaction ) )
		{
			factionIndex = partIndex;
			factionKey = possibleFaction;
			break;
		}
	}

	if( factionIndex <= 0 || factionKey === "" )
	{
		CommandFactionNpcUsage( pSock );
		return;
	}

	let npcName = parts.slice( 0, factionIndex ).join( " " );
	const targetNpc = CommandFindNpc( npcName );
	if( !ValidateObject( targetNpc ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25248, pSock.language ) );
		return;
	}

	let passive = 0;
	let requireTownControl = 0;
	for( let optionIndex = factionIndex + 1; optionIndex < parts.length; optionIndex++ )
	{
		let optionText = parts[optionIndex].toLowerCase();
		if( optionText === "passive" )
		{
			passive = 1;
		}
		else if( optionText === "aggressive" )
		{
			passive = 0;
		}
		else if( optionText === "town" || optionText === "townonly" || optionText === "controlled" )
		{
			requireTownControl = 1;
		}
		else if( optionText === "world" || optionText === "always" )
		{
			requireTownControl = 0;
		}
		else
		{
			CommandFactionNpcUsage( pSock );
			return;
		}
	}

	targetNpc.SetTag( "npc_faction", factionKey );
	targetNpc.SetTag( "faction", factionKey );
	targetNpc.SetTag( "faction_npc", 1 );
	targetNpc.SetTag( "npc_faction_passive", passive );
	targetNpc.SetTag( "npc_faction_require_town_control", requireTownControl );
	if( !targetNpc.HasScriptTrigger( commandFactionNpcScriptId ) )
		targetNpc.AddScriptTrigger( commandFactionNpcScriptId );

	let modeText = passive == 1 ? "passive" : "aggressive";
	if( requireTownControl == 1 )
		modeText += ", town controlled";

	pSock.SysMessage( GetDictionaryEntry( 25249, pSock.language ).replace( /%s/, String( targetNpc.name ) ).replace( /%s/, String( CommandFactionUsageName( factionKey ) ) ).replace( /%s/, String( modeText ) ) );
}

function command_FACTIONNPCCLEAR( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let npcName = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	if( npcName === "" )
	{
		pSock.SysMessage( GetDictionaryEntry( 25250, pSock.language ) );
		return;
	}

	const targetNpc = CommandFindNpc( npcName );
	if( !ValidateObject( targetNpc ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25248, pSock.language ) );
		return;
	}

	if( CommandClearGenericFactionNpcTags( targetNpc ) )
		pSock.SysMessage( GetDictionaryEntry( 25251, pSock.language ).replace( /%s/, String( targetNpc.name ) ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25252, pSock.language ).replace( /%s/, String( targetNpc.name ) ) );
}

function command_FACTIONITEMCHECK( pSock, cmdString )
{
	if( !TriggerEvent( 8507, "ShowFactionItemCheck", pSock ) )
		pSock.SysMessage( GetDictionaryEntry( 25253, pSock.language ) );
}

function command_FACTIONITEMCLEANUP( pSock, cmdString )
{
	const cleanedCount = TriggerEvent( 8507, "CleanupInvalidFactionItems" );
	pSock.SysMessage( GetDictionaryEntry( 25254, pSock.language ).replace( /%s/, String( cleanedCount ) ) );
}

function command_FACTIONSILVER( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let cmdText = String( cmdString ).replace( /^\s+|\s+$/g, "" );
	let parts = cmdText.length > 0 ? cmdText.split( /\s+/ ) : [];
	let action = "";
	let amount = 0;
	let targetChar = pUser;

	if( parts.length >= 1 )
		action = parts[0].toLowerCase();

	if( parts.length == 2 )
	{
		amount = parseInt( parts[1], 10 );
		if( action !== "add" && action !== "set" || isNaN( amount ) )
		{
			pSock.SysMessage( GetDictionaryEntry( 25255, pSock.language ) );
			return;
		}
	}
	else
	{
		let parsed = CommandParsePlayerAmountArgs( pSock, cmdText, "Usage: 'factionsilver <add|set> [player] <amount>" );
		if( parsed == null )
			return;

		action = parsed.action;
		amount = parsed.amount;
		targetChar = parsed.targetChar;
	}

	const currentSilver = CommandClampNonNegative( TriggerEvent( commandFactionPlayerDataScriptId, "GetFactionValue", targetChar, "silver", targetChar.GetTag( "faction_silver" ) ) );
	let newSilver = currentSilver;

	if( action === "add" )
		newSilver = currentSilver + amount;
	else
		newSilver = amount;

	newSilver = CommandClampSilver( newSilver );

	TriggerEvent( commandFactionPlayerDataScriptId, "SetFactionValue", targetChar, "silver", newSilver );
	pSock.SysMessage( GetDictionaryEntry( 25256, pSock.language ).replace( /%s/, String( targetChar.name ) ).replace( /%s/, String( newSilver ) ) );
	if( targetChar.socket != null && targetChar.serial != pUser.serial )
		targetChar.SysMessage( GetDictionaryEntry( 25257, ( targetChar.socket == null ? 0 : targetChar.socket.language ) ).replace( /%s/, String( newSilver ) ) );
}

function command_FACTIONKP( pSock, cmdString )
{
	let parsed = CommandParsePlayerAmountArgs( pSock, cmdString, "Usage: 'factionkp <add|set> <player> <amount>" );
	if( parsed == null )
		return;

	let currentPoints = CommandClampNonNegative( TriggerEvent( commandFactionPlayerDataScriptId, "GetFactionValue", parsed.targetChar, "killPoints", parsed.targetChar.GetTag( "faction_kp" ) ) );
	let newPoints = parsed.action === "add" ? currentPoints + parsed.amount : parsed.amount;
	newPoints = CommandClampNonNegative( newPoints );

	const oldRank = CommandClampNonNegative( TriggerEvent( commandFactionPlayerDataScriptId, "GetFactionValue", parsed.targetChar, "rank", parsed.targetChar.GetTag( "faction_rank" ) ) );
	TriggerEvent( commandFactionPlayerDataScriptId, "SetFactionValue", parsed.targetChar, "killPoints", newPoints );
	const newRank = CommandUpdateFactionRank( parsed.targetChar );

	pSock.SysMessage( GetDictionaryEntry( 25258, pSock.language ).replace( /%s/, String( parsed.targetChar.name ) ).replace( /%s/, String( newPoints ) ).replace( /%s/, String( CommandRankName( newRank ) ) ) );
	if( parsed.targetChar.socket != null )
	{
		parsed.targetChar.SysMessage( GetDictionaryEntry( 25259, ( parsed.targetChar.socket == null ? 0 : parsed.targetChar.socket.language ) ).replace( /%s/, String( newPoints ) ) );
		if( newRank !== oldRank )
			parsed.targetChar.SysMessage( GetDictionaryEntry( 25118, ( parsed.targetChar.socket == null ? 0 : parsed.targetChar.socket.language ) ).replace( /%s/, String( CommandRankName( newRank ) ) ) );
	}
}

function command_FACTIONCAPTURE( pSock, cmdString )
{
	let parsed = CommandParsePlayerAmountArgs( pSock, cmdString, "Usage: 'factioncapture <add|set> <player> <amount>" );
	if( parsed == null )
		return;

	const currentCaptures = CommandClampNonNegative( TriggerEvent( commandFactionPlayerDataScriptId, "GetFactionValue", parsed.targetChar, "captures", parsed.targetChar.GetTag( "faction_captures" ) ) );
	let newCaptures = parsed.action === "add" ? currentCaptures + parsed.amount : parsed.amount;
	newCaptures = CommandClampNonNegative( newCaptures );

	TriggerEvent( commandFactionPlayerDataScriptId, "SetFactionValue", parsed.targetChar, "captures", newCaptures );
	pSock.SysMessage( GetDictionaryEntry( 25260, pSock.language ).replace( /%s/, String( parsed.targetChar.name ) ).replace( /%s/, String( newCaptures ) ) );
	if( parsed.targetChar.socket != null )
		parsed.targetChar.SysMessage( GetDictionaryEntry( 25261, ( parsed.targetChar.socket == null ? 0 : parsed.targetChar.socket.language ) ).replace( /%s/, String( newCaptures ) ) );
}

function CommandCreateFactionItem( pSock, pUser, itemType, factionKey )
{
	let sectionId = "";
	if( itemType === "ROBE" )
		sectionId = "FACTION_ROBE_" + factionKey;
	else if( itemType === "SHIELD" )
		sectionId = "FACTION_SHIELD_" + factionKey;
	else if( itemType === "HORSE" )
		sectionId = "FACTION_WAR_HORSE_DEED";
	else
		return null;

	const newItem = CreateDFNItem( pSock, pUser, sectionId, 1, "ITEM", true );
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
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let parts = cmdString.toUpperCase().split( " " );
	let itemType = parts[0];
	let factionKey = parts[1];

	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25262, pSock.language ) );
		return;
	}

	const newItem = CommandCreateFactionItem( pSock, pUser, itemType, factionKey );
	if( !ValidateObject( newItem ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25263, pSock.language ) );
		return;
	}

	pSock.SysMessage( GetDictionaryEntry( 25264, pSock.language ) );
}

function command_SPAWNTRAPDEED( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	const trapType = cmdString.toUpperCase();
	if( trapType !== "EXPLOSION" && trapType !== "GAS" && trapType !== "SAW" && trapType !== "SPIKE" )
	{
		pSock.SysMessage( GetDictionaryEntry( 25265, pSock.language ) );
		return;
	}

	const deed = CreateDFNItem( pSock, pUser, "FACTION_TRAP_DEED", 1, "ITEM", true );
	if( ValidateObject( deed ) )
	{
		deed.SetTag( "trap_deed", 1 );
		deed.SetTag( "trap_deed_type", trapType );
		deed.name = "Faction " + trapType + " Trap Deed";
		pSock.SysMessage( GetDictionaryEntry( 25266, pSock.language ) );
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 25267, pSock.language ) );
	}
}

function command_ELECTIONSTART( pSock, cmdString )
{
	let factionKey = cmdString.toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25268, pSock.language ) );
		return;
	}

	if( TriggerEvent( commandFactionElectionScriptId, "StartElection", factionKey ) )
		pSock.SysMessage( GetDictionaryEntry( 25269, pSock.language ).replace( /%s/, String( factionKey ) ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25270, pSock.language ) );
}

function command_ELECTIONVOTE( pSock, cmdString )
{
	let factionKey = cmdString.toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25271, pSock.language ) );
		return;
	}

	if( TriggerEvent( commandFactionElectionScriptId, "BeginVoting", factionKey ) )
		pSock.SysMessage( GetDictionaryEntry( 25272, pSock.language ).replace( /%s/, String( factionKey ) ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25273, pSock.language ) );
}

function command_ELECTIONEND( pSock, cmdString )
{
	let factionKey = cmdString.toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25274, pSock.language ) );
		return;
	}

	if( TriggerEvent( commandFactionElectionScriptId, "ConcludeElection", factionKey ) )
		pSock.SysMessage( GetDictionaryEntry( 25275, pSock.language ).replace( /%s/, String( factionKey ) ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25276, pSock.language ) );
}

function command_ELECTIONSTATUS( pSock, cmdString )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	let factionKey = pUser.GetTag( "faction" );
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25105, pSock.language ) );
		return;
	}

	TriggerEvent( commandFactionElectionScriptId, "ShowElectionStatus", pSock, factionKey );
}

function command_ELECTIONRESET( pSock, cmdString )
{
	let factionKey = cmdString.toUpperCase();
	if( !CommandIsFactionValid( factionKey ) )
	{
		pSock.SysMessage( GetDictionaryEntry( 25277, pSock.language ) );
		return;
	}

	if( TriggerEvent( commandFactionElectionScriptId, "ResetElection", factionKey ) )
		pSock.SysMessage( GetDictionaryEntry( 25278, pSock.language ).replace( /%s/, String( factionKey ) ) );
	else
		pSock.SysMessage( GetDictionaryEntry( 25279, pSock.language ) );
}
