function SkillRegistration()
{
	RegisterSkill( 2, true );	// Animal Lore
}

// If disabled, a custom UOX3-specific, old school "stone"-themed gump shows instead of the scroll-based style for Animal Lore results
const OSIGumpEnabled = 1;

// If enabled, a creature's HP/Mana/Stamina regen rate will show, along with Necromancy skill, and damage types (Fire vs Cold, etc)
const AOSenabled = 0;

// If enabled, a creature's skill in Bushido and Ninjitsu will show
const SEenabled = 0;

// If enabled, a creature's skill in Mysticism, Focus and Spellweaving will show
const SAenabled = 0;

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		if( AOSenabled )
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

		if( !ourObj.npc || !ourObj.isAnimal )
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
		if( AOSenabled )
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
		if(( AOSenabled && pUser.CheckSkill( 2, 0, 1100 )) || ( !AOSenabled && pUser.CheckSkill( 2, 0, 1000 )))
		{
			position = 20;
			if( OSIGumpEnabled )
			{
				var position = 210;

				var AnimalLoreGump = new Gump;
				AnimalLoreGump.AddPage( 0 );
				AnimalLoreGump.AddBackground( 0, 24, 310, 325, 9380 );
				AnimalLoreGump.AddHTMLGump( 47, 32, 210, 18, false, false, "<basefont color=#000080>" + ourObj.name.toString() + "</basefont>" );
				AnimalLoreGump.AddButton( 140, 0, 0x82D, 0x82D, 0, 0, 0 );
				AnimalLoreGump.AddGump( 40, 62, 0x82B );
				AnimalLoreGump.AddGump( 40, 258, 0x82B );

				AnimalLoreGump.AddPage( 1 );
				AnimalLoreGump.AddGump( 28, 76, 0x826 );
				AnimalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2135, pSock.language ) + "</basefont>" );			// Attributes
				AnimalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2136, pSock.language ) + "</basefont>" );			// Hits
				AnimalLoreGump.AddHTMLGump( position, 92, 75, 18, false, false, ourObj.health.toString() + "/" + ourObj.maxhp.toString() );
				AnimalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2137, pSock.language ) + "</basefont>" );			// Stamina
				AnimalLoreGump.AddHTMLGump( position, 110, 75, 18, false, false, ourObj.stamina.toString(  ) + "/" + ourObj.maxstamina.toString() );
				AnimalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2138, pSock.language ) + "</basefont>" );			// Mana
				AnimalLoreGump.AddHTMLGump( position, 128, 75, 18, false, false, ourObj.mana.toString() + "/" + ourObj.maxmana.toString() );
				AnimalLoreGump.AddHTMLGump( 53, 146, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2139, pSock.language ) + "</basefont>" );			// Strength
				AnimalLoreGump.AddHTMLGump( position, 146, 75, 18, false, false, ourObj.strength.toString() );
				AnimalLoreGump.AddHTMLGump( 53, 164, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2140, pSock.language ) + "</basefont>" );			// Dexterity
				AnimalLoreGump.AddHTMLGump( position, 164, 75, 18, false, false, ourObj.dexterity.toString() );
				AnimalLoreGump.AddHTMLGump( 53, 182, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2141, pSock.language ) + "</basefont>" );			// Intelligence
				AnimalLoreGump.AddHTMLGump( position, 182, 75, 18, false, false, ourObj.intelligence.toString() );
				AnimalLoreGump.AddHTMLGump( 53, 200, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2142, pSock.language ) + "</basefont>" );			// Barding Difficulty
				AnimalLoreGump.AddHTMLGump( position, 200, 75, 18, false, false, ourObj.skillToPeace );
				AnimalLoreGump.AddGump( 28, 220, 0x826 );

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
						aloyalty = GetDictionaryEntry( 2250 + Math.floor( petLoyalty / 10), pSock.language ); // Confused > Very Happy
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

				AnimalLoreGump.AddHTMLGump( 47, 220, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry(2143, pSock.language ) + "</basefont>" );			// Loyalty Rating
				AnimalLoreGump.AddHTMLGump( 53, 236, 160, 18, false, false, aloyalty );

				if( AOSenabled )
				{
					AnimalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 2, 0 );
					AnimalLoreGump.AddPage( 2 );
					AnimalLoreGump.AddGump( 28, 76, 0x826 );
					AnimalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2135, pSock.language ) + "</basefont>" );			 // Attributes
					AnimalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2144, pSock.language ) + "</basefont>" );		     // Hit Point Regeneration
					AnimalLoreGump.AddHTMLGump( position, 92, 75, 18, false, false, "N/A" ); // Not implemented yet
					AnimalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2145, pSock.language ) + "</basefont>" );			 // Stamina Regeneration
					AnimalLoreGump.AddHTMLGump( position, 110, 75, 18, false, false, "N/A" ); // Not implemented yet
					AnimalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2146, pSock.language ) + "</basefont>" );			 // Mana Regeneration
					AnimalLoreGump.AddHTMLGump( position, 128, 75, 18, false, false, "N/A" ); // Not implemented yet
					AnimalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 3, 0 );
					AnimalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 1, 0 );
				}
				else
				{
					AnimalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 3, 0 );
				}

				AnimalLoreGump.AddPage( 3 );
				AnimalLoreGump.AddGump( 28, 76, 0x826 );
				AnimalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2147, pSock.language ) + "</basefont>" );		    // Resistances
				AnimalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2148, pSock.language ) + "</basefont>" );			// Physical
				AnimalLoreGump.AddHTMLGump( position, 92, 75, 18, false, false, ourObj.Resist( 1 ).toString() );
				AnimalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2149, pSock.language ) + "</basefont>" );			 // Fire
				AnimalLoreGump.AddHTMLGump( 210, 110, 75, 18, false, false, "<basefont color=#FF0000>" + ourObj.Resist( 5 ).toString() + "</BASEFONT>" );
				AnimalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2150, pSock.language ) + "</basefont>" );			 // Cold
				AnimalLoreGump.AddHTMLGump( position, 128, 75, 18, false, false, "<basefont color=#000080>" + ourObj.Resist( 4 ).toString() + "</BASEFONT>" );
				AnimalLoreGump.AddHTMLGump( 53, 146, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2151, pSock.language ) + "</basefont>" );			 // Poison
				AnimalLoreGump.AddHTMLGump( position, 146, 75, 18, false, false, "<basefont color=#008000>" + ourObj.Resist( 7 ).toString() + "</BASEFONT>" );
				AnimalLoreGump.AddHTMLGump( 53, 164, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2152, pSock.language ) + "</basefont>" );			 // Energy
				AnimalLoreGump.AddHTMLGump( position, 164, 75, 18, false, false, "<basefont color=#BF80FF>" + ourObj.Resist( 6 ).toString() + "</BASEFONT>" );
				AnimalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 4, 0 );
				if( AOSenabled )
					AnimalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 2, 0 );
				else
					AnimalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 1, 0 );

				AnimalLoreGump.AddPage( 4 );
				AnimalLoreGump.AddGump( 28, 76, 0x826 );
				AnimalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2153, pSock.language ) + "</basefont>" );				 // Damage
				AnimalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2154, pSock.language ) + "</basefont>" );			     // Base Damage
				var damageavg = (ourObj.lodamage + ourObj.hidamage) / 2;
				AnimalLoreGump.AddHTMLGump( position, 92, 75, 18, false, false, damageavg );
				if( AOSenabled )
			    {
					AnimalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2148, pSock.language ) + "</basefont>" );				 // Physical
					AnimalLoreGump.AddHTMLGump( position, 110, 75, 18, false, false, "N/A" ); // Not implemented yet
					AnimalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2149, pSock.language ) + "</basefont>" );			     // Fire
					AnimalLoreGump.AddHTMLGump( position, 128, 75, 18, false, false, "N/A" ); // Not implemented yet
					AnimalLoreGump.AddHTMLGump( 53, 146, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2150, pSock.language ) + "</basefont>" );				 // Cold
					AnimalLoreGump.AddHTMLGump( position, 146, 75, 18, false, false, "N/A" ); // Not implemented yet
					AnimalLoreGump.AddHTMLGump( 53, 164, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2151, pSock.language ) + "</basefont>" );				 // Poison
					AnimalLoreGump.AddHTMLGump( position, 164, 75, 18, false, false, "N/A" ); // Not implemented yet
					AnimalLoreGump.AddHTMLGump( 53, 182, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2152, pSock.language ) + "</basefont>" );				 // Energy
					AnimalLoreGump.AddHTMLGump( position, 182, 75, 18, false, false, "N/A" ); // Not implemented yet
				}
				AnimalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 5, 0 );
				AnimalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 3, 0 );

				AnimalLoreGump.AddPage( 5 );
				AnimalLoreGump.AddGump( 28, 76, 0x826 );
				AnimalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2155, pSock.language ) + "</basefont>" );				 // Combat Ratings
				AnimalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2156, pSock.language ) + "</basefont>" );				 // Wrestling
				AnimalLoreGump.AddHTMLGump( position, 92, 100, 18, false, false, parseInt( ourObj.skills.wrestling / 10 ));
				AnimalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2157, pSock.language ) + "</basefont>" );				 // Tactics
				AnimalLoreGump.AddHTMLGump( position, 110, 100, 18, false, false, parseInt( ourObj.skills.Tactics / 10 ));
				AnimalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2158, pSock.language ) + "</basefont>" );				 // Magic Resistance
				AnimalLoreGump.AddHTMLGump( position, 128, 100, 18, false, false, parseInt( ourObj.skills.MagicResistance / 10 ));
				AnimalLoreGump.AddHTMLGump( 53, 146, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2159, pSock.language ) + "</basefont>" );				 // Anatomy
				AnimalLoreGump.AddHTMLGump( position, 146, 100, 18, false, false, parseInt( ourObj.skills.Anatomy / 10 ));
				AnimalLoreGump.AddHTMLGump( 53, 164, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2160, pSock.language ) + "</basefont>" );				 // Healing
				AnimalLoreGump.AddHTMLGump( position, 164, 100, 18, false, false, parseInt( ourObj.skills.Healing / 10 ));
				AnimalLoreGump.AddHTMLGump( 53, 182, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2161, pSock.language ) + "</basefont>" );				 // Poisoning
				AnimalLoreGump.AddHTMLGump( position, 182, 100, 18, false, false, parseInt( ourObj.skills.Poisoning / 10 ));
				AnimalLoreGump.AddHTMLGump( 53, 200, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2162, pSock.language ) + "</basefont>" );				 // Detect Hidden
				AnimalLoreGump.AddHTMLGump( position, 200, 100, 18, false, false, parseInt( ourObj.skills.DetectHidden / 10 ));
				AnimalLoreGump.AddHTMLGump( 53, 218, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2163, pSock.language ) + "</basefont>" );				 // Hiding
				AnimalLoreGump.AddHTMLGump( position, 218, 100, 18, false, false, parseInt( ourObj.skills.Hiding / 10 ));
				AnimalLoreGump.AddHTMLGump( 53, 236, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2164, pSock.language ) + "</basefont>" );				 // Parrying
				AnimalLoreGump.AddHTMLGump( position, 236, 100, 18, false, false, parseInt( ourObj.skills.Parrying / 10 ));
				AnimalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 6, 0 );
				AnimalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 4, 0 );

				AnimalLoreGump.AddPage( 6 );
				AnimalLoreGump.AddGump( 28, 76, 0x826 );
				AnimalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2165, pSock.language ) + "</basefont>" );				 // Lore & Knowledge
				AnimalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2166, pSock.language ) + "</basefont>" );				 // Magery
				AnimalLoreGump.AddHTMLGump( position, 92, 100, 18, false, false, parseInt( ourObj.skills.Magery / 10 ));
				AnimalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2167, pSock.language ) + "</basefont>" );				 // Eval Int
				AnimalLoreGump.AddHTMLGump( position, 110, 100, 18, false, false, parseInt( ourObj.skills.evaluatingintel / 10 ));
				AnimalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2168, pSock.language ) + "</basefont>" );				 // Meditation
				AnimalLoreGump.AddHTMLGump( position, 128, 100, 18, false, false, parseInt( ourObj.skills.Meditation / 10 ));
				if( AOSenabled )
				{
					AnimalLoreGump.AddHTMLGump( 53, 146, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2169, pSock.language ) + "</basefont>" );			 // Necromancy
					AnimalLoreGump.AddHTMLGump( position, 146, 100, 18, false, false, parseInt( ourObj.skills.Necromancy / 10 ));
				}
				AnimalLoreGump.AddHTMLGump( 53, 164, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2170, pSock.language ) + "</basefont>" );				 // Spirit Speak
				AnimalLoreGump.AddHTMLGump( position, 164, 100, 18, false, false, parseInt( ourObj.skills.SpiritSpeak / 10 ));
				if( SAenabled )
				{
					AnimalLoreGump.AddHTMLGump( 53, 182, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2171, pSock.language ) + "</basefont>" );			 // Mysticism
					AnimalLoreGump.AddHTMLGump( position, 182, 100, 18, false, false, parseInt( ourObj.skills.Mysticism / 10 ));
					AnimalLoreGump.AddHTMLGump( 53, 200, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2172, pSock.language ) + "</basefont>" );		     // Focus
					AnimalLoreGump.AddHTMLGump( position, 200, 100, 18, false, false, parseInt( ourObj.skills.Focus / 10 ));
					AnimalLoreGump.AddHTMLGump( 53, 218, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2173, pSock.language ) + "</basefont>" );			 // Spellweaving
					AnimalLoreGump.AddHTMLGump( position, 218, 100, 18, false, false, parseInt( ourObj.skills.Spellweaving / 10 ));
				}
				AnimalLoreGump.AddHTMLGump( 53, 236, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2174, pSock.language ) + "</basefont>" );				 // Discordance
				AnimalLoreGump.AddHTMLGump( position, 236, 100, 18, false, false, parseInt( ourObj.skills.Discordance / 10 ));
				AnimalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 7, 0 );
				AnimalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 5, 0 );

				AnimalLoreGump.AddPage( 7 );
				AnimalLoreGump.AddGump( 28, 76, 0x826 );
				AnimalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry(2165, pSock.language) + "</basefont>" );					 // Lore & Knowledge
				if( SEenabled )
			    {
					AnimalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2175, pSock.language ) + "</basefont>" );			 // Bushido
					AnimalLoreGump.AddHTMLGump( position, 92, 100, 18, false, false, parseInt( ourObj.skills.Bushido / 10 ));
					AnimalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2176, pSock.language ) + "</basefont>" );			 // Ninjitsu
					AnimalLoreGump.AddHTMLGump( position, 110, 100, 18, false, false, parseInt( ourObj.skills.Ninjitsu / 10 ));
				}
				if( AOSenabled )
				{
					AnimalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2177, pSock.language ) + "</basefont>" );			 // Chivalry
					AnimalLoreGump.AddHTMLGump( position, 128, 100, 18, false, false, parseInt( ourObj.skills.Chivalry / 10 ));
				}
				AnimalLoreGump.AddGump( 28, 146, 0x826 );
				AnimalLoreGump.AddHTMLGump( 47, 144, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2178, pSock.language ) + "</basefont>" );				 // Preferred Foods

				var afoodlist = "<basefont color=#0000B8>" + GetDictionaryEntry( 2200, pSock.language ) + "</basefont>";//None

				var ourFoodList = ourObj.foodList.toLowerCase();
				if(( ourFoodList == "fruit" ) || ( ourFoodList == "vegetable" ))
					afoodlist = "<basefont color=#279053>" + GetDictionaryEntry( 2179, pSock.language ) + "</basefont>"; // Fruits and Vegetables
				else if (( ourFoodList == "crops" ) || ( ourFoodList == "hay" ))
					afoodlist = "<basefont color=#CFF507>" + GetDictionaryEntry( 2180, pSock.language ) + "</basefont>"; // Grains and Hay
				else if (( ourFoodList == "fish" ))
					afoodlist = "<basefont color=#F57607>" + GetDictionaryEntry( 2181, pSock.language ) + "</basefont>"; // Fish
				else if (( ourFoodList == "meat" ))
					afoodlist = "<basefont color=#F50707>" + GetDictionaryEntry( 2182, pSock.language ) + "</basefont>"; // Meat
				else if (( ourFoodList == "eggs" ))
					afoodlist = "<basefont color=#5070700>" + GetDictionaryEntry( 2183, pSock.language ) + "</basefont>"; // Eggs

				AnimalLoreGump.AddHTMLGump( 53, 164, 160, 18, false, false, afoodlist );

				AnimalLoreGump.AddGump( 28, 182, 0x826 );
				AnimalLoreGump.AddHTMLGump( 53, 182, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry(2165, pSock.language) + "</basefont>" );				 // Lore & Knowledge

				AnimalLoreGump.AddHTMLGump( 53, 200, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2202, pSock.language ) + "</basefont>" );				 // Pet Slots
				AnimalLoreGump.AddHTMLGump( position, 200, 100, 18, false, false, ourObj.controlSlots.toString() );

			    AnimalLoreGump.AddHTMLGump( 53, 218, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2203, pSock.language ) + "</basefont>" );				 // Control Chance
				AnimalLoreGump.AddHTMLGump( position, 218, 100, 18, false, false, (ourObj.CalculateControlChance( pUser ) / 10 ).toString() + "%" );

				AnimalLoreGump.AddHTMLGump( 53, 236, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2201, pSock.language ) + "</basefont>" );				 // Owner Count
				AnimalLoreGump.AddHTMLGump( position, 236, 100, 18, false, false, ourObj.ownerCount.toString() );

				AnimalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 8, 0 );
				AnimalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 6, 0 );

				AnimalLoreGump.AddPage( 8 );
				AnimalLoreGump.AddGump( 28, 76, 0x826 );
				AnimalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry(2204, pSock.language) + "</basefont>" );					// Friend List

				var yHeight = 92
				var friendList = ourObj.GetFriendList();
				for( var i = 0; i < friendList.length; i++ )
				{
					var myFriend = friendList[i];
					if( ValidateObject( myFriend ))
					{
						AnimalLoreGump.AddHTMLGump( 53, yHeight, 160, 18, false, false, myFriend.name );
						yHeight += 15;
					}
				}
				AnimalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 7, 0 );
				AnimalLoreGump.Send( pSock );
				AnimalLoreGump.Free();
			}
			else
			{
				var UOXAnimalLoreGump = new Gump;
				var UOXposition = 20;
				UOXAnimalLoreGump.AddBackground( 0, 0, 300, 300, 2600 );
				UOXAnimalLoreGump.AddButton( 260, 15, 4017, 1, 0, 1 );
				UOXAnimalLoreGump.AddText( 120, 15, 0, ourObj.name );
				UOXAnimalLoreGump.AddPage( 1 );
				UOXposition = addStringEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2135, pSock.language ), UOXposition );
				UOXposition = addDualEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2136, pSock.language ), ourObj.health, ourObj.maxhp, UOXposition );//Hits
				UOXposition = addDualEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2137, pSock.language ), ourObj.stamina, ourObj.maxstamina, UOXposition );//Stamina
				UOXposition = addDualEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2138, pSock.language ), ourObj.mana, ourObj.maxmana, UOXposition );//Mana
				UOXposition = addEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2139, pSock.language ), ourObj.strength, UOXposition );//Strength
				UOXposition = addEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2140, pSock.language ), ourObj.dexterity, UOXposition );//Dexterity
				UOXposition = addEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2141, pSock.language ), ourObj.intelligence, UOXposition );//Intelligence
				UOXposition = addStringEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2184, pSock.language ), UOXposition ); // Miscellaneous
				UOXposition = addEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2185, pSock.language ), ourObj.Resist( 1 ), UOXposition );//Armor Rating:
				UOXAnimalLoreGump.AddPageButton( 260, 260, 4005, 2 );
				UOXAnimalLoreGump.AddPage( 2 );
				UOXposition = 20;
				UOXposition = addStringEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2155, pSock.language ), UOXposition );//Combat Ratings
				UOXposition = addEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2156, pSock.language ), parseInt( ourObj.skills.wrestling / 10 ), UOXposition );//Wrestling
				UOXposition = addEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2157, pSock.language ), parseInt( ourObj.skills.tactics / 10 ), UOXposition );//tactics
				UOXposition = addEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2158, pSock.language ), parseInt( ourObj.skills.magicresistance / 10 ), UOXposition );//magicresistance
				UOXposition = addEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2159, pSock.language ), parseInt( ourObj.skills.anatomy / 10 ), UOXposition );//anatomy
				UOXposition = addEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2161, pSock.language ), parseInt( ourObj.skills.poisoning / 10 ), UOXposition );//poisoning
				UOXposition = addStringEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2165, pSock.language ), UOXposition );//Lore & Knowledge
				UOXposition = addEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2166, pSock.language ), parseInt( ourObj.skills.magery / 10 ), UOXposition );//magery
				UOXposition = addEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2167, pSock.language ), parseInt( ourObj.skills.evaluatingintel / 10 ), UOXposition );//evaluatingintel
				UOXposition = addEntry( UOXAnimalLoreGump, GetDictionaryEntry( 2168, pSock.language ), parseInt( ourObj.skills.meditation / 10 ), UOXposition );//meditation
				UOXAnimalLoreGump.AddPageButton( 10, 260, 4014, 1 );
				UOXAnimalLoreGump.Send( pSock );
				UOXAnimalLoreGump.Free();
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

function onGumpPress( socket, button, UOXAnimalLoreGump )
{
	switch( button )
	{
		case 1:			break;
		default:		break;
	}
}

function addEntry( UOXAnimalLoreGump, stringToAdd, dataToAdd, UOXposition )
{
	UOXAnimalLoreGump.AddText( 50, UOXposition, 0, stringToAdd );
	UOXAnimalLoreGump.AddText( 225, UOXposition, 0, dataToAdd.toString() );
	UOXposition += 20;
	return UOXposition;
}

function addDualEntry( UOXAnimalLoreGump, stringToAdd, dataToAdd, moreDataToAdd, UOXposition )
{
	UOXAnimalLoreGump.AddText( 50, UOXposition, 0, stringToAdd );
	UOXAnimalLoreGump.AddText( 225, UOXposition, 0, dataToAdd.toString() + "/" + moreDataToAdd.toString() );
	UOXposition += 20;
	return UOXposition;
}

function addStringEntry( UOXAnimalLoreGump, stringToAdd, UOXposition )
{
	UOXposition += 20;
	UOXAnimalLoreGump.AddText( 40, UOXposition, 0, stringToAdd );
	UOXposition += 20;
	return UOXposition;
}