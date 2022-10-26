// Gameboards
function onUseChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;
	if( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}
	switch( iUsed.id )
	{
		case 0x0E1C: // Backgammon
		case 0x0FAD:
			CreateBackgammonPieces( iUsed );
			break;
		case 0x0FA6: // Chessboard or Checkers
			if( iUsed.morey == 1 )
			{
				// Chess
				if( iUsed.totalItemCount != 32 )
				{
					CreateChessPieces( pUser, pSocket, iUsed );
					iUsed.SetTag( "turnCount", 1 );
					return true;
				}
			}
			else
			{
				// Checkers
				if( iUsed.totalItemCount != 24 )
				{
					CreateCheckersPieces( pUser, pSocket, iUsed );
					iUsed.SetTag( "turnCount", 1 );
					return true;
				}
			}
			break;
	}
	return true;
}

function onDropItemOnItem( iDropped, pDropper, iDroppedOn )
{
	// If any object is dropped on this container, disallow and bounce item back where it came from
	return 0;
}

function CreateBackgammonPieces( iUsed )
{
	// TODO
}

function CreateCheckersPieces( pUser, pSocket, iUsed )
{
	// First, clear out existing pieces
	if( iUsed.itemsinside > 0 )
	{
		for( mItem = iUsed.FirstItem(); !iUsed.FinishedItems(); mItem = iUsed.NextItem() )
		{
			if( ValidateObject( mItem ))
			{
				mItem.Delete();
			}
		}
	}

	CreateBlackCheckersPieces( pUser, pSocket, iUsed );
	CreateWhiteCheckersPieces( pUser, pSocket, iUsed );
}

function CreateBlackCheckersPieces( pUser, pSocket, iUsed )
{
	// Create black checker discs/men
	var xOffset = 45;
	var yOffset = 28;
	var i = 0;
	var j = 0;
	for( i = 0; i < 12; i++ )
	{
		var blackDisc = CreateDFNItem( pSocket, pUser, "0x358b", 1, "ITEM", false );
		blackDisc.container = iUsed;
		if( i < 4 )
		{
			xOffset = 45;
		}
		else if( i < 8 )
		{
			xOffset = 70;
		}
		else
		{
			xOffset = 95;
		}

		if( i == 0 || i == 8 )
		{
			yOffset = 28;
			j = 0;
		}
		else if( i == 4 )
		{
			yOffset = 53;
			j = 0;
		}

		blackDisc.x = xOffset;
		blackDisc.y = yOffset + ( j * 50 );
		blackDisc.SetTag( "team", 1 );
		j++
	}
}

function CreateWhiteCheckersPieces( pUser, pSocket, iUsed )
{
	// Create white checker discs/men
	var i = 0;
	for( i = 0; i < 12; i++ )
	{
		var whiteDisc = CreateDFNItem( pSocket, pUser, "0x3584", 1, "ITEM", false );
		whiteDisc.container = iUsed;
		if( i < 4 )
		{
			xOffset = 170;
		}
		else if( i < 8 )
		{
			xOffset = 195;
		}
		else
		{
			xOffset = 220;
		}

		if( i == 0 || i == 8 )
		{
			yOffset = 53;
			j = 0;
		}
		else if( i == 4 )
		{
			yOffset = 28;
			j = 0;
		}

		whiteDisc.x = xOffset;
		whiteDisc.y = yOffset + ( j * 50 );
		whiteDisc.SetTag( "team", 2 );
		j++
	}
}

function CreateChessPieces( pUser, pSocket, iUsed )
{
	// First, clear out existing pieces
	if( iUsed.itemsinside > 0 )
	{
		for( mItem = iUsed.FirstItem(); !iUsed.FinishedItems(); mItem = iUsed.NextItem() )
		{
			if( ValidateObject( mItem ))
			{
				mItem.Delete();
			}
		}
	}

	CreateBlackChessPieces( pUser, pSocket, iUsed );
	CreateWhiteChessPieces( pUser, pSocket, iUsed );
}

