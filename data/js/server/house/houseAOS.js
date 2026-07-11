/// <reference path="../../definitions.d.ts" />
// @ts-check
// AOS-style house sign menu. This script keeps AOS house UI separate from the
// classic sign gump so shard rules can choose between them at runtime.

const houseAOSScriptID = 15009;
const houseCommandScriptID = 15002;
const coOwnHousesOnSameAccountAOS = GetServerSetting( "CoOwnHousesOnSameAccount" );

var aosMulti = null;
var aosSign = null;
var aosOwnerName = "Unknown";
var aosVisitCount = 0;
var aosCoOwnerList = [];
var aosFriendList = [];
var aosGuestList = [];
var aosBanList = [];

const aosHangerIds = [ 2968, 2970, 2972, 2974, 2976, 2978 ];
const aosFoundationIds = [ 20, 189, 765, 65, 101 ];
const aosPostIds = [
	9, 29, 54, 90, 147, 169, 177, 204, 251, 257,
	263, 298, 347, 353, 424, 441, 466, 514, 553,
	600, 601, 602, 603, 660, 666, 672, 898, 970,
	974, 982, 11212, 11720, 11186, 13788, 13849,
	17190, 16796, 16733, 16663, 20758, 19214,
	39603, 39809
];

function HouseAOSCanManage( pUser, iMulti )
{
	return ( pUser.isGM || iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccountAOS && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum ));
}

function HouseAOSCanOwn( pUser, iMulti )
{
	return ( pUser.isGM || iMulti.IsOwner( pUser ) || ( coOwnHousesOnSameAccountAOS && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum ));
}

function HouseAOSCanAccessFriends( pUser, iMulti )
{
	return ( HouseAOSCanManage( pUser, iMulti ) || iMulti.IsOnFriendList( pUser ));
}

function HouseAOSStatusText( iMulti )
{
	if( iMulti.GetTag( "Grandfathered" ))
		return "<BASEFONT COLOR=#FF0000>Grandfathered - No Decay</BASEFONT>";

	var decayStages = {
		1: "Like New",
		2: "Slightly Worn",
		3: "Somewhat Worn",
		4: "Fairly Worn",
		5: "Greatly Worn",
		6: "In Danger of Collapsing"
	};

	var stage = parseInt( iMulti.GetTag( "decayStage" ), 10 );
	if( stage > 0 && decayStages[stage] )
		return "Condition: <BASEFONT COLOR=#FF0000>" + decayStages[stage] + "</BASEFONT>";

	return "";
}

function HouseAOSStoreContext( pUser, iMulti, iSign, houseOwner, visitCount )
{
	aosMulti = iMulti;
	aosSign = iSign;
	aosOwnerName = houseOwner;
	aosVisitCount = visitCount;

	if( pUser.socket != null )
	{
		pUser.socket.tempObj = iSign;
		pUser.socket.tempObj2 = iMulti;
	}
}

function HouseAOSRestoreContext( pSocket )
{
	if( pSocket == null )
		return false;

	if( ValidateObject( pSocket.tempObj2 ))
		aosMulti = pSocket.tempObj2;

	if( ValidateObject( pSocket.tempObj ))
		aosSign = pSocket.tempObj;

	if( !ValidateObject( aosMulti ) && ValidateObject( aosSign ))
		aosMulti = aosSign.multi;

	if( !ValidateObject( aosSign ) || !ValidateObject( aosMulti ))
		return false;

	return true;
}

function HouseAOSBoolTag( obj, tagName )
{
	var tagValue = obj.GetTag( tagName );
	return ( tagValue == true || tagValue == "true" || tagValue == 1 || tagValue == "1" );
}

function HouseAOSIsCustomFoundation( iMulti )
{
	return ( HouseAOSBoolTag( iMulti, "customFoundation" ) || iMulti.GetTag( "customFoundationOriginalId" ));
}

function HouseAOSLabel( text, selected )
{
	return "<BASEFONT COLOR=#" + ( selected ? "00FF00" : "FFFFFF" ) + ">" + text + "</BASEFONT>";
}

function HouseAOSDisabledLabel( text )
{
	return "<BASEFONT COLOR=#808080>" + text + "</BASEFONT>";
}

function HouseAOSAddButtonLine( houseGump, x, y, buttonID, text, enabled )
{
	if( enabled == null )
		enabled = true;

	if( enabled )
		houseGump.AddButton( x, y + 2, 4005, 4006, 1, 0, buttonID );

	houseGump.AddHTMLGump( x + 35, y, 240, 22, false, false, enabled ? text : HouseAOSDisabledLabel( text ));
}

function HouseAOSAddPageButtonLine( houseGump, x, y, pageButtonID, text, selected )
{
	houseGump.AddButton( x, y + 2, selected ? 4006 : 4005, 4007, 1, 0, pageButtonID );
	houseGump.AddHTMLGump( x + 45, y, 200, 22, false, false, HouseAOSLabel( text, selected ));
}

