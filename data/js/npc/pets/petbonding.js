function onDeathBlow( killedPet, petKiller )
{
	if( killedPet.tamed && killedPet.GetTag( "isBondedPet" ))
	{
		var petOwner = killedPet.owner;
		var petCorpse = CreateBlankItem( null, killedPet, 1, killedPet.name, 0x2006, 0x0, "ITEM", false );
		petCorpse.amount = killedPet.id;
		petCorpse.dir = killedPet.dir;
		petCorpse.colour = killedPet.colour;
		petCorpse.decayable = true;

		TriggerEvent( 3108, "SendNpcGhostMode", petOwner.socket, 0, killedPet.serial, 1  );
		killedPet.SetTag( "PetsAI", killedPet.aitype.toString() );
		killedPet.SetTag( "PetsHue", killedPet.colour.toString() );
		killedPet.SetTag( "isPetDead", true );
		killedPet.aitype = 0;
		killedPet.health = 1;
		killedPet.target = null;
		killedPet.atWar = false;
		killedPet.attacker = null;

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

function inRange( pet, objInRange )
{
	if( pet.GetTag( "isPetDead" ) == true )
	{
		TriggerEvent( 3108, "SendNpcGhostMode", objInRange.socket, 0, pet.serial, 1  );
	}
}

function onCombatStart( pAttacker, pDefender )
{
	if( pDefender.GetTag( "isPetDead" ) == true )
	{
		var pAttackSock = pAttacker.socket;
		if( pAttackSock )
		{
			pAttackSock.SysMessage( "You can not perform beneficial acts on your target.");
		}
		return false;
	}

	return true;
}

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
			pSock.SysMessage( "You can not perform beneficial acts on your target." ); // You may not cast that on the Young.
			return 2;
		}
	}

	// By default, allow Young player to be target of spells from monsters/npcs
	return 0;
}

function onDamage(damaged, pAttacker, damageValue, damageType)
{
	if (!ValidateObject(damaged))
		return false;

	// If attacker is a pet, use the owner instead
	if (ValidateObject( pAttacker ) && pAttacker.npc && ValidateObject( pAttacker.owner ) && !pAttacker.owner.npc )
	{
		pAttacker = pAttacker.owner;
	}

	// Prevent damaging a bonded pet that is ghosted
	if( damaged.GetTag( "isPetDead" ))
	{
		var atkSock = ValidateObject(pAttacker) ? pAttacker.socket : null;
		if (atkSock)
			atkSock.SysMessage( "You can not perform beneficial acts on your target." );
		return false;
	}

	return true;
}

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

function onDropItemOnNpc(pDropper, pPet, iFood)
{
	var tameSkillRequired = pPet.skillToTame;
	var dropperTameSkill = pDropper.baseskills.animaltaming;

	if(( tameSkillRequired <= 291 || dropperTameSkill >= tameSkillRequired ) && pPet.GetTag( "isBondedPet" ) == false )
	{
		if( !pPet.GetTag( "bondingStarted" ))
		{
			pPet.SetTag( "bondingStarted", true );
			pPet.SetTag( "bondingPlayer", pDropper.serial );

			var bondingDelay = 3 * 24 * 60 * 60 * 1000; // 3 days
			pPet.StartTimer( 1000, 42, true ); // TimerID 42, callback to same script
		}
	}
	else
	{
		pDropper.socket.SysMessage( GetDictionaryEntry( 19313, pDropper.socket.language ));// Your pet cannot form a bond with you until your animal taming ability has risen.
	}

	iFood.Refresh()
	return 1;
}

function onTimer( timerObj, timerID )
{
	if( timerID == 42 && timerObj.npc && !timerObj.GetTag( "isBondedPet" ))
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
	var petrSerial = CalcCharFromSer( pUser.GetTempTag( "petSerial" ));
	if( pButton == 1 )
	{
		petrSerial.Delete();
	}
}

function _restorecontext_() {}