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
			pUser.SysMessage( "You do not understand how to use this." ); // You do not understand how to use this.
			return;
		}
		else
		{
			var targMsg = "Target the corpse to make a trophy out of."; // Target the corpse to make a trophy out of.
			socket.CustomTarget( 1, targMsg );
		}
	}
	return false;
}

function onCallback1(socket, ourObj)
{
	var tileID = 0;
	var iUsed = socket.tempObj;

	tileID = ourObj.id;
	if( tileID != 0x2006 )
	{
		socket.SysMessage( "That is not a corpse!" ); // That is not a corpse!
		return;
	}
	else if( ourObj.GetTag( "VisitedByTaxidermist" ) == true)
	{
		socket.SysMessage( "That corpse seems to have been visited by a taxidermist already." ); // That corpse seems to have been visited by a taxidermist already.
		return;
	}
	else 
	{
		switch( ourObj.name )
		{
			case "corpse of a brown bear"://amount 167
				CreateDFNItem( socket, socket.currentChar, "brownbeartrophydeed", 1, "ITEM", true );
				ourObj.SetTag( "VisitedByTaxidermist", true );
				iUsed.Delete();
				break;
			case "corpse of a great hart"://amount 234
				CreateDFNItem( socket, socket.currentChar, "stagtrophydeed", 1, "ITEM", true );
				ourObj.SetTag( "VisitedByTaxidermist", true );
				iUsed.Delete();
				break;
			case "corpse of a gorilla"://amount 29
				CreateDFNItem( socket, socket.currentChar, "gorillatrophydeed", 1, "ITEM", true );
				ourObj.SetTag( "VisitedByTaxidermist", true );
				iUsed.Delete();
				break;
			case "corpse of a orc"://amount 17
				CreateDFNItem( socket, socket.currentChar, "orctrophydeed", 1, "ITEM", true );
				ourObj.SetTag( "VisitedByTaxidermist", true );
				iUsed.Delete();
				break;
			case "corpse of a polar bear"://amount 213
				CreateDFNItem( socket, socket.currentChar, "polarbeartrophydeed", 1, "ITEM", true );
				ourObj.SetTag( "VisitedByTaxidermist", true );
				iUsed.Delete();
				break;
			case "corpse of a troll"://amount 54
				CreateDFNItem( socket, socket.currentChar, "trolltrophydeed", 1, "ITEM", true );
				ourObj.SetTag( "VisitedByTaxidermist", true );
				iUsed.Delete();
				break;
			default:
				socket.SysMessage( "That does not look like something you want hanging on a wall." );
				break;
		}
	}
}