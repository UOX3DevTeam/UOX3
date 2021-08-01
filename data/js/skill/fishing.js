function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if ( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}

	if ( socket && iUsed && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if ( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pUser.SysMessage( GetDictionaryEntry( 1763, socket.language ) ); // That item must be in your backpack before it can be used.
			return false;
		}
		//else if( horse check )//You can't fish while riding!
		else if ( iUsed.type != 15 )
		{
			var targMsg = GetDictionaryEntry( 9310, socket.language ); // What water do you want to fish in?
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
	var targX = socket.GetWord( 11 );
	var targY = socket.GetWord( 13 );
	var targz = socket.GetSByte( 16 );
	var ShallowWaterFlag = CheckStaticFlag( targX, targY, targz, mChar.worldNumber, 7 )
	if ( mChar.visible == 1 || mChar.visible == 2 )
	{
		mChar.visible = 0;
	}

	if ( mChar && mChar.isChar )
	{
		var tileID = 0;
		if ( socket.GetByte( 1 ) )
		{
			tileID = socket.GetWord( 17 );
			if ( !tileID )
			{
				tileID = GetTileIDAtMapCoord( socket.GetWord( 11 ), socket.GetWord( 13 ), mChar.worldNumber );
			}
		}
		else if ( ourObj && ourObj.isItem )
		{
			tileID = ourObj.id;
		}

		if ( tileID != 0 )
		{
			if ( ShallowWater( tileID ) || ShallowWaterFlag )
			{
				FishingShallow( socket, mChar );
			}
			else
				socket.SysMessage( GetDictionaryEntry( 9311, socket.language ) ); // The fish don't seem to be biting here.
		}
		else
			socket.SysMessage( GetDictionaryEntry( 9311, socket.language ) ); // The fish don't seem to be biting here.
	}
}

function ShallowWater( tileID )
{
	return ( tileID == 0x00A8 || tileID == 0x00AB || tileID == 0x0136 || tileID == 0x0137 || tileID == 0x5797 || tileID == 0x579C || tileID == 0x746E
		   || tileID == 0x7485 || tileID == 0x7490 || tileID == 0x74AB || tileID == 0x74B5 || tileID == 0x75D5 );
}

function FishingShallow( socket, mChar )
{
	if ( mChar.skillsused.fishing )
	{
		socket.SysMessage( GetDictionaryEntry( 9312, socket.language ) ); // You are already fishing.
		return;
	}
	var fishingStaminaLoss = GetServerSetting( "FishingStaminaLoss" );
	if ( mChar.stamina - 2 < 2 )
	{
		mChar.SysMessage( GetDictionaryEntry( 845, socket.language ) );
		return;
	}
	mChar.stamina -= fishingStaminaLoss

	if ( !CheckDistance( socket, mChar ) )
	{
		socket.SysMessage( GetDictionaryEntry( 9313, socket.language ) ); //You need to be closer to the water to fish!
		return;
	}

	var targX = socket.GetWord( 11 );
	var targY = socket.GetWord( 13 );
	var targz = socket.GetSByte( 16 );
	var mResource = ResourceRegion( targX, targY, mChar.worldnumber );
	RegenerateFish( mResource, socket );
	if ( mResource.fishAmount <= 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 9311, socket.language ) ); // The fish don't seem to be biting here.
		return;
	}
	DoStaticEffect( targX, targY, targz, 0x352D, 0x3, 0x16, false )
	mChar.TurnToward( targX, targY );
	socket.clickX = targX;
	socket.clickY = targY;
	mChar.skillsused.fishing = true;
	mChar.DoAction( 0x0b );
	mChar.SoundEffect( 0x364, true );
	var baseFishingTimer = GetServerSetting( "BaseFishingTimer" );
	var randomFishingTimer = GetServerSetting( "RandomFishingTimer" );
	var fishingTimer = baseFishingTimer + RandomNumber( 0, randomFishingTimer );
	mChar.StartTimer( fishingTimer, 2, true );
}

