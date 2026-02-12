/// <reference path="../definitions.d.ts" />
// @ts-check
const LargeBODID = 5080;

/** @type {( pUser: Character, largeBOD: Item ) => boolean} */
function onUseChecked( pUser, largeBOD )
{
	const socket = pUser.socket;
	if( socket == null )
		return false;

	// Must be in pack or locked down – mirror small BOD behaviour
	const inPack    = ( largeBOD.container === pUser.pack );
	const lockedDown = ( largeBOD.movable === 3 || largeBOD.movable === 2 );

	if( !inPack && !lockedDown )
	{
		socket.SysMessage( GetDictionaryEntry( 17259, socket.language )); // You must have the item in your backpack to target it.
		return false;
	}

	// Ensure base tags exist
	InitLargeBODTags( largeBOD );

	// Open the large BOD gump
	const gumpID = LargeBODID + 0xffff;
	socket.CloseGump( gumpID, 0 );
	LargeBODGump( pUser, largeBOD );

	return false;
}

/** @param {Item} largeBOD */
function InitLargeBODTags( largeBOD )
{
	if( largeBOD.GetTag( "init" ) === true )
		return;

	// Base properties should normally already be set by the generator,
	// but we at least make sure reward-related tags exist.
	if( largeBOD.GetTag( "goldValue" )   == null ) largeBOD.SetTag( "goldValue",   0 );
	if( largeBOD.GetTag( "fameValue" )   == null ) largeBOD.SetTag( "fameValue",   0 );
	if( largeBOD.GetTag( "qualityValue" )== null ) largeBOD.SetTag( "qualityValue",0 );
	if( largeBOD.GetTag( "amountCur" )   == null ) largeBOD.SetTag( "amountCur",   0 );

	const entryCount = largeBOD.GetTag( "entryCount" );
	for( let i = 0; i < entryCount; i++ )
	{
		const keyAmt = "entry" + i + "_amount";
		if( largeBOD.GetTag( keyAmt ) == null )
			largeBOD.SetTag( keyAmt, 0 );
	}

	largeBOD.SetTag( "init", true );
	largeBOD.Refresh();
}

