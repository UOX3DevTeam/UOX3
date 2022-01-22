var syllableCount = ["skrit", "ch", "ch", "it", "ti", "it", "ti", "ak", "ek", "ik", "ok", "uk", "yk",
		"ka", "ke", "ki", "ko", "ku", "ky", "at", "et", "it", "ot", "ut", "yt",
		"cha", "che", "chi", "cho", "chu", "chy", "ach", "ech", "ich", "och", "uch", "ych",
		"att", "ett", "itt", "ott", "utt", "ytt", "tat", "tet", "tit", "tot", "tut", "tyt",
		"tta", "tte", "tti", "tto", "ttu", "tty", "tak", "tek", "tik", "tok", "tuk", "tyk",
		"ack", "eck", "ick", "ock", "uck", "yck", "cka", "cke", "cki", "cko", "cku", "cky",
		"rak", "rek", "rik", "rok", "ruk", "ryk", "tcha", "tche", "tchi", "tcho", "tchu", "tchy",
		"rach", "rech", "rich", "roch", "ruch", "rych", "rrap", "rrep", "rrip", "rrop", "rrup", "rryp",
		"ccka", "ccke", "ccki", "ccko", "ccku", "ccky"];

function onSpeech( myString, myPlayer, myNPC )
{
	if ( !myNPC.InRange( myPlayer, 2 ) )
		return;

	var mySocket = myPlayer.socket;
	if ( mySocket == null )
		return;

	Speech( myNPC );
}

function onAttack( pAttacker, pDefender )
{
	var Random = RandomNumber( 1, 100 )
	if ( Random >= 20 && Random <= 40 ) // 20% - 40 chance to talk
	{
		Speech( pAttacker );
	}
}

function onDefense( pAttacker, pDefender )
{
	var Random = RandomNumber( 1, 99 )
	if ( Random >= 20 && Random <= 40 ) // 20% chance to talk
	{
		Speech( pDefender );
	}
}

// Handle custom speech keywords
function Speech( myNPC )
{
	var mySentence = ""; // Prepare a variable for our sentence
	var sentenceLength = RandomNumber( 2, 6 ); // Randomize amount of words in sentence
	for ( var i = 0; i < sentenceLength; i++ )
	{
		// Build up a sentence from the randomly selected syllables
		mySentence += GetRandomSyllable();

		// Add a space between each syllable, except after the last one
		if ( i < sentenceLength - 1 )
			mySentence += " ";
	}
	myNPC.SoundEffect( 438, true );
	return myNPC.TextMessage( mySentence, true, 149, 2 );
}

function GetRandomSyllable()
{
	return syllableCount[RandomNumber(  0, syllableCount.length  )];
}

