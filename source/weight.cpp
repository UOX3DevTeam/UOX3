#include "uox3.h"
#include "weight.h"
#include "mapstuff.h"

//o-----------------------------------------------------------------------------------------------o
//|	File		-	weight.cpp
//|	Date		-	Pre-1999
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Weight Calculations
//o-----------------------------------------------------------------------------------------------o
//| Changes		-	Version History
//|
//|						1.0			Pre - 1999
//|						Initial implimentation
//|
//|						1.1			23rd February, 2003
//|						New Weight Design
//|						Purpose (Goals):
//|							Basically, the old weight system is entirely screwy, so here's my idea, and how the system will work
//|
//|							Firstly we need to state some parameters
//|								All weight is dealt with in the hundreths, so a GetWeight() of 1000 is actually 10.00 stones
//|								Wieght limits:
//|									Each Pack needs to have a weight limit, this is currently hardcapped to 400 stones, but should be scriptable and based upon container type
//|									Characters must also have a total weight limit, for now we will hardcap it at 65535 stones.
//|								Script-based Weight Settings:
//|									Every item should have a base weight, this weight will be (100 * actual weight), if there is not one, we will try to grab weight from the MUL files, otherwise assume 0
//|									NPC's should likewise have a base weight, so if someone was so inclined to make advanced AI that pickup and drop
//|										items, they can also overload themselves.
//|								Character Base Weight and Weight Per Strength
//|									All PC's should have a base weight calculated on their bulk (body weight, IE strength)
//|									Also their ability to hold items (take on more weight) should increase with their strength.
//|										Currently this offset is ((Strength * WEIGHT_PER_STR) + 30), where a Weight value greater than this value overloads them
//|										We will leave this value alone, for now, until we can find a better method of basing their total ability to hold items on their str
//|
//|							Next lets talk about how we handle weight for our in-game objects
//|								Items:
//|									On Item creation, grab its weight from the scripts and store it.
//|									If the item is a normal item, its weight should never change, unless purposely changed by user
//|										A special case would be pileable items, where the weight should be updated when the amount changes
//|									If the item is a pack, then we will increase and decrease its weight as items are added/removed
//|										from the pack. We will accomplish this as much as possible by adding/removing weight in the
//|										CItem::SetCont() function. This information will be stored as the items weight on worldsave,
//|										and we should never need to recalc weight unless it becomes invalid.
//|									Packs should also display the total weight of themselves and their contents on single-click
//|								Characters:
//|									Character has a base weight, setup by scripts and whatnot, once again this information is stored for the character.
//|									Just like packs, weight will be added and removed as items enter and leave his person (equipped or in a pack he owns).
//|									A Characters total weight will be saved in the worldfile and should never need to be recalced unless it becomes invalid.
//|								Special Cases:
//|									getItem, wearItem, packItem, and dropItem functions, will require special treatment, mostly due to the setting
//|									of an items container to INVALIDSERIAL apon grabbing it, and the sporadic way in which theese functions have been
//|									organized, hopefully the current implementation works, but special care will need to be taken in theese functions.
//|
//|							How we accomplish this:
//|								calcWeight() will calc and return the total weight of a pack based upon all items inside, their amounts, etc
//|								calcCharWeight() will calc and return the total weight of a Character, using calcWeight() to recurse his packs, and ignoring
//|									items/layers we don't want to add weight for (bank box, etc)
//|								calcAddWeight() Calculates the total weight of adding the new item to the container, returns false if it is over the weight limit
//|								calcSubtractWeight() Calculates the total weight of subtracting the item from the container, returns false if it's over the weight limit
//|								addItemWeight() this function will have two overloads, basically it adds the weight of CItem *i to the total weight of CItem *pack or CChar *mChar
//|								subtractItemWeight() this function will have two overloads, basically it subtracts the weight of CItem *i to the total weight of CItem *pack or CChar *mChar
//|								isOverloaded() returns true if a character can move or false if not (based upon his strength and weight)
//|								checkPackWeight() returns false if a pack is at its maximum weight or not based upon MAX_PACKWEIGHT
//|								checkCharWeight() returns false if a character is at their maximum weight or not based upon MAX_CHARWEIGHT
//o-----------------------------------------------------------------------------------------------o

CWeight *Weight = nullptr;