/** @param {Character} pUser @param {Item} largeBOD */
function LargeBODGump( pUser, largeBOD )
{
	const socket = pUser.socket;
	if( socket == null )
		return;

	const amountMax      = largeBOD.GetTag( "amountMax" );
	const reqExceptional = largeBOD.GetTag( "reqExceptional" );
	const materialColor  = largeBOD.GetTag( "materialColor" );
	const entryCount     = largeBOD.GetTag( "entryCount" );

	const largeBODGump = new Gump();

	pUser.largeBOD = largeBOD; // stash reference for callbacks

	largeBODGump.AddPage( 0 );

	// Dynamic height based on entry count and special requirements
	let extraHeight = 0;
	if( reqExceptional || materialColor > 0 )
	{
		extraHeight += 24; // "Special requirements to meet:"
		if( reqExceptional )
			extraHeight += 24;
		if( materialColor > 0 )
			extraHeight += 24;
	}

	const totalHeight = 218 + extraHeight + ( entryCount * 24 );

	largeBODGump.AddBackground( 50, 10, 455, totalHeight, 5054 );
	largeBODGump.AddTiledGump( 58, 20, 438, totalHeight - 18, 2624 );
	largeBODGump.AddCheckerTrans( 58, 20, 438, totalHeight - 18 );

	largeBODGump.AddGump( 45, 5, 10460 );
	largeBODGump.AddGump( 480, 5, 10460 );
	largeBODGump.AddGump( 45, totalHeight - 15, 10460 );
	largeBODGump.AddGump( 480, totalHeight - 15, 10460 );

	largeBODGump.AddHTMLGump( 225, 25, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17285, socket.language ) + "</basefont>" ); // A Large bulk order

	largeBODGump.AddHTMLGump( 75, 48, 250, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17251, socket.language ) + "</basefont>" ); // Amount to make:
	largeBODGump.AddText( 275, 48, 1152, amountMax.toString() );

	largeBODGump.AddHTMLGump( 75, 72, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17253, socket.language ) + "</basefont>" ); // Items requested:
	largeBODGump.AddHTMLGump( 275, 76, 200, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17252, socket.language ) + "</basefont>" ); // Amount finished:

	let y = 96;
	for( let i = 0; i < entryCount; i++ )
	{
		const itemName = largeBOD.GetTag( "entry" + i + "_itemName" );
		const entryAmt = parseInt( largeBOD.GetTag( "entry" + i + "_amount" ));

		largeBODGump.AddHTMLGump( 75, y, 210, 20, false, false, "<basefont color=#ffffff>" + itemName + "</basefont>" );
		largeBODGump.AddText( 275, y, 0x480, entryAmt );
		y += 24;
	}

	if( reqExceptional || materialColor > 0 )
	{
		largeBODGump.AddHTMLGump( 75, y, 200, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17255, socket.language ) + "</basefont>" ); // Special requirements to meet:
		y += 24;
	}

	if( reqExceptional )
	{
		largeBODGump.AddHTMLGump( 75, y, 300, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17256, socket.language ) + "</basefont>" ); // All items must be exceptional.
		y += 24;
	}

	if( materialColor > 0 )
	{
		const materialName = GetMaterialNameFromHue( materialColor );
		if( materialName != "" )
		{
			largeBODGump.AddHTMLGump( 75, y, 300, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17257, socket.language ) + " " + materialName + " material</basefont>" ); // All items must be made with X material.
			y += 24;
		}
	}

	// Combine button
	largeBODGump.AddButton( 125, y, 4005, 4007, 1, 0, 2 );
	largeBODGump.AddHTMLGump( 160, y, 300, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 17276, socket.language ) + "</basefont>" ); // Combine this deed with another deed.
	y += 24;

	// Exit button
	largeBODGump.AddButton( 125, y, 4005, 4007, 1, 0, 1 );
	largeBODGump.AddHTMLGump( 160, y, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10290, socket.language ) + "</basefont>" ); // EXIT

	largeBODGump.Send( socket );
	largeBODGump.Free();
}

/** @param {number} hue */
function GetMaterialNameFromHue( hue )
{
	switch( hue )
	{
		case 0:     return "iron";
		case 2419:  return "dull copper";
		case 2406:  return "shadow iron";
		case 2414:  return "copper";
		case 1750:  return "bronze";
		case 2213:  return "gold";
		case 2425:  return "agapite";
		case 2207:  return "verite";
		case 2219:  return "valorite";
		default:    return "";
	}
}

/** @type {( socket: Socket, pButton: number, gumpData: GumpData ) => void} */
function onGumpPress( socket, pButton, gumpData )
{
	if( socket == null )
	{
		return;
	}

	const pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	const largeBOD = pUser.largeBOD;
	if( !ValidateObject( largeBOD ))
		return;

	switch ( pButton )
	{
		case 0: // closed via ESC or CANCEL ( we don't use 0 explicitly here; 1 is EXIT btn )
		case 1: // EXIT
			delete pUser.largeBOD;
			break;

		case 2: // Combine with another deed ( Small BOD )
			BeginLargeBODCombine( pUser, largeBOD );
			break;

		default:
			break;
	}
}

/** @param {Character} pUser @param {Item} largeBOD */
function BeginLargeBODCombine( pUser, largeBOD )
{
	const socket = pUser.socket;
	if( socket == null )
		return;

	// If the large BOD is already complete, don't start a target
	if( IsLargeBODComplete( largeBOD ))
	{
		pUser.TextMessage( GetDictionaryEntry( 17283, socket.language ), false, 0x3b2, 0, pUser.serial ); // The maximum amount of requested items have already been combined to this deed.
		return;
	}

	pUser.largeBOD = largeBOD;
	pUser.CustomTarget( 0 );
}

/** @param {Item} largeBOD */
function IsLargeBODComplete( largeBOD )
{
	const amountMax  = largeBOD.GetTag( "amountMax" );
	const entryCount = largeBOD.GetTag( "entryCount" );

	for( let i = 0; i < entryCount; i++ )
	{
		const entryAmt = largeBOD.GetTag( "entry" + i + "_amount" );
		if( entryAmt < amountMax )
			return false;
	}
	return true;
}

