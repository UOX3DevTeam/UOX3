// These commands let shard admins wipe either all or a selection of objects based on certain
// conditions specified in command parameters

function CommandRegistration()
{
	RegisterCommand( "wipe", 3, true );
	RegisterCommand( "iwipe", 3, true );
}

function command_WIPE( socket, cmdString )
{
	iWipe = false;
	CallWipe( socket, cmdString );
}

function command_IWIPE( socket, cmdString )
{
	// Inverted wipe - wipes area OUTSIDE specified target locations/coordinates
	iWipe = true;
	CallWipe( socket, cmdString );
}

function CallWipe( socket, cmdString )
{
	all	= false;
	if( cmdString )
	{
		var splitString = cmdString.split( " " );
		if( splitString[3] )
		{
			// Wipe all objects between two sets of coordinates,
			// on the same facet as the command user
			facetToWipe = socket.currentChar.worldnumber;
			socket.clickX = parseInt( splitString[0] );
			socket.clickY = parseInt( splitString[1] );
			socket.SetWord( 11, parseInt( splitString[2] ));
			socket.SetWord( 13, parseInt( splitString[3] ));
			DoWipe( socket, null );
		}
		else if( splitString[0] )
		{
			// Wipe all objects, or based on type and facet
			var mChar = socket.currentChar;
			all	= true;
			forceWipe = false;
			var uKey = splitString[0].toUpperCase();
			var saidAll = ( uKey == "ALL" );
			//if( saidAll )
			//{
				if( splitString[1] && splitString[1].toUpperCase() == "FORCE" )
				{
					forceWipe = true;
				}
			//}

			// Check if a facet was provided as parameter
			facetToWipe = -1;
			var facetString = "";
			if( splitString[1] && splitString[1].toUpperCase() != "FORCE" )
			{
				facetString = splitString[1];
				switch( facetString.toUpperCase() )
				{
					case "0":
					case "FELUCCA":
						facetToWipe = 0;
						break;
					case "1":
					case "TRAMMEL":
						facetToWipe = 1;
						break;
					case "2":
					case "ILSHENAR":
						facetToWipe = 2;
						break;
					case "3":
					case "MALAS":
						facetToWipe = 3;
						break;
					case "4":
					case "TOKUNO":
						facetToWipe = 4;
						break;
					case "5":
					case "TERMUR":
						facetToWipe = 5;
						break;
					default:
						all = false;
						// Facet (%s) not found. Valid facets: 0/felucca, 1/trammel, 2/ilshenar, 3/malas, 4/tokuno, 5/termur
						var tempMsg = GetDictionaryEntry( 8111, socket.language );
						socket.SysMessage( tempMsg.replace( /%s/gi, facetString ));
						return;
				}
			}

			if( facetString != "" )
			{
				var tempMsg = GetDictionaryEntry( 8112, socket.language ); // Wiping %s on facet: " + facetString
				socket.SysMessage( tempMsg.replace( /%s/gi, uKey ) + " " + facetString );
			}
			else
			{
				var tempMsg = GetDictionaryEntry( 8113, socket.language ); // Wiping %s on ALL facets...
				socket.SysMessage( tempMsg.replace( /%s/gi, uKey ));
			}

			Console.PrintSectionBegin();
			var iterateString = "";
			var wipePerformed = false;
			var totalDeletionCount = 0;

			Console.Print( mChar.name + " has initiated a wipe of " + uKey + "\n" );

			// Loop through the different wipe "types" and wipe if eligible
			for( var i = 0; i < 5; i++ )
			{
				isItem = false;
				isMulti = false;
				isBoat = false;
				isNPC = false;
				isSpawner = false;
				iterateString = "";

				switch( i )
				{
					case 0: // ITEMS
						if( saidAll || uKey == "ITEMS" )
						{
							isItem = true;
							iterateString = "ITEM";
						}
						break;
					case 1: // MULTIS
						if( saidAll || uKey == "MULTIS" )
						{
							isMulti = true;
							iterateString = "MULTI";
						}
						break;
					case 2: // BOATS
						if( saidAll || uKey == "BOATS" )
						{
							isMulti = true;
							iterateString = "BOAT";
						}
						break;
					case 3: // NPCS
						if( saidAll || uKey == "NPCS" )
						{
							isNPC = true;
							iterateString = "CHARACTER";
						}
						break;
					case 4: // SPAWNERS
						if( saidAll || uKey == "SPAWNERS" )
						{
							isSpawner = true;
							iterateString = "SPAWNER";
						}
						break;
					default:
						break;
				}

				if( iterateString != "" )
				{
					// A wipe has been performed! Let the world know
					wipePerformed = true;
					totalDeletionCount += IterateOver( iterateString );
				}
			}
			Console.Print( uKey + " wipe deleted " + totalDeletionCount.toString() + " objects.\n" );
			socket.SysMessage( totalDeletionCount.toString() + " objects deleted." );

			// If no wipe was performed, it means the first param provided was invalid
			if( !wipePerformed )
			{
				var tempMsg = GetDictionaryEntry( 8114, socket.language ); // Invalid parameter %s. Valid params: all / items / npcs / spawners / (x1 y1 x2 y2), facet
				socket.SysMessage( tempMsg.replace( /%s/gi, uKey ));
			}

			Console.PrintDone();
		}
	}
	else
	{
		// Wipe all objects in area between two targeted locations
		// on the same facet as the command user
		facetToWipe = socket.currentChar.worldnumber;
		socket.clickX = -1;
		socket.clickY = -1;
		socket.CustomTarget( 0, GetDictionaryEntry( 8115, socket.language ), 1 ); // Choose top corner to wipe:
	}
}

