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

function HouseAOSCanManage( pUser, iMulti )
{
	return ( pUser.isGM || iMulti.IsOnOwnerList( pUser ) || ( coOwnHousesOnSameAccountAOS && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum ));
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

function HouseAOSAddButtonLine( houseGump, x, y, buttonID, text )
{
	houseGump.AddButton( x, y + 2, 4005, 4006, 1, 0, buttonID );
	houseGump.AddHTMLGump( x + 35, y, 215, 22, false, false, text );
}

function HouseAOSAddPageButtonLine( houseGump, x, y, pageID, text )
{
	houseGump.AddButton( x, y + 2, 4005, 4006, 0, pageID, 0 );
	houseGump.AddHTMLGump( x + 35, y, 145, 22, false, false, text );
}

function OpenHouseAOSGump( pUser, iMulti, iSign, houseOwner, visitCount )
{
	if( !ValidateObject( pUser ) || pUser.socket == null || !ValidateObject( iMulti ) || !ValidateObject( iSign ))
		return false;

	HouseAOSStoreContext( pUser, iMulti, iSign, houseOwner, visitCount );

	var pLanguage = pUser.socket.language;
	var statusText = HouseAOSStatusText( iMulti );
	var isPublicText = iMulti.isPublic
		? "<BASEFONT COLOR=#00FF00>" + GetDictionaryEntry( 2811, pLanguage ) + "</BASEFONT>"
		: "<BASEFONT COLOR=#FF0000>" + GetDictionaryEntry( 2810, pLanguage ) + "</BASEFONT>";

	var houseGump = new Gump;
	houseGump.AddPage( 0 );
	houseGump.AddBackground( 0, 0, 420, 430, 5054 );
	houseGump.AddTiledGump( 10, 10, 400, 410, 2624 );
	houseGump.AddCheckerTrans( 10, 10, 400, 410 );
	houseGump.AddGump( 150, 12, 100 );
	houseGump.AddHTMLGump( 165, Math.max(( 72 - iSign.name.length ), 22 ), 115, 60, false, false, "<CENTER><BIG>" + iSign.name + "</BIG></CENTER>" );
	houseGump.AddButton( 375, 18, 4017, 4018, 1, 0, 0 );

	houseGump.AddPage( 1 );
	houseGump.AddHTMLGump( 30, 110, 360, 22, false, false, GetDictionaryEntry( 2800, pLanguage ) + " " + houseOwner );
	houseGump.AddHTMLGump( 30, 132, 360, 22, false, false, GetDictionaryEntry( 2809, pLanguage ) + " " + isPublicText + " (" + visitCount.toString() + " " + GetDictionaryEntry( 2812, pLanguage ) + ")" );

	if( statusText != "" )
		houseGump.AddHTMLGump( 30, 154, 360, 22, false, false, statusText );

	houseGump.AddHTMLGump( 30, 190, 230, 22, false, false, GetDictionaryEntry( 2818, pLanguage ));
	houseGump.AddHTMLGump( 275, 190, 100, 22, false, false, iMulti.lockdowns + " / " + iMulti.maxLockdowns );
	houseGump.AddHTMLGump( 30, 212, 230, 22, false, false, GetDictionaryEntry( 2819, pLanguage ));
	houseGump.AddHTMLGump( 275, 212, 100, 22, false, false, iMulti.secureContainers + " / " + iMulti.maxSecureContainers );
	houseGump.AddHTMLGump( 30, 234, 230, 22, false, false, GetDictionaryEntry( 2820, pLanguage ));
	houseGump.AddHTMLGump( 275, 234, 100, 22, false, false, iMulti.trashContainers + " / " + iMulti.maxTrashContainers );
	houseGump.AddHTMLGump( 30, 256, 230, 22, false, false, GetDictionaryEntry( 2821, pLanguage ));
	houseGump.AddHTMLGump( 275, 256, 100, 22, false, false, iMulti.vendors + " / " + iMulti.maxVendors );

	HouseAOSAddPageButtonLine( houseGump, 30, 310, 2, "Access" );
	HouseAOSAddPageButtonLine( houseGump, 215, 310, 3, GetDictionaryEntry( 2814, pLanguage ));
	HouseAOSAddButtonLine( houseGump, 30, 342, 2, GetDictionaryEntry( 2815, pLanguage ));
	HouseAOSAddButtonLine( houseGump, 215, 342, 4, GetDictionaryEntry( 2816, pLanguage ));

	if( HouseAOSCanManage( pUser, iMulti ))
		HouseAOSAddButtonLine( houseGump, 30, 374, 70, GetDictionaryEntry( 40030, pLanguage ));

	houseGump.AddPage( 2 );
	houseGump.AddHTMLGump( 30, 110, 340, 25, false, false, "<BIG>Access</BIG>" );
	HouseAOSAddButtonLine( houseGump, 35, 150, 10, GetDictionaryEntry( 2824, pLanguage ));
	HouseAOSAddButtonLine( houseGump, 35, 176, 11, GetDictionaryEntry( 2825, pLanguage ));
	HouseAOSAddButtonLine( houseGump, 35, 202, 12, GetDictionaryEntry( 2826, pLanguage ));
	HouseAOSAddButtonLine( houseGump, 35, 228, 13, GetDictionaryEntry( 2827, pLanguage ));
	HouseAOSAddButtonLine( houseGump, 215, 150, 20, GetDictionaryEntry( 2828, pLanguage ));
	HouseAOSAddButtonLine( houseGump, 215, 176, 21, GetDictionaryEntry( 2829, pLanguage ));
	HouseAOSAddButtonLine( houseGump, 215, 202, 22, GetDictionaryEntry( 2830, pLanguage ));
	HouseAOSAddButtonLine( houseGump, 215, 228, 23, GetDictionaryEntry( 2831, pLanguage ));

	if( !iMulti.isPublic )
	{
		HouseAOSAddButtonLine( houseGump, 35, 270, 40, GetDictionaryEntry( 2832, pLanguage ));
		HouseAOSAddButtonLine( houseGump, 35, 296, 41, GetDictionaryEntry( 2833, pLanguage ));
		HouseAOSAddButtonLine( houseGump, 35, 322, 42, GetDictionaryEntry( 2834, pLanguage ));
		HouseAOSAddButtonLine( houseGump, 35, 348, 43, GetDictionaryEntry( 2835, pLanguage ));
	}

	HouseAOSAddButtonLine( houseGump, 215, 270, 32, GetDictionaryEntry( 2836, pLanguage ));
	HouseAOSAddButtonLine( houseGump, 215, 296, 30, "<BASEFONT COLOR=#FF0000>" + GetDictionaryEntry( 2837, pLanguage ) + "</BASEFONT>" );
	HouseAOSAddButtonLine( houseGump, 215, 322, 33, GetDictionaryEntry( 2838, pLanguage ));
	HouseAOSAddButtonLine( houseGump, 215, 348, 31, GetDictionaryEntry( 2839, pLanguage ));

	houseGump.AddButton( 20, 392, 4014, 4015, 0, 1, 0 );
	houseGump.AddButton( 375, 392, 4005, 4006, 0, 3, 0 );

	houseGump.AddPage( 3 );
	houseGump.AddHTMLGump( 30, 110, 340, 25, false, false, "<BIG>" + GetDictionaryEntry( 2814, pLanguage ) + "</BIG>" );
	HouseAOSAddButtonLine( houseGump, 35, 150, 50, GetDictionaryEntry( 2840, pLanguage ));
	HouseAOSAddButtonLine( houseGump, 35, 176, 51, GetDictionaryEntry( 2841, pLanguage ));
	HouseAOSAddButtonLine( houseGump, 35, 202, 52, GetDictionaryEntry( 2842, pLanguage ));

	if( !iMulti.isPublic )
		HouseAOSAddButtonLine( houseGump, 35, 228, 53, GetDictionaryEntry( 2843, pLanguage ));
	else
		HouseAOSAddButtonLine( houseGump, 35, 228, 54, GetDictionaryEntry( 2844, pLanguage ));

	if( pUser.isGM )
	{
		if( iMulti.GetTag( "Grandfathered" ))
			HouseAOSAddButtonLine( houseGump, 35, 270, 60, "Disable Grandfathered" );
		else
			HouseAOSAddButtonLine( houseGump, 35, 270, 61, "Enable Grandfathered" );
	}

	houseGump.AddButton( 20, 392, 4014, 4015, 0, 2, 0 );
	houseGump.AddButton( 375, 392, 4005, 4006, 0, 1, 0 );
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
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
				HouseAOSConfirmActionGump( pUser, "Are you sure you want to transfer ownership of this house?", 55 );
			break;
		case 51:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
				HouseAOSConfirmActionGump( pUser, "Are you sure you want to demolish this house?", 56 );
			break;
		case 52:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
				HouseAOSConfirmActionGump( pUser, "Are you sure you want to change the locks on this house?", 57 );
			break;
		case 53:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
				HouseAOSConfirmActionGump( pUser, "Are you sure you want to make this house public?", 58 );
			break;
		case 54:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
				HouseAOSConfirmActionGump( pUser, "Are you sure you want to make this house private?", 59 );
			break;
		case 55:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
				TriggerEvent( houseCommandScriptID, "TransferOwnership", pSocket, aosMulti );
			break;
		case 56:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
				TriggerEvent( houseCommandScriptID, "DemolishHouse", pSocket, aosMulti );
			break;
		case 57:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
				TriggerEvent( houseCommandScriptID, "ChangeHouseLocks", pSocket, aosMulti );
			break;
		case 58:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
				TriggerEvent( houseCommandScriptID, "DeclareHousePublic", pSocket, aosMulti );
			break;
		case 59:
			if( pUser.isGM || aosMulti.IsOwner( pUser ))
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
			if( HouseAOSCanManage( pUser, aosMulti ))
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
}
