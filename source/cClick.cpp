#include "uox3.h"
#include "cmdtable.h"
#include "magic.h"

#undef DBGFILE
#define DBGFILE "cClick.cpp"

//o---------------------------------------------------------------------------o
//|   Function    :  void paperdoll( cSocket *s, CChar *pdoll )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Open a characters paperdoll and show titles based on skill,
//|					 reputation, murder counts, race, ect.
//o---------------------------------------------------------------------------o
void paperdoll( cSocket *s, CChar *pdoll )
{
	char tempstr[128];
	CPPaperdoll pd		= (*pdoll);
	UnicodeTypes sLang	= s->Language();

	bool bContinue = false;
	if( pdoll->IsGM() )
		sprintf( tempstr, "%s %s", pdoll->GetName(), pdoll->GetTitle() );
	else if( pdoll->IsNpc() )
		sprintf( tempstr, "%s %s %s", title3( pdoll ), pdoll->GetName(), pdoll->GetTitle() );
	else if( pdoll->IsDead() )
		strcpy( tempstr, pdoll->GetName() );
	// Murder tags now scriptable in SECTION MURDERER - Titles.scp - Thanks Ab - Zane
	else if( pdoll->GetKills() > cwmWorldState->ServerData()->GetRepMaxKills() )
	{
		if( murdererTags.size() < 1 )
			sprintf( tempstr, Dictionary->GetEntry( 374, sLang ), pdoll->GetName(), pdoll->GetTitle(), title1( pdoll ), title2( pdoll ) );
		else if( pdoll->GetKills() < murdererTags[0].loBound )	// not a real murderer
			bContinue = true;
		else
		{
			SI16 mKills = pdoll->GetKills();
			UI32 kCtr;
			for( kCtr = 0; kCtr < murdererTags.size() - 1; kCtr++ )
			{
				if( mKills >= murdererTags[kCtr].loBound && mKills < murdererTags[kCtr+1].loBound )
					break;
			}
			if( kCtr >= murdererTags.size() )
				bContinue = true;
			else
				sprintf( tempstr, "%s %s, %s%s %s", murdererTags[kCtr].toDisplay.c_str(), pdoll->GetName(), pdoll->GetTitle(), title1( pdoll ), title2( pdoll ) );
		}
	}
	else if( pdoll->IsCriminal() )
		sprintf( tempstr, Dictionary->GetEntry( 373, sLang ), pdoll->GetName(), pdoll->GetTitle(), title1( pdoll ), title2( pdoll ) );
	else
		bContinue = true;
	if( bContinue )
	{
		sprintf( tempstr, "%s%s", title3( pdoll ), pdoll->GetName() );      //Repuation + Name
		if( pdoll->GetTownTitle() || pdoll->GetTownPriv() == 2 )	// TownTitle
		{
			if( pdoll->GetTownPriv() == 2 )	// is Mayor
				sprintf( tempstr, Dictionary->GetEntry( 379, sLang ), pdoll->GetName(), region[pdoll->GetTown()]->GetName(), title1( pdoll ), title2( pdoll ) );
			else	// is Resident
				sprintf( tempstr, "%s of %s, %s %s", pdoll->GetName(), region[pdoll->GetTown()]->GetName(), title1( pdoll ), title2( pdoll ) );
		}
		else	// No Town Title
		{
			char temp[128];
			strcpy( temp, tempstr );
			if( !pdoll->IsIncognito() && strlen( pdoll->GetTitle() ) > 0 )	// Titled & Skill
				sprintf( tempstr, "%s %s, %s %s", temp, pdoll->GetTitle(), title1( pdoll ), title2( pdoll ) );
			else	// Just skilled
				sprintf( tempstr, "%s, %s %s", temp, title1( pdoll ), title2( pdoll ) );
		}
	}
	pd.Text( tempstr );
	s->Send( &pd );
}

