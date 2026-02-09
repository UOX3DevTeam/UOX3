/// <reference path="../definitions.d.ts" />
// @ts-check

const chargeItemIDReq = 0x1f60;
const chargeItemSectionIDReq = "0x1f60";
const chargeAmount = 5;
const chargeMax = 1000;

const coOwnHousesOnSameAccount = GetServerSetting("CoOwnHousesOnSameAccount");

function IsTeleporterItem( item )
{
	return ( ValidateObject( item ) && item.isItem && (item.id == 0x40BB || item.id == 0x574A));
}

function IsScrollItem( item )
{
	return( ValidateObject(item) && item.isItem && ( item.id == chargeItemIDReq || item.sectionID == chargeItemSectionIDReq ));
}

function IsInPlayersPack( pUser, item )
{
	if( !ValidateObject( pUser ) || !ValidateObject( pUser.pack ) || !IsTeleporterItem( item ) && !IsScrollItem( item ))
		return false;

	var root = FindRootContainer( item, 0 );
	return( ValidateObject( root ) && root.isItem && root.serial == pUser.pack.serial );
}

function ReadIntTag( obj, tagName, defVal )
{
	var value = parseInt(obj.GetTag(tagName), 10);
	if( !isFinite( value ))
		value = defVal;
	return value;
}

function GetSecurityMode( teleItem )
{
	var m = ReadIntTag( teleItem, "HT_Security", 0 );
	if( m != 0 && m != 1 && m != 2 )
		m = 0;
	return m;
}

function SecurityName( mode )
{
	if( mode == 1 )
		return "Friends";
	if( mode == 2 )
		return "Anyone";
	return "Owner/Co-Owners";
}

function CanManageTeleporter( pChar, teleItem )
{
	// Who can change security setting?
	// (GM OR owner OR co-owner OR same-account-as-owner if enabled)
	if( !ValidateObject( pChar ) || !ValidateObject( teleItem ))
		return false;

	if( pChar.isGM )
		return true;

	var m = teleItem.multi;
	if( !ValidateObject( m ))
		return false;

	if( m.IsOwner( pChar ))
		return true;

	if( m.IsOnOwnerList( pChar ))
		return true;

	if( coOwnHousesOnSameAccount && ValidateObject( m.owner ))
	{
		if( m.owner.accountNum == pChar.accountNum )
			return true;
	}

	return false;
}

