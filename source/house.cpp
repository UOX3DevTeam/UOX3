// House code for deed creation
#include<algorithm>

#include "uox3.h"
#include "mapstuff.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "CPacketSend.h"
#include "classes.h"
#include "regions.h"
#include "Dictionary.h"
#include "StringUtility.hpp"


#include "ObjectFactory.h"

bool CreateBoat( CSocket *s, CBoatObj *b, UI08 id2, UI08 boattype );

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void DoHouseTarget( CSocket *mSock, UI08 houseEntry )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggered by double clicking a deed-> the deed's moreX is read for the house
//|					section in house.dfn. Extra items can be added using HOUSE ITEM, (this includes
//|					all doors!) and locked "LOCK" Space around the house with SPACEX/Y and CHAR
//|					offset CHARX/Y/Z
//o-----------------------------------------------------------------------------------------------o
void DoHouseTarget( CSocket *mSock, UI08 houseEntry )
{
	UI16 houseID = 0;
	std::string tag, data;
	std::string sect		= std::string("HOUSE ") + strutil::number( houseEntry );
	ScriptSection *House	= FileLookup->FindEntry( sect, house_def );
	if( House == nullptr )
	{
		return;
	}
	for( tag = House->First(); !House->AtEnd(); tag = House->Next() )
	{
		data = House->GrabData();
		if( strutil::toupper( tag ) == "ID" )
		{
			houseID = static_cast<UI16>(std::stoul(strutil::stripTrim( data ), nullptr, 0));
			break;
		}
	}
	if( !houseID )
	{
		Console.error(strutil::format( "Bad house script: #%u\n", houseEntry) );
	}
	else
	{
		mSock->AddID1( houseEntry );
		if( houseID >= 0x4000 )
		{
			mSock->mtarget( static_cast<UI16>(houseID - 0x4000), 576 );
		}
		else
		{
			mSock->target( 0, TARGET_BUILDHOUSE, 576 );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CreateHouseKey( CSocket *mSock, CChar *mChar, CMultiObj *house, UI16 houseID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Create key for tent/house/boat
//o-----------------------------------------------------------------------------------------------o
void CreateHouseKey( CSocket *mSock, CChar *mChar, CMultiObj *house, UI16 houseID )
{
	if( houseID >= 0x4000 )
	{
		std::string scriptName;
		if( (houseID%256) >= 0x70 && (houseID%256) <= 0x73 ) // It's a tent
			scriptName = "tent_key";
		else if( (houseID%256) <= 0x18 )					// It's a boat
			scriptName = "boat_key";
		else
			scriptName = "house_key";

		CItem *key = Items->CreateScriptItem( mSock, mChar, scriptName, 1, OT_ITEM, true );
		if( ValidateObject( key ) )
		{
			key->SetTempVar( CITV_MORE, house->GetSerial() );
			key->SetType( IT_KEY );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CreateHouseItems( CChar *mChar, STRINGLIST houseItems, CItem *house, UI16 houseID, SI16 x, SI16 y, SI08 z )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Create items for house as defined in house.dfn
//o-----------------------------------------------------------------------------------------------o
void CreateHouseItems( CChar *mChar, STRINGLIST houseItems, CItem *house, UI16 houseID, SI16 x, SI16 y, SI08 z )
{
	std::string tag, data, UTag;
	ScriptSection *HouseItem = nullptr;
	CItem *hItem = nullptr;
	STRINGLIST_CITERATOR hiIter;
	for( hiIter = houseItems.begin(); hiIter != houseItems.end(); ++hiIter )//Loop through the HOUSE_ITEMs
	{
		std::string sect = "HOUSE ITEM " + (*hiIter);
		HouseItem = FileLookup->FindEntry( sect, house_def );
		if( HouseItem != nullptr )
		{
			SI16 hiX = x, hiY = y;
			SI08 hiZ = z;
			UI08 hWorld = house->WorldNumber();
			UI16 hInstanceID = house->GetInstanceID();
			hItem = nullptr;	// clear it out
			for( tag = HouseItem->First(); !HouseItem->AtEnd(); tag = HouseItem->Next() )
			{
				UTag = strutil::toupper( tag );
				data = HouseItem->GrabData();
				data = strutil::stripTrim( data );
				if( UTag == "ITEM" )
				{
					hItem = Items->CreateBaseScriptItem( data, mChar->WorldNumber(), 1, hInstanceID );
					if( hItem == nullptr )
					{
						Console << "Error in house creation, item " << data << " could not be made" << myendl;
						break;
					}
					else
					{
						hItem->SetMovable( 2 );//Non-Moveable by default
						hItem->SetLocation( house );
						hItem->SetOwner( mChar );

						if( house->GetObjType() == OT_ITEM )
						{
							// House is not actually a house, but a house addon! Store reference to addon on item
							hItem->SetTempVar( CITV_MORE, house->GetSerial() );
							hItem->SetType( IT_HOUSEADDON );

							// Also add the addon's sub-items to the multi
							CMultiObj *mMulti = house->GetMultiObj();
							if( ValidateObject( mMulti ) )
								mMulti->AddToMulti( hItem );
						}
					}
				}
				else if( UTag == "DECAY" )
				{
					if( ValidateObject( hItem ))
						hItem->SetDecayable( true );
				}
				else if( UTag == "NODECAY" )
				{
					if( ValidateObject( hItem ) )
						hItem->SetDecayable( false );
				}
				else if( UTag == "PACK" )//put the item in the Builder's Backpack
				{
					if( ValidateObject( hItem ) )
					{
						CItem *pack = mChar->GetPackItem();
						hItem->SetCont( pack );
						hItem->PlaceInPack();
					}
				}
				else if( UTag == "MOVEABLE" )
				{
					if( ValidateObject( hItem ) )
						hItem->SetMovable( 1 );
				}
				else if( UTag == "INVISIBLE" )
				{
					if( ValidateObject( hItem ) )
						hItem->SetVisible( VT_PERMHIDDEN );
				}
				else if( UTag == "LOCK" && houseID >= 0x4000 )//lock it with the house key
				{
					if( ValidateObject( hItem ) )
					{
						hItem->SetTempVar( CITV_MORE, house->GetSerial() );
						if( hItem->GetType() == IT_HOUSESIGN )
						{
							// Also store a reference to the sign on the house itself
							house->SetTempVar( CITV_MORE, hItem->GetSerial() );
						}
					}
				}
				else if( UTag == "FRONTDOOR" )
				{
					if( ValidateObject( hItem ) )
					{
						TAGMAPOBJECT frontDoorTag;
						frontDoorTag.m_Destroy		= FALSE;
						frontDoorTag.m_StringValue	= "front";
						frontDoorTag.m_IntValue		= static_cast<SI32>(frontDoorTag.m_StringValue.size());
						frontDoorTag.m_ObjectType	= TAGMAP_TYPE_STRING;
						hItem->SetTag( "DoorType", frontDoorTag );

						// Lock it automatically since house is private when placed initially
						hItem->SetType( IT_LOCKEDDOOR );
					}
				}
				else if( UTag == "INTERIORDOOR" )
				{
					if( ValidateObject( hItem ) )
					{
						TAGMAPOBJECT frontDoorTag;
						frontDoorTag.m_Destroy		= FALSE;
						frontDoorTag.m_StringValue	= "interior";
						frontDoorTag.m_IntValue		= static_cast<SI32>(frontDoorTag.m_StringValue.size());
						frontDoorTag.m_ObjectType	= TAGMAP_TYPE_STRING;
						hItem->SetTag( "DoorType", frontDoorTag );
						}
				}
				else if( UTag == "X" ) //offset + or - from the center of the house:
				{
					hiX += static_cast<SI16>(std::stoi(data, nullptr, 0));
				}
				else if( UTag == "Y" )
				{
					hiY += static_cast<SI16>(std::stoi(data, nullptr, 0));
				}
				else if( UTag == "Z" )
				{
					hiZ += static_cast<SI16>(std::stoi(data, nullptr, 0));
				}
			}
			if( ValidateObject( hItem ) && hItem->GetCont() == nullptr )
				hItem->SetLocation( hiX, hiY, hiZ, hWorld, hInstanceID );
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CheckForValidHouseLocation( CSocket *mSock, CChar *mChar, SI16 x, SI16 y, SI08 z, 
//|							SI16 spaceX, SI16 spaceY, bool isBoat, bool isMulti )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Check whether the chosen location is valid for house placement
//o-----------------------------------------------------------------------------------------------o
bool CheckForValidHouseLocation( CSocket *mSock, CChar *mChar, SI16 x, SI16 y, SI08 z, 
	SI16 spaceX, SI16 spaceY, bool isBoat, bool isMulti )
{
	const UI08 worldNum = mChar->WorldNumber();
	const UI16 instanceID = mChar->GetInstanceID();

	MapData_st& mMap = Map->GetMapData( worldNum );
	if( ( x+spaceX > mMap.xBlock || x-spaceX < 0 || y+spaceY > mMap.yBlock || y-spaceY < 0 ) && !mChar->IsGM() )
	{
		mSock->sysmessage( 577 );
		return false;
	}

	SI16 curX, curY;
	if( !isMulti )
	{
		// House addon
		for( SI16 k = -spaceX; k <= spaceX; ++k )
		{
			curX = x + k;
			for( SI16 l = ( -spaceY ); l <= spaceY; ++l )
			{
				curY = y + l;
				if( mChar->GetCommandLevel() < CL_GM )
				{
					CMultiObj *mMulti = findMulti( curX, curY, z, worldNum, instanceID );
					if( !ValidateObject( mMulti ) || !mMulti->IsOwner( mChar ) )
					{
						mSock->sysmessage( "This object must be placed in your house" );
						return false;
					}

					// Check that house addon won't block a door/staircase when placed
					GenericList< CItem * > *itemList = mMulti->GetItemsInMultiList();
					for( CItem *mItem = itemList->First(); !itemList->Finished(); mItem = itemList->Next() )
					{
						if( !ValidateObject( mItem ) )
							continue;

						if( mItem->GetType() == 12 || mItem->GetType() == 13 )
						{
							SI08 mItemZ = mItem->GetZ();
							if( mItemZ > z && mItemZ - z >= 20 )
							{
								// Ignore doors on floors above
								continue;
							}
							else if( mItemZ < z && z - mItemZ >= 20 )
							{
								// Ignore doors on floors below, too!
								continue;
							}

							if( mItem->isDoorOpen() )
							{
								// Make sure to check against the distance from the door in it's closed state, rather than it's open state!
								TAGMAPOBJECT doorX = mItem->GetTag( "DOOR_X" );
								TAGMAPOBJECT doorY = mItem->GetTag( "DOOR_Y" );
								UI16 origX = mItem->GetX() - doorX.m_IntValue;
								UI16 origY = mItem->GetY() - doorY.m_IntValue;
								if( x - origX < 2 || origX - x < 2 || y - origY < 2 || origY - y < 2 )
								{
									mSock->sysmessage( "You cannot place a house-addon adjacent to a door." );
									return false;
								}
							}

							if( getDist( point3( x, y, z ), mItem->GetLocation() ) < 2 )
							{
								mSock->sysmessage( "You cannot place a house-addon adjacent to a door." );
								return false;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		// Multi
		// Loop through each tile in the multi's affected area, check if it contains any blocking tiles (dynamic, static or map)
		spaceX += 1; // Extra space around left and right side of house
		spaceY += 5; // Extra space in front and back of house

		for( SI16 k = -spaceX; k <= spaceX; ++k )
		{
			curX = x + k;
			for( SI16 l = (-spaceY + 1); l <= spaceY; ++l )
			{
				curY = y + l;
				bool checkOnlyMultis = false;
				bool checkOnlyNonMultis = false;
				bool checkForRoads = true;

				if(( k < ( -spaceX + 1 )) && ( l > -spaceY + 3 && l < spaceY -3 ))
				{
					// If looking at tiles in the immediate border around house
					checkOnlyNonMultis = true;
				}
				else if(( l < ( -spaceY + 4 )) || ( l > ( spaceY - 5 )))
				{
					// If looking at extended area behind and/or in front of house
					// Check only for other multis in the 4 tiles furthest in the back of the house, 
					// and in the 4 tiles furthest in front of the house; we don't care about blocking
					// statics like trees in this area
					checkOnlyMultis = true;
					checkForRoads = false;
				}
				else
				{
					// check within main boundary of house itself
					checkForRoads = true;
				}

				UI08 retVal1 = Map->ValidMultiLocation( curX, curY, z, worldNum, instanceID, !isBoat, checkOnlyMultis, checkOnlyNonMultis, checkForRoads );
				CMultiObj *retVal2 = findMulti( curX, curY, z, worldNum, instanceID );

				if( retVal1 != 1 || retVal2 != nullptr )
				{
					if( isBoat )
						mSock->sysmessage( 7 );
					else
					{
						if( retVal2 )
						{
							mSock->sysmessage( 577 );
						}
						else
						{
							switch( retVal1 )
							{
							case 0: // Blocked by terrain
								mSock->sysmessage( "You cannot build your house on the selected terrain!" );
								break;
							case 2: // Blocked by static item
								mSock->sysmessage( "You cannot build your house there - location is blocked by one or more items!" );
								break;
							case 3: // Blocked by region settings
								mSock->sysmessage( "You cannot build your house there - houses not allowed in this region!" );
								break;
							default:
								mSock->sysmessage( 577 ); // You cannot build your house there!
								break;
							}
						}
					}
					return false;
				}
			}
		}
	}

	return true;
}

CHARLIST findNearbyChars( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, UI16 distance );
ITEMLIST findNearbyItems( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceID, UI16 distance );
UI16 addRandomColor( const std::string& colorlist );
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void BuildHouse( CSocket *mSock, UI08 houseEntry )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when player attempts to place a tent/house/boat
//o-----------------------------------------------------------------------------------------------o
void BuildHouse( CSocket *mSock, UI08 houseEntry )
{
	if( mSock->GetDWord( 11 ) == INVALIDSERIAL )
		return;
	if( !houseEntry )
		return;

	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	const SI16 x = mSock->GetWord( 11 );
	const SI16 y = mSock->GetWord( 13 );
	SI08 z = static_cast<SI08>(mSock->GetByte( 16 ) + Map->TileHeight( mSock->GetWord( 17 ) ));
	//UI32 targetSerial = mSock->GetDWord( 7 );
	mSock->GetDWord( 7 );
	if( mSock->GetDWord( 7 ) != INVALIDSERIAL || getDist( mChar->GetLocation(), point3( x, y, z ) ) >= DIST_BUILDRANGE )
	{
		mSock->sysmessage( 577 );
		return;
	}

	SI16 sx = 0, sy = 0, cx = 0, cy = 0, bx = 0, by = 0;
	SI08 cz = 7;
	STRINGLIST houseItems;
	houseItems.resize( 0 );
	bool itemsWillDecay = false;
	bool isBoat			= false;
	bool isDamageable	= false;
	std::string houseDeed, tag, data, UTag;
	UI16 houseID		= 0;
	// Use default values matching a small house if no tags present
	UI16 maxLockdowns	= 425;
	UI16 maxSecureContainers = 3;
	UI16 maxFriends = 50;
	UI16 maxGuests = 50;
	UI16 maxBans = 50;
	UI16 maxOwners = 8;
	UI16 maxVendors = 10;
	UI16 maxTrashContainers = 1;
	UI16 scriptTrigger = 0;
	UI16 multiColour = 0;
	SI32 weightMax = 400; // Default
	UI16 maxItems = 125; // Default
	std::string customTagName;
	std::string customTagStringValue;
	TAGMAPOBJECT customTag;
	std::map<std::string, TAGMAPOBJECT> customTagMap;

	std::string sect = "HOUSE " + strutil::number( houseEntry );
	ScriptSection *House = FileLookup->FindEntry( sect, house_def );
	if( House == nullptr )
		return;

	std::vector<UI16> scriptIDs;
	for( tag = House->First(); !House->AtEnd(); tag = House->Next() )
	{
		UTag = strutil::toupper( tag );
		data = House->GrabData();
		data = strutil::stripTrim( data );
		
		if( UTag == "ID" )
		{
			houseID = static_cast<UI16>(std::stoul(data, nullptr, 0));
		}
		else if( UTag == "SPACEX" )
		{
			sx = static_cast<SI16>(std::stoi(data, nullptr, 0));
		}
		else if( UTag == "SPACEY" )
		{
			sy = static_cast<SI16>(std::stoi(data, nullptr, 0));
		}
		else if( UTag == "CHARX" )
		{
			cx = static_cast<SI16>(std::stoi(data, nullptr, 0));
		}
		else if( UTag == "CHARY" )
		{
			cy =static_cast<SI16>(std::stoi(data, nullptr, 0));
		}
		else if( UTag == "CHARZ" )
		{
			cz = static_cast<SI08>(std::stoi(data, nullptr, 0));
		}
		else if( UTag == "BANX" )
		{
			bx = static_cast<SI16>(std::stoi(data, nullptr, 0));
		}
		else if( UTag == "BANY" )
		{
			by = static_cast<SI16>(std::stoi(data, nullptr, 0));
		}
		else if( UTag == "Z" ) //to allow offsetting houses in Z to fix multis like Farmer's Cabin
		{
			z = z + static_cast<SI08>(std::stoi(data, nullptr, 0));
		}
		else if( UTag == "ITEMSDECAY" )
		{
			itemsWillDecay = ( static_cast<SI16>(std::stoi(data, nullptr, 0)) == 1 );
		}
		else if( UTag == "HOUSE_ITEM" )
		{
			houseItems.push_back( data );
		}
		else if( UTag == "HOUSE_DEED" )
		{
			houseDeed = data;
		}
		else if( UTag == "BOAT" )
		{
			isBoat = true;	//Boats
		}
		else if( UTag == "DAMAGEABLE" )
		{
			isDamageable = ( static_cast<SI16>( std::stoi(data, nullptr, 0) ) == 1 );
		}
		else if( UTag == "MAXITEMS" )
		{
			maxItems = static_cast<UI16>(std::stoul(data, nullptr, 0));
		}
		else if( UTag == "WEIGHTMAX" )
		{
			weightMax = static_cast<SI32>(std::stoi(data, nullptr, 0));
		}
		else if( UTag == "MAXBANS" )
		{
			maxBans = static_cast<UI16>(std::stoul(data, nullptr, 0));
		}
		else if( UTag == "MAXFRIENDS" )
		{
			maxFriends = static_cast<UI16>(std::stoul(data, nullptr, 0));
		}
		else if( UTag == "MAXGUESTS" )
		{
			maxGuests = static_cast<UI16>(std::stoul(data, nullptr, 0));
		}
		else if( UTag == "MAXLOCKDOWNS" )
		{
			maxLockdowns = static_cast<UI16>(std::stoul(data, nullptr, 0));
		}
		else if( UTag == "MAXTRASHCONTAINERS" )
		{
			maxTrashContainers = static_cast<UI16>(std::stoul(data, nullptr, 0));
		}
		else if( UTag == "MAXOWNERS" )
		{
			maxOwners = static_cast<UI16>(std::stoul(data, nullptr, 0));
		}
		else if( UTag == "MAXSECURECONTAINERS" )
		{
			maxSecureContainers = static_cast<UI16>(std::stoul(data, nullptr, 0));
		}
		else if( UTag == "MAXVENDORS" )
		{
			maxVendors = static_cast<UI16>(std::stoul(data, nullptr, 0));
		}
		else if( UTag == "SCRIPT" )
		{
			scriptTrigger = static_cast<UI16>(std::stoul(data, nullptr, 0));
			if( scriptTrigger > 0 )
			{
				scriptIDs.push_back( scriptTrigger );
			}
		}
		else if( UTag == "COLOUR" || UTag == "COLOR" )
		{
			multiColour = static_cast<UI16>(std::stoul(data, nullptr, 0));
		}
		else if( UTag == "COLOURLIST" || UTag == "COLORLIST" )
		{
			multiColour = addRandomColor( data );
		}
		else if( UTag == "CUSTOMSTRINGTAG" )
		{
			auto ssecs = strutil::sections( data, " " );
			auto count = 0;
			std::string result;
			for( auto &sec : ssecs )
			{
				if( count > 0 )
				{
					if( count == 1 )
					{
						result = strutil::stripTrim( sec );
					}
					else
					{
						result = result + " " + strutil::stripTrim( sec );
					}
				}
				count++;
			}
			customTagName			= strutil::stripTrim( ssecs[0] );
			customTagStringValue	= result;

			if( !customTagName.empty() && !customTagStringValue.empty() )
			{
				customTag.m_Destroy		= FALSE;
				customTag.m_StringValue	= customTagStringValue;
				customTag.m_IntValue	= static_cast<SI32>(customTag.m_StringValue.size());
				customTag.m_ObjectType	= TAGMAP_TYPE_STRING;
				customTagMap.insert( std::pair<std::string, TAGMAPOBJECT>( customTagName, customTag ));
			}
		}
		else if( UTag == "CUSTOMINTTAG" )
		{
			auto ssecs = strutil::sections( data, " " );
			customTagName			= strutil::stripTrim( ssecs[0] );
			customTagStringValue	= strutil::stripTrim( ssecs[1] );
			if( !customTagName.empty() && !customTagStringValue.empty() )
			{
				customTag.m_Destroy		= FALSE;
				customTag.m_IntValue 	= std::stoi(strutil::stripTrim( customTagStringValue ), nullptr, 0);
				customTag.m_ObjectType	= TAGMAP_TYPE_INT;
				customTag.m_StringValue	= "";
				customTagMap.insert( std::pair<std::string, TAGMAPOBJECT>( customTagName, customTag ));
			}
		}
	}

	if( !houseID )
	{
		Console.error( strutil::format("Bad house script # %u!", houseEntry) );
		return;
	}

	const bool isMulti = (houseID >= 0x4000);
	if( !CheckForValidHouseLocation( mSock, mChar, x, y, z, sx, sy, isBoat, isMulti ) )
	{
		return;
	}

	constexpr std::uint16_t maxsize = 1024 ;
	std::string temp ;
	CMultiObj *house = nullptr;
	CItem *fakeHouse = nullptr;
	if( isMulti )
	{
		if( (houseID%256) > 20 )
		{
			temp = strutil::format(maxsize, "%s's house", mChar->GetName().c_str() ); //This will make the little deed item you see when you have showhs on say the person's name, thought it might be helpful for GMs.
		}
		else
		{
			// Assign name of boat deed/model ship to newly created boat
			temp = mChar->GetSpeechItem()->GetTitle();
			if( temp == "" )
			{
				temp = "a ship";
			}
		}
		house = Items->CreateMulti( mChar, temp, houseID, isBoat );
		if( house == nullptr )
		{
			return;
		}

		house->SetLocation( x, y, z );
		house->SetDecayable( itemsWillDecay );
		house->SetDeed( houseDeed ); // crackerjack 8/9/99 - for converting back *into* deeds
		house->SetOwner( mChar );
		// Add all script IDs to multi's list of script IDs
		for( auto scriptID : scriptIDs )
		{
			house->AddScriptTrigger( scriptID );
		}
		house->SetMaxBans( maxBans );
		house->SetMaxFriends( maxFriends );
		house->SetMaxGuests( maxGuests );
		house->SetMaxLockdowns( maxLockdowns );
		house->SetMaxTrashContainers( maxTrashContainers );
		house->SetMaxSecureContainers( maxSecureContainers );
		house->SetMaxOwners( maxOwners );
		house->SetMaxVendors( maxVendors );
		house->SetColour( multiColour );
		house->SetDamageable( isDamageable );

		time_t buildTimestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		house->SetBuildTimestamp( buildTimestamp );

		// Add custom tags to multi
		for (const auto& [key, value] : customTagMap) {
			house->SetTag( key, value );
		}

		// Find corners of new house
		SI16 multiX1 = 0;
		SI16 multiY1 = 0;
		SI16 multiX2 = 0;
		SI16 multiY2 = 0;
		Map->MultiArea( house, multiX1, multiY1, multiX2, multiY2 );

		// Set ban location X and Y offsets.
		if( bx == 0 && by == 0 )
		{
			// If BANX/BANY were not specified in house DFN, use calculated SE corner of house instead
			bx = multiX2;
			by = multiY2;
		}
		else
		{
			// Use offset values specified in BANX/BANY
			bx = x + bx;
			by = y + by;
		}
		house->SetBanX( bx ); 
		house->SetBanY( by );

		// Move characters out of the way
		CHARLIST charList = findNearbyChars( x, y, mChar->WorldNumber(), mChar->GetInstanceID(), std::max( sx, sy ));
		for( CHARLIST_CITERATOR charCtr = charList.begin(); charCtr != charList.end(); ++charCtr )
		{
			CChar *ourChar = (*charCtr);
			if(( ourChar->GetX() >= multiX1 && ourChar->GetX() <= multiX2 ) && 
				( ourChar->GetY() >= multiY1 && ourChar->GetY() <= multiY2 ) && 
				( ourChar->GetZ() >= house->GetZ() - 16 ))
			{
				// Move character (NPC or PC, online or offline) to corner of multi so they don't get stuck in house!
				ourChar->SetLocation( bx, by, house->GetZ() );
			}
		}

		// Move items out of the way
		ITEMLIST itemList = findNearbyItems( x, y, mChar->WorldNumber(), mChar->GetInstanceID(), std::max( sx, sy ));
		for( ITEMLIST_CITERATOR itemCtr = itemList.begin(); itemCtr != itemList.end(); ++itemCtr )
		{
			CItem *ourItem = (*itemCtr);
			if( ourItem->GetVisible() == 0 && ourItem->GetObjType() != OT_MULTI && ourItem->GetObjType() != OT_BOAT )
			{
				if( ( ourItem->GetX() >= multiX1 && ourItem->GetX() <= multiX2 ) &&
					( ourItem->GetY() >= multiY1 && ourItem->GetY() <= multiY2 ) &&
					( ourItem->GetZ() >= house->GetZ() - 16 ) )
				{
					// Move item to corner of multi so they don't get stuck underneath the house!
					ourItem->SetLocation( bx, by, house->GetZ() );
				}
			}
		}
	}
	else
	{
		// House addon
		fakeHouse = Items->CreateItem( mSock, mChar, houseID, 1, 0, OT_ITEM );
		if( fakeHouse == nullptr )
		{
			return;
		}
		fakeHouse->SetLocation( x, y, z );

		CMultiObj *mMulti = findMulti( fakeHouse );
		if( ValidateObject( mMulti ) )
		{
			if( mMulti->GetLockdownCount() < mMulti->GetMaxLockdowns() )
			{
				mMulti->LockDownItem( fakeHouse );
			}
			else
			{
				mSock->sysmessage( "This house has reached the limit on locked down items already!" );
				fakeHouse->Delete();
				return;
			}
		}
		else
		{
			mSock->sysmessage( "House addons can only be placed in houses!" );
			fakeHouse->Delete();
			return;
		}

		fakeHouse->SetOwner( mChar );
		fakeHouse->SetDecayable( false );
		fakeHouse->SetVisible( VT_PERMHIDDEN );
		fakeHouse->SetType( IT_HOUSEADDON );

		// Store name of deed in a custom tag on the addon
		TAGMAPOBJECT deedObject;
		deedObject.m_Destroy		= FALSE;
		deedObject.m_StringValue	= houseDeed;
		deedObject.m_IntValue		= static_cast<SI32>(deedObject.m_StringValue.size());
		deedObject.m_ObjectType	= TAGMAP_TYPE_STRING;
		fakeHouse->SetTag( "deed", deedObject );
	}

	if( isBoat ) //Boats
	{
		CBoatObj *bObj = static_cast<CBoatObj *>(house);
		if( bObj == nullptr || !CreateBoat( mSock, bObj, (houseID%256), houseEntry ) )
		{
			house->Delete();
			return;
		}

		bObj->SetWeightMax( weightMax );
		bObj->SetMaxItems( maxItems );
		bObj->SetDamageable( isDamageable );

		if( !CreateBoat( mSock, bObj, ( houseID % 256 ), houseEntry ) )
		{
			house->Delete();
			return;
		}
	}

	mChar->GetSpeechItem()->Delete();
	mChar->SetSpeechItem( nullptr );

	CreateHouseKey( mSock, mChar, house, houseID );
	if( !houseItems.empty() )
	{
		if( isMulti )
		{
			fakeHouse = house;
		}
		CreateHouseItems( mChar, houseItems, fakeHouse, houseID, x, y, z );
	}

	if( isMulti || isBoat )
	{
		mChar->SetLocation( x + cx, y + cy, z + cz );
	}

	//Teleport pets as well
	GenericList< CChar * > *myPets = mChar->GetPetList();
	for( CChar *pet = myPets->First(); !myPets->Finished(); pet = myPets->Next() )
	{
		if( ValidateObject( pet ) )
		{
			if( !pet->GetMounted() && pet->IsNpc() && objInRange( mChar, pet, DIST_SAMESCREEN ) )
			{
				pet->SetLocation( mChar );
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool KillKeysFunctor( CBaseObject *a, UI32 &b, void *extraData )
//|					void killKeys( SERIAL targSerial )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when turning a house/boat into a deed, or when transferring to new owner
//|
//|	Notes		-	This function is rather CPU-expensive, but AFAIK there is no
//|					better way to find all keys than to do it this way.. :/
//o-----------------------------------------------------------------------------------------------o
bool KillKeysFunctor( CBaseObject *a, UI32 &b, void *extraData )
{
	UI32 *ourData		= (UI32 *)extraData;
	SERIAL targSerial	= ourData[0];
	SERIAL charSerial	= ourData[1];

	if( ValidateObject( a ) && a->CanBeObjType( OT_ITEM ) )
	{
		CItem *i = static_cast< CItem * >(a);
		if( !charSerial )
		{
			if( i->GetType() == IT_KEY && i->GetTempVar( CITV_MORE ) == targSerial )
			{
				// Delete all copies of the key!
				if( ValidateObject( i->GetMultiObj() ) )
				{
					// Remove from multi before deleting
					i->SetMulti( INVALIDSERIAL, false );
				}
				i->Delete();
			}
			else if( i->GetID() >= 0x1769 && i->GetID() <= 0x176b ) // Keyrings
			{
				// Also look in keyrings - first get count of keys in keyring
				SI32 keyCount = i->GetTag( "keys" ).m_IntValue;
				if( keyCount > 0 )
				{
					for( SI32 j = 1; j < keyCount + 1; j++ )
					{
						std::string keyTag = "key" + std::to_string(j) + "more";
						TAGMAPOBJECT keyMore = i->GetTag( keyTag );
						if( strutil::value<SERIAL>(keyMore.m_StringValue) == targSerial )
						{
							// More value of key in keyring matches house serial
							TAGMAPOBJECT localObject;
							localObject.m_Destroy		= FALSE;
							localObject.m_IntValue		= 0;
							localObject.m_ObjectType	= TAGMAP_TYPE_STRING;
							localObject.m_StringValue	= "0";
							i->SetTag( keyTag, localObject );
						}
					}
				}
			}
		}
		else
		{
			if( i->GetType() == IT_KEY && i->GetTempVar( CITV_MORE ) == targSerial )
			{
				CChar *itemPackOwner = FindItemOwner( i );
				if( ValidateObject( itemPackOwner ) )
				{
					// Key held somewhere in character's backpack or bankbox
					if( itemPackOwner->GetSerial() == charSerial )
					{
						i->Delete();
					}
				}
				else if( i->GetMovable() == 3 && ValidateObject( i->GetMultiObj() ))
				{
					// Locked down in character's house.
					if( i->GetMultiObj()->GetOwnerObj()->GetSerial() == charSerial )
					{
						// Remove from multi before deleting! Maybe this should be in Cleanup()?
						i->SetMulti( INVALIDSERIAL, false );
					}
					i->Delete();
				}
				else if( ValidateObject( i->GetCont()) && FindRootContainer( i )->GetOwner() == charSerial )
				{
					// In a container owned by character
					i->Delete();
				}
				else
				{
					// On the ground, not in a container, not locked down
					if( i->GetMovable() != 3 && i->GetCont() == nullptr )
					{
						i->Delete();
					}
				}
			}
			else if( i->GetID() >= 0x1769 && i->GetID() <= 0x176b ) // Keyrings
			{
				CChar *itemPackOwner = FindItemOwner( i );
				// If In backpack or bank, or locked down in character's house, or inside a container in character's house, or
				// on the ground of character's house but not locked down
				if(( ValidateObject( itemPackOwner ) && itemPackOwner->GetSerial() == charSerial ) ||
					( i->GetMovable() == 3 && ValidateObject( i->GetMultiObj() ) && i->GetMultiObj()->GetOwnerObj()->GetSerial() == charSerial ) ||
					( ValidateObject( i->GetCont() ) && FindRootContainer( i )->GetOwner() == charSerial ) ||
					( i->GetMovable() != 3 && i->GetCont() == nullptr ))
				{
					// Also look in keyrings - first get count of keys in keyring
					SI32 keyCount = i->GetTag( "keys" ).m_IntValue;
					if( keyCount > 0 )
					{
						for( SI32 j = 1; j < keyCount + 1; j++ )
						{
							std::string keyTag = "key" + std::to_string(j) + "more";
							TAGMAPOBJECT keyMore = i->GetTag( keyTag );
							if( strutil::value<SERIAL>(keyMore.m_StringValue) == targSerial )
							{
								// More value of key in keyring matches house serial
								TAGMAPOBJECT localObject;
								localObject.m_Destroy		= FALSE;
								localObject.m_IntValue		= 0;
								localObject.m_ObjectType	= TAGMAP_TYPE_STRING;
								localObject.m_StringValue	= "0";
								i->SetTag( keyTag, localObject );
							}
						}
					}
				}
			}
		}
	}
	return true;
}
void killKeys( SERIAL targSerial, SERIAL charSerial = INVALIDSERIAL )
{
	if( charSerial == INVALIDSERIAL )
	{
		// if no character serial is provided, just kill ALL keys that match the target serial
		UI32 toPass[1];
		toPass[0]	= targSerial;
		UI32 b		= 0;
		ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, toPass, &KillKeysFunctor );
	}
	else
	{
		// if character serial is provided, kill only keys belonging to said character, if key matches target serial
		UI32 toPass[2];
		toPass[0]	= targSerial;
		toPass[1]	= charSerial;
		UI32 b		= 0;
		ObjectFactory::getSingleton().IterateOver( OT_ITEM, b, toPass, &KillKeysFunctor );
	}
}

