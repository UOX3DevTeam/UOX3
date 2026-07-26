/// <reference path="../../../definitions.d.ts" />
// @ts-check

var vetDyeTubMaterialScriptId = 2506;
var vetDyeTubVetRewardScriptId = 7602;

var vetDyeTubHueGroupsSpecial = [
	[ "Violet", [ 1230, 1231, 1232, 1233, 1234, 1235 ] ],
	[ "Tan", [ 1501, 1502, 1503, 1504, 1505, 1506, 1507, 1508 ] ],
	[ "Brown", [ 2012, 2013, 2014, 2015, 2016, 2017 ] ],
	[ "Dark Blue", [ 1303, 1304, 1305, 1306, 1307, 1308 ] ],
	[ "Forest Green", [ 1420, 1421, 1422, 1423, 1424, 1425, 1426 ] ],
	[ "Pink", [ 1619, 1620, 1621, 1622, 1623, 1624, 1625, 1626 ] ],
	[ "Red", [ 1640, 1641, 1642, 1643, 1644 ] ],
	[ "Olive", [ 2001, 2002, 2003, 2004, 2005 ] ]
];

var vetDyeTubHueGroupsLeather = [
	[ "Dull Copper", [ 2419, 2420, 2421, 2422, 2423, 2424 ] ],
	[ "Shadow Iron", [ 2406, 2407, 2408, 2409, 2410, 2411, 2412 ] ],
	[ "Copper", [ 2413, 2414, 2415, 2416, 2417, 2418 ] ],
	[ "Bronze", [ 2414, 2415, 2416, 2417, 2418 ] ],
	[ "Golden", [ 2213, 2214, 2215, 2216, 2217, 2218 ] ],
	[ "Agapite", [ 2425, 2426, 2427, 2428, 2429, 2430 ] ],
	[ "Verite", [ 2207, 2208, 2209, 2210, 2211, 2212 ] ],
	[ "Valorite", [ 2219, 2220, 2221, 2222, 2223, 2224 ] ],
	[ "Reds", [ 2113, 2114, 2115, 2116, 2117, 2118 ] ],
	[ "Blues", [ 2119, 2120, 2121, 2122, 2123, 2124 ] ],
	[ "Greens", [ 2126, 2127, 2128, 2129, 2130 ] ],
	[ "Yellows", [ 2213, 2214, 2215, 2216, 2217, 2218 ] ]
];

/** @type { ( pUser: Character, iUsed: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ))
	{
		return false;
	}

	var socket = pUser.socket;
	if( socket == null )
	{
		return false;
	}

	var usableByAccount = TriggerEvent( vetDyeTubVetRewardScriptId, "CanUseVeteranRewardItem", pUser, iUsed );
	if( usableByAccount === false )
	{
		return false;
	}

	if( !IsVetDyeTubUsableByPlayer( pUser, iUsed ))
	{
		socket.SysMessage( "That must be in your backpack or near you to use it." );
		return false;
	}

	if( IsVetDyeTubRedyable( iUsed ))
	{
		ShowVetDyeTubHueGump( pUser, iUsed );
		return false;
	}

	pUser.SetTempTag( "VetDyeTubSerial", iUsed.serial );
	pUser.SetTempTag( "VetDyeTubHue", GetVetDyeTubHue( iUsed ));
	pUser.SetTempTag( "VetDyeTubMode", "hardcoded" );

	return true;
}

/** @type { ( pUser: Character, dyeTub: Item ) => void } */
function ShowVetDyeTubHueGump( pUser, dyeTub )
{
	if( !ValidateObject( pUser ) || !ValidateObject( dyeTub ))
	{
		return;
	}

	var socket = pUser.socket;
	if( socket == null )
	{
		return;
	}

	var hueGroups = GetVetDyeTubHueGroups( dyeTub );
	if( hueGroups.length == 0 )
	{
		BeginVetDyeTubTarget( pUser, dyeTub, GetVetDyeTubHue( dyeTub ));
		return;
	}

	pUser.SetTempTag( "VetDyeTubSerial", dyeTub.serial );
	pUser.SetTempTag( "VetDyeTubMode", "hue" );

	var hueGump = new Gump();
	hueGump.AddPage( 0 );
	hueGump.AddBackground( 0, 0, 450, 450, 5054 );
	hueGump.AddBackground( 10, 10, 430, 430, 3000 );
	hueGump.AddHTMLGump( 20, 30, 400, 25, false, false, GetVetDyeTubTitle( dyeTub ) );

	// Build one page per hue group. Each page redraws the same group list
	// on the left, then shows that group's hues on the right.
	for( var groupIndex = 0; groupIndex < hueGroups.length; groupIndex++ )
	{
		var pageNumber = groupIndex + 1;

		hueGump.AddPage( pageNumber );

		hueGump.AddBackground( 0, 0, 450, 450, 5054 );
		hueGump.AddBackground( 10, 10, 430, 430, 3000 );
		hueGump.AddHTMLGump( 20, 30, 400, 25, false, false, GetVetDyeTubTitle( dyeTub ) );

		DrawVetDyeTubHueGroupButtons( hueGump, hueGroups, groupIndex );
		DrawVetDyeTubHueChoices( hueGump, hueGroups[groupIndex] );

		hueGump.AddButton( 20, 400, 4005, 4007, 1, 0, 1 );
		hueGump.AddHTMLGump( 55, 400, 200, 25, false, false, "Okay" );

		if( VetDyeTubSupportsDefaultHue( dyeTub ))
		{
			hueGump.AddButton( 200, 400, 4005, 4007, 1, 0, 2 );
			hueGump.AddText( 235, 400, 0, "Default" );
		}
	}

	hueGump.Send( socket );
	hueGump.Free();
}

