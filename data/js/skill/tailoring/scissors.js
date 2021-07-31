// Scissor-usage; Sheep-shearing, cloth-cutting
// 24/07/2021 Xuri; xuri@uox3.org
// This script contains all the default functions of the scissors,
// from sheep-shearing to cloth cutting.
var AOSEnabled = 1;

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		var isInRange = pUser.InRange( iUsed, 3 );
		if( !isInRange )
	 	{
			socket.SysMessage( GetDictionaryEntry( 389, socket.language ) ); // That is too far away and you cannot reach it.
			return false;
		}
		socket.tempObj = iUsed;
		socket.CustomTarget( 0, GetDictionaryEntry( 6029, socket.language )); // What do you want to use these scissors on?
	}
	return false;
}

function onCallback0( pSock, myTarget )
{
	var pUser 	= pSock.currentChar;
	var StrangeByte	= pSock.GetWord( 1 );
	var tileID = pSock.GetWord( 17 );
	myTarget.SetTag( "ResourceColor", myTarget.colour );
	var ResourceColor = myTarget.GetTag( "ResourceColor" );

	if( !ValidateObject( myTarget ) || tileID == 0 )
	{ //Target is invalid or a Maptile
		pSock.SysMessage( GetDictionaryEntry( 6030, pSock.language ) ); // You can't use the scissors on that.
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
			{
				TriggerEvent( 2012, "shearSheep", pUser, myTarget );
			}
			else
				pSock.SysMessage( GetDictionaryEntry( 6030, pSock.language ) ); // You can't use the scissors on that.
			return;
		}
		else
		{ //Target is a Dynamic Item
			if( !pUser.InRange( myTarget, 3 ) )
		 	{
				pSock.SysMessage( GetDictionaryEntry( 461, pSock.language ) ); // You are too far away.
				return;
			}
			if( myTarget.movable == 3 )
			{
				pSock.SysMessage( GetDictionaryEntry( 6031, pSock.language ) ); // Locked down resources cannot be used!
				return;
			}

			var iPackOwner = GetPackOwner( myTarget, 0 );
			if( ValidateObject( iPackOwner ) ) // Is the target item in a backpack?
			{
				if( iPackOwner.serial != pUser.serial ) //And if so does the pack belong to the user?
				{
					pSock.SysMessage( GetDictionaryEntry( 6032, pSock.language ) ); // That resource is in someone else's backpack!
					return;
				}
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 6022, pSock.language ) ); // This has to be in your backpack before you can use it.
				return;
			}

			if (myTarget.isNewbie == true)
			{
				pSock.SysMessage(GetDictionaryEntry(6036, pSock.language)); // Scissors cannot be used on that to produce anything.
				return;
			}

			if( CutBolts( tileID ) )
			{ //Cut bolts of cloth into cut cloth
				pUser.SoundEffect( 0x248, true );
				pSock.SysMessage( GetDictionaryEntry( 6033, pSock.language ) ); // You cut the material and place it into your backpack.
				CreateDFNItem( pSock, pUser, "0x1766", myTarget.amount*50, "ITEM", true, ResourceColor );  //give the player some cut cloth
				myTarget.Delete();
				return;
			}
			else if( CutCloth( tileID ) )
			{ //Cut folded cloth and cut cloth into bandages
				pUser.SoundEffect( 0x248, true );
				pSock.SysMessage( GetDictionaryEntry( 6034, pSock.language ) ); // You cut the material into bandage and place it in your backpack.
				CreateDFNItem( pSock, pUser, "0x0e21", myTarget.amount, "ITEM", true, ResourceColor );  //give the player some bandages
				myTarget.Delete();
				return;
			}
			else if( CutLeather( tileID ) )
			{ //Cut up hides into leather
				pUser.SoundEffect( 0x248, true );
				pSock.SysMessage( GetDictionaryEntry( 6035, pSock.language ) ); // You cut the material into leather and place it in your backpack.
				CreateDFNItem( pSock, pUser, "0x1081", myTarget.amount, "ITEM", true, ResourceColor );
				myTarget.Delete();
				return;
			}
			else if ( CutClothing ( tileID ) )
			{//Cut Clothing into cut cloth
				pUser.SoundEffect( 0x248, true );
				CreateDFNItem( pSock, pUser, "0x1766", myTarget.amount*2 , "ITEM", true, ResourceColor );
				myTarget.Delete();
				return;
			}
			else if ( AOSEnabled == 1  && CutBones ( tileID ) )
			{//Cut Bones Parts into Bone
				pUser.SoundEffect( 0x248, true );
				CreateDFNItem( pSock, pUser, "0x0f7e", myTarget.amount*10 , "ITEM", true, ResourceColor );
				myTarget.Delete();
				return;
			}
		}
		pSock.SysMessage( GetDictionaryEntry( 6036, pSock.language ) ); // Scissors cannot be used on that to produce anything.
	}
}

function CutClothing( tileID )
{
	return ( ( tileID >= 5397 && tileID <= 5400 ) || ( tileID >= 5422 && tileID <= 5444 ) || ( tileID >= 7933 && tileID <= 7940 ) );
}

function CutLeather( tileID )
{
	return ( tileID >= 4216 && tileID <= 4217 );
}

function CutCloth( tileID )
{
	return ( tileID >= 5981 && tileID <= 5992 );
}

function CutBolts( tileID )
{
	return ( tileID >= 3989 && tileID <= 3996 );
}

function CutBones( tileID )
{
	return ( tileID >= 6921 && tileID <= 6929 || ( tileID >= 6935 && tileID <= 6937 ) );
}
