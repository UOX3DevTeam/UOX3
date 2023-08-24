// Also supports "Enchanted" sextant given to Young players as part of the New Player Experience,
// which points players in direction of nearest moongate, and to nearest bank

const feluccaMoongates = [
	[1336, 1997, "Britain"],
	[3563, 2139, "Magincia"],
	[4467, 1283, "Moonglow"],
	[643, 2067, "Skara Brae"],
	[1828, 2948, "Trinsic"],
	[2701, 692, "Vesper"],
	[771, 752, "Yew"],
	[1499, 3771, "Jhelom"]
];
const trammelMoongates = [
	[1336, 1997, "Britain"],
	[3563, 2139, "Magincia"],
	[4467, 1283, "Moonglow"],
	[643, 2067, "Skara Brae"],
	[1828, 2948, "Trinsic"],
	[2701, 692, "Vesper"],
	[771, 752, "Yew"],
	[1499, 3771, "Jhelom"]
];
const ilshenarMoongates = [
	[1215, 467, "Compassion"],
	[722, 1366, "Honesty"],
	[744, 724, "Honor"],
	[281, 1016, "Humility"],
	[987, 1011, "Justice"],
	[1174, 1286, "Sacrifice"],
	[1532, 1340, "Spirituality"],
	[528, 216, "Valor"],
	[1721, 218, "Chaos"]
];
const malasMoongates = [
	[1015, 527, "New Luna"],
	[1054, 358, "Old Luna"],
	[1997, 1386, "Umbra"]
];
const tokunoMoongates = [
	[1169, 998, "Isamu~Jima"],
	[801, 1204, "Makoto~Jima"],
	[270, 628, "Homare~Jima"]
];
const termurMoongates = [
	[855, 3526, "Royal City"],
	[928, 3989, "Holy City"],
	[1107, 1131, "Dugan"]
];

const feluccaBanks = [
	[1436, 1693, "Britain"],
	[2232, 1199, "Cove"],
	[2732, 2192, "Buccaneer's Den"],
	[1328, 3771, "Jhelom"],
	//[3728, 2160, "Magincia"], // Only pre-destruction of Magincia
	[2505, 560, "Minoc"],
	[4469, 1176, "Moonglow"],
	[3768, 1313, "Nujel'm"],
	[3690, 2520, "Ocllo"],
	[2879, 3471, "Serpent's Hold"],
	[595, 2152, "Skara Brae"],
	[1819, 2825, "Trinsic"],
	[1904, 2686, "Trinsic"],
	[2896, 673, "Vesper"],
	[660, 816, "Yew"],
	[5674, 3139, "Papua"],
	[5271, 3988, "Delucia"]
];
const trammelBanks = [
	[1436, 1693, "Britain"],
	[2232, 1199, "Cove"],
	[2732, 2192, "Buccaneer's Den"],
	[1328, 3771, "Jhelom"],
	//[3728, 2160, "Magincia"], // Only pre-destruction of Magincia
	[2505, 560, "Minoc"],
	[4469, 1176, "Moonglow"],
	[3768, 1313, "Nujel'm"],
	[3690, 2520, "Ocllo"],
	[2879, 3471, "Serpent's Hold"],
	[595, 2152, "Skara Brae"],
	[1819, 2825, "Trinsic"],
	[1904, 2686, "Trinsic"],
	[2896, 673, "Vesper"],
	[660, 816, "Yew"],
	[5674, 3139, "Papua"],
	[5271, 3988, "Delucia"]
];
const ilshenarBanks = [
	[1228, 553, "a gypsy caravan"],
	[1613, 556, "a desert outpost"]
];
const malasBanks = [
	[990, 520, "Luna Bank"],
	[2047, 1346, "Umbra Bank"],
];
const tokunoBanks = [
	[735, 1255, "Zento Bank"]
];
const termurBanks = [
	[842, 3440, "Royal Bank"]
];

