#include "uox3.h"
#include "skills.h"
#include "magic.h"
#include "cMagic.h"
#include "CJSMapping.h"
#include "mapstuff.h"
#include "cScript.h"
#include "cEffects.h"
#include "teffect.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "combat.h"
#include "CJSMapping.h"
#include "townregion.h"
#include "StringUtility.hpp"
#include <algorithm>

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::DeathAction()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays a characters death animation and sends it to nearby players
//o------------------------------------------------------------------------------------------------o
void cEffects::DeathAction( CChar *s, CItem *x, UI08 fallDirection )
{
	CPDeathAction toSend(( *s ), ( *x ));
	toSend.FallDirection( fallDirection );
	for( auto &mSock : FindNearbyPlayers( s ))
	{
		if( mSock->CurrcharObj() != s )	// Death action screws up corpse display for the person who died.
		{
			mSock->Send( &toSend );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::SpawnBloodEffect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Spawns a blood effect item and returns the newly created object to caller
//o------------------------------------------------------------------------------------------------o
CItem * cEffects::SpawnBloodEffect( UI08 worldNum, UI16 instanceId, UI16 bloodColour, BloodTypes bloodType )
{
	// Use default blood decay timer from ini setting
	R32 bloodDecayTimer = static_cast<R32>( cwmWorldState->ServerData()->SystemTimer( tSERVER_BLOODDECAY ));

	// Blood effects, sorted by size of effect from small to large
	std::vector<UI16> bloodIds{ 0x1645, 0x122C, 0x122E, 0x122B, 0x122D, 0x122A, 0x122F };
	UI16 bloodEffectId = 0x122c;

	switch( bloodType )
	{
		case BLOOD_DEATH:
			// Use corpse-specific blood decay timer instead of default one
			bloodDecayTimer = static_cast<R32>( cwmWorldState->ServerData()->SystemTimer( tSERVER_BLOODDECAYCORPSE ));

			// Randomize between large blood effects
			bloodEffectId = bloodIds[RandomNum( static_cast<UI16>( 3 ), static_cast<UI16>( 6 ))];
			break;
		case BLOOD_BLEED:
			// Randomize between small blood effects
			bloodEffectId = bloodIds[RandomNum( static_cast<UI16>( 0 ), static_cast<UI16>( 3 ))];
			break;
		case BLOOD_CRITICAL:
			// Randomize between medium to large blood effects
			bloodEffectId = bloodIds[RandomNum( static_cast<UI16>( 2 ), static_cast<UI16>( 5 ))];

			// More blood, so increase the time it takes to decay
			bloodDecayTimer *= 1.5;
			break;
		default:
			break;
	}
	
	// Spawn the blood effect item
	CItem *blood = Items->CreateBaseItem( worldNum, OT_ITEM, instanceId, false );
	if( ValidateObject( blood ))
	{
		blood->SetId( bloodEffectId );
		blood->SetColour( bloodColour );
		blood->SetDecayable( true );
		blood->SetDecayTime( BuildTimeValue( bloodDecayTimer ));
		return blood;
	}

	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::PlayMovingAnimation( CBaseObject *source, CBaseObject *dest, UI16 effect,
//|										UI08 speed, UI08 loop, bool explode, UI32 hue, UI32 renderMode )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends a message to client to display a moving animation from source object to target object
//o------------------------------------------------------------------------------------------------o
void cEffects::PlayMovingAnimation( CBaseObject *source, CBaseObject *dest, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 hue, UI32 renderMode, bool playLocalMoveFX )
{	//0x0f 0x42 = arrow 0x1b 0xfe=bolt
	if( !ValidateObject( source ) || !ValidateObject( dest ))
		return;

	// TODO - if enhanced client/3d client, send packet 0xC7 (CP3DGraphicalEffect) instead
	// CP3DGraphicalEffect toSend( 0, ( *source ), ( *dest ));

	CPGraphicalEffect2 toSend( 0, ( *source ), ( *dest ));
	toSend.Model( effect );
	toSend.SourceLocation(( *source ));
	toSend.TargetLocation(( *dest ));
	toSend.Z( dest->GetZ() + 14 );
	toSend.ZTrg( dest->GetZ() + 11 );
	toSend.Speed( speed );
	toSend.Duration( loop );
	toSend.ExplodeOnImpact( explode );
	toSend.Hue( hue );
	toSend.RenderMode( renderMode ); // 0x00000004

	// Edge case, in case someone want to play the moving FX locally on source object
	if( source == dest && playLocalMoveFX )
	{
		toSend.Effect( 0x03 );
		if( loop == 0 )
		{
			// Make sure effect has a fixed duration, or it will stick around for ever!
			toSend.Duration( 0x05 );
		}
		toSend.SourceSerial( 0x0000 );
		toSend.TargetSerial( 0x0000 );
		toSend.TargetLocation( 0, 0, 0 );
	}

	// TODO: Used with 3D graphical effect only
	/*toSend.EffectId3D( 0x0000 );
	toSend.ExplodeEffectId( 0x0001 );
	toSend.MovingEffectId( 0xFFFF );
	toSend.TargetObjSerial( dest->GetSerial() );
	toSend.LayerId( 0x01 );
	toSend.Unknown( 0x0000 );*/

	for( auto &tSock : Network->connClients )
	{
		if( ObjInRange( tSock, source, DIST_SAMESCREEN ) && ObjInRange( tSock, dest, DIST_SAMESCREEN ))
		{
			tSock->Send( &toSend );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::PlayMovingAnimation( CBaseObject *source, SI16 x, SI16 y, SI08 z, UI16 effect,
//|										UI08 speed, UI08 loop, bool explode, UI32 hue, UI32 renderMode )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends a message to client to display a moving animation from source object to target location
//o------------------------------------------------------------------------------------------------o
void cEffects::PlayMovingAnimation( CBaseObject *source, SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 hue, UI32 renderMode )
{	//0x0f 0x42 = arrow 0x1b 0xfe=bolt
	if( !ValidateObject( source ))
		return;

	CPGraphicalEffect2 toSend( 0, ( *source ));
	toSend.TargetSerial( INVALIDSERIAL );
	toSend.Model( effect );
	toSend.SourceLocation(( *source ));
	toSend.TargetLocation( x, y, z );
	toSend.Speed( speed );
	toSend.Duration( loop );
	toSend.ExplodeOnImpact( explode );
	toSend.Hue( hue );
	toSend.RenderMode( renderMode );

	for( auto &mSock : FindNearbyPlayers( source, DIST_SAMESCREEN ))
	{
		mSock->Send( &toSend );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::PlayMovingAnimation( SI16 srcX, SI16 srcY, SI08 srcZ, SI16 x, SI16 y, SI08 z, UI16 effect,
//|										UI08 speed, UI08 loop, bool explode, UI32 hue, UI32 renderMode )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends a message to client to display a moving animation from source object to target location
//o------------------------------------------------------------------------------------------------o
void cEffects::PlayMovingAnimation( SI16 srcX, SI16 srcY, SI08 srcZ, SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 hue, UI32 renderMode )
{	//0x0f 0x42 = arrow 0x1b 0xfe=bolt

	CPGraphicalEffect2 toSend( 0 );
	toSend.TargetSerial( INVALIDSERIAL );
	toSend.Model( effect );
	toSend.SourceLocation( srcX, srcY, srcZ );
	toSend.TargetLocation( x, y, z );
	toSend.Speed( speed );
	toSend.Duration( loop );
	toSend.ExplodeOnImpact( explode );
	toSend.Hue( hue );
	toSend.RenderMode( renderMode );

	for( auto &mSock : FindNearbyPlayers( srcX, srcY, srcZ, DIST_SAMESCREEN ))
	{
		mSock->Send( &toSend );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::PlayCharacterAnimation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends message to client to make character perform specified action/anim
//o------------------------------------------------------------------------------------------------o
void cEffects::PlayCharacterAnimation( CChar *mChar, UI16 actionId, UI08 frameDelay, UI08 frameCount, bool playBackwards )
{
	CPCharacterAnimation toSend = ( *mChar );
	toSend.Action( actionId );
	toSend.FrameDelay( frameDelay );
	toSend.FrameCount( frameCount );
	toSend.DoBackwards( playBackwards );
	for( auto &mSock : FindNearbyPlayers( mChar ))
	{
		mSock->Send( &toSend );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::PlayNewCharacterAnimation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends message to client to make character perform specified action/anim
//|					in client versions above 7.0.0.0
//o------------------------------------------------------------------------------------------------o
void cEffects::PlayNewCharacterAnimation( CChar *mChar, UI16 actionId, UI16 subActionId, UI08 subSubActionId )
{
	CPNewCharacterAnimation toSend = ( *mChar );
	toSend.Action( actionId );
	toSend.SubAction( subActionId );
	toSend.SubSubAction( subSubActionId );
	for( auto &mSock : FindNearbyPlayers( mChar ))
	{
		mSock->Send( &toSend );
	}	
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::PlaySpellCastingAnimation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles spellcasting action
//o------------------------------------------------------------------------------------------------o
void cEffects::PlaySpellCastingAnimation( CChar *mChar, UI16 actionId, bool monsterCast, bool monsterAreaCastAnim )
{
	if( !monsterCast && ( mChar->GetBodyType() == BT_GARGOYLE || cwmWorldState->ServerData()->ForceNewAnimationPacket() ))
	{
		if( mChar->GetBodyType() == BT_GARGOYLE )
		{
			if( actionId == 0x10 )
			{
				PlayNewCharacterAnimation( mChar, N_ACT_SPELL, S_ACT_SPELL_AREA ); // Action: 0x0b, SubAction: spell variation 0x01
			}
			else if( actionId == 0x11 )
			{
				PlayNewCharacterAnimation( mChar, N_ACT_SPELL, S_ACT_SPELL_TARGET ); // Action: 0x0b, SubAction: spell variation 0x00
			}
		}
		else
		{
			// Sub actions are in reverse order for humans, compared to gargoyles
			if( actionId == 0x10 )
			{
				PlayNewCharacterAnimation( mChar, N_ACT_SPELL, S_ACT_SPELL_TARGET ); // Action: 0x0b, SubAction: spell variation 0x00
			}
			else if( actionId == 0x11 )
			{
				PlayNewCharacterAnimation( mChar, N_ACT_SPELL, S_ACT_SPELL_AREA ); // Action: 0x0b, SubAction: spell variation 0x01
			}
		}
		return;
	}
	else if( mChar->IsOnHorse() || mChar->GetMounted() ) // Players and NPC are mounted differently...
	{
		if( actionId == 0x10 )
		{
			PlayCharacterAnimation( mChar, ACT_MOUNT_ATT_1H, 0, 5 ); // 0x1A
		}
		else if( actionId == 0x11 )
		{
			PlayCharacterAnimation( mChar, ACT_MOUNT_ATT_BOW, 0, 5 ); // 0x1B
		}
		return;
	}
	else
	{
		if( !monsterCast )
		{
			// Human old animation packet
			PlayCharacterAnimation( mChar, actionId, 0, 7 );
		}
		else
		{
			// Non-human old animation packet
			if( actionId == 0x22 )
				return;

			UI16 castAnim = 0;
			UI08 castAnimLength = 0;
			if( monsterAreaCastAnim )
			{
				castAnim = static_cast<UI16>( cwmWorldState->creatures[mChar->GetId()].CastAnimAreaId() );
				castAnimLength = cwmWorldState->creatures[mChar->GetId()].CastAnimAreaLength();
			}
			else
			{
				castAnim = static_cast<UI16>( cwmWorldState->creatures[mChar->GetId()].CastAnimTargetId() );
				castAnimLength = cwmWorldState->creatures[mChar->GetId()].CastAnimTargetLength();
			}

			// Play cast anim, but fallback to default attack anim (0x04) with anim length of 4 frames if no cast anim was defined in creatures.dfn
			PlayCharacterAnimation( mChar, ( castAnim != 0 ? castAnim : 0x04 ), 0, ( castAnimLength != 0 ? castAnimLength : 4 ));
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::PlayStaticAnimation( CBaseObject *target, UI16 effect, UI08 speed, UI08 loop, bool explode )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends message to client to play a static animation effect on an object
//o------------------------------------------------------------------------------------------------o
void cEffects::PlayStaticAnimation( CBaseObject *target, UI16 effect, UI08 speed, UI08 loop, bool explode )
{
	if( !ValidateObject( target ))
		return;

	CPGraphicalEffect toSend( 3, ( *target ));
	if( target->GetObjType() != OT_CHAR )
	{
		toSend.TargetSerial(( *target ));
	}
	else
	{
		toSend.TargetSerial( INVALIDSERIAL );
	}
	toSend.Model( effect );
	toSend.SourceLocation(( *target ));
	if( target->GetObjType() != OT_CHAR )
	{
		toSend.TargetLocation(( *target ));
	}
	else
	{
		toSend.ZTrg( 15 );
	}
	toSend.Speed( speed );
	toSend.Duration( loop );
	toSend.AdjustDir( false );
	toSend.ExplodeOnImpact( explode );

	for( auto &mSock : FindNearbyPlayers( target, DIST_SAMESCREEN ))
	{
		mSock->Send( &toSend );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::PlayStaticAnimation( SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends message to client to play a static animation effect at a location
//o------------------------------------------------------------------------------------------------o
void cEffects::PlayStaticAnimation( SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode )
{
	CPGraphicalEffect toSend( 2 );
	toSend.Model( effect );
	toSend.SourceLocation( x, y, z );
	toSend.TargetLocation( x, y, z );
	toSend.Speed( speed );
	toSend.Duration( loop );
	toSend.AdjustDir( false );
	toSend.ExplodeOnImpact( explode );

	std::for_each( Network->connClients.begin(), Network->connClients.end(), [&toSend]( CSocket *tSock )
	{
		tSock->Send( &toSend );
	});
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::Bolteffect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends message to client to play a lightning bolt effect on a specific player
//o------------------------------------------------------------------------------------------------o
void cEffects::Bolteffect( CChar *player )
{
	if( !ValidateObject( player ))
		return;

	CPGraphicalEffect toSend( 1, ( *player ));
	toSend.SourceLocation(( *player ));
	toSend.ExplodeOnImpact( false );
	toSend.AdjustDir( false );
	for( auto &mSock : FindNearbyPlayers( player ))
	{
		mSock->Send( &toSend );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ExplodeItem()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Explode an item, dealing damage to nearby characters and deleting the item
//o------------------------------------------------------------------------------------------------o
auto ExplodeItem( CSocket *mSock, CItem *nItem, UI32 damage = 0, [[maybe_unused]] UI08 damageType = 0, bool explodeNearby = true ) -> void
{
	auto c = mSock->CurrcharObj();

	UI32 dx, dy, dz;
	// - send the effect (visual and sound)
	if( nItem->GetCont() )
	{
		Effects->PlayStaticAnimation( c, 0x36B0, 0x00, 0x09 );
		nItem->SetCont( nullptr );
		nItem->SetLocation( c );
		Effects->PlaySound( c, 0x0207 );
	}
	else
	{
		Effects->PlayStaticAnimation( nItem, 0x36B0, 0x00, 0x09, 0x00 );
		Effects->PlaySound( nItem, 0x0207 );
	}

	UI32 len = nItem->GetTempVar( CITV_MOREX ) / 250; //4 square max damage at 100 alchemy
	if( damage == 0 )
	{
		damage	= RandomNum( nItem->GetTempVar( CITV_MOREZ ) * 5, nItem->GetTempVar( CITV_MOREZ ) * 10 );
	}

	if( damage < 5 )
	{
		damage = RandomNum( 5, 10 );  // 5 points minimum damage
	}
	if( len < 2 )
	{
		len = 2;  // 2 square min damage range
	}

	if( explodeNearby )
	{
		// Explode for characters nearby
		for( auto &Cell : MapRegion->PopulateList( nItem ))
		{
			bool chain = false;

			auto regChars = Cell->GetCharList();
			for( const auto &tempChar : regChars->collection() )
			{
				if( !ValidateObject( tempChar ) || tempChar->GetInstanceId() != nItem->GetInstanceId() )
					continue;

				if( !tempChar->GetRegion()->IsSafeZone() )
				{
					// Character is in not safe zone, count damage
					dx = abs( tempChar->GetX() - nItem->GetX() );
					dy = abs( tempChar->GetY() - nItem->GetY() );
					dz = abs( tempChar->GetZ() - nItem->GetZ() );
					if( dx <= len && dy <= len && dz <= len )
					{
						if( !tempChar->IsGM() && !tempChar->IsInvulnerable() && ( tempChar->IsNpc() || IsOnline(( *tempChar ))))
						{
							//UI08 hitLoc = Combat->CalculateHitLoc();
							damage = Combat->ApplyDefenseModifiers( HEAT, c, tempChar, ALCHEMY, 0, ( static_cast<SI32>( damage ) + ( 2 - std::min( dx, dy ))), true );
							[[maybe_unused]] bool retVal = tempChar->Damage( static_cast<SI16>( damage ), HEAT, c, true );
						}
					}
				}
			}
			auto regItems = Cell->GetItemList();
			for( const auto &tempItem : regItems->collection() )
			{
				if( !ValidateObject( tempItem ) || tempItem->GetInstanceId() != nItem->GetInstanceId() )
					continue;

				if( tempItem->GetId() == 0x0F0D && tempItem->GetType() == IT_POTION )
				{
					dx = abs( nItem->GetX() - tempItem->GetX() );
					dy = abs( nItem->GetY() - tempItem->GetY() );
					dz = abs( nItem->GetZ() - tempItem->GetZ() );

					if( dx <= 2 && dy <= 2 && dz <= 2 && !chain ) // only trigger if in a 2*2*2 cube
					{
						if( !( dx == 0 && dy == 0 && dz == 0 ))
						{
							if( RandomNum( 0, 1 ) == 1 )
							{
								chain = true;
							}
							Effects->TempEffect( c, tempItem, 17, 0, 1, 0 );
						}
					}
				}
			}
		}
	}
	else
	{
		// Only affect character associated with item
		if( !c->IsGM() && !c->IsInvulnerable() && ( c->IsNpc() || IsOnline(( *c ))))
		{
			damage = Combat->ApplyDefenseModifiers( HEAT, c, c, ALCHEMY, 0, static_cast<SI32>( damage ), true );
			[[maybe_unused]] bool retVal = c->Damage( static_cast<SI16>( damage ), HEAT, c, true );
		}
	}
	nItem->Delete();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::HandleMakeItemEffect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies maker's mark to crafted items and plays sound FX
//o------------------------------------------------------------------------------------------------o
void cEffects::HandleMakeItemEffect( CTEffect *tMake )
{
	if( tMake == nullptr )
		return;

	CChar *src			= CalcCharObjFromSer( tMake->Source() );
	UI16 iMaking		= tMake->More2();
	CreateEntry_st *toMake = Skills->FindItem( iMaking );
	if( toMake == nullptr )
		return;

	CSocket *sock = src->GetSocket();
	std::string addItem = toMake->addItem;
	UI16 amount = 1;
	auto csecs = oldstrutil::sections( addItem, "," );
	if( csecs.size() > 1 )
	{
		amount	= oldstrutil::value<UI16>( oldstrutil::extractSection( addItem, ",", 1, 1 ));
		addItem	= oldstrutil::extractSection( addItem, ",", 0, 0 );
	}

	UI16 iColour = 0;
	if( ValidateObject( src ))
	{
		// Get colour of the resource targeted by player (if any), so it can be applied to the item being made
		TAGMAPOBJECT tempTagObj = src->GetTempTag( "craftItemColor" );
		if( tempTagObj.m_ObjectType == TAGMAP_TYPE_INT && tempTagObj.m_IntValue > 0 )
		{
			iColour = static_cast<UI16>( tempTagObj.m_IntValue );
		}
	}

	// Create the actual item
	CItem *targItem = Items->CreateScriptItem( sock, src, addItem, amount, OT_ITEM, true, iColour );
	for( size_t skCounter = 0; skCounter < toMake->skillReqs.size(); ++skCounter )
	{
		src->SkillUsed( false, toMake->skillReqs[skCounter].skillNumber );
	}
	if( targItem == nullptr )
	{
		Console.Error( oldstrutil::format( "CSkills::MakeItem() bad script item # %s, made by player 0x%X", addItem.c_str(), src->GetSerial() ));
		return;
	}
	else
	{
		// targItem->SetName2( targItem->GetName().c_str() ); // Why was this here?
		SI32 rank = Skills->CalcRankAvg( src, ( *toMake ));
		SI32 maxrank = toMake->maxRank;
		Skills->ApplyRank( sock, targItem, static_cast<UI08>( rank ), static_cast<UI08>( maxrank ));

		// if we're not a GameMaster, see if we should store our creator
		if( !src->IsGM() && !toMake->skillReqs.empty() )
		{
			// Store serial of crafter
			targItem->SetCreator( src->GetSerial() );

			// Store skill number that item was crafted with
			targItem->SetMadeWith( toMake->skillReqs[0].skillNumber + 1 );

			// Set Makers Mark on item if primary skill used to craft item is at 100.0 or higher
			SI32 primarySkill = src->GetSkill( toMake->skillReqs[0].skillNumber );
			if( primarySkill >= 1000 )
			{
				targItem->SetMakersMark( true );
			}
		}
		else
		{
			// Crafted by a GM, don't apply maker's marks or give any additional information
			targItem->SetCreator( INVALIDSERIAL );
			targItem->SetMadeWith( 0 );
		}

		// Store the entry number from create DFNs that item was made from 
		targItem->EntryMadeFrom( iMaking );
	}

	// Make sure it's movable
	targItem->SetMovable( 1 );
	if( toMake->soundPlayed )
	{
		PlaySound( sock, toMake->soundPlayed, true );
	}

	if( FindItemOwner( targItem ) == src )
	{
		sock->SysMessage( 985 ); // You create the item and place it in your backpack.
	}

	// Trigger onMakeItem() JS event for character who crafted the item
	std::vector<UI16> scriptTriggers = src->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			toExecute->OnMakeItem( sock, src, targItem, iMaking );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::CheckTempeffects()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks timer-controlled temporary effects
//o------------------------------------------------------------------------------------------------o
auto cEffects::CheckTempeffects() -> void
{
	CItem *i = nullptr;
	CChar *s = nullptr, *src = nullptr;
	CSocket *tSock = nullptr;
	CBaseObject *myObj = nullptr;

	const UI32 j = cwmWorldState->GetUICurrentTime();
	std::vector<CTEffect*> removeEffects;
	auto collection = cwmWorldState->tempEffects.collection();
	for( auto &Effect : collection )
	{
		if( Effect == nullptr )
		{
			//removeEffects.push_back( Effect );
			continue;
		}
		else if( Effect->Destination() == INVALIDSERIAL )
		{
			removeEffects.push_back( Effect );
			continue;
		}
		if( Effect->ExpireTime() > j )
			continue;

		if( Effect->Destination() < BASEITEMSERIAL )
		{
			s = CalcCharObjFromSer( Effect->Destination() );
			if( !ValidateObject( s ))
			{
				removeEffects.push_back( Effect );
				//cwmWorldState->tempEffects.Remove( Effect, true );
				continue;
			}
			tSock = s->GetSocket();
		}
		bool equipCheckNeeded = false;
		bool validChar = ValidateObject( s );
		switch( Effect->Number() )
		{
			case 1: // Paralysis / Paralysis Field Spells
				if( validChar && s->IsFrozen() )
				{
					s->SetFrozen( false );
					if( tSock != nullptr )
					{
						// Don't show system message if we don't want it to
						if( Effect->More1() == 0 )
						{
							tSock->SysMessage( 700 ); // You are no longer frozen.
						}
						s->Update();
					}
				}
				break;
			case 2: // Nightsight Potion (JS) and Spell (code)
				if( validChar )
				{
					s->SetFixedLight( 255 );
					DoLight( tSock, cwmWorldState->ServerData()->WorldLightCurrentLevel() );
				}
				break;
			case 3: // Clumsy spell (JS)
				if( validChar )
				{
					s->IncDexterity2( Effect->More1() );
					equipCheckNeeded = true;
				}
				break;
			case 4: // Feeblemind spell (JS)
				if( validChar )
				{
					s->IncIntelligence2( Effect->More1() );
					equipCheckNeeded = true;
				}
				break;
			case 5: // Weaken Spell
				if( validChar )
				{
					s->IncStrength2( Effect->More1() );
					equipCheckNeeded = true;
				}
				break;
			case 6: // Agility Potion (JS) and Spell (code)
				if( validChar )
				{
					s->IncDexterity2( -Effect->More1() );
					s->SetStamina( std::min( s->GetStamina(), s->GetMaxStam() ));
					equipCheckNeeded = true;
				}
				break;
			case 7: // Cunning Spell
				if( validChar )
				{
					s->IncIntelligence2( -Effect->More1() );
					s->SetMana( std::min( s->GetMana(), s->GetMaxMana() ));
					equipCheckNeeded = true;
				}
				break;
			case 8: // Strength Potion (JS) and Spell (code)
				if( validChar )
				{
					s->IncStrength2( -Effect->More1() );
					s->SetHP( std::min( s->GetHP(), static_cast<SI16>( s->GetMaxHP() )));
					equipCheckNeeded = true;
				}
				break;
			case 9:	// Grind potion (also used for necro stuff)
				if( validChar )
				{
					switch( Effect->More1() )
					{
						case 0:
							if( Effect->More2() != 0 )
							{
								s->TextMessage( nullptr, 1270, EMOTE, 1, s->GetName().c_str() ); // *%s continues grinding*
							}
							PlaySound( s, 0x0242 );
							break;
						default: 
							break;
					}
				}
				break;
			case 11: // Bless Spell
				if( validChar )
				{
					s->IncStrength2( -Effect->More1() );
					s->SetHP( std::min( s->GetHP(), static_cast<SI16>( s->GetMaxHP() )));
					s->IncDexterity2( -Effect->More2() );
					s->SetStamina( std::min( s->GetStamina(), s->GetMaxStam() ));
					s->IncIntelligence2( -Effect->More3() );
					s->SetMana( std::min( s->GetMana(), s->GetMaxMana() ));
					equipCheckNeeded = true;
				}
				break;
			case 12: // Curse Spell
				equipCheckNeeded = false;
				if( validChar )
				{
					s->IncStrength2( Effect->More1() );
					s->IncDexterity2( Effect->More2() );
					s->IncIntelligence2( Effect->More3() );
					equipCheckNeeded = true;
				}
				break;
			case 15: // Reactive Armor Spell
				if( validChar )
				{
					s->SetReactiveArmour( false );
				}
				break;
			case 16: // Explosion potion messages (JS)
				src = CalcCharObjFromSer( Effect->Source() );
				if( src->GetTimer( tCHAR_ANTISPAM ) < cwmWorldState->GetUICurrentTime() )
				{
					src->SetTimer( tCHAR_ANTISPAM, BuildTimeValue( 1 ));
					std::string mTemp = oldstrutil::number( Effect->More3() );
					if( tSock )
					{
						tSock->SysMessage( mTemp.c_str() );
					}
				}
				break;
			case 17: // Explosion effect (JS and code)
				src = CalcCharObjFromSer( Effect->Source() );
				ExplodeItem( src->GetSocket(), static_cast<CItem *>( Effect->ObjPtr() )); //explode this item
				break;
			case 18: // Polymorph Spell
				if( validChar )
				{
					s->SetId( s->GetOrgId() );
					s->IsPolymorphed( false );
				}
				break;
			case 19: // Incognito Spell
				if( validChar )
				{
					s->SetId( s->GetOrgId() );

					// ------ NAME -----
					s->SetName( s->GetOrgName() );

					i = s->GetItemAtLayer( IL_HAIR );
					if( ValidateObject( i ))
					{
						i->SetColour( s->GetHairColour() );
						i->SetId( s->GetHairStyle() );
					}
					i = s->GetItemAtLayer( IL_FACIALHAIR );
					if( ValidateObject( i ) && s->GetId( 2 ) == 0x90 )
					{
						i->SetVisible( VT_VISIBLE );
						i->SetColour( s->GetBeardColour() );
						i->SetId( s->GetBeardStyle() );
					}
					if( tSock )
					{
						s->SendWornItems( tSock );
					}
					s->IsIncognito( false );
				}
				break;
			case 21: // Protection Spell
				if( validChar )
				{
					UI16 toDrop;
					toDrop = Effect->More1();
					if(( s->GetBaseSkill( PARRYING ) - toDrop ) < 0 )
					{
						s->SetBaseSkill( 0, PARRYING );
					}
					else
					{
						s->SetBaseSkill( s->GetBaseSkill( PARRYING ) - toDrop, PARRYING );
					}
					equipCheckNeeded = true;
				}
				break;
			case 25: // Temporarily set item as disabled
				Effect->ObjPtr()->SetDisabled( false );
				break;
			case 26: // Disallow immediately using another potion (JS)
				if( validChar )
				{
					s->SetUsingPotion( false );
				}
				break;
			case 27: // Explosion Spell
				src = CalcCharObjFromSer( Effect->Source() );
				if( ValidateObject( src ) && ValidateObject( s ))
				{
					Magic->PlaySound( src, 43 );
					Magic->DoMoveEffect( 43, s, src );
					Magic->DoStaticEffect( s, 43 );
					Magic->MagicDamage( s, Effect->More1(), src, HEAT );
					equipCheckNeeded = true;
				}
				break;
			case 28: // Magic Arrow Spell
				src = CalcCharObjFromSer( Effect->Source() );
				if( ValidateObject( src ) && ValidateObject( s ))
				{
					Magic->PlaySound( src, 5 );
					Magic->DoMoveEffect( 5, s, src );
					//  Caster.MovingParticles(d, 0x36E4, 5, 0, false, false, 3006, 0, 0);
					Magic->DoStaticEffect( s, 5 );
					Magic->MagicDamage( s, Effect->More1(), src, HEAT );
					equipCheckNeeded = true;
				}
				break;
			case 29: // Harm Spell
				src = CalcCharObjFromSer( Effect->Source() );
				if( ValidateObject( src ) && ValidateObject( s ))
				{
					Magic->PlaySound( src, 12 );
					Magic->DoMoveEffect( 12, s, src );
					Magic->DoStaticEffect( s, 12 );
					Magic->MagicDamage( s, Effect->More1(), src, COLD );
					equipCheckNeeded = true;
				}
				break;
			case 30: // Fireball Spell
				src = CalcCharObjFromSer( Effect->Source() );
				if( ValidateObject( src ) && ValidateObject( s ))
				{
					Magic->PlaySound( src, 18 );
					Magic->DoMoveEffect( 18, s, src );
					Magic->DoStaticEffect( s, 18 );
					Magic->MagicDamage( s, Effect->More1(), src, HEAT );
					equipCheckNeeded = true;
				}
				break;
			case 31: // Lightning Spell
				src = CalcCharObjFromSer( Effect->Source() );
				if( ValidateObject( src ) && ValidateObject( s ))
				{
					Effects->Bolteffect( s );
					Magic->PlaySound( src, 30 );
					Magic->DoMoveEffect( 30, s, src );
					Magic->DoStaticEffect( s, 30 );
					Magic->MagicDamage( s, Effect->More1(), src, LIGHTNING );
					equipCheckNeeded = true;
				}
				break;
			case 32: // Mind Blast
				src = CalcCharObjFromSer( Effect->Source() );
				if( ValidateObject( src ) && ValidateObject( s ))
				{
					Magic->PlaySound( src, 37 );
					Magic->DoMoveEffect( 37, s, src );
					Magic->DoStaticEffect( s, 37 );
					Magic->MagicDamage( s, Effect->More1(), src, COLD );
					equipCheckNeeded = true;
				}
				break;
			case 33: // Energy Bolt
				src = CalcCharObjFromSer( Effect->Source() );
				if( ValidateObject( src ) && ValidateObject( s ))
				{
					Magic->PlaySound( s, 42 );
					Magic->DoMoveEffect( 42, s, src );
					Magic->DoStaticEffect( s, 42 );
					Magic->MagicDamage( s, Effect->More1(), src, LIGHTNING );
					equipCheckNeeded = true;
				}
				break;
			case 34: // Chain Lightning
				src = CalcCharObjFromSer( Effect->Source() );
				if( ValidateObject( src ) && ValidateObject( s ))
				{
					Effects->Bolteffect( s );
					Magic->PlaySound( s, 49 );
					Magic->DoMoveEffect( 49, s, src );
					Magic->DoStaticEffect( s, 49 );
					Magic->MagicDamage( s, Effect->More1(), src, LIGHTNING );
					equipCheckNeeded = true;
				}
				break;
			case 35: // Flame Strike
				src = CalcCharObjFromSer( Effect->Source() );
				if( ValidateObject( src ) && ValidateObject( s ))
				{
					Magic->PlaySound( s, 51 );
					Magic->DoMoveEffect( 51, s, src );
					Magic->DoStaticEffect( s, 51 );
					Magic->MagicDamage( s, Effect->More1(), src, HEAT );
					equipCheckNeeded = true;
				}
				break;
			case 36: // Meteor Swarm
				src = CalcCharObjFromSer( Effect->Source() );
				if( ValidateObject( src ) && ValidateObject( s ))
				{
					Magic->PlaySound( src, 55 );
					if( s != src )
					{
						Magic->DoMoveEffect( 55, s, src );
					}
					Magic->DoStaticEffect( s, 55 );
					//Effects->PlaySound( target, 0x160 );
					//Effects->PlayMovingAnimation( caster, target, 0x36D5, 0x07, 0x00, 0x01 );
					Magic->MagicDamage( s, Effect->More1(), src, HEAT );
					equipCheckNeeded = true;
				}
				break;
			case 40: // Used by JS timers
			{
				// Default/Global script ID
				UI16 scpNum	= 0xFFFF;

				// Get script associated with effect, if any
				cScript *tScript = JSMapping->GetScript( Effect->AssocScript() );

				// items have serials of 0x40000000 and above, and we already know it's not INVALIDSERIAL
				if( Effect->Source() >= BASEITEMSERIAL )
				{
					myObj = CalcItemObjFromSer( Effect->Source() );
					equipCheckNeeded = false;
				}
				else
				{
					myObj = CalcCharObjFromSer( Effect->Source() );
					equipCheckNeeded = true;
				}

				// Check that object effect was running on still exists
				if( ValidateObject( myObj ))
				{
					// No associated script, so it must be another callback variety
					if( tScript == nullptr )
					{
						if( Effect->More2() != 0xFFFF ) // A scriptId other than default one was found
						{
							scpNum = Effect->More2();
							tScript = JSMapping->GetScript( scpNum );
						}
					}
					
					// Make sure to check for a specific script via envoke system when the previous checks ended in the global script.
					if(( tScript == nullptr || scpNum == 0 ) && Effect->Source() >= BASEITEMSERIAL )
					{
						if( JSMapping->GetEnvokeByType()->Check( static_cast<UI16>(( static_cast<CItem *>( myObj ))->GetType() )))
						{
							scpNum	= JSMapping->GetEnvokeByType()->GetScript( static_cast<UI16>(( static_cast<CItem *>( myObj ))->GetType() ));
							tScript = JSMapping->GetScript( scpNum );
						}
						else if( JSMapping->GetEnvokeById()->Check( myObj->GetId() ))
						{
							scpNum	= JSMapping->GetEnvokeById()->GetScript( myObj->GetId() );
							tScript	= JSMapping->GetScript( scpNum );
						}
					}
					
					// Callback to onTimer event in script
					if( tScript )
					{
						tScript->OnTimer( myObj, static_cast<UI16>( Effect->More1() ));
					}
				}
				break;
			}
			case 41: // Start of item crafting
				HandleMakeItemEffect( Effect );
				break;
			case 42: // End of item crafting
				src = CalcCharObjFromSer( Effect->Source() );
				PlaySound( src, Effect->More2() );
				break;
			case 43: // Turn a wooly sheep into a sheared sheep
				src = CalcCharObjFromSer( Effect->Source() );
				if( !ValidateObject( src ))	// char doesn't exist!
				{
					break;
				}
				else if( src->GetId() == 0xCF )
				{
					break;
				}

				src->SetId( 0xCF ); // Thats all we need to do
				break;
			case 50: // Unlock a magically locked object
			{
				myObj = CalcItemObjFromSer( Effect->Destination() );
				if( ValidateObject( myObj ))
				{
					auto lockedItem = static_cast<CItem *>( myObj );

					// Only continue if door is actually still locked
					auto lockType = lockedItem->GetType();
					if( lockType == IT_LOCKEDDOOR || lockType == IT_LOCKEDCONTAINER || lockType == IT_LOCKEDSPAWNCONT )
					{
						switch( lockType )
						{
							case IT_LOCKEDDOOR:			lockedItem->SetType( IT_DOOR );			break;
							case IT_LOCKEDCONTAINER:	lockedItem->SetType( IT_CONTAINER );	break;
							case IT_LOCKEDSPAWNCONT:	lockedItem->SetType( IT_SPAWNCONT );	break;
							default: break;
						}

						lockedItem->RemoveFromSight();
						lockedItem->Update();

						if( Magic->spells[23].Effect() != INVALIDID )
						{
							Effects->PlaySound( lockedItem, Magic->spells[23].Effect(), true );
						}

						CMagicStat temp = Magic->spells[23].StaticEffect();
						if( temp.Effect() != INVALIDID )
						{
							auto iCont = lockedItem->GetCont();
							if( ValidateObject( iCont ) && iCont->CanBeObjType( OT_CHAR ))
							{
								// Play unlock FX on character holding object
								Effects->PlayStaticAnimation( iCont, temp.Effect(), temp.Speed(), temp.Loop() );
							}
							else
							{
								// Play unlock FX directly on object
								Effects->PlayStaticAnimation( lockedItem, temp.Effect(), temp.Speed(), temp.Loop() );
							}
						}
					}
				}
				break;
			}
			default:
				Console.Error( oldstrutil::format( " Fallout of switch statement without default (%i). checktempeffects()", Effect->Number() ));
				break;
		}
		if( validChar && equipCheckNeeded )
		{
			Items->CheckEquipment( s ); // checks equipments for stat requirements
		}
		removeEffects.push_back( Effect );
	}
	std::for_each( removeEffects.begin(), removeEffects.end(), []( CTEffect *effect )
	{
		cwmWorldState->tempEffects.Remove( effect, true );
	});
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	ReverseEffect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Reverse a temp effect
//o------------------------------------------------------------------------------------------------o
void ReverseEffect( CTEffect *Effect )
{
	CChar *s = CalcCharObjFromSer( Effect->Destination() );
	if( ValidateObject( s ))
	{
		switch( Effect->Number() )
		{
			case 1: // Paralysis Spell
				s->SetFrozen( false );
				break;
			case 2: // Nightsight Potion (JS) and Spell (code)
				s->SetFixedLight( 255 );
				break;
			case 3:		s->IncDexterity2( Effect->More1() );		break; // Clumsy Spell (JS)
			case 4:		s->IncIntelligence2( Effect->More1() );		break; // Feeblemind Spell (JS)
			case 5:		s->IncStrength2( Effect->More1() );			break; // Weaken Spell
			case 6:		s->IncDexterity2( -Effect->More1() );		break; // Agility Potion (JS) and Spell (code)
			case 7:		s->IncIntelligence2( -Effect->More1() );	break; // Cunning Spell
			case 8:		s->IncStrength2( -Effect->More1() );		break; // Strength Potion (JS) and Spell (code)
			case 11: // Bless Spell
				s->IncStrength2( -Effect->More1() );
				s->IncDexterity2( -Effect->More2() );
				s->IncIntelligence2( -Effect->More3() );
				break;
			case 12: // Curse Spell
				s->IncStrength2( Effect->More1() );
				s->IncDexterity2( Effect->More2() );
				s->IncIntelligence2( Effect->More3() );
				break;
			case 18: // Polymorph Spell
				s->SetId( s->GetOrgId() );
				s->IsPolymorphed( false );
				break;
			case 19: // Incognito Spell
				CItem *j;

				// ------ SEX ------
				s->SetId( s->GetOrgId() );
				s->SetName( s->GetOrgName() );
				j = s->GetItemAtLayer( IL_HAIR );
				if( ValidateObject( j ))
				{
					j->SetColour( s->GetHairColour() );
					j->SetId( s->GetHairStyle() );
				}
				j = s->GetItemAtLayer( IL_FACIALHAIR );
				if( ValidateObject( j ) && s->GetId( 2 ) == 0x90 )
				{
					j->SetColour( s->GetBeardColour() );
					j->SetId( s->GetBeardStyle() );
				}
				// only refresh once
				CSocket *tSock;
				tSock = s->GetSocket();
				s->SendWornItems( tSock );
				s->IsIncognito( false );
				break;
			case 21: // Protection Spell
				SI32 toDrop;
				toDrop = Effect->More1();
				if(( s->GetBaseSkill( PARRYING ) - toDrop ) < 0 )
				{
					s->SetBaseSkill( 0, PARRYING );
				}
				else
				{
					s->SetBaseSkill( s->GetBaseSkill( PARRYING ) - toDrop, PARRYING );
				}
				break;
			case 26: // Re-enable the usage of potions
				s->SetUsingPotion( false );
				break;
			default:
				Console.Error( " Fallout of switch statement without default. uox3.cpp, reverseEffect()");
				break;
		}
	}
	Items->CheckEquipment( s );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::TempEffect()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a temp effect to a character
//o------------------------------------------------------------------------------------------------o
void cEffects::TempEffect( CChar *source, CChar *dest, UI08 num, UI16 more1, UI16 more2, UI16 more3, [[maybe_unused]] CItem *targItemPtr )
{
	//if( !ValidateObject( source ) || !ValidateObject( dest ))
	if( !ValidateObject( dest ))
		return;

	bool spellResisted = false;
	CTEffect *toAdd	= new CTEffect;
	SERIAL sourceSerial	= 0;
	if( source != nullptr )
	{
		sourceSerial = source->GetSerial();
	}
	SERIAL targSer	= dest->GetSerial();
	toAdd->Source( sourceSerial );
	toAdd->Destination( targSer );
	std::vector<CTEffect *> removeEffect;
	for( const auto &Effect : cwmWorldState->tempEffects.collection() )
	{
		if( Effect->Destination() == targSer )
		{
			if( Effect->Number() == num )
			{
				switch( Effect->Number() )
				{
					case 3: // Clumsy Spell (JS)
					case 4: // Feeblemind Spell (JS)
					case 5: // Weaken Spell
					case 6: // Agility Potion (JS) and Spell (code)
					case 7: // Cunning Spell
					case 8: // Strength Potion (JS) and Spell (code)
					case 11: // Bless Spell
					case 12: // Curse Spell
					case 18: // Polymorph Spell
					case 19: // Incognito Spell
					case 21: // Protection Spell
						ReverseEffect( Effect );
						removeEffect.push_back( Effect );
						break;
					default:
						break;
				}
			}
		}
	}
	std::for_each( removeEffect.begin(), removeEffect.end(), []( CTEffect *effect )
	{
		cwmWorldState->tempEffects.Remove( effect, true );
	});
	CSocket *tSock = dest->GetSocket();
	toAdd->Number( num );
	switch( num )
	{
		case 1: // Paralyse Spell
		{
			dest->SetFrozen( true );
			R32 effectDuration = 0;
			if( source == nullptr )
			{
				effectDuration = static_cast<R32>( more2 );
				if( more3 != 0 )
				{
					// Divide duration by more3 value
					effectDuration /= static_cast<R32>( more3 );
				}

				// Keep a reference to more1, if different than 0
				if( more1 != 0 )
				{
					toAdd->More1( more1 );
				}
			}
			else
			{
				effectDuration = static_cast<R32>( source->GetSkill( MAGERY )) / 100.0f;
			}
			toAdd->ExpireTime( BuildTimeValue( effectDuration ));
			toAdd->Dispellable( true );
			break;
		}
		case 2: // Nightsight Potion (JS) and Spell (code)
		{
			SI16 worldbrightlevel;
			worldbrightlevel = cwmWorldState->ServerData()->WorldLightBrightLevel();
			dest->SetFixedLight( static_cast<UI08>( worldbrightlevel ));
			DoLight( tSock, static_cast<SI08>( worldbrightlevel ));

			R32 effectDuration = 0;
			if( source == nullptr )
			{
				effectDuration = static_cast<R32>( more2 );
			}
			else
			{
				effectDuration = static_cast<R32>( source->GetSkill( MAGERY )) / 2.0f;
			}
			toAdd->ExpireTime( BuildTimeValue( effectDuration ));
			toAdd->Dispellable( true );
			break;
		}
		case 3: // Clumsy Spell (JS)
		{
			if( dest->GetDexterity() < more1 )
			{
				more1 = dest->GetDexterity();
			}
			dest->IncDexterity2( -more1 );
			dest->SetStamina( std::min( dest->GetStamina(), dest->GetMaxStam() ));

			R32 effectDuration = 0;
			if( source == nullptr )
			{
				// Use duration provided with effect call
				effectDuration = static_cast<R32>( more2 );
				
				// Halve effect-duration on resist
				spellResisted = Magic->CheckResist( more3, dest, 1 );
			}
			else
			{
				effectDuration = static_cast<R32>( source->GetSkill( MAGERY ) / 10.0f );

				//Halve effect-timer on resist
				spellResisted = Magic->CheckResist( source, dest, 1 );
			}

			if( spellResisted )
			{
				toAdd->ExpireTime( BuildTimeValue( effectDuration / 2.0f ));
			}
			else
			{
				toAdd->ExpireTime( BuildTimeValue( effectDuration ));
			}

			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		}
		case 4: // Feeblemind Spell (JS)
		{
			if( dest->GetIntelligence() < more1 )
			{
				more1 = dest->GetIntelligence();
			}
			dest->IncIntelligence2( -more1 );
			dest->SetMana( std::min( dest->GetMana(), dest->GetMaxMana() ));

			R32 effectDuration = 0;
			if( source == nullptr )
			{
				// Use duration provided with effect call
				effectDuration = static_cast<R32>( more2 );

				// Halve effect-duration on resist
				spellResisted = Magic->CheckResist( more3, dest, 3 );
			}
			else
			{
				effectDuration = static_cast<R32>( source->GetSkill( MAGERY ) / 10.0f );

				//Halve effect-timer on resist
				spellResisted = Magic->CheckResist( source, dest, 1 );
			}

			if( spellResisted )
			{
				toAdd->ExpireTime( BuildTimeValue( effectDuration / 2.0f ));
			}
			else
			{
				toAdd->ExpireTime( BuildTimeValue( effectDuration ));
			}

			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		}
		case 5: // Weaken Spell
		{
			if( dest->GetStrength() < more1 )
			{
				more1 = dest->GetStrength();
			}
			dest->IncStrength2( -more1 );
			dest->SetHP( std::min( dest->GetHP(), static_cast<SI16>( dest->GetMaxHP() )));

			R32 effectDuration = 0;
			if( source == nullptr )
			{
				// Use duration provided with effect call
				effectDuration = static_cast<R32>( more2 );

				// Halve effect-duration on resist
				spellResisted = Magic->CheckResist( more3, dest, 1 );
			}
			else
			{
				effectDuration = static_cast<R32>( source->GetSkill( MAGERY ) / 10.0f );

				//Halve effect-timer on resist
				spellResisted = Magic->CheckResist( source, dest, 1 );
			}

			//Halve effect-timer on resist
			if( spellResisted )
			{
				toAdd->ExpireTime( BuildTimeValue( effectDuration / 2.0f ));
			}
			else
			{
				toAdd->ExpireTime( BuildTimeValue( effectDuration ));
			}
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		}
		case 6: // Agility Potion (JS) and Spell (code)
		{
			dest->IncDexterity( more1 );
			if( source == nullptr )
			{
				// Use duration provided with effect call
				toAdd->ExpireTime( BuildTimeValue( static_cast<R32>( more2 )));
			}
			else
			{
				toAdd->ExpireTime( BuildTimeValue( static_cast<R32>( source->GetSkill( MAGERY )) / 10.0f ));
			}
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		}
		case 7: // Cunning Spell
			dest->IncIntelligence2( more1 );
			if( source == nullptr )
			{
				// Use duration provided with effect call
				toAdd->ExpireTime( BuildTimeValue( static_cast<R32>(more2) ));
			}
			else
			{
				toAdd->ExpireTime( BuildTimeValue( static_cast<R32>( source->GetSkill( MAGERY )) / 10.0f ));
			}
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		case 8: // Strength Potion (JS) and Spell (code)
			dest->IncStrength2( more1 );
			if( source == nullptr )
			{
				// Use duration provided with effect call
				toAdd->ExpireTime( BuildTimeValue( static_cast<R32>( more2 )));
			}
			else
			{
				toAdd->ExpireTime( BuildTimeValue( static_cast<R32>( source->GetSkill( MAGERY )) / 10.0f ));
			}
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		case 9: // Grind Necro Reagent
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>( more2 )));
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			toAdd->Dispellable( false );
			break;
		case 10: // ???
			toAdd->ExpireTime( BuildTimeValue( 12.0f ));
			toAdd->Dispellable( false );
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			break;
		case 11: // Bless Spell
			if( source == nullptr )
			{
				// No source! Effect gotten from equipping an item?
				// Apply same bonus to all stats
				dest->IncStrength2( more1 );
				dest->IncDexterity2( more1 );
				dest->IncIntelligence2( more1 );
				toAdd->ExpireTime( BuildTimeValue( more2 ));
				toAdd->More1( more1 );
				toAdd->More2( more1 );
				toAdd->More3( more1 );
			}
			else
			{
				// Effect gotten from a spell cast by another character
				dest->IncStrength2( more1 );
				dest->IncDexterity2( more2 );
				dest->IncIntelligence2( more3 );
				toAdd->ExpireTime( BuildTimeValue( static_cast<R32>( source->GetSkill( MAGERY )) / 10.0f ));
				toAdd->More1( more1 );
				toAdd->More2( more2 );
				toAdd->More3( more3 );
			}
			toAdd->Dispellable( true );
			break;
		case 12: // Curse Spell
		{
			bool spellResisted = false;
			R32 effectDuration = 0;
			if( source == nullptr )
			{
				auto effectStrength = more1;
				if( dest->GetStrength() < more1 )
				{
					effectStrength = dest->GetStrength();
				}
				dest->IncStrength2( -effectStrength );
				if( dest->GetDexterity() < more1 )
				{
					effectStrength = dest->GetDexterity();
				}
				dest->IncDexterity2( -effectStrength );
				if( dest->GetIntelligence() < more1 )
				{
					effectStrength = dest->GetIntelligence();
				}
				dest->IncIntelligence2( -effectStrength );
				toAdd->More1( more1 );
				toAdd->More2( more1 );
				toAdd->More3( more1 );

				// Use duration provided with effect call
				effectDuration = static_cast<R32>( more2 );

				// Halve effect-duration on resist
				spellResisted = Magic->CheckResist( more3, dest, 4 );
			}
			else
			{
				if( dest->GetStrength() < more1 )
				{
					more1 = dest->GetStrength();
				}
				if( dest->GetDexterity() < more2 )
				{
					more2 = dest->GetDexterity();
				}
				if( dest->GetIntelligence() < more3 )
				{
					more3 = dest->GetIntelligence();
				}
				dest->IncStrength2( -more1 );
				dest->IncDexterity2( -more2 );
				dest->IncIntelligence2( -more3 );
				toAdd->More1( more1 );
				toAdd->More2( more2 );
				toAdd->More3( more3 );

				effectDuration = static_cast<R32>( source->GetSkill( MAGERY )) / 10.0f;

				//Halve effect-timer on resist
				spellResisted = Magic->CheckResist( source, dest, 4 );
			}

			if( spellResisted )
			{
				toAdd->ExpireTime( BuildTimeValue( effectDuration / 2.0f ));
			}
			else
			{
				toAdd->ExpireTime( BuildTimeValue( effectDuration ));
			}

			toAdd->Dispellable( true );
			break;
		}
		case 15: // Reactive Armor Spell
			if( source != nullptr )
			{
				toAdd->ExpireTime( BuildTimeValue( static_cast<R32>( source->GetSkill( MAGERY )) / 10.0f ));
			}
			else
			{
				toAdd->ExpireTime( BuildTimeValue( 6.0f ));
			}
			toAdd->Dispellable( true );
			break;
		case 16: // Explosion potion messages (JS)
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>( more2 )));
			toAdd->More1( more1 ); //item/potion
			toAdd->More2( more2 ); //seconds
			toAdd->More3( more3 ); //countdown#
			toAdd->Dispellable( false );
			break;
		case 18: // Polymorph Spell
			toAdd->ExpireTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_POLYMORPH ));
			toAdd->Dispellable( false );

			UI16 k;
			// Grey flag when polymorphed
			dest->SetTimer( tCHAR_CRIMFLAG, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_POLYMORPH ));
			if( dest->IsOnHorse() )
			{
				DismountCreature( dest );
			}
			else if( dest->IsFlying() )
			{
				dest->ToggleFlying();
			}
			k = ( more1<<8 ) + more2;

			if( k <= 0x03e2 ) // body-values >0x3e1 crash the client
			{
				dest->SetId( k );
			}
			break;
		case 19: // Incognito Spell
			toAdd->ExpireTime( BuildTimeValue( 90.0f )); // 90 seconds
			toAdd->Dispellable( false );
			break;
		case 21: // Protection Spell
		{
			R32 effectDuration = 0;
			if( source == nullptr )
			{
				effectDuration = static_cast<R32>( more2 );
			}
			else
			{
				effectDuration = 120.0f;
			}
			toAdd->ExpireTime( BuildTimeValue( effectDuration ));
			toAdd->Dispellable( true );
			toAdd->More1( more1 );
			dest->SetBaseSkill( dest->GetBaseSkill( PARRYING ) + more1, PARRYING );
			break;
		}
		case 25: // Temporarily set item as disabled
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>( more1 )));
			toAdd->ObjPtr( dest );
			dest->SetDisabled( true );
			toAdd->More2( 1 );
			break;
		case 26: // Temporarily disable the usage of potions
			toAdd->ExpireTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_POTION ));
			dest->SetUsingPotion( true );
			break;
		case 27: // Explosion Spell
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>( Magic->spells[43].DamageDelay() )));
			toAdd->More1( more1 );
			break;
		case 28: // Magic Arrow Spell
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>(Magic->spells[5].DamageDelay() )));
			toAdd->More1( more1 );
			break;
		case 29: // Harm Spell
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>(Magic->spells[12].DamageDelay() )));
			toAdd->More1( more1 );
			break;
		case 30: // Fireball Spell
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>(Magic->spells[18].DamageDelay() )));
			toAdd->More1( more1 );
			break;
		case 31: // Lightning Spell
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>(Magic->spells[30].DamageDelay() )));
			toAdd->More1( more1 );
			break;
		case 32: // Mind Blast Spell
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>(Magic->spells[37].DamageDelay() )));
			toAdd->More1( more1 );
			break;
		case 33: // Energy Bolt Spell
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>(Magic->spells[42].DamageDelay() )));
			toAdd->More1( more1 );
			break;
		case 34: // Chain Lightning Spell
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>(Magic->spells[49].DamageDelay() )));
			toAdd->More1( more1 );
			break;
		case 35: // Flamestrike Spell
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>(Magic->spells[51].DamageDelay() )));
			toAdd->More1( more1 );
			break;
		case 36: // Meteor Swarm
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>(Magic->spells[55].DamageDelay() )));
			toAdd->More1( more1 );
			break;
		case 40: // Used by JS timers
			toAdd->ExpireTime( BuildTimeValue(( static_cast<R32>( more1 ) + static_cast<R32>( more2 ) / 1000.0f )));
			toAdd->More1( more3 );
			break;
		case 41: // creating item
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>( more1 ) / 100.0f ));
			toAdd->More2( more2 );
			break;
		case 42: // delayed sound effect for crafting
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>( more1 ) / 100.0f ));
			toAdd->More2( more2 );
			break;
		case 43: // regain wool for sheeps (JS)
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>( more1 )));
			break;
		default:
			Console.Error( oldstrutil::format( " Fallout of switch statement (%d) without default. uox3.cpp, tempeffect()", num ));
			delete toAdd;
			return;
	}
	cwmWorldState->tempEffects.Add( toAdd );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::TempEffect( CChar *source, CItem *dest, UI08 num, UI16 more1, UI16 more2, UI16 more3 )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a temp effect to an item
