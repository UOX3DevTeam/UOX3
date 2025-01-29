const PuzzleChestCylinder = {
	None: 0xE73,
	LightBlue: 0x186F,
	Blue: 0x186A,
	Green: 0x186B,
	Orange: 0x186C,
	Purple: 0x186D,
	Red: 0x186E,
	DarkBlue: 0x1869,
	Yellow: 0x1870
};

function PuzzleChestSolution( cylinders )
{
	this.cylinders = cylinders || [
		RandomCylinder(),
		RandomCylinder(),
		RandomCylinder(),
		RandomCylinder(),
		RandomCylinder()
	];
}

function RandomCylinder()
{
	var randomValue = RandomNumber( 0, 8 );
	switch ( randomValue )
	{
		case 0: return PuzzleChestCylinder.LightBlue;
		case 1: return PuzzleChestCylinder.Blue;
		case 2: return PuzzleChestCylinder.Green;
		case 3: return PuzzleChestCylinder.Orange;
		case 4: return PuzzleChestCylinder.Purple;
		case 5: return PuzzleChestCylinder.Red;
		case 6: return PuzzleChestCylinder.DarkBlue;
		default: return PuzzleChestCylinder.Yellow;
	}
}

function CompareSolutions( solutionCylinders, guessCylinders )
{
	var correctCylinders = 0;
	var correctColors = 0;
	var matchesSrc = [false, false, false, false, false];
	var matchesDst = [false, false, false, false, false];

	// Check for exact matches ( correct position and color )
	for ( var i = 0; i < solutionCylinders.length; i++ )
	{
		if( solutionCylinders[i] == guessCylinders[i] )
		{
			correctCylinders++;
			matchesSrc[i] = true;
			matchesDst[i] = true;
		}
	}

	// Check for color matches ( wrong position )
	for ( var i = 0; i < solutionCylinders.length; i++ )
	{
		if( !matchesSrc[i] )
		{
			for ( var j = 0; j < guessCylinders.length; j++ )
			{
				if( solutionCylinders[i] == guessCylinders[j] && !matchesDst[j] )
				{
					correctColors++;
					matchesDst[j] = true;
					break;
				}
			}
		}
	}

	return {
		correctCylinders: correctCylinders,
		correctColors: correctColors
	};
}

function onCreateDFN( objMade, objType ) 
{
	if( objType == 0 )
	{
		objMade.SetTag( "locked", "true" ); // Lock the chest by default
		var solution = new PuzzleChestSolution();
		objMade.SetTag( "solution", SerializeSolution( solution )); // Store the solution as a serialized string
	}
}

