/// <reference path="../definitions.d.ts" />
// @ts-check
// Glacial Staff with 3 special spells, 2 of which are randomly enabled for any glacial staff upon creation
// Ice Ball - Des Corp Del
// Ice Strike - In Corp Del
// Paralyze - An Ex Del

/** @type { ( thingCreated: BaseObject, thingType: 0 | 1 ) => void } */
function onCreateDFN( objMade, objType )
{
	if( objType == 0 && ValidateObject( objMade ))
	{
		// Set custom itemType to mark object as a glacial staff
		objMade.SetTag( "customType", 1 );

		// Select 2 out of 3 special magic spells upon creation
		var spellAvailability = new Array( 0, 1, 2 );

		// Select first spell
		var rndNum = RandomNumber( 0, 2 )
		spellAvailability.splice( rndNum, 1 );
		objMade.morex = rndNum;

		// Select second spell, after having removed the first from the available options
		rndNum = RandomNumber( 0, 1 );
		objMade.morey = spellAvailability[rndNum];

		// Set available charges on the staff (defaults to 25)
		objMade.morez = 25;
	}
}

// This event only triggers if ITEMSDETECTSPEECH setting in UOX.INI is enabled
/** @type { ( speech: string, personTalking: Character, talkingTo: BaseObject ) => null | undefined | number | boolean } */
function onSpeech( strSaid, pTalking, iTalkingTo )
{
	// If player says the words of power for a magic spell enabled on the glacial staff,
	// activate said spell, so the next time user double-clicks the staff, that's the spell
	// that will be cast
	var lHand = pTalking.FindItemLayer( 0x02 );
	if( lHand != iTalkingTo ) // will only work if glacial staff is equipped
		return false;

	var activeSpell = iTalkingTo.morex;
	var inActiveSpell = iTalkingTo.morey;
	if( inActiveSpell == 0 && strSaid == "Des Corp Del" )
	{
		iTalkingTo.morex = 0;
		iTalkingTo.morey = activeSpell;
	}
	else if( inActiveSpell == 1 && strSaid == "In Corp Del" )
	{
		iTalkingTo.morex = 1;
		iTalkingTo.morey = activeSpell;
	}
	else if( inActiveSpell == 2 && strSaid == "An Ex Del" )
	{
		iTalkingTo.morex = 2;
		iTalkingTo.morey = activeSpell;
	}
}

/** @type { ( equipper: Character, equipping: Item ) => boolean } */
function onEquip( pEquipper, iEquipped )
{
	if( !GetServerSetting( "ItemsDetectSpeech" ))
	{
		// If ITEMSDETECTSPEECH setting in UOX.INI is disabled, toggle between the two
		// enabled spells whenever the player equips the item
		var prevSpell = iEquipped.morex;
		iEquipped.morex = iEquipped.morey;
		iEquipped.morey = prevSpell;
	}
	return false;
}

