var ATLAS_MAX_RUNES = 48;       // 3 pages * 16 rows
var ATLAS_MAX_CHARGES = 100;    // default cap; override via .maxhp on the item
var ATLAS_COOLDOWN_MS = 7000;   // 7s, like Runebook
var SCRIPT_ID = 5063;			// jse_fileassociations.scp
var SPELL_RECALL = 32;
var SPELL_GATE = 52;
var SPELL_SACRED_JOUR = 209;    // optional – button shown only if user has spell

function mapHue( worldNum )
{
	// Tram( 0 )=0xA, Fel( 1 )=0x51, Malas( 4 )=0x44E, Tokuno( 3 )=0x482, TerMur( 5 )=0x66D
	switch( worldNum | 0 )
	{
		case 0: return 0x000A;
		case 1: return 0x0051;
		case 4: return 0x044E;
		case 3: return 0x0482;
		case 5: return 0x066D;
		default: return 0x0000;
	}
}

function GetMapCoordinates( xCoord, yCoord, worldNum )
{
	var resultArray = [], xCenter = 0, yCenter = 0, xWidth = 5120, yHeight = 4096;

	switch ( worldNum )
	{
		case 0:
		case 1:
			if( xCoord >= 0 && yCoord >= 0 && xCoord < 5120 && yCoord < 4096 )
			{
				xCenter = 1323; yCenter = 1624;
			}
			else if( xCoord >= 5120 && yCoord >= 0 && xCoord < 7168 && yCoord < 4096 )
			{
				xCenter = 5936; yCenter = 3112;
			}
			else
			{ 
				resultArray[0] = "-1"; resultArray[1] = "-1"; resultArray[2] = true; resultArray[3] = "-1"; resultArray[4] = "-1"; resultArray[5] = true; return resultArray;
			}
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			if( xCoord >= 0 && yCoord >= 0 && xCoord < 7168 && yCoord < 4096 )
			{
				xCenter = 1323; yCenter = 1624;
			}
			else
			{
				resultArray[0] = "-1"; resultArray[1] = "-1"; resultArray[2] = true; resultArray[3] = "-1"; resultArray[4] = "-1"; resultArray[5] = true; return resultArray;
			}
			break;
		default: break;
	}

	var absLong = (( xCoord - xCenter ) * 360 ) / xWidth;
	if( absLong > 180 )
		absLong = -180 + ( absLong % 180 );

	var xEast = ( absLong >= 0 );
	if( absLong < 0 )
		absLong = -absLong;

	var xLongDeg = Math.round( absLong ), xLongMin = Math.round(( absLong % 1 ) * 60 );
	var absLat = (( yCoord - yCenter ) * 360 ) / yHeight;
	if( absLat > 180 )
		absLat = -180 + ( absLat % 180 );

	var ySouth = ( absLat >= 0 );
	if( absLat < 0 )
		absLat = -absLat;

	var yLatDeg = Math.round( absLat ), yLatMin = Math.round(( absLat % 1 ) * 60 );

	resultArray[0] = xLongDeg; resultArray[1] = xLongMin; resultArray[2] = xEast;
	resultArray[3] = yLatDeg; resultArray[4] = yLatMin; resultArray[5] = ySouth;
	return resultArray;
}

function CheckAccessRights( socket, pUser, atlas )
{
	var root = FindRootContainer( atlas, 0 );
	if( ValidateObject( root ))
	{
		if( ValidateObject( root.owner ) && root.owner != pUser )
		{ 
			socket.SysMessage( GetDictionaryEntry( 9268, socket.language ));
			return false;
		}
	}
	if( atlas.movable == 3 )
	{
		socket.SysMessage( GetDictionaryEntry( 9269, socket.language ));
		return false;
	}
	return true;
}

