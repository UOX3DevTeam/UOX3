// Power Generators spawn Golems and Golem Controllers periodically until they are destroyed
// They can be destroyed via a control panel with a randomized puzzle that needs to be solved
// If incorrect options are selected in this puzzle, the player is shocked with electricity
// If they manage to solve the entire puzzle, the power generator is deactivated and they
// get some loot directly injected into their backpacks, and shadow iron ore spawned on the ground

const powerGenScriptID = 5036;
const powerGenLockpickReq = 650;
const powerGenShowTakenPath = true; // Show player's (successful) path through the puzzle?

// Upon spawning, add the rest of the items making up the power generator
function onCreateDFN( objMade, objType )
{
	// Start timer to create sub-components
	objMade.StartTimer( 100, 1, powerGenScriptID );

	// Remove script attached to this object
	objMade.RemoveScriptTrigger( powerGenScriptID );
}

function onTimer( timerObj, timerID )
{
	if( timerID == 1 )
	{
		// Initial setup
		// Add structural components and move them into place
		// First component is 0x73, which is also the component that's spawned to begin with
		var component2 = CreateDFNItem( null, null, "powergenerator_2", 1, "ITEM", false );
		if( ValidateObject( component2 ))
		{
			component2.Teleport( timerObj.x - 1, timerObj.y, timerObj.z, timerObj.worldnumber, timerObj.instanceID );
		}

		var component3 = CreateDFNItem( null, null, "powergenerator_3", 1, "ITEM", false );
		if( ValidateObject( component3 ))
		{
			component3.Teleport( timerObj.x, timerObj.y - 1, timerObj.z, timerObj.worldnumber, timerObj.instanceID );
		}

		var component4 = CreateDFNItem( null, null, "powergenerator_4", 1, "ITEM", false );
		if( ValidateObject( component4 ))
		{
			component4.Teleport( timerObj.x, timerObj.y, timerObj.z + 13, timerObj.worldnumber, timerObj.instanceID );
		}

		var component5 = CreateBlankItem( null, null, 1, "energy", 0x3818, 0, "ITEM", false );
		if( ValidateObject( component5 ))
		{
			component5.Teleport( timerObj.x, timerObj.y, timerObj.z + 28, timerObj.worldnumber, timerObj.instanceID );
		}

		// Add control panel
		var controlPanel = CreateDFNItem( null, null, "powergenerator_ctrlpanel", 1, "ITEM", false );
		if( ValidateObject( controlPanel ))
		{
			// Move control panel into place
			controlPanel.Teleport( timerObj.x + 1, timerObj.y, timerObj.z - 2, timerObj.worldnumber, timerObj.instanceID );

			// Store reference to other pieces on control panel
			controlPanel.SetTag( "component1", ( timerObj.serial ).toString() );
			controlPanel.SetTag( "component2", ( component2.serial ).toString() );
			controlPanel.SetTag( "component3", ( component3.serial ).toString() );
			controlPanel.SetTag( "component4", ( component4.serial ).toString() );
			controlPanel.SetTag( "component5", ( component5.serial ).toString() );

			// Assign a random value to the control panel to use for the puzzle
			controlPanel.morex = RandomNumber( 3, 6 );

			// Initialize the puzzle
			InitializePuzzle( controlPanel );

			// Assign this script to the controlPanel, so it can be interacted with
			controlPanel.AddScriptTrigger( powerGenScriptID );
		}
	}
	else if( timerID == 2 || timerID == 3 || timerID == 4 )
	{
		timerObj.frozen = true;
		timerObj.StartTimer( 1000, 10, powerGenScriptID );
		timerObj.socket.SoundEffect( 0x1f4, true);
		timerObj.TextMessage( GetDictionaryEntry( 2796, timerObj.socket.language ), false, 0x3b2, 0, timerObj.serial ); // * Your body convulses from electric shock *
		timerObj.BoltEffect();
		timerObj.Damage( 10, 6 );

		if( timerID < 4 )
		{
			timerObj.StartTimer( 5000, timerID + 1, powerGenScriptID );
		}
	}
	else if( timerID == 10 )
	{
		// Unfreeze player
		timerObj.frozen = false;
	}
}

