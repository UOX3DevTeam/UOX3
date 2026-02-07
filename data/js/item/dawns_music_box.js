/// <reference path="../../definitions.d.ts" />
// @ts-check

let DawnsMusicRarity = {
	Common: 0,
	Uncommon: 1,
	Rare: 2
};

let DawnsMusicInfo = {
	0:  { name: "OldUlt01",             cliloc: 1075142, rarity: DawnsMusicRarity.Common },
	8:  { name: "Stones2",              cliloc: 1075143, rarity: DawnsMusicRarity.Common },
	9:  { name: "Britain1",             cliloc: 1075144, rarity: DawnsMusicRarity.Common },
	10: { name: "Britain2",             cliloc: 1075145, rarity: DawnsMusicRarity.Common },
	11: { name: "Bucsden",              cliloc: 1075146, rarity: DawnsMusicRarity.Common },
	12: { name: "Jhelom",               cliloc: 1075147, rarity: DawnsMusicRarity.Common },
	13: { name: "LBCastle",             cliloc: 1075148, rarity: DawnsMusicRarity.Common },
	14: { name: "Linelle",              cliloc: 1075185, rarity: DawnsMusicRarity.Common },
	15: { name: "Magincia",             cliloc: 1075149, rarity: DawnsMusicRarity.Common },
	16: { name: "Minoc",                cliloc: 1075150, rarity: DawnsMusicRarity.Common },
	17: { name: "Ocllo",                cliloc: 1075151, rarity: DawnsMusicRarity.Common },
	18: { name: "Samlethe",             cliloc: 1075152, rarity: DawnsMusicRarity.Common },
	19: { name: "Serpents",             cliloc: 1075153, rarity: DawnsMusicRarity.Common },
	20: { name: "Skarabra",             cliloc: 1075154, rarity: DawnsMusicRarity.Common },
	21: { name: "Trinsic",              cliloc: 1075155, rarity: DawnsMusicRarity.Common },
	22: { name: "Vesper",               cliloc: 1075156, rarity: DawnsMusicRarity.Common },
	23: { name: "Wind",                 cliloc: 1075157, rarity: DawnsMusicRarity.Common },
	24: { name: "Yew",                  cliloc: 1075158, rarity: DawnsMusicRarity.Common },
	25: { name: "Cave01",               cliloc: 1075159, rarity: DawnsMusicRarity.Common },
	26: { name: "Dungeon9",             cliloc: 1075160, rarity: DawnsMusicRarity.Common },
	27: { name: "Forest_a",             cliloc: 1075161, rarity: DawnsMusicRarity.Common },
	30: { name: "Mountn_a",             cliloc: 1075162, rarity: DawnsMusicRarity.Common },
	32: { name: "Sailing",              cliloc: 1075163, rarity: DawnsMusicRarity.Common },
	34: { name: "Tavern01",             cliloc: 1075164, rarity: DawnsMusicRarity.Common },
	35: { name: "Tavern02",             cliloc: 1075165, rarity: DawnsMusicRarity.Common },
	36: { name: "Tavern03",             cliloc: 1075166, rarity: DawnsMusicRarity.Common },
	37: { name: "Tavern04",             cliloc: 1075167, rarity: DawnsMusicRarity.Common },
	38: { name: "Combat1",              cliloc: 1075168, rarity: DawnsMusicRarity.Common },
	39: { name: "Combat2",              cliloc: 1075169, rarity: DawnsMusicRarity.Common },
	40: { name: "Combat3",              cliloc: 1075170, rarity: DawnsMusicRarity.Common },
	42: { name: "Death",                cliloc: 1075171, rarity: DawnsMusicRarity.Common },
	43: { name: "Victory",              cliloc: 1075172, rarity: DawnsMusicRarity.Common },
	44: { name: "BTCastle",             cliloc: 1075173, rarity: DawnsMusicRarity.Common },
	45: { name: "Nujelm",               cliloc: 1075174, rarity: DawnsMusicRarity.Common },
	47: { name: "Cove",                 cliloc: 1075176, rarity: DawnsMusicRarity.Common },
	48: { name: "Moonglow",             cliloc: 1075177, rarity: DawnsMusicRarity.Common },
	49: { name: "Zento",                cliloc: 1075178, rarity: DawnsMusicRarity.Common },
	50: { name: "TokunoDungeon",        cliloc: 1075179, rarity: DawnsMusicRarity.Common },
	51: { name: "Taiko",                cliloc: 1075180, rarity: DawnsMusicRarity.Common },

	52: { name: "DreadHornArea",        cliloc: 1075181, rarity: DawnsMusicRarity.Uncommon },
	53: { name: "ElfCity",              cliloc: 1075182, rarity: DawnsMusicRarity.Uncommon },
	54: { name: "GrizzleDungeon",       cliloc: 1075186, rarity: DawnsMusicRarity.Uncommon },
	55: { name: "MelisandesLair",       cliloc: 1075183, rarity: DawnsMusicRarity.Uncommon },
	56: { name: "ParoxysmusLair",       cliloc: 1075184, rarity: DawnsMusicRarity.Uncommon },
	57: { name: "GwennoConversation",   cliloc: 1075131, rarity: DawnsMusicRarity.Uncommon },
	58: { name: "GoodEndGame",          cliloc: 1075132, rarity: DawnsMusicRarity.Uncommon },
	59: { name: "GoodVsEvil",           cliloc: 1075133, rarity: DawnsMusicRarity.Uncommon },
	60: { name: "GreatEarthSerpents",   cliloc: 1075134, rarity: DawnsMusicRarity.Uncommon },
	61: { name: "Humanoids_U9",         cliloc: 1075135, rarity: DawnsMusicRarity.Uncommon },
	62: { name: "MinocNegative",        cliloc: 1075136, rarity: DawnsMusicRarity.Uncommon },
	63: { name: "Paws",                 cliloc: 1075137, rarity: DawnsMusicRarity.Uncommon },

	64: { name: "SelimsBar",            cliloc: 1075138, rarity: DawnsMusicRarity.Rare },
	65: { name: "SerpentIsleCombat_U7", cliloc: 1075139, rarity: DawnsMusicRarity.Rare },
	66: { name: "ValoriaShips",         cliloc: 1075140, rarity: DawnsMusicRarity.Rare }
};

