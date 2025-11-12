/// <reference path="../definitions.d.ts" />
// @ts-check
// This script handles the double-click functionality of all swords/knives

/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;
	if( pSocket && iUsed && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pSocket.SysMessage( GetDictionaryEntry( 6019, pSocket.language )); // This must be in your backpack or equipped before it can be used.
			return false;
		}
		else if( iUsed.type != 15 )
		{
			var targMsg = GetDictionaryEntry( 462, pSocket.language ); // What would you like to use that on?
			pSocket.CustomTarget( 1, targMsg );
		}
		else
		{
			return true;
		}
	}
	return false;
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback1( socket, ourObj )
{
	var pUser = socket.currentChar;
	if( pUser && pUser.isChar )
	{
		var tileID = 0;
		if( !ourObj )
		{
			tileID = socket.GetWord( 17 );
		}
		else
		{
			tileID = ourObj.id;
			if( tileID == 0x00df || tileID == 0x00cf )
			{
				TriggerEvent( 2012, "ShearSheep", pUser, ourObj );
				return;
			}
			else
			{
				tileID = socket.GetWord( 17 );
			}
		}

		if( tileID != 0 )
		{
			if( tileID == 0x0CD0 || tileID == 0x0CD3 || tileID == 0x0CD6 || tileID == 0x0CD8 || tileID == 0x0CDA ||
				tileID == 0x0CDD || tileID == 0x0CE0 || tileID == 0x0CE3 || tileID == 0x0CE6 || tileID == 0x0D58 ||
				( tileID >= 0x0CCA && tileID <= 0x0CCE ) || ( tileID >= 0x12B8 && tileID <= 0x12BB ) || tileID == 0x0D42 ||
				tileID == 0x0D43 || tileID == 0x0D58 || tileID == 0x0D59 || tileID == 0x0D70 || tileID == 0x0D85 ||
				tileID == 0x0D94 || tileID == 0x0D95 || tileID == 0x0D98 || tileID == 0x0DA4 || tileID == 0x0DA8 ) // Trees
			{
				MakeKindling( socket, pUser );
			}
			else if( ourObj )
			{
				if( tileID >= 0x09CC && tileID <= 0x09CF )	// Fish
				{
					MakeFishSteaks( socket, pUser, ourObj );
				}
				else if( tileID == 0x1be0 || ( tileID >= 0x1BD7 && tileID <= 0x1BE2 ))	// Bowcraft
				{
					BowCraft( socket, pUser, ourObj, tileID );
				}
				else if( tileID == 0x2006 )
				{
					CarveCorpse( socket, pUser, ourObj );
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 1968, socket.language )); // You cannot carve that.
				}
			}
		}
	}
}

function BowCraft( socket, pUser, ourObj, tileID )
{
	var ownerObj = GetPackOwner( ourObj, 0 );
	if( ownerObj && pUser.serial == ownerObj.serial )
	{
		socket.MakeMenu( 49, 8 );
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 781, socket.language )); // You can't use lumber outside your backpack.
	}
}

function MakeFishSteaks( socket, pUser, ourObj )
{
	var ownerObj = GetPackOwner( ourObj, 0 );
	if( ownerObj && pUser.serial == ownerObj.serial )
	{
		var fishSteakAmount = ourObj.amount * 4;
		if( fishSteakAmount > 65534 )
		{
			var pilesOfSteaks = Math.ceil( fishSteakAmount / 65535 );
			for( var i = 0; i < pilesOfSteaks; i++ )
			{
				if( fishSteakAmount > 65534 )
				{
					CreateDFNItem( socket, pUser, "0x097A", 65535, "ITEM", true );
					fishSteakAmount -= 65535;
				}
				else
				{
					CreateDFNItem( socket, pUser, "0x097A", fishSteakAmount, "ITEM", true );
				}
			}
		}
		else
		{
			CreateDFNItem( socket, pUser, "0x097A", fishSteakAmount, "ITEM", true );
		}

		socket.SysMessage( GetDictionaryEntry( 9338, socket.language )); // You slice your fish into raw fish steaks
		ourObj.Delete();
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 775, socket.language )); // You can't use material outside your backpack.
	}
}

function MakeKindling( socket, pUser )
{
	var distX = Math.abs( pUser.x - socket.GetWord( 11 ));
	var distY = Math.abs( pUser.y - socket.GetWord( 13 ));
	var distZ = Math.abs( pUser.z - socket.GetSByte( 16 ));

	if( distX > 5 || distY > 5 || distZ > 9 )
	{
		socket.SysMessage( GetDictionaryEntry( 393, socket.language )); // That is too far away.
		return;
	}

	if( pUser.isonhorse )
	{
		pUser.DoAction( 0x1D );
	}
	else
	{
		pUser.DoAction( 0x0D );
	}

	pUser.SoundEffect( 0x013E, true );
	CreateDFNItem( socket, pUser, "0x0DE1", 1, "ITEM", true );
	socket.SysMessage( GetDictionaryEntry( 1049, socket.language )); // You hack at the tree and produce some kindling.
}

function CarveCorpse( socket, pUser, ourObj )
{
	if( pUser.InRange( ourObj, 3 ))
	{
		if( pUser.visible == 1 || pUser.visible == 2 )
		{
			pUser.visible = 0;
		}

		var moreYPart1 = ourObj.GetMoreVar( "morey", 1 );
		var moreYPart2 = ourObj.GetMoreVar( "morey", 2 );
		var moreYPart3 = ourObj.GetMoreVar( "morey", 3 );
		var moreYPart4 = ourObj.GetMoreVar( "morey", 4 );

		if( moreYPart1 == 0 ) // Corpse has not been carved before
		{
			if(( moreYPart2 != 0 || ourObj.carveSection != -1 ) && ourObj.Carve( socket ))
			{
				pUser.DoAction( 0x20 );
				ourObj.SetMoreVar( "morey", 1, 1 ); // Mark corpse as carved
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 1051, socket.language )); // You carve the corpse but find nothing useful.
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 393, socket.language )); // That is too far away.
	}
}
