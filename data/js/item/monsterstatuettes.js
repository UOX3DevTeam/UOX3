function onCreateDFN( objMade, objType )
{
		var rewardYear = "";
		switch( objMade.id )
		{
			case 0x20DA: // Crocodile
			case 0x20D3: // Daemon
			case 0x20D6: // Dragon
			case 0x20D7: // Earth Elemental
			case 0x20D8: // Ettin
			case 0x20D9: // Gargoyle
			case 0x20F5: // Gorilla
			case 0x20F8: // Lich
			case 0x20DE: // Lizardman
			case 0x20DF: // Ogre
			case 0x2133: // Ophidian
			case 0x20E0: // Orc
			case 0x20E3: // Ratman
			case 0x20E7: // Skeleton
			case 0x20E9: // Troll
				rewardYear = "1st year";
				break;
			case 0x20EC: // Zombie
			case 0x2103: // Cow
			case 0x20F6: // Llama
				rewardYear = "3rd year";
				break;
		}
		objMade.SetTag( "rewardYear", rewardYear );
}

function onUseChecked( pUser, monsterStatue )
{
	var turnedOnOff = monsterStatue.GetTag( "turnedOnOff" );
	var onOff = 0;
	var soundEffect = 0;
	if( turnedOnOff == 0 )
	{
		switch( monsterStatue.id )
		{
			case 0x2103: // Cow
				soundEffect = 120;
				onOff = 1;
				break;
			case 0x20DA: // Crocodile
				soundEffect = 660;
				onOff = 1;
				break;
			case 0x20D3: // Daemon
				soundEffect = 357;
				onOff = 1;
				break;
			case 0x20D6: // Dragon
				soundEffect = 362;
				onOff = 1;
				break;
			case 0x20D7: // Earth Elemental
				soundEffect = 268;
				onOff = 1;
				break;
			case 0x20D8: // Ettin
				soundEffect = 367;
				onOff = 1;
				break;
			case 0x20D9: // Gargoyle
				soundEffect = 372;
				onOff = 1;
				break;
			case 0x20F5: // Gorilla
				soundEffect = 158;
				onOff = 1;
				break;
			case 0x20F8: // Lich
				soundEffect = 1001;
				onOff = 1;
				break;
			case 0x20DE: // Lizardman
				soundEffect = 417;
				onOff = 1;
				break;
			case 0x20F6: // Llama
				soundEffect = 1011;
				onOff = 1;
				break;
			case 0x20DF: // Ogre
				soundEffect = 427;
				onOff = 1;
				break;
			case 0x2133: // Ophidian
				soundEffect = 634;
				onOff = 1;
				break;
			case 0x20E0: // Orc
				soundEffect = 1114;
				onOff = 1;
				break;
			case 0x20E3: // Ratman
				soundEffect = 437;
				onOff = 1;
				break;
			case 0x20E7: // Skeleton
				soundEffect = 1165;
				onOff = 1;
				break;
			case 0x20E9: // Troll
				soundEffect = 461;
				onOff = 1;
				break;
			case 0x20EC: // Zombie
				soundEffect = 471;
				onOff = 1;
				break;
		}
	}
	else
	{
		onOff = 0;
		soundEffect = 0x0
	}
	monsterStatue.SetTag( "turnedOnOff", onOff );
	monsterStatue.SetTag( "soundEffect", soundEffect );
	monsterStatue.Refresh();
}

function onMoveDetect( monsterStatue, pChar, rangeToChar, oldCharX, oldCharY )
{
	var soundChance = RandomNumber( 1, 500 );
	var soundEffect = monsterStatue.GetTag( "soundEffect" );
	var turnedOnOff = monsterStatue.GetTag( "turnedOnOff" );

	if( !ValidateObject( pChar ) || !pChar.isChar || pChar.npc || pChar.dead || pChar.isGM || pChar.isCounselor || pChar.visible == 3 )
		return true;

	if( rangeToChar <= 0x02  && monsterStatue.movable == 3 && turnedOnOff == 1 )
	{
		if( soundChance < 25 )
		{
			monsterStatue.SoundEffect( soundEffect, true );
		}
	}
	return true;
}

function onTooltip( monsterStatue, pSocket )
{
	var tooltipText = "";
	var turnedOnOff = monsterStatue.GetTag( "turnedOnOff" );
	var rewardYear = monsterStatue.GetTag( "rewardYear" );
	tooltipText = rewardYear + " veteran reward";
	if( turnedOnOff == 1 )
	{
		tooltipText += "\n Turned On";
	}
	else 
	{
		tooltipText += "\n Turned Off";
	}
	return tooltipText;
}