function DawnsRandomTrack( rarity )
{
	var list = [];
	for( var key in DawnsMusicInfo )
	{
		if( DawnsMusicInfo.hasOwnProperty( key ))
		{
			var info = DawnsMusicInfo[key];
			if( info && info.rarity === rarity )
			{
				list.push( parseInt( key, 10 ));
			}
		}
	}

	if( list.length <= 0 )
		return -1;

	var idx = Math.floor( Math.random() * list.length );
	return list[idx];
}

function DawnsGetTrackList( box )
{
	var tag = box.GetTag( "dawns_tracks" );
	if( tag === null || tag === 0 || tag === "" )
		return [];

	var parts = ("" + tag).split( "," );
	var result = [];

	for( var i = 0; i < parts.length; i++ )
	{
		var value = parseInt( parts[i], 10 );
		if( !isNaN( value ) && value >= 0 )
			result.push( value );
	}

	return result;
}

function DawnsSetTrackList( musicbox, tracks )
{
	var serial = tracks.join( "," );
	musicbox.SetTag( "dawns_tracks", serial );
	musicbox.Refresh();
}

function DawnsTrackExists( tracks, trackID )
{
	return tracks.indexOf( trackID ) !== -1;
}

/** @type { ( thingCreated: BaseObject, thingType: 0 | 1 ) => void } */
function onCreateDFN( objMade, objType  )
{
	if( !ValidateObject( objMade ))
		return;

	var isBox  = objMade.GetTag( "dawns_box" );
	var isGear = objMade.GetTag( "dawns_gear" );

	if( isBox === 1 )
	{
		var tracks = DawnsGetTrackList( objMade );
		if( tracks.length === 0 )
		{
			while( tracks.length < 4 )
			{
				var trackId = DawnsRandomTrack( DawnsMusicRarity.Common );
				if( trackId >= 0 && !DawnsTrackExists( tracks, trackId ))
					tracks.push( trackId );
			}
			DawnsSetTrackList( objMade, tracks );
		}
	}
	else if( isGear === 1 )
	{
		var musicID = objMade.GetTag( "dawns_music" );
		if( musicID < 0 )
		{
			var rarityTag = objMade.GetTag( "dawns_rarity" );
			var rarity = DawnsMusicRarity.Common;

			if( rarityTag === DawnsMusicRarity.Uncommon )
			{
				rarity = DawnsMusicRarity.Uncommon;
			}
			else if( rarityTag === DawnsMusicRarity.Rare )
			{
				rarity = DawnsMusicRarity.Rare;
			}

			var track = DawnsRandomTrack( rarity );
			if( track >= 0 )
				objMade.SetTag( "dawns_music", track );
		}
	}
}

