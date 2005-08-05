function onUse( pUser, iUsed )
{
	var pSock = pUser.socket;
	
	//Check if user is in range of combat dummy
	if( !iUsed.InRange( pUser, 1 ) )
	{
		pUser.SysMessage( GetDictionaryEntry( 482, pSock.Language )); //You need to be closer to use that.
		return false;
	}
	else if( iUsed.id == 0x1070 || iUsed.id == 0x1074 ) //if training dummy is motionless
	{
		//Determine weapon-type by calling external script and loading a value set there afterwards
		TriggerEvent( 2500, "getWeaponType", pUser );
		var weaponType = pUser.GetTag( "weaponType" );
		
		if( weaponType == "BOWS" || weaponType == "XBOWS" )
		{
			pUser.SysMessage( GetDictionaryEntry( 938, pSock.Language )); //Practice archery on archery buttes!
			return false;
		}
		
		//Check if character is mounted or not, and then call up an external script to determine combat animations
		if( pUser.isonhorse )
			TriggerEvent( 2501, "getHorseCombatAnim", pUser, weaponType );
		else
			TriggerEvent( 2501, "getFootCombatAnim", pUser, weaponType );
		var combatAnim = StringToNum( pUser.GetTag( "combatAnim" )); //convert to decimal for use with DoAction
		pUser.DoAction( combatAnim );

		//Remove the temporary tags we set on our character above
		pUser.SetTag( "weaponType", null );
		pUser.SetTag( "combatAnim", null );
		
		//Play some random sound effects when the training dummy is hit
		switch( RandomNumber( 0, 2 ) ) 
		{
			case 0: pUser.SoundEffect( 0x013B, true );	break;
			case 1: pUser.SoundEffect( 0x013C, true );	break; 
			case 2: pUser.SoundEffect( 0x013D, true );	break;
			default:  ConsoleMessage( " cSkills::TDummy -> Fallout of switch statement without default" );	return false;
		}
		
		//Change the motionless dummy to a swinging dummy!
		if( iUsed.id == 0x1070 )
			iUsed.id++;
		else if( iUsed.id == 0x1074 )
			iUsed.id++;
			
		//Star a timer so the dummy doesn't swing forever
		iUsed.StartTimer( 3000, 1, true );
		
		//Check the player's tactics skill to see if he gets chance to gain more skill
		if( pUser.skills.tactics > 250 )
			pUser.SysMessage( GetDictionaryEntry( 939, pSock.Language )); //You feel you would gain no more from using that.
		else
			pUser.CheckSkill( 27, 0, 250 );
	}
	else
		pUser.SysMessage( GetDictionaryEntry( 483, pSock.Language )); //You must wait for it to stop swinging!
	return false;
}

function onTimer( iUsed, timerID )
{
	//If timer is 1, stop the swinging dummy
	if( timerID == 1 )
	{
		if( iUsed.id == 0x1071 )
			iUsed.id--;
		else if( iUsed.id == 0x1075 )
			iUsed.id--;
	}	
}