function HouseAOSAddMainFrame( houseGump, iSign, pageName )
{
	houseGump.AddPage( 0 );
	houseGump.AddBackground( 0, 0, 420, 440, 5054 );
	houseGump.AddTiledGump( 10, 10, 400, 100, 2624 );
	houseGump.AddCheckerTrans( 10, 10, 400, 100 );
	houseGump.AddTiledGump( 10, 120, 400, 260, 2624 );
	houseGump.AddCheckerTrans( 10, 120, 400, 260 );
	houseGump.AddTiledGump( 10, 390, 400, 40, 2624 );
	houseGump.AddCheckerTrans( 10, 390, 400, 40 );
	houseGump.AddButton( 250, 410, 4005, 4007, 1, 0, 0 );
	houseGump.AddHTMLGump( 285, 410, 100, 22, false, false, HouseAOSLabel( "CLOSE" ));
	houseGump.AddGump( 10, 10, 100 );

	if( ValidateObject( iSign ))
		houseGump.AddHTMLGump( 22, Math.max(( 72 - iSign.name.length ), 22 ), 125, 60, false, false, "<CENTER>" + iSign.name + "</CENTER>" );

	HouseAOSAddPageButtonLine( houseGump, 150, 10, 201, "Information", pageName == "info" );
	HouseAOSAddPageButtonLine( houseGump, 150, 30, 202, "Security", pageName == "security" );
	HouseAOSAddPageButtonLine( houseGump, 150, 50, 203, "Storage", pageName == "storage" );
	HouseAOSAddPageButtonLine( houseGump, 150, 70, 204, "Customize", pageName == "customize" );
	HouseAOSAddPageButtonLine( houseGump, 150, 90, 205, "Ownership", pageName == "ownership" );
}

function HouseAOSAddQuickActions( houseGump, iMulti )
{
	if( iMulti.isPublic )
	{
		HouseAOSAddButtonLine( houseGump, 10, 390, 30, HouseAOSLabel("Banish", true ));
		HouseAOSAddButtonLine( houseGump, 10, 410, 33, HouseAOSLabel("Lift a Ban", true ));
	}
	else
	{
		HouseAOSAddButtonLine( houseGump, 10, 390, 41, HouseAOSLabel("Grant Access", true ));
		HouseAOSAddButtonLine( houseGump, 10, 410, 42, HouseAOSLabel("Revoke Access", true ));
	}
}

