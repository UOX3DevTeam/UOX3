function onUseChecked(pUser, iUsed)
{
	var socket = pUser.socket;
	socket.tempObj = iUsed;
	if( socket && iUsed && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pUser.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
			return;
		}
		else if( pUser.skills.carpentry < 900 )
		{
			pUser.SysMessage( GetDictionaryEntry( 2863, socket.language )); // You do not understand how to use this.
			return;
		}
		else
		{
			var targMsg = GetDictionaryEntry( 2864, socket.language ); // Target the corpse to make a trophy out of.
			socket.CustomTarget( 1, targMsg );
		}
	}
	return false;
}

function onCallback1(socket, myTarget)
{
	var tileID = 0;
	var pUser = socket.currentChar;
	var iUsed = socket.tempObj;
	var woodID = 0x1bd7;
	var resourceCount = pUser.ResourceCount( woodID );
	var amountNeeded = 10;
	tileID = myTarget.id;

	if( tileID != 0x2006 )
	{
		socket.SysMessage( GetDictionaryEntry( 749, socket.language )); // That is not a corpse!
		return;
	}
	else if( myTarget.GetTag( "VisitedByTaxidermist" ) == true )
	{
		socket.SysMessage( GetDictionaryEntry( 2865, socket.language )); // That corpse seems to have been visited by a taxidermist already.
		return;
	}
	else 
	{
		if( resourceCount >= amountNeeded )
		{
			switch( myTarget.sectionID ) 
			{
				case "brownbear":
					CreateDFNItem( socket, socket.currentChar, "brownbeartrophydeed", 1, "ITEM", true );
					socket.SysMessage( GetDictionaryEntry( 2866, socket.language )); // You review the corpse and find it worthy of a trophy.
					socket.SysMessage( GetDictionaryEntry( 2867, socket.language )); // You use your kit up making the trophy.
					myTarget.SetTag( "VisitedByTaxidermist", true );
					pUser.UseResource( amountNeeded,  woodID );
					iUsed.Delete();
					break;
				case "hart":
					CreateDFNItem( socket, socket.currentChar, "stagtrophydeed", 1, "ITEM", true );
					socket.SysMessage( GetDictionaryEntry( 2866, socket.language )); // You review the corpse and find it worthy of a trophy.
					socket.SysMessage( GetDictionaryEntry( 2867, socket.language )); // You use your kit up making the trophy.
					myTarget.SetTag( "VisitedByTaxidermist", true );
					pUser.UseResource( amountNeeded,  woodID );
					iUsed.Delete();
					break;
				case "gorilla":
					CreateDFNItem( socket, socket.currentChar, "gorillatrophydeed", 1, "ITEM", true );
					socket.SysMessage( GetDictionaryEntry( 2866, socket.language )); // You review the corpse and find it worthy of a trophy.
					socket.SysMessage( GetDictionaryEntry( 2867, socket.language )); // You use your kit up making the trophy.
					myTarget.SetTag( "VisitedByTaxidermist", true );
					pUser.UseResource( amountNeeded,  woodID );
					iUsed.Delete();
					break;
				case "orc":
				case "cluborc":
					CreateDFNItem( socket, socket.currentChar, "orctrophydeed", 1, "ITEM", true );
					socket.SysMessage( GetDictionaryEntry( 2866, socket.language )); // You review the corpse and find it worthy of a trophy.
					socket.SysMessage( GetDictionaryEntry( 2867, socket.language )); // You use your kit up making the trophy.
					myTarget.SetTag( "VisitedByTaxidermist", true );
					pUser.UseResource( amountNeeded,  woodID );
					iUsed.Delete();
					break;
				case "polarbear":
					CreateDFNItem( socket, socket.currentChar, "polarbeartrophydeed", 1, "ITEM", true );
					socket.SysMessage( GetDictionaryEntry( 2866, socket.language )); // You review the corpse and find it worthy of a trophy.
					socket.SysMessage( GetDictionaryEntry( 2867, socket.language )); // You use your kit up making the trophy.
					myTarget.SetTag( "VisitedByTaxidermist", true );
					pUser.UseResource( amountNeeded,  woodID );
					iUsed.Delete();
					break;
				case "troll":
					CreateDFNItem( socket, socket.currentChar, "trolltrophydeed", 1, "ITEM", true );
					socket.SysMessage( GetDictionaryEntry( 2866, socket.language )); // You review the corpse and find it worthy of a trophy.
					socket.SysMessage( GetDictionaryEntry( 2867, socket.language )); // You use your kit up making the trophy.
					myTarget.SetTag( "VisitedByTaxidermist", true );
					pUser.UseResource( amountNeeded,  woodID );
					iUsed.Delete(); 
					break;
				default:
					socket.SysMessage( GetDictionaryEntry( 2868, socket.language )); // That does not look like something you want hanging on a wall.
					break;
			}
		}
		else 
		{
			socket.SysMessage( GetDictionaryEntry( 2869, socket.language )); // You do not have enough boards.
			return;
		}
	}
}