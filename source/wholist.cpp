#include "uox3.h"
#include "wholist.h"
#include "network.h"
#include "CPacketSend.h"
#include "Dictionary.h"
#include "jail.h"

namespace UOX
{

cWhoList *WhoList;
cWhoList *OffList;

void	TweakTarget( CSocket *s );

void cWhoList::ResetUpdateFlag( void )
// PRE:		WhoList class is active
// POST:	Resets the update flag
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	needsUpdating = false;
}

cWhoList::cWhoList( bool trulyOnline )
// PRE:		On creation
// POST:	Initializes the variables of the wholist
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	FlagUpdate();					// first use will force it to update
	gmCount = 0;					// no GMs currently using it
	whoMenuData.resize( 0 );
	online = trulyOnline;
}

cWhoList::~cWhoList()
// PRE:		WhoList class is active
// POST:	Memory cleared/destroyed
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	ResetUpdateFlag();
	gmCount = 0;
	Delete();
}
void cWhoList::FlagUpdate( void )
// PRE:		WhoList class is active
// POST:	Flags the who list as needing to be updated
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	needsUpdating = true;
}
void cWhoList::SendSocket( CSocket *toSendTo )
// PRE:		WhoList class is active
// POST:	Sends the wholist to the socket
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	GMEnter();
	if( needsUpdating && gmCount <= 1 )	// If we need to update, do it now while we still can
		Update();

	if( online )
		SendVecsAsGump( toSendTo, one, two, 4, INVALIDSERIAL );
	else
		SendVecsAsGump( toSendTo, one, two, 11, INVALIDSERIAL );
}

void cWhoList::GMEnter( void )
// PRE:		WhoList class is active
// POST:	Increments the GM counter
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	++gmCount;
}

void cWhoList::GMLeave( void )
// PRE:		WhoList class is active
// POST:	Decrements the GM counter
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	if( gmCount != 0 )
		--gmCount;
}

void cWhoList::Delete( void )
// PRE:		WhoList class is active
// POST:	Deletes all the entries
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	one.resize( 0 );
	two.resize( 0 );
	whoMenuData.resize( 0 );

}

void cWhoList::AddSerial( SERIAL toAdd )
// PRE:		WhoList class is active
// POST:	Adds an entry into the whoMenuData array
// CODER:	Abaddon
// DATE:	23rd February, 2000
{
	whoMenuData.push_back( toAdd );
}

