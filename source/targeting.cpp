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

#include "ObjectFactory.h"
#include "PartySystem.h"

#undef DBGFILE
#define DBGFILE "targeting.cpp"

namespace UOX
{

void tweakItemMenu( CSocket *s, CItem *j );
void tweakCharMenu( CSocket *s, CChar *c );
void OpenPlank( CItem *p );

void PlVBuy( CSocket *s )//PlayerVendors
{
	VALIDATESOCKET( s );

	CChar *vChar = static_cast<CChar *>(s->TempObj());
	s->TempObj( NULL );
	if( !ValidateObject( vChar ) || vChar->isFree() ) 
		return;

	CChar *mChar	= s->CurrcharObj();
	UI32 gleft		= GetItemAmount( mChar, 0x0EED );

	CItem *p		= mChar->GetPackItem();
	if( !ValidateObject( p ) ) 
	{
		s->sysmessage( 773 ); 
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) || i->GetCont() == NULL ) 
		return;
	
	if( FindItemOwner( i ) != vChar )
		return;
	if( vChar->GetNPCAiType() != AI_PLAYERVENDOR )
		return;
	if( mChar == vChar->GetOwnerObj() )
	{
		vChar->TextMessage( s, 999, TALK, false );
		return;
	}
	if ( i->GetBuyValue() <= 0 )
		return;

	if( gleft < i->GetBuyValue() ) 
	{
		vChar->TextMessage( s, 1000, TALK, false );
		return;
	} 
	else 
	{
		UI32 tAmount = DeleteItemAmount( mChar, i->GetBuyValue(), 0x0EED );
		// tAmount > 0 indicates there wasn't enough money...
		// could be expanded to take money from bank too...
	}
	
	vChar->TextMessage( s, 1001, TALK, false );
	vChar->SetHoldG( vChar->GetHoldG() + i->GetBuyValue() );

	i->SetCont( p );	// move containers
}

void HandleGuildTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *trgChar	= NULL;
	CChar *mChar	= s->CurrcharObj();
	CGuild *mGuild	= NULL, *tGuild = NULL;
	switch( s->GetByte( 5 ) )
	{
		case 0:	// recruit character
			trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
			if( ValidateObject( trgChar ) )
			{
				if( trgChar->GetGuildNumber() == -1 )	// no existing guild
				{
					mGuild = GuildSys->Guild( mChar->GetGuildNumber() );
					if( mGuild != NULL )
						mGuild->NewRecruit( (*trgChar) );
				}
				else
					s->sysmessage( 1002 );
			}
			break;
		case 1:		// declare fealty
			trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
			if( ValidateObject( trgChar ) )
			{
				if( trgChar->GetGuildNumber() == mChar->GetGuildNumber() )	// same guild
					mChar->SetGuildFealty( trgChar->GetSerial() );
				else
					s->sysmessage( 1003 );
			}
			break;
		case 2:	// declare war
			trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
			if( ValidateObject( trgChar ) )
			{
				if( trgChar->GetGuildNumber() != mChar->GetGuildNumber() )
				{
					if( trgChar->GetGuildNumber() == -1 )
						s->sysmessage( 1004 );
					else
					{
						mGuild = GuildSys->Guild(mChar->GetGuildNumber() );
						if( mGuild != NULL )
						{
							mGuild->SetGuildRelation( trgChar->GetGuildNumber(), GR_WAR );
							tGuild = GuildSys->Guild( trgChar->GetGuildNumber() );
							if( tGuild != NULL )
								tGuild->TellMembers( 1005, mGuild->Name().c_str() );
						}
					}
				}
				else
					s->sysmessage( 1006 );
			}
			break;
		case 3:	// declare ally
			trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
			if( ValidateObject( trgChar ) )
			{
				if( trgChar->GetGuildNumber() != mChar->GetGuildNumber() )
				{
					if( trgChar->GetGuildNumber() == -1 )
						s->sysmessage( 1004 );
					else
					{
						mGuild = GuildSys->Guild( mChar->GetGuildNumber() );
						if( mGuild != NULL )
						{
							mGuild->SetGuildRelation( trgChar->GetGuildNumber(), GR_ALLY );
							tGuild = GuildSys->Guild( trgChar->GetGuildNumber() );
							if( tGuild != NULL )
								tGuild->TellMembers( 1007, mGuild->Name().c_str() );
						}
					}
				}
				else
					s->sysmessage( 1006 );
			}
			break;
	}

}

void HandleSetScpTrig( CSocket *s )
{
	VALIDATESOCKET( s );
	UI16 targTrig		= (UI16)s->TempInt();
	cScript *toExecute	= JSMapping->GetScript( targTrig );
	if( targTrig && toExecute == NULL )
	{
		s->sysmessage( 1752 );
		return;
	}

	SERIAL ser = s->GetDWord( 7 );
	if( ser < BASEITEMSERIAL )
	{	// character
		CChar *targChar = calcCharObjFromSer( ser );
		if( ValidateObject( targChar ) )
		{
			targChar->SetScriptTrigger( targTrig );
			s->sysmessage( 1653 );
		}
	}
	else
	{	// item
		CItem *targetItem = calcItemObjFromSer( ser );
		if( ValidateObject( targetItem ) )
		{
			s->sysmessage( 1652 );
			targetItem->SetScriptTrigger( targTrig );
		}
	}
}

void BuildHouse( CSocket *s, UI08 houseEntry );
void BuildHouseTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	BuildHouse( s, s->AddID1() );//If its a valid house, send it to buildhouse!

	s->AddID1( 0 );
}

void AddScriptNpc( CSocket *s )
// Abaddon 17th February, 2000
// Need to return the character we've made, else summon creature at least will fail
// We make the char, but never pass it back up the chain
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;
	
	CChar *mChar			= s->CurrcharObj();
	const SI16 coreX		= s->GetWord( 11 );
	const SI16 coreY		= s->GetWord( 13 );
	const SI08 coreZ		= static_cast<SI08>(s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) ));
	CChar *cCreated			= Npcs->CreateNPCxyz( s->XText(), coreX, coreY, coreZ, mChar->WorldNumber() );
}

void TeleTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	const SERIAL serial = s->GetDWord( 7 );

	CBaseObject *mObj = NULL;
	if( serial >= BASEITEMSERIAL )
		mObj = calcItemObjFromSer( serial );
	else
		mObj = calcCharObjFromSer( serial );

	SI16 targX, targY;
	SI08 targZ;
	if( ValidateObject( mObj ) )
	{
		targX = mObj->GetX();
		targY = mObj->GetY();
		targZ = mObj->GetZ();
	}
	else
	{
		targX = s->GetWord( 11 );
		targY = s->GetWord( 13 );
		targZ = (SI08)(s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) ));
	}
	CChar *mChar = s->CurrcharObj();

	if( mChar->IsGM() || LineOfSight( s, mChar, targX, targY, targZ, WALLS_CHIMNEYS + DOORS + ROOFING_SLANTED, false ) )	
	{
		if( s->CurrentSpellType() != 2 )  // not a wand cast
		{
			Magic->SubtractMana( mChar, 3 );  // subtract mana on scroll or spell
			if( s->CurrentSpellType() == 0 )             // del regs on normal spell
			{
				reag_st toDel;
				toDel.drake = 1;
				toDel.moss = 1;
				Magic->DelReagents( mChar, toDel );
			}
		}
		
		Effects->PlaySound( s, 0x01FE, true );
		
		mChar->SetLocation( targX, targY, targZ );
		Effects->PlayStaticAnimation( mChar, 0x372A, 0x09, 0x06 );
	} 
}

void DyeTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i		= NULL;
	CChar *c		= NULL;
	SERIAL serial	= s->GetDWord( 7 );
	if( s->AddID1() == 0xFF && s->AddID2() == 0xFF )
	{
		CPDyeVat toSend;
		if( serial >= BASEITEMSERIAL )
		{
			i = calcItemObjFromSer( serial );
			if( ValidateObject( i ) )
			{
				toSend = (*i);
				s->Send( &toSend );
			}
		}
		else
		{
			c = calcCharObjFromSer( serial );
			if( ValidateObject( c ) )
			{
				toSend = (*c);
				toSend.Model( 0x2106 );
				s->Send( &toSend );
			}
		}
	}
	else
	{
		if( serial >= BASEITEMSERIAL )
		{
			i = calcItemObjFromSer( serial );
			if( !ValidateObject( i ) )
				return;
			UI16 colour = (UI16)(( (s->AddID1())<<8 ) + s->AddID2());
			if( !s->DyeAll() )
			{
				if( colour < 0x0002 || colour > 0x03E9 )
					colour = 0x03E9;
			}
			
			int b = ((colour&0x4000)>>14) + ((colour&0x8000)>>15);   
			if( !b )
				i->SetColour( colour );
		}
		else
		{
			c = calcCharObjFromSer( serial );
			if( !ValidateObject( c ) )
				return;
			UI16 body = c->GetID();
			UI16 k = (UI16)(( ( s->AddID1() )<<8 ) + s->AddID2());

			if( (k&0x4000) == 0x4000 && ( body >= 0x0190 && body <= 0x03E1 ) ) 
				k = 0xF000; // but assigning the only "transparent" value that works, namly semi-trasnparency.
			
			if( k != 0x8000 ) // 0x8000 also crashes client ...
			{	
				c->SetSkin( k );
				c->SetOrgSkin( k );
			}
		}
	}
}

void KeyTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;

	CChar *mChar		= s->CurrcharObj();
	SERIAL moreSerial	= s->AddID();
	if( i->GetTempVar( CITV_MORE ) == 0 )
	{       
		if( i->GetType() == IT_KEY && objInRange( mChar, i, DIST_NEARBY ) )
		{
			if( !Skills->CheckSkill( mChar, TINKERING, 400, 1000 ) ) 
			{
				s->sysmessage( 1016 );
				i->Delete();
				return;
			}
			i->SetTempVar( CITV_MORE, moreSerial );
			s->sysmessage( 1017 );
		}
	}
	else if( i->GetTempVar( CITV_MORE ) == moreSerial || s->AddID1() == 0xFF )
	{
		if( objInRange( mChar, i, DIST_NEARBY ) )
		{
			switch( i->GetType() )
			{
			case IT_CONTAINER:
			case IT_SPAWNCONT:
				if( i->GetType() == IT_CONTAINER ) 
					i->SetType( IT_LOCKEDCONTAINER );
				else if( i->GetType() == IT_SPAWNCONT ) 
					i->SetType( IT_LOCKEDSPAWNCONT );
				s->sysmessage( 1018 );
				break;
			case IT_KEY:
				mChar->SetSpeechItem( i );
				mChar->SetSpeechMode( 5 );
				s->sysmessage( 1019 );
				break;
			case IT_LOCKEDCONTAINER:
			case IT_LOCKEDSPAWNCONT:
				if( i->GetType() == IT_LOCKEDCONTAINER ) 
					i->SetType( IT_CONTAINER );
				if( i->GetType() == IT_LOCKEDSPAWNCONT ) 
					i->SetType( IT_SPAWNCONT );
				s->sysmessage( 1020 );
				break;
			case IT_DOOR:
				i->SetType( IT_LOCKEDDOOR );
				s->sysmessage( 1021 );
				Effects->PlaySound( s, 0x0049, true );
				break;
			case IT_LOCKEDDOOR:
				i->SetType( IT_DOOR );
				s->sysmessage( 1022 );
				Effects->PlaySound( s, 0x0049, true );
				break;
			case IT_HOUSESIGN:
				s->sysmessage( 1023 );
				mChar->SetSpeechMode( 8 );
				mChar->SetSpeechItem( i );
				break;
			case IT_PLANK:
				OpenPlank( i );
				s->sysmessage( "You open the plank" );
				break;
			}
		}
		else
			s->sysmessage( 393 );
	}
	else
	{
		if( i->GetType() == IT_KEY ) 
			s->sysmessage( 1024 );
		else if( i->GetTempVar( CITV_MORE, 1 ) == 0 ) 
			s->sysmessage( 1025 );
		else 
			s->sysmessage( 1026 );
	}
}

void WstatsTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;
	GumpDisplay wStat( s, 300, 300 );
	wStat.SetTitle( "Walking Stats" );
	SERIAL charSerial = i->GetSerial();
	UI16 charID = i->GetID();
	wStat.AddData( "Serial", charSerial, 3 );
	wStat.AddData( "Body ID", charID, 5 );
	wStat.AddData( "Name", i->GetName() );
	wStat.AddData( "X", i->GetX() );
	wStat.AddData( "Y", i->GetY() );
	char temp[15];
	sprintf( temp, "%d", i->GetZ() );
	wStat.AddData( "Z", temp );
	wStat.AddData( "Wander", i->GetNpcWander() );
	wStat.AddData( "FX1", i->GetFx( 0 ) );
	wStat.AddData( "FY1", i->GetFy( 0 ) );
	wStat.AddData( "FZ1", i->GetFz() );
	wStat.AddData( "FX2", i->GetFx( 1 ) );
	wStat.AddData( "FY2", i->GetFy( 1 ) );
	wStat.Send( 4, false, INVALIDSERIAL );
}

void ColorsTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;
	if( i->GetID() == 0x0FAB || i->GetID() == 0x0EFF || i->GetID() == 0x0E27 )	// dye vat, hair dye
	{
		CPDyeVat toSend = (*i);
		s->Send( &toSend );
	}
	else
		s->sysmessage( 1031 );
}

void DvatTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	SERIAL serial	= s->GetDWord( 7 );
	CItem *i		= calcItemObjFromSer( serial );
	if( !ValidateObject( i ) )
		return;

	CChar *mChar = s->CurrcharObj();
	if( i->isDyeable() )
	{
		if( i->GetCont() != NULL )
		{
			CChar *c = FindItemOwner( i );
			if( ValidateObject( c ) && c != mChar )
			{
				s->sysmessage( 1032 );
				return;
			}
		}
		i->SetColour( ( ( s->AddID1() )<<8) + s->AddID2() );
		Effects->PlaySound( s, 0x023E, true );
	}
	else
		s->sysmessage( 1033 );
}

void InfoTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	if( !s->GetByte( 1 ) && s->GetDWord( 7 ) < BASEITEMSERIAL )
	{
		s->sysmessage( "This command can not be used on characters." );
		return;
	}

	const SI16 x		= s->GetWord( 11 );
	const SI16 y		= s->GetWord( 13 );
	const UI16 tileID	= s->GetWord( 17 );

	if( tileID == 0 )
	{
		UI08 worldNumber = 0;
		CChar *mChar = s->CurrcharObj();
		if( ValidateObject( mChar ) )
			worldNumber = mChar->WorldNumber();

		// manually calculating the ID's if it's a maptype
		const map_st map1 = Map->SeekMap( x, y, worldNumber );
		GumpDisplay mapStat( s, 300, 300 );
		mapStat.SetTitle( "Map Tile" );
		mapStat.AddData( "Tilenum", map1.id );
		if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
		{
			//7.0.9.0 tiledata and later
			CLandHS& land = Map->SeekLandHS( map1.id );
			mapStat.AddData( "Flags", land.FlagsNum(), 1 );
			mapStat.AddData( "Name", land.Name() );
		}
		else
		{
			//7.0.8.2 tiledata and earlier
			CLand& land = Map->SeekLand( map1.id );
			mapStat.AddData( "Flags", land.FlagsNum(), 1 );
			mapStat.AddData( "Name", land.Name() );
		}		
		mapStat.Send( 4, false, INVALIDSERIAL );
	} 
	else
	{
		if( cwmWorldState->ServerData()->ServerUsingHSTiles() )
		{
			//7.0.9.0 data and later
			CTileHS& tile = Map->SeekTileHS( tileID );

			GumpDisplay statTile( s, 300, 300 );
			statTile.SetTitle( "Map Tile" );

			statTile.AddData( "Tilenum", tileID, 1 );
			statTile.AddData( "Weight", tile.Weight(), 0 );
			statTile.AddData( "Layer", tile.Layer(), 1 );
			statTile.AddData( "Hue", tile.Hue(), 1 );
			statTile.AddData( "Anim", tile.Animation(), 1 );
			statTile.AddData( "Quantity", tile.Quantity(), 1 );
			statTile.AddData( "Unknown1", tile.Unknown1(), 1 );
			statTile.AddData( "Unknown2", tile.Unknown2(), 1 );
			statTile.AddData( "Unknown3", tile.Unknown3(), 1 );
			statTile.AddData( "Unknown4", tile.Unknown4(), 1 );
			statTile.AddData( "Unknown5", tile.Unknown5(), 1 );
			statTile.AddData( "Height", tile.Height(), 0 );
			statTile.AddData( "Name", tile.Name() );
			statTile.AddData( "Flags:", tile.FlagsNum(), 1 );
			statTile.AddData( "--> FloorLevel", tile.CheckFlag( TF_FLOORLEVEL ) );
			statTile.AddData( "--> Holdable", tile.CheckFlag( TF_HOLDABLE ) );
			statTile.AddData( "--> Transparent", tile.CheckFlag( TF_TRANSPARENT ) );
			statTile.AddData( "--> Translucent", tile.CheckFlag( TF_TRANSLUCENT ) );
			statTile.AddData( "--> Wall", tile.CheckFlag( TF_WALL ) );
			statTile.AddData( "--> Damaging", tile.CheckFlag( TF_DAMAGING ) );
			statTile.AddData( "--> Blocking", tile.CheckFlag( TF_BLOCKING ) );
			statTile.AddData( "--> Wet", tile.CheckFlag( TF_WET ) );
			statTile.AddData( "--> Unknown1", tile.CheckFlag( TF_UNKNOWN1 ) );
			statTile.AddData( "--> Surface", tile.CheckFlag( TF_SURFACE ) );
			statTile.AddData( "--> Climbable", tile.CheckFlag( TF_CLIMBABLE ) );
			statTile.AddData( "--> Stackable", tile.CheckFlag( TF_STACKABLE ) );
			statTile.AddData( "--> Window", tile.CheckFlag( TF_WINDOW ) );
			statTile.AddData( "--> NoShoot", tile.CheckFlag( TF_NOSHOOT ) );
			statTile.AddData( "--> DisplayA", tile.CheckFlag( TF_DISPLAYA ) );
			statTile.AddData( "--> DisplayAn", tile.CheckFlag( TF_DISPLAYAN ) );
			statTile.AddData( "--> Description", tile.CheckFlag( TF_DESCRIPTION ) );
			statTile.AddData( "--> Foilage", tile.CheckFlag( TF_FOLIAGE ) );
			statTile.AddData( "--> PartialHue", tile.CheckFlag( TF_PARTIALHUE ) );
			statTile.AddData( "--> Unknown2", tile.CheckFlag( TF_UNKNOWN2 ) );
			statTile.AddData( "--> Map", tile.CheckFlag( TF_MAP ) );
			statTile.AddData( "--> Container", tile.CheckFlag( TF_CONTAINER ) );
			statTile.AddData( "--> Wearable", tile.CheckFlag( TF_WEARABLE ) );
			statTile.AddData( "--> Light", tile.CheckFlag( TF_LIGHT ) );
			statTile.AddData( "--> Animated", tile.CheckFlag( TF_ANIMATED ) );
			statTile.AddData( "--> NoDiagonal", tile.CheckFlag( TF_NODIAGONAL ) ); //HOVEROVER in SA clients and later, to determine if tiles can be moved on by flying gargoyle
			statTile.AddData( "--> Unknown3", tile.CheckFlag( TF_UNKNOWN3 ) );
			statTile.AddData( "--> Armor", tile.CheckFlag( TF_ARMOR ) );
			statTile.AddData( "--> Roof", tile.CheckFlag( TF_ROOF ) );
			statTile.AddData( "--> Door", tile.CheckFlag( TF_DOOR ) );
			statTile.AddData( "--> StairBack", tile.CheckFlag( TF_STAIRBACK ) );
			statTile.AddData( "--> StairRight", tile.CheckFlag( TF_STAIRRIGHT ) );
			statTile.Send( 4, false, INVALIDSERIAL );
		}
		else
		{
			//7.0.8.2 data and earlier
			CTile& tile = Map->SeekTile( tileID );

			GumpDisplay statTile( s, 300, 300 );
			statTile.SetTitle( "Map Tile" );

			statTile.AddData( "Tilenum", tileID, 1 );
			statTile.AddData( "Weight", tile.Weight(), 0 );
			statTile.AddData( "Layer", tile.Layer(), 1 );
			statTile.AddData( "Hue", tile.Hue(), 1 );
			statTile.AddData( "Anim", tile.Animation(), 1 );
			statTile.AddData( "Quantity", tile.Quantity(), 1 );
			statTile.AddData( "Unknown1", tile.Unknown1(), 1 );
			statTile.AddData( "Unknown2", tile.Unknown2(), 1 );
			statTile.AddData( "Unknown3", tile.Unknown3(), 1 );
			statTile.AddData( "Unknown4", tile.Unknown4(), 1 );
			statTile.AddData( "Unknown5", tile.Unknown5(), 1 );
			statTile.AddData( "Height", tile.Height(), 0 );
			statTile.AddData( "Name", tile.Name() );
			statTile.AddData( "Flags:", tile.FlagsNum(), 1 );
			statTile.AddData( "--> FloorLevel", tile.CheckFlag( TF_FLOORLEVEL ) );
			statTile.AddData( "--> Holdable", tile.CheckFlag( TF_HOLDABLE ) );
			statTile.AddData( "--> Transparent", tile.CheckFlag( TF_TRANSPARENT ) );
			statTile.AddData( "--> Translucent", tile.CheckFlag( TF_TRANSLUCENT ) );
			statTile.AddData( "--> Wall", tile.CheckFlag( TF_WALL ) );
			statTile.AddData( "--> Damaging", tile.CheckFlag( TF_DAMAGING ) );
			statTile.AddData( "--> Blocking", tile.CheckFlag( TF_BLOCKING ) );
			statTile.AddData( "--> Wet", tile.CheckFlag( TF_WET ) );
			statTile.AddData( "--> Unknown1", tile.CheckFlag( TF_UNKNOWN1 ) );
			statTile.AddData( "--> Surface", tile.CheckFlag( TF_SURFACE ) );
			statTile.AddData( "--> Climbable", tile.CheckFlag( TF_CLIMBABLE ) );
			statTile.AddData( "--> Stackable", tile.CheckFlag( TF_STACKABLE ) );
			statTile.AddData( "--> Window", tile.CheckFlag( TF_WINDOW ) );
			statTile.AddData( "--> NoShoot", tile.CheckFlag( TF_NOSHOOT ) );
			statTile.AddData( "--> DisplayA", tile.CheckFlag( TF_DISPLAYA ) );
			statTile.AddData( "--> DisplayAn", tile.CheckFlag( TF_DISPLAYAN ) );
			statTile.AddData( "--> Description", tile.CheckFlag( TF_DESCRIPTION ) );
			statTile.AddData( "--> Foilage", tile.CheckFlag( TF_FOLIAGE ) );
			statTile.AddData( "--> PartialHue", tile.CheckFlag( TF_PARTIALHUE ) );
			statTile.AddData( "--> Unknown2", tile.CheckFlag( TF_UNKNOWN2 ) );
			statTile.AddData( "--> Map", tile.CheckFlag( TF_MAP ) );
			statTile.AddData( "--> Container", tile.CheckFlag( TF_CONTAINER ) );
			statTile.AddData( "--> Wearable", tile.CheckFlag( TF_WEARABLE ) );
			statTile.AddData( "--> Light", tile.CheckFlag( TF_LIGHT ) );
			statTile.AddData( "--> Animated", tile.CheckFlag( TF_ANIMATED ) );
			statTile.AddData( "--> NoDiagonal", tile.CheckFlag( TF_NODIAGONAL ) ); //HOVEROVER in SA clients and later, to determine if tiles can be moved on by flying gargoyle
			statTile.AddData( "--> Unknown3", tile.CheckFlag( TF_UNKNOWN3 ) );
			statTile.AddData( "--> Armor", tile.CheckFlag( TF_ARMOR ) );
			statTile.AddData( "--> Roof", tile.CheckFlag( TF_ROOF ) );
			statTile.AddData( "--> Door", tile.CheckFlag( TF_DOOR ) );
			statTile.AddData( "--> StairBack", tile.CheckFlag( TF_STAIRBACK ) );
			statTile.AddData( "--> StairRight", tile.CheckFlag( TF_STAIRRIGHT ) );
			statTile.Send( 4, false, INVALIDSERIAL );
		}
	}
}

void TweakTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	SERIAL serial	= s->GetDWord( 7 );
	CChar *c		= calcCharObjFromSer( serial );
	if( ValidateObject( c ) )
		tweakCharMenu( s, c );
	else 
	{
		CItem *i = calcItemObjFromSer( serial );
		if( ValidateObject( i ) )
			tweakItemMenu( s, i );
	}
}

void LoadCannon( CSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;
	SERIAL moreSerial = s->AddID();
	if( i->GetTempVar( CITV_MORE ) == moreSerial || s->AddID1() == 0xFF )
	{
		if( i->GetTempVar( CITV_MOREZ ) == 0 && objInRange( s, i, DIST_NEARBY ) )
		{
			i->SetTempVar( CITV_MOREZ, 1 );
			s->sysmessage( 1035 );
		}
		else
		{
			if( i->GetTempVar( CITV_MORE, 1 ) == 0x00 ) 
				s->sysmessage( 1036 );
			else 
				s->sysmessage( 1037 );
		}
	}
}

void Tiling( CSocket *s )  // Clicking the corners of tiling calls this function - Crwth 01/11/1999
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	if( s->ClickX() == -1 && s->ClickY() == -1 )
	{
		s->ClickX( s->GetWord( 11 ) );
		s->ClickY( s->GetWord( 13 ) );
		s->target( 0, TARGET_TILING, 1038 );
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
	
	UI16 addid = (UI16)(( ( s->AddID1() ) << 8 ) + s->AddID2());

	CItem *c = NULL;
	for( SI16 x = x1; x <= x2; ++x )
	{
		for( SI16 y = y1; y <= y2; ++y ) 
		{
			c = Items->CreateItem( NULL, s->CurrcharObj(), addid, 1, 0, OT_ITEM );
			if( !ValidateObject( c ) ) 
				return;
			c->SetDecayable( false );
			c->SetLocation( x, y, s->GetByte( 16 ) + Map->TileHeight( s->GetWord( 17 ) ) );
		}
	}
	s->AddID1( 0 );
	s->AddID2( 0 );
}

//o--------------------------------------------------------------------------o
//|	Function/Class	-	void newCarveTarget( CSocket *s, CItem *i )
//|	Date			-	09/22/2002
//|	Developer(s)	-	Unknown
//|	Company/Team	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Target carving system.
//|									
//|	Modification	-	unknown   	-	Human-corpse carving code added
//|									
//|	Modification	-	unknown   	-	Scriptable carving product added
//|									
//|	Modification	-	09/22/2002	-	Xuri - Fixed erroneous names for body parts 
//|									& made all body parts that are carved from human corpse	
//|									lie in same direction.
//o--------------------------------------------------------------------------o
bool CreateBodyPart( CChar *mChar, CItem *corpse, UI16 partID, SI32 dictEntry )
{
	CItem *toCreate = Items->CreateItem( NULL, mChar, partID, 1, 0, OT_ITEM );
	if( !ValidateObject( toCreate ) ) 
		return false;
	toCreate->SetName( UString::sprintf( Dictionary->GetEntry( dictEntry ).c_str(), corpse->GetName2() ) );
	toCreate->SetLocation( corpse );
	toCreate->SetOwner( corpse->GetOwnerObj() );
	toCreate->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAY ) );
	return true;
}
void newCarveTarget( CSocket *s, CItem *i )
{
	VALIDATESOCKET( s );

	CChar *mChar = s->CurrcharObj();
	CItem *c = Items->CreateItem( NULL, mChar, 0x122A, 1, 0, OT_ITEM ); // add the blood puddle
	if( c == NULL ) 
		return;
	c->SetLocation( i );
	c->SetMovable( 2 );
	c->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAY ) );

	// if it's a human corpse
	// Sept 22, 2002 - Xuri - Corrected the alignment of body parts that are carved.
	if( i->GetTempVar( CITV_MOREY, 2 ) )
	{
		ScriptSection *toFind	= FileLookup->FindEntry( "CARVE HUMAN", carve_def );
		if( toFind == NULL )
			return;
		UString tag;
		UString data;
		for( tag = toFind->First(); !toFind->AtEnd(); tag = toFind->Next() )
		{
			if( tag.upper() == "ADDITEM" )
			{
				data = toFind->GrabData();
				if( data.sectionCount( "," ) != 0 )
					if( !CreateBodyPart( mChar, i, data.section( ",", 0, 0 ).stripWhiteSpace().toUShort(), data.section( ",", 1, 1 ).stripWhiteSpace().toLong() ) )
						return;
			}
		}

		criminal( mChar );

		CDataList< CItem * > *iCont = i->GetContainsList();
		for( c = iCont->First(); !iCont->Finished(); c = iCont->Next() )
		{
			if( ValidateObject( c ) )
			{
				if( c->GetLayer() != IL_HAIR && c->GetLayer() != IL_FACIALHAIR )
				{
					c->SetCont( NULL );
					c->SetLocation( i );
					c->SetDecayTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_DECAY ) );
				}
			}
		}
		i->Delete();
	} 
	else
	{
		UString sect			= "CARVE " + UString::number( i->GetCarve() );
		ScriptSection *toFind	= FileLookup->FindEntry( sect, carve_def );
		if( toFind == NULL )
			return;
		UString tag;
		UString data;
		for( tag = toFind->First(); !toFind->AtEnd(); tag = toFind->Next() )
		{
			if( tag.upper() == "ADDITEM" )
			{
				data = toFind->GrabData();
				if( data.sectionCount( "," ) != 0 )
					Items->CreateScriptItem( s, mChar, data.section( ",", 0, 0 ).stripWhiteSpace(), data.section( ",", 1, 1 ).stripWhiteSpace().toUShort(), OT_ITEM, true );
				else
					Items->CreateScriptItem( s, mChar, data, 0, OT_ITEM, true );
			}
		}
	}
}

void CorpseTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( i ) || !ValidateObject( mChar ) )
		return;
	if( objInRange( mChar, i, DIST_NEARBY ) ) 
	{
		Effects->PlayCharacterAnimation( mChar, 0x20 );
		if( i->GetTempVar( CITV_MOREY, 1 ) == 0 )
		{
			i->SetTempVar( CITV_MOREY, 1, 1 );
			if( i->GetTempVar( CITV_MOREY, 2 ) || i->GetCarve() != -1 )
				newCarveTarget( s, i );
		} 
		else 
			s->sysmessage( 1051 );
	}
	else
		s->sysmessage( 393 );
}

void AttackTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *target	= static_cast<CChar *>(s->TempObj());
	CChar *target2	= calcCharObjFromSer( s->GetDWord( 7 ) );
	s->TempObj( NULL );
	
	if( !ValidateObject( target2 ) || !ValidateObject( target ) ) 
		return;
	if( target2 == target )
	{
		s->sysmessage( 1073 );
		return;
	}

	// Check if combat is allowed in attacker's AND target's regions
	if( target->GetRegion()->IsSafeZone() || target2->GetRegion()->IsSafeZone() )
	{
		// Target is in a safe zone where all aggressive actions are forbidden, disallow
		s->sysmessage( 1799 );
		return;
	}

	Combat->AttackTarget( target, target2 );
	if( target2->IsInnocent() && target2 != target->GetOwnerObj() )
	{
		CChar *pOwner = target->GetOwnerObj();
		if( ValidateObject( pOwner ) && WillResultInCriminal( pOwner, target2 ) )
			criminal( pOwner );
	}
}

void FollowTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *char1	= static_cast<CChar *>(s->TempObj());
	CChar *char2	= calcCharObjFromSer( s->GetDWord( 7 ) );
	s->TempObj( NULL );
	if( !ValidateObject( char1 ) || !ValidateObject( char2 ) )
		return;
	
	char1->SetFTarg( char2 );
	char1->SetNpcWander( WT_FOLLOW );
}

void TransferTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *char1 = static_cast<CChar *>(s->TempObj());
	CChar *char2 = calcCharObjFromSer( s->GetDWord( 7 ) );
	s->TempObj( NULL );
	
	if( !ValidateObject( char1 ) )
		return;

	if( !ValidateObject( char2 ) )
	{
		s->sysmessage( 1066 );
		return;
	}
	if( char1 == char2 )
	{
		s->sysmessage( 1066 );
		return;
	}

	Npcs->stopPetGuarding( char1 );

	char1->TextMessage( NULL, 1074, TALK, false, char1->GetName().c_str(), char2->GetName().c_str() );
	
	char1->SetOwner( char2 );
	char1->SetFTarg( NULL );
	char1->SetNpcWander( WT_FREE );
}

bool BuyShop( CSocket *s, CChar *c )
{
	if( s == NULL )
		return false;
	if( !ValidateObject( c ) )
		return false;

	//Check if vendor has onBuy script running
	UI16 charTrig		= c->GetScriptTrigger();
	cScript *toExecute	= JSMapping->GetScript( charTrig );
	if( toExecute != NULL )
	{
		if( toExecute->OnBuy( s, c ) )
			return false;
	}

	CItem *sellPack		= c->GetItemAtLayer( IL_SELLCONTAINER );
	CItem *boughtPack	= c->GetItemAtLayer( IL_BOUGHTCONTAINER );
	
	if( !ValidateObject( sellPack ) || !ValidateObject( boughtPack ) )
		return false;

	CPItemsInContainer iic;
	if( s->ClientVerShort() >= CVS_6017 )
		iic.UOKRFlag( true );
	iic.Type( 0x02 );
	iic.VendorSerial( sellPack->GetSerial() );
	CPOpenBuyWindow obw( sellPack, c, iic, s );

	CPItemsInContainer iic2;
	if( s->ClientVerShort() >= CVS_6017 )
		iic2.UOKRFlag( true );
	iic2.Type( 0x02 );
	iic2.VendorSerial( boughtPack->GetSerial() );
	CPOpenBuyWindow obw2( boughtPack, c, iic2, s );

	CPDrawContainer toSend;
	toSend.Model( 0x0030 );
	toSend.Serial( c->GetSerial() );
	if( s->ClientType() >= CV_HS2D && s->ClientVerShort() >= CVS_7090 )
		toSend.ContType( 0x00 );

	s->Send( &iic );
	s->Send( &iic2 );
	s->Send( &obw );
	s->Send( &obw2 );
	s->Send( &toSend );

	s->statwindow( s->CurrcharObj() ); // Make sure the gold total has been sent.
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function		-  void npcresurrecttarget( CChar *i )
//|   Date			-  UnKnown
//|   Programmer	-  UnKnown  (Touched tabstops by Tauriel Dec 28, 1998)
//|									
//|	Modification	-	09/22/2002	-	Xuri - Made players not appear with full 
//|									health/stamina after being resurrected by NPC Healer
//o---------------------------------------------------------------------------o
//|   Purpose     -  Resurrects a character
//o---------------------------------------------------------------------------o
void NpcResurrectTarget( CChar *i )
{
	if( !ValidateObject( i ) )
		return;

	if( i->IsNpc() )
	{
		Console.Error( Dictionary->GetEntry( 1079 ).c_str(), i );
		return;
	}
	CSocket *mSock = i->GetSocket();
	// the char is a PC, but not logged in.....
	if( mSock != NULL )
	{
		if( i->IsDead() )
		{
			UI16 charTrig		= i->GetScriptTrigger();
			cScript *toExecute	= JSMapping->GetScript( charTrig );
			if( toExecute != NULL )
			{
				if( toExecute->OnResurrect( i ) == 1 )	// if it exists and we don't want hard code, return
					return;
			}

			Fame( i, 0 );
			Effects->PlaySound( i, 0x0214 );
			i->SetID( i->GetOrgID() );
			i->SetSkin( i->GetOrgSkin() );
			i->SetDead( false );
			// Sept 22, 2002 - Xuri
			i->SetHP( i->GetMaxHP() / 10 );
			i->SetStamina( i->GetMaxStam() / 10 );
			//
			i->SetMana( i->GetMaxMana() / 10 );
			i->SetAttacker( NULL );
			i->SetAttackFirst( false );
			i->SetWar( false );
			i->SetHunger( 6 );
			CItem *c = NULL;
			for( CItem *j = i->FirstItem(); !i->FinishedItems(); j = i->NextItem() )
			{
				if( ValidateObject( j ) && !j->isFree() )
				{
					if( j->GetLayer() == IL_BUYCONTAINER )
					{
						j->SetLayer( IL_PACKITEM );	
						i->SetPackItem( j );
					}
					if( j->GetSerial() == i->GetRobe() )
					{
						j->Delete();
						
						c = Items->CreateScriptItem( NULL, i, "resurrection_robe", 1, OT_ITEM );
						if( c != NULL )
							c->SetCont( i );
					}
				}
			}
		}
		else
			mSock->sysmessage( 1080 );
	}
	else
		Console.Warning( "Attempt made to resurrect a PC (serial: 0x%X) that's not logged in", i->GetSerial() );
}


void killKeys( SERIAL targSerial );
void HouseOwnerTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	SERIAL o_serial = s->GetDWord( 7 );
	if( o_serial == INVALIDSERIAL ) 
		return;

	CChar *own = calcCharObjFromSer( o_serial );
	if( !ValidateObject( own ) )
		return;

	CSocket *oSock = own->GetSocket();
	if( oSock == NULL )
		return;

	CItem *sign = static_cast<CItem *>(s->TempObj());
	s->TempObj( NULL );
	if( !ValidateObject( sign ) )
		return;

	CItem *house = calcItemObjFromSer( sign->GetTempVar( CITV_MORE ) );;
	if( !ValidateObject( house ) )
		return;

	sign->SetOwner( own );
	house->SetOwner( own );

	killKeys( house->GetSerial() );

	CItem *key = Items->CreateScriptItem( oSock, own, "0x100F", 1, OT_ITEM, true );	// gold key for everything else
	if( key == NULL )
		return;
	key->SetName( "a house key" );
	key->SetTempVar( CITV_MORE, house->GetSerial() );
	key->SetType( IT_KEY );

	s->sysmessage( 1081, own->GetName().c_str() );
	oSock->sysmessage( 1082, mChar->GetName().c_str() );
}

void HouseEjectTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *c		= calcCharObjFromSer( s->GetDWord( 7 ) );
	CMultiObj *h	= static_cast<CMultiObj *>(s->TempObj());
	s->TempObj( NULL );
	if( ValidateObject( c ) && ValidateObject( h ) ) 
	{
		SI16 x1, y1, x2, y2;
		Map->MultiArea( h, x1, y1, x2, y2 );
		if( c->GetX() >= x1 && c->GetY() >= y1 && c->GetX() <= x2 && c->GetY() <= y2 )
		{
			c->SetLocation( x2, (y2+1), c->GetZ() );
			s->sysmessage( 1083 );
		}
		else 
			s->sysmessage( 1084 );
	}
}

UI08 AddToHouse( CMultiObj *house, CChar *toAdd, UI08 mode = 0 );
void HouseBanTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *c		= calcCharObjFromSer( s->GetDWord( 7 ) );
	CMultiObj *h	= static_cast<CMultiObj *>(s->TempObj());
	s->TempObj( NULL );
	if( ValidateObject( c ) && ValidateObject( h ) ) 
	{
		UI08 r = AddToHouse( h, c, 1 );
		if( r == 1 ) 
			s->sysmessage( 1085, c->GetName().c_str() );
		else if( r == 2 ) 
			s->sysmessage( 1086 );
		else 
			s->sysmessage( 1087 );
	}
}

void HouseFriendTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *c		= calcCharObjFromSer( s->GetDWord( 7 ) );
	CMultiObj *h	= static_cast<CMultiObj *>(s->TempObj());
	s->TempObj( NULL );
	if( ValidateObject( c ) && ValidateObject( h ) ) 
	{
		UI08 r = AddToHouse( h, c );
		if( r == 1 ) 
		{
			CSocket *cSock = c->GetSocket();
			if( cSock != NULL )
				cSock->sysmessage( 1089 );
			s->sysmessage( 1088, c->GetName().c_str() );
		} 
		else if( r == 2 ) 
			s->sysmessage( 1090 );
		else 
			s->sysmessage( 1091 );
	}
}

bool DeleteFromHouseList( CMultiObj *house, CChar *toDelete, UI08 mode );
void HouseUnlistTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *c		= calcCharObjFromSer( s->GetDWord( 7 ) );
	CMultiObj *h	=  static_cast<CMultiObj *>(s->TempObj());
	s->TempObj( NULL );
	if( ValidateObject( c ) && ValidateObject( h ) ) 
	{
		bool r = DeleteFromHouseList( h, c, static_cast<UI08>(s->TempInt()) );
		if( r )
			s->sysmessage( 1092, c->GetName().c_str() );
		else 
			s->sysmessage( 1093 );
	}
}

void ShowSkillTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( mChar ) )
	{
		s->sysmessage( 1103 );
		return;
	}

	SI32 dispType = s->TempInt();
	UI16 skillVal;

	GumpDisplay showSkills( s, 300, 300 );
	showSkills.SetTitle( "Skills Info" );
	for( UI08 i = 0; i < ALLSKILLS; ++i )
	{
		if( dispType == 0 || dispType == 1 )
			skillVal = mChar->GetBaseSkill( i );
		else
			skillVal = mChar->GetSkill( i );

		if( skillVal > 0 || dispType%2 == 0 )
			showSkills.AddData( cwmWorldState->skill[i].name, UString::number( (float)skillVal/10 ), 8 );
	}
	showSkills.Send( 4, false, INVALIDSERIAL );
}

void FriendTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	CChar *targChar = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( targChar ) )
	{
		s->sysmessage( 1103 );
		return;
	}
	if( targChar->IsNpc() || !isOnline( (*targChar) ) || targChar == mChar )
	{
		s->sysmessage( 1622 );
		return;
	}

	CChar *pet = static_cast<CChar *>(s->TempObj());
	s->TempObj( NULL );
	if( Npcs->checkPetFriend( targChar, pet ) )
	{
		s->sysmessage( 1621 );
		return;
	}

	CHARLIST *petFriends = pet->GetFriendList();
	// Make sure to cover the STL response
	if( petFriends != NULL )
	{
		if( petFriends->size() >= 10 )
		{
			s->sysmessage( 1623 );
			return;
		}
	}

	pet->AddFriend( targChar );
	s->sysmessage( 1624, pet->GetName().c_str(), targChar->GetName().c_str() );

	CSocket *targSock = targChar->GetSocket();
	if( targSock != NULL )
		targSock->sysmessage( 1625, mChar->GetName().c_str(), pet->GetName().c_str() );
}

void GuardTarget( CSocket *s )
//PRE:	Pet has been commanded to guard
//POST: Pet guards person, if owner currently
//DEV:	Abaddon
//DATE: 3rd October
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	CChar *petGuarding = static_cast<CChar *>(s->TempObj());
	s->TempObj( NULL );
	if( !ValidateObject( petGuarding ) )
		return;

	Npcs->stopPetGuarding( petGuarding );

	CChar *charToGuard = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( charToGuard ) )
	{
		if( charToGuard != petGuarding->GetOwnerObj() && !Npcs->checkPetFriend( charToGuard, petGuarding ) )
		{
			s->sysmessage( 1628 );
			return;
		}
		petGuarding->SetNPCAiType( AI_PET_GUARD ); // 32 is guard mode
		petGuarding->SetGuarding( charToGuard );
		mChar->SetGuarded( true );
		return;
	}
	CItem *itemToGuard = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( itemToGuard ) && !itemToGuard->isPileable() )
	{
		CMultiObj *multi = itemToGuard->GetMultiObj();
		if( ValidateObject( multi ) )
		{
			if( multi->GetOwnerObj() == mChar )
			{
				petGuarding->SetNPCAiType( AI_PET_GUARD );
				petGuarding->SetGuarding( itemToGuard );
				itemToGuard->SetGuarded( true );
			}
		}
		else
			s->sysmessage( 1628 );
	}
}

void HouseLockdown( CSocket *s ) // Abaddon
// PRE:		S is the socket of a valid owner/coowner and is in a valid house
// POST:	either locks down the item, or puts a message to the owner saying he's a moron
// CODER:	Abaddon
// DATE:	17th December, 1999
{
	VALIDATESOCKET( s );
	CItem *itemToLock = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( itemToLock ) )
	{
		if( !itemToLock->CanBeLockedDown() )
		{
			s->sysmessage( 1106 );
			return;
		}
		CMultiObj *house =  static_cast<CMultiObj *>(s->TempObj());
		s->TempObj( NULL );
		// time to lock it down!
		CMultiObj *multi = findMulti( itemToLock );
		if( ValidateObject( multi ) )
		{
			if( multi != house )
			{
				s->sysmessage( 1105 );
				return;
			}
			if( multi->GetLockDownCount() < multi->GetMaxLockDowns() )
			{
				multi->LockDownItem( itemToLock );
				UI16 lockDownsLeft = multi->GetMaxLockDowns() - multi->GetLockDownCount();
				s->sysmessage( 1786 ); //You lock down the targeted item
				s->sysmessage( 1788, lockDownsLeft ); //%i lockdowns remaining
			}
			else
				s->sysmessage( "You have too many locked down items" );
			return;
		}
		// not in a multi!
		s->sysmessage( 1107 );
	}
	else
		s->sysmessage( 1108 );
}