// When player interacts with the control panel, bring up gump
function onUseUnChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;
	if( pSocket == null )
		return false;

	if( DistanceBetween( pUser, iUsed, true ) > 3 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 393, pSocket.language )); // That is too far away.
		return false;
	}

	// Check if control panel has been accessed in the last X seconds, if so, it's probably being used by another player!
	var lastUsedAt = parseInt( iUsed.GetTag( "lastUsedAt" ));
	if( lastUsedAt > 0 && ( GetCurrentClock() / 1000 ) - lastUsedAt < 120 ) // 120 seconds / 2 minutes
	{
		pUser.SysMessage( GetDictionaryEntry( 2450, pSocket.language )); // Someone else is using this right now.
		return false;
	}

	// Store timestamp for when control panel was last used
	iUsed.SetTag( "lastUsedAt", Math.round( GetCurrentClock() / 1000 ).toString() );

	// Store a reference to the control panel on player
	pUser.SetTempTag( "powerGenControlPanelSer", iUsed.serial.toString() );

	// Fetch player's current progress (if any)
	var currentStepTag = "progress_" + iUsed.serial;
	var currentStep = parseInt( pUser.GetTempTag( currentStepTag )) || 0;

	// Display Gump for Control Panel
	ControlPanelGump( pSocket, iUsed, currentStep );
	return false;
}

function ControlPanelGump( pSocket, controlPanel, step )
{
	var pUser = pSocket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	var rndValue = controlPanel.morex;
	var ctrlPanelGump = new Gump();

	// Main background and dragons along the side
	ctrlPanelGump.AddBackground( 50, 0, 530, 410, 0x0a28 );
	ctrlPanelGump.AddGump( 0, 0, 0x28c8 );
	ctrlPanelGump.AddGump( 547, 0, 0x28c9 );

	// Gump header
	ctrlPanelGump.AddBackground( 95, 20, 442, 90, 0x0a28 );
	ctrlPanelGump.AddHTMLGump( 229, 35, 300, 45, false, false, "GENERATOR CONTROL PANEL" );
	ctrlPanelGump.AddHTMLGump( 223, 60, 300, 70, false, false, "Use the Directional Controls to" );
	ctrlPanelGump.AddHTMLGump( 253, 75, 300, 85, false, false, "Close the Grid Circuit" );
	ctrlPanelGump.AddGump( 140, 40, 0x28d3 );
	ctrlPanelGump.AddGump( 420, 40, 0x28d3 );

	// Directional controls
	ctrlPanelGump.AddBackground( 365, 120, 178, 210, 0x1400 );
	ctrlPanelGump.AddGump( 365, 115, 0x28d4 );
	ctrlPanelGump.AddGump( 365, 288, 0x28d4 );
	ctrlPanelGump.AddGump( 414, 189, 0x0589 );
	ctrlPanelGump.AddGump( 435, 210, 0x0a52 );
	ctrlPanelGump.AddButton( 408, 222, 0x29ea, 0x29ec, 1, 0, 1 ); // Left
	ctrlPanelGump.AddButton( 448, 185, 0x29cc, 0x29ce, 1, 0, 2 ); // Up
	ctrlPanelGump.AddButton( 473, 222, 0x29d6, 0x29d8, 1, 0, 3 ); // Right
	ctrlPanelGump.AddButton( 448, 243, 0x29e0, 0x29e2, 1, 0, 4 ); // Down

	// Circuit board
	ctrlPanelGump.AddBackground( 90, 115, 30 + 40 * rndValue, 30 + 40 * rndValue, 0x0a28 );
	ctrlPanelGump.AddBackground( 100, 125, 10 + 40 * rndValue, 10 + 40 * rndValue, 0x1400 );
	for( let i = 0; i < rndValue; i++ )
	{
		for( let j = 0; j < rndValue - 1; j++ )
		{
			ctrlPanelGump.AddGump( 120 + 40 * i, 162 + 40 * j, 0x13f9 );
		}
	}
	for( let i = 0; i < rndValue - 1; i++ )
	{
		for( let j = 0; j < rndValue; j++ )
		{
			ctrlPanelGump.AddGump( 138 + 40 * i, 147 + 40 * j, 0x13fd );
		}
	}

	var pathString = controlPanel.GetTag( "pathString" );
	if( typeof pathString !== "string" || !pathString )
	{
	    Console.Print( "Error: Invalid pathString value on controlPanel, in ControlPanelGump: " + controlPanel.serial );
	    return;
	}
	var pathArray = pathString.split( "|" ).map( function( str ) {
		var coords = str.split( "," );
		return { x: parseInt( coords[0] ), y: parseInt( coords[1] ) };
	});

	var hues = [[,]];
	for( let i = 0; i < rndValue; i++ )
	{
		for( let j = 0; j < rndValue; j++ )
		{
			hues[[i,j]] = 0x868; //0x25F8;
		}
	}

	for( let i = 0; i <= step; i++ )
	{
		hues[[pathArray[i].x, pathArray[i].y]] = 0x868; // Blue
	}

	// Keep track of player's visited nodes in the gump, and display them if enabled by powerGenShowTakenPath flag
	var visitedNodes = ( pUser.GetTempTag( "visitedPath_" + controlPanel.serial ) || "" ).split( "|" ).map( function( str ) {
	    var coords = str.split( "," );
	    return { x: parseInt( coords[0] ), y: parseInt( coords[1] ) };
	});

	// Set up buttons in grid
	for( let i = 0; i < rndValue; i++ )
	{
		for( let j = 0; j < rndValue; j++ )
		{
			var hue;
			if( i == rndValue - 1 && j == rndValue - 1 )
			{
				hue = 0x09a8; // Last node is red
			}
			else if( powerGenShowTakenPath && NodeVisited( visitedNodes, i, j ))
			{
				hue = 0x86a; //0x16d3;
			}
			else
			{
				hue = hues[[i,j]];
			}
			ctrlPanelGump.AddGump(110 + 40 * i, 135 + 40 * j, hue);
		}
	}

	// Check if the hint should be displayed
	var showHint = ( pUser.GetTempTag( "hintFor_" + controlPanel.serial ));
	if( showHint )
	{
		var nextStep = parseInt( pUser.GetTempTag( "progress_" + controlPanel.serial )) + 1;
		if( nextStep < pathArray.length )
		{
			ctrlPanelGump.AddGump( 119 + 40 * pathArray[nextStep].x, 143 + 40 * pathArray[nextStep].y, 0x939 ); // Green gem for hint
		}
	}

	// Display player's progress (if any)
	ctrlPanelGump.AddGump( 118 + 40 * pathArray[step].x, 143 + 40 * pathArray[step].y, 0x13a8 );

	// Display button to decipher next step, if player's lockpicking is high enough
	if( pUser.skills.lockpicking >= powerGenLockpickReq ) // over 65.0
	{
		ctrlPanelGump.AddButton( 365, 350, 0xfa6, 0xfa7, 1, 0, 5 ); // Attempt to show next correct position
		ctrlPanelGump.AddHTMLGump( 405, 345, 140, 40, false, false, "Attempt to Decipher the Circuit Path" );
	}

	ctrlPanelGump.Send( pSocket );
	ctrlPanelGump.Free();
}

