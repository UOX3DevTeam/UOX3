/// <reference path="../definitions.d.ts" />
// @ts-check

const chargeItemIDReq = 0x1f60;
const chargeItemSectionIDReq = "0x1f60";
const chargeAmount = 5;
const chargeMax = 1000;
const teleporterIDReq = [0x40BB, 0x574A];
const teleporterSectionIDReq = "house_teleporter";
const coOwnHousesOnSameAccount = GetServerSetting( "CoOwnHousesOnSameAccount" );

/** @type { ( item: Item | BaseObject ) => boolean } */
function IsTeleporterItem( item )
{
	if( !item  || !item.isItem )
		return false;

	// allow by sectionid
	if( item.sectionID == teleporterSectionIDReq )
		return true;

	// allow by id list
	for( var i = 0; i < teleporterIDReq.length; i++ )
	{
		if( item.id == teleporterIDReq[i] )
			return true;
	}
	return false;
}

/** @type { ( mode: number ) => string } */
function SecurityName( mode )
{
	if( mode == 1 )
		return "Friends";
	if( mode == 2 )
		return "Anyone";
	return "Owner/Co-Owners";
}

/** @type { ( pChar: Character, teleItem: Item ) => boolean } */
function CanManageTeleporter( pChar, teleItem )
{
	// Who can change security setting?
	// (GM OR owner OR co-owner OR same-account-as-owner if enabled)
	if( !ValidateObject( pChar ) || !ValidateObject( teleItem ))
		return false;

	if( pChar.isGM )
		return true;

	var multiHouse = teleItem.multi;
	if( !ValidateObject( multiHouse ))
		return false;

	if( multiHouse.IsOwner( pChar ))
		return true;

	if( multiHouse.IsOnOwnerList( pChar ))
		return true;

	if( coOwnHousesOnSameAccount && ValidateObject( multiHouse.owner ))
	{
		if( multiHouse.owner.accountNum == pChar.accountNum )
			return true;
	}

	return false;
}

/** @type { ( pChar: Character, teleItem: Item ) => boolean } */
function CanUseHouseTeleporter( pChar, teleItem )
{
	// Enforces configured security
	if( !ValidateObject( pChar ) || !ValidateObject( teleItem ))
		return false;

	if( pChar.isGM )
		return true;

	var multiHouse = teleItem.multi;
	if( !ValidateObject( multiHouse ))
		return false;

	var mode = teleItem.GetTag( "Security" );
	if( mode != 0 && mode != 1 && mode != 2 )
		mode = 0;

	// Owner/coowner always allowed in all modes
	if( multiHouse.IsOwner( pChar ))
		return true;

	if( multiHouse.IsOnOwnerList( pChar ))
		return true;

	if( coOwnHousesOnSameAccount && ValidateObject(multiHouse.owner ))
	{
		if( multiHouse.owner.accountNum == pChar.accountNum )
			return true;
	}

	// Friends mode
	if( mode == 1 )
	{
		if( multiHouse.IsOnFriendList( pChar ))
			return true;
		return false;
	}

	// Anyone mode
	if( mode == 2 )
	{
		return true;
	}

	// Owner/Co-Owners only
	return false;
}

