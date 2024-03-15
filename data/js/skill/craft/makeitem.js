const recipeEnabled = true;

function makeitem( pSock, bItem, makeID, resourceHue, recipeID, anvil, gumpDelay, timerID, itemDetailsID, itemDetailsScriptID )
{
	var pUser = pSock.currentChar;

	RecipeCheck( pSock, recipeID );

	if( makeID != 0 )
	{
		if( anvil > 0 || anvil == null )
		{
			// crafting_complete.js for applying special bonuses for exceptional equipment/from runic hammers
			pUser.AddScriptTrigger( 4033 );
			if( resourceHue != 0 )
			{
				MakeItem( pSock, pUser, makeID, resourceHue );
			}
			else
			{
				MakeItem( pSock, pUser, makeID );
			}

			var toolUseLimit = GetServerSetting( "ToolUseLimit" );
			var toolUseBreak = GetServerSetting( "ToolUseBreak" );

			// Check if player had runic hammer equipped while crafting
			var runicHammer = pUser.FindItemLayer( 0x01 ); // Right Hand
			if( ValidateObject( runicHammer ) && runicHammer.GetTag( "runicHammer" ) && runicHammer.usesLeft > 0 )
			{
				// Store some temp tags on player to get info on runic hammer used in crafting_complete.js
				pUser.SetTempTag( "usedRunicHammer", true );
				pUser.SetTempTag( "runicHammerType", runicHammer.color );

				// Wear and tear for equipped runic hammer, even if another tool was used to craft
				if( toolUseLimit && runicHammer != bItem )
				{
					runicHammer.usesLeft -= 1;
					if( runicHammer.usesLeft == 0 && toolUseBreak )
					{
						runicHammer.Delete();
						pSock.SysMessage( GetDictionaryEntry( 10202, pSock.language )); // You have worn out your tool!
						pSock.SoundEffect(0x051, true);
						// Play sound effect of tool breaking
					}
				}
			}

			if( toolUseLimit )
			{
				bItem.usesLeft -= 1;
				if( bItem.usesLeft == 0 && toolUseBreak )
				{
					bItem.Delete();
					pSock.SysMessage( GetDictionaryEntry(10202, pSock.language )); // You have worn out your tool!
					// Play sound effect of tool breaking
				}
			}
		}
		pUser.StartTimer( gumpDelay, timerID, true );
	}
	else if( itemDetailsID != 0 )
	{
		pUser.SetTempTag( "ITEMDETAILS", itemDetailsID );
		TriggerEvent( itemDetailsScriptID, "ItemDetailGump", pUser );
	}
}

function RecipeCheck( pSock, recipeID )
{
	if( recipeID != 0 && recipeEnabled )
	{
		var results = TriggerEvent( 4038, "NeedRecipe", pSock, recipeID );

		if( results == false )
			return false;
	}
}

function _restorecontext_() {}