function OpenHouseAOSGump( pUser, iMulti, iSign, houseOwner, visitCount, pageName )
{
	if( !ValidateObject( pUser ) || pUser.socket == null || !ValidateObject( iMulti ) || !ValidateObject( iSign ))
		return false;

	HouseAOSStoreContext( pUser, iMulti, iSign, houseOwner, visitCount );

	if( pageName == null || pageName == "" )
		pageName = "info";

	var pLanguage = pUser.socket.language;
	var statusText = HouseAOSStatusText( iMulti );
	var isOwner = HouseAOSCanOwn( pUser, iMulti );
	var isCoOwner = HouseAOSCanManage( pUser, iMulti );
	var isCustomizable = HouseAOSIsCustomFoundation( iMulti );
	var isPublicText = iMulti.isPublic ? "This house is open to the public." : "This house is private.";

	var houseGump = new Gump;
	HouseAOSAddMainFrame( houseGump, iSign, pageName );
	HouseAOSAddQuickActions( houseGump, iMulti );

	if( pageName == "info" )
	{
		houseGump.AddHTMLGump( 20, 130, 180, 22, false, false, HouseAOSLabel( "Owned By:" ));
		houseGump.AddText( 210, 130, 0x481, houseOwner );
		houseGump.AddHTMLGump( 20, 170, 380, 22, false, false, HouseAOSLabel( "This house is properly placed.", true ));
		houseGump.AddHTMLGump( 20, 190, 380, 22, false, false, HouseAOSLabel( "This house is of modern design.", true ));
		houseGump.AddHTMLGump( 20, 210, 380, 22, false, false, HouseAOSLabel( isCustomizable ? "This is a custom-built house." : "This is a pre-built house.", true ));
		houseGump.AddHTMLGump( 20, 230, 380, 22, false, false, HouseAOSLabel( isPublicText, true ));
		if( statusText != "" )
			houseGump.AddHTMLGump( 20, 250, 380, 22, false, false, HouseAOSLabel( statusText ));
		houseGump.AddHTMLGump( 20, 290, 200, 22, false, false, HouseAOSLabel( "Built On:", true ));
		houseGump.AddHTMLGump( 250, 290, 140, 22, false, false, HouseAOSLabel( iMulti.GetTag( "builtOn" )));
		houseGump.AddHTMLGump( 20, 310, 200, 22, false, false, HouseAOSLabel( "Last Traded:", true ));
		houseGump.AddHTMLGump( 250, 310, 140, 22, false, false, HouseAOSLabel( iMulti.GetTag( "lastTraded" )));
		houseGump.AddHTMLGump( 20, 330, 200, 22, false, false, HouseAOSLabel( "House Value", true ));
		houseGump.AddHTMLGump( 250, 330, 140, 22, false, false, HouseAOSLabel( iMulti.GetTag( "houseValue" )));
		houseGump.AddHTMLGump( 20, 360, 300, 22, false, false, HouseAOSLabel( "Number of visits this building has had:", true ));
		houseGump.AddHTMLGump( 350, 360, 40, 22, false, false, HouseAOSLabel( visitCount.toString() ));
	}
	else if( pageName == "security" )
	{
		HouseAOSAddButtonLine( houseGump, 10, 130, 10, HouseAOSLabel( "View Co-Owner List", true ));
		HouseAOSAddButtonLine( houseGump, 10, 150, 11, HouseAOSLabel( "Add a Co-Owner", isOwner ));
		HouseAOSAddButtonLine( houseGump, 10, 170, 12, HouseAOSLabel( "Remove a Co-Owner", isOwner ));
		HouseAOSAddButtonLine( houseGump, 10, 190, 13, HouseAOSLabel( "Clear Co-Owner List", isOwner ));
		HouseAOSAddButtonLine( houseGump, 10, 220, 20, HouseAOSLabel( "View Friends List", true ));
		HouseAOSAddButtonLine( houseGump, 10, 240, 21, HouseAOSLabel( "Add a Friend", isCoOwner ));
		HouseAOSAddButtonLine( houseGump, 10, 260, 22, HouseAOSLabel( "Remove a Friend", isCoOwner ));
		HouseAOSAddButtonLine( houseGump, 10, 280, 23, HouseAOSLabel( "Clear Friend List", isOwner ));

		if( iMulti.isPublic )
		{
			HouseAOSAddButtonLine( houseGump, 10, 310, 32, HouseAOSLabel( "View Ban List", true ));
			HouseAOSAddButtonLine( houseGump, 10, 330, 62, HouseAOSLabel( "Clear Ban List", isOwner ));
			HouseAOSAddButtonLine( houseGump, 210, 130, 54, HouseAOSLabel( "Change to Private", isOwner ));
			houseGump.AddHTMLGump( 245, 150, 150, 22, false, false, HouseAOSLabel( "Change to Public", true ));
		}
		else
		{
			HouseAOSAddButtonLine( houseGump, 10, 310, 40, HouseAOSLabel( "View Access List", true ));
			HouseAOSAddButtonLine( houseGump, 10, 330, 43, HouseAOSLabel( "Clear Access List", isOwner ));
			houseGump.AddHTMLGump( 245, 130, 150, 22, false, false, HouseAOSLabel( "Change to Private", true ));
			HouseAOSAddButtonLine( houseGump, 210, 150, 53, HouseAOSLabel( "Change to Public", isOwner ));
		}
	}
	else if( pageName == "storage" )
	{
		houseGump.AddHTMLGump( 10, 130, 400, 22, false, false, HouseAOSLabel( "<CENTER>HOUSE STORAGE SUMMARY</CENTER>" ));
		houseGump.AddHTMLGump( 10, 170, 300, 22, false, false, HouseAOSLabel( "Maximum Secure Storage" ));
		houseGump.AddHTMLGump( 310, 170, 80, 22, false, false, HouseAOSLabel( iMulti.maxSecureContainers.toString() ));
		houseGump.AddHTMLGump( 10, 210, 300, 22, false, false, HouseAOSLabel( "Used by Lockdowns" ));
		houseGump.AddHTMLGump( 310, 210, 80, 22, false, false, HouseAOSLabel( iMulti.lockdowns.toString() ));
		houseGump.AddHTMLGump( 10, 230, 300, 22, false, false, HouseAOSLabel( "Used by Secure Containers" ));
		houseGump.AddHTMLGump( 310, 230, 80, 22, false, false, HouseAOSLabel( iMulti.secureContainers.toString() ));
		houseGump.AddHTMLGump( 10, 250, 300, 22, false, false, HouseAOSLabel( "Available Storage" ));
		houseGump.AddHTMLGump( 310, 250, 80, 22, false, false, HouseAOSLabel( Math.max( iMulti.maxSecureContainers - iMulti.secureContainers, 0 ).toString() ));
		houseGump.AddHTMLGump( 10, 290, 300, 22, false, false, HouseAOSLabel( "Maximum Lockdowns" ));
		houseGump.AddHTMLGump( 310, 290, 80, 22, false, false, HouseAOSLabel( iMulti.maxLockdowns.toString() ));
		houseGump.AddHTMLGump( 10, 310, 300, 22, false, false, HouseAOSLabel( "Available Lockdowns" ));
		houseGump.AddHTMLGump( 310, 310, 80, 22, false, false, HouseAOSLabel( Math.max( iMulti.maxLockdowns - iMulti.lockdowns, 0 ).toString() ));
		houseGump.AddHTMLGump( 10, 350, 300, 22, false, false, HouseAOSLabel( "Vendor Count" ));
		houseGump.AddHTMLGump( 310, 350, 80, 22, false, false, HouseAOSLabel( iMulti.vendors + " / " + iMulti.maxVendors ));
	}
	else if( pageName == "customize" )
	{
		if( isCustomizable )
			HouseAOSAddButtonLine( houseGump, 10, 120, 70, HouseAOSLabel( "Customize This House", isOwner ));
		else
			HouseAOSAddButtonLine( houseGump, 10, 120, 70, HouseAOSLabel( "Convert Into Customizable House", isOwner ));
		HouseAOSAddButtonLine( houseGump, 10, 180, 2, HouseAOSLabel( "Change House Sign", isOwner && iMulti.isPublic ));
		HouseAOSAddButtonLine( houseGump, 10, 200, 206, HouseAOSLabel( "Change House Sign Hanger", isOwner && isCustomizable ));
		HouseAOSAddButtonLine( houseGump, 10, 220, 207, HouseAOSLabel( "Change Signpost", isOwner && isCustomizable ));
		HouseAOSAddButtonLine( houseGump, 10, 250, 208, HouseAOSLabel( "Change Foundation Style", isOwner && isCustomizable ));
		HouseAOSAddButtonLine( houseGump, 10, 280, 4, HouseAOSLabel( "Rename House", isCoOwner ));
	}
	else if( pageName == "ownership" )
	{
		HouseAOSAddButtonLine( houseGump, 10, 130, 51, HouseAOSLabel( "Demolish House", isOwner ));
		HouseAOSAddButtonLine( houseGump, 10, 150, 50, HouseAOSLabel( "Trade House", isOwner ));
		HouseAOSAddButtonLine( houseGump, 10, 190, 209, HouseAOSLabel( "Make Primary", false )	);
		if( pUser.isGM )
		{
			if( HouseAOSBoolTag( iMulti, "Grandfathered" ))
				HouseAOSAddButtonLine( houseGump, 10, 230, 60, HouseAOSLabel( "Disable Grandfathered", true ));
			else
				HouseAOSAddButtonLine( houseGump, 10, 230, 61, HouseAOSLabel( "Enable Grandfathered", true ));
		}
	}

	houseGump.Send( pUser.socket );
	houseGump.Free();
	return true;
}

