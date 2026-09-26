// Community Collections
// Script ID: 7700.
// Version 1.0

var COLLECTION_SCRIPT_ID = 7700;
var COLLECTION_DECAY_MS = 86400000;
var COLLECTION_MAX_DONATE_AMOUNT = 60000;
var COLLECTION_TITLE_PREFIX = "Title: ";
var COLLECTION_PET_HOLDING_X = 8000;
var COLLECTION_PET_HOLDING_Y = 8000;
var COLLECTION_PET_HOLDING_Z = 0;
var COLLECTION_DELETE_DONATED_PETS = true;

var collectionVersion = 2;
var collectionPlayerVersion = 1;
var collectionWorldVersion = 1;
var collectionRegistry = {};
var collectionConfigs = [];
var collectionRegistryLoaded = false;
var collectionRegistryLoadError = false;

function FindCollectionConfig( collectionId )
{
	if( !collectionRegistryLoaded )
	{
		LoadCollectionRegistry();
	}

	return collectionRegistry[String( collectionId )] || null;
}

function CollectionList( collectionId )
{
	if( !collectionRegistryLoaded )
	{
		LoadCollectionRegistry();
	}

	if( !collectionId )
	{
		return collectionRegistry;
	}

	return collectionRegistry[String( collectionId )] || null;
}

function ReloadCollectionRegistry()
{
	collectionRegistry = {};
	collectionConfigs = [];
	collectionRegistryLoaded = false;
	collectionRegistryLoadError = false;
	LoadCollectionRegistry();
}

function LoadCollectionRegistry()
{
	collectionRegistry = {};
	collectionConfigs = [];
	collectionRegistryLoaded = false;
	collectionRegistryLoadError = false;

	var indexFile = new UOXCFile();
	indexFile.Open( "collection_index.json", "r", "community_collections", true );

	if( indexFile == null || indexFile.Length() < 0 )
	{
		Console.Error( "Community collections: Unable to open js/jsdata/community_collections/collection_index.json" );
		collectionRegistryLoadError = true;
		return;
	}

	var fileText = ReadWholeFile( indexFile );
	fileText = SanitizeJsonText( fileText );

	if( fileText == "" )
	{
		Console.Error( "Community collections: collection_index.json is empty." );
		collectionRegistryLoadError = true;
		return;
	}

	var indexData = null;
	try
	{
		indexData = JSON.parse( fileText );
	}
	catch( error )
	{
		Console.Error( "Community collections: Failed to parse collection_index.json: " + error );
		collectionRegistryLoadError = true;
		return;
	}

	if( !indexData || typeof indexData != "object" || IsArrayValue( indexData ) )
	{
		Console.Error( "Community collections: collection_index.json must be an object map." );
		collectionRegistryLoadError = true;
		return;
	}

	for( var key in indexData )
	{
		if( !indexData.hasOwnProperty( key ) )
		{
			continue;
		}

		if( key.indexOf( "comment" ) == 0 )
		{
			continue;
		}

		var fileName = String( indexData[key] );
		var collectionData = LoadSingleCollectionFile( fileName );
		if( !collectionData )
		{
			Console.Warning( "Community collections: Failed to load collection file: " + fileName );
			continue;
		}

		if( !collectionData.id )
		{
			Console.Warning( "Community collections: Missing collection id in file: " + fileName );
			continue;
		}

		var collectionId = String( collectionData.id );
		collectionData.sourceFile = fileName;
		NormalizeCollectionDefaults( collectionData );
		collectionRegistry[collectionId] = collectionData;
		collectionConfigs.push( collectionData );
	}

	Console.Print( "Community collections: Loaded " + collectionConfigs.length + " collections.\n" );
	collectionRegistryLoaded = true;
	collectionRegistryLoadError = false;
}

function LoadSingleCollectionFile( fileName )
{
	var dataFile = new UOXCFile();
	dataFile.Open( fileName, "r", "community_collections", true );

	if( dataFile == null || dataFile.Length() < 0 )
	{
		return null;
	}

	var fileText = ReadWholeFile( dataFile );
	fileText = SanitizeJsonText( fileText );
	if( fileText == "" )
	{
		return null;
	}

	try
	{
		var parsedData = JSON.parse( fileText );
		if( !parsedData || typeof parsedData != "object" || IsArrayValue( parsedData ) )
		{
			return null;
		}
		return parsedData;
	}
	catch( error )
	{
		Console.Warning( "Community collections: JSON parse failed for " + fileName + ": " + error );
		return null;
	}
}

function NormalizeCollectionDefaults( config )
{
	if( !config.donations )
	{
		config.donations = [];
	}
	if( !config.rewards )
	{
		config.rewards = [];
	}
	if( !config.maxTier )
	{
		config.maxTier = 0;
	}
	if( !config.startTier )
	{
		config.startTier = 0;
	}
	if( !config.nextTier )
	{
		config.nextTier = 0;
	}
	if( !config.dailyDecay )
	{
		config.dailyDecay = 0;
	}
	if( !config.displayObjects )
	{
		config.displayObjects = [];
	}
}

function IsArrayValue( value )
{
	return Object.prototype.toString.call( value ) == "[object Array]";
}

function SanitizeJsonText( fileText )
{
	if( fileText == null || typeof fileText == "undefined" )
	{
		return "";
	}
	return String( fileText ).replace( /[^\x20-\x7E\r\n\t]/g, "" ).replace( /^\s+|\s+$/g, "" );
}

function ReadWholeFile( mFile )
{
	var fileText = "";
	if( !mFile )
	{
		return fileText;
	}

	while( !mFile.EOF() )
	{
		var rawLine = mFile.ReadUntil( "\n" );
		if( rawLine != null && typeof rawLine != "undefined" )
		{
			fileText += rawLine;
			if( !mFile.EOF() )
			{
				fileText += "\n";
			}
		}
	}

	mFile.Close();
	mFile.Free();
	return fileText;
}


function GetPlayerCollectionSaveFile( player )
{
	if( !ValidateObject( player ) || !player.account )
	{
		return "";
	}
	return "CommunityCollectionPlayers_" + player.account.id + ".json";
}

function ReadPlayerCollectionData( player )
{
	if( !ValidateObject( player ) || !player.account )
	{
		return { points: {}, titles: {}, selectedTitle: "" };
	}

	var fileName = GetPlayerCollectionSaveFile( player );
	var playerKey = String( player.serial );
	var mFile = new UOXCFile();
	mFile.Open( fileName, "r", "CommunityCollections" );

	if( !mFile || mFile.Length() <= 0 )
	{
		if( mFile )
		{
			mFile.Free();
		}
		return ImportPlayerCollectionTags( player );
	}

	var fileText = SanitizeJsonText( ReadWholeFile( mFile ) );
	if( fileText == "" )
	{
		return ImportPlayerCollectionTags( player );
	}

	try
	{
		var parsedData = JSON.parse( fileText );
		if( parsedData && parsedData.characters && parsedData.characters[playerKey] )
		{
			return NormalizePlayerCollectionData( parsedData.characters[playerKey] );
		}
	}
	catch( error )
	{
		Console.Warning( "Community collections: Failed to parse " + fileName + ": " + error );
	}

	return ImportPlayerCollectionTags( player );
}

