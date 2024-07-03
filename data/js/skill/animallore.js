function SkillRegistration()
{
	RegisterSkill( 2, true );	// Animal Lore
}

const coreShardEra = EraStringToNum(GetServerSetting("CoreShardEra"));

// If disabled, a custom UOX3-specific, old school "stone"-themed gump shows instead of the scroll-based style for Animal Lore results
const osiGumpEnabled = 1;

// If enabled, a creature's HP/Mana/Stamina regen rate will show, along with Necromancy skill, and damage types (Fire vs Cold, etc)
const aosEnabled = false;

// If enabled, a creature's skill in Bushido and Ninjitsu will show
const seEnabled = false;

// If enabled, a creature's skill in Mysticism, Focus and Spellweaving will show
const saEnabled = false;

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		if( aosEnabled || coreShardEra >= EraStringToNum( "aos" ))
		{
			//What animal should I look at ?
			pSock.CustomTarget( 0, GetDictionaryEntry( 2130, pSock.language ));
		}
		else
	    {
		    //What animal do you wish to get information about?
		    pSock.CustomTarget( 0, GetDictionaryEntry( 867, pSock.language ));
		}
	}
	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ourObj && ourObj.isChar && pUser )
	{
		if( !ourObj.InRange( pUser, 3 ))
		{
			pSock.SysMessage( GetDictionaryEntry( 393, pSock.language )); // That is too far away.
			return;
		}

		if( !ourObj.npc || ourObj.skillToTame > 1500 )
		{
			pSock.SysMessage( GetDictionaryEntry( 2134, pSock.language )); // That's not an animal!
			return;
		}

		if( ourObj.dead )
		{
			pSock.SysMessage( GetDictionaryEntry( 2205, pSock.language )); // The spirits of the dead are not the province of animal lore.
			return;
		}

		// Apply AoS rules, if applicable
		if( aosEnabled || coreShardEra >= EraStringToNum( "aos" ))
		{
			if( pUser.skills.animallore < 1000 && !ourObj.tamed )
			{
				// At your skill level, you can only lore tamed creatures.
				pSock.SysMessage( GetDictionaryEntry( 2132, pSock.language ));
				return;
			}
			else if( pUser.skills.animallore < 1100 && !ourObj.skillToTame )
			{
				// At your skill level, you can only lore tamed or tameable creatures.
				pSock.SysMessage( GetDictionaryEntry( 2133, pSock.language ));
				return;
			}
		}

		// All good, let's perform the skill check
		if( pUser.CheckSkill( 2, 0, 1000 ))
		{
			position = 20;
			if( osiGumpEnabled )
			{
				var position = 210;

				var animalLoreGump = new Gump;
				animalLoreGump.AddPage( 0 );
				animalLoreGump.AddBackground( 0, 24, 310, 325, 9380 );
				animalLoreGump.AddHTMLGump( 47, 32, 210, 18, false, false, "<basefont color=#000080>" + ourObj.name.toString() + "</basefont>" );
				animalLoreGump.AddButton( 140, 0, 0x82D, 0x82D, 0, 0, 0 );
				animalLoreGump.AddGump( 40, 62, 0x82B );
				animalLoreGump.AddGump( 40, 258, 0x82B );

				animalLoreGump.AddPage( 1 );
				animalLoreGump.AddGump( 28, 76, 0x826 );
				animalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2135, pSock.language ) + "</basefont>" );			// Attributes
				animalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2136, pSock.language ) + "</basefont>" );			// Hits
				animalLoreGump.AddHTMLGump( position, 92, 75, 18, false, false, ourObj.health.toString() + "/" + ourObj.maxhp.toString() );
				animalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2137, pSock.language ) + "</basefont>" );			// Stamina
				animalLoreGump.AddHTMLGump( position, 110, 75, 18, false, false, ourObj.stamina.toString(  ) + "/" + ourObj.maxstamina.toString() );
				animalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2138, pSock.language ) + "</basefont>" );			// Mana
				animalLoreGump.AddHTMLGump( position, 128, 75, 18, false, false, ourObj.mana.toString() + "/" + ourObj.maxmana.toString() );
				animalLoreGump.AddHTMLGump( 53, 146, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2139, pSock.language ) + "</basefont>" );			// Strength
				animalLoreGump.AddHTMLGump( position, 146, 75, 18, false, false, ourObj.strength.toString() );
				animalLoreGump.AddHTMLGump( 53, 164, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2140, pSock.language ) + "</basefont>" );			// Dexterity
				animalLoreGump.AddHTMLGump( position, 164, 75, 18, false, false, ourObj.dexterity.toString() );
				animalLoreGump.AddHTMLGump( 53, 182, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2141, pSock.language ) + "</basefont>" );			// Intelligence
				animalLoreGump.AddHTMLGump( position, 182, 75, 18, false, false, ourObj.intelligence.toString() );
				animalLoreGump.AddHTMLGump( 53, 200, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2142, pSock.language ) + "</basefont>" );			// Barding Difficulty
				animalLoreGump.AddHTMLGump( position, 200, 75, 18, false, false, ourObj.skillToPeace );
				animalLoreGump.AddGump( 28, 220, 0x826 );

				var aloyalty = GetDictionaryEntry( 2188, pSock.language ); // Wild;
				var petLoyalty = ourObj.loyalty;

				/* Pet Loyalty Levels
				2250=Confused 				1-9
				2251=Extremely Unhappy		10-19
				2252=Rather Unhappy			20-29
				2253=Unhappy				30-39
				2254=Somewhat Content		40-49
				2255=Content				50-59
				2256=Happy 					60-69
				2257=Rather Happy 			70-79
				2258=Very Happy 			80-89
				2259=Extremely Happy 		90-95
				2260=Wonderfully Happy 		96-99
				2261=Euphoric 				100*/

				if(( petLoyalty > 0 ))
				{
					if( petLoyalty < 90 )
					{
						aloyalty = GetDictionaryEntry( 2250 + Math.floor( petLoyalty / 10 ), pSock.language ); // Confused > Very Happy
					}
					else if( petLoyalty >= 90 && petLoyalty <= 95 )
					{
						aloyalty = GetDictionaryEntry( 2259, pSock.language );  // Extremely Happy
					}
					else if( petLoyalty >= 96 && petLoyalty <= 99 )
					{
						aloyalty = GetDictionaryEntry( 2260, pSock.language );  // Wonderfully happy
					}
					else // Euphoric
					{
						aloyalty = GetDictionaryEntry( 2261, pSock.language );  // Euphoric
					}
				}

				animalLoreGump.AddHTMLGump( 47, 220, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry(2143, pSock.language ) + "</basefont>" );			// Loyalty Rating
				animalLoreGump.AddHTMLGump( 53, 236, 160, 18, false, false, aloyalty );

				if( aosEnabled || coreShardEra >= EraStringToNum( "aos" ))
				{
					animalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 2, 0 );
					animalLoreGump.AddPage( 2 );
					animalLoreGump.AddGump( 28, 76, 0x826 );
					animalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2135, pSock.language ) + "</basefont>" );			 // Attributes
					animalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2144, pSock.language ) + "</basefont>" );		     // Hit Point Regeneration
					animalLoreGump.AddHTMLGump( position, 92, 75, 18, false, false, ourObj.healthRegen.toString() );
					animalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2145, pSock.language ) + "</basefont>" );			 // Stamina Regeneration
					animalLoreGump.AddHTMLGump( position, 110, 75, 18, false, false, ourObj.staminaRegen.toString() );
					animalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2146, pSock.language ) + "</basefont>" );			 // Mana Regeneration
					animalLoreGump.AddHTMLGump( position, 128, 75, 18, false, false, ourObj.manaRegen.toString() );
					animalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 3, 0 );
					animalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 1, 0 );
				}
				else
				{
					animalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 3, 0 );
				}

				animalLoreGump.AddPage( 3 );
				animalLoreGump.AddGump( 28, 76, 0x826 );
				animalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2147, pSock.language ) + "</basefont>" );		    // Resistances
				animalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2148, pSock.language ) + "</basefont>" );			// Physical
				animalLoreGump.AddHTMLGump( position, 92, 75, 18, false, false, ourObj.Resist( 1 ).toString() );
				animalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2149, pSock.language ) + "</basefont>" );			 // Fire
				animalLoreGump.AddHTMLGump( 210, 110, 75, 18, false, false, "<basefont color=#FF0000>" + ourObj.Resist( 5 ).toString() + "</BASEFONT>" );
				animalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2150, pSock.language ) + "</basefont>" );			 // Cold
				animalLoreGump.AddHTMLGump( position, 128, 75, 18, false, false, "<basefont color=#000080>" + ourObj.Resist( 4 ).toString() + "</BASEFONT>" );
				animalLoreGump.AddHTMLGump( 53, 146, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2151, pSock.language ) + "</basefont>" );			 // Poison
				animalLoreGump.AddHTMLGump( position, 146, 75, 18, false, false, "<basefont color=#008000>" + ourObj.Resist( 7 ).toString() + "</BASEFONT>" );
				animalLoreGump.AddHTMLGump( 53, 164, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2152, pSock.language ) + "</basefont>" );			 // Energy
				animalLoreGump.AddHTMLGump( position, 164, 75, 18, false, false, "<basefont color=#BF80FF>" + ourObj.Resist( 6 ).toString() + "</BASEFONT>" );
				animalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 4, 0 );
				if( aosEnabled || coreShardEra >= EraStringToNum( "aos" ))
					animalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 2, 0 );
				else
					animalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 1, 0 );

				animalLoreGump.AddPage( 4 );
				animalLoreGump.AddGump( 28, 76, 0x826 );
				animalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2153, pSock.language ) + "</basefont>" );				 // Damage
				animalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2154, pSock.language ) + "</basefont>" );			     // Base Damage
				var damageavg = ( ourObj.lodamage + ourObj.hidamage ) / 2;
				animalLoreGump.AddHTMLGump( position, 92, 75, 18, false, false, damageavg );
				if( aosEnabled || coreShardEra >= EraStringToNum( "aos" ))
			    {
					animalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2148, pSock.language ) + "</basefont>" );				 // Physical
					animalLoreGump.AddHTMLGump( position, 110, 75, 18, false, false, "N/A" ); // Not implemented yet
					animalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2149, pSock.language ) + "</basefont>" );			     // Fire
					animalLoreGump.AddHTMLGump( position, 128, 75, 18, false, false, "N/A" ); // Not implemented yet
					animalLoreGump.AddHTMLGump( 53, 146, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2150, pSock.language ) + "</basefont>" );				 // Cold
					animalLoreGump.AddHTMLGump( position, 146, 75, 18, false, false, "N/A" ); // Not implemented yet
					animalLoreGump.AddHTMLGump( 53, 164, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2151, pSock.language ) + "</basefont>" );				 // Poison
					animalLoreGump.AddHTMLGump( position, 164, 75, 18, false, false, "N/A" ); // Not implemented yet
					animalLoreGump.AddHTMLGump( 53, 182, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2152, pSock.language ) + "</basefont>" );				 // Energy
					animalLoreGump.AddHTMLGump( position, 182, 75, 18, false, false, "N/A" ); // Not implemented yet
				}
				animalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 5, 0 );
				animalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 3, 0 );

				animalLoreGump.AddPage( 5 );
				animalLoreGump.AddGump( 28, 76, 0x826 );
				var location = 92;
				animalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2155, pSock.language ) + "</basefont>" );				 // Combat Ratings
				animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2156, pSock.language ) + "</basefont>" );				 // Wrestling
				animalLoreGump.AddHTMLGump( position, 92, 100, 18, false, false, parseInt( ourObj.skills.wrestling / 10 ));
				location += 18;
				animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2157, pSock.language ) + "</basefont>" );				 // Tactics
				animalLoreGump.AddHTMLGump( position, 110, 100, 18, false, false, parseInt( ourObj.skills.tactics / 10 ));
				location += 18;
				animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2158, pSock.language ) + "</basefont>" );				 // Magic Resistance
				animalLoreGump.AddHTMLGump( position, 128, 100, 18, false, false, parseInt( ourObj.skills.magicresistance / 10 ));
				location += 18;
				animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2159, pSock.language ) + "</basefont>" );				 // Anatomy
				animalLoreGump.AddHTMLGump( position, 146, 100, 18, false, false, parseInt( ourObj.skills.anatomy / 10 ));
				location += 18;
				animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2160, pSock.language ) + "</basefont>" );				 // Healing
				animalLoreGump.AddHTMLGump( position, 164, 100, 18, false, false, parseInt( ourObj.skills.healing / 10 ));
				location += 18;
				animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2161, pSock.language ) + "</basefont>" );				 // Poisoning
				animalLoreGump.AddHTMLGump( position, 182, 100, 18, false, false, parseInt( ourObj.skills.poisoning / 10 ));
				location += 18;
				animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2162, pSock.language ) + "</basefont>" );				 // Detect Hidden
				animalLoreGump.AddHTMLGump( position, 200, 100, 18, false, false, parseInt( ourObj.skills.detecthidden / 10 ));
				location += 18;
				animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2163, pSock.language ) + "</basefont>" );				 // Hiding
				animalLoreGump.AddHTMLGump( position, 218, 100, 18, false, false, parseInt( ourObj.skills.hiding / 10 ));
				location += 18;
				animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2164, pSock.language ) + "</basefont>" );				 // Parrying
				animalLoreGump.AddHTMLGump( position, 236, 100, 18, false, false, parseInt( ourObj.skills.parrying / 10 ));
				animalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 6, 0 );
				animalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 4, 0 );

				animalLoreGump.AddPage( 6 );
				animalLoreGump.AddGump( 28, 76, 0x826 );
				var location = 92;
				animalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2165, pSock.language ) + "</basefont>" );				 // Lore & Knowledge
				animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2166, pSock.language ) + "</basefont>" );				 // Magery
				animalLoreGump.AddHTMLGump( position, location, 100, 18, false, false, parseInt( ourObj.skills.magery / 10 ));
				location += 18;
				animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2167, pSock.language ) + "</basefont>" );				 // Eval Int
				animalLoreGump.AddHTMLGump( position, location, 100, 18, false, false, parseInt( ourObj.skills.evaluatingintel / 10 ));
				location += 18;
				animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2168, pSock.language ) + "</basefont>" );				 // Meditation
				animalLoreGump.AddHTMLGump( position, location, 100, 18, false, false, parseInt( ourObj.skills.Meditation / 10 ));
				location += 18;
				if( aosEnabled || coreShardEra >= EraStringToNum( "aos" ))
				{
					animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2169, pSock.language ) + "</basefont>" );			 // Necromancy
					animalLoreGump.AddHTMLGump( position, location, 100, 18, false, false, parseInt( ourObj.skills.necromancy / 10 ));
					location += 18;
				}

				animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2170, pSock.language ) + "</basefont>" );				 // Spirit Speak
				animalLoreGump.AddHTMLGump( position, location, 100, 18, false, false, parseInt( ourObj.skills.spiritSpeak / 10 ));
				location += 18;
				if( saEnabled || coreShardEra >= EraStringToNum( "sa" ))
				{
					animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2171, pSock.language ) + "</basefont>" );			 // Mysticism
					animalLoreGump.AddHTMLGump( position, location, 100, 18, false, false, parseInt( ourObj.skills.mysticism / 10 ));
					location += 18;
					animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2172, pSock.language ) + "</basefont>" );		     // Focus
					animalLoreGump.AddHTMLGump( position, location, 100, 18, false, false, parseInt( ourObj.skills.focus / 10 ));
					location += 18;
					animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2173, pSock.language ) + "</basefont>" );			 // Spellweaving
					animalLoreGump.AddHTMLGump( position, location, 100, 18, false, false, parseInt( ourObj.skills.spellweaving / 10 ));
					location += 18;
				}
				animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2174, pSock.language ) + "</basefont>" );				 // Discordance
				animalLoreGump.AddHTMLGump( position, location, 100, 18, false, false, parseInt( ourObj.skills.discordance / 10 ));
				animalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 7, 0 );
				animalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 5, 0 );

				animalLoreGump.AddPage( 7 );
				var glocation = 76;
				var wlocation = 74;
				var flocation = 94;//164
				if( (seEnabled || coreShardEra >= EraStringToNum( "se" )) || ( aosEnabled || coreShardEra >= EraStringToNum( "aos" )))
				{
					animalLoreGump.AddGump(28, glocation, 0x826);
					animalLoreGump.AddHTMLGump(47, wlocation, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry(2165, pSock.language) + "</basefont>");					 // Lore & Knowledge
					glocation += 70;
					wlocation += 70;
					flocation += 70;
				}

				var location = 92;
				if( seEnabled || coreShardEra >= EraStringToNum( "se" ))
			    {
					animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2175, pSock.language ) + "</basefont>" );			 // Bushido
					animalLoreGump.AddHTMLGump( position, location, 100, 18, false, false, parseInt( ourObj.skills.bushido / 10 ));
					location += 18;
					animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2176, pSock.language ) + "</basefont>" );			 // Ninjitsu
					animalLoreGump.AddHTMLGump( position, location, 100, 18, false, false, parseInt( ourObj.skills.ninjitsu / 10 ));
					location += 18;
				}

				if( aosEnabled || coreShardEra >= EraStringToNum( "aos" ))
				{
					animalLoreGump.AddHTMLGump( 53, location, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2177, pSock.language ) + "</basefont>" );			 // Chivalry
					animalLoreGump.AddHTMLGump( position, location, 100, 18, false, false, parseInt( ourObj.skills.chivalry / 10 ));
				}

				animalLoreGump.AddGump( 28, glocation, 0x826 );
				animalLoreGump.AddHTMLGump( 47, wlocation, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2178, pSock.language ) + "</basefont>" );				 // Preferred Foods

				var validFruitsAndVegetables = [ "fruit", "vegetables", "horse", "boar", "cow", "goat", "gorilla", "deer", "bird", "rabbit", "sheep", "ostard", "parrot", "bear", "rat" ];
				var validGrainsandHay = [ "crops", "hay", "horse", "ostard", "sheep", "rat", "bird", "deer", "gorilla", "cow", "goat", "boar" ];
				var validFish = [ "fish", "bear", "rat", "toad" ];
				var validMeat = [ "meat", "toad", "ostard", "rat" ];
				var validEggs = [ "eggs"];

				var ourFoodList = ourObj.foodList.toLowerCase();
				var afoodlist = "";

				if( validFruitsAndVegetables.indexOf( ourFoodList ) !== -1 )
					afoodlist += "<basefont color=#279053>" + GetDictionaryEntry( 2179, pSock.language ) + "</basefont>"; // Fruits and Vegetables

				if( validGrainsandHay.indexOf( ourFoodList ) !== -1 )
					afoodlist += ( afoodlist ? ", " : "") + "<basefont color=#CFF507>" + GetDictionaryEntry( 2180, pSock.language ) + "</basefont>"; // Grains and Hay

				if( validFish.indexOf(ourFoodList) !== -1 )
					afoodlist += ( afoodlist ? ", " : "") + "<basefont color=#F57607>" + GetDictionaryEntry( 2181, pSock.language ) + "</basefont>"; // Fish

				if( validMeat.indexOf(ourFoodList) !== -1 )
					afoodlist += ( afoodlist ? ", " : "") + "<basefont color=#F50707>" + GetDictionaryEntry( 2182, pSock.language ) + "</basefont>"; // Meat

				if( validEggs.indexOf(ourFoodList) !== -1 )
					afoodlist += ( afoodlist ? ", " : "" ) + "<basefont color=#507070>" + GetDictionaryEntry( 2183, pSock.language ) + "</basefont>"; // Eggs

				if( !ourFoodList )
					afoodlist = "<basefont color=#0000B8>" + GetDictionaryEntry( 2200, pSock.language ) + "</basefont>"; // None

				animalLoreGump.AddHTMLGump( 53, flocation, 160, 80, false, false, afoodlist );

				animalLoreGump.AddGump( 28, 182, 0x826 );
				animalLoreGump.AddHTMLGump( 53, 182, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry(2165, pSock.language) + "</basefont>" );				 // Lore & Knowledge

				animalLoreGump.AddHTMLGump( 53, 200, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2202, pSock.language ) + "</basefont>" );				 // Pet Slots
				animalLoreGump.AddHTMLGump( position, 200, 100, 18, false, false, ourObj.controlSlots.toString() );

			    animalLoreGump.AddHTMLGump( 53, 218, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2203, pSock.language ) + "</basefont>" );				 // Control Chance
				animalLoreGump.AddHTMLGump( position, 218, 100, 18, false, false, (ourObj.CalculateControlChance( pUser ) / 10 ).toString() + "%" );

				animalLoreGump.AddHTMLGump( 53, 236, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2201, pSock.language ) + "</basefont>" );				 // Owner Count
				animalLoreGump.AddHTMLGump( position, 236, 100, 18, false, false, ourObj.ownerCount.toString() );

				animalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 8, 0 );
				animalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 6, 0 );

				animalLoreGump.AddPage( 8 );
				animalLoreGump.AddGump( 28, 76, 0x826 );
				animalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry(2204, pSock.language) + "</basefont>" );					// Friend List

				var yHeight = 92
				var friendList = ourObj.GetFriendList();
				for( var i = 0; i < friendList.length; i++ )
				{
					var myFriend = friendList[i];
					if( ValidateObject( myFriend ))
					{
						animalLoreGump.AddHTMLGump( 53, yHeight, 160, 18, false, false, myFriend.name );
						yHeight += 15;
					}
				}
				animalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 7, 0 );
				animalLoreGump.Send( pSock );
				animalLoreGump.Free();
			}
			else
			{
				var uoxAnimalLoreGump = new Gump;
				var uoxPosition = 20;
				uoxAnimalLoreGump.AddBackground( 0, 0, 300, 300, 2600 );
				uoxAnimalLoreGump.AddButton( 260, 15, 4017, 1, 0, 1 );
				uoxAnimalLoreGump.AddText( 120, 15, 0, ourObj.name );
				uoxAnimalLoreGump.AddPage( 1 );
				uoxPosition = addStringEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2135, pSock.language ), uoxPosition );
				uoxPosition = addDualEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2136, pSock.language ), ourObj.health, ourObj.maxhp, uoxPosition );//Hits
				uoxPosition = addDualEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2137, pSock.language ), ourObj.stamina, ourObj.maxstamina, uoxPosition );//Stamina
				uoxPosition = addDualEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2138, pSock.language ), ourObj.mana, ourObj.maxmana, uoxPosition );//Mana
				uoxPosition = addEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2139, pSock.language ), ourObj.strength, uoxPosition );//Strength
				uoxPosition = addEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2140, pSock.language ), ourObj.dexterity, uoxPosition );//Dexterity
				uoxPosition = addEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2141, pSock.language ), ourObj.intelligence, uoxPosition );//Intelligence
				uoxPosition = addStringEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2184, pSock.language ), uoxPosition ); // Miscellaneous
				uoxPosition = addEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2185, pSock.language ), ourObj.Resist( 1 ), uoxPosition );//Armor Rating:
				uoxAnimalLoreGump.AddPageButton( 260, 260, 4005, 2 );
				uoxAnimalLoreGump.AddPage( 2 );
				uoxPosition = 20;
				uoxPosition = addStringEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2155, pSock.language ), uoxPosition );//Combat Ratings
				uoxPosition = addEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2156, pSock.language ), parseInt( ourObj.skills.wrestling / 10 ), uoxPosition );//Wrestling
				uoxPosition = addEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2157, pSock.language ), parseInt( ourObj.skills.tactics / 10 ), uoxPosition );//tactics
				uoxPosition = addEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2158, pSock.language ), parseInt( ourObj.skills.magicresistance / 10 ), uoxPosition );//magicresistance
				uoxPosition = addEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2159, pSock.language ), parseInt( ourObj.skills.anatomy / 10 ), uoxPosition );//anatomy
				uoxPosition = addEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2161, pSock.language ), parseInt( ourObj.skills.poisoning / 10 ), uoxPosition );//poisoning
				uoxPosition = addStringEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2165, pSock.language ), uoxPosition );//Lore & Knowledge
				uoxPosition = addEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2166, pSock.language ), parseInt( ourObj.skills.magery / 10 ), uoxPosition );//magery
				uoxPosition = addEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2167, pSock.language ), parseInt( ourObj.skills.evaluatingintel / 10 ), uoxPosition );//evaluatingintel
				uoxPosition = addEntry( uoxAnimalLoreGump, GetDictionaryEntry( 2168, pSock.language ), parseInt( ourObj.skills.meditation / 10 ), UOXposition );//meditation
				uoxAnimalLoreGump.AddPageButton( 10, 260, 4014, 1 );
				uoxAnimalLoreGump.Send( pSock );
				uoxAnimalLoreGump.Free();
			}
		}
		else
		{
			pSock.SysMessage( GetDictionaryEntry( 2206, pSock.language )); // You can't think of anything you know offhand.
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 2187, pSock.language )); // That doesn't look much like an animal.
	}
}

function onGumpPress( socket, button, uoxAnimalLoreGump )
{
	switch( button )
	{
		case 1:			break;
		default:		break;
	}
}

function addEntry( uoxAnimalLoreGump, stringToAdd, dataToAdd, uoxPosition )
{
	uoxAnimalLoreGump.AddText( 50, uoxPosition, 0, stringToAdd );
	uoxAnimalLoreGump.AddText( 225, uoxPosition, 0, dataToAdd.toString() );
	uoxPosition += 20;
	return UOXposition;
}

function addDualEntry( uoxAnimalLoreGump, stringToAdd, dataToAdd, moreDataToAdd, uoxPosition )
{
	uoxAnimalLoreGump.AddText( 50, uoxPosition, 0, stringToAdd );
	uoxAnimalLoreGump.AddText( 225, uoxPosition, 0, dataToAdd.toString() + "/" + moreDataToAdd.toString() );
	uoxPosition += 20;
	return uoxPosition;
}

function addStringEntry( uoxAnimalLoreGump, stringToAdd, uoxPosition )
{
	uoxPosition += 20;
	uoxAnimalLoreGump.AddText( 40, uoxPosition, 0, stringToAdd );
	uoxPosition += 20;
	return uoxPosition;
}