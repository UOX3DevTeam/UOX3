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

	var team = iDropped.GetTag( "team" );
	// Prevent opponent from making a move out of turn
	if( whiteMove && team != 1 )
	{
		return 0;
	}
	else if( !whiteMove && team != 2 )
	{
		return 0;
	}

	// Fetch location where piece was dropped inside board
	var x = pDropper.socket.GetWord( 5 );
	var y = pDropper.socket.GetWord( 7 );
	iDropped.morex = 0;
	iDropped.morey = 0;

	// Check for surrender
	if(( whiteMove && x < 25 ) || ( !whiteMove && x > 235 ))
	{
		ConfirmSurrender( pDropper, iDropped, gameBoard );
		return 0;
	}

	// Setup offset
	var morexOffset = 2;
	var moreyOffset = 13;

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
		if( enableMovementRestrictions )
		{
			// Restrict movement in X axis based on max movement distance per piece, excluding moving
			// to/within areas on extreme left or extreme right side of board
			if( iDropped.morex > 25 && iDropped.morex <= 235 && iDropped.x > 25 && iDropped.x <= 235 )
			{
				var deltaX = Math.abs( iDropped.morex - iDropped.x );
				var deltaY = Math.abs( iDropped.morey - iDropped.y );

				if( Math.abs( deltaX - deltaY ) > 15 )
				{
					// Disallow NOT moving rock both vertically and horizontally at the same time
					iDropped.colour = 0x26;
					iDropped.movable = 2;
					iDropped.StartTimer( 200, 2, true );
					return 0;
				}
				else if(( team == 1 && iDropped.morex < iDropped.x ) || ( team == 2 && iDropped.morex > iDropped.x ))
				{
					// Disallow NOT moving towards opponent
					iDropped.colour = 0x26;
					iDropped.movable = 2;
					iDropped.StartTimer( 200, 2, true );
					return 0;
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
			retVal = CheckForExistingPieces( iDropped );
		}
		if( retVal == 0 )
		{
			return 0;
		}
	}

	// Safekeep old Y position for later
	var oldPosY = iDropped.y;

	// Start timer to snap piece to target square
	iDropped.StartTimer( 100, 1, true );

	// If move was an actual move on central board, let board announce the move
	// if( iDropped.morex > 25 && iDropped.morex <= 235 && iDropped.x > 25 && iDropped.x <= 235 )
	// 	SpeakNotation( pDropper, iDropped, gameBoard, morexOffset, moreyOffset, oldPosY, retVal );
}

function CheckPath( iDropped, targetY )
{
	var gameBoard = iDropped.container;
	if( !ValidateObject( gameBoard ))
		return 0;

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
			var yDiff = Math.abs( boardItemY - iOldY );
			var xDiff = Math.abs( boardItemX - iOldX );

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
							if( boardItem.GetTag( "team" ) == 1 )
							{
								boardItem.x = RandomNumber( 245, 255 );
								boardItem.y = RandomNumber( 5, 190 );
							}
							else
							{
								boardItem.x = RandomNumber( 5, 15 );
								boardItem.y = RandomNumber( 5, 190 );
							}
						}
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
				// Disallow move, square occupied by another piece!
				iDropped.morex = 0;
				iDropped.morey = 0;
				return 0;
			}
		}
	}
	return -1;
}

// function PromoteCheckerPiece( iDropped )
// {
// 	var finalPosX = iDropped.x;
// 	var team = parseInt(iDropped.GetTag( "team" ));

// 	if( team == 1 && finalPosX > 205 && finalPosX <= 235 )
// 	{
// 		// Promote Black Checker piece to King
// 		// ...
// 	}
// 	else if( team == 2 && finalPosX > 25 && finalPosX <= 55 )
// 	{
// 		// Promote White Checker piece to King
// 		// ...
// 	}
// }

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

	// TODO - Put in place detection of movement to king row, and promote checker piece to king
	//PromoteCheckerPiece( timerObj );
}

function SpeakNotation( pDropper, iDropped, gameBoard, morexOffset, moreyOffset, oldPosY, capturePiece )
{
	// TODO - Have gameboard speak proper checkers notation using "from"-"to" syntax
	// Example: 11-15 (square 11 to square 15)
}

function ConfirmSurrender( pDropper, iDropped, gameBoard )
{
	switch( iDropped.GetTag( "team" ))
	{
		case 1: team = "Black"; break;
		case 2: team = "White"; break;
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