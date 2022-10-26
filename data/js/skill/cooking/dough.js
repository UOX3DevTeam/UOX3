// use dough : target heat source : bread
// use dough : target jar of honey : sweet dough
// use dough : target apple : Unbaked Apple Pie
// use dough : target Peach : Unbaked Peach Cobbler
// use dough : target Pear : Unbaked Fruit Pie
// use dough : target Pumpkin  : Unbaked Pumpkin Pie
// use dough : target Eggs : Unbaked Quiche/Eggshells
// use dough : target Bird, Ham or Fish  : Unbaked Meat Pie
// use dough : target Sausage : Sausage Pizza
// use dough : target Cheese Wheel : Cheese Pizza

function onUseChecked ( pUser, iUsed )
{
	// get users socket
	var srcSock = pUser.socket;

	// is it in users pack?
	if( iUsed.container != null )
	{
		var iPackOwner = GetPackOwner( iUsed, 0 );
		if( iPackOwner.serial != pUser.serial )
		{
			srcSock.SysMessage( GetDictionaryEntry( 6022, srcSock.language )); // This has to be in your backpack before you can use it.
		}
		else
		{
			if( iUsed.colour == 150 )
			{
				srcSock.tempObj = iUsed;
				srcSock.CustomTarget( 1, GetDictionaryEntry( 6047, srcSock.language )); // What do you want to use the dough with?
			}
			else
			{
				srcSock.tempObj = iUsed;
				srcSock.CustomTarget( 0, GetDictionaryEntry( 6047, srcSock.language )); // What do you want to use the dough with?
			}
		}
	}
	else
	{
		srcSock.SysMessage( GetDictionaryEntry( 6022, srcSock.language )); // This has to be in your backpack before you can use it.
	}
	return false;
}

function onCallback0( tSock, myTarget )
{
	var pUser = tSock.currentChar;
	var iUsed = tSock.tempObj;
	var tileID = tSock.GetWord( 17 );

	if(( tileID == 0 && tSock.GetByte( 1 )) || ( ValidateObject( myTarget ) && myTarget.isChar ))
	{
		// Player targeted a map tile, or a character
		tSock.SysMessage( GetDictionaryEntry( 6102, tSock.language )); // You cannot use the dough with that.
		return;
	}

	// Check if player targeted a source of heat
	if( TriggerEvent( 106, "HeatSource", tileID ))
	{
		TriggerEvent( 104, "Cooking", tSock, myTarget );
		return;
	}

	if( !ValidateObject( myTarget ))
	{
		// Player targeted a static tile that's not a source of heat
		tSock.SysMessage( GetDictionaryEntry( 6102, tSock.language )); // You cannot use the dough with that.
		return;
	}

	var iPackOwner = GetPackOwner( myTarget, 0 );
	var targColor = myTarget.colour;
	var doughColor = iUsed.colour;

	if( ValidateObject( iPackOwner )) // Is the target item in a backpack?
	{
		if( iPackOwner.serial != pUser.serial ) // And if so does the pack belong to the user?
		{
			tSock.SysMessage( GetDictionaryEntry( 6032, tSock.language )); // That resource is in someone else's backpack!
			return;
		}
	}
	else
	{
		tSock.SysMessage( GetDictionaryEntry( 6022, tSock.language )); // This has to be in your backpack before you can use it.
		return;
	}

	switch( tileID )
	{
		case 0x09EC: // jar of honey
			CreateDFNItem( tSock, pUser, "sweet_dough", 1, "ITEM", true );
			break;
		case 0x09D0: // apple
			CreateDFNItem( tSock, pUser, "unbaked_apple_pie", 1, "ITEM", true );
			break;
		case 0x09D2: // peach
		case 0x172C:
			CreateDFNItem( tSock, pUser, "unbaked_peach_cobbler", 1, "ITEM", true );
			break;
		case 0x0994: // pear
		case 0x172D:
			CreateDFNItem( tSock, pUser, "unbaked_fruit_pie", 1, "ITEM", true );
			break;
		case 0x0C6A: // pumpkin
		case 0x0C6B:
			CreateDFNItem( tSock, pUser, "unbaked_pumpkin_pie", 1, "ITEM", true );
			break;
		case 0x0C6C: // pumpkin
			CreateDFNItem( tSock, pUser, "unbaked_pumpkin_pie", 1, "ITEM", true );
			break;
		case 0x09b5: // egg
			CreateDFNItem( tSock, pUser, "unbaked_quiche", 1, "ITEM", true );
			CreateDFNItem( tSock, pUser, "0x09B4", 1, "ITEM", true );
			break;
		case 0x1E1C: // Bird, Ham or Fish
		case 0x1E1D:
			CreateDFNItem( tSock, pUser, "unbaked_meat_pie", 1, "ITEM", true );
			break;
		case 0x1E1E: // Bird, Ham or Fish
		case 0x09B7:
			CreateDFNItem( tSock, pUser, "unbaked_meat_pie", 1, "ITEM", true );
			break;
		case 0x09B8: // Bird, Ham or Fish
		case 0x09C9:
			CreateDFNItem( tSock, pUser, "unbaked_meat_pie", 1, "ITEM", true );
			break;
		case 0x09D3: // Bird, Ham or Fish
			CreateDFNItem( tSock, pUser, "unbaked_meat_pie", 1, "ITEM", true );
			break;
		case 0x09C0: // Sausage
		case 0x09C1:
			CreateDFNItem( tSock, pUser, "uncooked_sausage_pizza", 1, "ITEM", true );
			break;
		case 0x097E: // Cheese Wheel
			CreateDFNItem( tSock, pUser, "uncooked_cheese_pizza", 1, "ITEM", true );
			break;
		default:
			return;
	}

	pUser.UseResource( 1, myTarget.id, targColor );
	pUser.UseResource( 1, iUsed.id, doughColor );
	pUser.SoundEffect( 0x0055, true );
}

