/// <reference path="../../../definitions.d.ts" />
// @ts-check

var vetRewardTable = null;
var vetRewardTableLoaded = false;
var vetRewardTableLoadError = false;

// Number of account playtime days required for each veteran reward point.
// 90 = about 3 months, 180 = about 6 months, 365 = about 1 year.
var vetRewardDaysPerReward = 90;

function HandleVetRewardLogin( sockPlayer, pChar )
{
	VetRewardOnLogin( sockPlayer, pChar );
}

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

function VetRewardMenu( pUser )
{
	ShowVeteranRewardNotice( pUser );
}

function ReloadVeteranRewardTable()
{
	vetRewardTable = null;
	vetRewardTableLoaded = false;
	vetRewardTableLoadError = false;
	LoadVeteranRewardTable();
}

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

			rewards.push({
				rewardID: rewardID,
				categoryIndex: categoryIndex,
				entryIndex: entryIndex,
				categoryName: category.name,
				name: entry.name || entry.section,
				section: entry.section,
				level: parseInt( entry.level, 10 ) || 1,
				color: entry.color,
				amount: parseInt( entry.amount, 10 ) || 1
			});

			rewardID++;
		}
	}

	return rewards;
}

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

function GetVeteranRewardDaysPerReward()
{
	var rewardDays = parseInt( vetRewardDaysPerReward, 10 );
	if( isNaN( rewardDays ) || rewardDays <= 0 )
	{
		rewardDays = 90;
	}

	return rewardDays;
}

function GetVeteranAccountAgeDays( player )
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

function GetVeteranAccountAgeHours( player )
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

	return Math.floor( totalPlayTime / 60 );
}


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

function GetVeteranRewardMax( player )
{
	var rewardLevel = GetVeteranRewardLevel( player );
	if( rewardLevel <= 0 )
	{
		return 0;
	}

	return rewardLevel;
}

function GetVeteranRewardsChosen( player )
{
	return ReadVeteranRewardSave( player ).rewardsChosen;
}

function GetVeteranRewardsLeft( player )
{
	var rewardsLeft = GetVeteranRewardMax( player ) - GetVeteranRewardsChosen( player );
	if( rewardsLeft < 0 )
	{
		rewardsLeft = 0;
	}

	return rewardsLeft;
}

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

function ClearVeteranRewardSave( player )
{
	if( !ValidateObject( player ) )
	{
		return false;
	}

	return WriteVeteranRewardSave( player, CreateDefaultVeteranRewardSave( player ) );
}

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

	var rewardLevel = GetVeteranRewardLevel( pUser );
	var rewardsChosen = GetVeteranRewardsChosen( pUser );
	var rewardsMax = GetVeteranRewardMax( pUser );
	var rewardsLeft = GetVeteranRewardsLeft( pUser );
	var accountAgeDays = GetVeteranAccountAgeDays( pUser );
	var daysUntilNextReward = GetVeteranDaysUntilNextReward( pUser );

	var noticeGump = new Gump();
	noticeGump.AddPage( 0 );
	noticeGump.AddBackground( 40, 80, 430, 300, 5054 );
	noticeGump.AddText( 155, 105, 1152, "Veteran Rewards" );
	noticeGump.AddHTMLGump( 70, 135, 370, 115, true, true,
		"Your account has earned veteran reward points. These rewards are saved in a veteran reward save file for this character instead of character tags.<br><br>" +
		"Reward level: " + rewardLevel + "<br>" +
		"Account playtime days: " + accountAgeDays + "<br>" +
		"Reward every: " + GetVeteranRewardDaysPerReward() + " days<br>" +
		"Days until next reward: " + daysUntilNextReward + "<br>" +
		"Rewards chosen: " + rewardsChosen + " / " + rewardsMax + "<br>" +
		"Rewards available: " + rewardsLeft );

	if( rewardsLeft > 0 )
	{
		noticeGump.AddButton( 100, 300, 4005, 4007, 1, 0, 1 );
		noticeGump.AddText( 135, 300, 0, "Choose Reward" );
	}

	noticeGump.AddButton( 300, 300, 4017, 4019, 1, 0, 0 );
	noticeGump.AddText( 335, 300, 0, "Close" );
	noticeGump.Send( socket );
	noticeGump.Free();
}

function ShowVeteranRewardCategories( pUser )
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
	var rewardLevel = GetVeteranRewardLevel( pUser );
	var rewardsLeft = GetVeteranRewardsLeft( pUser );

	var categoryGump = new Gump();
	categoryGump.AddPage( 0 );
	categoryGump.AddBackground( 40, 60, 390, 420, 5054 );
	categoryGump.AddText( 145, 85, 1152, "Veteran Rewards" );
	categoryGump.AddText( 70, 115, 0, "Reward level: " + rewardLevel + "  Available: " + rewardsLeft );

	var buttonY = 150;
	for( var categoryIndex = 0; categoryIndex < table.categories.length; categoryIndex++ )
	{
		var category = table.categories[categoryIndex];
		if( !category )
		{
			continue;
		}

		if( !VeteranRewardCategoryHasAvailableEntries( pUser, categoryIndex ) )
		{
			continue;
		}

		categoryGump.AddButton( 70, buttonY, 4005, 4007, 1, 0, 1000 + categoryIndex );
		categoryGump.AddText( 105, buttonY, 0, category.name );
		buttonY += 32;
	}

	if( buttonY == 150 )
	{
		categoryGump.AddText( 70, 160, 0, "No rewards are available for your level." );
	}

	categoryGump.AddButton( 70, 430, 4017, 4019, 1, 0, 0 );
	categoryGump.AddText( 105, 430, 0, "Close" );
	categoryGump.Send( socket );
	categoryGump.Free();
}

