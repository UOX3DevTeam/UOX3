#include "uox3.h"

//o---------------------------------------------------------------------------o
//|	Function	-	UI16 getAmount( CChar *s, UI16 realID )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of an item on a character
//o---------------------------------------------------------------------------o
UI16 getAmount( CChar *s, UI16 realID )
{
	CItem *p = getPack( s );
	if( p == NULL ) 
		return 0;
	SI32 total = 0;
	
	for( CItem *i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetID() == realID )
				total += i->GetAmount();
			if( i->GetType() == 1 || i->GetType() == 8 ) 
				total += getSubAmount( i, realID );
		}
	}
	return total;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 getSubAmount( CItem *p, UI16 realID )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of an item in a pack
//o---------------------------------------------------------------------------o
SI32 getSubAmount( CItem *p, UI16 realID )
{
	SI32 total = 0;
	for( CItem *i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetID() == realID )
				total += i->GetAmount();
			if( i->GetType() == 1 || i->GetType() == 8 ) 
				total += getSubAmount(i, realID );
		}
	}
	return total;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 deleQuan( CChar *s, UI16 realID, SI32 amount )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Delete specified amount of a certain item on a character
//o---------------------------------------------------------------------------o
SI32 deleQuan( CChar *s, UI16 realID, SI32 amount )
{
	if( s == NULL )
		return 0;
	CItem *p = getPack( s );
	if( p == NULL ) 
		return 0;
	for( CItem *i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetType() == 1 )
				amount -= deleSubQuan( i, realID, amount );
			if( i->GetID() == realID )
			{
				if( i->GetAmount() <= amount )
				{
					amount -= i->GetAmount();
					Items->DeleItem( i );
				}
				else
				{
					decItemAmount( i, amount );
					amount = 0;
				}
			}
			if( amount == 0 ) 
				return 0;
		}
	}
	return amount;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 deleSubQuan( CItem *p, UI16 realID, SI32 amount )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Delete specified amount of a certain item in a pack
//o---------------------------------------------------------------------------o
SI32 deleSubQuan( CItem *p, UI16 realID, SI32 amount )
{
	SI32 k, totaldel = 0;
	for( CItem *i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetType() == 1 )
			{
				k = deleSubQuan( i, realID, amount );
				amount -= k;
				totaldel += k;
			}
			if( i->GetID() == realID )
			{
				if( i->GetAmount() <= amount )
				{
					amount -= i->GetAmount();
					totaldel += i->GetAmount();
					Items->DeleItem( i );
				}
				else
				{
					decItemAmount( i, amount );
					totaldel += amount;
					amount = 0;
				}
			}
			if( amount == 0 ) 
				return totaldel;
		}
	}
	return totaldel;
}

//o---------------------------------------------------------------------------o
//|     Class         :          SI32 getBankCount( CHARACTER p, UI16 itemID, UI16 colour = 0x0000 )
//|     Date          :          October 23rd, 2000
//|     Programmer    :          Abaddon
//o---------------------------------------------------------------------------o
//|     Purpose       :          Searches through the bank to count the amount
//|                              of items with a specific ID and colour
//|                              
//o---------------------------------------------------------------------------o
SI32 getBankCount( CChar *p, UI16 itemID, UI16 colour )
{
	// colour not used yet
	if( p == NULL )
		return 0;
	SI32 goldCount = 0;
	ITEMLIST *ownedItems = p->GetOwnedItems();
	for( ITEM ci = 0; ci < ownedItems->size(); ci++ )
	{
		CItem *oItem = (*ownedItems)[ci];
		if( oItem != NULL )
		{
			if( oItem->GetType() == 1 && oItem->GetMoreX() == 1 )
				goldCount += getSubAmount( oItem, itemID );
		}
	}
	return goldCount;
}

//o---------------------------------------------------------------------------o
//|     Class         :         SI32 deleBankItem( CHARACTER p, UI16 itemID, UI16 colour, SI32 amt )
//|     Date          :         October 23rd, 2000
//|     Programmer    :         Abaddon
//o---------------------------------------------------------------------------o
//|     Purpose       :         Searches through the bank to and deletes a 
//|                             certain amount of a certain item
//|								Returns how many left over
//o---------------------------------------------------------------------------o
SI32 deleBankItem( CChar *p, UI16 itemID, UI16 colour, SI32 amt )
{
	if( p == NULL )
		return amt;
	ITEMLIST *ownedItems = p->GetOwnedItems();
	for( ITEM ci = 0; ci < ownedItems->size() && amt > 0; ci++ )
	{
		CItem *oItem = (*ownedItems)[ci];
		if( oItem != NULL )
		{
			if( oItem->GetType() == 1 && oItem->GetMoreX() == 1 )
			{
				amt -= deleSubItemAmt( oItem, itemID, colour, amt );
				if( amt == 0 )
					return 0;
			}
		}
	}
	return amt;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI16 deleteItemsFromChar( CChar *toFind, UI16 itemID )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Remove all items of specific ID from player
//o---------------------------------------------------------------------------o
SI16 deleteItemsFromChar( CChar *toFind, UI16 itemID )
{
	if( toFind == NULL )
		return 0;
	SI16 deleteCount = 0;
	for( CItem *item = toFind->FirstItem(); !toFind->FinishedItems(); item = toFind->NextItem() )
	{
		if( item != NULL )
		{
			if( item->GetID() == itemID )
			{
				Items->DeleItem( item );
				deleteCount++;
			}
			else if( item->GetLayer() == 0x15 )
			{
				SI16 tempDelete = deleteItemsFromPack( item, itemID );
				deleteCount += tempDelete;
			}
		}
	}
	return deleteCount;
}

//o---------------------------------------------------------------------------o
//|	Function	-	SI16 deleteItemsFromPack( CItem *item, UI16 itemID )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Remove all items of specific ID from pack
//o---------------------------------------------------------------------------o
SI16 deleteItemsFromPack( CItem *item, UI16 itemID )
{
	if( item == NULL )
		return 0;
	SI16 deleteCount = 0;
	for( CItem *itemToFind = item->FirstItemObj(); !item->FinishedItems(); itemToFind = item->NextItemObj() )
	{
		if( itemToFind != NULL )
		{
			if( itemToFind->GetID() == itemID )
			{
				Items->DeleItem( itemToFind );
				deleteCount++;
			}
			else if( itemToFind->GetType() == 1 || itemToFind->GetType() == 8 )	// search any subpacks, specifically pack and locked containers
			{
				SI16 tempDelete = deleteItemsFromPack( itemToFind, itemID );
				deleteCount += tempDelete;
			}
		}
	}
	return deleteCount;
}

//o---------------------------------------------------------------------------o
//|   Function    :  SI32 getItemAmt( CChar *s, UI16 realID, UI16 realColour )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Get the amount of an item of specified color on a character
//o---------------------------------------------------------------------------o
SI32 getItemAmt( CChar *s, UI16 realID, UI16 realColour )
{
	if( s == NULL )
		return 0;
	CItem *p = getPack( s );
	if( p == NULL ) 
		return 0;
	SI32 total = 0;
	for( CItem *i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetID() == realID && i->GetColour() == realColour )
				total += i->GetAmount();
			if( i->GetType() == 1 )	// more types than this around!
				total += getSubItemAmt( i, realID, realColour );
		}
	}
	return total;
}

