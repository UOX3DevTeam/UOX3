#include "uox3.h"
#include "skills.h"
#include "cMagic.h"
#include "trigger.h"
#include "mapstuff.h"
#include "cScript.h"
#include "cEffects.h"
#include "teffect.h"
#include "packets.h"


cEffects::cEffects()
{
}

cEffects::~cEffects()
{
}

//o---------------------------------------------------------------------------o
//|	Function	-	void deathAction( CChar *s, CItem *x )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Plays a characters death animation
//o---------------------------------------------------------------------------o
void cEffects::deathAction( CChar *s, CItem *x )
{
	CPDeathAction toSend( (*s), (*x) );
	toSend.FallDirection( (UI08)RandomNum( 0, 1 ) );
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( tSock->CurrcharObj() != s && charInRange( s, tSock->CurrcharObj() ) )
			tSock->Send( &toSend );
	}
	Network->PopConn();
}

void cEffects::movingeffect( cBaseObject *source, cBaseObject *dest, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 hue, UI32 renderMode )
{	//0x0f 0x42 = arrow 0x1b 0xfe=bolt
	if( source == NULL || dest == NULL ) 
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
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( objInRange( tSock, source, 18 ) && objInRange( tSock, dest, 18 ) )
			tSock->Send( &toSend );
	}
	Network->PopConn();
}

void cEffects::movingeffect( cBaseObject *source, SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode, UI32 hue, UI32 renderMode )
{	//0x0f 0x42 = arrow 0x1b 0xfe=bolt
	if( source == NULL ) 
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
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( objInRange( tSock, source, 18 ) )
			tSock->Send( &toSend );
	}
	Network->PopConn();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void action( cSocket *s, SI16 x )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Character does a certain action
//o---------------------------------------------------------------------------o
void cEffects::action( cSocket *s, SI16 x )
{
	CChar *mChar = s->CurrcharObj();
	
	CPCharacterAnimation toSend = (*mChar);
	toSend.Action( x );
	s->Send( &toSend );
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( tSock != s && charInRange( mChar, tSock->CurrcharObj() ) )
			tSock->Send( &toSend );
	}
	Network->PopConn();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void impaction( cSocket *s, SI16 act )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handles spellcasting action
//o---------------------------------------------------------------------------o
void cEffects::impaction( cSocket *s, SI16 act )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar->IsOnHorse() && ( act == 0x10 || act == 0x11 ) )
	{
		action( s, 0x1B );
		return;
	}
	if( ( mChar->IsOnHorse() || !mChar->isHuman() ) && act == 0x22 )
		return;
	action( s, act );
}


void cEffects::staticeffect( cBaseObject *target, UI16 effect, UI08 speed, UI08 loop, bool explode )
{
	if( target == NULL ) 
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
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( objInRange( tSock->CurrcharObj(), target, 18 ) )
			tSock->Send( &toSend );
	}
	Network->PopConn();
}

// for effects on items
void cEffects::staticeffect( SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode )
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
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{  // if inrange of effect and online send effect
		tSock->Send( &toSend );
	}
	Network->PopConn();
}

void cEffects::bolteffect( CChar *player )
{
	if( player == NULL ) 
		return;

	CPGraphicalEffect toSend( 1, (*player) );
	toSend.SourceLocation( (*player) );
	toSend.ExplodeOnImpact( false );
	toSend.AdjustDir( false );
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( charInRange( tSock->CurrcharObj(), player ) )
			tSock->Send( &toSend );
	}
	Network->PopConn();
}

