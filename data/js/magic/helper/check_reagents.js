/// <reference path="../../definitions.d.ts" />
// @ts-check
function CheckReagents( mChar, mSpell )
{
	for( var i = 0; i < mSpell.reagents.length; ++i )
	{
		var reagent = mSpell.reagents[i];
		var reagentCount = 0;
		for( var sectionIndex = 0; sectionIndex < reagent[2].length; ++sectionIndex )
		{
			reagentCount += reagent.length > 3 ? mChar.ResourceCount( reagent[0], reagent[3], -1, reagent[2][sectionIndex] ) : mChar.ResourceCount( reagent[0], -1, -1, reagent[2][sectionIndex] );
		}
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
		var amountRemaining = reagent[1];
		for( var sectionIndex = 0; sectionIndex < reagent[2].length && amountRemaining > 0; ++sectionIndex )
		{
			var available = reagent.length > 3 ? mChar.ResourceCount( reagent[0], reagent[3], -1, reagent[2][sectionIndex] ) : mChar.ResourceCount( reagent[0], -1, -1, reagent[2][sectionIndex] );
			var amountToUse = Math.min( amountRemaining, available );
			if( reagent.length > 3 )
			{
				mChar.UseResource( amountToUse, reagent[0], reagent[3], -1, reagent[2][sectionIndex] );
			}
			else
			{
				mChar.UseResource( amountToUse, reagent[0], -1, -1, reagent[2][sectionIndex] );
			}
			amountRemaining -= amountToUse;
		}
	}
}
