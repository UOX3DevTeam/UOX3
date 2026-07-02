/// <reference path="../definitions.d.ts" />
// @ts-check
// Guildstone Deed/Stone

const pendingGuildstoneDeedTag = "pendingGuildstoneDeed";
const defaultGuildCharter = "UOX3 Guildstone";
const defaultGuildWebpage = "http://www.uox3.org/";

/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var pSocket = pUser.socket;
	if ( pUser.visible == 1 || pUser.visible == 2 )
	{
		pUser.visible = 0;
	}
	if( pSocket != null && ValidateObject( iUsed ) && iUsed.isItem )
	{
		if( iUsed.id == 0x14F0 || iUsed.id == 0x1869 )
		{
			var itemOwner = GetPackOwner( iUsed, 0 );
			if( itemOwner == null || itemOwner.serial != pUser.serial )
			{
				pSocket.SysMessage( GetDictionaryEntry( 1763, pSocket.language )); // That item must be in your backpack before it can be used.
				return false;
			}

			if( iUsed.id == 0x14F0 )
			{
				var deedGuildMenuSystem = parseInt( GetServerSetting( "GuildMenuSystem" ), 10 );
				if( isNaN( deedGuildMenuSystem ))
					deedGuildMenuSystem = GetServerSetting( "ClassicOSIGuildMenu" ) ? 1 : 0;

				if( deedGuildMenuSystem == 2 )
				{
					pSocket.SysMessage( "Use the guild button on your paperdoll to create or access a guild." );
					return false;
				}

				if( !ValidateGuildstonePlacement( pUser, iUsed, true ))
					return false;

				if( pUser.SetTag )
					pUser.SetTag( pendingGuildstoneDeedTag, iUsed.serial );

				TriggerEvent( 8020, "GuildCreation", pUser );
				return false;
			}

			if( iUsed.id == 0x1869 )
			{
				if( !pUser.guild )
				{
					pSocket.SysMessage( "You don't appear to be in a guild" );
					return false;
				}

				var deedGuild = FindGuildById( iUsed.more );
				if( !deedGuild )
				{
					pSocket.SysMessage( GetDictionaryEntry( 174, pSocket.language )); // Critical error adding guildstone, please contact a GM!
					return false;
				}

				PlaceGuildStoneFromDeed( pUser, iUsed, deedGuild );
				return false;
			}
		}

		if( iUsed.id == 0x0ED5 )
		{
			var stoneGuild = FindGuildById( iUsed.more );
			if( !stoneGuild || !pUser.guild || pUser.guild.id != stoneGuild.id )
			{
				pSocket.SysMessage( GetDictionaryEntry( 1984, pSocket.language )); // You are not a member of this guild!
				return false;
			}

			var guildMenuSystem = parseInt( GetServerSetting( "GuildMenuSystem" ), 10 );
			if( isNaN( guildMenuSystem ))
				guildMenuSystem = GetServerSetting( "ClassicOSIGuildMenu" ) ? 1 : 0;

			if( guildMenuSystem == 1 )
				TriggerEvent( 8020, "ClassicGuildMenu", pUser );
			else if( guildMenuSystem == 2 )
				pSocket.SysMessage( "Use the guild button on your paperdoll to access the guild menu." );
			else
				TriggerEvent( 8020, "GuildMenu", pUser );
			return false;
		}
	}

	return false;
}

function PlacePendingGuildstone( pUser, guild )
{
	if( !ValidateObject( pUser ) || !guild || !pUser.GetTag )
		return false;

	var deedSerial = parseInt( pUser.GetTag( pendingGuildstoneDeedTag ), 10 );
	pUser.SetTag( pendingGuildstoneDeedTag, null );
	if( isNaN( deedSerial ) || deedSerial <= 0 )
		return false;

	var deed = CalcItemFromSer( deedSerial );
	if( !ValidateObject( deed ) || !deed.isItem )
		return false;

	return PlaceGuildStoneFromDeed( pUser, deed, guild );
}

function CanPlacePendingGuildstone( pUser )
{
	if( !ValidateObject( pUser ) || !pUser.GetTag )
		return true;

	var deedSerial = parseInt( pUser.GetTag( pendingGuildstoneDeedTag ), 10 );
	if( isNaN( deedSerial ) || deedSerial <= 0 )
		return true;

	var deed = CalcItemFromSer( deedSerial );
	if( !ValidateObject( deed ) || !deed.isItem )
	{
		if( pUser.SetTag )
			pUser.SetTag( pendingGuildstoneDeedTag, null );
		return false;
	}

	return ValidateGuildstonePlacement( pUser, deed, true );
}