// Choose first of two locations defining area to wipe
function onCallback0( socket, ourObj )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		socket.clickX = socket.GetWord( 11 );
		socket.clickY = socket.GetWord( 13 );
		socket.CustomTarget( 1, GetDictionaryEntry( 8116, socket.language ), 1 ); // Choose bottom corner to wipe
	}
}

// Choose second of two locations defining area to wipe
function onCallback1( socket, ourObj )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		DoWipe( socket, ourObj );
	}
	else
	{
		socket.clickX = 0;
		socket.clickY = 0;
	}
}

// Wipe based on locations clicked by user
function DoWipe( socket, ourObj )
{
	var mChar = socket.currentChar;

	// Fetch first set of coordinates from temporary socket variables
	x1 = socket.clickX;
	y1 = socket.clickY;

	// Fetch second set of coordinates from socket buffer
	x2 = socket.GetWord( 11 );
	y2 = socket.GetWord( 13 );

	// Reverse coordinate order if user clicked bottom-right to top-left
	// instead of top-left to bottom-right
	var tmpLoc;
	if( x1 > x2 )
	{
		tmpLoc 	= x1;
		x1 	= x2;
		x2	= tmpLoc;
	}
	if( y1 > y2 )
	{
		tmpLoc 	= y1;
		y1 	= y2;
		y2 	= tmpLoc;
	}

	// Perform the wipe
	Console.PrintSectionBegin();
	Console.Print( mChar.name + " has initiated a wipe.\n" );
	socket.SysMessage( GetDictionaryEntry( 8117, socket.language )); // Wiping..
	var counter 	= IterateOver( "ITEM" );
	var counterStr	= counter.toString();

	var tempMsg = GetDictionaryEntry( 8118, socket.language ); // Wiped %i items
	socket.SysMessage( tempMsg.replace( /%s/gi, counterStr ));
	Console.Print( "Wipe deleted " + counterStr + " items.\n" );
	Console.PrintDone();

	// Reset temporary socket variables and reset global variables
	socket.clickX = -1;
	socket.clickY = -1;
	x1 = 0;
	y1 = 0;
	x2 = 0;
	y2 = 0;
	iWipe = false;
	all	= false;
	isItem = false;
}

// Iterate through all objects of specified type
function onIterate( toCheck )
{
	if( ValidateObject( toCheck ))
	{
		if( all ) // Are we wiping all objects, regardless of coordinates?
		{
			// If a facet was specified, make sure only that facet is wiped
			if( facetToWipe >= 0 )
			{
				if( toCheck.worldnumber != facetToWipe )
					return false;
			}
			if( isItem && toCheck.isItem == true && toCheck.isSpawner == false && ( toCheck.wipable || forceWipe ))
			{
				if( toCheck.container == null && toCheck.type != 202 )
				{
					toCheck.Delete();
					return true;
				}
			}
			else if( !isItem && toCheck.isChar == true )
			{
				if( toCheck.npc && toCheck.aitype != 17 && !toCheck.tamed && ( toCheck.wipable || forceWipe ))
				{
					toCheck.Delete();
					return true;
				}
			}
			else if( isSpawner && toCheck.isSpawner == true && ( toCheck.wipable || forceWipe ))
			{
				toCheck.Delete();
				return true;
			}
			else if( isMulti && toCheck.IsMulti() && ( toCheck.wipable || forceWipe ))
			{
				toCheck.Delete();
				return true;
			}
			else if( isBoat && toCheck.IsBoat() && ( toCheck.wipable || forceWipe ))
			{
				toCheck.Delete();
				return true;
			}
		}
		else // Are we wiping objects only in a defined area?
		{
			if( toCheck.isItem == true && toCheck.container == null )
			{
				// If a facet was specified, make sure only that facet is wiped
				if( facetToWipe >= 0 )
				{
					if( toCheck.worldnumber != facetToWipe )
						return false;
				}

				// Only wipe items that aren't inside a valid multi
				var toCheckMulti = toCheck.multi;
				if( !toCheckMulti )
				{
					var shouldWipe 	= iWipe;
					var tX 		= toCheck.x;
					var tY 		= toCheck.y;
					if( tX >= x1 && tX <= x2 && tY >= y1 && tY <= y2 )
					{
						shouldWipe = !iWipe;
					}
					if( shouldWipe )
					{
						toCheck.Delete();
						return true;
					}
				}
			}
		}
	}
	return false;
}
