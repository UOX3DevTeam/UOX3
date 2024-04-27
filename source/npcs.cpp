#include "uox3.h"
#include "msgboard.h"
#include "movement.h"
#include "cServerDefinitions.h"
#include "cSpawnRegion.h"
#include "skills.h"
#include "ssection.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "teffect.h"
#include "CPacketSend.h"
#include "cEffects.h"
#include "classes.h"
#include "regions.h"
#include "cRaces.h"
#include "townregion.h"
#include "Dictionary.h"

#include "ObjectFactory.h"

using namespace std::string_literals;

CCharStuff *Npcs = nullptr;
GenericList<CChar *>	alwaysAwakeNPCs;

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::GetAlwaysAwakeNPCs()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns list of NPCs that are always awake
//o------------------------------------------------------------------------------------------------o
GenericList<CChar *> * CCharStuff::GetAlwaysAwakeNPCs( void )
{
	return &alwaysAwakeNPCs;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::AddRandomLoot()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Add loot to monsters packs
//o------------------------------------------------------------------------------------------------o
auto CCharStuff::AddRandomLoot( CItem *s, const std::string& lootlist, bool shouldSave ) ->CItem *
{
	CItem *retItem = nullptr;
	std::string sect =  "LOOTLIST "s + lootlist;
	auto lootList = FileLookup->FindEntry( sect, items_def );
	if( lootList )
	{
		auto i = lootList->NumEntries();
		if( i > 0 )
		{
			i = RandomNum( static_cast<size_t>( 0 ), i - 1 );
			std::string tag = lootList->MoveTo( i );
			std::string tagData = lootList->GrabData();
			auto csecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( tagData, "//" )), "," );
			auto tcsecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( tag, "//" )), "," );

			if( !tag.empty() )
			{
				UI16 iAmount = 0;

				if( oldstrutil::upper( tag ) == "LOOTLIST" )
				{
					if( csecs.size() > 1 ) // Amount specified behind lootlist entry?
					{
						iAmount = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" )), nullptr, 0 ));
						[[maybe_unused]] CItem *retItemNested = nullptr;
						for( UI16 iCount = 0; iCount < iAmount; ++iCount )
						{
							retItemNested = AddRandomLoot( s, oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), shouldSave );
						}
					}
					else
					{
						retItem = AddRandomLoot( s, lootList->GrabData(), shouldSave );
					}
				}
				else
				{
					if( tcsecs.size() > 1 ) // Amount specified behind lootlist entry?
					{
						iAmount = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( tcsecs[1], "//" )), nullptr, 0 ));
						retItem = Items->CreateBaseScriptItem( s, oldstrutil::trim( oldstrutil::removeTrailing( tcsecs[0], "//" )), s->WorldNumber(), iAmount, s->GetInstanceId(), OT_ITEM, 0xFFFF, shouldSave );
						if( retItem )
						{
							retItem->SetCont( s );
							retItem->PlaceInPack();
						}
					}
					else
					{
						retItem = Items->CreateBaseScriptItem( s, tag, s->WorldNumber(), 1, s->GetInstanceId(), OT_ITEM, 0xFFFF, shouldSave );
						if( retItem )
						{
							retItem->SetCont( s );
							retItem->PlaceInPack();
						}
					}
				}
			}
		}
	}
	return retItem;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::CreateBaseNPC()
//|	Date		-	10/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a basic npc from the scripts
//o------------------------------------------------------------------------------------------------o
CChar *CCharStuff::CreateBaseNPC( std::string ourNPC, bool shouldSave )
{
	ourNPC						= oldstrutil::trim( oldstrutil::removeTrailing( ourNPC, "//" ));
	CScriptSection *npcCreate	= FileLookup->FindEntry( ourNPC, npc_def );
	if( npcCreate == nullptr )
	{
		Console.Error( oldstrutil::format( "CreateBaseNPC(): Bad script npc %s (NPC Not Found).", ourNPC.c_str() ));
		return nullptr;
	}

	CChar *cCreated = nullptr;
	if( npcCreate->NpcListExist() )
	{
		cCreated = CreateRandomNPC( npcCreate->NpcListData() );
	}
	else
	{
		cCreated = static_cast<CChar *>( ObjectFactory::GetSingleton().CreateObject( OT_CHAR ));
		if( cCreated == nullptr )
			return nullptr;

		if( !shouldSave )
		{
			cCreated->ShouldSave( false );
		}

		cCreated->SetSectionId( ourNPC );
		cCreated->SetSkillTitles( true );
		cCreated->SetNpc( true );
		cCreated->SetLoDamage( 1 );
		cCreated->SetHiDamage( 1 );
		cCreated->SetResist( 1, PHYSICAL );
		cCreated->SetSpawn( INVALIDSERIAL );

		if( !ApplyNpcSection( cCreated, npcCreate, ourNPC ))
		{
			Console.Error( "Trying to apply an npc section failed" );
		}

		std::vector<UI16> scriptTriggers = cCreated->GetScriptTriggers();
		for( auto scriptTrig : scriptTriggers )
		{
			cScript *toExecute = JSMapping->GetScript( scriptTrig );
			if( toExecute != nullptr )
			{
				toExecute->OnCreate( cCreated, true, false );
			}
		}
	}
	return cCreated;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::ChooseNpcToCreate()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Selects a random weighted entry from a vector with key-value pair (section and weight)
//o------------------------------------------------------------------------------------------------o
auto CCharStuff::ChooseNpcToCreate( const std::vector<std::pair<std::string, UI16>> npcListVector ) -> std::string
{
	auto npcListSize = npcListVector.size();
	if( npcListSize <= 0 )
		return "";

	int sum_of_weight = 0;
	for( const auto& it : npcListVector )
	{
		//const std::string& sectionName = it.first;
		const UI16 &sectionWeight = it.second;
		sum_of_weight += sectionWeight;
	}

	int rndChoice = RandomNum( 0, sum_of_weight - 1 );
	[[maybe_unused]] int npcWeight = 0;

	std::vector<int> matchingEntries;

	int weightOfChosenNpc = 0;
	for( size_t i = 0; i < npcListVector.size(); ++i )
	{
		//const std::string &sectionName = npcList[i].first;
		const UI16 &sectionWeight = npcListVector[i].second;

		// Ok, section has a weight, let's compare that weight to our chosen random number
		if( rndChoice < sectionWeight )
		{
			// If we find another entry with same weight as the first one found, or if none have been found yet, add to list
			if( weightOfChosenNpc == 0 || weightOfChosenNpc == sectionWeight )
			{
				weightOfChosenNpc = sectionWeight;

				// Add the entry index to a temporary vector of all entries with shared weight, the continue looking for more!
				matchingEntries.push_back( static_cast<int>(i) );
				continue;
			}
		}
		rndChoice -= sectionWeight;
	}

	// Did we find one or more entry that matched our random weight criteria?
	int npcEntryToSpawn = ( matchingEntries.size() > 0 ? matchingEntries[static_cast<int>( RandomNum( static_cast<size_t>( 0 ), matchingEntries.size() - 1 ))] : -1 );
	matchingEntries.clear();

	std::string chosenNpcSection = "";
	if( npcEntryToSpawn != -1 )
	{
		// If entry was selected based on weights, use that
		chosenNpcSection = npcListVector[npcEntryToSpawn].first;
	}
	else
	{
		// else, use a random entry from the list
		chosenNpcSection = npcListVector[RandomNum( static_cast<size_t>( 0 ), npcListSize - 1 )].first;
	}

	return chosenNpcSection;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::NpcListLookup()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Retrieves entries from a specified NPCLIST and builds a vector that's provided
//|					for ChooseNpcToCreate() function to perform a weighted random selection
//o------------------------------------------------------------------------------------------------o
auto CCharStuff::NpcListLookup( const std::string &npclist ) -> std::string
{
	auto sect = "NPCLIST "s + npclist;
	sect = oldstrutil::trim( oldstrutil::removeTrailing( sect, "//" ));

	auto npcList = FileLookup->FindEntry( sect, npc_def );
	if( !npcList )
		return "";

	auto npcListSize = npcList->NumEntries();
	if( npcListSize <= 0 )
		return "";

	// Stuff each entry from the npcList into a vector
	std::vector<std::pair<std::string, UI16>> npcListVector;
	for( size_t i = 0; i < npcListSize; i++ )
	{
		// Split string for entry into a stringlist based on | as separator
		auto csecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( npcList->MoveTo( i ), "//" )), "|" );

		UI16 sectionWeight = 1;
		if( csecs.size() > 1 )
		{
			sectionWeight = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 ));
		}

		auto npcSection = ( csecs.size() > 1 ? csecs[1] : csecs[0] );
		npcListVector.emplace_back( npcSection, sectionWeight );
	}

	auto chosenNpcSection = ChooseNpcToCreate( npcListVector );
	if( chosenNpcSection.empty() )
		return "";

	auto csecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( chosenNpcSection, "//" )), "=" );
	if( oldstrutil::upper( csecs[0] ) == "NPCLIST" )
	{
		// Chosen entry contained another NPCLIST! Let's dive back into it...
		chosenNpcSection = NpcListLookup( chosenNpcSection );
	}

	return chosenNpcSection;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::CreateRandomNPC()
//|	Date		-	10/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates a random npc from an npclist in specified dfn file
//o------------------------------------------------------------------------------------------------o
auto CCharStuff::CreateRandomNPC( const std::string &npclist ) -> CChar *
{
	auto sect = "NPCLIST "s + npclist;
	sect = oldstrutil::trim( oldstrutil::removeTrailing( sect, "//" ));

	auto npcList = FileLookup->FindEntry( sect, npc_def );
	if( !npcList )
		return nullptr;

	auto npcListSize = npcList->NumEntries();
	if( npcListSize <= 0 )
		return nullptr;

	// Stuff each entry from the npcList into a vector
	std::vector<std::pair<std::string, UI16>> npcListVector;
	for( size_t i = 0; i < npcListSize; i++ )
	{
		// Split string for entry into a stringlist based on | as separator
		auto csecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( npcList->MoveTo( i ), "//" )), "|" );

		UI16 sectionWeight = 1;
		if( csecs.size() > 1 )
		{
			sectionWeight = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" )), nullptr, 0 ));
		}

		auto npcSection = ( csecs.size() > 1 ? csecs[1] : csecs[0] );
		npcListVector.emplace_back( npcSection, sectionWeight );
	}

	auto chosenNpcSection = ChooseNpcToCreate( npcListVector );
	if( chosenNpcSection.empty() )
		return nullptr;

	CChar *cCreated = nullptr;
	auto csecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( chosenNpcSection, "//" )), "=" );
	if( oldstrutil::upper( csecs[0] ) == "NPCLIST" )
	{
		// Chosen entry contained another NPCLIST! Let's dive back into it...
		cCreated = CreateRandomNPC( npcList->GrabData() );
	}
	else
	{
		// Finally, an actual NPC entry. Spawn it!
		cCreated = CreateBaseNPC( chosenNpcSection );
	}

	return cCreated;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::CreateNPC()
