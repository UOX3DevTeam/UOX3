#include "uox3.h"
#include "debug.h"
#include "cmdtable.h"
#include "ssection.h"

#undef DBGFILE
#define DBGFILE "commands.cpp"

void CommandReset( void );

//o---------------------------------------------------------------------------o
//|	Function	-	void splitline( char *toSplit )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Puts single words of cline into a comm array
//o---------------------------------------------------------------------------o
void splitline( char *toSplit )
{
	int i = 0;
	char *d = " ";
	char *s = strtok( toSplit, d );
	while( s != NULL )
	{
		comm[i] = s;
		i++;
		s = strtok( NULL, d );
	}
	tnum = i;
}

cCommands::cCommands()
{
	CommandReset();
}

//o---------------------------------------------------------------------------o
//|	Function	-	static inline void doTarget( cSocket *s, TARGET_S *ts )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Do targeting stuff
//o---------------------------------------------------------------------------o
static inline void doTarget( cSocket *s, TARGET_S *ts )
{
	target( s, ts->a1, ts->a2, ts->a3, ts->a4, ts->dictEntry );
}

//o--------------------------------------------------------------------------o
//|	Function			-	void cCommands::Command( cSocket *s )
//|	Date					-	
//|	Developers		-	EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Handles commands sent from client
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void cCommands::Command( cSocket *s )
{
	char *comm;
	char nonuni[512];
	cmd_offset = 9;

	memset( s->TBuffer(), 0, MAXBUFFER );
	CChar *mChar = s->CurrcharObj();
	char *tbuffer = (char *)s->TBuffer();
	int i = 9;
	if( !mChar->isUnicode() )
	{
		for( i = 9; s->GetByte( i ) != 0; i++ )
		{
			tbuffer[i] = s->GetByte( i );
			s->SetByte( i, toupper( s->GetByte( i ) ) );
		}
		tbuffer[i] = 0;
		splitline( (char *)&(s->Buffer()[8]) );
		if( tnum < 1 ) 
			return;
		comm = (char *)&(s->Buffer()[9]); 
	}
	else
	{
		cmd_offset = 14;
		// we will have to convert from unicode to non-unicode
		for( i = 13; i < s->GetWord( 1 ); i += 2 )
			nonuni[( i - 13 ) / 2] = s->GetByte( i );
		i = 14;
		while( nonuni[i - 13] != 0 )
		{
			tbuffer[i] = nonuni[i-13];
			nonuni[i-13] = toupper( nonuni[i-13] );
			i++;
		} 
		tbuffer[i] = 0; 
		splitline( &nonuni[1] );
		if( tnum < 1 ) 
			return; 
		comm = &nonuni[1];
	} 

	CmdTableIterator toFind = cmd_table.find( comm );
	if( toFind == cmd_table.end() ) 
	{ 	
		cScript *toGrab=Trigger->GetScript( s->CurrcharObj()->GetScriptTrigger() ); 	
		if( toGrab == NULL || !toGrab->OnCommand(s)) 		
			sysmessage( s, 336 ); 	
		return; 
	} 	
	else
	{
		bool plClearance = ( mChar->GetCommandLevel() >= toFind->second.cmdLevelReq || mChar->GetAccount().wAccountIndex == 0 );
		// from now on, account 0 ALWAYS has admin access, regardless of command level
		if( !plClearance )
		{
			Log( comm, mChar, NULL, "Insufficient clearance" );
			sysmessage( s, 337 );
			return;
		}
		Log( comm, mChar, NULL, "Cleared" );

		switch( toFind->second.cmdType ) 
		{
		case CMD_FUNC:
			(*((CMD_EXEC)toFind->second.cmd_extra)) (s);
			break;
		case CMD_ITEMMENU:
			NewAddMenu( s, (int)toFind->second.cmd_extra );
			break;
		case CMD_TARGET:
			doTarget(s, (TARGET_S *)toFind->second.cmd_extra );
			break;
		case CMD_TARGETX:
			if( tnum == 2 ) 
			{
				s->AddX( makenumber( 1 ) );
				doTarget( s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 338 );
			break;
		case CMD_TARGETXY:
			if( tnum == 3 ) 
			{
				s->AddX( makenumber( 1 ) );
				s->AddY( makenumber( 2 ) );
				doTarget( s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 339 );
			break;
		case CMD_TARGETXYZ:
			if( tnum == 4 ) 
			{
				s->AddX( makenumber( 1 ) );
				s->AddY( makenumber( 2 ) );
				s->AddZ( makenumber( 3 ) );
				doTarget( s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 340 );
			break;
		case CMD_TARGETHX:
			if( tnum == 2 ) 
			{
				s->AddX( makenumber( 1 ) );
				doTarget( s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 341 );
			break;
		case CMD_TARGETHXY:
			if( tnum == 3 ) 
			{
				s->AddX( makenumber( 1 ) );
				s->AddY( makenumber( 2 ) );
				doTarget( s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 342 );
			break;
		case CMD_TARGETHXYZ:
			if( tnum == 4 ) 
			{
				s->AddX( makenumber( 1 ) );
				s->AddY( makenumber( 2 ) );
				s->AddZ( makenumber( 3 ) );
				doTarget( s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 343 );
			break;
		case CMD_TARGETID1:
			if( tnum == 2 ) 
			{
				s->AddID1( makenumber( 1 ) );
				doTarget( s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 338 );
			break;
		case CMD_TARGETID2:
			if( tnum == 3 ) 
			{
				s->AddID1( makenumber( 1 ) );
				s->AddID2( makenumber( 2 ) );
				doTarget( s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 339 );
			break;
		case CMD_TARGETID3:
			if( tnum == 4 ) 
			{
				s->AddID1( makenumber( 1 ) );
				s->AddID2( makenumber( 2 ) );
				s->AddID3( makenumber( 3 ) );
				doTarget( s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 340 );
			break;
		case CMD_TARGETID4:
			if( tnum == 5 ) 
			{
				s->AddID1( makenumber( 1 ) );
				s->AddID2( makenumber( 2 ) );
				s->AddID3( makenumber( 3 ) );
				s->AddID4( makenumber( 4 ) );
				doTarget( s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 344 );
			break;
		case CMD_TARGETHID1:
			if( tnum == 2 ) 
			{
				s->AddID1( makenumber( 1 ) );
				doTarget( s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 341 );
			break;
		case CMD_TARGETHID2:
			if( tnum == 3 ) 
			{
				s->AddID1( makenumber( 1 ) );
				s->AddID2( makenumber( 2 ) );
				doTarget( s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 342 );
			break;
		case CMD_TARGETHID3:
			if( tnum == 4 ) 
			{
				s->AddID1( makenumber( 1 ) );
				s->AddID2( makenumber( 2 ) );
				s->AddID3( makenumber( 3 ) );
				doTarget( s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 343 );
			break;
		case CMD_TARGETHID4:
			if( tnum == 5 ) 
			{
				s->AddID1( makenumber( 1 ) );
				s->AddID2( makenumber( 2 ) );
				s->AddID3( makenumber( 3 ) );
				s->AddID4( makenumber( 4 ) );
				doTarget( s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 345 );
			break;
		case CMD_TARGETTMP:
			if( tnum == 2 ) 
			{
				s->TempInt( makenumber( 1 ) );
				doTarget( s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 338 );
			break;
		case CMD_TARGETHTMP:
			if( tnum == 2 ) 
			{
				s->TempInt( makenumber( 1 ) );
				doTarget(s, (TARGET_S *)toFind->second.cmd_extra );
			} 
			else 
				sysmessage( s, 341 );
			break;
		default:
			sysmessage( s, 346 );
			break;
		}
		return;
	}
	sysmessage( s, "BUG: Should never reach end of command() function!" );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::MakeShop( CChar *c )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Turn an NPC into a shopkeeper
//o---------------------------------------------------------------------------o
void cCommands::MakeShop( CChar *c )
{
	if( c == NULL )
		return;
	CItem *n = NULL;
	c->SetShop( true );

	cSocket *tSock = calcSocketObjFromChar( c );
	CItem *buyPack = FindItemOnLayer( c, 0x1A );
	CItem *boughtPack = FindItemOnLayer( c, 0x1B );
	CItem *sellPack = FindItemOnLayer( c, 0x1C );
	if( buyPack == NULL )
	{
		n = Items->SpawnItem( tSock, c, 1, "#", false, 0x2AF8, 0, false, false );
		if( n != NULL )
		{
			n->SetLayer( 0x1A );
			if( !n->SetCont( c->GetSerial() ) )
				Items->DeleItem( n );
			else
			{
				n->SetType( 1 );
				n->SetNewbie( true );
			}
		}
	}
	if( boughtPack == NULL )
	{
		n = Items->SpawnItem( tSock, c, 1, "#", false, 0x2AF8, 0, false, false );
		if( n != NULL )
		{
			n->SetLayer( 0x1B );
			if( n->SetCont( c->GetSerial() ) )
			{
				n->SetType( 1 );
				n->SetNewbie( true );
			}
		}
	}
	if( sellPack == NULL )
	{
		n = Items->SpawnItem( tSock, c, 1, "#", false, 0x2AF8, 0, false, false );
		if( n != NULL )
		{
			n->SetLayer( 0x1C );
			if( n->SetCont( c->GetSerial() ) )
			{
				n->SetType( 1 );
				n->SetNewbie( true );
			}
		}
	}

	c->Teleport();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::showQue( cSocket *s, bool isGM )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Shows next unhandled call in the queue
//o---------------------------------------------------------------------------o
void cCommands::showQue( cSocket *s, bool isGM )
{
	if( isGM )
		GMQueue->SendAsGump( s );
	else
		CounselorQueue->SendAsGump( s );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::nextCall( cSocket *s, bool isGM )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send GM/Counsellor to next call in the que
//o---------------------------------------------------------------------------o
void cCommands::nextCall( cSocket *s, bool isGM )
{
	CChar *j = NULL;
	int x = 0;
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetCallNum() != 0 )
		closeCall( s, isGM );
	HelpRequest *tempPage = NULL;
	if( isGM )
	{
		for( tempPage = GMQueue->First(); !GMQueue->AtEnd(); tempPage = GMQueue->Next() )
		{
			if( !tempPage->IsHandled() )
			{
				j = calcCharObjFromSer( tempPage->WhoPaging() );
				if( j != NULL )
				{
					GumpDisplay GMNext( s, 300, 200 );
					GMNext.AddData( "Pager: ", j->GetName() );
					GMNext.AddData( "Problem: ", tempPage->Reason() );
					GMNext.AddData( "Serial number ", tempPage->WhoPaging(), 3 );
					GMNext.AddData( "Paged at: ", tempPage->TimeOfPage() );
					GMNext.Send( 4, false, INVALIDSERIAL );
					tempPage->IsHandled( true );
					mChar->SetLocation( j );
					mChar->SetCallNum( tempPage->RequestID() );
					mChar->Teleport();
					x++;
				}
				if( x > 0 )
					break;
			}
		}
		if( x == 0 ) 
			sysmessage( s, 347 );
	}
	else //Player is a counselor
	{
		x = 0;
		for( tempPage = CounselorQueue->First(); !CounselorQueue->AtEnd(); tempPage = CounselorQueue->Next() )
		{
			if( !tempPage->IsHandled() )
			{
				j = calcCharObjFromSer( tempPage->WhoPaging() );
				if( j != NULL )
				{
					GumpDisplay CNext( s, 300, 200 );
					CNext.AddData( "Pager: ", j->GetName() );
					CNext.AddData( "Problem: ", tempPage->Reason() );
					CNext.AddData( "Serial number ", tempPage->WhoPaging(), 3 );
					CNext.AddData( "Paged at: ", tempPage->TimeOfPage() );
					CNext.Send( 4, false, INVALIDSERIAL );
					tempPage->IsHandled( true );
					mChar->SetCallNum( tempPage->RequestID() );
					mChar->SetLocation( j );
					mChar->Teleport();
					x++;
					break;
				}
			}
			if( x > 0 ) 
				break;
		}
		if( x == 0 ) 
			sysmessage( s, 348 );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::closeCall( cSocket *s, bool isGM )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Closes an open call in the Que
//o---------------------------------------------------------------------------o
void cCommands::closeCall( cSocket *s, bool isGM )
{
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetCallNum() != 0 )
	{
		if( isGM )
		{
			if( GMQueue->GotoPos( GMQueue->FindCallNum( mChar->GetCallNum() ) ) )
			{ 
				GMQueue->Remove();
				mChar->SetCallNum( 0 );
				sysmessage( s, 1285 );
			}
		}
		else
		{
			if( CounselorQueue->GotoPos( CounselorQueue->FindCallNum( mChar->GetCallNum() ) ) )
			{
				CounselorQueue->Remove();
				mChar->SetCallNum( 0 );
				sysmessage( s, 1286 );
			}
		}
	}
	else
		sysmessage( s, 1287 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::KillSpawn( cSocket *s, int r )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Kill spawns in a spawnregion
//o---------------------------------------------------------------------------o
void cCommands::KillSpawn( cSocket *s, int r )
{
	int killed = 0;

	sysmessage( s, 349 );
	for( CHARACTER i = 0; i < charcount; i++ )
	{
		if( chars[i].GetSpawn( 1 ) < 0x40 && chars[i].GetSpawn( 3 ) == r && chars[i].GetSpawn( 2 ) == 1 ) // spawn2 == 1 if spawned by region
		{
			CChar *iDead = &chars[i];
			bolteffect( iDead );
			soundeffect( iDead, 0x0029 );
			Npcs->DeleteChar( iDead );
            killed++;
		}
	}
	gcollect();
	sysmessage( s, "Done." );
	sysmessage( s, 350, killed, r );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::RegSpawnMax( cSocket *s, int r )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Spawn a region to its maximum capacity
//o---------------------------------------------------------------------------o
void cCommands::RegSpawnMax( cSocket *s, cSpawnRegion *spawnReg )
{
	int spawn = ( spawnReg->GetMaxSpawn() - spawnReg->GetCurrent() );
	if( spawn > 250 )
	{
		sysmessage( s, 351 );
		spawn = 250;
	}
  	char temps[650];  //  Adujested to 650 cause spawnregion[].name is 512 bytes long
	sprintf( temps, Dictionary->GetEntry( 352 ), spawnReg->GetName(), spawnReg->GetRegionNum(), spawn );
	sysbroadcast( temps );

	for( int i = 1; i < spawn; i++ )
		spawnReg->doRegionSpawn();
	spawnReg->SetNextTime( BuildTimeValue( 60 * RandomNum( spawnReg->GetMinTime(), spawnReg->GetMaxTime() ) ) );

	sysmessage( s, 353, spawn, spawnReg->GetName(), spawnReg->GetRegionNum() );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::RegSpawnNum( cSocket *s, int r, int n)
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Do a certain number of spawns in a region
//o---------------------------------------------------------------------------o
void cCommands::RegSpawnNum( cSocket *s, cSpawnRegion *spawnReg, int n)
{
	if( n > 250 )
	{
		sysmessage( s, 354 );
		return;
	}
	else
	{
		int spawn = ( spawnReg->GetMaxSpawn() - spawnReg->GetCurrent() );
		char temps[650];  //  Adjusted to 650 because spawnregion[].name is 512 bytes long
		if( spawn < n )
			sysmessage( s, 355, n, spawnReg->GetName(), spawnReg->GetRegionNum(), spawn, spawnReg->GetMaxSpawn() );
		else 
			spawn = n;
		sprintf( temps, Dictionary->GetEntry( 356 ), spawnReg->GetName(), spawnReg->GetRegionNum(), spawn);
		sysbroadcast( temps );
		for( int i = 1; i < spawn; i++ )
			spawnReg->doRegionSpawn();

		spawnReg->SetNextTime( BuildTimeValue( 60 * RandomNum( spawnReg->GetMinTime(), spawnReg->GetMaxTime() ) ) );
		sysmessage( s, 357, spawn, spawnReg->GetName(), spawnReg->GetRegionNum() );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::KillAll( cSocket *s, int percent, char* sysmsg)
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Kill all NPC's in a world
//o---------------------------------------------------------------------------o
void cCommands::KillAll( cSocket *s, int percent, const char* sysmsg )
{
	sysmessage( s, 358 );
	sysbroadcast( sysmsg );
	for( CHARACTER i = 0; i < charcount; i++ )
	{
		if( chars[i].IsNpc() )
		{
			if( RandomNum( 0, 99 ) + 1 <= percent )
			{
				CChar *iDead = &chars[i];
				bolteffect( iDead );
				soundeffect( iDead, 0x0029 );
				doDeathStuff( iDead );
			}
		}
	}
	sysmessage( s, "Done." );
}

//o---------------------------------------------------------------------------o
//|   Function	  -  void CPage( int s, char *reason )
//|   Date		  -  Unknown
//|   Programmer  -  Unknown  (Touched tabstops by EviLDeD Dec 23, 1998)
//o---------------------------------------------------------------------------o
//|   Purpose     -  Used when player pages a counselor
//o---------------------------------------------------------------------------o
void cCommands::CPage( cSocket *s, const char *reason )
{
	CChar *mChar = s->CurrcharObj();
	bool x = false;
	UI08 a1 = mChar->GetSerial( 1 );
	UI08 a2 = mChar->GetSerial( 2 );
	UI08 a3 = mChar->GetSerial( 3 );
	UI08 a4 = mChar->GetSerial( 4 );
	
	HelpRequest pageToAdd;
	pageToAdd.Reason( reason );
	pageToAdd.WhoPaging( mChar->GetSerial() );
	pageToAdd.IsHandled( false );
	pageToAdd.TimeOfPage( time( NULL ) );

	int callNum = CounselorQueue->Add( &pageToAdd );
	if( callNum != -1 )
	{
		mChar->SetPlayerCallNum( callNum );
		mChar->SetPageGM( 2 );
		if( !strcmp( reason, "OTHER" ) )
		{
			mChar->SetSpeechMode( 2 );
			sysmessage( s, 359 );
		}
		else
		{
			mChar->SetPageGM( 0 );
			char temp[1024];
			sprintf( temp, "Counselor Page from %s [%x %x %x %x]: %s", mChar->GetName(), a1, a2, a3, a4, reason );
			Network->PushConn();
			for( cSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
			{
				CChar *iChar = iSock->CurrcharObj();
				if( iChar->IsGMPageable() || iChar->IsCounselor() )
				{
					x = true;
					sysmessage( iSock, temp );
				}
			}
			Network->PopConn();
			if( x )
				sysmessage( s, 360 );
			else 
				sysmessage( s, 361 );
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function	  :  void GMPage( cSocket *s, char *reason )
//|   Date		  :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Used when player calls a GM
//o---------------------------------------------------------------------------o
void cCommands::GMPage( cSocket *s, const char *reason )
{
	bool x = false;
	CChar *mChar = s->CurrcharObj();
	UI08 a1 = mChar->GetSerial( 1 );
	UI08 a2 = mChar->GetSerial( 2 );
	UI08 a3 = mChar->GetSerial( 3 );
	UI08 a4 = mChar->GetSerial( 4 );
	
	HelpRequest pageToAdd;
	pageToAdd.Reason( reason );
	pageToAdd.WhoPaging( mChar->GetSerial() );
	pageToAdd.IsHandled( false );
	pageToAdd.TimeOfPage( time( NULL ) );
	int callNum = GMQueue->Add( &pageToAdd );
	if( callNum != -1 )
	{
		mChar->SetPlayerCallNum( callNum );
		mChar->SetPageGM( 1 );
		if( !strcmp( reason, "OTHER" ) )
		{
			mChar->SetSpeechMode( 1 );
			sysmessage( s, 362 );
		}
		else
		{
			mChar->SetPageGM( 0 );
			char temp[1024];
			sprintf( temp, "Page from %s [%x %x %x %x]: %s", mChar->GetName(), a1, a2, a3, a4, reason );
			Network->PushConn();
			for( cSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
			{
				CChar *iChar = iSock->CurrcharObj();
				if( iChar == NULL )
					continue;
				if( iChar->IsGMPageable() )
				{
					x = true;
					sysmessage( iSock, temp );
				}
			}
			Network->PopConn();
			if( x )
				sysmessage( s, 363 );
			else 
				sysmessage( s, 364 );
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::DyeItem( cSocket *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Change hue of the target
//o---------------------------------------------------------------------------o
void cCommands::DyeTarget( cSocket *s )
{
	CItem *i = calcItemObjFromSer( s->GetDWord( 1 ) );
	if( i != NULL )
	{
		UI16 colour = s->GetWord( 7 );
           
		if( !s->DyeAll() )
		{
			if( colour < 0x0002 || colour > 0x03E9 )
				colour = 0x03E9;
		}
	
		i->SetColour( colour );
		RefreshItem( i );
		
		soundeffect( s, 0x023E, true );
		return;
	}

	CChar *mChar = s->CurrcharObj();
	CChar *c = calcCharObjFromSer( s->GetDWord( 1 ) );
	if( c != NULL )
	{
		if( !mChar->IsGM() ) 
			return;

		SI32 k = s->GetWord( 7 );
		UI16 body = c->GetID();
		SI32 b = k&0x4000; 

		if( ( ( k>>8 ) < 0x80 ) && body >= 0x0190 && body <= 0x0193 ) 
			k += 0x8000;

		if( b == 16384 && (body >= 0x0190 && body <= 0x03e1 ) ) 
			k = 0xf000; // but assigning the only "transparent" value that works, namly semi-trasnparency.

		if( k != 0x8000 )
		{
			c->SetSkin( k );
			c->SetxSkin( k );
			c->Teleport();
		} 
	}
	soundeffect( s, 0x023E, true );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::AddHere( cSocket *s, SI08 z )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Add an item at players location
//o---------------------------------------------------------------------------o
void cCommands::AddHere( cSocket *s, SI08 z )
{
	CTile tile;
	UI16 itemID = ((s->AddID1())<<8) + s->AddID2();
	
	Map->SeekTile( itemID, &tile );
	bool pileable = tile.Stackable();
	CChar *mChar = s->CurrcharObj();
	CItem *c = Items->SpawnItem( s, mChar, 1, "#", pileable, itemID, 0, false, false );
	if( c != NULL )
	{
		c->SetLocation( mChar->GetX(), mChar->GetY(), z );
		c->SetDoorDir( 0 );
		c->SetPriv( 0 );
		RefreshItem( c );
	}
	s->AddID1( 0 );
	s->AddID2( 0 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::MakePlace( cSocket *s, int i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Change teleport location to x,y,z
//o---------------------------------------------------------------------------o
void cCommands::MakePlace( cSocket *s, int i )
{
	SI16 x = 0, y = 0;
	SI08 z = 0;
	char temp[1024];
	sprintf( temp, "LOCATION %i", i );
	ScriptSection *Location = FileLookup->FindEntry( temp, location_def );
	if( Location == NULL )
		return;
	for( const char *tag = Location->First(); !Location->AtEnd(); tag = Location->Next() )
	{
		const char *data = Location->GrabData();
		if( !strcmp( tag, "X" ) )
			x = (SI16)makeNum( data );
		else if( !strcmp( tag, "Y" ) )
			y = (SI16)makeNum( data );
		else if( !strcmp( tag, "Z" ) )
			z = (SI08)makeNum( data );
	}
	s->AddX( x );
	s->AddY( y );
	s->AddZ( z );
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *cCommands::DupeItem( cSocket *s, CItem *i, SI32 amount )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Dupe selected item
//o---------------------------------------------------------------------------o
CItem *cCommands::DupeItem( cSocket *s, CItem *i, SI32 amount )
{
	CChar *mChar = s->CurrcharObj();
	CItem *p = getRootPack( i ), *cp = mChar->GetPackItem();

	if ( ( !p && !cp ) || !mChar || i->isCorpse() )
		return NULL;
	
	ITEM tItem;
	CItem *c = i->Dupe( tItem );
	if( c == NULL )
		return NULL;

	c->IncX(2);
	c->IncY(2);
	c->IncZ(1);

	if ( !p && cp )
		c->SetCont( cp->GetSerial() );

	c->SetAmount( amount );
	if( c->GetSpawn() != 0 )
		nspawnsp.AddSerial( c->GetSpawn(), tItem );

	RefreshItem( c );
	return c;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::Wipe( cSocket *s )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Wipes all items in a world
//o---------------------------------------------------------------------------o
void cCommands::Wipe( cSocket *s )
{
	CChar *mChar = s->CurrcharObj();
	Console << mChar->GetName() << " has initiated an item wipe" << myendl;
	
	for( ITEM k = 0; k <= itemcount; k++ )
	{
		if( items[k].GetCont() == INVALIDSERIAL && !items[k].isWipeable() )
			Items->DeleItem( &items[k] );
	}
	sysbroadcast( Dictionary->GetEntry( 365 ) ); 
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::Possess( cSocket *s ) 
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Allow a GM to take over the body of another Character
//o---------------------------------------------------------------------------o
void cCommands::Possess( cSocket *s ) 
{
	sysmessage( s, 1635 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::Load( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Load the command table
//o---------------------------------------------------------------------------o
void cCommands::Load( void )
{
	SI32 tablePos;
	SI16 commandCount = 0;
	ScriptSection *commands = FileLookup->FindEntry( "COMMAND_OVERRIDE", command_def );
	if( commands == NULL )
	{
		InitClearance();
		return;
	}

	const char *tag = NULL;
	const char *data = NULL;

	std::vector< std::string > badCommands;
	for( tag = commands->First(); !commands->AtEnd(); tag = commands->Next() )
	{
		data = commands->GrabData();
		CmdTableIterator toFind = cmd_table.find( tag );
		if( toFind == cmd_table.end() )
		{
		  // make sure we don't index into array at -1
			badCommands.push_back( tag );
		}
		else
		{
			commandCount++;
			toFind->second.cmdLevelReq = makeNum( data );
//			if( commandCount % 10 == 0 )
//				Console << ".";
		}
		// check for commands here
	}
	if( badCommands.size() > 0 )
	{
		Console << myendl;
		for( tablePos = 0; tablePos < badCommands.size(); tablePos++ )
			Console << "Invalid command '" << badCommands[tablePos].c_str() << "' found in commands.dfn!" << myendl;
	}

	ScriptSection *cmdClearance = FileLookup->FindEntry( "COMMANDLEVELS", command_def );
	if( cmdClearance == NULL )
	{
		InitClearance();
		return;
	}

	int currentWorking;
	for( tag = cmdClearance->First(); !cmdClearance->AtEnd(); tag = cmdClearance->Next() )
	{
		data = cmdClearance->GrabData();
		currentWorking = clearance.size();
		clearance.push_back( new commandLevel_st );
		memset( &clearance[currentWorking], 0, sizeof( commandLevel_st ) );
		strcpy( clearance[currentWorking]->name, tag );
		clearance[currentWorking]->commandLevel = makeNum( data );
	}
	for( UI32 tempCounter = 0; tempCounter < clearance.size(); tempCounter++ )
	{
		cmdClearance = FileLookup->FindEntry( clearance[tempCounter]->name, command_def );
		if( cmdClearance == NULL )
			continue;
		for( tag = cmdClearance->First(); !cmdClearance->AtEnd(); tag = cmdClearance->Next() )
		{
			data = cmdClearance->GrabData();
			if( !strcmp( tag, "NICKCOLOUR" ) )
				clearance[tempCounter]->nickColour = (UI16)makeNum( data );
			else if( !strcmp( tag, "DEFAULTPRIV" ) )
				clearance[tempCounter]->defaultPriv = (UI16)makeNum( data );
			else if( !strcmp( tag, "BODYID" ) )
				clearance[tempCounter]->targBody = (UI16)makeNum( data );
			else if( !strcmp( tag, "ALLSKILL" ) )
				clearance[tempCounter]->allSkillVals = (UI16)makeNum( data );
			else if( !strcmp( tag, "BODYCOLOUR" ) )
				clearance[tempCounter]->bodyColour = (UI16)makeNum( data );
			else if( !strcmp( tag, "STRIPOFF" ) )
				clearance[tempCounter]->stripOff = ( makeNum( data ) != 0 );
			else
				Console << myendl << "Unknown tag in " << clearance[tempCounter]->name << ": " << tag << " with data of " << data << myendl;
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::RemoveShop( cSocket *s )
//|	Programmer	-	Abaddon
//o---------------------------------------------------------------------------o
//|	Purpose		-	Removes shopkeeper layers from a character
//o---------------------------------------------------------------------------o
void cCommands::RemoveShop( cSocket *s )
{
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
	{
		sysmessage( s, 366 );
		return;
	}

	i->SetShop( false );
	CItem *buyLayer   = FindItemOnLayer( i, 0x1A );
	CItem *sellLayer  = FindItemOnLayer( i, 0x1B );
	CItem *otherLayer = FindItemOnLayer( i, 0x1C );
	if( buyLayer != NULL )
		Items->DeleItem( buyLayer );
	if( sellLayer != NULL )
		Items->DeleItem( sellLayer );
	if( otherLayer != NULL )
		Items->DeleItem( otherLayer );
	sysmessage( s, 367 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::Log( char *command, CHARACTER player1, CHARACTER player2, char *extraInfo )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Writes toLog to a file
//o---------------------------------------------------------------------------o
void cCommands::Log( char *command, CChar *player1, CChar *player2, char *extraInfo )
{
	char logName[MAX_PATH];
	sprintf( logName, "%scommand.log", cwmWorldState->ServerData()->GetLogsDirectory() );
	FILE *commandLog = fopen( logName, "a+" );
	if( commandLog == NULL )
	{
		Console.Error( 1, "Unable to open command log file %s!", logName );
		return;
	}
	char dateTime[1024];
	time_t ltime;
	time( &ltime );
	char *t = ctime( &ltime );
	// just to be paranoid and avoid crashing
	if( t == NULL )
		t = "";
	else
	{
		// some ctime()s like to stick \r\n on the end, we don't want that
		for( int end = strlen(t) - 1; end >= 0 && isspace(t[end]); --end )
			t[end] = '\0';
	}
	safeCopy( dateTime, t, 1024);

	if( player2 != NULL )
		fprintf( commandLog, "[%s] %s (serial: %i ) used command <%s> on player %s (serial: %i ) Extra Info: %s\n", dateTime, player1->GetName(), player1->GetSerial(), command, player2->GetName(), player2->GetSerial(), extraInfo );
	else
		fprintf( commandLog, "[%s] %s (serial: %i ) used command <%s> Extra Info: %s\n", dateTime, player1->GetName(), player1->GetSerial(), command, extraInfo );
	fclose( commandLog );
}

//o---------------------------------------------------------------------------o
//|	Function	-	commandLevel_st *cCommands::GetClearance( const char *clearName )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the command level of a character
//o---------------------------------------------------------------------------o
commandLevel_st *cCommands::GetClearance( const char *clearName )
{
	char *nameBackup;
	nameBackup = new char[strlen( clearName ) + 1];
	strcpy( nameBackup, clearName );
	nameBackup = strupr( nameBackup );
	for( UI32 counter = 0; counter < clearance.size(); counter++ )
	{
		if( !strcmp( nameBackup, clearance[counter]->name ) )
		{
			delete [] nameBackup;
			return clearance[counter];
		}
	}
	delete [] nameBackup;
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI16 cCommands::GetColourByLevel( UI08 commandLevel )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get a players nick color based on his command level
//o---------------------------------------------------------------------------o
UI16 cCommands::GetColourByLevel( UI08 commandLevel )
{
	int clearanceSize = clearance.size();
	if( clearanceSize <= 0 )
		return 0x005A;
	if( commandLevel > clearance[0]->commandLevel )
		return clearance[0]->nickColour;

	for( int counter = 0; counter < ( clearanceSize - 1 ); counter++ )
	{
		if( commandLevel <= clearance[counter]->commandLevel && commandLevel > clearance[counter+1]->commandLevel )
			return clearance[counter]->nickColour;
	}
	return clearance[clearanceSize - 1]->nickColour;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::InitClearance( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Initialize command levels
//o---------------------------------------------------------------------------o
void cCommands::InitClearance( void )
{
	clearance.push_back( new commandLevel_st );	// 0 -> 2
	clearance.push_back( new commandLevel_st );
	clearance.push_back( new commandLevel_st );

	strcpy( clearance[0]->name, "GM" );
	strcpy( clearance[1]->name, "COUNSELOR" );
	strcpy( clearance[2]->name, "PLAYER" );

	clearance[0]->commandLevel = 2;
	clearance[1]->commandLevel = 1;
	clearance[2]->commandLevel = 0;

	clearance[0]->targBody = 0x03DB;
	clearance[1]->targBody = 0x03DB;
	clearance[2]->targBody = 0;

	clearance[0]->bodyColour = 0x8021;
	clearance[1]->bodyColour = 0x8002;

	clearance[0]->defaultPriv = 0xFD;
	clearance[1]->defaultPriv = 0x8DB6;
	clearance[2]->defaultPriv = 0;

	clearance[0]->nickColour = 0x03;
	clearance[1]->nickColour = 0x03;
	clearance[2]->nickColour = 0x005A;

	clearance[0]->allSkillVals = 1000;
	clearance[1]->allSkillVals = 0;
	clearance[2]->allSkillVals = 0;
}

//o---------------------------------------------------------------------------o
//|	Function	-	commandLevel_st *cCommands::GetClearance( UI08 commandLevel )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Geta characters command level
//o---------------------------------------------------------------------------o
commandLevel_st *cCommands::GetClearance( UI08 commandLevel )
{
	int clearanceSize = clearance.size();
	if( clearanceSize <= 0 )
		return NULL;
	if( commandLevel > clearance[0]->commandLevel )
		return clearance[0];

	for( int counter = 0; counter < ( clearanceSize - 1 ); counter++ )
	{
		if( commandLevel <= clearance[counter]->commandLevel && commandLevel > clearance[counter+1]->commandLevel )
			return clearance[counter];
	}
	return clearance[clearanceSize - 1];
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool cCommands::CommandExists( char *cmdName )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if a command is valid
//o---------------------------------------------------------------------------o
bool cCommands::CommandExists( char *cmdName )
{
	CmdTableIterator toFind = cmd_table.find( cmdName );
	return ( toFind != cmd_table.end() );
}

//o---------------------------------------------------------------------------o
//|	Function	-	const char * cCommands::FirstCommand( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get first command in cmd_table
//o---------------------------------------------------------------------------o
const char * cCommands::FirstCommand( void )
{
	cmdPointer = cmd_table.begin();
	if( FinishedCommandList() )
		return NULL;
	return cmdPointer->first.c_str();
}

//o---------------------------------------------------------------------------o
//|	Function	-	const char * cCommands::NextCommand( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get next command in cmd_table
//o---------------------------------------------------------------------------o
const char * cCommands::NextCommand( void )
{
	if( FinishedCommandList() )
		return NULL;
	cmdPointer++;
	if( FinishedCommandList() )
		return NULL;
	return cmdPointer->first.c_str();
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool cCommands::FinishedCommandList( void )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get end of cmd_table
//o---------------------------------------------------------------------------o
bool cCommands::FinishedCommandList( void )
{
	return ( cmdPointer == cmd_table.end() );
}

//o---------------------------------------------------------------------------o
//|	Function	-	cmdtable_mapentry *cCommands::CommandDetails( char *cmdName )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get command info
//o---------------------------------------------------------------------------o
cmdtable_mapentry *cCommands::CommandDetails( char *cmdName )
{
	if( !CommandExists( cmdName ) )
		return NULL;
	CmdTableIterator toFind = cmd_table.find( cmdName );
	return &(toFind->second);
}