function OpenHouseAOSPlayerListGump( pUser, listType )
{
	if( !HouseAOSRestoreContext( pUser.socket ))
		return false;

	var pLanguage = pUser.socket.language;
	var listCount = 0;
	var buttonBase = 0;
	var listTitle = "";
	var addButton = 0;
	var targetList = null;

	aosCoOwnerList.length = 0;
	aosFriendList.length = 0;
	aosGuestList.length = 0;
	aosBanList.length = 0;

	if( listType == "owner" )
	{
		listCount = aosMulti.owners;
		buttonBase = 100;
		listTitle = GetDictionaryEntry( 2801, pLanguage ) + " (" + aosMulti.owners + "/" + aosMulti.maxOwners + ")";
		addButton = 11;
		targetList = aosCoOwnerList;
	}
	else if( listType == "friend" )
	{
		listCount = aosMulti.friends;
		buttonBase = 300;
		listTitle = GetDictionaryEntry( 2802, pLanguage ) + " (" + aosMulti.friends + "/" + aosMulti.maxFriends + ")";
		addButton = 21;
		targetList = aosFriendList;
	}
	else if( listType == "guest" )
	{
		listCount = aosMulti.guests;
		buttonBase = 700;
		listTitle = GetDictionaryEntry( 2803, pLanguage ) + " (" + aosMulti.guests + "/" + aosMulti.maxGuests + ")";
		addButton = 41;
		targetList = aosGuestList;
	}
	else
	{
		listCount = aosMulti.bans;
		buttonBase = 500;
		listTitle = GetDictionaryEntry( 2804, pLanguage ) + " (" + aosMulti.bans + "/" + aosMulti.maxBans + ")";
		addButton = 30;
		targetList = aosBanList;
	}

	var pageCount = Math.max( 1, Math.ceil( listCount / 8 ));
	var gump = new Gump;
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 300, 380, 5054 );
	gump.AddTiledGump( 10, 10, 280, 360, 2624 );
	gump.AddCheckerTrans( 10, 10, 280, 360 );
	gump.AddButton( 260, 18, 4017, 4018, 1, 0, 1 );
	gump.AddHTMLGump( 20, 18, 240, 22, false, false, "<CENTER>" + GetDictionaryEntry( 2800, pLanguage ) + "</CENTER>" );
	gump.AddHTMLGump( 20, 40, 240, 22, false, false, "<CENTER>" + aosOwnerName + "</CENTER>" );
	gump.AddGump( 70, 58, 10452 );
	gump.AddHTMLGump( 35, 128, 190, 22, false, false, listTitle );
	gump.AddButton( 235, 128, 5402, 5402, 1, 0, addButton );

	var index = 0;
	var page = 1;
	var y = 160;
	var houseChar;
	for( houseChar = aosMulti.FirstChar( listType ); !aosMulti.FinishedChars( listType ); houseChar = aosMulti.NextChar( listType ))
	{
		if( !ValidateObject( houseChar ) || aosMulti.IsOwner( houseChar ))
			continue;

		if(( index % 8 ) == 0 )
		{
			page = Math.floor( index / 8 ) + 1;
			y = 160;
			gump.AddPage( page );

			if( pageCount > 1 )
				gump.AddHTMLGump( 105, 330, 110, 22, false, false, GetDictionaryEntry( 2805, pLanguage ) + " " + page + " of " + pageCount );

			if( page > 1 )
			{
				gump.AddButton( 35, 330, 4014, 4015, 0, page - 1, 0 );
				gump.AddHTMLGump( 75, 332, 45, 20, false, false, GetDictionaryEntry( 2807, pLanguage ));
			}

			if( page < pageCount )
			{
				gump.AddButton( 205, 330, 4005, 4006, 0, page + 1, 0 );
				gump.AddHTMLGump( 245, 332, 45, 20, false, false, GetDictionaryEntry( 2806, pLanguage ));
			}
		}

		gump.AddButton( 30, y + 2, 2710, 2711, 1, 0, buttonBase + targetList.length );
		gump.AddHTMLGump( 55, y, 220, 22, false, false, "(" + GetDictionaryEntry( 2808, pLanguage ) + ") - " + houseChar.name );
		targetList.push( houseChar );
		y += 20;
		index++;
	}

	gump.Send( pUser.socket );
	gump.Free();
	return true;
}

function OpenHouseAOSNameGump( pUser )
{
	var pLanguage = pUser.socket.language;
	var gump = new Gump;
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 350, 180, 2620 );
	gump.AddBackground( 10, 10, 330, 160, 3000 );
	gump.AddButton( 305, 15, 4017, 4018, 1, 0, 1 );
	gump.AddButton( 175, 125, 4023, 4024, 1, 0, 5 );
	gump.AddGump( 25, 45, 100 );
	gump.AddHTMLGump( 25, 20, 280, 20, false, false, "<BIG>" + GetDictionaryEntry( 2845, pLanguage ) + "</BIG>" );
	gump.AddHTMLGump( 175, 60, 150, 60, false, false, GetDictionaryEntry( 2846, pLanguage ));
	gump.AddHTMLGump( 210, 127, 130, 20, false, false, GetDictionaryEntry( 2847, pLanguage ));
	gump.AddTextEntryLimited( 50, 60, 100, 200, 0, 1, 3, aosSign.name, 60 );
	gump.Send( pUser.socket );
	gump.Free();
}

