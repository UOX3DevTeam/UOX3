function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pUser.SysMessage( "This must be in your backpack or equipped before it can be used." );
			return false;
		}
		else if( iUsed.type != 15 )
		{
			var targMsg = GetDictionaryEntry( 443, socket.Language );
			socket.CustomTarget( 1, targMsg );
		}
		else
			return true;		
	}
	return false;
}

function onCallback1( socket, ourObj )
{
	var mChar = socket.currentChar;

	if( mChar && mChar.isChar )
	{
		var tileID = 0;
		if( socket.GetByte( 1 ) )
		{
			tileID = socket.GetWord( 17 );
			if( !tileID )
				tileID = GetTileIDAtMapCoord( socket.GetWord( 11 ), socket.GetWord( 13 ), mChar.worldNumber );
		}
		else if( ourObj && ourObj.isItem )
			tileID = ourObj.id;
			

		if( tileID != 0 )
		{
			if( 	tileID == 0x0CD0 || tileID == 0x0CD3 || tileID == 0x0CD6 || tileID == 0x0CD8 || tileID == 0x0CDA || 
				tileID == 0x0CDD || tileID == 0x0CE0 || tileID == 0x0CE3 || tileID == 0x0CE6 || tileID == 0x0D58 || 
				( tileID >= 0x0CCA && tileID <= 0x0CCE ) || ( tileID >= 0x12B8 && tileID <= 0x12BB ) || tileID == 0x0D42 ||
				tileID == 0x0D43 || tileID == 0x0D58 || tileID == 0x0D59 || tileID == 0x0D70 || tileID == 0x0D85 || 
				tileID == 0x0D94 || tileID == 0x0D95 || tileID == 0x0D98 || tileID == 0x0DA4 || tileID == 0x0DA8 ) // Trees
			{
				ChopTree( socket, mChar );
			}
			else if( ourObj )
			{
				if( tileID >= 0x1BD7 && tileID <= 0x1BE2 )	// Bowcraft
					BowCraft( socket, mChar, ourObj, tileID );
				else if( tileID == 0x2006 )
					CarveCorpse( socket, mChar, ourObj );
				else
					socket.SysMessage( "You cannot carve that." );
			}
			else
				socket.SysMessage( "You cannot chop that" );
		}
		else
			socket.SysMessage( "You cannot chop that" );
	}
}

function BowCraft( socket, mChar, ourObj, tileID )
{
	var ownerObj = GetPackOwner( ourObj, 0 );
	if( ownerObj && mChar.serial == ownerObj.serial )
		socket.MakeMenu( 49, 8 );
	else
		socket.SysMessage( GetDictionaryEntry( 781, socket.Language ) );
}

function ChopTree( socket, mChar )
{
	if( mChar.skillsused.lumberjacking )
	{
		socket.SysMessage( "You are too busy to do that" );
		return;
	}
	if( !CheckDistance( socket, mChar ) )
	{
		socket.SysMessage( GetDictionaryEntry( 393, socket.Language ) );
		return;
	}
	
	var targX = socket.GetWord( 11 );
	var targY = socket.GetWord( 13 );
	var mResource = ResourceRegion( targX, targY, mChar.worldnumber );
	RegenerateLog( mResource, socket );
	if( mResource.logAmount <= 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 840, socket.Language ) );
		return;
	}

	mChar.TurnToward( targX, targY );

	socket.clickX = targX;
	socket.clickY = targY;
	mChar.skillsused.lumberjacking = true;
	mChar.StartTimer( 200, 2, true );
}

function CheckDistance( socket, mChar )
{
	var targX = socket.GetWord( 11 );
	var targY = socket.GetWord( 13 );
	var distX = Math.abs( mChar.x - targX );
	var distY = Math.abs( mChar.y - targY );
	var distZ = Math.abs( mChar.z - socket.GetSByte( 16 ) );

	if( distX > 2 || distY > 2 || distZ > 9 )
		return false;
	else
		return true;
}

function onTimer( mChar, timerID )
{
	var socket = mChar.socket;
	switch( timerID )
	{
	case 0:
		mChar.skillsused.lumberjacking = false;
		if( socket )
		{
			if( !CheckDistance( socket, mChar ) )
			{
				socket.SysMessage( GetDictionaryEntry( 393, socket.Language ) );
				return;
			}
			var mResource = ResourceRegion( socket.clickX, socket.clickY, mChar.worldnumber );
			socket.clickX = 0;
			socket.clickY = 0;

			if( mChar.CheckSkill( 44, 0, 1000 ) )
			{
				mResource.logAmount = mResource.logAmount-1;
				CreateDFNItem( socket, mChar, "0x1BE0", 10, "ITEM", true );
				socket.SysMessage( GetDictionaryEntry( 1435, socket.Language ) );
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 842, socket.Language ) );
				if( RandomNumber( 0, 1 ) )	// 50% chance to destroy some resources
					mResource.logAmount = mResource.logAmount-1;
			}
		}
		break;
	case 1:
		if( socket )
		{
			if( !CheckDistance( socket, mChar ) )
			{
				socket.SysMessage( GetDictionaryEntry( 393, socket.Language ) );
				mChar.skillsused.lumberjacking = false;
				return;
			}
		}
		if( mChar.isonhorse )
			mChar.DoAction( 0x1C );
		else
			mChar.DoAction( 0x0D );
	
		mChar.SoundEffect( 0x013E, true );

		mChar.StartTimer( 1500, 0, true );
		break;
	case 2:
		if( mChar.isonhorse )
			mChar.DoAction( 0x1C );
		else
			mChar.DoAction( 0x0D );
	
		mChar.SoundEffect( 0x013E, true );
		
		mChar.StartTimer( 1300, 1, true );	
		break;
	}
}

function RegenerateLog( mResource, socket)
{
	var logCeiling	= ResourceAmount( "LOGS" );
	var logTimer	= ResourceTime( "LOGS" );
	var currentTime	= GetCurrentClock();

	if( mResource.logTime <= currentTime )
	{
		for( var i = 0; i < logCeiling; ++i )
		{
			if( (mResource.logTime + (i * logTimer * 1000)) <= currentTime && mResource.logAmount < logCeiling )
				mResource.logAmount = mResource.logAmount + 1;
			else
				break;
		}
		mResource.logTime = (currentTime + (1000 * logTimer));
	}
}

function CarveCorpse( socket, mChar, ourObj )
{
	if( mChar.InRange( ourObj, 3 ) )
	{
		mChar.DoAction( 0x20 );
		if( (ourObj.morey>>24) == 0 )
		{
			var part1 = 1;
			var part2 = ourObj.morey>>16;
			var part3 = ourObj.morey>>8;
			var part4 = ourObj.morey%256;

			ourObj.morey = (part1<<24) + (part2<<16) + (part3<<8) + part4;
			if( part2 != 0 || ourObj.carveSection != -1 )
				ourObj.Carve( socket );
		}
		else
			socket.SysMessage( GetDictionaryEntry( 1051, socket.Language ) );
	}
	else
		socket.SysMessage( GetDictionaryEntry( 393, socket.Language ) );
}