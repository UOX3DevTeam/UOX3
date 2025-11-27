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

//======================================================================================
//  Data tables
//======================================================================================

const myPage = [

	// Page 1 - Other
	[10611, 10612, 10613, 10614, 10615, 10616, 10617, 10618, 10619, 10620, 10688],

	// Page 2 - Furniture
	[10621, 10622, 10623, 10624, 10625, 10626, 10627, 10628, 10629, 10630, 10631, 10632, 10633],

	// Page 3 - Containers
	[10634, 10635, 10636, 10637, 10638, 10639, 10640, 10641, 10642],

	// Page 4 - Weapons
	[10643, 10644, 10645, 10646, 10647],

	// Page 5 - Armor
	[10648],

	// Page 6 - Instruments
	[10649, 10650, 10651, 10652, 10653, 10654],

	// Page 7 - Misc. Add-Ons
	[10655, 10656, 10657, 10658, 10659, 10660, 10661, 10662, 10663, 10664, 10665],

	// Page 8 - Tailoring and Cooking
	[10666, 10667, 10668, 10669, 10670, 10671, 10672, 10673, 10674, 10675, 10676, 10677],

	// Page 9 - Anvil and Forges
	[10678, 10679, 10680, 10681, 10682],

	// Page 10 - Training
	[10683, 10684, 10685, 10686]
];

const craftItems = [
	// Page 1 (button 100..110)
	[ 73, 74, 89, 90, 91, 92, 76, 77, 78, 79, 72 ],

	// Page 2 (200..212)
	[ 50, 51, 52, 53, 57, 58, 54, 55, 56, 59, 60, 61, 62 ],

	// Page 3 (300..308)
	[ 63, 64, 65, 67, 68, 69, 70, 71, 66 ],

	// Page 4 (400..404)
	[ 80, 81, 82, 123, 124 ],

	// Page 5 (500)
	[ 75 ],

	// Page 6 (600..605)
	[ 83, 84, 85, 86, 87, 88 ],

	// Page 7 (700..710)
	[ 93, 93, 94, 93, 96, 95, 97, 98, 99, 100, 101 ],

	// Page 8 (800..812)
	[ 115, 116, 107, 108, 109, 110, 117, 118, 119, 120, 121, 122 ],

	// Page 9 (900..904)
	[ 102, 103, 104, 105, 106 ],

	// Page 10 (1000..1003)
	[ 111, 112, 113, 114 ]
];

// Map: buttonID carpentry entry
//   buttonID: 100..110, 200..212, ... 1000..1003
//   CarpentryMap[buttonID] = {
//       dictID: number,
//       page:   number,
//       timerID: number,
//       makeID: number,
//       customName?: string,
//       recipeID?: number,
//       minEra?: string,
//       maxEra?: string,
//       skill: number,
//       harvest: number[]
//   };
const CarpentryMap = {};

(function initCarpentryMap()
{
	for( var pageIdx = 0; pageIdx < myPage.length; pageIdx++ )
	{
		var dictList = myPage[pageIdx];
		var makeList = craftItems[pageIdx];

		for( var i = 0; i < dictList.length && i < makeList.length; i++ )
		{
			// Button layout:
			// page 1 => 100..110
			// page 2 => 200..212
			// page 3 => 300..308
			// ...
			var buttonID = ( ( pageIdx + 1 ) * 100 ) + i;
			var dictID   = dictList[i];
			var makeID   = makeList[i];

			CarpentryMap[buttonID] = {
				dictID:   dictID,
				page:     pageIdx + 1,
				timerID:  pageIdx + 1,
				makeID:   makeID,
				// recipeID: undefined,
				// minEra:   undefined,
				// maxEra:   undefined
				skill:    carpentrySkillID,
				harvest:  [ harvestDict ] // default: wood
			};
		}
	}

	// If you need per-entry overrides in future (recipes, extra resources, era gating),
	// you can do:
	// CarpentryMap[704].harvest = [ harvestDict, 11402 ]; // wood + cloth
	// CarpentryMap[709].minEra  = "ml";
})();

