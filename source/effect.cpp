#include "uox3.h"
#include "skills.h"
#include "cMagic.h"
#include "trigger.h"
#include "mapstuff.h"
#include "cScript.h"
#include "cEffects.h"
#include "teffect.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "combat.h"

namespace UOX
{

bool isDoorBlocked( CItem *door );

//o---------------------------------------------------------------------------o
//|	Function	-	void deathAction( CChar *s, CItem *x )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Plays a characters death animation
//o---------------------------------------------------------------------------o
void cEffects::deathAction( CChar *s, CItem *x )
{
	CPDeathAction toSend( (*s), (*x) );
	toSend.FallDirection( 0 );
	SOCKLIST nearbyChars = FindNearbyPlayers( s );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		if( (*cIter)->CurrcharObj() != s )	// Death action screws up corpse display for the person who died.
			(*cIter)->Send( &toSend );
	}
}

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
	Network->PushConn();
	for( CSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( objInRange( tSock, source, DIST_SAMESCREEN ) && objInRange( tSock, dest, DIST_SAMESCREEN ) )
			tSock->Send( &toSend );
	}
	Network->PopConn();
}

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

//o---------------------------------------------------------------------------o
//|	Function	-	void PlayCharacterAnimation( CChar *mChar, UI16 actionID )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Character does a certain action
//o---------------------------------------------------------------------------o
void cEffects::PlayCharacterAnimation( CChar *mChar, UI16 actionID )
{
	CPCharacterAnimation toSend = (*mChar);
	toSend.Action( actionID );
	SOCKLIST nearbyChars = FindNearbyPlayers( mChar );
	for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
	{
		(*cIter)->Send( &toSend );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void PlaySpellCastingAnimation( CChar *mChar, UI16 actionID )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handles spellcasting action
//o---------------------------------------------------------------------------o
void cEffects::PlaySpellCastingAnimation( CChar *mChar, UI16 actionID )
{
	if( mChar->IsOnHorse() && ( actionID == 0x10 || actionID == 0x11 ) )
	{
		PlayCharacterAnimation( mChar, 0x1B );
		return;
	}
	if( ( mChar->IsOnHorse() || !mChar->isHuman() ) && actionID == 0x22 )
		return;
	PlayCharacterAnimation( mChar, actionID );
}


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

// for effects on items
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
	Network->PushConn();
	for( CSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{  // if inrange of effect and online send effect
		tSock->Send( &toSend );
	}
	Network->PopConn();
}

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

//o---------------------------------------------------------------------------o
//|	Function	-	void explodeItem( CSocket *mSock, CItem *nItem )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Explode an item
//o---------------------------------------------------------------------------o
void explodeItem( CSocket *mSock, CItem *nItem )
{
	CChar *c = mSock->CurrcharObj();
	UI32 dmg = 0;
	UI32 dx, dy, dz;
	// - send the effect (visual and sound)
	if( nItem->GetCont() != NULL )
	{
		Effects->PlayStaticAnimation( c, 0x36B0, 0x00, 0x09 );
		nItem->SetCont( NULL );
		nItem->SetLocation(  c );
		Effects->PlaySound( c, 0x0207 );
	}
	else
	{
		Effects->PlayStaticAnimation( nItem, 0x36B0, 0x00, 0x09, 0x00);
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
		SubRegion *Cell = (*rIter);
		bool chain = false;
	
		CDataList< CChar * > *regChars = Cell->GetCharList();
		regChars->Push();
		for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
		{
			dx = abs( tempChar->GetX() - nItem->GetX() );
			dy = abs( tempChar->GetY() - nItem->GetY() );
			dz = abs( tempChar->GetZ() - nItem->GetZ() );
			if( dx <= len && dy <= len && dz <= len )
			{
				if( !tempChar->IsGM() && !tempChar->IsInvulnerable() && ( tempChar->IsNpc() || isOnline( tempChar ) ) )
				{
					if( Combat->WillResultInCriminal( c, tempChar ) )
						criminal( c );
					tempChar->IncHP( -(SI16)( (SI32)dmg + ( 2 - UOX_MIN( dx, dy ) ) ) );
					if( tempChar->GetHP() <= 0 )
						HandleDeath( tempChar );
					else
						Combat->AttackTarget( c, tempChar );
				}
			}
		}
		regChars->Pop();
		CDataList< CItem * > *regItems = Cell->GetItemList();
		regItems->Push();
		for( CItem *tempItem = regItems->First(); !regItems->Finished(); tempItem = regItems->Next() )
		{
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

void cEffects::HandleMakeItemEffect( CTEffect *tMake )
{
	if( tMake == NULL )
		return;
	CChar *src = calcCharObjFromSer( tMake->Source() );
	UI16 iMaking = tMake->More2();
	createEntry *toMake = Skills->FindItem( iMaking );
	if( toMake == NULL )
		return;

	CSocket *sock = calcSocketObjFromChar( src );
	// Create the item in our backpack
	CItem *targItem = Items->CreateScriptItem( sock, src, toMake->addItem, 1, OT_ITEM, true );
	for( size_t skCounter = 0; skCounter < toMake->skillReqs.size(); ++skCounter )
		src->SkillUsed( false, toMake->skillReqs[skCounter].skillNumber );
	if( targItem == NULL )
	{
		Console.Error( 2, "cSkills::MakeItem() bad script item # %s, made by player 0x%X", toMake->addItem.c_str(), src->GetSerial() );
		return;
	}
	else
	{
		targItem->SetName2( targItem->GetName().c_str() );
		SI32 rank = Skills->CalcRankAvg( src, (*toMake) );
		Skills->ApplyRank( sock, targItem, static_cast<UI08>(rank) );
		
		// if we're not a GM, see if we should store our creator
		if( !src->IsGM() && toMake->skillReqs.size() > 0 )
		{
			targItem->SetCreator( src->GetSerial() );
			int avgSkill, sumSkill = 0;
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
	PlaySound( sock, toMake->soundPlayed, true );

	sock->sysmessage( 985 );
	sock->statwindow( src );
}

void cEffects::checktempeffects( void )
{
	CItem *i = NULL;
	CChar *s = NULL, *src = NULL, *targ = NULL;
	CSocket *tSock = NULL;
	CBaseObject *myObj = NULL;

	UI32 j = cwmWorldState->GetUICurrentTime();
	TEffects->StartQueue();
	for( CTEffect *Effect = TEffects->First(); !TEffects->AtEnd(); Effect = TEffects->Next() )
	{
		if( Effect == NULL )
		{
			TEffects->QueueToKeep( Effect );
			continue;
		}
		if( Effect->ExpireTime() > j || Effect->Destination() == INVALIDSERIAL )
		{
			TEffects->QueueToKeep( Effect );
			continue;
		}

		if( Effect->Destination() < BASEITEMSERIAL )
		{
			s = calcCharObjFromSer( Effect->Destination() );
			if( !ValidateObject( s ) )
			{
				TEffects->QueueToKeep( Effect );
				continue;
			}
			tSock = calcSocketObjFromChar( s );
		}
		TEffects->QueueToRemove( Effect );
		bool equipCheckNeeded = false;

		switch( Effect->Number() )
		{
			case 1:
				if( s->IsFrozen() )
				{
					s->SetFrozen( false );
					if( tSock != NULL )
						tSock->sysmessage( 700 );
				}
				break;
			case 2:
				s->SetFixedLight( 255 );
				doLight( tSock, cwmWorldState->ServerData()->WorldLightCurrentLevel() );
				break;
			case 3:
				s->IncDexterity2( Effect->More1() );
				equipCheckNeeded = true;
				break;
			case 4:
				s->IncIntelligence2( Effect->More1() );
				equipCheckNeeded = true;
				break;
			case 5:
				s->IncStrength2( Effect->More1() );
				equipCheckNeeded = true;
				break;
			case 6:
				s->IncDexterity2( -Effect->More1() );
				s->SetStamina( UOX_MIN( s->GetStamina(), s->GetMaxStam() ) );
				equipCheckNeeded = true;
				break;
			case 7:
				s->IncIntelligence2( -Effect->More1() );
				s->SetMana( UOX_MIN( s->GetMana(), s->GetMaxMana() ) );
				equipCheckNeeded = true;
				break;
			case 8:
				s->IncStrength2( -Effect->More1() );
				s->SetHP( UOX_MIN( s->GetHP(), static_cast<SI16>(s->GetMaxHP()) ) );
				equipCheckNeeded = true;
				break;
			case 9:	// Grind potion (also used for necro stuff)
				switch( Effect->More1() )
				{
					case 0:
						if( Effect->More2() != 0 )
							s->emoteAll( 1270, true, s->GetName().c_str() );
						PlaySound( s, 0x0242 );
						break;
				}
				break;
			case 11:
				s->IncStrength2( -Effect->More1() );
				s->SetHP(  UOX_MIN( s->GetHP(), static_cast<SI16>(s->GetMaxHP())) );
				s->IncDexterity2( -Effect->More2() );
				s->SetStamina( UOX_MIN( s->GetStamina(), s->GetMaxStam() ) );
				s->IncIntelligence2( -Effect->More3() );
				s->SetMana( UOX_MIN( s->GetMana(), s->GetMaxMana() ) );
				equipCheckNeeded = true;
				break;
			case 12:
				if( s!=NULL )
				{
					s->IncStrength2( Effect->More1() );
					s->IncDexterity2( Effect->More2() );
					s->IncIntelligence2( Effect->More3() );
					equipCheckNeeded = true;
				}
				else
					equipCheckNeeded = false;
				break;
			case 13: // door
				i = calcItemObjFromSer( Effect->Destination() );
				if( !ValidateObject( i ) )
					continue;

				if( !i->isDoorOpen() ) 
					break;

				i->SetDoorOpen( false );

				if( isDoorBlocked( i ) )
					tempeffect( NULL, i, 13, 0, 0, 0 );
				else
					useDoor( NULL, i );
				break;
			case 14: //- training dummies Tauriel check to see if item moved or not before searching for it
				i = (CItem *)Effect->ObjPtr();
				SERIAL effectSerial;
				effectSerial = Effect->Destination();
				if( i->GetSerial() != effectSerial )
					i = calcItemObjFromSer( effectSerial );
				if( i->GetID() == 0x1071 )
				{
					i->IncID( -1 );
					i->SetGateTime( 0 );
				} 
				else if( i->GetID() == 0x1075 )
				{
					i->IncID( -1 );
					i->SetGateTime( 0 );
				} 
				break;
			case 15: //reactive armor
				s->SetReactiveArmour( false );
				break;
			case 16: //Explosion potion messages
				src = calcCharObjFromSer( Effect->Source() );
				if( src->GetTimer( tCHAR_ANTISPAM ) < cwmWorldState->GetUICurrentTime() )
				{
					src->SetTimer( tCHAR_ANTISPAM, BuildTimeValue( 1 ) );
					UString mTemp = UString::number( Effect->More3() );
					if( tSock != NULL )
						tSock->sysmessage( mTemp.c_str() );
				} 
				break;
			case 17: //Explosion potion
				src = calcCharObjFromSer( Effect->Source() );
				explodeItem( calcSocketObjFromChar( src ), (CItem *)Effect->ObjPtr() ); //explode this item
				break;
			case 18: //Polymorph spell
				s->SetID( s->GetOrgID() );
				s->IsPolymorphed( false );
				break;
			case 19: //Incognito spell
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
				if( tSock != NULL )
					s->SendWornItems( tSock );
				s->IsIncognito( false );
				break;
				
			case 21:
				UI16 toDrop;
				toDrop = Effect->More1();
				if( ( s->GetBaseSkill( PARRYING ) - toDrop ) < 0 )
					s->SetBaseSkill( 0, PARRYING );
				else
					s->SetBaseSkill( s->GetBaseSkill( PARRYING ) - toDrop, PARRYING );
				equipCheckNeeded = true;
				break;
			case 22:	// heal
			case 23:	// resurrect
			case 24:	// cure
				R32 newHealth;
				src = calcCharObjFromSer( Effect->Source() );
				targ = calcCharObjFromSer( Effect->Destination() );
				i = (CItem *)Effect->ObjPtr();
				if( ValidateObject( src ) && ValidateObject( targ ) )
				{
					if( src->SkillUsed( static_cast<UI08>(Effect->More1()) ) )
					{
						CSocket *srcSock = calcSocketObjFromChar( src );
						CSocket *targSock = calcSocketObjFromChar( targ );
						if( Effect->Number() == 22 )
						{
							newHealth = static_cast<R32>(targ->GetHP() + ( src->GetSkill( ANATOMY ) / 50 + RandomNum( 3, 10 ) + RandomNum( src->GetSkill( HEALING ) / 50, src->GetSkill( HEALING ) / 20 ) ));
							targ->SetHP( UOX_MIN( static_cast<SI16>(targ->GetMaxHP()), (SI16)newHealth ) );
							srcSock->sysmessage( 1271 );
						}
						else if( Effect->Number() == 23 )
						{
							NpcResurrectTarget( targ );
							srcSock->sysmessage( 1272 );
						}
						else
						{
							targ->SetPoisoned( 0 );
							if( targSock != NULL )
							{
								PlayStaticAnimation( targ, 0x373A, 0, 15 );
								PlaySound( targSock, 0x01E0, false );
								targSock->sysmessage( 1273 );
							}
							if( srcSock != NULL )
								srcSock->sysmessage( 1274 );
						}
					}
					src->SkillUsed( false, static_cast<UI08>(Effect->More1()) );

				}
				i->IncAmount( -1 );
				break;
			case 25:
				if( Effect->More2() == 0 )
					Effect->ObjPtr()->SetDisabled( false );
				else
					Effect->ObjPtr()->SetDisabled( false );
				break;
			case 26:
				s->SetUsingPotion( false );
				break;
			case 27:
				src = calcCharObjFromSer( Effect->Source() );
				targ = calcCharObjFromSer( Effect->Destination() );
				Magic->playSound( src, 43 );
				Magic->doMoveEffect( 43, targ, src );
				Magic->doStaticEffect( targ, 43 );
				Magic->MagicDamage( targ, Effect->More1(), src );
				equipCheckNeeded = true;
				break;

			case 40:
			{
				UI16 scpNum			= 0xFFFF;
				cScript *tScript	= Effect->AssocScript();

				if( Effect->Source() >= BASEITEMSERIAL )	// item's have serials of 0x40000000 and above, and we already know it's not INVALIDSERIAL
				{
					myObj = calcItemObjFromSer( Effect->Source() );
					equipCheckNeeded = false;
				}
				else
				{
					myObj = calcCharObjFromSer( Effect->Source() );
					equipCheckNeeded = true;
				}
				if( myObj == NULL )	// item no longer exists!
					break;
				if( tScript == NULL )	// No associated script, so it must be another callback variety
				{
					if( Effect->More2() != 0xFFFF )
						scpNum = Effect->More2();
					else
						scpNum = myObj->GetScriptTrigger();
					tScript = Trigger->GetScript( scpNum );
				}
				if( tScript == NULL && Effect->Source() >= BASEITEMSERIAL )
				{
					if( Trigger->CheckEnvoke( myObj->GetID() ) )
					{
						scpNum	= Trigger->GetScriptFromEnvoke( myObj->GetID() );
						tScript	= Trigger->GetScript( scpNum );
					}
				}
				if( tScript != NULL )				// do OnTimer stuff here
					tScript->OnTimer( myObj, static_cast<UI08>(Effect->More1()) );
				break;
			}
			case 41:	// Creating an item
				HandleMakeItemEffect( Effect );
				break;
			case 42:
				src = calcCharObjFromSer( Effect->Source() );
				PlaySound( src, Effect->More2() );
				break;
			case 43:
				src = calcCharObjFromSer( Effect->Source() );
				if( src->GetID() == 0xCF )
					break;

				src->SetID( 0xCF ); // Thats all we need to do
				break;
			case 44:	// untame pets if they are hungry
				src = calcCharObjFromSer( Effect->Source() );
				if( ValidateObject( src ) && src->GetHunger() <= 0 )
				{
					src->SetOwner( NULL );
					src->SetTamed( false );
					src->SetNpcWander( 2 );
				}
				break;
			default:
				Console.Error( 2, " Fallout of switch statement without default (%i). checktempeffects()", Effect->Number() );			
				return;
		}
		if( ValidateObject( s ) && equipCheckNeeded )
			Items->CheckEquipment( s ); // checks equipments for stat requirements
	}
	TEffects->Prune();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void reverseEffect( UI16 i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Reverse a temp effect
//o---------------------------------------------------------------------------o
void reverseEffect( CTEffect *Effect )
{
	CChar *s = calcCharObjFromSer( Effect->Destination() );
	if( ValidateObject( s ) )
	{
		switch( Effect->Number() )
		{
			case 1:
				s->SetFrozen( false );
				break;
			case 2:
				s->SetFixedLight( 255 );
				break;
			case 3:		s->IncDexterity2( Effect->More1() );		break;
			case 4:		s->IncIntelligence2( Effect->More1() );		break;
			case 5:		s->IncStrength2( Effect->More1() );			break;
			case 6:		s->IncDexterity2( -Effect->More1() );		break;
			case 7:		s->IncIntelligence2( -Effect->More1() );	break;
			case 8:		s->IncStrength2( -Effect->More1() );		break;
			case 11:
				s->IncStrength2( -Effect->More1() );
				s->IncDexterity2( -Effect->More2() );
				s->IncIntelligence2( -Effect->More3() );
				break;
			case 12:
				s->IncStrength2( Effect->More1() );
				s->IncDexterity2( Effect->More2() );
				s->IncIntelligence2( Effect->More3() );
				break;
			case 18: // Polymorph spell
				s->SetID( s->GetOrgID() );
				s->IsPolymorphed( false );
				break;
			case 19: // Incognito
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
				tSock = calcSocketObjFromChar( s );
				s->SendWornItems( tSock );
				s->IsIncognito( false );
				break;
			case 21:
				int toDrop;
				toDrop = Effect->More1() ;
				if( ( s->GetBaseSkill( PARRYING ) - toDrop ) < 0 )
					s->SetBaseSkill( 0, PARRYING );
				else
					s->SetBaseSkill( s->GetBaseSkill( PARRYING ) - toDrop, PARRYING );
				break;
			case 26:
				s->SetUsingPotion( false );
				break;
			default:
				Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, reverseEffect()");
				return;
		}
	}
	Items->CheckEquipment( s );
}

void cEffects::tempeffect( CChar *source, CChar *dest, UI08 num, UI16 more1, UI16 more2, UI16 more3, CItem *targItemPtr )
{
	if( !ValidateObject( source ) || !ValidateObject( dest ) )
		return;

	CTEffect *toAdd	= TEffects->CreateEffect();
	SERIAL sourSer	= source->GetSerial();
	SERIAL targSer	= dest->GetSerial();
	toAdd->Source( sourSer );
	toAdd->Destination( targSer );

	TEffects->StartQueue();
	for( CTEffect *Effect = TEffects->First(); !TEffects->AtEnd(); Effect = TEffects->Next() )
	{
		if( Effect->Destination() == targSer )
		{
			if( Effect->Number() == num )
			{
				switch( Effect->Number() )
				{
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 11:
					case 12:
					case 18:
					case 19:
					case 21:
						reverseEffect( Effect );
						TEffects->QueueToRemove( Effect );
						break;
					default:
						TEffects->QueueToKeep( Effect );
						break;
				}
			}
		}
	}
	TEffects->Prune();
	CSocket *tSock = calcSocketObjFromChar( dest );
	toAdd->Number( num );
	switch( num )
	{
		case 1:
			dest->SetFrozen( true );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 100.0f ) );
			toAdd->Dispellable( true );
			break;
		case 2:
			SI16 worldbrightlevel;
			worldbrightlevel = cwmWorldState->ServerData()->WorldLightBrightLevel();
			dest->SetFixedLight( static_cast<UI08>(worldbrightlevel) );
			doLight( tSock, static_cast<char>(worldbrightlevel) );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 2.0f ) );
			toAdd->Dispellable( true );
			break;
		case 3:
			if( dest->GetDexterity() < more1 )
				more1 = dest->GetDexterity();
			dest->IncDexterity2( -more1 );
			dest->SetStamina( UOX_MIN( dest->GetStamina(), dest->GetMaxStam() ) );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		case 4:
			if( dest->GetIntelligence() < more1 )
				more1 = dest->GetIntelligence();
			dest->IncIntelligence2( -more1 );
			dest->SetMana( UOX_MIN(dest->GetMana(), dest->GetMaxMana() ) );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		case 5:
			if( dest->GetStrength() < more1 )
				more1 = dest->GetStrength();
			dest->IncStrength2( -more1 );
			dest->SetHP( UOX_MIN( dest->GetHP(), static_cast<SI16>(dest->GetMaxHP()) ) );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		case 6:
			dest->IncDexterity( more1 );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		case 7:
			dest->IncIntelligence2( more1 );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		case 8:
			dest->IncStrength2( more1 );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->Dispellable( true );
			break;
		case 9:
			toAdd->ExpireTime( BuildTimeValue( (R32)more2 ) );
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			toAdd->Dispellable( false );
			break;
		case 10:
			toAdd->ExpireTime( BuildTimeValue( 12.0f ) );
			toAdd->Dispellable( false );
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			break;
		case 11: // Bless
			dest->IncStrength2( more1 );
			dest->IncDexterity2( more2 );
			dest->IncIntelligence2( more3 );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			toAdd->More3( more3 );
			toAdd->Dispellable( true );
			break;
		case 12: // Curse
			if( dest->GetStrength() < more1 )
				more1 = dest->GetStrength();
			if( dest->GetDexterity() < more2 )
				more2 = dest->GetDexterity();
			if( dest->GetIntelligence() < more3 )
				more3 = dest->GetIntelligence();
			dest->IncStrength2( -more1 );
			dest->IncDexterity2( -more2 );
			dest->IncIntelligence2( -more3 );
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			toAdd->More3( more3 );
			toAdd->Dispellable( true );
			break;
		case 15: // Reactive armor
			toAdd->ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
			toAdd->Dispellable( true );
			break;
		case 16: //Explosion potions
			toAdd->ExpireTime( BuildTimeValue( (R32)more2 ) );
			toAdd->More1( more1 ); //item/potion
			toAdd->More2( more2 ); //seconds
			toAdd->More3( more3 ); //countdown#
			toAdd->Dispellable( false );
			break;
		case 18:	// Polymorph
			toAdd->ExpireTime( BuildTimeValue( (R32)cwmWorldState->ServerData()->PolyDuration() ) );
			toAdd->Dispellable( false );
			
			UI16 k;
			// Grey flag when polymorphed
			dest->SetTimer( tCHAR_CRIMFLAG, BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->PolyDuration()) ) );
			if( dest->IsOnHorse() ) 
				DismountCreature( dest );
			k = ( more1<<8 ) + more2;
			
			if( k <= 0x03e1 ) // lord binary, body-values >0x3e1 crash the client
				dest->SetID( k );
			break;
		case 19: // incognito spell - AntiChrist (10/99)
			toAdd->ExpireTime( BuildTimeValue( 90.0f ) ); // 90 seconds
			toAdd->Dispellable( false );
			break;
			
		case 21:		// protection
			toAdd->ExpireTime( BuildTimeValue( 120.0f ) );
			toAdd->Dispellable( true );
			toAdd->More1( more1 );
			dest->SetBaseSkill( dest->GetBaseSkill( PARRYING ) + more1, PARRYING );
			break;
		case 22:		// healing skill, normal heal
		case 23:		// healing skill, resurrect
		case 24:		// healing skill, cure
			CTEffect *Test;
			CChar *oldTarg;
			if( source->SkillUsed( static_cast<UI08>(more1) ) )
			{
				TEffects->StartQueue();
				for( Test = TEffects->First(); !TEffects->AtEnd(); Test = TEffects->Next() )	// definitely not friendly and scalable, but it stops all prior healing attempts
				{	
					// another option would be to do a bit set, to specify already healing
					if( Test == NULL )
						continue;
					switch( Test->Number() )
					{
						case 22:
						case 23:
						case 24:
							if( Test->Source() == sourSer )
							{
								oldTarg = calcCharObjFromSer( Test->Destination() );
								if( Test->Number() == 22 )
									source->emoteAll( 1275, false, source->GetName().c_str(), oldTarg->GetName().c_str() );
								else if( Test->Number() == 23 )
									source->emoteAll( 1276, false, source->GetName().c_str(), oldTarg->GetName().c_str() );
								else if( Test->Number() == 24 )
									source->emoteAll( 1277, false, source->GetName().c_str(), oldTarg->GetName().c_str() );
								TEffects->QueueToRemove( Test );		// we're already involved in some form of healing, BREAK IT
							}
							else
								TEffects->QueueToKeep( Test );
							break;
						default:
							TEffects->QueueToKeep( Test );
							break;
					}
				}
				TEffects->Prune();
			}
			if( num == 22 )
			{
				if( dest == source )
					toAdd->ExpireTime( BuildTimeValue( 18.0f - (R32)RandomNum( 0, (int)(source->GetSkill( HEALING ) / 200.0f) ) ) );
				else
					toAdd->ExpireTime( BuildTimeValue( 7.0f - (R32)RandomNum( 0, (int)(source->GetSkill( HEALING ) / 333.0f) ) ) );
				source->emoteAll( 1278, false, source->GetName().c_str(), dest->GetName().c_str() );
			}
			else if( num == 23 )
			{
				toAdd->ExpireTime( BuildTimeValue( 15.0f ) );
				source->emoteAll( 1279, false, source->GetName().c_str(), dest->GetName().c_str() );
			}
			else if( num == 24 )
			{
				if( dest == source )
					toAdd->ExpireTime( BuildTimeValue( (R32)RandomNum( 15000, 18000 ) / 1000.0f ) );
				else
					toAdd->ExpireTime( BuildTimeValue( 6.0f ) );
				source->emoteAll( 1280, false, source->GetName().c_str(), dest->GetName().c_str() );
			}
			toAdd->Dispellable( false );
			toAdd->ObjPtr( targItemPtr );	// the bandage we are using to achieve this
			toAdd->More1( more1 );			// the skill we end up using (HEALING for players, VETERINARY for monsters)
			source->SkillUsed( true, static_cast<UI08>(more1) );
			break;
		case 25:
			toAdd->ExpireTime( BuildTimeValue( (R32)more1 ) );
			toAdd->ObjPtr( dest );
			dest->SetDisabled( true );
			toAdd->More2( 1 );
			break;
		case 26:
			toAdd->ExpireTime( BuildTimeValue( cwmWorldState->ServerData()->PotionDelay() ) );
			dest->SetUsingPotion( true );
			break;
		case 27:
			toAdd->ExpireTime( BuildTimeValue( cwmWorldState->ServerData()->CombatExplodeDelay() ) );
			toAdd->More1( more1 );
			break;
		case 40:
			toAdd->ExpireTime( BuildTimeValue( ( (R32)more1 + (R32)more2 / 1000.0f ) ) );
			toAdd->More1( more3 );
			break;
		case 41:	// creating item
			toAdd->ExpireTime( BuildTimeValue( (R32)(more1) / 100.0f ) );
			toAdd->More2( more2 );
			break;
		case 42:	// delayed sound effect
			toAdd->ExpireTime( BuildTimeValue( (R32)(more1) / 100.0f ) );
			toAdd->More2( more2 );
			break;
		case 43:	// regain wool for sheeps
			toAdd->ExpireTime( BuildTimeValue( (R32)(more1) ) );
			break;
		case 44:	// untame hungry pets
			toAdd->Source( source->GetSerial() );
			toAdd->Destination( source->GetSerial() );
			toAdd->Dispellable( false );
			toAdd->ExpireTime( BuildTimeValue( (R32)RandomNum( 0, 1800 ) ) ); // set time between 0 and 30 minutes
			break;
		default:
			Console.Error( 2, " Fallout of switch statement (%d) without default. uox3.cpp, tempeffect()", num );
			return;
	}

	TEffects->Add( toAdd );
}

void cEffects::tempeffect( CChar *source, CItem *dest, UI08 num, UI16 more1, UI16 more2, UI16 more3 )
{
	if( !ValidateObject( source ) || !ValidateObject( dest ) )
		return;

	CTEffect *toAdd = TEffects->CreateEffect();
	toAdd->Source( source->GetSerial() );
	toAdd->Destination( dest->GetSerial() );
	toAdd->Number( num );
	switch( num )
	{
		case 10:
			toAdd->ExpireTime( BuildTimeValue( 12 ) );
			toAdd->Dispellable( false );
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			source->SkillUsed( true, ALCHEMY );
			break;
		case 13:
			if( dest->isDoorOpen() )
			{
				dest->SetDoorOpen( false );
				return;
			}
			toAdd->ExpireTime( BuildTimeValue( 10 ) );
			toAdd->Dispellable( false );
			dest->SetDoorOpen( true );
			break;
		case 14: //training dummies swing for 5(?) seconds
			toAdd->ExpireTime( BuildTimeValue( 5 ) );
			toAdd->Dispellable( false );
			toAdd->ObjPtr( dest ); //used to try and cut search time down
			toAdd->More2( 0 );
			break;
		case 17: //Explosion potion (explosion)  Tauriel (explode in 4 seconds)
			toAdd->ExpireTime( BuildTimeValue( 4 ) );
			toAdd->More1( more1 );
			toAdd->More2( more2 );
			toAdd->ObjPtr( dest );
			toAdd->Dispellable( false );
			break;
		case 25:
			toAdd->ExpireTime( BuildTimeValue( more1 ) );
			toAdd->ObjPtr( dest );
			dest->SetDisabled( true );
			toAdd->More2( 0 );
			break;
		default:
			Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, tempeffect2()");
			return;
	}
	TEffects->Add( toAdd );
}

//o--------------------------------------------------------------------------
//|	Function		-	SaveEffects()
//|	Date			-	16 March, 2002
//|	Programmer		-	sereg
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Saves out the TempEffects
//o--------------------------------------------------------------------------
void cEffects::SaveEffects( void )
{
	std::ofstream writeDestination, effectDestination;
	const char blockDiscriminator[] = "\n\n---EFFECT---\n\n";
	int s_t							= getclock();

	Console << "Saving Effects...   ";
	Console.TurnYellow();

	std::string filename = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "effects.wsc";
	effectDestination.open( filename.c_str() );
	if( !effectDestination )
	{
		Console.Error( 1, "Failed to open %s for writing", filename.c_str() );
		return;
	}

	for( CTEffect *currEffect = TEffects->First(); currEffect != NULL; currEffect = TEffects->Next() )
	{
		currEffect->Save( effectDestination );
		effectDestination << blockDiscriminator;
	}
	effectDestination.close();

	Console << "\b\b\b\b";
	Console.PrintDone();

	int e_t = getclock();
	Console.Print( "Effects saved in %.02fsec\n", ((float)(e_t-s_t))/1000.0f );
}

//o--------------------------------------------------------------------------
//|	Function		-	LoadEffects( void )
//|	Date			-	16 March, 2002
//|	Programmer		-	sererg
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Loads in Effects from disk
//o--------------------------------------------------------------------------
void ReadWorldTagData( std::ifstream &inStream, UString &tag, UString &data );
void cEffects::LoadEffects( void )
{
	std::ifstream input;
	std::string filename = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "effects.wsc";
	CTEffect *toLoad;

	input.open( filename.c_str(), std::ios_base::in );
	char line[1024];

	UString tag = "o---o", data, UTag;

	if( input.is_open() )
	{
		while( !input.eof() && !input.fail() )
		{
			input.getline( line, 1024 );
			UString sLine( line );
			sLine = sLine.removeComment().stripWhiteSpace();
			if( !sLine.empty() )
			{
				if( sLine.upper() == "[EFFECT]" )
				{
					toLoad = TEffects->CreateEffect();
					while( !input.eof() && !input.fail() )
					{
						if( sLine != "o---o" )
						{
							ReadWorldTagData( input, tag, data );
							if( tag != "o---o" )
							{
								UTag = tag.upper();
								switch( (tag.data()[0]) )
								{
									case 'D':
										if( UTag == "DEST" )
											toLoad->Destination( data.toULong() );
										if( UTag == "DISPEL" )
											toLoad->Dispellable( ( (data.toULong() == 0) ? false : true ) );
										break;
									case 'E':
										if( UTag == "EXPIRE" )
											toLoad->ExpireTime( static_cast<UI32>(data.toULong() + cwmWorldState->GetUICurrentTime()) );
										break;
									case 'I':
										if( UTag == "ITEMPTR" )
										{
											SERIAL objSer = data.toULong();
											if( objSer != INVALIDSERIAL )
											{
												if( objSer < BASEITEMSERIAL )
													toLoad->ObjPtr( calcCharObjFromSer( objSer ) );
												else
													toLoad->ObjPtr( calcItemObjFromSer( objSer ) );
											}
											else
												toLoad->ObjPtr( NULL );
										}
										break;
									case 'M':
										if( UTag == "MORE1" )
											toLoad->More1( data.toUShort() );
										if( UTag == "MORE2" )
											toLoad->More2( data.toUShort() );
										if( UTag == "MORE3" )
											toLoad->More3( data.toUShort() );
										break;
									case 'N':
										if( UTag == "NUMBER" )
											toLoad->Number( data.toUByte() );
										break;
									case 'O':
										if( UTag == "OBJPTR" )
										{
											SERIAL objSer = data.toULong();
											if( objSer != INVALIDSERIAL )
											{
												if( objSer < BASEITEMSERIAL )
													toLoad->ObjPtr( calcCharObjFromSer( objSer ) );
												else
													toLoad->ObjPtr( calcItemObjFromSer( objSer ) );
											}
											else
												toLoad->ObjPtr( NULL );
										}
									case 'S':
										if( UTag == "SOURCE" )
											toLoad->Source( data.toULong() );
										break;
									default:
										Console.Error( 1, "Unknown effects tag %s with contents of %s", tag.c_str(), data.c_str() );
										break;
								}
							}
						}
						else
							break;
					}
					TEffects->Add( toLoad );
				}
				else
					break;
			}
		}
		input.close();
	}
}

}