function onCallback1( tSock, myTarget )
{
	var pUser = tSock.currentChar;
	var iUsed = tSock.tempObj;
	var tileID = tSock.GetWord( 17 );

	if(( tSock.GetWord( 1 ) && tileID == 0 ) || ( tileID == 0 && ( !ValidateObject( myTarget ) || myTarget.isChar )))
	{
		// Map tile targeted, or invalid target
		tSock.SysMessage( GetDictionaryEntry( 6102, tSock.language )); // You cannot use the dough with that.
		return;
	}

	if( tileID != 0 && TriggerEvent( 106, "HeatSource", tileID ))
	{
		TriggerEvent( 104, "Cooking", tSock, myTarget );
		return;
	}

	var iPackOwner = GetPackOwner( myTarget, 0 );
	var targColor = myTarget.colour;
	var doughColor = iUsed.colour;

	if( ValidateObject( iPackOwner )) // Is the target item in a backpack?
	{
		if( iPackOwner.serial != pUser.serial ) // And if so does the pack belong to the user?
		{
			tSock.SysMessage( GetDictionaryEntry( 6032, tSock.language )); // That resource is in someone else's backpack!
			return;
		}
	}
	else
	{
		tSock.SysMessage( GetDictionaryEntry( 6022, tSock.language )); // This has to be in your backpack before you can use it.
		return;
	}

	switch( tileID )
	{
		case 0x09EC://jar of honey
			CreateDFNItem( tSock, pUser, "0x103F", 1, "ITEM", true );
			break;
		case 0x1046://Open Sack Flour
	    case 0x103A:
			CreateDFNItem( tSock, pUser, "cake_mix", 1, "ITEM", true );
			break;
		default:
			return;
	}
	pUser.UseResource( 1, myTarget.id, targColor );
	pUser.UseResource( 1, iUsed.id, doughColor );
	pUser.SoundEffect( 0x0055, true );
}

function RangeCheck( tSock, pUser )
{
	var targX = tSock.GetWord( 11 );
	var targY = tSock.GetWord( 13 );
	var targZ = tSock.GetSByte( 16 );
	return ( pUser.x > targX + 3 ) || ( pUser.x < targX - 3 ) || ( pUser.y > targY + 3 ) || ( pUser.y < targY - 3 ) || ( pUser.z > targZ + 10 ) || ( pUser.z < targZ - 10 );
}