/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ))
		return false;

	var pSocket = pUser.socket;
	if( pSocket == null)
		return false;

	var isBox  = iUsed.GetTag( "dawns_box" );
	var isGear = iUsed.GetTag( "dawns_gear" );

	if( isBox === 1 )
	{
		if( !pUser.InRange( iUsed, 2 ))
		{
			pSocket.SysMessage( GetDictionaryEntry( 2500, pSocket.language )); // You are too far away to reach that.
			return false;
		}

		var itemOwner = GetPackOwner( iUsed, 0 );
		var isInPack = ( itemOwner && itemOwner.serial === pUser.serial );
		var isLockedDown = ( iUsed.movable === 2 || iUsed.movable === 3 );

		if( !isInPack && !isLockedDown )
		{
			pSocket.SysMessage( GetDictionaryEntry( 6540, pSocket.language )); // You must have the item in your backpack or locked down in order to use it.
			return false;
		}

		DawnsOpenBoxGump( pUser, iUsed, 0 );
		return false;
	}

	if( isGear === 1 )
	{
		pUser.SetTempTag( "dawns_pendingGear", iUsed.serial );

		var msg = GetDictionaryEntry( 6541, pSocket.language ); // Target a Dawns Music Box to add this song.
		pSocket.CustomTarget( 1, msg );

		return false;
	}

	return false;
}

/** @type { ( myObj: BaseObject, pSocket: Socket ) => string } */
function onTooltip( myObj, pSocket )
{
	if( !ValidateObject( myObj ))
		return "";

	var isBox  = myObj.GetTag( "dawns_box" );
	var isGear = myObj.GetTag( "dawns_gear" );

	if( isBox === 1 )
	{
		var tracks = DawnsGetTrackList( myObj );
		var common = 0;
		var uncommon = 0;
		var rare = 0;

		for( var i = 0; i < tracks.length; i++ )
		{
			var info = DawnsMusicInfo[ tracks[i] ];
			if( info )
			{
				if( info.rarity === DawnsMusicRarity.Common )
					common++;
				else if( info.rarity === DawnsMusicRarity.Uncommon )
					uncommon++;
				else if( info.rarity === DawnsMusicRarity.Rare )
					rare++;
			}
		}

		var parts = [];
		//parts.push( "Dawns Music Box" );
		if( common > 0 )
			parts.push( common + " Common tracks" );
		if( uncommon > 0 )
			parts.push( uncommon + " Uncommon tracks" );
		if( rare > 0 )
			parts.push( rare + " Rare tracks" );

		return parts.join( "\n" );
	}
	else if( isGear === 1 )
	{
		var musicID = myObj.GetTag( "dawns_music" );
		var info2 = DawnsMusicInfo[ musicID ];
		var line = "Gear for Dawns Music Box";

		if( info2 )
		{
			if( info2.rarity === DawnsMusicRarity.Common )
				line += " (Common)";
			else if( info2.rarity === DawnsMusicRarity.Uncommon )
				line += " (Uncommon)";
			else if( info2.rarity === DawnsMusicRarity.Rare )
				line += " (Rare)";

			line += "\n" + info2.name;
		}

		return line;
	}

	return "";
}

