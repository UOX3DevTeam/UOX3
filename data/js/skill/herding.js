// Herding is triggered via double-clicking a Shepherd's Crook or regular Crook
function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	var itemOwner = GetPackOwner( iUsed, 0 );
	if( iUsed.movable == 2 || iUsed.movable == 3 )
	{
		socket.SysMessage( GetDictionaryEntry( 774, socket.language )); // That is locked down and you cannot use it
		return false;
	}
	else if( itemOwner == null || itemOwner.serial != pUser.serial )
	{
		socket.SysMessage( GetDictionaryEntry( 6019, socket.language )); // This must be in your backpack or equipped before it can be used.
		return false;
	}
	else if( GetServerSetting( "ToolUseLimit" ) && iUsed.usesLeft < 1 )
	{
		// Item has no uses left
		socket.SysMessage( GetDictionaryEntry( 9262, socket.language )); // This has no more charges.
		return false;
	}
	else
	{
		socket.tempObj = iUsed;
		pUser.CustomTarget( 0, GetDictionaryEntry( 9116, socket.language )); // Target the animal you wish to herd.
	}
}

function onCallback0( socket, myTarget )
{
	var pUser = socket.currentChar;

	if( !socket.GetWord( 1 ) && ValidateObject( myTarget ) && myTarget.npc )
	{
		if( myTarget.tamed || !myTarget.skillToTame )
		{
			socket.SysMessage( GetDictionaryEntry( 9117, socket.language )); // That is not a herdable animal.
			return;
		}
		else if( !myTarget.InRange( pUser, 7 ))
		{
			socket.SysMessage( GetDictionaryEntry( 393, socket.language )); // That is out of range
			return;
		}
		else if( !pUser.CanSee( myTarget ))
		{
			socket.SysMessage( GetDictionaryEntry( 1646, socket.language )); // You cannot see that
			return;
		}
		else
		{
			if( myTarget.wandertype != 1 )
			{
				// Keep track of NPC's original wander type
				myTarget.SetTempTag( "oldWanderType", myTarget.wandertype );
			}

			if( GetServerSetting( "ToolUseLimit" ))
			{
				var herdingTool = socket.tempObj;
				if( !ValidateObject( herdingTool ) || herdingTool.usesLeft < 1 )
					return;

				herdingTool.usesLeft -= 1;
				if( herdingTool.usesLeft == 0 && GetServerSetting( "ToolUseBreak" ))
				{
					herdingTool.Delete();
					pSock.SysMessage( GetDictionaryEntry( 9122, pSock.language )); // You broke your shepherd's crook.
					// Play sound effect of tool breaking
					return;
				}
			}

			socket.tempObj = myTarget;
			socket.CustomTarget( 1, GetDictionaryEntry( 9118, socket.language )); // Click where you wish the animal to go.
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 9119, socket.language )); // You don't seem able to persuade that to move.
		return;
	}
}

function onCallback1( socket, myTarget )
{
	var targetChar = socket.tempObj;
	socket.tempObj = null;

	// If user cancels targeting with Escape, ClassicUO still sends a targeting response (unlike
	// regular UO client), but one byte in the packet is always 255 when this happens
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck != 255 )
	{
		var pUser = socket.currentChar;
		var targX = socket.GetWord( 11 );
		var targY = socket.GetWord( 13 );
		var skillToHerd = targetChar.skillToTame;
		var minSkill = 0;
		var maxSkill = skillToHerd;

		if( ValidateObject( targetChar ) && ( targX != targetChar.x || targY != targetChar.y ))
		{
			if( skillToHerd > pUser.skills.herding )
			{
				socket.SysMessage( GetDictionaryEntry( 9119, socket.language )); // You don't seem able to persuade that to move.
				return;
			}

			if( pUser.CheckSkill( 24, minSkill, maxSkill ))
			{
				if( ValidateObject( myTarget ) && myTarget == pUser )
				{
					// Targeted self
					targetChar.Follow( pUser );
					socket.SysMessage( GetDictionaryEntry( 9120, socket.language )); // The animal beings to follow you.
				}
				else
				{
					// Targeted a location
					// Stop NPC from following its target, and restore its original wandertype
					var oldWanderType = targetChar.GetTempTag( "oldWanderType" );
					targetChar.SetTempTag( "oldWanderType", null );
					targetChar.Follow( null );
					targetChar.wandertype = oldWanderType;

					// Have NPC walk to the targeted location, maximum 20 steps away
					targetChar.WalkTo( targX, targY, 20 );
					socket.SysMessage( GetDictionaryEntry( 9121, socket.language )); // The animal walks where it was instructed to.
				}
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 9119, socket.language )); // You don't seem able to persuade that to move.
			return;
		}
	}
}