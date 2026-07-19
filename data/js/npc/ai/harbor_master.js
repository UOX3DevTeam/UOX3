/// <reference path="../../definitions.d.ts" />
// @ts-check

var harborSearchOwner = 0;
var harborSearchResult = 0;

/** @type { ( socket: Socket, harborMaster: Character ) => any } */
function onContextMenuRequest( socket, harborMaster )
{
	var user = socket.currentChar;
	if( !ValidateObject( user ) || user.dead || !harborMaster.InRange( user, 6 ))
	{
		return true;
	}
	var entries = [{ id: 43, text: 1149570, flags: 0x0000, hue: 0x03E0 }];
	if( ValidateObject( FindOwnedShip( user )) && !user.GetTempTag( "harborAbandonPending" ))
	{
		entries.push({ id: 44, text: 1150110, flags: 0x0000, hue: 0x03E0 });
	}
	TriggerEvent( 18001, "modifyContextMenu", socket, harborMaster, entries, true );
	return false;
}

/** @type { ( socket: Socket, harborMaster: Character, popupEntry: number ) => any } */
function onContextMenuSelect( socket, harborMaster, popupEntry )
{
	var user = socket.currentChar;
	if( !ValidateObject( user ) || user.dead || !harborMaster.InRange( user, 6 ))
	{
		return false;
	}
	if( popupEntry == 43 )
	{
		CreateReplacementShipToken( socket, user, harborMaster );
	}
	else if( popupEntry == 44 )
	{
		ShowAbandonShipGump( socket, user );
	}
	return false;
}

/** @type { ( user: Character ) => any } */
function FindOwnedShip( user )
{
	harborSearchOwner = user.serial;
	harborSearchResult = 0;
	IterateOver( "MULTI" );
	harborSearchOwner = 0;
	return CalcItemFromSer( harborSearchResult );
}

/** @type { ( candidate: Character | Item ) => boolean } */
function onIterate( candidate )
{
	if( harborSearchResult || !ValidateObject( candidate ) || !candidate.isItem || !candidate.IsBoat() )
	{
		return false;
	}
	var owner = CalcCharFromSer( harborSearchOwner );
	if( !ValidateObject( owner ) || candidate.owner != owner || IsHarborRowBoat( candidate ))
	{
		return false;
	}
	harborSearchResult = candidate.serial;
	return true;
}

/** @type { ( boat: Multi ) => boolean } */
function IsHarborRowBoat( boat )
{
	var multiID = parseInt( boat.id ) - 0x4000;
	return ( multiID >= 0x3C && multiID <= 0x3F ) || ( multiID >= 0x50 && multiID <= 0x53 );
}

/** @type { ( boat: Multi ) => boolean } */
function IsHighSeasHarborShip( boat )
{
	var multiID = parseInt( boat.id ) - 0x4000;
	return ( multiID >= 0x18 && multiID <= 0x1B ) || ( multiID >= 0x24 && multiID <= 0x27 ) ||
		( multiID >= 0x30 && multiID <= 0x33 ) || ( multiID >= 0x40 && multiID <= 0x43 );
}

/** @type { ( socket: Socket, user: Character, harborMaster: Character ) => any } */
function CreateReplacementShipToken( socket, user, harborMaster )
{
	var boat = FindOwnedShip( user );
	if( !ValidateObject( boat ))
	{
		harborMaster.TextMessage( "Your ship could not be located.", true, 0x03B2 );
		return;
	}
	var bankBox = user.FindItemLayer( 29 );
	if( !ValidateObject( bankBox ) || bankBox.ResourceCount( 0x0EED, 0 ) < 100 )
	{
		harborMaster.TextMessage( "Begging thy pardon, but thou canst not afford that.", true, 0x03B2 );
		return;
	}
	var token = CreateDFNItem( socket, user, IsHighSeasHarborShip( boat ) ? "usedrecallrune" : "boat_key", 1, "ITEM", true );
	if( !ValidateObject( token ))
	{
		return;
	}
	bankBox.UseResource( 100, 0x0EED, 0 );
	if( IsHighSeasHarborShip( boat ))
	{
		token.id = 0x1F14;
		token.type = 50;
		token.morex = boat.x;
		token.morey = boat.y;
		token.morez = boat.z;
		token.more = boat.worldnumber;
		token.more0 = boat.instanceID;
		token.SetTag( "multiSerial", boat.serial.toString() );
		token.SetTag( "shipSerial", boat.serial );
		token.SetTag( "highSeasShipRune", 1 );
		token.name = "a recall rune for " + boat.name;
	}
	else
	{
		token.more = boat.serial;
		token.name = "a ship key";
	}
	token.Refresh();
	harborMaster.TextMessage( IsHighSeasHarborShip( boat ) ?
		"A recall rune to your ship has been placed in your backpack." :
		"A replacement ship key has been placed in your backpack.", true, 0x03B2 );
}