function onUseChecked( pUser, pItem )
{
	var socket = pUser.socket;
	socket.tempObj = pItem;

	if( pItem.GetTag( "locked" ) == "true" )
	{
		var serializedSolution = pItem.GetTag( "solution" );
		var solution = DeserializeSolution( serializedSolution );
		var lastGuess = pUser.GetTag( "lastGuess" );

		// Initialize lastGuess if it doesn't exist
		if( !lastGuess )
		{
			lastGuess = new PuzzleChestSolution();
			pUser.SetTag( "lastGuess", SerializeSolution( lastGuess ));
		}
		else
		{
			lastGuess = DeserializeSolution( pUser.GetTag( "lastGuess" ));
		}

		// Calculate the number of correct cylinders and colors
		var result = CompareSolutions( solution.cylinders, lastGuess.cylinders );

		// Generate hints based on player's lockpicking skill
		var hints = GenerateHints( pUser, result.correctCylinders, result.correctColors );

		// Show the puzzle gump to the player
		ShowPuzzleGump( pUser, pItem, solution, 0 ); // Pass the solution and default check = 0
	}
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	var pItem = pSock.tempObj; // Access the temporary object from the socket

	// Deserialize the puzzle solution and the last guess from the player's tags
	var solution = DeserializeSolution( pItem.GetTag( "solution" ));
	var lastGuess = DeserializeSolution( pUser.GetTag( "lastGuess" ));

	if( pButton == 1 )
	{
		// Submit button ( ID 1 )
		var result = CompareSolutions( solution.cylinders, lastGuess.cylinders );
		if( result.correctCylinders == 5 ) 
		{
			// Unlock the chest ifthe guess is correct
			pItem.SetTag( "locked", "false" );
			pUser.SysMessage( "The chest unlocks!" );
			pUser.SoundEffect( 0x241, true ); // Play unlock sound effect

			// Reward the player and clear the puzzle
			RewardPlayer( pUser, pItem );
			pUser.SetTag( "lastGuess", null );
			pItem.Delete(  ); // Delete the chest after it's unlocked
		} 
		else 
		{
			// Generate hints for incorrect guesses
			var hints = GenerateHints( pUser, result.correctCylinders, result.correctColors );

			// Show a status gump with hints and apply damage for incorrect guess
			ShowStatusGump( pUser, result.correctCylinders, result.correctColors, hints.hint1, hints.hint2 );
			DamagePlayer( pUser ); // Apply damage to the player for incorrect guesses
		}
	}
	else if( pButton >= 10 && pButton <= 17 ) 
	{
		// Handle left and right buttons to change cylinder color
		var selectedCylinderIndex = gumpData.getButton( 0 ); // Determine which cylinder was selected via radio buttons

		if( selectedCylinderIndex >= 0 && selectedCylinderIndex <= 4 ) 
		{
			// Handle the color change based on which button was pressed ( cylinder color buttons 10-17 )
			switch ( pButton )
			{
				case 10:
					lastGuess.cylinders[selectedCylinderIndex] = PuzzleChestCylinder.LightBlue;
					break;
				case 11:
					lastGuess.cylinders[selectedCylinderIndex] = PuzzleChestCylinder.Blue;
					break;
				case 12:
					lastGuess.cylinders[selectedCylinderIndex] = PuzzleChestCylinder.Green;
					break;
				case 13:
					lastGuess.cylinders[selectedCylinderIndex] = PuzzleChestCylinder.Orange;
					break;
				case 14:
					lastGuess.cylinders[selectedCylinderIndex] = PuzzleChestCylinder.Purple;
					break;
				case 15:
					lastGuess.cylinders[selectedCylinderIndex] = PuzzleChestCylinder.Red;
					break;
				case 16:
					lastGuess.cylinders[selectedCylinderIndex] = PuzzleChestCylinder.DarkBlue;
					break;
				case 17:
					lastGuess.cylinders[selectedCylinderIndex] = PuzzleChestCylinder.Yellow;
					break;
			}
		}

		// Update the player's last guess
		pUser.SetTag( "lastGuess", SerializeSolution( lastGuess ));

		// Redraw the puzzle gump to reflect the updated guess
		ShowPuzzleGump( pUser, pItem, lastGuess, selectedCylinderIndex );
	}
	else if( pButton == 100 )
	{
		// OK button in status gump ( ID 100 )
		// Reshow the puzzle gump after closing the status gump
		ShowPuzzleGump( pUser, pItem, lastGuess, 0 ); // Reshow the puzzle with the correct solution
	}
}

