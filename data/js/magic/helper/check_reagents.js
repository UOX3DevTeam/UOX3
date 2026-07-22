/// <reference path="../../definitions.d.ts" />
// @ts-check
function CheckReagents( mChar, mSpell )
{
	for( var i = 0; i < mSpell.reagents.length; ++i )
	{
		var reagent = mSpell.reagents[i];
		var reagentCount = reagent.length > 3 ? mChar.ResourceCount( reagent[0], reagent[3], -1, reagent[2] ) : mChar.ResourceCount( reagent[0], -1, -1, reagent[2] );
		if( reagentCount < reagent[1] )
		{
			if( mChar.socket != null )
			{
				mChar.socket.SysMessage( GetDictionaryEntry( 702, mChar.socket.language )); // You do not have enough reagents to cast that spell.
			}
			return false;
		}
	}
	return true;
}

function DeleteReagents( mChar, mSpell )
{
	for( var i = 0; i < mSpell.reagents.length; ++i )
	{
		var reagent = mSpell.reagents[i];
		if( reagent.length > 3 )
		{
			mChar.UseResource( reagent[1], reagent[0], reagent[3], -1, reagent[2] );
		}
		else
		{
			mChar.UseResource( reagent[1], reagent[0], -1, -1, reagent[2] );
		}
	}
}
