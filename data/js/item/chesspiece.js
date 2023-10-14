// If guided movement is enabled, pieces will snap to the center of the square they are moved to
const enableGuidedMovement = true;

// If movement restrictions are enabled, game will attempt to restrict movement of pieces
// based on the rules in place for each particular piece
const enableMovementRestrictions = true;

// Game piece is dropped on an object, or in a container
function onDropItemOnItem( iDropped, pDropper, gameBoard )
{
	if( iDropped.container != gameBoard )
		return 0;

	// Keep track of whose turn it is
	var turnCount = iDropped.container.GetTag( "turnCount" );
	if( turnCount % 2 == 0 )
	{
		whiteMove = false;
	}
	else
	{
		whiteMove = true;
	}

	// Prevent opponent from making a move out of turn
	if( whiteMove && iDropped.GetTag( "team" ) != 1 )
	{
		return 0;
	}
	else if( !whiteMove && iDropped.GetTag( "team" ) != 2 )
	{
		return 0;
	}

	// Fetch location where piece was dropped inside board
	var x       = pDropper.socket.GetWord( 5 );
	var y       = pDropper.socket.GetWord( 7 );
	iDropped.morex = 0;
	iDropped.morey = 0;

	// Check for surrender
	if(( whiteMove && iDropped.id == 0x3587 && x > 235 )
		|| ( !whiteMove && iDropped.id == 0x358e && x < 25 ))
	{
		ConfirmSurrender( pDropper, iDropped, gameBoard );
		return 0;
	}

	// Setup offset
	var morexOffset = 0;
	var moreyOffset = 0;
	switch( iDropped.id )
	{
		case 0x3589: // White Pawn
		case 0x3590: // Black Pawn
			moreyOffset = 0;
			break;
		case 0x3586: // White Rook
		case 0x358d: // Black Rook
			morexOffset = -1;
			moreyOffset = -10;
			break;
		case 0x3585: // White Bishop
		case 0x358c: // Black Bishop
			moreyOffset = -8;
			break;
		case 0x3588: // White Knight
		case 0x358f: // Black Knight
			morexOffset = -1;
			moreyOffset = -8;
			break;
		case 0x3587: // White King
		case 0x358e: // Black King
			morexOffset = -1;
			moreyOffset = -10;
			break;
		case 0x358a: // White Queen
		case 0x3591: // Black Queen
			morexOffset = -1;
			moreyOffset = -10;
			break;
		default:
			break;
	}

	var disableChecks = false;
	var disablePathChecks = false;
	if( enableGuidedMovement )
	{
		if( x > 25 && x <= 235 )
		{
			// If source location was off the board, disable path checks
			if( iDropped.x < 25 || iDropped.x > 235 )
			{
				disablePathChecks = true;
			}

			// X coordinates
			if( x > 25 && x <= 55 )
				iDropped.morex = 43 + morexOffset;
			else if( x > 55 && x <= 80 )
				iDropped.morex = 68 + morexOffset;
			else if( x > 80 && x <= 105 )
				iDropped.morex = 93 + morexOffset;
			else if( x > 105 && x <= 130 )
				iDropped.morex = 118 + morexOffset;
			else if( x > 130 && x <= 155 )
				iDropped.morex = 143 + morexOffset;
			else if( x > 155 && x <= 180 )
				iDropped.morex = 168 + morexOffset;
			else if( x > 180 && x <= 205 )
				iDropped.morex = 193 + morexOffset;
			else if( x > 205 && x <= 235 )
				iDropped.morex = 218 + morexOffset;

			// Y coordinates
			if( y <= 30 )
				iDropped.morey = 15 + moreyOffset;
			else if( y > 30 && y <= 55 )
				iDropped.morey = 40 + moreyOffset;
			else if( y > 55 && y <= 80 )
				iDropped.morey = 65 + moreyOffset;
			else if( y > 80 && y <= 105 )
				iDropped.morey = 90 + moreyOffset;
			else if( y > 105 && y <= 130 )
				iDropped.morey = 115 + moreyOffset;
			else if( y > 130 && y <= 155 )
				iDropped.morey = 140 + moreyOffset;
			else if( y > 155 && y <= 180 )
				iDropped.morey = 165 + moreyOffset;
			else if( y > 180 && y <= 220 )
				iDropped.morey = 190 + moreyOffset;
		}
		else
		{
			disableChecks = true;
		}
	}

	if( !disableChecks )
	{
		var checkForCastling = false;
		var checkEnPassant = false;
		if( enableMovementRestrictions )
		{
			// Restrict movement in X axis based on max movement distance per piece, excluding moving
			// to/within areas on extreme left or extreme right side of board
			if( iDropped.morex > 25 && iDropped.morex <= 235 && iDropped.x > 25 && iDropped.x <= 235 )
			{
				if( iDropped.more > 0 && Math.abs( iDropped.morex - (iDropped.x - morexOffset)) > iDropped.more )
				{
					iDropped.colour = 0x26;
					iDropped.movable = 2;
					iDropped.StartTimer( 200, 2, true );
					return 0;
				}
				if( iDropped.morez > 0 && Math.abs( iDropped.morey - (iDropped.y - moreyOffset)) > iDropped.morez )
				{
					iDropped.colour = 0x26;
					iDropped.movable = 2;
					iDropped.StartTimer( 200, 2, true );
					return 0;
				}

				var deltaX = Math.abs(iDropped.morex - iDropped.x );
				var deltaY = Math.abs(iDropped.morey - iDropped.y )

				// Piece-specific movement rules
				switch( iDropped.id )
				{
					case 0x3589: // White Pawn
					case 0x3590: // Black Pawn
						var disallowMove = false;

						// Disallow moving two spaces unless in starting square
						if( deltaX >= 50 && !( iDropped.x > 55 && iDropped.x <= 80 ) && !( iDropped.x > 180 && iDropped.x <= 205 ))
						{
							disallowMove = true;
						}

						if( deltaX >= 50 )
						{
							checkEnPassant = true;
						}

						// Disallow move if colliding with our own piece
						var canCapture = CheckForExistingPieces( iDropped, true, false );
						if( canCapture == 0 )
						{
							disallowMove = true;
						}

						// Disallow moving horizontally and capturing a piece at same time
						if( deltaX > 0 && deltaY == 0 && canCapture == 1 )
						{
							disallowMove = true;
						}

						// Disallow moving pawn vertically without also moving it diagonally
						if( deltaY > deltaX || deltaX > 25 && deltaY > 3 )
						{
							disallowMove = true;
						}

						// Disallow moving pawn vertically without also moving it diagonally
						if( deltaY == deltaX && ( canCapture == -1 || canCapture == 0 )
							&& ( !iDropped.GetTag( "canCaptureEnPassant" ) || turnCount != iDropped.GetTag( "enPassantTurnCount" ) + 1 ))
						{
							disallowMove = true;
						}

						// Disallow moving pawn backwards
						var team = parseInt( iDropped.GetTag( "team" ));
						if(( team == 1 && iDropped.morex > iDropped.x ) || ( team == 2 && iDropped.morex < iDropped.x ))
						{
							disallowMove = true;
						}

						if( disallowMove )
						{
							iDropped.colour = 0x26;
							iDropped.movable = 2;
							iDropped.StartTimer( 200, 2, true );
							return 0;
						}
						break;
					case 0x3586: // White Rook
					case 0x358d: // Black Rook
						if( deltaY >= 25 && deltaX > 3 || deltaX >= 25 && deltaY > 3 )
						{
							// Disallow moving rook both vertically and horizontally at the same time
							iDropped.colour = 0x26;
							iDropped.movable = 2;
							iDropped.StartTimer( 200, 2, true );
							return 0;
						}
						break;
					case 0x3585: // White Bishop
					case 0x358c: // Black Bishop
						if( Math.abs( deltaX - deltaY ) > 3 )
						{
							// Disallow NOT moving rock both vertically and horizontally at the same time
							iDropped.colour = 0x26;
							iDropped.movable = 2;
							iDropped.StartTimer( 200, 2, true );
							return 0;
						}
						break;
					case 0x3588: // White Knight
					case 0x358f: // Black Knight
						if(( deltaX == 0 || deltaY == 0 ) || ( deltaX == 25 && deltaY == 25 )
							|| ( deltaX == 50 && deltaY != 25 ) || ( deltaX == 50 && deltaY != 25 ))
						{
							// Disallow NOT moving 2 tiles in one axis and 1 tile in the other
							iDropped.colour = 0x26;
							iDropped.movable = 2;
							iDropped.StartTimer( 200, 2, true );
							return 0;
						}
						break;
					case 0x3587: // White King
					case 0x358e: // Black King
						if( deltaX > 25 || deltaY > 25 )
						{
							if( deltaY == 50 && ( iDropped.morex == 217 || iDropped.morex == 42 ))
							{
								checkForCastling = true;
								break;
							}

							// Disallow moving further than 1 tile in any direction
							iDropped.colour = 0x26;
							iDropped.movable = 2;
							iDropped.StartTimer( 200, 2, true );
							return 0;
						}
						break;
					case 0x358a: // White Queen
					case 0x3591: // Black Queen
						if( Math.abs( deltaX - deltaY ) > 2 && ( deltaX != 0 && deltaY != 0 ))
						{
							// Disallow moving in two axis at the same time if they're not same
							iDropped.colour = 0x26;
							iDropped.movable = 2;
							iDropped.StartTimer( 200, 2, true );
							return 0;
						}
						break;
					default:
						break;
				}
			}
			else
			{
				iDropped.colour = 0x26;
				iDropped.movable = 2;
				iDropped.StartTimer( 200, 2, true );
				return 0;
			}
		}

		// Check that path from old to new location is clear
		var retVal = 0;
		if( !disablePathChecks )
		{
			retVal = CheckPath( iDropped, 0 );
			if( retVal == 0 )
			{
				return 0;
			}
		}

		// Check for existing pieces in target square
		retVal = 0;
		if( !disableChecks && !disablePathChecks )
		{
			retVal = CheckForExistingPieces( iDropped, false, true );
		}
		else
		{
			retVal = CheckForExistingPieces( iDropped, false, false );
		}
		if( retVal == 0 )
			return 0;

		// Check if castling move is allowed
		if( checkForCastling )
		{
			var castlingCheck = CheckCastleMove( iDropped );
			if( castlingCheck == 0 )
				return 0;
		}

		// Check en passant move
		if( checkEnPassant || iDropped.GetTag( "canCaptureEnPassant" ))
		{
			retVal = CheckForEnPassant( iDropped, gameBoard, turnCount );
			if( iDropped.GetTag( "canCaptureEnPassant" ) && retVal != 2 )
			{
				iDropped.colour = 0x26;
				iDropped.movable = 2;
				iDropped.StartTimer( 200, 2, true );
				return 0;
			}
		}
	}

	var oldPosY = iDropped.y;

	// If either king or any of the Rooks have moved, tagged them as such
	if( iDropped.id == 0x3586 || iDropped.id == 0x358d || iDropped.id == 0x3587 || iDropped.id == 0x358e )
	{
		iDropped.SetTag( "hasMoved", true );
	}

	// Start timer to snap piece to target square
	iDropped.StartTimer( 100, 1, true );

	// If move was an actual move on central board, let board announce the move
	if( iDropped.morex > 25 && iDropped.morex <= 235 && iDropped.x > 25 && iDropped.x <= 235 )
	{
		SpeakNotation( pDropper, iDropped, gameBoard, morexOffset, moreyOffset, oldPosY, retVal );
	}
}

