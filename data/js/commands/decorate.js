// Decorate command - by Xuri (xuri@uox3.org)
// v1.5
//		1.5 - 18/01/2022
//			Added decayable = false as a condition for decorations to be saved/copied
//
//		1.4 - 08/11/2021
// 			Added version system so updates won't break older template versions
//
//		1.3 - 31/10/2021
//			Fixed a few bugs with incorrect dictionary messages
//			Added new sub commands to allow saving/loading/unloading of event decorations
//				based on event name (.event property) assigned to items
//				saveevent [eventName], loadevent [eventName] and unloadevent [eventName]
//
//		1.2 - 12/06/2021
//			Added new objectType - spawners
//			Saves/Loads new object properties: spawnsection, sectionalist, mininterval, maxinterval
//			Added progress gump display to all save, load, copy and clean sub-commands
//
// 		1.1 - 11/06/2021
// 			Added new objectType - containers
// 			Saves/Loads new object properties: dir, weight, weightMax, maxItems, amount
//
// 		1.0 - 10/06/2021
// 			Initial version
//
// Command that allows saving and loading world templates - essentially blueprints for world decoration
// Templates are by default read from/written in SCRIPTDATADIRECTORY/worldtemplates/
//
// Supported objectTypes: doors, signs, lights, containers, moongates, teleporters, misc (everything else)
//
// OPEN GUMP MENU WIHT OPTIONS
// 'decorate (TODO)
// 		Open gump with options for saving and loading world templates
//
// SAVING WORLD TEMPLATES MANUALLY
// 'decorate save
// 		Save all items to the default templates, based on objectType and facet
//
// 'decorate save <objectType>
// 		Save all items of objectType, regardless of facet
//
// 'decorate save <facetName>
//		Save all items on specified facet, regardless of objectType
//
// 'decorate save <customFileName>
// 		Save all items of all objectTypes on all facets to custom template file
//
// 'decorate save <objectType> <facetName>
// 		Save all items of objectType in the specified facet
//
// 'decorate save <customObjectType> <facetName> x1 y1 x2 y2
// 		Save all items within specified coordinates on specified facet to file with same name as customObjectType
//
// 'decorate saveevent <eventName>
//		Save all items with .event property that match eventName to file named event_<eventName>
//
// LOADING WORLD TEMPLATES
// 'decorate load
// 		Load all items from the default templates, regardless of objectType and facet
//
// 'decorate load <objectType>
// 		Load all items of objectType, regardless of facet
//
// 'decorate load <facetName>
//		Load all items on specified facet, regardless of objectType
//
// 'decorate load <customFileName>
//		Load all items from file named <customFileName>
//
// 'decorate load <objectType> <facetName>
// 		Load all items of objectType in the specified facet
//
// 'decorate loadevent <eventName>
//		Load all items from file named event_<eventName>, and assign eventName to their .event property
//
// CLEANING UP DECORATIONS
// 'decorate clean
// 		Clean up duplicate decorations that might have been loaded/added by accident. Counts as duplicate if
// 		item ID, location, type and hue are identical. BEWARE: Might affect player items on a live shard!
//
// 'decorate unloadevent <eventName>
//		Remove all items with .event property that matches eventName

// Save/load arrays
var decorateArray = [];
var decorateDoorsArray = [];
var decorateSignsArray = [];
var decorateLightsArray = [];
var decorateContainersArray = [];
var decorateMoongatesArray = [];
var decorateTeleportersArray = [];
var decorateSpawnersArray = [];
var decorateMiscArray = [];

// Facets, each will have their own set of files with relevant object types
const facetList = [ "felucca", "trammel", "ilshenar", "malas", "tokuno", "termur" ];

// Object types - each will be saved to its own file, per facet
const objectTypeList = [ "doors", "signs", "lights", "containers", "moongates", "teleporters", "spawners", "misc" ];

// List of sign IDs
const signIDs = [ 0x0b97,0x0b96,0x0b97,0x0b98,0x0b99,0x0b9a,0x0b9b,0x0b9c,0x0b9d,0x0b9e,0x0b9f,0x0ba0,0x0ba1,0x0ba2,0x0ba3,0x0ba4,0x0ba5,0x0ba6,0x0ba7,0x0ba8,0x0ba9,0x0baa,0x0bab,0x0bac,0x0bad,0x0bae,0x0baf,0x0bb0,0x0bb1,0x0bb2,0x0bb3,0x0bb4,0x0bb5,0x0bb6,0x0bb7,0x0bb8,0x0bb9,0x0bba,0x0bbb,0x0bbc,0x0bbd,0x0bbe,0x0bbf,0x0bc0,0x0bc1,0x0bc2,0x0bc3,0x0bc4,0x0bc5,0x0bc6,0x0bc7,0x0bc8,0x0bc9,0x0bca,0x0bcb,0x0bcc,0x0bcd,0x0bce,0x0bcf,0x0bd0,0x0bd1,0x0bd2,0x0bd3,0x0bd4,0x0bd5,0x0bd6,0x0bd7,0x0bd8,0x0bd9,0x0bda,0x0bdb,0x0bdc,0x0bdd,0x0bde,0x0bdf,0x0be0,0x0be1,0x0be2,0x0be3,0x0be4,0x0be5,0x0be6,0x0be7,0x0be8,0x0be9,0x0bea,0x0beb,0x0bec,0x0bed,0x0bee,0x0bef,0x0bf0,0x0bf1,0x0bf2,0x0bf3,0x0bf4,0x0bf5,0x0bf6,0x0bf7,0x0bf8,0x0bf9,0x0bfa,0x0bfb,0x0bfc,0x0bfd,0x0bfe,0x0bff,0x0c00,0x0c01,0x0c02,0x0c03,0x0c04,0x0c05,0x0c06,0x0c07,0x0c08,0x0c09,0x0c0a,0x0c0b,0x0c0c,0x0c0d,0x0c0e,0x1297,0x1298,0x1299,0x129a,0x129b,0x129c,0x129d,0x129e,0x163d,0x163e,0x163f,0x1641,0x1642,0x1643,0x1f28,0x1f29,0x4b20,0x4b21,0x9a0c,0x9a0d,0x9a0e,0x9a0f,0x9a10,0x9a11,0x9a12,0x9a13,0xa130,0xa131,0xa132,0xa133,0xa134,0xa135,0xa136,0xa137 ];

// List of moongate IDs
const moongateIDs = [ 0x0dda,0x0ddb,0x0ddc,0x0ddd,0x0dde,0x0f6c,0x0f6d,0x0f6e,0x0f6f,0x0f70,0x1ae5,0x1ae6,0x1ae7,0x1ae8,0x1ae9,0x1aea,0x1aeb,0x1aec,0x1aed,0x1af3,0x1af4,0x1af5,0x1af6,0x1af7,0x1af8,0x1af9,0x1afa,0x1afb,0x1fcb,0x1fcc,0x1fcd,0x1fce,0x1fcf,0x1fd0,0x1fd1,0x1fd2,0x1fd3,0x1fd4,0x1fd5,0x1fd6,0x1fd7,0x1fd8,0x1fde,0x1fdf,0x1fe0,0x1fe1,0x1fe2,0x1fe3,0x1fe4,0x1fe5,0x1fe6,0x1fe7,0x1fe8,0x1fe9,0x1fea,0x1feb,0x4b8f,0x4b90,0x4b91,0x4b92,0x4b93,0x4b94,0x4b95,0x4b96,0x4b97,0x4b98,0x4b99,0x4b9a,0x4b9b,0x4b9c,0x4bcb,0x4bcc,0x4bcd,0x4bce,0x4bcf,0x4bd0,0x4bd1,0x4bd2,0x4bd3,0x4bd4,0x4bd5,0x4bd6,0x4bd7,0x4bd8 ]

// ScriptID of this script, used to identify and close some gumps
const scriptID = 1059;
const templateSystemVer = 2;

function CommandRegistration()
{
	RegisterCommand( "decorate", 5, true ); // Admin only command
}