// Check if current x, y node has already been visited by player in the past
function NodeVisited( visitedNodes, x, y )
{
    for( var k = 0; k < visitedNodes.length; k++ )
    {
        if( visitedNodes[k].x == x && visitedNodes[k].y == y )
        {
            return true;
        }
    }
    return false;
}

function IsValidMove( x, y, rndValue, visited )
{
	return x >= 0 && x < rndValue && y >= 0 && y < rndValue && !visited[x + "," + y];
}

function GetNeighbors( x, y, rndValue, visited )
{
	let possibleMoves = [
		{ x: x - 1, y: y }, // Left
		{ x: x, y: y - 1 }, // Up
		{ x: x + 1, y: y }, // Right
		{ x: x, y: y + 1 }  // Down
	];
	
	return possibleMoves.filter( function( move ) {
		return IsValidMove( move.x, move.y, rndValue, visited );
	});
}

// Initialize puzzle using Depth-First Search Algorithm
function InitializePuzzle( controlPanel )
{
	// Get size of puzzle from morex property
	var rndValue = controlPanel.morex;

	// Create arrays to contain generated path of puzzle, and which nodes have been included already
	var visited = {};
	var stack = [];
	var currentNode = { x: 0, y: 0 };

	stack.push( currentNode );
	visited[currentNode.x + "," + currentNode.y] = true;

	// Generate a path through the puzzle
	while( stack.length > 0 )
	{
		var neighbors = GetNeighbors( currentNode.x, currentNode.y, rndValue, visited );
		if( neighbors.length === 0 )
		{
			// No valid neighbors, backtrack
			stack.pop();
            if( stack.length > 0 )
            {
                currentNode = stack[stack.length - 1];  // Reset currentNode after backtracking
            }
			continue;
		}

		// Choose a random neighbor
		var nextNode = neighbors[RandomNumber( 0, neighbors.length - 1 )];
		stack.push( nextNode );
		currentNode = nextNode;
		visited[nextNode.x + "," + nextNode.y] = true;

		if( nextNode.x === rndValue - 1 && nextNode.y === rndValue - 1 )
		{
			// Reached the target, break out of the loop
			break;
		}
	}

	// DEBUG - Print the path as directions in console
	/*for( var i = 0; i < stack.length - 1; i++ )
	{
		var currentNode = stack[i];
		var next = stack[i + 1];
		
		if( next.x > currentNode.x )
		{
			Console.Print( "RIGHT!\n" );
		}
		else if( next.x < currentNode.x )
		{
			Console.Print( "LEFT!\n" );
		}
		else if( next.y > currentNode.y )
		{
			Console.Print( "DOWN!\n" );
		}
		else
		{
			Console.Print( "UP!\n");
		}
	}*/

	// Store generated path as a tag on controlPanel
	controlPanel.m_Path = stack;
	controlPanel.SetTag( "pathString", stack.map( function( node ) {
		return node.x + "," + node.y;
	}).join( "|" ));
}

