#include "uox3.h"
#include "skills.h"
#include "cGuild.h"
#include "townregion.h"
#include "cServerDefinitions.h"
#include "commands.h"
#include "cMagic.h"
#include "ssection.h"
#include "gump.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "combat.h"
#include "magic.h"
#include "Dictionary.h"
#include "CResponse.h"

#include "ObjectFactory.h"
#include "PartySystem.h"
#include "StringUtility.hpp"

using namespace std::string_literals;

void OpenPlank( CItem *p );
bool CheckItemRange( CChar *mChar, CItem *i );

//o------------------------------------------------------------------------------------------------o
//|	Function	-	PlayerVendorBuy()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when player tries buying an item from a player vendor
//o------------------------------------------------------------------------------------------------o
void PlayerVendorBuy( CSocket *s )
{
	VALIDATESOCKET( s );

	CChar *vChar = static_cast<CChar *>( s->TempObj() );
	s->TempObj( nullptr );
	if( !ValidateObject( vChar ) || vChar->IsFree() )
		return;

	CChar *mChar	= s->CurrcharObj();
	UI32 goldLeft	= GetItemAmount( mChar, 0x0EED );

	CItem *p		= mChar->GetPackItem();
	if( !ValidateObject( p ))
	{
		s->SysMessage( 773 ); // Time to buy a backpack.
		return;
	}

	CItem *i = CalcItemObjFromSer( s->GetDWord( 7 ));
	if( !ValidateObject( i ) || i->GetCont() == nullptr )
		return;

	if( FindItemOwner( i ) != vChar )
		return;

	if( vChar->GetNpcAiType() != AI_PLAYERVENDOR )
		return;

	if( mChar == vChar->GetOwnerObj() )
	{
		vChar->TextMessage( s, 999, TALK, false ); // I work for you, you need not buy things from me!
		return;
	}

	if( i->GetVendorPrice() <= 0 )
	{
		vChar->TextMessage( s, 9182, TALK, false ); // This item is not for sale.
		return;
	}

	if( goldLeft < i->GetVendorPrice() )
	{
		vChar->TextMessage( s, 1000, TALK, false ); // You cannot afford that.
		return;
	}
	else
	{
		[[maybe_unused]] auto amountDeleted = DeleteItemAmount( mChar, i->GetVendorPrice(), 0x0EED );
		// tAmount > 0 indicates there wasn't enough money...
		// could be expanded to take money from bank too...
	}

	vChar->TextMessage( s, 1001, TALK, false ); // Thank you.
	vChar->SetHoldG( vChar->GetHoldG() + i->GetVendorPrice() );

	// Move the bought item to player's backpack
	i->SetCont( p );
}

void TextEntryGump( CSocket *s, SERIAL ser, UI08 type, UI08 index, SI16 maxlength, SI32 dictEntry );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleGuildTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles targeting related to guild actions
//o------------------------------------------------------------------------------------------------o
void HandleGuildTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *trgChar	= nullptr;
	CChar *mChar	= s->CurrcharObj();
	CGuild *mGuild	= nullptr, *tGuild = nullptr;
	switch( s->GetByte( 5 ))
	{
		case 0:	// recruit character
			trgChar = CalcCharObjFromSer( s->GetDWord( 7 ));
			if( ValidateObject( trgChar ))
			{
				if( trgChar->GetGuildNumber() == -1 )	// no existing guild
				{
					mGuild = GuildSys->Guild( mChar->GetGuildNumber() );
					if( mGuild != nullptr )
					{
						auto trgSock = trgChar->GetSocket();
						if( mGuild->Master() == mChar->GetSerial() )
						{
							// Guild Master can skip the "sponsor a candidate" stage and just recruit people as new members directly
							mGuild->NewMember(( *trgChar ));
							trgChar->SetGuildNumber( mChar->GetGuildNumber() );
							if( mGuild->Type() != GT_STANDARD )
							{
								trgChar->SetGuildToggle( true );
							}
							s->SysMessage( 1687, trgChar->GetName().c_str() ); // You have recruited %s as a new guild member!
							if( trgSock )
							{
								trgSock->SysMessage( 1688, mGuild->Name().c_str() ); // You have been recruited to join the guild %s!
							}	
						}
						else
						{
							mGuild->NewRecruit(( *trgChar ));
							s->SysMessage( 1985, trgChar->GetName().c_str() ); // You have sponsored %s as a candidate to join the guild!
							if( trgSock )
							{
								trgSock->SysMessage( 1986, mChar->GetName().c_str(), mGuild->Name().c_str() ); // You have been invited by %s to join the guild: %s
							}							
						}
						trgChar->Dirty( UT_UPDATE );
					}
				}
				else
				{
					s->SysMessage( 1002 ); // They are already in a guild!
				}
			}
			break;
		case 1:		// declare fealty
			trgChar = CalcCharObjFromSer( s->GetDWord( 7 ));
			if( ValidateObject( trgChar ))
			{
				if( trgChar->GetGuildNumber() == mChar->GetGuildNumber() ) // same guild
				{
					mChar->SetGuildFealty( trgChar->GetSerial() );
					s->SysMessage( 1987, trgChar->GetName().c_str() ); // You have declared fealty to %s!
					auto trgSock = trgChar->GetSocket();
					if( trgSock != nullptr )
					{
						trgSock->SysMessage( 1988, mChar->GetName().c_str() ); // %s has declared fealty to you!
					}
				}
				else
				{
					s->SysMessage( 1003 ); // They are not in your guild!
				}
			}
			break;
		case 2:	// declare war
			trgChar = CalcCharObjFromSer( s->GetDWord( 7 ));
			if( ValidateObject( trgChar ))
			{
				if( trgChar->GetGuildNumber() != mChar->GetGuildNumber() )
				{
					if( trgChar->GetGuildNumber() == -1 )
					{
						s->SysMessage( 1004 ); // They're not in a guild!
					}
					else
					{
						mGuild = GuildSys->Guild( mChar->GetGuildNumber() );
						if( mGuild != nullptr )
						{
							mGuild->SetGuildRelation( trgChar->GetGuildNumber(), GR_WAR );
							tGuild = GuildSys->Guild( trgChar->GetGuildNumber() );
							if( tGuild != nullptr )
							{
								mGuild->TellMembers( 1989, tGuild->Name().c_str() ); // Your guild has declared war on the guild %s!
								tGuild->TellMembers( 1005, mGuild->Name().c_str() ); // The guild %s has declared war upon you!
							}
						}
					}
				}
				else
				{
					s->SysMessage( 1006 ); // They are in your guild!
				}
			}
			break;
		case 3:	// declare ally
			trgChar = CalcCharObjFromSer( s->GetDWord( 7 ));
			if( ValidateObject( trgChar ))
			{
				if( trgChar->GetGuildNumber() != mChar->GetGuildNumber() )
				{
					if( trgChar->GetGuildNumber() == -1 )
					{
						s->SysMessage( 1004 ); // They're not in a guild!
					}
					else
					{
						mGuild = GuildSys->Guild( mChar->GetGuildNumber() );
						if( mGuild != nullptr )
						{
							mGuild->SetGuildRelation( trgChar->GetGuildNumber(), GR_ALLY );
							tGuild = GuildSys->Guild( trgChar->GetGuildNumber() );
							if( tGuild != nullptr )
							{
								mGuild->TellMembers( 1990, tGuild->Name().c_str() ); // Your guild has declared the guild %s as an ally!
								tGuild->TellMembers( 1007, mGuild->Name().c_str() ); // The guild %s has declared you to be an ally!
							}
						}
					}
				}
				else
				{
					s->SysMessage( 1006 ); // They are in your guild!
				}
			}
			break;
		case 4: // select member to grant title to
			trgChar = CalcCharObjFromSer( s->GetDWord( 7 ));
			if( ValidateObject( trgChar ))
			{
				if( trgChar->GetGuildNumber() == mChar->GetGuildNumber()  ) // In same guild
				{
					mGuild = GuildSys->Guild( mChar->GetGuildNumber() );
					if( mGuild != nullptr )
					{
						s->TempInt2( trgChar->GetSerial() );
						TextEntryGump( s, mChar->GetSerial(), 100, 6, 15, 1684 );	break;	// grant title to another member
					}
				}
				else
				{
					s->SysMessage( 1003 ); // They are not in your guild!
				}
			}
			break;
		default:
			break;
	}
}

