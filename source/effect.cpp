#include "uox3.h"
#include "skills.h"
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void deathAction( CChar *s, CItem *x, UI08 fallDirection )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays a characters death animation and sends it to nearby players
//o-----------------------------------------------------------------------------------------------o
void cEffects::deathAction( CChar *s, CItem *x, UI08 fallDirection )
{
	CPDeathAction toSend( (*s), (*x) );
	toSend.FallDirection( fallDirection );
	SOCKLIST nearbyChars = FindNearbyPlayers( s );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		if( (*cIter)->CurrcharObj() != s )	// Death action screws up corpse display for the person who died.
			(*cIter)->Send( &toSend );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SpawnBloodEffect( UI16 bloodColour )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Spawns a blood effect item and returns the newly created object to caller
//o-----------------------------------------------------------------------------------------------o
CItem * cEffects::SpawnBloodEffect( UI08 worldNum, UI16 instanceID, UI16 bloodColour, BloodTypes bloodType )
{
	// Use default blood decay timer from ini setting
	R32 bloodDecayTimer = static_cast<R32>(cwmWorldState->ServerData()->SystemTimer( tSERVER_BLOODDECAY ));

	// Blood effects, sorted by size of effect from small to large
	std::vector<UI16> bloodIDs{ 0x1645, 0x122C, 0x122E, 0x122B, 0x122D, 0x122A, 0x122F };
	UI16 bloodEffectID = 0x122c;

	switch( bloodType )
	{
		case BLOOD_DEATH:
			// Use corpse-specific blood decay timer instead of default one
			bloodDecayTimer = static_cast<R32>( cwmWorldState->ServerData()->SystemTimer( tSERVER_BLOODDECAYCORPSE ) );

			// Randomize between large blood effects
			bloodEffectID = bloodIDs[RandomNum( static_cast<UI16>( 3 ), static_cast<UI16>( 6 ) )];
			break;
		case BLOOD_BLEED:
			// Randomize between small blood effects
			bloodEffectID = bloodIDs[RandomNum( static_cast<UI16>( 0 ), static_cast<UI16>( 3 ) )];
			break;
		case BLOOD_CRITICAL:
			// Randomize between medium to large blood effects
			bloodEffectID = bloodIDs[RandomNum( static_cast<UI16>( 2 ), static_cast<UI16>( 5 ) )];

			// More blood, so increase the time it takes to decay
			bloodDecayTimer *= 1.5;
			break;
		default:
			break;
	}
	
	// Spawn the blood effect item
	CItem *blood = Items->CreateBaseItem( worldNum, OT_ITEM, instanceID );
	if( ValidateObject( blood ) )
	{
		blood->SetID( bloodEffectID );
		blood->SetColour( bloodColour );
		blood->SetDecayable( true );
		blood->SetDecayTime( BuildTimeValue( bloodDecayTimer ));
		return blood;
	}

	return nullptr;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PlayMovingAnimation( CBaseObject *source, CBaseObject *dest, UI16 effect,
//|										UI08 speed, UI08 loop, bool explode, UI32 hue, UI32 renderMode )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends a message to client to display a moving animation from source object to target object
//o-----------------------------------------------------------------------------------------------o
void cEffects::PlayMovingAnimation( CBaseObject *source, CBaseObject *dest, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 hue, UI32 renderMode )
{	//0x0f 0x42 = arrow 0x1b 0xfe=bolt
	if( !ValidateObject( source ) || !ValidateObject( dest ) )
		return;

	CPGraphicalEffect2 toSend( 0, (*source), (*dest) );
	toSend.Model( effect );
	toSend.SourceLocation( (*source) );
	toSend.TargetLocation( (*dest) );
	toSend.Speed( speed );
	toSend.Duration( loop );
	toSend.ExplodeOnImpact( explode );
	toSend.Hue( hue );
	toSend.RenderMode( renderMode );

	//std::scoped_lock lock(Network->internallock);

	Network->pushConn();
	for( CSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( objInRange( tSock, source, DIST_SAMESCREEN ) && objInRange( tSock, dest, DIST_SAMESCREEN ) )
			tSock->Send( &toSend );
	}
	Network->popConn();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PlayMovingAnimation( CBaseObject *source, SI16 x, SI16 y, SI08 z, UI16 effect,
//|										UI08 speed, UI08 loop, bool explode, UI32 hue, UI32 renderMode )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends a message to client to display a moving animation from source object to target location
//o-----------------------------------------------------------------------------------------------o
void cEffects::PlayMovingAnimation( CBaseObject *source, SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 hue, UI32 renderMode )
{	//0x0f 0x42 = arrow 0x1b 0xfe=bolt
	if( !ValidateObject( source ) )
		return;

	CPGraphicalEffect2 toSend( 0, (*source) );
	toSend.TargetSerial( INVALIDSERIAL );
	toSend.Model( effect );
	toSend.SourceLocation( (*source) );
	toSend.TargetLocation( x, y, z );
	toSend.Speed( speed );
	toSend.Duration( loop );
	toSend.ExplodeOnImpact( explode );
	toSend.Hue( hue );
	toSend.RenderMode( renderMode );

	SOCKLIST nearbyChars = FindNearbyPlayers( source, DIST_SAMESCREEN );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		(*cIter)->Send( &toSend );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PlayCharacterAnimation( CChar *mChar, UI16 actionID, UI08 frameDelay )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends message to client to make character perform specified action/anim
//o-----------------------------------------------------------------------------------------------o
void cEffects::PlayCharacterAnimation( CChar *mChar, UI16 actionID, UI08 frameDelay )
{
	CPCharacterAnimation toSend = (*mChar);
	toSend.Action( actionID );
	toSend.FrameDelay( frameDelay );
	SOCKLIST nearbyChars = FindNearbyPlayers( mChar );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		(*cIter)->Send( &toSend );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PlayNewCharacterAnimation( CChar *mChar, UI16 actionID, UI16 subActionID, UI08 subSubActionID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends message to client to make character perform specified action/anim
//|					in client versions above 7.0.0.0
//o-----------------------------------------------------------------------------------------------o
void cEffects::PlayNewCharacterAnimation( CChar *mChar, UI16 actionID, UI16 subActionID, UI08 subSubActionID )
{
	CPNewCharacterAnimation toSend = (*mChar);
	toSend.Action( actionID );
	toSend.SubAction( subActionID );
	toSend.SubSubAction( subSubActionID );
	SOCKLIST nearbyChars = FindNearbyPlayers( mChar );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		(*cIter)->Send( &toSend );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PlaySpellCastingAnimation( CChar *mChar, UI16 actionID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles spellcasting action
//o-----------------------------------------------------------------------------------------------o
void cEffects::PlaySpellCastingAnimation( CChar *mChar, UI16 actionID )
{
	if( mChar->GetBodyType() == BT_GARGOYLE || ( cwmWorldState->ServerData()->ForceNewAnimationPacket() 
		&& ( mChar->GetSocket() == nullptr || mChar->GetSocket()->ClientType() >= CV_SA2D )))
	{
		if( mChar->GetBodyType() == BT_GARGOYLE )
		{
			if( actionID == 0x10 )
				PlayNewCharacterAnimation( mChar, N_ACT_SPELL, S_ACT_SPELL_AREA ); // Action: 0x0b, SubAction: spell variation 0x01
			else if( actionID == 0x11 )
				PlayNewCharacterAnimation( mChar, N_ACT_SPELL, S_ACT_SPELL_TARGET ); // Action: 0x0b, SubAction: spell variation 0x00
		}
		else
		{
			// Sub actions are in reverse order for humans!
			if( actionID == 0x10 )
				PlayNewCharacterAnimation( mChar, N_ACT_SPELL, S_ACT_SPELL_TARGET ); // Action: 0x0b, SubAction: spell variation 0x00
			else if( actionID == 0x11 )
				PlayNewCharacterAnimation( mChar, N_ACT_SPELL, S_ACT_SPELL_AREA ); // Action: 0x0b, SubAction: spell variation 0x01
		}
		return;
	}
	else if( mChar->IsOnHorse() )
	{
		if( actionID == 0x10 )
			PlayCharacterAnimation( mChar, ACT_MOUNT_ATT_1H ); // 0x1A
		else if( actionID == 0x11 )
			PlayCharacterAnimation( mChar, ACT_MOUNT_ATT_BOW ); // 0x1B
		return;
	}
	if( !cwmWorldState->creatures[mChar->GetID()].IsHuman() && actionID == 0x22 )
		return;
	PlayCharacterAnimation( mChar, actionID );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PlayStaticAnimation( CBaseObject *target, UI16 effect, UI08 speed, UI08 loop, bool explode )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends message to client to play a static animation effect on an object
//o-----------------------------------------------------------------------------------------------o
void cEffects::PlayStaticAnimation( CBaseObject *target, UI16 effect, UI08 speed, UI08 loop, bool explode )
{
	if( !ValidateObject( target ) )
		return;

	CPGraphicalEffect toSend( 3, (*target) );
	if( target->GetObjType() != OT_CHAR )
		toSend.TargetSerial( (*target) );
	else
		toSend.TargetSerial( INVALIDSERIAL );
	toSend.Model( effect );
	toSend.SourceLocation( (*target) );
	if( target->GetObjType() != OT_CHAR )
		toSend.TargetLocation( (*target) );
	toSend.Speed( speed );
	toSend.Duration( loop );
	toSend.AdjustDir( false );
	toSend.ExplodeOnImpact( explode );

	SOCKLIST nearbyChars = FindNearbyPlayers( target, DIST_SAMESCREEN );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		(*cIter)->Send( &toSend );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void cEffects::PlayStaticAnimation( SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends message to client to play a static animation effect at a location
//o-----------------------------------------------------------------------------------------------o
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
	//std::scoped_lock lock(Network->internallock);
	Network->pushConn();
	for( CSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{  // if inrange of effect and online send effect
		tSock->Send( &toSend );
	}
	Network->popConn();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void bolteffect( CChar *player )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends message to client to play a lightning bolt effect on a specific player
//o-----------------------------------------------------------------------------------------------o
void cEffects::bolteffect( CChar *player )
{
	if( !ValidateObject( player ) )
		return;

	CPGraphicalEffect toSend( 1, (*player) );
	toSend.SourceLocation( (*player) );
	toSend.ExplodeOnImpact( false );
	toSend.AdjustDir( false );
	SOCKLIST nearbyChars = FindNearbyPlayers( player );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		(*cIter)->Send( &toSend );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void explodeItem( CSocket *mSock, CItem *nItem )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Explode an item, dealing damage to nearby characters and deleting the item
//o-----------------------------------------------------------------------------------------------o
void explodeItem( CSocket *mSock, CItem *nItem )
{
	CChar *c = mSock->CurrcharObj();
	UI32 dmg = 0;
	UI32 dx, dy, dz;
	// - send the effect (visual and sound)
	if( nItem->GetCont() != nullptr )
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
	
	UI32 len	= nItem->GetTempVar( CITV_MOREX ) / 250; //4 square max damage at 100 alchemy
	dmg			= RandomNum( nItem->GetTempVar( CITV_MOREZ ) * 5, nItem->GetTempVar( CITV_MOREZ ) * 10 );

	if( dmg < 5 )
		dmg = RandomNum( 5, 10 );  // 5 points minimum damage
	if( len < 2 )
		len = 2;  // 2 square min damage range

	REGIONLIST nearbyRegions = MapRegion->PopulateList( nItem );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *Cell = (*rIter);
		bool chain = false;

		GenericList< CChar * > *regChars = Cell->GetCharList();
		regChars->Push();
		for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
		{
			if( !ValidateObject( tempChar ) || tempChar->GetInstanceID() != nItem->GetInstanceID() )
				continue;
			if( tempChar->GetRegion()->IsSafeZone() )
			{
				// Character is in a safe zone, ignore damage
				continue;
			}
			dx = abs( tempChar->GetX() - nItem->GetX() );
			dy = abs( tempChar->GetY() - nItem->GetY() );
			dz = abs( tempChar->GetZ() - nItem->GetZ() );
			if( dx <= len && dy <= len && dz <= len )
			{
				if( !tempChar->IsGM() && !tempChar->IsInvulnerable() && ( tempChar->IsNpc() || isOnline( (*tempChar) ) ) )
				{
					//UI08 hitLoc = Combat->CalculateHitLoc();
					SI16 damage = Combat->ApplyDefenseModifiers( HEAT, c, tempChar, ALCHEMY, 0, ( (SI32)dmg + ( 2 - std::min( dx, dy ) ) ), true);
					tempChar->Damage( damage, c, true );
				}
			}
		}
		regChars->Pop();
		GenericList< CItem * > *regItems = Cell->GetItemList();
		regItems->Push();
		for( CItem *tempItem = regItems->First(); !regItems->Finished(); tempItem = regItems->Next() )
		{
			if( tempItem->GetInstanceID() != nItem->GetInstanceID() )
			{
				// Item is in a different instanceID - ignore
				continue;
			}
			if( tempItem->GetID() == 0x0F0D && tempItem->GetType() == IT_POTION )
			{
				dx = abs( nItem->GetX() - tempItem->GetX() );
				dy = abs( nItem->GetY() - tempItem->GetY() );
				dz = abs( nItem->GetZ() - tempItem->GetZ() );

				if( dx <= 2 && dy <= 2 && dz <= 2 && !chain ) // only trigger if in a 2*2*2 cube
				{
					if( !( dx == 0 && dy == 0 && dz == 0 ) )
					{
						if( RandomNum( 0, 1 ) == 1 )
							chain = true;
						Effects->tempeffect( c, tempItem, 17, 0, 1, 0 );
					}
				}
			}
		}
		regItems->Pop();
	}
	nItem->Delete();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void HandleMakeItemEffect( CTEffect *tMake )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Applies maker's mark to crafted items and plays sound FX
//o-----------------------------------------------------------------------------------------------o
void cEffects::HandleMakeItemEffect( CTEffect *tMake )
{
	if( tMake == nullptr )
		return;

	CChar *src			= calcCharObjFromSer( tMake->Source() );
	UI16 iMaking		= tMake->More2();
	createEntry *toMake = Skills->FindItem( iMaking );
	if( toMake == nullptr )
		return;

	CSocket *sock	= src->GetSocket();
	std::string addItem = toMake->addItem;
	UI16 amount		= 1;
	auto csecs = strutil::sections( addItem, "," );
	if( csecs.size() > 1 )
	{
		amount		= strutil::value<std::uint16_t>(strutil::extractSection(addItem, ",", 1, 1 ));
		addItem		= strutil::extractSection(addItem, ",", 0, 0 );
	}

	CItem *targItem = Items->CreateScriptItem( sock, src, addItem, amount, OT_ITEM, true );
	for( size_t skCounter = 0; skCounter < toMake->skillReqs.size(); ++skCounter )
		src->SkillUsed( false, toMake->skillReqs[skCounter].skillNumber );
	if( targItem == nullptr )
	{
		Console.error( strutil::format("cSkills::MakeItem() bad script item # %s, made by player 0x%X", addItem.c_str(), src->GetSerial()) );
		return;
	}
	else
	{
		targItem->SetName2( targItem->GetName().c_str() );
		SI32 rank = Skills->CalcRankAvg( src, (*toMake) );
		SI32 maxrank = toMake->maxRank;
		Skills->ApplyRank( sock, targItem, static_cast<UI08>(rank), static_cast<UI08>(maxrank) );

		// if we're not a GM, see if we should store our creator
		if( !src->IsGM() && !toMake->skillReqs.empty() )
		{
			targItem->SetCreator( src->GetSerial() );
			SI32 avgSkill, sumSkill = 0;
			// Find the average of our player's skills
			for( size_t resCounter = 0; resCounter < toMake->skillReqs.size(); ++resCounter )
				sumSkill += src->GetSkill( toMake->skillReqs[resCounter].skillNumber );
			avgSkill = sumSkill / toMake->skillReqs.size();
			if( avgSkill > 950 )
				targItem->SetMadeWith( toMake->skillReqs[0].skillNumber + 1 );
			else
				targItem->SetMadeWith( -(toMake->skillReqs[0].skillNumber + 1) );
		}
		else
		{
			targItem->SetCreator( INVALIDSERIAL );
			targItem->SetMadeWith( 0 );
		}
		targItem->EntryMadeFrom( iMaking );
	}

	// Make sure it's movable
	targItem->SetMovable( 1 );
	if( toMake->soundPlayed )
		PlaySound( sock, toMake->soundPlayed, true );

	if( FindItemOwner( targItem ) == src )
		sock->sysmessage( 985 ); // You create the item and place it in your backpack.
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void checktempeffects( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks timer-controlled temporary effects
//o-----------------------------------------------------------------------------------------------o
void cEffects::checktempeffects( void )
{
	CItem *i = nullptr;
	CChar *s = nullptr, *src = nullptr;
	CSocket *tSock = nullptr;
	CBaseObject *myObj = nullptr;

	const UI32 j = cwmWorldState->GetUICurrentTime();
	cwmWorldState->tempEffects.Push();
	for( CTEffect *Effect = cwmWorldState->tempEffects.First(); !cwmWorldState->tempEffects.Finished(); Effect = cwmWorldState->tempEffects.Next() )
	{
		if( Effect == nullptr )
		{
			cwmWorldState->tempEffects.Remove( Effect );
			continue;
		}
		else if( Effect->Destination() == INVALIDSERIAL )
		{
			cwmWorldState->tempEffects.Remove( Effect, true );
			continue;
		}
		if( Effect->ExpireTime() > j )
			continue;

		if( Effect->Destination() < BASEITEMSERIAL )
		{
			s = calcCharObjFromSer( Effect->Destination() );
			if( !ValidateObject( s ) )
			{
				cwmWorldState->tempEffects.Remove( Effect, true );
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
						tSock->sysmessage( 700 );
				}
				break;
			case 2: // Nightsight Potion (JS) and Spell (code)
				if( validChar )
				{
					s->SetFixedLight( 255 );
					doLight( tSock, cwmWorldState->ServerData()->WorldLightCurrentLevel() );
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
					s->SetStamina( std::min(s->GetStamina(), s->GetMaxStam()) );
					equipCheckNeeded = true;
				}
				break;
			case 7: // Cunning Spell
				if( validChar )
				{
					s->IncIntelligence2( -Effect->More1() );
					s->SetMana( std::min(s->GetMana(), s->GetMaxMana()) );
					equipCheckNeeded = true;
				}
				break;
			case 8: // Strength Potion (JS) and Spell (code)
				if( validChar )
				{
					s->IncStrength2( -Effect->More1() );
					s->SetHP( std::min(s->GetHP(), static_cast<SI16>(s->GetMaxHP())) );
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
								s->TextMessage( nullptr, 1270, EMOTE, 1, s->GetName().c_str() );
							PlaySound( s, 0x0242 );
							break;
					}
				}
				break;
			case 11: // Bless Spell
				if( validChar )
				{
					s->IncStrength2( -Effect->More1() );
					s->SetHP( std::min(s->GetHP(), static_cast<SI16>(s->GetMaxHP())) );
					s->IncDexterity2( -Effect->More2() );
					s->SetStamina( std::min(s->GetStamina(), s->GetMaxStam()) );
					s->IncIntelligence2( -Effect->More3() );
					s->SetMana( std::min(s->GetMana(), s->GetMaxMana()) );
					equipCheckNeeded = true;
					}
				break;
			case 12: // Curse Spell
				if( validChar )
				{
					s->IncStrength2( Effect->More1() );
					s->IncDexterity2( Effect->More2() );
					s->IncIntelligence2( Effect->More3() );
					equipCheckNeeded = true;
				}
				else
					equipCheckNeeded = false;
				break;
			case 15: // Reactive Armor Spell
				if( validChar )
				{
					s->SetReactiveArmour( false );
				}
				break;
			case 16: // Explosion potion messages (JS)
				src = calcCharObjFromSer( Effect->Source() );
				if( src->GetTimer( tCHAR_ANTISPAM ) < cwmWorldState->GetUICurrentTime() )
				{
					src->SetTimer( tCHAR_ANTISPAM, BuildTimeValue( 1 ) );
					std::string mTemp = strutil::number( Effect->More3() );
					if( tSock != nullptr )
					{
						tSock->sysmessage( mTemp.c_str() );
					}
				}
				break;
			case 17: // Explosion effect (JS and code)
				src = calcCharObjFromSer( Effect->Source() );
				explodeItem( src->GetSocket(), static_cast<CItem *>(Effect->ObjPtr()) ); //explode this item
				break;
			case 18: // Polymorph Spell
				if( validChar )
				{
					s->SetID( s->GetOrgID() );
					s->IsPolymorphed( false );
				}
				break;
			case 19: // Incognito Spell
				if( validChar )
				{
					s->SetID( s->GetOrgID() );

					// ------ NAME -----
					s->SetName( s->GetOrgName() );

					i = s->GetItemAtLayer( IL_HAIR );
					if( ValidateObject( i ) )
					{
						i->SetColour( s->GetHairColour() );
						i->SetID( s->GetHairStyle() );
					}
					i = s->GetItemAtLayer( IL_FACIALHAIR );
					if( ValidateObject( i ) && s->GetID( 2 ) == 0x90 )
					{
						i->SetColour( s->GetBeardColour() );
						i->SetID( s->GetBeardStyle() );
					}
					if( tSock != nullptr )
						s->SendWornItems( tSock );
					s->IsIncognito( false );
				}
				break;
			case 21: // Protection Spell
				if( validChar )
				{
					UI16 toDrop;
					toDrop = Effect->More1();
					if( ( s->GetBaseSkill( PARRYING ) - toDrop ) < 0 )
						s->SetBaseSkill( 0, PARRYING );
					else
						s->SetBaseSkill( s->GetBaseSkill( PARRYING ) - toDrop, PARRYING );
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
				src = calcCharObjFromSer( Effect->Source() );
				if( !ValidateObject( src ) || !ValidateObject( s ) )
					break;
				Magic->playSound( src, 43 );
				Magic->doMoveEffect( 43, s, src );
				Magic->doStaticEffect( s, 43 );
				Magic->MagicDamage( s, Effect->More1(), src, HEAT );
				equipCheckNeeded = true;
				break;
			case 40: // Used by JS timers
			{
				// Default/Global script ID
				UI16 scpNum			= 0xFFFF;

				// Get script associated with effect, if any
				cScript *tScript	= JSMapping->GetScript( Effect->AssocScript() );

				// items have serials of 0x40000000 and above, and we already know it's not INVALIDSERIAL
				if( Effect->Source() >= BASEITEMSERIAL )	
				{
					myObj = calcItemObjFromSer( Effect->Source() );
					equipCheckNeeded = false;
				}
				else
				{
					myObj = calcCharObjFromSer( Effect->Source() );
					equipCheckNeeded = true;
				}

				// Check that object effect was running on still exists
				if( !ValidateObject( myObj ) )
					break;

				// No associated script, so it must be another callback variety
				if( tScript == nullptr )
				{
					if( Effect->More2() != 0xFFFF ) // A scriptID other than default one was found
					{
						scpNum = Effect->More2();
						tScript = JSMapping->GetScript( scpNum );
					}
					/*else
					{
						// No specific script was associated with effect, so let's see if there's a script associated with object instead

						scpNum = myObj->GetScriptTrigger();
					}
					tScript = JSMapping->GetScript( scpNum );*/
				}

				// Make sure to check for a specific script via envoke system when the previous checks ended in the global script.
				if( ( tScript == nullptr || scpNum == 0) && Effect->Source() >= BASEITEMSERIAL )
				{
					if( JSMapping->GetEnvokeByType()->Check( static_cast<UI16>((static_cast<CItem *>(myObj))->GetType()) ) )
					{
						scpNum	= JSMapping->GetEnvokeByType()->GetScript( static_cast<UI16>((static_cast<CItem *>(myObj))->GetType()) );
						tScript = JSMapping->GetScript( scpNum );
					}
					else if( JSMapping->GetEnvokeByID()->Check( myObj->GetID() ) )
					{
						scpNum	= JSMapping->GetEnvokeByID()->GetScript( myObj->GetID() );
						tScript	= JSMapping->GetScript( scpNum );
					}
				}

				// Callback to onTimer event in script
				if( tScript != nullptr )
				{
					tScript->OnTimer( myObj, static_cast<UI08>(Effect->More1()) );
				}
				break;
			}
			case 41: // Start of item crafting
				HandleMakeItemEffect( Effect );
				break;
			case 42: // End of item crafting
				src = calcCharObjFromSer( Effect->Source() );
				PlaySound( src, Effect->More2() );
				break;
			case 43: // Turn a wooly sheep into a sheared sheep
				src = calcCharObjFromSer( Effect->Source() );
				if( !ValidateObject( src ) )	// char doesn't exist!
					break;
				else if( src->GetID() == 0xCF )
					break;

				src->SetID( 0xCF ); // Thats all we need to do
				break;
			default:
				Console.error( strutil::format(" Fallout of switch statement without default (%i). checktempeffects()", Effect->Number()) );
				break;
		}
		if( validChar && equipCheckNeeded )
			Items->CheckEquipment( s ); // checks equipments for stat requirements
		cwmWorldState->tempEffects.Remove( Effect, true );
	}
	cwmWorldState->tempEffects.Pop();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void reverseEffect( CTEffect *Effect )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Reverse a temp effect
//o-----------------------------------------------------------------------------------------------o
void reverseEffect( CTEffect *Effect )
{
	CChar *s = calcCharObjFromSer( Effect->Destination() );
	if( ValidateObject( s ) )
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
				s->SetID( s->GetOrgID() );
				s->IsPolymorphed( false );
				break;
			case 19: // Incognito Spell
				CItem *j;

				// ------ SEX ------
				s->SetID( s->GetOrgID() );
				s->SetName( s->GetOrgName() );
				j = s->GetItemAtLayer( IL_HAIR );
				if( ValidateObject( j ) )
				{
					j->SetColour( s->GetHairColour() );
					j->SetID( s->GetHairStyle() );
				}
				j = s->GetItemAtLayer( IL_FACIALHAIR );
				if( ValidateObject( j ) && s->GetID( 2 ) == 0x90 )
				{
					j->SetColour( s->GetBeardColour() );
					j->SetID( s->GetBeardStyle() );
				}
				// only refresh once
				CSocket *tSock;
				tSock = s->GetSocket();
				s->SendWornItems( tSock );
				s->IsIncognito( false );
				break;
			case 21: // Protection Spell
				SI32 toDrop;
				toDrop = Effect->More1() ;
				if( ( s->GetBaseSkill( PARRYING ) - toDrop ) < 0 )
					s->SetBaseSkill( 0, PARRYING );
				else
					s->SetBaseSkill( s->GetBaseSkill( PARRYING ) - toDrop, PARRYING );
				break;
			case 26: // Re-enable the usage of potions
				s->SetUsingPotion( false );
				break;
			default:
				Console.error( " Fallout of switch statement without default. uox3.cpp, reverseEffect()");
				break;
		}
	}
	Items->CheckEquipment( s );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void tempeffect( CChar *source, CChar *dest, UI08 num, UI16 more1, UI16 more2, UI16 more3, CItem *targItemPtr )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a temp effect to a character
//o-----------------------------------------------------------------------------------------------o
void cEffects::tempeffect( CChar *source, CChar *dest, UI08 num, UI16 more1, UI16 more2, UI16 more3, CItem *targItemPtr )
{
	if( !ValidateObject( source ) || !ValidateObject( dest ) )
		return;

	bool spellResisted = false;
	CTEffect *toAdd	= new CTEffect;
	SERIAL sourSer	= source->GetSerial();
	SERIAL targSer	= dest->GetSerial();
	toAdd->Source( sourSer );
	toAdd->Destination( targSer );

	cwmWorldState->tempEffects.Push();
	for( CTEffect *Effect = cwmWorldState->tempEffects.First(); !cwmWorldState->tempEffects.Finished(); Effect = cwmWorldState->tempEffects.Next() )
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
						reverseEffect( Effect );
						cwmWorldState->tempEffects.Remove( Effect, true );
						break;
					default:
						break;
				}
			}
		}
	}
	cwmWorldState->tempEffects.Pop();
	CSocket *tSock = dest->GetSocket();
	toAdd->Number( num );
	switch( num )
	{
		case 1: // Paralyse Spell
			dest->SetFrozen( true );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 100.0f ) );
			toAdd->Dispellable( true );
			break;
		case 2: // Nightsight Potion (JS) and Spell (code)
			SI16 worldbrightlevel;
			worldbrightlevel = cwmWorldState->ServerData()->WorldLightBrightLevel();
			dest->SetFixedLight( static_cast<UI08>(worldbrightlevel) );
			doLight( tSock, static_cast<SI08>(worldbrightlevel) );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 2.0f ) );
			toAdd->Dispellable( true );
			break;
		case 3: // Clumsy Spell (JS)
			if( dest->GetDexterity() < more1 )
				more1 = dest->GetDexterity();
			dest->IncDexterity2( -more1 );
			dest->SetStamina( std::min( dest->GetStamina(), dest->GetMaxStam() ) );
			//Halve effect-timer on resist
			spellResisted = Magic->CheckResist( source, dest, 1 );
			if( spellResisted )
				toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 20.0f ) );
			else
				toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		case 4: // Feeblemind Spell (JS)
			if( dest->GetIntelligence() < more1 )
				more1 = dest->GetIntelligence();
			dest->IncIntelligence2( -more1 );
			dest->SetMana( std::min(dest->GetMana(), dest->GetMaxMana() ) );
			//Halve effect-timer on resist
			spellResisted = Magic->CheckResist( source, dest, 1 );
			if( spellResisted )
				toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 20.0f ) );
			else
				toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		case 5: // Weaken Spell
			if( dest->GetStrength() < more1 )
				more1 = dest->GetStrength();
			dest->IncStrength2( -more1 );
			dest->SetHP( std::min( dest->GetHP(), static_cast<SI16>(dest->GetMaxHP()) ) );
			//Halve effect-timer on resist
			spellResisted = Magic->CheckResist( source, dest, 4 );
			if( spellResisted )
				toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 20.0f ) );
			else
				toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		case 6: // Agility Potion (JS) and Spell (code)
			dest->IncDexterity( more1 );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		case 7: // Cunning Spell
			dest->IncIntelligence2( more1 );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		case 8: // Strength Potion (JS) and Spell (code)
			dest->IncStrength2( more1 );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		case 9: // Grind Necro Reagent
			toAdd->ExpireTime( BuildTimeValue( (R32)more2 ) );
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			toAdd->Dispellable( false );
			break;
		case 10: // ???
			toAdd->ExpireTime( BuildTimeValue( 12.0f ) );
			toAdd->Dispellable( false );
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			break;
		case 11: // Bless Spell
			dest->IncStrength2( more1 );
			dest->IncDexterity2( more2 );
			dest->IncIntelligence2( more3 );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			toAdd->More3( more3 );
			toAdd->Dispellable( true );
			break;
		case 12: // Curse Spell
			if( dest->GetStrength() < more1 )
				more1 = dest->GetStrength();
			if( dest->GetDexterity() < more2 )
				more2 = dest->GetDexterity();
			if( dest->GetIntelligence() < more3 )
				more3 = dest->GetIntelligence();
			dest->IncStrength2( -more1 );
			dest->IncDexterity2( -more2 );
			dest->IncIntelligence2( -more3 );
			//Halve effect-timer on resist
			spellResisted = Magic->CheckResist( source, dest, 4 );
			if( spellResisted )
				toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 20.0f ) );
			else
				toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			toAdd->More3( more3 );
			toAdd->Dispellable( true );
			break;
		case 15: // Reactive Armor Spell
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->Dispellable( true );
			break;
		case 16: // Explosion potion messages (JS)
			toAdd->ExpireTime( BuildTimeValue( (R32)more2 ) );
			toAdd->More1( more1 ); //item/potion
			toAdd->More2( more2 ); //seconds
			toAdd->More3( more3 ); //countdown#
			toAdd->Dispellable( false );
			break;
		case 18: // Polymorph Spell
			toAdd->ExpireTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_POLYMORPH ) );
			toAdd->Dispellable( false );

			UI16 k;
			// Grey flag when polymorphed
			dest->SetTimer( tCHAR_CRIMFLAG, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_POLYMORPH ) );
			if( dest->IsOnHorse() )
				DismountCreature( dest );
			else if( dest->IsFlying() )
				dest->ToggleFlying();
			k = ( more1<<8 ) + more2;

			if( k <= 0x03e2 ) // body-values >0x3e1 crash the client
				dest->SetID( k );
			break;
		case 19: // Incognito Spell
			toAdd->ExpireTime( BuildTimeValue( 90.0f ) ); // 90 seconds
			toAdd->Dispellable( false );
			break;
		case 21: // Protection Spell
			toAdd->ExpireTime( BuildTimeValue( 120.0f ) );
			toAdd->Dispellable( true );
			toAdd->More1( more1 );
			dest->SetBaseSkill( dest->GetBaseSkill( PARRYING ) + more1, PARRYING );
			break;
		case 25: // Temporarily set item as disabled
			toAdd->ExpireTime( BuildTimeValue( (R32)more1 ) );
			toAdd->ObjPtr( dest );
			dest->SetDisabled( true );
			toAdd->More2( 1 );
			break;
		case 26: // Temporarily disable the usage of potions
			toAdd->ExpireTime( cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_POTION ) );
			dest->SetUsingPotion( true );
			break;
		case 27: // Explosion Spell
			toAdd->ExpireTime( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->CombatExplodeDelay()) ) );
			toAdd->More1( more1 );
			break;
		case 40: // Used by JS timers
			toAdd->ExpireTime( BuildTimeValue( ( (R32)more1 + (R32)more2 / 1000.0f ) ) );
			toAdd->More1( more3 );
			break;
		case 41: // creating item
			toAdd->ExpireTime( BuildTimeValue( (R32)(more1) / 100.0f ) );
			toAdd->More2( more2 );
			break;
		case 42: // delayed sound effect for crafting
			toAdd->ExpireTime( BuildTimeValue( (R32)(more1) / 100.0f ) );
			toAdd->More2( more2 );
			break;
		case 43: // regain wool for sheeps (JS)
			toAdd->ExpireTime( BuildTimeValue( (R32)(more1) ) );
			break;
		default:
			Console.error( strutil::format(" Fallout of switch statement (%d) without default. uox3.cpp, tempeffect()", num ));
			delete toAdd;
			return;
	}
	cwmWorldState->tempEffects.Add( toAdd );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void tempeffect( CChar *source, CItem *dest, UI08 num, UI16 more1, UI16 more2, UI16 more3 )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a temp effect to an item