function WritePlayerCollectionData( player, playerData )
{
	if( !ValidateObject( player ) || !player.account )
	{
		return false;
	}

	playerData = NormalizePlayerCollectionData( playerData );

	var fileName = GetPlayerCollectionSaveFile( player );
	var playerKey = String( player.serial );
	var saveObject = { version: collectionPlayerVersion, characters: {} };
	var mFile = new UOXCFile();
	mFile.Open( fileName, "r", "CommunityCollections" );

	if( mFile && mFile.Length() > 0 )
	{
		var existingText = SanitizeJsonText( ReadWholeFile( mFile ) );
		if( existingText != "" )
		{
			try
			{
				var parsedData = JSON.parse( existingText );
				if( parsedData && typeof parsedData == "object" )
				{
					saveObject = parsedData;
				}
			}
			catch( error )
			{
				Console.Warning( "Community collections: Failed to parse existing " + fileName + ": " + error );
			}
		}
	}
	else if( mFile )
	{
		mFile.Free();
	}

	if( !saveObject.characters )
	{
		saveObject.characters = {};
	}

	saveObject.version = collectionPlayerVersion;
	saveObject.characters[playerKey] = playerData;

	var fileText = "";
	try
	{
		fileText = JSON.stringify( saveObject, null, "\t" );
	}
	catch( error )
	{
		Console.Warning( "Community collections: Failed to serialize " + fileName + ": " + error );
		return false;
	}

	mFile = new UOXCFile();
	mFile.Open( fileName, "w", "CommunityCollections" );
	if( !mFile )
	{
		return false;
	}

	mFile.Write( fileText + "\n" );
	mFile.Close();
	mFile.Free();
	return true;
}

function NormalizePlayerCollectionData( playerData )
{
	if( !playerData || typeof playerData != "object" )
	{
		playerData = {};
	}
	if( !playerData.points )
	{
		playerData.points = {};
	}
	if( !playerData.titles )
	{
		playerData.titles = {};
	}
	if( !playerData.selectedTitle )
	{
		playerData.selectedTitle = "";
	}
	return playerData;
}

function ImportPlayerCollectionTags( player )
{
	var playerData = { points: {}, titles: {}, selectedTitle: "" };
	if( !ValidateObject( player ) )
	{
		return playerData;
	}

	if( !collectionRegistryLoaded )
	{
		LoadCollectionRegistry();
	}

	for( var i = 0; i < collectionConfigs.length; i++ )
	{
		var config = collectionConfigs[i];
		var points = player.GetTag( "ccPoints_" + config.id );
		if( points != 0 && points != null && points != "" )
		{
			playerData.points[config.id] = parseInt( points, 10 ) || 0;
		}

		for( var rewardIndex = 0; rewardIndex < config.rewards.length; rewardIndex++ )
		{
			var reward = config.rewards[rewardIndex];
			if( reward.title && player.GetTag( "ccTitle_" + reward.title ) == 1 )
			{
				playerData.titles[String( reward.title )] = true;
			}
		}
	}

	var selectedTitle = player.GetTag( "ccSelectedTitle" );
	if( selectedTitle != 0 && selectedTitle != null && selectedTitle != "" )
	{
		playerData.selectedTitle = String( selectedTitle );
	}

	WritePlayerCollectionData( player, playerData );
	return playerData;
}

function ReadWorldCollectionData()
{
	var mFile = new UOXCFile();
	mFile.Open( "CommunityCollectionWorld.json", "r", "CommunityCollections" );

	if( !mFile || mFile.Length() <= 0 )
	{
		if( mFile )
		{
			mFile.Free();
		}
		return { version: collectionWorldVersion, collections: {} };
	}

	var fileText = SanitizeJsonText( ReadWholeFile( mFile ) );
	if( fileText == "" )
	{
		return { version: collectionWorldVersion, collections: {} };
	}

	try
	{
		var parsedData = JSON.parse( fileText );
		if( parsedData && typeof parsedData == "object" )
		{
			if( !parsedData.collections )
			{
				parsedData.collections = {};
			}
			return parsedData;
		}
	}
	catch( error )
	{
		Console.Warning( "Community collections: Failed to parse CommunityCollectionWorld.json: " + error );
	}

	return { version: collectionWorldVersion, collections: {} };
}

function WriteWorldCollectionData( worldData )
{
	if( !worldData || typeof worldData != "object" )
	{
		worldData = { version: collectionWorldVersion, collections: {} };
	}
	if( !worldData.collections )
	{
		worldData.collections = {};
	}
	worldData.version = collectionWorldVersion;

	var fileText = "";
	try
	{
		fileText = JSON.stringify( worldData, null, "\t" );
	}
	catch( error )
	{
		Console.Warning( "Community collections: Failed to serialize CommunityCollectionWorld.json: " + error );
		return false;
	}

	var mFile = new UOXCFile();
	mFile.Open( "CommunityCollectionWorld.json", "w", "CommunityCollections" );
	if( !mFile )
	{
		return false;
	}

	mFile.Write( fileText + "\n" );
	mFile.Close();
	mFile.Free();
	return true;
}

function GetWorldCollectionEntry( collectionId )
{
	var worldData = ReadWorldCollectionData();
	if( !worldData.collections[String( collectionId )] )
	{
		worldData.collections[String( collectionId )] = { points: 0, tier: 0, lastDecay: 0 };
		WriteWorldCollectionData( worldData );
	}
	return worldData.collections[String( collectionId )];
}

function SetWorldCollectionEntry( collectionId, entry )
{
	var worldData = ReadWorldCollectionData();
	worldData.collections[String( collectionId )] = {
		points: parseInt( entry.points, 10 ) || 0,
		tier: parseInt( entry.tier, 10 ) || 0,
		lastDecay: parseInt( entry.lastDecay, 10 ) || 0
	};
	WriteWorldCollectionData( worldData );
}

function ImportHolderCollectionTags( holder, collectionId )
{
	if( !ValidateObject( holder ) )
	{
		return;
	}

	var entry = GetWorldCollectionEntry( collectionId );
	var tagPoints = holder.GetTag( "collectionPoints" );
	var tagTier = holder.GetTag( "collectionTier" );
	var changed = false;

	if( entry.points == 0 && tagPoints != 0 && tagPoints != null && tagPoints != "" )
	{
		entry.points = parseInt( tagPoints, 10 ) || 0;
		changed = true;
	}
	if( entry.tier == 0 && tagTier != 0 && tagTier != null && tagTier != "" )
	{
		entry.tier = parseInt( tagTier, 10 ) || 0;
		changed = true;
	}

	if( changed )
	{
		SetWorldCollectionEntry( collectionId, entry );
	}
}

function GetCollectionIdFromObject( obj )
{
	if( !ValidateObject( obj ))
		return "";
	var collectionId = obj.GetTag( "collectionId" );
	if( collectionId == 0 || collectionId == null || collectionId == "" )
		collectionId = "library";
	return collectionId;
}

function GetCollectionPoints( obj )
{
	var collectionId = GetCollectionIdFromObject( obj );
	ImportHolderCollectionTags( obj, collectionId );
	var entry = GetWorldCollectionEntry( collectionId );
	return parseInt( entry.points, 10 ) || 0;
}

function SetCollectionPoints( obj, points )
{
	if( points < 0 )
	{
		points = 0;
	}

	var collectionId = GetCollectionIdFromObject( obj );
	var entry = GetWorldCollectionEntry( collectionId );
	entry.points = parseInt( points, 10 ) || 0;
	SetWorldCollectionEntry( collectionId, entry );

	if( ValidateObject( obj ) )
	{
		obj.SetTag( "collectionPoints", entry.points );
	}
}

