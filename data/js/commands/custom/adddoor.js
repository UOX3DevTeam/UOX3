// ----- UI constants -----
var tooltipproperty = 1042971;
var width  = 740;
var height = 400;
var BTN_ID_BASE = 1;   // avoid 0 which is 'cancel'
var GRID_LEFT   = 40;   // comfy left margin
var GRID_TOP    = 34;   // push top row down a bit
var COL_SPACING = 105;  // keep this
var ROW_SPACING = 120;  // more room between rows
var SECOND_ROW_BUMP = 54;   // try 24–28px; tweak to taste
var CTRL_W   = 250;
var CTRL_X   = width - CTRL_W;  // start x of the control stack
var CTRL_GAP = 22;              // vertical spacing between lines
var PANEL_Y_START = 18;      // <— single source of truth for top Y
var PANEL_X_LABEL = CTRL_X + 8;
var PANEL_X_BTN   = PANEL_X_LABEL + 95; // label col width (~92)
var BTN_NEXT     = 9001;
var BTN_BACK     = 9002;
var BTN_UNDO_LAST = 9301;          // "Undo Last" (deletes last placed door)
var BTN_REPEAT_LAST = 9302; // near BTN_UNDO_LAST
var BTN_TOGGLE_PAINT = 9303;
var CBID_PLACE_DOOR = 0;         // onCallback0 will run after you pick a spot
var UNDO_MAX = 20;
var BTN_TAB_FAV  = 9201;
var BTN_TAB_REC  = 9202;
var BTN_TAB_CAT  = 9203;
var STAR_OFFSET  = 10000; // star button id = STAR_OFFSET + slotButtonId
var CATALOG_VIEW = "catalog";
var FAV_VIEW     = "favorites";
var REC_VIEW     = "recent";
var RECENT_MAX   = 12;
var BTN_HUE_SET   = 9401;
var BTN_HUE_PICK  = 9402;
var BTN_HUE_M10   = 9403;
var BTN_HUE_M1    = 9404;
var BTN_HUE_P1    = 9405;
var BTN_HUE_P10   = 9406;
var HUE_EDIT_ID   = 17;     // only text entry in this gump
var CBID_PICK_HUE = 1;   // onCallback1 for “Pick Hue”
var BTN_TOGGLE_LINK   = 9501;
var BTN_TOGGLE_UNLINK = 9502;   // optional, for unlink mode
var BTN_TOGGLE_LINK_ADD = 9503;   // UI toggle
var CBID_LINK   = 2;  // onCallback2
var CBID_UNLINK = 3;  // onCallback3

// map buttonId -> itemID to add (refilled every render)
var buttonToItem = {};

