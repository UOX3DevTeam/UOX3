function bowlHasFish( bowl )
{
	return ( bowl.GetTag( "vf_present" ) === 1);
}

function clearStoredFish( bowl )
{
	bowl.SetTag( "vf_present", 0 );
	bowl.SetTag( "vf_id", 0 );
	bowl.SetTag( "vf_hue", 0 );
	bowl.SetTag( "vf_name", "" );
	bowl.SetTag( "vf_dead", 0 );
	bowl.SetTag( "vf_extra", "" );
}

function storeFishFromItem( bowl, fish )
{
	// Copy core look
	bowl.SetTag( "vf_id",  fish.id );
	bowl.SetTag( "vf_hue", fish.color );
	bowl.SetTag( "vf_name", fish.name );

	// Small whitelist of extra tags you might care about
	var dead = fish.GetTag( "dead" );
	bowl.SetTag( "vf_dead", dead );

	var extras = [];
	extras.push( "isAquariumFish=1" ); // always re-apply
	var species = fish.GetTag( "species" );
	if( typeof species !== "undefined" && species !== null && species !== "" )
		extras.push( "species=" + String( species ));

	bowl.SetTag( "vf_extra", extras.join( "," ));

	bowl.SetTag( "vf_present", 1 );
}

function recreateFishToPack( pUser, bowl )
{

	var id   = bowl.GetTag( "vf_id" );
	var hue  = bowl.GetTag( "vf_hue" );
	var name = bowl.GetTag( "vf_name" );
	name = ( typeof name === "string" ? name : "" );

	if( id <= 0 )
		return false;

	// Create a plain item with the same art
	var fishItem = CreateBlankItem( pUser.socket, pUser, 1, name, id, hue, "ITEM", true  );
	if( !ValidateObject( fishItem ))
		return false;

	if( hue > 0 ) 
		fishItem.color = hue;
	if( name !== "" )
		fishItem.name = name;

	fishItem.SetTag( "fishAirActive", 1 );
	fishItem.AddScriptTrigger( 5101 );
	fishItem.StartTimer( 300000, 1, 5101 );

	fishItem.SetTag( "isAquariumFish", 1 );
	var dead = bowl.GetTag( "vf_dead" ) | 0;
	if( dead === 1 )
		fishItem.SetTag( "dead", 1 );

	var extra = bowl.GetTag( "vf_extra" );
	if( typeof extra === "string" && extra !== "" )
	{
		var parts = extra.split( "," );
		for ( var i = 0; i < parts.length; i++ )
		{
			var kv = parts[i].split( "=" );
			if( kv.length === 2 )
			{
				var k = kv[0], v = kv[1];
				if( k === "isAquariumFish" )
					fishItem.SetTag( k, v | 0 );
				else if( k === "species" ) 
					fishItem.SetTag( k, String( v ));
			}
		}
	}

	fishItem.PlaceInPack( pUser );
	bowl.Refresh();

	return true;
}

/** @type { ( thingCreated: BaseObject, thingType: 0 | 1 ) => void } */
function onCreateDFN( objMade, objType )
{
	if( objType === 0 )
	{
		objMade.id    = 0x241C;
		objMade.color = 0x047E;

		objMade.SetTag( "isFishBowl", 1 );

		if( !bowlHasFish( objMade ))
			clearStoredFish( objMade );
	}
}

/** @type { ( item: Item, dropper: Character, dest: Item ) => number } */
function onDropItemOnItem( iDropped, cDropper, iDroppedOn )
{
	if( !ValidateObject( iDroppedOn ))
		return 1;

	if( iDropped.id === 0x241C )
		return 1;

	if( iDroppedOn.id !== 0x241C )
		return 1;

	if( bowlHasFish( iDroppedOn ))
	{
		cDropper.SysMessage( "The fish bowl already contains a creature." );
		return 1;
	}

	var isFish = ( iDropped.GetTag( "isAquariumFish" ));
	if( isFish !== 1 )
	{
		cDropper.SysMessage( "The container can not hold that type of object." );
		return 1;
	}

	storeFishFromItem( iDroppedOn, iDropped );
	iDropped.Delete();

	cDropper.SysMessage( "You place the creature into the fish bowl." );
	if( typeof iDroppedOn.Refresh === "function" )
		iDroppedOn.Refresh(  );

	return 1;
}

/** @type { ( tSock: Socket, baseObj: BaseObject ) => boolean } */
function onContextMenuRequest( socket, targObj )
{
	if( !ValidateObject( targObj ))
		return true;

	var list = [];
	if( bowlHasFish( targObj ))
	{
		list.push( { id: 6242, text: 6242, flags: 0x0020, hue: 0x03E0 } );
	}
	var useKR = false;
	TriggerEvent( 18001, "modifyContextMenu", socket, targObj, list, useKR );
	return false;
}

/** @type { ( tSock: Socket, baseObj: BaseObject, popupEntry: number ) => boolean } */
function onContextMenuSelect( socket, targObj, entryID )
{
	if( !ValidateObject( targObj ))
		return false;

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser )) 
		return false;

	switch( entryID | 0 )
	{
		case 6242: // Remove creature
		{
			if( !bowlHasFish( targObj ))
				return false;

			if( !recreateFishToPack( pUser, targObj ))
			{
				socket.SysMessage( "Could not recreate the creature." );
				return false;
			}

			clearStoredFish( targObj );
			socket.SysMessage( "The creature has been removed from the fish bowl." );
			targObj.Refresh();
			break;
		}
	}
	return false;
}

/** @type { ( myObj: BaseObject, pSocket: Socket ) => string } */
function onTooltip( myObj, pSocket )
{
	var tooltipText  = ["A fish bowl"];

	if( bowlHasFish( myObj ))
	{
		var name = myObj.GetTag( "vf_name" );
		if( typeof name !== "string" || name === "" )
		{
			// fallback generic name
			tooltipText .push( "Contains: an aquarium creature" );
		}
		else
		{
			tooltipText .push( "Contains: " + name );
		}
	}
	return tooltipText.join( "\n" );
}