//o---------------------------------------------------------------------------o
//|   Function    :  handleCharDoubleClick( cSocket *mSock, SERIAL serial, bool keyboard )
//|   Date        :  Unknown
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles double-clicks on a character
//o---------------------------------------------------------------------------o
void handleCharDoubleClick( cSocket *mSock, SERIAL serial, bool keyboard )
{
	CChar *mChar = mSock->CurrcharObj();
	CItem *pack = NULL;
	CChar *c = calcCharObjFromSer( serial );
	if( c != NULL ) 
	{
		if( c->IsNpc() && c->IsValidMount() )	// Is a mount
		{
			if( ( c->IsTamed() && c->GetOwnerObj() == mChar ) || mChar->IsGM() )
			{
				if( objInRange( mChar, c, 2 ) )
				{
					if( mChar->GetID() != mChar->GetOrgID() )
					{
						sysmessage( mSock, 380 );
						return;
					}
					if( mChar->IsDead() ) 
						sysmessage( mSock, 381 );
					else 
						MountCreature( mChar, c );
				}
				else 
					sysmessage( mSock, 382 );
			}
			else
				sysmessage( mSock, 1214 );
			return; 
		}
		else if( c->IsNpc() && ( c->GetID( 1 ) != 0x01 || c->GetID( 2 ) < 0x90 || c->GetID( 2 ) > 0x93 ) ) // Is a monster
		{
			if( c->GetID() == 0x0123 || c->GetID() == 0x0124 )	// Is a pack animal
			{
				if( objInRange( mChar, c, 3 ) )
					sysmessage( mSock, 382 );
				else
				{
					pack = getPack( c );
					if( c->GetOwnerObj() == mChar )
					{
						if( pack != NULL )
							openPack( mSock, pack );
						else
							Console << "Pack animal " << c->GetSerial() << " has no backpack!" << myendl;
					}
					else
					{
						Skills->Snooping( mSock, c, pack );
						sysmessage( mSock, 383 );
					}
				}
				return;
			}
//			else
//				sysmessage( mSock, 384 );
			return; 
		}
		else	// Is a character
		{
			if( c->GetNPCAiType() == 17 )//PlayerVendors
			{
				npcTalk( mSock, c, 385, false );
				pack = getPack( c );
				if( pack != NULL ) 
					openPack( mSock, pack );
				return;
			}
			if( mChar->GetSerial() == serial )	// Double-clicked yourself
			{
				if( mChar->IsOnHorse() && !keyboard )
				{
					DismountCreature( mChar );	// If on horse, dismount
					return;
				}
			}
			paperdoll( mSock, c );
			return;
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool handleDoubleClickTypes( cSocket *mSock, CChar *mChar, CItem *x, UI08 iType )
//|   Date        :  2/11/2003
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Runs a switch to match an item type to a function
//o---------------------------------------------------------------------------o
bool handleDoubleClickTypes( cSocket *mSock, CChar *mChar, CItem *x, UI08 iType )
{
	CChar *iChar	= NULL;
	CItem *i		= NULL;
	UI32 j;
	UI16 itemID		= x->GetID();

	// Begin Check items by type
	switch( iType )
	{
	case 0:
	case 255:
		//sysmessage( mSock, 486 );
		break;

	case 1:	// Container, Backpack
	case 63: // Item spawn container
		if( x->GetMoreB( 1 ) )// Is trapped
			Magic->MagicTrap( mChar, x );
	case 65:	// Unlocked item spawn container
	case 87:	// Trash container ?
		bool onGround, isGM, isCounselor, isPackOwner, isPetOwner;
		onGround = false;

		if( x->GetCont() != NULL )	// It's inside another container, we need root container to calculate distance
		{
			iChar = getPackOwner( x );
			if( iChar == NULL )
			{
				CItem *rootContainer = getRootPack( x );	// In a pack on the ground
				if( rootContainer == NULL )
				{
					Console.Error( 2, "Pack with serial %li has an invalid container chain!", x->GetSerial() );
					return true;
				}
				else 
					onGround = true;
			}
		}
		else 
			onGround = true;	// It's on the ground

		isGM = mChar->IsGM();
		isCounselor = mChar->IsCounselor();
		isPackOwner = ( iChar == mChar );
		isPetOwner = ( iChar != NULL && iChar != mChar && iChar->GetOwnerObj() == mChar );

		if( onGround || isGM || isCounselor || isPackOwner || isPetOwner )
			openPack( mSock, x );
		else if( objInRange( mSock, iChar, 2 ) )	// Otherwise it's on an NPC, check if it's > 2 paces away
			Skills->Snooping( mSock, iChar, x );
		else
			sysmessage( mSock, 400 );
		return true;

	//Order and Chaos gate "openers" Item types 2 and 4 could cause alot of lag" )
	case 2: // Order gates opener
		for( j = 0; j < itemcount; j++ ) 
			if( items[j].GetType() == 3 )
			{
				if( items[j].GetMoreZ() == 1 )
				{
					items[j].SetMoreZ( 2 );
					items[j].SetZ( items[j].GetZ() + 17 );
					RefreshItem( &items[j] );
				}
				else if( items[j].GetMoreZ() == 2 )
				{
					items[j].SetMoreZ( 1 );
					items[j].SetZ( items[j].GetZ() - 17 );
					RefreshItem( &items[j] );
				}
			}
		return true;
	case 4: // Chaos gate opener
		for( j = 0; j < itemcount; j++ ) 
			if( items[j].GetType() == 5 )
			{
				if( items[j].GetMoreZ() == 3 )
				{
					items[j].SetMoreZ( 4 );
					items[j].SetZ( items[j].GetZ() + 17 );
					RefreshItem( &items[j] );
				}
				else if( items[j].GetMoreZ() == 4 )
				{
					items[j].SetMoreZ( 3 );
					items[j].SetZ( items[j].GetZ() - 17 );
					RefreshItem( &items[j] );
				}
			}
		return true;

	//Item type 6 in doubleclick() is yet another teleport item, should we remove?" )
	case 6: // Teleport rune
		target( mSock, 0, 1, 0, 2, 401 );
		return true;
	case 7: // Key
		mSock->AddID( x->GetMore() );
		target( mSock, 0, 1, 0, 11, 402 );
		return true;
	case 8:	// Locked container
	case 64: // locked spawn container
		sysmessage( mSock, 1428 );
		return true;
	case 9: // Spellbook
		i = getPack( mChar );
		if( i != NULL )
		{
			if( ( x->GetCont() == i || x->GetCont() == mChar ) || x->GetLayer() == 1 )
				Magic->SpellBook( mSock );
			else
				sysmessage( mSock, 403 );
		}
		return true;
	case 10: // Map
	{
		CPMapMessage m1;
		CPMapRelated m2;
		m1.KeyUsed( x->GetSerial() );
		m1.GumpArt( 0x139D );
		m1.UpperLeft( 0, 0 );
		m1.LowerRight( 0x13FF, 0x0FA0 );
		m1.Dimensions( 0x0190, 0x0190 );
		mSock->Send( &m1 );

		m2.ID( x->GetSerial() );
		m2.Command( 5 );
		m2.Location( 0, 0 );
		m2.PlotState( 0 );
		mSock->Send( &m2 );
		return true;
	}
	case 11:	// Readable book
		if( x->GetMoreX() != 666 && x->GetMoreX() != 999 )
			Books->OpenPreDefBook( mSock, x ); 
		else
			Books->OpenBook( mSock, x, ( x->GetMoreX() == 666 ) ); 
		return true;
	case 12: // Unlocked door
		if( !isDoorBlocked( x ) )
			useDoor( mSock, x );
		else
			sysmessage( mSock, 1661 );
		return true;
	case 13: // Locked door
		if( mChar->IsGM() )
		{
			sysmessage( mSock, 404 );
			useDoor( mSock, x );
			return true;
		}
		if( !keyInPack( mSock, mChar, getPack( mChar ), x ) ) 
			sysmessage( mSock, 406 );
		return true;
	case 14: // Food
		if( mChar->GetHunger() >= 6 )
		{
			sysmessage( mSock, 407 );
			return true;
		}
		else
		{
			soundeffect( mChar, 0x003A + RandomNum( 0, 2 ) );
			if( mChar->GetHunger() >= 0 && mChar->GetHunger() <= 6 )
				sysmessage( mSock, 408 + mChar->GetHunger() );
			else
				sysmessage( mSock, 415 );
			
			if( x->GetPoisoned() && mChar->GetPoisoned() < x->GetPoisoned() )
			{
				sysmessage( mSock, 416 + RandomNum( 0, 2 ) );
				soundeffect( mChar, 0x0246 ); //poison sound - SpaceDog
				mChar->SetPoisoned( x->GetPoisoned() );
				mChar->SetPoisonWearOffTime( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetSystemTimerStatus( POISON ) )) );
				mChar->SendToSocket( mSock, true, mChar );
			}
			//Remove a food item
			x = DecreaseItemAmount( x );
			mChar->SetHunger( mChar->GetHunger() + 1 );
		}
		return true;
	case 15: // Magic Wands
		if( x->GetMoreZ() != 0 )
		{
			mSock->CurrentSpellType( 2 );
			if( Magic->SelectSpell( mSock, ( 8 * ( x->GetMoreX() - 1 ) ) + x->GetMoreY() ) ) 
			{ 
				x->SetMoreZ( x->GetMoreZ() - 1 );
				if( x->GetMoreZ() == 0 )
				{
					x->SetType( x->GetType2() );
					x->SetMoreX( 0 );
					x->SetMoreY( 0 );
					x->SetOffSpell( 0 );
				}
			}
		}
		return true;
	case 18: // Crystal Ball
		objMessage( mSock, 419 + RandomNum( 0, 9 ), x );
		soundeffect( mSock, 0x01EC, true );
		return true;
	case 19: // Potions
		if( mChar->IsUsingPotion() )
			sysmessage( mSock, 430 );
		else
			usePotion( mChar, x );
		return true;
	case 35: // Townstone and Townstone Deed
		if( x->GetID() == 0x14F0 )		// Check for Deed
		{
			comm[1] = "3298";
			command_additem( mSock );
			Items->DeleItem( x );
		}
		else	// Display Townstone gump
		{
			UI08 useRegion = calcRegionFromXY( x->GetX(), x->GetY(), mChar->WorldNumber() );
			region[useRegion]->DisplayTownMenu( x, mSock );
		}
		return true;
		
	case 50: // Recall Rune
		if( x->GetMoreX() == 0 && x->GetMoreY() == 0 && x->GetMoreZ() == 0 )	// changed, to fix, Lord Vader
			sysmessage( mSock, 431 );
		else
		{
			sysmessage( mSock, 432 );
			mChar->SetSpeechItem( x->GetSerial() );
			mChar->SetSpeechMode( 7 );
		}
		return true;
	case 51:	// Starting Moongate
	case 52:	// Ending Moongate
		if( objInRange( mChar, x, 2 ) )
		{
			CItem *otherGate = calcItemObjFromSer( x->GetMoreX() );
			if( otherGate != NULL )
			{
				mChar->SetLocation( otherGate );
				mChar->Teleport();
			}
		}
		return true;
	case 60:	// Object Teleporter
		if( objInRange( mChar, x, 2 ) )
		{
			mChar->SetLocation( static_cast<SI16>(x->GetMoreX()), static_cast<SI16>(x->GetMoreY()), static_cast<SI08>(x->GetMoreZ() ));
			mChar->Teleport();
		}
		return true;
	case 89:
		SocketMapChange( mSock, mChar, x );
		return true;
	case 101: // Morph Object
		mChar->SetID( (UI16)x->GetMoreX() );
		mChar->Teleport();
		x->SetType( 102 );
		return true;
	case 102: // Unmorph Object
		mChar->SetID( mChar->GetxID() );
		mChar->Teleport();
		x->SetType( 101 );
		return true;
	case 104: // Teleport object
		mChar->SetLocation( static_cast<SI16>(x->GetMoreX()), static_cast<SI16>(x->GetMoreY()), static_cast<SI08>(x->GetMoreZ() ));
		mChar->Teleport();
		return true;
	case 105:  // Drink
		soundeffect( mChar, 0x30 + RandomNum( 0, 1 ) );
		if( RandomNum( 0, 1 ) )
			npcTalk( mSock, mChar, 435, false );
		else
			npcEmote( mSock, mChar, 436, false );

		if( x->GetAmount() > 1 )
			DecreaseItemAmount( x );
		else if( !(RandomNum( 0, 4 )) || x->GetAmount() == 1 )	// they emptied it.
		{
			x->SetType( 0 );
			x->SetAmount( 0 );
			sysmessage( mSock, 437 );
			switch( itemID )
			{
			case 0x09F0:
			case 0x09AD:
			case 0x0FF8:
			case 0x0FF9: 
			case 0x1F95:
			case 0x1F96:
			case 0x1F97:
			case 0x1F98:
			case 0x1F99:
			case 0x1F9A:
			case 0x1F9B:
			case 0x1F9C:
			case 0x1F9D:
			case 0x1F9E://Pitchers
				x->SetID( 0x09A7 );
				break;
				
			case 0x09EE:
			case 0x09EF:
			case 0x1F7D:
			case 0x1F7E:
			case 0x1F7F:
			case 0x1F80:
			case 0x1F85:
			case 0x1F86:
			case 0x1F87:
			case 0x1F88:
			case 0x1F89:
			case 0x1F8A:
			case 0x1F8B:
			case 0x1F8C:
			case 0x1F8D:
			case 0x1F8E:
			case 0x1F8F:
			case 0x1F90:
			case 0x1F91:
			case 0x1F92:
			case 0x1F93:
			case 0x1F94://glasses
				x->SetID( 0x1F84 );
				break;
			}
		}
		return true;
	case 117:	// Planks
		if( objInRange( mChar, x, 6 ) )
		{
			if( x->GetID( 2 ) == 0x84 || x->GetID( 2 ) == 0xD5 || x->GetID( 2 ) == 0xD4 || x->GetID( 2 ) == 0x89 )
				Boats->PlankStuff( mSock, x );
			else 
				sysmessage( mSock, 398 );//Locked
		} 
		else 
			sysmessage( mSock, 399 );
		return true;
	case 181: //Fireworks wands
		srand( uiCurrentTime );
		if( x->GetMoreX() == 0 )
		{
			sysmessage( mSock, 396 );
			return true;
		}
		x->SetMoreX( x->GetMoreX() - 1 );
		sysmessage( mSock, 397, x->GetMoreX() );
		UI08 j;
		for( j = 0; j < static_cast< UI08 >( RandomNum( 0, 3 ) + 2 ); j++ )
		{
			SI16 wx = ( mChar->GetX() + RandomNum( 0, 10 ) - 5 );
			SI16 wy = ( mChar->GetY() + RandomNum( 0, 10 ) - 5 );
			movingeffect( mChar, wx, wy, mChar->GetZ() + 10, 0x36E4, 17, 0, ( RandomNum( 0, 1 ) == 1 ) );
			switch( RandomNum( 0, 4 ) )
			{
			case 0:	staticeffect( wx, wy, mChar->GetZ() + 10, 0x373A, 0x09, 0, 0 );		break;
			case 1:	staticeffect( wx, wy, mChar->GetZ() + 10, 0x374A, 0x09, 0, 0 );		break;
			case 2:	staticeffect( wx, wy, mChar->GetZ() + 10, 0x375A, 0x09, 0, 0 );		break;
			case 3:	staticeffect( wx, wy, mChar->GetZ() + 10, 0x376A, 0x09, 0, 0 );		break;
			case 4:	staticeffect( wx, wy, mChar->GetZ() + 10, 0x377A, 0x09, 0, 0 );		break;
			}
		}
		return true;
	case 186: // Rename Deed
		mChar->SetSpeechItem( x->GetSerial() );
		mChar->SetSpeechMode( 6 );
		sysmessage( mSock, 434 );
		return true;
	case 190:	// Leather repair tool
		target( mSock, 0, 1, 0, 14, 485 );	// What do we wish to repair?
		return true;
	case 191:	// Bow repair tool
		target( mSock, 0, 1, 0, 15, 485 );	// What do we wish to repair?
		return true;
	case 200:	// Tillerman
		if( Boats->GetBoat( mSock ) == NULL )
		{
			CItem *boat = calcItemObjFromSer( x->GetMore() );
			Boats->ModelBoat( mSock, boat );
		}
		return true;
	case 202:	// Guildstone Deed
		if( itemID == 0x14F0 || itemID == 0x1869 )	// Check for Deed/Teleporter + Guild Type
		{
			mChar->SetSpeechItem( x->GetSerial() );		// we're using this as a generic item
			GuildSys->PlaceStone( mSock, x );
			return true;
		}
		else if( itemID == 0x0ED5 )			// Check for Guildstone + Guild Type
		{
			mSock->TempInt( x->GetMore() );	// track things properly
			if( mChar->GetGuildNumber() == -1 || mChar->GetGuildNumber() == x->GetMore() )
				GuildSys->Menu( mSock, BasePage + 1, static_cast<GuildID>(x->GetMore()) );	// more of the stone is the guild number
			else
				sysmessage( mSock, 438 );
			return true;
		}
		else 
			Console << "Unhandled guild item type named: " << x->GetName() << " with ID of: " << itemID << myendl;
		return true;
	case 203: // Open Housing Gump
		bool canOpen;
		canOpen = (  x->GetOwnerObj() == mChar || mChar->IsGM() );
		if( !canOpen && x->GetMoreZ() == 0 )
		{
			sysmessage( mSock, 439 );
			return true;
		}
		mSock->AddID( x->GetSerial() );
		if( !canOpen )
			Gumps->Menu( mSock, x->GetMoreZ() );
		else 
			Gumps->Menu( mSock, x->GetMoreX() );
		return true;
	case 204:	// tinker's tools
		target( mSock, 0, 1, 0, 180, 484 );
		return true;
	case 205:
		target( mSock, 0, 1, 0, 24, 485 );	// What do we wish to repair?
		return true;
	case 207:	// Forges
		target( mSock, 0, 1, 0, 237, 440 );
		return true;
	case 208:	// Dye
		mSock->DyeAll( 0 );
		target( mSock, 0, 1, 0, 31, 441 );
		return true;
	case 209:	// Dye vat
		mSock->AddID1( x->GetColour( 1 ) );
		mSock->AddID2( x->GetColour( 2 ) );
		target( mSock, 0, 1, 0, 32, 442 );
		return true;
	case 210:	// Model boat/Houses
		if( iType != 103 && iType != 202 )
		{
			if( x->GetMoreX() == 0 )
				break;
			mChar->SetMaking( x->GetSerial() );
			mChar->SetSpeechItem( x->GetSerial() );
			mSock->AddID3( static_cast<UI08>(x->GetMoreX()) );
			buildHouse( mSock, x->GetMoreX() );
		}
		return true;
	case 211:	// Archery buttle
		Skills->AButte( mSock, x );
		return true;
	case 212:	// Drum
		if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) ) 
			soundeffect( mChar, 0x0038 );
		else 
			soundeffect( mChar, 0x0039 );
		return true;
	case 213:	// Tambourine
		if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) ) 
			soundeffect( mChar, 0x0052 );
		else 
			soundeffect( mChar, 0x0053 );
		return true;
	case 214:	// Harps
		if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) ) 
			soundeffect( mChar, 0x0045 );
		else 
			soundeffect( mChar, 0x0046 );
		return true;
	case 215:	// Lute
		if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) ) 
			soundeffect( mChar, 0x004C );
		else 
			soundeffect( mChar, 0x004D );
		return true;
	case 216:	// Axes
		target( mSock, 0, 1, 0, 76, 443 );
		return true;
	case 217:	//Player Vendor Deeds
		CChar *m;
