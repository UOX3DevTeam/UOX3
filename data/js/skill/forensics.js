function SkillRegistration()
{
	RegisterSkill( 19, true );	// Forensics
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		pSock.CustomTarget( 0, GetDictionaryEntry( 9279, pSock.language )); // Select what you want to examine.
	}

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( ourObj ))
		return;

	if( ourObj.isItem )
	{
		var pLanguage = pSock.language;
		if( !ourObj.InRange( pUser, 7 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 393, pLanguage )); // That is too far away.
		}
		else if( ourObj.corpse && pUser.CheckSkill( 19, 0, 550 )) // Skill gain from corpses only possible until 55.0
		{
			pSock.SysMessage( GetDictionaryEntry( 6007, pLanguage )); // You examine the body..."

			// How old is the corpse?
			var timeSinceDeath = parseInt( GetCurrentClock() / 1000) - parseInt( ourObj.tempTimer );
			if( timeSinceDeath > 180 )
			{
				pSock.SysMessage( GetDictionaryEntry( 6008, pLanguage )); // It stinks quite a lot by now.
			}
			else if( timeSinceDeath > 60 )
			{
				pSock.SysMessage( GetDictionaryEntry( 6009, pLanguage )); // It is beginning to smell a bit.
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 6010, pLanguage )); // It looks to have been freshly... planted.
			}

			// Who killed it?
			var kObj = CalcCharFromSer( ourObj.morex );
			if( ValidateObject( kObj ))
			{
				var tempName = GetDictionaryEntry( 6011, pLanguage ); // The killer was %s.
				tempName = ( tempName.replace( /%s/gi, kObj.name ));
				pSock.SysMessage( tempName );
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 6012, pLanguage )); // The killer left no traces for you to find.
			}

			// Who looted it?
			var looterChar = CalcCharFromSer( ourObj.GetTempTag( "lootedBy" ));
			if( ValidateObject( looterChar ))
			{
				var tempLootedMsg = GetDictionaryEntry( 9280, pLanguage ); // The corpse was last looted by %s.
				tempLootedMsg = tempLootedMsg.replace( /%s/gi, looterChar.name );
				pSock.SysMessage( tempLootedMsg );
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 9281, pLanguage )); // The corpse has not been desecrated.
			}
		}
		else if( ourObj.type == 1 || ourObj.type == 12 || ourObj.type == 63 )
		{
			// Examine who last picked a lock (skillbased based on lockpicker's skill)
			var lockPickerSer = ourObj.GetTempTag( "lockPickedBy" );
			var lockPickedSkill = ourObj.GetTempTag( "lockPickedSkill" );
			if( pUser.CheckSkill( 19, 0, lockPickedSkill ))
			{
				var lockPicker = CalcCharFromSer( parseInt( lockPickedBy ));
				if( ValidateObject( lockPicker ))
				{
					var tempMsg = GetDictionaryEntry( 9282, pSock.language ); // This lock was last picked by %s.
					tempMsg = tempMsg.replace( /%s/gi, lockPicker.name );
					pSock.SysMessage( tempMsg );
				}
			}
			else
			{
				pSock.SysMessage( GetDictionaryEntry( 9283, pSock.language )); // You notice nothing unusual.
			}
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 9283, pSock.language )); // You notice nothing unusual.
		}
	}
	else
	{
		// Difficulty of detecting a thieves guild member depends on the thief's stealing skill
		if( pUser.CheckSkill( 19, 0, ourObj.skills.stealing ) && ourObj.npcGuild == 3 ) // Member of Thieves Guild
		{
			pSock.SysMessage( GetDictionaryEntry( 9284, pSock.language )); // This individual is a thief!
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 9283, pSock.language )); // You notice nothing unusual.
		}
	}
}
