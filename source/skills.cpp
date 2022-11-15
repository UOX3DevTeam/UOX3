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
#include "StringUtility.hpp"
#include "weight.h"
#include <algorithm>

using namespace std::string_literals;

bool CheckItemRange( CChar *mChar, CItem *i );

CSkills *Skills = nullptr;

const UI16 CREATE_MENU_OFFSET = 5000;	// This is how we differentiate a menu button from an item button (and the limit on ITEM=# in create.dfn)

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::CalcRankAvg()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate average rank of item based on player's skillpoints in skills
//|					required to craft item
//o------------------------------------------------------------------------------------------------o
SI32 CSkills::CalcRankAvg( CChar *player, CreateEntry_st& skillMake )
{
	// If rank system is not enabled, assume perfectly crafted item each time
	if( !cwmWorldState->ServerData()->RankSystemStatus() )
		return 10;

	R32 rankSum = 0;
	SI32 rk_range, rank;
	R32 sk_range, randnum, randnum1;

	// Calculate base chance of success at crafting based on primary skill, to use as a modifier for exceptional chance later
	R32 chanceSkillSuccess = 0;
	chanceSkillSuccess = static_cast<R32>( player->GetSkill( skillMake.skillReqs[0].skillNumber ) - skillMake.skillReqs[0].minSkill );
	chanceSkillSuccess /= ( static_cast<R32>( skillMake.skillReqs[0].maxSkill ) - static_cast<R32>( skillMake.skillReqs[0].minSkill ));

	// Loop through all skills required to craft the item and calculate average rank value for each
	for( size_t i = 0; i < skillMake.skillReqs.size(); ++i )
	{
		// Fetch the range of rank (min/max) values the item being crafted can have
		rk_range = skillMake.maxRank - skillMake.minRank;

		// Fetch range of skill we have to work with for this skill based on player's skill value and minimum skill required to craft the item
		sk_range = static_cast<R32>( player->GetSkill( skillMake.skillReqs[i].skillNumber ) - skillMake.skillReqs[i].minSkill );
		if( sk_range <= 0 )
		{
			sk_range = skillMake.minRank * 10;
		}
		else if( sk_range >= 1000 )
		{
			sk_range = skillMake.maxRank * 10;
		}

		// If generated number is under skill range value...
		randnum = static_cast<R32>( RandomNum( 0, 999 ));
		if( randnum <= sk_range )
		{
			rank = skillMake.maxRank; // ...assume perfectly crafted item!
		}
		else
		{
			// ...otherwise, generate a random number modified by skill range and difficulty level of rank system from uox.ini
			randnum1 = static_cast<R32>( RandomNum( 0, 999 )) - (( randnum - sk_range ) / ( 11 - cwmWorldState->ServerData()->SkillLevel() ));

			// Modify random number based on base chance of success at crafting
			randnum1 *= chanceSkillSuccess;

			// Calculate rank value for this skill
			rank = static_cast<SI32>(( randnum1 * rk_range ) / 1000 );
			rank += skillMake.minRank - 1;
			if( rank > skillMake.maxRank )
			{
				rank = skillMake.maxRank;
			}
			if( rank < skillMake.minRank )
			{
				rank = skillMake.minRank;
			}
		}

		// Add rank value for this skill to the total
		rankSum += rank;
	}

	// Return the average rank value of all skills required to craft the item
	return static_cast<SI32>( rankSum / skillMake.skillReqs.size() );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::ApplyRank()
//|	Date		-	24 August 1999
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Modify item properties based on item's rank.
//o------------------------------------------------------------------------------------------------o
void CSkills::ApplyRank( CSocket *s, CItem *c, UI08 rank, UI08 maxrank )
{
	char tmpmsg[512];
	*tmpmsg='\0';

	// Only apply rank system if enabled in ini, and only for non-pileable items!
	if( cwmWorldState->ServerData()->RankSystemStatus() && !c->IsPileable() )
	{
		c->SetRank( rank );

		if( c->GetLoDamage() > 0 )
		{
			c->SetLoDamage( static_cast<SI16>(( rank * c->GetLoDamage() ) / 10 ));
		}
		if( c->GetHiDamage() > 0 )
		{
			c->SetHiDamage( static_cast<SI16>(( rank * c->GetHiDamage() ) / 10 ));
		}
		if( c->GetResist( PHYSICAL ) > 0 )
		{
			c->SetResist( static_cast<UI16>(( rank * c->GetResist( PHYSICAL )) / 10 ), PHYSICAL );
		}
		if( c->GetHP() > 0 )
		{
			c->SetHP( static_cast<SI16>(( rank * c->GetHP() ) / 10 ));
		}
		if( c->GetMaxHP() > 0 )
		{
			c->SetMaxHP( static_cast<SI16>(( rank * c->GetMaxHP() ) / 10 ));
		}
		if( c->GetBuyValue() > 0 )
		{
			c->SetBuyValue( static_cast<UI32>(( rank * c->GetBuyValue() ) / 10 ));
		}
		if( c->GetMaxUses() > 0 )
		{
			c->SetUsesLeft( static_cast<UI16>(( rank * c->GetMaxUses() ) / 10 ));
		}
		if( c->GetId() == 0x22c5 && c->GetMaxHP() > 0 ) // Runebook
		{
			// Max charges for runebook stored in maxHP property, defaults to 10, ranges from 5-10 based on rank
			c->SetMaxHP( static_cast<UI16>( std::max( static_cast<UI16>( 5 ), static_cast<UI16>(( rank * c->GetMaxHP() ) / 10 ))));
		}

		// Convert item's rank to a value between 1 and 10, to fit rank system messages
		UI08 tempRank = floor( static_cast<R32>((( rank * 100 ) / maxrank ) / 10 ));

		if( tempRank >= 1 && tempRank <= 10 )
		{
			s->SysMessage( 783 + tempRank ); // =You cannot use that material for carpentry.
		}
		else if( tempRank < 1 )
		{
			s->SysMessage( 784 ); // You made an item with no quality!
		}
	}
	else
	{
		c->SetRank( rank );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::RegenerateOre()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Regenerate Ore in a resource region based on UOX.INI Ore respawn settings
//o------------------------------------------------------------------------------------------------o
void CSkills::RegenerateOre( SI16 grX, SI16 grY, UI08 worldNum )
{
	MapResource_st *orePart	= MapRegion->GetResource( grX, grY, worldNum );
	SI16 oreCeiling			= cwmWorldState->ServerData()->ResOre();
	UI16 oreTimer			= cwmWorldState->ServerData()->ResOreTime();
	if( static_cast<UI32>( orePart->oreTime) <= cwmWorldState->GetUICurrentTime() )	// regenerate some more?
	{
		for( SI16 counter = 0; counter < oreCeiling; ++counter )	// keep regenerating ore
		{
			if( orePart->oreAmt < oreCeiling && static_cast<UI32>( orePart->oreAmt + counter * oreTimer * 1000 ) < cwmWorldState->GetUICurrentTime() )
			{
				++orePart->oreAmt;
			}
			else
			{
				break;
			}
		}
		orePart->oreTime = BuildTimeValue( static_cast<R32>( oreTimer ));
	}
	if( orePart->oreAmt > oreCeiling )
	{
		orePart->oreAmt = oreCeiling;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MakeOre()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Spawn Ore in players pack when he successfully uses mining skill
//o------------------------------------------------------------------------------------------------o
void MakeOre( CSocket& mSock, CChar *mChar, CTownRegion *targRegion )
{
	if( targRegion == nullptr || !ValidateObject( mChar ))
		return;

	const UI16 getSkill			= mChar->GetSkill( MINING );
	const SI32 findOreChance	= RandomNum( static_cast<SI32>( 0 ), targRegion->GetOreChance() );	// find our base ore
	SI32 sumChance				= 0;
	bool oreFound				= false;
	const OrePref_st *toFind		= nullptr;
	const MiningData_st *found		= nullptr;
	for( size_t currentOre = 0; currentOre < targRegion->GetNumOrePreferences(); ++currentOre )
	{
		toFind = targRegion->GetOrePreference( currentOre );
		if( toFind == nullptr )
			continue;

		found = toFind->oreIndex;
		if( found == nullptr )
			continue;

		sumChance += toFind->percentChance;
		if( sumChance > findOreChance )
		{
			if( getSkill >= found->minSkill )
			{
				UI08 amtToMake = 1;
				if( targRegion->GetChanceBigOre() >= RandomNum( 1, 100 ))
				{
					amtToMake = 5;
				}

				// Randomize the size of ore found
				UI16 oreId = RandomNum( 0x19B7, 0x19BA );
				CItem *oreItem = Items->CreateItem( &mSock, mChar, oreId, amtToMake, found->colour, OT_ITEM, true );
				if( ValidateObject( oreItem ))
				{
					const std::string oreName = found->name + " ore";
					oreItem->SetName( oreName );
					if( oreItem->GetCont() != nullptr )
					{
						mSock.SysMessage( 982, oreName.c_str() ); // You place some %s in your pack.
					}
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
			mSock.SysMessage( 983 ); // You place a gem in your pack.
		}
		else
		{
			mSock.SysMessage( 1772 ); // You do not have enough skill to mine that ore!
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MineCheck()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check where player is attempting to mine versus MINECHECK setting in UOX.INI
//o------------------------------------------------------------------------------------------------o
bool MineCheck( CSocket& mSock, CChar *mChar, SI16 targetX, SI16 targetY, SI08 targetZ, UI08 targetId1, UI08 targetId2 )
{
	switch( cwmWorldState->ServerData()->MineCheck() )
	{
		case 0:
			return true;
		case 1:
			if( targetZ == 0 ) // check to see if we're targetting a dungeon floor
			{
				if( targetId1 == 0x05 )
				{
					if(( targetId2 >= 0x3B && targetId2 <= 0x4F ) || ( targetId2 >= 0x51 && targetId2 <= 0x53 ) || ( targetId2 == 0x6A ))
					{
						return true;
					}
				}
			}
			if( targetZ >= 0 ) // mountain not likely to be below 0 (but you never know, do you? =)
			{
				if( targetId1 != 0 && targetId2 != 0 ) // we might have a static rock or mountain
				{
					auto artwork = Map->ArtAt( targetX, targetY, mChar->WorldNumber() );
					for( auto &tile : artwork )
					{
						auto name = tile.name();
						if( targetZ == tile.altitude && (( name.find( "rock" ) != std::string::npos ) 
							|| ( name.find( "mountain" ) != std::string::npos ) || ( name.find( "cave" ) != std::string::npos )))
						{
							return true;
						}
					}
				}
				else // or it could be a map only
				{
					// manually calculating the ID's if a maptype
					auto map1 = Map->SeekMap( targetX, targetY, mChar->WorldNumber() );
					
					if(( map1.name().find( "rock" ) != std::string::npos ) 
						|| ( map1.name().find( "mountain" ) != std::string::npos ) || ( map1.name().find( "cave" ) != std::string::npos ))
					{
						return true;
					}
				}
			}
			break;
		case 2:	// need to modify scripts to support this!
			return true;
		default:
			mSock.SysMessage( 800 ); // Unknown mine check option, contact your shard op!
			return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::Mine()
//|	Modified	-	(February 19, 2000)
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles player's attempts to use mining skill
//|	Notes	    -	Skill checking now implemented. You cannot mine colored ore unless you have the
//|					proper mining skill for each ore type. -Cork
//|					Rewrote most of it to clear up some of the mess
//o------------------------------------------------------------------------------------------------o
void CSkills::Mine( CSocket *s )
{
	VALIDATESOCKET( s );
	CSocket& mSock = ( *s );
	CChar *mChar = s->CurrcharObj();

	// Check if item used to initialize target cursor is still within range
	CItem *miningTool = static_cast<CItem *>( s->TempObj() );
	s->TempObj( nullptr );
	if( ValidateObject( miningTool ))
	{
		if( !CheckItemRange( mChar, miningTool ))
		{
			s->SysMessage( 400 ); // That is too far away!
			return;
		}
	}

	// Don't allow mining without a valid tool
	if( !ValidateObject( miningTool ))
		return;

	if( mSock.GetDWord( 11 ) == INVALIDSERIAL )
		return;

	if( !ValidateObject( mChar ))
		return;

	// Check if player targeted an item, and if so, if he's in the same world/instance as said item
	CItem *targetItem = CalcItemObjFromSer( mSock.GetDWord( 7 ));
	if( ValidateObject( targetItem ))
	{
		if( mChar->WorldNumber() != targetItem->WorldNumber() )
			return;

		if( mChar->GetInstanceId() != targetItem->GetInstanceId() )
			return;
	}

	const SI16 targetX = mSock.GetWord( 11 );
	const SI16 targetY = mSock.GetWord( 13 );
	const SI08 targetZ = mSock.GetByte( 16 );

	const SI16 distX = abs( mChar->GetX() - targetX );
	const SI16 distY = abs( mChar->GetY() - targetY );

	if( distX > 5 || distY > 5 )
	{
		mSock.SysMessage( 799 ); // You are too far away to mine that!
		return;
	}

	const UI08 targetId1 = mSock.GetByte( 17 );
	const UI08 targetId2 = mSock.GetByte( 18 );

	if( targetZ < 28 && targetId1 == 0x0E )
	{
		switch( targetId2 )
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

	if( !MineCheck( mSock, mChar, targetX, targetY, targetZ, targetId1, targetId2 ))
	{
		mSock.SysMessage( 801 ); // You cannot mine there.
		return;
	}

	RegenerateOre( targetX, targetY, mChar->WorldNumber() );
	MapResource_st *orePart = MapRegion->GetResource( targetX, targetY, mChar->WorldNumber() );
	if( orePart->oreAmt <= 0 )
	{
		mSock.SysMessage( 802 ); // There is no metal to mine here.
		return;
	}

	// do action and sound
	if( mChar->GetBodyType() == BT_GARGOYLE 
		|| ( cwmWorldState->ServerData()->ForceNewAnimationPacket() && ( mChar->GetSocket() == nullptr || mChar->GetSocket()->ClientType() >= CV_SA2D )))
	{
		Effects->PlayNewCharacterAnimation( mChar, N_ACT_ATT, S_ACT_1H_BASH ); // action 0x00, subAction 0x03
	}
	else if( mChar->IsOnHorse() ) // Human/Elf, mounted, pre-v7.0.0.0
	{
		Effects->PlayCharacterAnimation( mChar, ACT_MOUNT_ATT_1H, 0, 5 ); // 0x1A
	}
	else // Human/Elf, on foot, pre-v7.0.0.0
	{
		Effects->PlayCharacterAnimation( mChar, ACT_ATT_1H_BASH, 0, 7 ); // 0x0B
	}

	Effects->PlaySound( &mSock, 0x0125, true );

	// Consume uses on mining tool, if tool use limits are enabled in ini
	if( cwmWorldState->ServerData()->ToolUseLimit() )
	{
		auto usesLeft = miningTool->GetUsesLeft();
		if( usesLeft > 0 )
		{
			usesLeft -= 1;
			miningTool->SetUsesLeft( usesLeft );

			if( usesLeft == 0 )
			{
				mSock.SysMessage( 10202 ); // You have worn out your tool!

				// If no uses left, break tool if tool breakage is enabled in ini
				if( cwmWorldState->ServerData()->ToolUseBreak() )
				{
					miningTool->Delete();
					// SFX of tool breaking
				}
			}
		}
	}

	if( CheckSkill( mChar, MINING, 0, 1000 )) // check to see if our skill is good enough
	{
		if( orePart->oreAmt > 0 )
		{
			--orePart->oreAmt;
		}

#if defined( UOX_DEBUG_MODE )
		Console << "DBG: Mine(\"" << mChar->GetName() << "\"[" << mChar->GetSerial()
		<< "]); --> MINING: " << mChar->GetSkill( MINING ) << "  RaceID: " << mChar->GetRace() << myendl;
#endif

		CTownRegion *targetReg = CalcRegionFromXY( targetX, targetY, mChar->WorldNumber(), mChar->GetInstanceId() );
		if( targetReg == nullptr )
			return;

		MakeOre( mSock, mChar, targetReg );
	}
	else
	{
		mSock.SysMessage( 803 ); // You sifted through the dirt and rocks, but found nothing usable.
		if( orePart->oreAmt > 0 && RandomNum( 0, 1 ))
		{
			--orePart->oreAmt;
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::GraveDig()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Do GraveDigging Stuff (Should probably be replaced with an OSI-like Treasure
//|					Hunting System based off a Resource System much like Ore and Logs)
//o------------------------------------------------------------------------------------------------o
void CSkills::GraveDig( CSocket *s )
{
	VALIDATESOCKET( s );
	SI16	nFame;
	CItem *	nItemId = nullptr;

	CChar *nCharId = s->CurrcharObj();
	Karma( nCharId, nullptr, -2000 ); // Karma loss no lower than the -2 pier

	// do action and sound
	if( nCharId->GetBodyType() == BT_GARGOYLE 
		|| ( cwmWorldState->ServerData()->ForceNewAnimationPacket() && ( nCharId->GetSocket() == nullptr || nCharId->GetSocket()->ClientType() >= CV_SA2D )))
	{
		Effects->PlayNewCharacterAnimation( nCharId, N_ACT_ATT, S_ACT_1H_BASH ); // Action 0x00, subAction 0x03
	}
	else if( nCharId->IsOnHorse() ) // Human/Elf, mounted
	{
		Effects->PlayCharacterAnimation( nCharId, ACT_MOUNT_ATT_1H, 0, 5 ); // Action 0x1A
	}
	else // Human/Elf, on foot
	{
		Effects->PlayCharacterAnimation( nCharId, ACT_ATT_1H_BASH, 0, 7 ); // Action 0x0B
	}

	Effects->PlaySound( s, 0x0125, true );
	if( !CheckSkill( nCharId, MINING, 0, 800 ))
	{
		s->SysMessage( 805 ); // You sifted through the dirt and found nothing.
		return;
	}

	nFame = nCharId->GetFame();

	// do action and sound (again?)
	if( nCharId->GetBodyType() == BT_GARGOYLE 
		|| ( cwmWorldState->ServerData()->ForceNewAnimationPacket() && ( nCharId->GetSocket() == nullptr || nCharId->GetSocket()->ClientType() >= CV_SA2D )))
	{
		Effects->PlayNewCharacterAnimation( nCharId, N_ACT_ATT, S_ACT_1H_BASH ); // Action 0x00, subAction 0x03
	}
	else if( nCharId->IsOnHorse() ) // Human/Elf, mounted
	{
		Effects->PlayCharacterAnimation( nCharId, ACT_MOUNT_ATT_1H, 0, 5 ); // Action 0x1A
	}
	else // Human/Elf, on foot
	{
		Effects->PlayCharacterAnimation( nCharId, ACT_ATT_1H_BASH, 0, 7 ); // Action 0x0B
	}

	Effects->PlaySound( s, 0x0125, true );
	CChar *spawnCreature = nullptr;
	switch( RandomNum( 0, 12 ))
	{
		case 2:
			spawnCreature = Npcs->CreateRandomNPC( "weakundeadlist" ); // Low level Undead - Random
			s->SysMessage( 806 ); // You have disturbed the rest of a vile undead creature.
			break;
		case 4:
			nItemId = Items->CreateRandomItem( s, "diggingarmor" ); // Armor and shields - Random
			if( nItemId == nullptr )
				break;

			if( nItemId->GetId() >= 7026 && nItemId->GetId() <= 7035 )
			{
				s->SysMessage( 807 ); // You unearthed an old shield and placed it in your pack.
			}
			else
			{
				s->SysMessage( 808 ); // You have found an old piece armour and placed it in your pack.
			}
			break;
		case 5:
			//Random treasure between gems and gold
			if( RandomNum( 0, 1 ))
			{  // randomly create a gem and place in backpack
				Items->CreateRandomItem( s, "digginggems" );
				s->SysMessage( 809 ); // You place a gem in your pack.
			}
			else
			{  // Create between 1 and 15 goldpieces and place directly in backpack
				UI08 nAmount = RandomNum( 1, 15 );
				Items->CreateScriptItem( s, nCharId, "0x0EED", nAmount, OT_ITEM, true );
				Effects->GoldSound( s, nAmount );
				s->SysMessage( 810, nAmount ); // You unearthed %i gold coins.
			}
			break;
		case 6:
			spawnCreature = Npcs->CreateRandomNPC( "weakundeadlist" ); // Low level Undead - Random
			s->SysMessage( 806 ); // You have disturbed the rest of a vile undead creature.
			break;
		case 8:
			Items->CreateRandomItem( s, "diggingweapons" );
			s->SysMessage( 811 ); // You unearthed a old weapon and placed it in your pack.
			break;
		case 10:
		case 12:
			if( nFame < 1000 )
			{
				spawnCreature = Npcs->CreateRandomNPC( "weakundeadlist" ); // Med level Undead - Random
			}
			else
			{
				spawnCreature = Npcs->CreateRandomNPC( "strongundeadlist" ); // High level Undead - Random
			}
			s->SysMessage( 806 ); // You have disturbed the rest of a vile undead creature.
			break;
		default:
			if( RandomNum( 0, 1 ))
			{
				Items->CreateRandomItem( s, "diggingbones" );		// Random Bones
				s->SysMessage( 812 ); // You have unearthed some old bones and placed them in your pack.
			}
			else	// found an empty grave
			{
				s->SysMessage( 813 ); // This grave seems to be empty.
			}
			break;
	}
	if( spawnCreature != nullptr )
	{
		spawnCreature->SetLocation( nCharId );
	}
}

//o------------------------------------------------------------------------------------------------o
//| Function	-	CSkills::SmeltOre();
//| Modified	-	(February 19, 2000)
//o------------------------------------------------------------------------------------------------o
//| Purpose		-	Rewritten to use case and structure, you'll find it is easier to make it
//|					scriptable now. The structure is pretty much all that'd be needed for any
//|					future ore->ingot conversions. Scripting the ore would probably be even
//|					simpler, requires less info
//o------------------------------------------------------------------------------------------------o
void CSkills::SmeltOre( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *chr			= s->CurrcharObj();
	CItem *itemToSmelt	= static_cast<CItem *>( s->TempObj() );
	s->TempObj( nullptr );
	CItem *forge		= CalcItemObjFromSer( s->GetDWord( 7 ));

	if( itemToSmelt->IsHeldOnCursor() )
	{
		s->SysMessage( 400 ); // That is too far away!
		return;
	}

	if( ValidateObject( forge )) // if we have a forge
	{
		CItem *playerPack = chr->GetPackItem();
		if( ValidateObject( playerPack ))
		{
			if( FindRootContainer( itemToSmelt ) != playerPack && FindRootContainer( forge ) != playerPack )
			{
				if( !ObjInRange( chr, forge, DIST_NEARBY ) || !CheckItemRange( chr, itemToSmelt )) // Check if forge & item to melt are in range
				{
					s->SysMessage( 400 ); // That is too far away!
					return;
				}
			}
		}

		UI16 smeltItemId = itemToSmelt->GetId();
		UI16 forgeId = forge->GetId();

		// Is player trying to combine ore?
		//if( forgeId == 0x19B7 || forgeId == 0x19B8 || forgeId == 0x19BA )
		if( forgeId >= 0x19B7 && forgeId <= 0x19BA )
		{
			if( forgeId == smeltItemId )
			{
				s->SysMessage( 1806 ); // You cannot combine these ore types
				return;
			}

			if( forge->GetColour() != itemToSmelt->GetColour() )
			{
				s->SysMessage( 1807 ); // You cannot combine ores of different metals.
				return;
			}

			if( forge->GetMovable() == 2 || forge->GetMovable() == 3 )
			{
				s->SysMessage( 1808 ); // You cannot do that.
				return;
			}

			// Player targeted some small or medium ore! Combine ores, if possible
			SI32 sourceAmount = itemToSmelt->GetAmount();
			SI32 targetAmount = forge->GetAmount();
			UI08 amountMultiplier = 1;

			if( smeltItemId == 0x19B9 && forgeId == 0x19B7 ) // Large ore, targeted small ore
			{
				amountMultiplier = 4;
			}
			else if( smeltItemId == 0x19B9 && ( forgeId == 0x19B8 || forgeId == 0x19BA )) // Large ore, targeted medium ore
			{
				amountMultiplier = 2;
			}
			else if(( smeltItemId == 0x19B8 || smeltItemId == 0x19BA ) && forgeId == 0x19B7 ) // Medium ore, targeted small ore
			{
				amountMultiplier = 2;
			}
			else
			{
				// Small ore, targeted at larger ore
				s->SysMessage( 1809 ); // You can only combine larger ores with smaller ores!
				return;
			}

			// Combine source stack into target stack, if it fits
			if( targetAmount + ( sourceAmount * amountMultiplier ) <= MAX_STACK )
			{
				CChar *forgeOwner = FindItemOwner( forge );

				SI32 newTargetWeight = ( targetAmount + ( sourceAmount * amountMultiplier )) * forge->GetBaseWeight();
				SI32 subtractWeight = ( targetAmount * forge->GetBaseWeight() ) + ( sourceAmount * itemToSmelt->GetBaseWeight() );

				if( ValidateObject( forgeOwner ) && forgeOwner == chr )
				{
					if(( newTargetWeight + chr->GetWeight() - subtractWeight ) > 200000 ) // 2000 stones
					{
						s->SysMessage( 1743 ); // That person can not possibly hold more weight
						return;	
					}
				}
				else if( forge->GetCont() != nullptr )
				{
					if(( newTargetWeight + forge->GetCont()->GetWeight() - subtractWeight ) > 200000 ) // 2000 stones
					{
						s->SysMessage( 1810 ); // The weight is too great to combine in a container.
						return;
					}
				}

				// Good to go, combine ores and remove source stack!
				forge->SetAmount( targetAmount + ( sourceAmount * amountMultiplier ));

				// Remove from multi before deleting
				if( itemToSmelt->IsLockedDown() && ValidateObject( itemToSmelt->GetMultiObj() ))
				{
					itemToSmelt->GetMultiObj()->ReleaseItem( itemToSmelt );
				}

				itemToSmelt->Delete();
				s->SysMessage( 1811 ); // You combine the ore with the stack of smaller ore.
			}
			else if( targetAmount < MAX_STACK )
			{
				// New targetStack would exceed MAX_STACK, but there might still be some room left! Cap amount at MAX_STACK
				UI16 amountLeft = MAX_STACK - targetAmount;
				if( amountLeft < amountMultiplier )
				{
					// Not enough space left in stack to combine any more ore
					s->SysMessage( 1812 ); // That stack cannot hold any more items
					return;
				}
				else
				{
					// There's still some room
					UI16 amountToRemove = static_cast<UI16>( floor( amountLeft / amountMultiplier ));
					itemToSmelt->SetAmount( sourceAmount - amountToRemove );
					forge->SetAmount( MAX_STACK );
					s->SysMessage( 1813 ); // You combine a portion of your ore into the stack of smaller ore.
				}
			}
			else
			{
				// targetStack is already at max cap and cannot hold any more ore
				s->SysMessage( 1812 ); // That stack cannot hold any more items
				return;
			}

			return;
		}

		switch( forgeId )	// Check to ensure it is a forge
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
				if( ObjInRange( chr, forge, DIST_NEARBY )) //Check if the forge is in range
				{
					UI16 targColour		= itemToSmelt->GetColour();
					MiningData_st *oreType	= FindOre( targColour );
					if( oreType == nullptr )
					{
						s->SysMessage( 814 ); // That material is foreign to you.
						return;
					}

					if( chr->GetSkill( MINING ) < oreType->minSkill )
					{
						s->SysMessage( 815 ); // You have no idea what to do with this strange ore.
						return;
					}

					if( !CheckSkill( chr, MINING, oreType->minSkill, 1000 ))	// if we do not have minimum skill to use it
					{
						if( itemToSmelt->GetAmount() > 1 )	// If more than 1 ore, half it
						{
							s->SysMessage( 817 ); // Your hand slips and some of your materials are destroyed.
							itemToSmelt->SetAmount( itemToSmelt->GetAmount() / 2 );
						}
						else
						{
							s->SysMessage( 816 ); // Your hand slips and the last of your materials are destroyed.
							// Remove from multi before deleting
							if( itemToSmelt->IsLockedDown() && ValidateObject( itemToSmelt->GetMultiObj() ))
							{
								itemToSmelt->GetMultiObj()->ReleaseItem( itemToSmelt );
							}

							itemToSmelt->Delete();
						}
						return;
					}

					UI16 ingotNum = 1;
					switch( smeltItemId )
					{
						case 0x19B7: // Small ore
							ingotNum = itemToSmelt->GetAmount() / 2; // 0.5 ingot per ore
							break;
						case 0x19B8: // Medium ore
						case 0x19BA: // Medium ore
							ingotNum = itemToSmelt->GetAmount(); // 1.0 ingot per ore
							break;
						case 0x19B9: // Large ore
							ingotNum = itemToSmelt->GetAmount() * 2; // 2.0 ingot per ore
							break;
					}

					CItem *ingot = Items->CreateScriptItem( s, chr, "0x1BF2", ingotNum, OT_ITEM, true, oreType->colour );
					if( ingot != nullptr )
					{
						ingot->SetName( oldstrutil::format( "%s Ingot", oreType->name.c_str() ));
					}

					if( smeltItemId == 0x19B7 && itemToSmelt->GetAmount() % 2 != 0 )
					{
						itemToSmelt->SetAmount( 1 ); // There's still some ore left, as the pile of small ore is not divisible by two
					}
					else
					{
						// Remove from multi before deleting
						if( itemToSmelt->IsLockedDown() && ValidateObject( itemToSmelt->GetMultiObj() ))
						{
							itemToSmelt->GetMultiObj()->ReleaseItem( itemToSmelt );
						}

						itemToSmelt->Delete();	// delete the ore
					}

					s->SysMessage( 818 ); // You have smelted your ore.
					s->SysMessage( 819, oreType->name.c_str() ); // You place some %s ingots in your pack.
				}
				break;
			default:
				s->SysMessage( 820 ); // You can't smelt here.
				break;
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::CalculatePetControlChance()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate a player's chance (returned as value between 0 to 1000) of controlling 
//|					(having commands accepted) a follower
//o------------------------------------------------------------------------------------------------o
UI16 CSkills::CalculatePetControlChance( CChar *mChar, CChar *Npc )
{
	SI16 petOrneriness = static_cast<SI16>( Npc->GetOrneriness() );
	
	// If difficulty to control is below a certain treshold (29.1) or it's a summoned creature, 100% chance to control creature
	if(( petOrneriness == 0 && Npc->GetTaming() <= 291 ) || petOrneriness <= 291 || Npc->IsDispellable() )
		return static_cast<SI16>( 1000 );

	// TODO: Check if NPC can be tamed via necromancer Dark Wolf Familiar
	//if( petOrneriness > 249 && CheckFamiliarMastery( mChar, Npc ))
		//toTame = 249;

	// Fetch player's skill values (with modifiers)
	SI16 tamingSkill	= static_cast<SI16>( mChar->GetSkill( TAMING ));
	SI16 loreSkill		= static_cast<SI16>( mChar->GetSkill( ANIMALLORE ));

	SI16 bonusChance	= 0;
	SI16 totalChance	= 700; // base chance of success
	SI16 tamingSkillMod	= 6;
	SI16 animalLoreMod	= 6;

	// Base bonuses based on skill vs orneriness
	SI16 tamingSkillBonus = static_cast<SI16>( tamingSkill - petOrneriness );
	SI16 animalLoreBonus = static_cast<SI16>( loreSkill - petOrneriness );

	// Calculated bonus modifiers
	if( tamingSkillBonus < 0 )
	{
		tamingSkillMod = 28;
	}
	if( animalLoreBonus < 0 )
	{
		animalLoreMod= 14;
	}

	tamingSkillBonus *= tamingSkillMod;
	animalLoreBonus *= animalLoreMod;

	// Bonus chance equals average of bonuses for tamingskill and animallore
	bonusChance = static_cast<SI16>(( tamingSkillBonus + animalLoreBonus ) / 2 );
	totalChance += bonusChance;

	// Modify chance based on NPCs loyalty (or lack thereof)
	totalChance -= ( Npc->GetMaxLoyalty() - Npc->GetLoyalty() ) * 10;

	// Finally, apply a 15% penalty to chance for friends of the pet trying to control
	if( Npcs->CheckPetFriend( mChar, Npc ))
	{
		totalChance -= ( totalChance * 0.15 );
	}

	// Clamp lower chance to 20% and upper chance to 99%
	totalChance = std::clamp( totalChance, static_cast<SI16>( 200 ), static_cast<SI16>( 990 ));

	return static_cast<UI16>( totalChance );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::CheckSkill()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Used to check a players skill based on the highskill and lowskill it was called
//|					with.  If skill is < than lowskill check will fail, but player will gain in the
//|					skill, if the players skill is > than highskill player will not gain
//o------------------------------------------------------------------------------------------------o
bool CSkills::CheckSkill( CChar *s, UI08 sk, SI16 lowSkill, SI16 highSkill, bool isCraftSkill )
{
	bool skillCheck		= false;
	bool exists			= false;
	std::vector<UI16> scriptTriggers = s->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// If script returns true/1, allows skillcheck to proceed, but also prevents other scripts with event from running
			if( toExecute->OnSkillCheck( s, sk, lowSkill, highSkill, isCraftSkill ) == 1 )
			{
				exists = true;
				break;
			}
		}
	}

	// o----------------------------------------------------------------------------o
	// | 15 March, 2002																|
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
		R32 chanceSkillSuccess = 0;

		if(( highSkill - lowSkill ) <= 0 || !ValidateObject( s ) || s->GetSkill( sk ) < lowSkill )
			return false;

		if( s->IsDead() )
		{
			CSocket *sSock = s->GetSocket();
			sSock->SysMessage( 1487 ); // You are dead and cannot gain skill.
			return false;
		}

		if( s->GetSkill( sk ) >= highSkill )
			return true;

		// Calculate base chance of success at using a skill
		chanceSkillSuccess = ( static_cast<R32>( s->GetSkill( sk )) - static_cast<R32>( lowSkill ));

		// Modify chance based on the range of highSkill vs lowSkill
		chanceSkillSuccess /= ( static_cast<R32>( highSkill ) - static_cast<R32>( lowSkill ));

		// Let's work with whole numbers again
		chanceSkillSuccess *= 1000;

		if( isCraftSkill )
		{
			// Give players at least 50% chance at success if this is a crafting skill and their skill is above minimum requirement
			chanceSkillSuccess = std::max( static_cast<R32>( 500 ), chanceSkillSuccess );
		}

		// Cap chance of success at 1000 (100.0%)
		chanceSkillSuccess = std::min( static_cast<R32>( 1000 ), chanceSkillSuccess );

		if( cwmWorldState->ServerData()->StatsAffectSkillChecks() )
		{
			// Modify base chance of success with bonuses from stats, if this feature is enabled in ini
			chanceSkillSuccess += static_cast<R32>( s->GetStrength() * cwmWorldState->skill[sk].strength ) / 1000.0f;
			chanceSkillSuccess += static_cast<R32>( s->GetDexterity() * cwmWorldState->skill[sk].dexterity ) / 1000.0f;
			chanceSkillSuccess += static_cast<R32>( s->GetIntelligence() * cwmWorldState->skill[sk].intelligence ) / 1000.0f;
		}

		// If player's command-level is equal to Counselor or higher, pass the skill-check automatically
		// Same if chance of success is 100% already - no need to proceed!
		if( s->GetCommandLevel() > 0 || chanceSkillSuccess == 1000 )
		{
			skillCheck = true;
		}
		else
		{
			// Generate a random number between 0 and highSkill (if less than 1000) or 1000 (if higher than 1000)
			SI16 rnd = RandomNum( 0, std::min( 1000, ( highSkill + 100 )));

			// Compare to chanceOfSkillSuccess to see if player succeeds!
			skillCheck = ( static_cast<SI16>( chanceSkillSuccess ) >= rnd );
		}

		CSocket *mSock = s->GetSocket();
		if( mSock != nullptr )
		{
			bool mageryUp = true;
			mageryUp = ( mSock->CurrentSpellType() == 0 );

			if( s->GetBaseSkill( sk ) < highSkill )
			{
				if( sk != MAGERY || mageryUp )
				{
					// Advance player's skill based on result of skillCheck
					if( AdvanceSkill( s, sk, skillCheck ))
					{
						UpdateSkillLevel( s, sk );
						mSock->UpdateSkill( sk );
					}
				}
			}
		}
		else if(( s->IsTamed() && cwmWorldState->ServerData()->PetCombatTraining() ) ||
				( !s->IsTamed() && s->CanBeHired() && cwmWorldState->ServerData()->HirelingCombatTraining() ) ||
				( !s->IsTamed() && !s->CanBeHired() && s->IsNpc() && cwmWorldState->ServerData()->NpcCombatTraining() ))
		{
			if( s->GetBaseSkill( sk ) < highSkill )
			{
				// Advance pet/NPC's skill based on result of skillCheck
				if( AdvanceSkill( s, sk, skillCheck ))
				{
					UpdateSkillLevel( s, sk );
				}
			}
		}
	}
	else
	{
		skillCheck = true;
	}
	return skillCheck;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::HandleSkillChange()
//|	Date		-	Jan 29, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Do atrophy for player c:
//|					find sk in our cronological list of atrophy skills, move it to the front, check
//|					total aginst skillCap to see if we need to lower a skill, if we do, again search
//|					skills for a skill that can be lowered, if one is found lower it and increase
//|					sk, if we can't find one, do nothing if atrophy is not need, increase sk.
//o------------------------------------------------------------------------------------------------o
void CSkills::HandleSkillChange( CChar *c, UI08 sk, SI08 skillAdvance, bool success )
{
	UI32 totalSkill			= 0;
	UI08 rem				= 0;
	UI08 atrop[ALLSKILLS+1];
	UI08 toDec				= 0xFF;
	UI08 counter			= 0;
	CSocket *mSock			= c->GetSocket();

	std::vector<UI16> scriptTriggers = c->GetScriptTriggers();

	UI08 amtToGain			= 1;
	if( success )
	{
		amtToGain			= cwmWorldState->skill[sk].advancement[skillAdvance].amtToGain;
	}
	UI16 skillCap			= cwmWorldState->ServerData()->ServerSkillTotalCapStatus();

	if( c->IsNpc() )
	{
		// Check for existence of onSkillGain event for NPC
		for( auto scriptTrig : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				// If retVal is -1, event doesn't exist in script
				// If retVal is 0, event exists, but returned false/0, and handles item usage. Don't proceed with hard code (or other scripts!)
				// If retVal is 1, event exists, proceed with hard code/other scripts
				if( !toExecute->OnSkillGain( c, sk, amtToGain ))
				{
					return;
				}
			}
		}

		// Increase base skill of NPC
		c->SetBaseSkill( c->GetBaseSkill( sk ) + amtToGain, sk );

		// Check for existence of onSkillChange event for NPC
		for( auto scriptTrig : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				toExecute->OnSkillChange( c, sk, amtToGain );
			}
		}
		return;
	}

	if( mSock == nullptr )
		return;

	//srand( GetClock() ); // Randomize

	atrop[ALLSKILLS] = 0; // set the last of out copy array
	for( counter = 0; counter < ALLSKILLS; ++counter )
	{
		atrop[counter] = c->GetAtrophy( counter );
	}

	for( counter = ALLSKILLS; counter > 0; --counter )
	{
		// add up skills and find the one being increased
		UI08 atrSkill = c->GetAtrophy( static_cast<UI08>( counter - 1 ));

		if( c->GetBaseSkill( atrSkill ) >= amtToGain && c->GetSkillLock( atrSkill ) == SKILL_DECREASE && atrSkill != sk )
		{
			toDec = atrSkill; // we found a skill that can be decreased, save it for later.
		}

		totalSkill += c->GetBaseSkill( static_cast<UI08>( counter - 1 ));

		atrop[counter] = atrop[counter-1];

		if( atrop[counter] == sk )
		{
			rem = counter; // remember this number
		}
	}

	atrop[0] = sk; // set the first one to our current skill

	//copy it back in
	for( counter = 0; counter < rem; ++counter )
	{
		c->SetAtrophy( atrop[counter], counter );
	}
	if( rem != ALLSKILLS ) // in the middle somewhere or first
	{
		for( counter = static_cast<UI08>( rem + 1 ); counter <= ALLSKILLS; ++counter )
		{
			c->SetAtrophy( atrop[counter], ( counter -1 ));
		}
	}

	if( RandomNum( static_cast<UI16>( 0 ), skillCap ) <= static_cast<UI16>( totalSkill ))
	{
		if( toDec != 0xFF )
		{
			// Check for existence of onSkillLoss event for player
			for( auto scriptTrig : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					// If retVal is -1, event doesn't exist in script
					// If retVal is 0, event exists, but returned false/0, and handles item usage. Don't proceed with hard code (or other scripts!)
					// If retVal is 1, event exists, proceed with hard code/other scripts
					if( !toExecute->OnSkillLoss( c, toDec, amtToGain ))
					{
						return;
					}
				}
			}

			// Reduce base skill of player
			totalSkill -= amtToGain;
			c->SetBaseSkill( c->GetBaseSkill( toDec ) - amtToGain, toDec );

			// Check for existence of onSkillChange event for player
			for( auto scriptTrig : scriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( scriptTrig );
				if( toExecute != nullptr )
				{
					toExecute->OnSkillChange( c, toDec, ( amtToGain * -1 ));
				}
			}
			mSock->UpdateSkill( toDec );
		}
	}

	if( skillCap > static_cast<UI16>( totalSkill ))
	{
		// Check for existence of onSkillGain event for player
		for( auto scriptTrig : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				// If retVal is -1, event doesn't exist in script
				// If retVal is 0, event exists, but returned false/0, and handles item usage. Don't proceed with hard code (or other scripts!)
				// If retVal is 1, event exists, proceed with hard code/other scripts
				if( !toExecute->OnSkillGain( c, sk, amtToGain ))
				{
					return;
				}
			}
		}

		// Increase base skill of player
		c->SetBaseSkill( c->GetBaseSkill( sk ) + amtToGain, sk );

		// Check for existence of onSkillChange event for player
		for( auto scriptTrig : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				toExecute->OnSkillChange( c, sk, amtToGain );
			}
		}
		mSock->UpdateSkill( sk );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::SkillUse()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when player uses a skill from the skill list
//o------------------------------------------------------------------------------------------------o
//void ClilocMessage( CSocket *mSock, SpeechType speechType, UI16 hue, UI16 font, UI32 messageNum, const char *types = "", ... );
void CSkills::SkillUse( CSocket *s, UI08 x )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	if( mChar->IsDead() )
	{
		//ClilocMessage( s, SYSTEM, cwmWorldState->ServerData()->SysMsgColour(), FNT_NORMAL, 500012 );
		s->SysMessage( 9054 ); // You cannot use skills while dead.
		return;
	}
	if(( x != STEALTH && mChar->GetVisible() == VT_TEMPHIDDEN ) || mChar->GetVisible() == VT_INVISIBLE )
	{
		mChar->ExposeToView();
	}
	mChar->BreakConcentration( s );
	if( mChar->GetTimer( tCHAR_SPELLTIME ) != 0 || mChar->IsCasting() )
	{
		s->SysMessage( 854 ); // You can't do that while you are casting!
		return;
	}
	if( s->GetTimer( tPC_SKILLDELAY ) <= cwmWorldState->GetUICurrentTime() || mChar->IsGM() )
	{
		s->SkillDelayMsgShown( false );
		bool doSwitch = true;
		std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
		for( auto scriptTrig : scriptTriggers )
		{
			// Loop through attached scripts
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				if( toExecute->OnSkill( mChar, x ))
				{
					// Look for onSkill JS event in script
					doSwitch = false;
				}
			}
		}

		// If no onSkill event was found in a script already, check if skill has a script
		// attached directly, with onSkill event
		if( doSwitch && cwmWorldState->skill[x].jsScript != 0xFFFF )
		{
			cScript *toExecute = JSMapping->GetScript( cwmWorldState->skill[x].jsScript );
			if( toExecute != nullptr )
			{
				if( toExecute->OnSkill( mChar, x ))
				{
					doSwitch = false;
				}
			}
		}

		// If no onSkill event has run yet, run the hardcoded version
		if( doSwitch )
		{
			switch( x )
			{
				case STEALING:
					// Check if stealing is allowed in player's current region
					if( mChar->GetRegion()->IsSafeZone() )
					{
						// Player is in a safe zone where all aggressive actions are forbidden, disallow
						s->SysMessage( 1799 ); // Hostile actions are not permitted in this safe area.
					}
					else if( cwmWorldState->ServerData()->RogueStatus() )
					{
						s->SendTargetCursor( 0, TARGET_STEALING, 1, 863 ); // What do you wish to steal?
					}
					else
					{
						s->SysMessage( 864 ); // Contact your shard operator if you want stealing available.
					}
					break;
				case TRACKING:			TrackingMenu( s, TRACKINGMENUOFFSET );	break;
				default:				s->SysMessage( 871 );					break; // That skill has not been implemented yet.
			}
		}

		// If skillDelay timer is still below currenttimer, it wasn't modified in onSkill event. So let's update it now!
		if( s->GetTimer( tPC_SKILLDELAY ) <= cwmWorldState->GetUICurrentTime() )
		{
			if( cwmWorldState->skill[x].skillDelay != -1 )
			{
				// Use skill-specific skill delay if one has been set
				s->SetTimer( tPC_SKILLDELAY, BuildTimeValue( static_cast<R32>( cwmWorldState->skill[x].skillDelay )));
			}
			else
			{
				// Otherwise use global skill delay from uox.ini
				s->SetTimer( tPC_SKILLDELAY, BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->ServerSkillDelayStatus() )));
			}
		}
		return;
	}
	else
	{
		if( !s->SkillDelayMsgShown() )
		{
			s->SysMessage( 872 ); // You must wait a few moments before using another skill.
			s->SkillDelayMsgShown( true );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::RandomSteal()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when player targets a PC/NPC with stealing skill
//|					instead of an item (randomly pics an item in their pack)
//o------------------------------------------------------------------------------------------------o
void CSkills::RandomSteal( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();
	CChar *npc = CalcCharObjFromSer( s->GetDWord( 7 ));
	if( !ValidateObject( npc ))
		return;

	// Check if stealing is allowed in target char's region
	if( npc->GetRegion()->IsSafeZone() )
	{
		// Target is in a safe zone where all aggressive actions are forbidden, disallow
		s->SysMessage( 1799 ); // Hostile actions are not permitted in this safe area.
		return;
	}

	CItem *p = npc->GetPackItem();
	if( !ValidateObject( p ))
	{
		s->SysMessage( 875 ); // Bad luck, your victim doesn't have a backpack.
		return;
	}

	CItem *item = nullptr;
	GenericList<CItem *> *tcCont = p->GetContainsList();
	const size_t numItems = tcCont->Num();

	std::vector<CItem *> contList;
	contList.reserve( numItems );
	if( numItems > 0 )
	{
		for( item = tcCont->First(); !tcCont->Finished(); item = tcCont->Next() )
		{
			if( ValidateObject( item ))
			{
				contList.push_back( item );
			}
		}
		item = contList[RandomNum( static_cast<size_t>( 0 ), ( contList.size() - 1 ))];
	}
	contList.clear();

	if( !ValidateObject( item ))
	{
		s->SysMessage( 876 ); // Muahaha, your victim doesn't have possessions.
		return;
	}
	DoStealing( s, mChar, npc, item );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::StealingTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when player targets an item with stealing skill
//o------------------------------------------------------------------------------------------------o
void CSkills::StealingTarget( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();

	if( s->GetByte( 7 ) < 0x40 )
	{
		RandomSteal( s );
		return;
	}
	CItem *item = CalcItemObjFromSer( s->GetDWord( 7 ));
	if( !ValidateObject( item ))
		return;

	CChar *npc = FindItemOwner( item );
	if( !ValidateObject( npc ))
		return;

	// Check if stealing is allowed in target char's region
	if( npc->GetRegion()->IsSafeZone() )
	{
		// Target is in a safe zone where all aggressive actions are forbidden, disallow
		s->SysMessage( 1799 ); // Hostile actions are not permitted in this safe area.
		return;
	}

	if( item->GetCont() == npc || item->GetCont() == nullptr || item->IsNewbie() )
	{
		s->SysMessage( 874 ); // You cannot steal that.
		return;
	}
	DoStealing( s, mChar, npc, item );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::DoStealing()
//|	Date		-	2/13/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Do the bulk of stealing stuff rather than having the same
//|					code twice in RandomSteal() and StealingTarget()
//o------------------------------------------------------------------------------------------------o
void CSkills::DoStealing( CSocket *s, CChar *mChar, CChar *npc, CItem *item )
{
	VALIDATESOCKET( s );
	if( npc == mChar )
	{
		s->SysMessage( 873 ); // You catch yourself red handed.
		return;
	}
	if( npc->GetNpcAiType() == AI_PLAYERVENDOR )
	{
		s->SysMessage( 874 ); // You cannot steal that.
		return;
	}
	CItem *itemCont = static_cast<CItem *>( item->GetCont() );
	if( itemCont != nullptr && itemCont->GetLayer() >= IL_SELLCONTAINER && itemCont->GetLayer() <= IL_BUYCONTAINER ) // is it in the sell or buy layer of a vendor?
	{
		s->SysMessage( 874 ); // You cannot steal that.
		return;
	}

	std::vector<UI16> scriptTriggers = item->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		// Loop through attached scripts
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			SI08 retVal = toExecute->OnSteal( mChar, item, npc );
			if( retVal == 1 ) // Item was not stolen, and we don't want to run hard code
			{
				return;
			}
			else if( retVal == 2 ) // item was stolen, and we don't want to run hard code
			{
				std::vector<UI16> targetScriptTriggers = npc->GetScriptTriggers();
				for( auto targScriptTrig : targetScriptTriggers )
				{
					cScript *targToExecute = JSMapping->GetScript( targScriptTrig );
					if( targToExecute != nullptr )
					{
						if( targToExecute->OnStolenFrom( mChar, npc, item ) == 1 )
						{
							break;
						}
					}
				}
				return;
			}
		}
	}

	std::string npcTargetName = GetNpcDictName( npc );
	std::string myNpcTargetName = GetNpcDictName( npc, s );

	s->SysMessage( 877, myNpcTargetName.c_str() ); // You reach into %s's pack and try to take something...
	if( ObjInRange( mChar, npc, DIST_NEXTTILE ))
	{
		if( Combat->CalcDef( mChar, 0, false ) > 40 )
		{
			s->SysMessage( 1643 ); // Your armour is too heavy to do that
			return;
		}
		SI16 stealCheck = CalcStealDiff( mChar, item );
		if( stealCheck == -1 )
		{
			s->SysMessage( 1551 ); // That is too heavy
			return;
		}

		if( mChar->GetCommandLevel() < npc->GetCommandLevel() )
		{
			s->SysMessage( 878 ); // You can't steal from gods.
			return;
		}
		if( item->IsNewbie() )
		{
			s->SysMessage( 879 ); // That item has no value to you.
			return;
		}

		const SI32 getDefOffset	= std::min( stealCheck + ( static_cast<SI32>(( Combat->CalcDef( mChar, 0, false ) - 1 ) / 10 ) * 100 ), 990 );
		const bool canSteal		= CheckSkill( mChar, STEALING, getDefOffset, 1000);
		if( canSteal )
		{
			CItem *pack = mChar->GetPackItem();
			item->SetCont( pack );
			s->SysMessage( 880 ); // You successfully steal that item.

			std::vector<UI16> targetScriptTriggers = npc->GetScriptTriggers();
			for( auto targScriptTrig : targetScriptTriggers )
			{
				cScript *toExecute = JSMapping->GetScript( targScriptTrig );
				if( toExecute != nullptr )
				{
					if( toExecute->OnStolenFrom( mChar, npc, item ) == 1 )
					{
						return;
					}
				}
			}
		}
		else
		{
			s->SysMessage( 881 ); // You failed to steal that item.
		}

		if(( !canSteal && RandomNum( 1, 5 ) == 5 ) || mChar->GetSkill( STEALING ) < RandomNum( 0, 1001 ))
		{
			//Did they get caught? (If they fail 1 in 5 chance, other wise their skill away from 1000 out of 1000 chance)
			s->SysMessage( 882 ); // You have been caught!
			if( npc->IsNpc() )
			{
				npc->TextMessage( nullptr, 883, TALK, false ); // Guards!! A thief is among us!
			}

			if( WillResultInCriminal( mChar, npc ))
			{
				MakeCriminal( mChar );
			}
			std::string temp;
			std::string temp2;

			if( item->GetName()[0] != '#' )
			{
				temp = oldstrutil::format(512, Dictionary->GetEntry( 884 ), mChar->GetNameRequest( npc ).c_str(), item->GetNameRequest( npc ).c_str() ); // // You notice %s trying to steal %s from you!
				temp2 = Dictionary->GetEntry( 885 ); // You notice %s trying to steal %s from %s!
			}
			else
			{
				std::string tileName;
				tileName.reserve( MAX_NAME );
				GetTileName(( *item ), tileName );
				temp = oldstrutil::format( 512, Dictionary->GetEntry( 884 ), mChar->GetNameRequest( npc ).c_str(), tileName.c_str() ); // You notice %s trying to steal %s from you!
				temp2 = Dictionary->GetEntry( 885 ); // You notice %s trying to steal %s from %s!
			}

			CSocket *npcSock = npc->GetSocket();
			if( npcSock != nullptr )
			{
				npcSock->SysMessage( temp );
			}

			for( auto &iSock : FindNearbyPlayers( mChar ))
			{
				CChar *iChar = iSock->CurrcharObj();
				if(( iSock != s ) && (( RandomNum( 10, 20 ) == 17 ) || (( RandomNum( 0, 1 ) == 1 ) && ( iChar->GetIntelligence() >= mChar->GetIntelligence() ))))
				{
					temp2 = oldstrutil::format( 512, temp2, mChar->GetNameRequest( iChar ).c_str(), item->GetNameRequest( iChar ).c_str(), npcTargetName.c_str() ); // You notice %s trying to steal %s from %s!
					iSock->SysMessage( temp2 );
				}
			}
		}
	}
	else
	{
		s->SysMessage( 886 ); // You are too far away to steal that item.
	}
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CalcStealDiff()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check if item is able to be stolen based on item type and weight vs stealing
//|					skill, then return the min skill needed to steal the item.
//o------------------------------------------------------------------------------------------------o
SI16 CSkills::CalcStealDiff( CChar *c, CItem *i )
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
			if( i->GetId() == 0x14F0 ) // Deeds
				break;

			itemWeight = i->GetWeight();
			calcDiff = static_cast<SI32>(( c->GetSkill( STEALING ) * 2 ) + 100 );  // GM thieves can steal up to 21 stones, newbie only 1 stone
			if( calcDiff > itemWeight )
			{
				stealDiff = static_cast<SI16>( std::max( std::min(( static_cast<SI32>(( itemWeight + 9 ) / 20 ) * 10 ), 990 ), 0 ));
			}
			break;
	}
	return stealDiff;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::Tracking()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Start tracking selected NPC/PC
//o------------------------------------------------------------------------------------------------o
void CSkills::Tracking( CSocket *s, SI32 selection )
{
	VALIDATESOCKET( s );
	CChar *i = s->CurrcharObj();

	// sets trackingtarget that was selected in the gump
	i->SetTrackingTarget( i->GetTrackingTargets( selection ));

	// tracking time in seconds ... gm tracker -> basetimer + 1 seconds, 0 tracking -> 1 sec, new calc
	s->SetTimer( tPC_TRACKING, BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->TrackingBaseTimer() * i->GetSkill( TRACKING ) / 1000 + 1 )));
	s->SetTimer( tPC_TRACKINGDISPLAY, BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->TrackingRedisplayTime() )));
	if( ValidateObject( i->GetTrackingTarget() ))
	{
		std::string trackingTargetName = GetNpcDictName( i->GetTrackingTarget() );
		s->SysMessage( 1644, trackingTargetName.c_str() ); // You are now tracking %s.
	}
	Track( i );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::CreateTrackingMenu()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Brings up Tracking Menu, Called when player uses Tracking Skill
//o------------------------------------------------------------------------------------------------o
void CSkills::CreateTrackingMenu( CSocket *s, UI16 m )
{
	VALIDATESOCKET( s );
	const std::string sect = std::string( "TRACKINGMENU " ) + oldstrutil::number( m );
	CScriptSection *TrackStuff = FileLookup->FindEntry( sect, menus_def );
	if( TrackStuff == nullptr )
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
	SI32 type				= 887; // You see no signs of any animals.
	UI08 MaxTrackingTargets = 0;
	const UI16 distance		= ( cwmWorldState->ServerData()->TrackingBaseRange() + ( mChar->GetSkill( TRACKING ) / 50 ));
	UnicodeTypes mLang		= s->Language();

	if( m == ( 2 + TRACKINGMENUOFFSET ))
	{
		creatureType	= CT_CREATURE;
		type			= 888; // You see no signs of any creatures.
	}
	else if( m == ( 3 + TRACKINGMENUOFFSET ))
	{
		creatureType	= CT_PERSON;
		type			= 889; // You see no signs of anyone.
	}

	std::string line;
	CPOpenGump toSend( *mChar );
	if( m >= TRACKINGMENUOFFSET )
	{
		toSend.GumpIndex( m );
	}
	else
	{
		toSend.GumpIndex( m + TRACKINGMENUOFFSET );
	}
	std::string tag		= TrackStuff->First();
	std::string data	= TrackStuff->GrabData();

	line = tag + " " + data;
	toSend.Question( line );

	for( auto &MapArea : MapRegion->PopulateList( mChar ))
	{
		if( MapArea )
		{
			auto regChars = MapArea->GetCharList();
			for( const auto &tempChar : regChars->collection() )
			{
				if( MaxTrackingTargets < cwmWorldState->ServerData()->TrackingMaxTargets() )
				{
					if( ValidateObject( tempChar ) && ( tempChar->GetInstanceId() == mChar->GetInstanceId() ))
					{
						id = tempChar->GetId();
						if(( !cwmWorldState->creatures[id].IsHuman() || creatureType == CT_PERSON ) && ( !cwmWorldState->creatures[id].IsAnimal() || creatureType == CT_ANIMAL ))
						{
							const bool cmdLevelCheck = ( IsOnline(( *tempChar )) && ( mChar->GetCommandLevel() >= tempChar->GetCommandLevel() ));
							if( tempChar != mChar && ObjInRange( tempChar, mChar, distance ) && !tempChar->IsDead() && ( cmdLevelCheck || tempChar->IsNpc() ))
							{
								mChar->SetTrackingTargets( tempChar, MaxTrackingTargets );
								++MaxTrackingTargets;
								
								SI32 dirMessage = 898; // right next to you.
								switch( Movement->Direction( mChar, tempChar->GetX(), tempChar->GetY() ))
								{
									case NORTH:		dirMessage = 890;	break; // to the North.
									case NORTHWEST:	dirMessage = 891;	break; // to the Northwest.
									case NORTHEAST:	dirMessage = 892;	break; // to the Northeast.
									case SOUTH:		dirMessage = 893;	break; // to the South.
									case SOUTHWEST:	dirMessage = 894;	break; // to the Southwest.
									case SOUTHEAST:	dirMessage = 895;	break; // to the Southeast.
									case WEST:		dirMessage = 896;	break; // to the West.
									case EAST:		dirMessage = 897;	break; // to the East.
									default:		dirMessage = 898;	break; // right next to you.
								}
								auto tempName = GetNpcDictName( tempChar );
								line = tempName + " "s + Dictionary->GetEntry( dirMessage, mLang );
								toSend.AddResponse( cwmWorldState->creatures[id].Icon(), 0, line );
							}
						}
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	if( MaxTrackingTargets == 0 )
	{
		s->SysMessage( type );
		return;
	}
	toSend.Finalize();
	s->Send( &toSend );
}

void HandleCommonGump( CSocket *mSock, CScriptSection *gumpScript, UI16 gumpIndex );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::TrackingMenu()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Brings up additional tracking menu with options listed in tracking.dfn
//o------------------------------------------------------------------------------------------------o
void CSkills::TrackingMenu( CSocket *s, UI16 gmindex )
{
	VALIDATESOCKET( s );
	const std::string sect		= std::string( "TRACKINGMENU " ) + oldstrutil::number( gmindex );
	CScriptSection *TrackStuff	= FileLookup->FindEntry( sect, menus_def );
	if( TrackStuff == nullptr )
	{
		return;
	}
	HandleCommonGump( s, TrackStuff, gmindex );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::Track()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Tracking stuff for older client
//o------------------------------------------------------------------------------------------------o
void CSkills::Track( CChar *i )
{
	CSocket *s = i->GetSocket();
	VALIDATESOCKET( s );
	CChar *trackTarg = i->GetTrackingTarget();
	if( !ValidateObject( trackTarg ) || trackTarg->IsDeleted() || trackTarg->GetY() == -1 )
	{
		s->SysMessage( 2059 ); // You have lost your quarry.
		s->SetTimer( tPC_TRACKING, 0 );
		CPTrackingArrow tSend;
		tSend.Active( 0 );
		if( s->ClientVersion() >= CV_HS2D )
		{
			tSend.AddSerial( i->GetTrackingTargetSerial() );
		}
		s->Send( &tSend );
		return;
	}
	CPTrackingArrow tSend = ( *trackTarg );
	tSend.Active( 1 );
	if( s->ClientType() >= CV_HS2D )
	{
		tSend.AddSerial( i->GetTrackingTarget()->GetSerial() );
	}
	s->Send( &tSend );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::UpdateSkillLevel()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate the skill of this character based on the characters baseskill and stats
//o------------------------------------------------------------------------------------------------o
void CSkills::UpdateSkillLevel( CChar *c, UI08 s ) const
{
	UI16 sStr = cwmWorldState->skill[s].strength;
	SI16 aStr = c->ActualStrength();
	UI16 sDex = cwmWorldState->skill[s].dexterity;
	SI16 aDex = c->ActualDexterity();
	UI16 sInt = cwmWorldState->skill[s].intelligence;
	SI16 aInt = c->ActualIntelligence();
	UI16 bSkill = c->GetBaseSkill( s );

	UI16 temp = ((( sStr * aStr ) / 100 + ( sDex * aDex ) / 100 + ( sInt + aInt ) / 100) * ( 1000 - bSkill )) / 1000 + bSkill;
	c->SetSkill( std::max( bSkill, temp ), s );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::Persecute()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when a Ghost attacks a Player.  If entry in UOX.INI
//|					is enabled, players mana decreases each time you try to
//|					persecute him
//o------------------------------------------------------------------------------------------------o
void CSkills::Persecute( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *c		= s->CurrcharObj();
	CChar *targChar	= c->GetTarg();
	if( !ValidateObject( targChar ) || targChar->IsGM() )
		return;

	SI32 decrease = static_cast<SI32>(( c->GetSkill( SPIRITSPEAK ) / 100 ) + ( c->GetIntelligence() / 20 )) + 3;

	if( s->GetTimer( tPC_SKILLDELAY ) <= cwmWorldState->GetUICurrentTime() || c->IsGM() )
	{
		if(( RandomNum( 0, 19 ) + c->GetIntelligence() ) > 45 ) // not always
		{
			CSocket *tSock = targChar->GetSocket();
			targChar->SetMana( targChar->GetMana() - decrease ); // decrease mana
			s->SysMessage( 972 ); // Your spiritual forces disturb the enemy!
			if( tSock != nullptr )
			{
				tSock->SysMessage( 973 ); // A damned soul is disturbing your mind!
			}

			if( cwmWorldState->skill[SPIRITSPEAK].skillDelay != -1 )
			{
				// Use skill-specific skill delay if one has been set
				s->SetTimer( tPC_SKILLDELAY, BuildTimeValue( static_cast<R32>( cwmWorldState->skill[SPIRITSPEAK].skillDelay )));
			}
			else
			{
				// Otherwise use global skill delay from uox.ini
				s->SetTimer( tPC_SKILLDELAY, BuildTimeValue( static_cast<R32>( cwmWorldState->ServerData()->ServerSkillDelayStatus() )));
			}

			std::string targCharName = GetNpcDictName( targChar );
			targChar->TextMessage( nullptr, 974, EMOTE, 1, targCharName.c_str() ); // %s is persecuted by a ghost!
		}
		else
		{
			s->SysMessage( 975 ); // Your mind is not strong enough to disturb the enemy
		}
	}
	else
	{
		s->SysMessage( 976 ); // You are unable to persecute him now. Rest a little.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::Smith()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when player attempts to smith an item
//o------------------------------------------------------------------------------------------------o
void CSkills::Smith( CSocket *s )
{
	VALIDATESOCKET( s );
	CChar *mChar	= s->CurrcharObj();
	CItem *packnum	= mChar->GetPackItem();

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

	if( !ValidateObject( packnum ))
	{
		s->SysMessage( 773 ); // Time to buy a backpack.
		return;
	}

	CItem *i = CalcItemObjFromSer( s->GetDWord( 7 ));
	if( ValidateObject( i ))
	{
		if( i->IsHeldOnCursor() || !CheckItemRange( mChar, i ))
		{
			s->SysMessage( 400 ); // That is too far away!
			return;
		}

		if( i->GetId() >= 0x1BE3 && i->GetId() <= 0x1BF9 )	// is it an ingot?
		{
			MiningData_st *oreType = FindOre( i->GetColour() );
			if( oreType == nullptr )
			{
				s->SysMessage( 977 ); // Unknown ingot type.
				return;
			}
			if( FindItemOwner( i ) != mChar )
			{
				s->SysMessage( 978, oreType->name.c_str() ); // You can't smith %s ingots outside your backpack.
			}
			else
			{
				AnvilTarget( s, ( *i ), oreType );
			}
		}
		else
		{
			// something we might repair?
			RepairMetal( s );
		}
		return;
	}
	s->SysMessage( 979 ); // You cannot use that material for blacksmithing!
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::AnvilTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check for the presence of an anvil when player attempts to use smithing skill,
//|					then verify that there is enough of the chosen ingot type in their backpack(s)
//o------------------------------------------------------------------------------------------------o
void CSkills::AnvilTarget( CSocket *s, CItem& item, MiningData_st *oreType )
{
	VALIDATESOCKET( s );
	CChar *mChar = s->CurrcharObj();

	for( auto &MapArea : MapRegion->PopulateList( mChar ))
	{
		if( MapArea )
		{
			auto regItems = MapArea->GetItemList();
			for( const auto &tempItem : regItems->collection() )
			{
				if( ValidateObject( tempItem ) && ( tempItem->GetInstanceId() == mChar->GetInstanceId() ))
				{
					if( tempItem->GetId() == 0x0FAF || tempItem->GetId() == 0x0FB0 )
					{
						if( ObjInRange( mChar, tempItem, DIST_NEARBY ))
						{
							UI32 getAmt = GetItemAmount( mChar, item.GetId(), item.GetColour(), item.GetTempVar( CITV_MORE ));
							if( getAmt == 0 )
							{
								s->SysMessage( 980, oreType->name.c_str() ); // You don't have enough %s ingots to make anything.
								return;
							}
							NewMakeMenu( s, oreType->makemenu, BLACKSMITHING );
							return;
						}
					}
				}
			}			
		}
	}
	s->SysMessage( 981 ); // The anvil is too far away.
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::CSkills()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Default constructor
//o------------------------------------------------------------------------------------------------o
CSkills::CSkills()
{
	ores.resize( 0 );
}
CSkills::~CSkills()
{
	ores.resize( 0 );
	skillMenus.clear();
	itemsForMenus.clear();
	actualMenus.clear();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::LoadMiningData()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load mining resource stuff from scripts
//o------------------------------------------------------------------------------------------------o
bool CSkills::LoadMiningData( void )
{
	ores.resize( 0 );
	// Let's first get our ore list, in SECTION ORE_LIST
	CScriptSection *oreList = FileLookup->FindEntry( "ORE_LIST", skills_def );
	bool rValue = false;
	if( oreList != nullptr )
	{
		std::vector<std::string> oreNameList;
		std::string tag;
		std::string data;
		std::string UTag;
		for( tag = oreList->First(); !oreList->AtEnd(); tag = oreList->Next() )
		{
			oreNameList.push_back( tag );
		}
		if( !oreNameList.empty() )
		{
			rValue = true;
			CScriptSection *individualOre = nullptr;
			for( auto &oreName : oreNameList )
			{
				individualOre = FileLookup->FindEntry( oreName, skills_def );
				if( individualOre != nullptr )
				{
					MiningData_st toAdd;
					toAdd.colour	= 0;
					toAdd.makemenu	= 0;
					toAdd.minSkill	= 0;
					toAdd.name		= oreName;
					toAdd.oreName	= oreName;
					toAdd.oreChance = 0;
					for( const auto &sec : individualOre->collection() )
					{
						tag = sec->tag;
						data = sec->data;
						UTag = oldstrutil::upper( tag );
						data = oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ));
						switch(( UTag.data()[0] )) // break on tag
						{
							case 'C':
								if( UTag == "COLOUR" )
								{
									toAdd.colour = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
								}
								break;
							case 'F':
								break;
							case 'M':
								if( UTag == "MAKEMENU" )
								{
									toAdd.makemenu = std::stoi( data, nullptr, 0 );
								}
								else if( UTag == "MINSKILL" )
								{
									toAdd.minSkill = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
								}
								break;
							case 'N':
								if( UTag == "NAME" )
								{
									toAdd.name = data;
								}
								break;
							case 'O':
								if( UTag == "ORECHANCE" )
								{
									toAdd.oreChance = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
								}
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
	return rValue;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::Load()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load mining stuff from scripts
//o------------------------------------------------------------------------------------------------o
void CSkills::Load( void )
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
		if( ourScript != nullptr )
		{
			ourScript->ScriptRegistration( "Skill" );
		}
	}

	Console.PrintSectionBegin();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::GetNumberOfOres()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns number of custom ores
//o------------------------------------------------------------------------------------------------o
size_t CSkills::GetNumberOfOres( void )
{
	return ores.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::GetOre()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a handle to the data about the ore
//o------------------------------------------------------------------------------------------------o
MiningData_st *CSkills::GetOre( size_t number )
{
	if( number >= ores.size() )
		return nullptr;

	return &ores[number];
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::FindOre()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a handle to the data about the ore based on it's name
//o------------------------------------------------------------------------------------------------o
MiningData_st *CSkills::FindOre( std::string const &name )
{
	std::vector<MiningData_st>::iterator	oreIter;
	for( oreIter = ores.begin(); oreIter != ores.end(); ++oreIter )
	{
		if(( *oreIter ).oreName == name )
		{
			return &( *oreIter );
		}
	}
	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	*CSkills::FindOre()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find ore color
//o------------------------------------------------------------------------------------------------o
MiningData_st *CSkills::FindOre( UI16 const &colour )
{
	std::vector<MiningData_st>::iterator oreIter;
	for( oreIter = ores.begin(); oreIter != ores.end(); ++oreIter )
	{
		if(( *oreIter ).colour == colour )
		{
			return &( *oreIter );
		}
	}
	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::LoadCreateMenus()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load Create menus from the create DFNs
//o------------------------------------------------------------------------------------------------o
auto CSkills::LoadCreateMenus() -> void
{
	actualMenus.clear();
	skillMenus.clear();
	itemsForMenus.clear();

	UI16 ourEntry; // our actual entry number
	for( Script *ourScript = FileLookup->FirstScript( create_def ); !FileLookup->FinishedScripts( create_def ); ourScript = FileLookup->NextScript( create_def ))
	{
		if( ourScript == nullptr )
		{
			continue;
		}

		for( const auto &[eName1, toSearch] : ourScript->collection() )
		{
			auto eName = eName1;
			if( "SUBMENU" == eName.substr( 0, 7 )) // is it a menu? (not really SUB, just to avoid picking up MAKEMENUs)
			{
				eName = oldstrutil::trim( oldstrutil::removeTrailing( eName, "//" ));
				auto ssecs = oldstrutil::sections( eName, " " );
				ourEntry = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 ));
				for( const auto &sec : toSearch->collection() )
				{
					auto tag = sec->tag;
					auto data = sec->data;
					auto UTag = oldstrutil::upper( tag );
					data = oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ));
					if( UTag == "MENU" )
					{
						actualMenus[ourEntry].menuEntries.push_back( static_cast<UI16>( std::stoul( data, nullptr, 0 )));
					}
					else if( UTag == "ITEM" )
					{
						actualMenus[ourEntry].itemEntries.push_back( static_cast<UI16>( std::stoul( data, nullptr, 0 )));
					}
				}
			}
			else if( "ITEM" == eName.substr( 0, 4 )) // is it an item?
			{
				auto ssecs = oldstrutil::sections( eName, " " );
				
				ourEntry = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 ));
				CreateEntry_st tmpEntry;
				tmpEntry.minRank		= 0;
				tmpEntry.maxRank		= 10;
				tmpEntry.colour			= 0;
				tmpEntry.targId			= 1;
				tmpEntry.soundPlayed	= 0;

				for( const auto &sec : toSearch->collection() )
				{
					auto tag = sec->tag;
					auto data = sec->data;
					auto UTag = oldstrutil::upper( tag );
					data = oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ));
					if( UTag == "COLOUR" )
					{
						tmpEntry.colour = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
					}
					else if( UTag == "ID" )
					{
						tmpEntry.targId = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
					}
					else if( UTag == "MINRANK" )
					{
						tmpEntry.minRank = static_cast<UI08>( std::stoul( data, nullptr, 0 ));
					}
					else if( UTag == "MAXRANK" )
					{
						tmpEntry.maxRank = static_cast<UI08>( std::stoul( data, nullptr, 0 ));
					}
					else if( UTag == "NAME" )
					{
						tmpEntry.name = data;
					}
					else if( UTag == "SOUND" )
					{
						tmpEntry.soundPlayed = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
					}
					else if( UTag == "ADDITEM" )
					{
						tmpEntry.addItem = data;
					}
					else if( UTag == "DELAY" )
					{
						tmpEntry.delay = static_cast<SI16>( std::stoi( data, nullptr, 0 ));
					}
					else if( UTag == "RESOURCE" )
					{
						ResAmountPair_st tmpResource;
						auto ssecs = oldstrutil::sections( data," ");
						if( ssecs.size() > 1 )
						{
							if( ssecs.size() == 4 )
							{
								tmpResource.moreVal = static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[3], "//" )), nullptr, 0 ));
							}
							if( ssecs.size() >= 3 )
							{
								tmpResource.colour = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[2], "//" )), nullptr, 0 ));
							}
							if( ssecs.size() >= 2 )
							{
								tmpResource.amountNeeded = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 ));
							}
						}
						auto resType = "RESOURCE "s + oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" ));
						auto resList = FileLookup->FindEntry( resType, create_def );
						if( resList )
						{
							for( const auto &sec : resList->collection() )
							{
								tmpResource.idList.push_back( static_cast<UI16>( std::stoul( sec->data, nullptr, 0 )));
							}
						}
						else
						{
							tmpResource.idList.push_back( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
						}

						tmpEntry.resourceNeeded.push_back( tmpResource );
					}
					else if( UTag == "SKILL" )
					{
						ResSkillReq_st tmpResource;
						auto ssecs = oldstrutil::sections( data, " " );
						if( ssecs.size() == 1 )
						{
							tmpResource.maxSkill	= 1000;
							tmpResource.minSkill	= 0;
							tmpResource.skillNumber	=  static_cast<UI16>( std::stoul( data, nullptr, 0 ));
						}
						else
						{
							if( ssecs.size() == 2 )
							{
								tmpResource.maxSkill = 1000;
								tmpResource.minSkill =  static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 ));
							}
							else
							{
								tmpResource.minSkill = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 ));
								tmpResource.maxSkill = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[2], "//" )), nullptr, 0 ));
							}
							tmpResource.skillNumber = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 ));
						}
						tmpEntry.skillReqs.push_back( tmpResource );
					}
					else if( UTag == "SPELL" )
					{
						tmpEntry.spell = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
					}
				}
				itemsForMenus[ourEntry] = tmpEntry;
			}
			else if( "MENUENTRY" == eName.substr( 0, 9 ))
			{
				auto ssecs = oldstrutil::sections( eName, " " );
				ourEntry = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 ));
				for( const auto &sec : toSearch->collection() )
				{
					auto tag = sec->tag;
					auto data = sec->data;
					auto UTag = oldstrutil::upper( tag );
					data = oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ));
					if( UTag == "ID" )
					{
						skillMenus[ourEntry].targId =  static_cast<UI16>( std::stoul( data, nullptr, 0 ));
					}
					else if( UTag == "COLOUR" )
					{
						skillMenus[ourEntry].colour =  static_cast<UI16>( std::stoul( data, nullptr, 0 ));
					}
					else if( UTag == "NAME" )
					{
						skillMenus[ourEntry].name = data;
					}
					else if( UTag == "SUBMENU" )
					{
						skillMenus[ourEntry].subMenu = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
					}
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::AdvanceSkill()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Advance players skill based on success or failure in CheckSkill()
//o------------------------------------------------------------------------------------------------o
bool CSkills::AdvanceSkill( CChar *s, UI08 sk, bool skillUsed )
{
	bool advSkill = false;
	SI16 skillGain;

	SI08 skillAdvance = FindSkillPoint( sk, s->GetBaseSkill( sk ));

	if( skillUsed )
	{
		skillGain = ( cwmWorldState->skill[sk].advancement[skillAdvance].success );
	}
	else
	{
		skillGain = ( cwmWorldState->skill[sk].advancement[skillAdvance].failure );
	}

	if( skillGain > RandomNum( 1, 100 ))
	{
		advSkill = true;
		if( s->GetSkillLock( sk ) == SKILL_INCREASE )
		{
			HandleSkillChange( s, sk, skillAdvance, skillUsed );
		}
	}

	if( s->GetSkillLock( sk ) != SKILL_LOCKED ) // if it's locked, stats can't advance
	{
		AdvanceStats( s, sk, skillUsed );
	}
	return advSkill;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::FindSkillPoint()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find skillpoint advancement parameters from a skill in skills.dfn,
//|					based on a specified skill value
//o------------------------------------------------------------------------------------------------o
SI08 CSkills::FindSkillPoint( UI08 sk, SI32 value )
{
	SI08 retVal = -1;
	for( size_t iCounter = 0; iCounter < cwmWorldState->skill[sk].advancement.size() - 1; ++iCounter )
	{
		if( cwmWorldState->skill[sk].advancement[iCounter].base <= value && value < cwmWorldState->skill[sk].advancement[iCounter+1].base )
		{
			retVal = static_cast<SI08>( iCounter );
			break;
		}
	}
	if( retVal == -1 )
	{
		retVal = static_cast<SI08>( cwmWorldState->skill[sk].advancement.size() - 1 );
	}
	return retVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::AdvanceStats()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Advance players stats
//o------------------------------------------------------------------------------------------------o
void CSkills::AdvanceStats( CChar *s, UI08 sk, bool skillsuccess )
{
	CRace *pRace = Races->Race( s->GetRace() );

	// If the Race is invalid just use the default race
	if( pRace == nullptr )
	{
		pRace = Races->Race( 0 );
	}

	//make sure socket is no npc
	if( s->IsNpc() )
		return;

	UI32 serverStatCap = cwmWorldState->ServerData()->ServerStatCapStatus();
	UI32 ttlStats = s->ActualStrength() + s->ActualDexterity() + s->ActualIntelligence();
	SI16 chanceStatGain = 0; //16bit because of freaks that raises it > 100
	SI32 statCount, nCount;
	SI32 toDec = 255;
	UI16 maxChance = 100;
	SI16 ActualStat[3] = { s->ActualStrength() , s->ActualDexterity() , s->ActualIntelligence() };
	UI16 StatModifier[3] = { cwmWorldState->skill[sk].strength , cwmWorldState->skill[sk].dexterity , cwmWorldState->skill[sk].intelligence };
	SkillLock StatLocks[3] = { s->GetSkillLock( STRENGTH ), s->GetSkillLock( DEXTERITY ), s->GetSkillLock( INTELLECT ) };

	std::vector<UI16> skillUpdTriggers = s->GetScriptTriggers();

	for( statCount = STRENGTH; statCount <= INTELLECT; ++statCount )
	{
		nCount = statCount - ALLSKILLS - 1;

		// if current stat isn't allowed to increase skip it.
		if( StatLocks[nCount] == SKILL_INCREASE )
		{
			//  the following will calculate the chances for str/dex/int to increase
			//
			//  it is divided into 2 "dices":
			//  first dice: get the success-skillpoint of the stat out of skills.dfn in dependence
			//				of the racial statCap for this stat... x means stat is x% of statCap
			//				=> get skillpoint for x
			//  sec. dice:	get the chance for this stat to increase when the used skill has been used
			//				(out of skills.dfn) => get x = statmodifier of skill

			//  last make it a integer between 0 and 1000 normally (negative or 0==no chance)

			//	special dice 1: the stat wont increase above x% of the racial statCap. x% is equivalent to dice 2.
			//  special dice 2: skill failed: decrease chance by 50%

			//  k, first let us calculate both dices
			UI08 modifiedStatLevel = FindSkillPoint( statCount-1, static_cast<SI32>( static_cast<R32>( ActualStat[nCount] ) / static_cast<R32>( pRace->Skill( statCount )) * 100 ));
			chanceStatGain = static_cast<SI16>(( static_cast<R32>( cwmWorldState->skill[statCount-1].advancement[modifiedStatLevel].success ) / 100 ) * ( static_cast<R32>( static_cast<R32>( StatModifier[nCount] ) / 10 ) / 100 ) * 1000 );
			// some mathematics in it;)

			// now, lets implement the special dice 1 and additionally check for onStatGain javascript method
			if( StatModifier[nCount] <= static_cast<SI32>( static_cast<R32>( ActualStat[nCount] ) / static_cast<R32>( pRace->Skill( statCount )) * 100 ))
			{
				chanceStatGain = 0;
			}

			// special dice 2
			if( !skillsuccess )
			{
				maxChance *= 2;
			}

			// if stat of char < racial statCap and chance for statgain > random number from 0 to 100
			if( ActualStat[nCount] < pRace->Skill( statCount ) && chanceStatGain > RandomNum( static_cast<UI16>( 0 ), maxChance ))
			{
				// Check if we have to decrease a stat
				if(( ttlStats + 1) >= RandomNum( serverStatCap - 10, serverStatCap ))
				{
					for( SI32 i = 0; i < 3; i++ )
					{
						if( StatLocks[i] == SKILL_DECREASE )
						{
							// Decrease the highest stat, that is allowed to decrease
							if( toDec == 255 )
							{
								toDec = i;
							}
							else
							{
								if( ActualStat[i] > ActualStat[toDec] )
								{
									toDec = i;
								}
							}
						}
					}

					switch( toDec )
					{
						case 0: // Decrease Strength stat
							// First trigger onStatLoss event
							for( auto skillTrig : skillUpdTriggers )
							{
								cScript *toExecute = JSMapping->GetScript( skillTrig );
								if( toExecute != nullptr )
								{
									if( !toExecute->OnStatLoss( s, STRENGTH, 1 ))
										return;
								}
							}

							// Do the actual stat decrease
							s->IncStrength( -1 );
							ttlStats--;

							// Trigger onStatChange event if onStatLoss either didn't exist, or returned true
							for( auto skillTrig : skillUpdTriggers )
							{
								cScript *toExecute = JSMapping->GetScript( skillTrig );
								if( toExecute != nullptr )
								{
									toExecute->OnStatChange( s, STRENGTH, -1 );
								}
							}
							break;
						case 1: // Decrease Dexterity stat
							// First trigger onStatLoss event
							for( auto skillTrig : skillUpdTriggers )
							{
								cScript *toExecute = JSMapping->GetScript( skillTrig );
								if( toExecute != nullptr )
								{
									if( !toExecute->OnStatLoss( s, DEXTERITY, 1 ))
										return;
								}
							}

							// Do the actual stat decrease
							s->IncDexterity( -1 );
							ttlStats--;

							// Trigger onStatChange event if onStatLoss either didn't exist, or returned true
							for( auto skillTrig : skillUpdTriggers )
							{
								cScript *toExecute = JSMapping->GetScript( skillTrig );
								if( toExecute != nullptr )
								{
									toExecute->OnStatChange( s, DEXTERITY, -1 );
								}
							}
							break;
						case 2: // Decrease Intelligence stat
							// First trigger onStatLoss event
							for( auto skillTrig : skillUpdTriggers )
							{
								cScript *toExecute = JSMapping->GetScript( skillTrig );
								if( toExecute != nullptr )
								{
									if( !toExecute->OnStatLoss( s, INTELLECT, 1 ))
										return;
								}
							}

							// Do the actual stat decrease
							s->IncIntelligence( -1 );
							ttlStats--;

							// Trigger onStatChange event if onStatLoss either didn't exist, or returned true
							for( auto skillTrig : skillUpdTriggers )
							{
								cScript *toExecute = JSMapping->GetScript( skillTrig );
								if( toExecute != nullptr )
								{
									toExecute->OnStatChange( s, INTELLECT, -1 );
								}
							}
							break;
						default:
							break;
					}

				}

				// Do we still hit the stat limit?
				if(( ttlStats + 1) <= serverStatCap )
				{
					// First trigger onStatGained
					for( auto skillTrig : skillUpdTriggers )
					{
						cScript *toExecute = JSMapping->GetScript( skillTrig );
						if( toExecute != nullptr )
						{
							if( !toExecute->OnStatGained( s, statCount, sk, 1 ))
								return;
						}
					}

					// Do the actual stat increase
					switch( statCount )
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

					// Trigger onStatChange event if onStatGained either didn't exist, or returned true
					for( auto skillTrig : skillUpdTriggers )
					{
						cScript *toExecute = JSMapping->GetScript( skillTrig );
						if( toExecute != nullptr )
						{
							toExecute->OnStatChange( s, statCount, 1 );
						}
					}

					break; // only one stat at a time fellas
				}
			}
		}
	}

	s->Dirty( UT_STATWINDOW );
	for( UI08 i = 0; i < ALLSKILLS; ++i )
	{
		UpdateSkillLevel( s, i );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::NewMakeMenu()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	New make menu system, based on create DFNs
//o------------------------------------------------------------------------------------------------o
void CSkills::NewMakeMenu( CSocket *s, SI32 menu, [[maybe_unused]] UI08 skill )
{
	VALIDATESOCKET( s );
	CChar *ourChar = s->CurrcharObj();
	s->AddId( menu );
	std::map<UI16, CreateMenu_st>::const_iterator p = actualMenus.find( menu );
	if( p == actualMenus.end() )
		return;

	UI16 background = cwmWorldState->ServerData()->BackgroundPic();
	UI16 btnCancel	= cwmWorldState->ServerData()->ButtonCancel();
	[[maybe_unused]] UI16 btnLeft	= cwmWorldState->ServerData()->ButtonLeft();
	UI16 btnRight	= cwmWorldState->ServerData()->ButtonRight();

	CPSendGumpMenu toSend;
	toSend.UserId( INVALIDSERIAL );
	toSend.GumpId( 12 );

	CreateMenu_st	ourMenu		= p->second;
	UI32		textCounter = 0;
	CScriptSection *GumpHeader = FileLookup->FindEntry( "ADDMENU HEADER", misc_def );
	if( GumpHeader == nullptr )
	{
		toSend.addCommand( oldstrutil::format( "resizepic 0 0 %i 400 320", background) );
		toSend.addCommand( "page 0" );
		toSend.addCommand( "text 200 20 0 0" );
		toSend.addText( "Create menu" );
		++textCounter;
		toSend.addCommand( oldstrutil::format( "button 360 15 %i %i 1 0 1", btnCancel, btnCancel + 1 ));
	}
	else
	{
		std::string tag, data, UTag;
		for( tag = GumpHeader->First(); !GumpHeader->AtEnd(); tag = GumpHeader->Next() )
		{
			UTag = oldstrutil::upper( tag );
			data = GumpHeader->GrabData();
			data = oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ));
			if( UTag == "BUTTONLEFT" )
			{
				btnLeft = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
			}
			else if( UTag == "BUTTONRIGHT" )
			{
				btnRight = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
			}
			else if( UTag == "BUTTONCANCEL" )
			{
				btnCancel = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
			}
			else
			{
				std::string built = tag;
				if( !data.empty() )
				{
					built += " ";
					built += data;
				}
				toSend.addCommand( built );
			}
		}
		CScriptSection *GumpText = FileLookup->FindEntry( "ADDMENU TEXT", misc_def );
		if( GumpText != nullptr )
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
				toSend.addText( built );
				++textCounter;
			}
		}
	}

	UI16 xLoc = 60, yLoc = 40;
	std::map<UI16, CreateEntry_st>::iterator imIter;
	std::map<UI16, CreateMenuEntry_st>::iterator smIter;
	SI32 actualItems = 0;
	for( ourMenu.iIter = ourMenu.itemEntries.begin(); ourMenu.iIter != ourMenu.itemEntries.end(); ++ourMenu.iIter )
	{
		if(( actualItems % 6 ) == 0 && actualItems != 0 )
		{
			xLoc += 180;
			yLoc = 40;
		}
		imIter = itemsForMenus.find(( *ourMenu.iIter ));
		if( imIter != itemsForMenus.end() )
		{
			CreateEntry_st iItem = imIter->second;
			bool canMake = true;
			for( size_t sCounter = 0; sCounter < iItem.skillReqs.size() && canMake; ++sCounter )
			{
				UI08 skillNum = iItem.skillReqs[sCounter].skillNumber;
				UI16 ourSkill = ourChar->GetSkill( skillNum );
				UI16 minSkill = iItem.skillReqs[sCounter].minSkill;
				canMake = ( ourSkill >= minSkill );
				// it was not thought that way, its not logical, maxSkill should say, that you can get maxRank with maxSkill and higher
			}
			if( iItem.spell ) // Should only display the spell if character has a spellbook and the spell in his book
			{
				CItem *spellBook = FindItemOfType( ourChar, IT_SPELLBOOK );
				if( !ValidateObject( spellBook ))
				{
					canMake = false;
				}
				else
				{
					SI32 getCir = static_cast<SI32>( iItem.spell * .1 );
					SI32 getSpell = iItem.spell - ( getCir * 10 ) + 1;
					if( !Magic->CheckBook( getCir, getSpell - 1, spellBook ))
					{
						canMake = false;
					}
				}
			}
			if( canMake )
			{
				toSend.addCommand( oldstrutil::format( "button %i %i %i %i 1 0 %i", xLoc - 40, yLoc, btnRight, btnRight + 1, ( *ourMenu.iIter )));
				if( iItem.targId )
				{
					toSend.addCommand( oldstrutil::format( "tilepic %i %i %i", xLoc - 20, yLoc, iItem.targId ));
				}
				toSend.addCommand( oldstrutil::format( "text %i %i 35 %i", xLoc + 20, yLoc, textCounter++ ));
				toSend.addText( iItem.name );
				yLoc += 40;
				++actualItems;
			}
		}
	}

	actualItems = 0;
	for( ourMenu.mIter = ourMenu.menuEntries.begin(); ourMenu.mIter != ourMenu.menuEntries.end(); ++ourMenu.mIter )
	{
		if(( actualItems % 6 ) == 0 && actualItems != 0 )
		{
			xLoc += 180;
			yLoc = 40;
		}
		smIter = skillMenus.find(( *ourMenu.mIter ));
		if( smIter != skillMenus.end() )
		{
			CreateMenuEntry_st iMenu = smIter->second;
			toSend.addCommand(oldstrutil::format( "button %i %i %i %i 1 0 %i", xLoc - 40, yLoc, btnRight, btnRight + 1, CREATE_MENU_OFFSET + ( *ourMenu.mIter )));
			if( iMenu.targId )
			{
				toSend.addCommand( oldstrutil::format( "tilepic %i %i %i", xLoc - 20, yLoc, iMenu.targId) );
			}
			toSend.addCommand( oldstrutil::format( "text %i %i 35 %i", xLoc + 20, yLoc, textCounter++) );
			toSend.addText( iMenu.name );
			yLoc += 40;
			++actualItems;
		}
	}
	toSend.Finalize();
	s->Send( &toSend );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::HandleMakeMenu()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles the button pressed in the new make menu
//o------------------------------------------------------------------------------------------------o
void CSkills::HandleMakeMenu( CSocket *s, SI32 button, SI32 menu )
{
	VALIDATESOCKET( s );
	CChar *ourChar = s->CurrcharObj();
	s->AddId( 0 );
	std::map<UI16, CreateMenu_st>::const_iterator p = actualMenus.find( menu );
	if( p == actualMenus.end() )
		return;

	CreateMenu_st ourMenu = p->second;
	if( button >= CREATE_MENU_OFFSET )	// menu pressed
	{
		std::map<UI16, CreateMenuEntry_st>::const_iterator q = skillMenus.find( button-CREATE_MENU_OFFSET );
		if( q == skillMenus.end() )
			return;

		NewMakeMenu( s, q->second.subMenu, 0 );
	}
	else				// item to make
	{
		std::map<UI16, CreateEntry_st>::iterator r = itemsForMenus.find( button );
		if( r == itemsForMenus.end() )
			return;

		MakeItem( r->second, ourChar, s, button );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::FindItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns a handle to a CreateEntry_st based on an itemNum
//o------------------------------------------------------------------------------------------------o
CreateEntry_st *CSkills::FindItem( UI16 itemNum )
{
	std::map<UI16, CreateEntry_st>::iterator r = itemsForMenus.find( itemNum );
	if( r == itemsForMenus.end() )
		return nullptr;

	return &( r->second );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::MakeItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Makes an item selected in the new make menu system
//o------------------------------------------------------------------------------------------------o
void CSkills::MakeItem( CreateEntry_st &toMake, CChar *player, CSocket *sock, UI16 itemEntry, UI16 resourceColour )
{
	VALIDATESOCKET( sock );

	if( !ValidateObject( player ))
		return;

	// Get potential tag with ID of a targeted sub-resource
	UI16 targetedSubResourceId = 0;
	TAGMAPOBJECT tempTagObj = player->GetTempTag( "targetedSubResourceId" );
	if( tempTagObj.m_ObjectType == TAGMAP_TYPE_INT && tempTagObj.m_IntValue > 0 )
	{
		targetedSubResourceId = static_cast<UI16>( tempTagObj.m_IntValue );
	}

	std::vector<ResAmountPair_st>::const_iterator resCounter;
	std::vector<ResSkillReq_st>::const_iterator sCounter;
	ResAmountPair_st resEntry;
	UI16 toDelete;
	UI16 targColour;
	UI16 targId;
	UI32 targMoreVal;
	bool canDelete = true;

	//Moved resource-check to top of file to disallow gaining skill by attempting to
	//craft items without having enough resources to do so :P
	for( resCounter = toMake.resourceNeeded.begin(); resCounter != toMake.resourceNeeded.end(); ++resCounter )
	{
		resEntry	= ( *resCounter );
		toDelete	= resEntry.amountNeeded;
		targColour	= resEntry.colour;
		targMoreVal = resEntry.moreVal;

		if( resCounter == toMake.resourceNeeded.begin() && resourceColour != 0 )
		{
			// If a specific colour was provided for the function as the intended colour of the material, use a material of that colour
			targColour = resourceColour;
		}
		for( std::vector<UI16>::const_iterator idCounter = resEntry.idList.begin(); idCounter != resEntry.idList.end(); ++idCounter )
		{
			targId = ( *idCounter );
			if( targetedSubResourceId == 0 || resCounter == toMake.resourceNeeded.begin() )
			{
				// Primary resource, or generic subresource
				toDelete -= std::min( GetItemAmount( player, targId, targColour, targMoreVal, true ), static_cast<UI32>( toDelete ));
			}
			else
			{
				if( targetedSubResourceId > 0 && targId == targetedSubResourceId )
				{
					// Player specifically targeted a secondary resource
					toDelete -= std::min( GetItemAmount( player, targId, targColour, targMoreVal, true ), static_cast<UI32>( toDelete ));
				}
			}

			if( toDelete == 0 )
				break;
		}
		if( toDelete > 0 )
		{
			canDelete = false;
		}
	}
	if( !canDelete )
	{
		sock->SysMessage( 1651 ); // You have insufficient resources on hand to do that
		return;
	}

	bool canMake = true;

	// Loop through the skills listed as skill requirement for crafting the item
	for( sCounter = toMake.skillReqs.begin(); sCounter != toMake.skillReqs.end(); ++sCounter )
	{
		if( player->SkillUsed(( *sCounter ).skillNumber ))
		{
			sock->SysMessage( 1650 ); // You are already using that skill
			return;
		}

		if( sCounter == toMake.skillReqs.begin() )
		{
			// Only perform skill check for first and main skill in skill requirement
			if( !CheckSkill( player, ( *sCounter ).skillNumber, ( *sCounter ).minSkill, ( *sCounter ).maxSkill, true ))
			{
				canMake = false;
				break;
			}
		}
		else
		{
			// For supporting skills, we only check if player has more than or equal to the minimum skill required
			if( player->GetSkill(( *sCounter ).skillNumber ) < ( *sCounter ).minSkill )
			{
				canMake = false;
				break;
			}
		}
	}

	if( !canMake )
	{
		// delete anywhere up to half of the resources needed
		if( toMake.soundPlayed )
		{
			Effects->PlaySound( sock, toMake.soundPlayed, true );
		}
		sock->SysMessage( 984 ); // You fail to create the item.
	}
	else
	{
		// Store temp tag on player with colour of item to craft
		TAGMAPOBJECT tagObject;
		tagObject.m_Destroy = false;
		tagObject.m_StringValue = "";
		tagObject.m_IntValue = resourceColour;
		tagObject.m_ObjectType = TAGMAP_TYPE_INT;
		player->SetTempTag( "craftItemColor", tagObject );

		for( sCounter = toMake.skillReqs.begin(); sCounter != toMake.skillReqs.end(); ++sCounter )
		{
			player->SkillUsed( true, ( *sCounter ).skillNumber );
		}

		Effects->TempEffect( player, player, 41, toMake.delay, itemEntry, 0 );
		if( toMake.soundPlayed )
		{
			if( toMake.delay > 300 )
			{
				for( SI16 i = 0; i < ( toMake.delay / 300 ); ++i )
				{
					Effects->TempEffect( player, player, 42, 300 * i, toMake.soundPlayed, 0 );
				}
			}
		}
	}

	if( !canMake || !canDelete )
	{
		// Trigger onMakeItem() JS event for character who tried to craft item, even though they failed
		std::vector<UI16> scriptTriggers = player->GetScriptTriggers();
		for( auto scriptTrig : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				toExecute->OnMakeItem( sock, player, nullptr, 0 );
			}
		}
	}
	for( resCounter = toMake.resourceNeeded.begin(); resCounter != toMake.resourceNeeded.end(); ++resCounter )
	{
		resEntry	= ( *resCounter );
		toDelete	= resEntry.amountNeeded;
		if( !canMake )
		{
			toDelete = RandomNum( 0, std::max( 1, toDelete / 2 ));
		}
		targColour	= resEntry.colour;
		targMoreVal = resEntry.moreVal;

		if( resCounter == toMake.resourceNeeded.begin() && resourceColour != 0 )
		{
			// If a specific colour was provided for the function as the intended colour of the material, use a material of that colour
			targColour = resourceColour;
		}
		for( std::vector<UI16>::const_iterator idCounter = resEntry.idList.begin(); idCounter != resEntry.idList.end(); ++idCounter )
		{
			targId = ( *idCounter );
			if( targetedSubResourceId == 0 || resCounter == toMake.resourceNeeded.begin() )
			{
				// Primary resource, or generic subresource
				toDelete -= DeleteItemAmount( player, toDelete, targId, targColour, targMoreVal );
			}
			else
			{
				if( targetedSubResourceId > 0 && targId == targetedSubResourceId )
				{
					// Player specifically targeted a secondary resource
					toDelete -= DeleteItemAmount( player, toDelete, targId, targColour, targMoreVal );
				}
			}

			if( toDelete == 0 )
				break;
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::RepairMetal()
//|	Date		-	October 16, 2000
//|	Modified	-	November 13, 2001 - changed to a metal repair function
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Repair armor and weapons.
//o------------------------------------------------------------------------------------------------o
void CSkills::RepairMetal( CSocket *s )
{
	SI16 minSkill = 999, maxSkill = 1000;

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

	CItem *j = CalcItemObjFromSer( s->GetDWord( 7 ));
	if( !ValidateObject( j ) || !j->IsMetalType() )
	{
		s->SysMessage( 986 ); // You cannot repair that.
		return;
	}

	if( j->IsHeldOnCursor() || !CheckItemRange( mChar, j ))
	{
		s->SysMessage( 400 ); // That is too far away!
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
				minSkill = ( 610 + static_cast<SI32>(( j->GetResist( PHYSICAL ) - 1 ) / 3 ) * 100 );
				// Maximum Skill = 84.9 + Defense - 1 / 3 * 50 (0-3 = 84.9, 4-6 = 89.9, ect)
				maxSkill = ( 849 + static_cast<SI32>(( j->GetResist( PHYSICAL ) - 1 ) / 3 ) * 50 );
			}
		}
		else if( j->GetHiDamage() > 0 )
		{
			SI32 offset = ( j->GetLoDamage() + j->GetHiDamage() ) / 2;
			// Items with > 25 Avg Damage would have impossible skill req's, with this equation
			if( offset <= 25 )
			{
				// Minimum Skill = 51.0 + Avg Damage - 1 / 5 * 100 (0-5 = 51.0, 6-10 = 61.0, ect)
				minSkill = ( 510 + static_cast<SI32>(( offset - 1 ) / 5 ) * 100 );
				// Maximum Skill = 79.9 + Avg Damage - 1 / 5 * 50 (0-5 = 79.9, 6-10 = 84.9, ect)
				maxSkill = ( 799 + static_cast<SI32>(( offset - 1 ) / 5 ) * 50 );
			}
		}
		else
		{
			s->SysMessage( 987 ); // You cannot repair this item.
			return;
		}
	}
	else
	{
		s->SysMessage( 988 ); // That item is already fully repaired.
		return;
	}
	if( CheckSkill( mChar, BLACKSMITHING, minSkill, maxSkill ))
	{
		j->SetHP( j->GetMaxHP() );
		s->SysMessage( 989 ); // You repair the item successfully.
		Effects->PlaySound( s, 0x002A, true );
	}
	else
	{
		s->SysMessage( 990 ); // You fail to repair the item.
	}
}

void CallGuards( CChar *mChar, CChar *targChar );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::Snooping()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when player snoops another PC/NPC's or a tamed animals pack
//o------------------------------------------------------------------------------------------------o
void CSkills::Snooping( CSocket *s, CChar *target, CItem *pack )
{
	CChar *mChar = s->CurrcharObj();
	CSocket *tSock = target->GetSocket();

	std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			// If script returns false, prevent hard-code and other scripts with event from running
			if( toExecute->OnSnoopAttempt( target, mChar ) == 0 )
			{
				return;
			}
		}
	}

	if( target->GetCommandLevel() > mChar->GetCommandLevel() )
	{
		s->SysMessage( 991 ); // You failed to peek into that container.
		if( tSock != nullptr )
		{
			tSock->SysMessage( 992, mChar->GetNameRequest( target ).c_str() ); // %s is snooping you!
		}
		return;
	}

	// Check if snooping is allowed in player's AND target's regions
	if( mChar->GetRegion()->IsSafeZone() || target->GetRegion()->IsSafeZone() )
	{
		// Target is in a safe zone where all aggressive actions are forbidden, disallow
		s->SysMessage( 1799 ); // Hostile actions are not permitted in this safe area.
		return;
	}

	scriptTriggers.clear();
	scriptTriggers.shrink_to_fit();
	scriptTriggers = target->GetScriptTriggers();

	if( CheckSkill( mChar, SNOOPING, 0, 1000 ))
	{
		s->OpenPack( pack );
		s->SysMessage( 993 ); // You successfully peek into that container.

		for( auto scriptTrig : scriptTriggers )
		{
			cScript *successSnoop = JSMapping->GetScript( scriptTrig );
			if( successSnoop != nullptr )
			{
				// If script returns true/1, prevent other scripts with event from running
				if( successSnoop->OnSnooped( target, mChar, true ) == 1 )
				{
					break;
				}
			}
		}
	}
	else
	{
		bool doNormal = true;

		for( auto scriptTrig : scriptTriggers )
		{
			cScript *failSnoop = JSMapping->GetScript( scriptTrig );
			if( failSnoop != nullptr )
			{
				// If script returns true/1, prevent hard code and other scripts with event from running
				if( failSnoop->OnSnooped( target, mChar, true ) == 1 )
				{
					doNormal = false;
				}
			}
		}

		if( doNormal )
		{
			s->SysMessage( 991 ); // You failed to peek into that container.
			if( target->IsNpc() )
			{
				if( cwmWorldState->creatures[target->GetId()].IsHuman() && target->GetNpcAiType() != AI_EVIL && target->GetNpcAiType() != AI_EVIL_CASTER && target->GetNpcAiType() != AI_HEALER_E )
				{
					// 994=Art thou attempting to disturb my privacy?
					// 995=Stop that!
					// 996=Be aware I am going to call the guards!
					target->TextMessage( s, 994 + RandomNum( 0, 2 ), TALK, false );
					if( cwmWorldState->ServerData()->SnoopIsCrime() )
					{
						if( RandomNum( 0, 1 ) == 1 && mChar->IsCriminal() )	//	50% chance of calling guards, on second time
						{
							CallGuards( target, mChar );
						}
					}
				}
				else
				{
					UI16 toPlay = cwmWorldState->creatures[target->GetId()].GetSound( SND_IDLE );
					if( toPlay != 0x00 )
					{
						Effects->PlaySound( target, toPlay );
					}
				}
			}
			else if( tSock != nullptr )
			{
				tSock->SysMessage( 997, mChar->GetNameRequest( target ).c_str() ); // You notice %s trying to peek into your pack!
			}
			if( cwmWorldState->ServerData()->SnoopIsCrime() )
			{
				MakeCriminal( mChar );
			}
			if( mChar->GetKarma() <= 1000 )
			{
				mChar->SetKarma( mChar->GetKarma() - 10 );
				s->SysMessage( 998 ); // You've lost a small bit of karma.
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CSkills::MakeNecroReg()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Create reagents for necromancer spells
//o------------------------------------------------------------------------------------------------o
void CSkills::MakeNecroReg( CSocket *nSocket, CItem *nItem, UI16 itemId )
{
	CItem *iItem = nullptr;
	CChar *iCharId = nSocket->CurrcharObj();

	if( itemId >= 0x1B11 && itemId <= 0x1B1C ) // Make bone powder.
	{
		iCharId->TextMessage( nullptr, 741, EMOTE, 1, iCharId->GetName().c_str() ); // %s is grinding some bone into powder
		Effects->TempEffect( iCharId, iCharId, 9, 0, 0, 0 );
		Effects->TempEffect( iCharId, iCharId, 9, 0, 3, 0 );
		Effects->TempEffect( iCharId, iCharId, 9, 0, 6, 0 );
		Effects->TempEffect( iCharId, iCharId, 9, 0, 9, 0 );
		iItem = Items->CreateItem( nSocket, iCharId, 0x0F8F, 1, 0, OT_ITEM, true );
		if( iItem == nullptr )
			return;

		iItem->SetName( "bone powder" );
		iItem->SetTempVar( CITV_MOREX, 666 );
		iItem->SetTempVar( CITV_MORE, 1, 1 ); // this will fill more with info to tell difference between ash and bone
		nItem->Delete();

	}
	if( itemId == 0x0E24 ) // Make vial of blood.
	{
		if( nItem->GetTempVar( CITV_MORE, 1 ) == 1 )
		{
			iItem = Items->CreateItem( nSocket, iCharId, 0x0F82, 1, 0, OT_ITEM, true );
			if( iItem == nullptr )
				return;

			iItem->SetBuyValue( 15 );
			iItem->SetTempVar( CITV_MOREX, 666 );
		}
		else
		{
			iItem = Items->CreateItem( nSocket, iCharId, 0x0F7D, 1, 0, OT_ITEM, true );
			if( iItem == nullptr )
				return;

			iItem->SetBuyValue( 10 );
			iItem->SetTempVar( CITV_MOREX, 666 );
		}
		nItem->IncAmount( -1 );
	}
}
