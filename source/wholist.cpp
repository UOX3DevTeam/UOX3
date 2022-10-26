#include "uox3.h"
#include "wholist.h"
#include "network.h"
#include "CPacketSend.h"
#include "Dictionary.h"
#include "jail.h"
#include "commands.h"


CWhoList *WhoList;
CWhoList *OffList;

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWhoList::ResetUpdateFlag()
//|	Date		-	12th February, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Disables the WhoList update flag
//o------------------------------------------------------------------------------------------------o
void CWhoList::ResetUpdateFlag( void )
{
	needsUpdating = false;
}

//o------------------------------------------------------------------------------------------------o
//|	Class		-	CWhoList::CWhoList()
//|	Date		-	12th February, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Initializes the variables of the wholist on creation
//o------------------------------------------------------------------------------------------------o
CWhoList::CWhoList( bool trulyOnline )
{
	FlagUpdate();					// first use will force it to update
	gmCount = 0;					// no GMs currently using it
	whoMenuData.resize( 0 );
	online = trulyOnline;
}

CWhoList::~CWhoList()
// PRE:		WhoList class is active
// POST:	Memory cleared/destroyed
// DATE:	12th February, 2000
{
	ResetUpdateFlag();
	gmCount = 0;
	Delete();
}
//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWhoList::ResetUpdateFlag()
//|	Date		-	12th February, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Flags the WhoList as needing to be updated
//o------------------------------------------------------------------------------------------------o
void CWhoList::FlagUpdate( void )
{
	needsUpdating = true;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWhoList::SendSocket()
//|	Date		-	12th February, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends the WhoList to a player's socket
//o------------------------------------------------------------------------------------------------o
void CWhoList::SendSocket( CSocket *toSendTo )
{
	GMEnter();
	if( needsUpdating && gmCount <= 1 )	// If we need to update, do it now while we still can
	{
		Update();
	}

	if( online )
	{
		SendVecsAsGump( toSendTo, one, two, 4, INVALIDSERIAL );
	}
	else
	{
		SendVecsAsGump( toSendTo, one, two, 11, INVALIDSERIAL );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWhoList::GMEnter()
//|	Date		-	12th February, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Increments the GM counter
//o------------------------------------------------------------------------------------------------o
void CWhoList::GMEnter( void )
{
	++gmCount;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWhoList::GMLeave()
//|	Date		-	12th February, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Decrements the GM counter
//o------------------------------------------------------------------------------------------------o
void CWhoList::GMLeave( void )
{
	if( gmCount != 0 )
	{
		--gmCount;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWhoList::Delete()
//|	Date		-	12th February, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Deletes all the entries in the WhoList
//o------------------------------------------------------------------------------------------------o
void CWhoList::Delete( void )
{
	one.resize( 0 );
	two.resize( 0 );
	whoMenuData.resize( 0 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWhoList::AddSerial()
//|	Date		-	23rd February, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds an entry into the whoMenuData array
//o------------------------------------------------------------------------------------------------o
void CWhoList::AddSerial( SERIAL toAdd )
{
	whoMenuData.push_back( toAdd );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWhoList::ButtonSelect()
//|	Date		-	23rd February, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Does action based upon button selected
//o------------------------------------------------------------------------------------------------o
void CWhoList::ButtonSelect( CSocket *toSendTo, UI16 buttonPressed, UI08 type )
{
	CChar *sourceChar = toSendTo->CurrcharObj();

	if( buttonPressed < 200 )
	{
		if( buttonPressed == 0 )
			return;

		buttonPressed -= 7;
		toSendTo->AddId( buttonPressed );
		Command( toSendTo, type, buttonPressed );
		return;
	}

	GMLeave();
	SERIAL ser = toSendTo->AddId();
	if( ser == INVALIDSERIAL )
	{
		toSendTo->SysMessage( 1387 ); // Selected character not found.
		return;
	}

	SERIAL charSerial = whoMenuData[ser];

	CChar *targetChar = CalcCharObjFromSer( charSerial ); // find selected char ...
	if( !ValidateObject( targetChar ))
	{
		toSendTo->SysMessage( 1387 ); // Selected character not found.
		return;
	}
	CSocket *trgSock = nullptr;
	switch( buttonPressed )
	{
		case 200: // gochar
			if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
			{
				toSendTo->SysMessage( 1388 ); // You cannot go to such powerful beings.
				return;
			}
			if( sourceChar->WorldNumber() != targetChar->WorldNumber() )
			{
				sourceChar->SetLocation( targetChar );
				SendMapChange( targetChar->WorldNumber(), toSendTo );
			}
			else
			{
				sourceChar->SetLocation( targetChar );
			}
			break;
		case 201: // xtele
			if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
			{
				toSendTo->SysMessage( 1389 ); // Such powerful beings don't come on your whim!
				return;
			}
			if( !targetChar->IsNpc() && targetChar->WorldNumber() != sourceChar->WorldNumber() )
			{
				targetChar->SetLocation( sourceChar );
				trgSock = targetChar->GetSocket();
				SendMapChange( sourceChar->WorldNumber(), trgSock );
			}
			else
			{
				targetChar->SetLocation( sourceChar );
			}
			break;
		case 202: // jail char
			if( sourceChar == targetChar )
			{
				toSendTo->SysMessage( 1390 ); // You cannot jail yourself!
				break;
			}
			else
			{
				if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
				{
					toSendTo->SysMessage( 1391 ); // You cannot jail someone superior to you.
					return;
				}
				if( targetChar->IsJailed() )
				{
					toSendTo->SysMessage( 1070 ); // That player is already in jail!
				}

				else if( !JailSys->JailPlayer( targetChar, 0xFFFFFFFF ))
				{
					toSendTo->SysMessage( 1072 ); // All jails are currently full!
				}
				break;
			}
		case 203: // release
			if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
			{
				toSendTo->SysMessage( 1392 ); // You release someone more powerful than you.
				return;
			}
			if( !targetChar->IsJailed() )
			{
				toSendTo->SysMessage( 1064 ); // That player is not in jail!
			}
			else
			{
				JailSys->ReleasePlayer( targetChar );
				toSendTo->SysMessage( 1065, targetChar->GetName().c_str() ); // Player %s released.
			}
			break;
		case 204: // kick player
			if( targetChar == sourceChar )
			{
				toSendTo->SysMessage( 1393 ); // You cannot kick yourself.
				break;
			}
			else
			{
				if( !IsOnline(( *targetChar )))	// local var overloads it
				{
					toSendTo->SysMessage( 1394 ); // You can't kick an offline player.
					break;
				}
				if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
				{
					toSendTo->SysMessage( 1395 ); // You cannot kick someone more powerful than you.
					return;
				}
				toSendTo->SysMessage( 1396 ); // Kicking player.
				Network->Disconnect( targetChar->GetSocket() );
				break;
			}
		case 205:
			// make it prettier later!
			if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
			{
				toSendTo->SysMessage( 1397 ); // You cannot spy on your betters.
				return;
			}
			toSendTo->SysMessage( "X: %i Y: %i Z: %i World: %u", targetChar->GetX(), targetChar->GetY(), targetChar->GetZ(), targetChar->WorldNumber() );
			break;
		case 206: // remote cstats
			if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
			{
				toSendTo->SysMessage( 1398 ); // You have not the right to see the intimate details of your betters!
				return;
			}
			toSendTo->SetDWord( 7, targetChar->GetSerial() );
			// Trigger scripted command
			Commands->Command( toSendTo, targetChar, "cstats", true );
			break;
		case 207: // remote tweak
			if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
			{
				toSendTo->SysMessage( 1399 ); // You certainly cannot adjust those that are your betters!
				return;
			}
			toSendTo->SetDWord( 7, targetChar->GetSerial() );
			// Trigger scripted command
			Commands->Command( toSendTo, targetChar, "tweak", true );
			break;
		default:
			Console.Error( " Fallout of switch statement without default. wholist.cpp, ButtonSelect()" );
			break;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWhoList::Command()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Builds the WhoList gump and sends it to the player
//o------------------------------------------------------------------------------------------------o
void CWhoList::Command( CSocket *toSendTo, UI08 type, UI16 buttonPressed )
{
	SERIAL serial = whoMenuData[buttonPressed];
	CChar *targetChar = CalcCharObjFromSer( serial ); // find selected char ...

	if( !ValidateObject( targetChar ))
	{
		toSendTo->SysMessage( 1387 ); // Selected character not found.
		return;
	}

	CPSendGumpMenu toSend;
	toSend.UserId( INVALIDSERIAL );
	toSend.GumpId( type );

	UnicodeTypes sLang	= toSendTo->Language();
	UI16 lColour		= cwmWorldState->ServerData()->LeftTextColour();
	UI16 tColour		= cwmWorldState->ServerData()->TitleColour();
	UI16 butRight		= cwmWorldState->ServerData()->ButtonRight();
	//--static pages
	toSend.addCommand( "page 0" );
	toSend.addCommand( oldstrutil::format( "resizepic 0 0 %u 260 340", cwmWorldState->ServerData()->BackgroundPic() ));
	toSend.addCommand( oldstrutil::format( "button 30 300 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1 )); // OKAY
	toSend.addCommand( oldstrutil::format( "text 30 40 %u 0", tColour ));	// text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	toSend.addCommand( oldstrutil::format( "text 30 60 %u 1", tColour) );
	toSend.addCommand( "page 1" );

	toSend.addCommand( oldstrutil::format( "text 50 90 %u 2", lColour) );	// goto text
	toSend.addCommand( oldstrutil::format( "text 50 110 %u 3", lColour) );	// gettext
	toSend.addCommand( oldstrutil::format( "text 50 130 %u 4", lColour ));	// Jail text
	toSend.addCommand( oldstrutil::format( "text 50 150 %u 5", lColour ));	// Release text
	toSend.addCommand( oldstrutil::format( "text 50 170 %u 6", lColour ));	// Kick user text
	toSend.addCommand( oldstrutil::format( "text 50 190 %u 7", lColour ));
	toSend.addCommand( oldstrutil::format( "text 50 210 %u 8", lColour ));
	toSend.addCommand( oldstrutil::format( "text 50 230 %u 9", lColour) );
	toSend.addCommand( oldstrutil::format( "text 50 270 %u 10", lColour ));
	toSend.addCommand( oldstrutil::format( "button 20 90 %u %i 1 0 200", butRight, butRight + 1 ));		// goto button
	toSend.addCommand( oldstrutil::format( "button 20 110 %u %i 1 0 201", butRight, butRight + 1 ));	// get button
	toSend.addCommand( oldstrutil::format( "button 20 130 %u %i 1 0 202", butRight, butRight + 1 ));	// Jail button
	toSend.addCommand( oldstrutil::format( "button 20 150 %u %i 1 0 203", butRight, butRight + 1 ));	// Release button
	toSend.addCommand( oldstrutil::format( "button 20 170 %u %i 1 0 204", butRight, butRight + 1 ));	// kick button
	toSend.addCommand( oldstrutil::format( "button 20 190 %u %i 1 0 205", butRight, butRight + 1 ));	// Where button
	toSend.addCommand( oldstrutil::format( "button 20 210 %u %i 1 0 206", butRight, butRight + 1 ));	// Cstats button
	toSend.addCommand( oldstrutil::format( "button 20 230 %u %i 1 0 207", butRight, butRight + 1 ));	// Tweak button

	toSend.addText(oldstrutil::format( "User %u selected (account %u)", buttonPressed, targetChar->GetAccount().wAccountIndex));
	toSend.addText( oldstrutil::format( "Name: %s", targetChar->GetName().c_str() ));
	toSend.addText( Dictionary->GetEntry( 1400, sLang ));
	toSend.addText( Dictionary->GetEntry( 1401, sLang ));
	toSend.addText( Dictionary->GetEntry( 1402, sLang ));
	toSend.addText( Dictionary->GetEntry( 1403, sLang ));
	toSend.addText( Dictionary->GetEntry( 1404, sLang ));
	toSend.addText( Dictionary->GetEntry( 1405, sLang ));
	toSend.addText( Dictionary->GetEntry( 1406, sLang ));
	toSend.addText( Dictionary->GetEntry( 1407, sLang ));
	toSend.addText( oldstrutil::format( "Serial#[%x %x %x %x]", targetChar->GetSerial( 1 ), targetChar->GetSerial( 2 ), targetChar->GetSerial( 3 ), targetChar->GetSerial( 4 )));

	toSend.Finalize();
	toSendTo->Send( &toSend );
}

void CWhoList::ZeroWho( void )
{
	FlagUpdate();
	gmCount = 0;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CWhoList::Update()
//|	Date		-	12th February, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	If multiple GMs using, doesn't actually update (deferred update)
//|						Otherwise updates the arrays, and resets the update flag
//|
//|	Changes		-	1.1 June 29th, 2000
//|						Written to use SendVecsAsGump, as well as making it flexible enough to show offline players
//o------------------------------------------------------------------------------------------------o
void CWhoList::Update( void )
{
	if( gmCount > 1 )
	{
		return;
	}

	Delete();
	constexpr UI16 maxsize = 512;

	size_t i				= 0;
	const UI32 numPerPage	= 13;
	UI32 pagenum = 1, position = 40, linenum = 1, buttonnum = 7;

	size_t k		= cwmWorldState->GetPlayersOnline();
	SI32 realCount	= 1;

	//--static pages
	one.push_back( "page 0"  );
	one.push_back( oldstrutil::format( maxsize, "resizepic 0 0 %u 320 340", cwmWorldState->ServerData()->BackgroundPic() ));
	one.push_back( oldstrutil::format( maxsize,"button 250 15 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1 ));
	one.push_back( oldstrutil::format( maxsize, "text 45 15 %u 0", cwmWorldState->ServerData()->TitleColour() ));
	one.push_back( oldstrutil::format( maxsize, "page %u", pagenum ));
	two.push_back( "Wholist" );

	//--Start User List
	if( online )
	{
		for( i = 0; i < k; ++i )
		{
			CSocket *tSock	= Network->GetSockPtr( static_cast<UI32>( i ));
			CChar *tChar	= tSock->CurrcharObj();
			if( !ValidateObject( tChar ))
				continue;

			if( realCount > 0 && !( realCount % numPerPage ))
			{
				position = 40;
				++pagenum;

				one.push_back( oldstrutil::format( maxsize, "page %u", pagenum ));
			}

			one.push_back( oldstrutil::format( maxsize, "text 50 %u %u %u", position, cwmWorldState->ServerData()->LeftTextColour(), linenum ));
			one.push_back( oldstrutil::format( maxsize,"button 20 %u %u %i %u 0 %u", position, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum, buttonnum ));
			two.push_back( oldstrutil::format( maxsize, " %s", tChar->GetName().c_str() ));
			AddSerial( tChar->GetSerial() );

			position += 20;
			++linenum;
			++buttonnum;
			++realCount;
		}

		two[0] = oldstrutil::format( maxsize, "Wholist [%i online]", realCount - 1 );

		pagenum = 1;
		k = realCount;
		for( i = 0; i < k; i += numPerPage )
		{
			one.push_back( oldstrutil::format( maxsize, "page %u", pagenum ));
			if( i >= numPerPage )
			{
				// back button
				one.push_back( oldstrutil::format( maxsize, "button 50 300 %u %i 0 %i", cwmWorldState->ServerData()->ButtonLeft(), cwmWorldState->ServerData()->ButtonLeft() + 1, pagenum - 1 ));
			}
			if(( k > numPerPage ) && (( i + numPerPage ) < k ))
			{
				one.push_back( oldstrutil::format( maxsize, "button 260 300 %u %i 0 %i", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum + 1 ));
			}
			++pagenum;
		}
	}
	else
	{
		MAPUSERNAMEID_ITERATOR I;
		CChar *ourChar = nullptr;
		for( I = Accounts->Begin(); I != Accounts->End(); ++I )
		{
			CAccountBlock_st& actbSearch = I->second;
			for( i = 0; i < 7; ++i )
			{
				ourChar = actbSearch.lpCharacters[i];
				if( ValidateObject( ourChar ))
				{
					if( !IsOnline(( *ourChar )))
					{
						if( realCount > 0 && !( realCount % numPerPage ))
						{
							position = 40;
							++pagenum;

							one.push_back( oldstrutil::format( maxsize,"page %u", pagenum ));
						}

						one.push_back( oldstrutil::format( maxsize, "text 50 %u %u %u", position, cwmWorldState->ServerData()->LeftTextColour(), linenum ));
						one.push_back( oldstrutil::format( maxsize,"button 20 %u %u %i %u 0 %u", position, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum, buttonnum ));
						two.push_back( oldstrutil::format( maxsize," %s (%s)", ourChar->GetName().c_str(), ourChar->GetLastOn().c_str() ));
						AddSerial( ourChar->GetSerial() );

						position += 20;
						++linenum;
						++buttonnum;
						++realCount;
					}
				}
			}
		}

		two[0] = oldstrutil::format( maxsize, "Wholist [%i offline]", realCount - 1 );

		pagenum = 1;
		k = realCount;
		for( i = 0; i < k; i += numPerPage )
		{
			one.push_back( oldstrutil::format( maxsize, "page %u", pagenum ));
			if( i >= numPerPage )
			{
				 //back button
				one.push_back( oldstrutil::format( maxsize, "button 50 300 %u %i 0 %i", cwmWorldState->ServerData()->ButtonLeft(), cwmWorldState->ServerData()->ButtonLeft() + 1, pagenum - 1 ));
			}
			if(( k > numPerPage ) && (( i + numPerPage ) < k ))
			{
				one.push_back( oldstrutil::format( maxsize, "button 260 300 %u %i 0 %i", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum + 1 ));
			}
			++pagenum;
		}
	}
	ResetUpdateFlag();
}