function command_DECORATE( socket, cmdString )
{
	// Reset arrays used for saving/loading decorations
	decorateArray.length = 0;
	decorateDoorsArray.length = 0;
	decorateSignsArray.length = 0;
	decorateLightsArray.length = 0;
	decorateContainersArray.length = 0;
	decorateMoongatesArray.length = 0;
	decorateTeleportersArray.length = 0;
	decorateSpawnersArray.length = 0;
	decorateMiscArray.length = 0;

	// Reset some global variables used to track stuff across functions
	cleanMode = false;
	copyMode = false;
	saveAll = false;
	saveCustom = false;
	loadCustom = false;
	saveEvent = false;
	loadEvent = false;
	unloadEvent = false;
	facetID = -1;
	facetName = "";
	objectType = "";
	eventName = "";

	if( cmdString )
	{
		// Handle subcommand
		var splitString = cmdString.split( " " );
		switch( splitString[0].toUpperCase() )
		{
			case "SAVE":
				// Save decorations to world template files
				HandleDecorateSave( socket, splitString );
				break;
			case "LOAD":
				// Load decorations from world template files
				HandleDecorateLoad( socket, splitString );
				break;
			case "SAVEEVENT":
				// Save event decorations to event world template file
				if( typeof(splitString[1]) != "undefined" )
				{
					// Keep event info for later
					eventName = splitString[1].toLowerCase();
					HandleDecorateSave( socket, splitString );
				}
				break;
			case "LOADEVENT":
				// Load event decorations from event world template file
				if( typeof(splitString[1]) != "undefined" )
				{
					// Keep event info for later
					eventName = splitString[1].toLowerCase();
					HandleDecorateLoad( socket, splitString );
				}
				break;
			case "UNLOADEVENT":
				if( typeof(splitString[1]) != "undefined" )
				{
					// Keep event info for later
					eventName = splitString[1].toLowerCase();
					HandleDecorateUnloadEvent( socket );
				}
				break;
			case "CLEAN":
				// Clean up duplicate decorations
				HandleDecorateClean( socket );
				break;
			case "COPY":
				// Copy decorations from one facet to another
				HandleDecorateCopy( socket, splitString );
				break;
			default:
			{
				var tempMsg = GetDictionaryEntry( 8012, socket.language ); // Unknown subcommand (%s) provided for DECORATE command.
				socket.SysMessage( tempMsg.replace( /%s/gi, splitString[0] ));
				break;
			}
		}
	}
	/*else
	{
		// TODO - decorate menu
		// Open decorate gump
		socket.SysMessage( "Opening Decorate menu..." );
	}*/
}

// Get Decoration Type from command string
function GetDecorationType( socket, splitString )
{
	objectType = "";
	if( splitString != "" && typeof( splitString ) != "undefined" )
	{
		switch( splitString.toUpperCase() )
		{
			case "DOORS":
				objectType = "doors";
				decorateArray = decorateDoorsArray;
				break;
			case "SIGNS": // All signs, including house signs and road signs
				objectType = "signs";
				decorateArray = decorateSignsArray;
				break;
			case "LIGHTS": // All items with light-flag set in tiledata
				objectType = "lights";
				decorateArray = decorateLightsArray;
				break;
			case "CONTAINERS": // All items with container type, or container tiledata flag
				objectType = "containers";
				decorateArray = decorateContainersArray;
				break;
			case "MOONGATES": // All moongates
				objectType = "moongates";
				decorateArray = decorateMoongatesArray;
				break;
			case "TELEPORTERS": // All teleporter objects?
				objectType = "teleporters";
				decorateArray = decorateTeleportersArray;
				break;
			case "SPAWNERS": // All spawner objects
				objectType = "spawners";
				decorateArray = decorateSpawnersArray;
				break;
			case "MISC": // All other objects that don't fit into the other categories
				objectType = "misc";
				decorateArray = decorateMiscArray;
				break;
			default:
				if( facetList.indexOf( splitString ) == -1 )
				{
					objectType = splitString.toLowerCase();
				}
				break;
		}
	}
}

// Get Facet from command string
function GetFacet( socket, splitString )
{
	facetName = "";
	facetID = -1;
	if( splitString != "" && typeof( splitString ) != "undefined" )
	{
		switch( splitString.toUpperCase() )
		{
			case "0":
			case "FELUCCA":
				facetName = "felucca";
				facetID = 0;
				break;
			case "1":
			case "TRAMMEL":
				facetName = "trammel";
				facetID = 1;
				break;
			case "2":
			case "ILSHENAR":
				facetName = "ilshenar";
				facetID = 2;
				break;
			case "3":
			case "MALAS":
				facetName = "malas";
				facetID = 3;
				break;
			case "4":
			case "TOKUNO":
				facetName = "tokuno";
				facetID = 4;
				break;
			case "5":
			case "TERMUR":
			case "TER-MUR":
				facetName = "termur";
				facetID = 5;
				break;
			default:
				socket.SysMessage( GetDictionaryEntry( 8013, socket.language )); // No valid facet type specified. Facets supported: felucca, trammel, ilshenar, malas, tokuno or termur
				break;
		}
	}
}

// Handle copying of decorations from one facet to another
function HandleDecorateCopy( socket, splitString )
{
	copyMode = true;
	totalCopied = 0;
	sourceFacet = "";
	targetFacet = "";
	facetID = -1;
	targetFacetID = -1;

	if( splitString[1] )
	{
		sourceFacet = splitString[1].toUpperCase();
	}
	if( splitString[2] )
	{
		targetFacet = splitString[2].toUpperCase();
	}
	if( sourceFacet != "" && targetFacet != "" )
	{
		var tempMsg = GetDictionaryEntry( 8014, socket.language ); // Copying ALL decorations from facet '%s' to facet '%t'...
		tempMsg = tempMsg.replace( /%s/gi, sourceFacet.toLowerCase() );
		socket.SysMessage( tempMsg.replace( /%t/gi, targetFacet.toLowerCase() ));

		switch( sourceFacet )
		{
			case "FELUCCA":
				facetID = 0;
				break;
			case "TRAMMEL":
				facetID = 1;
				break;
			case "ILSHENAR":
				facetID = 2;
				break;
			case "MALAS":
				facetID = 3;
				break;
			case "TOKUNO":
				facetID = 4;
				break;
			case "TERMUR":
				facetID = 5;
				break;
			default:
				break;
		}

		switch( targetFacet )
		{
			case "FELUCCA":
				targetFacetID = 0;
				break;
			case "TRAMMEL":
				targetFacetID = 1;
				break;
			case "ILSHENAR":
				targetFacetID = 2;
				break;
			case "MALAS":
				targetFacetID = 3;
				break;
			case "TOKUNO":
				targetFacetID = 4;
				break;
			case "TERMUR":
				targetFacetID = 5;
				break;
			default:
				break;
		}

		if( facetID != -1 && targetFacetID != -1 )
		{
			DisplayProgressGump( socket, GetDictionaryEntry( 8015, socket.language ), 0 ); // Copying Decorations

			totalCopied = IterateOver( "ITEM" );
			if( totalCopied > 0 )
			{
				var tempMsg = GetDictionaryEntry( 8016, socket.language ); // %i decorations copied from %s to %t.
				tempMsg = tempMsg.replace( /%i/gi, totalCopied.toString() );
				tempMsg = tempMsg.replace( /%s/gi, sourceFacet );
				socket.SysMessage( tempMsg.replace( /%t/gi, targetFacet ));
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 8017, socket.language )); // No decorations were copied.
			}

			socket.CloseGump( scriptID + 0xffff, 0 );
		}
	}
	else
	{
		// Invalid facets provided as arguments (fromFacet, toFacet) to copy command. Valid arguments: felucca, trammel, ilshenar, malas, tokuno, termur
		socket.SysMessage( GetDictionaryEntry( 8018, socket.language ));
	}
}

// Handle cleaning up of duplicate decorations, if any
function HandleDecorateClean( socket )
{
	socket.SysMessage( GetDictionaryEntry( 8019, socket.language )); // Cleaning decorations...
	cleanMode = true;
	totalCleaned = 0;

	DisplayProgressGump( socket, GetDictionaryEntry( 8020, socket.language ), 0 ); // Cleaning Duplicates

	var progressOne = true;
	var progressTwo = true;
	var progressThree = true;
	var progressFour = true;
	var progressFive = true;

	var facetCount = facetList.length;
	for( var i = 0; i < facetCount; i++ )
	{
		facetID = i;
		IterateOver( "ITEM" );

		if( progressFive && i >= 5 )
		{
			DisplayProgressGump( socket, GetDictionaryEntry( 8020, socket.language ), 75 ); // Cleaning Duplicates
			progressFive = false;
		}
		else if( progressFour && i >= 4 )
		{
			DisplayProgressGump( socket, GetDictionaryEntry( 8020, socket.language ), 60 ); // Cleaning Duplicates
			progressFour = false;
		}
		else if( progressThree && i >= 3 )
		{
			DisplayProgressGump( socket, GetDictionaryEntry( 8020, socket.language ), 45 ); // Cleaning Duplicates
			progressThree = false;
		}
		else if( progressTwo && i >= 2 )
		{
			DisplayProgressGump( socket, GetDictionaryEntry( 8020, socket.language ), 30 ); // Cleaning Duplicates
			progressTwo = false;
		}
		else if( progressOne && i >= 1 )
		{
			DisplayProgressGump( socket, GetDictionaryEntry( 8020, socket.language ), 15 ); // Cleaning Duplicates
			progressOne = false;
		}

	}

	socket.CloseGump( scriptID + 0xffff, 0 );

	if( totalCleaned > 0 )
	{
		var tempMsg = GetDictionaryEntry( 8021, socket.language ); // %i duplicate items removed.
		socket.SysMessage( tempMsg.replace( /%i/gi, totalCleaned.toString() ));
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 8022, socket.language )); // No duplicate items found amongst decorations.
	}
}