/** @type { ( tSock: Socket, baseObj: BaseObject ) => boolean } */
function onContextMenuRequest( socket, targObj )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return true;

	if( !IsTeleporterItem( targObj ))
		return true;

	var inPack = false;
	if( ValidateObject( pUser ) && ValidateObject( pUser.pack ))
	{
		var root0 = FindRootContainer( targObj, 0 );
		inPack = ( ValidateObject( root0 ) && root0.isItem && root0.serial == pUser.pack.serial );
	}

	// Show "Set Security" only when it's a house-placed (locked down) teleporter
	var canShowSecurity = false;
	if( targObj.movable == 3 && ValidateObject( targObj.multi ) && CanManageTeleporter( pUser, targObj ))
		canShowSecurity = true;

	// Show "Rename" only when locked down, in house, manageable, AND linked
	var canShowRename = false;
	if( targObj.movable == 3 && ValidateObject( targObj.multi ) && CanManageTeleporter( pUser, targObj ))
	{
		var linkSerTmp = targObj.GetTag( "LinkSer" );
		if( linkSerTmp > 0 )
		{
			var otherTmp = CalcItemFromSer( linkSerTmp );
			if( IsTeleporterItem( otherTmp ) && otherTmp.movable == 3 )
				canShowRename = true;
		}
	}

	var numEntries = 1; // Status
	if( inPack )
	{
		numEntries++;
	}

	if( targObj.id == 0x574A )
	{
		numEntries++;
	}

	if( canShowSecurity )
		numEntries++;

	if( canShowRename )
	numEntries++;

	var offset = 12;
	var toSend = new Packet();
	var packetLen = 12 + ( numEntries * 8 );

	toSend.ReserveSize( packetLen );
	toSend.WriteByte( 0, 0xBF );
	toSend.WriteShort( 1, packetLen );
	toSend.WriteShort( 3, 0x14 );   // subCmd
	toSend.WriteShort( 5, 0x0001 ); // 2D client
	toSend.WriteLong( 7, targObj.serial );
	toSend.WriteByte( 11, numEntries );

	// Entry: Status
	toSend.WriteShort( offset, 0x0101 );
	toSend.WriteShort( offset += 2, 2140 );
	toSend.WriteShort( offset += 2, 0x0020 );
	toSend.WriteShort( offset += 2, 0x03E0 );
	offset += 2;

	// Entry: Link (only if in backpack)
	if( inPack )
	{
		toSend.WriteShort( offset, 0x0102 );
		toSend.WriteShort( offset += 2, 5119 );
		toSend.WriteShort( offset += 2, 0x0020 );
		toSend.WriteShort( offset += 2, 0x03E0 );
		offset += 2;
	}

	// Entry: Recharge (pink only)
	if( targObj.id == 0x574A )
	{
		toSend.WriteShort( offset, 0x0103 );
		toSend.WriteShort( offset += 2, 5042 );
		toSend.WriteShort( offset += 2, 0x0020 );
		toSend.WriteShort( offset += 2, 0x03E0 );
		offset += 2;
	}

	// Set Security
	if( canShowSecurity )
	{
		toSend.WriteShort( offset, 0x0104 );
		toSend.WriteShort( offset += 2, 6203 );
		toSend.WriteShort( offset += 2, 0x0020 );
		toSend.WriteShort( offset += 2, 0x03E0 );
		offset += 2;
	}

	// Rename (linked + placed only)
	if( canShowRename )
	{
		toSend.WriteShort( offset, 0x0105 );
		toSend.WriteShort( offset += 2, 404 );     // cliloc for name
		toSend.WriteShort( offset += 2, 0x0020 );
		toSend.WriteShort( offset += 2, 0x03E0 );
		offset += 2;
	}

	socket.Send( toSend );
	toSend.Free();

	return false;
}