//|	Date		-	10/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates an npc spawned from an item spawner
//o------------------------------------------------------------------------------------------------o
CChar *CCharStuff::CreateNPC( CSpawnItem *iSpawner, const std::string &npc )
{
	const ItemTypes iType = iSpawner->GetType();
	// If the spawner type is 125 and escort quests are not active then abort
	if( iType == IT_ESCORTNPCSPAWNER && !cwmWorldState->ServerData()->EscortsEnabled() )
		return nullptr;

	CChar *cCreated = nullptr;
	if( iSpawner->IsSectionAList() )
	{
		cCreated = CreateRandomNPC( npc );
	}
	else
	{
		cCreated = CreateBaseNPC( npc );
	}
	if( cCreated == nullptr )
		return nullptr;

	cCreated->SetSpawn( iSpawner->GetSerial() );
	SI16 awayX = 0, awayY = 0;
	if(( iType == IT_AREASPAWNER || iType == IT_ESCORTNPCSPAWNER ) && iSpawner->GetCont() == nullptr )
	{
		awayX = iSpawner->GetTempVar( CITV_MORE, 3 );
		awayY = iSpawner->GetTempVar( CITV_MORE, 4 );
	}

	FindSpotForNPC( cCreated, iSpawner->GetX(), iSpawner->GetY(), awayX, awayY, iSpawner->GetZ(), iSpawner->WorldNumber(), iSpawner->GetInstanceId() );
	PostSpawnUpdate( cCreated );

	if( iType == IT_ESCORTNPCSPAWNER )
	{
		MsgBoardQuestEscortCreate( cCreated );
	}

	cCreated->SetWipeable( true );

	return cCreated;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::CreateNPCxyz()
//|	Date		-	10/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates an npc at location xyz
//o------------------------------------------------------------------------------------------------o
CChar *CCharStuff::CreateNPCxyz( const std::string &npc, SI16 x, SI16 y, SI08 z, UI08 worldNumber, UI16 instanceId, bool useNpcList )
{
	CChar *cCreated = nullptr;
	if( useNpcList )
	{
		cCreated = CreateRandomNPC( npc );
	}
	else
	{
		cCreated = CreateBaseNPC( npc );
	}
	if( cCreated == nullptr )
		return nullptr;

	cCreated->SetLocation( x, y, z, worldNumber, instanceId );
	// Update "old location" for new NPCs straight away
	cCreated->SetOldLocation( x, y, z );
	PostSpawnUpdate( cCreated );
	return cCreated;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::PostSpawnUpdate()
//|	Date		-	10/12/2003
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates created npc
//o------------------------------------------------------------------------------------------------o
void CCharStuff::PostSpawnUpdate( CChar *cCreated )
{
	CTownRegion *tReg = CalcRegionFromXY( cCreated->GetX(), cCreated->GetY(), cCreated->WorldNumber(), cCreated->GetInstanceId(), cCreated );
	cCreated->SetRegion( tReg->GetRegionNum() );

	for( UI08 z = 0; z < ALLSKILLS; ++z )
	{
		Skills->UpdateSkillLevel( cCreated, z );
	}

	// Set hunger timer so NPC's hunger level doesn't instantly drop after spawning
	auto hungerRate	 = Races->GetHungerRate( cCreated->GetRace() );
	cCreated->SetTimer( tCHAR_HUNGER, BuildTimeValue( static_cast<R32>( hungerRate )));

	UpdateFlag( cCreated );
	cCreated->Update();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetRadius()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Generate a sensible radius given the values from the NPC DFNs
//o------------------------------------------------------------------------------------------------o
SI16 GetRadius( CChar *c )
{
	// see if they supplied a 'radius'
	if( c->GetFx( 1 ) > 0 )
	{
		// if they were supplying a bounding area, use the radius from that
		if( c->GetFx( 0 ) > 0 )
		{
			c->SetFx( abs(c->GetFx( 0 ) - c->GetFx( 1 )), 1 );
		}
	}
	// ensure its not something bogus
	if( c->GetFx( 1 ) <= 0 || c->GetFx( 1 ) > 100)
	{
		c->SetFx( 10, 1 );
	}
	return c->GetFx( 1 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckBoundingBox()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check bounding box
//o------------------------------------------------------------------------------------------------o
bool CheckBoundingBox( const SI16 xPos, const SI16 yPos, const SI16 fx1, const SI16 fy1, const SI08 fz1,
					  const SI16 fx2, const SI16 fy2, const UI08 worldNumber, const UI16 instanceId )
{
	if( xPos >= (( fx1 < fx2 ) ? fx1 : fx2 ) && xPos <= (( fx1 < fx2 ) ? fx2 : fx1 ))
	{
		if( yPos >= (( fy1 < fy2 ) ? fy1 : fy2 ) && yPos <= (( fy1 < fy2 ) ? fy2 : fy1 ))
		{
			if( fz1 == -1 || abs( fz1 - Map->Height( xPos, yPos, fz1, worldNumber, instanceId )) <= 5 )
				return true;
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckBoundingCircle()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check bounding circle
//o------------------------------------------------------------------------------------------------o
bool CheckBoundingCircle( const SI16 xPos, const SI16 yPos, const SI16 fx1, const SI16 fy1, const SI08 fz1,
						 const SI16 radius, const UI08 worldNumber, const UI16 instanceId )
{
	if(( xPos - fx1 ) * ( xPos - fx1 ) + ( yPos - fy1 ) * ( yPos - fy1 ) <= radius * radius )
	{
		if( fz1 == -1 || abs( fz1 - Map->Height( xPos, yPos, fz1, worldNumber, instanceId )) <= 5 )
			return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	InitializeWanderArea()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Setup the wander area if the npcwander is rect or circle
//o------------------------------------------------------------------------------------------------o
void InitializeWanderArea( CChar *c, SI16 xAway, SI16 yAway )
{
	// compute the rectangular bounding area
	if( WT_BOX == c->GetNpcWander() )
	{
		// if they provided a legal rectangle and
		// ensure the bounding rect contains the current location
		// if it doesn't the monster will never move!
		if( c->GetFx( 0 ) >= 0 && c->GetFy( 0 ) >= 0 && c->GetFy( 1 ) >= 0 && c->GetFx( 1 ) >= 0 &&
		   CheckBoundingBox( c->GetX(), c->GetY(), c->GetFx( 0 ), c->GetFy( 0 ), c->GetFz(), c->GetFx( 1 ), c->GetFy( 1 ), c->WorldNumber(), c->GetInstanceId() ))
		{
			return;// don't do anything to use what they specified in the NPC DFNs
		}
		else
		{
			// if they provided a 'radius' in the NPC DFNs, use that
			if( c->GetFx( 1 ) > 0 )
			{
				xAway = yAway = GetRadius( c );
			}
			// setup info for rectangular areas
			c->SetFx( c->GetX() - xAway, 0 );
			c->SetFy( c->GetY() - yAway, 0 );
			c->SetFx( c->GetX() + xAway, 1 );
			c->SetFy( c->GetY() + yAway, 1 );
		}
	}
	else if( WT_CIRCLE == c->GetNpcWander() )
	{
		// if they provided a legal circle and
		// ensure the bounding circle contains the current location
		// if it doesn't the monster will never move!
		if( c->GetFx( 0 ) >= 0 && c->GetFy( 0 ) >= 0 && c->GetFx( 1 ) >= 0 &&
		   CheckBoundingCircle( c->GetX(), c->GetY(), c->GetFx( 0 ), c->GetFy( 0 ), c->GetFz(), c->GetFx( 1 ), c->WorldNumber(), c->GetInstanceId() ))
		{
			return;// don't do anything to use what they specified in NPC DFNs
		}
		else
		{
			// if they provided a 'radius' in the NPC DFNs use that
			if( c->GetFx( 1 ) > 0 )
			{
				xAway = yAway = GetRadius( c );
			}
			// setup info for circular areas
			c->SetFx( c->GetX(), 0 );
			c->SetFy( c->GetY(), 0 );
			c->SetFx( xAway, 1 );
			c->SetFy( -1, 1 );
		}
	}
	// setting fz1 actually makes it check against the height and slows the system down a lot
	// does anyone really need to constrain the height at which a monster can move??
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::FindSpotForNPC()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find a valid spot to drop an NPC near the spawners location
//|
//|	Changes		-	06/26/2020	-	Added support for instanceId
//|
//|	Changes		-	04/20/2002	-	(Type 69) xos and yos (X OffSet, Y OffSet)
//|									are used to find a random number that is then added to the spawner's
//|									x and y (Using the spawner's z) and then place the NPC anywhere in
//|									a square around the spawner. This square is random anywhere from -10
//|									to +10 from the spawner's location (for x and y) If the place chosen
//|									is not a valid position (the NPC can't walk there) then a new place
//|									will be chosen, if a valid place cannot be found in a certain # of
//|									tries (50),the NPC will be placed directly on the spawner and the
//|									server op will be warned.
//o------------------------------------------------------------------------------------------------o
void CCharStuff::FindSpotForNPC( CChar *cCreated, const SI16 originX, const SI16 originY, const SI16 xAway,
								const SI16 yAway, const SI08 z, const UI08 worldNumber, const UI16 instanceId )
{

#ifdef DEBUG_SPAWN
	Console.Print( oldstrutil::format( "Going to spawn at (%d,%d) within %d by %d\n", originX, originY, xAway, yAway ));
#endif

	if( !ValidateObject( cCreated ))
		return;

	SI32 k = xAway * yAway / 2;
	SI16 xos = 0, yos = 0;
	SI08 targZ = 0;
	bool foundSpot = false;
	if( k > 50 )
	{
		k = 50;
	}

	while( !foundSpot )
	{
		targZ = z;
		if( --k < 0 ) //this CAN be a bit laggy. adjust as nessicary
		{
			if( xAway > 0 && yAway > 0 )
			{
				Console << "Problem area spawner found, NPC placed at default location" << myendl;
			}
			xos = originX;
			yos = originY;
			foundSpot = true;
			break;
		}

		xos = originX + RandomNum( static_cast<SI16>( -xAway ), xAway );
		yos = originY + RandomNum( static_cast<SI16>( -yAway ), yAway );

		if( xos >= 1 && yos >= 1 )
		{
			targZ = Map->Height( xos, yos, z, worldNumber, instanceId );
			if( !cwmWorldState->creatures[cCreated->GetId()].IsWater() )
			{
				foundSpot = Map->ValidSpawnLocation( xos, yos, targZ, worldNumber, instanceId );
			}
			else if( cwmWorldState->creatures[cCreated->GetId()].IsWater() || ( !foundSpot && cwmWorldState->creatures[cCreated->GetId()].IsAmphibian() ))
			{
				foundSpot = Map->ValidSpawnLocation( xos, yos, targZ, worldNumber, instanceId, false );
			}
		}
	}

	cCreated->SetLocation( xos, yos, targZ, worldNumber, instanceId );
	InitializeWanderArea( cCreated, xAway, yAway );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::LoadShopList()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Loads the shopping list pointed to by data in ITEM DFNs
//o------------------------------------------------------------------------------------------------o
auto CCharStuff::LoadShopList( const std::string& list, CChar *c ) -> void
{
	auto buyLayer = c->GetItemAtLayer( IL_BUYCONTAINER ); //Contains items the NPC is willing to buy
	auto boughtLayer = c->GetItemAtLayer( IL_BOUGHTCONTAINER ); //Contains items the NPC has already bought
	auto sellLayer = c->GetItemAtLayer( IL_SELLCONTAINER ); //Contains items the NPC will sell

	auto sect = "SHOPLIST "s + list;
	sect = oldstrutil::trim( oldstrutil::removeTrailing( sect, "//" ));
	auto ShoppingList = FileLookup->FindEntry( sect, items_def );
	if( ShoppingList == nullptr )
		return;

	bool shouldSave = c->ShouldSave();
	std::string cdata;
	SI32 ndata		= -1;
  [[maybe_unused]] SI32 odata = -1;
	CItem *retItem	= nullptr;
	for( const auto &sec : ShoppingList->collection2() )
	{
		auto tag = sec->tag;
		cdata = sec->cdata;
		ndata = sec->ndata;
		odata = sec->odata;
		switch( tag )
		{
			case DFNTAG_RSHOPITEM:
				if( ValidateObject( buyLayer ))
				{
					retItem = Items->CreateBaseScriptItem( nullptr, cdata, c->WorldNumber(), 1, c->GetInstanceId(), OT_ITEM, 0xFFFF, shouldSave );
					if( retItem )
					{
						retItem->SetCont( buyLayer );
						retItem->PlaceInPack();
						if( !retItem->GetName2().empty() && ( retItem->GetName2() != "#" ))
						{
							retItem->SetName( retItem->GetName2() ); // Item identified!
						}
					}
				}
				else
				{
					Console << "Warning: Bad Shopping List " << list << " with no Vendor Buy Pack for NPC " << c << " (serial: " << c->GetSerial() << myendl;
				}
				break;
			case DFNTAG_SELLITEM:
				if( ValidateObject( sellLayer ))
				{
					retItem = Items->CreateBaseScriptItem( nullptr, cdata, c->WorldNumber(), 1, c->GetInstanceId(), OT_ITEM, 0xFFFF, shouldSave );
					if( retItem )
					{
						retItem->SetCont( sellLayer );
						//retitem->SetSellValue( retitem->GetBuyValue() / 2 );
						retItem->PlaceInPack();
						if( !retItem->GetName2().empty() && ( retItem->GetName2() != "#" ))
						{
							retItem->SetName( retItem->GetName2() );
						}

						// Let's start out with the shops fully restocked after spawning
						retItem->IncAmount( retItem->GetRestock() );
						retItem->SetRestock( 0 );
					}
				}
				else
				{
					Console << "Warning: Bad Shopping List " << list << " with no Vendor Sell Pack for NPC " << c << " (serial: " << c->GetSerial() << myendl;
				}
				break;
			case DFNTAG_SHOPITEM:
				if( ValidateObject( boughtLayer ))
				{
					retItem = Items->CreateBaseScriptItem( nullptr, cdata, c->WorldNumber(), 1, c->GetInstanceId(), OT_ITEM, 0xFFFF, shouldSave );
					if( retItem )
					{
						retItem->SetCont( boughtLayer );
						retItem->PlaceInPack();
						if( !retItem->GetName2().empty() && ( retItem->GetName2() != "#" ))
						{
							retItem->SetName( retItem->GetName2() );
						}
					}
				}
				else
				{
					Console << "Warning: Bad Shopping List " << list << " with no Vendor Bought Pack for NPC " << c << " (serial: " << c->GetSerial() << myendl;
				}
				break;
			case DFNTAG_VALUE:
				if( retItem )
				{
					if( !cdata.empty() )
					{
						auto ssecs = oldstrutil::sections( cdata, " " );
						if( ssecs.size() > 1 )
						{
							retItem->SetBuyValue(  static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" )), nullptr, 0 )));
							retItem->SetSellValue( static_cast<UI32>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssecs[1], "//" )), nullptr, 0 )));
							break;
						}
					}
					retItem->SetBuyValue( ndata );
					retItem->SetSellValue(( ndata / 2 ));
				}
				break;
			default:
				break;
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	SetRandomName()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets a character with a random name from NPC namelists in namelists.dfn
//o------------------------------------------------------------------------------------------------o
void SetRandomName( CBaseObject *s, const std::string& namelist )
{
	std::string sect	= std::string( "RANDOMNAME " ) + namelist;
	sect				= oldstrutil::trim( oldstrutil::removeTrailing( sect, "//" ));
	std::string tempName;
	auto cat		= s->CanBeObjType(OT_CHAR) ? npc_def : items_def;

	CScriptSection *RandomName = FileLookup->FindEntry( sect, cat );
	if( RandomName == nullptr )
	{
		tempName = std::string( "Error Namelist " ) + namelist + std::string( " Not Found" );
	}
	else
	{
		size_t i = RandomName->NumEntries();
		if( i > 0 )
		{
			i = RandomNum( static_cast<size_t>( 0 ), i - 1 );
			tempName = RandomName->MoveTo( static_cast<SI16>( i ));
		}
	}
	s->SetName( tempName );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	AddRandomColor()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Randomly colors character
//o------------------------------------------------------------------------------------------------o
UI16 AddRandomColor( const std::string& colorlist )
{
	std::string sect				= std::string( "RANDOMCOLOR " ) + colorlist;
	sect							= oldstrutil::trim( oldstrutil::removeTrailing( sect, "//" ));
	CScriptSection *RandomColours	= FileLookup->FindEntry( sect, colors_def );
	if( RandomColours == nullptr )
	{
		Console.Warning( oldstrutil::format( "Error Colorlist %s Not Found", colorlist.c_str() ));
		return 0;
	}
	size_t i = RandomColours->NumEntries();
	if( i > 0 )
	{
		i = RandomNum( static_cast<size_t>( 0 ), i - 1 );
		std::string tag = RandomColours->MoveTo( static_cast<SI16>( i ));
		return static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( tag, "//" )), nullptr, 0 ));
	}
	return 0;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MakeShop()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Turn an NPC into a shopkeeper
//o------------------------------------------------------------------------------------------------o
void MakeShop( CChar *c )
{
	if( !ValidateObject( c ))
		return;

	c->SetShop( true );
	CItem *tPack = nullptr;
	for( UI08 i = IL_SELLCONTAINER; i <= IL_BUYCONTAINER; ++i )
	{
		tPack = c->GetItemAtLayer( static_cast<ItemLayers>( i ));
		if( !ValidateObject( tPack ))
		{
			bool shouldSave = c->ShouldSave();
			tPack = Items->CreateItem( nullptr, c, 0x2AF8, 1, 0, OT_ITEM, false, shouldSave );
			if( ValidateObject( tPack ))
			{
				tPack->SetDecayable( false );
				tPack->SetLayer( static_cast<ItemLayers>( i ));
				if( !tPack->SetCont( c ))
				{
					tPack->Delete();
				}
				else
				{
					tPack->SetType( IT_CONTAINER );
					tPack->SetNewbie( true );
				}
			}
		}
	}
	c->Update();
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::ApplyNpcSection()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Apply NPC DFN sections to an NPC
//o------------------------------------------------------------------------------------------------o
auto CCharStuff::ApplyNpcSection( CChar *applyTo, CScriptSection *NpcCreation, std::string sectionId, bool isGate ) -> bool
{
	if( NpcCreation == nullptr || !ValidateObject( applyTo ))
		return false;

	UI16 haircolor	= INVALIDCOLOUR;
	CItem *buyPack	= nullptr, *boughtPack = nullptr, *sellPack = nullptr;
	CItem *retitem	= nullptr, *mypack = nullptr;

	std::string cdata;
	SI32 ndata		= -1, odata = -1;
	UI08 skillToSet = 0xFF;

	TAGMAPOBJECT customTag;
	std::string customTagName;
	std::string customTagStringValue;
	for( const auto &sec : NpcCreation->collection2() )
	{
		DFNTAGS tag = sec->tag;
		cdata = sec->cdata;
		ndata = sec->ndata;
		odata = sec->odata;
		cdata = oldstrutil::trim( oldstrutil::removeTrailing( cdata, "//" ));
		auto ssects = oldstrutil::sections( cdata, " " );
		auto csects = oldstrutil::sections( cdata, "," );
		switch( tag )
		{
			case DFNTAG_ALCHEMY:			skillToSet = ALCHEMY;				break;
			case DFNTAG_ANATOMY:			skillToSet = ANATOMY;				break;
			case DFNTAG_ANIMALLORE: 		skillToSet = ANIMALLORE;			break;
			case DFNTAG_ARMSLORE:			skillToSet = ARMSLORE;				break;
			case DFNTAG_ARCHERY:			skillToSet = ARCHERY;				break;
			case DFNTAG_DAMAGE:
			case DFNTAG_ATT:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetLoDamage( static_cast<SI16>( ndata ));
						applyTo->SetHiDamage( static_cast<SI16>( odata ));
					}
					else
					{
						applyTo->SetLoDamage( static_cast<SI16>( ndata ));
						applyTo->SetHiDamage( static_cast<SI16>( ndata ));
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in ATT/DAMAGE tag inside NPC script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_AWAKE:
				if( !isGate )
				{
					applyTo->SetAwake( ndata != 0 );
				}
				break;
			case DFNTAG_BACKPACK:
				if( !isGate )
				{
					if( mypack == nullptr )
					{
						mypack = applyTo->GetPackItem();
					}
					if( mypack == nullptr )
					{
						bool shouldSave = applyTo->ShouldSave();
						mypack = Items->CreateItem( nullptr, applyTo, 0x0E75, 1, 0, OT_ITEM, false, shouldSave );
						if( ValidateObject( mypack ))
						{
							mypack->SetDecayable( false );
							applyTo->SetPackItem( mypack );
							mypack->SetName( "Backpack" );
							mypack->SetLayer( IL_PACKITEM );
							if( !mypack->SetCont( applyTo ))
							{
								mypack = nullptr;
							}
							else
							{
								mypack->SetX( 0 );
								mypack->SetY( 0 );
								mypack->SetZ( 0 );
								mypack->SetType( IT_CONTAINER );
								mypack->SetDye( true );
								mypack->SetMaxItems( cwmWorldState->ServerData()->MaxPlayerPackItems() );
								mypack->SetWeightMax( cwmWorldState->ServerData()->MaxPlayerPackWeight() );
							}
						}
					}
				}
				break;
			case DFNTAG_BEGGING:			skillToSet = BEGGING;				break;
			case DFNTAG_BLACKSMITHING:		skillToSet = BLACKSMITHING;			break;
			case DFNTAG_BOWCRAFT:			skillToSet = BOWCRAFT;				break;
			case DFNTAG_BUSHIDO:			skillToSet = BUSHIDO;				break;
			case DFNTAG_CAMPING:			skillToSet = CAMPING;				break;
			case DFNTAG_CARPENTRY:			skillToSet = CARPENTRY;				break;
			case DFNTAG_CARTOGRAPHY:		skillToSet = CARTOGRAPHY;			break;
			case DFNTAG_CARVE:
				if( !isGate )
				{
					applyTo->SetCarve( static_cast<SI16>( ndata ));
				}
				break;
			case DFNTAG_CHIVALRY:			skillToSet = CHIVALRY;				break;
			case DFNTAG_COOKING:			skillToSet = COOKING;				break;
			case DFNTAG_COLOUR:
				if( retitem != nullptr )
				{
					retitem->SetColour( static_cast<UI16>( ndata ));
				}
				break;
			case DFNTAG_COLOURLIST:
				if( retitem != nullptr )
				{
					retitem->SetColour( AddRandomColor( cdata ));
				}
				break;
			case DFNTAG_COLOURMATCHHAIR:
				if( retitem != nullptr )
				{
					retitem->SetColour( static_cast<UI16>( haircolor ));
				}
				break;
			case DFNTAG_CONTROLSLOTS:
				if( !isGate )
				{
					applyTo->SetControlSlots( static_cast<UI16>( ndata ));
				}
				break;
			case DFNTAG_COPPER:
				if( !isGate )
				{
					if( !ValidateObject( mypack ))
					{
						mypack = applyTo->GetPackItem();
					}
					if( ValidateObject( mypack ))
					{
						if( ndata >= 0 )
						{
							bool shouldSave = applyTo->ShouldSave();
							if( odata && odata > ndata )
							{
								retitem = Items->CreateScriptItem( nullptr, applyTo, "0x0EEA", static_cast<UI16>( RandomNum( ndata, odata )), OT_ITEM, true, 0xFFFF, shouldSave );
							}
							else
							{
								retitem = Items->CreateScriptItem( nullptr, applyTo, "0x0EEA", ndata, OT_ITEM, true, 0xFFFF, shouldSave );
							}
						}
						else
						{
							Console.Warning( oldstrutil::format( "Invalid data found in COPPER tag inside NPC script [%s]", sectionId.c_str() ));
						}
					}
					else
					{
						Console.Warning( oldstrutil::format( "Bad NPC Script ([%s]) with problem no backpack for copper", sectionId.c_str() ));
					}
				}
				break;
			case DFNTAG_DEX:
				if( ndata > 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetDexterity( static_cast<SI16>( RandomNum( ndata, odata )));
					}
					else
					{
						applyTo->SetDexterity( ndata );
					}
					applyTo->SetStamina( applyTo->GetMaxStam() );
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in DEX tag inside NPC script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_DETECTINGHIDDEN:	skillToSet = DETECTINGHIDDEN;			break;
			case DFNTAG_DEF:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetResist( static_cast<UI16>( RandomNum( ndata, odata )), PHYSICAL );
					}
					else
					{
						applyTo->SetResist( static_cast<UI16>( ndata ), PHYSICAL );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in DEF tag inside NPC script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_DEFBONUS:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetResist( applyTo->GetResist( PHYSICAL ) + static_cast<UI16>( RandomNum( ndata, odata )), PHYSICAL );
					}
					else
					{
						applyTo->SetResist( applyTo->GetResist( PHYSICAL ) + static_cast<UI16>( ndata ), PHYSICAL );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in DEFBONUS tag inside item script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_DIR:
				if( !isGate )
				{
					std::string cupper = oldstrutil::upper( cdata );
					if( cupper == "NE" )
					{
						applyTo->SetDir( NORTHEAST );
					}
					else if( cupper == "E" )
					{
						applyTo->SetDir( EAST );
					}
					else if( cupper == "SE" )
					{
						applyTo->SetDir( SOUTHEAST );
					}
					else if( cupper == "S" )
					{
						applyTo->SetDir( SOUTH );
					}
					else if( cupper == "SW" )
					{
						applyTo->SetDir( SOUTHWEST );
					}
					else if( cupper == "W" )
					{
						applyTo->SetDir( WEST );
					}
					else if( cupper == "NW" )
					{
						applyTo->SetDir( NORTHWEST );
					}
					else if( cupper == "N" )
					{
						applyTo->SetDir( NORTH );
					}
					else if( cupper == "RND" )
					{
						UI08 rndDir = RandomNum( 0, 7 );
						switch( rndDir )
						{
							case 0:
								applyTo->SetDir( NORTHEAST );
								break;
							case 1:
								applyTo->SetDir( EAST );
								break;
							case 2:
								applyTo->SetDir( SOUTHEAST );
								break;
							case 3:
								applyTo->SetDir( SOUTH );
								break;
							case 4:
								applyTo->SetDir( SOUTHWEST );
								break;
							case 5:
								applyTo->SetDir( WEST );
								break;
							case 6:
								applyTo->SetDir( NORTHWEST );
								break;
							case 7:
								applyTo->SetDir( NORTH );
								break;
						}
					}
				}
				break;
			case DFNTAG_ELEMENTRESIST:
				if( ssects.size() >= 4 )
				{
					applyTo->SetResist(( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssects[0], "//" )), nullptr, 0 ))), HEAT );
					applyTo->SetResist(( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssects[1], "//" )), nullptr, 0 ))), COLD );
					applyTo->SetResist(( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssects[2], "//" )), nullptr, 0 ))), LIGHTNING );
					applyTo->SetResist(( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssects[3], "//" )), nullptr, 0 ))), POISON );
				}
				break;
			case DFNTAG_ERBONUS:
				if( ssects.size() >= 4 )
				{
					applyTo->SetResist( applyTo->GetResist( HEAT ) + static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssects[0], "//" )), nullptr, 0 )), HEAT );
					applyTo->SetResist( applyTo->GetResist( COLD ) + static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssects[1], "//" )), nullptr, 0 )), COLD );
					applyTo->SetResist( applyTo->GetResist( LIGHTNING ) + static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssects[2], "//" )), nullptr, 0 )), LIGHTNING );
					applyTo->SetResist( applyTo->GetResist( POISON ) + static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssects[3], "//" )), nullptr, 0 )), POISON );
				}
				break;
			case DFNTAG_EMOTECOLOUR:
				if( !isGate )
				{
					applyTo->SetEmoteColour( static_cast<UI16>( ndata ));
				}
				break;
			case DFNTAG_ENTICEMENT:			skillToSet = ENTICEMENT;				break;
			case DFNTAG_EQUIPITEM:
				if( !isGate )
				{
					bool shouldSave = applyTo->ShouldSave();
					retitem = Items->CreateBaseScriptItem( nullptr, cdata, applyTo->WorldNumber(), 1, applyTo->GetInstanceId(), OT_ITEM, 0xFFFF, shouldSave );
					if( retitem != nullptr )
					{
						if( retitem->GetLayer() == IL_NONE )
						{
							Console << "Warning: Bad NPC Script ([" << sectionId.c_str() << "]) with problem item " << cdata << " executed!" << myendl;
						}
						else if( !retitem->SetCont( applyTo ))
						{
							if( !retitem->SetCont( applyTo->GetPackItem() ))
							{
								retitem = nullptr;
							}
						}

						if( retitem->GetLayer() == 25 )
						{
							// Item equipped on mount layer. Set NPC as mounted!
							applyTo->SetMounted( true );
						}
					}
				}
				break;
			case DFNTAG_EVALUATINGINTEL:	skillToSet = EVALUATINGINTEL;			break;
			case DFNTAG_FAME:				applyTo->SetFame( static_cast<SI16>( ndata ));		break;
			case DFNTAG_FENCING:			skillToSet = FENCING;					break;
			case DFNTAG_FISHING:			skillToSet = FISHING;					break;
			case DFNTAG_FLEEAT:
				if( !isGate )
				{
					applyTo->SetFleeAt( static_cast<SI16>( ndata ));
				}
				break;
			case DFNTAG_FLEEINGSPEED:
				applyTo->SetFleeingSpeed( std::stof( cdata ));
				break;
			case DFNTAG_FLEEINGSPEEDMOUNTED:
				applyTo->SetMountedFleeingSpeed( std::stof( cdata ));
				break;
			case DFNTAG_FLAG:
				if( !isGate )
				{
					if( !cdata.empty() )
					{
						std::string upperData = oldstrutil::upper( cdata );
						if( upperData == "NEUTRAL" )
						{
							applyTo->SetNPCFlag( fNPC_NEUTRAL );
						}
						else if( upperData == "INNOCENT" )
						{
							applyTo->SetNPCFlag( fNPC_INNOCENT );
						}
						else if( upperData == "EVIL" )
						{
							applyTo->SetNPCFlag( fNPC_EVIL );
						}
					}
				}
				break;
			case DFNTAG_FORENSICS:			skillToSet = FORENSICS;					break;
			case DFNTAG_FOCUS:				skillToSet = FOCUS;						break;
			case DFNTAG_FX1:
				if( !isGate )
				{
					applyTo->SetFx( static_cast<SI16>( ndata ), 0 );
				}
				break;
			case DFNTAG_FX2:
				if( !isGate )
				{
					applyTo->SetFx( static_cast<SI16>( ndata ), 1 );
				}
				break;
			case DFNTAG_FY1:
				if( !isGate )
				{
					applyTo->SetFy( static_cast<SI16>( ndata ), 0 );
				}
				break;
			case DFNTAG_FY2:
				if( !isGate )
				{
					applyTo->SetFy( static_cast<SI16>( ndata ), 1 );
				}
				break;
			case DFNTAG_FZ1:
				if( !isGate )
				{
					applyTo->SetFz( static_cast<SI08>( ndata ));
				}
				break;
			case DFNTAG_FOOD:				applyTo->SetFood( cdata );				break;
			case DFNTAG_GET:
			{
				std::string scriptEntry = "";
				if( ssects.size() == 1 )
				{
					scriptEntry = cdata;
				}
				else
				{
					SI32 rndEntry = RandomNum( 0, static_cast<SI32>( ssects.size() - 1 ));
					scriptEntry = oldstrutil::trim( oldstrutil::removeTrailing( ssects[rndEntry], "//" ));
				}

				CScriptSection *toFind = FileLookup->FindEntry( scriptEntry, npc_def );
				if( toFind == nullptr )
				{
					Console.Warning( oldstrutil::format( "Invalid script entry ([%s]) called with GET tag, NPC serial 0x%X", scriptEntry.c_str(), applyTo->GetSerial() ));
				}
				else if( toFind == NpcCreation )
				{
					Console.Warning( oldstrutil::format( "Infinite loop avoided with GET tag inside NPC script [%s]", scriptEntry.c_str() ));
				}
				else
				{
					ApplyNpcSection( applyTo, toFind, scriptEntry, isGate );
				}
				break;
			}
			case DFNTAG_GETUO:
			case DFNTAG_GETT2A:
			case DFNTAG_GETUOR:
			case DFNTAG_GETTD:
			case DFNTAG_GETLBR:
			case DFNTAG_GETAOS:
			case DFNTAG_GETSE:
			case DFNTAG_GETML:
			case DFNTAG_GETSA:
			case DFNTAG_GETHS:
			case DFNTAG_GETTOL:
			{
				// Inherit stats for object based on active ruleset
				bool getParent = false;
				std::string tagName = "";
				switch( cwmWorldState->ServerData()->ExpansionCoreShardEra() )
				{
					case ER_UO:		if( tag == DFNTAG_GETUO ) { getParent = true; tagName = "GETUO"; }		break;
					case ER_T2A:	if( tag == DFNTAG_GETT2A ) { getParent = true; tagName = "GETT2A"; }		break;
					case ER_UOR:	if( tag == DFNTAG_GETUOR ) { getParent = true; tagName = "GETUOR"; }		break;
					case ER_TD:		if( tag == DFNTAG_GETTD ) { getParent = true; tagName = "GETTD"; }			break;
					case ER_LBR:	if( tag == DFNTAG_GETLBR ) { getParent = true; tagName = "GETLBR"; }		break;
					case ER_AOS:	if( tag == DFNTAG_GETAOS ) { getParent = true; tagName = "GETAOS"; }		break;
					case ER_SE:		if( tag == DFNTAG_GETSE ) { getParent = true; tagName = "GETSE"; }			break;
					case ER_ML:		if( tag == DFNTAG_GETML ) { getParent = true; tagName = "GETML"; }			break;
					case ER_SA:		if( tag == DFNTAG_GETSA ) { getParent = true; tagName = "GETSA"; }			break;
					case ER_HS:		if( tag == DFNTAG_GETHS ) { getParent = true; tagName = "GETHS"; }			break;
					case ER_TOL:	if( tag == DFNTAG_GETTOL ) { getParent = true; tagName = "GETTOL"; }		break;
					default:
						break;
				}

				if( getParent )
				{
					std::string scriptEntry = "";
					if( ssects.size() == 1 )
					{
						scriptEntry = cdata;
					}
					else
					{
						SI32 rndEntry = RandomNum( 0, static_cast<SI32>( ssects.size() - 1 ));
						scriptEntry = oldstrutil::trim( oldstrutil::removeTrailing( ssects[rndEntry], "//" ));
					}

					CScriptSection *toFind = FileLookup->FindEntry( scriptEntry, npc_def );
					if( toFind == NULL )
					{
						Console.Warning( oldstrutil::format( "Invalid script entry ([%s]) called with %s tag, NPC serial 0x%X", scriptEntry.c_str(), tagName.c_str(), applyTo->GetSerial() ));
					}
					else if( toFind == NpcCreation )
					{
						Console.Warning( oldstrutil::format( "Infinite loop avoided with %s tag inside NPC script [%s]", tagName.c_str(), scriptEntry.c_str() ));
					}
					else
					{
						ApplyNpcSection( applyTo, toFind, scriptEntry, isGate );
					}
					break;
				}
				break;
			}
			case DFNTAG_GOLD:
				if( !isGate )
				{
					if( !ValidateObject( mypack ))
					{
						mypack = applyTo->GetPackItem();
					}
					if( ValidateObject( mypack ))
					{
						if( ndata >= 0 )
						{
							bool shouldSave = applyTo->ShouldSave();
							if( odata && odata > ndata )
							{
								retitem = Items->CreateScriptItem( nullptr, applyTo, "0x0EED", static_cast<UI16>( RandomNum( ndata, odata )), OT_ITEM, true, 0xFFFF, shouldSave );
							}
							else
							{
								retitem = Items->CreateScriptItem( nullptr, applyTo, "0x0EED", ndata, OT_ITEM, true, 0xFFFF, shouldSave );
							}
						}
						else
						{
							Console.Warning( oldstrutil::format( "Invalid data found in GOLD tag inside NPC script [%s]", sectionId.c_str() ));
						}
					}
					else
					{
						Console.Warning( oldstrutil::format( "Bad NPC Script ([%s]) with problem no backpack for gold", sectionId.c_str() ));
					}
				}
				break;
			case DFNTAG_HAIRCOLOUR:
				haircolor = AddRandomColor( cdata );
				if( retitem != nullptr )
				{
					retitem->SetColour( haircolor );
				}
				break;
			case DFNTAG_HEALING:			skillToSet = HEALING;					break;
			case DFNTAG_HERDING:			skillToSet = HERDING;					break;
			case DFNTAG_HIDING:				skillToSet = HIDING;					break;
			case DFNTAG_HIDAMAGE:			applyTo->SetHiDamage( static_cast<SI16>( ndata ));	break;
			case DFNTAG_HIRELING:
				if( !isGate )
				{
					applyTo->SetCanHire( true );
				}
				break;
			case DFNTAG_HP:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetHP( static_cast<SI16>( RandomNum( ndata, odata )));
					}
					else
					{
						applyTo->SetHP( ndata );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in HP tag inside NPC script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_HPMAX:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetFixedMaxHP( static_cast<SI16>( RandomNum( ndata, odata )));
					}
					else
					{
						applyTo->SetFixedMaxHP( ndata );
					}

					// Update current HP
					applyTo->SetHP( applyTo->GetMaxHP() );
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in HPMAX tag inside NPC script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_ID:
				/*applyTo->SetId( static_cast<UI16>( ndata ));
				applyTo->SetOrgId( static_cast<UI16>( ndata ));
				break;*/

				if( ssects.size() == 1 )
				{
					applyTo->SetId( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssects[0], "//" )), nullptr, 0 )));
					applyTo->SetOrgId( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssects[0], "//" )), nullptr, 0 )));
				}
				else
				{
					SI32 rndEntry = RandomNum( 0, static_cast<SI32>( ssects.size() - 1 ));
					applyTo->SetId( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssects[rndEntry], "//" )), nullptr, 0 )));
					applyTo->SetOrgId( static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( ssects[rndEntry], "//" )), nullptr, 0 )));
				}
				break;
			case DFNTAG_IMBUING:			skillToSet = IMBUING;					break;
			case DFNTAG_INSCRIPTION:		skillToSet = INSCRIPTION;				break;
			case DFNTAG_INTELLIGENCE:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetIntelligence( static_cast<SI16>( RandomNum( ndata, odata )));
					}
					else
					{
						applyTo->SetIntelligence( ndata );
					}
					applyTo->SetMana( applyTo->GetMaxMana() );
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in INTELLIGENCE tag inside NPC script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_ITEMID:				skillToSet = ITEMID;					break;
			case DFNTAG_KARMA:				applyTo->SetKarma( static_cast<SI16>( ndata ));		break;
			case DFNTAG_PACKITEM:
				[[fallthrough]];
			case DFNTAG_LOOT:
				if( !isGate )
				{
					if( !ValidateObject( mypack ))
					{
						mypack = applyTo->GetPackItem();
					}
					if( ValidateObject( mypack ))
					{
						if( !cdata.empty() )
						{
							auto csecs = oldstrutil::sections( oldstrutil::trim( oldstrutil::removeTrailing( cdata, "//" )), "," );
							if( csecs.size() > 1 )
							{
								UI16 iAmount = 0;
								std::string amountData = oldstrutil::trim( oldstrutil::removeTrailing( csecs[1], "//" ));
								auto tsects = oldstrutil::sections( amountData, " " );
								if( tsects.size() > 1 ) // check if the second part of the tag-data contains two sections separated by a space
								{
									auto first = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( tsects[0], "//" )), nullptr, 0 ));
									auto second = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( tsects[1], "//" )), nullptr, 0 ));

									// Tag contained a minimum and maximum value for amount! Let's randomize!
									iAmount = static_cast<UI16>( RandomNum( first, second ));
								}
								else
								{
									iAmount = static_cast<UI16>( std::stoul(amountData, nullptr, 0 ));
								}
								auto tdata = oldstrutil::trim( oldstrutil::removeTrailing( csecs[0], "//" ));

								if( tag == DFNTAG_LOOT )
								{
									Items->AddRespawnItem( mypack, tdata, true, true, iAmount, true );
								}
								else
								{
									Items->AddRespawnItem( mypack, tdata, true, false, iAmount, false );
								}
							}
							else
							{
								if( tag == DFNTAG_LOOT )
								{
									Items->AddRespawnItem( mypack, cdata, true, true, 1, true );
								}
								else
								{
									Items->AddRespawnItem( mypack, cdata, true, false, 1, false );
								}
							}
						}
					}
					else
					{
						Console.Warning( oldstrutil::format( "Bad NPC Script ([%s]) with problem no backpack for loot", sectionId.c_str() ));
					}
				}
				break;
			case DFNTAG_LOCKPICKING:		skillToSet = LOCKPICKING;				break;
			case DFNTAG_LODAMAGE:			applyTo->SetLoDamage( static_cast<SI16>( ndata ));	break;
			case DFNTAG_LUMBERJACKING:		skillToSet = LUMBERJACKING;				break;
			case DFNTAG_LOYALTY:
				applyTo->SetLoyalty( static_cast<UI16>( ndata ));
				break;
			case DFNTAG_MACEFIGHTING:		skillToSet = MACEFIGHTING;				break;
			case DFNTAG_MAGERY:				skillToSet = MAGERY;					break;
			case DFNTAG_MAGICRESISTANCE:	skillToSet = MAGICRESISTANCE;			break;
			case DFNTAG_MANA:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetMana( static_cast<SI16>( RandomNum( ndata, odata )));
					}
					else
					{
						applyTo->SetMana( ndata );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in MANA tag inside NPC script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_MANAMAX:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetFixedMaxMana( static_cast<SI16>( RandomNum( ndata, odata )));
					}
					else
					{
						applyTo->SetFixedMaxMana( ndata );
					}

					// Update current Mana
					applyTo->SetMana( applyTo->GetMaxMana() );
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in MANAMAX tag inside NPC script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_MAXLOYALTY:
				applyTo->SetMaxLoyalty( static_cast<UI16>( ndata ));
				break;
			case DFNTAG_MEDITATION:			skillToSet = MEDITATION;				break;
			case DFNTAG_MINING:				skillToSet = MINING;					break;
			case DFNTAG_MUSICIANSHIP:		skillToSet = MUSICIANSHIP;				break;
			case DFNTAG_MYSTICISM:			skillToSet = MYSTICISM;					break;
			case DFNTAG_NAME:				applyTo->SetName( cdata );				break;
			case DFNTAG_NAMELIST:			SetRandomName( applyTo, cdata );		break;
			case DFNTAG_NECROMANCY:			skillToSet = NECROMANCY;				break;
			case DFNTAG_NINJITSU:			skillToSet = NINJITSU;					break;
			case DFNTAG_NPCWANDER:
				if( !isGate )
				{
					applyTo->SetNpcWander( static_cast<SI08>( ndata ));
				}
				break;
			case DFNTAG_NPCAI:
				if( !isGate )
				{
					applyTo->SetNPCAiType( static_cast<SI16>( ndata ));
				}
				break;
			case DFNTAG_NPCGUILD:
				if( !isGate )
				{
					applyTo->SetNPCGuild( static_cast<UI16>( ndata ));
				}
				break;
			case DFNTAG_NOHIRELING:
				if( !isGate )
				{
					applyTo->SetCanHire( false );
				}
				break;
			case DFNTAG_NOTRAIN:
				if( !isGate )
				{
					applyTo->SetCanTrain( false );
				}
				break;
			case DFNTAG_ORIGIN:				applyTo->SetOrigin( static_cast<ExpansionRuleset>( cwmWorldState->ServerData()->EraStringToEnum( cdata )));			break;
			case DFNTAG_POISONSTRENGTH:		applyTo->SetPoisonStrength( static_cast<UI08>( ndata )); break;
			case DFNTAG_PRIV:
				if( !isGate )
				{
					applyTo->SetPriv( static_cast<UI16>( ndata ));
				}
				break;
			case DFNTAG_PARRYING:			skillToSet = PARRYING;					break;
			case DFNTAG_PEACEMAKING:		skillToSet = PEACEMAKING;				break;
			case DFNTAG_PROVOCATION:		skillToSet = PROVOCATION;				break;
			case DFNTAG_POISONING:			skillToSet = POISONING;					break;
			case DFNTAG_RESISTFIRE:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetResist( static_cast<UI16>( RandomNum( ndata, odata )), HEAT );
					}
					else
					{
						applyTo->SetResist( ndata, HEAT );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in RESISTFIRE tag inside NPC script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_RESISTCOLD:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetResist( static_cast<UI16>( RandomNum( ndata, odata )), COLD );
					}
					else
					{
						applyTo->SetResist( ndata, COLD );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in RESISTCOLD tag inside NPC script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_RESISTLIGHTNING:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetResist( static_cast<UI16>( RandomNum( ndata, odata )), LIGHTNING );
					}
					else
					{
						applyTo->SetResist( ndata, LIGHTNING );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in RESISTLIGHTNING tag inside NPC script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_RESISTPOISON:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetResist( static_cast<UI16>( RandomNum( ndata, odata )), POISON );
					}
					else
					{
						applyTo->SetResist( ndata, POISON );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in RESISTPOISON tag inside NPC script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_RSHOPITEM:
				if( !isGate )
				{
					if( !ValidateObject( buyPack ))
					{
						buyPack = applyTo->GetItemAtLayer( IL_BUYCONTAINER );
					}
					if( ValidateObject( buyPack ))
					{
						retitem = Items->CreateBaseScriptItem( nullptr, cdata, applyTo->WorldNumber(), 1 );
						if( retitem )
						{
							retitem->SetCont( buyPack );
							retitem->PlaceInPack();
							if( !retitem->GetName2().empty() && ( retitem->GetName2() != "#" ))
							{
								retitem->SetName( retitem->GetName2() ); // Item identified!
							}
						}
					}
					else
					{
						Console.Warning( oldstrutil::format( "Bad NPC Script ([%s]) with no Vendor Buy Pack for item", sectionId.c_str() ));
					}
				}
				break;
			case DFNTAG_REATTACKAT:
				if( !isGate )
				{
					applyTo->SetReattackAt( static_cast<SI16>( ndata ));
				}
				break;
			case DFNTAG_REMOVETRAP:			skillToSet = REMOVETRAP;				break;
			case DFNTAG_RACE:				applyTo->SetRace( static_cast<UI16>( ndata ));		break;
			case DFNTAG_RUNS:
				if( !isGate )
				{
					applyTo->SetRun( true );
				}
				break;
			case DFNTAG_RUNNINGSPEED:
				applyTo->SetRunningSpeed( std::stof( cdata ));
				break;
			case DFNTAG_RUNNINGSPEEDMOUNTED:
				applyTo->SetMountedRunningSpeed( std::stof( cdata ));
				break;
			case DFNTAG_SECTIONID:			applyTo->SetSectionId( cdata );							break;
			case DFNTAG_SKIN:				applyTo->SetSkin( static_cast<UI16>( ndata ));		break;
			case DFNTAG_SILVER:
				if( !isGate )
				{
					if( !ValidateObject( mypack ))
					{
						mypack = applyTo->GetPackItem();
					}
					if( ValidateObject( mypack ))
					{
						if( ndata >= 0 )
						{
							bool shouldSave = applyTo->ShouldSave();
							if( odata && odata > ndata )
							{
								retitem = Items->CreateScriptItem( nullptr, applyTo, "0x0EF0", static_cast<UI16>( RandomNum( ndata, odata )), OT_ITEM, true, 0xFFFF, shouldSave );
							}
							else
							{
								retitem = Items->CreateScriptItem( nullptr, applyTo, "0x0EF0", ndata, OT_ITEM, true, 0xFFFF, shouldSave );
							}
						}
						else
						{
							Console.Warning( oldstrutil::format( "Invalid data found in SILVER tag inside NPC script [%s]", sectionId.c_str() ));
						}
					}
					else
					{
						Console.Warning( oldstrutil::format( "Bad NPC Script ([%s]) with problem no backpack for silver", sectionId.c_str() ));
					}
				}
				break;
			case DFNTAG_SHOPKEEPER:
				if( !isGate )
				{
					MakeShop( applyTo );
				}
				break;
			case DFNTAG_SHOPLIST:
				if( !isGate )
				{
					LoadShopList( cdata, applyTo );
				}
				break;
			case DFNTAG_SELLITEM:
				if( !isGate )
				{
					if( !ValidateObject( sellPack ))
					{
						sellPack = applyTo->GetItemAtLayer( IL_SELLCONTAINER );
					}
					if( ValidateObject( sellPack ))
					{
						bool shouldSave = applyTo->ShouldSave();
						retitem = Items->CreateBaseScriptItem( nullptr, cdata, applyTo->WorldNumber(), 1, applyTo->GetInstanceId(), OT_ITEM, 0xFFFF, shouldSave );
						if( retitem != nullptr )
						{
							retitem->SetCont( sellPack );
							retitem->PlaceInPack();
							if( !retitem->GetName2().empty() && ( retitem->GetName2() != "#" ))
							{
								retitem->SetName( retitem->GetName2() );
							}
						}
					}
					else
					{
						Console.Warning( oldstrutil::format( "Bad NPC Script ([%s]) with no Vendor SellPack for item", sectionId.c_str() ));
					}
				}
				break;
			case DFNTAG_SHOPITEM:
				if( !isGate )
				{
					if( !ValidateObject( boughtPack ))
					{
						boughtPack = applyTo->GetItemAtLayer( IL_BOUGHTCONTAINER );
					}
					if( ValidateObject( boughtPack ))
					{
						bool shouldSave = applyTo->ShouldSave();
						retitem = Items->CreateBaseScriptItem( nullptr, cdata, applyTo->WorldNumber(), 1, applyTo->GetInstanceId(), OT_ITEM, 0xFFFF, shouldSave );
						if( retitem != nullptr )
						{
							retitem->SetCont( boughtPack );
							retitem->PlaceInPack();
							if( !retitem->GetName2().empty() && ( retitem->GetName2() != "#" ))
							{
								retitem->SetName( retitem->GetName2() );
							}
						}
					}
					else
					{
						Console.Warning(oldstrutil::format( "Bad NPC Script ([%s]) with no Vendor Bought Pack for item", sectionId.c_str() ));
					}
				}
				break;
			case DFNTAG_SAYCOLOUR:
				if( !isGate )
				{
					applyTo->SetSayColour( static_cast<UI16>( ndata ));
				}
				break;
			case DFNTAG_SPATTACK:
				if( !isGate )
				{
					applyTo->SetSpAttack( static_cast<SI16>( ndata ));
				}
				break;
			case DFNTAG_SPADELAY:
				if( !isGate )
				{
					applyTo->SetSpDelay( static_cast<SI08>( ndata ));
				}
				break;
			case DFNTAG_SPELLWEAVING:		skillToSet = SPELLWEAVING;				break;
			case DFNTAG_SPLIT:
				if( !isGate )
				{
					applyTo->SetSplit( static_cast<UI08>( ndata ));
				}
				break;
			case DFNTAG_SPLITCHANCE:
				if( !isGate )
				{
					applyTo->SetSplitChance( static_cast<UI08>( ndata ));
				}
				break;
			case DFNTAG_SNOOPING:			skillToSet = SNOOPING;					break;
			case DFNTAG_SPIRITSPEAK:		skillToSet = SPIRITSPEAK;				break;
			case DFNTAG_STEALING:			skillToSet = STEALING;					break;
			case DFNTAG_STRENGTH:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetStrength( static_cast<SI16>( RandomNum( ndata, odata )));
					}
					else
					{
						applyTo->SetStrength( ndata );
					}
					applyTo->SetHP( applyTo->GetMaxHP() );
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in STRENGTH tag inside NPC script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_SWORDSMANSHIP:		skillToSet = SWORDSMANSHIP;				break;
			case DFNTAG_STAMINA:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetStamina( static_cast<SI16>( RandomNum( ndata, odata )));
					}
					else
					{
						applyTo->SetStamina( ndata );
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in STAMINA tag inside NPC script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_STAMINAMAX:
				if( ndata >= 0 )
				{
					if( odata && odata > ndata )
					{
						applyTo->SetFixedMaxStam( static_cast<SI16>( RandomNum( ndata, odata )));
					}
					else
					{
						applyTo->SetFixedMaxStam( ndata );
					}

					// Update current Stamina
					applyTo->SetStamina( applyTo->GetMaxStam() );
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in STAMINA tag inside NPC script [%s]", sectionId.c_str() ));
				}
				break;
			case DFNTAG_STEALTH:			skillToSet = STEALTH;					break;
			case DFNTAG_SKINLIST:			applyTo->SetSkin( AddRandomColor( cdata ));			break;
			case DFNTAG_SKILL:				applyTo->SetBaseSkill( static_cast<UI16>( odata ), static_cast<UI08>( ndata )); break;
			case DFNTAG_SCRIPT:
				if( !isGate )
				{
					applyTo->AddScriptTrigger( static_cast<UI16>( ndata ));
				}
				break;
			case DFNTAG_THROWING:			skillToSet = THROWING;					break;
			case DFNTAG_TITLE:				applyTo->SetTitle( cdata );		break;
			case DFNTAG_TOTAME:
				if( !isGate )
				{
					applyTo->SetTaming( static_cast<SI16>( ndata ));
					applyTo->SetOrneriness( static_cast<UI16>( ndata )); // Set default difficulty to control equal to skill required to tame
				}
				break;
			case DFNTAG_TOPROV:
				if( !isGate )
				{
					applyTo->SetProvoing( static_cast<SI16>( ndata ));
				}
				break;
			case DFNTAG_TOPEACE:
				if( !isGate )
				{
					applyTo->SetPeaceing( static_cast<SI16>( ndata ));
				}
				applyTo->SetBrkPeaceChanceGain( static_cast<SI16>( odata ));
				break;
			case DFNTAG_TAMEDHUNGER:
				if( !isGate )
				{
					applyTo->SetTamedHungerRate( static_cast<UI16>( ndata ));
					applyTo->SetTamedHungerWildChance( static_cast<UI08>( odata ));
				}
				break;
			case DFNTAG_TAMEDTHIRST:
				if( !isGate )
				{
					applyTo->SetTamedThirstRate( static_cast<UI16>( ndata ));
					applyTo->SetTamedThirstWildChance( static_cast<UI08>( odata ));
				}
				break;
			case DFNTAG_WILLHUNGER:
				if( !isGate )
				{
					if( ndata > 0 )
					{
						applyTo->SetHungerStatus( true );
					}
					else
					{
						applyTo->SetHungerStatus( false );
					}
				}
				break;
			case DFNTAG_WALKINGSPEED:
				applyTo->SetWalkingSpeed( std::stof( cdata ));
				break;
			case DFNTAG_WALKINGSPEEDMOUNTED:
				applyTo->SetMountedWalkingSpeed( std::stof( cdata ));
				break;
			case DFNTAG_TACTICS:			skillToSet = TACTICS;					break;
			case DFNTAG_TAILORING:			skillToSet = TAILORING;					break;
			case DFNTAG_TAMING:				skillToSet = TAMING;					break;
			case DFNTAG_TASTEID:			skillToSet = TASTEID;					break;
			case DFNTAG_TINKERING:			skillToSet = TINKERING;					break;
			case DFNTAG_TRACKING:			skillToSet = TRACKING;					break;
			case DFNTAG_VETERINARY:			skillToSet = VETERINARY;				break;
			case DFNTAG_WRESTLING:			skillToSet = WRESTLING;					break;
			case DFNTAG_CUSTOMSTRINGTAG:
			{
				auto count = 0;
				std::string result;
				for( auto &sec : ssects )
				{
					if( count > 0 )
					{
						if( count == 1 )
						{
							result = oldstrutil::trim( oldstrutil::removeTrailing( sec, "//" ));
						}
						else
						{
							result = result + " " + oldstrutil::trim( oldstrutil::removeTrailing( sec, "//" ));
						}
					}
					count++;
				}
				customTagName			= oldstrutil::trim( oldstrutil::removeTrailing( ssects[0], "//" ));
				customTagStringValue	= result;

				if( !customTagName.empty() && !customTagStringValue.empty() )
				{
					customTag.m_Destroy		= false;
					customTag.m_StringValue	= customTagStringValue;
					customTag.m_IntValue	= static_cast<SI32>( customTag.m_StringValue.size() );
					customTag.m_ObjectType	= TAGMAP_TYPE_STRING;
					applyTo->SetTag( customTagName, customTag);
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in CUSTOMSTRINGTAG tag inside NPC script [%s] - Supported data format: <tagName> <text>", sectionId.c_str() ));
				}
				break;
			}
			case DFNTAG_CUSTOMINTTAG:
			{
				auto count = 0;
				std::string result;
				for( auto &sec : ssects )
				{
					if( count > 0 )
					{
						if( count == 1 )
						{
							result = oldstrutil::trim( oldstrutil::removeTrailing( sec, "//" ));
						}
					}
					count++;
				}
				customTagName = oldstrutil::trim( oldstrutil::removeTrailing( ssects[0], "//" ));
				customTagStringValue = result;
				if( !customTagName.empty() && !customTagStringValue.empty() )
				{
					customTag.m_Destroy = false;
					customTag.m_IntValue = std::stoi( customTagStringValue );
					customTag.m_ObjectType = TAGMAP_TYPE_INT;
					customTag.m_StringValue = "";
					applyTo->SetTag( customTagName, customTag );
					if( count > 2 )
					{
						Console.Warning( oldstrutil::format( "Multiple values detected for CUSTOMINTTAG in NPC script [%s] - only first value will be used! Supported data format: <tagName> <value>", sectionId.c_str() ));
					}
				}
				else
				{
					Console.Warning( oldstrutil::format( "Invalid data found in CUSTOMINTTAG tag in NPC script [%s] - Supported data format: <tagName> <value>", sectionId.c_str() ));
				}
				break;
			}
			default:
				Console << "Unknown tag in ApplyNpcSection(): " << static_cast<SI32>( tag ) << myendl; break;
		}
		if( skillToSet != 0xFF )
		{
			applyTo->SetBaseSkill( static_cast<UI16>( RandomNum( ndata, odata )), skillToSet );
			skillToSet = 0xFF;
		}
	}
	return true;
}

