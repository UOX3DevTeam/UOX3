/// <reference path="../../definitions.d.ts" />
// @ts-check
// Enables pet bonding through feeding. When true, pets must be fed after taming to start the 
// 7-day bonding countdown. Without feeding, bonding will not begin.
const enablePetBondingViaFood = true;

/** @type { ( mKilled: Character, mKiller: Character ) => boolean } */
function onDeathBlow( killedPet, petKiller )
{
	if( killedPet.tamed && killedPet.GetTag( "isBondedPet" ))
	{
		var petOwner = killedPet.owner;
		var petCorpse = CreateBlankItem( null, killedPet, 1, killedPet.name, 0x2006, 0x0, "ITEM", false );
		petCorpse.amount = killedPet.id;
		petCorpse.dir = killedPet.dir;
		petCorpse.colour = killedPet.colour;
		petCorpse.type = 1;
		petCorpse.corpse = true;
		petCorpse.carveSection = 26;
		petCorpse.decaytime = 300;
		petCorpse.decayable = true;
		petCorpse.maxItems = 150;
		petCorpse.wipable = true;
		petCorpse.weightMax = 50000

		TriggerEvent( 3108, "SendNpcGhostMode", petOwner.socket, 0, killedPet.serial, 1  );
		killedPet.SetTag( "PetAI", killedPet.aitype.toString() );
		killedPet.SetTag( "PetHue", killedPet.colour.toString() );
		killedPet.SetTag( "isPetDead", true );
		killedPet.SetTag( "playTimeAtDeath", petOwner.playTime ); // store current playtime
		killedPet.SetTempTag( "bondedPetDeathTime", GetCurrentClock().toString() );
		killedPet.colour = 0x3e6;
		killedPet.aitype = 0;
		killedPet.health = 1;
		killedPet.target = null;
		killedPet.atWar = false;
		killedPet.attacker = null;

		// Start immediate, repeating timer (30 mins) to monitor playtime
		killedPet.StartTimer( 30 * 60 * 1000, 32, 3107 );

		// Start one-time 7-day fallback timer
		killedPet.StartTimer( 7 * 24 * 60 * 60 * 1000, 33, 3107 );

		if( ValidateObject( petKiller ))
		{
			petKiller.target = null;
			petKiller.attacker = null;
			petKiller.atWar = false;
		}

		if( petOwner != null )
		{
			killedPet.Follow( petOwner );
		}

		return false;
	}
	return true;
}

/** @type { ( targSock: Socket, objColliding: Character, objCollideWith: BaseObject ) => boolean } */
function onCollide( socket, objCollider, objCollideWith )
{
	if( objCollideWith.GetTag( "isPetDead" ) == true )
	{
		return false;
	}
	return true;
}

/** @type { ( srcObj: BaseObject, objInRange: BaseObject ) => void } */
function inRange( pet, objInRange )
{
	if (objInRange.socket == null)
	{
		return;
	}
	if( pet.GetTag( "isPetDead" ) == true )
	{
		TriggerEvent( 3108, "SendNpcGhostMode", objInRange.socket, 0, pet.serial, 1  );
	}
}

/** @type { ( srcObj: BaseObject, objVanish: BaseObject ) => void } */
function outOfRange( pet, objVanish )
{
	if( objVanish.socket == null )
	{
		return;
	}
	if( pet.GetTag( "isPetDead" ) == true )
	{
		TriggerEvent( 3108, "SendNpcGhostMode", objVanish.socket, 0, pet.serial, 1  );
	}
}

/** @type { ( attacker: Character, defender: Character ) => boolean } */
function onCombatStart( pAttacker, pDefender )
{
	// If the target is a dead pet, block combat and reset NPC state
	if( pDefender.GetTag( "isPetDead" ) == true )
	{
		// Handle for player attacker
		var pAttackSock = pAttacker.socket;
		if( pAttackSock )
		{
			pAttackSock.SysMessage( GetDictionaryEntry( 19323, pDropper.socket.language )); // You can not perform beneficial acts on your target.

		}

		pAttacker.target = null;
		pAttacker.attacker = null;
		pAttacker.atWar = false;

		pDefender.target = null;
		pDefender.attacker = null;
		pDefender.atWar = false;

		return false; // Block combat
	}

	return true;
}

