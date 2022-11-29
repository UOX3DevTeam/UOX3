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

using namespace std::string_literals;

bool CreateBoat( CSocket *s, CBoatObj *b, UI08 id2, UI08 boattype );

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DoHouseTarget()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Triggered by double clicking a deed-> the deed's moreX is read for the house
//|					section in house.dfn. Extra items can be added using HOUSE ITEM, (this includes
//|					all doors!) and locked "LOCK" Space around the house with SPACEX/Y and CHAR
//|					offset CHARX/Y/Z
//o------------------------------------------------------------------------------------------------o
auto DoHouseTarget( CSocket *mSock, UI16 houseEntry ) -> void
{
	UI16 houseId = 0;
	std::string tag, data;
	auto sect = "HOUSE "s + oldstrutil::number( houseEntry );
	auto House = FileLookup->FindEntry( sect, house_def );
	if( House )
	{
		for( const auto &sec : House->collection() )
		{
			auto tag = sec->tag;
			auto data = sec->data;
			if( oldstrutil::upper( tag ) == "ID" )
			{
				houseId = static_cast<UI16>( std::stoul( oldstrutil::trim( oldstrutil::removeTrailing( data, "//" )), nullptr, 0 ));
				break;
			}
		}
		if( !houseId )
		{
			Console.Error( oldstrutil::format( "Bad house script: #%u\n", houseEntry ));
		}
		else
		{
			mSock->AddId( houseEntry );
			if( houseId >= 0x4000 )
			{
				mSock->mtarget( static_cast<UI16>( houseId - 0x4000 ), 576 ); // Select a location for the building.
			}
			else
			{
				mSock->SendTargetCursor( 0, TARGET_BUILDHOUSE, 0, 9184 ); // Select a location for the house addon.
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CreateHouseKey()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Create key for tent/house/boat
//o------------------------------------------------------------------------------------------------o
void CreateHouseKey( CSocket *mSock, CChar *mChar, CMultiObj *house, UI16 houseId )
{
	if( houseId >= 0x4000 )
	{
		std::string scriptName;
		if(( houseId % 256 ) >= 0x70 && ( houseId % 256 ) <= 0x73 ) // It's a tent
		{
			scriptName = "tent_key";
		}
		else if(( houseId % 256 ) <= 0x18 ) // It's a boat
		{
			scriptName = "boat_key";
		}
		else // It's a house
		{
			scriptName = "house_key";
		}

		CItem *key = Items->CreateScriptItem( mSock, mChar, scriptName, 1, OT_ITEM, true );
		if( ValidateObject( key ))
		{
			key->SetTempVar( CITV_MORE, house->GetSerial() );
			key->SetType( IT_KEY );
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CreateHouseItems()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Create items for house as defined in house.dfn
//o------------------------------------------------------------------------------------------------o
auto CreateHouseItems( CChar *mChar, std::vector<std::string> houseItems, CItem *house, UI16 houseId, SI16 x, SI16 y, SI08 z, UI08 worldNum ) -> void
{
	std::string tag, data, UTag;
	CScriptSection *HouseItem = nullptr;
	CItem *hItem = nullptr;
	
	if( ValidateObject( mChar ))
	{
		worldNum = mChar->WorldNumber();
	}

	for( const auto &entry : houseItems )
	{
		auto sect = "HOUSE ITEM "s + entry;
		HouseItem = FileLookup->FindEntry( sect, house_def );
		if( HouseItem )
		{
			SI16 hiX = x, hiY = y;
			SI08 hiZ = z;
			UI08 hWorld = house->WorldNumber();
			UI16 hInstanceId = house->GetInstanceId();
			hItem = nullptr;	// clear it out
			for( const auto &sec : HouseItem->collection() )
			{
				tag = sec->tag;
				data = sec->data;
				UTag = oldstrutil::upper( tag );
				data = oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ));
				if( UTag == "ITEM" )
				{
					hItem = Items->CreateBaseScriptItem( nullptr, data, worldNum, 1, hInstanceId );
					if( hItem == nullptr )
					{
						Console << "Error in house creation, item " << data << " could not be made" << myendl;
						break;
					}
					else
					{
						hItem->SetMovable( 2 );//Non-Moveable by default
						hItem->SetLocation( house );

						if( ValidateObject( mChar ))
						{
							hItem->SetOwner( mChar );
						}

						if( house->GetObjType() == OT_ITEM )
						{
							// House is not actually a house, but a house addon! Store reference to addon on item
							hItem->SetTempVar( CITV_MORE, house->GetSerial() );

							// Store a custom tag on addon to mark it as a house addon
							TAGMAPOBJECT addonTagObject;
							addonTagObject.m_Destroy		= false;
							addonTagObject.m_IntValue 		= 1;
							addonTagObject.m_ObjectType		= TAGMAP_TYPE_INT;
							addonTagObject.m_StringValue	= "";
							hItem->SetTag( "addon", addonTagObject );

							// Also add the addon's sub-items to the multi
							CMultiObj *mMulti = house->GetMultiObj();
							if( ValidateObject( mMulti ))
							{
								mMulti->AddToMulti( hItem );
							}

							// Do we need to rotate the addon?
							if( hItem->GetTempVar( CITV_MOREZ ) != 0 )
							{
								// An alternate rotation for item exists, stored in CITV_MOREZ
								// Let's assume default rotation is North/South oriented, and check for walls to the left of the addon:
								bool wallFound = ( Map->CheckDynamicFlag( hItem->GetX() - 1, hItem->GetY(), hItem->GetZ(), worldNum, hInstanceId, TF_WALL ));
								if( wallFound )
								{
									// What if it's placed in a corner? Look for north wall too:
									bool northWallFound = ( Map->CheckDynamicFlag( hItem->GetX(), hItem->GetY() - 1, hItem->GetZ(), worldNum, hInstanceId, TF_WALL ));
									if( northWallFound )
									{
										// Randomize between the two directions
										if( RandomNum( 0, 1 ))
										{
											hItem->SetId( hItem->GetTempVar( CITV_MOREZ ));
										}
									}
									else
									{
										// Found a wall west of addon, but none north! Let's rotate addon to face east:
										hItem->SetId( hItem->GetTempVar( CITV_MOREZ ));
									}
								}
							}
						}
					}
				}
				else if( UTag == "DECAY" )
				{
					if( ValidateObject( hItem ))
					{
						hItem->SetDecayable( true );
					}
				}
				else if( UTag == "NODECAY" )
				{
					if( ValidateObject( hItem ))
					{
						hItem->SetDecayable( false );
					}
				}
				else if( UTag == "PACK" ) // put the item in the Builder's Backpack
				{
					if( ValidateObject( mChar ) && ValidateObject( hItem ))
					{
						CItem *pack = mChar->GetPackItem();
						hItem->SetCont( pack );
						hItem->PlaceInPack();
					}
				}
				else if( UTag == "MOVEABLE" )
				{
					if( ValidateObject( hItem ))
					{
						hItem->SetMovable( 1 );
					}
				}
				else if( UTag == "INVISIBLE" )
				{
					if( ValidateObject( hItem ))
					{
						hItem->SetVisible( VT_PERMHIDDEN );
					}
				}
				else if( UTag == "LOCK" && houseId >= 0x4000 ) // lock it with the house key
				{
					if( ValidateObject( hItem ))
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
					if( ValidateObject( hItem ))
					{
						TAGMAPOBJECT frontDoorTag;
						frontDoorTag.m_Destroy		= false;
						frontDoorTag.m_StringValue	= "front";
						frontDoorTag.m_IntValue		= static_cast<SI32>( frontDoorTag.m_StringValue.size() );
						frontDoorTag.m_ObjectType	= TAGMAP_TYPE_STRING;
						hItem->SetTag( "DoorType", frontDoorTag );

						// Lock it automatically since house is private when placed initially
						hItem->SetType( IT_LOCKEDDOOR );
					}
				}
				else if( UTag == "INTERIORDOOR" )
				{
					if( ValidateObject( hItem ))
					{
						TAGMAPOBJECT frontDoorTag;
						frontDoorTag.m_Destroy		= false;
						frontDoorTag.m_StringValue	= "interior";
						frontDoorTag.m_IntValue		= static_cast<SI32>( frontDoorTag.m_StringValue.size() );
						frontDoorTag.m_ObjectType	= TAGMAP_TYPE_STRING;
						hItem->SetTag( "DoorType", frontDoorTag );
					}
				}
				else if( UTag == "X" ) //offset + or - from the center of the house:
				{
					hiX += static_cast<SI16>( std::stoi( data, nullptr, 0 ));
				}
				else if( UTag == "Y" )
				{
					hiY += static_cast<SI16>( std::stoi( data, nullptr, 0 ));
				}
				else if( UTag == "Z" )
				{
					hiZ += static_cast<SI16>( std::stoi( data, nullptr, 0 ));
				}
			}
			if( ValidateObject( hItem ))
			{
				if( hItem->GetCont() == nullptr )
				{
					hItem->SetLocation( hiX, hiY, hiZ, hWorld, hInstanceId );
				}

				if( hItem->GetMulti() == INVALIDSERIAL && hItem->GetType() != IT_HOUSESIGN && hItem->GetType() != IT_DOOR )
				{
					// Add item to multi. It's not a door, nor a sign - could be stairs in a custom foundation!
					CMultiObj *mMulti = FindMulti( house );
					if( ValidateObject( mMulti ))
					{
						hItem->SetMulti( mMulti );
						mMulti->AddToMulti( hItem );
					}
				}
			}
		}
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CheckForValidHouseLocation()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Check whether the chosen location is valid for house placement
//o------------------------------------------------------------------------------------------------o
auto CheckForValidHouseLocation( CSocket *mSock, CChar *mChar, SI16 x, SI16 y, SI08 z,
					  SI16 spaceX, SI16 spaceY, UI08 worldNum, UI16 instanceId, bool isBoat, bool isMulti ) -> bool
{
	auto [mapWidth, mapHeight] = Map->SizeOfMap(worldNum);
	if(( x + spaceX > mapWidth || x - spaceX < 0 || y + spaceY > mapHeight || y - spaceY < 0 ) && !mChar->IsGM() )
	{
		if( mSock )
		{
			mSock->SysMessage( 577 ); // =You cannot build your house there!
		}
		return false;
	}

	SI16 curX, curY;
	if( !isMulti )
	{
		// House addon
		for( SI16 k = 0; k <= spaceX - 1; ++k )
		{
			curX = x + k;
			for( SI16 l = 0; l <= spaceY - 1; ++l )
			{
				curY = y + l;
				if( ValidateObject( mChar ) && mChar->GetCommandLevel() < CL_GM )
				{
					CMultiObj *mMulti = FindMulti( curX, curY, z, worldNum, instanceId );
					if( !ValidateObject( mMulti ) || !mMulti->IsOwner( mChar ))
					{
						if( mSock )
						{
							mSock->SysMessage( 9027 ); // This object must be placed in your house
						}
						return false;
					}

					// Check that house addon won't block a door/staircase when placed
					auto itemList = mMulti->GetItemsInMultiList();
					for( const auto &mItem : itemList->collection() )
					{
						if( !ValidateObject( mItem ))
							continue;

						if( mItem->GetType() == 12 || mItem->GetType() == 13 )
						{
							SI08 mItemZ = mItem->GetZ();
							if(( mItemZ > z ) && (( mItemZ - z ) >= 20 )) // Ignore doors on floors above
							{
								continue;
							}
							else if(( mItemZ < z ) && ( z - mItemZ ) >= 20 ) // Ignore doors on floors below, too!
							{
								continue;
							}

							if( mItem->IsDoorOpen() )
							{
								// Make sure to check against the distance from the door in it's closed state, rather than it's open state!
								auto doorX = mItem->GetTag( "DOOR_X" );
								auto doorY = mItem->GetTag( "DOOR_Y" );
								UI16 origX = mItem->GetX() - doorX.m_IntValue;
								UI16 origY = mItem->GetY() - doorY.m_IntValue;
								
								// Make sure to check absolute values for the distance, since values could be negative
								if( abs( x - origX ) < 2 && abs( y - origY ) < 2 )
								{
									if( mSock )
									{
										mSock->SysMessage( 9028 ); // You cannot place a house-addon adjacent to a door.
									}
									return false;
								}
							}

							if( GetDist( Point3_st( x, y, z ), mItem->GetLocation() ) < 2 )
							{
								if( mSock )
								{
									mSock->SysMessage( 9028 ); // You cannot place a house-addon adjacent to a door.
								}
								return false;
							}

						}

					}

					// Don't allow placing addon if it collides with a blocking tile at same height
					bool locationBlocked = ( Map->CheckDynamicFlag( curX, curY, z, worldNum, instanceId, TF_BLOCKING ));
					if( locationBlocked )
					{
						if( mSock )
						{
							mSock->SysMessage( 9097 ); // You cannot place this house-addon there, location is blocked!
						}
						return false;
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

				UI08 retVal1 = Map->ValidMultiLocation( curX, curY, z, worldNum, instanceId, !isBoat, checkOnlyMultis, checkOnlyNonMultis, checkForRoads );
				auto retVal2 = FindMulti( curX, curY, z, worldNum, instanceId );

				if( retVal1 != 1 || retVal2 != nullptr )
				{
					if( isBoat )
					{
						if( mSock )
						{
							mSock->SysMessage( 7 ); // You cannot place your boat there.
						}
					}
					else
					{
						if( retVal2 )
						{
							if( mSock )
							{
								mSock->SysMessage( 577 ); // You cannot build your house there!
							}
						}
						else
						{
							switch( retVal1 )
							{
							case 0: // Blocked by terrain
								if( mSock )
								{
									mSock->SysMessage( 9029 ); // You cannot build your house on the selected terrain!
								}
								break;
							case 2: // Blocked by static item
								if( mSock )
								{
									mSock->SysMessage( 9030 ); // You cannot build your house there - location is blocked by one or more items!
								}
								break;
							case 3: // Blocked by region settings
								if( mSock )
								{
									mSock->SysMessage( 9031 ); // You cannot build your house there - houses not allowed in this region!
								}
								break;
							default:
								if( mSock )
								{
									mSock->SysMessage( 577 ); // You cannot build your house there!
								}
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

auto FindNearbyChars( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceId, UI16 distance ) -> std::vector<CChar *>;
auto FindNearbyItems( SI16 x, SI16 y, UI08 worldNumber, UI16 instanceId, UI16 distance ) -> std::vector<CItem *>;
UI16 AddRandomColor( const std::string& colorlist );
//o------------------------------------------------------------------------------------------------o
//|	Function	-	BuildHouse()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when player attempts to place a tent/house/boat
//o------------------------------------------------------------------------------------------------o
CMultiObj * BuildHouse( CSocket *mSock, UI16 houseEntry, bool checkLocation = true, SI16 xLoc = -1, SI16 yLoc = -1, SI08 zLoc = 127, UI08 worldNumber = 0, UI16 instanceId = 0 )
{
	if( mSock && mSock->GetDWord( 11 ) == INVALIDSERIAL )
		return nullptr;

	if( !houseEntry )
		return nullptr;

	CChar *mChar = nullptr;
	if( mSock )
	{
		mChar = mSock->CurrcharObj();
		if( !ValidateObject( mChar ))
		{
			return nullptr;
		}
		else
		{
			worldNumber = mChar->WorldNumber();
			instanceId = mChar->GetInstanceId();
		}
	}
	else
	{
		// No socket found, do we have coordinates?
		if( xLoc == -1 || yLoc == -1 || zLoc == 127 )
		{
			return nullptr;
		}
	}

	// Use coordinates if provided in arguments, otherwise rely on values stored on socket
	const SI16 x = xLoc > -1 ? xLoc : mSock->GetWord( 11 );
	const SI16 y = yLoc > -1 ? yLoc : mSock->GetWord( 13 );
	SI08 tileHeight = zLoc != 127 ? 0 : Map->TileHeight( mSock->GetWord( 17 ));
	SI08 z = zLoc != 127 ? zLoc : static_cast<SI08>( mSock->GetByte( 16 ) + tileHeight );

	if( mSock )
	{
		if( mSock->GetDWord( 7 ) != INVALIDSERIAL || GetDist( mChar->GetLocation(), Point3_st( x, y, z )) >= DIST_BUILDRANGE )
		{
			mSock->SysMessage( 577 ); // You cannot build your house there!
			return nullptr;
		}
	}

	SI16 sx = 0, sy = 0, cx = 0, cy = 0, bx = 0, by = 0;
	SI08 cz = 7;
	std::vector<std::string> houseItems;
	houseItems.resize( 0 );
	bool itemsWillDecay = false;
	bool isBoat			= false;
	bool isDamageable	= false;
	std::string houseDeed, tag, data, UTag;
	UI16 houseId		= 0;
	// Use default values matching a small house if no tags present
	UI16 maxLockdowns = 425;
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

	std::string sect = "HOUSE " + oldstrutil::number( houseEntry );
	CScriptSection *House = FileLookup->FindEntry( sect, house_def );
	if( House == nullptr )
		return nullptr; // House entry not found

	std::vector<UI16> scriptIds;
	for( const auto &sec : House->collection() )
	{
		tag = sec->tag;
		data = sec->data;
		UTag = oldstrutil::upper( tag );
		data = oldstrutil::trim( oldstrutil::removeTrailing( data, "//" ));
		
		if( UTag == "ID" )
		{
			houseId = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
		}
		else if( UTag == "SPACEX" )
		{
			sx = static_cast<SI16>( std::stoi( data, nullptr, 0 ));
		}
		else if( UTag == "SPACEY" )
		{
			sy = static_cast<SI16>( std::stoi( data, nullptr, 0 ));
		}
		else if( UTag == "CHARX" )
		{
			cx = static_cast<SI16>( std::stoi( data, nullptr, 0 ));
		}
		else if( UTag == "CHARY" )
		{
			cy =static_cast<SI16>( std::stoi( data, nullptr, 0 ));
		}
		else if( UTag == "CHARZ" )
		{
			cz = static_cast<SI08>( std::stoi( data, nullptr, 0 ));
		}
		else if( UTag == "BANX" )
		{
			bx = static_cast<SI16>( std::stoi( data, nullptr, 0 ));
		}
		else if( UTag == "BANY" )
		{
			by = static_cast<SI16>( std::stoi( data, nullptr, 0 ));
		}
		else if( UTag == "Z" ) //to allow offsetting houses in Z to fix multis like Farmer's Cabin
		{
			z = z + static_cast<SI08>( std::stoi( data, nullptr, 0 ));
		}
		else if( UTag == "ITEMSDECAY" )
		{
			itemsWillDecay = ( static_cast<SI16>( std::stoi( data, nullptr, 0 )) == 1 );
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
			isDamageable = ( static_cast<SI16>( std::stoi( data, nullptr, 0 )) == 1 );
		}
		else if( UTag == "MAXITEMS" )
		{
			maxItems = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
		}
		else if( UTag == "WEIGHTMAX" )
		{
			weightMax = static_cast<SI32>( std::stoi( data, nullptr, 0 ));
		}
		else if( UTag == "MAXBANS" )
		{
			maxBans = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
		}
		else if( UTag == "MAXFRIENDS" )
		{
			maxFriends = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
		}
		else if( UTag == "MAXGUESTS" )
		{
			maxGuests = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
		}
		else if( UTag == "MAXLOCKDOWNS" )
		{
			maxLockdowns = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
		}
		else if( UTag == "MAXTRASHCONTAINERS" )
		{
			maxTrashContainers = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
		}
		else if( UTag == "MAXOWNERS" )
		{
			maxOwners = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
		}
		else if( UTag == "MAXSECURECONTAINERS" )
		{
			maxSecureContainers = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
		}
		else if( UTag == "MAXVENDORS" )
		{
			maxVendors = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
		}
		else if( UTag == "SCRIPT" )
		{
			scriptTrigger = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
			if( scriptTrigger > 0 )
			{
				scriptIds.push_back( scriptTrigger );
			}
		}
		else if( UTag == "COLOUR" || UTag == "COLOR" )
		{
			multiColour = static_cast<UI16>( std::stoul( data, nullptr, 0 ));
		}
		else if( UTag == "COLOURLIST" || UTag == "COLORLIST" )
		{
			multiColour = AddRandomColor( data );
		}
		else if( UTag == "CUSTOMSTRINGTAG" )
		{
			auto ssecs = oldstrutil::sections( data, " " );
			auto count = 0;
			std::string result;
			for( auto &sec : ssecs )
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
			customTagName			= oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" ));
			customTagStringValue	= result;

			if( !customTagName.empty() && !customTagStringValue.empty() )
			{
				customTag.m_Destroy		= false;
				customTag.m_StringValue	= customTagStringValue;
				customTag.m_IntValue	= static_cast<SI32>( customTag.m_StringValue.size() );
				customTag.m_ObjectType	= TAGMAP_TYPE_STRING;
				customTagMap.insert( std::pair<std::string, TAGMAPOBJECT>( customTagName, customTag ));
			}
			else
			{
				Console.Warning( oldstrutil::format( "Invalid data found in CUSTOMSTRINGTAG tag inside House script [%s] - Supported data format: <tagName> <text>", sect.c_str() ));
			}
			break;
		}
		else if( UTag == "CUSTOMINTTAG" )
		{
			auto ssecs = oldstrutil::sections( data, " " );
			auto count = 0;
			std::string result;
			for( auto &sec : ssecs )
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
			customTagName			= oldstrutil::trim( oldstrutil::removeTrailing( ssecs[0], "//" ));
			customTagStringValue	= result;
			if( !customTagName.empty() && !customTagStringValue.empty() )
			{
				customTag.m_Destroy		= false;
				customTag.m_IntValue 	= std::stoi( oldstrutil::trim( oldstrutil::removeTrailing( customTagStringValue, "//" )), nullptr, 0 );
				customTag.m_ObjectType	= TAGMAP_TYPE_INT;
				customTag.m_StringValue	= "";
				customTagMap.insert( std::pair<std::string, TAGMAPOBJECT>( customTagName, customTag ));
				if( count > 1 )
				{
					Console.Warning( oldstrutil::format( "Multiple values detected for CUSTOMINTTAG in House script [%s] - only first value will be used! Supported data format: <tagName> <value>", sect.c_str() ));
				}
			}
			else
			{
				Console.Warning( oldstrutil::format( "Invalid data found in CUSTOMINTTAG tag in House script [%s] - Supported data format: <tagName> <value>", sect.c_str() ));
			}
		}
	}

	if( !houseId )
	{
		Console.Error( oldstrutil::format( "Bad house script # %u!", houseEntry ));
		return nullptr;
	}

	const bool isMulti = ( houseId >= 0x4000 );
	if( !isMulti )
	{
		// Trying to place a house addon, let's subtract the tileHeight of the targeted tile so it doesn't mess with placement rules
		z -= tileHeight;
	}

	if( checkLocation && !CheckForValidHouseLocation( mSock, mChar, x, y, z, sx, sy, worldNumber, instanceId, isBoat, isMulti ))
	{
		return nullptr;
	}

	constexpr UI16 maxSize = 1024;
	std::string temp;
	CMultiObj *house = nullptr;
	CItem *fakeHouse = nullptr;
	if( isMulti )
	{
		if(( houseId % 256 ) > 20 )
		{
			if( ValidateObject( mChar ))
			{
				temp = oldstrutil::format( maxSize, "%s's house", mChar->GetName().c_str() ); // This will make the little deed item you see when you have showhs on say the person's name, thought it might be helpful for GMs.
			}
			else
			{
				temp = "a house";
			}
		}
		else
		{
			// Assign name of boat deed/model ship to newly created boat
			temp = "";
			if( ValidateObject( mChar ))
			{
				temp = mChar->GetSpeechItem()->GetTitle();
			}

			if( temp == "" )
			{
				temp = "a ship";
			}
		}
		house = Items->CreateMulti( temp, houseId, isBoat, worldNumber, instanceId );
		if( house == nullptr )
		{
			return nullptr;
		}

		house->SetLocation( x, y, z );
		house->SetDecayable( itemsWillDecay );
		house->SetDeed( houseDeed ); // crackerjack 8/9/99 - for converting back *into* deeds
		if( ValidateObject( mChar ))
		{
			house->SetOwner( mChar );
		}
		// Add all script IDs to multi's list of script IDs
		for( auto scriptId : scriptIds )
		{
			house->AddScriptTrigger( scriptId );
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

		time_t buildTimestamp = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
		house->SetBuildTimestamp( buildTimestamp );

		// Add custom tags to multi
		for( const auto& [key, value] : customTagMap )
		{
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
		for( auto &ourChar : FindNearbyChars( x, y, worldNumber, instanceId, std::max( sx, sy )))
		{
			if(( ourChar->GetX() >= multiX1 && ourChar->GetX() <= multiX2 ) &&
			   ( ourChar->GetY() >= multiY1 && ourChar->GetY() <= multiY2 ) &&
			   ( ourChar->GetZ() >= house->GetZ() - 16 ))
			{
				// Move character (NPC or PC, online or offline) to corner of multi so they don't get stuck in house!
				ourChar->SetLocation( bx, by, house->GetZ() );
			}
		}

		// Move items out of the way
		for( auto &ourItem : FindNearbyItems( x, y, worldNumber, instanceId, std::max( sx, sy )))
		{
			if( ourItem->GetVisible() == 0 && ourItem->GetObjType() != OT_MULTI && ourItem->GetObjType() != OT_BOAT )
			{
				if(( ourItem->GetX() >= multiX1 && ourItem->GetX() <= multiX2 ) &&
				   ( ourItem->GetY() >= multiY1 && ourItem->GetY() <= multiY2 ) &&
				   ( ourItem->GetZ() >= house->GetZ() - 16 ))
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
		if( ValidateObject( mChar ))
		{
			fakeHouse = Items->CreateItem( mSock, mChar, houseId, 1, 0, OT_ITEM );
		}
		else
		{
			fakeHouse = Items->CreateItem( nullptr, nullptr, houseId, 1, 0, OT_ITEM, false, true, worldNumber, instanceId, xLoc, yLoc, zLoc );
		}
		if( fakeHouse == nullptr )
		{
			return nullptr;
		}
		fakeHouse->SetLocation( x, y, z );

		CMultiObj *mMulti = FindMulti( fakeHouse );
		if( checkLocation && ValidateObject( mMulti ))
		{
			if( mMulti->GetLockdownCount() < mMulti->GetMaxLockdowns() )
			{
				mMulti->LockDownItem( fakeHouse );
			}
			else
			{
				if( mSock )
				{
					mSock->SysMessage( 9032 ); // This house has reached the limit on locked down items already!
				}
				fakeHouse->Delete();
				return nullptr;
			}
		}
		else if( checkLocation )
		{
			if( mSock )
			{
				mSock->SysMessage( 9033 ); // House addons can only be placed in houses!
			}
			fakeHouse->Delete();
			return nullptr;
		}

		if( ValidateObject( mChar ))
		{
			fakeHouse->SetOwner( mChar );
		}
		else
		{
			fakeHouse->SetOwner( nullptr );
		}
		fakeHouse->SetDecayable( false );
		fakeHouse->SetVisible( VT_PERMHIDDEN );

		// Store name of deed in a custom tag on the addon
		TAGMAPOBJECT deedObject;
		deedObject.m_Destroy		= false;
		deedObject.m_StringValue	= houseDeed;
		deedObject.m_IntValue		= static_cast<SI32>( deedObject.m_StringValue.size() );
		deedObject.m_ObjectType	= TAGMAP_TYPE_STRING;
		fakeHouse->SetTag( "deed", deedObject );
	}

	if( isBoat ) // Boats
	{
		CBoatObj *bObj = static_cast<CBoatObj *>( house );
		if( bObj == nullptr || !CreateBoat( mSock, bObj, ( houseId % 256 ), houseEntry ))
		{
			house->Delete();
			return nullptr;
		}

		bObj->SetWeightMax( weightMax );
		bObj->SetMaxItems( maxItems );
		bObj->SetDamageable( isDamageable );
	}

	if( ValidateObject( mChar ))
	{
		mChar->GetSpeechItem()->Delete();
		mChar->SetSpeechItem( nullptr );
		CreateHouseKey( mSock, mChar, house, houseId );
	}

	if( !houseItems.empty() )
	{
		if( isMulti )
		{
			fakeHouse = house;
		}
		CreateHouseItems( mChar, houseItems, fakeHouse, houseId, x, y, z, worldNumber );
	}

	if( ValidateObject( mChar ) && ( isMulti || isBoat ))
	{
		mChar->SetLocation( x + cx, y + cy, z + cz );

		//Teleport pets as well
		auto myPets = mChar->GetPetList();
		for( const auto &pet : myPets->collection() )
		{
			if( ValidateObject( pet ) && pet->GetNpcAiType() != AI_PLAYERVENDOR )
			{
				if( !pet->GetMounted() && pet->IsNpc() && ObjInRange( mChar, pet, DIST_SAMESCREEN ))
				{
					pet->SetLocation( mChar );
				}
			}
		}
	}

	return house;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	BuildBaseMulti()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when a script attempts to create a base multi from raw multi ID
//o------------------------------------------------------------------------------------------------o
CMultiObj * BuildBaseMulti( UI16 multiId, SI16 xLoc = -1, SI16 yLoc = -1, SI08 zLoc = 127, UI08 worldNumber = 0, UI16 instanceId = 0 )
{
	if( !multiId )
		return nullptr;

	// Do we have coordinates?
	if( xLoc == -1 || yLoc == -1 || zLoc == 127 )
		return nullptr;

	SI16 sx = 0, sy = 0;
  [[maybe_unused]] SI16 cx = 0;
  [[maybe_unused]] SI16 cy = 0;
  [[maybe_unused]] SI16 bx = 0;
  [[maybe_unused]] SI16 by = 0;
	[[maybe_unused]] SI08 cz = 7;

	// If multiId provided was lower than 0x4000, add 0x4000 so we get the ID the system expects
	if( multiId < 0x4000 )
	{
		multiId += 0x4000;
	}

	CMultiObj *iMulti = nullptr;
	std::string temp = "a multi";
	iMulti = Items->CreateMulti( temp, multiId, false, worldNumber, instanceId, true );
	if( iMulti == nullptr )
	{
		return nullptr;
	}

	iMulti->SetLocation( xLoc, yLoc, zLoc );
	iMulti->SetDecayable( false );

	// Add all script IDs to multi's list of script IDs
	iMulti->SetDamageable( false );

	time_t buildTimestamp = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
	iMulti->SetBuildTimestamp( buildTimestamp );

	// Find corners of new iMulti
	SI16 multiX1 = 0;
	SI16 multiY1 = 0;
	SI16 multiX2 = 0;
	SI16 multiY2 = 0;
	Map->MultiArea( iMulti, multiX1, multiY1, multiX2, multiY2 );

	// Move characters out of the way
	for( auto &ourChar : FindNearbyChars( xLoc, yLoc, worldNumber, instanceId, std::max( sx, sy )))
	{
		if(( ourChar->GetX() >= multiX1 && ourChar->GetX() <= multiX2 ) &&
			( ourChar->GetY() >= multiY1 && ourChar->GetY() <= multiY2 ) &&
			( ourChar->GetZ() >= iMulti->GetZ() - 16 ))
		{
			// Move character (NPC or PC, online or offline) to corner of multi so they don't get stuck in multi!
			ourChar->SetLocation( multiX2, multiY2, iMulti->GetZ() );
		}
	}

	// Move items out of the way
	for( auto &ourItem : FindNearbyItems( xLoc, yLoc, worldNumber, instanceId, std::max( sx, sy )))
	{
		if( ourItem->GetVisible() == 0 && ourItem->GetObjType() != OT_MULTI && ourItem->GetObjType() != OT_BOAT )
		{
			if(( ourItem->GetX() >= multiX1 && ourItem->GetX() <= multiX2 ) &&
				( ourItem->GetY() >= multiY1 && ourItem->GetY() <= multiY2 ) &&
				( ourItem->GetZ() >= iMulti->GetZ() - 16 ))
			{
				// Move item to corner of multi so they don't get stuck underneath the multi!
				ourItem->SetLocation( multiX2, multiY2, iMulti->GetZ() );
			}
		}
	}

	return iMulti;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	KillKeysFunctor()
//|					KillKeys()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Called when turning a house/boat into a deed, or when transferring to new owner
//|
//|	Notes		-	This function is rather CPU-expensive, but AFAIK there is no
//|					better way to find all keys than to do it this way.. :/
//o------------------------------------------------------------------------------------------------o
bool KillKeysFunctor( CBaseObject *a, [[maybe_unused]] UI32 &b, void *extraData )
{
	UI32 *ourData		= (UI32 *)extraData;
	SERIAL targSerial	= ourData[0];
	SERIAL charSerial	= ourData[1];

	if( ValidateObject( a ) && a->CanBeObjType( OT_ITEM ))
	{
		CItem *i = static_cast<CItem *>( a );
		if( !charSerial )
		{
			if( i->GetType() == IT_KEY && i->GetTempVar( CITV_MORE ) == targSerial )
			{
				// Delete all copies of the key!
				if( ValidateObject( i->GetMultiObj() ))
				{
					// Remove from multi before deleting
					i->SetMulti( INVALIDSERIAL, false );
				}
				i->Delete();
			}
			else if( i->GetId() >= 0x1769 && i->GetId() <= 0x176b ) // Keyrings
			{
				// Also look in keyrings - first get count of keys in keyring
				SI32 keyCount = i->GetTag( "keys" ).m_IntValue;
				if( keyCount > 0 )
				{
					for( SI32 j = 1; j < keyCount + 1; j++ )
					{
						std::string keyTag = "key" + std::to_string( j ) + "more";
						TAGMAPOBJECT keyMore = i->GetTag( keyTag );
						if( oldstrutil::value<SERIAL>( keyMore.m_StringValue ) == targSerial )
						{
							// More value of key in keyring matches house serial
							TAGMAPOBJECT localObject;
							localObject.m_Destroy		= false;
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
				if( ValidateObject( itemPackOwner ))
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
				else if( ValidateObject( i->GetCont() ) && FindRootContainer( i )->GetOwner() == charSerial )
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
			else if( i->GetId() >= 0x1769 && i->GetId() <= 0x176b ) // Keyrings
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
							std::string keyTag = "key" + std::to_string( j ) + "more";
							TAGMAPOBJECT keyMore = i->GetTag( keyTag );
							if( oldstrutil::value<SERIAL>( keyMore.m_StringValue ) == targSerial )
							{
								// More value of key in keyring matches house serial
								TAGMAPOBJECT localObject;
								localObject.m_Destroy		= false;
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
void KillKeys( SERIAL targSerial, SERIAL charSerial = INVALIDSERIAL )
{
	if( charSerial == INVALIDSERIAL )
	{
		// if no character serial is provided, just kill ALL keys that match the target serial
		UI32 toPass[1];
		toPass[0]	= targSerial;
		UI32 b		= 0;
		ObjectFactory::GetSingleton().IterateOver( OT_ITEM, b, toPass, &KillKeysFunctor );
	}
	else
	{
		// if character serial is provided, kill only keys belonging to said character, if key matches target serial
		UI32 toPass[2];
		toPass[0]	= targSerial;
		toPass[1]	= charSerial;
		UI32 b		= 0;
		ObjectFactory::GetSingleton().IterateOver( OT_ITEM, b, toPass, &KillKeysFunctor );
	}
}

