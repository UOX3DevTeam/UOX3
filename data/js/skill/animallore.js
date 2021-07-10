function SkillRegistration()
{
	RegisterSkill( 2, true );	// Animal Lore
}
var enabledAOS = 1;

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if ( pSock )
	{
		if ( enabledAOS == 1 )
		{
			//What animal should I look at ?
			pSock.CustomTarget( 0, GetDictionaryEntry( 2130, pSock.language ) );
		}
		else
	    {
		    //What animal do you wish to get information about?
		    pSock.CustomTarget( 0, GetDictionaryEntry( 867, pSock.language ) );
		}
	}
	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if ( ourObj && ourObj.isChar && pUser )
	{
		if ( !ourObj.InRange( pUser, 3 ) )
		{
			// That is too far away.
			pSock.SysMessage( GetDictionaryEntry( 393, pSock.language ) );
		}
		else if ( !ourObj.npc || !ourObj.isAnimal )
		{
			// That's not an animal!
			pSock.SysMessage( GetDictionaryEntry( 2134, pSock.language ) );
		}
		else if ( ourObj.tamed || pUser.skills.animallore >= 700 )
		{
			if ( !pUser.CheckSkill( 2, 0, 1000 ) )
			{
				// At your skill level, you can only lore tamed creatures.
				pSock.SysMessage( GetDictionaryEntry( 2132, pSock.language ) );
			}
			else if ( enabledAOS == 1 && !pUser.CheckSkill( 2, 0, 1100 ) )
			{
				// At your skill level, you can only lore tamed or tameable creatures.
				pSock.SysMessage(GetDictionaryEntry( 2133, pSock.language ) );
			}
			else
			{
				position = 20;
				if ( enabledAOS == 1 )
				{
					var ColorLabel = 0xF424E5;

					var AnimalLoreGump = new Gump;
					AnimalLoreGump.AddPage( 0 );
					AnimalLoreGump.AddBackground( 0, 24, 310, 325, 9380 );
					AnimalLoreGump.AddHTMLGump( 47, 32, 210, 18, false, false, "<basefont color=#000080>" + ourObj.name.toString() + "</basefont>" );
					AnimalLoreGump.AddButton( 140, 0, 0x82D, 0x82D, 0, 0, 0 );
					AnimalLoreGump.AddGump( 40, 62, 0x82B );
					AnimalLoreGump.AddGump( 40, 258, 0x82B );

					AnimalLoreGump.AddPage( 1 );
					AnimalLoreGump.AddGump( 28, 76, 0x826 );
					AnimalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2135, pSock.language ) + "</basefont>" );				// Attributes
					AnimalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2136, pSock.language ) + "</basefont>" );				// Hits
					AnimalLoreGump.AddHTMLGump( 180, 92, 75, 18, false, false, ourObj.health.toString() + "/" + ourObj.maxhp.toString() );
					AnimalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2137, pSock.language ) + "</basefont>" );			// Stamina
					AnimalLoreGump.AddHTMLGump( 180, 110, 75, 18, false, false, ourObj.stamina.toString(  ) + "/" + ourObj.maxstamina.toString() );
					AnimalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2138, pSock.language ) + "</basefont>" );			// Mana
					AnimalLoreGump.AddHTMLGump( 180, 128, 75, 18, false, false, ourObj.mana.toString() + "/" + ourObj.maxmana.toString() );
					AnimalLoreGump.AddHTMLGump( 53, 146, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2139, pSock.language ) + "</basefont>" );			// Strength
					AnimalLoreGump.AddHTMLGump( 180, 146, 75, 18, false, false, ourObj.strength.toString() );
					AnimalLoreGump.AddHTMLGump( 53, 164, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2140, pSock.language ) + "</basefont>" );			// Dexterity
					AnimalLoreGump.AddHTMLGump( 180, 164, 75, 18, false, false, ourObj.dexterity.toString() );
					AnimalLoreGump.AddHTMLGump( 53, 182, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2141, pSock.language ) + "</basefont>" );			// Intelligence
					AnimalLoreGump.AddHTMLGump( 180, 182, 75, 18, false, false, ourObj.intelligence.toString() );
					AnimalLoreGump.AddHTMLGump( 53, 200, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2142, pSock.language ) + "</basefont>" );			// Barding Difficulty
					AnimalLoreGump.AddHTMLGump(180, 200, 75, 18, false, false, ourObj.skillToPeace );
					AnimalLoreGump.AddGump( 28, 220, 0x826 );
					AnimalLoreGump.AddHTMLGump( 47, 220, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2143, pSock.language ) + "</basefont>" );					// Loyalty Rating
					AnimalLoreGump.AddHTMLGump( 53, 236, 160, 18, false, false, "Not Implanted Yet" );
					//AnimalLoreGump.AddXMFHTMLGumpColor( 53, 236, 160, 18, 1061643 : 1049595, false, false, ColorLabel  );
					AnimalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 2, 0 );
					AnimalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 1, 0 );

					AnimalLoreGump.AddPage( 2 );
					AnimalLoreGump.AddGump( 28, 76, 0x826 );
					AnimalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2135, pSock.language ) + "</basefont>" );					 // Attributes
					AnimalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2144, pSock.language ) + "</basefont>" );				 // Hit Point Regeneration
					//AnimalLoreGump.AddHTMLGump( 180, 92, 75, 18, false, false, "0" );
					AnimalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2145, pSock.language ) + "</basefont>" );			 // Stamina Regeneration
					//AnimalLoreGump.AddHTMLGump( 180, 110, 75, 18, false, false, "0" );
					AnimalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2146, pSock.language ) + "</basefont>" );			 // Mana Regeneration
					//AnimalLoreGump.AddHTMLGump( 180, 128, 75, 18, false, false, "0" );
					AnimalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 3, 0 );
					AnimalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 1, 0 );

					AnimalLoreGump.AddPage( 3 );
					AnimalLoreGump.AddGump( 28, 76, 0x826 );
					AnimalLoreGump.AddXMFHTMLGumpColor( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2147, pSock.language ) + "</basefont>" );					 // Resistances
					AnimalLoreGump.AddXMFHTMLGumpColor( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2148, pSock.language ) + "</basefont>" );				 // Physical
					AnimalLoreGump.AddHTMLGump( 180, 92, 75, 18, false, false, ourObj.Resist( 1 ).toString(  ) );
					AnimalLoreGump.AddXMFHTMLGumpColor( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2149, pSock.language ) + "</basefont>" );			 // Fire
					AnimalLoreGump.AddHTMLGump( 180, 110, 75, 18, false, false, "<basefont color=#FF0000>" + ourObj.Resist( 5 ).toString() + "</BASEFONT>" );
					AnimalLoreGump.AddXMFHTMLGumpColor( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2150, pSock.language ) + "</basefont>" );			 // Cold
					AnimalLoreGump.AddHTMLGump( 180, 128, 75, 18, false, false, "<basefont color=#000080>" + ourObj.Resist( 4 ).toString() + "</BASEFONT>" );
					AnimalLoreGump.AddXMFHTMLGumpColor( 53, 146, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2151, pSock.language ) + "</basefont>" );			 // Poison
					AnimalLoreGump.AddHTMLGump( 180, 146, 75, 18, false, false, "<basefont color=#008000>" + ourObj.Resist( 7 ).toString() + "</BASEFONT>" );
					AnimalLoreGump.AddXMFHTMLGumpColor( 53, 164, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2152, pSock.language ) + "</basefont>" );			 // Energy
					AnimalLoreGump.AddHTMLGump( 180, 164, 75, 18, false, false, "<basefont color=#BF80FF>" + ourObj.Resist( 6 ).toString() + "</BASEFONT>" );
					AnimalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 4, 0 );
					AnimalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 2, 0 );

					AnimalLoreGump.AddPage(4);
					AnimalLoreGump.AddGump( 28, 76, 0x826 );
					AnimalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2153, pSock.language ) + "</basefont>" );						 // Damage
					AnimalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2148, pSock.language ) + "</basefont>" );				 // Physical
					//AnimalLoreGump.AddHTMLGump( 180, 92, 75, 18, false, false, ourObj.Physical.toString() );
					AnimalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2149, pSock.language ) + "</basefont>" );			     // Fire
					//AnimalLoreGump.AddHTMLGump( 180, 110, 75, 18, false, false, ourObj.Fire.toString() );
					AnimalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2150, pSock.language ) + "</basefont>" );				 // Cold
					//AnimalLoreGump.AddHTMLGump( 180, 128, 75, 18, false, false, ourObj.Cold.toString() );
					AnimalLoreGump.AddHTMLGump( 53, 146, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2151, pSock.language ) + "</basefont>" );				 // Poison
					//AnimalLoreGump.AddHTMLGump( 180, 146, 75, 18, false, false, ourObj.Poison.toString() );
					AnimalLoreGump.AddHTMLGump( 53, 164, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2152, pSock.language ) + "</basefont>" );				 // Energy
					//AnimalLoreGump.AddHTMLGump( 180, 164, 75, 18, false, false, ourObj.Energy.toString() );
					AnimalLoreGump.AddHTMLGump(53, 182, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2154, pSock.language ) + "</basefont>" );			     // Base Damage
					var damageavg = (ourObj.lodamage + ourObj.hidamage) / 2;
					AnimalLoreGump.AddHTMLGump(180, 182, 75, 18, false, false, damageavg );
					AnimalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 5, 0 );
					AnimalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 3, 0 );

					AnimalLoreGump.AddPage( 5 );
					AnimalLoreGump.AddGump( 28, 76, 0x826 );
					AnimalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2155, pSock.language ) + "</basefont>" );						 // Combat Ratings
					AnimalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2156, pSock.language ) + "</basefont>" );					 // Wrestling
					AnimalLoreGump.AddHTMLGump( 180, 92, 100, 18, false, false, parseInt( ourObj.skills.wrestling / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2157, pSock.language ) + "</basefont>" );				 // Tactics
					AnimalLoreGump.AddHTMLGump( 180, 110, 100, 18, false, false, parseInt( ourObj.skills.Tactics / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2158, pSock.language ) + "</basefont>" );				 // Magic Resistance
					AnimalLoreGump.AddHTMLGump( 180, 128, 100, 18, false, false, parseInt( ourObj.skills.MagicResistance / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 146, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2159, pSock.language ) + "</basefont>" );				 // Anatomy
					AnimalLoreGump.AddHTMLGump( 180, 146, 100, 18, false, false, parseInt( ourObj.skills.Anatomy / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 164, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2160, pSock.language ) + "</basefont>" );				 // Healing
					AnimalLoreGump.AddHTMLGump( 180, 164, 100, 18, false, false, parseInt( ourObj.skills.Healing / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 182, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2161, pSock.language ) + "</basefont>" );				 // Poisoning
					AnimalLoreGump.AddHTMLGump( 180, 182, 100, 18, false, false, parseInt( ourObj.skills.Poisoning / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 200, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2162, pSock.language ) + "</basefont>" );				 // Detect Hidden
					AnimalLoreGump.AddHTMLGump( 180, 200, 100, 18, false, false, parseInt( ourObj.skills.DetectHidden / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 218, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2163, pSock.language ) + "</basefont>" );				 // Hiding
					AnimalLoreGump.AddHTMLGump( 180, 218, 100, 18, false, false, parseInt( ourObj.skills.Hiding / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 236, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2164, pSock.language ) + "</basefont>" );				 // Parrying
					AnimalLoreGump.AddHTMLGump( 180, 236, 100, 18, false, false, parseInt( ourObj.skills.Parrying / 10 ) );
					AnimalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 6, 0 );
					AnimalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 4, 0 );

					AnimalLoreGump.AddPage( 6 );
					AnimalLoreGump.AddGump( 28, 76, 0x826 );
					AnimalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2165, pSock.language ) + "</basefont>" );						 // Lore & Knowledge
					AnimalLoreGump.AddHTMLGump( 53, 92, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2166, pSock.language ) + "</basefont>" );					 // Magery
					AnimalLoreGump.AddHTMLGump( 180, 92, 100, 18, false, false, parseInt( ourObj.skills.Magery / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 110, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2167, pSock.language ) + "</basefont>" );				 // Eval Int
					AnimalLoreGump.AddHTMLGump( 180, 110, 100, 18, false, false, parseInt( ourObj.skills.evaluatingintel / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 128, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2168, pSock.language ) + "</basefont>" );				 // Meditation
					AnimalLoreGump.AddHTMLGump( 180, 128, 100, 18, false, false, parseInt( ourObj.skills.Meditation / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 146, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2169, pSock.language ) + "</basefont>" );				 // Necromancy
					AnimalLoreGump.AddHTMLGump( 180, 146, 100, 18, false, false, parseInt( ourObj.skills.Necromancy / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 164, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2170, pSock.language ) + "</basefont>" );				 // Spirit Speak
					AnimalLoreGump.AddHTMLGump( 180, 164, 100, 18, false, false, parseInt( ourObj.skills.SpiritSpeak / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 182, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2171, pSock.language ) + "</basefont>" );				 // Mysticism
					AnimalLoreGump.AddHTMLGump( 180, 182, 100, 18, false, false, parseInt( ourObj.skills.Mysticism / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 200, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2172, pSock.language ) + "</basefont>" );				 // Focus
					AnimalLoreGump.AddHTMLGump( 180, 200, 100, 18, false, false, parseInt( ourObj.skills.Focus / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 218, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2173, pSock.language ) + "</basefont>" );				 // Spellweaving
					AnimalLoreGump.AddHTMLGump( 180, 218, 100, 18, false, false, parseInt( ourObj.skills.Spellweaving / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 236, 160, 18, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2174, pSock.language ) + "</basefont>" );				 // Discordance
					AnimalLoreGump.AddHTMLGump( 180, 236, 100, 18, false, false, parseInt( ourObj.skills.Discordance / 10 ) );
					AnimalLoreGump.AddButton( 240, 328, 0x15E1, 0x15E5, 0, 7, 0 );
					AnimalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 5, 0 );

					AnimalLoreGump.AddPage( 7 );
					AnimalLoreGump.AddGump( 28, 76, 0x826 );
					AnimalLoreGump.AddHTMLGump( 47, 74, 160, 18, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2165, pSock.language ) + "</basefont>" );						 // Lore & Knowledge
					AnimalLoreGump.AddHTMLGump( 53, 92, 160, 18, 1044112, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2175, pSock.language ) + "</basefont>" );				 // Bushido
					AnimalLoreGump.AddHTMLGump( 180, 92, 100, 18, false, false, parseInt( ourObj.skills.Discordance / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 110, 160, 18, 1044113, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2176, pSock.language ) + "</basefont>" );				 // Ninjitsu
					AnimalLoreGump.AddHTMLGump( 180, 110, 100, 18, false, false, parseInt( ourObj.skills.Discordance / 10 ) );
					AnimalLoreGump.AddHTMLGump( 53, 128, 160, 18, 1044111, false, false, "<basefont color=#33310b>" + GetDictionaryEntry( 2177, pSock.language ) + "</basefont>" );				 // Chivalry
					AnimalLoreGump.AddHTMLGump( 180, 128, 100, 18, false, false, parseInt( ourObj.skills.Discordance / 10 ) );
					AnimalLoreGump.AddGump( 28, 146, 0x826 );
					AnimalLoreGump.AddHTMLGump( 47, 144, 160, 18, 1049563, false, false, "<basefont color=#0000C8>" + GetDictionaryEntry( 2178, pSock.language ) + "</basefont>" );						 // Preferred Foods

					var afoodlist = "<basefont color=#0000C8>" + GetDictionaryEntry( 2179, pSock.language ) + "</basefont>";//3000340 not sure what this is yet

					if ( ( ourObj.foodlist = "fruit" ) || ( ourObj.foodlist = "vegetable" ) )
						afoodlist = "<basefont color=#0000C8>" + GetDictionaryEntry( 2179, pSock.language ) + "</basefont>"; // Fruits and Vegetables
					else if ( ( ourObj.foodlist = "crops" ) || ( ourObj.foodlist = "hay" ) )
						afoodlist = "<basefont color=#0000C8>" + GetDictionaryEntry( 2180, pSock.language ) + "</basefont>"; // Grains and Hay
					else if ( ( ourObj.foodlist = "fish" ) )
						afoodlist = "<basefont color=#0000C8>" + GetDictionaryEntry( 2181, pSock.language ) + "</basefont>"; // Fish
					else if ( ( ourObj.foodlist = "meat" ) )
						afoodlist = "<basefont color=#0000C8>" + GetDictionaryEntry( 2182, pSock.language ) + "</basefont>"; // Meat
					else if ( ( ourObj.foodlist = "eggs" ) )
						afoodlist = "<basefont color=#0000C8>" + GetDictionaryEntry( 2183, pSock.language ) + "</basefont>"; // Eggs

					AnimalLoreGump.AddHTMLGump( 53, 164, 160, 18, false, false, afoodlist );
					AnimalLoreGump.AddGump( 28, 182, 0x826 );
					AnimalLoreGump.AddButton( 217, 328, 0x15E3, 0x15E7, 0, 6, 0 );
					AnimalLoreGump.Send( pSock );
					AnimalLoreGump.Free();
				}
				else
				{
					var UOXANimalLoreGump = new Gump;
					UOXANimalLoreGump.AddBackground( 0, 0, 300, 300, 2600 );
					UOXANimalLoreGump.AddButton( 260, 15, 4017, 1, 0, 1 );
					UOXANimalLoreGump.AddText( 120, 15, 0, ourObj.name );
					UOXANimalLoreGump.AddPage( 1 );
					addStringEntry( UOXANimalLoreGump, GetDictionaryEntry( 2135, pSock.language ) );
					addDualEntry( UOXANimalLoreGump, GetDictionaryEntry( 2136, pSock.language ), ourObj.health, ourObj.maxhp );//Hits
					addDualEntry( UOXANimalLoreGump, GetDictionaryEntry( 2137, pSock.language ), ourObj.stamina, ourObj.maxstamina );//Stamina
					addDualEntry( UOXANimalLoreGump, GetDictionaryEntry( 2138, pSock.language ), ourObj.mana, ourObj.maxmana );//Mana
					addEntry( UOXANimalLoreGump, GetDictionaryEntry( 2139, pSock.language ), ourObj.strength );//Strength
					addEntry( UOXANimalLoreGump, GetDictionaryEntry( 2140, pSock.language ), ourObj.dexterity );//Dexterity
					addEntry( UOXANimalLoreGump, GetDictionaryEntry( 2141, pSock.language ), ourObj.intelligence );//Intelligence
					addStringEntry( UOXANimalLoreGump, GetDictionaryEntry( 2184, pSock.language ) ); // Miscellaneous
					addEntry( UOXANimalLoreGump, GetDictionaryEntry( 2185, pSock.language ), ourObj.Resist( 1 ) );//Armor Rating:
					UOXANimalLoreGump.AddPageButton( 260, 260, 4005, 2 );
					UOXANimalLoreGump.AddPage( 2 );
					position = 20;
					addStringEntry( UOXANimalLoreGump, GetDictionaryEntry( 2155, pSock.language ) );//Combat Ratings
					addEntry( UOXANimalLoreGump, GetDictionaryEntry( 2156, pSock.language ), parseInt( ourObj.skills.wrestling / 10 ) );//Wrestling
					addEntry( UOXANimalLoreGump, GetDictionaryEntry( 2157, pSock.language ), parseInt( ourObj.skills.tactics / 10 ) );//tactics
					addEntry( UOXANimalLoreGump, GetDictionaryEntry( 2158, pSock.language ), parseInt( ourObj.skills.magicresistance / 10 ) );//magicresistance
					addEntry( UOXANimalLoreGump, GetDictionaryEntry( 2159, pSock.language ), parseInt( ourObj.skills.anatomy / 10 ) );//anatomy
					addEntry( UOXANimalLoreGump, GetDictionaryEntry( 2161, pSock.language ), parseInt( ourObj.skills.poisoning / 10 ) );//poisoning
					addStringEntry( UOXANimalLoreGump, GetDictionaryEntry( 2165, pSock.language ) );//Lore & Knowledge
					addEntry( UOXANimalLoreGump, GetDictionaryEntry( 2166, pSock.language ), parseInt( ourObj.skills.magery / 10 ) );//magery
					addEntry( UOXANimalLoreGump, GetDictionaryEntry( 2167, pSock.language ), parseInt( ourObj.skills.evaluatingintel / 10 ) );//evaluatingintel
					addEntry( UOXANimalLoreGump, GetDictionaryEntry( 2168, pSock.language ), parseInt( ourObj.skills.meditation / 10 ) );//meditation
					UOXANimalLoreGump.AddPageButton( 10, 260, 4014, 1 );
					UOXANimalLoreGump.Send( pSock );
					UOXANimalLoreGump.Free();
				}
			}
		}
		else
		{
			// You are not skilled enough to understand that creature
			pSock.SysMessage( GetDictionaryEntry( 2186, pSock.language ) );
		}
	}
	else
	{
		// That doesn't look much like an animal.
		pSock.SysMessage( GetDictionaryEntry( 2187, pSock.language ));
	}
}

function onGumpPress( socket, button, UOXANimalLoreGump )
{
	switch( button )
	{
	case 1:			break;
	default:		break;
	}
}

function addEntry(UOXANimalLoreGump, stringToAdd, dataToAdd )
{
	UOXANimalLoreGump.AddText( 50, position, 0, stringToAdd );
	UOXANimalLoreGump.AddText( 225, position, 0, dataToAdd.toString() );
	position += 20;
}

function addDualEntry(UOXANimalLoreGump, stringToAdd, dataToAdd, moreDataToAdd )
{
	UOXANimalLoreGump.AddText( 50, position, 0, stringToAdd );
	UOXANimalLoreGump.AddText( 225, position, 0, dataToAdd.toString() + "/" + moreDataToAdd.toString() );
	position += 20;
}

function addStringEntry(UOXANimalLoreGump, stringToAdd )
{
	position += 20;
	UOXANimalLoreGump.AddText( 40, position, 0, stringToAdd );
	position += 20;
}