void cWhoList::ButtonSelect( CSocket *toSendTo, UI16 buttonPressed, UI08 type )
// PRE:		WhoList class is active
// POST:	Does action based upon button selected
// CODER:	Abaddon
// DATE:	23rd February, 2000
{
	CChar *sourceChar = toSendTo->CurrcharObj();

	if( buttonPressed < 200 )
	{		
		buttonPressed -= 7;
		toSendTo->AddID( buttonPressed );
		Command( toSendTo, type, buttonPressed );
		return;
	}

	GMLeave();
	SERIAL ser = toSendTo->AddID();
	if( ser == INVALIDSERIAL )
	{
		toSendTo->sysmessage( 1387 );
		return;
	}

	SERIAL charSerial = whoMenuData[ser];

	CChar *targetChar = calcCharObjFromSer( charSerial ); // find selected char ...
	if( !ValidateObject( targetChar ) )
	{
		toSendTo->sysmessage( 1387 );
		return;
	}
	CSocket *trgSock = NULL;
	switch( buttonPressed )
	{
		case 200://gochar
			if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
			{
				toSendTo->sysmessage( 1388 );
				return;
			}
			if( sourceChar->WorldNumber() != targetChar->WorldNumber() )
			{
				sourceChar->SetLocation( targetChar );
				SendMapChange( targetChar->WorldNumber(), toSendTo );
			}
			else
				sourceChar->SetLocation( targetChar );
			break;
		case 201://xtele
			if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
			{
				toSendTo->sysmessage( 1389 );
				return;
			}
			if( !targetChar->IsNpc() && targetChar->WorldNumber() != sourceChar->WorldNumber() )
			{
				targetChar->SetLocation( sourceChar );
				trgSock = targetChar->GetSocket();
				SendMapChange( sourceChar->WorldNumber(), trgSock );
			}
			else
				targetChar->SetLocation( sourceChar );
			break;
		case 202://jail char
			if( sourceChar == targetChar )
			{
				toSendTo->sysmessage( 1390 );
				break;
			}
			else
			{
				if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
				{
					toSendTo->sysmessage( 1391 );
					return;
				}
				if( targetChar->IsJailed() )
					toSendTo->sysmessage( 1070 );

				else if( !JailSys->JailPlayer( targetChar, 0xFFFFFFFF ) )
					toSendTo->sysmessage( 1072 );
				break;
			}						 
		case 203://release
			if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
			{
				toSendTo->sysmessage( 1392 );
				return;
			}
			if( !targetChar->IsJailed() )
				toSendTo->sysmessage( 1064 );
			else
			{
				JailSys->ReleasePlayer( targetChar );
				toSendTo->sysmessage( 1065, targetChar->GetName().c_str() );
			}
			break;
		case 204:
			if( targetChar == sourceChar )
			{
				toSendTo->sysmessage( 1393 );
				break;
			}
			else
			{
				if( !UOX::isOnline( (*targetChar) ) )	// local var overloads it
				{
					toSendTo->sysmessage( 1394 );		// you realize the break isn't necessary?
					break;
				}
				if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
				{
					toSendTo->sysmessage( 1395 );
					return;
				}
				toSendTo->sysmessage( 1396 );
				Network->Disconnect( targetChar->GetSocket() );
				break;
			}
		case 205:	// not active currently, a remote where
			// make it prettier later!
			if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
			{
				toSendTo->sysmessage( 1397 );
				return;
			}
			toSendTo->sysmessage( "X: %i Y: %i Z: %i World: %u", targetChar->GetX(), targetChar->GetY(), targetChar->GetZ(), targetChar->WorldNumber() );
			break;
		case 206:	// remote cstats
			if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
			{
				toSendTo->sysmessage( 1398 );
				return;
			}
			toSendTo->SetDWord( 7, targetChar->GetSerial() );
#if defined( _MSC_VER )
#pragma note( "Need to link this to the JS CStats Gump" )
#endif
			//CstatsTarget( toSendTo );
			break;
		case 207:	// remote tweak
			if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
			{
				toSendTo->sysmessage( 1399 );
				return;
			}
			toSendTo->SetDWord( 7, targetChar->GetSerial() );
			TweakTarget( toSendTo );
			break;
		default:
			Console.Error( 2, " Fallout of switch statement without default. wholist.cpp, ButtonSelect()" );
			break;
	}
}