// Handle unloading of event decorations, if any
function HandleDecorateUnloadEvent( socket )
{
	socket.SysMessage( GetDictionaryEntry( 9093, socket.language )); // Unloading Event...
	cleanMode = true;
	unloadEvent = true;
	totalCleaned = 0;

	DisplayProgressGump( socket, GetDictionaryEntry( 9094, socket.language ), 0 ); // Unloading Event...

	var progressOne = true;
	var progressTwo = true;
	var progressThree = true;
	var progressFour = true;
	var progressFive = true;

	var facetCount = facetList.length;
	for( var i = 0; i < facetCount; i++ )
	{
		facetID = i;
		IterateOver( "ITEM" );

		if( progressFive && i >= 5 )
		{
			DisplayProgressGump( socket, GetDictionaryEntry( 9094, socket.language ), 75 ); // Unloading Event...
			progressFive = false;
		}
		else if( progressFour && i >= 4 )
		{
			DisplayProgressGump( socket, GetDictionaryEntry( 9094, socket.language ), 60 ); // Unloading Event...
			progressFour = false;
		}
		else if( progressThree && i >= 3 )
		{
			DisplayProgressGump( socket, GetDictionaryEntry( 9094, socket.language ), 45 ); // Unloading Event...
			progressThree = false;
		}
		else if( progressTwo && i >= 2 )
		{
			DisplayProgressGump( socket, GetDictionaryEntry( 9094, socket.language ), 30 ); // Unloading Event...
			progressTwo = false;
		}
		else if( progressOne && i >= 1 )
		{
			DisplayProgressGump( socket, GetDictionaryEntry( 9094, socket.language ), 15 ); // Unloading Event...
			progressOne = false;
		}

	}

	socket.CloseGump( scriptID + 0xffff, 0 );

	if( totalCleaned > 0 )
	{
		var tempMsg = GetDictionaryEntry( 9095, socket.language ); // %i event decorations removed for event %s.
		tempMsg = tempMsg.replace( /%s/gi, eventName.toLowerCase() );
		socket.SysMessage( tempMsg.replace( /%i/gi, totalCleaned.toString() ));
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 9096, socket.language )); // No event decorations found.
	}
}