function CheckCastleMove( iDropped )
{
	var hasKingMoved = iDropped.GetTag( "hasMoved" );
	if( hasKingMoved == true )
	{
		// Disallow castling move if king already moved!
		iDropped.colour = 0x26;
		iDropped.movable = 2;
		iDropped.StartTimer( 200, 2, true );
		return 0;
	}

	var gameBoard = iDropped.container;
	if( !ValidateObject( gameBoard ))
		return 0;

	var mItem;
	var castlingAllowed = false;
	var i = 0;
	var targetItem = null;
	for( mItem = gameBoard.FirstItem(); !gameBoard.FinishedItems(); mItem = gameBoard.NextItem() )
	{
		if( i > gameBoard.itemsinside )
		{
			Console.Warning( "Exceeded item loop count! " );
			return 0;
		}
		i++;

		var team = mItem.GetTag( "team" );
		if(( mItem.id == 0x3586 || mItem.id == 0x358d ) && team == iDropped.GetTag( "team" ))
		{
			var checkPathResult = 0
			if( iDropped.morey > iDropped.y )
			{
				// Moving king down
				if( mItem.y > iDropped.morey && mItem.GetTag( "hasMoved" ) == false )
				{
					mItem.morey = mItem.y - 15;
					break;
				}
			}
			else
			{
				// Moving king up
				if( mItem.y < iDropped.morey && mItem.GetTag( "hasMoved" ) == false )
				{
					mItem.morey = mItem.y + 15;
					break;
				}
			}
		}
	}

	// Check to see path to rook is clear
	if( ValidateObject( mItem ))
	{
		var rookPathResult = CheckPath( iDropped, mItem.morey );
		if( rookPathResult == -1 )
		{
			if( iDropped.y > mItem.y )
			{
				mItem.y -= 75;
			}
			else
			{
				mitem.y += 50;
			}

			mItem.morey = 0;
			mItem.SetTag( "hasMoved", true );
			castlingAllowed = true;
		}
	}

	if( !castlingAllowed )
		return 0;

	iDropped.SetTag( "hasMoved", true );
	return 1;
}