/** @type { ( tSock: Socket, baseObj: BaseObject, popupEntry: number ) => boolean } */
function onContextMenuSelect( socket, targObj, popupEntry )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return true;

	if( !IsTeleporterItem( targObj ))
		return true;

	if( popupEntry == 0x0101 )
	{
		var linkSer = targObj.GetTag( "LinkSer" );

		var linked = false;
		if( linkSer > 0 )
		{
			var other = CalcItemFromSer( linkSer );
			if( IsTeleporterItem( other ))
				linked = true;
		}

		if( targObj.id == 0x574A )
		{
			var charge = targObj.GetTag( "Charges" );
			if( charge < 0 )
				charge = 0;

			if( charge > chargeMax )
				charge = chargeMax;

			socket.SysMessage( "Teleporter: " + ( linked ? "Linked" : "Unlinked" ) + " | Charges: " + charge + "/" + chargeMax );
		}
		else
		{
			socket.SysMessage( "Teleporter: " + ( linked ? "Linked" : "Unlinked" ));
		}
		return false;
	}

	if( popupEntry == 0x0102 )
	{
		var inPackLink = false;
		if( ValidateObject( pUser.pack ))
		{
			var root1 = FindRootContainer( targObj, 0 );
			inPackLink = ( ValidateObject( root1 ) && root1.isItem && root1.serial == pUser.pack.serial );
		}

		if( !inPackLink )
		{
			if (socket != null)
				socket.SysMessage( GetDictionaryEntry( 30600, socket.language ));
			return false;
		}

		pUser.SetTempTag( "LinkSrcSer", targObj.serial );
		if( socket != null )
			socket.CustomTarget( 0, GetDictionaryEntry( 30601, socket.language )); // Target the other teleporter in your backpack to link.
		targObj.Refresh();
		return false;
	}

	if( popupEntry == 0x0103 )
	{
		if( targObj.GetTag( "chargeable" ) != 1 )
			return false;

		var cur = targObj.GetTag( "Charges" );
		if( cur < 0 )
			cur = 0;

		if( cur >= chargeMax )
		{
			if( socket != null )
				socket.SysMessage( GetDictionaryEntry( 30602, socket.language ));
			return false;
		}

		pUser.SetTempTag( "RechargeSer", targObj.serial );
		if( socket != null )
			socket.CustomTarget( 1, GetDictionaryEntry( 30603, socket.language )); // Target recharge item in your backpack to recharge.
		targObj.Refresh();
		return false;
	}

	
	if( popupEntry == 0x0104 )
	{
		// Must be allowed to manage
		if( !( targObj.movable == 3 && ValidateObject( targObj.multi ) && CanManageTeleporter( pUser, targObj )))
		{
			if( socket != null )
				socket.SysMessage( GetDictionaryEntry( 30604, socket.language )); // You cannot change this teleporter's security.
			return false;
		}

		var curMode = targObj.GetTag( "Security" );
		if( curMode != 0 && curMode != 1 && curMode != 2 )
			curMode = 0;
		var nextMode = curMode + 1;
		if( nextMode > 2 )
			nextMode = 0;

		targObj.SetTag( "Security", nextMode );
		if( socket != null )
			socket.SysMessage( "Teleporter security set to: " + SecurityName( nextMode )); // Teleporter security set to: %s
		targObj.Refresh();
		return false;
	}

	if( popupEntry == 0x0105 )
	{
		// Must be placed + manageable
		if(!( targObj.movable == 3 && ValidateObject( targObj.multi ) && CanManageTeleporter( pUser, targObj )))
		{
			if( socket != null )
				socket.SysMessage( GetDictionaryEntry( 30606, socket.language )); // You cannot rename this teleporter.
			return false;
		}

		// Must be linked
		var linkSer = targObj.GetTag( "LinkSer" );
		if( linkSer <= 0 )
		{
			if( socket != null )
				socket.SysMessage( GetDictionaryEntry( 30607, socket.language )); // This teleporter must be linked before it can be renamed.
			return false;
		}

		var other = CalcItemFromSer( linkSer );
		if( !IsTeleporterItem( other ))
		{
			if( socket != null )
				socket.SysMessage( GetDictionaryEntry( 30607, socket.language )); // This teleporter must be linked before it can be renamed.
			return false;
		}

		// Store the serial so onSpeechInput knows what to rename
		pUser.SetTempTag( "RenameSer", targObj.serial );

		// Prompt for name
		pUser.SpeechInput( 1, targObj );
		return false;
	}

	return false;
}

