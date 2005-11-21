#include "uox3.h"
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
#include "CJSMapping.h"
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
//|   Function    :  void cSkills::ApplyRank( CSocket *s, CItem *c, UI08 rank )
//|   Date        :  24 August 1999
//|   Programmer  :  Magius(CHE)
//o---------------------------------------------------------------------------o
//|   Purpose     :  Modify variables based on item's rank.
//o---------------------------------------------------------------------------o

void cSkills::ApplyRank( CSocket *s, CItem *c, UI08 rank )
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
//|   Function    :  void cSkills::RegenerateOre( SI16 grX, SI16 grY )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Regenerate Ore based on Server.scp Ore respawn settings
//o---------------------------------------------------------------------------o
void cSkills::RegenerateOre( SI16 grX, SI16 grY, UI08 worldNum )
{
	MapResource_st *orePart	= MapRegion->GetResource( grX, grY, worldNum );
	SI16 oreCeiling			= cwmWorldState->ServerData()->ResOre();
	UI16 oreTimer			= cwmWorldState->ServerData()->ResOreTime();
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

//o---------------------------------------------------------------------------o
//|   Function    :  void MakeOre( UI08 Region, CChar *actor, CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Spawn Ore in players pack when he successfully mines
//o---------------------------------------------------------------------------o
void MakeOre( CSocket& mSock, CChar *mChar, CTownRegion *targRegion )
{
	if( targRegion == NULL || !ValidateObject( mChar ) )
		return;

	const UI16 getSkill			= mChar->GetSkill( MINING );
	const int oreChance			= RandomNum( static_cast< SI32 >(0), targRegion->GetOreChance() );	// find our base ore
	int sumChance				= 0;
	bool oreFound				= false;
	const orePref *toFind		= NULL;
	const miningData *found		= NULL;
	for( size_t currentOre = 0; currentOre < targRegion->GetNumOrePreferences(); ++currentOre )
	{
		toFind = targRegion->GetOrePreference( currentOre );
		if( toFind == NULL )
			continue;

		found = toFind->oreIndex;
		if( found == NULL )
			continue;

		if( sumChance > oreChance || oreChance < ( sumChance + toFind->percentChance ) )
		{
			if( getSkill >= found->minSkill )
			{
				const std::string oreName	= found->name + " ore";
				CItem *oreItem				= Items->CreateItem( &mSock, mChar, 0x19B9, 1, found->colour, OT_ITEM, true );
				if( oreItem != NULL )
				{
					oreItem->SetName( oreName );
					if( RandomNum( 0, 100 ) > targRegion->GetChanceBigOre() )
						oreItem->SetAmount( 5 );
				}

				mSock.sysmessage( 982, oreName.c_str() );
				oreFound = true;
				break;
			}	
		}

		sumChance += toFind->percentChance;
	}
	if( !oreFound )
	{
		if( getSkill >= 850 )
		{
			Items->CreateRandomItem( &mSock, "digginggems" ); 
			mSock.sysmessage( 983 );
		}
		else
			mSock.sysmessage( 1772 );
	}
}

bool MineCheck( CSocket& mSock, CChar *mChar, SI16 targetX, SI16 targetY, SI08 targetZ, UI08 targetID1, UI08 targetID2 )
{
	switch( cwmWorldState->ServerData()->MineCheck() )
	{
		case 0:
			return true;
		case 1:
			if( targetZ == 0 )			// check to see if we're targetting a dungeon floor
			{
				if( targetID1 == 0x05 )
				{
					if( ( targetID2 >= 0x3B && targetID2 <= 0x4F ) || ( targetID2 >= 0x51 && targetID2 <= 0x53 ) || ( targetID2 == 0x6A ) )
						return true;
				}
			}
			if( targetZ >= 0 )	// mountain not likely to be below 0 (but you never know, do you? =)
			{
				if( targetID1 != 0 && targetID2 != 0 )	// we might have a static rock or mountain
				{
					MapStaticIterator msi( targetX, targetY, mChar->WorldNumber() );
					CTile tile;
					for( staticrecord *stat = msi.Next(); stat != NULL; stat = msi.Next() )
					{
						msi.GetTile( &tile );
						if( targetZ == stat->zoff && ( !strcmp( tile.Name(), "rock" ) || !strcmp( tile.Name(), "mountain" ) || !strcmp( tile.Name(), "cave" ) ) )
							return true;
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
						return true; 
				}
			}
			break;
		case 2:	// need to modify scripts to support this!
			return true;
		default:
			mSock.sysmessage( 800 );
			return true;
	}
	return false;
}

//o---------------------------------------------------------------------------o
//| Function    - void cSkills::Mine( CSocket *s )
//| Date        - Unknown
//| Programmer  - Unknown
//| Modified    - Cork(Unknown)/Abaddon(February 19, 2000)
//o---------------------------------------------------------------------------o
//| Purpose     - (Fill this in)
//| Comments    - Skill checking now implemented. You cannot mine colored ore
//|               unless you have the proper mining skill for each ore type. -Cork
//|               Rewrote most of it to clear up some of the mess-Abaddon
//o---------------------------------------------------------------------------o
void cSkills::Mine( CSocket *s )
{
	VALIDATESOCKET( s );
	CSocket& mSock = (*s);

	if( mSock.GetDWord( 11 ) == INVALIDSERIAL )
		return;

	CChar *mChar = mSock.CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	const SI16 targetX = mSock.GetWord( 11 );
	const SI16 targetY = mSock.GetWord( 13 );
	const SI08 targetZ = mSock.GetWord( 16 );

	const SI16 distX = abs( mChar->GetX() - targetX );
	const SI16 distY = abs( mChar->GetY() - targetY );

	if( distX > 5 || distY > 5 )
	{
		mSock.sysmessage( 799 );
		return;
	}
	
	mSock.SetTimer( tPC_SKILLDELAY, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->ServerSkillDelayStatus() )) );

	const UI08 targetID1 = mSock.GetByte( 17 );
	const UI08 targetID2 = mSock.GetByte( 18 );

	if( targetZ < 28 && targetID1 == 0x0E )
	{
		switch( targetID2 )
		{
			case 0xD3:
			case 0xDF:
			case 0xE0:
			case 0xE1:
			case 0xE8:
				GraveDig( s );		// check to see if we targeted a grave, if so, check it
				break;
			default:
				break;
		}
	}

	if( !MineCheck( mSock, mChar, targetX, targetY, targetZ, targetID1, targetID2 ) )
	{
		mSock.sysmessage( 801 );
		return;
	}

	RegenerateOre( targetX, targetY, mChar->WorldNumber() );
	MapResource_st *orePart = MapRegion->GetResource( targetX, targetY, mChar->WorldNumber() );
	if( orePart->oreAmt <= 0 )
	{
		mSock.sysmessage( 802 );
		return;
	}
	if( mChar->IsOnHorse() != 0 )	// do action and sound
		Effects->PlayCharacterAnimation( mChar, 0x1A );
	else
		Effects->PlayCharacterAnimation( mChar, 0x0B );
	
	Effects->PlaySound( &mSock, 0x0125, true ); 
	
	if( CheckSkill( mChar, MINING, 0, 1000 ) ) // check to see if our skill is good enough
	{
		if( orePart->oreAmt > 0 )
			--orePart->oreAmt;
		
#if defined( UOX_DEBUG_MODE )
		Console << "DBG: Mine(\"" << mChar->GetName() << "\"[" << mChar->GetSerial() << "]); --> MINING: " << mChar->GetSkill( MINING ) << "  RaceID: " << mChar->GetRace() << myendl;
#endif
			
		CTownRegion *targetReg = calcRegionFromXY( targetX, targetY, mChar->WorldNumber() );
		if( targetReg == NULL )
			return;

		MakeOre( mSock, mChar, targetReg );
	}
	else
	{
		mSock.sysmessage( 803 );
		if( orePart->oreAmt > 0 && RandomNum( 0, 1 ) )
			--orePart->oreAmt;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::GraveDig( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do GraveDigging Stuff (Should probably be replaced with an
//|											OSI-like Treasure Hunting System based off a Resource
//|											System much like Ore and Logs)
//o---------------------------------------------------------------------------o
void cSkills::GraveDig( CSocket *s )
{
	VALIDATESOCKET( s );
	SI16	nFame;
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
//| Function    - void cSkills::SmeltOre( CSocket *s );
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
void cSkills::SmeltOre( CSocket *s )
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
					CItem *ingot = Items->CreateScriptItem( s, chr, "0x1BF2", ingotNum, OT_ITEM, true );
					if( ingot != NULL )
					{
						ingot->SetName( ingotString );
						ingot->SetColour( oreType->colour );
					}
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
	chr->Dirty( UT_STATWINDOW );
}

//o---------------------------------------------------------------------------o
//|   Function    :  cSkills::handleCooking( CSocket *s )
//|   Date        :  January 30, 2003
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handle baking/cooking in a single function
//o---------------------------------------------------------------------------o
void cSkills::handleCooking( CSocket *s )
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
				bool isMeat = ( ( realItem >= 0x0DE3 && realItem <= 0x0DE9 ) ||	realItem == 0x0FAC ||
					realItem == 0x0FB1 || ( realItem >= 0x197A && realItem <= 0x19B6 ) ||
					( realItem >= 0x0461 && realItem <= 0x0480 ) ||	( realItem >= 0x0E31 && realItem <= 0x0E33 ) ||
					realItem == 0x19BB || realItem == 0x1F2B || ( realItem >= 0x092B && realItem <= 0x0934 ) ||
					( realItem >= 0x0937 && realItem <= 0x0942 ) ||	( realItem >= 0x0945 && realItem <= 0x0950 ) ||
					( realItem >= 0x0953 && realItem <= 0x095E ) ||	( realItem >= 0x0961 && realItem <= 0x096C ) );
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
//|   Function    :  void cSkills::PeaceMaking( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when using the PeaceMaking Skill, Also requires
//|					 adequate Musicianship skill to succeed.
//o---------------------------------------------------------------------------o
void cSkills::PeaceMaking( CSocket *s )
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
		for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
		{
			CMapRegion *MapArea = (*rIter);
			if( MapArea == NULL )	// no valid region
				continue;
			CDataList< CChar * > *regChars = MapArea->GetCharList();
			regChars->Push();
			for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
			{
				if( !ValidateObject( tempChar ) )
					continue;
				if( charInRange( tempChar, mChar ) && tempChar->IsAtWar() )
				{
					CSocket *jSock = tempChar->GetSocket();
					if( jSock != NULL )
						jSock->sysmessage( 1440 );
					if( tempChar->IsAtWar() ) 
						tempChar->ToggleCombat();
					tempChar->SetTarg( NULL );
					tempChar->SetAttacker( NULL );
					tempChar->SetAttackFirst( false );
				}
			}
			regChars->Pop();
		}
	}
	else 
	{
		PlayInstrument( s, getInst, false );
		s->sysmessage( 1441 );
	}
}

//o---------------------------------------------------------------------------o
//|		Function    :	void cSkills::PlayInstrument( CSocket *s, CItem *i, bool wellPlayed )
//|		Date        :	2/10/2003
//|		Programmer  :	Zane
//o---------------------------------------------------------------------------o
//|		Purpose     :	Play an Instrument (plays "played well" sfx if wellPlayed == true
//|						Otherwise plays "played poorly" sfx
//o---------------------------------------------------------------------------o
void cSkills::PlayInstrument( CSocket *s, CItem *i, bool wellPlayed )
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
//|   Function    :  CItem *cSkills::GetInstrument( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Used by Peacemaking, Provocation and Enticement to find
//|					 the instrument in your pack (if any exist)
//o---------------------------------------------------------------------------o
CItem * cSkills::GetInstrument( CSocket *s )
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
	CDataList< CItem * > *xCont = x->GetContainsList();
	for( CItem *i = xCont->First(); !xCont->Finished(); i = xCont->Next() )
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
//|   Function    :  void cSkills::ProvocationTarget1( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check 1st Provoc Target and Add its ID to the socket, then
//|					 bring up a targeting cursor for player to select second
//|					 Provoc Target
//o---------------------------------------------------------------------------o
void cSkills::ProvocationTarget1( CSocket *s )
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
//|   Function    :  void cSkills::EnticementTarget1( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check 1st Entice Target and Add its ID to the socket, then
//|					 bring up a targeting cursor for player to select second
//|					 Entice Target
//o---------------------------------------------------------------------------o
void cSkills::EnticementTarget1( CSocket *s )
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
//|   Function    :  void cSkills::EnticementTarget2( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check 2nd Entice Target and players Entice and Musicianship
//|					 skill then if checks pass flag the player criminal
//o---------------------------------------------------------------------------o
void cSkills::EnticementTarget2( CSocket *s )
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
//|   Function    :  void cSkills::ProvocationTarget2( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check 2nd Provoc Target and players Provoc and Musicianship
//|					 skill.  If either target is Innocent, flag the player criminal
//o---------------------------------------------------------------------------o
void cSkills::ProvocationTarget2( CSocket *s )
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
		for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
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
	cScript *tScript	= JSMapping->GetScript( scpNum );
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
			CSocket *sSock = s->GetSocket();
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
		
		CSocket *mSock = s->GetSocket();
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
	CSocket *mSock = c->GetSocket();
	UI16 skillTrig = c->GetScriptTrigger();
	cScript *scpSkill = JSMapping->GetScript( skillTrig );
		
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
//|   Function    :  void cSkills::ItemIDTarget( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses the ItemID skill on an item, can
//|					 give valuable information on items (Will reveal hidden
//|					 magical names and charges on items as well)
//o---------------------------------------------------------------------------o
void cSkills::ItemIDTarget( CSocket *s )
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
			getTileName( (*i), name );
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
					sprintf( temp, Dictionary->GetEntry( 1548, sLang ).c_str(), cwmWorldState->skill[0-i->GetMadeWith()-1].madeword.c_str(), mCreater->GetName().c_str() );
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
//|   Function    :  void cSkills::FishTarget( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets an area with a fishing pole
//o---------------------------------------------------------------------------o
void cSkills::FishTarget( CSocket *s )
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
		baseTime = static_cast<R32>(cwmWorldState->ServerData()->SystemTimer( tSERVER_FISHINGBASE ) / 25);
		baseTime += RandomNum( 0, static_cast< int >(cwmWorldState->ServerData()->SystemTimer( tSERVER_FISHINGRANDOM ) / 15) );
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
	CSocket *s = i->GetSocket();
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
//|   Function    :  void cSkills::SkillUse( CSocket *s, UI08 x )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses a skill from the skill list
//o---------------------------------------------------------------------------o
void cSkills::SkillUse( CSocket *s, UI08 x )
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
		cScript *skScript = JSMapping->GetScript( mChar->GetScriptTrigger() );
		bool doSwitch = true;
		if( skScript != NULL )
			doSwitch = !skScript->OnSkill( mChar, x );
		else if( cwmWorldState->skill[x].jsScript != 0xFFFF )
		{
			skScript = JSMapping->GetScript( cwmWorldState->skill[x].jsScript );
			if( skScript != NULL )
				doSwitch = !skScript->OnSkill( mChar, x );
		}

		if( doSwitch )
		{
			switch( x )
			{
				case ITEMID:			s->target( 0, TARGET_ITEMID, 857 );			break;
				case PEACEMAKING:		PeaceMaking(s);								break;
				case PROVOCATION:		s->target( 0, TARGET_PROVOCATION, 861 );	break;
				case ENTICEMENT:		s->target( 0, TARGET_ENTICEMENT, 862 );		break;
				case STEALING:
					if( cwmWorldState->ServerData()->RogueStatus() )
						s->target( 0, TARGET_STEALING, 863 );
					else
						s->sysmessage( 864 );
					break;
				case INSCRIPTION:		s->target( 0, TARGET_INSCRIBE, 865 );		break;
				case TRACKING:			TrackingMenu( s, TRACKINGMENUOFFSET );		break;
				case MEDITATION:
					if( cwmWorldState->ServerData()->ArmorAffectManaRegen() )
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
//|   Function    :  void cSkills::RandomSteal( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets a PC/NPC with stealing skill
//|					 instead of an item (randomly pics an item in their pack)
//o---------------------------------------------------------------------------o
void cSkills::RandomSteal( CSocket *s )
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
		item = p->GetContainsList()->GetCurrent( RandomNum( static_cast< size_t >(0), p->GetContainsList()->Num() - 1 ) );
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
//|   Function    :  void cSkills::StealingTarget( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets an item with stealing skill
//o---------------------------------------------------------------------------o
void cSkills::StealingTarget( CSocket *s )
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
//|   Function    :  void cSkills::doStealing( CSocket *s, CChar *mChar, CChar *npc, CItem *item )
//|   Date        :  2/13/2003
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do the bulk of stealing stuff rather than having the same
//|						code twice in RandomSteal() and StealingTarget()
//o---------------------------------------------------------------------------o
void cSkills::doStealing( CSocket *s, CChar *mChar, CChar *npc, CItem *item )
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
			cScript *toExecute	= JSMapping->GetScript( targTrig );
			if( toExecute != NULL )
				toExecute->OnSteal( mChar, item );

			targTrig	= npc->GetScriptTrigger();
			toExecute	= JSMapping->GetScript( targTrig );
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
				getTileName( (*item), tileName );
				sprintf( temp, Dictionary->GetEntry( 884 ).c_str(), mChar->GetName().c_str(), tileName.c_str() );
				sprintf( temp2, Dictionary->GetEntry( 885 ).c_str(), mChar->GetName().c_str(), tileName.c_str(), npc->GetName().c_str() );
			}

			CSocket *npcSock = npc->GetSocket();
			if( npcSock != NULL )
				npcSock->sysmessage( temp );

			SOCKLIST nearbyChars = FindNearbyPlayers( mChar );
			for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
			{
				CSocket *iSock = (*cIter);
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
//|   Function    :  void cSkills::Tracking( CSocket *s, int selection )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Start tracking selected NPC/PC
//o---------------------------------------------------------------------------o
void cSkills::Tracking( CSocket *s, int selection )
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
//|   Function    :  void cSkills::CreateTrackingMenu( CSocket *s, int m )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Brings up Tracking Menu, Called when player uses Tracking Skill
//o---------------------------------------------------------------------------o
void cSkills::CreateTrackingMenu( CSocket *s, UI16 m )
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
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *MapArea = (*rIter);
		if( MapArea == NULL )	// no valid region
			continue;
		CDataList< CChar * > *regChars = MapArea->GetCharList();
		regChars->Push();
		for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
		{
			if( !ValidateObject( tempChar ) )
				continue;
			id					= tempChar->GetID();
			CSocket *tSock		= tempChar->GetSocket();
			bool cmdLevelCheck	= isOnline( (*tempChar) ) && ( c->GetCommandLevel() > tempChar->GetCommandLevel() );
			if( objInRange( tempChar, c, static_cast<UI16>(distance) ) && !tempChar->IsDead() && id >= id1 && id <= id2 && tSock != s && ( cmdLevelCheck || tempChar->IsNpc() ) )
			{
				c->SetTrackingTargets( tempChar, static_cast<UI08>(MaxTrackingTargets) );
				++MaxTrackingTargets;
				if( MaxTrackingTargets >= cwmWorldState->ServerData()->TrackingMaxTargets() ) 
				{
					regChars->Pop();
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
		regChars->Pop();
	}
	
	if( MaxTrackingTargets == 0 )
	{
		s->sysmessage( type );
		return;
	}
	toSend.Finalize();
	s->Send( &toSend );
}

void HandleCommonGump( CSocket *mSock, ScriptSection *gumpScript, UI16 gumpIndex );
//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::TrackingMenu( CSocket *s, int gmindex )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Unknown
//o---------------------------------------------------------------------------o
void cSkills::TrackingMenu( CSocket *s, UI16 gmindex )
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
	CSocket *s = i->GetSocket();
	VALIDATESOCKET( s );
	CChar *trackTarg = i->GetTrackingTarget();
	if( !ValidateObject( trackTarg ) || trackTarg->GetY() == -1 ) 
		return;
	CPTrackingArrow tSend = (*trackTarg);
	tSend.Active( 1 );
	s->Send( &tSend );
}

//o---------------------------------------------------------------------------o
//|   Function    :  UI08 cSkills::TrackingDirection( CSocket *s, CChar *i )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Get location of the tracking target based on your loc
//o---------------------------------------------------------------------------o
UI08 cSkills::TrackingDirection( CSocket *s, CChar *i )
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
//|   Function    :  void cSkills::Inscribe( CSocket *s )
//|   Date        :  January 31, 2003
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when a player attempts to inscribe a blank scroll
//|					 Based upon inscribe.dfn
//o---------------------------------------------------------------------------o
void cSkills::Inscribe( CSocket *s )
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
//|   Function    :  bool cSkills::EngraveAction( CSocket *s, CItem *i, int getCir, int getSpell )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player starts engraving an item
//o---------------------------------------------------------------------------o
bool cSkills::EngraveAction( CSocket *s, CItem *i, int getCir, int getSpell )
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
//|   Function    :  void cSkills::LockPick( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses lockpics on a locked item
//o---------------------------------------------------------------------------o
void cSkills::LockPick( CSocket *s )
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
//|   Function    :  void cSkills::TinkerAxel( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Use tinkering on an Axel
//o---------------------------------------------------------------------------o
void cSkills::TinkerAxel( CSocket *s )
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
//|   Function    :  void cSkills::TinkerAwg( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Unknown : Tinkering
//o---------------------------------------------------------------------------o
void cSkills::TinkerAwg( CSocket *s )
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
//|   Function    :  void cSkills::TinkerClock( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Used when player Tinkers a clock
//o---------------------------------------------------------------------------o
void cSkills::TinkerClock( CSocket *s )
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
//|   Function    :  void cSkills::Meditation( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses meditation skill
//o---------------------------------------------------------------------------o
void cSkills::Meditation( CSocket *s )
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
//|   Function    :  void cSkills::Persecute( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when a Ghost attacks a Player.  If entry in server.scp
//|					 is enabled, players mana decreases each time you try to
//|					 persecute him
//o---------------------------------------------------------------------------o
void cSkills::Persecute( CSocket *s )
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
			CSocket *tSock = targChar->GetSocket();
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
//|   Function    :  void cSkills::Smith( CSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player attempts to smith an item
//o---------------------------------------------------------------------------o
void cSkills::Smith( CSocket *s )
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
//|   Function    :  void cSkills::AnvilTarget( CSocket *s, CItem& item, SI16 oreType )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Unknown : Smithy
//o---------------------------------------------------------------------------o
void cSkills::AnvilTarget( CSocket *s, CItem& item, miningData *oreType )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();

	REGIONLIST nearbyRegions = MapRegion->PopulateList( mChar );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *MapArea = (*rIter);
		if( MapArea == NULL )	// no valid region
			continue;
		CDataList< CItem * > *regItems = MapArea->GetItemList();
		regItems->Push();
		for( CItem *tempItem = regItems->First(); !regItems->Finished(); tempItem = regItems->Next() )
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
						regItems->Pop();
						return;
					}
					NewMakeMenu( s, oreType->makemenu, BLACKSMITHING );
					regItems->Pop();
					return;
				}
			}
		}
		regItems->Pop();
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
	ores.resize( 0 );
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
		if( !oreNameList.empty() )
		{
			rvalue = true;
			ScriptSection *individualOre = NULL;
			STRINGLIST_CITERATOR toCheck;
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
	{
		Shutdown( FATAL_UOX3_ORELIST );
		return;
	}

	Console.PrintDone();
	
	Console << "Loading creation menus         ";	
	LoadCreateMenus();
	Console.PrintDone();

	CJSMappingSection *skillSection = JSMapping->GetSection( SCPT_SKILLUSE );
	for( cScript *ourScript = skillSection->First(); !skillSection->Finished(); ourScript = skillSection->Next() )
	{
		if( ourScript != NULL )
			ourScript->ScriptRegistration( "Skill" );
	}

	Console.PrintSectionBegin();
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
//|   Function    :  void cSkills::LoadCreateMenus( void )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Load Create menus from the create DFNs
//o---------------------------------------------------------------------------o
void cSkills::LoadCreateMenus( void )
{
	UString tag, data, UTag;
	UI16 ourEntry;							// our actual entry number
	for( Script *ourScript = FileLookup->FirstScript( create_def ); !FileLookup->FinishedScripts( create_def ); ourScript = FileLookup->NextScript( create_def ) )
	{
		if( ourScript == NULL )
			continue;

		for( ScriptSection *toSearch = ourScript->FirstEntry(); toSearch != NULL; toSearch = ourScript->NextEntry() )
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
				tmpEntry.soundPlayed	= 0;

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
	for( size_t iCounter = 0; iCounter < cwmWorldState->skill[sk].advancement.size() - 1; ++iCounter )
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
	cScript *skillTrig = JSMapping->GetScript( skillUpdTrig );
	
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
	if( ttlStats >= RandomNum( ServStatCap-10, ServStatCap ) )
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

	s->Dirty( UT_STATWINDOW );
	for( UI08 i = 0; i < ALLSKILLS; ++i )
		updateSkillLevel( s, i );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::NewMakeMenu( CSocket *s, int menu, UI08 skill )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  New make menu system, based on create.scp
//o---------------------------------------------------------------------------o
void cSkills::NewMakeMenu( CSocket *s, int menu, UI08 skill )
{
	VALIDATESOCKET( s );
	CChar *ourChar = s->CurrcharObj();
	s->AddID( menu );
	std::map< UI16, createMenu >::const_iterator p = actualMenus.find( menu );
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
	int actualItems = 0;
//	for( iCounter = 0; iCounter < ourMenu.itemEntries.size(); ++iCounter )
	for( ourMenu.iIter = ourMenu.itemEntries.begin(); ourMenu.iIter != ourMenu.itemEntries.end(); ++ourMenu.iIter )
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
			for( size_t sCounter = 0; sCounter < iItem.skillReqs.size() && canMake; ++sCounter )
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
					int getCir = (int)( iItem.spell * .1 );
					int getSpell = iItem.spell - ( getCir * 10 ) + 1;
					if( !Magic->CheckBook( getCir, getSpell - 1, spellBook ) )
						canMake = false;
				}
			}
			if( canMake )
			{
				toSend.AddCommand( "button %i %i %i %i 1 0 %i", xLoc - 40, yLoc, btnRight, btnRight + 1, (*ourMenu.iIter) );
				toSend.AddCommand( "tilepic %i %i %i", xLoc - 20, yLoc, iItem.targID );
				toSend.AddCommand( "text %i %i 35 %i", xLoc + 20, yLoc, textCounter++ );
				toSend.AddText( iItem.name );
				yLoc += 40;
				++actualItems;
			}
		}
	}

	actualItems = 0;
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
			toSend.AddCommand( "button %i %i %i %i 1 0 %i", xLoc - 40, yLoc, btnRight, btnRight + 1, 1000 + (*ourMenu.mIter) );
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
//|   Function    :  void cSkills::HandleMakeMenu( CSocket *s, int button, int menu )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles the button pressed in the new make menu
//o---------------------------------------------------------------------------o
void cSkills::HandleMakeMenu( CSocket *s, int button, int menu )
{
	VALIDATESOCKET( s );
	CChar *ourChar = s->CurrcharObj();
	s->AddID( 0 );
	std::map< UI16, createMenu >::const_iterator p = actualMenus.find( menu );
	if( p == actualMenus.end() )
		return;
	createMenu ourMenu = p->second;
	if( button >= 1000 )	// menu pressed
	{
		std::map< UI16, createMenuEntry >::const_iterator q = skillMenus.find( button-1000 );
		if( q == skillMenus.end() )
			return;
		NewMakeMenu( s, q->second.subMenu, 0 );
	}
	else				// item to make
	{
		std::map< UI16, createEntry >::iterator r = itemsForMenus.find( button );
		if( r == itemsForMenus.end() )
			return;
		MakeItem( r->second, ourChar, s, button );
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
//|   Function    :  void cSkills::MakeItem( createEntry &toMake, CChar *player, CSocket *sock )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Makes an item selected in the new make menu system
//o---------------------------------------------------------------------------o
void cSkills::MakeItem( createEntry &toMake, CChar *player, CSocket *sock, UI16 itemEntry )
{
	VALIDATESOCKET( sock );
	bool canMake = true;
	// we need to check ALL skills, even if the first one fails
	if( player->GetCommandLevel() < CNS_CMDLEVEL )
	{
		for( size_t sCounter = 0; sCounter < toMake.skillReqs.size(); ++sCounter )
		{
			if( !CheckSkill( player, toMake.skillReqs[sCounter].skillNumber, toMake.skillReqs[sCounter].minSkill, toMake.skillReqs[sCounter].maxSkill ) )
				canMake = false;
		}
	}
	size_t resCounter;
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
		if( toMake.soundPlayed )
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
		if( toMake.soundPlayed )
		{
			if( toMake.delay > 300 )
			{
				for( SI16 i = 0; i < ( toMake.delay / 300 ); ++i )
					Effects->tempeffect( player, player, 42, 300 * i, toMake.soundPlayed, 0 );
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|		Function	:	void cSkills::RepairMetal( CSocket *s )
//|		Date		:	October 16, 2000
//|		Programmer	:	Thaliq
//|		Modified	:	November 13, 2001 - BelCMC, changed to a metal repair function
//o---------------------------------------------------------------------------o
//|		Purpose     :	Repair armor and weapons.
//o---------------------------------------------------------------------------o

void cSkills::RepairMetal( CSocket *s )
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

void callGuards( CChar *mChar, CChar *targChar );
//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Snooping( CSocket *s, CChar *target, CItem *pack )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player snoops another PC/NPC's or a tamed animals pack
//o---------------------------------------------------------------------------o
void cSkills::Snooping( CSocket *s, CChar *target, CItem *pack ) 
{
	CChar *mChar = s->CurrcharObj();

	CSocket *tSock = target->GetSocket();

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
		cScript *successSnoop = JSMapping->GetScript( target->GetScriptTrigger() );
		if( successSnoop != NULL )
			successSnoop->OnSnooped( target, mChar, true );
	}
	else 
	{
		bool doNormal = true;
		cScript *failSnoop = JSMapping->GetScript( target->GetScriptTrigger() );
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
				{
					UI16 toPlay = cwmWorldState->creatures[target->GetID()].GetSound( SND_IDLE );
					if( toPlay != 0x00 )
						Effects->PlaySound( target, toPlay );
				}
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

void cSkills::MakeNecroReg( CSocket *nSocket, CItem *nItem, UI16 itemID )
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
