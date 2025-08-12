/// <reference path="../../definitions.d.ts" />
// @ts-check
/** @type { ( sourceObj: BaseObject, CharInRange: Character, rangeToChar: number, oldCharX: number, oldCharY: number ) => void } */
function onMoveDetect( iObj, pChar, rangeToChar, oldCharX, oldCharY )
{
	if( !ValidateObject( pChar ) || !pChar.isChar || pChar.npc || pChar.dead || pChar.isGM || pChar.isCounselor || pChar.visible == 3 )
		return true;

	// Fetch settings from MORE value
	var triggerRange = parseInt( iObj.GetMoreVar( "more", 1 ));
	let sfxChance = parseInt( iObj.GetMoreVar( "more", 2 ));

	// Fetch effect tags
	let inactiveID = parseInt( iObj.GetTag( "inactiveID" ));
	let activeID = parseInt( iObj.GetTag( "activeID" ));
	let inRangeSFX = parseInt( iObj.GetTag( "inRangeSFX" ));
	let outOfRangeSFX = parseInt( iObj.GetTag( "outOfRangeSFX" ));

	if( rangeToChar < triggerRange && DistanceBetween( iObj.x, iObj.y, oldCharX, oldCharY ) >= triggerRange )
	{
		// Change visual, if "active" visual is defined
		if( activeID != 0 && iObj.id != activeID )
		{
			iObj.id = activeID;
			iObj.Refresh();
			if( iObj.id != 0x1 )
			{
				iObj.visible = 0;
			}
		}

		// Play sound, if one is defined
		if( inRangeSFX != 0 && RandomNumber( 1, 100 ) <= sfxChance )
		{
			// Only play it for character who triggered effect
			pChar.SoundEffect( inRangeSFX, false );
		}
	}
	else if( rangeToChar == triggerRange && DistanceBetween( iObj.x, iObj.y, oldCharX, oldCharY ) < triggerRange )
	{
		// Change visual, if "inactive" visual is defined
		if( inactiveID != 0 && iObj.id != inactiveID )
		{
			// Only deactivate item if no other players are in range
			var charsInRange = AreaCharacterFunction( "FindCharsInRange", iObj, triggerRange - 1 );
			if( charsInRange == 0 )
			{
				iObj.id = inactiveID;
				iObj.Refresh();
				if( iObj.id == 0x1 ) // no draw tile
				{
					iObj.visible = 3; // Hide it, just in case some clients display it
				}
			}
		}

		// Play out-of-range SFX, if one is defined
		if( outOfRangeSFX != 0 && RandomNumber( 1, 100 ) <= sfxChance )
		{
			// Only play it for character who triggered effect
			pChar.SoundEffect( outOfRangeSFX, false );
		}
	}
	return true;
}

function FindCharsInRange( srcItem, trgChar, pSock )
{
	if( ValidateObject( trgChar ) && trgChar.isChar && !trgChar.npc
		&& !trgChar.dead && !trgChar.isGM && !trgChar.isCounselor && trgChar.visible != 3 )
	{
		return true;
	}
	return false;
}
