/// <reference path="../../../definitions.d.ts" />
// @ts-check

var vetRewardTable = null;
var vetRewardTableLoaded = false;
var vetRewardTableLoadError = false;

// Number of account age days required for each veteran reward point.
// 90 = about 3 months, 180 = about 6 months, 365 = about 1 year.
const vetRewardDaysPerReward  = 90;

// Account age source for veteran rewards.
// "playtime" = use account.totalPlayTime minutes.
// "firstlogin" = use account.firstLogin real account age.
// "auto" = use firstLogin if available, otherwise fall back to playtime.
const vetRewardAccountAgeMode = "firstlogin";

/** @type { ( sockPlayer: Socket, pChar: Character ) => void } */
function HandleVetRewardLogin( sockPlayer, pChar )
{
	VetRewardOnLogin( sockPlayer, pChar );
}

/** @type { ( sockPlayer: Socket, pChar: Character ) => void } */
function VetRewardOnLogin( sockPlayer, pChar )
{
	if( !ValidateObject( pChar ) || pChar.dead )
	{
		return;
	}

	if( GetVeteranRewardsLeft( pChar ) <= 0 )
	{
		return;
	}

	ShowVeteranRewardNotice( pChar );
}

/** @type { ( pUser: Character ) => void } */
function VetRewardMenu( pUser )
{
	ShowVeteranRewardNotice( pUser );
}

/** @type { () => void } */
function ReloadVeteranRewardTable()
{
	vetRewardTable = null;
	vetRewardTableLoaded = false;
	vetRewardTableLoadError = false;
	LoadVeteranRewardTable();
}

/** @type { () => any } */
function GetVeteranRewardTable()
{
	if( !vetRewardTableLoaded )
	{
		LoadVeteranRewardTable();
	}

	if( !vetRewardTable || !vetRewardTable.categories )
	{
		return { version: 1, categories: [] };
	}

	return vetRewardTable;
}

/** @type { () => any } */
function LoadVeteranRewardTable()
{
	vetRewardTable = { version: 1, categories: [] };
	vetRewardTableLoaded = false;
	vetRewardTableLoadError = false;

	var tableFile = new UOXCFile();
	tableFile.Open( "vet_rewards_table.json", "r", "vetrewards", true );

	if( tableFile == null || tableFile.Length() < 0 )
	{
		Console.Error( "Veteran rewards: Unable to open js/jsdata/vetrewards/vet_rewards_table.json" );
		vetRewardTableLoadError = true;
		return;
	}

	var fileText = "";
	while( !tableFile.EOF() )
	{
		var rawLine = tableFile.ReadUntil( "\n" );
		if( rawLine != null && typeof rawLine != "undefined" )
		{
			fileText += rawLine;
			if( !tableFile.EOF() )
			{
				fileText += "\n";
			}
		}
	}

	tableFile.Close();
	tableFile.Free();

	fileText = VetRewardSanitizeJsonText( fileText );
	if( fileText == "" )
	{
		Console.Error( "Veteran rewards: vet_rewards_table.json is empty." );
		vetRewardTableLoadError = true;
		return;
	}

	try
	{
		var parsedTable = JSON.parse( fileText );
		if( parsedTable && typeof parsedTable == "object" && parsedTable.categories && VetRewardIsArray( parsedTable.categories ) )
		{
			vetRewardTable = parsedTable;
			NormalizeVeteranRewardTable( vetRewardTable );
			vetRewardTableLoaded = true;
			vetRewardTableLoadError = false;
			Console.Print( "Veteran rewards: Loaded " + CountVeteranRewardEntries( vetRewardTable ) + " rewards.\n" );
			return;
		}
	}
	catch( error )
	{
		Console.Error( "Veteran rewards: Failed to parse vet_rewards_table.json: " + error );
		vetRewardTableLoaded = true;
		vetRewardTableLoadError = true;
		return;
	}

	Console.Error( "Veteran rewards: vet_rewards_table.json must contain a categories array." );
	vetRewardTableLoaded = true;
	vetRewardTableLoadError = true;
}

/** @type { ( table: any ) => void } */
function NormalizeVeteranRewardTable( table )
{
	if( !table.categories )
	{
		table.categories = [];
	}

	for( var categoryIndex = 0; categoryIndex < table.categories.length; categoryIndex++ )
	{
		var category = table.categories[categoryIndex];
		if( !category )
		{
			continue;
		}

		if( !category.name )
		{
			category.name = "Veteran Rewards";
		}

		if( !category.entries )
		{
			category.entries = [];
		}
	}
}

/** @type { ( table: any ) => number } */
function CountVeteranRewardEntries( table )
{
	var rewardCount = 0;

	if( !table || !table.categories )
	{
		return rewardCount;
	}

	for( var categoryIndex = 0; categoryIndex < table.categories.length; categoryIndex++ )
	{
		var category = table.categories[categoryIndex];
		if( !category || !category.entries )
		{
			continue;
		}

		rewardCount += category.entries.length;
	}

	return rewardCount;
}