void cEffects::tempeffectsoff( void )
{
	CChar *s = NULL;
	cBaseObject *myObj = NULL;
	bool equipCheckNeeded = false;

	for( teffect_st *Effect = TEffects->First(); !TEffects->AtEnd(); Effect = TEffects->Next() )
	{
		s = calcCharObjFromSer( Effect->Destination() );
		if( s != NULL )
		{
			switch( Effect->Number() )
			{
			case 1:	s->SetFrozen( false );					break;
			case 2:	s->SetFixedLight( 255 );					break;
			case 3:	s->IncDexterity2( Effect->More1() );		break;
			case 4:	s->IncIntelligence2( Effect->More1() );	break;
			case 5:	s->IncStrength2( Effect->More1() );		break;
			case 6:	s->IncDexterity2( -Effect->More1() );		break;
			case 7:	s->IncIntelligence2( -Effect->More1() );	break;
			case 8:	s->IncStrength2( -Effect->More1() );		break;
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
			case 15:
				s->SetReactiveArmour( false );
				break;
			case 26:
				s->SetUsingPotion( false );
				break;
			case 40:
			{
				UI16 scpNum = 0xFFFF;
	
				if( Effect->Source() >= BASEITEMSERIAL )	// item's have serials of 0x40000000 and above, and we already know it's not INVALIDSERIAL
				{
					myObj = calcItemObjFromSer( Effect->Source() );
					scpNum = myObj->GetScriptTrigger();
					equipCheckNeeded = false;
				}
				else
				{
					myObj = calcCharObjFromSer( Effect->Source() );
					if( Effect->More2() != 0xFFFF )
						scpNum = Effect->More2();
					else
						scpNum = myObj->GetScriptTrigger();
					equipCheckNeeded = true;
				}
				if( myObj == NULL )	// item no longer exists!
					break;
				cScript *tScript = Trigger->GetScript( scpNum );
				if( tScript == NULL && Effect->Source() >= BASEITEMSERIAL )
				{
					if( Trigger->CheckEnvoke( myObj->GetID() ) )
					{
						scpNum	= Trigger->GetScriptFromEnvoke( myObj->GetID() );
						tScript	= Trigger->GetScript( scpNum );
					}
				}
				if( tScript != NULL )
				{	// script exists
					tScript->OnTimer( myObj, static_cast<UI08>(Effect->More1()) );
					// do OnTimer stuff here
				}
				TEffects->DelCurrent();
				if( s != NULL && equipCheckNeeded )
					Items->CheckEquipment( s ); // checks equipments for stat requirements
				break;
			}
			default:
				Console.Error( 2, "tempeffectsoff() Unknown effect %i on character serial %i", Effect->Number(), s->GetSerial() ); //Morrolan
				break;
			}
		}
	}
}

void cEffects::tempeffectson( void )
{
	CChar *s = NULL;
	for( teffect_st *Effect = TEffects->First(); !TEffects->AtEnd(); Effect = TEffects->Next() )
	{
		s = calcCharObjFromSer( Effect->Destination() );
		if( s != NULL )
		{
			switch( Effect->Number() )
			{
			case 1:	s->SetFrozen( true );						break;
			case 2:	s->SetFixedLight( static_cast<UI08>(cwmWorldState->ServerData()->GetWorldLightBrightLevel() ));	break;
			case 3:	s->IncDexterity2( -Effect->More1() );		break;
			case 4:	s->IncIntelligence2( -Effect->More1() );	break;
			case 5:	s->IncStrength2( -Effect->More1() );		break;
			case 6:	s->IncDexterity2( Effect->More1() );		break;
			case 7:	s->IncIntelligence2( Effect->More1() );	break;
			case 8:	s->IncStrength2( Effect->More1() );		break;
			case 11:
				s->IncStrength2( Effect->More1() );
				s->IncDexterity2( Effect->More2() );
				s->IncIntelligence2( Effect->More3() );
				break;
			case 12:
				s->IncStrength2( -Effect->More1() );
				s->IncDexterity2( -Effect->More2() );
				s->IncIntelligence2( -Effect->More3() );
				break;
			case 15:
				s->SetReactiveArmour( true );
				break;
			default:
				Console.Error( 2, " Fallout of switch statement without default. tempeffectson()");
				break;
			}
		}
	}
}