/** @type { ( hueGump: Gump, hueGroups: any[], selectedGroupIndex: number ) => void } */
function DrawVetDyeTubHueGroupButtons( hueGump, hueGroups, selectedGroupIndex )
{
	for( var groupIndex = 0; groupIndex < hueGroups.length; groupIndex++ )
	{
		var buttonY = 85 + ( groupIndex * 25 );
		var pageNumber = groupIndex + 1;
		var textHue = 0;

		if( groupIndex == selectedGroupIndex )
		{
			textHue = 1152;
		}

		hueGump.AddPageButton( 30, buttonY, 5224, 5224, pageNumber );
		hueGump.AddHTMLGump( 55, buttonY, 200, 25, false, false, hueGroups[groupIndex][0] );
	}
}

/** @type { ( hueGump: Gump, hueGroup: any[] ) => void } */
function DrawVetDyeTubHueChoices( hueGump, hueGroup )
{
	if( !hueGroup || !hueGroup[1] )
	{
		return;
	}

	var hues = hueGroup[1];

	for( var hueIndex = 0; hueIndex < hues.length; hueIndex++ )
	{
		var hue = parseInt( hues[hueIndex], 10 );
		if( isNaN( hue ) || hue < 0 )
		{
			continue;
		}

		var buttonY = 90 + ( hueIndex * 25 );

		hueGump.AddButton( 260, buttonY, 210, 211, 1, 0, 10000 + hue );
		hueGump.AddText( 278, buttonY, hue - 1, "*****" );
	}
}

/** @type { ( socket: Socket, button: number, gumpData: GumpData ) => void } */
function onGumpPress( socket, button, gumpData )
{
	if( socket == null )
	{
		return;
	}

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
	{
		return;
	}

	var dyeTubSerial = parseInt( pUser.GetTempTag( "VetDyeTubSerial" ), 10 );
	if( isNaN( dyeTubSerial ) || dyeTubSerial <= 0 )
	{
		return;
	}

	var dyeTub = CalcItemFromSer( dyeTubSerial );
	if( !ValidateObject( dyeTub ))
	{
		pUser.SetTempTag( "VetDyeTubSerial", null );
		return;
	}

	if( button == 0 )
	{
		pUser.SetTempTag( "VetDyeTubSerial", null );
		pUser.SetTempTag( "VetDyeTubMode", null );
		return;
	}

	if( button == 1 )
	{
		BeginVetDyeTubTarget( pUser, dyeTub, GetVetDyeTubHue( dyeTub ));
		return;
	}

	if( button == 2 && VetDyeTubSupportsDefaultHue( dyeTub ))
	{
		BeginVetDyeTubTarget( pUser, dyeTub, 0 );
		return;
	}

	if( button >= 10000 )
	{
		var hue = button - 10000;
		if( hue < 0 )
		{
			hue = 0;
		}

		dyeTub.SetTag( "dyeTubHue", hue );
		dyeTub.color = hue;
		BeginVetDyeTubTarget( pUser, dyeTub, hue );
		return;
	}
}