/** @type { () => any[] } */
function GetVeteranRewardFlatList()
{
	var table = GetVeteranRewardTable();
	var rewards = [];
	var rewardID = 1;

	for( var categoryIndex = 0; categoryIndex < table.categories.length; categoryIndex++ )
	{
		var category = table.categories[categoryIndex];
		if( !category || !category.entries )
		{
			continue;
		}

		for( var entryIndex = 0; entryIndex < category.entries.length; entryIndex++ )
		{
			var entry = category.entries[entryIndex];
			if( !entry || !entry.section )
			{
				continue;
			}

			rewards.push({ rewardID: rewardID, categoryIndex: categoryIndex, entryIndex: entryIndex, categoryName: category.name, name: entry.name || entry.section, section: entry.section, level: parseInt( entry.level, 10 ) || 1, color: entry.color, amount: parseInt( entry.amount, 10 ) || 1 });

			rewardID++;
		}
	}

	return rewards;
}

/** @type { ( rewardID: number|string ) => any|null } */
function GetVeteranRewardByID( rewardID )
{
	rewardID = parseInt( rewardID, 10 );
	if( isNaN( rewardID ) || rewardID <= 0 )
	{
		return null;
	}

	var rewards = GetVeteranRewardFlatList();
	for( var rewardIndex = 0; rewardIndex < rewards.length; rewardIndex++ )
	{
		if( rewards[rewardIndex].rewardID == rewardID )
		{
			return rewards[rewardIndex];
		}
	}

	return null;
}

/** @type { ( player: Character ) => any } */
function ReadVeteranRewardSave( player )
{
	if( !ValidateObject( player ) || !player.account )
	{
		return CreateDefaultVeteranRewardSave( player );
	}

	var saveFile = new UOXCFile();
	var account = player.account;
	var fileName = "VetRewards_" + account.id + ".json";
	var playerKey = String( player.serial );

	saveFile.Open( fileName, "r", "VeteranRewards" );
	if( !saveFile || saveFile.Length() <= 0 )
	{
		if( saveFile )
		{
			saveFile.Free();
		}
		return CreateDefaultVeteranRewardSave( player );
	}

	var fileText = "";
	while( !saveFile.EOF() )
	{
		var rawLine = saveFile.ReadUntil( "\n" );
		if( rawLine != null && typeof rawLine != "undefined" )
		{
			fileText += rawLine;
			if( !saveFile.EOF() )
			{
				fileText += "\n";
			}
		}
	}

	saveFile.Close();
	saveFile.Free();

	fileText = VetRewardSanitizeJsonText( fileText );
	if( fileText == "" )
	{
		return CreateDefaultVeteranRewardSave( player );
	}

	try
	{
		var parsedSave = JSON.parse( fileText );
		if( parsedSave && parsedSave.characters && parsedSave.characters[playerKey] )
		{
			return NormalizeVeteranRewardSave( player, parsedSave.characters[playerKey] );
		}
	}
	catch( error )
	{
		Console.Warning( "Veteran rewards: Failed to parse " + fileName + ": " + error );
	}

	return CreateDefaultVeteranRewardSave( player );
}

/** @type { ( player: Character, characterSave: any ) => boolean } */
function WriteVeteranRewardSave( player, characterSave )
{
	if( !ValidateObject( player ) || !player.account )
	{
		return false;
	}

	characterSave = NormalizeVeteranRewardSave( player, characterSave );

	var saveFile = new UOXCFile();
	var account = player.account;
	var fileName = "VetRewards_" + account.id + ".json";
	var playerKey = String( player.serial );

	var saveData = {
		version: 1,
		characters: {}
	};

	saveFile.Open( fileName, "r", "VeteranRewards" );
	if( saveFile && saveFile.Length() > 0 )
	{
		var existingText = "";
		while( !saveFile.EOF() )
		{
			var rawLine = saveFile.ReadUntil( "\n" );
			if( rawLine != null && typeof rawLine != "undefined" )
			{
				existingText += rawLine;
				if( !saveFile.EOF() )
				{
					existingText += "\n";
				}
			}
		}

		saveFile.Close();
		saveFile.Free();

		existingText = VetRewardSanitizeJsonText( existingText );
		if( existingText != "" )
		{
			try
			{
				var parsedSave = JSON.parse( existingText );
				if( parsedSave && typeof parsedSave == "object" )
				{
					saveData = parsedSave;
				}
			}
			catch( error )
			{
				Console.Warning( "Veteran rewards: Failed to parse existing " + fileName + ": " + error );
			}
		}
	}
	else if( saveFile )
	{
		saveFile.Free();
	}

	if( !saveData.characters )
	{
		saveData.characters = {};
	}

	saveData.version = 1;
	saveData.characters[playerKey] = characterSave;

	var fileText = "";
	try
	{
		fileText = JSON.stringify( saveData, null, "\t" );
	}
	catch( error )
	{
		Console.Warning( "Veteran rewards: Failed to serialize " + fileName + ": " + error );
		return false;
	}

	saveFile = new UOXCFile();
	saveFile.Open( fileName, "w", "VeteranRewards" );
	if( !saveFile )
	{
		return false;
	}

	saveFile.Write( fileText + "\n" );
	saveFile.Close();
	saveFile.Free();
	return true;
}