void cEffects::HandleMakeItemEffect( teffect_st *tMake )
{
	if( tMake == NULL )
		return;
	CChar *src = calcCharObjFromSer( tMake->Source() );
	UI16 iMaking = tMake->More2();
	createEntry *toMake = Skills->FindItem( iMaking );
	if( toMake == NULL )
		return;

	cSocket *sock = calcSocketObjFromChar( src );
	// Create the item in our backpack
	CItem *targItem = Items->SpawnItemToPack( sock, src, toMake->addItem, false );
	for( UI32 skCounter = 0; skCounter < toMake->skillReqs.size(); skCounter++ )
		src->SkillUsed( false, toMake->skillReqs[skCounter].skillNumber );
	if( targItem == NULL )
	{
		Console.Error( 2, "cSkills::MakeItem() bad script item # %s, made by player %i", toMake->addItem.c_str(), src->GetSerial() );
		return;
	}
	else
	{
		targItem->SetName2( targItem->GetName() );
		SI32 rank = Skills->CalcRankAvg( src, (*toMake) );
		Skills->ApplyRank( sock, targItem, static_cast<UI08>(rank) );
		
		// if we're not a GM, see if we should store our creator
		if( !src->IsGM() && toMake->skillReqs.size() > 0 )
		{
			targItem->SetCreator( src->GetSerial() );
			int avgSkill, sumSkill = 0;
			// Find the average of our player's skills
			for( UI32 resCounter = 0; resCounter < toMake->skillReqs.size(); resCounter++ )
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

	sysmessage( sock, 985 );
	statwindow( sock, src );
}

void cEffects::checktempeffects( void )
{
	CItem *i = NULL;
	CChar *s = NULL, *src = NULL, *targ = NULL;
	cSocket *tSock = NULL;
	cBaseObject *myObj = NULL;

	UI32 j = cwmWorldState->GetUICurrentTime();
	char temp[1024];
	for( teffect_st *Effect = TEffects->First(); !TEffects->AtEnd(); Effect = TEffects->Next() )
	{
		if( Effect == NULL )
			continue;
		if( Effect->ExpireTime() > j )
			continue;

		if( Effect->Destination() < BASEITEMSERIAL )
		{
			s = calcCharObjFromSer( Effect->Destination() );
			if( s == NULL )
				continue;
			tSock = calcSocketObjFromChar( s );
		}

		bool equipCheckNeeded = false;

		switch( Effect->Number() )
		{
		case 1:
			if( s->IsFrozen() )
			{
				s->SetFrozen( false );
				sysmessage( tSock, 700 );
			}
			break;
		case 2:
			s->SetFixedLight( 255 );
			doLight( tSock, static_cast<char>(cwmWorldState->ServerData()->GetWorldLightCurrentLevel() ));
			break;
		case 3:
			s->IncDexterity2( Effect->More1() );
			statwindow( tSock, s );
			equipCheckNeeded = true;
			break;
		case 4:
			s->IncIntelligence2( Effect->More1() );
			statwindow( tSock, s );
			equipCheckNeeded = true;
			break;
		case 5:
			s->IncStrength2( Effect->More1() );
			statwindow( tSock, s );
			equipCheckNeeded = true;
			break;
		case 6:
			s->IncDexterity2( -Effect->More1() );
			s->SetStamina( min( s->GetStamina(), s->GetMaxStam() ) );
			statwindow( tSock, s );
			equipCheckNeeded = true;
			break;
		case 7:
			s->IncIntelligence2( -Effect->More1() );
			s->SetMana( min( s->GetMana(), s->GetMaxMana() ) );
			statwindow( tSock, s );
			equipCheckNeeded = true;
			break;
		case 8:
			s->IncStrength2( -Effect->More1() );
			s->SetHP( min( s->GetHP(), s->GetMaxHP() ) );
			statwindow( tSock, s );
			equipCheckNeeded = true;
			break;
		case 9:	// Grind potion (also used for necro stuff)
			switch( Effect->More1() )
			{
			case 0:
				if( Effect->More2() != 0 )
					npcEmoteAll( s, 1270, true, s->GetName() );
				PlaySound( s, 0x0242 );
				break;
			}
			break;
/*		case 10:		// Pour potion into bottle
			src = calcCharObjFromSer( Effect->Source() );
			mortar = calcItemObjFromSer( Effect->Destination() );
			Skills->CreatePotion( src, Effect->More1(), Effect->More2(), mortar );
			src->SkillUsed( false, ALCHEMY );
			break;
*/		case 11:
			s->IncStrength2( -Effect->More1() );
			s->SetHP(  min( s->GetHP(), s->GetMaxHP()) );
			s->IncDexterity2( -Effect->More2() );
			s->SetStamina( min( s->GetStamina(), s->GetMaxStam() ) );
			s->IncIntelligence2( -Effect->More3() );
			s->SetMana( min( s->GetMana(), s->GetMaxMana() ) );
			statwindow( tSock, s );
			equipCheckNeeded = true;
			break;
		case 12:
			s->IncStrength2( Effect->More1() );
			s->IncDexterity2( Effect->More2() );
			s->IncIntelligence2( Effect->More3() );
			statwindow( tSock, s );
			equipCheckNeeded = true;
			break;
		case 13: // door
			i = calcItemObjFromSer( Effect->Destination() );
			if( i == NULL )
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
				RefreshItem( i );
			} 
			else if( i->GetID() == 0x1075 )
			{
				i->IncID( -1 );
				i->SetGateTime( 0 );
				RefreshItem( i );
			} 
			break;
		case 15: //reactive armor
			s->SetReactiveArmour( false );
			break;
		case 16: //Explosion potion messages
			src = calcCharObjFromSer( Effect->Source() );
			if( src->GetAntiSpamTimer() < cwmWorldState->GetUICurrentTime() )
			{
				src->SetAntiSpamTimer( BuildTimeValue( 1 ) );
				sprintf( temp, "%i", Effect->More3() );
				sysmessage( tSock, temp );
			} 
			break;
		case 17: //Explosion potion
			src = calcCharObjFromSer( Effect->Source() );
			explodeItem( calcSocketObjFromChar( src ), (CItem *)Effect->ObjPtr() ); //explode this item
			break;
		case 18: //Polymorph spell
			s->SetID( s->GetOrgID() );
			s->Teleport();
			s->IsPolymorphed( false );
			break;
		case 19: //Incognito spell
			s->SetID( s->GetOrgID() );
			
			// ------ NAME -----
			s->SetName( s->GetOrgName() );
			
			i = s->GetItemAtLayer( 0x0B );
			if( i != NULL ) 
			{
				i->SetColour( s->GetHairColour() );
				i->SetID( s->GetHairStyle() );
			}
			i = s->GetItemAtLayer( 0x10 );
			if( i != NULL && s->GetID( 2 ) == 0x90 )
			{
				i->SetColour( s->GetBeardColour() );
				i->SetID( s->GetBeardStyle() );
			}
			if( tSock != NULL )
				wornItems( tSock, s );
			s->Teleport();
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
			if( src != NULL && targ != NULL )
			{
				if( src->SkillUsed( static_cast<UI08>(Effect->More1()) ) )
				{
					cSocket *srcSock = calcSocketObjFromChar( src );
					cSocket *targSock = calcSocketObjFromChar( targ );
					if( Effect->Number() == 22 )
					{
						newHealth = static_cast<R32>(targ->GetHP() + ( src->GetSkill( ANATOMY ) / 50 + RandomNum( 3, 10 ) + RandomNum( src->GetSkill( HEALING ) / 50, src->GetSkill( HEALING ) / 20 ) ));
						targ->SetHP( min( targ->GetMaxHP(), (SI16)newHealth ) );
						updateStats( targ, 0 );
						sysmessage( srcSock, 1271 );
					}
					else if( Effect->Number() == 23 )
					{
						Targ->NpcResurrectTarget( targ );
						sysmessage( srcSock, 1272 );
					}
					else
					{
						targ->SetPoisoned( 0 );
						if( targSock != NULL )
						{
							staticeffect( targ, 0x373A, 0, 15 );
							PlaySound( targSock, 0x01E0, false );
							sysmessage( targSock, 1273 );
							targ->SendToSocket( targSock, true, targ );
						}
						if( srcSock != NULL )
							sysmessage( srcSock,  1274 );
					}
				}
				src->SkillUsed( false, static_cast<UI08>(Effect->More1()) );

			}
			DecreaseItemAmount( i );
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
			UI16 scpNum = 0xFFFF;

			if( Effect->Source() >= BASEITEMSERIAL )	// item's have serials of 0x40000000 and above, and we already know it's not INVALIDSERIAL
			{
				myObj = calcItemObjFromSer( Effect->Source() );
				scpNum = myObj->GetScriptTrigger();
				equipCheckNeeded = false;
			}
			else
			{
				myObj = calcCharObjFromSer( Effect->Source() );
				if( Effect->More2() != 0xFFFF )
					scpNum = Effect->More2();
				else
					scpNum = myObj->GetScriptTrigger();
				equipCheckNeeded = true;
			}
			if( myObj == NULL )	// item no longer exists!
				break;
			cScript *tScript = Trigger->GetScript( scpNum );
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
			src->Teleport();
			break;
		case 44:	// untame pets if they are hungry
			src = calcCharObjFromSer( Effect->Source() );
			if( src != NULL && src->GetHunger() <= 0 )
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
		TEffects->DelCurrent();
		if( s != NULL && equipCheckNeeded )
			Items->CheckEquipment( s ); // checks equipments for stat requirements
	}
}

void cEffects::tempeffect( CChar *source, CChar *dest, SI08 num, UI16 more1, UI16 more2, UI16 more3, CItem *targItemPtr )
{
	teffect_st toAdd;
	teffect_st *Effect = NULL;

	SERIAL targSer = INVALIDSERIAL, sourSer = INVALIDSERIAL;
	if( source != NULL )
		sourSer = source->GetSerial();
	toAdd.Source( sourSer );
	if( dest != NULL )
		targSer = dest->GetSerial();
	toAdd.Destination( targSer );

	UI16 ic;
	for( Effect = TEffects->First(), ic = 0; !TEffects->AtEnd(); Effect = TEffects->Next(), ic++ )
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
					reverseEffect( ic );
					break;
				default:
					break;
				}
			}
		}
	}
	cSocket *tSock = calcSocketObjFromChar( dest );
	toAdd.Number( num );
	switch( num )
	{
	case 1:
		dest->SetFrozen( true );
		toAdd.ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 100.0f ) );
		toAdd.Dispellable( true );
		break;
	case 2:
		SI16 worldbrightlevel;
		worldbrightlevel = cwmWorldState->ServerData()->GetWorldLightBrightLevel();
		dest->SetFixedLight( static_cast<UI08>(worldbrightlevel) );
		doLight( tSock, static_cast<char>(worldbrightlevel) );
		toAdd.ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 2.0f ) );
		toAdd.Dispellable( true );
		break;
	case 3:
		if( dest->GetDexterity() < more1 )
			more1 = dest->GetDexterity();
		dest->IncDexterity2( -more1 );
		dest->SetStamina( min( dest->GetStamina(), dest->GetMaxStam() ) );
		statwindow( tSock, dest );
		toAdd.ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
		toAdd.More1( more1 );
		toAdd.Dispellable( true );
		break;
	case 4:
		if( dest->GetIntelligence() < more1 )
			more1 = dest->GetIntelligence();
		dest->IncIntelligence2( -more1 );
		dest->SetMana( min(dest->GetMana(), dest->GetMaxMana() ) );
		statwindow( tSock, dest );
		toAdd.ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
		toAdd.More1( more1 );
		toAdd.Dispellable( true );
		break;
	case 5:
		if( dest->GetStrength() < more1 )
			more1 = dest->GetStrength();
		dest->IncStrength2( -more1 );
		dest->SetHP( min( dest->GetHP(), dest->GetMaxHP() ) );
		statwindow( tSock, dest );
		toAdd.ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
		toAdd.More1( more1 );
		toAdd.Dispellable( true );
		break;
	case 6:
		dest->IncDexterity( more1 );
		statwindow( tSock, dest );
		toAdd.ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
		toAdd.More1( more1 );
		toAdd.Dispellable( true );
		break;
	case 7:
		dest->IncIntelligence2( more1 );
		statwindow( tSock, dest );
		toAdd.ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
		toAdd.More1( more1 );
		toAdd.Dispellable( true );
		break;
	case 8:
		dest->IncStrength2( more1 );
		statwindow( tSock, dest );
		toAdd.ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
		toAdd.More1( more1 );
		toAdd.Dispellable( true );
		break;
	case 9:
		toAdd.ExpireTime( BuildTimeValue( (R32)more2 ) );
		toAdd.More1( more1 );
		toAdd.More2( more2 );
		toAdd.Dispellable( false );
		break;
	case 10:
		toAdd.ExpireTime( BuildTimeValue( 12.0f ) );
		toAdd.Dispellable( false );
		toAdd.More1( more1 );
		toAdd.More2( more2 );
		break;
	case 11: // Bless
		dest->IncStrength2( more1 );
		dest->IncDexterity2( more2 );
		dest->IncIntelligence2( more3 );
		statwindow( tSock, dest );
		toAdd.ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
		toAdd.More1( more1 );
		toAdd.More2( more2 );
		toAdd.More3( more3 );
		toAdd.Dispellable( true );
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
		statwindow( tSock, dest );
		toAdd.ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
		toAdd.More1( more1 );
		toAdd.More2( more2 );
		toAdd.More3( more3 );
		toAdd.Dispellable( true );
		break;
	case 15: // Reactive armor
		toAdd.ExpireTime( BuildTimeValue( (R32)source->GetSkill( MAGERY ) / 10.0f ) );
		toAdd.Dispellable( true );
		break;
	case 16: //Explosion potions
		toAdd.ExpireTime( BuildTimeValue( (R32)more2 ) );
		toAdd.More1( more1 ); //item/potion
		toAdd.More2( more2 ); //seconds
		toAdd.More3( more3 ); //countdown#
		toAdd.Dispellable( false );
		break;
	case 18:	// Polymorph
		toAdd.ExpireTime( BuildTimeValue( (R32)cwmWorldState->GetPolyDuration() ) );
		toAdd.Dispellable( false );
		
		UI16 k;
		// Grey flag when polymorphed
		dest->SetCrimFlag( BuildTimeValue( (R32)cwmWorldState->GetPolyDuration() ) );
		if( dest->IsOnHorse() ) 
			DismountCreature( tSock->CurrcharObj() );
		k = ( more1<<8 ) + more2;
		
		if( k <= 0x03e1 ) // lord binary, body-values >0x3e1 crash the client
		{ 
			dest->SetID( k );
			dest->SetxID( k );
		}
		break;
	case 19: // incognito spell - AntiChrist (10/99)
		toAdd.ExpireTime( BuildTimeValue( 90.0f ) ); // 90 seconds
		toAdd.Dispellable( false );
		break;
		
	case 21:		// protection
		toAdd.ExpireTime( BuildTimeValue( 120.0f ) );
		toAdd.Dispellable( true );
		toAdd.More1( more1 );
		dest->SetBaseSkill( dest->GetBaseSkill( PARRYING ) + more1, PARRYING );
		break;
	case 22:		// healing skill, normal heal
	case 23:		// healing skill, resurrect
	case 24:		// healing skill, cure
		teffect_st *Test;
		CHARACTER oldTarg;
		if( source->SkillUsed( static_cast<UI08>(more1) ) )
		{
			for( Test = TEffects->First(); !TEffects->AtEnd(); Test = TEffects->Next() )	// definitely not friendly and scalable, but it stops all prior healing attempts
			{	// another option would be to do a bit set, to specify already healing
				if( Test == NULL )
					continue;
				switch( Test->Number() )
				{
				case 22:
				case 23:
				case 24:
					if( Test->Source() == sourSer )
					{
						oldTarg = calcCharFromSer( Test->Destination() );
						if( Test->Number() == 22 )
							npcEmoteAll( source, 1275, false, source->GetName(), chars[oldTarg].GetName() );
						else if( Test->Number() == 23 )
							npcEmoteAll( source, 1276, false, source->GetName(), chars[oldTarg].GetName() );
						else if( Test->Number() == 24 )
							npcEmoteAll( source, 1277, false, source->GetName(), chars[oldTarg].GetName() );
						TEffects->DelCurrent();		// we're already involved in some form of healing, BREAK IT
					}
					break;
				default:
					break;
				}
			}
		}
		if( num == 22 )
		{
			if( dest == source )
				toAdd.ExpireTime( BuildTimeValue( 18.0f - (R32)RandomNum( 0, (int)(source->GetSkill( HEALING ) / 200.0f) ) ) );
			else
				toAdd.ExpireTime( BuildTimeValue( 7.0f - (R32)RandomNum( 0, (int)(source->GetSkill( HEALING ) / 333.0f) ) ) );
			npcEmoteAll( source, 1278, false, source->GetName(), dest->GetName() );
		}
		else if( num == 23 )
		{
			toAdd.ExpireTime( BuildTimeValue( 15.0f ) );
			npcEmoteAll( source, 1279, false, source->GetName(), dest->GetName() );
		}
		else if( num == 24 )
		{
			if( dest == source )
				toAdd.ExpireTime( BuildTimeValue( (R32)RandomNum( 15000, 18000 ) / 1000.0f ) );
			else
				toAdd.ExpireTime( BuildTimeValue( 6.0f ) );
			npcEmoteAll( source, 1280, false, source->GetName(), dest->GetName() );
		}
		toAdd.Dispellable( false );
		toAdd.ObjPtr( targItemPtr );	// the bandage we are using to achieve this
		toAdd.More1( more1 );			// the skill we end up using (HEALING for players, VETERINARY for monsters)
		source->SkillUsed( true, static_cast<UI08>(more1) );
		break;
	case 25:
		toAdd.ExpireTime( BuildTimeValue( (R32)more1 ) );
		toAdd.ObjPtr( dest );
		dest->SetDisabled( true );
		toAdd.More2( 1 );
		break;
	case 26:
		toAdd.ExpireTime( BuildTimeValue( cwmWorldState->ServerData()->GetPotionDelay() ) );
		dest->SetUsingPotion( true );
		break;
	case 27:
		toAdd.ExpireTime( BuildTimeValue( cwmWorldState->ServerData()->GetCombatExplodeDelay() ) );
		toAdd.More1( more1 );
		break;
	case 40:
		toAdd.ExpireTime( BuildTimeValue( ( (R32)more1 + (R32)more2 / 1000.0f ) ) );
		toAdd.More1( more3 );
		break;
	case 41:	// creating item
		toAdd.ExpireTime( BuildTimeValue( (R32)(more1) / 100.0f ) );
		toAdd.More2( more2 );
		break;
	case 42:	// delayed sound effect
		toAdd.ExpireTime( BuildTimeValue( (R32)(more1) / 100.0f ) );
		toAdd.More2( more2 );
		break;
	case 43:	// regain wool for sheeps
		toAdd.ExpireTime( BuildTimeValue( (R32)(more1) ) );
		break;
	case 44:	// untame hungry pets
		toAdd.Source( source->GetSerial() );
		toAdd.Destination( source->GetSerial() );
		toAdd.Dispellable( false );
		toAdd.ExpireTime( BuildTimeValue( (R32)RandomNum( 0, 1800 ) ) ); // set time between 0 and 30 minutes
		break;
	default:
		Console.Error( 2, " Fallout of switch statement (%d) without default. uox3.cpp, tempeffect()", num );
		return;
	}

	TEffects->Add( toAdd );
}