function OpenHouseAOSSignSelectionGump( pUser )
{
	var pLanguage = pUser.socket.language;
	var signIds = [
		2966, 2980, 2982, 2984, 2986, 2988, 2990, 2992,
		2994, 2996, 2998, 3000, 3002, 3004, 3006, 3008,
		3010, 3012, 3014, 3016, 3018, 3020, 3022, 3024,
		3026, 3028, 3030, 3032, 3034, 3036, 3038, 3040,
		3042, 3044, 3046, 3048, 3052, 3054, 3056, 3058,
		3060, 3062, 3064, 3066, 3068, 3070, 3072, 3074,
		3076, 3078, 3080, 3082, 3084, 3086, 3140
	];

	var gump = new Gump;
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 350, 370, 2620 );
	gump.AddBackground( 10, 10, 330, 350, 3000 );
	gump.AddHTMLGump( 0, 13, 350, 20, false, false, "<CENTER><BIG>" + GetDictionaryEntry( 2851, pLanguage ) + "</BIG></CENTER>" );
	gump.AddHTMLGump( 0, 315, 350, 20, false, false, "<CENTER>" + GetDictionaryEntry( 2852, pLanguage ) + "</CENTER>" );
	gump.AddButton( 305, 15, 4017, 4018, 1, 0, 1 );
	gump.AddButton( 150, 335, 4023, 4024, 1, 0, 3 );

	for( var i = 0; i < signIds.length; i++ )
	{
		var page = Math.floor( i / 28 ) + 1;
		var pageIndex = i % 28;
		var col = Math.floor( pageIndex / 7 );
		var row = pageIndex % 7;
		var x = 22 + ( col * 80 );
		var y = 40 + ( row * 40 );
		var signId = signIds[i];

		if( pageIndex == 0 )
			gump.AddPage( page );

		gump.AddRadio( x, y + 5, 9722, ( aosSign.id == signId ? 1 : 0 ), signId );
		gump.AddPicture( x + 23, y, signId );

		if( pageIndex == 27 && i < signIds.length - 1 )
		{
			gump.AddText( 245, 335, 0, GetDictionaryEntry( 2806, pLanguage ));
			gump.AddButton( 305, 335, 4005, 4006, 0, page + 1, 0 );
		}
	}

	gump.AddText( 60, 335, 0, GetDictionaryEntry( 2807, pLanguage ));
	gump.AddButton( 19, 335, 4014, 4015, 0, 1, 0 );
	gump.Send( pUser.socket );
	gump.Free();
}

function OpenHouseAOSCustomizationSelectionGump( pUser, selectionType )
{
	if( !HouseAOSRestoreContext( pUser.socket ))
		return false;

	var title = "";
	var itemIds = [];
	var buttonBase = 0;
	var columns = 0;
	var itemSpacingX = 0;
	var itemSpacingY = 0;
	var itemsPerPage = 0;

	if( selectionType == "hanger" )
	{
		title = "Select House Sign Hanger";
		itemIds = aosHangerIds;
		buttonBase = 800;
		columns = 3;
		itemSpacingX = 100;
		itemSpacingY = 80;
		itemsPerPage = 6;
	}
	else if( selectionType == "foundation" )
	{
		title = "Select Foundation Style";
		itemIds = aosFoundationIds;
		buttonBase = 900;
		columns = 5;
		itemSpacingX = 80;
		itemSpacingY = 80;
		itemsPerPage = 15;
	}
	else
	{
		title = "Select Signpost";
		itemIds = aosPostIds;
		buttonBase = 1000;
		columns = 8;
		itemSpacingX = 50;
		itemSpacingY = 110;
		itemsPerPage = 16;
	}

	var pageCount = Math.max( 1, Math.ceil( itemIds.length / itemsPerPage ));
	var gump = new Gump;
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 420, 440, 5054 );
	gump.AddTiledGump( 10, 10, 400, 100, 2624 );
	gump.AddCheckerTrans( 10, 10, 400, 100 );
	gump.AddTiledGump( 10, 120, 400, 260, 2624 );
	gump.AddCheckerTrans( 10, 120, 400, 260 );
	gump.AddTiledGump( 10, 390, 400, 40, 2624 );
	gump.AddCheckerTrans( 10, 390, 400, 40 );
	gump.AddButton( 250, 410, 4005, 4007, 1, 0, 204 );
	gump.AddHTMLGump( 285, 410, 100, 22, false, false, "BACK" );
	gump.AddHTMLGump( 20, 40, 380, 30, false, false, "<CENTER><BIG>" + title + "</BIG></CENTER>" );

	for( var i = 0; i < itemIds.length; ++i )
	{
		var page = Math.floor( i / itemsPerPage ) + 1;
		var pageIndex = i % itemsPerPage;
		var x = 15 + (( pageIndex % columns ) * itemSpacingX );
		var y = 150 + ( Math.floor( pageIndex / columns ) * itemSpacingY );

		if( pageIndex == 0 )
		{
			gump.AddPage( page );

			if( pageCount > 1 )
			{
				gump.AddButton( 10, 360, page == 1 ? 4005 : 4014, page == 1 ? 4007 : 4016, 0, ( page % pageCount ) + 1, 0 );
				gump.AddHTMLGump( 55, 360, 130, 22, false, false, "Page " + page + " of " + pageCount );
			}
		}

		gump.AddButton( x, y, 4005, 4007, 1, 0, buttonBase + i );
		gump.AddPicture( x + 20, y, itemIds[i] );
	}

	gump.Send( pUser.socket );
	gump.Free();
	return true;
}

