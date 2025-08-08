/*
This file is here for you to have a gump with placing single item addons, example as the wreath you can select the direction and target the area you wish it to be placed.
*/

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	socket.tempObj = iUsed;
	var iMulti = pUser.multi;
	var itemOwner = GetPackOwner( iUsed, 0 );

	if( itemOwner == null || itemOwner.serial != pUser.serial ) 
	{
		socket.SysMessage( GetDictionaryEntry( 1763, socket.language )); // The item must be in your backpack to use it.
		return false;
	}

	if( ValidateObject( iMulti ) && ( iMulti.IsOnOwnerList( pUser )
		|| ( GetServerSetting( "COOWNHOUSESONSAMEACCOUNT" ) && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum ))) 
	{
		AddonGump( pUser, iUsed );
	}
	else
	{
		socket.SysMessage(GetDictionaryEntry( 2067, socket.language )); // You must be in your house to do this.
		return false;
	}
}

function AddonGump( pUser, iUsed )
{
	var socket = pUser.socket;
	var addongump = new Gump;

	var addonID = AddonDisplayID( iUsed );
	var itemID = addonID[0];
	var itemID2 = addonID[1];

	addongump.AddPage( 0 );
	addongump.AddBackground( 0, 0, 300, 150, 0xA28 );

	addongump.AddPicture( 90, 30, itemID );
	addongump.AddPicture( 180, 30, itemID2 );
	addongump.AddButton( 50, 35, 0x868, 0x869, 1, 0, 1 );
	addongump.AddButton( 145, 35, 0x868, 0x869, 1, 0, 1 );

	addongump.Send( pUser );
	addongump.Free();
}

function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;
	var targMsg = GetDictionaryEntry( 5500, socket.language );// Where would you like to place this decoration?

	var iMulti = pUser.multi;
	if( ValidateObject( iMulti ) && ( iMulti.IsOnOwnerList( pUser ) || ( GetServerSetting( "COOWNHOUSESONSAMEACCOUNT" ) && ValidateObject( iMulti.owner ) && iMulti.owner.accountNum == pUser.accountNum )))
	{
		switch( pButton )
		{
			case 0: break;
			case 1: socket.CustomTarget( 0, targMsg ); break;
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 2067, socket.language )); // You must be in your house to do this.
	}
}

function onCallback0( socket, myTarget )
{
	var iUsed = socket.tempObj;
	var mChar = socket.currentChar;
	var targX = socket.GetWord(11);
	var targY = socket.GetWord(13);
	var targZ = socket.GetSByte(16);

	var iMulti = FindMulti( targX, targY, targZ, socket.currentChar.worldnumber );
	var NorthWall = CheckDynamicFlag( targX, targY - 1, targZ, socket.currentChar.worldnumber, mChar.instanceID, 4 );
	var WestWall = CheckDynamicFlag( targX - 1, targY, targZ, socket.currentChar.worldnumber, mChar.instanceID, 4 );

	if(ValidateObject( iMulti ) && !iMulti.IsBoat() && iMulti.IsInMulti( socket.currentChar )) 
	{
		if (ValidateObject( myTarget ))
		{
			// Check for nearby doors
			var foundDoor = AreaItemFunction("CheckForNearbyDoors", myTarget, 2, socket );
			if( foundDoor ) 
			{
				socket.SysMessage( GetDictionaryEntry( 1890, socket.language )); // You cannot lock down blocking items adjacent to a door.
				return false;
			}
		}

		if( iMulti.lockdowns < iMulti.maxLockdowns )
		{
			var addonID = AddonDisplayID( iUsed );
			var itemID = addonID[0];
			var itemID2 = addonID[1];

			var addonitem = CreateDFNItem( socket, mChar, "0x232D", 1, "ITEM", true );
			addonitem.Teleport( targX, targY, targZ );
			if( NorthWall )
			{
				addonitem.id = itemID;
			}
			else if( WestWall ) 
			{
				addonitem.id = itemID2;
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 5501, socket.language ));// The decoration must be placed next to a wall.
				addonitem.Delete();
			}
			addonitem.SetTag( "addon", 1 );
			addonitem.container = null;
			iMulti.LockDownItem( addonitem );
			iUsed.Delete();
		}
		else
		{
			socket.SysMessage(GetDictionaryEntry( 1895, socket.language )); // This house cannot have any additional lockdowns!
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 5502, socket.language )); // That location is not in your house.
	}
}

/*
The purpose of this function to determining itemID and itemID2 values based on the result of calling GetTag("addondeed") on the iUsed object.
If the result is 1, it sets specific values for itemID and itemID2; otherwise, they remain 0.
The function then returns an array containing these values. To a Gump and targetting cursor.
Example of Dedd you make and add addondeed tag to

customint=addondeed 2 < long as you change this number the case below needs to match up.
in the switch add case 2 and the two ids you want to be displayed on gump for west wall and north wall.
*/
function AddonDisplayID( iUsed )
{
	var itemID = 0;
	var itemID2 = 0;
	switch( iUsed.GetTag( "addondeed" ))
	{
		case 1: itemID = 0x232D; itemID2 = 0x232C; break;
		default: break;
	}

	return [itemID, itemID2];
}

function CheckForNearbyDoors( myTarget, itemToCheck, pSocket )
{
	if( ValidateObject( itemToCheck ))
	{
		if(( itemToCheck.type == 12 || itemToCheck.type == 13 ))
		{
			//if( itemToCheck.z > myTarget.z && itemToCheck.z - myTarget.z > GetTileHeight( myTarget ))
			if( itemToCheck.z > myTarget.z && itemToCheck.z - myTarget.z >= 20 ) 
			{
				// Ignore doors on floors above
				return false;
			}
			else if( itemToCheck.z < myTarget.z && myTarget.z - itemToCheck.z >= 20 ) 
			{
				// Ignore doors on floors below, too!
				return false;
			}

			if( itemToCheck.isDoorOpen )
			{
				// Make sure to check against the distance from the door in it's closed state, rather than it's open state!
				var origX = itemToCheck.x - itemToCheck.GetTag( "DOOR_X" );
				var origY = itemToCheck.y - itemToCheck.GetTag( "DOOR_Y" );
				if( myTarget.x - origX < 2 || origX - myTarget.x < 2 || myTarget.y - origY < 2 || origY - myTarget.y < 2)
				{
					return true;
				}
			}

			if( myTarget.DistanceTo( itemToCheck ) <= 2 ) 
			{
				return true;
			}
		}
	}
	return false;
}