/** @type { ( player: Character ) => any } */
function CreateDefaultVeteranRewardSave( player )
{
	var playerSerial = 0;
	if( ValidateObject( player ) )
	{
		playerSerial = player.serial;
	}

	return {
		serial: playerSerial,
		rewardsChosen: 0,
		testLevel: -1,
		history: []
	};
}

/** @type { ( player: Character, characterSave: any ) => any } */
function NormalizeVeteranRewardSave( player, characterSave )
{
	if( !characterSave || typeof characterSave != "object" )
	{
		characterSave = CreateDefaultVeteranRewardSave( player );
	}

	characterSave.serial = ValidateObject( player ) ? player.serial : ( parseInt( characterSave.serial, 10 ) || 0 );
	characterSave.rewardsChosen = parseInt( characterSave.rewardsChosen, 10 );
	if( isNaN( characterSave.rewardsChosen ) || characterSave.rewardsChosen < 0 )
	{
		characterSave.rewardsChosen = 0;
	}

	characterSave.testLevel = parseInt( characterSave.testLevel, 10 );
	if( isNaN( characterSave.testLevel ) )
	{
		characterSave.testLevel = -1;
	}

	if( !characterSave.history || !VetRewardIsArray( characterSave.history ) )
	{
		characterSave.history = [];
	}

	return characterSave;
}

/** @type { ( player: Character ) => number } */
function GetVeteranRewardLevel( player )
{
	if( !ValidateObject( player ) || !player.account )
	{
		return 0;
	}

	var characterSave = ReadVeteranRewardSave( player );
	if( characterSave.testLevel >= 0 )
	{
		return characterSave.testLevel;
	}

	var accountAgeDays = GetVeteranAccountAgeDays( player );
	var rewardDays = GetVeteranRewardDaysPerReward();
	var rewardLevel = Math.floor( accountAgeDays / rewardDays );
	if( isNaN( rewardLevel ) || rewardLevel < 0 )
	{
		rewardLevel = 0;
	}

	return rewardLevel;
}

/** @type { () => number } */
function GetVeteranRewardDaysPerReward()
{
	var rewardDays = parseInt( vetRewardDaysPerReward, 10 );
	if( isNaN( rewardDays ) || rewardDays <= 0 )
	{
		rewardDays = 90;
	}

	return rewardDays;
}

/** @type { ( player: Character ) => number } */
function GetVeteranAccountAgeDays( player )
{
	if( !ValidateObject( player ) || !player.account )
	{
		return 0;
	}

	var accountAgeMode = String( vetRewardAccountAgeMode || "playtime" ).toLowerCase();

	if( accountAgeMode == "firstlogin" )
	{
		return GetVeteranAccountAgeDaysFromFirstLogin( player );
	}

	if( accountAgeMode == "auto" )
	{
		var firstLoginDays = GetVeteranAccountAgeDaysFromFirstLogin( player );
		if( firstLoginDays > 0 )
		{
			return firstLoginDays;
		}

		return GetVeteranAccountAgeDaysFromPlayTime( player );
	}

	return GetVeteranAccountAgeDaysFromPlayTime( player );
}

/** @type { ( player: Character ) => number } */
function GetVeteranAccountAgeDaysFromPlayTime( player )
{
	if( !ValidateObject( player ) || !player.account )
	{
		return 0;
	}

	var totalPlayTime = parseInt( player.account.totalPlayTime, 10 );
	if( isNaN( totalPlayTime ) || totalPlayTime <= 0 )
	{
		return 0;
	}

	return Math.floor( totalPlayTime / 1440 );
}

/** @type { ( player: Character ) => number } */
function GetVeteranAccountAgeDaysFromFirstLogin( player )
{
	if( !ValidateObject( player ) || !player.account )
	{
		return 0;
	}

	var firstLoginMinutes = parseInt( player.account.firstLogin, 10 );
	if( isNaN( firstLoginMinutes ) || firstLoginMinutes <= 0 )
	{
		return 0;
	}

	var currentMinutes = Math.floor( Date.now() / 60000 );
	var accountAgeMinutes = currentMinutes - firstLoginMinutes;

	if( isNaN( accountAgeMinutes ) || accountAgeMinutes <= 0 )
	{
		return 0;
	}

	return Math.floor( accountAgeMinutes / 1440 );
}