//o-----------------------------------------------------------------------------------------------o
void cEffects::tempeffect( CChar *source, CItem *dest, UI08 num, UI16 more1, UI16 more2, UI16 more3 )
{
	if( !ValidateObject( dest ) )
		return;

	CTEffect *toAdd = new CTEffect;

	if( ValidateObject( source ) )
		toAdd->Source( source->GetSerial() );
	else
		toAdd->Source( INVALIDSERIAL );

	toAdd->Destination( dest->GetSerial() );
	toAdd->Number( num );
	switch( num )
	{
		case 10: // Crafting Potion??
			toAdd->ExpireTime( BuildTimeValue( 12 ) );
			toAdd->Dispellable( false );
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			source->SkillUsed( true, ALCHEMY );
			break;
		case 13: // Timer for door (gangplank on boats?) to autoclose?
			if( !dest->isDoorOpen() )
				dest->SetDoorOpen( true );

			toAdd->ExpireTime( BuildTimeValue( 10 ) );
			toAdd->Dispellable( false );
			break;
		case 17: // Explosion potion (explosion)  Tauriel (explode in 4 seconds)
			toAdd->ExpireTime( BuildTimeValue( 4 ) );
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			toAdd->ObjPtr( dest );
			toAdd->Dispellable( false );
			break;
		case 25: // // Temporarily set item as disabled
			toAdd->ExpireTime( BuildTimeValue( more1 ) );
			toAdd->ObjPtr( dest );
			dest->SetDisabled( true );
			toAdd->More2( 0 );
			break;
		default:
			Console.error( " Fallout of switch statement without default. uox3.cpp, tempeffect2()");
			delete toAdd;
			return;
	}
	cwmWorldState->tempEffects.Add( toAdd );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SaveEffects( void )
//|	Date		-	16 March, 2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Saves out the TempEffects to effects.wsc
//o-----------------------------------------------------------------------------------------------o
void cEffects::SaveEffects( void )
{
	std::ofstream effectDestination;
	const char blockDiscriminator[] = "\n\n---EFFECT---\n\n";
	SI32 s_t							= getclock();

	Console << "Saving Effects...   ";
	Console.TurnYellow();

	std::string filename = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "effects.wsc";
	effectDestination.open( filename.c_str() );
	if( !effectDestination )
	{
		Console.error( strutil::format("Failed to open %s for writing", filename.c_str()) );
		return;
	}

	cwmWorldState->tempEffects.Push();
	for( CTEffect *currEffect = cwmWorldState->tempEffects.First(); !cwmWorldState->tempEffects.Finished(); currEffect = cwmWorldState->tempEffects.Next() )
	{
		if( currEffect == nullptr )
			continue;
		currEffect->Save( effectDestination );
		effectDestination << blockDiscriminator;
	}
	effectDestination.close();
	cwmWorldState->tempEffects.Pop();

	Console << "\b\b\b\b";
	Console.PrintDone();

	SI32 e_t = getclock();
	Console.print( strutil::format("Effects saved in %.02fsec\n", ((R32)(e_t-s_t))/1000.0f) );
}

void ReadWorldTagData( std::ifstream &inStream, std::string &tag, std::string &data );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void LoadEffects( void )
//|	Date		-	16 March, 2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads in Effects from disk
//o-----------------------------------------------------------------------------------------------o
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
			std::string sLine(line);
			sLine = strutil::stripTrim( sLine );
			auto usLine = strutil::toupper( sLine );
			
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
							UTag = strutil::toupper( tag );
							switch( (UTag.data()[0]) )
							{
								case 'A':
									if( UTag == "ASSOCSCRIPT" )
										toLoad->AssocScript( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
									break;
								case 'D':
									if( UTag == "DEST" )
										toLoad->Destination( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
									if( UTag == "DISPEL" )
										toLoad->Dispellable((( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) == 0 ) ? false : true ));
									break;
								case 'E':
									if( UTag == "EXPIRE" )
										toLoad->ExpireTime( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) + cwmWorldState->GetUICurrentTime() );
									break;
								case 'I':
									if( UTag == "ITEMPTR" )
									{
										SERIAL objSer = static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
										if( objSer != INVALIDSERIAL )
										{
											if( objSer < BASEITEMSERIAL )
												toLoad->ObjPtr( calcCharObjFromSer( objSer ) );
											else
												toLoad->ObjPtr( calcItemObjFromSer( objSer ) );
										}
										else
											toLoad->ObjPtr( nullptr );
									}
									break;
								case 'M':
									if( UTag == "MORE1" )
										toLoad->More1( static_cast<UI16>(std::stoul(strutil::stripTrim( data ),nullptr, 0)) );
									if( UTag == "MORE2" )
										toLoad->More2( static_cast<UI16>(std::stoul(strutil::stripTrim( data ),nullptr, 0)) );
									if( UTag == "MORE3" )
										toLoad->More3( static_cast<UI16>(std::stoul(strutil::stripTrim( data ),nullptr, 0)) );
									break;
								case 'N':
									if( UTag == "NUMBER" )
										toLoad->Number( static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)));
									break;
								case 'O':
									if( UTag == "OBJPTR" )
									{
										SERIAL objSer = static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
										if( objSer != INVALIDSERIAL )
										{
											if( objSer < BASEITEMSERIAL )
												toLoad->ObjPtr( calcCharObjFromSer( objSer ) );
											else
												toLoad->ObjPtr( calcItemObjFromSer( objSer ) );
										}
										else
											toLoad->ObjPtr( nullptr );
									}
									break;
								case 'S':
									if( UTag == "SOURCE" )
										toLoad->Source( static_cast<UI32>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
									break;
								default:
									Console.error( strutil::format("Unknown effects tag %s with contents of %s", tag.c_str(), data.c_str()) );
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool Save( std::ofstream &effectDestination ) const
//|	Date		-	March 16, 2002
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Output the effect data to the effect destination. Returns true/false
//|					indicating the success of the write operation
//o-----------------------------------------------------------------------------------------------o
bool CTEffect::Save( std::ofstream &effectDestination ) const
{
	CBaseObject *getPtr = nullptr;

	effectDestination << "[EFFECT]" << '\n';

	// Hexadecimal Values
	effectDestination << std::hex;
	effectDestination << "Source=" << "0x" << Source() << '\n';
	effectDestination << "Dest=" << "0x" << Destination() << '\n';

	getPtr = ObjPtr();
	effectDestination << "ObjPtr=" << "0x";
	if( ValidateObject( getPtr ) )
		effectDestination << getPtr->GetSerial() << '\n';
	else
		effectDestination << INVALIDSERIAL << '\n';

	// Decimal / String Values
	effectDestination << std::dec;
	effectDestination << "Expire=" << ( ExpireTime() - cwmWorldState->GetUICurrentTime() ) << '\n';
	effectDestination << "Number=" << static_cast<UI16>(Number()) << '\n';
	effectDestination << "More1=" << More1() << '\n';
	effectDestination << "More2=" << More2() << '\n';
	effectDestination << "More3=" << More3() << '\n';
	effectDestination << "Dispel=" << Dispellable() << '\n';
	effectDestination << "AssocScript=" << AssocScript() << '\n';
	effectDestination << '\n' << "o---o" << '\n' << '\n';
	return true;
}
