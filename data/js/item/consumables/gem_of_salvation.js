function onDeath( pDead, iCorpse )
{
	if( !ValidateObject( pDead ) || pDead.npc || !pDead.online )
	{
		return false;
	}

	var gemOfSalvation = pDead.FindItemSection( "gemofsalvation" );
	if( ValidateObject( gemOfSalvation ))
	{
		// Player has a Gem of Salvation in their pack, give them chance to resurrect!
		// Fetch timestamp for when they last used it
		var lastUsedGoS = parseInt( pDead.GetTag( "lastUsedGoS" ));
		if( lastUsedGoS > 0 && ( Date.now() / 1000 ) - lastUsedGoS < 21600 ) // 21600 seconds / 360 minutes / 6 hours
		{
			pDead.socket.SysMessage( GetDictionaryEntry( 18500, pDead.socket.language ), Math.ceil(( lastUsedGoS + 21600 - ( Date.now() / 1000 )) / 60 )); // Your spirit lacks cohesion. You must wait %i minutes before invoking the power of a Gem of Salvation.
		}
		else
		{
			pDead.socket.tempObj = gemOfSalvation;
			pDead.frozen = true;
			DisplayResurrectGump( pDead );
		}
	}

	// Allow any other onDeath scripts to also run
	return false;
}

// Display self-resurrection gump to player because they had a Gem of Salvation in their inventory on death
function DisplayResurrectGump( pDead )
{
	var pSock = pDead.socket;
	if( pSock == null )
		return;

	var pLang = pSock.language;

	var gemResurrectGump = new Gump;
	gemResurrectGump.NoClose();
	gemResurrectGump.NoDispose();
	gemResurrectGump.AddPage( 0 );
	gemResurrectGump.AddBackground( 0, 0, 400, 350, 2600 );

	gemResurrectGump.AddHTMLGump( 0, 20, 400, 35, 0, 0, ( "<CENTER>" + GetDictionaryEntry( 18502, pLang ) + "</CENTER>" )); // Resurrection

	// It is possible for you to be resurrected here by this healer. Do you wish to try?<br>CONTINUE - You chose to try to come back to life now.<br>CANCEL - You prefer to remain a ghost for now.
	gemResurrectGump.AddHTMLGump( 50, 55, 300, 140, 0, 0, "<CENTER>" + GetDictionaryEntry( 18503, pLang ) + "</CENTER>" );

	gemResurrectGump.AddButton( 200, 227, 4005, 4007, 1, 0, 0 ); // CANCEL
	gemResurrectGump.AddHTMLGump( 235, 230, 110, 35, 0, 0, "<CENTER>" + GetDictionaryEntry( 2709, pLang ) + "</CENTER>" );

	gemResurrectGump.AddButton( 65, 227, 4005, 4007, 1, 0, 1 ); // CONTINUE
	gemResurrectGump.AddHTMLGump( 100, 230, 110, 35, 0, 0, "<CENTER>" + GetDictionaryEntry( 2708, pLang ) + "</CENTER>" );

	gemResurrectGump.Send( pSock );
	gemResurrectGump.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pDead = pSock.currentChar;

	// Don't continue if character is invalid
	if( !ValidateObject( pDead ))
		return;

	var gemOfSalvation = pSock.tempObj;
	pSock.tempObj = null;
	if( !ValidateObject( gemOfSalvation ))
	{
		return;
	}

	switch( pButton )
	{
		case 0: // Cancel, save gem for another time
			break;
		case 1: // Resurrect
			pDead.SoundEffect( 0x0214, true );
			pDead.Resurrect();
			pDead.SysMessage( GetDictionaryEntry( 18501, pSock.language )); // The gem infuses you with its power and is destroyed in the process.

			// Store a timestamp for when Gem of Salvation was last used
			pDead.SetTag( "lastUsedGoS", Math.round( Date.now() / 1000 ).toString() );

			if( gemOfSalvation.amount > 1 )
			{
				// If gem is pileable, reduce one from the pile
				gemOfSalvation.amount--;
			}
			else
			{
				// Otherwise, delete item
				gemOfSalvation.Delete();
			}
		default:
			break;
	}

	pDead.frozen = false;
}

function _restorecontext_() {}
