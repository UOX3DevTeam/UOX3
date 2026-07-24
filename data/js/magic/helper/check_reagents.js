/// <reference path="../../definitions.d.ts" />
// @ts-check
function CheckReagents( mChar, mSpell )
{
	var anyItemID = 0xffff; // Match reagents by section ID instead of item ID
	for( var i = 0; i < mSpell.reagents.length; ++i )
	{
		var reagent = mSpell.reagents[i];
		var reagentCount = reagent.length > 2 ? mChar.ResourceCount( anyItemID, reagent[2], -1, reagent[0] ) : mChar.ResourceCount( anyItemID, -1, -1, reagent[0] );
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
	var anyItemID = 0xffff; // Match reagents by section ID instead of item ID
	for( var i = 0; i < mSpell.reagents.length; ++i )
	{
		var reagent = mSpell.reagents[i];
		if( reagent.length > 2 )
		{
			mChar.UseResource( reagent[1], anyItemID, reagent[2], -1, reagent[0] );
		}
		else
		{
			mChar.UseResource( reagent[1], anyItemID, -1, -1, reagent[0] );
		}
	}
}
