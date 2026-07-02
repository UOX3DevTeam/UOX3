/// <reference path="../../../definitions.d.ts" />
// @ts-check

/** @type { ( ambushNPC: Character, timerID: number ) => void } */
function onTimer( ambushNPC, timerID )
{
	if( !ValidateObject( ambushNPC ) )
	{
		return;
	}

	if( timerID != 1 )
	{
		return;
	}

	if( !ambushNPC.GetTag( "EscortAmbushSpawned" ) )
	{
		return;
	}

	ambushNPC.Delete();
}