// Handle saving of decorations, based on parameters in command string
function HandleDecorateSave( socket, splitString )
{
	socket.SysMessage( GetDictionaryEntry( 8023, socket.language )); // Saving decorations...
	saveAll = false;
	saveCustom = false;
	saveEvent = false;

	var facetCount = facetList.length;
	var objectTypeCount = objectTypeList.length;
	var numArguments = splitString.length;

	if( eventName != "" )
	{
		saveEvent = true;
	}

	switch( numArguments )
	{
		case 1: // No additional arguments provided, save everything
		{
			saveAll = true;

			// Loop through each facet and save out files
			for( var i = 0; i < facetCount; i++ )
			{
				// Reset save arrays for each facet
				decorateDoorsArray.length = 0;
				decorateSignsArray.length = 0;
				decorateLightsArray.length = 0;
				decorateContainersArray.length = 0;
				decorateMoongatesArray.length = 0;
				decorateTeleportersArray.length = 0;
				decorateSpawnersArray.length = 0;
				decorateMiscArray.length = 0;

				switch( i )
				{
					case 0:
						socket.SysMessage( GetDictionaryEntry( 8024, socket.language )); // Saving decorations for Felucca...
						facetID = 0;
						facetName = "felucca";
						DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 0 ); // Saving Decorations
						break;
					case 1:
						socket.SysMessage( GetDictionaryEntry( 8025, socket.language )); // Saving decorations for Trammel...
						facetID = 1;
						facetName = "trammel";
						DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 20 ); // Saving Decorations
						break;
					case 2:
						socket.SysMessage( GetDictionaryEntry( 8026, socket.language )); // Saving decorations for Ilshenar...
						facetID = 2;
						facetName = "ilshenar";
						DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 35 ); // Saving Decorations
						break;
					case 3:
						socket.SysMessage( GetDictionaryEntry( 8027, socket.language )); // Saving decorations for Malas...
						facetID = 3;
						facetName = "malas";
						DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 50 ); // Saving Decorations
						break;
					case 4:
						socket.SysMessage( GetDictionaryEntry( 8028, socket.language )); // Saving decorations for Tokuno...
						facetID = 4;
						facetName = "tokuno";
						DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 65 ); // Saving Decorations
						break;
					case 5:
						socket.SysMessage( GetDictionaryEntry( 8029, socket.language )); // Saving decorations for Termur...
						facetID = 5;
						facetName = "termur";
						DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 80 ); // Saving Decorations
						break;
					default:
						socket.SysMessage( GetDictionaryEntry( 8031, socket.language )); // Invalid facet selected, saving aborted!
						return;
				}

				// Save items into relevant array
				var iterateCount = IterateOver( "ITEM" );
				if( iterateCount == 0 )
					continue; // No decorations found, of any type. Check next facet!

				var tempMsg = GetDictionaryEntry( 9091, socket.language ); // ...%i decorations saved!
				socket.SysMessage( tempMsg.replace( /%i/gi, iterateCount.toString() ));

				// With facetID/Name sorted, let's loop through each objectType
				for( var j = 0; j < objectTypeCount; j++ )
				{
					switch( j )
					{
						case 0:
							objectType = "doors";
							SaveDecorationsToFile( socket, decorateDoorsArray, false );
							break;
						case 1:
							objectType = "signs";
							SaveDecorationsToFile( socket, decorateSignsArray, false  );
							break;
						case 2:
							objectType = "lights";
							SaveDecorationsToFile( socket, decorateLightsArray, false  );
							break;
						case 3:
							objectType = "containers";
							SaveDecorationsToFile( socket, decorateContainersArray, false  );
							break;
						case 4:
							objectType = "moongates";
							SaveDecorationsToFile( socket, decorateMoongatesArray, false  );
							break;
						case 5:
							objectType = "teleporters";
							SaveDecorationsToFile( socket, decorateTeleportersArray, false  );
							break;
						case 6:
							objectType = "spawners";
							SaveDecorationsToFile( socket, decorateSpawnersArray, false  );
							break;
						case 7:
							objectType = "misc";
							SaveDecorationsToFile( socket, decorateMiscArray, false  );
							break;
						default:
							socket.SysMessage( GetDictionaryEntry( 8032, socket.language )); // Invalid objectType selected, saving aborted!
							return;
					}
				}
			}
			socket.CloseGump( scriptID + 0xffff, 0 );
			return;
		}
		case 2: // User trying to save custom file, or specified either objectType or facetName (but not both)
		{
			// First, attempt to get objectType
			GetDecorationType( socket, splitString[1] );

			if( objectType == "" && typeof(splitString[1]) != "undefined" )
			{
				// We didn't get a valid decorationType - neither one of the default ones, nor a custom one. Did user supply a facet first, instead?
				GetFacet( socket, splitString[1] );
			}

			if( facetID == -1 )
			{
				// We didn't get facetID from splitString[1], so let's try it from splitString[2] instead
				GetFacet( socket, splitString[2] );
			}

			// If no objectType was specified, AND no facetID, then assume user wants to save everything to a custom file
			// if( objectType != "" && objectType != "doors" && objectType != "signs" && objectType != "lights" && objectType != "containers" && objectType != "moongates" && objectType != "teleporters" && objectType != "misc" )
			if( objectType != "" && objectTypeList.indexOf( objectType ) == -1 )
			{
				// Save in custom file
				saveCustom = true;
				saveAll = true;

				DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 0 ); // Saving Decorations

				var iterateCount = IterateOver( "ITEM" );
				if( iterateCount > 0 )
				{
					if( SaveDecorationsToFile( socket, decorateArray, true ))
					{
						var tempMsg2 = GetDictionaryEntry( 8033, socket.language ); // %i decorations saved to custom world template '%s'. Check <SCRIPTSDATADIRECTORY>/worldtemplates/ for output files!
						tempMsg2 = tempMsg2.replace( /%i/gi, iterateCount.toString() );
						socket.SysMessage( tempMsg2.replace( /%s/gi, objectType ));
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 8034, socket.language )); // Unable to save decorations to template. Something may have gone wrong!
					}
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 9092, socket.language )); // No decorations found. None saved!
				}
				socket.CloseGump( scriptID + 0xffff, 0 );
			}
			else
			{
				if( objectType == "" && facetID != -1 )
				{
					saveAll = true;

					DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 0 ); // Saving Decorations

					var iterateCount = IterateOver( "ITEM" );
					if( iterateCount > 0 )
					{
						// Save ALL objects on specified facet
						// Loop through each objectType and save out each file
						for( var j = 0; j < objectTypeCount; j++ )
						{
							switch( j )
							{
								case 0:
									objectType = "doors";
									SaveDecorationsToFile( socket, decorateDoorsArray, false );
									DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 12 ); // Saving Decorations
									break;
								case 1:
									objectType = "signs";
									SaveDecorationsToFile( socket, decorateSignsArray, false );
									DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 24 ); // Saving Decorations
									break;
								case 2:
									objectType = "lights";
									SaveDecorationsToFile( socket, decorateLightsArray, false );
									DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 36 ); // Saving Decorations
									break;
								case 3:
									objectType = "containers";
									SaveDecorationsToFile( socket, decorateContainersArray, false );
									DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 48 ); // Saving Decorations
									break;
								case 4:
									objectType = "moongates";
									SaveDecorationsToFile( socket, decorateMoongatesArray, false );
									DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 60 ); // Saving Decorations
									break;
								case 5:
									objectType = "teleporters";
									SaveDecorationsToFile( socket, decorateTeleportersArray, false );
									DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 72 ); // Saving Decorations
									break;
								case 6:
									objectType = "spawners";
									SaveDecorationsToFile( socket, decorateSpawnersArray, false );
									DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 84 ); // Saving Decorations
									break;
								case 7:
									objectType = "misc";
									SaveDecorationsToFile( socket, decorateMiscArray );
									break;
								default:
									socket.SysMessage( GetDictionaryEntry( 8032, socket.language )); // Invalid objectType selected, saving aborted!
									socket.CloseGump( scriptID + 0xffff, 0 );
									return;
							}
						}
						socket.CloseGump( scriptID + 0xffff, 0 );

						var tempMsg3 = GetDictionaryEntry( 8035, socket.language ); // %i decorations successfully saved to templates for facet '%s'. Check <SCRIPTDATADIRECTORY/worldtemplates/ for saved template files!
						tempMsg3 = tempMsg3.replace( /%i/gi, iterateCount.toString() );
						socket.SysMessage( tempMsg3.replace( /%s/gi, facetName ));
					}
				}
				else if( objectType != "" && facetID == -1 )
				{
					// Save all objects of specified type on ALL facets
					for( var i = 0; i < facetCount; i++ )
					{
						// Reset save arrays for each facet
						decorateDoorsArray.length = 0;
						decorateSignsArray.length = 0;
						decorateLightsArray.length = 0;
						decorateMoongatesArray.length = 0;
						decorateTeleportersArray.length = 0;
						decorateSpawnersArray.length = 0;
						decorateMiscArray.length = 0;

						switch( i )
						{
							case 0:
								facetID = 0;
								facetName = "felucca";
								DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 15 ); // Saving Decorations
								break;
							case 1:
								facetID = 1;
								facetName = "trammel";
								DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 30 ); // Saving Decorations
								break;
							case 2:
								facetID = 2;
								facetName = "ilshenar";
								DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 45 ); // Saving Decorations
								break;
							case 3:
								facetID = 3;
								facetName = "malas";
								DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 60 ); // Saving Decorations
								break;
							case 4:
								facetID = 4;
								facetName = "tokuno";
								DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 75 ); // Saving Decorations
								break;
							case 5:
								facetID = 5;
								facetName = "termur";
								DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 90 ); // Saving Decorations
								break;
							default:
								socket.SysMessage( GetDictionaryEntry( 8031, socket.language )); // Invalid facet selected, saving aborted!
								socket.CloseGump( scriptID + 0xffff, 0 );
								return;
						}

						var iterateCount = IterateOver( "ITEM" );
						if( iterateCount == 0 )
							continue;

						var tempMsg3 = GetDictionaryEntry( 8036, socket.language ); // Saving %i decorations of type '%s' for facet '%t'...
						tempMsg3 = tempMsg3.replace( /%i/gi, iterateCount.toString() );
						tempMsg3 = tempMsg3.replace( /%s/gi, objectType );
						socket.SysMessage( tempMsg3.replace( /%t/gi, facetName ));

						switch( objectType.toUpperCase() )
						{
							case "DOORS":
								SaveDecorationsToFile( socket, decorateDoorsArray, false );
								break;
							case "SIGNS":
								SaveDecorationsToFile( socket, decorateSignsArray, false );
								break;
							case "LIGHTS":
								SaveDecorationsToFile( socket, decorateLightsArray, false );
								break;
							case "CONTAINERS":
								SaveDecorationsToFile( socket, decorateContainersArray, false );
								break;
							case "MOONGATES":
								SaveDecorationsToFile( socket, decorateMoongatesArray, false );
								break;
							case "TELEPORTERS":
								SaveDecorationsToFile( socket, decorateTeleportersArray, false );
								break;
							case "SPAWNERS":
								SaveDecorationsToFile( socket, decorateSpawnersArray, false );
								break;
							case "MISC":
								SaveDecorationsToFile( socket, decorateMiscArray, false );
								break;
							default:
								socket.SysMessage( GetDictionaryEntry( 8032, socket.language )); // Invalid objectType selected, saving aborted!
								socket.CloseGump( scriptID + 0xffff, 0 );
								return;
						}
					}
					socket.CloseGump( scriptID + 0xffff, 0 );
				}
			}
			break;
		}
		case 3: // User specified both objectType and facetName
		{
			GetDecorationType( socket, splitString[1] );
			GetFacet( socket, splitString[2] );
			if( facetID != -1 && objectType != "" )
			{
				DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 0 ); // Saving Decorations

				var iterateCount = IterateOver( "ITEM" );
				if( iterateCount > 0 )
				{
					var tempMsg4 = GetDictionaryEntry( 8037, socket.language ); // %i items of type '%s' detected in facet '%t'. Saving...
					tempMsg4 = tempMsg4.replace( /%i/gi, iterateCount.toString() );
					tempMsg4 = tempMsg4.replace( /%s/gi, objectType );
					socket.SysMessage( tempMsg4.replace( /%t/gi, facetName ));

					if( SaveDecorationsToFile( socket, decorateArray, true ))
					{
						socket.SysMessage( GetDictionaryEntry( 8038, socket.language )); // World template save completed. Check <SCRIPTDATADIRECTORY>/worldtemplates/ for saved template files!
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 8039, socket.language )); // Unable to save decorations to template. Something may have gone wrong!
					}
				}
				else
				{
					var tempMsg5 = GetDictionaryEntry( 8040, socket.language ); // No decorations of type '%s' was found in facet '%t'.
					tempMsg5 = tempMsg5.replace( /%s/gi, objectType );
					socket.SysMessage( tempMsg5.replace( /%t/gi, facetName ));
				}

				socket.CloseGump( scriptID + 0xffff, 0 );
			}
			break;
		}
		case 7: // User provided coordinates to save an area in a custom decorations file
		{
			if( splitString[6] )
			{
				saveCustom = true;

				// User provided coordinates to save a custom decorations file
				objectType = splitString[1]; // functions as custom filename
				GetFacet( socket, splitString[2] );
				x1 = parseInt( splitString[3] );
				y1 = parseInt( splitString[4] );
				x2 = parseInt( splitString[5] );
				y2 = parseInt( splitString[6] );

				if( isNaN( x1 ) || isNaN( y1 ) || isNaN( x2 ) || isNaN( y2 ) || objectType == "" || facetID == -1 )
				{
					socket.SysMessage( GetDictionaryEntry( 8041, socket.language )); // Invalid arguments provided - with 7 arguments the syntax is: 'decorate save <filename> <facetName> x1 y1 x2 y2
					return;
				}

				DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 0 ); // Saving Decorations

				// If both facet and objectType is defined...
				var iterateCount = IterateOver( "ITEM" );
				if( iterateCount > 0 )
				{
					var tempMsg6 = GetDictionaryEntry( 8042, socket.language ); // %i items detected in facet '%s'. Saving to custom world template titled '%t'...
					tempMsg6 = tempMsg6.replace( /%i/gi, iterateCount.toString() );
					tempMsg6 = tempMsg6.replace( /%s/gi, objectType );
					socket.SysMessage( tempMsg6.replace( /%t/gi, facetName ));
					if( SaveDecorationsToFile( socket, decorateArray, true ))
					{
						socket.SysMessage( GetDictionaryEntry( 8043, socket.language )); // World template save completed. Check <SCRIPTDATADIRECTORY>/worldtemplates/ for saved template files!
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 8034, socket.language )); // Unable to save decorations to template. Something may have gone wrong!
					}
				}

				socket.CloseGump( scriptID + 0xffff, 0 );
			}
			break;
		}
		default:
			break;
	}
}