void cWhoList::Command( CSocket *toSendTo, UI08 type, UI16 buttonPressed )
{
	SERIAL serial = whoMenuData[buttonPressed];
	CChar *targetChar = calcCharObjFromSer( serial ); // find selected char ...

	if( !ValidateObject( targetChar ) )
	{
		toSendTo->sysmessage( 1387 );
		return;
	}
	
	CPSendGumpMenu toSend;
	toSend.UserID( INVALIDSERIAL );
	toSend.GumpID( type );

	UnicodeTypes sLang	= toSendTo->Language();
	UI16 lColour		= cwmWorldState->ServerData()->LeftTextColour();
	UI16 tColour		= cwmWorldState->ServerData()->TitleColour();
	UI16 butRight		= cwmWorldState->ServerData()->ButtonRight();
	//--static pages
	toSend.AddCommand( "nomove" );
	toSend.AddCommand( "noclose" );
	toSend.AddCommand( "page 0" );
	toSend.AddCommand( "resizepic 0 0 %u 260 340", cwmWorldState->ServerData()->BackgroundPic() );
	toSend.AddCommand( "button 30 300 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1); //OKAY
	toSend.AddCommand( "text 30 40 %u 0", tColour );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	toSend.AddCommand( "text 30 60 %u 1", tColour );
	toSend.AddCommand( "page 1" );

	toSend.AddCommand( "text 50 90 %u 2", lColour );	//goto text
	toSend.AddCommand( "text 50 110 %u 3", lColour );	//gettext
	toSend.AddCommand( "text 50 130 %u 4", lColour );	//Jail text
	toSend.AddCommand( "text 50 150 %u 5", lColour );	//Release text
	toSend.AddCommand( "text 50 170 %u 6", lColour );	//Kick user text
	toSend.AddCommand( "text 50 190 %u 7", lColour );
	toSend.AddCommand( "text 50 210 %u 8", lColour );
	toSend.AddCommand( "text 50 230 %u 9", lColour );
	toSend.AddCommand( "text 50 270 %u 10", lColour );
	toSend.AddCommand( "button 20 90 %u %i 1 0 200", butRight, butRight + 1 ); //goto button
	toSend.AddCommand( "button 20 110 %u %i 1 0 201", butRight, butRight + 1 ); //get button
	toSend.AddCommand( "button 20 130 %u %i 1 0 202", butRight, butRight + 1 ); //Jail button
	toSend.AddCommand( "button 20 150 %u %i 1 0 203", butRight, butRight + 1 ); //Release button
	toSend.AddCommand( "button 20 170 %u %i 1 0 204", butRight, butRight + 1 ); //kick button
	toSend.AddCommand( "button 20 190 %u %i 1 0 205", butRight, butRight + 1 ); //Where button
	toSend.AddCommand( "button 20 210 %u %i 1 0 206", butRight, butRight + 1 ); //Cstats button
	toSend.AddCommand( "button 20 230 %u %i 1 0 207", butRight, butRight + 1 ); //Tweak button

	toSend.AddText( "User %u selected (account %u)",buttonPressed, targetChar->GetAccount().wAccountIndex);
	toSend.AddText( "Name: %s", targetChar->GetName().c_str() );   
	toSend.AddText( Dictionary->GetEntry( 1400, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1401, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1402, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1403, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1404, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1405, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1406, sLang ) );
	toSend.AddText( Dictionary->GetEntry( 1407, sLang ) );
	toSend.AddText( "Serial#[%x %x %x %x]", targetChar->GetSerial( 1 ), targetChar->GetSerial( 2 ), targetChar->GetSerial( 3 ), targetChar->GetSerial( 4 ) );   

	toSend.Finalize();
	toSendTo->Send( &toSend );
}

void cWhoList::ZeroWho( void )
{
	FlagUpdate();
	gmCount = 0;
}

void cWhoList::Update( void )
// PRE:		WhoList class is active
// POST:	If multiple GMs using, doesn't actually update (deferred update)
//			Otherwise updates the arrays, and resets the update flag
// CODER:	Abaddon
// DATE:	12th February, 2000
// REVISION 1.1 June 29th, 2000
//				Written to use SendVecsAsGump, as well as making it flexible enough to show offline players
{
	if( gmCount > 1 )
	{
		return;
	}
	// do code
	Delete();

	char temp[512];
	size_t i				= 0;
	const UI32 numPerPage	= 13;
	UI32 pagenum = 1, position = 40, linenum = 1, buttonnum = 7;
	
	size_t k		= cwmWorldState->GetPlayersOnline();
	int realCount	= 1;
	//--static pages
	one.push_back( "noclose" );
	one.push_back( "page 0"  );
	sprintf( temp, "resizepic 0 0 %u 320 340", cwmWorldState->ServerData()->BackgroundPic() );
	one.push_back( temp );
	sprintf( temp, "button 250 15 %u %i 1 0 1", cwmWorldState->ServerData()->ButtonCancel(), cwmWorldState->ServerData()->ButtonCancel() + 1 );
	one.push_back( temp );
	sprintf( temp, "text 45 15 %u 0", cwmWorldState->ServerData()->TitleColour() );
	one.push_back( temp );

	sprintf( temp, "page %lu", pagenum );
	one.push_back( temp );

	sprintf( temp, "Wholist" );
	two.push_back( temp );

	//--Start User List
	if( online )
	{
		for( i = 0; i < k; ++i )
		{
			CSocket *tSock	= Network->GetSockPtr( i );
			CChar *tChar	= tSock->CurrcharObj();
			if( !ValidateObject( tChar ) )
				continue;
			if( realCount > 0 && !(realCount%numPerPage) )
			{
				position = 40;
				++pagenum;
				sprintf(temp, "page %lu", pagenum );
				one.push_back( temp );
			}
			sprintf( temp, "text 50 %lu %u %lu", position, cwmWorldState->ServerData()->LeftTextColour(), linenum ); 
			one.push_back( temp );
			sprintf( temp, "button 20 %lu %u %i %lu 0 %lu", position, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum, buttonnum );
			one.push_back( temp );
			sprintf( temp, " %s", tChar->GetName().c_str() );
			two.push_back( temp );
			AddSerial( tChar->GetSerial() );

			position += 20;
			++linenum;
			++buttonnum;
			++realCount;
		}
		
		sprintf( temp, "Wholist [%i online]", realCount - 1 );
		two[0] = temp;

		pagenum = 1; 
		k = realCount;
		for( i = 0; i < k; i += numPerPage )
		{
			sprintf( temp, "page %lu", pagenum );
			one.push_back( temp );
			if( i >= numPerPage )
			{
				sprintf( temp, "button 50 300 %u %i 0 %li", cwmWorldState->ServerData()->ButtonLeft(), cwmWorldState->ServerData()->ButtonLeft() + 1, pagenum - 1 ); //back button
				one.push_back( temp );
			}
			if( ( k > numPerPage ) && ( ( i + numPerPage ) < k ) )
			{
				sprintf( temp, "button 260 300 %u %i 0 %li", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum + 1 );
				one.push_back( temp );
			}
			++pagenum;
		}
	}
	else
	{
		MAPUSERNAMEID_ITERATOR I;
		//
		CChar *ourChar = NULL;
		for( I = Accounts->begin(); I != Accounts->end(); ++I )
		{
			ACCOUNTSBLOCK& actbSearch = I->second;
			//
			for( i = 0; i < 6; ++i )
			{
				ourChar = actbSearch.lpCharacters[i];
				if( ValidateObject( ourChar ) )
				{
					if( !UOX::isOnline( (*ourChar) ) )
					{
						if( realCount > 0 && !(realCount%numPerPage) )
						{
							position = 40;
							++pagenum;
							sprintf(temp, "page %lu", pagenum );
							one.push_back( temp );
						}
						sprintf( temp, "text 50 %lu %u %lu", position, cwmWorldState->ServerData()->LeftTextColour(), linenum ); 
						one.push_back( temp );
						sprintf( temp, "button 20 %lu %u %i %lu 0 %lu", position, cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum, buttonnum );
						one.push_back( temp );

						sprintf( temp, " %s (%s)", ourChar->GetName().c_str(), ourChar->GetLastOn().c_str() );
						two.push_back( temp );
						AddSerial( ourChar->GetSerial() );

						position += 20;
						++linenum;
						++buttonnum;
						++realCount;
					}
				}
			}
		}
		sprintf( temp, "Wholist [%i offline]", realCount-1 );
		two[0] = temp;

		pagenum = 1; 
		k = realCount;
		for( i = 0; i < k; i += numPerPage )
		{
			sprintf( temp, "page %lu", pagenum );
			one.push_back( temp );
			if( i >= numPerPage )
			{
				sprintf( temp, "button 50 300 %u %i 0 %li", cwmWorldState->ServerData()->ButtonLeft(), cwmWorldState->ServerData()->ButtonLeft() + 1, pagenum - 1 ); //back button
				one.push_back( temp );
			}
			if( ( k > numPerPage ) && ( ( i + numPerPage ) < k ) )
			{
				sprintf( temp, "button 260 300 %u %i 0 %li", cwmWorldState->ServerData()->ButtonRight(), cwmWorldState->ServerData()->ButtonRight() + 1, pagenum + 1 );
				one.push_back( temp );
			}
			++pagenum;
		}
	}		
	ResetUpdateFlag();
}

}
