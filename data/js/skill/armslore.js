/// <reference path="../definitions.d.ts" />
// @ts-check
function SkillRegistration()
{
	RegisterSkill( 4, true );	// Arms Lore
}

/** @type { ( skillUse: BaseObject, skillUsed: number, objType: 0 | 1 ) => boolean } */
function onSkill( pUser, skillUsed, objType )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		pSock.CustomTarget( 0, GetDictionaryEntry( 855, pSock.language )); // What item do you wish to get information about?
	}

	return true;
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ourObj && ourObj.isItem && pUser )
	{
		var pLanguage = pSock.language;
		var ourDef = ourObj.Resist( 1 );
		var ourLoDmg = ourObj.lodamage;
		var ourHiDmg = ourObj.hidamage;

		if( ourDef == 0 && ourLoDmg == 0 && ourHiDmg == 0 )
		{
			pSock.SysMessage( GetDictionaryEntry( 1503, pLanguage )); // That does not appear to be a weapon.
		}
		else
		{
			var ourOwner = GetPackOwner( ourObj, 0 );
			if( ourOwner && ourOwner.isChar && ourOwner.serial == pUser.serial )
			{
				if( !pUser.CheckSkill( 4, 0, pUser.skillCaps.armslore ))
				{
					pSock.SysMessage( GetDictionaryEntry( 1504, pLanguage )); // You are not certain...
				}
				else
				{
					var ourMaxHP = ourObj.maxhp;
					var offset;
					var sysString = "";
					if( ourMaxHP )
					{
						var hpString;
						// Items HP's - 1 / by items total HP * 10 (0-3 = 0, 4 - 5 = 1, ect)
						offset = parseInt( parseFloat(( ourObj.health - 1 ) / ourMaxHP ) * 10 );
						if( offset >= 0 && offset <= 8 )
						{
							hpString = GetDictionaryEntry( 1515 - offset, pLanguage );
						}
						else
						{
							hpString = GetDictionaryEntry( 1506, pLanguage ); // is brand new.
						}
						sysString = hpString;
					}

					if( ourHiDmg )
					{
						var dmgString = "";
						if( pUser.skills.armslore > 750 )
						{
							// Poison
							if( ourObj.poison > 0 )
							{
								var pString;
								offset = ourObj.poison;
								if( offset > 0 && offset < 5 )
								{
									pString = GetDictionaryEntry( 1455 + offset, pLanguage );
								}
								else
								{
									pString = GetDictionaryEntry( 1459, pLanguage ); // glistens with very deadly poison.
								}
								dmgString = pString;
							}

							// Corrosion
							let corrosionLevel = ourObj.GetTag( "corrosionLevel" );
							if( corrosionLevel > 0 )
							{
								if( corrosionLevel >= 5 )
								{
									dmgString += " " + GetDictionaryEntry( 6319, pLanguage ); // It looks like acid is dripping from what is left of the blade.
								}
								else if( corrosionLevel == 4 )
								{
									dmgString += " " + GetDictionaryEntry( 6320, pLanguage ); // It looks as if a substance is eating away pieces of the blade.
								}
								else if( corrosionLevel == 3 )
								{
									dmgString += " " + GetDictionaryEntry( 6321, pLanguage ); // It looks as if this blade is covered with an acidic substance.
								}
								else if( corrosionLevel == 2 )
								{
									dmgString += " " + GetDictionaryEntry( 6322, pLanguage ); // It looks like this blade has a corrosive element on it.
								}
								else
								{
									dmgString += " " + GetDictionaryEntry( 6323, pLanguage ); // It looks like this blade has a light corrosive element on it.
								}
							}
						}

						// HiDamage + LoDamage / 10 ( 0-9 = 0, 10-19 = 1, ect )
						offset = parseInt(( ourHiDmg + ourLoDmg) / 10 );
						if( offset <= 5 )
						{
							dmgString = dmgString + " " + GetDictionaryEntry( 1522 - offset, pLanguage );
						}
						else
						{
							dmgString = dmgString + " " + GetDictionaryEntry( 1516, pLanguage ); // Would be extraordinarily deadly.
						}

						if( pUser.skills.armslore > 250 )
						{
							var spString;
							// Items Speed - 5 / 10 ( 0-14 = 0, 15-25 = 1, ect)
							offset = parseInt(( ourObj.speed - 5) / 10 );
							if( offset <= 2 )
							{
								spString = GetDictionaryEntry( 1526 - offset, pLanguage );
							}
							else
							{
								spString = GetDictionaryEntry( 1523, pLanguage ); // and is very fast.
							}
							dmgString = dmgString + " " + spString;
						}

						sysString = sysString + " " + dmgString;
					}
					else if( ourDef )
					{
						var defString;
						// Items Defense + 1 / 2 ( 0 = 0, 1-2 = 1, 3-4 = 2, ect)
						offset = parseInt(( ourDef + 1 ) / 2 );

						if( offset <= 6 )
						{
							defString = GetDictionaryEntry( 1534 - offset, pLanguage );
						}
						else
						{
							defString = GetDictionaryEntry( 1527, pLanguage ); // is superbly crafted to provide maximum protection.
						}

						sysString = sysString + " " + defString;
					}
					pSock.SysMessage( GetDictionaryEntry( 6003, pLanguage ) + " " + sysString ); // This item [is...]
				}
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 6004, pLanguage )); // You must be holding the item to examine it.
			}
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 1569, pSock.language )); // That is not a player!
	}
}
