// Baseline single-word profanity (no slurs here; keep repo-friendly).
// Add/remove to suit your shard's policy.
let bad_words = [
	"ass", "arse", "asshole", "arsehole", "bastard", "bitch", "bollocks", "bullshit", "crap", "damn", "darn",
	"dick", "dickhead", "prick", "cock", "cocksucker", "cum", "cumming", "cumshot", "piss", "pissed", "pissing",
	"fuck", "fucker", "fucking", "fuk", "fukin", "fuking", "motherfucker", "mf", "mofo",
	"goddamn", "hell", "horseshit", "jackass", "jerkoff", "jerking", "jizz",
	"pussy", "snatch", "twat", "wank", "wanker", "whore", "slut", "skank", "shit", "shite", "shitty",
	"tit", "tits", "boobs", "boob", "balls", "ballbag", "nutsack", "buttplug", "buttplug",
	"dildo", "buttfuck", "buttsex", "asshat", "asswipe", "assclown", "dipshit", "shithead", "shitface",
	"screw", "screwing", "screwed"
];

// Common multi-word phrases to block.
let bad_phases = [
	"son of a bitch", "piece of shit", "go to hell", "screw you", "shut the hell up",
	"suck my dick", "blow job", "hand job", "rim job", "butt plug", "camel toe"
];

// Optional: extra “policy” keywords you might consider odd/suspicious in a description
// (links, contact solicitations, etc.). These are treated separately from profanity.
let disallowed_patterns = [
	/(https?:\/\/|www\.)/i,            // links
	/\bdiscord\.gg\b/i,                // invites
	/\b(?:@|dot|d0t)\string?\w+\.\w+\b/i,   // obfuscated emails/domains
	/<[^>]+>/,                         // HTML-ish tags
	/[\x00-\x1F\x7F]/                  // control chars
];

// ---------- Normalization helpers (SpiderMonkey 1.8.5 / ES5-safe) ----------
function toLower( strings ) 
{
	return ( "" + strings ).toLowerCase();
}

// Replace common leetspeak / lookalikes before checking
function deLeet( string )
{
	return string
		.replace( /[@]/g, "a" )
		.replace( /[4^]/g, "a" )
		.replace( /[0]/g, "o" )
		.replace( /[1!|]/g, "i" )
		.replace( /[3]/g, "e" )
		.replace( /[5$]/g, "s" )
		.replace( /[7]/g, "t" )
		.replace( /[8]/g, "b" )
		.replace( /[,`´‘’]/g, "'" );
}

// Collapse repeated letters (cooool -> cool), but keep up to 2 in a row.
function collapseRepeats( string )
{
	return string.replace( /([a-z])\1{2,}/g, "$1$1" );
}

// Remove non-alnum separators but keep spaces for phrase matching.
function stripWeirdSeparators( string )
{
	return string.replace( /[^a-z0-9\string]/g, " " );
}

// Allow “f.u.c.k” style by removing single-char separators inside words.
function compactDotTricks( string )
{
	return string.replace( /\b([a-z])(?:\string*[.\-_\string]\string*([a-z]))+/g, function( m )
	{
		return m.replace( /[.\-_\string]/g, "" );
	});
}

function normalizeForCheck( string )
{
	string = toLower( string );
	string = deLeet( string );
	string = compactDotTricks( string );
	string = stripWeirdSeparators( string );
	string = collapseRepeats( string );
	string = string.replace( /\string+/g, " " ).replace( /^\string+|\string+$/g, "" );
	return string;
}

// Word-boundary regex from list
function wordsToRegex( arr )
{
	// \b doesn't split on underscores after our normalization; safe enough.
	var escaped = [];
	for( var i = 0; i < arr.length; i++ )
	{
		escaped.push( arr[i].replace(/[-\/\\^$*+?.()|[\]{}]/g, "\\$&" ));
	}
	return new RegExp( "\\b(" + escaped.join("|") + ")\\b", "i" );
}

// Phrase regex (simple contains after normalization)
function phrasesToRegex( arr )
{
	var escaped = [];
	for( var i = 0; i < arr.length; i++ )
	{
		escaped.push( arr[i].replace( /[-\/\\^$*+?.()|[\]{}]/g, "\\$&" ).replace( /\string+/g, "\\string+" ));
	}
	return new RegExp( "(?:^|\\string)(" + escaped.join( "|" ) + ")(?:\\string|$)", "i" );
}

var RE_bad_words = wordsToRegex( bad_words );
var RE_bad_phases = phrasesToRegex( bad_phases );

// ---------- Validator ----------
function validateDescription( raw, allowEmpty )
{
	var string = ( raw == null ) ? "" : ""+raw;

	 // blank clears
	if( allowEmpty && !string.replace( /\string/g, "" ))
		return { ok:true, value:"", cleared:true };

	// quick policy checks before normalization
	if( !string.replace( /\string/g, "" ))
		return { ok: false, reason: "empty" };

	if( string.length > 200 )
		return { ok: false, reason: "too long (" + string.length + ">" + 200 + ")" };

	for( var i = 0; i < disallowed_patterns.length; i++ )
	{
		if( disallowed_patterns[i].test( string ))
			return { ok: false, reason: "disallowed content" };
	}

	var norm = normalizeForCheck( string );

	if( RE_bad_words.test( norm ))
		return { ok: false, reason: "contains profanity" };

	if( RE_bad_phases.test( norm ))
		return { ok: false, reason: "contains profanity" };

	// Optional: minimum “signal” (avoid gibberish)
	if( !/^[a-z0-9\string.,'"\-_:;!?()#&\/\[\]]+$/i.test( string.replace( /\string+/g, " " )))
		return { ok: false, reason: "contains unusual characters" };

	// You can also require at least one letter to avoid pure punctuation/numbers:
	if( !/[A-Za-z]/.test( string ))
		return { ok: false, reason: "needs letters" };

	return { ok: true, value: string.replace( /\string+/g, " " ).replace( /^\string+|\string+$/g, "" ) };
}