function GetCollectionTier( obj )
{
	var collectionId = GetCollectionIdFromObject( obj );
	ImportHolderCollectionTags( obj, collectionId );
	var entry = GetWorldCollectionEntry( collectionId );
	return parseInt( entry.tier, 10 ) || 0;
}

function SetCollectionTier( obj, tier )
{
	if( tier < 0 )
	{
		tier = 0;
	}

	var collectionId = GetCollectionIdFromObject( obj );
	var entry = GetWorldCollectionEntry( collectionId );
	entry.tier = parseInt( tier, 10 ) || 0;
	SetWorldCollectionEntry( collectionId, entry );

	if( ValidateObject( obj ) )
	{
		obj.SetTag( "collectionTier", entry.tier );
	}
}

function GetPlayerCollectionPoints( pUser, collectionId )
{
	var playerData = ReadPlayerCollectionData( pUser );
	var points = playerData.points[String( collectionId )];
	return parseInt( points, 10 ) || 0;
}

function SetPlayerCollectionPoints( pUser, collectionId, points )
{
	if( points < 0 )
	{
		points = 0;
	}

	var playerData = ReadPlayerCollectionData( pUser );
	playerData.points[String( collectionId )] = parseInt( points, 10 ) || 0;
	WritePlayerCollectionData( pUser, playerData );
}

function HasRewardTitle( pUser, titleName )
{
	var playerData = ReadPlayerCollectionData( pUser );
	return playerData.titles[String( titleName )] ? true : false;
}

function AddRewardTitle( pUser, titleName )
{
	var playerData = ReadPlayerCollectionData( pUser );
	playerData.titles[String( titleName )] = true;
	playerData.selectedTitle = String( titleName );
	WritePlayerCollectionData( pUser, playerData );
}

function SetSelectedCollectionTitle( pUser, titleName )
{
	var playerData = ReadPlayerCollectionData( pUser );
	if( playerData.titles[String( titleName )] )
	{
		playerData.selectedTitle = String( titleName );
		WritePlayerCollectionData( pUser, playerData );
	}
}

function GetSelectedCollectionTitle( pUser )
{
	var playerData = ReadPlayerCollectionData( pUser );
	return playerData.selectedTitle || "";
}

function GetTierGoal( config, tier )
{
	if( tier < 1 )
		return config.startTier;
	return config.startTier + ( config.nextTier * tier );
}

function UpdateCollectionTier( holder, config )
{
	var points = GetCollectionPoints( holder );
	var tier = GetCollectionTier( holder );
	var oldTier = tier;

	while( tier < config.maxTier && points >= GetTierGoal( config, tier ))
		tier++;

	while( tier > 0 && points < GetTierGoal( config, tier - 1 ))
		tier--;

	SetCollectionTier( holder, tier );

	if( tier > oldTier )
	{
		BroadcastMessage( config.name + " has reached collection tier " + tier + "." );
		SpawnTierDisplays( holder, config, oldTier + 1, tier );
	}
}

function MatchDonationItem( item, donation )
{
	if( !ValidateObject( item ) || !item.isItem )
		return false;
	if( item.id != donation.itemId )
		return false;
	if( donation.hue >= 0 && item.colour != donation.hue && item.color != donation.hue )
		return false;
	return true;
}

function IsPetDonation( donation )
{
	return donation && String( donation.type || "item" ).toLowerCase() == "pet";
}

function DoesPetMatchDonation( petObj, donation )
{
	if( !ValidateObject( petObj ) || !petObj.isChar || !IsPetDonation( donation ) )
		return false;

	var hasMatcher = false;
	var petSectionID = String( petObj.sectionID || "" );

	if( donation.npcIDs && donation.npcIDs.length )
	{
		hasMatcher = true;
		for( var npcIndex = 0; npcIndex < donation.npcIDs.length; npcIndex++ )
		{
			if( String( donation.npcIDs[npcIndex] ) == petSectionID )
				return true;
		}
	}

	if( donation.bodyIds && donation.bodyIds.length )
	{
		hasMatcher = true;
		for( var bodyIndex = 0; bodyIndex < donation.bodyIds.length; bodyIndex++ )
		{
			if( parseInt( donation.bodyIds[bodyIndex], 10 ) == petObj.id )
				return true;
		}
	}

	if( typeof donation.bodyId != "undefined" && donation.bodyId != null )
	{
		hasMatcher = true;
		if( parseInt( donation.bodyId, 10 ) == petObj.id )
			return true;
	}

	if( donation.hue >= 0 && petObj.colour != donation.hue && petObj.color != donation.hue )
	{
		return false;
	}

	return !hasMatcher;
}

function ValidatePetDonation( pUser, holder, petObj )
{
	if( !ValidateObject( pUser ) || !ValidateObject( holder ) || !ValidateObject( petObj ) || !petObj.isChar )
		return "That is not a valid pet.";

	if( !pUser.InRange( holder, 8 ) )
		return "You are too far away from the collection donation box.";

	if( petObj == pUser )
		return "You cannot donate yourself.";

	if( !petObj.tamed || petObj.isHuman || petObj.GetTag( "isPetDead" ) == true || petObj.GetTag( "isPetDead" ) == 1 )
		return "You cannot donate that creature.";

	if( petObj.owner != pUser )
		return "That is not your pet.";

	if( !petObj.InRange( holder, 8 ) )
		return "The pet must be near the collection donation box.";

	if( petObj.isDispellable )
		return "You cannot donate summoned creatures.";

	if(( petObj.id == 0x0123 || petObj.id == 0x0124 ) && petObj.pack && petObj.pack.itemsinside > 0 )
		return "You need to unload your pack animal first.";

	if( petObj.atWar )
		return "Your pet is busy right now.";

	if( petObj.stabled == 1 )
		return "That creature is already stabled.";

	return "";
}

function ConsumeDonatedPet( pUser, petObj )
{
	if( !ValidateObject( pUser ) || !ValidateObject( petObj ) )
		return;

	pUser.RemoveFollower( petObj );
	pUser.controlSlotsUsed = Math.max( 0, pUser.controlSlotsUsed - petObj.controlSlots );

	petObj.owner = null;
	petObj.Follow( null );
	petObj.stabled = 1;
	petObj.visible = 3;
	petObj.wandertype = 0;
	petObj.frozen = true;
	petObj.hunger = 6;
	petObj.willhunger = false;
	petObj.willthirst = false;
	petObj.vulnerable = false;
	petObj.Teleport( COLLECTION_PET_HOLDING_X, COLLECTION_PET_HOLDING_Y, COLLECTION_PET_HOLDING_Z );

	if( COLLECTION_DELETE_DONATED_PETS )
	{
		petObj.Delete();
	}
}

function IsItemInPlayerPack( pUser, item )
{
	var owner = GetPackOwner( item, 0 );
	return ValidateObject( owner ) && owner.serial == pUser.serial;
}

function GetItemAmountSafe( item )
{
	if( item.amount > 0 )
		return item.amount;
	return 1;
}

function ConsumeItemAmount( item, amount )
{
	var itemAmount = GetItemAmountSafe( item );
	if( amount >= itemAmount )
	{
		item.Delete();
		return;
	}
	item.amount = itemAmount - amount;
}

function MakeRewardItem( pUser, reward, hue )
{
	var rewardItem = CreateBlankItem( pUser.socket, pUser, 1, reward.name, reward.itemId, hue, "ITEM", true );
	if( ValidateObject( rewardItem ))
	{
		rewardItem.name = reward.name;
		rewardItem.colour = hue;
		rewardItem.SetTag( "communityCollectionReward", 1 );
		rewardItem.SetTag( "collectionRewardKey", reward.key );
	}
	return rewardItem;
}


