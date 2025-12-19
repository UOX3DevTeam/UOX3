/// <reference path="../definitions.d.ts" />
// @ts-check
// add this script number to make lost items script=8010// honesty item

const honestyVirtueEnabled = GetServerSetting( "HonestyVirtueEnabled" );

var VirtueName = VirtueName || {
	Humility:     0,
	Sacrifice:    1,
	Compassion:   2,
	Spirituality: 3,
	Valor:        4,
	Honor:        5,
	Justice:      6,
	Honesty:      7
};

var HONESTY_ITEM_LIFETIME_MS = 3 * 60 * 60 * 1000;
var HONESTY_GAIN_ANY_TOWN = 30;
var HONESTY_GAIN_CORRECT_TOWN = 60;
var HonestyTownRegions = {
	"Britain":   3,
	"Yew":       4,
	"Moonglow":  5,
	"Trinsic":   6,
	"Skara Brae":7,
	"New Magincia":8,
	"Jhelom":    9,
	"Minoc":     10
};

function Honesty_OnLostItemCreated( item, townName, ownerName )
{
	if( !ValidateObject( item ))
		return;

	var now = GetCurrentClock() | 0;

	item.SetTag( "HonestyQuest", 1 );
	item.SetTag( "HonestyTown", townName ? townName : "" );
	item.SetTag( "HonestyOwner", ownerName ? ownerName : "" );
	item.SetTag( "HonestyExpire", ( now + HONESTY_ITEM_LIFETIME_MS ).toString() );

	// Add hint to name if you want:
	// In OSI it says "Lost Item (Return To Gain Honesty)" in the name/tooltip.
	var baseName = item.name;
	if( !baseName || baseName.length === 0 )
		baseName = "lost item";

	item.name = baseName + " (return to gain Honesty)";
}

function Honesty_CheckLostItem( item )
{
	var r = { valid: false, expired: false, reason: "" };

	if( !ValidateObject( item ) || !item.isItem )
	{
		r.reason = "Not an item.";
		return r;
	}

	var flag = item.GetTag( "HonestyQuest" );
	if( flag !== 1 && flag !== "1" )
	{
		r.reason = "Not a Honesty quest item.";
		return r;
	}

	var expireTag = item.GetTag( "HonestyExpire" );
	var expireTime = Number( expireTag ) | 0;
	if( !expireTag || isNaN( expireTime ))
	{
		// If no expire set, treat as invalid quest item
		r.reason = "Honesty item missing expire tag.";
		return r;
	}

	var now = GetCurrentClock() | 0;
	if( now > expireTime )
	{
		r.expired = true;
		r.reason = "The time limit for returning this item has passed.";
		return r;
	}

	r.valid = true;
	return r;
}

function Honesty_TurnInLostItem( pUser, item, boxTown )
{
	if( !ValidateObject( pUser ) || !ValidateObject( item ))
		return;

	var pSocket = pUser.socket;
	if( !pSocket )
		return;

	var check = Honesty_CheckLostItem( item );
	if( !check.valid )
	{
		if( check.expired )
		{
			pSocket.SysMessage( GetDictionaryEntry( 30043, pSocket.language )); // This item is no longer a Honesty quest item.

		}
		else
		{
			pSocket.SysMessage( GetDictionaryEntry( 30044, pSocket.language )); // This is not a lost item.
		}
		return;
	}

	var itemTown = item.GetTag( "HonestyTown" ) || "";
	var ownerName = item.GetTag( "HonestyOwner" ) || "";

	// Determine base gain
	var gain = HONESTY_GAIN_ANY_TOWN;

	// Double credit if the item town matches the box town (case insensitive)
	if( boxTown && itemTown &&
		boxTown.toLowerCase() === itemTown.toLowerCase() )
	{
		gain = HONESTY_GAIN_CORRECT_TOWN;
	}

	// In OSI, turning in to the exact owner yields even more (x4).
	// We can add that later once you hook NPC turn in. For now, box only.

	// Award Honesty through helper
	var result = TriggerEvent( 8003, "Virtue_Award", pUser, VirtueName.Honesty, gain );

	// "You place the item in the lost and found. You have gained some Honesty!"
	pSocket.SysMessage( GetDictionaryEntry( 30045, pSocket.language ));

	if( result && result.success && result.gainedPath )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30046, pSocket.language )); // You have gained a path in Honesty!
	}

	// Consume the item
	item.Delete();
}

/** @type { ( thingCreated: BaseObject, thingType: 0 | 1 ) => void } */
function onCreateDFN( objMade, objType )
{
	if( !honestyVirtueEnabled )
		return;

	if( !ValidateObject( objMade ) || !objMade.isItem )
		return;

	// If this is a Lost & Found box, do NOT treat it as a lost item
	var boxTown = objMade.GetTag( "LostTownBox" );
	if( boxTown === 1 )
	{
		// It is a bank box, skip lost-item logic
		return;
	}

	// 1) Check if DFN already specified town/owner explicitly
	var town  = objMade.GetTag( "HonestyTown" );
	var owner = objMade.GetTag( "HonestyOwner" );

	// 2) No explicit town set. Pick a random town from HonestyTownRegions.
	var townNames = [];
	for( var key in HonestyTownRegions )
	{
		if( HonestyTownRegions.hasOwnProperty( key ))
			townNames.push( key );
	}

	if( townNames.length === 0 )
	{
		// No towns configured; give up
		return;
	}

	var idx = RandomNumber( 0, townNames.length - 1 );
	town = townNames[idx];

	// Store on item for later use
	objMade.SetTag("HonestyTown", town);

	// 3) If owner not set yet, try to pick a random vendor from that town region
	var vend = Honesty_FindVendorForTown( town );
	if( ValidateObject( vend ))
	{
		owner = vend.name;
		objMade.SetTag( "HonestyOwner", owner );
	}

	// 4) Finally, mark the item as a Honesty quest item
	Honesty_OnLostItemCreated( objMade, town.toString(), owner ? owner.toString() : "" );
}