/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;
	if( pSocket && ValidateObject( iUsed ) && iUsed.isItem )
	{
		var lHand = pUser.FindItemLayer( 0x02 );
		if( lHand != iUsed ) // will only work if glacial staff is equipped
			return false;

		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pSocket.SysMessage( GetDictionaryEntry( 1763, pSocket.language )); // That item must be in your backpack before it can be used.
			return false;
		}

		if( iUsed.morez > 0 ) // Still has charges left
		{
			var targMsg = GetDictionaryEntry( 443, pSocket.language ); // What would you like to use that on?
			pUser.SetTag( "staffSerial", ( iUsed.serial ).toString() );
			pSocket.CustomTarget( 1, targMsg );
		}

	}
	return false;
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback1( socket, myTarget )
{
	var pUser = socket.currentChar;
	if( ValidateObject( pUser ) && ValidateObject( myTarget ) && myTarget.isChar )
	{
		if( !myTarget.vulnerable )
			return;

		var glacialStaff = CalcItemFromSer( parseInt( pUser.GetTag( "staffSerial" )));
		var activeSpell = glacialStaff.morex;

		pUser.SetTimer( Timer.SPELLTIME, 1500 );
		pUser.isCasting = true;
		pUser.TurnToward( myTarget );
		pUser.DoAction( 17 );
		pUser.SetTag( "glacialTarg", myTarget.serial );

		switch( activeSpell )
		{
			case 0: // Ice Ball
				pUser.StartTimer( 1500, 0, true );
				break;
			case 1: // Ice Strike
				pUser.StartTimer( 1500, 1, true );
				break;
			case 2: // Paralyze
				pUser.StartTimer( 1500, 2, true );
				break;
		}
	}
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( pUser, timerID )
{
	var pSocket = pUser.socket;
	if( !ValidateObject( pUser ) || !pUser.isChar || pUser.dead || !pUser.online )
		return;

	pUser.isCasting = false;
	var glacialStaff = CalcItemFromSer( parseInt( pUser.GetTag( "staffSerial" )));
	glacialStaff.morez--;
	if( glacialStaff.morez == 0 )
	{
		// Shatter glacial staff
		if( pSocket != null )
		{
			pUser.SoundEffect( 0x003e, true );
			pSocket.SysMessage( GetDictionaryEntry( 2799, pSocket.language ); // All charges have been expended for the glacial staff, which shatters in your hands!
		}
		glacialStaff.Delete();
	}

	var targetChar = CalcCharFromSer( parseInt( pUser.GetTag( "glacialTarg" )));
	if( !ValidateObject( targetChar ) || !targetChar.isChar || targetChar.dead || ( !targetChar.npc && !targetChar.online ) || !targetChar.vulnerable )
	{
		pUser.TextMessage( "Nope 1" );
		pUser.TextMessage( targetChar );
		return;
	}

	if( WillResultInCriminal( mChar, targetChar ))
		pUser.criminal = true;

	switch( timerID )
	{
		case 0: // Ice Ball
			var iceBallFX = 0x36D4;
			DoMovingEffect( pUser, targetChar, iceBallFX, 0x06, 0x00, true, 0x047f, 0xa );
			if( targetChar.magicReflect )
			{
				DoMovingEffect( targetChar, pUser, iceBallFX, 0x06, 0x00, true, 0x047f, 0xa );
				targetChar = pUser;
			}
			if( CheckResist( 381, targetChar, 3 ))
			{
				targetChar.Damage( RandomNumber( 5, 7 ), 4, pUser, true );
			}
			else
			{
				targetChar.Damage( RandomNumber( 10, 15 ), 4, pUser, true );
			}
			break;
		case 1: // Ice Strike
			var iceStrikeFX = 0x3709;
			DoMovingEffect( targetChar, targetChar, iceStrikeFX, 0x06, 0x1e, false, 0x047f, 0xa );
			if( targetChar.magicReflect )
			{
				DoMovingEffect( pUser, pUser, iceStrikeFX, 0x06, 0x1e, false, 0x047f, 0xa );
				targetChar = pUser;
			}
			targetChar.SoundEffect( 0x0208, true );
			if( CheckResist( 881, targetChar, 7 ))
			{
				targetChar.Damage( RandomNumber( 20, 30 ), 4, pUser, true );
			}
			else
			{
				targetChar.Damage( RandomNumber( 10, 15 ), 4, pUser, true );
			}
			break;
		case 2: // Paralyze
			var effectDuration = 6;
			var resistDifficulty = 581;
			DoMovingEffect( targetChar, targetChar, 0x376A, 0x06, 0x1e, false, 0x047f, 0xa );
			DoTempEffect( 0, null, targetChar, 1, 0, effectDuration, resistDifficulty );
			targetChar.SoundEffect( 0x0204, true );
			break;
		default:
			return;
	}
}

function CheckResist( resistDifficulty, targetChar, spellCircle )
{
	// Perform a skill check for magic resistance
	var resistSuccess = targetChar.CheckSkill( 26, 80*spellCircle, 800+(80*spellCircle) );
	var pSocket = targetChar.socket;

	// Check which is higher between user's normal resist chance and a fallback value
	// To ensure user always has a chance of resisting, however small their resist skill (except at 0)
	var defaultChance = ( targetChar.skills.magicresistance + ( targetChar.race.magicResistance * 10 )) / 5;
	var resistChance = ( targetChar.skills.magicresistance + ( targetChar.race.magicResistance * 10 )) - ((( resistDifficulty - 20 ) / 5 ) + ( spellCircle * 5 ));
	if( defaultChance > resistChance )
	{
		resistChance = defaultChance;
	}

	if( RandomNumber( 1, 100 ) < resistChance / 10 )
	{
		if( pSocket )
		{
			pSocket.SysMessage( GetDictionaryEntry( 699, pSocket.language )); // You feel yourself resisting magical energy!
		}
		resistSuccess = true;
	}
	else
	{
		resistSuccess = false;
	}

	return resistSuccess;
}