function DawnsOpenBoxGump( pUser, iBox, page )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iBox ))
		return;

	var socket = pUser.socket;
	if( socket == null )
		return;

	var tracks = DawnsGetTrackList( iBox );
	var total = tracks.length;

	if( page === undefined || page === null )
		page = 0;

	var maxPage = 0;
	if( total > 0 )
		maxPage = Math.floor(( total - 1 ) / 10 );

	if( page < 0 )
		page = 0;
	if( page > maxPage )
		page = maxPage;

	pUser.SetTempTag( "dawns_box_serial", iBox.serial );
	pUser.SetTempTag( "dawns_box_tracks", iBox.GetTag( "dawns_tracks" ));
	pUser.SetTempTag( "dawns_page", page );

	var g = new Gump;

	g.AddPage( 0 );
	g.AddBackground( 0, 0, 273, 324, 0x13BE );
	g.AddTiledGump( 10, 10, 253, 20, 0xA40 );
	g.AddTiledGump( 10, 40, 253, 244, 0xA40 );
	g.AddTiledGump( 10, 294, 253, 20, 0xA40 );
	g.AddButton( 10, 294, 0xFB1, 1, 0, 0 );
	g.AddXMFHTMLGumpColor(45, 296, 450, 20, 1060051, false, false, 0x7FFF);
	g.AddXMFHTMLGumpColor(14, 12, 273, 20, 1075130, false, false, 0x7FFF);
	g.AddPage( page + 1 );

	var startIndex = page * 10;
	var y = 49;
	var shown = 0;

	// Add up to 10 tracks for this page
	for( var i = startIndex; i < total && shown < 10; i++ )
	{
		var trackID = tracks[i];
		var info = DawnsMusicInfo[ trackID ];
		var fallback = "Track " + trackID;

		if( info )
		{
			var buttonID = 100 + shown;

			g.AddButton( 19, y, 0x845, 1, 0, buttonID );

			if( info && info.cliloc )
			{
				g.AddXMFHTMLGumpColor( 44, y - 2, 213, 20, info.cliloc, false, false, 0x7FFF );
			}
			else
			{
				g.AddText( 44, y - 2, 0x7FFF, info ? info.name : fallback );
			}

			y += 24;
			shown++;
		}
	}

	if( page < maxPage )
	{
		g.AddButton( 228, 294, 0xFA5, 1, 0, 2001 );
	}
	if( page > 0 )
	{
		g.AddButton( 193, 294, 0xFAE, 1, 0, 2000 );
	}

	g.AddButton( 19, y, 0x845, 1, 0, 1 );
	g.AddXMFHTMLGumpColor( 44, y - 2, 213, 20, 1075207, false, false, 0x7FFF ); // Stop Song

	g.Send( socket );
	g.Free();
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ))
		return;

	if( pButton === 0 )
	{
		return;
	}

	var currentPage = pUser.GetTempTag( "dawns_page" );
	var boxSerial  = pUser.GetTempTag( "dawns_box_serial" );

	function reopenBoxGump()
	{
		if( !boxSerial )
			return;

		var box = CalcItemFromSer( boxSerial );
		if( ValidateObject( box ))
			DawnsOpenBoxGump( pUser, box, currentPage );
	}

	if( pButton === 1 )
	{
		DawnsStopMusic( pUser );
		reopenBoxGump();
		return;
	}

	if( pButton === 2000 || pButton === 2001 )
	{
		if( boxSerial )
		{
			var box = CalcItemFromSer( boxSerial );
			if( ValidateObject( box ))
			{
				var newPage = currentPage + ( pButton === 2001 ? 1 : -1 );
				DawnsOpenBoxGump( pUser, box, newPage );
			}
		}
		return;
	}

	if( pButton >= 100 && pButton < 110 )
	{
		var indexOnPage = pButton - 100;
		var perPage = 10;

		var tracksStr = pUser.GetTempTag( "dawns_box_tracks" ) || "";
		if( tracksStr === "" )
			return;

		var parts = ("" + tracksStr).split( "," );
		var globalIndex = currentPage * perPage + indexOnPage;

		if( globalIndex >= 0 && globalIndex < parts.length )
		{
			var musicID = parseInt( parts[globalIndex], 10 );
			if( !isNaN( musicID ) && musicID >= 0 )
			{
				DawnsPlayTrack( pUser, musicID );
				reopenBoxGump();
			}
		}
	}
}

function DawnsPlayTrack( pUser, trackID )
{
	var socket = pUser.socket;
	if( socket == null)
		return;

	var allowed = pUser.GetTempTag( "dawns_box_tracks" );
	if( allowed && allowed !== "" )
	{
		var parts = ("" + allowed).split( "," );
		var ok = false;
		for( var i = 0; i < parts.length; i++ )
		{
			var v = parseInt( parts[i], 10 );
			if( v === trackID )
			{
				ok = true;
				break;
			}
		}
		if( !ok )
			return;
	}

	var boxSerial = pUser.GetTempTag( "dawns_box_serial" );
	if( boxSerial )
	{
		var box = CalcItemFromSer( boxSerial );
		if( ValidateObject( box ) && (( box.GetTag( "dawns_box" )) === 1 ))
		{
			var baseID = box.GetTag( "dawns_baseID" );
			if( baseID === 0 )
			{
				baseID = box.id;
				box.SetTag( "dawns_baseID", baseID );
			}

			box.KillJSTimer( 1, 5070 );

			box.id = baseID;
			box.SetTag( "dawns_animCount", 0 );
			box.Refresh();

			box.StartTimer( 500, 1, true );
		}
	}

	DawnsSendPlayMusicPacket( socket, trackID );
}

