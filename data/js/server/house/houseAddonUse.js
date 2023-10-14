// Used to trigger hard-coded targeting actions for some items that normally rely on their item
// type for functionality, but then lose that functionality when becoming a house addon, since
// all house addons end up with item type IT_HOUSEADDON
//
// Instead, this script is triggered via entries in jse_objectassociations.scp, and call on the
// hard-coded target requests that would normally be executed for the non-addon variant of the
// items
function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( iUsed ))
		return false;

	var pSocket = pUser.socket;

	switch( iUsed.id )
	{
		case 0x0faf: // anvil north/south (metalrepairtool)
		case 0x0fb0: // anvil east/west (metalrepairtool)
		case 0x2dd5: // elven anvil north/south (metalrepairtool)
		case 0x2dd6: // elven anvil east/west (metalrepairtool)
			pSocket.tempObj = iUsed;
			pUser.PopUpTarget( 4, GetDictionaryEntry( 485, pSocket.language )); // What item would you like to repair?
			break;
		case 0x0fb1: // small forge
		case 0x197a: // forge east/west, part 1
		case 0x197e: // forge east/west, part 2
		case 0x1982: // forge east/west, part 3
		case 0x1986: // forge north/south, part 1
		case 0x198a: // forge north/south, part 2
		case 0x198e: // forge north/south, part 3
		case 0x1992: // forge south/north, part 1
		case 0x1996: // forge south/north, part 2
		case 0x199a: // forge south/north, part 3
		case 0x199e: // forge west/east, part 1
		case 0x19a2: // forge west/east, part 2
		case 0x19a6: // forge west/east, part 3
		case 0x2dd8: // elven forge
			pSocket.tempObj = iUsed;
			pUser.PopUpTarget( 41, GetDictionaryEntry( 440, pSocket.language )); // What item would you like to smelt?
			break;
		case 0x1015: // spinning wheel north/south
		case 0x1019: // spinning wheel east/west
		case 0x101c: // spinning wheel south/north
		case 0x10a4: // spinning wheel west/east
			//iUsed.TextMessage( "Try spinning flax, cotton or wool on this spinning wheel", false, 0x3b2, 0, pUser.serial ); //GetDictionaryEntry( X, pSocket.language ), false, 0x3b2, 0, pUser.serial ); // You can weave yarn or thread on this loom
			pSocket.tempObj = iUsed;
			pUser.CustomTarget( 0, "What do you want to spin on this spinning wheel?" );
			break;
		case 0x105f: // loom north/south, part 1
		case 0x1060: // loom north/south, part 2
		case 0x1061: // loom east/west, part 1
		case 0x1062: // loom east/west, part 2
			//iUsed.TextMessage( "Try weaving yarn or thread on this loom", false, 0x3b2, 0, pUser.serial ); //GetDictionaryEntry( X, pSocket.language ), false, 0x3b2, 0, pUser.serial ); // You can weave yarn or thread on this loom
			pSocket.tempObj = iUsed;
			pUser.CustomTarget( 1, "What do you want to weave on this loom?" );
			break;
		default:
			// Hint about using axe to return addon to deed-form
			iUsed.TextMessage( GetDictionaryEntry( 9185, pSocket.language ), false, 0x3b2, 0, pUser.serial ); // You can use an axe to dismantle house addons and get their deeds back.
	}

	return false;
}

// Spinning Wheel functionality
function onCallback0( pSocket, myTarget )
{
	var pUser = pSocket.currentChar;
	var spinningWheel = pSocket.tempObj;
	var StrangeByte = pSocket.GetWord( 1 );

	// Make sure target is a valid item
	if( !ValidateObject( myTarget ) || !myTarget.isItem )
	{
		pSocket.SysMessage( "You may only spin flax, cotton or wool on a spinning wheel." );
		return;
	}

	// Make sure targeted item is in player's backpack
	var itemOwner = GetPackOwner( myTarget, 0 );
	if( itemOwner == null || itemOwner.serial != pUser.serial )
	{
		pUser.SysMessage( GetDictionaryEntry( 1763, pSocket.language )); // That item must be in your backpack before it can be used.
		return;
	}

	// Do something if a valid item was targeted
	switch( myTarget.id )
	{
		case 0x0def: // bale of cotton
		case 0x0df9: // bale of cotton
			pSocket.tempObj = myTarget;
			TriggerEvent( 132, "onCallback0", pSocket, spinningWheel );
			break;
		case 0x1a9c: // flax bundle
		case 0x1a9d: // flax bundle
			pSocket.tempObj = myTarget;
			TriggerEvent( 133, "onCallback0", pSocket, spinningWheel );
			break;
		case 0x0df8: // unspun wool
		case 0x101f: // unspun wool
			pSocket.tempObj = myTarget;
			TriggerEvent( 134, "onCallback0", pSocket, spinningWheel );
			break;
		default:
			pSocket.SysMessage( "You may only spin flax, cotton or wool on a spinning wheel." );
			break;
	}
}

// Loom functionality
function onCallback1( pSocket, myTarget )
{
	var pUser = pSocket.currentChar;
	var loom = pSocket.tempObj;
	var StrangeByte = pSocket.GetWord( 1 );

	// Make sure target is a valid item
	if( !ValidateObject( myTarget ) || !myTarget.isItem )
	{
		pSocket.SysMessage( "You may only weave yarn or thread on a loom." );
		return;
	}

	// Make sure targeted item is in player's backpack
	var itemOwner = GetPackOwner( myTarget, 0 );
	if( itemOwner == null || itemOwner.serial != pUser.serial )
	{
		pUser.SysMessage( GetDictionaryEntry( 1763, pSocket.language )); // That item must be in your backpack before it can be used.
		return;
	}

	// Do something if a valid item was targeted
	switch( myTarget.id )
	{
		case 0x0e1d: // ball of yarn
		case 0x0e1e: // ball of yarn
		case 0x0e1f: // ball of yarn
			pSocket.tempObj = myTarget;
			TriggerEvent( 135, "onCallback0", pSocket, loom );
			break;
		case 0x0fa0: // spool of thread
		case 0x0fa1: // spool of thread
			pSocket.tempObj = myTarget;
			TriggerEvent( 135, "onCallback0", pSocket, loom );
			break;
		default:
			pSocket.SysMessage( "You may only weave yarn or thread on a loom." );
			break;
	}
}