void HouseRelease( CSocket *s ) // Abaddon
// PRE:		S is the socket of a valid owner/coowner and is in a valid house, the item is locked down
// POST:	either releases the item from lockdown, or puts a message to the owner saying he's a moron
// CODER:	Abaddon
// DATE:	17th December, 1999
{
	VALIDATESOCKET( s );
	CItem *itemToLock = calcItemObjFromSer( s->GetDWord( 7 ) );

	if( ValidateObject( itemToLock ) ) // || !itemToLock->IsLockedDown() )
	{
		if( itemToLock->IsLockedDown() )
		{
			CMultiObj *house =  static_cast<CMultiObj *>(s->TempObj());	// let's find our house
			s->TempObj( NULL );
			// time to release it!
			CMultiObj *multi = findMulti( itemToLock );
			if( ValidateObject( multi ) )
			{
				if( multi != house )
				{
					s->sysmessage( 1109 );
					return;
				}
				if( multi->GetLockDownCount() > 0 )
				{
					multi->RemoveLockDown( itemToLock );	// Default as stored by the client, perhaps we should keep a backup?
					UI16 lockDownsLeft = multi->GetMaxLockDowns() - multi->GetLockDownCount();
					s->sysmessage( 1787 ); //You lock down the targeted item
					s->sysmessage( 1788, lockDownsLeft ); //%i lockdowns remaining
				}
				return;
			}
			// not in a multi!
			s->sysmessage( 1107 );
		}
		else
			s->sysmessage( 1785 );
	}
	else
		s->sysmessage( 1108 );
}

void MakeTownAlly( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	CChar *targetChar = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( targetChar ) )
	{
		s->sysmessage( 1110 );
		return;
	}
	UI16 srcTown = mChar->GetTown();
	UI16 trgTown = targetChar->GetTown();

	if( !cwmWorldState->townRegions[srcTown]->MakeAlliedTown( trgTown ) )	
		s->sysmessage( 1111 );
}

