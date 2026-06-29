// =============================================================================
// factions_playerdata.js
// UOX3 Faction System - shared per-player faction data storage
// Script ID suggestion: 8513
// =============================================================================

var FactionPlayerDataVersion = 1;
var FactionPlayerDataFolder = "Factions";
var FactionPlayerDataMaxSilver = 100000;
var FactionPlayerDataMirrorTags = [
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
	"faction_role_set_at",
	"faction_kp_decay_time"
];

function FactionDataIsValidFaction( factionKey )
{
	return ( factionKey === "TB" || factionKey === "COM" || factionKey === "MIN" || factionKey === "SL" );
}

function FactionDataParseNumber( value, fallback )
{
	var parsed = parseInt( value, 10 );
	if( isNaN( parsed ) )
		return fallback;

	return parsed;
}

function FactionDataDefault()
{
	return {
		faction: "",
		joinTime: 0,
		leaveTime: 0,
		killPoints: 0,
		silver: 0,
		rank: 0,
		captures: 0,
		commander: false,
		role: "",
		roleFaction: "",
		roleSetAt: 0,
		kpDecayTime: 0,
		votes: { TB: false, COM: false, MIN: false, SL: false },
		recentKills: {}
	};
}

function FactionDataAccountFileName( pChar )
{
	if( !ValidateObject( pChar ) || !pChar.account )
		return "";

	return "FactionPlayers_" + pChar.account.id + ".json";
}

function FactionDataReadAccount( pChar )
{
	var fileName = FactionDataAccountFileName( pChar );
	var accountData = {
		version: FactionPlayerDataVersion,
		characters: {}
	};

	if( fileName === "" )
		return accountData;

	var dataFile = new UOXCFile();
	dataFile.Open( fileName, "r", FactionPlayerDataFolder );
	if( !dataFile || dataFile.Length() <= 0 )
	{
		if( dataFile )
			dataFile.Free();
		return accountData;
	}

	var fileText = "";
	while( !dataFile.EOF() )
	{
		var rawLine = dataFile.ReadUntil( "\n" );
		if( rawLine != null && typeof rawLine != "undefined" )
		{
			fileText += rawLine;
			if( !dataFile.EOF() )
				fileText += "\n";
		}
	}

	dataFile.Close();
	dataFile.Free();

	fileText = fileText.replace( /[^\x20-\x7E\r\n\t]/g, "" ).trim();
	if( fileText === "" )
		return accountData;

	try
	{
		var parsedData = JSON.parse( fileText );
		if( parsedData && typeof parsedData == "object" )
			accountData = parsedData;
	}
	catch( error )
	{
		Console.Warning( "Faction player data: Failed to parse " + fileName + ": " + error );
	}

	if( !accountData.characters )
		accountData.characters = {};
	accountData.version = FactionPlayerDataVersion;
	return accountData;
}

function FactionDataWriteAccount( pChar, accountData )
{
	var fileName = FactionDataAccountFileName( pChar );
	if( fileName === "" )
		return false;

	if( !accountData || typeof accountData != "object" )
		accountData = { version: FactionPlayerDataVersion, characters: {} };
	if( !accountData.characters )
		accountData.characters = {};
	accountData.version = FactionPlayerDataVersion;

	var fileText = "";
	try
	{
		fileText = JSON.stringify( accountData, null, "\t" );
	}
	catch( error )
	{
		Console.Warning( "Faction player data: Failed to serialize " + fileName + ": " + error );
		return false;
	}

	var dataFile = new UOXCFile();
	dataFile.Open( fileName, "w", FactionPlayerDataFolder );
	if( !dataFile )
		return false;

	dataFile.Write( fileText + "\n" );
	dataFile.Close();
	dataFile.Free();
	return true;
}