function ShowPuzzleGump( pUser, pItem, lastGuess, check )
{
	var myGump = new Gump;

	// Add the main background and title
	myGump.AddBackground( 25, 0, 500, 410, 0x53 );
	myGump.AddGump( 62, 20, 0x67, 0 ); // Title image
	myGump.AddHTMLGump( 80, 36, 110, 70, false, false, "<b>A Puzzle Lock</b>" );

	// Instructions
	myGump.AddHTMLGump( 214, 26, 270, 90, false, false,
		"Correctly choose the sequence of cylinders needed to open the latch. Each cylinder may potentially be used more than once. Beware! A false attempt could be deadly!" );

	// Left cylinder buttons
	AddLeftCylinderButton( myGump, 62, 130, PuzzleChestCylinder.LightBlue, 10 );
	AddLeftCylinderButton( myGump, 62, 180, PuzzleChestCylinder.Blue, 11 );
	AddLeftCylinderButton( myGump, 62, 230, PuzzleChestCylinder.Green, 12 );
	AddLeftCylinderButton( myGump, 62, 280, PuzzleChestCylinder.Orange, 13 );

	// Right cylinder buttons
	AddRightCylinderButton( myGump, 451, 130, PuzzleChestCylinder.Purple, 14 );
	AddRightCylinderButton( myGump, 451, 180, PuzzleChestCylinder.Red, 15 );
	AddRightCylinderButton( myGump, 451, 230, PuzzleChestCylinder.DarkBlue, 16 );
	AddRightCylinderButton( myGump, 451, 280, PuzzleChestCylinder.Yellow, 17 );

	var solution = DeserializeSolution( pItem.GetTag( "solution" ));
	// Call AddLockpickingHints with the solution from the chest
	AddLockpickingHints( myGump, pUser, solution );

	// Previous guess
	var lastGuess = DeserializeSolution( pUser.GetTag( "lastGuess" ));
	if( lastGuess )
	{
		myGump.AddHTMLGump( 127, 249, 170, 20, false, false, "Thy previous guess:" );
		myGump.AddBackground( 290, 247, 115, 25, 0x13EC );

		// Add each cylinder of the previous guess
		for ( var i = 0; i < lastGuess.cylinders.length; i++ )
		{
			AddCylinder( myGump, 281 + ( i * 22 ), 254, lastGuess.cylinders[i] );
		}
	}

	// Pedestals for current guess
	AddPedestal( myGump, 140, 270, lastGuess.cylinders[0], 0, check == 0 );
	AddPedestal( myGump, 195, 270, lastGuess.cylinders[1], 1, check == 1 );
	AddPedestal( myGump, 250, 270, lastGuess.cylinders[2], 2, check == 2 );
	AddPedestal( myGump, 305, 270, lastGuess.cylinders[3], 3, check == 3 );
	AddPedestal( myGump, 360, 270, lastGuess.cylinders[4], 4, check == 4 );

	// Submit button
	myGump.AddButton( 258, 370, 0xFA5, 0xFA7, 1, 0, 1 );

	// Send the gump to the player
	myGump.Send( pUser );
	myGump.Free(  ); // Clear this gump from UOX memory
}

function AddLockpickingHints( myGump, pUser, pChestSolution ) 
{
	var lockpickingSkill = pUser.baseskills.lockpicking; // Lockpicking skill in UOX3

	if( lockpickingSkill >= 600 )
	{
		myGump.AddHTMLGump( 160, 125, 230, 24, false, false, "Lockpicking hint:" ); // Display the hint header
		myGump.AddBackground( 159, 150, 230, 95, 0x13EC ); // Hint background

		// Add hints based on lockpicking skill thresholds
		if( lockpickingSkill >= 800 )
		{
			// Show the first correct cylinder from the chest solution
			myGump.AddHTMLGump( 165, 157, 200, 40, false, false, "In the first slot:" );
			AddCylinder( myGump, 350, 165, pChestSolution.cylinders[0] ); // Show the first cylinder

			// Indicate that a cylinder is used in an unknown slot
			myGump.AddHTMLGump( 165, 197, 200, 40, false, false, "Used in an unknown slot:" );
			AddCylinder( myGump, 350, 200, pChestSolution.cylinders[1] ); // Show the second cylinder as a hint

			if( lockpickingSkill >= 900 ) 
			{
				// Add a hint for the third correct cylinder
				AddCylinder( myGump, 350, 212, pChestSolution.cylinders[2] ); // Add third hint
			}
			if( lockpickingSkill >= 1000 )
			{
				// Add a hint for the fourth correct cylinder
				AddCylinder( myGump, 350, 224, pChestSolution.cylinders[3] ); // Add fourth hint
			}
		}
		else 
		{
			// Provide a basic hint ifthe player's skill is between 600-799
			myGump.AddHTMLGump( 165, 157, 200, 40, false, false, "Used in an unknown slot:" );
			AddCylinder( myGump, 350, 160, pChestSolution.cylinders[0] ); // Show the first cylinder as a basic hint

			if( lockpickingSkill >= 700 )
			{
				// Add a second hint ifthe player's skill is 700-799
				AddCylinder( myGump, 350, 172, pChestSolution.cylinders[1] ); // Add second hint for 700+ skill
			}
		}
	}
}

function AddLeftCylinderButton( myGump, x, y, cylinder, buttonID )
{
	myGump.AddBackground( x, y, 30, 30, 0x13EC );
	AddCylinder( myGump, x - 7, y + 10, cylinder );
	myGump.AddButton( x + 38, y + 9, 0x13A8, 0x13A9, 1, 0, buttonID );
}

