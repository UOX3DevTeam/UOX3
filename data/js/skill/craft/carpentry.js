/// <reference path="../../definitions.d.ts" />
// @ts-check
const textHue					= 0x480;    // Color of the text.
const carpentryID				= 4025;     // Use this to tell the gump what script to close.
const gumpDelay					= 2000;     // Timer for the gump to reappear after crafting.
const repairDelay				= 200;      // Timer for the gump to reappear after repairing an item.
const itemDetailsScriptID		= 4026;     // Generic item detail gump
const craftGumpID				= 4027;     // Crafting frame gump
const itemsPerPage				= 10;		// Number of craftable items shown per gump subpage
const displayUnlearnedRecipes	= true;		// For future recipe use
const coreShardEra				= EraStringToNum( GetServerSetting( "CoreShardEra" ) );
const carpentrySkillID			= 11;		// Carpentry skill ID
const harvestDict				= 10014;	// Dictionary entry for "wood" (boards/logs)

const craftMapRegistryID = 4038;
var CarpentryMap = {};

function LoadCarpentryMap()
{
	CarpentryMap = {};

	var carpentryEntries = TriggerEvent( craftMapRegistryID, "CraftMapRegistry", "carpentry" );

	if( !carpentryEntries || !IsCarpentryArrayValue( carpentryEntries ) )
	{
		Console.Warning( "Carpentry: Unable to load carpentry craft map data." );
		return false;
	}

	for( var i = 0; i < carpentryEntries.length; i++ )
	{
		var entry = carpentryEntries[i];

		if( !entry || typeof entry.buttonID == "undefined" )
			continue;

		if( entry.skill === undefined )
			entry.skill = carpentrySkillID;

		if( !entry.harvest )
			entry.harvest = [ harvestDict ];

		CarpentryMap[entry.buttonID] = entry;
	}

	Console.Print( "Carpentry: Loaded " + carpentryEntries.length + " craft map entries.\n" );
	return true;
}

function IsCarpentryArrayValue( value )
{
	return Object.prototype.toString.call( value ) == "[object Array]";
}