function CheckPath( iDropped, targetY )
{

	var gameBoard = iDropped.container;
	if( !ValidateObject( gameBoard ))
		return 0;

	// Knights are special cases! They cannot be blocked
	if( iDropped.id == 0x3588 || iDropped.id == 0x358f )
		return -1;

	var iOldX = iDropped.x;
	var iOldY = iDropped.y;
	var iNewX = iDropped.morex;
	var iNewY = iDropped.morey;

	if( targetY > 0 )
	{
		iNewY = targetY;
	}

	var boardItem;
	for( boardItem = gameBoard.FirstItem(); !gameBoard.FinishedItems(); boardItem = gameBoard.NextItem() )
	{
		if( ValidateObject( boardItem ) && boardItem != iDropped )
		{
			var boardItemX = boardItem.x;
			var boardItemY = boardItem.y;

			// Check - in absolute values - how far boardItem is from the iDropped on one axis or the other
			var yDiff = Math.abs(boardItemY - iOldY);
			var xDiff = Math.abs(boardItemX - iOldX);

			// Check how far iDrops moves in one axis or the other
			var iDiffX = Math.abs( iOldX - iNewX );
			var iDiffY = Math.abs( iOldY - iNewY );

			if( iDiffX > 13 && iDiffY > 13 )
			{
				// iDropped moves on both X and Y axis
				var dirX = iNewX > iOldX ? 1 : -1;
				var dirY = iNewY > iOldY ? 1 : -1;

				var tempX = 0;
				var tempY = 0;

				for( var i = 25; i < Math.abs( iNewX - iOldX ); i += 25 )
				{
					if( Math.abs( boardItemX - ( iOldX + ( i * dirX ))) < 13 && Math.abs( boardItemY - ( iOldY + ( i * dirY ))) < 13 )
					{
						if( Math.abs( boardItem.x - iNewX ) > 3 && Math.abs( boardItem.y - iNewY ) > 3 )
						{
							iDropped.morex = 0;
							iDropped.morey = 0;
							iDropped.colour = 0x26;
							iDropped.movable = 2;
							iDropped.StartTimer( 200, 2, true );
							return 0;
						}
					}
				}
			}
			else if( iDiffX > 13 && yDiff < 13 )
			{
				// iDropped moves on X axis
				if(( boardItemX > iOldX && boardItemX < iNewX ) || ( boardItemX < iOldX && boardItemX > iNewX ))
				{
					if( Math.abs( boardItem.x - iNewX ) > 3 && Math.abs( boardItem.y - iNewY ) < 13 )
					{
						iDropped.morex = 0;
						iDropped.morey = 0;
						iDropped.colour = 0x26;
						iDropped.movable = 2;
						iDropped.StartTimer( 200, 2, true );
						return 0;
					}
				}
			}
			else if( iDiffY > 13 && xDiff < 3 )
			{
				// iDropped moves on Y axis
				if(( boardItemY > iOldY && boardItemY < iNewY ) || ( boardItemY < iOldY && boardItemY > iNewY ))
				{
					if( Math.abs( boardItem.x - iNewX ) < 13 && Math.abs( boardItem.y - iNewY ) > 3 )
					{
						iDropped.morex = 0;
						iDropped.morey = 0;
						iDropped.colour = 0x26;
						iDropped.movable = 2;
						iDropped.StartTimer( 200, 2, true );
						return 0;
					}
				}
			}
		}
	}
	return -1;
}

