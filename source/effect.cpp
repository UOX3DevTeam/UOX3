#include "uox3.h"

void movingeffect( cBaseObject *source, cBaseObject *dest, UI16 effect, UI08 speed, UI08 loop, bool explode )
{	//0x0f 0x42 = arrow 0x1b 0xfe=bolt
	if( source == NULL || dest == NULL ) 
		return;

	CPGraphicalEffect toSend( 0, (*source), (*dest) );
	toSend.Model( effect );
	toSend.SourceLocation( (*source) );
	toSend.TargetLocation( (*dest) );
	toSend.Speed( speed );
	toSend.Duration( loop );
	toSend.ExplodeOnImpact( explode );
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		CChar *socketTest = tSock->CurrcharObj();
		if( objInRange( socketTest, source, 18 ) && objInRange( socketTest, dest, 18 ) )
			tSock->Send( &toSend );
	}
	Network->PopConn();
}

void movingeffect( cBaseObject *source, SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode )
{	//0x0f 0x42 = arrow 0x1b 0xfe=bolt
	if( source == NULL ) 
		return;

	CPGraphicalEffect toSend( 0, (*source) );
	toSend.TargetSerial( INVALIDSERIAL );
	toSend.Model( effect );
	toSend.SourceLocation( (*source) );
	toSend.TargetLocation( x, y, z );
	toSend.Speed( speed );
	toSend.Duration( loop );
	toSend.ExplodeOnImpact( explode );
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( tSock != NULL )
			tSock->Send( &toSend );
	}
	Network->PopConn();
}

void staticeffect( cBaseObject *target, UI16 effect, UI08 speed, UI08 loop, bool explode )
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
void staticeffect( SI16 x, SI16 y, SI08 z, UI16 effect, UI08 speed, UI08 loop, bool explode )
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

void bolteffect( CChar *player )
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

void tempeffectsoff( void )
{
	CChar *s = NULL;
	cBaseObject *myObj = NULL;
	bool equipCheckNeeded = false;

	for( teffect_st *Effect = Effects->First(); !Effects->AtEnd(); Effect = Effects->Next() )
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
				cScript *tScript;
				UI16 scpNum;
	
				if( Effect->Source() >= 0x40000000 )	// item's have serials of 0x40000000 and above, and we already know it's not INVALIDSERIAL
				{
					myObj = calcItemObjFromSer( Effect->Source() );
					scpNum = myObj->GetScriptTrigger();
					equipCheckNeeded = false;
				}
				else
				{
					myObj = calcCharObjFromSer( Effect->Source() );
					scpNum = myObj->GetScriptTrigger();
					equipCheckNeeded = true;
				}
	
				tScript = Trigger->GetScript( scpNum );
				if( tScript != NULL )
				{	// script exists
					tScript->OnTimer( myObj, Effect->More1() );
					// do OnTimer stuff here
				}
				Effects->DelCurrent();
				if( s != NULL && equipCheckNeeded )
					Items->CheckEquipment( s ); // checks equipments for stat requirements
				break;
			default:
				Console.Error( 2, "tempeffectsoff() Unknown effect %i on character serial %i", Effect->Number(), s->GetSerial() ); //Morrolan
				break;
			}
		}
	}
}

