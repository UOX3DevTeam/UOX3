// This script handles all double-click functionality of axes (including lumberjacking)

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		var itemOwner = GetPackOwner( iUsed, 0 );
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			pUser.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
			return false;
		}
		else if( iUsed.type != 15 )
		{
			var targMsg = GetDictionaryEntry( 443, socket.language ); // What would you like to use that on?
			socket.CustomTarget( 1, targMsg );
		}
		else
		{
			return true;
		}
	}
	return false;
}

function onCallback1( socket, ourObj )
{
	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	var mChar = socket.currentChar;
	if( mChar && mChar.isChar )
	{
		var tileID = 0;
		if( socket.GetByte( 1 ))
		{
			tileID = socket.GetWord( 17 );
			if( !tileID )
			{
				tileID = GetTileIDAtMapCoord( socket.GetWord( 11 ), socket.GetWord( 13 ), mChar.worldNumber );
			}
		}
		else if( ourObj && ourObj.isItem )
		{
			tileID = ourObj.id;
		}

		if( tileID != 0 )
		{
			if( tileID == 0x0CD0 || tileID == 0x0CD3 || tileID == 0x0CD6 || tileID == 0x0CD8 || tileID == 0x0CDA ||
				tileID == 0x0CDD || tileID == 0x0CE0 || tileID == 0x0CE3 || tileID == 0x0CE6 || tileID == 0x0D58 ||
				( tileID >= 0x0CCA && tileID <= 0x0CCE ) || ( tileID >= 0x12B8 && tileID <= 0x12BB ) || tileID == 0x0D42 ||
				tileID == 0x0D43 || tileID == 0x0D58 || tileID == 0x0D59 || tileID == 0x0D70 || tileID == 0x0D85 ||
				tileID == 0x0D94 || tileID == 0x0D95 || tileID == 0x0D98 || tileID == 0x0DA4 || tileID == 0x0DA8 ) // Trees
			{
				ChopTree( socket, mChar );
			}
			else if( ourObj )
			{
				if( ourObj.type == 1 && ourObj.movable == 2 && !ourObj.GetTag( "addon" )) // Strongbox or Tent chest, and not an addon
				{
					var objMulti = ourObj.multi;
					if( ValidateObject( objMulti ))
					{
						if( ValidateObject( ourObj.owner ) && ( ourObj.owner == mChar || objMulti.owner == mChar )) // Only allow house owner or co-owner who owns strongbox to destroy it
						{
							if( ourObj.itemsinside > 0 )
							{
								if( ourObj.id == 0x0e43 ) // Tent chest
								{
									socket.SysMessage( GetDictionaryEntry( 9115, socket.language )); // You must empty the chest before you can remove this tent!
								}
								else // Strongbox
								{
									socket.SysMessage( GetDictionaryEntry( 1964, socket.language )); // You must empty the strongbox before you can destroy it!
								}
								return;
							}
							else
							{
								if( mChar.visible == 1 || mChar.visible == 2 )
								{
									mChar.visible = 0;
								}
								socket.SoundEffect( 0x3B3, true);
								socket.SysMessage( GetDictionaryEntry( 1965, socket.language )); // You destroy the item.

								if( ourObj.id == 0x0e43 ) // Tent chest
								{
									// First delete the chest
									ourObj.Delete();

									// Then add a new deed for the tent
									var newDeed = CreateDFNItem( socket, mChar, objMulti.deed, 1, "ITEM", true );
									if( newDeed )
									{
										socket.SysMessage( GetDictionaryEntry( 578, socket.language ), objMulti.name ); // Demolishing House %s.
										socket.SysMessage( GetDictionaryEntry( 579, socket.language ), newDeed.name ); // Converted into a %s.
									}

									// Finally, remove the tent itself
									objMulti.Delete();
								}
								else
								{
									// Strongbox
									ourObj.Delete();
								}
							}
						}
					}
				}
				else if( ourObj.type == 87 ) // Trash barrel/container
				{
					var objMulti = ourObj.multi;
					if( ValidateObject( objMulti ))
					{
						if( objMulti.IsOnOwnerList( socket.currentChar ))
						{
							if( objMulti.IsSecureContainer( ourObj ))
							{
								socket.SysMessage( GetDictionaryEntry( 1966, socket.language )); // You must unsecure the trash barrel before you can destroy it!
							}
							else
							{
								if( mChar.visible == 1 || mChar.visible == 2 )
								{
									mChar.visible = 0;
								}
								socket.SoundEffect( 0x3B3, true);
								socket.SysMessage( GetDictionaryEntry( 1965, socket.language )); // You destroy the item.
								objMulti.RemoveTrashCont( ourObj );
								ourObj.Delete();
							}
						}
						else
						{
							socket.SysMessage( GetDictionaryEntry( 1967, socket.language )); // You cannot chop that.
						}
					}
					else
					{
						socket.SysMessage( GetDictionaryEntry( 1967, socket.language )); // You cannot chop that.
					}
				}
				else if( ourObj.GetTag( "addon" ) || ourObj.type == 201 ) // looks for item type for backwards compatibility only!
				{
					var addonParent;
					if( ourObj.more == 0 )
					{
						addonParent = ourObj;
					}
					else if( ourObj.more > 0 )
					{
						// Calculate add-on parent based on serial stored in .more value of each add-on piece
						var addonParentSerial = ourObj.more;
						var serialPart1 = ( addonParentSerial >> 24 );
						var serialPart2 = ( addonParentSerial >> 16 );
						var serialPart3 = ( addonParentSerial >> 8 );
						var serialPart4 = ( addonParentSerial % 256 );
						addonParent = CalcItemFromSer( serialPart1, serialPart2, serialPart3, serialPart4 );
					}

					var objMulti = addonParent.multi;
					if( !ValidateObject( objMulti ))
					{
						socket.SysMessage( GetDictionaryEntry( 1969, socket.language )); // This house add-on does not appear to be in a house.
					}
					else if( !objMulti.IsOwner( mChar ))
					{
						socket.SysMessage( GetDictionaryEntry( 1967, socket.language )); // You cannot chop that.
					}
					else
					{
						objMulti.ReleaseItem( addonParent );
						var lockdownsLeft = objMulti.maxLockdowns - objMulti.lockdowns;
						socket.SysMessage( GetDictionaryEntry( 1902, socket.language ), lockdownsLeft ); // You release the item (%i lockdown(s) remaining).

						var addonDeed = addonParent.GetTag( "deed" );
						if( addonDeed )
						{
							var newDeed = CreateDFNItem( socket, socket.currentChar, addonDeed, 1, "ITEM", true );
							if( newDeed )
							{
								socket.SysMessage( GetDictionaryEntry( 1970, socket.language )); // A deed for the house add-on has been placed in your backpack.
							}
						}

						// Loop through items belonging to the house add-on and remove them
						var tempItem;
						for( tempItem = objMulti.FirstItem(); !objMulti.FinishedItems(); tempItem = objMulti.NextItem() )
						{
							if( !ValidateObject( tempItem ))
								continue;

							if( !ValidateObject( tempItem.multi ))
								continue;

							if( !tempItem.GetTag( "addon" ) && tempItem.type != 201 )
								continue;

							if( tempItem.more == addonParent.serial )
							{
								tempItem.Delete();
							}
						}

						if( mChar.visible == 1 || mChar.visible == 2 )
						{
							mChar.visible = 0;
						}

						// Finally delete the parent item for the house add-on
						addonParent.Delete();
					}
				}
				else if( tileID >= 0x1BD7 && tileID <= 0x1BE2 )	// Bowcraft
				{
					BowCraft( socket, mChar, ourObj, tileID );
				}
				else if( tileID == 0x2006 )
				{
					CarveCorpse( socket, mChar, ourObj );
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 1968, socket.language )); // You cannot carve that.
				}
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 1967, socket.language )); // You cannot chop that.
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 1967, socket.language )); // You cannot chop that.
		}
	}
}