function CheckDistance( socket, mChar )
{
	var targX = socket.GetWord( 11 );
	var targY = socket.GetWord( 13 );
	var distX = Math.abs( mChar.x - targX );
	var distY = Math.abs( mChar.y - targY );
	var distZ = Math.abs( mChar.z - socket.GetSByte( 16 ) );

	if ( distX > 5 || distY > 5 || distZ > 9 )
		return false;
	else
		return true;
}

function onTimer( mChar, timerID )
{
	var socket = mChar.socket;
	if ( mChar.visible == 1 || mChar.visible == 2 )
	{
		mChar.visible = 0;
	}
	switch ( timerID )
	{
		case 0:
			mChar.skillsused.fishing = false;
			if ( socket )
			{
				if ( !CheckDistance( socket, mChar ) )
				{
					socket.SysMessage( GetDictionaryEntry( 9313, socket.language ) ); // You need to be closer to the water to fish!
					return;
				}
				var mResource = ResourceRegion( socket.clickX, socket.clickY, mChar.worldnumber );
				socket.clickX = 0;
				socket.clickY = 0;

				if ( mChar.CheckSkill( 18, 0, 1000 ) )
				{
					switch ( RandomNumber( 0, 25 ) )
					{
						case 0://PrizedFish, WondrousFish, TrulyRareFish, PeculiarFish
							mResource.fishAmount = mResource.fishAmount - 1;
							CreateDFNItem( mChar.socket, mChar, "randommagicfish", 1, "ITEM", true );
							mChar.SysMessage( GetDictionaryEntry( 9315, socket.language ) );// You pull out an item : magic fish
							break;
						case 1://Fish Random
							mResource.fishAmount = mResource.fishAmount - 1;
							CreateDFNItem( mChar.socket, mChar, "randomfish", 1, "ITEM", true );
							mChar.SysMessage( GetDictionaryEntry( 9316, socket.language ) );// You pull out an item : fish
							break;
						case 2://Shoes/boots
							mResource.fishAmount = mResource.fishAmount - 1;
							CreateDFNItem( mChar.socket, mChar, "randomfootwear", 1, "ITEM", true );
							mChar.SysMessage( GetDictionaryEntry( 9317, socket.language ) );// You pull out an item : footwear
							break;
						default:
							mResource.fishAmount = mResource.fishAmount - 1;
							CreateDFNItem( mChar.socket, mChar, "randomfish", 1, "ITEM", true );
							mChar.SysMessage( GetDictionaryEntry( 9316, socket.language ) );// You pull out an item : fish
							break;
						//Treasure Map
						//Message In A Bottle
						//Special Fishing Net
					}
					break;
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 9314, socket.language ) ); // You fish a while, but fail to catch anything.
					if ( RandomNumber( 0, 1 ) )	// 50% chance to destroy some resources
						mResource.fishAmount = mResource.fishAmount - 1;
				}
			}
			break;
		case 1:
			if ( socket )
			{
				if ( !CheckDistance( socket, mChar ) )
				{
					socket.SysMessage( GetDictionaryEntry( 9313, socket.language ) ); // You need to be closer to the water to fish!
					mChar.skillsused.fishing = false;
					return;
				}
			}
			mChar.StartTimer( 1500, 0, true );
			break;
		case 2:
			mChar.StartTimer( 400, 1, true );
			break;
	}
}

function RegenerateFish( mResource, socket )
{
	var fishCeiling = ResourceAmount( "FISH" );
	var fishTimer = ResourceTime( "FISH" );
	var currentTime = GetCurrentClock();

	if ( mResource.fishTime <= currentTime )
	{
		for ( var i = 0; i < fishCeiling; ++i )
		{
			if ( ( mResource.fishTime + ( i * fishTimer * 1000 ) ) <= currentTime && mResource.fishAmount < fishCeiling ) {
				mResource.fishAmount = mResource.fishAmount + 1;
			}
			else
				break;
		}
		mResource.fishTime = ( currentTime + ( 1000 * fishTimer ) );
	}
}