#pragma note( "Param Warning: in tempeffect(), more3 is unrefrenced" )
void cEffects::tempeffect( CChar *source, CItem *dest, SI08 num, UI16 more1, UI16 more2, UI16 more3 )
{
	teffect_st toAdd;
	if( source != NULL )
		toAdd.Source( source->GetSerial() );
	else
		toAdd.Source( INVALIDSERIAL );
	if( dest != NULL )
		toAdd.Destination( dest->GetSerial() );
	else
		toAdd.Destination( INVALIDSERIAL );

	toAdd.Number( num );
	switch( num )
	{
	case 10:
		toAdd.ExpireTime( BuildTimeValue( 12 ) );
		toAdd.Dispellable( false );
		toAdd.More1( more1 );
		toAdd.More2( more2 );
		source->SkillUsed( true, ALCHEMY );
		break;
	case 13:
		if( dest->isDoorOpen() )
		{
			dest->SetDoorOpen( false );
			return;
		}
		toAdd.ExpireTime( BuildTimeValue( 10 ) );
		toAdd.Dispellable( false );
		dest->SetDoorOpen( true );
		break;
	case 14: //training dummies swing for 5(?) seconds
		toAdd.ExpireTime( BuildTimeValue( 5 ) );
		toAdd.Dispellable( false );
		toAdd.ObjPtr( dest ); //used to try and cut search time down
		toAdd.More2( 0 );
		break;
	case 17: //Explosion potion (explosion)  Tauriel (explode in 4 seconds)
		toAdd.ExpireTime( BuildTimeValue( 4 ) );
		toAdd.More1( more1 );
		toAdd.More2( more2 );
		toAdd.ObjPtr( dest );
		toAdd.Dispellable( false );
		break;
	case 25:
		toAdd.ExpireTime( BuildTimeValue( more1 ) );
		toAdd.ObjPtr( dest );
		dest->SetDisabled( true );
		toAdd.More2( 0 );
		break;
	default:
		Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, tempeffect2()");
		return;
	}
	TEffects->Add( toAdd );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void reverseEffect( UI16 i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Reverse a temp effect
//o---------------------------------------------------------------------------o
void cEffects::reverseEffect( UI16 i )
{
	teffect_st *Effect = TEffects->GrabSpecific( i );
	CChar *s = calcCharObjFromSer( Effect->Destination() );
	if( s != NULL )
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
			s->Teleport();
			s->IsPolymorphed( false );
			break;
		case 19: // Incognito
			CItem *j;
			
			// ------ SEX ------
			s->SetID( s->GetOrgID( 2 ), 2 );
			s->SetName( s->GetOrgName() );
			j = s->GetItemAtLayer( 0x0B );
			if( j != NULL )
			{
				j->SetColour( s->GetHairColour() );
				j->SetID( s->GetHairStyle() );
			}
			j = s->GetItemAtLayer( 0x10 );
			if( j != NULL && s->GetID( 2 ) == 0x90 )
			{
				j->SetColour( s->GetBeardColour() );
				j->SetID( s->GetBeardStyle() );
			}
			// only refresh once
			cSocket *tSock;
			tSock = calcSocketObjFromChar( s );
			wornItems( tSock, s );
			s->Teleport();
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
	TEffects->DelCurrent();
	
	Items->CheckEquipment( s );
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
	char filename[MAX_PATH];
	std::ofstream writeDestination, effectDestination;
	const char blockDiscriminator[] = "\n\n---EFFECT---\n\n";
	const char binBlockDisc = static_cast<char>(0xFF);

	int Mode = cwmWorldState->ServerData()->SaveMode();

	int s_t = getclock();

	Console << "Saving Effects...   ";
	Console.TurnYellow();

	sprintf( filename, "%seffects.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );
	
	if( Mode == 1 ) 
	{
		effectDestination.open( filename, std::ios::binary|std::ios::out );
		if( !effectDestination )
		{
			Console.Error( 1, "Failed to open %s for writing", filename );
			return;
		}
		effectDestination.write( "B", 1 );
	} 
	else 
	{
		effectDestination.open( filename );
		if( !effectDestination )
		{
			Console.Error( 1, "Failed to open %s for writing", filename );
			return;
		}
	}

	for( teffect_st *currEffect = TEffects->First(); currEffect != NULL; currEffect = TEffects->Next() )
	{
		currEffect->Save( effectDestination, Mode );

		if( Mode == 1 )
			effectDestination.write( &binBlockDisc, 1 );
		else
			effectDestination << blockDiscriminator;
	}
	effectDestination.close();

	Console << "\b\b\b\b";
	Console.PrintDone();

	int e_t = getclock();
	Console.Print( "%s effects saved in %.02fsec\n", ( Mode == 1 ? "BINARY" : "ASCII" ), ((float)(e_t-s_t))/1000.0f );
}

//o--------------------------------------------------------------------------
//|	Function		-	LoadEffects( void )
//|	Date			-	16 March, 2002
//|	Programmer		-	sererg
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Loads in Effects from disk
//o--------------------------------------------------------------------------
void cEffects::LoadEffects( void )
{
	char filename[MAX_PATH];
	std::ifstream readDestination;
	UI08 test;

	int mode = 0;//ascii

	sprintf( filename, "%seffects.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );	// let's name our file
	readDestination.open( filename, std::ios::in|std::ios::binary );//open first file and check for binary
	if( readDestination.is_open() )
	{
		readDestination.read( (char *)&test, 1 );//read the 'B' to make sure we are binary
		if( test == 'B' )
			mode = 1;
		readDestination.close();
	}

	teffect_st toLoad;

	if( mode == 1 )//binary
	{
		readDestination.open( filename, std::ios::in|std::ios::binary );
		if( !readDestination.is_open() )
			return;
		readDestination.seekg(1);
		do 
		{
			BinBuffer buff( readDestination );
			switch ( buff.GetType() )
			{
			case 1:		
				toLoad.Source( buff.GetLong() );
				toLoad.Destination( buff.GetLong() );
				toLoad.ExpireTime( buff.GetLong() + cwmWorldState->GetUICurrentTime() );
				toLoad.Number( buff.GetByte() );
				toLoad.More1( buff.GetShort() );
				toLoad.More2( buff.GetShort() );
				toLoad.More3( buff.GetShort() );
				toLoad.Dispellable( ( (buff.GetByte() == 0) ? false : true ) );
				SERIAL objSer;
				objSer = buff.GetLong();
				if( objSer != INVALIDSERIAL )
				{
					if( objSer < BASEITEMSERIAL )
						toLoad.ObjPtr( calcCharObjFromSer( objSer ) );
					else
						toLoad.ObjPtr( calcItemObjFromSer( objSer ) );
				}
				else
					toLoad.ObjPtr( NULL );
				TEffects->Add( toLoad );
				break;
			case 0xFF:
				break;
			default:
				Console << "Error!: Found unknown binary section header " << (UI16)buff.GetType() << myendl;
				break;
			}
		} while( !readDestination.eof() && !readDestination.fail() );
	} 
	else 
	{//ascii
		readDestination.open( filename, std::ios::in|std::ios::binary );
		if( !readDestination.is_open() )
			return;
		char line[1024];

		do
		{
			bool valid = false;
			do 
			{
				readDestination.getline( line, 1024 );
#if defined(__unix__)
				trimWindowsText( line );
#endif

				if( readDestination.eof() || readDestination.fail() )
				{
					strcpy( line, "---EFFECT---" );
					valid = true;
				}
				else if( line[0] == '/' || line[0] == ' ' || line[0] == 10 || line[0] == 13 || line[0] == 0 )
					valid = false;
				else
					valid = true;
			} while( !valid );

			if( line[0] == '[' )	// in a section
			{
				if( !strcmp( line, "[EFFECT]" ) )
				{
					char tag[128], data[512];

					bool bFinished = false;
					while( !bFinished)
					{
						ReadWorldTagData( readDestination, tag, data );

						switch( tag[0] )
						{
						case 'D':
							if( !strcmp( tag, "Dest" ) )
								toLoad.Destination( atoi( data ) );
							if( !strcmp( tag, "Dispel" ) )
								toLoad.Dispellable( ( (atoi( data ) == 0) ? false : true ) );
							break;
						case 'E':
							if( !strcmp( tag, "Expire" ) )
								toLoad.ExpireTime( atoi( data ) + cwmWorldState->GetUICurrentTime() );
							break;
						case 'I':
							if( !strcmp( tag, "ItemPtr" ) )
							{
								SERIAL objSer = atoi( data );
								if( objSer != INVALIDSERIAL )
								{
									if( objSer < BASEITEMSERIAL )
										toLoad.ObjPtr( calcCharObjFromSer( objSer ) );
									else
										toLoad.ObjPtr( calcItemObjFromSer( objSer ) );
								}
								else
									toLoad.ObjPtr( NULL );
							}
							break;
						case 'M':
							if( !strcmp( tag, "More1" ) )
								toLoad.More1( atoi( data ) );
							if( !strcmp( tag, "More2" ) )
								toLoad.More2( atoi( data ) );
							if( !strcmp( tag, "More3" ) )
								toLoad.More3( atoi( data ) );
							break;
						case 'N':
							if( !strcmp( tag, "Number" ) )
								toLoad.Number( atoi(data) );
							break;
						case 'O':
							if( !strcmp( tag, "ObjPtr" ) )
							{
								SERIAL objSer = atoi( data );
								if( objSer != INVALIDSERIAL )
								{
									if( objSer < BASEITEMSERIAL )
										toLoad.ObjPtr( calcCharObjFromSer( objSer ) );
									else
										toLoad.ObjPtr( calcItemObjFromSer( objSer ) );
								}
								else
									toLoad.ObjPtr( NULL );
							}
						case 'S':
							if( !strcmp( tag, "Source" ) )
								toLoad.Source( atoi(data) );
							break;
						default:
							Console.Error( 1, "Unknown tag in effects.wsc" );
							break;
						}
						bFinished = ( strcmp( tag, "o---o" ) == 0 );
					}
					TEffects->Add( toLoad );
				}
			}
		} while( strcmp( line, "EOF" ) && !readDestination.eof() );
	}
	readDestination.close();
}

