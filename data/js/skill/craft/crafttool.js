var enableHPLOSS = 0;				// Disable or enable hp on tools to break.
var enableUOX3Craft = 0;            // Disable or enable to use old uox3 mneus.
var enableBreak = 0;				// Disable or enable Breaking of tools.
var blacksmithID = 4023;			// Use this to tell the gump what script to close.
var bronzeID = 4015;
var copperID = 4016;
var agapiteID = 4017;
var dullcopperID = 4018;
var goldID = 4019;
var shadowironID = 4020;
var valoriteID = 4021;
var veriteID = 4022;
var Carptenry = 4025;
var Alchemy = 4028;
var Fletching = 4029;
var Tailoring = 4030;

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	var gumpID = Carptenry + 0xffff;
	var gumpID2 = Alchemy + 0xffff;
	var gumpID3 = Fletching + 0xffff;
	var gumpID4 = Tailoring + 0xffff;
	var gumpID5 = blacksmithID + 0xffff;
	var gumpID6 = bronzeID + 0xffff;
	var gumpID7 = copperID + 0xffff;
	var gumpID8 = agapiteID + 0xffff;
	var gumpID9 = dullcopperID + 0xffff;
	var gumpID10 = goldID + 0xffff;
	var gumpID11 = shadowironID + 0xffff;
	var gumpID12 = valoriteID + 0xffff;
	var gumpID13 = veriteID + 0xffff;
	if ( socket && iUsed && iUsed.isItem )
	{
		if ( enableHPLOSS == 1 )
		{
			iUsed.health -= 1;
		}

		if ( enableBreak == 1 && iUsed.health == 0 )
		{
			iUsed.Delete();
			pUser.SysMessage( GetDictionaryEntry( 10202, socket.language ) );
			return false;
		}

		if ( !pUser.InRange( iUsed, 3 ) )
		{
			pUser.SysMessage( GetDictionaryEntry( 461, socket.language ) ); // You are too far away.
			return false;
		}

		if ( iUsed.movable == 3 )
		{
			pUser.SysMessage( GetDictionaryEntry( 6031, socket.language ) ); // Locked down resources cannot be used!
			return false;
		}

		var iPackOwner = GetPackOwner( iUsed, 0 );
		if ( ValidateObject( iPackOwner ) ) // Is the target item in a backpack?
		{
			if ( iPackOwner.serial != pUser.serial ) //And if so does the pack belong to the user?
			{
				pUser.SysMessage( GetDictionaryEntry( 6032, socket.language ) ); // That resource is in someone else's backpack!
				return false;
			}
		}
		else
		{
			pUser.SysMessage( GetDictionaryEntry( 6022, socket.language ) ); // This has to be in your backpack before you can use it.
			return false;
		}

		socket.tempObj = iUsed;
		if ( iUsed.id >= 0x1026 && iUsed.id <= 0x1029 || iUsed.id >= 0x102C && iUsed.id <= 0x102F || iUsed.id >= 0x1030 && iUsed.id <= 0x1035 || iUsed.id >= 0x10E4 && iUsed.id <= 0x10E6 )
		{
			if ( enableUOX3Craft == 1 )
			{
				TriggerEvent( 4006, "onUseChecked", pUser, iUsed );
				return;
			}
			socket.CloseGump( gumpID, 0 );
			pUser.SetTag( "CRAFT", 1 )
			switch ( pUser.GetTag( "page" ) )
			{
				case 1:
					TriggerEvent( Carpentry, "page1", socket, pUser );
					break;
				case 2:
					TriggerEvent( Carpentry, "page2", socket, pUser );
					break;
				case 3:
					TriggerEvent( Carpentry, "page3", socket, pUser );
					break;
				case 4:
					TriggerEvent( Carpentry, "page4", socket, pUser );
					break;
				case 5:
					TriggerEvent( Carpentry, "page5", socket, pUser );
					break;
				case 6:
					TriggerEvent( Carpentry, "page6", socket, pUser );
					break;
				case 7:
					TriggerEvent( Carpentry, "page7", socket, pUser );
					break;
				case 8:
					TriggerEvent( Carpentry, "page8", socket, pUser );
					break;
				case 9:
					TriggerEvent( Carpentry, "page9", socket, pUser );
					break;
				case 10:
					TriggerEvent( Carpentry, "page10", socket, pUser );
					break;
				default: TriggerEvent( Carpentry, "page1", socket, pUser );
					break;
			}
		}
		else if ( iUsed.id == 0x0E9B )
		{
			if ( enableUOX3Craft == 1 )
			{
				TriggerEvent( 4007, "onUseChecked", pUser, iUsed );
				return;
			}
			socket.CloseGump( gumpID2, 0 );
			pUser.SetTag( "CRAFT", 2 )
			switch ( pUser.GetTag( "page" ) )
			{
				case 1:
					TriggerEvent( Alchemy, "page1", socket, pUser );
					break;
				case 2:
					TriggerEvent( Alchemy, "page2", socket, pUser );
					break;
				case 3:
					TriggerEvent( Alchemy, "page3", socket, pUser );
					break;
				case 4:
					TriggerEvent( Alchemy, "page4", socket, pUser );
					break;
				default: TriggerEvent( Alchemy, "page1", socket, pUser );
					break;
			}
		}
		else if ( iUsed.id == 0x1022 || iUsed.id == 0x1BD1 || iUsed.id == 0x1BD4 )
		{
			if ( enableUOX3Craft == 1 )
			{
				TriggerEvent( 4005, "onUseChecked", pUser, iUsed );
				return;
			}
			socket.CloseGump( gumpID3, 0 );
			pUser.SetTag( "CRAFT", 3 )
			switch ( pUser.GetTag( "page" ) )
			{
				case 1:
					TriggerEvent( Fletching, "page1", socket, pUser );
					break;
				case 2:
					TriggerEvent( Fletching, "page2", socket, pUser );
					break;
				case 3:
					TriggerEvent( Fletching, "page3", socket, pUser );
					break;
				default: TriggerEvent( Fletching, "page1", socket, pUser );
					break;
			}
		}
		else if ( iUsed.id == 0x0F9D )
		{
			if ( enableUOX3Craft == 1 )
			{
				TriggerEvent( 4004, "onUseChecked", pUser, iUsed );
				return;
			}
			socket.CloseGump( gumpID4, 0 );
			pUser.SetTag( "CRAFT", 4 )
			switch ( pUser.GetTag( "page" ) )
			{
				case 1:
					TriggerEvent( Tailoring, "page1", socket, pUser );
					break;
				case 2:
					TriggerEvent( Tailoring, "page2", socket, pUser );
					break;
				case 3:
					TriggerEvent( Tailoring, "page3", socket, pUser );
					break;
				case 4:
					TriggerEvent( Tailoring, "page4", socket, pUser );
					break;
				case 5:
					TriggerEvent( Tailoring, "page5", socket, pUser );
					break;
				case 6:
					TriggerEvent( Tailoring, "page6", socket, pUser );
					break;
				case 7:
					TriggerEvent( Tailoring, "page7", socket, pUser );
					break;
				case 8:
					TriggerEvent( Tailoring, "page8", socket, pUser );
					break;
				default: TriggerEvent( Tailoring, "page1", socket, pUser );
					break;
			}
		}
		else if ( iUsed.id == 0x0FBB || iUsed.id == 0x0FBC || iUsed.id == 0x13E3 || iUsed.id == 0x13E4 )
		{
			if ( enableUOX3Craft == 1 )
			{
				return true;
			}
			socket.CloseGump( gumpID5, 0 );
			socket.CloseGump( gumpID6, 0 );
			socket.CloseGump( gumpID7, 0 );
			socket.CloseGump( gumpID8, 0 );
			socket.CloseGump( gumpID9, 0 );
			socket.CloseGump( gumpID10, 0 );
			socket.CloseGump( gumpID11, 0 );
			socket.CloseGump( gumpID12, 0 );
			socket.CloseGump( gumpID13, 0 );
			pUser.SetTag( "CRAFT", 5 )
			switch ( pUser.GetTag( "page" ) )
			{
				case 1:
					TriggerEvent( blacksmithID, "page1", socket, pUser );
					break;
				case 2:
					TriggerEvent( blacksmithID, "page2", socket, pUser );
					break;
				case 3:
					TriggerEvent( blacksmithID, "page3", socket, pUser );
					break;
				case 4:
					TriggerEvent( blacksmithID, "page4", socket, pUser );
					break;
				case 5:
					TriggerEvent( blacksmithID, "page5", socket, pUser );
					break;
				case 6:
					TriggerEvent( blacksmithID, "page6", socket, pUser );
					break;
				case 7:
					TriggerEvent( blacksmithID, "page7", socket, pUser );
					break;
				case 8:
					TriggerEvent( blacksmithID, "page8", socket, pUser );
					break;
				default: TriggerEvent( blacksmithID, "page1", socket, pUser );
					break;
			}
		}
	}
	return false;
}