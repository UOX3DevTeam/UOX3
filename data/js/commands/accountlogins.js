function CommandRegistration()
{
	RegisterCommand( "acctlogins", 2, true );
}

function command_ACCTLOGINS( socket, cmdString )
{
	if( cmdString )
	{
		var iSQL = "SELECT AccountLoginHistory.Username, AccountLoginHistory.Password, AccountLoginHistory.IPAddress4, AccountLoginHistory.WhenAttempted, AttemptInfo.Description FROM AccountLoginHistory INNER JOIN AttemptInfo ON AccountLoginHistory.AttemptStatus = AttemptInfo.AttemptStatus";
		iSQL	+= " WHERE AccountLoginHistory.Username = '" + cmdString + "' ORDER BY AccountLoginHistory.WhenAttempted DESC;";

		var idxStmt = ODBC.ExecuteQuery( iSQL );
		var gumpWidth	= 360;
		var gumpHeight	= 370;
		var sectTop	= 120;
		var itemHeight	= 60;
		if( ODBC.lastOK )
		{
			var idxStmtNum	= parseInt( idxStmt );
			var last10	= 0;
			var myChar	= socket.currentChar;
			var bDone	= false;
			var myGump	= new Gump; 

			myChar.SoundEffect( 0x58, false ); 

			myGump.AddPage( 0 );
			myGump.AddBackground( 20, 20, gumpWidth, gumpHeight, 0x23f0 ); // was 0x23f0

			myGump.AddButton( gumpWidth - 20, 20, 4017, 1, 0, 0 ); 
			myGump.AddGump( (gumpWidth/2)-50, 20, 0x0064 );
			myGump.AddText( (gumpWidth/2), 40, 0, "ACCOUNT" ); 
			myGump.AddText( (gumpWidth/2)-30, 60, 0, "'" + cmdString + "'" ); 

			while( ODBC.FetchRow( idxStmtNum ) && !bDone )
			{
				last10++;

				// 0	AccountLoginHistory.Username
				// 1	AccountLoginHistory.Password
				// 2	AccountLoginHistory.IPAddress4
				// 3	AccountLoginHistory.WhenAttempted
				// 4	AttemptInfo.Description

				if( last10 > 1 )
					myGump.AddPageButton( gumpWidth - 20, gumpHeight - 5, 4005, last10 );
				// add a background 

				myGump.AddPage( last10 );

				myGump.AddText( 30, 20, 0, "Page " + last10 ); 

				myGump.AddText( (gumpWidth/2)-20, sectTop, 0, "Password" ); 
				myGump.AddBackground( 30,  sectTop + 20, gumpWidth - 20,  itemHeight - 20, 3500 ); 

				myGump.AddText( (gumpWidth/2)-20, sectTop + itemHeight * 1, 0, "IP Addy" ); 
				myGump.AddBackground( 30, sectTop + itemHeight * 1 + 20, gumpWidth - 20, itemHeight - 20, 3500 ); 

				myGump.AddText( (gumpWidth/2)-20, sectTop + itemHeight * 2, 0, "When" ); 
				myGump.AddBackground( 30, sectTop + itemHeight * 2 + 20, gumpWidth - 20, itemHeight - 20, 3500 ); 

				myGump.AddText( (gumpWidth/2)-20, sectTop + itemHeight * 3, 0, "Status" ); 
				myGump.AddBackground( 30, sectTop + itemHeight * 3 + 20, gumpWidth - 20, itemHeight - 20, 3500 ); 

				colData = ODBC.GetColumn( 1, idxStmtNum );
				if( ODBC.lastOK )
					myGump.AddText( 50, sectTop + 30, 0, colData ); 
				else
					myGump.AddText( 50, sectTop + 30, 0, "ERROR" ); 

				colData = ODBC.GetColumn( 2, idxStmtNum );
				if( ODBC.lastOK )
					myGump.AddText( 50, sectTop + itemHeight * 1 + 30, 0, colData ); 
				else
					myGump.AddText( 50, sectTop + itemHeight * 1 + 30, 0, "ERROR" ); 

				colData = ODBC.GetColumn( 3, idxStmtNum );
				if( ODBC.lastOK )
					myGump.AddText( 50, sectTop + itemHeight * 2 + 30, 0, colData ); 
				else
					myGump.AddText( 50, sectTop + itemHeight * 2 + 30, 0, "ERROR" ); 

				colData = ODBC.GetColumn( 4, idxStmtNum );
				if( ODBC.lastOK )
					myGump.AddText( 50, sectTop + itemHeight * 3 + 30, 0, colData ); 
				else
					myGump.AddText( 50, sectTop + itemHeight * 3 + 30, 0, "ERROR" ); 

				if( last10 > 1 )
					myGump.AddPageButton( 30, gumpHeight - 5, 4014, last10 - 1 );
				bDone = (last10 == 10);

			}
			ODBC.QueryRelease();
			if( last10 != 0 )
				myGump.Send( socket ); 
			else
				socket.SysMessage( "No attempted logins" );
		}
	}
}