function GetListPerPage( page )
{
	if( page == 3 )
		return 8;
	return 7;
}

function GetListOffsetTag( page )
{
	if( page == 1 )
		return "ccDonationOffset";
	if( page == 2 )
		return "ccRewardOffset";
	if( page == 3 )
		return "ccTitleOffset";
	return "";
}

function GetListOffset( pUser, page )
{
	var tagName = GetListOffsetTag( page );
	if( tagName == "" )
		return 0;

	var offset = parseInt( pUser.GetTempTag( tagName ), 10 );
	if( isNaN( offset ) || offset < 0 )
		offset = 0;

	return offset;
}

function SetListOffset( pUser, page, offset )
{
	var tagName = GetListOffsetTag( page );
	if( tagName == "" )
		return;

	offset = parseInt( offset, 10 );
	if( isNaN( offset ) || offset < 0 )
		offset = 0;

	pUser.SetTempTag( tagName, offset );
}

function GetCollectionItemWidth( entry )
{
	var itemWidth = parseInt( entry.width, 10 );
	if( isNaN( itemWidth ) || itemWidth <= 0 )
		itemWidth = 44;
	return itemWidth;
}

function GetCollectionItemHeight( entry )
{
	var itemHeight = parseInt( entry.height, 10 );
	if( isNaN( itemHeight ) || itemHeight < 20 )
		itemHeight = 20;
	return itemHeight;
}

function GetCollectionItemArtOffsetX( entry )
{
	var itemX = parseInt( entry.artOffsetX, 10 );
	if( isNaN( itemX ) )
	{
		itemX = parseInt( entry.x, 10 );
		if( isNaN( itemX ) )
			itemX = 0;
	}
	return itemX;
}

function GetCollectionItemArtOffsetY( entry )
{
	var itemY = parseInt( entry.artOffsetY, 10 );
	if( isNaN( itemY ) )
	{
		itemY = parseInt( entry.y, 10 );
		if( isNaN( itemY ) )
			itemY = 0;
	}
	return itemY;
}

function GetCollectionMaxArtWidth( list )
{
	var maxWidth = 0;
	if( !list )
		return 44;

	for( var i = 0; i < list.length; i++ )
	{
		var width = GetCollectionItemWidth( list[i] );
		if( maxWidth < width )
			maxWidth = width;
	}

	if( maxWidth <= 0 )
		maxWidth = 44;
	return maxWidth;
}


function GetCollectionTooltipCliloc( entry )
{
	if( !entry )
		return 0;

	var tooltipCliloc = 0;
	if( typeof entry.tooltipCliloc != "undefined" && entry.tooltipCliloc != null )
	{
		tooltipCliloc = parseInt( entry.tooltipCliloc, 10 );
	}
	else if( typeof entry.cliloc != "undefined" && entry.cliloc != null )
	{
		tooltipCliloc = parseInt( entry.cliloc, 10 );
	}
	else if( typeof entry.labelCliloc != "undefined" && entry.labelCliloc != null )
	{
		tooltipCliloc = parseInt( entry.labelCliloc, 10 );
	}

	if( isNaN( tooltipCliloc ) || tooltipCliloc <= 0 )
		tooltipCliloc = 0;

	return tooltipCliloc;
}

function DrawCollectionEntryArt( gump, entry, x, y, maxWidth, enabled )
{
	var itemId = parseInt( entry.itemId, 10 );
	if( isNaN( itemId ) || itemId <= 0 )
		return;

	var hue = parseInt( entry.hue, 10 );
	if( isNaN( hue ) || hue < 0 )
		hue = 0;

	if( !enabled )
		hue = 0x03E9;

	var width = GetCollectionItemWidth( entry );
	var imageX = x + GetCollectionItemArtOffsetX( entry ) + Math.floor( maxWidth / 2 ) - Math.floor( width / 2 );
	var imageY = y + GetCollectionItemArtOffsetY( entry );
	var height = GetCollectionItemHeight( entry );
	if( height < 20 )
		imageY += Math.floor(( 20 - height ) / 2 );

	if( hue > 0 )
	{
		gump.AddPictureColor( imageX, imageY, itemId, hue );
	}
	else
	{
		gump.AddPicture( imageX, imageY, itemId );
	}

	var tooltipCliloc = GetCollectionTooltipCliloc( entry );
	if( tooltipCliloc > 0 )
	{
		gump.AddToolTip( tooltipCliloc );
	}
}

function CountMatchingDonationItems( pUser, donation )
{
	if( !ValidateObject( pUser ) || !donation )
		return 0;

	if( IsPetDonation( donation ) )
		return 0;

	var pack = pUser.pack;
	if( !ValidateObject( pack ) )
		return 0;

	var totalAmount = 0;
	for( var currentItem = pack.FirstItem(); !pack.FinishedItems(); currentItem = pack.NextItem() )
	{
		if( !ValidateObject( currentItem ) )
			continue;

		if( MatchDonationItem( currentItem, donation ) )
			totalAmount += GetItemAmountSafe( currentItem );
	}

	return totalAmount;
}

function FormatCollectionPoints( points )
{
	var pointValue = parseFloat( points );
	if( isNaN( pointValue ) )
		pointValue = 0;

	if( pointValue > 0 && pointValue < 1 )
	{
		var perAmount = Math.round( 1 / pointValue );
		if( perAmount < 1 )
			perAmount = 1;
		return "1 per " + perAmount;
	}

	return String( Math.round( pointValue ) );
}

function DrawCommunityCollectionFrame( gump )
{
	gump.AddPage( 0 );
	gump.AddGump( 0, 0, 0x1F40 );
	gump.AddTiledGump( 20, 37, 300, 308, 0x1F42 );
	gump.AddGump( 20, 325, 0x1F43 );
	gump.AddGump( 35, 8, 0x0039 );
	gump.AddTiledGump( 65, 8, 257, 10, 0x003A );
	gump.AddGump( 290, 8, 0x003B );
	gump.AddGump( 32, 33, 0x2635 );
	gump.AddTiledGump( 70, 55, 230, 2, 0x23C5 );
	gump.AddXMFHTMLGump( 70, 35, 270, 20, 1072835, false, false ); // Community Collection
}

function OpenCollectionGump( pUser, holder, page )
{
	if( !ValidateObject( pUser ) || !ValidateObject( holder ))
		return;

	var collectionId = GetCollectionIdFromObject( holder );
	var config = FindCollectionConfig( collectionId );
	if( config == null )
	{
		pUser.SysMessage( "This collection is not configured." );
		return;
	}

	if( page == 0 )
	{
		SetListOffset( pUser, 1, 0 );
		SetListOffset( pUser, 2, 0 );
		SetListOffset( pUser, 3, 0 );
		page = 1;
	}

	pUser.SetTempTag( "ccHolderSerial", holder.serial );
	pUser.SetTempTag( "ccCollectionId", collectionId );

	var gump = new Gump();
	DrawCommunityCollectionFrame( gump );

	if( page == 2 )
		AddRewardPage( gump, pUser, config );
	else if( page == 3 )
		AddTitlePage( gump, pUser, config );
	else
		AddDonationPage( gump, pUser, config, holder );

	gump.Send( pUser.socket );
	gump.Free();
}

