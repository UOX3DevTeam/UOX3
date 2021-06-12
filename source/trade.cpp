#include "uox3.h"
#include "classes.h"
#include "CPacketSend.h"
#include "ObjectFactory.h"


//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void sendTradeStatus( CSocket *mSock, CSocket *nSock, CItem *tradeWindowOne, CItem *tradeWindowTwo )
//|	Date		-	February 2, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates the status of the secure trade buttons (checked or unchecked) to both clients
//o-----------------------------------------------------------------------------------------------o
void sendTradeStatus( CSocket *mSock, CSocket *nSock, CItem *tradeWindowOne, CItem *tradeWindowTwo )
{
	CPSecureTrading cpstOne( (*tradeWindowOne), (tradeWindowOne->GetTempVar( CITV_MOREZ ) % 256), (tradeWindowTwo->GetTempVar( CITV_MOREZ ) % 256) );
	cpstOne.Action( 2 );
	mSock->Send( &cpstOne );

	CPSecureTrading cpstTwo( (*tradeWindowTwo), (tradeWindowTwo->GetTempVar( CITV_MOREZ ) % 256), (tradeWindowOne->GetTempVar( CITV_MOREZ ) % 256) );
	cpstTwo.Action( 2 );
	nSock->Send( &cpstTwo );
}
void sendTradeStatus( CItem *tradeWindowOne, CItem *tradeWindowTwo )
{
	CChar *p1 = FindItemOwner( tradeWindowOne );
	CChar *p2 = FindItemOwner( tradeWindowTwo );
	if( !ValidateObject( p1 ) || !ValidateObject( p2 ) )
		return;

	CSocket *s1 = p1->GetSocket();
	CSocket *s2 = p2->GetSocket();
	if( s1 != nullptr && s2 != nullptr )
		sendTradeStatus( s1, s2, tradeWindowOne, tradeWindowTwo );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *CreateTradeWindow( CSocket *mSock, CSocket *nSock, CChar *mChar )
//|	Date		-	February 2, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a TradeWindow and sends it to the clients
//o-----------------------------------------------------------------------------------------------o
CItem *CreateTradeWindow( CSocket *mSock, CSocket *nSock, CChar *mChar )
{
	CItem *mPack = mChar->GetPackItem();
	if( !ValidateObject( mPack ) )
	{
		mSock->sysmessage( 773 );
		nSock->sysmessage( 1357, mChar->GetName().c_str() );
		return nullptr;
	}

	CItem *tradeWindow = Items->CreateItem( nullptr, mChar, 0x1E5E, 1, 0, OT_ITEM, false );
	if( !ValidateObject( tradeWindow ) )
		return nullptr;

	tradeWindow->SetType( IT_TRADEWINDOW );
	tradeWindow->SetCont( mChar );
	tradeWindow->SetX( 0 );
	tradeWindow->SetY( 0 );
	tradeWindow->SetZ( 0 );
	tradeWindow->SetDye( false );
	tradeWindow->SetTempVar( CITV_MOREZ, 0 );
	tradeWindow->SetDecayable( false );

	tradeWindow->SendPackItemToSocket( mSock );
	tradeWindow->SendPackItemToSocket( nSock );

	return tradeWindow;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	CItem *startTrade( CSocket *mSock, CChar *nChar )
//|	Date		-	February 2, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles everything necesarry to start a secure trade
//o-----------------------------------------------------------------------------------------------o
CItem *startTrade( CSocket *mSock, CChar *nChar )
{
	if( mSock == nullptr || !ValidateObject( nChar ) )
		return nullptr;

	CChar *mChar	= mSock->CurrcharObj();
	CSocket *nSock	= nChar->GetSocket();

	if( !ValidateObject( mChar ) || nSock == nullptr )
		return nullptr;

	CItem *tradeWindowOne = CreateTradeWindow( mSock, nSock, mChar );
	if( !ValidateObject( tradeWindowOne ) )
		return nullptr;

	CItem *tradeWindowTwo = CreateTradeWindow( nSock, mSock, nChar );
	if( !ValidateObject( tradeWindowTwo ) )
	{
		tradeWindowOne->Delete();
		return nullptr;
	}

	const SERIAL tw1Serial = tradeWindowOne->GetSerial();
	const SERIAL tw2Serial = tradeWindowTwo->GetSerial();

	tradeWindowOne->SetTempVar( CITV_MOREX, tw2Serial );
	tradeWindowTwo->SetTempVar( CITV_MOREX, tw1Serial );

	CPSecureTrading cpstOne( (*nChar), tw1Serial, tw2Serial );
	cpstOne.Name( nChar->GetName() );
	mSock->Send( &cpstOne );

	CPSecureTrading cpstTwo( (*mChar), tw2Serial, tw1Serial );
	cpstTwo.Name( mChar->GetName() );
	nSock->Send( &cpstTwo );

	CPWornItem toWear = (*tradeWindowOne);
	mSock->Send( &toWear );
	nSock->Send( &toWear );

	CPWornItem toWear2 = (*tradeWindowTwo);
	mSock->Send( &toWear2 );
	nSock->Send( &toWear2 );

	return tradeWindowOne;
}

bool clearTradesFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	bool retVal = true;
	if( ValidateObject( a ) && a->CanBeObjType( OT_ITEM ) )
	{
		// Body of the functor goes here
		CItem *i = static_cast< CItem * >(a);
		if( ValidateObject( i ) )
		{
			if( i->GetType() == IT_TRADEWINDOW )
			{
				CChar *k = FindItemOwner( i );
				if( ValidateObject( k ) )
				{
					CItem *p = k->GetPackItem();
					if( ValidateObject( p ) )
					{
						GenericList< CItem * > *iCont = i->GetContainsList();
						for( CItem *j = iCont->First(); !iCont->Finished(); j = iCont->Next() )
						{
							if( ValidateObject( j ) )
								j->SetCont( p );
						}
					}
				}
				i->Delete();
				++b;	// let's track how many we cleared
			}
		}
	}
	return retVal;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void clearTrades( void )
//|	Date		-	February 2, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Loops through all items to clear any active trades
//o-----------------------------------------------------------------------------------------------o
void clearTrades( void )
{
	UI32 b = 0;
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, nullptr, &clearTradesFunctor );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void completeTrade( CItem *tradeWindowOne, CItem *tradeWindowTwo, bool tradeSuccess )
//|	Date		-	February 2, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles everything necesarry to complete a trade
//o-----------------------------------------------------------------------------------------------o
void completeTrade( CItem *tradeWindowOne, CItem *tradeWindowTwo, bool tradeSuccess )
{
	CChar *p1 = FindItemOwner( tradeWindowOne );
	CChar *p2 = FindItemOwner( tradeWindowTwo );
	if( !ValidateObject( p1 ) || !ValidateObject( p2 ) )
		return;

	CSocket *mSock = p1->GetSocket();
	if( mSock != nullptr )
	{
		CPSecureTrading cpstOne( (*tradeWindowOne) );
		cpstOne.Action( 1 );
		mSock->Send( &cpstOne );
	}
	CSocket *nSock = p2->GetSocket();
	if( nSock != nullptr )
	{
		CPSecureTrading cpstTwo( (*tradeWindowTwo) );
		cpstTwo.Action( 1 );
		nSock->Send( &cpstTwo );
	}

	CItem *bp1 = p1->GetPackItem();
	CItem *bp2 = p2->GetPackItem();
	if( ValidateObject( bp1 ) && ValidateObject( bp2 ) )
	{
		CItem *i = nullptr;
		GenericList< CItem * > *c1Cont = tradeWindowOne->GetContainsList();
		for( i = c1Cont->First(); !c1Cont->Finished(); i = c1Cont->Next() )
		{
			if( ValidateObject( i ) )
			{
				if( tradeSuccess )
					i->SetCont( bp2 );
				else
					i->SetCont( bp1 );
				i->PlaceInPack();
			}
		}
		GenericList< CItem * > *c2Cont = tradeWindowTwo->GetContainsList();
		for( i = c2Cont->First(); !c2Cont->Finished(); i = c2Cont->Next() )
		{
			if( ValidateObject( i ) )
			{
				if( tradeSuccess )
					i->SetCont( bp1 );
				else
					i->SetCont( bp2 );
				i->PlaceInPack();
			}
		}
	}

	tradeWindowOne->Delete();
	tradeWindowTwo->Delete();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void cancelTrade( CItem *tradeWindowOne )
//|	Date		-	February 2, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Cancels a secure trade
//o-----------------------------------------------------------------------------------------------o
void cancelTrade( CItem *tradeWindowOne )
{
	CItem *tradeWindowTwo = calcItemObjFromSer( tradeWindowOne->GetTempVar( CITV_MOREX ) );
	if( !ValidateObject( tradeWindowTwo ) )
		return;

	tradeWindowOne->SetTempVar( CITV_MOREZ, 0 );
	tradeWindowTwo->SetTempVar( CITV_MOREZ, 0 );
	sendTradeStatus( tradeWindowOne, tradeWindowTwo );

	completeTrade( tradeWindowOne, tradeWindowTwo, false );
}

bool CPITradeMessage::Handle( void )
{
	CItem *tradeWindowOne = calcItemObjFromSer( tSock->GetDWord( 4 ) );
	if( ValidateObject( tradeWindowOne ) )
	{
		CItem *tradeWindowTwo = nullptr;
		switch( tSock->GetByte( 3 ) )
		{
			case 0://Start trade - Never happens, sent out by the server only.
				break;
			case 2://Change check marks.  Possibly conclude trade
				tradeWindowTwo = calcItemObjFromSer( tradeWindowOne->GetTempVar( CITV_MOREX ) );
				if( ValidateObject( tradeWindowTwo ) )
				{
					tradeWindowOne->SetTempVar( CITV_MOREZ, tSock->GetByte( 11 ) );
					sendTradeStatus( tradeWindowOne, tradeWindowTwo );
					if( tradeWindowOne->GetTempVar( CITV_MOREZ ) && tradeWindowTwo->GetTempVar( CITV_MOREZ ) )
						completeTrade( tradeWindowOne, tradeWindowTwo, true );
				}
				break;
			case 1://Cancel trade.  Send each person cancel messages, move items.
				cancelTrade( tradeWindowOne );
				break;
			default:
				Console.error( " Fallout of switch statement without default. trade.cpp, trademsg()" );
				break;
		}
	}
	return true;
}

bool killTradesFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	bool retVal = true;
	if( ValidateObject( a ) && a->CanBeObjType( OT_ITEM ) )
	{
		// Body of the functor goes here
		CItem *i = static_cast< CItem * >(a);
		if( ValidateObject( i ) )
		{
			if( i->GetType() == IT_TRADEWINDOW )
			{
				CChar *k = static_cast<CChar *>(i->GetCont());
				if( ValidateObject( k ) )
				{
					CChar *mChar = static_cast<CChar *>(extraData);
					if( k == mChar )
						cancelTrade( i );
				}
				++b;	// let's track how many we cleared
			}
		}
	}
	return retVal;
}
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void killTrades( CChar *i )
//|	Date		-	February 2, 2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Cancels any active trades associated with a character.
//o-----------------------------------------------------------------------------------------------o
void killTrades( CChar *i )
{
	UI32 b = 0;
	ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, i, &killTradesFunctor );
}
