#include "uox3.h"
#include "boats.h"

#define XP 0
#define YP 1

//============================================================================================
//UooS Item translations - You guys are the men! :o)

//[4]=direction of ship
//[4]=Which Item (PT Plank, SB Plank, Hatch, TMan)
//[2]=Coord (x,y) offsets
SI16 iSmallShipOffsets[4][4][2] =
//	 X	 Y	 X	 Y	 X	 Y	 X	 Y
{ 
	{{-2, 0},{ 2, 0},{ 0,-4},{1,4}},//Dir
	{{0,-2},{0,2},{4,0},{-4,0}},
	{{2,0},{-2,0},{ 0,4},{0,-4}},
	{{0,2},{0,-2},{-4,0},{4,0}} 
};
//  P1    P2   Hold  Tiller
SI16 iMediumShipOffsets[4][4][2] =
//	 X	 Y	 X	 Y	 X	 Y	 X	 Y
{ 
    {{-2,0},{2,0},{0,-4},{1,5}},
    {{0,-2},{0,2},{4,0},{-5,0}},
    {{2,0},{-2,0},{0,4},{0,-5}},
    {{0,2},{0,-2},{-4,0},{5,0}} 
};
SI16 iLargeShipOffsets[4][4][2] =
//	 X	 Y	 X	 Y	 X	 Y	 X	 Y
{ 
    {{-2,-1},{2,-1},{0,-5},{1,5}},
    {{1,-2},{1,2},{5,0},{-5,0}},
    {{2,1},{-2,1},{0,5},{0,5}},
    {{-1,2},{-1,-2},{-5, 0},{5,0}} 
};
//Ship Items
//[4] = direction
//[6] = Which Item (PT Plank Up,PT Plank Down, SB Plank Up, SB Plank Down, Hatch, TMan)
UI08 cShipItems[4][6]=
{
  {0xB1,0xD5,0xB2,0xD4,0xAE,0x4E},
  {0x8A, 0x89, 0x85, 0x84, 0x65, 0x53},
	{0xB2, 0xD4, 0xB1, 0xD5, 0xB9, 0x4B},
	{0x85, 0x84, 0x8A, 0x89, 0x93, 0x50} 
};
//============================================================================================

cBoat::cBoat()//Consturctor
{
}

