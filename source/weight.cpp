//------------------------------------------------------------------------
//  weight.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by unknown real name (Moongraven)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//	
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//   
//------------------------------------------------------------------------
/****************************************************
 *	      All weight function are coded here		*										
 *               Done by Moongraven					*
 ****************************************************/

#include "uox3.h"

/**************************************************************************************************************************
void newcalcweight(int);

  Ison 2-20-99 - re-wrote by Tauriel 3/20/99
  
	calcweight will search player's paperdoll and then backpacks for items with item.weight set to a value.  Values are 
	added until total weight of character is determined.  The called character's weight is first set to zero then re-calculated
	during the function.  Since it is a global variable I just modifed .weight instead of returning a value.

  pass the current character number to the function

**************************************************************************************************************************/

void cWeight::NewCalc(int p)
{
	int bp, ci,serhash,serial;
	int i;
	double totalweight=0.0;

	bp=packitem(p);
	serial=chars[p].serial;
	chars[p].weight=0; // vinceb weight

	//totalweight=0.0;

	//get weight for items on players
	serhash=serial%HASHMAX;
	for (i=0;i<contsp[serhash].max;i++)
	{
		ci=contsp[serhash].pointer[i];
		if (ci!=-1)
		if ((items[ci].contserial==serial) && 
				(items[ci].layer!=0x0B) && (items[ci].layer!=0x10) && //no weight for hair/beard
				(items[ci].layer!=0x1D) && (items[ci].layer!=0x19))   //no weight for steed/bank box
		{
			totalweight+=(Weight->ItemWeight(ci)/100.0); //(items[ci].weight/100);
		}
	}

  // Items in players pack
	if (bp!=-1) totalweight+=Weight->RecursePacks(bp); //LB

	chars[p].weight+=(int)totalweight;// /10); // vinceb weight

	return;
}

/**************************************************************************************************************************
void recursepacks(int);

  Ison 2-20-99  - rewrote by Tauriel 3/20/99
  
     recursive function to check all backpacks of a character for weight

  pass the characters backpack number to the function

**************************************************************************************************************************/
double cWeight::RecursePacks(int bp)
{
	int ci,serial,serhash,i,itemsweight;
	double totalweight;
	
	totalweight=0.0;

	if (bp<=-1) return 0.0;
	
	serial=items[bp].serial;	//pack's serial number
	serhash=serial%HASHMAX;				//contents lookup hash
	for (i=0;i<contsp[serhash].max;i++)
	{
		ci=contsp[serhash].pointer[i];
		if (ci>-1)
		{
		  if ((items[ci].contserial==serial))
		  {
			  itemsweight=Weight->ItemWeight(ci);
			  if (items[ci].type==1) //item is another container
			  {
				totalweight = totalweight + ((double)((double)(itemsweight))/(double)(100.00)); //(items[ci].weight/100);
				totalweight+=Weight->RecursePacks(ci); //find the item's weight within this container
			  }
			
			  if (items[ci].id1==0x0E && items[ci].id2==0xED)
				totalweight = totalweight + (double)((double)(items[ci].amount)*(double)(GOLD_WEIGHT));
			else totalweight = totalweight + (double)(((double)(itemsweight)*(double)(items[ci].amount))/100.00); //((items[ci].weight*items[ci].amount)/100);  // Ison 2-21-99
		  }
		}
	}
	return totalweight;
}

// return the weight of an item. May have to seek it from MUL files
int cWeight::ItemWeight(int item)
{
	unsigned int itemweight=0;
	tile_st tile;

  if (items[item].weight<=0) //weight is defined in scripts for this item
  {
		Map->SeekTile((items[item].id1<<8)+items[item].id2, &tile);
		if (tile.weight==0) // can't find weight
		{
		
			if(items[item].type!=14)	itemweight = 2; // not food weighs .02 stone
			else if (items[item].type==14) itemweight = 100; //food weighs 1 stone
		}
		else //found the weight from the tile, set it for next time
		{			
			itemweight = tile.weight*100;
			items[item].weight=itemweight; // set weight so next time don't have to search
		}
  }
	else itemweight=items[item].weight;
	return itemweight;
}

/*float calcweight(int p)	//First let's grab all item that player wear
{
	int p1, p2, p3, p4;
	unsigned int i;
	float totalweight=0;
	tile_st tile;

	p1=chars[p].ser1;
	p2=chars[p].ser2;
	p3=chars[p].ser3;
	p4=chars[p].ser4;


	for (i=0;i<itemcount;i++)
	
		if ((items[i].cont1==p1)&&(items[i].cont2==p2)
	    	&&(items[i].cont3==p3)&&(items[i].cont4==p4)&&(!items[i].free))
		{
		  if(items[i].type==1 &&(items[i].id1!=0x09 && items[i].id2!=0xAB))
		  {
			  Map->SeekTile((items[i].id1*256)+items[i].id2, &tile);
		      totalweight += tile.weight;
			  totalweight += Weight->CalcWeightPack(i);
		  }
		  else 
		  { 
			  if (items[i].id1!=0x09 && items[i].id2!=0xAB)
			  {
				Map->SeekTile((items[i].id1*256)+items[i].id2, &tile);
				if (tile.weight==0)	totalweight += (items[i].amount*0.02);
				else totalweight += (items[i].amount*tile.weight);
			  }
		  }
		}
	
	return totalweight;
}*/
	
