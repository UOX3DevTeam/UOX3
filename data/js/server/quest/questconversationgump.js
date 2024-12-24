function QuestConversationGump( pUser, targObj, quest )
{
	var socket = pUser.socket;

	// Create a new gump
	var gump = new Gump();
	gump.AddPage( 0 );
	gump.AddBackground( 30, 120, 296, 447, 1579 ); // Background
	gump.AddGump( 70, 130, 1577 ); // Decorative gump
	gump.AddGump( 90, 280, 96 ); // Decorative gump
	gump.AddGump( 90, 380, 96 ); // Decorative gump
	gump.AddButton( 60, 500, 2450, 2452, 1, 0, 1 ); // Start quest button
	gump.AddButton( 220, 500, 2453, 2455, 1, 0, 0 ); // Cancel button

	var yOffset = 210;

	// Display quest details
	var step = quest.steps[0]; // Assuming we always show the first step for a new quest

	gump.AddHTMLGump( 50, yOffset, 264, 100, false, false,
		"<H1>Quest: " + quest.name + "</H1><br>" +
		"<H2>Description</H2>: " + step.description + "" );

	// Display objectives
	var objectives = "";
	for( var o = 0; o < step.objectives.length; o++ )
	{
		var obj = step.objectives[o];
		if( obj.type === "collect" )
		{
			objectives += "Collect: 0 / " + obj.count + "<br>";
		}
		else if( obj.type === "kill" && obj.targets && obj.targets.length )
		{
			for( var t = 0; t < obj.targets.length; t++ )
			{
				var target = obj.targets[t];
				objectives += "Kill: " + target.name + " 0 / " + target.count + "<br>";
			}
		} 
		else
		{
			objectives += obj.type + ": 0 / " + obj.count + "<br>";
		}
	}

	gump.AddHTMLGump( 50, yOffset + 100, 264, 200, false, false,
		"<H2>Objective: </H2><br>" + objectives );

	// Display rewards
	var rewards = step.rewards || {};
	var rewardText = "Gold: " + ( rewards.gold || 0 ) + "<br>";
	if( rewards.items && rewards.items.length > 0 ) {
		rewardText += "Items: " + rewards.items.join( ", " ) + "<br>";
	}

	gump.AddHTMLGump( 50, yOffset + 200, 250, 200, false, false,
		"<H2>Rewards</H2><br>" + rewardText );

	// Send the gump
	gump.Send( socket );
	gump.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	var questNpc = CalcCharFromSer( parseInt( pUser.GetTempTag( "questNpcSerial" )) );
	var questID = questNpc.GetTag( "QuestID" );
	switch ( pButton ) 
	{
		case 0:break;
		case 1: startQuest( pUser, questID ); break;// quest the player gets if he hits okay
	}
}

function onCharDoubleClick( pUser, questNpc )
{
	pUser.SetTempTag( "questNpcSerial", ( questNpc.serial ).toString(  ));
	var questID = questNpc.GetTag( "QuestID" );
	if( !questNpc.InRange( pUser, 2 ))
		return;

	// Validate the targeted object and player
	if( !ValidateObject( pUser ) || !ValidateObject( questNpc ))
	{
		pUser.SysMessage( "Invalid target or player." );
		return false;
	}

	// Check if the NPC has a quest to offer
	if( !questID ) 
	{
		pUser.SysMessage( questNpc.name + " has no quests to offer." );
		return false;
	}

	// Fetch the quest from the chainQuests
	var chainQuests = TriggerEvent( 50505, "getQuests" );
	var quest = null;

	for( var i = 0; i < chainQuests.length; i++ )
	{
		if( chainQuests[i].id === questID )
		{
			quest = chainQuests[i];
			break;
		}
	}

	if( !quest ) 
	{
		pUser.SysMessage( "The quest could not be found." );
		return false;
	}

	questNpc.TurnToward( pUser );
	// Show the quest conversation gump
	QuestConversationGump( pUser, questNpc, quest );
	return false;

}