/** @type { ( player: Character ) => number } */
function GetVeteranAccountAgeHours( player )
{
	var accountAgeDays = GetVeteranAccountAgeDays( player );
	return accountAgeDays * 24;
}

/** @type { ( player: Character ) => number } */
function GetVeteranDaysUntilNextReward( player )
{
	var accountAgeDays = GetVeteranAccountAgeDays( player );
	var rewardDays = GetVeteranRewardDaysPerReward();
	var daysIntoCurrentPeriod = accountAgeDays % rewardDays;
	var daysLeft = rewardDays - daysIntoCurrentPeriod;

	if( daysLeft == rewardDays && accountAgeDays > 0 )
	{
		return rewardDays;
	}

	return daysLeft;
}

/** @type { ( player: Character ) => number } */
function GetVeteranRewardMax( player )
{
	var rewardLevel = GetVeteranRewardLevel( player );
	if( rewardLevel <= 0 )
	{
		return 0;
	}

	return rewardLevel;
}

/** @type { ( player: Character ) => number } */
function GetVeteranRewardsChosen( player )
{
	return ReadVeteranRewardSave( player ).rewardsChosen;
}

/** @type { ( player: Character ) => number } */
function GetVeteranRewardsLeft( player )
{
	var rewardsLeft = GetVeteranRewardMax( player ) - GetVeteranRewardsChosen( player );
	if( rewardsLeft < 0 )
	{
		rewardsLeft = 0;
	}

	return rewardsLeft;
}

/** @type { ( player: Character, testLevel: number|string ) => boolean } */
function SetVeteranRewardTestLevel( player, testLevel )
{
	if( !ValidateObject( player ) )
	{
		return false;
	}

	testLevel = parseInt( testLevel, 10 );
	if( isNaN( testLevel ) || testLevel < 0 )
	{
		testLevel = 0;
	}

	var characterSave = ReadVeteranRewardSave( player );
	characterSave.testLevel = testLevel;
	return WriteVeteranRewardSave( player, characterSave );
}

/** @type { ( player: Character ) => boolean } */
function ClearVeteranRewardSave( player )
{
	if( !ValidateObject( player ) )
	{
		return false;
	}

	return WriteVeteranRewardSave( player, CreateDefaultVeteranRewardSave( player ) );
}

/** @type { ( rewardDays: number|string ) => string } */
function GetVeteranRewardIntervalText( rewardDays )
{
	rewardDays = parseInt( rewardDays, 10 );
	if( isNaN( rewardDays ) || rewardDays <= 0 )
	{
		rewardDays = 90;
	}

	if( rewardDays == 30 )
	{
		return "month";
	}
	if( rewardDays == 60 )
	{
		return "two months";
	}
	if( rewardDays == 90 )
	{
		return "three months";
	}
	if( rewardDays == 180 )
	{
		return "six months";
	}
	if( rewardDays == 365 )
	{
		return "year";
	}

	return rewardDays + " day" + ( rewardDays == 1 ? "" : "s" );
}

/** @type { ( categoryIndex: number|string, entryIndex: number|string ) => number } */
function GetVeteranRewardButtonID( categoryIndex, entryIndex )
{
	categoryIndex = parseInt( categoryIndex, 10 );
	entryIndex = parseInt( entryIndex, 10 );

	if( isNaN( categoryIndex ) || categoryIndex < 0 )
	{
		categoryIndex = 0;
	}
	if( isNaN( entryIndex ) || entryIndex < 0 )
	{
		entryIndex = 0;
	}

	return 2 + ( entryIndex * 20 ) + categoryIndex;
}

/** @type { ( buttonID: number|string ) => any|null } */
function DecodeVeteranRewardButtonID( buttonID )
{
	buttonID = parseInt( buttonID, 10 );
	if( isNaN( buttonID ) || buttonID < 2 )
	{
		return null;
	}

	var decodedButton = buttonID - 2;
	return {
		categoryIndex: decodedButton % 20,
		entryIndex: Math.floor( decodedButton / 20 )
	};
}

/** @type { ( categoryIndex: number|string, entryIndex: number|string ) => any|null } */
function GetVeteranRewardByCategoryEntry( categoryIndex, entryIndex )
{
	var table = GetVeteranRewardTable();
	categoryIndex = parseInt( categoryIndex, 10 );
	entryIndex = parseInt( entryIndex, 10 );

	if( isNaN( categoryIndex ) || categoryIndex < 0 || isNaN( entryIndex ) || entryIndex < 0 )
	{
		return null;
	}

	if( !table.categories || !table.categories[categoryIndex] || !table.categories[categoryIndex].entries )
	{
		return null;
	}

	var entry = table.categories[categoryIndex].entries[entryIndex];
	if( !entry || !entry.section )
	{
		return null;
	}

	var rewards = GetVeteranRewardFlatList();
	for( var rewardIndex = 0; rewardIndex < rewards.length; rewardIndex++ )
	{
		var reward = rewards[rewardIndex];
		if( reward.categoryIndex == categoryIndex && reward.entryIndex == entryIndex )
		{
			return reward;
		}
	}

	return null;
}