function AddCollectionNavButtons( gump, activePage )
{
	var navY = 335;

	if( activePage != 1 )
	{
		gump.AddButton( 40, navY, 0x15E3, 0x15E7, 1, 0, 10 );
		gump.AddXMFHTMLGump( 65, navY, 75, 20, 1072845, false, false ); // Status
	}

	if( activePage != 2 )
	{
		gump.AddButton( 140, navY, 0x15E3, 0x15E7, 1, 0, 20 );
		gump.AddXMFHTMLGump( 165, navY, 80, 20, 1072842, false, false ); // Rewards
	}

	if( activePage != 3 )
	{
		gump.AddButton( 245, navY, 0x15E3, 0x15E7, 1, 0, 30 );
		gump.AddText( 270, navY, 1152, "Titles" );
	}
}

function AddDonationPage( gump, pUser, config, holder )
{
	gump.AddXMFHTMLGump( 50, 65, 150, 20, 1072836, false, false ); // Current Tier:
	gump.AddText( 230, 65, 0x0064, String( GetCollectionTier( holder )));
	gump.AddXMFHTMLGump( 50, 85, 150, 20, 1072837, false, false ); // Current Points:
	gump.AddText( 230, 85, 0x0064, String( GetCollectionPoints( holder )));
	gump.AddXMFHTMLGump( 50, 105, 150, 20, 1072838, false, false ); // Points Until Next Tier:
	gump.AddText( 230, 105, 0x0064, String( Math.max( 0, GetTierGoal( config, GetCollectionTier( holder )) - GetCollectionPoints( holder ) )));

	gump.AddTiledGump( 35, 125, 270, 2, 0x23C5 );
	gump.AddXMFHTMLGump( 35, 130, 270, 20, 1072840, false, false ); // Donations Accepted:

	var offset = GetListOffset( pUser, 1 );
	if( offset >= config.donations.length )
		offset = 0;

	var maxWidth = GetCollectionMaxArtWidth( config.donations );
	var y = 150;
	var usedSlots = 0;
	var perPage = 7;
	var end = Math.min( offset + perPage, config.donations.length );

	for( var i = offset; i < end; i++ )
	{
		var donation = config.donations[i];
		var height = GetCollectionItemHeight( donation );
		var amount = CountMatchingDonationItems( pUser, donation );
		var canDonate = ( amount > 0 || IsPetDonation( donation ) );

		if( canDonate )
			gump.AddButton( 35, y + Math.floor( height / 2 ) - 5, 0x0837, 0x0838, 1, 0, 1000 + i );

		DrawCollectionEntryArt( gump, donation, 55, y, maxWidth, true );
		gump.AddText( 65 + maxWidth, y + Math.floor( height / 2 ) - 10, 0x0064, FormatCollectionPoints( donation.points ));

		if( amount > 0 )
			gump.AddText( 235, y + Math.floor( height / 2 ) - 5, 0x00B1, String( amount ));

		y += 5 + height;
		usedSlots++;
	}

	AddListPageButtons( gump, 1, offset, perPage, config.donations.length );
	AddCollectionNavButtons( gump, 1 );
}

function AddOverviewPage( gump, pUser, config, holder )
{
	AddDonationPage( gump, pUser, config, holder );
}

function AddRewardPage( gump, pUser, config )
{
	var points = GetPlayerCollectionPoints( pUser, config.id );
	gump.AddXMFHTMLGump( 50, 65, 150, 20, 1072843, false, false ); // Your Reward Points:
	gump.AddText( 230, 65, 0x0064, String( points ));
	gump.AddTiledGump( 35, 85, 270, 2, 0x23C5 );
	gump.AddXMFHTMLGump( 35, 90, 270, 20, 1072844, false, false ); // Please Choose a Reward:

	var offset = GetListOffset( pUser, 2 );
	if( offset >= config.rewards.length )
		offset = 0;

	var maxWidth = GetCollectionMaxArtWidth( config.rewards );
	var y = 110;
	var perPage = 7;
	var end = Math.min( offset + perPage, config.rewards.length );

	for( var i = offset; i < end; i++ )
	{
		var reward = config.rewards[i];
		var height = GetCollectionItemHeight( reward );
		var canClaim = ( points >= reward.cost );

		if( canClaim )
			gump.AddButton( 35, y + Math.floor( height / 2 ) - 5, 0x0837, 0x0838, 1, 0, 2000 + i );

		DrawCollectionEntryArt( gump, reward, 55, y, maxWidth, canClaim );
		gump.AddText( 65 + maxWidth, y + Math.floor( height / 2 ) - 10, canClaim ? 0x0064 : 0x0021, String( reward.cost ));
		y += 5 + height;
	}

	AddListPageButtons( gump, 2, offset, perPage, config.rewards.length );
	AddCollectionNavButtons( gump, 2 );
}

function AddTitlePage( gump, pUser, config )
{
	var points = GetPlayerCollectionPoints( pUser, config.id );
	gump.AddText( 50, 65, 1152, "Your Reward Points:" );
	gump.AddText( 230, 65, 0x0064, String( points ));
	gump.AddTiledGump( 35, 85, 270, 2, 0x23C5 );
	gump.AddText( 35, 90, 1152, "Please Select a Title:" );

	var titleIndexes = [];
	for( var i = 0; i < config.rewards.length; i++ )
	{
		if( config.rewards[i].title )
			titleIndexes.push( i );
	}

	var offset = GetListOffset( pUser, 3 );
	var perPage = 8;
	if( offset >= titleIndexes.length )
		offset = 0;

	var y = 120;
	var end = Math.min( offset + perPage, titleIndexes.length );
	for( var t = offset; t < end; t++ )
	{
		var rewardIndex = titleIndexes[t];
		var reward = config.rewards[rewardIndex];
		var ownsTitle = HasRewardTitle( pUser, reward.title );
		gump.AddButton( 35, y + 3, 0x0837, 0x0838, 1, 0, 3000 + rewardIndex );
		gump.AddText( 65, y, ownsTitle ? 0x0064 : 0x0026, ownsTitle ? reward.title : reward.title + " locked" );
		y += 22;
	}

	AddListPageButtons( gump, 3, offset, perPage, titleIndexes.length );
	AddCollectionNavButtons( gump, 3 );
}

function AddListPageButtons( gump, page, offset, perPage, totalCount )
{
	if( totalCount <= perPage )
		return;

	// Paging sits above the lower navigation bar so it does not overlap Status, Rewards or Titles.
	if( offset > 0 )
	{
		gump.AddButton( 40, 310, 0x15E3, 0x15E7, 1, 0, 11000 + page );
		gump.AddXMFHTMLGump( 65, 310, 85, 20, 1074880, false, false ); // Previous
	}

	if( offset + perPage < totalCount )
	{
		gump.AddXMFHTMLGump( 245, 310, 55, 20, 1072854, false, false ); // Next
		gump.AddButton( 300, 310, 0x15E1, 0x15E5, 1, 0, 12000 + page );
	}
}