function ValidateGuildstonePlacement( pUser, iUsed, requireNoGuild )
{
	var pSocket = pUser.socket;
	if( pSocket == null || !ValidateObject( iUsed ) || !iUsed.isItem )
		return false;

	var itemOwner = GetPackOwner( iUsed, 0 );
	if( itemOwner == null || itemOwner.serial != pUser.serial )
	{
		pSocket.SysMessage( GetDictionaryEntry( 1763, pSocket.language )); // That item must be in your backpack before it can be used.
		return false;
	}

	var iMulti = pUser.multi;
	if( !ValidateObject( iMulti ) || !iMulti.IsInMulti( pUser ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 2723, pSocket.language )); // Guildstones can only be placed in houses!
		return false;
	}

	if( !iMulti.IsOwner( pUser ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 2724, pSocket.language )); // Only the house owner can place a guildstone in a house!
		return false;
	}

	if( requireNoGuild && pUser.guild != null )
	{
		pSocket.SysMessage( GetDictionaryEntry( 173, pSocket.language )); // You are already in a guild!
		return false;
	}

	var tempItem;
	for( tempItem = iMulti.FirstItem(); !iMulti.FinishedItems(); tempItem = iMulti.NextItem() )
	{
		if( !ValidateObject( tempItem ))
			continue;

		if( tempItem.type == 202 && tempItem.id == 0x0ED5 )
		{
			pSocket.SysMessage( GetDictionaryEntry( 2725, pSocket.language )); // You cannot place any additional guildstones in this house!
			return false;
		}
	}

	var foundDoor = AreaItemFunction( "checkForNearbyDoors", pUser, 3, pSocket );
	if( foundDoor )
	{
		pSocket.SysMessage( GetDictionaryEntry( 2726, pSocket.language )); // You cannot place a guildstone adjacent to a door!
		return false;
	}

	return true;
}

function PlaceGuildStoneFromDeed( pUser, deed, guild )
{
	if( !ValidateGuildstonePlacement( pUser, deed, false ))
		return false;

	var pSocket = pUser.socket;
	if( !guild )
	{
		pSocket.SysMessage( GetDictionaryEntry( 174, pSocket.language )); // Critical error adding guildstone, please contact a GM!
		return false;
	}

	var stoneName = guild.name ? ( "A guildstone for " + guild.name ) : "Guildstone for an unnamed guild";
	var stone = CreateBlankItem( pSocket, pUser, 1, stoneName, 0x0ED5, 0, "ITEM", false );
	if( !ValidateObject( stone ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 176, pSocket.language )); // Critical error, unable to spawn guildstone, please contact a GM!
		return false;
	}

	stone.SetLocation( pUser );
	stone.type = 202;
	stone.more = guild.id;
	stone.wipable = false;
	stone.decayable = false;
	guild.charter = defaultGuildCharter;
	guild.webPage = defaultGuildWebpage;
	guild.stone = stone;
	deed.Delete();
	return true;
}

function PackGuildstone( pUser, guild )
{
	if( !ValidateObject( pUser ) || !guild || !pUser.socket )
		return false;

	var pSocket = pUser.socket;
	if( !pUser.guild || pUser.guild.id != guild.id )
	{
		pSocket.SysMessage( GetDictionaryEntry( 1984, pSocket.language )); // You are not a member of this guild!
		return false;
	}

	if( !guild.master || guild.master.serial != pUser.serial )
	{
		pSocket.SysMessage( "Only the guild master can move the guildstone." );
		return false;
	}

	var stone = guild.stone;
	if( !ValidateObject( stone ) || !stone.isItem )
	{
		pSocket.SysMessage( "This guild does not have a placed guildstone." );
		return false;
	}

	var deed = CreateBlankItem( pSocket, pUser, 1, "guildstone transporter", 0x1869, 0, "ITEM", true );
	if( !ValidateObject( deed ) || !deed.isItem )
	{
		pSocket.SysMessage( "Unable to create a guildstone transporter deed." );
		return false;
	}

	deed.type = 202;
	deed.more = guild.id;
	deed.decayable = false;
	guild.stone = null;
	stone.Delete();
	pSocket.SysMessage( "The guildstone has been packed into a transporter deed." );
	return true;
}

function FindGuildById( guildId )
{
	var allGuilds = GetAllGuilds();
	if( !allGuilds )
		return null;

	for( var i = 0; i < allGuilds.length; i++ )
	{
		if( allGuilds[i] && allGuilds[i].id == guildId )
			return allGuilds[i];
	}

	return null;
}

function checkForNearbyDoors( pUser, itemToCheck, pSocket )
{
	if( ValidateObject( itemToCheck ))
	{
		if(( itemToCheck.type == 12 || itemToCheck.type == 13 ))
		{
			if( itemToCheck.z > pUser.z && itemToCheck.z - pUser.z >= 20 )
			{
				// Ignore doors on floors above
				return false;
			}
			else if( itemToCheck.z < pUser.z && pUser.z - itemToCheck.z >= 20 )
			{
				// Ignore doors on floors below, too!
				return false;
			}

			if( itemToCheck.isDoorOpen )
			{
				// Make sure to check against the distance from the door in it's closed state, rather than it's open state!
				var origX = itemToCheck.x  - itemToCheck.GetTag( "DOOR_X" );
				var origY = itemToCheck.y  - itemToCheck.GetTag( "DOOR_Y" );
				if( pUser.x - origX < 2 || origX - pUser.x < 2 || pUser.y - origY < 2 || origY - pUser.y < 2 )
					return true;
			}

			if( pUser.DistanceTo( itemToCheck ) <= 2 )
				return true;
		}
	}
	return false;
}