/** @type { ( pUser: Character, dyeTub: Item, targItem: Item ) => boolean } */
function onDyeTarget( pUser, dyeTub, targItem )
{
	if( !ValidateObject( pUser ) || !ValidateObject( dyeTub ) || !ValidateObject( targItem ))
	{
		return false;
	}

	var socket = pUser.socket;
	if( socket == null )
	{
		return false;
	}

	var usableByAccount = TriggerEvent( vetDyeTubVetRewardScriptId, "CanUseVeteranRewardItem", pUser, dyeTub );
	if( usableByAccount === false )
	{
		return false;
	}

	if( !IsVetDyeTubUsableByPlayer( pUser, dyeTub ))
	{
		socket.SysMessage( "That must be in your backpack or near you to use it." );
		return false;
	}

	var hue = GetVetDyeTubHue( dyeTub );
	if( !ApplyVetDyeTubToTarget( pUser, dyeTub, targItem, hue ))
	{
		return false;
	}

	return false;
}

/** @type { ( pUser: Character, dyeTub: Item, hue: number|string ) => void } */
function BeginVetDyeTubTarget( pUser, dyeTub, hue )
{
	if( !ValidateObject( pUser ) || !ValidateObject( dyeTub ))
	{
		return;
	}

	var socket = pUser.socket;
	if( socket == null )
	{
		return;
	}

	hue = parseInt( hue, 10 );
	if( isNaN( hue ) || hue < 0 )
	{
		hue = 0;
	}

	pUser.SetTempTag( "VetDyeTubSerial", dyeTub.serial );
	pUser.SetTempTag( "VetDyeTubHue", hue );
	pUser.SetTempTag( "VetDyeTubMode", "target" );

	socket.CustomTarget( 0, GetVetDyeTubTargetMessage( dyeTub ) );
}

/** @type { ( socket: Socket, target: Character | Item | null ) => void } */
function onCallback0( socket, target )
{
	if( socket == null )
	{
		return;
	}

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
	{
		return;
	}

	var dyeTubSerial = parseInt( pUser.GetTempTag( "VetDyeTubSerial" ), 10 );
	var hue = parseInt( pUser.GetTempTag( "VetDyeTubHue" ), 10 );
	pUser.SetTempTag( "VetDyeTubSerial", null );
	pUser.SetTempTag( "VetDyeTubHue", null );
	pUser.SetTempTag( "VetDyeTubMode", null );

	if( isNaN( dyeTubSerial ) || dyeTubSerial <= 0 )
	{
		socket.SysMessage( "Unable to find the dye tub." );
		return;
	}

	if( isNaN( hue ) || hue < 0 )
	{
		hue = 0;
	}

	var dyeTub = CalcItemFromSer( dyeTubSerial );
	if( !ValidateObject( dyeTub ))
	{
		socket.SysMessage( "Unable to find the dye tub." );
		return;
	}

	if( !ValidateObject( target ) || !target.isItem )
	{
		socket.SysMessage( GetVetDyeTubFailMessage( dyeTub ) );
		return;
	}

	var itemTarget = target;
	ApplyVetDyeTubToTarget( pUser, dyeTub, itemTarget, hue );
}

/** @type { ( pUser: Character, dyeTub: Item, itemTarget: Item, hue: number|string ) => boolean } */
function ApplyVetDyeTubToTarget( pUser, dyeTub, itemTarget, hue )
{
	if( !ValidateObject( pUser ) || !ValidateObject( dyeTub ) || !ValidateObject( itemTarget ))
	{
		return false;
	}

	var socket = pUser.socket;
	if( socket == null )
	{
		return false;
	}

	hue = parseInt( hue, 10 );
	if( isNaN( hue ) || hue < 0 )
	{
		hue = GetVetDyeTubHue( dyeTub );
	}

	if( !CanVetDyeTubDyeTarget( pUser, dyeTub, itemTarget ))
	{
		return false;
	}

	itemTarget.color = hue;
	itemTarget.Refresh();
	pUser.SoundEffect( 0x23E, true );
	socket.SysMessage( "You dye the item." );

	return true;
}

/** @type { ( pUser: Character, dyeTub: Item, itemTarget: Item ) => boolean } */
function CanVetDyeTubDyeTarget( pUser, dyeTub, itemTarget )
{
	var socket = pUser.socket;
	if( socket == null )
	{
		return false;
	}

	if( itemTarget.GetTag( "QuestItem" ))
	{
		socket.SysMessage( "You may not dye toggled quest items." );
		return false;
	}

	if( !IsVetDyeTubTargetInRange( pUser, dyeTub, itemTarget ))
	{
		socket.SysMessage( "That is too far away." );
		return false;
	}

	if( IsVetDyeTubTargetWorn( itemTarget ))
	{
		socket.SysMessage( "Cannot dye clothing that is being worn." );
		return false;
	}

	var dyeType = GetVetDyeTubType( dyeTub );
	if( dyeType == "furniture" )
	{
		if( IsVetDyeTubFurnitureTarget( itemTarget ))
		{
			return true;
		}
	}
	else if( dyeType == "leather" )
	{
		if( IsVetDyeTubLeatherTarget( itemTarget ))
		{
			return true;
		}
	}
	else if( dyeType == "runebook" )
	{
		if( IsVetDyeTubRunebookTarget( itemTarget ))
		{
			return true;
		}
	}
	else if( dyeType == "statuette" )
	{
		if( IsVetDyeTubStatuetteTarget( itemTarget ))
		{
			return true;
		}
	}
	else
	{
		if( IsVetDyeTubNormalDyableTarget( itemTarget ))
		{
			return true;
		}
	}

	socket.SysMessage( GetVetDyeTubFailMessage( dyeTub ) );
	return false;
}