double cWeight::CalcWeightPack(int backpack) 	//Next let's grab all item in player backpack
{
	int k, a;
	unsigned int i;
	double totalweight=0;
	tile_st tile;

	k=backpack;
	
	if (k<=-1) return 0.0;

	for(a=0;a<contsp[items[k].serial%HASHMAX].max;a++)
	{
		i=contsp[items[k].serial%HASHMAX].pointer[a];
		if (i!=-1)
		{
		  if (items[i].type==1||items[i].type==8)
		  {
			Map->SeekTile((items[i].id1<<8)+items[i].id2, &tile);
		    totalweight += tile.weight;
		 	totalweight += Weight->CalcWeightPack(i);
		  }
		  else 
		  {
			Map->SeekTile((items[i].id1<<8)+items[i].id2, &tile);
			if (tile.weight==0)	
			{
				if(items[i].id1==0x0E&&items[i].id2==0xED)
					totalweight = totalweight + (double)((double)(items[i].amount)*(double)(GOLD_WEIGHT));
				else totalweight = totalweight + (double)((double)(items[i].amount)*(double)(0.1));
			}
			else totalweight += (items[i].amount*tile.weight);
		  }
		} // end of if i!=-1
	} // end for loop

return totalweight;
}
 

char cWeight::CheckMaxWeightPack(int chars) //Check max capacity of container
{								   //Not used for now 
	int pack;
	unsigned int i;
	double  totalweight=0;
	tile_st tile;

	pack=packitem(chars);
	if (pack==-1) return 0;  //LB

    /*for (i=0;i<itemcount;i++) //Lag Fix -- Zippy
	
		if((items[i].cont1==items[pack].ser1)&&(items[i].cont2==items[pack].ser2)&&
			(items[i].cont3==items[pack].ser3)&&(items[i].cont4==items[pack].ser4)&&(!items[i].free))
		{*/
	for(int a=0;a<contsp[items[pack].serial%HASHMAX].max;a++)
	{
		i=contsp[items[pack].serial%HASHMAX].pointer[a];
		
		    if (i!=-1) // LB
			if(items[i].type==1||items[i].type==8) totalweight += Weight->CalcWeightPack(i);
			else 
			{
				Map->SeekTile((items[i].id1<<8)+items[i].id2, &tile);
				totalweight += (items[i].amount*tile.weight);
			}
		}
		if (items[pack].weight>=totalweight) return 0;
		else 
		{
		  sysmessage(chars,"Your backpack are full!");
		  return 1;
		}
}

int cWeight::CheckWeight(int s, int k) // Check when player is walking if overloaded
// PARAM WARNING: k is never used
{
   if ((chars[s].weight > (chars[s].st*WEIGHT_PER_STR)+30))
   {
	   float res=float(chars[s].weight - ((chars[s].st*WEIGHT_PER_STR)+30))*2;
	
	   chars[s].stm -= (int)res;
	   if (chars[s].stm<=0)
      {
         chars[s].stm=0;
		 // AntiChrist - displays a message
		 sysmessage( calcSocketFromChar( s ), "You are overloaded! You can't hold all this weight.." );
	      return 0;
	   }
   }
   return 1;
}

void cWeight::AddItemWeight(int i, int s) //add weight of this item  on  player's weight
{
   tile_st tile;
  
	Map->SeekTile((items[i].id1<<8)+items[i].id2, &tile);
   if (items[i].type==1||items[i].type==8) //if item is container add container
   {									    //and all item  weight
	   chars[s].weight += (tile.weight);
       chars[s].weight += (int)(Weight->CalcWeightPack(i));
   }
   else
   {
	  if (tile.weight==0)	
	   {
		if(items[i].id1==0x0E && items[i].id2==0xED)
			chars[s].weight += (int)(items[i].amount*GOLD_WEIGHT);
		else chars[s].weight += (int)(items[i].amount*0.1);
	   }	
	   else chars[s].weight += (items[i].amount*tile.weight);
   }
}

