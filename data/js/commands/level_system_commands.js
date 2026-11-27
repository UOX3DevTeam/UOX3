/// <reference path="../definitions.d.ts" />
// @ts-check

function CommandRegistration()
{
	RegisterCommand( "levelinfo", 0, true );
	RegisterCommand( "abilities", 0, true );
	RegisterCommand( "professions", 0, true );
	RegisterCommand( "addxp", 8, true );
	RegisterCommand( "setlevel", 8, true );
	RegisterCommand( "resetlevel", 8, true );
	RegisterCommand( "setclass", 8, true );
	RegisterCommand( "setprofession", 8, true );
	RegisterCommand( "forgetprofession", 8, true );
	RegisterCommand( "resetprofessions", 8, true );
}

function command_PROFESSIONS( socket, cmdString )
{
	if( !GetServerSetting( "LevelSystemEnabled" ) || !GetServerSetting( "LevelSystemCrafting" ))
	{
		socket.SysMessage( "The level-system professions are disabled." );
		return;
	}
	TriggerEvent( 2509, "DisplayProfessionGump", socket.currentChar, 0 );
}

function command_ABILITIES( socket, cmdString )
{
	if( !GetServerSetting( "LevelSystemEnabled" ))
	{
		socket.SysMessage( "The level system is disabled." );
		return;
	}
	if( socket.currentChar.levelClass == 0 )
	{
		TriggerEvent( 2509, "DisplayLevelSystem", socket.currentChar );
		return;
	}
	TriggerEvent( 2509, "DisplayClassAbilityGump", socket.currentChar, 0 );
}

function command_LEVELINFO( socket, cmdString )
{
	if( !GetServerSetting( "LevelSystemEnabled" ))
	{
		socket.SysMessage( "The level system is disabled." );
		return;
	}
	TriggerEvent( 2509, "DisplayLevelSystem", socket.currentChar );
}

function command_ADDXP( socket, cmdString )
{
	const amount = Number( cmdString );
	if( !Number.isSafeInteger( amount ) || amount <= 0 || amount > 2147483647 )
	{
		socket.SysMessage( "Usage: addxp <positive amount>" );
		return;
	}
	socket.tempInt = amount;
	socket.CustomTarget( 0, "Select the player who should receive experience." );
}

function onCallback0( socket, target )
{
	if( !ValidPlayerTarget( socket, target ))
		return;
	const amount = socket.tempInt;
	const levelsGained = target.AddExperience( amount );
	socket.SysMessage( "Added " + amount + " experience to " + target.name + " (" + levelsGained + " level(s) gained)." );
	ShowTargetProgress( target );
}

function command_SETLEVEL( socket, cmdString )
{
	const level = Number( cmdString );
	if( !Number.isInteger( level ) || level < 1 || level > Number( GetServerSetting( "LevelCap" )))
	{
		socket.SysMessage( "Usage: setlevel <1-LevelCap>" );
		return;
	}
	socket.tempInt = level;
	socket.CustomTarget( 1, "Select the player whose level should be changed." );
}

function onCallback1( socket, target )
{
	if( !ValidPlayerTarget( socket, target ))
		return;
	if( target.SetLevelProgress( socket.tempInt ))
	{
		socket.SysMessage( target.name + " is now level " + target.level + "." );
		ShowTargetProgress( target );
	}
}

function command_RESETLEVEL( socket, cmdString )
{
	const clearClass = String( cmdString ).trim().toLowerCase() == "clearclass";
	socket.tempInt = clearClass ? 1 : 0;
	socket.CustomTarget( 2, clearClass ? "Select the player whose progression and class should be reset." : "Select the player whose progression should be reset." );
}

function onCallback2( socket, target )
{
	if( !ValidPlayerTarget( socket, target ))
		return;
	const clearedClass = socket.tempInt == 1;
	target.ResetLevelProgress( clearedClass );
	socket.SysMessage( "Reset level progression for " + target.name + ( clearedClass ? " and cleared the class." : "." ));
	ShowTargetProgress( target );
}

function command_SETCLASS( socket, cmdString )
{
	const classId = FindClassId( String( cmdString ).trim() );
	if( classId < 0 )
	{
		socket.SysMessage( "Usage: setclass <class id, class name, or none>" );
		return;
	}
	socket.tempInt = classId;
	socket.CustomTarget( 3, "Select the player whose class should be changed." );
}

function onCallback3( socket, target )
{
	if( !ValidPlayerTarget( socket, target ))
		return;
	const classId = socket.tempInt;
	if( target.SetLevelClass( classId, true ))
	{
		const className = classId == 0 ? "Unselected" : String( GetLevelClassProperty( classId, "NAME" ));
		socket.SysMessage( target.name + " is now class " + className + "." );
		ShowTargetProgress( target );
	}
}