/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;
	if( pSocket == null ) 
		return false;

	if( !IsTeleporterItem( iUsed ))
		return false;

	var inPackUse = false;
	if( ValidateObject( pUser ) && ValidateObject( pUser.pack ))
	{
		var root2 = FindRootContainer( iUsed, 0 );
		inPackUse = ( ValidateObject( root2 ) && root2.isItem && root2.serial == pUser.pack.serial );
	}

	if( !inPackUse )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30608, pSocket.language )); // To link, both teleporters must be in your backpack.
		return false;
	}

	pUser.SetTempTag( "LinkSrcSer", iUsed.serial );
	pSocket.CustomTarget( 0, GetDictionaryEntry( 30609, pSocket.language )); // Target the other teleporter in your backpack to link.
	return false;
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback0( socket, target )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	if( socket.GetWord( 1 ))
		return;

	var srcSer = pUser.GetTempTag( "LinkSrcSer" );
	var teleporterA = CalcItemFromSer( srcSer );
	var teleporterB = target;

	if( !IsTeleporterItem( teleporterA ))
	{
		if( socket != null )
			socket.SysMessage( GetDictionaryEntry( 30610, socket.language )); // Source teleporter not found.
		return;
	}
	if( !IsTeleporterItem( teleporterB ))
	{
		if( socket != null )
			socket.SysMessage( GetDictionaryEntry( 30611, socket.language )); // That is not a house teleporter.
		return;
	}
	if( teleporterA.serial == teleporterB.serial )
	{
		if( socket != null )
		socket.SysMessage( GetDictionaryEntry( 30612, socket.language )); // You must target the other teleporter.
		return;
	}
	if( teleporterA.id != teleporterB.id )
	{
		if( socket != null )
			socket.SysMessage( GetDictionaryEntry( 30613, socket.language )); // These teleporters are different types and cannot be linked.
		return;
	}

	var aInPack = false;
	var bInPack = false;

	if( ValidateObject( pUser ) && ValidateObject( pUser.pack ))
	{
		var rootA = FindRootContainer( teleporterA, 0 );
		var rootB = FindRootContainer( teleporterB, 0 );

		aInPack = ( ValidateObject( rootA ) && rootA.isItem && rootA.serial == pUser.pack.serial );
		bInPack = ( ValidateObject( rootB ) && rootB.isItem && rootB.serial == pUser.pack.serial );
	}

	if( !aInPack || !bInPack )
	{
		if( socket != null )
			socket.SysMessage( GetDictionaryEntry( 30614, socket.language )); // Both teleporters must be in your backpack to link.
		return;
	}

	UnlinkOther( teleporterA, teleporterB.serial );
	UnlinkOther( teleporterB, teleporterA.serial );

	teleporterA.SetTag( "LinkSer", teleporterB.serial );
	teleporterB.SetTag( "LinkSer", teleporterA.serial );
	if( socket != null )
		socket.SysMessage( GetDictionaryEntry( 30615, socket.language )); // Teleporters linked.
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback1( socket, target )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	if( socket.GetWord( 1 ))
		return;

	var teleporterSerial = pUser.GetTempTag( "RechargeSer" );
	var teleporter = CalcItemFromSer( teleporterSerial );
	if( !ValidateObject( teleporter ) || !teleporter.isItem || teleporter.GetTag( "chargeable" ) != 1 )
		return;

	var scroll = target;
	if( !( ValidateObject( scroll ) && scroll.isItem && ( scroll.id == chargeItemIDReq || scroll.sectionID == chargeItemSectionIDReq )))
	{
		if( socket != null )
			socket.SysMessage(GetDictionaryEntry( 30616, socket.language ));
		return;
	}

	var scrollInPack = false;
	if( ValidateObject( pUser ) && ValidateObject( pUser.pack ))
	{
		var root3 = FindRootContainer( scroll, 0 );
		scrollInPack = ( ValidateObject( root3 ) && root3.isItem && root3.serial == pUser.pack.serial );
	}

	if( !scrollInPack )
	{
		if( socket != null )
			socket.SysMessage( GetDictionaryEntry( 30617, socket.language )); // This item must be in your backpack.
		return;
	}

	var cur = teleporter.GetTag( "Charges");
	if( cur < 0 )
		cur = 0;

	var stackAmt = ( scroll.amount );
	if( stackAmt < 1 )
		stackAmt = 1;

	var remaining = chargeMax - cur;
	if( remaining <= 0 )
	{
		if( socket != null )
			socket.SysMessage( GetDictionaryEntry( 30618, socket.language )); // The House Teleporter cannot be charged any further.
		return;
	}

	var needScrolls = Math.ceil( remaining / chargeAmount );
	var useScrolls = needScrolls;

	if( useScrolls > stackAmt )
		useScrolls = stackAmt;
	if( useScrolls < 1 )
		useScrolls = 1;

	// consume that many from the stack
	if( stackAmt > useScrolls )
	{
		scroll.amount = stackAmt - useScrolls;
	}
	else
	{
		scroll.Delete();
	}

	cur += ( useScrolls * chargeAmount );
	if( cur > chargeMax )
		cur = chargeMax;

	teleporter.SetTag( "Charges", cur );
	if( socket != null )
		socket.SysMessage( GetDictionaryEntry( 30619, socket.language )); // The recharge item crumbles to dust as it strengthens the House Teleporter.
	teleporter.Refresh();
}