function PageX( socket, pUser, pageNum )
{
	if( !socket || !ValidateObject( pUser ))
		return;

	if( !CarpentryMap || Object.keys( CarpentryMap ).length == 0 )
	{
		if( !LoadCarpentryMap() )
		{
			socket.SysMessage( "Carpentry craft map failed to load." );
			return;
		}
	}

	var pageItems = [];

	if( pageNum == 999 )
	{
		var lastRaw = pUser.GetTempTag( "LastTenCarpentry" ) || "";
		var split   = lastRaw.split( "," );
		for( var i = 0; i < split.length; i++ )
		{
			var val = parseInt( split[i] );
			if( !isNaN( val ) && CarpentryMap[val] )
				pageItems.push( val );
		}
	}
	else
	{
		for( var buttonIDStr in CarpentryMap )
		{
			if( !CarpentryMap.hasOwnProperty( buttonIDStr ))
				continue;

			var buttonID = parseInt( buttonIDStr );
			var data     = CarpentryMap[buttonID];

			if( data.page == pageNum && eraOK( data ))
			{
				var needsRecipe = data.recipeID;
				var showAll     = displayUnlearnedRecipes;

				if( !needsRecipe || showAll || HasLearnedRecipe( pUser, needsRecipe ))
				{
					pageItems.push( buttonID );
				}
			}
		}

		pageItems.sort(function(a, b){ return a - b; });
	}

	if( pageItems.length == 0 )
	{
		var emptyGump = new Gump;
		TriggerEvent( craftGumpID, "CraftingGumpMenu", emptyGump, socket );
		emptyGump.AddPage( 1 );
		emptyGump.AddText( 220, 60, textHue, "No items available on this page." );
		emptyGump.Send( socket );
		emptyGump.Free();
		return;
	}

	var subPage       = pUser.GetTempTag( "subPage" );
	var totalSubPages = Math.ceil( pageItems.length / itemsPerPage );

	if( totalSubPages < 1 )
		totalSubPages = 1;

	if( subPage < 1 )
		subPage = 1;

	if( subPage > totalSubPages )
		subPage = totalSubPages;

	pUser.SetTempTag( "page", pageNum );
	pUser.SetTempTag( "subPage", subPage );

	var startIndex = ( subPage - 1 ) * itemsPerPage;
	var endIndex   = Math.min( startIndex + itemsPerPage, pageItems.length );

	if( startIndex >= pageItems.length )
	{
		subPage    = 1;
		startIndex = 0;
		endIndex   = Math.min( itemsPerPage, pageItems.length );
		pUser.SetTempTag( "subPage", subPage );
	}

	var carpGump = new Gump;
	TriggerEvent( craftGumpID, "CraftingGumpMenu", carpGump, socket );
	carpGump.AddPage( 1 );

	var drawIndex = 0;

	for( var i = startIndex; i < endIndex; i++ )
	{
		var buttonID = pageItems[i];
		var data     = CarpentryMap[buttonID];

		var entryText = "";
		if( data.customName )
		{
			entryText = data.customName;
		}
		else if( data.dictID )
		{
			entryText = GetDictionaryEntry( data.dictID, socket.language );
			if( !entryText || entryText === "" )
				entryText = "[Missing EntryID: " + data.dictID + "]";
		}
		else
		{
			entryText = "[Unnamed Item: " + buttonID + "]";
		}

		carpGump.AddButton( 220, 60 + ( drawIndex * 20 ), 4005, 4007, 1, 0, buttonID );
		carpGump.AddText(   255, 60 + ( drawIndex * 20 ), textHue, entryText );

		carpGump.AddButton( 480, 60 + ( drawIndex * 20 ), 4011, 4012, 1, 0, 2000 + buttonID );

		drawIndex++;
	}

	// Prev subpage
	if( subPage > 1 )
	{
		carpGump.AddButton( 220, 260, 4014, 4015, 1, 0, 8000 + ( subPage - 1 ));
		carpGump.AddHTMLGump( 255, 263, 100, 18, 0, 0,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" ); // PREV PAGE
	}

	// Next subpage
	if( subPage < totalSubPages )
	{
		carpGump.AddButton( 370, 260, 4005, 4007, 1, 0, 9000 + ( subPage + 1 ));
		carpGump.AddHTMLGump( 405, 263, 100, 18, 0, 0,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" ); // NEXT PAGE
	}

	carpGump.Send( socket );
	carpGump.Free();
}

function RepairTarget( pSock )
{
	pSock.CustomTarget( 2, GetDictionaryEntry( 485, pSock.language )); // What item would you like to repair?
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback2( pSock, ourObj )
{
	// Repair Item
	var mChar = pSock.currentChar;

	// Don't continue if character is invalid, or worse... dead!
	if( !ValidateObject( mChar ) || mChar.dead )
		return;

	var bItem  = pSock.tempObj;
	var gumpID = carpentryID + 0xffff;
	pSock.tempObj = null;

	if( ValidateObject( mChar ) && mChar.isChar && ValidateObject( bItem ) && bItem.isItem )
	{
		if( !ValidateObject( ourObj ) || !ourObj.isItem
			|| TriggerEvent( 2506, "GetItemMaterialType", ourObj ) != "wood"
			|| !CheckTileFlag( ourObj.id, 22 )) // TF_WEARABLE
		{
			// Targeted object is not an item that can be repaired
			pSock.tempObj = bItem;
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "CANTREPAIR" );
			mChar.StartTimer( repairDelay, 1, true );
			return;
		}

		var itemDurabilityLossEnabled = GetServerSetting( "ItemRepairDurabilityLoss" );
		var ownerObj = GetPackOwner( ourObj, 0 );

		if( ownerObj && mChar.serial == ownerObj.serial )
		{
			var maxHitpoints     = ourObj.maxhp;
			var currentHitpoints = ourObj.health;

			if( currentHitpoints < maxHitpoints )
			{
				var deltaHP          = maxHitpoints - currentHitpoints;
				var repairDifficulty = (( deltaHP / maxHitpoints ) * 1000 );
				var minDifficulty    = repairDifficulty - 250;
				var skillBonus       = 0;
				var repairSkill      = mChar.skills.carpentry;

				if( minDifficulty < 0 )
				{
					skillBonus    = minDifficulty * -1;
					minDifficulty = 0;
				}
				else if( minDifficulty > repairSkill )
				{
					pSock.tempObj = bItem;
					pSock.CloseGump( gumpID, 0 );
					mChar.SetTempTag( "prevActionResult", "CANTREPAIR" );
					mChar.StartTimer( repairDelay, 1, true );
					return;
				}

				var maxDifficulty = Math.min( repairDifficulty + 250, mChar.skillCaps.carpentry );

				if( RandomNumber( minDifficulty, 1000 ) < ( Math.max( repairSkill + skillBonus, 999 )))
				{
					if( RandomNumber( 1, 5 ) == 1 )
						mChar.CheckSkill( 11, minDifficulty, maxDifficulty ); // carpentry

					if( itemDurabilityLossEnabled )
						ourObj.maxhp -= 1;

					ourObj.health = ourObj.maxhp;
					pSock.SoundEffect( 0x023D, true );

					pSock.CloseGump( gumpID, 0 );
					mChar.SetTempTag( "prevActionResult", "REPAIRSUCCESS" );
					mChar.StartTimer( repairDelay, 1, true );
				}
				else
				{
					if( repairSkill >= 1000 )
						ourObj.health -= 1;
					else if( repairSkill >= 715 )
						ourObj.health -= 2;
					else
						ourObj.health -= 3;

					if( ourObj.health <= 0 )
					{
						pSock.SysMessage( GetDictionaryEntry( 311, pSock.language ).replace(/%s/gi, ourObj.name )); // destroyed
						ourObj.Delete();
					}

					pSock.tempObj = bItem;
					pSock.CloseGump( gumpID, 0 );
					mChar.SetTempTag( "prevActionResult", "FAILREPAIR" );
					mChar.StartTimer( repairDelay, 1, true );
				}
			}
			else
			{
				pSock.tempObj = bItem;
				pSock.CloseGump( gumpID, 0 );
				mChar.SetTempTag( "prevActionResult", "FULLREPAIR" );
				mChar.StartTimer( repairDelay, 1, true );
			}
		}
		else
		{
			pSock.tempObj = bItem;
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "CHECKPACK" );
			mChar.StartTimer( repairDelay, 1, true );
		}
	}
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( pUser, timerID )
{
	if( !ValidateObject( pUser ))
		return;

	var socket = pUser.socket;
	if( socket == null )
		return;

	if( timerID >= 1 && timerID <= 10 )
	{
		PageX( socket, pUser, timerID );
	}
	else if( timerID == 999 )
	{
		PageX( socket, pUser, 999 );
	}
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( pSock, pButton, gumpData )
{
	if( pSock == null )
		return;

	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) || pUser.dead )
		return;

	var bItem = pSock.tempObj;
	if( !ValidateObject( bItem ) || !pUser.InRange( bItem, 3 ))
	{
		pSock.SysMessage( GetDictionaryEntry( 461, pSock.language )); // too far
		return;
	}

	if( bItem.movable == 3 )
	{
		pSock.SysMessage( GetDictionaryEntry( 6031, pSock.language )); // locked down
		return;
	}

	var iPackOwner = GetPackOwner( bItem, 0 );
	if( ValidateObject( iPackOwner ))
	{
		if( iPackOwner.serial != pUser.serial )
		{
			pSock.SysMessage( GetDictionaryEntry( 6032, pSock.language )); // someone else's pack
			return;
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 6022, pSock.language )); // must be in pack
		return;
	}

	var gumpID = carpentryID + 0xffff;

	// Subpage back / forward
	if( pButton >= 8001 && pButton < 9000 )
	{
		var subPage = pButton - 8000;
		var pageNum = pUser.GetTempTag( "page" );
		pUser.SetTempTag( "subPage", subPage );
		PageX( pSock, pUser, pageNum );
		return;
	}

	if( pButton >= 9001 && pButton < 10000 )
	{
		var subPage2 = pButton - 9000;
		var pageNum2 = pUser.GetTempTag( "page" );
		pUser.SetTempTag( "subPage", subPage2 );
		PageX( pSock, pUser, pageNum2 );
		return;
	}

	// Page tabs 1..10
	if( pButton >= 1 && pButton <= 10 )
	{
		pUser.SetTempTag( "page", pButton );
		pUser.SetTempTag( "subPage", 1 );
		PageX( pSock, pUser, pButton );
		return;
	}

	// Last Ten
	if( pButton == 11000 )
	{
		pUser.SetTempTag( "page", 999 );
		pUser.SetTempTag( "subPage", 1 );
		PageX( pSock, pUser, 999 );
		return;
	}

	// Close gump
	if( pButton == 0 )
	{
		pUser.SetTempTag( "MakeLast_Carpentry", null );
		pUser.SetTempTag( "CRAFT", null );
		pSock.CloseGump( gumpID, 0 );
		return;
	}

	// Make Last
	if( pButton == 5000 )
	{
		var last = pUser.GetTempTag( "MakeLast_Carpentry" );
		if( last )
			pButton = last;
		else
			return;
	}

	if( CarpentryMap[pButton] !== undefined )
	{
		var entry   = CarpentryMap[pButton];
		var makeID  = entry.makeID;
		var timerID = entry.timerID || entry.page || 1;

		if( !eraOK( entry ))
		{
			pSock.SysMessage( "That item is not available in this era." );
			return;
		}

		if( entry.recipeID && !TriggerEvent( 4022, "NeedRecipe", pUser, entry.recipeID ))
		{
			pSock.SysMessage( "You must learn that recipe from a scroll." );
			return;
		}

		pUser.SetTempTag( "MakeLast_Carpentry", pButton );

		// Let crafting_complete.js hook things like locks on containers
		pUser.AddScriptTrigger( 4033 );

		MakeItem( pSock, pUser, makeID );
		AddToLastTen( pUser, pButton );

		if( GetServerSetting( "ToolUseLimit" ))
		{
			bItem.usesLeft -= 1;
			if( bItem.usesLeft == 0 && GetServerSetting( "ToolUseBreak" ))
			{
				bItem.Delete();
				pSock.SysMessage( GetDictionaryEntry( 10202, pSock.language )); // tool worn out
			}
		}

		pUser.StartTimer( gumpDelay, timerID, carpentryID );
		return;
	}

	// Item detail buttons: 2000 + buttonID
	if( pButton >= 2000 && pButton < 4000 )
	{
		var detailButtonID = pButton - 2000;
		var entry         = CarpentryMap[detailButtonID];

		if( entry )
		{
			pUser.SetTempTag( "ITEMDETAILS", entry.makeID );

			pUser.SetTempTag( "Skill", entry.skill || carpentrySkillID );

			// Clear old harvests
			pUser.SetTempTag( "Harvest",  null );
			pUser.SetTempTag( "Harvest2", null );
			pUser.SetTempTag( "Harvest3", null );
			pUser.SetTempTag( "Harvest4", null );

			// Clear old harvest names
			pUser.SetTempTag( "HarvestName",  null );
			pUser.SetTempTag( "Harvest2Name", null );
			pUser.SetTempTag( "Harvest3Name", null );
			pUser.SetTempTag( "Harvest4Name", null );

			if( entry.harvest && entry.harvest.length > 0 )
			{
				if( entry.harvest.length >= 1 )
					pUser.SetTempTag( "Harvest",  entry.harvest[0] );
				if( entry.harvest.length >= 2 )
					pUser.SetTempTag( "Harvest2", entry.harvest[1] );
				if( entry.harvest.length >= 3 )
					pUser.SetTempTag( "Harvest3", entry.harvest[2] );
				if( entry.harvest.length >= 4 )
					pUser.SetTempTag( "Harvest4", entry.harvest[3] );
			}

			// OPTIONAL custom names – these override the dictionary string
			if( entry.harvestNames && entry.harvestNames.length > 0 )
			{
				if( entry.harvestNames.length >= 1 )
					pUser.SetTempTag( "HarvestName",  entry.harvestNames[0] );
				if( entry.harvestNames.length >= 2 )
					pUser.SetTempTag( "Harvest2Name", entry.harvestNames[1] );
				if( entry.harvestNames.length >= 3 )
					pUser.SetTempTag( "Harvest3Name", entry.harvestNames[2] );
				if( entry.harvestNames.length >= 4 )
					pUser.SetTempTag( "Harvest4Name", entry.harvestNames[3] );
			}

			if( entry.recipeID && entry.recipeID > 0 )
				pUser.SetTempTag( "needRecipeID", entry.recipeID );
			else
				pUser.SetTempTag( "needRecipeID", 0 );

			TriggerEvent( itemDetailsScriptID, "ItemDetailGump", pUser );
		}
		return;
	}
}

