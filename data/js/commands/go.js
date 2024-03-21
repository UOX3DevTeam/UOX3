// This command is used to quickly teleport to a specific place, character, or set of coordinates
// See location map at bottom of file for list of locations to teleport directly to using
// 'go [locationName]

function CommandRegistration()
{
	RegisterCommand( "go", 1, true );
}

function command_GO( socket, cmdString )
{
	var mChar = socket.currentChar;
	if( cmdString && mChar )
	{
		var splitString = cmdString.split( " " );
		if( splitString.length == 1 )
		{
			// There was only one entry, let's check locationMap
			var namedLocation = splitString[0].toUpperCase();
			if( locationMap.hasOwnProperty( namedLocation ))
			{
				// Match was found! Teleport player to location ID defined in locationMap
				var locationVal = locationMap[namedLocation];
				mChar.SetLocation( locationVal );
			}
			else
			{
				var locMapKey = locationMap.hasOwnPropertyCI( namedLocation );
				if( locMapKey !== false )
				{
					var locationVal = locationMap[locMapKey];
					mChar.SetLocation( locationVal );
					return;
				}
				// Default to opening the travel menu
				mChar.ExecuteCommand( "itemmenu 6000" );
			}

			return;
		}

		switch( splitString[0].toUpperCase() )
		{
			case "PLACE":
				if( splitString[1] )
				{
					mChar.SetLocation( parseInt( splitString[1] ));
				}
				break;
			case "CHAR":
				var teleTo;
				if( splitString[4] )	// 4-byte Serial
				{
					var a1 = parseInt( splitString[1] );
					var a2 = parseInt( splitString[2] );
					var a3 = parseInt( splitString[3] );
					var a4 = parseInt( splitString[4] );
					teleTo = CalcCharFromSer( a1, a2, a3, a4 );
				}
				else if( splitString[1] )
				{
					var ourNum = parseInt( splitString[1] );
					if( splitString[1].match( /0x/i ))	// Whole Serial
					{
						teleTo = CalcCharFromSer( ourNum );
					}
					else	// Socket index
					{
						var tSock = GetSocketFromIndex( ourNum );
						if( tSock )
						{
							teleTo = tSock.currentChar;
						}
					}
				}

				if( teleTo )
				{
					if( teleTo.commandLevel > mChar.commandLevel )
					{
						socket.SysMessage( GetDictionaryEntry( 19, socket.language )); // You cannot move to your betters!
					}
					else
					{
						mChar.SetLocation( teleTo );
					}
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 1110, socket.language )); // No such character exists!
				}
				break;
			default:
				var x = 0, y = 0, z = 0;
				var worldNumber = mChar.worldnumber;
				var instanceID = mChar.instanceID;
				if( splitString[4] )
				{
					instanceID = parseInt( splitString[4] );
				}

				if( splitString[3] )
				{
					worldNumber = parseInt( splitString[3] );
				}

				if( splitString[2] )
				{
					x = parseInt( splitString[0] );
					y = parseInt( splitString[1] );
					z = parseInt( splitString[2] );
				}
				else
				{
					return;
				}
				mChar.SetLocation( x, y, z, worldNumber, instanceID );
				break;
		}
	}
	else if( mChar )
	{
		mChar.ExecuteCommand( "itemmenu 6000" );
	}
}