/** @type {( socket: Socket, target: BaseObject | null ) => void} */
function onCallback0( socket, myTarget )
{
	if( socket == null )
		return;

	const pUser = socket.currentChar;
	const largeBOD = /** @type {Item} */ ( pUser.largeBOD );
	const gumpID = LargeBODID + 0xffff;

	if( !ValidateObject( pUser ))
		return;

	delete pUser.largeBOD; // always clear temporary ref

	if( !ValidateObject( largeBOD ))
		return;

	// Abort if player cancels target or clicks empty space
	const cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck === 255 || !myTarget )
	{
		socket.CloseGump( gumpID, 0 );
		LargeBODGump( pUser, largeBOD );
		return;
	}

	// Target must be a Small BOD deed
	if( !myTarget.isItem || typeof myTarget.sectionID !== "string" || myTarget.sectionID.split( "_" )[0] !== "smallbod" )
	{
		socket.SysMessage( GetDictionaryEntry( 17277, socket.language )); // That is not a bulk order.
		socket.CloseGump( gumpID, 0 );
		LargeBODGump( pUser, largeBOD );
		return;
	}

	const smallBOD = myTarget;

	// Must be in player's pack
	const owner = GetPackOwner( smallBOD, 0 );
	if( owner == null || owner.serial !== pUser.serial )
	{
		socket.SysMessage( GetDictionaryEntry( 17259, socket.language )); // You must have the item in your backpack to target it.
		socket.CloseGump( gumpID, 0 );
		LargeBODGump( pUser, largeBOD );
		return;
	}

	CombineSmallIntoLarge( pUser, socket, largeBOD, smallBOD );

	socket.CloseGump( gumpID, 0 );
	LargeBODGump( pUser, largeBOD );
}