function CheckForExistingPieces( iDropped, simulateCapture, allowCapture )
{
	var gameBoard = iDropped.container;
	var mItem;
	if( !ValidateObject( gameBoard ))
		return 0;

	for( mItem = gameBoard.FirstItem(); !gameBoard.FinishedItems(); mItem = gameBoard.NextItem() )
	{
		if( ValidateObject( mItem ))
		{
			if( Math.abs( mItem.x - iDropped.morex ) < 11 && Math.abs( mItem.y - iDropped.morey ) < 11 )
			{
				var iTeam = parseInt( iDropped.GetTag( "team" ));
				var mTeam = parseInt( mItem.GetTag( "team" ));

				if( iTeam != mTeam )
				{
					// Square occupied by opponent's piece - capture it!
					if( !simulateCapture && allowCapture )
					{
						if( iTeam == 1 )
						{
							mItem.x = RandomNumber( 245, 255 );
							mItem.y = RandomNumber( 5, 190 );
						}
						else
						{
							mItem.x = RandomNumber( 5, 15 );
							mItem.y = RandomNumber( 5, 190 );
						}
						return 1;
					}
					else if( !simulateCapture && !allowCapture )
					{
						iDropped.morex = 0;
						iDropped.morey = 0;
						return 0;
					}
					else
						return 1;
				}
				else
				{
					// Disallow move, square occupied by own chess piece!
					iDropped.morex = 0;
					iDropped.morey = 0;
					return 0;
				}
			}
		}
	}
	return -1;
}