function OpenConfirmRewardGump( pUser, rewardIndex, hueIndex )
{
	var collectionId = pUser.GetTempTag( "ccCollectionId" );
	var config = FindCollectionConfig( collectionId );
	if( config == null || rewardIndex < 0 || rewardIndex >= config.rewards.length )
		return;

	var reward = config.rewards[rewardIndex];
	var hue = reward.hue;
	if( reward.hues && hueIndex >= 0 && hueIndex < reward.hues.length )
		hue = reward.hues[hueIndex];

	pUser.SetTempTag( "ccRewardIndex", rewardIndex );
	pUser.SetTempTag( "ccRewardHue", hue );

	var gump = new Gump();
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 270, 120, 0x13BE );
	gump.AddBackground( 10, 10, 250, 100, 0x0BB8 );
	gump.AddXMFHTMLGump( 20, 15, 230, 20, 1074974, false, false ); // Confirm Selection
	gump.AddXMFHTMLGump( 20, 35, 230, 40, 1074975, true, false ); // Are you sure you wish to select this?
	DrawCollectionEntryArt( gump, reward, 150, 65, GetCollectionItemWidth( reward ), true );
	gump.AddButton( 20, 80, 0x0FA5, 0x0FA7, 1, 0, 4001 );
	gump.AddText( 55, 80, 0x0000, "ACCEPT" );
	gump.AddButton( 135, 80, 0x0FA5, 0x0FA7, 1, 0, 4002 );
	gump.AddText( 170, 80, 0x0000, "CANCEL" );
	gump.Send( pUser.socket );
	gump.Free();
}

function OpenHueRewardGump( pUser, rewardIndex )
{
	var config = FindCollectionConfig( pUser.GetTempTag( "ccCollectionId" ));
	if( config == null || rewardIndex < 0 || rewardIndex >= config.rewards.length )
		return;

	var reward = config.rewards[rewardIndex];
	if( !reward.hues || reward.hues.length == 0 )
	{
		OpenConfirmRewardGump( pUser, rewardIndex, -1 );
		return;
	}

	pUser.SetTempTag( "ccRewardIndex", rewardIndex );
	var gump = new Gump();
	DrawCommunityCollectionFrame( gump );
	gump.AddXMFHTMLGump( 50, 65, 150, 20, 1072843, false, false ); // Your Reward Points:
	gump.AddText( 230, 65, 0x0064, String( GetPlayerCollectionPoints( pUser, config.id )));
	gump.AddTiledGump( 35, 85, 270, 2, 0x23C5 );
	gump.AddXMFHTMLGump( 35, 90, 270, 20, 1074255, false, false ); // Please select a hue for your Reward:

	var height = GetCollectionItemHeight( reward );
	var y = 110;
	var end = Math.min( reward.hues.length, 8 );
	for( var i = 0; i < end; i++ )
	{
		gump.AddButton( 35, y + Math.floor( height / 2 ) - 5, 0x0837, 0x0838, 1, 0, 5000 + i );
		var hueEntry = { itemId: reward.itemId, hue: reward.hues[i], width: reward.width, height: reward.height, x: reward.x, y: reward.y };
		DrawCollectionEntryArt( gump, hueEntry, 55, y, GetCollectionItemWidth( reward ), true );
		y += 5 + height;
	}

	gump.AddButton( 50, 335, 0x15E3, 0x15E7, 1, 0, 20 );
	gump.AddXMFHTMLGump( 75, 335, 100, 20, 1072842, false, false ); // Rewards
	gump.Send( pUser.socket );
	gump.Free();
}

function OpenSelectTitleGump( pUser, page )
{
	var playerData = ReadPlayerCollectionData( pUser );
	var titles = [];
	for( var titleName in playerData.titles )
	{
		if( playerData.titles.hasOwnProperty( titleName ) && playerData.titles[titleName] )
			titles.push( titleName );
	}

	if( page < -1 )
		page = -1;
	if( page >= titles.length )
		page = -1;

	pUser.SetTempTag( "ccSelectTitlePage", page );

	var gump = new Gump();
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 270, 120, 0x13BE );
	gump.AddBackground( 10, 10, 250, 100, 0x0BB8 );
	gump.AddXMFHTMLGump( 20, 15, 230, 20, 1073994, false, false ); // Your title will be:

	if( page > -1 && page < titles.length )
		gump.AddHTMLGump( 20, 35, 230, 40, true, false, "<BASEFONT COLOR=#000032>" + titles[page] + "</BASEFONT>" );
	else
		gump.AddXMFHTMLGump( 20, 35, 230, 40, 1073995, true, false );

	gump.AddXMFHTMLGump( 55, 80, 75, 20, 1073996, false, false ); // ACCEPT
	gump.AddXMFHTMLGump( 170, 80, 75, 20, 1073997, false, false ); // NEXT
	gump.AddButton( 20, 80, 0x0FA5, 0x0FA7, 1, 0, 6001 );
	gump.AddButton( 135, 80, 0x0FA5, 0x0FA7, 1, 0, 6002 );
	gump.Send( pUser.socket );
	gump.Free();
}

function ClaimReward( pUser )
{
	var collectionId = pUser.GetTempTag( "ccCollectionId" );
	var config = FindCollectionConfig( collectionId );
	var rewardIndex = pUser.GetTempTag( "ccRewardIndex" );
	var hue = pUser.GetTempTag( "ccRewardHue" );
	if( config == null || rewardIndex < 0 || rewardIndex >= config.rewards.length )
		return;

	var reward = config.rewards[rewardIndex];
	var points = GetPlayerCollectionPoints( pUser, collectionId );
	if( points < reward.cost )
	{
		pUser.SysMessage( "You do not have enough collection points for that reward." );
		return;
	}

	SetPlayerCollectionPoints( pUser, collectionId, points - reward.cost );

	if( reward.title )
	{
		AddRewardTitle( pUser, reward.title );
		pUser.SysMessage( "The title has been bestowed upon you: " + reward.title );
	}
	else
	{
		if( hue == 0 || hue == null || hue == "" )
			hue = reward.hue;
		MakeRewardItem( pUser, reward, hue );
		pUser.SysMessage( "The reward has been placed in your backpack." );
	}
	OpenCollectionGumpFromStoredHolder( pUser, 2 );
}

function BeginDonationTarget( pUser, donationIndex )
{
	pUser.SetTempTag( "ccDonationIndex", donationIndex );
	pUser.socket.CustomTarget( 0, "Target the item or pet you want to donate." );
}

function onCallback0( socket, target )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ) || !ValidateObject( target ))
		return;

	var holder = CalcItemFromSer( pUser.GetTempTag( "ccHolderSerial" ));
	if( !ValidateObject( holder ))
		holder = CalcCharFromSer( pUser.GetTempTag( "ccHolderSerial" ));
	if( !ValidateObject( holder ))
	{
		pUser.SysMessage( "The collection holder is no longer available." );
		return;
	}

	var collectionId = GetCollectionIdFromObject( holder );
	var config = FindCollectionConfig( collectionId );
	var donationIndex = pUser.GetTempTag( "ccDonationIndex" );
	if( config == null || donationIndex < 0 || donationIndex >= config.donations.length )
		return;

	var donation = config.donations[donationIndex];
	var amount = 1;
	var points = 0;

	if( IsPetDonation( donation ))
	{
		var petError = ValidatePetDonation( pUser, holder, target );
		if( petError != "" )
		{
			pUser.SysMessage( petError );
			return;
		}

		if( !DoesPetMatchDonation( target, donation ))
		{
			pUser.SysMessage( "That pet does not match the selected donation." );
			return;
		}

		points = Math.round( donation.points );
		if( points < 1 )
			points = 1;

		var petName = target.name;
		ConsumeDonatedPet( pUser, target );
		SetCollectionPoints( holder, GetCollectionPoints( holder ) + points );
		SetPlayerCollectionPoints( pUser, collectionId, GetPlayerCollectionPoints( pUser, collectionId ) + points );
		UpdateCollectionTier( holder, config );

		pUser.SysMessage( "You donate " + petName + " and receive " + points + " collection points." );
		OpenCollectionGump( pUser, holder, 1 );
		return;
	}

	if( !MatchDonationItem( target, donation ))
	{
		pUser.SysMessage( "That item does not match the selected donation." );
		return;
	}

	if( !IsItemInPlayerPack( pUser, target ))
	{
		pUser.SysMessage( "The donated item must be in your backpack." );
		return;
	}

	amount = GetItemAmountSafe( target );
	if( amount > COLLECTION_MAX_DONATE_AMOUNT )
		amount = COLLECTION_MAX_DONATE_AMOUNT;

	points = Math.round( amount * donation.points );
	if( points < 1 )
		points = 1;

	ConsumeItemAmount( target, amount );
	SetCollectionPoints( holder, GetCollectionPoints( holder ) + points );
	SetPlayerCollectionPoints( pUser, collectionId, GetPlayerCollectionPoints( pUser, collectionId ) + points );
	UpdateCollectionTier( holder, config );

	pUser.SysMessage( "You donate " + amount + " " + donation.name + " and receive " + points + " collection points." );
	OpenCollectionGump( pUser, holder, 1 );
}