function SaveDecorationToArray( toCheck, arrayRef )
{
	// Item properties to save
	var itemType = ( toCheck.type ).toString();
	var itemID = ( toCheck.id ).toString();
	var itemName = toCheck.name;
	var itemHue = ( toCheck.colour ).toString();
	var itemX = ( toCheck.x ).toString();
	var itemY = ( toCheck.y ).toString();
	var itemZ = ( toCheck.z ).toString();
	var itemWorld = ( toCheck.worldnumber ).toString();
	var itemInstanceID = ( toCheck.instanceID ).toString();
	var itemMovable = ( toCheck.movable ).toString();
	var itemVisible = ( toCheck.visible ).toString();
	var itemDirection = ( toCheck.dir ).toString();
	var itemWeight = ( toCheck.weight ).toString();
	var itemWeightMax = ( toCheck.weightMax ).toString();
	var itemMaxItems = ( toCheck.maxItems ).toString();
	var itemAmount = ( toCheck.amount ).toString();
	var more = ( toCheck.more ).toString();
	var moreX = ( toCheck.morex ).toString();
	var moreY = ( toCheck.morey ).toString();
	var moreZ = ( toCheck.morez ).toString();
	var spawnSection = 0;
	var sectionAList = 0;
	var minInterval = 0;
	var maxInterval = 0;
	if( toCheck.isSpawner )
	{
		spawnSection = toCheck.spawnsection;
		sectionAList = ( toCheck.sectionalist ).toString();
		minInterval = ( toCheck.mininterval ).toString();
		maxInterval = ( toCheck.maxinterval ).toString();
	}

	// Form new string with object properties separated by a |
	var newEntry = itemID + "|" + itemName + "|" + itemHue + "|" + itemType + "|" + itemX + "|"
		+ itemY + "|" + itemZ + "|" + itemWorld + "|" + itemInstanceID + "|" + itemMovable + "|"
		+ itemVisible + "|" + itemDirection + "|" + itemWeight + "|" + itemWeightMax + "|" + itemMaxItems + "|"
		+ itemAmount + "|" + spawnSection + "|" + sectionAList + "|" + minInterval + "|"
		+ maxInterval + "|" + more + "|" + moreX + "|" + moreY + "|" + moreZ;

	// Append any ScriptTriggers attached to object to string
	var scriptTriggers = toCheck.scriptTriggers;
	var size = scriptTriggers.length;
	for( var i = 0; i < size; i++ )
	{
		newEntry += ( "|" + scriptTriggers[i] );
	}

	// Push string with object properties to array, so we can output that to file later
	arrayRef.push( newEntry );
}

function SaveDecorationsToFile( socket, arrayRef, singleSave )
{
	if( !arrayRef )
	{
		socket.SysMessage( GetDictionaryEntry( 8044, socket.language )); // No valid array reference was provided. Aborting save of decorations.
		return;
	}

	var arrayLength = arrayRef.length;
	if( arrayLength < 1 )
	{
		return;
	}

	var displayProgressTen = false;
	var displayProgressTwenty = false;
	var displayProgressForty = false;
	var displayProgressSixty = false;
	var displayProgressEighty = false;

	if( singleSave )
	{
		displayProgressTen = true;
		displayProgressTwenty = true;
		displayProgressForty = true;
		displayProgressSixty = true;
		displayProgressEighty = true;
	}

	// Create a new file object
	var mFile = new UOXCFile();
	var fileName = "";
	if( saveEvent )
	{
		fileName = "event_" + eventName + ".jsdata";
	}
	else if( saveCustom )
	{
		fileName = objectType + ".jsdata";
	}
	else
	{
		fileName = facetName + "_" + objectType +".jsdata";
	}

	// Open world file template contained in worldfileTemplates subfolder of js/jsdata/ (true flag)
	mFile.Open( fileName, "w", "worldtemplates", true );
	if( mFile != null )
	{
		// Append each line in arrayRef to a new line in the file
		var newLine = "";
		mFile.Write( templateSystemVer + "\n" );

		for( var i = 0; i < arrayLength; i++ )
		{
			mFile.Write( arrayRef[i] + "\n" );

			if( displayProgressEighty && (( i * 100 ) / arrayLength ) >= 80 )
			{
				DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 80 ); // Saving Decorations
				displayProgressEighty = false;
			}
			else if( displayProgressSixty && (( i * 100 ) / arrayLength ) >= 60 )
			{
				DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 60 ); // Saving Decorations
				displayProgressSixty = false;
			}
			else if( displayProgressForty && (( i * 100 ) / arrayLength ) >= 40 )
			{
				DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 40 ); // Saving Decorations
				displayProgressForty = false;
			}
			else if( displayProgressTwenty && (( i * 100 ) / arrayLength ) >= 20 )
			{
				DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 20 ); // Saving Decorations
				displayProgressTwenty = false;
			}
			else if( displayProgressTen )
			{
				DisplayProgressGump( socket, GetDictionaryEntry( 8030, socket.language ), 10 ); // Saving Decorations
				displayProgressTen = false;
			}
		}

		// Close file and free up memory allocated by file object
		mFile.Close();

		// Free memory allocated by file object
		mFile.Free();
		return true;
	}

	// No file was saved, return false
	return false;
}