/** @type { ( pUser: Character, dyeTub: Item, itemTarget: Item ) => boolean } */
function IsVetDyeTubTargetInRange( pUser, dyeTub, itemTarget )
{
	if( !IsVetDyeTubUsableByPlayer( pUser, dyeTub ))
	{
		return false;
	}

	var packOwner = GetPackOwner( itemTarget, 0 );
	if( ValidateObject( packOwner ))
	{
		return ( packOwner.serial == pUser.serial );
	}

	return itemTarget.InRange( pUser, 1 );
}

/** @type { ( pUser: Character, dyeTub: Item ) => boolean } */
function IsVetDyeTubUsableByPlayer( pUser, dyeTub )
{
	if( !ValidateObject( pUser ) || !ValidateObject( dyeTub ))
	{
		return false;
	}

	var tubOwner = GetPackOwner( dyeTub, 0 );
	if( ValidateObject( tubOwner ) && tubOwner.serial == pUser.serial )
	{
		return true;
	}

	return dyeTub.InRange( pUser, 1 );
}

/** @type { ( itemTarget: Item ) => boolean } */
function IsVetDyeTubTargetWorn( itemTarget )
{
	if( !ValidateObject( itemTarget ))
	{
		return false;
	}

	if( ValidateObject( itemTarget.container ) && itemTarget.container.isChar )
	{
		return true;
	}

	return false;
}

/** @type { ( itemTarget: Item ) => boolean } */
function IsVetDyeTubNormalDyableTarget( itemTarget )
{
	if( !ValidateObject( itemTarget ))
	{
		return false;
	}

	if( itemTarget.isDyeable )
	{
		return true;
	}

	var materialType = GetVetDyeTubMaterialType( itemTarget );
	return ( materialType == "cloth" );
}

/** @type { ( itemTarget: Item ) => boolean } */
function IsVetDyeTubLeatherTarget( itemTarget )
{
	return ( GetVetDyeTubMaterialType( itemTarget ) == "leather" || itemTarget.GetTag( "leatherDyeable" ));
}

/** @type { ( itemTarget: Item ) => boolean } */
function IsVetDyeTubFurnitureTarget( itemTarget )
{
	if( itemTarget.GetTag( "furnitureDyeable" ) || itemTarget.GetTag( "dyeTubFurniture" ))
	{
		return true;
	}

	var materialType = GetVetDyeTubMaterialType( itemTarget );
	return ( materialType == "wood" && itemTarget.movable == 2 );
}

/** @type { ( itemTarget: Item ) => boolean } */
function IsVetDyeTubRunebookTarget( itemTarget )
{
	if( itemTarget.GetTag( "runebookDyeable" ) || itemTarget.GetTag( "dyeTubRunebook" ))
	{
		return true;
	}

	var sectionID = String( itemTarget.sectionID || "" ).toLowerCase();
	var itemName = String( itemTarget.name || "" ).toLowerCase();
	return ( sectionID.indexOf( "runebook" ) >= 0 || sectionID.indexOf( "recallrune" ) >= 0 || itemName.indexOf( "runebook" ) >= 0 || itemName.indexOf( "recall rune" ) >= 0 );
}

/** @type { ( itemTarget: Item ) => boolean } */
function IsVetDyeTubStatuetteTarget( itemTarget )
{
	if( itemTarget.GetTag( "statuetteDyeable" ) || itemTarget.GetTag( "monsterStatuette" ) || itemTarget.GetTag( "dyeTubStatuette" ))
	{
		return true;
	}

	var sectionID = String( itemTarget.sectionID || "" ).toLowerCase();
	var itemName = String( itemTarget.name || "" ).toLowerCase();
	return ( sectionID.indexOf( "statuette" ) >= 0 || itemName.indexOf( "statuette" ) >= 0 );
}

