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
#include "townregion.h"

#include "ObjectFactory.h"

namespace UOX
{

#undef DBGFILE
#define DBGFILE "npcs.cpp"

cCharStuff *Npcs = NULL;


//o---------------------------------------------------------------------------o
//|	Function	-	CItem *addRandomLoot( CItem *s, std::string lootlist )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Add loot to monsters packs
//o---------------------------------------------------------------------------o
CItem *cCharStuff::addRandomLoot( CItem *s, const std::string lootlist )
{
	CItem *retitem			= NULL;
	UString sect			= "LOOTLIST " + lootlist;
	ScriptSection *LootList = FileLookup->FindEntry( sect, items_def );
	if( LootList == NULL )
		return NULL;
	size_t i = LootList->NumEntries();
	if( i > 0 )
	{
		i = RandomNum( static_cast< size_t >(0), i - 1 );
		UString tag = LootList->MoveTo( i );
		if( tag.empty() )
			return NULL;
		if( tag.upper() == "LOOTLIST" )
			retitem = addRandomLoot( s, LootList->GrabData() );
		else
		{
			retitem = Items->CreateBaseScriptItem( tag, s->WorldNumber() );
			if( retitem != NULL )
			{
				retitem->SetCont( s );
				retitem->PlaceInPack();
			}
		}
	}
	return retitem;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CChar *CreateBaseNPC( std::string npc, UI08 worldNumber )
//|	Date			-	10/12/2003
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Creates a basic npc from the scripts
//o--------------------------------------------------------------------------o
CChar *cCharStuff::CreateBaseNPC( UString ourNPC )
{
	ourNPC						= ourNPC.stripWhiteSpace();
	ScriptSection *npcCreate	= FileLookup->FindEntry( ourNPC, npc_def );
	if( npcCreate == NULL )
	{
		Console.Error( "CreateBaseNPC(): Bad script npc %s (NPC Not Found).", ourNPC.c_str() );
		return NULL;
	}

	CChar *cCreated = NULL;
	if( npcCreate->NpcListExist() )
		cCreated = CreateRandomNPC( npcCreate->NpcListData() );
	else
	{
		cCreated = static_cast< CChar * >(ObjectFactory::getSingleton().CreateObject( OT_CHAR ));
		if( cCreated == NULL )
			return NULL;

		cCreated->SetSkillTitles( true );
		cCreated->SetNpc( true );
		cCreated->SetLoDamage( 1 );
		cCreated->SetHiDamage( 1 );
		cCreated->SetResist( 1, PHYSICAL );
		cCreated->SetSpawn( INVALIDSERIAL );

		if( !ApplyNpcSection( cCreated, npcCreate ) )
			Console.Error( "Trying to apply an npc section failed" );

		cScript *toGrab = JSMapping->GetScript( cCreated->GetScriptTrigger() );
		if( toGrab != NULL )
			toGrab->OnCreate( cCreated, true );
	}
	return cCreated;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CChar *CreateRandomNPC( std::string npcList )
//|	Date			-	10/12/2003
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Creates a random npc from an npclist in specified dfn file
//o--------------------------------------------------------------------------o
CChar *cCharStuff::CreateRandomNPC( const std::string npcList )
{
	CChar *cCreated			= NULL;
	UString sect			= "NPCLIST " + npcList;
	sect					= sect.stripWhiteSpace();
	ScriptSection *NPCList	= FileLookup->FindEntry( sect, npc_def );
	if( NPCList != NULL )
	{
		const size_t i = NPCList->NumEntries();
		if( i > 0 )
		{
			UString k = NPCList->MoveTo( RandomNum( static_cast< size_t >(0), i - 1 ) );
			if( !k.empty() )
			{
				if( k.upper() == "NPCLIST" )
					cCreated = CreateRandomNPC( NPCList->GrabData() );
				else
					cCreated = CreateBaseNPC( k );
			}
		}
	}
	return cCreated;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CChar *CreateNPC( CSpawnItem *iSpawner, std::string npc )
//|	Date			-	10/12/2003
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Creates an npc spawned from an item spawner
//o--------------------------------------------------------------------------o
CChar *cCharStuff::CreateNPC( CSpawnItem *iSpawner, std::string npc )
{
	const ItemTypes iType = iSpawner->GetType();
	// If the spawner type is 125 and escort quests are not active then abort
	if( iType == IT_ESCORTNPCSPAWNER && !cwmWorldState->ServerData()->EscortsEnabled() )
		return NULL;

	CChar *cCreated = NULL;
	if( iSpawner->IsSectionAList() )
		cCreated = CreateRandomNPC( npc );
	else
		cCreated = CreateBaseNPC( npc );
	if( cCreated == NULL )
		return NULL;

	cCreated->SetSpawn( iSpawner->GetSerial() );
	SI16 awayX = 0, awayY = 0;
	if( iType == IT_AREASPAWNER && iSpawner->GetCont() == NULL )
		awayX = awayY = 10;
	else if( iType == IT_ESCORTNPCSPAWNER && iSpawner->GetCont() == NULL )
	{
		awayX = iSpawner->GetTempVar( CITV_MORE, 3 );
		awayY = iSpawner->GetTempVar( CITV_MORE, 4 );
	}
	FindSpotForNPC( cCreated, iSpawner->GetX(), iSpawner->GetY(), awayX, awayY, iSpawner->GetZ(), iSpawner->WorldNumber() );
	PostSpawnUpdate( cCreated );

	if( iType == IT_ESCORTNPCSPAWNER )
		MsgBoardQuestEscortCreate( cCreated );

	return cCreated;
}

//o--------------------------------------------------------------------------o
//|	Function		-	CChar *CreateNPCxyz( std::string npc, SI16 x, SI16 y, SI08 z, UI08 worldNumber )
//|	Date			-	10/12/2003
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Creates an npc at location xyz
//o--------------------------------------------------------------------------o
CChar *cCharStuff::CreateNPCxyz( std::string npc, SI16 x, SI16 y, SI08 z, UI08 worldNumber )
{
	CChar *cCreated = CreateBaseNPC( npc );
	if( cCreated == NULL )
		return NULL;

	cCreated->SetLocation( x, y, z, worldNumber );
	PostSpawnUpdate( cCreated );
	return cCreated;
}

//o--------------------------------------------------------------------------o
//|	Function		-	PostSpawnUpdate( CChar *cCreated )
//|	Date			-	10/12/2003
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Updates created npc
//o--------------------------------------------------------------------------o
void cCharStuff::PostSpawnUpdate( CChar *cCreated )
{
	CTownRegion *tReg = calcRegionFromXY( cCreated->GetX(), cCreated->GetY(), cCreated->WorldNumber() );
	cCreated->SetRegion( tReg->GetRegionNum() );

	for( UI08 z = 0; z < ALLSKILLS; ++z )
		Skills->updateSkillLevel( cCreated, z );

	UpdateFlag( cCreated );
	cCreated->Update();
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI16 getRadius( CChar *c )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Generate a sensible radius given the values from npc.scp
//o---------------------------------------------------------------------------o
SI16 getRadius( CChar *c )
{
	// see if they supplied a 'radius'
	if( c->GetFx( 1 ) > 0 )
	{
		// if they were supplying a bounding area, use the radius from that
		if( c->GetFx( 0 ) > 0 )
			c->SetFx( abs(c->GetFx( 0 ) - c->GetFx( 1 )), 1 );
	}
	// ensure its not something bogus
	if( c->GetFx( 1 ) <= 0 || c->GetFx( 1 ) > 100)
		c->SetFx( 10, 1 );
	return c->GetFx( 1 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool checkBoundingBox( SI16 xPos, SI16 yPos, SI16 fx1, SI16 fy1, SI08 fz1, SI16 fx2, SI16 fy2 )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check bounding box
//o---------------------------------------------------------------------------o
bool checkBoundingBox( const SI16 xPos, const SI16 yPos, const SI16 fx1, const SI16 fy1, const SI08 fz1, const SI16 fx2, const SI16 fy2, const UI08 worldNumber )
{
	if( xPos >= ( ( fx1 < fx2 ) ? fx1 : fx2 ) && xPos <= ( ( fx1 < fx2 ) ? fx2 : fx1 ) )
	{
		if( yPos >= ( ( fy1 < fy2 ) ? fy1 : fy2 ) && yPos <= ( ( fy1 < fy2 ) ? fy2 : fy1 ) )
		{
			if( fz1 == -1 || abs( fz1 - Map->Height( xPos, yPos, fz1, worldNumber ) ) <= 5 )
				return true;
		}
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	checkBoundingCircle( SI16 xPos, SI16 yPos, SI16 fx1, SI16 fy1, SI08 fz1, SI16 radius, UI08 worldNumber )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Check bounding circle
//o---------------------------------------------------------------------------o
bool checkBoundingCircle( const SI16 xPos, const SI16 yPos, const SI16 fx1, const SI16 fy1, const SI08 fz1, const SI16 radius, const UI08 worldNumber )
{
	if( ( xPos - fx1 ) * ( xPos - fx1 ) + ( yPos - fy1 ) * ( yPos - fy1 ) <= radius * radius )
	{
		if( fz1 == -1 || abs( fz1 - Map->Height( xPos, yPos, fz1, worldNumber ) ) <= 5 )
			return true;
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void InitializeWanderArea( CChar *c, SI16 xAway, SI16 yAway )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Setup the wander area if the npcwander is rect or circle
//o---------------------------------------------------------------------------o
void InitializeWanderArea( CChar *c, SI16 xAway, SI16 yAway )
{
	// compute the rectangular bounding area
	if( WT_BOX == c->GetNpcWander() )
	{
		// if they provided a legal rectangle and
		// ensure the bounding rect contains the current location
		// if it doesn't the monster will never move!
		if( c->GetFx( 0 ) >= 0 && c->GetFy( 0 ) >= 0 && c->GetFy( 1 ) >= 0 && c->GetFx( 1 ) >= 0 &&
			checkBoundingBox( c->GetX(), c->GetY(), c->GetFx( 0 ), c->GetFy( 0 ), c->GetFz(), c->GetFx( 1 ), c->GetFy( 1 ), c->WorldNumber() ) )
		{
			return;// don't do anything to use what they specified in npc.scp		
		}
		else
		{
			// if they provided a 'radius' in the npc.scp use that
			if( c->GetFx( 1 ) > 0 )
				xAway = yAway = getRadius( c );
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
			checkBoundingCircle( c->GetX(), c->GetY(), c->GetFx( 0 ), c->GetFy( 0 ), c->GetFz(), c->GetFx( 1 ), c->WorldNumber() ) )
		{
			return;// don't do anything to use what they specified in npc.scp		
		}
		else
		{
			// if they provided a 'radius' in the npc.scp use that
			if( c->GetFx( 1 ) > 0 )
				xAway = yAway = getRadius( c );
			// setup info for circular areas
			c->SetFx( c->GetX(), 0 );
			c->SetFy( c->GetY(), 0 );
			c->SetFx( xAway, 1 );
			c->SetFy( -1, 1 );
		}
	}
	// setting fz1 actually makes it check against the height and slows the system down a lot
	// does anyone really need to constrain the height at which a monster can move?? - fur
	
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCharStuff::FindSpotForNPC( CChar *cCreated, SI16 originX, SI16 originY, SI16 xAway, SI16 yAway, SI08 z, UI08 worldNumber )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find a valid spot to drop an NPC near the spawners location
//|									
//|	Modification	-	04/20/2002	-	Zippy -  (Type 69) xos and yos (X OffSet, Y OffSet)
//|									are used to find a random number that is then added to the spawner's 
//|									x and y (Using the spawner's z) and then place the NPC anywhere in 
//|									a square around the spawner. This square is random anywhere from -10 
//|									to +10 from the spawner's location (for x and y) If the place chosen 
//|									is not a valid position (the NPC can't walk there) then a new place 
//|									will be chosen, if a valid place cannot be found in a certain # of 
//|									tries (50),the NPC will be placed directly on the spawner and the 
//|									server op will be warned. 
//o---------------------------------------------------------------------------o
void cCharStuff::FindSpotForNPC( CChar *cCreated, const SI16 originX, const SI16 originY, const SI16 xAway, const SI16 yAway, const SI08 z, const UI08 worldNumber )
{

#ifdef DEBUG_SPAWN
	Console.Print( "Going to spawn at (%d,%d) within %d by %d\n", originX, originY, xAway, yAway );
#endif
	
	if( !ValidateObject( cCreated ) )
		return;
	SI32 k = xAway * yAway / 2;
	SI16 xos = 0, yos = 0;
	SI08 targZ = 0;
	bool foundSpot = false;
	if( k > 50 )
		k = 50;
	
	while( !foundSpot )
	{
		targZ = 0;
		if( --k < 0 ) //this CAN be a bit laggy. adjust as nessicary
		{
			if( xAway > 0 && yAway > 0 )
				Console << "Problem area spawner found, NPC placed at default location" << myendl;
			xos = originX;
			yos = originY;
			foundSpot = true;
			break;
		}
	    
		xos = originX + RandomNum( static_cast< SI16 >(-xAway), xAway );
		yos = originY + RandomNum( static_cast< SI16 >(-yAway), yAway );
	    
		if( xos >= 1 && yos >= 1 )
		{
			targZ = Map->Height( xos, yos, z, worldNumber );
			if( !cwmWorldState->creatures[cCreated->GetID()].IsWater() )
				foundSpot = Map->ValidSpawnLocation( xos, yos, targZ, worldNumber );
			else if( cwmWorldState->creatures[cCreated->GetID()].IsWater() || ( !foundSpot && cwmWorldState->creatures[cCreated->GetID()].IsAmphibian() ) )
				foundSpot = Map->ValidSpawnLocation( xos, yos, targZ, worldNumber, false );
		}
	}

	cCreated->SetLocation( xos, yos, targZ, worldNumber );
	InitializeWanderArea( cCreated, xAway, yAway );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCharStuff::LoadShopList( std::string list, CChar *c )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Loads the shopping list pointed to by data in items.scp
//o---------------------------------------------------------------------------o
void cCharStuff::LoadShopList( const std::string list, CChar *c )
{
	CItem *buyLayer		= c->GetItemAtLayer( IL_BUYCONTAINER );
	CItem *boughtLayer	= c->GetItemAtLayer( IL_BOUGHTCONTAINER );
	CItem *sellLayer	= c->GetItemAtLayer( IL_SELLCONTAINER );

	UString sect		= "SHOPLIST " + list;
	sect				= sect.stripWhiteSpace();
	ScriptSection *ShoppingList = FileLookup->FindEntry( sect, items_def );
	if( ShoppingList == NULL )
		return;

	UString cdata;
	SI32 ndata		= -1, odata = -1;
	CItem *retitem	= NULL;
	for( DFNTAGS tag = ShoppingList->FirstTag(); !ShoppingList->AtEndTags(); tag = ShoppingList->NextTag() )
	{
		cdata = ShoppingList->GrabData( ndata, odata );
		switch( tag )
		{
			case DFNTAG_RSHOPITEM:
				if( ValidateObject( buyLayer ) )
				{
					retitem = Items->CreateBaseScriptItem( cdata, c->WorldNumber() );
					if( retitem != NULL )
					{
						retitem->SetCont( buyLayer );
						retitem->PlaceInPack();
						if( retitem->GetName2()[0] && ( strcmp( retitem->GetName2(), "#" ) ) )
							retitem->SetName( retitem->GetName2() ); // Item identified! -- by Magius(CHE)
					}
				}
				else
					Console << "Warning: Bad Shopping List " << list << " with no Vendor Buy Pack for NPC " << c << " (serial: " << c->GetSerial() << myendl;
				break;
			case DFNTAG_SELLITEM:
				if( ValidateObject( sellLayer ) )
				{
					retitem = Items->CreateBaseScriptItem( cdata, c->WorldNumber() );
					if( retitem != NULL )
					{
						retitem->SetCont( sellLayer );
						retitem->SetSellValue( retitem->GetBuyValue() / 2 );
						retitem->PlaceInPack();
						if( retitem->GetName2()[0] && ( strcmp( retitem->GetName2(), "#" ) ) )
							retitem->SetName( retitem->GetName2() );
					}
				}
				else
					Console << "Warning: Bad Shopping List " << list << " with no Vendor Sell Pack for NPC " << c << " (serial: " << c->GetSerial() << myendl;
				break;
			case DFNTAG_SHOPITEM:
				if( ValidateObject( boughtLayer ) )
				{
					retitem = Items->CreateBaseScriptItem( cdata, c->WorldNumber() );
					if( retitem != NULL )
					{
						retitem->SetCont( boughtLayer );
						retitem->PlaceInPack();
						if( retitem->GetName2()[0] && ( strcmp( retitem->GetName2(), "#" ) ) )
							retitem->SetName( retitem->GetName2() );
					}
				}
				else
					Console << "Warning: Bad Shopping List " << list << " with no Vendor Bought Pack for NPC " << c << " (serial: " << c->GetSerial() << myendl;
				break;
			case DFNTAG_VALUE:
				if( retitem != NULL )
				{
					if( !cdata.empty() )
					{
						if( cdata.sectionCount( " " ) != 0 )
						{
							retitem->SetBuyValue(  cdata.section( " ", 0, 0 ).stripWhiteSpace().toULong() );
							retitem->SetSellValue( cdata.section( " ", 1, 1 ).stripWhiteSpace().toULong() );
							break;
						}
					}
					retitem->SetBuyValue( ndata );
					retitem->SetSellValue( (ndata / 2) );
				}
				break;
			default:
				break;
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void setRandomName( CChar *s, std::string namelist )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Sets a character with a random name from NPC.scp namelist
//o---------------------------------------------------------------------------o
void setRandomName( CChar *s, const std::string namelist )
{
	UString sect	= "RANDOMNAME " + namelist;
	sect			= sect.stripWhiteSpace();
	UString tempName;

	ScriptSection *RandomName = FileLookup->FindEntry( sect, npc_def );
	if( RandomName == NULL )
		tempName = UString::sprintf( "Error Namelist %s Not Found", namelist.c_str() );
	else
	{
		size_t i = RandomName->NumEntries();
		if( i > 0 )
		{
			i = RandomNum( static_cast<size_t>(0), i - 1 );
			tempName = RandomName->MoveTo( static_cast<SI16>(i) );
		}
	}
	s->SetName( tempName );
}

//o---------------------------------------------------------------------------o
//|	Function	-	int addRandomColor( std::string colorlist )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Randomly colors character
//o---------------------------------------------------------------------------o
UI16 addRandomColor( const std::string colorlist )
{
	UString sect					= "RANDOMCOLOR " + colorlist;
	sect							= sect.stripWhiteSpace();
	ScriptSection *RandomColours	= FileLookup->FindEntry( sect, colors_def );
	if( RandomColours == NULL )
	{
		Console.Warning( "Error Colorlist %s Not Found", colorlist.c_str() );
		return 0;
	}
	size_t i = RandomColours->NumEntries();
	if( i > 0 )
	{
		i = RandomNum( static_cast<size_t>(0), i - 1 );
		UString tag = RandomColours->MoveTo( static_cast<SI16>(i) );
		return tag.toUShort();
	}
	return 0;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCommands::MakeShop( CChar *c )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Turn an NPC into a shopkeeper
//o---------------------------------------------------------------------------o
void MakeShop( CChar *c )
{
	if( !ValidateObject( c ) )
		return;
	c->SetShop( true );
	CItem *tPack = NULL;
	for( UI08 i = IL_BUYCONTAINER; i <= IL_SELLCONTAINER; ++i )
	{
		tPack = c->GetItemAtLayer( static_cast<ItemLayers>(i) );
		if( !ValidateObject( tPack ) )
		{
			tPack = Items->CreateItem( NULL, c, 0x2AF8, 1, 0, OT_ITEM );
			if( ValidateObject( tPack ) )
			{
				tPack->SetDecayable( false );
				tPack->SetLayer( static_cast<ItemLayers>(i) );
				if( !tPack->SetCont( c ) )
					tPack->Delete();
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

//o---------------------------------------------------------------------------o
//|	Function	-	bool cCharStuff::ApplyNpcSection( CChar *applyTo, ScriptSection *NpcCreation, bool isGate )
//|	Programmer	-	UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|	Purpose		-	Apply Npc.scp sections to an NPC
//o---------------------------------------------------------------------------o
bool cCharStuff::ApplyNpcSection( CChar *applyTo, ScriptSection *NpcCreation, bool isGate )
{
	if( NpcCreation == NULL || !ValidateObject( applyTo ) )
		return false;

	UI16 haircolor	= INVALIDCOLOUR;
	CItem *buyPack	= NULL, *boughtPack = NULL, *sellPack = NULL;
	CItem *retitem	= NULL, *mypack = NULL;

	UString cdata;
	SI32 ndata		= -1, odata = -1;
	UI08 skillToSet = 0xFF;

	TAGMAPOBJECT customTag;
	UString customTagName;
	UString customTagStringValue;

	for( DFNTAGS tag = NpcCreation->FirstTag(); !NpcCreation->AtEndTags(); tag = NpcCreation->NextTag() )
	{
		cdata = NpcCreation->GrabData( ndata, odata );
		switch( tag )
		{
			case DFNTAG_ALCHEMY:			skillToSet = ALCHEMY;				break;
			case DFNTAG_ANATOMY:			skillToSet = ANATOMY;				break;
			case DFNTAG_ANIMALLORE: 		skillToSet = ANIMALLORE;			break;
			case DFNTAG_ARMSLORE:			skillToSet = ARMSLORE;				break;
			case DFNTAG_ARCHERY:			skillToSet = ARCHERY;				break;
			case DFNTAG_DAMAGE:
			case DFNTAG_ATT:
											applyTo->SetLoDamage( static_cast<SI16>(ndata) );
											applyTo->SetHiDamage( static_cast<SI16>(odata) );
											break;
			case DFNTAG_BACKPACK:
											if( !isGate )
											{
												if( mypack == NULL )
													mypack = applyTo->GetPackItem();
												if( mypack == NULL )
												{
													mypack = Items->CreateItem( NULL, applyTo, 0x0E75, 1, 0, OT_ITEM );
													if( ValidateObject( mypack ) )
													{
														mypack->SetDecayable( false );
														applyTo->SetPackItem( mypack );
														mypack->SetName( "Backpack" );
														mypack->SetLayer( IL_PACKITEM );
														if( !mypack->SetCont( applyTo ) )
															mypack = NULL;
														else
														{
															mypack->SetX( 0 );
															mypack->SetY( 0 );
															mypack->SetZ( 0 );
															mypack->SetType( IT_CONTAINER );
															mypack->SetDye( true );
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
												applyTo->SetCarve( static_cast<SI16>(ndata) );
											break;
			case DFNTAG_CHIVALRY:			skillToSet = CHIVALRY;				break;
			case DFNTAG_COOKING:			skillToSet = COOKING;				break;
			case DFNTAG_COLOUR:
											if( retitem != NULL )
												retitem->SetColour( static_cast<UI16>(ndata) );
											break;
			case DFNTAG_COLOURLIST:
											if( retitem != NULL )
												retitem->SetColour( addRandomColor( cdata ) );
											break;
			case DFNTAG_COLOURMATCHHAIR:
											if( retitem != NULL )
												retitem->SetColour( static_cast<UI16>(haircolor) );
											break;
			case DFNTAG_ELEMENTRESIST:
											if( cdata.sectionCount( " " ) == 3 )
											{
												applyTo->SetResist( ( cdata.section( " ", 0, 0 ).stripWhiteSpace().toUShort() ), HEAT );
												applyTo->SetResist( ( cdata.section( " ", 1, 1 ).stripWhiteSpace().toUShort() ), COLD );
												applyTo->SetResist( ( cdata.section( " ", 2, 2 ).stripWhiteSpace().toUShort() ), LIGHTNING );
												applyTo->SetResist( ( cdata.section( " ", 3, 3 ).stripWhiteSpace().toUShort() ), POISON );
											}
											break;
			case DFNTAG_DEX:
											applyTo->SetDexterity( static_cast<SI16>(RandomNum( ndata, odata )) );
											applyTo->SetStamina( applyTo->GetMaxStam() );
											break;
			case DFNTAG_DETECTINGHIDDEN:	skillToSet = DETECTINGHIDDEN;			break;
			case DFNTAG_DEF:				applyTo->SetResist( static_cast<UI16>(RandomNum( ndata, odata )), PHYSICAL ); break;
			case DFNTAG_DIR:
											if( !isGate )
											{
												UString cupper = cdata.upper();
												if( cupper == "NE" )
													applyTo->SetDir( NORTHEAST );
												else if( cupper == "E" )
													applyTo->SetDir( EAST );
												else if( cupper == "SE" )
													applyTo->SetDir( SOUTHEAST );
												else if( cupper == "S" )
													applyTo->SetDir( SOUTH );
												else if( cupper == "SW" )
													applyTo->SetDir( SOUTHWEST );
												else if( cupper == "W" )
													applyTo->SetDir( WEST );
												else if( cupper == "NW" )
													applyTo->SetDir( NORTHWEST );
												else if( cupper == "N" )
													applyTo->SetDir( NORTH );
											}
											break;
			case DFNTAG_EMOTECOLOUR:		
											if( !isGate )
												applyTo->SetEmoteColour( static_cast<UI16>(ndata) );
											break;
			case DFNTAG_ENTICEMENT:			skillToSet = ENTICEMENT;				break;
			case DFNTAG_EQUIPITEM:
											if( !isGate )
											{
												retitem = Items->CreateBaseScriptItem( cdata, applyTo->WorldNumber() );
												if( retitem != NULL )
												{
													if( retitem->GetLayer() == IL_NONE )
														Console << "Warning: Bad NPC Script with problem item " << cdata << " executed!" << myendl;
													else if( !retitem->SetCont( applyTo ) )
													{
														if( !retitem->SetCont( applyTo->GetPackItem() ) )
															retitem = NULL;
													}
												}
											}
											break;
			case DFNTAG_EVALUATINGINTEL:	skillToSet = EVALUATINGINTEL;			break;
			case DFNTAG_FAME:				applyTo->SetFame( static_cast<SI16>(ndata) );		break;
			case DFNTAG_FENCING:			skillToSet = FENCING;					break;
			case DFNTAG_FISHING:			skillToSet = FISHING;					break;
			case DFNTAG_FLEEAT:								
											if( !isGate )
												applyTo->SetFleeAt( static_cast<SI16>(ndata) );
											break;
			case DFNTAG_FLEEINGSPEED:
												applyTo->SetFleeingSpeed( cdata.stripWhiteSpace().toFloat() );
											break;
			case DFNTAG_FLAG:				
											if( !isGate )
											{
												if( !cdata.empty() )
												{
													UString UDat = UString( cdata ).upper();
													if( UDat == "NEUTRAL" )
														applyTo->SetNPCFlag( fNPC_NEUTRAL );
													else if( UDat == "INNOCENT" )
														applyTo->SetNPCFlag( fNPC_INNOCENT );
													else if( UDat == "EVIL" )
														applyTo->SetNPCFlag( fNPC_EVIL );
												}
											}
											break;
			case DFNTAG_FORENSICS:			skillToSet = FORENSICS;					break;
			case DFNTAG_FOCUS:				skillToSet = FOCUS;						break;
			case DFNTAG_FX1:								
											if( !isGate )
												applyTo->SetFx( static_cast<SI16>(ndata), 0 );
											break;
			case DFNTAG_FX2:								
											if( !isGate )
												applyTo->SetFx( static_cast<SI16>(ndata), 1 );
											break;
			case DFNTAG_FY1:				
											if( !isGate )
												applyTo->SetFy( static_cast<SI16>(ndata), 0 );
											break;
			case DFNTAG_FY2:								
											if( !isGate )
												applyTo->SetFy( static_cast<SI16>(ndata), 1 );
											break;
			case DFNTAG_FZ1:								
											if( !isGate )
												applyTo->SetFz( static_cast<SI08>(ndata) );
											break;
			case DFNTAG_FOOD:				applyTo->SetFood( cdata );				break;
			case DFNTAG_GET:
			{
											ScriptSection *toFind = FileLookup->FindEntry( cdata, npc_def );
											if( toFind == NULL )
												Console.Warning( "Invalid script entry called with GET tag, character serial 0x%X" , applyTo->GetSerial() );
											else
												ApplyNpcSection( applyTo, toFind, isGate );
			}
											break;
			case DFNTAG_GOLD:
											if( !isGate )
											{
												if( !ValidateObject( mypack ) )
													mypack = applyTo->GetPackItem();
												if( ValidateObject( mypack ) )
													retitem = Items->CreateItem( NULL, applyTo, 0x0EED, static_cast<UI16>(RandomNum( ndata, odata )), 0, OT_ITEM, true );
												else
													Console.Warning( "Bad NPC Script with problem no backpack for gold" );
											}
											break;
			case DFNTAG_HAIRCOLOUR:
											haircolor = addRandomColor( cdata );
											if( retitem != NULL )
												retitem->SetColour( haircolor );
											break;
			case DFNTAG_HEALING:			skillToSet = HEALING;					break;
			case DFNTAG_HERDING:			skillToSet = HERDING;					break;
			case DFNTAG_HIDING:				skillToSet = HIDING;					break;
			case DFNTAG_HIDAMAGE:			applyTo->SetHiDamage( static_cast<SI16>(ndata) );	break;
			case DFNTAG_HP:					applyTo->SetHP( static_cast<SI16>(RandomNum( ndata, odata )) );			break;
			case DFNTAG_HPMAX:				applyTo->SetFixedMaxHP( static_cast<SI16>(RandomNum( ndata, odata )) );			break;
			case DFNTAG_ID:
											applyTo->SetID( static_cast<UI16>(ndata) );
											applyTo->SetOrgID( static_cast<UI16>(ndata) );
											break;
			case DFNTAG_INSCRIPTION:		skillToSet = INSCRIPTION;				break;
			case DFNTAG_INTELLIGENCE:
											applyTo->SetIntelligence( static_cast<SI16>(RandomNum( ndata, odata )) );
											applyTo->SetMana( applyTo->GetMaxMana() );
											break;
			case DFNTAG_ITEMID:				skillToSet = ITEMID;					break;
			case DFNTAG_KARMA:				applyTo->SetKarma( static_cast<SI16>(ndata) );		break;
			case DFNTAG_LOOT:
											if( !isGate )
											{
												if( !ValidateObject( mypack ) )
													mypack = applyTo->GetPackItem();
												if( ValidateObject( mypack ) )
													retitem = addRandomLoot( mypack, cdata );
												else
													Console.Warning( "Bad NPC Script with problem no backpack for loot" );
											}
											break;
			case DFNTAG_LOCKPICKING:		skillToSet = LOCKPICKING;				break;
			case DFNTAG_LODAMAGE:			applyTo->SetLoDamage( static_cast<SI16>(ndata) );	break;
			case DFNTAG_LUMBERJACKING:		skillToSet = LUMBERJACKING;				break;
			case DFNTAG_MACEFIGHTING:		skillToSet = MACEFIGHTING;				break;
			case DFNTAG_MAGERY:				skillToSet = MAGERY;					break;
			case DFNTAG_MAGICRESISTANCE:	skillToSet = MAGICRESISTANCE;			break;
			case DFNTAG_MANA:				applyTo->SetMana( static_cast<SI16>(RandomNum( ndata, odata )) );		break;
			case DFNTAG_MANAMAX:			applyTo->SetFixedMaxMana( static_cast<SI16>(RandomNum( ndata, odata )) );			break;
			case DFNTAG_MEDITATION:			skillToSet = MEDITATION;				break;
			case DFNTAG_MINING:				skillToSet = MINING;					break;
			case DFNTAG_MUSICIANSHIP:		skillToSet = MUSICIANSHIP;				break;
			case DFNTAG_NAME:				applyTo->SetName( cdata );				break;
			case DFNTAG_NAMELIST:			setRandomName( applyTo, cdata );		break;
			case DFNTAG_NECROMANCY:			skillToSet = NECROMANCY;				break;
			case DFNTAG_NINJITSU:			skillToSet = NINJITSU;					break;
			case DFNTAG_NPCWANDER:			
											if( !isGate )
												applyTo->SetNpcWander( static_cast<SI08>(ndata) );
											break;
			case DFNTAG_NPCAI:				
											if( !isGate )
												applyTo->SetNPCAiType( static_cast<SI16>(ndata) );
											break;
			case DFNTAG_NOTRAIN:			
											if( !isGate )
												applyTo->SetCanTrain( false );
											break;
			case DFNTAG_PACKITEM:
											if( !isGate )
											{
												if( !ValidateObject( mypack ) )
													mypack = applyTo->GetPackItem();
												if( ValidateObject( mypack ) )
												{
													if( !cdata.empty() )
													{
														if( cdata.sectionCount( "," ) != 0 )
															retitem = Items->CreateScriptItem( NULL, applyTo, cdata.section( ",", 0, 0 ).stripWhiteSpace(), cdata.section( ",", 1, 1 ).stripWhiteSpace().toUShort(), OT_ITEM, true );
														else
															retitem = Items->CreateScriptItem( NULL, applyTo, cdata, 1, OT_ITEM, true );
													}
												}
												else
													Console << "Warning: Bad NPC Script with problem no backpack for packitem" << myendl;
											}
											break;
			case DFNTAG_POISONSTRENGTH:		applyTo->SetPoisonStrength( static_cast<UI08>(ndata) ); break;
			case DFNTAG_PRIV:				
											if( !isGate )
												applyTo->SetPriv( static_cast<UI16>(ndata) );
											break;
			case DFNTAG_PARRYING:			skillToSet = PARRYING;					break;
			case DFNTAG_PEACEMAKING:		skillToSet = PEACEMAKING;				break;
			case DFNTAG_PROVOCATION:		skillToSet = PROVOCATION;				break;
			case DFNTAG_POISONING:			skillToSet = POISONING;					break;
			case DFNTAG_RSHOPITEM:
											if( !isGate )
											{
												if( !ValidateObject( buyPack ) )
													buyPack = applyTo->GetItemAtLayer( IL_BUYCONTAINER );
												if( ValidateObject( buyPack ) )
												{
													retitem = Items->CreateBaseScriptItem( cdata, applyTo->WorldNumber() );
													if( retitem != NULL )
													{
														retitem->SetCont( buyPack );
														retitem->PlaceInPack();
														if( retitem->GetName2()[0] && ( strcmp( retitem->GetName2(), "#" )))
															retitem->SetName( retitem->GetName2() ); // Item identified! -- by Magius(CHE)
													}
												}
												else
													Console.Warning( "Bad NPC Script with no Vendor Buy Pack for item" );
											}
											break;
			case DFNTAG_REATTACKAT:			
											if( !isGate )
												applyTo->SetReattackAt( static_cast<SI16>(ndata) );
											break;
			case DFNTAG_REMOVETRAPS:		skillToSet = REMOVETRAPS;				break;
			case DFNTAG_RACE:				applyTo->SetRace( static_cast<UI16>(ndata));		break;
			case DFNTAG_RUNS:				
											if( !isGate )
												applyTo->SetRun( true );
											break;
			case DFNTAG_RUNNINGSPEED:
												applyTo->SetRunningSpeed( cdata.stripWhiteSpace().toFloat() );
											break;
			case DFNTAG_SKIN:				applyTo->SetSkin( static_cast<UI16>(ndata) );		break;
			case DFNTAG_SHOPKEEPER:			
											if( !isGate )
												MakeShop( applyTo );
											break;
			case DFNTAG_SHOPLIST:			
											if( !isGate )
												LoadShopList( cdata, applyTo );
											break;
			case DFNTAG_SELLITEM:
											if( !isGate )
											{
												if( !ValidateObject( sellPack ) )
													sellPack = applyTo->GetItemAtLayer( IL_SELLCONTAINER );
												if( ValidateObject( sellPack ) )
												{
													retitem = Items->CreateBaseScriptItem( cdata, applyTo->WorldNumber() );
													if( retitem != NULL )
													{
														retitem->SetCont( sellPack );
														retitem->PlaceInPack();
														if( retitem->GetName2()[0] && ( strcmp( retitem->GetName2(), "#" ) ) )
															retitem->SetName( retitem->GetName2() );
													}
												}
												else
													Console.Warning( "Bad NPC Script with no Vendor SellPack for item" );
											}
											break;
			case DFNTAG_SHOPITEM:
											if( !isGate )
											{
												if( !ValidateObject( boughtPack ) )
													boughtPack = applyTo->GetItemAtLayer( IL_BOUGHTCONTAINER );
												if( ValidateObject( boughtPack ) )
												{
													retitem = Items->CreateBaseScriptItem( cdata, applyTo->WorldNumber() );
													if( retitem != NULL )
													{
														retitem->SetCont( boughtPack );
														retitem->PlaceInPack();
														if( retitem->GetName2()[0] && ( strcmp( retitem->GetName2(), "#" ) ) )
															retitem->SetName( retitem->GetName2() );
													}
												}
												else
													Console.Warning( "Bad NPC Script with no Vendor Bought Pack for item" );
											}
											break;
			case DFNTAG_SAYCOLOUR:			
											if( !isGate )
												applyTo->SetSayColour( static_cast<UI16>(ndata) );
											break;
			case DFNTAG_SPATTACK:			
											if( !isGate )
												applyTo->SetSpAttack( static_cast<SI16>(ndata) );
											break;
			case DFNTAG_SPADELAY:			
											if( !isGate )
												applyTo->SetSpDelay( static_cast<SI08>(ndata) );
											break;
			case DFNTAG_SPLIT:				
											if( !isGate )
												applyTo->SetSplit( static_cast<UI08>(ndata) );
											break;
			case DFNTAG_SPLITCHANCE:		
											if( !isGate )
												applyTo->SetSplitChance( static_cast<UI08>(ndata) );
											break;
			case DFNTAG_SNOOPING:			skillToSet = SNOOPING;					break;
			case DFNTAG_SPIRITSPEAK:		skillToSet = SPIRITSPEAK;				break;
			case DFNTAG_STEALING:			skillToSet = STEALING;					break;
			case DFNTAG_STRENGTH:
											applyTo->SetStrength( static_cast<SI16>(RandomNum( ndata, odata )) );
											applyTo->SetHP( applyTo->GetMaxHP() );
											break;
			case DFNTAG_SWORDSMANSHIP:		skillToSet = SWORDSMANSHIP;				break;
			case DFNTAG_STAMINA:			applyTo->SetStamina( static_cast<SI16>(RandomNum( ndata, odata )) );		break;
			case DFNTAG_STAMINAMAX:			applyTo->SetFixedMaxStam( static_cast<SI16>(RandomNum( ndata, odata )) );		break;
			case DFNTAG_STEALTH:			skillToSet = STEALTH;					break;
			case DFNTAG_SKINLIST:			applyTo->SetSkin( addRandomColor( cdata ) );			break;
			case DFNTAG_SKILL:				applyTo->SetBaseSkill( static_cast<UI16>(odata), static_cast<UI08>(ndata) ); break;
			case DFNTAG_SCRIPT:				
											if( !isGate )
												applyTo->SetScriptTrigger( static_cast<UI16>(ndata) );
											break;
			case DFNTAG_TITLE:				applyTo->SetTitle( cdata );		break;
			case DFNTAG_TOTAME:				
											if( !isGate )
												applyTo->SetTaming( static_cast<SI16>(ndata) );
											break;
			case DFNTAG_TOPROV:				
											if( !isGate )
												applyTo->SetProvoing( static_cast<SI16>(ndata) );
											break;
			case DFNTAG_TOPEACE:				
											if( !isGate )
												applyTo->SetPeaceing( static_cast<SI16>(ndata) );
												applyTo->SetBrkPeaceChanceGain( static_cast<SI16>(odata) );
											break;
			case DFNTAG_TAMEDHUNGER:
											if( !isGate )
											{
												applyTo->SetTamedHungerRate( static_cast<UI16>(ndata) );
												applyTo->SetTamedHungerWildChance( static_cast<UI08>(odata) );
											}
											break;
			case DFNTAG_WILLHUNGER:
											if( !isGate )
											{
												if( ndata > 0 )
													applyTo->SetHungerStatus( true );
												else
													applyTo->SetHungerStatus( false );
											}
											break;
			case DFNTAG_WALKINGSPEED:
												applyTo->SetWalkingSpeed( cdata.stripWhiteSpace().toFloat() );
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
				customTagName			= cdata.section( " ", 0, 0 );
				customTagStringValue	= cdata.section( " ", 1 );
				if( !customTagName.empty() && !customTagStringValue.empty() )
				{
					customTag.m_Destroy		= FALSE;
					customTag.m_StringValue	= customTagStringValue;
					customTag.m_IntValue	= customTag.m_StringValue.length();
					customTag.m_ObjectType	= TAGMAP_TYPE_STRING;
					applyTo->SetTag( customTagName, customTag);
				}
				break;
			case DFNTAG_CUSTOMINTTAG:
				customTagName			= cdata.section(" ", 0, 0);
				customTagStringValue	= cdata.section(" ", 1);
				if( !customTagName.empty() && !customTagStringValue.empty() )
				{
					customTag.m_Destroy		= FALSE;
					customTag.m_IntValue	= customTagStringValue.toInt();
					customTag.m_ObjectType	= TAGMAP_TYPE_INT;
					customTag.m_StringValue	= "";
					applyTo->SetTag( customTagName, customTag);
				}
				break;
			default:						Console << "Unknown tag in ApplyNpcSection(): " << (SI32)tag << myendl; break;
		}
		if( skillToSet != 0xFF )
		{
			applyTo->SetBaseSkill( static_cast<UI16>(RandomNum( ndata, odata )), skillToSet );
			skillToSet = 0xFF;
		}
	}
	return true;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CChar * cCharStuff::getGuardingPet( CChar *mChar, CBaseObject *guarded )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the pet guarding an item / character
//o---------------------------------------------------------------------------o
CChar * cCharStuff::getGuardingPet( CChar *mChar, CBaseObject *guarded )
{
	if( !ValidateObject( mChar ) || !ValidateObject( guarded ) )
		return NULL;

	CDataList< CChar * > *myPets = mChar->GetPetList();
	for( CChar *pet = myPets->First(); !myPets->Finished(); pet = myPets->Next() )
	{
		if( ValidateObject( pet ) )
		{
			if( !pet->GetMounted() && pet->GetNPCAiType() == AI_PET_GUARD && 
				pet->GetGuarding() == guarded && pet->GetOwnerObj() == mChar )
				return pet;
		}
	}
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	bool cCharStuff::checkPetFriend( CChar *mChar, CChar *pet )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Search a pets friends list to check if a character is a friend
//o---------------------------------------------------------------------------o
bool cCharStuff::checkPetFriend( CChar *mChar, CChar *pet )
{
	CChar *getFriend		= NULL;
	CHARLIST *petFriends	= pet->GetFriendList();
	if( petFriends != NULL )
	{
		for( CHARLIST_CITERATOR I = petFriends->begin(); I != petFriends->end(); ++I )
		{
			getFriend = (*I);
			if( ValidateObject( getFriend ) && getFriend == mChar )
				return true;
		}
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|	Function	-	void cCharStuff::stopPetGuarding( CChar *pet )
//|	Programmer	-	Zane
//o---------------------------------------------------------------------------o
//|	Purpose		-	Find the item/char pet is guarding and set it to not guarded
//o---------------------------------------------------------------------------o
void cCharStuff::stopPetGuarding( CChar *pet )
{
	CBaseObject *petGuarding = pet->GetGuarding();
	if( !ValidateObject( petGuarding ) )
		return;

	if( petGuarding->GetObjType() == OT_ITEM )
	{
		CItem *itemGuard = static_cast<CItem *>(petGuarding);
		if( ValidateObject( itemGuard ) )
			itemGuard->SetGuarded( false );
	}
	else
	{
		CChar *charGuard = static_cast<CChar *>(petGuarding);
		if( ValidateObject( charGuard ) )
			charGuard->SetGuarded( false );
	}
	pet->SetGuarding( NULL );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void MonsterGate( CChar *s, SI32 x )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle monster gates (polymorphs players into monster bodies)
//o---------------------------------------------------------------------------o
void MonsterGate( CChar *s, const std::string scriptEntry )
{
	CItem *mypack = NULL, *retitem = NULL;
	if( s->IsNpc() ) 
		return;

	ScriptSection *Monster = FileLookup->FindEntry( UString( scriptEntry ).stripWhiteSpace(), npc_def );
	if( Monster == NULL )
		return;
	
	s->SetTitle( "\0" );

	CItem *n;
	for( CItem *z = s->FirstItem(); !s->FinishedItems(); z = s->NextItem() )
	{
		if( ValidateObject( z ) )
		{
			if( z->isFree() )
				continue;
			if( z->GetLayer() == IL_HAIR || z->GetLayer() == IL_FACIALHAIR )
				z->Delete();
			else if( z->GetLayer() != IL_PACKITEM && z->GetLayer() != IL_BANKBOX )
			{
				if( mypack == NULL )
					mypack = s->GetPackItem();
				if( mypack == NULL )
				{
					n = Items->CreateItem( NULL, s, 0x0E75, 1, 0, OT_ITEM );
					if( !ValidateObject( n ) ) 
						return;
					s->SetPackItem( n );
					n->SetDecayable( false );
					n->SetLayer( IL_PACKITEM );
					if( n->SetCont( s ) )
					{
						n->SetType( IT_CONTAINER );
						n->SetDye( true );
						mypack = n;
						retitem = n;
					}
				}
				z->SetCont( mypack );
				z->PlaceInPack();
			}
		}
	}
	
	Npcs->ApplyNpcSection( s, Monster, true );
	//Now find real 'skill' based on 'baseskill' (stat modifiers)
	for( UI08 j = 0; j < ALLSKILLS; ++j )
		Skills->updateSkillLevel( s, j );
	s->Update();
	Effects->PlayStaticAnimation( s, 0x373A, 0, 15 );
	Effects->PlaySound( s, 0x01E9 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void Karma( CChar *nCharID, CChar *nKilledID, SI16 nKarma )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle karma addition/subtraction when character kills
//|					another Character / NPC
//o---------------------------------------------------------------------------o
void Karma( CChar *nCharID, CChar *nKilledID, const SI16 nKarma )
{
	SI16 nChange	= 0;
	bool nEffect	= false;
	SI16 nCurKarma	= nCharID->GetKarma();
	if( nCurKarma > 10000 )
	{
		nCharID->SetKarma( 10000 );
		nCurKarma = 10000;
	}
	else if( nCurKarma < -10000 ) 
	{
		nCharID->SetKarma( -10000 );
		nCurKarma = -10000;
	}

	if( nCurKarma < nKarma && nKarma > 0 )
	{
		nChange = ( ( nKarma - nCurKarma ) / 75 );
		nCharID->SetKarma( (SI16)( nCurKarma + nChange ) );
		nEffect = true;
	}
	if( nCurKarma > nKarma && ( !ValidateObject( nKilledID ) || nKilledID->GetKarma() > 0 ) )
	{
		nChange = ( ( nCurKarma - nKarma ) / 50 );
		nCharID->SetKarma( (SI16)( nCurKarma - nChange ) );
		nEffect = false;
	}
	if( nChange == 0 )	// NPCs CAN gain/lose karma
		return;

	CSocket *mSock = nCharID->GetSocket();
	if( nCharID->IsNpc() || mSock == NULL )
		return;
	if( nChange <= 25 )
	{
		if( nEffect )
			mSock->sysmessage( 1367 );
		else
			mSock->sysmessage( 1368 );
		return;
	}
	if( nChange <= 50 )
	{
		if( nEffect )
			mSock->sysmessage( 1369 );
		else
			mSock->sysmessage( 1370 );
		return;
	}
	if( nEffect )
		mSock->sysmessage( 1371 );
	else
		mSock->sysmessage( 1372 );
}

//o---------------------------------------------------------------------------o
//|	Function	-	void Fame( CChar *nCharID, SI16 nFame )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Handle fame addition when character kills another
//|					Character / NPC
//o---------------------------------------------------------------------------o
void Fame( CChar *nCharID, const SI16 nFame )
{
	SI16 nChange			= 0;
	bool nEffect			= false;
	const SI16 nCurFame		= nCharID->GetFame();
	if( nCharID->IsDead() )
	{
		if( nCurFame <= 0 )
			nCharID->SetFame( 0 );
		else
		{
			nChange = ( nCurFame - 0 ) / 25;
			nCharID->SetFame( (SI16)( nCurFame - nChange ) );
		}
		nCharID->SetDeaths( (UI16)( nCharID->GetDeaths() + 1 ) );
		nEffect = false;
	}
	else if( nCurFame <= nFame )
	{
		nChange = ( nFame - nCurFame ) / 75;
		nCharID->SetFame( (SI16)( nCurFame + nChange ) );
		nEffect = true;
		if( nCharID->GetFame() > 10000 )
			nCharID->SetFame( 10000 );
	}
	else
		return;	// current fame is greater than target fame, and we're not dead
	if( nChange == 0 )
		return;
	CSocket *mSock = nCharID->GetSocket();
	if( mSock == NULL || nCharID->IsNpc() )
		return;
	if( nChange <= 25 )
	{
		if( nEffect )
			mSock->sysmessage( 1373 );
		else
			mSock->sysmessage( 1374 );
	}
	else if( nChange <= 50 )
	{
		if( nEffect )
			mSock->sysmessage( 1375 );
		else
			mSock->sysmessage( 1376 );
	}
	else
	{
		if( nEffect )
			mSock->sysmessage( 1377 );
		else
			mSock->sysmessage( 1378 );
	}
}

}


