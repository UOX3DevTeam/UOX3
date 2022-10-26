// Fire Breath
const fireBreathDelay = 5;
const maxRange = 12;
const manaCost = 30;

function onAISliver( mNPC )
{
	var nextFireBallTime = parseInt( mNPC.GetTag( "nextFB" ));
	if( mNPC.target != null && ( nextFireBallTime < GetCurrentClock() || nextFireBallTime > GetCurrentClock() + ( fireBreathDelay * 2000 )) && mNPC.InRange( mNPC.target, maxRange ))
	{
		var chanceOfFireBreath = mNPC.GetTag( "fbChance" );
		if( mNPC.mana >= manaCost && RandomNumber( 1, 2000 ) <= chanceOfFireBreath )
		{
			// Cast Fire Breath, then reset chance to cast back to 0
			chanceOfFireBreath = 1;
			nextFireBallTime = (( RandomNumber( fireBreathDelay, fireBreathDelay * 2 ) * 1000 ) + GetCurrentClock() );
			mNPC.SetTag( "fbChance", chanceOfFireBreath );
			mNPC.SetTag( "nextFB", nextFireBallTime.toString() );
			if( !mNPC.CanSee( mNPC.target ))
				return;
			CastFireBreath( mNPC );
		}
		else
		{
			// Every cycle Fire Breath is NOT cast, increase chance to cast it in next cycle
			chanceOfFireBreath++;
			mNPC.SetTag( "fbChance", chanceOfFireBreath );
		}
	}
}

function CastFireBreath( mNPC )
{
	var ourTarg = mNPC.target;
	if( ValidateObject( ourTarg ) && ( ourTarg.npc || ( ourTarg.online && !ourTarg.dead )))
	{
		// Pause dragon for 1 second as they cast their Dragon Breath
		mNPC.SetTimer( Timer.MOVETIME, 1500 );

		mNPC.frozen = true;
		mNPC.isCasting = true;

		// Turn toward target and play casting animation, if one exists
		mNPC.TurnToward( ourTarg );

		// Fetch information on NPC's fire breath ability
		GetFireBreathInfo( mNPC );

		// Give the NPC some time to stop before starting the casting anim
		mNPC.StartTimer( 100, 0, true );
	}
}

function onTimer( mNPC, timerID )
{
	if( timerID == 0 && ValidateObject( mNPC ))
	{
		// Play spell-cast animation for NPC
		var fireBreathActionID = mNPC.GetTag( "fbActID" );
		var fireBreathFrameCount = mNPC.GetTag( "fbFrCnt" );
		var fireBreathFrameDelay = mNPC.GetTag( "fbFrDelay" );
		var fireBreathSFX = mNPC.GetTag( "fbSFX" );
		mNPC.DoAction( fireBreathActionID, null, fireBreathFrameCount, fireBreathFrameDelay ); // Dragon "stomp" animation is 15 frames long
		mNPC.SoundEffect( fireBreathSFX, true );
		mNPC.StartTimer( 1400, 1, true );
	}
	else if( timerID == 1 && ValidateObject( mNPC ))
	{
		// Fetch fire breath FX details from NPC
		var fireBreathFX = mNPC.GetTag( "fbFX" );
		var fireBreathFXColor = mNPC.GetTag( "fbFXHue" );

		// Play effects
		var target = mNPC.target;
		if( ValidateObject( target ))
		{
			DoMovingEffect( mNPC.x, mNPC.y, mNPC.z + 5, target.x, target.y, target.z, fireBreathFX, 0x06, 0x00, true, fireBreathFXColor );

			// Base damage of fireball on dragon's current health. modify it based on
			var currentHealth = mNPC.health;
			var fireBreathDamage = Math.round( RandomNumber( currentHealth * 0.05, currentHealth * 0.075 ));

			// If NPC health is too low, Dragon Breath essentially does so little damage it's pointless. Let's throw in a minimum!
			if( fireBreathDamage < mNPC.maxhp / 50 )
				fireBreathDamage = Math.round( mNPC.maxhp );

			// Apply damage
			mNPC.target.Damage( fireBreathDamage, 5, mNPC );
		}

		mNPC.frozen = false;
		mNPC.isCasting = false;
	}
}

