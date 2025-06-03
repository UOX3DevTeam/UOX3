// Remote Movement Commands!
//
// Use 'xwalk <target> <target-location> to make the targeted character walk to targeted location
// Use 'xrun <target> <target-location> to make the targeted character run to targeted location
// Use 'xturn <target> <target-location> to make targeted character turn towards the targeted location

function CommandRegistration()
{
	RegisterCommand( "xwalk", 2, true );
	RegisterCommand( "xrun", 2, true );
	RegisterCommand( "xturn", 2, true );
}

function command_XWALK( socket, cmdString )
{
	var pUser = socket.currentChar;
	socket.xText = "walk";
	socket.allowPartial = false;
	socket.ignoreDoors = false;

	var cmdParams = cmdString.split( " " );
	if( cmdParams[0] )
	{
		socket.allowPartial = true;
	}

	if( cmdParams[1] )
	{
		socket.ignoreDoors = true;
	}
	pUser.CustomTarget( 0, GetDictionaryEntry( 8254, socket.language )); // Select NPC to move remotely using xwalk:
}

function command_XRUN( socket, cmdString )
{
	var pUser = socket.currentChar;
	socket.xText = "run";
	socket.allowPartial = false;
	socket.ignoreDoors = false;

	var cmdParams = cmdString.split( " " );
	if( cmdParams[0] )
	{
		socket.allowPartial = true;
	}

	if( cmdParams[1] )
	{
		socket.ignoreDoors = true;
	}
	pUser.CustomTarget( 0, GetDictionaryEntry( 8253, socket.language )); // Select NPC to move remotely using xrun:
}

function command_XTURN( socket, cmdString )
{
	var pUser = socket.currentChar;
	socket.xText = "turn";
	pUser.CustomTarget( 0, GetDictionaryEntry( 8255, socket.language )); // Select NPC to turn remotely using xturn:
}

function onCallback0( socket, myTarget )
{
	var pUser = socket.currentChar;
	if( !socket.GetWord( 1 ) && ValidateObject( myTarget ) && myTarget.isChar )
	{
		socket.tempObj = myTarget;
  		var targMsg = GetDictionaryEntry( 8256, socket.language ); // Select movement (%s) destination:
		pUser.CustomTarget( 1, targMsg.replace( /%s/gi, socket.xText ));
	}
	else
	{
		pUser.SysMessage( GetDictionaryEntry( 8257, socket.language )); // You must target an NPC!
	}
}

function onCallback1( socket, myTarget )
{
	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var targetChar = socket.tempObj;
		if( ValidateObject( targetChar ))
		{
			var pUser = socket.currentChar;
			var StrangeByte   = socket.GetWord( 1 );
			var targX = socket.GetWord( 11 );
			var targY = socket.GetWord( 13 );

			var walkType = socket.xText;
			if( walkType == "turn" )
			{
				targetChar.TurnToward( targX, targY );
			}
			else
			{
				var allowPartial = socket.allowPartial ? socket.allowPartial : false;
				var ignoreDoors = socket.ignoreDoors ? socket.ignoreDoors : false;
				targetChar.SetTempTag( "allowPartial", allowPartial );
				targetChar.SetTempTag( "ignoreDoors", ignoreDoors );

				if( walkType == "walk" )
				{
					targetChar.WalkTo( targX, targY, 600, allowPartial, ignoreDoors );
				}
				else if( walkType == "run" )
				{
					targetChar.RunTo( targX, targY, 600, allowPartial, ignoreDoors );
				}
			}

  			var targMsg = GetDictionaryEntry( 8256, socket.language ); // Select movement (%s) destination:
			pUser.CustomTarget( 1, targMsg.replace( /%s/gi, socket.xText ));
		}
	}
	else
	{
		// User cancelled target cursor
		socket.xText = "";
		socket.tempObj = null;
		socket.allowPartial = null;
		socket.ignoreDoors = null;
	}
}

function _restorecontext_() {}