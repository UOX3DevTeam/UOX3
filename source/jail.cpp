#include "uox3.h"
#include "jail.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "scriptc.h"
#include "StringUtility.hpp"

JailSystem *JailSys;

JailCell::~JailCell()
{
	for( size_t i = 0; i < playersInJail.size(); ++i )
	{
		delete playersInJail[i];
	}
	playersInJail.resize( 0 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsEmpty( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Checks if jailcell is empty
//o-----------------------------------------------------------------------------------------------o
bool JailCell::IsEmpty( void ) const
{
	return playersInJail.empty();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	size_t JailCell::JailedPlayers( void ) const
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns number of players in jailcell
//o-----------------------------------------------------------------------------------------------o
size_t JailCell::JailedPlayers( void ) const
{
	return playersInJail.size();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 X( void ) const
//|					void X( SI16 nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets X coordinate for jailcell
//o-----------------------------------------------------------------------------------------------o
SI16 JailCell::X( void ) const
{
	return x;
}
void JailCell::X( SI16 nVal )
{
	x = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI16 Y( void ) const
//|					void Y( SI16 nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets Y coordinate for jailcell
//o-----------------------------------------------------------------------------------------------o
SI16 JailCell::Y( void ) const
{
	return y;
}
void JailCell::Y( SI16 nVal )
{
	y = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI08 Z( void ) const
//|					void Z( SI08 nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets Z coordinate for jailcell
//o-----------------------------------------------------------------------------------------------o
SI08 JailCell::Z( void ) const
{
	return z;
}
void JailCell::Z( SI08 nVal )
{
	z = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI08 World( void ) const
//|					void World( UI08 nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets world number for jailcell
//o-----------------------------------------------------------------------------------------------o
UI08 JailCell::World( void ) const
{
	return world;
}
void JailCell::World( UI08 nVal )
{
	world = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 InstanceID( void ) const
//|					void InstanceID( UI16 nVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Gets/Sets instanceID for jailcell
//o-----------------------------------------------------------------------------------------------o
UI16 JailCell::InstanceID( void ) const
{
	return instanceID;
}
void JailCell::InstanceID( UI16 nVal )
{
	instanceID = nVal;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddOccupant( CChar *pAdd, SI32 secsFromNow )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Add player to jail for a certain amount of time
//o-----------------------------------------------------------------------------------------------o
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
	toAdd->world = pAdd->WorldNumber();
	toAdd->instanceID = pAdd->GetInstanceID();
	pAdd->SetLocation( x, y, z, world, instanceID );
	SendMapChange( pAdd->WorldNumber(), pAdd->GetSocket(), false );
	playersInJail.push_back( toAdd );
}

void JailCell::AddOccupant( JailOccupant *toAdd )
{
	playersInJail.push_back( toAdd );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void EraseOccupant( size_t occupantID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove player from jail
//o-----------------------------------------------------------------------------------------------o
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
		return nullptr;
	return playersInJail[occupantID];
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PeriodicCheck( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Perform period check of each player in jailcell, and release them if time is up
//o-----------------------------------------------------------------------------------------------o
void JailCell::PeriodicCheck( void )
{
	time_t now;
	time( &now );
	for( size_t i = playersInJail.size() - 1; i >= 0 && i < playersInJail.size(); --i )
	{
		if( difftime( now, playersInJail[i]->releaseTime ) >= 0 )
		{	// time to release them
			CChar *toRelease = calcCharObjFromSer( playersInJail[i]->pSerial );
			if( !ValidateObject( toRelease ) )
				EraseOccupant( i );
			else
			{
				toRelease->SetLocation( playersInJail[i]->x, playersInJail[i]->y, playersInJail[i]->z, playersInJail[i]->world, playersInJail[i]->instanceID );
				SendMapChange( toRelease->WorldNumber(), toRelease->GetSocket(), false );
				toRelease->SetCell( -1 );
				EraseOccupant( i );
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void WriteData( std::ofstream &outStream, size_t cellNumber )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Save out data on jailed players to stream
//o-----------------------------------------------------------------------------------------------o
void JailCell::WriteData( std::ofstream &outStream, size_t cellNumber )
{
	std::vector< JailOccupant * >::const_iterator jIter;
	for( jIter = playersInJail.begin(); jIter != playersInJail.end(); ++jIter )
	{
		JailOccupant *mOccupant = (*jIter);
		if( mOccupant != nullptr )
		{
			outStream << "[PRISONER]" << '\n' << "{" << '\n';
			outStream << "CELL=" << cellNumber << '\n';
			outStream << "SERIAL=" << std::hex << mOccupant->pSerial << '\n';
			outStream << "OLDX=" << std::dec << mOccupant->x << '\n';
			outStream << "OLDY=" << mOccupant->y << '\n';
			outStream << "OLDZ=" << (SI16)mOccupant->z << '\n';
			outStream << "WORLD=" << mOccupant->world << '\n';
			outStream << "INSTANCEID=" << mOccupant->instanceID << '\n';
			outStream << "RELEASE=" << mOccupant->releaseTime << '\n';
			outStream << "}" << '\n' << '\n';
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ReadSetup( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Load jail-cell setup from regions DFNs, otherwise use default, hardcoded setup
//o-----------------------------------------------------------------------------------------------o
void JailSystem::ReadSetup( void )
{
	ScriptSection *Regions = FileLookup->FindEntry( "JAILS", regions_def );
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
		JailCell toAdd;
		std::string tag;
		std::string data;
		for( tag = Regions->First(); !Regions->AtEnd(); tag = Regions->Next() )
		{
			if( tag.empty() )
				continue;
			data = Regions->GrabData();
			data = strutil::stripTrim( data );
			switch( (tag.data()[0]) )
			{
				case 'X':	toAdd.X( static_cast<SI16>(std::stoi(data, nullptr, 0)) );	break;
				case 'Y':	toAdd.Y( static_cast<SI16>(std::stoi(data, nullptr, 0)) );	break;
				case 'Z':
					toAdd.Z( static_cast<SI08>(std::stoi(data, nullptr, 0)) );
					jails.push_back( toAdd );
					break;
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ReadData( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Load data on jailed players from jails.wsc in shared folder
//o-----------------------------------------------------------------------------------------------o
void JailSystem::ReadData( void )
{
	if( jails.empty() )
	{
		return;
	}
	std::string temp	= cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "jails.wsc";
	if( FileExists( temp ) )
	{
		Script *jailData = new Script( temp, NUM_DEFS, false );
		if( jailData != nullptr )
		{
			ScriptSection *prisonerData = nullptr;
			for( prisonerData = jailData->FirstEntry(); prisonerData != nullptr; prisonerData = jailData->NextEntry() )
			{
				if( prisonerData == nullptr )
				{
					continue;
				}
				std::string tag;
				std::string data;
				std::string UTag;
				JailOccupant toPush;
				UI08 cellNumber = 0;
				for( tag = prisonerData->First(); !prisonerData->AtEnd(); tag = prisonerData->Next() )
				{
					if( tag.empty() )
					{
						continue;
					}
					UTag = strutil::toupper( tag );
					data = prisonerData->GrabData();
					data = strutil::stripTrim( data );
					switch( (UTag.data()[0]) )
					{
						case 'C':
							if( UTag == "CELL" )
							{
								cellNumber = static_cast<UI08>(std::stoul(data, nullptr, 0));
							}
							break;
						case 'O':
							if( UTag == "OLDX" )
							{
								toPush.x = static_cast<SI16>(std::stoi(data, nullptr, 0));
							}
							else if( UTag == "OLDY" )
							{
								toPush.y = static_cast<SI16>(std::stoi(data, nullptr, 0));
							}
							else if( UTag == "OLDZ" )
							{
								toPush.z = static_cast<SI08>(std::stoi(data, nullptr, 0));
							}
							else if( UTag == "WORLD" )
							{
								toPush.world = static_cast<SI08>(std::stoi(data, nullptr, 0));
							}
							break;
						case 'R':
							if( UTag == "RELEASE" )
							{
								toPush.releaseTime = std::stoi(data, nullptr, 0);
							}
							break;
						case 'S':
							if( UTag == "SERIAL" )
							{
								toPush.pSerial = static_cast<UI32>(std::stoul(data, nullptr, 0));
							}
							break;
					}
				}
				if( cellNumber < jails.size() )
				{
					jails[cellNumber].AddOccupant( &toPush );
				}
				else
				{
					jails[RandomNum( static_cast<size_t>(0), jails.size() - 1 )].AddOccupant( &toPush );
				}
			}
		}
		delete jailData;
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void WriteData( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Save out details about jailed players to jails.wsc in shared folder
//o-----------------------------------------------------------------------------------------------o
void JailSystem::WriteData( void )
{
	std::string jailsFile = cwmWorldState->ServerData()->Directory( CSDDP_SHARED ) + "jails.wsc";
	std::ofstream jailsDestination( jailsFile.c_str() );
	if( !jailsDestination )
	{
		Console.error( strutil::format("Failed to open %s for writing", jailsFile.c_str() ));
		return;
	}
	for( size_t jCtr = 0; jCtr < jails.size(); ++jCtr )
	{
		jails[jCtr].WriteData( jailsDestination, jCtr );
	}
	jailsDestination.close();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PeriodicCheck( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Perform period check of each jailcell
//o-----------------------------------------------------------------------------------------------o
void JailSystem::PeriodicCheck( void )
{
	std::vector< JailCell >::iterator jIter;
	for( jIter = jails.begin(); jIter != jails.end(); ++jIter )
		(*jIter).PeriodicCheck();
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void ReleasePlayer( CChar *toRelease )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Release player from jail
//o-----------------------------------------------------------------------------------------------o
void JailSystem::ReleasePlayer( CChar *toRelease )
{
	if( !ValidateObject( toRelease ) )
		return;
	SI08 cellNum = toRelease->GetCell();
	if( cellNum < 0 || cellNum >= static_cast<SI08>(jails.size()) )
		return;
	for( size_t iCounter = 0; iCounter < jails[cellNum].JailedPlayers(); ++iCounter )
	{
		JailOccupant *mOccupant = jails[cellNum].Occupant( iCounter );
		if( mOccupant == nullptr )
			continue;
		if( mOccupant->pSerial == toRelease->GetSerial() )
		{
			toRelease->SetLocation( mOccupant->x, mOccupant->y, mOccupant->z, mOccupant->world, mOccupant->instanceID );
			SendMapChange( mOccupant->world, toRelease->GetSocket(), false );
			toRelease->SetCell( -1 );
			jails[cellNum].EraseOccupant( iCounter );
			return;
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool JailPlayer( CChar *toJail, SI32 numSecsToJail )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Send player to jail for a certain amount of time
//o-----------------------------------------------------------------------------------------------o
bool JailSystem::JailPlayer( CChar *toJail, SI32 numSecsToJail )
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
			minCell = i;
	}

	jails[minCell].AddOccupant( toJail, numSecsToJail );
	toJail->SetCell( static_cast<SI08>(minCell) );
	return true;
}