function HandleDecorateLoad( socket, splitString )
{
	var multipleCmd = false;
	var facetCount = facetList.length;
	var objectTypeCount = objectTypeList.length;
	var numArguments = splitString.length;
	loadEvent = false;

	if( eventName != "" )
	{
		loadEvent = true;
	}

	switch( numArguments )
	{
		case 1:
		{
			// Load everything
			// If no facet OR object type is specified - load everything
			decorateArray.length = 0;
			DisplayProgressGump( socket, GetDictionaryEntry( 8045, socket.language ), 0 ); // Loading Decorations
			for( var i = 0; i < facetCount; i++ )
			{
				switch( i )
				{
					case 0:
						if( !silentMode )
						{
							socket.SysMessage( GetDictionaryEntry( 8046, socket.language )); // Loading decorations for Felucca...
						}
						facetID = 0;
						facetName = "felucca";
						break;
					case 1:
						if( !silentMode )
						{
							socket.SysMessage( GetDictionaryEntry( 8047, socket.language )); // Loading decorations for Trammel...
						}
						facetID = 1;
						facetName = "trammel";
						break;
					case 2:
						if( !silentMode )
						{
							socket.SysMessage( GetDictionaryEntry( 8048, socket.language )); // Loading decorations for Ilshenar...
						}
						facetID = 2;
						facetName = "ilshenar";
						break;
					case 3:
						if( !silentMode )
						{
							socket.SysMessage( GetDictionaryEntry( 8049, socket.language )); // Loading decorations for Malas...
						}
						facetID = 3;
						facetName = "malas";
						break;
					case 4:
						if( !silentMode )
						{
							socket.SysMessage( GetDictionaryEntry( 8050, socket.language )); // Loading decorations for Tokuno...
						}
						facetID = 4;
						facetName = "tokuno";
						break;
					case 5:
						if( !silentMode )
						{
							socket.SysMessage( GetDictionaryEntry( 8051, socket.language )); // Loading decorations for Termur...
						}
						facetID = 5;
						facetName = "termur";
						break;
					default:
						if( !silentMode )
						{
							socket.SysMessage( GetDictionaryEntry( 8052, socket.language )); // Invalid facet selected, loading aborted!
						}
						return;
				}

				// With facetID/Name sorted, let's loop through each objectType
				for( var j = 0; j < objectTypeCount; j++ )
				{
					switch( j )
					{
						case 0:
							objectType = "doors";
							break;
						case 1:
							objectType = "signs";
							break;
						case 2:
							objectType = "lights";
							break;
						case 3:
							objectType = "containers";
							break;
						case 4:
							objectType = "moongates";
							break;
						case 5:
							objectType = "teleporters";
							break;
						case 6:
							objectType = "spawners";
							break;
						case 7:
							objectType = "misc";
							break;
						default:
							if( !silentMode )
							{
								socket.SysMessage( GetDictionaryEntry( 8053, socket.language )); // Invalid objectType selected, loading aborted!
							}
							socket.CloseGump( scriptID + 0xffff, 0 );
							return;
					}

					// Load decorations of selected objectType from current facet
					LoadDecorationsFromFile( socket );
				}
			}
			break;
		}
		case 2:
		{
			// Either load a custom template file,
			// a specific objectType on all facets
			// or load all objecttypes for a specific facet

			// First, attempt to get objectType
			GetDecorationType( socket, splitString[1] );

			if( objectType == "" && typeof( splitString[1] ) != "undefined" )
			{
				// We didn't get a valid decorationType - neither one of the default ones, nor a custom one. Did user supply a facet first, instead?
				GetFacet( socket, splitString[1] );
			}

			if( facetID == -1 )
			{
				// We didn't get facetID from splitString[1], so let's try it from splitString[2] instead
				GetFacet( socket, splitString[2] );
			}

			// If no objectType was specified, AND no facetID, then attempt to load custom decorations file instead!
			//if( objectType != "" && objectType != "doors" && objectType != "signs" && objectType != "lights" && objectType != "moongates" && objectType != "teleporters" && objectType != "misc" )
			if( objectType != "" && objectTypeList.indexOf( objectType.toLowerCase() ) == -1 )
			{
				DisplayProgressGump( socket, GetDictionaryEntry( 8045, socket.language ), 0 ); // Loading Decorations
				loadCustom = true;
				if( !LoadDecorationsFromFile( socket ))
				{
					if( !silentMode )
					{
						var tempMsg = GetDictionaryEntry( 8054, socket.language ); // Unable to load decorations from custom file. Does the file (%s.jsdata) exist?
						socket.SysMessage( tempMsg.replace( /%s/gi, objectType ));
					}
					socket.CloseGump( scriptID + 0xffff, 0 );
					return;
				}
			}
			else if( objectType != "" && facetID == -1 )
			{
				DisplayProgressGump( socket, GetDictionaryEntry( 8045, socket.language ), 0 ); // Loading Decorations
				// Object type specified, but not facet. Load object type for ALL facets
				for( var i = 0; i < facetCount; i++ )
				{
					switch( i )
					{
						case 0:
							if( !silentMode )
							{
								var tempMsg = GetDictionaryEntry( 8055, socket.language ); // Loading %s for Felucca...
								socket.SysMessage( tempMsg.replace( /%s/gi, objectType ));
							}
							facetID = 0;
							facetName = "felucca";
							LoadDecorationsFromFile( socket );
							break;
						case 1:
							if( !silentMode )
							{
								var tempMsg = GetDictionaryEntry( 8056, socket.language ); // Loading %s for Trammel...
								socket.SysMessage( tempMsg.replace( /%s/gi, objectType ));
							}
							facetID = 1;
							facetName = "trammel";
							LoadDecorationsFromFile( socket );
							break;
						case 2:
							if( !silentMode )
							{
								var tempMsg = GetDictionaryEntry( 8057, socket.language ); // Loading %s for Ilshenar...
								socket.SysMessage( tempMsg.replace( /%s/gi, objectType ));
							}
							facetID = 2;
							facetName = "ilshenar";
							LoadDecorationsFromFile( socket );
							break;
						case 3:
							if( !silentMode )
							{
								var tempMsg = GetDictionaryEntry( 8058, socket.language ); // Loading %s for Malas...
								socket.SysMessage( tempMsg.replace( /%s/gi, objectType ));
							}
							facetID = 3;
							facetName = "malas";
							LoadDecorationsFromFile( socket );
							break;
						case 4:
							if( !silentMode )
							{
								var tempMsg = GetDictionaryEntry( 8059, socket.language ); // Loading %s for Tokuno...
								socket.SysMessage( tempMsg.replace( /%s/gi, objectType ));
							}
							facetID = 4;
							facetName = "tokuno";
							LoadDecorationsFromFile( socket );
							break;
						case 5:
							if( !silentMode )
							{
								var tempMsg = GetDictionaryEntry( 8060, socket.language ); // Loading %s for Termur...
								socket.SysMessage( tempMsg.replace( /%s/gi, objectType ));
							}
							facetID = 5;
							facetName = "termur";
							LoadDecorationsFromFile( socket );
							break;
						default:
							if( !silentMode )
							{
								socket.SysMessage( GetDictionaryEntry( 8052, socket.language )); // Invalid facet selected, loading aborted!
							}
							socket.CloseGump( scriptID + 0xffff, 0 );
							return;
					}
				}
			}
			else// if( objectType == "" && facetID != -1 )
			{
				// Load all contents of a specific facet, regardless of object type
				DisplayProgressGump( socket, GetDictionaryEntry( 8045, socket.language ), 0 ); // Loading Decorations
				for( var i = 0; i < objectTypeCount; i++ )
				{
					switch( i )
					{
						case 0:
							objectType = "doors";
							LoadDecorationsFromFile( socket );
							break;
						case 1:
							objectType = "signs";
							LoadDecorationsFromFile( socket );
							break;
						case 2:
							objectType = "lights";
							LoadDecorationsFromFile( socket );
							break;
						case 3:
							objectType = "containers";
							LoadDecorationsFromFile( socket );
							break;
						case 4:
							objectType = "moongates";
							LoadDecorationsFromFile( socket );
							break;
						case 5:
							objectType = "teleporters";
							LoadDecorationsFromFile( socket );
							break;
						case 6:
							objectType = "spawners";
							LoadDecorationsFromFile( socket );
							break;
						case 7:
							objectType = "misc";
							LoadDecorationsFromFile( socket );
							break;
						default:
							if( !silentMode )
							{
								socket.SysMessage( GetDictionaryEntry( 8053, socket.language )); // Invalid objectType selected, loading aborted!
							}
							socket.CloseGump( scriptID + 0xffff, 0 );
							return;
					}
				}
			}
			break;
		}
		case 3: // Load specific objectType from specific facet
		case 4: // optional SILENT mode
		case 5: // optional MULTIPLE flag
		{
			var silentMode = false;
			// Load a specific objectType for a specific facet, with optional silent parameter
			if( numArguments >= 4 && splitString[3].toUpperCase() == "SILENT" )
			{
				silentMode = true;
			}

			if( numArguments == 5 && splitString[4].toUpperCase() == "MULTIPLE" )
			{
				multipleCmd = true;
			}

			// Fetch info on what objectType to load
			GetDecorationType( socket, splitString[1] );

			// Fetch info on what facet to load
			GetFacet( socket, splitString[2] );

			DisplayProgressGump( socket, GetDictionaryEntry( 8045, socket.language ), 0 ); // Loading Decorations

			// Attempt to load specified decorations of specified objectType and facet from file
			if( !LoadDecorationsFromFile( socket ))
			{
				if( !silentMode )
				{
					var tempMsg = GetDictionaryEntry( 8061, socket.language ); // Unable to load decorations from file. Does the file (%s_%t.jsdata) you're trying to load exist?
					tempMsg = tempMsg.replace( /%s/gi, facetName );
					socket.SysMessage( tempMsg.replace( /%t/gi, objectType ));
				}
				socket.CloseGump( scriptID + 0xffff, 0 );
				return;
			}
			break;
		}
		default:
			break;
	}

	if( decorateArray.length == 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 8062, socket.language )); // No decorations were loaded from world template files, unable to decorate world.
		socket.CloseGump( scriptID + 0xffff, 0 );
		return;
	}
	else if( !multipleCmd )
	{
		// socket.SysMessage( "Decorations loaded; please wait while world is decorated..." );
		BroadcastMessage( GetDictionaryEntry( 8063, socket.language )); // Decorations loaded; please wait while world is decorated...
		socket.currentChar.StartTimer( 100, 1, true );
	}
	else
	{
		DecorateWorld( socket );
	}
}

function DisplayProgressGump( socket, progressText, percentage )
{
	var fontColor = "";
	if( percentage >= 80 )
	{
		fontColor = "#57E90F";
	}
	else if( percentage >= 60 )
	{
		fontColor = "#A9E90F";
	}
	else if( percentage >= 40 )
	{
		fontColor = "#E9D70F";
	}
	else if( percentage >= 20 )
	{
		fontColor = "#E9940F";
	}
	else
	{
		fontColor = "#E9660F";
	}

	var progressGump = new Gump;
	progressGump.NoClose();
	progressGump.NoMove();
	progressGump.NoDispose();
	progressGump.AddPage( 0 );
	progressGump.AddBackground( 0, 0, 175, 55, 5054 ); // Tile White Background
	progressGump.AddCheckerTrans( 0, 5, 175, 45 );

	progressGump.AddHTMLGump( 10, 7, 155, 20, 0, 0, "<CENTER><BIG><BASEFONT color=#EECD8B>" + progressText + "...</BASEFONT></BIG></CENTER>" );
	progressGump.AddHTMLGump( 10, 27, 155, 20, 0, 0, "<CENTER><BIG><BASEFONT color=" + fontColor + ">" + GetDictionaryEntry( 8064, socket.language ) +" " + ( percentage >= 20 ? percentage + "%" : "") + "</BASEFONT></BIG></CENTER>" );
	progressGump.Send( socket );
	progressGump.Free();
}

