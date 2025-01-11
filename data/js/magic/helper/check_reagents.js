function CheckReagents( mChar, mSpell )
{
	var failedCheck = 0;
	if( mSpell.ash > 0 && mChar.ResourceCount( 0x0F8C ) < mSpell.ash )
	{
		failedCheck = 1;
	}
	if( mSpell.drake > 0 && mChar.ResourceCount( 0x0F86 ) < mSpell.drake )
	{
		failedCheck = 1;
	}
	if( mSpell.garlic > 0 && mChar.ResourceCount( 0x0F84 ) < mSpell.garlic )
	{
		failedCheck = 1;
	}
	if( mSpell.ginseng > 0 && mChar.ResourceCount( 0x0F85 ) < mSpell.ginseng )
	{
		failedCheck = 1;
	}
	if( mSpell.moss > 0 && mChar.ResourceCount( 0x0F7B ) < mSpell.moss )
	{
		failedCheck = 1;
	}
	if( mSpell.pearl > 0 && mChar.ResourceCount( 0x0F7A ) < mSpell.pearl )
	{
		failedCheck = 1;
	}
	if( mSpell.shade > 0 && mChar.ResourceCount( 0x0F88 ) < mSpell.shade )
	{
		failedCheck = 1;
	}
	if( mSpell.silk > 0 && mChar.ResourceCount( 0x0F8D ) < mSpell.silk )
	{
		failedCheck = 1;
	}
	if( mSpell.gravedust > 0 && mChar.ResourceCount( 0xF8F ) < mSpell.gravedust )
	{
		failedCheck = 1;
	}
	if( mSpell.daemonblood > 0 && mChar.ResourceCount( 0xF7D ) < mSpell.daemonblood )
	{
		failedCheck = 1;
	}
	if( failedCheck == 1 )
	{
		if( mChar.socket != null )
		{
			mChar.socket.SysMessage( GetDictionaryEntry( 702, mChar.socket.language )); // You do not have enough reagents to cast that spell.
		}
		return false;
	}
	//else
	//{
		return true;
	//}
}

function DeleteReagents( mChar, mSpell )
{
	mChar.UseResource( mSpell.pearl, 0x0F7A );
	mChar.UseResource( mSpell.moss, 0x0F7B );
	mChar.UseResource( mSpell.garlic, 0x0F84 );
	mChar.UseResource( mSpell.ginseng, 0x0F85 );
	mChar.UseResource( mSpell.drake, 0x0F86 );
	mChar.UseResource( mSpell.shade, 0x0F88 );
	mChar.UseResource( mSpell.ash, 0x0F8C );
	mChar.UseResource( mSpell.silk, 0x0F8D );
	mChar.UseResource( mSpell.gravedust, 0xF8F );
	mChar.UseResource( mSpell.daemonblood, 0xF7D );
}