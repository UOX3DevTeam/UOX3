
#include "uox3.h"
#include "debug.h"
#include "magic.h"
#include "cdice.h"
#include "ssection.h"

#undef DBGFILE
#define DBGFILE "skills.cpp"

int carptype = 0;

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Tailoring( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do Tailoring Skill Stuff
//o---------------------------------------------------------------------------o
void cSkills::Tailoring( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::Tailoring() Invalid socket" );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	CItem *packnum = getPack( mChar );
	if( packnum == NULL ) 
	{
		sysmessage( s, 773 ); 
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
	{
		sysmessage( s, 777 );
		return;
	}
	UI16 realID = i->GetID();
	if( ( realID >= 0x0F95 && realID <= 0x0F9C ) || ( realID >= 0x175D && realID <= 0x1764 ) || realID == 0x1078 )
	{
		if( !mChar->IsGM() && i->IsLockedDown() )
		{
			sysmessage( s, 774 );
			return;
		}
		if( getPackOwner( i ) != mChar )
			sysmessage( s, 775 );
		else
		{
			make_st mMake = s->ItemMake();
			int getAmt = getAmount( mChar, i->GetID() );
			if( getAmt < 1 )
			{ 
				sysmessage( s, 776 );
				return;
			}
			mMake.has = getAmt;
			mMake.material1 = i->GetID();
			s->ItemMake( mMake );
			if( realID == 0x1078 )
			{
				if( i->GetColour() == 0x00EF )
					mChar->SetTailItem( (unsigned long)-17 );	// this is better
				else
					mChar->SetTailItem( INVALIDSERIAL );
			}
			NewMakeMenu( s, 39, TAILORING );
		}
	}
}   

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Fletching( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do Fletching Skill Stuff
//o---------------------------------------------------------------------------o
void cSkills::Fletching( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::Fletching() Invalid socket" );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	CItem *packnum = getPack( mChar );
	if( packnum == NULL ) 
	{ 
		sysmessage( s, 773 ); 
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	make_st mMake = s->ItemMake();
	if( i != NULL )
	{
		if( !mChar->IsGM() && i->IsLockedDown() )
		{
			sysmessage( s, 774 );
			return;
		}
		UI16 realID = i->GetID();
		UI08 matID2 = (UI08)(mMake.material1%256);
		if( ( matID2 >= 0xD4 && matID2 <= 0xD6 && realID >= 0x1BD1 && realID <= 0x1BD3 ) ||
			( matID2 >= 0xD1 && matID2 <= 0xD3 && realID >= 0x1BD4 && realID <=0x1BD6 ) )
		{
			if( getPackOwner( i ) != mChar )
				sysmessage( s, 778 );
			else
			{
				mMake.material2 = i->GetID();
				mMake.has  = getAmount( mChar, mMake.material1 ); 
				mMake.has2 = getAmount( mChar, mMake.material2 );
				s->ItemMake( mMake );
				NewMakeMenu( s, 51, BOWCRAFT );
			}
			return;
		}
	}
	sysmessage( s, 779 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::BowCraft( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do Bowcraft Skill Stuff
//o---------------------------------------------------------------------------o
void cSkills::BowCraft( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::BowCraft() Invalid socket" );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	CItem *packnum = getPack( mChar );
	if( packnum == NULL ) 
	{
		sysmessage( s, 773 ); 
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;
	if( !mChar->IsGM() && i->IsLockedDown() )
	{
		sysmessage( s, 774 );
		return;
	}
	if( i->GetID() == 0x1BDD || i->GetID() == 0x1BE0 )
	{
		if( getPackOwner( i ) != mChar )
		{
			sysmessage( s, 780 );
			return;
		}           
		make_st mMake = s->ItemMake();
		UI16 getAmt = getAmount( mChar, i->GetID() );
		if( getAmt < 2 )
		{
			sysmessage( s, 776 );
			return;
		}
		if( mChar->IsOnHorse() )
			action( s, 0x1C );	// moved here rather then the top of fun
		else 
			action( s, 0x0D );	// so that we don't make a motion if invalid target!

		mMake.has = getAmt;
		mMake.material1 = i->GetID();
		s->ItemMake( mMake );
		NewMakeMenu( s, 49, BOWCRAFT );
	} 
}


//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Carpentry( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do Carpentry Skill Stuff
//o---------------------------------------------------------------------------o
void cSkills::Carpentry( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::Carpentry() Invalid socket" );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	CItem *packnum = getPack( mChar );
	if( packnum == NULL ) 
	{
		sysmessage( s, 773 );
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
	{
		sysmessage( s, 783 );
		return;
	}
	if( !mChar->IsGM() && i->IsLockedDown() )
	{
		sysmessage( s, 774 );
		return;
	}
	int realID = i->GetID();
	if( realID >= 0x1BD7 && realID <= 0x1BE2 )
	{
		if( getPackOwner( i ) != mChar )
		{
			sysmessage( s, 781 );
			return;
		}         
		make_st mMake = s->ItemMake();
		UI16 getAmt = getAmount( mChar, i->GetID() );
		if( getAmt < 9 )
		{	 
			sysmessage( s, 782 );
			return;
		}
		switch( realID )
		{
		case 0x1BD7:	carptype = 2;	break;
		case 0x1BE0:	
		default:		carptype = 1;	break;
		}           
		mMake.has = getAmt;
		mMake.material1 = i->GetID();
		s->ItemMake( mMake );
		NewMakeMenu( s, 19, CARPENTRY );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  SI32 cSkills::CalcRankAvg( CChar *player, createEntry& skillMake )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Unknown
//o---------------------------------------------------------------------------o
SI32 cSkills::CalcRankAvg( CChar *player, createEntry& skillMake )
{
	if( !cwmWorldState->ServerData()->GetRankSystemStatus() ) 
		return 10;

	R32 rankSum = 0;

	int rk_range, rank;
	R32 sk_range, randnum, randnum1;
	
	for( int i = 0; i < skillMake.skillReqs.size(); i++ )
	{
		rk_range = skillMake.maxRank - skillMake.minRank;
		sk_range = 50.00 + player->GetSkill( skillMake.skillReqs[i].skillNumber ) - skillMake.skillReqs[i].minSkill;
		if( sk_range <= 0 ) 
			rank = skillMake.minRank;
		else if( sk_range >= 1000 ) 
			rank = skillMake.maxRank;
		randnum = RandomNum( 0, 999 );
		if( randnum <= sk_range ) 
			rank = skillMake.maxRank;
		else
		{
			randnum1 = (R32)( RandomNum( 0, 999 ) ) - (( randnum - sk_range ) / ( 11 - cwmWorldState->ServerData()->GetSkillLevel() ) );
			rank = (int)( ( randnum1 * rk_range ) / 1000 );
			rank += skillMake.minRank - 1;
			if( rank > skillMake.maxRank ) 
				rank = skillMake.maxRank;
			if( rank < skillMake.minRank ) 
				rank = skillMake.minRank;
		}
		rankSum += rank;
	}
	return (int)(rankSum / skillMake.skillReqs.size());
}

//o---------------------------------------------------------------------------o
//|   Function    :  SI32 cSkills::CalcRank( cSocket *s, int skill )
//|   Date        :  24 August 1999
//|   Programmer  :  Magius(CHE)
//o---------------------------------------------------------------------------o
//|   Purpose     :  Calculate item rank based on player' skill.
//o---------------------------------------------------------------------------o
SI32 cSkills::CalcRank( cSocket *s, int skill )
{
	if( s == 0 )
		return 5;
	int rk_range, rank;
	R32 sk_range, randnum, randnum1;
	make_st mMake = s->ItemMake();
	CChar *mChar = s->CurrcharObj();
	rk_range = mMake.maxrank - mMake.minrank;
	sk_range = (R32) 50.00 + mChar->GetSkill( skill ) - mMake.minskill;
	if( sk_range <= 0 ) 
		rank = mMake.minrank;
	else if( sk_range >= 1000 ) 
		rank = mMake.maxrank;
	randnum = RandomNum( 0, 999 );
	//randnum = sk_range + 1.0; // for debug
	if( randnum <= sk_range ) 
		rank = mMake.maxrank;
	else
	{
		randnum1 = (R32)( RandomNum( 0, 999 ) ) - (( randnum - sk_range ) / ( 11 - cwmWorldState->ServerData()->GetSkillLevel() ) );
		rank = (int)( randnum1*rk_range)/1000;
		rank += mMake.minrank - 1;
		if( rank > mMake.maxrank ) 
			rank = mMake.maxrank;
		if( rank < mMake.minrank ) 
			rank = mMake.minrank;
	}
	s->ItemMake( mMake );
	return rank;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::ApplyRank( cSocket *s, CItem *c, int rank )
//|   Date        :  24 August 1999
//|   Programmer  :  Magius(CHE)
//o---------------------------------------------------------------------------o
//|   Purpose     :  Modify variables based on item's rank.
//o---------------------------------------------------------------------------o

void cSkills::ApplyRank( cSocket *s, CItem *c, int rank )
{
	char tmpmsg[512];
	*tmpmsg='\0';
	
	if( cwmWorldState->ServerData()->GetRankSystemStatus() )
	{
		c->SetRank( rank );
		if( c->GetLoDamage() > 0 ) 
			c->SetLoDamage( (int)( ( rank * c->GetLoDamage() ) / 10 ) );
		if( c->GetHiDamage() > 0 ) 
			c->SetHiDamage( (int)( ( rank * c->GetHiDamage() ) / 10 ) );
		if( c->GetDef() > 0 )      
			c->SetDef( (int)( ( rank * c->GetDef() ) / 10 ) );
		if( c->GetHP() > 0 )      
			c->SetHP( (int)( ( rank * c->GetHP() ) / 10 ) );
		if( c->GetMaxHP() > 0 )   
			c->SetMaxHP( (int)( ( rank * c->GetMaxHP() ) / 10 ) );
		if( c->GetValue() > 0 )
			c->SetValue( (int)( ( rank * c->GetValue() ) / 10 ) );
		
		if( rank >= 0 && rank <= 10 )
			sysmessage( s, 783 + rank );
	}
	else 
		c->SetRank( rank );
	
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Zero_Itemmake( cSocket *s )
//|   Date        :  24 August 1999
//|   Programmer  :  Magius(CHE)
//o---------------------------------------------------------------------------o
//|   Purpose     :  Resets all values into itemmake[s].
//o---------------------------------------------------------------------------o

void cSkills::Zero_Itemmake( cSocket *s )
{
	make_st mMake = s->ItemMake();
	memset( &mMake, 0, sizeof( make_st ) );
	mMake.minrank = mMake.maxrank = 10;
	s->ItemMake( mMake );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void MakeMenuTarget( cSocket *s, string x, int skill )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  create a specified menuitem target.(Can someone elaborate 
//|                  here please (EviLDeD)
//o---------------------------------------------------------------------------o
void cSkills::MakeMenuTarget( cSocket *s, std::string x, int skill )
{
	if( s == NULL )
	{
		Console.Error( 0, "MakeMenuTarget: Invalid socket making item %s and using skill %i", x.c_str(), skill );
		return;
	}
	CItem *c = NULL;
	int rank = 10, tmpneed = 0;
	UI16 getAmt = 0;
	signed oreType = -1;
	CChar *mChar = s->CurrcharObj();
	make_st mMake = s->ItemMake();

	UI16 madeMaterial1 = mMake.material1;
	UI16 madeMaterial2 = mMake.material2;
	
	if( mChar->GetMaking() != 999 )	// when using /add and a door, skill was 1755
	{
		if( !CheckSkill( mChar, skill, mMake.minskill, mMake.maxskill ) && !mChar->IsGM() ) 
		{
			if( skill != BLACKSMITHING )
				getAmt = getAmount( mChar, madeMaterial1 );
			else
			{
				oreType = mChar->GetAddPart();
				getAmt = mMake.has = getItemAmt( mChar, 0x1BF2, ores[oreType].colour );

			}
			if( getAmt < mMake.needs )
			{
				sysmessage( s, 794 );
				return;
			}
			tmpneed = mMake.needs / 2;
			if( tmpneed == 0 ) 
				mMake.needs++;
			switch( skill ) 
			{
			case BLACKSMITHING: 
				deleItemAmt( mChar, 0x1BF2, ores[oreType].colour, mMake.needs / 2 );
				
				soundeffect( s, 0x002A, true );
				sysmessage( s, 795 );
				break;
			case CARPENTRY:     
				if( carptype == 1 ) 
					deleQuan( mChar, 0x1BE0, mMake.needs / 2 );
				if( carptype == 2 ) 
					deleQuan( mChar, 0x1BD7, mMake.needs / 2 );
				soundeffect( s, 0x023D, true );
				sysmessage( s, 795 );
				break;
			case INSCRIPTION:
				deleQuan( mChar, 0x0E34, 1 );
				sysmessage( s, 796 );
				break;
			case TAILORING:     
				if( mChar->GetTailItem() != INVALIDSERIAL )	// corrupting for our purposes, better than runenumb!
					deleItemAmt( mChar, madeMaterial1, 0x00EF, mMake.needs / 2 );
				else
					deleQuan( mChar, madeMaterial1, mMake.needs / 2 );  
				soundeffect( s, 0x0248, true );
				sysmessage( s, 795 );
				mChar->SetTailItem( INVALIDSERIAL );
				break;
			case COOKING:     
				deleQuan( mChar, 0x175D, mMake.needs / 2 );  
				soundeffect( s, 0x0225, true );
				sysmessage( s, 795 );
				break;
			case BOWCRAFT:      
				if( mMake.has < 2 ) 
					deleQuan( mChar, madeMaterial1, 1 );
				else 
					deleQuan( mChar, madeMaterial1, mMake.needs / 2 );
				if( mMake.has2 < 2 ) 
					deleQuan( mChar, madeMaterial2, 1 );
				else 
					deleQuan( mChar, madeMaterial2, mMake.needs / 2 );
				soundeffect( s, 0x004A, true );
				sysmessage( s, 795 );
				mMake.has = 0;
				mMake.has2 = 0;
				break;
			case TINKERING:
				deleQuan( mChar, madeMaterial1, mMake.needs / 2 );
				soundeffect( s, 0x002A, true );
				sysmessage( s, 795 );
				break;
			default:
				deleQuan( mChar, madeMaterial1, mMake.needs / 2 );
				sysmessage( s, 795 );
			}
			s->ItemMake( mMake );
			return;
		}
		if( skill != BLACKSMITHING )
			getAmt = getAmount( mChar, madeMaterial1 );
		else
		{
			oreType = mChar->GetAddPart();
			getAmt = mMake.has = getItemAmt( mChar, 0x1BF2, ores[oreType].colour );
		}
		if( getAmt < mMake.needs && !mChar->IsGM() )
		{
			sysmessage( s, 794 );
			return;
		}
		switch( skill )
		{
			
		case BLACKSMITHING:
			deleItemAmt( mChar, 0x1BF2, ores[oreType].colour, mMake.needs );
			break;
		case CARPENTRY:
			if( carptype == 1 ) 
				deleQuan( mChar, 0x1BE0, mMake.needs );
			if( carptype == 2 ) 
				deleQuan( mChar, 0x1BD7, mMake.needs );
			break;
		case INSCRIPTION:	deleQuan( mChar, madeMaterial1, 1 ); break; // don't use default, cauz we delete 1 scroll // use materialid
		case BOWCRAFT:
			deleQuan( mChar, madeMaterial1, mMake.needs );
			deleQuan( mChar, madeMaterial2, mMake.needs );
			break;		
		case TAILORING:
			if( mChar->GetTailItem() != INVALIDSERIAL )
				deleItemAmt( mChar, madeMaterial1, 0x00EF, mMake.needs / 2 );
			else
				deleQuan( mChar, madeMaterial1, mMake.needs/2 );
			mChar->SetTailItem( INVALIDSERIAL );
			break;
		default:	
			deleQuan( mChar, madeMaterial1, mMake.needs );
		}
		
		mMake.material1 = 0;
		c = Items->SpawnItemToPack( s, mChar, x, false );
		if( c == NULL )
		{
			Console.Error( 2, "MakeMenuTarget bad script item # %s(Item Not found).\n", x.c_str() );
			return;
		}
		else
		{
			c->SetName2( c->GetName() );
			if( cwmWorldState->ServerData()->GetRankSystemStatus() ) 
				rank = CalcRank( s, skill );
			else
				rank = 10;
			ApplyRank( s, c, rank );
			
			if( !mChar->IsGM() )
			{
				c->SetCreator( mChar->GetSerial() );
				if( mChar->GetSkill( skill ) > 950 ) 
					c->SetMadeWith( skill + 1 );
				else 
					c->SetMadeWith( 0 - skill - 1 );
			}
			else
			{
				c->SetCreator( INVALIDSERIAL );
				c->SetMadeWith( 0 );
			}
		}
		
		c->SetMagic( 1 );
		if( mChar->GetMaking() == 999 ) 
			mChar->SetMaking( c->GetSerial() );
		else 
			mChar->SetMaking( 0 );
		switch( skill )
		{
		case MINING:		soundeffect( s, 0x0054, true ); break;
		case BLACKSMITHING:	soundeffect( s, 0x002A, true ); break;
		case CARPENTRY:		soundeffect( s, 0x023D, true ); break;
		case INSCRIPTION:	soundeffect( s, 0x0249, true ); break;
		case TAILORING:		soundeffect( s, 0x0248, true ); break;
		case TINKERING:		soundeffect( s, 0x002A, true ); break;
		case COOKING:		soundeffect( s, 0x0225, true ); break;
		}

		if( !mChar->GetMaking() ) 
			sysmessage( s, 797 );
		mMake.has = 0;
		mMake.has2 = 0;
		statwindow( s, mChar );
		s->ItemMake( mMake );
	}
}
//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::RegenerateOre( long grX, long grY )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Regenerate Ore based on Server.scp Ore respawn settings
//o---------------------------------------------------------------------------o
void cSkills::RegenerateOre( long grX, long grY )
{
	resourceEntry *orePart = &resources[grX][grY];
	UI32 oreCeiling = cwmWorldState->ServerData()->GetResOre();
	UI32 oreTimer = cwmWorldState->ServerData()->GetResOreTime();
	if( orePart->oreTime <= uiCurrentTime )	// regenerate some more?
	{
		for( UI32 counter = 0; counter < oreCeiling; counter++ )	// keep regenerating ore
		{
			if( orePart->oreAmt < oreCeiling && ( orePart->oreAmt + counter * oreTimer * CLOCKS_PER_SEC ) < uiCurrentTime )
				orePart->oreAmt++;
			else
				break;
		}
		orePart->oreTime = BuildTimeValue( oreTimer );
	}
	if( orePart->oreAmt > oreCeiling )
		orePart->oreAmt = oreCeiling;
}

//o--------------------------------------------------------------------------
//| Function    - void cSkills::Mine( cSocket *s )
//| Date        - Unknown
//| Programmer  - Unknown
//| Modified    - Cork(Unknown)/Abaddon(February 19, 2000)
//o--------------------------------------------------------------------------
//| Purpose     - (Fill this in)
//| Comments    - Skill checking now implemented. You cannot mine colored ore
//|               unless you have the proper mining skill for each ore type. -Cork
//|               Rewrote most of it to clear up some of the mess-Abaddon
//o--------------------------------------------------------------------------
void cSkills::Mine( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::Mine() Invalid socket" );
		return;
	}
	bool floor = false, mountain = false;
	SI16 oreX, oreY;
	
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;	// did we cancel the target?

	if( cwmWorldState->ServerData()->GetResOreArea() < 10 )	// make sure there are enough minimum areas
		cwmWorldState->ServerData()->SetResOreArea( 10 );

	SI16 targetX = s->GetWord( 0x0B );		// store our target x y and z locations
	SI16 targetY = s->GetWord( 0x0D );
	SI08 targetZ = s->GetByte( 0x10 );
	CChar *mChar = s->CurrcharObj();
	SI16 distX = abs( mChar->GetX() - targetX );			// find our distance
	SI16 distY = abs( mChar->GetY() - targetY );

	if( distX > 5 || distY > 5 )							// too far away?
	{
		sysmessage( s, 799 );
		return;
	}
	
	mChar->SetSkillDelay( BuildTimeValue( cwmWorldState->ServerData()->GetServerSkillDelayStatus() ) );

	UI08 targetID1 = s->GetByte( 0x11 );							// store ids of what we targeted
	UI08 targetID2 = s->GetByte( 0x12 );

	if( targetZ < 28 && targetID1 == 14 )					// if not too high
	{
		switch( targetID2 )
		{
		  case 0xD3:
		  case 0xDF:
		  case 0xE0:
		  case 0xE1:
		  case 0xE8:
			  GraveDig( s );							// check to see if we targeted a grave, if so, check it
			  break;
		  default:
			  break;
		}
	}
	
	switch( cwmWorldState->ServerData()->GetMineCheck() )
	{
	case 0:
		floor = true;
		mountain = true;
		break;
	case 1:
		if( targetZ == 0 )			// check to see if we're targetting a dungeon floor
		{
			if( targetID1 == 0x05 )
			{
				if( ( targetID2 >= 0x3B && targetID2 <= 0x4F ) || ( targetID2 >= 0x51 && targetID2 <= 0x53 ) || ( targetID2 == 0x6A ) )
					floor = true;
			}
		}
		if( !floor && targetZ >= 0 )	// mountain not likely to be below 0 (but you never know, do you? =)
		{
			if( targetID1 != 0 && targetID2 != 0 )	// we might have a static rock or mountain
			{
				MapStaticIterator msi( targetX, targetY, mChar->WorldNumber() );
				CTile tile;
				staticrecord *stat = NULL;
				while( ( ( stat = msi.Next() ) != NULL ) && !mountain )
				{
					msi.GetTile( &tile );
					if( targetZ == stat->zoff && ( !strcmp( tile.Name(), "rock" ) || !strcmp( tile.Name(), "mountain" ) || !strcmp( tile.Name(), "cave" ) ) )
						mountain = true;
				}
			}
			else		// or it could be a map only
			{  // manually calculating the ID's if a maptype
				map_st map1;
				CLand land;
				map1 = Map->SeekMap0( targetX, targetY, mChar->WorldNumber() );
				Map->SeekLand( map1.id, &land );
				if( !strcmp( "rock", land.Name() ) || !strcmp( land.Name(), "mountain" ) || !strcmp( land.Name(), "cave" ) ) 
					mountain = true; 
			}
		}
		break;
	case 2:	// need to modify scripts to support this!
		floor = true;		// we'll default to work everywhere for these scripts
		mountain = true;
		break;
	default:
		sysmessage( s, 800 );
		return;
	}

	oreX = targetX / cwmWorldState->ServerData()->GetResOreArea();	// we want the area where we targetted, not where we are
	oreY = targetY / cwmWorldState->ServerData()->GetResOreArea();
	if( (!floor && !mountain) || (oreX >= 610 || oreY >= 410) )		// if we can't mine here or if for some reason it's invalid, clear out
	{
		sysmessage( s, 801 );
		return;
	}

	RegenerateOre( oreX, oreY );
	resourceEntry *orePart = &resources[oreX][oreY];
	if( orePart->oreAmt <= 0 )
	{
		sysmessage( s, 802 );
		return;
	}
	if( mChar->IsOnHorse() != 0 )	// do action and sound
		action( s, 0x1A );
	else
		action( s, 0x0B );
	
	soundeffect( s, 0x0125, true ); 
	
	if( !CheckSkill( mChar, MINING, 0, 1000 ) ) // check to see if our skill is good enough
	{
		sysmessage( s, 803 );
		if( orePart->oreAmt > 0 && RandomNum( 0, 1 ) )
			orePart->oreAmt--;
		return;
	} 
	else if( orePart->oreAmt > 0 )
		orePart->oreAmt--;
	
#ifdef _DEBUG
		Console << "DBG: Mine(\"" << mChar->GetName() << "\"[" << mChar->GetSerial() << "]); --> MINING: " << mChar->GetSkill( MINING ) << "  RaceID: " << mChar->GetRace() << myendl;
#endif
		
		cTownRegion *targRegion = region[mChar->GetRegion()];
		if( mChar->GetSkill( MINING ) >= targRegion->GetMinColourSkill() && RandomNum( 0, 100 ) >= targRegion->GetChanceColourOre() )
		MakeOre( mChar->GetRegion(), mChar, s );
		else  //  We didn't find any colored ore, so grab some iron ore
		{
			if( RandomNum( 0, 100 ) > targRegion->GetChanceBigOre() )
				Items->SpawnItem( s, 5, "Iron Ore", true, 0x19BA, 0, true, true );
			else
				Items->SpawnItem( s, 1, "Iron Ore", true, 0x19B9, 0, true, true );
			sysmessage( s, 804 );
		}
	}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::GraveDig( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do GraveDigging Stuff (Should probably be replaced with an
//|											OSI-like Treasure Hunting System based off a Resource
//|											System much like Ore and Logs)
//o---------------------------------------------------------------------------o
void cSkills::GraveDig( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::GraveDig() Invalid socket" );
		return;
	}
	int		nAmount, nFame;
	char	iID = 0;
	CItem *nItemID = NULL;
	
	CChar *nCharID = s->CurrcharObj();
	Karma( nCharID, NULL, -2000 ); // Karma loss no lower than the -2 pier
	
	if( nCharID->IsOnHorse() )
		action( s, 0x1A );
	else
		action( s, 0x0b );
	soundeffect( s, 0x0125, true );
	if( !CheckSkill( nCharID, MINING, 0, 800 ) ) 
	{
		sysmessage( s, 805 );
		return;
	}
	
	nFame = nCharID->GetFame();
	if( nCharID->IsOnHorse() )
		action( s, 0x1A );
	else  
		action( s, 0x0B );
	soundeffect( s, 0x0125, true );
	switch( RandomNum( 0, 12 ) )
	{
	case 2:
		SpawnRandomMonster( s, "necro.scp", "UNDEADLIST", "1000" ); // Low level Undead - Random
		sysmessage( s, 806 );
		break;
	case 4:
		nItemID = SpawnRandomItem( s, true, "necro.scp", "ITEMLIST", "1001" ); // Armor and shields - Random
		if( nItemID == NULL )
			break;
		if( nItemID->GetID() >= 7026 && nItemID->GetID() <= 7035 )
			sysmessage( s, 807 );
		else
			sysmessage( s, 808 );
		break;
	case 5:
		//Random treasure between gems and gold
		if( RandomNum( 0, 1 ) )
		{  // randomly create a gem and place in backpack
			SpawnRandomItem( s, true, "necro.scp", "ITEMLIST", "999" );
			sysmessage( s, 809 );
		}
		else
		{  // Create between 1 and 15 goldpieces and place directly in backpack
			nAmount = RandomNum( 1, 15 );
			addgold( s, nAmount );
			goldsfx( s, nAmount );
			if( nAmount == 1 )
				sysmessage( s, 810, nAmount );
			else
				sysmessage( s, 810, nAmount );
		}
		break;
	case 6:
		if( nFame < 500 )
			SpawnRandomMonster( s, "necro.scp", "UNDEADLIST", "1000" ); // Low level Undead - Random
		else
			SpawnRandomMonster( s, "necro.scp", "UNDEADLIST", "1001" ); // Med level Undead - Random
		sysmessage( s, 806 );
		break;
	case 8:
		SpawnRandomItem( s, true, "necro.scp", "ITEMLIST", "1000" );
		sysmessage( s, 811 );
		break;
	case 10:
		if( nFame < 1000 )
			SpawnRandomMonster( s, "necro.scp", "UNDEADLIST", "1001" ); // Med level Undead - Random
		else
			SpawnRandomMonster( s, "necro.scp", "UNDEADLIST", "1002" ); // High level Undead - Random
		sysmessage( s, 806 );
		break;
	case 12:
		if( nFame > 1000 )
			SpawnRandomMonster( s, "necro.scp", "UNDEADLIST", "1002" ); // High level Undead - Random
		else
			SpawnRandomMonster( s, "necro.scp", "UNDEADLIST", "1001" ); // Med level Undead - Random
		sysmessage( s, 806 );
		break;
	default:
		if( RandomNum( 0, 1 ) )
		{
			iID = RandomNum( 0, 11 ) + 0x11;
			Items->SpawnItem( s, 1, NULL, false, 0x1B00 + iID, 0, true, true );
			sysmessage( s, 812 );
		}
		else	// found an empty grave
			sysmessage( s, 813 );
	}
}

//o--------------------------------------------------------------------------
//| Function    - void cSkills::SmeltOre( cSocket *s );
//| Date        - Unknown
//| Programmer  - Unknown
//| Modified    - Abaddon(February 19, 2000)
//o--------------------------------------------------------------------------
//| Purpose     - Rewritten to use case and structure, you'll find it is 
//|               easier to make it scriptable now. The structure is pretty 
//|               much all that'd be needed for any future ore->ingot conversions
//|               scripting the ore would probably be even simpler, requires 
//|               less info
//o--------------------------------------------------------------------------
void cSkills::SmeltOre( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::SmeltOre() Invalid socket" );
		return;
	}

	CChar *chr = s->CurrcharObj();
	ITEM smeltedItem = chr->GetSmeltItem();
	CItem *anvil = calcItemObjFromSer( s->GetDWord( 7 ) );				// Let's find our anvil
	
	if( anvil != NULL )					// if we have an anvil
	{
		switch( anvil->GetID() )	// Check to ensure it is an anvil
		{
		case 0x0FB1:
		case 0x197A:
		case 0x19A9:
		case 0x197E:
		case 0x1982:
		case 0x1986:
		case 0x198A:
		case 0x198E:
		case 0x1992:
		case 0x1996:
		case 0x199A:
		case 0x19A6:
		case 0x19A2:
		case 0x199E:
			if( itemInRange( chr, anvil, 3 ) ) //Check if the forge is in range  
			{
				int oreType = -1;
				UI16 targColour = items[smeltedItem].GetColour();
				oreType = FindOreType( targColour );
				if( oreType == -1 )
				{
					sysmessage( s, 814 );
					return;
				}
				char ingotString[100];
				if( ores[oreType].foreign )	// if not iron, generally
				{
					if( chr->GetSkill( MINING ) < ores[oreType].minSkill )
					{
						sysmessage( s, 815 );
						return;
					}
				}
				if( !CheckSkill( chr, MINING, ores[oreType].minSkill, 1000 ) )	// if we do not have minimum skill to use it
				{
					if( items[smeltedItem].GetAmount() > 1 )	// If more than 1 ore, half it
					{
						sysmessage( s, 817 );
						items[smeltedItem].SetAmount( items[smeltedItem].GetAmount() / 2 );
					}
					else
					{
						sysmessage( s, 816 );
						Items->DeleItem( &items[smeltedItem] );
					}
					return;
				}
				UI16 ingotNum = items[smeltedItem].GetAmount() * 2;	// 2 Ingots per ore pile.... shouldn't this be variable based on quality of ore?
				sprintf( ingotString, "%s Ingot", ores[oreType].name.c_str() );
				Items->SpawnItem( s, ingotNum, ingotString, true, 0x1BF2, ores[oreType].colour, true, true );
				sysmessage( s, 818 );
				sysmessage( s, 819, ores[oreType].name.c_str() );
				Items->DeleItem( &items[chr->GetSmeltItem()] );	// delete the ore
			}
			break;
		default:
			sysmessage( s, 820 );
			break;
		}     
	} 

	chr->SetSmeltItem( INVALIDSERIAL );
	Weight->calcWeight( chr );
	statwindow( s, chr );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Wheel( cSocket *s, int mat )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when using Yarn or Thread on a Spinning Wheel
//o---------------------------------------------------------------------------o
void cSkills::Wheel( cSocket *s )
{
	if( s == NULL )
		return;
	
	bool canTailor = false;

	CChar *mChar = s->CurrcharObj();
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		if( !mChar->IsGM() && i->IsLockedDown() )
		{
			sysmessage( s, 774 );
			return;
		}
		if( i->GetID() >= 0x10A4 && i->GetID() <= 0x10A6 )
		{
			if( itemInRange( mChar, i,3 ) )  
			{
				if( !CheckSkill( mChar, TAILORING, 0, 1000 ) ) 
				{
					sysmessage( s, 821 );
					return;
				}   
				sysmessage( s, 822 );

				ITEM tailItem = mChar->GetTailItem();
				items[tailItem].SetName( "#" );
				if( items[tailItem].GetID() == 0x0DF8 ) // Wool
					items[tailItem].SetID( 0x0E1D );
				else if( items[tailItem].GetID() == 0x0DF9 ) // Cotton
					items[tailItem].SetID( 0x0FA0 );
				items[tailItem].SetAmount( items[tailItem].GetAmount() * 4 );
				
				items[mChar->GetTailItem()].SetDecayable( true );
				RefreshItem( &items[mChar->GetTailItem()] );
				canTailor = true;
			}
		}     
	}   
	mChar->SetTailItem( INVALIDSERIAL );        
	if( !canTailor ) 
		sysmessage( s, 823 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Loom( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when using a loom to make cloth
//o---------------------------------------------------------------------------o
void cSkills::Loom( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::Loom() Invalid socket" );
		return;
	}
	bool canTailor = false;

	CChar *mChar = s->CurrcharObj();
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		if( !mChar->IsGM() && i->IsLockedDown() )
		{
			sysmessage( s, 774 );
			return;
		}
		if( i->GetID() >= 0x105F && i->GetID() <= 0x1061 )
		{
			if( itemInRange( mChar, i, 3 ) )  
			{
				if( !CheckSkill( mChar, TAILORING, 0, 1000 ) ) 
				{
					sysmessage( s, 824 );
					return;
				}   
				sysmessage( s, 825 );
				ITEM tailItem = mChar->GetTailItem();
				items[tailItem].SetName( "#");
				items[tailItem].SetID( 0x175D );
				items[tailItem].SetDecayable( true );
				items[tailItem].SetAmount( items[tailItem].GetAmount() * 2 );
				
				RefreshItem( &items[mChar->GetTailItem()] );
				canTailor = true;
			}
		}     
	}   
	mChar->SetTailItem( INVALIDSERIAL );
	if( !canTailor ) 
		sysmessage( s, 823 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::CookMeat( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Used by the cooking skill to cook meat.
//o---------------------------------------------------------------------------o
void cSkills::CookMeat( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::CookMeat() Invalid socket" );
		return;
	}
	bool canCook = false;

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar = s->CurrcharObj();
	ITEM tailItem;
	if( i != NULL )
	{
		if( !mChar->IsGM() && i->IsLockedDown() )
		{
			sysmessage( s, 774 );
			return;
		}
		UI16 realItem = i->GetID();
		if( realItem >= 0x0DE3 && realItem <= 0x0DE9 ||	realItem == 0x0FAC ||
			realItem == 0x0FB1 || realItem >= 0x197A && realItem <= 0x19B6 ||
			realItem >= 0x0461 && realItem <= 0x0480 ||	realItem >= 0x0E31 && realItem <= 0x0E33 ||
			realItem == 0x19BB || realItem == 0x1F2B || realItem >= 0x092B && realItem <= 0x0934 ||
			realItem >= 0x0937 && realItem <= 0x0942 ||	realItem >= 0x0945 && realItem <= 0x0950 ||
			realItem >= 0x0953 && realItem <= 0x095E ||	realItem >= 0x0961 && realItem <= 0x096C )
		{
			if( itemInRange( mChar, i, 3 ) )  
			{
				tailItem = mChar->GetTailItem();
				if( !CheckSkill( mChar, COOKING, 0, 1000 ) ) 
				{
					char burntName[50];
					int amntDiff = RandomNum( 1, items[mChar->GetTailItem()].GetAmount() );
					sprintf( burntName, Dictionary->GetEntry( 1430 ), amntDiff );
					sysmessage( s, 1431 );
					tailItem = decItemAmount( tailItem, amntDiff );
					Items->SpawnItem( s, amntDiff, burntName, false, 0x1EB0, 0, true, true );
					return;
				}   
				sysmessage( s, 1432 );
				
				items[tailItem].SetName( "#");
				
				items[tailItem].SetID( 0x09F2 );
				items[tailItem].SetType( 14 );
				items[tailItem].SetDecayable( true ); 
				
				RefreshItem( &items[tailItem] );
				canCook = true;
			}
		}     
	}   
	mChar->SetTailItem( INVALIDSERIAL );
	if( !canCook ) 
		sysmessage( s, 1433 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::MakeDough( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Make Dough by using flour and water in a mixer
//o---------------------------------------------------------------------------o
void cSkills::MakeDough( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::MakeDuugh() Invalid socket" );
		return;
	}
	bool canMix = false;
	ITEM tailItem;

	CChar *mChar = s->CurrcharObj();
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		if( !mChar->IsGM() && i->IsLockedDown() )
		{
			sysmessage( s, 774 );
			return;
		}
		if( i->GetID() == 0x103A )
		{
			if( itemInRange( mChar, i, 3 ) )
			{
				if( !CheckSkill( mChar, COOKING, 0, 1000 ) ) 
				{
					sysmessage( s, 826 );
					return;
				}   
				sysmessage( s, 827 );
				tailItem = mChar->GetTailItem();	
				items[tailItem].SetName( "#" );
				
				items[tailItem].SetID( 0x103D );
				items[tailItem].SetDecayable( true );
				items[tailItem].SetAmount( items[tailItem].GetAmount() * 2 );
				
				RefreshItem( &items[tailItem] );
				canMix = true;
			}
		}     
	}   
	mChar->SetTailItem( INVALIDSERIAL );
	if( !canMix ) 
		sysmessage( s, 830 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::MakePizza( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Used to Make Pizza (Perhaps we should combine this cooking stuff?)
//o---------------------------------------------------------------------------o
void cSkills::MakePizza( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::MakePizza() Invalid socket" );
		return;
	}
	bool canMix = false;
	ITEM tailItem;	

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar = s->CurrcharObj();
	if( i != NULL )
	{
		if( !mChar->IsGM() && i->IsLockedDown() )
		{
			sysmessage( s, 774 );
			return;
		}
		if( i->GetID() == 0x103D )
		{
			if( itemInRange( mChar, i, 3 ) )  
			{
				if( !CheckSkill( mChar, COOKING, 0, 1000 ) ) 
				{
					sysmessage( s, 828 );
					Items->DeleItem( i );
					return;
				}   
				sysmessage( s, 829 );
				tailItem = mChar->GetTailItem();
				items[tailItem].SetName( "#" );
				
				items[tailItem].SetID( 0x1083 );
				items[tailItem].SetDecayable( true );
				items[tailItem].SetAmount( items[tailItem].GetAmount() * 2 );
				
				RefreshItem( &items[tailItem] );
				canMix = true;
			}
		}     
	}   
	mChar->SetTailItem( INVALIDSERIAL );
	if( !canMix ) 
		sysmessage( s, 830 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Hide( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//|									
//|	Modification	-	09/22/2002	-	Xuri - Support for user config of hiding 
//|									while mounted
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses hiding skill
//o---------------------------------------------------------------------------o
void cSkills::Hide( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::Hide() Invalid socket" );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	
	if(!cwmWorldState->ServerData()->GetCharHideWhileMounted())
	{
		if( mChar->IsOnHorse() )
		{
			sysmessage( s, 831 );
			return;
		}
	}
	if( mChar->GetAttacker() != INVALIDSERIAL && charInRange( mChar, &chars[mChar->GetAttacker()] ) )
	{
		sysmessage( s, 832 );
		return;
	}
	if( mChar->GetHidden() == 1 )
	{
		sysmessage( s, 833 );
		return;
	}
	if( !CheckSkill( mChar, HIDING, 0, 1000 ) ) 
	{
		sysmessage( s, 834 );
		return;
	}   
	sysmessage( s, 835 );
	mChar->SetHidden( 1 );
	mChar->SetStealth( -1 );
	mChar->Update();
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Stealth( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses Stealth skill
//o---------------------------------------------------------------------------o
void cSkills::Stealth( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::Stealth() Invalid socket" );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	if( mChar->GetHidden() == 0 ) 
	{
		sysmessage( s, 836 );
		return;
	}
	if( mChar->IsOnHorse() )
	{
		sysmessage( s, 837 );
		return;
	}
	if( mChar->GetSkill( HIDING ) < 700 )
	{
		sysmessage( s, 838 );
		mChar->ExposeToView();
		return;
	}   		
	if( !CheckSkill( mChar, STEALTH, 0, 1000 ) ) 
	{
		mChar->ExposeToView();
		return;
	}   
	sysmessage( s, 839, cwmWorldState->ServerData()->GetMaxStealthMovement() );
	mChar->SetHidden( 1 );
	mChar->SetStealth( 0 );
	mChar->RemoveFromSight();
	mChar->SendToSocket( s, true, mChar );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::TreeTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//|									
//|	Modification	-	09/22/2002	-	Xuri - Changed default minimum range for 
//|									chopping trees from unrealistic 5 to logical 2 :)
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses an item and targets a tree
//o---------------------------------------------------------------------------o
void cSkills::TreeTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::TreeTarget() Invalid socket" );
		return;
	}

	CChar *mChar = s->CurrcharObj();
	if( cwmWorldState->ServerData()->GetResLogArea() < 10 ) 
		cwmWorldState->ServerData()->SetResLogArea( 10 );
	
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;
	
	int px = s->GetWord( 0x0B );
	int py = s->GetWord( 0x0D );
	int cx = abs( mChar->GetX() - px );
	int cy = abs( mChar->GetY() - py );

	UI08 dir = getCharDir( mChar, px, py );
	if( dir != 0xFF && mChar->GetDir() != dir )
	{
		mChar->SetDir( dir );
		mChar->Teleport();
	}
	if( cx > 2 || cy > 2 )
	{
		sysmessage( s, 393 );
		return;
	}

	SI16 a = mChar->GetX() / cwmWorldState->ServerData()->GetResLogArea();
	SI16 b = mChar->GetY() / cwmWorldState->ServerData()->GetResLogArea();

	if( a >= 610 || b >= 410 ) 
		return; // wih the previous a < 20 || b < 20, wind may not have worked right, as well as some dungeons
	
	RegenerateLog( a, b );
	resourceEntry *logPart = &resources[a][b];
	if( logPart->logAmt <= 0 )
	{
		sysmessage( s, 840 );
		return;
	}
	
	CItem *packnum = getPack( mChar );
	if( packnum == NULL ) 
	{ 
		sysmessage( s, 841 ); 
		return; 
	}
	
	if( mChar->IsOnHorse() ) 
		action( s, 0x1C );
	else 
		action( s, 0x0D );
	soundeffect( s, 0x013E, true );
	
	if( !CheckSkill( mChar, LUMBERJACKING, 0, 1000 ) ) 
	{
		sysmessage( s, 842 );
		if( logPart->logAmt > 0 && RandomNum( 1, 2 ) == 1 ) 
			logPart->logAmt--;//Randomly deplete resources even when they fail 1/2 chance you'll loose wood.
		return;
	}
	
	if( logPart->logAmt > 0 ) 
		logPart->logAmt--;
	

		CItem *c = Items->SpawnItem( s, 10, "#", true, 0x1BE0, 0, true, true );
		if( c == NULL )
			return;
		if( c->GetAmount() > 10 ) 
			sysmessage( s, 1434 );
		else 
			sysmessage( s, 1435 );
	}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::RegenerateLog( long grX, long grY )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Used by logging system to respawn the log resource
//o---------------------------------------------------------------------------o
void cSkills::RegenerateLog( long grX, long grY )
{
	SI16 logCeiling = cwmWorldState->ServerData()->GetResLogs();
	long logTimer = cwmWorldState->ServerData()->GetResLogTime();
	resourceEntry *logPart = &resources[grX][grY];
	if( logPart->logTime <= uiCurrentTime )
	{
		for( int c = 0; c < logCeiling; c++ )
		{
			if( ( logPart->logTime + ( c * logTimer * CLOCKS_PER_SEC ) ) <= uiCurrentTime && logPart->logAmt < logCeiling )
				logPart->logAmt++;
			else 
				break;
		}
		logPart->logTime = BuildTimeValue( logTimer );//10 more mins
	}
	
	if( logPart->logAmt > logCeiling ) 
		logPart->logAmt = logCeiling;
	
}


//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::DetectHidden( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses the detect hidden skill, Greater
//|					 your skill is the longer range you can detect from where
//|					 where you target, hiders near the center of the target
//|					 are easier to detect, while hiders further from the center
//|					 are more difficult to detect
//o---------------------------------------------------------------------------o
void cSkills::DetectHidden( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::DetectHidden() Invalid socket" );
		return;
	}
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;
	
	int j, getSkill;
	SI16 dx, dy;
	R64 c,range;         //int is too restricting
	
	SI16 x = s->GetWord( 11 );
	SI16 y = s->GetWord( 13 );
//	SI08 z = s->GetByte( 16 );
	CChar *mChar = s->CurrcharObj();

	j = mChar->GetSkill( DETECTINGHIDDEN );
	
	range = (j*j/1.0E6) * ( MAXVISRANGE + Races->VisRange( mChar->GetRace() ) );     // this seems like an ok formula
	
	int xOffset = MapRegion->GetGridX( mChar->GetX() );
	int yOffset = MapRegion->GetGridY( mChar->GetY() );
	UI08 worldNumber = s->CurrcharObj()->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
			if( MapArea == NULL )	// no valid region
				continue;
			CChar *tempChar;
			MapArea->PushChar();
			for( tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
			{
				if( tempChar == NULL )
					continue;
				if( tempChar->GetHidden() == 1 ) // do not detect invis people only hidden ones
				{
					dx = abs( tempChar->GetX() - x );
					dy = abs( tempChar->GetY() - y );
					c = hypot( dx, dy );
					getSkill = (int)( tempChar->GetSkill( HIDING ) * tempChar->GetSkill( HIDING ) / 1E3 - (range*50/(MAXVISRANGE + Races->VisRange( mChar->GetRace() )) )*(range-c)/range);
					if( getSkill < 0 ) 
						getSkill = 0;
					if( getSkill > 1000 ) 
						getSkill = 1000;
					
					if( ( CheckSkill( mChar, DETECTINGHIDDEN, getSkill, 1000 ) ) && ( c <= range ) )
					{
						tempChar->ExposeToView();
						cSocket *kSock = calcSocketObjFromChar( tempChar );
						if( kSock != NULL )
							sysmessage( kSock, 1436 );
					}
					else 
						sysmessage( s, 1437 );
				}
			}
			MapArea->PopChar();
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::PeaceMaking( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when using the PeaceMaking Skill, Also requires
//|					 adequate Musicianship skill to succeed.
//o---------------------------------------------------------------------------o
void cSkills::PeaceMaking( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::PeaceMaking() Invalid socket" );
		return;
	}
	CItem *getInst = GetInstrument( s );
	if( getInst == NULL ) 
	{
		sysmessage( s, 1438 );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	bool cs1 = CheckSkill( mChar, MUSICIANSHIP, 0, 1000 );
	bool cs2 = CheckSkill( mChar, PEACEMAKING, 0, 1000 );
	if( cs1 && cs2 )
	{
		PlayInstrumentWell( s, getInst );
		sysmessage( s, 1439 );

		int xOffset = MapRegion->GetGridX( mChar->GetX() );
		int yOffset = MapRegion->GetGridY( mChar->GetY() );
		UI08 worldNumber = mChar->WorldNumber();
		for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
		{
			for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
			{
				SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
				if( MapArea == NULL )	// no valid region
					continue;
				MapArea->PushChar();
				for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
				{
					if( tempChar == NULL )
						continue;
					if( charInRange( tempChar, s->CurrcharObj() ) && tempChar->IsAtWar() )
					{
						cSocket *jSock = calcSocketObjFromChar( tempChar );
						if( jSock != NULL )
							sysmessage( jSock, 1440 );
						if( tempChar->IsAtWar() ) 
							npcToggleCombat( tempChar );
						tempChar->SetTarg( INVALIDSERIAL );
						tempChar->SetAttacker( INVALIDSERIAL );
						tempChar->SetAttackFirst( false );
					}
				}
				MapArea->PopChar();
			}
		}
	} 
	else 
	{
		PlayInstrumentPoor( s, getInst );
		sysmessage( s, 1441 );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::PlayInstrumentWell( cSocket *s, CItem *i )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Play "PlayedWell" soundfx based on instrument used
//o---------------------------------------------------------------------------o
void cSkills::PlayInstrumentWell( cSocket *s, CItem *i )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::PlayInstrumentWell() Invalid socket using item %i ", i->GetSerial() );
		return;
	}
	switch( i->GetID( 2 ) )
	{
	case 0x9C:	soundeffect( s, 0x0038, true );	break;
	case 0x9D:
	case 0x9E:	soundeffect( s, 0x0052, true );	break;
	case 0xB1:
	case 0xB2:	soundeffect( s, 0x0043, true );	break;
	case 0xB3:
	case 0xB4:	soundeffect( s, 0x004C, true );	break;
	default:
		Console.Error( 2, " Fallout of switch statement without default. skills.cpp, cSkills::PlayInstrumentWell()" );
		return;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::PlayInstrumentPoor( cSocket *s, CItem *i )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Play "PlayPoorly" soundfx based on instrument used
//o---------------------------------------------------------------------------o
void cSkills::PlayInstrumentPoor( cSocket *s, CItem *i )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::PlayInstrumentPoor() Invalid socket using item %i", i->GetSerial() );
		return;
	}
	switch( i->GetID( 2 ) )
	{
	case 0x9C:	soundeffect( s, 0x0039, true );	break;
	case 0x9D:
	case 0x9E:	soundeffect( s, 0x0053, true );	break;
	case 0xB1:
	case 0xB2:	soundeffect( s, 0x0044, true );	break;
	case 0xB3:
	case 0xB4:	soundeffect( s, 0x004D, true );	break;
	default:
		Console.Error( 2, " Fallout of switch statement without default. skills.cpp, cSkills::PlayInstrumentPoor()" );
		return;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  CItem *cSkills::GetInstrument( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Used by Peacemaking, Provocation and Enticement to find
//|					 the instrument in your pack (if any exist)
//o---------------------------------------------------------------------------o
CItem * cSkills::GetInstrument( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::GetInstrument() Invalid socket" );
		return NULL;
	}
	CChar *mChar = s->CurrcharObj();
	CItem *x = getPack( mChar );
	if( x == NULL ) 
		return NULL;
	for( CItem *i = x->FirstItemObj(); !x->FinishedItems(); i = x->NextItemObj() )
	{
		if( i != NULL )
		{
			switch( i->GetID() )
			{
			case 0x0E9C:
			case 0x0E9D:
			case 0x0E9E:
			case 0x0EB1:
			case 0x0EB2:
			case 0x0EB3:
			case 0x0EB4:
				return i;
			default:
				break;
			}
		}
	}
	return NULL;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::ProvocationTarget1( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check 1st Provoc Target and Add its ID to the socket, then
//|					 bring up a targeting cursor for player to select second
//|					 Provoc Target
//o---------------------------------------------------------------------------o
void cSkills::ProvocationTarget1( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::ProvocationTarget1() Invalid socket" );
		return;
	}
	CChar *trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( trgChar == NULL )
		return;
	CItem *getInst = GetInstrument( s );
	if( getInst == NULL ) 
	{
		sysmessage( s, 1438 );
		return;
	}
	
	if( trgChar->IsInvulnerable() || trgChar->GetNPCAiType() == 17 || trgChar->GetNPCAiType() == 4 ) // not invul, a player vendor, or a guard
	{
		sysmessage( s, "You cannot provoke such a person!" );
		return;
	}
	
	if( !trgChar->IsNpc() )
		sysmessage( s, 1442 );
	else
	{
		s->AddID( s->GetDWord( 7 ) );
		target( s, 0, 1, 0, 80, 1443 );
		PlayInstrumentWell( s, getInst );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::EnticementTarget1( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check 1st Entice Target and Add its ID to the socket, then
//|					 bring up a targeting cursor for player to select second
//|					 Entice Target
//o---------------------------------------------------------------------------o
void cSkills::EnticementTarget1( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::EnticementTarget1() Invalid socket" );
		return;
	}
	SERIAL serial = s->GetDWord( 7 );
	CChar *trgChar = calcCharObjFromSer( serial );
	if( trgChar == NULL )
		return;
	CItem *getInst = GetInstrument( s );
	if( getInst == NULL ) 
	{
		sysmessage( s, 1444 );
		return;
	}
	if( !trgChar->IsNpc() )
		sysmessage( s, 1445 );
	else if( trgChar->GetNPCAiType() == 17 || trgChar->IsInvulnerable() || trgChar->GetNPCAiType() == 4 )	// no PV, guard, or invul person
		sysmessage( s, 1642 );
	else
	{
		s->AddID( serial );
		target( s, 0, 1, 0, 82, 1446 );
		PlayInstrumentWell( s, getInst );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::EnticementTarget2( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check 2nd Entice Target and players Entice and Musicianship
//|					 skill then if checks pass flag the player criminal
//o---------------------------------------------------------------------------o
void cSkills::EnticementTarget2( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::EnticementTarget2() Invalid socket" );
		return;
	}
	CChar *trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( trgChar == NULL )
		return;
	if( trgChar->GetNPCAiType() == 17 || trgChar->IsInvulnerable() || trgChar->GetNPCAiType() == 4 )
	{
		sysmessage( s, 1642 );
		return;
	}
	CItem *getInst = GetInstrument( s );
	if( getInst == NULL ) 
	{
		sysmessage( s, 1438 );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	bool checkSkill1 = CheckSkill( mChar, ENTICEMENT, 0, 1000 );
	bool checkSkill2 = CheckSkill( mChar, MUSICIANSHIP, 0, 1000 );
	if( checkSkill1 && checkSkill2 )
	{
		criminal( mChar );
		CChar *target = calcCharObjFromSer( s->AddID() );
		target->SetFTarg( calcCharFromSer( trgChar->GetSerial() ) );
		target->SetNpcWander( 1 );
		sysmessage( s, 1447 );
		PlayInstrumentWell( s, getInst );
	}
	else 
	{
		sysmessage( s, 1448 );
		PlayInstrumentPoor( s, getInst );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::ProvocationTarget2( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check 2nd Provoc Target and players Provoc and Musicianship
//|					 skill.  If either target is Innocent, flag the player criminal
//o---------------------------------------------------------------------------o
void cSkills::ProvocationTarget2( cSocket *s )
{
	char temp[1024];
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::ProvocationTarget2() Invalid socket" );
		return;
	}
	CChar *trgChar = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( trgChar == NULL )
		return;
	
	CItem *getInst = GetInstrument( s );
	if( getInst == NULL ) 
	{
		sysmessage( s, 1438 );
		return;
	}

	if( trgChar->IsInvulnerable() || trgChar->GetNPCAiType() == 17 || trgChar->GetNPCAiType() == 4 ) // not invul, a player vendor, or a guard
	{
		sysmessage( s, "You cannot provoke such a person!" );
		return;
	}

	CChar *target =  calcCharObjFromSer( s->AddID() );
	if( target == trgChar )
	{
		sysmessage( s, 1449 );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	bool checkMusic = CheckSkill( mChar, MUSICIANSHIP, 0, 1000 );
	if( checkMusic )
	{
		bool checkProvoc = CheckSkill( mChar, PROVOCATION, 0, 1000 );
		PlayInstrumentWell( s, getInst );
		if( checkProvoc )
		{
			if( trgChar->IsInnocent() )
				criminal( mChar );
			sysmessage( s, 1450 );
		}
		else 
		{
			sysmessage( s, 1451 );
			trgChar = mChar;
		}

		target->SetTarg( calcCharFromSer( trgChar->GetSerial() ) );
		trgChar->SetTarg( calcCharFromSer( target->GetSerial() ) );
		if( target->GetHidden() && !( target->IsPermHidden() ) )
			target->ExposeToView();
		if( target->GetMed() )
			target->SetMed( 0 );
		if( trgChar->GetHidden() && !trgChar->IsPermHidden() )
			trgChar->ExposeToView();
		if( trgChar->GetMed() )
			trgChar->SetMed( 0 );
		target->SetAttackFirst( true );
		trgChar->SetAttackFirst( false );
		target->SetAttacker( calcCharFromSer( trgChar->GetSerial() ) );
		trgChar->SetAttacker( calcCharFromSer( target->GetSerial() ) );
		if( target->IsNpc() )
		{
			if( !target->IsAtWar() ) 
				npcToggleCombat( target );
			target->SetNpcMoveTime( BuildTimeValue( cwmWorldState->ServerData()->GetNPCSpeed() ) );
		}
		if( trgChar->IsNpc() )
		{
			if( !trgChar->IsAtWar() ) 
				npcToggleCombat( trgChar );
			trgChar->SetNpcMoveTime( BuildTimeValue( cwmWorldState->ServerData()->GetNPCSpeed() ) );
		}
		sprintf( temp, "* You see %s attacking %s *", target->GetName(), trgChar->GetName() );
		Network->PushConn();
		for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
		{
			if( charInRange( tSock->CurrcharObj(), target ) )
				itemmessage( tSock, temp, (*target) );
		}
		Network->PopConn();
	}
	else
	{
		PlayInstrumentPoor(s, getInst );
		sysmessage( s, 1452 );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::AlchemyTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets a reagent with a mortar, then
//|					 brings up a MakeMenu for player to select which potion
//|					 to make
//o---------------------------------------------------------------------------o
void cSkills::AlchemyTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::AlchemyTarget() Invalid socket" );
		return;
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		switch( i->GetID() )
		{
		case 0x0F7B:	NewMakeMenu( s, 90, ALCHEMY );	break;	// Agility,
		case 0x0F84:	NewMakeMenu( s, 91, ALCHEMY );	break;	// Cure, Garlic
		case 0x0F8C:	NewMakeMenu( s, 92, ALCHEMY );	break;	// Explosion, Sulfurous Ash
		case 0x0F85:	NewMakeMenu( s, 93, ALCHEMY );	break;	// Heal, Ginseng
		case 0x0F88:	NewMakeMenu( s, 94, ALCHEMY );	break;	// Poison, Nightshade
		case 0x0F7A:	NewMakeMenu( s, 95, ALCHEMY );	break;	// Refresh,
		case 0x0F86:	NewMakeMenu( s, 96, ALCHEMY );	break;	// Strength,
		case 0x0F8D:	NewMakeMenu( s, 97, ALCHEMY );	break;	// Night sight
		case 0x0E9B:									break; // Mortar
		default:
			if( i->GetID() >= 0x1B11 && i->GetID() <= 0x1B1C )
			{
				MakeNecroReg( s, i, i->GetID() );
				sysmessage( s, 1453 );
				return;
			}
			sysmessage( s, 1454 );
			return;
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::DoPotion( cSocket *s, int type, int sub, CItem *mortar )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Start *Name starts grinding reag in the mortar* emote based
//|					 on which potion was selected, and delete proper amount of reags
//o---------------------------------------------------------------------------o
void cSkills::DoPotion( cSocket *s, int type, int sub, CItem *mortar )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::DoPotion() Invalid socket making type %i with sub %i and mortar %i", type, sub, mortar->GetSerial() );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	char temp[1024], temp2[1024];
	switch( ( type<<8 ) + sub )
	{
	case 0x0101://agility
		if( getAmount( mChar, 0x0F7B) >= 1 )
		{
			strcpy( temp, "blood moss" );
			deleQuan( mChar, 0x0F7B, 1 );
		}
		break;
	case 0x0102://greater agility
		if( getAmount( mChar, 0x0F7B ) >= 3 )
		{
			strcpy( temp, "blood moss" );
			deleQuan( mChar, 0x0F7B, 3);
		}
		break;
	case 0x0201://lesser cure
		if( getAmount( mChar, 0x0F84) >= 1 )
		{
			strcpy( temp, "garlic" );
			deleQuan( mChar, 0x0F84, 1);
		}
		break;
	case 0x0202://cure
		if( getAmount( mChar, 0x0F84 ) >= 3 )
		{
			strcpy( temp, "garlic" );
			deleQuan( mChar, 0x0F84, 3 );
		}
		break;
	case 0x0203://greater cure
		if( getAmount( mChar, 0x0F84 ) >= 6 )
		{
			strcpy( temp, "garlic" );
			deleQuan( mChar, 0x0F84, 6 );
		}
		break;
	case 0x0301://lesser explosion
		if( getAmount( mChar, 0x0F8C ) >= 3 )
		{
			strcpy( temp, "sulfurous ash" );
			deleQuan( mChar, 0x0F8C, 3 );
		}
		break;
	case 0x0302://explosion
		if( getAmount( mChar, 0x0F8C ) >= 5 )
		{
			strcpy( temp, "sulfurous ash" );
			deleQuan( mChar, 0x0F8C, 5 );
		}
		break;
	case 0x0303://greater explosion
		if( getAmount( mChar, 0x0F8C ) >= 10 )
		{
			strcpy( temp, "sulfurous ash" );
			deleQuan( mChar, 0x0F8C, 10 );
		}
		break;
	case 0x0401://lesser heal
		if( getAmount( mChar, 0x0F85 ) >= 1 )
		{
			strcpy( temp, "ginseng" );
			deleQuan( mChar, 0x0F85, 1 );
		}
		break;
	case 0x0402://heal
		if( getAmount( mChar, 0x0F85 ) >= 3 )
		{
			strcpy( temp, "ginseng" );
			deleQuan( mChar, 0x0F85, 3 );
		}
		break;
	case 0x0403://greater heal
		if( getAmount( mChar, 0x0F85 ) >= 7 )
		{
			strcpy( temp, "ginseng" );
			deleQuan( mChar, 0x0F85, 7 );
		}
		break;
	case 0x0501://night sight
		if( getAmount( mChar, 0x0F8D ) >= 1 )
		{
			strcpy( temp, "spider's silk" );
			deleQuan( mChar, 0x0F8D, 1 );
		}
		break;
	case 0x0601://lesser poison
		if( getAmount( mChar, 0x0F88 ) >= 1 )
		{
			strcpy( temp, "nightshade" );
			deleQuan( mChar, 0x0F88, 1 );
		}
		break;
	case 0x0602://poison
		if( getAmount( mChar, 0x0F88 ) >= 2 )
		{
			strcpy( temp, "nightshade" );
			deleQuan( mChar, 0x0F88, 2 );
		}
		break;
	case 0x0603://greater poison
		if( getAmount( mChar, 0x0F88 ) >= 4 )
		{
			strcpy( temp, "nightshade" );
			deleQuan( mChar, 0x0F88, 4 );
		}
		break;
	case 0x0604://deadly poison
		if( getAmount( mChar, 0x0F88 ) >= 8 )
		{
			strcpy( temp, "nightshade" );
			deleQuan(mChar, 0x0F88, 8);
		}
		break;
	case 0x0701://refresh
		if( getAmount(mChar, 0x0F7A) >= 1 )
		{
			strcpy( temp, "black pearl" );
			deleQuan(mChar, 0x0F7A, 1);
		}
		break;
	case 0x0702://total refreshment
		if( getAmount(mChar, 0x0F7A) >= 5 )
		{
			strcpy( temp, "black pearl" );
			deleQuan(mChar, 0x0F7A, 5);
		}
		break;
	case 0x0801://strength
		if( getAmount(mChar, 0x0F86) >= 2 )
		{
			strcpy( temp, "mandrake" );
			deleQuan(mChar, 0x0F86, 2);
		}
		break;
	case 0x0802://greater strength
		if( getAmount(mChar, 0x0F86) >= 5 )
		{
			strcpy( temp, "mandrake" );
			deleQuan(mChar, 0x0F86, 5);
		}
		break;
	default:
		return;
	}
	if( strlen( temp ) != 0 )
	{
		sprintf( temp2, Dictionary->GetEntry( 1455 ), mChar->GetName(), temp );
		npcEmoteAll( mChar, temp2, true );
		tempeffect( mChar, mChar, 9, 0, 0, 0 );
		tempeffect( mChar, mChar, 9, 0, 3, 0 );
		tempeffect( mChar, mChar, 9, 0, 6, 0 );
		tempeffect( mChar, mChar, 9, 0, 9, 0 );
		tempeffect( mChar, mortar, 10, type, sub, 0 );
	}
	else
		sysmessage( s, 1463 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::CreatePotion( CChar *s, char type, char sub, CItem *mortar )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Create potion based on the type of potion selected
//o---------------------------------------------------------------------------o
void cSkills::CreatePotion( CChar *s, char type, char sub, CItem *mortar )
{
	bool canMake = false;
	cSocket *mSock = calcSocketObjFromChar( s );
	char temp[1024];

	switch( (type<<8) + sub )
	{
	case 0x0101:	canMake = CheckSkill( s, ALCHEMY, 151, 651 );	break;	// agility
	case 0x0102:	canMake = CheckSkill( s, ALCHEMY, 351, 851 );	break;	// greater agility
	case 0x0201:	canMake = CheckSkill( s, ALCHEMY, 0, 500 );		break;	// lesser cure
	case 0x0202:	canMake = CheckSkill( s, ALCHEMY, 251, 751 );	break;	// cure
	case 0x0203:	canMake = CheckSkill( s, ALCHEMY, 651, 1151 );	break;	// greater cure
	case 0x0301:	canMake = CheckSkill( s, ALCHEMY, 51, 551 );	break;	// lesser explosion
	case 0x0302:	canMake = CheckSkill( s, ALCHEMY, 351, 851 );	break;	// explosion
	case 0x0303:	canMake = CheckSkill( s, ALCHEMY, 651, 1151 );	break;	// greater explosion
	case 0x0401:	canMake = CheckSkill( s, ALCHEMY, 0, 500 );		break;	// lesser heal
	case 0x0402:	canMake = CheckSkill( s, ALCHEMY, 151, 651 );	break;	// heal
	case 0x0403:	canMake = CheckSkill( s, ALCHEMY, 551, 1051 );	break;	// greater heal
	case 0x0501:	canMake = CheckSkill( s, ALCHEMY, 0, 500 );		break;	// night sight
	case 0x0601:	canMake = CheckSkill( s, ALCHEMY, 0, 500 );		break;	// lesser poison
	case 0x0602:	canMake = CheckSkill( s, ALCHEMY, 151, 651 );	break;	// poison
	case 0x0603:	canMake = CheckSkill( s, ALCHEMY, 551, 1051 );	break;	// greater poison
	case 0x0604:	canMake = CheckSkill( s, ALCHEMY, 901, 1401 );	break;	// deadly poison
	case 0x0701:	canMake = CheckSkill( s, ALCHEMY, 0, 500 );		break;	// refresh
	case 0x0702:	canMake = CheckSkill( s, ALCHEMY, 251, 751 );	break;	// total refreshment
	case 0x0801:	canMake = CheckSkill( s, ALCHEMY, 251, 751 );	break;	// strength
	case 0x0802:	canMake = CheckSkill( s, ALCHEMY, 451, 951 );	break;	// greater strength
	default:		
		Console.Error( 2, " cSkills::CreatePotion -> Fallout of switch statement without default" );
		return;
	}
	if( !canMake && !s->IsGM() )
	{
		sprintf( temp, Dictionary->GetEntry( 1464 ), s->GetName());
		npcEmoteAll( s, temp, false );
		return;
	}
	mortar->SetType( 17 );
	mortar->SetMore( type, sub, mortar->GetMore( 3 ), mortar->GetMore( 4 ) );
	mortar->SetMoreX( s->GetSkill( ALCHEMY ) );
	
	if( !( getAmount( s, 0x0F0E ) >= 1 ) )
		target( mSock, 0, 1, 0, 109, 1465 );
	else
	{
		soundeffect( mSock, 0x0240, true );  // Pour potion sfx
		sprintf( temp, Dictionary->GetEntry( 1466 ), s->GetName() );
		npcEmoteAll( s, temp, false );
		deleQuan( s, 0x0F0E, 1 );
		PotionToBottle( s, mortar );
	} 
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::BottleTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when UOX can't find a bottle in players pack or
//|					 player uses a full mortar,
//o---------------------------------------------------------------------------o
void cSkills::BottleTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::BottleTarget() Invalid socket" );
		return;
	}

	char temp[1024];
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i != NULL )
	{
		if( i->GetID() == 0x0F0E )
		{
			decItemAmount( i );
			CItem *mortar = calcItemObjFromSer( s->AddID() );
			if( mortar == NULL ) 
				return;
			CChar *mChar = s->CurrcharObj();
			if( mortar->GetType() == 17 ) 
			{
				sprintf( temp, Dictionary->GetEntry( 1466 ), mChar->GetName() );
				npcEmoteAll( mChar, temp, false );
				PotionToBottle( mChar, mortar );
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::PotionToBottle( CChar *s, CItem *mortar )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Create the potion and use up a bottle in players pack
//o---------------------------------------------------------------------------o
void cSkills::PotionToBottle( CChar *s, CItem *mortar )
{
	CItem *c = Items->SpawnItem( calcSocketObjFromChar( s ), s, 1, "#", false, 0x0915, 0, true, false );
	if( c == NULL ) 
		return;
	c->SetType( 19 );
	c->SetMoreX( mortar->GetMoreX() );
	c->SetMoreY( mortar->GetMore( 1 ) );
	c->SetMoreZ( mortar->GetMore( 2 ) );
	c->SetWeight( 100 );
	
	switch( (mortar->GetMore( 1 )<<8) + mortar->GetMore( 2 ) )
	{
	case 0x0101:	c->SetName( Dictionary->GetEntry( 1467 ) );	c->SetID( 0x0F08 );	break;
	case 0x0102:	c->SetName( Dictionary->GetEntry( 1468 ) );	c->SetID( 0x0F08 );	break;
	case 0x0201:	c->SetName( Dictionary->GetEntry( 1469 ) );	c->SetID( 0x0F07 );	break;
	case 0x0202:	c->SetName( Dictionary->GetEntry( 1470 ) );	c->SetID( 0x0F07 );	break;
	case 0x0203:	c->SetName( Dictionary->GetEntry( 1471 ) );	c->SetID( 0x0F07 );	break;
	case 0x0301:	c->SetName( Dictionary->GetEntry( 1472 ) );	c->SetID( 0x0F0D );	break;
	case 0x0302:	c->SetName( Dictionary->GetEntry( 1473 ) );	c->SetID( 0x0F0D );	break;
	case 0x0303:	c->SetName( Dictionary->GetEntry( 1474 ) );	c->SetID( 0x0F0D );	break;
	case 0x0401:	c->SetName( Dictionary->GetEntry( 1475 ) );	c->SetID( 0x0F0C );	break;
	case 0x0402:	c->SetName( Dictionary->GetEntry( 1476 ) );	c->SetID( 0x0F0C );	break;
	case 0x0403:	c->SetName( Dictionary->GetEntry( 1477 ) );	c->SetID( 0x0F0C );	break;
	case 0x0501:	c->SetName( Dictionary->GetEntry( 1478 ) );	c->SetID( 0x0F06 );	break;
	case 0x0601:	c->SetName( Dictionary->GetEntry( 1479 ) );	c->SetID( 0x0F0A );	break;
	case 0x0602:	c->SetName( Dictionary->GetEntry( 1480 ) );	c->SetID( 0x0F0A );	break;
	case 0x0603:	c->SetName( Dictionary->GetEntry( 1481 ) );	c->SetID( 0x0F0A );	break;
	case 0x0604:	c->SetName( Dictionary->GetEntry( 1482 ) );	c->SetID( 0x0F0A );	break;
	case 0x0701:	c->SetName( Dictionary->GetEntry( 1483 ) );	c->SetID( 0x0F0B );	break;
	case 0x0702:	c->SetName( Dictionary->GetEntry( 1484 ) );	c->SetID( 0x0F0B );	break;
	case 0x0801:	c->SetName( Dictionary->GetEntry( 1485 ) );	c->SetID( 0x0F09 );	break;
	case 0x0802:	c->SetName( Dictionary->GetEntry( 1486 ) );	c->SetID( 0x0F09 );	break;
	default:		
		Console.Error( 2, " cSkills::PotionToBottle -> Fallout of switch statement without default" );
		return;
	}
 
	Items->GetScriptItemSetting( c );
	if( !s->IsGM() )
	{
		c->SetCreator( s->GetSerial() );
		if( s->GetSkill( ALCHEMY ) > 950 ) 
			c->SetMadeWith( ALCHEMY+1 );
		else 
			c->SetMadeWith( 0-ALCHEMY-1 );
	}
	else
	{
		c->SetCreator( INVALIDSERIAL );
		c->SetMadeWith( 0 );
	}
	RefreshItem( c );
	mortar->SetType( 0 );
	return;
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool cSkills::CheckSkill( CChar *s, int sk, int lowSkill, int highSkill )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Used to check a players skill based on the highskill and
//|					 lowskill it was called with.  If skill is < than lowskill
//|					 check will fail, but player will gain in the skill, if
//|					 the players skill is > than highskill player will not gain
//o---------------------------------------------------------------------------o
bool cSkills::CheckSkill( CChar *s, UI08 sk, int lowSkill, int highSkill )
{
	bool skillCheck = false;

	cScript *tScript = NULL;
	UI16 scpNum = s->GetScriptTrigger();
	tScript = Trigger->GetScript( scpNum );
	bool exists = false;

	if( tScript != NULL )
		exists = tScript->OnSkillCheck( s, sk, lowSkill, highSkill );	
	
	// o----------------------------------------------------------------------------o
	// | Programmer:sereg, 15 March, 2002											|
	// | Comment   :Now lets make this more readable, and even more mathematical:)	|
	// o----------------------------------------------------------------------------o
	// | 1. if a player has skill > highskill, checkskill should return true		|
	// | 2. if a player has skill < lowskill, checkskill returns false				|
	// | 3. we have statmodifiers defined in skills.dfn, now lets make it influence	|
	// |	checkskill with a certain additional bonus through the players stat.	|
	// | 4. we dont need to throw the dices for skillgain... we must do this in		|
	// |	AdvanceSkill(...), so just call it after calculation					|
	// o----------------------------------------------------------------------------o
	// | how to calculate if a skill succeeds?										|
	// | 1. we have a certain range between lowskill and highskill, on lowskill the	|
	// |	chance is 0%, on highskill 100%.										|
	// | 2. get the statmodifiers and the players stat. make 3 special dices for	|
	// |	each stat. make them a small bonus for skillsuccess						|
	// | 3. execute the 4 calculated dices one after the other by adding them		|
	// o----------------------------------------------------------------------------o

	if( !exists )
	{
		SI32 chanceskillsuccess = 0;
		
		if( ( ( highSkill - lowSkill ) <= 0 ) ||
			( s == NULL ) ||
			( s->GetSkill( sk ) <= lowSkill ) )
			return false;

		if( s->IsDead() )
		{
			sysmessage( calcSocketObjFromChar( s ), 1487 );
			return false;
		}

		if( ( s->GetCommandLevel() > 0 ) ||
			( s->GetSkill( sk ) >= highSkill ) )
			return true;



		chanceskillsuccess = (SI32)( (R32)( ( (R32)( s->GetSkill( sk ) - lowSkill ) / 1000.0f ) +
									 (R32)( (R32)( s->GetStrength() * skill[sk].strength ) / 100000.0f ) +
									 (R32)( (R32)( s->GetDexterity() * skill[sk].dexterity ) / 100000.0f ) +
									 (R32)( (R32)( s->GetIntelligence() * skill[sk].intelligence  ) / 100000.0f ) ) * 1000 );
		
		// chanceskillsuccess is a number between 0 and 1000, lets throw the dices now
		skillCheck = ( chanceskillsuccess >= RandomNum( 0, 1000 ) );
		
		cSocket *mSock = calcSocketObjFromChar( s );
		bool mageryUp = true;
		if( mSock != NULL )
		{
			mageryUp = ( mSock->CurrentSpellType() == 0 );
			
			if( s->GetBaseSkill( sk ) < highSkill )
			{
				CChar *targChar = calcCharObjFromSer( s->GetSerial() );
				if( sk != MAGERY || ( sk == MAGERY && mageryUp ) )
				{
					if( AdvanceSkill( targChar, sk, skillCheck ) )
					{
						updateSkillLevel( targChar, sk ); 
						updateskill( mSock, sk );
					}
				}
			}
		}
	}
	else
		skillCheck = true;
	return skillCheck;
}          

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Atrophy( CChar *c, UI16 sk )
//|   Date        :  Jan 29, 2000
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Do atrophy for player c:
//|						find sk in our cronological list of atrophy skills, 
//|						move it to the front, check total aginst skillcap to 
//|						see if we need to lower a skill, if we do, again search 
//|						skills for a skill that can be lowered, if one is found
//|						lower it and increase sk, if we can't find one, do 
//|						nothing if atrophy is not need, increase sk.
//o---------------------------------------------------------------------------o
void cSkills::Atrophy( CChar *c, UI08 sk )
{
	UI32 a=0, ttl = 0, rem=0;
	UI16 atrop[ALLSKILLS+1];
	SI16 toDec = -1;
	UI08 counter = 0;
	cSocket *mSock = calcSocketObjFromChar( c );
	UI16 skillTrig = c->GetScriptTrigger();
	cScript *scpSkill = Trigger->GetScript( skillTrig );
		
	if( c->IsNpc() || c->GetCommandLevel() >= CNSCMDLEVEL || mSock == NULL )	// GM's and NPC's dont atrophy
	{
		c->SetBaseSkill( c->GetBaseSkill( sk ) + 1, sk );
		if( scpSkill != NULL )
		{
			if( !scpSkill->OnSkillGain( c, sk ) )
				scpSkill->OnSkillChange( c, sk );
		}
		if( mSock != NULL )
			updateskill( mSock, sk );
		return;
	}

	srand( getclock() ); // Randomize
	
	atrop[ALLSKILLS]=0;//set the last of out copy array
	for( counter = 0; counter <= ALLSKILLS; counter++ )
	{
		atrop[counter] = c->GetAtrophy( counter );
	}
	
	for( a = TRUESKILLS; a > 0; a-- )
	{//add up skills and find the one being increased
		if( c->GetBaseSkill( c->GetAtrophy( a-1 ) )>0 && c->GetSkillLock( c->GetAtrophy( a-1) ) == 1 && c->GetAtrophy( a-1 ) != sk)
			toDec = c->GetAtrophy(a-1);//we found a skill that can be decreased, save it for later.

		ttl += c->GetBaseSkill( a-1 );
		atrop[a]=atrop[a-1];
		if( atrop[a] == sk )
			rem = a;//remember this number
	}

	atrop[0] = sk;//set the first one to our current skill
	
	//copy it back in
	if( rem == ALLSKILLS )//it was last
	{
		for( counter = 0; counter <= ALLSKILLS; counter++ )
			c->SetAtrophy( atrop[counter], counter );
	} 
	else	//in the middle somewhere or first
	{
		for( counter = 0; counter < rem; counter++ )
			c->SetAtrophy( atrop[counter], counter );
		for( counter = rem + 1; counter <= ALLSKILLS; counter++ )
			c->SetAtrophy( atrop[counter], counter );
	}

	if( RandomNum( 0, cwmWorldState->ServerData()->GetServerSkillCapStatus() ) <= ttl )
	{//if the rand is less than their total skills, they loose one.
		if( toDec != -1 )
		{
			c->SetBaseSkill( c->GetBaseSkill( toDec ) - 1, toDec );
			c->SetBaseSkill( c->GetBaseSkill( sk ) + 1, sk );
			if( scpSkill != NULL )
			{
				if( !scpSkill->OnSkillGain( c, sk ) )
					scpSkill->OnSkillChange( c, sk );
				if( !scpSkill->OnSkillLoss( c, toDec ) )
					scpSkill->OnSkillChange( c, toDec );
			}
			updateskill( mSock, sk );
			updateskill( mSock, toDec );
		}
		return;
	} 

	c->SetBaseSkill( c->GetBaseSkill( sk ) + 1, sk );
	if( scpSkill != NULL )
	{
		if( !scpSkill->OnSkillGain( c, sk ) )
			scpSkill->OnSkillChange( c, sk );
	}
	updateskill( mSock, sk );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::CreateBandageTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player cuts cloth into bandages
//o---------------------------------------------------------------------------o
void cSkills::CreateBandageTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::CreateBandageTarget() Invalid socket" );
		return;
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
	{
		sysmessage( s, 1491 );
		return;
	}

	CItem *c;

	// Gain 50 Folded cloth
	// And dont create bandages
	// automatically !?!?
	UI16 myID = i->GetID();

	if( myID >= 0x0F95 && myID <= 0x0F9C )
	{
		c = Items->SpawnItem( s, 50, Dictionary->GetEntry( 1490 ), true, 0x175F, 0, true, true );
		if( c == NULL ) 
			return;

		decItemAmount( i );

		return;
	}
	
	// Create RollDice 1d4 Bandages per cutted cloth
	if( ( myID >= 0x1515 && myID <= 1518 ) || ( myID >= 0x152E && myID <= 0x1531 ) || ( myID >= 0x1537 && myID <= 0x1544 ) || ( myID >= 0x1914 && myID <= 0x1915 ) || ( myID >= 0x1EFD && myID <= 0x1F04 ) )
	{
		cDice *myDice = new cDice( "1d4+1" );
		UI32 Amount = (UI32)( myDice->roll() );
		delete myDice;

		CChar *myChar = myChar = s->CurrcharObj();

		if( myChar == NULL )
			return;

		if( i->GetCont() == myChar->GetSerial() )
		{
			sysmessage( s, "You cannot cut equipped cloth" );
			return;
		}

		c = Items->SpawnItem( s, Amount, Dictionary->GetEntry( 1489 ), true, 0x0E21, 0, true, true );

		if( c == NULL ) 
			return;

		decItemAmount( i );

		return;
	}

	// Gain 2 bandages per Folded Cloth
	if( i->GetID() >= 0x175D && i->GetID() <= 0x1764 )
	{
		soundeffect( s, 0x0248, true );
		sysmessage( s, 1488 );
		c = Items->SpawnItem( s, 2, Dictionary->GetEntry( 1489 ), true, 0x0E21, 0, true, true );
		if( c == NULL ) 
			return;
		decItemAmount( i );

		return;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::HealingSkillTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets Human NPC or other Players with
//|					 bandages.  Checks both Healing and Anatomy for success,
//|					 players with > 60 in both can cure poison (sometimes, 
//|					 dependant on poison level), players with > 80 in both can
//|					 resurrect players (once again, sometimes)
//o---------------------------------------------------------------------------o
void cSkills::HealingSkillTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::HealingSkillTarget() Invalid socket" );
		return;
	}
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar = s->CurrcharObj();
	ITEM mItem = s->AddMItem();
	if( i != NULL )
	{
		if( !charInRange( s, i, 1 ) )
		{
			sysmessage( s, 1498 );
			return;
		}
		if( i->IsDead() )
		{
			if( mChar->GetBaseSkill( HEALING ) >= 800 && mChar->GetBaseSkill( ANATOMY ) >= 800 )
			{
				if( i->IsMurderer() )
					criminal( mChar );
				if( !CheckSkill( mChar, HEALING, 800, 1000 ) || !CheckSkill( mChar, ANATOMY, 800, 1000 ) )	// failed either healing or anat check
				{
					sysmessage( s, 1492 );
					decItemAmount( mItem );
					return;
				}
				tempeffect( mChar, i, 23, HEALING, 0, 0, mItem );	// sets up timer for resurrect
				return;
			}
			sysmessage( s, 1493 );
			return;
		}

		if( i->GetPoisoned() > 0 )
		{
			if( mChar->GetBaseSkill( HEALING ) >= 600 && mChar->GetBaseSkill( ANATOMY ) >= 600 )
			{
				if( i->IsMurderer() )
					criminal( mChar );
				if( !CheckSkill( mChar, HEALING, 600, 1000 ) || !CheckSkill( mChar, ANATOMY, 600, 1000 ) )
				{
					sysmessage( s, 1494 );
					decItemAmount( mItem );
					return;
				}
				tempeffect( mChar, i, 24, HEALING, 0, 0, mItem );
				return;
			}
			else
			{
				sysmessage( s, 1495 );
				sysmessage( s, 1496 );
			}
			return;
		}

		if( i->GetHP() == i->GetStrength() )
		{
			sysmessage( s, 1497 );
			return;
		}

		UI08 targetSkill;
		if( i->GetID() == 0x0190 || i->GetID() == 0x0191 ) //Used on human
			targetSkill = HEALING;
		else
			targetSkill = VETERINARY;

		if( i->IsMurderer() )
			criminal( mChar );
		if( !CheckSkill( mChar, targetSkill, 0, 1000 ) )
		{
			if( targetSkill == HEALING )
			{
				i->IncHP( 1 );
				sysmessage( s, 1499 );
			}
			else
				sysmessage( s, 1500 );
			return;
		}
		tempeffect( mChar, i, 22, targetSkill, 0, 0, mItem );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::SpiritSpeak( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses spirit speak skill.. Translates a
//|					 ghosts normal oOoOooo text into readable text (what he
//|					 is actually saying), Operates on a timer based on settings
//|					 in server.scp
//o---------------------------------------------------------------------------o
void cSkills::SpiritSpeak( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::SpiritSpeak() Invalid socket" );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	if( !CheckSkill( mChar, SPIRITSPEAK, 0, 1000 ) )
	{
		sysmessage( s, 1501 );
		return;
	}
	
	impaction( s,0x11 );
	soundeffect( s, 0x024A, true );
	sysmessage( s, 1502 );
	
	mChar->SetSpiritSpeakTimer( BuildTimeValue( cwmWorldState->ServerData()->GetSpiritSpeakTimer() + mChar->GetSkill( SPIRITSPEAK ) / 10 + mChar->GetIntelligence() ) ); // spirit speak duration
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::ArmsLoreTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses ArmsLore on an item (Only works for
//|					 weapons and armor (Items with Defense or HiDamage/LoDamage
//o---------------------------------------------------------------------------o
void cSkills::ArmsLoreTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::ArmsLoreTarget() Invalid socket" );
		return;
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;
	SI32 offset;
	char temp2[60], temp[1024];
	CChar *mChar = s->CurrcharObj();
	if( i->isPileable() || ( i->GetDef() == 0 && ( i->GetLoDamage() == 0 && 
		i->GetHiDamage() == 0 ) && ( i->GetRank() < 1 || i->GetRank() > 9 ) ) )
	{
		sysmessage( s, 1503 );
		return;
	}
	if( mChar->IsGM() )
	{
		sprintf( temp, "Defense [%i] LoDamage [%i] HiDamage [%i], Poison [%i], Rank [%i]", i->GetDef(), i->GetLoDamage(), i->GetHiDamage(), i->GetPoisoned(), i->GetRank() );
		sysmessage( s, temp );
		return;
	}
	if( CheckSkill( mChar, ARMSLORE, 0, 1000 ) )
	{
		if( i->GetMaxHP() )
		{
			strcpy( temp, "This item " );
			// Items HP's - 1 / by items total HP * 10 (0-3 = 0, 4 - 5 = 1, ect)
			offset = static_cast<SI32>(((R32)(( i->GetHP() - 1) / i->GetMaxHP() ) * 10 ));
			if( offset >= 0 && offset <= 8 )
				strcpy( temp2, Dictionary->GetEntry( 1515 - offset ) );
			else
				strcpy( temp2, Dictionary->GetEntry( 1506 ) );

			strcat(temp,temp2);
		}
		else
			sysmessage( s, 1505 );
		if( i->GetHiDamage() )
		{
			if( mChar->GetSkill( ARMSLORE ) > 750 && i->GetPoisoned() > 0 )
			{
				offset = i->GetPoisoned();
				if( offset > 0 && offset < 5 )
					strcpy( temp2, Dictionary->GetEntry( 1455 + offset ) );
				else
					strcpy( temp2, Dictionary->GetEntry( 1459 ) );
				strcat( temp, temp2 );
			}
			// HiDamage + LoDamage / 10 ( 0-9 = 0, 10-19 = 1, ect )
			offset = (i->GetHiDamage() + i->GetLoDamage() ) / 10;
			if( offset <= 5 )
				strcpy( temp2, Dictionary->GetEntry( 1522 - offset ) );
			else
				strcpy( temp2, Dictionary->GetEntry( 1516 ) );
			strcat( temp, temp2 );
			
			if( mChar->GetSkill( ARMSLORE ) > 250 )
			{
				// Items Speed - 5 / 10 ( 0-14 = 0, 15-25 = 1, ect)
				offset = ((i->GetSpeed() - 5) / 10);
				if( offset <= 2 )
					strcpy( temp2, Dictionary->GetEntry( 1526 - offset ) );
				else
					strcpy( temp2, Dictionary->GetEntry( 1523 ) );
				strcat( temp, temp2 );
			}
		}
		else if( i->GetDef() )
		{
			// Items Defense + 1 / 2 ( 0 = 0, 1-2 = 1, 3-4 = 2, ect)
			offset = ((i->GetDef() + 1) / 2);

			if( offset <= 6 )
				strcpy( temp2, Dictionary->GetEntry( 1534 - offset ) );
			else
				strcpy( temp2, Dictionary->GetEntry( 1527 ) );

			strcat( temp, temp2 );
		}
		sysmessage( s, temp );
		if( mChar->GetSkill( ARMSLORE ) > 250 && cwmWorldState->ServerData()->GetRankSystemStatus() )
		{
			offset = i->GetRank();
			if( offset >= 0 && offset <= 10 )
				sysmessage( s, 1534 + offset );
		}
	}
	else
		sysmessage( s, 1504 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::ItemIDTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses the ItemID skill on an item, can
//|					 give valuable information on items (Will reveal hidden
//|					 magical names and charges on items as well)
//o---------------------------------------------------------------------------o
void cSkills::ItemIDTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::ItemIdTarget() Invalid socket" );
		return;
	}
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;
	if( i->isCorpse() )
	{
		sysmessage( s, 1546 );
		return;
	}

	char temp[1024];

	CChar *mChar = s->CurrcharObj();
	if( CheckSkill( mChar, ITEMID, 250, 500 ) )
	{
		if( i->GetName2() && strcmp( i->GetName2(), "#" ) ) 
			i->SetName( i->GetName2() );
		if( i->GetName()[0] == '#') 
			getTileName( i, temp );
		else 
			strcpy( temp, i->GetName() );
		sysmessage( s, 1547, temp );
		
		if( i->GetCreator() != INVALIDSERIAL )
		{
			CChar *mCreater = calcCharObjFromSer( i->GetCreator() );
			if( mCreater != NULL )
			{
				if( i->GetMadeWith() > 0 ) 
					sprintf( temp, Dictionary->GetEntry( 1548 ), skill[i->GetMadeWith()-1].madeword, mCreater->GetName() );
				else if( i->GetMadeWith() < 0 )
					sprintf( temp, Dictionary->GetEntry( 1548 ), skill[0-i->GetMadeWith() - 1].madeword, mCreater->GetName() );
				else
					sprintf( temp, Dictionary->GetEntry( 1549 ), mCreater->GetName() );
		}
		else
			strcpy( temp, Dictionary->GetEntry( 1550 ) );
		}
		else
			strcpy( temp, Dictionary->GetEntry( 1550 ) );
		sysmessage( s, temp );
		
		if( mChar->GetSkill( ITEMID ) > 350 )
		{
			if( i->GetType() != 15 )
			{
				sysmessage( s, 1553 );
				return;
			}
			if( CheckSkill( mChar, ITEMID, 500, 750 ) )
			{
				UI16 spellToScan = ( 8 * ( i->GetMoreX() - 1 ) ) + i->GetMoreY() - 1;
				if( !CheckSkill( mChar, ITEMID, 750, 1000 ) )
					sysmessage( s, 1555, Dictionary->GetEntry( magic_table[spellToScan].spell_name ) );
				else
					sysmessage( s, 1556, Dictionary->GetEntry( magic_table[spellToScan].spell_name ), i->GetMoreZ() );
			}
			else
				sysmessage( s, 1554 );
		}
		else
			sysmessage( s, 1552 );
	}
	else
		sysmessage( s, 1545 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::EvaluateIntTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses EvalInt on a PC/NPC, gives
//|					 a text message based on their Intelligence
//o---------------------------------------------------------------------------o
void cSkills::EvaluateIntTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::EvaluateIntTarget() Invalid socket" );
		return;
	}
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;
	char buf[75];
	CChar *mChar = s->CurrcharObj();
	if( CheckSkill( mChar, EVALUATINGINTEL, 0, 1000 ) ) 
	{
		if( i->GetIntelligence() == 0 )
			sysmessage( s, 1557 );
		else
		{
			// Intelligence - 1 / 10 (0-10 = 0, 11-20 = 1, ect)
			int offset = ((i->GetIntelligence() - 1 ) / 10);

			if( offset >= 0 && offset <= 8 )
				strcpy( buf, Dictionary->GetEntry( 1558 + offset ) );
			else
				strcpy( buf, Dictionary->GetEntry( 1567 ) );

			sysmessage( s, 1568, buf );
		}
	}
	else
		sysmessage( s, 1504 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::AnatomyTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses Anatomy skill on a PC/NPC, gives
//|					 a text message based on their Strength and Dexterity
//o---------------------------------------------------------------------------o
void cSkills::AnatomyTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::AnatomyTarget() Invalid socket" );
		return;
	}
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
	{
		sysmessage( s, 1569 );
		return;
	}
	char buf[125];
	char buf2[125];
	CChar *mChar = s->CurrcharObj();
	if( getDist( i, mChar ) >= cwmWorldState->ServerData()->GetCombatMaxRange() )
	{
		sysmessage( s, 1570 );
		return;
	}
	
	if( i->IsDead() )
	{
		sysmessage( s, 1571 );
		return;
	}
	if( CheckSkill( mChar, ANATOMY, 0, 1000 ) ) 
	{
		int offset;

		// Strength - 1 / 10 (0-10 = 0, 11-20 = 1, ect)
		offset = ((i->GetStrength() - 1 )/ 10);

		if( offset >= 0 && offset <= 8 )
			strcpy( buf, Dictionary->GetEntry( 1572 + offset ) );
		else
			strcpy( buf, Dictionary->GetEntry( 1581 ) );


		// Dexterity - 1 /10 (0-10 = 0, 11-20 = 1, ect)
		offset = ((i->GetDexterity() - 1 )/ 10);

		if( offset >= 0 && offset <= 8 )
			strcpy( buf2, Dictionary->GetEntry( 1582 + offset ) );
		else
			strcpy( buf2, Dictionary->GetEntry( 1591 ) );

		sysmessage( s, 1592, buf, buf2 );
	}
	else
		sysmessage( s, 1504 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::TameTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//|									
//|	Modification	-	09/22/2002	-	Xuri - Changed sysmessage at end of taming 
//|									function to say "You are too far away" instead of 
//|									"You can't tame that!" Easier for the users to understand 
//|									what's wrong when they can't tame due to being too far away...
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player attempts to tame an NPC
//o---------------------------------------------------------------------------o
void cSkills::TameTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::TameTarget() Invalid socket" );
		return;
	}
	if( s->GetByte( 7 ) == 0xFF ) 
		return;
	
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;
	CChar *mChar = s->CurrcharObj();
	char temp[1024];
	if( i->IsNpc() && getDist( mChar, i ) <= 3 )
	{
		if( i->GetTaming()>1000 || i->GetTaming() == 0 )
		{
			sysmessage( s, 1593 );
			return;
		}
		if( i->IsTamed() && i->GetOwner() == mChar->GetSerial() )
		{
			sysmessage( s, 1594 );
			return;
		}
		if( i->IsTamed() )
		{
			sysmessage( s, 1595 );
			return;
		}
		sprintf( temp, Dictionary->GetEntry( 1596 ), mChar->GetName(), i->GetName() );
		for( UI08 a = 0; a < 3; a++ )
		{
			switch( RandomNum( 0, 3 ) )
			{
			case 0: npcTalkAll( mChar, 1597, false );	break;
			case 1: npcTalkAll( mChar, 1598, false );	break;
			case 2: 
				sprintf( temp, Dictionary->GetEntry( 1599 ), i->GetName() ); 
				npcTalkAll( mChar, temp, false ); 
				break;
			case 3: 
				sprintf( temp, Dictionary->GetEntry( 1600 ), i->GetName() ); 
				npcTalkAll( mChar, temp, false ); 
				break;
			}
		}
		if( i->GetHunger() < 0 || mChar->GetSkill( TAMING ) < i->GetTaming() || !CheckSkill( mChar, TAMING, 0, 1000 ) ) 
		{
			sysmessage( s, 1601 );
			return;
		}   
		npcTalk( s, mChar, 1602, false );
		i->SetOwner( mChar->GetSerial() );
		i->SetNpcWander( 0 );
		i->SetNPCAiType( 0 );
		i->SetTamed( true );
		if( i->IsAtWar() && &chars[i->GetTarg()] == mChar )
			npcToggleCombat( i );
	}
	else
	{
		// Sept 22, 2002 - Xuri
		sysmessage( s, 400 );
		//
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::FishTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets an area with a fishing pole
//o---------------------------------------------------------------------------o
void cSkills::FishTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::FishTarget() Invalid socket" );
		return;
	}
	if( s->GetDWord( 11 ) == INVALIDSERIAL )
		return;

	CChar *mChar = s->CurrcharObj();

	SI16 targetX = s->GetWord( 0x0B );
	SI16 targetY = s->GetWord( 0x0D );
	SI08 targetZ = s->GetByte( 0x10 );
	SI16 distX = abs( mChar->GetX() - targetX );
	SI16 distY = abs( mChar->GetY() - targetY );

	UI08 targetID1 = s->GetByte( 0x11 );
	UI08 targetID2 = s->GetByte( 0x12 );

	CItem *targetItem = calcItemObjFromSer( s->GetDWord( 7 ) );
	bool validLocation = false;
	if( targetItem != NULL )
		validLocation = Map->IsTileWet( targetItem->GetID() );
	else if( targetID1 != 0 && targetID2 != 0 )
	{
		MapStaticIterator msi( targetX, targetY, mChar->WorldNumber() );
		CTile tile;
		staticrecord *stat = NULL;
		while( ( ( stat = msi.Next() ) != NULL ) && !validLocation )
		{
			msi.GetTile(&tile);
			if( targetZ == stat->zoff && tile.LiquidWet() )	// right place, and wet
				validLocation = true;
		}
	}
	else		// or it could be a map only
	{  // manually calculating the ID's if a maptype
		map_st map1;
		CLand land;
		map1 = Map->SeekMap0( targetX, targetY, mChar->WorldNumber() );
		Map->SeekLand( map1.id, &land );
		if( land.LiquidWet() )
			validLocation = true; 
	}
	if( validLocation )
	{
		if( distX > 5 || distY > 5 )
		{
			sysmessage( s, 843 );
			return;
		}
		if( mChar->GetZ() < targetZ )
		{
			sysmessage( s, 844 );
			return;
		}
		if( mChar->GetStamina() - 2 <= 2 )
		{
			sysmessage( s, 845 );
			return;
		}
		mChar->SetStamina( mChar->GetStamina() - 2 );
		action( s, 0x0b );
		R32 baseTime;
		baseTime = cwmWorldState->ServerData()->GetFishingBaseTime() / 25;
		baseTime += RandomNum( 0, cwmWorldState->ServerData()->GetFishingRandomTime() / 15 );
		mChar->SetFishingTimer( BuildTimeValue( baseTime ) ); //2x faster at war and can run
		soundeffect( s, 0x023F, true );
	}
	else
		sysmessage( s, 846 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Fish( CChar *i )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Fish up items based on skill and random values (this should
//|					 be rewritten using a resource system akin to Ore and Logs)
//o---------------------------------------------------------------------------o
void cSkills::Fish( CChar *i )
{
	cSocket *s = calcSocketObjFromChar( i );
	if( !CheckSkill( i, FISHING, 0, 1000 ) ) 
	{
		sysmessage( s, 847 );
		return;
	}
	UI16 getSkill = i->GetSkill( FISHING );
	switch( RandomNum( 0, 25 ) )
	{
	case 1:
		if( getSkill > 920 )
		{
			SpawnRandomItem( s, true, "necro.scp", "ITEMLIST", "1" );	// random paintings
			sysmessage( s, 848 );
		}
		break;
	case 2:
		if( getSkill > 970 )
		{
			SpawnRandomItem( s, true, "necro.scp", "ITEMLIST", "2" );	// Some new weapons
			sysmessage( s, 849 );
		}
		break;
	case 3:	// Random gold and gems
		if( RandomNum( 0, 12 ) )
		{
			SpawnRandomItem( s, true, "necro.scp", "ITEMLIST", "3" ); 
			sysmessage( s, 850 );
		}
		else
		{	// Create between 200 and 1300 gold
			int nAmount = RandomNum( 200, 1300 );
			addgold( s, nAmount );
			goldsfx( s, nAmount );
			sysmessage( s, 851, nAmount );
		}
		break;
	case 4:
		if( getSkill > 850 )
		{
			SpawnRandomItem( s, true, "necro.scp", "ITEMLIST", "4" );	// Random bones and crap
			sysmessage( s, 852 );
		}
		break;
	default:
		SpawnRandomItem( s, true, "necro.scp", "ITEMLIST", "5" );	// User defined fish
		sysmessage( s, 853 );
		break;
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::SkillUse( cSocket *s, int x )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses a skill from the skill list
//o---------------------------------------------------------------------------o
void cSkills::SkillUse( cSocket *s, int x )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::SkillUse() Invalid socket using skill %i", x );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	if( mChar->IsDead() )
	{
		sysmessage( s, 392 );
		return;
	}
	if( x != STEALTH && mChar->GetHidden() && !mChar->IsPermHidden() )
		mChar->ExposeToView();
	mChar->BreakConcentration( s );
	if( mChar->GetSpellTime() != 0 || ( mChar->GetCasting() == -1 || mChar->GetCasting() == 1 ) )
	{
		sysmessage( s, 854 );
		return;
	}
	if( mChar->GetSkillDelay() <= uiCurrentTime || mChar->IsGM() )
	{
		cScript *skScript = Trigger->GetScript( mChar->GetScriptTrigger() );
		bool doSwitch = true;
		if( skScript != NULL )
			doSwitch = !skScript->OnSkill( mChar, x );
		if( doSwitch )
		{
			switch( x )
			{
			case ARMSLORE:			target( s, 0, 1, 0, 29, 855 );		break;
			case ANATOMY:			target( s, 0, 1, 0, 37, 856 );		break;
			case ITEMID:			target( s, 0, 1, 0, 40, 857 );		break;
			case EVALUATINGINTEL:	target( s, 0, 1, 0, 41, 858 );		break;
			case TAMING:			target( s, 0, 1, 0, 42, 859 );		break;
			case HIDING:			Hide( s );	break;
			case STEALTH:			Stealth( s );														break;
			case DETECTINGHIDDEN:	target( s, 0, 1, 0, 77, 860 );		break;
			case PEACEMAKING:		PeaceMaking(s);														break;
			case PROVOCATION:		target(s, 0, 1, 0, 79, 861 );		break;
			case ENTICEMENT:		target(s, 0, 1, 0, 81, 862 );		break;
			case SPIRITSPEAK:		SpiritSpeak(s);														break;
			case STEALING:
				if( cwmWorldState->ServerData()->GetRogueStatus() )
					target( s, 0, 1, 0, 205, 863 );
				else
					sysmessage( s, 864 );
				break;
			case INSCRIPTION:		target( s, 0, 1, 0, 160, 865 );		break;
			case TRACKING:			TrackingMenu( s, TRACKINGMENUOFFSET );								break;
			case BEGGING:			target( s, 0, 1, 0, 152, 866 );		break;
			case ANIMALLORE:		target( s, 0, 1, 0, 153, 867 );		break;
			case FORENSICS:			target( s, 0, 1, 0, 154, 868 );		break;
			case POISONING:			target( s, 0, 1, 0, 155, 869 );		break;
			case MEDITATION:
				if( cwmWorldState->ServerData()->GetServerArmorAffectManaRegen() )
					Meditation( s );
				else 
					sysmessage( s, 870 );
				break;
			default:				sysmessage( s, 871 );				break;
			}
		}
		mChar->SetSkillDelay( BuildTimeValue( cwmWorldState->ServerData()->GetServerSkillDelayStatus() ) );
		return;
	}
	else
		sysmessage( s, 872 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::RandomSteal( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets a PC/NPC with stealing skill
//|					 instead of an item (randomly pics an item in their pack)
//o---------------------------------------------------------------------------o
void cSkills::RandomSteal( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::RandomSteal() Invalid socket" );
		return;
	}
	char temp2[512], temp[512];

	CChar *mChar = s->CurrcharObj();
	CChar *npc = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( npc == NULL ) 
		return;

	if( npc == mChar ) 
	{
		sysmessage( s, 873 );
		return;
	}
	if( npc->GetNPCAiType() == 17 )
	{
		sysmessage( s, 874 );
		return;
	}

	CItem *p = getPack( npc );
	if( p == NULL ) 
	{
		sysmessage( s, 875 ); 
		return; 
	}

	CItem *item = NULL;
	for( UI08 i = 0; i < 50; i++ )
	{
		item = p->GetItemObj( RandomNum( 0, p->NumItems() - 1 ) );
		if( item != NULL ) 
			break;
	} 
	if( item == NULL )
	{
		sysmessage( s, 876 );
		return;
	}
	CItem *itemCont = calcItemObjFromSer( item->GetCont() ); 
	if( itemCont != NULL && itemCont->GetLayer() >= 0x1A && itemCont->GetLayer() <= 0x1C ) // is it in the buy or sell layer of a vendor?
	{
		sysmessage( s, 874 );
		return;
	}
	sysmessage( s, 877, npc->GetName(), item->GetName() );
	if( charInRange( s, npc, 1 ) )
	{
		if( Combat->calcDef( mChar, 0, false ) > 40 )
		{
			sysmessage( s, 1643 );
			return;
		}
		SI16 stealCheck = calcStealDiff( mChar, item );
		if( stealCheck == -1 )
		{
			sysmessage( s, 1551 );
			return;
		}

		if( mChar->GetCommandLevel() < npc->GetCommandLevel() )
		{
			sysmessage( s, 878 );
			return;
		}
		if( item->isNewbie() )
		{
			sysmessage( s, 879 );
			return;
		}
		
		int getDefOffset = min( stealCheck + ( (int)( ( Combat->calcDef( mChar, 0, false ) - 1) / 10 ) * 100 ), 990 );
		bool canSteal = CheckSkill( mChar, STEALING, getDefOffset, 1000);
		if( canSteal )
		{
			CItem *pack = getPack( mChar );
			item->SetCont( pack->GetSerial() );
			sysmessage( s, 880 );
			RefreshItem( item );

			sendItemsInRange( s );

			UI16 targTrig = item->GetScriptTrigger();
			cScript *toExecute = Trigger->GetScript( targTrig );
			if( toExecute != NULL )
				toExecute->OnSteal( mChar, item );

			targTrig = npc->GetScriptTrigger();
			toExecute = Trigger->GetScript( targTrig );
			if( toExecute != NULL )
				toExecute->OnStolenFrom( mChar, npc, item );
		} 
		else 
			sysmessage( s, 881 );
		
		if( ( !canSteal && RandomNum( 1, 5 ) == 5 ) || mChar->GetSkill( STEALING ) < RandomNum( 0, 1000 ) )
		{//Did they get caught? (If they fail 1 in 5 chance, other wise their skill away from 1000 out of 1000 chance)
			sysmessage( s, 882 );
			if( npc->IsNpc() ) 
				npcTalkAll( npc, 883, false );
			
			if( npc->IsInnocent() && &chars[mChar->GetAttacker()] != npc && GuildSys->ResultInCriminal( mChar, npc ) && Races->Compare( mChar, npc ) == 0 )
			{
				criminal( mChar );
			}
			if( item->GetName()[0] != '#' )
			{
				sprintf( temp, Dictionary->GetEntry( 884 ), mChar->GetName(), item->GetName() );
				sprintf( temp2, Dictionary->GetEntry( 885 ), mChar->GetName(), item->GetName(), npc->GetName() );
			} 
			else 
			{
				CTile tile;
				Map->SeekTile( item->GetID(), &tile );
				sprintf( temp, Dictionary->GetEntry( 884 ), mChar->GetName(), tile.Name() );
				sprintf( temp2, Dictionary->GetEntry( 885 ), mChar->GetName(), tile.Name(), npc->GetName() );
			}

			cSocket *npcSock = calcSocketObjFromChar( npc );
			if( npcSock != NULL )
				sysmessage( npcSock, temp );

			Network->PushConn();
			for( cSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
			{
				CChar *iChar = iSock->CurrcharObj();
				if( iSock != s && charInRange( mChar, iChar ) && ( RandomNum( 10, 20 ) == 17 || ( RandomNum( 0, 1 ) == 1 && iChar->GetIntelligence() >= mChar->GetIntelligence() ) ) )
					sysmessage( s, temp2 );
			}
			Network->PopConn();
		}
	} 
	else 
		sysmessage( s, 886 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::StealingTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets an item with stealing skill
//o---------------------------------------------------------------------------o
void cSkills::StealingTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::StealingTarget() Invalid socket" );
		return;
	}
	ITEM b;
	CItem *x;
	bool canSteal;
	char temp2[512], temp[512];

	CChar *mChar = s->CurrcharObj();
	if( s->GetByte( 7 ) < 0x40 )
	{
		RandomSteal( s );
		return;
	}
	CItem *item = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( item == NULL )
		return;

	for( UI08 z = 0; z < 255; z++ )
	{
		x = calcItemObjFromSer( x->GetCont() );
		if( x == NULL )
			break;
		b = x->GetCont( 1 );
		if( b < 0x40 )
			break;
	}
	if( x == NULL )
		return;

	CChar *npc = calcCharObjFromSer( x->GetCont() );
	if( npc == NULL )
		return;

	x = item;
	if( item->GetCont() == npc->GetSerial() || item->GetCont() == INVALIDSERIAL || item->isNewbie() )
	{
		sysmessage( s, 874 );
		return;
	}

	if( npc == mChar )
	{
		sysmessage( s, 873 );
		return;
	}
	if( npc->GetNPCAiType() == 17 )
	{
		sysmessage( s, 874 );
		return;
	}

	CItem *itemCont = calcItemObjFromSer( item->GetCont() ); 
	if( itemCont != NULL && itemCont->GetLayer() >= 0x1A && itemCont->GetLayer() <= 0x1C ) // is it in the buy or sell layer of a vendor?
	{
		sysmessage( s, 874 );
		return;
	}

	if( charInRange( s, npc, 1 ) )
	{
		if( Combat->calcDef( mChar, 0, false ) > 40 )
		{
			sysmessage( s, 1643 );
			return;
		}
		SI16 stealCheck = calcStealDiff( mChar, item );
		if( stealCheck == -1 )
		{
			sysmessage( s, 1551 );
			return;
		}
		int getDefOffset = min( stealCheck + ( (int)( ( Combat->calcDef( mChar, 0, false ) - 1) / 10 ) * 100 ), 990 );
		canSteal = CheckSkill( mChar, STEALING, getDefOffset, 1000 );
		if( canSteal )
		{
			cScript *toExecute;
			CItem *pack = getPack( mChar );
			item->SetCont( pack->GetSerial() );
			sysmessage( s, 880 );
			RefreshItem( item );	// let's reuse some code that will cope with visibility
			UI16 targTrig = item->GetScriptTrigger();
			toExecute = Trigger->GetScript( targTrig );
			if( toExecute != NULL )
				toExecute->OnSteal( mChar, item );

			targTrig = npc->GetScriptTrigger();
			toExecute = Trigger->GetScript( targTrig );
			if( toExecute != NULL )
				toExecute->OnStolenFrom( mChar, npc, item );
		} 
		else 
			sysmessage( s, 881 );

		if( ( !canSteal && RandomNum( 0, 15 ) == 7 ) || ( mChar->GetSkill( STEALING ) < RandomNum( 0, 1001 ) ) ) //	0 to 1001, so ever a gm has a SMALL chance of being caught
		{
			sysmessage( s, 882 );

			if( npc->IsNpc() ) 
				npcTalkAll( npc, 883, false );

			if( npc->IsInnocent() && &chars[mChar->GetAttacker()] != npc && GuildSys->ResultInCriminal( mChar, npc ) == 0 && Races->Compare( mChar, npc ) == 0 )
			{
				criminal( mChar );//Blue and not attacker and not guild
			}
			char tileName[128];
			getTileName( item, tileName );
			sprintf( temp, Dictionary->GetEntry( 884 ), mChar->GetName(), tileName );
			sprintf( temp2, Dictionary->GetEntry( 885 ), mChar->GetName(), tileName, npc->GetName() );
			if( !npc->IsNpc() )
				sysmessage( calcSocketObjFromChar( npc ), temp );	// presuming that just perhaps you might be able to have a pc here
		}
	
		Network->PushConn();
		for( cSocket *iSock = Network->FirstSocket(); !Network->FinishedSockets(); iSock = Network->NextSocket() )
		{
			CChar *iChar = iSock->CurrcharObj();
			if( iSock != s && charInRange( mChar, iChar ) && ( RandomNum( 10, 20 ) == 17 || ( RandomNum( 0, 1 ) == 1 && iChar->GetIntelligence() >= mChar->GetIntelligence() - RandomNum( 0, 14 ) ) ) )
				sysmessage( iSock, temp2 );
		}
		Network->PopConn();
	} 
	else 
		sysmessage( s, 886 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  SI16 calcStealDiff( CChar *c, CItem *i )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check if item is able to be stolen based on item type and
//|					 weight vs stealing skill, then return the min skill needed to
//|					 steal the item.
//o---------------------------------------------------------------------------o
SI16 cSkills::calcStealDiff( CChar *c, CItem *i )
{
	if( i->IsLockedDown() )
		return -1;

	SI32 calcDiff, itemWeight;
	switch( i->GetType() )
	{
	case 1: // Backpack
		itemWeight = static_cast<SI32>(Weight->calcPackWeight( i ));
		calcDiff = (10 + c->GetSkill( STEALING ) / 4 );
		if( calcDiff > itemWeight )
			return max( min( ((int)((itemWeight + 9) / 20) * 10), 990 ), 0 );
		break;
	case 9:		// Spellbook
	case 63:	// Item spawn container
	case 64:	// Locked spawn container
	case 65:	// Unlockable item spawn container
	case 87:	// Trash container
		break;
	default:
		if( i->GetID() == 0x14F0 ) // Deeds
			return -1;

		if( i->GetWeight() <= 0 )
			i->SetWeight( Weight->calcItemWeight( i ) );

		itemWeight = i->GetWeight();
		calcDiff = (int)( 100 + c->GetSkill( STEALING ) * 2 );
		if( calcDiff > itemWeight ) // make it 2 times the base stealing skill... GM thieves can steal up to 21 stones, newbie only 1 stone
			return max( min( ((int)((itemWeight + 9) / 20) * 10), 990 ), 0 );
		break;
	}
	return -1;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Tracking( cSocket *s, int selection )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Start tracking selected NPC/PC
//o---------------------------------------------------------------------------o
void cSkills::Tracking( cSocket *s, int selection )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::Tracking() Invalid socket trying to track selection %i", selection );
		return;
	}
	CChar *i = s->CurrcharObj();
	i->SetTrackingTarget( i->GetTrackingTargets( selection ) ); // sets trackingtarget that was selected in the gump
	i->SetTrackingTimer( BuildTimeValue( cwmWorldState->ServerData()->GetTrackingBaseTimer() * i->GetSkill( TRACKING ) / 1000 + 1 ) ); // tracking time in seconds ... gm tracker -> basetimer + 1 seconds, 0 tracking -> 1 sec, new calc by LB
	i->SetTrackingDisplayTimer( BuildTimeValue( cwmWorldState->ServerData()->GetTrackingRedisplayTime() ) );
	sysmessage( s, 1644, chars[i->GetTrackingTarget()].GetName() );
	Track( i );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::CreateTrackingMenu( cSocket *s, int m )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Brings up Tracking Menu, Called when player uses Tracking Skill
//o---------------------------------------------------------------------------o
void cSkills::CreateTrackingMenu( cSocket *s, int m )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::CreateTrackingMenu() Invalid socket using menu %i", m );
		return;
	}
	UI32 i;
	int total, d;
	char lentext;
	char sect[512];
	char gmtext[MAXTRACKINGTARGETS][257];
	int gmid[MAXTRACKINGTARGETS];
	int id, id1 = 62, id2 = 399;
	int icon=8404; 
	CChar *c = s->CurrcharObj();
	SI32 type = 887;
	UI32 MaxTrackingTargets = 0;
	UI32 distance = cwmWorldState->ServerData()->GetTrackingBaseRange() + c->GetSkill( TRACKING ) / 50;
	
	if( m == ( 2 + TRACKINGMENUOFFSET ) )
	{
		id1 = 1;
		id2 = 61;
		icon = 0x20D1;
		type = 888;
	}
	if( m == ( 3 + TRACKINGMENUOFFSET ) )
	{
		id1 = 400;
		id2 = 402;
		icon = 8454;
		type = 889;
	}

	UnicodeTypes mLang = s->Language();
	
	sprintf( sect, "TRACKINGMENU %i", m );
	ScriptSection *TrackStuff = FileLookup->FindEntry( sect, tracking_def );
	if( TrackStuff == NULL )
		return;
	const char *tag = TrackStuff->First();
	const char *data = TrackStuff->GrabData();
	lentext = sprintf( gmtext[0], "%s %s", tag, data );
	int xOffset = MapRegion->GetGridX( c->GetX() );
	int yOffset = MapRegion->GetGridY( c->GetY() );
	UI08 worldNumber = c->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
			if( MapArea == NULL )	// no valid region
				continue;
			MapArea->PushChar();
			for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
			{
				if( tempChar == NULL )
					continue;
				d = getDist( tempChar, c );
				id = tempChar->GetID();
				cSocket *tSock = calcSocketObjFromChar( tempChar );
				bool cmdLevelCheck = isOnline( tempChar ) && ( c->GetCommandLevel() > tempChar->GetCommandLevel() );
				if( d <= distance && !tempChar->IsDead() && id >= id1 && id <= id2 && tSock != s && ( cmdLevelCheck || tempChar->IsNpc() ) )
				{
					c->SetTrackingTargets( calcCharFromSer( tempChar->GetSerial() ), MaxTrackingTargets );
					MaxTrackingTargets++;
					if( MaxTrackingTargets >= MAXTRACKINGTARGETS ) 
						break;
					SI32 dirMessage = 898;
					switch( TrackingDirection( s, tempChar ) )
					{
					case NORTH:		dirMessage = 890;	break;
					case NORTHWEST:	dirMessage = 891;	break;
					case NORTHEAST:	dirMessage = 892;	break;
					case SOUTH:		dirMessage = 893;	break;
					case SOUTHWEST:	dirMessage = 894;	break;
					case SOUTHEAST:	dirMessage = 895;	break;
					case WEST:		dirMessage = 896;	break;
					case EAST:		dirMessage = 897;	break;
					default:		dirMessage = 898;	break;
					}
					sprintf( gmtext[MaxTrackingTargets], "%s %s", tempChar->GetName(), Dictionary->GetEntry( dirMessage, mLang ) );
					gmid[MaxTrackingTargets] = creatures[id].Icon(); // placing correct icon
				}
			}
			MapArea->PopChar();
		}
	}
	
	if( MaxTrackingTargets == 0 )
	{
		sysmessage( s, type );
		return;
	}
	
	total = 9 + 1 + lentext + 1;
	for( i = 1; i <= MaxTrackingTargets; i++ ) 
		total += 4 + 1 + strlen( gmtext[i] );
	gmprefix[1] = total>>8;
	gmprefix[2] = total%256;
	gmprefix[3] = c->GetSerial( 1 );
	gmprefix[4] = c->GetSerial( 2 );
	gmprefix[5] = c->GetSerial( 3 );
	gmprefix[6] = c->GetSerial( 4 );
	if( m >= TRACKINGMENUOFFSET )
		m -= TRACKINGMENUOFFSET;
	gmprefix[7] = ( m + TRACKINGMENUOFFSET )>>8;
	gmprefix[8] = ( m + TRACKINGMENUOFFSET )%256;
	s->Send( gmprefix, 9 );
	s->Send( &lentext, 1 );
	s->Send( gmtext[0], lentext );
	s->Send( &MaxTrackingTargets, 1 );
	for( i = 1; i <= MaxTrackingTargets; i++ )
	{
		gmmiddle[0] = gmid[i]>>8;
		gmmiddle[1] = gmid[i]%256;
		s->Send( gmmiddle, 4 );
		lentext = strlen( gmtext[i] );
		s->Send( &lentext, 1 );
		s->Send( gmtext[i], lentext );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::TrackingMenu( cSocket *s, int gmindex )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Unknown
//o---------------------------------------------------------------------------o
void cSkills::TrackingMenu( cSocket *s, int gmindex )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::TrackingMenu() Invalid socket with gmindex %i", gmindex );
		return;
	}
	int total, i;
	char lentext;
	char sect[512];
	char gmtext[MAXTRACKINGTARGETS][257];
	int gmid[MAXTRACKINGTARGETS];
	int gmnumber=0;
	if( gmindex < 1000 )
	{
		Tracking( s, gmindex );
		return;
	}
	sprintf( sect, "TRACKINGMENU %i", gmindex );
	ScriptSection *TrackStuff = FileLookup->FindEntry( sect, tracking_def );
	if( TrackStuff == NULL )
		return;
	const char *tag = NULL;
	const char *data = NULL;
	tag = TrackStuff->First(); data = TrackStuff->GrabData();
	lentext = sprintf( gmtext[0], "%s %s", tag, data );
	for( tag = TrackStuff->Next(); !TrackStuff->AtEnd(); tag = TrackStuff->Next() )
	{
		data = TrackStuff->GrabData();
		gmnumber++;
		gmid[gmnumber] = makeNum( tag );
		strcpy( gmtext[gmnumber], data );
		tag = TrackStuff->Next();
	}
	total = 9 + 1 + lentext + 1;
	if( gmindex >= TRACKINGMENUOFFSET )
		gmindex -= TRACKINGMENUOFFSET;
	for( i = 1; i <= gmnumber; i++ ) 
		total += 4 + 1 + strlen( gmtext[i] );
	CChar *mChar = s->CurrcharObj();
	gmprefix[1] = total>>8;
	gmprefix[2] = total%256;
	gmprefix[3] = mChar->GetSerial( 1 );
	gmprefix[4] = mChar->GetSerial( 2 );
	gmprefix[5] = mChar->GetSerial( 3 );
	gmprefix[6] = mChar->GetSerial( 4 );
	gmprefix[7] = ( gmindex + TRACKINGMENUOFFSET )>>8;
	gmprefix[8] = ( gmindex + TRACKINGMENUOFFSET )%256;
	s->Send( gmprefix, 9 );
	s->Send( &lentext, 1 );
	s->Send( gmtext[0], lentext );
	lentext = gmnumber;
	s->Send( &lentext, 1 );
	for( i = 1; i <= gmnumber; i++ )
	{
		gmmiddle[0] = gmid[i]>>8;
		gmmiddle[1] = gmid[i]%256;
		s->Send( gmmiddle, 4 );
		lentext = strlen( gmtext[i] );
		s->Send( &lentext, 1 );
		s->Send( gmtext[i], lentext );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Track( CChar *i )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Tracking stuff for older client
//o---------------------------------------------------------------------------o
void cSkills::Track( CChar *i )
{
	cSocket *s = calcSocketObjFromChar( i );
	if( s == NULL )
		return;
	CHARACTER trackTarg = i->GetTrackingTarget();
	if( chars[trackTarg].GetY() == -1 ) 
		return;
	CPTrackingArrow tSend = chars[trackTarg];
	tSend.Active( 1 );
	s->Send( &tSend );
}

//o---------------------------------------------------------------------------o
//|   Function    :  UI08 cSkills::TrackingDirection( cSocket *s, CChar *i )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Get location of the tracking target based on your loc
//o---------------------------------------------------------------------------o
UI08 cSkills::TrackingDirection( cSocket *s, CChar *i )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::TrackingDirection() Invalid socket for character %i", i->GetSerial() );
		return 0;
	}
	UI08 direction = 5;
	CChar *j = s->CurrcharObj();
	if( ( j->GetY() - direction ) >= i->GetY() )  // North
	{
		if( ( j->GetX() - direction ) > i->GetX() )
			return NORTHWEST;
		if( ( j->GetX() + direction ) < i->GetX() )
			return NORTHEAST;
		return NORTH;
	}
	else if( ( j->GetY() + direction ) <= i->GetY() )  // South
	{
		if( ( j->GetX() - direction ) > i->GetX() )
			return SOUTHWEST;
		if( ( j->GetX() + direction ) < i->GetX() )
			return SOUTHEAST;
		return SOUTH;
	}
	else if( ( j->GetX() - direction ) >= i->GetX() )  // West
		return WEST;
	else if( ( j->GetX() + direction ) <= i->GetX() )  // East
		return EAST;
	else 
		return NORTH;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::BeggingTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets PC/NPC with the begging skill
//o---------------------------------------------------------------------------o
void cSkills::BeggingTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::BeggingTarget() Invalid socket" );
		return;
	}
	CChar *targChar = calcCharObjFromSer( s->GetDWord( 7 ) );
	if( calcSocketFromChar( targChar ) != -1 )
	{
		sysmessage( s, 899 );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	if( targChar == NULL )
		return;

	if( !targChar->IsNpc() )
	{
		sysmessage( s, 899 );
		return;
	}
	if( targChar->GetNPCAiType() == 17 )
	{
		sysmessage( s, 900 );
		return;
	}
	if( getDist( targChar, mChar ) >= cwmWorldState->ServerData()->GetBeggingRange() )
	{
		sysmessage( s, 901 );
		return;
	}
	if( targChar->GetID() == 0x0190 || targChar->GetID() == 0x0191 )
	{
		SI32 bankGold = getBankCount( mChar, 0x0EED, 0 );
		SI32 currentGold = getItemAmt( mChar, 0x0EED, 0 );
		SI32 petGold = 0;
		CHARLIST *myPets = mChar->GetPetList();
		// Find all the gold that will be on any pets we own
		if( myPets != NULL )
		{
			for( int a = 0; a < myPets->size(); a++ )	// All pet/hirelings
			{
				CChar *pet = (*myPets)[a];
				if( pet != NULL )
					petGold += getItemAmt( pet, 0x0EED, 0 );
			}
		}
		if( bankGold + currentGold + petGold > 3000 )
		{
			sysmessage( s, 1645 );
			return;
		}
		npcTalkAll( mChar, RandomNum( 1662, 1664 ), false ); // Updated to make use of dictionary
		if( !CheckSkill( mChar, BEGGING, 0, 1000 ) )
			sysmessage( s, 902 );
		else
		{
			npcTalkAll( targChar, 903, false ); 
			addgold( s, ( 10 + RandomNum( 0, mChar->GetSkill( BEGGING ) + 1 ) / 25 ) );
			sysmessage( s, 904 );
		}
	}
	else
		sysmessage( s, 905 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::AnimalLoreTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets animal with Animal Lore skill
//o---------------------------------------------------------------------------o
void cSkills::AnimalLoreTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::AnimalLoreTarget() Invalid socket" );
		return;
	}
	CChar *i = calcCharObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar = s->CurrcharObj();
	if( i == NULL )
		return;
	if( i->GetCommandLevel() > 0 )
	{
		sysmessage( s, 906 );
		return;
	}
	if( i->GetID() == 0x0190 || i->GetID() == 0x0191 )
	{
		sysmessage( s, 907 );
		return;
	}
	char temp[1024];
	if( CheckSkill( mChar, ANIMALLORE, 0, 1000 ) )
	{
		sprintf( temp, "Attack [%i %i], Defense [%i] Taming [%i] Hit Points [%i]", i->GetLoDamage(), i->GetHiDamage(), i->GetDef(), i->GetTaming()/10, i->GetHP() );
		npcEmote( s, i, temp, false );
	}
	else
		sysmessage( s, 908 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::ForensicsTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets corpse with forensics skill
//|					 (needs to be given more functionality so players can get
//|					 more use out of the skill)
//o---------------------------------------------------------------------------o
void cSkills::ForensicsTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::ForensicsTarget() Invalid socket" );
		return;
	}
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar = s->CurrcharObj();
	if( i == NULL )
	{
		sysmessage( s, 909 );
		return;
	}
	if( !i->isCorpse() )
	{
		sysmessage( s, 909 );
		return;
	}

	if( !itemInRange( mChar, i, 6 ) )
	{
		sysmessage( s, 393 );
		return;
	}

	if( !LineOfSight( s, mChar, i->GetX(), i->GetY(), i->GetZ(), WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) )
	{
		sysmessage( s, 1646 );
		return;
	}

	CChar *cMurderer = calcCharObjFromSer( i->GetMurderer() );
	char temp[1024];
	int getTime = uiCurrentTime;
	if( mChar->IsGM() )
	{
		if( cMurderer != NULL )
			sysmessage( s, 910, i->GetName(), (getTime - i->GetMurderTime() ) / CLOCKS_PER_SEC, cMurderer->GetName() );
		else
			sysmessage( s, 910, i->GetName(), (getTime - i->GetMurderTime() ) / CLOCKS_PER_SEC, "" );
	}
	else 
	{
		if( !CheckSkill( mChar, FORENSICS, 0, 500 ) ) 
			sysmessage( s, 911 ); 
		else
		{
			int duration = ( getTime - i->GetMurderTime() ) / CLOCKS_PER_SEC;
			if(		 duration > 180 ) 
				strcpy( temp, Dictionary->GetEntry( 912 ) );
			else if( duration >  60 ) 
				strcpy( temp, Dictionary->GetEntry( 913 ) );
			else					  
				strcpy( temp, Dictionary->GetEntry( 914 ) );

			sysmessage( s, 915, &i->GetName()[2], temp );
			if( !CheckSkill( mChar, FORENSICS, 500, 1000 ) || cMurderer == NULL ) 
				sysmessage( s, 916 ); 
			else if( cMurderer != NULL )
				sysmessage( s, 917, cMurderer->GetName() );
			else
				sysmessage( s, 917, "" );
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::PoisoningTarget( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets an item with PoisoningSkill
//o---------------------------------------------------------------------------o
void cSkills::PoisoningTarget( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::PoisoningTarget() Invalid socket" );
		return;
	}
	bool canPoison = false;
	CChar *mChar = s->CurrcharObj();
	CItem *poison = calcItemObjFromSer( mChar->GetPoisonSerial() );
	mChar->SetPoisonSerial( 0 );
	if( poison == NULL )
		return;

	if( poison->GetType() != 19 || poison->GetMoreY() != 6 ) 
	{
		sysmessage( s, 918 );
		return;
	}

	CItem *toPoison = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( toPoison == NULL )
	{
		sysmessage( s, 920 );
		return;
	}
	UI16 realID = toPoison->GetID();
	if( ( realID >= 0x0F4F && realID <= 0x0F50 ) || ( realID >= 0x13B1 && realID <= 0x13B2 ) || ( realID >= 0x13FC && realID <= 0x13FD ) )
	{
		sysmessage( s, 1647 );
		return;
	}
	if( toPoison->GetHiDamage() <= 0 )
	{
		sysmessage( s, 1648 );
		return;
	}		
	switch( poison->GetMoreZ() ) 
	{
	case 1:	canPoison = CheckSkill( mChar, POISONING, 0, 500 );		break;	// lesser poison
	case 2:	canPoison = CheckSkill( mChar, POISONING, 151, 651 );		break;	// poison
	case 3:	canPoison = CheckSkill( mChar, POISONING, 551, 1051 );	break;	// greater poison
	case 4:	canPoison = CheckSkill( mChar, POISONING, 901, 1401 );	break;	// deadly poison
	default:	Console.Error( 2, "cSkills::PoisoningTarget -> Fallout of switch statement without default" );	return;
	}
	if( canPoison ) 
	{
		soundeffect( mChar, 0x0247 );	// poisoning effect
		if( toPoison->GetPoisoned() < poison->GetMoreZ() ) 
			toPoison->SetPoisoned( poison->GetMoreZ() );
		sysmessage( s, 919 );
	} 
	else 
	{
		soundeffect( mChar, 0x0247 ); // poisoning effect
		sysmessage( s, 1649 );
	}

	decItemAmount( poison );
	CItem *bPotion = Items->SpawnItem( s, mChar, 1, "#", true, 0x0F0E, 0, true, false );
	if( bPotion != NULL )
	{
		bPotion->SetDecayable( true );
		RefreshItem( bPotion );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void TellScroll( char *menu_name, cSocket *player, long item_param )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Used for inscribe itemmenu
//o---------------------------------------------------------------------------o
void TellScroll( const char *menu_name, cSocket *player, long item_param )
{
	Skills->Inscribe( player, item_param );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Inscribe( cSocket *s, long snum )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player targets an item with Inscribe skill
//|					 if item is not a scroll calls Engraving function
//|					 which allows a player to give Weapons and Armor magical
//|					 properties
//o---------------------------------------------------------------------------o
void cSkills::Inscribe( cSocket *s, long snum )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::Inscribe() Invalid socket inscribing %i", snum );
		return;
	}
	CChar *inscChar = s->CurrcharObj();
	CItem *i = NULL;
	
	if( snum > 0 ) // if 0 then its the first time
	{
		unsigned getCir = (int)( ( snum - 800 ) * .1 );
		unsigned getSpell = ( snum - 800 ) - ( getCir * 10 ) + 1;
		CItem *spellBook = FindItemOfType( inscChar, 9 );
		if( spellBook == NULL )
		{
			sysmessage( s, 921 );
			return;
		}
		if( !Magic->CheckBook( getCir, getSpell - 1, spellBook ) )
		{
			sysmessage( s, 922 );
			return;
		}
		i = calcItemObjFromSer( inscChar->GetMaking() ); // lets re-grab the item they clicked on
		inscChar->SetMaking( 0 );          //clear it out now that we are done with it.
		
		make_st mMake = s->ItemMake();		
		if( i->GetID() == 0x0E34 )
		{
			mMake.minskill = ( snum - 810 ) * 10; mMake.maxskill = ( snum - 490 ) * 10; 

			int num = 8 * ( getCir - 1 ) + getSpell;
			if( spells[num].Action() ) 
				impaction( s, spells[num].Action() ); // Should have a default action instead
			npcTalkAll( inscChar, spells[num].Mantra(), false );
			
			if( !Magic->CheckReagents( inscChar, spells[num].ReagantsPtr() ) || !Magic->CheckMana( inscChar, num ) )
			{ 
				Magic->SpellFail( s );
				return;
			}
			
			// Check after resource check, so no exploited gains
			CheckSkill( inscChar, INSCRIPTION, ( snum - 810 ) * 10, (snum - 490 ) * 10 );
			Magic->DelReagents( inscChar, spells[num].Reagants() );
			Magic->SubtractMana( inscChar, spells[num].Mana() );
			s->ItemMake( mMake );
#pragma note( "TODO: Inscription relies on item numbered spells!  We either figure out a way to express this as a string name, or we need the spells as item number entries still" )
			char tempString[32];
			sprintf( tempString, "%i", snum );
			MakeMenuTarget( s, tempString, INSCRIPTION ); //put it in your pack
		}
		else if( cwmWorldState->ServerData()->GetEngraveStatus() && ( i->GetDef() || i->GetHiDamage() ) ) //or is it an item?
		{
			int canEngrave = 0;
			if( CheckSkill( inscChar, INSCRIPTION, (getCir * 100 - 89 ), ( getCir * 100 + 301 ) ) )
				canEngrave++;
			if( CheckSkill( inscChar, MAGERY, (getCir * 100 - 89 ), ( getCir * 100 + 301 ) ) )
				canEngrave++;
			if( CheckSkill( inscChar, TINKERING, (getCir * 100 - 89 ), ( getCir * 100 + 301 ) ) )
				canEngrave++;
			
			if( canEngrave < 3 ) 
			{
				sysmessage( s, 923 + canEngrave );
				i->IncHP( (canEngrave - 3) );
				if( i->GetHP() < 1 )	// Check to ensure items HP are > 0
					i->SetHP( 1 );
			}
			else
			{
				if( !( i->GetMoreZ() == 0 ||( i->GetMoreX() == getCir && i->GetMoreY() == getSpell ) ) )
					sysmessage( s, 926 );
				else if( i->GetMoreZ() >= ( 18 - i->GetMoreX() * 2 ) )	// 2 less charges per circle
					sysmessage( s, 927 );
				else if( i->GetMoreX() == getCir && i->GetMoreY() == getSpell ) 
				{
					if( EngraveAction( s, i, getCir, getSpell ) )
					{
						i->SetMoreZ( i->GetMoreZ() + 1 );
						sysmessage( s, 928 );
					}
					else 
						sysmessage( s, 929 );
				}
				else if( EngraveAction( s, i, getCir, getSpell ) )
				{
					i->SetType2( i->GetType() ); //kept type of item for returning to this type when item remain no charge 
					i->SetType( 15 );  // make it magical
					i->SetMoreX( getCir ); // spell circle
					i->SetMoreY( getSpell ); // spell number
					i->SetMoreZ( i->GetMoreZ() + 1 );  // charges
					sysmessage( s, 928 );
				}
				else 
					sysmessage( s, 929 );
			}
			decItemAmount( i );
		}
		return;
	}
 
	 // - Find what they clicked on
	 if( s->GetDWord( 11 ) == INVALIDSERIAL )	// don't do this if the buffer got flushed
	 {
		 Console << "Inscribe() called an invalid object." << myendl;
		 return;
	 }
	 i = calcItemObjFromSer( s->GetDWord( 7 ) );
	 if( i == NULL )
	 {
		 sysmessage( s, 930 );
		 inscChar->SetMaking( 0 );
		 return;
	 }
 
	 inscChar->SetMaking( i->GetSerial() );  //we gotta remember what they clicked on!
	 if( i->GetID() == 0x0E34 || ( cwmWorldState->ServerData()->GetEngraveStatus() && ( i->GetDef() || i->GetHiDamage() ) ) )
	 {
/* 
		select spell gump menu system here, must return control to UOX so we dont
		freeze the game. when returning to this routine, use snum to determine where to go
		and snum is also the value of what they selected. 
*/
		 im_sendmenu( "InscribeMenu", s );  //this is in im.cpp file using inscribe.gmp
	 }
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool cSkills::EngraveAction( cSocket *s, CItem *i, int getCir, int getSpell )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player starts engraving an item
//o---------------------------------------------------------------------------o
bool cSkills::EngraveAction( cSocket *s, CItem *i, int getCir, int getSpell )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::EngraveAction() Invalid socket with circle %i and spell %i on item %i", getCir, getSpell, i->GetSerial() );
		return false;
	}
	CChar *mChar = s->CurrcharObj();
	int num = ( 8 * ( getCir - 1 ) ) + getSpell;
	
	if( spells[num].Action() ) 
		impaction( s, spells[num].Action() ); // Should have a default action instead
	if( !Magic->CheckReagents( mChar, spells[num].ReagantsPtr() ) || !Magic->CheckMana( mChar, num ) )
	{
		Magic->SpellFail( s );
		return false;
	}
		
	npcTalkAll( mChar, spells[num].Mantra(), false );
	Magic->DelReagents( mChar, spells[num].Reagants() );
	Magic->SubtractMana( mChar, spells[num].Mana() );
	
	switch( getCir )
	{
	case 1:	// Circle 1
		switch( getSpell )
		{
		case 1: i->SetOffSpell( 1 );	break;      // Clumsy
		case 3:	i->SetOffSpell( 2 );	break;      // Feeblemind
		case 5: i->SetOffSpell( 3 );	break;		// Magic Arrow
		case 7: sysmessage( s, 931 );		return false;	// Reactive Armor
		case 8: i->SetOffSpell( 4 );	break;		// Weaken
		case 2:											// Create Food
		case 4:											// Heal
		case 6:											// Night Sight
				break;
		default:	
				Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
				return false;
		}
		break;
	case 2:	// Circle 2
		switch( getSpell )
		{
		case 1:			// Agility
		case 2:			// Cunning
		case 8:			// Strength
				break;
		case 3:			// Cure
		case 5:			// Magic Trap
		case 6:			// Magic Untrap
		case 7:			// Protection
				sysmessage( s, 931 );	
				return false;
		case 4:			 // Harm
				i->SetOffSpell( 5 );	
				break;
		default:	
				Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
				return false;
		}
		break;
	case 3:	// Circle 3
		switch( getSpell )
		{
		case 2:			// Fireball
				i->SetOffSpell( 6 );	
				break;
		case 3: //Magic lock
		case 5: //Telekinesis
				sysmessage( s, 931 );	
				return false;
		case 1:	// Bless
		case 4: //Poison
		case 6:	// Teleport
		case 7:	// Unlock
		case 8:	// Wall of Stone
				break;
		default:	
				Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
				return false;
		}
		break;
	case 4:	// Circle 4
		switch( getSpell )
		{       
		case 1:// Arch Cure
		case 2:// Arch Protection
		case 7://Mana Drain
				sysmessage( s, 931 );	
				return false;
		case 3:		// Curse
				i->SetOffSpell( 8 );	
				break;
		case 4:// Fire Field
		case 5:// Greater Heal
		case 8:// Recall
				break;
		case 6:		// Lightning
				i->SetOffSpell( 9 );	
				break;
		default:	
				Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
				return false;
		}
		break;
	case 5:	// Circle 5
		switch( getSpell )
		{
		case 1://Blade Spirit
		case 2://Dispel Field
		case 4://Magic Reflection
		case 7://Poison Field
				break;
		case 3:	//Incognito
		case 8:	//Summon Creature
				sysmessage( s, 931 );	
				return false;
		case 5:		//Mind Blast
				i->SetOffSpell( 11 );		
				break;
		case 6:		//Paralyse
				i->SetOffSpell( 12 );		
				break;
		default:	
				Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
				return false;
		}
		break;
	case 6:	// Circle 6
		switch( getSpell )
		{
		case 1://Dispel
		case 4://Invisibility
		case 5://Mark
		case 7:// Paralyze Field
		case 8://Reveal
				break;
		case 2:	  //Energy Bolt
				i->SetOffSpell( 13 );		
				break;
		case 3:		//Explosion
				i->SetOffSpell( 14 );		
				break;
		case 6:		// Mass Curse
				sysmessage( s, 931 );	
				return false;
		default:	
				Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
				return false;
		}
		break ;
	case 7:	// Circle 7
		switch( getSpell )
		{
		case 1://Chain Lightning
		case 5:// Mana Vampire
		case 6:// Mass Dispel
		case 7:// Meteor Storm
		case 8:// Polymorph
				sysmessage( s, 931 );	
				return false;
		case 2://Energy Field
		case 4://Gate Travel
				break;
		case 3:		// FlameStrike
				i->SetOffSpell( 15 );	
				break;
		default:	
				Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
				return false;
		}
		break ;
	case 8:	// Circle 8
		switch( getSpell )
		{
		case 1:// Earthquake
				sysmessage( s, 931 );	
				return false;
		case 2://Energy Vortex
		case 3://Resurrection
		case 4:// Summon Air Elemental
		case 5://Summon Daemon
		case 6:// Summon Earth Elemental
		case 7:// Summon Fire Elemental
		case 8:// Summon Water Elemental
				break;
		default:	
				Console.Error( 2, " cSkills::Inscribe -> Fallout of switch statement without default" );
				return false;
		}
		break;
	default:
		Console.Error( 2, " Fallout of switch statement without default. skills.cpp, cSkills::Inscribe" );
		return false;
  }
  return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::updateSkillLevel( CChar *c, int s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Calculate the skill of this character based on the 
//|					 characters baseskill and stats
//o---------------------------------------------------------------------------o
void cSkills::updateSkillLevel( CChar *c, int s )
{
	UI16 sstr = skill[s].strength, astr = c->ActualStrength();
	UI16 sdex = skill[s].dexterity, adex = c->ActualDexterity();
	UI16 sint = skill[s].intelligence, aint = c->ActualIntelligence();
	UI16 bskill = c->GetBaseSkill( s );

	UI16 temp = ( ( (sstr * astr) / 100 + (sdex * adex) / 100 + (sint + aint) / 100) * ( 1000 - bskill ) ) / 1000 + bskill;
	c->SetSkill( max( bskill, temp ), s );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::LockPick( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses lockpics on a locked item
//o---------------------------------------------------------------------------o
void cSkills::LockPick( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::LockPick() Invalid socket" );
		return;
	}
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar = s->CurrcharObj();
	if( i == NULL )
		return;
	if( i->GetType()==1 || i->GetType()==12 || i->GetType() == 63 ) 
	{
		sysmessage( s, 932 );
		return;
	}
	
	if( i->GetID() == 0x1E2C || i->GetID() == 0x1E2D )
	{
		if( mChar->GetSkill( LOCKPICKING ) < 300 )
		{
			CheckSkill( mChar, LOCKPICKING, 0, 1000 );	// check their skill
			soundeffect(s, 0x0241, true );						// lockpicking sound
		}
		else
		{
			if( RandomNum( 0, 99 ) > 50 )		// chance it could break the pick
			{
				sysmessage( s, 933 );
				ITEM mItem = s->AddMItem();
				decItemAmount( mItem );
			}
			else
				sysmessage( s, 934 );
		}
	}
	if(i->GetType()==8 || i->GetType()==13 || i->GetType()==64)
	{
		if( i->GetMore() == 0 ) //Make sure it isn't an item that has a key (i.e. player house, chest..etc)
		{
			bool canPick = false;
			if( s->AddMItem() == INVALIDSERIAL ) 
			{
				if( s->CurrentSpellType() != 2 )              // not a wand cast
				{
					Magic->SubtractMana( mChar, 5 );  // subtract mana on scroll or spell
					if( s->CurrentSpellType() == 0 )             // del regs on normal spell
					{
						reag_st toDel;
						toDel.ash = 1;
						toDel.moss = 1;
						Magic->DelReagents( mChar, toDel );
					}
				}
				canPick = true;
			} 
			else if( CheckSkill( mChar, LOCKPICKING, 0, 1000 ) )
				canPick = true;
			else
			{
				if( RandomNum( 0, 99 ) > 50 ) 
				{
					sysmessage( s, 933 );
					Items->DeleItem( &items[s->AddMItem()] );
				} 
				else
					sysmessage( s, 936 );
			}
			if( canPick )
			{
				switch( i->GetType() )
				{
				case 8:		i->SetType( 1 );	break;
				case 13:	i->SetType( 12 ); break;
				case 64:	i->SetType( 63 ); break;
				default:	Console.Error( 2, " cSkills::LockPick -> Fallout of switch statement without default" ); return;
				}
				soundeffect( i, 0x01FF );
				sysmessage( s, 935 );
			}
		} 
		else
			sysmessage( s, 937 );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::TDummy( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses a training dummy
//o---------------------------------------------------------------------------o
void cSkills::TDummy( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::TDummy() Invalid socket" );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	UI08 type = Combat->getBowType( mChar );
	
	if( type > 0 )
	{
		sysmessage( s, 938 );
		return;
	}
	if( mChar->IsOnHorse() )
		Combat->CombatOnHorse( mChar );
	else
		Combat->CombatOnFoot( mChar );
	
	switch( RandomNum( 0, 2 ) )
	{
	case 0: soundeffect( s, 0x013B, true );	break;
	case 1: soundeffect( s, 0x013C, true );	break;        
	case 2: soundeffect( s, 0x013D, true );	break;
	default:	Console.Error( 2, " cSkills::TDummy -> Fallout of switch statement without default" );	return;
	}            
	SERIAL serial = (s->GetDWord( 1 ))&0x7FFFFFFF;
	CItem *j = calcItemObjFromSer( serial );
	if( j != NULL )
	{
		if( j->GetID( 2 ) == 0x70 ) 
			j->SetID( 0x71, 2 );
		else if( j->GetID( 2 ) == 0x74 ) 
			j->SetID( 0x75, 2 );
		tempeffect( NULL, j, 14, 0, 0, 0 );
		RefreshItem( j );
	}
	UI16 skillused = Combat->getCombatSkill( mChar );
	if( mChar->GetSkill( skillused ) < 300 )
	{
		CheckSkill( mChar, skillused, 0, 1000 );
		if( mChar->GetSkill( TACTICS ) < 300 )
			CheckSkill( mChar, TACTICS, 0, 250 );  //Dupois - Increase tactics but only by a fraction of the normal rate
	}
	else
		sysmessage( s, 939 );
}                           


//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Tinkering( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses tinkering skill
//o---------------------------------------------------------------------------o
void cSkills::Tinkering( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::Tinkering() Invalid socket" );
		return;
	}

	CChar *mChar = s->CurrcharObj();
	CItem *packnum = getPack( mChar );
	if( packnum == NULL ) 
	{
		sysmessage( s, 773 ); 
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	make_st mMake = s->ItemMake();
	if( i == NULL )
	{
		sysmessage( s, 942 );
		return;
	}
	UI16 realID = i->GetID();
	if( realID == 0x1BEF || realID == 0x1BF2 || realID == 0x1BDD || realID == 0x1BE0 )
	{
		if( getPackOwner( i ) != mChar )
			sysmessage( s, 775 );
		else
		{
			int getAmt = getAmount( mChar, realID );
			if( getAmt < 2 )
			{ 
				sysmessage( s, 940 );
				return;
			}
			mMake.has = getAmt;
			mMake.material1 = i->GetID();
			s->ItemMake( mMake );
			if( realID == 0x1BDD || realID == 0x1BE0 )
			{
				if( getAmt < 4 )
				{
					sysmessage( s, 941 );
					return;
				} 
			}
			NewMakeMenu( s, 59, TINKERING );
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::AButte( cSocket *s, CItem *x )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses an Archery Butte
//o---------------------------------------------------------------------------o
void cSkills::AButte( cSocket *s, CItem *x )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::AButte() Invalid socket" );
		return;
	}
	int getDist, i = 0;
	int arrowsquant=0;
	CChar *mChar = s->CurrcharObj();
	UI08 type = Combat->getBowType( mChar );
	
	if( x->GetID( 2 ) == 0x0a )
	{ // East Facing Butte
		if( x->GetX() > mChar->GetX() || x->GetY() != mChar->GetY() )
			getDist = -1;
		else 
			getDist = mChar->GetX() - x->GetX();
	}
	else
	{ // South Facing Butte
		if( x->GetY() > mChar->GetY() || x->GetX() != mChar->GetX() )
			getDist = -1;
		else 
			getDist = mChar->GetY() - x->GetY();
	}
	
	if( getDist == 1 )
	{
		CItem *c;
		if( x->GetMore( 1 ) > 0 )
		{
			c = Items->SpawnItem( s, x->GetMore( 1 ), "#", true, 0x0F3F, 0, true, false );
			if( c == NULL ) 
				return;
			RefreshItem( c );
		}
		if( x->GetMore( 2 ) > 0 )
		{
			c = Items->SpawnItem( s, x->GetMore( 2 ),"#", true, 0x1BFB, 0, true, false );
			if( c == NULL ) 
				return;
			RefreshItem( c );
		}
		
		if( x->GetMore( 1 ) > 0 ) 
			i++;
		if( x->GetMore( 2 ) > 0 ) 
			i += 2;
		
		switch( i )
		{
		case 0:		sysmessage( s, 943 );												break;
		case 1:		sysmessage( s, 944, x->GetMore( 1 ) );						break;
		case 2:		sysmessage( s, 945, x->GetMore( 2 ) );						break;
		case 3:		sysmessage( s, 946, x->GetMore( 1 ), x->GetMore( 2 ) );		break;
		default:	Console.Error( 2, " cSkills::AButte -> Fallout of switch statement without default" );
			return;
		}
		x->SetMore( 0, 1 );
		x->SetMore( 0, 2 );
	}
	
	if( getDist >= 5 && getDist <= 8 )
	{
		if( type == 0 )
		{
			sysmessage( s, 947 );
			return;
		} 
		if( x->GetMore( 1 ) + x->GetMore( 2 ) > 99 )
		{
			sysmessage( s, 948 );
			return;
		}
		if( type == BOWS ) 
			arrowsquant = getAmount( mChar, 0x0F3F );
		else 
			arrowsquant = getAmount( mChar, 0x1BFB );
		if( arrowsquant == 0 ) 
		{
			sysmessage( s, 949 );
			return;
		}
		if( type == BOWS )
		{
			deleQuan( mChar, 0x0F3F, 1 );
			x->SetMore( x->GetMore( 1 ) + 1, 1 );
			//add moving effect here to item, not character
		}
		else
		{
			deleQuan( mChar, 0x1BFB, 1 );
			x->SetMore( x->GetMore( 2 ) + 1, 2 );
			//add moving effect here to item, not character
		} 
		if( mChar->IsOnHorse() ) 
			Combat->CombatOnHorse( mChar );
		else 
			Combat->CombatOnFoot( mChar );
		
		if( mChar->GetSkill( ARCHERY ) < 350 )
			CheckSkill( mChar, ARCHERY, 0, 1000 );
		else
			sysmessage( s, 950 );
		
		switch( ( mChar->GetSkill( ARCHERY ) + ( RandomNum( 0, 199 ) - 100 ) ) / 100 )
		{
		case -1:
		case 0:
		case 1:		
			sysmessage( s, 951 );	
			soundeffect( s, 0x0238, true );	
			break;
		case 2:
		case 3:		
			sysmessage( s, 952 );	
			soundeffect( s, 0x0234, true );	
			break;
		case 4:
		case 5:
		case 6:		
			sysmessage( s, 953 );	
			soundeffect( s, 0x0234, true );	
			break;
		case 7:
		case 8:
		case 9:		
			sysmessage( s, 954 );	
			soundeffect( s, 0x0234, true );	
			break;
		case 10:
		case 11:	
			sysmessage( s, 955 );	
			soundeffect( s, 0x0234, true );	
			break;
		default:																						break;
		}
	}
	else
		sysmessage( s, 956 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::TinkerAxel( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Use tinkering on an Axel
//o---------------------------------------------------------------------------o
void cSkills::TinkerAxel( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::TinkerAxel() Invalid socket" );
		return;
	}
	CItem *origPart = calcItemObjFromSer( s->AddID() );
	if( origPart == NULL )
	{
		sysmessage( s, 957 );
		return;
	}

	bool canCombine = false;
	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar = s->CurrcharObj();
	if( i != NULL )
	{
		if( i == origPart )
		{
			sysmessage( s, 958 );
			return;
		}

		UI16 p2id = i->GetID();
		switch( origPart->GetID() )
		{
		case 0x1053:
		case 0x1054:	// gears
			if( p2id == 0x105B || p2id == 0x105C )
				canCombine = true;
			break;
		case 0x105B:
		case 0x105C:	// axles
			if( p2id == 0x1053 || p2id == 0x1054 )
				canCombine = true;
			break;
		default:	// shouldn't get here
			sysmessage( s, "You shouldn't have got here cSkills::TinkerAxel()" );
			return;
		}
		if( canCombine && itemInRange( mChar, i, 3 ) )
		{
			if( !CheckSkill( mChar, TINKERING, 0, 1000 ) )
				sysmessage( s, 959 );
			else
			{
				sysmessage( s, 960 );
				decItemAmount( origPart );
				decItemAmount( i );
				CItem *c = Items->SpawnItem( s, 1, Dictionary->GetEntry( 961 ), true, 0x1051, 0, true, true );
				if( c == NULL ) 
					return;
			}
		}
	} 
	mChar->SetTailItem( INVALIDSERIAL );
	if( !canCombine ) 
		sysmessage( s, 962 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::TinkerAwg( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Unknown : Tinkering
//o---------------------------------------------------------------------------o
void cSkills::TinkerAwg( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::TinkerAwg() Invalid socket" );
		return;
	}

	CChar *mChar = s->CurrcharObj();
	CItem *packnum = getPack( mChar );
	if( packnum == NULL ) 
	{
		sysmessage( s, 773 );
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( i == NULL )
		return;

	make_st mMake = s->ItemMake();
	char id2;
	UI16 realID = i->GetID();
	UI16 matID1 = mMake.material1;
	if( ( realID == 0x1051 || realID == 0x1052 ) && ( matID1 == 0x105D || matID1 == 0x105E ) ||
		( realID == 0x105D || realID == 0x105E ) && ( matID1 == 0x1051 || matID1 == 0x1052 ) )
		id2 = 0x4F;
	else if( ( realID == 0x1051 || realID == 0x1052 ) && ( matID1 == 0x1055 || matID1 == 0x1056 ) ||
		( realID == 0x1055 || realID == 0x1056 ) && ( matID1 == 0x1051 || matID1 == 0x1052 ) )
		id2 = 0x59;
	else
	{
		sysmessage( s, 962 );
		return;
	}
	if( getPackOwner( i ) != mChar )
	{
		sysmessage( s, 963 );
		return;
	}
	else if( !CheckSkill( mChar, TINKERING, 0, 1000 ) )
	{
		sysmessage( s, 964 );
		deleQuan( mChar, i->GetID(), 1 );
		return;
	}
	else
	{
		char temp[1024];
		if( id2 == 0x4F ) 
			strcpy( temp, Dictionary->GetEntry( 965 ) );
		else if( id2 == 0x59 ) 
			strcpy( temp, Dictionary->GetEntry( 966 ) );
		CItem *c = Items->SpawnItem( s, 1, temp, false, 4096 + id2, 0, true, true );
		if( c == NULL ) 
			return;
		sysmessage( s, 960 );
	}
	deleQuan( mChar, mMake.material1, 1 );
	deleQuan( mChar, i->GetID(), 1 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::TinkerClock( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Used when player Tinkers a clock
//o---------------------------------------------------------------------------o
void cSkills::TinkerClock( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::TinkerClock() Invalid socket" );
		return;
	}
	bool canCombine = false;

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	CChar *mChar = s->CurrcharObj();
	if( i != NULL )
	{
		UI16 realID = i->GetID();
		if( realID >= 0x104D && realID <= 0x1050 )
		{
			if( itemInRange( mChar, i, 3 ) )
			{
				if( !CheckSkill( mChar, TINKERING, 0, 1000 ) ) 
					sysmessage( s, 959 );
				else
				{
					sysmessage( s, 960 );
					
					CItem *c = Items->SpawnItem( s, 1, "clock", 0, 0x104B, 0, true, true );
					if( c == NULL ) 
						return;
				}
				decItemAmount( i );
				canCombine = true;
			}
		} 
	}   
	mChar->SetTailItem( INVALIDSERIAL );
	if( !canCombine ) 
		sysmessage( s, 962 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Meditation( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player uses meditation skill
//o---------------------------------------------------------------------------o
void cSkills::Meditation( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::Meditation() Invalid socket" );
		return;
	}

	CChar *mChar = s->CurrcharObj();
	if( Combat->calcDef( mChar, 0, false ) > 10 )
	{
		sysmessage( s, 967 );
		mChar->SetMed( 0 );
		return;
	}
	if( Combat->getWeapon( mChar ) != NULL || Combat->getShield( mChar ) != NULL )
	{
		sysmessage( s, 968 );
		mChar->SetMed( 0 );
		return;
	}
	if( mChar->GetMana() == mChar->GetMaxMana() )
	{
		sysmessage( s, 969 );
		mChar->SetMed( 0 );
		return;
	}
	if( !CheckSkill( mChar, MEDITATION, 0, 1000 ) ) 
	{
		sysmessage( s, 970 );
		mChar->SetMed( 0 );
		return;
	}
	sysmessage( s, 971 );
	mChar->SetMed( 1 );
	soundeffect( s, 0x00F9, true );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Persecute( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when a Ghost attacks a Player.  If entry in server.scp
//|					 is enabled, players mana decreases each time you try to
//|					 persecute him
//o---------------------------------------------------------------------------o
void cSkills::Persecute( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::Persecute() Invalid socket" );
		return;
	}
	CChar *c = s->CurrcharObj();
	CHARACTER target = c->GetTarg();
	if( target == INVALIDSERIAL || chars[target].IsGM() ) 
		return;
	
	CChar *targChar = &chars[target];

	int decrease = (int)( c->GetIntelligence() / 10 ) + 3;
	
	if( c->GetSkillDelay() <= uiCurrentTime || c->IsGM() )
	{
		if( ( RandomNum( 0, 19 ) + c->GetIntelligence() ) > 45 ) // not always
		{
			targChar->SetMana( targChar->GetMana() - decrease ); // decrease mana
			updateStats( targChar, 1 ); // update
			sysmessage( s, 972 );
			sysmessage( calcSocketObjFromChar( targChar ), 973 );
			c->SetSkillDelay( BuildTimeValue( cwmWorldState->ServerData()->GetServerSkillDelayStatus() ) );
			Network->PushConn();
			for( cSocket *jSock = Network->FirstSocket(); !Network->FinishedSockets(); jSock = Network->NextSocket() )
			{
				if( s != jSock && charInRange( s, jSock ) )
					npcEmote( jSock, targChar, 974, true, targChar->GetName() );
			}
			Network->PopConn();
		}
		else
			sysmessage( s, 975 );
	}
	else
		sysmessage( s, 976 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  SI16 cSkills::FindOreType( UI16 colour )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Find ore color
//o---------------------------------------------------------------------------o
SI16 cSkills::FindOreType( UI16 colour )
{
	for( SI16 i = 0; i < ores.size(); i++ )
	{
		if( ores[i].colour == colour )
			return i;
	}
	return -1;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Smith( cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player attempts to smith an item
//o---------------------------------------------------------------------------o
void cSkills::Smith( cSocket *s )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::Smith() Invalid socket" );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	CItem *packnum = getPack( mChar );
	
	if( packnum == NULL ) 
	{
		sysmessage( s, 773 ); 
		return; 
	}

	CItem *i = calcItemObjFromSer( s->GetDWord( 7 ) );
	make_st mMake = s->ItemMake();
	if( i != NULL )
	{
		if( i->GetID() >= 0x1BE3 && i->GetID() <= 0x1BF9 )	// is it an ingot?
		{
			SI16 oreType = FindOreType( i->GetColour() );
		if( oreType == -1 )
		{
			mMake.material1 = 0x00;
			sysmessage( s, 977 );
			return;
		}
		mMake.material1 = i->GetID();
		s->ItemMake( mMake );
		if( getPackOwner( i ) != mChar )
			sysmessage( s, 978, ores[oreType].name.c_str() );
		else
			AnvilTarget( s, (*i), oreType );
		mMake.material1 = 0x00;
		s->ItemMake( mMake );
		}
		else	// something we might repair?
		{
			RepairMetal( s );
		}
		return;
	}
	sysmessage( s, 979 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::AnvilTarget( cSocket *s, CItem& item, SI16 oreType )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Unknown : Smithy
//o---------------------------------------------------------------------------o
void cSkills::AnvilTarget( cSocket *s, CItem& item, SI16 oreType )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::AnvilTarget() Invalid socket with item %i and oreType %i", item.GetSerial(), oreType );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	
	int xOffset = MapRegion->GetGridX( mChar->GetX() );
	int yOffset = MapRegion->GetGridY( mChar->GetY() );

	make_st mMake = s->ItemMake();
	UI08 worldNumber = mChar->WorldNumber();
	for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
	{
		for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
		{
			SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
			if( MapArea == NULL )	// no valid region
				continue;
			CItem *tempItem;
			MapArea->PushItem();
			for( tempItem = MapArea->FirstItem(); !MapArea->FinishedItems(); tempItem = MapArea->GetNextItem() )
			{
				if( tempItem == NULL )
					continue;
				if( tempItem->GetID() == 0x0FAF || tempItem->GetID() == 0x0FB0 )
				{
					if( itemInRange( mChar, tempItem, 3 ) )
					{
						int getAmt = getItemAmt( mChar, item.GetID(), item.GetColour() );     
						if( getAmt < ores[oreType].minAmount )
						{ 
							sysmessage( s, 980, ores[oreType].name.c_str() );
							MapArea->PopItem();
							return;
						}
						mMake.has = getAmt;
						s->ItemMake( mMake );
						NewMakeMenu( s, ores[oreType].makemenu, BLACKSMITHING );
						mChar->SetAddPart( oreType );
						MapArea->PopItem();
						return;
					}
				}
			}
			MapArea->PopItem();
		}
	}
	sysmessage( s, 981 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  cSkills::cSkills( void )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Default constructor
//o---------------------------------------------------------------------------o
cSkills::cSkills( void )
{
	ores.resize( 0 );
}
cSkills::~cSkills( void )
{
	ores.resize( 0 );
	skillMenus.erase( skillMenus.begin(), skillMenus.end() );
	itemsForMenus.erase( itemsForMenus.begin(), itemsForMenus.end() );
	actualMenus.erase( actualMenus.begin(), actualMenus.end() );
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool cSkills::LoadMiningData( void )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Load mining resource stuff from scripts
//o---------------------------------------------------------------------------o
bool cSkills::LoadMiningData( void )
{
	// Let's first get our ore list, in SECTION ORE_LIST
	ScriptSection *oreList = FileLookup->FindEntry( "ORE_LIST", skills_def );

	if( oreList == NULL )
		return false;
	std::vector< std::string > oreNameList;
	const char *tag = NULL;
	const char *data = NULL;
	for( tag = oreList->First(); !oreList->AtEnd(); tag = oreList->Next() )
		oreNameList.push_back( tag );
	if( oreNameList.size() == 0 )
		return false;

	ScriptSection *individualOre = NULL;
	for( int counter = 0; counter < oreNameList.size(); counter++ )
	{
		individualOre = FileLookup->FindEntry( oreNameList[counter].c_str(), skills_def );
		if( individualOre == NULL )
			continue;	// couldn't find section, SKIP OVER
		miningData toAdd;
		toAdd.colour = 0;
		toAdd.foreign = true;
		toAdd.makemenu = 0;
		toAdd.minAmount = 3;
		toAdd.minSkill = 0;
		toAdd.name = oreNameList[counter];
		for( tag = individualOre->First(); !individualOre->AtEnd(); tag = individualOre->Next() )
		{
			data = individualOre->GrabData();
			switch( tag[0] )	// break on tag
			{
			case 'c':
			case 'C':	
				if( !stricmp( tag, "COLOUR" ) )
					toAdd.colour = makeNum( data );
				break;
			case 'f':
			case 'F':
				if( !stricmp( tag, "FOREIGN" ) )
					toAdd.foreign = ( makeNum( data ) != 0 );
				break;
			case 'm':
			case 'M':
				if( !stricmp( tag, "MAKEMENU" ) )
					toAdd.makemenu = makeNum( data );
				else if( !stricmp( tag, "MINAMOUNT" ) )
					toAdd.minAmount = makeNum( data );
				else if( !stricmp( tag, "MINSKILL" ) )
					toAdd.minSkill = makeNum( data );
				break;
			case 'n':
			case 'N':
				if( !stricmp( tag, "NAME" ) )
					toAdd.name = data;
				break;
			default:
				Console << "Unknown mining tag " << tag << " with data " << data << " in SECTION " << oreNameList[counter].c_str() << myendl;
				break;
			}
		}
		ores.push_back( toAdd );

	}
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Load( void )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Load mining stuff from scripts
//o---------------------------------------------------------------------------o
void cSkills::Load( void )
{
	Console << "Loading custom ore data        ";

	if( !LoadMiningData() )
	{	// we need to setup our default ores and stuff here!
		miningData miningstuff;
		miningstuff.minAmount = 3;		
		miningstuff.foreign = false;	miningstuff.minSkill = 0;	miningstuff.name = "Iron";	miningstuff.colour = 0;	miningstuff.makemenu = 1;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 850;	miningstuff.name = "Gold";	miningstuff.colour = 0x045E;	miningstuff.makemenu = 50;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 750;	miningstuff.name = "Copper";miningstuff.colour = 0x0641;	miningstuff.makemenu = 814;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 790;	miningstuff.name = "Silver";miningstuff.colour = 0x038A;	miningstuff.makemenu = 813;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 900;	miningstuff.name = "Agapite";miningstuff.colour = 0x0150;	miningstuff.makemenu = 806;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 650;	miningstuff.name = "Adamantium";miningstuff.colour = 0x0386;	miningstuff.makemenu = 800;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 950;	miningstuff.name = "Verite";miningstuff.colour = 0x022F;	miningstuff.makemenu = 802;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 800;	miningstuff.name = "Bronze";miningstuff.colour = 0x02E7;	miningstuff.makemenu = 801;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 700;	miningstuff.name = "Merkite";miningstuff.colour = 0x02C3;	miningstuff.makemenu = 804;
		ores.push_back( miningstuff );
		miningstuff.foreign = true;		miningstuff.minSkill = 990;	miningstuff.name = "Mythril";miningstuff.colour = 0x0191;	miningstuff.makemenu = 803;
		ores.push_back( miningstuff );
	}
	Console.PrintDone();
	
	Console << "Loading creation menus         ";	
	LoadCreateMenus();
	Console.PrintDone();

	Console << "Loading/initializing resources ";
	LoadResourceData();
	Console.PrintDone();

	Console.PrintSectionBegin();
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::SaveResources( void )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Saves resource system info
//o---------------------------------------------------------------------------o
void cSkills::SaveResources( void )
{
	char resourceFile[MAX_PATH];

	sprintf(resourceFile, "%s%s", cwmWorldState->ServerData()->GetSharedDirectory(), "resource.bin");

	FILE *toWrite = fopen( resourceFile, "wb" );
	if( toWrite != NULL )
	{
		for( UI16 gridX = 0; gridX < 610; gridX++ )
		{
			for( UI16 gridY = 0; gridY < 410; gridY++ )
				fwrite( &resources[gridX][gridY], sizeof( resourceEntry ), 1, toWrite );
		}
		fclose( toWrite );
	}
	else
	{
		// Can't save resources
		Console.Error( 1, "Failed to open resource.bin for writing" );
	}
} 

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::LoadResourceData( void )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Loads resource system info
//o---------------------------------------------------------------------------o
void cSkills::LoadResourceData( void )
{
	UI32 resOre = cwmWorldState->ServerData()->GetResOre();
	UI32 resLog = cwmWorldState->ServerData()->GetResLogs();
	UI32 oreTime = BuildTimeValue( cwmWorldState->ServerData()->GetResOreTime() );
	UI32 logTime = BuildTimeValue( cwmWorldState->ServerData()->GetResLogTime() );
	char resourceFile[MAX_PATH];

	sprintf(resourceFile, "%s%s", cwmWorldState->ServerData()->GetSharedDirectory(), "resource.bin");

	FILE *binData = fopen( resourceFile, "rb" );

	bool fileExists = ( binData != NULL );

	resourceEntry toRead;
	for( SI16 gridX = 0; gridX < 610; gridX++ )
	{
		for( SI16 gridY = 0; gridY < 410; gridY++ )
		{
			if( fileExists )
			{
				fread( &toRead, sizeof( resourceEntry ), 1, binData );
				resources[gridX][gridY].oreAmt = toRead.oreAmt;
				resources[gridX][gridY].logAmt = toRead.logAmt;
			}
			else
			{
				resources[gridX][gridY].oreAmt  = resOre;
				resources[gridX][gridY].logAmt  = resLog;
			}
			// No need to preserve time.  Do a refresh automatically
			resources[gridX][gridY].oreTime = oreTime;
			resources[gridX][gridY].logTime = logTime;
		}
	}
	if( fileExists )
		fclose( binData );
}

//o---------------------------------------------------------------------------o
//|   Function    :  int cSkills::GetNumberOfOres( void )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Returns number of custom ores
//o---------------------------------------------------------------------------o
int cSkills::GetNumberOfOres( void )
{
	return ores.size();
}

//o---------------------------------------------------------------------------o
//|   Function    :  miningData *cSkills::GetOre( int number )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Returns a handle to the data about the ore
//o---------------------------------------------------------------------------o
miningData *cSkills::GetOre( int number )
{
	if( number < 0 || number >= ores.size() )
		return NULL;
	return &ores[number];
}

//o---------------------------------------------------------------------------o
//|   Function    :  miningData *cSkills::GetOre( string name )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Returns a handle to the data about the ore based on it's name
//o---------------------------------------------------------------------------o
miningData *cSkills::GetOre( std::string name )
{
	for( int i = 0; i < ores.size(); i++ )
	{
		if( ores[i].name == name )
			return &ores[i];
	}
	return NULL;
}

//o---------------------------------------------------------------------------o
//|   Function    :  int cSkills::GetOreIndex( string name )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Returns the index of the ore of name name
//o---------------------------------------------------------------------------o
int cSkills::GetOreIndex( std::string name )
{
	for( int i = 0; i < ores.size(); i++ )
	{
		if( ores[i].name == name )
			return i;
	}
	return -1;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::MakeOre( int Region, CChar *actor, cSocket *s )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Spawn Ore in players pack when he successfully mines
//o---------------------------------------------------------------------------o
void cSkills::MakeOre( int Region, CChar *actor, cSocket *s )
{
	cTownRegion *targRegion = region[Region];
	if( targRegion == NULL || actor == NULL )
		return;
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::MakeOre() Invalid socket in region %i by %i", Region, actor->GetSerial() );
		return;
	}
	UI16 getSkill = actor->GetSkill( MINING );
	int oreChance = RandomNum( 0, targRegion->GetOreChance() );	// find our base ore
	int currentOre = 0, sumChance = 0;
	bool oreFound = false;
	const orePref *toFind = NULL;
	miningData *found = NULL;
	for( currentOre = 0; currentOre < targRegion->GetNumOrePreferences(); currentOre++ )
	{
		toFind = targRegion->GetOrePreference( currentOre );
		if( toFind == NULL )
			continue;
		if( oreChance >= sumChance && oreChance < ( sumChance + toFind->percentChance ) )
		{
			found = GetOre( toFind->oreIndex );
			if( found != NULL )
			{
				if( getSkill >= found->minSkill )
				{
					char temp[32];
					sprintf( temp, "%s ore", found->name.c_str() );
					Items->SpawnItem( s, 1, temp, true, 0x19B9, found->colour, true, true );
					sysmessage( s, 982, temp );
					oreFound = true;
				}
			}
		}
		if( sumChance > oreChance )
		{
			found = GetOre( toFind->oreIndex );
			if( found != NULL )
			{
				if( getSkill >= found->minSkill )
				{
					char temp[32];
					sprintf( temp, "%s ore", found->name.c_str() );
					Items->SpawnItem( s, 1, temp, true, 0x19B9, found->colour, true, true );
					sysmessage( s, 982, temp );
					oreFound = true;
				}
			}
		}
		if( oreFound )
			break;
		sumChance += toFind->percentChance;
	}
	if( !oreFound || ( oreFound && found == NULL ) )
	{
		if( getSkill >= 850 )
		{
			SpawnRandomItem( s, true, "necro.scp", "ITEMLIST", "999" ); 
			sysmessage( s, 983 );
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::LoadCreateMenus( void )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Load Create menus from create.scp
//o---------------------------------------------------------------------------o
void cSkills::LoadCreateMenus( void )
{
	ScpList *toScan = FileLookup->GetFiles( create_def );
	if( toScan == NULL )
		return;
	ScriptSection *createScpMenu = NULL;	// each entry in the create.scp file
	ScriptSection *toSearch = NULL;			// data in a particular section
	const char *tag = NULL;						// entry tag
	const char *data = NULL;						// entry data
	char *numStart = NULL;					// pointer to our entry number
	UI16 ourEntry;							// our actual entry number
	char tempID[12];
	for( SI32 mc = 0; mc < toScan->size(); mc++ )
	{
		if( (*toScan)[mc] == NULL )
			continue;
		Script *ourScript = (*toScan)[mc];
		for( createScpMenu = ourScript->FirstEntry(); createScpMenu != NULL; createScpMenu = ourScript->NextEntry() )
		{
			const char *eName = ourScript->EntryName();
			if( strstr( eName, "SUBMENU" ) )	// is it a menu? (not really SUB, just to avoid picking up MAKEMENUs)
			{
				toSearch = createScpMenu;
				numStart = strchr( eName, ' ' );
				ourEntry = makeNum( numStart );
				for( tag = toSearch->First(); !toSearch->AtEnd(); tag = toSearch->Next() )
				{
					data = toSearch->GrabData();
					if( !stricmp( tag, "MENU" ) )
						actualMenus[ourEntry].menuEntries.push_back( makeNum( data ) );
					else if( !stricmp( tag, "ITEM" ) )
						actualMenus[ourEntry].itemEntries.push_back( makeNum( data ) );
				}
			}
			else if( strstr( eName, "ITEM" ) )	// is it an item?
			{
				toSearch = createScpMenu;
				numStart = strchr( eName, ' ' );
				ourEntry = makeNum( numStart );
				createEntry tmpEntry;
				tmpEntry.minRank = 0;
				tmpEntry.maxRank = 10;
				tmpEntry.colour = 0;
				tmpEntry.targID = 1;
				tmpEntry.soundPlayed = 1;

				for( tag = toSearch->First(); !toSearch->AtEnd(); tag = toSearch->Next() )
				{
					data = toSearch->GrabData();
					if( !stricmp( tag, "COLOUR" ) )
						tmpEntry.colour = makeNum( data );
					else if( !stricmp( tag, "ID" ) )
						tmpEntry.targID = makeNum( data );
					else if( !stricmp( tag, "MINRANK" ) )
						tmpEntry.minRank = makeNum( data );
					else if( !stricmp( tag, "MAXRANK" ) )
						tmpEntry.maxRank = makeNum( data );
					else if( !stricmp( tag, "NAME" ) )
						tmpEntry.name = data;
					else if( !stricmp( tag, "SOUND" ) )
						tmpEntry.soundPlayed = makeNum( data );
					else if( !stricmp( tag, "ADDITEM" ) )
						tmpEntry.addItem = data;
					else if( !stricmp( tag, "DELAY" ) )
						tmpEntry.delay = makeNum( data );
					else if( !stricmp( tag, "RESOURCE" ) )
					{
						resAmountPair tmpResource;
						numStart = strchr( data, ' ' );
						if( numStart == NULL )
						{
							tmpResource.amountNeeded = 1;
							tmpResource.colour = 0;
							tmpResource.itemID = makeNum( data );
						}
						else
						{
							char *sndSpace = NULL;
							sndSpace = strchr( numStart + 1, ' ' );
							if( sndSpace == NULL )
							{
								tmpResource.amountNeeded = makeNum( numStart );
								tmpResource.colour = 0;
							}
							else
							{
								strncpy( tempID, numStart, (sndSpace - numStart) );
								tempID[sndSpace-numStart] = 0;
								tmpResource.colour = makeNum( tempID );
								tmpResource.amountNeeded = makeNum( sndSpace );
							}
							strncpy( tempID, data, (numStart - data) );
							tempID[numStart-data] = 0;
							tmpResource.itemID = makeNum( tempID );
						}
						tmpEntry.resourceNeeded.push_back( tmpResource );
					}
					else if( !stricmp( tag, "SKILL" ) )
					{
						resSkillReq tmpResource;
						numStart = strchr( data, ' ' );
						if( numStart == NULL )
						{
							tmpResource.maxSkill = 1000;
							tmpResource.minSkill = 0;
						}
						else
						{
							char *secondSpace = NULL;
							secondSpace = strchr( numStart+1, ' ' );
							if( secondSpace == NULL )
								tmpResource.maxSkill = 1000;
							else
								tmpResource.maxSkill = makeNum( secondSpace );
							if( secondSpace == NULL )
								tmpResource.minSkill = makeNum( data );
							else
							{
								strncpy( tempID, numStart, (secondSpace - numStart) );
								tempID[secondSpace-numStart] = 0;
								tmpResource.minSkill = makeNum( tempID );
							}

						}
						if( numStart == NULL )
							tmpResource.skillNumber = (UI08)makeNum( data );
						else
						{
							strncpy( tempID, data, (numStart - data) );
							tempID[numStart-data] = 0;
							tmpResource.skillNumber = (UI08)makeNum( tempID );
						}
						tmpEntry.skillReqs.push_back( tmpResource );
					}
					itemsForMenus[ourEntry] = tmpEntry;
				}
			}
			else if( strstr( eName, "MENUENTRY" ) )
			{
				toSearch = createScpMenu;
				numStart = strchr( eName, ' ' );
				ourEntry = makeNum( numStart );
				for( tag = toSearch->First(); !toSearch->AtEnd(); tag = toSearch->Next() )
				{
					data = toSearch->GrabData();
					if( !stricmp( "ID", tag ) )
						skillMenus[ourEntry].targID = makeNum( data );
					else if( !stricmp( "COLOUR", tag ) )
						skillMenus[ourEntry].colour = makeNum( data );
					else if( !stricmp( "NAME", tag ) )
						skillMenus[ourEntry].name = data;
					else if( !stricmp( "SUBMENU", tag ) )
						skillMenus[ourEntry].subMenu = makeNum( data );
				}
			}
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool cSkills::AdvanceSkill( CChar *s, int sk, bool skillUsed )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Advance players skill based on success or failure in CheckSkill()
//o---------------------------------------------------------------------------o
bool cSkills::AdvanceSkill( CChar *s, int sk, bool skillUsed )
{
	bool advSkill = false;
	SI16 skillGain;
	
	SI08 skillAdvance = FindSkillPoint( sk, s->GetBaseSkill( sk ) );
	
	if( skillUsed )
		skillGain = ( skill[sk].advancement[skillAdvance].success ) * 10;
	else
		skillGain = ( skill[sk].advancement[skillAdvance].failure ) * 10;

	if( skillGain > RandomNum( 0, 1000 ) )
	{
		advSkill = true;
		if( RandomNum( 0, 99 ) <= 10 )
		{
			if( s->GetSkillLock( sk ) == 0 && s->GetBaseSkill( sk ) > 0 )
				s->SetBaseSkill( s->GetBaseSkill( sk ) + 1, sk );
		}
		else if( s->GetSkillLock( sk ) == 0 )
			Atrophy( s, sk );
	}
	
	if( s->GetSkillLock( sk ) != 2 ) // if it's locked, stats can't advance
		AdvanceStats( s, sk, skillUsed );
	return advSkill;
}

//o---------------------------------------------------------------------------o
//|   Function    :  SI08 cSkills::FindSkillPoint( int sk, int value )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Unknown
//o---------------------------------------------------------------------------o
SI08 cSkills::FindSkillPoint( int sk, int value )
{
	SI08 retVal = -1;
	for( int iCounter = 0; iCounter < skill[sk].advancement.size() - 1; iCounter++ )
	{
		if( skill[sk].advancement[iCounter].base <= value && value < skill[sk].advancement[iCounter+1].base )
		{
			retVal = iCounter;
			break;
		}
	}
	if( retVal == -1 )
		retVal = skill[sk].advancement.size() - 1;
	return retVal;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::AdvanceStats( CChar *s, int sk, bool skillsuccess )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Advance players stats
//o---------------------------------------------------------------------------o
void cSkills::AdvanceStats( CChar *s, int sk, bool skillsuccess ) 
{ 
    CRace *pRace = Races->Race( s->GetRace() ); 
	
    // If the Race is invalid just use the default race
    if( pRace == NULL )
		pRace = Races->Race( 0 );
	
    bool IsPlayer = ( !s->IsNpc() && s->GetCommandLevel() < CNSCMDLEVEL ); 
	if ( !IsPlayer ) return;
	
    //make sure socket is no npc nor counsi/gm
    bool update = false;

    UI32 ServStatCap = cwmWorldState->ServerData()->GetServerStatCapStatus(); 
    UI32 ttlStats = s->ActualStrength() + s->ActualDexterity() + s->ActualIntelligence(); 
    SI16 chanceStatGain = 0; //16bit because of freaks that raises it > 100 
    int StatCount, nCount; 
    int u = 0;
	int maxChance = 1000;
    SI16 ActualStat[3] = { s->ActualStrength() , s->ActualDexterity() , s->ActualIntelligence() }; 
    UI16 StatModifier[3] = { skill[sk].strength , skill[sk].dexterity , skill[sk].intelligence }; 
	
	UI16 skillUpdTrig = s->GetScriptTrigger();
	cScript *skillTrig = Trigger->GetScript( skillUpdTrig );
	
    for ( StatCount = STRENGTH; StatCount <= INTELLECT; StatCount++ ) 
	{ 
		nCount = StatCount - ALLSKILLS - 1;  

		//  the following will calculate the chances for str/dex/int to increase 
		//
		//  it is divided into 2 "dices":
		//  first dice: get the success-skillpoint of the stat out of skills.dfn in dependence
		//				of the racial statcap for this stat... x means stat is x% of statcap
		//				=> get skillpoint for x
		//  sec. dice:	get the chance for this stat to increase when the used skill has been used
		//				(out of skills.dfn) => get x = statmodifier of skill

		//  last make it a integer between 0 and 1000 normally (negative or 0==no chance) 

		//	special dice 1: the stat wont increase above x% of the racial statcap. x% is equivalent to dice 2.
		//  special dice 2: skill failed: decrease chance by 50%

		//  k, first let us calculate both dices
		chanceStatGain = (SI16)                               
			(((float)skill[StatCount-1].advancement[ FindSkillPoint( StatCount-1, (int)( (float)ActualStat[nCount] / (float)pRace->Skill( StatCount ) * 100 ) ) ].success / 100) 
			* 
			((float)( (float)(StatModifier[nCount]) / 10 ) / 100) * 1000); 
		// some mathematics in it ;) 

		// now, lets implement the special dice 1 and additionally check for onStatGain javascript method
		if( StatModifier[nCount] <= (int)( (float)ActualStat[nCount] / (float)pRace->Skill( StatCount ) * 100 ) )
			chanceStatGain = 0;

		// special dice 2
		if( !skillsuccess )
			maxChance = maxChance * 2;
		
		if ( ActualStat[nCount] < pRace->Skill( StatCount ) && chanceStatGain > RandomNum( 0, maxChance ) ) // if stat of char < racial statcap and chance for statgain > random number from 0 to 100 
		{ 
			switch( StatCount ) 
			{ 
			case STRENGTH: 
				s->IncStrength(); 
				break; 
			case DEXTERITY: 
				s->IncDexterity(); 
				break; 
			case INTELLECT: 
				s->IncIntelligence(); 
				break; 
			default: 
				break; 
			} 
			update = true;
			
			if( skillTrig != NULL )
			{
				if( !skillTrig->OnStatGained( s, StatCount, sk ) )
					skillTrig->OnStatChange( s, StatCount );
			}
			
			break;//only one stat at a time fellas
		}
	}
	
	
	/*if total stats (before gain) is above a random number from 0 to the stat cap 
	(hence the closer we get to the cap, the more likely, and at or above the cap, 
	always) then this statement will execute.  This statement removes a stat from one
	of the other stats... just like OSI... so it makes it real hard to get to the cap.
	ALSO this prevents char's stats from FREEZEING when they reach the cap*/
	
	//k will only take away from other stats if within 10 of the server stat cap
	if( ttlStats >= RandomNum( ServStatCap-10, ServStatCap ) )
	{
		UI08 skillDrop = 0;
		
		switch( StatCount )
		{
		case INTELLECT:
			if( RandomNum( 0, 1 ) ) 
			{
				if( ActualStat[0] > 10 )
					skillDrop = STRENGTH;
				else if( ActualStat[1] > 10 )
					skillDrop = DEXTERITY;
				else
					skillDrop = INTELLECT;
			}
			else
			{
				if( ActualStat[1] > 10 )
					skillDrop = DEXTERITY;
				else if( ActualStat[0] > 10 )
					skillDrop = STRENGTH;
				else
					skillDrop = INTELLECT;
			}
			break;
		case DEXTERITY:
			if( RandomNum( 0, 1 ) ) 
			{
				if( ActualStat[0] > 10 )
					skillDrop = STRENGTH;
				else if( ActualStat[2] > 10 )
					skillDrop = INTELLECT;
				else
					skillDrop = DEXTERITY;
			}
			else
			{
				if( ActualStat[2] > 10 )
					skillDrop = INTELLECT;
				else if( ActualStat[0] > 10 )
					skillDrop = STRENGTH;
				else
					skillDrop = DEXTERITY;
			}
			break;
		case STRENGTH:
			if( RandomNum( 0, 1 ) ) 
			{
				if( ActualStat[1] > 10 )
					skillDrop = DEXTERITY;
				else if( ActualStat[2] > 10 )
					skillDrop = INTELLECT;
				else
					skillDrop = STRENGTH;
			} 
			else 
			{
				if( ActualStat[2] > 10 )
					skillDrop = INTELLECT;
				else if( ActualStat[1] >10 )
					skillDrop = DEXTERITY;
				else
					skillDrop = STRENGTH;
			}
			break;
		}
		
		if( update )
		{
			switch( skillDrop )
			{
			case STRENGTH:	s->DecStrength();			break;
			case DEXTERITY:	s->DecDexterity();			break;
			case INTELLECT:	s->DecIntelligence();		break;
			}
			
			if( skillTrig != NULL )
			{
				if( !skillTrig->OnStatLoss( s, skillDrop ) )
					skillTrig->OnStatChange( s, skillDrop );
			}
		}
	}
	
	cSocket *jSock = calcSocketObjFromChar( s );
	if( jSock != NULL )
	{
		statwindow( jSock, s );
		for( int i = 0; i < TRUESKILLS; i++ )
			updateSkillLevel( s, i );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::NewMakeMenu( cSocket *s, int menu, UI08 skill )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  New make menu system, based on create.scp
//o---------------------------------------------------------------------------o
void cSkills::NewMakeMenu( cSocket *s, int menu, UI08 skill )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::NewMakeMenu() Invalid socket using skill %i to make menu %i", skill, menu );
		return;
	}
	CChar *ourChar = s->CurrcharObj();;
	ourChar->SetMaking( menu );
	std::map< UI16, createMenu >::iterator p = actualMenus.find( menu );
	if( p == actualMenus.end() )
		return;

	createMenu ourMenu = p->second;
	int iCounter;
	stringList one, two;	
	char tempString[128];
	one.push_back( "noclose" );
	sprintf( tempString, "resizepic 0 0 %i 400 320", cwmWorldState->ServerData()->GetBackgroundPic() );
	one.push_back( tempString );
	one.push_back( "page 0" );
	one.push_back( "text 200 20 0 0" );
	two.push_back( "Create menu" );
	sprintf( tempString, "button 40 300 %i %i 1 0 1", cwmWorldState->ServerData()->GetButtonCancel(), cwmWorldState->ServerData()->GetButtonCancel() + 1 );
	one.push_back( tempString );
	UI16 xLoc = 60, yLoc = 40;
	std::map< UI16, createEntry >::iterator iIter;
	std::map< UI16, createMenuEntry >::iterator iMenuIter;
	int actualItems = 0;
	int textCounter = 1;
	for( iCounter = 0; iCounter < ourMenu.itemEntries.size(); iCounter++ )
	{
		if( (actualItems%6) == 0 && actualItems != 0 )
		{
			xLoc += 180;
			yLoc = 40;
		}
		iIter = itemsForMenus.find( ourMenu.itemEntries[iCounter] );
		if( iIter != itemsForMenus.end() )
		{
			createEntry iItem = iIter->second;
			bool canMake = true;
			for( int sCounter = 0; sCounter < iItem.skillReqs.size() && canMake; sCounter++ )
			{
				UI08 skillNum = iItem.skillReqs[sCounter].skillNumber;
				int ourSkill = ourChar->GetSkill( skillNum );
				int minSkill = iItem.skillReqs[sCounter].minSkill;
				int maxSkill = iItem.skillReqs[sCounter].maxSkill;
				//canMake = ( ourSkill >= minSkill && ourSkill <= maxSkill * 1.5 );
				canMake = ( ourSkill >= minSkill );
				// it was not thought that way, its not logical, maxSkill should say, that you can get maxRank with maxSkill and higher
			}
			if( canMake )
			{
				sprintf( tempString, "button %i %i %i %i 1 0 %i", xLoc - 40, yLoc, cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1, 10 + iCounter );
				one.push_back( tempString );
				sprintf( tempString, "tilepic %i %i %i", xLoc - 20, yLoc, iItem.targID );
				one.push_back( tempString );
				sprintf( tempString, "text %i %i 35 %i", xLoc + 20, yLoc, textCounter++ );
				one.push_back( tempString );
				two.push_back( iItem.name );
				yLoc += 40;
				actualItems++;
			}
		}
	}

	for( iCounter = 0; iCounter < ourMenu.menuEntries.size(); iCounter++ )
	{
		if( (actualItems%6) == 0 && actualItems != 0 )
		{
			xLoc += 180;
			yLoc = 40;
		}
		iMenuIter = skillMenus.find( ourMenu.menuEntries[iCounter] );
		if( iMenuIter != skillMenus.end() )
		{
			createMenuEntry iMenu = iMenuIter->second;
			sprintf( tempString, "button %i %i %i %i 1 0 %i", xLoc - 40, yLoc, cwmWorldState->ServerData()->GetButtonRight(), cwmWorldState->ServerData()->GetButtonRight() + 1, 100 + iCounter );
			one.push_back( tempString );
			sprintf( tempString, "tilepic %i %i %i", xLoc - 20, yLoc, iMenu.targID );
			one.push_back( tempString );
			sprintf( tempString, "text %i %i 35 %i", xLoc + 20, yLoc, textCounter++ );
			one.push_back( tempString );
			two.push_back( iMenu.name );
			yLoc += 40;
			actualItems++;
		}
	}
	SendVecsAsGump( s, one, two, 12, INVALIDSERIAL );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::HandleMakeMenu( cSocket *s, int button, int menu )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Handles the button pressed in the new make menu
//o---------------------------------------------------------------------------o
void cSkills::HandleMakeMenu( cSocket *s, int button, int menu )
{
	if( s == NULL )
	{
		Console.Error( 0, "cSkills::HandleMakeMenu() Invalid socket hit button %i on menu %i", button, menu );
		return;
	}
	CChar *ourChar = s->CurrcharObj();
	ourChar->SetMaking( 0 );
	std::map< UI16, createMenu >::iterator p = actualMenus.find( menu );
	if( p == actualMenus.end() )
		return;
	createMenu ourMenu = p->second;
	if( button >= 100 )	// menu pressed
	{
		std::map< UI16, createMenuEntry >::iterator q = skillMenus.find( ourMenu.menuEntries[button-100] );
		if( q == skillMenus.end() )
			return;
		NewMakeMenu( s, q->second.subMenu, 0 );
	}
	else				// item to make
	{
		std::map< UI16, createEntry >::iterator r = itemsForMenus.find( ourMenu.itemEntries[button-10] );
		if( r == itemsForMenus.end() )
			return;
		MakeItem( r->second, ourChar, s, ourMenu.itemEntries[button - 10] );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    :  createEntry *cSkills::FindItem( UI16 itemNum )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Returns a handle to a createEntry based on an itemNum
//o---------------------------------------------------------------------------o
createEntry *cSkills::FindItem( UI16 itemNum )
{
	std::map< UI16, createEntry >::iterator r = itemsForMenus.find( itemNum );
	if( r == itemsForMenus.end() )
		return NULL;
	return &(r->second);
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::MakeItem( createEntry &toMake, CChar *player, cSocket *sock )
//|   Date        :  Unknown
//|   Programmer  :  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     :  Makes an item selected in the new make menu system
//o---------------------------------------------------------------------------o
void cSkills::MakeItem( createEntry &toMake, CChar *player, cSocket *sock, UI16 itemEntry )
{
	if( sock == NULL )
	{
		Console.Error( 0, "cSkills::MakeItem() Invalid socket with player %i making %s(%s)", player, toMake.name.c_str(), toMake.addItem.c_str() );
		return;
	}
	bool canMake = true;
	// we need to check ALL skills, even if the first one fails
	if( player->GetCommandLevel() < CNSCMDLEVEL )
	{
		for( int sCounter = 0; sCounter < toMake.skillReqs.size(); sCounter++ )
		{
			if( !CheckSkill( player, toMake.skillReqs[sCounter].skillNumber, toMake.skillReqs[sCounter].minSkill, toMake.skillReqs[sCounter].maxSkill ) )
				canMake = false;
		}
	}
	int resCounter;
	UI16 toDelete;
	UI16 targColour;
	UI16 targID;
	if( !canMake )
	{
		// delete anywhere up to half of the resources needed
		for( resCounter = 0; resCounter < toMake.resourceNeeded.size(); resCounter++ )
		{
			toDelete = RandomNum( 0, toMake.resourceNeeded[resCounter].amountNeeded / 2 );
			targColour = toMake.resourceNeeded[resCounter].colour;
			targID = toMake.resourceNeeded[resCounter].itemID;
			deleItemAmt( player, targID, targColour, toDelete );
		}
		soundeffect( sock, toMake.soundPlayed, true );
		sysmessage( sock, 984 );
	}
	else
	{
		bool canDelete = true;
		for( resCounter = 0; resCounter < toMake.skillReqs.size(); resCounter++ )
		{
			if( player->SkillUsed( toMake.skillReqs[resCounter].skillNumber ) )
			{
				sysmessage( sock, 1650 );
				return;
			}
		}
		for( resCounter = 0; resCounter < toMake.resourceNeeded.size(); resCounter++ )
		{
			toDelete = toMake.resourceNeeded[resCounter].amountNeeded;
			targColour = toMake.resourceNeeded[resCounter].colour;
			targID = toMake.resourceNeeded[resCounter].itemID;
			if( getItemAmt( player, targID, targColour ) < toDelete )
				canDelete = false;
		}
		if( !canDelete )
		{
			sysmessage( sock, 1651 );
			return;
		}
		// Delete our resources up front
		for( resCounter = 0; resCounter < toMake.resourceNeeded.size(); resCounter++ )
		{
			toDelete = toMake.resourceNeeded[resCounter].amountNeeded;
			targColour = toMake.resourceNeeded[resCounter].colour;
			targID = toMake.resourceNeeded[resCounter].itemID;
			deleItemAmt( player, targID, targColour, toDelete );
		}
		for( resCounter = 0; resCounter < toMake.skillReqs.size(); resCounter++ )
			player->SkillUsed( true, toMake.skillReqs[resCounter].skillNumber );
		tempeffect( player, player, 41, toMake.delay, itemEntry, 0 );
		if( toMake.delay > 300 )
		{
			for( int i = 0; i < ( toMake.delay / 300 ); i++ )
				tempeffect( player, player, 42, 300 * i, toMake.soundPlayed, 0 );
		}
	}
}

//o---------------------------------------------------------------------------o
//|		Function	:	void cSkills::RepairMetal( cSocket *s )
//|		Date		:	October 16, 2000
//|		Programmer	:	Thaliq
//|		Modified	:	November 13, 2001 - BelCMC, changed to a metal repair function
//o---------------------------------------------------------------------------o
//|		Purpose     :	Repair armor and weapons.
//o---------------------------------------------------------------------------o

void cSkills::RepairMetal( cSocket *s )
{
	SI16 minSkill = 999, maxSkill = 1000;
	CItem *j = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( j == NULL || !j->IsMetalType() )
	{
		sysmessage( s, 986 );
		return;
	}

	if( j->GetHP() < j->GetMaxHP() )
	{
		if( j->GetDef() > 0 )
		{
			// Items with > 12 def would have impossible skill req's, with this equation
			if( j->GetDef() <= 12 )
			{
				// Minimum Skill = 61.0 + Defense - 1 / 3 * 100 (0-3 = 61.0, 4-6 = 71.0, ect)
				minSkill = (610 + (int)((j->GetDef() - 1) / 3) * 100);
				// Maximum Skill = 84.9 + Defense - 1 / 3 * 50 (0-3 = 84.9, 4-6 = 89.9, ect)
				maxSkill = (849 + (int)((j->GetDef() - 1) / 3) * 50);
			}
		}
		else if( j->GetHiDamage() > 0 )
		{
			int offset = ( j->GetLoDamage() + j->GetHiDamage() ) / 2;
			// Items with > 25 Avg Damage would have impossible skill req's, with this equation
			if( offset <= 25 )
			{
				// Minimum Skill = 51.0 + Avg Damage - 1 / 5 * 100 (0-5 = 51.0, 6-10 = 61.0, ect)
				minSkill = (510 + (int)((offset - 1) / 5 ) * 100 );
				// Maximum Skill = 79.9 + Avg Damage - 1 / 5 * 50 (0-5 = 79.9, 6-10 = 84.9, ect)
				maxSkill = (799 + (int)((offset-1) / 5) * 50);
			}
		}
		else
		{
			sysmessage( s, 987 );
			return;
		}
	}
	else
	{
		sysmessage( s, 988 );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	if( CheckSkill( mChar, BLACKSMITHING, minSkill, maxSkill ) )
	{
		j->SetHP( j->GetMaxHP() );
		sysmessage( s, 989 );
		soundeffect( s, 0x002A, true );
	}
	else
		sysmessage( s, 990 );
}

//o---------------------------------------------------------------------------o
//|		Function	:	void cSkills::RepairLeather( cSocket *s )
//|		Date		:	October 16, 2000
//|		Programmer	:	Thaliq
//|		Modified	:	November 13, 2001 - BelCMC, changed to a metal repair function
//o---------------------------------------------------------------------------o
//|		Purpose     :	Repair armor and weapons.
//o---------------------------------------------------------------------------o

void cSkills::RepairLeather( cSocket *s )
{
	SI16 minSkill = 999, maxSkill = 1000;
	CItem *j = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( j == NULL || !j->IsLeatherType() )
	{
		sysmessage( s, 986 );
		return;
}

	if( j->GetHP() < j->GetMaxHP() )
	{
		if( j->GetDef() > 0 )
		{
			// Items with > 12 def would have impossible skill req's, with this equation
			if( j->GetDef() <= 12 )
			{
				// Minimum Skill = 61.0 + Defense - 1 / 3 * 100 (0-3 = 61.0, 4-6 = 71.0, ect)
				minSkill = (610 + (int)((j->GetDef() - 1) / 3) * 100);
				// Maximum Skill = 84.9 + Defense - 1 / 3 * 50 (0-3 = 84.9, 4-6 = 89.9, ect)
				maxSkill = (849 + (int)((j->GetDef() - 1) / 3) * 50);
			}
		}
		else if( j->GetHiDamage() > 0 )
		{
			int offset = ( j->GetLoDamage() + j->GetHiDamage() ) / 2;
			// Items with > 25 Avg Damage would have impossible skill req's, with this equation
			if( offset <= 25 )
			{
				// Minimum Skill = 51.0 + Avg Damage - 1 / 5 * 100 (0-5 = 51.0, 6-10 = 61.0, ect)
				minSkill = (510 + (int)((offset - 1) / 5 ) * 100 );
				// Maximum Skill = 79.9 + Avg Damage - 1 / 5 * 50 (0-5 = 79.9, 6-10 = 84.9, ect)
				maxSkill = (799 + (int)((offset-1) / 5) * 50);
			}
		}
		else
		{
			sysmessage( s, 987 );
			return;
		}
	}
	else
	{
		sysmessage( s, 988 );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	if( CheckSkill( mChar, TAILORING, minSkill, maxSkill ) )
	{
		j->SetHP( j->GetMaxHP() );
		sysmessage( s, 989 );
		soundeffect( s, 0x002A, true );
	}
	else
		sysmessage( s, 990 );
}

//o---------------------------------------------------------------------------o
//|		Function	:	void cSkills::RepairBow( cSocket *s )
//|		Date		:	October 16, 2000
//|		Programmer	:	Thaliq
//|		Modified	:	November 13, 2001 - BelCMC, changed to a metal repair function
//o---------------------------------------------------------------------------o
//|		Purpose     :	Repair armor and weapons.
//o---------------------------------------------------------------------------o

void cSkills::RepairBow( cSocket *s )
{
	SI16 minSkill = 999, maxSkill = 1000;
	CItem *j = calcItemObjFromSer( s->GetDWord( 7 ) );
	if( j == NULL || !j->IsBowType() )
	{
		sysmessage( s, 986 );
		return;
	}

	if( j->GetHP() < j->GetMaxHP() )
	{
		if( j->GetDef() > 0 )
		{
			// Items with > 12 def would have impossible skill req's, with this equation
			if( j->GetDef() <= 12 )
			{
				// Minimum Skill = 61.0 + Defense - 1 / 3 * 100 (0-3 = 61.0, 4-6 = 71.0, ect)
				minSkill = (610 + (int)((j->GetDef() - 1) / 3) * 100);
				// Maximum Skill = 84.9 + Defense - 1 / 3 * 50 (0-3 = 84.9, 4-6 = 89.9, ect)
				maxSkill = (849 + (int)((j->GetDef() - 1) / 3) * 50);
			}
		}
		else if( j->GetHiDamage() > 0 )
		{
			int offset = ( j->GetLoDamage() + j->GetHiDamage() ) / 2;
			// Items with > 25 Avg Damage would have impossible skill req's, with this equation
			if( offset <= 25 )
			{
				// Minimum Skill = 51.0 + Avg Damage - 1 / 5 * 100 (0-5 = 51.0, 6-10 = 61.0, ect)
				minSkill = (510 + (int)((offset - 1) / 5 ) * 100 );
				// Maximum Skill = 79.9 + Avg Damage - 1 / 5 * 50 (0-5 = 79.9, 6-10 = 84.9, ect)
				maxSkill = (799 + (int)((offset-1) / 5) * 50);
			}
		}
		else
		{
			sysmessage( s, 987 );
			return;
		}
	}
	else
	{
		sysmessage( s, 988 );
		return;
	}
	CChar *mChar = s->CurrcharObj();
	if( CheckSkill( mChar, BOWCRAFT, minSkill, maxSkill ) )
	{
		j->SetHP( j->GetMaxHP() );
		sysmessage( s, 989 );
		soundeffect( s, 0x002A, true );
	}
	else
		sysmessage( s, 990 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cSkills::Snooping( cSocket *s, CChar *target, SERIAL serial )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Called when player snoops another PC/NPC's or a tamed animals pack
//o---------------------------------------------------------------------------o
void cSkills::Snooping( cSocket *s, CChar *target, SERIAL serial ) 
{
	CChar *mChar = s->CurrcharObj();
	char temp[64];

	cSocket *tSock = calcSocketObjFromChar( target );

	if( target->GetCommandLevel() > mChar->GetCommandLevel() )
	{
		sysmessage( s, 991 );
		sysmessage( tSock, 992, mChar->GetName() );
		return;
	}
	
	if( CheckSkill( mChar, SNOOPING, 0, 1000 ) )
	{
		openPack( s, serial );
		sysmessage( s, 993 );
		cScript *successSnoop = Trigger->GetScript( target->GetScriptTrigger() );
		if( successSnoop != NULL )
			successSnoop->OnSnooped( target, mChar, true );
	}
	else 
	{
		bool doNormal = true;
		cScript *failSnoop = Trigger->GetScript( target->GetScriptTrigger() );
		if( failSnoop != NULL )
			doNormal = !failSnoop->OnSnooped( target, mChar, true );

		if( doNormal )
		{
			sysmessage( s, 991 );
			if( target->IsNpc() )
			{
				if( isHuman( target ) )
				{
					strcpy( temp, Dictionary->GetEntry( 994 + RandomNum( 0, 2 ) ) );
					npcTalk( s, target, temp, false );
					if( cwmWorldState->ServerData()->GetSnoopIsCrime() )
					{
						if( RandomNum( 0, 100 ) < 50 && mChar->IsCriminal() )	//	50% chance of calling guards, on second time
							callGuards( target, mChar );
					}
				}
				else
					playMonsterSound( target, target->GetID(), SND_IDLE );	// Play idle sound, if not human
			}
			else
				sysmessage( tSock, 997, mChar->GetName() );
			if( cwmWorldState->ServerData()->GetSnoopIsCrime() )
				criminal( mChar );
			if( mChar->GetKarma() <= 1000 )
			{
				mChar->SetKarma( mChar->GetKarma() - 10 );
				sysmessage( s, 998 );
			}
		}
	}
}