#pragma note( "DEPENDENT ON NUMERIC NPC SECTION" )
		//m = Npcs->AddNPCxyz( NULL, "player_vendor", mChar->GetX(), mChar->GetY(), mChar->GetZ(), mChar->WorldNumber() );	// Should impliment support for this soon - Zane
		m = Npcs->AddNPCxyz(NULL, "2117", mChar->GetX(), mChar->GetY(), mChar->GetZ(), mChar->WorldNumber() );
		m->SetNPCAiType( 17 );
		m->SetInvulnerable( true );
		m->SetHidden( 0 );
		m->SetDir( mChar->GetDir() );
		m->SetNpcWander( 0 );
		m->SetFlag( m->GetFlag()^7 );
		m->SetOwner( (cBaseObject *)mChar );
		Items->DeleItem( x );
		m->Teleport();
		npcTalk( mSock, m, 388, false, m->GetName() );
		return true;
	case 218:	// Smithy
		target( mSock, 0, 1, 0, 50, 444 );
		return true; 
	case 219:	// Carpentry
		target( mSock, 0, 1, 0, 134, 445 );
		return true;
	case 220:	// Mining
		target( mSock, 0, 1, 0, 51, 446 );
		return true; 
	case 221:	// empty vial
		i = getPack( mChar );
		if( i != NULL )
		{
			if( x->GetCont() == i )
			{
				mSock->AddMItem( x );
				target( mSock, 0, 1, 0, 186, 447 );
			}
			else 
				sysmessage( mSock, 448 );
		}
		return true;
	case 222:	// wool to yarn/cotton to thread
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 164, 449 );
		return true;
	case 223:	// cooking fish
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 168, 450 );
		return true;
	case 224:	//
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 168, 451 );
		return true;
	case 225:	// yarn/thread to cloth
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 165, 452 );
		return true;
	case 226:	// make shafts
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 172, 454 );
		return true;
	case 227:	// cannon ball
		target( mSock, 0, 1, 0, 170, 455 );
		Items->DeleItem( x );
		return true;
	case 228:	// pitcher of water to flour
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 173, 456 );
		return true;
	case 229:	// sausages to dough
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 174, 457 );
		return true;
	case 230:	// sewing kit for tailoring
		target( mSock, 0, 1, 0, 167, 459 );
		return true;
	case 231:	// smelt ore
		mChar->SetSmeltItem( x );
		target( mSock, 0, 1, 0, 52, 460 );
		return true;
	case 232:	// Message board opening
		if( objInRange( mChar, x, 2 ) )
			MsgBoardEvent( mSock );
		else
			sysmessage( mSock, 461 );
		return true;
	case 233:	// skinning
		target( mSock, 0, 1, 0, 86, 462 );
		return true;
	case 234:	// camping
		if( objInRange( mChar, x, 6 ) )
		{
			if( Skills->CheckSkill( mChar, CAMPING, 0, 500 ) ) // Morrolan TODO: insert logout code for campfires here
			{
				i = Items->SpawnItem( NULL, mChar, 1, "#", false, 0x0DE3, 0, false, false );
				if( i == NULL )
					return true;
				i->SetType( 45 );
				i->SetDir( 2 );

				if( x->GetCont() == NULL )
					i->SetLocation( x );
				else
					i->SetLocation( mChar );
				i->SetDecayable( true );
				i->SetDecayTime( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetSystemTimerStatus( DECAY ) )) );
				DecreaseItemAmount( x );
				RefreshItem( i );
			}
			else
				sysmessage( mSock, 463 );
		}
		else
			sysmessage( mSock, 393 );
		return true;
	case 235:	// magic statue?
		if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
		{
			x->SetID( 0x1509 );
			x->SetType( 45 );
			RefreshItem( x );
		}
		else
			sysmessage( mSock, 465 );
		return true;
	case 236:	// guillotines?
		if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
		{
			x->SetID( 0x1245 );
			x->SetType( 45 );
			RefreshItem( x );
		}
		else
			sysmessage( mSock, 466 );
		return true;
	case 237:	// Guillotine stop animation
		if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
		{
			x->SetID( 0x1230 );
			x->SetType( 45 );
			RefreshItem( x );
		}
		else
			sysmessage( mSock, 466 );
		return true;
	case 238:	// closed flour sack
		if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
		{
			x->SetID( 0x103A );
			x->SetType( 45 );
			RefreshItem( x );
		}
		else
			sysmessage( mSock, 466 );
		return true;
	case 239:	// open flour sack
		if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
		{
			x->SetID( 0x1039 );
			x->SetType( 45 );
			RefreshItem( x );
		}
		else
			sysmessage( mSock, 466 );
		return true;
	case 240:	// fishing
		if( mChar->GetFishingTimer() )
			sysmessage( mSock, 467 );
		else
			target( mSock, 0, 1, 0, 45, 468 );
		return true;
	case 241:	// clocks
		telltime( mSock );
		return true;
	case 242:	// Mortar for Alchemy
		if( !mChar->SkillUsed( ALCHEMY ) )
			target( mSock, 0, 1, 0, 108, 470 );
		else
			sysmessage( mSock, 1631 );
		return true;
	case 243:	// scissors
		target( mSock, 0, 1, 0, 128, 471 );
		return true;
	case 244:	// healing
		if( mChar->GetSkillDelay() <= uiCurrentTime )
		{
			mSock->AddMItem( x );
			target( mSock, 0, 1, 0, 130, 472 );
			mChar->SetSkillDelay( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetServerSkillDelayStatus() )) );
		}
		else
			sysmessage( mSock, 473 );
		return true;
	case 245:	// sextants
		sysmessage( mSock, 474, mChar->GetX(), mChar->GetY() );
		return true;
	case 246:	// Hair Dye
		mSock->AddID( x->GetSerial() );
		Gumps->Menu( mSock, 6 );
		return true;
	case 247:	// lockpicks
		mSock->AddMItem( x );
		target( mSock, 0, 1, 0, 162, 475 );
		return true;
	case 248:	// cotton plants
		if( !mChar->IsOnHorse() ) 
			action( mSock, 0x0D );
		else 
			action( mSock, 0x1D );
		soundeffect( mSock, 0x013E, true );
		i = Items->SpawnItem( mSock, mChar, 1, "#", true, 0x0DF9, 0, true, true );
		if( i == NULL )
			return true;
		CItem *pack;
		pack = getPack( mChar );
		if( pack != NULL )
			i->SetCont( pack );
		CItem *skillItem;
		skillItem = mChar->GetSkillItem();
		if( skillItem != NULL )
		{
			skillItem->SetDecayable( true );
			sysmessage( mSock, 476 );
			mChar->SetSkillItem( NULL );
		}
		return true; // cotton
	case 249:	// tinker axle
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 183, 477 );
		return true;
	case 250:
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 184, 478 );
		return true;
	case 251:	// tinker clock
		target( mSock, 0, 1, 0, 185, 479 );
		Items->DeleItem( x );
		return true;
	case 252:	//tinker sextant
		if( Skills->CheckSkill( mChar, TINKERING, 0, 1000 ) )
		{
			sysmessage( mSock, 480 );
			i = Items->SpawnItem( mSock, mChar, 1, Dictionary->GetEntry( 1429 ), false, 0x1057, 0, true, true );
			if( i == NULL )
				return true;
			pack = getPack( mChar );
			i->SetCont( pack );
			CItem *skillItem;
			skillItem = mChar->GetSkillItem();
			if( skillItem != NULL )
			{
				skillItem->SetDecayable( true );
				Items->DeleItem( x );
				mChar->SetSkillItem( NULL );
			}
		}
		else 
			sysmessage( mSock, 481 );
		return true;
	case 253:	// training dummies
		if( objInRange( mChar, x, 1 ) ) 
			Skills->TDummy( mSock );
		else 
			sysmessage( mSock, 482 );
		return true;
	case 254:	// swinging training dummy
		sysmessage( mSock, 483 );
		return true;
	default:
		if( iType ) 
			Console << "Unhandled item type for item: " << x->GetName() << "[" << x->GetSerial() << "] of type: " << iType << myendl;
		break;
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool handleDoubleClickIDs( cSocket *mSock, CChar *mChar, CItem *x, UI16 itemID )
//|   Date        :  2/11/2003
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Runs a switch to match an item ID to a function.
//o---------------------------------------------------------------------------o
bool handleDoubleClickIDs( cSocket *mSock, CChar *mChar, CItem *x, UI16 itemID )
{
	CItem *i = NULL;
	UI08 iType = x->GetType();
	switch( itemID )
	{
	case 0x0FB1: // small forge
	case 0x1982: // partial lg forge
	case 0x197A: // partial lg forge
	case 0x197E: // partial lg forge
		target( mSock, 0, 1, 0, 237, 440 );
		return true; 
	case 0x0FA9:// Dye
		mSock->DyeAll( 0 );
		target( mSock, 0, 1, 0, 31, 441 );
		return true;
	case 0x0FAB:	// Dye vat
		mSock->AddID1( x->GetColour( 1 ) );
		mSock->AddID2( x->GetColour( 2 ) );
		target( mSock, 0, 1, 0, 32, 442 );
		return true;
	case 0x14F4:	// Model boat
	case 0x14F0:	// Houses
		if( iType != 103 && iType != 202 )
		{  //experimental house code
			if( x->GetMoreX() == 0 )
				break;
			mChar->SetMaking( x->GetSerial() );
			mChar->SetSpeechItem( x->GetSerial() );
			mSock->AddID3( static_cast<UI08>(x->GetMoreX()) );
			buildHouse( mSock, x->GetMoreX() );
		}
		return true;
	case 0x100A:
	case 0x100B:// Archery butte
		Skills->AButte( mSock, x );
		return true;
	case 0x0E9C: // Drum
		if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) ) 
			soundeffect( mChar, 0x0038 );
		else 
			soundeffect( mChar, 0x0039 );
		return true;
	case 0x0E9D: // Tambourine
	case 0x0E9E:
		if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) ) 
			soundeffect( mChar, 0x0052 );
		else 
			soundeffect( mChar, 0x0053 );
		return true;
	case 0x0EB1: // Standing harp
	case 0x0EB2: // Lap harp
		if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) ) 
			soundeffect( mChar, 0x0045 );
		else 
			soundeffect( mChar, 0x0046 );
		return true;
	case 0x0EB3: // Lute
	case 0x0EB4: // Lute
		if( Skills->CheckSkill( mChar, MUSICIANSHIP, 0, 1000 ) ) 
			soundeffect( mChar, 0x004C );
		else 
			soundeffect( mChar, 0x004D );
		return true;
	// SMITHY
	case 0x102A: // hammer
	case 0x102B: // hammer
	case 0x0FB4:// sledge hammer
	case 0x0FB5:// sledge hammer
	case 0x0FBB:// tongs
	case 0x0FBC:// tongs
	case 0x13E3:// smith's hammer
	case 0x13E4:// smith's hammer
		target( mSock, 0, 1, 0, 50, 444 );
		return true; 
	// CARPENTRY
	case 0x1026: // chisels
	case 0x1027: // chisels
	case 0x1028:// Dove Tail Saw
	case 0x1029:// Dove Tail Saw
	case 0x102C:// Moulding Planes
	case 0x102D:// moulding planes
	case 0x102E: // nails - carp. tool but not sure if this works here?
	case 0x102F: // nails - carp. tool but not sure if this works here?
	case 0x1030:// jointing plane
	case 0x1031:// jointing plane
	case 0x1032:// smoothing plane
	case 0x1033:// smoothing plane
	case 0x1034:	// saw
	case 0x1035: // saw
	case 0x10E4: // draw knife
	case 0x10E5: // froe - carp. tool but not sure if this works here?
	case 0x10E6: // inshave - carp. tool but not sure if this works here?
