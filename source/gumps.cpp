#include "uox3.h"
#include "CGump.h"
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
#include "StringUtility.hpp"
#include <algorithm>
#include "osunique.hpp"

using namespace std::string_literals;

void CollectGarbage( void );
std::string GetUptime( void );

//o------------------------------------------------------------------------------------------------o
//|	Function	-	TextEntryGump()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Open entry gump with specified dictionary message and max value length
//o------------------------------------------------------------------------------------------------o
void TextEntryGump( CSocket *s, SERIAL ser, UI08 type, UI08 index, SI16 maxlength, SI32 dictEntry )
{
	if( s == nullptr )
		return;

	std::string txt = Dictionary->GetEntry( dictEntry, s->Language() );
	if( txt.empty() )
	{
		Console.Error( "Invalid text in TextEntryGump()" );
		return;
	}

	auto temp = oldstrutil::format( "(%i chars max)", maxlength );
	CPGumpTextEntry toSend( txt, temp );
	toSend.Serial( ser );
	toSend.ParentId( type );
	toSend.ButtonId( index );
	toSend.Cancel( 1 );
	toSend.Style( 1 );
	toSend.Format( maxlength );
	s->Send( &toSend );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	BuildGumpFromScripts()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Pull gump info from misc.dfn
//o------------------------------------------------------------------------------------------------o
void BuildGumpFromScripts( CSocket *s, UI16 m )
{
	CPSendGumpMenu toSend;
	toSend.UserId( INVALIDSERIAL );

	std::string sect = std::string( "GUMPMENU " ) + oldstrutil::number( m );
	CScriptSection *gump = FileLookup->FindEntry( sect, misc_def );
	if( gump == nullptr )
		return;

	UI08 targType	= 0x12;
	std::string tag	= gump->First();
	if( oldstrutil::upper( oldstrutil::extractSection( tag, " ", 0, 0 )) == "TYPE" )
	{
		targType = oldstrutil::value<UI08>( oldstrutil::extractSection( tag, " ", 1, 1 ));
		tag = gump->Next();
	}
	for(; !gump->AtEnd(); tag = gump->Next() )
	{
		auto temp = oldstrutil::format( "%s %s", tag.c_str(), gump->GrabData().c_str() );
		toSend.addCommand( temp );
	}
	sect = std::string( "GUMPTEXT " ) + oldstrutil::number( m );
	gump = FileLookup->FindEntry( sect, misc_def );
	if( gump == nullptr )
		return;

	for( tag = gump->First(); !gump->AtEnd(); tag = gump->Next() )
	{
		toSend.addText( oldstrutil::format( "%s %s", tag.c_str(), gump->GrabData().c_str() ));
	}
	toSend.GumpId( targType );
	toSend.Finalize();
	s->Send( &toSend );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleAccountButton()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles the accounts gump button
//o------------------------------------------------------------------------------------------------o
void HandleAccountButton( CSocket *s, UI32 button, CChar *j )
{
	if( !ValidateObject( j ))
		return;

	CChar *mChar = s->CurrcharObj();
	CAccountBlock_st& actbTemp = Accounts->GetAccountById( j->GetAccount().wAccountIndex );
	if( actbTemp.wAccountIndex == AB_INVALID_ID )
		return;

	CSocket *targSocket = j->GetSocket();
	switch( button )
	{
		case 2:
			if( mChar->IsGM() && !j->IsNpc() )
			{
				s->SysMessage( 487 ); // You have banned that players account!
				if( targSocket != nullptr )
				{
					targSocket->SysMessage( 488 ); // You have been banned!
				}
				actbTemp.wFlags.set( AB_FLAGS_BANNED, true );
				if( targSocket != nullptr )
				{
					Network->Disconnect( targSocket );
				}
			}
			else
			{
				s->SysMessage( 489 ); // That player cannot be banned!
			}
			break;
		case 3:
			if( mChar->IsGM() && !j->IsNpc() )
			{
				s->SysMessage( 490 ); // You have given that player's account a time ban!
				if( targSocket != nullptr )
				{
					targSocket->SysMessage( 491 ); // You have been banned for 24 hours!
				}
				actbTemp.wFlags.set( AB_FLAGS_BANNED, true );
				actbTemp.wTimeBan = GetMinutesSinceEpoch() + static_cast<UI32>( 1440 );

				if( targSocket != nullptr )
				{
					Network->Disconnect( targSocket );
				}
			}
			else
			{
				s->SysMessage( 492 ); // That player cannot be banned!
			}
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			// (489 + 4) Wipe functionality doesn't exist.
			// (489 + 5) Locking functionality doesn't exist.
			// (489 + 6) Warning functionality doesn't exist.
			// (489 + 7) Enter the new ID number for the item in hex. (???)
			s->SysMessage( 489 + button );
			break;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleTownstoneButton()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles button pressed in townstone gump
//o------------------------------------------------------------------------------------------------o
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
			result = ourRegion->RemoveTownMember(( *mChar ));
			if( !result )
			{
				s->SysMessage( 540 ); // There was an error removing you from your town, contact a GM promptly!
			}
			break;
		case 3:		// view taxes
			targetRegion = CalcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber(), mChar->GetInstanceId() );
			if( targetRegion != nullptr )
			{
				targetRegion->ViewTaxes( s );
			}
			else
			{
				s->SysMessage( 541 ); //Can't view taxes for a town you are not in.
			}
			break;
		case 4:		// toggle town title
			mChar->SetTownTitle( !mChar->GetTownTitle() );
			ourRegion->DisplayTownMenu( nullptr, s );
			break;
		case 5:		s->SendTargetCursor( 0, TARGET_VOTEFORMAYOR, 0, 542 );								break;		// vote for town mayor
		case 6:		TextEntryGump(  s, ser, static_cast<UI08>( type ), static_cast<UI08>( button ), 6, 543 );	break;		// gold donation
		case 7:		ourRegion->ViewBudget( s );			break;		// View Budget
		case 8:		ourRegion->SendAlliedTowns( s );	break;		// View allied towns
		case 9:		ourRegion->SendEnemyTowns( s );		break;
		case 20:	// access mayoral functions, never hit here if we don't have mayoral access anyway!
			// also, we'll only get access, if we're in our OWN region
			ourRegion->DisplayTownMenu( nullptr, s, 0x01 );	// mayor
			break;
		case 21:	s->SysMessage( 544 );				break;	// Can't set the taxes yet!
		case 22:	ourRegion->DisplayTownMembers( s );	break;	// list town members
		case 23:	ourRegion->ForceEarlyElection();		break;	// force early election
		case 24:	s->SysMessage( 545 );	break;	// You cannot purchase more guards at this time.
		case 25:	s->SysMessage( 546 );	break;	// You can't fire a guard at the moment.
		case 26:	s->SendTargetCursor( 0, TARGET_TOWNALLY, 0, 547 );								break;	// make a town an ally
		case 40:	ourRegion->DisplayTownMenu( nullptr, s );	break;	// we can't return from mayor menu if we weren't mayor!
		case 41:	// join town!
			if( !( CalcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber(), mChar->GetInstanceId() )->AddAsTownMember(( *mChar ))))
			{
				s->SysMessage( 548 ); // You were unable to be added, contact a GM for further information.
			}
			break;
		case 61:	// seize townstone!
			if( !Skills->CheckSkill( mChar, STEALING, 950, 1000 )	) // minimum 95.0 stealing
			{
				targetRegion = CalcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber(), mChar->GetInstanceId() );
				if( targetRegion != nullptr )
				{
					SI32 chanceToSteal	= RandomNum( 0, targetRegion->GetHealth() / 2 );
					SI32 fudgedStealing	= RandomNum( mChar->GetSkill( STEALING ), static_cast<UI16>( mChar->GetSkill( STEALING ) * 2 ));
					if( fudgedStealing >= chanceToSteal )
					{
						// redo stealing code here
						s->SysMessage( 549, targetRegion->GetName().c_str() ); // Congrats go to you, for you have successfully dealt %s a nasty blow this day!
						targetRegion->DoDamage( targetRegion->GetHealth() / 2 );	// we reduce the region's health by half
						for( auto &toCheck : MapRegion->PopulateList( mChar ))
						{
							if( toCheck )
							{
								CItem *iTownStone = nullptr;
								auto regItems = toCheck->GetItemList();
								for( const auto &itemCheck : regItems->collection() )
								{
									if( ValidateObject( itemCheck ))
									{
										if( itemCheck->GetType() == IT_TOWNSTONE && itemCheck->GetId() != 0x14F0 )
										{
											// found our townstone
											iTownStone = itemCheck;
											break;
										}
									}
								}

								if( ValidateObject( iTownStone ))
								{
									auto charPack = mChar->GetPackItem();
									if( ValidateObject( charPack ))
									{
										iTownStone->SetCont( charPack );
										iTownStone->SetTempVar( CITV_MOREX, targetRegion->GetRegionNum() );
										s->SysMessage( 550 ); // Quick, make it back to your town with the stone to deal the death blow to this region!
										targetRegion->TellMembers( 551, mChar->GetName().c_str() ); // Quickly, %s has stolen your treasured townstone!
										return;	// dump out
									}
								}
							}
						}
					}
					else
					{
						s->SysMessage( 552 ); // Try as you might, you have not the ability to seize the day.
					}
				}
				else
				{
					s->SysMessage( 553 ); // This is not a valid region!
				}
			}
			else
			{
				s->SysMessage( 554 ); // You have insufficient ability to seize the townstone.
			}
			break;
		case 62:	// attack townstone
			targetRegion = CalcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber(), mChar->GetInstanceId() );
			for( UI08 counter = 0; counter < RandomNum( 5, 10 ); ++counter )
			{
				Effects->PlayMovingAnimation( mChar, mChar->GetX() + RandomNum( -6, 6 ), mChar->GetY() + RandomNum( -6, 6 ), mChar->GetZ(), 0x36E4, 17, 0, ( RandomNum( 0, 1 ) == 1 ));
				Effects->PlayStaticAnimation( mChar->GetX() + RandomNum( -6, 6 ), mChar->GetY() + RandomNum( -6, 6 ), mChar->GetZ(), 0x373A + RandomNum( 0, 4 ) * 0x10, 0x09, 0, 0 );
			}
			targetRegion->DoDamage( RandomNum( 0, targetRegion->GetHealth() / 8 ));	// we reduce the region's health by half
			break;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleAccountModButton()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles button pressed in account gump
