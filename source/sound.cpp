#include "uox3.h"
#include "townregion.h"
#include "cRaces.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "regions.h"

namespace UOX
{

cEffects *Effects;

//o--------------------------------------------------------------------------o
//|	Function		-	void cEffects::PlaySound( CSocket *mSock, UI16 soundID, bool allHear )
//|	Date			-	Unknown
//|	Developers		-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Plays sound effect for player, echo's to all players if allHear is true
//o--------------------------------------------------------------------------o
void cEffects::PlaySound( CSocket *mSock, UI16 soundID, bool allHear )
{
	if( mSock == NULL ) 
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

//o--------------------------------------------------------------------------o
//|	Function		-	void cEffects::PlaySound( CBaseObject *baseObj, UI16 soundID, bool allHear )
//|	Date			-	Unknown
//|	Developers		-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Plays sound effect originating from object for all players nearby(default) or originator only
//o--------------------------------------------------------------------------o
void cEffects::PlaySound( CBaseObject *baseObj, UI16 soundID, bool allHear )
{
	assert( baseObj );
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
			CSocket *mSock = calcSocketObjFromChar( (CChar *)baseObj );
			if( mSock != NULL )
				mSock->Send( &toSend );
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cEffects::doorSound( CItem *item, UI16 id, bool isOpen )
//|	Date			-	Oct 8, 1998
//|	Developers		-	Dupois
//o--------------------------------------------------------------------------o
//|	Description		-	Plays the proper door sfx for doors/gates/secretdoors
//o--------------------------------------------------------------------------o
void cEffects::doorSound( CItem *item, UI16 id, bool isOpen )
{
	const UI16 OPENWOOD		= 0x00EA;
	const UI16 OPENGATE		= 0x00EB;
	const UI16 OPENSTEEL	= 0x00EC;
	const UI16 OPENSECRET	= 0x00ED;
	const UI16 CLOSEWOOD	= 0x00F1;
	const UI16 CLOSEGATE	= 0x00F2;
	const UI16 CLOSESTEEL	= 0x00F3;
	const UI16 CLOSESECRET	= 0x00F4;
	
	if( !isOpen ) // Request open door sfx
	{
		if( ( id >= 0x0695 && id < 0x06C5 ) || // Open wooden / ratan door
			( id >= 0x06D5 && id <= 0x06F4 ) )
			PlaySound( item, OPENWOOD );
		
		if( ( id >= 0x0839 && id <= 0x0848 ) || // Open gate
			( id >= 0x084C && id <= 0x085B ) ||
			( id >= 0x0866 && id <= 0x0875 ) )
			PlaySound( item, OPENGATE );
		
		if( ( id >= 0x0675 && id < 0x0695 ) || // Open metal
			( id >= 0x06C5 && id < 0x06D5 ) )
			PlaySound(item, OPENSTEEL );
		
		if( id >= 0x0314 && id <= 0x0365 ) // Open secret
			PlaySound( item, OPENSECRET );
	}
	else // Request close door sfx
	{
		if( ( id >= 0x0695 && id < 0x06C5 ) || // close wooden / ratan door
			( id >= 0x06D5 && id <= 0x06F4 ) )
			PlaySound( item, CLOSEWOOD );
		
		if( ( id >= 0x0839 && id <= 0x0848 ) || // close gate
			( id >= 0x084C && id <= 0x085B ) ||
			( id >= 0x0866 && id <= 0x0875 ) )
			PlaySound( item, CLOSEGATE );
		
		if( ( id >= 0x0675 && id < 0x0695 ) || // close metal
			( id >= 0x06C5 && id < 0x06D5 ) )
			PlaySound( item, CLOSESTEEL );
		
		if( id >= 0x0314 && id <= 0x0365 ) // close secret
			PlaySound( item, CLOSESECRET );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cEffects::itemSound( CSocket *s, CItem *item, bool allHear )
//|	Programmer	-	Dupois
//o---------------------------------------------------------------------------o
//|	Purpose		-	Play item drop sound based on ID (Gems, Gold, or Default)
//o---------------------------------------------------------------------------o
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
		if( getCont != NULL )
		{
			if( getCont->GetObjType() == OT_ITEM )
			{
				CItem *iCont = (CItem *)getCont;
				switch( iCont->GetID() )
				{
					case 0x0E75:	// backpack
					case 0x0E76:	// leather bag
					case 0x0E79:	// pouch
					case 0x09B0:	// pouch
					case 0x2AF8:	// Shopkeeper buy, sell and sold layers
						effectID = 0x0048;
						break;
				}
			}
			else
				effectID = 0x0048;
		}
		PlaySound( s, effectID, allHear );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cEffects::goldSound( CSocket *s, UI32 goldtotal, bool allHear )
//|	Programmer	-	Dupois
//o---------------------------------------------------------------------------o
//|	Purpose		-	Play gold drop sound based on Amount
//o---------------------------------------------------------------------------o
void cEffects::goldSound( CSocket *s, UI32 goldtotal, bool allHear )
{
	if( goldtotal <= 1 ) 
		PlaySound( s, 0x0035, allHear );
	else if( goldtotal < 6 ) 
		PlaySound( s, 0x0036, allHear );
	else 
		PlaySound( s, 0x0037, allHear );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cEffects::playMonsterSound( CChar *monster, UI16 id, monsterSound sfx )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Play proper sound effect based on monsters skin
//o---------------------------------------------------------------------------o
void cEffects::playMonsterSound( CChar *monster, UI16 id, monsterSound sfx )
{
	UI16 basesound	= cwmWorldState->creatures[id].BaseSound();
	UI08 offset		= sfx;
	if( basesound != 0 )
	{
		switch( cwmWorldState->creatures[id].SoundFlag() )
		{
			case 0: break; // in normal case the offset is correct
			case 1: break; // birds sounds will be implmented later
			case 2:  // only start-attack, attack & dýing sounds available
				switch( sfx )
				{
					case 1:
					case 3:	offset = 0xFF;	break;	// idle, defend
					case 2:	offset = 1;		break; // correct offset
					case 4:	offset = 2;		break;
				}
				break;
			case 3: // only start-attack, attack, defense & dying
				switch( sfx )
				{
					case 1:	offset = 0xFF;		break;
					case 2:
					case 3:
					case 4:	offset = sfx - 1;	break;
				}
				break;
			case 4: // only a single sound
				if( sfx != 0 ) 
					offset = 0xFF; 
				else 
					offset = 0;
				break;
		}
		
		basesound += offset;
		
		if( offset != 0xFF ) 
			PlaySound( monster, basesound );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cEffects::playDeathSound( CChar *i )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send death sound to all sockets in range
//o---------------------------------------------------------------------------o
void cEffects::playDeathSound( CChar *i )
{
	if( i->GetOrgID() == 0x0191 )// Female Death
	{
		switch( RandomNum( 0, 3 ) )
		{
			case 0:		PlaySound( i, 0x0150 );	break;
			case 1:		PlaySound( i, 0x0151 );	break;
			case 2:		PlaySound( i, 0x0152 );	break;
			case 3:		PlaySound( i, 0x0153 );	break;
			default:	PlaySound( i, 0x0150 );	break;
		}
	}
	else if( i->GetOrgID() == 0x0190 )	// Male Death
	{
		switch( RandomNum( 0, 3 ) )
		{
			case 0:		PlaySound( i, 0x015A );	break;
			case 1:		PlaySound( i, 0x015B );	break;
			case 2:		PlaySound( i, 0x015C );	break;
			case 3:		PlaySound( i, 0x015D );	break;
			default:	PlaySound( i, 0x015A );	break;
		}
	}
	else
		playMonsterSound( i, i->GetOrgID(), SND_DIE );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cEffects::playMidi( CSocket *s, UI16 number )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Plays midi in client
//o---------------------------------------------------------------------------o
void cEffects::playMidi( CSocket *s, UI16 number )
{
	CPPlayMusic toSend( number );
	s->Send( &toSend );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cEffects::bgsound( CChar *s )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Play background sounds based on location
//o---------------------------------------------------------------------------o
void cEffects::PlayBGSound( CSocket& mSock, CChar& mChar )
{
	std::vector< CChar * > inrange;
	inrange.reserve( 11 );

	REGIONLIST nearbyRegions = MapRegion->PopulateList( (&mChar) );
	for( REGIONLIST_CITERATOR rIter = nearbyRegions.begin(); rIter != nearbyRegions.end(); ++rIter )
	{
		SubRegion *MapArea = (*rIter);
		if( MapArea == NULL )	// no valid region
			continue;
		CDataList< CChar * > *regChars = MapArea->GetCharList();
		regChars->Push();
		for( CChar *tempChar = regChars->First(); !regChars->Finished(); tempChar = regChars->Next() )
		{
			if( !ValidateObject( tempChar ) || tempChar->isFree() )
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
		if( xx > 2048 )
			return;

		basesound = cwmWorldState->creatures[xx].BaseSound();
		if( basesound != 0 )
		{
			switch( cwmWorldState->creatures[xx].SoundFlag() ) // play only idle sounds, if there arnt any, dont play them !
			{
				case 0:	++basesound;	break;	// normal case -> play idle sound
				case 1:	++basesound;	break;	// birds sounds will be implmented later
				case 2:
				case 3:	basesound = 0;	break;	// no idle sound, so dont play it !
				case 4:					break;	// only a single sound, play it !
			}
			
			if( basesound != 0 ) // bugfix lb
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

//o---------------------------------------------------------------------------o
//|	Function	-	void cEffects::dosocketmidi( CSocket *s )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Send midi to client
//o---------------------------------------------------------------------------o
void cEffects::dosocketmidi( CSocket *s )
{
	int i = 0;
	char midiarray[50];
	UString sect;

	CChar *mChar = s->CurrcharObj();

	CTownRegion *mReg = mChar->GetRegion();
	if( mReg == NULL )
		return;

	UI16 midiList = mReg->GetMidiList();
	if( midiList == 0 )
		return;

	if( mChar->IsAtWar() )
		sect = "MIDILIST COMBAT";
	else
		sect = "MIDILIST " + UString::number( midiList );

	ScriptSection *MidiList = FileLookup->FindEntry( sect, regions_def );
	if( MidiList == NULL )
		return;
	UString data;
	for( UString tag = MidiList->First(); !MidiList->AtEnd(); tag = MidiList->Next() )
	{
		data = MidiList->GrabData();
		if( tag.upper() == "MIDI" )
			midiarray[i++] = data.toByte();
	}
	if( i != 0 )
	{
		i = RandomNum( 0, i - 1 );
		playMidi( s, midiarray[i] );
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cEffects::playTileSound( CSocket *mSock )
//|	Programmer	-	Abaddon
//o---------------------------------------------------------------------------o
//|	Purpose		-	Play a sound based on the tile character is on
//o---------------------------------------------------------------------------o
void cEffects::playTileSound( CSocket *mSock )
{
	if( mSock == NULL )
		return;

	CTile tile;
	char search1[10];
	UI16 sndid = 0;
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
	bool onHorse = false;

	CChar *mChar = mSock->CurrcharObj();
	
	if( mChar->GetVisible() != VT_VISIBLE || mChar->GetCommandLevel() >= CNS_CMDLEVEL )
		return;
	
	if( mChar->IsOnHorse() )
		onHorse = true;
	
	if( mChar->GetStep() == 1 || mChar->GetStep() == 0 )	// if we play a sound
	{
		SI16 x = mChar->GetX();
		SI16 y = mChar->GetY();
		
		MapStaticIterator msi( x, y, mChar->WorldNumber() );
		staticrecord *stat = msi.Next();
		if( stat )
			msi.GetTile(&tile);
	}
	
	if( tile.LiquidWet() )
		tileType = TT_WATER;
	strcpy( search1, "wood" );
	if( strstr( tile.Name(), search1 ) )
		tileType = TT_WOODEN;
	strcpy( search1, "ston" );
	if( strstr( tile.Name(), search1 ) )
		tileType = TT_STONE;
	strcpy( search1, "gras" );
	if( strstr( tile.Name(), search1 ) )
		tileType = TT_GRASS;
	
	switch( mChar->GetStep() )	// change step info
	{
		case 0:
			mChar->SetStep( 3 );	// step 2
			switch( tileType )
			{
				case TT_NORMAL:
					if( onHorse )
						sndid = 0x024C;
					else
						sndid = 0x012B; 
					break;
				case TT_WATER:	// water
					break;
				case TT_STONE: // stone
					sndid = 0x0130;
					break;
				case TT_OTHER: // other
				case TT_WOODEN: // wooden
					sndid = 0x0123;
					break;
				case TT_GRASS: // grass
					sndid = 0x012D;
					break;
			}
			break;
		case 1:
			mChar->SetStep( 0 );	// step 1
			switch( tileType )
			{
				case TT_NORMAL:
					if( onHorse )
						sndid = 0x024B;
					else
						sndid = 0x012C; 
					break;
				case TT_WATER:	// water
					break;
				case TT_STONE: // stone
					sndid = 0x012F;
					break;
				case TT_OTHER: // other
				case TT_WOODEN: // wooden
					sndid = 0x0122;
					break;
				case TT_GRASS: // grass
					sndid = 0x012E;
					break;
			}
			break;
		case 2:
		case 3:
		default:
			mChar->SetStep( 1 );	// pause
			break;
	}
	if( sndid )			// if we have a valid sound
		PlaySound( mSock, sndid, true );
}

}