function onUseChecked( pUser, atlas )
{
	var socket = pUser.socket;

	var now = GetCurrentClock(  );
	var nextUse = parseInt( atlas.GetTag( "useDelayed" ));
	if(( now - nextUse ) < ATLAS_COOLDOWN_MS )
	{
		socket.SysMessage( GetDictionaryEntry( 9250, socket.language ));
		return false;
	}

	var root = FindRootContainer( atlas, 0 );
	if( !ValidateObject( root ))
	{
		if( !atlas.InRange( pUser, 3 ))
		{
			socket.SysMessage( GetDictionaryEntry( 393, socket.language ));
			return false;
		}
	}
	else
	{
		if( root != pUser.pack )
		{
			var owner = GetPackOwner( root, 0 );
			if( ValidateObject( owner ))
			{ 
				socket.SysMessage( GetDictionaryEntry( 9251, socket.language )); 
				return false;
			}
			else if( !root.InRange( pUser, 3 ))
			{ 
				socket.SysMessage( GetDictionaryEntry( 393, socket.language ));
				return false;
			}
		}
	}

	// lock usage
	var inUse = atlas.GetTag( "inUse" );
	if( inUse )
	{
		var ser = atlas.GetTag( "userSerial" );
		if( ser != 0 )
		{
			var other = CalcCharFromSer( ser & 0x00FFFFFF );
			if( ValidateObject( other ) && other != pUser )
			{
				if( other.online && other.InRange( atlas, 3 ))
				{ 
					socket.SysMessage( GetDictionaryEntry( 2450, socket.language ));
					return false;
				}
			}
		}
	}

	var cap = atlas.maxhp > 0 ? atlas.maxhp : ATLAS_MAX_CHARGES;
	atlas.SetTempTag( "atlasMaxCharges", cap.toString(  ));
	atlas.SetTag( "inUse", true );
	atlas.SetTag( "userSerial", ( pUser.serial ).toString(  ));
	pUser.SoundEffect( 0x58, false );

	var page = parseInt( atlas.GetTag( "atlasPage" )) || 0;
	DisplayAtlasGump( socket, pUser, atlas, page );
	return false;
}

