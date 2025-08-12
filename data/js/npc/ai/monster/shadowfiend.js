/// <reference path="../../../definitions.d.ts" />
// @ts-check
// Shadowfiend AI

// shadowfiend_SearchAmount = Number of times shadowfiend will look around for hidden chars to seek out before stopping
// shadowfiend_SarchHiddenMaxCount = Number of times shadowfiend will try to seek out a detected hidden character before giving up
// shadowfiend_SearchInterval = The interval at which the shadowfiend looks around for hidden chars to seek out after having started searching
// shadowfiend_SearchHiddenInterval = The interval at which the shadowfiend tries to detect hidden characters it's seeking out
// shadowfiend_SearchRange = The radius in which the shadowfiend searches for hidden chars
const shadowfiend_SearchAmount = 10;
const shadowfiend_SarchHiddenMaxCount = 3;
const shadowfiend_SearchInterval = 5000;
const shadowfiend_SearchHiddenInterval = 2500;
const shadowfiend_SearchRange = 10;

/** @type { ( srcObj: BaseObject, objInRange: BaseObject ) => void } */
function inRange( shadowfiend, objInRange )
{
	if( !ValidateObject( objInRange ) || objInRange.isItem || ( objInRange.isChar && ( objInRange.npc || ( !objInRange.online || objInRange.dead ))))
		return;

	// Do nothing if shadowfiend is already in combat
	if( shadowfiend.atWar && ValidateObject( shadowfiend.target ))
		return;

	// Enable our search functionality
	if( typeof( shadowfiend._isSearchEnabled ) == "undefined" || shadowfiend._isSearchEnabled == false )
	{
		shadowfiend._isSearchEnabled = true;
	}
}