function BowCraft( socket, mChar, ourObj, tileID )
{
	var ownerObj = GetPackOwner( ourObj, 0 );
	if( ownerObj && mChar.serial == ownerObj.serial )
	{
		socket.MakeMenu( 49, 8 );
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 781, socket.language )); // You can't use lumber outside your backpack.
	}
}

function ChopTree( socket, mChar )
{
	if( mChar.skillsused.lumberjacking )
	{
		socket.SysMessage( GetDictionaryEntry( 1971, socket.language )); // You are too busy to do that.
		return;
	}
	if( !CheckDistance( socket, mChar ))
	{
		socket.SysMessage( GetDictionaryEntry( 393, socket.language )); // That is too far away.
		return;
	}

	var targX = socket.GetWord( 11 );
	var targY = socket.GetWord( 13 );
	var mResource = ResourceRegion( targX, targY, mChar.worldnumber );
	RegenerateLog( mResource, socket );
	if( mResource.logAmount <= 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 840, socket.language )); // There is no more wood here to chop.
		return;
	}

	if( mChar.visible == 1 || mChar.visible == 2 )
	{
		mChar.visible = 0;
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
	var distZ = Math.abs( mChar.z - socket.GetSByte( 16 ));

	if( distX > 2 || distY > 2 || distZ > 9 )
	{
		return false;
	}
	else
	{
		return true;
	}
}