function DisplayAtlasGump( socket, pUser, atlas, pageIndex )
{
	// persist page
	atlas.SetTag( "atlasPage", ( pageIndex | 0 ).toString(  ));

	var cap = parseInt( atlas.GetTempTag( "atlasMaxCharges" )) || ATLAS_MAX_CHARGES;
	var charges = atlas.health | 0;
	var selected = parseInt( atlas.GetTag( "selectedSlot" )); // 1..48, or NaN
	var defaultSlot = parseInt( atlas.GetTag( "defaultRuneLoc" )) || 0;

	var g = new Gump;
	g.AddPage( 0 );
	g.AddGump( 0, 0, 39923 );

	// Top header text ( Charges, Rename )
	g.AddHTMLGump( 60, 9, 147, 22, false, false, "<BASEFONT size=4>Charges:</BASEFONT>" );
	g.AddHTMLGump( 110, 9, 97, 22, false, false, "<BASEFONT size=4>" + charges + " / " + cap + "</BASEFONT>" );

	g.AddHTMLGump( 264, 9, 144, 18, false, false, "<BASEFONT size=3>rename book</BASEFONT>" );
	g.AddButton( 248, 14, 2103, 2103, 1, 0, 1 ); // Rename

	// Grid of 16 entries, two columns of 8, starting at slot S = pageIndex*16 + 1
	var startSlot = pageIndex * 16 + 1;

	for( var idx = 0; idx < 16; idx++ )
	{
		var slot = startSlot + idx;
		if( slot > ATLAS_MAX_RUNES )
			break;

		var tag = atlas.GetTag( "rune" + slot + "Data" );
		var desc = "Empty", hue = 0x0000;

		if( tag != 0 )
		{
			var s = tag.split( "," );
			desc = s[0] || "Empty";
			hue = mapHue( parseInt( s[5] ));

			if( selected === slot )
				hue = 0x014B;
		}

		var col = ( idx / 8 ) | 0; var row = idx % 8;
		var bx = 46 + ( col * 205 ); var by = 55 + ( row * 20 ); // select button
		var tx = 62 + ( col * 205 ); var ty = 50 + ( row * 20 ); // description

		g.AddButton( bx, by, 2103, 2104, 1, 0, 100 + ( slot - 1 ));
		g.AddCroppedText( tx, ty, hue, 144, 18, desc );
	}

	// Selected entry details ( coords + action area )
	var entryTag = ( selected && selected >= 1 && selected <= ATLAS_MAX_RUNES ) ? atlas.GetTag( "rune" + selected + "Data" ) : 0;
	var coordsText = "Nowhere";
	var entryName = "Empty";

	if( entryTag != 0 )
	{
		var e = entryTag.split( "," );
		var ex = parseInt( e[2] ) | 0, ey = parseInt( e[3] ) | 0, ez = parseInt( e[4] ) | 0, ew = parseInt( e[5] ) | 0;
		var mc = GetMapCoordinates( ex, ey, ew );
		if( mc[0] != "-1" )
		{
			// "yLatDego yLatMin'N/S, xLongDego xLongMin'E/W"
			coordsText = mc[3] + "o " + mc[4] + "'" + ( mc[5] ? "S" : "N" ) + ", " + mc[0] + "o " + mc[1] + "'" + ( mc[2] ? "E" : "W" );
		}
		entryName = e[0] || "Empty";
	}

	g.AddHTMLGump( 25, 254, 182, 18, false, false, "<CENTER><BASEFONT size=3>" + coordsText + "</BASEFONT></CENTER>" );

	// Set Default
	g.AddHTMLGump( 62, 290, 144, 18, false, false, "<BASEFONT size=3>Set default</BASEFONT>" );
	g.AddButton( 46, 295, 2103, 2103, 1, 0, 2 );

	// Drop Rune
	g.AddHTMLGump( 62, 310, 144, 18, false, false, "<BASEFONT size=3>Drop rune</BASEFONT>" );
	g.AddButton( 46, 315, 2103, 2103, 1, 0, 3 );

	// Entry name centered near bottom
	g.AddHTMLGump( 25, 348, 182, 18, false, false, "<CENTER><BASEFONT size=3>" + entryName + "</BASEFONT></CENTER>" );

	// Right-side action list ( Recall Spell / Recall Charge / Gate / Sacred )
	var hy = 284, by = 289;

	g.AddHTMLGump( 280, hy, 128, 18, false, false, "<BASEFONT size=3>Recall ( Spell )</BASEFONT>" );
	g.AddButton( 264, by, 2103, 2103, 1, 0, 4 );
	hy += 18; by += 18;

	if( charges > 0 )
	{
		g.AddHTMLGump( 280, hy, 128, 18, false, false, "<BASEFONT size=3>Recall ( Charge )</BASEFONT>" );
		g.AddButton( 264, by, 2103, 2103, 1, 0, 5 );
		hy += 18; by += 18;
	}

	// Gate Travel ( Magery >= ~66 in RunUO; here we always show; validation happens in cast )
	g.AddHTMLGump( 280, hy, 128, 18, false, false, "<BASEFONT size=3>Gate Travel</BASEFONT>" );
	g.AddButton( 264, by, 2103, 2103, 1, 0, 6 );
	hy += 18; by += 18;

	// Sacred Journey ( only if player has it; we show conditionally )
	if( pUser.HasSpell && pUser.HasSpell( SPELL_SACRED_JOUR ))
	{
		g.AddHTMLGump( 280, hy, 128, 18, false, false, "<BASEFONT size=3>Sacred Journey</BASEFONT>" );
		g.AddButton( 264, by, 2103, 2103, 1, 0, 7 );
	}

	// Page nav
	if( pageIndex < 2 )
		g.AddButton( 374, 3, 2206, 2206, 1, 0, 1150 );
	if( pageIndex > 0 )
		g.AddButton( 23, 5, 2205, 2205, 1, 0, 1151 );

	// carry state
	socket.tempObj2 = atlas;

	g.Send( socket );
	g.Free(  );
}

