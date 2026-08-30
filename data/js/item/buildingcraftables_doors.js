/// <reference path="../definitions.d.ts" />
// @ts-check
/** @type { ( thingCreated: BaseObject, thingType: 0 | 1 ) => void } */
function onCreateDFN( iCreated, dfnSection )
{
	iCreated.SetTag( "BuildingCraftableDoor", 1 );
}

/** @type { ( myObj: BaseObject, pSocket: Socket ) => string } */
function onTooltip( myObj, pSocket )
{
	return "House Only";
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

	var isBuilding =  iDropped.GetTag( "BuildingCraftableDoor" ) === 1;
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

	var isBuilding = iDropped.GetTag( "BuildingCraftableDoor" ) === 1;
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