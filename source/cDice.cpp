#include "cdice.h"

inline int RandomNum( int nLowNum, int nHighNum )
{
	if( nHighNum - nLowNum + 1 )
		return ((rand() % ( nHighNum - nLowNum + 1 )) + nLowNum );
	else
		return nLowNum;
};

cDice::cDice() : dice( 1 ), sides( 1 ), addition( 0 )
{
}

cDice::cDice( std::string dieString )
{
	convStringToDice( dieString );
}

cDice::cDice( int d, int s, int a ) : dice( d ), sides( s ), addition( a )
{
}

cDice::~cDice()
{
}

void cDice::setDice( int newDice )
{
	dice = newDice;
}

void cDice::setSides( int newSides )
{
	sides = newSides;
}

void cDice::setAddition( int newAddition )
{
	addition = newAddition;
}

int cDice::roll( void )
{
	int sum = 0;
	for( int rolls = 0; rolls < dice; rolls++ )
	{
		sum += RandomNum( 1, sides );
	}
	sum += addition;
	return sum;
}

bool cDice::convStringToDice( std::string dieString )
{
	int dLoc = -1, pLoc = -1;
	dice = 1;
	sides = 1;
	addition = 0;
	dLoc = dieString.find( "d" );
	if( dLoc <= 0 )
		return false;
	char tmpString[10];
	int tmpCounter;
	for( tmpCounter = 0; tmpCounter < dLoc; tmpCounter++ )
		tmpString[tmpCounter] = dieString[tmpCounter];
	tmpString[tmpCounter] = 0;
	dice = atoi( tmpString );
	pLoc = dieString.find( "+" );
	if( pLoc <= 0 )
	{	// no +
		for( tmpCounter = dLoc+1; tmpCounter < dieString.length(); tmpCounter++ )
			tmpString[tmpCounter-dLoc-1] = dieString[tmpCounter];
		tmpString[tmpCounter-dLoc-1] = 0;
		sides = atoi( tmpString );
	}
	else
	{
		for( tmpCounter = dLoc+1; tmpCounter < pLoc; tmpCounter++ )
			tmpString[tmpCounter-dLoc-1] = dieString[tmpCounter];
		tmpString[tmpCounter-dLoc-1] = 0;
		sides = atoi( tmpString );
		for( tmpCounter = pLoc + 1; tmpCounter < dieString.length(); tmpCounter++ )
			tmpString[tmpCounter-pLoc-1] = tmpString[tmpCounter];
		tmpString[tmpCounter-pLoc-1] = 0;
		addition = atoi( tmpString );
	}
	return true;
}