function command_SETPROFESSION( socket, cmdString )
{
	const match = String( cmdString ).trim().match( /^(.*?)\s+(\d+(?:\.\d+)?)$/ );
	if( match == null )
	{
		socket.SysMessage( "Usage: setprofession <profession id or name> <0.0-100.0>" );
		return;
	}
	const professionId = FindProfessionId( match[1] );
	const proficiency = Math.round( Number( match[2] ) * 10 );
	if( professionId < 1 )
	{
		socket.SysMessage( "That profession does not exist." );
		return;
	}
	const minimum = Number( GetLevelProfessionProperty( professionId, "STARTVALUE" ) || 0 );
	const maximum = Number( GetLevelProfessionProperty( professionId, "MAXVALUE" ) || 1000 );
	if( !Number.isInteger( proficiency ) || proficiency < minimum || proficiency > maximum )
	{
		socket.SysMessage( "The proficiency must be between " + ( minimum / 10 ).toFixed( 1 ) + " and " + ( maximum / 10 ).toFixed( 1 ) + "." );
		return;
	}
	socket.currentChar.SetTempTag( "adminProfessionId", professionId );
	socket.currentChar.SetTempTag( "adminProfessionValue", proficiency );
	socket.CustomTarget( 4, "Select the player whose profession progress should be changed." );
}

function onCallback4( socket, target )
{
	if( !ValidPlayerTarget( socket, target ))
		return;
	const professionId = Number( socket.currentChar.GetTempTag( "adminProfessionId" ) || 0 );
	const proficiency = Number( socket.currentChar.GetTempTag( "adminProfessionValue" ) || 0 );
	socket.currentChar.SetTempTag( "adminProfessionId", null );
	socket.currentChar.SetTempTag( "adminProfessionValue", null );
	if( professionId > 0 && target.SetProfessionProgress( professionId, proficiency, true ))
	{
		const name = String( GetLevelProfessionProperty( professionId, "NAME" ));
		const actualProficiency = target.GetProfessionProficiency( professionId );
		socket.SysMessage( target.name + " now has " + ( actualProficiency / 10 ).toFixed( 1 ) + " " + name + "." );
		ShowTargetProfessions( target );
	}
}

function command_FORGETPROFESSION( socket, cmdString )
{
	const professionId = FindProfessionId( String( cmdString ).trim() );
	if( professionId < 1 )
	{
		socket.SysMessage( "Usage: forgetprofession <profession id or name>" );
		return;
	}
	socket.tempInt = professionId;
	socket.CustomTarget( 5, "Select the player who should forget this profession." );
}

function onCallback5( socket, target )
{
	if( !ValidPlayerTarget( socket, target ))
		return;
	const professionId = socket.tempInt;
	const name = String( GetLevelProfessionProperty( professionId, "NAME" ));
	if( target.ForgetProfession( professionId ))
	{
		socket.SysMessage( target.name + " has forgotten " + name + " and lost its progress." );
		ShowTargetProfessions( target );
	}
	else
	{
		socket.SysMessage( target.name + " has not learned " + name + "." );
	}
}

function command_RESETPROFESSIONS( socket, cmdString )
{
	socket.CustomTarget( 6, "Select the player whose professions should be reset." );
}

function onCallback6( socket, target )
{
	if( !ValidPlayerTarget( socket, target ))
		return;
	target.ResetProfessions();
	socket.SysMessage( "Reset all professions for " + target.name + "." );
	ShowTargetProfessions( target );
}

function FindClassId( value )
{
	if( value.toLowerCase() == "none" )
		return 0;
	const numericId = Number( value );
	if( Number.isInteger( numericId ) && numericId > 0 && numericId <= GetLevelClassCount() )
		return numericId;
	for( let classId = 1; classId <= GetLevelClassCount(); ++classId )
	{
		if( String( GetLevelClassProperty( classId, "NAME" )).toLowerCase() == value.toLowerCase() )
			return classId;
	}
	return -1;
}

function FindProfessionId( value )
{
	const numericId = Number( value );
	if( Number.isInteger( numericId ) && numericId > 0 && numericId <= GetLevelProfessionCount() )
		return numericId;
	for( let professionId = 1; professionId <= GetLevelProfessionCount(); ++professionId )
	{
		if( String( GetLevelProfessionProperty( professionId, "NAME" )).toLowerCase() == value.toLowerCase() )
			return professionId;
	}
	return -1;
}

function ValidPlayerTarget( socket, target )
{
	if( socket.GetWord( 1 ) || !ValidateObject( target ) || !target.isChar || target.npc )
	{
		socket.SysMessage( "You must target a player character." );
		return false;
	}
	return true;
}

function ShowTargetProgress( target )
{
	if( target.socket != null )
		TriggerEvent( 2509, "DisplayLevelSystem", target );
}

function ShowTargetProfessions( target )
{
	if( target.socket != null )
		TriggerEvent( 2509, "DisplayProfessionGump", target, 0 );
}