void MakeStatusTarget( CSocket *sock )
{
	VALIDATESOCKET( sock );
	CChar *targetChar = calcCharObjFromSer( sock->GetDWord( 7 ) );
	if( !ValidateObject( targetChar ) )
	{
		sock->sysmessage( 1110 );
		return;
	}
	UI08 origCommand			= targetChar->GetCommandLevel();
	commandLevel_st *targLevel	= Commands->GetClearance( sock->XText() );
	commandLevel_st *origLevel	= Commands->GetClearance( origCommand );
	
	if( targLevel == NULL )
	{
		sock->sysmessage( 1112 );
		return;
	}
	CChar *mChar = sock->CurrcharObj();
	char temp[1024], temp2[1024];

	UI08 targetCommand = targLevel->commandLevel;
	sprintf( temp, "account%i.log", mChar->GetAccount().wAccountIndex );
	sprintf( temp2, "%s has made %s a %s.\n", mChar->GetName().c_str(), targetChar->GetName().c_str(), targLevel->name.c_str() );
	Console.Log( temp2, temp );

	DismountCreature( targetChar );

	if( targLevel->targBody != 0 )
	{
		targetChar->SetID( targLevel->targBody );
		targetChar->SetOrgID( targLevel->targBody );
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

	UString playerName = targetChar->GetName();
	if( targetCommand != origCommand && origLevel != NULL )
	{
		const size_t position = playerName.find( origLevel->name );
		if( position != std::string::npos )
			playerName.replace( position, origLevel->name.size(), "" );
	}
	if( targetCommand != 0 && targetCommand != origCommand )
		targetChar->SetName( UString::sprintf( "%s %s", targLevel->name.c_str(), playerName.stripWhiteSpace().c_str() ).stripWhiteSpace() );
	else if( origCommand != 0 )
		targetChar->SetName( playerName.stripWhiteSpace() );

	CItem *mypack	= targetChar->GetPackItem();

	if( targLevel->stripOff.any() )
	{
		for( CItem *z = targetChar->FirstItem(); !targetChar->FinishedItems(); z = targetChar->NextItem() )
		{
			if( ValidateObject( z ) )
			{
				switch( z->GetLayer() )
				{
					case IL_HAIR:
					case IL_FACIALHAIR:
						if( targLevel->stripOff.test( BIT_STRIPHAIR ) )
							z->Delete();
						break;
					case IL_NONE:
					case IL_MOUNT:
					case IL_PACKITEM:
					case IL_BANKBOX:
						break;
					default:
						if( targLevel->stripOff.test( BIT_STRIPITEMS ) )
						{
							if( !ValidateObject( mypack ) )
								mypack = targetChar->GetPackItem();
							if( !ValidateObject( mypack ) )
							{
								CItem *iMade = Items->CreateItem( NULL, targetChar, 0x0E75, 1, 0, OT_ITEM );
								if( !ValidateObject( iMade ) ) 
									return;
								targetChar->SetPackItem( iMade );
								iMade->SetDecayable( false );
								iMade->SetLayer( IL_PACKITEM );
								if( iMade->SetCont( targetChar ) )
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

void SmeltTarget( CSocket *s )
{
	VALIDATESOCKET( s );

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) || i->GetCont() == NULL )
		return;
	if( i->GetCreator() == INVALIDSERIAL )
	{
		s->sysmessage( 1113 );
		return;
	}
	UI16 iMadeFrom = i->EntryMadeFrom();

	createEntry *ourCreateEntry = Skills->FindItem( iMadeFrom );
	if( iMadeFrom == 0 || ourCreateEntry == NULL )
	{
		s->sysmessage( 1114 );
		return;
	}

	CChar *mChar = s->CurrcharObj();

	R32 avgMin = ourCreateEntry->AverageMinSkill();
	if( mChar->GetSkill( MINING ) < avgMin )
	{
		s->sysmessage( 1115 );
		return;
	}
	R32 avgMax = ourCreateEntry->AverageMaxSkill();

	Skills->CheckSkill( mChar, MINING, (SI16)avgMin, (SI16)avgMax );

	UI08 sumAmountRestored = 0;

	for( UI32 skCtr = 0; skCtr < ourCreateEntry->resourceNeeded.size(); ++skCtr )
	{
		UI16 amtToRestore = ourCreateEntry->resourceNeeded[skCtr].amountNeeded / 2;
		UString itemID = UString::number( ourCreateEntry->resourceNeeded[skCtr].idList.front(), 16 );
		UI16 itemColour = ourCreateEntry->resourceNeeded[skCtr].colour;
		sumAmountRestored += amtToRestore;
		Items->CreateScriptItem( s, mChar, "0x"+itemID, amtToRestore, OT_ITEM, true, itemColour );
	}
	
	s->sysmessage( 1116, sumAmountRestored );
	i->Delete();
}

void VialTarget( CSocket *mSock )
{
	VALIDATESOCKET( mSock );
	SERIAL targSerial = mSock->GetDWord( 7 );
	if( targSerial == INVALIDSERIAL )
		return;

	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	CItem *nVialID = static_cast<CItem *>(mSock->TempObj());
	mSock->TempObj( NULL );
	if( ValidateObject( nVialID ) )
	{
		CItem *nDagger = Combat->getWeapon( mChar );
		if( !ValidateObject( nDagger ) ) 
		{
			mSock->sysmessage( 742 );
			return;
		}
		if( nDagger->GetID() != 0x0F51 && nDagger->GetID() != 0x0F52 )
		{
			mSock->sysmessage( 743 );
			return;
		}

		nVialID->SetTempVar( CITV_MORE, 1, 0 );
		if( targSerial >= BASEITEMSERIAL )
		{	// it's an item
			CItem *targItem = calcItemObjFromSer( targSerial );
			if( !targItem->isCorpse() )
				mSock->sysmessage( 749 );
			else
			{
				nVialID->SetTempVar( CITV_MORE, 1, targItem->GetTempVar( CITV_MORE, 1 ) );
				Karma( mChar, NULL, -1000 );
				if( targItem->GetTempVar( CITV_MORE, 2 ) < 4 )
				{
					mSock->sysmessage( 750 );
					Skills->MakeNecroReg( mSock, nVialID, 0x0E24 );
					targItem->SetTempVar( CITV_MORE, 2, targItem->GetTempVar( CITV_MORE, 2 ) + 1 );
				}
				else 
					mSock->sysmessage( 751 );
			}
		}
		else
		{	// it's a char
			CChar *targChar = calcCharObjFromSer( targSerial );
			if( targChar == mChar )
			{
				if( targChar->GetHP() <= 10 )
					mSock->sysmessage( 744 );
				else
					mSock->sysmessage( 745 );
			}
			else if( objInRange( mChar, targChar, DIST_NEARBY ) )
			{
				if( targChar->IsNpc() )
				{
					if( targChar->GetID( 1 ) == 0x00 && ( targChar->GetID( 2 ) == 0x0C ||
						( targChar->GetID( 2 ) >= 0x3B && targChar->GetID( 2 ) <= 0x3D ) ) )
						nVialID->SetTempVar( CITV_MORE, 1, 1 );
				}
				else
				{
					CSocket *nCharSocket = targChar->GetSocket();
					nCharSocket->sysmessage( 746, mChar->GetName().c_str() );
				}
				if( WillResultInCriminal( mChar, targChar ) )
					criminal( mChar );
				Karma( mChar, targChar, -targChar->GetKarma() );
			}
			else
			{
				mSock->sysmessage( 747 );
				return;
			}
			targChar->Damage( RandomNum( 0, 5 ) + 2 );
			Skills->MakeNecroReg( mSock, nVialID, 0x0E24 );
		}
	}	
}

//o--------------------------------------------------------------------------o
//|	Function		-	void MultiTarget( CSocket *s )
//|	Date			-	Unknown
//|	Developers		-	Unknown
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Runs various commands based upon the target ID we sent to the socket
//o--------------------------------------------------------------------------o
//| Modifications	-	Overhauled to use an enum allowing simple modification (Zane)
//o--------------------------------------------------------------------------o
bool CPITargetCursor::Handle( void )
{
	CChar *mChar = tSock->CurrcharObj();
	if( tSock->TargetOK() )
	{
		if( !tSock->GetByte( 1 ) && !tSock->GetDWord( 7 ) && tSock->GetDWord( 11 ) == INVALIDSERIAL )
		{
			if( mChar->GetSpellCast() != -1 )	// need to stop casting if we don't target right
				mChar->StopSpell();
			return true; // do nothing if user cancels, avoids CRASH! - Morrolan
		}
		if( tSock->GetByte( 1 ) == 1 && !tSock->GetDWord( 7 ) )
			tSock->SetDWord( 7, INVALIDSERIAL );	// Client sends TargSer as 0 when we target an XY/Static, use INVALIDSERIAL as 0 could be a valid Serial - giwo

		UI08 a1 = tSock->GetByte( 2 );
		UI08 a2 = tSock->GetByte( 3 );
		UI08 a3 = tSock->GetByte( 4 );
		TargetIDs targetID = (TargetIDs)tSock->GetByte( 5 );
		tSock->TargetOK( false );
		if( mChar->IsDead() && !mChar->IsGM() && mChar->GetAccount().wAccountIndex != 0 )
		{
			tSock->sysmessage( 1008 );
			if( mChar->GetSpellCast() != -1 )	// need to stop casting if we don't target right
				mChar->StopSpell();
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
				cScript *tScript	= reinterpret_cast<cScript *>(tSock->TempInt());
				if( tScript != NULL )
					tScript->DoCallback( tSock, tSock->GetDWord( 7 ), static_cast<UI08>(targetID) );
				return true;
			}
 			else if( a3 == 0 )
			{
				switch( targetID )
				{
					case TARGET_ADDSCRIPTNPC:	AddScriptNpc( tSock );					break;
					case TARGET_BUILDHOUSE:		BuildHouseTarget( tSock );				break;
					case TARGET_TELE:			TeleTarget( tSock );					break;	
					case TARGET_DYE:			DyeTarget( tSock );						break;
					case TARGET_KEY:			KeyTarget( tSock );						break;
					case TARGET_DYEALL:			ColorsTarget( tSock );					break;
					case TARGET_DVAT:			DvatTarget( tSock );					break;
					case TARGET_INFO:			InfoTarget( tSock );					break;
					case TARGET_WSTATS:			WstatsTarget( tSock );					break;
					case TARGET_NPCRESURRECT:	NpcResurrectTarget( mChar );			break;
					case TARGET_TWEAK:			TweakTarget( tSock );					break;
					case TARGET_MAKESTATUS:		MakeStatusTarget( tSock );				break;
					case TARGET_SETSCPTRIG:		HandleSetScpTrig( tSock );				break;
					case TARGET_LOADCANNON:		LoadCannon( tSock );					break;
					case TARGET_VIAL:			VialTarget( tSock );					break;
					case TARGET_TILING:			Tiling( tSock );						break;
					case TARGET_SHOWSKILLS:		ShowSkillTarget( tSock );				break;
					// Vendors
					case TARGET_PLVBUY:			PlVBuy( tSock );						break;
					// Town Stuff
					case TARGET_TOWNALLY:		MakeTownAlly( tSock );					break;
					case TARGET_VOTEFORMAYOR:	cwmWorldState->townRegions[mChar->GetTown()]->VoteForMayor( tSock ); break;
					// House Functions
					case TARGET_HOUSEOWNER:		HouseOwnerTarget( tSock );				break;
					case TARGET_HOUSEEJECT:		HouseEjectTarget( tSock );				break;
					case TARGET_HOUSEBAN:		HouseBanTarget( tSock );				break;
					case TARGET_HOUSEFRIEND:	HouseFriendTarget( tSock );				break;
					case TARGET_HOUSEUNLIST:	HouseUnlistTarget( tSock );				break;
					case TARGET_HOUSELOCKDOWN:	HouseLockdown( tSock );					break;
					case TARGET_HOUSERELEASE:	HouseRelease( tSock );					break;
					// Pets
					case TARGET_FOLLOW:			FollowTarget( tSock );					break;
					case TARGET_ATTACK:			AttackTarget( tSock );					break;
					case TARGET_TRANSFER:		TransferTarget( tSock );				break;
					case TARGET_GUARD:			GuardTarget( tSock );					break;
					case TARGET_FRIEND:			FriendTarget( tSock );					break;
					// Magic
					case TARGET_CASTSPELL:		Magic->CastSpell( tSock, mChar );		break;					
					// Skills Functions
					case TARGET_ITEMID:			Skills->ItemIDTarget( tSock );			break;
					case TARGET_FISH:			Skills->FishTarget( tSock );			break;
					case TARGET_SMITH:			Skills->Smith( tSock );					break;
					case TARGET_MINE:			Skills->Mine( tSock );					break;
					case TARGET_SMELTORE:		Skills->SmeltOre( tSock );				break;
					case TARGET_REPAIRMETAL:	Skills->RepairMetal( tSock );			break;
					case TARGET_SMELT:			SmeltTarget( tSock );					break;
					case TARGET_STEALING:		Skills->StealingTarget( tSock );		break;
					case TARGET_PARTYADD:		PartyFactory::getSingleton().CreateInvite( tSock );	break;
					case TARGET_PARTYREMOVE:	PartyFactory::getSingleton().Kick( tSock );			break;
					default:															break;
				}
			}
		}
	}
	mChar->BreakConcentration( tSock );
	return true;
}

}