/** @type { ( pSliver: Character ) => boolean } */
function onAISliver( shadowfiend )
{
	if( !shadowfiend._isSearchEnabled || shadowfiend._isSearching )
		return;

	let searchCount = shadowfiend.GetTempTag( "searchCount" );
	if( searchCount <= shadowfiend_SearchAmount )
	{
		// Get the current server clock, and if it exists, the time for when the last search was started
		// Compare the two, and the script will see if enough time has passed to initiate a new search
		// This also ensures that the script stays working even if the server saves in the middle of a
		// search, but crashes before the next save.
		var currentTime = GetCurrentClock();
		var lastSearchTime = parseInt( shadowfiend.GetTempTag( "lastSearchTime" ));

		// Start new search if
		//	- Search has never run before
		//	- More than shadowfiend_SearchInterval ms have passed since last search

		if( lastSearchTime == null || lastSearchTime == 0 || ( currentTime - lastSearchTime ) >= shadowfiend_SearchInterval )
		{
			shadowfiend.SetTempTag( "searchCount", shadowfiend.GetTempTag( "searchCount" ) + 1 );
			shadowfiend.SetTempTag( "lastSearchTime", currentTime.toString() );
			shadowfiend.StartTimer( shadowfiend_SearchInterval, 1, 3228 );
			shadowfiend._isSearching = true;
		}
	}
	else
	{
		// Exceeded search count, let's reset
		shadowfiend._isSearchEnabled = false;
		shadowfiend._isSearching = false;
		shadowfiend.SetTempTag( "searchCount", 0 );
	}
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( shadowfiend, timerID )
{
	if( !ValidateObject( shadowfiend ))
		return;

	// Don't proceed if shadowfiend is already in combat
	if( shadowfiend.atWar && ValidateObject( shadowfiend.target ))
		return;

	if( timerID == 1 )
	{
		// Search for nearby hidden players the specified amount of times
		//var searchCount = shadowfiend.GetTempTag( "searchCount" );
		//if( searchCount < shadowfiend_SearchAmount )
		//{
			let searchResult = AreaCharacterFunction( "SearchForHidden", shadowfiend, shadowfiend_SearchRange, null );
			if( searchResult == 0 )
			{
				shadowfiend._isSearching = false;
				// Keep looking
				//shadowfiend.StartTimer( shadowfiend_SearchInterval, 1, 3228 );
				//searchCount++;
			}
			else
			{
				// We found someone!
				let hiddenTargSerial = parseInt( shadowfiend.GetTempTag( "hiddenTargSerial" ));
				let hiddenTarg = CalcCharFromSer( hiddenTargSerial );
				if( ValidateObject( hiddenTarg ))
				{
					searchCount = shadowfiend_SearchAmount;
					shadowfiend.WalkTo( hiddenTarg, 20 );
					shadowfiend.StartTimer( shadowfiend_SearchHiddenInterval, 2, 3228 );
				}
				else
				{
					shadowfiend.SetTempTag( "hiddenTargSerial", null );
					shadowfiend.SetTempTag( "hiddenSearchCount", null );
				}
			}
		/*}
		else
		{
			searchCount = 0;
			shadowfiend._isSearchEnabled = false;
		}
		shadowfiend.SetTempTag( "searchCount", searchCount );*/
	}
	else if( timerID == 2 )
	{
		// Try to detect hidden target character
		let hiddenTargSerial = parseInt( shadowfiend.GetTempTag( "hiddenTargSerial" ));
		let hiddenTarg = CalcCharFromSer( hiddenTargSerial );
		if( ValidateObject( hiddenTarg ))
		{
			if( shadowfiend.DistanceTo( hiddenTarg ) <= 2 )
			{
				// Reveal hidden character
				hiddenTarg.visible = 0;
				if( hiddenTarg.socket != null )
				{
					hiddenTarg.TextMessage( GetDictionaryEntry( 1436, hiddenTarg.socket.language ), false, 0, 5, hiddenTarg.serial ); // You were revealed!
				}
			}
			else if( shadowfiend.DistanceTo( hiddenTarg ) <= shadowfiend_SearchRange && shadowfiend.CanSee( hiddenTarg ))
			{
				// Target still within range, let's keep searching
				let hiddenSearchCount = shadowfiend.GetTempTag( "hiddenSearchCount" );
				if( hiddenSearchCount < shadowfiend_SarchHiddenMaxCount )
				{
					shadowfiend.SetTempTag( "hiddenSearchCount", hiddenSearchCount + 1 );
					shadowfiend.WalkTo( hiddenTarg, 20 );
					shadowfiend.StartTimer( shadowfiend_SearchHiddenInterval, 2, 3228 );
					return;
				}
			}
		}

		// Target already revealed, or out of range or not in LoS, or max hidden search count reached
		shadowfiend.SetTempTag( "hiddenTargSerial", null );
		shadowfiend.SetTempTag( "hiddenSearchCount", null );
		shadowfiend._isSearching = false;

		// Do a check to see if there still are players within range, to determine whether to
		// continue looking for hidden players or not
		if( AreaCharacterFunction( "SearchForPlayers", shadowfiend, 24, null ) == 0 )
		{
			// No players found in distance of 24 tiles, disable search and wait for next player to come within range
			shadowfiend._isSearchEnabled = false;
			Console.Warning( "Shadowfiend halting search for hidden players, no more players nearby." );
		}
	}
}

// This function iterates through all characters within the specified radius in AreaCharacterFunction
// It then checks to make sure they are valid as hidden players that can be sought out
function SearchForHidden( shadowfiend, trgChar, pSock )
{
	if( !ValidateObject( trgChar ) || !ValidateObject( shadowfiend ) || shadowfiend.GetTempTag( "hiddenTargSerial" ))
		return false;

	if( !trgChar.npc && ( trgChar.online && !trgChar.dead && !trgChar.isGM && !trgChar.isCounselor && trgChar.visible == 1 ))
	{
		if( shadowfiend.CanSee( trgChar ))
		{
			if( shadowfiend.DistanceTo( trgChar ) > 2 )
			{
				shadowfiend.SetTempTag( "hiddenTargSerial", trgChar.serial.toString() );
				return true;
			}
		}
	}
}

// Another function to look for players within "vis range" (24)
// We don't care whether they're hidden or not in this case,
// we just want to know if there are ANY valid players nearby, so we can
// continue running our checks for hidden players.
function SearchForPlayers( shadowfiend, trgChar, pSock )
{
	if( !ValidateObject( trgChar ) || !ValidateObject( shadowfiend ))
		return false;

	if( !trgChar.npc && ( trgChar.online && !trgChar.dead && !trgChar.isGM && !trgChar.isCounselor ))
	{
		return true;
	}
}

function _restorecontext_() {}