function onGumpPress( pSocket, buttonID, gumpData )
{
	if( pSocket == null )
		return;

	var pUser = pSocket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	var controlPanelSerial = parseInt( pUser.GetTempTag( "powerGenControlPanelSer" ));
	var controlPanel = CalcItemFromSer( controlPanelSerial );

	if( DistanceBetween( pUser, controlPanel, true ) > 3 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 393, pSocket.language )); // That is too far away.
		return;
	}

	var pathString = controlPanel.GetTag( "pathString" );
	if( typeof pathString !== "string" || !pathString )
	{
	    Console.Print( "Error: Invalid pathString value on controlPanel, in onGumpPress: " + controlPanel.serial );
	    return;
	}
	var pathArray = pathString.split( "|" ).map( function( str ) {
		var coords = str.split( "," );
		return { x: parseInt( coords[0] ), y: parseInt( coords[1] ) };
	});

	var progressTag = "progress_" + controlPanelSerial;
	var currentStep = parseInt( pUser.GetTempTag( progressTag ) || "0" );

	if( currentStep >= pathArray.length - 1 )
	{
		// Player has already reached the end, they shouldn't be pressing any more buttons.
		return;
	}

	var currentPosition = pathArray[currentStep];
	var nextPosition = pathArray[currentStep + 1];
	var chosenDirection = null;

	switch( buttonID )
	{
		case 1: // Left
			chosenDirection = { x: currentPosition.x - 1, y: currentPosition.y };
			break;
		case 2: // Up
			chosenDirection = { x: currentPosition.x, y: currentPosition.y - 1 };
			break;
		case 3: // Right
			chosenDirection = { x: currentPosition.x + 1, y: currentPosition.y };
			break;
		case 4: // Down
			chosenDirection = { x: currentPosition.x, y: currentPosition.y + 1 };
			break;
		case 5: // Decipher next step in Circuit Path
			if( pUser.skills.lockpicking >= powerGenLockpickReq ) // Over 65.0
			{
				pSocket.SoundEffect( 0x241, true);

				if( RandomNumber( 1, 100 ) <= 75 ) // 75% chance to succeed
				{
					pUser.SetTempTag( "hintFor_" + controlPanel.serial, true );
					ControlPanelGump( pSocket, controlPanel, currentStep );
					return;
				}
				else
				{
					CauseElectricShock( pUser );
					return;
				}
			}
			else
			{
				return;
			}
			break;
		default:
			return; // Unrecognized buttonID
	}

	if( chosenDirection.x === nextPosition.x && chosenDirection.y === nextPosition.y )
	{
		// Hide hint for next step
		pUser.SetTempTag( "hintFor_" + controlPanel.serial, null );

		// Player made the correct choice
		pUser.SetTempTag( progressTag, ( currentStep + 1 ).toString() );

		// Add the new position to the visited path
		var visitedPathTag = "visitedPath_" + controlPanelSerial;
		var visitedPath = pUser.GetTempTag( visitedPathTag ) || "";
		visitedPath += ( visitedPath ? "|" : "" ) + chosenDirection.x + "," + chosenDirection.y;
		pUser.SetTempTag( visitedPathTag, visitedPath );

		if( currentStep + 1 == pathArray.length - 1 )
		{
			// End of the line! Great success!
			SolvePuzzle( pUser, controlPanel );
		}
		else
		{
			// Redraw gump
			pSocket.SoundEffect( 0x1f4, true);
			ControlPanelGump( pSocket, controlPanel, currentStep + 1 );
		}
	}
	else
	{
		// Player made the wrong choice
		CauseElectricShock( pUser );
	}
}