/** @type { ( attacker: Character, defender: Character, hitStatus: boolean, hitLoc: number, damageDealt: number ) => void } */
function onAttack( pAttacker, pDefender, hitStatus, hitLoc, damageDealt )
{
	// Block attack if defender is a dead pet
	if( pDefender.GetTag( "isPetDead" ) == true )
	{
		// Notify attacker if player
		var pAttackSock = pAttacker.socket;
		if( pAttackSock )
		{
			pAttackSock.SysMessage( GetDictionaryEntry( 19323, pDropper.socket.language )); // You can not perform beneficial acts on your target.
		}

		pAttacker.target = null;
		pAttacker.attacker = null;
		pAttacker.atWar = false;

		pDefender.target = null;
		pDefender.attacker = null;
		pDefender.atWar = false;
	}
}

/** @type { ( target: BaseObject, caster: Character, spellNum: number ) => number } */
function onSpellTarget( myTarget, pCaster, spellID )
{
	// We don't care if caster and target is the same - allow it!
	if( myTarget == pCaster )
		return 2;

	if( myTarget.GetTag( "isPetDead" ) == true )
	{
		var pSock = pCaster.socket;
		if( pSock != null )
		{
			pSock.SysMessage( GetDictionaryEntry( 19323, pDropper.socket.language )); // You can not perform beneficial acts on your target.
			return 2;
		}

		pCaster.target = null;
		pCaster.attacker = null;
		pCaster.atWar = false;

		myTarget.target = null;
		myTarget.attacker = null;
		myTarget.atWar = false;
	}

	// By default, allow Young player to be target of spells from monsters/npcs
	return 0;
}

/** @type { ( damaged: Character, attacker: Character, damageValue: number, damageType: WeatherType ) => boolean } */
function onDamage( damaged, pAttacker, damageValue, damageType )
{
	if (!ValidateObject( damaged ))
		return false;

	// If attacker is a pet, use the owner instead
	if( ValidateObject( pAttacker ) && pAttacker.npc && ValidateObject( pAttacker.owner ) && !pAttacker.owner.npc )
	{
		pAttacker = pAttacker.owner;
	}

	// Prevent damaging a bonded pet that is ghosted
	if( damaged.GetTag( "isPetDead" ))
	{
		var atkSock = ValidateObject( pAttacker ) ? pAttacker.socket : null;
		if( atkSock )
		{
			atkSock.SysMessage( GetDictionaryEntry( 19323, pDropper.socket.language ));// You can not perform beneficial acts on your target.
		}

		pAttacker.target = null;
		pAttacker.attacker = null;
		pAttacker.atWar = false;

		damaged.target = null;
		damaged.attacker = null;
		damaged.atWar = false;
		return false;
	}

	return true;
}

/** @type { ( currChar: Character, targChar: Character ) => boolean } */
function onCharDoubleClick( pUser, pet )
{
	if( pet.GetTag( "isPetDead" ) == true )
	{
		return false;
	}
	return true;
}

function onReleasePet( pUser, pet )
{
	if( pet.GetTag( "isPetDead" ) == true )
	{
		var myGump = new Gump;
		myGump.AddPage( 0 );
		myGump.AddBackground( 0, 0, 270, 120, 0x13BE );

		myGump.AddXMFHTMLGump( 10, 10, 250, 75, 1049669, true, false ); // <div align=center>Releasing a ghost pet will destroy it, with no chance of recovery.  Do you wish to continue?</div>

		myGump.AddXMFHTMLGump(55, 90, 75, 20, 1011011, false, false ); // CONTINUE
		myGump.AddButton( 20, 90, 0xFA5, 0xFA7, 1, 0, 1 );

		myGump.AddXMFHTMLGump(170, 90, 75, 20, 1011012, false, false ); // CANCEL
		myGump.AddButton( 135, 90, 0xFA5, 0xFA7, 1, 0, 0 );
		myGump.Send( pUser );
		myGump.Free();

		pUser.SetTempTag( "petSerial", pet.serial );
		return false;
	}
	return true;
}

/** @type { ( srcChar: Character, targChar: Character, i: Item ) => number } */
function onDropItemOnNpc( pDropper, pPet, iFood )
{
	if( pPet.GetTag( "isPetDead" ) == true )
	{
		iFood.Refresh()
		return 0;
	}

	if( enablePetBondingViaFood && !pPet.GetTag( "bondingStarted" ))
	{
		StartBonding( pDropper, pPet );
	}
	iFood.Refresh()
	return 1;
}