CarpentryMap[109].harvest = [ harvestDict, 10016 ];	 // Fishing Pole wood + cloth
CarpentryMap[308].harvest = [ 10611, 10612, 11860 ]; // open keg Barrel Staves Barrel Hoops Barrel Lid
CarpentryMap[600].harvest = [ harvestDict, 10016 ];	 // lap harp wood + cloth
CarpentryMap[601].harvest = [ harvestDict, 10016 ];	 // Standing Harp wood + cloth
CarpentryMap[602].harvest = [ harvestDict, 10016 ];	 // Drum wood + cloth
CarpentryMap[603].harvest = [ harvestDict, 10016 ];	 // Lute wood + cloth
CarpentryMap[604].harvest = [ harvestDict, 10016 ];	 // Tambourine wood + cloth
CarpentryMap[605].harvest = [ harvestDict, 10016 ];	 // Tambourine wood + cloth
CarpentryMap[700].harvest = [ harvestDict, 10016 ];	 // Small Bed (S) wood + cloth
CarpentryMap[701].harvest = [ harvestDict, 10016 ];	 // Small Bed (E) wood + cloth
CarpentryMap[702].harvest = [ harvestDict, 10016 ];	 // Large Bed (S) wood + cloth
CarpentryMap[703].harvest = [ harvestDict, 10016 ];	 // Large Bed (E) wood + cloth
CarpentryMap[709].harvest = [ harvestDict, 10015 ];	 // Pentagram wood + ingots
CarpentryMap[710].harvest = [ harvestDict, 10015 ];	 // Abbatoir wood + ingots


CarpentryMap[800].harvest = [ harvestDict, 10016 ];	 // Dressform wood + cloth
CarpentryMap[801].harvest = [ harvestDict, 10016 ];	 // Dressform wood + cloth
CarpentryMap[802].harvest = [ harvestDict, 10016 ];	 // Spin Wheel (E) wood + cloth
CarpentryMap[803].harvest = [ harvestDict, 10016 ];	 // Spin Wheel (S) wood + cloth
CarpentryMap[804].harvest = [ harvestDict, 10016 ];	 // Loom (E) wood + cloth
CarpentryMap[805].harvest = [ harvestDict, 10016 ];	 // Loom (S) wood + cloth
CarpentryMap[806].harvest = [ harvestDict, 10015 ];	 // Stone Oven (E) wood + ingots
CarpentryMap[807].harvest = [ harvestDict, 10015 ];	 // Stone Oven (S) wood + ingots
CarpentryMap[808].harvest = [ harvestDict, 10015 ];	 // Flour Mill (E) wood + ingots
CarpentryMap[809].harvest = [ harvestDict, 10015 ];	 // Flour Mill (S) wood + ingots

CarpentryMap[900].harvest = [ harvestDict, 10015 ];	 // Small Forge wood + ingots
CarpentryMap[901].harvest = [ harvestDict, 10015 ];	 // Large Forge (E) wood + ingots
CarpentryMap[902].harvest = [ harvestDict, 10015 ];	 // Large Forge (S) wood + ingots
CarpentryMap[903].harvest = [ harvestDict, 10015 ];	 // Anvil (E) wood + ingots
CarpentryMap[904].harvest = [ harvestDict, 10015 ];	 // Anvil (S) wood + ingots

CarpentryMap[1000].harvest = [ harvestDict, 10016 ]; // Dummy (E) wood + cloth
CarpentryMap[1001].harvest = [ harvestDict, 10016 ]; // Dummy (S) wood + cloth
CarpentryMap[1002].harvest = [ harvestDict, 10016 ]; // Pickpocket (E) wood + cloth
CarpentryMap[1003].harvest = [ harvestDict, 10016 ]; // Pickpocket (S) wood + cloth


function PageX( socket, pUser, pageNum )
{
	if( !socket || !ValidateObject( pUser ))
		return;

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
		var dEntry         = CarpentryMap[detailButtonID];

		if( dEntry )
		{
			pUser.SetTempTag( "ITEMDETAILS", dEntry.makeID );

			pUser.SetTempTag( "Skill", dEntry.skill || carpentrySkillID );

			// Clear old harvests
			pUser.SetTempTag( "Harvest",  null );
			pUser.SetTempTag( "Harvest2", null );
			pUser.SetTempTag( "Harvest3", null );
			pUser.SetTempTag( "Harvest4", null );

			if( dEntry.harvest && dEntry.harvest.length > 0 )
			{
				if( dEntry.harvest.length >= 1 )
					pUser.SetTempTag( "Harvest",  dEntry.harvest[0] );
				if( dEntry.harvest.length >= 2 )
					pUser.SetTempTag( "Harvest2", dEntry.harvest[1] );
				if( dEntry.harvest.length >= 3 )
					pUser.SetTempTag( "Harvest3", dEntry.harvest[2] );
				if( dEntry.harvest.length >= 4 )
					pUser.SetTempTag( "Harvest4", dEntry.harvest[3] );
			}

			if( dEntry.recipeID && dEntry.recipeID > 0 )
				pUser.SetTempTag( "needRecipeID", dEntry.recipeID );
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