/** @type { ( pUser: Character, rewardEntry: any ) => boolean } */
function HasVeteranRewardAccess( pUser, rewardEntry )
{
	if( !ValidateObject( pUser ) || !rewardEntry )
	{
		return false;
	}

	var requiredLevel = parseInt( rewardEntry.level, 10 );
	if( isNaN( requiredLevel ) || requiredLevel <= 0 )
	{
		requiredLevel = 1;
	}

	return ( GetVeteranRewardLevel( pUser ) >= requiredLevel );
}

/** @type { ( pUser: Character, category: any ) => boolean } */
function HasVeteranRewardCategoryAccess( pUser, category )
{
	if( !category || !category.entries )
	{
		return false;
	}

	for( var entryIndex = 0; entryIndex < category.entries.length; entryIndex++ )
	{
		if( HasVeteranRewardAccess( pUser, category.entries[entryIndex] ) )
		{
			return true;
		}
	}

	return false;
}


/** @type { ( pUser: Character, category: any ) => number } */
function GetVeteranRewardCategoryPageCount( pUser, category )
{
	if( !category || !category.entries )
	{
		return 1;
	}

	var availableCount = 0;
	for( var entryIndex = 0; entryIndex < category.entries.length; entryIndex++ )
	{
		if( HasVeteranRewardAccess( pUser, category.entries[entryIndex] ) )
		{
			availableCount++;
		}
	}

	var pageCount = Math.ceil( availableCount / 24 );
	if( pageCount <= 0 )
	{
		pageCount = 1;
	}

	return pageCount;
}

/** @type { ( pUser: Character ) => void } */
function ShowVeteranRewardNotice( pUser )
{
	if( !ValidateObject( pUser ) )
	{
		return;
	}

	var socket = pUser.socket;
	if( socket == null )
	{
		return;
	}

	var rewardsLeft = GetVeteranRewardsLeft( pUser );
	if( rewardsLeft <= 0 )
	{
		socket.SysMessage( "You do not have any veteran rewards available." );
		return;
	}

	pUser.SetTempTag( "VeteranRewardGumpMode", "notice" );
	pUser.SetTempTag( "VeteranRewardConfirmID", null );

	var noticeGump = new Gump;
	noticeGump.AddPage( 0 );
	noticeGump.AddBackground( 10, 10, 500, 135, 2600 );
	noticeGump.AddHTMLGump( 52, 35, 420, 55, true, true, "You have reward items available.<br>Click 'ok' below to get the selection menu or 'cancel' to be prompted upon your next login." );

	noticeGump.AddButton( 60, 95, 4005, 4007, 1, 0, 1 );
	noticeGump.AddHTMLGump( 95, 96, 150, 35, false, false, "Ok" );

	noticeGump.AddButton( 285, 95, 4017, 4019, 1, 0, 0 );
	noticeGump.AddHTMLGump( 320, 96, 150, 35, false, false, "Cancel" );

	noticeGump.Send( socket );
	noticeGump.Free();
}

/** @type { ( pUser: Character ) => void } */
function ShowVeteranRewardCategories( pUser )
{
	ShowVeteranRewardChoiceGump( pUser );
}