//const SI32 MAX_PACKWEIGHT = 400000;	// Lets have maximum weight of packs be 400 stones for now
//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 calcWeight( CItem *pack )
//|	Date		-	2/23/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate the total weight of a pack based upon all items inside,
//|							their amounts, etc. This function should never need to be called
//|							but is available for bruteforce weight updating
//o-----------------------------------------------------------------------------------------------o
SI32 CWeight::calcWeight( CItem *pack )
{
	SI32 totalWeight = 0;
	SI32 contWeight = 0;

	GenericList< CItem * > *pCont = pack->GetContainsList();
	for( CItem *i = pCont->First(); !pCont->Finished(); i = pCont->Next() )
	{
		if( !ValidateObject( i ) )
			continue;

		if( i->IsContType() )	// Item is a container
		{
			/*			// Code grabs weight of container based on tile-weight to get the "real" weight of the container
			 // This only works if the weight gotten from the tiledata is correct, however. If it's undefined it defaults to 255 stones!
			 // Instead, the code should maybe subtract the weight of all items contained in the container before re-adding them?*/

			contWeight = i->GetBaseWeight(); // Find the base container weight, stored when item was created
			if( contWeight == 0 )	// If they have no weight grab the tiledata weight for the item
			{
				CTile& tile = Map->SeekTile( i->GetID() );
				contWeight = static_cast<SI32>( tile.Weight() * 100);	// Add the weight of the container
			}
			contWeight += calcWeight( i );	// Find and add the weight of the items in the container
			i->SetWeight( contWeight, false );		// Also update the weight property of the container
			totalWeight += contWeight;
			if( totalWeight >= MAX_WEIGHT )
				return MAX_WEIGHT;
		}
		else
		{
			if( !calcAddWeight( i, totalWeight ) )
				return MAX_WEIGHT;
		}
	}
	return totalWeight;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SI32 calcCharWeight( CChar *mChar )
//|	Date		-	2/23/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate the total weight of a character based upon all items he owns,
//|							This function should never need to be called but is available for
//|							bruteforce weight updating
//o-----------------------------------------------------------------------------------------------o
SI32 CWeight::calcCharWeight( CChar *mChar )
{
	SI32 totalWeight = 0;
	SI32 contWeight = 0;

	for( CItem *i = mChar->FirstItem(); !mChar->FinishedItems(); i = mChar->NextItem() )
	{
		if( !ValidateObject( i ) )
			continue;

		if( IsWeightedContainer( i ) )
		{
			if( i->GetLayer() == IL_PACKITEM )
			{
				contWeight = i->GetBaseWeight(); // Find the base container weight, stored when item was created
				if( contWeight == 0 )	// If they have no weight grab the tiledata weight for the item
				{
					CTile& tile = Map->SeekTile( i->GetID() );
					contWeight = static_cast<SI32>( tile.Weight() * 100);	// Add the weight of the container
				}
				contWeight += calcWeight( i );	// Find and add the weight of the items in the container
				i->SetWeight( contWeight, false );		// Also update the weight property of the container
				totalWeight += contWeight;
			}
			else
				totalWeight += i->GetWeight();	// Normal item, just add its weight
		}
		if( totalWeight >= MAX_WEIGHT )
			return MAX_WEIGHT;
	}
	return totalWeight;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool calcAddWeight( CItem *item, SI32 &totalWeight )
//|	Date		-	2/23/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate the total weight of adding an item to a container, returns
//|							false if the item is over the max weight limit
//o-----------------------------------------------------------------------------------------------o
bool CWeight::calcAddWeight( CItem *item, SI32 &totalWeight )
{
	SI32 itemWeight = item->GetWeight();
	if( itemWeight == 0 )	// If they have no weight find the weight of the tile
	{
		CTile& tile = Map->SeekTile( item->GetID() );
		itemWeight = static_cast<SI32>( tile.Weight() * 100);
	}

	if( item->GetAmount() > 1 )	// Stackable items weight is Amount * Weight
		totalWeight += (item->GetAmount() * itemWeight);
	else	// Else just the items weight
		totalWeight += itemWeight;

	if( totalWeight > MAX_WEIGHT )	// Don't let them go over the weight limit
		return false;
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool calcSubtractWeight( CItem *item, SI32 &totalWeight )
//|	Date		-	2/23/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Calculate the total weight of removing an item from a container, returns
//|							false if the item is over the max weight limit or less than 0
//o-----------------------------------------------------------------------------------------------o
bool CWeight::calcSubtractWeight( CItem *item, SI32 &totalWeight )
{
	SI32 itemWeight = item->GetWeight();
	if( itemWeight == 0 )	// If they have no weight find the weight of the tile
	{
		CTile& tile = Map->SeekTile( item->GetID() );
		itemWeight = static_cast<SI32>( tile.Weight() * 100 );
	}

	if( item->GetAmount() > 1 )	// Stackable items weight is Amount * Weight
		totalWeight -= (item->GetAmount() * itemWeight);
	else	// Else just the items weight
		totalWeight -= itemWeight;

	if( totalWeight < 0 || totalWeight > MAX_WEIGHT )	// Don't let them go under 0 or over the weight limit
		return false;
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void addItemWeight( CBaseObject *getObj, Citem *item )
//|	Date		-	2/23/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds an items weight to the total weight of the object
//o-----------------------------------------------------------------------------------------------o
void CWeight::addItemWeight( CBaseObject *getObj, CItem *item )
{
	if( getObj->GetObjType() == OT_CHAR )
		addItemWeight( static_cast<CChar *>(getObj), item );
	else
		addItemWeight( static_cast<CItem *>(getObj), item );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void addItemWeight( CChar *mChar, CItem *item )
//|	Date		-	2/23/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds an items weight to the total weight of the character
//o-----------------------------------------------------------------------------------------------o
void CWeight::addItemWeight( CChar *mChar, CItem *item )
{
	SI32 totalWeight = mChar->GetWeight();

	if( item->GetCont() != mChar || IsWeightedContainer( item ) )
	{
		if( calcAddWeight( item, totalWeight ) )
			mChar->SetWeight( totalWeight );
		else
			mChar->SetWeight( MAX_WEIGHT );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void addItemWeight( CItem *pack, CItem *item )
//|	Date		-	2/23/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds an items weight to the total weight of the pack updating
//|							the packs container in the process
//o-----------------------------------------------------------------------------------------------o
void CWeight::addItemWeight( CItem *pack, CItem *item )
{
	SI32 totalWeight = pack->GetWeight();

	if( calcAddWeight( item, totalWeight ) )
		pack->SetWeight( totalWeight, false );//why false?
	else
		pack->SetWeight( MAX_WEIGHT, false );

	CBaseObject *pCont = pack->GetCont();
	if( ValidateObject( pCont ) )
	{
		if( pCont->CanBeObjType( OT_ITEM ) )
		{
			CItem *pPack = static_cast<CItem *>(pCont);
			if( ValidateObject( pPack ) )
				addItemWeight( pPack, item );
		}
		else
		{
			CChar *packOwner = static_cast<CChar *>(pCont);
			if( ValidateObject( packOwner ) )
			{
				if( IsWeightedContainer( pack ) )
					addItemWeight( packOwner, item );
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool IsWeightedContainer( CItem *toCheck )
//|	Date		-	3/11/2006
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Helper function to handle most of our layer checks in one place
//o-----------------------------------------------------------------------------------------------o
bool CWeight::IsWeightedContainer( CItem *toCheck )
{
	switch( toCheck->GetLayer() )
	{
		case IL_NONE:		// Trade Window
		case IL_HAIR:		// hair
		case IL_FACIALHAIR:	// beard
		case IL_MOUNT:		// steed
		case IL_BANKBOX:	// bank box
		case IL_BUYCONTAINER: //Contains items that NPC is interested in buying from
		case IL_BOUGHTCONTAINER: //Contains items shopkeepers have bought from players
		case IL_SELLCONTAINER:  //Contains items shopkeepers sell to players
			return false;	// no weight for any of these
		default:
			break;
	}
	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void subtractItemWeight( CBaseObject *getObj, Citem *item )
//|	Date		-	2/23/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Subtracts an items weight from the total weight of the object
//o-----------------------------------------------------------------------------------------------o
void CWeight::subtractItemWeight( CBaseObject *getObj, CItem *item )
{
	if( getObj->GetObjType() == OT_CHAR )
		subtractItemWeight( static_cast<CChar *>(getObj), item );
	else
		subtractItemWeight( static_cast<CItem *>(getObj), item );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void subtractItemWeight( CChar *mChar, CItem *item )
//|	Date		-	2/23/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Subtracts an items weight from the total weight of the character
//o-----------------------------------------------------------------------------------------------o
void CWeight::subtractItemWeight( CChar *mChar, CItem *item )
{
	SI32 totalWeight = mChar->GetWeight();

	if( item->GetCont() != mChar || IsWeightedContainer( item ) )
	{
		if( calcSubtractWeight( item, totalWeight ) )
			mChar->SetWeight( totalWeight );
		else
			mChar->SetWeight( 0 );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void subtractItemWeight( CItem *pack, CItem *item )
//|	Date		-	2/23/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Subtracts an items weight from the total weight of the pack updating
//|							the packs container in the process
//o-----------------------------------------------------------------------------------------------o
void CWeight::subtractItemWeight( CItem *pack, CItem *item )
{
	SI32 totalWeight = pack->GetWeight();

	if( calcSubtractWeight( item, totalWeight ) )
		pack->SetWeight( totalWeight, false );
	else
		pack->SetWeight( 0, false );

	CBaseObject *pCont = pack->GetCont();
	if( ValidateObject( pCont ) )
	{
		if( pCont->CanBeObjType( OT_ITEM ) )
		{
			CItem *pPack = static_cast<CItem *>(pCont);
			if( ValidateObject( pPack ) )
				subtractItemWeight( pPack, item );
		}
		else
		{
			CChar *packOwner = static_cast<CChar *>(pCont);
			if( ValidateObject( packOwner ) )
			{
				if( IsWeightedContainer( pack ) )
					subtractItemWeight( packOwner, item );
			}
		}
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool isOverloaded( CChar *mChar ) const
//|	Date		-	2/23/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns true if the character is overloaded based upon his strength
//o-----------------------------------------------------------------------------------------------o
bool CWeight::isOverloaded( CChar *mChar ) const
{
	if( (mChar->GetWeight() / 100) > ((mChar->GetStrength() * cwmWorldState->ServerData()->WeightPerStr()) + 40) )
		return true;
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool checkPackWeight( CChar *ourChar, CItem *pack, CItem *item )
//|	Date		-	2/23/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns false if a pack will be overloaded when items weight is added
//|							or true if not based upon MAX_PACKWEIGHT
//o-----------------------------------------------------------------------------------------------o
bool CWeight::checkPackWeight( CChar *ourChar, CItem *pack, CItem *item )
{
	if( !ValidateObject( pack ) )
		return true;

	if( pack->GetContSerial() < BASEITEMSERIAL )		// If the pack's container is a character, (it's his root pack), we don't have a container weight limit
		return checkCharWeight( ourChar, static_cast<CChar *>(pack->GetCont()), item );

	const SI32 packWeight = pack->GetWeight();
	SI32 packWeightMax = pack->GetWeightMax();
	SI32 itemWeight = item->GetWeight();
	if( item->GetAmount() > 1 )
		itemWeight *= item->GetAmount();
	if( (itemWeight + packWeight) <= packWeightMax ) // <= MAX_PACKWEIGHT )
	{	// Calc the weight and compare to packWeightMax //MAX_PACKWEIGHT
		if( pack->GetCont() == nullptr )	// No container above pack
			return true;
		if( pack->GetContSerial() >= BASEITEMSERIAL )	// pack is in another pack, lets ensure it won't overload that pack
			return checkPackWeight( ourChar, static_cast<CItem *>(pack->GetCont()), item );
	}
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool checkCharWeight( CChar *ourChar, CChar *mChar, CItem *item )
//|	Date		-	2/23/2003
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Returns false if a character will be overloaded when items weight is added
//|							or true if not based upon MAX_CHARWEIGHT
//o-----------------------------------------------------------------------------------------------o
bool CWeight::checkCharWeight( CChar *ourChar, CChar *mChar, CItem *item )
{
	if( !ValidateObject( mChar ) )
		return true;

	SI32 MAX_CHARWEIGHT = 0;
	if( mChar->IsGM() )
		MAX_CHARWEIGHT = ( MAX_WEIGHT - 1 );	// Weight on GM maxes only at our logical limit
	else
		MAX_CHARWEIGHT = 200000;	// Normal characters have hardcap of 2,000 stones

	const SI32 charWeight = mChar->GetWeight();
	SI32 itemWeight = 0;
	if( ourChar != mChar )	// Item weight has already been added to the character if we picked it up
		itemWeight = item->GetWeight();
	if( item->GetAmount() > 1 )
		itemWeight *= item->GetAmount();
	if( (itemWeight + charWeight) <= MAX_CHARWEIGHT )
		return true;

	return false;
}