// =================== Catalog Data ===================
// Each page: 8 slots (artId shown and cmd used for radd)
var PAGES = [
	// Page 1 - Metal Doors
	{
		label: "1",
		slots: [
			{ artId:0x0675, dict:9900, cmd:0x0675 },
			{ artId:0x0677, dict:9900, cmd:0x0677 },
			{ artId:0x0679, dict:9900, cmd:0x0679 },
			{ artId:0x067B, dict:9900, cmd:0x067B },
			{ artId:0x067D, dict:9901, cmd:0x067D },
			{ artId:0x067F, dict:9901, cmd:0x067F },
			{ artId:0x0681, dict:9901, cmd:0x0681 },
			{ artId:0x0683, dict:9901, cmd:0x0683 }
		]
	},
	// Page 2 - Barred Metal
	{
		label: "2",
		slots: [
			{ artId:1669, dict:9902, cmd:1669 },
			{ artId:1671, dict:9902, cmd:1671 },
			{ artId:1673, dict:9902, cmd:1673 },
			{ artId:1675, dict:9902, cmd:1675 },
			{ artId:1683, dict:9903, cmd:1683 },
			{ artId:1681, dict:9903, cmd:1681 },
			{ artId:1679, dict:9903, cmd:1679 },
			{ artId:1677, dict:9903, cmd:1677 }
		]
	},
	// Page 3 - Rotten
	{
		label: "3",
		slots: [
			{ artId:1685, dict:9904, cmd:1685 },
			{ artId:1687, dict:9904, cmd:1687 },
			{ artId:1689, dict:9904, cmd:1689 },
			{ artId:1691, dict:9904, cmd:1691 },
			{ artId:1699, dict:9905, cmd:1699 },
			{ artId:1697, dict:9905, cmd:1697 },
			{ artId:1695, dict:9905, cmd:1695 },
			{ artId:1693, dict:9905, cmd:1693 }
		]
	},
	// Page 4 - Wooden
	{
		label: "4",
		slots: [
			{ artId:1701, dict:9906, cmd:1701 },
			{ artId:1703, dict:9906, cmd:1703 },
			{ artId:1705, dict:9906, cmd:1705 },
			{ artId:1707, dict:9906, cmd:1707 },
			{ artId:1715, dict:9907, cmd:1715 },
			{ artId:1713, dict:9907, cmd:1713 },
			{ artId:1711, dict:9907, cmd:1711 },
			{ artId:1709, dict:9907, cmd:1709 }
		]
	},
	// Page 5 - Tall Wooden
	{
		label: "5",
		slots: [
			{ artId:1717, dict:9908, cmd:1717 },
			{ artId:1719, dict:9908, cmd:1719 },
			{ artId:1721, dict:9908, cmd:1721 },
			{ artId:1723, dict:9908, cmd:1723 },
			{ artId:1731, dict:9909, cmd:1731 },
			{ artId:1729, dict:9909, cmd:1729 },
			{ artId:1727, dict:9909, cmd:1727 },
			{ artId:1725, dict:9909, cmd:1725 }
		]
	},
	// Page 6 - Tall Metal
	{
		label: "6",
		slots: [
			{ artId:0x06C5, dict:9910, cmd:0x06C5 },
			{ artId:0x06C7, dict:9910, cmd:0x06C7 },
			{ artId:0x06C9, dict:9910, cmd:0x06C9 },
			{ artId:0x06CB, dict:9910, cmd:0x06CB },
			{ artId:0x06CD, dict:9910, cmd:0x06CD },
			{ artId:0x06CF, dict:9910, cmd:0x06CF },
			{ artId:0x06D1, dict:9910, cmd:0x06D1 },
			{ artId:0x06D3, dict:9910, cmd:0x06D3 }
		]
	},
	// Page 7 - Light Wooden
	{
		label: "7",
		slots: [
			{ artId:0x06D5, dict:9911, cmd:0x06D5 },
			{ artId:0x06D7, dict:9911, cmd:0x06D7 },
			{ artId:0x06D9, dict:9911, cmd:0x06D9 },
			{ artId:0x06DB, dict:9911, cmd:0x06DB },
			{ artId:0x06DD, dict:9912, cmd:0x06DD },
			{ artId:0x06DF, dict:9912, cmd:0x06DF },
			{ artId:0x06E1, dict:9912, cmd:0x06E1 },
			{ artId:0x06E3, dict:9912, cmd:0x06E3 }
		]
	},
	// Page 8 - Wood w/ Metal Braces
	{
		label: "8",
		slots: [
			{ artId:0x06E5, dict:9913, cmd:0x06E5 },
			{ artId:0x06E7, dict:9913, cmd:0x06E7 },
			{ artId:0x06E9, dict:9913, cmd:0x06E9 },
			{ artId:0x06EB, dict:9913, cmd:0x06EB },
			{ artId:0x06ED, dict:9914, cmd:0x06ED },
			{ artId:0x06EF, dict:9914, cmd:0x06EF },
			{ artId:0x06F1, dict:9914, cmd:0x06F1 },
			{ artId:0x06F3, dict:9914, cmd:0x06F3 }
		]
	},
	// Page 9 - Wooden gates
	{
		label: "9",
		slots: [
			{ artId:0x0866, dict:9913, cmd:0x0866 },
			{ artId:0x0868, dict:9913, cmd:0x0868 },
			{ artId:0x086A, dict:9913, cmd:0x086A },
			{ artId:0x086C, dict:9913, cmd:0x086C },
			{ artId:0x086E, dict:9914, cmd:0x086E },
			{ artId:0x0870, dict:9914, cmd:0x0870 },
			{ artId:0x0872, dict:9914, cmd:0x0872 },
			{ artId:0x0874, dict:9914, cmd:0x0874 }
		]
	},
	// Page 10 - Light Wooden gates
	{
		label: "10",
		slots: [
			{ artId:0x0839, dict:9913, cmd:0x0839 },
			{ artId:0x083B, dict:9913, cmd:0x083B },
			{ artId:0x083D, dict:9913, cmd:0x083D },
			{ artId:0x083F, dict:9913, cmd:0x083F },
			{ artId:0x0841, dict:9914, cmd:0x0841 },
			{ artId:0x0843, dict:9914, cmd:0x0843 },
			{ artId:0x0845, dict:9914, cmd:0x0845 },
			{ artId:0x0847, dict:9914, cmd:0x0847 }
		]
	},
	// Page 11 - Tall Metal Gates
	{
		label: "11",
		slots: [
			{ artId:0x0824, dict:9913, cmd:0x0824 },
			{ artId:0x0826, dict:9913, cmd:0x0826 },
			{ artId:0x0828, dict:9913, cmd:0x0828 },
			{ artId:0x082A, dict:9913, cmd:0x082A },
			{ artId:0x082C, dict:9914, cmd:0x082C },
			{ artId:0x082E, dict:9914, cmd:0x082E },
			{ artId:0x0830, dict:9914, cmd:0x0830 },
			{ artId:0x0832, dict:9914, cmd:0x0832 }
		]
	},
	// Page 12 - Short Metal Gates
	{
		label: "12",
		slots: [
			{ artId:0x084C, dict:9913, cmd:0x084C },
			{ artId:0x084E, dict:9913, cmd:0x084E },
			{ artId:0x0850, dict:9913, cmd:0x0850 },
			{ artId:0x0852, dict:9913, cmd:0x0852 },
			{ artId:0x0854, dict:9914, cmd:0x0854 },
			{ artId:0x0856, dict:9914, cmd:0x0856 },
			{ artId:0x0858, dict:9914, cmd:0x0858 },
			{ artId:0x085A, dict:9914, cmd:0x085A }
		]
	},
	// Page 13 - bar Door
	{
		label: "13",
		slots: [
			{ artId:0x190E, dict:9913, cmd:0x190E },
			{ artId:0x190F, dict:9913, cmd:0x190F }
		]
	},
	// Page 14 - sliding doors
	{
		label: "14",
		slots: [
			{ artId:0x2A05, dict:9913, cmd:0x2A05 },
			{ artId:0x2A07, dict:9913, cmd:0x2A07 },
			{ artId:0x2A09, dict:9913, cmd:0x2A09 },
			{ artId:0x2A0B, dict:9913, cmd:0x2A0B },
			{ artId:0x2A0D, dict:9914, cmd:0x2A0D },
			{ artId:0x2A0F, dict:9914, cmd:0x2A0F },
			{ artId:0x2A11, dict:9914, cmd:0x2A11 },
			{ artId:0x2A13, dict:9914, cmd:0x2A13 }
		]
	},
	// Page 15 - sliding doors
	{
		label: "15",
		slots: [
			{ artId:0x2A15, dict:9913, cmd:0x2A15 },
			{ artId:0x2A17, dict:9913, cmd:0x2A17 },
			{ artId:0x2A19, dict:9913, cmd:0x2A19 },
			{ artId:0x2A1B, dict:9913, cmd:0x2A1B }
		]
	},
	// Page 16 - metal door?
	{
		label: "16",
		slots: [
			{ artId:0x9AD7, dict:9913, cmd:0x9AD7 },
			{ artId:0x9AD9, dict:9913, cmd:0x9AD9 },
			{ artId:0x9ADB, dict:9913, cmd:0x9ADB },
			{ artId:0x9ADD, dict:9913, cmd:0x9ADD },
			{ artId:0x9ADF, dict:9914, cmd:0x9ADF },
			{ artId:0x9AE1, dict:9914, cmd:0x9AE1 },
			{ artId:0x9AE3, dict:9914, cmd:0x9AE3 },
			{ artId:0x9AE5, dict:9914, cmd:0x9AE5 }
		]
	},
	// Page 17 - door
	{
		label: "17",
		slots: [
			{ artId:0x50C8, dict:9913, cmd:0x50C8 },
			{ artId:0x50CA, dict:9913, cmd:0x50CA },
			{ artId:0x50CC, dict:9913, cmd:0x50CC },
			{ artId:0x50CE, dict:9913, cmd:0x50CE },
			{ artId:0x50D0, dict:9914, cmd:0x50D0 },
			{ artId:0x50D2, dict:9914, cmd:0x50D2 },
			{ artId:0x50D4, dict:9914, cmd:0x50D4 },
			{ artId:0x50D6, dict:9914, cmd:0x50D6 }
		]
	},
	// Page 18 - door
	{
		label: "18",
		slots: [
			{ artId:0x5142, dict:9913, cmd:0x5142 },
			{ artId:0x5144, dict:9913, cmd:0x5144 },
			{ artId:0x5146, dict:9913, cmd:0x5146 },
			{ artId:0x5148, dict:9913, cmd:0x5148 }
		]
	},
	// Page 19 - large queen door
	{
		label: "19",
		slots: [
			{ artId:0x4D1A, dict:9913, cmd:0x4D1A },
			{ artId:0x4D1C, dict:9913, cmd:0x4D1C },
			{ artId:0x4D1E, dict:9913, cmd:0x4D1E },
			{ artId:0x4D20, dict:9913, cmd:0x4D20 }
		]
	},
	// Page 20 - small queen door
	{
		label: "20",
		slots: [
			{ artId:0x4D22, dict:9914, cmd:0x4D22 },
			{ artId:0x4D24, dict:9914, cmd:0x4D24 },
			{ artId:0x4D26, dict:9914, cmd:0x4D26 },
			{ artId:0x4D28, dict:9914, cmd:0x4D28 }
		]
	}
];

// Build a quick art-info map for Fav/Recent pages
var ARTINFO = {};
function buildArtInfo() 
{
	for (var p = 0; p < PAGES.length; p++)
	{
		var page = PAGES[p];
		for (var i = 0; i < page.slots.length; i++)
		{
			var s = page.slots[i];
			ARTINFO[s.cmd] = { artId: s.artId, dict: s.dict, cmd: s.cmd };
		}
	}
}
buildArtInfo();

function CommandRegistration()
{
	RegisterCommand( "adddoor", 2, true ); // GM-only
}

function command_ADDDOOR( socket, cmdString )
{
	var pUser = socket.currentChar;
	var view  = getView( pUser );
	var page  = pUser.GetTempTag( "getdoor" ) || 1;

	if( view === FAV_VIEW )
	{
		showFavorites( pUser, ( page|0 ) - 1 );
	}
	else if( view === REC_VIEW )
	{
		showRecent( pUser, ( page|0 ) - 1 );
	}
	else
	{
		showDoorPage( pUser, page - 1 );
	}
}