function AddToLastTen( pUser, buttonID )
{
	var raw  = pUser.GetTempTag( "LastTenCarpentry" ) || "";
	var list = raw.split( "," );

	for( var i = 0; i < list.length; i++ )
	{
		if( parseInt( list[i] ) == buttonID )
		{
			list.splice( i, 1 );
			break;
		}
	}

	var newList = [ buttonID ];
	for( var j = 0; j < list.length && newList.length < 10; j++ )
	{
		var entry = parseInt( list[j] );
		if( !isNaN( entry ) && entry > 0 )
			newList.push( entry );
	}

	pUser.SetTempTag( "LastTenCarpentry", newList.join( "," ) );
}

function HasLearnedRecipe( pUser, recipeID )
{
	var myData = TriggerEvent( 4022, "ReadRecipeID", pUser );
	if( !myData || myData.length == 0 )
		return false;

	for( var i = 0; i < myData.length; i++ )
	{
		var data = myData[i].split( "," );
		if( data[0] == recipeID )
			return true;
	}
	return false;
}

function eraOK( entry )
{
	if( entry.minEra && coreShardEra < EraStringToNum( entry.minEra ))
		return false;
	if( entry.maxEra && coreShardEra > EraStringToNum( entry.maxEra ))
		return false;
	return true;
}