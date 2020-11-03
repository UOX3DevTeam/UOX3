// Scissor-usage; Sheep-shearing, cloth-cutting
// 02/03/2003 Xuri; xuri@sensewave.com
// This script contains all the default functions of the scissors,
// from sheep-shearing to cloth cutting.

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		var isInRange = pUser.InRange( iUsed, 3 );
		if( !isInRange )
	 	{
			pUser.SysMessage( GetDictionaryEntry( 389, socket.language ) ); // That is too far away and you cannot reach it.
			return false;
		}
		socket.tempObj = iUsed;
		pUser.socket.CustomTarget( 0, "What should I use these scissors on?" );
	}
	return false;
}

function onCallback0( pSock, myTarget )
{
	var pUser 	= pSock.currentChar;
	var StrangeByte	= pSock.GetWord( 1 );
	var tileID	= pSock.GetWord( 17 );

	if( !ValidateObject( myTarget ) || tileID == 0 )
	{ //Target is invalid or a Maptile
		pSock.SysMessage( "You can't use the scissors on that." );
	}
	else if( StrangeByte == 0 )
	{
		if( myTarget.isChar )
		{ //Target is a Character
			var isInRange = pUser.InRange( myTarget, 3 );
			if( !isInRange )
		 	{
				pSock.SysMessage( GetDictionaryEntry( 461, pSock.language ) ); // You are too far away.
				return;
			}
			else if( myTarget.id == 0x00df || myTarget.id == 0x00cf )
				TriggerEvent( 2012, "shearSheep", pUser, myTarget );
			else
				pSock.SysMessage( "You can't use the scissors on that." );
			return;
		}
		else
		{ //Target is a Dynamic Item
			if( !pUser.InRange( myTarget, 3 ) )
		 	{
				pSock.SysMessage( "You are too far away to reach that." );
				return;
			}
			if( myTarget.movable == 3 )
			{
				pSock.SysMessage( "Locked down resources cannot be used!" );
				return;
			}

			var iPackOwner = GetPackOwner( myTarget, 0 );
			if( ValidateObject( iPackOwner ) ) // Is the target item in a backpack?
			{
				if( iPackOwner.serial != pUser.serial ) //And if so does the pack belong to the user?
				{
					pSock.SysMessage( "That resource is in someone else's backpack!" );
					return;
				}
			}
			else // ...or is the target item on the ground?
			{
				pUser.SysMessage( "This has to be in your backpack before you can use it." );
				return;
			}
			tileID = myTarget.id;
			if( tileID >= 3989 && tileID <= 3996 ) // <-- Decimal item-ids
			{ //Cut bolts of cloth into cut cloth
				pSock.SysMessage( "You cut the material and place it into your backpack." );
				var itemMade = CreateDFNItem( pSock, pUser, "0x1766", myTarget.amount*50, "ITEM", true );  //give the player some cut cloth
				myTarget.Delete();
				return;
			}
			else if(( tileID >= 5981 && tileID <= 5992 ) || ( tileID >= 5397 && tileID <= 5400 ) || ( tileID >= 5422 && tileID <= 5444 ) || ( tileID >= 7933 && tileID <= 7940 )) // <-- Decimal item-ids
			{ //Cut folded cloth/clothes into bandages
				pSock.SysMessage( "You cut the material into bandage and place it in your backpack." );
				var itemMade = CreateDFNItem( pSock, pUser, "0x0e21", myTarget.amount, "ITEM", true );  //give the player some bandages
				myTarget.Delete();
				return;
			}
			else if( tileID >= 4216 && tileID <= 4217 ) // <-- Decimal item-ids
			{ //Cut up hides into leather
				pSock.SysMessage( "You cut the material into leather and place it in your backpack." );
				var itemMade = CreateDFNItem( pSock, pUser, "0x1081", myTarget.amount, "ITEM", true );  //give the player some leather
				myTarget.Delete();
				return;
			}
		}
		pSock.SysMessage( "Scissors cannot be used on that to produce anything." );
	}
}