function AddBlueBack( gump )
{
	gump.AddBackground( 0, 0, width, height, 0x0E10 );
	gump.AddBackground( 8, 5, width - 16, height - 11, 0x053 );
	gump.AddTiledGump( 15, 14, width - 29, height - 29, 0x0E14 );
	gump.AddCheckerTrans( 15, 14, width - 29, height - 29 );
	gump.AddTiledGump( CTRL_X - 8, 14, 5, height - 29, 0x0E14 );
}

function addDoorButton( gump, socket, btnId, slot )
{
	var pUser = socket.currentChar;
	gump.AddButton( slot.btnX, slot.btnY, 0xfc, 0xfd, 1, 0, btnId );
	gump.AddPictureColor( slot.picX, slot.picY, slot.artId, getHue( socket.currentChar ));
	gump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + GetDictionaryEntry( slot.dict, socket.language ) + "</basefont>" );
}

function addNavigation( gump, socket, pageIndex, totalPages )
{
	var xLbl = PANEL_X_LABEL, xBtn = PANEL_X_BTN, y = PANEL_Y_START;
	gump.AddHTMLGump( xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>Catalog</basefont>" );
	gump.AddButton( xBtn, y-2, 0x0FA5, 0x0FA7, 1, 0, BTN_TAB_CAT );
	y += CTRL_GAP;

	gump.AddHTMLGump( xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>Fav</basefont>" );
	gump.AddButton( xBtn, y-2, 0x0FA5, 0x0FA7, 1, 0, BTN_TAB_FAV );
	y += CTRL_GAP;

	gump.AddHTMLGump( xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>Recent</basefont>" );
	gump.AddButton( xBtn, y-2, 0x0FA5, 0x0FA7, 1, 0, BTN_TAB_REC );
	y += CTRL_GAP + 6;

	gump.AddHTMLGump( xLbl, y, 140, 20, false, false, "<basefont color=#ffffff>Page " + ( pageIndex + 1 ) + " / " + totalPages + "</basefont>" );
	y += CTRL_GAP + 2;

	if( pageIndex < totalPages - 1 )
	{
		gump.AddHTMLGump( xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9917, socket.language ) + "</basefont>" );
		gump.AddButton( xBtn, y-2, 0x0FA5, 0x0FA7, 1, 0, BTN_NEXT );
		y += CTRL_GAP;
	}

	if( pageIndex > 0 )
	{
		gump.AddHTMLGump( xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 9918, socket.language ) + "</basefont>" );
		gump.AddButton( xBtn, y-2, 0x0FAE, 0x0FB0, 1, 0, BTN_BACK );
	}
	y += CTRL_GAP;

	gump.AddHTMLGump( xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>Undo Last</basefont>" );
	gump.AddButton( xBtn, y - 2, 0x0FAB, 0x0FAD, 1, 0, BTN_UNDO_LAST );
	y += CTRL_GAP;

	gump.AddHTMLGump(xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>Repeat Last</basefont>" );
	gump.AddButton( xBtn, y - 2, 0x0FA5, 0x0FA7, 1, 0, BTN_REPEAT_LAST );
	y += CTRL_GAP;

	var pUser = socket.currentChar;
	var paintOn = ( pUser.GetTempTag( "doorPaintMode" )|0 ) === 1;
	gump.AddHTMLGump( xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>Paint: " + ( paintOn ? "ON" : "OFF" ) + "</basefont>" );
	gump.AddButton( xBtn, y - 2, paintOn ? 0x0FB0 : 0x0FAE, paintOn ? 0x0FB0 : 0x0FAE, 1, 0, BTN_TOGGLE_PAINT );
	y += CTRL_GAP;

	// Link mode toggle
	var linkOn = ( pUser.GetTag("doorLinkMode" )|0 ) === 1;
	gump.AddHTMLGump( PANEL_X_LABEL, y, 120, 20, false, false, "<basefont color=#ffffff>Link: " + ( linkOn ? "ON" : "OFF" ) + "</basefont>" );
	gump.AddButton( PANEL_X_BTN, y - 2, linkOn ? 0x0FB0 : 0x0FAE, linkOn ? 0x0FB0 : 0x0FAE, 1, 0, BTN_TOGGLE_LINK );
	y += CTRL_GAP;

	// (optional) Unlink mode toggle
	var unlinkOn = ( pUser.GetTag( "doorUnlinkMode" )|0 ) === 1;
	gump.AddHTMLGump( PANEL_X_LABEL, y, 120, 20, false, false, "<basefont color=#ffffff>Unlink: " + ( unlinkOn ? "ON" : "OFF" ) + "</basefont>" );
	gump.AddButton( PANEL_X_BTN, y - 2, unlinkOn ? 0x0FB0 : 0x0FAE, unlinkOn ? 0x0FB0 : 0x0FAE, 1, 0, BTN_TOGGLE_UNLINK );

	// Auto-link on Add toggle
	y += CTRL_GAP;
	var autoLinkOn = ( socket.currentChar.GetTag( "doorLinkOnAdd" )|0 ) === 1;
	gump.AddHTMLGump( PANEL_X_LABEL, y, 140, 20, false, false, "<basefont color=#ffffff>Auto-link: " + ( autoLinkOn ? "ON" : "OFF" ) + "</basefont>" );
	gump.AddButton( PANEL_X_BTN, y - 2, autoLinkOn ? 0x0FB0 : 0x0FAE, autoLinkOn ? 0x0FB0 : 0x0FAE, 1, 0, BTN_TOGGLE_LINK_ADD )
	y += CTRL_GAP;

	// --- Hue controls ---
	// Pick Hue (copy from target)
	gump.AddHTMLGump( PANEL_X_LABEL, y, 100, 20, false, false, "<basefont color=#ffffff>Pick Hue</basefont>" );
	gump.AddButton( PANEL_X_BTN, y - 2, 0x0FA5, 0x0FA7, 1, 0, BTN_HUE_PICK );
	y += CTRL_GAP;

	// two-column layout
	var xLeftLbl  = PANEL_X_LABEL;
	var xLeftBtn  = PANEL_X_BTN;
	var xRightLbl = PANEL_X_BTN + 78 - 24;   // label before right button
	var xRightBtn = PANEL_X_BTN + 78;

	// Row 1: -10 (left)  +10 (right)
	gump.AddHTMLGump( xLeftLbl,  y, 40, 20, false, false, "<basefont color=#ffffff>-10</basefont>" );
	gump.AddButton( xLeftBtn, y - 2, 0x0FAE, 0x0FB0, 1, 0, BTN_HUE_M10 );
	gump.AddHTMLGump(xRightLbl, y, 40, 20, false, false, "<basefont color=#ffffff>+10</basefont>" );
	gump.AddButton( xRightBtn,y - 2, 0x0FA5, 0x0FA7, 1, 0, BTN_HUE_P10 );
	y += CTRL_GAP;

	// Row 2: +1  (left)  -1  (right)
	gump.AddHTMLGump( xLeftLbl,  y, 40, 20, false, false, "<basefont color=#ffffff>+1</basefont>" );
	gump.AddButton( xLeftBtn, y - 2, 0x0FA5, 0x0FA7, 1, 0, BTN_HUE_P1 );
	gump.AddHTMLGump( xRightLbl, y, 40, 20, false, false, "<basefont color=#ffffff>-1</basefont>" );
	gump.AddButton( xRightBtn,y - 2, 0x0FAE, 0x0FB0, 1, 0, BTN_HUE_M1 );
	y += CTRL_GAP;

	// Hue label + input + Set
	gump.AddHTMLGump( PANEL_X_LABEL, y, 100, 20, false, false, "<basefont color=#ffffff>Hue</basefont>" );
	gump.AddTextEntry( PANEL_X_BTN, y - 2, 64, 20, getHue( socket.currentChar ), HUE_EDIT_ID, HUE_EDIT_ID, fmtHue( getHue( socket.currentChar )));
	gump.AddButton( PANEL_X_BTN + 78, y - 2, 0x0FA5, 0x0FA7, 1, 0, BTN_HUE_SET );

}

// Favorite “star” icons: off=0x0647, on=0x0648
function addStarButton( gump, socket, starBtnId, isFav, btnX, btnY )
{
	// place star just above-right of the small arrow button
	var bx = btnX + 22;   // horizontal nudge from the arrow
	var by = btnY - 10;   // sit clearly above the door, not between rows

	var up   = isFav ? 0x0648 : 0x0647; // yellow if favorite, brown if not
	var down = up;

	gump.AddButton( bx, by, up, down, 1, 0, starBtnId );
	gump.AddToolTip( tooltipproperty, socket, "<basefont color=#ffffff>" + ( isFav ? "Remove Favorite" : "Add Favorite" ) + "</basefont>" );
}

function showDoorPage( pUser, pageIndex )
{
	setView( pUser, CATALOG_VIEW );

	var socket = pUser.socket;
	var DoorGump = new Gump;

	// clamp and remember
	if( pageIndex < 0 )
		pageIndex = 0;

	if( pageIndex >= PAGES.length )
		pageIndex = PAGES.length - 1;

	pUser.SetTempTag( "getdoor", pageIndex + 1 );

	DoorGump.AddPage( 0 );
	AddBlueBack( DoorGump );

	// clear stale button map
	buttonToItem = {};

	var page = PAGES[pageIndex];
	for( var i = 0; i < page.slots.length; i++ )
	{
		var s     = page.slots[i];
		var btnId = BTN_ID_BASE + ( pageIndex * 100 ) + i;

		// 2x4 grid position
		var col  = i % 4;
		var row  = ( i / 4 ) | 0;
		var rowBump = ( row === 1 ? SECOND_ROW_BUMP : 0 );
		var btnX = GRID_LEFT + ( col * COL_SPACING );
		var btnY = GRID_TOP  + ( row * ROW_SPACING ) + rowBump;
		// When computing picture coords in BOTH renderers:
		var picX = btnX - 10;   // leave as-is
		var picY = btnY + 20;   // leave as-is

		buttonToItem[btnId] = s.cmd;

		// draw door
		addDoorButton( DoorGump, socket, btnId, { btnX: btnX, btnY: btnY, picX: picX, picY: picY, artId: s.artId, dict: s.dict } );

		// star toggle
		var fav = isFavorite( pUser, s.cmd );
		addStarButton( DoorGump, socket, STAR_OFFSET + btnId, fav, btnX, btnY );
	}

	addNavigation( DoorGump, socket, pageIndex, PAGES.length );

	DoorGump.Send( pUser );
	DoorGump.Free();
}

// Generic renderer for a list of artIds (8 per page)
function showListPage( pUser, viewName, artList, pageIndex )
{
	setView( pUser, viewName );

	var socket = pUser.socket;
	var DoorGump = new Gump;

	// paginate
	var per = 8;
	var totalPages = Math.max( 1, Math.ceil( artList.length / per ));

	if( pageIndex < 0 )
		pageIndex = 0;

	if( pageIndex >= totalPages )
		pageIndex = totalPages - 1;

	// remember page per view
	pUser.SetTempTag( "getdoor", pageIndex + 1 );

	DoorGump.AddPage( 0 );
	AddBlueBack( DoorGump );

	// clear stale button map
	buttonToItem = {};

	// 2x4 grid layout
	var start = pageIndex * per;
	var end   = Math.min( artList.length, start + per );
	var idx   = 0;

	for( var k = start; k < end; k++, idx++ )
	{
		var artId = artList[k];
		var info  = ARTINFO[artId] || { artId: artId, dict: 9900, cmd: artId };

		// grid positions (same constants as catalog)
		var col  = idx % 4;
		var row  = ( idx / 4 ) | 0;
		var rowBump = ( row === 1 ? SECOND_ROW_BUMP : 0 );
		var btnX = GRID_LEFT + ( col * COL_SPACING );
		var btnY = GRID_TOP  + ( row * ROW_SPACING ) + rowBump;
		var picX = btnX - 5;
		var picY = btnY + 15;

		// synthetic button id bucket to avoid collisions
		var btnId = ( viewName === FAV_VIEW ? 40000 : 50000 ) + k;

		// draw entry
		buttonToItem[btnId] = info.cmd;
		addDoorButton( DoorGump, socket, btnId, { btnX:btnX, btnY:btnY, picX:picX, picY:picY, artId:info.artId, dict:info.dict });

		// star toggle (present in both views)
		var fav = isFavorite( pUser, info.cmd );
		addStarButton( DoorGump, socket, STAR_OFFSET + btnId, fav, btnX, btnY );
	}

	// right panel nav for list views
	var gump = DoorGump;
	var xLbl = PANEL_X_LABEL, xBtn = PANEL_X_BTN, y = PANEL_Y_START;

	gump.AddHTMLGump( xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>Catalog</basefont>" );
	gump.AddButton( xBtn, y-2, 0x0FA5, 0x0FA7, 1, 0, BTN_TAB_CAT );
	y += CTRL_GAP;

	gump.AddHTMLGump( xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>Fav</basefont>" );
	gump.AddButton( xBtn, y-2, 0x0FA5, 0x0FA7, 1, 0, BTN_TAB_FAV );
	y += CTRL_GAP;

	gump.AddHTMLGump( xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>Recent</basefont>");
	gump.AddButton( xBtn, y-2, 0x0FA5, 0x0FA7, 1, 0, BTN_TAB_REC );
	y += CTRL_GAP + 6;

	var label = ( viewName === FAV_VIEW ? "Favorites" : "Recent" );
	gump.AddHTMLGump( xLbl, y, 140, 20, false, false, "<basefont color=#ffffff>" + label + " " + ( pageIndex+1 ) + " / " + totalPages + "</basefont>" );
	y += CTRL_GAP + 2;

	if( pageIndex < totalPages - 1 )
	{
	  gump.AddHTMLGump( xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>Next</basefont>" );
	  gump.AddButton( xBtn, y-2, 0x0FA5, 0x0FA7, 1, 0, BTN_NEXT );
	  y += CTRL_GAP;
	}

	if( pageIndex > 0 )
	{
	  gump.AddHTMLGump( xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>Back</basefont>" );
	  gump.AddButton( xBtn, y-2, 0x0FAE, 0x0FB0, 1, 0, BTN_BACK );
	}

	// Undo Last
	gump.AddHTMLGump( xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>Undo Last</basefont>" );
	gump.AddButton( xBtn, y-2, 0x0FAB, 0x0FAD, 1, 0, BTN_UNDO_LAST );
	y += CTRL_GAP;

	// Repeat Last
	gump.AddHTMLGump( xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>Repeat Last</basefont>" );
	gump.AddButton( xBtn, y-2, 0x0FA5, 0x0FA7, 1, 0, BTN_REPEAT_LAST );
	y += CTRL_GAP;

	// Paint toggle
	var pUser = socket.currentChar;
	var paintOn = ( pUser.GetTempTag( "doorPaintMode" )|0 ) === 1;
	gump.AddHTMLGump( xLbl, y, 100, 20, false, false, "<basefont color=#ffffff>Paint: " + ( paintOn ? "ON" : "OFF" ) + "</basefont>" );
	gump.AddButton( xBtn, y-2, paintOn ? 0x0FB0 : 0x0FAE, paintOn ? 0x0FB0 : 0x0FAE, 1, 0, BTN_TOGGLE_PAINT );
	y += CTRL_GAP;

	// Link mode toggle
	var linkOn = ( pUser.GetTag( "doorLinkMode" )|0 ) === 1;
	gump.AddHTMLGump( PANEL_X_LABEL, y, 120, 20, false, false, "<basefont color=#ffffff>Link: " + ( linkOn ? "ON" : "OFF" ) + "</basefont>" );
	gump.AddButton( PANEL_X_BTN, y - 2, linkOn ? 0x0FB0 : 0x0FAE, linkOn ? 0x0FB0 : 0x0FAE, 1, 0, BTN_TOGGLE_LINK );
	y += CTRL_GAP;

	// (optional) Unlink mode toggle
	var unlinkOn = ( pUser.GetTag( "doorUnlinkMode" )|0 ) === 1;
	gump.AddHTMLGump( PANEL_X_LABEL, y, 120, 20, false, false, "<basefont color=#ffffff>Unlink: " + ( unlinkOn ? "ON" : "OFF" ) + "</basefont>" );
	gump.AddButton( PANEL_X_BTN, y - 2, unlinkOn ? 0x0FB0 : 0x0FAE, unlinkOn ? 0x0FB0 : 0x0FAE, 1, 0, BTN_TOGGLE_UNLINK );
	y += CTRL_GAP;

	// Auto-link on Add toggle
	var autoLinkOn = ( socket.currentChar.GetTag( "doorLinkOnAdd" )|0 ) === 1;
	gump.AddHTMLGump( PANEL_X_LABEL, y, 140, 20, false, false, "<basefont color=#ffffff>Auto-link: " + (autoLinkOn ? "ON" : "OFF" ) + "</basefont>" );
	gump.AddButton( PANEL_X_BTN, y - 2, autoLinkOn ? 0x0FB0 : 0x0FAE, autoLinkOn ? 0x0FB0 : 0x0FAE, 1, 0, BTN_TOGGLE_LINK_ADD )
	y += CTRL_GAP;

	// --- Hue controls ---
	// Pick Hue (copy from target)
	gump.AddHTMLGump( PANEL_X_LABEL, y, 100, 20, false, false, "<basefont color=#ffffff>Pick Hue</basefont>" );
	gump.AddButton( PANEL_X_BTN, y-2, 0x0FA5, 0x0FA7, 1, 0, BTN_HUE_PICK );
	y += CTRL_GAP;

	// two-column layout
	var xLeftLbl  = PANEL_X_LABEL;
	var xLeftBtn  = PANEL_X_BTN;
	var xRightLbl = PANEL_X_BTN + 78 - 24;   // label before right button
	var xRightBtn = PANEL_X_BTN + 78;

	// Row 1: -10 (left)  +10 (right)
	gump.AddHTMLGump( xLeftLbl,  y, 40, 20, false, false, "<basefont color=#ffffff>-10</basefont>" );
	gump.AddButton( xLeftBtn, y-2, 0x0FAE, 0x0FB0, 1, 0, BTN_HUE_M10 );
	gump.AddHTMLGump( xRightLbl, y, 40, 20, false, false, "<basefont color=#ffffff>+10</basefont>" );
	gump.AddButton( xRightBtn,y-2, 0x0FA5, 0x0FA7, 1, 0, BTN_HUE_P10 );
	y += CTRL_GAP;

	// Row 2: +1  (left)  -1  (right)
	gump.AddHTMLGump( xLeftLbl,  y, 40, 20, false, false, "<basefont color=#ffffff>+1</basefont>" );
	gump.AddButton( xLeftBtn, y-2, 0x0FA5, 0x0FA7, 1, 0, BTN_HUE_P1 );
	gump.AddHTMLGump( xRightLbl, y, 40, 20, false, false, "<basefont color=#ffffff>-1</basefont>" );
	gump.AddButton( xRightBtn,y-2, 0x0FAE, 0x0FB0, 1, 0, BTN_HUE_M1 );
	y += CTRL_GAP;

	// Hue label + input + Set
	gump.AddHTMLGump( PANEL_X_LABEL, y, 100, 20, false, false, "<basefont color=#ffffff>Hue</basefont>" );
	gump.AddTextEntry( PANEL_X_BTN, y - 2, 64, 20, getHue(socket.currentChar ), HUE_EDIT_ID, HUE_EDIT_ID, getHue( socket.currentChar ));
	gump.AddButton( PANEL_X_BTN + 78, y - 2, 0x0FA5, 0x0FA7, 1, 0, BTN_HUE_SET );
	y += CTRL_GAP;

	DoorGump.Send( pUser );
	DoorGump.Free();
}

function showFavorites( pUser, pageIndex )
{
	var fav = readArrTag( pUser, "doorFavorites" );
	showListPage( pUser, FAV_VIEW, fav, pageIndex || 0 );
}

function showRecent( pUser, pageIndex )
{
	var rec = readArrTag( pUser, "doorRecent" );
	showListPage( pUser, REC_VIEW, rec, pageIndex || 0 );
}

function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;
	if( !pButton )
		return;

	var view    = getView( pUser );
	var pageIdx = ( pUser.GetTempTag( "getdoor" ) || 1 ) - 1;

	// Tabs
	if( pButton === BTN_TAB_CAT )
	{ 
		setView( pUser, CATALOG_VIEW );
		showDoorPage( pUser, 0 );
		return;
	}

	if( pButton === BTN_TAB_FAV )
	{ 
		setView( pUser, FAV_VIEW );
		showFavorites( pUser, 0 );
		return;
	}

	if( pButton === BTN_TAB_REC )
	{
		setView( pUser, REC_VIEW );
		showRecent( pUser, 0 );
		return;
	}

	// Paging depends on view
	if( pButton === BTN_NEXT )
	{
		if( view === CATALOG_VIEW )
		{
			showDoorPage( pUser, pageIdx + 1 );
		}
		else if( view === FAV_VIEW )
		{
			showFavorites( pUser, pageIdx + 1 );
		}
		else
		{
			showRecent( pUser,   pageIdx + 1 );
		}
		return;
	}

	if( pButton === BTN_BACK )
	{
		if( view === CATALOG_VIEW )
		{
			showDoorPage( pUser, pageIdx - 1 );
		}
		else if( view === FAV_VIEW ) 
		{
			showFavorites( pUser, pageIdx - 1 );
		}
		else
		{
			showRecent( pUser,   pageIdx - 1 );
		}
		return;
	}

	// Door placement STARTS A TARGET now (works for all views)
	var itemId = buttonToItem[pButton];
	if( itemId != null )
	{
		// Remember what to place, then ask for a location target
		pUser.SetTempTag( "doorPendingItemId", itemId );

		var msg = "Select where to place the door.";
		socket.CustomTarget( CBID_PLACE_DOOR, msg );

		// Keep gump open; just re-send current view so it stays on screen
		var view = getView( pUser );
		var pageIdx = ( pUser.GetTempTag( "getdoor" ) || 1 ) - 1;
		if( view === CATALOG_VIEW )
		{
			showDoorPage( pUser, pageIdx );
		}
		else if( view === FAV_VIEW )
		{
			showFavorites( pUser, pageIdx );
		}
		else
		{
			showRecent( pUser, pageIdx );
		}
		return;
	}

	// Star toggles
	if( pButton >= STAR_OFFSET )
	{
		var baseId = pButton - STAR_OFFSET;
		var artId  = buttonToItem[baseId];
		if( artId != null )
		{
			toggleFavorite( pUser, artId );
		}

		// refresh current view in place
		if( view === CATALOG_VIEW )
		{
			showDoorPage( pUser, pageIdx );
		}
		else if( view === FAV_VIEW )
		{
			showFavorites( pUser, pageIdx );
		}
		else
		{
			showRecent( pUser, pageIdx );
		}
		return;
	}

	if( pButton === BTN_UNDO_LAST )
	{
		var ok = performUndoLast( pUser );
		if( !ok )
		{
			socket.SysMessage( "Nothing to undo." );
		}
		else
		{
			socket.SysMessage( "Undid last placed door." );
		}
		// Repaint current view
		var view = getView( pUser );
		var pageIdx = (socket.currentChar.GetTempTag( "getdoor" ) || 1 ) - 1;
		if( view === CATALOG_VIEW )
		{
			showDoorPage( pUser, pageIdx );
		}
		else if( view === FAV_VIEW )
		{
			showFavorites( pUser, pageIdx );
		}
		else
		{
			showRecent( pUser, pageIdx );
		}
		return;
	}

	if( pButton === BTN_REPEAT_LAST )
	{
		var lastId = parseInt( pUser.GetTempTag( "doorLastItemId" ) || "0", 10 );
		if( !lastId )
		{ 
			socket.SysMessage( "No last door to repeat." );
			return;
		}

		pUser.SetTempTag( "doorPendingItemId", String( lastId ));
		socket.CustomTarget( CBID_PLACE_DOOR, "Select where to place the door." );

		// repaint current view
		var view = getView( pUser );
		var pageIdx = ( pUser.GetTempTag( "getdoor" ) || 1 ) - 1;
		if( view === CATALOG_VIEW )
		{
			showDoorPage( pUser, pageIdx );
		}
		else if( view === FAV_VIEW )
		{
			showFavorites( pUser, pageIdx );
		}
		else
		{
			showRecent( pUser, pageIdx );
		}
		return;
	}

	if( pButton === BTN_TOGGLE_PAINT )
	{
		var on = ( pUser.GetTempTag( "doorPaintMode" )|0 ) === 1 ? 0 : 1;

		pUser.SetTempTag( "doorPaintMode", on );
		socket.SysMessage( "Paint mode " + ( on ? "enabled" : "disabled") + "." );

		var view = getView( pUser );
		var pageIdx = ( pUser.GetTempTag( "getdoor" ) || 1 ) - 1;
		if( view === CATALOG_VIEW )
		{
			showDoorPage( pUser, pageIdx );
		}
		else if( view === FAV_VIEW )
		{
			showFavorites( pUser, pageIdx );
		}
		else
		{
			showRecent( pUser, pageIdx );
		}
		return;
	}

	// --- Link mode ---
	if( pButton === BTN_TOGGLE_LINK )
	{
		var link = ( pUser.GetTag( "doorLinkMode" )|0 ) === 1 ? 0 : 1;
		pUser.SetTag( "doorLinkMode", link );
		// Turning link ON cancels unlink mode, and vice versa
		if( link ) 
		{ 
			pUser.SetTag( "doorUnlinkMode", 0 ); 
			socket.clickX = null; socket.tempObj = null;
		    socket.CustomTarget( CBID_LINK, GetDictionaryEntry( 8898, socket.language ));// Which two doors do you want to link? (1/2)
		}
		else 
		{
			socket.clickX = null; socket.tempObj = null;
		}

		var view = getView( pUser );
		var pageIdx = ( pUser.GetTempTag( "getdoor" ) || 1 ) - 1;
		if( view === CATALOG_VIEW )
		{
			showDoorPage( pUser, pageIdx );
		}
		else if( view === FAV_VIEW )
		{
			showFavorites( pUser, pageIdx );
		}
		else
		{
			showRecent( pUser, pageIdx );
		}
		return;
	}

	// --- Unlink mode (optional) ---
	if( pButton === BTN_TOGGLE_UNLINK )
	{
		var unlink = ( pUser.GetTag( "doorUnlinkMode" )|0 ) === 1 ? 0 : 1;
		pUser.SetTag("doorUnlinkMode", unlink );

		if( unlink )
		{ 
			pUser.SetTag( "doorLinkMode", 0 );
			socket.clickX = null; socket.tempObj = null;
		    socket.CustomTarget( CBID_UNLINK, GetDictionaryEntry( 8901, socket.language )); // Unlink which two doors? (1/2)
		}
		else
		{
			socket.clickX = null;
			socket.tempObj = null;
		}

		var view = getView( pUser );
		var pageIdx = ( pUser.GetTempTag( "getdoor" ) || 1 ) - 1;
		if( view === CATALOG_VIEW )
		{
			showDoorPage( pUser, pageIdx );
		}
		else if( view === FAV_VIEW )
		{
			showFavorites( pUser, pageIdx );
		}
		else
		{
			showRecent( pUser, pageIdx );
		}
		return;
	}

	if( pButton === BTN_TOGGLE_LINK_ADD )
	{
		var linkon = ( pUser.GetTag( "doorLinkOnAdd" )|0 ) === 1 ? 0 : 1;
		pUser.SetTag( "doorLinkOnAdd", linkon );

		if( !linkon )
		{
			pUser.SetTag( "doorLinkPendingSer", null ); // clear any half pair
			socket.SysMessage( "Auto-link " + ( linkon ? "enabled" : "disabled") + "." );
		}

		var view = getView( pUser );
		var pageIdx = ( pUser.GetTempTag( "getdoor" ) || 1 ) - 1;
		if( view === CATALOG_VIEW )
		{
			showDoorPage( pUser, pageIdx );
		}
		else if( view === FAV_VIEW )
		{
			showFavorites( pUser, pageIdx );
		}
		else
		{
			showRecent( pUser, pageIdx );
		}
		return;
		}

	// ---- Hue: Set from text entry ----
	if( pButton == BTN_HUE_SET ) 
	{
		var raw = ( gumpData && gumpData.getEdit ) ? gumpData.getEdit( 0 ) : null;
		var hv  = parseHue( raw );
		setHue( pUser, hv );
		socket.SysMessage( "Hue set to " + fmtHue( hv ) + "." );

		// repaint current view
		var view = getView( pUser );
		var pageIdx = ( pUser.GetTempTag( "getdoor" ) || 1 ) - 1;
		if( view === CATALOG_VIEW )
		{
			showDoorPage( pUser, pageIdx );
		}
		else if( view === FAV_VIEW )
		{
			showFavorites( pUser, pageIdx );
		}
		else
		{
			showRecent( pUser, pageIdx );
		}
		return;
	}

	// ---- Hue: quick adjustments ----
	if( pButton === BTN_HUE_M10 || pButton === BTN_HUE_M1 || pButton === BTN_HUE_P1 || pButton === BTN_HUE_P10 )
	{
		var cur = getHue( pUser );
		if( pButton === BTN_HUE_M10 )
		{
			cur -= 10;
		}

		if( pButton === BTN_HUE_M1 )
		{
			cur -= 1;
		}

		if( pButton === BTN_HUE_P1 )
		{
			cur += 1;
		}

		if( pButton === BTN_HUE_P10 )
		{
			cur += 10;
		}

		if( cur < 0 ) 
		{
			cur = 0;
		}

		if( cur > 0xFFFF )
		{
			cur = 0xFFFF;
		}
		setHue( pUser, cur );
		socket.SysMessage( "Hue set to " + fmtHue( cur ) + "." );
		var view = getView( pUser );
		var pageIdx = ( pUser.GetTempTag( "getdoor" ) || 1 ) - 1;
		if( view === CATALOG_VIEW )
		{
			showDoorPage( pUser, pageIdx );
		}
		else if( view === FAV_VIEW )
		{
			showFavorites( pUser, pageIdx );
		}
		else
		{
			showRecent( pUser, pageIdx );
		}
		return;
	}

	// ---- Hue: pick from an object ----
	if( pButton === BTN_HUE_PICK )
	{
		socket.CustomTarget( CBID_PICK_HUE, "Select an item/char to copy hue from." );
		// keep gump open
		var view = getView( pUser );
		var pageIdx = ( pUser.GetTempTag( "getdoor" ) || 1 ) - 1;
		if( view === CATALOG_VIEW )
		{
			showDoorPage( pUser, pageIdx );
		}
		else if( view === FAV_VIEW )
		{
			showFavorites( pUser, pageIdx );
		}
		else
		{
			showRecent( pUser, pageIdx );
		}
		return;
	}

	// Fallback
	if( view === CATALOG_VIEW )
	{
		showDoorPage( pUser, pageIdx );
	}
	else if( view === FAV_VIEW )
	{
		showFavorites( pUser, pageIdx );
	}
	else
	{
		showRecent( pUser, pageIdx );
	}
}

// Target result handler for placing a door
function onCallback0( socket, ourObj )
{
	var pUser   = socket.currentChar;
	var pending = pUser.GetTempTag( "doorPendingItemId" );
	if( !pending )
	{ 
		socket.SysMessage( "No door is pending placement." );
		return;
	}

	var itemId = parseInt( pending, 10 ) || 0;
	if( !itemId ) 
	{
		socket.SysMessage( "Invalid pending door ID." );
		return;
	}

	// Resolve where to place: ground coords from packet OR object coords
	var loc = getTargetLocation( socket, pUser, ourObj || socket.tempObj2 );

	// Spawn a blank door (not in pack), then move it to the target
	var doorName = "Door";
	var hue      = getHue( pUser );
	var itm = CreateBlankItem( socket, pUser, 1, doorName, itemId, hue, "ITEM", false );
	if( !ValidateObject( itm ))
	{
		socket.SysMessage( "Failed to create the door." );
		return;
	}

	// Move it and refresh
	// Teleport sets xyz/world; instanceID we set explicitly (if your Teleport doesn't)
	itm.Teleport( loc.x, loc.y, loc.z, loc.world );
	itm.instanceID = loc.inst;
	itm.Refresh();

	// --- Auto-link on Add ---
	if(( pUser.GetTag( "doorLinkOnAdd" )|0 ) === 1 )
	{
		var pendingSer = pUser.GetTag( "doorLinkPendingSer" );
		if( !pendingSer )
		{
			// This is the first door of the pair
			pUser.SetTag( "doorLinkPendingSer", String(itm.serial) );
			socket.SysMessage( "First door captured. Place the second door to link." );
		} 
		else 
		{
			// Try to link with the previously placed door
			var firstDoor = CalcItemFromSer( parseInt( pendingSer, 10 ));
			if( linkTwoDoors( firstDoor, itm ))
			{
				socket.SysMessage( "Doors linked." );
			} 
			else
			{
				socket.SysMessage( "Could not link the doors (missing or same item)." );
			}
			// Clear pending so the next placement starts a fresh pair
			pUser.SetTag( "doorLinkPendingSer", null );
		}
	}

	// Track for Undo + Recent
	pushUndo( pUser, itm.serial );
	pushRecent( pUser, itemId );

	socket.SysMessage( "Door placed at ( "+loc.x+","+loc.y+","+loc.z+")." );

	// Keep the same view/page visible
	var view = getView( pUser );
	var pageIdx = ( pUser.GetTempTag( "getdoor" ) || 1 ) - 1;
	if( view === CATALOG_VIEW )
	{
		showDoorPage( pUser, pageIdx );
	}
	else if( view === FAV_VIEW )
	{
		showFavorites( pUser, pageIdx );
	}
	else
	{
		showRecent( pUser, pageIdx );
	}

	pUser.SetTempTag( "doorLastItemId", String( itemId ));

	if(( pUser.GetTempTag( "doorPaintMode")|0) === 1 )
	{
		// immediately ask for another placement of the SAME pending item
		socket.CustomTarget( 0, "Select where to place the door (paint mode)." );
	}
	else
	{
		// clear pending (optional); Repeat Last still uses doorLastItemId
		pUser.SetTempTag( "doorPendingItemId", null );
	}
}

function onCallback1( socket, ourObj )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( ourObj ))
	{
		socket.SysMessage( "No valid target." );
		return;
	}

	// Items and characters both expose .colour
	var hv = parseHue( ourObj.colour );
	setHue( pUser, hv );
	socket.SysMessage( "Copied hue " + fmtHue(hv) + "." );

	// repaint
	var view = getView(pUser);
	var pageIdx = ( pUser.GetTempTag( "getdoor" ) || 1 ) - 1;
	if( view === CATALOG_VIEW )
	{
		showDoorPage( pUser, pageIdx );
	}
	else if( view === FAV_VIEW )
	{
		showFavorites( pUser, pageIdx );
	}
	else
	{
		showRecent( pUser, pageIdx );
	}
}

// ===== Link two doors (mode-friendly) =====
function onCallback2( pSock, myTarget )
{
	var pUser = pSock.currentChar;

	// Only handle when Link mode is actually ON (prevents accidental triggers)
	if(( pUser.GetTag( "doorLinkMode" )|0 ) !== 1 )
	{ 
		pSock.clickX = null;
		pSock.tempObj = null;
		return;
	}

	if( !pSock.GetWord( 1 ) && ValidateObject( myTarget ) && myTarget.isItem )
	{
		if( pSock.clickX != 1 )
		{
			// First target
			pSock.tempObj = myTarget;
			pSock.clickX  = 1;
			pSock.CustomTarget( 13, GetDictionaryEntry( 8899, pSock.language )); // Which two doors do you want to link? (2/2)
			return;
		}

		// Second target
		var Door1 = pSock.tempObj;
		var Door2 = myTarget;
		if( !ValidateObject( Door1 ) || !ValidateObject( Door2 ))
		{
			pUser.SysMessage(GetDictionaryEntry( 8894, pSock.language )); // You need to target a dynamic item.
		}
		else if( Door1.serial === Door2.serial )
		{
			pUser.SysMessage( "Select two different doors." );
		}
		else
		{
			// Link tags both ways
			Door1.SetTag( "linked", true );
			Door1.SetTag( "linkSer1", Door2.GetSerial( 1 ));
			Door1.SetTag( "linkSer2", Door2.GetSerial( 2 ));
			Door1.SetTag( "linkSer3", Door2.GetSerial( 3 ));
			Door1.SetTag( "linkSer4", Door2.GetSerial( 4 ));

			Door2.SetTag( "linked", true);
			Door2.SetTag( "linkSer1", Door1.GetSerial( 1 ));
			Door2.SetTag( "linkSer2", Door1.GetSerial( 2 ));
			Door2.SetTag( "linkSer3", Door1.GetSerial( 3 ));
			Door2.SetTag( "linkSer4", Door1.GetSerial( 4 ));

			pUser.SysMessage(GetDictionaryEntry(8900, pSock.language)); // The two doors have been linked.
		}

		// For continuous linking while mode is ON, reset for the next pair:
		pSock.clickX = null; pSock.tempObj = null;
		if(( pUser.GetTag( "doorLinkMode" )|0 ) === 1 )
		{
			pSock.CustomTarget(13, GetDictionaryEntry( 8898, pSock.language )); // (1/2)
		}
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8894, pSock.language )); // Need a dynamic item
	}
}

// ===== Unlink two doors (mode-friendly) =====
function onCallback3( pSock, myTarget )
{
	var pUser = pSock.currentChar;
	if(( pUser.GetTag( "doorUnlinkMode" )|0 ) !== 1 ) 
	{ 
		pSock.clickX = null;
		pSock.tempObj = null;
		return;
	}

	if( !pSock.GetWord(1) && ValidateObject( myTarget ) && myTarget.isItem )
	{
		if( pSock.clickX != 1 )
		{
			pSock.tempObj = myTarget;
			pSock.clickX  = 1;
			pSock.CustomTarget( 14, GetDictionaryEntry( 8902, pSock.language )); // (2/2)
			return;
		}

		var Door1 = pSock.tempObj;
		var Door2 = myTarget;

		if( ValidateObject( Door1 ))
		{
			Door1.SetTag( "linked", null );
			Door1.SetTag( "linkSer1", null );
			Door1.SetTag( "linkSer2", null );
			Door1.SetTag( "linkSer3", null );
			Door1.SetTag( "linkSer4", null );
		}
		if( ValidateObject( Door2 ))
		{
			Door2.SetTag( "linked", null );
			Door2.SetTag( "linkSer1", null );
			Door2.SetTag( "linkSer2", null );
			Door2.SetTag( "linkSer3", null );
			Door2.SetTag( "linkSer4", null );
		}

		pUser.SysMessage( GetDictionaryEntry( 8903, pSock.language )); // The two doors have been unlinked.

		// Keep mode active for next pair
		pSock.clickX = null; pSock.tempObj = null;
		if(( pUser.GetTag( "doorUnlinkMode" )|0 ) === 1 )
		{
			pSock.CustomTarget( 14, GetDictionaryEntry( 8901, pSock.language )); // (1/2)
		}
	}	
	else
	{
		pUser.SysMessage(GetDictionaryEntry( 8894, pSock.language )); // Need a dynamic item
	}
}

// =================== Favorites / Recent storage ===================
function readArrTag( pUser, tagName )
{
	var raw = pUser.GetTempTag(tagName);
	if(!raw || raw === "null" || raw === "undefined") return [];
	try {
		var arr = JSON.parse(raw);
		return (arr && arr.splice) ? arr : [];
	} catch (e) { return []; }
}
function writeArrTag( pUser, tagName, arr )
{
	try { pUser.SetTempTag(tagName, JSON.stringify(arr || [])); } catch(e) {}
}

function isFavorite( pUser, artId )
{
	var fav = readArrTag(pUser, "doorFavorites");
	for(var i=0;i<fav.length;i++) if (fav[i] === artId) return true;
	return false;
}
function toggleFavorite( pUser, artId ) 
{
	var fav = readArrTag(pUser, "doorFavorites");
	var i = -1;
	for(var k=0;k<fav.length;k++) if (fav[k] === artId) { i = k; break; }
	if(i >= 0) fav.splice(i,1); else fav.push(artId);
	writeArrTag(pUser, "doorFavorites", fav);
}

function pushRecent( pUser, artId )
{
	var rec = readArrTag(pUser, "doorRecent");
	// remove if exists
	for(var k=rec.length-1;k>=0;k--) if(rec[k] === artId) rec.splice(k,1);
	rec.unshift(artId);
	while (rec.length > RECENT_MAX) rec.pop();
	writeArrTag(pUser, "doorRecent", rec);
}

// View state
function getView( pUser )
{
	return pUser.GetTempTag( "doorView" ) || CATALOG_VIEW;
}
function setView( pUser, view )
{
	pUser.SetTempTag( "doorView", view || CATALOG_VIEW );
}

function readUndo( pUser )
{
	var raw = pUser.GetTempTag( "doorUndoStack" );
	if( !raw || raw === "null" || raw === "undefined" )
		return [];

	try
	{
		var arr = JSON.parse( raw );
		return( arr && arr.splice ) ? arr : [];
	}
	catch (e)
	{ 
		return [];
	}
}
function writeUndo( pUser, arr )
{
	try
	{
		pUser.SetTempTag( "doorUndoStack", JSON.stringify( arr || [] ));
	}
	catch( e )
	{}
}
function pushUndo( pUser, serial )
{
	var st = readUndo( pUser );
	st.push( serial );
	while( st.length > UNDO_MAX ) st.shift(); // keep the newest UNDO_MAX
	writeUndo( pUser, st );
}
function performUndoLast( pUser )
{
	var st = readUndo( pUser );
	if( !st.length )
		return false;

	var ser = st.pop();
	writeUndo( pUser, st );

	var itm = CalcItemFromSer( ser );
	if( ValidateObject( itm ))
	{
		itm.Delete(); // deletes the actual item
		return true;
	}
	// If the item no longer exists, we still consider it undone
	return true;
}

	// Returns {x,y,z,world,inst, fromGround:true/false}
function getTargetLocation( socket, pUser, fallbackObj )
{
	// When player clicks ground, these fields are filled in the target packet
	// (same offsets you showed: 11, 13, 16)
	if( socket.GetWord( 1 ))
	{ // non-zero -> ground/land tile target present
		var x = socket.GetWord( 11 );
		var y = socket.GetWord( 13 );
		var z = socket.GetSByte( 16 );
		return { x:x, y:y, z:z, world:pUser.worldnumber, inst:pUser.instanceID, fromGround:true };
	}

	// Otherwise, if they clicked an object, use its coords
	if( ValidateObject( fallbackObj ))
	{
		return { x: fallbackObj.x, y: fallbackObj.y, z: fallbackObj.z, world: fallbackObj.worldnumber, inst: fallbackObj.instanceID, fromGround:false};
	}

	// Fallback: player’s feet
	return { x: pUser.x, y: pUser.y, z: pUser.z, world: pUser.worldnumber, inst: pUser.instanceID, fromGround:false };
}

function parseHue( str )
{
	if(str === 0 || str === "0" )
		return 0;
	if( !str && str !== 0 )
		return 0;

	var s = String(str).replace( /\s+/g, "" );
	var n;
	if(s.length > 2 && ( s.substr( 0,2 ) === "0x" || s.substr( 0,2 ) === "0X" ))
	{
		n = parseInt( s, 16 );
	}
	else
	{
		n = parseInt( s, 10 );
	}

	if( !isFinite( n ) || n < 0 )
		n = 0;

	if( n > 0xFFFF )
		n = 0xFFFF;

	return n|0;
}
function fmtHue( n )
{
  n = parseHue( n) ;
  var hex = ( n>>>0 ).toString( 16 ).toUpperCase();
  while( hex.length < 4 ) hex = "0" + hex;   // pad to 4
  return "0x" + hex;
}
function getHue( pUser )
{
	return parseHue( pUser.GetTempTag( "doorHue" ) );
}
function setHue( pUser, value )
{
	pUser.SetTempTag( "doorHue", String(parseHue( value )));
}

function linkTwoDoors( doorA, doorB )
{
	if( !ValidateObject( doorA ) || !ValidateObject( doorB ))
		return false;

	if( doorA.serial === doorB.serial )
		return false;

	doorA.SetTag( "linked", true );
	doorA.SetTag( "linkSer1", doorB.GetSerial( 1 ));
	doorA.SetTag( "linkSer2", doorB.GetSerial( 2 ));
	doorA.SetTag( "linkSer3", doorB.GetSerial( 3 ));
	doorA.SetTag( "linkSer4", doorB.GetSerial( 4 ));

	doorB.SetTag( "linked", true );
	doorB.SetTag( "linkSer1", doorA.GetSerial( 1 ));
	doorB.SetTag( "linkSer2", doorA.GetSerial( 2 ));
	doorB.SetTag( "linkSer3", doorA.GetSerial( 3 ));
	doorB.SetTag( "linkSer4", doorA.GetSerial( 4 ));
	return true;
}