function onSpawn( objMade, spawnRegion  )
{
	if( !honestyVirtueEnabled )
		return;

	if( !ValidateObject( objMade ) || !objMade.isItem )
		return;

	// If this is a Lost & Found box, do NOT treat it as a lost item
	var boxTown = objMade.GetTag( "LostTownBox" );
	if( boxTown === 1 )
	{
		// It is a bank box, skip lost-item logic
		return;
	}

	// 1) Check if DFN already specified town/owner explicitly
	var town  = objMade.GetTag( "HonestyTown" );
	var owner = objMade.GetTag( "HonestyOwner" );

	// 2) No explicit town set. Pick a random town from HonestyTownRegions.
	var townNames = [];
	for( var key in HonestyTownRegions )
	{
		if( HonestyTownRegions.hasOwnProperty( key ))
			townNames.push( key );
	}

	if( townNames.length === 0 )
	{
		// No towns configured; give up
		return;
	}

	var idx = RandomNumber( 0, townNames.length - 1 );
	town = townNames[idx];

	// Store on item for later use
	objMade.SetTag( "HonestyTown", town );

	// 3) If owner not set yet, try to pick a random vendor from that town region
	var vend = Honesty_FindVendorForTown( town );
	if( ValidateObject( vend ))
	{
		owner = vend.name;
		objMade.SetTag( "HonestyOwner", owner );
	}

	// 4) Finally, mark the item as a Honesty quest item
	Honesty_OnLostItemCreated( objMade, town.toString(), owner ? owner.toString() : "" );
}

// Internal search state for vendor lookup
var Honesty_FindVendorTargetRegionID = -1;
var Honesty_FoundVendors = [];

function Honesty_IsVendor( ch )
{
	if( !ValidateObject( ch ) || !ch.npc )
		return false;

	var vendTag = ch.GetTag( "HonestyVendor" );
	if( vendTag === 1 || vendTag === "1" )
		return true;

	return false;
}

function Honesty_FindVendorForTown( townName )
{
	var regionID = HonestyTownRegions[townName];
	if( regionID === undefined )
		return null;

	Honesty_FoundVendors = [];
	Honesty_FindVendorTargetRegionID = regionID;

	// This will call onIterate for every CHARACTER in the world
	IterateOver( "CHARACTER" );

	Honesty_FindVendorTargetRegionID = -1;

	if( Honesty_FoundVendors.length === 0 )
		return null;

	var idx = RandomNumber( 0, Honesty_FoundVendors.length - 1 );
	return Honesty_FoundVendors[idx];
}

/** @type { ( obj: Character | Item, mSock: Socket ) => boolean } */
function onIterate( charCheck )
{
	if( !ValidateObject( charCheck ))
		return false;

	// If we are not currently doing a Honesty vendor search, bail
	if( Honesty_FindVendorTargetRegionID === -1 )
		return false;

	if( !charCheck.npc )
		return false;

	var reg = charCheck.region;
	if( !reg )
		return false;

	// Compare region id to our target town region
	if( reg.id !== Honesty_FindVendorTargetRegionID )
		return false;

	// Check if this NPC should count as "vendor"
	if( !Honesty_IsVendor( charCheck ))
		return false;

	// Collect this vendor
	Honesty_FoundVendors.push( charCheck );

	// We return false so IterateOver keeps going and finds more options
	return false;
}

/** @type { ( myObj: BaseObject, pSocket: Socket ) => string } */
function onTooltip( myObj, pSocket )
{
	if( !ValidateObject( myObj ) || !myObj.isItem )
		return "";

	// If this is a Lost & Found box, do NOT treat it as a lost item
	var boxTown = myObj.GetTag( "LostTownBox" );
	if( boxTown === 1 )
	{
		// It is a bank box, skip lost-item logic
		return "";
	}

	// Only show tooltip for Honesty quest items
	var flag = myObj.GetTag( "HonestyQuest" );
	if( flag !== 1 && flag !== "1" )
		return "";

	var text = "A lost item. Return it to a town's lost and found box to gain Honesty.";

	// Town hint (from HonestyTown tag)
	var town = myObj.GetTag( "HonestyTown" );
	if( town && town !== "" )
	{
		text += "\nIt seems to be from the town of " + town + ".";
	}

	// Owner hint (from HonestyOwner tag, if you want this visible)
	var owner = myObj.GetTag( "HonestyOwner" );
	if( owner && owner !== "" )
	{
		text += "\nIt appears to belong to someone named " + owner + ".";
	}

	// Time remaining before it stops being a quest item
	var expireTag = myObj.GetTag( "HonestyExpire" );
	if( expireTag !== null && expireTag !== undefined )
	{
		var expireTime = Number( expireTag );
		if( !isNaN( expireTime ))
		{
			var now = GetCurrentClock() | 0;
			var remainingMs = expireTime - now;

			if( remainingMs <= 0 )
			{
				text += "\nThe time limit for returning this item has passed.";
			}
			else
			{
				var mins = Math.floor( remainingMs / 60000 );
				if( mins < 1 )
				{
					text += "\nYou have less than a minute to return this item.";
				}
				else
				{
					text += "\nYou have about " + mins + " minute";
					if( mins !== 1 )
						text += "s";
					text += " to return this item.";
				}
			}
		}
	}

	// If you wanted to force a cliloc template (like uses remaining: %i):
	// myObj.SetTempTag( "clilocTooltip", 1042971 ); // ~1_NOTHING~
	// But plain text is fine, so we just return the text.

	return text;
}
