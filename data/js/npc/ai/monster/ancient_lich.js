/// <reference path="../../../definitions.d.ts" />
// @ts-check
// Ancient Lich can summon other undeads, and even transform into them to "hide"
const undeadSummonList = [
	"skeleton", "zombie", "spectre"
];

const lichSummonMinCooldown = 5;
const lichSummonMaxCount = 3;

/** @type { ( caster: Character, target: BaseObject, spellNum: number ) => number } */
function onSpellTargetSelect( pTarget, npcLich, spellID )
{
	let chanceToSummon = RandomNumber( 1, 100 );
	if( chanceToSummon > 5 )
	{
		if( ValidateSummonsList( npcLich ))
		{
			if( ValidateObject( pTarget ))
			{
				CastSummonUndead( npcLich, pTarget );
				// Replace spell being cast with a summon spell
				return 2;
			}
		}
	}

	// Continue like normal
	return 0;
}

function ValidateSummonsList( npcLich )
{
	// Fetch list of summons, and validate them before allowing/disallowing summoning more
	let summonList = [];
	let summonListArrayString = npcLich.GetTag( "summonList" );
	if( summonListArrayString != 0 && typeof summonListArrayString === 'string' && summonListArrayString.length > 0 )
	{
		summonList = summonListArrayString.split(",");
		if( summonList.length > 0 )
		{
			for( let i = summonList.length - 1; i >= 0; i-- )
			{
				let tempUndead = CalcCharFromSer( parseInt( summonList[i] ));
				if( !ValidateObject( tempUndead ))
				{
					summonList.splice( i, 1 ); // Remove 1 element at index i
				}
			}
			npcLich.SetTag( "summonList", summonList.join( "," ));
		}
	}

	if( summonList.length == 0 || summonList.length < lichSummonMaxCount )
		return true;
	else
		return false;
}

function CastSummonUndead( npcLich, pTarget )
{
	// Ensure lich won't cast another spell while summoning
	npcLich.SetTimer( Timer.SPELLTIME, 2 );

	// Ensure lich won't move while summoning
	npcLich.SetTimer( Timer.MOVETIME, 2 );

	// Play FX/SFX associated with Summon Creature spell
	var spellNum = 40; // Summon Creature

	// Get Spell object based on ID
	var summonSpell = Spells[spellNum];

	// Play Summon Creature STATFX on the lich
	npcLich.SpellStaticEffect( summonSpell );

	// Play SFX associated with Summon Creature spell
	npcLich.SoundEffect( summonSpell.soundEffect, true );

	// Play an animation on lich
	npcLich.DoAction( 0x0c );

	SummonUndead( npcLich, pTarget );
}

function SummonUndead( npcLich, pTarget )
{
	// Find a valid location to spawn an undead
	var npcWorldNumber = npcLich.worldnumber;
	var npcInstanceID = npcLich.instanceID;
	var spawnLocations = [];
	const TF_SURFACE = 9;
	for( let x = -1; x <= 1; x++ )
	{
		for( let y = -1; y <= 1; y++ )
		{
			let targX = npcLich.x + x;
			let targY = npcLich.y + y;
			let targZ = GetMapElevation( targX, targY, npcLich.worldnumber );
			if( targX == npcLich.x && targY == npcLich.y )
				continue; // Same spot as lich

			// Check for Character blocking
			if( DoesCharacterBlock( targX, targY, targZ, npcWorldNumber, npcInstanceID ))
			{
				continue;
			}

			// Check for Map blocking
			if( DoesMapBlock( targX, targY, targZ, npcWorldNumber, true, false, false, false ))
			{
				continue;
			}

			// Check for Statics blocking
			if( DoesStaticBlock( targX, targY, targZ, npcWorldNumber, true ))
			{
				continue;
			}

			// Check for Dynamics blocking
			if( DoesDynamicBlock( targX, targY, targZ, npcWorldNumber, npcInstanceID, true, false, false, false ))
			{
				continue;
			}

			// Nothing seems to be blocking - great!
			spawnLocations.push( [targX, targY, targZ ] );
		}
	}

	if( spawnLocations.length > 0 )
	{
		// Choose random spawn location from list
		let spawnLocIndex = RandomNumber( 0, spawnLocations.length - 1 );
		var spawnLoc = spawnLocations[spawnLocIndex];

		// Spawn Undead
		var newUndead = SpawnNPC( undeadSummonList[RandomNumber( 0, undeadSummonList.length - 1 )], spawnLoc[0], spawnLoc[1], spawnLoc[2], npcWorldNumber, npcInstanceID );
		if( ValidateObject( newUndead ))
		{
			// Add newUndead.serial to tag on npcLich with comma-separated serials
			let summonList = [];
			let summonListArrayString = npcLich.GetTag( "summonList" );
			if( summonListArrayString != 0 && typeof( summonListArrayString ) === 'string' && summonListArrayString.length > 0 )
			{
				summonList = summonListArrayString.split( "," );
			}

			summonList.push( newUndead.serial.toString() )
			npcLich.SetTag( "summonList", summonList.join( "," ));

			// Start a timer on lich to prevent additional summoning before min 'cooldown has passed
			npcLich.StartTimer( lichSummonMinCooldown, 1, this.script_id );

			// Make newly spawned undead attack lich's target
			if( ValidateObject( npcLich.target ))
			{
				newUndead.InitiateCombat( npcLich.target );
			}

			// Add random chance to turn lich into one of the NPCs spawned
			// First, remove the last location used to spawn an NPC
			if( summonList.length == lichSummonMaxCount )
			{
				spawnLocations.splice( spawnLocIndex, 1 ); // Remove 1 element at index i
				if( spawnLocations.length > 0 )
				{
					// Store original info about lich
					npcLich._origID = npcLich.id;
					npcLich._origName = npcLich.name;
					npcLich._origAIType = npcLich.aitype;
					npcLich._origNpcFlag = npcLich.npcFlag;

					// Morph into the summoned creature
					npcLich.id = newUndead.id;
					npcLich.name = newUndead.name;
					npcLich.aitype = newUndead.aitype;
					npcLich.npcFlag = newUndead.npcFlag

					// Teleport morphed lich to confuse players
					spawnLocIndex = RandomNumber( 0, spawnLocations.length - 1 );
					spawnLoc = spawnLocations[spawnLocIndex];
					npcLich.Teleport( spawnLoc[0], spawnLoc[1], spawnLoc[2] );
				}
			}
		}
	}
}

/** @type { ( attacker: Character, defender: Character ) => boolean } */
function onCombatEnd( npcLich, pTarget )
{
	// Restore lich to original appearance
	if( typeof( npcLich._origID ) != "undefined" )
	{
		npcLich.id = npcLich._origID;
		npcLich.name = npcLich._origName;
		npcLich.aitype = npcLich._origAIType;
		npcLich.npcFlag = npcLich._origNpcFlag;
	}
	return true;
}

/** @type { ( mKilled: Character, mKiller: Character ) => boolean } */
function onDeathBlow( npcLich, pKiller )
{
	// Call onCombatEnd to revert lich back to original form before dying
	onCombatEnd( npcLich, pKiller );
	return true;
}