/** @type { ( tile: Item, keepSer: number ) => void } */
function UnlinkOther( tile, keepSer )
{
	var old = tile.GetTag( "LinkSer");
	tile.SetTag( "LinkSer", null );

	if( old > 0 && old != keepSer )
	{
		var oldtele = CalcItemFromSer( old );
		if( IsTeleporterItem( oldtele ))
			oldtele.SetTag( "LinkSer", null );
	}
}

/** @type { ( targSock: Socket, objColliding: Character, objCollideWith: BaseObject ) => boolean } */
function onCollide( trgSock, pColliding, objCollidedWith )
{
	if( !ValidateObject( pColliding ) || !pColliding.isChar )
		return false;

	if( !IsTeleporterItem( objCollidedWith ))
		return false;

	if( trgSock == null )
		return false;

	if( objCollidedWith.movable != 3 )
	{
		if( trgSock )
			trgSock.SysMessage( GetDictionaryEntry( 30620, trgSock.language )); // This must be locked down in a house to function.
		return false;
	}

	var iMulti = FindMulti( objCollidedWith.x, objCollidedWith.y, objCollidedWith.z, objCollidedWith.worldnumber, objCollidedWith.instanceID );
	var insideHouse = ( ValidateObject( iMulti ) && iMulti.IsInMulti( objCollidedWith ) );

	if( !insideHouse )
	{
		if( trgSock )
			trgSock.SysMessage( GetDictionaryEntry( 30621, trgSock.language ) ); // This must be placed inside a house.
		return false;
	}

	// SECURITY: Owner + Co-Owners only
	if(!CanUseHouseTeleporter(pColliding, objCollidedWith))
	{
		if( trgSock )
			trgSock.SysMessage( GetDictionaryEntry( 30622, trgSock.language )); // Only the house owner and co-owners may use this teleporter.
		return false;
	}

	if( trgSock )
	{
		if( pColliding.dead || pColliding.criminal )
		{
			trgSock.SysMessage( GetDictionaryEntry( 30623, trgSock.language )); // You cannot use that right now.
			return false;
		}
	}
	
	var linkSer = objCollidedWith.GetTag( "LinkSer" );
	if( linkSer <= 0 )
	{
		if( trgSock ) 
			trgSock.SysMessage( GetDictionaryEntry( 30624, trgSock.language )); // This teleporter is not linked.
		return false;
	}

	var other = CalcItemFromSer( linkSer );
	if( !IsTeleporterItem( other ))
	{
		if( trgSock )
			trgSock.SysMessage( GetDictionaryEntry( 30624, trgSock.language )); // This teleporter is not linked.
		return false;
	}

	if( other.movable != 3 )
	{
		if( trgSock )
			trgSock.SysMessage( GetDictionaryEntry( 30625, trgSock.language )); // The destination teleporter is not properly placed.
		return false;
	}

	if( !IsInBuilding( other.x, other.y, other.z, other.worldnumber, other.instanceID, true ))
	{
		if( trgSock )
			trgSock.SysMessage( GetDictionaryEntry( 30625, trgSock.language )); // The destination teleporter is not properly placed.
		return false;
	}

	if( objCollidedWith.id == 0x574A )
	{
		var charge = objCollidedWith.GetTag( "Charges" );
		if( charge < 0 )
			charge = 0;

		if( charge <= 0 )
		{
			if( trgSock )
				trgSock.SysMessage( GetDictionaryEntry( 30626, trgSock.language )); // There are no charges left in this teleporter.
			return false;
		}

		objCollidedWith.SetTag( "Charges", charge - 1 );
	}

	pColliding.Teleport( other.x, other.y, other.z, other.worldnumber );
	return false;
}