void tempeffectson( void )
{
	CChar *s = NULL;
	for( teffect_st *Effect = Effects->First(); !Effects->AtEnd(); Effect = Effects->Next() )
	{
		s = calcCharObjFromSer( Effect->Destination() );
		if( s != NULL )
		{
			switch( Effect->Number() )
			{
			case 1:	s->SetFrozen( true );						break;
			case 2:	s->SetFixedLight( cwmWorldState->ServerData()->GetWorldLightBrightLevel() );	break;
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

void HandleMakeItemEffect( teffect_st *tMake )
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
	for( SI32 skCounter = 0; skCounter < toMake->skillReqs.size(); skCounter++ )
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
		Skills->ApplyRank( sock, targItem, rank );
		
		// if we're not a GM, see if we should store our creator
		if( !src->IsGM() && toMake->skillReqs.size() > 0 )
		{
			targItem->SetCreator( src->GetSerial() );
			int avgSkill, sumSkill = 0;
			// Find the average of our player's skills
			for( SI32 resCounter = 0; resCounter < toMake->skillReqs.size(); resCounter++ )
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
	targItem->SetMagic( 1 );
	soundeffect( sock, toMake->soundPlayed, true );

	sysmessage( sock, 985 );
	statwindow( sock, src );
}

void checktempeffects( void )
{
	CItem *mortar = NULL, *i = NULL;
	CChar *s = NULL, *src = NULL, *targ = NULL;
	cSocket *tSock = NULL;
	cBaseObject *myObj = NULL;

	UI32 j = uiCurrentTime;
	char temp[1024];
	for( teffect_st *Effect = Effects->First(); !Effects->AtEnd(); Effect = Effects->Next() )
	{
		if( Effect == NULL )
			continue;
		if( Effect->ExpireTime() > j )
			continue;

		s = calcCharObjFromSer( Effect->Destination() );
		if( s != NULL )
			tSock = calcSocketObjFromChar( s );

		bool equipCheckNeeded = false;

		switch( Effect->Number() )
		{
		case 1:
			if( s->IsFrozen() )
			{
				s->SetFrozen( false );
				if( tSock != NULL ) 
					sysmessage( tSock, 700 );
			}
			break;
		case 2:
			if( s == NULL )
				continue;

			s->SetFixedLight( 255 );
			doLight( tSock, cwmWorldState->ServerData()->GetWorldLightCurrentLevel() );
			break;
		case 3:
			if( s == NULL )
				continue;
			s->IncDexterity2( Effect->More1() );
			statwindow( tSock, s );
			equipCheckNeeded = true;
			break;
		case 4:
			if( s == NULL )
				continue;

			s->IncIntelligence2( Effect->More1() );
			statwindow( tSock, s );
			equipCheckNeeded = true;
			break;
		case 5:
			if( s == NULL )
				continue;

			s->IncStrength2( Effect->More1() );
			statwindow( tSock, s );
			equipCheckNeeded = true;
			break;
		case 6:
			if( s == NULL )
				continue;

			s->IncDexterity2( -Effect->More1() );
			s->SetStamina( min( s->GetStamina(), s->GetMaxStam() ) );
			statwindow( tSock, s );
			equipCheckNeeded = true;
			break;
		case 7:
			if( s == NULL )
				continue;

			s->IncIntelligence2( -Effect->More1() );
			s->SetMana( min( s->GetMana(), s->GetMaxMana() ) );
			statwindow( tSock, s );
			equipCheckNeeded = true;
			break;
		case 8:
			if( s == NULL )
				continue;

			s->IncStrength2( -Effect->More1() );
			s->SetHP( min( s->GetHP(), s->GetMaxHP() ) );
			statwindow( tSock, s );
			equipCheckNeeded = true;
			break;
		case 9:
			if( s == NULL )
				continue;

			switch( Effect->More1() )
			{
			case 0:
				if( Effect->More2() != 0 )
				{
					sprintf( temp, Dictionary->GetEntry( 1270 ), s->GetName() );
					npcEmoteAll( s, temp, true );
				}
				soundeffect( s, 0x0242 );
				break;
			}
			break;
		case 10:
			src = calcCharObjFromSer( Effect->Source() );
			mortar = calcItemObjFromSer( Effect->Destination() );
			Skills->CreatePotion( src, Effect->More1(), Effect->More2(), mortar );
			src->SkillUsed( false, ALCHEMY );
			break;
		case 11:
			if( s == NULL )
				continue;

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
			if( s == NULL )
				continue;

			s->IncStrength2( Effect->More1() );
			s->IncDexterity2( Effect->More2() );
			s->IncIntelligence2( Effect->More3() );
			statwindow( tSock, s );
			equipCheckNeeded = true;
			break;
		case 13: // door
			mortar = calcItemObjFromSer( Effect->Destination() );
			if( mortar == NULL )
				continue;

			if( !mortar->isDoorOpen() ) 
				break;

			mortar->SetDoorOpen( false );

			if( isDoorBlocked( mortar ) )
				tempeffect( NULL, mortar, 13, 0, 0, 0 );
			else
				useDoor( NULL, mortar );
			break;
		case 14: //- training dummies Tauriel check to see if item moved or not before searching for it
			i = &items[Effect->ItemPtr()];
			SERIAL effectSerial;
			effectSerial = Effect->Destination();
			if( i->GetSerial() == effectSerial )
				mortar = i;
			else 
				mortar = calcItemObjFromSer( effectSerial );
			if( mortar->GetID() == 0x1071 )
			{
				mortar->IncID( -1 );
				mortar->SetGateTime( 0 );
				RefreshItem( mortar );
			} 
			else if( mortar->GetID() == 0x1075 )
			{
				mortar->IncID( -1 );
				mortar->SetGateTime( 0 );
				RefreshItem( mortar );
			} 
			break;
		case 15: //reactive armor
			if( s == NULL )
				continue;

			s->SetReactiveArmour( false );
			break;
		case 16: //Explosion potion messages
			if( s == NULL )
				continue;

			src = calcCharObjFromSer( Effect->Source() );
			if( src->GetAntiSpamTimer() < uiCurrentTime )
			{
				src->SetAntiSpamTimer( BuildTimeValue( 1 ) );
				sprintf( temp, "%i", Effect->More3() );
				sysmessage( tSock, temp );
			} 
			break;
		case 17: //Explosion potion
			src = calcCharObjFromSer( Effect->Source() );
			explodeItem( calcSocketObjFromChar( src ), &items[Effect->ItemPtr()] ); //explode this item
			break;
		case 18: //Polymorph spell
			if( s == NULL )
				continue;

			s->SetID( s->GetOrgID() );
			s->Teleport();
			s->IsPolymorphed( false );
			break;
		case 19: //Incognito spell
			if( s == NULL )
				continue;

			s->SetID( s->GetOrgID() );
			
			// ------ NAME -----
			s->SetName( s->GetOrgName() );
			
			i = FindItemOnLayer( s, 0x0B );
			if( i != NULL ) 
			{
				i->SetColour( s->GetHairColour() );
				i->SetID( s->GetHairStyle() );
			}
			i = FindItemOnLayer( s, 0x10 );
			if( i != NULL && s->GetID( 2 ) == 0x90 )
			{
				i->SetColour( s->GetBeardColour() );
				i->SetID( s->GetBeardStyle() );
			}
			wornItems( tSock, s );
			s->Teleport();
			s->IsIncognito( false );
			break;
			
	    case 21:
			if( s == NULL )
				continue;

			int toDrop;
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
			i = &items[Effect->ItemPtr()];
			if( src != NULL && targ != NULL )
			{
				if( src->SkillUsed( Effect->More1() ) )
				{
					cSocket *srcSock = calcSocketObjFromChar( src );
					cSocket *targSock = calcSocketObjFromChar( targ );
					if( Effect->Number() == 22 )
					{
						newHealth = targ->GetHP() + ( src->GetSkill( ANATOMY ) / 50 + RandomNum( 3, 10 ) + RandomNum( src->GetSkill( HEALING ) / 50, src->GetSkill( HEALING ) / 20 ) );
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
							soundeffect( targSock, 0x01E0, false );
							sysmessage( targSock, 1273 );
							targ->SendToSocket( targSock, true, targ );
						}
						if( srcSock != NULL )
							sysmessage( srcSock,  1274 );
					}
				}
				src->SkillUsed( false, Effect->More1() );

			}
			decItemAmount( i );
			break;
		case 25:
			if( Effect->More2() == 0 )
				items[Effect->ItemPtr()].SetDisabled( false );
			else
				chars[Effect->ItemPtr()].SetDisabled( false );
			break;
		case 26:
			if( s == NULL )
				continue;
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
			cScript *tScript;
			UI16 scpNum;

			if( Effect->Source() >= 0x40000000 )	// item's have serials of 0x40000000 and above, and we already know it's not INVALIDSERIAL
			{
				myObj = calcItemObjFromSer( Effect->Source() );
				if( myObj != NULL )
					scpNum = myObj->GetScriptTrigger();
				equipCheckNeeded = false;
			}
			else
			{
				myObj = calcCharObjFromSer( Effect->Source() );
				if( myObj != NULL )
					scpNum = myObj->GetScriptTrigger();
				equipCheckNeeded = true;
			}

			tScript = Trigger->GetScript( scpNum );
			if( tScript != NULL )
			{	// script exists
				tScript->OnTimer( myObj, Effect->More1() );
				// do OnTimer stuff here
			}
			break;
		case 41:	// Creating an item
			HandleMakeItemEffect( Effect );
			break;
		case 42:
			src = calcCharObjFromSer( Effect->Source() );
			soundeffect( src, Effect->More2() );
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
				src->SetOwner( INVALIDSERIAL );
				src->SetTamed( false );
				src->SetNpcWander( 2 );
			}
			break;
		default:
			Console.Error( 2, " Fallout of switch statement without default. checktempeffects()"); //Morrolan
			return;
		}
		Effects->DelCurrent();
		if( s != NULL && equipCheckNeeded )
			Items->CheckEquipment( s ); // checks equipments for stat requirements
	}
}

void tempeffect( CChar *source, CChar *dest, SI08 num, UI16 more1, UI16 more2, UI16 more3, ITEM targItemPtr )
{
	int ic;
	
	teffect_st toAdd;
	teffect_st *Effect = NULL;

	SERIAL targSer = INVALIDSERIAL, sourSer = INVALIDSERIAL;
	if( source != NULL )
		sourSer = source->GetSerial();
	toAdd.Source( sourSer );
	if( dest != NULL )
		targSer = dest->GetSerial();
	toAdd.Destination( targSer );

	for( Effect = Effects->First(), ic = 0; !Effects->AtEnd(); Effect = Effects->Next(), ic++ )
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
					reverse_effect( ic );
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
		dest->SetFixedLight( worldbrightlevel );
		doLight( tSock, worldbrightlevel );
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
		toAdd.ExpireTime( BuildTimeValue( (R32)polyduration ) );
		toAdd.Dispellable( false );
		
		UI16 k;
		// Grey flag when polymorphed
		dest->SetCrimFlag( BuildTimeValue( (R32)polyduration ) );
		if( dest->IsOnHorse() ) 
			DismountCreature( tSock->CurrcharObj() );
		k = ( more1<<8 ) + more2;
		
		if( k >= 0x000 && k <= 0x3e1 ) // lord binary, body-values >0x3e1 crash the client
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
		char temp[100];
		teffect_st *Test;
		long checkChar;
		long oldTarg;
		if( source->SkillUsed( more1 ) )
		{
			for( Test = Effects->First(); !Effects->AtEnd(); Test = Effects->Next() )	// definitely not friendly and scalable, but it stops all prior healing attempts
			{	// another option would be to do a bit set, to specify already healing
				if( Test == NULL )
					continue;
				switch( Test->Number() )
				{
				case 22:
				case 23:
				case 24:
					checkChar = Test->Source();
					if( checkChar == sourSer )
					{
						oldTarg = calcCharFromSer( Test->Destination() );
						if( Test->Number() == 22 )
							sprintf( temp, Dictionary->GetEntry( 1275 ), source->GetName(), chars[oldTarg].GetName() );
						else if( Test->Number() == 23 )
							sprintf( temp, Dictionary->GetEntry( 1276 ), source->GetName(), chars[oldTarg].GetName() );
						else if( Test->Number() == 24 )
							sprintf( temp, Dictionary->GetEntry( 1277 ), source->GetName(), chars[oldTarg].GetName() );
						npcEmoteAll( source, temp, false );
						Effects->DelCurrent();		// we're already involved in some form of healing, BREAK IT
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
			sprintf( temp, Dictionary->GetEntry( 1278 ), source->GetName(), dest->GetName() );
			npcEmoteAll( source, temp, false );
		}
		else if( num == 23 )
		{
			toAdd.ExpireTime( BuildTimeValue( 15.0f ) );
			sprintf( temp, Dictionary->GetEntry( 1279 ), source->GetName(), dest->GetName() );
			npcEmoteAll( source, temp, false );
		}
		else if( num == 24 )
		{
			if( dest == source )
				toAdd.ExpireTime( BuildTimeValue( (R32)RandomNum( 15000, 18000 ) / 1000.0f ) );
			else
				toAdd.ExpireTime( BuildTimeValue( 6.0f ) );
			sprintf( temp, Dictionary->GetEntry( 1280 ), source->GetName(), dest->GetName() );
			npcEmoteAll( source, temp, false );
		}
		toAdd.Dispellable( false );
		toAdd.ItemPtr( targItemPtr );	// the bandage we are using to achieve this
		toAdd.More1( more1 );			// the skill we end up using (HEALING for players, VETERINARY for monsters)
		source->SkillUsed( true, more1 );
		break;
	case 25:
		toAdd.ExpireTime( BuildTimeValue( (R32)more1 ) );
		toAdd.ItemPtr( calcCharFromSer( dest->GetSerial() ) );
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

	Effects->Add( toAdd );
}

void tempeffect( CChar *source, CItem *dest, SI08 num, UI16 more1, UI16 more2, UI16 more3, ITEM targItemptr )
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
		toAdd.ItemPtr( calcItemFromSer( dest->GetSerial() ) ); //used to try and cut search time down
		toAdd.More2( 0 );
		break;
	case 17: //Explosion potion (explosion)  Tauriel (explode in 4 seconds)
		toAdd.ExpireTime( BuildTimeValue( 4 ) );
		toAdd.More1( more1 );
		toAdd.More2( more2 );
		toAdd.ItemPtr( calcItemFromSer( dest->GetSerial() ) );
		toAdd.Dispellable( false );
		break;
	case 25:
		toAdd.ExpireTime( BuildTimeValue( more1 ) );
		toAdd.ItemPtr( calcItemFromSer( dest->GetSerial() ) );
		dest->SetDisabled( true );
		toAdd.More2( 0 );
		break;
	default:
		Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, tempeffect2()");
		return;
	}
	Effects->Add( toAdd );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void reverse_effect( int i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Reverse a temp effect
//o---------------------------------------------------------------------------o
void reverse_effect( int i )
{
	teffect_st *Effect = Effects->GrabSpecific( i );
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
		case 4:		s->IncIntelligence2( Effect->More1() );	break;
		case 5:		s->IncStrength2( Effect->More1() );		break;
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
			j = FindItemOnLayer( s, 0x0B );
			if( j != NULL )
			{
				j->SetColour( s->GetHairColour() );
				j->SetID( s->GetHairStyle() );
			}
			j = FindItemOnLayer( s, 0x10 );
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
			Console.Error( 2, " Fallout of switch statement without default. uox3.cpp, reverse_effect()");
			return;
		}
	}
	Effects->DelCurrent();
	
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
void SaveEffects( void )
{
	char filename[MAX_PATH];
	std::ofstream writeDestination, effectDestination;
	const char blockDiscriminator[] = "\n\n---EFFECT---\n\n";
	const char binBlockDisc = (char)0xFF;

	const UI08 Version = 1;

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

	for( teffect_st *currEffect = Effects->First(); currEffect != NULL; currEffect = Effects->Next() )
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
void LoadEffects( void )
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
				toLoad.ExpireTime( buff.GetLong() + uiCurrentTime );
				toLoad.Number( buff.GetByte() );
				toLoad.More1( buff.GetShort() );
				toLoad.More2( buff.GetShort() );
				toLoad.More3( buff.GetShort() );
				toLoad.Dispellable( ( (buff.GetByte() == 0) ? false : true ) );
				toLoad.ItemPtr( buff.GetLong() );
				Effects->Add( toLoad );
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
#ifdef __LINUX__
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
								toLoad.ExpireTime( atoi( data ) + uiCurrentTime );
							break;
						case 'I':
							if( !strcmp( tag, "ItemPtr" ) )
								toLoad.ItemPtr( atoi( data ) );
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
					Effects->Add( toLoad );
				}
			}
		} while( strcmp( line, "EOF" ) && !readDestination.eof() );
	}
	readDestination.close();
}