/** @type { ( pUser: Character ) => void } */
function ShowVeteranRewardChoiceGump( pUser )
{
	if( !ValidateObject( pUser ) )
	{
		return;
	}

	var socket = pUser.socket;
	if( socket == null )
	{
		return;
	}

	var table = GetVeteranRewardTable();
	if( !table || !table.categories )
	{
		socket.SysMessage( "Veteran reward table is not loaded." );
		return;
	}

	var rewardsChosen = GetVeteranRewardsChosen( pUser );
	var rewardsMax = GetVeteranRewardMax( pUser );
	var rewardsAvailable = GetVeteranRewardsLeft( pUser );
	var rewardDays = GetVeteranRewardDaysPerReward();
	var intervalText = GetVeteranRewardIntervalText( rewardDays );

	pUser.SetTempTag( "VeteranRewardGumpMode", "choice" );
	pUser.SetTempTag( "VeteranRewardConfirmID", null );

	var choiceGump = new Gump;
	choiceGump.AddPage( 0 );
	choiceGump.AddBackground( 10, 10, 600, 450, 2600 );
	choiceGump.AddButton( 530, 415, 4017, 4019, 1, 0, 0 );
	choiceGump.AddHTMLGump( 565, 416, 80, 20, false, false, "Close" );
	choiceGump.AddPageButton( 60, 415, 4014, 4016, 1 );
	choiceGump.AddHTMLGump( 95, 416, 200, 20, false, false, "Main Menu" );

	choiceGump.AddPage( 1 );
	choiceGump.AddHTMLGump( 60, 35, 500,70, true, true,
		"<B>Ultima Online Rewards Program</B><BR>" +
		"Thank you for being a part of the Ultima Online community for a full " + intervalText + ". " +
		"As a token of our appreciation, you may select from the following in-game reward items listed below. " +
		"The gift items will be attributed to the character you have logged-in with on the shard you are on when you choose the item. " +
		"The number of rewards you are entitled to are listed below and are for your entire account." );

	choiceGump.AddHTMLGump( 60, 105, 300, 35, false, false, "Your current total of rewards to choose:" );
	choiceGump.AddText( 370, 107, 50, String( rewardsAvailable ));
	choiceGump.AddHTMLGump( 60, 140, 300, 35, false, false, "You have already chosen:" );
	choiceGump.AddText( 370, 142, 50, String( rewardsChosen ));
	choiceGump.AddHTMLGump( 60, 165, 300, 35, false, false, "Your reward limit on this account:" );
	choiceGump.AddText( 370, 167, 50, String( rewardsMax ));

	var categoryPageStarts = [];
	var currentPage = 2;
	var visibleCategoryIndex = 0;

	for( var categoryIndex = 0; categoryIndex < table.categories.length; categoryIndex++ )
	{
		var category = table.categories[categoryIndex];
		categoryPageStarts[categoryIndex] = currentPage;

		var categoryPageCount = GetVeteranRewardCategoryPageCount( pUser, category );
		if( HasVeteranRewardCategoryAccess( pUser, category ))
		{
			var categoryX = 100;
			var categoryY = 205 + ( visibleCategoryIndex * 30 );
			choiceGump.AddPageButton( categoryX, categoryY, 4005, 4007, currentPage );
			choiceGump.AddHTMLGump( categoryX + 35, categoryY + 1, 300, 22, false, false, category.name || "Veteran Rewards" );
			visibleCategoryIndex++;
		}

		currentPage += categoryPageCount;
	}

	if( visibleCategoryIndex == 0 )
	{
		choiceGump.AddHTMLGump( 100, 210, 350, 60, true, true, "No rewards are available for your current reward level." );
	}

	for( var renderCategoryIndex = 0; renderCategoryIndex < table.categories.length; renderCategoryIndex++ )
	{
		RenderVeteranRewardCategoryPages( choiceGump, pUser, table.categories[renderCategoryIndex], renderCategoryIndex, categoryPageStarts[renderCategoryIndex] );
	}

	choiceGump.Send( socket );
	choiceGump.Free();
}

/** @type { ( rewardGump: Gump, pUser: Character, category: any, categoryIndex: number, startPage: number ) => void } */
function RenderVeteranRewardCategoryPages( rewardGump, pUser, category, categoryIndex, startPage )
{
	if( !category || !category.entries )
	{
		rewardGump.AddPage( startPage );
		return;
	}

	var page = startPage;
	var visibleIndex = 0;
	rewardGump.AddPage( page );
	rewardGump.AddHTMLGump( 55, 40, 500, 25, false, false, "<B>" + ( category.name || "Veteran Rewards" ) + "</B>" );

	for( var entryIndex = 0; entryIndex < category.entries.length; entryIndex++ )
	{
		var rewardEntry = category.entries[entryIndex];
		if( !HasVeteranRewardAccess( pUser, rewardEntry ))
		{
			continue;
		}

		if( visibleIndex > 0 && ( visibleIndex % 24 ) == 0 )
		{
			rewardGump.AddPageButton( 305, 415, 0x0FA5, 0x0FA7, page + 1 );
			rewardGump.AddHTMLGump( 340, 416, 150, 20, false, false, "Next page" );
			page++;
			rewardGump.AddPage( page );
			rewardGump.AddHTMLGump( 55, 40, 500, 25, false, false, "<B>" + ( category.name || "Veteran Rewards" ) + "</B>" );
			rewardGump.AddPageButton( 270, 415, 0x0FAE, 0x0FB0, page - 1 );
			rewardGump.AddHTMLGump( 165, 416, 150, 20, false, false, "Previous page" );
		}

		var pageVisibleIndex = visibleIndex % 24;
		var column = Math.floor( pageVisibleIndex / 12 );
		var row = pageVisibleIndex % 12;
		var buttonX = 55 + ( column * 260 );
		var textX = 82 + ( column * 260 );
		var lineY = 80 + ( row * 25 );

		rewardGump.AddButton( buttonX, lineY, 5540, 5541, 1, 0, GetVeteranRewardButtonID( categoryIndex, entryIndex ));
		rewardGump.AddHTMLGump( textX, lineY, 235, 20, false, false, rewardEntry.name || rewardEntry.section || "Veteran Reward" );
		visibleIndex++;
	}

	if( visibleIndex == 0 )
	{
		rewardGump.AddHTMLGump( 55, 80, 400, 40, true, true, "No rewards are available in this category yet." );
	}
}