function ConvertPawn( iDropped )
{
	var finalPosX = iDropped.x;
	var team = parseInt( iDropped.GetTag( "team" ));

	if( team == 1 )
	{
		// White Pawn
		if( iDropped.id == 0x3589 )
		{
			if( finalPosX > 25 && finalPosX <= 55 )
			{
				// Turn White Pawn into White Queen
				var whiteQueen = CreateDFNItem( null, null, "0x358a", 1, "ITEM", false, 0, iDropped.worldnumber, iDropped.instanceID );
				whiteQueen.container = iDropped.container;
				whiteQueen.x = iDropped.x - 1;
				whiteQueen.y = iDropped.y - 10;
				whiteQueen.SetTag( "team", 1 );
				iDropped.Delete();
			}
		}
	}
	else // team == 2
	{
		// Black Pawn
		if( iDropped.id == 0x3590 )
		{
			if( finalPosX > 205 && finalPosX <= 235 )
			{
				// Turn Black Pawn into Black Queen
				var blackQueen = CreateDFNItem( null, null, "0x3591", 1, "ITEM", false, 0, iDropped.worldnumber, iDropped.instanceID );
				blackQueen.container = iDropped.container;
				blackQueen.x = iDropped.x - 1;
				blackQueen.y = iDropped.y - 10;
				blackQueen.SetTag( "team", 2 );
				iDropped.Delete();
			}
		}
	}
}

