#include "uox3.h"
#include "weight.h"

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetSubTotalItemCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of items in a container
//o------------------------------------------------------------------------------------------------o
auto GetSubTotalItemCount( CItem *objCont ) -> UI32
{
	UI32 total = 0;
	auto pCont = objCont->GetContainsList();
	for( const auto &i : pCont->collection() )
	{
		if( ValidateObject( i ))
		{
			if( i->GetType() == IT_CONTAINER || i->GetType() == IT_LOCKEDCONTAINER )
			{
				total += 1; // Also count the container
				total += GetSubTotalItemCount( i );
			}
			else
			{
				if( !( i->GetLayer() == IL_FACIALHAIR || i->GetLayer() == IL_HAIR ))
				{
					total += 1;
				}
			}
		}
	}
	return total;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetTotalItemCount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of items in a container
//o------------------------------------------------------------------------------------------------o
UI32 GetTotalItemCount( CItem *objCont )
{
	if( !ValidateObject( objCont ) || ( objCont->GetType() != IT_CONTAINER && objCont->GetType() != IT_LOCKEDCONTAINER ))
		return 0;

	return GetSubTotalItemCount( objCont );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetSubItemAmount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of an item in a pack
//o------------------------------------------------------------------------------------------------o
auto GetSubItemAmount( CItem *p, UI16 realId, UI16 realColour, UI32 realMoreVal, bool colorCheck = false ) -> UI32
{
	UI32 total = 0;
	auto pCont = p->GetContainsList();
	for( const auto &i : pCont->collection() )
	{
		if( ValidateObject( i ))
		{
			if( i->GetId() != realId && ( i->GetType() == IT_CONTAINER || i->GetType() == IT_LOCKEDCONTAINER ))
			{
				total += GetSubItemAmount( i, realId, realColour, realMoreVal );
			}
			else if( i->GetId() == realId && ( !colorCheck || ( colorCheck && i->GetColour() == realColour )))
			{
				if( i->GetUsesLeft() > 0 )
				{
					total += i->GetUsesLeft();
				}
				else
				{
					total += i->GetAmount();
				}
			}
		}
	}
	return total;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetItemAmount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of an item on a character
//o------------------------------------------------------------------------------------------------o
UI32 GetItemAmount( CChar *s, UI16 realId, UI16 realColour, UI32 realMoreVal, bool colorCheck )
{
	CItem *p = s->GetPackItem();
	if( !ValidateObject( p ))
		return 0;

	return GetSubItemAmount( p, realId, realColour, realMoreVal, colorCheck );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DeleteSubItemAmount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove a certain amount of an item of specified color in a pack
//o------------------------------------------------------------------------------------------------o
auto DeleteSubItemAmount( CItem *p, UI32 amount, UI16 realId, UI16 realColour, UI32 realMoreVal ) -> UI32
{
	if( !ValidateObject( p ))
		return 0;

	UI32 amtDeleted = 0;
	UI32 total		= amount;
	auto pCont = p->GetContainsList();
	auto collection = pCont->collection(); // force a copy, we will be deleting
	for( auto &i : collection )
	{
		if( !ValidateObject( i ))
			continue;

		if( i->GetId() != realId && ( i->GetType() == IT_CONTAINER || i->GetType() == IT_LOCKEDCONTAINER ))
		{
			// Is item an pack or container?
			amtDeleted += DeleteSubItemAmount( i, total, realId, realColour );
		}
		else if( i->GetId() == realId && i->GetColour() == realColour && i->GetTempVar( CITV_MORE ) == realMoreVal )
		{
			UI16 usesLeft = i->GetUsesLeft();
			if( usesLeft > 0 )
			{
				// If item has uses left, but not enough to cover the total resource cost...
				if( usesLeft <= total )
				{
					// ...deplete remaining uses from total and delete item
					amtDeleted += usesLeft;
					i->Delete();
				}
				else
				{
					// Otherwise, reduce amount of uses left on item
					i->SetUsesLeft( usesLeft - total );
				}
			}
			else
			{
				// There are no uses on item, but there might be a stack of the item, or multiple items
				if( i->GetAmount() <= total )
				{
					amtDeleted += i->GetAmount();
					i->Delete();
				}
				else
				{
					i->IncAmount( -( static_cast<SI32>( total )));
					amtDeleted += total;
				}
			}
		}

		if( amtDeleted >= amount )
		{
			break;
		}
		else
		{
			total = static_cast<UI32>( amount - amtDeleted );
		}
	}

	return amtDeleted;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DeleteItemAmount()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove a certain amount of an item of specified color on a character
//|
//|	Changes		-	09/24/2002	-	Resource calculations fixed.
//|
//|	Changes		-	09/25/2002	-	Weight Fixes
//o------------------------------------------------------------------------------------------------o
UI32 DeleteItemAmount( CChar *s, UI32 amount, UI16 realId, UI16 realColour, UI32 realMoreVal )
{
	if( !ValidateObject( s ))
		return 0;

	CItem *p = s->GetPackItem();
	if( !ValidateObject( p ))
		return 0;

	return DeleteSubItemAmount( p, amount, realId, realColour, realMoreVal );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	GetBankCount()
//|	Date		-	October 23rd, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Searches through the bank to count the amount of items with a specific ID and colour
//o------------------------------------------------------------------------------------------------o
UI32 GetBankCount( CChar *p, UI16 itemId, UI16 colour, UI32 moreVal )
{
	if( !ValidateObject( p ))
		return 0;

	UI32 goldCount = 0;
	for( auto &oItem : *p->GetOwnedItems() )
	{
		if( ValidateObject( oItem ))
		{
			if( oItem->GetType() == IT_CONTAINER && oItem->GetTempVar( CITV_MOREX ) == 1 )
			{
				goldCount += GetSubItemAmount( oItem, itemId, colour, moreVal, false );
			}
		}
	}
	return goldCount;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	DeleteBankItem()
//|	Date		-	October 23rd, 2000
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Searches through the bank to and deletes a certain amount of a certain item
//|					Returns how many left over
//o------------------------------------------------------------------------------------------------o
UI32 DeleteBankItem( CChar *p, UI32 amt, UI16 itemId, UI16 colour, UI32 moreVal )
{
	if( !ValidateObject( p ))
		return amt;

	for( auto &oItem : *p->GetOwnedItems() )
	{
		if( ValidateObject( oItem ))
		{
			if( oItem->GetType() == IT_CONTAINER && oItem->GetTempVar( CITV_MOREX ) == 1 )
			{
				amt -= DeleteSubItemAmount( oItem, amt, itemId, colour, moreVal );
				if( amt == 0 )
					return 0;
			}
		}
	}
	return amt;
}