function onContextMenuRequest( socket, targObj )
{
	// handle your own packet with context menu here
	var pUser = socket.currentChar;
	var offset = 12;
	var numEntries = 2;

	// Prepare packet
	var toSend = new Packet();
	var packetLen = ( 12 + ( numEntries * 8 ));
	toSend.ReserveSize( packetLen );
	toSend.WriteByte( 0, 0xBF );
	toSend.WriteShort( 1, packetLen );
	toSend.WriteShort( 3, 0x14 ); // subCmd
	toSend.WriteShort( 5, 0x0001 ); // 0x0001 for 2D client, 0x0002 for KR ( maybe this needs to be 0x0002? )
	toSend.WriteLong( 7, targObj.serial );
	toSend.WriteByte( 11, numEntries ); // Number of entries

	toSend.WriteShort( offset, 0x000A );    // Unique ID
	toSend.WriteShort( offset += 2, 6156 ); // Quest Conversation
	toSend.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
	toSend.WriteShort( offset += 2, 0x03E0 ); // Hue of text

	offset += 2; // for each additional entry

	toSend.WriteShort( offset, 0x000b );    // Unique ID
	toSend.WriteShort( offset += 2, 6155 ); // Cancel Quest
	toSend.WriteShort( offset += 2, 0x0020 ); // Flag, color enabled
	toSend.WriteShort( offset += 2, 0x03E0 ); // Hue of text

	//Send packet
	socket.Send( toSend );
	toSend.Free();

	return false;
}

function onContextMenuSelect( socket, targObj, popupEntry )
{
	switch ( popupEntry )
	{
		case 0x000A: // Quest Conversation
			{
				var pUser = socket.currentChar;

				pUser.SetTempTag( "questNpcSerial", ( targObj.serial ).toString(  ));
				// Validate the targeted object and player
				if( !ValidateObject( pUser ) || !ValidateObject( targObj ))
				{
					pUser.SysMessage( "Invalid target or player." );
					return false;
				}

				// Check if the NPC has a quest to offer
				var questID = targObj.GetTag( "QuestID" );
				if( !questID ) 
				{
					pUser.SysMessage( targObj.name  + " has no quests to offer." );
					return false;
				}

				// Fetch the quest from the chainQuests
				var chainQuests = TriggerEvent( 50505, "getQuests" );
				var quest = null;

				for( var i = 0; i < chainQuests.length; i++ )
				{
					if( chainQuests[i].id === questID )
					{
						quest = chainQuests[i];
						break;
					}
				}

				if( !quest )
				{
					pUser.SysMessage( "The quest could not be found." );
					return false;
				}

				// Show the quest conversation gump
				QuestConversationGump( pUser, targObj, quest );
			}
			break;

		case 0x000B: // Cancel Quest ( Optional )
			{
				socket.currentChar.SysMessage( "Quest cancellation not implemented yet." );
			}
			break;

		default:
			return true; // Let the default context menu handling proceed
	}

	return false; // Prevent default context menu handling
}


function startQuest( pUser, questID ) 
{
	var completedQuests = TriggerEvent( 50502, "ReadQuestLog", pUser ); // Read completed quests log
	var playerSerial = pUser.serial.toString(  );
	var questProgressArray = TriggerEvent( 50502, "ReadQuestProgress", pUser );
	var chainQuests = TriggerEvent( 50505, "getQuests" );

	// Check if the quest already exists in progress
	for( var i = 0; i < questProgressArray.length; i++ )
	{
		if( questProgressArray[i].questID === questID )
		{
			pUser.SysMessage( "You are already on this quest." );
			return;
		}
	}

	// Check if the player has already completed this quest
	for( var i = 0; i < completedQuests.length; i++ )
	{
		var questData = completedQuests[i].split( "," );
		if( questData[0] === playerSerial && questData[1] === questID )
		{
			pUser.SysMessage( "You have already completed this quest!" );
			return;
		}
	}

	// Find the corresponding quest
	var quest = null;
	for( var i = 0; i < chainQuests.length; i++ )
	{
		if( chainQuests[i].id === questID )
		{
			quest = chainQuests[i];
			break;
		}
	}

	if( !quest )
	{
		pUser.SysMessage( "Quest not found." );
		return;
	}

	// Initialize the quest progress
	var progress = [];
	for( var i = 0; i < quest.steps[0].objectives.length; i++ )
	{
		progress.push( 0 );
	}

	// Add the quest to the progress array
	questProgressArray.push( { questID: questID, step: 0, progress: progress, completed: false } );

	// Save the updated progress to the file
	TriggerEvent( 50502, "WriteQuestProgress", pUser, questProgressArray );

	pUser.SysMessage( "Quest started: " + quest.name );
}