function onGumpPress( socket, btn, gumpData )
{
	if( !socket )
		return;

	var atlas = socket.tempObj2; socket.tempObj2 = null;
	if( !ValidateObject( atlas ))
	{
		socket.SysMessage( GetDictionaryEntry( 9258, socket.language ));
		return;
	}

	var pUser = socket.currentChar;

	// range recheck
	if( !pUser.InRange( atlas, 3 ))
	{
		var root = FindRootContainer( atlas, 0 );
		if( ValidateObject( root ))
		{
			if( !pUser.InRange( root, 3 ))
			{
				socket.SysMessage( GetDictionaryEntry( 393, socket.language ));
				atlas.SetTag( "inUse", null );
				atlas.SetTag( "userSerial", null );
				return false;
			}
		}
		else
		{ 
			socket.SysMessage( GetDictionaryEntry( 393, socket.language ));
			atlas.SetTag( "inUse", null );
			atlas.SetTag( "userSerial", null );
			return false;
		}
	}

	var page = parseInt( atlas.GetTag( "atlasPage" )) || 0;
	var cap = parseInt( atlas.GetTempTag( "atlasMaxCharges" )) || ATLAS_MAX_CHARGES;

	// Close
	if( btn === 0 )
	{ 
		atlas.SetTag( "inUse", null );
		atlas.SetTag( "userSerial", null );
		return;
	}

	// Page nav
	if( btn === 1150 )
	{
		page = Math.min( 2, page + 1 );
		atlas.SetTag( "atlasPage", page.toString());
		return DisplayAtlasGump( socket, pUser, atlas, page );
	}
	if( btn === 1151 )
	{ 
		page = Math.max( 0, page - 1 );
		atlas.SetTag( "atlasPage", page.toString());
		return DisplayAtlasGump( socket, pUser, atlas, page );
	}

	// Rename
	if( btn === 1 )
	{
		if( !CheckAccessRights( socket, pUser, atlas ))
			return;
		socket.SysMessage( GetDictionaryEntry( 9261, socket.language )); // enter title
		pUser.SpeechInput( 1, atlas );
		return;
	}

	// Select row ( 100..147 for 48 entries )
	if( btn >= 100 && btn < 100 + ATLAS_MAX_RUNES )
	{
		var slot = ( btn - 100 ) + 1;
		atlas.SetTag( "selectedSlot", slot );
		return DisplayAtlasGump( socket, pUser, atlas, page );
	}

	// Read current selection
	var selected = parseInt( atlas.GetTag( "selectedSlot" ));
	var hasSel = !!( selected && selected >= 1 && selected <= ATLAS_MAX_RUNES );
	var entryTag = hasSel ? atlas.GetTag( "rune" + selected + "Data" ) : 0;

	switch( btn )
	{
		case 2: // Set Default
			if( !hasSel )
			{ 
				socket.SysMessage( GetDictionaryEntry( 9260, socket.language ));
				break;
			}

			if( !CheckAccessRights( socket, pUser, atlas ))
				return;

			if( entryTag != 0 )
			{
				var s = entryTag.split( "," );
				atlas.morex = s[2] | 0; atlas.morey = s[3] | 0; atlas.morez = s[4] | 0; atlas.more = s[5] | 0; if( s[6] ) atlas.more0 = s[6] | 0;
				atlas.SetTag( "defaultRuneLoc", selected );
				socket.SysMessage( GetDictionaryEntry( 9259, socket.language ));
			}
			else
				socket.SysMessage( GetDictionaryEntry( 9260, socket.language ));
			break;

		case 3: // Drop Rune
			if( !hasSel )
			{
				socket.SysMessage( GetDictionaryEntry( 9260, socket.language ));
				break;
			}

			if( !CheckAccessRights( socket, pUser, atlas ))
				return;

			var pack = pUser.pack;
			if( pack.totalItemCount >= pack.maxItems || pack.weight >= pack.weightMax )
			{ 
				socket.SysMessage( GetDictionaryEntry( 9263, socket.language ));
				break;
			}

			if( entryTag != 0 )
			{
				var s2 = entryTag.split( "," );
				var dropped = CreateDFNItem( socket, pUser, "0x1f14", 1, "ITEM", true );
				dropped.morex = s2[2] | 0; dropped.morey = s2[3] | 0; dropped.morez = s2[4] | 0; dropped.more = s2[5] | 0; if( s2[6] ) dropped.more0 = s2[6] | 0;
				dropped.name = s2[0];

				if(( atlas.GetTag( "defaultRuneLoc" ) | 0 ) === selected )
				{
					atlas.SetTag( "defaultRuneLoc", null );
					atlas.morex = 0;
					atlas.morey = 0;
					atlas.morez = 0; 
					atlas.more = 0; 
					atlas.more0 = 0;
				}

				atlas.SetTag( "rune" + selected + "Data", null );
				var cnt = parseInt( atlas.GetTag( "runeCount" )) | 0; atlas.SetTag( "runeCount", Math.max( 0, cnt - 1 ));
				socket.SysMessage( GetDictionaryEntry( 9264, socket.language ));
			}
			else
				socket.SysMessage( GetDictionaryEntry( 9265, socket.language ));
			break;

		case 4: // Recall ( Spell )
			if( !hasSel || entryTag == 0 ) 
			{
				socket.SysMessage( GetDictionaryEntry( 9260, socket.language ));
				break;
			}
			socket.tempObj2 = atlas; socket.tempInt2 = selected;
			atlas.SetTag( "inUse", null ); atlas.SetTag( "userSerial", null );
			atlas.SetTempTag( "useDelayed", GetCurrentClock().toString());
			CastAtlasSpell( socket, pUser, SPELL_RECALL, true );
			return;

		case 5: // Recall ( Charge )
			if( !hasSel || entryTag == 0 )
			{
				socket.SysMessage( GetDictionaryEntry( 9260, socket.language ));
				break;
			}

			if(( atlas.health | 0 ) <= 0 )
			{ 
				socket.SysMessage( GetDictionaryEntry( 9262, socket.language ));
				atlas.health = 0;
				break;
			}
			atlas.health = Math.max( 0, ( atlas.health | 0 ) - 1 );
			socket.tempObj2 = atlas; socket.tempInt2 = selected;
			atlas.SetTag( "inUse", null ); atlas.SetTag( "userSerial", null );
			atlas.SetTempTag( "useDelayed", GetCurrentClock().toString());
			CastAtlasSpell( socket, pUser, SPELL_RECALL, false );
			return;

		case 6: // Gate Travel
			if( !hasSel || entryTag == 0 )
			{ 
				socket.SysMessage( GetDictionaryEntry( 9260, socket.language ));
				break;
			}
			socket.tempObj2 = atlas; socket.tempInt2 = selected;
			atlas.SetTag( "inUse", null ); atlas.SetTag( "userSerial", null );
			atlas.SetTempTag( "useDelayed", GetCurrentClock().toString());
			CastAtlasSpell( socket, pUser, SPELL_GATE, true );
			return;

		case 7: // Sacred Journey ( optional )
			if( !hasSel || entryTag == 0 ) 
			{ 
				socket.SysMessage( GetDictionaryEntry( 9260, socket.language ));
				break;
			}
			if( !( pUser.HasSpell && pUser.HasSpell( SPELL_SACRED_JOUR )))
			{
				socket.SysMessage( GetDictionaryEntry( 9266, socket.language ));
				break;
			}
			socket.tempObj2 = atlas; socket.tempInt2 = selected;
			atlas.SetTag( "inUse", null ); atlas.SetTag( "userSerial", null );
			atlas.SetTempTag( "useDelayed", GetCurrentClock().toString());
			CastAtlasSpell( socket, pUser, SPELL_SACRED_JOUR, true );
			return;

		default: break;
	}

	DisplayAtlasGump( socket, pUser, atlas, page );
}

