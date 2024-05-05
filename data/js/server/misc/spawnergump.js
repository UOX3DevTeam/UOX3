function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	socket.tempObj = iUsed;
	var gumpID = 5037 + 0xffff;

	if( socket && iUsed && iUsed.isItem )
	{
		socket.CloseGump( gumpID, 0 );
		spawnerGump( socket, pUser, iUsed );
	}

	return false;
}

function spawnerGump( socket, pUser, iUsed )
{
	var spawner = new Gump;
	var powerState = "";
	var spawnName = "";
	if( iUsed.sectionalist == true )
	{
		powerState = "<basefont color=#00ff00><big>Enabled</big></basefont>";
		if( iUsed.type == 61 ) 
		{
			spawnName = "Item List"
		}
		else
			spawnName = "Npc List"
	}
	else 
	{
		powerState = "<basefont color=#ff2800><big>Disabled</big></basefont>";
		if( iUsed.type == 61 ) 
		{
			spawnName = "Item"
		}
		else
			spawnName = "Npc"
	}

	var typeName = "";
	switch( iUsed.type )
	{
		case 61: typeName = "<basefont color=#00ff00><big>Item</big></basefont>"; break
		case 62: typeName = "<basefont color=#00ff00><big>Npc</big></basefont>"; break
		case 125: typeName = "<basefont color=#00ff00><big>Escort</big></basefont>"; break
	}

	var amountLabel = ( iUsed.type != 61 ) ? "<basefont color=#ffffff>Amount</basefont>" : "<basefont color=#ffffff>Spawn Item DFN</basefont>";
	var applyLabel = "<basefont color=#ffffff>Apply</basefont>";
	var minLabel = "<basefont color=#ffffff>Min:</basefont>";
	var maxLabel = "<basefont color=#ffffff>Max:</basefont>";

	spawner.AddPage( 0 );

	spawner.AddBackground( 40, 80, 413, 134, 5054 );
	spawner.AddCheckerTrans( 40, 80, 413, 134 );
	spawner.AddPicture( 50, 100, 7956 );

	if( iUsed.type != 61 )
	{
		spawner.AddHTMLGump( 310, 100, 178, 22, false, false, amountLabel );
		spawner.AddHTMLGump( 90, 100, 203, 22, false, false, "<basefont color=#ffffff>Spawn " + spawnName + " DFN</basefont>" );

		spawner.AddHTMLGump( 255, 155, 140, 22, false, false, "<basefont color=#ffffff>Rename</basefont >" );

		spawner.AddButton( 50, 180, 4005, 4007, 1, 0, 1 );
		spawner.AddHTMLGump( 90, 180, 140, 22, false, false, applyLabel );

		spawner.AddHTMLGump( 80, 152, 140, 22, false, false, minLabel );
		spawner.AddHTMLGump( 160, 152, 140, 22, false, false, maxLabel );

		spawner.AddCheckbox( 130, 180, 2152, 0, 1 );
		spawner.AddHTMLGump( 160, 185, 140, 22, false, false, "<basefont color=#ffffff>Spawnlist: " + powerState + "</basefont>" );
		spawner.AddCheckbox( 300, 180, 2152, 0, 2 );
		spawner.AddHTMLGump( 330, 185, 140, 22, false, false, "<basefont color=#ffffff>Spawn Type: " + typeName + "</basefont>" );
		spawner.AddBackground( 80, 120, 201, 28, 5120 );
		spawner.AddBackground( 305, 120, 134, 28, 5120 );
		spawner.AddBackground( 105, 150, 50, 25, 5120 );
		spawner.AddBackground( 190, 150, 50, 25, 5120 );
		spawner.AddBackground(305, 152, 134, 28, 5120);

		spawner.AddText( 90, 125, 0, iUsed.spawnsection );
		spawner.AddText( 315, 125, 0, iUsed.amount );
		spawner.AddText( 110, 155, 0, iUsed.mininterval );
		spawner.AddText( 195, 155, 0, iUsed.maxinterval );
		spawner.AddText(310, 152, 0, iUsed.name);

		spawner.AddTextEntry( 90, 125, 178, 20, 1153, 0, 8, iUsed.spawnsection );
		spawner.AddTextEntry( 315, 125, 115, 20, 1153, 0, 9, iUsed.amount );
		spawner.AddTextEntry( 110, 155, 40, 20, 1153, 0, 10, iUsed.mininterval );
		spawner.AddTextEntry( 195, 155, 40, 20, 1153, 0, 11, iUsed.maxinterval );
		spawner.AddTextEntry(310, 152, 140, 25, 1153, 0, 12, iUsed.name);
	}
	else
	{
		spawner.AddHTMLGump( 90, 100, 203, 22, false, false, "<basefont color=#ffffff>Spawn " + spawnName + " DFN</basefont>" );
		spawner.AddHTMLGump( 255, 155, 140, 22, false, false, "<basefont color=#ffffff>Rename</basefont >" );
		spawner.AddButton( 50, 180, 4005, 4007, 1, 0, 1 );
		spawner.AddHTMLGump( 90, 180, 140, 22, false, false, applyLabel );

		spawner.AddHTMLGump( 80, 152, 140, 22, false, false, minLabel );
		spawner.AddHTMLGump( 160, 152, 140, 22, false, false, maxLabel );

		spawner.AddCheckbox( 130, 180, 2152, 0, 1 );
		spawner.AddHTMLGump( 160, 185, 140, 22, false, false, "<basefont color=#ffffff>Spawnlist: " + powerState + "</basefont>" );
		spawner.AddCheckbox( 300, 180, 2152, 0, 2 );
		spawner.AddHTMLGump( 330, 185, 140, 22, false, false, "<basefont color=#ffffff>Spawn Type: " + typeName + "</basefont>" );

		spawner.AddBackground( 80, 120, 201, 28, 5120 );
		spawner.AddBackground( 105, 150, 50, 25, 5120 );
		spawner.AddBackground( 190, 150, 50, 25, 5120 );
		spawner.AddBackground(305, 152, 134, 28, 5120);

		spawner.AddText( 90, 125, 0, iUsed.spawnsection );
		spawner.AddText( 110, 155, 0, iUsed.mininterval );
		spawner.AddText( 195, 155, 0, iUsed.maxinterval );
		spawner.AddText( 310, 152, 0, iUsed.name );

		spawner.AddTextEntry( 90, 125, 178, 20, 1153, 0, 11, iUsed.spawnsection );
		spawner.AddTextEntry( 110, 155, 40, 20, 1153, 0, 13, iUsed.maxinterval );
		spawner.AddTextEntry( 195, 155, 40, 20, 1153, 0, 12, iUsed.mininterval );
		spawner.AddTextEntry( 310, 152, 140, 25, 1153, 0, 10, iUsed.name );

	}
	spawner.Send( socket );
	spawner.Free();
}