/** @type { ( itemTarget: Item ) => string } */
function GetVetDyeTubMaterialType( itemTarget )
{
	var materialType = TriggerEvent( vetDyeTubMaterialScriptId, "GetItemMaterialType", itemTarget );
	if( materialType == null || typeof materialType == "undefined" || materialType == 0 )
	{
		return "unknown";
	}

	return String( materialType ).toLowerCase();
}

/** @type { ( dyeTub: Item ) => string } */
function GetVetDyeTubType( dyeTub )
{
	var dyeType = String( dyeTub.GetTag( "dyeTubType" ) || "dyable" ).toLowerCase();
	if( dyeType == "" || dyeType == "0" )
	{
		dyeType = "dyable";
	}

	return dyeType;
}

/** @type { ( dyeTub: Item ) => number } */
function GetVetDyeTubHue( dyeTub )
{
	var hue = parseInt( dyeTub.GetTag( "dyeTubHue" ), 10 );
	if( isNaN( hue ) || hue < 0 )
	{
		hue = parseInt( dyeTub.color, 10 );
		if( isNaN( hue ) || hue < 0 )
		{
			hue = 0;
		}
	}

	return hue;
}

/** @type { ( dyeTub: Item ) => boolean } */
function IsVetDyeTubRedyable( dyeTub )
{
	var redyable = parseInt( dyeTub.GetTag( "dyeTubRedyable" ), 10 );
	return ( redyable > 0 );
}

/** @type { ( dyeTub: Item ) => boolean } */
function VetDyeTubSupportsDefaultHue( dyeTub )
{
	var picker = String( dyeTub.GetTag( "dyeTubHuePicker" ) || "" ).toLowerCase();
	return ( picker == "leather" || picker == "metallic" );
}

/** @type { ( dyeTub: Item ) => any[] } */
function GetVetDyeTubHueGroups( dyeTub )
{
	var picker = String( dyeTub.GetTag( "dyeTubHuePicker" ) || "" ).toLowerCase();
	if( picker == "special" )
	{
		return vetDyeTubHueGroupsSpecial;
	}
	if( picker == "leather" )
	{
		return vetDyeTubHueGroupsLeather;
	}
	if( picker == "metallic" )
	{
		return GetVetDyeTubMetallicHueGroups();
	}

	return [];
}

/** @type { () => any[] } */
function GetVetDyeTubMetallicHueGroups()
{
	var groups = [];
	for( var page = 1; page < 14; page++ )
	{
		var hues = [];
		for( var row = 0; row < 12; row++ )
		{
			var hue = 2501 + (( page == 13 ) ? 6 : 0 ) + ( row + ( 12 * ( page - 1 )));
			hues.push( hue );
		}
		groups.push([ "Metallic " + page, hues ]);
	}

	return groups;
}

/** @type { ( dyeTub: Item ) => string } */
function GetVetDyeTubTitle( dyeTub )
{
	var dyeType = GetVetDyeTubType( dyeTub );
	if( dyeType == "leather" )
	{
		return "Select leather dye tub hue";
	}
	if( dyeType == "furniture" )
	{
		return "Select furniture dye tub hue";
	}
	if( dyeType == "runebook" )
	{
		return "Select runebook dye tub hue";
	}
	if( dyeType == "statuette" )
	{
		return "Select statuette dye tub hue";
	}

	return "Select dye tub hue";
}

/** @type { ( dyeTub: Item ) => string } */
function GetVetDyeTubTargetMessage( dyeTub )
{
	var dyeType = GetVetDyeTubType( dyeTub );
	if( dyeType == "furniture" )
	{
		return "Select the furniture to dye.";
	}
	if( dyeType == "leather" )
	{
		return "Select the leather item to dye.";
	}
	if( dyeType == "runebook" )
	{
		return "Target the runebook or runestone to dye.";
	}
	if( dyeType == "statuette" )
	{
		return "Target the statuette to dye.";
	}

	return "Select the clothing to dye.";
}

/** @type { ( dyeTub: Item ) => string } */
function GetVetDyeTubFailMessage( dyeTub )
{
	var dyeType = GetVetDyeTubType( dyeTub );
	if( dyeType == "furniture" )
	{
		return "That is not a piece of furniture.";
	}
	if( dyeType == "leather" )
	{
		return "You can only dye leather with this tub.";
	}
	if( dyeType == "runebook" )
	{
		return "You can only dye runestones or runebooks with this tub.";
	}
	if( dyeType == "statuette" )
	{
		return "You can only dye veteran reward statuettes with this tub.";
	}

	return "You can not dye that.";
}