/** @param {Character} pUser @param {Socket} socket @param {Item} largeBOD @param {Item} smallBOD */
function CombineSmallIntoLarge( pUser, socket, largeBOD, smallBOD )
{
	if( socket == null )
		return;

	const amountMaxLarge      = parseInt( largeBOD.GetTag( "amountMax" ));
	const reqExceptionalLarge = !!largeBOD.GetTag( "reqExceptional" );
	const materialColorLarge  = largeBOD.GetTag( "materialColor" );
	const bodTypeLarge        = largeBOD.GetTag( "bodType" );

	const amountMaxSmall      = parseInt( smallBOD.GetTag( "amountMax" ));
	const amountCurSmall      = parseInt( smallBOD.GetTag( "amountCur" ));
	const reqExceptionalSmall = !!smallBOD.GetTag( "reqExceptional" );
	const materialSmall       = smallBOD.GetTag( "materialColor" );
	const bodTypeSmall        = smallBOD.GetTag( "bodType" );

	// Type/BOD type check
	if( bodTypeSmall !== bodTypeLarge )
	{
		socket.SysMessage( GetDictionaryEntry( 17272, socket.language )); // That order is for some other shopkeeper. (close enough)
		return;
	}

	// Exceptional check
	if( reqExceptionalLarge && !reqExceptionalSmall )
	{
		socket.SysMessage( GetDictionaryEntry( 17278, socket.language )); // Both orders must be of exceptional quality.
		return;
	}

	// Material check – allow none or exact match
	if( materialColorLarge > 0 && materialColorLarge !== materialSmall )
	{
		socket.SysMessage( GetDictionaryEntry( 17279, socket.language )); // Both orders must use the same resource type.
		return;
	}

	// AmountMax must match
	if( amountMaxLarge !== amountMaxSmall )
	{
		socket.SysMessage( GetDictionaryEntry( 17280, socket.language )); // The two orders have different requested amounts and cannot be combined.
		return;
	}

	// Small must be completed
	if( amountCurSmall < amountMaxSmall )
	{
		socket.SysMessage( GetDictionaryEntry( 17281, socket.language )); // The order to combine with is not completed.
		return;
	}

	const entryCount = parseInt( largeBOD.GetTag( "entryCount" ));
	let allFull = true;
	for( let i = 0; i < entryCount; i++ )
	{
		const cur = parseInt( largeBOD.GetTag( "entry" + i + "_amount" ));
		if( cur < amountMaxLarge )
		{
			allFull = false;
			break;
		}
	}

	if( allFull )
	{
		socket.SysMessage( GetDictionaryEntry( 17283, socket.language )); // The maximum amount of requested items have already been combined to this deed.
		return;
	}

	// Find matching entry based on itemName
	const smallName  = smallBOD.GetTag( "itemName" );
	const entryCount = parseInt( largeBOD.GetTag( "entryCount" ));
	let entryIndex   = -1;

	for( let i = 0; i < entryCount; i++ )
	{
		const entryName = largeBOD.GetTag( "entry" + i + "_itemName" );
		if( entryName === smallName )
		{
			entryIndex = i;
			break;
		}
	}

	if( entryIndex === -1 )
	{
		socket.SysMessage( GetDictionaryEntry( 17282, socket.language )); // That is not a bulk order for this large request.
		return;
	}

	let entryAmt = parseInt( largeBOD.GetTag( "entry" + entryIndex + "_amount" ));
	if( entryAmt >= amountMaxLarge )
	{
		socket.SysMessage( GetDictionaryEntry( 17283, socket.language )); // The maximum amount of requested items has already been reached for this entry.
		return;
	}

	// Combine – cap at amountMaxLarge just in case
	entryAmt += amountCurSmall;
	if( entryAmt > amountMaxLarge )
		entryAmt = amountMaxLarge;

	largeBOD.SetTag( "entry" + entryIndex + "_amount", entryAmt );

	// Aggregate reward tags from small into large so DispenseBODRewards can be reused
	const goldLarge    = largeBOD.GetTag( "goldValue" );
	const fameLarge    = largeBOD.GetTag( "fameValue" );
	const qualityLarge = largeBOD.GetTag( "qualityValue" );
	const amtCurLarge  = largeBOD.GetTag( "amountCur" );

	const goldSmall    = smallBOD.GetTag( "goldValue" );
	const fameSmall    = smallBOD.GetTag( "fameValue" );
	const qualitySmall = smallBOD.GetTag( "qualityValue" );

	largeBOD.SetTag( "goldValue",    goldLarge    + goldSmall );
	largeBOD.SetTag( "fameValue",    fameLarge    + fameSmall );
	largeBOD.SetTag( "qualityValue", qualityLarge + qualitySmall );
	largeBOD.SetTag( "amountCur",    amtCurLarge  + amountCurSmall );

	let nowAllFull = true;
	for( let i = 0; i < entryCount; i++ )
	{
		const cur = parseInt( largeBOD.GetTag( "entry" + i + "_amount" ));
		if( cur < amountMaxLarge )
		{
			nowAllFull = false;
			break;
		}
	}

	if( nowAllFull )
	{
		largeBOD.SetTag( "complete", 1 ); // Handy for vendor turn-in checks
	}

	largeBOD.Refresh();
	smallBOD.Delete();
	socket.SysMessage( GetDictionaryEntry( 17284, socket.language )); // The orders have been combined.
}

/** @type {( largeBOD: BaseObject, pSocket: Socket ) => string} */
function onTooltip( largeBOD, pSocket )
{
	const amountMax  = largeBOD.GetTag( "amountMax" );
	const entryCount = largeBOD.GetTag( "entryCount" );

	if( amountMax === 0 || entryCount === 0 )
		return "";

	let tt = "Large bulk order\n";
	tt += "Amount to make per item: " + amountMax;

	for( let i = 0; i < entryCount; i++ )
	{
		const name = largeBOD.GetTag( "entry" + i + "_itemName" );
		const amt  = parseInt( largeBOD.GetTag( "entry" + i + "_amount" ));
		tt += "\n" + name + " : " + amt + "/" + amountMax;
	}

	return tt;
}