function onCreateDFN( objMade, objType )
{
	//If you create the Bod with adminmenu or add command.
	var rand		= RandomNumber( 1, 49 );
	var bodEntry	= CreateEntries[rand];
	var itemName	= bodEntry.name; // name of the create entry
	var graphicID	= bodEntry.id; // section id of item to craft
	var weaponType  = TriggerEvent( 2500, "GetWeaponType", null, bodEntry.id );
	var init		= objMade.GetTag( "init" )
	if( objType == 0 && init == false )
	{
		objMade.SetTag( "itemName", itemName );
		objMade.SetTag( "graphicID", graphicID );

		if( weaponType == "WRESTLING" ) // check make sure its not a weapon before asking for ore type
		{
			var materialColor = 0;
			switch( RandomNumber( 0, 8 ))
			{
				case 0:
					materialColor = 1750; // bronze
					break;
				case 1:
					materialColor = 2414; // copper
					break;
				case 2:
					materialColor = 2425; // agapite
					break;
				case 3:
					materialColor = 2419;// dull copper
					break;
				case 4:
					materialColor = 2213; // gold
					break;
				case 5:
					materialColor = 2406; // shadow iron
					break;
				case 6:
					materialColor = 2219; // valorite
					break;
				case 7:
					materialColor = 2207; // verite
					break;
				case 8:
					materialColor = 0; // iron
					break;
				default:
					materialColor = 0;
					break;
			}
			objMade.SetTag( "materialColor", materialColor );
			objMade.SetTag( "reqMaterial", true );
		}
		objMade.Refresh();
	}
}

function onUseChecked( pUser, smallBOD )
{
	// Store iUsed as a custom property on pUser
	pUser.bodItem = smallBOD;
	var socket = pUser.socket;
	if( pUser.InRange( pUser, 2 ))
	{
		//Check to make sure you are not invisible.
		if( pUser.visible == 1 || pUser.visible == 2 )
		{
			pUser.visible = 0;
		}

		//Check to see if it's locked down
		if( smallBOD.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.language )); //That is locked down and you cannot use it
			return false;
		}
		else 
		{
			var init = smallBOD.GetTag( "init" ) // just to make sure we dont set the bod over
			if( init == false ) // Keep from resetting the amount needed.
			{
				var amountMax = 0;
				if( pUser.skills.blacksmithing >= 701 )
				{
					if((( pUser.skills.blacksmithing + 800 ) / 2 ) > RandomNumber( 0, 1000 ))
					{
						smallBOD.SetTag( "reqExceptional", 1 );
					}

					var values = [ 10, 15, 20, 20 ];
					amountMax = values[Math.floor( Math.random() * values.length )];
				}
				else if( pUser.skills.blacksmithing >= 501 )
				{					
					var values = [ 10, 15, 15, 20 ];
					amountMax = values[Math.floor( Math.random() * values.length )];
				}
				else
				{
					var values = [ 10, 10, 15, 20 ];
					amountMax = values[Math.floor( Math.random() * values.length )];					
				}
				smallBOD.SetTag( "init", true );
				smallBOD.SetTag( "amountMax", amountMax );
			}

			smallBOD.Refresh();
			SmallBODGump( pUser, smallBOD );
		}
	}
}

function SmallBODGump( pUser, smallBOD )
{
	var amountMax = parseInt( smallBOD.GetTag("amountMax" )); // amount you have to make of the item
	var itemName = smallBOD.GetTag( "itemName" ); // itemName of the item set when item is created
	var graphicID =smallBOD.GetTag( "graphicID" ); // graphicID of the item set when item is created
	var amountCur = parseInt( smallBOD.GetTag("amountCur" )); // amount you have combined to the bod
	var reqExceptional = parseInt( smallBOD.GetTag("reqExceptional" ));
	var reqMaterial = smallBOD.GetTag( "reqMaterial" );
	var socket = pUser.socket;
	var bodGump = new Gump;

	bodGump.AddPage(0);

	bodGump.AddBackground( 50, 10, 455, 260, 5054 );
	bodGump.AddTiledGump( 58, 20, 438, 241, 2624 );
	bodGump.AddCheckerTrans( 58, 20, 438, 241 );
	bodGump.AddGump( 45, 5, 10460 );
	bodGump.AddGump( 480, 5, 10460 );
	bodGump.AddGump( 45, 245, 10460 );
	bodGump.AddGump( 480, 245, 10460 );

	bodGump.AddHTMLGump( 225, 25, 120, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17250, socket.language ) + "</basefont>" ); // A bulk order

	bodGump.AddHTMLGump( 75, 48, 250, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17251, socket.language ) + "</basefont>"); // Amount to make:
	bodGump.AddText( 275, 48, 1152, amountMax );

	bodGump.AddHTMLGump( 275, 76, 200, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17252, socket.language ) + "</basefont>"); // Amount finished:
	bodGump.AddText( 275, 96, 0x480, amountCur );

	bodGump.AddHTMLGump( 75, 72, 120, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17253, socket.language ) + "</basefont>" ); // Item requested:

	if( reqExceptional == 1 )
	{
		bodGump.AddHTMLGump( 75, 96, 210, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17254, socket.language ) + " " + itemName + "</basefont>");// exceptional
	}
	else
	{
		bodGump.AddHTMLGump( 75, 96, 210, 20, 0, 0, "<basefont color=#ffffff>" + itemName + "</basefont>");
	}

	bodGump.AddPicture( 410, 72, graphicID );// image of the item requested
	if( reqExceptional == 1 || reqMaterial == true )
	{
		bodGump.AddHTMLGump( 75, 120, 200, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17255, socket.language ) + "</basefont>" ); // Special requirements to meet:
	}

	if( reqExceptional == 1 )
	{
		bodGump.AddHTMLGump( 75, 144, 300, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17256, socket.language ) + "</basefont>"); // All items must be exceptional.
	}

	if( reqMaterial == true ) // checks to see if a material is required for the bod.
	{
		if( reqExceptional == 1 )
		{
			var y = 168;
		}
		else
		{
			var y = 144;
		}
		var materialName = "";
		switch( smallBOD.GetTag( "materialColor" ))
		{
			case 0:
				materialName = "normal";
				break;
			case 2419:
				materialName = "dull copper";
				break;
			case 2406:
				materialName = "shadow iron";
				break;
			case 2414:
				materialName = "copper";
				break;
			case 1750:
				materialName = "bronze";
				break;
			case 2213:
				materialName = "gold";
				break;
			case 2425:
				materialName = "agapite";
				break;
			case 2207:
				materialName = "verite";
				break;
			case 2219:
				materialName = "valorite";
				break;
			default:
				materialName = "normal";
				break;
		}
		if( materialName != "" )
		{
			bodGump.AddHTMLGump( 75, y, 300, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17257, socket.language ) + " " + materialName + " material</basefont>" ); // All items must be made with 
		}
	}

	bodGump.AddButton( 125, 192, 4005, 4007, 1, 0, 1 );
	bodGump.AddHTMLGump( 160, 192, 300, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 17258, socket.language ) + "</basefont>" ); // Combine this deed with the item requested.

	bodGump.AddButton( 125, 216, 4005, 4007, 1, 0, 0 );
	bodGump.AddHTMLGump( 160, 216, 120, 20, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10290, socket.language ) + "</basefont>" ); // EXIT

	bodGump.Send( pUser );
	bodGump.Free();
}