function FactionDataNormalize( pChar, factionData )
{
	var normalized = FactionDataDefault();
	if( factionData && typeof factionData == "object" )
	{
		for( var dataKey in normalized )
		{
			if( typeof factionData[dataKey] != "undefined" )
				normalized[dataKey] = factionData[dataKey];
		}
	}

	if( !FactionDataIsValidFaction( normalized.faction ) )
		normalized.faction = "";
	normalized.joinTime = FactionDataParseNumber( normalized.joinTime, 0 );
	normalized.leaveTime = FactionDataParseNumber( normalized.leaveTime, 0 );
	normalized.killPoints = FactionDataParseNumber( normalized.killPoints, 0 );
	normalized.silver = FactionDataParseNumber( normalized.silver, 0 );
	normalized.rank = FactionDataParseNumber( normalized.rank, 0 );
	normalized.captures = FactionDataParseNumber( normalized.captures, 0 );
	normalized.roleSetAt = FactionDataParseNumber( normalized.roleSetAt, 0 );
	normalized.kpDecayTime = FactionDataParseNumber( normalized.kpDecayTime, 0 );
	normalized.commander = normalized.commander ? true : false;
	normalized.role = String( normalized.role || "" );
	normalized.roleFaction = FactionDataIsValidFaction( normalized.roleFaction ) ? normalized.roleFaction : "";
	if( !normalized.votes || typeof normalized.votes != "object" )
		normalized.votes = { TB: false, COM: false, MIN: false, SL: false };
	normalized.votes.TB = normalized.votes.TB ? true : false;
	normalized.votes.COM = normalized.votes.COM ? true : false;
	normalized.votes.MIN = normalized.votes.MIN ? true : false;
	normalized.votes.SL = normalized.votes.SL ? true : false;
	if( !normalized.recentKills || typeof normalized.recentKills != "object" )
		normalized.recentKills = {};

	if( normalized.killPoints < 0 )
		normalized.killPoints = 0;
	if( normalized.silver < 0 )
		normalized.silver = 0;
	if( normalized.silver > FactionPlayerDataMaxSilver )
		normalized.silver = FactionPlayerDataMaxSilver;
	if( normalized.captures < 0 )
		normalized.captures = 0;
	if( normalized.rank < 0 )
		normalized.rank = 0;
	if( normalized.rank > 9 )
		normalized.rank = 9;

	return normalized;
}

function FactionDataFromTags( pChar )
{
	var data = FactionDataDefault();
	if( !ValidateObject( pChar ) )
		return data;

	var factionKey = pChar.GetTag( "faction" );
	data.faction = FactionDataIsValidFaction( factionKey ) ? factionKey : "";
	data.joinTime = FactionDataParseNumber( pChar.GetTag( "faction_join_time" ), 0 );
	data.leaveTime = FactionDataParseNumber( pChar.GetTag( "faction_leave_time" ), 0 );
	data.killPoints = FactionDataParseNumber( pChar.GetTag( "faction_kp" ), 0 );
	data.silver = FactionDataParseNumber( pChar.GetTag( "faction_silver" ), 0 );
	data.rank = FactionDataParseNumber( pChar.GetTag( "faction_rank" ), 0 );
	data.captures = FactionDataParseNumber( pChar.GetTag( "faction_captures" ), 0 );
	data.commander = ( pChar.GetTag( "faction_commander" ) == 1 || pChar.GetTag( "faction_commander" ) === "1" );
	data.role = String( pChar.GetTag( "faction_role" ) || "" );
	var roleFaction = pChar.GetTag( "faction_role_faction" );
	data.roleFaction = FactionDataIsValidFaction( roleFaction ) ? roleFaction : "";
	data.roleSetAt = FactionDataParseNumber( pChar.GetTag( "faction_role_set_at" ), 0 );
	data.kpDecayTime = FactionDataParseNumber( pChar.GetTag( "faction_kp_decay_time" ), 0 );
	data.votes = {
		TB: ( pChar.GetTag( "elec_voted_TB" ) === "1" ),
		COM: ( pChar.GetTag( "elec_voted_COM" ) === "1" ),
		MIN: ( pChar.GetTag( "elec_voted_MIN" ) === "1" ),
		SL: ( pChar.GetTag( "elec_voted_SL" ) === "1" )
	};

	return FactionDataNormalize( pChar, data );
}

function ReadFactionPlayerData( pChar )
{
	if( !ValidateObject( pChar ) || pChar.npc || !pChar.account )
		return FactionDataDefault();

	var accountData = FactionDataReadAccount( pChar );
	var playerKey = String( pChar.serial );
	if( accountData.characters && accountData.characters[playerKey] )
		return FactionDataNormalize( pChar, accountData.characters[playerKey] );

	return FactionDataFromTags( pChar );
}

function WriteFactionPlayerData( pChar, factionData )
{
	if( !ValidateObject( pChar ) || pChar.npc || !pChar.account )
		return false;

	var accountData = FactionDataReadAccount( pChar );
	var playerKey = String( pChar.serial );
	accountData.characters[playerKey] = FactionDataNormalize( pChar, factionData );
	if( !FactionDataWriteAccount( pChar, accountData ) )
		return false;

	SyncFactionMirrorTags( pChar, accountData.characters[playerKey] );
	return true;
}