//o---------------------------------------------------------------------------o
//|   Function    :  SI32 getSubItemAmt( CItem *p, UI16 realID, UI16 realColour )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Get the amount of an item of specified color in a pack
//o---------------------------------------------------------------------------o
SI32 getSubItemAmt( CItem *p, UI16 realID, UI16 realColour )
{
	if( p == NULL )
		return 0;

	CItem *i;
	SI32 total = 0;
	for( i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetID() == realID && i->GetColour() == realColour )
				total += i->GetAmount();
			if( i->GetType() == 1 ) 
				total += getSubItemAmt( i, realID, realColour );
		}
	}
	return total;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void DeleItemAmt( CChar *s, UI16 realID, UI16 realColour, SI32 amount )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Remove a certain amount of an item of specified color on a character
//o---------------------------------------------------------------------------o
SI32 deleItemAmt( CChar *s, UI16 realID, UI16 realColour, SI32 amount )
{
	if( s == NULL )
		return 0;
	CItem *p = getPack( s );
	if( p == NULL ) 
		return 0;

	SI32 total = amount;
	SI32 amtDeleted = 0;
	CItem *i;
	for( i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetType() == 1 )
			{
				SI32 deld = deleSubItemAmt( i, realID, realColour, amount );
				total -= deld;
				amtDeleted += deld;
			}
			if( i->GetID() == realID && i->GetColour() == realColour )
			{
				if( i->GetAmount() <= total )
				{
					total -= i->GetAmount();
					amtDeleted += i->GetAmount();
					Items->DeleItem( i );
				}
				else
				{
					decItemAmount( i, total );
					total = 0;
					amtDeleted = amount;
				}
			}
			if( total == 0 ) 
				return amtDeleted;
		}
	}
	return amtDeleted;
}

//o---------------------------------------------------------------------------o
//|   Function    :  SI32 deleSubItemAmt( CItem *p, UI16 realID, UI16 realColour, SI32 amount )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Remove a certain amount of an item of specified color in a pack
//o---------------------------------------------------------------------------o
SI32 deleSubItemAmt( CItem *p, UI16 realID, UI16 realColour, SI32 amount )
{
	if( p == NULL )
		return 0;
	SI32 totaldel = 0, k;
	SI32 total = amount;
	CItem *i;
	for( i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetType() == 1 )
			{
				k = deleSubItemAmt( i, realID, realColour, total );
				total -= k;
				totaldel += k;
			}
			if( i->GetID() == realID && i->GetID() == realColour )
			{
				if( i->GetAmount() <= total )
				{
					total -= i->GetAmount();
					totaldel += i->GetAmount();
					Items->DeleItem( i );
				}
				else
				{
					decItemAmount( i, total );
					totaldel += total;
					total = 0;
				}
			}
			if( total == 0 ) 
				return totaldel;
		}
	}
	return totaldel;
}

ITEM decItemAmount( ITEM toDelete, SI32 amt )
{
	if( items[toDelete].GetAmount() > 1 )
	{
		items[toDelete].SetAmount( items[toDelete].GetAmount() - amt );
		if( items[toDelete].GetAmount() < 1 )
		{
			Items->DeleItem( &items[toDelete] );
			return INVALIDSERIAL;
		}
		else if( items[toDelete].GetAmount() == 1 )
				RefreshItem( &items[toDelete] );
	}
	else
	{
		Items->DeleItem( &items[toDelete] );
		return INVALIDSERIAL;
	}
	return toDelete;
}

CItem *decItemAmount( CItem *toDelete, SI32 amt )
{
	if( toDelete == NULL )
		return NULL;
	if( toDelete->GetAmount() > 1 )
	{
		toDelete->SetAmount( toDelete->GetAmount() - amt );
		if( toDelete->GetAmount() < 1 )
		{
			Items->DeleItem( toDelete );
			return NULL;
		}
		if( toDelete->GetAmount() == 1 )
			RefreshItem( toDelete );
	}
	else
	{

		Items->DeleItem( toDelete );
		return NULL;
	}
	return toDelete;
}
