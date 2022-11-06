function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	socket.tempObj = iUsed;
	if( socket && iUsed && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pUser.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
		}
		else if( pUser.skills.carpentry < 900 )
		{
			pUser.SysMessage( GetDictionaryEntry( 2863, socket.language )); // You are not skilled enough as a carpenter to use this.
		}
		else
		{
			var targMsg = GetDictionaryEntry( 2864, socket.language ); // Target the corpse to make a trophy out of.
			socket.CustomTarget( 1, targMsg );
		}
	}
	return false;
}

function onCallback1( socket, myTarget )
{
	var pUser = socket.currentChar;

	if( socket.GetWord( 1 ) || !ValidateObject( myTarget ))
	{
		// No valid dynamic item was targeted
		socket.SysMessage( GetDictionaryEntry( 749, socket.language )); // That is not a corpse!
		return;
	}

	var iUsed = socket.tempObj;
	if( myTarget.id != 0x2006 && myTarget.sectionID != "big_fish" )
	{
		socket.SysMessage( GetDictionaryEntry( 749, socket.language )); // That is not a corpse!
		return;
	}
	else if( myTarget.GetTag( "usedForTrophy" ) == true )
	{
		socket.SysMessage( GetDictionaryEntry( 2865, socket.language )); // That corpse seems to have been visited by a taxidermist already.
		return;
	}
	else 
	{
		var woodID = 0x1bd7;
		var resourceCount = pUser.ResourceCount( woodID );
		var amountNeeded = 10;

		if( resourceCount >= amountNeeded )
		{
			var deedName = "";
			switch( myTarget.sectionID ) 
			{
				case "brownbear":
					deedName = "brownbeartrophydeed";
					break;
				case "hart":
					deedName = "stagtrophydeed";
					break;
				case "gorilla":
					deedName = "gorillatrophydeed";
					break;
				case "orc":
				case "cluborc":
					deedName = "orctrophydeed";
					break;
				case "polarbear":
					deedName = "polarbeartrophydeed";
					break;
				case "troll":
					deedName = "trolltrophydeed";
					break;
				case "bigfish":
					deedName = "bigfishdeed";
					break;
				default:
					socket.SysMessage( GetDictionaryEntry( 2868, socket.language )); // That does not look like something you want hanging on a wall.
					break;
			}

			if( deedName != "" )
			{
				// Create the trophy!
				if(	pUser.UseResource( amountNeeded, woodID ) == amountNeeded )
				{
					var trophyItem = CreateDFNItem( socket, socket.currentChar, deedName, 1, "ITEM", true );
					socket.SysMessage( GetDictionaryEntry( 2866, socket.language )); // You review the corpse and find it worthy of a trophy.
					socket.SysMessage( GetDictionaryEntry( 2867, socket.language )); // You use your kit up making the trophy.

					if( ValidateObject( trophyItem ))
					{
						// Delete target if it was a big fish
						if( myTarget.sectionID == "big_fish" )
						{
							myTarget.Delete();
						}
						else
						{
							// Otherwise, mark corpse as "used"
							myTarget.SetTag( "usedForTrophy", true );
						}

						// Finally, delete the taxidermy kit
						iUsed.Delete();
					}
				}
			}
		}
		else 
		{
			socket.SysMessage( GetDictionaryEntry( 2869, socket.language )); // You do not have enough boards.
			return;
		}
	}
}