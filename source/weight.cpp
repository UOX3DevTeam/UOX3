#include "uox3.h"
#include "weight.h"
#include "mapstuff.h"

namespace UOX
{

//o--------------------------------------------------------------------------o
//|	File			-	weight.cpp
//|	Date			-	Pre-1999
//|	Developers		-	Ripper / Zane
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Weight Calculations
//o--------------------------------------------------------------------------o
//| Modifications	-	Version History
//|
//|							1.0			Ripper		Pre - 1999
//|							Initial implimentation
//|
//|							1.1			Zane		23rd February, 2003
//|							New Weight Design
//|							Purpose (Goals):
//|								Basically, the old weight system is entirely screwy, so here's my idea, and how the system will work
//|							
//|								Firstly we need to state some parameters
//|									All weight is dealt with in the hundreths, so a GetWeight() of 1000 is actually 10.00 stones
//|									Wieght limits:
//|										Each Pack needs to have a weight limit, this is currently hardcapped to 400 stones, but should be scriptable and based upon container type
//|										Characters must also have a total weight limit, for now we will hardcap it at 65535 stones.
//|									Script-based Weight Settings:
//|										Every item should have a base weight, this weight will be (100 * actual weight), if there is not one, we will try to grab weight from the MUL files, otherwise assume 0
//|										NPC's should likewise have a base weight, so if someone was so inclined to make advanced AI that pickup and drop
//|											items, they can also overload themselves.
//|									Character Base Weight and Weight Per Strength
//|										All PC's should have a base weight calculated on their bulk (body weight, IE strength)
//|										Also their ability to hold items (take on more weight) should increase with their strength.
//|											Currently this offset is ((Strength * WEIGHT_PER_STR) + 30), where a Weight value greater than this value overloads them
//|											We will leave this value alone, for now, until we can find a better method of basing their total ability to hold items on their str
//|							
//|								Next lets talk about how we handle weight for our in-game objects
//|									Items:
//|										On Item creation, grab its weight from the scripts and store it.
//|										If the item is a normal item, its weight should never change, unless purposely changed by user
//|											A special case would be pileable items, where the weight should be updated when the amount changes
//|										If the item is a pack, then we will increase and decrease its weight as items are added/removed
//|											from the pack. We will accomplish this as much as possible by adding/removing weight in the 
//|											CItem::SetCont() function. This information will be stored as the items weight on worldsave,
//|											and we should never need to recalc weight unless it becomes invalid.
//|										Packs should also display the total weight of themselves and their contents on single-click
//|									Characters:
//|										Character has a base weight, setup by scripts and whatnot, once again this information is stored for the character.
//|										Just like packs, weight will be added and removed as items enter and leave his person (equipped or in a pack he owns).
//|										A Characters total weight will be saved in the worldfile and should never need to be recalced unless it becomes invalid.
//|									Special Cases:
//|										getItem, wearItem, packItem, and dropItem functions, will require special treatment, mostly due to the setting
//|										of an items container to INVALIDSERIAL apon grabbing it, and the sporadic way in which theese functions have been
//|										organized, hopefully the current implementation works, but special care will need to be taken in theese functions.
//|							
//|								How we accomplish this:
//|									calcWeight() will calc and return the total weight of a pack based upon all items inside, their amounts, etc
//|									calcCharWeight() will calc and return the total weight of a Character, using calcWeight() to recurse his packs, and ignoring
//|										items/layers we don't want to add weight for (bank box, etc)
//|									calcAddWeight() Calculates the total weight of adding the new item to the container, returns false if it is over the weight limit
//|									calcSubtractWeight() Calculates the total weight of subtracting the item from the container, returns false if it's over the weight limit
//|									addItemWeight() this function will have two overloads, basically it adds the weight of CItem *i to the total weight of CItem *pack or CChar *mChar
//|									subtractItemWeight() this function will have two overloads, basically it subtracts the weight of CItem *i to the total weight of CItem *pack or CChar *mChar
//|									isOverloaded() returns true if a character can move or false if not (based upon his strength and weight)
//|									checkPackWeight() returns false if a pack is at its maximum weight or not based upon MAX_PACKWEIGHT
//|									checkCharWeight() returns false if a character is at their maximum weight or not based upon MAX_CHARWEIGHT
//|
//o--------------------------------------------------------------------------o

cWeight *Weight = NULL;

const SI32 MAX_PACKWEIGHT = 40000;	// Lets have maximum weight of packs be 400 stones for now
//o--------------------------------------------------------------------------o
//|	Function		-	SI32 cWeight::calcWeight( CItem *pack )
//|	Date			-	2/23/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Calculate the total weight of a pack based upon all items inside,
//|							their amounts, etc. This function should never need to be called
//|							but is available for bruteforce weight updating
//o--------------------------------------------------------------------------o
SI32 cWeight::calcWeight( CItem *pack )
{
	SI32 totalWeight = 0;

	CTile tile;
	for( CItem *i = pack->Contains.First(); !pack->Contains.Finished(); i = pack->Contains.Next() )
	{
		if( !ValidateObject( i ) )
			continue;

		if( i->IsContType() )	// Item is a container
		{
			Map->SeekTile( i->GetID(), &tile );
			totalWeight += (tile.Weight() * 100);	// Add the weight of the container
			totalWeight += calcWeight( i );	// Find and add the weight of the items in the container
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

//o--------------------------------------------------------------------------o
//|	Function		-	SI32 cWeight::calcCharWeight( CChar *mChar )
//|	Date			-	2/23/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Calculate the total weight of a character based upon all items he owns,
//|							This function should never need to be called but is available for
//|							bruteforce weight updating
//o--------------------------------------------------------------------------o
SI32 cWeight::calcCharWeight( CChar *mChar )
{
	SI32 totalWeight = 0;

	CTile tile;
	for( CItem *i = mChar->FirstItem(); !mChar->FinishedItems(); i = mChar->NextItem() )
	{
		if( !ValidateObject( i ) )
			continue;

		switch( i->GetLayer() )
		{
			case IL_NONE:
			case IL_HAIR:		// hair
			case IL_FACIALHAIR:	// beard
			case IL_MOUNT:		// steed
			case IL_BANKBOX:	// bank box
				break;	// no weight for any of these
			case IL_PACKITEM:	// backpack
				Map->SeekTile( i->GetID(), &tile );
				totalWeight += (tile.Weight() * 100);	// Add the weight of the container
				totalWeight += calcWeight( i );	// Find and add the weight of the items in the container
			default:
				totalWeight += i->GetWeight();	// Normal item, just add its weight
				break;
		}
		if( totalWeight >= MAX_WEIGHT )
			return MAX_WEIGHT;
	}
	return totalWeight;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool cWeight::calcAddWeight( CItem *item, SI32 &totalWeight )
//|	Date			-	2/23/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Calculate the total weight of adding an item to a container, returns
//|							false if the item is over the max weight limit
//o--------------------------------------------------------------------------o
bool cWeight::calcAddWeight( CItem *item, SI32 &totalWeight )
{
	SI32 itemWeight = item->GetWeight();
	if( itemWeight == 0 )	// If they have no weight find the weight of the tile
	{
		CTile tile;
		Map->SeekTile( item->GetID(), &tile );
		itemWeight = (tile.Weight() * 100);
	}

	if( item->GetAmount() > 1 )	// Stackable items weight is Amount * Weight
		totalWeight += (item->GetAmount() * itemWeight);
	else	// Else just the items weight
		totalWeight += itemWeight;

	if( totalWeight > MAX_WEIGHT )	// Don't let them go over the weight limit
		return false;
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool cWeight::calcSubtractWeight( CItem *item, SI32 &totalWeight )
//|	Date			-	2/23/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Calculate the total weight of removing an item from a container, returns
//|							false if the item is over the max weight limit or less than 0
//o--------------------------------------------------------------------------o
bool cWeight::calcSubtractWeight( CItem *item, SI32 &totalWeight )
{
	SI32 itemWeight = item->GetWeight();
	if( itemWeight == 0 )	// If they have no weight find the weight of the tile
	{
		CTile tile;
		Map->SeekTile( item->GetID(), &tile );
		itemWeight = (tile.Weight() * 100);
	}

	if( item->GetAmount() > 1 )	// Stackable items weight is Amount * Weight
		totalWeight -= (item->GetAmount() * itemWeight);
	else	// Else just the items weight
		totalWeight -= itemWeight;

	if( totalWeight < 0 || totalWeight > MAX_WEIGHT )	// Don't let them go under 0 or over the weight limit
		return false;
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cWeight::addItemWeight( cBaseObject *getObj, Citem *item )
//|	Date			-	2/23/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Adds an items weight to the total weight of the object
//o--------------------------------------------------------------------------o
void cWeight::addItemWeight( cBaseObject *getObj, CItem *item )
{
	if( getObj->GetObjType() == OT_CHAR )
		addItemWeight( (CChar *)getObj, item );
	else
		addItemWeight( (CItem *)getObj, item );
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cWeight::addItemWeight( CChar *mChar, CItem *item )
//|	Date			-	2/23/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Adds an items weight to the total weight of the character
//o--------------------------------------------------------------------------o
void cWeight::addItemWeight( CChar *mChar, CItem *item )
{
	SI32 totalWeight = mChar->GetWeight();

	if( calcAddWeight( item, totalWeight ) )
		mChar->SetWeight( totalWeight );
	else
		mChar->SetWeight( MAX_WEIGHT );
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cWeight::addItemWeight( CItem *pack, CItem *item )
//|	Date			-	2/23/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Adds an items weight to the total weight of the pack updating
//|							the packs container in the process
//o--------------------------------------------------------------------------o
void cWeight::addItemWeight( CItem *pack, CItem *item )
{
	SI32 totalWeight = pack->GetWeight();
	
	if( calcAddWeight( item, totalWeight ) )
		pack->SetWeight( totalWeight );
	else
		pack->SetWeight( MAX_WEIGHT );

	cBaseObject *pCont = pack->GetCont();
	if( pCont != NULL )
	{
		if( pCont->GetObjType() == OT_ITEM )
		{
			CItem *pPack = (CItem *)pCont;
			if( pPack != NULL )
				addItemWeight( pPack, item );
		}
		else
		{
			CChar *packOwner = (CChar *)pCont;
			if( packOwner != NULL )
				addItemWeight( packOwner, item );
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cWeight::subtractItemWeight( cBaseObject *getObj, Citem *item )
//|	Date			-	2/23/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Subtracts an items weight from the total weight of the object
//o--------------------------------------------------------------------------o
void cWeight::subtractItemWeight( cBaseObject *getObj, CItem *item )
{
	if( getObj->GetObjType() == OT_CHAR )
		subtractItemWeight( (CChar *)getObj, item );
	else
		subtractItemWeight( (CItem *)getObj, item );
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cWeight::addItemWeight( CItem *pack, CItem *item )
//|	Date			-	2/23/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Subtracts an items weight from the total weight of the character
//o--------------------------------------------------------------------------o
void cWeight::subtractItemWeight( CChar *mChar, CItem *item )
{
	SI32 totalWeight = mChar->GetWeight();

	if( calcSubtractWeight( item, totalWeight ) )
		mChar->SetWeight( totalWeight );
	else
		mChar->SetWeight( 0 );
}

//o--------------------------------------------------------------------------o
//|	Function		-	void cWeight::subtractItemWeight( CItem *pack, CItem *item )
//|	Date			-	2/23/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Subtracts an items weight from the total weight of the pack updating
//|							the packs container in the process
//o--------------------------------------------------------------------------o
void cWeight::subtractItemWeight( CItem *pack, CItem *item )
{
	SI32 totalWeight = pack->GetWeight();

	if( calcSubtractWeight( item, totalWeight ) )
		pack->SetWeight( totalWeight );
	else
		pack->SetWeight( 0 );

	cBaseObject *pCont = pack->GetCont();
	if( pCont != NULL )
	{
		if( pCont->GetObjType() == OT_ITEM )
		{
			CItem *pPack = (CItem *)pCont;
			if( pPack != NULL )
				subtractItemWeight( pPack, item );
		}
		else
		{
			CChar *packOwner = (CChar *)pCont;
			if( packOwner != NULL )
				subtractItemWeight( packOwner, item );
		}
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool cWeight::isOverloaded( CChar *mChar )
//|	Date			-	2/23/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Returns true if the character is overloaded based upon his strength
//o--------------------------------------------------------------------------o
bool cWeight::isOverloaded( CChar *mChar )
{
	if( (mChar->GetWeight() /  100) > ((mChar->GetStrength() * cwmWorldState->ServerData()->WeightPerStr()) + 30) )
		return true;
	return false;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool cWeight::checkPackWeight( CItem *pack, CItem *item )
//|	Date			-	2/23/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Returns false if a pack will be overloaded when items weight is added
//|							or true if not based upon MAX_PACKWEIGHT
//o--------------------------------------------------------------------------o
bool cWeight::checkPackWeight( CChar *ourChar, CItem *pack, CItem *item )
{
	if( !ValidateObject( pack ) )
		return true;

	if( pack->GetContSerial() < BASEITEMSERIAL )		// If the pack's container is a character, (it's his root pack), we don't have a container weight limit
		return checkCharWeight( ourChar, (CChar *)pack->GetCont(), item );

	SI32 packWeight = pack->GetWeight();
	SI32 itemWeight = item->GetWeight();
	if( item->GetAmount() > 1 )
		itemWeight *= item->GetAmount();
	if( (itemWeight + packWeight) <= MAX_PACKWEIGHT )
	{	// Calc the weight and compare to MAX_PACKWEIGHT
		if( pack->GetCont() == NULL )	// No container above pack
			return true;
		if( pack->GetContSerial() >= BASEITEMSERIAL )	// pack is in another pack, lets ensure it won't overload that pack
			return checkPackWeight( ourChar, (CItem *)pack->GetCont(), item );
	}
	return false;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool cWeight::checkCharWeight( CChar *ourChar, CChar *mChar, CItem *item )
//|	Date			-	2/23/2003
//|	Developers		-	Zane
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Returns false if a character will be overloaded when items weight is added
//|							or true if not based upon MAX_CHARWEIGHT
//o--------------------------------------------------------------------------o
bool cWeight::checkCharWeight( CChar *ourChar, CChar *mChar, CItem *item )
{
	if( !ValidateObject( mChar ) )
		return true;

	SI32 MAX_CHARWEIGHT = 0;
	if( mChar->IsGM() )
		MAX_CHARWEIGHT = ( MAX_WEIGHT - 1 );	// Weight on GM maxes only at our logical limit
	else
		MAX_CHARWEIGHT = 200000;	// Normal characters have hardcap of 2,000 stones

	SI32 charWeight = mChar->GetWeight();
	SI32 itemWeight = 0;
	if( ourChar != mChar )	// Item weight has already been added to the character if we picked it up
		itemWeight = item->GetWeight();
	if( item->GetAmount() > 1 )
		itemWeight *= item->GetAmount();
	if( (itemWeight + charWeight) <= MAX_CHARWEIGHT )
		return true;

	return false;
}

}