bool CCharStuff::CanControlPet( CChar *mChar, CChar *Npc, bool isRestricted, bool checkDifficulty, bool ignoreOwnerCheck, bool ignoreLoyaltyChanges )
{
	if( ValidateObject( Npc->GetOwnerObj() ) && Npc->GetNpcAiType() != AI_PLAYERVENDOR && Npc->GetQuestType() == 0 )
	{
		if( !ignoreOwnerCheck && Npc->GetOwnerObj() != mChar && ( isRestricted || !Npcs->CheckPetFriend( mChar, Npc )))
			return false;

		if( checkDifficulty && Npc->IsTamed() && cwmWorldState->ServerData()->CheckPetControlDifficulty() )
		{
			// Let's base this on how difficult it is to control the pet, as well
			UI16 chanceToControl = Skills->CalculatePetControlChance( mChar, Npc );
			UI16 loyaltyGainOnSuccess = cwmWorldState->ServerData()->GetPetLoyaltyGainOnSuccess();
			UI16 loyaltyLossOnFailure = cwmWorldState->ServerData()->GetPetLoyaltyLossOnFailure();

			// Easy to control pets/summoned creatures don't gain/lose loyalty from commands
			if( chanceToControl == 1000 )
				return true;

			if( chanceToControl >= RandomNum( 0, 1000 ))
			{
				// Succeeded in controlling pet
				if( !ignoreLoyaltyChanges )
				{
					Npc->SetLoyalty( std::min( 100, Npc->GetLoyalty() + loyaltyGainOnSuccess ));
				}
				return true;
			}
			else
			{
				// Failed to control pet
				if( !ignoreLoyaltyChanges )
				{
					Npc->SetLoyalty( std::max( 0, Npc->GetLoyalty() - loyaltyLossOnFailure ));
					UI16 soundToPlay = cwmWorldState->creatures[Npc->GetId()].GetSound( SND_STARTATTACK );
					Effects->PlaySound( Npc, soundToPlay );

					if( Npc->GetLoyalty() == 0 )
					{
						// Reduce player's control slot usage by the amount of control slots taken up by the pet
						mChar->SetControlSlotsUsed( std::max( 0, mChar->GetControlSlotsUsed() - Npc->GetControlSlots() ));

						// Pet goes wild for lack of loyalty
						Npcs->ReleasePet( Npc );
						return false;
					}

					CSocket *mSock = mChar->GetSocket();
					if( mSock != nullptr )
					{
						std::string npcName = GetNpcDictName( Npc, mSock, NRS_SPEECH );
						mSock->SysMessage( 2412, npcName.c_str() ); // %s disobeys your command
					}
				}

				return false;
			}
		}

		// No difficulty checking needed, just allow it
		return true;
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::FinalizeTransfer()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Finalize transfer of pet to new owner
//o------------------------------------------------------------------------------------------------o
void CCharStuff::FinalizeTransfer( CChar *petChar, CChar *srcChar, CChar *targChar )
{
	// If the pet is guarding something, stop guarding that something
	Npcs->StopPetGuarding( petChar );

	// Clear pet's existing friend-list
	petChar->ClearFriendList();

	std::string petName = GetNpcDictName( petChar, nullptr, NRS_SPEECH );
	petChar->TextMessage( nullptr, 1074, TALK, 0, petName.c_str(), targChar->GetName().c_str() ); // * %s will now take %s as his master *

	// Transfer ownership
	petChar->SetOwner( targChar );

	// Make pet follow their new owner
	petChar->SetFTarg( static_cast<CChar*>( targChar ));
	petChar->FlushPath();
	petChar->SetNpcWander( WT_FOLLOW );

	// Update control slots used for both old and new owners
	srcChar->SetControlSlotsUsed( std::max(0, srcChar->GetControlSlotsUsed() - petChar->GetControlSlots() ));
	targChar->SetControlSlotsUsed( std::clamp( targChar->GetControlSlotsUsed() + petChar->GetControlSlots(), 0, 255 ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::ReleasePet()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Release the pet from its owner
//o------------------------------------------------------------------------------------------------o
void CCharStuff::ReleasePet( CChar *pet )
{
	// Stop guarding
	Npcs->StopPetGuarding( pet );

	// Stop following
	pet->SetFTarg( nullptr );
	pet->SetNpcWander( WT_FREE );

	// Remove owner
	pet->SetOwner( nullptr );

	std::string petName = GetNpcDictName( pet, nullptr, NRS_SPEECH );
	pet->TextMessage( nullptr, 1325, TALK, 0, petName.c_str() ); // *%s appears to have decided that it is better off without a master *

	// If a summoned creature, unsummon it
	if( pet->GetTimer( tNPC_SUMMONTIME ))
	{
		Effects->PlaySound( pet, 0x01FE );
		pet->Delete();
	}

	// Reset hunger
	pet->SetHunger( 6 );

	// Increase pet orneriness, to make it more difficult to control for its next owner
	auto ownerCount = pet->GetOwnerCount();
	switch( ownerCount )
	{
		case 1:
			pet->SetOrneriness( pet->GetTaming() + 48 );
			break;
		case 2:
			pet->SetOrneriness( pet->GetTaming() + 192 );
			break;
		case 3:
			pet->SetOrneriness( pet->GetTaming() + 432 );
			break;
		case 4:
			pet->SetOrneriness( pet->GetTaming() + 768 );
			break;
		default:
			break;
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::GetGuardingFollower()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the active follower guarding an item / character
//o------------------------------------------------------------------------------------------------o
auto CCharStuff::GetGuardingFollower( CChar *mChar, CBaseObject *guarded ) ->CChar *
{
	if( ValidateObject( mChar ) && ValidateObject( guarded ))
	{
		auto myFollowers = mChar->GetFollowerList();
		for( const auto &follower : myFollowers->collection() )
		{
			if( ValidateObject( follower ))
			{
				//if( !follower->GetMounted() && follower->GetNpcAiType() == AI_PET_GUARD &&
				if( !follower->GetMounted() && follower->GetGuarding() == guarded && follower->GetOwnerObj() == mChar )
				{
					return follower;
				}
			}
		}
	}
	return nullptr;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::CheckPetFriend()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Search a pets friends list to check if a character is a friend
//o------------------------------------------------------------------------------------------------o
bool CCharStuff::CheckPetFriend( CChar *mChar, CChar *pet )
{
	auto petFriends	= pet->GetFriendList();
	if( petFriends != nullptr )
	{
		for( auto &getFriend : *petFriends )
		{
			if( ValidateObject( getFriend ) && getFriend == mChar )
				return true;
		}
	}
	return false;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CCharStuff::StopPetGuarding()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Find the item/char pet is guarding and set it to not guarded
//o------------------------------------------------------------------------------------------------o
void CCharStuff::StopPetGuarding( CChar *pet )
{
	CBaseObject *petGuarding = pet->GetGuarding();
	if( !ValidateObject( petGuarding ))
		return;

	if( petGuarding->GetObjType() == OT_ITEM )
	{
		CItem *itemGuard = static_cast<CItem *>( petGuarding );
		if( ValidateObject( itemGuard ))
		{
			itemGuard->SetGuarded( false );
		}
	}
	else
	{
		CChar *charGuard = static_cast<CChar *>( petGuarding );
		if( ValidateObject( charGuard ))
		{
			charGuard->SetGuarded( false );
		}
	}
	pet->SetGuarding( nullptr );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MonsterGate()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle monster gates (polymorphs players into monster bodies)
//o------------------------------------------------------------------------------------------------o
void MonsterGate( CChar *s, const std::string& scriptEntry )
{
	CItem *mypack = nullptr;
	if( s->IsNpc() )
		return;
		
	auto entry = scriptEntry;
	entry = oldstrutil::trim( oldstrutil::removeTrailing( entry, "//" ));
	CScriptSection *Monster = FileLookup->FindEntry( entry, npc_def );
	if( Monster == nullptr )
		return;

	s->SetTitle( "\0" );

	CItem *n;
	for( CItem *z = s->FirstItem(); !s->FinishedItems(); z = s->NextItem() )
	{
		if( ValidateObject( z ))
		{
			if( z->IsFree() )
				continue;

			if( z->GetLayer() == IL_HAIR || z->GetLayer() == IL_FACIALHAIR )
			{
				z->Delete();
			}
			else if( z->GetLayer() != IL_PACKITEM && z->GetLayer() != IL_BANKBOX )
			{
				if( mypack == nullptr )
				{
					mypack = s->GetPackItem();
				}
				if( mypack == nullptr )
				{
					n = Items->CreateItem( nullptr, s, 0x0E75, 1, 0, OT_ITEM );
					if( !ValidateObject( n ))
						return;

					s->SetPackItem( n );
					n->SetDecayable( false );
					n->SetLayer( IL_PACKITEM );
					if( n->SetCont( s ))
					{
						n->SetType( IT_CONTAINER );
						n->SetDye( true );
						mypack = n;
					}
				}
				z->SetCont( mypack );
				z->PlaceInPack();
			}
		}
	}

	Npcs->ApplyNpcSection( s, Monster, scriptEntry, true );
	//Now find real 'skill' based on 'baseskill' (stat modifiers)
	for( UI08 j = 0; j < ALLSKILLS; ++j )
	{
		Skills->UpdateSkillLevel( s, j );
	}
	s->Update();
	Effects->PlayStaticAnimation( s, 0x373A, 0, 15 );
	Effects->PlaySound( s, 0x01E9 );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Karma()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle karma addition/subtraction when character kills
//|					another Character / NPC
//o------------------------------------------------------------------------------------------------o
void Karma( CChar *nCharId, CChar *nKilledId, const SI16 nKarma )
{
	SI16 nChange	= 0;
	bool nEffect	= false;
	SI16 nCurKarma	= nCharId->GetKarma();
	if( nCurKarma > 10000 )
	{
		nCharId->SetKarma( 10000 );
		nCurKarma = 10000;
	}
	else if( nCurKarma < -10000 )
	{
		nCharId->SetKarma( -10000 );
		nCurKarma = -10000;
	}

	if( nCurKarma < nKarma && nKarma > 0 )
	{
		nChange = (( nKarma - nCurKarma ) / 75 );
		nCharId->SetKarma( static_cast<SI16>( nCurKarma + nChange ));
		nEffect = true;
	}
	if( nCurKarma > nKarma && ( !ValidateObject( nKilledId ) || nKilledId->GetKarma() > 0 ))
	{
		nChange = (( nCurKarma - nKarma ) / 50 );
		nCharId->SetKarma( static_cast<SI16>( nCurKarma - nChange ));
		nEffect = false;
	}
	if( nChange == 0 )	// NPCs CAN gain/lose karma
	{
		return;
	}

	CSocket *mSock = nCharId->GetSocket();
	if( nCharId->IsNpc() || mSock == nullptr )
	{
		return;
	}

	if( nChange <= 25 )
	{
		if( nEffect )
		{
			mSock->SysMessage( 1367 ); // You have gained a little karma.
		}
		else
		{
			mSock->SysMessage( 1368 ); // You have lost a little karma.
		}
		return;
	}
	if( nChange <= 50 )
	{
		if( nEffect )
		{
			mSock->SysMessage( 1369 ); // You have gained some karma.
		}
		else
		{
			mSock->SysMessage( 1370 ); // You have lost some karma.
		}
		return;
	}
	if( nEffect )
	{
		mSock->SysMessage( 1371 ); // You have gained a lot of karma.
	}
	else
	{
		mSock->SysMessage( 1372 ); // You have lost a lot of karma.
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	Fame()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Handle fame addition when character kills another Character / NPC
//o------------------------------------------------------------------------------------------------o
void Fame( CChar *nCharId, const SI16 nFame )
{
	SI16 nChange			= 0;
	bool nEffect			= false;
	const SI16 nCurFame		= nCharId->GetFame();
	if( nCharId->IsDead() )
	{
		if( nCurFame <= 0 )
		{
			nCharId->SetFame( 0 );
		}
		else
		{
			nChange = ( nCurFame - 0 ) / 25;
			nCharId->SetFame( static_cast<SI16>( nCurFame - nChange ));
		}
		nCharId->SetDeaths( static_cast<UI16>( nCharId->GetDeaths() + 1 ));
		nEffect = false;
	}
	else if( nCurFame <= nFame )
	{
		nChange = ( nFame - nCurFame ) / 75;
		nCharId->SetFame( static_cast<SI16>( nCurFame + nChange ));
		nEffect = true;
		if( nCharId->GetFame() > 10000 )
		{
			nCharId->SetFame( 10000 );
		}
	}
	else
	{
		return;	// current fame is greater than target fame, and we're not dead
	}

	if( nChange == 0 )
	{
		return;
	}

	CSocket *mSock = nCharId->GetSocket();
	if( mSock == nullptr || nCharId->IsNpc() )
	{
		return;
	}

	if( nChange <= 25 )
	{
		if( nEffect )
		{
			mSock->SysMessage( 1373 ); // You have gained a little fame.
		}
		else
		{
			mSock->SysMessage( 1374 ); // You have lost a little fame.
		}
	}
	else if( nChange <= 50 )
	{
		if( nEffect )
		{
			mSock->SysMessage( 1375 ); // You have gained some fame.
		}
		else
		{
			mSock->SysMessage( 1376 ); // You have lost some fame.
		}
	}
	else
	{
		if( nEffect )
		{
			mSock->SysMessage( 1377 ); // You have gained a lot of fame.
		}
		else
		{
			mSock->SysMessage( 1378 ); // You have lost a lot of fame.
		}
	}
}


