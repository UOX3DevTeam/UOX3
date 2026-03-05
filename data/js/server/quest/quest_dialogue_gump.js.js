/// <reference path="../../definitions.d.ts" />
// @ts-check

function onTalk( pTalking, strSaid )
{
	if( !ValidateObject( pTalking ))
		return true;

	var sock = pTalking.socket;
	if( sock == null )
		return true;

	// Normalize input
	var s = ("" + strSaid);
	s = s.replace(/^\s+|\s+$/g, "");
	var low = s.toLowerCase();

	// Example keyword trigger: "'garrick" or "garrick"
	// You can pick your own rules here.
	if( low == "garrick" || low == "'garrick" || low.indexOf("garrick") != -1 )
	{
		// Find the NPC nearby (optional but recommended)
		var npc = FindNearbyNpcByName( pTalking, "garrick" );
		if( ValidateObject( npc ))
		{
			// Start tree
			TriggerEvent( 7510, "OpenDialogueTree", pTalking, npc, "blacksmith_tutorial", null );

			// Prevent speech showing + block hardcode
			return false;
		}

		// If no NPC found, let speech show
		return true;
	}

	return true;
}

// Simple nearby scan. Adjust radius if you want.
function FindNearbyNpcByName( pUser, nameLower )
{
	// This uses AreaCharacterFunction pattern: it calls a callback for chars in radius.
	// If your server build does not have AreaCharFunction, tell me and I will swap it
	// to a different scan method you do have.
	pUser.SetTempTag( "DLG_FindNpcResult", "0" );
	AreaCharacterFunction( "DLG_FindNpc_Callback", pUser, 10, nameLower );

	var ser = parseInt( pUser.GetTempTag( "DLG_FindNpcResult" ), 10 );
	if( isNaN( ser ) || ser <= 0 )
		return null;

	var c = CalcCharFromSer( ser );
	if( ValidateObject( c ))
		return c;

	return null;
}

function DLG_FindNpc_Callback( srcChar, trgChar, socket, nameLower )
{
	// trgChar is the char found in area
	if( !ValidateObject( trgChar ) || !ValidateObject( srcChar ))
		return false;

	// Only NPCs
	if( trgChar.npc != 1 )
		return false;

	var n = ("" + trgChar.name).toLowerCase();
	if( n.indexOf( nameLower ) != -1 )
	{
		srcChar.SetTempTag( "DLG_FindNpcResult", "" + trgChar.serial );
		return true; // stop scanning if AreaCharFunction respects return true as "found"
	}

	return false;
}

// ------------------------------------------------------------
// Entry point
// ------------------------------------------------------------
function OpenDialogueTree( pUser, pNpc, treeID, nodeId )
{
	if( !ValidateObject( pUser ))
		return;

	var socket = pUser.socket;
	if( socket == null )
		return;

	var tree = TriggerEvent( 0, "GetDialogueTree_Trigger", treeID ); // If you register TriggerEvent for it
	// If you are not using TriggerEvent, call GetDialogueTree(treeID) directly.

	if( !tree )
	{
		socket.SysMessage( "Dialogue tree not found: " + treeID );
		return;
	}

	nodeId = nodeId || tree.startNode;
	var node = tree.nodes[nodeId];
	if( !node )
	{
		socket.SysMessage( "Dialogue node not found: " + nodeId );
		return;
	}

	// Save context so onGumpPress can continue the conversation
	pUser.SetTempTag( "DLG_TreeID", String( treeID ));
	pUser.SetTempTag( "DLG_NodeID", String( nodeId ));
	pUser.SetTempTag( "DLG_NpcSerial", ValidateObject(pNpc) ? ("" + pNpc.serial) : "0" );

	var g = new Gump();
	g.AddPage( 0 );
	g.NoClose();

	g.AddBackground( 30, 100, 360, 420, 1579 );
	g.AddGump( 55, 110, 1577 );

	// Title
	g.AddHTMLGump( 55, 135, 310, 25, true, false, "<center><b>" + (tree.title || "Conversation") + "</b></center>" );

	// Node text
	g.AddHTMLGump( 55, 165, 310, 180, true, true, node.text || "" );

	// Choices area
	var choices = node.choices || [];
	var y = 360;
	var shown = 0;

	for( var i = 0; i < choices.length && shown < DLG_MAX_CHOICES; i++ )
	{
		var c = choices[i];
		if( !ChoiceAllowed( pUser, c ))
			continue;

		// button id: 100 + indexInNode
		g.AddButton( 55, y, 0x2EE0, 0x2EE2, 1, 0, 100 + i );
		g.AddText( 90, y + 2, 0, c.text || "..." );

		y += 26;
		shown++;
	}

	// Close button
	g.AddButton( 300, 470, 0x2EEC, 0x2EEE, 1, 0, 1 );
	g.AddText( 325, 472, 0, "Close" );

	g.Send( socket );
	g.Free();
}

