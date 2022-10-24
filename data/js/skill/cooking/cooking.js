// cooking script
// 17/06/2001 Yeshe; yeshe@manofmystery.org
// 21/07/2003 Xuri; Updated/rewrote the script
// 25/07/2021 Updated to use dictionary messages
// 8/31/21 all cooking updated. Dragon Slayer
// Raw bird : Heat source : Cooked bird
// Raw fish steaks : Heat source : Cooked fish steak
// Raw Chicken : Heat source : Cooked Chicken
// Raw Leg Of Lamb : Heat source : Cooked Leg Of Lamb
// Raw Cut Of Ribs : Heat source : Cooked Cut Of Ribs
// Raw Eggs : Heat source : Fried Eggs
// sauage or cheese pizza : Heat source : pizza

function onUseChecked( pUser, iUsed )
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
			// If player used a cooking tool, open the crafting menu
			if( iUsed.id == 0x1043 || iUsed.id == 0x097f || iUsed.id == 0x09e2 || iUsed.id == 0x103e )
			{
				srcSock.MakeMenu( 750, 13 ); // Open submenu 750 using Cooking skill
			}
			else
			{
				// let the user target the heat source
				srcSock.tempObj = iUsed;
				srcSock.CustomTarget( 0, GetDictionaryEntry( 6097, srcSock.language )); // What do you want to cook this on?
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
	Cooking( tSock, myTarget )
}

function Cooking( tSock, myTarget )
{
	var pUser = tSock.currentChar;
	var iUsed = tSock.tempObj;
	var strangeByte = tSock.GetWord( 1 );
	var tileID = tSock.GetWord( 17 );
	var foodColor = iUsed.colour;
	var cookingMsg = "";

	if(( tileID == 0 && tSock.GetByte( 1 )) || ( ValidateObject( myTarget ) && myTarget.isChar ))
	{
		// Player targeted a map tile, or a character
		cookingMsg = GetDictionaryEntry( 6095, tSock.language ); // You cannot cook your %s on that.
		tSock.SysMessage( cookingMsg.replace( /%s/gi, iUsed.name ));
		return;
	}

	if( TriggerEvent( 106, "HeatSource", tileID ))
	{
		if( TriggerEvent( 103, "RangeCheck", tSock, pUser ))
		{
			tSock.SysMessage( GetDictionaryEntry( 393, tSock.language )); // That is too far away.
			return;
		}

		if( !pUser.CheckSkill( 13, 0, 1000 ))
		{
			cookingMsg = GetDictionaryEntry( 6093, tSock.language ); // You burn the %s to a crisp! It's ruined.
			tSock.SysMessage( cookingMsg.replace( /%s/gi, iUsed.name ));
			iUsed.id = 0x09F2; // burnt food id
			return;
		}

		if( pUser.GetTempTag( "CookingDelay" ) == 1 )
		{
			tSock.SysMessage( GetDictionaryEntry( 6094, tSock.language )); // You are already cooking something.
			return;
		}

		if( iUsed.name == "sweet dough" )
        {
            pUser.SetTempTag( "CookingDelay", 1 )
            pUser.StartTimer( 5000, 9, true );
            pUser.SoundEffect( 0x0057, true );
            if( iUsed.amount > 1 )
            {
                iUsed.amount -= 1;
            }
            else
            {
                iUsed.Delete();
            }
            return;
        }

		switch( iUsed.id )
		{
			case 0x09B9: // Raw Bird
			case 0x09BA:
				pUser.StartTimer( 5000, 1, true );
				break;
			case 0x097A: // Raw Fish Steaks
			    pUser.StartTimer( 5000, 2, true );
				break;
			case 0x103D: // Dough
				pUser.StartTimer( 5000, 3, true );
				break;
			case 0x1607: // Raw Chicken
				pUser.StartTimer( 5000, 4, true );
				break;
			case 0x1609: // Raw Leg Of Lamb
				pUser.StartTimer( 5000, 5, true );
				break;
			case 0x09F1: // Raw Cut Of Ribs
				pUser.StartTimer( 5000, 6, true );
				break;
			case 0x09b5: // Fried Eggs
				pUser.StartTimer( 5000, 7, true );
				break;
			case 0x1083: // Pizzas
				pUser.StartTimer( 5000, 8, true );
				break;
		}

		pUser.SetTempTag( "CookingDelay", 1 )
		pUser.UseResource( 1, iUsed.id, foodColor );
		pUser.SoundEffect( 0x0057, true );
		return;
	}
}

function onTimer( pUser, timerID )
{
	if( !ValidateObject( pUser ) || pUser.socket == null )
		return;

	var socket = pUser.socket;
	switch( timerID )
	{
		case 1:
			CreateDFNItem( socket, pUser, "0x09B8", 1, "ITEM", true ); // makes a cooked bird
			socket.SysMessage( GetDictionaryEntry( 6053, socket.language )); // You cook a bird. Smells good!
			break;
		case 2:
			CreateDFNItem( socket, pUser, "0x097B", 1, "ITEM", true ); // makes a fish steak
			socket.SysMessage( GetDictionaryEntry( 6063, socket.language )); // You cook a fish steak.
			break;
		case 3:
			var rndBread = RandomNumber( 0, 1 );
			if( rndBread )
			{
				CreateDFNItem( socket, pUser, "0x103B", 1, "ITEM", true ); // makes a loaf of bread
			}
			else
			{
				CreateDFNItem( socket, pUser, "0x103C", 1, "ITEM", true ); // makes a loaf of bread
			}
			socket.SysMessage( GetDictionaryEntry( 6051, socket.language )); // You bake a loaf of bread.
			break;
		case 4:
			CreateDFNItem( socket, pUser, "0x1608", 1, "ITEM", true ); // makes a cooked chicken leg
			socket.SysMessage( GetDictionaryEntry( 6058, socket.language )); // You cook a chicken leg. Smells good!
			break;
		case 5:
			CreateDFNItem( socket, pUser, "0x160A", 1, "ITEM", true ); // makes a cooked lamb leg
			socket.SysMessage( GetDictionaryEntry( 6068, socket.language )); // You cook a leg of lamb.
			break;
		case 6:
			CreateDFNItem( socket, pUser, "0x09F2", 1, "ITEM", true ); // makes a cooked rib
			socket.SysMessage( GetDictionaryEntry( 6073, socket.language )); // You cook a rib.
			break;
		case 7:
			CreateDFNItem( socket, pUser, "0x09B6", 1, "ITEM", true ); // makes fried eggs
			socket.SysMessage( GetDictionaryEntry( 6088, socket.language )); // You fry the eggs.
			break;
		case 8:
			CreateDFNItem( socket, pUser, "0x1040", 1, "ITEM", true ); // makes pizza
			socket.SysMessage( GetDictionaryEntry( 6096, socket.language )); // You cook a pizza.
			break;
		case 9:
			CreateDFNItem( socket, pUser, "0x09EA", 1, "ITEM", true ); // makes muffins
			socket.SysMessage( GetDictionaryEntry( 6098, socket.language )); // You cook a muffin.
			break;
		default:
			return;
	}
	pUser.SetTempTag( "CookingDelay", null );
}
