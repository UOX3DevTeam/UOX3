#include "uox3.h"
#include "jail.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "scriptc.h"

namespace UOX
{

JailSystem *JailSys;

bool JailCell::IsEmpty( void ) const		
{ 
	return playersInJail.size() == 0; 
}
size_t JailCell::JailedPlayers( void ) const 
{ 
	return playersInJail.size(); 
}
SI16 JailCell::X( void ) const				
{ 
	return x; 
}
SI16 JailCell::Y( void ) const				
{ 
	return y; 
}
SI08 JailCell::Z( void ) const				
{ 
	return z; 
}
void JailCell::X( SI16 nVal )				
{ 
	x = nVal; 
}
void JailCell::Y( SI16 nVal )				
{ 
	y = nVal; 
}
void JailCell::Z( SI08 nVal )				
{ 
	z = nVal; 
}
void JailCell::AddOccupant( CChar *pAdd, SI32 secsFromNow ) 
{ 
	if( !ValidateObject( pAdd ) )
		return;
	JailOccupant *toAdd = new JailOccupant; 
	time( &(toAdd->releaseTime) ); 
	toAdd->releaseTime += secsFromNow; 
	toAdd->pSerial = pAdd->GetSerial();
	toAdd->x = pAdd->GetX();
	toAdd->y = pAdd->GetY();
	toAdd->z = pAdd->GetZ();
	pAdd->SetLocation( x, y, z );
	playersInJail.push_back( toAdd );
}

void JailCell::AddOccupant( JailOccupant *toAdd )
{
	playersInJail.push_back( toAdd );
}

void JailCell::EraseOccupant( size_t occupantID )
{
	if( occupantID >= playersInJail.size() )
		return;
	delete playersInJail[occupantID];
	playersInJail.erase( playersInJail.begin() + occupantID );
}
JailOccupant *JailCell::Occupant( size_t occupantID ) 
{ 
	if( occupantID >= playersInJail.size() )
		return NULL;
	return playersInJail[occupantID];
}
void JailCell::PeriodicCheck( void )
{
	time_t now;
	time( &now );
	for( size_t i = playersInJail.size() - 1; i >= 0 && i < playersInJail.size(); --i )
	{
		if( difftime( now, playersInJail[i]->releaseTime ) < 0 )
		{	// time to release them
			CChar *toRelease = calcCharObjFromSer( playersInJail[i]->pSerial );
			if( !ValidateObject( toRelease ) )
				EraseOccupant( i );
			else
			{
				toRelease->SetLocation( playersInJail[i]->x, playersInJail[i]->y, playersInJail[i]->z );
				toRelease->SetCell( -1 );
				EraseOccupant( i );
			}
		}
	}
}

void JailCell::WriteData( std::ofstream &outStream, size_t cellNumber )
{
	std::vector< JailOccupant * >::iterator jIter;
	for( jIter = playersInJail.begin(); jIter != playersInJail.end(); ++jIter )
	{
		JailOccupant *mOccupant = (*jIter);
		if( mOccupant != NULL )
		{
			outStream << "[PRISONER]" << std::endl << "{" << std::endl;
			outStream << "CELL=" << cellNumber << std::endl;
			outStream << "SERIAL=" << std::hex << mOccupant->pSerial << std::endl;
			outStream << "OLDX=" << std::dec << mOccupant->x << std::endl;
			outStream << "OLDY=" << mOccupant->y << std::endl;
			outStream << "OLDZ=" << (SI16)mOccupant->z << std::endl;
			outStream << "RELEASE=" << mOccupant->releaseTime << std::endl;
			outStream << "}" << std::endl << std::endl;
		}
	}
}

JailSystem::JailSystem()
{
	jails.resize( 0 );
}
JailSystem::~JailSystem()
{
	jails.clear();
}
void JailSystem::ReadSetup( void )
{
	ScriptSection *Regions = FileLookup->FindEntry( "JAILS", regions_def );
	if( Regions == NULL )
	{
		jails.resize( 10 );
		jails[0].X( 5276 );		jails[0].Y( 1164 );		jails[0].Z( 0 );
		jails[1].X( 5286 );		jails[1].Y( 1164 );		jails[1].Z( 0 );
		jails[2].X( 5296 );		jails[2].Y( 1164 );		jails[2].Z( 0 );
		jails[3].X( 5306 );		jails[3].Y( 1164 );		jails[3].Z( 0 );
		jails[4].X( 5276 );		jails[4].Y( 1174 );		jails[4].Z( 0 );
		jails[5].X( 5286 );		jails[5].Y( 1174 );		jails[5].Z( 0 );
		jails[6].X( 5296 );		jails[6].Y( 1174 );		jails[6].Z( 0 );
		jails[7].X( 5306 );		jails[7].Y( 1174 );		jails[7].Z( 0 );
		jails[8].X( 5283 );		jails[8].Y( 1184 );		jails[8].Z( 0 );
		jails[9].X( 5304 );		jails[9].Y( 1184 );		jails[9].Z( 0 );
	}
	else
	{
		JailCell toAdd;
		UString tag;
		UString data;
		for( tag = Regions->First(); !Regions->AtEnd(); tag = Regions->Next() )
		{
			if( tag.empty() )
				continue;
			data = Regions->GrabData();
			switch( (tag.data()[0]) )
			{
				case 'X':	toAdd.X( data.toShort() );	break;
				case 'Y':	toAdd.Y( data.toShort() );	break;
				case 'Z':	
							toAdd.Z( data.toByte() );
							jails.push_back( toAdd );
							break;
			}
		}
	}
}
void JailSystem::ReadData( void )
{
	if( jails.size() == 0 )
		return;
	std::string temp	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "jails.wsc";
	if( FileExists( temp ) )
	{
		Script *jailData = new Script( temp, NUM_DEFS, false );
		if( jailData != NULL )
		{
			ScriptSection *prisonerData = NULL;
			for( prisonerData = jailData->FirstEntry(); prisonerData != NULL; prisonerData = jailData->NextEntry() )
			{
				if( prisonerData == NULL )
					continue;
				UString tag;
				UString data;
				UString UTag;
				JailOccupant *toPush = new JailOccupant;
				UI08 cellNumber = 0;
				for( tag = prisonerData->First(); !prisonerData->AtEnd(); tag = prisonerData->Next() )
				{
					if( tag.empty() )
						continue;
					UTag = tag.upper();
					data = prisonerData->GrabData();
					switch( (tag.data()[0]) )
					{
						case 'C':
							if( UTag == "CELL" )
								cellNumber = data.toUByte();
							break;
						case 'O':
							if( UTag == "OLDX" )
								toPush->x = data.toShort();
							else if( UTag == "OLDY" )
								toPush->y = data.toShort();
							else if( UTag == "OLDZ" )
								toPush->z = data.toByte();
							break;
						case 'R':
							if( UTag == "RELEASE" )
								toPush->releaseTime = data.toLong();
							break;
						case 'S':
							if( UTag == "SERIAL" )
								toPush->pSerial = data.toULong();
							break;
					}
				}
				if( cellNumber < jails.size() )
					jails[cellNumber].AddOccupant( toPush );
				else
					jails[RandomNum( static_cast< size_t >(0), jails.size() - 1 )].AddOccupant( toPush );
			}
		}
		delete jailData;
	}
}
void JailSystem::WriteData( void )
{
	std::string jailsFile = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "jails.wsc";
	std::ofstream jailsDestination( jailsFile.c_str() );
	if( !jailsDestination )
	{
		Console.Error( 1, "Failed to open %s for writing", jailsFile.c_str() );
		return;
	}
	for( size_t jCtr = 0; jCtr < jails.size(); ++jCtr )
	{
		jails[jCtr].WriteData( jailsDestination, jCtr );
	}
	jailsDestination.close();
}
void JailSystem::PeriodicCheck( void )
{
	std::vector< JailCell >::iterator jIter;
	for( jIter = jails.begin(); jIter != jails.end(); ++jIter )
		(*jIter).PeriodicCheck();
}
void JailSystem::ReleasePlayer( CChar *toRelease )
{
	if( !ValidateObject( toRelease ) )
		return;
	SI08 cellNum = toRelease->GetCell();
	if( cellNum < 0 || cellNum >= static_cast<SI08>(jails.size()) )
		return;
	for( UI32 iCounter = 0; iCounter < jails[cellNum].JailedPlayers(); ++iCounter )
	{
		JailOccupant *mOccupant = jails[cellNum].Occupant( iCounter );
		if( mOccupant == NULL )
			continue;
		if( mOccupant->pSerial == toRelease->GetSerial() )
		{
			toRelease->SetLocation( mOccupant->x, mOccupant->y, mOccupant->z );
			toRelease->SetCell( -1 );
			jails[cellNum].EraseOccupant( iCounter );
			return;
		}
	}
}
bool JailSystem::JailPlayer( CChar *toJail, SI32 numSecsToJail )
{
	if( jails.size() == 0 || toJail == NULL )
		return false;
	int minCell = 0;
	for( UI32 i = 0; i < jails.size(); ++i )
	{
		if( jails[i].IsEmpty() )
		{
			minCell = i;
			break;
		}
		else if( jails[i].JailedPlayers() < jails[minCell].JailedPlayers() )
			minCell = i;
	}

	jails[minCell].AddOccupant( toJail, numSecsToJail );
	toJail->SetCell( minCell );
	return true;
}

}