function OpenCollectionGumpFromStoredHolder( pUser, page )
{
	var holder = CalcItemFromSer( pUser.GetTempTag( "ccHolderSerial" ));
	if( !ValidateObject( holder ))
		holder = CalcCharFromSer( pUser.GetTempTag( "ccHolderSerial" ));
	if( ValidateObject( holder ))
		OpenCollectionGump( pUser, holder, page );
}

function onUseChecked( pUser, iUsed )
{
	OpenCollectionGump( pUser, iUsed, 0 );
	return false;
}

function onCharDoubleClick( pUser, targChar, nonMouseClickEvent )
{
	if( nonMouseClickEvent )
		return true;
	OpenCollectionGump( pUser, targChar, 0 );
	return false;
}

function onSpeech( strSaid, pUser, holder )
{
	if( strSaid == "donate" || strSaid == "collection" || strSaid == "rewards" )
	{
		OpenCollectionGump( pUser, holder, 0 );
		return false;
	}
	return true;
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ))
		return;

	if( pButton == 10 )
	{
		OpenCollectionGumpFromStoredHolder( pUser, 1 );
		return;
	}
	if( pButton == 20 )
	{
		OpenCollectionGumpFromStoredHolder( pUser, 2 );
		return;
	}
	if( pButton == 30 )
	{
		OpenCollectionGumpFromStoredHolder( pUser, 3 );
		return;
	}
	if( pButton >= 11001 && pButton <= 11003 )
	{
		var prevPage = pButton - 11000;
		SetListOffset( pUser, prevPage, GetListOffset( pUser, prevPage ) - GetListPerPage( prevPage ) );
		OpenCollectionGumpFromStoredHolder( pUser, prevPage );
		return;
	}
	if( pButton >= 12001 && pButton <= 12003 )
	{
		var nextPage = pButton - 12000;
		SetListOffset( pUser, nextPage, GetListOffset( pUser, nextPage ) + GetListPerPage( nextPage ) );
		OpenCollectionGumpFromStoredHolder( pUser, nextPage );
		return;
	}
	if( pButton >= 1000 && pButton < 1100 )
	{
		BeginDonationTarget( pUser, pButton - 1000 );
		return;
	}
	if( pButton >= 2000 && pButton < 2100 )
	{
		OpenHueRewardGump( pUser, pButton - 2000 );
		return;
	}
	if( pButton >= 3000 && pButton < 3100 )
	{
		var config = FindCollectionConfig( pUser.GetTempTag( "ccCollectionId" ));
		var rewardIndex = pButton - 3000;
		if( config != null && rewardIndex >= 0 && rewardIndex < config.rewards.length && config.rewards[rewardIndex].title )
		{
			if( HasRewardTitle( pUser, config.rewards[rewardIndex].title ))
			{
				SetSelectedCollectionTitle( pUser, config.rewards[rewardIndex].title );
				pUser.SysMessage( "Selected title: " + config.rewards[rewardIndex].title );
			}
			else
				pUser.SysMessage( "You have not unlocked that title yet." );
		}
		return;
	}
	if( pButton == 4001 )
	{
		ClaimReward( pUser );
		return;
	}
	if( pButton == 4002 )
	{
		OpenCollectionGumpFromStoredHolder( pUser, 2 );
		return;
	}
	if( pButton >= 5000 && pButton < 5100 )
	{
		OpenConfirmRewardGump( pUser, pUser.GetTempTag( "ccRewardIndex" ), pButton - 5000 );
		return;
	}

	if( pButton == 6001 )
	{
		var selectedTitlePage = parseInt( pUser.GetTempTag( "ccSelectTitlePage" ), 10 );
		var playerData = ReadPlayerCollectionData( pUser );
		var titles = [];
		for( var titleName in playerData.titles )
		{
			if( playerData.titles.hasOwnProperty( titleName ) && playerData.titles[titleName] )
				titles.push( titleName );
		}
		if( selectedTitlePage > -1 && selectedTitlePage < titles.length )
		{
			SetSelectedCollectionTitle( pUser, titles[selectedTitlePage] );
			pUser.SysMessage( "Selected title: " + titles[selectedTitlePage] );
		}
		return;
	}
	if( pButton == 6002 )
	{
		var nextTitlePage = parseInt( pUser.GetTempTag( "ccSelectTitlePage" ), 10 );
		var titleData = ReadPlayerCollectionData( pUser );
		var titleCount = 0;
		for( var titleKey in titleData.titles )
		{
			if( titleData.titles.hasOwnProperty( titleKey ) && titleData.titles[titleKey] )
				titleCount++;
		}
		if( titleCount <= 0 )
			nextTitlePage = -1;
		else if( nextTitlePage == titleCount - 1 )
			nextTitlePage = -1;
		else if( nextTitlePage < titleCount - 1 && nextTitlePage > -1 )
			nextTitlePage++;
		else
			nextTitlePage = 0;
		OpenSelectTitleGump( pUser, nextTitlePage );
		return;
	}

}

function onTimer( timerObj, timerId )
{
	if( timerId != 1 || !ValidateObject( timerObj ))
	{
		return;
	}

	ApplyCollectionDailyDecay( timerObj );
	timerObj.StartTimer( COLLECTION_DECAY_MS, 1, true );
}

function ApplyCollectionDailyDecay( holder )
{
	if( !ValidateObject( holder ))
	{
		return;
	}

	var collectionId = GetCollectionIdFromObject( holder );
	var config = FindCollectionConfig( collectionId );
	if( config == null || config.dailyDecay <= 0 )
	{
		return;
	}

	var worldData = ReadWorldCollectionData();
	if( !worldData.collections[String( collectionId )] )
	{
		worldData.collections[String( collectionId )] = { points: 0, tier: 0, lastDecay: 0 };
	}

	var entry = worldData.collections[String( collectionId )];
	var now = Date.now();
	var lastDecay = parseInt( entry.lastDecay, 10 );
	if( isNaN( lastDecay ))
	{
		lastDecay = 0;
	}

	if( lastDecay > 0 && ( now - lastDecay ) < COLLECTION_DECAY_MS )
	{
		return;
	}

	entry.points = parseInt( entry.points, 10 ) || 0;
	entry.tier = parseInt( entry.tier, 10 ) || 0;
	entry.points -= config.dailyDecay;
	if( entry.points < 0 )
	{
		entry.points = 0;
	}
	entry.lastDecay = now;
	worldData.collections[String( collectionId )] = entry;
	WriteWorldCollectionData( worldData );

	if( ValidateObject( holder ) )
	{
		holder.SetTag( "collectionPoints", entry.points );
		holder.SetTag( "collectionTier", entry.tier );
	}

	UpdateCollectionTier( holder, config );
}

