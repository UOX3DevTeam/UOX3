#include "jail.h"
#include "ssection.h"

bool JailCell::IsEmpty( void ) const		
{ 
	return playersInJail.size() == 0; 
}
UI32 JailCell::JailedPlayers( void ) const 
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
	if( pAdd == NULL )
		return;
	JailOccupant *toAdd = new JailOccupant; 
	time( &(toAdd->releaseTime) ); 
	toAdd->releaseTime += secsFromNow; 
	toAdd->pSerial = pAdd->GetSerial();
	toAdd->x = pAdd->GetX();
	toAdd->y = pAdd->GetY();
	toAdd->z = pAdd->GetZ();
	pAdd->SetLocation( x, y, z );
	pAdd->Teleport();
	playersInJail.push_back( toAdd );
}

void JailCell::AddOccupant( JailOccupant *toAdd )
{
	playersInJail.push_back( toAdd );
}

void JailCell::EraseOccupant( SI32 occupantID )
{
	if( occupantID < 0 || occupantID >= static_cast<int>(playersInJail.size()) )
		return;
	delete playersInJail[occupantID];
	playersInJail.erase( playersInJail.begin() + occupantID );
}
JailOccupant *JailCell::Occupant( SI32 occupantID ) 
{ 
	if( occupantID < 0 || occupantID >= static_cast<int>(playersInJail.size()) )
		return NULL;
	return playersInJail[occupantID];
}
void JailCell::PeriodicCheck( void )
{
	time_t now;
	time( &now );
	for( int i = playersInJail.size() - 1; i >= 0; i-- )
	{
		if( difftime( now, playersInJail[i]->releaseTime ) < 0 )
		{	// time to release them
			CChar *toRelease = calcCharObjFromSer( playersInJail[i]->pSerial );
			if( toRelease == NULL )
				EraseOccupant( i );
			else
			{
				toRelease->SetLocation( playersInJail[i]->x, playersInJail[i]->y, playersInJail[i]->z );
				toRelease->Teleport();
				toRelease->SetCell( -1 );
				EraseOccupant( i );
			}
		}
	}
}

JailSystem::JailSystem()
{
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
		SI16 x = 0, y = 0;
		SI08 z = 0;
		const char *tag = NULL;
		const char *data = NULL;
		UI08 currentJail = 0;
		for( tag = Regions->First(); !Regions->AtEnd(); tag = Regions->Next() )
		{
			if( tag == NULL )
				continue;
			data = Regions->GrabData();
			switch( tag[0] )
			{
			case 'X':	x = (SI16)makeNum( data );	break;
			case 'Y':	y = (SI16)makeNum( data );	break;
			case 'Z':	
						z = (SI08)makeNum( data );
						jails.resize( jails.size() + 1 );
						jails[currentJail].X( x );
						jails[currentJail].Y( y );
						jails[currentJail].Z( z );
						currentJail++;
						break;
			}
		}
	}
}
void JailSystem::ReadData( void )
{
	if( jails.size() == 0 )
		return;
	FILE *toOpen = NULL;
	char temp[MAX_PATH];
	sprintf( temp, "%sjails.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );
	toOpen = fopen( temp, "r" );
	if( toOpen == NULL )
	{
		Console.Error( 1, "Failed to open %s for reading", temp );
		return;
	}
	fclose( toOpen );
	Script *jailData = new Script( temp, NUM_DEFS, false );
	if( jailData != NULL )
	{
		ScriptSection *prisonerData = NULL;
		for( prisonerData = jailData->FirstEntry(); prisonerData != NULL; prisonerData = jailData->NextEntry() )
		{
			if( prisonerData == NULL )
				continue;
			const char *tag = NULL;
			const char *data = NULL;
			JailOccupant *toPush = new JailOccupant;
			UI08 cellNumber = 0;
			for( tag = prisonerData->First(); !prisonerData->AtEnd(); tag = prisonerData->Next() )
			{
				if( tag == NULL )
					continue;
				data = prisonerData->GrabData();
				switch( tag[0] )
				{
				case 'C':
					if( !strcmp( tag, "CELL" ) )
						cellNumber = static_cast<UI08>(makeNum( data ));
					break;
				case 'O':
					if( !strcmp( tag, "OLDX" ) )
						toPush->x = static_cast<SI16>(makeNum( data ));
					else if( !strcmp( tag, "OLDY" ) )
						toPush->y = static_cast<SI16>(makeNum( data ));
					else if( !strcmp( tag, "OLDZ" ) )
						toPush->z = static_cast<SI08>(makeNum( data) );
					break;
				case 'R':
					if( !strcmp( tag, "RELEASE" ) )
						toPush->releaseTime = makeNum( data );
					break;
				case 'S':
					if( !strcmp( tag, "SERIAL" ) )
						toPush->pSerial = makeNum( data );
					break;
				}
			}
			if( cellNumber < jails.size() )
				jails[cellNumber].AddOccupant( toPush );
			else
				jails[RandomNum( 0, jails.size() - 1 )].AddOccupant( toPush );
		}
	}
	delete jailData;
}
void JailSystem::WriteData( void )
{
	FILE *toWrite = NULL;
	char temp[MAX_PATH];
	sprintf( temp, "%s/jails.wsc", cwmWorldState->ServerData()->GetSharedDirectory() );
	toWrite = fopen( temp, "w" );
	if( toWrite == NULL )
	{
		Console.Error( 1, "Failed to open %s for writing", temp );
		return;
	}
	for( UI32 jCtr = 0; jCtr < jails.size(); jCtr++ )
	{
		for( UI32 cCtr = 0; cCtr < jails[jCtr].JailedPlayers(); cCtr++ )
		{
			JailOccupant *mOccupant = jails[jCtr].Occupant( cCtr );
			if( mOccupant != NULL )
			{
				fprintf( toWrite, "[PRISONER]\n{\n" );
				fprintf( toWrite, "CELL=%i\n", jCtr );
				fprintf( toWrite, "SERIAL=%i\n", mOccupant->pSerial );
				fprintf( toWrite, "OLDX=%i\n", mOccupant->x );
				fprintf( toWrite, "OLDY=%i\n", mOccupant->y );
				fprintf( toWrite, "OLDZ=%i\n", mOccupant->z );
				fprintf( toWrite, "RELEASE=%i\n", mOccupant->releaseTime );
				fprintf( toWrite, "}\n\n" );
			}
		}
	}
	fclose(toWrite);
}
void JailSystem::PeriodicCheck( void )
{
	for( UI32 i = 0; i < jails.size(); i++ )
		jails[i].PeriodicCheck();
}
void JailSystem::ReleasePlayer( CChar *toRelease )
{
	if( toRelease == NULL )
		return;
	SI08 cellNum = toRelease->GetCell();
	if( cellNum < 0 || cellNum >= static_cast<int>(jails.size()) )
		return;
	for( UI32 iCounter = 0; iCounter < jails[cellNum].JailedPlayers(); iCounter++ )
	{
		JailOccupant *mOccupant = jails[cellNum].Occupant( iCounter );
		if( mOccupant == NULL )
			continue;
		if( mOccupant->pSerial == toRelease->GetSerial() )
		{
			toRelease->SetLocation( mOccupant->x, mOccupant->y, mOccupant->z );
			toRelease->SetCell( -1 );
			toRelease->Teleport();
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
	for( UI32 i = 0; i < jails.size(); i++ )
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