function CheckForEnPassant( iDropped, gameBoard, turnCount )
{
	var iTeam = parseInt( iDropped.GetTag( "team" ));

	// Check to see if there are any pieces that are allowed to capture this piece "en passant"
	for( mItem = gameBoard.FirstItem(); !gameBoard.FinishedItems(); mItem = gameBoard.NextItem() )
	{
		// We only care about pawns!
		if( mItem.id != 0x3589 && mItem.id != 0x3590 )
			continue;

		if( Math.abs( mItem.x - iDropped.morex ) < 3 && Math.abs( mItem.y - iDropped.morey ) < 30 )
		{
			// Mark iDropped as being able to be captured
			iDropped.SetTag( "enPassantEnabled", true );

			// Mark nearby pawn as allowed to capture
			mItem.SetTag( "canCaptureEnPassant", ( iDropped.serial ).toString() );
			mItem.SetTag( "enPassantTurnCount", turnCount );
			return 1;
		}

		if( iDropped.GetTag( "canCaptureEnPassant" ) && mItem.GetTag( "enPassantEnabled" ) && Math.abs( mItem.x - iDropped.morex ) == 25 && Math.abs( mItem.y - iDropped.morey ) < 3 )
		{
			// Capture another pawn with en passant move
			if( iTeam == 1 )
			{
				mItem.x = RandomNumber( 245, 255 );
				mItem.y = RandomNumber( 5, 190 );
			}
			else
			{
				mItem.x = RandomNumber( 5, 15 );
				mItem.y = RandomNumber( 5, 190 );
			}
			mItem.SetTag( "enPassantEnabled", null );
			iDropped.SetTag( "canCaptureEnPassant", null );
			return 2;
		}
	}
	return -1;
}

function onTimer( timerObj, timerID )
{
	if( timerID == 1 )
	{
		// Snap piece to new square after a very short delay
		if( timerObj.morex != 0 )
		{
			timerObj.x = timerObj.morex;
			timerObj.morex = 0;
		}
		if( timerObj.morey != 0 )
		{
			timerObj.y = timerObj.morey;
			timerObj.morey = 0;
		}

		// Increase turnCount on gameBoard
		timerObj.container.SetTag( "turnCount", timerObj.container.GetTag( "turnCount" ) + 1 );

		// Reset color to remove any highlighting
		timerObj.colour = 0x0;
	}

	// Reset color to remove any highlighting
	timerObj.colour = 0x0;

	// Make piece movable again
	timerObj.movable = 1;

	if( timerObj.id == 0x3589 || timerObj.id == 0x3590 )
	{
		ConvertPawn( timerObj );
	}
}