function onTimer( timerObj, timerID )
{
	if( timerID == 1 )
	{
		DecorateWorld( timerObj.socket );
	}
}

function LoadDecorationsFromFile( socket )
{
	var mFile = new UOXCFile();
	var fileName = "";

	if( loadEvent )
	{
		fileName = "event_" + eventName + ".jsdata";
	}
	else if( loadCustom )
	{
		fileName = objectType + ".jsdata";
	}
	else
	{
		fileName = facetName + "_" + objectType + ".jsdata";
	}

	mFile.Open( fileName, "r", "worldtemplates", true );

	if( mFile && mFile.Length() >= 0 )
	{
		// Read until we reach end of the file
		while( !mFile.EOF() )
		{
			// Read a line of text from the file
			var line = mFile.ReadUntil( "\n" );
			if( line.length <= 0 || line == "" )
				continue;

			// Add each line to the array of decorations to load
			decorateArray.push( line );
		}

		// Close file and free up memory allocated by file object
		mFile.Close();

		// Free memory allocated by file object
		mFile.Free();

		return true;
	}

	// No file was loaded, return false
	return false;
}

function DecorateWorld( socket )
{
	var arrayLength = decorateArray.length;
	var id = -1;
	var name = "";
	var hue = "";
	var type = -1;
	var x = -1;
	var y = -1;
	var z = -1;
	var worldNum = 0;
	var instanceID = 0;
	var movable = 0;
	var visible = 1;
	var dir = 0;
	var weight = 0;
	var itemWeightMax = 0;
	var maxItems = 0;
	var amount = 0;
	var spawnSection = "";
	var sectionAList = 0;
	var minInterval = 0;
	var maxInterval = 0;
	var scriptTriggers = [];
	var more = 0;
	var moreX = 0;
	var moreY = 0;
	var moreZ = 0;

	var newItemCount = 0;
	var twentyPercent = Math.round( arrayLength * 0.2 );
	var fortyPercent = Math.round( arrayLength * 0.4 );
	var sixtyPercent = Math.round( arrayLength * 0.6 );
	var eightyPercent = Math.round( arrayLength * 0.8 );
	var progress = 0;
	var fontColor = "";

	var templateVer = 0;

	for( var i = 0; i < arrayLength; i++ )
	{
		// Reset scriptTriggers array for each line
		scriptTriggers.length = 0;
		var splitString = decorateArray[i].split( "|" );
		var splitStringLength = splitString.length;
		if( splitStringLength == 1 )
		{
			// Version number exists, read it and move on to the next go
			templateVer = parseInt( splitString );
			continue;
		}

		for( var j = 0; j < splitString.length; j++ )
		{
			if( typeof( splitString[j] ) == "undefined" )
				continue;

			if( j == 0 ) // ID
				id = parseInt( splitString[j] );
			else if ( j == 1 ) // Name
				name = splitString[j];
			else if( j == 2 ) // Hue/Color
				hue = parseInt( splitString[j] );
			else if( j == 3 ) // Type
				type = parseInt( splitString[j] );
			else if( j == 4 ) // X
				x = parseInt( splitString[j] );
			else if( j == 5 ) // Y
				y = parseInt( splitString[j] );
			else if( j == 6 ) // Z
				z = parseInt( splitString[j] );
			else if( j == 7 ) // WorldNumber
				worldNum = parseInt( splitString[j] );
			else if( j == 8 ) // InstanceID
				instanceID = parseInt( splitString[j] );
			else if( j == 9 ) // Movable
				movable = parseInt( splitString[j] );
			else if( j == 10 ) // Visible
				visible = parseInt( splitString[j] );
			else if( j == 11 ) // Dir
				dir = parseInt( splitString[j] );
			else if( j == 12 ) // Weight
				weight = parseInt( splitString[j] );
			else if( j == 13 ) // WeightMax
				itemWeightMax = parseInt( splitString[j] );
			else if( j == 14 ) // MaxItems
				maxItems = parseInt( splitString[j] );
			else if( j == 15 ) // Amount
				amount = parseInt( splitString[j] );
			else if( j == 16 ) // SpawnSection
				spawnSection = splitString[j];
			else if( j == 17 ) // sectionAList
				sectionAList = parseInt( splitString[j] );
			else if( j == 18 ) // minInterval
				minInterval = parseInt( splitString[j] );
			else if( j == 19 ) // maxInterval
				maxInterval = parseInt( splitString[j] );
			else if( templateVer == 0 )
			{
				if( j >= 20 ) // ScriptTriggers
				{
					scriptTriggers.push(parseInt( splitString[j] ));
				}
			}
			else if( templateVer >= 1 )
			{
				// more, morex, morey and morez were added
				if( j == 20 )
					more = parseInt( splitString[j] );
				else if( j == 21 )
					moreX = parseInt( splitString[j] );
				else if( j == 22 )
					moreY = parseInt( splitString[j] );
				else if( j == 23 )
					moreZ = parseInt( splitString[j] );
				else if( j >= 24 )
					scriptTriggers.push( parseInt( splitString[j] ));
			}
		}

		if( i == twentyPercent )
		{
			progress = 20;
			/*fontColor = "#E9940F";
			twentyPercent = -1;*/
		}
		else if( i == fortyPercent )
		{
			progress = 40;
			/*fontColor = "#E9D70F";
			fortyPercent = -1;*/
		}
		else if( i == sixtyPercent )
		{
			progress = 60;
			/*fontColor = "#A9E90F";
			sixtyPercent = -1;*/
		}
		else if( i == eightyPercent )
		{
			progress = 80;
			/*fontColor = "#57E90F";
			eightyPercent = -1;*/
		}

		if( progress == 20 || progress == 40 || progress == 60 || progress == 80 )
		{
			socket.CloseGump( scriptID + 0xffff, 0 );
			DisplayProgressGump( socket, GetDictionaryEntry( 8065, socket.language ), progress ); // Decorating World
			/*var decorateWait = new Gump;
			decorateWait.NoClose();
			decorateWait.NoMove();
			decorateWait.NoDispose();
			decorateWait.AddPage( 0 );
			decorateWait.AddBackground( 0, 0, 175, 55, 5054 ); // Tile White Background
			decorateWait.AddCheckerTrans( 0, 5, 175, 45 );

			decorateWait.AddHTMLGump( 10, 7, 155, 20, 0, 0, "<CENTER><BIG><BASEFONT color=#EECD8B>Decorating World...</BASEFONT></BIG></CENTER>" );
			decorateWait.AddHTMLGump( 10, 27, 155, 20, 0, 0, "<CENTER><BIG><BASEFONT color=" + fontColor + ">Please wait... " + progress + "%</BASEFONT></BIG></CENTER>" );
			decorateWait.Send( socket );
			decorateWait.Free();*/

			if( progress == 20 )
			{
				progress = 0;
			}
			else if( progress == 40 )
			{
				progress = 0;
			}
			else if( progress == 60 )
			{
				progress = 0;
			}
			else if( progress == 80 )
			{
				progress = 0;
			}
		}

		var newItem;
		if(( type >= 61 && type <= 65 ) || type == 69 || type == 125 )
		{
			// Object Spawner, so set the appropriate properties for it
			newItem = CreateBlankItem( socket, socket.currentChar, 1, name, id, hue, "SPAWNER", false );
			if( ValidateObject( newItem ))
			{
				newItem.spawnsection = spawnSection;
				newItem.sectionalist = sectionAList;
				newItem.mininterval = minInterval;
				newItem.maxinterval = maxInterval;
			}
		}
		else
		{
			// Not a spawner, so create a normal item
			newItem = CreateBlankItem( socket, socket.currentChar, 1, name, id, hue, "ITEM", false );
		}

		if( ValidateObject( newItem ))
		{
			newItem.type = type;
			if( x < 0 || y < 0 )
			{
				newItem.Delete();
				socket.SysMessage( "Error: Invalid coordinates (x " + x + ", y " + y + ") detected for an item with ID " + id + ". Deleting item."  );
				Console.Error( "Error: Invalid coordinates (x " + x + ", y " + y + ") detected for an item with ID " + id + ". Deleting item." );
				continue;
			}
			newItem.x = x;
			newItem.y = y;
			newItem.z = z;
			newItem.worldnumber = worldNum;
			newItem.instanceID = instanceID;
			newItem.movable = movable;
			newItem.visible = visible;
			newItem.dir = dir;
			newItem.weight = weight;
			newItem.maxItems = maxItems;
			newItem.decayable = false;
			if( maxItems == 0 )
			{
				newItem.maxItems = 125;
			}
			newItem.weightMax = itemWeightMax;
			if( itemWeightMax == 0 )
			{
				newItem.weightMax = 400000;
			}
			newItem.amount = amount;

			// Add new if statements with each version of worldfile templates
			// Generally, changes are made based on difference between
			// previous version and current, and changes from all previous versions are
			// usually made
			if( templateVer >= 1 )
			{
				newItem.more = more;
				newItem.morex = moreX;
				newItem.morey = moreY;
				newItem.morez = moreZ;
			}
			if( templateVer >= 2 )
			{
				if( spawnSection == "dungeontreasureloot1" )
				{
					newItem.mininterval = 15;
					newItem.maxinterval = 20;
				}
				else if( spawnSection == "dungeontreasureloot2" )
				{
					newItem.mininterval = 20;
					newItem.maxinterval = 25;
				}
				else if( spawnSection == "dungeontreasureloot3" )
				{
					newItem.mininterval = 25;
					newItem.maxinterval = 30;
				}
				else if( spawnSection == "dungeontreasureloot4" )
				{
					newItem.mininterval = 30;
					newItem.maxinterval = 35;
				}
			}

			if( scriptTriggers.length > 0 )
			{
				for( var k = 0; k < scriptTriggers.length; k++ )
				{
					newItem.AddScriptTrigger( scriptTriggers[k] );
				}
			}

			if( loadEvent )
			{
				// Associate the item with the event being loaded
				newItem.event = eventName;
			}
			newItemCount++;
		}
	}

	var tempMsg = GetDictionaryEntry( 8066, socket.language ); // %i decorations added!
	tempMsg = tempMsg.replace( /%i/gi, newItemCount.toString() );
	socket.SysMessage( tempMsg.replace( /%s/gi, objectType ));
	socket.CloseGump( scriptID + 0xffff, 0 );
}

