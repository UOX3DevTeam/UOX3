#include "uox3.h"
#include "fileio.h"
#include "teffect.h"

cTEffect::cTEffect()
{
	internalData.resize( 0 );
	internalData.clear();
	effectCount = 0;
	currentEffect = 0;
	delFlag = false;
}

cTEffect::~cTEffect()
{

	for( int i = 0; i < effectCount; i++ )
		delete internalData[i];

}

teffect_st *cTEffect::First( void )		// returns the first entry
{
	currentEffect = 0;
	delFlag = false;
	if( effectCount == 0 )
		return NULL;
	return internalData[0];
	
}

teffect_st *cTEffect::Current( void )	// returns current entry
{
	if( currentEffect >= effectCount )
		return NULL;
	return internalData[currentEffect];
}

teffect_st *cTEffect::Next( void )		// returns the next entry
{
	currentEffect++;
	if( currentEffect >= effectCount )
	{
		delFlag = false;
		return NULL;
	}
	if( delFlag )
	{
		currentEffect--;
		delFlag = false;
	}
	return internalData[currentEffect];
}

bool cTEffect::AtEnd( void )			// have we hit the end?
{
	return( currentEffect >= effectCount );
}

bool cTEffect::Add( teffect_st toAdd )	// adds another entry
{
	internalData.resize( effectCount + 1 );
	internalData[effectCount] = new teffect_st;
	memcpy( internalData[effectCount], &toAdd, sizeof( teffect_st ) );
	effectCount++;
	return true;
}

bool cTEffect::DelCurrent( void )	// deletes the current entry
{
	if( currentEffect >= effectCount )
		return false;
	delete internalData[currentEffect];
	for( int i = currentEffect; i < effectCount-1; i++ )
		internalData[i] = internalData[i+1];
	effectCount--;
	delFlag = true;
	return true;
}

UI16 cTEffect::Count( void )
{
	return effectCount;
}

teffect_st *cTEffect::GrabSpecific( UI16 index )
{
	if( index >= effectCount )
		return NULL;
	currentEffect = index;
	return internalData[index];
}

//o-----------------------------------------------------------------------o
//|	Function		-	Save(ofstream &effectDestination, SI32 mode)
//|	Date			-	March 16, 2002
//|	Programmer		-	sereg
//o-----------------------------------------------------------------------o
//|	Returns			-	true/false indicating the success of the write operation
//o-----------------------------------------------------------------------o
bool teffect_st::Save( std::ofstream &effectDestination, SI32 mode ) const
{
	
	std::string destination; 
	std::ostringstream dumping( destination ); 
	BinBuffer buff;

	cBaseObject *getPtr = NULL;

	switch( mode )
	{
	case 1:	
		buff.SetType( 1 );

		buff.PutLong( Source() );

		buff.PutLong( Destination() );

		buff.PutLong( ( ExpireTime() - cwmWorldState->GetUICurrentTime() ) );

		buff.PutByte( Number() );
		
		buff.PutShort( More1() );

		buff.PutShort( More2() );

		buff.PutShort( More3() );

		buff.PutByte( Dispellable() );

		getPtr = ObjPtr();
		if( getPtr != NULL )
			buff.PutLong( getPtr->GetSerial() );
		else
			buff.PutLong( INVALIDSERIAL );

		buff.Write( effectDestination );
		break;
	case 0:
	default:
		effectDestination << "[EFFECT]" << std::endl;

		dumping << "Source=" << Source() << std::endl;
		dumping << "Dest=" << Destination() << std::endl;
		dumping << "Expire=" << ( ExpireTime() - cwmWorldState->GetUICurrentTime() ) << std::endl;
		dumping << "Number=" << Number() << std::endl;  
		dumping << "More1=" << More1() << std::endl;
		dumping << "More2=" << More2() << std::endl;
		dumping << "More3=" << More3() << std::endl;
		dumping << "Dispel=" << Dispellable() << std::endl;

		getPtr = ObjPtr();
		if( getPtr != NULL )
			dumping << "ObjPtr=" << getPtr->GetSerial() << std::endl;
		else
			dumping << "ObjPtr=" << INVALIDSERIAL << std::endl;

		effectDestination << dumping.str();

		effectDestination << std::endl << "o---o" << std::endl << std::endl;
		break;
	}
	return true;
}