function VeteranRewardCategoryHasAvailableEntries( pUser, categoryIndex )
{
	var table = GetVeteranRewardTable();
	if( !table.categories || !table.categories[categoryIndex] )
	{
		return false;
	}

	var rewardLevel = GetVeteranRewardLevel( pUser );
	var category = table.categories[categoryIndex];
	for( var entryIndex = 0; entryIndex < category.entries.length; entryIndex++ )
	{
		var entry = category.entries[entryIndex];
		if( entry && ( parseInt( entry.level, 10 ) || 1 ) <= rewardLevel )
		{
			return true;
		}
	}

	return false;
}

function ShowVeteranRewardCategory( pUser, categoryIndex )
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
	categoryIndex = parseInt( categoryIndex, 10 );
	if( isNaN( categoryIndex ) || categoryIndex < 0 || !table.categories[categoryIndex] )
	{
		ShowVeteranRewardCategories( pUser );
		return;
	}

	var category = table.categories[categoryIndex];
	var rewards = GetVeteranRewardFlatList();
	var rewardLevel = GetVeteranRewardLevel( pUser );

	var rewardGump = new Gump();
	rewardGump.AddPage( 0 );
	rewardGump.AddBackground( 40, 60, 480, 500, 5054 );
	rewardGump.AddText( 170, 85, 1152, category.name );
	rewardGump.AddText( 70, 115, 0, "Reward level: " + rewardLevel + "  Available: " + GetVeteranRewardsLeft( pUser ) );

	var rewardY = 145;
	for( var rewardIndex = 0; rewardIndex < rewards.length; rewardIndex++ )
	{
		var reward = rewards[rewardIndex];
		if( reward.categoryIndex != categoryIndex )
		{
			continue;
		}

		if( reward.level > rewardLevel )
		{
			continue;
		}

		rewardGump.AddButton( 70, rewardY, 4005, 4007, 1, 0, 2000 + reward.rewardID );
		rewardGump.AddText( 105, rewardY, 0, reward.name + " (Level " + reward.level + ")" );
		rewardY += 25;

		if( rewardY > 505 )
		{
			break;
		}
	}

	if( rewardY == 145 )
	{
		rewardGump.AddText( 70, 155, 0, "No rewards are available in this category yet." );
	}

	rewardGump.AddButton( 70, 525, 4014, 4016, 1, 0, 1 );
	rewardGump.AddText( 105, 525, 0, "Back" );
	rewardGump.Send( socket );
	rewardGump.Free();
}

function ShowVeteranRewardConfirm( pUser, rewardID )
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

	var reward = GetVeteranRewardByID( rewardID );
	if( !reward )
	{
		socket.SysMessage( "That veteran reward does not exist." );
		ShowVeteranRewardCategories( pUser );
		return;
	}

	var confirmGump = new Gump();
	confirmGump.AddPage( 0 );
	confirmGump.AddBackground( 50, 100, 420, 260, 5054 );
	confirmGump.AddText( 165, 125, 1152, "Confirm Reward" );
	confirmGump.AddHTMLGump( 80, 160, 360, 90, true, true,
		"Choose this veteran reward?<br><br>" +
		"Reward: " + reward.name + "<br>" +
		"Required level: " + reward.level );
	confirmGump.AddButton( 105, 295, 4005, 4007, 1, 0, 3000 + reward.rewardID );
	confirmGump.AddText( 140, 295, 0, "Confirm" );
	confirmGump.AddButton( 285, 295, 4017, 4019, 1, 0, 0 );
	confirmGump.AddText( 320, 295, 0, "Cancel" );
	confirmGump.Send( socket );
	confirmGump.Free();
}

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

	if( reward.level > GetVeteranRewardLevel( pUser ) )
	{
		socket.SysMessage( "Your account is not old enough for that veteran reward." );
		return false;
	}

	var pack = pUser.pack;
	if( !ValidateObject( pack ) || pack.totalItemCount >= pack.maxItems )
	{
		socket.SysMessage( GetDictionaryEntry( 1819, socket.language ) );
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

	if( !ValidateObject( rewardItem ) )
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
	characterSave.history.push({
		rewardID: reward.rewardID,
		name: reward.name,
		section: reward.section,
		level: reward.level,
		itemSerial: rewardItem.serial,
		claimedAt: Date.now()
	});

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

function CanUseVeteranRewardItem( pUser, rewardItem )
{
	if( !ValidateObject( pUser ) || !ValidateObject( rewardItem ) )
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

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
	{
		return;
	}

	if( pButton == 0 )
	{
		return;
	}

	if( pButton == 1 )
	{
		ShowVeteranRewardCategories( pUser );
		return;
	}

	if( pButton >= 1000 && pButton < 2000 )
	{
		ShowVeteranRewardCategory( pUser, pButton - 1000 );
		return;
	}

	if( pButton >= 2000 && pButton < 3000 )
	{
		ShowVeteranRewardConfirm( pUser, pButton - 2000 );
		return;
	}

	if( pButton >= 3000 && pButton < 4000 )
	{
		ClaimVeteranReward( pUser, pButton - 3000 );
		return;
	}
}

function VetRewardSanitizeJsonText( fileText )
{
	if( fileText == null || typeof fileText == "undefined" )
	{
		return "";
	}

	return String( fileText ).replace( /[^\x20-\x7E\r\n\t]/g, "" ).trim();
}

function VetRewardIsArray( value )
{
	return Object.prototype.toString.call( value ) == "[object Array]";
}
