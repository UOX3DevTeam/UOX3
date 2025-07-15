function onDamage(pBarracoon, pAttacker, damageValue, damageType)
{
	if (!ValidateObject(pBarracoon) || !ValidateObject(pAttacker))
		return true;


	let isPet = pAttacker.owner && ValidateObject(pAttacker.owner);
	let realAttacker = isPet ? pAttacker.owner : pAttacker;

	if (!ValidateObject(realAttacker))
		return true;

	if (HasSavageKinPaint(realAttacker))
		return true;


	let body = realAttacker.body;
	let isPoly = realAttacker.isPolymorphed;
	let isHumanForm = (body === 0x0190 || body === 0x0191);

	let ratCount = AreaCharacterFunction("CountBarracoonRat", pBarracoon, 60, null);

	if (ratCount < 10 && RandomNumber(1, 100) <= 10)
	{
		SummonRatmen(realAttacker, pBarracoon);
		pBarracoon.SoundEffect(0x3D, true);
		pBarracoon.TextMessage("*Barracoon plays a haunting tune...*");
	}

	if (pBarracoon.id != 0x002A && RandomNumber(1, 100) <= 10)
	{
		pBarracoon.id = 0x002A;
		pBarracoon.StartTimer(180000, 2, 3230); // timerID = 2
	}

	if (isPoly && !isHumanForm)
		return true;

	if (RandomNumber(1, 100) > 60)
		return true;

	// Store pet's original body
	if (isPet)
	{
		realAttacker.SetTag("OriginalBody", realAttacker.id);
	}
	
	// Apply polymorph into Ratman (0x002A)
	realAttacker.id = 0x002A;
	realAttacker.isPolymorphed = true;
	realAttacker.TextMessage("*You feel your form shifting into that of a ratman!*");

	// Start 3-minute revert timer
	realAttacker.StartTimer(180000, 1, 3230); // timerID = 1

	// Players only: criminal flag in Felucca
	if (!isPet)
	{
		realAttacker.criminal = true;
		realAttacker.TextMessage("*You have been flagged criminal!*");
	}

	return true;
}

function onTimer(pChar, timerID)
{
	if (timerID == 1)
	{
		if (!pChar.isPolymorphed)
			return;

		let isPet = pChar.owner && ValidateObject(pChar.owner);
		if (isPet)
		{
			// Pets: Restore original body from tag
			let originalBody = parseInt(pChar.GetTag("OriginalBody"), 10);
			if (!isNaN(originalBody))
			{
				pChar.id = originalBody;
				pChar.SetTag("OriginalBody", null);
			}
		}
		else
		{
			// Players: Revert to human male/female body
			pChar.id = pChar.orgID;//(pChar.gender == 0) ? 0x0190 : 0x0191;
		}

		pChar.isPolymorphed = false;
		pChar.TextMessage("*Your form returns to normal.*");
	}
	if (timerID == 2)
	{
		// Only revert if still disguised as a ratman
		if (pChar.id == 0x002A)
		{
			pChar.id = 0x0190;
			pChar.TextMessage("*Barracoon reveals his true form!*");
		}
	}
}

function HasSavageKinPaint(pChar)
{
	if (!ValidateObject(pChar))
		return false;

	return pChar.id === 0x00B7 || pChar.id === 0x00B8;
}

function SummonRatmen(attacker, barracoon)
{
	if (!ValidateObject(attacker))
		return;

	//let isPet = attacker.owner && ValidateObject(attacker.owner);
	//let realTarget = isPet ? attacker.owner : attacker;

	const ratmanTypes = [ "ratman", "ratmanarcher", "ratmanmage" ]; // your NPC section IDs
	let spawnCount = RandomNumber(3, 6);

	for (let i = 0; i < spawnCount; ++i)
	{
		let npcType = ratmanTypes[RandomNumber(0, ratmanTypes.length - 1)];

		let xOffset = RandomNumber(-2, 2);
		let yOffset = RandomNumber(-2, 2);
		let spawnX = attacker.x + xOffset;
		let spawnY = attacker.y + yOffset;
		let spawnZ = attacker.z;
		let worldNum = attacker.worldnumber;
		let instanceID = attacker.instanceID;

		let rat = SpawnNPC(npcType, spawnX, spawnY, spawnZ, worldNum, instanceID, false);
		if (ValidateObject(rat))
		{
			rat.TextMessage("*snarls*");
			rat.SetTag("barracoonSummoner", barracoon.serial.toString());
			rat.Wander( barracoon.x, barracoon.y, 10 );
		}
	}
}

function CountBarracoonRat(barracoon, potentialRat, sock)
{
	if (!ValidateObject(potentialRat) || !potentialRat.npc)
		return false;

	let tag = potentialRat.GetTag("barracoonSummoner");
	if (tag && tag == barracoon.serial.toString())
		return true;

	return false;
}

function _restorecontext_() {}