#include "uox3.h"
#include "townregion.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "regions.h"

using namespace std::string_literals;

cEffects *Effects;

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::PlaySound()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays sound effect for player, echo's to all players if allHear is true
//o------------------------------------------------------------------------------------------------o
void cEffects::PlaySound( CSocket *mSock, UI16 soundId, bool allHear )
{
	if( mSock == nullptr )
		return;

	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ))
		return;

	CPPlaySoundEffect toSend = ( *mChar );
	toSend.Model( soundId );

	if( allHear )
	{		
		auto nearbyChars = FindNearbyPlayers( mChar );
		std::for_each( nearbyChars.begin(), nearbyChars.end(), [&toSend]( CSocket *entry )
		{
			entry->Send( &toSend );
		});
	}
	mSock->Send( &toSend );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::PlaySound()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays sound effect originating from object for all players nearby(default) or originator only
//o------------------------------------------------------------------------------------------------o
void cEffects::PlaySound( CBaseObject *baseObj, UI16 soundId, bool allHear )
{
	if( !ValidateObject( baseObj ))
		return;

	CPPlaySoundEffect toSend = ( *baseObj );
	toSend.Model( soundId );

	if( allHear )
	{
		auto  nearbyChars = FindPlayersInVisrange( baseObj );
		std::for_each( nearbyChars.begin(), nearbyChars.end(), [&toSend]( CSocket *entry )
		{
			entry->Send( &toSend );
		});
	}
	else
	{
		if( baseObj->GetObjType() == OT_CHAR )
		{
			CSocket *mSock = ( static_cast<CChar *>( baseObj ))->GetSocket();
			if( mSock != nullptr )
			{
				mSock->Send( &toSend );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::ItemSound()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Play item drop sound based on ID (Gems, Gold, or Default)
//o------------------------------------------------------------------------------------------------o
void cEffects::ItemSound( CSocket *s, CItem *item, bool allHear )
{
	UI16 itemId = item->GetId();
	if( itemId >= 0x0F0F && itemId <= 0x0F20 )		// Large Gem Stones
	{
		PlaySound( s, 0x0034, allHear );
	}
	else if( itemId >= 0x0F21 && itemId <= 0x0F30 )	// Small Gem Stones
	{
		PlaySound( s, 0x0033, allHear );
	}
	else if( itemId >= 0x0EEA && itemId <= 0x0EF2 )	// Gold
	{
		GoldSound( s, item->GetAmount(), allHear );
	}
	else
	{
		UI16 effectId = 0x0042;
		CBaseObject *getCont = item->GetCont();
		if( getCont != nullptr )
		{
			if( getCont->GetObjType() == OT_ITEM )
			{
				CItem *iCont = static_cast<CItem *>( getCont );
				switch( iCont->GetId() )
				{
					case 0x0E75:	// backpack
					case 0x0E76:	// leather bag
					case 0x0E79:	// pouch
					case 0x09B0:	// pouch
					case 0x2AF8:	// Shopkeeper buy, sell and sold layers
					case 0x2256:	// bagball
					case 0x2257:	// bagball
						effectId = 0x0048;
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
						effectId = 0x004F;
						break;
					default:
						break;
				}
			}
			else
			{
				effectId = 0x0048;
			}
		}
		PlaySound( s, effectId, allHear );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::GoldSound()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Play gold drop sound based on Amount
//o------------------------------------------------------------------------------------------------o
void cEffects::GoldSound( CSocket *s, UI32 goldTotal, bool allHear )
{
	if( goldTotal <= 1 )
	{
		PlaySound( s, 0x0035, allHear );
	}
	else if( goldTotal < 6 )
	{
		PlaySound( s, 0x0036, allHear );
	}
	else
	{
		PlaySound( s, 0x0037, allHear );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::PlayDeathSound()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Send death sound to all sockets in range
//o------------------------------------------------------------------------------------------------o
void cEffects::PlayDeathSound( CChar *i )
{
	if( i->GetOrgId() == 0x0191 || i->GetOrgId() == 0x025E || i->GetOrgId() == 0x029B || i->GetOrgId() == 0x00B8 || i->GetOrgId() == 0x00BA || i->GetId() == 0x02EF ) // Female Death (human/elf/gargoyle/savage)
	{
		UI16 deathSound = 0x0150 + RandomNum( 0, 3 );
		PlaySound( i, deathSound );
	}
	else if( i->GetOrgId() == 0x0190 || i->GetOrgId() == 0x025D || i->GetOrgId() == 0x029A || i->GetOrgId() == 0x00B7 || i->GetOrgId() == 0x00B9 || i->GetOrgId() == 0x02EE ) // Male Death (human/elf/gargoyle/savage)
	{
		UI16 deathSound = 0x015A + RandomNum( 0, 3 );
		PlaySound( i, deathSound );
	}
	else
	{
		const UI16 toPlay = cwmWorldState->creatures[i->GetOrgId()].GetSound( SND_DIE );
		if( toPlay != 0x00 )
		{
			Effects->PlaySound( i, toPlay );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::PlayMusic()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Plays midi/mp3 in client
//o------------------------------------------------------------------------------------------------o
void cEffects::PlayMusic( CSocket *s, UI16 number )
{
	CPPlayMusic toSend( number );
	s->Send( &toSend );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::PlayBGSound()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Play background sounds based on location
//o------------------------------------------------------------------------------------------------o
void cEffects::PlayBGSound( CSocket& mSock, CChar& mChar )
{
	std::vector<CChar *> inRange;
	inRange.reserve( 11 );

	for( auto &MapArea : MapRegion->PopulateList( &mChar ))
	{
		if( MapArea )
		{
			auto regChars = MapArea->GetCharList();
			for( const auto &tempChar : regChars->collection() )
			{
				if( ValidateObject( tempChar ) && !tempChar->IsFree() && ( tempChar->GetInstanceId() == mChar.GetInstanceId() ))
				{
					if( tempChar->IsNpc() && !tempChar->IsDead() && !tempChar->IsAtWar() && CharInRange(( &mChar ), tempChar ))
					{
						inRange.push_back( tempChar );
					}
					
					if( inRange.size() == 11 )
					{
						break;
					}
				}
			}
		}
	}

	UI16 basesound = 0;
	if( !inRange.empty() )
	{
		CChar *soundSrc = inRange[RandomNum( static_cast<size_t>( 0 ), inRange.size() - 1 )];
		UI16 xx	= soundSrc->GetId();
		if( cwmWorldState->creatures.find( xx ) == cwmWorldState->creatures.end() )
			return;

		//Play creature sounds, but add a small chance that they won't, to give players a break every now and then
		UI08 soundChance = static_cast<UI08>( RandomNum( static_cast<size_t>( 0 ), inRange.size() + 9 ));
		if( soundChance > 5 )
		{
			basesound = cwmWorldState->creatures[xx].GetSound( SND_IDLE );
			if( basesound != 0x00 )
			{
				CPPlaySoundEffect toSend = ( *soundSrc );
				toSend.Model( basesound );
				mSock.Send( &toSend );
			}
		}
	}
	else // play random mystic-sounds also if no creature is in range
	{
		if( RandomNum( 0, 3332 ) == 33 )
		{
			switch( RandomNum( 0, 6 ))
			{
				case 0: basesound = 595;	break; // gnome sound
				case 1: basesound = 287;	break; // bigfoot 1
				case 2: basesound = 288;	break; // bigfoot 2
				case 3: basesound = 639;	break; // old snake sound
				case 4: basesound = 179;	break; // lion sound
				case 5: basesound = 246;	break; // mystic
				case 6: basesound = 253;	break; // mystic II
				default:					break;
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

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::DoSocketMusic()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends message to client to play specific midi/mp3
//o------------------------------------------------------------------------------------------------o
auto cEffects::DoSocketMusic( CSocket *s ) -> void
{
	// Return if the socket is not valid
	if( s == nullptr )
		return;

	SI32 i = 0;
	char musicArray[50];
	std::string sect;

	CChar *mChar = s->CurrcharObj();

	CTownRegion *mReg = nullptr;

	// Fetch subregion player is in, if any
	auto subRegionNum = mChar->GetSubRegion();
	if( subRegionNum != 0 )
	{
		if( cwmWorldState->townRegions.find( subRegionNum ) != cwmWorldState->townRegions.end() )
		{
			mReg = cwmWorldState->townRegions[subRegionNum];
		}
	}

	if( mReg == nullptr )
	{
		// Otherwise, fetch the actual region player is in
		mReg = mChar->GetRegion();
		if( mReg == nullptr )
			return;
	}

	UI16 musicList = mReg->GetMusicList();
	if( musicList != 0 )
	{
		if( mChar->IsDead() )
		{
			sect = "MUSICLIST DEATH";
		}
		else if( mChar->IsAtWar() )
		{
			sect = "MUSICLIST COMBAT";
		}
		else
		{
			sect = std::string( "MUSICLIST " ) + oldstrutil::number( musicList );
		}
		
		auto MusicList = FileLookup->FindEntry( sect, regions_def );
		if( MusicList )
		{
			for( const auto &sec : MusicList->collection() )
			{
				if( oldstrutil::upper( sec->tag ) == "MUSIC" )
				{
					musicArray[i++] = static_cast<SI08>( std::stoi( sec->data, nullptr, 0 ));
				}
			}
			if( i != 0 )
			{
				i = RandomNum( 0, i - 1 );
				PlayMusic( s, musicArray[i] );
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	cEffects::PlayTileSound()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Play a sound based on the tile character is on
//o------------------------------------------------------------------------------------------------o
auto cEffects::PlayTileSound( CChar *mChar, CSocket *mSock ) -> void
{
	if( !ValidateObject( mChar ))
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
	auto artwork = Map->ArtAt( mChar->GetX(), mChar->GetY(), mChar->WorldNumber() );
	std::sort( artwork.begin(), artwork.end(), []( const Tile_st &lhs, const Tile_st &rhs )
	{
		return ( lhs.altitude + lhs.artInfo->Height() ) < ( rhs.altitude + rhs.artInfo->Height() );
	});

	// now find the static that works for us
	// start from the bottom of the vector, as we want the first one that is below us
	auto alt = mChar->GetZ(); // so we dont keep calling it
	auto iter = std::find_if( artwork.rbegin(), artwork.rend(), [alt]( const Tile_st &value )
	{
		return alt >= ( value.altitude + value.artInfo->Height() );
	});

	if( iter != artwork.rend() )
	{
		if( iter->artInfo->CheckFlag( TF_WET ))
		{
			tileType = TT_WATER;
		}
		if( iter->artInfo->CheckFlag( TF_SURFACE) || iter->artInfo->CheckFlag( TF_CLIMBABLE ))
		{
			auto pos = iter->artInfo->Name().find( "wood" );
			if( pos != std::string::npos )
			{
				tileType = TT_WOODEN;
			}
			else
			{
				pos = iter->artInfo->Name().find( "ston" );
				if( pos != std::string::npos )
				{
					tileType = TT_STONE;
				}
				else
				{
					pos = iter->artInfo->Name().find( "gras" );
					if( pos != std::string::npos )
					{
						tileType = TT_GRASS;
					}
				}
			}
		}
	}

	UI16 soundId = 0;
	switch( mChar->GetStep() )	// change step info
	{
		case 0:
			if(( !isRunning && !onHorse) || ( isRunning && onHorse ))
			{
				mChar->SetStep( 1 );	// step 2
			}
			else if( isRunning || ( !isRunning && onHorse ))
			{
				mChar->SetStep( 2 );	// Running step 2
			}
			switch( tileType )
			{
				case TT_NORMAL:
					if( onHorse )
					{
						if( isRunning )
						{
							soundId = 0x012A;
						}
						else
						{
							soundId = 0x024C;
						}
					}
					else
					{
						soundId = 0x012B;
					}
					break;
				case TT_WATER:	// water
					break;
				case TT_STONE: // stone
					soundId = 0x0130;
					break;
				case TT_WOODEN: // wooden
					soundId = 0x0123;
					break;
				case TT_OTHER: // other
					[[fallthrough]];
				case TT_GRASS: // grass
					soundId = 0x012D;
					break;
			}
			break;
		case 1:
			if( !isRunning || ( isRunning && onHorse ))
			{
				mChar->SetStep( 0 );	// step 1
			}
			else if( isRunning || ( !isRunning && onHorse ))
			{
				mChar->SetStep( 3 );	// Running step 4
			}
			switch( tileType )
			{
				case TT_NORMAL:
					if( onHorse )
					{
						if( isRunning )
						{
							soundId = 0x0129;
						}
						else
						{
							soundId = 0x024B;
						}
					}
					else
					{
						soundId = 0x012C;
					}
					break;
				case TT_WATER:	// water
					break;
				case TT_STONE: // stone
					soundId = 0x012F;
					break;
				case TT_WOODEN: // wooden
					soundId = 0x0122;
					break;
				case TT_OTHER: // other
					[[fallthrough]];
				case TT_GRASS: // grass
					soundId = 0x012E;
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
			mChar->SetStep( 1 ); // pause
			break;
	}

	if( soundId )			// if we have a valid sound
	{
		if( mSock == nullptr && ValidateObject( mChar ))
		{
			// It's an NPC!
			auto nearbyChars = FindNearbyPlayers( mChar );
			std::for_each( nearbyChars.begin(), nearbyChars.end(), [soundId, this]( CSocket *entry )
			{
				PlaySound( entry, soundId, false );
			});
			return;
		}
		else
		{
			// It's a player!
			PlaySound( mSock, soundId, true );
		}
	}
}