function AddRightCylinderButton( myGump, x, y, cylinder, buttonID )
{
	myGump.AddBackground( x, y, 30, 30, 0x13EC );
	AddCylinder( myGump, x - 7, y + 10, cylinder );
	myGump.AddButton( x - 26, y + 9, 0x13A8, 0x13A9, 1, 0, buttonID );
}

function AddCylinder( myGump, x, y, cylinder )
{
	if( cylinder != PuzzleChestCylinder.None )
	{
		myGump.AddPicture( x, y, cylinder );
	}
	else
	{
		myGump.AddPicture( x + 9, y, cylinder );
	}
}

function AddPedestal( myGump, x, y, cylinder, switchID, initialState )
{
	myGump.AddPicture( x, y, 0xB10 );
	myGump.AddPicture( x - 23, y + 12, 0xB12 );
	myGump.AddPicture( x + 23, y + 12, 0xB13 );
	myGump.AddPicture( x, y + 23, 0xB11 );

	if( cylinder != PuzzleChestCylinder.None )
	{
		myGump.AddPicture( x, y + 2, 0x51A );
		AddCylinder( myGump, x - 1, y + 19, cylinder );
	} else {
		myGump.AddPicture( x, y + 2, 0x521 );
	}

	// Use AddRadio to allow selection of one cylinder at a time
	myGump.AddRadio( x + 7, y + 65, 0x868, initialState ? 1 : 0, switchID );
}

function ShowStatusGump( pUser, correctCylinders, correctColors )
{
	var myGump = new Gump;

	myGump.AddBackground( 50, 50, 300, 200, 0x13BE ); // Add background
	myGump.AddHTMLGump( 60, 60, 250, 25, false, false, "<center><b>Incorrect Guess!</b></center>" ); // Title text

	// Display number of correct guesses
	myGump.AddHTMLGump( 60, 90, 250, 25, false, false, "Correct Cylinders: " + correctCylinders );
	myGump.AddHTMLGump( 60, 120, 250, 25, false, false, "Correct Colors ( wrong position ): " + correctColors );

	// Add OK button
	myGump.AddButton( 150, 170, 0xFA5, 0xFA7, 1, 0, 100 ); // OK button

	// Send the gump to the player
	myGump.Send( pUser );
	myGump.Free(  );
}

function RewardPlayer( pUser, pItem )
{
	pUser.SysMessage( "You find some treasure inside the chest!" );
	CreateDFNItem( pUser.socket, pUser, "0x0eed", 500, "ITEM", true ); // Drop 500 gold
}

function GenerateHints( pUser, correctCylinders, correctColors ) 
{
	var lockpickingSkill = pUser.CheckSkill( 24, 0, 1000 );
	var hint1 = "";
	var hint2 = "";

	if( RandomNumber( 0, 1000 ) < lockpickingSkill )
	{
		if( correctCylinders > 0 )
		{
			hint1 = "One of the cylinders is in the first slot.";
		}
		if( correctColors > 0 )
		{
			hint2 = "Some colors are used, but thou art not certain where.";
		}
	}

	return { hint1: hint1, hint2: hint2 };
}

function DamagePlayer( pUser )
{
	var randomEffect = RandomNumber( 0, 3 );
	switch ( randomEffect )
	{
		case 0:
			pUser.SysMessage( "A toxic vapor envelops thee." );
			pUser.Damage( RandomNumber( 10, 40 ));
			break;
		case 1:
			pUser.SysMessage( "Searing heat scorches thy skin." );
			pUser.Damage( RandomNumber( 10, 40 ));
			break;
		case 2:
			pUser.SysMessage( "Pain lances through thee from a sharp metal blade." );
			pUser.Damage( RandomNumber( 10, 40 ));
			break;
		default:
			pUser.SysMessage( "Lightning arcs through thy body." );
			pUser.Damage( RandomNumber( 10, 40 ));
			break;
	}
}

function SerializeSolution( solution ) 
{
	return solution.cylinders.join( "," );
}

function DeserializeSolution( data )
{
	var cylinderStrings = data.split( "," );
	var cylinders = [];

	for ( var i = 0; i < cylinderStrings.length; i++ ) {
		cylinders.push( parseInt( cylinderStrings[i], 10 ));
	}

	return new PuzzleChestSolution( cylinders );
}
