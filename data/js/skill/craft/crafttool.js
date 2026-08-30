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
var craftToolMap = null;
var craftToolMapLoaded = false;
var craftToolMapLoadError = false;

/** @type { () => object|null } */
function LoadCraftToolMap()
{
	if( craftToolMapLoaded )
		return craftToolMap;

	craftToolMap = null;
	craftToolMapLoaded = false;
	craftToolMapLoadError = false;

	var craftToolFile = new UOXCFile();
	craftToolFile.Open( "crafttools.json", "r", "crafting", true );

	if( craftToolFile == null || craftToolFile.Length() < 0 )
	{
		Console.Error( "CraftTool system: Unable to open js/jsdata/crafting/crafttools.json" );
		craftToolMapLoadError = true;
		return null;
	}

	var fileText = "";
	while( !craftToolFile.EOF() )
	{
		var rawLine = craftToolFile.ReadUntil( "\n" );
		if( rawLine != null && typeof( rawLine ) != "undefined" )
		{
			fileText += rawLine;
		}
	}

	craftToolFile.Close();
	craftToolFile.Free();

	fileText = SanitizeCraftToolJsonText( fileText );

	try
	{
		craftToolMap = JSON.parse( fileText );
	}
	catch( error )
	{
		Console.Error( "CraftTool system: Failed to parse crafttools.json: " + error );
		Console.Error( "CraftTool system: crafttools.json length after sanitize: " + fileText.length );
		Console.Error( "CraftTool system: last char code: " + fileText.charCodeAt( fileText.length - 1 ));
		craftToolMapLoadError = true;
		return null;
	}

	if( !IsCraftToolArrayValue( craftToolMap ))
	{
		Console.Error( "CraftTool system: crafttools.json must contain a JSON array." );
		craftToolMapLoadError = true;
		return null;
	}

	craftToolMapLoaded = true;
	Console.Print( "CraftTool system: Loaded " + craftToolMap.length + " craft tool entries.\n" );

	return craftToolMap;
}

/** @type { ( pUser: Character, iUsed: Item ) => object|null } */
function GetCraftToolEntry( pUser, iUsed )
{
	var toolMap = LoadCraftToolMap();

	if( !toolMap )
		return null;

	for( var i = 0; i < toolMap.length; i++ )
	{
		var entry = toolMap[i];

		if( CraftToolMatchesEntry( iUsed, entry ))
			return entry;
	}

	return null;
}

/** @type { ( iUsed: Item, entry: object ) => boolean } */
function CraftToolMatchesEntry( iUsed, entry )
{
	if( !entry )
		return false;

	if( entry.sectionIDs && IsCraftToolArrayValue( entry.sectionIDs ))
	{
		for( var i = 0; i < entry.sectionIDs.length; i++ )
		{
			if( iUsed.sectionID == entry.sectionIDs[i] )
				return true;
		}
	}

	if( entry.toolIDs && IsCraftToolArrayValue( entry.toolIDs ))
	{
		for( var j = 0; j < entry.toolIDs.length; j++ )
		{
			if( iUsed.id == entry.toolIDs[j] )
				return true;
		}
	}

	return false;
}

/** @type { ( value: any ) => boolean } */
function IsCraftToolArrayValue( value )
{
	return Object.prototype.toString.call( value ) == "[object Array]";
}

/** @type { ( text: string ) => string } */
function SanitizeCraftToolJsonText( text )
{
	if( text == null || typeof( text ) == "undefined" )
		return "";

	text = String( text );

	if( text.length > 0 && text.charCodeAt( 0 ) == 65279 )
		text = text.substring( 1 );

	text = text.split( "\r\n" ).join( "\n" );
	text = text.split( "\r" ).join( "\n" );
	text = text.split( String.fromCharCode( 160 ) ).join( " " );
	text = text.split( String.fromCharCode( 255 ) ).join( "" );
	text = text.split( "\t" ).join( " " );

	text = TrimCraftToolString( text );

	var lastBracket = text.lastIndexOf( "]" );
	if( lastBracket >= 0 )
		text = text.substring( 0, lastBracket + 1 );

	return TrimCraftToolString( text );
}

/** @type { ( text: string ) => string } */
function TrimCraftToolString( text )
{
	if( text == null || typeof( text ) == "undefined" )
		return "";

	return text.replace( /^\s+|\s+$/g, "" );
}

/** @type { ( specialPages: object ) => object|null } */
function BuildSpecialPageMap( specialPages )
{
	if( !specialPages || !IsCraftToolArrayValue( specialPages ))
		return null;

	var pageMap = {};

	for( var i = 0; i < specialPages.length; i++ )
	{
		var entry = specialPages[i];

		if( entry && typeof entry.page != "undefined" && entry["function"] )
		{
			pageMap[entry.page] = entry["function"];
		}
	}

	return pageMap;
}

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

	var craftToolEntry = GetCraftToolEntry( pUser, iUsed );

	if( !craftToolEntry )
		return false;

	if( craftToolEntry.requiredTag && pUser.GetTag( craftToolEntry.requiredTag ) == 0 )
	{
		if( craftToolEntry.requiredTagMessage )
			socket.SysMessage( GetDictionaryEntry( craftToolEntry.requiredTagMessage, socket.language ));

		return false;
	}

	if( enableUOX3Craft == 1 )
	{
		if( craftToolEntry.legacyReturnTrue )
			return true;

		if( craftToolEntry.legacyScriptID )
		{
			TriggerEvent( craftToolEntry.legacyScriptID, "onUseChecked", pUser, iUsed );
			return false;
		}
	}

	if( craftToolEntry.craft )
		pUser.SetTempTag( "CUSTOMCRAFT", craftToolEntry.craft );

	openCraftMenu(
		pUser,
		socket,
		craftToolEntry.scriptID,
		craftToolEntry.craftIndex,
		craftToolEntry.maxPage,
		BuildSpecialPageMap( craftToolEntry.specialPages )
	);

	return false;
}