// This command is used to show the item type and some details about that for targeted object

function CommandRegistration()
{
	RegisterCommand( "showdetail", 1, true );
}

function command_SHOWDETAIL( socket, cmdString )
{
	socket.CustomTarget( 0, GetDictionaryEntry( 8110, socket.language )); // Select Item to get Type Info from:
}

function onCallback0( socket, ourObj )
{
	var message;

	if( !socket.GetWord( 1 ) && ourObj )
	{
		switch( ourObj.type )
		{
		case 0:	// no type
			message = "Default type";
			break;
		case 1:	// container/backpack
			message = "Container/backpack:";
			if( ourObj.moreb > 0 )
			{
				message += "Magically trapped";
			}
			break;
		case 2:	// opener for castle gate 1
		case 4:	// opener for castle gate 2
			message = "Opener for castle gate";
			break;
		case 3:	// castle gate 1
		case 5:	// castle gate 2
			message = "Castle gate";
			break;
		case 6:	// teleporter rune
			message = "Teleporter rune, acts like teleport was cast";
			break;
		case 7:	// key
			message = "Key";
			break;
		case 8:	// locked container
			message = "Locked container:";
			if( ourObj.moreb > 0 )
			{
				message += "Magically trapped";
			}
			break;
		case 9:	// Spellbook (item 14FA)
			message = "Spellbook";
			break;
		case 10: // map( item 0E EB )
			message = "This opens a map based on the serial num of the item";
			break;
		case 11: // book
			message = "A book:Entry in misc.scp: " + ourObj.more.toString();
			break;
		case 12: // doors
			message = "Unlocked door";
			break;
		case 13: // locked door
			message = "Locked door";
			break;
		case 14: // food
			message = "Food item, reduces hunger by one point";
			break;
		case 15: // magic wand
			message = "Magic wand\nCircle: " + ourObj.morex.toString();
			message += ":Spell within circle: " + ourObj.morey.toString();
			message += ":Charges left: " + ourObj.morez.toString();
			break;
		case 16: // resurrection object
			message = "Resurrection object";
			break;
		case 18: // enchanted item (Crystal ball)
			message = "Enchanted item that displays a random message";
			break;
		case 19: // potion
			message = "Potion: ";
			switch( ourObj.morey )
			{
				case 1: // Agility Potion
					switch( ourObj.morez )
					{
						case 1:		message += "Agility potion";		break;
						case 2:		message += "Greater Agility potion";	break;
						default:	message += "Unknown Agility potion";	break;
					}
					break;
				case 2: // Cure Potion
					switch( ourObj.morez )
					{
						case 1:		message += "Lesser cure potion"; 	break;
						case 2:		message += "Cure potion"; 		break;
						case 3:		message += "Greater Cure potion"; 	break;
						default:	message += "Unknown cure potion"; 	break;
					}
					break;
				case 3: // Explosion Potion
					message += "Explosion potion";
					break;
				case 4: // Heal Potion
					switch( ourObj.morez )
					{
						case 1:		message += "Lesser Heal potion";	break;
						case 2:		message += "Heal potion"; 		break;
						case 3:		message += "Greater Heal potion"; 	break;
						default:	message += "Unknown Heal potion";	break;
					}
					break;
				case 5: // Night Sight Potion
					message += "Night sight potion";
					break;
				case 6: // Poison Potion
					switch( ourObj.morez )
					{
					case 0:		message += "Poison potion with no poison"; 	break;
					case 1:		message += "Lesser Poison potion"; 		break;
					case 2:		message += "Poison potion"; 			break;
					case 3:		message += "Greater Poison potion"; 		break;
					case 4:		message += "Deadly Poison potion"; 		break;
					default:	message += "Unknown Poison potion";  		break;
					}
					break;
				case 7: // Refresh Potion
					switch( ourObj.morez )
					{
					case 1:		message += "Lesser Refresh potion"; 		break;
					case 2:		message += "Refresh potion";			break;
					default:	message += "Unknown Refresh potion"; 		break;
					}
					break;
				case 8: // Strength Potion
					switch( ourObj.morez )
					{
					case 1:		message += "Lesser Strength potion";		break;
					case 2:		message += "Strength potion"; 			break;
					default:	message += "Unknown Strength potion"; 		break;
					}
					break;
				case 9: // Mana Potion
					switch( ourObj.morez )
					{
					case 1:		message += "Lesser Mana potion";		break;
					case 2:		message += "Mana potion"; 			break;
					default:	message += "Unknown Mana potion"; 		break;
					}
					break;
				default:
					message += "Unknown potion";
					break;
			}
			break;
		case 35: // townstone deed/stone
			if( ourObj.id == 0x14F0 )
			{
				message = "Townstone deed, will make townstone";
			}
			else
			{
				message = "Townstone, use to find out information on the town";
			}
			break;
		case 50: // recall rune
			if( ourObj.morex == 0 && ourObj.morey == 0 && ourObj.morez == 0 )	// changed, to fix, Lord Vader
			{
				message ="Unmarked recall rune";
			}
			else
			{
				message = "This will rename a recall rune";
			}
			break;
		case 51: // Moongate;
		case 52:
			var moonGate;
			moonGate = CalcItemFromSer( ourObj.morex );
			message = "Moongate going to X " + moonGate.x.toString();
			message += " Y " + moonGate.y.toString();
			message += " Z " + moonGate.z.toString();
			break;
		case 60: // object teleporter
			message = "A teleporter going to X " + ourObj.morex.toString();
			message += " Y " + ourObj.morey.toString();
			message += " Z " + ourObj.morez.toString();
			break;
		case 61: // item spawner
			message = "Item spawner:ItemEntry: " + ourObj.spawnsection;
			message += ":Respawn max time: " + ourObj.maxinterval.toString();
			message += ":Respawn min time: " + ourObj.mininterval.toString();
			break;
		case 62:	// monster/npc spanwer
			message = "NPC spawner:NpcEntry: " + ourObj.spawnsection;
			message += ":Respawn max time: " + ourObj.maxinterval.toString();
			message += ":Respawn min time: " + ourObj.mininterval.toString();
			break;
		case 63:	// spawn container
			message = "Item Spawn container:";
			if( ourObj.moreb > 0 )
			{
				message += "Magically trapped:";
			}
			message += ":Respawn max time: " + ourObj.maxinterval.toString();
			message += ":Respawn min time: " + ourObj.mininterval.toString();
			break;
		case 64:	// locked spawn container
			message = "Locked item spawn container:";
			if( ourObj.moreb > 0 )
			{
				message += "Magically trapped:";
			}
			message += ":Respawn max time: " + ourObj.maxinterval.toString();
			message += ":Respawn min time: " + ourObj.mininterval.toString();
			break;
		case 65:	// unlockable item spawner container
			message = "Unlockable item spawner container";
			break;
		case 69:	// area spawner
			message = "Area spawner:X +/- value: " + ourObj.more.toString();
			message += "Y +/- value: " + ourObj.more.toString();
			message += "Amount: " + ourObj.amount.toString();
			message += "NpcEntry: " + ourObj.spawnsection;
			message += "Respawn max time: " + ourObj.maxinterval.toString();
			message += "Respawn min time: " + ourObj.mininterval.toString();
			break;
		case 80:	// single use advancement gate
			message = "Single use advancement gate: advance.scp entry " + ourObj.morex.toString();
			break;
		case 81:	// multi-use advancement gate
			message = "Multi use advancement gate: advance.scp entry " + ourObj.morex.toString();
			break;
		case 82:	// monster gate
			message = "Monster gate: monster number " + ourObj.morex.toString();
			break;
		case 83:	// race gates
			message = "Race Gate:Turns into race num: " + ourObj.morex.toString();
			if( ourObj.morey == 1 )
			{
				message += "Constantly reuseable:";
			}
			else
			{
				message += "One time use only:";
			}
			break;
		case 85:	// damage object
			var minDam = ourObj.morex + ourObj.morey;
			var maxDam = ourObj.morex + ourObj.morez;
			message = "Damage object:Minimum damage " + minDam.toString();
			message += "Maximum Damage " + maxDam.toString();
			break;
		case 87:	// trash container
			message = "A trash container";
			break;
		case 88:	// sound object
			message = "Sound object that plays whenever someone is near:Soundeffect: " + ourObj.morex.toString();
			message += "\nRadius: " + ourObj.morey.toString();
			message += "\nProbability: " + ourObj.morez.toString();
			break;
		case 89:	// map change
			message = "Map Change object that changes world:World: " + ourObj.more.toString();
			break;
		case 101:	// morph object morex = npc# in npc.scp
			var id1 = (ourObj.morex>>8);
			var id2 = (ourObj.morex%256);
			message = "Morph object:Morphs char into body " + id1.toString() + " " + id2.toString();
			break;
		case 102:	// unmorph
			message = "Unmorph object, unmorphs back to body before using it as type 101, switches to 101 again";
			break;
		case 105:	// drink object
			message = "You can drink this";
			break;
		case 117:	// plank
			message = "This is a plank for a boat";
			break;
		case 125:	// escort npc spawner
			message = "Escort NPC spawner, behaves like type 62/69";
			break;
		case 118:	// fireworks wand
			message = "A fireworks wand with " + ourObj.morex.toString() + " charges left on it";
			break;
		case 202:	// guildstone deed
			message = "Guildstone deed";
			break;
		case 203:	// opens gump menu
			message = "Opens housing gump: ";
			break;
		case 217:	// player vendor deed
			message = "Player vendor deed";
			break;
		case 90:	// world change gate
			message = "World change gate: " + ourObj.morex.toString();
			break;
		}
		socket.SysMessage( message );
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 1656 )); // Invalid item selected!
	}
}