// If false, regular players will also get directions to nearest bank and/or moongate
const youngPlayerDirectionsOnly = true;

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && ValidateObject( iUsed ) && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			socket.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
			return false;
		}

		if( pUser.region.isDungeon || IsInBuilding( pUser.x, pUser.y, pUser.z, pUser.worldnumber, pUser.instanceID, true ))
		{
			// Cannot use in a dungeon
			socket.SysMessage( GetDictionaryEntry( 2789, socket.language )); // You can only use this outdoors.
			return false;
		}

		var mapCoords = TriggerEvent( 2503, "GetMapCoordinates", iUsed.morex, iUsed.morey, iUsed.morez );
		var mapCoordsString = mapCoords[3] + "o " + mapCoords[4] + "'" + ( mapCoords[5] ? "S" : "N" ) + " " + mapCoords[0] + "o " + mapCoords[1] + "'" + ( mapCoords[2] ? "E" : "W" );
		var finalString = GetDictionaryEntry( 274, socket.language ); // You are at %s
		finalString = ( finalString.replace( /%s/gi, mapCoordsString ));
		socket.SysMessage( finalString );

		if( !youngPlayerDirectionsOnly || pUser.account.isYoung )
		{
			var moongates;
			var banks;
			// Show directions to nearest bank, and to nearest moongate
			switch( pUser.worldnumber )
			{
				case 0:
					moongates = feluccaMoongates;
					banks = feluccaBanks;
					break;
				case 1:
					moongates = trammelMoongates;
					banks = trammelBanks;
					break;
				case 2:
					moongates = ilshenarMoongates;
					banks = ilshenarBanks;
					break;
				case 3:
					moongates = malasMoongates;
					banks = malasBanks;
					break;
				case 4:
					moongates = tokunoMoongates;
					banks = tokunoBanks;
					break;
				case 5:
					moongates = termurMoongates;
					banks = termurBanks;
					break;
				default:
					// Handle invalid world number
					break;
			}
			const closestMoongate = FindClosestLocation( pUser.x, pUser.y, moongates );
			const closestBank = FindClosestLocation( pUser.x, pUser.y, banks );

			pUser.SysMessage( GetDictionaryEntry( 18744, socket.language ), closestMoongate.direction, closestMoongate.name, closestMoongate.distance ); // Nearest moongate: %s, near %s (%i m away)
			pUser.SysMessage( GetDictionaryEntry( 18745, socket.language ), closestBank.direction, closestBank.name, closestBank.distance ); // Nearest bank: %s, near %s (%i m away)
		}

	}
	return false;
}

function FindClosestLocation( pX, pY, locations )
{
	var closestLocation = null;
	var closestDistance = -1;

	for( var i = 0; i < locations.length; i++ )
	{
		const [x, y, name] = locations[i];
		const distance = DistanceBetween( pX, pY, x, y );

		if( closestDistance == -1 || distance < closestDistance )
		{
			const dx = x - pX;
			const dy = y - pY;
			const angle = Math.atan2( dy, dx );
			const degrees = (angle * 180) / Math.PI;
			var direction;

			if( degrees >= -22.5 && degrees <= 22.5 )
			{
				direction = "east";
			}
			else if( degrees > 22.5 && degrees < 67.5 )
			{
				direction = "southeast";
			}
			else if( degrees >= 67.5 && degrees <= 112.5 )
			{
				direction = "south";
			}
			else if( degrees > 112.5 && degrees < 157.5 )
			{
				direction = "southwest";
			}
			else if( degrees >= 157.5 || degrees <= -157.5 )
			{
				direction = "west";
			}
			else if( degrees > -157.5 && degrees < -112.5 )
			{
				direction = "northwest";
			}
			else if( degrees >= -112.5 && degrees <= -67.5 )
			{
				direction = "north";
			}
			else if( degrees > -67.5 && degrees < -22.5 )
			{
				direction = "northeast";
			}

			closestLocation = { "name": name, "distance": distance, "direction": direction };
			closestDistance = distance;
		}
	}

	return closestLocation;
}