/// <reference path="../../definitions.d.ts" />
// @ts-check
let signTypes = {
	1673: "Happy Holidays",
	1674: "Merry Christmas",
	1675: "Seasons Greetings",
	1676: "Happy Hanukkah"
};

/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;
	if ( pSocket == null )
		return false;

	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ))
		return false;

	var iMulti = pUser.multi;
	if( ValidateObject( iMulti ) && ( iMulti.IsOnOwnerList( pUser )
		|| ( GetServerSetting( "COOWNHOUSESONSAMEACCOUNT" ) && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum ))) 
	{
		var gumpID = iUsed.GetTag( "GumpID" ) | 0;
		if( gumpID < 1673 || gumpID > 1676 )
		{
			gumpID = 1673;
			iUsed.SetTag( "GumpID", gumpID );
		}

		var holidaysignGump = new Gump();
		holidaysignGump.AddPage( 0 );
		holidaysignGump.AddGump( 0, 0, gumpID );
		holidaysignGump.Send( pSocket );
		holidaysignGump.Free();
	}
	else
	{
		pSocket.SysMessage(GetDictionaryEntry( 2067, pSocket.language )); // You must be in your house to do this.
		return false;
	}

	return false;
}

/** @type { ( tSock: Socket, baseObj: BaseObject ) => boolean } */
function onContextMenuRequest( socket, targObj )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return false;

	var list = [];
	list.push( { id: 0x1001, text: 1158829, flags: 0x0000, hue: 0x03E0 } ); // Change Sign Text
	list.push( { id: 0x1002, text: 1155742, flags: 0x0000, hue: 0x03E0 } ); // Toggle: On/Off

	var useKR = true; // 2D so hue works
	TriggerEvent( 18001, "modifyContextMenu", socket, targObj, list, useKR );
	return false;
}

/** @type { ( tSock: Socket, baseObj: BaseObject, popupEntry: number ) => boolean } */
function onContextMenuSelect( socket, targObj, popupEntry )
{
	var pUser = socket.currentChar;

	if( !ValidateObject( pUser ) || !ValidateObject( targObj ) || !targObj.isItem )
		return false;

	var sign = targObj;

	switch( popupEntry )
	{
		// Change Sign Text
		case 0x1001:
		{
			var gumpID = sign.GetTag( "GumpID" ) | 0;

			// Normalize range first
			if( gumpID < 1673 || gumpID > 1676 )
				gumpID = 1673;
			else if( gumpID === 1676 )
				gumpID = 1673;      // wrap back to first
			else
				gumpID++;           // next text

			sign.SetTag( "GumpID", gumpID );

			var txt = signTypes[gumpID] || "Happy Holidays";
			socket.SysMessage( "The sign text has been set to: " + txt + "." );
			break;
		}

		// Toggle On / Off (lights on / lights off)
		case 0x1002:
		{
			var id = sign.id | 0;
			if( id === 0xA130 || id === 0xA134 )
			{
				sign.id = id + 1;
			}
			else if( id === 0xA131 || id === 0xA135 )
			{
				sign.id = id - 1;
			}

			sign.Refresh();
			break;
		}
	}

	return false;
}