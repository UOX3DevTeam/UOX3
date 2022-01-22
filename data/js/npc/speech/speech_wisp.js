var syllableCount =["b", "c", "d", "f", "g", "h", "i", "j", "k", "l", "m", "n", "p", "r", "s", "t", "v", "w", "x", "z", "c",
	"c", "x", "x", "x", "x", "x", "y", "y", "y", "y", "t", "t", "k", "k", "l", "l", "m", "m", "m", "m", "z"];

var i = 0;
var searchAmount = 3;
var searchInterval = 5000;
var searchRange = 8;
var searchTimer = 15000;

function onSpeech( myString, myPlayer, myNPC )
{
	if(  !myNPC.InRange(  myPlayer, 2  )  )
		return;

	var mySocket = myPlayer.socket;
	if(  mySocket == null  )
		return;
		
	HandleCustomKeyWords( myNPC );
}

function inRange(pCharacter, objInRange)
{
	// Get the current server clock, and if it exists, the time for when the last search was started 
	// Compare the two, and the script will see if enough time has passed to initiate a new search 
	// This also ensures that the script stays working even if the server saves in the middle of a 
	// search, but crashes before the next save. 
	var iTime = GetCurrentClock();
	var initSearchTime = pCharacter.GetTempTag( "initSearchTime" );
	//If search has already been initiated, don't start a new search, unless an abnormal amount of time has passed 
	if( ( initSearchTime != null && initSearchTime != 0 ) && ( ( ( iTime - initSearchTime ) < searchTimer ) && !( initSearchTime > iTime ) ) )
	{
		// pCharacter.TextMessage( initSearchTime + " " + iTime + " " + searchTimer ); 
		// return; 
	}
	else if( ( ( iTime - initSearchTime ) > searchTimer ) || initSearchTime > iTime )
	{
		pCharacter.SetTempTag( "initSearchTime", iTime );
		Speech( pCharacter );
		pCharacter.StartTimer( searchInterval, 1, true );
	}
}

function onTimer(srcChar, timerID)
{
	if( timerID == 1 )
	{//Search for nearby characters the specified amount of times
		if( i < searchAmount )
		{
			AreaCharacterFunction( "searchForPlayers", srcChar, searchRange );
			srcChar.StartTimer( searchInterval, 1, true );
			i++;
		}
		else
		{
			i = 0;
			Speech( pCharacter );
		}
	}
}

//This function iterates through all characters within the specified radius in AreaCharacterFunction 
//It then checks to make sure they are valid for msgs
function searchForPlayers( srcChar, trgChar, pSock )
{
	if( srcChar == trgChar )
		return;

	var Random = RandomNumber( 1, 100 )
	if( Random >= 20 && Random <= 40 ) // 20% - 40 chance to talk
	{
		Speech(srcChar);
	}
}

function onAttack( pAttacker, pDefender )
{
	var Random = RandomNumber( 1, 100 )
	if( Random >= 20 && Random <= 40 ) // 20% - 40 chance to talk
	{
		Speech( pAttacker );
	}
}

function onDefense( pAttacker, pDefender )
{
	var Random = RandomNumber( 1, 99 )
	if( Random >= 20 && Random <= 40 ) // 20% chance to talk
	{
		Speech( pDefender );
	}
}

// Handle custom speech keywords
function Speech( myNPC )
{
	var mySentence = ""; // Prepare a variable for our sentence
	var sentenceLength = RandomNumber( 2, 6 ); // Randomize amount of words in sentence
	for( var i = 0; i < sentenceLength; i++ )
	{
		// Build up a sentence from the randomly selected syllables
		mySentence += GetRandomSyllable();

		// Add a space between each syllable, except after the last one
		if( i < sentenceLength - 1 )
			mySentence += " ";
	}

	myNPC.SoundEffect(  466, true  );
	return myNPC.TextMessage( mySentence, true, 89, 2 );
}

function GetRandomSyllable()
{
	return syllableCount[RandomNumber(  0, syllableCount.length  )];
}