function CanUseHouseTeleporter( pChar, teleItem )
{
	// Enforces configured security
	if( !ValidateObject( pChar ) || !ValidateObject( teleItem ))
		return false;

	if( pChar.isGM )
		return true;

	var m = teleItem.multi;
	if( !ValidateObject( m ))
		return false;

	var mode = GetSecurityMode( teleItem );

	// Owner/coowner always allowed in all modes
	if( m.IsOwner( pChar ))
		return true;

	if( m.IsOnOwnerList( pChar ))
		return true;

	if( coOwnHousesOnSameAccount && ValidateObject(m.owner ))
	{
		if( m.owner.accountNum == pChar.accountNum )
			return true;
	}

	// Friends mode
	if( mode == 1 )
	{
		if(m.IsOnFriendList( pChar ))
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

	var inPack = IsInPlayersPack( pUser, targObj );

	// Show "Set Security" only when it's a house-placed (locked down) teleporter
	var canShowSecurity = false;
	if( targObj.movable == 3 && ValidateObject( targObj.multi ) && CanManageTeleporter( pUser, targObj ))
		canShowSecurity = true;

	// Show "Rename" only when locked down, in house, manageable, AND linked
	var canShowRename = false;
	if( targObj.movable == 3 && ValidateObject( targObj.multi ) && CanManageTeleporter( pUser, targObj ))
	{
		var linkSerTmp = ReadIntTag( targObj, "HT_LinkSer", 0 );
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
		var linkSer = ReadIntTag( targObj, "HT_LinkSer", 0 );

		var linked = false;
		if( linkSer > 0 )
		{
			var other = CalcItemFromSer( linkSer );
			if( IsTeleporterItem( other ))
				linked = true;
		}

		if( targObj.id == 0x574A )
		{
			var charge = ReadIntTag( targObj, "HT_Charges", 0 );
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
		if( !IsInPlayersPack( pUser, targObj ))
		{
			socket.SysMessage( "This must be in your backpack to link it." );
			return false;
		}

		pUser.SetTempTag( "HT_LinkSrcSer", targObj.serial );
		socket.CustomTarget( 0, "Target the other teleporter in your backpack to link." );
		targObj.Refresh();
		return false;
	}

	if( popupEntry == 0x0103 )
	{
		if( targObj.id != 0x574A )
			return false;

		var cur = ReadIntTag( targObj, "HT_Charges", 0 );
		if( cur < 0 )
			cur = 0;

		if( cur >= chargeMax )
		{
			socket.SysMessage( "This teleporter is fully charged." );
			return false;
		}

		pUser.SetTempTag( "HT_RechargeSer", targObj.serial );
		socket.CustomTarget( 1, "Target gate travel scrolls in your backpack to recharge." );
		targObj.Refresh();
		return false;
	}

	
	if( popupEntry == 0x0104 )
	{
		// Must be allowed to manage
		if( !( targObj.movable == 3 && ValidateObject( targObj.multi ) && CanManageTeleporter( pUser, targObj )))
		{
			socket.SysMessage( "You cannot change this teleporter's security." );
			return false;
		}

		var curMode = GetSecurityMode( targObj );
		var nextMode = curMode + 1;
		if( nextMode > 2 )
			nextMode = 0;

		targObj.SetTag( "HT_Security", nextMode );
		socket.SysMessage( "Teleporter security set to: " + SecurityName( nextMode ));
		targObj.Refresh();
		return false;
	}

	if( popupEntry == 0x0105 )
	{
		// Must be placed + manageable
		if (!(targObj.movable == 3 && ValidateObject(targObj.multi) && CanManageTeleporter(pUser, targObj)))
		{
			socket.SysMessage("You cannot rename this teleporter.");
			return false;
		}

		// Must be linked
		var linkSer = ReadIntTag(targObj, "HT_LinkSer", 0);
		if (linkSer <= 0)
		{
			socket.SysMessage("This teleporter must be linked before it can be renamed.");
			return false;
		}

		var other = CalcItemFromSer(linkSer);
		if (!IsTeleporterItem(other))
		{
			socket.SysMessage("This teleporter must be linked before it can be renamed.");
			return false;
		}

		// Store the serial so onSpeechInput knows what to rename
		pUser.SetTempTag( "HT_RenameSer", targObj.serial );

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
	if( !pSocket ) 
		return false;

	if( !IsTeleporterItem( iUsed ))
		return false;

	if( !IsInPlayersPack(pUser, iUsed) )
	{
		pSocket.SysMessage( "To link, both teleporters must be in your backpack." );
		return true;
	}

	pUser.SetTempTag( "HT_LinkSrcSer", iUsed.serial );
	pSocket.CustomTarget( 0, "Target the other teleporter in your backpack to link." );
	return true;
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback0( socket, target )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	if( socket.GetWord( 1 ))
		return;

	var srcSer = parseInt( pUser.GetTempTag( "HT_LinkSrcSer" ), 10 );
	if( !isFinite(srcSer) || srcSer <= 0 ) return;

	var telporterA = CalcItemFromSer( srcSer );
	var telporterB = target;

	if( !IsTeleporterItem( telporterA ))
	{
		socket.SysMessage( "Source teleporter not found." );
		return;
	}
	if( !IsTeleporterItem( telporterB ))
	{
		socket.SysMessage( "That is not a house teleporter." );
		return;
	}
	if( telporterA.serial == telporterB.serial )
	{
		socket.SysMessage( "You must target the other teleporter." );
		return;
	}
	if( telporterA.id != telporterB.id )
	{
		socket.SysMessage( "These teleporters are different types and cannot be linked." );
		return;
	}

	if( !IsInPlayersPack( pUser, telporterA ) || !IsInPlayersPack( pUser, telporterB ))
	{
		socket.SysMessage( "Both teleporters must be in your backpack to link." );
		return;
	}

	UnlinkOther( telporterA, telporterB.serial );
	UnlinkOther( telporterB, telporterA.serial );

	telporterA.SetTag( "HT_LinkSer", telporterB.serial );
	telporterB.SetTag( "HT_LinkSer", telporterA.serial );

	socket.SysMessage( "Teleporters linked." );
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback1( socket, target )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	if( socket.GetWord( 1 ))
		return;

	var teleporterSerial = parseInt( pUser.GetTempTag( "HT_RechargeSer" ), 10 );
	if( !isFinite( teleporterSerial ) || teleporterSerial <= 0 )
		return;

	var teleporter = CalcItemFromSer( teleporterSerial );
	if( !ValidateObject( teleporter ) || !teleporter.isItem || teleporter.id != 0x574A )
		return;

	var scroll = target;
	if( !IsScrollItem( scroll ))
	{
		socket.SysMessage("Target gate travel scrolls to recharge the teleporter." );
		return;
	}

	if( !IsInPlayersPack( pUser, scroll ))
	{
		socket.SysMessage( "This item must be in your backpack." );
		return;
	}

	var cur = ReadIntTag( teleporter, "HT_Charges", 0 );
	if( cur < 0 )
		cur = 0;

	if( cur >= chargeMax )
	{
		socket.SysMessage( "The House Teleporter cannot be charged any further." );
		return;
	}

	if(( scroll.amount|0 ) > 1 )
	{
		scroll.amount = ( scroll.amount|0 ) - 1;
	}
	else
	{
		scroll.Delete();
	}

	cur += chargeAmount;
	if( cur > chargeMax )
		cur = chargeMax;

	teleporter.SetTag( "HT_Charges", cur );
	socket.SysMessage( "The Gate Travel scroll crumbles to dust as it strengthens the House Teleporter." );
	teleporter.Refresh();
}

function UnlinkOther( tile, keepSer )
{
	var old = ReadIntTag( tile, "HT_LinkSer", 0 );
	tile.SetTag( "HT_LinkSer", null );

	if( old > 0 && old != keepSer )
	{
		var oldtele = CalcItemFromSer( old );
		if( IsTeleporterItem( oldtele ))
			oldtele.SetTag( "HT_LinkSer", null );
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
			trgSock.SysMessage( "This must be locked down in a house to function." );
		return false;
	}

	if( !IsInBuilding( objCollidedWith.x, objCollidedWith.y, objCollidedWith.z, objCollidedWith.worldnumber, objCollidedWith.instanceID, true ))
	{
		if( trgSock )
			trgSock.SysMessage( "This must be placed inside a house." );
		return false;
	}

		// SECURITY: Owner + Co-Owners only
	if(!CanUseHouseTeleporter(pColliding, objCollidedWith))
	{
		if( trgSock )
			trgSock.SysMessage("Only the house owner and co-owners may use this teleporter.");
		return false;
	}

	if( trgSock )
	{
		if( pColliding.dead || pColliding.criminal )
		{
			trgSock.SysMessage("You cannot use that right now.");
			return false;
		}
	}
	
	var linkSer = ReadIntTag( objCollidedWith, "HT_LinkSer", 0 );
	if( linkSer <= 0 )
	{
		if( trgSock ) 
			trgSock.SysMessage( "This teleporter is not linked." );
		return false;
	}

	var other = CalcItemFromSer( linkSer );
	if( !IsTeleporterItem( other ))
	{
		if( trgSock )
			trgSock.SysMessage( "This teleporter is not linked." );
		return false;
	}

	if( other.movable != 3 )
	{
		if( trgSock )
			trgSock.SysMessage( "The destination teleporter is not properly placed." );
		return false;
	}

	if( !IsInBuilding( other.x, other.y, other.z, other.worldnumber, other.instanceID, true ))
	{
		if( trgSock )
			trgSock.SysMessage( "The destination teleporter is not properly placed." );
		return false;
	}

	if( objCollidedWith.id == 0x574A )
	{
		var charge = ReadIntTag( objCollidedWith, "HT_Charges", 0 );
		if( charge < 0 )
			charge = 0;

		if( charge <= 0 )
		{
			if( trgSock )
				trgSock.SysMessage( "There are no charges left in this teleporter." );
			return false;
		}

		objCollidedWith.SetTag( "HT_Charges", charge - 1 );
	}

	pColliding.Teleport( other.x, other.y, other.z, other.worldnumber );
	return false;
}

/** @type { ( item: Item, dropper: Character, dest: Item ) => number } */
function onDropItemOnItem( pUser, iDropped, iOn )
{
	var pSocket = pUser.socket;
	if( !pSocket )
		return true;

	if( !ValidateObject( iDropped ) || !ValidateObject( iOn ) )
		return true;

	if( !iOn.isItem || iOn.id != 0x574A )
		return true;

	if( !iDropped.isItem || !( iDropped.id == chargeItemIDReq || iDropped.sectionID == chargeItemSectionIDReq ))
		return true;

	var cur = ReadIntTag( iOn, "HT_Charges", 0 );
	if( cur < 0 )
		cur = 0;

	if( cur >= chargeMax )
	{
		pSocket.SysMessage( "This teleporter is fully charged." );
		return false;
	}

	iDropped.Delete();

	cur += chargeAmount;
	if( cur > chargeMax ) 
		cur = chargeMax;

	iOn.SetTag( "HT_Charges", cur );
	pSocket.SysMessage( "Charges: " + cur + " / " + chargeMax );
	return false;
}

/** @type { ( myChar: Character, myItem: Item, mySpeech: string, mySpeechId: number ) => void } */
function onSpeechInput( pUser, targObj, pSpeech, pSpeechID )
{
	var sock = pUser.socket;
	if( !sock )
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
		sock.SysMessage( GetDictionaryEntry( 9270, sock.language )); // too short / none entered
		return;
	}
	if( pSpeech.length > 60 )
	{
		sock.SysMessage( GetDictionaryEntry( 9271, sock.language )); // too long (we'll reuse)
		return;
	}

	// Resolve which teleporter we are renaming
	var srcSer = parseInt( pUser.GetTempTag( "HT_RenameSer" ), 10 );
	if( !isFinite( srcSer ) || srcSer <= 0 )
		return;

	var teleA = CalcItemFromSer( srcSer );
	if( !IsTeleporterItem( teleA ))
	{
		sock.SysMessage( "Teleporter not found." );
		return;
	}

	// Must still be linked
	var linkSer = ReadIntTag( teleA, "HT_LinkSer", 0 );
	if( linkSer <= 0 )
	{
		sock.SysMessage( "This teleporter must be linked before it can be renamed." );
		return;
	}

	var teleB = CalcItemFromSer( linkSer );
	if( !IsTeleporterItem( teleB ))
	{
		sock.SysMessage( "This teleporter must be linked before it can be renamed." );
		return;
	}

	// Apply name to BOTH
	teleA.name = pSpeech;
	teleB.name = pSpeech;

	teleA.Refresh();
	teleB.Refresh();

	sock.SysMessage( "Teleporter name set to: " + pSpeech );
}

/** @type { ( myObj: BaseObject, pSocket: Socket ) => string } */
function onTooltip( myObj, pSocket )
{
	if( !ValidateObject( myObj ) || !myObj.isItem )
		return "";

	if( !IsTeleporterItem( myObj ))
		return "";

	var parts = [];
	var secMode = GetSecurityMode( myObj );
	parts.push( "Security: " + SecurityName( secMode ));

	if( myObj.id == 0x574A )
	{
		var charge = ReadIntTag( myObj, "HT_Charges", 0 );
		if( charge < 0 ) charge = 0;
		if( charge > chargeMax ) charge = chargeMax;
		parts.push( "Charges: " + charge + "/" + chargeMax );
	}

	var linkSer = ReadIntTag( myObj, "HT_LinkSer", 0 );
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
