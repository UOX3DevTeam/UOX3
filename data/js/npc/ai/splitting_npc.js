// When attached to a character, this script will check, after being attacked in combat,
// whether the character is allowed to split into copies of itself. Primary example of this
// would be a slime NPC, which has a chance to split into two upon being attacked

function onDefense( pAttacker, pDefender )
{
	if( pDefender.split > 0 )
	{
		if( RandomNumber( 0, 100 ) <= pDefender.splitchance )
		{
			var splitNum = RandomNumber( 1, pDefender.split );

			for( var i = 0; i < splitNum; i++ )
			{
				var dupeChar = pDefender.Dupe();
				if( !ValidateObject( dupeChar ))
					continue;

				// Set some specific properties for the new NPC
				dupeChar.atWar = false;
				dupeChar.target = null;
				dupeChar.Follow( null );
				dupeChar.wandertype = 2;
				dupeChar.Teleport( pDefender.x + 1, pDefender.y, pDefender.z, pDefender.worldnumber, pDefender.instanceID );
				dupeChar.murdercount = 0;
				dupeChar.health = pDefender.maxhp;
				dupeChar.stamina = pDefender.maxstamina;
				dupeChar.mana = pDefender.maxmana;

				// Give the new NPC a chance to split again in the future?
				if( RandomNumber( 0, 34 ) == 5 )
				{
					dupeChar.split = 1;
				}
				else
				{
					dupeChar.split = 0;
				}
			}
		}
	}
}