function OpenHouseAOSGuestGump( pUser, iMulti, iSign, houseOwner, visitCount )
{
	if( !ValidateObject( pUser ) || pUser.socket == null || !ValidateObject( iMulti ) || !ValidateObject( iSign ))
		return false;

	HouseAOSStoreContext( pUser, iMulti, iSign, houseOwner, visitCount );

	var pLanguage = pUser.socket.language;
	var houseGuestGump = new Gump;
	houseGuestGump.AddPage( 0 );
	houseGuestGump.AddBackground( 0, 0, 240, 240, 5054 );
	houseGuestGump.AddTiledGump( 10, 10, 220, 220, 2624 );
	houseGuestGump.AddCheckerTrans( 10, 10, 220, 220 );
	houseGuestGump.AddGump( 45, 8, 100 );
	houseGuestGump.AddHTMLGump( 60, Math.max(( 72 - iSign.name.length ), 22 ), 115, 60, false, false, "<CENTER><BIG>" + iSign.name + "</BIG></CENTER>" );
	houseGuestGump.AddHTMLGump( 25, 115, 190, 22, false, false, "<CENTER>" + GetDictionaryEntry( 2800, pLanguage ) + "</CENTER>" );
	houseGuestGump.AddHTMLGump( 25, 137, 190, 22, false, false, "<CENTER>" + houseOwner + "</CENTER>" );

	if( iMulti.isPublic )
		houseGuestGump.AddHTMLGump( 25, 170, 190, 42, false, false, "<CENTER><BASEFONT COLOR=#00FF00>" + GetDictionaryEntry( 2811, pLanguage ) + "</BASEFONT><BR>" + visitCount.toString() + " " + GetDictionaryEntry( 2812, pLanguage ) + "</CENTER>" );
	else
		houseGuestGump.AddHTMLGump( 25, 170, 190, 42, false, false, "<CENTER><BASEFONT COLOR=#FF0000>" + GetDictionaryEntry( 2810, pLanguage ) + "</BASEFONT></CENTER>" );

	houseGuestGump.AddButton( 205, 15, 4017, 4018, 1, 0, 0 );
	houseGuestGump.Send( pUser.socket );
	houseGuestGump.Free();
	return true;
}

function HouseAOSConfirmActionGump( pUser, confirmString, confirmButtonID )
{
	var pLanguage = pUser.socket.language;
	var confirmGump = new Gump;
	confirmGump.AddPage( 0 );
	confirmGump.AddBackground( 0, 0, 300, 140, 2620 );
	confirmGump.AddBackground( 10, 10, 280, 120, 3000 );
	confirmGump.AddHTMLGump( 10, 20, 280, 60, false, false, "<CENTER><BIG>" + GetDictionaryEntry( 2848, pLanguage ) + "</BIG></CENTER>" );
	confirmGump.AddHTMLGump( 25, 50, 250, 60, false, false, confirmString );
	confirmGump.AddButton( 100, 100, 4023, 4024, 1, 0, confirmButtonID );
	confirmGump.AddButton( 175, 100, 4020, 4021, 1, 0, 1 );
	confirmGump.AddText( 25, 100, 0, GetDictionaryEntry( 2849, pLanguage ));
	confirmGump.AddText( 210, 100, 0, GetDictionaryEntry( 2850, pLanguage ));
	confirmGump.Send( pUser.socket );
	confirmGump.Free();
}