//		case 0x10E7: // scorp - carp. tool but not sure if this works here?
		target( mSock, 0, 1, 0, 134, 445 );
		return true;
	// MINING
	case 0x0E85:// pickaxe
	case 0x0E86:// pickaxe
	case 0x0F39:// shovel
	case 0x0F3A:// shovel
		target( mSock, 0, 1, 0, 51, 446 );
		return true; 
	case 0x0E24: // empty vial
		i = getPack( mChar );
		if( i != NULL )
		{
			if( x->GetCont() == i )
			{
				mSock->AddMItem( x );
				target( mSock, 0, 1, 0, 186, 447 );
			}
			else 
				sysmessage( mSock, 448 );
		}
		return true;
	case 0x0DF8: // wool to yarn 
	case 0x0DF9: // cotton to thread
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 164, 449 );
		return true;
	// Sept 22, 2002 - Xuri 
	//case 0x09CC: // raw fish       -- krazyglue: added support for cooking raw fish
	//case 0x09CD: // raw fish          should they have to fillet them first then
	//case 0x09CE: // raw fish          cook the fillet pieces?  or is this good enough?
	//case 0x09CF: // raw fish
	//	mChar->SetSkillItem( x );
	//	target( mSock, 0, 1, 0, 168, 450 );
	//	return true;
	//
	case 0x09F1: // 
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 168, 451 );
		return true;
	case 0x0FA0:
	case 0x0FA1: // thread to cloth
	case 0x0E1D:
	case 0x0E1F:
	case 0xE11E:  // yarn to cloth
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 165, 452 );
		return true;
	case 0x1BD1:
	case 0x1BD2:
	case 0x1BD3:
	case 0x1BD4:
	case 0x1BD5:
	case 0x1BD6:	// make shafts
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 172, 454 );
		return true;
	case 0x0E73: // cannon ball
		target( mSock, 0, 1, 0, 170, 455 );
		Items->DeleItem( x );
		return true;
	case 0x0FF8:
	case 0x0FF9: // pitcher of water to flour
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 173, 456 );
		return true;
	case 0x09C0:
	case 0x09C1: // sausages to dough
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 174, 457 );
		return true;
	case 0x0F9D: // sewing kit for tailoring
		target( mSock, 0, 1, 0, 167, 459 );
		return true;
	case 0x19B7:
	case 0x19B9:
	case 0x19BA:
	case 0x19B8: // smelt ore
		mChar->SetSmeltItem( x );
		target( mSock, 0, 1, 0, 52, 460 );
		return true;
	case 0x1E5E:
	case 0x1E5F: // Message board opening
		if( objInRange( mChar, x, 2 ) )
			MsgBoardEvent( mSock );
		else
			sysmessage( mSock, 461 );
		return true;
	case 0x0DE1:
	case 0x0DE2: // camping
		if( objInRange( mChar, x, 6 ) )
		{
			if( Skills->CheckSkill( mChar, CAMPING, 0, 500 ) ) // Morrolan TODO: insert logout code for campfires here
			{
				i = Items->SpawnItem( NULL, mChar, 1, "#", false, 0x0DE3, 0, false, false );
				if( i == NULL )
					return true;
				i->SetType( 45 );
				i->SetDir( 2 );

				if( x->GetCont() == NULL )
					i->SetLocation( x );
				else
					i->SetLocation( mChar );
				i->SetDecayable( true );
				i->SetDecayTime( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetSystemTimerStatus( DECAY ) )) );
				DecreaseItemAmount( x );
				RefreshItem( i );
			}
			else
				sysmessage( mSock, 463 );
		}
		else
			sysmessage( mSock, 393 );
		return true;
	case 0x1508: // magic statue?
		if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
		{
			x->SetID( 0x1509 );
			x->SetType( 45 );
			RefreshItem( x );
		}
		else
			sysmessage( mSock, 465 );
		return true;
	case 0x1509:
		if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
		{
			x->SetID( 0x1508 );
			x->SetType( 45 );
			RefreshItem( x );
		}
		else
			sysmessage( mSock, 465 );
		return true;
	case 0x1230:
	case 0x1246: // guillotines?
		if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
		{
			x->SetID( 0x1245 );
			x->SetType( 45 );
			RefreshItem( x );
		}
		else
			sysmessage( mSock, 466 );
		return true;
	case 0x1245: // Guillotine stop animation
		if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
		{
			x->SetID( 0x1230 );
			x->SetType( 45 );
			RefreshItem( x );
		}
		else
			sysmessage( mSock, 466 );
		return true;
	case 0x1039:  // closed flour sack
		if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
		{
			x->SetID( 0x103A );
			x->SetType( 45 );
			RefreshItem( x );
		}
		else
			sysmessage( mSock, 466 );
		return true;
	case 0x103A: // open flour sack
		if( Skills->CheckSkill( mChar, ITEMID, 0, 10 ) )
		{
			x->SetID( 0x1039 );
			x->SetType( 45 );
			RefreshItem( x );
		}
		else
			sysmessage( mSock, 466 );
		return true;
	case 0x0DBF:	// fishing pole
	case 0x0DC0:	// fishing pole
	case 0x0DC8:	// fishing nets
	case 0x0DC9:
	case 0x0DCA:
	case 0x0DCB:
	case 0x0DD0:
	case 0x0DD1:
	case 0x0DD2:
	case 0x0DD3:
	case 0x0DD4:
	case 0x0DD5:
	case 0x1EA3:	// nets
	case 0x1EA4:
	case 0x1EA5:
	case 0x1EA6:
		if( mChar->GetFishingTimer() )
			sysmessage( mSock, 467 );
		else
			target( mSock, 0, 1, 0, 45, 468 );
		return true;
	case 0x104B: // clock
	case 0x104C: // clock
	case 0x1086: // bracelet
		telltime( mSock );
		return true;
	case 0x0E9B: // Mortar for Alchemy
		if( !mChar->SkillUsed( ALCHEMY ) )
			target( mSock, 0, 1, 0, 108, 470 );
		else
			sysmessage( mSock, 1631 );
		return true;
	case 0x0F9E:
	case 0x0F9F: // scissors
		target( mSock, 0, 1, 0, 128, 471 );
		return true;
	case 0x0E21: // healing
		if( mChar->GetSkillDelay() <= uiCurrentTime )
		{
			mSock->AddMItem( x );
			target( mSock, 0, 1, 0, 130, 472 );
			mChar->SetSkillDelay( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetServerSkillDelayStatus() )) );
		}
		else
			sysmessage( mSock, 473 );
		return true;
	case 0x1057:
	case 0x1058: // sextants
		sysmessage( mSock, 474, mChar->GetX(), mChar->GetY() );
		return true;
	case 0x0E27:
	case 0x0EFF:   // Hair Dye
		mSock->AddID( x->GetSerial() );
		Gumps->Menu( mSock, 6 );
		return true;
	case 0x14FB:
	case 0x14FC:
	case 0x14FD:
	case 0x14FE: // lockpicks
		mSock->AddMItem( x );
		target( mSock, 0, 1, 0, 162, 475 );
		return true;
	case 0x0C4F:
	case 0x0C50:
	case 0x0C51:
	case 0x0C52:
	case 0x0C53:
	case 0x0C54: // cotton plants
		if( !mChar->IsOnHorse() ) 
			action( mSock, 0x0D );
		else 
			action( mSock, 0x1D );
		soundeffect( mSock, 0x013E, true );
		i = Items->SpawnItem( mSock, mChar, 1, "#", true, 0x0DF9, 0, true, true );
		if( i == NULL )
			return true;
		CItem *pack;
		pack = getPack( mChar );
		if( pack != NULL )
			i->SetCont( pack );
		CItem *skillItem;
		skillItem = mChar->GetSkillItem();
		if( skillItem != NULL )
		{
			skillItem->SetDecayable( true );
			sysmessage( mSock, 476 );
			mChar->SetSkillItem( NULL );
		}
		return true; // cotton
	case 0x105B:
	case 0x105C:
	case 0x1053:
	case 0x1054: // tinker axle
		mSock->AddID( x->GetSerial() );
		target( mSock, 0, 1, 0, 183, 477 );
		return true;
	case 0x1051:
	case 0x1052:
	case 0x1055:
	case 0x1056:
	case 0x105D:
	case 0x105E:
		mChar->SetSkillItem( x );
		target( mSock, 0, 1, 0, 184, 478 );
		return true;
	case 0x104F:
	case 0x1050:
	case 0x104D:
	case 0x104E:// tinker clock
		target( mSock, 0, 1, 0, 185, 479 );
		Items->DeleItem( x );
		return true;
	case 0x1059:
	case 0x105A://tinker sextant
		if( Skills->CheckSkill( mChar, TINKERING, 0, 1000 ) )
		{
			sysmessage( mSock, 480 );
			i = Items->SpawnItem( mSock, mChar, 1, Dictionary->GetEntry( 1429 ), false, 0x1057, 0, true, true );
			if( i == NULL )
				return true;
			CItem *pack;
			pack = getPack( mChar );
			if( pack != NULL )
				i->SetCont( pack );
			CItem *skillItem;
			skillItem = mChar->GetSkillItem();
			if( skillItem != NULL )
			{
				skillItem->SetDecayable( true );
				Items->DeleItem( x );
				mChar->SetSkillItem( NULL );
			}
		}
		else 
			sysmessage( mSock, 481 );
		return true;
	case 0x1070:
	case 0x1074: // training dummies
		if( objInRange( mChar, x, 1 ) ) 
			Skills->TDummy( mSock );
		else 
			sysmessage( mSock, 482 );
		return true;
	case 0x1071:
	case 0x1073:
	case 0x1075:
	case 0x1077:// swinging training dummy
		sysmessage( mSock, 483 );
		return true;
	case 0x1EBC: // tinker's tools
		target( mSock, 0, 1, 0, 180, 484 );
		return true;
	case 0x0FAF:
	case 0x0FB0:
		target( mSock, 0, 1, 0, 24, 485 );	// What do we wish to repair?
		return true;
	case 0x0EC1:	// Leather repair tool
		target( mSock, 0, 1, 0, 14, 485 );	// What do we wish to repair?
		return true;
	case 0x10E7:	// Bow repair tool
		target( mSock, 0, 1, 0, 15, 485 );	// What do we wish to repair?
		return true;
	default:
		break;
	}
	return false;
}
//o---------------------------------------------------------------------------o
//|   Function    :  void doubleClick( cSocket *mSock )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//|									
//|	Modification:	09/22/2002	-	Xuri - Removed piece of code which is not 
//|									needed. Cooking of raw fish shouldn't produce cooked ribs,
//|									people can cook the fish after filleting with a knife.
//o---------------------------------------------------------------------------o
//|   Purpose     :  Player doubleclicks on a character or item
//o---------------------------------------------------------------------------o
void doubleClick( cSocket *mSock )
{
	SERIAL serial = mSock->GetDWord( 1 );

	UI08 a1 = mSock->GetByte( 1 );

	bool keyboard = ( (a1 & 0x80) == 0x80 );
	if( keyboard )
		serial -= 0x80000000;
	
	if( serial < BASEITEMSERIAL )
	{
		handleCharDoubleClick( mSock, serial, keyboard );
		return;
	}

	// Begin Items / Guildstones Section
	CItem *x = calcItemObjFromSer( serial );
	if( x == NULL )
		return;

	CChar *mChar = mSock->CurrcharObj(), *iChar = NULL;

	// Check for Object Delay
	if( ( static_cast<UI32>(mChar->GetObjectDelay()) >= uiCurrentTime || overflow ) && !mChar->IsGM() )
	{
		sysmessage( mSock, 386 );
		return;
	}
	else
		mChar->SetObjectDelay( BuildTimeValue( static_cast<R32>(cwmWorldState->ServerData()->GetSystemTimerStatus( OBJECT_USAGE ) )) );

	UI08 iType = x->GetType();
	UI16 itemID = x->GetID();
	if( x->GetCont() != NULL )
	{
		if( x->GetCont( 1 ) >= 0x40 )
			iChar = getPackOwner( x );
		else	// Item is in a character
			iChar = (CChar *)x->GetCont();
		if( !mChar->IsGM() && iChar != NULL && iChar != mChar && !( x->IsContType() && cwmWorldState->ServerData()->GetRogueStatus() ) )
		{
			sysmessage( mSock, 387 );	// Can't use stuff that isn't in your pack.
			return;
		}
	}
	// Check if item is in a house?
	else if( iType != 12 && iType != 13 && iType != 117 )
	{
		if( !mChar->IsGM() && x->GetMultiObj() != mChar->GetMultiObj() )
		{
			sysmessage( mSock, 389 );
			return;
		}
	}
	
	if( mChar->IsDead() )
	{
		if( iType == 16 )	// Check for a resurrect item type
		{
			Targ->NpcResurrectTarget( mChar );
			sysmessage( mSock, 390 );
			return;
		}
		else  // If it's not a ressurect item, and you're dead, forget it
		{
			sysmessage( mSock, 392 );
			return;
		}
	}
	// Begin checking objects that we force an object delay for (std objects)
	else if( mSock != NULL )
	{
		UI16 itemTrig = x->GetScriptTrigger();
		cScript *toExecute = Trigger->GetScript( itemTrig );
		if( toExecute != NULL )
		{
			// on ground and not in range
			if( x->GetCont() == NULL && !objInRange( mChar, x, 8 ) )
			{
				sysmessage( mSock, 393 );
				return;
			}
			if( x->IsDisabled() )
			{
				sysmessage( mSock, 394 );
				return;
			}
			if( toExecute->OnUsage( mChar, x ) )
			{
				;
			}
			if( toExecute->OnUse( mChar, x ) )
				return;
		}
		//check this on trigger in the event that the .trigger property is not set on the item
		//trigger code.  Check to see if item is envokable by id
		else if( Trigger->CheckEnvoke( itemID ) )
		{
			UI16 envTrig = Trigger->GetScriptFromEnvoke( itemID );
			cScript *envExecute = Trigger->GetScript( envTrig );
			if( envExecute->OnUse( mChar, x ) )
				return;
		}
	}
	// Range check for double clicking on items
	if( iType != 1 && iType != 117 && iType != 203 && !checkItemRange( mChar, x, 3 ) )
	{
		sysmessage( mSock, 389 );
		return;
	}
	if( x->isCorpse() && !mChar->IsGM() && !mChar->IsCounselor() )
	{
		bool willCrim = false;
		iChar = (CChar *)x->GetOwnerObj();
		if( iChar != NULL )
		{
			bool pInnocent = iChar->IsInnocent();
			bool guildCheck = GuildSys->ResultInCriminal( mChar, iChar ); 
			bool raceCheck = ( Races->Compare( mChar, iChar ) == 0 );
			bool otherCheck = ( pInnocent && iChar->GetSerial() != mChar->GetSerial() ); 
			// if the corpse is from an innocent player, and is not our own corpse				if( otherCheck
			// and if the corpse is not from an enemy/allied guild									&& guildCheck
			// and if the races are not allied/enemy												&& raceCheck )
			willCrim = guildCheck && raceCheck && otherCheck;
		}
		else
			willCrim = ( (x->GetMoreZ()&0x04) == 0x04 );
		if( willCrim )
			criminal( mChar );

		if( iChar != NULL )
		{
			if( iChar->IsGuarded() ) // Is the corpse being guarded?
				petGuardAttack( mChar, iChar, iChar->GetSerial() );
			else if( x->isGuarded() )
				petGuardAttack( mChar, iChar, x->GetSerial() );
		}
	}
	else if( x->isGuarded() )
	{
		CMultiObj *multi = findMulti( x );
		if( multi != NULL )
		{
			if( multi->GetOwnerObj() != mChar )
				petGuardAttack( mChar, (CChar *)multi->GetOwnerObj(), x->GetSerial() );
		}
		else
		{
			iChar = getPackOwner( x );
			if( iChar != NULL && iChar->GetSerial() != mChar->GetSerial() )
				petGuardAttack( mChar, iChar, x->GetSerial() );
		}
	}
	if( handleDoubleClickTypes( mSock, mChar, x, iType ) )
		return;
	if( handleDoubleClickIDs( mSock, mChar, x, itemID ) )
		return;
	
	switch( Combat->getWeaponType( x ) )	// Check weapon-types (For carving/chopping)
	{
	case DEF_SWORDS:
	case SLASH_SWORDS:
	case ONEHND_LG_SWORDS:
	case ONEHND_AXES:
		target( mSock, 0, 1, 0, 86, 462 );
		return;
	case DEF_FENCING:
		if( itemID == 0x0F51 || itemID == 0x0F52 ) //dagger
			target( mSock, 0, 1, 0, 86, 462 );
		return;
	case TWOHND_LG_SWORDS:
	case BARDICHE:
	case TWOHND_AXES:
		target( mSock, 0, 1, 0, 76, 443 );
		return;
	}

	//	Begin Scrolls check
	if( x->GetID( 1 ) == 0x1F && ( x->GetID( 2 ) > 0x2C && x->GetID( 2 ) < 0x6D ) )
	{
		bool success = false;
		mSock->CurrentSpellType( 1 );	// spell from scroll
		if( x->GetID( 2 ) == 0x2D )  // Reactive Armor spell scrolls
			success = Magic->SelectSpell( mSock, 7 ); 
		if( x->GetID( 2 ) >= 0x2E && x->GetID( 2 ) <= 0x34 )  // first circle spell scrolls
			success = Magic->SelectSpell( mSock, x->GetID( 2 ) - 0x2D ); 
		else if( x->GetID( 2 ) >= 0x35 && x->GetID( 2 ) <= 0x6C )  // 2 to 8 circle spell scrolls
			success = Magic->SelectSpell( mSock, x->GetID( 2 ) - 0x2D + 1 ); 
		
		if( success )
			DecreaseItemAmount( x );
		return;
	}
	sysmessage( mSock, 486 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  char *AppendData( CItem *i, char *currentName )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Add data onto the end of the string in singleclick() based
//|					 on an items type
//o---------------------------------------------------------------------------o
char *AppendData( CItem *i, char *currentName )
{
	char temp[256];
	switch( i->GetType() )
	{
	case 1:
	case 12:
	case 63:
	case 65:
		strcpy( temp, currentName );
		sprintf( currentName, "%s (%i items, %i stones)", temp, i->NumItems(), ( i->GetWeight() / 100 ) );
		break;
	case 8:		// containers
	case 13:
	case 64:	// spawn containers
		strcpy( temp, currentName );
		sprintf( currentName, "%s (%i items, %i stones) [Locked]", temp, i->NumItems(), ( i->GetWeight() / 100 ) );
		break;
	case 50:
	case 51:
	case 52:
	case 60:
		strcpy( temp, currentName );
		UI08 newRegion;
		newRegion = calcRegionFromXY( static_cast<SI16>(i->GetMoreX()), static_cast<SI16>(i->GetMoreY()), i->WorldNumber() );
		sprintf( currentName, "%s (%s)", temp, region[newRegion]->GetName() );
	}
	// Question: Do we put the creator thing here, saves some redundancy a bit later
	return currentName;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void singleClick( cSocket *mSock )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when an item or character is single-clicked (also
//|					 used for AllNames macro)
//o---------------------------------------------------------------------------o
void singleClick( cSocket *mSock )
{
	char temp2[200];
	char realname[128];
	char temp[512];
	char mode = 0;
	bool used = false;
	
	SERIAL serial = mSock->GetDWord( 1 );
	UI08 a1 = mSock->GetByte( 1 );
	UI08 a2 = mSock->GetByte( 2 );
	UI08 a3 = mSock->GetByte( 3 );
	UI08 a4 = mSock->GetByte( 4 );
	CChar *mChar = mSock->CurrcharObj();
	// Begin chars/npcs section
	CChar *c = calcCharObjFromSer( serial );
	if( c != NULL )		
	{
		showcname( mSock, c, 0 );
		return;
	}
	//End chars/npcs section
	//Begin items section
	CItem *i = calcItemObjFromSer( serial );
	if( i == NULL )
		return;
	// October 6, 2002 - Brakhtus - Added support for the onClick event
	cScript *onClickScp = Trigger->GetScript(i->GetScriptTrigger());
	if(onClickScp!=NULL)
		onClickScp->OnClick(mSock,i);

	if( mChar->GetSingClickSer() )
	{
		sprintf( temp, "[%x %x %x %x]", a1, a2, a3, a4 );
		objMessage( mSock, AppendData( i, temp ), i );
	}

	UI16 getAmount = i->GetAmount();
	if( i->GetCont() != NULL && i->GetCont( 1 ) >= 0x40 )
	{
		CChar *w = getPackOwner( (CItem *)i->GetCont() );
		if( w != NULL )
		{
			if( w->GetNPCAiType() == 17 )
			{
				if( i->GetCreator() != INVALIDSERIAL && i->GetMadeWith() > 0 ) 
				{
					CChar *mCreater = calcCharObjFromSer( i->GetCreator() );
					if( mCreater != NULL )
						sprintf( temp2, "%s %s by %s", i->GetDesc(), skill[i->GetMadeWith()-1].madeword, mCreater->GetName() );
					else
						strcpy( temp2, i->GetDesc() );
				}
				else
					strcpy( temp2, i->GetDesc() );
				sprintf( temp, "%s at %igp", temp2, i->GetValue() );
				objMessage( mSock, AppendData( i, temp ), i );
				return;
			}
		}
	}

	if( i->GetName()[0] != '#' )
	{
		if( i->GetID() == 0x0ED5 )//guildstone
			sprintf( realname, Dictionary->GetEntry( 101, mSock->Language() ), i->GetName() );
		if( !i->isPileable() || getAmount == 1 )
		{
			if( mChar->IsGM() && !i->isCorpse() && getAmount > 1 )
				sprintf( realname, "%s (%i)", i->GetName(), getAmount );
			else 
				strcpy( realname, i->GetName() );
		}
		else
			sprintf( realname, "%i %ss", getAmount, i->GetName() );
	}
	else
	{
		CTile tile;
		Map->SeekTile( i->GetID(), &tile );
		if( getAmount == 1 )
		{
			if( tile.DisplayAsAn() ) 
				strcpy( temp, "an " );
			else if( tile.DisplayAsA() ) 
				strcpy( temp, "a " );
			else 
				temp[0] = 0;
		}
		else 
			sprintf( temp, "%i ", getAmount );
		const char *tName = tile.Name();
		int namLen = strlen( temp );

		for( UI32 j = 0; j < strlen( tName ); j++ )
		{
			if( tName[j] == '%' )
			{
				if( mode )
					mode = 0;
				else
					mode = 2;
			}
			else if( tName[j] == '/' && mode == 2 )
				mode = 1;
			else if( mode == 0 || ( mode == 1 && getAmount == 1 ) || ( mode == 2 && getAmount > 1 ) )
			{
				temp[namLen++] = tName[j];
				temp[namLen  ] = '\0';
				if( mode )
					used = true;
			}
		}
		if( !used && getAmount > 1 )
		{
			temp[namLen++] = 's';
			temp[namLen  ] = '\0';
		}
		strcpy( realname, temp );
	}
	if( i->GetType() == 15 )
	{
		int spellNum = ( 8 * ( i->GetMoreX() - 1 ) ) + i->GetMoreY() - 1;	// we pick it up from the array anyway
		char buf[10];
		strcat( realname, " of " );
		strcat( realname, Dictionary->GetEntry( magic_table[spellNum].spell_name, mSock->Language() ) );
		strcat( realname, " with " );
		sprintf( buf, "%d", i->GetMoreZ() );
		strcat( realname, buf );
		strcat( realname, " charges" );
	}
	if( i->GetCreator() != INVALIDSERIAL && i->GetMadeWith() > 0 )
	{
		CChar *mCreater = calcCharObjFromSer( i->GetCreator() );
		if( mCreater != NULL )
			sprintf( temp2, "%s %s by %s", realname, skill[i->GetMadeWith()-1].madeword, mCreater->GetName() );
		else
			strcpy( temp2, realname );
	}
	else
		strcpy( temp2, realname );
	objMessage( mSock, temp2, i );
	if( i->IsLockedDown() )
		objMessage( mSock, "[Locked down]", i );//, 0x0481 );
	if( i->isGuarded() )
		objMessage( mSock, "[Guarded]", i );//, 0x0481 );
}