function SpeakNotation( pDropper, iDropped, gameBoard, morexOffset, moreyOffset, oldPosY, capturePiece )
{
	var finalPosX = iDropped.morex - morexOffset;
	var finalPosY = iDropped.morey - moreyOffset;

	var notationPieceName = "";
	switch( iDropped.id )
	{
		case 0x3589: // White Pawn
		case 0x3590: // Black Pawn
			if( capturePiece == 1 )
			{
				if( oldPosY <= 30 )
					notationPieceName = "h";
				else if( oldPosY > 30 && oldPosY <= 55 )
					notationPieceName = "g";
				else if( oldPosY > 55 && oldPosY <= 80 )
					notationPieceName = "f";
				else if( oldPosY > 80 && oldPosY <= 105 )
					notationPieceName = "e";
				else if( oldPosY > 105 && oldPosY <= 130 )
					notationPieceName = "d";
				else if( oldPosY > 130 && oldPosY <= 155 )
					notationPieceName = "c";
				else if( oldPosY > 155 && oldPosY <= 180 )
					notationPieceName = "b";
				else if( oldPosY > 180 && oldPosY <= 220 )
					notationPieceName = "a";
			}
			break;
		case 0x3586: // White Rook
		case 0x358d: // Black Rook
			notationPieceName = "R";
			break;
		case 0x3585: // White Bishop
		case 0x358c: // Black Bishop
			notationPieceName = "B";
			break;
		case 0x3588: // White Knight
		case 0x358f: // Black Knight
			notationPieceName = "N";
			break;
		case 0x3587: // White King
		case 0x358e: // Black King
			notationPieceName = "K";
			break;
		case 0x358a: // White Queen
		case 0x3591: // Black Queen
			notationPieceName = "Q";
			break;
		default:
			break;
	}

	var notationPosX = "";
	if( finalPosX > 25 && finalPosX <= 55 )
		notationPosX = "8";
	else if( finalPosX > 55 && finalPosX <= 80 )
		notationPosX = "7";
	else if( finalPosX > 80 && finalPosX <= 105 )
		notationPosX = "6";
	else if( finalPosX > 105 && finalPosX <= 130 )
		notationPosX = "5";
	else if( finalPosX > 130 && finalPosX <= 155 )
		notationPosX = "4";
	else if( finalPosX > 155 && finalPosX <= 180 )
		notationPosX = "3";
	else if( finalPosX > 180 && finalPosX <= 205 )
		notationPosX = "2";
	else if( finalPosX > 205 && finalPosX <= 235 )
		notationPosX = "1";

	var notationPosY = "";
	if( finalPosY <= 30 )
		notationPosY = "h";
	else if( finalPosY > 30 && finalPosY <= 55 )
		notationPosY = "g";
	else if( finalPosY > 55 && finalPosY <= 80 )
		notationPosY = "f";
	else if( finalPosY > 80 && finalPosY <= 105 )
		notationPosY = "e";
	else if( finalPosY > 105 && finalPosY <= 130 )
		notationPosY = "d";
	else if( finalPosY > 130 && finalPosY <= 155 )
		notationPosY = "c";
	else if( finalPosY > 155 && finalPosY <= 180 )
		notationPosY = "b";
	else if( finalPosY > 180 && finalPosY <= 220 )
		notationPosY = "a";

	var team = iDropped.GetTag( "team" );
	if( team == 1 )
	{
		notationColor = 0x07d7;
	}
	else
	{
		notationColor = 0x060f;
	}

	if( gameBoard.container == null )
		gameBoard.TextMessage( notationPieceName + ( capturePiece == 1 ? "x" : "" ) + notationPosY + notationPosX, true, notationColor );
}

function ConfirmSurrender( pDropper, iDropped, gameBoard )
{
	switch( iDropped.GetTag( "team" ))
	{
		case 1: team = "White"; break;
		case 2: team = "Black"; break;
		default:
			Console.Warning( "Not team tag found for game piece with item serial: " + iDropped.serial );
			return;
	}

	if( ValidateObject( iDropped ))
	{
		iDropped.Delete();
	}

	var tempMsg = GetDictionaryEntry( 2721 ) // %s Surrenders! Resetting game board.
	gameBoard.TextMessage( tempMsg.replace( /%s/gi, team ));
	TriggerEvent( 5024, "onUseChecked", pDropper, gameBoard );
}

// Game piece is dropped on a character
function onDropItemOnNpc( pDropper, pDroppedOn, iDropped )
{
	return 0;
}

// Game piece is dropped on ground outside board
function onDrop( iDropped, pDropper )
{
	return 0;
}