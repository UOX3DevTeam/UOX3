/// <reference path="../../definitions.d.ts" />
// @ts-check
// This command is a shortcut to add house signs - brings up a menu to select from
// =============================================================
// Usage:
//   'bridge  -> Opens Bridge Builder gump (pick material, dir, coords, flags, hue, undo last)
//   All placement is done from the GUMP + two map clicks (Corner A / Corner B)
//
// Notes:
// - Direction: North/South (rails at Y1 & Y2) or East/West (rails at X1 & X2)
// - Z snaps to nearest 5 unless "No snap" is ON
// - Validate checks map/static blocking unless "No validate" is ON
// - Corner A sets X1/Y1/(Z if you did not type one). Corner B sets X2/Y2.
// - State persists in TempTags so reopening the gump keeps your last settings
// =============================================================

function CommandRegistration()
{ 
	RegisterCommand( 'addbridge', 2, true );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_ADDBRIDGE( socket, cmdString )
{ 
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	BridgeGump( pUser );
}

var MATERIALS = [
	{ key: 'wood', idx: 0, ns: 0x0724, ew: 0x0725, block: 0x0721, endNS: 0x0722, endEW: 0x0723 },
	{ key: 'darkwood', idx: 1, ns: 0x0739, ew: 0x073A, block: 0x0738, endNS: 0x0739, endEW: 0x073A },
	{ key: 'woodramp', idx: 2, ns: 0x087B, ew: 0x087A, block: 0x0721, endNS: 0x087B, endEW: 0x087A },
	{ key: 'bluestone', idx: 3, ns: 0x071F, ew: 0x0736, block: 0x071E, endNS: 0x071F, endEW: 0x0736 },
	{ key: 'whitestone', idx: 4, ns: 0x0751, ew: 0x0752, block: 0x0750, endNS: 0x0751, endEW: 0x0752 },
	{ key: 'graystone', idx: 5, ns: 0x0789, ew: 0x078A, block: 0x0788, endNS: 0x0789, endEW: 0x078A },
	{ key: 'sandstone', idx: 6, ns: 0x076D, ew: 0x076E, block: 0x076C, endNS: 0x076D, endEW: 0x076E },
	{ key: 'greenstone', idx: 7, ns: 0x07A4, ew: 0x07A5, block: 0x07A3, endNS: 0x07A4, endEW: 0x07A5 },
	{ key: 'yellowstone', idx: 8, ns: 0x03EF, ew: 0x03F0, block: 0x03EE, endNS: 0x03EF, endEW: 0x03F0 },
	{ key: 'whitemarble', idx: 9, ns: 0x070A, ew: 0x070B, block: 0x0709, endNS: 0x070A, endEW: 0x0709 }
];

function clamp( num, min, max )
{
	return Math.min( Math.max( num, min ), max );
}

function snapNearest5( height )
{
  var roundz = Math.round( height / 5 ) * 5;
  return clamp( roundz, -128, 127 );
}

function placeItemAt( rawId, x, y, z, world, instance, collector, hue )
{
	var pUser = ( this && this.currentChar ) || null;
	var socket = ( pUser && pUser.socket ) ? pUser.socket : null;
	var name = "Bridge";

	if( typeof hue === 'undefined' || hue === null )
		hue = getDefaultHue(pUser);

	var item = CreateBlankItem( socket, pUser, 1, name, rawId, hue | 0, "ITEM", false );
	if( !ValidateObject( item ))
		return null;

	item.Teleport( x, y, z, world );
	item.instanceID = instance;

	if( typeof item.movable !== "undefined" )
		item.movable = 2;

	if( typeof item.decayable !== "undefined" )
		item.decayable = false;

	if( typeof item.decay !== "undefined" )
		item.decay = false;

	if( typeof item.Refresh === "function" )
		item.Refresh();

	if( collector && typeof item.serial !== "undefined" )
		collector.push(item.serial | 0);

	return item;
}
function lineNS( edgeNear, edgeFar, blockId, x1, x2, yTop, yBottom, z, world, instance, collector, validate, hue )
{
	var x, y;
	for( x = x1; x <= x2; x++ )
	{
		if( !validate || ( !DoesMapBlock( x, yTop, z, world, false, false, false, false ) && !DoesStaticBlock( x, yTop, z, world, false )))
			placeItemAt(edgeNear, x, yTop, z, world, instance, collector, hue);
	}

	for( x = x1; x <= x2; x++ )
	{
		if( !validate || ( !DoesMapBlock( x, yBottom, z, world, false, false, false, false ) && !DoesStaticBlock( x, yBottom, z, world, false )))
			placeItemAt( edgeFar, x, yBottom, z, world, instance, collector, hue );
	}

	for( y = yTop + 1; y <= yBottom - 1; y++ )
	{
		for( x = x1; x <= x2; x++ )
		{
			if( !validate || ( !DoesMapBlock( x, y, z, world, false, false, false, false ) && !DoesStaticBlock( x, y, z, world, false )))
				placeItemAt( blockId, x, y, z, world, instance, collector, hue );
		}
	}
}

function lineEW( edgeNear, edgeFar, blockId, y1, y2, xLeft, xRight, z, world, instance, collector, validate, hue )
{
	var x, y;
	for( y = y1; y <= y2; y++ )
	{
		if( !validate || ( !DoesMapBlock(xLeft, y, z, world, false, false, false, false ) && !DoesStaticBlock( xLeft, y, z, world, false )))
			placeItemAt( edgeNear, xLeft, y, z, world, instance, collector, hue );
	}

	for( y = y1; y <= y2; y++ )
	{
		if( !validate || ( !DoesMapBlock( xRight, y, z, world, false, false, false, false ) && !DoesStaticBlock( xRight, y, z, world, false )))
			placeItemAt( edgeFar, xRight, y, z, world, instance, collector, hue );
	}

	for( x = xLeft + 1; x <= xRight - 1; x++ )
	{
		for( y = y1; y <= y2; y++ )
		{
			if( !validate || ( !DoesMapBlock( x, y, z, world, false, false, false, false ) && !DoesStaticBlock(x, y, z, world, false )))
				placeItemAt(blockId, x, y, z, world, instance, collector, hue);
		}
	}
}

function BuildBridge( socket, dirTok, mat, z, x1, y1, x2, y2, flags, hue )
{
	var pUser = socket.currentChar;

	if( !ValidateObject( pUser ))
		return;

	var world = pUser.worldnumber | 0, inst = pUser.instanceID | 0;

	if( x2 < x1 )
	{ 
		var tx = x1; x1 = x2; x2 = tx;
	}

	if( y2 < y1 ) 
	{ 
		var ty = y1; y1 = y2; y2 = ty;
	}

	if(!( flags && flags.nosnap ))
		z = snapNearest5( z );

	var placedSerial = []; // collect serials for undo

	if( dirTok === 'ns' )
	{
		lineNS( mat.ns, mat.endNS, mat.block, x1, x2, y1, y2, z, world, inst, placedSerial, !( flags && flags.novalidate ), hue );
	}
	else
	{
		lineEW( mat.ew, mat.endEW, mat.block, y1, y2, x1, x2, z, world, inst, placedSerial, !( flags && flags.novalidate ), hue );
	}

	// persist the last build serial list for Undo
	pUser.SetTempTag( 'bridge_lastSer', placedSerial.join( ',' ));
	socket.SysMessage( 'Bridge built: ' + dirTok.toUpperCase() + ', material=' + mat.key + ', Z=' + z + ' from (' + x1 + ',' + y1 + ') to (' + x2 + ',' + y2 + ')  [' + placedSerial.length + ' tiles]' );
}

// ---------------------------
// Target helper (ground/object/feet)
// ---------------------------
function TargetLocation( socket, pUser, fallbackObj )
{
	if( socket.GetWord( 1 ))
	{
		var x = socket.GetWord( 11 ), y = socket.GetWord( 13 ), z = socket.GetSByte( 16 );

		return { x: x, y: y, z: z, world: pUser.worldnumber, inst: pUser.instanceID, fromGround: true };
	}

	if( ValidateObject( fallbackObj ))
	{
		return { x: fallbackObj.x, y: fallbackObj.y, z: fallbackObj.z, world: fallbackObj.worldnumber, inst: fallbackObj.instanceID, fromGround: false };
	}

	return { x: pUser.x, y: pUser.y, z: pUser.z, world: pUser.worldnumber, inst: pUser.instanceID, fromGround: false };
}

var L = 300 + 8, B = L + 120;
var BTN_DIR = 9101, BTN_MPREV = 9102, BTN_MNEXT = 9103, BTN_NOSNAP = 9104, BTN_NOVAL = 9105, BTN_USEPOS = 9106, BTN_PICKA = 9107, BTN_PICKB = 9108, BTN_BUILD = 9109, BTN_CLOSE = 9110, BTN_UNDO = 9111, BTN_USEHUE = 9112;
var INPUT_HUE = 0;

function getDefaultHue( pUser )
{
	if( typeof getHue === 'function' )
		return getHue( pUser );
	return 0;
}

// Returns a reasonable default hue for the user (or 0 if unknown)
function getHue( pUser )
{
	if( !ValidateObject( pUser ))
		return 0;

	// Common character hue props (varies by shard build)
	if( typeof pUser.colour !== "undefined" )
		return pUser.colour | 0;

	return 0;
}

function Background( gump )
{
	gump.AddBackground( 0, 0, 520, 330, 0x0E10 );
	gump.AddBackground( 8, 5, 520 - 16, 330 - 11, 0x053 );
	gump.AddTiledGump( 15, 14, 520 - 29, 330 - 29, 0x0E14 );
	gump.AddCheckerTrans( 15, 14, 520 - 29, 330 - 29 );
	gump.AddTiledGump( 300 - 8, 14, 5, 330 - 29, 0x0E14 );
}

function BridgeGump( pUser )
{
	var dirTok = ( pUser.GetTempTag( 'bridge_dir' ) || 'ns' );
	var matIdx = ( parseInt( pUser.GetTempTag( 'bridge_mat' ) || '0', 10 ) | 0 );

	if ( matIdx < 0 || matIdx >= MATERIALS.length )
		matIdx = 0;

	var z = parseInt( pUser.GetTempTag( 'bridge_z' ) || String( pUser.z | 0 ), 10 ) | 0;
	var x1 = parseInt( pUser.GetTempTag( 'bridge_x1' ) || String( pUser.x | 0 ), 10 ) | 0;
	var y1 = parseInt( pUser.GetTempTag( 'bridge_y1' ) || String( pUser.y | 0 ), 10 ) | 0;
	var x2 = parseInt( pUser.GetTempTag( 'bridge_x2' ) || String(( pUser.x | 0 ) + 5 ), 10 ) | 0;
	var y2 = parseInt( pUser.GetTempTag( 'bridge_y2' ) || String(( pUser.y | 0 ) + 3 ), 10 ) | 0;
	var nosnap = (( pUser.GetTempTag( 'bridge_nosnap' ) | 0 ) === 1 );
	var noval = (( pUser.GetTempTag( 'bridge_noval' ) | 0 ) === 1 );

	var gump = new Gump;

	gump.AddPage( 0 );
	Background( gump );

	gump.AddHTMLGump( 22, 18, 220, 20, false, false, '<basefont color=#ffffff>Bridge Builder</basefont>' );

	gump.AddHTMLGump( 22, 50, 260, 20, false, false, '<basefont color=#ffffff>Direction: ' + ( dirTok === 'ns' ? 'North/South' : 'East/West' ) + '</basefont>' );
	gump.AddHTMLGump( 22, 72, 260, 20, false, false, '<basefont color=#ffffff>Material: ' + MATERIALS[matIdx].key + ' (  #' + matIdx + '  )</basefont>' );

	// Right-side controls
	var y = 20;
	gump.AddHTMLGump( L, y, 120, 20, false, false, '<basefont color=#ffffff>Toggle Dir</basefont>' );
	gump.AddButton( B, y - 2, 0x0FA5, 0x0FA7, 1, 0, BTN_DIR );
	y += 22;

	gump.AddHTMLGump( L, y, 120, 20, false, false, '<basefont color=#ffffff>Material -</basefont>' );
	gump.AddButton( B, y - 2, 0x0FAE, 0x0FB0, 1, 0, BTN_MPREV );
	y += 22;

	gump.AddHTMLGump( L, y, 120, 20, false, false, '<basefont color=#ffffff>Material +</basefont>' );
	gump.AddButton( B, y - 2, 0x0FA5, 0x0FA7, 1, 0, BTN_MNEXT );
	y += 22;

	gump.AddHTMLGump( L, y, 160, 20, false, false, '<basefont color=#ffffff>No snap: ' + ( nosnap ? 'ON' : 'OFF' ) + '</basefont>' );
	gump.AddButton( B, y - 2, nosnap ? 0x0FB0 : 0x0FAE, nosnap ? 0x0FB0 : 0x0FAE, 1, 0, BTN_NOSNAP );
	y += 22;

	gump.AddHTMLGump( L, y, 160, 20, false, false, '<basefont color=#ffffff>No validate: ' + ( noval ? 'ON' : 'OFF' ) + '</basefont>' );
	gump.AddButton( B, y - 2, noval ? 0x0FB0 : 0x0FAE, noval ? 0x0FB0 : 0x0FAE, 1, 0, BTN_NOVAL );
	y += 22;

	gump.AddHTMLGump( L, y, 160, 20, false, false, '<basefont color=#ffffff>Use my Z/X1/Y1</basefont>' );
	gump.AddButton( B, y - 2, 0x0FA5, 0x0FA7, 1, 0, BTN_USEPOS );
	y += 22;

	gump.AddHTMLGump( L, y, 160, 20, false, false, '<basefont color=#ffffff>Use my Hue</basefont>' );
	gump.AddButton( B, y - 2, 0x0FA5, 0x0FA7, 1, 0, BTN_USEHUE );
	y += 22;

	gump.AddHTMLGump( L, y, 160, 20, false, false, '<basefont color=#ffffff>Pick Corner A</basefont>' );
	gump.AddButton( B, y - 2, 0x0FA5, 0x0FA7, 1, 0, BTN_PICKA );
	y += 22;

	gump.AddHTMLGump( L, y, 160, 20, false, false, '<basefont color=#ffffff>Pick Corner B</basefont>' );
	gump.AddButton( B, y - 2, 0x0FA5, 0x0FA7, 1, 0, BTN_PICKB );
	y += 22;

	gump.AddHTMLGump( L, y, 120, 20, false, false, '<basefont color=#ffffff>Build</basefont>' );
	gump.AddButton( B, y - 2, 0x0FA5, 0x0FA7, 1, 0, BTN_BUILD );
	y += 22;

	gump.AddHTMLGump( L, y, 120, 20, false, false, '<basefont color=#ffffff>Undo last</basefont>' );
	gump.AddButton( B, y - 2, 0x0FAB, 0x0FAD, 1, 0, BTN_UNDO );
	y += 22;

	gump.AddHTMLGump( L, y, 120, 20, false, false, '<basefont color=#ffffff>Close</basefont>' );
	gump.AddButton( B, y - 2, 0x0FAB, 0x0FAD, 1, 0, BTN_CLOSE );

	// IMPORTANT: AddTextEntry right before Send/Free
	gump.AddHTMLGump( 22, 104, 40, 20, false, false, '<basefont color=#ffffff>Z:</basefont>' );
	gump.AddGump( 33, 100, 1593 );

	gump.AddHTMLGump( 122, 104, 40, 20, false, false, '<basefont color=#ffffff>X1:</basefont>' );
	gump.AddGump( 140, 100, 1593 );

	gump.AddHTMLGump( 208, 104, 40, 20, false, false, '<basefont color=#ffffff>Y1:</basefont>' );
	gump.AddGump( 225, 100, 1593 );

	gump.AddHTMLGump( 120, 135, 40, 20, false, false, '<basefont color=#ffffff>X2:</basefont>' );
	gump.AddGump( 140, 132, 1593 );

	gump.AddHTMLGump( 205, 135, 40, 20, false, false, '<basefont color=#ffffff>Y2:</basefont>' );
	gump.AddGump( 225, 132, 1593 );

	// Label + box
	gump.AddHTMLGump( 22, 165, 40, 20, false, false, '<basefont color=#ffffff>Hue:</basefont>' );
	gump.AddGump( 55, 162, 1593 );

	var mat = MATERIALS[matIdx];

	// compute current hue to display ( tag or default)
	var dispHue = parseInt( pUser.GetTempTag( 'bridge_hue' ), 10 );
	if( isNaN( dispHue ))
	{
		dispHue = getDefaultHue( pUser )
	}

	// --- NEW: material block preview ( with hue ) ---
	// We preview the filler block ( mat.block ) so it matches the internal fill art.
	// If you prefer to preview the near edge instead, swap to ( dirTok==='ns'? mat.ns : mat.ew ).
	var prevX = 160, prevY = 180;
	gump.AddHTMLGump( 198, 188, 120, 20, false, false, '<basefont color=#ffffff>Preview</basefont>' );
	gump.AddGump( prevX - 3, prevY - 3, 2719 );
	var matId = mat.block;

	// Use shard’s colored picture call; fallback to normal picture if color call not present
	if( typeof gump.AddPictureColor === 'function')
	{
		gump.AddPictureColor( prevX + 40, prevY + 30, matId, dispHue );
	}
	else
	{
		gump.AddPicture( prevX + 40, prevY + 30, matId );
	}

	gump.AddTextEntryLimited( 40, 102, 60, 20, INPUT_HUE, 22, 22, String( z ), 3 );

	gump.AddTextEntryLimited( 145, 102, 60, 20, INPUT_HUE, 23, 23, String( x1 ), 4 );

	gump.AddTextEntryLimited( 230, 102, 60, 20, INPUT_HUE, 24, 24, String( y1 ), 4 );

	gump.AddTextEntryLimited( 145, 135, 60, 20, INPUT_HUE, 25, 25, String( x2 ), 4 );

	gump.AddTextEntryLimited( 230, 135, 60, 20, INPUT_HUE, 26, 26, String( y2 ), 4 );

	gump.AddTextEntryLimited( 70, 163, 60, 20, INPUT_HUE, 27, 27, String( dispHue ), 5 );

	gump.Send( pUser );
	gump.Free();
}

function onGumpPress( socket, buttonID, gumpData )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	function toInt( s )
	{ 
		var n = parseInt( s,10 );
		return isNaN( n ) ? null : n;
	}

	function resolveFromEditOrTag( tagName, editVal, fallback )
	{
		var n = toInt( editVal );
		if( n === 0 || n === null )
		{
			var t = toInt( pUser.GetTempTag( tagName ));
			return ( t !== null ) ? t : fallback;
		}
		return n;
	}

	function parseHue( str )
	{
		if( str && /^0x/i.test( str ))
			return parseInt( str,16 );

		var n = parseInt( str,10);

		if( !isNaN( n ))
			return n;

		var tag = parseInt( pUser.GetTempTag( 'bridge_hue' ),10 );
		return isNaN( tag ) ? getDefaultHue( pUser ) : tag;
	}

	function reopen()
	{
		BridgeGump( pUser );
	}

	// ---- state ----
	var dirTok = ( pUser.GetTempTag( 'bridge_dir' ) || 'ns' );
	var matIdx = ( parseInt(pUser.GetTempTag('bridge_mat') || '0',10) | 0);

	if( matIdx < 0 || matIdx >= MATERIALS.length )
		matIdx = 0;

	var nosnap = (( pUser.GetTempTag( 'bridge_nosnap' )|0) === 1 );
	var noval  = (( pUser.GetTempTag( 'bridge_noval' ) |0) === 1 );

	var zEdit   = gumpData.getEdit(0);
	var x1Edit  = gumpData.getEdit(1);
	var y1Edit  = gumpData.getEdit(2);
	var x2Edit  = gumpData.getEdit(3);
	var y2Edit  = gumpData.getEdit(4);
	var hueEdit = gumpData.getEdit(5);

	switch( buttonID )
	{
		case BTN_CLOSE:
			return;

		case BTN_DIR:
			dirTok = ( dirTok === 'ns' ) ? 'ew' : 'ns';
			pUser.SetTempTag( 'bridge_dir', dirTok );
			return reopen();

		case BTN_MPREV:
			matIdx = ( matIdx + MATERIALS.length - 1 ) % MATERIALS.length;
			pUser.SetTempTag( 'bridge_mat', matIdx );
			return reopen();

		case BTN_MNEXT:
			matIdx = ( matIdx + 1 ) % MATERIALS.length;
			pUser.SetTempTag( 'bridge_mat', matIdx );
			return reopen();

		case BTN_NOSNAP:
			nosnap = !nosnap;
			pUser.SetTempTag( 'bridge_nosnap', nosnap ? 1 : 0 );
			return reopen();

		case BTN_NOVAL:
			noval = !noval;
			pUser.SetTempTag( 'bridge_noval', noval ? 1 : 0 );
			return reopen();

		case BTN_USEPOS:
			pUser.SetTempTag( 'bridge_x1', String( pUser.x|0 ));
			pUser.SetTempTag( 'bridge_y1', String( pUser.y|0 ));
			pUser.SetTempTag( 'bridge_z',  String( pUser.z|0 ));
			return reopen();

		case BTN_USEHUE:
		{
			var hue = getHue( pUser );
			pUser.SetTempTag( 'bridge_hue', String( hue ));
			return reopen();
		}

		case BTN_PICKA:
			socket.CustomTarget( 0, 'Select Corner A (X1/Y1/Z)' );
			return reopen();

		case BTN_PICKB:
			socket.CustomTarget( 1, 'Select Corner B (X2/Y2)' );
			return reopen();

		case BTN_UNDO:
		{
			var list = pUser.GetTempTag( 'bridge_lastSer' );
			if( !list || list.length === 0 )
			{
				socket.SysMessage( 'Nothing to undo.' );
				return reopen();
			}

			var parts = String(list).split(','), removed = 0;
			for( var i=0; i<parts.length; i++ )
			{
				var s = parseInt( parts[i],10 );
				if( isNaN( s ))
					continue;

				var item = CalcItemFromSer( s );
				if( ValidateObject( item ))
				{
					item.Delete();
					removed++;
				}
			}
			pUser.SetTempTag( 'bridge_lastSer','' );
			socket.SysMessage( 'Undo: removed ' + removed + ' items from last bridge.' );
			return reopen();
		}

		case BTN_BUILD:
		{
			var mat   = MATERIALS[matIdx];
			var flags = { nosnap: nosnap, novalidate: noval };
			var hue   = parseHue(hueEdit);

			var z  = resolveFromEditOrTag( 'bridge_z',  zEdit,  pUser.z|0 );
			var x1 = resolveFromEditOrTag( 'bridge_x1', x1Edit, pUser.x|0 );
			var y1 = resolveFromEditOrTag( 'bridge_y1', y1Edit, pUser.y|0 );
			var x2 = resolveFromEditOrTag( 'bridge_x2', x2Edit, ( pUser.x|0 )+5 );
			var y2 = resolveFromEditOrTag( 'bridge_y2', y2Edit, ( pUser.y|0 )+3 );

			// persist effective values
			pUser.SetTempTag( 'bridge_z',  String( z ));
			pUser.SetTempTag( 'bridge_x1', String( x1 ));
			pUser.SetTempTag( 'bridge_y1', String( y1 ));
			pUser.SetTempTag( 'bridge_x2', String( x2 ));
			pUser.SetTempTag( 'bridge_y2', String( y2 ));
			pUser.SetTempTag( 'bridge_hue', String( hue ));

			BuildBridge( socket, dirTok, mat, z, x1, y1, x2, y2, flags, hue );
			return reopen();
		}
	}

	return reopen();
}


// ---------------------------
// Target callbacks (A/B)
// ---------------------------
/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback0( socket, targObj )
{ // Corner A
	var pUser = socket.currentChar;

	if (!ValidateObject( pUser )) 
		return;

	var loc = TargetLocation( socket, pUser, targObj );

	pUser.SetTempTag( 'bridge_x1', String( loc.x | 0 ));
	pUser.SetTempTag( 'bridge_y1', String (loc.y | 0 ));
	pUser.SetTempTag( 'bridge_z', String( loc.z | 0 ));

	BridgeGump( pUser );
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback1( socket, targObj )
{ // Corner B
	var pUser = socket.currentChar;

	if( !ValidateObject( pUser ))
		return;

	var loc = TargetLocation( socket, pUser, targObj );

	pUser.SetTempTag( 'bridge_x2', String( loc.x | 0 ));
	pUser.SetTempTag( 'bridge_y2', String( loc.y | 0 ));

	BridgeGump( pUser );
}