#include "uox3.h"

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
void cWhoList::SendSocket( cSocket *toSendTo, UI08 option )
// PRE:		WhoList class is active
// POST:	Sends the wholist to the socket, depending on option
//			0 = On/off line players
//			1 = Online players
//			2 = Offline players
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
	gmCount++;
}

void cWhoList::GMLeave( void )
// PRE:		WhoList class is active
// POST:	Decrements the GM counter
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	if( gmCount != 0 )
		gmCount--;
}

SERIAL cWhoList::GrabSerial( CHARACTER index )
// PRE:		WhoList class is active
// POST:	Returns serial as refered to by index
// CODER:	Abaddon
// DATE:	12th February, 2000
{
//	if( index >= onlineEntries.size() )
//		return offlineEntries[index - onlineEntries.size()]->player;
//	return onlineEntries[index]->player;
	return INVALIDSERIAL;
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
	int size = whoMenuData.size();
	whoMenuData.resize( size + 1 );
	whoMenuData[size] = toAdd;
}

void cWhoList::ButtonSelect( cSocket *toSendTo, UI16 buttonPressed, UI08 type )
// PRE:		WhoList class is active
// POST:	Does action based upon button selected
// CODER:	Abaddon
// DATE:	23rd February, 2000
{
	CChar *sourceChar = toSendTo->CurrcharObj();

	if( buttonPressed < 200 )
	{		
		buttonPressed -= 7;
		sourceChar->SetMaking( buttonPressed );
		Command( toSendTo, type, buttonPressed );
		return;
	}

	GMLeave();
	SERIAL ser = sourceChar->GetMaking();
	if( ser == INVALIDSERIAL )
	{
		sysmessage( toSendTo, 1387 );
		return;
	}

	SERIAL charSerial = whoMenuData[ser];

	CChar *targetChar = calcCharObjFromSer( charSerial ); // find selected char ...
	if( targetChar == NULL )
	{
		sysmessage( toSendTo, 1387 );
		return;
	}
	cSocket *trgSock = NULL;
	switch( buttonPressed )
	{
	case 200://gochar
		if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
		{
			sysmessage( toSendTo, 1388 );
			return;
		}
		if( sourceChar->WorldNumber() != targetChar->WorldNumber() )
		{
			sourceChar->SetLocation( targetChar );
			SendMapChange( targetChar->WorldNumber(), toSendTo );
		}
		else
			sourceChar->SetLocation( targetChar );
		sourceChar->Teleport();
		break;
	case 201://xtele
		if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
		{
			sysmessage( toSendTo, 1389 );
			return;
		}
		if( !targetChar->IsNpc() && targetChar->WorldNumber() != sourceChar->WorldNumber() )
		{
			targetChar->SetLocation( sourceChar );
			trgSock = calcSocketObjFromChar( targetChar );
			SendMapChange( sourceChar->WorldNumber(), trgSock );
		}
		else
			targetChar->SetLocation( sourceChar );
		targetChar->Teleport();
		break;
	case 202://jail char
		if( sourceChar == targetChar )
		{
			sysmessage( toSendTo, 1390 );
			break;
		}
		else
		{
			if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
			{
				sysmessage( toSendTo, 1391 );
				return;
			}
			Targ->JailTarget( toSendTo, targetChar->GetSerial() );
			break;
		}						 
	case 203://release
		if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
		{
			sysmessage( toSendTo, 1392 );
			return;
		}
		Targ->ReleaseTarget( toSendTo, targetChar->GetSerial() );
		break;
	case 204:
		if( targetChar == sourceChar )
		{
			sysmessage( toSendTo, 1393 );
			break;
		}
		else
		{
			if( !::isOnline( targetChar ) )	// local var overloads it
			{
				sysmessage( toSendTo, 1394 );		// you realize the break isn't necessary?
				break;
			}
			if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
			{
				sysmessage( toSendTo, 1395 );
				return;
			}
			sysmessage( toSendTo, 1396 );
			Network->Disconnect( calcSocketFromChar( targetChar ) );
			break;
		}
	case 205:	// not active currently, a remote where
		// make it prettier later!
		if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
		{
			sysmessage( toSendTo, 1397 );
			return;
		}
		sysmessage( toSendTo, "X: %i Y: %i Z: %i World: %i", targetChar->GetX(), targetChar->GetY(), targetChar->GetZ(), targetChar->WorldNumber() );
		break;
	case 206:	// remote cstats
		if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
		{
			sysmessage( toSendTo, 1398 );
			return;
		}
		toSendTo->SetDWord( 7, targetChar->GetSerial() );
		Targ->CstatsTarget( toSendTo );
		break;
	case 207:	// remote tweak
		if( targetChar->GetCommandLevel() > sourceChar->GetCommandLevel() )
		{
			sysmessage( toSendTo, 1399 );
			return;
		}
		toSendTo->SetDWord( 7, targetChar->GetSerial() );
		Targ->TweakTarget( toSendTo );
		break;
	default:
		Console.Error( 2, " Fallout of switch statement without default. wholist.cpp, ButtonSelect()" );
	}
}