function onTimer( mChar, timerID )
{
	var socket = mChar.socket;
	switch( timerID )
	{
		case 0: // Lumberjacking, end-result
			mChar.skillsused.lumberjacking = false;
			if( socket )
			{
				if( !CheckDistance( socket, mChar ))
				{
					socket.SysMessage( GetDictionaryEntry( 393, socket.language )); // That is too far away.
					return;
				}
				var mResource = ResourceRegion( socket.clickX, socket.clickY, mChar.worldnumber );
				socket.clickX = 0;
				socket.clickY = 0;

				if( mChar.CheckSkill( 44, 0, 1000 ))
				{
					mResource.logAmount = mResource.logAmount-1;
					CreateDFNItem( socket, mChar, "0x1BE0", 10, "ITEM", true );
					socket.SysMessage( GetDictionaryEntry( 1435, socket.language )); // You place some logs in your pack.
				}
				else
				{
					socket.SysMessage( GetDictionaryEntry( 842, socket.language )); // =You chop for a while, but fail to produce any usable wood.
					if( RandomNumber( 0, 1 ))	// 50% chance to destroy some resources
					{
						mResource.logAmount = mResource.logAmount-1;
					}
				}
			}
			break;
		case 1: // Lumberjacking, half-ways there
			if( socket )
			{
				if( !CheckDistance( socket, mChar ))
				{
					socket.SysMessage( GetDictionaryEntry( 393, socket.language )); // That is too far away.
					mChar.skillsused.lumberjacking = false;
					return;
				}
			}
			if( mChar.visible == 1 || mChar.visible == 2 )
			{
				mChar.visible = 0;
			}
			if( mChar.isonhorse )
			{
				mChar.DoAction( 0x1C );
			}
			else
			{
				mChar.DoAction( 0x0D );
			}

			mChar.SoundEffect( 0x013E, true );

			mChar.StartTimer( 1500, 0, true );
			break;
		case 2: // Lumberjacking, start of chopping action
			if( mChar.visible == 1 || mChar.visible == 2 )
			{
				mChar.visible = 0;
			}
			if( mChar.isonhorse )
			{
				mChar.DoAction( 0x1C );
			}
			else
			{
				mChar.DoAction( 0x0D );
			}

			mChar.SoundEffect( 0x013E, true );

			mChar.StartTimer( 1300, 1, true );
			break;
		default:
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
			if((( mResource.logTime + ( i * logTimer * 1000 )) / 1000 ) <= ( currentTime / 1000 ) && mResource.logAmount < logCeiling )
			{
				mResource.logAmount = mResource.logAmount + 1;
			}
			else
			{
				break;
			}
		}
		mResource.logTime = ( currentTime + ( 1000 * parseInt( logTimer ))) / 1000;
	}
}

function CarveCorpse( socket, mChar, ourObj )
{
	if( mChar.InRange( ourObj, 3 ))
	{
		mChar.DoAction( 0x20 );
		if(( ourObj.morey >> 24 ) == 0 )
		{
			var part1 = 1;
			var part2 = ourObj.morey >> 16;
			var part3 = ourObj.morey >> 8;
			var part4 = ourObj.morey % 256;

			ourObj.morey = ( part1 << 24 ) + (part2 << 16 ) + ( part3 << 8 ) + part4;
			if( part2 != 0 || ourObj.carveSection != -1 )
			{
				ourObj.Carve( socket );
			}
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 1051, socket.language )); // You carve the corpse but find nothing useful.
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 393, socket.language )); // That is too far away.
	}
}