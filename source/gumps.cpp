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
#include "StringUtility.hpp"
#include <algorithm>


void CollectGarbage( void );
std::string GetUptime( void );

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void TextEntryGump( CSocket *s, SERIAL ser, char type, char index, SI16 maxlength, SI32 dictEntry )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Open entry gump with specified dictionary message and max value length
//o-----------------------------------------------------------------------------------------------o
void TextEntryGump( CSocket *s, SERIAL ser, UI08 type, UI08 index, SI16 maxlength, SI32 dictEntry )
{
	if( s == nullptr )
		return;
	std::string txt = Dictionary->GetEntry( dictEntry, s->Language() );
	if( txt.empty() )
	{
		Console.error( "Invalid text in TextEntryGump()" );
		return;
	}

	auto temp = strutil::format( "(%i chars max)", maxlength );
	CPGumpTextEntry toSend( txt, temp );
	toSend.Serial( ser );
	toSend.ParentID( type );
	toSend.ButtonID( index );
	toSend.Cancel( 1 );
	toSend.Style( 1 );
	toSend.Format( maxlength );
	s->Send( &toSend );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void BuildGumpFromScripts( CSocket *s, UI16 m )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Pull gump info from misc.dfn
//o-----------------------------------------------------------------------------------------------o
void BuildGumpFromScripts( CSocket *s, UI16 m )
{
	CPSendGumpMenu toSend;
	toSend.UserID( INVALIDSERIAL );

	std::string sect = std::string("GUMPMENU ") + strutil::number( m );
	ScriptSection *gump = FileLookup->FindEntry( sect, misc_def );
	if( gump == nullptr )
		return;

	UI08 targType	= 0x12;
	std::string tag	= gump->First();
	if( strutil::toupper(strutil::extractSection(tag, " ", 0, 0 )) == "TYPE" )
	{
		targType = strutil::value<std::uint8_t>(strutil::extractSection(tag, " ", 1, 1 ));
		tag = gump->Next();
	}
	for( ; !gump->AtEnd(); tag = gump->Next() )
	{
		auto temp = strutil::format( "%s %s", tag.c_str(), gump->GrabData().c_str() );
		toSend.addCommand( temp );
	}
	sect = std::string("GUMPTEXT ") + strutil::number( m );
	gump = FileLookup->FindEntry( sect, misc_def );
	if( gump == nullptr )
		return;
	for( tag = gump->First(); !gump->AtEnd(); tag = gump->Next() )
	{
		toSend.addText(strutil::format( "%s %s", tag.c_str(), gump->GrabData().c_str()) );
	}
	toSend.GumpID( targType );
	toSend.Finalize();
	s->Send( &toSend );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void HandleAccountButton( CSocket *s, UI32 button, CChar *j )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles the accounts gump button
//o-----------------------------------------------------------------------------------------------o
void HandleAccountButton( CSocket *s, UI32 button, CChar *j )
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
				if( targSocket != nullptr )
					targSocket->sysmessage( 488 );
				actbTemp.wFlags.set( AB_FLAGS_BANNED, true );
				if( targSocket != nullptr )
					Network->Disconnect( targSocket );
			}
			else
				s->sysmessage( 489 );
			break;
		case 3:
			if( mChar->IsGM() && !j->IsNpc() )
			{
				s->sysmessage( 490 );
				if( targSocket != nullptr )
					targSocket->sysmessage( 491 );
				actbTemp.wFlags.set( AB_FLAGS_BANNED, true );
				actbTemp.wTimeBan = GetMinutesSinceEpoch() + static_cast<UI32>( 1440 );

				if( targSocket != nullptr )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void HandleTownstoneButton( CSocket *s, SERIAL button, SERIAL ser, SERIAL type )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles button pressed in townstone gump
//o-----------------------------------------------------------------------------------------------o
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
			targetRegion = calcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber(), mChar->GetInstanceID() );
			if( targetRegion != nullptr )
				targetRegion->ViewTaxes( s );
			else
				s->sysmessage( 541 );
			break;
		case 4:		// toggle town title
			mChar->SetTownTitle( !mChar->GetTownTitle() );
			ourRegion->DisplayTownMenu( nullptr, s );
			break;
		case 5:		s->target( 0, TARGET_VOTEFORMAYOR, 542 );								break;		// vote for town mayor
		case 6:		TextEntryGump(  s, ser, static_cast<UI08>(type), static_cast<UI08>(button), 6, 543 );	break;		// gold donation
		case 7:		ourRegion->ViewBudget( s );			break;		// View Budget
		case 8:		ourRegion->SendAlliedTowns( s );		break;		// View allied towns
		case 9:		ourRegion->SendEnemyTowns( s );		break;
		case 20:	// access mayoral functions, never hit here if we don't have mayoral access anyway!
			// also, we'll only get access, if we're in our OWN region
			ourRegion->DisplayTownMenu( nullptr, s, 0x01 );	// mayor
			break;
		case 21:	s->sysmessage( 544 );				break;	// set taxes
		case 22:	ourRegion->DisplayTownMembers( s );	break;	// list town members
		case 23:	ourRegion->ForceEarlyElection();		break;	// force early election
		case 24:	s->sysmessage( 545 );	break;	// purchase more guards
		case 25:	s->sysmessage( 546 );	break;	// fire a guard
		case 26:	s->target( 0, TARGET_TOWNALLY, 547 );								break;	// make a town an ally
		case 40:	ourRegion->DisplayTownMenu( nullptr, s );	break;	// we can't return from mayor menu if we weren't mayor!
		case 41:	// join town!
			if( !(calcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber(), mChar->GetInstanceID() )->AddAsTownMember( (*mChar) ) ) )
				s->sysmessage( 548 );
			break;
		case 61:	// seize townstone!
			if( !Skills->CheckSkill( mChar, STEALING, 950, 1000 )	) // minimum 95.0 stealing
			{
				targetRegion = calcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber(), mChar->GetInstanceID() );
				if( targetRegion != nullptr )
				{
					SI32 chanceToSteal	= RandomNum( 0, targetRegion->GetHealth() / 2 );
					SI32 fudgedStealing	= RandomNum( mChar->GetSkill( STEALING ), static_cast< UI16 >(mChar->GetSkill( STEALING ) * 2) );
					if( fudgedStealing >= chanceToSteal )
					{
						// redo stealing code here
						s->sysmessage( 549, targetRegion->GetName().c_str() );
						targetRegion->DoDamage( targetRegion->GetHealth() / 2 );	// we reduce the region's health by half
						REGIONLIST nearbyRegions = MapRegion->PopulateList( mChar );
						for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
						{
							CMapRegion *toCheck = (*rIter);
							if( toCheck == nullptr )	// no valid region
								continue;
							GenericList< CItem * > *regItems = toCheck->GetItemList();
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
			targetRegion = calcRegionFromXY( mChar->GetX(), mChar->GetY(), mChar->WorldNumber(), mChar->GetInstanceID() );
			for( UI08 counter = 0; counter < RandomNum( 5, 10 ); ++counter )
			{
				Effects->PlayMovingAnimation( mChar, mChar->GetX() + RandomNum( -6, 6 ), mChar->GetY() + RandomNum( -6, 6 ), mChar->GetZ(), 0x36E4, 17, 0, ( RandomNum( 0, 1 ) == 1 ) );
				Effects->PlayStaticAnimation( mChar->GetX() + RandomNum( -6, 6 ), mChar->GetY() + RandomNum( -6, 6 ), mChar->GetZ(), 0x373A + RandomNum( 0, 4 ) * 0x10, 0x09, 0, 0 );
			}
			targetRegion->DoDamage( RandomNum( 0, targetRegion->GetHealth() / 8) );	// we reduce the region's health by half
			break;
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void HandleHairDyeButton( CSocket *s, CItem *j )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles button pressed in hair dye gump
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void HandleAccountModButton( CPIGumpMenuSelect *packet )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles button pressed in account gump
//o-----------------------------------------------------------------------------------------------o
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
			case 1000:	username	= packet->GetTextString( i );	break;
			case 1001:	password	= packet->GetTextString( i );	break;
			case 1002:	emailAddy	= packet->GetTextString( i );	break;
			default:
				Console.warning( strutil::format("Unknown textID %i with string %s", textID, packet->GetTextString( i ).c_str()) );
		}
	}

	CAccountBlock& actbAccountFind = Accounts->GetAccountByName( username );
	if( actbAccountFind.wAccountIndex != AB_INVALID_ID )
	{
		s->sysmessage( 555 );
		return;
	}
	Console.print( strutil::format("Attempting to add username %s with password %s at emailaddy %s", username.c_str(), password.c_str(), emailAddy.c_str()) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function		-	void BuildAddMenuGump( CSocket *s, UI16 m )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose			-	Menu for item creation based on ITEMMENU entries in the ItemMenu DFNs
//|
//|	Modification	-	04042004 - Added support for the new Auto-AddMenu
//|									items if they exist. Remember the multimap is groupID based
//|									so it should order it accordingly we only have to look for
//|									the entries.
//o-----------------------------------------------------------------------------------------------o
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

	std::string sect = std::string("ITEMMENU ") + strutil::number( m );
	ScriptSection *ItemMenu = FileLookup->FindEntry( sect, items_def );
	if( ItemMenu == nullptr )
		return;

	// page header
	toSend.addCommand( "nodispose" );
	toSend.addCommand( "page 0" );

	// Need more control over this aspect of uox3 from outside some how.. first step to get the variables more flexible. later we will emporer them to a template, or a script or something
	UI16 xStart = 0, xWidth = 500;
	UI16 yStart = 0, yWidth = 375;

	UI32 bgImage	=	cwmWorldState->ServerData()->BackgroundPic();

	// Set and resize the gumps background image.
	toSend.addCommand( strutil::format("resizepic %u %u %u %u %u",xStart,yStart, bgImage, xWidth, yWidth) );
	toSend.addCommand( strutil::format("checkertrans %u %u %u %u",xStart+5,yStart+5,xWidth-10,yWidth-11));

	// Next we create and position the close window button as well set its Down, and Up states.
	toSend.addCommand(strutil::format( "button %u %u %u %u %u %u %u",xWidth-28,yStart+1,0xA51, 0xA50, 1, 0, 1));
	// Grab the first tag/value pair from the gump itemmenu respectivly
	std::string tag		= ItemMenu->First();
	std::string data	= ItemMenu->GrabData();
	toSend.addCommand(strutil::format( "resizepic %u %u %u %u %u",2,4,0xDAC,470,40));
	// Create the text stuff for what appears to be the title of the gump. This appears to change from page to page.

	linenum = 0;
	toSend.addCommand( strutil::format("text %u %u %u %u",30,yStart+13, 39, linenum++));
	toSend.addText( "Players: " );
	// Player count
	auto szBuffer = strutil::format( "%4i ", cwmWorldState->GetPlayersOnline() );
	toSend.addCommand( strutil::format("text %u %u %u %u", 80, yStart+13, 25, linenum++ ));
	toSend.addText( szBuffer );
	// Gm Pages
	toSend.addCommand( strutil::format("text %u %u %u %u",118,yStart+13, 39, linenum++));
	toSend.addText( "GM Pages: " );
	szBuffer = "0";
	toSend.addCommand( strutil::format("text %u %u %u %u",185,yStart+13, 25, linenum++));
	toSend.addText( szBuffer );
	// Current Time/Date
	toSend.addCommand( strutil::format("text %u %u %u %u",230,yStart+13, 39, linenum++));
	toSend.addText( "Time: " );
	// Current server time
	auto timet = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	auto today = std::localtime(&timet);
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
	szBuffer = strutil::format( "%s %.8s%s", tmpBuffer, asctime( today ) + 11, ( (isAM)?"Am":"Pm") );
	toSend.addCommand( strutil::format("text %u %u %u %u",280,yStart+13, 25, linenum++));
	toSend.addText( szBuffer );

	// add the next gump portion. New server level services, in the form of a gump Configuration, and Accounts tabs to start. These are default tabs
	toSend.addCommand(strutil::format( "resizepic %u %u %u %u %u",xStart+10,yStart+62, 0x13EC, 190, 300));
	UI32 tabNumber = 1;
	if( m == 1 )
	{
		// Do the shard tab
		toSend.addCommand( strutil::format("button %u %u %u %u %u %u %u",17,yStart+47,0x138E, 0x138F, 0, 2, 0));
		szBuffer = "Shard";
		toSend.addCommand( strutil::format("text %u %u %u %u",42,yStart+46, 47, linenum++));
		toSend.addText( szBuffer );
		// Do the server tab
		toSend.addCommand( strutil::format("button %u %u %u %u %u %u %u",105,yStart+47,0x138E, 0x138F, 0, 30, 1));
		toSend.addCommand( strutil::format("text %u %u %u %u",132,yStart+46, 47, linenum++));
		toSend.addText( "Server" );
	}
	else
	{
		// Do the shard tab
		toSend.addCommand( strutil::format("button %u %u %u %u %u %u %u",17,yStart+47,0x138E, 0x138F, 0, 1, 0));
		szBuffer = "Shard";
		toSend.addCommand( strutil::format("text %u %u %u %u",42,yStart+46, 47, linenum++));
		toSend.addText( szBuffer );
		// Do the server tab
		toSend.addCommand( strutil::format("button %u %u %u %u %u %u %u",105,yStart+47,0x138E, 0x138F, 0, 30, 1));
		toSend.addCommand(strutil::format( "text %u %u %u %u",132,yStart+46, 47, linenum++));
		toSend.addText( "Server" );
	}

	// Need a seperator
	// 0x2393 does NOT exist on early clients!  Someone's been naughty!
	if( s->ClientVersionMajor() >= 4 ) {
		toSend.addCommand( strutil::format("gumppictiled %u %u %u %u %u",xStart+22,yWidth-50,165,5,0x2393));
	}
	else {
		toSend.addCommand(strutil::format( "gumppictiled %u %u %u %u %u", xStart + 22, yWidth - 50, 165, 5, 0x145E ));
	}
	// Ok, now the job of pulling the rest of the first itemmenu information and making tabs for them
	toSend.addCommand(strutil::format( "text %u %u %u %u",20,yWidth-40, 94, linenum++ ));
	toSend.addText( "Enjoy the fish!" );

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
		toSend.addCommand( "page 1" );
		// Thanks for choosing UOX3 text
		toSend.addCommand( strutil::format("text %u %u %u %u", 15, yStart+65, 52, linenum++));
		toSend.addText( "Thank you for choosing UOX3!" );
		toSend.addCommand(strutil::format( "htmlgump %u %u %u %u %u %u %u", 58, yStart+83, 200, 20, linenum++,0,0));
		toSend.addText(strutil::format( "<a href=\"http://www.uox3.org/\">www.uox3.org</a>" ));
		// Need a seperator
		if( s->ClientVersionMajor() >= 4 ) {
			toSend.addCommand( strutil::format("gumppictiled %u %u %u %u %u", xStart + 21, yStart + 105, 165, 5, 0x2393 ));
		}
		else {
			toSend.addCommand( strutil::format("gumppictiled %u %u %u %u %u", xStart + 21, yStart + 105, 165, 5, 0x145E) );
		}
		toSend.addCommand( strutil::format("htmlgump %u %u %u %u %u %u %u", xStart+16,yStart+120,175,140,linenum++,1,1));
		toSend.addText( "<b>Server:</b>\n\nThe 'Server' tab is where a shard op will find most of the server related configuration tools.\n\n<b>Shard:</b>\n\nThe 'Shard'  tab is where a ShardOP will find the tools, items, and objects related to operating a shard, and filling it content.\n" );
		// Show the version and total memory used by UOX3
		toSend.addCommand( strutil::format("text %u %u %u %u",40,305,121,linenum++));
		toSend.addText( "Version: " );
		toSend.addCommand( strutil::format("text %u %u %u %u",100,305,120,linenum++));
		szBuffer = " v" + CVersionClass::GetVersion() + "." + CVersionClass::GetBuild();
		toSend.addText( szBuffer );
		// Do the UOX logo (okok so what! O_o)
		toSend.addCommand( strutil::format("gumppic %u %u %u",248,98,0x1392));
		if( s->ClientVersionMajor() >= 4 )
			toSend.addCommand( strutil::format("gumppic %u %u %u",315,150,0x2328));
		else
			toSend.addCommand( strutil::format("gumppic %u %u %u", 315, 150, 0x1580) );
		toSend.addCommand(strutil::format( "text %u %u %u %u",370,180,37,linenum++));
		toSend.addText( "X" );
		toSend.addCommand( strutil::format("text %u %u %u %u",370,190,67,linenum++));
		toSend.addText( "3" );

		// Now we make the fisrt page that will escentially display our list of Main GM Groups.
		pagenum = 2;
		toSend.addCommand( "page 2" );
		// Do the shard tab
		toSend.addCommand( strutil::format("gumppic %u %u %u ",17,yStart+47,0x138F));
		szBuffer = "Shard";
		toSend.addCommand( strutil::format("text %u %u %u %u",42,yStart+46, 70, linenum++) );
		toSend.addText( szBuffer );

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
				toSend.addCommand(strutil::format( "page %i", ++pagenum ));
				xOffset		= SXOFFSET;
				yOffset		= SYOFFSET;
			}
			// Drop in the page number text area image
			toSend.addCommand(strutil::format( "gumppic %u %u %u",xStart+260,yWidth-28,0x98E));
			// Add the page number text to the text area for display
			toSend.addCommand( strutil::format("text %u %u %u %u",xStart+295,yWidth-27,901,linenum++));
			szBuffer = strutil::format( "Menu %i - Page %i", m, pagenum-1 );
			toSend.addText( szBuffer );
			// Spin the tagged items loaded in from the dfn files.
			toSend.addCommand( strutil::format("button %u %u %u %u %u %u %u",15,position, 0x4B9, 0x4BA, 1, 0, buttonnum));
			toSend.addCommand( strutil::format("croppedtext %u %u %u %u %u %u",35, position-3,150,20, 50, linenum++ ));
			toSend.addText( data );
			if( tag.data()[0] != '<' && tag.data()[0] != ' ' )	// it actually has a picture, well bugger me! :>
			{
				// Draw a frame for the item to make it stand out a touch more.
				toSend.addCommand( strutil::format("resizepic %u %u %u %u %u", xOffset, yOffset, 0x53, 65, 100 ));
				toSend.addCommand( strutil::format("checkertrans %u %u %u %u", xOffset + 7, yOffset + 9, 52, 82 ));
				toSend.addCommand( strutil::format("tilepic %u %u %i",xOffset + 5, yOffset + 10, std::stoi(tag, nullptr, 0) ));
				toSend.addCommand(strutil::format( "croppedtext %u %u %u %u %u %u", xOffset, yOffset+65, 65, 20, 55, linenum++) );
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
			toSend.addCommand(strutil::format( "page %i", currentPage ));
			if( i >= 10 )
			{
				if( s->ClientVersionMajor() >= 4 ){
					toSend.addCommand( strutil::format("button %u %u %u %u %u %u",xStart+240,yWidth-25, 0x25EB, 0x25EA,0, currentPage-1));
				}//back button
				else {
					toSend.addCommand( strutil::format("button %u %u %u %u %u %u",xStart+240,yWidth-25, 0x15A2, 0x15A3,0, currentPage-1));
				}//back button
			}
			if( ( numCounter > 12 ) && ( ( i + 12 ) < numCounter ) )
			{
				if( s->ClientVersionMajor() >= 4 ){
					toSend.addCommand( strutil::format("button %u %u %u %u %u %u",xWidth-60,yWidth-25, 0x25E7, 0x25E6,0 ,currentPage+1));
				}
				else{
					toSend.addCommand( strutil::format("button %u %u %u %u %u %u",xWidth-60,yWidth-25, 0x15A5, 0x15A6,0 ,currentPage+1));
				}
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
		toSend.addCommand(strutil::format( "gumppic %u %u %u ",17,yStart+47,0x138F));
		szBuffer = "Shard";
		toSend.addCommand( strutil::format("text %u %u %u %u",42,yStart+46, 70, linenum++ ));
		toSend.addText( szBuffer );
		// here we hunt tags to make sure that we get them all from the itemmenus etc.
		UI08 numCounter = 0;
		position		= 80;
		xOffset			= SXOFFSET;
		yOffset			= SYOFFSET;

		// Drop in the page number text area image
		toSend.addCommand( strutil::format("gumppic %u %u %u",xStart+260,yWidth-28,0x98E));
		// Add the page number text to the text area for display
		toSend.addCommand( strutil::format("text %u %u %u %u",xStart+295,yWidth-27,901,linenum++));
		szBuffer = strutil::format( "Menu %i - Page %i", m, pagenum-1 );
		toSend.addText( szBuffer );

		for( tag = ItemMenu->Next(); !ItemMenu->AtEnd(); tag = ItemMenu->Next() )
		{
			data = ItemMenu->GrabData();

			if( numCounter > 0 && (!(numCounter % 12 ) ) )
			{
				position = 80;
				toSend.addCommand( strutil::format("page %i", pagenum++) );
				xOffset=SXOFFSET;
				yOffset=SYOFFSET;
			}
			// Drop in the page number text area image
			if( strutil::toupper( tag ) == "INSERTADDMENUITEMS" )
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
					toSend.addCommand( strutil::format("button %u %u %u %u %u %u %u",15,position, 0x4B9, 0x4BA ,1,0, buttonnum));
					toSend.addCommand( strutil::format("croppedtext %u %u %u %u %u %u",35, position-3,150,20, 40, linenum) );
					toSend.addText( CI->second.itemName );
					// check to make sure that we have an image now, seeing as we might not have one with the new changes in 0.98.01.2+
					if(CI->second.tileID != 0)
					{
						// Draw a frame for the item to make it stand out a touch more.
						toSend.addCommand( strutil::format("resizepic %u %u %u %u %u",xOffset,yOffset,0x53,65,100));
						toSend.addCommand( strutil::format("checkertrans %u %u %u %u",xOffset+7,yOffset+9,52,82));
						toSend.addCommand( strutil::format("tilepic %u %u %i", xOffset+5, yOffset+10, CI->second.tileID ));
						toSend.addCommand( strutil::format("croppedtext %u %u %u %u %u %u",xOffset,yOffset+65,65,20,55,linenum++));
						toSend.addText( CI->second.itemName );
						toSend.addText( CI->second.itemName.c_str() );
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
			toSend.addCommand( strutil::format("button %u %u %u %u %u %u %u",15,position, 0x4B9, 0x4BA, 1, 0, buttonnum ));
			toSend.addCommand( strutil::format("croppedtext %u %u %u %u %u %u",35, position-3,150,20, 50, linenum++ ));
			toSend.addText( data );
			if( tag.data()[0] != '<' && tag.data()[0] != ' ' )	// it actually has a picture, well bugger me! :>
			{
				// Draw a frame for the item to make it stand out a touch more.
				toSend.addCommand( strutil::format("resizepic %u %u %u %u %u", xOffset, yOffset, 0x53, 65, 100 ));
				toSend.addCommand( strutil::format("checkertrans %u %u %u %u", xOffset + 7, yOffset + 9, 52, 82 ));
				toSend.addCommand( strutil::format("tilepic %u %u %i", xOffset + 5, yOffset + 10, std::stoi(tag, nullptr, 0) ));
				toSend.addCommand( strutil::format("croppedtext %u %u %u %u %u %u", xOffset, yOffset + 65, 65, 20, 55, linenum++ ));
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
			toSend.addCommand( strutil::format("page %i", currentPage) );
			if( i >= 10 )
			{
				if( s->ClientVersionMajor() >= 4 ) {
					toSend.addCommand( strutil::format("button %u %u %u %u %u %u",xStart+240,yWidth-25, 0x25EB, 0x25EA,0, currentPage-1)); //back button
				}
				else {
					toSend.addCommand( strutil::format("button %u %u %u %u %u %u",xStart+240,yWidth-25, 0x15A2, 0x15A3,0, currentPage-1)); //back button
				}
			}
			if( ( numCounter > 12 ) && ( ( i + 12 ) < numCounter ) )
			{
				if( s->ClientVersionMajor() >= 4 ){
					toSend.addCommand( strutil::format("button %u %u %u %u %u %u",xWidth-60,yWidth-25, 0x25E7, 0x25E6,0 ,currentPage+1));
				}
				else
					toSend.addCommand(strutil::format( "button %u %u %u %u %u %u",xWidth-60,yWidth-25, 0x15A5, 0x15A6,0 ,currentPage+1));
			}
			++currentPage;
		}
	}


	// Reserved page # 800 - 899 for the server support pages
	toSend.addCommand( "page 30" );
	// Show the selected tab image for this page.
	toSend.addCommand( strutil::format("gumppic %u %u %u",105, yStart + 47, 0x138F) );
	toSend.addCommand( strutil::format("text %u %u %u %u",132, yStart + 46, 70, linenum++ ));
	toSend.addText( "Server" );
	// Create the Server Shutdown button
	if( s->ClientVersionMajor() >= 4 ){
		toSend.addCommand( strutil::format("button %u %u %u %u %u %u %u", 45, yStart + 72, 0x2A58, 0x2A44, 1, 4, tabNumber++ ));
	}
	else{
		toSend.addCommand( strutil::format("button %u %u %u %u %u %u %u", 45, yStart + 72, 0x047F, 0x0480, 1, 4, tabNumber++) );
	}
	toSend.addCommand(strutil::format( "text %u %u %u %u", 54, yStart + 76, 52, linenum++) );
	toSend.addText( "ShutdownServer" );
	if( s->ClientVersionMajor() >= 4 )
		toSend.addCommand(strutil::format( "button %u %u %u %u %u %u %u", 165, yStart + 75, 0x5689, 0x568A, 0, 31, tabNumber++ ));
	else
		toSend.addCommand(strutil::format( "button %u %u %u %u %u %u %u", 165, yStart + 75, 0x047F, 0x0480, 0, 31, tabNumber++) );
	// Need a seperator
	if( s->ClientVersionMajor() >= 4 )
		toSend.addCommand(strutil::format( "gumppictiled %u %u %u %u %u", xStart + 21, yStart + 105, 165, 5, 0x2393 ));
	else
		toSend.addCommand( strutil::format("gumppictiled %u %u %u %u %u", xStart + 21, yStart + 105, 165, 5, 0x145E ));
	// Ok short message saying that were working on this tab.
	toSend.addCommand( strutil::format("htmlgump %u %u %u %u %u %u %u", xStart + 16, yStart + 130, 175, 50, linenum++, 2, 0 ));
	toSend.addText( strutil::format("Currently this tab is under construction.") );

	// Reserved pages 900-999 for the online help system. (comming soon)
	toSend.addCommand( "page 31" );
	// Ok display the scroll that we use to display our help information
	toSend.addCommand( strutil::format("resizepic %u %u %u %u %u", xStart + 205, yStart + 62, 0x1432, 175, 200) );
	// Write out what page were on (Mainly for debug purposes
	szBuffer = strutil::format( "%5u", 31 );
	toSend.addCommand( strutil::format("text %u %u %u %u",xWidth-58,yWidth-25,110, linenum++) );
	toSend.addText( szBuffer );
	// Ok, now the job of pulling the rest of the first itemmenu information and making tabs for them
	szBuffer = "Page 31";
	toSend.addCommand(strutil::format( "text %u %u %u %u",30, yStart + 200, 87, linenum++) );
	toSend.addText( szBuffer );
	toSend.addCommand( strutil::format("button %u %u %u %u %u %u %u",104, yStart + 300, 0x138E, 0x138E, 0, 1, tabNumber++) );

#if defined( UOX_DEBUG_MODE )
	Console << "==============================" << myendl;
#endif
	// Finish up and send the gump to the client socket.
	toSend.Finalize();
	s->Send( &toSend );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CPIHelpRequest::Handle( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens the "Help" Menu from the paperdoll
//o-----------------------------------------------------------------------------------------------o
bool CPIHelpRequest::Handle( void )
{
	UI16 gmnumber	= 0;
	CChar *mChar	= tSock->CurrcharObj();

	std::string sect = std::string("GMMENU ") + strutil::number( menuNum );
	ScriptSection *GMMenu = FileLookup->FindEntry( sect, menus_def );
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

//o-----------------------------------------------------------------------------------------------o
//|   Function	  -  void CPage( CSocket *s, const std::string& reason )
//o-----------------------------------------------------------------------------------------------o
//|   Purpose     -  Used when player pages a counselor
//o-----------------------------------------------------------------------------------------------o
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
	pageToAdd.TimeOfPage( time( nullptr ) );

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

			{
				//std::scoped_lock lock(Network->internallock);
				Network->pushConn();
				for( CSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
				{
					CChar *iChar = iSock->CurrcharObj();
					if( iChar->IsGMPageable() || iChar->IsCounselor() )
					{
						x = true;
						iSock->sysmessage( strutil::format(1024, "Counselor Page from %s [%x %x %x %x]: %s", mChar->GetName().c_str(), a1, a2, a3, a4, reason.c_str() ) );
					}
				}
				Network->popConn();
			}
			if( x )
				s->sysmessage( 360 );
			else
				s->sysmessage( 361 );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|   Function	  :  void GMPage( CSocket *s, const std::string reason )
//o-----------------------------------------------------------------------------------------------o
//|   Purpose     :  Used when player calls a GM
//o-----------------------------------------------------------------------------------------------o
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
	pageToAdd.TimeOfPage( time( nullptr ) );
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
			{
				//std::scoped_lock lock(Network->internallock);
				Network->pushConn();
				for( CSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
				{
					CChar *iChar = iSock->CurrcharObj();
					if( !ValidateObject( iChar ) )
						continue;
					if( iChar->IsGMPageable() )
					{
						x = true;
						iSock->sysmessage( strutil::format(1024, "Page from %s [%x %x %x %x]: %s", mChar->GetName().c_str(), a1, a2, a3, a4, reason.c_str() ) );
					}
				}
				Network->popConn();
			}
			if( x )
				s->sysmessage( 363 );
			else
				s->sysmessage( 364 );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void HandleGumpCommand( CSocket *s, std::string cmd, std::string data )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Execute a command from scripts
//o-----------------------------------------------------------------------------------------------o
void HandleGumpCommand( CSocket *s, std::string cmd, std::string data )
{
	if( !s || cmd.empty() )
		return;

	CChar *mChar = s->CurrcharObj();

	cmd		= strutil::toupper( cmd );
	data	= strutil::toupper( data );
	std::string builtString;

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
					auto secs = strutil::sections( data, "," );
					if( secs.size() > 1 )
					{
						std::string tmp		= strutil::stripTrim( secs[0] );
						UI16 num			= strutil::value<std::uint16_t>(strutil::stripTrim( secs[1] ));
						Items->CreateScriptItem( s, mChar, tmp, num, itemType, true );
					}
					else
					{
						Items->CreateScriptItem( s, mChar, data, 0, itemType, true );
					}
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
				{
					return;
				}
				CPIHelpRequest toHandle( s,static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
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
				UI16 placeNum = static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
				if( cwmWorldState->goPlaces.find( placeNum ) != cwmWorldState->goPlaces.end() )
				{
					GoPlaces_st toGoTo = cwmWorldState->goPlaces[placeNum];

					if( toGoTo.worldNum == 0 && mChar->WorldNumber() <= 1 )
					{
						// Stay in same world if already in world 0 or 1
						mChar->SetLocation( toGoTo.x, toGoTo.y, toGoTo.z, mChar->WorldNumber(), mChar->GetInstanceID() );
					}
					else if( toGoTo.worldNum != mChar->WorldNumber() )
					{
						// Change map!
						mChar->SetLocation( toGoTo.x, toGoTo.y, toGoTo.z, toGoTo.worldNum, mChar->GetInstanceID() );
						SendMapChange( toGoTo.worldNum, s );
					}
				}
			}
			else if( cmd == "GUIINFORMATION" )
			{
				GumpDisplay guiInfo( s, 400, 300 );
				guiInfo.SetTitle( CVersionClass::GetProductName() + " Status" );
				builtString = GetUptime();
				guiInfo.AddData( "Version", CVersionClass::GetVersion() + "." + CVersionClass::GetBuild() + " [" + OS_STR + "]" );
				guiInfo.AddData( "Compiled By", CVersionClass::GetName() );
				guiInfo.AddData( "Uptime", builtString );
				guiInfo.AddData( "Accounts", Accounts->size() );
				guiInfo.AddData( "Items", ObjectFactory::getSingleton().CountOfObjects( OT_ITEM ) );
				guiInfo.AddData( "Chars", ObjectFactory::getSingleton().CountOfObjects( OT_CHAR ) );
				guiInfo.AddData( "Players in world", static_cast<UI32>(cwmWorldState->GetPlayersOnline()) );
				guiInfo.Send( 0, false, INVALIDSERIAL );
			}
			break;
		case 'I':
			if( cmd == "ITEMMENU" )
			{
				if( data.empty() )
				{
					return;
				}
				BuildAddMenuGump( s, static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0)) );
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
				{
					return;
				}
				Skills->NewMakeMenu( s,std::stoi(strutil::stripTrim( data ), nullptr, 0), (UI08)s->AddID() );
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
				{
					return;
				}
				UI16 newBody = static_cast<UI16>(std::stoul(data, nullptr, 0));
				mChar->SetID( newBody );
				mChar->SetOrgID( newBody );
			}
			break;
		case 'S':
			if( cmd == "SYSMESSAGE" )
			{
				if( data.empty() )
				{
					return;
				}
				s->sysmessage( data.c_str() );
			}
			else if( cmd == "SKIN" )
			{
				if( data.empty() )
				{
					return;
				}
				COLOUR newSkin = static_cast<UI16>(std::stoul(data, nullptr, 0));
				mChar->SetSkin( newSkin );
				mChar->SetOrgSkin( newSkin );
			}
			break;
		case 'V':
			if( cmd == "VERSION" )
			{
				s->sysmessage( "%s v%s(%s) [%s] Compiled by %s ", CVersionClass::GetProductName().c_str(), CVersionClass::GetVersion().c_str(), CVersionClass::GetBuild().c_str(), OS_STR, CVersionClass::GetName().c_str() );
			}
			break;
		case 'W':
			if( cmd == "WEBLINK" )
			{
				if( data.empty() )
				{
					return;
				}
				s->OpenURL( data );
			}
			break;
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void HandleAddMenuButton( CSocket *s, UI32 button )

//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles button pressed in add menu gump
//o-----------------------------------------------------------------------------------------------o
void HandleAddMenuButton( CSocket *s, UI32 button )
{
	SI32 addMenuLoc	= s->TempInt();
	std::string sect = std::string("ITEMMENU ") + strutil::number( addMenuLoc );
	ScriptSection *ItemMenu = FileLookup->FindEntry( sect, items_def );
	if( ItemMenu == nullptr )
		return;

	// If we get here we have to check to see if there are any other entryies added via the auto-addmenu code. Each item == 2 entries IE: IDNUM=Text name of Item, and ADDITEM=itemID to add
	std::pair<ADDMENUMAP_CITERATOR,ADDMENUMAP_CITERATOR> pairRange = g_mmapAddMenuMap.equal_range( addMenuLoc );
	UI32 autoAddMenuItemCount = 0;
	for(ADDMENUMAP_CITERATOR CI = pairRange.first;CI!=pairRange.second;++CI)
		autoAddMenuItemCount += 2;	// Need to inicrement by 2 because each entry is measured in the dfn' as two lines. Used in teh calculation below.
	// let's skip over the name, and get straight to where we should be headed
	size_t entryNum = ((static_cast<size_t>(button) - 6) * 2);
	autoAddMenuItemCount += static_cast<UI32>(ItemMenu->NumEntries());
	if( autoAddMenuItemCount >= entryNum )
	{
		std::string tag		= ItemMenu->MoveTo( entryNum );
		std::string data	= ItemMenu->GrabData();
		HandleGumpCommand( s, tag, data );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void HandleHowTo( CSocket *sock, SI32 cmdNumber )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles response when player enters HOWTO command
//o-----------------------------------------------------------------------------------------------o
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

	if( !found )
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


		auto filename = strutil::format( "help/commands/%s.txt", cmdName.c_str() );

		std::ifstream toOpen( filename );
		if( !toOpen.is_open() )
			CommandInfo.AddData( "", "No extra information is available about this command", 7 );
		else
		{
			char cmdLine[129];
			while( !toOpen.eof() && !toOpen.fail() )
			{
				toOpen.getline(cmdLine, 128);
				cmdLine[toOpen.gcount()] = 0;
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

void HandleHowToButton( CSocket *s, UI32 button )
{
	HandleHowTo( s, button - 2 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CPIGumpMenuSelect::Handle( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles button press in gumps
//o-----------------------------------------------------------------------------------------------o
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
		CChar *targChar = nullptr;
		if( buttonID == 1 && switchCount > 0 ) // Clicked on a players Virtue Gump icon
		{
			SERIAL targSer = tSock->GetDWord( 19 );
			targChar = calcCharObjFromSer( targSer );
		}
		else // Clicked an item on the virtue gump
			targChar = calcCharObjFromSer( id );

		cScript *toExecute = nullptr;
		std::vector<UI16> scriptTriggers = tSock->CurrcharObj()->GetScriptTriggers();
		for( auto scriptTrig : scriptTriggers )
		{
			toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				if( toExecute->OnVirtueGumpPress( tSock->CurrcharObj(), targChar, buttonID ) == 1 )
				{
					return true;
				}
			}
		}

		toExecute = JSMapping->GetScript( (UI16)0 ); // Global script
		if( toExecute != nullptr )
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
		if( toExecute != nullptr )
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
	// TBDU To Be Decided Upon
	// 1	Tweak Item
	// 2	Tweak Char
	// 3	Townstones
	// 4	WhoList
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

	CItem *j = nullptr;
	switch( gumpID )
	{
		case 3:	HandleTownstoneButton( tSock, buttonID, id, gumpID );			break;	// Townstones
		case 4:	WhoList->ButtonSelect( tSock, static_cast<UI16>(buttonID), static_cast<UI08>(gumpID) );					break;	// Wholist
		case 6:																		// Hair Dye Menu
			j = static_cast<CItem *>(tSock->TempObj());
			tSock->TempObj( nullptr );
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
		case 12: Skills->HandleMakeMenu( tSock, buttonID, static_cast<SI32>(tSock->AddID()));	break;	// New Make Menu
		case 13: HandleHowToButton( tSock, buttonID );							break;	// Howto
		default:
			break;
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void HandleTownstoneText( UI08 index )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles new values for townstones
//o-----------------------------------------------------------------------------------------------o
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
			amountToDonate = static_cast<UI32>(std::stoul(reply, nullptr, 0));
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CPIGumpInput::Handle()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles new values for gumps
//o-----------------------------------------------------------------------------------------------o
bool CPIGumpInput::Handle( void )
{
	switch( type )
	{
		case 3:		HandleTownstoneText( index );	break;
		case 20:
		{
			CChar *mChar = calcCharObjFromSer( id );
			if( ValidateObject( mChar ) )
			{
				CSocket *mSock = mChar->GetSocket();
				if( mSock != nullptr )
				{
					UI32 scriptNo = mSock->AddID();
					if( scriptNo >= 0xFFFF )
					{
						cScript *toExecute = JSMapping->GetScript( (scriptNo - 0xFFFF) );
						if( toExecute != nullptr )
							toExecute->HandleGumpInput( this );
						mSock->AddID( 0 );
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
	ScriptSection *sectionData = FileLookup->FindEntry( sect, menus_def );
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CPIGumpChoice::Handle( void )
//|	Modified	-	2/10/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles choice "Help" menu, Tracking Menu, and Polymorph menu
//o-----------------------------------------------------------------------------------------------o
bool CPIGumpChoice::Handle( void )
{
	std::string sect;
	std::string tag;
	std::string data;
	UI16 main		= tSock->GetWord( 5 );
	UI16 sub		= tSock->GetWord( 7 );
	CChar *mChar	= tSock->CurrcharObj();

	if( main >= POLYMORPHMENUOFFSET )
	{
		sect = std::string("POLYMORPHMENU ") + strutil::number( main );
		data = GrabMenuData( sect, (static_cast<size_t>(sub) * 2), tag );
		if( !data.empty() )
		{
			if( main == POLYMORPHMENUOFFSET )
			{
				Magic->PolymorphMenu( tSock, static_cast<UI16>(std::stoul(data, nullptr, 0)) );
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
				Magic->Polymorph( tSock, static_cast<UI16>(std::stoul(data,nullptr,0)) );
			}
		}
	}
	else if( main >= TRACKINGMENUOFFSET )
	{
		if( main == TRACKINGMENUOFFSET )
		{
			sect = std::string("TRACKINGMENU ") + strutil::number( main );
			data = GrabMenuData( sect, (static_cast<size_t>(sub) * 2), tag );
			if( !data.empty() && tag != "What" )
			{
				Skills->CreateTrackingMenu( tSock, static_cast<UI16>(std::stoul(data, nullptr, 0)) );
			}
		}
		else
		{
			if( !Skills->CheckSkill( mChar, TRACKING, 0, 1000 ) )
			{
				tSock->sysmessage( 575 );
				return true;
			}
			Skills->Tracking( tSock, static_cast<SI32>(sub - 1) );
		}
	}
	else if( main < ITEMMENUOFFSET ) // GM Menus
	{
		sect = std::string("GMMENU ") + strutil::number( main );
		data = GrabMenuData( sect, (static_cast<size_t>(sub) * 2), tag );
		if( !tag.empty() )
			HandleGumpCommand( tSock, tag, data );
	}
	return true;
}

void HandleCommonGump( CSocket *mSock, ScriptSection *gumpScript, UI16 gumpIndex )
{
	CChar *mChar = mSock->CurrcharObj();
	std::string line;
	UI16 modelID = 0;
	UI16 modelColour = 0;
	std::string tag = gumpScript->First();
	std::string data = gumpScript->GrabData();
	line = tag + " " + data;
	CPOpenGump toSend( *mChar );
	toSend.GumpIndex( gumpIndex );
	toSend.Question( line );
	for( tag = gumpScript->Next(); !gumpScript->AtEnd(); tag = gumpScript->Next() )
	{
		data = gumpScript->GrabData();
		modelID = static_cast<UI16>(stoul(tag, nullptr, 0));
		toSend.AddResponse( modelID, modelColour, data );
		tag = gumpScript->Next();
	}
	toSend.Finalize();
	mSock->Send( &toSend );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void GumpDisplay::AddData( GumpInfo *toAdd )
//|	Date		-	Unknown
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds the data to the gump to send
//o-----------------------------------------------------------------------------------------------o
void GumpDisplay::AddData( GumpInfo *toAdd )
{
	GumpInfo *gAdd		= new GumpInfo;
	gAdd->name			= toAdd->name;
	gAdd->value			= toAdd->value;
	gAdd->type			= toAdd->type;
	gAdd->stringValue	= toAdd->stringValue;
	gumpData.push_back( gAdd );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void GumpDisplay::AddData( std::string toAdd, UI32 value, UI08 type )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds the data to the gump to send
//o-----------------------------------------------------------------------------------------------o
void GumpDisplay::AddData( std::string toAdd, UI32 value, UI08 type )
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void GumpDisplay::AddData( std::string toAdd, std::string toSet, UI08 type )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds the data to the gump to send
//o-----------------------------------------------------------------------------------------------o
void GumpDisplay::AddData( std::string toAdd, const std::string &toSet, UI08 type )
{
	if( toAdd.empty() && type != 7 )
		return;

	GumpInfo *gAdd = new GumpInfo;
	gAdd->name = toAdd;
	gAdd->stringValue = toSet;
	gAdd->type = type;
	gumpData.push_back( gAdd );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	GumpDisplay::GumpDisplay( CSocket *target ) : toSendTo( target )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Begin GumpDisplay stuff by setting the target, clearing any existing data, and setting the w / h
//o-----------------------------------------------------------------------------------------------o
GumpDisplay::GumpDisplay( CSocket *target ) : toSendTo( target )
{
	gumpData.resize( 0 );
	width = 340;
	height = 320;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	GumpDisplay::GumpDisplay( CSocket *target, UI16 gumpWidth, UI16 gumpHeight ) :
//|						width( gumpWidth ), height( gumpHeight ), toSendTo( target )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Begin GumpDisplay stuff by setting the target, clearing any existing data, and setting the w / h
//o-----------------------------------------------------------------------------------------------o
GumpDisplay::GumpDisplay( CSocket *target, UI16 gumpWidth, UI16 gumpHeight ) :
width( gumpWidth ), height( gumpHeight ), toSendTo( target )
{
	gumpData.resize( 0 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	GumpDisplay::~GumpDisplay()
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Clear any existing GumpDisplay data
//o-----------------------------------------------------------------------------------------------o
GumpDisplay::~GumpDisplay()
{
	for( size_t i = 0; i < gumpData.size(); ++i )
		delete gumpData[i];
	Delete();
	gumpData.resize( 0 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void GumpDisplay::Delete( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Delete all GumpDisplay entries
//o-----------------------------------------------------------------------------------------------o
void GumpDisplay::Delete( void )
{
	one.resize( 0 );
	two.resize( 0 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void GumpDisplay::SetTitle( const std::string newTitle )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Set a gumps title
//o-----------------------------------------------------------------------------------------------o
void GumpDisplay::SetTitle( const std::string& newTitle )
{
	title = newTitle;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SendVecsAsGump( CSocket *sock, stringList& one, stringList& two, UI32 type, SERIAL serial )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends to socket sock the data in one and two.  One is control, two is data
//o-----------------------------------------------------------------------------------------------o
void SendVecsAsGump( CSocket *sock, STRINGLIST& one, STRINGLIST& two, UI32 type, SERIAL serial )
{
	CPSendGumpMenu toSend;
	toSend.GumpID( type );
	toSend.UserID( serial );

	size_t line = 0;
	for( line = 0; line < one.size(); ++line )
		toSend.addCommand( one[line] );

	for( line = 0; line < two.size(); ++line )
		toSend.addText( two[line] );

	toSend.Finalize();
	sock->Send( &toSend );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void GumpDisplay::Send( UI32 gumpNum, bool isMenu, SERIAL serial )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends gump data to the socket
//o-----------------------------------------------------------------------------------------------o
void GumpDisplay::Send( UI32 gumpNum, bool isMenu, SERIAL serial )
{
	constexpr auto maxsize = 512 ;
	std::string temp ;
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
	temp = strutil::format(maxsize, "resizepic 0 0 %i %i %i", cwmWorldState->ServerData()->BackgroundPic(), width, height );
	one.push_back( temp );
	temp = strutil::format(maxsize, "button %i 15 %i %i 1 0 1", width - 40, cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1 );
	one.push_back( temp );
	temp = strutil::format(maxsize, "text 45 15 %i 0", cwmWorldState->ServerData()->TitleColour() );
	one.push_back( temp );

	temp = strutil::format(maxsize, "page %u", pagenum );
	one.push_back( temp );

	if( title.length() == 0 ){
		temp = "General Gump";
	}
	else{
		 temp= title;
	}
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
			temp = strutil::format(maxsize, "page %u", pagenum );
			one.push_back( temp );
		}
		if( gumpData[i]->type != 7 )
		{
			temp = strutil::format(maxsize, "text 50 %u %i %u", position, cwmWorldState->ServerData()->LeftTextColour(), linenum++ );
			one.push_back( temp );
			if( isMenu )
			{
				temp = strutil::format(maxsize, "button 20 %u %i %i 1 0 %u", position, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, buttonnum );
				one.push_back( temp );
			}
			temp = strutil::format(maxsize, "text %i %u %i %u", (width / 2) + 10, position, cwmWorldState->ServerData()->RightTextColour(), linenum++ );
			one.push_back( temp );
			two.push_back( gumpData[i]->name );
		}
		else
		{
			temp = strutil::format(maxsize, "text 30 %u %i %u", position, cwmWorldState->ServerData()->LeftTextColour(), linenum++ );
			one.push_back( temp );
		}

		UI32 value = gumpData[i]->value;
		switch( gumpData[i]->type )
		{
			case 0:		// all numbers of sorts
				temp = std::to_string( value );
				break;
			case 1:
				temp = strutil::format( "%x", static_cast<UI32>(value) );
				break;
			case 2:
				ser1 = (UI08)(value>>24);
				ser2 = (UI08)(value>>16);
				ser3 = (UI08)(value>>8);
				ser4 = (UI08)(value%256);
				temp = strutil::format( "%i %i %i %i", ser1, ser2, ser3, ser4 );
				break;
			case 3:
				ser1 = (UI08)(value>>24);
				ser2 = (UI08)(value>>16);
				ser3 = (UI08)(value>>8);
				ser4 = (UI08)(value%256);
				temp = strutil::format( "%x %x %x %x", ser1, ser2, ser3, ser4 );
				break;
			case 4:
				if( gumpData[i]->stringValue.empty() ){
					temp =  "nullptr POINTER" ;
				}
				else{
					temp= gumpData[i]->stringValue;
				}
				if( temp.size() > stringWidth )	// too wide for one line, CRAP!
				{

					std::string temp2;
					std::string temp3;

					size_t tempWidth =  temp.size() - stringWidth;
					temp2 = temp.substr(0,stringWidth) ;

					two.push_back( temp2 );
					for( UI32 tempCounter = 0; tempCounter < tempWidth / ( static_cast<size_t>(stringWidth) * 2 ) + 1; ++tempCounter )
					{
						//LOOKATME
						position += 20;
						++lineForButton;
						temp3 = strutil::format( "text %i %u %i %u", 30, position, cwmWorldState->ServerData()->RightTextColour(), linenum++ );
						one.push_back( temp3 );
						auto remaining = std::min<std::size_t>(( temp.size() - ( static_cast<size_t>(tempCounter) + 1 ) * static_cast<size_t>(stringWidth) * 2), static_cast<size_t>(stringWidth) * 2 );

						temp2 = temp.substr( static_cast<size_t>(stringWidth) + static_cast<size_t>(tempCounter) * static_cast<size_t>(stringWidth) * 2, remaining );
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
				temp = strutil::format( "0x%02x%02x", ser1, ser2 );
				break;
			case 6:
				ser1 = (UI08)(value>>8);
				ser2 = (UI08)(value%256);
				temp = strutil::format("%i %i", ser1, ser2 );
				break;
			case 7:
				if( gumpData[i]->stringValue.empty() ){
					temp = "Null Pointer";
				}
				else{
					temp = gumpData[i]->stringValue;
				}
				SI32 sWidth;
				sWidth = stringWidth * 2;
				if( temp.size() > static_cast<size_t>(sWidth) )	// too wide for one line, CRAP!
				{
					std::string temp2 ;
					std::string temp3 ;
					size_t tempWidth = temp.size() - sWidth;
					temp2= temp.substr(0, sWidth );

					two.push_back( temp2 );
					for( UI32 tempCounter = 0; tempCounter < tempWidth / sWidth + 1; ++tempCounter )
					{
						position += 20;
						++lineForButton;
						temp3 = strutil::format(512, "text %i %u %i %u", 30, position, cwmWorldState->ServerData()->LeftTextColour(), linenum++ );
						one.push_back( temp3 );
						auto remaining = std::min<std::size_t>(temp.size() - ( static_cast<size_t>(tempCounter) + 1 ) * static_cast<size_t>(sWidth), static_cast<std::size_t>( sWidth ));

						temp2= temp.substr(( static_cast<size_t>(tempCounter) + 1 ) * static_cast<size_t>(sWidth), remaining );

						two.push_back( temp2 );
					}
					// be stupid for the moment and do no text wrapping over pages
				}
				else
					two.push_back( temp );
				break;
			case 8:
				if( gumpData[i]->stringValue.empty() ){
					temp =  "0.00" ;
				}
				else{

					temp = strutil::format( "%.2f",  std::stof(gumpData[i]->stringValue ) );
				}
				break;
			default:
				temp = std::to_string( value) ;
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
		temp =  "page "+std::to_string( pagenum );
		one.push_back( temp );
		if( i >= 10 )
		{
			temp = strutil::format( "button 10 %i %i %i 0 %u", height - 40, cwmWorldState->ServerData()->ButtonLeft(), cwmWorldState->ServerData()->ButtonLeft() + 1, pagenum-1 ); //back button
			one.push_back( temp );
		}
		if( lineForButton > numToPage && static_cast<UI32>(( i + numToPage )) < lineForButton )
		{
			temp = strutil::format( "button %i %i %i %i 0 %u", width - 40, height - 40, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum+1 ); //forward button
			one.push_back( temp );
		}
		++pagenum;
	}
	SendVecsAsGump( toSendTo, one, two, gumpNum, serial );
}
