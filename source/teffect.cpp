#include "uox3.h"
#include "teffect.h"

namespace UOX
{

CTEffectContainer *TEffects;

CTEffectContainer::CTEffectContainer() : isScanning( false )
{
	internalData.resize( 0 );
	currentEffect = internalData.end();
}

CTEffectContainer::~CTEffectContainer()
{
	for( size_t i = 0; i < internalData.size(); ++i )
		delete internalData[i];
	internalData.clear();
}

void CTEffectContainer::QueueToKeep( CTEffect *toKeep )
{
	backupKeep.push_back( toKeep );
}
void CTEffectContainer::QueueToRemove( CTEffect *toRemove )
{
	backupRemove.push_back( toRemove );
}
void CTEffectContainer::StartQueue( void )
{
	isScanning = true;
}
void CTEffectContainer::Prune( void )
{
	for( size_t i = 0; i < backupRemove.size(); ++i )
	{
		delete backupRemove[i];
	}
	internalData.clear();
	internalData.resize( backupKeep.size() );
	for( size_t j = 0; j < backupKeep.size(); ++j )
	{
		internalData[j] = backupKeep[j];
	}
	backupKeep.clear();
	backupRemove.clear();
	isScanning = false;
}

CTEffect *CTEffectContainer::First( void )		// returns the first entry
{
	CTEffect *myEffect = NULL;
	currentEffect = internalData.begin();
	if( !AtEnd() )
		myEffect = (*currentEffect);
	return myEffect;
}

CTEffect *CTEffectContainer::Current( void )	// returns current entry
{
	CTEffect *myEffect = NULL;
	if( !AtEnd() )
        myEffect = (*currentEffect);
	return myEffect;
}

CTEffect *CTEffectContainer::Next( void )		// returns the next entry
{
	CTEffect *myEffect = NULL;
	if( !AtEnd() )
	{
		++currentEffect;
		if( !AtEnd() )
			myEffect = (*currentEffect);
	}
	return myEffect;
}

bool CTEffectContainer::AtEnd( void )			// have we hit the end?
{
	return ( currentEffect == internalData.end() );
}

bool CTEffectContainer::Add( CTEffect *toAdd )	// adds another entry
{
	if( isScanning )
		QueueToKeep( toAdd );
	else
		internalData.push_back( toAdd );
	return true;
}

UI16 CTEffectContainer::Count( void )
{
	return internalData.size();
}

CTEffect *CTEffectContainer::CreateEffect( void )
{
	return new CTEffect;
}

CTEffect *CTEffectContainer::GrabSpecific( UI16 index )
{
	if( index >= internalData.size() )
		return NULL;
	currentEffect = (internalData.begin() + index);
	return (*currentEffect);
}

//o-----------------------------------------------------------------------o
//|	Function		-	Save(ofstream &effectDestination )
//|	Date			-	March 16, 2002
//|	Programmer		-	sereg
//o-----------------------------------------------------------------------o
//|	Returns			-	true/false indicating the success of the write operation
//o-----------------------------------------------------------------------o
bool CTEffect::Save( std::ofstream &effectDestination ) const
{
	std::string destination; 
	std::ostringstream dumping( destination ); 
	CBaseObject *getPtr = NULL;

	effectDestination << "[EFFECT]" << std::endl;

	dumping << "Source=" << Source() << std::endl;
	dumping << "Dest=" << Destination() << std::endl;
	dumping << "Expire=" << ( ExpireTime() - cwmWorldState->GetUICurrentTime() ) << std::endl;
	dumping << "Number=" << static_cast<UI16>(Number()) << std::endl;  
	dumping << "More1=" << More1() << std::endl;
	dumping << "More2=" << More2() << std::endl;
	dumping << "More3=" << More3() << std::endl;
	dumping << "Dispel=" << Dispellable() << std::endl;

	getPtr = ObjPtr();
	if( ValidateObject( getPtr ) )
		dumping << "ObjPtr=" << getPtr->GetSerial() << std::endl;
	else
		dumping << "ObjPtr=" << INVALIDSERIAL << std::endl;

	effectDestination << dumping.str();

	effectDestination << std::endl << "o---o" << std::endl << std::endl;
	return true;
}

}