cBoat::~cBoat()//Destructor
{
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cBoat::PlankStuff( cSocket *s, CItem *p )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	If not on a boat, will send character to the plank, other
//|					wise will call OpenPlank to open/close the plank
//o---------------------------------------------------------------------------o
void cBoat::PlankStuff( cSocket *s, CItem *p )
{
	CChar *mChar = s->CurrcharObj();

	CItem *boat = GetBoat( s );
	if( boat == NULL )
	{
		mChar->SetLocation( p->GetX(), p->GetY(), p->GetZ() + 5 );
		SERIAL mser = p->GetMore();
		CItem *boat2 = calcItemObjFromSer( mser );
		if( boat2 != NULL )
		{
			CHARLIST *myPets = mChar->GetPetList();
			if( myPets != NULL )
			{
				for( UI32 a = 0; a < myPets->size(); a++ )	// All pet/hirelings
				{
					CChar *pet = (*myPets)[a];
					if( pet != NULL )
					{
						if( !pet->IsMounted() && pet->IsNpc() && objInRange( mChar, pet, 15 ) )
						{
							pet->SetLocation( boat2->GetX() + 1, boat2->GetY() + 1, boat2->GetZ() + 4 );
							pet->SetMulti( boat2 );
							pet->Teleport();
						}
					}
				}
			}
		}
		
		if( boat2 != NULL )
		{
			sysmessage( s, 1 );
			mChar->SetMulti( boat2 );
		} 
		else 
			sysmessage( s, 2 );
	} 
	else 
		LeaveBoat( s, p );
	mChar->Teleport();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cBoat::LeaveBoat( cSocket *s, CItem *p )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Leave a boat
//o---------------------------------------------------------------------------o
void cBoat::LeaveBoat( cSocket *s, CItem *p )
{
	CItem *boat = GetBoat( s );
	if( boat == NULL ) 
		return;

	SI16 x2 = p->GetX();
	SI16 y2 = p->GetY();
	SI08 z = p->GetZ(), typ;
	CChar *mChar = s->CurrcharObj();
	UI08 worldNumber = mChar->WorldNumber();
	for( SI16 x = x2 - 2; x < x2 + 3; x++ )
	{
		for( SI16 y = y2 - 2; y < y2 + 3; y++ )
		{
			SI08 sz = (SI08)Map->StaticTop( x, y, z, worldNumber );
			SI08 mz = (SI08) Map->MapElevation( x, y, worldNumber );
			if( sz == illegal_z) 
				typ = 0;
			else
				typ = 1;
			
			if( ( typ == 0 && mz != 5) || ( typ == 1 && sz != -5 ) )// everthing the blocks a boat is ok to leave the boat ... LB
			{
				CHARLIST *myPets = mChar->GetPetList();
				if( myPets != NULL )
				{
					for( UI32 a = 0; a < myPets->size(); a++ )
					{
						CChar *pet = (*myPets)[a];
						if( pet != NULL )
						{
							if( !pet->IsMounted() && pet->IsNpc() && objInRange( mChar, pet, 15 ) )
							{
								if( typ )
									pet->SetLocation( x, y, sz );
								else 
									pet->SetLocation( x, y, mz );
								
								if( pet->GetMultiObj() != NULL )
									pet->SetMulti( INVALIDSERIAL );
								pet->Teleport();
							}
						}
					}
				}
				mChar->SetMulti( INVALIDSERIAL );
				
				if( typ ) 
					mChar->SetLocation( x, y, sz );
				else  
					mChar->SetLocation( x, y, mz );
				
				sysmessage( s, 3 );
				return;
			}
		}
	}
	sysmessage( s, 4 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cBoat::OpenPlank( CItem *p )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Open / Close a plank
//o---------------------------------------------------------------------------o
void cBoat::OpenPlank( CItem *p )
{
	switch( p->GetID( 2 ) )
	{
		//Open plank->
	case 0xE9: p->SetID( 0x84, 2 ); break;
	case 0xB1: p->SetID( 0xD5, 2 ); break;
	case 0xB2: p->SetID( 0xD4, 2 ); break;
	case 0x8A: p->SetID( 0x89, 2 ); break;
	case 0x85: p->SetID( 0x84, 2 ); break;
		//Close Plank->
	case 0x84: p->SetID( 0xE9, 2 ); break;
	case 0xD5: p->SetID( 0xB1, 2 ); break;
	case 0xD4: p->SetID( 0xB2, 2 ); break;
	case 0x89: p->SetID( 0x8A, 2 ); break;
	default: 	Console.Warning( 2, "Invalid plank ID called! Plank %i '%s' [%i]", p->GetSerial(), p->GetName(), p->GetID() );
			break;
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool cBoat::CreateBoat( cSocket *s, CItem *b, char id2, int boattype )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Create a boat
//o---------------------------------------------------------------------------o
bool cBoat::CreateBoat( cSocket *s, CItem *b, char id2, int boattype )
{
	if( b == NULL ) 
	{
		sysmessage( s, 5 );
		return false;
	}
	switch( id2 )
	{
	case 0x00:
	case 0x04:
	case 0x08:
	case 0x0C:
	case 0x10:
	case 0x14:
		break;
	default:
		sysmessage( s, 6 );
		return false;
	}

	if( BlockBoat( b, 0, 0, 0 ) )
	{
		sysmessage( s, 7 );
		return false;
	}

	CChar *mChar = s->CurrcharObj();
	b->SetMore( id2, id2+3, b->GetMore( 3 ), b->GetMore( 4 ) );
	b->SetType( 117 );// Boat type
	b->SetZ( -5 );// Z in water
	b->SetName( Dictionary->GetEntry( 1408 ) );//Name is something other than "%s's house"
	SERIAL serial = b->GetSerial();
	CItem *tiller = Items->SpawnItem( NULL, mChar, 1, Dictionary->GetEntry( 1409 ), false, 0x3E4E, 0, false, false );
	if( tiller == NULL ) 
		return false;
	tiller->SetPriv( 0 );
	tiller->SetType( 200 );
	tiller->SetMore( serial );
	tiller->SetMoreX( boattype );

	CItem *p2 = Items->SpawnItem( NULL, mChar, 1, "#", false, 0x3EB2, 0, false, false );//Plank2 is on the RIGHT side of the boat
	if( p2 == NULL ) 
		return false;
	p2->SetType( 117 );
	p2->SetType2( 3 );
	p2->SetMore( serial );
	p2->SetPriv( 0 );//Nodecay

	CItem *p1 = Items->SpawnItem( NULL, mChar, 1, "#", false, 0x3EB1, 0, false, false );//Plank1 is on the LEFT side of the boat
	if( p1 == NULL ) 
		return false;
	p1->SetType( 117 );//Boat type
	p1->SetType2( 3 );//Plank sub type
	p1->SetMore( serial );
	p1->SetPriv( 0 );

	CItem *hold = Items->SpawnItem( NULL, mChar, 1, "#", false, 0x3EAE, 0, false, false );
	if( hold == NULL ) 
		return false;
	hold->SetMore( serial );
	hold->SetType( 1 );//Conatiner
	hold->SetPriv( 0 );
	
	b->SetMoreB( tiller->GetSerial() );
	b->SetMoreX( p1->GetSerial() );// Store the other stuff anywhere it will fit :-)
	b->SetMoreY( p2->GetSerial() );
	b->SetMoreZ( hold->GetSerial() );

	SI16 x = b->GetX(), y = b->GetY();
	SI08 z = b->GetZ();
	switch( id2 ) //Give everything the right Z for it size boat
	{
	case 0x00:
	case 0x04:
		tiller->SetLocation( x + 1, y + 4, -5 );
		p1->SetLocation( x - 2, y, -5 );
		p2->SetLocation( x + 2, y, -5 );
		hold->SetLocation( x, y - 4, -5 );
		break;
	case 0x08:
	case 0x0C:
		tiller->SetLocation( x + 1, y + 5, -5 );
		p1->SetLocation( x - 2, y, -5 );
		p2->SetLocation( x + 2, y, -5 );
		hold->SetLocation( x, y - 4, -5 );
		break;
	case 0x10:
	case 0x14:
		tiller->SetLocation( x + 1, y + 5, -5 );
		p1->SetLocation( x - 2, y - 1, -5 );
		p2->SetLocation( x + 2, y - 1, -5 );
		hold->SetLocation( x, y - 5, -5 );
		break;
	}
	//their x pos is set by BuildHouse(), so just fix their Z...
	mChar->SetZ( z + 3 );
	mChar->SetDispZ( z + 3 );
    mChar->SetMulti( b );
	return true;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem * cBoat::GetBoat( cSocket *s )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the boat a character is on
//o---------------------------------------------------------------------------o
CItem * cBoat::GetBoat( cSocket *s )
{
	CItem *boat = NULL;
	CChar *mChar = s->CurrcharObj();

	UI08 worldNumber = mChar->WorldNumber();
  if( mChar->GetMultiObj() != NULL )
		return (CItem*)mChar->GetMultiObj();
	else 
	{
		int xOffset = MapRegion->GetGridX( mChar->GetX() );
		int yOffset = MapRegion->GetGridY( mChar->GetY() );
		for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
		{
			for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
			{
				SubRegion *toCheck = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
				if( toCheck == NULL )
					continue;
				toCheck->PushItem();
				for( CItem *itemCheck = toCheck->FirstItem(); !toCheck->FinishedItems(); itemCheck = toCheck->GetNextItem() )
				{
					if( itemCheck == NULL )
						continue;
					if( itemCheck->GetID( 1 ) >= 0x40 )
					{
						if( objInRange( mChar, itemCheck, 30 ) )
						{
							if( itemCheck->GetType() == 117 )
								boat = itemCheck;
						}
					}
				}
				toCheck->PopItem();
			}
		}
		if( boat != NULL )
		{
			if( !inMulti( mChar->GetX(), mChar->GetY(), mChar->GetZ(), boat, worldNumber ) )
				boat = NULL;
		}
		return boat;
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool cBoat::BlockBoat( CItem *b, SI16 xmove, SI16 ymove, UI08 dir )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check if a boat can move to a certain loc
//o---------------------------------------------------------------------------o
#pragma note( "Param Warning: in cBoat::BlockBoat(). Parameter dir is not referanced(used)." )
bool cBoat::BlockBoat( CItem *b, SI16 xmove, SI16 ymove, UI08 dir )
{
	MapStaticIterator *msi;
	staticrecord *stat = NULL;
	map_st map;
	CLand land;
	CTile tile;
	SI16 cx = b->GetX(), cy = b->GetY();
	SI16 x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	UI08 type = 0;

	cx += xmove;
	cy += ymove;

	switch( b->GetID( 2 ) )
	{
	case 0:
	case 1:
	case 2:
	case 3://small
		type = 1;
		break;
	case 4:
	case 5:
	case 6:
	case 7://small dragon
		type = 1;
		break;

	case 8:
	case 9:
	case 10:
	case 11://medium
		type = 2;
		break;
	case 12:
	case 13:
	case 14:
	case 15://medium dragon
		type = 2;
		break;
	case 16:
	case 17:
	case 18:
	case 19://large
		type = 3;
		break;
	case 20:
	case 21:
	case 22:
	case 23://large dragon
		type = 3;
		break;
	default:
		return true;
	}

	//small = 5,11 
	//medium = 5, 13
	//large = 5, 15
	switch( b->GetDir() )	
	{
		case 1: // U
		case 5: // D
		case 0: // N
		case 4: // S
			x1 = cx - 2;
			x2 = cx + 2;
			switch( type )
			{
				case 1:	y1 = cy - 6; y2 = cy + 6; break;
				case 2:	y1 = cy - 6; y2 = cy + 7; break;
				case 3: y1 = cy - 8; y2 = cy + 8; break;
				default:	Console.Error( 2, " Fallout of North/South switch() statement in cBoats::BlockBoat()" );	break;
			}
			break;
		case 3: // R
		case 7: // L
		case 2: // E
		case 6: // W
			y1 = cy - 2;
			y2 = cy + 2;
			switch( type )
			{
				case 1:	x1 = cx - 6; x2 = cx + 6; break;
				case 2:	x1 = cx - 7; x2 = cx + 7; break;
				case 3: x1 = cx - 8; x2 = cx + 8; break;
				default:	Console.Error( 2, " Fallout of East/West switch() statement in cBoats::BlockBoat()" );	break;
			}
			break;
		default: return true;
	}

	UI08 worldNumber = b->WorldNumber();
	for( SI16 x = x1; x < x2; x++ )
	{
		for( SI16 y = y1; y < y2; y++ )
		{
			SI08 sz = Map->StaticTop( x, y, b->GetZ(), worldNumber );

			if( sz == illegal_z ) //map tile
			{
				map = Map->SeekMap0( x, y, worldNumber );
				Map->SeekLand( map.id, &land );
				if( map.z >= -5 && !land.LiquidWet() && strcmp( land.Name(), "water" ) )//only tiles on/above the water
					return true;
			} 
			else 
			{ //static tile
				msi = new MapStaticIterator( x, y, worldNumber );
				while( stat = msi->Next() )
				{
					msi->GetTile( &tile );
					SI08 zt = stat->zoff + tile.Height();
					if( !tile.LiquidWet() && zt > -5 && zt <= 15 && strcmp( (char*)tile.Name(), "water" ) ) 
					{
						delete msi;
						return true;
					}
				}
				delete msi;
			}
		}
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cBoat::MoveBoat( cSocket *s, UI08 dir, CItem *boat )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Move the boat and everything on it 1 tile
//o---------------------------------------------------------------------------o
void cBoat::MoveBoat( cSocket *s, UI08 dir, CItem *boat )
{
	SI16 tx = 0, ty = 0;

	CItem *tiller = calcItemObjFromSer( boat->GetMoreB() );
	CItem *p1 = calcItemObjFromSer( boat->GetMoreX() );
	CItem *p2 = calcItemObjFromSer( boat->GetMoreY() );
	CItem *hold = calcItemObjFromSer( boat->GetMoreZ() );
	
	if( boat == NULL )
		return;
	
	if( s == NULL || tiller == NULL || p1 == NULL || p2 == NULL || hold == NULL )
		return;

	CPPauseResume prSend( 1 );
	s->Send( &prSend );
	
	switch( dir&0x0F )
	{
		case 0:	ty--;			break;
		case 1:	tx++;	ty--;	break;
		case 2:	tx++;			break;
		case 3:	tx++;	ty++;	break;
		case 4:	ty++;			break;
		case 5:	tx--;	ty++;	break;
		case 6:	tx--;			break;
		case 7:	tx--;	ty--;	break;
		default: Console.Error( 2, "Boat direction error: %i in boat %i", dir&0x0F, boat->GetSerial() ); break;	
	}

	SI16 x = boat->GetX(), y = boat->GetY();
	prSend.Mode( 0 );
	if( ( (x + tx) <= 200 || (x + tx) >= 6000 ) && ( (y + ty) <= 200 || (y + ty) >= 4900 ) )
	{
		boat->SetType2( 0 );
		itemTalk( s, tiller, 8 );
		s->Send( &prSend );
		return;
	}
	//if(!validNPCMove(items[boat].x+tx,items[boat].y+ty,items[boat].z,currchar[s]))
	if( BlockBoat( boat, tx, ty, dir ) )
	{
		boat->SetType2( 0 );
		itemTalk( s, tiller, 9 );
		s->Send( &prSend );
		return;
	}
	
	//Move all the special items
	MapRegion->RemoveItem( boat );
	boat->IncX( tx );
	boat->IncY( ty );
	MapRegion->AddItem( boat );
	RefreshItem( boat );
	
	MapRegion->RemoveItem( tiller );
	tiller->IncX( tx );
	tiller->IncY( ty );
	MapRegion->AddItem( tiller );
	RefreshItem( tiller );
	
	MapRegion->RemoveItem( p1 );
	p1->IncX( tx );	// Jediman Boat fix
	p1->IncY( ty );
	MapRegion->AddItem( p1 );
	RefreshItem( p1 );
	
	MapRegion->RemoveItem( p2 );
	//p2->IncX( tx );
	//p2->IncY( ty );
	MapRegion->AddItem( p2 );
	RefreshItem( p2 );
	
	MapRegion->RemoveItem( hold );
	hold->IncX( tx );
	hold->IncY( ty );
	MapRegion->AddItem( hold );
	RefreshItem( hold );
	
	CMultiObj *realBoat = static_cast< CMultiObj *>(boat);
	for( CItem *bItem = realBoat->FirstItemMulti(); !realBoat->FinishedItemMulti(); bItem = realBoat->NextItemMulti() )
	{
		if( bItem == NULL )
			continue;
		MapRegion->RemoveItem( bItem );
		bItem->IncX( tx );
		bItem->IncY( ty );
		MapRegion->AddItem( bItem );
		RefreshItem( bItem );
	}
	
	for( CChar *bChar = realBoat->FirstCharMulti(); !realBoat->FinishedCharMulti(); bChar = realBoat->NextCharMulti() )
	{
		if( bChar == NULL )
			continue;
		bChar->SetLocation( bChar->GetX() + tx, bChar->GetY() + ty, bChar->GetZ() );
		bChar->Teleport();
	}
	s->Send( &prSend );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cBoat::TurnStuff( CItem *b, CItem *i, bool rightTurn )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Turn an item on a boat
//o---------------------------------------------------------------------------o
void cBoat::TurnStuff( CItem *b, CItem *i, bool rightTurn )
{
    if( b == NULL ) 
		return;
	
	SI16 dx = i->GetX() - b->GetX();//get their distance x
	SI16 dy = i->GetY() - b->GetY();//and distance Y
	
	MapRegion->RemoveItem( i );
	if( rightTurn )
	{
		i->SetX( b->GetX() - dy );
		i->SetY( b->GetY() + dx );
	} 
	else 
	{
		i->SetX( b->GetX() + dy );
		i->SetY( b->GetY() - dx );
	}
	MapRegion->AddItem( i );
	RefreshItem( i );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cBoat::TurnStuff( CItem *b, CChar *i, bool rightTurn )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Turn a character on a boat
//o---------------------------------------------------------------------------o
void cBoat::TurnStuff( CItem *b, CChar *i, bool rightTurn )
{
    if( b == NULL ) 
		return;
	
	SI16 dx = i->GetX() - b->GetX();
	SI16 dy = i->GetY() - b->GetY();

	MapRegion->RemoveChar( i );
	if( rightTurn )
		i->SetLocation( b->GetX() - dy, b->GetY() + dx, i->GetZ() );
	else 
		i->SetLocation( b->GetX() + dy, b->GetY() - dx, i->GetZ() );
	i->Teleport();
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cBoat::TurnBoat( CItem *b, bool rightTurn )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Turn the boat and use TurnStuff() to turn all items/chars on it
//o---------------------------------------------------------------------------o
void cBoat::TurnBoat( CItem *b, bool rightTurn )
{
	if( b == NULL ) 
		return; 

	std::vector< cSocket * > Send;
	SI16 id2 = b->GetID( 2 );
	UI08 olddir = b->GetDir();
	UI32 a = 0;
	
	CPPauseResume prSend( 1 );
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( objInRange( tSock, b, BUILDRANGE ) )
		{
			Send.push_back( tSock );
			tSock->Send( &prSend );
		}
	}
	Network->PopConn();

	CItem *tiller = calcItemObjFromSer( b->GetMoreB() );
	CItem *p1 = calcItemObjFromSer( b->GetMoreX() );
	CItem *p2 = calcItemObjFromSer( b->GetMoreY() );
	CItem *hold = calcItemObjFromSer( b->GetMoreZ() );

    if( tiller == NULL || p1 == NULL || p2 == NULL || hold == NULL ) 
		return;
	
	if( rightTurn )
	{
		b->SetDir( b->GetDir() + 2 );
		id2++;
	} 
	else 
	{
		b->SetDir( b->GetDir() - 2 );
		id2--;
	}
	if( b->GetDir() > 250 ) 
		b->SetDir( b->GetDir() + 4 );
	if( b->GetDir() > 7 ) 
		b->SetDir( b->GetDir() - 4 );//Make sure we dont have any DIR errors
	
	if( id2 < b->GetMore( 1 ) ) 
		id2 += 4;//make sure we don't have any id errors either
	if( id2 > b->GetMore( 2 ) ) 
		id2 -= 4;//Now you know what the min/max id is for :-)
	
	prSend.Mode( 0 );
	if( BlockBoat( b, 0, 0, b->GetDir() ) )
	{
		b->SetDir( olddir );
		for( a = 0; a < Send.size(); a++ )
		{
			Send[a]->Send( &prSend );
			itemTalk( Send[a], tiller, 1410, 0, 0x0481 );
		}
		return;
	}

	b->SetID( static_cast<UI08>(id2), 2 );//set the id
	
	if( b->GetID( 2 ) == b->GetMore( 1 ) ) 
		b->SetDir( 0 );//extra DIR error checking
	if( b->GetID( 2 ) == b->GetMore( 2 ) ) 
		b->SetDir( 6 );
	
	
	CMultiObj *realBoat = static_cast< CMultiObj * >(b);
	
    for( CItem *bItem = realBoat->FirstItemMulti(); !realBoat->FinishedItemMulti(); bItem = realBoat->NextItemMulti() )
	{
		if( bItem == NULL )
			continue;
		TurnStuff( b, bItem, rightTurn );
	}
	for( CChar *bChar = realBoat->FirstCharMulti(); !realBoat->FinishedCharMulti(); bChar = realBoat->NextCharMulti() )
	{
		if( bChar == NULL )
			continue;
		TurnStuff( b, bChar, rightTurn );
	}

	UI08 dir = (b->GetDir()&0x0F)/2;
	SI16 x = b->GetX(), y = b->GetY();

	MapRegion->RemoveItem( p1 );
	p1->SetX( x );
	p1->SetY( y );
	p1->SetID( cShipItems[dir][PORT_P_C], 2 );//change the ID
	
	MapRegion->RemoveItem( p2 );
	p2->SetX( x );
	p2->SetY( y );
	p2->SetID( cShipItems[dir][STAR_P_C], 2 );
	
	MapRegion->RemoveItem( tiller );
	tiller->SetX( x );
	tiller->SetY( y );
	tiller->SetID( cShipItems[dir][TILLERID], 2 );
	
	MapRegion->RemoveItem( hold );
	hold->SetX( x );
	hold->SetY( y );
	hold->SetID( cShipItems[dir][HOLDID], 2 );
	
	switch( b->GetMore( 1 ) )//Now set what size boat it is and move the specail items
	{
	case 0x00:
	case 0x04:
		p1->IncX( iSmallShipOffsets[dir][PORT_PLANK][XP] );
		p1->IncY( iSmallShipOffsets[dir][PORT_PLANK][YP] );
		p2->IncX( iSmallShipOffsets[dir][STARB_PLANK][XP] );
		p2->IncY( iSmallShipOffsets[dir][STARB_PLANK][YP] );
		tiller->IncX( iSmallShipOffsets[dir][TILLER][XP] );
		tiller->IncY( iSmallShipOffsets[dir][TILLER][YP] );
		hold->IncX( iSmallShipOffsets[dir][HOLD][XP] );
		hold->IncY( iSmallShipOffsets[dir][HOLD][YP] );
		RefreshItem(p1);
		RefreshItem(p2);
		RefreshItem(hold);
		RefreshItem(tiller);
		break;
	case 0x08:
	case 0x0C:
		p1->IncX( iMediumShipOffsets[dir][PORT_PLANK][XP] );
		p1->IncY( iMediumShipOffsets[dir][PORT_PLANK][YP] );
		p2->IncX( iMediumShipOffsets[dir][STARB_PLANK][XP] );
		p2->IncY( iMediumShipOffsets[dir][STARB_PLANK][YP] );
		tiller->IncX( iMediumShipOffsets[dir][TILLER][XP] );
		tiller->IncY( iMediumShipOffsets[dir][TILLER][YP] );
		hold->IncX( iMediumShipOffsets[dir][HOLD][XP] );
		hold->IncY( iMediumShipOffsets[dir][HOLD][YP] );
		break;
	case 0x10:
	case 0x14:
		p1->IncX( iLargeShipOffsets[dir][PORT_PLANK][XP] );
		p1->IncY( iLargeShipOffsets[dir][PORT_PLANK][YP] );
		p2->IncX( iLargeShipOffsets[dir][STARB_PLANK][XP] );
		p2->IncY( iLargeShipOffsets[dir][STARB_PLANK][YP] );
		tiller->IncX( iLargeShipOffsets[dir][TILLER][XP] );
		tiller->IncY( iLargeShipOffsets[dir][TILLER][YP] );
		hold->IncX( iLargeShipOffsets[dir][HOLD][XP] );
		hold->IncY( iLargeShipOffsets[dir][HOLD][YP] );
		break;
	default: Console.Error( 2, "TurnBoat() more1 error! more1 = %c not found!", b->GetMore( 1 ) );
	}

	MapRegion->AddItem( p1 );
	MapRegion->AddItem( p2 );
	MapRegion->AddItem( tiller );
	MapRegion->AddItem( hold );

	RefreshItem( p1 );
	RefreshItem( p2 );
	RefreshItem( hold );
	RefreshItem( tiller );
	
	for( a = 0; a < Send.size(); a++ )
		Send[a]->Send( &prSend );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cBoat::Speech( cSocket *mSock, char *talk )
//|	Programmer	-	Zippy
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check for boat commands
//o---------------------------------------------------------------------------o
void cBoat::Speech( cSocket *mSock, char *talk )
{
	if( mSock == NULL ) 
		return;

	CItem *boat = GetBoat( mSock );
	if( boat == NULL )
		return;

	UI08 dir = boat->GetDir()&0x0F;
	char msg2[512], msg[512];
	SI16 tx = 0, ty = 0;
	
	strcpy( msg2, talk );

	CItem *tiller = calcItemObjFromSer( boat->GetMoreB() );
	
	strcpy( msg, strupr( msg2 ) );
	UnicodeTypes mLang = mSock->Language();
	const char *txtEntries[16];
	for( UI08 txtEntryCtr = 0; txtEntryCtr < 16; txtEntryCtr++ )
		txtEntries[txtEntryCtr] = Dictionary->GetEntry( 1411 + txtEntryCtr, mLang );
	//forward, backward, right, left, anchor down, raise anchor, one left, one right, one 
	if( strstr( msg, txtEntries[0] ) || strstr( msg, txtEntries[1] ) )
	{
		boat->SetType2( 1 ); // Moving
		itemTalk( mSock, tiller, 10 );
	} 
	else if( strstr( msg, txtEntries[2] ) || strstr( msg, txtEntries[3] ) )
	{
		boat->SetType2( 2 ); // Moving backward
		if( dir >= 4 )
			dir -= 4; 
		else
			dir += 4;
		itemTalk( mSock, tiller, 10 );
	} 
	else if( strstr( msg, txtEntries[4] ) || strstr( msg, txtEntries[5] ) ) 
	{ 
		boat->SetType2( 0 );
		itemTalk( mSock, tiller, 10 ); 
	}// Moving is type2 1 and 2, so stop is 0 :-)
	else if( ( strstr( msg, txtEntries[6] ) && ( strstr( msg, txtEntries[7] ) || strstr( msg, txtEntries[8] ) || strstr( msg, txtEntries[9] ) ) )
		|| strstr( msg, txtEntries[10] ) || strstr( msg, txtEntries[11] ) || strstr( msg, txtEntries[12] ) )
	{
		if( strstr( msg, txtEntries[9] ) || strstr( msg, txtEntries[11] ) ) 
		{
			if( dir >= 2 )
				dir -= 2;
			else
				dir += 6;
			
			switch( dir&0x0F ) // little reminder for myself: move this swtich to a function to have less code ... LB
			{
	           case 0x00:	ty--;			break;
			   case 0x01:	tx++;	ty--;	break;
			   case 0x02:	tx++;			break;
			   case 0x03:	tx++;	ty++;	break;
			   case 0x04:	ty++;			break;
			   case 0x05:	tx--;	ty++;	break;
			   case 0x06:	tx--;			break;
			   case 0x07:	tx--;	ty--;	break;
			}
			
			if( !BlockBoat( boat, tx, ty, dir ) )
			{
				itemTalk( mSock, tiller, 10 );
				TurnBoat( boat, true );
			} 
			else 
			{ 
				boat->SetType2( 0 );
				itemTalk( mSock,tiller, 9 ); 
			}   
		}
		else if( strstr( msg, txtEntries[8] ) || strstr( msg, txtEntries[10] ) ) 
		{
			dir += 2;
			if( dir > 7 )
				dir -= 8;
			switch( dir&0x0F )
			{
	           case 0x00:	ty--;			break;
			   case 0x01:	tx++;	ty--;	break;
			   case 0x02:	tx++;			break;
			   case 0x03:	tx++;	ty++;	break;
			   case 0x04:	ty++;			break;
			   case 0x05:	tx--;	ty++;	break;
			   case 0x06:	tx--;			break;
			   case 0x07:	tx--;	ty--;	break;
			}

			if( !BlockBoat( boat, tx, ty, dir ) )
			{
				itemTalk( mSock, tiller, 10 );
				TurnBoat( boat, false );			
			}
			else 
			{ 
				itemTalk( mSock,tiller, 9 ); 
				boat->SetType2( 0 );
			}
		}
		else if( strstr( msg, txtEntries[12] ) || strstr( msg, txtEntries[7] ) )
		{
			itemTalk( mSock, tiller, 10 );
			TurnBoat( boat, true );
			TurnBoat( boat, true );
		}
	}
	else if( strstr( msg, txtEntries[8] ) )
	{
		if( dir >= 2 )
			dir -= 2;
		else
			dir += 6;
		itemTalk( mSock, tiller, 10 );
		MoveBoat( mSock, dir, boat );
	} 
	else if( strstr( msg, txtEntries[9] ) )
	{
		dir += 2;
		if( dir >= 8 )
			dir -= 8;
		itemTalk( mSock, tiller, 10 );
		MoveBoat( mSock, dir, boat );
	}
	else if( strstr( msg, txtEntries[13] ) )
	{
		char *cmd = strstr( msg, txtEntries[14] ); // note: also checking for space
		if( !cmd )
		{
			itemTalk( mSock, tiller, 11 );
			return;
		}
		cmd += 9;
		while( *cmd && *cmd == ' ') 
			++cmd; // remove any extra spaces
		if( !(*cmd) )
		{
			itemTalk( mSock, tiller, 12 );
			return;
		}

		char tempname[512];
		sprintf( tempname, txtEntries[15], &talk[msg - cmd] );
		tiller->SetName( tempname );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cBoat::ModelBoat( cSocket *s, CItem *i )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Turn a boat into a boat model that can be re-placed
//o---------------------------------------------------------------------------o
void cBoat::ModelBoat( cSocket *s, CItem *i )
{
	CItem *tiller	= calcItemObjFromSer( i->GetMoreB() );
	CItem *p1		= calcItemObjFromSer( i->GetMoreX() );
	CItem *p2		= calcItemObjFromSer( i->GetMoreY() );
	CItem *hold		= calcItemObjFromSer( i->GetMoreZ() );

	CChar *mChar = s->CurrcharObj();

	SERIAL serial = p1->GetMore();
	if( i->GetOwnerObj() == mChar )
	{
		CItem *model = Items->SpawnItemToPack( s, mChar, "0x14f3", 0 );
		if( model == NULL )
		{
			Console.Error( 3, " Turning boat into model failed on model creation, attempted by character serial %i\n", mChar->GetSerial() );
			return;
		}

		model->SetMoreX( tiller->GetMoreX() );
		Weight->subtractItemWeight( mChar, model );
		model->SetWeight( 0 );
		Weight->addItemWeight( mChar, model );
		model->SetType(210);
		model->SetMovable( 0 );

		for( CItem *a = hold->FirstItemObj(); !hold->FinishedItems(); a = hold->NextItemObj() )
			Items->DeleItem( a );

		Items->DeleItem( tiller );
		Items->DeleItem( p1 );
		Items->DeleItem( p2 );
		Items->DeleItem( hold );
		Items->DeleItem( i );
		for( ITEM aa = 0; aa < itemcount; aa++ )
		{
			if( items[aa].GetMore() == serial )
				Items->DeleItem( &items[aa] );
		}
	}
	mChar->Teleport();
}

