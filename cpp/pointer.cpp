//------------------------------------------------------------------------
//  pointer.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by Unknown real name (Tauriel)
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
// - Tauriel experimental pointer array *crap*
//
// -- pointer.cpp		Item specific routines (add, delete change) in preperation for
//									going to a pointer based system (compiled under level 3 no errors!
//
#include <uox3.h>
#include <debug.h>

#define DBGFILE "pointer.cpp"

//
// - Sets an item into the array, reallocating space if needed
//   Usage: setptr(&itemsp[serial%256], item#);
//          setptr(&charsp[serial%256], char#);
//   NOTE: can be used to set other pointer arrays too (like regions)
void setptr(lookuptr_st *ptr, int item) //set item in pointer array
{
	int i;
	if (ptr == NULL) //this should NEVER NEVER HAPPEN!
		return;
	else if (ptr->pointer == NULL) 
	{
		ptr->pointer = new int[25];
		if (ptr->pointer == NULL) 
			return;
		memset(ptr->pointer, 0xFF, 25*sizeof(int));
		//ConOut("%i!!", ptr->max );
		ptr->max = 25;
	}

	for (i=0;i<(ptr->max);i++)
	{		
		if (ptr->pointer[i]==-1)
		{
			ptr->pointer[i]=item;
			return;
		} 
		else if (ptr->pointer[i]==item) 
			return;
	}
	
	// Must be out of slots, so reallocate some and set item
	int newlength = ptr->max+25;
	int *tmpptr = new int[ptr->max];
	//---Reallocate---
	memcpy(tmpptr, ptr->pointer, ptr->max*sizeof(int));//copy to temp
	delete [] ptr->pointer;//delete old
	ptr->pointer = new int[newlength];//create new
	memset(ptr->pointer, 0xFF, newlength*sizeof(int));//set everything to -1
	memcpy(ptr->pointer, tmpptr, ptr->max*sizeof(int));//copy in the on top of the -1s
	delete [] tmpptr;//delete the temp
	
	ptr->pointer[ptr->max]=item;
	ptr->max = newlength;
	return;
}

// - Find a specific item/char by serial number.
//   Usage: item=findbyserial(&itemsp[serial%256], serial, 0);
//          char=findbyserial(&charsp[serial%256], serial, 1);
//          if (item==-1) ConOut("Couldn't find by serial: %d\n", serial);
int findbyserial(lookuptr_st *ptr, int nSerial, int nType)
{ 
	if (nSerial<0) return-1;  //prevent errors from some clients being slower than the server clicking on nolonger valid items
	
	int i=0;
	int cnt=1;
	for ( ;i<(ptr->max);i++, cnt++ )
	{ 
		if ((nType==0) && (ptr->pointer[i]>-1 && ptr->pointer[i]<imem) && 
			(items[ptr->pointer[i]].serial==nSerial))
		{
			//ConOut("Found item %d out of %d in %d hits.\n",ptr->pointer[i],itemcount,cnt);
			return ptr->pointer[i];
		}
		if ((nType==1) && (ptr->pointer[i]>-1 && ptr->pointer[i]<cmem) && 
			(chars[ptr->pointer[i]].serial==nSerial))
		{
			// ConOut("Found char %d out of %d in %d hits.\n",ptr->pointer[i],charcount,cnt);
			return ptr->pointer[i];
		}
	}
	return -1;
}

// - Remove an item/char from a pointer array
//   (Ok, just mark it as a free slot ;P )
//   Usage: if (!removefromptr(&itemsp[serial%256], item))
//            ConOut "Error removing item/char %d from pointer array\n",item);
int removefromptr(lookuptr_st *ptr, int nItem)
{
	int i;
	for (i=0;i<(ptr->max);i++)
	{
		if (ptr->pointer[i]==nItem)
		{
			ptr->pointer[i]=-1;
			return 1;
		}
	}
	return 0;
}