/** @type { ( pUser: Character, categoryIndex: number|string ) => void } */
function ShowVeteranRewardCategory( pUser, categoryIndex )
{
	ShowVeteranRewardChoiceGump( pUser );
}

/** @type { ( pUser: Character, rewardID: number|string ) => void } */
function ShowVeteranRewardConfirm( pUser, rewardID )
{
	if( !ValidateObject( pUser ))
	{
		return;
	}

	var socket = pUser.socket;
	if( socket == null )
	{
		return;
	}

	var reward = GetVeteranRewardByID( rewardID );
	if( !reward )
	{
		socket.SysMessage( "That veteran reward does not exist." );
		ShowVeteranRewardChoiceGump( pUser );
		return;
	}

	pUser.SetTempTag( "VeteranRewardGumpMode", "confirm" );
	pUser.SetTempTag( "VeteranRewardConfirmID", reward.rewardID );

	var confirmGump = new Gump;
	confirmGump.AddPage( 0 );
	confirmGump.AddBackground( 10, 10, 500, 300, 2600 );
	confirmGump.AddHTMLGump( 30, 55, 300, 35, false, false, "You have selected:" );
	confirmGump.AddHTMLGump( 335, 55, 150, 35, false, false, reward.name || reward.section );
	confirmGump.AddHTMLGump( 30, 95, 300, 35, false, false, "This will be assigned to this character:" );
	confirmGump.AddText( 335, 95, 0, pUser.name );
	confirmGump.AddHTMLGump( 35, 160, 450, 90, true, true, "Are you sure you wish to select this reward for this character? You will not be able to transfer this reward to another character on another shard. Click 'ok' below to confirm your selection or 'cancel' to go back to the selection screen." );

	confirmGump.AddButton( 60, 265, 4005, 4007, 1, 0, 1 );
	confirmGump.AddHTMLGump( 95, 266, 150, 35, false, false, "Ok" );
	confirmGump.AddButton( 295, 265, 4017, 4019, 1, 0, 0 );
	confirmGump.AddHTMLGump( 330, 266, 150, 35, false, false, "Cancel" );
	confirmGump.Send( socket );
	confirmGump.Free();
}

/** @type { ( pUser: Character, rewardID: number|string ) => boolean } */
function ClaimVeteranReward( pUser, rewardID )
{
	if( !ValidateObject( pUser ) )
	{
		return false;
	}

	var socket = pUser.socket;
	if( socket == null )
	{
		return false;
	}

	var reward = GetVeteranRewardByID( rewardID );
	if( !reward )
	{
		socket.SysMessage( "That veteran reward does not exist." );
		return false;
	}

	if( GetVeteranRewardsLeft( pUser ) <= 0 )
	{
		socket.SysMessage( "You do not have any veteran reward points available." );
		return false;
	}

	if( reward.level > GetVeteranRewardLevel( pUser ))
	{
		socket.SysMessage( "Your account is not old enough for that veteran reward." );
		return false;
	}

	var pack = pUser.pack;
	if( !ValidateObject( pack ) || pack.totalItemCount >= pack.maxItems )
	{
		socket.SysMessage( GetDictionaryEntry( 1819, socket.language ));
		return false;
	}

	var rewardColor = GetVeteranRewardColor( reward );
	var rewardItem = null;

	if( rewardColor > 0 )
	{
		rewardItem = CreateDFNItem( socket, pUser, reward.section, reward.amount, "ITEM", true, rewardColor );
	}
	else
	{
		rewardItem = CreateDFNItem( socket, pUser, reward.section, reward.amount, "ITEM", true );
	}

	if( !ValidateObject( rewardItem ))
	{
		socket.SysMessage( "Unable to create veteran reward: " + reward.section );
		return false;
	}

	if( rewardColor > 0 )
	{
		rewardItem.color = rewardColor;
	}

	rewardItem.name = reward.name;
	rewardItem.SetTag( "VetRewardItem", 1 );
	rewardItem.SetTag( "VetRewardLevel", reward.level );
	rewardItem.SetTag( "VetRewardOwner", pUser.account.id );

	var characterSave = ReadVeteranRewardSave( pUser );
	characterSave.rewardsChosen++;
	characterSave.history.push({ rewardID: reward.rewardID, name: reward.name, section: reward.section, level: reward.level, itemSerial: rewardItem.serial, claimedAt: Date.now() });

	if( !WriteVeteranRewardSave( pUser, characterSave ) )
	{
		rewardItem.Delete();
		socket.SysMessage( "Unable to save veteran reward progress." );
		return false;
	}

	pUser.SoundEffect( 0x5B5, true );
	socket.SysMessage( "You have claimed a veteran reward: " + reward.name + "." );
	return true;
}

