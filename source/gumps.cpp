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
#include "CJSMapping.h"
#include "cScript.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "regions.h"
#include "classes.h"
#include "commands.h"
#include "Dictionary.h"
#include "PageVector.h"
#include "ObjectFactory.h"

#undef DBGFILE
#define DBGFILE "gumps.cpp"

namespace UOX
{

void CollectGarbage( void );
void deedHouse( CSocket *s, CMultiObj *i );
UString GetUptime( void );

//o---------------------------------------------------------------------------o
//|   Function    :  void TextEntryGump( CSocket *s, SERIAL ser, char type, char index, SI16 maxlength, SI32 dictEntry )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Open entry gump with specified dictionary message and max value length
//o---------------------------------------------------------------------------o
void TextEntryGump( CSocket *s, SERIAL ser, UI08 type, UI08 index, SI16 maxlength, SI32 dictEntry )
{
	if( s == NULL )
		return;
	std::string txt = Dictionary->GetEntry( dictEntry, s->Language() );
	if( txt.empty() )
	{
		Console.Error( "Invalid text in TextEntryGump()" );
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
//|   Function    :  void BuildGumpFromScripts( CSocket *s, UI16 m )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Pull gump info from misc.scp
//o---------------------------------------------------------------------------o
void BuildGumpFromScripts( CSocket *s, UI16 m )
{
	CPSendGumpMenu toSend;
	toSend.UserID( INVALIDSERIAL );

	UString sect = "GUMPMENU " + UString::number( m );
	ScriptSection *gump = FileLookup->FindEntry( sect, misc_def );
	if( gump == NULL )
		return;
	char temp[512];
	UI08 targType	= 0x12;
	UString tag		= gump->First();
	if( tag.section( " ", 0, 0 ).upper() == "TYPE" )
	{
		targType = tag.section( " ", 1, 1 ).toUByte();
		tag = gump->Next();
	}
	for( ; !gump->AtEnd(); tag = gump->Next() )
	{
		sprintf( temp, "%s %s", tag.c_str(), gump->GrabData().c_str() );
		toSend.AddCommand( temp );
	}
	sect = "GUMPTEXT " + UString::number( m );
	gump = FileLookup->FindEntry( sect, misc_def );
	if( gump == NULL )
		return;
	for( tag = gump->First(); !gump->AtEnd(); tag = gump->Next() )
	{
		toSend.AddText( "%s %s", tag.c_str(), gump->GrabData().c_str() );
	}
	toSend.GumpID( targType );
	toSend.Finalize();
	s->Send( &toSend );
}

//o--------------------------------------------------------------------------o
//|	Function		-	void HandleAccountButton( CSocket *s, long button, CChar *j )
//|	Date			-	
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Handles the accounts gump button
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void HandleAccountButton( CSocket *s, long button, CChar *j )
{
	if( !ValidateObject( j ) )
		return;
	CChar *mChar = s->CurrcharObj();
	CAccountBlock& actbTemp = Accounts->GetAccountByID( j->GetAccount().wAccountIndex );
	if( actbTemp.wAccountIndex == AB_INVALID_ID )
		return;
	//
	CSocket *targSocket = j->GetSocket();
	switch( button )
	{
		case 2:
			if( mChar->IsGM() && !j->IsNpc() )
			{
				s->sysmessage( 487 );
				if( targSocket != NULL ) 
					targSocket->sysmessage( 488 );
				actbTemp.wFlags.set( AB_FLAGS_BANNED, true );
				if( targSocket != NULL ) 
					Network->Disconnect( targSocket );
			}
			else 
				s->sysmessage( 489 );
			break;
		case 3:
			if( mChar->IsGM() && !j->IsNpc() )
			{
				s->sysmessage( 490 );
				if( targSocket != NULL ) 
					targSocket->sysmessage( 491 );
				actbTemp.wFlags.set( AB_FLAGS_BANNED, true );
				actbTemp.wTimeBan=static_cast<UI16>(BuildTimeValue( 60 * 60 * 24 ));

				if( targSocket != NULL ) 
					Network->Disconnect( targSocket );
			}
			else 
				s->sysmessage( 492 );
			break;
		case 4:		
		case 5:		
		case 6:		
		case 7:
			s->sysmessage( 489 + button );
			break;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleTweakItemButton( CSocket *s, long button, SERIAL ser, long type )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button pressed in tweak item gump
//o---------------------------------------------------------------------------o
void HandleTweakItemButton( CSocket *s, SERIAL button, SERIAL ser, SERIAL type )
{
	button -= 6;
	if( button == 0 )
	{
		s->sysmessage( 1700 );
		return;
	}
	switch( button )
	{
		case 15:	// Low Damage
		case 16:	// High Damage
		case 17:	// Defense
		case 18:	// Speed
		case 19:	// Rank
			TextEntryGump( s, ser, static_cast<UI08>(type), static_cast<UI08>(button), 4, 495 + button );	// allow 0x for hex value
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
		case 27:	// Good
		case 28:	// Buy Value
		case 29:	// Sell Value
		case 30:	// Carve
		case 36:	// ScriptTrigger ID
		case 37:	// AmmoID
		case 38:	// AmmoHue
		case 39:	// AmmoFX
		case 40:	// AmmoFXHue
		case 41:	// AmmoFXRender (mode)
			TextEntryGump( s, ser, static_cast<UI08>(type), static_cast<UI08>(button), 6, 495 + button );	// allow 0x for hex value
			break;
		case 7:		// Moveable
		case 24:	// POISON
		case 26:	// Decay
		case 31:	// Stackable
		case 32:	// Dyable
		case 33:	// Corpse
		case 34:	// Visible
			TextEntryGump( s, ser, static_cast<UI08>(type), static_cast<UI08>(button), 1, 495 + button );
			break;
		case 11:	// Amount
		case 12:	// Strength
		case 25:	// Weight
		case 42:	// WeightMax
		case 43:	// BaseWeight
			TextEntryGump( s, ser, static_cast<UI08>(type), static_cast<UI08>(button), 7, 495 + button );	// allow 0x for hex value
			break;
		case 2:		// Name
		case 3:		// Name 2
		case 35:	// Creator
		case 44:	// Spawnobj/Spawnobjlist
			TextEntryGump( s, ser, static_cast<UI08>(type), static_cast<UI08>(button), 50, 495 + button );
			break;
		default:	Console << Dictionary->GetEntry( 533 ) << (SI32)button << myendl;	break;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleTweakCharButton( CSocket *s, long button, SERIAL ser, long type )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button pressed in tweak char gump
//o---------------------------------------------------------------------------o
void HandleTweakCharButton( CSocket *s, SERIAL button, SERIAL ser, SERIAL type )
{
	button -= 6;
	if( button == 0 )
	{
		s->sysmessage( 1700 );
		return;
	}

	switch( button )
	{
		case 1:		// Name
		case 2:		// Title
			TextEntryGump( s, ser, static_cast<UI08>(type), static_cast<UI08>(button), 50, 1700 + button );
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
		case 29:	// ScriptTrigger ID
			TextEntryGump( s, ser, static_cast<UI08>(type), static_cast<UI08>(button), 6, 1700 + button );	// allow 0x for hex
			break;
		case 10:	// Strength
		case 11:	// Dexterity
		case 12:	// Intelligence
		case 19:	// Fame
		case 20:	// Karma
		case 26:	// Weight
			TextEntryGump( s, ser, static_cast<UI08>(type), static_cast<UI08>(button), 7, 1700 + button );	// allow 0x for hex
			break;

		case 16:	// Low Damage
		case 17:	// High Damage
		case 18:	// Defense
		case 22:	// AI Type
		case 24:	// Hunger
			TextEntryGump( s, ser, static_cast<UI08>(type), static_cast<UI08>(button), 4, 1700 + button );	// allow 0x for hex
			break;
		case 23:	// NPC Wander
		case 25:	// Poison
		case 28:	// Visible
			TextEntryGump( s, ser, static_cast<UI08>(type), static_cast<UI08>(button), 1, 1700 + button );
			break;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleTownstoneButton( CSocket *s, long button, SERIAL ser, long type )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button pressed in townstone gump
//o---------------------------------------------------------------------------o
void HandleTownstoneButton( CSocket *s, SERIAL button, SERIAL ser, SERIAL type )
{
	CChar *mChar = s->CurrcharObj();
	CTownRegion *targetRegion;
	CTownRegion *ourRegion = cwmWorldState->townRegions[mChar->GetTown()];
	switch( button )
	{
		// buttons 2-20 are for generic town members
		// buttons 21-40 are for mayoral functions
		// buttons 41-60 are for potential candidates
		// buttons 61-80 are for enemy towns
		case 2:		// leave town
			bool result;
			// safe to assume we want to remove ourselves from our only town!
			result = ourRegion->RemoveTownMember( (*mChar) );
			if( !result )
				s->sysmessage( 540 );
			break;
		case 3:		// view taxes
			targetRegion = calcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber() );
			if( targetRegion != NULL )
				targetRegion->ViewTaxes( s );
			else
				s->sysmessage( 541 );
			break;
		case 4:		// toggle town title
			mChar->SetTownTitle( !mChar->GetTownTitle() );
			ourRegion->DisplayTownMenu( NULL, s );
			break;
		case 5:		s->target( 0, TARGET_VOTEFORMAYOR, 542 );								break;		// vote for town mayor
		case 6:		TextEntryGump(  s, ser, static_cast<UI08>(type), static_cast<UI08>(button), 6, 543 );	break;		// gold donation
		case 7:		ourRegion->ViewBudget( s );			break;		// View Budget
		case 8:		ourRegion->SendAlliedTowns( s );		break;		// View allied towns
		case 9:		ourRegion->SendEnemyTowns( s );		break;
		case 20:	// access mayoral functions, never hit here if we don't have mayoral access anyway!
					// also, we'll only get access, if we're in our OWN region
			ourRegion->DisplayTownMenu( NULL, s, 0x01 );	// mayor
			break;
		case 21:	s->sysmessage( 544 );				break;	// set taxes
		case 22:	ourRegion->DisplayTownMembers( s );	break;	// list town members
		case 23:	ourRegion->ForceEarlyElection();		break;	// force early election
		case 24:	s->sysmessage( 545 );	break;	// purchase more guards
		case 25:	s->sysmessage( 546 );	break;	// fire a guard
		case 26:	s->target( 0, TARGET_TOWNALLY, 547 );								break;	// make a town an ally
		case 40:	ourRegion->DisplayTownMenu( NULL, s );	break;	// we can't return from mayor menu if we weren't mayor!
		case 41:	// join town!
			if( !(calcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber() )->AddAsTownMember( (*mChar) ) ) )
				s->sysmessage( 548 );
			break;
		case 61:	// seize townstone!
			if( !Skills->CheckSkill( mChar, STEALING, 950, 1000 )	) // minimum 95.0 stealing
			{
				targetRegion = calcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber() );
				if( targetRegion != NULL )
				{
					int chanceToSteal	= RandomNum( 0, targetRegion->GetHealth() / 2 );
					int fudgedStealing	= RandomNum( mChar->GetSkill( STEALING ), static_cast< UI16 >(mChar->GetSkill( STEALING ) * 2) );
					if( fudgedStealing >= chanceToSteal )
					{
						// redo stealing code here
						s->sysmessage( 549, targetRegion->GetName().c_str() );
						targetRegion->DoDamage( targetRegion->GetHealth() / 2 );	// we reduce the region's health by half
						REGIONLIST nearbyRegions = MapRegion->PopulateList( mChar );
						for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
						{
							CMapRegion *toCheck = (*rIter);
							if( toCheck == NULL )	// no valid region
								continue;
							CDataList< CItem * > *regItems = toCheck->GetItemList();
							regItems->Push();
							for( CItem *itemCheck = regItems->First(); !regItems->Finished(); itemCheck = regItems->Next() )
							{
								if( !ValidateObject( itemCheck ) )
									continue;
								if( itemCheck->GetType() == IT_TOWNSTONE && itemCheck->GetID() != 0x14F0 )
								{	// found our townstone
									CItem *charPack = mChar->GetPackItem();
									if( ValidateObject( charPack ) )
									{
										itemCheck->SetCont( charPack );
										itemCheck->SetTempVar( CITV_MOREX, targetRegion->GetRegionNum() );
										s->sysmessage( 550 );
										targetRegion->TellMembers( 551, mChar->GetName().c_str() );
										regItems->Pop();
										return;	// dump out
									}
								}
							}
							regItems->Pop();
						}
					}
					else
						s->sysmessage( 552 );
				}
				else
					s->sysmessage( 553 );
			}
			else
				s->sysmessage( 554 );
			break;
		case 62:	// attack townstone
			targetRegion = calcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber() );
			for( UI08 counter = 0; counter < RandomNum( 5, 10 ); ++counter )
			{
				Effects->PlayMovingAnimation( mChar, mChar->GetX() + RandomNum( -6, 6 ), mChar->GetY() + RandomNum( -6, 6 ), mChar->GetZ(), 0x36E4, 17, 0, ( RandomNum( 0, 1 ) == 1 ) );
				Effects->PlayStaticAnimation( mChar->GetX() + RandomNum( -6, 6 ), mChar->GetY() + RandomNum( -6, 6 ), mChar->GetZ(), 0x373A + RandomNum( 0, 4 ) * 0x10, 0x09, 0, 0 );
			}
			targetRegion->DoDamage( RandomNum( 0, targetRegion->GetHealth() / 8) );	// we reduce the region's health by half
			break;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleHairDyeButton( CSocket *s, CItem *j )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button pressed in hair dye gump
//o---------------------------------------------------------------------------o
void HandleHairDyeButton( CSocket *s, CItem *j )
{
	if( !ValidateObject( j ) )
		return;

	// we only HAVE one button, so we'll assume safely that the colour has changed if we hit here
	CChar *dest	= s->CurrcharObj();
	if( !ValidateObject( dest ) )
		return;

	UI16 dyeColour		= s->GetWord( 21 );
	CItem *beardobject	= dest->GetItemAtLayer( IL_FACIALHAIR );
	CItem *hairobject	= dest->GetItemAtLayer( IL_HAIR );

	if( ValidateObject( hairobject ) )
		hairobject->SetColour( dyeColour );
	if( ValidateObject( beardobject ) )
		beardobject->SetColour( dyeColour );
	j->Delete();
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleAccountModButton( CSocket *s, long button )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button pressed in account gump
//o---------------------------------------------------------------------------o
void HandleAccountModButton( CPIGumpMenuSelect *packet )
{
	CSocket *s = packet->GetSocket();

	std::string username	= "";
	std::string password	= "";
	std::string emailAddy	= "";

	for( UI32 i = 0; i < packet->TextCount(); ++i )
	{
		UI16 textID = packet->GetTextID( i );
		switch( textID )
		{
			case 1000:	username	= packet->GetTextUString( i );	break;
			case 1001:	password	= packet->GetTextUString( i );	break;
			case 1002:	emailAddy	= packet->GetTextUString( i );	break;
			default:
				Console.Warning( "Unknown textID %i with string %s", textID, packet->GetTextUString( i ).c_str() );
		}
	}

	CAccountBlock& actbAccountFind = Accounts->GetAccountByName( username );
	if( actbAccountFind.wAccountIndex != AB_INVALID_ID )
	{
		s->sysmessage( 555 );
		return;
	}
	Console.Print( "Attempting to add username %s with password %s at emailaddy %s", username.c_str(), password.c_str(), emailAddy.c_str() );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleHouseButton( CSocket *s, long button, CItem *j )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button pressed in house gump
//o---------------------------------------------------------------------------o
void HandleHouseButton( CSocket *s, long button, CItem *j )
{
	if( !ValidateObject( j ) )
		return;

	CMultiObj *house = calcMultiFromSer( j->GetTempVar( CITV_MORE ) );

	if( button == 2 )
		s->TempObj( j );
	else if( button != 20 )
		s->TempObj( house );
	switch( button )
	{
		case 20: if( s->GetWord( 21 ) > 0 )								// Change house sign's appearance
				 {
					j->SetID( s->GetWord( 21 ) );
					s->sysmessage( 556 );
				 }												break;
		case 0:													break;
		case 1:													break;
		case 2:	s->target( 0, TARGET_HOUSEOWNER, 557 );			break;  // Bestow ownership upon someone else
		case 3:	deedHouse( s, house );							break;  // Turn house into a deed
		case 4:	s->target( 0, TARGET_HOUSEEJECT, 558 );			break;  // Kick someone out of house
		case 5:	s->target( 0, TARGET_HOUSEBAN, 559 );			break;  // Ban somebody
		case 6: s->TempInt( 1 );
				s->target( 0, TARGET_HOUSEUNLIST, 560 );		break; // Remove someone from ban list
		case 7:	s->target( 0, TARGET_HOUSEFRIEND, 561 );		break; // Make someone a friend
		case 8:	s->TempInt( 0 );
				s->target( 0, TARGET_HOUSEUNLIST, 560 );		break; // Remove someone from house list
		default:
				char temp[1024];
				sprintf( temp, "HouseGump Called - Button=%li", button );
				s->sysmessage( temp );
																break;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :	 void BuildAddMenuGump( CSocket *s, UI16 m )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Menu for item creation based on ITEMMENU entries in items.scp
//|									
//|	Modification	-	04042004 - EviLDeD - Added support for the new Auto-AddMenu
//|									items if they exist. Remember the multimap is groupID based
//|									so it should order it accordingly we only have to look for
//|									the entries.
//o---------------------------------------------------------------------------o
void BuildAddMenuGump( CSocket *s, UI16 m )
{
	UI32 pagenum	= 1, position = 40, linenum = 1, buttonnum = 7;
	UI08 i			= 0;
	CChar *mChar	= s->CurrcharObj();

	if( !mChar->IsGM() && m > 990 && m < 999 ) // 990 - 999 reserved for online help system
	{
		s->sysmessage( 337 );
		return;
	}

#if defined( UOX_DEBUG_MODE )
	Console << "Menu: " << m << myendl;
#endif

	CPSendGumpMenu toSend;
	toSend.UserID( INVALIDSERIAL );
	toSend.GumpID( 9 );

	////////////////////////////////////////////////
	s->TempInt( m );	// Store what menu they are in

	UString sect = "ITEMMENU " + UString::number( m );
	ScriptSection *ItemMenu = FileLookup->FindEntry( sect, items_def );
	if( ItemMenu == NULL )
		return;

	// page header
	toSend.AddCommand( "noclose" );
	toSend.AddCommand( "nodispose" );
	toSend.AddCommand( "page 0" );

	// Need more control over this aspect of uox3 from outside some how.. first step to get the variables more flexible. later we will emporer them to a template, or a script or something
	UI16 xStart = 0, xWidth = 500;
	UI16 yStart = 0, yWidth = 375;

	UI32 bgImage	=	cwmWorldState->ServerData()->BackgroundPic();

	// Set and resize the gumps background image.
	toSend.AddCommand( "resizepic %u %u %u %u %u",xStart,yStart, bgImage, xWidth, yWidth );
	toSend.AddCommand( "checkertrans %u %u %u %u",xStart+5,yStart+5,xWidth-10,yWidth-11);
	
	// Next we create and position the close window button as well set its Down, and Up states.
	toSend.AddCommand( "button %u %u %u %u %u %u %u",xWidth-28,yStart+1,0xA51, 0xA50, 1, 0, 1); 
	// Grab the first tag/value pair from the gump itemmenu respectivly
	UString tag		= ItemMenu->First();
	UString data	= ItemMenu->GrabData();
	toSend.AddCommand( "resizepic %u %u %u %u %u",2,4,0xDAC,470,40);
	// Create the text stuff for what appears to be the title of the gump. This appears to change from page to page.
	UString szBuffer;
	linenum = 0;
	toSend.AddCommand( "text %u %u %u %u",30,yStart+13, 39, linenum++);
	toSend.AddText( "Players: " ); 
	// Player count
	szBuffer = UString::sprintf( "%4i ", cwmWorldState->GetPlayersOnline() );
	toSend.AddCommand( "text %u %u %u %u", 80, yStart+13, 25, linenum++ );
	toSend.AddText( szBuffer ); 
	// Gm Pages 
	toSend.AddCommand( "text %u %u %u %u",118,yStart+13, 39, linenum++);
	toSend.AddText( "GM Pages: " ); 
	szBuffer = "0";
	toSend.AddCommand( "text %u %u %u %u",185,yStart+13, 25, linenum++);
	toSend.AddText( szBuffer ); 
	// Current Time/Date
	toSend.AddCommand( "text %u %u %u %u",230,yStart+13, 39, linenum++);
	toSend.AddText( "Time: " ); 
	// Current server time
	struct tm *today;
	time_t ltime=time(NULL);
	today = localtime(&ltime);
	char tmpBuffer[200];
	bool isAM = true;
	strftime(tmpBuffer,128,"%b %d, %Y",today);
	if(today->tm_hour>12)
	{
		isAM = false;
		today->tm_hour-=12;
	} 
	else if(today->tm_hour==0)
	{
		isAM = true;
		today->tm_hour=12;
	}
	// Draw the current date to the gump
	szBuffer = UString::sprintf( "%s %.8s%s", tmpBuffer, asctime( today ) + 11, ( (isAM)?"Am":"Pm") );
	toSend.AddCommand( "text %u %u %u %u",280,yStart+13, 25, linenum++);
	toSend.AddText( szBuffer ); 
		
	// add the next gump portion. New server level services, in the form of a gump Configuration, and Accounts tabs to start. These are default tabs
	toSend.AddCommand( "resizepic %u %u %u %u %u",xStart+10,yStart+62, 0x13EC, 190, 300);
	UI32 tabNumber = 1;
	if( m == 1 )
	{
		// Do the shard tab
		toSend.AddCommand( "button %u %u %u %u %u %u %u",17,yStart+47,0x138E, 0x138F, 0, 2, 0); 
		szBuffer = "Shard";
		toSend.AddCommand( "text %u %u %u %u",42,yStart+46, 47, linenum++);
		toSend.AddText( szBuffer );
		// Do the server tab
		toSend.AddCommand( "button %u %u %u %u %u %u %u",105,yStart+47,0x138E, 0x138F, 0, 30, 1); 
		toSend.AddCommand( "text %u %u %u %u",132,yStart+46, 47, linenum++);
		toSend.AddText( "Server" );
	}
	else
	{
		// Do the shard tab
		toSend.AddCommand( "button %u %u %u %u %u %u %u",17,yStart+47,0x138E, 0x138F, 0, 1, 0); 
		szBuffer = "Shard";
		toSend.AddCommand( "text %u %u %u %u",42,yStart+46, 47, linenum++);
		toSend.AddText( szBuffer );
		// Do the server tab
		toSend.AddCommand( "button %u %u %u %u %u %u %u",105,yStart+47,0x138E, 0x138F, 0, 30, 1); 
		toSend.AddCommand( "text %u %u %u %u",132,yStart+46, 47, linenum++);
		toSend.AddText( "Server" );
	}

	// Need a seperator
	// 0x2393 does NOT exist on early clients!  Someone's been naughty!
	if( s->ClientVersionMajor() >= 4 )
		toSend.AddCommand( "gumppictiled %u %u %u %u %u",xStart+22,yWidth-50,165,5,0x2393);
	else
		toSend.AddCommand( "gumppictiled %u %u %u %u %u", xStart + 22, yWidth - 50, 165, 5, 0x145E );
	// Ok, now the job of pulling the rest of the first itemmenu information and making tabs for them
	toSend.AddCommand( "text %u %u %u %u",20,yWidth-40, 94, linenum++ );
	toSend.AddText( "(c)1997-2012 UOX3 DevTeam" );

	// Ok here we have some conditions that we need to filter. First being the menu called.
	UI32 xOffset;
	UI32 yOffset;
#define SXOFFSET	210
#define SYOFFSET	44
#define YOFFSET	103
#define XOFFSET	68
	if( m == 1 )
	{
		// Now we make the fisrt page that will escentially display our list of Main GM Groups.
		toSend.AddCommand( "page 1" );
		// Thanks for choosing UOX3 text
		toSend.AddCommand( "text %u %u %u %u", 15, yStart+65, 52, linenum++);
		toSend.AddText( "Thank you for choosing UOX3!" );
		toSend.AddCommand( "htmlgump %u %u %u %u %u %u %u", 58, yStart+83, 200, 20, linenum++,0,0);
		toSend.AddText( "<a href=\"http://www.uox3.org/\">www.uox3.org</a>" );
		// Need a seperator
		if( s->ClientVersionMajor() >= 4 )
			toSend.AddCommand( "gumppictiled %u %u %u %u %u", xStart + 21, yStart + 105, 165, 5, 0x2393 );
		else
			toSend.AddCommand( "gumppictiled %u %u %u %u %u", xStart + 21, yStart + 105, 165, 5, 0x145E );
		toSend.AddCommand( "htmlgump %u %u %u %u %u %u %u", xStart+16,yStart+120,175,140,linenum++,1,1);
		toSend.AddText( "<b>Server:</b>\n\nThe 'Server' tab is where a shard op will find most of the server related configuration tools.\n\n<b>Shard:</b>\n\nThe 'Shard'  tab is where a ShardOP will find the tools, items, and objects related to operating a shard, and filling it content.\n" );
		// Show the version and total memory used by UOX3
		toSend.AddCommand( "text %u %u %u %u",40,305,121,linenum++);
		toSend.AddText( "Version: " );
		toSend.AddCommand( "text %u %u %u %u",100,305,120,linenum++);
		szBuffer = " v" + CVersionClass::GetVersion() + "." + CVersionClass::GetBuild();
		toSend.AddText( szBuffer );
		// Do the UOX logo (okok so what! O_o) 
		toSend.AddCommand( "gumppic %u %u %u",248,98,0x1392);
		if( s->ClientVersionMajor() >= 4 )
			toSend.AddCommand( "gumppic %u %u %u",315,150,0x2328);
		else
			toSend.AddCommand( "gumppic %u %u %u", 315, 150, 0x1580 );
		toSend.AddCommand( "text %u %u %u %u",370,180,37,linenum++);
		toSend.AddText( "X" );
		toSend.AddCommand( "text %u %u %u %u",370,190,67,linenum++);
		toSend.AddText( "3" );

		// Now we make the fisrt page that will escentially display our list of Main GM Groups.
		pagenum = 2;
		toSend.AddCommand( "page 2" );
		// Do the shard tab
		toSend.AddCommand( "gumppic %u %u %u ",17,yStart+47,0x138F); 
		szBuffer = "Shard";
		toSend.AddCommand( "text %u %u %u %u",42,yStart+46, 70, linenum++ );
		toSend.AddText( szBuffer );
		
		// here we hunt tags to make sure that we get them all from the itemmenus etc.
		UI08 numCounter = 0;
		position		= 80;
		xOffset			= SXOFFSET;
		yOffset			= SYOFFSET;
		for( tag = ItemMenu->Next(); !ItemMenu->AtEnd(); tag = ItemMenu->Next() )
		{
			data = ItemMenu->GrabData();
			if( numCounter > 0 && (!(numCounter % 12 ) ) )
			{
				position	= 80;
				toSend.AddCommand( "page %i", ++pagenum );
				xOffset		= SXOFFSET;
				yOffset		= SYOFFSET;
			}
			// Drop in the page number text area image
			toSend.AddCommand( "gumppic %u %u %u",xStart+260,yWidth-28,0x98E);
			// Add the page number text to the text area for display
			toSend.AddCommand( "text %u %u %u %u",xStart+295,yWidth-27,901,linenum++);
			szBuffer = UString::sprintf( "Menu %i - Page %i", m, pagenum-1 );
			toSend.AddText( szBuffer );
			// Spin the tagged items loaded in from the dfn files.
			toSend.AddCommand( "button %u %u %u %u %u %u %u",15,position, 0x4B9, 0x4BA, 1, 0, buttonnum);
			toSend.AddCommand( "croppedtext %u %u %u %u %u %u",35, position-3,150,20, 50, linenum++ ); 
			toSend.AddText( data );
			if( tag.data()[0] != '<' && tag.data()[0] != ' ' )	// it actually has a picture, well bugger me! :>
			{
				// Draw a frame for the item to make it stand out a touch more.
				toSend.AddCommand( "resizepic %u %u %u %u %u", xOffset, yOffset, 0x53, 65, 100 );
				toSend.AddCommand( "checkertrans %u %u %u %u", xOffset + 7, yOffset + 9, 52, 82 );
				toSend.AddCommand( "tilepic %u %u %i",xOffset+5, yOffset+10, tag.toLong() );
				toSend.AddCommand( "croppedtext %u %u %u %u %u %u", xOffset, yOffset+65, 65, 20, 55, linenum++ );
				toSend.AddText( data );
				xOffset += XOFFSET;
				if( xOffset > 480 )
				{
					xOffset	=	SXOFFSET;
					yOffset	+=	YOFFSET;
				}
			}
			position += 20;
			++buttonnum;
			++numCounter;

			tag = ItemMenu->Next();
		}
		UI32 currentPage = 1;
		
		for( i = 0; i < numCounter; i += 12 )
		{
			toSend.AddCommand( "page %i", currentPage );
			if( i >= 10 )
			{
				if( s->ClientVersionMajor() >= 4 )
					toSend.AddCommand( "button %u %u %u %u %u %u",xStart+240,yWidth-25, 0x25EB, 0x25EA,0, currentPage-1); //back button
				else
					toSend.AddCommand( "button %u %u %u %u %u %u",xStart+240,yWidth-25, 0x15A2, 0x15A3,0, currentPage-1); //back button
			}
			if( ( numCounter > 12 ) && ( ( i + 12 ) < numCounter ) )
			{
				if( s->ClientVersionMajor() >= 4 )
					toSend.AddCommand( "button %u %u %u %u %u %u",xWidth-60,yWidth-25, 0x25E7, 0x25E6,0 ,currentPage+1);
				else
					toSend.AddCommand( "button %u %u %u %u %u %u",xWidth-60,yWidth-25, 0x15A5, 0x15A6,0 ,currentPage+1);
			}
			currentPage++;
		}		
	}
	else	// m != 1
	{
		// Now we make the first page that will essentially display our list of Main GM Groups.
		pagenum = 2;
		toSend.AddCommand( "page 1" );
		// Do the shard tab
		toSend.AddCommand( "gumppic %u %u %u ",17,yStart+47,0x138F); 
		szBuffer = "Shard";
		toSend.AddCommand( "text %u %u %u %u",42,yStart+46, 70, linenum++ );
		toSend.AddText( szBuffer );
		// here we hunt tags to make sure that we get them all from the itemmenus etc.
		UI08 numCounter = 0;
		position		= 80;
		xOffset			= SXOFFSET;
		yOffset			= SYOFFSET;
		
		// Drop in the page number text area image
		toSend.AddCommand( "gumppic %u %u %u",xStart+260,yWidth-28,0x98E);
		// Add the page number text to the text area for display
		toSend.AddCommand( "text %u %u %u %u",xStart+295,yWidth-27,901,linenum++);
		szBuffer = UString::sprintf( "Menu %i - Page %i", m, pagenum-1 );
		toSend.AddText( szBuffer );

		for( tag = ItemMenu->Next(); !ItemMenu->AtEnd(); tag = ItemMenu->Next() )
		{
			data = ItemMenu->GrabData();

			if( numCounter > 0 && (!(numCounter % 12 ) ) )
			{
				position = 80;
				toSend.AddCommand( "page %i", pagenum++ );
				xOffset=SXOFFSET;
				yOffset=SYOFFSET;
			}
			// Drop in the page number text area image
			if( tag.upper() == "INSERTADDMENUITEMS" )
			{
				// Check to see if the desired menu has any items to add
				if(g_mmapAddMenuMap.find(m)==g_mmapAddMenuMap.end())
				{
					continue;
				}
				// m contains the groupId that we need to do fetch the auto-addmenu items
				std::pair<ADDMENUMAP_CITERATOR,ADDMENUMAP_CITERATOR> pairRange = g_mmapAddMenuMap.equal_range( m );
				for(ADDMENUMAP_CITERATOR CI = pairRange.first;CI!=pairRange.second;CI++)
				{
					toSend.AddCommand( "button %u %u %u %u %u %u %u",15,position, 0x4B9, 0x4BA ,1,0, buttonnum);
					toSend.AddCommand( "croppedtext %u %u %u %u %u %u",35, position-3,150,20, 40, linenum ); 
					toSend.AddText( CI->second.itemName );
					// check to make sure that we have an image now, seeing as we might not have one with the new changes in 0.98.01.2+
					if(CI->second.tileID != 0)
					{
						// Draw a frame for the item to make it stand out a touch more.
						toSend.AddCommand( "resizepic %u %u %u %u %u",xOffset,yOffset,0x53,65,100);
						toSend.AddCommand( "checkertrans %u %u %u %u",xOffset+7,yOffset+9,52,82);
						toSend.AddCommand( "tilepic %u %u %i", xOffset+5, yOffset+10, CI->second.tileID );
						toSend.AddCommand( "croppedtext %u %u %u %u %u %u",xOffset,yOffset+65,65,20,55,linenum++);
						toSend.AddText( CI->second.itemName );
						toSend.AddText( CI->second.itemName.c_str() );
						xOffset += XOFFSET;
						if(xOffset>480)
						{
							xOffset=SXOFFSET;
							yOffset+=YOFFSET;
						}
					}
					position += 20;
					++linenum;
					++buttonnum;
					++numCounter;
				}
				continue;
			}
			toSend.AddCommand( "button %u %u %u %u %u %u %u",15,position, 0x4B9, 0x4BA, 1, 0, buttonnum );
			toSend.AddCommand( "croppedtext %u %u %u %u %u %u",35, position-3,150,20, 50, linenum++ ); 
			toSend.AddText( data );
			if( tag.data()[0] != '<' && tag.data()[0] != ' ' )	// it actually has a picture, well bugger me! :>
			{
				// Draw a frame for the item to make it stand out a touch more.
				toSend.AddCommand( "resizepic %u %u %u %u %u", xOffset, yOffset, 0x53, 65, 100 );
				toSend.AddCommand( "checkertrans %u %u %u %u", xOffset + 7, yOffset + 9, 52, 82 );
				toSend.AddCommand( "tilepic %u %u %i", xOffset + 5, yOffset + 10, tag.toLong() );
				toSend.AddCommand( "croppedtext %u %u %u %u %u %u", xOffset, yOffset + 65, 65, 20, 55, linenum++ );
				toSend.AddText( data );
				xOffset += XOFFSET;
				if( xOffset > 480 )
				{
					xOffset	=	SXOFFSET;
					yOffset	+=	YOFFSET;
				}
			}
			position += 20;
			++buttonnum;
			++numCounter;

			tag = ItemMenu->Next();
		}
		
		UI32 currentPage = 1;
		for( i = 0; i < numCounter; i += 12 )
		{
			toSend.AddCommand( "page %i", currentPage );
			if( i >= 10 )
			{
				if( s->ClientVersionMajor() >= 4 )
					toSend.AddCommand( "button %u %u %u %u %u %u",xStart+240,yWidth-25, 0x25EB, 0x25EA,0, currentPage-1); //back button
				else
					toSend.AddCommand( "button %u %u %u %u %u %u",xStart+240,yWidth-25, 0x15A2, 0x15A3,0, currentPage-1); //back button
			}
			if( ( numCounter > 12 ) && ( ( i + 12 ) < numCounter ) )
			{
				if( s->ClientVersionMajor() >= 4 )
					toSend.AddCommand( "button %u %u %u %u %u %u",xWidth-60,yWidth-25, 0x25E7, 0x25E6,0 ,currentPage+1);
				else
					toSend.AddCommand( "button %u %u %u %u %u %u",xWidth-60,yWidth-25, 0x15A5, 0x15A6,0 ,currentPage+1);
			}
			++currentPage;
		}		
	}


	// Reserved page # 800 - 899 for the server support pages
	toSend.AddCommand( "page 30" );  
	// Show the selected tab image for this page. 
	toSend.AddCommand( "gumppic %u %u %u",105, yStart + 47, 0x138F );
	toSend.AddCommand( "text %u %u %u %u",132, yStart + 46, 70, linenum++ );
	toSend.AddText( "Server" );
	// Create the Server Shutdown button
	if( s->ClientVersionMajor() >= 4 )
		toSend.AddCommand( "button %u %u %u %u %u %u %u", 45, yStart + 72, 0x2A58, 0x2A44, 1, 4, tabNumber++ );
	else
		toSend.AddCommand( "button %u %u %u %u %u %u %u", 45, yStart + 72, 0x047F, 0x0480, 1, 4, tabNumber++ );
	toSend.AddCommand( "text %u %u %u %u", 54, yStart + 76, 52, linenum++ );
	toSend.AddText( "ShutdownServer" );
	if( s->ClientVersionMajor() >= 4 )
		toSend.AddCommand( "button %u %u %u %u %u %u %u", 165, yStart + 75, 0x5689, 0x568A, 0, 31, tabNumber++ );
	else
		toSend.AddCommand( "button %u %u %u %u %u %u %u", 165, yStart + 75, 0x047F, 0x0480, 0, 31, tabNumber++ );
	// Need a seperator
	if( s->ClientVersionMajor() >= 4 )
		toSend.AddCommand( "gumppictiled %u %u %u %u %u", xStart + 21, yStart + 105, 165, 5, 0x2393 );
	else
		toSend.AddCommand( "gumppictiled %u %u %u %u %u", xStart + 21, yStart + 105, 165, 5, 0x145E );
	// Ok short message saying that were working on this tab.
	toSend.AddCommand( "htmlgump %u %u %u %u %u %u %u", xStart + 16, yStart + 130, 175, 50, linenum++, 2, 0 );
	toSend.AddText( "Currently this tab is under construction." );

	// Reserved pages 900-999 for the online help system. (comming soon)
	toSend.AddCommand( "page 31" );
	// Ok display the scroll that we use to display our help information
	toSend.AddCommand( "resizepic %u %u %u %u %u", xStart + 205, yStart + 62, 0x1432, 175, 200 );
	// Write out what page were on (Mainly for debug purposes
	szBuffer = UString::sprintf( "%5u", 31 );
	toSend.AddCommand( "text %u %u %u %u",xWidth-58,yWidth-25,110, linenum++ );
	toSend.AddText( szBuffer );
	// Ok, now the job of pulling the rest of the first itemmenu information and making tabs for them
	szBuffer = "Page 31";
	toSend.AddCommand( "text %u %u %u %u",30, yStart + 200, 87, linenum++ );
	toSend.AddText( szBuffer );
	toSend.AddCommand( "button %u %u %u %u %u %u %u",104, yStart + 300, 0x138E, 0x138E, 0, 1, tabNumber++ );

#if defined( UOX_DEBUG_MODE )
	Console << "==============================" << myendl;
#endif
	// Finish up and send the gump to the client socket.	
	toSend.Finalize();
	s->Send( &toSend );
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool CPIHelpRequest::Handle( void )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Opens the "Help" Menu from the paperdoll
//o---------------------------------------------------------------------------o
bool CPIHelpRequest::Handle( void )
{
	UI16 gmnumber	= 0;
	CChar *mChar	= tSock->CurrcharObj();
	
	UString sect = "GMMENU " + UString::number( menuNum );
	ScriptSection *GMMenu = FileLookup->FindEntry( sect, menus_def );
	if( GMMenu == NULL )
		return true;

	std::string line;
	CPOpenGump toSend( *mChar );
	toSend.GumpIndex( menuNum );
	UString tag		= GMMenu->First();
	UString data	= GMMenu->GrabData();
	line = tag + " " + data;
	toSend.Question( line );
	for( tag = GMMenu->Next(); !GMMenu->AtEnd(); tag = GMMenu->Next() )
	{
		data = GMMenu->GrabData();
		line = tag + " " + data;
		toSend.AddResponse( gmnumber++, 0, line );
		tag = GMMenu->Next();
	}
	toSend.Finalize();
	tSock->Send( &toSend );
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function	  -  void CPage( int s, const std::string reason )
//|   Date		  -  Unknown
//|   Programmer  -  Unknown  (Touched tabstops by EviLDeD Dec 23, 1998)
//o---------------------------------------------------------------------------o
//|   Purpose     -  Used when player pages a counselor
//o---------------------------------------------------------------------------o
void CPage( CSocket *s, const std::string& reason )
{
	CChar *mChar = s->CurrcharObj();
	UI08 a1 = mChar->GetSerial( 1 );
	UI08 a2 = mChar->GetSerial( 2 );
	UI08 a3 = mChar->GetSerial( 3 );
	UI08 a4 = mChar->GetSerial( 4 );
	
	HelpRequest pageToAdd;
	pageToAdd.Reason( reason );
	pageToAdd.WhoPaging( mChar->GetSerial() );
	pageToAdd.IsHandled( false );
	pageToAdd.TimeOfPage( time( NULL ) );

	SERIAL callNum = CounselorQueue->Add( &pageToAdd );
	if( callNum != INVALIDSERIAL )
	{
		mChar->SetPlayerCallNum( callNum );
		if( reason == "OTHER" )
		{
			mChar->SetSpeechMode( 2 );
			s->sysmessage( 359 );
		}
		else
		{
			bool x = false;
			char temp[1024];
			sprintf( temp, "Counselor Page from %s [%x %x %x %x]: %s", mChar->GetName().c_str(), a1, a2, a3, a4, reason.c_str() );
			Network->PushConn();
			for( CSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
			{
				CChar *iChar = iSock->CurrcharObj();
				if( iChar->IsGMPageable() || iChar->IsCounselor() )
				{
					x = true;
					iSock->sysmessage( temp );
				}
			}
			Network->PopConn();
			if( x )
				s->sysmessage( 360 );
			else
				s->sysmessage( 361 );
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function	  :  void GMPage( CSocket *s, const std::string reason )
//|   Date		  :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Used when player calls a GM
//o---------------------------------------------------------------------------o
void GMPage( CSocket *s, const std::string& reason )
{
	CChar *mChar = s->CurrcharObj();
	UI08 a1 = mChar->GetSerial( 1 );
	UI08 a2 = mChar->GetSerial( 2 );
	UI08 a3 = mChar->GetSerial( 3 );
	UI08 a4 = mChar->GetSerial( 4 );
	
	HelpRequest pageToAdd;
	pageToAdd.Reason( reason );
	pageToAdd.WhoPaging( mChar->GetSerial() );
	pageToAdd.IsHandled( false );
	pageToAdd.TimeOfPage( time( NULL ) );
	SERIAL callNum = GMQueue->Add( &pageToAdd );
	if( callNum != INVALIDSERIAL )
	{
		mChar->SetPlayerCallNum( callNum );
		if( reason == "OTHER" )
		{
			mChar->SetSpeechMode( 1 );
			s->sysmessage( 362 );
		}
		else
		{
			bool x = false;
			char temp[1024];
			sprintf( temp, "Page from %s [%x %x %x %x]: %s", mChar->GetName().c_str(), a1, a2, a3, a4, reason.c_str() );
			Network->PushConn();
			for( CSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
			{
				CChar *iChar = iSock->CurrcharObj();
				if( !ValidateObject( iChar ) )
					continue;
				if( iChar->IsGMPageable() )
				{
					x = true;
					iSock->sysmessage( temp );
				}
			}
			Network->PopConn();
			if( x )
				s->sysmessage( 363 );
			else
				s->sysmessage( 364 );
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void HandleGumpCommand( CSocket *s, UString cmd, UString data )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Execute a command from scripts
//o---------------------------------------------------------------------------o
void HandleGumpCommand( CSocket *s, UString cmd, UString data )
{
	CChar *mChar = s->CurrcharObj();
	if( !s || cmd.empty() )
		return;

	cmd		= cmd.upper();
	data	= data.upper();
	UString builtString;
	
	switch( cmd.data()[0] )
	{
		case 'A':
			if( !data.empty() )
			{
				bool runCommand = false;
				ObjectType itemType = OT_ITEM;
				if( cmd == "ADDITEM" )
					runCommand = true;
				else if( cmd == "ADDSPAWNER" )
				{
					runCommand = true;
					itemType = OT_SPAWNER;
				}
				if( runCommand )
				{
					if( data.sectionCount( "," ) != 0 )
					{
						UString tmp		= data.section( ",", 0, 0 ).stripWhiteSpace();
						UI16 num			= data.section( ",", 1, 1 ).stripWhiteSpace().toUShort();
						Items->CreateScriptItem( s, mChar, tmp, num, itemType, true );
					}
					else
						Items->CreateScriptItem( s, mChar, data, 0, itemType, true );
				}
			}
			break;
		case 'C':
			if( cmd == "CPAGE" )
			{
				if( data.empty() )
					return;
				CPage( s, data );
			}
			break;
		case 'G':
			if( cmd == "GMMENU" )
			{
				if( data.empty() )
					return;
				CPIHelpRequest toHandle( s, data.toUShort() );
				toHandle.Handle();
			}
			else if( cmd == "GMPAGE" )
			{
				if( data.empty() )
					return;
				GMPage( s, data );
			}
			else if( cmd == "GCOLLECT" )
				CollectGarbage();
			else if( cmd == "GOPLACE" )
			{
				if( data.empty() )
					return;
				size_t placeNum = data.toULong();
				if( cwmWorldState->goPlaces.find( placeNum ) != cwmWorldState->goPlaces.end() )
				{
					GoPlaces_st toGoTo = cwmWorldState->goPlaces[placeNum];
					mChar->SetLocation( toGoTo.x, toGoTo.y, toGoTo.z, toGoTo.worldNum );
				}
			}
			else if( cmd == "GUIINFORMATION" )
			{
				GumpDisplay guiInfo( s, 400, 300 );
				guiInfo.SetTitle( CVersionClass::GetProductName() + " Status" );
				builtString = GetUptime();
				guiInfo.AddData( "Version", CVersionClass::GetVersion() + "(" + CVersionClass::GetBuild() + ") [" + OS_STR + "]" );
				guiInfo.AddData( "Compiled By", CVersionClass::GetName() );
				guiInfo.AddData( "Uptime", builtString );
				guiInfo.AddData( "Accounts", Accounts->size() );
				guiInfo.AddData( "Items", ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ) );
				guiInfo.AddData( "Chars", ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ) );
				guiInfo.AddData( "Players in world", cwmWorldState->GetPlayersOnline() );
				guiInfo.Send( 0, false, INVALIDSERIAL );
			}
			break;
		case 'I':
			if( cmd == "ITEMMENU" )
			{
				if( data.empty() )
					return;
				BuildAddMenuGump( s, data.toUShort() );
			}
			else if( cmd == "INFORMATION" )
			{
				builtString = GetUptime();
				s->sysmessage( 1211, builtString.c_str(), cwmWorldState->GetPlayersOnline(), Accounts->size(), ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ), ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ) );
			}
			break;
		case 'M':
			if( cmd == "MAKEMENU" )
			{
				if( data.empty() )
					return;
				Skills->NewMakeMenu( s, data.toLong(), (UI08)s->AddID() );
			}
			break;
		case 'N':
			if( cmd == "NPC" )
			{
				if( data.empty() )
					return;
				s->XText( data );
				s->target( 0, TARGET_ADDSCRIPTNPC, 1212, data.c_str() );
			}
			break;
		case 'P':
			if( cmd == "POLYMORPH" )
			{
				if( data.empty() )
					return;
				UI16 newBody = data.toUShort();
				mChar->SetID( newBody );
				mChar->SetOrgID( newBody );
			}
			break;
		case 'S':
			if( cmd == "SYSMESSAGE" )
			{
				if( data.empty() )
					return;
				s->sysmessage( data.c_str() );
			}
			else if( cmd == "SKIN" )
			{
				if( data.empty() )
					return;
				COLOUR newSkin = data.toUShort();
				mChar->SetSkin( newSkin );
				mChar->SetOrgSkin( newSkin );
			}
			break;
		case 'V':
			if( cmd == "VERSION" )
				s->sysmessage( "%s v%s(%s) [%s] Compiled by %s ", CVersionClass::GetProductName().c_str(), CVersionClass::GetVersion().c_str(), CVersionClass::GetBuild().c_str(), OS_STR, CVersionClass::GetName().c_str() );
			break;
		case 'W':
			if( cmd == "WEBLINK" )
			{
				if( data.empty() )
					return;
				s->OpenURL( data );
			}
			break;
	}
}


//o---------------------------------------------------------------------------o
//|   Function    :  void HandleAddMenuButton( CSocket *s, long button )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button pressed in add menu gump
//o---------------------------------------------------------------------------o
void HandleAddMenuButton( CSocket *s, long button )
{
	SI32 addMenuLoc	= s->TempInt();
	UString sect = "ITEMMENU " + UString::number( addMenuLoc );
	ScriptSection *ItemMenu = FileLookup->FindEntry( sect, items_def );
	if( ItemMenu == NULL )
		return;
	
	// If we get here we have to check to see if there are any other entryies added via the auto-addmenu code. Each item == 2 entries IE: IDNUM=Text name of Item, and ADDITEM=itemID to add
	std::pair<ADDMENUMAP_CITERATOR,ADDMENUMAP_CITERATOR> pairRange = g_mmapAddMenuMap.equal_range( addMenuLoc );
	UI32 autoAddMenuItemCount = 0;
	for(ADDMENUMAP_CITERATOR CI = pairRange.first;CI!=pairRange.second;++CI)
		autoAddMenuItemCount += 2;	// Need to inicrement by 2 because each entry is measured in the dfn' as two lines. Used in teh calculation below.
	// let's skip over the name, and get straight to where we should be headed
	size_t entryNum = static_cast<size_t>((button - 6) * 2);
	autoAddMenuItemCount += ItemMenu->NumEntries();
	if( autoAddMenuItemCount >= entryNum )
	{
		UString tag		= ItemMenu->MoveTo( entryNum );
		UString data	= ItemMenu->GrabData();
		HandleGumpCommand( s, tag, data );
	}
}

void HandleHowTo( CSocket *sock, int cmdNumber )
{
	int iCounter				= 0;
	UI08 cmdLevelReq			= 0xFF;
	UI08 cmdType				= 0xFF;
	std::string cmdName			= "";
	COMMANDMAP_ITERATOR toFind	= CommandMap.begin();
	while( iCounter != cmdNumber && toFind != CommandMap.end() )
	{
		++iCounter;
		++toFind;
	}
	if( iCounter != cmdNumber )
	{
		TARGETMAP_ITERATOR findTarg = TargetMap.begin();
		while( iCounter != cmdNumber && findTarg != TargetMap.end() )
		{
			++iCounter;
			++findTarg;
		}
		if( iCounter != cmdNumber )
		{
			JSCOMMANDMAP_ITERATOR findJS = JSCommandMap.begin();
			while( iCounter != cmdNumber && findJS != JSCommandMap.end() )
			{
				++iCounter;
				++findJS;
			}
			if( iCounter == cmdNumber )
			{
				cmdName		= findJS->first;
				cmdLevelReq = findJS->second.cmdLevelReq;
			}
		}
		else
		{
			cmdName		= findTarg->first;
			cmdLevelReq = findTarg->second.cmdLevelReq;
		}
	}
	else
	{
		cmdName		= toFind->first;
		cmdLevelReq = toFind->second.cmdLevelReq;
		cmdType		= toFind->second.cmdType;
	}
	if( iCounter == cmdNumber )
	{
		// Build gump structure here, with basic information like Command Level, Name, Command Type, and parameters (if any, from table)
		GumpDisplay CommandInfo( sock, 480, 320 );
		CommandInfo.SetTitle( cmdName );

		CommandInfo.AddData( "Minimum Command Level", cmdLevelReq );
		switch( cmdType )
		{
			case CMD_TARGET:		CommandInfo.AddData( "Syntax", "None (generic target)" );					break;
			case CMD_FUNC:			CommandInfo.AddData( "Syntax", "None (generic command)" );					break;
			case CMD_SOCKFUNC:		CommandInfo.AddData( "Syntax", "None (generic command)" );					break;
			case CMD_TARGETINT:		CommandInfo.AddData( "Syntax", "arg1 (hex or decimal)" );					break;
			case CMD_TARGETXYZ:		CommandInfo.AddData( "Syntax", "arg1 arg2 arg3 (hex or decimal)" );			break;
			case CMD_TARGETTXT:		CommandInfo.AddData( "Syntax", "String" );									break;
			default:				break;
		}

		char filename[256];
		sprintf( filename, "help/commands/%s.txt", cmdName.c_str() );
		std::ifstream toOpen( filename );
		if( !toOpen.is_open() )
			CommandInfo.AddData( "", "No extra information is available about this command", 7 );
		else
		{
			char cmdLine[128];
			while( !toOpen.eof() && !toOpen.fail() )
			{
				toOpen.getline( cmdLine, 128 );
				if( cmdLine[0] != 0 )
					CommandInfo.AddData( "", cmdLine, 7 );
			};
			toOpen.close();
		}
		CommandInfo.Send( 4, false, INVALIDSERIAL );
	}
	else
		sock->sysmessage( 280 );
}

void HandleHowToButton( CSocket *s, long button )
{
	HandleHowTo( s, button - 2 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool CPIGumpMenuSelect::Handle( void )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles button press in gumps
//o---------------------------------------------------------------------------o
bool CPIGumpMenuSelect::Handle( void )
{
	id			= tSock->GetDWord( 3 );
	gumpID		= tSock->GetDWord( 7 );
	buttonID	= tSock->GetDWord( 11 );
	switchCount	= tSock->GetDWord( 15 );

#if defined( UOX_DEBUG_MODE )
	Console << "CPIGumpMenuSelect::Handle(void)" << myendl;
	Console << "        GumpID : " << gumpID << myendl;
	Console << "      ButtonID : " << buttonID << myendl;
	Console << "   SwitchCount : " << switchCount << myendl;
#endif

	if( gumpID == 461 ) // Virtue gump
	{
		CChar *targChar = NULL;
		if( buttonID == 1 && switchCount > 0 ) // Clicked on a players Virtue Gump icon
		{
			SERIAL targSer = tSock->GetDWord( 19 );
			targChar = calcCharObjFromSer( targSer );
		}
		else // Clicked an item on the virtue gump
			targChar = calcCharObjFromSer( id );

		cScript *toExecute = JSMapping->GetScript( (UI16)0 ); // Global script	
		if( toExecute != NULL )
			toExecute->OnVirtueGumpPress( tSock->CurrcharObj(), targChar, buttonID );

		return true;
	}

	textOffset	= 19 + (4 * switchCount);
	textCount	= tSock->GetDWord( textOffset );

#if defined( UOX_DEBUG_MODE )
	Console << "    TextOffset : " << textOffset << myendl;
	Console << "     TextCount : " << textCount << myendl;
#endif

	BuildTextLocations();

	if( buttonID > 10000 ) 
	{
		BuildGumpFromScripts( tSock, (UI16)(buttonID - 10000) );
		return true;
	}
	if( gumpID >= 8000 && gumpID <= 8020 )
	{
		GuildSys->GumpChoice( tSock );
		// guild collection call goes here
		return true;
	}
	else if( gumpID >= 0xFFFF )	// script gump
	{
		cScript *toExecute = JSMapping->GetScript( (gumpID - 0xFFFF) );
		if( toExecute != NULL )
			toExecute->HandleGumpPress( this );
	}
	else if( gumpID == 21 ) // Multi functional gump
	{
		MultiGumpCallback( tSock, id, buttonID );
		return true;
	}
	else if( gumpID > 13 ) 
		return true; //increase this value with each new gump added.
	if( buttonID == 1 ) 
	{
		if( gumpID == 4 )
			WhoList->GMLeave();
		else if( gumpID == 11 )
			OffList->GMLeave();
		return true;
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
	// 10	Unused
	// 11	Who's Offline
	// 12	New Make Menu
	// 13	HOWTO

#if defined( UOX_DEBUG_MODE )
	Console << "Type is " << gumpID << " button is " <<  buttonID << myendl;
#endif

	CItem *j = NULL;
	switch( gumpID )
	{
		case 1:	HandleTweakItemButton( tSock, buttonID, id, gumpID );			break;	// Tweak Item
		case 2:	HandleTweakCharButton( tSock, buttonID, id, gumpID );			break;	// Tweak Char
		case 3:	HandleTownstoneButton( tSock, buttonID, id, gumpID );			break;	// Townstones
		case 4:	WhoList->ButtonSelect( tSock, static_cast<UI16>(buttonID), static_cast<UI08>(gumpID) );					break;	// Wholist
		case 5:																		// House Functions
			j = static_cast<CItem *>(tSock->TempObj());
			tSock->TempObj( NULL );
			if( ValidateObject( j ) ) 
				HandleHouseButton( tSock, buttonID, j );
			break;
		case 6:																		// Hair Dye Menu
			j = static_cast<CItem *>(tSock->TempObj());
			tSock->TempObj( NULL );
			if( ValidateObject( j ) )
				HandleHairDyeButton( tSock, j );
			break;
		case 7:																		// Accounts
			CChar *c;
			c = calcCharObjFromSer( id );
			if( ValidateObject( c ) )
				HandleAccountButton( tSock, buttonID, c );
			break;
		case 8:	HandleAccountModButton( this );							break;	// Race Editor
		case 9:	HandleAddMenuButton( tSock, buttonID );							break;	// Add Menu
		case 11: OffList->ButtonSelect( tSock, static_cast<UI16>(buttonID), static_cast<UI08>(gumpID) );				break;	// Who's Offline
		case 12: Skills->HandleMakeMenu( tSock, buttonID, static_cast<int>(tSock->AddID()));	break;	// New Make Menu
		case 13: HandleHowToButton( tSock, buttonID );							break;	// Howto
	}
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void tweakItemMenu( CSocket *s, CItem *c )
//|   Date        :  Unknown
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handle new tweak item menu
//o---------------------------------------------------------------------------o
void tweakItemMenu( CSocket *s, CItem *i )
{
	if( !ValidateObject( i ) )
		return;
	GumpDisplay tweakItem( s, 350, 345 );
	tweakItem.SetTitle( "Tweak Item Menu" );

	UI16 itemID		= i->GetID();
	UI16 itemColour = i->GetColour();

	tweakItem.AddData( "ID", itemID, 5 );
	if( !i->GetName().empty() )
		tweakItem.AddData( "Name", i->GetName() );
	else
		tweakItem.AddData( "Name", "#" );
	if( i->GetName2() != NULL && i->GetName2()[0] > 0 )
		tweakItem.AddData( "Name2", i->GetName2() );
	else
		tweakItem.AddData( "Name2", "#" );
	tweakItem.AddData( "Colour", itemColour, 5 );
	tweakItem.AddData( "Layer", static_cast<UI08>(i->GetLayer()) );
	tweakItem.AddData( "Type", static_cast<UI08>(i->GetType()) );
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
	tweakItem.AddData( "Defense", i->GetResist( PHYSICAL ) );
	tweakItem.AddData( "Speed", i->GetSpeed() );
	tweakItem.AddData( "Rank", i->GetRank() );
	tweakItem.AddData( "More", i->GetTempVar( CITV_MORE ) );
	tweakItem.AddData( "More X", i->GetTempVar( CITV_MOREX ) );
	tweakItem.AddData( "More Y", i->GetTempVar( CITV_MOREY ) );
	tweakItem.AddData( "More Z", i->GetTempVar( CITV_MOREZ ) );
	tweakItem.AddData( "Poisoned", i->GetPoisoned() );
	tweakItem.AddData( "Weight", i->GetWeight() );
	tweakItem.AddData( "Decay", i->isDecayable()?1:0 );
	tweakItem.AddData( "Good", i->GetGood() );
	tweakItem.AddData( "Buy Value", i->GetBuyValue() );
	tweakItem.AddData( "Sell Value", i->GetSellValue() );
	tweakItem.AddData( "Carve", i->GetCarve() );
	tweakItem.AddData( "Stackable", i->isPileable()?1:0 );
	tweakItem.AddData( "Dyeable", i->isDyeable()?1:0 );
	tweakItem.AddData( "Corpse", i->isCorpse()?1:0 );
	tweakItem.AddData( "Visible", i->GetVisible() );
	tweakItem.AddData( "Creator", i->GetCreator() );
	tweakItem.AddData( "Script ID:", i->GetScriptTrigger() );
	tweakItem.AddData( "AmmoID:", i->GetAmmoID() );
	tweakItem.AddData( "AmmoHue:", i->GetAmmoHue() );
	tweakItem.AddData( "AmmoFX:", i->GetAmmoFX() );
	tweakItem.AddData( "AmmoFXHue:", i->GetAmmoFXHue() );
	tweakItem.AddData( "AmmoFXRender:", i->GetAmmoFXRender() );
	tweakItem.AddData( "WeightMax:", i->GetWeightMax() );
	tweakItem.AddData( "BaseWeight:", i->GetBaseWeight() );
	if( i->GetObjType() == OT_SPAWNER )
	{
		CSpawnItem *spawnItem = static_cast<CSpawnItem *>(i);

		std::string spawnSection = spawnItem->GetSpawnSection();
		if( spawnSection.empty() )
			spawnSection = "0";

		if( spawnItem->IsSectionAList() )
			tweakItem.AddData( "Spawnobjlist", spawnSection );
		else
			tweakItem.AddData( "Spawnobj", spawnSection );
	}
	tweakItem.Send( 1, true, i->GetSerial() );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleTweakItemText( CSocket *s, long index )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles new values for 'tweak items
//o---------------------------------------------------------------------------o
void CPIGumpInput::HandleTweakItemText( UI08 index )
{
	CChar *mChar	= tSock->CurrcharObj();

	if( mChar->GetCommandLevel() >= CL_GM || mChar->GetAccount().wAccountIndex == 0 )
	{
		CItem *j = calcItemObjFromSer( id );
		if( !ValidateObject( j ) ) 
			return;
		if( unk[0] == 0 )
		{
			tweakItemMenu( tSock, j );
			return;
		}
		switch( index )
		{
			case 1:		j->SetID( reply.toUShort() );				break;	// ID
			case 2:		j->SetName( reply );						break;	// Name
			case 3:		j->SetName2( reply.c_str() );				break;	// Name 2
			case 4:		j->SetColour( reply.toUShort() );			break;	// Colour
			case 5:		j->SetLayer( static_cast<ItemLayers>(reply.toUByte()) );		break;	// Layer
			case 6:		j->SetType( static_cast<ItemTypes>(reply.toUByte()) );			break;	// Type
			case 7:		j->SetMovable( reply.toByte() );			break;	// Moveable
			case 8:		j->SetLocation( reply.toShort(), j->GetY(), j->GetZ() );			break;	// X
			case 9:		j->SetLocation( j->GetX(), reply.toShort(), j->GetZ() );			break;	// Y
			case 10:	j->SetZ( reply.toByte() );					break;	// Z
			case 11:	j->SetAmount( reply.toUShort() );			break;	// Amount
			case 12:	j->SetStrength( reply.toShort() );			break;	// Strength
			case 13:	j->SetHP( reply.toShort() ); 				break;	// Hit Points
			case 14:	j->SetMaxHP( reply.toShort() );				break;	// Max HP
			case 15:	j->SetLoDamage( reply.toShort() );			break;	// Low Damage
			case 16:	j->SetHiDamage( reply.toShort() );			break;	// High Damage
			case 17:	j->SetResist( reply.toUShort(), PHYSICAL ); break;	// Defense
			case 18:	j->SetSpeed( reply.toUByte() );				break;	// Speed
			case 19:	j->SetRank( reply.toByte() );				break;	// Rank
			case 20:	j->SetTempVar( CITV_MORE, reply.toULong() );				break;	// More
			case 21:	j->SetTempVar( CITV_MOREX, reply.toULong() );				break;	// More X
			case 22:	j->SetTempVar( CITV_MOREY, reply.toULong() );				break;	// More Y
			case 23:	j->SetTempVar( CITV_MOREZ, reply.toULong() );				break;	// More Z
			case 24:	j->SetPoisoned( reply.toUByte() );			break;	// Poisoned
			case 25:	j->SetWeight( reply.toLong() );				break;	// Weight
			case 26:	j->SetDecayable( reply.toUByte() != 0 );	break;	// Decay
			case 27:	j->SetGood( reply.toShort() );				break;	// Good
			case 28:	j->SetBuyValue( reply.toULong() );			break;	// Buy Value
			case 29:	j->SetSellValue( reply.toULong() );			break;	// Sell Value - Not implimented yet
			case 30:	j->SetCarve( reply.toShort() );				break;	// Carve
			case 31: 	j->SetPileable( reply.toUByte() != 0 );		break;	// Stackable
			case 32:	j->SetDye( reply.toUByte() != 0 );			break;	// Dyable
			case 33:	j->SetCorpse( reply.toUByte() != 0 );		break;	// Corpse
			case 34:	j->SetVisible( (VisibleTypes)reply.toByte() );	break;	// Visible
			case 35:	j->SetCreator( reply.toULong() );			break;	// Creator
			case 36:	j->SetScriptTrigger( reply.toULong() );		break; //ScriptTrigger ID
			case 37:	j->SetAmmoID( reply.toUShort() );			break; //AmmoID
			case 38:	j->SetAmmoHue( reply.toUShort() );			break; //AmmoHue
			case 39:	j->SetAmmoFX( reply.toUShort() );			break; //AmmoFX
			case 40:	j->SetAmmoFXHue( reply.toUShort() );		break; //AmmoFXHue
			case 41:	j->SetAmmoFXRender( reply.toUShort() );		break; //AmmoFXRender
			case 42:	j->SetWeightMax( reply.toLong() );			break; //WeightMax
			case 43:	j->SetBaseWeight( reply.toULong() );		break; //BaseWeight
			case 44:
						if( j->GetObjType() == OT_SPAWNER )
							(static_cast<CSpawnItem *>(j))->SetSpawnSection( reply.c_str() );	break;	// Spawnobj/Spawnobjlist
		}
		tweakItemMenu( tSock, j );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void tweakCharMenu( CSocket *s, CChar *c )
//|   Date        :  Unknown
//|   Programmer  :  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handle new tweak character menu
//o---------------------------------------------------------------------------o
void tweakCharMenu( CSocket *s, CChar *c )
{
	if( !ValidateObject( c ) )
		return;

	GumpDisplay tweakChar( s, 350, 345 );
	tweakChar.SetTitle( "Tweak Character Menu" );

	UI16 charID = c->GetID();
	UI16 getSkin = c->GetSkin();

	if( !c->GetName().empty() )
		tweakChar.AddData( "Name", c->GetName() );
	else
		tweakChar.AddData( "Name", "None" );
	if( !c->GetTitle().empty() )
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
	tweakChar.AddData( "Defense", c->GetResist( PHYSICAL ) );
	tweakChar.AddData( "Fame", c->GetFame() );
	tweakChar.AddData( "Karma", c->GetKarma() );
	tweakChar.AddData( "Kills", c->GetKills() );
	tweakChar.AddData( "AI Type", c->GetNPCAiType()  );
	tweakChar.AddData( "NPC Wander", c->GetNpcWander() );
	tweakChar.AddData( "Hunger", c->GetHunger() );
	tweakChar.AddData( "Poison", c->GetPoisonStrength() );
	tweakChar.AddData( "Weight", c->GetWeight() );
	tweakChar.AddData( "Carve", c->GetCarve() );
	tweakChar.AddData( "Visible", c->GetVisible() );
	tweakChar.AddData( "Script ID:", c->GetScriptTrigger() );
	tweakChar.Send( 2, true, c->GetSerial() );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleTweakCharText( CSocket *s, long index )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles new values for 'tweak chars
//o---------------------------------------------------------------------------o
void CPIGumpInput::HandleTweakCharText( UI08 index )
{
	CChar *mChar	= tSock->CurrcharObj();
	if( mChar->GetCommandLevel() >= CL_GM || mChar->GetAccount().wAccountIndex == 0 )
	{
		CChar *j = calcCharObjFromSer( id );
		if( !ValidateObject( j ) ) 
			return;

		if( unk[0] == 0 )
		{
			tweakCharMenu( tSock, j );
			return;
		}
		UI16 k;
		
		switch( index )
		{
			case 1:		j->SetName( reply );						break;	// Name
			case 2:		j->SetTitle( reply );					break;	// Title
			case 3:													// Body
				k = reply.toUShort();
				if( k <= 0x3e2 ) // body-values >0x3e crash the client
				{ 
					j->SetID( k );
					j->SetOrgID( k );
				}
				break;
			case 4:													// Skin
				k = reply.toUShort();
				j->SetColour( k );
				j->SetOrgSkin( k );
				break;
			case 5:		j->SetRace( reply.toUShort() );			break;	// Race
			case 6:		j->SetLocation( reply.toShort(), j->GetY(), j->GetZ() );	break;	// X
			case 7:		j->SetLocation( j->GetX(), reply.toShort(), j->GetZ() );	break;	// Y
			case 8:		j->SetZ( reply.toByte() );									break;	// Z
			case 9:		j->SetDir( static_cast<UI08>((reply.toUByte())&0x0F) );		break;	// Direction
			case 10:												// Strength
				if( reply.toShort() > 0 ) 
					j->SetStrength( reply.toShort() ); 
				break;
			case 11:												// Dexterity
				if( reply.toShort() > 0 ) 
					j->SetDexterity( reply.toShort() );
				break;
			case 12:												// Intelligence
				if( reply.toShort() > 0 ) 
					j->SetIntelligence( reply.toShort() ); 
				break;
			case 13:												// Hitpoints
				if( reply.toShort() > 0 )
					j->SetHP( reply.toShort() );
				break;
			case 14:												// Stamina
				if( reply.toUShort() > 0 )
					j->SetStamina( reply.toUShort() );
				break;
			case 15:												// Mana
				if( reply.toShort() > 0 )
					j->SetMana( reply.toShort() );
				break;
			case 16:	j->SetHiDamage( reply.toShort() );		break;	// Low Damage
			case 17:	j->SetLoDamage( reply.toShort() );		break;	// High Damage
			case 18:	j->SetResist( reply.toUShort(), PHYSICAL );	break;	// Defense
			case 19:	j->SetFame( reply.toShort() );			break;	// Fame
			case 20:	j->SetKarma( reply.toShort() );			break;	// Karma
			case 21:	j->SetKills( reply.toShort() );			break;	// Kills
			case 22:												// AI Type
				if( j->IsNpc() )
					j->SetNPCAiType( reply.toShort() );
				break;
			case 23:												// NPC Wander
				if( j->IsNpc() )
					j->SetNpcWander( reply.toByte() );
				break;
			case 24:	j->SetHunger( reply.toByte() );					break;	// Hunger
			case 25:	j->SetPoisonStrength( reply.toUByte() );		break;	// Poison
			case 26:	j->SetWeight( reply.toShort() );				break;	// Weight
			case 27:	j->SetCarve( reply.toShort() );					break;	// Carve
			case 28:	j->SetVisible( (VisibleTypes)reply.toByte() );	break;	// Visible
			case 29:	j->SetScriptTrigger( reply.toULong() );			break;	// ScriptTrigger ID
		}
		tweakCharMenu( tSock, j );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void HandleTownstoneText( CSocket *s, long index )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles new values for townstones
//o---------------------------------------------------------------------------o
void CPIGumpInput::HandleTownstoneText( UI08 index )
{
	CChar *mChar	= tSock->CurrcharObj();
	UI16 resourceID;
	UI32 amountToDonate;
	switch( index )
	{
		case 6:	// it's our donate resource button
		{
			CTownRegion *ourRegion = cwmWorldState->townRegions[mChar->GetTown()];
			amountToDonate = reply.toULong();
			if( amountToDonate == 0 )
			{
				tSock->sysmessage( 562 );
				return;
			}
			resourceID			= ourRegion->GetResourceID();
			UI32 numResources	= GetItemAmount( mChar, resourceID );

			if( amountToDonate > numResources )
				tSock->sysmessage( 563, numResources );
			else
			{
				DeleteItemAmount( mChar, amountToDonate, resourceID );
				ourRegion->DonateResource( tSock, amountToDonate );
			}
		}
		break;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool CPIGumpInput::Handle()
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles new values for gumps
//o---------------------------------------------------------------------------o
bool CPIGumpInput::Handle( void )
{
	switch( type )
	{
		case 1:		HandleTweakItemText( index );	break;
		case 2:		HandleTweakCharText( index );	break;
		case 3:		HandleTownstoneText( index );	break;
		case 20:
		{
			CChar *mChar = calcCharObjFromSer( id );
			if( ValidateObject( mChar ) )
			{
				CSocket *mSock = mChar->GetSocket();
				if( mSock != NULL )
				{
					UI32 scriptNo = mSock->AddID();
					if( scriptNo >= 0xFFFF )
					{
						cScript *toExecute = JSMapping->GetScript( (scriptNo - 0xFFFF) );
						if( toExecute != NULL )
							toExecute->HandleGumpInput( this );
						mSock->AddID( 0 );
					}
				}
			}
		}
		case 100:	GuildSys->GumpInput( this );			break;
		default:
			break;
	}
	return true;
}

UString GrabMenuData( UString sect, size_t entryNum, UString &tag )
{
	UString data;
	ScriptSection *sectionData = FileLookup->FindEntry( sect, menus_def );
	if( sectionData != NULL )
	{
		if( sectionData->NumEntries() >= entryNum )
		{
			tag		= sectionData->MoveTo( entryNum );
			data	= sectionData->GrabData();
		}
	}
	return data;
}

//o---------------------------------------------------------------------------o
//|		Function    :	bool CPIGumpChoice::Handle( void )
//|		Date        :	Unknown
//|		Programmer  :	UOX3 DevTeam
//|		Modified	:	Zane - 2/10/2003	
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles choice "Help" menu, Tracking Menu, and Polymorph menu
//o---------------------------------------------------------------------------o
bool CPIGumpChoice::Handle( void )
{
	UString sect;
	UString tag;
	UString data;
	UI16 main		= tSock->GetWord( 5 );
	UI16 sub		= tSock->GetWord( 7 );
	CChar *mChar	= tSock->CurrcharObj();

	if( main >= POLYMORPHMENUOFFSET )
	{
		sect = "POLYMORPHMENU " + UString::number( main );
		data = GrabMenuData( sect, static_cast<size_t>(sub *2), tag );
		if( !data.empty() )
		{
			if( main == POLYMORPHMENUOFFSET )
				Magic->PolymorphMenu( tSock, data.toUShort() );
			else
			{
                if( mChar->IsOnHorse() )
					DismountCreature( tSock->CurrcharObj() );		// can't be polymorphed on a horse
				Magic->Polymorph( tSock, data.toUShort() );
			}
		}
	}
	else if( main >= TRACKINGMENUOFFSET )
	{
		if( main == TRACKINGMENUOFFSET )
		{
			sect = "TRACKINGMENU " + UString::number( main );
			data = GrabMenuData( sect, static_cast<size_t>(sub * 2), tag );
			if( !data.empty() )
				Skills->CreateTrackingMenu( tSock, data.toUShort() );
		}
		else
		{
			if( !Skills->CheckSkill( mChar, TRACKING, 0, 1000 ) )
			{
				tSock->sysmessage( 575 );
				return true;
			}
			Skills->Tracking( tSock, static_cast<int>(sub - 1) );
		}
	}
	else if( main < ITEMMENUOFFSET ) // GM Menus
	{
		sect = "GMMENU " + UString::number( main );
		data = GrabMenuData( sect, static_cast<size_t>(sub * 2), tag );
		if( !tag.empty() )
			HandleGumpCommand( tSock, tag, data );
	}
	return true;
}

void HandleCommonGump( CSocket *mSock, ScriptSection *gumpScript, UI16 gumpIndex )
{
	CChar *mChar		= mSock->CurrcharObj();
	std::string line;
	UI16 modelID		= 0;
	UI16 modelColour	= 0;
	UString tag			= gumpScript->First(); 
	UString data		= gumpScript->GrabData();
	line				= tag + " " + data;
	CPOpenGump toSend( *mChar );
	toSend.GumpIndex( gumpIndex );
	toSend.Question( line );
	for( tag = gumpScript->Next(); !gumpScript->AtEnd(); tag = gumpScript->Next() )
	{
		data	= gumpScript->GrabData();
		modelID = tag.toUShort();
		toSend.AddResponse( modelID, modelColour, data );
		tag		= gumpScript->Next();
	}
	toSend.Finalize();
	mSock->Send( &toSend );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void GumpDisplay::AddData( GumpInfo *toAdd )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Adds the data to the gump to send
//o---------------------------------------------------------------------------o
void GumpDisplay::AddData( GumpInfo *toAdd )
{
	GumpInfo *gAdd		= new GumpInfo;
	gAdd->name			= toAdd->name;
	gAdd->value			= toAdd->value;
	gAdd->type			= toAdd->type;
	gAdd->stringValue	= toAdd->stringValue;
	gumpData.push_back( gAdd );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void GumpDisplay::AddData( std::string toAdd, long int value, UI08 type )
//|   Date        :  Unknown
//|   Programmer  :  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     :  Adds the data to the gump to send
//o---------------------------------------------------------------------------o
void GumpDisplay::AddData( std::string toAdd, long int value, UI08 type )
{
	if( toAdd.empty() )
		return;

	GumpInfo *gAdd		= new GumpInfo;
	gAdd->name			= toAdd;
	gAdd->stringValue	= "";
	gAdd->value			= value;
	gAdd->type			= type;
	gumpData.push_back( gAdd );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void GumpDisplay::AddData( std::string toAdd, std::string toSet, UI08 type )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Adds the data to the gump to send
//o---------------------------------------------------------------------------o
void GumpDisplay::AddData( std::string toAdd, std::string toSet, UI08 type )
{
	if( toAdd.empty() && type != 7 )
		return;

	GumpInfo *gAdd = new GumpInfo;
	gAdd->name = toAdd;
	gAdd->stringValue = toSet;
	gAdd->type = type;
	gumpData.push_back( gAdd );
}

//o---------------------------------------------------------------------------o
//|   Function    :  GumpDisplay::GumpDisplay( CSocket *target ) : toSendTo( target )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Begin GumpDisplay stuff by setting the target, clearing any existing data, and setting the w / h
//o---------------------------------------------------------------------------o
GumpDisplay::GumpDisplay( CSocket *target ) : toSendTo( target )
{
	gumpData.resize( 0 );
	width = 340;
	height = 320;
}

//o---------------------------------------------------------------------------o
//|   Function    :  GumpDisplay::GumpDisplay( CSocket *target, UI16 gumpWidth, UI16 gumpHeight ) : 
//|						width( gumpWidth ), height( gumpHeight ), toSendTo( target )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Begin GumpDisplay stuff by setting the target, clearing any existing data, and setting the w / h
//o---------------------------------------------------------------------------o
GumpDisplay::GumpDisplay( CSocket *target, UI16 gumpWidth, UI16 gumpHeight ) : 
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
	for( size_t i = 0; i < gumpData.size(); ++i )
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
//|   Function    :	 void GumpDisplay::SetTitle( const std::string newTitle )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Set a gumps title
//o---------------------------------------------------------------------------o
void GumpDisplay::SetTitle( const std::string& newTitle )
{
	title = newTitle;
}

//o---------------------------------------------------------------------------o
//|   Function    :	 void SendVecsAsGump( CSocket *sock, stringList& one, stringList& two, long type, SERIAL serial )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Sends to socket sock the data in one and two.  One is control, two is data
//o---------------------------------------------------------------------------o
void SendVecsAsGump( CSocket *sock, STRINGLIST& one, STRINGLIST& two, long type, SERIAL serial )
{
	CPSendGumpMenu toSend;
	toSend.GumpID( type );
	toSend.UserID( serial );

	size_t line = 0;
	for( line = 0; line < one.size(); ++line )
		toSend.AddCommand( one[line] );

	for( line = 0; line < two.size(); ++line )
		toSend.AddText( two[line] );

	toSend.Finalize();
	sock->Send( &toSend );
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
	size_t i;
	UI32 pagenum = 1, position = 40, linenum = 1, buttonnum = 7;
	UI08 numToPage = 10, stringWidth = 10;

	if( !one.empty() && !two.empty() )
	{
		SendVecsAsGump( toSendTo, one, two, gumpNum, serial );
		return;
	}
	UI08 ser1, ser2, ser3, ser4;
	//--static pages
	one.push_back( "noclose" );
	one.push_back( "page 0"  );
	sprintf( temp, "resizepic 0 0 %i %i %i", cwmWorldState->ServerData()->BackgroundPic(), width, height );
	one.push_back( temp );
	sprintf( temp, "button %i 15 %i %i 1 0 1", width - 40, cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1 );
	one.push_back( temp );
	sprintf( temp, "text 45 15 %i 0", cwmWorldState->ServerData()->TitleColour() );
	one.push_back( temp );

	sprintf( temp, "page %lu", pagenum );
	one.push_back( temp );

	if( title.length() == 0 )
		strcpy( temp, "General Gump" );
	else
		strcpy( temp, title.c_str() );
	two.push_back( temp );

	numToPage = (UI08)((( height - 30 ) / 20) - 2);
	stringWidth = (UI08)( ( width / 2 ) / 10 );
	UI32 lineForButton;
	for( i = 0, lineForButton = 0; i < gumpData.size(); ++i, ++lineForButton )
	{
		if( lineForButton > 0 && ( !( lineForButton % numToPage ) ) )
		{
			position = 40;
			++pagenum;
			sprintf( temp, "page %lu", pagenum );
			one.push_back( temp );
		}
		if( gumpData[i]->type != 7 )
		{
			sprintf( temp, "text 50 %lu %i %lu", position, cwmWorldState->ServerData()->LeftTextColour(), linenum++ ); 
			one.push_back( temp );
			if( isMenu )
			{
				sprintf( temp, "button 20 %lu %i %i 1 0 %lu", position, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, buttonnum );
				one.push_back( temp );
			}
			sprintf( temp, "text %i %lu %i %lu", (width / 2) + 10, position, cwmWorldState->ServerData()->RightTextColour(), linenum++ );
			one.push_back( temp );
			two.push_back( gumpData[i]->name );
		}
		else
		{
			sprintf( temp, "text 30 %lu %i %lu", position, cwmWorldState->ServerData()->LeftTextColour(), linenum++ ); 
			one.push_back( temp );
		}

		long value			= gumpData[i]->value;
		switch( gumpData[i]->type )
		{
			case 0:		// all numbers of sorts
				sprintf( temp, "%li", value );
				break;
			case 1:
				sprintf( temp, "%x", static_cast<UI32>(value) );
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
				if( gumpData[i]->stringValue.empty() )
					strcpy( temp, "NULL POINTER" );
				else
					strcpy( temp, gumpData[i]->stringValue.c_str() );
				if( strlen( temp ) > stringWidth )	// too wide for one line, CRAP!
				{
					char temp2[512], temp3[512];
					size_t tempWidth = strlen( temp ) - stringWidth;
					strncpy( temp2, temp, stringWidth );
					temp2[stringWidth] = 0;
					two.push_back( temp2 );
					for( UI32 tempCounter = 0; tempCounter < tempWidth / ( stringWidth * 2 ) + 1; ++tempCounter )
					{
						position += 20;
						++lineForButton;
						sprintf( temp3, "text %i %lu %i %lu", 30, position, cwmWorldState->ServerData()->RightTextColour(), linenum++ );
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
				if( gumpData[i]->stringValue.empty() )
					strcpy( temp, "NULL POINTER" );
				else
					strcpy( temp, gumpData[i]->stringValue.c_str() );
				int sWidth;
				sWidth = stringWidth * 2;
				if( strlen( temp ) > static_cast<size_t>(sWidth) )	// too wide for one line, CRAP!
				{
					char temp2[512], temp3[512];
					size_t tempWidth = strlen( temp ) - sWidth;
					strncpy( temp2, temp, sWidth );
					temp2[sWidth] = 0;
					two.push_back( temp2 );
					for( UI32 tempCounter = 0; tempCounter < tempWidth / sWidth + 1; ++tempCounter )
					{
						position += 20;
						++lineForButton;
						sprintf( temp3, "text %i %lu %i %lu", 30, position, cwmWorldState->ServerData()->LeftTextColour(), linenum++ );
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
			case 8:
				if( gumpData[i]->stringValue.empty() )
					strcpy( temp, "0.00" );
				else
					sprintf( temp, "%.2f", UString( gumpData[i]->stringValue ).toFloat() );
				break;
			default:
				sprintf( temp, "%li", value );
				break;
		}
		if( gumpData[i]->type != 4 && gumpData[i]->type != 7 )
			two.push_back( temp );
		position += 20;
		++buttonnum;
	}
	
	pagenum = 1; 
	for( i = 0; static_cast<UI32>(i) <= lineForButton; i += numToPage )
	{
		sprintf( temp, "page %lu", pagenum );
		one.push_back( temp );
		if( i >= 10 )
		{
			sprintf( temp, "button 10 %i %i %i 0 %lu", height - 40, cwmWorldState->ServerData()->ButtonLeft(), cwmWorldState->ServerData()->ButtonLeft() + 1, pagenum-1 ); //back button
			one.push_back( temp );
		}
		if( lineForButton > numToPage && static_cast<UI32>(( i + numToPage )) < lineForButton )
		{
			sprintf( temp, "button %i %i %i %i 0 %lu", width - 40, height - 40, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum+1 ); //forward button
			one.push_back( temp );
		}
		++pagenum;
	}		
	SendVecsAsGump( toSendTo, one, two, gumpNum, serial );
}

}