//o------------------------------------------------------------------------------------------------o
void HandleAccountModButton( CPIGumpMenuSelect *packet )
{
	CSocket *s = packet->GetSocket();

	std::string username	= "";
	std::string password	= "";
	std::string emailAddy	= "";

	for( UI32 i = 0; i < packet->TextCount(); ++i )
	{
		UI16 textId = packet->GetTextId( i );
		switch( textId )
		{
			case 1000:	username	= packet->GetTextString( i );	break;
			case 1001:	password	= packet->GetTextString( i );	break;
			case 1002:	emailAddy	= packet->GetTextString( i );	break;
			default:
				Console.Warning( oldstrutil::format( "Unknown textId %i with string %s", textId, packet->GetTextString( i ).c_str() ));
		}
	}

	CAccountBlock_st& actbAccountFind = Accounts->GetAccountByName( username );
	if( actbAccountFind.wAccountIndex != AB_INVALID_ID )
	{
		s->SysMessage( 555 ); // An account by that name already exists!
		return;
	}
	Console.Print( oldstrutil::format( "Attempting to add username %s with password %s at emailaddy %s", username.c_str(), password.c_str(), emailAddy.c_str() ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function		-	BuildAddMenuGump()
//o------------------------------------------------------------------------------------------------o
//|	Purpose			-	Menu for item creation based on ITEMMENU entries in the ItemMenu DFNs
//|
//|	Modification	-	04042004 - Added support for the new Auto-AddMenu
//|									items if they exist. Remember the multimap is groupId based
//|									so it should order it accordingly we only have to look for
//|									the entries.
//o------------------------------------------------------------------------------------------------o
void BuildAddMenuGump( CSocket *s, UI16 m )
{
	UI32 pagenum	= 1, position = 40, linenum = 1, buttonnum = 7;
	UI08 i			= 0;
	CChar *mChar	= s->CurrcharObj();

	if( !mChar->IsGM() && m > 990 && m < 999 ) // 990 - 999 reserved for online help system
	{
		s->SysMessage( 337 ); // Unrecognized command.
		return;
	}

#if defined( UOX_DEBUG_MODE )
	Console << "Menu: " << m << myendl;
#endif

	CPSendGumpMenu toSend;
	toSend.UserId( INVALIDSERIAL );
	toSend.GumpId( 9 );

	////////////////////////////////////////////////
	s->TempInt( m );	// Store what menu they are in

	std::string sect = std::string( "ITEMMENU " ) + oldstrutil::number( m );
	CScriptSection *ItemMenu = FileLookup->FindEntry( sect, items_def );
	if( ItemMenu == nullptr )
		return;

	// page header
	toSend.addCommand( "nodispose" );
	toSend.addCommand( "page 0" );

	// Need more control over this aspect of uox3 from outside some how.. first step to get the variables more flexible. later we will emporer them to a template, or a script or something
	UI16 xStart = 0, xWidth = 680;
	UI16 yStart = 0, yWidth = 420;

	UI32 bgImage = cwmWorldState->ServerData()->BackgroundPic();

	// Set and resize the gumps background image.
	toSend.addCommand( oldstrutil::format( "resizepic %u %u %u %u %u", xStart, yStart, bgImage, xWidth, yWidth ));
	toSend.addCommand( oldstrutil::format( "checkertrans %u %u %u %u", xStart + 5, yStart + 5, xWidth - 10, yWidth - 11 ));

	// Grab the first tag/value pair from the gump itemmenu respectivly
	std::string tag		= ItemMenu->First();
	std::string data	= ItemMenu->GrabData();
	toSend.addCommand( oldstrutil::format( "resizepic %u %u %u %u %u", 2, 4, 0xDAC, 675, 40 ));
	// Create the text stuff for what appears to be the title of the gump. This appears to change from page to page.

	// Next we create and position the close window button as well set its Down, and Up states.
	toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", xWidth - 32, yStart + 10, 0xA51, 0xA50, 1, 0, 1 ));

	linenum = 0;
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 30, yStart + 13, 39, linenum++ ));
	toSend.addText( "Players: " );
	// Player count
	auto szBuffer = oldstrutil::format( "%4i ", cwmWorldState->GetPlayersOnline() );
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 80, yStart + 13, 25, linenum++ ));
	toSend.addText( szBuffer );
	// Gm Pages
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 118, yStart + 13, 39, linenum++ ));
	toSend.addText( "GM Pages: " );
	szBuffer = "0";
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 200, yStart + 13, 25, linenum++ ));
	toSend.addText( szBuffer );
	// Current Time/Date
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 230, yStart + 13, 39, linenum++ ));
	toSend.addText( "Time: " );
	// Current server time
	auto timet = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
	struct tm ttoday;
	auto today = lcltime( timet, ttoday );
	char tmpBuffer[200];
	bool isAM = true;
	strftime( tmpBuffer, 128, "%b %d, %Y", today );
	if( today-> tm_hour > 12 )
	{
		isAM = false;
		today->tm_hour -= 12;
	}
	else if( today->tm_hour == 0 )
	{
		isAM = true;
		today->tm_hour = 12;
	}
	// Draw the current date to the gump
	char tbuffer[100];
	szBuffer = oldstrutil::format( "%s %.8s%s", tmpBuffer, asciitime( tbuffer, 100, *today ) + 11, (( isAM ) ? "Am" : "Pm" ));
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 280, yStart + 13, 25, linenum++ ));
	toSend.addText( szBuffer );

	// add the next gump portion. New server level services, in the form of a gump Configuration, and Accounts tabs to start. These are default tabs
	toSend.addCommand( oldstrutil::format( "resizepic %u %u %u %u %u", xStart + 10, yStart + 62, 5054, 190, 340 ));
	UI32 tabNumber = 1;

	// Do the shard tab
	toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 17, yStart + 47, 0x138E, 0x138F, 0, 1, 0 ));
	szBuffer = "Objects";
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 42, yStart + 46, 47, linenum++ ));
	toSend.addText( szBuffer );
	// Do the server tab
	toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 105, yStart + 47, 0x138E, 0x138F, 0, 30, 1 ));
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 132, yStart + 46, 47, linenum++ ));
	toSend.addText( "Settings" );

	// Need a seperator
	toSend.addCommand( oldstrutil::format( "gumppictiled %u %u %u %u %u", xStart + 22, yWidth - 50, 165, 5, 0x2393 ));

	// Ok, now the job of pulling the rest of the first itemmenu information and making tabs for them
	toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 65, yWidth-40, 0x4B9, 0x4BA, 1, 0, 50002 ));
	toSend.addCommand( oldstrutil::format( "croppedtext %u %u %u %u %u %u", 85, yWidth-42, 150, 20, 94, linenum++ ));
	toSend.addText( "Home" );

	// Ok here we have some conditions that we need to filter. First being the menu called.
	UI32 xOffset;
	UI32 yOffset;
