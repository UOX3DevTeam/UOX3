// Glacial Staff with 3 special spells, 2 of which are randomly enabled for any glacial staff upon creation
// Ice Ball - Des Corp Del
// Ice Strike - In Corp Del
// Paralyze - An Ex Del

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
function onSpeech( strSaid, pTalking, iTalkingTo )
{
	// If player says the words of power for a magic spell enabled on the glacial staff,
	// activate said spell, so the next time user double-clicks the staff, that's the spell
	// that will be cast
	var lHand = pUser.FindItemLayer( 0x02 );
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
}

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && ValidateObject( iUsed ) && iUsed.isItem )
	{
		var lHand = pUser.FindItemLayer( 0x02 );
		if( lHand != iUsed ) // will only work if glacial staff is equipped
			return false;

		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pUser.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
			return false;
		}

		if( iUsed.morez > 0 ) // Still has charges left
		{
			var targMsg = GetDictionaryEntry( 443, socket.language ); // What would you like to use that on?
			pUser.SetTag( "staffSerial", ( iUsed.serial ).toString() );
			socket.CustomTarget( 1, targMsg );
		}

	}
	return false;
}

function onCallback1( socket, myTarget )
{
	var mChar = socket.currentChar;
	if( ValidateObject( mChar ) && ValidateObject( myTarget ) && myTarget.isChar )
	{
		if( !myTarget.vulnerable )
			return;

		var glacialStaff = CalcItemFromSer( parseInt( mChar.GetTag( "staffSerial" )));
		var activeSpell = glacialStaff.morex;

		mChar.SetTimer( Timer.SPELLTIME, 1500 );
		mChar.isCasting = true;
		mChar.TurnToward( myTarget );
		mChar.DoAction( 17 );
		mChar.SetTag( "glacialTarg", myTarget.serial );

		switch( activeSpell )
		{
			case 0: // Ice Ball
				mChar.StartTimer( 1500, 0, true );
				break;
			case 1: // Ice Strike
				mChar.StartTimer( 1500, 1, true );
				break;
			case 2: // Paralyze
				mChar.StartTimer( 1500, 2, true );
				break;
			default:
				break;
		}
	}
}

function onTimer( mChar, timerID )
{
	if( !ValidateObject( mChar ) || !mChar.isChar || mChar.dead || !mChar.online )
		return;

	mChar.isCasting = false;
	var glacialStaff = CalcItemFromSer( parseInt( mChar.GetTag( "staffSerial" )));
	glacialStaff.morez--;
	if( glacialStaff.morez == 0 )
	{
		// Shatter glacial staff
		if( mChar.socket != null )
		{
			mChar.SoundEffect( 0x003e, true );
			mChar.socket.SysMessage( "All charges have been expended for the glacial staff, which shatters in your hands!" );
		}
		glacialStaff.Delete();
	}

	var targetChar = CalcCharFromSer( parseInt( mChar.GetTag( "glacialTarg" )));
	if( !ValidateObject( targetChar ) || !targetChar.isChar || targetChar.dead || ( !targetChar.npc && !targetChar.online ) || !targetChar.vulnerable )
	{
		mChar.TextMessage( "Nope 1" );
		mChar.TextMessage( targetChar );
		return;
	}

	if( WillResultInCriminal( mChar, targetChar ))
		mChar.criminal = true;

	switch( timerID )
	{
		case 0: // Ice Ball
			var iceBallFX = 0x36D4;
			DoMovingEffect( mChar, targetChar, iceBallFX, 0x06, 0x00, true, 0x047f, 0xa );
			if( targetChar.magicReflect )
			{
				DoMovingEffect( targetChar, mChar, iceBallFX, 0x06, 0x00, true, 0x047f, 0xa );
				targetChar = mChar;
			}
			if( CheckResist( 381, targetChar, 3 ))
			{
				targetChar.Damage( RandomNumber( 5, 7 ), 4, mChar, true );
			}
			else
			{
				targetChar.Damage( RandomNumber( 10, 15 ), 4, mChar, true );
			}
			break;
		case 1: // Ice Strike
			var iceStrikeFX = 0x3709;
			DoMovingEffect( targetChar, targetChar, iceStrikeFX, 0x06, 0x1e, false, 0x047f, 0xa );
			if( targetChar.magicReflect )
			{
				DoMovingEffect( mChar, mChar, iceStrikeFX, 0x06, 0x1e, false, 0x047f, 0xa );
				targetChar = mChar;
			}
			targetChar.SoundEffect( 0x0208, true );
			if( CheckResist( 881, targetChar, 7 ))
			{
				targetChar.Damage( RandomNumber( 20, 30 ), 4, mChar, true );
			}
			else
			{
				targetChar.Damage( RandomNumber( 10, 15 ), 4, mChar, true );
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
	var targSock = targetChar.socket;

	// Check which is higher between user's normal resist chance and a fallback value
	// To ensure user always has a chance of resisting, however small their resist skill (except at 0)
	var defaultChance = targetChar.skills.magicresistance / 5;
	var resistChance = targetChar.skills.magicresistance - ((( resistDifficulty - 20 ) / 5 ) + ( spellCircle * 5 ));
	if( defaultChance > resistChance )
	{
		resistChance = defaultChance;
	}

	if( RandomNumber( 1, 100 ) < resistChance / 10 )
	{
		if( targSock )
		{
			targSock.SysMessage( GetDictionaryEntry( 699, targSock.language )); // You feel yourself resisting magical energy!
		}
		resistSuccess = true;
	}
	else
	{
		resistSuccess = false;
	}

	return resistSuccess;
}