// This maps location names with location IDs in locations.dfn
const locationMap = {
	// locationName: locationID
	// Location names should be without spaces

	// Special Locations
	"JailCell1": 1,
	"JailCell2": 2,
	"JailCell3": 3,
	"JailCell4": 4,
	"JailCell5": 5,
	"JailCell6": 6,
	"JailCell7": 7,
	"JailCell8": 8,
	"JailCell9": 9,
	"JailCell10": 10,
	"HelDeskOfHell": 11,
	"HelpRoomBackdoor": 12,
	"GreenAcres": 13,
	"ErrorSpawnLocation": 14,

	// Cities - Britannia
	"BritainLordBritishThrone": 15,
	"CastleBritish": 15,
	"BritainLordBlackthornsCastle": 16,
	"CastleBlackthorn": 16,
	"BritainCenter": 20,
	"Britain": 20,
	"BritainLordBritishCastleEntrance": 21,
	"CastleBritishEntrance": 21,
	"BritainBlackthornCastleEntrance": 22,
	"CastleBlackthornEntrance": 22,
	"BritainSuburbs": 23,
	"BritainPark": 24,
	"BritainFarmlands": 25,
	"BritainCemetery": 26,
	"BritainGraveyard": 26,
	"BritainWestBank": 27,
	"BritainBankWest": 27,
	"BritainEastBank": 28,
	"BritainBankEast": 28,
	"BuccsDen": 30,
	"BuccsDenDocks": 30,
	"BuccsDenBathhouse": 31,
	"BuccsDenTavern": 32,
	"BuccsDenHiddenCaves": 33,
	"BuccsDenBank": 34,
	"Cove": 40,
	"CoveGates": 40,
	"CoveGuardPost": 41,
	"CoveOrcFort": 42,
	"CoveCemetery": 43,
	"CoveGraveyard": 43,
	"CoveBank": 44,
	"Jhelom": 50,
	"JhelomMainIsland": 50,
	"JhelomFightingPit": 51,
	"JhelomEastDocks": 52,
	"JhelomDocksEast": 52,
	"JhelomCemetery": 53,
	"JhelomGraveyard": 53,
	"JhelomMediumIsland": 54,
	"JhelomSmallIsland": 55,
	"JhelomBank": 56,
	"MaginciaDocks": 60,
	"MaginciaParliament": 61,
	"MaginciaPark": 62,
	"MaginciaBank": 63,
	"Minoc": 70,
	"MinocNorth": 70,
	"MinocSouth": 71,
	"MinocGypsyCamp": 72,
	"MinocBridge": 73,
	"MinocMiningCamp": 74,
	"MinocBank": 75,
	"Moonglow": 80,
	"MoonglowCenter": 80,
	"MoonGlowDocks": 81,
	"MoonGlowTelescope": 82,
	"MoonglowZoo": 83,
	"MoonglowCemetery": 84,
	"MoonglowGraveyard": 84,
	"MoonglowBank": 85,
	"Nuhelm": 90,
	"NujelmDocks": 90,
	"NujelmPalace": 91,
	"NujelmNorth": 92,
	"NujelmEast": 93,
	"NujelmWest": 94,
	"NujelmChessBoard": 95,
	"NujelmCemetery": 96,
	"NujelmGraveyard": 96,
	"NujelmBank": 97,
	"Ocllo": 100,
	"OclloDocks": 100,
	"OclloNorth": 101,
	"OclloFarmlands": 102,
	"OclloBank": 103,
	"SerpentsHold": 110,
	"SerpentsHoldNorth": 110,
	"SerpentsHoldWestDocks": 111,
	"SerpentsHoldDocksWest": 111,
	"SerpentsHoldSouth": 112,
	"SerpentsHoldSouthGuardpost": 113,
	"SerpentsHoldBank": 114,
	"SkaraBrae": 120,
	"SkaraBraeWestDocks": 120,
	"SkaraBraeDocksWest": 120,
	"SkaraBraeWest": 121,
	"SkaraBraeEastDocks": 122,
	"SkaraBraeDocksEast": 122,
	"SkaraBraeSouth": 123,
	"SkaraBraeNorth": 124,
	"SkaraBraeEast": 125,
	"SkaraBraeBank": 126,
	"Trinsic": 130,
	"TrinsicCenter": 130,
	"TrinsicWestGate": 131,
	"TrinsicGateWest": 131,
	"TrinsicSouthGate": 132,
	"TrinsicGateSouth": 132,
	"TrinsicNorth": 133,
	"TrinsicSouth": 134,
	"TrinsicEastDocks": 135,
	"TrinsicDocksEast": 135,
	"TrinsicIslandPark": 136,
	"TrinsicPark": 136,
	"TrinsicBankWest": 137,
	"TrinsicWestBank": 137,
	"TrinsicBankEast": 138,
	"TrinsicEastBank": 138,
	"Vesper": 140,
	"VesperCenter": 140,
	"VesperDocks": 141,
	"VesperNorth": 142,
	"VesperEast": 143,
	"VesperGraveyard": 144,
	"VesperCemetery": 144,
	"VesperWest": 145,
	"VesperBank": 146,
	"WindEntrance": 150,
	"WindOutsideEntrance": 150,
	"WindCaves": 151,
	"WindSouth": 152,
	"Wind": 153,
	"WindEast": 153,
	"WindWest": 154,
	"WindPark": 155,
	"Yew": 160,
	"YewCenter": 160,
	"YewEmpathAbbey": 161,
	"YewCourts": 162,
	"YewPrison": 162,
	"YewHiddenCave": 163,
	"YewCemetery": 164,
	"YewGraveyard": 164,
	"YewOrcFort": 165,
	"YewBank": 166,

	// Shrines - Britannia
	"Chaos": 170,
	"Compassion": 171,
	"Honesty": 172,
	"Honor": 173,
	"Humility": 174,
	"Justice": 175,
	"Sacrifice": 176,
	"Spirituality": 177,
	"Valor": 178,

	// Dungeons - Britannia
	"Covetous": 200,
	"CovetousEntrance": 200,
	"CovetousLevel1": 201,
	"CovetousLevel2": 202,
	"CovetousLevel3": 203,
	"CovetousLakeRoom": 204,
	"CovetousTortureRoom": 205,
	"Deceit": 210,
	"DeceitEntrance": 210,
	"DeceitLevel1": 211,
	"DeceitLevel2": 212,
	"DeceitLevel3": 213,
	"DeceitLevel4": 214,
	"Despise": 220,
	"DespiseEntrance": 220,
	"DespiseEntryWay": 221,
	"DespiseLevel1": 222,
	"DespiseLevel2": 223,
	"DespiseLevel3": 224,
	"Destard": 230,
	"DestardEntrance": 230,
	"DestardLevel1": 231,
	"DestardLevel2": 232,
	"DestardLevel3": 233,
	"Hythloth": 240,
	"HythlothEntrance": 240,
	"HythlothLevel1": 241,
	"HythlothLevel2": 242,
	"HythlothLevel3": 243,
	"HythlothLevel4": 244,
	"Shame": 250,
	"ShameEntrance": 250,
	"ShameLevel1": 251,
	"ShameLevel2": 252,
	"ShameLevel3": 253,
	"ShameLevel4": 254,
	"Wrong": 260,
	"WrongEntrance": 260,
	"WrongLevel1": 261,
	"WrongLevel2": 262,
	"WrongLevel3": 263,
	"Ice": 265,
	"IceT2AEntrance": 265,
	"IceBritEntrance": 266,
	"IceLevel1": 267,
	"IceRatmanFort": 268,
	"IceIceDemonLair": 269,
	"Fire": 275,
	"FireT2AEntrance": 275,
	"FireBritEntrance": 276,
	"FireLevel1": 277,
	"FireLevel2": 278,
	"HythlothFirepit": 300,
	"YewBrigandCamp": 301,
	"BritainBrigandCamp": 301,
	"YewFortOfTheDamned": 302,

	// Lost Lands
	"BritainSewers": 303,
	"Papua": 304,
	"PapuaMageShop": 304,
	"Delucia": 305,
	"DeluciaCenter": 305,
	"IceDungeon": 306,
	"PapuaBank": 307,
	"DeluciaBank": 308,

	// Cities - Ilshenar
	"GargoyleCityBank": 400,
	"GargoyleCity": 401,
	"GargoyleCityCenter": 401,
	"Lakeshire": 402,
	"DesertOutpostWest": 403,
	"DesertOutpostEast": 404,
	"Mistas": 405,
	"Montor": 406,
	"RegVolon": 407,
	"SavageVillage": 408,
	"TerortSkitas": 409,

	// Dungeons - Ilshenar
	"Ankh": 420,
	"AnkhEntrance": 421,
	"AnkhLevel1": 422,
	"AnkhKirinPassage": 423,
	"AnkhSerpentinePassage": 424,
	"Blood": 430,
	"BloodEntrance": 430,
	"BloodLevel1": 431,
	"Exodus": 435,
	"ExodusEntrance": 435,
	"ExodusLevel1": 436,
	"Rock": 440,
	"RockEntrance": 440,
	"RockLevel1": 441,
	"RockLevel2": 442,
	"SorcerersDungeon": 445,
	"SorcerersDungeonEntrance": 445,
	"SorcerersDungeonLevel1": 446,
	"SorcerersDungeonLevel2": 447,
	"SorcerersDungeonLevel3": 448,
	"SorcerersDungeonLevel4": 449,
	"SorcerersDungeonLevel5": 450,
	"Spectre": 460,
	"SpectreEntrance": 460,
	"SpectreLevel1": 461,
	"Wisp": 465,
	"WispEntrance": 465,
	"WispLevel1": 466,
	"WispLevel2": 467,
	"WispLevel3": 468,
	"WispLevel4": 469,
	"WispLevel5": 470,
	"WispLevel6": 471,
	"WispLevel7": 472,
	"WispLevel8": 473,
	"TwistedWeald": 485,

	// Caves - Ilshenar
	"AncientLair": 490,
	"AncientLairEntrance": 490,
	"AncientLairLevel1": 491,
	"LizardPassage": 495,
	"LizardPassageEntrance": 495,
	"LizardPassageLevel1": 496,
	"LizardPassageLevel2": 497,
	"MushroomCave": 500,
	"MushroomCaveEntrance": 500,
	"RatCave": 505,
	"RatCaveEntrance": 505,
	"RatCaveLevel1": 506,
	"RatCaveLevel2": 507,
	"SpiderCave": 510,
	"SpiderCaveEntrance": 510,
	"SpiderCaveLevel1": 511,
	"SpiderCaveLevel2": 512,
	"SpiderCaveEtherealKeep": 513,

	// Shrines - Ilshenar
	"IlshenarCompassion": 520,
	"IlshenarHonesty": 521,
	"IlshenarHonor": 522,
	"IlshenarHumility": 523,
	"IlshenarJustice": 524,
	"IlshenarSacrifice": 525,
	"IlshenarSpirituality": 526,
	"IlshenarValor": 527,
	"GargoyleShrineOfVirtues": 528,

	// Other - Ilshenar
	"AncientCitadel": 535,
	"HarpyNest": 536,
	"HealersGrove": 537,
	"BanditTown": 538,
	"Bet-LemReg": 539,
	"BurnedForest": 540,
	"IlshenarCastleBlackthorn": 541,
	"CentralIlshenarDesert": 542,
	"CyclopsCamp": 543,
	"CyclopsPyramid": 544,
	"ElementalValley": 545,
	"GargoyleMiningCamp": 546,
	"LizardmanVillage": 547,
	"MeerCatacombs": 548,
	"MeerCatacombsInside": 549,
	"RatmanVilla": 550,
	"PassOfKarnaughLevel1": 551,
	"PassOfKarnaughLevel2": 552,
	"SavageCamp": 553,
	"SheepFarm": 554,
	"UndeadCamp": 555,
	"UndeadFort": 556,

	// Cities - Malas
	"Luna": 600,
	"LunaCenter": 600,
	"LunaBank": 600,
	"LunaShrineOfWisdom": 601,
	"Umbra": 610,
	"UmbraCenter": 610,
	"UmbraBank": 610,

	// Dungeons - Malas
	"Doom": 620,
	"DoomEntrance": 620,
	"DoomTunnel": 621,
	"DoomInside": 622,
	"DoomGauntlet": 623,
	"DoomGuardiansRoom": 624,
	"DoomLampRoom": 625,
	"Labyrinth": 626,
	"LabyrinthEntrance": 626,
	"LabyrinthInside": 627,
	"LabyrinthNorthWest": 628,
	"LabyrinthCenter": 629,
	"LabyrinthEnd": 630,

	// Points of Interest - Malas
	"BrokenMountains": 640,
	"CorruptedForest": 641,
	"CrumblinContinent": 642,
	"CrystalFens": 643,
	"DivideOfTheAbyss": 644,
	"DryHighlands": 645,
	"ForgottenPyramid": 646,
	"GravewaterLake": 647,
	"GrimswindRuins": 648,
	"NorthernCrags": 649,
	"OrcFortress": 650,
	"HansesHostel": 651,

	// Caves/Mines - Malas
	"MalasMine1": 665,
	"MalasMine2": 666,
	"MalasMine3": 667,
	"MalasMine4": 668,
	"MalasMine5": 669,
	"MalasMine6": 670,
	"MalasMine7": 671,
	"MalasMine8": 672,
	"MalasMine9": 673,

	// Orc Forts - Malas
	"MalasOrcFort1": 680,
	"MalasOrcFort2": 681,
	"MalasOrcFort3": 682,
	"MalasOrcFort4": 683,
	"MalasOrcFort5": 684,
	"MalasOrcFort6": 685,

	// Cities - Tokuno Islands
	"Zento": 800,
	"ZentoCenter": 800,
	"ZentoBank": 800,
	"ZentoShrine": 801,
	"ZentoMoongate": 802,
	"TheWaste": 810,
	"InuTheCrone": 811,

	// Isamu-Jima - Tokuno Islands
	"IsamuMoongate": 820,
	"IsamuShrine": 821,
	"LotusLakes": 822,
	"MountSho": 823,
	"DragonValley": 824,
	"WinterSpur": 825,

	// Homare-Jima - Tokuno Islands
	"HomareMoongate": 830,
	"HomareShrine": 831,
	"FieldOfEchoes": 832,
	"CraneMarsh": 833,
	"BushidoDojo": 834,
	"KitsuneWoods": 835,

	// Dungeons - Tokuno Islands
	"FanDancerDojo": 840,
	"FanDancerDojoEntrance": 840,
	"FanDancerDojoLevel1": 841,
	"FanDancerDojoLevel2": 842,
	"FanDancerDojoLevel3": 843,
	"TheCitadel": 851,
	"TheCitadelEntrance": 851,
	"TheCitadelInside": 852,
	"YomotsuMines": 860,
	"YomotsuMinesEntrance": 860,

	// Cities - Ter Mur
	"RoyalCity": 1000,
	"RoyalCityBank": 1000,
	"RoyalCityMoongate": 1001,
	"HolyCity": 1020,
	"HolyCityMoongate": 1020,
	"Dugan": 1030,

	// Dungeons - Ter Mur
	"TombOfKings": 1050,
	"TombOfKingsEntrance": 1050,
	"TombOfKingsGateToStygianAbyss": 1051,
	"StygianAbyssExitToTombOfKings": 1055,
	"StygianAbyssExitToUnderworld": 1056,
	"StygianAbyssAbyssalLair": 1057,
	"StygianAbyssCavernsOfTheDiscarded": 1058,
	"StygianAbyssClanScratch": 1059,
	"StygianAbyssCrimsonVeins": 1060,
	"StygianAbyssEnslavedGoblins": 1061,
	"StygianAbyssFairyDragonLair": 1062,
	"StygianAbyssFireTempleRuins": 1063,
	"StygianAbyssHydra": 1064,
	"StygianAbyssLandsOfTheLich": 1065,
	"StygianAbyssLavaCaldera": 1066,
	"StygianAbyssMedusasLair": 1067,
	"StygianAbyssPassageOfTears": 1068,
	"StygianAbyssSecretGarden": 1069,
	"StygianAbyssSerpentLair": 1070,
	"StygianAbyssSilverSapling": 1071,
	"StygianAbyssStygianDragonLair": 1072,
	"StygianAbyssSutekTheMage": 1073,
	"Underworld": 1080,
	"UnderworldEntrance": 1080,

	// Points of Interest - Ter Mur
	"AtollBend": 1100,
	"ChickenChase": 1101,
	"CityResidential": 1102,
	"CoralDesert": 1103,
	"FishermansReach": 1104,
	"GatedIsle": 1105,
	"HighPlain": 1106,
	"HolyCityIsland": 1107,
	"KepetchWaste": 1108,
	"LavaLake": 1109,
	"LavapitPyramid": 1110,
	"LostSettlement": 1111,
	"NorthernSteppes": 1112,
	"RaptorIsland": 1113,
	"RoyalPark": 1114,
	"ShrineOfSingularity": 1115,
	"SlithValley": 1116,
	"SpiderIsland": 1117,
	"SpidersGuarde": 1118,
	"TalonPoint": 1119,
	"TreefellowCourse": 1120,
	"VoidIsle": 1121,
	"WalledCircus": 1122,
	"WaterfallPoint": 1123,

	// Define a custom method - hasOwnPropertyCI - on the object, which can
	// be used to do case-insensitive property comparisons
	hasOwnPropertyCI: function( prop ) {
		for( var key in this )
		{
			if( this.hasOwnProperty( key ) && key.toLowerCase() === prop.toLowerCase() )
			{
				return key;
			}
		}
		return false;
	}
};

function _restorecontext_() {}