// ------------------------------------------------------------
// Conditions
// ------------------------------------------------------------
function ChoiceAllowed( pUser, choice )
{
	if( !choice || !choice.cond )
		return true;

	var cond = choice.cond;

	// tagEquals
	if( cond.tagEquals )
	{
		var t = cond.tagEquals.tag;
		var v = cond.tagEquals.value;

		// UOX3 GetTag returns 0 if missing
		var cur = pUser.GetTag( t );
		return ( String(cur) == String(v) );
	}

	// tagMin
	if( cond.tagMin )
	{
		var t2 = cond.tagMin.tag;
		var minV = cond.tagMin.value;

		var cur2 = pUser.GetTag( t2 );
		cur2 = parseInt( cur2, 10 );
		if( isNaN(cur2) ) cur2 = 0;

		return ( cur2 >= minV );
	}

	// hasQuest
	if( cond.hasQuest )
	{
		return HasQuestActive( pUser, cond.hasQuest );
	}

	// completedQuest
	if( cond.completedQuest )
	{
		return HasQuestCompleted( pUser, cond.completedQuest );
	}

	return true;
}

function HasQuestActive( pUser, questID )
{
	var progress = TriggerEvent( 5800, "ReadQuestProgress", pUser ) || [];
	for( var i = 0; i < progress.length; i++ )
	{
		if( progress[i].serial == pUser.serial && progress[i].questID == questID )
			return true;
	}
	return false;
}

function HasQuestCompleted( pUser, questID )
{
	var archived = TriggerEvent( 5800, "ReadArchivedQuests", pUser ) || [];
	for( var i = 0; i < archived.length; i++ )
	{
		if( parseInt( archived[i], 10 ) == questID )
			return true;
	}
	return false;
}

// ------------------------------------------------------------
// Button handler
// ------------------------------------------------------------
/** @type { ( pSock: Socket, pButton: number, gumpData: GumpData ) => void } */
function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ))
		return;

	// Close
	if( pButton == 1 || pButton == 0 )
		return;

	var treeID = String( pUser.GetTempTag( "DLG_TreeID" ));
	var nodeID = String( pUser.GetTempTag( "DLG_NodeID" ));
	var npcSer = parseInt( pUser.GetTempTag( "DLG_NpcSerial" ), 10 );
	if( isNaN(npcSer) ) npcSer = 0;

	var pNpc = null;
	if( npcSer > 0 )
		pNpc = CalcCharFromSer( npcSer );

	var tree = TriggerEvent( 0, "GetDialogueTree_Trigger", treeID );
	if( !tree )
		return;

	var node = tree.nodes[nodeID];
	if( !node )
		return;

	var idx = pButton - 100;
	if( idx < 0 )
		return;

	var choice = (node.choices && node.choices[idx]) ? node.choices[idx] : null;
	if( !choice )
		return;

	if( !ChoiceAllowed( pUser, choice ))
	{
		pSock.SysMessage( "You cannot choose that." );
		OpenDialogueTree( pUser, pNpc, treeID, nodeID );
		return;
	}

	// Perform action
	DoChoiceAction( pUser, pNpc, choice.action );

	// Advance
	if( choice.next == null )
		return;

	OpenDialogueTree( pUser, pNpc, treeID, String(choice.next) );
}

function DoChoiceAction( pUser, pNpc, action )
{
	if( !action )
		return;

	// startQuest
	if( action.startQuest )
	{
		// Use your quest core start logic
		TriggerEvent( 5800, "CheckQuest", pUser, action.startQuest, "start" );
		return;
	}

	// turnInQuest
	if( action.turnInQuest )
	{
		TriggerEvent( 5800, "CompleteQuest", pUser, action.turnInQuest );
		return;
	}

	// setTags
	if( action.setTags )
	{
		for( var k in action.setTags )
		{
			if( action.setTags.hasOwnProperty(k) )
			{
				var v = action.setTags[k];
				pUser.SetTag( k, v );
			}
		}
	}

	// sysMsg
	if( action.sysMsg )
	{
		var sock = pUser.socket;
		if( sock )
			sock.SysMessage( String(action.sysMsg) );
	}
}