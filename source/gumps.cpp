#include "uox3.h"
#include "gump.h"

#include "cGuild.h"
#include "townregion.h"
#include "cServerDefinitions.h"
#include "wholist.h"
#include "skills.h"
#include "cMagic.h"
#include "cVersionClass.h"
#include "ssection.h"
#include "trigger.h"
#include "cScript.h"
#include "cEffects.h"
#include "packets.h"

#undef DBGFILE
#define DBGFILE "gumps.cpp"

std::string GetStrFromSock( cSocket *sock, UI16 offset, UI16 length );
std::string GetUniStrFromSock( cSocket *sock, UI16 offset, UI16 length );

//o--------------------------------------------------------------------------o
//|	Function			-	void HandleAccountButton( cSocket *s, long button, CChar *j )
//|	Date					-	
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Handles the accounts gump button
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void HandleAccountButton( cSocket *s, long button, CChar *j )
{
	if( j == NULL )
		return;
	CChar *mChar = s->CurrcharObj();
	ACCOUNTSBLOCK actbTemp;
	if(!Accounts->GetAccountByID(j->GetAccount().wAccountIndex,actbTemp))
		return;
	//
	cSocket *targSocket = calcSocketObjFromChar( j );
	switch( button )
	{
	case 2:
		if( mChar->IsGM() && !j->IsNpc() )
		{
			sysmessage( s, 487 );
			sysmessage( targSocket, 488 );
			actbTemp.wFlags|=AB_FLAGS_BANNED;	// Banned
			if( isOnline( j ) ) 
				Network->Disconnect( calcCharFromSer( j->GetSerial() ) );
		}
		else 
			sysmessage( s, 489 );
		break;
	case 3:
		if( mChar->IsGM() && !j->IsNpc() )
		{
			sysmessage( s, 490 );
			sysmessage( targSocket, 491 );
			actbTemp.wFlags|=AB_FLAGS_BANNED;
			actbTemp.wTimeBan=static_cast<UI16>(BuildTimeValue( 60 * 60 * 24 ));

			if( isOnline( j ) ) 
				Network->Disconnect( calcCharFromSer( j->GetSerial() ) );
		}
		else 
			sysmessage( s, 492 );
		break;
	case 4:		
	case 5:		
	case 6:		
	case 7:
		sysmessage( s, 489 + button );
		break;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleTweakItemButton( cSocket *s, long button, SERIAL ser, long type )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button pressed in tweak item gump
//o---------------------------------------------------------------------------o
void HandleTweakItemButton( cSocket *s, long button, SERIAL ser, long type )
{
	button -= 6;
	if( button <= 0 )
	{
		sysmessage( s, 1700 );
		return;
	}

	switch( button )
	{
	case 15:	// Low Damage
	case 16:	// High Damage
	case 17:	// Defense
	case 18:	// Speed
	case 19:	// Rank
		entrygump( s, ser, static_cast<char>(type), static_cast<char>(button), 4, 495 + button );	// allow 0x for hex value
		break;
	case 1:		// ID
	case 4:		// Colour
	case 5:		// Layer
	case 6:		// Type
	case 8:		// X
	case 9:		// Y
	case 10:	// Z
	case 13:	// Hit Points
	case 14:	// Max HitPoints
	case 20:	// More
	case 21:	// More X
	case 22:	// More Y
	case 23:	// More Z
	case 24:	// More B
	case 28:	// Good
	case 29:	// Value
	case 30:	// Carve
		entrygump( s, ser, static_cast<char>(type), static_cast<char>(button), 6, 495 + button );	// allow 0x for hex value
		break;
	case 7:		// Moveable
	case 25:	// Poisoned
	case 27:	// Decay
	case 31:	// Stackable
	case 32:	// Dyable
	case 33:	// Corpse
	case 34:	// Visible
		entrygump( s, ser, static_cast<char>(type), static_cast<char>(button), 1, 495 + button );
		break;
	case 11:	// Amount
	case 12:	// Strength
	case 26:	// Weight
		entrygump( s, ser, static_cast<char>(type), static_cast<char>(button), 7, 495 + button );	// allow 0x for hex value
		break;
	case 2:		// Name
	case 3:		// Name 2
	case 35:	// Creator
		entrygump( s, ser, static_cast<char>(type), static_cast<char>(button), 50, 495 + button );
		break;
	default:	Console << Dictionary->GetEntry( 525 ) << (SI32)button << myendl;	break;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleTweakCharButton( cSocket *s, long button, SERIAL ser, long type )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button pressed in tweak char gump
//o---------------------------------------------------------------------------o
void HandleTweakCharButton( cSocket *s, long button, SERIAL ser, long type )
{
	button -= 6;
	if( button <= 0 )
	{
		sysmessage( s, 1700 );
		return;
	}

	switch( button )
	{
	case 1:		// Name
	case 2:		// Title
		entrygump( s, ser, static_cast<char>(type), static_cast<char>(button), 50, 1700 + button );
		break;
	case 3:		// Body
	case 4:		// Skin
	case 5:		// Race
	case 6:		// X
	case 7:		// Y
	case 8:		// Z
	case 9:		// Direction
	case 13:	// Hitpoints
	case 14:	// Stamina
	case 15:	// Mana
	case 21:	// Kills
	case 27:	// Carve
		entrygump( s, ser, static_cast<char>(type), static_cast<char>(button), 6, 1700 + button );	// allow 0x for hex
		break;
	case 10:	// Strength
	case 11:	// Dexterity
	case 12:	// Intelligence
	case 19:	// Fame
	case 20:	// Karma
	case 26:	// Weight
		entrygump( s, ser, static_cast<char>(type), static_cast<char>(button), 7, 1700 + button );	// allow 0x for hex
		break;

	case 16:	// Low Damage
	case 17:	// High Damage
	case 18:	// Defense
	case 22:	// AI Type
	case 24:	// Hunger
		entrygump( s, ser, static_cast<char>(type), static_cast<char>(button), 4, 1700 + button );	// allow 0x for hex
		break;
	case 23:	// NPC Wander
	case 25:	// Poison
	case 28:	// Visible
		entrygump( s, ser, static_cast<char>(type), static_cast<char>(button), 1, 1700 + button );
		break;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleTownstoneButton( cSocket *s, long button, SERIAL ser, long type )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button pressed in townstone gump
//o---------------------------------------------------------------------------o
void HandleTownstoneButton( cSocket *s, long button, SERIAL ser, long type )
{
	CChar *mChar = s->CurrcharObj();
	UI08 targetRegion;
	switch( button )
	{
		// buttons 2-20 are for generic town members
		// buttons 21-40 are for mayoral functions
		// buttons 41-60 are for potential candidates
		// buttons 61-80 are for enemy towns
	case 2:		// leave town
		bool result;
		// safe to assume we want to remove ourselves from our only town!
		result = region[mChar->GetTown()]->RemoveTownMember( (*mChar) );
		if( !result )
			sysmessage( s, 540 );
		break;
	case 3:		// view taxes
		targetRegion = calcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber() );
		if( targetRegion != 0xFF )
			region[targetRegion]->ViewTaxes( s );
		else
			sysmessage( s, 541 );
		break;
	case 4:		// toggle town title
		mChar->SetTownTitle( !mChar->GetTownTitle() );
		region[mChar->GetTown()]->DisplayTownMenu( NULL, s );
		break;
	case 5:		target( s, 0, 161, 542 );								break;		// vote for town mayor
	case 6:		entrygump(  s, ser, static_cast<char>(type), static_cast<char>(button), 6, 543 );	break;		// gold donation
	case 7:		region[mChar->GetTown()]->ViewBudget( s );			break;		// View Budget
	case 8:		region[mChar->GetTown()]->SendAlliedTowns( s );		break;		// View allied towns
	case 9:		region[mChar->GetTown()]->SendEnemyTowns( s );		break;
	case 20:	// access mayoral functions, never hit here if we don't have mayoral access anyway!
				// also, we'll only get access, if we're in our OWN region
		region[mChar->GetTown()]->DisplayTownMenu( NULL, s, 0x01 );	// mayor
		break;
	case 21:	sysmessage( s, 544 );				break;	// set taxes
	case 22:	region[mChar->GetTown()]->DisplayTownMembers( s );	break;	// list town members
	case 23:	region[mChar->GetTown()]->ForceEarlyElection();		break;	// force early election
	case 24:	sysmessage( s, 545 );	break;	// purchase more guards
	case 25:	sysmessage( s, 546 );	break;	// fire a guard
	case 26:	target( s, 0, 49, 547 );								break;	// make a town an ally
	case 40:	region[mChar->GetTown()]->DisplayTownMenu( NULL, s );	break;	// we can't return from mayor menu if we weren't mayor!
	case 41:	// join town!
		if( !(region[calcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber() )]->AddAsTownMember( (*mChar) ) ) )
			sysmessage( s, 548 );
		break;
	case 61:	// seize townstone!
		if( !Skills->CheckSkill( mChar, STEALING, 950, 1000 )	) // minimum 95.0 stealing
		{
			targetRegion = calcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber() );
			if( targetRegion != 0xFF )
			{
				int chanceToSteal = RandomNum( 0, region[targetRegion]->GetHealth() / 2 );
				int fudgedStealing = RandomNum( mChar->GetSkill( STEALING ), mChar->GetSkill( STEALING ) * 2 );
				if( fudgedStealing >= chanceToSteal )
				{
					// redo stealing code here
					sysmessage( s, 549, region[targetRegion]->GetName() );
					region[targetRegion]->DoDamage( region[targetRegion]->GetHealth() / 2 );	// we reduce the region's health by half
					int xOffset = MapRegion->GetGridX( mChar->GetX() );
					int yOffset = MapRegion->GetGridY( mChar->GetY() );
					UI08 worldNumber = mChar->WorldNumber();
					for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
					{
						for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
						{
							SubRegion *toCheck = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
							if( toCheck == NULL )	
								continue;
							toCheck->PushItem();
							for( CItem *itemCheck = toCheck->FirstItem(); !toCheck->FinishedItems(); itemCheck = toCheck->GetNextItem() )
							{
								if( itemCheck == NULL )		
									continue;
								if( itemCheck->GetType() == 35 && itemCheck->GetID() != 0x14F0 )
								{	// found our townstone
									CItem *charPack = getPack( mChar );
									if( charPack != NULL )
									{
										itemCheck->SetCont( charPack );
										itemCheck->SetMoreX( targetRegion );
										sysmessage( s, 550 );
										region[targetRegion]->TellMembers( 551, mChar->GetName() );
										toCheck->PopItem();
										return;	// dump out
									}
								}
							}
							toCheck->PopItem();
						}
					}
				}
				else
					sysmessage( s, 552 );
			}
			else
				sysmessage( s, 553 );
		}
		else
			sysmessage( s, 554 );
		break;
	case 62:	// attack townstone
		targetRegion = calcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber() );
		for( UI08 counter = 0; counter < RandomNum( 5, 10 ); counter++ )
		{
			Effects->movingeffect( mChar, mChar->GetX() + RandomNum( -6, 6 ), mChar->GetY() + RandomNum( -6, 6 ), mChar->GetZ(), 0x36E4, 17, 0, ( RandomNum( 0, 1 ) == 1 ) );
			Effects->staticeffect( mChar->GetX() + RandomNum( -6, 6 ), mChar->GetY() + RandomNum( -6, 6 ), mChar->GetZ(), 0x373A + RandomNum( 0, 4 ) * 0x10, 0x09, 0, 0 );
		}
		region[targetRegion]->DoDamage( RandomNum( 0, region[targetRegion]->GetHealth() / 8) );	// we reduce the region's health by half
		break;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleHairDyeButton( cSocket *s, CItem *j )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button pressed in hair dye gump
//o---------------------------------------------------------------------------o
void HandleHairDyeButton( cSocket *s, CItem *j )
{
	if( j == NULL )
		return;

	// we only HAVE one button, so we'll assume safely that the colour has changed if we hit here
	UI16 dyeColour = s->GetWord( 21 );
	UseHairDye( s, dyeColour, j );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleAccountModButton( cSocket *s, long button )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button pressed in account gump
//o---------------------------------------------------------------------------o
void HandleAccountModButton( CPIGumpMenuSelect *packet )
{
	cSocket *s = packet->GetSocket();

	std::string username	= "";
	std::string password	= "";
	std::string emailAddy	= "";

	for( unsigned int i = 0; i < packet->TextCount(); i++ )
	{
		UI16 textID = packet->GetTextID( i );
		switch( textID )
		{
		case 1000:	username	= packet->GetTextUString( i );	break;
		case 1001:	password	= packet->GetTextUString( i );	break;
		case 1002:	emailAddy	= packet->GetTextUString( i );	break;
		default:
			Console.Warning( 1, "Unknown textID %i with string %s", textID, packet->GetTextUString( i ).c_str() );
		}
	}

	ACCOUNTSBLOCK actbAccountFind;
	if( Accounts->GetAccountByName( username, actbAccountFind ) )
	{
		sysmessage( s, 555 );
		return;
	}
	Console.Print( "Attempting to add username %s with password %s at emailaddy %s", username.c_str(), password.c_str(), emailAddy.c_str() );
//	Accounts->AddAccount( username, password, emailAddy );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleHouseButton( cSocket *s, long button, CItem *j )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button pressed in house gump
//o---------------------------------------------------------------------------o
void HandleHouseButton( cSocket *s, long button, CItem *j )
{
	if( j == NULL )
		return;

	if( button != 20 && button != 2 ) 
		s->AddID( j->GetSerial() );
	switch( button )
	{
	case 20: // Change house sign's appearance
		if( s->GetWord( 21 ) > 0 ) 
		{
			s->SetDWord( 7, s->AddID() );
			s->AddID1( s->GetByte( 21 ) );
			s->AddID2( s->GetByte( 22 ) );
			Targ->IDtarget( s );
			sysmessage( s, 556 );
		}
		return;
	case 0:		return;
	case 2:	target( s, 0, 227, 557 );	return;  // Bestow ownership upon someone else
	case 3:	deedHouse( s, j );				return;  // Turn house into a deed
	case 4:	target( s, 0, 228, 558 );	return;  // Kick someone out of house
	case 5:	target( s, 0, 229, 559 );	return;  // Ban somebody
	case 6:
	case 8:	target( s, 0, 231, 560 );	return; // Remove someone from house list
	case 7:	target( s, 0, 230, 561 );	return; // Make someone a friend
	default:
		char temp[1024];
		sprintf( temp, "HouseGump Called - Button=%i", button );
		sysmessage( s, temp );
		return;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleAddMenuButton( cSocket *s, long button )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button pressed in add menu gump
//o---------------------------------------------------------------------------o
void HandleAddMenuButton( cSocket *s, long button )
{
	CChar *mChar = s->CurrcharObj();
	SI32 addMenuLoc = mChar->GetAddPart();
	char sect[512];
	sprintf( sect, "ITEMMENU %i", addMenuLoc );
	ScriptSection *ItemMenu = FileLookup->FindEntry( sect, items_def );
	if( ItemMenu == NULL )
		return;

	const char *tag = ItemMenu->First();
	const char *data = ItemMenu->GrabData();	// let's skip the title
	UI08 counter = 7;

	for( tag = ItemMenu->Next(); !ItemMenu->AtEnd(); tag = ItemMenu->Next(), counter++ )
	{	// let's skip over the name, and get straight to where we should be headed
		tag = ItemMenu->Next();
		data = ItemMenu->GrabData();
		if( counter == button )
		{	// we've found it
			scriptcommand( s, tag, data );
			break;
		}
		
	}
}

extern void HandleHowTo( cSocket *sock, int cmdNumber );
 
void HandleHowToButton( cSocket *s, long button )
{
	HandleHowTo( s, button - 2 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void void cGump::Button( CPIGumpMenuSelect *packet )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button press in gumps
//o---------------------------------------------------------------------------o
void cGump::Button( CPIGumpMenuSelect *packet )
{
	if( packet == NULL )
		return;
	cSocket *s = packet->GetSocket();
	if( s == NULL )
		return;
	CChar *mChar = s->CurrcharObj();

	UI32 button = packet->ButtonID();
	if( button > 10000 ) 
	{
		button -= 10000;
		Menu( s, button );
		return;
	}
	SERIAL serial = packet->ID();
	SERIAL gumpID = packet->GumpID();
	if( gumpID >= 8000 && gumpID <= 8020 )
	{
		GuildSys->GumpChoice( s );
		// guild collection call goes here
		return;
	}
	else if( gumpID == 20 )	// script gump
	{
		UI16 gumpTrig = (UI16)s->TempInt();
		cScript *toExecute = Trigger->GetScript( gumpTrig );
		if( toExecute != NULL )
			toExecute->HandleGumpPress( packet );
	}
	else if( gumpID == 21 ) // Multi functional gump
	{
		MultiGumpCallback( s, serial, button );
		return;
	}
	else if( gumpID == 0x01CD )
	{
#pragma note( "Newly passed button press due to the pentagram in the 2D and 3D clients" )
#pragma note( "The question is... what does it DO?" )
	}
	else if( gumpID > 13 ) 
		return; //increase this value with each new gump added.
	if( button == 1 ) 
	{
		if( gumpID == 4 )
			WhoList->GMLeave();
		else if( gumpID == 11 )
			OffList->GMLeave();
		return;
	}

	// gump types
	// TBD  To Be Done
	// TBR  To Be Replaced
	// TBDU To Be Decided Upon
	// 1	Tweak Item
	// 2	Tweak Char
	// 3	Townstones
	// 4	WhoList
	// 5	House functions (TBR)
	// 6	Hair Dye Menu
	// 7	Accounts (TBR)
	// 8	Racial editor (TBDU)
	// 9	Add menu
	// 10	Safe sex window (TBD)
	// 11	Who's Offline
	// 12	New Make Menu
	// 13	HOWTO

#ifdef DEBUG
	Console << "Type is " << gumpID << " button is " <<  button << myendl;
#endif

	SERIAL is;
	CItem *j;
	switch( gumpID )
	{
	case 1:	HandleTweakItemButton( s, button, serial, gumpID );			break;	// Tweak Item
	case 2:	HandleTweakCharButton( s, button, serial, gumpID );			break;	// Tweak Char
	case 3:	HandleTownstoneButton( s, button, serial, gumpID );			break;	// Townstones
	case 4:	WhoList->ButtonSelect( s, static_cast<UI16>(button), static_cast<UI08>(gumpID) );					break;	// Wholist
	case 5:																		// House Functions
		is = s->AddID();
		j = calcItemObjFromSer( is );
		if( j != NULL ) 
		{
			is = j->GetMore(); 
			j = calcItemObjFromSer( is );
		}
		HandleHouseButton( s, button, j );
		break;
	case 6:																		// Hair Dye Menu
		is = s->AddID();
		j = calcItemObjFromSer( is );
		HandleHairDyeButton( s, j );
		break;
	case 7:																		// Accounts
		CChar *c;
		c = calcCharObjFromSer( serial );
		HandleAccountButton( s, button, c );
		break;
	case 8:	HandleAccountModButton( packet );							break;	// Race Editor
	case 9:	HandleAddMenuButton( s, button );							break;	// Add Menu
	case 11: OffList->ButtonSelect( s, static_cast<UI16>(button), static_cast<UI08>(gumpID) );				break;	// Who's Offline
	case 12: Skills->HandleMakeMenu( s, button, static_cast<int>(mChar->GetMaking() ));	break;	// New Make Menu
	case 13: HandleHowToButton( s, button );							break;	// Howto
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleTweakItemText( cSocket *s, long index )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles new values for 'tweak items
//o---------------------------------------------------------------------------o
void HandleTweakItemText( cSocket *s, long index )
{
	CChar *mChar = s->CurrcharObj();
	char *text = (char *)&(s->Buffer()[12]);
	SERIAL serial = s->GetDWord( 3 );

	if( mChar->IsGM() )
	{
		CItem *j = calcItemObjFromSer( serial );
		if( j == NULL ) 
			return;
		if( s->GetByte( 9 ) == 0 )
		{
			tweakItemMenu( s, j );
			return;
		}
		switch( index )
		{
		case 1:		j->SetID( static_cast<UI16>(makeNum( text )) );				break;	// ID
		case 2:		j->SetName( text );							break;	// Name
		case 3:		j->SetName2( text );						break;	// Name 2
		case 4:		j->SetColour( static_cast<UI16>(makeNum( text )) );			break;	// Colour
		case 5:		j->SetLayer( static_cast<SI08>(makeNum( text ) ));				break;	// Layer
		case 6:		j->SetType( static_cast<UI08>(makeNum( text )) );				break;	// Type
		case 7:		j->SetMovable( static_cast<SI08>(makeNum( text ) ));				break;	// Moveable
		case 8:
			MapRegion->RemoveItem( j );
			j->SetX( static_cast<SI16>(makeNum( text )) );
			MapRegion->AddItem( j );
			break;	// X
		case 9:		
			MapRegion->RemoveItem( j );
			j->SetY( static_cast<SI16>(makeNum( text )) );					
			MapRegion->AddItem( j );
			break;	// Y
		case 10:	j->SetZ(static_cast<SI08>( makeNum( text )) );					break;	// Z
		case 11:	j->SetAmount( (UI32)makeNum( text ) );			break;	// Amount
		case 12:	j->SetStrength( static_cast<SI16>(makeNum( text )) );			break;	// Strength
		case 13:	j->SetHP( static_cast<SI16>(makeNum( text )) ); 				break;	// Hit Points
		case 14:	j->SetMaxHP( static_cast<SI16>(makeNum( text) ) );				break;	// Max HP
		case 15:	j->SetLoDamage( static_cast<SI16>(makeNum( text )) );			break;	// Low Damage
		case 16:	j->SetHiDamage( static_cast<SI16>(makeNum( text )) );			break;	// High Damage
		case 17:	j->SetDef( static_cast<UI16>(makeNum( text )) );				break;	// Defense
		case 18:	j->SetSpeed( static_cast<UI08>(makeNum( text )) );				break;	// Speed
		case 19:	j->SetRank( static_cast<SI08>(makeNum( text )) );				break;	// Rank
		case 20:	j->SetMore( makeNum( text ) );				break;	// More
		case 21:	j->SetMoreX( makeNum( text ) );				break;	// More X
		case 22:	j->SetMoreY( makeNum( text ) );				break;	// More Y
		case 23:	j->SetMoreZ( makeNum( text ) );				break;	// More Z
		case 24: 	j->SetMoreB( makeNum( text ) );				break;	// More B
		case 25:	j->SetPoisoned( static_cast<UI08>(makeNum( text) ) );			break;	// Poisoned
		case 26:	j->SetWeight( makeNum( text ) );			break;	// Weight
		case 27:	j->SetDecayable( makeNum( text ) != 0 );	break;	// Decay
		case 28:	j->SetGood( makeNum( text ) );				break;	// Good
		case 29:	j->SetBuyValue( makeNum( text ) );			break;	// Buy Value
		case 30:	j->SetSellValue( makeNum( text ) );			break;	// Sell Value - Not implimented yet
		case 31:	j->SetCarve( makeNum( text ) );				break;	// Carve
		case 32: 	j->SetPileable( makeNum( text ) != 0 );		break;	// Stackable
		case 33:	j->SetDye( makeNum( text ) != 0 );			break;	// Dyable
		case 34:	j->SetCorpse( makeNum( text ) != 0 );		break;	// Corpse
		case 35:	j->SetVisible( static_cast<SI08>(makeNum( text ) ));			break;	// Visible
		case 36:	j->SetCreator( makeNum( text ) );			break;	// Creator
		}
		RefreshItem( j );
		tweakItemMenu( s, j );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleTweakCharText( cSocket *s, long index )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles new values for 'tweak chars
//o---------------------------------------------------------------------------o
void HandleTweakCharText( cSocket *s, long index )
{
	CChar *mChar = s->CurrcharObj();
	char *text = (char *)&(s->Buffer()[12]);
	SERIAL serial = s->GetDWord( 3 );
	int k;
	if( mChar->IsGM() )
	{
		CChar *j = calcCharObjFromSer( serial );
		if( j == NULL ) 
			return;

		if( s->GetByte( 9 ) == 0 )
		{
			tweakCharMenu( s, j );
			return;
		}
		switch( index )
		{
		case 1:		j->SetName( text );					break;	// Name
		case 2:		j->SetTitle( text );				break;	// Title
		case 3:													// Body
			k = makeNum( text );
			if( k >= 0x000 && k <= 0x3e1 ) // body-values >0x3e crash the client
			{ 
				j->SetID( (UI16)k );
				j->SetxID( (UI16)k );
				j->SetOrgID( (UI16)k );
			}
			break;
		case 4:													// Skin
			k = makeNum( text );
			j->SetColour( (UI16)k );
			j->SetxSkin( (UI16)k );
			j->SetOrgSkin( (UI16)k );
			break;
		case 5:		j->SetRace( static_cast<RACEID>(makeNum( text )) );			break;	// Race
		case 6:		
			MapRegion->RemoveChar( j );
			j->SetX( (SI16)makeNum( text ) );
			MapRegion->AddChar( j );
			break;	// X
		case 7:		
			MapRegion->RemoveChar( j );
			j->SetY( (SI16)makeNum( text ) );			
			MapRegion->AddChar( j );
			break;	// Y
		case 8:													// Z
			j->SetZ( static_cast<SI08>(makeNum( text) ) );
			j->SetDispZ( static_cast<SI08>(makeNum( text )) );
			break;
		case 9:		j->SetDir( static_cast<UI08>(makeNum( text )&0x0F) );		break;	// Direction
		case 10:												// Strength
			if( makeNum( text ) > 0 ) 
				j->SetStrength( static_cast<SI16>(makeNum( text) ) ); 
			break;
		case 11:												// Dexterity
			if( makeNum( text ) > 0 ) 
				j->SetDexterity(static_cast<SI16>( makeNum( text )) );
			break;
		case 12:												// Intelligence
			if( makeNum( text ) > 0 ) 
				j->SetIntelligence( static_cast<SI16>(makeNum( text )) ); 
			break;
		case 13:												// Hitpoints
			if( makeNum( text ) > 0 )
				j->SetHP( static_cast<SI16>(makeNum( text )) );
			break;
		case 14:												// Stamina
			if( makeNum( text ) > 0 )
				j->SetStamina( static_cast<UI16>(makeNum( text )) );
			break;
		case 15:												// Mana
			if( makeNum( text ) > 0 )
				j->SetMana( static_cast<SI16>(makeNum( text )) );
			break;
		case 16:	j->SetHiDamage( static_cast<SI16>(makeNum( text )) );		break;	// Low Damage
		case 17:	j->SetLoDamage( static_cast<SI16>(makeNum( text )) );		break;	// High Damage
		case 18:	j->SetDef( static_cast<UI16>(makeNum( text )) );			break;	// Defense
		case 19:	j->SetFame( static_cast<SI16>(makeNum( text )) );			break;	// Fame
		case 20:	j->SetKarma( static_cast<SI16>(makeNum( text )) );		break;	// Karma
		case 21:	j->SetKills( static_cast<SI16>(makeNum( text )) );		break;	// Kills
		case 22:												// AI Type
			if( j->IsNpc() )
				j->SetNPCAiType( static_cast<SI16>(makeNum( text )) );
			break;
		case 23:												// NPC Wander
			if( j->IsNpc() )
				j->SetNpcWander( static_cast<SI08>(makeNum( text ) ));
			break;
		case 24:	j->SetHunger( static_cast<SI08>(makeNum( text )) );		break;	// Hunger
		case 25:	j->SetPoison( static_cast<SI08>(makeNum( text ) ));		break;	// Poison
		case 26:	j->SetWeight( static_cast<SI16>(makeNum( text )) );		break;	// Weight
		case 27:	j->SetCarve( makeNum( text ) );		break;	// Carve
		case 28:	j->SetVisible( static_cast<SI08>(makeNum( text ) ));		break;	// Visible
		}
		j->Teleport();
		tweakCharMenu( s, j );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleTownstoneText( cSocket *s, long index )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles new values for townstones
//o---------------------------------------------------------------------------o
void HandleTownstoneText( cSocket *s, long index )
{
	CChar *mChar = s->CurrcharObj();
	char *text = (char *)&(s->Buffer()[12]);
	UI16 resourceID;
	int amountToDonate;
	switch( index )
	{
	case 6:	// it's our donate resource button
		amountToDonate = makeNum( text );
		if( amountToDonate == 0 )
		{
			sysmessage( s, 562 );
			return;
		}
		resourceID = region[mChar->GetTown()]->GetResourceID();
		int numResources;
		numResources = GetAmount( mChar, resourceID );

		if( amountToDonate > numResources )
			sysmessage( s, 563, numResources );
		else
		{
			DeleteQuantity( mChar, resourceID, amountToDonate );
			region[mChar->GetTown()]->DonateResource( s, amountToDonate );
		}
		break;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cGump::Input( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles new values for gumps
//o---------------------------------------------------------------------------o
void cGump::Input( cSocket *s )
{
	char type = s->GetByte( 7 );
	char index = s->GetByte( 8 );

	switch( type )
	{
	case 1:		HandleTweakItemText( s, index );	break;
	case 2:		HandleTweakCharText( s, index );	break;
	case 3:		HandleTownstoneText( s, index );	break;
	case 20:
		{
			UI16 gumpTrig = static_cast<UI16>(s->TempInt());
			cScript *toExecute = Trigger->GetScript( gumpTrig );
			if( toExecute != NULL )
				toExecute->HandleGumpInput( s );
			break;
		}
	case 100:	GuildSys->GumpInput( s );			break;
	}

}

enum TXTREPLACEMENT
{
	TXT_ACCOUNTCOUNT	=	0,
	TXT_NUMBERPCS,
	TXT_NUMCHARACTERS,
	TXT_NUMBERNPCS,
	TXT_NUMBERITEMS,
	TXT_SERVERTIME,
	TXT_SERVERVERSION,
	TXT_PLAYERSONLINE,
	TXT_COUNT
};

extern cVersionClass CVC;

const std::string replacements[TXT_COUNT] =
{
	"%accountcount%",
	"%pcs%",
	"%chars%",
	"%npcs",
	"%items%",
	"%servertime",
	"%serverver",
	"%onlinecount%"
};

std::string ReplaceObj( TXTREPLACEMENT toReplace )
{
	char temp[128];
	UI32 ampm = cwmWorldState->ServerData()->GetServerTimeAMPM();
	SI16 hour = cwmWorldState->ServerData()->GetServerTimeHours();
	SI16 minute = cwmWorldState->ServerData()->GetServerTimeMinutes();

	switch( toReplace )
	{
	case TXT_ACCOUNTCOUNT:
		sprintf( temp, "%i", Accounts->size() );
		return temp;
	//case TXT_NUMBERPCS:
		//sprintf( temp, "%i", GPCL.count() );
		//return temp;
	case TXT_NUMCHARACTERS:
		sprintf( temp, "%i", cwmWorldState->GetCharCount() );
		return temp;
	//case TXT_NUMBERNPCS:
		//sprintf( temp, "%i", charcount - GPCL.count() );
		//return temp;
	case TXT_NUMBERITEMS:
		sprintf( temp, "%i", cwmWorldState->GetItemCount() );
		return temp;
	case TXT_SERVERTIME:
		sprintf( temp, " %i:%i %s", hour, minute, ((ampm!=0)?"pm":"am") );
		return temp;
	case TXT_SERVERVERSION:
		sprintf( temp, " %s v%i(%i)", CVC.GetProductName(), CVC.GetVersion(), CVC.GetBuild() );
		return temp;
	case TXT_PLAYERSONLINE:
		sprintf( temp, "%i", cwmWorldState->GetPlayersOnline() );
		return temp;
	default:
		sprintf(temp," ") ;
		break;
	}
	return "";
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cGump::Menu( cSocket *s, int m )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Pull gump info from misc.scp
//o---------------------------------------------------------------------------o
void cGump::Menu( cSocket *s, int m )
{
	char sect[512];

	STRINGLIST one, two;
	
	sprintf( sect, "GUMPMENU %i", m );
	ScriptSection *gump = FileLookup->FindEntry( sect, misc_def );
	if( gump == NULL )
		return;
	const char *tag = NULL;
	char temp[512];
	UI08 targType = 0x12;
	for( tag = gump->First(); !gump->AtEnd(); tag = gump->Next() )
	{
		sprintf( temp, "%s %s", tag, gump->GrabData() );
		if( strncmp( temp, "type ", 5 ) )
			one.push_back( temp );
		else
			targType = static_cast<UI08>(makeNum( &temp[5] ));
	}
	sprintf( sect, "GUMPTEXT %i", m );
	gump = FileLookup->FindEntry( sect, misc_def );
	if( gump == NULL )
		return;
	for( tag = gump->First(); !gump->AtEnd(); tag = gump->Next() )
	{
		sprintf( temp, "%s %s", tag, gump->GrabData() );
		two.push_back( temp );
	}
	SendVecsAsGump( s, one, two, targType, INVALIDSERIAL );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void tweakCharMenu( cSocket *s, CChar *c )
//|   Date        :  Unknown
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handle new tweak character menu
//o---------------------------------------------------------------------------o
void tweakCharMenu( cSocket *s, CChar *c )
{
	if( c == NULL )
		return;

	GumpDisplay tweakChar( s, 350, 345 );
	tweakChar.SetTitle( "Tweak Character Menu" );

	UI16 charID = c->GetID();
	UI16 getSkin = c->GetSkin();

	if( c->GetName() != NULL && c->GetName()[0] > 0 )
		tweakChar.AddData( "Name", c->GetName() );
	else
		tweakChar.AddData( "Name", "None" );
	if( c->GetTitle() != NULL && c->GetTitle()[0] > 0 )
		tweakChar.AddData( "Title", c->GetTitle() );
	else
		tweakChar.AddData( "Title", "None" );
	tweakChar.AddData( "Body", charID, 5 );
	tweakChar.AddData( "Skin", getSkin, 5 );
	tweakChar.AddData( "Race", c->GetRace() );
	tweakChar.AddData( "X coord", c->GetX() );
	tweakChar.AddData( "Y coord", c->GetY() );
	tweakChar.AddData( "Z coord", c->GetZ() );
	tweakChar.AddData( "Direction", c->GetDir() );
	tweakChar.AddData( "Strength", c->GetStrength() );
	tweakChar.AddData( "Dexterity", c->GetDexterity() );
	tweakChar.AddData( "Intelligence", c->GetIntelligence() );
	tweakChar.AddData( "Hitpoints", c->GetHP() );
	tweakChar.AddData( "Stamina", c->GetStamina() );
	tweakChar.AddData( "Mana", c->GetMana() );
	tweakChar.AddData( "LowDamage", c->GetLoDamage() );
	tweakChar.AddData( "HighDamage", c->GetHiDamage() );
	tweakChar.AddData( "Defense", c->GetDef() );
	tweakChar.AddData( "Fame", c->GetFame() );
	tweakChar.AddData( "Karma", c->GetKarma() );
	tweakChar.AddData( "Kills", c->GetKills() );
	tweakChar.AddData( "AI Type", c->GetNPCAiType()  );
	tweakChar.AddData( "NPC Wander", c->GetNpcWander() );
	tweakChar.AddData( "Hunger", c->GetHunger() );
	tweakChar.AddData( "Poison", c->GetPoison() );
	tweakChar.AddData( "Weight", c->GetWeight() );
	tweakChar.AddData( "Carve", c->GetCarve() );
	tweakChar.AddData( "Visible", c->GetVisible() );
	tweakChar.Send( 2, true, c->GetSerial() );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void tweakItemMenu( cSocket *s, CItem *c )
//|   Date        :  Unknown
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handle new tweak item menu
//o---------------------------------------------------------------------------o
void tweakItemMenu( cSocket *s, CItem *i )
{
	if( i == NULL )
		return;
	GumpDisplay tweakItem( s, 350, 345 );
	tweakItem.SetTitle( "Tweak Item Menu" );

	UI16 itemID = i->GetID();
	UI16 itemColour = i->GetColour();

	tweakItem.AddData( "ID", itemID, 5 );
	if( i->GetName() != NULL && i->GetName()[0] > 0 )
		tweakItem.AddData( "Name", i->GetName() );
	else
		tweakItem.AddData( "Name", "#" );
	if( i->GetName2() != NULL && i->GetName2()[0] > 0 )
		tweakItem.AddData( "Name2", i->GetName2() );
	else
		tweakItem.AddData( "Name2", "#" );
	tweakItem.AddData( "Colour", itemColour, 5 );
	tweakItem.AddData( "Layer", i->GetLayer() );
	tweakItem.AddData( "Type", i->GetType() );
	tweakItem.AddData( "Moveable", i->GetMovable() );
	tweakItem.AddData( "X coord", i->GetX() );
	tweakItem.AddData( "Y coord", i->GetY() );
	tweakItem.AddData( "Z coord", i->GetZ() );
	tweakItem.AddData( "Amount", i->GetAmount() );
	tweakItem.AddData( "Strength", i->GetStrength() );
	tweakItem.AddData( "HP", i->GetHP() );
	tweakItem.AddData( "Max HP", i->GetMaxHP() );
	tweakItem.AddData( "LowDamage", i->GetLoDamage() );
	tweakItem.AddData( "HighDamage", i->GetHiDamage() );
	tweakItem.AddData( "Defense", i->GetDef() );
	tweakItem.AddData( "Speed", i->GetSpeed() );
	tweakItem.AddData( "Rank", i->GetRank() );
	tweakItem.AddData( "More", i->GetMore() );
	tweakItem.AddData( "More X", i->GetMoreX() );
	tweakItem.AddData( "More Y", i->GetMoreY() );
	tweakItem.AddData( "More Z", i->GetMoreZ() );
	tweakItem.AddData( "More B", i->GetMoreB() ); 
	tweakItem.AddData( "Poisoned", i->GetPoisoned() );
	tweakItem.AddData( "Weight", i->GetWeight() );
	tweakItem.AddData( "Decay", i->isDecayable()?1:0 );
	tweakItem.AddData( "Good", i->GetGood() );
	tweakItem.AddData( "Value", i->GetBuyValue() );
	tweakItem.AddData( "SellValue", i->GetSellValue() );
	tweakItem.AddData( "Carve", i->GetCarve() );
	tweakItem.AddData( "Stackable", i->isPileable()?1:0 );
	tweakItem.AddData( "Dyeable", i->isDyeable()?1:0 );
	tweakItem.AddData( "Corpse", i->isCorpse()?1:0 );
	tweakItem.AddData( "Visible", i->GetVisible() );
	tweakItem.AddData( "Creator", i->GetCreator() );
	tweakItem.Send( 1, true, i->GetSerial() );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void entrygump( cSocket *s, SERIAL ser, char type, char index, SI16 maxlength, char *text1 )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Open entry gump with specified text message and max value length
//o---------------------------------------------------------------------------o
void entrygump( cSocket *s, SERIAL ser, char type, char index, SI16 maxlength, const char *txt )
{
	if( s == NULL )
		return;
	if( txt == NULL )
	{
		Console.Error( 2, "Invalid text in entrygump()" );
		return;
	}

	char temp[32];
	sprintf( temp, "(%i chars max)", maxlength );
	CPGumpTextEntry toSend( txt, temp );
	toSend.Serial( ser );
	toSend.ParentID( type );
	toSend.ButtonID( index );
	toSend.Cancel( 1 );
	toSend.Style( 1 );
	toSend.Format( maxlength );
	s->Send( &toSend );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void entrygump( cSocket *s, SERIAL ser, char type, char index, SI16 maxlength, SI32 dictEntry )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Open entry gump with specified dictionary message and max value length
//o---------------------------------------------------------------------------o
void entrygump( cSocket *s, SERIAL ser, char type, char index, SI16 maxlength, SI32 dictEntry )
{
	const char *txt = Dictionary->GetEntry( dictEntry, s->Language() );
	entrygump( s, ser, type, index, maxlength, txt );
}

//o---------------------------------------------------------------------------o
//|		Function    :	void choice( cSocket *s )
//|		Date        :	Unknown
//|		Programmer  :	Unknown
//|		Modified	:	Zane - 2/10/2003	
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles choice "Help" menu, Tracking Menu, and Polymorph menu
//o---------------------------------------------------------------------------o
void choice( cSocket *s )
{
	char sect[512];
	DEFINITIONCATEGORIES script;
	
	UI16 main = s->GetWord( 5 );
	UI16 sub = s->GetWord( 7 );
	CChar *mChar = s->CurrcharObj();

	if( main < ITEMMENUOFFSET ) // GM Menus
	{
		sprintf(sect, "GMMENU %i", main);
		script = menus_def;
	}
	// PolyMorph spell menu (scriptable) by AntiChrist (9/99)
	else if( main >= POLYMORPHMENUOFFSET && main < POLYMORPHMENUOFFSET + 50 )
	{
		if( mChar->IsOnHorse() )
			DismountCreature( s->CurrcharObj() );		// can't be polymorphed on a horse
		Magic->Polymorph( s, POLYMORPHMENUOFFSET, sub );
		return;
	}
	else // Tracking
	{
		if( main >= TRACKINGMENUOFFSET + 1 && main <= TRACKINGMENUOFFSET + 3 )
		{
			if( !sub ) 
				return;
			if( !Skills->CheckSkill( mChar, TRACKING, 0, 1000 ) )
			{
				sysmessage( s, 575 );
				return;
			}
			Skills->TrackingMenu( s, static_cast<int>(sub - 1) );
		}
		sprintf( sect, "TRACKINGMENU %i", main );
		script = tracking_def;
	}
	
	ScriptSection *sectionData = FileLookup->FindEntry( sect, script );
	if( sectionData == NULL )
		return;

	char lscomm[512], lsnum[512];
	const char *data = NULL;
	const char *tag = sectionData->First();
	tag = sectionData->Next();
	for( UI16 i = 0; !sectionData->AtEnd(); tag = sectionData->Next() )
	{
		i++;
		tag = sectionData->Next();
		if( i == sub )
		{
			data = sectionData->GrabData();
			strcpy( lscomm, tag );
			strcpy( lsnum, data );
			scriptcommand( s, lscomm, lsnum );
			break;
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void gmmenu( cSocket *s, int m )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Opens the GM Call menu
//o---------------------------------------------------------------------------o
void gmmenu( cSocket *s, int m )
{
	int total, i;
	int lentext;
	char sect[512];
	char gmtext[11][257];
	char gmmiddle[5]="\x00\x00\x00\x00";
	int gmnumber=0;
	int gmindex;

	CChar *mChar = s->CurrcharObj();
	
	sprintf( sect, "GMMENU %i", m );
	ScriptSection *GMMenu = FileLookup->FindEntry( sect, menus_def );
	if( GMMenu == NULL )
		return;
	gmindex = m;
	char line[1024];
	const char *tag = GMMenu->First();
	const char *data = GMMenu->GrabData();
	sprintf( line, "%s %s", tag, data );
	lentext = sprintf(gmtext[0], "%s", line );
	for( tag = GMMenu->Next(); !GMMenu->AtEnd(); tag = GMMenu->Next() )
	{
		gmnumber++;
		data = GMMenu->GrabData();
		sprintf( line, "%s %s", tag, data );
		tag = GMMenu->Next();
		strcpy( gmtext[gmnumber], line );
	}
	total=9+1+lentext+1;
	for( i = 1; i <= gmnumber; i++ )
	{
		total += 4 + 1 + strlen( gmtext[i] );
	}
	CPOpenGump toSend = (*mChar);
	toSend.Length( total );
	toSend.GumpIndex( gmindex );
	s->Send( &toSend );
	s->Send( &lentext, 1 );
	s->Send( gmtext[0], lentext );
	lentext = gmnumber;
	s->Send( &lentext, 1 );
	for( i = 1; i <= gmnumber;i++ )
	{
		gmmiddle[0] = (UI08)((i-1)>>8);
		gmmiddle[1] = (UI08)((i-1)%256);
		s->Send( gmmiddle, 4 );
		lentext = strlen( gmtext[i] );
		s->Send( &lentext, 1 );
		s->Send( gmtext[i], lentext );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void itemmenu( cSocket *s, int m)
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Opens an itemmenu
//o---------------------------------------------------------------------------o
void itemmenu( cSocket *s, int m)
{
	int total, i;
	int lentext;
	char sect[512];
	char gmtext[30][257];
	char gmmiddle[5]="\x00\x00\x00\x00";
	int gmid[30];
	int gmnumber=0;
	int gmindex;
	
	gmindex = m;

	CChar *mChar = s->CurrcharObj();

	if( !mChar->IsGM() && m < 990 && m > 999 )
	{
		sysmessage( s, 337 );
		return;
	}

	sprintf( sect, "ITEMMENU %i", m );
	ScriptSection *ItemMenu = FileLookup->FindEntry( sect, items_def );
	if( ItemMenu == NULL )
		return;
	////////////////////////////////////////////////

	char tempLine[512], temp[1024];
	const char *tag = ItemMenu->First();
	const char *data = ItemMenu->GrabData();
	sprintf( tempLine, "%s %s", tag, data );
	strcpy( gmtext[0], tempLine );
	for( tag = ItemMenu->Next(); !ItemMenu->AtEnd(); tag = ItemMenu->Next() )
	{
		data = ItemMenu->GrabData();
		gmnumber++;
		gmid[gmnumber] = makeNum( tag );
		strcpy( gmtext[gmnumber], data );
		tag = ItemMenu->Next();
	}
	sprintf( temp, "%i: %s", m, gmtext[0] );
	lentext = sprintf( gmtext[0], "%s", temp );
	total = 9 + 1 + lentext + 1;
	for( i = 1; i <= gmnumber; i++ )
	{
		total += 4 + 1 + strlen( gmtext[i] );
	}
	CPOpenGump toSend = (*mChar);
	toSend.Length( total );
	toSend.GumpIndex( (gmindex + ITEMMENUOFFSET) );
	s->Send( &toSend );
	s->Send( &lentext, 1 );
	s->Send( gmtext[0], lentext );
	lentext = gmnumber;
	s->Send( &lentext, 1 );
	for( i = 1; i <= gmnumber; i++ )
	{
		gmmiddle[0] = (UI08)(gmid[i]>>8);
		gmmiddle[1] = (UI08)(gmid[i]%256);
		s->Send( gmmiddle, 4 );
		lentext = strlen( gmtext[i] );
		s->Send( &lentext, 1 );
		s->Send( gmtext[i], lentext );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cGump::Open( cSocket *s, CChar *i, UI08 num1, UI08 num2)
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Opens shopping gump
//o---------------------------------------------------------------------------o
void cGump::Open( cSocket *s, CChar *i, UI16 gumpNum )
{
	// evaluate calls to this func, see if num1 and num2 need be so large
	char shopgumpopen[8] = "\x24\x00\x00\x00\x01\x00\x30";
	shopgumpopen[1] = i->GetSerial( 1 );
	shopgumpopen[2] = i->GetSerial( 2 );
	shopgumpopen[3] = i->GetSerial( 3 );
	shopgumpopen[4] = i->GetSerial( 4 );
	shopgumpopen[5] = (UI08)(gumpNum>>8);
	shopgumpopen[6] = (UI08)(gumpNum%256);
	s->Send( shopgumpopen, 7 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void GumpDisplay::AddData( GumpInfo *toAdd )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Adds the data to the gump to send
//o---------------------------------------------------------------------------o
void GumpDisplay::AddData( GumpInfo *toAdd )
{
	GumpInfo *gAdd = new GumpInfo;
	gAdd->name = toAdd->name;
	gAdd->value = toAdd->value;
	gAdd->type = toAdd->type;
	gAdd->stringValue = toAdd->stringValue;
	gumpData.push_back( gAdd );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void GumpDisplay::AddData( const char *toAdd, long int value, UI08 type )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Adds the data to the gump to send
//o---------------------------------------------------------------------------o
void GumpDisplay::AddData( const char *toAdd, long int value, UI08 type )
{
	GumpInfo *gAdd = new GumpInfo;
	gAdd->name = toAdd;
	gAdd->stringValue = "";
	gAdd->value = value;
	gAdd->type = type;
	gumpData.push_back( gAdd );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void GumpDisplay::AddData( const char *toAdd, const char *toSet, UI08 type )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Adds the data to the gump to send
//o---------------------------------------------------------------------------o
void GumpDisplay::AddData( const char *toAdd, const char *toSet, UI08 type )
{
	if( toSet == NULL )
		return;

	if( toSet[0] == 0 )
		return;

	GumpInfo *gAdd = new GumpInfo;
	gAdd->name = toAdd;
	gAdd->stringValue = toSet;
	gAdd->type = type;
	gumpData.push_back( gAdd );
}

//o---------------------------------------------------------------------------o
//|   Function    :  GumpDisplay::GumpDisplay( cSocket *target ) : toSendTo( target )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Begin GumpDisplay stuff by setting the target, clearing any existing data, and setting the w / h
//o---------------------------------------------------------------------------o
GumpDisplay::GumpDisplay( cSocket *target ) : toSendTo( target )
{
	gumpData.resize( 0 );
	width = 340;
	height = 320;
}

//o---------------------------------------------------------------------------o
//|   Function    :  GumpDisplay::GumpDisplay( cSocket *target, UI16 gumpWidth, UI16 gumpHeight ) : 
//|						width( gumpWidth ), height( gumpHeight ), toSendTo( target )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Begin GumpDisplay stuff by setting the target, clearing any existing data, and setting the w / h
//o---------------------------------------------------------------------------o
GumpDisplay::GumpDisplay( cSocket *target, UI16 gumpWidth, UI16 gumpHeight ) : 
	width( gumpWidth ), height( gumpHeight ), toSendTo( target )
{
	gumpData.resize( 0 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  GumpDisplay::~GumpDisplay()
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Clear any existing GumpDisplay data
//o---------------------------------------------------------------------------o
GumpDisplay::~GumpDisplay()
{
	for( UI32 i = 0; i < gumpData.size(); i++ )
		delete gumpData[i];
	Delete();
	gumpData.resize( 0 );
}

//o---------------------------------------------------------------------------o
//|   Function    :	 void GumpDisplay::Delete( void )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Delete all GumpDisplay entries
//o---------------------------------------------------------------------------o
void GumpDisplay::Delete( void )
{
	one.resize( 0 );
	two.resize( 0 );
}

//o---------------------------------------------------------------------------o
//|   Function    :	 void GumpDisplay::SetTitle( const char *newTitle )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Set a gumps title
//o---------------------------------------------------------------------------o
void GumpDisplay::SetTitle( const char *newTitle )
{
	title = newTitle;
}

//o---------------------------------------------------------------------------o
//|   Function    :	 void SendVecsAsGump( cSocket *sock, stringList& one, stringList& two, long type, SERIAL serial )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Sends to socket sock the data in one and two.  One is control, two is data
//o---------------------------------------------------------------------------o
void SendVecsAsGump( cSocket *sock, STRINGLIST& one, STRINGLIST& two, long type, SERIAL serial )
{
	UI08 i = 0;
	char sect[512];

	UI32 length = 21;
	UI32 length2 = 1;

	UI32 linecount = one.size();
	UI32 linecount1 = two.size();
	UI32 line, textlines;

	char gump1[22]="\xB0\x04\x0A\x40\x91\x51\xE7\x00\x00\x00\x03\x00\x00\x00\x6E\x00\x00\x00\x46\x02\x3B";
	char gump2[4]="\x00\x00\x00";
	char gump3[3]="\x00\x00";

	for( line = 0; line < linecount; line++ )
	{
		if( one[line].length() == 0 )
			break;
		length += one[line].length() + 4;
		length2 += one[line].length() + 4;
	}
	
	length += 3;
	textlines = 0;
	line = 0;

	for( line = 0; line < linecount1; line++ )
	{
		if( two[line].length() == 0 )
			break;
		length += two[line].length() * 2 + 2;
		textlines++;
	}
	
	gump1[1] = (UI08)(length>>8);
	gump1[2] = (UI08)(length%256);
	if( serial != INVALIDSERIAL )
	{
		gump1[3] = (UI08)(serial>>24);
		gump1[4] = (UI08)(serial>>16);
		gump1[5] = (UI08)(serial>>8);
		gump1[6] = (UI08)(serial%256);
	}
	// gump1[3] -> gump1[6] are UID
	gump1[7] = (UI08)(type>>24);
	gump1[8] = (UI08)(type>>16);
	gump1[9] = (UI08)(type>>8);
	gump1[10] = (UI08)(type%256); // Gump Number
	gump1[19] = (UI08)(length2>>8);
	gump1[20] = (UI08)(length2%256);
	sock->Send( gump1, 21 );
	
	for( line = 0; line < linecount; line++ )
	{
		sprintf( sect, "{ %s }", one[line].c_str() );
		sock->Send( sect, strlen( sect ) );
	}
	
	gump2[1] = (UI08)(textlines>>8);
	gump2[2] = (UI08)(textlines%256);
	
	sock->Send( gump2, 3 );

	for( line = 0; line < linecount1; line++ )
	{
		if( two[line].length() == 0 )
			break;
		gump3[0] = (UI08)(( two[line].length() )>>8);
		gump3[1] = (UI08)(( two[line].length() )%256);
		sock->Send( gump3, 2 );
		gump3[0]=0;
		for( i = 0; i < two[line].length(); i++ )
		{
			gump3[1] = (two[line])[i];
			sock->Send( gump3, 2 );
		}
	}

}

//o---------------------------------------------------------------------------o
//|   Function    :	 void NewAddMenu( cSocket *s, int m )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Menu for item creation based on ITEMMENU entries in items.scp
//o---------------------------------------------------------------------------o
void NewAddMenu( cSocket *s, int m )
{
	STRINGLIST one, two;
	char tempString[256];
	UI32 pagenum = 1, position = 40, linenum = 1, buttonnum = 7;
	UI08 i = 0;

	char sect[512];
	CChar *mChar = s->CurrcharObj();

	if( !mChar->IsGM() && m < 990 && m > 999 )
	{
		sysmessage( s, 337 );
		return;
	}
	////////////////////////////////////////////////
	mChar->SetAddPart( m );	// let's store what menu we're at

	sprintf( sect, "ITEMMENU %i", m );
	ScriptSection *ItemMenu = FileLookup->FindEntry( sect, items_def );
	if( ItemMenu == NULL )
		return;

	// page header
	one.push_back( "nomove" );
	one.push_back( "noclose" );
	one.push_back( "page 0" );
	sprintf( tempString, "resizepic 0 0 %i 320 340", cwmWorldState->ServerData()->GetBackgroundPic() );
	one.push_back( tempString );
	sprintf( tempString, "button 280 10 %i %i 1 0 1", cwmWorldState->ServerData()->GetButtonCancel(), cwmWorldState->ServerData()->GetButtonCancel() + 1); //OKAY
	one.push_back( tempString );
	sprintf( tempString, "text 70 10 %i 0", cwmWorldState->ServerData()->GetTitleColour() );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	one.push_back( tempString );
	const char *tag = ItemMenu->First();
	const char *data = ItemMenu->GrabData();
	sprintf( tempString, "%s %s", tag, data );	// our title
	two.push_back( tempString );

	sprintf( tempString, "text 140 310 %i %i", cwmWorldState->ServerData()->GetTitleColour(), linenum++ );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	one.push_back( tempString );
	sprintf( tempString, "Menu %i", m );
	two.push_back( tempString );

	one.push_back( "page 1" );

	UI08 numCounter = 0;
	for( tag = ItemMenu->Next(); !ItemMenu->AtEnd(); tag = ItemMenu->Next() )
	{
		data = ItemMenu->GrabData();

		if( numCounter > 0 && (!(numCounter % 13 ) ) )
		{
			position = 40;
			pagenum++;
			sprintf( tempString, "page %i", pagenum );
			one.push_back( tempString );
		}

		sprintf( tempString, "text 50 %i %i %i", position, cwmWorldState->ServerData()->GetLeftTextColour(), linenum ); 
		one.push_back( tempString );
		sprintf( tempString, "button 20 %i %i %i 1 0 %i",position, cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1, buttonnum);
		one.push_back( tempString );
		two.push_back( data );
		if( tag[0] != '<' && tag[0] != ' ' )	// it actually has a picture, well bugger me! :>
		{
			if( numCounter % 2 == 0 )
			{
				sprintf( tempString, "tilepic 220 %i %i", position, makeNum( tag ) );
				one.push_back( tempString );
			}
			else
			{
				sprintf( tempString, "tilepic 270 %i %i", position, makeNum( tag ) );
				one.push_back( tempString );
			}
		}

		position+=20;
		linenum++;
		buttonnum++;
		numCounter++;

		tag = ItemMenu->Next();
	}
	pagenum = 1;
	for( i = 0; i < numCounter; i += 13 )
	{
		sprintf( tempString, "page %i", pagenum);
		one.push_back( tempString );
		if( i >= 10 )
		{
			sprintf( tempString, "button 50 300 %i %i  0 %i", cwmWorldState->ServerData()->GetButtonLeft(), cwmWorldState->ServerData()->GetButtonLeft() + 1, pagenum-1); //back button
			one.push_back( tempString );
		}
		if( ( numCounter > 13 ) && ( ( i + 13 ) < numCounter ) )
		{
			sprintf( tempString, "button 260 300 %i %i  0 %i", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1, pagenum + 1 );
			one.push_back( tempString );
		}
		pagenum++;
	}		

	SendVecsAsGump( s, one, two, 9, INVALIDSERIAL );	// send code here
}

//o---------------------------------------------------------------------------o
//|   Function    :	 void GumpDisplay::Send( long gumpNum, bool isMenu, SERIAL serial )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Sends gump data to the socket
//o---------------------------------------------------------------------------o
void GumpDisplay::Send( long gumpNum, bool isMenu, SERIAL serial )
{
	char temp[512];
	int i;
	UI32 pagenum = 1, position = 40, linenum = 1, buttonnum = 7;
	UI08 numToPage = 10, stringWidth = 10;

	if( one.size() > 0 && two.size() > 0 )
	{
		SendVecsAsGump( toSendTo, one, two, gumpNum, serial );
		return;
	}
	UI08 ser1, ser2, ser3, ser4;
	//--static pages
	one.push_back( "noclose" );
	one.push_back( "page 0"  );
	sprintf( temp, "resizepic 0 0 %i %i %i", cwmWorldState->ServerData()->GetBackgroundPic(), width, height );
	one.push_back( temp );
	sprintf( temp, "button %i 15 %i %i 1 0 1", width - 40, cwmWorldState->ServerData()->GetButtonCancel(), cwmWorldState->ServerData()->GetButtonCancel() + 1 );
	one.push_back( temp );
	sprintf( temp, "text 45 15 %i 0", cwmWorldState->ServerData()->GetTitleColour() );
	one.push_back( temp );

	sprintf( temp, "page %i", pagenum );
	one.push_back( temp );

	if( title.length() == 0 )
		strcpy( temp, "General Gump" );
	else
		strcpy( temp, title.c_str() );
	two.push_back( temp );

	numToPage = (UI08)((( height - 30 ) / 20) - 2);
	stringWidth = (UI08)( ( width / 2 ) / 10 );
	UI32 lineForButton;
	for( i = 0, lineForButton = 0; static_cast<unsigned int>(i) < gumpData.size(); i++, lineForButton++ )
	{
		if( lineForButton > 0 && ( !( lineForButton % numToPage ) ) )
		{
			position = 40;
			pagenum++;
			sprintf( temp, "page %i", pagenum );
			one.push_back( temp );
		}
		if( gumpData[i]->type != 7 )
		{
			sprintf( temp, "text 50 %i %i %i",position, cwmWorldState->ServerData()->GetLeftTextColour(), linenum++ ); 
			one.push_back( temp );
			if( isMenu )
			{
				sprintf( temp, "button 20 %i %i %i 1 0 %i",position, cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1, buttonnum);
				one.push_back( temp );
			}
			sprintf( temp, "text %i %i %i %i", (width / 2) + 10, position, cwmWorldState->ServerData()->GetRightTextColour(), linenum++);
			one.push_back( temp );
			two.push_back( gumpData[i]->name );
		}
		else
		{
			sprintf( temp, "text 30 %i %i %i",position, cwmWorldState->ServerData()->GetLeftTextColour(), linenum++ ); 
			one.push_back( temp );
		}

		const char *sVal = gumpData[i]->stringValue.c_str();
		long value = gumpData[i]->value;
		switch( gumpData[i]->type )
		{
		case 0:		// all numbers of sorts
			sprintf( temp, "%li", value );
			break;
		case 1:
			sprintf( temp, "%lx", value );
			break;
		case 2:
			ser1 = (UI08)(value>>24);
			ser2 = (UI08)(value>>16);
			ser3 = (UI08)(value>>8);
			ser4 = (UI08)(value%256);
			sprintf( temp, "%i %i %i %i", ser1, ser2, ser3, ser4 );
			break;
		case 3:
			ser1 = (UI08)(value>>24);
			ser2 = (UI08)(value>>16);
			ser3 = (UI08)(value>>8);
			ser4 = (UI08)(value%256);
			sprintf( temp, "%x %x %x %x", ser1, ser2, ser3, ser4 );
			break;
		case 4:
			if( sVal == NULL )
				strcpy( temp, "NULL POINTER" );
			else
				strcpy( temp, sVal );
			if( strlen( temp ) > stringWidth )	// too wide for one line, CRAP!
			{
				char temp2[512], temp3[512];
				int tempWidth;
				tempWidth = strlen( temp ) - stringWidth;
				strncpy( temp2, temp, stringWidth );
				temp2[stringWidth] = 0;
				two.push_back( temp2 );
				for( int tempCounter = 0; tempCounter < tempWidth / ( stringWidth * 2 ) + 1; tempCounter++ )
				{
					position += 20;
					lineForButton++;
					sprintf( temp3, "text %i %i %i %i", 30, position, cwmWorldState->ServerData()->GetRightTextColour(), linenum++ );
					one.push_back( temp3 );
					strncpy( temp2, &temp[stringWidth + tempCounter * stringWidth * 2], stringWidth * 2 );
					temp2[stringWidth * 2] = 0;
					two.push_back( temp2 );
				}
				// be stupid for the moment and do no text wrapping over pages
			}
			else
				two.push_back( temp );
			break;
		case 5:
			ser1 = (UI08)(value>>8);
			ser2 = (UI08)(value%256);
			sprintf( temp, "%x %x", ser1, ser2 );
			break;
		case 6:
			ser1 = (UI08)(value>>8);
			ser2 = (UI08)(value%256);
			sprintf( temp, "%i %i", ser1, ser2 );
			break;
		case 7:
			if( sVal == NULL )
				strcpy( temp, "NULL POINTER" );
			else
				strcpy( temp, sVal );
			int sWidth;
			sWidth = stringWidth * 2;
			if( strlen( temp ) > static_cast<size_t>(sWidth) )	// too wide for one line, CRAP!
			{
				char temp2[512], temp3[512];
				int tempWidth;
				tempWidth = strlen( temp ) - sWidth;
				strncpy( temp2, temp, sWidth );
				temp2[sWidth] = 0;
				two.push_back( temp2 );
				for( int tempCounter = 0; tempCounter < tempWidth / sWidth + 1; tempCounter++ )
				{
					position += 20;
					lineForButton++;
					sprintf( temp3, "text %i %i %i %i", 30, position, cwmWorldState->ServerData()->GetLeftTextColour(), linenum++ );
					one.push_back( temp3 );
					strncpy( temp2, &temp[(tempCounter + 1) * sWidth], sWidth );
					temp2[sWidth] = 0;
					two.push_back( temp2 );
				}
				// be stupid for the moment and do no text wrapping over pages
			}
			else
				two.push_back( temp );
			break;
		default:
			sprintf( temp, "%li", value );
			break;
		}
		if( gumpData[i]->type != 4 && gumpData[i]->type != 7 )
			two.push_back( temp );
		position += 20;
		buttonnum++;
	}
	
	pagenum = 1; 
	for( i = 0; static_cast<UI32>(i) <= lineForButton; i += numToPage )
	{
		sprintf( temp, "page %i", pagenum );
		one.push_back( temp );
		if( i >= 10 )
		{
			sprintf( temp, "button 10 %i %i %i 0 %i", height - 40, cwmWorldState->ServerData()->GetButtonLeft(), cwmWorldState->ServerData()->GetButtonLeft() + 1, pagenum-1); //back button
			one.push_back( temp );
		}
		if( lineForButton > numToPage && static_cast<UI32>(( i + numToPage )) < lineForButton )
		{
			sprintf( temp, "button %i %i %i %i 0 %i", width - 40, height - 40, cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1, pagenum+1); //forward button
			one.push_back( temp );
		}
		pagenum++;
	}		
	SendVecsAsGump( toSendTo, one, two, gumpNum, serial );
}

//o---------------------------------------------------------------------------o
//|   Function    :	 string GetStrFromSock( cSocket *sock, UI16 offset, UI16 length )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Get string data from the socket
//o---------------------------------------------------------------------------o
std::string GetStrFromSock( cSocket *sock, UI16 offset, UI16 length )
{
	char temp[512];
	strncpy( temp, (const char *)&(sock->Buffer()[offset]), length );
	temp[length] = 0;
	return temp;
}

//o---------------------------------------------------------------------------o
//|   Function    :	 string GetUniStrFromSock( cSocket *sock, UI16 offset, UI16 length )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Get string data from the socket
//o---------------------------------------------------------------------------o
std::string GetUniStrFromSock( cSocket *sock, UI16 offset, UI16 length )
{
	char temp[512];
	for( int counter = 0; counter < length; counter++ )
		temp[counter] = sock->GetByte( offset + counter * 2 + 1 );
	temp[length] = 0;
	return temp;
}