function onGumpPress( pSocket, pButton, gumpData )
{
	if( pButton == 0 || pSocket == null || !HouseAOSRestoreContext( pSocket ))
		return;

	var pUser = pSocket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	switch( pButton )
	{
		case 1:
			OpenHouseAOSGump( pUser, aosMulti, aosSign, aosOwnerName, aosVisitCount );
			break;
		case 201:
			OpenHouseAOSGump( pUser, aosMulti, aosSign, aosOwnerName, aosVisitCount, "info" );
			return;
		case 202:
			OpenHouseAOSGump( pUser, aosMulti, aosSign, aosOwnerName, aosVisitCount, "security" );
			return;
		case 203:
			OpenHouseAOSGump( pUser, aosMulti, aosSign, aosOwnerName, aosVisitCount, "storage" );
			return;
		case 204:
			OpenHouseAOSGump( pUser, aosMulti, aosSign, aosOwnerName, aosVisitCount, "customize" );
			return;
		case 205:
			OpenHouseAOSGump( pUser, aosMulti, aosSign, aosOwnerName, aosVisitCount, "ownership" );
			return;
		case 206:
			if( HouseAOSCanOwn( pUser, aosMulti ) && HouseAOSIsCustomFoundation( aosMulti ))
				OpenHouseAOSCustomizationSelectionGump( pUser, "hanger" );
			return;
		case 207:
			if( HouseAOSCanOwn( pUser, aosMulti ) && HouseAOSIsCustomFoundation( aosMulti ))
				OpenHouseAOSCustomizationSelectionGump( pUser, "post" );
			return;
		case 208:
			if( HouseAOSCanOwn( pUser, aosMulti ) && HouseAOSIsCustomFoundation( aosMulti ))
				OpenHouseAOSCustomizationSelectionGump( pUser, "foundation" );
			return;
		case 2:
			if( aosMulti.isPublic && HouseAOSCanManage( pUser, aosMulti ))
				OpenHouseAOSSignSelectionGump( pUser );
			else if( !aosMulti.isPublic )
				pSocket.SysMessage( GetDictionaryEntry( 1941, pSocket.language ));
			else
				pSocket.SysMessage( GetDictionaryEntry( 1940, pSocket.language ));
			break;
		case 3:
			if( aosMulti.isPublic && HouseAOSCanManage( pUser, aosMulti ) && pSocket.GetWord( 21 ) > 0 )
			{
				var newID = pSocket.GetWord( 21 );
				if( aosSign.id % 2 != 0 )
					newID -= 1;

				aosSign.id = newID;
				pSocket.SysMessage( GetDictionaryEntry( 556, pSocket.language ));
			}
			OpenHouseAOSGump( pUser, aosMulti, aosSign, aosOwnerName, aosVisitCount );
			break;
		case 4:
			if( HouseAOSCanManage( pUser, aosMulti ))
				OpenHouseAOSNameGump( pUser );
			else
				pSocket.SysMessage( GetDictionaryEntry( 1942, pSocket.language ));
			break;
		case 5:
			if( HouseAOSCanManage( pUser, aosMulti ))
			{
				var newName = gumpData.getEdit( 0 );
				if( newName.length <= 60 )
				{
					aosSign.name = newName;
					pSocket.SysMessage( GetDictionaryEntry( 1943, pSocket.language ), newName );
				}
				else
					pSocket.SysMessage( GetDictionaryEntry( 1944, pSocket.language ));
			}
			OpenHouseAOSGump( pUser, aosMulti, aosSign, aosOwnerName, aosVisitCount );
			break;
		case 10:
			if( HouseAOSCanManage( pUser, aosMulti ))
				OpenHouseAOSPlayerListGump( pUser, "owner" );
			else
				pSocket.SysMessage( GetDictionaryEntry( 1945, pSocket.language ));
			break;
		case 20:
			if( HouseAOSCanAccessFriends( pUser, aosMulti ))
				OpenHouseAOSPlayerListGump( pUser, "friend" );
			break;
		case 32:
			if( HouseAOSCanAccessFriends( pUser, aosMulti ))
				OpenHouseAOSPlayerListGump( pUser, "banned" );
			break;
		case 11:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
				TriggerEvent( houseCommandScriptID, "AddOwner", pSocket, aosMulti );
			break;
		case 12:
			if( HouseAOSCanManage( pUser, aosMulti ))
				TriggerEvent( houseCommandScriptID, "RemoveOwnerTrigger", pSocket, aosMulti );
			break;
		case 13:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
				HouseAOSConfirmActionGump( pUser, GetDictionaryEntry( 1946, pSocket.language ), 14 );
			else
				pSocket.SysMessage( GetDictionaryEntry( 1947, pSocket.language ));
			break;
		case 14:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
			{
				aosMulti.ClearOwnerList();
				pSocket.SysMessage( GetDictionaryEntry( 1948, pSocket.language ));
			}
			break;
		case 21:
			if( HouseAOSCanManage( pUser, aosMulti ))
				TriggerEvent( houseCommandScriptID, "AddFriend", pSocket, aosMulti );
			break;
		case 22:
			if( HouseAOSCanManage( pUser, aosMulti ))
				TriggerEvent( houseCommandScriptID, "RemoveFriendTrigger", pSocket, aosMulti );
			break;
		case 23:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
				HouseAOSConfirmActionGump( pUser, GetDictionaryEntry( 1949, pSocket.language ), 24 );
			else
				pSocket.SysMessage( GetDictionaryEntry( 1950, pSocket.language ));
			break;
		case 24:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
			{
				aosMulti.ClearFriendList();
				pSocket.SysMessage( GetDictionaryEntry( 1951, pSocket.language ));
			}
			break;
		case 30:
			if( HouseAOSCanAccessFriends( pUser, aosMulti ))
				TriggerEvent( houseCommandScriptID, "BanPlayer", pSocket, aosMulti );
			break;
		case 31:
			if( HouseAOSCanAccessFriends( pUser, aosMulti ))
				TriggerEvent( houseCommandScriptID, "EjectPlayer", pSocket, aosMulti );
			break;
		case 33:
			if( HouseAOSCanAccessFriends( pUser, aosMulti ))
				TriggerEvent( houseCommandScriptID, "UnbanPlayerTrigger", pSocket, aosMulti );
			break;
		case 62:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
				HouseAOSConfirmActionGump( pUser, "Are you sure you want to clear the ban list?", 63 );
			break;
		case 63:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
				aosMulti.ClearBanList();
			break;
		case 40:
			if( HouseAOSCanAccessFriends( pUser, aosMulti ))
				OpenHouseAOSPlayerListGump( pUser, "guest" );
			break;
		case 41:
			if( HouseAOSCanManage( pUser, aosMulti ))
				TriggerEvent( houseCommandScriptID, "AddGuest", pSocket, aosMulti );
			else
				pSocket.SysMessage( GetDictionaryEntry( 1859, pSocket.language ));
			break;
		case 42:
			if( HouseAOSCanManage( pUser, aosMulti ))
				TriggerEvent( houseCommandScriptID, "RemoveGuestTrigger", pSocket, aosMulti );
			else
				pSocket.SysMessage( GetDictionaryEntry( 1867, pSocket.language ));
			break;
		case 43:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
				HouseAOSConfirmActionGump( pUser, GetDictionaryEntry( 1952, pSocket.language ), 44 );
			else
				pSocket.SysMessage( GetDictionaryEntry( 1953, pSocket.language ));
			break;
		case 44:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
			{
				aosMulti.ClearGuestList();
				pSocket.SysMessage( GetDictionaryEntry( 1954, pSocket.language ));
			}
			break;
		case 50:
			if( HouseAOSCanOwn( pUser, aosMulti ))
				HouseAOSConfirmActionGump( pUser, "Are you sure you want to transfer ownership of this house?", 55 );
			break;
		case 51:
			if( HouseAOSCanOwn( pUser, aosMulti ))
				HouseAOSConfirmActionGump( pUser, "Are you sure you want to demolish this house?", 56 );
			break;
		case 52:
			if( HouseAOSCanOwn( pUser, aosMulti ))
				HouseAOSConfirmActionGump( pUser, "Are you sure you want to change the locks on this house?", 57 );
			break;
		case 53:
			if( HouseAOSCanOwn( pUser, aosMulti ))
				HouseAOSConfirmActionGump( pUser, "Are you sure you want to make this house public?", 58 );
			break;
		case 54:
			if( HouseAOSCanOwn( pUser, aosMulti ))
				HouseAOSConfirmActionGump( pUser, "Are you sure you want to make this house private?", 59 );
			break;
		case 55:
			if( HouseAOSCanOwn( pUser, aosMulti ))
				TriggerEvent( houseCommandScriptID, "TransferOwnership", pSocket, aosMulti );
			break;
		case 56:
			if( HouseAOSCanOwn( pUser, aosMulti ))
				TriggerEvent( houseCommandScriptID, "DemolishHouse", pSocket, aosMulti );
			break;
		case 57:
			if( HouseAOSCanOwn( pUser, aosMulti ))
				TriggerEvent( houseCommandScriptID, "ChangeHouseLocks", pSocket, aosMulti );
			break;
		case 58:
			if( HouseAOSCanOwn( pUser, aosMulti ))
				TriggerEvent( houseCommandScriptID, "DeclareHousePublic", pSocket, aosMulti );
			break;
		case 59:
			if( HouseAOSCanOwn( pUser, aosMulti ))
				TriggerEvent( houseCommandScriptID, "DeclareHousePrivate", pSocket, aosMulti );
			break;
		case 60:
			if( pUser.isGM )
			{
				aosMulti.SetTag( "Grandfathered", false );
				for( var houseTimerID = 1; houseTimerID <= 7; ++houseTimerID )
					aosMulti.KillJSTimer( houseTimerID, 15000 );
				aosMulti.StartTimer( GetServerSetting( "DecayStageLikeNewMins" ) * 60 * 1000, 1, 15000 );
				aosMulti.SetTag( "decayStage", 1 );
				aosMulti.SetTag( "houseDecayInit", true );
				pSocket.SysMessage( "Grandfathered status disabled (decay enabled)" );
				OpenHouseAOSGump( pUser, aosMulti, aosSign, aosOwnerName, aosVisitCount );
			}
			break;
		case 61:
			if( pUser.isGM )
			{
				aosMulti.SetTag( "Grandfathered", true );
				for( var houseTimerID2 = 1; houseTimerID2 <= 7; ++houseTimerID2 )
					aosMulti.KillJSTimer( houseTimerID2, 15000 );
				pSocket.SysMessage( "Grandfathered status enabled (no decay)" );
				OpenHouseAOSGump( pUser, aosMulti, aosSign, aosOwnerName, aosVisitCount );
			}
			break;
		case 70:
			if( HouseAOSCanOwn( pUser, aosMulti ))
			{
				if( !HouseBeginCustomize( pUser, aosMulti ))
					pSocket.SysMessage( "Unable to enter house customization mode." );
			}
			break;
		default:
			break;
	}

	if( pButton >= 100 && pButton <= 250 && ( pUser.isGM || aosMulti.IsOwner( pUser )))
		TriggerEvent( houseCommandScriptID, "RemoveOwner", pSocket, aosCoOwnerList[( pButton - 100 )], aosMulti );
	else if( pButton >= 300 && pButton <= 450 && HouseAOSCanManage( pUser, aosMulti ))
		TriggerEvent( houseCommandScriptID, "RemoveFriend", pSocket, aosFriendList[( pButton - 300 )], aosMulti );
	else if( pButton >= 500 && pButton <= 650 && HouseAOSCanAccessFriends( pUser, aosMulti ))
		TriggerEvent( houseCommandScriptID, "UnbanPlayer", pSocket, aosBanList[( pButton - 500 )], aosMulti );
	else if( pButton >= 700 && pButton <= 750 && HouseAOSCanManage( pUser, aosMulti ))
		TriggerEvent( houseCommandScriptID, "RemoveGuest", pSocket, aosGuestList[( pButton - 700 )], aosMulti );
	else if( pButton >= 800 && pButton < 800 + aosHangerIds.length && HouseAOSCanOwn( pUser, aosMulti ))
	{
		if( HouseSetCustomizationFixture( pUser, aosMulti, 0, aosHangerIds[( pButton - 800 )] ))
			OpenHouseAOSGump( pUser, aosMulti, aosSign, aosOwnerName, aosVisitCount, "customize" );
	}
	else if( pButton >= 900 && pButton < 900 + aosFoundationIds.length && HouseAOSCanOwn( pUser, aosMulti ))
	{
		if( HouseSetCustomizationFixture( pUser, aosMulti, 2, ( pButton - 900 ) ))
			OpenHouseAOSGump( pUser, aosMulti, aosSign, aosOwnerName, aosVisitCount, "customize" );
	}
	else if( pButton >= 1000 && pButton < 1000 + aosPostIds.length && HouseAOSCanOwn( pUser, aosMulti ))
	{
		if( HouseSetCustomizationFixture( pUser, aosMulti, 1, aosPostIds[( pButton - 1000 )] ))
			OpenHouseAOSGump( pUser, aosMulti, aosSign, aosOwnerName, aosVisitCount, "customize" );
	}
}