void cWeight::SubtractItemWeight(int i, int s) //subtract weight of this item on player's weight
{									  
   tile_st tile;
   Map->SeekTile((items[i].id1<<8)+items[i].id2, &tile);
   if (items[i].type==1||items[i].type==8) //if item is container subtract container 
   {									   //and all item  weight
  	    chars[s].weight -= tile.weight;
        chars[s].weight -= (int)(Weight->CalcWeightPack(i));
   }
   else
   {
	   if (tile.weight==0)	
	   {
		if(items[i].id1==0x0E&&items[i].id2==0xED)
			chars[s].weight -= (int)(items[i].amount*GOLD_WEIGHT);
		else chars[s].weight -= (int)(items[i].amount*0.1);
	   }	
	   else chars[s].weight -= (items[i].amount*tile.weight);
   }
}

int cWeight::CheckWhereItem( int pack, int i, int s)
{

  if (i!=-1 && pack!=-1 && s!=-1) //LB
  {
    if (!((items[i].cont1==items[pack].ser1 && items[i].cont2==items[pack].ser2 &&
         items[i].cont3==items[pack].ser3 && items[i].cont4==items[pack].ser4 )||
	    (items[i].cont1==chars[currchar[s]].ser1&&items[i].cont2==chars[currchar[s]].ser2&&
	     items[i].cont3==chars[currchar[s]].ser3&&items[i].cont4==chars[currchar[s]].ser4)))
	
	    return 1;
  else return 0;
  }
  return 0;
}

void cWeight::SubtractQuanWeight(int i, int s, int total) //subtract weight of this item on player's weight
{									  
   tile_st tile;

   Map->SeekTile((items[i].id1<<8)+items[i].id2, &tile);
   	   if (tile.weight==0)	
	   {
		if(items[i].id1==0x0E&&items[i].id2==0xED)
			chars[s].weight -= (int)(total*GOLD_WEIGHT);
		else chars[s].weight -= (int)(total*0.1);
	   }	
	   else chars[s].weight -= (total*tile.weight);
  
}

void cWeight::AddAmountWeight(int i, int s, int amount) //add weight of this item  on  player's weight
{
   tile_st tile;

	Map->SeekTile((items[i].id1<<8)+items[i].id2, &tile);
     if (tile.weight==0)	
	   {
		if(items[i].id1==0x0E && items[i].id2==0xED)
			chars[currchar[s]].weight += (int)(amount*GOLD_WEIGHT);
		else chars[currchar[s]].weight += (int)(amount*0.1);
	   }	
	   else chars[currchar[s]].weight += (amount*tile.weight);
   
}

int cWeight::WhereItem( int p, int i, int s) // To find if item are in possesion of player
{									// If return is 1 item are not on player otherwise they are on	
	if ((items[i].cont1==255 && items[i].cont2==255 && items[i].cont3==255 && items[i].cont4==255)||
		(items[p].cont1==255 && items[p].cont2==255 && items[p].cont3==255 && items[p].cont4==255)||
		(items[p].id1==0x09 && items[p].id2==0xAB))  return 1;

	else if ((items[i].cont1==items[p].ser1 && items[i].cont2==items[p].ser2 &&
			  items[i].cont3==items[p].ser3 && items[i].cont4==items[p].ser4)||
             (items[i].cont1==chars[currchar[s]].ser1 && items[i].cont2==chars[currchar[s]].ser2 &&
		      items[i].cont3==chars[currchar[s]].ser3 && items[i].cont4==chars[currchar[s]].ser4))
			 return 0;
	else return (Weight->WhereSub(i,p));
}

int cWeight::WhereSub(int i, int p) // If item are in a subcontainer posseded by player 
{						   // If return is 1 item are not on player otherwise they are on
	long a, b;
	//for (unsigned int a=0;a<itemcount;a++)
	if ( items[i].contserial < 1 )
		return 1;

	for (b=0;b<contsp[items[i].contserial%HASHMAX].max;b++)
	{
		a=contsp[items[i].contserial%HASHMAX].pointer[b];
		
		if (a>-1)
		{
			if (items[i].cont1==items[a].ser1 && items[i].cont2==items[a].ser2 &&
				items[i].cont3==items[a].ser3 && items[i].cont4==items[a].ser4   )
			{
				if (items[a].cont1==items[p].ser1 && items[a].cont2==items[p].ser2 &&
					items[a].cont3==items[p].ser3 && items[a].cont4==items[p].ser4   )
					 return 0;

				else if ((items[a].cont1==255 && items[a].cont2==255 &&
						  items[a].cont3==255 && items[a].cont4==255) ||
						 (items[a].id1==0x09 && items[a].id2==0xAB)) 
						 return 1;

				else return (Weight->WhereSub(a,p));
			}
		}
	}
	return 1;
}

int cWeight::CheckWeight2(int s) // Morrolan - Check when player is teleporting if overloaded
{
	if ((chars[s].weight > (chars[s].st*WEIGHT_PER_STR)+30))
	{
		chars[s].mn -= 30;
		if (chars[s].mn<=0)
		{
			chars[s].mn=0;
		}
		return 1;
	}
	return 0;
}
