/// <reference path="../../../definitions.d.ts" />
// @ts-check

/** @type { ( pUser: Character, iUsed: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ))
	{
		return false;
	}

	var socket = pUser.socket;
	if( socket == null )
	{
		return false;
	}

	if( !iUsed.InRange( pUser, 2 ))
	{
		socket.SysMessage( "You are too far away." );
		return false;
	}

	var questID = GetItemQuestID( pUser, iUsed );
	if( questID <= 0 )
	{
		socket.SysMessage( "This item does not offer a quest." );
		return false;
	}

	var quest = TriggerEvent( 5801, "QuestList", questID );
	if( !quest )
	{
		socket.SysMessage( "Quest data could not be loaded." );
		return false;
	}

	if( quest.type == "escort" || ( quest.guidedWalk && quest.guidedWalk.enabled ) || ( quest.race && quest.race.enabled ))
	{
		socket.SysMessage( "This quest must be started by an NPC." );
		return false;
	}

	pUser.SetTempTag( "QuestConversationQuestID", questID );
	TriggerEvent( 5802, "QuestConversationGump", pUser, null, questID );
	return false;
}

/** @type { ( pUser: Character, questItem: Item ) => number } */
function GetItemQuestID( pUser, questItem )
{
	var questID = parseInt( questItem.GetTag( "QuestID" ), 10 );
	if( !isNaN( questID ) && questID > 0 )
	{
		return questID;
	}

	var questRootID = parseInt( questItem.GetTag( "QuestRootID" ), 10 );
	if( isNaN( questRootID ) || questRootID <= 0 )
	{
		return 0;
	}

	var playerQuestID = TriggerEvent( 5802, "ResolvePlayerQuestID", pUser, questRootID );
	if( !playerQuestID )
	{
		return questRootID;
	}

	return parseInt( playerQuestID, 10 );
}