function CreateBlackChessPieces( pUser, pSocket, iUsed )
{
	// Create Pawns
	var i = 0;
	for( i = 0; i < 8; i++ )
	{
		var blackPawn = CreateDFNItem( pSocket, pUser, "0x3590", 1, "ITEM", false );
		blackPawn.container = iUsed;
		blackPawn.x = 68;
		blackPawn.y = ( 25 * i ) + 15;
		blackPawn.SetTag( "team", 2 );
	}

	// Create Rooks
	var blackRook = CreateDFNItem( pSocket, pUser, "0x358d", 1, "ITEM", false );
	blackRook.container = iUsed;
	blackRook.x = 42;
	blackRook.y = 5;
	blackRook.SetTag( "team", 2 );
	blackRook = CreateDFNItem( pSocket, pUser, "0x358d", 1, "ITEM", false );
	blackRook.container = iUsed;
	blackRook.x = 42;
	blackRook.y = 180;
	blackRook.SetTag( "team", 2 );

	// Create Knights
	var blackKnight = CreateDFNItem( pSocket, pUser, "0x358f", 1, "ITEM", false );
	blackKnight.container = iUsed;
	blackKnight.x = 42;
	blackKnight.y = 32;
	blackKnight.SetTag( "team", 2 );
	blackKnight = CreateDFNItem( pSocket, pUser, "0x358f", 1, "ITEM", false );
	blackKnight.container = iUsed;
	blackKnight.x = 42;
	blackKnight.y = 157;
	blackKnight.SetTag( "team", 2 );

	// Create Bishops
	var blackBishop = CreateDFNItem( pSocket, pUser, "0x358c", 1, "ITEM", false );
	blackBishop.container = iUsed;
	blackBishop.x = 43;
	blackBishop.y = 57;
	blackBishop.SetTag( "team", 2 );
	blackBishop = CreateDFNItem( pSocket, pUser, "0x358c", 1, "ITEM", false );
	blackBishop.container = iUsed;
	blackBishop.x = 43;
	blackBishop.y = 132;
	blackBishop.SetTag( "team", 2 );

	// Create Queen
	var blackQueen = CreateDFNItem( pSocket, pUser, "0x3591", 1, "ITEM", false );
	blackQueen.container = iUsed;
	blackQueen.x = 42;
	blackQueen.y = 105;
	blackQueen.SetTag( "team", 2 );

	// Create King
	var blackKing = CreateDFNItem( pSocket, pUser, "0x358e", 1, "ITEM", false );
	blackKing.container = iUsed;
	blackKing.x = 42;
	blackKing.y = 80;
	blackKing.SetTag( "team", 2 );
}

function CreateWhiteChessPieces( pUser, pSocket, iUsed )
{
	// Create Pawns
	var i = 0;
	for( i = 0; i < 8; i++ )
	{
		var whitePawn = CreateDFNItem( pSocket, pUser, "0x3589", 1, "ITEM", false );
		whitePawn.container = iUsed;
		whitePawn.x = 193;
		whitePawn.y = ( 25 * i ) + 15;
		whitePawn.SetTag( "team", 1 );
	}

	// Create Rooks
	var whiteRook = CreateDFNItem( pSocket, pUser, "0x3586", 1, "ITEM", false );
	whiteRook.container = iUsed;
	whiteRook.x = 217;
	whiteRook.y = 5;
	whiteRook.SetTag( "team", 1 );
	whiteRook = CreateDFNItem( pSocket, pUser, "0x3586", 1, "ITEM", false );
	whiteRook.container = iUsed;
	whiteRook.x = 217;
	whiteRook.y = 180;
	whiteRook.SetTag( "team", 1 );

	// Create Knights
	var whiteKnight = CreateDFNItem( pSocket, pUser, "0x3588", 1, "ITEM", false );
	whiteKnight.container = iUsed;
	whiteKnight.x = 217;
	whiteKnight.y = 32;
	whiteKnight.SetTag( "team", 1 );
	whiteKnight = CreateDFNItem( pSocket, pUser, "0x3588", 1, "ITEM", false );
	whiteKnight.container = iUsed;
	whiteKnight.x = 217;
	whiteKnight.y = 157;
	whiteKnight.SetTag( "team", 1 );

	// Create Bishops
	var whiteBishop = CreateDFNItem( pSocket, pUser, "0x3585", 1, "ITEM", false );
	whiteBishop.container = iUsed;
	whiteBishop.x = 218;
	whiteBishop.y = 57;
	whiteBishop.SetTag( "team", 1 );
	whiteBishop = CreateDFNItem( pSocket, pUser, "0x3585", 1, "ITEM", false );
	whiteBishop.container = iUsed;
	whiteBishop.x = 218;
	whiteBishop.y = 132;
	whiteBishop.SetTag( "team", 1 );

	// Create Queen
	var whiteQueen = CreateDFNItem( pSocket, pUser, "0x358a", 1, "ITEM", false );
	whiteQueen.container = iUsed;
	whiteQueen.x = 217;
	whiteQueen.y = 105;
	whiteQueen.SetTag( "team", 1 );

	// Create King
	var whiteKing = CreateDFNItem( pSocket, pUser, "0x3587", 1, "ITEM", false );
	whiteKing.container = iUsed;
	whiteKing.x = 217;
	whiteKing.y = 80;
	whiteKing.SetTag( "team", 1 );
}

