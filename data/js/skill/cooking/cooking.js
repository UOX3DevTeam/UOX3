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
			// let the user target the heat source
			srcSock.tempObj = iUsed;
			srcSock.CustomTarget( 0, GetDictionaryEntry( 6097, srcSock.language )); // what do you want to cook this on?
	}
	else
		srcSock.SysMessage( GetDictionaryEntry( 6022, srcSock.language )); // This has to be in your backpack before you can use it.
	return false;
}

function onCallback0( tSock, targSerial )
{
	Cooking(tSock, targSerial)
}

function Cooking(tSock, targSerial)
{
	var pUser = tSock.currentChar;
	var iUsed = tSock.tempObj;
	var StrangeByte   = tSock.GetWord( 1 );
	var tileID = tSock.GetWord(17);
	var foodColor = iUsed.colour;

	if( tileID == 0 || ( StrangeByte == 0 && targSerial.isChar ))
	{
		tSock.SysMessage( GetDictionaryEntry( 6095, tSock.language ) + " " + iUsed.name + " " + GetDictionaryEntry( 6096, tSock.language ) ); // You cannot cook your foodname on that.
		return;
	}

	if(TriggerEvent( 106, "HeatSource", tileID ))
	{
		if (TriggerEvent( 103, "RangeCheck", tSock, pUser ))
		{
			tSock.SysMessage(GetDictionaryEntry(393, tSock.language)); // That is too far away.
			return;
		}

		if( !pUser.CheckSkill( 13, 0, 140 ) )
		{
			tSock.SysMessage(GetDictionaryEntry( 6093, tSock.language ) + " " + iUsed.name + " " + GetDictionaryEntry( 6094, tSock.language ) ); // You burn the foodname to a crisp! It's ruined.
			iUsed.id = 0x09F2;//burn food id
			return;
		}

		if (pUser.GetTag("CookingDelay") == 1)
		{
			tSock.SysMessage(GetDictionaryEntry(6098, tSock.language) + " " + iUsed.name + " " + GetDictionaryEntry(6099, tSock.language)); // You can not cook this foodname why you are cooking something!
			return;
		}
		switch (iUsed.name)
		{
			case "sweet dough":
				pUser.StartTimer(5000, 9, true);
				break;
		}

		switch (iUsed.id)
		{
			case 0x09B9:// Raw Bird
			case 0x09BA:
				pUser.StartTimer(5000, 1, true);
				break;
			case 0x097A:// Raw Fish Steaks
			    pUser.StartTimer(5000, 2, true);
				break;
			case 0x103D:// Dough
				pUser.StartTimer(5000, 3, true);
				break;
			case 0x1607:// Raw Chicken
				pUser.StartTimer(5000, 4, true);
				break;
			case 0x1609:// Raw Leg Of Lamb
				pUser.StartTimer(5000, 5, true);
				break;
			case 0x09F1:// Raw Cut Of Ribs
				pUser.StartTimer(5000, 6, true);
				break;
			case 0x09b5:// Fried Eggs
				pUser.StartTimer(5000, 7, true);
				break;
			case 0x1083:// Pizzas
				pUser.StartTimer(5000, 8, true);
				break;
		}
		pUser.SetTag("CookingDelay", 1)
		pUser.UseResource(1, iUsed.id, foodColor);
		pUser.UseResource(1, iUsed.name, foodColor);
		pUser.SoundEffect( 0x0057, true );
		return;
	}
}

function onTimer(pUser, timerID)
{
	var socket = pUser.socket;
	switch (timerID)
	{
		case 1:
			CreateDFNItem(socket, pUser, "0x09B8", 1, "ITEM", true); // makes a cooked bird
			socket.SysMessage(GetDictionaryEntry(6053, socket.language)); // You cook a bird. Smells good!
			pUser.SetTag("CookingDelay", null)
			break;
		case 2:
			CreateDFNItem( socket, pUser, "0x097B", 1, "ITEM", true ); // makes a fish steak
			socket.SysMessage( GetDictionaryEntry( 6063, socket.language )); // You cook a fish steak.
			pUser.SetTag("CookingDelay", null)
			break;
		case 3:
			CreateDFNItem(socket, pUser, "0x103B", 1, "ITEM", true); // makes a loaf of bread
			socket.SysMessage(GetDictionaryEntry(6051, socket.language)); // You bake a loaf of bread.
			pUser.SetTag("CookingDelay", null)
			break;
		case 4:
			CreateDFNItem( socket, pUser, "0x1608", 1, "ITEM", true ); // makes a cooked chicken leg
			socket.SysMessage( GetDictionaryEntry( 6058, socket.language )); // You cook a chicken leg. Smells good!
			pUser.SetTag("CookingDelay", null)
			break;
		case 5:
			CreateDFNItem( socket, pUser, "0x160A", 1, "ITEM", true ); // makes a cooked lamb leg
			socket.SysMessage( GetDictionaryEntry( 6068, socket.language )); // You cook a leg of lamb.
			pUser.SetTag("CookingDelay", null)
			break;
		case 6:
			CreateDFNItem( socket, pUser, "0x09F2", 1, "ITEM", true ); // makes a cooked rib
			socket.SysMessage( GetDictionaryEntry( 6073, socket.language )); // You cook a rib.
			pUser.SetTag("CookingDelay", null)
			break;
		case 7:
			CreateDFNItem( socket, pUser, "0x09B6", 1, "ITEM", true ); // makes fried eggs
			socket.SysMessage( GetDictionaryEntry( 6088, socket.language )); // You fry the eggs.
			pUser.SetTag("CookingDelay", null)
			break;
		case 8:
			CreateDFNItem( socket, pUser, "0x1040", 1, "ITEM", true ); // makes pizza
			socket.SysMessage( "You cook a pizza");
			pUser.SetTag("CookingDelay", null)
			break;
		case 9:
			CreateDFNItem( socket, pUser, "0x09EA", 1, "ITEM", true ); // makes muffins
			socket.SysMessage( "You cook a muffins");
			pUser.SetTag("CookingDelay", null)
			break;
	}
}