/** @type { ( socket: Socket, user: Character ) => any } */
function ShowAbandonShipGump( socket, user )
{
	var boat = FindOwnedShip( user );
	if( !ValidateObject( boat ) || user.GetTempTag( "harborAbandonPending" ))
	{
		return;
	}
	user.SetTempTag( "harborAbandonBoat", boat.serial );
	var gump = new Gump;
	gump.AddPage( 0 );
	gump.AddBackground( 0, 0, 420, 190, 0x13BE );
	gump.AddHTMLGump( 20, 20, 380, 100, false, false,
		"<CENTER><BIG>Abandon Ship</BIG><BR><BR>WARNING: Your ship and everything aboard it or in its cargo hold will be deleted in five minutes. Are you certain?</CENTER>" );
	gump.AddButton( 80, 140, 0x0FA5, 0x0FA7, 1, 0, 1 );
	gump.AddHTMLGump( 115, 142, 90, 25, false, false, "Accept" );
	gump.AddButton( 235, 140, 0x0FB1, 0x0FB2, 1, 0, 0 );
	gump.AddHTMLGump( 270, 142, 90, 25, false, false, "Cancel" );
	gump.Send( user );
	gump.Free();
}

/** @type { ( socket: Socket, button: number, gumpData: GumpData ) => any } */
function onGumpPress( socket, button, gumpData )
{
	var user = socket.currentChar;
	if( button != 1 )
	{
		user.SetTempTag( "harborAbandonBoat", null );
		socket.SysMessage( "Cancelled." );
		return;
	}
	var boat = CalcItemFromSer( parseInt( user.GetTempTag( "harborAbandonBoat" )));
	if( !ValidateObject( boat ) || !boat.IsBoat() || boat.owner != user || IsHarborRowBoat( boat ))
	{
		socket.SysMessage( "Your ship could not be located." );
		return;
	}
	user.SetTempTag( "harborAbandonPending", boat.serial );
	boat.SetTempTag( "harborAbandonOwner", user.serial );
	boat.SetTempTag( "harborAbandonPending", 1 );
	boat.StartTimer( 300000, 1, 3235 );
	socket.SysMessage( "Your ship has been abandoned. It will decay within five minutes." );
}

/** @type { ( boat: Multi, timerID: number ) => any } */
function onTimer( boat, timerID )
{
	if( timerID != 1 || !ValidateObject( boat ) || parseInt( boat.GetTempTag( "harborAbandonPending" )) != 1 )
	{
		return;
	}
	var owner = CalcCharFromSer( parseInt( boat.GetTempTag( "harborAbandonOwner" )));
	if( ValidateObject( owner ))
	{
		owner.SetTempTag( "harborAbandonPending", null );
		owner.SetTempTag( "harborAbandonBoat", null );
	}
	var itemSerials = [];
	for( var item = boat.FirstItem(); !boat.FinishedItems(); item = boat.NextItem() )
	{
		if( ValidateObject( item ))
		{
			itemSerials.push( item.serial );
		}
	}
	for( var i = 0; i < itemSerials.length; ++i )
	{
		var shipItem = CalcItemFromSer( itemSerials[i] );
		if( ValidateObject( shipItem ))
		{
			shipItem.Delete();
		}
	}
	boat.Delete();
}
