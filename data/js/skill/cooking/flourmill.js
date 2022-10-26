function onUseChecked ( pUser, iUsed )
{
	var srcSock = pUser.socket;

	if( !iUsed.InRange( pUser, 3 ))
	{
		srcSock.SysMessage( GetDictionaryEntry( 393, srcSock.language )); // That is too far away.
		return;
	}

	var millStatus = iUsed.GetTag( "millStatus" );
	switch( millStatus )
	{
		case 0: // No tag/not in use. Reset ID just to be sure
			if( iUsed.id == 0x1923 || iUsed.id == 0x1927 )
			{
				iUsed.id = 0x1922;
			}
			else if( iUsed.id == 0x192f || iUsed.id == 0x1933 )
			{
				iUsed.id = 0x192e;
			}
			AreaItemFunction( "myAreaFunc", iUsed, 1 );
			break;
		case 1: // Busy grinding wheat
			srcSock.SysMessage( GetDictionaryEntry( 6103, tSock.language )); // The flour mill is currently busy.
			break;
		case 2: // Done grinding wheat, awaiting pickup
			CreateDFNItem( srcSock, pUser, "0x1045", 1, "ITEM", true ); // makes a sack of flour
			srcSock.SysMessage( GetDictionaryEntry( 6083, srcSock.language )); // You grind some wheat and put a sack of flour in your pack!
			AreaItemFunction( "myAreaFunc", iUsed, 1 );
			if( iUsed.id == 0x1927 )
			{
				iUsed.id = 0x1922;
			}
			else if( iUsed.id == 0x1933 )
			{
				iUsed.id = 0x192e;
			}
			iUsed.SetTag( "millStatus", 0 );
			break;
	}
}

// Find second part of flour mill and change its ID
function myAreaFunc( srcChar, trgItem, iUsed )
{
	if( trgItem.id == 0x1921 )
	{
		trgItem.id = 0x1920;
	}
	else if( trgItem.id == 0x192d )
	{
		trgItem.id = 0x192c;
	}
}

function FlourMill( tileID )
{
	return ( tileID == 0x188b || ( tileID >= 0x1920 && tileID <= 0x1923 ) || ( tileID >= 0x1925 && tileID <= 0x1927 ) ||
		( tileID >= 0x192c && tileID <= 0x192f ) || ( tileID >= 0x1931 && tileID <= 0x1933 ));
}