CMultiObj * BuildHouse( CSocket *s, UI16 houseEntry, bool checkLocation = true, SI16 xLoc = -1, SI16 yLoc = -1, SI08 zLoc = 127, UI08 worldNumber = 0, UI16 instanceId = 0 );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	BuildHouseTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Verifies player has a valid target when trying to place a house
//o------------------------------------------------------------------------------------------------o
void BuildHouseTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	// Check if item used to initialize target cursor is still within range
	CChar *mChar = s->CurrcharObj();
	CItem *deedObj = mChar->GetSpeechItem();
	if( ValidateObject( deedObj ))
	{
		CChar *deedObjOwner = FindItemOwner( deedObj );
		if( !ValidateObject( deedObjOwner ) || deedObjOwner != mChar )
		{
			s->SysMessage( 1763 ); // That item must be in your backpack before it can be used.
			return;
		}
	}

	[[maybe_unused]] CMultiObj *newMulti = BuildHouse( s, static_cast<UI16>( s->AddId() )); // If its a valid house, send it to buildhouse!

	s->AddId( 0 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	AddScriptNpc()
//|	Date		-	17th February, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Add NPC at targeted location
//| Notes		-	Need to return the character we've made, else summon creature at least will fail
//|					We make the char, but never pass it back up the chain
//o------------------------------------------------------------------------------------------------o
void AddScriptNpc( CSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	CChar *mChar			= s->CurrcharObj();
	const SI16 coreX		= s->GetWord( 11 );
	const SI16 coreY		= s->GetWord( 13 );
	const SI08 coreZ		= static_cast<SI08>( s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 )));
	Npcs->CreateNPCxyz( s->XText(), coreX, coreY, coreZ, mChar->WorldNumber(), mChar->GetInstanceId() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	TeleTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles targeting of Teleport spell
//o------------------------------------------------------------------------------------------------o
void TeleTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	const SERIAL serial = s->GetDWord( 7 );

	CBaseObject *mObj = nullptr;
	if( serial >= BASEITEMSERIAL )
	{
		mObj = CalcItemObjFromSer( serial );
	}
	else
	{
		mObj = CalcCharObjFromSer( serial );
	}

	SI16 targX, targY;
	SI08 targZ;
	if( ValidateObject( mObj ))
	{
		targX = mObj->GetX();
		targY = mObj->GetY();
		targZ = mObj->GetZ();
	}
	else
	{
		targX = s->GetWord( 11 );
		targY = s->GetWord( 13 );
		targZ = static_cast<SI08>( s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 )));
	}
	CChar *mChar = s->CurrcharObj();

	if( mChar->IsGM() || LineOfSight( s, mChar, targX, targY, targZ, WALLS_CHIMNEYS + DOORS + ROOFING_SLANTED, false ))
	{
		if( s->CurrentSpellType() != 2 ) // not a wand cast
		{
			Magic->SubtractMana( mChar, 3 );	// subtract mana on scroll or spell
			if( s->CurrentSpellType() == 0 )	// del regs on normal spell
			{
				Reag_st toDel;
				toDel.drake = 1;
				toDel.moss = 1;
				Magic->DelReagents( mChar, toDel );
			}
		}

		Effects->PlaySound( s, 0x01FE, true );

		mChar->SetLocation( targX, targY, targZ );
		Effects->PlayStaticAnimation( mChar, 0x372A, 0x09, 0x06 );
	}
	else if( s != nullptr )
	{
		s->SysMessage( 687 ); // You would like to see if anything was there, but there is too much stuff in the way.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DyeTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dye target object with specified hue, or show dye gump to player
//o------------------------------------------------------------------------------------------------o
void DyeTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i		= nullptr;
	CChar *c		= nullptr;
	SERIAL serial	= s->GetDWord( 7 );
	if( s->AddId1() == 0xFF && s->AddId2() == 0xFF )
	{
		// Dye with no specific color - show hue gump
		CPDyeVat toSend;
		if( serial >= BASEITEMSERIAL )
		{
			// Dye an item
			i = CalcItemObjFromSer( serial );
			if( ValidateObject( i ))
			{
				toSend = ( *i );
				s->Send( &toSend );
			}
		}
		else
		{
			// Dye a character
			c = CalcCharObjFromSer( serial );
			if( ValidateObject( c ))
			{
				toSend = ( *c );
				toSend.Model( 0x2106 );
				s->Send( &toSend );
			}
		}
	}
	else
	{
		// Dye with a specific color in mind
		if( serial >= BASEITEMSERIAL )
		{
			// Dye an item
			i = CalcItemObjFromSer( serial );
			if( !ValidateObject( i ))
				return;

			UI16 colour = static_cast<UI16>((( s->AddId1() ) << 8 ) + s->AddId2() );
			if( !s->DyeAll() )
			{
				if( colour < 0x0002 || colour > 0x03E9 )
				{
					colour = 0x03E9;
				}
			}

			SI32 b = (( colour & 0x4000 ) >> 14 ) + (( colour & 0x8000 ) >> 15 );
			if( !b )
			{
				i->SetColour( colour );
			}
		}
		else
		{
			// Dye a character
			c = CalcCharObjFromSer( serial );
			if( !ValidateObject( c ))
				return;

			UI16 body = c->GetId();
			UI16 k = static_cast<UI16>((( s->AddId1() ) << 8 ) + s->AddId2() );

			if(( k & 0x4000 ) == 0x4000 && ( body >= 0x0190 && body <= 0x03E1 ))
			{
				k = 0xF000; // but assigning the only "transparent" value that works, namly semi-trasnparency.
			}

			if( k != 0x8000 ) // 0x8000 also crashes client ...
			{
				c->SetSkin( k );
				c->SetOrgSkin( k );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	WStatsTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Show NPC wander information for targeted NPC
//o------------------------------------------------------------------------------------------------o
void WStatsTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = CalcCharObjFromSer( s->GetDWord( 7 ));
	if( !ValidateObject( i ))
		return;

	CGumpDisplay wStat( s, 300, 300 );
	wStat.SetTitle( "Walking Stats" );
	SERIAL charSerial = i->GetSerial();
	UI16 charId = i->GetId();
	wStat.AddData( "Serial", charSerial, 3 );
	wStat.AddData( "Body ID", charId, 5 );

	std::string iName = GetNpcDictName( i, s );
	wStat.AddData( "Name", iName );
	wStat.AddData( "X", i->GetX() );
	wStat.AddData( "Y", i->GetY() );
	wStat.AddData( "Z", oldstrutil::format( "%d", i->GetZ() ));
	wStat.AddData( "Wander", i->GetNpcWander() );
	wStat.AddData( "FX1", i->GetFx( 0 ));
	wStat.AddData( "FY1", i->GetFy( 0 ));
	wStat.AddData( "FZ1", i->GetFz() );
	wStat.AddData( "FX2", i->GetFx( 1 ));
	wStat.AddData( "FY2", i->GetFy( 1 ));
	wStat.Send( 4, false, INVALIDSERIAL );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ColorsTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Use dyes to apply a color to a targeted dye tub or hair dye item
//o------------------------------------------------------------------------------------------------o
void ColorsTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();

	// Check if item used to initialize target cursor is still within range
	CItem *tempObj = static_cast<CItem *>( s->TempObj() );
	s->TempObj( nullptr );
	if( ValidateObject( tempObj ))
	{
		if( tempObj->IsHeldOnCursor() || !CheckItemRange( mChar, tempObj ))
		{
			s->SysMessage( 400 ); // That is too far away!
			return;
		}
	}

	CItem *i = CalcItemObjFromSer( s->GetDWord( 7 ));
	if( !ValidateObject( i ))
		return;

	if( i->IsHeldOnCursor() || ( FindItemOwner( i ) != nullptr && FindItemOwner( i ) != mChar ) || !CheckItemRange( mChar, i ))
	{
		s->SysMessage( 400 ); // That is too far away!
		return;
	}

	if( i->GetId() == 0x0FAB || i->GetId() == 0x0EFF || i->GetId() == 0x0E27 ) // dye vat, hair dye
	{
		CPDyeVat toSend = ( *i );
		s->Send( &toSend );
	}
	else
	{
		s->SysMessage( 1031 ); // You can only use this item on a dye vat.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DVatTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Dye target object - if dyable - with specified dye tub color
//o------------------------------------------------------------------------------------------------o
void DVatTarget( CSocket *s )
{
	VALIDATESOCKET( s );

	CChar *mChar = s->CurrcharObj();

	// Check if item used to initialize target cursor is still within range
	CItem *tempObj = static_cast<CItem *>( s->TempObj() );
	s->TempObj( nullptr );
	if( ValidateObject( tempObj ))
	{
		if( tempObj->IsHeldOnCursor() || !CheckItemRange( mChar, tempObj ))
		{
			s->SysMessage( 400 ); // That is too far away!
			return;
		}
	}

	SERIAL serial	= s->GetDWord( 7 );
	CItem *i		= CalcItemObjFromSer( serial );
	if( !ValidateObject( i ))
		return;

	if( i->IsHeldOnCursor() || !CheckItemRange( mChar, i ))
	{
		s->SysMessage( 400 ); // That is too far away!
		return;
	}

	// Look for onDyeTarget event on dye tub
	std::vector<UI16> scriptTriggers = tempObj->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			if( toExecute->OnDyeTarget( mChar, tempObj, i ) == 0 ) // return false
			{
				// Don't continue with hard-coded dyeing
				return;
			}
		}
	}

	if( i->IsDyeable() )
	{
		if( i->IsLockedDown() )
		{
			CMultiObj *iMulti = i->GetMultiObj();
			if( ValidateObject( iMulti ) && !iMulti->IsOnOwnerList( mChar ))
			{
				s->SysMessage( 1032 ); // This is not yours!
				return;
			}
		}
		if( i->GetCont() != nullptr )
		{
			CChar *c = FindItemOwner( i );
			if( ValidateObject( c ) && c != mChar )
			{
				s->SysMessage( 1032 ); // This is not yours!
				return;
			}
		}

		i->SetColour((( s->AddId1() ) << 8 ) + s->AddId2() );
		Effects->PlaySound( s, 0x023E, true );
	}
	else
	{
		s->SysMessage( 1033 ); // You can only dye clothes with this.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	InfoTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Show information about targeted maptile or static item
//o------------------------------------------------------------------------------------------------o
void InfoTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	if( !s->GetByte( 1 ) && s->GetDWord( 7 ) < BASEITEMSERIAL )
	{
		s->SysMessage( 9039 ); // This command can not be used on characters.
		return;
	}

	const SI16 x		= s->GetWord( 11 );
	const SI16 y		= s->GetWord( 13 );
	const UI16 tileId	= s->GetWord( 17 );

	if( tileId == 0 )
	{
		// Map Tile
		UI08 worldNumber = 0;
		CChar *mChar = s->CurrcharObj();
		if( ValidateObject( mChar ))
		{
			worldNumber = mChar->WorldNumber();
		}

		// manually calculating the ID's if it's a maptype
		auto map1 = Map->SeekMap( x, y, worldNumber );
		CGumpDisplay mapStat( s, 300, 300 );
		mapStat.SetTitle( "Map Tile" );
		mapStat.AddData( "Tilenum", map1.tileId, 5 );
		mapStat.AddData( "Flags", map1.terrainInfo->FlagsNum(), 1 );
		mapStat.AddData( "Name", map1.name() );
		mapStat.Send( 4, false, INVALIDSERIAL );
	}
	else
	{
		// Static Tile
		CTile& tile = Map->SeekTile( tileId );
		
		CGumpDisplay statTile( s, 300, 300 );
		statTile.SetTitle( "Map Tile" );
		
		statTile.AddData( "Tilenum", tileId, 5 );
		statTile.AddData( "Weight", tile.Weight(), 0 );
		statTile.AddData( "Layer", tile.Layer(), 1 );
		statTile.AddData( "Hue", tile.Hue(), 5 );
		statTile.AddData( "Anim", tile.Animation(), 1 );
		statTile.AddData( "Quantity", tile.Quantity(), 1 );
		statTile.AddData( "Unknown1", tile.Unknown1(), 1 );
		statTile.AddData( "Unknown2", tile.Unknown2(), 1 );
		statTile.AddData( "Unknown3", tile.Unknown3(), 1 );
		statTile.AddData( "Unknown4", tile.Unknown4(), 1 );
		statTile.AddData( "Unknown5", tile.Unknown5(), 1 );
		statTile.AddData( "Height", tile.Height(), 0 );
		statTile.AddData( "Name", tile.Name().c_str() );
		statTile.AddData( "Flags:", tile.FlagsNum(), 1 );
		statTile.AddData( "--> FloorLevel", tile.CheckFlag( TF_FLOORLEVEL ));
		statTile.AddData( "--> Holdable", tile.CheckFlag( TF_HOLDABLE ));
		statTile.AddData( "--> Transparent", tile.CheckFlag( TF_TRANSPARENT ));
		statTile.AddData( "--> Translucent", tile.CheckFlag( TF_TRANSLUCENT ));
		statTile.AddData( "--> Wall", tile.CheckFlag( TF_WALL ));
		statTile.AddData( "--> Damaging", tile.CheckFlag( TF_DAMAGING ));
		statTile.AddData( "--> Blocking", tile.CheckFlag( TF_BLOCKING ));
		statTile.AddData( "--> Wet", tile.CheckFlag( TF_WET ));
		statTile.AddData( "--> Unknown1", tile.CheckFlag( TF_UNKNOWN1 ));
		statTile.AddData( "--> Surface", tile.CheckFlag( TF_SURFACE ));
		statTile.AddData( "--> Climbable", tile.CheckFlag( TF_CLIMBABLE ));
		statTile.AddData( "--> Stackable", tile.CheckFlag( TF_STACKABLE ));
		statTile.AddData( "--> Window", tile.CheckFlag( TF_WINDOW ));
		statTile.AddData( "--> NoShoot", tile.CheckFlag( TF_NOSHOOT ));
		statTile.AddData( "--> DisplayA", tile.CheckFlag( TF_DISPLAYA ));
		statTile.AddData( "--> DisplayAn", tile.CheckFlag( TF_DISPLAYAN ));
		statTile.AddData( "--> Description", tile.CheckFlag( TF_DESCRIPTION ));
		statTile.AddData( "--> Foilage", tile.CheckFlag( TF_FOLIAGE ));
		statTile.AddData( "--> PartialHue", tile.CheckFlag( TF_PARTIALHUE ));
		statTile.AddData( "--> Unknown2", tile.CheckFlag( TF_UNKNOWN2 ));
		statTile.AddData( "--> Map", tile.CheckFlag( TF_MAP ));
		statTile.AddData( "--> Container", tile.CheckFlag( TF_CONTAINER ));
		statTile.AddData( "--> Wearable", tile.CheckFlag( TF_WEARABLE ));
		statTile.AddData( "--> Light", tile.CheckFlag( TF_LIGHT ));
		statTile.AddData( "--> Animated", tile.CheckFlag( TF_ANIMATED ));
		statTile.AddData( "--> NoDiagonal", tile.CheckFlag( TF_NODIAGONAL )); //HOVEROVER in SA clients and later, to determine if tiles can be moved on by flying gargoyle
		statTile.AddData( "--> Unknown3", tile.CheckFlag( TF_UNKNOWN3 ));
		statTile.AddData( "--> Armor", tile.CheckFlag( TF_ARMOR ));
		statTile.AddData( "--> Roof", tile.CheckFlag( TF_ROOF ));
		statTile.AddData( "--> Door", tile.CheckFlag( TF_DOOR ));
		statTile.AddData( "--> StairBack", tile.CheckFlag( TF_STAIRBACK ));
		statTile.AddData( "--> StairRight", tile.CheckFlag( TF_STAIRRIGHT ));
		statTile.AddData( "--> AlphaBlend", tile.CheckFlag( TF_ALPHABLEND ));
		statTile.AddData( "--> UseNewArt", tile.CheckFlag( TF_USENEWART ));
		statTile.AddData( "--> ArtUsed", tile.CheckFlag( TF_ARTUSED ));
		statTile.AddData( "--> NoShadow", tile.CheckFlag( TF_NOSHADOW ));
		statTile.AddData( "--> PixelBleed", tile.CheckFlag( TF_PIXELBLEED ));
		statTile.AddData( "--> PlayAnimOnce", tile.CheckFlag( TF_PLAYANIMONCE ));
		statTile.AddData( "--> MultiMovable", tile.CheckFlag( TF_MULTIMOVABLE ));
		statTile.Send( 4, false, INVALIDSERIAL );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Tiling()
//|	Date		-	01/11/1999
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clicking the corners of tiling calls this function. Will fill up each tile
//|					of targeted area with specified item
//o------------------------------------------------------------------------------------------------o
void Tiling( CSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	if( s->ClickX() == -1 && s->ClickY() == -1 )
	{
		s->ClickX( s->GetWord( 11 ));
		s->ClickY( s->GetWord( 13 ));
		s->SendTargetCursor( 0, TARGET_TILING, 0, 1038 ); // Select second corner of bounding box.
		return;
	}

	SI16 x1 = s->ClickX(), x2 = s->GetWord( 11 );
	SI16 y1 = s->ClickY(), y2 = s->GetWord( 13 );
	SI16 j;

	s->ClickX( -1 );
	s->ClickY( -1 );

	if( x1 > x2 )
	{
		j = x1;
		x1 = x2;
		x2 = j;
	}
	if( y1 > y2 )
	{
		j = y1;
		y1 = y2;
		y2 = j;
	}

	UI16 addId = static_cast<UI16>((( s->AddId1() ) << 8 ) + s->AddId2() );
	SI32 rndVal = s->TempInt2();
	s->TempInt2( 0 );
	UI16 rndId = 0;

	CItem *c = nullptr;
	for( SI16 x = x1; x <= x2; ++x )
	{
		for( SI16 y = y1; y <= y2; ++y )
		{
			rndId = addId + RandomNum( static_cast<UI16>( 0 ), static_cast<UI16>( rndVal ));
			c = Items->CreateItem( nullptr, s->CurrcharObj(), rndId, 1, 0, OT_ITEM );
			if( !ValidateObject( c ))
				return;

			c->SetDecayable( false );
			c->SetLocation( x, y, s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 )));
		}
	}
	s->AddId1( 0 );
	s->AddId2( 0 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CreateBodyPart()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Create body parts after carving up a corpse
//o------------------------------------------------------------------------------------------------o
bool CreateBodyPart( CChar *mChar, CItem *corpse, std::string partId, SI32 dictEntry )
{
	CItem *toCreate = Items->CreateScriptItem( nullptr, mChar, partId, 1, OT_ITEM, false, 0x0 );
	if( !ValidateObject( toCreate ))
		return false;

	toCreate->SetName( oldstrutil::format( Dictionary->GetEntry( dictEntry ).c_str(), corpse->GetName2().c_str() ));
	toCreate->SetLocation( corpse );
	toCreate->SetOwner( corpse->GetOwnerObj() );
	toCreate->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAY ));
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	NewCarveTarget()
//|	Date		-	09/22/2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Target carving system.
//|
//|	Changes		-	unknown   	-	Human-corpse carving code added
//|
//|	Changes		-	unknown   	-	Scriptable carving product added
//|
//|	Changes		-	09/22/2002	-	Fixed erroneous names for body parts
//|									& made all body parts that are carved from human corpse
//|									lie in same direction.
//o------------------------------------------------------------------------------------------------o
auto NewCarveTarget( CSocket *s, CItem *i ) -> void
{
	VALIDATESOCKET( s );

	auto mChar = s->CurrcharObj();

	// Look for onCarveCorpse event on item
	std::vector<UI16> scriptTriggers = i->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			if( toExecute->OnCarveCorpse( mChar, i ) == 0 ) // return false
			{
				// Don't continue with hard-coded carving
				return;
			}
		}
	}

	auto c = Items->CreateItem( nullptr, mChar, 0x122A, 1, 0, OT_ITEM ); // add the blood puddle
	if( c == nullptr )
		return;

	// Place blood on ground
	c->SetLocation( i );
	c->SetMovable( 2 );
	c->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAY ));

	// if it's a human corpse
	// Sept 22, 2002 - Corrected the alignment of body parts that are carved.
	if( i->GetTempVar( CITV_MOREY, 2 ))
	{
		auto toFind	= FileLookup->FindEntry( "CARVE HUMAN", carve_def );
		if( toFind )
		{
			for( const auto &sec : toFind->collection() )
			{
				auto tag = sec->tag;
				if( oldstrutil::upper( tag ) == "ADDITEM" )
				{
					auto data = sec->data;
					data = oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ));
					auto csecs = oldstrutil::sections( data, "," );
					if( csecs.size() > 1 )
					{
						if( !CreateBodyPart( mChar, i, oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 ))))
						{
							return;
						}
					}
				}
			}
			
			MakeCriminal( mChar );
			
			std::vector<CItem *> corpseItems;
			auto iCont = i->GetContainsList();
			for( const auto &c : iCont->collection() )
			{
				if( ValidateObject( c ))
				{
					if( c->GetLayer() != IL_HAIR && c->GetLayer() != IL_FACIALHAIR )
					{
						// Store a reference to the item we want to move out of corpse...
						corpseItems.push_back( c );
					}
				}
			}

			// Loop through the items we want to move out of corpse
			std::for_each( corpseItems.begin(), corpseItems.end(), [i]( CItem *corpseItem )
			{
				corpseItem->SetCont( nullptr );
				corpseItem->SetLocation( i );
				corpseItem->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAY ));
			});

			i->Delete();
		}
	}
	else
	{
		auto sect = "CARVE "s + oldstrutil::number( i->GetCarve() );
		auto toFind	= FileLookup->FindEntry( sect, carve_def );
		if( toFind )
		{
			for( const auto &sec : toFind->collection() )
			{
				auto tag = sec->tag;
				if( oldstrutil::upper( tag ) == "ADDITEM" )
				{
					auto data = sec->data;
					data = oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ));
					auto csecs = oldstrutil::sections( data, "," );
					if( csecs.size() > 1 )
					{
						Items->CreateScriptItem( s, mChar, oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 )), OT_ITEM, true );
					}
					else
					{
						Items->CreateScriptItem( s, mChar, data, 0, OT_ITEM, true );
					}
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	AttackTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Make pet attack target
//o------------------------------------------------------------------------------------------------o
void AttackTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mPet	= static_cast<CChar *>( s->TempObj() );
	CChar *target = CalcCharObjFromSer( s->GetDWord( 7 ));
	s->TempObj( nullptr );

	if( !ValidateObject( target ) || !ValidateObject( mPet ))
		return;

	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	// Don't allow attacking offline characters
	if( !target->IsNpc() && !IsOnline( *target ))
		return;

	// Check if combat is disallowed in attacker's OR mPet's regions
	if( mChar->GetRegion()->IsSafeZone() || mPet->GetRegion()->IsSafeZone() || target->GetRegion()->IsSafeZone() )
	{
		// Target is in a safe zone where all aggressive actions are forbidden, disallow
		s->SysMessage( 1799 ); // Hostile actions are not permitted in this safe area.
		return;
	}

	// Check if the command was applied to all pets
	if( s->TempInt() == 1 )
	{
		s->TempInt( 0 );
		GenericList<CChar *> *myPets = mChar->GetPetList();
		for( CChar *myPet = myPets->First(); !myPets->Finished(); myPet = myPets->Next() )
		{
			// Make sure pet returned from petList is still a valid character
			if( !ValidateObject( myPet ))
				continue;

			// Make sure pet cannot attack itself
			if( target == myPet )
			{
				s->SysMessage( 1073 ); // Your pet cannot attack itself!
				continue;
			}

			// Make sure only nearby pets can respond
			if( !ObjInRange( mChar, myPet, 12 ))
				continue;

			// Make sure pets can only attack nearby targets
			if( !ObjInRange( mChar, target, 12 ))
			{
				s->SysMessage( 393 ); // That is too far away
				continue;
			}

			if( myPet->IsNpc() && myPet->GetOwnerObj() == mChar )
			{
				myPet->FlushPath();
				Combat->AttackTarget( myPet, target );
				if( target->IsInnocent() && target != myPet->GetOwnerObj() )
				{
					if( WillResultInCriminal( mChar, target ))
					{
						MakeCriminal( mChar );
					}
				}
			}
		}
	}
	else
	{
		// Make sure pet cannot attack itself
		if( target == mPet )
		{
			s->SysMessage( 1073 ); // Your pet cannot attack itself!
			return;
		}

		// Make sure only nearby pets can respond
		if( !ObjInRange( mChar, mPet, 12 ))
			return;

		// Make sure pet can only attack nearby targets
		if( !ObjInRange( mChar, target, 12 ))
		{
			s->SysMessage( 393 ); // That is too far away
			return;
		}

		// Make sure a friend cannot order a pet to attack its owner
		if( target == mPet->GetOwnerObj() && mChar != target )
		{
			s->SysMessage( 2408 ); // They are unwilling to attack their own master.
			return;
		}

		mPet->FlushPath();
		Combat->AttackTarget( mPet, target );
		if( target->IsInnocent() && target != mChar )
		{
			if( WillResultInCriminal( mChar, target ))
			{
				MakeCriminal( mChar );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FollowTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Make pet follow target
//o------------------------------------------------------------------------------------------------o
void FollowTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mPet	= static_cast<CChar *>( s->TempObj() );
	CChar *target	= CalcCharObjFromSer( s->GetDWord( 7 ));
	s->TempObj( nullptr );
	if( !ValidateObject( mPet ) || !ValidateObject( target ))
		return;

	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	if( s->TempInt() == 1 )
	{
		s->TempInt( 0 );

		GenericList<CChar *> *myPets = mChar->GetPetList();
		for( CChar *myPet = myPets->First(); !myPets->Finished(); myPet = myPets->Next() )
		{
			// Make sure pet returned from petList is still a valid character
			if( !ValidateObject( myPet ))
				continue;

			if( myPet == target )
			{
				s->SysMessage( 2388 ); // Your pet cannot follow itself!
				continue;
			}

			// Make sure only nearby pets can respond
			if( !ObjInRange( mChar, myPet, 12 ))
				continue;

			if( !ObjInRange( mChar, target, 12 ))
			{
				s->SysMessage( 393 ); // That is too far away
				continue;
			}

			if( myPet->IsNpc() && myPet->GetOwnerObj() == mChar )
			{
				myPet->SetFTarg( target );
				myPet->FlushPath();
				myPet->SetNpcWander( WT_FOLLOW );
			}
		}
	}
	else
	{
		if( mPet == target )
		{
			s->SysMessage( 2388 ); // Your pet cannot follow itself!
			return;
		}

		// Make sure only nearby pets can respond
		if( !ObjInRange( mChar, mPet, 12 ))
			return;

		if( !ObjInRange( mChar, target, 12 ))
		{
			s->SysMessage( 393 ); // That is too far away
			return;
		}

		mPet->SetFTarg( target );
		mPet->FlushPath();
		mPet->SetNpcWander( WT_FOLLOW );
	}
}

void		SendTradeStatus( CItem *cont1, CItem *cont2 );
CItem *		StartTrade( CSocket *mSock, CChar *i );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	TransferTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Transfer pet ownership to targeted player
//o------------------------------------------------------------------------------------------------o
void TransferTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *petChar = static_cast<CChar *>( s->TempObj() );
	CChar *targChar = CalcCharObjFromSer( s->GetDWord( 7 ));
	s->TempObj( nullptr );

	if( !ValidateObject( petChar ))
		return;

	if( petChar->IsDispellable() )
	{
		s->SysMessage( 2386 ); // You cannot transfer ownership of a summoned creature.
		return;
	}

	if( !ValidateObject( targChar ))
	{
		s->SysMessage( 1066 ); // That is not a valid person!
		return;
	}
	
	if( petChar == targChar )
	{
		s->SysMessage( 2371 ); // Ownership can only be transferred to other players.
		return;
	}

	if( petChar->IsAtWar() )
	{
		s->SysMessage( 2414 ); // You may not transfer a pet that has recently been in combat.
		if( targChar->GetSocket() != nullptr )
		{
			targChar->GetSocket()->SysMessage( 2415 ); //  The pet may not be transfered to you because it has recently been in combat.
		}
		return;
	}

	CChar *mChar = s->CurrcharObj();

	if( targChar == mChar )
	{
		s->SysMessage( 2409 ); // That would be pointless - you are already their master!
		return;
	}

	if( targChar->IsDead() )
	{
		s->SysMessage( 2384 ); // You cannot transfer to someone that is dead.
		return;
	}

	if( targChar->IsNpc() )
	{
		s->SysMessage( 2371 ); // Ownership can only be transferred to other players!
		return;
	}

	// Don't allow transfer of pet if either party is a criminal
	if( mChar->IsCriminal() )
	{
		s->SysMessage( 2379 ); // The pet refuses to be transferred because it will not obey you sufficiently.
		if( targChar->GetSocket() != nullptr )
		{
			targChar->GetSocket()->SysMessage( 2382, mChar->GetNameRequest( targChar ).c_str() ); // The pet will not accept you as a master because it does not trust %s.
		}
		return;
	}
	else if( targChar->IsCriminal() )
	{
		s->SysMessage( 2380, targChar->GetNameRequest( mChar ).c_str() ); // The pet refuses to be transferred because it will not obey %s.
		if( targChar->GetSocket() != nullptr )
		{
			targChar->GetSocket()->SysMessage( 2381 ); // The pet will not accept you as a master because it does not trust you.
		}
		return;
	}

	UI08 maxControlSlots = cwmWorldState->ServerData()->MaxControlSlots();
	if( maxControlSlots > 0 && ( targChar->GetControlSlotsUsed() + petChar->GetControlSlots() > maxControlSlots ))
	{
		s->SysMessage( 2391 ); // That would exceed the other player's maximum pet control slots.
		if( targChar->GetSocket() != nullptr )
		{
			targChar->GetSocket()->SysMessage( 2390 ); // That would exceed your maximum pet control slots.
		}
		return;
	}

	UI08 maxPetOwners = cwmWorldState->ServerData()->MaxPetOwners();
	if( petChar->IsOnPetOwnerList( targChar ) && maxPetOwners > 0 && ( petChar->GetPetOwnerList()->Num() >= maxPetOwners ))
	{
		s->SysMessage( 2402 ); // The creature has had too many masters and is not willing to do the bidding of another one!
		if( targChar->GetSocket() != nullptr )
		{
			targChar->GetSocket()->SysMessage( 2402 ); // The creature has had too many masters and is not willing to do the bidding of another one!
		}
		return;
	}

	// Check loyalty of pet to old master
	if( cwmWorldState->ServerData()->CheckPetControlDifficulty() && !Npcs->CanControlPet( mChar, petChar, true, false, false, true ))
	{
		s->SysMessage( 2379 ); // The pet refuses to be transferred because it will not obey you sufficiently.
		if( targChar->GetSocket() != nullptr )
		{
			targChar->GetSocket()->SysMessage( 2382, mChar->GetNameRequest( targChar ).c_str() ); // The pet will not accept you as a master because it does not trust %s.
		}
		return;
	}

	// Check loyalty of pet to new master
	if( cwmWorldState->ServerData()->CheckPetControlDifficulty() && !Npcs->CanControlPet( targChar, petChar, true, true, true, true ))
	{
		s->SysMessage( 2380 ); // The pet refuses to be transferred because it will not obey %s.
		if( targChar->GetSocket() != nullptr )
		{
			targChar->GetSocket()->SysMessage( 2381, mChar->GetNameRequest( targChar ).c_str() ); // The pet will not accept you as a master because it does not trust you.
		}
		return;
	}

	if( targChar->GetSocket() != nullptr )
	{
		// Create a pet transfer deed
		CItem *petTransferDeed = Items->CreateScriptItem( s, mChar, "0x14F0", 1, OT_ITEM, false, 0 );
		if( ValidateObject( petTransferDeed ))
		{
			std::string petName = GetNpcDictName( petChar );
			petTransferDeed->SetName( oldstrutil::format( "a transfer deed for %s (%s)", petName.c_str(), Dictionary->GetEntry( 3000 + petChar->GetId(), targChar->GetSocket()->Language() ).c_str() )); //cwmWorldState->creatures[petChar->GetId()].CreatureType().c_str() ));
			petTransferDeed->SetTempVar( CITV_MORE, petChar->GetSerial() );
			petTransferDeed->SetMovable( 2 ); // Disallow moving the deed out of the trade window
			petTransferDeed->SetType( IT_PETTRANSFERDEED ); // Set a type to let us identify it later when trade is accepted

			// Open a trade window, and place the petTransferDeed in the window
			CItem *j = StartTrade( s, targChar );
			if( ValidateObject( j ))
			{
				petTransferDeed->SetCont( j );
				petTransferDeed->SetX( 30 );
				petTransferDeed->SetY( 30 );
				petTransferDeed->SetZ( 9 );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	BuyShop()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens a vendor's buy menu with list of items for sale
//o------------------------------------------------------------------------------------------------o
bool BuyShop( CSocket *s, CChar *c )
{
	if( s == nullptr )
		return false;

	if( !ValidateObject( c ))
		return false;

	std::vector<UI16> scriptTriggers = c->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			if( toExecute->OnBuy( s, c ) == 0 )
				return false;
		}
	}

	CItem *sellPack		= c->GetItemAtLayer( IL_SELLCONTAINER );
	CItem *boughtPack	= c->GetItemAtLayer( IL_BOUGHTCONTAINER );

	if( !ValidateObject( sellPack ) || !ValidateObject( boughtPack ))
		return false;

	CPItemsInContainer iic;
	if( s->ClientVerShort() >= CVS_6017 )
	{
		iic.UOKRFlag( true );
	}
	iic.Type( 0x02 );
	iic.VendorSerial( sellPack->GetSerial() );
	CPOpenBuyWindow obw( sellPack, c, iic, s );

	CPItemsInContainer iic2;
	if( s->ClientVerShort() >= CVS_6017 )
	{
		iic2.UOKRFlag( true );
	}
	iic2.Type( 0x02 );
	iic2.VendorSerial( boughtPack->GetSerial() );
	CPOpenBuyWindow obw2( boughtPack, c, iic2, s );

	CPDrawContainer toSend;
	toSend.Model( 0x0030 );
	toSend.Serial( c->GetSerial() );
	if( s->ClientType() >= CV_HS2D && s->ClientVerShort() >= CVS_7090 )
	{
		toSend.ContType( 0x00 );
	}

	s->Send( &iic );
	s->Send( &iic2 );
	s->Send( &obw );
	s->Send( &obw2 );
	s->Send( &toSend );

	s->StatWindow( s->CurrcharObj() ); // Make sure the gold total has been sent.
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	NpcResurrectTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Resurrects a character
//o------------------------------------------------------------------------------------------------o
//|	Changes		-	09/22/2002	-	Made players not appear with full
//|									health/stamina after being resurrected by NPC Healer
//o------------------------------------------------------------------------------------------------o
void NpcResurrectTarget( CChar *i )
{
	if( !ValidateObject( i ))
		return;

	if( i->IsNpc() )
	{
		Console.Error( oldstrutil::format( Dictionary->GetEntry( 1079 ), i )); // Resurrect attempted on character %i.
		return;
	}
	CSocket *mSock = i->GetSocket();
	// the char is a PC, but not logged in.....
	if( mSock != nullptr )
	{
		if( i->IsDead() )
		{
			std::vector<UI16> scriptTriggers = i->GetScriptTriggers();
			for( auto scriptTrig : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					if( toExecute->OnResurrect( i ) == 1 )	// if it exists and we don't want hard code, return
						return;
				}
			}

			Fame( i, 0 );
			Effects->PlaySound( i, 0x0214 );
			i->SetId( i->GetOrgId() );
			i->SetSkin( i->GetOrgSkin() );
			i->SetDead( false );

			// Restore hair
			UI16 hairStyleId = i->GetHairStyle();
			UI16 hairStyleColor = i->GetHairColour();
			CItem *hairItem = Items->CreateItem( mSock, i, hairStyleId, 1, hairStyleColor, OT_ITEM );

			if( hairItem != nullptr )
			{
				hairItem->SetDecayable( false );
				hairItem->SetLayer( IL_HAIR );
				hairItem->SetCont( i );
			}

			// Restore beard
			UI16 beardStyleId = i->GetBeardStyle();
			UI16 beardStyleColor = i->GetBeardColour();
			CItem *beardItem = Items->CreateItem( mSock, i, beardStyleId, 1, beardStyleColor, OT_ITEM );

			if( beardItem != nullptr )
			{
				beardItem->SetDecayable( false );
				beardItem->SetLayer( IL_FACIALHAIR );
				beardItem->SetCont( i );
			}

			i->SetHP( i->GetMaxHP() / 10 );
			i->SetStamina( i->GetMaxStam() / 10 );
			i->SetMana( i->GetMaxMana() / 10 );
			i->SetAttacker( nullptr );
			i->SetAttackFirst( false );
			i->SetWar( false );
			i->SetHunger( 6 );
			CItem *c = nullptr;
			for( CItem *j = i->FirstItem(); !i->FinishedItems(); j = i->NextItem() )
			{
				if( ValidateObject( j ) && !j->IsFree() )
				{
					if( j->GetLayer() == IL_BUYCONTAINER )
					{
						j->SetLayer( IL_PACKITEM );
						i->SetPackItem( j );
					}
					if( j->GetSerial() == i->GetRobe() )
					{
						j->Delete();

						c = Items->CreateScriptItem( nullptr, i, "resurrection_robe", 1, OT_ITEM );
						if( c != nullptr )
						{
							c->SetCont( i );
						}
					}
				}
			}
		}
		else
		{
			mSock->SysMessage( 1080 ); // That person isn't dead.
		}
	}
	else
	{
		Console.Warning( oldstrutil::format( "Attempt made to resurrect a PC (serial: 0x%X) that's not logged in", i->GetSerial() ));
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ShowSkillTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Show a gump with information about target character's skills
//o------------------------------------------------------------------------------------------------o
void ShowSkillTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = CalcCharObjFromSer( s->GetDWord( 7 ));
	if( !ValidateObject( mChar ))
	{
		s->SysMessage( 1103 ); // Not a valid target!
		return;
	}

	SI32 dispType = s->TempInt();
	UI16 skillVal;

	CGumpDisplay showSkills( s, 300, 300 );
	showSkills.SetTitle( "Skills Info" );
	for( UI08 i = 0; i < ALLSKILLS; ++i )
	{
		if( dispType == 0 || dispType == 1 )
		{
			skillVal = mChar->GetBaseSkill( i );
		}
		else
		{
			skillVal = mChar->GetSkill( i );
		}

		if( skillVal > 0 || dispType%2 == 0 )
		{
			showSkills.AddData( cwmWorldState->skill[i].name, oldstrutil::number( static_cast<R32>( skillVal ) / 10 ), 8 );
		}
	}
	showSkills.Send( 4, false, INVALIDSERIAL );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	FriendTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Add targeted player to pet's friendslist
//o------------------------------------------------------------------------------------------------o
void FriendTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	CChar *targChar = CalcCharObjFromSer( s->GetDWord( 7 ));
	if( !ValidateObject( targChar ))
	{
		s->SysMessage( 1103 ); // Not a valid target!
		return;
	}
	if( targChar->IsNpc() || !IsOnline(( *targChar )))
	{
		s->SysMessage( 1622 ); // That person cannot be made a friend of this creature.
		return;
	}

	CChar *pet = static_cast<CChar *>( s->TempObj() );
	s->TempObj( nullptr );

	if( !ValidateObject( pet ))
	{
		s->SysMessage( 1999 ); // Object not found
		return;
	}

	if( targChar == mChar )
	{
		s->SysMessage( 2409 ); // That would be pointless - you are already their master!
		return;
	}

	if( pet->IsDispellable() )
	{
		s->SysMessage( 2385 ); // Summoned creatures are loyal only to their summoners.
		return;
	}

	if( Npcs->CheckPetFriend( targChar, pet ))
	{
		s->SysMessage( 1621 ); // That person is already a friend of this creature!
		return;
	}

	auto petFriends = pet->GetFriendList();
	// Make sure to cover the STL response
	if( petFriends != nullptr )
	{
		if( petFriends->size() >= 10 )
		{
			s->SysMessage( 1623 ); // This creature has too many friends.
			return;
		}
	}

	// Check loyalty of pet to master
	if( cwmWorldState->ServerData()->CheckPetControlDifficulty() && !Npcs->CanControlPet( mChar, pet, false, true, true ))
	{
		s->SysMessage( 2417 ); // The pet refuses to accept a new friend because it will not obey you sufficiently.
		if( targChar->GetSocket() != nullptr )
		{
			targChar->GetSocket()->SysMessage( 2418, mChar->GetNameRequest( targChar ).c_str() ); // The pet will not accept you as a friend because it does not trust %s.
		}
		return;
	}

	// Check loyalty of pet to new friend
	if( cwmWorldState->ServerData()->CheckPetControlDifficulty() && !Npcs->CanControlPet( targChar, pet, false, true, true ))
	{
		s->SysMessage( 2419, targChar->GetNameRequest( mChar ).c_str() ); // The pet refuses to accept %s as a friend because it will not obey them.
		if( targChar->GetSocket() != nullptr )
		{
			targChar->GetSocket()->SysMessage( 2420 ); // The pet will not accept you as a friend because it does not trust you.
		}
		return;
	}

	if( pet->AddFriend( targChar ))
	{
		// %s will now treat %s as a friend.
		std::string petName = GetNpcDictName( pet, s );
		s->SysMessage( 1624, petName.c_str(), targChar->GetNameRequest( mChar ).c_str() );

		// Inform the player added as friend
		CSocket *targSock = targChar->GetSocket();
		if( targSock != nullptr )
		{
			// %s has befriended %s to you.
			petName = GetNpcDictName( pet, targSock );
			targSock->SysMessage( 1625, mChar->GetNameRequest( targChar ).c_str(), petName.c_str() );
		}
	}
	else
	{
		// You cannot do that at the moment
		s->SysMessage( 394 );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	RemoveFriendTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove targeted player from pet's friendslist
//o------------------------------------------------------------------------------------------------o
void RemoveFriendTarget( CSocket *s )
{
	// Validate player's socket and character
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	// Get targeted character from socket
	CChar *targChar = CalcCharObjFromSer( s->GetDWord( 7 ));
	if( !ValidateObject( targChar ))
	{
		s->SysMessage( 1103 ); // Not a valid target!
		return;
	}
	if( targChar->IsNpc() || !IsOnline(( *targChar )) || targChar == mChar )
	{
		s->SysMessage( 1103 ); // Not a valid target!
		return;
	}

	CChar *pet = static_cast<CChar *>( s->TempObj() );
	s->TempObj( nullptr );
	if( !Npcs->CheckPetFriend( targChar, pet ))
	{
		s->SysMessage( 1856 ); // That player is not on the friend list!
		return;
	}

	if( pet->RemoveFriend( targChar ))
	{
		// %s has been removed from %s's friend list.
		std::string petName = GetNpcDictName( pet, s );
		s->SysMessage( 2300, targChar->GetNameRequest( mChar ).c_str(), petName.c_str() );

		// Inform the player removed as friend
		CSocket *targSock = targChar->GetSocket();
		if( targSock != nullptr )
		{	
			petName = GetNpcDictName( pet, targSock );

			// You have been removed from %s's friend list.
			targSock->SysMessage( 2301, petName.c_str() );
		}
	}
	else
	{
		// You cannot do that at the moment
		s->SysMessage( 394 );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GuardTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Command pet to guard target object
//|	Notes		-	PRE: Pet has been commanded to guard
//|					POST: Pet guards person, if owner currently
//o------------------------------------------------------------------------------------------------o
void GuardTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	CChar *petGuarding = static_cast<CChar *>( s->TempObj() );
	s->TempObj( nullptr );
	if( !ValidateObject( petGuarding ))
		return;

	Npcs->StopPetGuarding( petGuarding );

	CChar *charToGuard = CalcCharObjFromSer( s->GetDWord( 7 ));
	if( ValidateObject( charToGuard ))
	{
		if( charToGuard != petGuarding->GetOwnerObj() && !Npcs->CheckPetFriend( charToGuard, petGuarding ))
		{
			s->SysMessage( 1628 ); // Your pet may only guard you, his friends, and items in your house!
			return;
		}
		petGuarding->SetNPCAiType( AI_PET_GUARD ); // 32 is guard mode
		petGuarding->SetGuarding( charToGuard );
		mChar->SetGuarded( true );
		petGuarding->SetFTarg( charToGuard );
		petGuarding->FlushPath();
		petGuarding->SetNpcWander( WT_FOLLOW );

		if( charToGuard == mChar )
		{
			s->SysMessage( 1321 ); // Your pet is now guarding you.
		}
		else
		{
			if( charToGuard->GetSocket() != nullptr )
			{
				std::string petName = GetNpcDictName( petGuarding, charToGuard->GetSocket() );
				charToGuard->GetSocket()->SysMessage( 2374, petName.c_str() ); // ~1_PETNAME~ is now guarding you.
			}
		}

		return;
	}
	CItem *itemToGuard = CalcItemObjFromSer( s->GetDWord( 7 ));
	if( ValidateObject( itemToGuard ) && !itemToGuard->IsPileable() )
	{
		CMultiObj *multi = itemToGuard->GetMultiObj();
		if( ValidateObject( multi ))
		{
			if( multi->GetOwnerObj() == mChar )
			{
				petGuarding->SetNPCAiType( AI_PET_GUARD );
				petGuarding->SetGuarding( itemToGuard );
				itemToGuard->SetGuarded( true );
			}
		}
		else
		{
			s->SysMessage( 1628 ); // Your pet may only guard you, his friends, and items in your house!
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MakeTownAlly()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds town of targeted character as an ally of player's town
//o------------------------------------------------------------------------------------------------o
void MakeTownAlly( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	CChar *targetChar = CalcCharObjFromSer( s->GetDWord( 7 ));
	if( !ValidateObject( targetChar ))
	{
		s->SysMessage( 1110 ); // No such character exists!
		return;
	}
	UI16 srcTown = mChar->GetTown();
	UI16 trgTown = targetChar->GetTown();

	if( srcTown == 0 || trgTown == 0 )
	{
		return;
	}

	if( !cwmWorldState->townRegions[srcTown]->MakeAlliedTown( trgTown ))
	{
		s->SysMessage( 1111 ); // You were unable to do that.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MakeStatusTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Change privileges of targeted character to specified command level
//|					as defined in the COMMANDSLEVEL section of dfndata/commands/commands.dfn
//o------------------------------------------------------------------------------------------------o
void MakeStatusTarget( CSocket *sock )
{
	VALIDATESOCKET( sock );
	CChar *targetChar = CalcCharObjFromSer( sock->GetDWord( 7 ));
	if( !ValidateObject( targetChar ))
	{
		sock->SysMessage( 1110 ); // No such character exists!
		return;
	}
	if( targetChar->IsDead() )
	{
		sock->SysMessage( 7509 ); // Some people just won't let the dead rest in peace.
		return;
	}
	UI08 origCommand			= targetChar->GetCommandLevel();
	CommandLevel_st *targLevel	= Commands->GetClearance( sock->XText() );
	CommandLevel_st *origLevel	= Commands->GetClearance( origCommand );

	if( targLevel == nullptr )
	{
		sock->SysMessage( 1112 ); // No such clearance level!
		return;
	}
	CChar *mChar = sock->CurrcharObj();
	//char temp[1024], temp2[1024];

	UI08 targetCommand = targLevel->commandLevel;
	auto temp = oldstrutil::format( "account%i.log", mChar->GetAccount().wAccountIndex );
	auto temp2 = oldstrutil::format( "%s has made %s a %s.\n", mChar->GetName().c_str(), targetChar->GetName().c_str(), targLevel->name.c_str() );

	Console.Log( temp2, temp );

	DismountCreature( targetChar );

	if( targLevel->targBody != 0 )
	{
		targetChar->SetId( targLevel->targBody );
		targetChar->SetOrgId( targLevel->targBody );
	}
	if( targLevel->bodyColour != 0 )
	{
		targetChar->SetSkin( targLevel->bodyColour );
		targetChar->SetOrgSkin( targLevel->bodyColour );
	}

	targetChar->SetPriv( targLevel->defaultPriv );
	targetChar->SetCommandLevel( targetCommand );

	if( targLevel->allSkillVals != 0 )
	{
		for( UI08 j = 0; j < ALLSKILLS; ++j )
		{
			targetChar->SetBaseSkill( targLevel->allSkillVals, j );
			targetChar->SetSkill( targLevel->allSkillVals, j );
		}
		targetChar->SetStrength( 100 );
		targetChar->SetDexterity( 100 );
		targetChar->SetIntelligence( 100 );
		targetChar->SetStamina(  100 );
		targetChar->SetHP( 100 );
		targetChar->SetMana( 100 );
	}

	std::string playerName = targetChar->GetName();
	if( targetCommand != origCommand && origLevel != nullptr )
	{
		const size_t position = playerName.find( origLevel->title );
		if( position != std::string::npos )
		{
			playerName.replace( position, origLevel->title.size(), "" );
		}
	}
	if( targetCommand != 0 && targetCommand != origCommand )
	{
		targetChar->SetName( oldstrutil::trim(oldstrutil::format("%s %s", targLevel->title.c_str(), oldstrutil::trim(playerName).c_str() )) );
	}
	else if( origCommand != 0 )
	{
		targetChar->SetName( oldstrutil::trim( playerName ));
	}

	CItem *mypack = targetChar->GetPackItem();

	if( targLevel->stripOff.any() )
	{
		for( CItem *z = targetChar->FirstItem(); !targetChar->FinishedItems(); z = targetChar->NextItem() )
		{
			if( ValidateObject( z ))
			{
				switch( z->GetLayer() )
				{
					case IL_HAIR:
					case IL_FACIALHAIR:
						if( targLevel->stripOff.test( BIT_STRIPHAIR ))
						{
							z->Delete();
						}
						break;
					case IL_NONE:
					case IL_MOUNT:
					case IL_PACKITEM:
					case IL_BANKBOX:
						break;
					default:
						if( targLevel->stripOff.test( BIT_STRIPITEMS ))
						{
							if( !ValidateObject( mypack ))
							{
								mypack = targetChar->GetPackItem();
							}
							if( !ValidateObject( mypack ))
							{
								CItem *iMade = Items->CreateItem( nullptr, targetChar, 0x0E75, 1, 0, OT_ITEM );
								if( !ValidateObject( iMade ))
									return;

								targetChar->SetPackItem( iMade );
								iMade->SetDecayable( false );
								iMade->SetLayer( IL_PACKITEM );
								if( iMade->SetCont( targetChar ))
								{
									iMade->SetType( IT_CONTAINER );
									iMade->SetDye( true );
									mypack = iMade;
								}
							}
							z->SetCont( mypack );
							z->PlaceInPack();
						}
						break;
				}
			}
		}
	}
	targetChar->Teleport();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SmeltTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Smelt targeted item to receive some crafting resources
//o------------------------------------------------------------------------------------------------o
void SmeltTarget( CSocket *s )
{
	VALIDATESOCKET( s );

	CChar *mChar = s->CurrcharObj();
	// Check if item used to initialize target cursor is still within range
	CItem *tempObj = static_cast<CItem *>( s->TempObj() );
	s->TempObj( nullptr );
	if( ValidateObject( tempObj ))
	{
		if( tempObj->IsHeldOnCursor() || !CheckItemRange( mChar, tempObj ))
		{
			s->SysMessage( 400 ); // That is too far away!
			return;
		}
	}

	CItem *i = CalcItemObjFromSer( s->GetDWord( 7 ));
	if( !ValidateObject( i ) || i->GetCont() == nullptr )
		return;

	if( i->IsHeldOnCursor() || !CheckItemRange( mChar, i ))
	{
		s->SysMessage( 400 ); // That is too far away!
		return;
	}

	if( i->GetCreator() == INVALIDSERIAL )
	{
		s->SysMessage( 1113 ); // You can melt only player made items.
		return;
	}

	UI16 iMadeFrom = i->EntryMadeFrom();

	CreateEntry_st *ourCreateEntry = Skills->FindItem( iMadeFrom );
	if( iMadeFrom == 0 || ourCreateEntry == nullptr )
	{
		s->SysMessage( 1114 ); // You have no knowledge on how to melt that.
		return;
	}

	R32 avgMin = ourCreateEntry->AverageMinSkill();
	if( mChar->GetSkill( MINING ) < avgMin )
	{
		s->SysMessage( 1115 ); // You aren't skilled enough to melt this.
		return;
	}
	R32 avgMax = ourCreateEntry->AverageMaxSkill();

	Skills->CheckSkill( mChar, MINING, static_cast<SI16>( avgMin ), static_cast<SI16>( avgMax ));

	UI08 sumAmountRestored = 0;

	for( UI32 skCtr = 0; skCtr < ourCreateEntry->resourceNeeded.size(); ++skCtr )
	{
		UI16 amtToRestore = ourCreateEntry->resourceNeeded[skCtr].amountNeeded / 2;
		std::string itemId = oldstrutil::number( ourCreateEntry->resourceNeeded[skCtr].idList.front(), 16 );
		UI16 itemColour = ourCreateEntry->resourceNeeded[skCtr].colour;
		sumAmountRestored += amtToRestore;
		Items->CreateScriptItem( s, mChar, "0x" + itemId, amtToRestore, OT_ITEM, true, itemColour );
	}

	s->SysMessage( 1116, sumAmountRestored ); // You melt the item and place %i ingots in your pack.
	i->Delete();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	VialTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles targeting of objects after player uses a vial to create necro reagents
//o------------------------------------------------------------------------------------------------o
void VialTarget( CSocket *mSock )
{
	VALIDATESOCKET( mSock );

	CItem *nVialId = static_cast<CItem *>( mSock->TempObj() );
	mSock->TempObj( nullptr );

	SERIAL targSerial = mSock->GetDWord( 7 );
	if( targSerial == INVALIDSERIAL )
		return;

	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	if( ValidateObject( nVialId ))
	{
		if( nVialId->IsHeldOnCursor() || !CheckItemRange( mChar, nVialId ))
		{
			mSock->SysMessage( 400 ); // That is too far away!
			return;
		}

		CItem *nDagger = Combat->GetWeapon( mChar );
		if( !ValidateObject( nDagger ))
		{
			mSock->SysMessage( 742 ); // You do not have a dagger in your hands!
			return;
		}
		if( nDagger->GetId() != 0x0F51 && nDagger->GetId() != 0x0F52 )
		{
			mSock->SysMessage( 743 ); // That is not a dagger!
			return;
		}

		nVialId->SetTempVar( CITV_MORE, 1, 0 );
		if( targSerial >= BASEITEMSERIAL ) // it's an item
		{
			CItem *targItem = CalcItemObjFromSer( targSerial );
			if( !targItem->IsCorpse() )
			{
				mSock->SysMessage( 749 ); // That is not a corpse!
			}
			else
			{
				if( !CheckItemRange( mChar, targItem ))
				{
					mSock->SysMessage( 400 ); // That is too far away!
					return;
				}

				nVialId->SetTempVar( CITV_MORE, 1, targItem->GetTempVar( CITV_MORE, 1 ));
				Karma( mChar, nullptr, -1000 );
				if( targItem->GetTempVar( CITV_MORE, 2 ) < 4 )
				{
					mSock->SysMessage( 750 ); // You take a sample of blood from the corpse.
					Skills->MakeNecroReg( mSock, nVialId, 0x0E24 );
					targItem->SetTempVar( CITV_MORE, 2, targItem->GetTempVar( CITV_MORE, 2 ) + 1 );
				}
				else
				{
					mSock->SysMessage( 751 ); // You examine the corpse, but decide any further blood samples would be too contaminated.
				}
			}
		}
		else // it's a char
		{
			CChar *targChar = CalcCharObjFromSer( targSerial );
			if( targChar == mChar )
			{
				if( targChar->GetHP() <= 10 )
				{
					mSock->SysMessage( 744 ); // You are too wounded to continue
				}
				else
				{
					mSock->SysMessage( 745 ); // You prick your finger and fill the vial
				}
			}
			else if( ObjInRange( mChar, targChar, DIST_NEARBY ))
			{
				if( targChar->IsNpc() )
				{
					if( targChar->GetId( 1 ) == 0x00 && ( targChar->GetId( 2 ) == 0x0C || ( targChar->GetId( 2 ) >= 0x3B && targChar->GetId( 2 ) <= 0x3D )) )
					{
						nVialId->SetTempVar( CITV_MORE, 1, 1 );
					}
				}
				else
				{
					CSocket *nCharSocket = targChar->GetSocket();
					nCharSocket->SysMessage( 746, mChar->GetNameRequest( targChar ).c_str() ); // %s has pricked you with a dagger and sampled your blood
				}
				if( WillResultInCriminal( mChar, targChar ))
				{
					MakeCriminal( mChar );
				}
				Karma( mChar, targChar, -targChar->GetKarma() );
			}
			else
			{
				mSock->SysMessage( 747 ); // That individual is not anywhere near you.
				return;
			}
			if( targChar->Damage( RandomNum( 0, 5 ) + 2, PHYSICAL ))
			{
				Skills->MakeNecroReg( mSock, nVialId, 0x0E24 );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPITargetCursor::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Runs various commands based upon the target ID we sent to the socket
//o------------------------------------------------------------------------------------------------o
//| Changes		-	Overhauled to use an enum allowing simple modification
//o------------------------------------------------------------------------------------------------o
bool CPITargetCursor::Handle( void )
{
	CChar *mChar = tSock->CurrcharObj();
	if( tSock->TargetOK() )
	{
		if( !tSock->GetByte( 1 ) && !tSock->GetDWord( 7 ) && tSock->GetDWord( 11 ) == INVALIDSERIAL )
		{
			if( mChar->GetSpellCast() != -1 )	// need to stop casting if we don't target right
			{
				mChar->StopSpell();
			}
			return true; // do nothing if user cancels, avoids CRASH!
		}
		if( tSock->GetByte( 1 ) == 1 && !tSock->GetDWord( 7 ))
		{
			tSock->SetDWord( 7, INVALIDSERIAL );	// Client sends TargSer as 0 when we target an XY/Static, use INVALIDSERIAL as 0 could be a valid Serial -
		}

		UI08 a1 = tSock->GetByte( 2 );
		UI08 a2 = tSock->GetByte( 3 );
		UI08 a3 = tSock->GetByte( 4 );
		TargetIds targetId = static_cast<TargetIds>( tSock->GetByte( 5 ));
		tSock->TargetOK( false );
		if( mChar->IsDead() && !mChar->IsGM() && mChar->GetAccount().wAccountIndex != 0 )
		{
			tSock->SysMessage( 1008 ); // Invalid item!
			if( mChar->GetSpellCast() != -1 )	// need to stop casting if we don't target right
			{
				mChar->StopSpell();
			}
			return true;
		}
		if( a1 == 0 && a2 == 1 )
		{
			if( a3 == 2 )	// Guilds
			{
				HandleGuildTarget( tSock );
				return true;
			}
			else if( a3 == 1 )	// CustomTarget
			{
				// not a great fix, but better then assuming a ptr size.
				cScript *tScript = tSock->scriptForCallBack;
				//cScript *tScript	= reinterpret_cast<cScript *>( tSock->TempInt() );
				if( tScript != nullptr )
				{
					tScript->DoCallback( tSock, tSock->GetDWord( 7 ), static_cast<UI08>( targetId ));
				}
				return true;
			}
			else if( a3 == 0 )
			{
				switch( targetId )
				{
					case TARGET_ADDSCRIPTNPC:	AddScriptNpc( tSock );					break;
					case TARGET_BUILDHOUSE:		BuildHouseTarget( tSock );				break;
					case TARGET_TELE:			TeleTarget( tSock );					break;
					case TARGET_DYE:			DyeTarget( tSock );						break;
					case TARGET_DYEALL:			ColorsTarget( tSock );					break;
					case TARGET_DVAT:			DVatTarget( tSock );					break;
					case TARGET_INFO:			InfoTarget( tSock );					break;
					case TARGET_WSTATS:			WStatsTarget( tSock );					break;
					case TARGET_NPCRESURRECT:	NpcResurrectTarget( mChar );			break;
					case TARGET_MAKESTATUS:		MakeStatusTarget( tSock );				break;
					case TARGET_VIAL:			VialTarget( tSock );					break;
					case TARGET_TILING:			Tiling( tSock );						break;
					case TARGET_SHOWSKILLS:		ShowSkillTarget( tSock );				break;
						// Vendors
					case TARGET_PLVBUY:			PlayerVendorBuy( tSock );						break;
						// Town Stuff
					case TARGET_TOWNALLY:		MakeTownAlly( tSock );					break;
					case TARGET_VOTEFORMAYOR:	cwmWorldState->townRegions[mChar->GetTown()]->VoteForMayor( tSock ); break;
						// Pets
					case TARGET_FOLLOW:			FollowTarget( tSock );					break;
					case TARGET_ATTACK:			AttackTarget( tSock );					break;
					case TARGET_TRANSFER:		TransferTarget( tSock );				break;
					case TARGET_GUARD:			GuardTarget( tSock );					break;
					case TARGET_FRIEND:			FriendTarget( tSock );					break;
					case TARGET_REMOVEFRIEND:	RemoveFriendTarget( tSock );			break;
						// Magic
					case TARGET_CASTSPELL:		Magic->CastSpell( tSock, mChar );		break;
						// Skills Functions
					case TARGET_SMITH:			Skills->Smith( tSock );					break;
					case TARGET_MINE:			Skills->Mine( tSock );					break;
					case TARGET_SMELTORE:		Skills->SmeltOre( tSock );				break;
					case TARGET_REPAIRMETAL:	Skills->RepairMetal( tSock );			break;
					case TARGET_SMELT:			SmeltTarget( tSock );					break;
					case TARGET_STEALING:		Skills->StealingTarget( tSock );		break;
					case TARGET_PARTYADD:		PartyFactory::GetSingleton().CreateInvite( tSock );	break;
					case TARGET_PARTYREMOVE:	PartyFactory::GetSingleton().Kick( tSock );			break;
					default:															break;
				}
			}
		}
	}
	mChar->BreakConcentration( tSock );
	return true;
}
