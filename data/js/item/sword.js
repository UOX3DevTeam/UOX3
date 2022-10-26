// This script handles the double-click functionality of all swords/knives

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pUser.SysMessage( GetDictionaryEntry( 6019, socket.language )); // This must be in your backpack or equipped before it can be used.
			return false;
		}
		else if( iUsed.type != 15 )
		{
			var targMsg = GetDictionaryEntry( 462, socket.language ); // What would you like to use that on?
			socket.CustomTarget( 1, targMsg );
		}
		else
		{
			return true;
		}
	}
	return false;
}

function onCallback1( socket, ourObj )
{
	var mChar = socket.currentChar;
	if( mChar && mChar.isChar )
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
				TriggerEvent( 2012, "ShearSheep", mChar, ourObj );
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
				MakeKindling( socket, mChar );
			}
			else if( ourObj )
			{
				if( tileID >= 0x09CC && tileID <= 0x09CF )	// Fish
				{
					MakeFishSteaks( socket, mChar, ourObj );
				}
				else if( tileID == 0x1be0 || ( tileID >= 0x1BD7 && tileID <= 0x1BE2 ))	// Bowcraft
				{
					BowCraft( socket, mChar, ourObj, tileID );
				}
				else if( tileID == 0x2006 )
				{
					CarveCorpse( socket, mChar, ourObj );
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 1968, socket.language )); // You cannot carve that.
				}
			}
		}
	}
}

function BowCraft( socket, mChar, ourObj, tileID )
{
	var ownerObj = GetPackOwner( ourObj, 0 );
	if( ownerObj && mChar.serial == ownerObj.serial )
	{
		socket.MakeMenu( 49, 8 );
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 781, socket.language )); // You can't use lumber outside your backpack.
	}
}

function MakeFishSteaks( socket, mChar, ourObj )
{
	var ownerObj = GetPackOwner( ourObj, 0 );
	if( ownerObj && mChar.serial == ownerObj.serial )
	{
		var fishSteakAmount = ourObj.amount * 4;
		if( fishSteakAmount > 65534 )
		{
			var pilesOfSteaks = Math.ceil( fishSteakAmount / 65535 );
			for( var i = 0; i < pilesOfSteaks; i++ )
			{
				if( fishSteakAmount > 65534 )
				{
					CreateDFNItem( mChar.socket, mChar, "0x097A", 65535, "ITEM", true );
					fishSteakAmount -= 65535;
				}
				else
				{
					CreateDFNItem( mChar.socket, mChar, "0x097A", fishSteakAmount, "ITEM", true );
				}
			}
		}
		else
		{
			CreateDFNItem( mChar.socket, mChar, "0x097A", fishSteakAmount, "ITEM", true );
		}

		mChar.SysMessage( GetDictionaryEntry( 9338, socket.language )); // You slice your fish into raw fish steaks
		ourObj.Delete();
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 775, socket.language )); // You can't use material outside your backpack.
	}
}

function MakeKindling( socket, mChar )
{
	var distX = Math.abs( mChar.x - socket.GetWord( 11 ));
	var distY = Math.abs( mChar.y - socket.GetWord( 13 ));
	var distZ = Math.abs( mChar.z - socket.GetSByte( 16 ));

	if( distX > 5 || distY > 5 || distZ > 9 )
	{
		socket.SysMessage( GetDictionaryEntry( 393, socket.language )); // That is too far away.
		return;
	}

	if( mChar.isonhorse )
	{
		mChar.DoAction( 0x1D );
	}
	else
	{
		mChar.DoAction( 0x0D );
	}

	mChar.SoundEffect( 0x013E, true );
	CreateDFNItem( socket, mChar, "0x0DE1", 1, "ITEM", true );
	socket.SysMessage( GetDictionaryEntry( 1049, socket.language )); // You hack at the tree and produce some kindling.
}

function CarveCorpse( socket, mChar, ourObj )
{
	if( mChar.InRange( ourObj, 3 ))
	{
		if( mChar.visible == 1 || mChar.visible == 2 )
		{
			mChar.visible = 0;
		}
		mChar.DoAction( 0x20 );
		if( (ourObj.morey>>24) == 0 )
		{
			var part1 = 1;
			var part2 = ourObj.morey >> 16;
			var part3 = ourObj.morey >> 8;
			var part4 = ourObj.morey % 256;

			ourObj.morey = ( part1 << 24 ) + ( part2 << 16 ) + ( part3 << 8 ) + part4;
			if( part2 != 0 || ourObj.carveSection != -1 )
			{
				ourObj.Carve( socket );
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