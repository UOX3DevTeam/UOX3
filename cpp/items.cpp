//------------------------------------------------------------------------
//  items.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by Marcus Rating (Cironian)
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
//
// -- Items.cpp  Item specific routines (add, delete change) in preperation for
//               going to a pointer based system
//
#include <uox3.h>
#include <debug.h>

#define DBGFILE "items.cpp"

//Instance of cItemHandle class to handle item memory//

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::cItemHandle()
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
cItemHandle::cItemHandle( void )
{
	DefaultItem = new item_st;
	memset(DefaultItem, 0, sizeof(item_st));
	
	//setup the item's important properties (ones that might be checked if its used, and need to be this way
	DefaultItem->free = 1;
	DefaultItem->ser1 = DefaultItem->ser2 = DefaultItem->ser3 = DefaultItem->ser4 = 0xFF;
	DefaultItem->serial = 0;
	DefaultItem->contserial = DefaultItem->ownserial = DefaultItem->spawnserial = -1;

	Acctual = Free = 0;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::~cItemHandle()
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
cItemHandle::~cItemHandle()
{
	unsigned long i;

	for (i=0;i<Items.size();i++)//Memory Cleanup
	{
		if (Items[i] != NULL)
			delete Items[i];
	}

	Items.clear();
	Items.resize(0);
	
	FreeNums.clear();
	FreeNums.resize(0);

	delete DefaultItem;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::New()
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Free memory for an item, create the item in memory 
//|					(returns item number)
//o---------------------------------------------------------------------------o
unsigned long cItemHandle::New( void )
{
	unsigned long i;
	
	if ( Free > 0 )
	{
		i = FreeNums[FreeNums.size()-1];//get the oldest entry
		FreeNums.resize( FreeNums.size() - 1 ); //Delete it cause it ain't free no more.
		Free = max( Free-1, (unsigned int)0 );

		if (Items[i] != NULL)
			delete Items[i];

		Items[i] = new item_st;
	} else {
		i = Items.size();
		//Items.resize( i+1 );
		//Items[i] = new item_st;
		Items.push_back( new item_st );
	}

	Acctual++;

	return i;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::Delete( long int )
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Free item memory that's used by this item (delete it)
//o---------------------------------------------------------------------------o
void cItemHandle::Delete( long int Num )
{
	if ( Num > -1 && Num < Items.size() )
	{
		if ( Items[Num] != NULL  )
		{
			delete Items[Num];
			Items[Num] = NULL;

			FreeNums.insert(FreeNums.begin(), Num);
			Free++;
			Acctual = max(Acctual-1, (unsigned long)0);
		} else if ( !isFree( Num ) )
		{
			FreeNums.insert(FreeNums.begin(), Num);
			Free++;
			Acctual = max(Acctual-1, (unsigned long)0);
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::Size()
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Get the size (in bytes) that items are taking up in memory
//o---------------------------------------------------------------------------o
unsigned long cItemHandle::Size( void )
{
	unsigned long sz;

	sz = Items.size() * 4;
	sz += FreeNums.size() * sizeof(unsigned long);
	sz += sizeof(item_st) * Acctual;
	sz += sizeof(cItemHandle);

	return sz;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::Reserve( unsigned int )
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Reserve memory for Num number of items (unused)
//o---------------------------------------------------------------------------o
void cItemHandle::Reserve( unsigned int Num )
{
	unsigned int i, cs = FreeNums.size(), is = Items.size();
	
	Free+=Num;
	FreeNums.resize( cs + Num );
	Items.resize( is+Num );

	for (i=cs;i<(cs+Num);i++)
	{// is + (cs+(Num-1))-i) is acctual number ( 0 to Num )
		FreeNums[i] = is + ((cs+(Num-1))-i);
		Items[(is + (cs+(Num-1))-i)] = NULL;
	}
}

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::isFree( unsigned long)
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Check to see if item Num is marked free (unused (slow))
//o---------------------------------------------------------------------------o
bool cItemHandle::isFree( unsigned long Num )
{
	unsigned int i;
	for (i=0;i<FreeNums.size();i++)
	{
		if ( FreeNums[i] == Num )
			return true;
	}
	return false;
}
const bool EVILDED=false;
//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::operator[]( long int )
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Reference item Num -  Check to make sure Num is a valid 
//|					item number and exists in memory
//o---------------------------------------------------------------------------o
item_st& cItemHandle::operator[] ( long int Num )
{
	if ( Num >= 0 && Num < Items.size())
	{
		if ( Items[Num] != NULL )//&& !isFree( Num ) ) //isFree isSlow
			return *Items[Num];	
	}
  else
  {
    ConOut("WARNING: Items[%i] referenced in invalid. Crash averted!\n", Num);
  }
	//Make sure these props are always this way, they may have been chaged by other functions, so put them back
	DefaultItem->free = 1;
	DefaultItem->ser1 = DefaultItem->ser2 = DefaultItem->ser3 = DefaultItem->ser4 = 0xFF;
	DefaultItem->serial = 0;
	DefaultItem->x = DefaultItem->y = 0;
	DefaultItem->contserial = DefaultItem->ownserial = DefaultItem->spawnserial = -1;
	return *DefaultItem;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::Count()
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Return the number of items in the world (Acctual)
//o---------------------------------------------------------------------------o
unsigned long cItemHandle::Count( void )
{
	return Acctual;
}
//End of cItemHandle class//


// -- Find a free item slot, checking freeitemmem[] first
int cItem::MemItemFree()
{
	static unsigned int OldRealloc=0;
	signed int nItem=-1;
	char memerr=0;

	nItem = items.New();

	int slots=4000;	// bugfix for crashes under w95/w98
					// careful with this number.....
					// if it too low => slow and can cause crashes under
					// w95/98.  This is because w95/98 can only handle 8196
					// subsequent realloc calls ( crappy thing...)
					// free() calls DON'T help !!!, btw
					// so we have to make that number real big
					// under nt and unix this limit doesn't exist
	if (itemcount>=imem) //theres no more free sluts.. er slots, so get more memory
	{
		int *Copy_loscache;
		int *Copy_itemids;

		// initialize to NULL so if an error has occurred we can properly
		// free them by checking for NULL
		Copy_loscache = NULL;
		Copy_itemids = NULL;

		// create new objects to copy original data to
		if(( Copy_loscache = new int[imem] ) == NULL ) memerr = 1;
		else if(( Copy_itemids = new int[imem] ) == NULL ) memerr = 1;

		// make sure nothing bad happened
		if( !memerr )
		{
			// make a copy - I prefer memcpy although a copy constructor wouldn't hurt =)
			memcpy(Copy_loscache, loscache, sizeof( int ) * imem );
			memcpy(Copy_itemids, itemids, sizeof( int ) * imem );

			// delete the old objects
			delete [] itemids;
			delete [] loscache;

			// initialize to NULL so if an error has occurred we can properly
			// free them by checking for NULL
			loscache = NULL;
			itemids = NULL;

			// create new objects with more room for more items, etc.
			if(( loscache = new int[imem + slots] ) == NULL ) memerr = 2;
			else if(( itemids = new int[imem + slots] ) == NULL ) memerr = 2;

			if( !memerr )
			{
				// restore copy to new objects
				memcpy( loscache, Copy_loscache, sizeof( int ) * imem );
				memcpy( itemids, Copy_itemids, sizeof( int ) * imem );

				// delete copies
				delete [] Copy_itemids;
				delete [] Copy_loscache;
			}
		}
		if (memerr)
		{
			// cleanup if neccessary
			if( memerr >= 1 )
			{
				if( Copy_itemids ) delete [] Copy_itemids;
				if( Copy_loscache ) delete [] Copy_loscache;

				// cleanup if neccessary
				if( memerr == 2 )
				{
					if( itemids ) delete [] Copy_itemids;
					if( loscache ) delete [] Copy_loscache;
				}
			}
			ConOut("ERROR: Could not reallocate item memory after %i. No more items will be created.\nWARNING: UOX may become unstable.\n",imem);
//			cwmWorldState->savenewworld(1);           // Commented out because it wipes your items =)
		} 
		else 
		{
			imem+=slots;
		}
	}

	return nItem;
}




// -- Initialize an Item in the items[] array
void cItem::InitItem(int nItem, char ser)
{
	if (nItem==-1) return;

	// krazyglue - initialize everything to zero with one shot (saves a LOT of excess 'xx = 0' calls!)
	// someone could also take out all those commented lines below, although I would suggest
	// writing those nifty descriptions that someone so happily entered below! =)
	memset( &items[nItem], 0, sizeof( struct item_st ) );

	if (ser)
	{
		splitSerial(itemcount2, items[nItem].ser1, items[nItem].ser2, items[nItem].ser3, items[nItem].ser4);
		items[nItem].serial=itemcount2;
		setptr(&itemsp[itemcount2%HASHMAX], nItem);
		itemcount2++;
	} else {
		items[nItem].ser1=255; // Item serial number
		items[nItem].ser2=255;
		items[nItem].ser3=255;
		items[nItem].ser4=255;
		items[nItem].serial=-1;
	}
	if (nItem==itemcount) itemcount++;

	items[nItem].name[0] = '#';
	items[nItem].name2[0] = '#';
	items[nItem].creator[0] = 0;
	items[nItem].good = -1; // Magius(CHE)

	items[nItem].multi1 = 255;//Multi serial1
	items[nItem].multi2 = 255;//Multi serial2
	items[nItem].multi3 = 255;//Multi serial3
	items[nItem].multi4 = 255;//Multi serial4
	items[nItem].multis = -1;//Multi serial
	
	items[nItem].id2 = 0x01;
	items[nItem].x=100;
	items[nItem].y=100;
	items[nItem].cont1 = 255; // Container that this item is found in
	items[nItem].cont2 = 255;
	items[nItem].cont3 = 255;
	items[nItem].cont4 = 255;
	items[nItem].contserial=-1;
	items[nItem].amount=1; // Amount of items in pile
	items[nItem].carve = -1; // AntiChrist - for new carving system
	items[nItem].gatenumber=-1;
	items[nItem].owner1=255;
	items[nItem].owner2=255;
	items[nItem].owner3=255;
	items[nItem].owner4=255;
	items[nItem].ownserial=-1;
	items[nItem].racialEffect = 65535;
}
// -- delete an item (Actually just mark it is free)
void cItem::DeleItem(int i)
{
	unsigned int j;
	int ci, ptr;

	if( items[i].free == 0 )
	{
		if( items[i].contserial != 0xFFFFFFFF && items[i].contserial < 0x40000000 ) // it's a player!
		{
			CHARACTER player = calcCharFromSer( items[i].contserial );
			if( player != -1 )
			{
				/************************************************************/
				/* OakWood "plate destroyed but don't restore stats" bugfix */
				chars[player].st -= items[i].st2;
				chars[player].dx -= items[i].dx2;
				chars[player].in -= items[i].in2;
				if(items[i].poisoned) 
				{
					chars[player].poison -= items[i].poisoned;
					if( chars[player].poison < 0 ) 
						chars[player].poison = 0;
				}			
				/************************************************************/
			}
		}
		removeitem[1]=items[i].ser1;
		removeitem[2]=items[i].ser2;
		removeitem[3]=items[i].ser3;
		removeitem[4]=items[i].ser4;
		for (j=0;j<now;j++)
		{
			if (perm[j]) Network->xSend(j, removeitem, 5, 0);
		}
		if( items[i].glow > 0 )
		{
			int j = calcItemFromSer( items[i].glow );
			if( j != -1 )
				DeleItem( j );  // LB glow stuff, deletes the glower of a glowing stuff automatically
		}
		if (items[i].type==1 || items[i].type==63 || items[i].type==65 || items[i].type==87)
		{
			for (ptr=0;ptr<contsp[items[i].serial%HASHMAX].max;ptr++)
			{
				ci=contsp[items[i].serial%HASHMAX].pointer[ptr];
				if (ci!=-1)
//					DeleItem( ci );
				{
					//recursion_is_devine=1;
					//DeleItem(ci);
				}
			}
		}

		// - remove from pointer arrays
		removefromptr(&itemsp[items[i].serial%HASHMAX], i);
		if( items[i].spawnserial != -1 && items[i].spawnserial != 0 ) 
			removefromptr(&spawnsp[items[i].spawnserial%HASHMAX], i);
		if( items[i].ownserial != -1 ) 
			removefromptr(&ownsp[items[i].ownserial%HASHMAX], i);
		if( items[i].contserial != -1 ) 
			removefromptr(&contsp[items[i].contserial%HASHMAX], i);
		// - remove from mapRegions if a world item
		if( items[i].contserial == -1 )
		{
			if( da != 1 ) 
				mapRegions->RemoveItem( i ); // da==1 not added !!
		}
		items[i].free=1;
		items[i].x=20+(xcounter++);
		items[i].y=50+(ycounter);
		items[i].z=9;
		items[i].cont1=255;
		items[i].cont2=255;
		items[i].cont3=255;
		items[i].cont4=255;
		items[i].contserial=-1;

		if (xcounter==40)
		{
			ycounter++;
			xcounter=0;
		}
		if (ycounter==80)
		{
			ycounter=0;
			xcounter=0;
		}
	}
	items.Delete( i );
}

// sockets
int cItem::CreateScriptItem(int s, int itemnum, int nSpawned)
{
	char sect[512];
	int tmp, theitem;
	unsigned int c;
	long int pos;
	tile_st tile;
 
	openscript("items.scp");
	sprintf(sect, "ITEM %i", itemnum);
	if (!i_scripts[items_script]->find(sect))
	{
		closescript();
		if (n_scripts[custom_item_script][0]!=0)
		{
			openscript(n_scripts[custom_item_script]);
			if (!i_scripts[custom_item_script]->find(sect))
			{
				closescript();
				return -1;
			}
			else strcpy(sect, n_scripts[custom_item_script]);
		} else return -1;
	} else strcpy(sect, "items.scp");

	c=MemItemFree();
	if( c == -1 )
		return -1;

	InitItem(c);
	theitem=c;
	items[c].id1=0x09;
	items[c].id2=0x15;
	items[c].wipe=0;
	items[c].amount = 1;		// obviously some things have no default val, and 0 is not a good number
	if ((s!=-1) && (!nSpawned))
	{
		if (triggerx)
		{
			items[c].x=triggerx;
			items[c].y=triggery;
			items[c].z=triggerz;
		}
		else
		{
			items[c].x=(buffer[s][11]<<8)+buffer[s][12];
			items[c].y=(buffer[s][13]<<8)+buffer[s][14];
			items[c].z=buffer[s][16]+Map->TileHeight((buffer[s][17]<<8)+buffer[s][18]);
		}
	}


	int anum;   
	do
	{
		read2();
		if (script1[0]!='}')
		{
			switch( script1[0] )
			{
			case 'a':
			case 'A':
				if (!(strcmp("AMOUNT",script1))) items[c].amount=str2num(script2);
				else if (!(strcmp("ATT", script1 ))) items[c].att = str2num( script2 );
				else if (!(strcmp("AC", script1))) items[c].armorClass = str2num( script2 );
				break;
			case 'c':
			case 'C':
				if (!(strcmp("CREATOR", script1))) strcpy( items[c].creator, script2 ); // by Magius(CHE)
				else if (!(strcmp("COLOR",script1)))
				{
					tmp=hstr2num(script2);
					items[c].color1=(unsigned char)(tmp>>8);
					items[c].color2=(unsigned char)(tmp%256);
				}
				else if (!(strcmp(script1, "CORPSE"))) items[c].corpse=str2num(script2);
				break;
			case 'd':
			case 'D':
				if ((!(strcmp("DAMAGE",script1)))||(!(strcmp("ATT",script1)))) 
					items[c].att = getstatskillvalue(script2);
				else if (!(strcmp("DEF",script1))) items[c].def=str2num(script2);
				else if (!(strcmp("DEX", script1))) items[c].dx=str2num(script2);
				else if (!(strcmp("DEXADD", script1))) items[c].dx2=str2num(script2);
				else if (!(strcmp("DIR",script1))) items[c].dir=str2num(script2);
				else if (!(strcmp("DYE",script1))) items[c].dye=str2num(script2);
				else if (!(strcmp("DECAY",script1))) items[c].priv=items[c].priv|0x01;
				else if (!(strcmp("DISPELLABLE",script1))) items[c].priv=items[c].priv|0x04;
				else if (!(strcmp("DISABLED",script1))) items[c].disabled=str2num(script2);
				else if (!(strcmp("DX", script1))) items[c].dx=str2num(script2);
				else if (!(strcmp("DX2", script1))) items[c].dx2=str2num(script2);
				else if (!(strcmp(script1, "DYEABLE"))) items[c].dye=str2num(script2);
				else if (!(strcmp(script1, "DOORFLAG"))) items[c].doordir=str2num(script2);
				break;
			case 'g':
			case 'G':
				if (!(strcmp("GOOD", script1 ))) items[c].good = str2num( script2 ); // Added by Magius(CHE)
				else if( !(strcmp(script1, "GLOW"))) items[c].glow = str2num( script2 );
				else if( !(strcmp(script1, "GLOWBC")))
				{
					unsigned int i = str2num( script2 );
					items[c].glow_c1 = (unsigned char)(i>>8);
					items[c].glow_c2 = (unsigned char)(i%256);
				}
				else if( !(strcmp(script1, "GLOWTYPE"))) items[c].glow_effect = str2num( script2 ); 
				break;
			case 'h':
			case 'H':
				if (!(strcmp("HP", script1))) items[c].hp=str2num(script2);
				else if (!(strcmp("HIDAMAGE", script1))) items[c].hidamage=str2num(script2);
				break;
			case 'i':
			case 'I':
				if (!(strcmp("ITEMLIST", script1)))
				{
					pos=ftell(scpfile);
					closescript();
					DeleItem(c);
					theitem = c = CreateScriptRandomItem(s, script2);
					if( c == -1 )
						return -1;
					//addrandomitem(c,script2,s);
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					strcpy(script1, "DUMMY");
				}
				else if (!(strcmp("ID",script1)))
				{
					tmp=hstr2num(script2);
					items[c].id1=(unsigned char)(tmp>>8);
					items[c].id2=(unsigned char)(tmp%256);
				}
				else if (!(strcmp("INT", script1))) items[c].in=str2num(script2);
				else if (!(strcmp("INTADD", script1))) items[c].in2=str2num(script2);
				else if (!(strcmp(script1, "IN"))) items[c].in=str2num(script2); 
				else if (!(strcmp(script1, "IN2"))) items[c].in2=str2num(script2); 
				else if (!(strcmp("ITEMHAND", script1 ))) items[c].itmhand = str2num( script2 );
				break;
			case 'l':
			case 'L':
				if (!(strcmp("LODAMAGE", script1))) items[c].lodamage=str2num(script2);
				else if (!(strcmp("LAYER",script1))&&(s==-1)) items[c].layer=str2num(script2);
				break;
			case 'm':
			case 'M':
				if (!(strcmp("MAXHP", script1))) items[c].maxhp=str2num(script2);
				else if (!(strcmp("MOVABLE",script1))) items[c].magic=str2num(script2);
				else if (!(strcmp("MORE", script1)))
				{
					tmp=str2num(script2);
					items[c].more1=(unsigned char)(tmp>>24);
					items[c].more2=(unsigned char)(tmp>>16);
					items[c].more3=(unsigned char)(tmp>>8);
					items[c].more4=(unsigned char)(tmp%256);
				}
				else if (!(strcmp("MORE2", script1)))
				{
					tmp=str2num(script2);
					items[c].moreb1=(unsigned char)(tmp>>24);
					items[c].moreb2=(unsigned char)(tmp>>16);
					items[c].moreb3=(unsigned char)(tmp>>8);
					items[c].moreb4=(unsigned char)(tmp%256);
				}
				else if (!(strcmp("MOREX",script1))) items[c].morex=str2num(script2);
				else if (!(strcmp("MOREY",script1))) items[c].morey=str2num(script2);
				else if (!(strcmp("MOREZ",script1))) items[c].morez=str2num(script2);
				break;
			case 'n':
			case 'N':
				if (!(strcmp("NAME",script1))) strcpy(items[c].name, script2);
				else if (!(strcmp("NAME2",script1))) strcpy(items[c].name2, script2);
				else if (!(strcmp("NEWBIE",script1))) items[c].priv=items[c].priv|0x02;
				break;
			case 'o':
			case 'O':
				if (!(strcmp("OFFSPELL",script1))) items[c].offspell=str2num(script2);
				break;
			case 'p':
			case 'P':
				if (!(strcmp("POISONED",script1))) items[c].poisoned=str2num(script2);
				else if (!(strcmp(script1, "PILEABLE"))) items[c].pileable=str2num(script2); 
				else if (!(strcmp(script1, "PRIV"))) items[c].priv=str2num(script2); 
				break;
			case 'r':
			case 'R':
				if (!(strcmp("RANK", script1)))
				{
					items[c].rank = str2num( script2 ); // By Magius(CHE)
					if( items[c].rank <= 0 ) items[c].rank = 10;
				}
				else if (!(strcmp("RACE",script1))) items[c].racialEffect = str2num( script2 );
				else if (!(strcmp("RESTOCK",script1))) items[c].restock=str2num(script2);
				break;
			case 's':
			case 'S':
				if( !(strcmp( "SK_MADE", script1 ))) items[c].madewith = str2num( script2 ); // by Magius(CHE)
				else if (!(strcmp("SPD",script1))) items[c].spd=str2num(script2);
				else if (!(strcmp("STR", script1))) items[c].st=str2num(script2);
				else if (!(strcmp("STRADD", script1))) items[c].st2=str2num(script2);
				else if (!(strcmp(script1, "ST"))) items[c].st=str2num(script2); 
				else if (!(strcmp(script1, "ST2"))) items[c].st2=str2num(script2);
				break;
			case 't':
			case 'T':
				if (!(strcmp("TYPE",script1))) items[c].type=str2num(script2);
				else if (!(strcmp("TRIGGER",script1))) items[c].trigger=str2num(script2);
				else if (!(strcmp("TRIGTYPE",script1))) items[c].trigtype=str2num(script2);
				else if (!(strcmp(script1, "TYPE2"))) items[c].type2=str2num(script2);
				break;
			case 'u':
			case 'U':
				if (!(strcmp("USES",script1))) items[c].tuses=str2num(script2);
				break;
			case 'v':
			case 'V':
				if (!(strcmp("VISIBLE",script1))) items[c].visible=str2num(script2);
				else if (!(strcmp("VALUE",script1))) items[c].value=str2num(script2);
				break;
			case 'w':
			case 'W':
				if (!(strcmp("WEIGHT",script1)))
				{
					anum=3;
					//anum=4;
					anum=str2num(script2); // Ison 2-20-99
					items[c].weight=anum;
				}
				else if (!(strcmp(script1, "WIPE"))) items[c].wipe=str2num(script2); 
				break;
			default:	break;
			}
		}
	}
	while (script1[0]!='}');

	closescript();
	if( c == -1 )
		return -1;
	Map->SeekTile( ( items[c].id1<<8 ) + items[c].id2, &tile );
	if( tile.flag2&0x08 ) 
		items[c].pileable = 1;
	if( c == itemcount ) 
		itemcount++;
	itemcount2++;
	if( !items[c].maxhp && items[c].hp ) 
		items[c].maxhp = items[c].hp;

	mapRegions->RemoveItem( c );
	if( items[c].contserial==-1 ) 
		mapRegions->AddItem( c );
	return theitem;

	
}

int cItem::CreateRandomItem(char * sItemList)//NEW FUNCTION -- 24/6/99 -- AntiChrist merging codes
{
	int i=0, Items[256];  //-- no more than 256 items in a single item list
	char sect[512];
	openscript("items.scp");
	sprintf(sect, "ITEMLIST %s", sItemList);
	if (!i_scripts[items_script]->find(sect)) // -- Valid itemlist?
	{
		closescript();
		if (n_scripts[custom_item_script][0]!=0)
		{
			openscript(n_scripts[custom_item_script]);
			if (!i_scripts[custom_item_script]->find(sect))
			{
				closescript();
				return -1;
			}
		} else return -1;
	}

	do  // -- count items storing item #'s in Items[]
	{
		read1();
		if (script1[0]!='}')
		{
			Items[i]=str2num(script1);
			i++;
		}
	}
	while (script1[0]!='}');
	closescript();

	if( i == 0 ) 
		return Items[0];
	else
		return Items[rand()%i];
}

int cItem::CreateScriptRandomItem(int s, char * sItemList)
{
	int i=0, Items[512], k;  //-- no more than 512 items in a single item list (changed by Magius(CHE))
	char sect[512];
	openscript("items.scp");
	sprintf(sect, "ITEMLIST %s", sItemList);
	if (!i_scripts[items_script]->find(sect)) // -- Valid itemlist?
	{
		closescript();
		if (n_scripts[custom_item_script][0]!=0)
		{
			openscript(n_scripts[custom_item_script]);
			if (!i_scripts[custom_item_script]->find(sect))
			{
				closescript();
				return -1;
			}
		} else return -1;
	}

	do  // -- count items storing item #'s in Items[]
	{
		read1();
		if (script1[0]!='}')
		{
			Items[i]=str2num(script1);
			i++;
		}
	}
	while (script1[0]!='}');
	closescript();

	if( i != 0 ) i=rand()%(i);
	k=Items[i];   // -- Get random Item #

	if (k!=0)
	{
		i=CreateScriptItem(s, k, 1);  // -- Create Item
	}
	return i;
}

//added by genesis 11-4-98 modified by Myth 11/12/98 fix for dupe problem in pack.
//int SpawnItemBackpack(int s, int nAmount, char* cName, int nStackable,
//        char cItemId1, char cItemId2, char cColorId1, char cColorId2)

// Spawn item- Will create an item based on item ID1 and ID2. If nPack is 1 it will
//             put the item in nSocket's pack (auto-stacking). If nSend is 1 it will
//             senditem() to all online characters. If you want to make specific changes
//             to the item before sending the information pass nSend as 0.
int cItem::SpawnItem(UOXSOCKET nSocket, int nAmount, char* cName, int nStackable,
        unsigned char cItemId1, unsigned char cItemId2, unsigned char cColorId1, unsigned char cColorId2, int nPack, int nSend)
{
	/*
	** OK, here's the scoop, in a few key places we were calling add item when there was
	** flat out no way to get a socket, say if a region spawner made an item, there is no
	** associated socket.  This was causing the socket to be -1, and it was indexing into
	** curchar[-1] and blowing up.  So I made a new version that takes a socket and a
	** a character (the following function) if you want to call that one with socket = -1
	** thats fine, because you have to pass the character in as well.  If you call this
	** function (which was the original) you are not allowed to pass in -1 for the socket
	** because it will crash. I've put the assert() in its place, so at least when it does
	** crash you'll know why. If this assert() ever happens to you, it means you need to
	** fix the CALLER of this function to pass the character in and call the other function
	** instead. Taking the assert() out won't help, its just liable to crash on currchar[-1]
	** anyways.  - fur
	*/
	
//	assert(nSocket >= 0);
	if( nSocket < 0 )
	{
		ConOut( "ERROR: Crash being avoided, valid socket needs to be passed to SpawnItem" );
		return -1;
	}
	CHARACTER ch = currchar[nSocket];
	return SpawnItem(nSocket, ch, nAmount, cName, nStackable, cItemId1, cItemId2, cColorId1, cColorId2, nPack, nSend);
}

int cItem::SpawnItem(UOXSOCKET nSocket, CHARACTER ch, int nAmount, char* cName, int nStackable,
					 unsigned char cItemId1, unsigned char cItemId2, unsigned char cColorId1, unsigned char cColorId2, int nPack, int nSend)
{
	if( nAmount == 0 )
		return -1;
	int inpack = 0, c = -1, i = -1;
	// yikes! what if nSocket is -1???
	int p=packitem(ch); //new added by Myth 11/12/98
	
	//Auto-Stack code!
	if (nPack && nStackable==1 && p!=-1)
	{
		for (int ptr=0;ptr<contsp[items[p].serial%HASHMAX].max;ptr++)
		{
			if ( ptr > -1 )
			{
				i = contsp[items[p].serial%HASHMAX].pointer[ptr];
				if ((i > -1) && (items[i].id1==cItemId1) && (items[i].id2==cItemId2) &&
					(items[i].color1==cColorId1 && items[i].color2==cColorId2))//)
				{
					if (items[i].contserial==items[p].serial)
					{
						items[i].amount=items[i].amount+nAmount;
						inpack = 1;
						RefreshItem( i ); // AntiChrist
						break;
					}
				}
			}
		}
	}
	
	if(!nPack || (nPack && !inpack))
	{
		c=MemItemFree();
		InitItem(c);
		if (c==-1) return -1;
		i=c;
		if(cName!=NULL) strcpy(items[c].name,cName);
		items[c].id1=cItemId1;
		items[c].id2=cItemId2;
		if (nPack && p!=-1) 
		{
			setserial(c, p, 1);
		}
		items[c].color1=cColorId1;
		items[c].color2=cColorId2;
		if (nPack)
		{
			items[c].x=(50+rand()%80);
			items[c].y=(50+rand()%80);
			items[c].z=9;
			
			if (p==-1)
			{
				mapRegions->RemoveItem(c);
				items[c].x=chars[ch].x;  // LB place it at players feet if he hasnt got backpack
				items[c].y=chars[ch].y; 
				items[c].z=chars[ch].z;          
				
				mapRegions->AddItem(c);
			}
		}
		items[c].amount=nAmount;
		items[c].pileable=nStackable;
		items[c].att=5;
		items[c].priv |= 0x01;
		// yikes, what if nSocket is -1?
		GetScriptItemSetting( c ); // Added by Magius(CHE)
		chars[ch].making=c;
		//		if (nSend) for(m=0;m<now;m++) if(perm[m]) senditem(m,c); 
		RefreshItem( c );
		if(c==itemcount) itemcount++;
		itemcount2++;
	}
	
	if (nSend && nSocket != -1) statwindow(nSocket,ch);
	return i;
}

void cItem::GetScriptItemSetting(int c)
{// by Magius(CHE)
	int tmp, sectfound = 0;
	char buff[512];
	char tscript1[512];
	char tscript2[512];
	if( c == -1 )
		return;

	strcpy( tscript1, script1 ); // AntiChrist
	strcpy( tscript2, script2 );
    openscript("harditems.scp");
	buff[0] = '\0'; // Fix by Magius(CHE)
	sprintf(buff,"x%x%x",items[c].id1,items[c].id2);
	int anum=3;
	do
	{
		read2();

		if (script1[0]!='}')
		{
			if( !(strcmp("SECTION", script1 ) ) ) if( strstr( script2, buff)) sectfound = 1; // AntiChrist bugfix

			if( sectfound ) // AntiChrist bugfix
			{
				switch( script1[0] )
				{
				case 'a':
				case 'A':
					if (!(strcmp("AMOUNT",script1))) items[c].amount=str2num(script2);
					else if (!(strcmp("AC", script1))) items[c].armorClass = str2num( script2 );
					break;
				case 'c':
				case 'C':
					if (!(strcmp("CREATOR", script1))) strcpy(items[c].creator, script2); // by Magius(CHE)
					else if (!(strcmp(script1, "CORPSE"))) items[c].corpse=str2num(script2);
					else if (!(strcmp("COLOR",script1)))
					{
						tmp=hstr2num(script2);
						items[c].color1=(unsigned char)(tmp>>8);
						items[c].color2=(unsigned char)(tmp%256);
					}
					break;
				case 'd':
				case 'D':
					if ((!(strcmp("DAMAGE",script1)))||(!(strcmp("ATT",script1)))) 
						items[c].att = getstatskillvalue(script2);
					else if (!(strcmp("DEF",script1))) items[c].def=str2num(script2);
					else if (!(strcmp("DEX", script1))) items[c].dx=str2num(script2);
					else if (!(strcmp("DEXADD", script1))) items[c].dx2=str2num(script2);
					else if (!(strcmp("DX", script1))) items[c].dx=str2num(script2);
					else if (!(strcmp("DX2", script1))) items[c].dx2=str2num(script2);
					else if (!(strcmp("DYE",script1))) items[c].dye=str2num(script2);
					else if (!(strcmp("DIR",script1))) items[c].dir=str2num(script2);
					else if (!(strcmp("DECAY",script1))) items[c].priv=items[c].priv|0x01;
					else if (!(strcmp("DISPELLABLE",script1))) items[c].priv=items[c].priv|0x04;
					else if (!(strcmp("DISABLED",script1))) items[c].disabled=str2num(script2);
					else if (!(strcmp(script1, "DYEABLE"))) items[c].dye=str2num(script2);
					else if (!(strcmp(script1, "DOORFLAG"))) items[c].doordir=str2num(script2); 
					break;
				case 'g':
				case 'G':
					if (!(strcmp("GOOD",script1))) items[c].good=str2num(script2); // Added by Magius(CHE)
					else if( !(strcmp(script1, "GLOW"))) items[c].glow = str2num( script2 ); 
					else if( !(strcmp(script1, "GLOWBC")))
					{
						unsigned int i = str2num( script2 );
						items[c].glow_c1 = (unsigned char)(i>>8);
						items[c].glow_c2 = (unsigned char)(i%256);
					}
					else if( !(strcmp(script1, "GLOWTYPE"))) items[c].glow_effect = str2num( script2 ); 
					break;
				case 'h':
				case 'H':
					if (!(strcmp("HP", script1))) items[c].hp=str2num(script2);
					else if (!(strcmp("HIDAMAGE", script1))) items[c].hidamage=str2num(script2);
					break;
				case 'i':
				case 'I':
					if (!(strcmp("INT", script1))) items[c].in=str2num(script2);
					else if (!(strcmp(script1, "ID")))
					{
						unsigned int i=str2num(script2);
						items[c].id1=(unsigned char)(i>>8);
						items[c].id2=(unsigned char)(i%256);
					}
					else if (!(strcmp("INTADD", script1))) items[c].in2=str2num(script2);
					else if (!(strcmp("IN", script1))) items[c].in=str2num(script2);
					else if (!(strcmp("IN2", script1))) items[c].in2=str2num(script2);
					else if (!(strcmp("ITEMHAND",script1))) items[c].itmhand=str2num(script2);
					break;
				case 'l':
				case 'L':
					if (!(strcmp("LODAMAGE", script1))) items[c].lodamage=str2num(script2);
					else if (!(strcmp("LAYER",script1))) items[c].layer=str2num(script2);
					break;
				case 'm':
				case 'M':
					if (!(strcmp("MAXHP", script1))) items[c].maxhp=str2num(script2); // by Magius(CHE)
					else if (!(strcmp("MOVABLE",script1))) items[c].magic=str2num(script2);
					else if (!(strcmp("MORE", script1)))
					{
						tmp=str2num(script2);
						items[c].more1 = (unsigned char)(tmp>>24);
						items[c].more2 = (unsigned char)(tmp>>16);
						items[c].more3 = (unsigned char)(tmp>>8);
						items[c].more4 = (unsigned char)(tmp%256);
					}
					//MORE2 may not be useful ?
					else if (!(strcmp("MORE2", script1)))
					{
						tmp=str2num(script2);
						items[c].moreb1 = (unsigned char)(tmp>>24);
						items[c].moreb2 = (unsigned char)(tmp>>16);
						items[c].moreb3 = (unsigned char)(tmp>>8);
						items[c].moreb4 = (unsigned char)(tmp%256);
					}
					else if (!(strcmp("MOREX",script1))) items[c].morex=str2num(script2);
					else if (!(strcmp("MOREY",script1))) items[c].morey=str2num(script2);
					else if (!(strcmp("MOREZ",script1))) items[c].morez=str2num(script2);
					break;
				case 'n':
				case 'N':
					if (!(strcmp("NAME",script1))) strcpy(items[c].name, script2);
					else if (!(strcmp("NAME2",script1))) strcpy(items[c].name2, script2);
					else if (!(strcmp("NEWBIE",script1))) items[c].priv=items[c].priv|0x02;
					break;
				case 'o':
				case 'O':
					if (!(strcmp("OFFSPELL",script1))) items[c].offspell=str2num(script2);
					break;
				case 'p':
				case 'P':
					if (!(strcmp("POISONED",script1))) items[c].poisoned=str2num(script2);
					else if (!(strcmp(script1, "PILEABLE"))) items[c].pileable=str2num(script2); 
					else if (!(strcmp(script1, "PRIV"))) items[c].priv=str2num(script2); 
					break;
				case 'r':
				case 'R':
					if (!(strcmp(script1, "RANK"))) {
						items[c].rank=str2num(script2); // By Magius(CHE)
						if( items[c].rank == 0 ) 
							items[c].rank = 10;
					}
					else if (!(strcmp("RESTOCK",script1))) items[c].restock=str2num(script2);
					else if (!(strcmp("RACE",script1))) items[c].racialEffect=str2num(script2); // Added by Magius(CHE)
					break;
				case 's':
				case 'S':
					if (!(strcmp("SK_MADE", script1))) items[c].madewith=str2num(script2); // by Magius(CHE)
					else if (!(strcmp("STR", script1))) items[c].st=str2num(script2);
					else if (!(strcmp("STRADD", script1))) items[c].st2=str2num(script2);
					else if (!(strcmp("SPD",script1))) items[c].spd=str2num(script2);
					else if (!(strcmp("ST", script1))) items[c].st=str2num(script2);
					else if (!(strcmp("ST2", script1))) items[c].st2=str2num(script2);
					break;
				case 't':
				case 'T':
					if (!(strcmp("TYPE",script1))) items[c].type=str2num(script2);
					else if (!(strcmp("TYPE2",script1))) items[c].type2=str2num(script2);
					else if (!(strcmp("TRIGGER",script1))) items[c].trigger=str2num(script2);
					else if (!(strcmp("TRIGTYPE",script1))) items[c].trigtype=str2num(script2);
					break;
				case 'u':
				case 'U':
					if (!(strcmp("USES",script1))) items[c].tuses=str2num(script2);
					break;
				case 'v':
				case 'V':
					if (!(strcmp("VISIBLE",script1))) items[c].visible=str2num(script2);
					else if (!(strcmp("VALUE",script1))) items[c].value=str2num(script2);
					break;
				case 'w':
				case 'W':
					if (!(strcmp("WEIGHT",script1)))
					{
						anum=4;
						anum=str2num(script2); // Ison 2-20-99
						items[c].weight=anum;
					}
				default:	break;
				}
			}
		}
		if( script1[0]=='}' && sectfound ) break;
	}
    while (strcmp( script1, "EOF" ));
    closescript();
	strcpy( script1, tscript1 ); // AntiChrist
	strcpy( script2, tscript2 );
}


int cItem::SpawnItemBackpack2(UOXSOCKET s, CHARACTER ch, int nItem, int nDigging) // Added by Genesis 11-5-98
{
	char sect[512];
	int c, nTheItem,p, pos;

	openscript("items.scp");
	sprintf(sect, "ITEM %i", nItem);
	if(!i_scripts[items_script]->find(sect))
	{
		closescript();
		if (n_scripts[custom_item_script][0]!=0)
		{
			openscript(n_scripts[custom_item_script]);
			if (!i_scripts[custom_item_script]->find(sect))
			{
				closescript();
				return -1;
			}
			else strcpy(sect, n_scripts[custom_item_script]);
		} else return -1;
	} else strcpy(sect, "items.scp");

	p=packitem(ch);
	pos=ftell(scpfile); /* lord binary */
	closescript();             
	if (p==-1) return -1;
	nTheItem=c=CreateScriptItem(s, nItem, 1);
	if( c == -1 )
		return -1;
	openscript(sect);
	fseek(scpfile, pos, SEEK_SET);
	setserial(c, p, 1); 
	items[c].x=(50+rand()%80);
	items[c].y=(50+rand()%80);
	items[c].z=9;
	items[c].magic=1;

	if(nDigging) 
	{
		if (items[c].value!=0) items[c].value=1+(rand()%(items[c].value));
		if (items[c].hp!=0) items[c].hp=1+(rand()%(items[c].hp));
	}
	pos=ftell( scpfile );
	closescript(); // MUST close script before calling GetScriptItem() because it will open its own script - fur
	GetScriptItemSetting( c ); // Added by Magius(CHE) (2)
	openscript( sect );
	fseek( scpfile, pos, SEEK_SET );
//	for(unsigned int m=0;m<now;m++) if(perm[m]) senditem(m,c); 
	RefreshItem( c );
	statwindow(s,currchar[s]);
	closescript();
	return nTheItem;
}

char cItem::isFieldSpellItem(int i) //LB
{
  char a = 0;

  if (items[i].id1==0x39 && (items[i].id2==0x96 || items[i].id2==0x8C)) a=1; // fire field
  if (items[i].id1==0x39 && (items[i].id2==0x15 || items[i].id2==0x20)) a=2; // poison field
  if (items[i].id1==0x39 && (items[i].id2==0x79 || items[i].id2==0x67)) a=3; // paralyse field
  if (items[i].id1==0x39 && (items[i].id2==0x56 || items[i].id2==0x46)) a=4; // energy field;

  return a;

}

//NEW DECAYITEM ZIPPY FUNCTION -- AntiChrist merging codes -- (24/6/99)
void cItem::DecayItem(unsigned int currenttime, int i) 
{
	int j,/*k,*/serial,serhash,ci,multi, preservebody;
	//if (nextdecaytime<=currenttime||(overflow))
	if( items[i].decaytime <= currenttime || (overflow) )//fixed by JustMichael
	{
		if (items[i].priv&0x01 && items[i].contserial==-1 && items[i].free==0)
		{  // decaytime = 5 minutes, * 60 secs per min, * clocks_per_sec
			if (items[i].decaytime==0) 
			{
				items[i].decaytime = (unsigned int)( server_data.decaytimer * CLOCKS_PER_SEC + currenttime );
			}
			
			if (items[i].decaytime<=currenttime)
			{
                //Multis --Boats ->

				if (!isFieldSpellItem(i)) // Gives fieldspells a chance to decay in multis, LB
				{
				  if (items[i].multis<1 && !items[i].corpse)
				  {
					// JustMichael -- Added a check to see if item is in a house
					multi=findmulti(items[i].x, items[i].y, items[i].z);
					if (multi!=-1)
					{
						if( items[multi].more4==0) //JustMichael -- set more to 1 and stuff can decay in the building
						{
							items[i].decaytime = (server_data.decaytimer * CLOCKS_PER_SEC + currenttime);
							setserial(i,multi,7);
							return;
						}
					}
				} 
				  else if (items[i].multis>0 && !items[i].corpse) 
				{					
					  items[i].decaytime = (server_data.decaytimer * CLOCKS_PER_SEC + currenttime);
					  return;
				}
				}
				//End Boats/Mutlis
				//JustMichael--Keep player's corpse as long as it has more than 1 item on it
				//up to playercorpsedecaymultiplier times the decay rate
				if( items[i].corpse == 1 && items[i].ownserial != -1 )
				{
					preservebody=0;
					serial=items[i].serial;
					serhash=serial%HASHMAX;
					for( ci=0; ci < contsp[serhash].max; ci++ )
					{
						j = contsp[serhash].pointer[ci];
						if( j != -1 )
						{
							if( items[j].contserial == serial )	// hash container issue *sighs*  Potentially premature decay based on items in same hash line
								preservebody++;
						}
					}
					if( preservebody > 1 && items[i].more4 )
					{
						items[i].more4--;
						items[i].decaytime = server_data.decaytimer * CLOCKS_PER_SEC + currenttime;
						return;
					}
				}
				if( (items[i].type == 1 && items[i].corpse != 1 ) || (items[i].ownserial != -1 && items[i].corpse) || (!server_data.lootdecayswithcorpse && items[i].corpse ))
				{
					serial=items[i].serial;
					serhash=serial%HASHMAX;
					for (ci=0;ci<contsp[serhash].max;ci++)
					{
						j=contsp[serhash].pointer[ci];
						if( j != -1 )
						{
							if( items[j].contserial == items[i].serial )
							{
								if( items[j].contserial != -1 ) 
									removefromptr(&contsp[items[j].contserial%HASHMAX], j);
								items[j].cont1 = 255;
								items[j].cont2 = 255;
								items[j].cont3 = 255;
								items[j].cont4 = 255;
								items[j].contserial = -1;
								mapRegions->RemoveItem( j );
								items[j].x = items[i].x;
								items[j].y = items[i].y;
								items[j].z = items[i].z;
								mapRegions->AddItem( j ); //add this item to a map cell

								items[j].decaytime = (unsigned int)( uiCurrentTime + (server_data.decaytimer*CLOCKS_PER_SEC ) ); // AntiChrist - make the item decay
								RefreshItem( j ); // AntiChrist
							}
						}
					}
					DeleItem(i);
				} 
				else 
				{
					if( items[i].contserial == -1 )
					{
						DeleItem(i);
					}
					else
					{
						items[i].decaytime = (unsigned int)(server_data.decaytimer*CLOCKS_PER_SEC + currenttime);
					}
				}
			}
		}
	}
}

//END FUNCTION

//NEW RESPAWNITEM FUNCTION STARTS HERE -- AntiChrist merging codes -- (24/6/99)

void cItem::RespawnItem(unsigned int currenttime, int i)
{
	int  j, k,m,serial,serhash,ci, c;
	if( i == -1 )
		return;
	if (items[i].free) return;
	switch( items[i].type )
	{
	case 61:
	case 62:
	case 63:
	case 64:
	case 65:
	case 69:
	case 125:
		break;
	default:
		return;
	}

	for(c=0;c<items[i].amount;c++)
	{
		if(items[i].gatetime+(c*items[i].morez*CLOCKS_PER_SEC)<=currenttime)// && chars[i].hp<=chars[i].st)
		{
			if ((items[i].disabled!=0)&&((items[i].disabled<=currenttime)||(overflow)))
			{
				items[i].disabled=0;
			}
			m=0;
			if (items[i].type==61)
			{
				k=0;
				serial=items[i].serial;
				serhash=serial%HASHMAX;
				for (j=0;j<spawnsp[serhash].max;j++)
				{
					ci=spawnsp[serhash].pointer[j];
					if( ci > -1 )
						if( items[i].serial==items[ci].spawnserial && ( items[ci].free == 0 ) )
					{
						if (i!=ci && items[ci].x==items[i].x && items[ci].y==items[i].y && items[ci].z==items[i].z)
						{
							k=1;
							break;
						}
					}
				}

				if (k==0)
				{
					if (items[i].gatetime==0)
					{
						items[i].gatetime = (unsigned int)( (rand()%((int)(1+((items[i].morez-items[i].morey)*(CLOCKS_PER_SEC*60))))) +
							(items[i].morey*CLOCKS_PER_SEC*60)+currenttime );
					}
					if ((items[i].gatetime<=currenttime ||(overflow)) && items[i].morex!=0)
					{
						AddRespawnItem( i, items[i].morex, 0 );
						items[i].gatetime=0;
					}
				}
			}
			if (items[i].type==62 || items[i].type==69 || items[i].type == 125 )
			{
				k=0;
				if (items[i].serial==1073763561)
				{
					k=0;
				}
				serial=items[i].serial;
				serhash=serial%HASHMAX;
				for (j=0;j<cspawnsp[serhash].max;j++)
				{
					ci=cspawnsp[serhash].pointer[j];
					if( ci > -1 )
						if( chars[ci].spawnserial == serial && chars[ci].free == 0 )
					{
						k++;
					}
				}

				if (k<items[i].amount)
				{
					if (items[i].gatetime==0)
					{
						items[i].gatetime = (unsigned int)((rand()%((int)(1+
							((items[i].morez-items[i].morey)*(CLOCKS_PER_SEC*60))))) +
							(items[i].morey*CLOCKS_PER_SEC*60)+currenttime);
					}
					if ((items[i].gatetime<=currenttime || (overflow)) && items[i].morex!=0)
					{
						Npcs->AddRespawnNPC(i,-1,items[i].morex,1);
						items[i].gatetime=0;
					}
				}
			}
			if ((items[i].type==63)||(items[i].type==64)||(items[i].type==65))
			{
				serial=items[i].serial;
				serhash=serial%HASHMAX;
				for (j=0;j<contsp[serhash].max;j++)
				{
					ci=contsp[serhash].pointer[j];
					if( ci > -1 )
					{
						if( items[ci].contserial == serial && items[ci].free == 0 )
							m++;
					}
				}
				if( (unsigned)m < items[i].amount )
				{
					if (items[i].gatetime==0)
					{
						items[i].gatetime = (unsigned int)((rand()%((int)(1+((items[i].morez-items[i].morey)*(CLOCKS_PER_SEC*60))))) +
							(items[i].morey*CLOCKS_PER_SEC*60)+uiCurrentTime );
					}
					if ((items[i].gatetime<=currenttime ||(overflow)) && items[i].morex!=0)
					{
						if( items[i].type == 63 )
						{
							items[i].type = 64; // Lock the container
						}
						AddRespawnItem(i,items[i].morex, 1); // If the item contains an item list then it will randomly choose one from the list, JM
						items[i].gatetime=0;
					}
				}
			}
		}//If time
	}//for 
}

// author: LB purpose: returns the type of pack
// to handle its x,y coord system correctly.
// interpretation of the result:
// valid x,y ranges depending on type:
// type -1 : no pack
// type 1  : y-range 50  .. 100
// type 2  : y-range 30  .. 80
// type 3  : y-range 100 .. 150
// type 4  : y-range 40  .. 140
// x-range 18 .. 118 for 1,2,3
//         40 .. 140 for 4
// modified by abaddon in the case statements to be more readable and flexible

unsigned char cItem::PackType( unsigned char id1, unsigned char id2 )
{
	switch( id1 )
	{
	case 0x09:
		switch( id2 )
		{
		case 0xA8: 
		case 0xAA:
		case 0xB0:	return 1;
		case 0xA9:	return 2;
		case 0xAB:	return 3;
		case 0xB2:	return 4;
		default:	return 0xFF;
		}
	case 0x0E:
		switch( id2 )
		{
		case 0x3C:
		case 0x3D:
		case 0x3E:
		case 0x3F:
		case 0x7E:
		case 0x78:	return 2;
		case 0x76:
		case 0x79:
		case 0x7D:
		case 0x80:
		case 0x7A:	return 1;
		case 0x40:
		case 0x41:
		case 0x42:
		case 0x7C:
		case 0x43:	return 3;
		case 0x75:
		case 0x7F:
		case 0x83:
		case 0x77:	return 4;
		default:	return 0xFF;
		}
	default:	return 0xFF;
	}
	return 0xFF;		// safety catch all (avoids compiler warnings)
}

// NEW RESPAWNITEM FUNCTION ENDS HERE -- AntiChrist merging codes --
void cItem::AddRespawnItem(int s, int x, int y)
{
	int c, /*j,*/ k;
	c=CreateScriptItem( -1, x, 1 ); // lb, bugfix


	if( c == -1 ) return;
  
	if( y <= 0 )
	{
		items[c].x=items[s].x;
		items[c].y=items[s].y;
		items[c].z=items[s].z;
	    mapRegions->RemoveItem(c);
		mapRegions->AddItem(c); //add spawned item to map cell if not in a container
	} else
	{
		setserial( c, s, 1 );
	}
	setserial(c,s,2);
	
	//** Lb bugfix for spawning in wrong pack positions **//
	if( y > 0 )
	{
		int z = -1;
		if( items[c].spawnserial != -1 && items[c].spawnserial != 0 )	// legacy support until next build
			z = calcItemFromSer( items[c].spawnserial );
		if( z != -1 )
		{
			k = PackType( items[z].id1, items[z].id2 );
			if( k == -1 )
			{
				ConOut( "Warning: A non-container item was set as container spawner \n" );
				k = 1;
			}
			items[c].x = (rand()%100) + 18;
			items[c].z = 9;
			switch( k )
			{
			case 1:	items[c].y = (short int)( rand()%50 ) + 50; break;
			case 2:	items[c].y = (short int)( rand()%50 ) + 30; break;
			case 3:	items[c].y = (short int)( rand()%40 ) + 100; break;
			case 4:	items[c].y = (short int)( rand()%80 ) + 60; // bugfix
					items[c].x = (short int)( rand()%80 ) + 60; break;
			default:	
					items[c].y = (short int)( rand()%50 ) + 30;
			}
		}
	}
	//  if (x) for (j=0;j<now;j++) if (perm[j]) senditem(j,c);
	RefreshItem( c ); // AntiChrist
}

void cItem::AddRandomItem(int s, char * itemlist, int spawnpoint) //Tauriel will be removed
{
	//This function gets the random npc number from the list and recalls
	//addrespawnnpc passing the new number
	char sect[512];
	int i=0,j=0,k=0;
	openscript("items.scp");
	sprintf(sect, "ITEMLIST %s", itemlist);
	if (!i_scripts[items_script]->find(sect))
	{
		closescript();
		if (n_scripts[custom_item_script][0]!=0)
		{
			openscript(n_scripts[custom_item_script]);
			if (!i_scripts[custom_item_script]->find(sect))
			{
				closescript();
				return;
			}	
		} else return;
	}
	do
	{
		read1();
		if (script1[0]!='}')
		{
			i++;
		}
	}
	while (script1[0]!='}');
	closescript();
	if(i>0)
	{
		i=rand()%(i);
		openscript("items.scp");
		if(!i_scripts[items_script]->find(sect))
		{
			closescript();
			if (n_scripts[custom_item_script][0]!=0)
			{
				openscript(n_scripts[custom_item_script]);
				if (!i_scripts[custom_item_script]->find(sect))
				{
					closescript();
					return;
				}
			} else return;
		}
		do
		{
			read1();
			if (script1[0]!='}')
			{
				if(j==i)
				{
					k=str2num(script1);
					j++;
				}
				else j++;
			}
		}
		while (script1[0]!='}');
		closescript();
	}
	if(k!=0)
	{
		if (spawnpoint==-1)
		{
			addmitem[s]=k;
			Targ->AddMenuTarget(s, 0, addmitem[s]);
		}
		else
		{
			AddRespawnItem(spawnpoint,k,1);
		}
	}
}


ARMORCLASS cItem::ArmorClass( ITEM i )
{
	return items[i].armorClass;
}

void cItem::GlowItem( UOXSOCKET s, int i )
{
	int j,c;

	//ConOut("glow check called, char#: %i\n",s);
	if (items[i].glow>0)
	{
		c=items[i].glow;
		j = calcItemFromSer( c );

		//ConOut("contser: %i layer: %i serial: %i\n",items[i].contserial,items[i].layer,items[i].serial);

		if (j==-1) return;

		items[j].layer=items[i].layer; // copy layer information of the glowing item to the invisible light emitting object

		if(items[j].layer==0 && items[i].contserial==-1) // unequipped -> light source coords = item coords
		{
			items[j].dir=29;
			items[j].x=items[i].x;
			items[j].y=items[i].y;
			items[j].z=items[i].z;
		} else if (items[j].layer==0 && items[i].contserial!=-1) // euqipped -> light source coords = players coords
		{
			items[j].x = chars[s].x;
			items[j].y = chars[s].y;
			items[j].z = (signed char)chars[s].z+4;
			items[j].dir = 99; // gives no light in backpacks
		} else
		{
			items[j].x = chars[s].x;
			items[j].y = chars[s].y;
			items[j].z = (signed char)chars[s].z+4;
			items[j].dir = 29;

		}
		RefreshItem( j ); // AntiChrist
	}
}

void cItem::CheckEquipment(CHARACTER p) // check equipment of character p
{
	int i = -1, serial, serhash, ci;
	unsigned int j;
	if( p < 0 ) 
		return;

	serial = chars[p].serial;
	serhash = serial%HASHMAX;

	for( ci = 0; ci < contsp[serhash].max; ci++ )
	{
		i = contsp[serhash].pointer[ci];
		if( i != -1 ) //  crashfix
			if( items[i].contserial == serial )
			{
				if( items[i].st > chars[p].st ) //if strength required > character's strength
				{
					if( items[i].name[0] == '#' ) 
						getname( i, temp2 );
					else 
						strcpy( temp2, items[i].name );

					sprintf( temp, "You are not strong enough to keep %s equipped!", temp2 );
					UOXSOCKET pSock = calcSocketFromChar( p );
					sysmessage( pSock, temp );
					itemsfx( pSock, items[i].id1, items[i].id2 );
  					
					//Subtract stats bonus and poison
					chars[p].st -= items[i].st2;
					chars[p].dx -= items[i].dx2;
					chars[p].in -= items[i].in2;
               		if( chars[p].poison && items[i].poisoned ) 
						chars[p].poison -= items[i].poisoned;
					if( chars[p].poison < 0 ) 
						chars[p].poison = 0;

					// AntiChrist - this should be .contserial, not .serial       |THIS|
					//if (items[i].contserial!=-1) removefromptr(&contsp[items[i].serial%HASHMAX], i);
					if( items[i].contserial != -1 ) 
						removefromptr( &contsp[items[i].contserial%HASHMAX], i );
					items[i].cont1 = 255;
					items[i].cont2 = 255;
					items[i].cont3 = 255;
					items[i].cont4 = 255;
					items[i].contserial = -1;
					mapRegions->RemoveItem( i );
					items[i].x = chars[p].x;
					items[i].y = chars[p].y;
					items[i].z = chars[p].z;
					mapRegions->AddItem( i );
								
					for( j = 0; j < now; j++ )
						if( perm[j] && inrange1p( p, currchar[j] ) )
						{
							wornitems(j, p);
						}
					RefreshItem( i );
				}
			}	
	}		
}

//Put item in character's backpack - AntiChrist
void cItem::BounceInBackpack(CHARACTER p, ITEM i)
{
	if(p<0 || i<0) return;//it's more safe :)

	int newpack=packitem(p);//new character's backpack
	int s1=calcSocketFromChar(p);//new character's socket
	int newp=p;//new character
	
	if(items[i].contserial!=-1)
	{
		int serial=items[i].contserial;
		int serhash=serial%HASHMAX;
		int oldpack = calcItemFromSer( serial );//old container

		int oldp=GetPackOwner(oldpack);//this should now be the old char that owned the item

		if (items[i].glow>0) // LB's glowing items stuff
			removefromptr(&glowsp[chars[oldp].serial%HASHMAX],i);//remove from old char

		removefromptr(&contsp[serhash], i);//remove from old container (if one)
	}

	removeitem[1]=items[i].ser1;
	removeitem[2]=items[i].ser2;
	removeitem[3]=items[i].ser3;
	removeitem[4]=items[i].ser4;
				
	//AUTOSTACK code
	int ptr, stack,hash;
	hash=items[newpack].serial%HASHMAX;
	for(ptr=0;ptr<contsp[hash].max;ptr++)
	{
		stack=contsp[hash].pointer[ptr];
		if (stack!=-1)
		{
		if (items[stack].contserial==items[newpack].serial) // this is important zippy, or it wont work !!! LB
			// btw, this is the "true" bugfix for the gold-stacking bug, morolan :)
		{										
			
			if (items[stack].pileable && items[i].pileable && items[stack].serial!=items[i].serial &&
			(items[stack].id1==items[i].id1 && items[stack].id2==items[i].id2) && 
			( items[stack].color1 == items[i].color1 && items[stack].color2 == items[i].color2 ) )
			{//Time to stack.
				//ConOut("stacking %s %x %x\n",items[stack].name,items[stack].id1,items[stack].id2);
				if (items[stack].amount+items[i].amount>65535)
				{
					items[i].x=items[stack].x;
					items[i].y=items[stack].y;
					items[i].z=9;
					items[i].amount=(items[stack].amount+items[i].amount)-65535;
					items[stack].amount=65535;
					setserial(i, newpack,1); // add to container hash
					Network->xSend(s1, removeitem, 5, 0);
					//senditem(s1, i);
					RefreshItem(i);//AntiChrist
				} else 
				{
					items[stack].amount=items[stack].amount+items[i].amount;
					DeleItem(i);
				}
				Network->xSend(s1, removeitem, 5, 0);//Morrolan fix
				//senditem(s1, stack);
				RefreshItem(stack);//AntiChrist
				itemsfx(s1, items[stack].id1, items[stack].id2);
				Weight->NewCalc(p);
				statwindow(s1,p);
				//return;
				break;//break FOR cycle
			} // end if pillable
		}// if stack !=-1
		}
		else // no autostacking
		{ 
			setserial(i, newpack,1);      // no autostacking -> add it to container hash ! 
			items[i].x = (short int)20 + ( rand() % 100) ;// and set new random pack coords
			items[i].y = (short int)40 + ( rand() % 80 );
			items[i].z = 9;

			//LB GLOWING STUFF
			setptr(&glowsp[chars[newp].serial%HASHMAX],i);//add in new char
			GlowItem(newp,i);

			UOXSOCKET k;
			for( k = 0; k < now; k++ )
			{
				if( perm[k] )
				{
					Network->xSend( k, removeitem, 5, 0 );
				}
			} // end for k

			RefreshItem(i);//AntiChrist

			break;//break FOR cycle
		} // else
	} // end for

	Weight->NewCalc(p);
	statwindow(s1,p);
}

//Put item at character's feet - AntiChrist
void cItem::BounceItemOnGround(CHARACTER p, ITEM i)
{
	if(p<0 || i<0) return;

	if(items[i].contserial!=-1)
	{
		int serial=items[i].contserial;
		int serhash=serial%HASHMAX;
		int oldpack = calcItemFromSer( serial );//old container
	
		int oldp = GetPackOwner(oldpack);//this should now be the old char that owned the item
	
		if (items[i].glow>0) // LB's glowing items stuff
			removefromptr(&glowsp[chars[oldp].serial%HASHMAX],i);//remove glowing from old char

		removefromptr(&contsp[serhash], i);//remove from old container
	}

	//no container (ground)
	items[i].cont1=255;
	items[i].cont2=255;
	items[i].cont3=255;
	items[i].cont4=255;
	items[i].contserial=-1;

	mapRegions->RemoveItem(i);
	items[i].x=chars[p].x;
	items[i].y=chars[p].y;
	items[i].z=chars[p].z;
	mapRegions->AddItem(i);

    GlowItem(p,i); //LB's glowing item stuff

	//for (int k=0;k<now;k++) if (perm[k] && inrange2(k,i)) senditem(k,i);
	RefreshItem(i);//AntiChrist

	itemsfx(calcSocketFromChar(p),items[i].id1,items[i].id2);
}

bool cItem::isShieldType( ITEM i )
{
	unsigned short itemID = (unsigned short)(items[i].id1<<8) + items[i].id2;
	if( itemID >= 0x1B72 && itemID <= 0x1B7B ) 
		return true;
	if( itemID >= 0x1BC3 && itemID <= 0x1BC5 ) 
		return true;
//	tile_st toCheck;
//	Map->SeekTile( itemID, &toCheck );
//	if( toCheck.layer == 2 )
//	{
//		if( (toCheck.flag3 & 0xC0 ) )	// if equipable, and on layer 2
//			return true;
//	}
	return false;
}

bool cItem::isLeftHandType( ITEM i ) // Need to seperate this from ShieldType - Thunderstorm
{
	unsigned short itemID = (items[i].id1<<8) + items[i].id2;
	tile_st toCheck;
	Map->SeekTile( itemID, &toCheck );
	if( toCheck.layer == 2 )
	{
		// Need to seperate from two handed Weapons, so I am checking if the item has a hidamage value. Any better solutions here would be greatly appreciated :) - Thunderstorm
		if( (toCheck.flag3 & 0xC0) && items[i].hidamage == 0 )	// if equipable, and on layer 2, and doesn't deal damage
			return true;
	}
	return false;
}

// Written by AntiChrist - 4/11/1999
//
//This is very useful for bandwidth I think.... I replaced all the
//stupid loops in the entire code sending an item to ALL the
//sockets... without checkomg if the socket's character was in
//range.....very baaad!
//
//I also added the inpack check and the worned check....
//
//Added a check if invisible and not GM, not sent (Abaddon)
void RefreshItem( ITEM i ) //  Send this item to all online people in range
{ // check if item is in a pack or on the ground, then use different methods
	
	unsigned int a;
	
	if( i == -1 ) return; // just to be on the right side
	
	if( items[i].contserial == items[i].serial )
	{
		ConOut( "\nALERT ! item %s [serial: %i] has dangerous container value, autocorrecting\n", items[i].name, items[i].serial );
		items[i].contserial = -1;
	}
	
	// first check: let's check if it's on the ground....
	if( items[i].cont1 == 255 && items[i].cont2 == 255 && items[i].cont3 == 255 && items[i].cont4 == 255 )
	{ // yeah, it's on ground!
		for( a = 0; a < now; a++ ) // send this item to all the sockets in range
		{
			if( perm[a] && iteminrange( a, i, Races->getVisRange( chars[currchar[a]].race ) ) )
			{
				if( ( items[i].visible == 0 ) || ( ( items[i].visible == 1 || items[i].visible == 2 ) && ( chars[a].priv&0x01 ) ) )// we're a GM, or not hidden
					senditem( a, i );
			}
		}
		return;
	}
	
	// if not, let's check if it's on a char or in a pack
	
	int cserial = items[i].contserial;
	int iserial = items[i].contserial;
	int charcont = -1;
	int itemcont = -1;
	
	if( (unsigned char)(cserial>>24) < 0x40 )
		charcont = calcCharFromSer( cserial );
	else
		itemcont = calcItemFromSer( iserial );
	//		EviLDeD -		February 29, 2000
#ifdef DEBUG
	ConOut("DEBUG:Item=%i(%s) charcont=%i itemcont=%i\n", i, items[i].name, charcont, itemcont );
#endif
	//		EviLDeD -		End
	
	if( charcont > -1 ) // container is a player... it means it's equipped on a character!
	{
		wearitem[1] = items[i].ser1;
		wearitem[2] = items[i].ser2;
		wearitem[3] = items[i].ser3;
		wearitem[4] = items[i].ser4;
		wearitem[5] = items[i].id1;
		wearitem[6] = items[i].id2;
		wearitem[8] = items[i].layer;
		wearitem[9] = items[i].cont1;
		wearitem[10] = items[i].cont2;
		wearitem[11] = items[i].cont3;
		wearitem[12] = items[i].cont4;
		wearitem[13] = items[i].color1;
		wearitem[14] = items[i].color2;
		for( a = 0; a < now; a++ ) // send this item to all the sockets in range
		{
			if( perm[a] && inrange1p( currchar[a], charcont ) )
			{
				Network->xSend( a, wearitem, 15, 0 );
			}
		}
		return;
	}
	
	if( itemcont > -1 ) // container is an item... it means we have to use sendbpitem()!!
	{
		for( a = 0; a < now; a++ ) // send this item to all the sockets in range
		{
			if( perm[a] )
				sendbpitem( a, i ); // NOTE: there's already the inrange check
			// in the sendbpitem() function, so it's unuseful
			// to do a double check!!
		}
		return;
	}
	
	ConOut("Error in RefreshItem(%i): cannot determine container type!", i );
}