#define SXOFFSET	210
#define SYOFFSET	44
#define YOFFSET		110
#define XOFFSET		110
	if( m == 1 )
	{
		// Now we make the first page that will escentially display our list of Main GM Groups.
		pagenum = 2;
		toSend.addCommand( "page 1" );
		// Do the shard tab
		toSend.addCommand( oldstrutil::format( "gumppic %u %u %u ", 17, yStart + 47, 0x138F ));
		szBuffer = "Objects";
		toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 42, yStart + 46, 70, linenum++ ));
		toSend.addText( szBuffer );

		toSend.addCommand( oldstrutil::format( "htmlgump %i %i %i %i %u %i %i", 245, 70, 220, 30, linenum++, 0, 0 ));
		toSend.addText( "<CENTER><BIG><BASEFONT color=#F64040>Ultima Offline eXperiment 3</BASEFONT></BIG></CENTER>" );

		// Shard Menu Version
		szBuffer = "v" + CVersionClass::GetVersion() + "." + CVersionClass::GetBuild();
		toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 320, 90, 27, linenum++ ));
		toSend.addText( szBuffer );

		// Shard Menu Description
		szBuffer = "Greetings! Using these menus you can";
		toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 225, 120, 94, linenum++ ));
		toSend.addText( szBuffer );

		szBuffer = "quickly and easily add Items, Spawners,";
		toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 225, 135, 94, linenum++ ));
		toSend.addText( szBuffer );

		szBuffer = "NPCs and more to your shard, in order";
		toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 225, 150, 94, linenum++ ));
		toSend.addText( szBuffer );

		szBuffer = "to customize it to your needs.";
		toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 225, 165, 94, linenum++ ));
		toSend.addText( szBuffer );

		szBuffer = "Objects tab is used to find stuff to";
		toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 225, 190, 94, linenum++ ));
		toSend.addText( szBuffer );

		szBuffer = "add. Use Settings tab to configure";
		toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 225, 205, 94, linenum++ ));
		toSend.addText( szBuffer );

		szBuffer = "various options for this menu!";
		toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 225, 220, 94, linenum++ ));
		toSend.addText( szBuffer );

		szBuffer = "Don't forget to check out the UOX3 Docs!";
		toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 225, 260, 27, linenum++ ));
		toSend.addText( szBuffer );

		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 295, 295, 0x5d1, 0x5d3, 1, 0, 50020 ));

		// here we hunt tags to make sure that we get them all from the itemmenus etc.
		UI08 numCounter = 0;
		position		= 80;
		xOffset			= SXOFFSET;
		yOffset			= SYOFFSET;
		for( tag = ItemMenu->Next(); !ItemMenu->AtEnd(); tag = ItemMenu->Next() )
		{
			data = ItemMenu->GrabData();
			if( numCounter > 0 && ( !( numCounter % 12 )))
			{
				position	= 80;
				toSend.addCommand( oldstrutil::format( "page %i", ++pagenum ));
				xOffset		= SXOFFSET;
				yOffset		= SYOFFSET;
			}
			// Drop in the page number text area image
			toSend.addCommand( oldstrutil::format( "gumppic %u %u %u", xStart + 300, yWidth - 28, 0x98E ));
			// Add the page number text to the text area for display
			toSend.addCommand( oldstrutil::format( "text %u %u %u %u", xStart + 335, yWidth - 27, 39, linenum++ ));
			szBuffer = oldstrutil::format( "Menu %i - Page %i", m, pagenum - 1 );
			toSend.addText( szBuffer );
			// Spin the tagged items loaded in from the dfn files.
			toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 15, position, 0x4B9, 0x4BA, 1, 0, buttonnum));
			toSend.addCommand( oldstrutil::format( "croppedtext %u %u %u %u %u %u", 35, position-3, 150, 20, 50, linenum++ ));
			toSend.addText( data );
			if( tag.data()[0] != '<' && tag.data()[0] != ' ' )	// it actually has a picture, well bugger me! :>
			{
				// Draw a frame for the item to make it stand out a touch more.
				toSend.addCommand( oldstrutil::format( "resizepic %u %u %u %u %u", xOffset, yOffset, 0x53, 65, 100 ));
				toSend.addCommand( oldstrutil::format( "checkertrans %u %u %u %u", xOffset + 7, yOffset + 9, 52, 82 ));
				toSend.addCommand( oldstrutil::format( "tilepic %u %u %i", xOffset + 5, yOffset + 10, std::stoi( tag, nullptr, 0 )));
				toSend.addCommand( oldstrutil::format( "croppedtext %u %u %u %u %u %u", xOffset, yOffset+65, 65, 20, 55, linenum++ ));
				toSend.addText( data );
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
			toSend.addCommand( oldstrutil::format( "page %i", currentPage ));
			if( i >= 10 )
			{
				toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u", xStart + 240, yWidth - 25, 0x25EB, 0x25EA, 0, currentPage - 1 ));
			}
			if(( numCounter > 12 ) && (( i + 12 ) < numCounter ))
			{
				toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u", xWidth - 60, yWidth - 25, 0x25E7, 0x25E6, 0, currentPage + 1 ));
			}
			currentPage++;
		}
	}
	else	// m != 1
	{
		// Now we make the first page that will essentially display our list of Main GM Groups.
		pagenum = 2;
		toSend.addCommand( "page 1" );
		// Do the shard tab
		toSend.addCommand( oldstrutil::format( "gumppic %u %u %u ", 17, yStart + 47, 0x138F ));
		szBuffer = "Shard";
		toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 42, yStart + 46, 39, linenum++ ));
		toSend.addText( szBuffer );
		// here we hunt tags to make sure that we get them all from the itemmenus etc.
		UI08 numCounter = 0;
		position		= 80;
		xOffset			= SXOFFSET;
		yOffset			= SYOFFSET;

		// Drop in the page number text area image
		toSend.addCommand( oldstrutil::format( "gumppic %u %u %u", xStart + 300, yWidth - 28, 0x98E ));
		// Add the page number text to the text area for display
		toSend.addCommand( oldstrutil::format( "text %u %u %u %u", xStart + 335, yWidth - 27, 39, linenum++ ));
		szBuffer = oldstrutil::format( "Menu %i - Page %i", m, pagenum - 1 );
		toSend.addText( szBuffer );

		for( tag = ItemMenu->Next(); !ItemMenu->AtEnd(); tag = ItemMenu->Next() )
		{
			data = ItemMenu->GrabData();

			if( numCounter > 0 && (!(numCounter % 12 )))
			{
				position = 80;
				toSend.addCommand( oldstrutil::format( "page %i", pagenum++ ));
				xOffset=SXOFFSET;
				yOffset=SYOFFSET;

				// Drop in the page number text area image
				toSend.addCommand( oldstrutil::format( "gumppic %u %u %u", xStart + 300, yWidth - 28, 0x98E ));
				// Add the page number text to the text area for display
				toSend.addCommand( oldstrutil::format( "text %u %u %u %u", xStart + 335, yWidth - 27, 39, linenum++ ));
				szBuffer = oldstrutil::format( "Menu %i - Page %i", m, pagenum - 1 );
				toSend.addText( szBuffer );
			}
			// Drop in the page number text area image
			if( oldstrutil::upper( tag ) == "INSERTADDMENUITEMS" )
			{
				// Check to see if the desired menu has any items to add
				if( g_mmapAddMenuMap.find( m ) == g_mmapAddMenuMap.end() )
				{
					continue;
				}
				// m contains the groupId that we need to do fetch the auto-addmenu items
				std::pair<ADDMENUMAP_CITERATOR, ADDMENUMAP_CITERATOR> pairRange = g_mmapAddMenuMap.equal_range( m );
				for( ADDMENUMAP_CITERATOR CI = pairRange.first; CI!=pairRange.second; CI++ )
				{
					toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 15, position, 0x4B9, 0x4BA , 1, 0, buttonnum ));
					toSend.addCommand( oldstrutil::format( "croppedtext %u %u %u %u %u %u", 35, position - 3, 150, 20, 40, linenum) );
					toSend.addText( CI->second.itemName );
					// check to make sure that we have an image now, seeing as we might not have one with the new changes in 0.98.01.2+
					if(CI->second.tileId != 0 )
					{
						// Draw a frame for the item to make it stand out a touch more.
						toSend.addCommand( oldstrutil::format( "resizepic %u %u %u %u %u", xOffset, yOffset, 0x53, 65, 100 ));
						toSend.addCommand( oldstrutil::format( "checkertrans %u %u %u %u", xOffset + 7, yOffset + 9, 52, 82 ));
						toSend.addCommand( oldstrutil::format( "tilepic %u %u %i", xOffset + 5, yOffset + 10, CI->second.tileId ));
						toSend.addCommand( oldstrutil::format( "croppedtext %u %u %u %u %u %u", xOffset, yOffset + 65, 65, 20, 55, linenum++ ));
						toSend.addText( CI->second.itemName );
						toSend.addText( CI->second.itemName.c_str() );
						xOffset += XOFFSET;
						if( xOffset > 480 )
						{
							xOffset = SXOFFSET;
							yOffset += YOFFSET;
						}
					}
					position += 20;
					++linenum;
					++buttonnum;
					++numCounter;
				}
				continue;
			}
			toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 15, position, 0x4B9, 0x4BA, 1, 0, buttonnum ));
			toSend.addCommand( oldstrutil::format( "croppedtext %u %u %u %u %u %u", 35, position - 3, 150, 20, 50, linenum++ ));
			toSend.addText( data );
			if( tag.data()[0] != '<' && tag.data()[0] != ' ' )	// it actually has a picture, well bugger me! :>
			{
				// Draw a frame for the item to make it stand out a touch more.
				if( s->ClientVerShort() <= CVS_70160 )
                {
                    // Fallback for older clients that don't support buttontileart
                    toSend.addCommand( oldstrutil::format("resizepic %u %u %u %u %u", xOffset, yOffset, 0x53, 65, 100 ));
                    toSend.addCommand( oldstrutil::format("checkertrans %u %u %u %u", xOffset + 7, yOffset + 9, 52, 82 ));
                    toSend.addCommand( oldstrutil::format("tilepic %u %u %i", xOffset + 5, yOffset + 10, std::stoi(tag, nullptr, 0) ));
                    toSend.addCommand( oldstrutil::format("croppedtext %u %u %u %u %u %u", xOffset, yOffset + 65, 65, 20, 55, linenum++ ));
                }
                else
                {
                    toSend.addCommand( oldstrutil::format( "checkertrans %u %u %u %u", xOffset + 7, yOffset + 9, 110, 110 ));
                    toSend.addCommand( oldstrutil::format( "buttontileart %u %u 0x0a9f 0x0aa1 %u %u %u %u %u %u %u", xOffset, yOffset, 1, 0, buttonnum, std::stoi( tag, nullptr, 0 ), 0, 25, 25 ));
                    toSend.addCommand( oldstrutil::format( "tooltip 1042971 @%s@", data.c_str() ));
                    toSend.addCommand( oldstrutil::format( "croppedtext %u %u %u %u %u %u", xOffset + 15, yOffset + 85, 100, 20, 50, linenum++ ));
                }
				toSend.addText( data );
				xOffset += XOFFSET;
				if( xOffset > 640 )
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
			toSend.addCommand( oldstrutil::format( "page %i", currentPage) );
			if( i >= 10 )
			{
				toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u", xStart + 240, yWidth - 25, 0x25EB, 0x25EA, 0, currentPage - 1 )); //back button
			}
			if(( numCounter > 12 ) && (( i + 12 ) < numCounter ))
			{
				toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u", xWidth - 60, yWidth - 25, 0x25E7, 0x25E6, 0, currentPage + 1 ));
			}
			++currentPage;
		}
	}


	// Reserved page # 800 - 899 for the server support pages
	toSend.addCommand( "page 30" );
	// Show the selected tab image for this page.
	toSend.addCommand( oldstrutil::format( "gumppic %u %u %u", 105, yStart + 47, 0x138F ));
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 132, yStart + 46, 70, linenum++ ));
	toSend.addText( "Settings" );

	// Create the Quick Access Menu
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 57, yStart + 76, 52, linenum++ ));
	toSend.addText( "Quick-Access" );

	// Need a seperator
	toSend.addCommand( oldstrutil::format( "gumppictiled %u %u %u %u %u", xStart + 21, yStart + 105, 165, 5, 0x2393 ));

	// Create the Commandlist button
	toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 45, yStart + 125, 0x2A30, 0x2A44, 1, 4, 50008 ));
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 65, yStart + 129, 52, linenum++ ));
	toSend.addText( "Commandlist" );

	// Create the Wholist Online button
	toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 45, yStart + 155, 0x2A30, 0x2A44, 1, 4, 50009 ));
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 62, yStart + 159, 52, linenum++ ));
	toSend.addText( "Who is Online" );

	// Create the Wholist Offline button
	toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 45, yStart + 185, 0x2A30, 0x2A44, 1, 4, 50010 ));
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 60, yStart + 189, 52, linenum++ ));
	toSend.addText( "Who is Offline" );

	// Create the Reload DFNs button
	toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 45, yStart + 215, 0x2A30, 0x2A44, 1, 4, 50011 ));
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 60, yStart + 219, 52, linenum++ ));
	toSend.addText( "Reload DFNs" );

	// Create the Server Shutdown button
	if( mChar->GetCommandLevel() >= CL_ADMIN )
	{
		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 45, yStart + 275, 0x2A58, 0x2A44, 1, 4, 50012 ));
		toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 73, yStart + 279, 52, linenum++ ));
		toSend.addText( "Shutdown" );
	}

	// Settings Backgrounds
	toSend.addCommand( oldstrutil::format( "resizepic %u %u %u %u %u", 210, 55, 5120, 250, 150 ));
	toSend.addCommand( oldstrutil::format( "resizepic %u %u %u %u %u", 210, 205, 5120, 250, 150 ));

	// Settings Header 1
	toSend.addCommand( oldstrutil::format( "htmlgump %i %i %i %i %u %i %i", 235, 60, 200, 60, linenum++, 0, 0 ));
	toSend.addText( "<CENTER><BIG><BASEFONT color=#EECD8B>Menu Settings</BASEFONT></BIG></CENTER>" );

	// Settings Header 2
	toSend.addCommand( oldstrutil::format( "htmlgump %i %i %i %i %u %i %i", 235, 220, 200, 60, linenum++, 0, 0 ));
	toSend.addText( "<CENTER><BIG><BASEFONT color=#EECD8B>Item Settings</BASEFONT></BIG></CENTER>" );

	// Settings Options START
	// Add at Location
	TAGMAPOBJECT addAtLoc = mChar->GetTag( "addAtLoc" );
	if( addAtLoc.m_IntValue == 1 )
	{
		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 225, 80, 0x869, 0x867, 1, 0, 50000 ));
	}
	else
	{
		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 225, 80, 0x867, 0x869, 1, 0, 50000 ));
	}
	szBuffer = "Add item at specific location";
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 255, 80, 94, linenum++ ));
	toSend.addText( szBuffer );

	szBuffer = "instead of in GM's backpack";
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 255, 95, 94, linenum++ ));
	toSend.addText( szBuffer );

	// Repeat Add Object
	TAGMAPOBJECT repeatAdd = mChar->GetTag( "repeatAdd" );
	if( repeatAdd.m_IntValue == 1 )
	{
		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 250, 120, 0x869, 0x867, 1, 0, 50001 ));
	}
	else
	{
		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 250, 120, 0x867, 0x869, 1, 0, 50001 ));
	}
	szBuffer = "Add item repeatedly";
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 280, 120, 94, linenum++ ));
	toSend.addText( szBuffer );

	szBuffer = "until cancelled";
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 280, 135, 94, linenum++ ));
	toSend.addText( szBuffer );

	// Auto-reopen Menu
	TAGMAPOBJECT reopenMenu = mChar->GetTag( "reopenMenu" );
	if( reopenMenu.m_IntValue == 1 )
	{
		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 225, 160, 0x869, 0x867, 1, 0, 50003 ));
	}
	else
	{
		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 225, 160, 0x867, 0x869, 1, 0, 50003 ));
	}
	szBuffer = "Automatically reopen menu";
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 255, 160, 94, linenum++ ));
	toSend.addText( szBuffer );

	szBuffer = "after selecting object to add";
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 255, 175, 94, linenum++ ));
	toSend.addText( szBuffer );

	// Force-Decayable Off
	TAGMAPOBJECT forceDecayOff = mChar->GetTag( "forceDecayOff" );
	if( forceDecayOff.m_IntValue == 1 )
	{
		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 240, 260, 0x16ca, 0x16cb, 1, 0, 50004 ));
	}
	else
	{
		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 240, 260, 0x16c6, 0x16c7, 1, 0, 50004 ));
	}
	// Force-Decayable On
	TAGMAPOBJECT forceDecayOn = mChar->GetTag( "forceDecayOn" );
	if( forceDecayOn.m_IntValue == 1 )
	{
		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 350, 260, 0x16c4, 0x16c5, 1, 0, 50005 ));
	}
	else
	{
		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 350, 260, 0x16c0, 0x16c1, 1, 0, 50005 ));
	}
	szBuffer = "Default Decayable Status of Items";
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 230, 240, 94, linenum++ ));
	toSend.addText( szBuffer );
	szBuffer = "Force OFF";
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 275, 265, 27, linenum++ ));
	toSend.addText( szBuffer );
	szBuffer = "Force ON";
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 385, 265, 72, linenum++ ));
	toSend.addText( szBuffer );

	// Force-Movable Off
	TAGMAPOBJECT forceMovableOff = mChar->GetTag( "forceMovableOff" );
	if( forceMovableOff.m_IntValue == 1 )
	{
		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 240, 320, 0x16ca, 0x16cb, 1, 0, 50006 ));
	}
	else
	{
		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 240, 320, 0x16c6, 0x16c7, 1, 0, 50006 ));
	}
	// Force-Movable On
	TAGMAPOBJECT forceMovableOn = mChar->GetTag( "forceMovableOn" );
	if( forceMovableOn.m_IntValue == 1 )
	{
		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 350, 320, 0x16c4, 0x16c5, 1, 0, 50007 ));
	}
	else
	{
		toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 350, 320, 0x16c0, 0x16c1, 1, 0, 50007 ));
	}
	szBuffer = "Default Movable State of Items";
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 230, 300, 94, linenum++ ));
	toSend.addText( szBuffer );
	szBuffer = "Force OFF";
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 275, 325, 27, linenum++ ));
	toSend.addText( szBuffer );
	szBuffer = "Force ON";
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 385, 325, 72, linenum++ ));
	toSend.addText( szBuffer );
	// Settings Options END

	// Reserved pages 900-999 for the online help system. (comming soon)
	toSend.addCommand( "page 31" );
	// Ok display the scroll that we use to display our help information
	toSend.addCommand( oldstrutil::format( "resizepic %u %u %u %u %u", xStart + 205, yStart + 62, 0x1432, 175, 200 ));
	// Write out what page were on (Mainly for debug purposes
	szBuffer = oldstrutil::format( "%5u", 31 );
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", xWidth - 58, yWidth - 25, 110, linenum++ ));
	toSend.addText( szBuffer );
	// Ok, now the job of pulling the rest of the first itemmenu information and making tabs for them
	szBuffer = "Page 31";
	toSend.addCommand( oldstrutil::format( "text %u %u %u %u", 30, yStart + 200, 87, linenum++ ));
	toSend.addText( szBuffer );
	toSend.addCommand( oldstrutil::format( "button %u %u %u %u %u %u %u", 104, yStart + 300, 0x138E, 0x138E, 0, 1, tabNumber++ ));