void setserial(int nChild, int nParent, int nType)
{ // Sets the serial #'s and adds to pointer arrays
	// types: 1-item container, 2-item spawn, 3-Item's owner 4-container is PC/NPC
	//        5-NPC's owner, 6-NPC spawned
	if( nChild == -1 || nParent == -1 ) 
		return;
	switch( nType )
	{
	case 1:  //Set the item's container
		items[nChild].cont1=items[nParent].ser1;
		items[nChild].cont2=items[nParent].ser2;
		items[nChild].cont3=items[nParent].ser3;
		items[nChild].cont4=items[nParent].ser4;
		items[nChild].contserial=items[nParent].serial;
		setptr(&contsp[items[nChild].contserial%HASHMAX], nChild);
		break;
	case 2:	//Set the item's spawner
		items[nChild].spawn1=items[nParent].ser1;
		items[nChild].spawn2=items[nParent].ser2;
		items[nChild].spawn3=items[nParent].ser3;
		items[nChild].spawn4=items[nParent].ser4;
		items[nChild].spawnserial=items[nParent].serial;
		setptr(&spawnsp[items[nChild].spawnserial%HASHMAX], nChild);
		break;
	case 3:  //Set the item's owner
		items[nChild].owner1=chars[nParent].ser1;
		items[nChild].owner2=chars[nParent].ser2;
		items[nChild].owner3=chars[nParent].ser3;
		items[nChild].owner4=chars[nParent].ser4;
		items[nChild].ownserial=chars[nParent].serial;
		setptr(&ownsp[items[nChild].ownserial%HASHMAX], nChild);
		break;
	case 4:  //Set the Container to a character
		items[nChild].cont1=chars[nParent].ser1;
		items[nChild].cont2=chars[nParent].ser2;
		items[nChild].cont3=chars[nParent].ser3;
		items[nChild].cont4=chars[nParent].ser4;
		items[nChild].contserial=chars[nParent].serial;
		setptr(&contsp[chars[nParent].serial%HASHMAX], nChild);
		break;
	case 5:  //Set the character's owner
		chars[nChild].own1=chars[nParent].ser1;
		chars[nChild].own2=chars[nParent].ser2;
		chars[nChild].own3=chars[nParent].ser3;
		chars[nChild].own4=chars[nParent].ser4;
		chars[nChild].ownserial=chars[nParent].serial;
		setptr(&cownsp[chars[nChild].ownserial%HASHMAX], nChild);
		if( nChild != nParent && chars[nChild].npc )
			chars[nChild].tamed = true;
		else
			chars[nChild].tamed = false;
		break;
	case 6:  // Set the character's spawner
		chars[nChild].spawn1=items[nParent].ser1;
		chars[nChild].spawn2=items[nParent].ser2;
		chars[nChild].spawn3=items[nParent].ser3;
		chars[nChild].spawn4=items[nParent].ser4;
		chars[nChild].spawnserial=items[nParent].serial;
		setptr(&cspawnsp[chars[nChild].spawnserial%HASHMAX], nChild);
		break;
	case 7: //Set the ITEM in a multi
		items[nChild].multi1=items[nParent].ser1;
		items[nChild].multi2=items[nParent].ser2;
		items[nChild].multi3=items[nParent].ser3;
		items[nChild].multi4=items[nParent].ser4;
		items[nChild].multis=items[nParent].serial;
		setptr(&imultisp[items[nChild].multis%HASHMAX], nChild);
		break;
	case 8: //Set the CHARACTER in a multi
		chars[nChild].multi1=items[nParent].ser1;
		chars[nChild].multi2=items[nParent].ser2;
		chars[nChild].multi3=items[nParent].ser3;
		chars[nChild].multi4=items[nParent].ser4;
		chars[nChild].multis=items[nParent].serial;
		setptr(&cmultisp[chars[nChild].multis%HASHMAX], nChild);
		break;
	}
}

void unsetserial( int nChild, int nType )
{
	switch( nType )
	{
	case 1:
		if( items[nChild].contserial == -1 )
			return;
		removefromptr( &contsp[items[nChild].contserial%HASHMAX], nChild );
		items[nChild].cont1 = 0xFF;
		items[nChild].cont2 = 0xFF;
		items[nChild].cont3 = 0xFF;
		items[nChild].cont4 = 0xFF;
		items[nChild].contserial = -1;
		break;
	case 2:		break;
	case 3:		break;
	case 4:		break;
	case 5:		break;
	case 6:		break;
	case 7:		
		removefromptr( &imultisp[items[nChild].multis%HASHMAX], nChild );
		items[nChild].multi1 = 0xFF;
		items[nChild].multi2 = 0xFF;
		items[nChild].multi3 = 0xFF;
		items[nChild].multi4 = 0xFF;
		items[nChild].multis = -1;
		break;
	case 8:		
		removefromptr( &cmultisp[chars[nChild].multis%HASHMAX], nChild );
		chars[nChild].multi1 = 0xFF;
		chars[nChild].multi2 = 0xFF;
		chars[nChild].multi3 = 0xFF;
		chars[nChild].multi4 = 0xFF;
		chars[nChild].multis = -1;
		break;
	default:	break;
	}

}