function CauseElectricShock( pUser )
{
	var pSocket = pUser.socket;
	pUser.TextMessage( GetDictionaryEntry( 2796, pSocket.language ), false, 0x3b2, 0, pUser.serial ); // * Your body convulses from electric shock *
	pUser.BoltEffect();
	pSocket.SoundEffect( 0x1f4, true);
	pUser.Damage( 60, 6 );
	pUser.StartTimer( 5000, 2, powerGenScriptID );
}

function SolvePuzzle( pUser, controlPanel )
{
	var pSocket = pUser.socket;

	// Play SFX
	pSocket.SoundEffect( 0x211, true );
	pSocket.SoundEffect( 0x1f3, true );

	// Play VFX
	DoStaticEffect( controlPanel.x, controlPanel.y, controlPanel.z, 0x36B0, 4, 4, false );
	DoStaticEffect( controlPanel.x - 1, controlPanel.y - 1, controlPanel.z + 2, 0x36B0, 4, 4, false );
	DoStaticEffect( controlPanel.x - 2, controlPanel.y - 1, controlPanel.z + 2, 0x36B0, 4, 4, false );

	pUser.TextMessage( GetDictionaryEntry( 2795, pSocket.language ), false, 0x3b2, 0, pUser.serial ); // You scrounge some gems from the wreckage.

	// Add as many Arcane Gems to player's pack as the puzzle is wide/tall (don't stack, so add individually)
	for( var i = 0; i < controlPanel.morex; i++ )
	{
		var arcaneGem = CreateDFNItem( pSocket, pUser, "arcane_gem", 1, "ITEM", true );
	}

	// Add as many Diamonds to player's pack as the puzzle is wide/tall (stacks, so can just set amount directly)
	var diamonds = CreateDFNItem( pSocket, pUser, "0x0F26", controlPanel.morex, "ITEM", true );


	// Add a few piles of shadow iron ore on the ground in place of the controlPanel
	// Randomize amount of ore in each pile from 9 to 14
	var shadowIronOre1 = CreateDFNItem( null, null, "0x19B8", RandomNumber( 9, 14 ), "ITEM", false, 0x0966, controlPanel.worldnumber, controlPanel.instanceID );
	if( ValidateObject( shadowIronOre1 ))
	{
		shadowIronOre1.name = "shadow iron ore";
		shadowIronOre1.Teleport( controlPanel.x - 1, controlPanel.y, controlPanel.z + 2 );
	}

	var shadowIronOre2 = CreateDFNItem( null, null, "0x19B8", RandomNumber( 9, 14 ), "ITEM", false, 0x0966, controlPanel.worldnumber, controlPanel.instanceID );
	if( ValidateObject( shadowIronOre2 ))
	{
		shadowIronOre2.name = "shadow iron ore";
		shadowIronOre2.Teleport( controlPanel.x - 2, controlPanel.y - 1, controlPanel.z + 2 );
	}

	// Remove tags on player related to this puzzle
	pUser.SetTempTag( "powerGenControlPanelSer", null );
	pUser.SetTempTag( "hintFor_" + controlPanel.serial, null );
	pUser.SetTempTag( "progress_" + controlPanel.serial, null );
	pUser.SetTempTag( "visitedPath_" + controlPanel.serial, null );

	// Delete control panel and all it's subcomponents
	controlPanel.Delete();
}

function onDelete( controlPanel, objType )
{
	// Fetch references to other pieces and delete them
    const componentTags = ["component1", "component2", "component3", "component4", "component5"];
    
    for( var i = 0; i < componentTags.length; i++ ) 
    {
        var component = CalcItemFromSer( parseInt( controlPanel.GetTag( componentTags[i] )));
        if( ValidateObject( component ))
        {
            component.Delete();
        }
    }

	return true;
}

function _restorecontext_() {}