/** @type { ( myChar: Character, myItem: Item, mySpeech: string, mySpeechId: number ) => void } */
function onSpeechInput( pUser, targObj, pSpeech, pSpeechID )
{
	var pSocket = pUser.socket;
	if( !pSocket )
		return;

	if( pSpeechID != 1 )
		return;

	// Basic validation
	if( pSpeech == null )
		return;

	pSpeech = ("" + pSpeech);

	// trim (simple ES5)
	pSpeech = pSpeech.replace(/^\s+|\s+$/g, "");
	if( pSpeech.length <= 0 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 9270, pSocket.language )); // too short / none entered
		return;
	}
	if( pSpeech.length > 60 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 9271, pSocket.language )); // too long (we'll reuse)
		return;
	}

	// Resolve which teleporter we are renaming
	var srcSer = pUser.GetTempTag( "RenameSer" );
	var teleA = CalcItemFromSer( srcSer );
	if( !IsTeleporterItem( teleA ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 30628, pSocket.language )); // Teleporter not found.
		return;
	}

	// Must still be linked
	var linkSer = teleA.GetTag( "LinkSer" );
	if( linkSer <= 0 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 30629, pSocket.language )); // This teleporter must be linked before it can be renamed.
		return;
	}

	var teleB = CalcItemFromSer( linkSer );
	if( !IsTeleporterItem( teleB ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 30629, pSocket.language )); // This teleporter must be linked before it can be renamed.
		return;
	}

	// Apply name to BOTH
	teleA.name = pSpeech;
	teleB.name = pSpeech;

	teleA.Refresh();
	teleB.Refresh();

	pSocket.SysMessage( "Teleporter name set to: " + pSpeech ); // Teleporter name set to: %s
}

/** @type { ( myObj: BaseObject, pSocket: Socket ) => string } */
function onTooltip( myObj, pSocket )
{
	if( !ValidateObject( myObj ) || !myObj.isItem )
		return "";

	if( !IsTeleporterItem( myObj ))
		return "";

	var parts = [];
	var secMode = myObj.GetTag( "Security" );
	if( secMode != 0 && secMode != 1 && secMode != 2 )
		secMode = 0;

	parts.push( "Security: " + SecurityName( secMode ));

	if( myObj.id == 0x574A )
	{
		var charge = myObj.GetTag( "Charges" );
		if( charge < 0 )
			charge = 0;

		if( charge > chargeMax )
			charge = chargeMax;

		parts.push( "Charges: " + charge + "/" + chargeMax );
	}

	var linkSer = myObj.GetTag( "LinkSer" );
	var linked = false;
	if( linkSer > 0 )
	{
		var other = CalcItemFromSer( linkSer );
		if( IsTeleporterItem( other ))
			linked = true;
	}
	parts.push( "Link: " + ( linked ? "Linked" : "Unlinked" ));

	return parts.join( "<br>" );
}