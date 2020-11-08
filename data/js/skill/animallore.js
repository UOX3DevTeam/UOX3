function SkillRegistration()
{
	RegisterSkill( 2, true );	// Animal Lore
}

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
		pSock.CustomTarget( 0, GetDictionaryEntry( 867, pSock.language ) );

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ourObj && ourObj.isChar && pUser )
	{
		if( !ourObj.InRange( pUser, 3 ) )
			pSock.SysMessage( "That is too far away." );
		else if( !ourObj.npc || !ourObj.isAnimal )
			pSock.SysMessage( "That doesn't look much like an animal." );
		else if( ourObj.tamed || pUser.skills.animallore >= 700 )
		{
			if( !pUser.CheckSkill( 2, 0, 1000 ) )
				pSock.SysMessage( GetDictionaryEntry( 908, pSock.language ) );
			else
			{
				position 	= 20;
				var myGump 	= new Gump;
				myGump.AddBackground( 0, 0, 300, 300, 2600 );
				myGump.AddButton( 260, 15, 4017, 1, 0, 1 );
				myGump.AddText( 120, 15, 0, ourObj.name );
				myGump.AddPage( 1 );
				addStringEntry( myGump, "Attributes" );
				addDualEntry( myGump, "Hits:", ourObj.health, ourObj.maxhp );
				addDualEntry( myGump, "Stamina:", ourObj.stamina, ourObj.maxstamina );
				addDualEntry( myGump, "Mana:", ourObj.mana, ourObj.maxmana );
				addEntry( myGump, "Strength:", ourObj.strength );
				addEntry( myGump, "Dexterity:", ourObj.dexterity );
				addEntry( myGump, "Intelligence:", ourObj.intelligence );
				addStringEntry( myGump, "Miscellaneous" );
				addEntry( myGump, "Armor Rating:", ourObj.Resist( 1 ) );
				myGump.AddPageButton( 260, 260, 4005, 2 );
				myGump.AddPage( 2 );
				position = 20;
				addStringEntry( myGump, "Combat Ratings" );
				addEntry( myGump, "Wrestling:", parseInt(ourObj.skills.wrestling/10) );
				addEntry( myGump, "Tactics:", parseInt(ourObj.skills.tactics/10) );
				addEntry( myGump, "Magic Resistance:", parseInt(ourObj.skills.magicresistance/10) );
				addEntry( myGump, "Anatomy:", parseInt(ourObj.skills.anatomy/10) );
				addEntry( myGump, "Poisoning:", parseInt(ourObj.skills.poisoning/10) );
				addStringEntry( myGump, "Lore & Knowledge" );
				addEntry( myGump, "Magery:", parseInt(ourObj.skills.magery/10) );
				addEntry( myGump, "Evaluating Intelligence:", parseInt(ourObj.skills.evaluatingintel/10) );
				addEntry( myGump, "Meditation:", parseInt(ourObj.skills.meditation/10) );
				myGump.AddPageButton( 10, 260, 4014, 1 );
				myGump.Send( pSock );
				myGump.Free;
			}
		}
		else
			pSock.SysMessage( "You are not skilled enough to understand that creature" );
	}
	else
		pSock.SysMessage( "That doesn't look much like an animal." );
}

function onGumpPress( socket, button, myGump )
{
	switch( button )
	{
	case 1:			break;
	default:		break;
	}
}

function addEntry( myGump, stringToAdd, dataToAdd )
{
	myGump.AddText( 50, position, 0, stringToAdd );
	myGump.AddText( 225, position, 0, dataToAdd.toString() );
	position += 20;
}

function addDualEntry( myGump, stringToAdd, dataToAdd, moreDataToAdd )
{
	myGump.AddText( 50, position, 0, stringToAdd );
	myGump.AddText( 225, position, 0, dataToAdd.toString() + "/" + moreDataToAdd.toString() );
	position += 20;
}

function addStringEntry( myGump, stringToAdd )
{
	position += 20;
	myGump.AddText( 40, position, 0, stringToAdd );
	position += 20;
}