function onCreateDFN( objMade, objType )
{
	if( ValidateObject( objMade ))
		objMade.StartTimer( COLLECTION_DECAY_MS, 1, true );
}

function SpawnTierDisplays( holder, config, minTier, maxTier )
{
	if( !ValidateObject( holder ) || !config )
		return;

	minTier = parseInt( minTier, 10 );
	maxTier = parseInt( maxTier, 10 );
	if( isNaN( minTier ) || minTier < 1 )
		minTier = 1;
	if( isNaN( maxTier ) || maxTier < minTier )
		return;

	if( config.displayObjects && config.displayObjects.length )
	{
		for( var displayIndex = 0; displayIndex < config.displayObjects.length; displayIndex++ )
		{
			var displayData = config.displayObjects[displayIndex];
			if( !displayData )
				continue;

			var displayTier = parseInt( displayData.tier, 10 );
			if( isNaN( displayTier ) || displayTier < minTier || displayTier > maxTier )
				continue;

			SpawnCollectionDisplayObject( holder, config, displayData, displayIndex );
		}
		return;
	}

	// Fallback display for old collection files that do not define displayObjects yet.
	SpawnCollectionDisplayObject( holder, config, { tier: maxTier, itemID: 0x1E5E, x: 0, y: 0, z: maxTier }, 0 );
}

function SpawnCollectionDisplayObject( holder, config, displayData, displayIndex )
{
	if( !ValidateObject( holder ) || !config || !displayData )
		return null;

	var displayTier = parseInt( displayData.tier, 10 );
	if( isNaN( displayTier ) || displayTier < 1 )
		displayTier = 1;

	var offsetX = parseInt( displayData.x, 10 );
	var offsetY = parseInt( displayData.y, 10 );
	var offsetZ = parseInt( displayData.z, 10 );
	if( isNaN( offsetX ) )
		offsetX = 0;
	if( isNaN( offsetY ) )
		offsetY = 0;
	if( isNaN( offsetZ ) )
		offsetZ = 0;

	var spawnX = holder.x + offsetX;
	var spawnY = holder.y + offsetY;
	var spawnZ = holder.z + offsetZ;
	var spawnWorld = holder.worldnumber;
	var spawnInstance = holder.instanceID;

	// C# collection decorations use absolute world coordinates.
	// If absolute is enabled in JSON, x/y/z are used directly instead of as holder offsets.
	if( displayData.absolute == true || displayData.absolute == 1 )
	{
		spawnX = offsetX;
		spawnY = offsetY;
		spawnZ = offsetZ;
		if( typeof displayData.world != "undefined" )
		{
			spawnWorld = parseInt( displayData.world, 10 );
			if( isNaN( spawnWorld ) )
				spawnWorld = holder.worldnumber;
		}
		if( typeof displayData.instance != "undefined" )
		{
			spawnInstance = parseInt( displayData.instance, 10 );
			if( isNaN( spawnInstance ) )
				spawnInstance = holder.instanceID;
		}
	}

	var displayType = String( displayData.type || "item" ).toLowerCase();
	var displayObj = null;

	if( displayType == "npc" || displayData.npcID || displayData.npcId )
	{
		var npcID = displayData.npcID || displayData.npcId;
		if( npcID )
		{
			displayObj = SpawnNPC( String( npcID ), spawnX, spawnY, spawnZ, spawnWorld, spawnInstance, false );
		}
	}
	else
	{
		var sectionID = displayData.sectionID || displayData.sectionId;
		if( sectionID )
		{
			var sectionAmount = parseInt( displayData.amount, 10 );
			if( isNaN( sectionAmount ) || sectionAmount <= 0 )
				sectionAmount = 1;

			var sectionHue = 0;
			if( typeof displayData.hue != "undefined" && displayData.hue != null )
				sectionHue = parseInt( displayData.hue, 10 );
			else if( typeof displayData.color != "undefined" && displayData.color != null )
				sectionHue = parseInt( displayData.color, 10 );
			if( isNaN( sectionHue ) )
				sectionHue = 0;

			displayObj = CreateDFNItem( null, null, String( sectionID ), sectionAmount, "ITEM", false, sectionHue, spawnWorld, spawnInstance );
			if( ValidateObject( displayObj ) )
			{
				displayObj.Teleport( spawnX, spawnY, spawnZ, spawnWorld );
				if( displayData.name )
					displayObj.name = String( displayData.name );
				if( typeof displayData.movable != "undefined" )
					displayObj.movable = parseInt( displayData.movable, 10 );
				else
					displayObj.movable = 2;
			}
		}
		else
		{
		var itemId = displayData.itemID;
		if( typeof itemId == "undefined" || itemId == null )
			itemId = displayData.itemId;

		itemId = parseInt( itemId, 10 );
		if( isNaN( itemId ) || itemId <= 0 )
			return null;

		var amount = parseInt( displayData.amount, 10 );
		if( isNaN( amount ) || amount <= 0 )
			amount = 1;

		var hue = 0;
		if( typeof displayData.hue != "undefined" && displayData.hue != null )
			hue = parseInt( displayData.hue, 10 );
		else if( typeof displayData.color != "undefined" && displayData.color != null )
			hue = parseInt( displayData.color, 10 );
		if( isNaN( hue ) )
			hue = 0;

		var displayName = displayData.name || ( config.name + " tier " + displayTier + " display" );
		displayObj = CreateBlankItem( null, null, amount, displayName, itemId, hue, "ITEM", false );
		if( ValidateObject( displayObj ) )
		{
			displayObj.Teleport( spawnX, spawnY, spawnZ, spawnWorld );
			displayObj.name = displayName;
			displayObj.colour = hue;
			displayObj.color = hue;
			if( typeof displayData.movable != "undefined" )
				displayObj.movable = parseInt( displayData.movable, 10 );
			else
				displayObj.movable = 2;
		}
		}
	}

	if( ValidateObject( displayObj ) )
	{
		if( displayType == "npc" || displayData.npcID || displayData.npcId )
		{
			displayObj.frozen = true;
			displayObj.vulnerable = false;
			if( typeof displayData.tamable != "undefined" )
				displayObj.tamable = ( parseInt( displayData.tamable, 10 ) == 1 );
		}

		displayObj.SetTag( "communityCollectionDisplay", 1 );
		displayObj.SetTag( "collectionId", config.id );
		displayObj.SetTag( "collectionDisplayTier", displayTier );
		displayObj.SetTag( "collectionDisplayIndex", displayIndex );
		displayObj.SetTag( "collectionDisplayAnchor", holder.serial );

		if( typeof displayData.dir != "undefined" )
		{
			displayObj.dir = parseInt( displayData.dir, 10 );
		}

		if( typeof displayData.visible != "undefined" )
		{
			displayObj.visible = parseInt( displayData.visible, 10 );
		}

		if( displayData.name && displayObj.name != displayData.name )
		{
			displayObj.name = String( displayData.name );
		}
	}

	return displayObj;
}
