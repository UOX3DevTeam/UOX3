#include "uox3.h"
#include "townregion.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "regions.h"


cEffects *Effects;

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PlaySound( CSocket *mSock, UI16 soundID, bool allHear )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays sound effect for player, echo's to all players if allHear is true
//o-----------------------------------------------------------------------------------------------o
void cEffects::PlaySound( CSocket *mSock, UI16 soundID, bool allHear )
{
	if( mSock == nullptr )
		return;
	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	CPPlaySoundEffect toSend = (*mChar);
	toSend.Model( soundID );

	if( allHear )
	{
		SOCKLIST nearbyChars = FindNearbyPlayers( mChar );
		for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
		{
			(*cIter)->Send( &toSend );
		}
	}
	mSock->Send( &toSend );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PlaySound( CBaseObject *baseObj, UI16 soundID, bool allHear )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays sound effect originating from object for all players nearby(default) or originator only
//o-----------------------------------------------------------------------------------------------o
void cEffects::PlaySound( CBaseObject *baseObj, UI16 soundID, bool allHear )
{
	if( !ValidateObject( baseObj ) )
		return;
	CPPlaySoundEffect toSend = (*baseObj);
	toSend.Model( soundID );

	if( allHear )
	{
		SOCKLIST nearbyChars = FindPlayersInVisrange( baseObj );
		for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
		{
			(*cIter)->Send( &toSend );
		}
	}
	else
	{
		if( baseObj->GetObjType() == OT_CHAR )
		{
			CSocket *mSock = (static_cast<CChar *>(baseObj))->GetSocket();
			if( mSock != nullptr )
				mSock->Send( &toSend );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void itemSound( CSocket *s, CItem *item, bool allHear )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Play item drop sound based on ID (Gems, Gold, or Default)
//o-----------------------------------------------------------------------------------------------o
void cEffects::itemSound( CSocket *s, CItem *item, bool allHear )
{
	UI16 itemID = item->GetID();
	if( itemID >= 0x0F0F && itemID <= 0x0F20 )		// Large Gem Stones
		PlaySound( s, 0x0034, allHear );
	else if( itemID >= 0x0F21 && itemID <= 0x0F30 )	// Small Gem Stones
		PlaySound( s, 0x0033, allHear );
	else if( itemID >= 0x0EEA && itemID <= 0x0EF2 )	// Gold
		goldSound( s, item->GetAmount(), allHear );
	else
	{
		UI16 effectID = 0x0042;
		CBaseObject *getCont = item->GetCont();
		if( getCont != nullptr )
		{
			if( getCont->GetObjType() == OT_ITEM )
			{
				CItem *iCont = static_cast<CItem *>(getCont);
				switch( iCont->GetID() )
				{
					case 0x0E75:	// backpack
					case 0x0E76:	// leather bag
					case 0x0E79:	// pouch
					case 0x09B0:	// pouch
					case 0x2AF8:	// Shopkeeper buy, sell and sold layers
					case 0x2256:	// bagball
					case 0x2257:	// bagball
						effectID = 0x0048;
						break;
					case 0x0E7A:	// picnic basket
					case 0x24D5:	// SE basket
					case 0x24D6:	// SE basket
					case 0x24D9:	// SE basket
					case 0x24DA:	// SE basket
					case 0x0990:	// basket
					case 0x09AC:	// bushel
					case 0x09B1:	// basket
					case 0x24D7:	// SE basket
					case 0x24D8:	// SE basket
					case 0x24DD:	// SE basket
					case 0x24DB:	// SE basket
					case 0x24DC:	// SE basket
						effectID = 0x004F;
						break;
				}
			}
			else
				effectID = 0x0048;
		}
		PlaySound( s, effectID, allHear );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void goldSound( CSocket *s, UI32 goldtotal, bool allHear )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Play gold drop sound based on Amount
//o-----------------------------------------------------------------------------------------------o
void cEffects::goldSound( CSocket *s, UI32 goldtotal, bool allHear )
{
	if( goldtotal <= 1 )
		PlaySound( s, 0x0035, allHear );
	else if( goldtotal < 6 )
		PlaySound( s, 0x0036, allHear );
	else
		PlaySound( s, 0x0037, allHear );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void playDeathSound( CChar *i )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Send death sound to all sockets in range
//o-----------------------------------------------------------------------------------------------o
void cEffects::playDeathSound( CChar *i )
{
	if( i->GetOrgID() == 0x0191 || i->GetOrgID() == 0x025E || i->GetOrgID() == 0x029B || i->GetOrgID() == 0x00B8 || i->GetOrgID() == 0x00BA || i->GetID() == 0x02EF ) // Female Death (human/elf/gargoyle/savage)
	{
		UI16 deathSound = 0x0150 + RandomNum( 0, 3 );
		PlaySound( i, deathSound );
	}
	else if( i->GetOrgID() == 0x0190 || i->GetOrgID() == 0x025D || i->GetOrgID() == 0x029A || i->GetOrgID() == 0x00B7 || i->GetOrgID() == 0x00B9 || i->GetOrgID() == 0x02EE ) // Male Death (human/elf/gargoyle/savage)
	{
		UI16 deathSound = 0x015A + RandomNum( 0, 3 );
		PlaySound( i, deathSound );
	}
	else
	{
		const UI16 toPlay = cwmWorldState->creatures[i->GetOrgID()].GetSound( SND_DIE );
		if( toPlay != 0x00 )
			Effects->PlaySound( i, toPlay );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void playMusic( CSocket *s, UI16 number )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays midi/mp3 in client
//o-----------------------------------------------------------------------------------------------o
void cEffects::playMusic( CSocket *s, UI16 number )
{
	CPPlayMusic toSend( number );
	s->Send( &toSend );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void PlayBGSound( CSocket& mSock, CChar& mChar )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Play background sounds based on location
//o-----------------------------------------------------------------------------------------------o
void cEffects::PlayBGSound( CSocket& mSock, CChar& mChar )
{
	std::vector< CChar * > inrange;
	inrange.reserve( 11 );

	REGIONLIST nearbyRegions = MapRegion->PopulateList( (&mChar) );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		CMapRegion *MapArea = (*rIter);
		if( MapArea == nullptr )	// no valid region
			continue;
		GenericList< CChar * > *regChars = MapArea->GetCharList();
		regChars->Push();
		for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
		{
			if( !ValidateObject( tempChar ) || tempChar->isFree() || tempChar->GetInstanceID() != mChar.GetInstanceID() )
				continue;
			if( tempChar->IsNpc() && !tempChar->IsDead() && !tempChar->IsAtWar() && charInRange( (&mChar), tempChar ) )
				inrange.push_back( tempChar );

			if( inrange.size() == 11 )
				break;
		}
		regChars->Pop();
	}

	UI16 basesound = 0;
	if( !inrange.empty() )
	{
		CChar *soundSrc = inrange[RandomNum( static_cast<size_t>(0), inrange.size() - 1 )];
		UI16 xx			= soundSrc->GetID();
		if( cwmWorldState->creatures.find( xx ) == cwmWorldState->creatures.end() )
			return;

		//Play creature sounds, but add a small chance that they won't, to give players a break every now and then
		UI08 soundChance = static_cast<UI08>(RandomNum( static_cast<size_t>(0), inrange.size() + 9  ));
		if( soundChance > 5 )
		{
			basesound = cwmWorldState->creatures[xx].GetSound( SND_IDLE );
			if( basesound != 0x00 )
			{
				CPPlaySoundEffect toSend = (*soundSrc);
				toSend.Model( basesound );
				mSock.Send( &toSend );
			}
		}
	}
	else // play random mystic-sounds also if no creature is in range
	{
		if( RandomNum( 0, 3332 ) == 33 )
		{
			switch( RandomNum( 0, 6 ) )
			{
				case 0: basesound = 595;	break; // gnome sound
				case 1: basesound = 287;	break; // bigfoot 1
				case 2: basesound = 288;	break; // bigfoot 2
				case 3: basesound = 639;	break; // old snake sound
				case 4: basesound = 179;	break; // lion sound
				case 5: basesound = 246;	break; // mystic
				case 6: basesound = 253;	break; // mystic II
			}
			if( basesound != 0 )
			{
				CPPlaySoundEffect toSend = mChar;
				toSend.Model( basesound );
				mSock.Send( &toSend );
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void doSocketMusic( CSocket *s )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends message to client to play specific midi/mp3
//o-----------------------------------------------------------------------------------------------o
void cEffects::doSocketMusic( CSocket *s )
{
	SI32 i = 0;
	char musicArray[50];
	std::string sect;

	CChar *mChar = s->CurrcharObj();

	CTownRegion *mReg = mChar->GetRegion();
	if( mReg == nullptr )
	{
		return;
	}

	UI16 musicList = mReg->GetMusicList();
	if( musicList == 0 )
	{
		return;
	}

	if( mChar->IsAtWar() )
	{
		sect = "MUSICLIST COMBAT";
	}
	else
	{
		sect = std::string("MUSICLIST ") + strutil::number( musicList );
	}

	ScriptSection *MusicList = FileLookup->FindEntry( sect, regions_def );
	if( MusicList == nullptr )
	{
		return;
	}
	std::string data;
	for( std::string tag = MusicList->First(); !MusicList->AtEnd(); tag = MusicList->Next() )
	{
		data = MusicList->GrabData();
		if( strutil::toupper( tag ) == "MUSIC" )
		{
			musicArray[i++] = static_cast<SI08>(std::stoi(data, nullptr, 0));
		}
	}
	if( i != 0 )
	{
		i = RandomNum( 0, i - 1 );
		playMusic( s, musicArray[i] );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void playTileSound( CSocket *mSock )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Play a sound based on the tile character is on
//o-----------------------------------------------------------------------------------------------o
void cEffects::playTileSound( CChar *mChar, CSocket *mSock )
{
	if( !ValidateObject( mChar ) )
		return;

	if( mChar->GetVisible() != VT_VISIBLE || ( mChar->IsGM() || mChar->IsCounselor() ))
		return;

	if( mChar->IsFlying() ) // No footstep sounds for flying gargoyles
		return;

	enum TileType
	{
		TT_NORMAL = 0,
		TT_WATER,
		TT_STONE,
		TT_OTHER,
		TT_WOODEN,
		TT_GRASS
	};
	TileType tileType = TT_NORMAL;

	bool onHorse = mChar->IsOnHorse();
	bool isRunning = ( mChar->GetRunning() > 0 );

	CStaticIterator msi(  mChar->GetX(), mChar->GetY(), mChar->WorldNumber() );
	Static_st *stat = msi.Next();

	if( stat )
	{
		CTile& tile = Map->SeekTile( stat->itemid );
		if( tile.CheckFlag( TF_WET ) )
			tileType = TT_WATER;
		else if( tile.CheckFlag( TF_SURFACE) || tile.CheckFlag( TF_CLIMBABLE ) )
		{
			char search1[10];
			strcpy( search1, "wood" );
			if( strstr( tile.Name(), search1 ) )
				tileType = TT_WOODEN;
			strcpy( search1, "ston" );
			if( strstr( tile.Name(), search1 ) )
				tileType = TT_STONE;
			strcpy( search1, "gras" );
			if( strstr( tile.Name(), search1 ) )
				tileType = TT_GRASS;
		}
	}

	UI16 soundID = 0;
	switch( mChar->GetStep() )	// change step info
	{
		case 0:
			if( !isRunning || ( isRunning && onHorse ) )
				mChar->SetStep( 1 );	// step 2
			else if( isRunning || ( !isRunning && onHorse ) )
				mChar->SetStep( 2 );	// Running step 2
			switch( tileType )
			{
				case TT_NORMAL:
					if( onHorse )
					{
						if( isRunning )
							soundID = 0x012A;
						else		
							soundID = 0x024C;
					}
					else
						soundID = 0x012B;
					break;
				case TT_WATER:	// water
					break;
				case TT_STONE: // stone
					soundID = 0x0130;
					break;
				//case TT_OTHER: // other
				case TT_WOODEN: // wooden
					soundID = 0x0123;
					break;
				case TT_GRASS: // grass
					soundID = 0x012D;
					break;
			}
			break;
		case 1:
			if( !isRunning || ( isRunning && onHorse ) )
				mChar->SetStep( 0 );	// step 1
			else if( isRunning || ( !isRunning && onHorse ) )
				mChar->SetStep( 3 );	// Running step 4
			switch( tileType )
			{
				case TT_NORMAL:
					if( onHorse )
					{
						if( isRunning )
							soundID = 0x0129;
						else
							soundID = 0x024B;
					}
					else
						soundID = 0x012C;
					break;
				case TT_WATER:	// water
					break;
				case TT_STONE: // stone
					soundID = 0x012F;
					break;
				//case TT_OTHER: // other
				case TT_WOODEN: // wooden
					soundID = 0x0122;
					break;
				case TT_GRASS: // grass
					soundID = 0x012E;
					break;
			}
			break;
		case 2:
			mChar->SetStep( 1 ); // Running step 3
			break;
		case 3:
			mChar->SetStep( 0 ); // Running step 0
			break;
		default:
			mChar->SetStep( 1 );	// pause
			break;
	}

	if( soundID )			// if we have a valid sound
	{
		if( mSock == nullptr && ValidateObject( mChar ) )
		{
			// It's an NPC!
			SOCKLIST nearbyChars = FindNearbyPlayers( mChar );
			for( SOCKLIST_CITERATOR cIter = nearbyChars.begin(); cIter != nearbyChars.end(); ++cIter )
			{
				PlaySound( (*cIter), soundID, false );
			}
			return;
		}
		else
		{
			// It's a player!
			PlaySound( mSock, soundID, true );
		}
	}
}

