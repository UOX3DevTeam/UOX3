// Ethereal Warrior

// searchAmount = Number of times healer will look around for chars to resurrect before stopping
// searchInterval = The interval at which the healer looks around for chars to resurrect after having started searching
// searchRange = The radius in which the healer searches for wounded people
// searchTimer = Amount of time between each time the healer initiates his searches
// resTimer = Minimum time that must pass before healer will resurrect same char again (1000 = 1 second)
// manaCostRes = the amount of mana the NPC uses for resurrecting
var searchAmount = 3;
var searchInterval = 5000;
var searchRange = 4;
var searchTimer = 15000;
var resTimer = 30000;
var manaCostRes = 50;

// These variables can be customized or commented out if you don't want to use them
var resSoundFX = 0x376A;
var resGFXEffect = 0x376A;

function inRange( pCharacter, objInRange )
{
	if( !ValidateObject( objInRange ) || objInRange.isItem || ( objInRange.isChar && ( objInRange.npc || ( !objInRange.online || !objInRange.dead ))))
		return;

	// Get the current server clock, and if it exists, the time for when the last search was started
	// Compare the two, and the script will see if enough time has passed to initiate a new search
	// This also ensures that the script stays working even if the server saves in the middle of a
	// search, but crashes before the next save.
	var iTime = GetCurrentClock();
	var initSearchTime = pCharacter.GetTag( "initSearchTime" );

	//If search has already been initiated, don't start a new search, unless an abnormal amount of time has passed
	if(( initSearchTime != null && initSearchTime != 0 ) && ((( iTime - initSearchTime ) < searchTimer ) && !( initSearchTime > iTime )))
		return;
	else if((( iTime - initSearchTime ) > searchTimer ) || initSearchTime > iTime )
	{
		pCharacter.SetTag( "initSearchTime", iTime );
		pCharacter.StartTimer( searchInterval, 1, true );
	}
}

// This triggers the keyword-based healing/resurrection
function onSpeech( strSaid, pTalking, npcHealer )
{
	if( strSaid == "heal" )
	{
		if( pTalking.dead )
		{
			Resurrect( pTalking, npcHealer );
		}
	}
	return false;
}

function Resurrect( deadChar, npcHealer )
{
	if( !deadChar.criminal && !deadChar.murderer && deadChar.karma > 0 )
	{
		if( npcHealer.mana > manaCostRes )
		{
			if( !npcHealer.CanSee( deadChar )) // Do most expensive check last
				return;

			var iTime = GetCurrentClock();
			var lastResTime = deadChar.GetTag( "lastResTime" );
			if(( lastResTime == null || lastResTime == 0 ) || ((( iTime - lastResTime ) > resTimer ) || ( lastResTime > iTime )))
			{
               	npcHealer.StaticEffect( 0x376A, 0, 0x0f );
				npcHealer.SoundEffect( 0x1F2, true );
				deadChar.Resurrect();
				deadChar.SetTag( "lastResTime", iTime );
				npcHealer.mana -= manaCostRes;
			}
		}
	}
}

function onTimer( srcChar, timerID )
{
	if( !ValidateObject( srcChar ))
		return;

	if( timerID == 1 )
	{ //Search for nearby wounded characters the specified amount of times
		var searchCount = srcChar.GetTag( "searchCount" );
		if( searchCount < searchAmount )
		{
			AreaCharacterFunction( "SearchForWounded", srcChar, searchRange );
			srcChar.StartTimer( searchInterval, 1, true );
			searchCount++;
		}
		else
		{
			searchCount = 0;
		}
		srcChar.SetTag( "searchCount", searchCount );
	}
}

//This function iterates through all characters within the specified radius in AreaCharacterFunction
//It then checks to make sure they are valid for receiving healing or resurrection.
function SearchForWounded( srcChar, trgChar, pSock )
{
	if( !ValidateObject( trgChar ) || !ValidateObject( srcChar ))
		return;

	if( trgChar.serial != srcChar.serial )
	{
		if( !trgChar.npc && ( trgChar.online && trgChar.dead ))
		{
			Resurrect( trgChar, srcChar );
		}
	}
}

// Handle draining of health, stamina and mana on attack
function onAttack( pAttacker, pDefender, hitStatus, hitLoc, damageDealt )
{
	if( !ValidateObject( pDefender ))
		return;

	if( RandomNumber( 1, 100 ) <= 25 ) // 25% chance to drain
	{
		// Amount to drain from target
		var drainAmount = RandomNumber( 20, 30 );

		switch( RandomNumber( 1, 3 )) // 33% chance to drain either health, stamina or mana
		{
			case 1: // Health
				if( pDefender.health >= drainAmount )
				{
					pAttacker.health += drainAmount;
				}
				else
				{
					pAttacker.health += pDefender.health;
				}
				pDefender.Damage( drainAmount, 1 );
				pAttacker.TextMessage( GetDictionaryEntry( 9069 ), false, 0, 5, pDefender.serial ); // I can grant life, and I can sap it as easily.
				break;
			case 2: // Stamina
				if( pDefender.stamina >= drainAmount )
				{
					pAttacker.stamina += drainAmount;
				}
				else
				{
					pAttacker.stamina += pDefender.stamina;
				}
				pDefender.stamina -= drainAmount;
				pAttacker.TextMessage( GetDictionaryEntry( 9070 ), false, 0, 5, pDefender.serial ); // You'll go nowhere, unless I deem it should be so.
				break;
			case 3: // Mana
				if( pDefender.mana >= drainAmount )
				{
					pAttacker.mana += drainAmount;
				}
				else
				{
					pAttacker.mana += pDefender.mana;
				}
				pDefender.mana -= drainAmount;
				pAttacker.TextMessage( GetDictionaryEntry( 9071 ), false, 0, 5, pDefender.serial ); // Your power is mine to use as I will.
				break;
			default:
				break;
		}
	}
}

function _restorecontext_() {}