function onGumpPress( socket, pButton, gumpData )
{
	var pUser = socket.currentChar;
	var iUsed = socket.tempObj;

	var spawn = gumpData.getEdit( 0 );
	if( iUsed.type != 61 ) 
	{
		var num = gumpData.getEdit( 1 );
		var min = gumpData.getEdit( 2 );
		var max = gumpData.getEdit( 3 );
		var name = gumpData.getEdit( 4 );
	}
	else 
	{
		var min = gumpData.getEdit( 1 );
		var max = gumpData.getEdit( 2 );
		var name = gumpData.getEdit( 3 );
	}
	var OtherButton = gumpData.getButton( 0 );
	switch( pButton ) 
	{
		case 0:
			break;
		case 1:
			if( iUsed.type != 61 )
			{
				iUsed.spawnsection = spawn;
				iUsed.amount = num;
				iUsed.mininterval = min;
				iUsed.maxinterval = max;
			}
			else
			{
				iUsed.spawnsection = spawn;
				iUsed.mininterval = min;
				iUsed.maxinterval = max;
			}

			if( name == null || name == " " )
			{
				socket.SysMessage(GetDictionaryEntry(9270, socket.language)); // That name is too short, or no name was entered.
			}
			else if( name.length > 50 )
			{
				pSocket.SysMessage(GetDictionaryEntry(9271, pSocket.language)); // That name is too long. Maximum 50 chars.
			}
			else
			{
				iUsed.name = name;
			}

			switch( OtherButton ) 
			{
				case 1:
					if( iUsed.sectionalist == true ) 
					{
						pUser.SysMessage( "You have disabled the spawn list. You can now add single DFN." );
						iUsed.sectionalist = false;
					}
					else
					{
						pUser.SysMessage( "You have enabled the spawn list. It will now only accept DFN lists." );
						iUsed.sectionalist = true;
					}
					break;
				case 2:
					var typeTransitions = {
						61: { nextType: 62, message: "Changed Type to Npc", resetAmount: false },
						62: { nextType: 125, message: "Changed Type to Escort", resetAmount: false },
						125: { nextType: 61, message: "Changed Type to Item", resetAmount: true }
					};

					var currentTransition = typeTransitions[iUsed.type];
					if( currentTransition ) 
					{
						iUsed.type = currentTransition.nextType;
						if( currentTransition.resetAmount ) 
						{
							iUsed.amount = 0;
						}
						pUser.SysMessage( currentTransition.message );
					}
					break;
			}
			spawnerGump( socket, pUser, iUsed );
			break;
	}
}

function onTooltip( spawner )
{
	var typeName = "";
	switch( spawner.type ) 
	{
		case 61: typeName = "<basefont color=#00ff00><big>Item</big></basefont>"; break
		case 62: typeName = "<basefont color=#00ff00><big>Npc</big></basefont>"; break
		case 125: typeName = "<basefont color=#00ff00><big>Escort</big></basefont>"; break
	}
	var powerState = "";
	if( spawner.sectionalist == true ) 
	{
		powerState = "<basefont color=#00ff00><big>Enabled</big></basefont>";
	}
	else
	{
		powerState = "<basefont color=#ff2800><big>Disabled</big></basefont>";
	}

	var tooltipText = "";
	tooltipText = "Spawn DFN: " + spawner.spawnsection;
	tooltipText += "\n" + "Spawn Type: " + typeName;
	tooltipText += "\n" + "Spawn List: " + powerState;
	tooltipText += "\n" + "Amount: " + "<basefont color=#00ff00>" + spawner.amount + "</big></basefont >";
	tooltipText += "\n" + "Min Interval: " + "<basefont color=#00ff00>" + spawner.mininterval + "</big></basefont>" + " Max Interval: " + "<basefont color=#00ff00>" + spawner.maxinterval + "</big></basefont>";
	tooltipText += "\n" + "Region: " + "<basefont color=#00ff00>" + spawner.region.name + "</big></basefont>";
	return tooltipText;
}