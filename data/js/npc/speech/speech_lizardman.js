var syllableCount = ["ss", "sth", "iss", "is", "ith", "kth", "sith", "this", "its", "sit", "tis", "tsi", "ssi", "sil", "lis", "sis", "lil", "thil",
	"lith", "sthi", "lish", "shi", "shash", "sal", "miss", "ra", "tha", "thes", "ses", "sas", "las", "les", "sath", "sia", "ais", "isa", "asi", "asth",
	"stha", "sthi", "isth", "asa", "ath", "tha", "als", "sla", "thth", "ci", "ce", "cy", "yss", "ys", "yth", "syth", "thys", "yts", "syt", "tys", "tsy", "ssy",
	"syl", "lys", "sys", "lyl", "thyl", "lyth", "sthy", "lysh", "shy", "myss", "ysa", "sthy", "ysth"];

function onSpeech( myString, myPlayer, myNPC )
{
	if( !myNPC.InRange( myPlayer, 2 ) )
		return;

	var mySocket = myPlayer.socket;
	if( mySocket == null )
		return;

	HandleCustomKeyWords( myNPC );
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
		
	myNPC.SoundEffect(  418, true  );
	return myNPC.TextMessage( mySentence, true, 58, 2 );
}

function GetRandomSyllable()
{
	return syllableCount[RandomNumber(  0, syllableCount.length  )];
}

