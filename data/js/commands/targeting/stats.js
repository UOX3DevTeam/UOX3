function CommandRegistration()
{
	RegisterCommand( "fullstats", 2, true );
	RegisterCommand( "heal", 1, true );
	RegisterCommand( "stamina", 1, true );
	RegisterCommand( "mana", 1, true );
}

function command_FULLSTATS( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 243, socket.Language );
	socket.CustomTarget( 0, targMsg );
}

function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		ourObj.SoundEffect( 0x01F2, true );
		ourObj.StaticEffect( 0x376A, 0x09, 0x06 );
		ourObj.mana 	= ourObj.maxmana;
		ourObj.health 	= ourObj.maxhp;
		ourObj.stamina	= ourObj.maxstamina;
	}
}

function command_HEAL( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 227, socket.Language );
	socket.CustomTarget( 1, targMsg );
}

function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		var mSpell = Spells[4];
		ourObj.MagicEffect( 4 );
		ourObj.SoundEffect( mSpell.soundEffect, true );
		ourObj.health = ourObj.maxhp;
	}
}

function command_STAMINA( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 231, socket.Language );
	socket.CustomTarget( 2, targMsg );
}

function onCallback2( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		ourObj.SoundEffect( 0x01F2, true );
		ourObj.StaticEffect( 0x376A, 0x09, 0x06 );
		ourObj.stamina = ourObj.maxstamina;
	}
}

function command_MANA( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 230, socket.Language );
	socket.CustomTarget( 3, targMsg );
}

function onCallback3( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		ourObj.SoundEffect( 0x01F2, true );
		ourObj.StaticEffect( 0x376A, 0x09, 0x06 );
		ourObj.mana = ourObj.maxmana;
	}
}

