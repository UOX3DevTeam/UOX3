#include "uox3.h"
#include "jail.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "scriptc.h"
#include "StringUtility.hpp"

#include <memory>
using namespace std::string_literals;


CJailSystem *JailSys;

CJailCell::~CJailCell()
{
	for( size_t i = 0; i < playersInJail.size(); ++i )
	{
		delete playersInJail[i];
	}
	playersInJail.resize( 0 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailCell::IsEmpty()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if jailcell is empty
//o------------------------------------------------------------------------------------------------o
bool CJailCell::IsEmpty( void ) const
{
	return playersInJail.empty();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailCell::JailedPlayers()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns number of players in jailcell
//o------------------------------------------------------------------------------------------------o
size_t CJailCell::JailedPlayers( void ) const
{
	return playersInJail.size();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailCell::X()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets X coordinate for jailcell
//o------------------------------------------------------------------------------------------------o
SI16 CJailCell::X( void ) const
{
	return x;
}
void CJailCell::X( SI16 nVal )
{
	x = nVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailCell::Y()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets Y coordinate for jailcell
//o------------------------------------------------------------------------------------------------o
SI16 CJailCell::Y( void ) const
{
	return y;
}
void CJailCell::Y( SI16 nVal )
{
	y = nVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailCell::Z()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets Z coordinate for jailcell
//o------------------------------------------------------------------------------------------------o
SI08 CJailCell::Z( void ) const
{
	return z;
}
void CJailCell::Z( SI08 nVal )
{
	z = nVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailCell::()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets world number for jailcell
//o------------------------------------------------------------------------------------------------o
UI08 CJailCell::World( void ) const
{
	return world;
}
void CJailCell::World( UI08 nVal )
{
	world = nVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailCell::InstanceId()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets instanceId for jailcell
//o------------------------------------------------------------------------------------------------o
UI16 CJailCell::InstanceId( void ) const
{
	return instanceId;
}
void CJailCell::InstanceId( UI16 nVal )
{
	instanceId = nVal;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailCell::AddOccupant()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Add player to jail for a certain amount of time
//o------------------------------------------------------------------------------------------------o
void CJailCell::AddOccupant( CChar *pAdd, SI32 secsFromNow )
{
	if( !ValidateObject( pAdd ))
		return;

	JailOccupant_st *toAdd = new JailOccupant_st;
	time( &( toAdd->releaseTime ));
	toAdd->releaseTime += secsFromNow;
	toAdd->pSerial = pAdd->GetSerial();
	toAdd->x = pAdd->GetX();
	toAdd->y = pAdd->GetY();
	toAdd->z = pAdd->GetZ();
	toAdd->world = pAdd->WorldNumber();
	toAdd->instanceId = pAdd->GetInstanceId();
	pAdd->SetLocation( x, y, z, world, instanceId );
	SendMapChange( pAdd->WorldNumber(), pAdd->GetSocket(), false );
	playersInJail.push_back( toAdd );
}

void CJailCell::AddOccupant( JailOccupant_st *toAdd )
{
	playersInJail.push_back( toAdd );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailCell::EraseOccupant()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove player from jail
//o------------------------------------------------------------------------------------------------o
void CJailCell::EraseOccupant( size_t occupantId )
{
	if( occupantId >= playersInJail.size() )
		return;

	delete playersInJail[occupantId];
	playersInJail.erase( playersInJail.begin() + occupantId );
}
JailOccupant_st *CJailCell::Occupant( size_t occupantId )
{
	if( occupantId >= playersInJail.size() )
		return nullptr;

	return playersInJail[occupantId];
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailCell::PeriodicCheck()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Perform period check of each player in jailcell, and release them if time is up
//o------------------------------------------------------------------------------------------------o
void CJailCell::PeriodicCheck( void )
{
	time_t now;
	time( &now );
	for( size_t i = playersInJail.size(); i-- > 0 && i < playersInJail.size(); )
	{
		if( difftime( now, playersInJail[i]->releaseTime ) >= 0 )
		{	// time to release them
			CChar *toRelease = CalcCharObjFromSer( playersInJail[i]->pSerial );
			if( !ValidateObject( toRelease ))
			{
				EraseOccupant( i );
			}
			else
			{
				toRelease->SetLocation( playersInJail[i]->x, playersInJail[i]->y, playersInJail[i]->z, playersInJail[i]->world, playersInJail[i]->instanceId );
				SendMapChange( toRelease->WorldNumber(), toRelease->GetSocket(), false );
				toRelease->SetCell( -1 );
				EraseOccupant( i );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailCell::WriteData()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Save out data on jailed players to stream
//o------------------------------------------------------------------------------------------------o
void CJailCell::WriteData( std::ofstream &outStream, size_t cellNumber )
{
	std::vector<JailOccupant_st *>::const_iterator jIter;
	for( jIter = playersInJail.begin(); jIter != playersInJail.end(); ++jIter )
	{
		JailOccupant_st *mOccupant = ( *jIter );
		if( mOccupant != nullptr )
		{
			outStream << "[PRISONER]" << '\n' << "{" << '\n';
			outStream << "CELL=" << cellNumber << '\n';
			outStream << "SERIAL=" << std::hex << mOccupant->pSerial << '\n';
			outStream << "OLDX=" << std::dec << mOccupant->x << '\n';
			outStream << "OLDY=" << mOccupant->y << '\n';
			outStream << "OLDZ=" << static_cast<SI16>( mOccupant->z ) << '\n';
			outStream << "WORLD=" << mOccupant->world << '\n';
			outStream << "INSTANCEID=" << mOccupant->instanceId << '\n';
			outStream << "RELEASE=" << mOccupant->releaseTime << '\n';
			outStream << "}" << '\n' << '\n';
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailSystem::ReadSetup()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load jail-cell setup from regions DFNs, otherwise use default, hardcoded setup
//o------------------------------------------------------------------------------------------------o
auto CJailSystem::ReadSetup() -> void
{
	auto Regions = FileLookup->FindEntry( "JAILS", regions_def );
	if( Regions == nullptr )
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
		CJailCell toAdd;
		for( const auto &sec :Regions->collection() )
		{
			auto tag = sec->tag;
			auto data = sec->data;
			if( !tag.empty() )
			{
				data = oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ));
				switch(( tag.data()[0] ))
				{
					case 'X':	toAdd.X( static_cast<SI16>( std::stoi( data, nullptr, 0 )));	break;
					case 'Y':	toAdd.Y( static_cast<SI16>( std::stoi( data, nullptr, 0 )));	break;
					case 'Z':
						toAdd.Z( static_cast<SI08>( std::stoi( data, nullptr, 0 )));
						jails.push_back( toAdd );
						break;
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailCell::ReadData()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Load data on jailed players from jails.wsc in shared folder
//o------------------------------------------------------------------------------------------------o
auto CJailSystem::ReadData() -> void
{
	if( !jails.empty() )
	{
		auto temp = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "jails.wsc";
		if( !FileExists( temp ))
			return;

		auto jailData = std::make_unique<Script>( temp, NUM_DEFS, false );
		if( !jailData )
			return;

		for( const auto &[id, prisonerData] : jailData->collection() )
		{
			if( !prisonerData )
				continue;

			JailOccupant_st toPush;
			UI08 cellNumber = 0;
			for( const auto &sec : prisonerData->collection() )
			{
				auto tag = sec->tag;
				if( !tag.empty() )
				{
					auto UTag = oldstrutil::upper( tag );
					auto data = sec->data;
					data = oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ));
					switch(( UTag.data()[0] ))
					{
						case 'C':
							if( UTag == "CELL" )
							{
								cellNumber = static_cast<UI08>( std::stoul( data, nullptr, 0 ));
							}
							break;
						case 'O':
							if( UTag == "OLDX" )
							{
								toPush.x = static_cast<SI16>( std::stoi( data, nullptr, 0 ));
							}
							else if( UTag == "OLDY" )
							{
								toPush.y = static_cast<SI16>( std::stoi( data, nullptr, 0 ));
							}
							else if( UTag == "OLDZ" )
							{
								toPush.z = static_cast<SI08>( std::stoi( data, nullptr, 0 ));
							}
							else if( UTag == "WORLD" )
							{
								toPush.world = static_cast<SI08>( std::stoi( data, nullptr, 0 ));
							}
							break;
						case 'R':
							if( UTag == "RELEASE" )
							{
								toPush.releaseTime = std::stoi( data, nullptr, 0);
							}
							break;
						case 'S':
							if( UTag == "SERIAL" )
							{
								toPush.pSerial = static_cast<UI32>( std::stoul( data, nullptr, 0 ));
							}
							break;
					}
				}
			}
			if( cellNumber < jails.size() )
			{
				jails[cellNumber].AddOccupant( &toPush );
			}
			else
			{
				jails[RandomNum( static_cast<size_t>( 0 ), jails.size() - 1 )].AddOccupant( &toPush );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailSystem::WriteData()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Save out details about jailed players to jails.wsc in shared folder
//o------------------------------------------------------------------------------------------------o
void CJailSystem::WriteData( void )
{
	std::string jailsFile = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "jails.wsc";
	std::ofstream jailsDestination( jailsFile.c_str() );
	if( !jailsDestination )
	{
		Console.Error( oldstrutil::format( "Failed to open %s for writing", jailsFile.c_str() ));
		return;
	}
	for( size_t jCtr = 0; jCtr < jails.size(); ++jCtr )
	{
		jails[jCtr].WriteData( jailsDestination, jCtr );
	}
	jailsDestination.close();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailSystem::PeriodicCheck()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Perform period check of each jailcell
//o------------------------------------------------------------------------------------------------o
void CJailSystem::PeriodicCheck( void )
{
	std::vector<CJailCell>::iterator jIter;
	for( jIter = jails.begin(); jIter != jails.end(); ++jIter )
	{
		( *jIter ).PeriodicCheck();
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailSystem::ReleasePlayer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Release player from jail
//o------------------------------------------------------------------------------------------------o
void CJailSystem::ReleasePlayer( CChar *toRelease )
{
	if( !ValidateObject( toRelease ))
		return;

	SI08 cellNum = toRelease->GetCell();
	if( cellNum < 0 || cellNum >= static_cast<SI08>( jails.size() ))
		return;

	for( size_t iCounter = 0; iCounter < jails[cellNum].JailedPlayers(); ++iCounter )
	{
		JailOccupant_st *mOccupant = jails[cellNum].Occupant( iCounter );
		if( mOccupant == nullptr )
			continue;

		if( mOccupant->pSerial == toRelease->GetSerial() )
		{
			toRelease->SetLocation( mOccupant->x, mOccupant->y, mOccupant->z, mOccupant->world, mOccupant->instanceId );
			SendMapChange( mOccupant->world, toRelease->GetSocket(), false );
			toRelease->SetCell( -1 );
			jails[cellNum].EraseOccupant( iCounter );
			return;
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CJailSystem::JailPlayer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send player to jail for a certain amount of time
//o------------------------------------------------------------------------------------------------o
bool CJailSystem::JailPlayer( CChar *toJail, SI32 numSecsToJail )
{
	if( jails.empty() || toJail == nullptr )
		return false;

	size_t minCell = 0;
	for( size_t i = 0; i < jails.size(); ++i )
	{
		if( jails[i].IsEmpty() )
		{
			minCell = i;
			break;
		}
		else if( jails[i].JailedPlayers() < jails[minCell].JailedPlayers() )
		{
			minCell = i;
		}
	}

	jails[minCell].AddOccupant( toJail, numSecsToJail );
	toJail->SetCell( static_cast<SI08>( minCell ));
	return true;
}
