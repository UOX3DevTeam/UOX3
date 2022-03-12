#include "uox3.h"
#include "weight.h"

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetSubTotalItemCount( CItem *objCont )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of items in a container
//o-----------------------------------------------------------------------------------------------o
UI32 GetSubTotalItemCount( CItem *objCont )
{
	UI32 total = 0;
	GenericList< CItem * > *pCont = objCont->GetContainsList();
	for( CItem *i = pCont->First(); !pCont->Finished(); i = pCont->Next() )
	{
		if( ValidateObject( i ) )
		{
			if( i->GetType() == IT_CONTAINER || i->GetType() == IT_LOCKEDCONTAINER )
			{
				total += 1; // Also count the container
				total += GetSubTotalItemCount( i );
			}
			else
			{
				if( i->GetLayer() == IL_FACIALHAIR || i->GetLayer() == IL_HAIR )
					continue;

				total += 1;
			}
		}
	}
	return total;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetTotalItemCount( CItem *objCont )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of items in a container
//o-----------------------------------------------------------------------------------------------o
UI32 GetTotalItemCount( CItem *objCont )
{
	if( !ValidateObject( objCont ) || ( objCont->GetType() != IT_CONTAINER && objCont->GetType() != IT_LOCKEDCONTAINER ))
		return 0;
	return GetSubTotalItemCount( objCont );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetSubItemAmount( CItem *p, UI16 realID, UI16 realColour, UI32 realMoreVal, bool colorCheck = false )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of an item in a pack
//o-----------------------------------------------------------------------------------------------o
UI32 GetSubItemAmount( CItem *p, UI16 realID, UI16 realColour, UI32 realMoreVal, bool colorCheck = false )
{
	UI32 total = 0;
	GenericList< CItem * > *pCont = p->GetContainsList();
	for( CItem *i = pCont->First(); !pCont->Finished(); i = pCont->Next() )
	{
		if( ValidateObject( i ) )
		{
			if( i->GetID() != realID && ( i->GetType() == IT_CONTAINER || i->GetType() == IT_LOCKEDCONTAINER ))
				total += GetSubItemAmount( i, realID, realColour, realMoreVal );
			else if( i->GetID() == realID && ( !colorCheck || ( colorCheck && i->GetColour() == realColour )))
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetItemAmount( CChar *s, UI16 realID, UI16 realColour, UI32 realMoreVal, bool colorCheck )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of an item on a character
//o-----------------------------------------------------------------------------------------------o
UI32 GetItemAmount( CChar *s, UI16 realID, UI16 realColour, UI32 realMoreVal, bool colorCheck )
{
	CItem *p = s->GetPackItem();
	if( !ValidateObject( p ) )
		return 0;
	return GetSubItemAmount( p, realID, realColour, realMoreVal, colorCheck );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 DeleteSubItemAmount( CItem *p, UI32 amount, UI16 realID, UI16 realColour, UI32 realMoreVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove a certain amount of an item of specified color in a pack
//o-----------------------------------------------------------------------------------------------o
UI32 DeleteSubItemAmount( CItem *p, UI32 amount, UI16 realID, UI16 realColour, UI32 realMoreVal )
{
	if( !ValidateObject( p ) )
		return 0;
	UI32 amtDeleted = 0;
	UI32 total		= amount;
	GenericList< CItem * > *pCont = p->GetContainsList();
	for( CItem *i = pCont->First(); !pCont->Finished(); i = pCont->Next() )
	{
		if( ValidateObject( i ) )
		{
			if( i->GetID() != realID && ( i->GetType() == IT_CONTAINER || i->GetType() == IT_LOCKEDCONTAINER )) // Is item an pack or container?
				amtDeleted += DeleteSubItemAmount( i, total, realID, realColour );
			else if( i->GetID() == realID && i->GetColour() == realColour && i->GetTempVar( CITV_MORE ) == realMoreVal )
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
						i->IncAmount( -(static_cast<SI32>(total) ));
						amtDeleted += total;
					}
				}
			}
			if( amtDeleted >= amount )
				break;
			else
				total = static_cast<UI32>( amount - amtDeleted );
		}
	}
	return amtDeleted;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 DeleteItemAmount( CChar *s, UI32 amount, UI16 realID, UI16 realColour, UI32 realMoreVal )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove a certain amount of an item of specified color on a character
//|
//|	Changes		-	09/24/2002	-	Resource calculations fixed.
//|
//|	Changes		-	09/25/2002	-	Weight Fixes
//o-----------------------------------------------------------------------------------------------o
UI32 DeleteItemAmount( CChar *s, UI32 amount, UI16 realID, UI16 realColour, UI32 realMoreVal )
{
	if( !ValidateObject( s ) )
		return 0;
	CItem *p = s->GetPackItem();
	if( !ValidateObject( p ) )
		return 0;

	return DeleteSubItemAmount( p, amount, realID, realColour, realMoreVal );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetBankCount( CChar *p, UI16 itemID, UI16 colour, UI32 moreVal )
//|	Date		-	October 23rd, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Searches through the bank to count the amount of items with a specific ID and colour
//o-----------------------------------------------------------------------------------------------o
UI32 GetBankCount( CChar *p, UI16 itemID, UI16 colour, UI32 moreVal )
{
	if( !ValidateObject( p ) )
		return 0;
	UI32 goldCount = 0;
	ITEMLIST *ownedItems = p->GetOwnedItems();
	for( ITEMLIST_CITERATOR I = ownedItems->begin(); I != ownedItems->end(); ++I )
	{
		CItem *oItem = (*I);
		if( ValidateObject( oItem ) || I == ownedItems->end() )
		{
			if( oItem->GetType() == IT_CONTAINER && oItem->GetTempVar( CITV_MOREX ) == 1 )
				goldCount += GetSubItemAmount( oItem, itemID, colour, moreVal, false );
		}
	}
	return goldCount;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 DeleteBankItem( CChar *p, UI32 amt, UI16 itemID, UI16 colour, UI32 moreVal )
//|	Date		-	October 23rd, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Searches through the bank to and deletes a certain amount of a certain item
//|					Returns how many left over
//o-----------------------------------------------------------------------------------------------o
UI32 DeleteBankItem( CChar *p, UI32 amt, UI16 itemID, UI16 colour, UI32 moreVal )
{
	if( !ValidateObject( p ) )
		return amt;
	ITEMLIST *ownedItems = p->GetOwnedItems();
	for( ITEMLIST_CITERATOR I = ownedItems->begin(); I != ownedItems->end() && amt > 0; ++I )
	{
		CItem *oItem = (*I);
		if( ValidateObject( oItem ) || I == ownedItems->end() )
		{
			if( oItem->GetType() == IT_CONTAINER && oItem->GetTempVar( CITV_MOREX ) == 1 )
			{
				amt -= DeleteSubItemAmount( oItem, amt, itemID, colour, moreVal );
				if( amt == 0 )
					return 0;
			}
		}
	}
	return amt;
}

