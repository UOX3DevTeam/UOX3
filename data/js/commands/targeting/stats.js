/// <reference path="../../definitions.d.ts" />
// @ts-check
// GM commands to adjust character stats

function CommandRegistration()
{
	RegisterCommand( "fullstats", 8, true );
	RegisterCommand( "heal", 4, true );
	RegisterCommand( "cure", 4, true );
	RegisterCommand( "stamina", 4, true );
	RegisterCommand( "mana", 4, true );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_FULLSTATS( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 243, socket.language ); // Select creature to restore full stats.
	socket.CustomTarget( 0, targMsg );
}

/** @type { ( tSock: Socket, target: BaseObject | null ) => void } */
function onCallback0( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ( ourObj.isChar || (( ourObj.isItem || ourObj.isMulti ) && ourObj.isDamageable )))
	{
		ourObj.SoundEffect( 0x01F2, true );
		ourObj.StaticEffect( 0x376A, 0x09, 0x06 );
		ourObj.health 	= ourObj.maxhp;

		if( ourObj.isChar )
		{
			ourObj.mana 	= ourObj.maxmana;
			ourObj.stamina	= ourObj.maxstamina;
		}
		else
		{
			ourObj.UpdateStats( 0 );
		}
	}
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_HEAL( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 227, socket.language ); // Select person to heal.
	socket.CustomTarget( 1, targMsg );
}

/** @type { ( tSock: Socket, target: BaseObject | null ) => void } */
function onCallback1( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ( ourObj.isChar || (( ourObj.isItem || ourObj.isMulti ) && ourObj.isDamageable )))
	{
		var mSpell = Spells[4];
		if( ourObj.isChar )
		{
			ourObj.MagicEffect( 4 );
		}

		ourObj.SoundEffect( mSpell.soundEffect, true );
		ourObj.health = ourObj.maxhp;

		if( !ourObj.isChar )
		{
			ourObj.UpdateStats( 0 );
		}
	}
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_STAMINA( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 231, socket.language ); // Select person to refresh.
	socket.CustomTarget( 2, targMsg );
}

/** @type { ( tSock: Socket, target: BaseObject | null ) => void } */
function onCallback2( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		ourObj.SoundEffect( 0x01F2, true );
		ourObj.StaticEffect( 0x376A, 0x09, 0x06 );
		ourObj.stamina = ourObj.maxstamina;
	}
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_MANA( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 230, socket.language ); // Select person to restore mana to.
	socket.CustomTarget( 3, targMsg );
}

/** @type { ( tSock: Socket, target: BaseObject | null ) => void } */
function onCallback3( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		ourObj.SoundEffect( 0x01F2, true );
		ourObj.StaticEffect( 0x376A, 0x09, 0x06 );
		ourObj.mana = ourObj.maxmana;
	}
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_CURE( socket, cmdString )
{
	var targMsg = GetDictionaryEntry( 1974, socket.language ); // Select person to cure.
	socket.CustomTarget( 4, targMsg );
}

/** @type { ( tSock: Socket, target: BaseObject | null ) => void } */
function onCallback4( socket, ourObj )
{
	if( !socket.GetWord( 1 ) && ourObj.isChar )
	{
		var mSpell = Spells[4];
		ourObj.MagicEffect( 4 );
		ourObj.SoundEffect( mSpell.soundEffect, true );
		ourObj.SetPoisoned( 0, 0 );
	}
}