function GetFireBreathInfo( mNPC )
{
	var fireBreathActionID = 0;
	var fireBreathFrameCount = 7;
	var fireBreathFrameDelay = 0;
	var fireBreathFX = 0x36d4;
	var fireBreathFXColor = 0;
	var fireBreathSFX = 0;
	// Get details of dragon breath based on the NPC in question
	switch( mNPC.id )
	{
		case 0x0096: // Sea Serpent, 11 to 17 dmg at max health
		case 0x0091: // Deep Sea Serpent, 21 to 32 dmg at max health
			fireBreathActionID = 0x5;
			fireBreathFrameCount = 15;
			fireBreathSFX = 0x16a;
			break;
		case 0x00c9: // Hell Cat, 5 to 8 dmg at max health
		case 0x00be: // Fire Steed, 6 to 9 dmg at max health
		case 0x0061: // Hell Hounds, 8 to 11 dmg at max health
		case 0x00CE: // Lava Lizard, 8 to 11 dmg at max health
		case 0x007f: // Predator Hell Cat, 9 to 14 dmg at max health
		case 0x031a: // Swamp Dragon, 15 to 22 dmg at max health
		case 0x031f: // Armored Swamp Dragon, 15 to 22 dmg at max health
		case 0x0082: // Fire Gargoyle, 20 to 30 dmg at max health
		case 0x0074: // Nightmare, 26 to 39 dmg at max health
		case 0x00b1: // Nightmare, 26 to 39 dmg at max health
		case 0x00b2: // Nightmare, 26 to 39 dmg at max health
		case 0x00b3: // Nightmare, 26 to 39 dmg at max health
		case 0x0072: // Dark Steed, 26 to 39 dmg at max health
		case 0x0075: // Silver Steed, 26 to 39 dmg at max health
		case 0x0114: // Reptalon, 51 to 77 dmg at max health
			fireBreathActionID = 0x5;
			fireBreathFrameCount = 5;
			fireBreathFrameDelay = 2;
			fireBreathSFX = 0x357;
			break;
		case 0x0067: // Serpentine Dragon, 22 to 32 dmg at max health
		case 0x004d: // Kraken, 39 to 59 dmg at max health
			fireBreathActionID = 0x5;
			fireBreathFrameCount = 4;
			fireBreathFrameDelay = 3;
			fireBreathSFX = 0x16a;
			break;
		case 0x003c: // Drake (Gray), 22 to 32 dmg at max health
		case 0x003d: // Drake (Red), 22 to 32 dmg at max health
		case 0x003b: // Dragon (Red), 41 to 62 dmg at max health
		case 0x000c: // Dragon (Gray), 41 to 62 dmg at max health
		case 0x006a: // Shadow Wyrm, 50 to 75 dmg at max health
		case 0x0069: // Ancient Wyrm, 60 to 90 dmg at max health
			fireBreathActionID = 0x0c;
			fireBreathFrameCount = 15;
			fireBreathSFX = 0x16a;
			break;
		case 0x0068: // Skeletal Dragon, 52 to 77 dmg at max health
			fireBreathFX = 0x36D4;
			fireBreathFXColor = 0x047f;
			fireBreathSFX = 0x16a;
			fireBreathActionID = 0x5;
			fireBreathFrameCount = 4;
			fireBreathFrameDelay = 3;
			break;
	}

	mNPC.SetTag( "fbActID", fireBreathActionID );
	mNPC.SetTag( "fbFrCnt", fireBreathFrameCount );
	mNPC.SetTag( "fbFrDelay", fireBreathFrameDelay );
	mNPC.SetTag( "fbFX", fireBreathFX );
	mNPC.SetTag( "fbFXHue", fireBreathFXColor );
	mNPC.SetTag( "fbSFX", fireBreathSFX );
}
