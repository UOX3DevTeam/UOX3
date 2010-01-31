#include "uox3.h"
#include "magic.h"
#include "skills.h"
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

const UI16 CREATE_MENU_OFFSET = 5000;	// This is how we differentiate a menu button from an item button (and the limit on ITEM=# in create.dfn)

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
//|   Function    :  void cSkills::ApplyRank( CSocket *s, CItem *c, UI08 rank, UI08 maxrank )
//|   Date        :  24 August 1999
//|   Programmer  :  Magius(CHE)
//o---------------------------------------------------------------------------o
//|   Purpose     :  Modify variables based on item's rank.
//o---------------------------------------------------------------------------o

void cSkills::ApplyRank( CSocket *s, CItem *c, UI08 rank, UI08 maxrank )
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
		if( c->GetResist( PHYSICAL ) > 0 )      
			c->SetResist( (UI16)( ( rank * c->GetResist( PHYSICAL ) ) / 10 ), PHYSICAL );
		if( c->GetHP() > 0 )      
			c->SetHP( (SI16)( ( rank * c->GetHP() ) / 10 ) );
		if( c->GetMaxHP() > 0 )   
			c->SetMaxHP( (SI16)( ( rank * c->GetMaxHP() ) / 10 ) );
		if( c->GetBuyValue() > 0 )
			c->SetBuyValue( (UI32)( ( rank * c->GetBuyValue() ) / 10 ) );

		// Convert item's rank to a value between 1 and 10, to fit rank system messages
		UI08 tempRank = floor((( rank * 100 ) / maxrank ) / 10 );

		if( tempRank >= 1 && tempRank <= 10 )
			s->sysmessage( 783 + tempRank );
		else if( tempRank < 1 )
			s->sysmessage( 784 );

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

		sumChance += toFind->percentChance;
		if( sumChance > oreChance )
		{
			if( getSkill >= found->minSkill )
			{
				UI08 amtToMake = 1;
				if( targRegion->GetChanceBigOre() >= RandomNum( 0, 100 ) )
					amtToMake = 5;
				CItem *oreItem = Items->CreateItem( &mSock, mChar, 0x19B9, amtToMake, found->colour, OT_ITEM, true );
				if( ValidateObject( oreItem ) )
				{
					const std::string oreName = found->name + " ore";
					oreItem->SetName( oreName );
					mSock.sysmessage( 982, oreName.c_str() );
				}
				oreFound = true;
				break;
			}	
		}
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
					CStaticIterator msi( targetX, targetY, mChar->WorldNumber() );
					for( Static_st *stat = msi.Next(); stat != NULL; stat = msi.Next() )
					{
						CTile& tile = Map->SeekTile( stat->itemid );
						if( targetZ == stat->zoff && ( !strcmp( tile.Name(), "rock" ) || !strcmp( tile.Name(), "mountain" ) || !strcmp( tile.Name(), "cave" ) ) )
							return true;
					}
				}
				else		// or it could be a map only
				{  
					// manually calculating the ID's if a maptype
					const map_st map1 = Map->SeekMap( targetX, targetY, mChar->WorldNumber() );
					CLand& land = Map->SeekLand( map1.id );
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
	const SI08 targetZ = mSock.GetByte( 16 );

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
					if( chr->GetSkill( MINING ) < oreType->minSkill )
					{
						s->sysmessage( 815 );
						return;
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
					CItem *ingot = Items->CreateScriptItem( s, chr, "0x1BF2", ingotNum, OT_ITEM, true, oreType->colour );
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
	const UI16 scpNum	= s->GetScriptTrigger();
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

		if( s->GetSkill( sk ) >= highSkill )
			return true;


		chanceskillsuccess = (SI32)( (R32)( ( (R32)( s->GetSkill( sk ) - lowSkill ) / 1000.0f ) +
									 (R32)( (R32)( s->GetStrength() * cwmWorldState->skill[sk].strength ) / 100000.0f ) +
									 (R32)( (R32)( s->GetDexterity() * cwmWorldState->skill[sk].dexterity ) / 100000.0f ) +
									 (R32)( (R32)( s->GetIntelligence() * cwmWorldState->skill[sk].intelligence  ) / 100000.0f ) ) * 1000 );
		
		// chanceskillsuccess is a number between 0 and 1000, lets throw the dices now
		if( s->GetCommandLevel() > 0 )
			skillCheck = true;
		else
			skillCheck = ( chanceskillsuccess >= RandomNum( 0, UOX_MIN( 1000, (highSkill+100) ) ) );
		
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
//|   Function    :  void cSkills::HandleSkillChange( CChar *c, UI16 sk )
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
void cSkills::HandleSkillChange( CChar *c, UI08 sk, SI08 skillAdvance, bool success )
{
	UI32 totalSkill			= 0;
	UI08 rem				= 0;
	UI08 atrop[ALLSKILLS+1];
	UI08 toDec				= 0xFF;
	UI08 counter			= 0;
	CSocket *mSock			= c->GetSocket();
	const UI16 skillTrig	= c->GetScriptTrigger();
	cScript *scpSkill		= JSMapping->GetScript( skillTrig );
	UI08 amtToGain			= 1;
	if( success )
		amtToGain			= cwmWorldState->skill[sk].advancement[skillAdvance].amtToGain;
	UI16 skillCap			= cwmWorldState->ServerData()->ServerSkillCapStatus();

	if( c->IsNpc() )
	{
		c->SetBaseSkill( c->GetBaseSkill( sk ) + amtToGain, sk );
		if( scpSkill != NULL )
		{
			if( !scpSkill->OnSkillGain( c, sk ) )
				scpSkill->OnSkillChange( c, sk );
		}
		return;
	}
	
	if( mSock == NULL )
		return;

	srand( getclock() ); // Randomize
	
	atrop[ALLSKILLS] = 0;//set the last of out copy array
	for( counter = 0; counter < ALLSKILLS; ++counter )
	{
		atrop[counter] = c->GetAtrophy( counter );
	}

	for( counter = ALLSKILLS; counter > 0; --counter )
	{//add up skills and find the one being increased
		UI08 atrSkill = c->GetAtrophy( static_cast<UI08>(counter-1) );
		
		if( c->GetBaseSkill( atrSkill ) >= amtToGain && c->GetSkillLock( atrSkill ) == SKILL_DECREASE && atrSkill != sk )
			toDec = atrSkill;//we found a skill that can be decreased, save it for later.

		totalSkill += c->GetBaseSkill( static_cast<UI08>(counter-1) );
		
		atrop[counter] = atrop[counter-1];

		if( atrop[counter] == sk )
			rem = counter;//remember this number
	}

	atrop[0] = sk;//set the first one to our current skill
	
	//copy it back in
	for( counter = 0; counter < rem; ++counter )
		c->SetAtrophy( atrop[counter], counter );
	if( rem != ALLSKILLS )//in the middle somewhere or first
	{
		for( counter = static_cast<UI08>(rem + 1); counter <= ALLSKILLS; ++counter )
			c->SetAtrophy( atrop[counter], ( counter -1 ));
	}

	if( RandomNum( static_cast<UI16>(0), skillCap ) <= static_cast<UI16>(totalSkill) )
	{
		if( toDec != 0xFF )
		{
			totalSkill -= amtToGain;
			c->SetBaseSkill( c->GetBaseSkill( toDec ) - amtToGain, toDec );
			if( scpSkill != NULL )
			{
				if( !scpSkill->OnSkillLoss( c, toDec ) )
					scpSkill->OnSkillChange( c, toDec );
			}
			mSock->updateskill( toDec );
		}
	}

	if( skillCap > static_cast<UI16>(totalSkill) )
	{
		c->SetBaseSkill( c->GetBaseSkill( sk ) + amtToGain, sk );
		if( scpSkill != NULL )
		{
			if( !scpSkill->OnSkillGain( c, sk ) )
				scpSkill->OnSkillChange( c, sk );
		}
		mSock->updateskill( sk );
	}
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

	const SI16 targetX = s->GetWord( 0x0B );
	const SI16 targetY = s->GetWord( 0x0D );
	const SI08 targetZ = s->GetByte( 0x10 );
	const SI16 distX = abs( mChar->GetX() - targetX );
	const SI16 distY = abs( mChar->GetY() - targetY );

	const UI08 targetID1 = s->GetByte( 0x11 );
	const UI08 targetID2 = s->GetByte( 0x12 );

	CItem *targetItem = calcItemObjFromSer( s->GetDWord( 7 ) );
	bool validLocation = false;
	if( ValidateObject( targetItem ) )
		validLocation = Map->SeekTile( targetItem->GetID() ).CheckFlag( TF_WET );
	else if( targetID1 != 0 && targetID2 != 0 )
	{
		CStaticIterator msi( targetX, targetY, mChar->WorldNumber() );
		for( Static_st *stat = msi.First(); stat != NULL; stat = msi.Next() )
		{
			CTile& tile = Map->SeekTile( stat->itemid );
			if( targetZ == stat->zoff && tile.CheckFlag( TF_WET ) )	// right place, and wet
				validLocation = true;
		}
	}
	else		// or it could be a map only
	{  
		// manually calculating the ID's if a maptype
		const map_st map1 = Map->SeekMap( targetX, targetY, mChar->WorldNumber() );
		CLand& land = Map->SeekLand( map1.id );
		if( land.CheckFlag( TF_WET ) )
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
void cSkills::Fish( CSocket *mSock, CChar *mChar )
{
	if( !CheckSkill( mChar, FISHING, 0, 1000 ) ) 
	{
		mSock->sysmessage( 847 );
		return;
	}
	const UI16 getSkill = mChar->GetSkill( FISHING );
	switch( RandomNum( 0, 25 ) )
	{
		case 1:
			if( getSkill > 920 )
			{
				Items->CreateRandomItem( mSock, "fishtreasure" );	// random paintings
				mSock->sysmessage( 848 );
			}
			break;
		case 2:
			if( getSkill > 970 )
			{
				Items->CreateRandomItem( mSock, "fishweapons" );	// Some new weapons
				mSock->sysmessage( 849 );
			}
			break;
		case 3:	// Random gold and gems
			if( RandomNum( 0, 12 ) )
			{
				Items->CreateRandomItem( mSock, "fishgems" );
				mSock->sysmessage( 850 );
			}
			else
			{	// Create between 200 and 1300 gold
				UI16 nAmount = RandomNum( 200, 1300 );
				Items->CreateItem( mSock, mChar, 0x0EED, nAmount, 0, OT_ITEM, true );
				Effects->goldSound( mSock, nAmount );
				mSock->sysmessage( 851, nAmount );
			}
			break;
		case 4:
			if( getSkill > 850 )
			{
				Items->CreateRandomItem( mSock, "fishbones" );	// Random bones and crap
				mSock->sysmessage( 852 );
			}
			break;
		default:
			Items->CreateRandomItem( mSock, "randomfish" );	// User defined fish
			mSock->sysmessage( 853 );
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
void ClilocMessage( CSocket *mSock, UI08 type, UI16 hue, UI16 font, UI32 messageNum, const char *types = "", ... );
void cSkills::SkillUse( CSocket *s, UI08 x )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( mChar->IsDead() )
	{
		ClilocMessage( s, 6, 0x0040, FNT_NORMAL, 500012 ); 
		//s->sysmessage( 392 );
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
		if( doSwitch && cwmWorldState->skill[x].jsScript != 0xFFFF )
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
				case STEALING:
					if( cwmWorldState->ServerData()->RogueStatus() )
						s->target( 0, TARGET_STEALING, 863 );
					else
						s->sysmessage( 864 );
					break;
				case TRACKING:			TrackingMenu( s, TRACKINGMENUOFFSET );		break;
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
	CDataList< CItem * > *tcCont = p->GetContainsList();
	const size_t numItems = tcCont->Num();

	std::vector< CItem * > contList;
	contList.reserve( numItems );
	if( numItems > 0 )
	{
		for( item = tcCont->First(); !tcCont->Finished(); item = tcCont->Next() )
		{
			if( ValidateObject( item ) )
				contList.push_back( item );
		}
		item = contList[RandomNum( static_cast<size_t>(0), (contList.size()-1) )];
	}
	contList.clear();

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
	if( npc->GetNPCAiType() == AI_PLAYERVENDOR )
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
		
		const int getDefOffset	= UOX_MIN( stealCheck + ( (int)( ( Combat->calcDef( mChar, 0, false ) - 1) / 10 ) * 100 ), 990 );
		const bool canSteal		= CheckSkill( mChar, STEALING, getDefOffset, 1000);
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
				npc->TextMessage( NULL, 883, TALK, false );
			
			if( WillResultInCriminal( mChar, npc ) )
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
	const UString sect = "TRACKINGMENU " + UString::number( m );
	ScriptSection *TrackStuff = FileLookup->FindEntry( sect, menus_def );
	if( TrackStuff == NULL )
		return;

	enum CreatureTypes
	{
	CT_ANIMAL = 0,
	CT_CREATURE,
	CT_PERSON
	};

	CChar *mChar			= s->CurrcharObj();
	UI16 id					= 0;
	CreatureTypes creatureType		= CT_ANIMAL;
	SI32 type				= 887;
	UI08 MaxTrackingTargets = 0;
	const UI16 distance		= (cwmWorldState->ServerData()->TrackingBaseRange() + (mChar->GetSkill( TRACKING ) / 50));
	UnicodeTypes mLang		= s->Language();
	
	if( m == ( 2 + TRACKINGMENUOFFSET ) )
	{
		creatureType	= CT_CREATURE;
		type			= 888;
	}
	else if( m == ( 3 + TRACKINGMENUOFFSET ) )
	{
		creatureType	= CT_PERSON;
		type			= 889;
	}

	std::string line;
	CPOpenGump toSend( *mChar );
	if( m >= TRACKINGMENUOFFSET )
		toSend.GumpIndex( m );
	else
		toSend.GumpIndex( m + TRACKINGMENUOFFSET );
	UString tag		= TrackStuff->First();
	UString data	= TrackStuff->GrabData();

	line = tag + " " + data;
	toSend.Question( line );

	REGIONLIST nearbyRegions = MapRegion->PopulateList( mChar );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *MapArea = (*rIter);
		if( MapArea == NULL )	// no valid region
			continue;
		CDataList< CChar * > *regChars = MapArea->GetCharList();
		regChars->Push();
		for( CChar *tempChar = regChars->First(); !regChars->Finished() && MaxTrackingTargets < cwmWorldState->ServerData()->TrackingMaxTargets(); tempChar = regChars->Next() )
		{
			if( !ValidateObject( tempChar ) )
				continue;
			id = tempChar->GetID();
			if( ( !tempChar->isHuman() || creatureType == CT_PERSON ) && ( !cwmWorldState->creatures[id].IsAnimal() || creatureType == CT_ANIMAL ) )
			{
				const bool cmdLevelCheck = ( isOnline( (*tempChar) ) && ( mChar->GetCommandLevel() >= tempChar->GetCommandLevel() ) );
				if( tempChar != mChar && objInRange( tempChar, mChar, distance ) && !tempChar->IsDead() && ( cmdLevelCheck || tempChar->IsNpc() ) )
				{
					mChar->SetTrackingTargets( tempChar, MaxTrackingTargets );
					++MaxTrackingTargets;

					SI32 dirMessage = 898;
					switch( Movement->Direction( mChar, tempChar->GetX(), tempChar->GetY() ) )
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
	const UString sect			= "TRACKINGMENU " + UString::number( gmindex );
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
//|   Function    :  void cSkills::updateSkillLevel( CChar *c, UI08 s )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Calculate the skill of this character based on the 
//|					 characters baseskill and stats
//o---------------------------------------------------------------------------o
void cSkills::updateSkillLevel( CChar *c, UI08 s ) const 
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
			targChar->TextMessage( NULL, 974, EMOTE, true, targChar->GetName().c_str() );
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
					if( getAmt == 0 )
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
					toAdd.makemenu	= 0;
					toAdd.minSkill	= 0;
					toAdd.name		= oreName;
					for( tag = individualOre->First(); !individualOre->AtEnd(); tag = individualOre->Next() )
					{
						UTag = tag.upper();
						data = individualOre->GrabData();
						switch( (UTag.data()[0]) )	// break on tag
						{
							case 'C':	
								if( UTag == "COLOUR" )
									toAdd.colour = data.toUShort();
								break;
							case 'F':
								break;
							case 'M':
								if( UTag == "MAKEMENU" )
									toAdd.makemenu = data.toLong();
								else if( UTag == "MINSKILL" )
									toAdd.minSkill = data.toUShort();
								break;
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
	actualMenus.clear();
	skillMenus.clear();
	itemsForMenus.clear();

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
						if( data.sectionCount( " " ) > 0 )
						{
							if( data.sectionCount( " " ) == 1 )
								tmpResource.amountNeeded	= data.section( " ", 1 ).toUByte();
							else
							{
								tmpResource.amountNeeded	= data.section( " ", 1, 1 ).toUByte();
								tmpResource.colour			= data.section( " ", 2, 2 ).toUShort();
							}
						}
						UString resType = "RESOURCE " + data.section( " ", 0, 0 );
						ScriptSection *resList = FileLookup->FindEntry( resType, create_def );
						if( resList != NULL )
						{
							UString resData;
							for( UString resTag = resList->First(); !resList->AtEnd(); resTag = resList->Next() )
							{
								resData = resList->GrabData();
								tmpResource.idList.push_back( resData.toUShort() );
							}
						}
						else
							tmpResource.idList.push_back( data.section( " ", 0, 0 ).toUShort() );

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
		skillGain = ( cwmWorldState->skill[sk].advancement[skillAdvance].success );
	else
		skillGain = ( cwmWorldState->skill[sk].advancement[skillAdvance].failure );

	if( skillGain > RandomNum( 0, 100 ) )
	{
		advSkill = true;
		if( s->GetSkillLock( sk ) == SKILL_INCREASE )
			HandleSkillChange( s, sk, skillAdvance, skillUsed );
	}
	
	if( s->GetSkillLock( sk ) != SKILL_LOCKED ) // if it's locked, stats can't advance
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
 
    //make sure socket is no npc
	if( s->IsNpc() ) 
		return;
	
    UI32 ServStatCap = cwmWorldState->ServerData()->ServerStatCapStatus(); 
    UI32 ttlStats = s->ActualStrength() + s->ActualDexterity() + s->ActualIntelligence(); 
    SI16 chanceStatGain = 0; //16bit because of freaks that raises it > 100 
    int StatCount, nCount;
	int toDec = 255;
	UI16 maxChance = 100;
    SI16 ActualStat[3] = { s->ActualStrength() , s->ActualDexterity() , s->ActualIntelligence() }; 
    UI16 StatModifier[3] = { cwmWorldState->skill[sk].strength , cwmWorldState->skill[sk].dexterity , cwmWorldState->skill[sk].intelligence }; 
	SkillLock StatLocks[3] = { s->GetSkillLock( STRENGTH ), s->GetSkillLock( DEXTERITY ), s->GetSkillLock( INTELLECT ) }; 

	UI16 skillUpdTrig = s->GetScriptTrigger();
	cScript *skillTrig = JSMapping->GetScript( skillUpdTrig );

	
    for ( StatCount = STRENGTH; StatCount <= INTELLECT; ++StatCount ) 
	{ 
		nCount = StatCount - ALLSKILLS - 1;  

		// if current stat isn't allowed to increase skip it.
		if( StatLocks[nCount] == SKILL_INCREASE )
		{

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
			UI08 modifiedStatLevel = FindSkillPoint( StatCount-1, (int)( (float)ActualStat[nCount] / (float)pRace->Skill( StatCount ) * 100 ) );
			chanceStatGain = (SI16)(((float)cwmWorldState->skill[StatCount-1].advancement[modifiedStatLevel].success / 100) * ((float)( (float)(StatModifier[nCount]) / 10 ) / 100) * 1000);
			// some mathematics in it ;) 

			// now, lets implement the special dice 1 and additionally check for onStatGain javascript method
			if( StatModifier[nCount] <= (int)( (float)ActualStat[nCount] / (float)pRace->Skill( StatCount ) * 100 ) )
				chanceStatGain = 0;

			// special dice 2
			if( !skillsuccess )
				maxChance *= 2;
		
			if( ActualStat[nCount] < pRace->Skill( StatCount ) && chanceStatGain > RandomNum( static_cast<UI16>(0), maxChance ) ) // if stat of char < racial statcap and chance for statgain > random number from 0 to 100 
			{ 
				// Check if we have to decrease a stat
				if( ( ttlStats + 1) >= RandomNum( ServStatCap-10, ServStatCap ) )
				{
					for( int i = 0; i < 3; i++)
					{
						if( StatLocks[i] == SKILL_DECREASE )
						{
							// Decrease the highest stat, that is allowed to decrease
							if( toDec == 255 ) 
								toDec = i;
							else
								if( ActualStat[i] > ActualStat[toDec] )
									toDec = i;
						}
					}
	
					switch( toDec ) 
					{ 
						case 0: 
							s->IncStrength( -1 );
							ttlStats--;
							if( skillTrig != NULL )
							{
								if( !skillTrig->OnStatLoss( s, STRENGTH ) )
								skillTrig->OnStatChange( s, STRENGTH );
							}
							break; 
						case 1: 
							s->IncDexterity( -1 );
							ttlStats--;
							if( skillTrig != NULL )
							{
								if( !skillTrig->OnStatLoss( s, DEXTERITY ) )
								skillTrig->OnStatChange( s, DEXTERITY );
							}
							break; 
						case 2: 
							s->IncIntelligence( -1 );
							ttlStats--;
							if( skillTrig != NULL )
							{
								if( !skillTrig->OnStatLoss( s, INTELLECT ) )
								skillTrig->OnStatChange( s, INTELLECT );
							}
							break; 
						default: 
							break; 
					} 
					
				}
	
				// Do we still hit the stat limit?
				if( ( ttlStats + 1) <= ServStatCap )
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
			
					if( skillTrig != NULL )
					{
						if( !skillTrig->OnStatGained( s, StatCount, sk ) )
							skillTrig->OnStatChange( s, StatCount );
					}
				
					break;//only one stat at a time fellas
				}
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
		toSend.AddCommand( "button 360 15 %i %i 1 0 1", btnCancel, btnCancel + 1 );
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

	UI16 xLoc = 60, yLoc = 40;
	std::map< UI16, createEntry >::iterator imIter;
	std::map< UI16, createMenuEntry >::iterator smIter;
	int actualItems = 0;
	for( ourMenu.iIter = ourMenu.itemEntries.begin(); ourMenu.iIter != ourMenu.itemEntries.end(); ++ourMenu.iIter )
	{
		if( (actualItems%6) == 0 && actualItems != 0 )
		{
			xLoc += 180;
			yLoc = 40;
		}
		imIter = itemsForMenus.find( (*ourMenu.iIter) );
		if( imIter != itemsForMenus.end() )
		{
			createEntry iItem = imIter->second;
			bool canMake = true;
			for( size_t sCounter = 0; sCounter < iItem.skillReqs.size() && canMake; ++sCounter )
			{
				UI08 skillNum = iItem.skillReqs[sCounter].skillNumber;
				UI16 ourSkill = ourChar->GetSkill( skillNum );
				UI16 minSkill = iItem.skillReqs[sCounter].minSkill;
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
				if( iItem.targID )
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
			toSend.AddCommand( "button %i %i %i %i 1 0 %i", xLoc - 40, yLoc, btnRight, btnRight + 1, CREATE_MENU_OFFSET + (*ourMenu.mIter) );
			if( iMenu.targID )
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
	if( button >= CREATE_MENU_OFFSET )	// menu pressed
	{
		std::map< UI16, createMenuEntry >::const_iterator q = skillMenus.find( button-CREATE_MENU_OFFSET );
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

	std::vector< resAmountPair >::const_iterator resCounter;
	std::vector< resSkillReq >::const_iterator sCounter;
	resAmountPair resEntry;
	UI16 toDelete;
	UI16 targColour;
	UI16 targID;
	bool canDelete = true;

	//Moved resource-check to top of file to disallow gaining skill by attempting to
	//craft items without having enough resources to do so :P
	for( resCounter = toMake.resourceNeeded.begin(); resCounter != toMake.resourceNeeded.end(); ++resCounter )
	{
		resEntry	= (*resCounter);
		toDelete	= resEntry.amountNeeded;
		targColour	= resEntry.colour;
		for( std::vector< UI16 >::const_iterator idCounter = resEntry.idList.begin(); idCounter != resEntry.idList.end(); ++idCounter )
		{
			targID = (*idCounter);
			toDelete -= UOX_MIN( GetItemAmount( player, targID, targColour ), static_cast<UI32>(toDelete) );
			if( toDelete == 0 )
				break;
		}
		if( toDelete > 0 )
			canDelete = false;
	}
	if( !canDelete )
	{
		sock->sysmessage( 1651 );
		return;
	}

	bool canMake = true;

	// we need to check ALL skills, even if the first one fails
	for( sCounter = toMake.skillReqs.begin(); sCounter != toMake.skillReqs.end(); ++sCounter )
	{
		if( player->SkillUsed( (*sCounter).skillNumber ) )
		{
			sock->sysmessage( 1650 );
			return;
		}

		if( !CheckSkill( player, (*sCounter).skillNumber, (*sCounter).minSkill, (*sCounter).maxSkill ) )
			canMake = false;
	}

	if( !canMake )
	{
		// delete anywhere up to half of the resources needed
		if( toMake.soundPlayed )
			Effects->PlaySound( sock, toMake.soundPlayed, true );
		sock->sysmessage( 984 );
	}
	else
	{
		for( sCounter = toMake.skillReqs.begin(); sCounter != toMake.skillReqs.end(); ++sCounter )
			player->SkillUsed( true, (*sCounter).skillNumber );

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
	for( resCounter = toMake.resourceNeeded.begin(); resCounter != toMake.resourceNeeded.end(); ++resCounter )
	{
		resEntry	= (*resCounter);
		toDelete	= resEntry.amountNeeded;
		if( !canMake )
			toDelete = RandomNum( 0, toDelete / 2 );
		targColour	= resEntry.colour;
		for( std::vector< UI16 >::const_iterator idCounter = resEntry.idList.begin(); idCounter != resEntry.idList.end(); ++idCounter )
		{
			targID = (*idCounter);
			toDelete -= DeleteItemAmount( player, toDelete, targID, targColour );
			if( toDelete == 0 )
				break;
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
		if( j->GetResist( PHYSICAL ) > 0 )
		{
			// Items with > 12 def would have impossible skill req's, with this equation
			if( j->GetResist( PHYSICAL ) <= 12 )
			{
				// Minimum Skill = 61.0 + Defense - 1 / 3 * 100 (0-3 = 61.0, 4-6 = 71.0, ect)
				minSkill = (610 + (int)((j->GetResist( PHYSICAL ) - 1) / 3) * 100);
				// Maximum Skill = 84.9 + Defense - 1 / 3 * 50 (0-3 = 84.9, 4-6 = 89.9, ect)
				maxSkill = (849 + (int)((j->GetResist( PHYSICAL ) - 1) / 3) * 50);
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
					target->TextMessage( s, 994 + RandomNum( 0, 2 ), TALK, false );
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
		iCharID->TextMessage( NULL, 741, EMOTE, true, iCharID->GetName().c_str() );
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
