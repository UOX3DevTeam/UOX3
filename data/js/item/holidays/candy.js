function onUseChecked(pUser, iUsed)
{
	var socket = pUser.socket;
	socket.tempObj = iUsed;

	var soundNumbers = [0x03A, 0x03B, 0x03C];
	var randomIndex = Math.floor( Math.random() * soundNumbers.length );
	var randomSoundNumber = soundNumbers[randomIndex];
	var Acidity = parseInt( pUser.GetTempTag( "Acidity" ));
	var Toothach = parseInt( pUser.GetTempTag( "toothach" ));

	if( iUsed.sectionID == "RedCandyCane" || iUsed.sectionID == "0x2bdd" || iUsed.sectionID == "0x2bde" || iUsed.sectionID == "GreenCandyCane" || iUsed.sectionID == "0x2bdf" || iUsed.sectionID == "0x2be0" )
	{
		if( Acidity == 30 ) 
		{
			socket.SysMessage( GetDictionaryEntry(5503, socket.language ));// The extreme pain in your teeth subsides.
			return;
		}
		else 
		{
			if( Acidity <= 30 ) 
			{
				pUser.SetTempTag( "Acidity", Acidity += 5 );
			}

			if ( Toothach == 0)
			{
				pUser.SetTempTag( "toothach", 1 );
				pUser.StartTimer( 1000, 0, true );
			}
		}
	}

	if( iUsed.sectionID == "gingerbreadcookie" || iUsed.sectionID == "0x2be1" || iUsed.sectionID == "0x2be2" ) 
	{
		var rand = RandomNumber(0, 1);
		if( rand == 1 )
		{
			const msg = [
				"Noooo!",
				"Please don't eat me... *whimper",
				"Not the face!",
				"Ahhhhhh! My foot’s gone!",
				"Please. No! I have gingerkids!",
				"No, no! I’m really made of poison. Really.",
				"Run, run as fast as you can! You can't catch me! I'm the gingerbread man!"
			];

			var randomIndex = getRandomIndex( msg );
			iUsed.TextMessage( msg[randomIndex] );
		}
		else
		{
			iUsed.Delete();
		}

	}
	else 
	{
		iUsed.Delete();
		socket.SysMessage( GetDictionaryEntry(5504, socket.language ));//You feel as if you could eat as much as you wanted!
		pUser.SoundEffect(randomSoundNumber, false);
	}
}

// Function to get a random index from an array
function getRandomIndex( array ) 
{
	return Math.floor( Math.random() * array.length );
}

function onTimer( pUser, timerID ) 
{
	var socket = pUser.socket;
	var Acidity = pUser.GetTempTag( "Acidity" );
	const phrases = [
		"ARRGH! My tooth hurts sooo much!",
		"You just can't find a good Britannian dentist these days...",
		"My teeth!",
		"MAKE IT STOP!",
		"AAAH! It feels like someone kicked me in the teeth!"
	];

	if( timerID == 0 )
	{
		if( pUser.GetTempTag( "toothach" ) == 1 )
		{
			if( Acidity == 30 ) 
			{
				socket.SysMessage( GetDictionaryEntry( 5503, socket.language ));// The extreme pain in your teeth subsides.
				pUser.StartTimer( 1000, 1, true );
				return;
			}
			if( Acidity > 0 )
			{
				pUser.SetTempTag("Acidity", Acidity -= 1);
				var randomIndex = getRandomIndex(phrases);
				pUser.TextMessage(phrases[randomIndex]);
				pUser.StartTimer(9000, 0, true);
			}
			else
			{
				pUser.StartTimer(1000, 1, true);
			}
		}
		else 
		{
			pUser.StartTimer( 1000, 1, true );
		}
	}

	if( timerID == 1 ) 
	{
		pUser.SetTempTag( "toothach", 0 );
		pUser.SetTempTag( "Acidity", 0 );
	}
}