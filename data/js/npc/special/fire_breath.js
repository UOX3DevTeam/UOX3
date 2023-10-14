// Fire Breath
const fireBreathDelay = 5;
const fireBreathMaxRange = 12;
const fireBreathManaCost = 30;

function onAISliver( mNPC )
{
	var nextFireBallTime = parseInt( mNPC.GetTag( "nextFB" ));
	if( mNPC.target != null && ( nextFireBallTime < GetCurrentClock() || nextFireBallTime > GetCurrentClock() + ( fireBreathDelay * 2000 )) && mNPC.InRange( mNPC.target, fireBreathMaxRange ))
	{
		var chanceOfFireBreath = mNPC.GetTag( "fbChance" );
		if( mNPC.mana >= fireBreathManaCost && RandomNumber( 1, 2000 ) <= chanceOfFireBreath )
		{
			// Cast Fire Breath, then reset chance to cast back to 0
			chanceOfFireBreath = 1;
			nextFireBallTime = (( RandomNumber( fireBreathDelay, fireBreathDelay * 2 ) * 1000 ) + GetCurrentClock() );
			mNPC.SetTag( "fbChance", chanceOfFireBreath );
			mNPC.SetTag( "nextFB", nextFireBallTime.toString() );
			if( !mNPC.CanSee( mNPC.target ))
			{
				return;
			}
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
		mNPC.StartTimer( 100, 0, SCRIPT.script_id );
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
		mNPC.StartTimer( 1400, 1, SCRIPT.script_id );
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
	switch( mNPC.sectionID )
	{
		case "seaserpent": // Sea Serpent, 11 to 17 dmg at max health
		case "deepseaserpent": // Deep Sea Serpent, 21 to 32 dmg at max health
			fireBreathActionID = 0x5;
			fireBreathFrameCount = 15;
			fireBreathSFX = 0x16a;
			break;
		case "hellcat": // Hell Cat, 5 to 8 dmg at max health
		case "firesteed": // Fire Steed, 6 to 9 dmg at max health
		case "hellhound": // Hell Hound, 8 to 11 dmg at max health
		case "lavalizard": // Lava Lizard, 8 to 11 dmg at max health
		case "largehellcat": // Predator Hell Cat, 9 to 14 dmg at max health
		case "swampdragon": // Swamp Dragon, 15 to 22 dmg at max health
		case "armoredswampdragon": // Armored Swamp Dragon, 15 to 22 dmg at max health
		case "firegargoyle": // Fire Gargoyle, 20 to 30 dmg at max health
		case "nightmare": // Nightmare, 26 to 39 dmg at max health
		case "darknightmare": // Nightmare, 26 to 39 dmg at max health
		case "manenightmare": // Nightmare, 26 to 39 dmg at max health
		case "purenightmare": // Nightmare, 26 to 39 dmg at max health
		case "darksteed": // Dark Steed, 26 to 39 dmg at max health
		case "silversteed": // Silver Steed, 26 to 39 dmg at max health
		case "reptalon": // Reptalon, 51 to 77 dmg at max health
			fireBreathActionID = 0x5;
			fireBreathFrameCount = 5;
			fireBreathFrameDelay = 2;
			fireBreathSFX = 0x357;
			break;
		case "serpentinedragon": // Serpentine Dragon, 22 to 32 dmg at max health
		case "kraken": // Kraken, 39 to 59 dmg at max health
			fireBreathActionID = 0x5;
			fireBreathFrameCount = 4;
			fireBreathFrameDelay = 3;
			fireBreathSFX = 0x16a;
			break;
		case "graydrake": // Drake (Gray), 22 to 32 dmg at max health
		case "reddrake": // Drake (Red), 22 to 32 dmg at max health
		case "reddragon": // Dragon (Red), 41 to 62 dmg at max health
		case "graydragon": // Dragon (Gray), 41 to 62 dmg at max health
		case "shadowwyrm": // Shadow Wyrm, 50 to 75 dmg at max health
		case "ancientwyrm": // Ancient Wyrm, 60 to 90 dmg at max health
			fireBreathActionID = 0x0c;
			fireBreathFrameCount = 15;
			fireBreathSFX = 0x16a;
			break;
		case "skeletaldragon": // Skeletal Dragon, 52 to 77 dmg at max health
			fireBreathFX = 0x36D4;
			fireBreathFXColor = 0x047f;
			fireBreathSFX = 0x16a;
			fireBreathActionID = 0x5;
			fireBreathFrameCount = 4;
			fireBreathFrameDelay = 3;
			break;
		case "chaosdragoon": // mounted on armored swamp dragon
		case "chaosdragoonelite": // mounted on armored swamp dragon
		case "m_chaosdragoon": // mounted on armored swamp dragon
		case "m_chaosdragoonelite": // mounted on armored swamp dragon
		case "f_chaosdragoon": // mounted on armored swamp dragon
		case "f_chaosdragoonelite": // mounted on armored swamp dragon
			fireBreathSFX = 0x357;
			fireBreathActionID = 0x1B;
			break;
	}

	mNPC.SetTag( "fbActID", fireBreathActionID );
	mNPC.SetTag( "fbFrCnt", fireBreathFrameCount );
	mNPC.SetTag( "fbFrDelay", fireBreathFrameDelay );
	mNPC.SetTag( "fbFX", fireBreathFX );
	mNPC.SetTag( "fbFXHue", fireBreathFXColor );
	mNPC.SetTag( "fbSFX", fireBreathSFX );
}

function _restorecontext_() {}
