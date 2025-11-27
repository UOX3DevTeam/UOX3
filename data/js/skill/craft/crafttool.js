/// <reference path="../../definitions.d.ts" />
// @ts-check
const enableUOX3Craft  = 0;      // 0 = new system, 1 = old UOX3 menus
const BlacksmithingID  = 4023;
const CarpentryID      = 4025;
const AlchemyID        = 4028;
const FletchingID      = 4029;
const TailoringID      = 4030;
const TinkeringID      = 4032;
const CookingID        = 4034;
const CartographyID    = 4035;
const GlassblowingID   = 4036;
const MasonryID        = 4037;

/**
 * Ensure the tool is usable: charges > 0, in range, not locked down,
 * and in the user's own backpack.
 * @param {Character} pUser
 * @param {Item} iUsed
 * @returns {boolean}
 */
function checkToolUsable(pUser, iUsed)
{
	var socket = pUser.socket;
	if( !socket || !ValidateObject( iUsed ) || !iUsed.isItem )
		return false;

	if( GetServerSetting( "ToolUseLimit" ) && iUsed.usesLeft == 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 9262, socket.language )); // This has no more charges.
		return false;
	}

	if( !pUser.InRange( iUsed, 3 ))
	{
		socket.SysMessage( GetDictionaryEntry( 461, socket.language )); // You are too far away.
		return false;
	}

	if( iUsed.movable == 3 )
	{
		socket.SysMessage( GetDictionaryEntry( 6031, socket.language )); // Locked down resources cannot be used!
		return false;
	}

	var iPackOwner = GetPackOwner( iUsed, 0 );
	if( ValidateObject( iPackOwner ))
	{
		if( iPackOwner.serial != pUser.serial )
		{
			socket.SysMessage( GetDictionaryEntry( 6032, socket.language )); // That resource is in someone else's backpack!
			return false;
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 6022, socket.language )); // This has to be in your backpack before you can use it.
		return false;
	}

	return true;
}

/**
 * Open a paged crafting gump using a standard "PageX" entry point, with optional
 * special page handlers (e.g. Page8, Page20).
 *
 * @param {Character} pUser
 * @param {Socket} socket
 * @param {number} scriptID      Script ID of the crafting handler
 * @param {number} craftIndex    Value to store in "CRAFT" temp tag
 * @param {number} maxPage       Highest normal page number (handled by "PageX")
 * @param {{[page: number]: string}=} specialPages  Map: page -> function name
 */
function openCraftMenu(pUser, socket, scriptID, craftIndex, maxPage, specialPages)
{
	if( !socket )
		return;

	var gumpID   = scriptID + 0xffff;
	var tempPage = pUser.GetTempTag( "page" );

	socket.CloseGump( gumpID, 0 );
	pUser.SetTempTag( "CRAFT", craftIndex );

	if( typeof tempPage !== "number" || tempPage < 1 )
		tempPage = 1;

	// Custom page handlers (e.g. Page8, Page20)
	if( specialPages && specialPages[tempPage] )
	{
		TriggerEvent( scriptID, specialPages[tempPage], socket, pUser );
		return;
	}

	// Normal pages handled by PageX, up to maxPage
	if( tempPage >= 1 && tempPage <= maxPage )
	{
		TriggerEvent( scriptID, "PageX", socket, pUser, tempPage );
	}
	else
	{
		TriggerEvent( scriptID, "PageX", socket, pUser, 1 );
	}
}

// Tool ID helpers for readability
function isCarpentryTool( id )
{
	return (( id >= 0x1026 && id <= 0x1029 ) ||
		( id >= 0x102C && id <= 0x102F ) ||
		( id >= 0x1030 && id <= 0x1035 ) ||
		( id >= 0x10E4 && id <= 0x10E6 ));
}

function isFletchingTool( id )
{
	return ( id == 0x1022 || id == 0x1BD1 || id == 0x1BD4 );
}

function isBlacksmithTool( id )
{
	return ( id == 0x0FBB || id == 0x0FBC || id == 0x13E3 || id == 0x13E4 );
}

function isCookingTool( id )
{
	return ( id == 0x1043 || id == 0x097F || id == 0x09E2 || id == 0x103E );
}

// ---------------------------------------------------------------------------
// Main entry
// ---------------------------------------------------------------------------