function SyncFactionMirrorTags( pChar, factionData )
{
	if( !ValidateObject( pChar ) || pChar.npc )
		return false;

	factionData = FactionDataNormalize( pChar, factionData || ReadFactionPlayerData( pChar ) );
	pChar.SetTag( "faction", factionData.faction );
	pChar.SetTag( "faction_join_time", factionData.joinTime );
	pChar.SetTag( "faction_kp", factionData.killPoints );
	pChar.SetTag( "faction_silver", factionData.silver );
	pChar.SetTag( "faction_rank", factionData.rank );
	pChar.SetTag( "faction_captures", factionData.captures );
	pChar.SetTag( "faction_leave_time", factionData.leaveTime );
	pChar.SetTag( "faction_commander", factionData.commander ? "1" : "0" );
	pChar.SetTag( "faction_role", factionData.role );
	pChar.SetTag( "faction_role_faction", factionData.roleFaction );
	pChar.SetTag( "faction_role_set_at", factionData.roleSetAt );
	pChar.SetTag( "faction_kp_decay_time", factionData.kpDecayTime );
	pChar.SetTag( "elec_voted_TB", factionData.votes.TB ? "1" : "" );
	pChar.SetTag( "elec_voted_COM", factionData.votes.COM ? "1" : "" );
	pChar.SetTag( "elec_voted_MIN", factionData.votes.MIN ? "1" : "" );
	pChar.SetTag( "elec_voted_SL", factionData.votes.SL ? "1" : "" );
	return true;
}

function GetFactionValue( pChar, dataKey, fallback )
{
	var factionData = ReadFactionPlayerData( pChar );
	if( typeof factionData[dataKey] == "undefined" )
		return fallback;

	return factionData[dataKey];
}

function SetFactionValue( pChar, dataKey, dataValue )
{
	var factionData = ReadFactionPlayerData( pChar );
	factionData[dataKey] = dataValue;
	return WriteFactionPlayerData( pChar, factionData );
}

function AddFactionValue( pChar, dataKey, amount )
{
	var factionData = ReadFactionPlayerData( pChar );
	var currentValue = FactionDataParseNumber( factionData[dataKey], 0 );
	factionData[dataKey] = currentValue + amount;
	return WriteFactionPlayerData( pChar, factionData );
}

function GetRecentKillTime( pChar, targetSerial )
{
	var factionData = ReadFactionPlayerData( pChar );
	var killKey = String( targetSerial );
	return FactionDataParseNumber( factionData.recentKills[killKey], 0 );
}

function SetRecentKillTime( pChar, targetSerial, killTime )
{
	var factionData = ReadFactionPlayerData( pChar );
	var killKey = String( targetSerial );
	factionData.recentKills[killKey] = FactionDataParseNumber( killTime, 0 );
	return WriteFactionPlayerData( pChar, factionData );
}

function GetFactionVote( pChar, factionKey )
{
	if( !FactionDataIsValidFaction( factionKey ) )
		return false;

	var factionData = ReadFactionPlayerData( pChar );
	return factionData.votes[factionKey] ? true : false;
}

function SetFactionVote( pChar, factionKey, voteValue )
{
	if( !FactionDataIsValidFaction( factionKey ) )
		return false;

	var factionData = ReadFactionPlayerData( pChar );
	factionData.votes[factionKey] = voteValue ? true : false;
	return WriteFactionPlayerData( pChar, factionData );
}

function ClearFactionPlayerData( pChar )
{
	if( !ValidateObject( pChar ) || pChar.npc || !pChar.account )
		return false;

	var accountData = FactionDataReadAccount( pChar );
	var playerKey = String( pChar.serial );
	var hadData = false;
	if( accountData.characters && accountData.characters[playerKey] )
	{
		delete accountData.characters[playerKey];
		hadData = true;
		FactionDataWriteAccount( pChar, accountData );
	}

	for( var tagIndex = 0; tagIndex < FactionPlayerDataMirrorTags.length; tagIndex++ )
	{
		if( pChar.GetTag( FactionPlayerDataMirrorTags[tagIndex] ) !== "" && pChar.GetTag( FactionPlayerDataMirrorTags[tagIndex] ) != 0 )
			hadData = true;
		pChar.SetTag( FactionPlayerDataMirrorTags[tagIndex], null );
	}
	pChar.SetTag( "elec_voted_TB", null );
	pChar.SetTag( "elec_voted_COM", null );
	pChar.SetTag( "elec_voted_MIN", null );
	pChar.SetTag( "elec_voted_SL", null );
	return hadData;
}

function MigrateFactionTagsToFile( pChar )
{
	if( !ValidateObject( pChar ) || pChar.npc || !pChar.account )
		return false;

	var factionData = FactionDataFromTags( pChar );
	return WriteFactionPlayerData( pChar, factionData );
}
