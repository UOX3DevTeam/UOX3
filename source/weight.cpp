#include "uox3.h"

const UI08 WEIGHT_PER_STR = 4;
//o---------------------------------------------------------------------------o
//|   Function    :  void cWeight::calcWeight( CChar *mChar )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Calculate the total weight of a player based on all items
//|					 in his pack, on his paperdoll or in his hands
//o---------------------------------------------------------------------------o
void cWeight::calcWeight( CChar *mChar )
{

	R64 totalweight = 0.0;
	for( CItem *i = mChar->FirstItem(); !mChar->FinishedItems(); i = mChar->NextItem() )
	{
		if( i != NULL )
		{
			switch( i->GetLayer() )
			{
			case 0x0B:	// hair
			case 0x10:	// beard
			case 0x19:	// bank box
			case 0x1D:	// steed
				break;	// no weight for any of these
			case 0x15:	// backpack
				totalweight += RecursePacks( i );
			default:
				totalweight += ( calcItemWeight( i ) / 100.0 );
				break;
			}
		}
	}
	if( totalweight > 32000 )
		totalweight = 32000;
	if( totalweight < 0 )
		totalweight = 0;

	mChar->SetWeight( (SI16)totalweight );
}

//o---------------------------------------------------------------------------o
//|   Function    :  R64 cWeight::RecursePacks( CItem *pack )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Checks all backpacks on a character for their weight
//o---------------------------------------------------------------------------o
R64 cWeight::RecursePacks( CItem *pack )
{
	if( pack == NULL ) 
		return 0.0;

	R64 totalweight = 0.0;

	int itemsweight = 0;
	for( CItem *i = pack->FirstItemObj(); !pack->FinishedItems(); i = pack->NextItemObj() )
	{
		if( i != NULL )
		{
			itemsweight = calcItemWeight( i );
			if( i->IsContType() )
			{
				totalweight += ((R64)((R64)(itemsweight))/(R64)(100.00));
				totalweight += RecursePacks( i ); //find the item's weight within this container
			}
			else if( i->GetID() == 0x0EED )
				totalweight += (R64)((R64)(i->GetAmount())*(R64)(GOLD_WEIGHT));
			else 
				totalweight += (R64)(((R64)(itemsweight)*(R64)(i->GetAmount()))/100.00);
		}
	}
	return totalweight;
}

//o---------------------------------------------------------------------------o
//|   Function    :  SI32 cWeight::calcItemWeight( CItem *item )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Search for and return the weight of an item
//o---------------------------------------------------------------------------o
SI32 cWeight::calcItemWeight( CItem *item )
{
	if( item == NULL )
		return 0;

	SI32 itemweight = 0;
	if( item->GetWeight() <= 0 ) //weight is defined in scripts for this item
	{
		CTile tile;
		Map->SeekTile( item->GetID(), &tile );
		if( tile.Weight() == 0 ) // can't find weight
		{
			if( item->GetType() != 14 )	
				itemweight = 2; // not food weighs .02 stone
			else
				itemweight = 100; //food weighs 1 stone
		}
		else //found the weight from the tile, set it for next time
		{			
			itemweight = tile.Weight() * 100;
			item->SetWeight( itemweight ); // set weight so next time don't have to search
		}
	}
	else 
		itemweight = item->GetWeight();
	return itemweight;
}

//o---------------------------------------------------------------------------o
//|   Function    :  R64 cWeight::calcPackWeight( CItem *pack )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Calculate the weight of a single backpack based on its
//|					 contents
//o---------------------------------------------------------------------------o
R64 cWeight::calcPackWeight( CItem *pack )
{
	if( pack == NULL ) 
		return 0.0;

	R64 totalweight = 0;
	CTile tile;
	for( CItem *i = pack->FirstItemObj(); !pack->FinishedItems(); i = pack->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->IsContType() )
			{
				Map->SeekTile( i->GetID(), &tile );
				totalweight += tile.Weight();
				totalweight += calcPackWeight( i );
			}
			else 
			{
				Map->SeekTile( i->GetID(), &tile );
				if( tile.Weight() == 0 )	
				{
					if( i->GetID() == 0x0EED )
						totalweight += (R64)((R64)(i->GetAmount())*(R64)(GOLD_WEIGHT));
					else 
						totalweight += (R64)((R64)(i->GetAmount())*(R64)(0.1));
				}
				else 
					totalweight += (i->GetAmount()*tile.Weight() );
			}
		}
	}
	return totalweight;
}
 