/** @type { ( pUser: Character, iUsed: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( !socket )
		return false;

	if( !checkToolUsable( pUser, iUsed ))
		return false;

	// Save tool on socket so skill gumps can reference it
	socket.tempObj = iUsed;
	var id = iUsed.id;

	// -------------------------------------------------------------------
	// Carpentry
	// -------------------------------------------------------------------
	if( isCarpentryTool( id ))
	{
		if( enableUOX3Craft == 1 )
		{
			// Old UOX3 carpentry gump
			TriggerEvent( 4006, "onUseChecked", pUser, iUsed );
			return false;
		}

		// New carpentry menu – Pages 1–10
		openCraftMenu( pUser, socket, CarpentryID, 1, 10 );
		return false;
	}

	// -------------------------------------------------------------------
	// Alchemy (mortar and pestle)
	// -------------------------------------------------------------------
	if( id == 0x0E9B )
	{
		if( enableUOX3Craft == 1 )
		{
			TriggerEvent( 4007, "onUseChecked", pUser, iUsed );
			return false;
		}

		// New alchemy menu – Pages 1–4
		openCraftMenu( pUser, socket, AlchemyID, 2, 4 );
		return false;
	}

	// -------------------------------------------------------------------
	// Bowcraft / Fletching
	// -------------------------------------------------------------------
	if( isFletchingTool( id ))
	{
		if( enableUOX3Craft == 1 )
		{
			TriggerEvent( 4005, "onUseChecked", pUser, iUsed );
			return false;
		}

		// New fletching menu – Pages 1–3
		openCraftMenu( pUser, socket, FletchingID, 3, 3 );
		return false;
	}

	// -------------------------------------------------------------------
	// Tailoring (sewing kit)
	// -------------------------------------------------------------------
	if( id == 0x0F9D )
	{
		if( enableUOX3Craft == 1 )
		{
			TriggerEvent( 4004, "onUseChecked", pUser, iUsed );
			return false;
		}

		// New tailoring menu – Pages 1–8
		openCraftMenu( pUser, socket, TailoringID, 4, 8 );
		return false;
	}

	// -------------------------------------------------------------------
	// Blacksmithing (tongs, smith hammers)
	// -------------------------------------------------------------------
	if( isBlacksmithTool( id ))
	{
		if( enableUOX3Craft == 1 )
		{
			// Fall back to original blacksmith behavior when enabled
			return true;
		}

		// New blacksmithing menu – Pages 1–7 = PageX, page 8 = Page8
		openCraftMenu( pUser, socket, BlacksmithingID, 5, 7, { 8: "Page8" } );
		return false;
	}

	// -------------------------------------------------------------------
	// Cooking (skillet, flour sifter, rolling pin, etc.)
	// -------------------------------------------------------------------
	if( isCookingTool( id ))
	{
		if( enableUOX3Craft == 1 )
		{
			// Old-school cooking: use raw food with heat sources, or legacy script
			TriggerEvent( 104, "onUseChecked", pUser, iUsed );
			return false;
		}

		// New cooking menu – Pages 1–4
		openCraftMenu( pUser, socket, CookingID, 6, 4 );
		return false;
	}

	// -------------------------------------------------------------------
	// Tinkering (tinker's tools)
	// -------------------------------------------------------------------
	if( iUsed.sectionID == "tinkerstools" || // optional if you use sectionID
	    id == 0x1EB8 || id == 0x1EB9 || id == 0x1EBA || id == 0x1EBB || id == 0x1EBC )
	{
		if( enableUOX3Craft == 1 )
		{
			TriggerEvent( 4003, "onUseChecked", pUser, iUsed );
			return false;
		}

		// New tinkering menu – Pages 1–9
		openCraftMenu( pUser, socket, TinkeringID, 7, 9 );
		return false;
	}

	// -------------------------------------------------------------------
	// Cartography (mapmaker's pen)
	// -------------------------------------------------------------------
	if( iUsed.sectionID == "mapmakerspen" )
	{
		// Only one page currently – PageX with page 1
		openCraftMenu( pUser, socket, CartographyID, 8, 1 );
		return false;
	}

	// -------------------------------------------------------------------
	// Glassblowing (blow pipe)
	// -------------------------------------------------------------------
	if( iUsed.sectionID == "blowpipe" )
	{
		if( pUser.GetTag( "GlassBlowing" ) == 0 )
		{
			// NOTE: fixed .Language -> .language here
			socket.SysMessage( GetDictionaryEntry( 6300, socket.language )); // You haven't learned glassblowing.
			return false;
		}

		// New glassblowing menu – Page 1 only for now
		openCraftMenu( pUser, socket, GlassblowingID, 9, 1 );
		return false;
	}

	// -------------------------------------------------------------------
	// Masonry (mallet and chisel)
	// -------------------------------------------------------------------
	if( iUsed.sectionID == "malletandchisel" )
	{
		if( pUser.GetTag( "StoneCrafting" ) == 0 )
		{
			socket.SysMessage( GetDictionaryEntry( 6297, socket.language )); // You haven't learned masonry.
			return false;
		}

		// Masonry: Pages 1–9 = PageX, Page 20 = Page20
		openCraftMenu( pUser, socket, MasonryID, 10, 9, { 20: "Page20" } );
		return false;
	}

	return false;
}