function onIterate( toCheck )
{
	if( ValidateObject( toCheck ) && toCheck.isItem && toCheck.container == null && !toCheck.isMulti && !ValidateObject( toCheck.multi ) && toCheck.shouldSave && !toCheck.decayable )
	{
		if(( saveCustom || saveEvent ) && saveAll )
		{
			// Only save items that match event type if we're saving event decorations
			if( saveEvent && ( toCheck.event ).toLowerCase() != eventName )
				return false;

			/*if( !saveEvent && toCheck.worldnumber != facetID )
				return false;*/

			// Save ALL decorations on ALL facets to one custom file
			SaveDecorationToArray( toCheck, decorateArray );
			return true;
		}

		if( toCheck.worldnumber != facetID )
			return false;

		if( copyMode )
		{
			var newItem = toCheck.Dupe( null );
			if( ValidateObject( newItem ))
			{
				newItem.worldnumber = targetFacetID;
				return true;
			}
		}
		else if( unloadEvent )
		{
			// Remove event decorations
			var eventItemCount = AreaItemFunction( "CheckForEventItems", toCheck, 2, null );
			if( eventItemCount > 0 )
			{
				totalCleaned += eventItemCount;
				return true;
			}
		}
		else if( cleanMode )
		{
			// Clean out duplicate, identical items
			var duplicateCount = AreaItemFunction( "CheckForDuplicate", toCheck, 2, null );
			if( duplicateCount > 0 )
			{
				totalCleaned += duplicateCount;
				return true;
			}
		}
		else if( saveAll )
		{
			// Save ALL objects to their respective arrays
			var id = toCheck.id;
			if( toCheck.type == 12 || toCheck.type == 13 )
			{
				// Doors
				SaveDecorationToArray( toCheck, decorateDoorsArray );
				return true;
			}
			else if( signIDs.indexOf( id ) > -1 )
			{
				// Signs
				SaveDecorationToArray( toCheck, decorateSignsArray );
				return true;
			}
			else if( toCheck.type == 1 || toCheck.type == 8 || toCheck.type == 87 || CheckTileFlag( toCheck.id, 21 )) // 21 = TF_CONTAINER
			{
				// Containers
				SaveDecorationToArray( toCheck, decorateContainersArray );
				return true;
			}
			else if( moongateIDs.indexOf( id ) > -1 )
			{
				// Moongates
				SaveDecorationToArray( toCheck, decorateMoongatesArray );
				return true;
			}
			else if( toCheck.type == 60 )
			{
				// Teleporters
				SaveDecorationToArray( toCheck, decorateTeleportersArray );
				return true;
			}
			else if( toCheck.isSpawner )
			{
				// Spawner Objects
				SaveDecorationToArray( toCheck, decorateSpawnersArray );
				return true;
			}
			else if( CheckTileFlag( toCheck.id, 23 )) // 23 = TF_LIGHT
			{
				// Lights
				SaveDecorationToArray( toCheck, decorateLightsArray );
				return true;
			}
			else
			{
				// Misc
				SaveDecorationToArray( toCheck, decorateMiscArray );
				return true;
			}
		}
		else
		{
			// Save only specified objectType to their respective arrays
			switch( objectType )
			{
				case "doors":
					if( toCheck.type == 12 || toCheck.type == 13 ) // Unlocked/Locked doors
					{
						SaveDecorationToArray( toCheck, decorateDoorsArray );
						return true;
					}
					break;
				case "signs":
				{
					// Check if itemID matches that of a sign
					var id = toCheck.id;
					if( signIDs.indexOf( id ) > -1 )
					{
						SaveDecorationToArray( toCheck, decorateSignsArray );
						return true;
					}
					break;
				}
				case "lights":
				{
					// Check if item has lightsource flag set in tiledata
					var checkFlag = CheckTileFlag( toCheck.id, 23 ); // Flag 23 is TF_LIGHT
					if( checkFlag == true )
					{
						SaveDecorationToArray( toCheck, decorateLightsArray );
						return true;
					}
					break;
				}
				case "containers":
				{
					// Check if item has a container item-type, or if container flag set in tiledata
					if( toCheck.type == 1 || toCheck.type == 8 || toCheck.type == 87 || CheckTileFlag( toCheck.id, 21 ))
					{
						SaveDecorationToArray( toCheck, decorateContainersArray );
						return true;
					}
					break;
				}
				case "moongates":
				{
					var id = toCheck.id;
					if( moongateIDs.indexOf( id ) > -1 )
					{
						SaveDecorationToArray( toCheck, decorateMoongatesArray );
						return true;
					}
					break;
				}
				case "teleporters":
					if( toCheck.type == 60 ) // object teleporter
					{
						SaveDecorationToArray( toCheck, decorateTeleportersArray );
						return true;
					}
					break;
				case "spawners":
					if( toCheck.isSpawner )
					{
						// Spawner Objects
						SaveDecorationToArray( toCheck, decorateSpawnersArray );
						return true;
					}
					break;
				case "misc":
				{
					var id = toCheck.id;
					if(( toCheck.type != 12 && toCheck.type != 13 ) && ( signIDs.indexOf( id ) == -1 ) && ( !CheckTileFlag( toCheck.id, 23 ))
						&& ( toCheck.type != 1 && toCheck.type != 8 && toCheck.type != 87 && !CheckTileFlag( toCheck.id, 21 ))
						&& ( moongateIDs.indexOf( id ) == -1 ) && ( toCheck.type != 60 )
						&& ( !toCheck.isSpawner ))
					{
						// Only save out items that don't fit into another category
						SaveDecorationToArray( toCheck, decorateMiscArray );
						return true;
					}
					break;
				}
				default:
					var toCheckX = toCheck.x;
					var toCheckY = toCheck.y;
					if(( toCheckX >= x1 && toCheckX <= x2 ) && ( toCheckY >= y1 && toCheckY <= y2 ))
					{
						SaveDecorationToArray( toCheck, decorateArray );
						return true;
					}
					break;
			}
		}
	}

	return false;
}

function CheckForEventItems( srcItem, trgItem, pSock )
{
	if(( trgItem.event ).toLowerCase() == eventName )
	{
		trgItem.Delete();
		return true;
	}
	return false;
}

function CheckForDuplicate( srcItem, trgItem, pSock )
{
	if( trgItem != srcItem && trgItem.id == srcItem.id && trgItem.x == srcItem.x && trgItem.y == srcItem.y && trgItem.z == srcItem.z && trgItem.worldnumber == srcItem.worldnumber && trgItem.instanceID == srcItem.instanceID && trgItem.type == srcItem.type && trgItem.color == srcItem.color )
	{
		trgItem.Delete();
		return true;
	}
	return false;
}