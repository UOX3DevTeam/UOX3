/// <reference path="../../definitions.d.ts" />
// @ts-check
/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( pSocket && iUsed && iUsed.isItem )
	{
		//Check to see if it's locked down
		if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			pSocket.SysMessage( GetDictionaryEntry( 774, pSocket.language )); // That is locked down and you cannot use it
			return false;
		}
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pUser.SysMessage( "This must be in your backpack to use it." );
			return false;
		}
		else if( pUser.isonhorse )
		{
			pSocket.SysMessage( GetDictionaryEntry( 2713, pSocket.language )); // You cannot use this while riding a mount.
			return false;
		}
		else
		{
			pSocket.CustomTarget( 0, "What do you wish to carve?" );
		}
	}
	return false;
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback0( socket, myTarget )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;
	// An array of item options
	var itemPumpkins = ["tallwitch", "talluo", "tallankh", "tallspider", "tallbat", "tallskull", "talldemon", "tallspirit", "tallscalele"];
	var itemGord = ["gordtallwitch", "gordtallzombie"];
	var itemWarty = ["pumpkinwartyserpent", "pumpkinwartyraven"];
	var itemWhite = ["pumpkinwhitevoidcat", "pumpkinwhitetsillyface"];
	var itemStriped = ["pumpkinstrippedvictor", "pumpkinstrippedhappyface"];

	// Generate a random index to select an item
	var randomIndexP = Math.floor( Math.random() * itemPumpkins.length );
	var randomIndexG = Math.floor( Math.random() * itemGord.length );
	var randomIndexWarty = Math.floor( Math.random() * itemWarty.length );
	var randomIndexWhite = Math.floor( Math.random() * itemWhite.length );
	var randomIndexStriped = Math.floor( Math.random() * itemStriped.length );

	// Use the randomly selected item
	var randomPumpkin = itemPumpkins[randomIndexP];
	var randomGord = itemGord[randomIndexG];
	var randomWarty = itemWarty[randomIndexWarty];
	var randomWhite = itemWhite[randomIndexWhite];
	var randomStriped = itemStriped[randomIndexStriped];

	if( myTarget == null )
		return;

	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if( myTarget.sectionID == "CarvablePumpkin" )
	{
		pUser.SoundEffect( 0x249, true );
		var carvedpumpkin = CreateDFNItem( pUser.socket, pUser, randomPumpkin, 1, "ITEM", true );
		carvedpumpkin.name = "Carved By " + pUser.name;
		pUser.EmoteMessage( "*You carefully carve the pumpkin*" );
		pUser.UseResource( 1, 0x992F )
	}
	else if( myTarget.sectionID == "CarvableGord" )
	{
		pUser.SoundEffect( 0x249, true );
		var carvedpumpkin = CreateDFNItem( pUser.socket, pUser, randomGord, 1, "ITEM", true );
		carvedpumpkin.name = "Carved By " + pUser.name;
		pUser.EmoteMessage( "*You carefully carve the gord*" );
		pUser.UseResource( 1, 0x9D22 )
	}
	else if( myTarget.sectionID == "pumpkinwartyraw" )
	{
		pUser.SoundEffect( 0x249, true );
		var carvedpumpkin = CreateDFNItem( pUser.socket, pUser, randomWarty, 1, "ITEM", true );
		carvedpumpkin.name = "Carved By " + pUser.name;
		pUser.EmoteMessage( "*You carefully carve the gord*" );
		pUser.UseResource( 1, 0x9D22 )
	}
	else if( myTarget.sectionID == "pumpkinwhiteraw" )
	{
		pUser.SoundEffect( 0x249, true );
		var carvedpumpkin = CreateDFNItem( pUser.socket, pUser, randomWhite, 1, "ITEM", true );
		carvedpumpkin.name = "Carved By " + pUser.name;
		pUser.EmoteMessage( "*You carefully carve the gord*" );
		pUser.UseResource( 1, 0x9D22 )
	}
	else if( myTarget.sectionID == "pumpkinstripedraw" )
	{
		pUser.SoundEffect( 0x249, true );
		var carvedpumpkin = CreateDFNItem( pUser.socket, pUser, randomStriped, 1, "ITEM", true );
		carvedpumpkin.name = "Carved By " + pUser.name;
		pUser.EmoteMessage( "*You carefully carve the gord*" );
		pUser.UseResource( 1, 0x9D22 )
	}
	else
	{
		pUser.SysMessage( "That is not suitable for carving." );
		return;
	}
}