void cWhoList::Command( cSocket *toSendTo, UI08 type, UI16 buttonPressed )
{
	SERIAL serial = whoMenuData[buttonPressed];
	CChar *targetChar = calcCharObjFromSer( serial ); // find selected char ...

	if( targetChar == NULL )
	{
		sysmessage( toSendTo, 1387 );
		return;
	}
	
	stringList one, two;
	
	UI16 lColour = cwmWorldState->ServerData()->GetLeftTextColour(), tColour = cwmWorldState->ServerData()->GetTitleColour();
	UI16 butRight = cwmWorldState->ServerData()->GetButtonRight();
	//--static pages
	one.push_back( "nomove" );
	one.push_back( "noclose" );
	one.push_back( "page 0" );
	char temp[512];
	sprintf( temp,"resizepic 0 0 %i 260 340", cwmWorldState->ServerData()->GetBackgroundPic() );
	one.push_back( temp );
	sprintf( temp, "button 30 300 %i %i 1 0 1", cwmWorldState->ServerData()->GetButtonCancel(), cwmWorldState->ServerData()->GetButtonCancel() + 1); //OKAY
	one.push_back( temp );
	sprintf( temp, "text 30 40 %i 0", tColour );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	one.push_back( temp );
	sprintf( temp, "text 30 60 %i 1", tColour );
	one.push_back( temp );

	one.push_back( "page 1" );

	sprintf( temp, "text 50 90 %i 2", lColour );	//goto text
	one.push_back( temp );
	sprintf( temp, "text 50 110 %i 3", lColour );	//gettext
	one.push_back( temp );
	sprintf( temp, "text 50 130 %i 4", lColour );	//Jail text
	one.push_back( temp );
	sprintf( temp, "text 50 150 %i 5", lColour );	//Release text
	one.push_back( temp );
	sprintf( temp, "text 50 170 %i 6", lColour );	//Kick user text
	one.push_back( temp );
	sprintf( temp, "text 50 190 %i 7", lColour );
	one.push_back( temp );
	sprintf( temp, "text 50 210 %i 8", lColour );
	one.push_back( temp );
	sprintf( temp, "text 50 230 %i 9", lColour );
	one.push_back( temp );
	sprintf( temp, "text 50 270 %i 10", lColour );
	one.push_back( temp );
	sprintf( temp, "button 20 90 %i %i 1 0 200", butRight, butRight + 1 ); //goto button
	one.push_back( temp );
	sprintf( temp, "button 20 110 %i %i 1 0 201", butRight, butRight + 1 ); //get button
	one.push_back( temp );
	sprintf( temp, "button 20 130 %i %i 1 0 202", butRight, butRight + 1 ); //Jail button
	one.push_back( temp );
	sprintf( temp, "button 20 150 %i %i 1 0 203", butRight, butRight + 1 ); //Release button
	one.push_back( temp );
	sprintf( temp, "button 20 170 %i %i 1 0 204", butRight, butRight + 1 ); //kick button
	one.push_back( temp );
	sprintf( temp, "button 20 190 %i %i 1 0 205", butRight, butRight + 1 ); //Where button
	one.push_back( temp );
	sprintf( temp, "button 20 210 %i %i 1 0 206", butRight, butRight + 1 ); //Cstats button
	one.push_back( temp );
	sprintf( temp, "button 20 230 %i %i 1 0 207", butRight, butRight + 1 ); //Tweak button
	one.push_back( temp );


	sprintf( temp, "User %i selected (account %i)",buttonPressed, targetChar->GetAccount().wAccountIndex);
	two.push_back( temp );
	sprintf( temp, "Name: %s", targetChar->GetName() );   
	two.push_back( temp );
	two.push_back( Dictionary->GetEntry( 1400 ) );
	two.push_back( Dictionary->GetEntry( 1401 ) );
	two.push_back( Dictionary->GetEntry( 1402 ) );
	two.push_back( Dictionary->GetEntry( 1403 ) );
	two.push_back( Dictionary->GetEntry( 1404 ) );
	two.push_back( Dictionary->GetEntry( 1405 ) );
	two.push_back( Dictionary->GetEntry( 1406 ) );
	two.push_back( Dictionary->GetEntry( 1407 ) );
	sprintf( temp, "Serial#[%x %x %x %x]", targetChar->GetSerial( 1 ), targetChar->GetSerial( 2 ), targetChar->GetSerial( 3 ), targetChar->GetSerial( 4 ) );   
	two.push_back( temp );

	SendVecsAsGump( toSendTo, one, two, type, INVALIDSERIAL );
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
	int i,k;
	int numPerPage = 13;
	UI32 pagenum = 1, position = 40, linenum = 1, buttonnum = 7;
	
	k = now;
	int realCount = 1;
	//--static pages
	one.push_back( "noclose" );
	one.push_back( "page 0"  );
	sprintf( temp, "resizepic 0 0 %i 320 340", cwmWorldState->ServerData()->GetBackgroundPic() );
	one.push_back( temp );
	sprintf( temp, "button 250 15 %i %i 1 0 1", cwmWorldState->ServerData()->GetButtonCancel(), cwmWorldState->ServerData()->GetButtonCancel() + 1 );
	one.push_back( temp );
	sprintf( temp, "text 45 15 %i 0", cwmWorldState->ServerData()->GetTitleColour() );
	one.push_back( temp );

	sprintf( temp, "page %i", pagenum );
	one.push_back( temp );

	sprintf( temp, "Wholist" );
	two.push_back( temp );

	//--Start User List
	if( online )
	{
		for( i = 0; i < k; i++ )
		{
			cSocket *tSock = calcSocketObjFromSock( i );
			CChar *tChar = tSock->CurrcharObj();
			if( tChar == NULL )
				continue;
			if( realCount > 0 && !(realCount%numPerPage) )
			{
				position = 40;
				pagenum++;
				sprintf(temp, "page %i", pagenum );
				one.push_back( temp );
			}
			sprintf( temp, "text 50 %i %i %i", position, cwmWorldState->ServerData()->GetLeftTextColour(), linenum ); 
			one.push_back( temp );
			sprintf( temp, "button 20 %i %i %i %i 0 %i", position, cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1, pagenum, buttonnum );
			one.push_back( temp );
			sprintf( temp, " %s", tChar->GetName() );
			two.push_back( temp );
			AddSerial( tChar->GetSerial() );

			position += 20;
			linenum++;
			buttonnum++;
			realCount++;
		}
		
		sprintf( temp, "Wholist [%i online]", realCount - 1 );
		two[0] = temp;

		pagenum = 1; 
		k = realCount;
		for( i = 0; i < k; i += numPerPage )
		{
			sprintf( temp, "page %i", pagenum );
			one.push_back( temp );
			if( i >= numPerPage )
			{
				sprintf( temp, "button 50 300 %i %i 0 %i", cwmWorldState->ServerData()->GetButtonLeft(), cwmWorldState->ServerData()->GetButtonLeft() + 1, pagenum - 1 ); //back button
				one.push_back( temp );
			}
			if( ( k > numPerPage ) && ( ( i + numPerPage ) < k ) )
			{
				sprintf( temp, "button 260 300 %i %i 0 %i", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1, pagenum + 1 );
				one.push_back( temp );
			}
			pagenum++;
		}
	}
	else
	{
		ACCOUNTSBLOCK actbSearch;
		actbSearch.wAccountIndex=AB_INVALID_ID;
		MAPUSERNAMEID_ITERATOR I;
		//
		CChar *ourChar = NULL;
		for(I=Accounts->begin();I!=Accounts->end();I++)
		{
			actbSearch = I->second;
			//
			for( i = 0; i < 5; i++ )
			{
				ourChar = actbSearch.lpCharacters[i];
				if( ourChar != NULL )
				{
					if( !::isOnline( ourChar ) )
					{
						if( realCount > 0 && !(realCount%numPerPage) )
						{
							position = 40;
							pagenum++;
							sprintf(temp, "page %i", pagenum );
							one.push_back( temp );
						}
						sprintf( temp, "text 50 %i %i %i", position, cwmWorldState->ServerData()->GetLeftTextColour(), linenum ); 
						one.push_back( temp );
						sprintf( temp, "button 20 %i %i %i %i 0 %i", position, cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1, pagenum, buttonnum );
						one.push_back( temp );

						sprintf( temp, " %s (%s)", ourChar->GetName(), ourChar->GetLastOn() );
						two.push_back( temp );
						AddSerial( ourChar->GetSerial() );

						position += 20;
						linenum++;
						buttonnum++;
						realCount++;
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
			sprintf( temp, "page %i", pagenum );
			one.push_back( temp );
			if( i >= numPerPage )
			{
				sprintf( temp, "button 50 300 %i %i 0 %i", cwmWorldState->ServerData()->GetButtonLeft(), cwmWorldState->ServerData()->GetButtonLeft() + 1, pagenum - 1 ); //back button
				one.push_back( temp );
			}
			if( ( k > numPerPage ) && ( ( i + numPerPage ) < k ) )
			{
				sprintf( temp, "button 260 300 %i %i 0 %i", cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1, pagenum + 1 );
				one.push_back( temp );
			}
			pagenum++;
		}
	}		
	ResetUpdateFlag();
}