/** @type { ( reward: any ) => number } */
function GetVeteranRewardColor( reward )
{
	if( !reward || reward.color == null || typeof reward.color == "undefined" )
	{
		return 0;
	}

	var rewardColor = 0;

	if( typeof reward.color == "number" )
	{
		rewardColor = reward.color;
	}
	else
	{
		var colorText = String( reward.color );

		if( colorText.indexOf( "0x" ) == 0 || colorText.indexOf( "0X" ) == 0 )
		{
			rewardColor = parseInt( colorText, 16 );
		}
		else
		{
			rewardColor = parseInt( colorText, 10 );
		}
	}

	if( isNaN( rewardColor ) || rewardColor < 0 )
	{
		rewardColor = 0;
	}

	return rewardColor;
}

/** @type { ( pUser: Character, rewardItem: Item ) => boolean } */
function CanUseVeteranRewardItem( pUser, rewardItem )
{
	if( !ValidateObject( pUser ) || !ValidateObject( rewardItem ))
	{
		return false;
	}

	var socket = pUser.socket;
	if( socket == null )
	{
		return false;
	}

	if( !rewardItem.GetTag( "VetRewardItem" ) )
	{
		return true;
	}

	var ownerAccountID = parseInt( rewardItem.GetTag( "VetRewardOwner" ), 10 );
	if( !isNaN( ownerAccountID ) && pUser.account && ownerAccountID != pUser.account.id )
	{
		socket.SysMessage( "You are not the owner of that veteran reward." );
		return false;
	}

	var requiredLevel = parseInt( rewardItem.GetTag( "VetRewardLevel" ), 10 );
	if( isNaN( requiredLevel ) || requiredLevel <= 0 )
	{
		return true;
	}

	if( GetVeteranRewardLevel( pUser ) < requiredLevel )
	{
		socket.SysMessage( "Your account is not old enough to use that veteran reward." );
		return false;
	}

	return true;
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( pSock, pButton, gumpData )
{
	if( pSock == null )
	{
		return;
	}

	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ))
	{
		return;
	}

	var gumpMode = String( pUser.GetTempTag( "VeteranRewardGumpMode" ) || "" );

	if( gumpMode == "notice" )
	{
		if( pButton == 1 )
		{
			ShowVeteranRewardChoiceGump( pUser );
		}
		return;
	}

	if( gumpMode == "confirm" )
	{
		if( pButton == 1 )
		{
			var confirmRewardID = parseInt( pUser.GetTempTag( "VeteranRewardConfirmID" ), 10 );
			pUser.SetTempTag( "VeteranRewardConfirmID", null );

			if( !isNaN( confirmRewardID ) && ClaimVeteranReward( pUser, confirmRewardID ))
			{
				if( GetVeteranRewardsLeft( pUser ) > 0 )
				{
					ShowVeteranRewardNotice( pUser );
				}
				else
				{
					pUser.SetTempTag( "VeteranRewardGumpMode", null );
				}
			}
		}
		else
		{
			ShowVeteranRewardChoiceGump( pUser );
		}
		return;
	}

	if( gumpMode == "choice" )
	{
		if( pButton == 0 )
		{
			if( GetVeteranRewardsLeft( pUser ) > 0 )
			{
				ShowVeteranRewardNotice( pUser );
			}
			return;
		}

		var decodedButton = DecodeVeteranRewardButtonID( pButton );
		if( decodedButton == null )
		{
			return;
		}

		var reward = GetVeteranRewardByCategoryEntry( decodedButton.categoryIndex, decodedButton.entryIndex );
		if( !reward )
		{
			return;
		}

		if( !HasVeteranRewardAccess( pUser, reward ))
		{
			return;
		}

		ShowVeteranRewardConfirm( pUser, reward.rewardID );
		return;
	}

	if( pButton == 1 )
	{
		ShowVeteranRewardChoiceGump( pUser );
	}
}

/** @type { ( fileText: string ) => string } */
function VetRewardSanitizeJsonText( fileText )
{
	if( fileText == null || typeof fileText == "undefined" )
	{
		return "";
	}

	return String( fileText ).replace( /[^\x20-\x7E\r\n\t]/g, "" ).trim();
}

/** @type { ( value: any ) => boolean } */
function VetRewardIsArray( value )
{
	return Object.prototype.toString.call( value ) == "[object Array]";
}