#include "uox3.h"

//o---------------------------------------------------------------------------o
//|	Function	-	UI32 getAmount( CChar *s, UI16 realID )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of an item on a character
//o---------------------------------------------------------------------------o
UI32 GetAmount( CChar *s, UI16 realID )
{
	CItem *p = getPack( s );
	if( p == NULL ) 
		return 0;
	UI32 total = 0;
	
	for( CItem *i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetID() == realID )
				total += i->GetAmount();
			if( i->GetType() == 1 || i->GetType() == 8 ) 
				total += GetSubAmount( i, realID );
		}
	}
	return total;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI32 getSubAmount( CItem *p, UI16 realID )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Get the total amount of an item in a pack
//o---------------------------------------------------------------------------o
UI32 GetSubAmount( CItem *p, UI16 realID )
{
	UI32 total = 0;
	for( CItem *i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetID() == realID )
				total += i->GetAmount();
			if( i->GetType() == 1 || i->GetType() == 8 ) 
				total += GetSubAmount(i, realID );
		}
	}
	return total;
}

//o---------------------------------------------------------------------------o
//|	Function	-	UI32 deleQuan( CChar *s, UI16 realID, UI32 amount )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Delete specified amount of a certain item on a character
//o---------------------------------------------------------------------------o
UI32 DeleteQuantity( CChar *s, UI16 realID, UI32 amount )
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
				amount -= DeleteSubQuantity( i, realID, amount );
			if( i->GetID() == realID )
			{
				if( i->GetAmount() <= amount )
				{
					amount -= i->GetAmount();
					Items->DeleItem( i );
				}
				else
				{
					DecreaseItemAmount( i, amount );
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
//|	Function	-	UI32 deleSubQuan( CItem *p, UI16 realID, UI32 amount )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Delete specified amount of a certain item in a pack
//o---------------------------------------------------------------------------o
UI32 DeleteSubQuantity( CItem *p, UI16 realID, UI32 amount )
{
	UI32 k, totaldel = 0;
	for( CItem *i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetType() == 1 )
			{
				k = DeleteSubQuantity( i, realID, amount );
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
					DecreaseItemAmount( i, amount );
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
//|     Class         :          UI32 getBankCount( CHARACTER p, UI16 itemID, UI16 colour = 0x0000 )
//|     Date          :          October 23rd, 2000
//|     Programmer    :          Abaddon
//o---------------------------------------------------------------------------o
//|     Purpose       :          Searches through the bank to count the amount
//|                              of items with a specific ID and colour
//|                              
//o---------------------------------------------------------------------------o
UI32 GetBankCount( CChar *p, UI16 itemID, UI16 colour )
{
	// colour not used yet
	if( p == NULL )
		return 0;
	UI32 goldCount = 0;
	ITEMLIST *ownedItems = p->GetOwnedItems();
	for( ITEM ci = 0; ci < ownedItems->size(); ci++ )
	{
		CItem *oItem = (*ownedItems)[ci];
		if( oItem != NULL )
		{
			if( oItem->GetType() == 1 && oItem->GetMoreX() == 1 )
				goldCount += GetSubAmount( oItem, itemID );
		}
	}
	return goldCount;
}

//o---------------------------------------------------------------------------o
//|     Class         :         UI32 delBankItem( CHARACTER p, UI16 itemID, UI16 colour, UI32 amt )
//|     Date          :         October 23rd, 2000
//|     Programmer    :         Abaddon
//o---------------------------------------------------------------------------o
//|     Purpose       :         Searches through the bank to and deletes a 
//|                             certain amount of a certain item
//|								Returns how many left over
//o---------------------------------------------------------------------------o
UI32 DeleteBankItem( CChar *p, UI16 itemID, UI16 colour, UI32 amt )
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
				amt -= DeleteSubItemAmount( oItem, itemID, colour, amt );
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
SI16 DeleteItemsFromChar( CChar *toFind, UI16 itemID )
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
				SI16 tempDelete = DeleteItemsFromPack( item, itemID );
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
SI16 DeleteItemsFromPack( CItem *item, UI16 itemID )
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
				SI16 tempDelete = DeleteItemsFromPack( itemToFind, itemID );
				deleteCount += tempDelete;
			}
		}
	}
	return deleteCount;
}