//o------------------------------------------------------------------------------------------------o
void cEffects::TempEffect( CChar *source, CItem *dest, UI08 num, UI16 more1, UI16 more2, [[maybe_unused]] UI16 more3 )
{
	if( !ValidateObject( dest ))
		return;

	CTEffect *toAdd = new CTEffect;

	if( ValidateObject( source ))
	{
		toAdd->Source( source->GetSerial() );
	}
	else
	{
		toAdd->Source( INVALIDSERIAL );
	}

	toAdd->Destination( dest->GetSerial() );
	toAdd->Number( num );
	switch( num )
	{
		case 10: // Crafting Potion??
			toAdd->ExpireTime( BuildTimeValue( 12 ));
			toAdd->Dispellable( false );
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			source->SkillUsed( true, ALCHEMY );
			break;
		case 13: // Timer for door (gangplank on boats?) to autoclose?
			if( !dest->IsDoorOpen() )
			{
				dest->SetDoorOpen( true );
			}

			toAdd->ExpireTime( BuildTimeValue( 10 ));
			toAdd->Dispellable( false );
			break;
		case 17: // Explosion potion (explosion)  Tauriel (explode in 4 seconds)
			toAdd->ExpireTime( BuildTimeValue( 4 ));
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			toAdd->ObjPtr( dest );
			toAdd->Dispellable( false );
			break;
		case 25: // // Temporarily set item as disabled
			toAdd->ExpireTime( BuildTimeValue( more1 ));
			toAdd->ObjPtr( dest );
			dest->SetDisabled( true );
			toAdd->More2( 0 );
			break;
		case 50: // Magically locked, expires in X seconds (more1, based on caster's Magery skill)
			toAdd->ExpireTime( BuildTimeValue( more1 ));
			toAdd->ObjPtr( dest );
			toAdd->Dispellable( false );
			break;
		default:
			Console.Error( " Fallout of switch statement without default. uox3.cpp, tempeffect2()" );
			delete toAdd;
			return;
	}
	cwmWorldState->tempEffects.Add( toAdd );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::SaveEffects()
//|	Date		-	16 March, 2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves out the TempEffects to effects.wsc
//o------------------------------------------------------------------------------------------------o
void cEffects::SaveEffects( void )
{
	std::ofstream effectDestination;
	const char blockDiscriminator[] = "\n\n---EFFECT---\n\n";
	SI32 s_t = GetClock();

	Console << "Saving Effects...   ";
	Console.TurnYellow();

	std::string filename = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "effects.wsc";
	effectDestination.open( filename.c_str() );
	if( !effectDestination )
	{
		Console.Error( oldstrutil::format( "Failed to open %s for writing", filename.c_str() ));
		return;
	}

	for( const auto &currEffect : cwmWorldState->tempEffects.collection() )
	{
		if( currEffect )
		{
			currEffect->Save( effectDestination );
			effectDestination << blockDiscriminator;
		}
	}
	effectDestination.close();

	Console << "\b\b\b\b";
	Console.PrintDone();

	SI32 e_t = GetClock();
	Console.Print( oldstrutil::format("Effects saved in %.02fsec\n", ( static_cast<R32>( e_t-s_t )) / 1000.0f ));
}

void ReadWorldTagData( std::ifstream &inStream, std::string &tag, std::string &data );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::LoadEffects()
//|	Date		-	16 March, 2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads in Effects from disk
//o------------------------------------------------------------------------------------------------o
void cEffects::LoadEffects( void )
{
	std::ifstream input;
	std::string filename = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "effects.wsc";

	input.open( filename.c_str(), std::ios_base::in );
	input.seekg( 0, std::ios::beg );

	std::string tag;
	std::string data;
	std::string UTag;

	if( input.is_open() )
	{
		CTEffect *toLoad;
		char line[1024];

		while( !input.eof() && !input.fail() )
		{
			input.getline(line, 1023);
			line[input.gcount()] = 0;
			std::string sLine( line );
			sLine = oldstrutil::trim( oldstrutil::removeTrailing( sLine, "//" ));
			auto usLine = oldstrutil::upper( sLine );
			
			if( !sLine.empty() )
			{
				if( usLine == "[EFFECT]" )
				{
					toLoad = new CTEffect;
					while( tag != "o---o" )
					{
						ReadWorldTagData( input, tag, data );
						if( tag != "o---o" )
						{
							UTag = oldstrutil::upper( tag );
							switch(( UTag.data()[0] ))
							{
								case 'A':
									if( UTag == "ASSOCSCRIPT" )
									{
										toLoad->AssocScript( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
									}
									break;
								case 'D':
									if( UTag == "DEST" )
									{
										toLoad->Destination( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
									}
									if( UTag == "DISPEL" )
									{
										toLoad->Dispellable((( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) == 0 ) ? false : true ));
									}
									break;
								case 'E':
									if( UTag == "EXPIRE" )
									{
										toLoad->ExpireTime( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )) + cwmWorldState->GetUICurrentTime() );
									}
									break;
								case 'I':
									if( UTag == "ITEMPTR" )
									{
										SERIAL objSer = static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
										if( objSer != INVALIDSERIAL )
										{
											if( objSer < BASEITEMSERIAL )
											{
												toLoad->ObjPtr( CalcCharObjFromSer( objSer ));
											}
											else
											{
												toLoad->ObjPtr( CalcItemObjFromSer( objSer ));
											}
										}
										else
										{
											toLoad->ObjPtr( nullptr );
										}
									}
									break;
								case 'M':
									if( UTag == "MORE1" )
									{
										toLoad->More1( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
									}
									if( UTag == "MORE2" )
									{
										toLoad->More2( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
									}
									if( UTag == "MORE3" )
									{
										toLoad->More3( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
									}
									break;
								case 'N':
									if( UTag == "NUMBER" )
									{
										toLoad->Number( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
									}
									break;
								case 'O':
									if( UTag == "OBJPTR" )
									{
										SERIAL objSer = static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
										if( objSer != INVALIDSERIAL )
										{
											if( objSer < BASEITEMSERIAL )
											{
												toLoad->ObjPtr( CalcCharObjFromSer( objSer ));
											}
											else
											{
												toLoad->ObjPtr( CalcItemObjFromSer( objSer ));
											}
										}
										else
										{
											toLoad->ObjPtr( nullptr );
										}
									}
									break;
								case 'S':
									if( UTag == "SOURCE" )
									{
										toLoad->Source( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
									}
									break;
								default:
									Console.Error( oldstrutil::format( "Unknown effects tag %s with contents of %s", tag.c_str(), data.c_str() ));
									break;
							}
						}
					}
					cwmWorldState->tempEffects.Add( toLoad );
					tag = "";
				}
			}
		}
		input.close();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CTEffect::Save()
//|	Date		-	March 16, 2002
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Output the effect data to the effect destination. Returns true/false
//|					indicating the success of the write operation
//o------------------------------------------------------------------------------------------------o
bool CTEffect::Save( std::ofstream &effectDestination ) const
{
	CBaseObject *getPtr = nullptr;
	const char newLine = '\n';

	effectDestination << "[EFFECT]" << newLine;

	// Hexadecimal Values
	effectDestination << std::hex;
	effectDestination << "Source=0x" << Source() << newLine;
	effectDestination << "Dest=0x" << Destination() << newLine;

	getPtr = ObjPtr();
	effectDestination << "ObjPtr=0x";
	if( ValidateObject( getPtr ))
	{
		effectDestination << getPtr->GetSerial() << newLine;
	}
	else
	{
		effectDestination << INVALIDSERIAL << newLine;
	}

	// Decimal / String Values
	effectDestination << std::dec;
	effectDestination << "Expire=" + std::to_string( ExpireTime() - cwmWorldState->GetUICurrentTime() ) + newLine;
	effectDestination << "Number=" + std::to_string( Number() ) + newLine;
	effectDestination << "More1=" + std::to_string( More1() ) + newLine;
	effectDestination << "More2=" + std::to_string( More2() ) + newLine;
	effectDestination << "More3=" + std::to_string( More3() ) + newLine;
	effectDestination << "Dispel=" + std::to_string( Dispellable() ? 1 : 0 ) + newLine;
	effectDestination << "AssocScript=" + std::to_string( AssocScript() ) + newLine;
	effectDestination << "\no---o\n\n";
	return true;
}
