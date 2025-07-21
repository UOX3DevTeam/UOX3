function onSpellTargetSelect( myTarget, pCaster, spellID )
{
	if( !ValidateObject( pCaster ))
		return 2;

	if( pCaster.region.id == 42 ) // Khaldun
	{
		var socket = pCaster.socket;
		if( spellID == 22 ) // Teleport
		{
			if( socket != null )
			{
				pCaster.SysMessage( "Thou canst not use teleportation spells in this place!" );
			}
			return 2;
		}
		else if( spellID == 59 ) // Resurrect
		{
			if( socket != null )
			{
				pCaster.SysMessage( "The veil of death in this area is too strong and resists thy efforts to restore life." );
			}
			if( ValidateObject( myTarget ) && myTarget.isChar && !myTarget.npc )
			{
				var targSock = myTarget.socket;
				if( targSock != null )
				{
					myTarget.SysMessage( pCaster.name + " attempted to ressurect thee, but the veil of death is too strong in this area and prevents it!" );
				}
			}
			return 2;
		}
	}

	// default to code
	return 0;
}