//o---------------------------------------------------------------------------o
//|   Function    :  bool cWeight::checkMaxPackWeight( CChar *c )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check maximum capacity of container
//o---------------------------------------------------------------------------o
bool cWeight::checkMaxPackWeight( CChar *c )
{
	CItem *pack = getPack( c );
	if( pack == NULL ) 
		return false;

	R64  totalweight=0;
	CTile tile;
	for( CItem *i = pack->FirstItemObj(); !pack->FinishedItems(); i = pack->NextItemObj() )
	{
		if( i != NULL )
		{
			if( i->IsContType() )
				totalweight += calcPackWeight( i );
			else 
			{
				Map->SeekTile( i->GetID(), &tile);
				totalweight += (i->GetAmount()*tile.Weight() );
			}
		}
	}
	if( pack->GetWeight() >= totalweight ) 
		return false;
	sysmessage( calcSocketObjFromChar( c ), 1385 );
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  bool cWeight::checkWeight( CChar *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Check a players weight (used to tell if overloaded when walking )
//o---------------------------------------------------------------------------o
bool cWeight::checkWeight( CChar *s, bool isTele )
{
	if( ( s->GetWeight() > ( s->GetStrength() * WEIGHT_PER_STR ) + 30 ) )
	{
		if( isTele )
		{
			s->SetMana( s->GetMana() - 30 );
			if( s->GetMana() <= 0 )
				s->SetMana( 0 );
			return false;
		}

		R32 res = (R32)( s->GetWeight() - ( ( s->GetStrength() * WEIGHT_PER_STR ) + 30 ) ) * 2;
	
		s->SetStamina( s->GetStamina() - (int)res );
		if( s->GetStamina() <= 0 )
		{
			s->SetStamina( 0 );
			sysmessage( calcSocketObjFromChar( s ), 1386 );
			return false;
		}
	}
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cWeight::AddItemWeight( CItem *i, CChar *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Add an items weight to players total weight
//o---------------------------------------------------------------------------o
void cWeight::AddItemWeight( CItem *i, CChar *s )
{
	CTile tile;
	Map->SeekTile( i->GetID(), &tile);
	if( i->IsContType() )
	{									    //and all item  weight
		s->SetWeight( s->GetWeight() + tile.Weight() );
		s->SetWeight( s->GetWeight() + (int)(calcPackWeight( i ) ) );
	}
	else
	{
		if( tile.Weight() == 0 )	
		{
			if( i->GetID() == 0x0EED )
				s->SetWeight( s->GetWeight() + (int)(i->GetAmount()*GOLD_WEIGHT) );
			else 
				s->SetWeight( s->GetWeight() + (int)(i->GetAmount()*0.1) );
		}	
		else 
			s->SetWeight( s->GetWeight() + (i->GetAmount()*tile.Weight() ) );
	}
	if( s->GetWeight() > 32000 )
		s->SetWeight( 32000 );
}

//o---------------------------------------------------------------------------o
//|   Function    :  void cWeight::SubtractItemWeight( CItem *i, CChar *s )
//|   Date        :  Unknown
//|   Programmer  :  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     :  Subtract an items weight from players total weight
//o---------------------------------------------------------------------------o
void cWeight::SubtractItemWeight( CItem *i, CChar *s )
{									  
	CTile tile;
	Map->SeekTile( i->GetID(), &tile );
	if( i->IsContType() )
	{									   //and all item  weight
		s->SetWeight( s->GetWeight() - tile.Weight() );
		s->SetWeight( s->GetWeight() - (int)(calcPackWeight( i ) ) );
	}
	else
	{
		if( tile.Weight() == 0 )	
		{
			if( i->GetID() == 0x0EED )
				s->SetWeight( s->GetWeight() - (int)(i->GetAmount()*GOLD_WEIGHT) );
			else 
				s->SetWeight( s->GetWeight() - (int)(i->GetAmount()*0.1) );
		}	
		else 
			s->SetWeight( s->GetWeight() - (i->GetAmount()*tile.Weight() ) );
	}
	if( s->GetWeight() < 0 )
		s->SetWeight( 0 );
}