#if defined( UOX_DEBUG_MODE )
	Console << "==============================" << myendl;
#endif
	// Finish up and send the gump to the client socket.
	toSend.Finalize();
	s->Send( &toSend );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIHelpRequest::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens the "Help" Menu from the paperdoll
//o------------------------------------------------------------------------------------------------o
bool CPIHelpRequest::Handle( void )
{
	UI16 gmnumber	= 0;
	CChar *mChar	= tSock->CurrcharObj();

	std::vector<UI16> scriptTriggers = mChar->GetScriptTriggers();
	for( auto scriptTrig : scriptTriggers )
	{
		cScript *toExecute = JSMapping->GetScript( scriptTrig );
		if( toExecute != nullptr )
		{
			if( toExecute->OnHelpButton( mChar ) == 0 )
			{
				return true;
			}
		}
	}

	// No individual scripts handling OnHelpButton returned true - let's check global script!
	cScript *toExecute = JSMapping->GetScript( static_cast<UI16>( 0 ));
	if( toExecute != nullptr )
	{
		if( toExecute->OnHelpButton( mChar ) == 0 )
		{
			return true;
		}
	}

	std::string sect = std::string( "GMMENU " ) + oldstrutil::number( menuNum );
	CScriptSection *GMMenu = FileLookup->FindEntry( sect, menus_def );
	if( GMMenu == nullptr )
		return true;

	std::string line;
	CPOpenGump toSend( *mChar );
	toSend.GumpIndex( menuNum );
	std::string tag		= GMMenu->First();
	std::string data	= GMMenu->GrabData();
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

//o------------------------------------------------------------------------------------------------o
//|   Function	  -  CPage()
//o------------------------------------------------------------------------------------------------o
//|   Purpose     -  Used when player pages a counselor
//o------------------------------------------------------------------------------------------------o
void CPage( CSocket *s, const std::string& reason )
{
	CChar *mChar = s->CurrcharObj();
	UI08 a1 = mChar->GetSerial( 1 );
	UI08 a2 = mChar->GetSerial( 2 );
	UI08 a3 = mChar->GetSerial( 3 );
	UI08 a4 = mChar->GetSerial( 4 );

	CHelpRequest pageToAdd;
	pageToAdd.Reason( reason );
	pageToAdd.WhoPaging( mChar->GetSerial() );
	pageToAdd.IsHandled( false );
	pageToAdd.TimeOfPage( time( nullptr ));

	SERIAL callNum = CounselorQueue->Add( &pageToAdd );
	if( callNum != INVALIDSERIAL )
	{
		mChar->SetPlayerCallNum( callNum );
		if( reason == "OTHER" )
		{
			mChar->SetSpeechMode( 2 );
			s->SysMessage( 359 ); // Please enter the reason for your Counselor request.
		}
		else
		{
			bool x = false;

			{
				for( auto &iSock : Network->connClients )
				{
					CChar *iChar = iSock->CurrcharObj();
					if( iChar->IsGMPageable() || iChar->IsCounselor() )
					{
						x = true;
						iSock->SysMessage( oldstrutil::format( 1024, "Counselor Page from %s [%x %x %x %x]: %s", mChar->GetName().c_str(), a1, a2, a3, a4, reason.c_str() ));
					}
				}
			}
			if( x )
			{
				s->SysMessage( 360 ); // Your request for info has been logged, and will be handled by a Counselor as soon as possible!
			}
			else
			{
				s->SysMessage( 361 ); // Your request for info has been logged, and will be handled by a Counselor as soon as possible!
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|   Function	  :  GMPage()
//o------------------------------------------------------------------------------------------------o
//|   Purpose     :  Used when player calls a GM
//o------------------------------------------------------------------------------------------------o
void GMPage( CSocket *s, const std::string& reason )
{
	CChar *mChar = s->CurrcharObj();
	UI08 a1 = mChar->GetSerial( 1 );
	UI08 a2 = mChar->GetSerial( 2 );
	UI08 a3 = mChar->GetSerial( 3 );
	UI08 a4 = mChar->GetSerial( 4 );

	CHelpRequest pageToAdd;
	pageToAdd.Reason( reason );
	pageToAdd.WhoPaging( mChar->GetSerial() );
	pageToAdd.IsHandled( false );
	pageToAdd.TimeOfPage( time( nullptr ));
	SERIAL callNum = GMQueue->Add( &pageToAdd );
	if( callNum != INVALIDSERIAL )
	{
		mChar->SetPlayerCallNum( callNum );
		if( reason == "OTHER" )
		{
			mChar->SetSpeechMode( 1 );
			s->SysMessage( 362 ); // Please enter the reason for your GM request.
		}
		else
		{
			bool x = false;
			{
				for( auto &iSock : Network->connClients )
				{
					CChar *iChar = iSock->CurrcharObj();
					if( ValidateObject( iChar ))
					{
						if( iChar->IsGMPageable() )
						{
							x = true;
							iSock->SysMessage( oldstrutil::format( 1024, "Page from %s [%x %x %x %x]: %s", mChar->GetName().c_str(), a1, a2, a3, a4, reason.c_str() ));
						}
					}
				}
			}
			if( x )
			{
				s->SysMessage( 363 ); // Your request for assistance has been logged, and will be handled by a GM as soon as possible!
			}
			else
			{
				s->SysMessage( 364 ); // Your request for assistance has been logged, and will be handled by a GM as soon as possible!
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleGumpCommand()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Execute a command from scripts
//o------------------------------------------------------------------------------------------------o
void HandleGumpCommand( CSocket *s, std::string cmd, std::string data )
{
	if( !s || cmd.empty() )
		return;

	CChar *mChar = s->CurrcharObj();

	cmd		= oldstrutil::upper( cmd );
	data	= oldstrutil::upper( data );
	std::string builtString;

	switch( cmd.data()[0] )
	{
		case 'A':
			if( !data.empty() )
			{
				bool runCommand = false;
				ObjectType itemType = OT_ITEM;
				if( cmd == "ADDITEM" )
				{
					runCommand = true;
				}
				else if( cmd == "ADDSPAWNER" )
				{
					runCommand = true;
					itemType = OT_SPAWNER;
				}
				TAGMAPOBJECT reopenMenu = mChar->GetTag( "reopenMenu" );
				if( runCommand )
				{
					TAGMAPOBJECT addAtLoc = mChar->GetTag( "addAtLoc" );
					TAGMAPOBJECT repeatAdd = mChar->GetTag( "repeatAdd" );
					TAGMAPOBJECT forceDecayOn = mChar->GetTag( "forceDecayOn" );
					TAGMAPOBJECT forceDecayOff = mChar->GetTag( "forceDecayOff" );
					TAGMAPOBJECT forceMovableOn = mChar->GetTag( "forceMovableOn" );
					TAGMAPOBJECT forceMovableOff = mChar->GetTag( "forceMovableOff" );
					auto secs = oldstrutil::sections( data, "," );
					if( secs.size() > 1 )
					{
						std::string tmp		= oldstrutil::trim( oldstrutil::removeTrailing( secs[0], "//" ));
						UI16 num			= oldstrutil::value<UI16>( oldstrutil::trim( oldstrutil::removeTrailing( secs[1], "//" )));

						if( addAtLoc.m_IntValue == 1 )
						{
							std::string addCmd = "";
							if( repeatAdd.m_IntValue == 1 )
							{
								if( itemType == OT_SPAWNER )
								{
									addCmd = oldstrutil::format( "raddspawner %s", tmp.c_str() );
								}
								else
								{
									addCmd = oldstrutil::format( "radditem %s", tmp.c_str() );
								}
							}
							else
							{
								if( itemType == OT_SPAWNER )
								{
									addCmd = oldstrutil::format( "add spawner %s", tmp.c_str() );
								}
								else
								{
									addCmd = oldstrutil::format( "add item %s", tmp.c_str() );
								}
							}
							Commands->Command( s, mChar, addCmd );
						}
						else if( repeatAdd.m_IntValue == 1 )
						{
							std::string addCmd = "";
							if( itemType == OT_SPAWNER )
							{
								addCmd = oldstrutil::format( "raddspawner %s", tmp.c_str() );
							}
							else
							{
								addCmd = oldstrutil::format( "radditem %s", tmp.c_str() );
							}
							Commands->Command( s, mChar, addCmd );
						}
						else
						{
							CItem *newItem = Items->CreateScriptItem( s, mChar, tmp, num, itemType, true );
							if( ValidateObject( newItem ))
							{
								if( forceDecayOff.m_IntValue == 1 )
								{
									newItem->SetDecayable( false );
								}
								else if( forceDecayOn.m_IntValue == 1 )
								{
									newItem->SetDecayable( true );
								}
								if( forceMovableOff.m_IntValue == 1 )
								{
									newItem->SetMovable( 2 );
								}
								else if( forceMovableOn.m_IntValue == 1 )
								{
									newItem->SetMovable( 1 );
								}
							}
							else if( !ValidateObject( mChar->GetItemAtLayer( IL_PACKITEM )))
							{
								s->SysMessage( 9150 ); // No backpack detected, unable to add item! Try the 'addpack command?
							}
						}
					}
					else
					{
						if( addAtLoc.m_IntValue == 1 )
						{
							std::string addCmd = "";
							if( repeatAdd.m_IntValue == 1 )
							{
								if( itemType == OT_SPAWNER )
								{
									addCmd = oldstrutil::format( "raddspawner %s", data.c_str() );
								}
								else
								{
									addCmd = oldstrutil::format( "radditem %s", data.c_str() );
								}
							}
							else
							{
								if( itemType == OT_SPAWNER )
								{
									addCmd = oldstrutil::format( "add spawner %s", data.c_str() );
								}
								else
								{
									addCmd = oldstrutil::format( "add item %s", data.c_str() );
								}
							}
							Commands->Command( s, mChar, addCmd );
						}
						else if( repeatAdd.m_IntValue == 1 )
						{
							std::string addCmd = "";
							if( itemType == OT_SPAWNER )
							{
								addCmd = oldstrutil::format( "raddspawner %s", data.c_str() );
							}
							else
							{
								addCmd = oldstrutil::format( "radditem %s", data.c_str() );
							}
							Commands->Command( s, mChar, addCmd );
						}
						else
						{
							CItem *newItem = Items->CreateScriptItem( s, mChar, data, 0, itemType, true );
							if( ValidateObject( newItem ))
							{
								if( forceDecayOff.m_IntValue == 1 )
								{
									newItem->SetDecayable( false );
								}
								else if( forceDecayOn.m_IntValue == 1 )
								{
									newItem->SetDecayable( true );
								}
								if( forceMovableOff.m_IntValue == 1 )
								{
									newItem->SetMovable( 2 );
								}
								else if( forceMovableOn.m_IntValue == 1 )
								{
									newItem->SetMovable( 1 );
								}
							}
							else if( !ValidateObject( mChar->GetItemAtLayer( IL_PACKITEM )))
							{
								s->SysMessage( 9150 ); // No backpack detected, unable to add item! Try the 'addpack command?
							}
						}
					}
				}
				if( reopenMenu.m_IntValue == 1 )
				{
					std::string menuString = oldstrutil::format( "itemmenu %d", s->TempInt() );
					Commands->Command( s, mChar, menuString );
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

				CPIHelpRequest toHandle( s, static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
				toHandle.Handle();
			}
			else if( cmd == "GMPAGE" )
			{
				if( data.empty() )
					return;

				GMPage( s, data );
			}
			else if( cmd == "GCOLLECT" )
			{
				CollectGarbage();
			}
			else if( cmd == "GOPLACE" )
			{
				if( data.empty() )
					return;

				SI08 targetWorld = -1;
				auto secs = oldstrutil::sections( data, "," );
				if( secs.size() > 1 )
				{
					// See if a target world override was specified in travel menu (allows using same location entries in locations.dfn for Fel/Trammel, for instance)
					targetWorld = oldstrutil::value<SI08>( oldstrutil::trim( oldstrutil::removeTrailing( secs[1], "//" )));
				}

				UI16 placeNum = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
				if( cwmWorldState->goPlaces.find( placeNum ) != cwmWorldState->goPlaces.end() )
				{
					GoPlaces_st toGoTo = cwmWorldState->goPlaces[placeNum];

					auto oldWorld = mChar->WorldNumber();
					targetWorld = targetWorld != -1 ? targetWorld : static_cast<SI08>( toGoTo.worldNum );
					mChar->SetLocation( toGoTo.x, toGoTo.y, toGoTo.z, static_cast<UI08>( targetWorld ), mChar->GetInstanceId() );

					if( static_cast<UI08>( targetWorld ) != oldWorld )
					{
						// Change map!
						SendMapChange( targetWorld, s );
					}

					// Additional update required for regular UO client
					mChar->Update();
				}
			}
			else if( cmd == "GUIINFORMATION" )
			{
				CGumpDisplay guiInfo( s, 400, 300 );
				guiInfo.SetTitle( CVersionClass::GetProductName() + " Status" );
				builtString = GetUptime();
				guiInfo.AddData( "Version", CVersionClass::GetVersion() + "." + CVersionClass::GetBuild() + " [" + OS_STR + "]" );
				guiInfo.AddData( "Compiled By", CVersionClass::GetName() );
				guiInfo.AddData( "Uptime", builtString );
				guiInfo.AddData( "Accounts", Accounts->size() );
				guiInfo.AddData( "Items", ObjectFactory::GetSingleton().CountOfObjects( OT_ITEM ));
				guiInfo.AddData( "Chars", ObjectFactory::GetSingleton().CountOfObjects( OT_CHAR ));
				guiInfo.AddData( "Players in world", static_cast<UI32>( cwmWorldState->GetPlayersOnline() ));
				guiInfo.Send( 0, false, INVALIDSERIAL );
			}
			break;
		case 'I':
			if( cmd == "ITEMMENU" )
			{
				if( data.empty() )
					return;

				BuildAddMenuGump( s, static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 )));
			}
			else if( cmd == "INFORMATION" )
			{
				builtString = GetUptime();
				s->SysMessage( 1211, builtString.c_str(), cwmWorldState->GetPlayersOnline(), Accounts->size(), ObjectFactory::GetSingleton().CountOfObjects( OT_ITEM ), ObjectFactory::GetSingleton().CountOfObjects( OT_CHAR ));
			}
			break;
		case 'M':
			if( cmd == "MAKEMENU" )
			{
				if( data.empty() )
					return;

				Skills->NewMakeMenu( s, std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ), static_cast<UI08>( s->AddId() ));
			}
			break;
		case 'N':
			if( cmd == "NPC" )
			{
				if( data.empty() )
					return;

				std::string menuString = oldstrutil::format( "itemmenu %d", s->TempInt() );
				Commands->Command( s, mChar, menuString );

				TAGMAPOBJECT repeatAdd = mChar->GetTag( "repeatAdd" );
				if( repeatAdd.m_IntValue == 1 )
				{
					std::string addCmd = oldstrutil::format( "raddnpc %s", data.c_str() );
					Commands->Command( s, mChar, addCmd );
				}
				else
				{
					s->XText( data );
					s->SendTargetCursor( 0, TARGET_ADDSCRIPTNPC, 0, 1212, data.c_str() ); // Select location for NPC. [Number: %s]
				}
			}
			break;
		case 'P':
			if( cmd == "POLYMORPH" )
			{
				if( data.empty() )
					return;

				UI16 newBody = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
				mChar->SetId( newBody );
				mChar->SetOrgId( newBody );
			}
			break;
		case 'S':
			if( cmd == "SYSMESSAGE" )
			{
				if( data.empty() )
					return;

				s->SysMessage( data.c_str() );
			}
			else if( cmd == "SKIN" )
			{
				if( data.empty() )
					return;

				COLOUR newSkin = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
				mChar->SetSkin( newSkin );
				mChar->SetOrgSkin( newSkin );
			}
			break;
		case 'V':
			if( cmd == "VERSION" )
			{
				s->SysMessage( "%s v%s(%s) [%s] Compiled by %s ", CVersionClass::GetProductName().c_str(), CVersionClass::GetVersion().c_str(), CVersionClass::GetBuild().c_str(), OS_STR, CVersionClass::GetName().c_str() );
			}
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleAddMenuButton()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles button pressed in add menu gump
//o------------------------------------------------------------------------------------------------o
void HandleAddMenuButton( CSocket *s, UI32 button )
{
	SI32 addMenuLoc	= s->TempInt();
	std::string sect = std::string( "ITEMMENU " ) + oldstrutil::number( addMenuLoc );
	CChar *mChar = s->CurrcharObj();

	if( button >= 50000 && button <= 50020 )
	{
		TAGMAPOBJECT addAtLoc = mChar->GetTag( "addAtLoc" );
		TAGMAPOBJECT repeatAdd = mChar->GetTag( "repeatAdd" );
		TAGMAPOBJECT reopenMenu = mChar->GetTag( "reopenMenu" );
		TAGMAPOBJECT forceDecayOn = mChar->GetTag( "forceDecayOn" );
		TAGMAPOBJECT forceDecayOff = mChar->GetTag( "forceDecayOff" );
		TAGMAPOBJECT forceMovableOn = mChar->GetTag( "forceMovableOn" );
		TAGMAPOBJECT forceMovableOff = mChar->GetTag( "forceMovableOff" );

		std::string tagName = "";
		SI32 tagVal = 0;
		TAGMAPOBJECT customTag;
		std::map<std::string, TAGMAPOBJECT> customTagMap;
		if( button == 50000 ) // Add item at specific location
		{
			tagName = "addAtLoc";
			TAGMAPOBJECT addAtLoc = mChar->GetTag( "addAtLoc" );
			if( addAtLoc.m_IntValue == 1 )
			{
				tagVal = 0;
				if( repeatAdd.m_IntValue == 1 )
				{
					customTag.m_Destroy		= false;
					customTag.m_IntValue 	= 0;
					customTag.m_ObjectType	= TAGMAP_TYPE_INT;
					customTag.m_StringValue	= "";
					customTagMap.insert( std::pair<std::string, TAGMAPOBJECT>( "repeatAdd", customTag ));
				}
			}
			else
			{
				tagVal = 1;
			}
		}
		else if( button == 50001 ) // Repeatedly add items until cancelled
		{
			tagName = "repeatAdd";
			TAGMAPOBJECT addAtLoc = mChar->GetTag( "repeatAdd" );
			if( addAtLoc.m_IntValue == 1 )
			{
				tagVal = 0;
			}
			else
			{
				tagVal = 1;
				if( addAtLoc.m_IntValue == 0 )
				{
					customTag.m_Destroy		= false;
					customTag.m_IntValue 	= 1;
					customTag.m_ObjectType	= TAGMAP_TYPE_INT;
					customTag.m_StringValue	= "";
					customTagMap.insert( std::pair<std::string, TAGMAPOBJECT>( "addAtLoc", customTag ));
				}
			}
		}
		else if( button == 50002 )
		{
			// Return to home page of menu
			Commands->Command( s, mChar, "add" );
			return;
		}
		else if( button == 50003 ) // Automatically reopen menu after adding objects
		{
			tagName = "reopenMenu";
			TAGMAPOBJECT reopenMenu = mChar->GetTag( "reopenMenu" );
			if( reopenMenu.m_IntValue == 1 )
			{
				tagVal = 0;
			}
			else
			{
				tagVal = 1;
				if( reopenMenu.m_IntValue == 0 )
				{
					customTag.m_Destroy		= false;
					customTag.m_IntValue 	= 1;
					customTag.m_ObjectType	= TAGMAP_TYPE_INT;
					customTag.m_StringValue	= "";
					customTagMap.insert( std::pair<std::string, TAGMAPOBJECT>( "reopenMenu", customTag ));
				}
			}
		}
		else if( button == 50004 ) // Toggle Force Decay state for items on/off
		{
			tagName = "forceDecayOff";
			TAGMAPOBJECT forceDecayOff = mChar->GetTag( "forceDecayOff" );
			if( forceDecayOff.m_IntValue == 1 )
			{
				tagVal = 0;
			}
			else
			{
				tagVal = 1;
				if( forceDecayOn.m_IntValue == 1 )
				{
					customTag.m_Destroy		= false;
					customTag.m_IntValue 	= 0;
					customTag.m_ObjectType	= TAGMAP_TYPE_INT;
					customTag.m_StringValue	= "";
					customTagMap.insert( std::pair<std::string, TAGMAPOBJECT>( "forceDecayOn", customTag ));
				}
			}
		}
		else if( button == 50005 ) // Toggle Force Decay state for items on/off
		{
			tagName = "forceDecayOn";
			TAGMAPOBJECT forceDecayOn = mChar->GetTag( "forceDecayOn" );
			if( forceDecayOn.m_IntValue == 1 )
			{
				tagVal = 0;
			}
			else
			{
				tagVal = 1;
				if( forceDecayOff.m_IntValue == 1 )
				{
					customTag.m_Destroy		= false;
					customTag.m_IntValue 	= 0;
					customTag.m_ObjectType	= TAGMAP_TYPE_INT;
					customTag.m_StringValue	= "";
					customTagMap.insert( std::pair<std::string, TAGMAPOBJECT>( "forceDecayOff", customTag ));
				}
			}
		}
		else if( button == 50006 ) // Toggle Force OFF Movable state for items
		{
			tagName = "forceMovableOff";
			TAGMAPOBJECT forceMovableOff = mChar->GetTag( "forceMovableOff" );
			if( forceMovableOff.m_IntValue == 1 )
			{
				tagVal = 0;
			}
			else
			{
				tagVal = 1;
				if( forceMovableOn.m_IntValue == 1 )
				{
					customTag.m_Destroy		= false;
					customTag.m_IntValue 	= 0;
					customTag.m_ObjectType	= TAGMAP_TYPE_INT;
					customTag.m_StringValue	= "";
					customTagMap.insert( std::pair<std::string, TAGMAPOBJECT>( "forceMovableOn", customTag ));
				}
			}
		}
		else if( button == 50007 ) // Toggle Force ON Movable state for items
		{
			tagName = "forceMovableOn";
			TAGMAPOBJECT forceMovableOn = mChar->GetTag( "forceMovableOn" );
			if( forceMovableOn.m_IntValue == 1 )
			{
				tagVal = 0;
			}
			else
			{
				tagVal = 1;
				if( forceMovableOff.m_IntValue == 1 )
				{
					customTag.m_Destroy		= false;
					customTag.m_IntValue 	= 0;
					customTag.m_ObjectType	= TAGMAP_TYPE_INT;
					customTag.m_StringValue	= "";
					customTagMap.insert( std::pair<std::string, TAGMAPOBJECT>( "forceMovableOff", customTag ));
				}
			}
		}
		else if( button == 50008 )
		{
			// Show command list
			Commands->Command( s, mChar, "howto" );
			return;
		}
		else if( button == 50009 )
		{
			// Show wholist online
			Commands->Command( s, mChar, "wholist on" );
			return;
		}
		else if( button == 50010 )
		{
			// Show wholist offline
			Commands->Command( s, mChar, "wholist off" );
			return;
		}
		else if( button == 50011 )
		{
			// Reload DFNs
			Commands->Command( s, mChar, "reloaddefs" );
			return;
		}
		else if( button == 50012 )
		{
			// Shutdown Server
			if( mChar->GetCommandLevel() >= CL_ADMIN )
				Commands->Command( s, mChar, "shutdown 1" );
			return;
		}
		else if( button == 50020 )
		{
			// Browse UOX3 Docs online
			Commands->Command( s, mChar, "browse https://www.uox3.org/docs" );
			
			// Reopen menu
			Commands->Command( s, mChar, sect );
			return;
		}
		customTag.m_Destroy		= false;
		customTag.m_IntValue 	= tagVal;
		customTag.m_ObjectType	= TAGMAP_TYPE_INT;
		customTag.m_StringValue	= "";
		customTagMap.insert( std::pair<std::string, TAGMAPOBJECT>( tagName, customTag ));

		// Add custom tags to multi
		for( const auto& [key, value] : customTagMap )
		{
			mChar->SetTag( key, value );
		}

		// Reopen menu
		Commands->Command( s, mChar, sect );
		return;
	}

	CScriptSection *ItemMenu = FileLookup->FindEntry( sect, items_def );
	if( ItemMenu == nullptr )
		return;

	// If we get here we have to check to see if there are any other entryies added via the auto-addmenu code. Each item == 2 entries IE: IDNUM=Text name of Item, and ADDITEM=itemID to add
	std::pair<ADDMENUMAP_CITERATOR, ADDMENUMAP_CITERATOR> pairRange = g_mmapAddMenuMap.equal_range( addMenuLoc );
	UI32 autoAddMenuItemCount = 0;
	for( ADDMENUMAP_CITERATOR CI = pairRange.first; CI != pairRange.second; ++CI )
	{
		autoAddMenuItemCount += 2;	// Need to inicrement by 2 because each entry is measured in the dfn' as two lines. Used in teh calculation below.
	}
	// let's skip over the name, and get straight to where we should be headed
	size_t entryNum = (( static_cast<size_t>( button ) - 6 ) * 2 );
	autoAddMenuItemCount += static_cast<UI32>( ItemMenu->NumEntries() );
	if( autoAddMenuItemCount >= entryNum )
	{
		std::string tag		= ItemMenu->MoveTo( entryNum );
		std::string data	= ItemMenu->GrabData();
		HandleGumpCommand( s, tag, data );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	HandleHowTo()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response when player enters HOWTO command
//o------------------------------------------------------------------------------------------------o
void HandleHowTo( CSocket *sock, SI32 cmdNumber )
{
	if( cmdNumber == -2 )
		return;

	SI32 iCounter = 0;
	UI08 cmdLevelReq = 0xFF;
	UI08 cmdType = 0xFF;
	std::string cmdName = "";
	bool found = false;
	for( auto itr = CommandMap.begin(); itr != CommandMap.end(); ++itr )
	{
		if( iCounter == cmdNumber )
		{
			cmdName = itr->first;
			cmdLevelReq = itr->second.cmdLevelReq;
			cmdType = itr->second.cmdType;
			found = true;
			break;
		}
		++iCounter;
	}

	if( !found )
	{
		for( auto itr = TargetMap.begin(); itr != TargetMap.end(); ++itr )
		{
			if( iCounter == cmdNumber )
			{
				cmdName = itr->first;
				cmdLevelReq = itr->second.cmdLevelReq;
				found = true;
				break;
			}
			++iCounter;
		}
	}

	if( !found )
	{
		for( auto itr = JSCommandMap.begin(); itr != JSCommandMap.end(); ++itr )
		{
			if( iCounter == cmdNumber )
			{
				cmdName = itr->first;
				cmdLevelReq = itr->second.cmdLevelReq;
				break;
			}
			++iCounter;
		}
	}

	if( iCounter == cmdNumber )
	{
		// Build gump structure here, with basic information like Command Level, Name, Command Type, and parameters (if any, from table)
		CGumpDisplay CommandInfo( sock, 480, 320 );
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


		auto filename = oldstrutil::format( "help/commands/%s.txt", cmdName.c_str() );

		std::ifstream toOpen( filename );
		if( !toOpen.is_open() )
		{
			CommandInfo.AddData( "", "No extra information is available about this command", 7 );
		}
		else
		{
			char cmdLine[129];
			while( !toOpen.eof() && !toOpen.fail() )
			{
				toOpen.getline( cmdLine, 128 );
				cmdLine[toOpen.gcount()] = 0;
				if( cmdLine[0] != 0 )
				{
					CommandInfo.AddData( "", cmdLine, 7 );
				}
			};
			toOpen.close();
		}
		CommandInfo.Send( 4, false, INVALIDSERIAL );
	}
	else
	{
		sock->SysMessage( 280 ); // Error finding command
	}
}

void HandleHowToButton( CSocket *s, UI32 button )
{
	HandleHowTo( s, button - 2 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIGumpMenuSelect::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles button press in gumps
//o------------------------------------------------------------------------------------------------o
bool CPIGumpMenuSelect::Handle( void )
{
	id			= tSock->GetDWord( 3 );
	gumpId		= tSock->GetDWord( 7 );
	buttonId	= tSock->GetDWord( 11 );
	switchCount	= tSock->GetDWord( 15 );

#if defined( UOX_DEBUG_MODE )
	Console << "CPIGumpMenuSelect::Handle(void)" << myendl;
	Console << "        GumpId : " << gumpId << myendl;
	Console << "      ButtonId : " << buttonId << myendl;
	Console << "   SwitchCount : " << switchCount << myendl;
#endif

	if( gumpId == 461 ) // Virtue gump
	{
		CChar *targChar = nullptr;
		if( buttonId == 1 && switchCount > 0 ) // Clicked on a players Virtue Gump icon
		{
			SERIAL targSer = tSock->GetDWord( 19 );
			targChar = CalcCharObjFromSer( targSer );
		}
		else // Clicked an item on the virtue gump
		{
			targChar = CalcCharObjFromSer( id );
		}

		cScript *toExecute = nullptr;
		std::vector<UI16> scriptTriggers = tSock->CurrcharObj()->GetScriptTriggers();
		for( auto scriptTrig : scriptTriggers )
		{
			toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				if( toExecute->OnVirtueGumpPress( tSock->CurrcharObj(), targChar, buttonId ) == 1 )
				{
					return true;
				}
			}
		}

		toExecute = JSMapping->GetScript( static_cast<UI16>( 0 )); // Global script
		if( toExecute != nullptr )
		{
			toExecute->OnVirtueGumpPress( tSock->CurrcharObj(), targChar, buttonId );
		}

		return true;
	}

	textOffset	= 19 + ( 4 * switchCount );
	textCount	= tSock->GetDWord( textOffset );

#if defined( UOX_DEBUG_MODE )
	Console << "    TextOffset : " << textOffset << myendl;
	Console << "     TextCount : " << textCount << myendl;
#endif

	BuildTextLocations();

	if( buttonId > 10000 && buttonId < 20000 )
	{
		BuildGumpFromScripts( tSock, static_cast<UI16>( buttonId - 10000 ));
		return true;
	}
	if( gumpId >= 8000 && gumpId <= 8020 )
	{
		GuildSys->GumpChoice( tSock );
		// guild collection call goes here
		return true;
	}
	else if( gumpId >= 0xFFFF )	// script gump
	{
		cScript *toExecute = JSMapping->GetScript(( gumpId - 0xFFFF ));
		if( toExecute != nullptr )
		{
			toExecute->HandleGumpPress( this );
		}
	}
	else if( gumpId == 21 ) // Multi functional gump
	{
		MultiGumpCallback( tSock, id, buttonId );
		return true;
	}
	else if( gumpId > 13 )
		return true; //increase this value with each new gump added.

	if( buttonId == 1 )
	{
		if( gumpId == 4 )
		{
			WhoList->GMLeave();
		}
		else if( gumpId == 11 )
		{
			OffList->GMLeave();
		}
		return true;
	}

	// gump types
	// TBD  To Be Done
	// TBDU To Be Decided Upon
	// 1	Tweak Item
	// 2	Tweak Char
	// 3	Townstones
	// 4	WhoList
	// 7	Accounts (TBR)
	// 8	Racial editor (TBDU)
	// 9	Add menu
	// 10	Unused
	// 11	Who's Offline
	// 12	New Make Menu
	// 13	HOWTO

#if defined( UOX_DEBUG_MODE )
	Console << "Type is " << gumpId << " button is " <<  buttonId << myendl;
#endif

	switch( gumpId )
	{
		case 3:	HandleTownstoneButton( tSock, buttonId, id, gumpId );	break;	// Townstones
		case 4:	WhoList->ButtonSelect( tSock, static_cast<UI16>( buttonId ), static_cast<UI08>( gumpId ));	break;	// Wholist
		case 7:																		// Accounts
			CChar *c;
			c = CalcCharObjFromSer( id );
			if( ValidateObject( c ))
			{
				HandleAccountButton( tSock, buttonId, c );
			}
			break;
		case 8:	HandleAccountModButton( this );							break;	// Race Editor
		case 9:	HandleAddMenuButton( tSock, buttonId );					break;	// Add Menu
		case 11: OffList->ButtonSelect( tSock, static_cast<UI16>( buttonId ), static_cast<UI08>( gumpId ));	break;	// Who's Offline
		case 12: Skills->HandleMakeMenu( tSock, buttonId, static_cast<SI32>( tSock->AddId() ));				break;	// New Make Menu
		case 13: HandleHowToButton( tSock, buttonId );					break;	// Howto
		default:
			break;
	}
	return true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIGumpInput::HandleTownstoneText()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles new values for townstones
//o------------------------------------------------------------------------------------------------o
void CPIGumpInput::HandleTownstoneText( UI08 index )
{
	CChar *mChar	= tSock->CurrcharObj();
	UI16 resourceId;
	UI32 amountToDonate;
	switch( index )
	{
		case 6:	// it's our donate resource button
		{
			CTownRegion *ourRegion = cwmWorldState->townRegions[mChar->GetTown()];
			amountToDonate = static_cast<UI32>( std::stoul( reply, nullptr, 0 ));
			if( amountToDonate == 0 )
			{
				tSock->SysMessage( 562 ); // You are donating nothing!
				return;
			}
			resourceId			= ourRegion->GetResourceId();
			UI32 numResources	= GetItemAmount( mChar, resourceId );

			if( amountToDonate > numResources )
			{
				tSock->SysMessage( 563, numResources ); // You have insufficient resources to donate that much!  You only have %i of them.
			}
			else
			{
				DeleteItemAmount( mChar, amountToDonate, resourceId );
				ourRegion->DonateResource( tSock, amountToDonate );
			}
			break;
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIGumpInput::Handle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles new values for gumps
//o------------------------------------------------------------------------------------------------o
bool CPIGumpInput::Handle( void )
{
	switch( type )
	{
		case 3:		HandleTownstoneText( index );	break;
		case 20:
		{
			CChar *mChar = CalcCharObjFromSer( id );
			if( ValidateObject( mChar ))
			{
				CSocket *mSock = mChar->GetSocket();
				if( mSock != nullptr )
				{
					UI32 scriptNo = mSock->AddId();
					if( scriptNo >= 0xFFFF )
					{
						cScript *toExecute = JSMapping->GetScript(( scriptNo - 0xFFFF ));
						if( toExecute != nullptr )
						{
							toExecute->HandleGumpInput( this );
						}
						mSock->AddId( 0 );
					}
				}
			}
		}
		[[fallthrough]]; // Indicate to compiler that fallthrough is intentional to suppress warning
		case 100:	GuildSys->GumpInput( this );			break;
		default:
			break;
	}
	return true;
}

std::string GrabMenuData( std::string sect, size_t entryNum, std::string &tag )
{
	std::string data;
	CScriptSection *sectionData = FileLookup->FindEntry( sect, menus_def );
	if( sectionData != nullptr )
	{
		if( sectionData->NumEntries() >= entryNum )
		{
			tag		= sectionData->MoveTo( entryNum );
			data	= sectionData->GrabData();
		}
	}
	return data;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CPIGumpChoice::Handle()
//|	Modified	-	2/10/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles choice "Help" menu, Tracking Menu, and Polymorph menu
//o------------------------------------------------------------------------------------------------o
bool CPIGumpChoice::Handle( void )
{
	std::string sect;
	std::string tag;
	std::string data;
	UI16 main		= tSock->GetWord( 5 );
	UI16 sub		= tSock->GetWord( 7 );
	CChar *mChar	= tSock->CurrcharObj();

	if( main >= JSGUMPMENUOFFSET ) // Between 0x4000 and 0xFFFF
	{
		// Handle button presses via global JS script
		cScript *toExecute = JSMapping->GetScript( static_cast<UI16>( 0 ));
		if( toExecute != nullptr )
		{
			if( toExecute->OnScrollingGumpPress( tSock, main, sub ) == 0 )
			{
				return true;
			}
		}

	}
	else if( main >= POLYMORPHMENUOFFSET )
	{
		sect = std::string( "POLYMORPHMENU " ) + oldstrutil::number( main );
		data = GrabMenuData( sect, ( static_cast<size_t>( sub ) * 2 ), tag );
		if( !data.empty() )
		{
			if( main == POLYMORPHMENUOFFSET )
			{
				Magic->PolymorphMenu( tSock, static_cast<UI16>( std::stoul( data, nullptr, 0 )));
			}
			else
			{
				if( mChar->IsOnHorse() )
				{
					DismountCreature( mChar );		// can't be polymorphed on a horse
				}
				else if( mChar->IsFlying() )
				{
					mChar->ToggleFlying();
				}
				Magic->Polymorph( tSock, static_cast<UI16>( std::stoul( data, nullptr, 0 )));
			}
		}
	}
	else if( main >= TRACKINGMENUOFFSET )
	{
		if( main == TRACKINGMENUOFFSET )
		{
			sect = std::string( "TRACKINGMENU " ) + oldstrutil::number( main );
			data = GrabMenuData( sect, ( static_cast<size_t>( sub ) * 2 ), tag );
			if( !data.empty() && tag != "What" )
			{
				Skills->CreateTrackingMenu( tSock, static_cast<UI16>( std::stoul( data, nullptr, 0 )));
			}
		}
		else
		{
			if( !Skills->CheckSkill( mChar, TRACKING, 0, 1000 ))
			{
				tSock->SysMessage( 575 ); // You fail your attempt at tracking.
				return true;
			}
			Skills->Tracking( tSock, static_cast<SI32>( sub - 1 ));
		}
	}
	else if( main < ITEMMENUOFFSET ) // GM Menus
	{
		sect = std::string( "GMMENU " ) + oldstrutil::number( main );
		data = GrabMenuData( sect, ( static_cast<size_t>( sub ) * 2 ), tag );
		if( !tag.empty() )
		{
			HandleGumpCommand( tSock, tag, data );
		}
	}
	return true;
}

//============================================================================================
auto HandleCommonGump( CSocket *mSock, CScriptSection *gumpScript, UI16 gumpIndex ) -> void
{
	auto mChar = mSock->CurrcharObj();
	std::string line;
	UI16 modelId = 0;
	UI16 modelColour = 0;
	std::string tag = gumpScript->First();
	std::string data = gumpScript->GrabData();
	line = tag + " "s + data;
	CPOpenGump toSend( *mChar );
	toSend.GumpIndex( gumpIndex );
	toSend.Question( line );
	for( tag = gumpScript->Next(); !gumpScript->AtEnd(); tag = gumpScript->Next() )
	{
		data = gumpScript->GrabData();
		modelId = static_cast<UI16>( stoul( tag, nullptr, 0 ));
		toSend.AddResponse( modelId, modelColour, data );
		tag = gumpScript->Next();
	}
	toSend.Finalize();
	mSock->Send( &toSend );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::AddData()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds the data to the gump to send
//o------------------------------------------------------------------------------------------------o
void CGumpDisplay::AddData( GumpInfo_st *toAdd )
{
	GumpInfo_st *gAdd		= new GumpInfo_st;
	gAdd->name			= toAdd->name;
	gAdd->value			= toAdd->value;
	gAdd->type			= toAdd->type;
	gAdd->stringValue	= toAdd->stringValue;
	gumpData.push_back( gAdd );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::AddData()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds the data to the gump to send
//o------------------------------------------------------------------------------------------------o
void CGumpDisplay::AddData( std::string toAdd, UI32 value, UI08 type )
{
	if( toAdd.empty() )
		return;

	GumpInfo_st *gAdd		= new GumpInfo_st;
	gAdd->name			= toAdd;
	gAdd->stringValue	= "";
	gAdd->value			= value;
	gAdd->type			= type;
	gumpData.push_back( gAdd );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::AddData()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds the data to the gump to send
//o------------------------------------------------------------------------------------------------o
void CGumpDisplay::AddData( std::string toAdd, const std::string &toSet, UI08 type )
{
	if( toAdd.empty() && type != 7 )
		return;

	GumpInfo_st *gAdd = new GumpInfo_st;
	gAdd->name = toAdd;
	gAdd->stringValue = toSet;
	gAdd->type = type;
	gumpData.push_back( gAdd );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::CGumpDisplay()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Begin CGumpDisplay stuff by setting the target, clearing any existing data, and setting the w / h
//o------------------------------------------------------------------------------------------------o
CGumpDisplay::CGumpDisplay( CSocket *target ) : toSendTo( target )
{
	gumpData.resize( 0 );
	width = 340;
	height = 320;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::CGumpDisplay( CSocket *target, UI16 gumpWidth, UI16 gumpHeight )
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Begin CGumpDisplay stuff by setting the target, clearing any existing data, and setting the w / h
//o------------------------------------------------------------------------------------------------o
CGumpDisplay::CGumpDisplay( CSocket *target, UI16 gumpWidth, UI16 gumpHeight ) :
width( gumpWidth ), height( gumpHeight ), toSendTo( target )
{
	gumpData.resize( 0 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::~CGumpDisplay()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Clear any existing CGumpDisplay data
//o------------------------------------------------------------------------------------------------o
CGumpDisplay::~CGumpDisplay()
{
	for( size_t i = 0; i < gumpData.size(); ++i )
	{
		delete gumpData[i];
	}
	Delete();
	gumpData.resize( 0 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::Delete()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Delete all CGumpDisplay entries
//o------------------------------------------------------------------------------------------------o
void CGumpDisplay::Delete( void )
{
	one.resize( 0 );
	two.resize( 0 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::SetTitle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Set a gumps title
//o------------------------------------------------------------------------------------------------o
void CGumpDisplay::SetTitle( const std::string& newTitle )
{
	title = newTitle;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SendVecsAsGump()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends to socket sock the data in one and two.  One is control, two is data
//o------------------------------------------------------------------------------------------------o
void SendVecsAsGump( CSocket *sock, std::vector<std::string>& one, std::vector<std::string>& two, UI32 type, SERIAL serial )
{
	CPSendGumpMenu toSend;
	toSend.GumpId( type );
	toSend.UserId( serial );

	size_t line = 0;
	for( line = 0; line < one.size(); ++line )
	{
		toSend.addCommand( one[line] );
	}

	for( line = 0; line < two.size(); ++line )
	{
		toSend.addText( two[line] );
	}

	toSend.Finalize();
	sock->Send( &toSend );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGumpDisplay::Send()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends gump data to the socket
//o------------------------------------------------------------------------------------------------o
void CGumpDisplay::Send( UI32 gumpNum, bool isMenu, SERIAL serial )
{
	constexpr auto maxsize = 512;
	std::string temp;
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
	one.push_back( "page 0"  );
	temp = oldstrutil::format( maxsize, "resizepic 0 0 %i %i %i", cwmWorldState->ServerData()->BackgroundPic(), width, height );
	one.push_back( temp );
	temp = oldstrutil::format( maxsize, "button %i 15 %i %i 1 0 1", width - 40, cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1 );
	one.push_back( temp );
	temp = oldstrutil::format( maxsize, "text 45 15 %i 0", cwmWorldState->ServerData()->TitleColour() );
	one.push_back( temp );

	temp = oldstrutil::format( maxsize, "page %u", pagenum );
	one.push_back( temp );

	if( title.length() == 0 )
	{
		temp = "General Gump";
	}
	else
	{
		 temp = title;
	}
	two.push_back( temp );

	numToPage = static_cast<UI08>((( height - 30 ) / 20 ) - 2 );
	stringWidth = static_cast<UI08>(( width / 2 ) / 10 );
	UI32 lineForButton;
	for( i = 0, lineForButton = 0; i < gumpData.size(); ++i, ++lineForButton )
	{
		if( lineForButton > 0 && ( !( lineForButton % numToPage )))
		{
			position = 40;
			++pagenum;
			temp = oldstrutil::format( maxsize, "page %u", pagenum );
			one.push_back( temp );
		}
		if( gumpData[i]->type != 7 )
		{
			temp = oldstrutil::format( maxsize, "text 50 %u %i %u", position, cwmWorldState->ServerData()->LeftTextColour(), linenum++ );
			one.push_back( temp );
			if( isMenu )
			{
				temp = oldstrutil::format( maxsize, "button 20 %u %i %i 1 0 %u", position, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, buttonnum );
				one.push_back( temp );
			}
			temp = oldstrutil::format( maxsize, "text %i %u %i %u", ( width / 2 ) + 10, position, cwmWorldState->ServerData()->RightTextColour(), linenum++ );
			one.push_back( temp );
			two.push_back( gumpData[i]->name );
		}
		else
		{
			temp = oldstrutil::format( maxsize, "text 30 %u %i %u", position, cwmWorldState->ServerData()->LeftTextColour(), linenum++ );
			one.push_back( temp );
		}

		UI32 value = gumpData[i]->value;
		switch( gumpData[i]->type )
		{
			case 0:		// all numbers of sorts
				temp = std::to_string( value );
				break;
			case 1:
				temp = oldstrutil::format( "%x", static_cast<UI32>( value ));
				break;
			case 2:
				ser1 = static_cast<UI08>( value >> 24 );
				ser2 = static_cast<UI08>( value >> 16 );
				ser3 = static_cast<UI08>( value >> 8 );
				ser4 = static_cast<UI08>( value % 256 );
				temp = oldstrutil::format( "%i %i %i %i", ser1, ser2, ser3, ser4 );
				break;
			case 3:
				ser1 = static_cast<UI08>( value >> 24 );
				ser2 = static_cast<UI08>( value >> 16 );
				ser3 = static_cast<UI08>( value >> 8 );
				ser4 = static_cast<UI08>( value % 256 );
				temp = oldstrutil::format( "%x %x %x %x", ser1, ser2, ser3, ser4 );
				break;
			case 4:
				if( gumpData[i]->stringValue.empty() )
				{
					temp = "nullptr POINTER";
				}
				else
				{
					temp = gumpData[i]->stringValue;
				}
				if( temp.size() > stringWidth )	// too wide for one line, CRAP!
				{
					std::string temp2;
					std::string temp3;

					size_t tempWidth =  temp.size() - stringWidth;
					temp2 = temp.substr( 0, stringWidth );

					two.push_back( temp2 );
					for( UI32 tempCounter = 0; tempCounter < tempWidth / ( static_cast<size_t>( stringWidth ) * 2 ) + 1; ++tempCounter )
					{
						//LOOKATME
						position += 20;
						++lineForButton;
						temp3 = oldstrutil::format( "text %i %u %i %u", 30, position, cwmWorldState->ServerData()->RightTextColour(), linenum++ );
						one.push_back( temp3 );
						auto remaining = std::min<std::size_t>(( temp.size() - ( static_cast<size_t>( tempCounter ) + 1 ) * static_cast<size_t>( stringWidth ) * 2 ), static_cast<size_t>( stringWidth ) * 2 );

						temp2 = temp.substr( static_cast<size_t>( stringWidth ) + static_cast<size_t>( tempCounter ) * static_cast<size_t>( stringWidth ) * 2, remaining );
						two.push_back( temp2 );
					}
					// be stupid for the moment and do no text wrapping over pages
				}
				else
				{
					two.push_back( temp );
				}
				break;
			case 5:
				ser1 = static_cast<UI08>( value >> 8 );
				ser2 = static_cast<UI08>( value % 256 );
				temp = oldstrutil::format( "0x%02x%02x", ser1, ser2 );
				break;
			case 6:
				ser1 = static_cast<UI08>( value >> 8 );
				ser2 = static_cast<UI08>( value % 256 );
				temp = oldstrutil::format( "%i %i", ser1, ser2 );
				break;
			case 7:
				if( gumpData[i]->stringValue.empty() )
				{
					temp = "Null Pointer";
				}
				else
				{
					temp = gumpData[i]->stringValue;
				}
				SI32 sWidth;
				sWidth = stringWidth * 2;
				if( temp.size() > static_cast<size_t>( sWidth ))	// too wide for one line, CRAP!
				{
					std::string temp2;
					std::string temp3;
					size_t tempWidth = temp.size() - sWidth;
					temp2= temp.substr( 0, sWidth );

					two.push_back( temp2 );
					for( UI32 tempCounter = 0; tempCounter < tempWidth / sWidth + 1; ++tempCounter )
					{
						position += 20;
						++lineForButton;
						temp3 = oldstrutil::format( 512, "text %i %u %i %u", 30, position, cwmWorldState->ServerData()->LeftTextColour(), linenum++ );
						one.push_back( temp3 );
						auto remaining = std::min<std::size_t>( temp.size() - ( static_cast<size_t>( tempCounter ) + 1 ) * static_cast<size_t>( sWidth ), static_cast<std::size_t>( sWidth ));

						temp2= temp.substr(( static_cast<size_t>( tempCounter ) + 1 ) * static_cast<size_t>( sWidth ), remaining );

						two.push_back( temp2 );
					}
					// be stupid for the moment and do no text wrapping over pages
				}
				else
				{
					two.push_back( temp );
				}
				break;
			case 8:
				if( gumpData[i]->stringValue.empty() )
				{
					temp =  "0.00";
				}
				else
				{

					temp = oldstrutil::format( "%.2f", std::stof( gumpData[i]->stringValue ));
				}
				break;
			default:
				temp = std::to_string( value);
				break;
		}
		if( gumpData[i]->type != 4 && gumpData[i]->type != 7 )
		{
			two.push_back( temp );
		}
		position += 20;
		++buttonnum;
	}

	pagenum = 1;
	for( i = 0; static_cast<UI32>( i ) <= lineForButton; i += numToPage )
	{
		temp =  "page "+std::to_string( pagenum );
		one.push_back( temp );
		if( i >= 10 )
		{
			temp = oldstrutil::format( "button 10 %i %i %i 0 %u", height - 40, cwmWorldState->ServerData()->ButtonLeft(), cwmWorldState->ServerData()->ButtonLeft() + 1, pagenum - 1 ); //back button
			one.push_back( temp );
		}
		if( lineForButton > numToPage && static_cast<UI32>(( i + numToPage )) < lineForButton )
		{
			temp = oldstrutil::format( "button %i %i %i %i 0 %u", width - 40, height - 40, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum + 1 ); //forward button
			one.push_back( temp );
		}
		++pagenum;
	}
	SendVecsAsGump( toSendTo, one, two, gumpNum, serial );
}