function DawnsStopMusic( pUser )
{
	var socket = pUser.socket;
	if( socket == null )
		return;

	var boxSerial = pUser.GetTempTag( "dawns_box_serial" );
	if( boxSerial )
	{
		var box = CalcItemFromSer( boxSerial );
		if( ValidateObject( box ) && (( box.GetTag( "dawns_box" )) === 1) )
		{
			box.KillJSTimer( 1, 5070);

			var baseID = box.GetTag( "dawns_baseID" );
			if( baseID > 0 )
				box.id = baseID;

			box.SetTag( "dawns_animCount", 0 );
			box.Refresh();
		}
	}

	DawnsSendStopMusicPacket( socket );
}

/** @type { ( pSocket: Socket, trackID: number ) => void } */
function DawnsSendPlayMusicPacket( pSocket, trackID )
{
	if( pSocket == null )
		return;

	var myPacket = new Packet();
	myPacket.ReserveSize( 3 );
	myPacket.WriteByte( 0, 0x6D );
	myPacket.WriteShort( 1, trackID );
	pSocket.Send( myPacket );
	myPacket.Free();
}

/** @type { ( pSocket: Socket ) => void } */
function DawnsSendStopMusicPacket( pSocket )
{
	if( pSocket == null )
		return;
	
	var myPacket = new Packet();
	myPacket.ReserveSize( 3 );
	myPacket.WriteByte( 0, 0x6D );
	myPacket.WriteShort( 1, 0x1FFF );
	pSocket.Send( myPacket );
	myPacket.Free();
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( timerObj, timerID )
{
	if( timerID !== 1 )
		return;

	if( !ValidateObject( timerObj ))
		return;

	if(( timerObj.GetTag( "dawns_box" )) !== 1 )
		return;

	var count  = timerObj.GetTag( "dawns_animCount" );
	var baseID = timerObj.GetTag( "dawns_baseID" );

	if( baseID === 0 )
		baseID = timerObj.id;

	// advance animation step
	count++;

	if( count >= 4 )
	{
		// end of cycle: restore base frame and stop timer
		timerObj.id = baseID;
		timerObj.SetTag( "dawns_animCount", 0 );
		timerObj.KillJSTimer( 1, 5070 );
	}
	else
	{
		// always animate relative to baseID so it never drifts
		timerObj.id = baseID + count;
		timerObj.SetTag( "dawns_animCount", count );
	}

	timerObj.Refresh();
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback1( socket, targetObj )
{
	if( socket == null )
		return;

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	if( socket.GetWord( 1 ) !== 0 )
	{
		return;
	}

	if( !ValidateObject( targetObj ) || !targetObj.isItem )
	{
		socket.SysMessage( GetDictionaryEntry( 6542, socket.language )); // You must target a Dawns Music Box.
		return;
	}

	// Must be a Dawns Music Box
	var isBox = targetObj.GetTag( "dawns_box" );
	if( isBox !== 1 )
	{
		socket.SysMessage( GetDictionaryEntry( 6543, socket.language )); // That is not a Dawns Music Box.
		return;
	}

	// Find the gear we were using
	var gearSer = pUser.GetTempTag( "dawns_pendingGear" );
	if( gearSer == null || gearSer === 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 6544, socket.language )); // No music gear is pending.
		return;
	}

	var gear = CalcItemFromSer( gearSer );
	if( !ValidateObject( gear ) || ( gear.GetTag( "dawns_gear" )) !== 1 )
	{
		socket.SysMessage( GetDictionaryEntry( 6545, socket.language )); // The music gear could not be found.
		return;
	}

	var musicID = gear.GetTag( "dawns_music" );
	if( musicID < 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 6546, socket.language )); // This gear has no song assigned.
		return;
	}

	var tracks = DawnsGetTrackList( targetObj );
	if( DawnsTrackExists( tracks, musicID ))
	{
		socket.SysMessage( GetDictionaryEntry( 6547, socket.language )); // This song track is already in the music box.
		return;
	}

	tracks.push( musicID );
	DawnsSetTrackList( targetObj, tracks );
	gear.Delete();

	socket.SysMessage( GetDictionaryEntry( 6548, socket.language )); // This song has been added to the music box.

	pUser.SetTempTag( "dawns_pendingGear", null );
}
