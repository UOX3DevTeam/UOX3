const enableUOX3Craft = 0;            // Disable or enable to use old uox3 menus.
const blacksmithID = 4023;			// Use this to tell the gump what script to close.
const Carpentry = 4025;
const Alchemy = 4028;
const Fletching = 4029;
const Tailoring = 4030;
const Tinkering = 4032;
const Cooking = 4034;

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	var gumpID = Carpentry + 0xffff;
	var gumpID2 = Alchemy + 0xffff;
	var gumpID3 = Fletching + 0xffff;
	var gumpID4 = Tailoring + 0xffff;
	var gumpID5 = blacksmithID + 0xffff;
	var gumpID6 = Cooking + 0xffff;
	var gumpID7 = Tinkering + 0xffff;

	if( socket && ValidateObject( iUsed ) && iUsed.isItem )
	{
		if( GetServerSetting( "ToolUseLimit" ) && iUsed.usesLeft == 0 )
		{
			// Tool has no uses left
			socket.SysMessage( GetDictionaryEntry( 9262, socket.language )); // This has no more charges.
			return false;
		}

		if( !pUser.InRange( iUsed, 3 ))
		{
			socket.SysMessage( GetDictionaryEntry( 461, socket.language )); // You are too far away.
			return false;
		}

		if( iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 6031, socket.language )); // Locked down resources cannot be used!
			return false;
		}

		var iPackOwner = GetPackOwner( iUsed, 0 );
		if( ValidateObject( iPackOwner )) // Is the item in a backpack?
		{
			if( iPackOwner.serial != pUser.serial ) //And if so does the pack belong to the user?
			{
				socket.SysMessage( GetDictionaryEntry( 6032, socket.language )); // That resource is in someone else's backpack!
				return false;
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 6022, socket.language )); // This has to be in your backpack before you can use it.
			return false;
		}

		socket.tempObj = iUsed;
		var tempPage = pUser.GetTempTag( "page" );
		if( iUsed.id >= 0x1026 && iUsed.id <= 0x1029 || iUsed.id >= 0x102C && iUsed.id <= 0x102F || iUsed.id >= 0x1030 && iUsed.id <= 0x1035 || iUsed.id >= 0x10E4 && iUsed.id <= 0x10E6 )
		{
			// Carpentry
			if( enableUOX3Craft == 1 )
			{
				TriggerEvent( 4006, "onUseChecked", pUser, iUsed );
				return;
			}
			socket.CloseGump( gumpID, 0 );
			pUser.SetTempTag( "CRAFT", 1 )
			switch( tempPage )
			{
				case 1: // Page 1
				case 2: // Page 2
				case 3: // Page 3
				case 4: // Page 4
				case 5: // Page 5
				case 6: // Page 6
				case 7: // Page 7
				case 8: // Page 8
				case 9: // Page 9
				case 10: // Page 10
					TriggerEvent( Carpentry, "PageX", socket, pUser, tempPage );
					break;
				default: TriggerEvent( Carpentry, "PageX", socket, pUser, 1 );
					break;
			}
		}
		else if( iUsed.id == 0x0E9B ) // mortar and pestle
		{
			// Alchemy
			if( enableUOX3Craft == 1 )
			{
				TriggerEvent( 4007, "onUseChecked", pUser, iUsed );
				return;
			}
			socket.CloseGump( gumpID2, 0 );
			pUser.SetTempTag( "CRAFT", 2 )
			switch( tempPage )
			{
				case 1: // Page 1
				case 2: // Page 2
				case 3: // Page 3
				case 4: // Page 4
					TriggerEvent( Alchemy, "PageX", socket, pUser, tempPage );
					break;
				default: TriggerEvent( Alchemy, "PageX", socket, pUser, 1 );
					break;
			}
		}
		else if( iUsed.id == 0x1022 || iUsed.id == 0x1BD1 || iUsed.id == 0x1BD4 )
		{
			// Bowcraft/Fletching
			if( enableUOX3Craft == 1 )
			{
				TriggerEvent( 4005, "onUseChecked", pUser, iUsed );
				return;
			}
			socket.CloseGump( gumpID3, 0 );
			pUser.SetTempTag( "CRAFT", 3 )
			switch( tempPage )
			{
				case 1: // Page 1
				case 2: // Page 2
				case 3: // Page 3
					TriggerEvent( Fletching, "PageX", socket, pUser, tempPage );
					break;
				default: TriggerEvent( Fletching, "PageX", socket, pUser, 1 );
					break;
			}
		}
		else if( iUsed.id == 0x0F9D ) // Sewing Kit
		{
			// Tailoring
			if( enableUOX3Craft == 1 )
			{
				TriggerEvent( 4004, "onUseChecked", pUser, iUsed );
				return;
			}
			socket.CloseGump( gumpID4, 0 );
			pUser.SetTempTag( "CRAFT", 4 )
			switch( tempPage )
			{
				case 1: // Page 1
				case 2: // Page 2
				case 3: // Page 3
				case 4: // Page 4
				case 5: // Page 5
				case 6: // Page 6
				case 7: // Page 7
				case 8: // Page 8
					TriggerEvent( Tailoring, "PageX", socket, pUser, tempPage );
					break;
				default: TriggerEvent( Tailoring, "PageX", socket, pUser, 1 );
					break;
			}
		}
		else if( iUsed.id == 0x0FBB || iUsed.id == 0x0FBC || iUsed.id == 0x13E3 || iUsed.id == 0x13E4 )
		{
			// Blacksmithing
			if( enableUOX3Craft == 1 )
			{
				return true;
			}
			socket.CloseGump( gumpID5, 0 );
			pUser.SetTempTag( "CRAFT", 5 )
			switch( tempPage )
			{
				case 1: // Page 1
				case 2: // Page 2
				case 3: // Page 3
				case 4: // Page 4
				case 5: // Page 5
				case 6: // Page 6
				case 7: // Page 7
					TriggerEvent( blacksmithID, "PageX", socket, pUser, tempPage );
					break;
				case 8:
					TriggerEvent( blacksmithID, "Page8", socket, pUser );
					break;
				default: TriggerEvent( blacksmithID, "PageX", socket, pUser, 1 );
					break;
			}
		}
		else if( iUsed.id == 0x1043 || iUsed.id == 0x097f || iUsed.id == 0x09e2 || iUsed.id == 0x103e )
		{
			// Cooking
			if( enableUOX3Craft == 1 )
			{
				//socket.SysMessage( "Old-school crafting gumps have not been implemented for Cooking. Use raw food with heat sources to cook!" );
				TriggerEvent( 104, "onUseChecked", pUser, iUsed );
				return;
			}
			socket.CloseGump( gumpID6, 0 );
			pUser.SetTempTag( "CRAFT", 6 )
			switch( tempPage )
			{
				case 1: // Page 1
				case 2: // Page 2
				case 3: // Page 3
				case 4: // Page 4
					TriggerEvent( Cooking, "PageX", socket, pUser, tempPage );
					break;
				default: TriggerEvent( Cooking, "PageX", socket, pUser, 1 );
					break;
			}
		}
		else if( iUsed.id == 0x1eb8 || iUsed.id == 0x1eb9 || iUsed.id == 0x1eba || iUsed.id == 0x1ebb || iUsed.id == 0x1ebc ) // Tinker's tools
		{
			// Tinkering
			if( enableUOX3Craft == 1 )
			{
				TriggerEvent( 4003, "onUseChecked", pUser, iUsed );
				return;
			}
			socket.CloseGump( gumpID7, 0 );
			pUser.SetTempTag( "CRAFT", 7 )
			switch( tempPage )
			{
				case 1: // Page 1
				case 2: // Page 2
				case 3: // Page 3
				case 4: // Page 4
				case 5: // Page 5
				case 6: // Page 6
				case 7: // Page 7
				case 8: // Page 8
				case 9: // Page 9
					TriggerEvent( Tinkering, "PageX", socket, pUser, tempPage );
					break;
				default: TriggerEvent( Tinkering, "PageX", socket, pUser, 1 );
					break;
			}
		}
	}
	return false;
}