function StartBonding( pUser, pPet )
{
	if( !enablePetBondingViaFood )
	{
		return;
	}
	else
	{
		var tameSkillRequired = pPet.skillToTame;
		var pUserTameSkill = pUser.baseskills.animaltaming;
	
		if( tameSkillRequired <= 291 || pUserTameSkill >= tameSkillRequired )
		{
			if( !pPet.GetTag( "bondingStarted" ))
			{
				pPet.SetTag( "bondingStarted", true );
				pPet.SetTag( "bondingPlayer", pUser.serial );

				var bondingDelay = 604800000; // 7 days in milliseconds
				pPet.StartTimer( bondingDelay, 42, 3107 );
			}
		}
		else
		{
			pUser.socket.SysMessage( GetDictionaryEntry( 19313, pUser.socket.language ));// Your pet cannot form a bond with you until your animal taming ability has risen.
		}
	}
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( timerObj, timerID )
{
	if( timerID == 32 )
	{
		if( !timerObj.GetTag( "isPetDead" ))
		{// Pet has been resurrected, stop checking.
			return;
		}

		var petOwner = timerObj.owner;
		if( !ValidateObject( petOwner ))
		{
			timerObj.Delete();
			return;
		}

		var playTimeAtDeath = parseInt( timerObj.GetTag( "playTimeAtDeath" ));
		var currentPlayTime = petOwner.playTime;
		var hoursPlayedSinceDeath = ( currentPlayTime - playTimeAtDeath ) / 60;

		if( hoursPlayedSinceDeath >= 24 ) // 24 hours of playtime since death
		{
			petOwner.RemoveFollower( timerObj );
			petOwner.controlSlotsUsed = Math.max( 0, petOwner.controlSlotsUsed - timerObj.controlSlots );

			Console.Log(
				"Pet [" + timerObj.name + "] (Serial: " + timerObj.serial.toString() + 
				") owned by " + petOwner.name + " (Serial: " + petOwner.serial.toString() + 
				") was auto-deleted after 24 hours of active character playtime without resurrection." );

			timerObj.Delete();
		}
		else
		{
			// Recheck again in 30 minutes
			timerObj.StartTimer( 30 * 60 * 1000, 32, 3107 );
		}
	}
	if( timerID == 33 )
	{
		// Fallback: force delete if pet is still dead after 7 real days
		if( !timerObj.GetTag( "isPetDead" ))
			return;

		var petOwner = timerObj.owner;
		if( ValidateObject( petOwner ))
		{
			petOwner.RemoveFollower( timerObj );
			petOwner.controlSlotsUsed = Math.max( 0, petOwner.controlSlotsUsed - timerObj.controlSlots );
		}

		Console.Log(
			"Pet [" + timerObj.name + "] (Serial: " + timerObj.serial.toString() +
			") was force-deleted after 7 days of inactivity." );

		timerObj.Delete();
	}
	if( timerID == 42 )
	{
		timerObj.SetTag( "isBondedPet", true );
		timerObj.Refresh();

		var ownerSerial = timerObj.GetTag( "bondingPlayer" );
		if( ownerSerial )
		{
			var owner = CalcCharFromSer( ownerSerial );
			if (ValidateObject( owner ))
			{
				owner.socket.SysMessage( GetDictionaryEntry( 19308, owner.socket.language )); // Your pet has bonded with you!
			}
		}
		timerObj.SetTag( "bondingStarted", null );
		timerObj.SetTag( "bondingPlayer", null );
	}
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	var petSerial = CalcCharFromSer( pUser.GetTempTag( "petSerial" ));
	if( pButton == 1 )
	{
		// Remove pet as an active follower
		pUser.RemoveFollower( petSerial );
		// Reduce control slots in use for player by amount occupied by pet that was stabled
		pUser.controlSlotsUsed = Math.max( 0, pUser.controlSlotsUsed - petSerial.controlSlots );
		Console.Log( pUser.name + " (Serial: " + pUser.serial.toString() + ") released dead pet [" + petSerial.name + "] (Serial: " + petSerial.serial.toString() + ")" );
		petSerial.Delete();
	}
}
