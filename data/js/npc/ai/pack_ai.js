// Simple Pack "AI"
// When one member of a pack is attacked (or attacks), they call upon any nearby members of the pack who are
// within line of sight to also attack the same target
// "Pack" is simply defined as any other creature of same type within range at a given time

function onCombatStart( pAttacker, pDefender )
{
	AreaCharacterFunction( "AlertPack", pAttacker, 18 );
	return true;
}
	
function onDamage( damaged, attacker, damageValue, damageType )
{
	AreaCharacterFunction( "AlertPack", damaged, 18 );
	return true;
}

function AlertPack( srcChar, packMember )
{
	var myTarget = srcChar.target;
	if( !ValidateObject( myTarget ) || ( !myTarget.npc && ( myTarget.dead || !myTarget.online )))
		return false;

	if( ValidateObject( packMember ) && packMember.isChar && packMember.npc )
	{
		if( packMember.id == srcChar.id && packMember.aitype == srcChar.aitype )
		{
			if( !packMember.atWar && packMember.CanSee( srcChar ))
			{
				return tempPet.InitiateCombat( myTarget );
			}
		}
	}
	return false;
}