function CastAtlasSpell( socket, pUser, spellNum, checkReagents )
{
	var atlas = socket.tempObj2;
	var slot = socket.tempInt2 | 0;
	var data = atlas.GetTag( "rune" + slot + "Data" );
	var s = data.split( "," );
	var x = s[2] | 0, y = s[3] | 0, z = s[4] | 0, w = s[5] | 0, inst = s[6] ? ( s[6] | 0 ) : 0;

	if( pUser.GetTimer( Timer.SPELLTIME ) != 0 )
	{
		if( pUser.isCasting )
		{ 
			socket.SysMessage( GetDictionaryEntry( 762, socket.language ));
			return;
		}
		else if( pUser.GetTimer( Timer.SPELLTIME ) > GetCurrentClock())
		{
			socket.SysMessage( GetDictionaryEntry( 1638, socket.language ));
			return;
		}
	}

	if( pUser.isJailed )
	{
		socket.SysMessage( GetDictionaryEntry( 704, socket.language ));
		return;
	}

	if( TriggerEvent( 6002, "CheckTravelRestrictions", pUser, spellNum, x, y, w, inst ))
		return;

	var mSpell = Spells[spellNum];
	if( !mSpell || !mSpell.enabled )
	{
		socket.SysMessage( GetDictionaryEntry( 707, socket.language ));
		return;
	}
	if( checkReagents && !pUser.HasSpell( spellNum ))
	{
		socket.SysMessage( GetDictionaryEntry( 9266, socket.language ));
		return;
	}

	if( !GetServerSetting( "TravelSpellsWhileAggressor" ) && ( pUser.IsAggressor() || pUser.criminal ))
	{
		socket.SysMessage( GetDictionaryEntry( 2066, socket.language ));
		return;
	}

	var r = pUser.FindItemLayer( 0x01 ), l = pUser.FindItemLayer( 0x02 );
	if(( ValidateObject( l ) && l.type != 119 ) || ( ValidateObject( r ) && ( r.type != 9 && r.type != 119 )) )
	{ 
		socket.SysMessage( GetDictionaryEntry( 708, socket.language ));
		return;
	}

	if( checkReagents && !TriggerEvent( 6004, "CheckReagents", pUser, mSpell ))
		return;

	if( mSpell.mana > pUser.mana )
	{
		socket.SysMessage( GetDictionaryEntry( 696, socket.language ));
		return;
	}
	if( mSpell.stamina > pUser.stamina )
	{
		socket.SysMessage( GetDictionaryEntry( 697, socket.language ));
		return;
	}
	if( mSpell.health >= pUser.health )
	{
		socket.SysMessage( GetDictionaryEntry( 698, socket.language ));
		return;
	}

	if( pUser.visible == 1 || pUser.visible == 2 ) 
		pUser.visible = 0;

	pUser.BreakConcentration( socket );
	pUser.spellCast = spellNum; pUser.nextAct = 75;

	var delay = mSpell.delay * 100;
	pUser.SetTimer( Timer.SPELLTIME, delay );
	pUser.frozen = true;

	if( !pUser.isonhorse )
	{
		var act = mSpell.action;
		if( pUser.isHuman || act != 0x22 )
		pUser.DoAction( act );
	}

	if( !pUser.CheckSkill( 25, mSpell.lowSkill, mSpell.highSkill ))
	{
		pUser.TextMessage( mSpell.mantra );

		if( checkReagents )
			TriggerEvent( 6004, "DeleteReagents", pUser, mSpell );

		pUser.SpellFail(  );
		pUser.isCasting = false;
		pUser.frozen = false;
		pUser.SetTimer( Timer.SPELLTIME, 0 );
		pUser.spellCast = -1;
		return;
	}

	pUser.mana -= mSpell.mana; pUser.health -= mSpell.health; pUser.stamina -= mSpell.stamina;
	if( checkReagents )
		TriggerEvent( 6004, "DeleteReagents", pUser, mSpell );

	pUser.TextMessage( mSpell.mantra );
	pUser.isCasting = true;

	// keep atlas/slot for timer
	socket.tempObj2 = atlas; socket.tempInt2 = slot;

	pUser.StartTimer( delay, spellNum, true );
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( timerObj, timerID )
{
	var socket = timerObj.socket;
	if( !socket )
		return;

	var atlas = socket.tempObj2; socket.tempObj2 = null;
	if( !ValidateObject( atlas ))
	{ 
		socket.SysMessage( GetDictionaryEntry( 9267, socket.language ));
		return;
	}

	var slot = socket.tempInt2 | 0; socket.tempInt2 = 0;
	if( slot < 1 || slot > ATLAS_MAX_RUNES )
	{
		socket.SysMessage( GetDictionaryEntry( 9267, socket.language ));
		return;
	}

	var rd = atlas.GetTag( "rune" + slot + "Data" );
	var s = rd.split( "," );
	var x = s[2] | 0, y = s[3] | 0, z = s[4] | 0, w = s[5] | 0, inst = s[6] ? ( s[6] | 0 ) : 0;

	var spellNum = timerObj.spellCast, mSpell = Spells[spellNum];
	timerObj.isCasting = false; timerObj.frozen = false; timerObj.SetTimer( Timer.SPELLTIME, 0 ); timerObj.spellCast = -1;

	timerObj.SoundEffect( mSpell.soundEffect, true );
	timerObj.SpellStaticEffect( mSpell );

	if( spellNum == SPELL_GATE )
		timerObj.Gate( x, y, z, w, inst );
	else
		TriggerEvent( 6003, "TeleportHelper", timerObj, x, y, z, w, inst, true );
}

/* --------------------------- Drag/drop + Rename --------------------------- */
function onDropItemOnItem( iDropped, pUser, atlas )
{
	var socket = pUser.socket;

	if( iDropped.type == 50 ) // recall rune
	{
		if( !CheckAccessRights( socket, pUser, atlas ))
			return false;

		if( iDropped.morex == 0 && iDropped.morey == 0 && iDropped.morez == 0 )
		{
			socket.SysMessage( GetDictionaryEntry( 431, pUser.socket.language ));
			return false;
		}

		var count = parseInt( atlas.GetTag( "runeCount" )) | 0;
		if( count >= ATLAS_MAX_RUNES ) 
		{ 
			socket.SysMessage( GetDictionaryEntry( 9273, socket.language ));
			return false;
		}

		for( var i = 1; i <= ATLAS_MAX_RUNES; i++ )
		{
			var rd = atlas.GetTag( "rune" + i + "Data" );
			if( rd == 0 )
			{
				var nm = iDropped.name, col = iDropped.color;
				var x = iDropped.morex | 0, y = iDropped.morey | 0, z = iDropped.morez | 0, w = iDropped.more | 0, inst = iDropped.more0 | 0;
				atlas.SetTag( "rune" + i + "Data", nm + "," + col + "," + x + "," + y + "," + z + "," + w + "," + inst );
				atlas.SetTag( "runeCount", count + 1 );
				iDropped.Delete(  );

				var msg = GetDictionaryEntry( 9274, socket.language );
				socket.SysMessage( msg.replace( /%s/gi, nm ));

				// select the newly added and jump to correct page
				atlas.SetTag( "selectedSlot", i );
				var pageForI = (( i - 1 ) / 16 ) | 0; atlas.SetTag( "atlasPage", pageForI.toString(  ));
				socket.CloseGump( 0xffff + SCRIPT_ID, 0 );
				onUseChecked( pUser, atlas );
				return 2;
			}
		}
	}
	else if( iDropped.id == 0x1F4C ) // recall scroll
	{
		var cap = atlas.maxhp > 0 ? atlas.maxhp : ATLAS_MAX_CHARGES;
		var c = atlas.health | 0;
		if( c >= cap )
		{ 
			socket.SysMessage( GetDictionaryEntry( 9275, socket.language ));
			return false;
		}

		if( iDropped.amount > 1 )
		{
			if( iDropped.amount <= cap - c )
			{
				atlas.health = c + iDropped.amount; iDropped.Delete();
			}
			else
			{ 
				atlas.health = cap; 
				iDropped.amount -= ( cap - c );
			}
			socket.SysMessage( GetDictionaryEntry( 9276, socket.language ));
		}
		else
		{
			atlas.health = c + 1; iDropped.Delete(  );
			socket.SysMessage( GetDictionaryEntry( 9277, socket.language ));
		}

		socket.CloseGump( 0xffff + SCRIPT_ID, 0 );
		onUseChecked( pUser, atlas );
		return 2;
	}

	return true;
}

/** @type { (  myChar: Character, myItem: Item, mySpeech: string  ) => void } */
function onSpeechInput( pUser, atlas, text, id )
{
	var socket = pUser.socket;
	if( !ValidateObject( socket ))
	{
		return;
	}

	if( text == null || text == " " )
	{ 
		socket.SysMessage( GetDictionaryEntry( 9270, socket.language ));
		return;
	}

	switch( id )
	{
		case 1:
			if( text.length > 50 )
			{ 
				pUser.SysMessage( GetDictionaryEntry( 9271, socket.language ));
				return;
			}
			if( ValidateObject( atlas ))
			{
				atlas.name = text;
				var msg = GetDictionaryEntry( 9272, socket.language );
				socket.SysMessage( msg.replace( /%s/gi, atlas.name ));
			}
			else
				socket.SysMessage( GetDictionaryEntry( 9267, socket.language ));
			break;
		default: break;
	}
}