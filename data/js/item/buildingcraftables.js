/// <reference path="../definitions.d.ts" />
// @ts-check
/** @type { ( thingCreated: BaseObject, thingType: 0 | 1 ) => void } */
function onCreateDFN( iCreated, dfnSection )
{
	setLockedState( iCreated, false );
	iCreated.SetTag( "BuildingCraftable", 1 );
}

/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;

	if( iUsed.container !== null )
	{
		if( iUsed.container === pUser.pack )
		{
			if (pSocket)
				pSocket.SysMessage( "Place the wall in your house, then double-click it to lock it down." );
		}
		else
		{
			if( pSocket )
				pSocket.SysMessage( "You must first place this wall in the world." );
		}
		return false;
	}

	if( !isInOwnHouse( pUser ))
	{
		if( pSocket )
		{
			pSocket.SysMessage( GetDictionaryEntry( 2067, pSocket.language )); // You must be in your house to do this.
		}
		return false;
	}

	var iMulti = iUsed.multi;
	if( !ValidateObject( iMulti ) || iMulti !== pUser.multi )
	{
		if( pSocket )
			pSocket.SysMessage( "This crafted wall must be inside your house to lock it down." );
		return false;
	}

	var locked = iUsed.GetTag("CraftWallLocked");

	if( locked == 1 )
	{
		setLockedState( iUsed, false );
		if( pSocket )
			pSocket.SysMessage("You unlock the crafted wall. It can now be moved and will decay normally.");
	}
	else
	{
		setLockedState( iUsed, true );
		if( pSocket )
			pSocket.SysMessage("You lock the crafted wall in place. It will no longer decay.");
	}

	return false;
}

/** @type { ( myObj: BaseObject, pSocket: Socket ) => string } */
function onTooltip( myObj, pSocket )
{
	var locked = myObj.GetTag( "CraftWallLocked" );

	if( locked == 1 )
	{
		// Line 1: House Only
		// Line 2: unlock hint
		return "House Only<br>You must double click this to unlock it.";
	}
	else
	{
		// Line 1: House Only
		// Line 2: lock hint
		return "House Only<br>You must double click this to lock it down.";
	}
}

function onDropItemOnNpc( pDropper, pDroppedOn, iDropped )
{
	var targPack = pDroppedOn.pack;
	if( ValidateObject( targPack ) && pDroppedOn.sectionID == "packhorse" )
	{
		return 0;
	}

	return 1;
}

function onDropItemOnItem( iDropped, cDropper, iDroppedOn )
{
	if( !ValidateObject( iDropped ) || !ValidateObject( cDropper ) || !ValidateObject( iDroppedOn ))
		return 0;

	var isBuilding =  iDropped.GetTag( "BuildingCraftable" ) === 1;
	if( !isBuilding )
		return 0

	var owner = iDroppedOn.container;
	if( ValidateObject( owner ) && owner.isChar && owner.sectionID == "packhorse" )
	{
		var sock = cDropper.socket;
		if( sock )
			sock.SysMessage( "You cannot place building pieces on a pack animal." );

		return 0;
	}

	return 1;
}

function onDrop( iDropped, pDropper )
{
	if( !ValidateObject( iDropped ) || !ValidateObject( pDropper ))
		return 0;

	var isBuilding = iDropped.GetTag( "BuildingCraftable" ) === 1;
	if( !isBuilding )
		return 0;

	var socket = pDropper.socket;
	if( !isInOwnHouse( pDropper ))
	{
		if( socket != null)
			socket.SysMessage( "The building piece crumbles when dropped on the ground." );

		iDropped.Delete();
		return 2;
	}

	return 1;
}


function isInOwnHouse( pUser )
{
	if( !ValidateObject( pUser ))
		return false;

	var iMulti = pUser.multi;
	if( !ValidateObject( iMulti ))
		return false;

	// Owner / co-owner / whatever IsOnOwnerList covers
	if( iMulti.IsOnOwnerList( pUser ))
		return true;

	// Optional: co-owned houses on same account
	if( GetServerSetting( "COOWNHOUSESONSAMEACCOUNT" ))
	{
		if( ValidateObject( iMulti.owner ) && iMulti.owner.accountNum === pUser.accountNum )
			return true;
	}

	return false;
}

function setLockedState( item, locked )
{
	if( !ValidateObject( item ))
		return;

	if( locked )
	{
		item.SetTag( "CraftWallLocked", 1 );
		item.movable = 2;		// locked down / secure
		item.decayable = false; // no decay
	}
	else
	{
		item.SetTag( "CraftWallLocked", 0 );
		item.movable = 1;      // movable
		item.decayable = true; // normal decay
	}
	item.Refresh();
}