//o---------------------------------------------------------------------------o
//|   Function    :  UI32 getItemAmt( CChar *s, UI16 realID, UI16 realColour )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Get the amount of an item of specified color on a character
//o---------------------------------------------------------------------------o
UI32 GetItemAmount( CChar *s, UI16 realID, UI16 realColour )
{
	if( s == NULL )
		return 0;
	CItem *p = getPack( s );
	if( p == NULL ) 
		return 0;
	UI32 total = 0;
	for( CItem *i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetID() == realID && i->GetColour() == realColour )
				total += i->GetAmount();
			if( i->GetType() == 1 )	// more types than this around!
				total += GetSubItemAmount( i, realID, realColour );
		}
	}
	return total;
}

//o---------------------------------------------------------------------------o
//|   Function    :  UI32 getSubItemAmt( CItem *p, UI16 realID, UI16 realColour )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Get the amount of an item of specified color in a pack
//o---------------------------------------------------------------------------o
UI32 GetSubItemAmount( CItem *p, UI16 realID, UI16 realColour )
{
	if( p == NULL )
		return 0;

	CItem *i;
	UI32 total = 0;
	for( i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetID() == realID && i->GetColour() == realColour )
				total += i->GetAmount();
			if( i->GetType() == 1 ) 
				total += GetSubItemAmount( i, realID, realColour );
		}
	}
	return total;
}

//o---------------------------------------------------------------------------o
//|   Function    :  UI32 DeleItemAmt( CChar *s, UI16 realID, UI16 realColour, UI32 amount )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Remove a certain amount of an item of specified color on 
//|									a character
//|									
//|	Modification	-	09/24/2002	-	EviLDeD - Resource calculations fixed.
//|									
//|	Modification	-	09/25/2002	-	Brakhtus - Weight Fixes
//o---------------------------------------------------------------------------o
UI32 DeleteItemAmount( CChar *s, UI16 realID, UI16 realColour, UI32 amount )
{
	if( s == NULL )
		return 0;
	CItem *p = getPack( s );
	if( p == NULL ) 
		return 0;

	UI32 total = amount;
	UI32 amtDeleted = 0, deld;
	CItem *i;
	for( i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetType() == 1 )	// Is item an pack or contaier ?
			{
				Weight->SubtractItemWeight(i,s);
				deld = DeleteSubItemAmount( i, realID, realColour, amount );
				if(i)
					Weight->AddItemWeight(i,s);
				total -= deld;
				amtDeleted += deld;
			}
			if( i->GetID() == realID && i->GetColour() == realColour )
			{
				if( i->GetAmount() <= total )
				{
					total -= i->GetAmount();
					amtDeleted += i->GetAmount();
					Weight->SubtractItemWeight(i,s);
					Items->DeleItem( i );
				}
				else
				{
					Weight->SubtractItemWeight(i,s);
					DecreaseItemAmount( i, total );
					if(i)
						Weight->AddItemWeight(i,s);
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
//|   Function    :  UI32 deleSubItemAmt( CItem *p, UI16 realID, UI16 realColour, UI32 amount )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Remove a certain amount of an item of specified color in a pack
//o---------------------------------------------------------------------------o
UI32 DeleteSubItemAmount( CItem *p, UI16 realID, UI16 realColour, UI32 amount )
{
	if( p == NULL )
		return 0;
	UI32 totaldel = 0, k;
	SI32 total = amount;
	CItem *i;
	for( i = p->FirstItemObj(); !p->FinishedItems(); i = p->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->GetType() == 1 )
			{
				k = DeleteSubItemAmount( i, realID, realColour, total );
				total -= k;
				totaldel += k;
			}
			if( i->GetID() == realID && i->GetColour() == realColour )
			{
				if( i->GetAmount() <= total )
				{
					total -= i->GetAmount();
					totaldel += i->GetAmount();
					Items->DeleItem( i );
				}
				else
				{
					DecreaseItemAmount( i, total );
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

ITEM DecreaseItemAmount( ITEM toDelete, UI16 amt )
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

CItem *DecreaseItemAmount( CItem *toDelete, UI16 amt )
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
