#include "uox3.h"
#include "weight.h"

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetSubItemAmount( CItem *p, UI16 realID, UI16 realColour )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of an item in a pack
//o-----------------------------------------------------------------------------------------------o
UI32 GetSubItemAmount( CItem *p, UI16 realID, UI16 realColour )
{
	UI32 total = 0;
	CDataList< CItem * > *pCont = p->GetContainsList();
	for( CItem *i = pCont->First(); !pCont->Finished(); i = pCont->Next() )
	{
		if( ValidateObject( i ) )
		{
			if( i->GetType() == IT_CONTAINER || i->GetType() == IT_LOCKEDCONTAINER )
				total += GetSubItemAmount( i, realID, realColour );
			else if( i->GetID() == realID && ( realColour == 0 || i->GetColour() == realColour ) )
				total += i->GetAmount();
		}
	}
	return total;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetItemAmount( CChar *s, UI16 realID, UI16 realColour )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of an item on a character
//o-----------------------------------------------------------------------------------------------o
UI32 GetItemAmount( CChar *s, UI16 realID, UI16 realColour )
{
	CItem *p = s->GetPackItem();
	if( !ValidateObject( p ) )
		return 0;
	return GetSubItemAmount( p, realID, realColour );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 DeleteSubItemAmount( CItem *p, UI32 amount, UI16 realID, UI16 realColour )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove a certain amount of an item of specified color in a pack
//o-----------------------------------------------------------------------------------------------o
UI32 DeleteSubItemAmount( CItem *p, UI32 amount, UI16 realID, UI16 realColour )
{
	if( !ValidateObject( p ) )
		return 0;
	UI32 amtDeleted = 0;
	UI32 total		= amount;
	CDataList< CItem * > *pCont = p->GetContainsList();
	for( CItem *i = pCont->First(); !pCont->Finished(); i = pCont->Next() )
	{
		if( ValidateObject( i ) )
		{
			if( i->GetType() == IT_CONTAINER || i->GetType() == IT_LOCKEDCONTAINER ) // Is item an pack or container?
				amtDeleted += DeleteSubItemAmount( i, total, realID, realColour );
			else if( i->GetID() == realID && i->GetColour() == realColour )
			{
				if( i->GetAmount() <= total )
				{
					amtDeleted += i->GetAmount();
					i->Delete();
				}
				else
				{
					i->IncAmount( -(SI32)total );
					amtDeleted += total;
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
//|	Function	-	UI32 DeleteItemAmount( CChar *s, UI32 amount, UI16 realID, UI16 realColour )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Remove a certain amount of an item of specified color on a character
//|
//|	Changes		-	09/24/2002	-	Resource calculations fixed.
//|
//|	Changes		-	09/25/2002	-	Weight Fixes
//o-----------------------------------------------------------------------------------------------o
UI32 DeleteItemAmount( CChar *s, UI32 amount, UI16 realID, UI16 realColour )
{
	if( !ValidateObject( s ) )
		return 0;
	CItem *p = s->GetPackItem();
	if( !ValidateObject( p ) )
		return 0;

	return DeleteSubItemAmount( p, amount, realID, realColour );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 GetBankCount( CChar *p, UI16 itemID, UI16 colour )
//|	Date		-	October 23rd, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Searches through the bank to count the amount of items with a specific ID and colour
//o-----------------------------------------------------------------------------------------------o
UI32 GetBankCount( CChar *p, UI16 itemID, UI16 colour )
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
				goldCount += GetSubItemAmount( oItem, itemID, colour );
		}
	}
	return goldCount;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI32 DeleteBankItem( CChar *p, UI32 amt, UI16 itemID, UI16 colour )
//|	Date		-	October 23rd, 2000
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Searches through the bank to and deletes a certain amount of a certain item
//|					Returns how many left over
//o-----------------------------------------------------------------------------------------------o
UI32 DeleteBankItem( CChar *p, UI32 amt, UI16 itemID, UI16 colour )
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
				amt -= DeleteSubItemAmount( oItem, amt, itemID, colour );
				if( amt == 0 )
					return 0;
			}
		}
	}
	return amt;
}

