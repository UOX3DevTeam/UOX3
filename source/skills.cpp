#include "uox3.h"
#include <algorithm>
#include "magic.h"
#include "cdice.h"
#include "skills.h"
#include "cGuild.h"
#include "combat.h"
#include "townregion.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "cMagic.h"
#include "ssection.h"
#include "trigger.h"
#include "scriptc.h"
#include "cScript.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "Dictionary.h"
#include "movement.h"

#undef DBGFILE
#define DBGFILE "skills.cpp"

namespace UOX
{

cSkills *Skills = NULL;

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Tailoring( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do Tailoring Skill Stuff
//o---------------------------------------------------------------------------o
void cSkills::Tailoring( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar	= s->CurrcharObj();
	CItem *packnum	= mChar->GetPackItem();
	if( !ValidateObject( packnum ) ) 
	{
		s->sysmessage( 773 ); 
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
	{
		s->sysmessage( 777 );
		return;
	}
	UI16 realID = i->GetID();
	if( ( realID >= 0x0F95 && realID <= 0x0F9C ) || ( realID >= 0x175D && realID <= 0x1764 ) || realID == 0x1078 )
	{
		if( i->IsLockedDown() )
		{
			s->sysmessage( 774 );
			return;
		}
		if( FindItemOwner( i ) != mChar )
			s->sysmessage( 775 );
		else
		{
			UI32 getAmt = GetItemAmount( mChar, i->GetID() );
			if( getAmt < 1 )
			{ 
				s->sysmessage( 776 );
				return;
			}
			if( realID == 0x1078 )
				s->TempObj( NULL );
			NewMakeMenu( s, 39, TAILORING );
		}
	}
}   

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Fletching( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do Fletching Skill Stuff
//o---------------------------------------------------------------------------o
void cSkills::Fletching( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar	= s->CurrcharObj();
	CItem *packnum	= mChar->GetPackItem();
	if( !ValidateObject( packnum ) ) 
	{ 
		s->sysmessage( 773 ); 
		return; 
	}

	CItem *i			= calcItemObjFromSer( s->GetDWord( 7 ) );
	CItem *skillItem	= static_cast<CItem *>(s->TempObj());
	s->TempObj( NULL );
	if( ValidateObject( i ) )
	{
		if( i->IsLockedDown() )
		{
			s->sysmessage( 774 );
			return;
		}
		UI16 realID = i->GetID();
		UI08 matID2 = (UI08)(skillItem->GetID( 2 ));
		if( ( matID2 >= 0xD4 && matID2 <= 0xD6 && realID >= 0x1BD1 && realID <= 0x1BD3 ) ||
			( matID2 >= 0xD1 && matID2 <= 0xD3 && realID >= 0x1BD4 && realID <=0x1BD6 ) )
		{
			if( FindItemOwner( i ) != mChar )
				s->sysmessage( 778 );
			else
				NewMakeMenu( s, 51, BOWCRAFT );
			return;
		}
	}
	s->sysmessage( 779 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::BowCraft( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do Bowcraft Skill Stuff
//o---------------------------------------------------------------------------o
void cSkills::BowCraft( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar	= s->CurrcharObj();
	CItem *packnum	= mChar->GetPackItem();
	if( !ValidateObject( packnum ) ) 
	{
		s->sysmessage( 773 ); 
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;
	if( i->IsLockedDown() )
	{
		s->sysmessage( 774 );
		return;
	}
	if( i->GetID() == 0x1BDD || i->GetID() == 0x1BE0 )
	{
		if( FindItemOwner( i ) != mChar )
		{
			s->sysmessage( 780 );
			return;
		}           
		UI32 getAmt = GetItemAmount(mChar, i->GetID() );
		if( getAmt < 2 )
		{
			s->sysmessage( 776 );
			return;
		}
		if( mChar->IsOnHorse() )
			Effects->PlayCharacterAnimation( mChar, 0x1C );	// moved here rather then the top of fun
		else 
			Effects->PlayCharacterAnimation( mChar, 0x0D );	// so that we don't make a motion if invalid target!

		NewMakeMenu( s, 49, BOWCRAFT );
	} 
}


//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Carpentry( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do Carpentry Skill Stuff
//o---------------------------------------------------------------------------o
void cSkills::Carpentry( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar	= s->CurrcharObj();
	CItem *packnum	= mChar->GetPackItem();
	if( !ValidateObject( packnum ) ) 
	{
		s->sysmessage( 773 );
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
	{
		s->sysmessage( 783 );
		return;
	}
	if( i->IsLockedDown() )
	{
		s->sysmessage( 774 );
		return;
	}
	UI16 realID = i->GetID();
	if( realID >= 0x1BD7 && realID <= 0x1BE2 )
	{
		if( FindItemOwner( i ) != mChar )
		{
			s->sysmessage( 781 );
			return;
		}         
		UI32 getAmt = GetItemAmount(mChar, i->GetID() );
		if( getAmt < 9 )
		{	 
			s->sysmessage( 782 );
			return;
		}       
		NewMakeMenu( s, 19, CARPENTRY );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  SI32 cSkills::CalcRankAvg( CChar *player, createEntry& skillMake )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Unknown
//o---------------------------------------------------------------------------o
SI32 cSkills::CalcRankAvg( CChar *player, createEntry& skillMake )
{
	if( !cwmWorldState->ServerData()->RankSystemStatus() ) 
		return 10;

	R32 rankSum = 0;

	int rk_range, rank;
	R32 sk_range, randnum, randnum1;
	
	for( size_t i = 0; i < skillMake.skillReqs.size(); ++i )
	{
		rk_range = skillMake.maxRank - skillMake.minRank;
		sk_range = static_cast<R32>(50.00 + player->GetSkill( skillMake.skillReqs[i].skillNumber ) - skillMake.skillReqs[i].minSkill);
		if( sk_range <= 0 ) 
			rank = skillMake.minRank;
		else if( sk_range >= 1000 ) 
			rank = skillMake.maxRank;
		randnum = static_cast<R32>(RandomNum( 0, 999 ));
		if( randnum <= sk_range ) 
			rank = skillMake.maxRank;
		else
		{
			randnum1 = (R32)( RandomNum( 0, 999 ) ) - (( randnum - sk_range ) / ( 11 - cwmWorldState->ServerData()->SkillLevel() ) );
			rank = (int)( ( randnum1 * rk_range ) / 1000 );
			rank += skillMake.minRank - 1;
			if( rank > skillMake.maxRank ) 
				rank = skillMake.maxRank;
			if( rank < skillMake.minRank ) 
				rank = skillMake.minRank;
		}
		rankSum += rank;
	}
	return (SI32)(rankSum / skillMake.skillReqs.size());
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::ApplyRank( cSocket *s, CItem *c, UI08 rank )
//|   Date        :  24 August 1999
//|   Programmer  :  Magius(CHE)
//o---------------------------------------------------------------------------o
//|   Purpose     :  Modify variables based on item's rank.
//o---------------------------------------------------------------------------o

void cSkills::ApplyRank( cSocket *s, CItem *c, UI08 rank )
{
	char tmpmsg[512];
	*tmpmsg='\0';
	
	if( cwmWorldState->ServerData()->RankSystemStatus() )
	{
		c->SetRank( rank );
		if( c->GetLoDamage() > 0 ) 
			c->SetLoDamage( (SI16)( ( rank * c->GetLoDamage() ) / 10 ) );
		if( c->GetHiDamage() > 0 ) 
			c->SetHiDamage( (SI16)( ( rank * c->GetHiDamage() ) / 10 ) );
		if( c->GetDef() > 0 )      
			c->SetDef( (UI16)( ( rank * c->GetDef() ) / 10 ) );
		if( c->GetHP() > 0 )      
			c->SetHP( (SI16)( ( rank * c->GetHP() ) / 10 ) );
		if( c->GetMaxHP() > 0 )   
			c->SetMaxHP( (SI16)( ( rank * c->GetMaxHP() ) / 10 ) );
		if( c->GetBuyValue() > 0 )
			c->SetBuyValue( (UI32)( ( rank * c->GetBuyValue() ) / 10 ) );
		
		if( rank <= 10 )
			s->sysmessage( 783 + rank );
	}
	else 
		c->SetRank( rank );
	
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::RegenerateOre( long grX, long grY )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Regenerate Ore based on Server.scp Ore respawn settings
//o---------------------------------------------------------------------------o
void cSkills::RegenerateOre( long grX, long grY )
{
	resourceEntry *orePart	= &resources[grX][grY];
	SI16 oreCeiling			= cwmWorldState->ServerData()->ResOre();
	SI16 oreTimer			= cwmWorldState->ServerData()->ResOreTime();
	if( static_cast<UI32>(orePart->oreTime) <= cwmWorldState->GetUICurrentTime() )	// regenerate some more?
	{
		for( SI16 counter = 0; counter < oreCeiling; ++counter )	// keep regenerating ore
		{
			if( orePart->oreAmt < oreCeiling && static_cast<UI32>(orePart->oreAmt + counter * oreTimer * 1000) < cwmWorldState->GetUICurrentTime() )
				++orePart->oreAmt;
			else
				break;
		}
		orePart->oreTime = BuildTimeValue( static_cast<R32>(oreTimer) );
	}
	if( orePart->oreAmt > oreCeiling )
		orePart->oreAmt = oreCeiling;
}

//o--------------------------------------------------------------------------
//| Function    - void cSkills::Mine( cSocket *s )
//| Date        - Unknown
//| Programmer  - Unknown
//| Modified    - Cork(Unknown)/Abaddon(February 19, 2000)
//o--------------------------------------------------------------------------
//| Purpose     - (Fill this in)
//| Comments    - Skill checking now implemented. You cannot mine colored ore
//|               unless you have the proper mining skill for each ore type. -Cork
//|               Rewrote most of it to clear up some of the mess-Abaddon
//o--------------------------------------------------------------------------
void cSkills::Mine( cSocket *s )
{
	VALIDATESOCKET( s );
	bool floor = false, mountain = false;
	SI16 oreX, oreY;
	
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;	// did we cancel the target?

	if( cwmWorldState->ServerData()->ResOreArea() < 10 )	// make sure there are enough minimum areas
		cwmWorldState->ServerData()->ResOreArea( 10 );

	SI16 targetX = s->GetWord( 0x0B );		// store our target x y and z locations
	SI16 targetY = s->GetWord( 0x0D );
	SI08 targetZ = s->GetByte( 0x10 );
	CChar *mChar = s->CurrcharObj();
	SI16 distX = abs( mChar->GetX() - targetX );			// find our distance
	SI16 distY = abs( mChar->GetY() - targetY );

	if( distX > 5 || distY > 5 )							// too far away?
	{
		s->sysmessage( 799 );
		return;
	}
	
	s->SetTimer( tPC_SKILLDELAY, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->ServerSkillDelayStatus() )) );

	UI08 targetID1 = s->GetByte( 0x11 );							// store ids of what we targeted
	UI08 targetID2 = s->GetByte( 0x12 );

	if( targetZ < 28 && targetID1 == 14 )					// if not too high
	{
		switch( targetID2 )
		{
			case 0xD3:
			case 0xDF:
			case 0xE0:
			case 0xE1:
			case 0xE8:
				GraveDig( s );							// check to see if we targeted a grave, if so, check it
				break;
			default:
				break;
		}
	}
	
	switch( cwmWorldState->ServerData()->MineCheck() )
	{
		case 0:
			floor = true;
			mountain = true;
			break;
		case 1:
			if( targetZ == 0 )			// check to see if we're targetting a dungeon floor
			{
				if( targetID1 == 0x05 )
				{
					if( ( targetID2 >= 0x3B && targetID2 <= 0x4F ) || ( targetID2 >= 0x51 && targetID2 <= 0x53 ) || ( targetID2 == 0x6A ) )
						floor = true;
				}
			}
			if( !floor && targetZ >= 0 )	// mountain not likely to be below 0 (but you never know, do you? =)
			{
				if( targetID1 != 0 && targetID2 != 0 )	// we might have a static rock or mountain
				{
					MapStaticIterator msi( targetX, targetY, mChar->WorldNumber() );
					CTile tile;
					staticrecord *stat = NULL;
					while( ( ( stat = msi.Next() ) != NULL ) && !mountain )
					{
						msi.GetTile( &tile );
						if( targetZ == stat->zoff && ( !strcmp( tile.Name(), "rock" ) || !strcmp( tile.Name(), "mountain" ) || !strcmp( tile.Name(), "cave" ) ) )
							mountain = true;
					}
				}
				else		// or it could be a map only
				{  
					// manually calculating the ID's if a maptype
					map_st map1;
					CLand land;
					map1 = Map->SeekMap0( targetX, targetY, mChar->WorldNumber() );
					Map->SeekLand( map1.id, &land );
					if( !strcmp( "rock", land.Name() ) || !strcmp( land.Name(), "mountain" ) || !strcmp( land.Name(), "cave" ) ) 
						mountain = true; 
				}
			}
			break;
		case 2:	// need to modify scripts to support this!
			floor = true;		// we'll default to work everywhere for these scripts
			mountain = true;
			break;
		default:
			s->sysmessage( 800 );
			return;
	}

	oreX = static_cast<SI16>(targetX / cwmWorldState->ServerData()->ResOreArea());	// we want the area where we targetted, not where we are
	oreY = static_cast<SI16>(targetY / cwmWorldState->ServerData()->ResOreArea());
	if( (!floor && !mountain) || (oreX >= 610 || oreY >= 410) )		// if we can't mine here or if for some reason it's invalid, clear out
	{
		s->sysmessage( 801 );
		return;
	}

	RegenerateOre( oreX, oreY );
	resourceEntry *orePart = &resources[oreX][oreY];
	if( orePart->oreAmt <= 0 )
	{
		s->sysmessage( 802 );
		return;
	}
	if( mChar->IsOnHorse() != 0 )	// do action and sound
		Effects->PlayCharacterAnimation( mChar, 0x1A );
	else
		Effects->PlayCharacterAnimation( mChar, 0x0B );
	
	Effects->PlaySound( s, 0x0125, true ); 
	
	if( !CheckSkill( mChar, MINING, 0, 1000 ) ) // check to see if our skill is good enough
	{
		s->sysmessage( 803 );
		if( orePart->oreAmt > 0 && RandomNum( 0, 1 ) )
			--orePart->oreAmt;
		return;
	} 
	else if( orePart->oreAmt > 0 )
		--orePart->oreAmt;
	
#ifdef _DEBUG
		Console << "DBG: Mine(\"" << mChar->GetName() << "\"[" << mChar->GetSerial() << "]); --> MINING: " << mChar->GetSkill( MINING ) << "  RaceID: " << mChar->GetRace() << myendl;
#endif
		
		cTownRegion *targRegion = mChar->GetRegion();
		if( mChar->GetSkill( MINING ) >= targRegion->GetMinColourSkill() && RandomNum( 0, 100 ) >= targRegion->GetChanceColourOre() )
			MakeOre( mChar->GetRegionNum(), mChar, s );
		else  //  We didn't find any colored ore, so grab some iron ore
		{
			CItem *oreItem = NULL;
			if( RandomNum( 0, 100 ) > targRegion->GetChanceBigOre() )
				oreItem = Items->CreateItem( s, mChar, 0x19BA, 5, 0, OT_ITEM, true );
			else
				oreItem = Items->CreateItem( s, mChar, 0x19B9, 1, 0, OT_ITEM, true );
			if( oreItem != NULL )
				oreItem->SetName( "Iron Ore" );
			s->sysmessage( 804 );
		}
	}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::GraveDig( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do GraveDigging Stuff (Should probably be replaced with an
//|											OSI-like Treasure Hunting System based off a Resource
//|											System much like Ore and Logs)
//o---------------------------------------------------------------------------o
void cSkills::GraveDig( cSocket *s )
{
	VALIDATESOCKET( s );
	SI16	nFame;
	char	iID = 0;
	CItem *	nItemID = NULL;
	
	CChar *nCharID = s->CurrcharObj();
	Karma( nCharID, NULL, -2000 ); // Karma loss no lower than the -2 pier
	
	if( nCharID->IsOnHorse() )
		Effects->PlayCharacterAnimation( nCharID, 0x1A );
	else
		Effects->PlayCharacterAnimation( nCharID, 0x0b );
	Effects->PlaySound( s, 0x0125, true );
	if( !CheckSkill( nCharID, MINING, 0, 800 ) ) 
	{
		s->sysmessage( 805 );
		return;
	}
	
	nFame = nCharID->GetFame();
	if( nCharID->IsOnHorse() )
		Effects->PlayCharacterAnimation( nCharID, 0x1A );
	else  
		Effects->PlayCharacterAnimation( nCharID, 0x0B );
	Effects->PlaySound( s, 0x0125, true );
	CChar *spawnCreature = NULL;
	switch( RandomNum( 0, 12 ) )
	{
		case 2:
			spawnCreature = Npcs->CreateRandomNPC( "weakundeadlist" ); // Low level Undead - Random
			s->sysmessage( 806 );
			break;
		case 4:
			nItemID = Items->CreateRandomItem( s, "diggingarmor" ); // Armor and shields - Random
			if( nItemID == NULL )
				break;
			if( nItemID->GetID() >= 7026 && nItemID->GetID() <= 7035 )
				s->sysmessage( 807 );
			else
				s->sysmessage( 808 );
			break;
		case 5:
			//Random treasure between gems and gold
			if( RandomNum( 0, 1 ) )
			{  // randomly create a gem and place in backpack
				Items->CreateRandomItem( s, "digginggems" );
				s->sysmessage( 809 );
			}
			else
			{  // Create between 1 and 15 goldpieces and place directly in backpack
				UI08 nAmount = RandomNum( 1, 15 );
				Items->CreateItem( s, nCharID, 0x0EED, nAmount, 0, OT_ITEM, true );
				Effects->goldSound( s, nAmount );
				if( nAmount == 1 )
					s->sysmessage( 810, nAmount );
				else
					s->sysmessage( 810, nAmount );
			}
			break;
		case 6:
			spawnCreature = Npcs->CreateRandomNPC( "weakundeadlist" ); // Low level Undead - Random
			s->sysmessage( 806 );
			break;
		case 8:
			Items->CreateRandomItem( s, "diggingweapons" );
			s->sysmessage( 811 );
			break;
		case 10:
		case 12:
			if( nFame < 1000 )
				spawnCreature = Npcs->CreateRandomNPC( "weakundeadlist" ); // Med level Undead - Random
			else
				spawnCreature = Npcs->CreateRandomNPC( "strongundeadlist" ); // High level Undead - Random
			s->sysmessage( 806 );
			break;
		default:
			if( RandomNum( 0, 1 ) )
			{
				Items->CreateRandomItem( s, "diggingbones" );		// Random Bones
				s->sysmessage( 812 );
			}
			else	// found an empty grave
				s->sysmessage( 813 );
			break;
	}
	if( spawnCreature != NULL )
		spawnCreature->SetLocation( nCharID );
}

//o--------------------------------------------------------------------------
//| Function    - void cSkills::SmeltOre( cSocket *s );
//| Date        - Unknown
//| Programmer  - Unknown
//| Modified    - Abaddon(February 19, 2000)
//o--------------------------------------------------------------------------
//| Purpose     - Rewritten to use case and structure, you'll find it is 
//|               easier to make it scriptable now. The structure is pretty 
//|               much all that'd be needed for any future ore->ingot conversions
//|               scripting the ore would probably be even simpler, requires 
//|               less info
//o--------------------------------------------------------------------------
void cSkills::SmeltOre( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *chr			= s->CurrcharObj();
	CItem *smeltedItem	= static_cast<CItem *>(s->TempObj());
	CItem *anvil		= calcItemObjFromSer( s->GetDWord( 7 ) );				// Let's find our anvil
	
	if( ValidateObject( anvil ) )					// if we have an anvil
	{
		switch( anvil->GetID() )	// Check to ensure it is an anvil
		{
			case 0x0FB1:
			case 0x197A:
			case 0x19A9:
			case 0x197E:
			case 0x1982:
			case 0x1986:
			case 0x198A:
			case 0x198E:
			case 0x1992:
			case 0x1996:
			case 0x199A:
			case 0x19A6:
			case 0x19A2:
			case 0x199E:
				if( objInRange( chr, anvil, DIST_NEARBY ) ) //Check if the forge is in range  
				{
					UI16 targColour		= smeltedItem->GetColour();
					miningData *oreType	= FindOre( targColour );
					if( oreType == NULL )
					{
						s->sysmessage( 814 );
						return;
					}
					char ingotString[100];
					if( oreType->foreign )	// if not iron, generally
					{
						if( chr->GetSkill( MINING ) < oreType->minSkill )
						{
							s->sysmessage( 815 );
							return;
						}
					}
					if( !CheckSkill( chr, MINING, oreType->minSkill, 1000 ) )	// if we do not have minimum skill to use it
					{
						if( smeltedItem->GetAmount() > 1 )	// If more than 1 ore, half it
						{
							s->sysmessage( 817 );
							smeltedItem->SetAmount( smeltedItem->GetAmount() / 2 );
						}
						else
						{
							s->sysmessage( 816 );
							smeltedItem->Delete();
						}
						return;
					}
					UI16 ingotNum = smeltedItem->GetAmount() * 2;	// 2 Ingots per ore pile.... shouldn't this be variable based on quality of ore?
					sprintf( ingotString, "%s Ingot", oreType->name.c_str() );
					CItem *ingot = Items->CreateItem( s, chr, 0x1BF2, ingotNum, oreType->colour, OT_ITEM, true );
					if( ingot != NULL )
						ingot->SetName( ingotString );
					s->sysmessage( 818 );
					s->sysmessage( 819, oreType->name.c_str() );
					smeltedItem->Delete();	// delete the ore
				}
				break;
			default:
				s->sysmessage( 820 );
				break;
		}     
	} 

	s->TempObj( NULL );
	s->statwindow( chr );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Wheel( cSocket *s, int mat )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when using Yarn or Thread on a Spinning Wheel
//o---------------------------------------------------------------------------o
void cSkills::Wheel( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
	{
		if( i->IsLockedDown() )
		{
			s->sysmessage( 774 );
			return;
		}
		if( i->GetID() >= 0x10A4 && i->GetID() <= 0x10A6 )
		{
			if( objInRange( mChar, i, DIST_NEARBY ) )  
			{
				if( !CheckSkill( mChar, TAILORING, 0, 1000 ) ) 
				{
					s->sysmessage( 821 );
					return;
				}   
				s->sysmessage( 822 );

				CItem *skillItem = static_cast<CItem *>(s->TempObj());
				s->TempObj( NULL );
				if( ValidateObject( skillItem ) )
				{
					skillItem->SetName( "#" );
					if( skillItem->GetID() == 0x0DF8 ) // Wool
						skillItem->SetID( 0x0E1D );
					else if( skillItem->GetID() == 0x0DF9 ) // Cotton
						skillItem->SetID( 0x0FA0 );
					skillItem->SetAmount( skillItem->GetAmount() * 4 );
					
					skillItem->SetDecayable( true );
					return;
				}
			}
		}     
	}   
	s->sysmessage( 823 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Loom( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when using a loom to make cloth
//o---------------------------------------------------------------------------o
void cSkills::Loom( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar	= s->CurrcharObj();
	CItem *i		= calcItemObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		if( i->IsLockedDown() )
		{
			s->sysmessage( 774 );
			return;
		}
		if( i->GetID() >= 0x105F && i->GetID() <= 0x1061 )
		{
			if( objInRange( mChar, i, DIST_NEARBY ) )  
			{
				if( !CheckSkill( mChar, TAILORING, 0, 1000 ) ) 
				{
					s->sysmessage( 824 );
					return;
				}   
				s->sysmessage( 825 );
				CItem *skillItem = static_cast<CItem *>(s->TempObj());
				s->TempObj( NULL );
				if( ValidateObject( skillItem ) )
				{
					skillItem->SetName( "#");
					skillItem->SetID( 0x175D );
					skillItem->SetDecayable( true );
					skillItem->SetAmount( skillItem->GetAmount() * 2 );
					
					return;
				}
			}
		}     
	}   
	s->sysmessage( 823 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  cSkills::handleCooking( cSocket *s )
//|   Date        :  January 30, 2003
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handle baking/cooking in a single function
//o---------------------------------------------------------------------------o
void cSkills::handleCooking( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		if( i->IsLockedDown() )
			s->sysmessage( 774 );
		else if( objInRange( mChar, i, DIST_NEARBY ) )
		{
			CItem *skillItem = static_cast<CItem *>(s->TempObj());
			s->TempObj( NULL );
			if( ValidateObject( skillItem ) )
			{
				UI16 realItem = i->GetID();
				bool isMeat = ( realItem >= 0x0DE3 && realItem <= 0x0DE9 ||	realItem == 0x0FAC ||
					realItem == 0x0FB1 || realItem >= 0x197A && realItem <= 0x19B6 ||
					realItem >= 0x0461 && realItem <= 0x0480 ||	realItem >= 0x0E31 && realItem <= 0x0E33 ||
					realItem == 0x19BB || realItem == 0x1F2B || realItem >= 0x092B && realItem <= 0x0934 ||
					realItem >= 0x0937 && realItem <= 0x0942 ||	realItem >= 0x0945 && realItem <= 0x0950 ||
					realItem >= 0x0953 && realItem <= 0x095E ||	realItem >= 0x0961 && realItem <= 0x096C );
				if( !CheckSkill( mChar, COOKING, 0, 1000 ) ) 
				{
					if( isMeat )
					{
						char burntName[50];
						UI16 amntDiff = RandomNum( static_cast< UI16 >(1), skillItem->GetAmount() );
						sprintf( burntName, Dictionary->GetEntry( 1430 ).c_str(), amntDiff );
						s->sysmessage( 1431 );
						skillItem->IncAmount( -amntDiff );
						CItem *burnItem = Items->CreateItem( s, mChar, 0x1EB0, amntDiff, 0, OT_ITEM, true );
						if( burnItem != NULL )
							burnItem->SetName( burntName );
						return;
					}
					s->sysmessage( 826 );
					i->Delete();
				}   
				else
				{
					skillItem->SetName( "#" );
					skillItem->SetDecayable( true );
					switch( i->GetID() )
					{
						case 0x103A:	// Dough
							s->sysmessage( 827 );
							skillItem->SetID( 0x103D );
							skillItem->SetAmount( skillItem->GetAmount() * 2 );
							break;
						case 0x103D:	// Pizza
							s->sysmessage( 828 );
							skillItem->SetID( 0x1083 );
							skillItem->SetAmount( skillItem->GetAmount() * 2 );
							break;
						default:
							if( isMeat )
							{
								s->sysmessage( 1432 );
								skillItem->SetID( 0x09F2 );
								skillItem->SetType( IT_FOOD );
							}
							else
								s->sysmessage( 829 );
							break;
					}
				}
			}
		}
		else
			s->sysmessage( 830 );
	}
}
//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Hide( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//|									
//|	Modification	-	09/22/2002	-	Xuri - Support for user config of hiding 
//|									while mounted
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses hiding skill
//o---------------------------------------------------------------------------o
void cSkills::Hide( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	
	if( !cwmWorldState->ServerData()->CharHideWhileMounted() )
	{
		if( mChar->IsOnHorse() )
		{
			s->sysmessage( 831 );
			return;
		}
	}
	if( ValidateObject( mChar->GetAttacker() ) && charInRange( mChar, mChar->GetAttacker() ) )
	{
		s->sysmessage( 832 );
		return;
	}
	if( mChar->GetVisible() != VT_VISIBLE )
	{
		s->sysmessage( 833 );
		return;
	}
	if( !CheckSkill( mChar, HIDING, 0, 1000 ) ) 
	{
		s->sysmessage( 834 );
		return;
	}   
	s->sysmessage( 835 );
	mChar->SetVisible( VT_TEMPHIDDEN );
	mChar->SetStealth( -1 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Stealth( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses Stealth skill
//o---------------------------------------------------------------------------o
void cSkills::Stealth( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetVisible() != VT_TEMPHIDDEN ) 
	{
		s->sysmessage( 836 );
		return;
	}
	if( mChar->IsOnHorse() )
	{
		s->sysmessage( 837 );
		return;
	}
	if( mChar->GetSkill( HIDING ) < 700 )
	{
		s->sysmessage( 838 );
		mChar->ExposeToView();
		return;
	}   		
	if( !CheckSkill( mChar, STEALTH, 0, 1000 ) ) 
	{
		mChar->ExposeToView();
		return;
	}   
	s->sysmessage( 839, cwmWorldState->ServerData()->MaxStealthMovement() );
	mChar->SetVisible( VT_TEMPHIDDEN );
	mChar->SetStealth( 0 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::TreeTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//|									
//|	Modification	-	09/22/2002	-	Xuri - Changed default minimum range for 
//|									chopping trees from unrealistic 5 to logical 2 :)
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses an item and targets a tree
//o---------------------------------------------------------------------------o
void cSkills::TreeTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( cwmWorldState->ServerData()->ResLogArea() < 10 ) 
		cwmWorldState->ServerData()->ResLogArea( 10 );
	
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;
	
	SI16 px = s->GetWord( 0x0B );
	SI16 py = s->GetWord( 0x0D );
	SI16 cx = abs( mChar->GetX() - px );
	SI16 cy = abs( mChar->GetY() - py );

	UI08 dir = Movement->Direction( mChar, px, py );
	if( dir != 0xFF && mChar->GetDir() != dir )
		mChar->SetDir( dir );
	if( cx > 2 || cy > 2 )
	{
		s->sysmessage( 393 );
		return;
	}

	SI16 a = static_cast<SI16>(mChar->GetX() / cwmWorldState->ServerData()->ResLogArea());
	SI16 b = static_cast<SI16>(mChar->GetY() / cwmWorldState->ServerData()->ResLogArea());

	if( a >= 610 || b >= 410 ) 
		return; // wih the previous a < 20 || b < 20, wind may not have worked right, as well as some dungeons
	
	RegenerateLog( a, b );
	resourceEntry *logPart = &resources[a][b];
	if( logPart->logAmt <= 0 )
	{
		s->sysmessage( 840 );
		return;
	}
	
	CItem *packnum = mChar->GetPackItem();
	if( !ValidateObject( packnum ) ) 
	{ 
		s->sysmessage( 841 ); 
		return; 
	}
	
	if( mChar->IsOnHorse() ) 
		Effects->PlayCharacterAnimation( mChar, 0x1C );
	else 
		Effects->PlayCharacterAnimation( mChar, 0x0D );
	Effects->PlaySound( s, 0x013E, true );
	
	if( !CheckSkill( mChar, LUMBERJACKING, 0, 1000 ) ) 
	{
		s->sysmessage( 842 );
		if( logPart->logAmt > 0 && RandomNum( 1, 2 ) == 1 ) 
			--logPart->logAmt;//Randomly deplete resources even when they fail 1/2 chance you'll loose wood.
		return;
	}
	
	if( logPart->logAmt > 0 ) 
		--logPart->logAmt;
	

		CItem *c = Items->CreateItem( s, mChar, 0x1BE0, 10, 0, OT_ITEM, true );
		if( c == NULL )
			return;
		if( c->GetAmount() > 10 ) 
			s->sysmessage( 1434 );
		else 
			s->sysmessage( 1435 );
	}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::RegenerateLog( long grX, long grY )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Used by logging system to respawn the log resource
//o---------------------------------------------------------------------------o
void cSkills::RegenerateLog( long grX, long grY )
{
	SI16 logCeiling			= cwmWorldState->ServerData()->ResLogs();
	UI16 logTimer			= cwmWorldState->ServerData()->ResLogTime();
	resourceEntry *logPart	= &resources[grX][grY];
	if( static_cast<UI32>(logPart->logTime) <= cwmWorldState->GetUICurrentTime() )
	{
		for( SI16 c = 0; c < logCeiling; ++c )
		{
			if( static_cast<UI32>(( logPart->logTime + ( c * logTimer * 1000 ) )) <= cwmWorldState->GetUICurrentTime() && logPart->logAmt < logCeiling )
				++logPart->logAmt;
			else 
				break;
		}
		logPart->logTime = BuildTimeValue( static_cast<R32>(logTimer ));//10 more mins
	}
	
	if( logPart->logAmt > logCeiling ) 
		logPart->logAmt = logCeiling;
	
}


//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::DetectHidden( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses the detect hidden skill, Greater
//|					 your skill is the longer range you can detect from where
//|					 where you target, hiders near the center of the target
//|					 are easier to detect, while hiders further from the center
//|					 are more difficult to detect
//o---------------------------------------------------------------------------o
void cSkills::DetectHidden( cSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;
	
	UI16 j, getSkill;
	SI16 dx, dy;
	R64 c,range;         //int is too restricting
	
	SI16 x = s->GetWord( 11 );
	SI16 y = s->GetWord( 13 );
//	SI08 z = s->GetByte( 16 );
	CChar *mChar = s->CurrcharObj();

	j = mChar->GetSkill( DETECTINGHIDDEN );
	
	range = (j*j/1.0E6) * ( MAX_VISRANGE + Races->VisRange( mChar->GetRace() ) );     // this seems like an ok formula
	
	REGIONLIST nearbyRegions = MapRegion->PopulateList( mChar );
	REGIONLIST_ITERATOR rIter;
	for( rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		SubRegion *MapArea = (*rIter);
		if( MapArea == NULL )	// no valid region
			continue;
		MapArea->PushChar();
		for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
		{
			if( !ValidateObject( tempChar ) )
				continue;
			if( tempChar->GetVisible() == VT_TEMPHIDDEN ) // do not detect invis people only hidden ones
			{
				dx = abs( tempChar->GetX() - x );
				dy = abs( tempChar->GetY() - y );
				c = hypot( dx, dy );
				getSkill = (UI16)( tempChar->GetSkill( HIDING ) * tempChar->GetSkill( HIDING ) / 1E3 - (range*50/(MAX_VISRANGE + Races->VisRange( mChar->GetRace() )) )*(range-c)/range);
				//if( getSkill < 0 )  // Can't happen, getSkill is unsigned
					//getSkill = 0;
				if( getSkill > 1000 ) 
					getSkill = 1000;
				
				if( ( CheckSkill( mChar, DETECTINGHIDDEN, getSkill, 1000 ) ) && ( c <= range ) )
				{
					tempChar->ExposeToView();
					cSocket *kSock = calcSocketObjFromChar( tempChar );
					if( kSock != NULL )
						kSock->sysmessage( 1436 );
				}
				else 
					s->sysmessage( 1437 );
			}
		}
		MapArea->PopChar();
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::PeaceMaking( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when using the PeaceMaking Skill, Also requires
//|					 adequate Musicianship skill to succeed.
//o---------------------------------------------------------------------------o
void cSkills::PeaceMaking( cSocket *s )
{
	VALIDATESOCKET( s );
	CItem *getInst = GetInstrument( s );
	if( !ValidateObject( getInst ) ) 
	{
		s->sysmessage( 1438 );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	bool cs1 = CheckSkill( mChar, MUSICIANSHIP, 0, 1000 );
	bool cs2 = CheckSkill( mChar, PEACEMAKING, 0, 1000 );
	if( cs1 && cs2 )
	{
		PlayInstrument( s, getInst, true );
		s->sysmessage( 1439 );

		REGIONLIST nearbyRegions = MapRegion->PopulateList( mChar );
		REGIONLIST_ITERATOR rIter;
		for( rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
		{
			SubRegion *MapArea = (*rIter);
			if( MapArea == NULL )	// no valid region
				continue;
			MapArea->PushChar();
			for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
			{
				if( !ValidateObject( tempChar ) )
					continue;
				if( charInRange( tempChar, mChar ) && tempChar->IsAtWar() )
				{
					cSocket *jSock = calcSocketObjFromChar( tempChar );
					if( jSock != NULL )
						jSock->sysmessage( 1440 );
					if( tempChar->IsAtWar() ) 
						tempChar->ToggleCombat();
					tempChar->SetTarg( NULL );
					tempChar->SetAttacker( NULL );
					tempChar->SetAttackFirst( false );
				}
			}
			MapArea->PopChar();
		}
	}
	else 
	{
		PlayInstrument( s, getInst, false );
		s->sysmessage( 1441 );
	}
}

//o---------------------------------------------------------------------------o
//|		Function    :	void cSkills::PlayInstrument( cSocket *s, CItem *i, bool wellPlayed )
//|		Date        :	2/10/2003
//|		Programmer  :	Zane
//o---------------------------------------------------------------------------o
//|		Purpose     :	Play an Instrument (plays "played well" sfx if wellPlayed == true
//|						Otherwise plays "played poorly" sfx
//o---------------------------------------------------------------------------o
void cSkills::PlayInstrument( cSocket *s, CItem *i, bool wellPlayed )
{
	switch( i->GetID( 2 ) )
	{
		case 0x9C:
			if( wellPlayed )
				Effects->PlaySound( s, 0x0038, true );	
			else
				Effects->PlaySound( s, 0x0039, true );	
			break;
		case 0x9D:
		case 0x9E:	
			if( wellPlayed )
				Effects->PlaySound( s, 0x0052, true );	
			else
				Effects->PlaySound( s, 0x0053, true );	
			break;
		case 0xB1:
		case 0xB2:	
			if( wellPlayed )
				Effects->PlaySound( s, 0x0043, true );	
			else
				Effects->PlaySound( s, 0x0044, true );	
			break;
		case 0xB3:
		case 0xB4:	
			if( wellPlayed )
				Effects->PlaySound( s, 0x004C, true );	
			else
				Effects->PlaySound( s, 0x004D, true );	
			break;
		default:
			Console.Error( 2, " Fallout of switch statement without default. skills.cpp, cSkills::PlayInstrument()" );
			break;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  CItem *cSkills::GetInstrument( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Used by Peacemaking, Provocation and Enticement to find
//|					 the instrument in your pack (if any exist)
//o---------------------------------------------------------------------------o
CItem * cSkills::GetInstrument( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::GetInstrument() Invalid socket" );
		return NULL;
	}
	CChar *mChar	= s->CurrcharObj();
	CItem *x		= mChar->GetPackItem();
	if( !ValidateObject( x ) ) 
		return NULL;
	for( CItem *i = x->FirstItem(); !x->FinishedItems(); i = x->NextItem() )
	{
		if( ValidateObject( i ) )
		{
			switch( i->GetID() )
			{
				case 0x0E9C:
				case 0x0E9D:
				case 0x0E9E:
				case 0x0EB1:
				case 0x0EB2:
				case 0x0EB3:
				case 0x0EB4:
					return i;
				default:
					break;
			}
		}
	}
	return NULL;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::ProvocationTarget1( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check 1st Provoc Target and Add its ID to the socket, then
//|					 bring up a targeting cursor for player to select second
//|					 Provoc Target
//o---------------------------------------------------------------------------o
void cSkills::ProvocationTarget1( cSocket *s )
{
	VALIDATESOCKET( s );
	SERIAL getSer = s->GetDWord( 7 );
	CChar *trgChar = calcCharObjFromSer( getSer );
	if( !ValidateObject( trgChar ) )
		return;
	else if( !trgChar->IsNpc() )
		s->sysmessage( 1442 );
	else if( trgChar->IsInvulnerable() || trgChar->GetNPCAiType() == aiPLAYERVENDOR || trgChar->GetNPCAiType() == aiGUARD ) // not invul, a player vendor, or a guard
		s->sysmessage( 830 );
	else
	{
		CItem *getInst = GetInstrument( s );
		if( !ValidateObject( getInst ) ) 
		{
			s->sysmessage( 1438 );
			return;
		}
		s->TempObj( trgChar );
		s->target( 0, TARGET_PROVOCATION2, 1443 );
		PlayInstrument( s, getInst, true );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::EnticementTarget1( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check 1st Entice Target and Add its ID to the socket, then
//|					 bring up a targeting cursor for player to select second
//|					 Entice Target
//o---------------------------------------------------------------------------o
void cSkills::EnticementTarget1( cSocket *s )
{
	VALIDATESOCKET( s );
	SERIAL getSer = s->GetDWord( 7 );
	CChar *trgChar = calcCharObjFromSer( getSer );
	if( !ValidateObject( trgChar ) )
		return;
	else if( !trgChar->IsNpc() )
		s->sysmessage( 1445 );
	else if( trgChar->GetNPCAiType() == aiPLAYERVENDOR || trgChar->IsInvulnerable() || trgChar->GetNPCAiType() == aiGUARD )	// no PV, guard, or invul person
		s->sysmessage( 1642 );
	else
	{
		CItem *getInst = GetInstrument( s );
		if( !ValidateObject( getInst ) ) 
		{
			s->sysmessage( 1444 );
			return;
		}
		s->TempObj( trgChar );
		s->target( 0, TARGET_ENTICEMENT2, 1446 );
		PlayInstrument( s, getInst, true );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::EnticementTarget2( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check 2nd Entice Target and players Entice and Musicianship
//|					 skill then if checks pass flag the player criminal
//o---------------------------------------------------------------------------o
void cSkills::EnticementTarget2( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( trgChar ) )
		return;
	if( trgChar->GetNPCAiType() == aiPLAYERVENDOR || trgChar->IsInvulnerable() || trgChar->GetNPCAiType() == aiGUARD )
	{
		s->sysmessage( 1642 );
		return;
	}
	CItem *getInst = GetInstrument( s );
	if( !ValidateObject( getInst ) ) 
	{
		s->sysmessage( 1438 );
		return;
	}
	CChar *mChar		= s->CurrcharObj();
	bool checkSkill1	= CheckSkill( mChar, ENTICEMENT, 0, 1000 );
	bool checkSkill2	= CheckSkill( mChar, MUSICIANSHIP, 0, 1000 );
	if( checkSkill1 && checkSkill2 )
	{
		criminal( mChar );
		CChar *target = static_cast<CChar *>(s->TempObj());
		s->TempObj( NULL );
		target->SetFTarg( trgChar );
		target->SetNpcWander( 1 );
		s->sysmessage( 1447 );
		PlayInstrument( s, getInst, true );
	}
	else 
	{
		s->sysmessage( 1448 );
		PlayInstrument( s, getInst, false );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::ProvocationTarget2( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check 2nd Provoc Target and players Provoc and Musicianship
//|					 skill.  If either target is Innocent, flag the player criminal
//o---------------------------------------------------------------------------o
void cSkills::ProvocationTarget2( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( trgChar ) )
		return;
	
	CItem *getInst = GetInstrument( s );
	if( !ValidateObject( getInst ) ) 
	{
		s->sysmessage( 1438 );
		return;
	}

	if( trgChar->IsInvulnerable() || trgChar->GetNPCAiType() == aiPLAYERVENDOR || trgChar->GetNPCAiType() == aiGUARD ) // not invul, a player vendor, or a guard
	{
		s->sysmessage( 1730 );
		return;
	}

	CChar *target = static_cast<CChar *>(s->TempObj());
	s->TempObj( NULL );
	if( target == trgChar )
	{
		s->sysmessage( 1449 );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	bool checkMusic = CheckSkill( mChar, MUSICIANSHIP, 0, 1000 );
	if( checkMusic )
	{
		bool checkProvoc = CheckSkill( mChar, PROVOCATION, 0, 1000 );
		PlayInstrument( s, getInst, true );
		if( checkProvoc )
		{
			if( trgChar->IsInnocent() )
				criminal( mChar );
			s->sysmessage( 1450 );
		}
		else 
		{
			s->sysmessage( 1451 );
			trgChar = mChar;
		}

		target->SetTarg( trgChar );
		trgChar->SetTarg( target );
		if( target->GetVisible() == VT_TEMPHIDDEN || target->GetVisible() == VT_INVISIBLE )
			target->ExposeToView();
		if( target->IsMeditating() )
			target->SetMeditating( false );
		if( trgChar->GetVisible() == VT_TEMPHIDDEN || trgChar->GetVisible() == VT_INVISIBLE )
			trgChar->ExposeToView();
		if( trgChar->IsMeditating() )
			trgChar->SetMeditating( false );
		target->SetAttackFirst( true );
		trgChar->SetAttackFirst( false );
		target->SetAttacker( trgChar );
		trgChar->SetAttacker( target );
		if( target->IsNpc() )
		{
			if( !target->IsAtWar() ) 
				target->ToggleCombat();
			target->SetTimer( tNPC_MOVETIME, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->NPCSpeed() )) );
		}
		if( trgChar->IsNpc() )
		{
			if( !trgChar->IsAtWar() ) 
				trgChar->ToggleCombat();
			trgChar->SetTimer( tNPC_MOVETIME, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->NPCSpeed() )) );
		}
		SOCKLIST nearbyChars = FindNearbyPlayers( target );
		SOCKLIST_ITERATOR cIter;
		for( cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
		{
			(*cIter)->objMessage( 334, target, 0.0f, 0x03B2 );
		}
	}
	else
	{
		PlayInstrument( s, getInst, false );
		s->sysmessage( 1452 );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::AlchemyTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets a reagent with a mortar, then
//|					 brings up a MakeMenu for player to select which potion
//|					 to make
//o---------------------------------------------------------------------------o
void cSkills::AlchemyTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		switch( i->GetID() )
		{
			case 0x0F7B:	NewMakeMenu( s, 90, ALCHEMY );	break;	// Agility,
			case 0x0F84:	NewMakeMenu( s, 91, ALCHEMY );	break;	// Cure, Garlic
			case 0x0F8C:	NewMakeMenu( s, 92, ALCHEMY );	break;	// Explosion, Sulfurous Ash
			case 0x0F85:	NewMakeMenu( s, 93, ALCHEMY );	break;	// Heal, Ginseng
			case 0x0F88:	NewMakeMenu( s, 94, ALCHEMY );	break;	// Poison, Nightshade
			case 0x0F7A:	NewMakeMenu( s, 95, ALCHEMY );	break;	// Refresh,
			case 0x0F86:	NewMakeMenu( s, 96, ALCHEMY );	break;	// Strength,
			case 0x0F8D:	NewMakeMenu( s, 97, ALCHEMY );	break;	// Night sight
			case 0x0E9B:									break; // Mortar
			default:
				if( i->GetID() >= 0x1B11 && i->GetID() <= 0x1B1C )
				{
					MakeNecroReg( s, i, i->GetID() );
					s->sysmessage( 1453 );
					return;
				}
				s->sysmessage( 1454 );
				return;
		}
	}
}
//o---------------------------------------------------------------------------o
//|   Function    :  bool cSkills::CheckSkill( CChar *s, UI08 sk, SI16 lowSkill, SI16 highSkill )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Used to check a players skill based on the highskill and
//|					 lowskill it was called with.  If skill is < than lowskill
//|					 check will fail, but player will gain in the skill, if
//|					 the players skill is > than highskill player will not gain
//o---------------------------------------------------------------------------o
bool cSkills::CheckSkill( CChar *s, UI08 sk, SI16 lowSkill, SI16 highSkill )
{
	bool skillCheck		= false;
	UI16 scpNum			= s->GetScriptTrigger();
	cScript *tScript	= Trigger->GetScript( scpNum );
	bool exists			= false;

	if( tScript != NULL )
		exists = tScript->OnSkillCheck( s, sk, lowSkill, highSkill );	
	
	// o----------------------------------------------------------------------------o
	// | Programmer:sereg, 15 March, 2002											|
	// | Comment   :Now lets make this more readable, and even more mathematical:)	|
	// o----------------------------------------------------------------------------o
	// | 1. if a player has skill > highskill, checkskill should return true		|
	// | 2. if a player has skill < lowskill, checkskill returns false				|
	// | 3. we have statmodifiers defined in skills.dfn, now lets make it influence	|
	// |	checkskill with a certain additional bonus through the players stat.	|
	// | 4. we dont need to throw the dices for skillgain... we must do this in		|
	// |	AdvanceSkill(...), so just call it after calculation					|
	// o----------------------------------------------------------------------------o
	// | how to calculate if a skill succeeds?										|
	// | 1. we have a certain range between lowskill and highskill, on lowskill the	|
	// |	chance is 0%, on highskill 100%.										|
	// | 2. get the statmodifiers and the players stat. make 3 special dices for	|
	// |	each stat. make them a small bonus for skillsuccess						|
	// | 3. execute the 4 calculated dices one after the other by adding them		|
	// o----------------------------------------------------------------------------o

	if( !exists )
	{
		SI32 chanceskillsuccess = 0;
		
		if( ( highSkill - lowSkill ) <= 0 || !ValidateObject( s ) || s->GetSkill( sk ) <= lowSkill )
			return false;

		if( s->IsDead() )
		{
			cSocket *sSock = calcSocketObjFromChar( s );
			sSock->sysmessage( 1487 );
			return false;
		}

		if( s->GetCommandLevel() > 0 || s->GetSkill( sk ) >= highSkill )
			return true;


		chanceskillsuccess = (SI32)( (R32)( ( (R32)( s->GetSkill( sk ) - lowSkill ) / 1000.0f ) +
									 (R32)( (R32)( s->GetStrength() * cwmWorldState->skill[sk].strength ) / 100000.0f ) +
									 (R32)( (R32)( s->GetDexterity() * cwmWorldState->skill[sk].dexterity ) / 100000.0f ) +
									 (R32)( (R32)( s->GetIntelligence() * cwmWorldState->skill[sk].intelligence  ) / 100000.0f ) ) * 1000 );
		
		// chanceskillsuccess is a number between 0 and 1000, lets throw the dices now
		skillCheck = ( chanceskillsuccess >= RandomNum( 0, 1000 ) );
		
		cSocket *mSock = calcSocketObjFromChar( s );
		bool mageryUp = true;
		if( mSock != NULL )
		{
			mageryUp = ( mSock->CurrentSpellType() == 0 );
			
			if( s->GetBaseSkill( sk ) < highSkill )
			{
				if( sk != MAGERY || ( sk == MAGERY && mageryUp ) )
				{
					if( AdvanceSkill( s, sk, skillCheck ) )
					{
						updateSkillLevel( s, sk ); 
						mSock->updateskill( sk );
					}
				}
			}
		}
	}
	else
		skillCheck = true;
	return skillCheck;
}          

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Atrophy( CChar *c, UI16 sk )
//|   Date        :  Jan 29, 2000
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do atrophy for player c:
//|						find sk in our cronological list of atrophy skills, 
//|						move it to the front, check total aginst skillcap to 
//|						see if we need to lower a skill, if we do, again search 
//|						skills for a skill that can be lowered, if one is found
//|						lower it and increase sk, if we can't find one, do 
//|						nothing if atrophy is not need, increase sk.
//o---------------------------------------------------------------------------o
void cSkills::Atrophy( CChar *c, UI08 sk )
{
	UI32 a=0, ttl = 0, rem=0;
	UI16 atrop[ALLSKILLS+1];
	SI16 toDec = -1;
	UI08 counter = 0;
	cSocket *mSock = calcSocketObjFromChar( c );
	UI16 skillTrig = c->GetScriptTrigger();
	cScript *scpSkill = Trigger->GetScript( skillTrig );
		
	if( c->IsNpc() || c->GetCommandLevel() >= CNS_CMDLEVEL || mSock == NULL )	// GM's and NPC's dont atrophy
	{
		c->SetBaseSkill( c->GetBaseSkill( sk ) + 1, sk );
		if( scpSkill != NULL )
		{
			if( !scpSkill->OnSkillGain( c, sk ) )
				scpSkill->OnSkillChange( c, sk );
		}
		if( mSock != NULL )
			mSock->updateskill( sk );
		return;
	}

	srand( getclock() ); // Randomize
	
	atrop[ALLSKILLS]=0;//set the last of out copy array
	for( counter = 0; counter <= ALLSKILLS; ++counter )
	{
		atrop[counter] = c->GetAtrophy( counter );
	}
	
	for( a = ALLSKILLS; a > 0; --a )
	{//add up skills and find the one being increased
		if( c->GetBaseSkill( static_cast<UI08>(c->GetAtrophy( static_cast<UI08>(a-1) ) ))>0 && c->GetSkillLock( static_cast<UI08>(c->GetAtrophy( static_cast<UI08>(a-1)) )) == 1 && c->GetAtrophy( static_cast<UI08>(a-1) ) != sk)
			toDec = c->GetAtrophy(static_cast<UI08>(a-1));//we found a skill that can be decreased, save it for later.

		ttl += c->GetBaseSkill( static_cast<UI08>(a-1) );
		atrop[a]=atrop[a-1];
		if( atrop[a] == sk )
			rem = a;//remember this number
	}

	atrop[0] = sk;//set the first one to our current skill
	
	//copy it back in
	if( rem == ALLSKILLS )//it was last
	{
		for( counter = 0; counter <= ALLSKILLS; ++counter )
			c->SetAtrophy( atrop[counter], counter );
	} 
	else	//in the middle somewhere or first
	{
		for( counter = 0; counter < rem; ++counter )
			c->SetAtrophy( atrop[counter], counter );
		for( counter = static_cast<UI08>(rem + 1); counter <= ALLSKILLS; ++counter )
			c->SetAtrophy( atrop[counter], counter );
	}

	if( RandomNum( static_cast< UI16 >(0), cwmWorldState->ServerData()->ServerSkillCapStatus() ) <= static_cast< UI16 >(ttl ))
	{//if the rand is less than their total skills, they loose one.
		if( toDec != -1 )
		{
			c->SetBaseSkill( c->GetBaseSkill( static_cast<UI08>(toDec) ) - 1, static_cast<UI08>(toDec ));
			c->SetBaseSkill( c->GetBaseSkill( sk ) + 1, sk );
			if( scpSkill != NULL )
			{
				if( !scpSkill->OnSkillGain( c, sk ) )
					scpSkill->OnSkillChange( c, sk );
				if( !scpSkill->OnSkillLoss( c, static_cast<UI08>(toDec) ) )
					scpSkill->OnSkillChange( c, static_cast<UI08>(toDec) );
			}
			mSock->updateskill( sk );
			mSock->updateskill( static_cast<UI08>(toDec ));
		}
		return;
	} 

	c->SetBaseSkill( c->GetBaseSkill( sk ) + 1, sk );
	if( scpSkill != NULL )
	{
		if( !scpSkill->OnSkillGain( c, sk ) )
			scpSkill->OnSkillChange( c, sk );
	}
	mSock->updateskill( sk );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::CreateBandageTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player cuts cloth into bandages
//o---------------------------------------------------------------------------o
void cSkills::CreateBandageTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
	{
		s->sysmessage( 1491 );
		return;
	}
	CChar *myChar = myChar = s->CurrcharObj();
	if( !ValidateObject( myChar ) )
		return;

	CItem *c	= NULL;
	UI16 myID	= i->GetID();

	if( myID >= 0x0F95 && myID <= 0x0F9C )	// Bold of Cloth
	{
		c = Items->CreateItem( s, myChar, 0x175F, 50, 0, OT_ITEM, true );
		if( c != NULL ) 
		{
			c->SetName( Dictionary->GetEntry( 1490 ) );
			i->IncAmount( -1 );
		}
	}
	// Create RollDice 1d4 Bandages per cutted cloth
	else if( ( myID >= 0x1515 && myID <= 1518 ) || ( myID >= 0x152E && myID <= 0x1531 ) || ( myID >= 0x1537 && myID <= 0x1544 ) || ( myID >= 0x1914 && myID <= 0x1915 ) || ( myID >= 0x1EFD && myID <= 0x1F04 ) )
	{
		cDice myDice( 1, 4, 1 );
		UI32 Amount = (UI32)( myDice.roll() );

		if( i->GetCont() == myChar )
		{
			s->sysmessage( 1491 );
			return;
		}

		c = Items->CreateItem( s, myChar, 0x0E21, Amount, 0, OT_ITEM, true );
		if( c != NULL ) 
		{
			c->SetName( Dictionary->GetEntry( 1489 ) );
			i->IncAmount( -1 );
		}
	}
	// Gain 2 bandages per Folded Cloth
	else if( myID >= 0x175D && myID <= 0x1764 )
	{
		Effects->PlaySound( s, 0x0248, true );
		s->sysmessage( 1488 );
		c = Items->CreateItem( s, myChar, 0x0E21, 2, 0, OT_ITEM, true );
		if( c != NULL ) 
		{
			c->SetName( Dictionary->GetEntry( 1489 ) );
            i->IncAmount( -1 );
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::HealingSkillTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets Human NPC or other Players with
//|					 bandages.  Checks both Healing and Anatomy for success,
//|					 players with > 60 in both can cure poison (sometimes, 
//|					 dependant on poison level), players with > 80 in both can
//|					 resurrect players (once again, sometimes)
//o---------------------------------------------------------------------------o
void cSkills::HealingSkillTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i		= calcCharObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar	= s->CurrcharObj();
	CItem *mItem	= static_cast<CItem *>(s->TempObj());
	s->TempObj( NULL );
	if( ValidateObject( i ) )
	{
		if( !objInRange( mChar, i, DIST_NEXTTILE ) )
		{
			s->sysmessage( 1498 );
			return;
		}
		if( i->IsDead() )
		{
			if( mChar->GetBaseSkill( HEALING ) >= 800 && mChar->GetBaseSkill( ANATOMY ) >= 800 )
			{
				if( i->IsMurderer() )
					criminal( mChar );
				if( !CheckSkill( mChar, HEALING, 800, 1000 ) || !CheckSkill( mChar, ANATOMY, 800, 1000 ) )	// failed either healing or anat check
				{
					s->sysmessage( 1492 );
					mItem->IncAmount( -1 );
					return;
				}
				Effects->tempeffect( mChar, i, 23, HEALING, 0, 0, mItem );	// sets up timer for resurrect
				return;
			}
			s->sysmessage( 1493 );
			return;
		}

		if( i->GetPoisoned() > 0 )
		{
			if( mChar->GetBaseSkill( HEALING ) >= 600 && mChar->GetBaseSkill( ANATOMY ) >= 600 )
			{
				if( i->IsMurderer() )
					criminal( mChar );
				if( !CheckSkill( mChar, HEALING, 600, 1000 ) || !CheckSkill( mChar, ANATOMY, 600, 1000 ) )
				{
					s->sysmessage( 1494 );
					mItem->IncAmount( -1 );
					return;
				}
				Effects->tempeffect( mChar, i, 24, HEALING, 0, 0, mItem );
				return;
			}
			else
			{
				s->sysmessage( 1495 );
				s->sysmessage( 1496 );
			}
			return;
		}

		if( i->GetHP() == i->GetStrength() )
		{
			s->sysmessage( 1497 );
			return;
		}

		UI08 targetSkill;
		if( i->GetID() == 0x0190 || i->GetID() == 0x0191 ) //Used on human
			targetSkill = HEALING;
		else
			targetSkill = VETERINARY;

		if( i->IsMurderer() )
			criminal( mChar );
		if( !CheckSkill( mChar, targetSkill, 0, 1000 ) )
		{
			if( targetSkill == HEALING )
			{
				i->IncHP( 1 );
				s->sysmessage( 1499 );
			}
			else
				s->sysmessage( 1500 );
			return;
		}
		Effects->tempeffect( mChar, i, 22, targetSkill, 0, 0, mItem );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::SpiritSpeak( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses spirit speak skill.. Translates a
//|					 ghosts normal oOoOooo text into readable text (what he
//|					 is actually saying), Operates on a timer based on settings
//|					 in server.scp
//o---------------------------------------------------------------------------o
void cSkills::SpiritSpeak( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !CheckSkill( mChar, SPIRITSPEAK, 0, 1000 ) )
	{
		s->sysmessage( 1501 );
		return;
	}
	
	Effects->PlaySpellCastingAnimation( mChar, 0x11 );
	Effects->PlaySound( s, 0x024A, true );
	s->sysmessage( 1502 );
	
	s->SetTimer( tPC_SPIRITSPEAK, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->SystemTimer( SPIRIT_SPEAK ) + mChar->GetSkill( SPIRITSPEAK ) / 10 + mChar->GetIntelligence() )) ); // spirit speak duration
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::ArmsLoreTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses ArmsLore on an item (Only works for
//|					 weapons and armor (Items with Defense or HiDamage/LoDamage
//o---------------------------------------------------------------------------o
void cSkills::ArmsLoreTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	UnicodeTypes sLang = s->Language();
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;
	SI32 offset;
	char temp2[60], temp[1024];
	CChar *mChar = s->CurrcharObj();
	if( i->isPileable() || ( i->GetDef() == 0 && ( i->GetLoDamage() == 0 && 
		i->GetHiDamage() == 0 ) && ( i->GetRank() < 1 || i->GetRank() > 9 ) ) )
	{
		s->sysmessage( 1503 );
		return;
	}
	if( mChar->IsGM() )
	{
		s->sysmessage( 1731, i->GetDef(), i->GetLoDamage(), i->GetHiDamage(), i->GetPoisoned(), i->GetRank() );
		return;
	}
	if( CheckSkill( mChar, ARMSLORE, 0, 1000 ) )
	{
		if( i->GetMaxHP() )
		{
			strcpy( temp, Dictionary->GetEntry( 1731, sLang ).c_str() );
			// Items HP's - 1 / by items total HP * 10 (0-3 = 0, 4 - 5 = 1, ect)
			offset = static_cast<SI32>(((R32)(( i->GetHP() - 1) / i->GetMaxHP() ) * 10 ));
			if( offset >= 0 && offset <= 8 )
				strcpy( temp2, Dictionary->GetEntry( 1515 - offset, sLang ).c_str() );
			else
				strcpy( temp2, Dictionary->GetEntry( 1506, sLang ).c_str() );

			strcat( temp, temp2 );
		}
		else
			s->sysmessage( 1505 );
		if( i->GetHiDamage() )
		{
			if( mChar->GetSkill( ARMSLORE ) > 750 && i->GetPoisoned() > 0 )
			{
				offset = i->GetPoisoned();
				if( offset > 0 && offset < 5 )
					strcpy( temp2, Dictionary->GetEntry( 1455 + offset, sLang ).c_str() );
				else
					strcpy( temp2, Dictionary->GetEntry( 1459, sLang ).c_str() );
				strcat( temp, temp2 );
			}
			// HiDamage + LoDamage / 10 ( 0-9 = 0, 10-19 = 1, ect )
			offset = (i->GetHiDamage() + i->GetLoDamage() ) / 10;
			if( offset <= 5 )
				strcpy( temp2, Dictionary->GetEntry( 1522 - offset, sLang ).c_str() );
			else
				strcpy( temp2, Dictionary->GetEntry( 1516, sLang ).c_str() );
			strcat( temp, temp2 );
			
			if( mChar->GetSkill( ARMSLORE ) > 250 )
			{
				// Items Speed - 5 / 10 ( 0-14 = 0, 15-25 = 1, ect)
				offset = ((i->GetSpeed() - 5) / 10);
				if( offset <= 2 )
					strcpy( temp2, Dictionary->GetEntry( 1526 - offset, sLang ).c_str() );
				else
					strcpy( temp2, Dictionary->GetEntry( 1523, sLang ).c_str() );
				strcat( temp, temp2 );
			}
		}
		else if( i->GetDef() )
		{
			// Items Defense + 1 / 2 ( 0 = 0, 1-2 = 1, 3-4 = 2, ect)
			offset = ((i->GetDef() + 1) / 2);

			if( offset <= 6 )
				strcpy( temp2, Dictionary->GetEntry( 1534 - offset, sLang ).c_str() );
			else
				strcpy( temp2, Dictionary->GetEntry( 1527, sLang ).c_str() );

			strcat( temp, temp2 );
		}
		s->sysmessage( temp );
		if( mChar->GetSkill( ARMSLORE ) > 250 && cwmWorldState->ServerData()->RankSystemStatus() )
		{
			offset = i->GetRank();
			if( offset >= 0 && offset <= 10 )
				s->sysmessage( 1534 + offset );
		}
	}
	else
		s->sysmessage( 1504 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::ItemIDTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses the ItemID skill on an item, can
//|					 give valuable information on items (Will reveal hidden
//|					 magical names and charges on items as well)
//o---------------------------------------------------------------------------o
void cSkills::ItemIDTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;
	if( i->isCorpse() )
	{
		s->sysmessage( 1546 );
		return;
	}

	CChar *mChar = s->CurrcharObj();
	if( CheckSkill( mChar, ITEMID, 250, 500 ) )
	{
		UnicodeTypes sLang = s->Language();

		std::string name;
		name.reserve( MAX_NAME );
		if( i->GetName2() && strcmp( i->GetName2(), "#" ) ) 
			i->SetName( i->GetName2() );
		if( i->GetName()[0] == '#') 
			getTileName( i, name );
		else 
			name = i->GetName();
		s->sysmessage( 1547, name.c_str() );
		
		char temp[1024];
		if( i->GetCreator() != INVALIDSERIAL )
		{
			CChar *mCreater = calcCharObjFromSer( i->GetCreator() );
			if( ValidateObject( mCreater ) )
			{
				if( i->GetMadeWith() > 0 ) 
					sprintf( temp, Dictionary->GetEntry( 1548, sLang ).c_str(), cwmWorldState->skill[i->GetMadeWith()-1].madeword.c_str(), mCreater->GetName().c_str() );
				else if( i->GetMadeWith() < 0 )
					sprintf( temp, Dictionary->GetEntry( 1548, sLang ).c_str(), cwmWorldState->skill[0-i->GetMadeWith() - 1].madeword.c_str(), mCreater->GetName().c_str() );
				else
					sprintf( temp, Dictionary->GetEntry( 1549, sLang ).c_str(), mCreater->GetName().c_str() );
		}
		else
			strcpy( temp, Dictionary->GetEntry( 1550, sLang ).c_str() );
		}
		else
			strcpy( temp, Dictionary->GetEntry( 1550, sLang ).c_str() );
		s->sysmessage( temp );
		
		if( mChar->GetSkill( ITEMID ) > 350 )
		{
			if( i->GetType() != IT_MAGICWAND )
			{
				s->sysmessage( 1553 );
				return;
			}
			if( CheckSkill( mChar, ITEMID, 500, 750 ) )
			{
				UI16 spellToScan = static_cast<UI16>(( 8 * ( i->GetTempVar( CITV_MOREX ) - 1 ) ) + i->GetTempVar( CITV_MOREY ) - 1);
				if( !CheckSkill( mChar, ITEMID, 750, 1000 ) )
					s->sysmessage( 1555, magic_table[spellToScan].spell_name );
				else
					s->sysmessage( 1556, magic_table[spellToScan].spell_name, i->GetTempVar( CITV_MOREZ ) );
			}
			else
				s->sysmessage( 1554 );
		}
		else
			s->sysmessage( 1552 );
	}
	else
		s->sysmessage( 1545 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::EvaluateIntTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses EvalInt on a PC/NPC, gives
//|					 a text message based on their Intelligence
//o---------------------------------------------------------------------------o
void cSkills::EvaluateIntTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;
	char buf[75];
	CChar *mChar = s->CurrcharObj();
	if( CheckSkill( mChar, EVALUATINGINTEL, 0, 1000 ) ) 
	{
		if( i->GetIntelligence() == 0 )
			s->sysmessage( 1557 );
		else
		{
			// Intelligence - 1 / 10 (0-10 = 0, 11-20 = 1, ect)
			int offset = ((i->GetIntelligence() - 1 ) / 10);

			if( offset >= 0 && offset <= 8 )
				strcpy( buf, Dictionary->GetEntry( 1558 + offset, s->Language() ).c_str() );
			else
				strcpy( buf, Dictionary->GetEntry( 1567, s->Language() ).c_str() );

			s->sysmessage( 1568, buf );
		}
	}
	else
		s->sysmessage( 1504 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::AnatomyTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses Anatomy skill on a PC/NPC, gives
//|					 a text message based on their Strength and Dexterity
//o---------------------------------------------------------------------------o
void cSkills::AnatomyTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
	{
		s->sysmessage( 1569 );
		return;
	}
	char buf[125];
	char buf2[125];
	CChar *mChar = s->CurrcharObj();
	if( !objInRange( i, mChar, cwmWorldState->ServerData()->CombatMaxRange() ) )
	{
		s->sysmessage( 1570 );
		return;
	}
	
	if( i->IsDead() )
	{
		s->sysmessage( 1571 );
		return;
	}
	if( CheckSkill( mChar, ANATOMY, 0, 1000 ) ) 
	{
		UnicodeTypes sLang = s->Language();
		SI16 offset;

		// Strength - 1 / 10 (0-10 = 0, 11-20 = 1, ect)
		offset = ((i->GetStrength() - 1 )/ 10);

		if( offset >= 0 && offset <= 8 )
			strcpy( buf, Dictionary->GetEntry( 1572 + offset, sLang ).c_str() );
		else
			strcpy( buf, Dictionary->GetEntry( 1581, sLang ).c_str() );


		// Dexterity - 1 /10 (0-10 = 0, 11-20 = 1, ect)
		offset = ((i->GetDexterity() - 1 )/ 10);

		if( offset >= 0 && offset <= 8 )
			strcpy( buf2, Dictionary->GetEntry( 1582 + offset, sLang ).c_str() );
		else
			strcpy( buf2, Dictionary->GetEntry( 1591, sLang ).c_str() );

		s->sysmessage( 1592, buf, buf2 );
	}
	else
		s->sysmessage( 1504 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::TameTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//|									
//|	Modification	-	09/22/2002	-	Xuri - Changed sysmessage at end of taming 
//|									function to say "You are too far away" instead of 
//|									"You can't tame that!" Easier for the users to understand 
//|									what's wrong when they can't tame due to being too far away...
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player attempts to tame an NPC
//o---------------------------------------------------------------------------o
void cSkills::TameTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetByte( 7 ) == 0xFF ) 
		return;
	
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;
	CChar *mChar = s->CurrcharObj();
	if( i->IsNpc() && objInRange( mChar, i, DIST_NEARBY ) )
	{
		if( i->GetTaming() > 1000 || i->GetTaming() == 0 )
		{
			s->sysmessage( 1593 );
			return;
		}
		if( i->IsTamed() && i->GetOwnerObj() == mChar )
		{
			s->sysmessage( 1594 );
			return;
		}
		if( i->IsTamed() )
		{
			s->sysmessage( 1595 );
			return;
		}
		for( UI08 a = 0; a < 3; ++a )
		{
			switch( RandomNum( 0, 3 ) )
			{
				case 0: mChar->talkAll( 1597, false );	break;
				case 1: mChar->talkAll( 1598, false );	break;
				case 2: 
					mChar->talkAll( 1599, false, i->GetName().c_str() ); 
					break;
				case 3: 
					mChar->talkAll( 1600, false, i->GetName().c_str() ); 
					break;
			}
		}
		if( i->GetHunger() < 0 || mChar->GetSkill( TAMING ) < i->GetTaming() || !CheckSkill( mChar, TAMING, 0, 1000 ) ) 
		{
			s->sysmessage( 1601 );
			return;
		}   
		mChar->talk( s, 1602, false );
		i->SetOwner( mChar );
		i->SetNpcWander( aiNOAI );
		i->SetNPCAiType( 0 );
		i->SetTamed( true );
		if( i->IsAtWar() && i->GetTarg() == mChar )
		{
			i->SetTarg( NULL );
			i->SetAttacker( NULL );
			if( i->IsAtWar() )
				i->ToggleCombat();
			if( mChar->IsAtWar() && mChar->GetTarg() == i )
			{
				mChar->SetTarg( NULL );
				mChar->SetAttacker( NULL );
				if( mChar->IsAtWar() )
					mChar->ToggleCombat();
			}
		}
	}
	else // Sept 22, 2002 - Xuri
		s->sysmessage( 400 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::FishTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets an area with a fishing pole
//o---------------------------------------------------------------------------o
void cSkills::FishTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	CChar *mChar = s->CurrcharObj();

	SI16 targetX = s->GetWord( 0x0B );
	SI16 targetY = s->GetWord( 0x0D );
	SI08 targetZ = s->GetByte( 0x10 );
	SI16 distX = abs( mChar->GetX() - targetX );
	SI16 distY = abs( mChar->GetY() - targetY );

	UI08 targetID1 = s->GetByte( 0x11 );
	UI08 targetID2 = s->GetByte( 0x12 );

	CItem *targetItem = calcItemObjFromSer( s->GetDWord( 7 ) );
	bool validLocation = false;
	if( ValidateObject( targetItem ) )
		validLocation = Map->IsTileWet( targetItem->GetID() );
	else if( targetID1 != 0 && targetID2 != 0 )
	{
		MapStaticIterator msi( targetX, targetY, mChar->WorldNumber() );
		CTile tile;
		staticrecord *stat = NULL;
		while( ( ( stat = msi.Next() ) != NULL ) && !validLocation )
		{
			msi.GetTile(&tile);
			if( targetZ == stat->zoff && tile.LiquidWet() )	// right place, and wet
				validLocation = true;
		}
	}
	else		// or it could be a map only
	{  
		// manually calculating the ID's if a maptype
		map_st map1;
		CLand land;
		map1 = Map->SeekMap0( targetX, targetY, mChar->WorldNumber() );
		Map->SeekLand( map1.id, &land );
		if( land.LiquidWet() )
			validLocation = true; 
	}
	if( validLocation )
	{
		if( distX > 5 || distY > 5 )
		{
			s->sysmessage( 843 );
			return;
		}
		if( mChar->GetZ() < targetZ )
		{
			s->sysmessage( 844 );
			return;
		}
		if( mChar->GetStamina() - 2 <= 2 )
		{
			s->sysmessage( 845 );
			return;
		}
		mChar->SetStamina( mChar->GetStamina() - 2 );
		Effects->PlayCharacterAnimation( mChar, 0x0b );
		R32 baseTime;
		baseTime = static_cast<R32>(cwmWorldState->ServerData()->SystemTimer( BASE_FISHING ) / 25);
		baseTime += RandomNum( 0, static_cast< int >(cwmWorldState->ServerData()->SystemTimer( RANDOM_FISHING ) / 15) );
		s->SetTimer( tPC_FISHING, BuildTimeValue( baseTime ) ); //2x faster at war and can run
		Effects->PlaySound( s, 0x023F, true );
	}
	else
		s->sysmessage( 846 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Fish( CChar *i )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Fish up items based on skill and random values (this should
//|					 be rewritten using a resource system akin to Ore and Logs)
//o---------------------------------------------------------------------------o
void cSkills::Fish( CChar *i )
{
	cSocket *s = calcSocketObjFromChar( i );
	if( !CheckSkill( i, FISHING, 0, 1000 ) ) 
	{
		s->sysmessage( 847 );
		return;
	}
	UI16 getSkill = i->GetSkill( FISHING );
	switch( RandomNum( 0, 25 ) )
	{
		case 1:
			if( getSkill > 920 )
			{
				Items->CreateRandomItem( s, "fishtreasure" );	// random paintings
				s->sysmessage( 848 );
			}
			break;
		case 2:
			if( getSkill > 970 )
			{
				Items->CreateRandomItem( s, "fishweapons" );	// Some new weapons
				s->sysmessage( 849 );
			}
			break;
		case 3:	// Random gold and gems
			if( RandomNum( 0, 12 ) )
			{
				Items->CreateRandomItem( s, "fishgems" ); 
				s->sysmessage( 850 );
			}
			else
			{	// Create between 200 and 1300 gold
				UI16 nAmount = RandomNum( 200, 1300 );
				Items->CreateItem( s, i, 0x0EED, nAmount, 0, OT_ITEM, true );
				Effects->goldSound( s, nAmount );
				s->sysmessage( 851, nAmount );
			}
			break;
		case 4:
			if( getSkill > 850 )
			{
				Items->CreateRandomItem( s, "fishbones" );	// Random bones and crap
				s->sysmessage( 852 );
			}
			break;
		default:
			Items->CreateRandomItem( s, "randomfish" );	// User defined fish
			s->sysmessage( 853 );
			break;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::SkillUse( cSocket *s, UI08 x )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses a skill from the skill list
//o---------------------------------------------------------------------------o
void cSkills::SkillUse( cSocket *s, UI08 x )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( mChar->IsDead() )
	{
		s->sysmessage( 392 );
		return;
	}
	if( ( x != STEALTH && mChar->GetVisible() == VT_TEMPHIDDEN ) || mChar->GetVisible() == VT_INVISIBLE )
		mChar->ExposeToView();
	mChar->BreakConcentration( s );
	if( mChar->GetTimer( tCHAR_SPELLTIME ) != 0 || mChar->IsCasting() )
	{
		s->sysmessage( 854 );
		return;
	}
	if( s->GetTimer( tPC_SKILLDELAY ) <= cwmWorldState->GetUICurrentTime() || mChar->IsGM() )
	{
		cScript *skScript = Trigger->GetScript( mChar->GetScriptTrigger() );
		bool doSwitch = true;
		if( skScript != NULL )
			doSwitch = !skScript->OnSkill( mChar, x );
		if( doSwitch )
		{
			switch( x )
			{
				case ARMSLORE:			s->target( 0, TARGET_ARMSLORE, 855 );		break;
				case ANATOMY:			s->target( 0, TARGET_ANATOMY, 856 );		break;
				case ITEMID:			s->target( 0, TARGET_ITEMID, 857 );			break;
				case EVALUATINGINTEL:	s->target( 0, TARGET_EVALINT, 858 );		break;
				case TAMING:			s->target( 0, TARGET_TAME, 859 );			break;
				case HIDING:			Hide( s );									break;
				case STEALTH:			Stealth( s );								break;
				case DETECTINGHIDDEN:	s->target( 0, TARGET_DETECTHIDDEN, 860 );	break;
				case PEACEMAKING:		PeaceMaking(s);								break;
				case PROVOCATION:		s->target( 0, TARGET_PROVOCATION, 861 );	break;
				case ENTICEMENT:		s->target( 0, TARGET_ENTICEMENT, 862 );		break;
				case SPIRITSPEAK:		SpiritSpeak(s);								break;
				case STEALING:
					if( cwmWorldState->ServerData()->RogueStatus() )
						s->target( 0, TARGET_STEALING, 863 );
					else
						s->sysmessage( 864 );
					break;
				case INSCRIPTION:		s->target( 0, TARGET_INSCRIBE, 865 );		break;
				case TRACKING:			TrackingMenu( s, TRACKINGMENUOFFSET );		break;
				case BEGGING:			s->target( 0, TARGET_BEGGING, 866 );		break;
				case ANIMALLORE:		s->target( 0, TARGET_ANIMALLORE, 867 );		break;
				case FORENSICS:			s->target( 0, TARGET_FORENSICS, 868 );		break;
				case POISONING:			s->target( 0, TARGET_APPLYPOISON, 869 );	break;
				case MEDITATION:
					if( cwmWorldState->ServerData()->SystemTimer( ARMORAFFECTMANA_REGEN ) )
						Meditation( s );
					else 
						s->sysmessage( 870 );
					break;
				default:				s->sysmessage( 871 );					break;
			}
		}
		s->SetTimer( tPC_SKILLDELAY, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->ServerSkillDelayStatus() )) );
		return;
	}
	else
		s->sysmessage( 872 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::RandomSteal( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets a PC/NPC with stealing skill
//|					 instead of an item (randomly pics an item in their pack)
//o---------------------------------------------------------------------------o
void cSkills::RandomSteal( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	CChar *npc = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( npc ) ) 
		return;

	CItem *p = npc->GetPackItem();
	if( !ValidateObject( p ) ) 
	{
		s->sysmessage( 875 ); 
		return; 
	}

	CItem *item = NULL;
	for( UI08 i = 0; i < 50; ++i )
	{
		item = p->GetItemObj( RandomNum( static_cast< size_t >(0), p->NumItems() - 1 ) );
		if( ValidateObject( item ) ) 
			break;
	} 
	if( !ValidateObject( item ) )
	{
		s->sysmessage( 876 );
		return;
	}
	doStealing( s, mChar, npc, item );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::StealingTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets an item with stealing skill
//o---------------------------------------------------------------------------o
void cSkills::StealingTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( s->GetByte( 7 ) < 0x40 )
	{
		RandomSteal( s );
		return;
	}
	CItem *item = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( item ) )
		return;

	CChar *npc = FindItemOwner( item );
	if( !ValidateObject( npc ) )
		return;

	if( item->GetCont() == npc || item->GetCont() == NULL || item->isNewbie() )
	{
		s->sysmessage( 874 );
		return;
	}
	doStealing( s, mChar, npc, item );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::doStealing( cSocket *s, CChar *mChar, CChar *npc, CItem *item )
//|   Date        :  2/13/2003
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do the bulk of stealing stuff rather than having the same
//|						code twice in RandomSteal() and StealingTarget()
//o---------------------------------------------------------------------------o
void cSkills::doStealing( cSocket *s, CChar *mChar, CChar *npc, CItem *item )
{
	VALIDATESOCKET( s );
	if( npc == mChar ) 
	{
		s->sysmessage( 873 );
		return;
	}
	if( npc->GetNPCAiType() == aiPLAYERVENDOR )
	{
		s->sysmessage( 874 );
		return;
	}
	CItem *itemCont = (CItem *)item->GetCont(); 
	if( itemCont != NULL && itemCont->GetLayer() >= IL_BUYCONTAINER && itemCont->GetLayer() <= IL_SELLCONTAINER ) // is it in the buy or sell layer of a vendor?
	{
		s->sysmessage( 874 );
		return;
	}
	s->sysmessage( 877, npc->GetName().c_str(), item->GetName().c_str() );
	if( objInRange( mChar, npc, DIST_NEXTTILE ) )
	{
		if( Combat->calcDef( mChar, 0, false ) > 40 )
		{
			s->sysmessage( 1643 );
			return;
		}
		SI16 stealCheck = calcStealDiff( mChar, item );
		if( stealCheck == -1 )
		{
			s->sysmessage( 1551 );
			return;
		}

		if( mChar->GetCommandLevel() < npc->GetCommandLevel() )
		{
			s->sysmessage( 878 );
			return;
		}
		if( item->isNewbie() )
		{
			s->sysmessage( 879 );
			return;
		}
		
		int getDefOffset	= UOX_MIN( stealCheck + ( (int)( ( Combat->calcDef( mChar, 0, false ) - 1) / 10 ) * 100 ), 990 );
		bool canSteal		= CheckSkill( mChar, STEALING, getDefOffset, 1000);
		if( canSteal )
		{
			CItem *pack = mChar->GetPackItem();
			item->SetCont( pack );
			s->sysmessage( 880 );

			UI16 targTrig		= item->GetScriptTrigger();
			cScript *toExecute	= Trigger->GetScript( targTrig );
			if( toExecute != NULL )
				toExecute->OnSteal( mChar, item );

			targTrig	= npc->GetScriptTrigger();
			toExecute	= Trigger->GetScript( targTrig );
			if( toExecute != NULL )
				toExecute->OnStolenFrom( mChar, npc, item );
		} 
		else 
			s->sysmessage( 881 );
		
		if( ( !canSteal && RandomNum( 1, 5 ) == 5 ) || mChar->GetSkill( STEALING ) < RandomNum( 0, 1001 ) )
		{//Did they get caught? (If they fail 1 in 5 chance, other wise their skill away from 1000 out of 1000 chance)
			s->sysmessage( 882 );
			if( npc->IsNpc() ) 
				npc->talkAll( 883, false );
			
			if( Combat->WillResultInCriminal( mChar, npc ) )
				criminal( mChar );
			char temp2[512], temp[512];

			if( item->GetName()[0] != '#' )
			{
				sprintf( temp, Dictionary->GetEntry( 884 ).c_str(), mChar->GetName().c_str(), item->GetName().c_str() );
				sprintf( temp2, Dictionary->GetEntry( 885 ).c_str(), mChar->GetName().c_str(), item->GetName().c_str(), npc->GetName().c_str() );
			} 
			else 
			{
				std::string tileName;
				tileName.reserve( MAX_NAME );
				getTileName( item, tileName );
				sprintf( temp, Dictionary->GetEntry( 884 ).c_str(), mChar->GetName().c_str(), tileName.c_str() );
				sprintf( temp2, Dictionary->GetEntry( 885 ).c_str(), mChar->GetName().c_str(), tileName.c_str(), npc->GetName().c_str() );
			}

			cSocket *npcSock = calcSocketObjFromChar( npc );
			if( npcSock != NULL )
				npcSock->sysmessage( temp );

			SOCKLIST nearbyChars = FindNearbyPlayers( mChar );
			SOCKLIST_ITERATOR cIter;
			for( cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
			{
				cSocket *iSock = (*cIter);
				CChar *iChar = iSock->CurrcharObj();
				if( iSock != s && ( RandomNum( 10, 20 ) == 17 || ( RandomNum( 0, 1 ) == 1 && iChar->GetIntelligence() >= mChar->GetIntelligence() ) ) )
					iSock->sysmessage( temp2 );
			}
		}
	} 
	else 
		s->sysmessage( 886 );
}
//o---------------------------------------------------------------------------o
//|   Function    :  SI16 calcStealDiff( CChar *c, CItem *i )
//|   Date        :  Unknown
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check if item is able to be stolen based on item type and
//|					 weight vs stealing skill, then return the min skill needed to
//|					 steal the item.
//o---------------------------------------------------------------------------o
SI16 cSkills::calcStealDiff( CChar *c, CItem *i )
{
	if( i->IsLockedDown() )
		return -1;

	SI16 stealDiff = -1;
	SI32 calcDiff, itemWeight;
	switch( i->GetType() )
	{
		case IT_SPELLBOOK:		// Spellbook
		case IT_SPAWNCONT:	// Item spawn container
		case IT_LOCKEDSPAWNCONT:	// Locked spawn container
		case IT_UNLOCKABLESPAWNCONT:	// Unlockable item spawn container
		case IT_TRASHCONT:	// Trash container
			break;
		case IT_CONTAINER:		// Backpack
		default:
			if( i->GetID() == 0x14F0 ) // Deeds
				break;

			itemWeight = i->GetWeight();
			calcDiff = (SI32)((c->GetSkill( STEALING ) * 2) + 100);  // GM thieves can steal up to 21 stones, newbie only 1 stone
			if( calcDiff > itemWeight )
				stealDiff = (SI16)UOX_MAX( UOX_MIN( ((int)((itemWeight + 9) / 20) * 10), 990 ), 0 );
			break;
	}
	return stealDiff;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Tracking( cSocket *s, int selection )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Start tracking selected NPC/PC
//o---------------------------------------------------------------------------o
void cSkills::Tracking( cSocket *s, int selection )
{
	VALIDATESOCKET( s );
	CChar *i = s->CurrcharObj();
	i->SetTrackingTarget( i->GetTrackingTargets( selection ) ); // sets trackingtarget that was selected in the gump
	s->SetTimer( tPC_TRACKING, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->TrackingBaseTimer() * i->GetSkill( TRACKING ) / 1000 + 1 )) ); // tracking time in seconds ... gm tracker -> basetimer + 1 seconds, 0 tracking -> 1 sec, new calc by LB
	s->SetTimer( tPC_TRACKINGDISPLAY, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->TrackingRedisplayTime() ) ));
	if( ValidateObject( i->GetTrackingTarget() ) )
		s->sysmessage( 1644, i->GetTrackingTarget()->GetName().c_str() );
	Track( i );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::CreateTrackingMenu( cSocket *s, int m )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Brings up Tracking Menu, Called when player uses Tracking Skill
//o---------------------------------------------------------------------------o
void cSkills::CreateTrackingMenu( cSocket *s, UI16 m )
{
	VALIDATESOCKET( s );
	UString sect = "TRACKINGMENU " + UString::number( m );
	ScriptSection *TrackStuff = FileLookup->FindEntry( sect, menus_def );
	if( TrackStuff == NULL )
		return;

	UI16 id					= 0;
	UI16 id1				= 62;
	UI16 id2				= 399;
	UI16 icon				= 8404; 
	CChar *c				= s->CurrcharObj();
	SI32 type				= 887;
	UI32 MaxTrackingTargets = 0;
	UI32 distance			= cwmWorldState->ServerData()->TrackingBaseRange() + c->GetSkill( TRACKING ) / 50;
	UnicodeTypes mLang		= s->Language();
	
	if( m == ( 2 + TRACKINGMENUOFFSET ) )
	{
		id1		= 1;
		id2		= 61;
		icon	= 0x20D1;
		type	= 888;
	}
	if( m == ( 3 + TRACKINGMENUOFFSET ) )
	{
		id1		= 400;
		id2		= 402;
		icon	= 8454;
		type	= 889;
	}

	std::string line;
	CPOpenGump toSend( *c );
	if( m >= TRACKINGMENUOFFSET )
		toSend.GumpIndex( m );
	else
		toSend.GumpIndex( m + TRACKINGMENUOFFSET );
	UString tag		= TrackStuff->First();
	UString data	= TrackStuff->GrabData();

	line = tag + " " + data;
	toSend.Question( line );

	REGIONLIST nearbyRegions = MapRegion->PopulateList( c );
	REGIONLIST_ITERATOR rIter;
	for( rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		SubRegion *MapArea = (*rIter);
		if( MapArea == NULL )	// no valid region
			continue;
		MapArea->PushChar();
		for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
		{
			if( !ValidateObject( tempChar ) )
				continue;
			id					= tempChar->GetID();
			cSocket *tSock		= calcSocketObjFromChar( tempChar );
			bool cmdLevelCheck	= isOnline( tempChar ) && ( c->GetCommandLevel() > tempChar->GetCommandLevel() );
			if( objInRange( tempChar, c, static_cast<UI16>(distance) ) && !tempChar->IsDead() && id >= id1 && id <= id2 && tSock != s && ( cmdLevelCheck || tempChar->IsNpc() ) )
			{
				c->SetTrackingTargets( tempChar, static_cast<UI08>(MaxTrackingTargets) );
				++MaxTrackingTargets;
				if( MaxTrackingTargets >= cwmWorldState->ServerData()->TrackingMaxTargets() ) 
				{
					MapArea->PopChar();
					return;
				}
				SI32 dirMessage = 898;
				switch( TrackingDirection( s, tempChar ) )
				{
					case NORTH:		dirMessage = 890;	break;
					case NORTHWEST:	dirMessage = 891;	break;
					case NORTHEAST:	dirMessage = 892;	break;
					case SOUTH:		dirMessage = 893;	break;
					case SOUTHWEST:	dirMessage = 894;	break;
					case SOUTHEAST:	dirMessage = 895;	break;
					case WEST:		dirMessage = 896;	break;
					case EAST:		dirMessage = 897;	break;
					default:		dirMessage = 898;	break;
				}
				line = tempChar->GetName() + " " + Dictionary->GetEntry( dirMessage, mLang );
				toSend.AddResponse( cwmWorldState->creatures[id].Icon(), 0, line );
			}
		}
		MapArea->PopChar();
	}
	
	if( MaxTrackingTargets == 0 )
	{
		s->sysmessage( type );
		return;
	}
	toSend.Finalize();
	s->Send( &toSend );
}

void HandleCommonGump( cSocket *mSock, ScriptSection *gumpScript, UI16 gumpIndex );
//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::TrackingMenu( cSocket *s, int gmindex )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Unknown
//o---------------------------------------------------------------------------o
void cSkills::TrackingMenu( cSocket *s, UI16 gmindex )
{
	VALIDATESOCKET( s );
	UString sect				= "TRACKINGMENU " + UString::number( gmindex );
	ScriptSection *TrackStuff	= FileLookup->FindEntry( sect, menus_def );
	if( TrackStuff == NULL )
		return;
	HandleCommonGump( s, TrackStuff, gmindex );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Track( CChar *i )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Tracking stuff for older client
//o---------------------------------------------------------------------------o
void cSkills::Track( CChar *i )
{
	cSocket *s = calcSocketObjFromChar( i );
	VALIDATESOCKET( s );
	CChar *trackTarg = i->GetTrackingTarget();
	if( !ValidateObject( trackTarg ) || trackTarg->GetY() == -1 ) 
		return;
	CPTrackingArrow tSend = (*trackTarg);
	tSend.Active( 1 );
	s->Send( &tSend );
}

//o---------------------------------------------------------------------------o
//|   Function    :  UI08 cSkills::TrackingDirection( cSocket *s, CChar *i )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Get location of the tracking target based on your loc
//o---------------------------------------------------------------------------o
UI08 cSkills::TrackingDirection( cSocket *s, CChar *i )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::TrackingDirection() Invalid socket for character 0x%X", i->GetSerial() );
		return 0;
	}
	UI08 direction = 5;
	CChar *j = s->CurrcharObj();
	if( ( j->GetY() - direction ) >= i->GetY() )  // North
	{
		if( ( j->GetX() - direction ) > i->GetX() )
			return NORTHWEST;
		if( ( j->GetX() + direction ) < i->GetX() )
			return NORTHEAST;
		return NORTH;
	}
	else if( ( j->GetY() + direction ) <= i->GetY() )  // South
	{
		if( ( j->GetX() - direction ) > i->GetX() )
			return SOUTHWEST;
		if( ( j->GetX() + direction ) < i->GetX() )
			return SOUTHEAST;
		return SOUTH;
	}
	else if( ( j->GetX() - direction ) >= i->GetX() )  // West
		return WEST;
	else if( ( j->GetX() + direction ) <= i->GetX() )  // East
		return EAST;
	else 
		return NORTH;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::BeggingTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets PC/NPC with the begging skill
//o---------------------------------------------------------------------------o
void cSkills::BeggingTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *targChar = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( targChar ) )
		return;

	if( !targChar->IsNpc() )
	{
		s->sysmessage( 899 );
		return;
	}
	if( targChar->GetNPCAiType() == aiPLAYERVENDOR )
	{
		s->sysmessage( 900 );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	if( !objInRange( targChar, mChar, cwmWorldState->ServerData()->BeggingRange() ) )
	{
		s->sysmessage( 901 );
		return;
	}
	if( targChar->GetID() == 0x0190 || targChar->GetID() == 0x0191 )
	{
		UI32 bankGold		= GetBankCount( mChar, 0x0EED, 0 );
		UI32 currentGold	= GetItemAmount( mChar, 0x0EED );
		UI32 petGold		= 0;
		CHARLIST *myPets	= mChar->GetPetList();
		// Do the STL
		CHARLIST_ITERATOR I;
		for( I = myPets->begin(); I != myPets->end(); ++I )
		{
			CChar *pet = (*I);
			if( ValidateObject( pet ) )
				petGold += GetItemAmount( pet, 0x0EED );
		}
		if( bankGold + currentGold + petGold > 3000 )
		{
			s->sysmessage( 1645 );
			return;
		}
		mChar->talkAll( RandomNum( 1662, 1664 ), false ); // Updated to make use of dictionary
		if( !CheckSkill( mChar, BEGGING, 0, 1000 ) )
			s->sysmessage( 902 );
		else
		{
			targChar->talkAll( 903, false ); 
			Items->CreateItem( s, mChar, 0x0EED, ( 10 + RandomNum( 0, mChar->GetSkill( BEGGING ) + 1 ) / 25 ), 0, OT_ITEM, true );
			s->sysmessage( 904 );
		}
	}
	else
		s->sysmessage( 905 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::AnimalLoreTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets animal with Animal Lore skill
//o---------------------------------------------------------------------------o
void cSkills::AnimalLoreTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *i		= calcCharObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar	= s->CurrcharObj();
	if( !ValidateObject( i ) )
		return;
	if( i->GetCommandLevel() > 0 )
	{
		s->sysmessage( 906 );
		return;
	}
	if( i->GetID() == 0x0190 || i->GetID() == 0x0191 )
	{
		s->sysmessage( 907 );
		return;
	}
	if( CheckSkill( mChar, ANIMALLORE, 0, 1000 ) )
		i->emote( s, 1729, false, i->GetLoDamage(), i->GetHiDamage(), i->GetDef(), i->GetTaming()/10, i->GetHP() );
	else
		s->sysmessage( 908 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::ForensicsTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets corpse with forensics skill
//|					 (needs to be given more functionality so players can get
//|					 more use out of the skill)
//o---------------------------------------------------------------------------o
void cSkills::ForensicsTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( i ) )
	{
		s->sysmessage( 909 );
		return;
	}
	if( !i->isCorpse() )
	{
		s->sysmessage( 909 );
		return;
	}

	if( !objInRange( mChar, i, DIST_INRANGE ) )
	{
		s->sysmessage( 393 );
		return;
	}

	if( !LineOfSight( s, mChar, i->GetX(), i->GetY(), i->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) )
	{
		s->sysmessage( 1646 );
		return;
	}

	CChar *cMurderer = calcCharObjFromSer( i->GetTempVar( CITV_MOREX ) );
	char temp[1024];
	UI32 getTime = cwmWorldState->GetUICurrentTime();
	if( mChar->IsGM() )
	{
		if( ValidateObject( cMurderer ) )
			s->sysmessage( 910, i->GetName().c_str(), (getTime - i->GetMurderTime() ) / 1000, cMurderer->GetName().c_str() );
		else
			s->sysmessage( 910, i->GetName().c_str(), (getTime - i->GetMurderTime() ) / 1000, "" );
	}
	else 
	{
		if( !CheckSkill( mChar, FORENSICS, 0, 500 ) ) 
			s->sysmessage( 911 ); 
		else
		{
			int duration = ( getTime - i->GetMurderTime() ) / 1000;
			if(		 duration > 180 ) 
				strcpy( temp, Dictionary->GetEntry( 912, s->Language() ).c_str() );
			else if( duration >  60 ) 
				strcpy( temp, Dictionary->GetEntry( 913, s->Language() ).c_str() );
			else					  
				strcpy( temp, Dictionary->GetEntry( 914, s->Language() ).c_str() );

			s->sysmessage( 915, &i->GetName().c_str()[2], temp );
			if( !CheckSkill( mChar, FORENSICS, 500, 1000 ) || !ValidateObject( cMurderer ) ) 
				s->sysmessage( 916 ); 
			else if( ValidateObject( cMurderer ) )
				s->sysmessage( 917, cMurderer->GetName().c_str() );
			else
				s->sysmessage( 917, "" );
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::PoisoningTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets an item with PoisoningSkill
//o---------------------------------------------------------------------------o
void cSkills::PoisoningTarget( cSocket *s )
{
	VALIDATESOCKET( s );
	bool canPoison	= false;
	CChar *mChar	= s->CurrcharObj();
	CItem *poison	= static_cast<CItem *>(s->TempObj());
	s->TempObj( NULL );
	if( !ValidateObject( poison ) )
		return;

	if( poison->GetType() != IT_POTION || poison->GetTempVar( CITV_MOREY ) != 6 ) 
	{
		s->sysmessage( 918 );
		return;
	}

	CItem *toPoison = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( toPoison ) )
	{
		s->sysmessage( 920 );
		return;
	}
	UI16 realID = toPoison->GetID();
	if( ( realID >= 0x0F4F && realID <= 0x0F50 ) || ( realID >= 0x13B1 && realID <= 0x13B2 ) || ( realID >= 0x13FC && realID <= 0x13FD ) )
	{
		s->sysmessage( 1647 );
		return;
	}
	if( toPoison->GetHiDamage() <= 0 )
	{
		s->sysmessage( 1648 );
		return;
	}		
	switch( poison->GetTempVar( CITV_MOREZ ) ) 
	{
		case 1:	canPoison = CheckSkill( mChar, POISONING, 0, 500 );		break;	// lesser poison
		case 2:	canPoison = CheckSkill( mChar, POISONING, 151, 651 );		break;	// poison
		case 3:	canPoison = CheckSkill( mChar, POISONING, 551, 1051 );	break;	// greater poison
		case 4:	canPoison = CheckSkill( mChar, POISONING, 901, 1401 );	break;	// deadly poison
		default:	Console.Error( 2, "cSkills::PoisoningTarget -> Fallout of switch statement without default" );	return;
	}
	if( canPoison ) 
	{
		Effects->PlaySound( mChar, 0x0247 );	// poisoning effect
		if( toPoison->GetPoisoned() < poison->GetTempVar( CITV_MOREZ ) ) 
			toPoison->SetPoisoned( static_cast<UI08>(poison->GetTempVar( CITV_MOREZ ) ));
		s->sysmessage( 919 );
	} 
	else 
	{
		Effects->PlaySound( mChar, 0x0247 ); // poisoning effect
		s->sysmessage( 1649 );
	}

	poison->IncAmount( -1 );
	CItem *bPotion = Items->CreateItem( s, mChar, 0x0F0E, 1, 0, OT_ITEM, true );
	if( bPotion != NULL )
		bPotion->SetDecayable( true );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Inscribe( cSocket *s )
//|   Date        :  January 31, 2003
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when a player attempts to inscribe a blank scroll
//|					 Based upon inscribe.dfn
//o---------------------------------------------------------------------------o
void cSkills::Inscribe( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar	= s->CurrcharObj();
	CItem *packnum	= mChar->GetPackItem();
	if( !ValidateObject( packnum ) )
	{
		s->sysmessage( 773 );
		return;
	}
	
	CItem *spellBook = FindItemOfType( mChar, IT_SPELLBOOK );
	if( !ValidateObject( spellBook ) && !mChar->IsGM() )
	{
		s->sysmessage( 921 );
		return;
	}
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;
	if( i->IsLockedDown() )
	{
		s->sysmessage( 774 );
		return;
	}
	if( i->GetID() == 0x0E34 || i->GetID() == 0x0EF3 )
	{
		if( FindItemOwner( i ) != mChar )
		{
			s->sysmessage( 778 );
			return;
		}
		UI32 getAmt = GetItemAmount( mChar, i->GetID() );
		if( getAmt < 1 )
		{
			s->sysmessage( 776 );
			return;
		}
		NewMakeMenu( s, 99, INSCRIPTION );
	 }
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool cSkills::EngraveAction( cSocket *s, CItem *i, int getCir, int getSpell )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player starts engraving an item
//o---------------------------------------------------------------------------o
bool cSkills::EngraveAction( cSocket *s, CItem *i, int getCir, int getSpell )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::EngraveAction() Invalid socket with circle %i and spell %i on item 0x%X", getCir, getSpell, i->GetSerial() );
		return false;
	}
	CChar *mChar = s->CurrcharObj();
	int num = ( 8 * ( getCir - 1 ) ) + getSpell;
	
	if( Magic->spells[num].Action() ) 
		Effects->PlaySpellCastingAnimation( mChar, Magic->spells[num].Action() ); // Should have a default action instead
	if( !Magic->CheckReagents( mChar, Magic->spells[num].ReagantsPtr() ) || !Magic->CheckMana( mChar, num ) )
	{
		Magic->SpellFail( s );
		return false;
	}
		
	mChar->talkAll( Magic->spells[num].Mantra().c_str(), false );
	Magic->DelReagents( mChar, Magic->spells[num].Reagants() );
	Magic->SubtractMana( mChar, Magic->spells[num].Mana() );
	
	switch( getCir )
	{
		case 1:	// Circle 1
			switch( getSpell )
			{
				case 1: i->SetOffSpell( 1 );	break;      // Clumsy
				case 3:	i->SetOffSpell( 2 );	break;      // Feeblemind
				case 5: i->SetOffSpell( 3 );	break;		// Magic Arrow
				case 7: s->sysmessage( 931 );	return false;	// Reactive Armor
				case 8: i->SetOffSpell( 4 );	break;		// Weaken
				case 2:											// Create Food
				case 4:											// Heal
				case 6:											// Night Sight
						break;
				default:	
						Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
						return false;
			}
			break;
		case 2:	// Circle 2
			switch( getSpell )
			{
				case 1:			// Agility
				case 2:			// Cunning
				case 8:			// Strength
						break;
				case 3:			// Cure
				case 5:			// Magic Trap
				case 6:			// Magic Untrap
				case 7:			// Protection
						s->sysmessage( 931 );	
						return false;
				case 4:			 // Harm
						i->SetOffSpell( 5 );	
						break;
				default:	
						Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
						return false;
			}
			break;
		case 3:	// Circle 3
			switch( getSpell )
			{
				case 2:			// Fireball
						i->SetOffSpell( 6 );	
						break;
				case 3: //Magic lock
				case 5: //Telekinesis
						s->sysmessage( 931 );	
						return false;
				case 1:	// Bless
				case 4: //Poison
				case 6:	// Teleport
				case 7:	// Unlock
				case 8:	// Wall of Stone
						break;
				default:	
						Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
						return false;
			}
			break;
		case 4:	// Circle 4
			switch( getSpell )
			{       
				case 1:// Arch Cure
				case 2:// Arch Protection
				case 7://Mana Drain
						s->sysmessage( 931 );	
						return false;
				case 3:		// Curse
						i->SetOffSpell( 8 );	
						break;
				case 4:// Fire Field
				case 5:// Greater Heal
				case 8:// Recall
						break;
				case 6:		// Lightning
						i->SetOffSpell( 9 );	
						break;
				default:	
						Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
						return false;
			}
			break;
		case 5:	// Circle 5
			switch( getSpell )
			{
				case 1://Blade Spirit
				case 2://Dispel Field
				case 4://Magic Reflection
				case 7://Poison Field
						break;
				case 3:	//Incognito
				case 8:	//Summon Creature
						s->sysmessage( 931 );	
						return false;
				case 5:		//Mind Blast
						i->SetOffSpell( 11 );		
						break;
				case 6:		//Paralyse
						i->SetOffSpell( 12 );		
						break;
				default:	
						Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
						return false;
			}
			break;
		case 6:	// Circle 6
			switch( getSpell )
			{
				case 1://Dispel
				case 4://Invisibility
				case 5://Mark
				case 7:// Paralyze Field
				case 8://Reveal
						break;
				case 2:	  //Energy Bolt
						i->SetOffSpell( 13 );		
						break;
				case 3:		//Explosion
						i->SetOffSpell( 14 );		
						break;
				case 6:		// Mass Curse
						s->sysmessage( 931 );	
						return false;
				default:	
						Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
						return false;
			}
			break ;
		case 7:	// Circle 7
			switch( getSpell )
			{
				case 1://Chain Lightning
				case 5:// Mana Vampire
				case 6:// Mass Dispel
				case 7:// Meteor Storm
				case 8:// Polymorph
						s->sysmessage( 931 );	
						return false;
				case 2://Energy Field
				case 4://Gate Travel
						break;
				case 3:		// FlameStrike
						i->SetOffSpell( 15 );	
						break;
				default:	
						Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
						return false;
			}
			break ;
		case 8:	// Circle 8
			switch( getSpell )
			{
				case 1:// Earthquake
						s->sysmessage( 931 );	
						return false;
				case 2://Energy Vortex
				case 3://Resurrection
				case 4:// Summon Air Elemental
				case 5://Summon Daemon
				case 6:// Summon Earth Elemental
				case 7:// Summon Fire Elemental
				case 8:// Summon Water Elemental
						break;
				default:	
						Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
						return false;
			}
			break;
		default:
			Console.Error( 2, " Fallout of switch statement without default. skills.cpp, cSkills::Inscribe" );
			return false;
  }
  return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::updateSkillLevel( CChar *c, UI08 s )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Calculate the skill of this character based on the 
//|					 characters baseskill and stats
//o---------------------------------------------------------------------------o
void cSkills::updateSkillLevel( CChar *c, UI08 s )
{
	UI16 sstr = cwmWorldState->skill[s].strength;
	SI16 astr = c->ActualStrength();
	UI16 sdex = cwmWorldState->skill[s].dexterity;
	SI16 adex = c->ActualDexterity();
	UI16 sint = cwmWorldState->skill[s].intelligence;
	SI16 aint = c->ActualIntelligence();
	UI16 bskill = c->GetBaseSkill( s );

	UI16 temp = ( ( (sstr * astr) / 100 + (sdex * adex) / 100 + (sint + aint) / 100) * ( 1000 - bskill ) ) / 1000 + bskill;
	c->SetSkill( UOX_MAX( bskill, temp ), s );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::LockPick( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses lockpics on a locked item
//o---------------------------------------------------------------------------o
void cSkills::LockPick( cSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i		= calcItemObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar	= s->CurrcharObj();
	CItem *lockPick	= static_cast<CItem *>(s->TempObj());
	s->TempObj( NULL );
	if( !ValidateObject( i ) )
		return;
	ItemTypes iType = i->GetType();
	if( iType == IT_CONTAINER || iType == IT_DOOR || iType == IT_SPAWNCONT ) 
	{
		s->sysmessage( 932 );
		return;
	}
	
	if( i->GetID() == 0x1E2C || i->GetID() == 0x1E2D )
	{
		if( mChar->GetSkill( LOCKPICKING ) < 300 )
		{
			CheckSkill( mChar, LOCKPICKING, 0, 1000 );	// check their skill
			Effects->PlaySound(s, 0x0241, true );						// lockpicking sound
		}
		else
		{
			if( RandomNum( 0, 1 ) == 0 )		// chance it could break the pick
			{
				s->sysmessage( 933 );
				lockPick->IncAmount( -1 );
			}
			else
				s->sysmessage( 934 );
		}
	}
	if( iType == IT_LOCKEDCONTAINER || iType == IT_LOCKEDDOOR || iType == IT_LOCKEDSPAWNCONT )
	{
		if( i->GetTempVar( CITV_MORE ) == 0 ) //Make sure it isn't an item that has a key (i.e. player house, chest..etc)
		{
			bool canPick = false;
			if( !ValidateObject( lockPick ) ) 
			{
				if( s->CurrentSpellType() != 2 )              // not a wand cast
				{
					Magic->SubtractMana( mChar, 5 );  // subtract mana on scroll or spell
					if( s->CurrentSpellType() == 0 )             // del regs on normal spell
					{
						reag_st toDel;
						toDel.ash = 1;
						toDel.moss = 1;
						Magic->DelReagents( mChar, toDel );
					}
				}
				canPick = true;
			} 
			else if( CheckSkill( mChar, LOCKPICKING, 0, 1000 ) )
				canPick = true;
			else
			{
				if( RandomNum( 0, 1 ) == 0 ) 
				{
					s->sysmessage( 933 );
					lockPick->Delete();
				} 
				else
					s->sysmessage( 936 );
			}
			if( canPick )
			{
				switch( iType )
				{
					case IT_LOCKEDCONTAINER:	i->SetType( IT_CONTAINER );	break;
					case IT_LOCKEDDOOR:			i->SetType( IT_DOOR );		break;
					case IT_LOCKEDSPAWNCONT:	i->SetType( IT_SPAWNCONT );	break;
					default:	Console.Error( 2, " cSkills::LockPick -> Fallout of switch statement without default" ); return;
				}
				Effects->PlaySound( i, 0x01FF );
				s->sysmessage( 935 );
			}
		} 
		else
			s->sysmessage( 937 );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::TDummy( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses a training dummy
//o---------------------------------------------------------------------------o
void cSkills::TDummy( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar	= s->CurrcharObj();
	UI08 type		= Combat->getBowType( Combat->getWeapon( mChar ) );
	
	if( type > 0 )
	{
		s->sysmessage( 938 );
		return;
	}
	if( mChar->IsOnHorse() )
		Combat->CombatOnHorse( mChar );
	else
		Combat->CombatOnFoot( mChar );
	
	switch( RandomNum( 0, 2 ) )
	{
		case 0: Effects->PlaySound( s, 0x013B, true );	break;
		case 1: Effects->PlaySound( s, 0x013C, true );	break;        
		case 2: Effects->PlaySound( s, 0x013D, true );	break;
		default:	Console.Error( 2, " cSkills::TDummy -> Fallout of switch statement without default" );	return;
	}            
	CItem *j = calcItemObjFromSer( (s->GetDWord( 1 ))&0x7FFFFFFF );
	if( ValidateObject( j ) )
	{
		if( j->GetID( 2 ) == 0x70 ) 
			j->SetID( 0x71, 2 );
		else if( j->GetID( 2 ) == 0x74 ) 
			j->SetID( 0x75, 2 );
		Effects->tempeffect( NULL, j, 14, 0, 0, 0 );
	}
	UI16 skillused = Combat->getCombatSkill( Combat->getWeapon( mChar ) );
	if( mChar->GetSkill( static_cast<UI08>(skillused )) < 300 )
	{
		CheckSkill( mChar, static_cast<UI08>(skillused), 0, 1000 );
		if( mChar->GetSkill( TACTICS ) < 300 )
			CheckSkill( mChar, TACTICS, 0, 250 );  //Dupois - Increase tactics but only by a fraction of the normal rate
	}
	else
		s->sysmessage( 939 );
}                           


//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Tinkering( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses tinkering skill
//o---------------------------------------------------------------------------o
void cSkills::Tinkering( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar	= s->CurrcharObj();
	CItem *packnum	= mChar->GetPackItem();
	if( !ValidateObject( packnum ) ) 
	{
		s->sysmessage( 773 ); 
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
	{
		s->sysmessage( 942 );
		return;
	}
	UI16 realID = i->GetID();
	if( realID == 0x1BEF || realID == 0x1BF2 || realID == 0x1BDD || realID == 0x1BE0 )
	{
		if( FindItemOwner( i ) != mChar )
			s->sysmessage( 775 );
		else
		{
			UI32 getAmt = GetItemAmount(mChar, realID );
			if( getAmt < 2 )
			{ 
				s->sysmessage( 940 );
				return;
			}
			if( realID == 0x1BDD || realID == 0x1BE0 )
			{
				if( getAmt < 4 )
				{
					s->sysmessage( 941 );
					return;
				} 
			}
			NewMakeMenu( s, 59, TINKERING );
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::AButte( cSocket *s, CItem *x )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses an Archery Butte
//o---------------------------------------------------------------------------o
void cSkills::AButte( cSocket *s, CItem *x )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;
	
	SI16 getDist = -1;
	if( x->GetID( 2 ) == 0x0a )
	{ // East Facing Butte
		if( x->GetX() <= mChar->GetX() && x->GetY() == mChar->GetY() )
			getDist = mChar->GetX() - x->GetX();
	}
	else
	{ // South Facing Butte
		if( x->GetY() <= mChar->GetY() && x->GetX() == mChar->GetX() )
			getDist = mChar->GetY() - x->GetY();
	}
	
	if( getDist == 1 )
	{
		UI08 i = 0;
		CItem *c;
		if( x->GetTempVar( CITV_MORE, 1 ) > 0 )
		{
			c = Items->CreateItem( s, mChar, 0x0F3F, x->GetTempVar( CITV_MORE, 1 ), 0, OT_ITEM, true );
			if( c == NULL ) 
				return;
		}
		if( x->GetTempVar( CITV_MORE, 2 ) > 0 )
		{
			c = Items->CreateItem( s, mChar, 0x1BFB, x->GetTempVar( CITV_MORE, 2 ), 0, OT_ITEM, true );
			if( c == NULL ) 
				return;
		}
		
		if( x->GetTempVar( CITV_MORE, 1 ) > 0 ) 
			++i;
		if( x->GetTempVar( CITV_MORE, 2 ) > 0 ) 
			i += 2;
		
		switch( i )
		{
			case 0:		s->sysmessage( 943 );										break;
			case 1:		s->sysmessage( 944, x->GetTempVar( CITV_MORE, 1 ) );						break;
			case 2:		s->sysmessage( 945, x->GetTempVar( CITV_MORE, 2 ) );						break;
			case 3:		s->sysmessage( 946, x->GetTempVar( CITV_MORE, 1 ), x->GetTempVar( CITV_MORE, 2 ) );		break;
			default:	Console.Error( 2, " cSkills::AButte -> Fallout of switch statement without default" );
				return;
		}
		x->SetTempVar( CITV_MORE, 0, 1 );
		x->SetTempVar( CITV_MORE, 0, 2 );
	}
	
	if( getDist >= 5 && getDist <= 8 )
	{
		UI08 bowType = Combat->getBowType( Combat->getWeapon( mChar ) );
		if( bowType == 0 )
		{
			s->sysmessage( 947 );
			return;
		} 
		if( x->GetTempVar( CITV_MORE, 1 ) + x->GetTempVar( CITV_MORE, 2 ) > 99 )
		{
			s->sysmessage( 948 );
			return;
		}
		UI32 getArrows = 0;
		if( bowType == BOWS ) 
			getArrows = GetItemAmount( mChar, 0x0F3F );
		else 
			getArrows = GetItemAmount( mChar, 0x1BFB );
		if( getArrows == 0 ) 
		{
			s->sysmessage( 949 );
			return;
		}
		if( bowType == BOWS )
		{
			DeleteItemAmount( mChar, 1, 0x0F3F );
			x->SetTempVar( CITV_MORE, x->GetTempVar( CITV_MORE, 1 ) + 1, 1 );
			//add moving effect here to item, not character
		}
		else
		{
			DeleteItemAmount( mChar, 1, 0x1BFB );
			x->SetTempVar( CITV_MORE, x->GetTempVar( CITV_MORE, 2 ) + 1, 2 );
			//add moving effect here to item, not character
		} 
		if( mChar->IsOnHorse() ) 
			Combat->CombatOnHorse( mChar );
		else 
			Combat->CombatOnFoot( mChar );
		
		if( mChar->GetSkill( ARCHERY ) < 350 )
			CheckSkill( mChar, ARCHERY, 0, 1000 );
		else
			s->sysmessage( 950 );
		
		switch( ( mChar->GetSkill( ARCHERY ) + ( RandomNum( 0, 199 ) - 100 ) ) / 100 )
		{
			case -1:
			case 0:
			case 1:		
				s->sysmessage( 951 );	
				Effects->PlaySound( s, 0x0238, true );	
				break;
			case 2:
			case 3:		
				s->sysmessage( 952 );	
				Effects->PlaySound( s, 0x0234, true );	
				break;
			case 4:
			case 5:
			case 6:		
				s->sysmessage( 953 );	
				Effects->PlaySound( s, 0x0234, true );	
				break;
			case 7:
			case 8:
			case 9:		
				s->sysmessage( 954 );	
				Effects->PlaySound( s, 0x0234, true );	
				break;
			case 10:
			case 11:	
				s->sysmessage( 955 );	
				Effects->PlaySound( s, 0x0234, true );	
				break;
			default:																						break;
		}
	}
	else
		s->sysmessage( 956 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::TinkerAxel( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Use tinkering on an Axel
//o---------------------------------------------------------------------------o
void cSkills::TinkerAxel( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar		= s->CurrcharObj();
	CItem *skillItem	= static_cast<CItem *>(s->TempObj());
	s->TempObj( NULL );
	if( !ValidateObject( skillItem ) )
	{
		s->sysmessage( 957 );
		return;
	}
	bool canCombine = false;
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		if( i == skillItem )
		{
			s->sysmessage( 958 );
			return;
		}

		UI16 p2id = i->GetID();
		switch( skillItem->GetID() )
		{
			case 0x1053:
			case 0x1054:	// gears
				if( p2id == 0x105B || p2id == 0x105C )
					canCombine = true;
				break;
			case 0x105B:
			case 0x105C:	// axles
				if( p2id == 0x1053 || p2id == 0x1054 )
					canCombine = true;
				break;
			default:	// shouldn't get here
				Console.Error( 2, " cSkills::TinkerAxel -> Fallout of switch statement without default!" );
				return;
		}
		if( canCombine && objInRange( mChar, i, DIST_NEARBY ) )
		{
			if( !CheckSkill( mChar, TINKERING, 0, 1000 ) )
				s->sysmessage( 959 );
			else
			{
				s->sysmessage( 960 );
				skillItem->IncAmount( -1 );
				i->IncAmount( -1 );
				CItem *c = Items->CreateItem( s, mChar, 0x1051, 1, 0, OT_ITEM, true );
				if( c == NULL ) 
					return;
				c->SetName( Dictionary->GetEntry( 961 ) );
			}
		}
	} 
	if( !canCombine ) 
		s->sysmessage( 962 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::TinkerAwg( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Unknown : Tinkering
//o---------------------------------------------------------------------------o
void cSkills::TinkerAwg( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar	= s->CurrcharObj();
	CItem *packnum	= mChar->GetPackItem();
	if( !ValidateObject( packnum ) ) 
	{
		s->sysmessage( 773 );
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( i ) )
		return;

	CItem *skillItem	= static_cast<CItem *>(s->TempObj());
	s->TempObj( NULL );
	if( !ValidateObject( skillItem ) )
		return;
	char id2;
	UI16 realID = i->GetID();
	UI16 matID = skillItem->GetID();
	if( ( realID == 0x1051 || realID == 0x1052 ) && ( matID == 0x105D || matID == 0x105E ) ||
		( realID == 0x105D || realID == 0x105E ) && ( matID == 0x1051 || matID == 0x1052 ) )
		id2 = 0x4F;
	else if( ( realID == 0x1051 || realID == 0x1052 ) && ( matID == 0x1055 || matID == 0x1056 ) ||
		( realID == 0x1055 || realID == 0x1056 ) && ( matID == 0x1051 || matID == 0x1052 ) )
		id2 = 0x59;
	else
	{
		s->sysmessage( 962 );
		return;
	}
	if( FindItemOwner( i ) != mChar )
	{
		s->sysmessage( 963 );
		return;
	}
	else if( !CheckSkill( mChar, TINKERING, 0, 1000 ) )
	{
		s->sysmessage( 964 );
		DeleteItemAmount( mChar, 1, realID );
		return;
	}
	else
	{
		std::string temp;
		if( id2 == 0x4F ) 
			temp = Dictionary->GetEntry( 965 );
		else if( id2 == 0x59 ) 
			temp = Dictionary->GetEntry( 966 );
		CItem *c = Items->CreateItem( s, mChar, (4096 + id2), 1, 0, OT_ITEM, true );
		if( c == NULL ) 
			return;
		c->SetName( temp );
		s->sysmessage( 960 );
	}
	DeleteItemAmount( mChar, 1, matID );
	DeleteItemAmount( mChar, 1, realID );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::TinkerClock( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Used when player Tinkers a clock
//o---------------------------------------------------------------------------o
void cSkills::TinkerClock( cSocket *s )
{
	VALIDATESOCKET( s );
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar = s->CurrcharObj();
	if( ValidateObject( i ) )
	{
		UI16 realID = i->GetID();
		if( realID >= 0x104D && realID <= 0x1050 )
		{
			if( objInRange( mChar, i, DIST_NEARBY ) )
			{
				if( !CheckSkill( mChar, TINKERING, 0, 1000 ) ) 
					s->sysmessage( 959 );
				else
				{
					s->sysmessage( 960 );
					
					CItem *c = Items->CreateItem( s, mChar, 0x104B, 1, 0, OT_ITEM, true );
					if( c == NULL ) 
						return;
					c->SetName( "clock" );
				}
				i->IncAmount( -1 );
				return;
			}
		} 
	}   
	s->sysmessage( 962 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Meditation( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses meditation skill
//o---------------------------------------------------------------------------o
void cSkills::Meditation( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( Combat->calcDef( mChar, 0, false ) > 10 )
	{
		s->sysmessage( 967 );
		mChar->SetMeditating( false );
		return;
	}
	if( ValidateObject( Combat->getWeapon( mChar ) ) || ValidateObject( Combat->getShield( mChar ) ) )
	{
		s->sysmessage( 968 );
		mChar->SetMeditating( false );
		return;
	}
	if( mChar->GetMana() == mChar->GetMaxMana() )
	{
		s->sysmessage( 969 );
		mChar->SetMeditating( false );
		return;
	}
	if( !CheckSkill( mChar, MEDITATION, 0, 1000 ) ) 
	{
		s->sysmessage( 970 );
		mChar->SetMeditating( false );
		return;
	}
	s->sysmessage( 971 );
	mChar->SetMeditating( true );
	Effects->PlaySound( s, 0x00F9, true );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Persecute( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when a Ghost attacks a Player.  If entry in server.scp
//|					 is enabled, players mana decreases each time you try to
//|					 persecute him
//o---------------------------------------------------------------------------o
void cSkills::Persecute( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *c		= s->CurrcharObj();
	CChar *targChar	= c->GetTarg();
	if( !ValidateObject( targChar ) || targChar->IsGM() ) 
		return;
	
	int decrease = (int)( c->GetIntelligence() / 10 ) + 3;
	
	if( s->GetTimer( tPC_SKILLDELAY ) <= cwmWorldState->GetUICurrentTime() || c->IsGM() )
	{
		if( ( RandomNum( 0, 19 ) + c->GetIntelligence() ) > 45 ) // not always
		{
			cSocket *tSock = calcSocketObjFromChar( targChar );
			targChar->SetMana( targChar->GetMana() - decrease ); // decrease mana
			s->sysmessage( 972 );
			if( tSock != NULL )
				tSock->sysmessage( 973 );
			s->SetTimer( tPC_SKILLDELAY, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->ServerSkillDelayStatus() )) );
			targChar->emoteAll( 974, true, targChar->GetName().c_str() );
		}
		else
			s->sysmessage( 975 );
	}
	else
		s->sysmessage( 976 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Smith( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player attempts to smith an item
//o---------------------------------------------------------------------------o
void cSkills::Smith( cSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar	= s->CurrcharObj();
	CItem *packnum	= mChar->GetPackItem();
	
	if( !ValidateObject( packnum ) ) 
	{
		s->sysmessage( 773 ); 
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( ValidateObject( i ) )
	{
		if( i->GetID() >= 0x1BE3 && i->GetID() <= 0x1BF9 )	// is it an ingot?
		{
			miningData *oreType = FindOre( i->GetColour() );
			if( oreType == NULL )
			{
				s->sysmessage( 977 );
				return;
			}
			if( FindItemOwner( i ) != mChar )
				s->sysmessage( 978, oreType->name.c_str() );
			else
				AnvilTarget( s, (*i), oreType );
		}
		else	// something we might repair?
			RepairMetal( s );
		return;
	}
	s->sysmessage( 979 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::AnvilTarget( cSocket *s, CItem& item, SI16 oreType )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Unknown : Smithy
//o---------------------------------------------------------------------------o
void cSkills::AnvilTarget( cSocket *s, CItem& item, miningData *oreType )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();

	REGIONLIST nearbyRegions = MapRegion->PopulateList( mChar );
	REGIONLIST_ITERATOR rIter;
	for( rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		SubRegion *MapArea = (*rIter);
		if( MapArea == NULL )	// no valid region
			continue;
		MapArea->PushItem();
		for( CItem *tempItem = MapArea->FirstItem(); !MapArea->FinishedItems(); tempItem = MapArea->GetNextItem() )
		{
			if( !ValidateObject( tempItem ) )
				continue;
			if( tempItem->GetID() == 0x0FAF || tempItem->GetID() == 0x0FB0 )
			{
				if( objInRange( mChar, tempItem, DIST_NEARBY ) )
				{
					UI32 getAmt = GetItemAmount( mChar, item.GetID(), item.GetColour() );     
					if( getAmt < oreType->minAmount )
					{ 
						s->sysmessage( 980, oreType->name.c_str() );
						MapArea->PopItem();
						return;
					}
					NewMakeMenu( s, oreType->makemenu, BLACKSMITHING );
					MapArea->PopItem();
					return;
				}
			}
		}
		MapArea->PopItem();
	}
	s->sysmessage( 981 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  cSkills::cSkills( void )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Default constructor
//o---------------------------------------------------------------------------o
cSkills::cSkills( void )
{
	ores.resize( 0 );
}
cSkills::~cSkills( void )
{
	ores.resize( 0 );
	skillMenus.clear();
	itemsForMenus.clear();
	actualMenus.clear();
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool cSkills::LoadMiningData( void )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Load mining resource stuff from scripts
//o---------------------------------------------------------------------------o
bool cSkills::LoadMiningData( void )
{
	// Let's first get our ore list, in SECTION ORE_LIST
	ScriptSection *oreList = FileLookup->FindEntry( "ORE_LIST", skills_def );
	bool rvalue = false;
	if( oreList != NULL )
	{
		STRINGLIST oreNameList;
		UString tag;
		UString data;
		UString UTag;
		for( tag = oreList->First(); !oreList->AtEnd(); tag = oreList->Next() )
			oreNameList.push_back( tag );
		if( oreNameList.size() != 0 )
		{
			rvalue = true;
			ScriptSection *individualOre = NULL;
			STRINGLIST_ITERATOR toCheck;
			for( toCheck = oreNameList.begin(); toCheck != oreNameList.end(); ++toCheck )
			{
				std::string oreName = (*toCheck);
				individualOre = FileLookup->FindEntry( oreName, skills_def );
				if( individualOre != NULL )
				{
					miningData toAdd;
					toAdd.colour	= 0;
					toAdd.foreign	= true;
					toAdd.makemenu	= 0;
					toAdd.minAmount = 3;
					toAdd.minSkill	= 0;
					toAdd.name		= oreName;
					for( tag = individualOre->First(); !individualOre->AtEnd(); tag = individualOre->Next() )
					{
						UTag = tag.upper();
						data = individualOre->GrabData();
						switch( (tag.data()[0]) )	// break on tag
						{
							case 'c':
							case 'C':	
								if( UTag == "COLOUR" )
									toAdd.colour = data.toUShort();
								break;
							case 'f':
							case 'F':
								if( UTag == "FOREIGN" )
									toAdd.foreign = ( data.toUShort() != 0 );
								break;
							case 'm':
							case 'M':
								if( UTag == "MAKEMENU" )
									toAdd.makemenu = data.toLong();
								else if( UTag == "MINAMOUNT" )
									toAdd.minAmount = data.toUByte();
								else if( UTag == "MINSKILL" )
									toAdd.minSkill = data.toUShort();
								break;
							case 'n':
							case 'N':
								if( UTag == "NAME" )
									toAdd.name = data;
								break;
							default:
								Console << "Unknown mining tag " << tag << " with data " << data << " in SECTION " << oreName << myendl;
								break;
						}
					}
					ores.push_back( toAdd );
				}
			}
		}
	}
	return rvalue;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Load( void )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Load mining stuff from scripts
//o---------------------------------------------------------------------------o
void cSkills::Load( void )
{
	Console << "Loading custom ore data        ";

	if( !LoadMiningData() )
	{	// we need to setup our default ores and stuff here!
		miningData miningstuff;
		miningstuff.minAmount = 3;		
		miningstuff.foreign = false;	miningstuff.minSkill = 0;	miningstuff.name = "Iron";	miningstuff.colour = 0;	miningstuff.makemenu = 1;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 850;	miningstuff.name = "Gold";	miningstuff.colour = 0x045E;	miningstuff.makemenu = 50;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 750;	miningstuff.name = "Copper";miningstuff.colour = 0x0641;	miningstuff.makemenu = 814;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 790;	miningstuff.name = "Silver";miningstuff.colour = 0x038A;	miningstuff.makemenu = 813;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 900;	miningstuff.name = "Agapite";miningstuff.colour = 0x0150;	miningstuff.makemenu = 806;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 650;	miningstuff.name = "Adamantium";miningstuff.colour = 0x0386;	miningstuff.makemenu = 800;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 950;	miningstuff.name = "Verite";miningstuff.colour = 0x022F;	miningstuff.makemenu = 802;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 800;	miningstuff.name = "Bronze";miningstuff.colour = 0x02E7;	miningstuff.makemenu = 801;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 700;	miningstuff.name = "Merkite";miningstuff.colour = 0x02C3;	miningstuff.makemenu = 804;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 990;	miningstuff.name = "Mythril";miningstuff.colour = 0x0191;	miningstuff.makemenu = 803;
		ores.push_back( miningstuff );
	}
	Console.PrintDone();
	
	Console << "Loading creation menus         ";	
	LoadCreateMenus();
	Console.PrintDone();

	Console << "Loading/initializing resources ";
	LoadResourceData();
	Console.PrintDone();

	Console.PrintSectionBegin();
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::SaveResources( void )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Saves resource system info
//o---------------------------------------------------------------------------o
void cSkills::SaveResources( void )
{
	std::string resourceFile	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "resource.bin";
	FILE *toWrite				= fopen( resourceFile.c_str(), "wb" );
	if( toWrite != NULL )
	{
		for( UI16 gridX = 0; gridX < 610; ++gridX )
		{
			for( UI16 gridY = 0; gridY < 410; ++gridY )
				fwrite( &resources[gridX][gridY], sizeof( resourceEntry ), 1, toWrite );
		}
		fclose( toWrite );
	}
	else // Can't save resources
		Console.Error( 1, "Failed to open resource.bin for writing" );
} 

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::LoadResourceData( void )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Loads resource system info
//o---------------------------------------------------------------------------o
void cSkills::LoadResourceData( void )
{
	SI16 resOre					= cwmWorldState->ServerData()->ResOre();
	SI16 resLog					= cwmWorldState->ServerData()->ResLogs();
	UI32 oreTime				= BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->ResOreTime() ));
	UI32 logTime				= BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->ResLogTime()) );
	std::string resourceFile	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "resource.bin";
	FILE *binData				= fopen( resourceFile.c_str(), "rb" );
	bool fileExists				= ( binData != NULL );

	resourceEntry toRead;
	for( SI16 gridX = 0; gridX < 610; ++gridX )
	{
		for( SI16 gridY = 0; gridY < 410; ++gridY )
		{
			if( fileExists )
			{
				fread( &toRead, sizeof( resourceEntry ), 1, binData );
				resources[gridX][gridY].oreAmt = toRead.oreAmt;
				resources[gridX][gridY].logAmt = toRead.logAmt;
			}
			else
			{
				resources[gridX][gridY].oreAmt  = resOre;
				resources[gridX][gridY].logAmt  = resLog;
			}
			// No need to preserve time.  Do a refresh automatically
			resources[gridX][gridY].oreTime = oreTime;
			resources[gridX][gridY].logTime = logTime;
		}
	}
	if( fileExists )
		fclose( binData );
}

//o---------------------------------------------------------------------------o
//|   Function    :  size_t cSkills::GetNumberOfOres( void )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Returns number of custom ores
//o---------------------------------------------------------------------------o
size_t cSkills::GetNumberOfOres( void )
{
	return ores.size();
}

//o---------------------------------------------------------------------------o
//|   Function    :  miningData *cSkills::GetOre( int number )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Returns a handle to the data about the ore
//o---------------------------------------------------------------------------o
miningData *cSkills::GetOre( size_t number )
{
	if( number >= ores.size() )
		return NULL;
	return &ores[number];
}

//o---------------------------------------------------------------------------o
//|   Function    :  miningData *cSkills::FindOre( string name )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Returns a handle to the data about the ore based on it's name
//o---------------------------------------------------------------------------o
miningData *cSkills::FindOre( std::string name )
{
	std::vector< miningData >::iterator	oreIter;
	for( oreIter = ores.begin(); oreIter != ores.end(); ++oreIter )
	{
		if( (*oreIter).name == name )
			return &(*oreIter);
	}
	return NULL;
}

//o---------------------------------------------------------------------------o
//|   Function    :  miningData *cSkills::FindOre( UI16 colour)
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Find ore color
//o---------------------------------------------------------------------------o
miningData *cSkills::FindOre( UI16 colour )
{
	std::vector< miningData >::iterator oreIter;
	for( oreIter = ores.begin(); oreIter != ores.end(); ++oreIter )
	{
		if( (*oreIter).colour == colour )
			return &(*oreIter);
	}
	return NULL;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::MakeOre( UI08 Region, CChar *actor, cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Spawn Ore in players pack when he successfully mines
//o---------------------------------------------------------------------------o
void cSkills::MakeOre( UI08 Region, CChar *actor, cSocket *s )
{
	cTownRegion *targRegion = regions[Region];
	if( targRegion == NULL || !ValidateObject( actor ) )
		return;
	VALIDATESOCKET( s );
	UI16 getSkill			= actor->GetSkill( MINING );
	int oreChance			= RandomNum( static_cast< SI32 >(0), targRegion->GetOreChance() );	// find our base ore
	int sumChance			= 0;
	bool oreFound			= false;
	const orePref *toFind	= NULL;
	miningData *found		= NULL;
	CItem *oreItem			= NULL;
	for( size_t currentOre = 0; currentOre < targRegion->GetNumOrePreferences(); ++currentOre )
	{
		toFind = targRegion->GetOrePreference( currentOre );
		if( toFind == NULL )
			continue;
		if( oreChance >= sumChance && oreChance < ( sumChance + toFind->percentChance ) )
		{
			found = toFind->oreIndex;
			if( found != NULL )
			{
				if( getSkill >= found->minSkill )
				{
					std::string temp;
					temp = found->name + " ore";
					oreItem = Items->CreateItem( s, actor, 0x19B9, 1, found->colour, OT_ITEM, true );
					if( oreItem != NULL )
						oreItem->SetName( temp );
					s->sysmessage( 982, temp.c_str() );
					oreFound = true;
				}
			}
		}
		if( sumChance > oreChance )
		{
			found = toFind->oreIndex;
			if( found != NULL )
			{
				if( getSkill >= found->minSkill )
				{
					std::string temp;
					temp = found->name + " ore";
					oreItem = Items->CreateItem( s, actor, 0x19B9, 1, found->colour, OT_ITEM, true );
					if( oreItem != NULL )
						oreItem->SetName( temp );
					s->sysmessage( 982, temp.c_str() );
					oreFound = true;
				}
			}
		}
		if( oreFound )
			break;
		sumChance += toFind->percentChance;
	}
	if( !oreFound || ( oreFound && found == NULL ) )
	{
		if( getSkill >= 850 )
		{
			Items->CreateRandomItem( s, "digginggems" ); 
			s->sysmessage( 983 );
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::LoadCreateMenus( void )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Load Create menus from the create DFNs
//o---------------------------------------------------------------------------o
void cSkills::LoadCreateMenus( void )
{
	VECSCRIPTLIST *toScan = FileLookup->GetFiles( create_def );
	VECSCRIPTLIST_ITERATOR toCheck;
	if( toScan == NULL )
		return;
	ScriptSection *toSearch = NULL;			// data in a particular section
	UString tag;							// entry tag
	UString data;							// entry data
	UString UTag;
	UI16 ourEntry;							// our actual entry number
	for( toCheck = toScan->begin(); toCheck != toScan->end(); ++toCheck )
	{
		if( (*toCheck) == NULL )
			continue;
		Script *ourScript = (*toCheck);
		for( toSearch = ourScript->FirstEntry(); toSearch != NULL; toSearch = ourScript->NextEntry() )
		{
			UString eName = ourScript->EntryName();
			if( "SUBMENU" == eName.substr( 0, 7 ) )	// is it a menu? (not really SUB, just to avoid picking up MAKEMENUs)
			{
				ourEntry = eName.section( " ", 1 ).stripWhiteSpace().toUShort();
				for( tag = toSearch->First(); !toSearch->AtEnd(); tag = toSearch->Next() )
				{
					UTag = tag.upper();
					data = toSearch->GrabData();
					if( UTag == "MENU" )
						actualMenus[ourEntry].menuEntries.push_back( data.toUShort() );
					else if( UTag == "ITEM" )
						actualMenus[ourEntry].itemEntries.push_back( data.toUShort() );
				}
			}
			else if( "ITEM" == eName.substr( 0, 4 ) )	// is it an item?
			{
				ourEntry = eName.section( " ", 1 ).stripWhiteSpace().toUShort();
				createEntry tmpEntry;
				tmpEntry.minRank		= 0;
				tmpEntry.maxRank		= 10;
				tmpEntry.colour			= 0;
				tmpEntry.targID			= 1;
				tmpEntry.soundPlayed	= 1;

				for( tag = toSearch->First(); !toSearch->AtEnd(); tag = toSearch->Next() )
				{
					UTag = tag.upper();
					data = toSearch->GrabData();
					if( UTag == "COLOUR" )
						tmpEntry.colour = data.toUShort();
					else if( UTag == "ID" )
						tmpEntry.targID = data.toUShort();
					else if( UTag == "MINRANK" )
						tmpEntry.minRank = data.toUByte();
					else if( UTag == "MAXRANK" )
						tmpEntry.maxRank = data.toUByte();
					else if( UTag == "NAME" )
						tmpEntry.name = data;
					else if( UTag == "SOUND" )
						tmpEntry.soundPlayed = data.toUShort();
					else if( UTag == "ADDITEM" )
						tmpEntry.addItem = data;
					else if( UTag == "DELAY" )
						tmpEntry.delay = data.toShort();
					else if( UTag == "RESOURCE" )
					{
						resAmountPair tmpResource;
						if( data.sectionCount( " " ) == 0 )
						{
							tmpResource.amountNeeded	= 1;
							tmpResource.colour			= 0;
							tmpResource.itemID			= data.toUShort();
						}
						else
						{
							if( data.sectionCount( " " ) == 1 )
							{
								tmpResource.amountNeeded	= data.section( " ", 1 ).toUByte();
								tmpResource.colour			= 0;
							}
							else
							{
								tmpResource.colour			= data.section( " ", 1, 1 ).toUShort();
								tmpResource.amountNeeded	= data.section( " ", 2, 2 ).toUByte();
							}
							tmpResource.itemID = data.section( " ", 0, 0 ).toUShort();
						}
						tmpEntry.resourceNeeded.push_back( tmpResource );
					}
					else if( UTag == "SKILL" )
					{
						resSkillReq tmpResource;
						if( data.sectionCount( " " ) == 0 )
						{
							tmpResource.maxSkill	= 1000;
							tmpResource.minSkill	= 0;
							tmpResource.skillNumber	= data.toUByte();
						}
						else
						{
							if( data.sectionCount( " " ) == 1 )
							{
								tmpResource.maxSkill = 1000;
								tmpResource.minSkill = data.section( " ", 1, 1 ).toUShort();
							}
							else
							{
								tmpResource.minSkill = data.section( " ", 1, 1 ).toUShort();
								tmpResource.maxSkill = data.section( " ", 2, 2 ).toUShort();
							}
							tmpResource.skillNumber = data.section( " ", 0, 0 ).toUByte();
						}
						tmpEntry.skillReqs.push_back( tmpResource );
					}
					else if( UTag == "SPELL" )
						tmpEntry.spell = data.toUShort();
				}
				itemsForMenus[ourEntry] = tmpEntry;
			}
			else if( "MENUENTRY" == eName.substr( 0, 9 ) )
			{
				ourEntry = eName.section( " ", 1, 1 ).toUShort();
				for( tag = toSearch->First(); !toSearch->AtEnd(); tag = toSearch->Next() )
				{
					UTag = tag.upper();
					data = toSearch->GrabData();
					if( UTag == "ID" )
						skillMenus[ourEntry].targID = data.toUShort();
					else if( UTag == "COLOUR" )
						skillMenus[ourEntry].colour = data.toUShort();
					else if( UTag == "NAME" )
						skillMenus[ourEntry].name = data;
					else if( UTag == "SUBMENU" )
						skillMenus[ourEntry].subMenu = data.toUShort();
				}
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool cSkills::AdvanceSkill( CChar *s, UI08 sk, bool skillUsed )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Advance players skill based on success or failure in CheckSkill()
//o---------------------------------------------------------------------------o
bool cSkills::AdvanceSkill( CChar *s, UI08 sk, bool skillUsed )
{
	bool advSkill = false;
	SI16 skillGain;
	
	SI08 skillAdvance = FindSkillPoint( sk, s->GetBaseSkill( sk ) );
	
	if( skillUsed )
		skillGain = ( cwmWorldState->skill[sk].advancement[skillAdvance].success ) * 10;
	else
		skillGain = ( cwmWorldState->skill[sk].advancement[skillAdvance].failure ) * 10;

	if( skillGain > RandomNum( 0, 1000 ) )
	{
		advSkill = true;
		if( RandomNum( 0, 99 ) <= 10 )
		{
			if( s->GetSkillLock( sk ) == 0 && s->GetBaseSkill( sk ) > 0 )
				s->SetBaseSkill( s->GetBaseSkill( sk ) + 1, sk );
		}
		else if( s->GetSkillLock( sk ) == 0 )
			Atrophy( s, sk );
	}
	
	if( s->GetSkillLock( sk ) != 2 ) // if it's locked, stats can't advance
		AdvanceStats( s, sk, skillUsed );
	return advSkill;
}

//o---------------------------------------------------------------------------o
//|   Function    :  SI08 cSkills::FindSkillPoint( UI08 sk, int value )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Unknown
//o---------------------------------------------------------------------------o
SI08 cSkills::FindSkillPoint( UI08 sk, int value )
{
	SI08 retVal = -1;
	for( unsigned int iCounter = 0; iCounter < cwmWorldState->skill[sk].advancement.size() - 1; ++iCounter )
	{
		if( cwmWorldState->skill[sk].advancement[iCounter].base <= value && value < cwmWorldState->skill[sk].advancement[iCounter+1].base )
		{
			retVal = iCounter;
			break;
		}
	}
	if( retVal == -1 )
		retVal = static_cast< SI08 >(cwmWorldState->skill[sk].advancement.size() - 1);
	return retVal;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::AdvanceStats( CChar *s, UI08 sk, bool skillsuccess )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Advance players stats
//o---------------------------------------------------------------------------o
void cSkills::AdvanceStats( CChar *s, UI08 sk, bool skillsuccess ) 
{ 
    CRace *pRace = Races->Race( s->GetRace() ); 
	
    // If the Race is invalid just use the default race
    if( pRace == NULL )
		pRace = Races->Race( 0 );
	
    bool IsPlayer = ( !s->IsNpc() && s->GetCommandLevel() < CNS_CMDLEVEL ); 
	if( !IsPlayer ) 
		return;
	
    //make sure socket is no npc nor counsi/gm
    bool update = false;

    UI32 ServStatCap = cwmWorldState->ServerData()->ServerStatCapStatus(); 
    UI32 ttlStats = s->ActualStrength() + s->ActualDexterity() + s->ActualIntelligence(); 
    SI16 chanceStatGain = 0; //16bit because of freaks that raises it > 100 
    int StatCount, nCount; 
	int maxChance = 1000;
    SI16 ActualStat[3] = { s->ActualStrength() , s->ActualDexterity() , s->ActualIntelligence() }; 
    UI16 StatModifier[3] = { cwmWorldState->skill[sk].strength , cwmWorldState->skill[sk].dexterity , cwmWorldState->skill[sk].intelligence }; 
	
	UI16 skillUpdTrig = s->GetScriptTrigger();
	cScript *skillTrig = Trigger->GetScript( skillUpdTrig );
	
    for ( StatCount = STRENGTH; StatCount <= INTELLECT; ++StatCount ) 
	{ 
		nCount = StatCount - ALLSKILLS - 1;  

		//  the following will calculate the chances for str/dex/int to increase 
		//
		//  it is divided into 2 "dices":
		//  first dice: get the success-skillpoint of the stat out of skills.dfn in dependence
		//				of the racial statcap for this stat... x means stat is x% of statcap
		//				=> get skillpoint for x
		//  sec. dice:	get the chance for this stat to increase when the used skill has been used
		//				(out of skills.dfn) => get x = statmodifier of skill

		//  last make it a integer between 0 and 1000 normally (negative or 0==no chance) 

		//	special dice 1: the stat wont increase above x% of the racial statcap. x% is equivalent to dice 2.
		//  special dice 2: skill failed: decrease chance by 50%

		//  k, first let us calculate both dices
		chanceStatGain = (SI16)                               
			(((float)cwmWorldState->skill[StatCount-1].advancement[ FindSkillPoint( StatCount-1, (int)( (float)ActualStat[nCount] / (float)pRace->Skill( StatCount ) * 100 ) ) ].success / 100) 
			* 
			((float)( (float)(StatModifier[nCount]) / 10 ) / 100) * 1000); 
		// some mathematics in it ;) 

		// now, lets implement the special dice 1 and additionally check for onStatGain javascript method
		if( StatModifier[nCount] <= (int)( (float)ActualStat[nCount] / (float)pRace->Skill( StatCount ) * 100 ) )
			chanceStatGain = 0;

		// special dice 2
		if( !skillsuccess )
			maxChance = maxChance * 2;
		
		if( ActualStat[nCount] < pRace->Skill( StatCount ) && chanceStatGain > RandomNum( 0, maxChance ) ) // if stat of char < racial statcap and chance for statgain > random number from 0 to 100 
		{ 
			switch( StatCount ) 
			{ 
				case STRENGTH: 
					s->IncStrength(); 
					break; 
				case DEXTERITY: 
					s->IncDexterity(); 
					break; 
				case INTELLECT: 
					s->IncIntelligence(); 
					break; 
				default: 
					break; 
			} 
			update = true;
			
			if( skillTrig != NULL )
			{
				if( !skillTrig->OnStatGained( s, StatCount, sk ) )
					skillTrig->OnStatChange( s, StatCount );
			}
			
			break;//only one stat at a time fellas
		}
	}
	
	
	/*if total stats (before gain) is above a random number from 0 to the stat cap 
	(hence the closer we get to the cap, the more likely, and at or above the cap, 
	always) then this statement will execute.  This statement removes a stat from one
	of the other stats... just like OSI... so it makes it real hard to get to the cap.
	ALSO this prevents char's stats from FREEZEING when they reach the cap*/
	
	//k will only take away from other stats if within 10 of the server stat cap
	if( static_cast<int>(ttlStats) >= RandomNum( ServStatCap-10, ServStatCap ) )
	{
		UI08 skillDrop = 0;
		
		switch( StatCount )
		{
			case INTELLECT:
				if( RandomNum( 0, 1 ) ) 
				{
					if( ActualStat[0] > 10 )
						skillDrop = STRENGTH;
					else if( ActualStat[1] > 10 )
						skillDrop = DEXTERITY;
					else
						skillDrop = INTELLECT;
				}
				else
				{
					if( ActualStat[1] > 10 )
						skillDrop = DEXTERITY;
					else if( ActualStat[0] > 10 )
						skillDrop = STRENGTH;
					else
						skillDrop = INTELLECT;
				}
				break;
			case DEXTERITY:
				if( RandomNum( 0, 1 ) ) 
				{
					if( ActualStat[0] > 10 )
						skillDrop = STRENGTH;
					else if( ActualStat[2] > 10 )
						skillDrop = INTELLECT;
					else
						skillDrop = DEXTERITY;
				}
				else
				{
					if( ActualStat[2] > 10 )
						skillDrop = INTELLECT;
					else if( ActualStat[0] > 10 )
						skillDrop = STRENGTH;
					else
						skillDrop = DEXTERITY;
				}
				break;
			case STRENGTH:
				if( RandomNum( 0, 1 ) ) 
				{
					if( ActualStat[1] > 10 )
						skillDrop = DEXTERITY;
					else if( ActualStat[2] > 10 )
						skillDrop = INTELLECT;
					else
						skillDrop = STRENGTH;
				} 
				else 
				{
					if( ActualStat[2] > 10 )
						skillDrop = INTELLECT;
					else if( ActualStat[1] >10 )
						skillDrop = DEXTERITY;
					else
						skillDrop = STRENGTH;
				}
				break;
		}
		
		if( update )
		{
			switch( skillDrop )
			{
				case STRENGTH:	s->DecStrength();			break;
				case DEXTERITY:	s->DecDexterity();			break;
				case INTELLECT:	s->DecIntelligence();		break;
			}
			
			if( skillTrig != NULL )
			{
				if( !skillTrig->OnStatLoss( s, skillDrop ) )
					skillTrig->OnStatChange( s, skillDrop );
			}
		}
	}
	
	cSocket *jSock = calcSocketObjFromChar( s );
	if( jSock != NULL )
	{
		jSock->statwindow( s );
		for( UI08 i = 0; i < ALLSKILLS; ++i )
			updateSkillLevel( s, i );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::NewMakeMenu( cSocket *s, int menu, UI08 skill )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  New make menu system, based on create.scp
//o---------------------------------------------------------------------------o
void cSkills::NewMakeMenu( cSocket *s, int menu, UI08 skill )
{
	VALIDATESOCKET( s );
	CChar *ourChar = s->CurrcharObj();;
	s->AddID( menu );
	std::map< UI16, createMenu >::iterator p = actualMenus.find( menu );
	if( p == actualMenus.end() )
		return;
	UI16 background = cwmWorldState->ServerData()->BackgroundPic();
	UI16 btnCancel	= cwmWorldState->ServerData()->ButtonCancel();
	UI16 btnLeft	= cwmWorldState->ServerData()->ButtonLeft();
	UI16 btnRight	= cwmWorldState->ServerData()->ButtonRight();

	CPSendGumpMenu toSend;
	toSend.UserID( INVALIDSERIAL );
	toSend.GumpID( 12 );

	createMenu	ourMenu		= p->second;
	UI32		textCounter = 0;
	ScriptSection *GumpHeader = FileLookup->FindEntry( "ADDMENU HEADER", misc_def );
	if( GumpHeader == NULL )
	{
		toSend.AddCommand( "noclose" );
		toSend.AddCommand( "resizepic 0 0 %i 400 320", background );
		toSend.AddCommand( "page 0" );
		toSend.AddCommand( "text 200 20 0 0" );
		toSend.AddText( "Create menu" );
		++textCounter;
		toSend.AddCommand( "button 40 300 %i %i 1 0 1", btnCancel, btnCancel + 1 );
	}
	else
	{
		UString tag, data, UTag;
		for( tag = GumpHeader->First(); !GumpHeader->AtEnd(); tag = GumpHeader->Next() )
		{
			UTag = tag.upper();
			data = GumpHeader->GrabData();
			if( UTag == "BUTTONLEFT" )
				btnLeft = data.toUShort();
			else if( UTag == "BUTTONRIGHT" )
				btnRight = data.toUShort();
			else if( UTag == "BUTTONCANCEL" )
				btnCancel = data.toUShort();
			else
			{
				std::string built = tag;
				if( !data.empty() )
				{
					built += " ";
					built += data;
				}
				toSend.AddCommand( built );
			}
		}
		ScriptSection *GumpText = FileLookup->FindEntry( "ADDMENU TEXT", misc_def );
		if( GumpText != NULL )
		{
			for( tag = GumpText->First(); !GumpText->AtEnd(); tag = GumpText->Next() )
			{
				data = GumpText->GrabData();
				std::string built = tag;
				if( !data.empty() )
				{
					built += " ";
					built += data;
				}
				toSend.AddText( built );
				++textCounter;
			}
		}
	}

	//UI32 iCounter;
	UI16 xLoc = 60, yLoc = 40;
	std::map< UI16, createEntry >::iterator imIter;
	std::map< UI16, createMenuEntry >::iterator smIter;
	int actualItems = 0, iCounter = 0;
	for( ourMenu.iIter = ourMenu.itemEntries.begin(); ourMenu.iIter != ourMenu.itemEntries.end(); ++ourMenu.iIter )
//	for( iCounter = 0; iCounter < ourMenu.itemEntries.size(); ++iCounter )
	{
		if( (actualItems%6) == 0 && actualItems != 0 )
		{
			xLoc += 180;
			yLoc = 40;
		}
		imIter = itemsForMenus.find( (*ourMenu.iIter) );
//		iIter = itemsForMenus.find( ourMenu.itemEntries[iCounter] );
		if( imIter != itemsForMenus.end() )
		{
			createEntry iItem = imIter->second;
			bool canMake = true;
			for( unsigned int sCounter = 0; sCounter < iItem.skillReqs.size() && canMake; ++sCounter )
			{
				UI08 skillNum = iItem.skillReqs[sCounter].skillNumber;
				UI16 ourSkill = ourChar->GetSkill( skillNum );
				UI16 minSkill = iItem.skillReqs[sCounter].minSkill;
				//UI16 maxSkill = iItem.skillReqs[sCounter].maxSkill;
				//canMake = ( ourSkill >= minSkill && ourSkill <= maxSkill * 1.5 );
				canMake = ( ourSkill >= minSkill );
				// it was not thought that way, its not logical, maxSkill should say, that you can get maxRank with maxSkill and higher
			}
			if( iItem.spell ) // Should only display the spell if character has a spellbook and the spell in his book - Zane
			{
				CItem *spellBook = FindItemOfType( ourChar, IT_SPELLBOOK );
				if( !ValidateObject( spellBook ) )
					canMake = false;
				else
				{
					unsigned getCir = (int)( iItem.spell * .1 );
					unsigned getSpell = iItem.spell - ( getCir * 10 ) + 1;
					if( !Magic->CheckBook( getCir, getSpell - 1, spellBook ) )
						canMake = false;
				}
			}
			if( canMake )
			{
				toSend.AddCommand( "button %i %i %i %i 1 0 %i", xLoc - 40, yLoc, btnRight, btnRight + 1, 10 + ++iCounter );
				toSend.AddCommand( "tilepic %i %i %i", xLoc - 20, yLoc, iItem.targID );
				toSend.AddCommand( "text %i %i 35 %i", xLoc + 20, yLoc, textCounter++ );
				toSend.AddText( iItem.name );
				yLoc += 40;
				++actualItems;
			}
		}
	}

	for( ourMenu.mIter = ourMenu.menuEntries.begin(); ourMenu.mIter != ourMenu.menuEntries.end(); ++ourMenu.mIter )
	{
		if( (actualItems%6) == 0 && actualItems != 0 )
		{
			xLoc += 180;
			yLoc = 40;
		}
		smIter = skillMenus.find( (*ourMenu.mIter) );
		if( smIter != skillMenus.end() )
		{
			createMenuEntry iMenu = smIter->second;
			toSend.AddCommand( "button %i %i %i %i 1 0 %i", xLoc - 40, yLoc, btnRight, btnRight + 1, 100 + iCounter );
			toSend.AddCommand( "tilepic %i %i %i", xLoc - 20, yLoc, iMenu.targID );
			toSend.AddCommand( "text %i %i 35 %i", xLoc + 20, yLoc, textCounter++ );
			toSend.AddText( iMenu.name );
			yLoc += 40;
			++actualItems;
		}
	}
	toSend.Finalize();
	s->Send( &toSend );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::HandleMakeMenu( cSocket *s, int button, int menu )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles the button pressed in the new make menu
//o---------------------------------------------------------------------------o
void cSkills::HandleMakeMenu( cSocket *s, int button, int menu )
{
	VALIDATESOCKET( s );
	CChar *ourChar = s->CurrcharObj();
	s->AddID( 0 );
	std::map< UI16, createMenu >::iterator p = actualMenus.find( menu );
	if( p == actualMenus.end() )
		return;
	createMenu ourMenu = p->second;
	if( button >= 100 )	// menu pressed
	{
		std::map< UI16, createMenuEntry >::iterator q = skillMenus.find( ourMenu.menuEntries[button-100] );
		if( q == skillMenus.end() )
			return;
		NewMakeMenu( s, q->second.subMenu, 0 );
	}
	else				// item to make
	{
		std::map< UI16, createEntry >::iterator r = itemsForMenus.find( ourMenu.itemEntries[button-10] );
		if( r == itemsForMenus.end() )
			return;
		MakeItem( r->second, ourChar, s, ourMenu.itemEntries[button - 10] );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  createEntry *cSkills::FindItem( UI16 itemNum )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Returns a handle to a createEntry based on an itemNum
//o---------------------------------------------------------------------------o
createEntry *cSkills::FindItem( UI16 itemNum )
{
	std::map< UI16, createEntry >::iterator r = itemsForMenus.find( itemNum );
	if( r == itemsForMenus.end() )
		return NULL;
	return &(r->second);
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::MakeItem( createEntry &toMake, CChar *player, cSocket *sock )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Makes an item selected in the new make menu system
//o---------------------------------------------------------------------------o
void cSkills::MakeItem( createEntry &toMake, CChar *player, cSocket *sock, UI16 itemEntry )
{
	VALIDATESOCKET( sock );
	bool canMake = true;
	// we need to check ALL skills, even if the first one fails
	if( player->GetCommandLevel() < CNS_CMDLEVEL )
	{
		for( unsigned int sCounter = 0; sCounter < toMake.skillReqs.size(); ++sCounter )
		{
			if( !CheckSkill( player, toMake.skillReqs[sCounter].skillNumber, toMake.skillReqs[sCounter].minSkill, toMake.skillReqs[sCounter].maxSkill ) )
				canMake = false;
		}
	}
	UI32 resCounter;
	UI16 toDelete;
	UI16 targColour;
	UI16 targID;
	if( !canMake )
	{
		// delete anywhere up to half of the resources needed
		for( resCounter = 0; resCounter < toMake.resourceNeeded.size(); ++resCounter )
		{
			toDelete = RandomNum( 0, toMake.resourceNeeded[resCounter].amountNeeded / 2 );
			targColour = toMake.resourceNeeded[resCounter].colour;
			targID = toMake.resourceNeeded[resCounter].itemID;
			DeleteItemAmount( player, toDelete, targID, targColour );
		}
		Effects->PlaySound( sock, toMake.soundPlayed, true );
		sock->sysmessage( 984 );
	}
	else
	{
		bool canDelete = true;
		for( resCounter = 0; resCounter < toMake.skillReqs.size(); ++resCounter )
		{
			if( player->SkillUsed( toMake.skillReqs[resCounter].skillNumber ) )
			{
				sock->sysmessage( 1650 );
				return;
			}
		}
		for( resCounter = 0; resCounter < toMake.resourceNeeded.size(); ++resCounter )
		{
			toDelete = toMake.resourceNeeded[resCounter].amountNeeded;
			targColour = toMake.resourceNeeded[resCounter].colour;
			targID = toMake.resourceNeeded[resCounter].itemID;
			if( GetItemAmount( player, targID, targColour ) < toDelete )
				canDelete = false;
		}
		if( !canDelete )
		{
			sock->sysmessage( 1651 );
			return;
		}
		// Delete our resources up front
		for( resCounter = 0; resCounter < toMake.resourceNeeded.size(); ++resCounter )
		{
			toDelete	= toMake.resourceNeeded[resCounter].amountNeeded;
			targColour	= toMake.resourceNeeded[resCounter].colour;
			targID		= toMake.resourceNeeded[resCounter].itemID;
			DeleteItemAmount( player, toDelete, targID, targColour );
		}
		for( resCounter = 0; resCounter < toMake.skillReqs.size(); ++resCounter )
			player->SkillUsed( true, toMake.skillReqs[resCounter].skillNumber );
		Effects->tempeffect( player, player, 41, toMake.delay, itemEntry, 0 );
		if( toMake.delay > 300 )
		{
			for( SI16 i = 0; i < ( toMake.delay / 300 ); ++i )
				Effects->tempeffect( player, player, 42, 300 * i, toMake.soundPlayed, 0 );
		}
	}
}

//o---------------------------------------------------------------------------o
//|		Function	:	void cSkills::RepairMetal( cSocket *s )
//|		Date		:	October 16, 2000
//|		Programmer	:	Thaliq
//|		Modified	:	November 13, 2001 - BelCMC, changed to a metal repair function
//o---------------------------------------------------------------------------o
//|		Purpose     :	Repair armor and weapons.
//o---------------------------------------------------------------------------o

void cSkills::RepairMetal( cSocket *s )
{
	SI16 minSkill = 999, maxSkill = 1000;
	CItem *j = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( j ) || !j->IsMetalType() )
	{
		s->sysmessage( 986 );
		return;
	}

	if( j->GetHP() < j->GetMaxHP() )
	{
		if( j->GetDef() > 0 )
		{
			// Items with > 12 def would have impossible skill req's, with this equation
			if( j->GetDef() <= 12 )
			{
				// Minimum Skill = 61.0 + Defense - 1 / 3 * 100 (0-3 = 61.0, 4-6 = 71.0, ect)
				minSkill = (610 + (int)((j->GetDef() - 1) / 3) * 100);
				// Maximum Skill = 84.9 + Defense - 1 / 3 * 50 (0-3 = 84.9, 4-6 = 89.9, ect)
				maxSkill = (849 + (int)((j->GetDef() - 1) / 3) * 50);
			}
		}
		else if( j->GetHiDamage() > 0 )
		{
			int offset = ( j->GetLoDamage() + j->GetHiDamage() ) / 2;
			// Items with > 25 Avg Damage would have impossible skill req's, with this equation
			if( offset <= 25 )
			{
				// Minimum Skill = 51.0 + Avg Damage - 1 / 5 * 100 (0-5 = 51.0, 6-10 = 61.0, ect)
				minSkill = (510 + (int)((offset - 1) / 5 ) * 100 );
				// Maximum Skill = 79.9 + Avg Damage - 1 / 5 * 50 (0-5 = 79.9, 6-10 = 84.9, ect)
				maxSkill = (799 + (int)((offset-1) / 5) * 50);
			}
		}
		else
		{
			s->sysmessage( 987 );
			return;
		}
	}
	else
	{
		s->sysmessage( 988 );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	if( CheckSkill( mChar, BLACKSMITHING, minSkill, maxSkill ) )
	{
		j->SetHP( j->GetMaxHP() );
		s->sysmessage( 989 );
		Effects->PlaySound( s, 0x002A, true );
	}
	else
		s->sysmessage( 990 );
}

//o---------------------------------------------------------------------------o
//|		Function	:	void cSkills::RepairLeather( cSocket *s )
//|		Date		:	October 16, 2000
//|		Programmer	:	Thaliq
//|		Modified	:	November 13, 2001 - BelCMC, changed to a metal repair function
//o---------------------------------------------------------------------------o
//|		Purpose     :	Repair armor and weapons.
//o---------------------------------------------------------------------------o

void cSkills::RepairLeather( cSocket *s )
{
	SI16 minSkill = 999, maxSkill = 1000;
	CItem *j = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( j ) || !j->IsLeatherType() )
	{
		s->sysmessage( 986 );
		return;
}

	if( j->GetHP() < j->GetMaxHP() )
	{
		if( j->GetDef() > 0 )
		{
			// Items with > 12 def would have impossible skill req's, with this equation
			if( j->GetDef() <= 12 )
			{
				// Minimum Skill = 61.0 + Defense - 1 / 3 * 100 (0-3 = 61.0, 4-6 = 71.0, ect)
				minSkill = (610 + (int)((j->GetDef() - 1) / 3) * 100);
				// Maximum Skill = 84.9 + Defense - 1 / 3 * 50 (0-3 = 84.9, 4-6 = 89.9, ect)
				maxSkill = (849 + (int)((j->GetDef() - 1) / 3) * 50);
			}
		}
		else if( j->GetHiDamage() > 0 )
		{
			int offset = ( j->GetLoDamage() + j->GetHiDamage() ) / 2;
			// Items with > 25 Avg Damage would have impossible skill req's, with this equation
			if( offset <= 25 )
			{
				// Minimum Skill = 51.0 + Avg Damage - 1 / 5 * 100 (0-5 = 51.0, 6-10 = 61.0, ect)
				minSkill = (510 + (int)((offset - 1) / 5 ) * 100 );
				// Maximum Skill = 79.9 + Avg Damage - 1 / 5 * 50 (0-5 = 79.9, 6-10 = 84.9, ect)
				maxSkill = (799 + (int)((offset-1) / 5) * 50);
			}
		}
		else
		{
			s->sysmessage( 987 );
			return;
		}
	}
	else
	{
		s->sysmessage( 988 );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	if( CheckSkill( mChar, TAILORING, minSkill, maxSkill ) )
	{
		j->SetHP( j->GetMaxHP() );
		s->sysmessage( 989 );
		Effects->PlaySound( s, 0x002A, true );
	}
	else
		s->sysmessage( 990 );
}

//o---------------------------------------------------------------------------o
//|		Function	:	void cSkills::RepairBow( cSocket *s )
//|		Date		:	October 16, 2000
//|		Programmer	:	Thaliq
//|		Modified	:	November 13, 2001 - BelCMC, changed to a metal repair function
//o---------------------------------------------------------------------------o
//|		Purpose     :	Repair armor and weapons.
//o---------------------------------------------------------------------------o

void cSkills::RepairBow( cSocket *s )
{
	SI16 minSkill = 999, maxSkill = 1000;
	CItem *j = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( !ValidateObject( j ) || !Combat->getBowType( j ) )
	{
		s->sysmessage( 986 );
		return;
	}

	if( j->GetHP() < j->GetMaxHP() )
	{
		if( j->GetDef() > 0 )
		{
			// Items with > 12 def would have impossible skill req's, with this equation
			if( j->GetDef() <= 12 )
			{
				// Minimum Skill = 61.0 + Defense - 1 / 3 * 100 (0-3 = 61.0, 4-6 = 71.0, ect)
				minSkill = (610 + (int)((j->GetDef() - 1) / 3) * 100);
				// Maximum Skill = 84.9 + Defense - 1 / 3 * 50 (0-3 = 84.9, 4-6 = 89.9, ect)
				maxSkill = (849 + (int)((j->GetDef() - 1) / 3) * 50);
			}
		}
		else if( j->GetHiDamage() > 0 )
		{
			int offset = ( j->GetLoDamage() + j->GetHiDamage() ) / 2;
			// Items with > 25 Avg Damage would have impossible skill req's, with this equation
			if( offset <= 25 )
			{
				// Minimum Skill = 51.0 + Avg Damage - 1 / 5 * 100 (0-5 = 51.0, 6-10 = 61.0, ect)
				minSkill = (510 + (int)((offset - 1) / 5 ) * 100 );
				// Maximum Skill = 79.9 + Avg Damage - 1 / 5 * 50 (0-5 = 79.9, 6-10 = 84.9, ect)
				maxSkill = (799 + (int)((offset-1) / 5) * 50);
			}
		}
		else
		{
			s->sysmessage( 987 );
			return;
		}
	}
	else
	{
		s->sysmessage( 988 );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	if( CheckSkill( mChar, BOWCRAFT, minSkill, maxSkill ) )
	{
		j->SetHP( j->GetMaxHP() );
		s->sysmessage( 989 );
		Effects->PlaySound( s, 0x002A, true );
	}
	else
		s->sysmessage( 990 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Snooping( cSocket *s, CChar *target, CItem *pack )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player snoops another PC/NPC's or a tamed animals pack
//o---------------------------------------------------------------------------o
void cSkills::Snooping( cSocket *s, CChar *target, CItem *pack ) 
{
	CChar *mChar = s->CurrcharObj();

	cSocket *tSock = calcSocketObjFromChar( target );

	if( target->GetCommandLevel() > mChar->GetCommandLevel() )
	{
		s->sysmessage( 991 );
		if( tSock != NULL )
			tSock->sysmessage( 992, mChar->GetName().c_str() );
		return;
	}
	
	if( CheckSkill( mChar, SNOOPING, 0, 1000 ) )
	{
		s->openPack( pack );
		s->sysmessage( 993 );
		cScript *successSnoop = Trigger->GetScript( target->GetScriptTrigger() );
		if( successSnoop != NULL )
			successSnoop->OnSnooped( target, mChar, true );
	}
	else 
	{
		bool doNormal = true;
		cScript *failSnoop = Trigger->GetScript( target->GetScriptTrigger() );
		if( failSnoop != NULL )
			doNormal = !failSnoop->OnSnooped( target, mChar, true );

		if( doNormal )
		{
			s->sysmessage( 991 );
			if( target->IsNpc() )
			{
				if( target->isHuman() )
				{
					target->talk( s, 994 + RandomNum( 0, 2 ), false );
					if( cwmWorldState->ServerData()->SnoopIsCrime() )
					{
						if( RandomNum( 0, 1 ) == 1 && mChar->IsCriminal() )	//	50% chance of calling guards, on second time
							callGuards( target, mChar );
					}
				}
				else
					Effects->playMonsterSound( target, target->GetID(), SND_IDLE );	// Play idle sound, if not human
			}
			else if( tSock != NULL )
				tSock->sysmessage( 997, mChar->GetName().c_str() );
			if( cwmWorldState->ServerData()->SnoopIsCrime() )
				criminal( mChar );
			if( mChar->GetKarma() <= 1000 )
			{
				mChar->SetKarma( mChar->GetKarma() - 10 );
				s->sysmessage( 998 );
			}
		}
	}
}

void cSkills::MakeNecroReg( cSocket *nSocket, CItem *nItem, UI16 itemID )
{
	CItem *iItem = NULL;
	CChar *iCharID = nSocket->CurrcharObj();
	if( itemID >= 0x1B11 && itemID <= 0x1B1C ) // Make bone powder.
	{
		iCharID->emoteAll( 741, true, iCharID->GetName().c_str() );
		Effects->tempeffect( iCharID, iCharID, 9, 0, 0, 0 );
		Effects->tempeffect( iCharID, iCharID, 9, 0, 3, 0 );
		Effects->tempeffect( iCharID, iCharID, 9, 0, 6, 0 );
		Effects->tempeffect( iCharID, iCharID, 9, 0, 9, 0 );
		iItem = Items->CreateItem( nSocket, iCharID, 0x0F8F, 1, 0, OT_ITEM, true );
		if( iItem == NULL ) 
			return;
		iItem->SetName( "bone powder" );
		iItem->SetTempVar( CITV_MOREX, 666 );
		iItem->SetTempVar( CITV_MORE, 1, 1 ); // this will fill more with info to tell difference between ash and bone
		nItem->Delete();
		
	}
	if( itemID == 0x0E24 ) // Make vial of blood.
	{
		if( nItem->GetTempVar( CITV_MORE, 1 ) == 1 )
		{
			iItem = Items->CreateItem( nSocket, iCharID, 0x0F82, 1, 0, OT_ITEM, true );
			if( iItem == NULL ) 
				return;
			iItem->SetBuyValue( 15 );
			iItem->SetTempVar( CITV_MOREX, 666 );
		}
		else
		{
			iItem = Items->CreateItem( nSocket, iCharID, 0x0F7D, 1, 0, OT_ITEM, true );
			if( iItem == NULL ) 
				return;
			iItem->SetBuyValue( 10 );
			iItem->SetTempVar( CITV_MOREX, 666 );
		}
		nItem->IncAmount( -1 );
	}
}

}
