function onDeath( pDead, iCorpse )
{
	iCorpse.StaticEffect( 0x36B0, 0, 0x09 );
	iCorpse.SoundEffect( 0x0207, true );
	AreaCharacterFunction( "DealAreaDamage", iCorpse, 3 );
}

function DealAreaDamage( srcObj, trgChar )
{
	if( ValidateObject( trgChar ) && trgChar != srcObj && trgChar.vulnerable && !trgChar.dead
		&& (( trgChar.npc && ( trgChar.tamed || trgChar.hireling )) || trgChar.online ))
	{
		trgChar.Damage( 30, 1 );
	}
}