function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;
	var smallBOD = pUser.bodItem;
	switch( pButton )
	{
		case 0:// Exit
			break;
		case 1:CombineItemWithBod( pUser, smallBOD );
			break;
	}
}

function CombineItemWithBod( pUser, smallBOD )
{
	var socket = pUser.socket;
	var amountMax = parseInt( smallBOD.GetTag( "amountMax" )); // amount you have to make of the item
	var amountCur = parseInt( smallBOD.GetTag( "amountCur" )); // amount you have combined

	if( amountCur >= amountMax )
	{
		pUser.TextMessage( GetDictionaryEntry( 17260, socket.language )); // The maximum amount of requested items have already been combined to this deed.
		return;
	}
	else
	{
		pUser.CustomTarget(0);
		return;
	}
}


function onCallback0( socket, myTarget )
{
	var pUser = socket.currentChar;
	var smallBOD = pUser.bodItem;
	var targID = smallBOD.GetTag( "graphicID" );
	var amountMax = parseInt(smallBOD.GetTag( "amountMax" )); // amount you have to make of the item
	var amountCur = parseInt(smallBOD.GetTag( "amountCur" )); // amount you have combined
	var reqExceptional = parseInt(smallBOD.GetTag( "reqExceptional" ));
	var materialColor = parseInt(smallBOD.GetTag( "materialColor" ));

	if( !socket.GetWord(1) && myTarget.isItem && myTarget.id == targID )
	{
		// That must be in your pack for you to use it.
		var itemOwner = GetPackOwner( myTarget, 0 );
		if( itemOwner == null || itemOwner != pUser ) 
		{
			socket.SysMessage( GetDictionaryEntry( 17259, socket.language )); // You must have the item in your backpack to target it.
			return;
		}
		else if( amountCur >= amountMax )
		{
			pUser.TextMessage( GetDictionaryEntry( 17260, socket.language )); // The maximum amount of requested items have already been combined to this deed.
			return;
		}
		else if( materialColor != myTarget.colour )
		{
			socket.SysMessage( GetDictionaryEntry( 17261, socket.language )); // The item is not made from the requested ore.
			return;
		}
		else
		{
			if( reqExceptional == 1 && myTarget.rank != 10 )
			{
				socket.SysMessage( GetDictionaryEntry( 17262, socket.language )); // The item must be exceptional.
				SmallBODGump( pUser, smallBOD );
				return;
			}
			else 
			{
				smallBOD.SetTag( "amountCur", amountCur + 1 );
				myTarget.Delete();
				pUser.TextMessage( GetDictionaryEntry( 17263, socket.language )); // The item has been combined with the deed.
				smallBOD.Refresh();
				if( amountCur < amountMax )
				{
					pUser.CustomTarget(0);
					SmallBODGump( pUser, smallBOD );
				}
			}
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 17264, socket.language )); // The item is not in the request.
		SmallBODGump( pUser, smallBOD );
	}
}

function onTooltip( smallBOD )
{
	var tooltipText = "";
	var amountMax = parseInt( smallBOD.GetTag( "amountMax" )); // amount you have to make of the item
	var amountCur = parseInt( smallBOD.GetTag( "amountCur" )); // amount you have combined
	var itemName = smallBOD.GetTag( "itemName" ); // name of the item set when item is created
	tooltipText = "amount to make : " + amountMax;
	tooltipText += "\n" + itemName + " : " + amountCur;
	return tooltipText;
}