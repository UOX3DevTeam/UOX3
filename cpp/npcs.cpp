//------------------------------------------------------------------------
//  npcs.cpp
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
#include <uox3.h>
#include <debug.h>

#define DBGFILE "npcs.cpp"

//Instance of cItemHandle class to handle item memory//

//o---------------------------------------------------------------------------o
//|	Class		:	cCharacterHandle::cCharacterHandle()
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
cCharacterHandle::cCharacterHandle( void )
{
	DefaultChar = new char_st;
	memset(DefaultChar, 0, sizeof(char_st));
	
	//setup the item's important properties (ones that might be checked if its used, and need to be this way
	DefaultChar->free = 1;
	DefaultChar->ser1 = DefaultChar->ser2 = DefaultChar->ser3 = DefaultChar->ser4 = 0xFF;
	DefaultChar->serial = 0;

	Acctual = Free = 0;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cCharacterHandle::~cCharacterHandle()
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
cCharacterHandle::~cCharacterHandle()
{
	unsigned long i;

	for (i=0;i<Chars.size();i++)//Memory Cleanup
	{
		if (Chars[i] != NULL)
			delete Chars[i];
	}

	Chars.clear();
	Chars.resize(0);
	
	FreeNums.clear();
	FreeNums.resize(0);

	delete DefaultChar;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cCharacterHandle::New()
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Free memory for a character, create the character in memory 
//|					(returns item number)
//o---------------------------------------------------------------------------o
unsigned long cCharacterHandle::New( void )
{
	unsigned long i;
	
	if ( Free > 0 )
	{
		i = FreeNums[FreeNums.size()-1];//get the oldest entry
		FreeNums.resize( FreeNums.size() - 1 ); //Delete it cause it ain't free no more.
		Free = max( Free-1, (unsigned int)0 );

		if (Chars[i] != NULL)
			delete Chars[i];

		Chars[i] = new char_st;
	} else {
		i = Chars.size();
		Chars.push_back( new char_st );
	}

	Acctual++;

	return i;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cCharacterHandle::Delete( long int )
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Free character memory (delete the char)
//o---------------------------------------------------------------------------o
void cCharacterHandle::Delete( long int Num )
{
	if ( Num > -1 && Num < Chars.size() )
	{
		if ( Chars[Num] != NULL  )
		{
			delete Chars[Num];
			Chars[Num] = NULL;

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
//|	Class		:	cCharacterHandle::Size()
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Get the size (in bytes) that characters are taking up
//o---------------------------------------------------------------------------o
unsigned long cCharacterHandle::Size( void )
{
	unsigned long sz;

	sz = Chars.size() * 4;
	sz += FreeNums.size() * sizeof(unsigned long);
	sz += sizeof(char_st) * Acctual;
	sz += sizeof(cCharacterHandle);

	return sz;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cCharacterHandle::Reserve( unsigned int )
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Reserve memory for Num number of characters (unused)
//o---------------------------------------------------------------------------o
void cCharacterHandle::Reserve( unsigned int Num )
{
	unsigned int i, cs = FreeNums.size(), is = Chars.size();
	
	Free+=Num;
	FreeNums.resize( cs + Num );
	Chars.resize( is+Num );

	for (i=cs;i<(cs+Num);i++)
	{// is + (cs+(Num-1))-i) is acctual number ( 0 to Num )
		FreeNums[i] = is + ((cs+(Num-1))-i);
		Chars[(is + (cs+(Num-1))-i)] = NULL;
	}
}

//o---------------------------------------------------------------------------o
//|	Class		:	cCharacterHandle::isFree( unsigned long)
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Check to see if item Num is marked free (unused (slow))
//o---------------------------------------------------------------------------o
bool cCharacterHandle::isFree( unsigned long Num )
{
	unsigned int i;
	for (i=0;i<FreeNums.size();i++)
	{
		if ( FreeNums[i] == Num )
			return true;
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cCharacterHandle::operator[]( long int )
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Reference character Num -  Check to make sure Num is a 
//|					valid character number and exists in memory
//o---------------------------------------------------------------------------o
char_st& cCharacterHandle::operator[] ( long int Num )
{
	if ( Num >= 0 && Num < Chars.size())
	{
		if ( Chars[Num] != NULL )//&& !isFree( Num ) ) //isFree isSLOW
			return *Chars[Num];
	} 
  else
  {
		printf("WARNING: Chars[%i] referenced is invalid. Crash averted!\n", Num);
  }  
  //Make sure these props are always this way, they may have been chaged by other functions, so put them back
	DefaultChar->free = 1;
	DefaultChar->ser1 = DefaultChar->ser2 = DefaultChar->ser3 = DefaultChar->ser4 = 0xFF;
	DefaultChar->serial = 0;
	DefaultChar->x = DefaultChar->y = 0;
	return *DefaultChar;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cCharacterHandle::Count()
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Return the number of characters in the world (Acctual)
//o---------------------------------------------------------------------------o
unsigned long cCharacterHandle::Count( void )
{
	return Acctual;
}
//End of cCharacterHandle class//



void cCharStuff::InitChar(int nChar, char ser)
{
	static unsigned int LastInitTE=0;
	int i;
	memset( &chars[nChar], 0, sizeof( char_st ) );
	if( ser )
	{
		chars[nChar].ser1 = (unsigned char)(charcount2>>24); // Character serial number
		chars[nChar].ser2 = (unsigned char)(charcount2>>16);
		chars[nChar].ser3 = (unsigned char)(charcount2>>8);
		chars[nChar].ser4 = (unsigned char)(charcount2%256);
		chars[nChar].serial=charcount2;
		setptr(&charsp[charcount2%HASHMAX], nChar);
		charcount2++;
	} 
	else 
	{
		chars[nChar].serial=-1;
	}
	if (nChar==charcount) charcount++;

    chars[nChar].multi1=255;//Multi serial1
	chars[nChar].multi2=255;//Multi serial2
	chars[nChar].multi3=255;//Multi serial3
	chars[nChar].multi4=255;//Multi serial4
	chars[nChar].multis=-1;//Multi serial
	strcpy(chars[nChar].name,"Mr. noname");

	chars[nChar].account=-1;
	chars[nChar].x=100;
	chars[nChar].y=100;

	chars[nChar].id1=chars[nChar].xid1=chars[nChar].orgid1=0x01; // Character body type
	chars[nChar].id2=chars[nChar].xid2=chars[nChar].orgid2=0x90; // Character body type
	chars[nChar].skin1=chars[nChar].xskin1=0x00; // Skin color
	chars[nChar].skin2=chars[nChar].xskin2=0x00; // Skin color
	chars[nChar].keynumb=-1;  // for renaming keys 
	chars[nChar].fonttype=3; // Speech font to use
	chars[nChar].saycolor1=0x17; // Color for say messages
	chars[nChar].emotecolor2=0x23; // Color for emote messages
	chars[nChar].st=50; // Strength
	chars[nChar].dx=50; // Dexterity
	chars[nChar].in=50; // Intelligence
	chars[nChar].hp=50; // Hitpoints
	chars[nChar].stm=50; // Stamina
	chars[nChar].mn=50; // Mana

	for (i=0;i<ALLSKILLS;i++)
		chars[nChar].atrophy[i] = i;

	chars[nChar].own1=255; // If Char is an NPC, this sets its owner
	chars[nChar].own2=255; // If Char is an NPC, this sets its owner
	chars[nChar].own3=255; // If Char is an NPC, this sets its owner
	chars[nChar].own4=255; // If Char is an NPC, this sets its owner
	chars[nChar].ownserial=-1; // If Char is an NPC, this sets its owner
	chars[nChar].robe1=255; // Serial number of generated death robe (If char is a ghost)
	chars[nChar].robe2=255; // Serial number of generated death robe (If char is a ghost)
	chars[nChar].robe3=255; // Serial number of generated death robe (If char is a ghost)
	chars[nChar].robe4=255; // Serial number of generated death robe (If char is a ghost)

	chars[nChar].packitem=-1; // Only used during character creation
	chars[nChar].fixedlight=255; // Fixed lighting level (For chars in dungeons, where they dont see the night)
	// was -1 thanks to LB, nice idea, but this is UNSIGNED, therefore 255

	chars[nChar].runs = false;  
	chars[nChar].targ=-1; // Current combat target

	chars[nChar].runenumb=-1; // Used for naming runes
	chars[nChar].attacker=-1; // Character who attacked this character

	chars[nChar].ftarg=-1; // NPC Follow Target
	chars[nChar].fx1=-1; //NPC Wander Point 1 x
	chars[nChar].fx2=-1; //NPC Wander Point 2 x
	chars[nChar].fy1=-1; //NPC Wander Point 1 y
	chars[nChar].fy2=-1; //NPC Wander Point 2 y
	chars[nChar].fz1=-1; //NPC Wander Point 1 z

	chars[nChar].hunger=6;  // Level of hungerness, 6 = full, 0 = "empty"
	chars[nChar].smeltitem=-1;
	chars[nChar].tailitem=-1;
	chars[nChar].callnum=-1; //GM Paging
	chars[nChar].playercallnum=-1; //GM Paging
	chars[nChar].region=(unsigned char)(255);
	chars[nChar].making=-1; // skill number of skill using to make item, 0 if not making anything.

	chars[nChar].town=(unsigned char)(255);       //Matches Region number in regions.scp
	chars[nChar].townvote1=255; //Serial Number of who they want to be mayor.
	chars[nChar].townvote2=255; //Serial Number of who they want to be mayor.
	chars[nChar].townvote3=255; //Serial Number of who they want to be mayor.
	chars[nChar].townvote4=255; //Serial Number of who they want to be mayor.

	chars[nChar].envokeitem=-1;

	chars[nChar].cantrain=1;

	chars[nChar].guildfealty=-1;		// Serial of player you are loyal to (default=yourself)	(DasRaetsel)

	chars[nChar].flag = 0x02; //1=red 2=grey 4=Blue 8=green 10=Orange

	chars[nChar].crimflag=-1; //Time when No longer criminal -1=Not Criminal
	chars[nChar].spellCast=-1; //current spell they are casting....
	chars[nChar].poisonserial=-1; //AntiChrist -- poisoning skill
	
	chars[nChar].stealth=-1; //AntiChrist - stealth ( steps already done, -1=not using )

	chars[nChar].swingtarg=-1; //Target they are going to hit after they swing

	chars[nChar].raceGate = 65535;
	chars[nChar].shopSpawn = -1;

	chars[nChar].tamed = false; // True if NPC is tamed
	chars[nChar].pathnum = PATHNUM;

	chars[nChar].guarded = false;	// True if CHAR is guarded by some NPC

	chars[nChar].carve = -1; // AntiChrist - for new carving system

	chars[nChar].postType = LOCALPOST;
}
void cCharStuff::DeleteChar (int k) // Delete character
{
	int j;//,serial; //Zippy lag
	//int ptr,ci;
	
	removeitem[1]=chars[k].ser1;
	removeitem[2]=chars[k].ser2;
	removeitem[3]=chars[k].ser3;
	removeitem[4]=chars[k].ser4;
//	if(chars[k].spawn1<=0x40 && chars[k].spawnserial!=-1)//Not spawned by item (Region spawn) //New -- Zippy spawn regions
	if( chars[k].spawn1 < 0x40 && chars[k].spawn2 == 1 )
		spawnregion[chars[k].spawn3].current--;
	
	j=removefromptr(&charsp[chars[k].serial%HASHMAX], k);
	
//	if (chars[k].spawnserial!=-1) j=removefromptr(&cspawnsp[chars[k].spawnserial%HASHMAX], k);
	if( chars[k].spawnserial != 0 && chars[k].spawnserial != -1 )	// legacy support until next build
		j = removefromptr( &cspawnsp[chars[k].spawnserial%HASHMAX], k );
	if (chars[k].ownserial!=-1) j=removefromptr(&cownsp[chars[k].ownserial%HASHMAX], k);
	
	for (j=0;j<now;j++)
	{
		Network->xSend(j, removeitem, 5, 0);
		//if (currchar[j]>k) currchar[j]--;
	}
	
	if (k>-1) mapRegions->RemoveItem(k+1000000); // taking it out of mapregions BEFORE x,y changed, LB
	
	chars[k].free=1;
	chars[k].x=20+(xcounter++);
	chars[k].y=50+(ycounter);
	chars[k].z=9;
	chars[k].summontimer=0;
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
	chars.Delete( k );
}

int cCharStuff::MemCharFree()
{
	char memerr=0;

	signed long nChar = chars.New();
	
	int slots=3000;
	if (charcount>=cmem) //Lets ASSUME theres no more memory left instead of acctually checking all the items to find a free one.
	{
		int *Copy_clickx;
		int *Copy_clicky;
		int *Copy_currentSpellType;
		char *Copy_targetok;

		// initialize to NULL so if an error has occurred we can properly
		// free them by checking for NULL
		Copy_clickx = NULL;
		Copy_clicky = NULL;
		Copy_currentSpellType = NULL;
		Copy_targetok = NULL;

		// create new objects to copy original data to
		if(( Copy_clickx = new int[cmem] ) == NULL ) memerr=1;
		else if(( Copy_clicky = new int[cmem] ) == NULL ) memerr=1;
		else if(( Copy_currentSpellType = new int[cmem] ) == NULL ) memerr=1;
		else if(( Copy_targetok = new char[cmem] ) == NULL ) memerr=1;

		// make sure nothing bad happened
		if (!memerr)
		{
			// make a copy - I prefer memcpy although a copy constructor wouldn't hurt =)
			memcpy(Copy_clickx, clickx, sizeof(int) * cmem);
			memcpy(Copy_clicky, clicky, sizeof(int) * cmem);
			memcpy(Copy_currentSpellType, currentSpellType, sizeof(int) * cmem);
			memcpy(Copy_targetok, targetok, sizeof(char) * cmem);

			// delete the old objects
			delete [] clickx;
			delete [] clicky;
			delete [] currentSpellType;
			delete [] targetok;

			// initialize to NULL so if an error has occurred we can properly
			// free them by checking for NULL
			clickx = NULL;
			clicky = NULL;
			currentSpellType = NULL;
			targetok = NULL;

			// create new objects with more room for more items, etc.
			if(( clickx = new int[cmem + slots] ) == NULL ) memerr=2;
			else if(( clicky = new int[cmem + slots] ) == NULL ) memerr=2;
			else if(( currentSpellType = new int[cmem + slots] ) == NULL ) memerr=2;
			else if(( targetok = new char[cmem + slots] ) == NULL ) memerr=2;

			if (!memerr)
			{
				// restore copy to new objects
				memcpy(clickx, Copy_clickx, sizeof(int) * cmem);
				memcpy(clicky, Copy_clicky, sizeof(int) * cmem);
				memcpy(currentSpellType, Copy_currentSpellType, sizeof(int) * cmem);
				memcpy(targetok, Copy_targetok, sizeof(char) * cmem);

				// delete copies
				delete [] Copy_clickx;
				delete [] Copy_clicky;
				delete [] Copy_currentSpellType;
				delete [] Copy_targetok;
			}
		}

		if (memerr)
		{
			// cleanup if neccessary
			if( memerr >= 1 )
			{
				if (Copy_clickx) delete [] Copy_clickx;
				if (Copy_clicky) delete [] Copy_clicky;
				if (Copy_currentSpellType) delete [] Copy_currentSpellType;
				if (Copy_targetok) delete [] Copy_targetok;

				// cleanup if neccessary
				if (memerr == 2)
				{
					if (clickx) delete [] clickx;
					if (clicky) delete [] clicky;
					if (currentSpellType) delete [] currentSpellType;
					if (targetok) delete [] targetok;
				}
			}

			printf("ERROR: Could not reallocate character memory after %i. No more characters will be created.\nWARNING: UOX may become unstable.\n", cmem);
		} else {
			// make sure everything has been cleared out
			memset(clickx + cmem, 0x00, sizeof(int) * slots);
			memset(clicky + cmem, 0x00, sizeof(int) * slots);
			memset(currentSpellType + cmem, 0x00, sizeof(int) * slots);
			memset(targetok + cmem, 0x00, sizeof(char) * slots);
			cmem+=slots;
		}
	}
	
	return nChar;
}


int cCharStuff::AddRandomLoot(int s, char * lootlist)
{
	char sect[512];
	int i,j,retitem, storeval;
	retitem=-1;
	storeval=-1;
	long int pos;
	i=0; j=0;
	openscript("npc.scp");
	sprintf(sect, "LOOTLIST %s", lootlist);
	if (!i_scripts[npc_script]->find(sect))
	{
		closescript();
		if (n_scripts[custom_npc_script][0]!=0)
		{
			openscript(n_scripts[custom_npc_script]);
			if (!i_scripts[custom_npc_script]->find(sect))
			{
				closescript();
				return -1;
			}
		} else return -1;
	}
	do
	{
		read1();
		if (script1[0]!='}')
		{
			i++; // Count number of entries on list.
		}
	} while (script1[0]!='}');
	
	closescript();
	if(i>0)
	{
		i=rand()%(i);
		openscript("npc.scp");
		if(!i_scripts[npc_script]->find(sect))
		{
			closescript();
			if (n_scripts[custom_npc_script][0]!=0)
			{
				openscript(n_scripts[custom_npc_script]);
				if (!i_scripts[custom_npc_script]->find(sect))
				{
					closescript();
					return -1;
				}
				else strcpy(sect, n_scripts[custom_npc_script]);
			} else return -1;
		} else strcpy(sect, "npc.scp");
		do
		{
			read1();
			if (script1[0]!='}')
			{
				if(j==i)
				{
					//script1 = ITEM#
					storeval=str2num(script1);
					pos=ftell(scpfile);
					closescript();
					retitem=Targ->AddMenuTarget(-1, 0, storeval);
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					if(retitem!=-1)
					{
						items[retitem].x=50+(rand()%80);
						items[retitem].y=50+(rand()%80);
						items[retitem].z=9;
						setserial(retitem,s,1);
					}
					j++;    
				}
				else j++;
			}
		}	while (script1[0]!='}');
		closescript();
	}
	return retitem;
}

/*** s: socket ***/
int cCharStuff::AddRandomNPC(int s, char * npclist, int spawnpoint)
{
	//This function gets the random npc number from the list and recalls
	//addrespawnnpc passing the new number
	char sect[512];
	unsigned int uiTempList[100];
	int i=0, k = 0;
	openscript("npc.scp");
	sprintf(sect, "NPCLIST %s", npclist);

	if (!i_scripts[npc_script]->find(sect))
	{
		closescript();
		if (n_scripts[custom_npc_script][0]!=0)
		{
			openscript(n_scripts[custom_npc_script]);
			if (!i_scripts[custom_npc_script]->find(sect))
			{
				closescript();
				return -1;
			}
		} else return -1;
	}
	do
	{
		read1();
		if (script1[0]!='}')
		{
			uiTempList[i]=str2num(script1);
			i++;
		}
	}
	while (script1[0]!='}');
	closescript();
	if(i>0)
	{
		i=rand()%(i);
		k=uiTempList[i];
	}
	if(k!=0)
	{
		if (spawnpoint==-1)
		{
			addmitem[s]=k;
			return Npcs->AddNPCxyz( s, k, 0, chars[currchar[s]].x, chars[currchar[s]].y, chars[currchar[s]].z );
		}
		else
		{
			return k; //addrespawnnpc(spawnpoint,k,1);
		}
	}
	return -1;
}


/*
** spawn an NPC,
** s: this is the index into items[] of the spawner
** region: this is the region that spawned the item if its a region spawner
** NOTE: You can only specify s or region, but not both. One must always be -1
**
** npcNum is the number of the NPC SECTION in ncp.scp to load up
** type appears to not be a type at all but type=0 means throw up a target cursor
** to let someone choose where to place it first, type=1 means just create it
*/
int cCharStuff::AddRespawnNPC( int s, int region, int npcNum, int type )
//void cCharStuff::AddRespawnNPC( int s, int npcNum, int type )
{
//	assert((s != -1 && region == -1) || (s == -1 && region != -1));

	int tmp, z,c,n, lovalue, hivalue, mypack, retitem;
	int storeval, shoppack1, shoppack2, shoppack3;
	int xos = 0, yos = 0;
	char sect[512];
	long int pos;
	char rndlootlist[20];
	int haircolor; //(we need this to remember the haircolor)
	haircolor=-1;
	
	mypack=-1;
	retitem=-1;
	storeval=-1;
	shoppack1=-1;
	shoppack2=-1;
	shoppack3=-1;

	// Dupois - Added Dec 20, 1999
	// If the spawner type is 125 and escort quests are not active then abort
	if( s != -1 )
		if( ( items[s].type == 125 ) && ( server_data.escortactive == 0 ) ) return -1;
	//
	// First things first...lets find out what NPC# we should spawn
	//
	openscript("npc.scp");
	sprintf(sect, "NPC %i", npcNum);
	if (!i_scripts[npc_script]->find(sect)) {
		closescript();
		if (n_scripts[custom_npc_script][0]!=0)
		{
			openscript(n_scripts[custom_npc_script]);
			if (!i_scripts[custom_npc_script]->find(sect))
			{
				closescript();	// we open, but never close it (Abaddon)
				return -1;
			}
		} else return -1;
	}

	// need to properly track whether we closed the script or not - fur
	bool didClose = false;
	do
	{
		read2();
		if (script1[0]!='}')
		{
			if (!(strcmp("NPCLIST", script1)))
			{
				pos=ftell(scpfile);
				closescript();
				didClose = true;
				if (type==1)
				{
					npcNum=AddRandomNPC(s,script2,1);
					if (npcNum==-1) 
					{
						closescript();
						return -1;
					}
				}
				else
				{
					npcNum=AddRandomNPC(s,script2,-1);
					if (npcNum==-1) 
					{
						closescript();
						return -1;
					}
				}
				openscript( "npc.scp" );
				didClose = false;			// if we don't flag this false, it won't close, eventually causing a crash! (Abaddon 17th February, 2000)
				fseek( scpfile, pos, SEEK_SET );
				break;  //got the NPC number to add stop reading
			}
		}
	} while (script1[0]!='}');
	if (!didClose)
		closescript();
	
	//
	// Now lets spawn him/her
	//
	c=MemCharFree ();
	if( c == -1 )
		return -1;
	InitChar(c);
	
	chars[c].priv=0x10;
	chars[c].npc=1;
	chars[c].att=1;
	chars[c].def=1;
	if( region != -1 )
	{
		chars[c].spawn1 = 0;
		chars[c].spawn2 = 1;		// 1 means region spawned -- Dodger_
		chars[c].spawn3 = region;
		chars[c].spawn4 = 0;
		chars[c].spawnserial = (1<<16) + (region<<8);
	}
	else
	{
		chars[c].spawn1 = 0;
		chars[c].spawn2 = 0;
		chars[c].spawn3 = 0;
		chars[c].spawn4 = 0;		// clear out values
		chars[c].spawnserial = 0;
	}
	int skl, sklvalue;
	
	openscript("npc.scp");
	sprintf(sect, "NPC %i", npcNum);
	if (!i_scripts[npc_script]->find(sect)) {
		closescript();
		if (n_scripts[custom_npc_script][0]!=0)
		{
			openscript(n_scripts[custom_npc_script]);
			if (!i_scripts[custom_npc_script]->find(sect))
			{
				closescript();
				return -1;
			}
			else strcpy(sect, n_scripts[custom_npc_script]);
		} else return -1;
	} else strcpy(sect, "npc.scp");
	do {
		read2();
		if (script1[0]!='}') 
		{
			switch( script1[0] )
			{
			case 'a':
			case 'A':
				if ((!(strcmp("ALCHEMY",script1)))||(!(strcmp("SKILL0",script1)))) chars[c].baseskill[ALCHEMY] = getstatskillvalue(script2);
				else if ((!(strcmp("ANATOMY",script1)))||(!(strcmp("SKILL1",script1)))) chars[c].baseskill[ANATOMY] = getstatskillvalue(script2);
				else if ((!(strcmp("ANIMALLORE",script1)))||(!(strcmp("SKILL2",script1)))) chars[c].baseskill[ANIMALLORE] = getstatskillvalue(script2);
				else if ((!(strcmp("ARMSLORE",script1)))||(!(strcmp("SKILL4",script1)))) chars[c].baseskill[ARMSLORE] = getstatskillvalue(script2);
				else if ((!(strcmp("ARCHERY",script1)))||(!(strcmp("SKILL31",script1)))) chars[c].baseskill[ARCHERY] = getstatskillvalue(script2);
				else if (!(strcmp("ATT",script1)))
				{
					gettokennum(script2, 0);
					lovalue=str2num(gettokenstr);
					gettokennum(script2, 1);
					hivalue=str2num(gettokenstr);
					chars[c].lodamage = lovalue;
					chars[c].hidamage = lovalue;
					if(hivalue) 
					{
						chars[c].hidamage = hivalue;
					}
				}
				break;

			case 'b':
			case 'B':
				if (!(strcmp("BACKPACK", script1))) 
				{
					if (mypack==-1) 
					{
						for (z=0;z<contsp[chars[c].serial%HASHMAX].max;z++)
						{
							mypack = contsp[chars[c].serial%HASHMAX].pointer[z];
							if ( mypack > -1 && items[mypack].contserial == chars[c].serial && items[mypack].layer == 0x15 )
								break;
							else
								mypack = -1;
						}
						if ( z == contsp[chars[c].serial%HASHMAX].max )
							mypack = -1;
					}
					if (mypack==-1)
					{
						pos=ftell(scpfile);
						closescript();
						chars[c].packitem=n=Items->SpawnItem(-1,c,1,"Backpack",0,0x0E,0x75,0,0,0,0);
						if( n != -1 )
						{
							items[n].x=0;
							items[n].y=0;
							items[n].z=0;
							setserial(n,c,4);
							items[n].layer=0x15;
							items[n].type=1;
							items[n].dye=1;
							mypack=n;
							retitem=n;
						}
						openscript(sect);
						fseek(scpfile, pos, SEEK_SET);
						strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
					}
				}
				else if ((!(strcmp("BEGGING",script1)))||(!(strcmp("SKILL6",script1)))) chars[c].baseskill[BEGGING] = getstatskillvalue(script2);
				else if ((!(strcmp("BLACKSMITHING",script1)))||(!(strcmp("SKILL7",script1)))) chars[c].baseskill[BLACKSMITHING] = getstatskillvalue(script2);
				else if ((!(strcmp("BOWCRAFT",script1)))||(!(strcmp("SKILL8",script1)))) chars[c].baseskill[BOWCRAFT] = getstatskillvalue(script2);
				break;

			case 'c':
			case 'C':
				if (!(strcmp("COLOR",script1))) 
				{
					if (retitem>-1)
					{
					  items[retitem].color1 = (unsigned char)((hstr2num(script2))>>8);
					  items[retitem].color2 = (unsigned char)((hstr2num(script2))%256);
					}
				}
				else if ((!(strcmp("CAMPING",script1)))||(!(strcmp("SKILL10",script1)))) chars[c].baseskill[CAMPING] = getstatskillvalue(script2);
				else if ((!(strcmp("CARPENTRY",script1)))||(!(strcmp("SKILL11",script1)))) chars[c].baseskill[CARPENTRY] = getstatskillvalue(script2);
				else if ((!(strcmp("CARTOGRAPHY",script1)))||(!(strcmp("SKILL12",script1)))) chars[c].baseskill[CARTOGRAPHY] = getstatskillvalue(script2);
				else if ((!(strcmp("COOKING",script1)))||(!(strcmp("SKILL13",script1)))) chars[c].baseskill[COOKING] = getstatskillvalue(script2);
				else if (!(strcmp("COLORMATCHHAIR",script1)))
				{
					if (retitem>-1)
					{
					  items[retitem].color1 = (unsigned char)((haircolor)>>8);
					  items[retitem].color2 = (unsigned char)((haircolor)%256);
					}
				}
				else if (!(strcmp("COLORLIST",script1)))
				{
					pos=ftell(scpfile);
					closescript();
					storeval=addrandomcolor(c,script2);
					if (retitem>-1)
					{
					  items[retitem].color1 = (unsigned char)((storeval)>>8);
					  items[retitem].color2 = (unsigned char)((storeval)%256);
					}
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					strcpy(script1, "DUMMY"); // To prevent accidental exit of loop.
				}
				break;

			case 'd':
			case 'D':
				if (!(strcmp("DIRECTION",script1))) 
				{
					if (!(strcmp("NE",script2))) chars[c].dir=1;
					else if (!(strcmp("E",script2))) chars[c].dir=2;
					else if (!(strcmp("SE",script2))) chars[c].dir=3;
					else if (!(strcmp("S",script2))) chars[c].dir=4;
					else if (!(strcmp("SW",script2))) chars[c].dir=5;
					else if (!(strcmp("W",script2))) chars[c].dir=6;
					else if (!(strcmp("NW",script2))) chars[c].dir=7;
					else if (!(strcmp("N",script2))) chars[c].dir=0;
				}
				else if ((!(strcmp("DEX",script1)))||(!(strcmp("DEXTERITY",script1)))) {
					chars[c].dx  = getstatskillvalue(script2);
					chars[c].dx2 = chars[c].dx;
					chars[c].stm = chars[c].dx;
				}
				else if ((!(strcmp("DETECTINGHIDDEN",script1)))||(!(strcmp("SKILL14",script1)))) chars[c].baseskill[DETECTINGHIDDEN] = getstatskillvalue(script2);
				else if (!(strcmp("DAMAGE",script1)))
				{
					gettokennum(script2, 0);
					lovalue=str2num(gettokenstr);
					gettokennum(script2, 1);
					hivalue=str2num(gettokenstr);
					chars[c].lodamage = lovalue;
					chars[c].hidamage = lovalue;
					if(hivalue) {
						chars[c].hidamage = hivalue;
					}
				}
				else if (!(strcmp("DEF",script1))) chars[c].def = getstatskillvalue(script2);
				break;

			case 'e':
			case 'E':
				if (!(strcmp("EMOTECOLOR",script1))) {
					chars[c].emotecolor1 = (unsigned char)((hstr2num(script2))>>8);
					chars[c].emotecolor2 = (unsigned char)((hstr2num(script2))%256);
				}
				else if ((!(strcmp("ENTICEMENT",script1)))||(!(strcmp("SKILL15",script1)))) chars[c].baseskill[ENTICEMENT] = getstatskillvalue(script2);
				else if ((!(strcmp("EVALUATINGINTEL",script1)))||(!(strcmp("SKILL16",script1)))) chars[c].baseskill[EVALUATINGINTEL] = getstatskillvalue(script2);

				break;

			case 'f':
			case 'F':
				if (!(strcmp("FAME",script1))) chars[c].fame=str2num(script2);
				else if (!(strcmp("FX1",script1))) chars[c].fx1=str2num(script2);  // new NPCWANDER implementation
				else if (!(strcmp("FX2",script1))) chars[c].fx2=str2num(script2);
				else if (!(strcmp("FY1",script1))) chars[c].fy1=str2num(script2);
				else if (!(strcmp("FY2",script1))) chars[c].fy2=str2num(script2);
				else if (!(strcmp("FZ1",script1))) chars[c].fz1=str2num(script2);
				else if (!(strcmp("FLEEAT",script1))) chars[c].fleeat=str2num(script2);
				else if ((!(strcmp("FISHING",script1)))||(!(strcmp("SKILL18",script1)))) chars[c].baseskill[FISHING] = getstatskillvalue(script2);
				else if ((!(strcmp("FORENSICS",script1)))||(!(strcmp("SKILL19",script1)))) chars[c].baseskill[FORENSICS] = getstatskillvalue(script2);
				else if ((!(strcmp("FENCING",script1)))||(!(strcmp("SKILL42",script1)))) chars[c].baseskill[FENCING] = getstatskillvalue(script2);

				break;

			case 'g':
			case 'G':
				if (!(strcmp("GOLD", script1))) 
				{
					if (mypack==-1) 
					{
						for (z=0;z<contsp[chars[c].serial%HASHMAX].max;z++)
						{
							mypack = contsp[chars[c].serial%HASHMAX].pointer[z];
							if ( mypack > -1 && items[mypack].contserial == chars[c].serial && items[mypack].layer == 0x15 )
								break;//We found it
							else
								mypack = -1;//we didn't, try to keep the pack set to -1
						}
						if ( z == contsp[chars[c].serial%HASHMAX].max )
							mypack = -1;//try to keep the pack set to -1
					}
					if (mypack!=-1)
					{ 
						pos=ftell(scpfile); /* lord binary ! */
						closescript();
						n = Items->SpawnItem(-1,c,1,"#",1,0x0E,0xED,0,0,1,0);
						openscript(sect);
						fseek(scpfile, pos, SEEK_SET);
						if( n != -1 )
						{
							items[n].priv=items[n].priv|0x01;
							gettokennum(script2, 0);
							lovalue=str2num(gettokenstr);
							gettokennum(script2, 1);
							hivalue=str2num(gettokenstr);
							if (hivalue==0) {
								items[n].amount=lovalue/2 + (rand()%(lovalue/2));
							} else {
								items[n].amount=lovalue + (rand()%(hivalue-lovalue));
							}
							setserial(n,mypack,1);
						}
					} else {
						printf("Warning: Bad NPC Script %d with problem no backpack for gold.\n", npcNum);
					}
				}

				break;

			case 'h':
			case 'H':
				if ((!(strcmp("HEALING",script1)))||(!(strcmp("SKILL17",script1)))) chars[c].baseskill[HEALING] = getstatskillvalue(script2);
				else if ((!(strcmp("HERDING",script1)))||(!(strcmp("SKILL20",script1)))) chars[c].baseskill[HERDING] = getstatskillvalue(script2);
				else if ((!(strcmp("HIDING",script1)))||(!(strcmp("SKILL21",script1)))) chars[c].baseskill[HIDING] = getstatskillvalue(script2);
				else if (!(strcmp("HAIRCOLOR",script1)))
				{
					pos=ftell(scpfile);
					closescript();
					haircolor=addrandomhaircolor(charcount,script2);
					if (retitem >-1)
					{
					  items[retitem].color1=(haircolor)>>8;
					  items[retitem].color2=(haircolor)%256;
					}
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					strcpy(script1, "DUMMY"); // To prevent accidental exit of loop.
				}
				else if (!(strcmp("HIDAMAGE",script1))) chars[c].hidamage=str2num(script2);

				break;

			case 'i':
			case 'I':
				if (!(strcmp("ID",script1))) 
				{
					tmp=hstr2num(script2);
					chars[c].id1=tmp>>8;
					chars[c].id2=tmp%256;
					chars[c].xid1=chars[c].id1;
					chars[c].xid2=chars[c].id2;
					chars[c].orgid1=chars[c].id1;
					chars[c].orgid2=chars[c].id2;
				}
				else if (!(strcmp("ITEM",script1))) 
				{
					storeval=str2num(script2);
					pos=ftell(scpfile);
					closescript();
					retitem=Targ->AddMenuTarget(-1, 0, storeval);
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					if (retitem!=-1)
					{
						setserial(retitem,c,4);
						if (items[retitem].layer==0) {
							printf("Warning: Bad NPC Script %d with problem item %d executed!\n", npcNum, storeval);
						}
					}
					strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
				}
				else if ((!(strcmp("INT",script1)))||(!(strcmp("INTELLIGENCE",script1)))) 
				{
					chars[c].in  = getstatskillvalue(script2);
					chars[c].in2 = chars[c].in;
					chars[c].mn  = chars[c].in;
				}
				else if ((!(strcmp("ITEMID",script1)))||(!(strcmp("SKILL3",script1)))) chars[c].baseskill[ITEMID] = getstatskillvalue(script2);
				else if ((!(strcmp("INSCRIPTION",script1)))||(!(strcmp("SKILL23",script1)))) chars[c].baseskill[INSCRIPTION] = getstatskillvalue(script2);
				break;

			case 'k':
			case 'K':
				if (!(strcmp("KARMA",script1))) chars[c].karma=str2num(script2);
				break;

			case 'l':
			case 'L':
				if (!(strcmp("LOOT",script1))) 
				{
					if (mypack==-1) {
						for (z=0;z<contsp[chars[c].serial%HASHMAX].max;z++)
						{
							mypack = contsp[chars[c].serial%HASHMAX].pointer[z];
							if ( mypack > -1 && items[mypack].contserial == chars[c].serial && items[mypack].layer == 0x15 )
								break;//We found it
							else
								mypack = -1;//we didn't, try to keep the pack set to -1
						}
						if ( z == contsp[chars[c].serial%HASHMAX].max )
							mypack = -1;//try to keep the pack set to -1
					}
					if (mypack!=-1) 
					{
						strcpy(rndlootlist, script2);
						pos=ftell(scpfile);
						closescript();
						retitem=AddRandomLoot(mypack, rndlootlist);
						openscript(sect);
						fseek(scpfile, pos, SEEK_SET);
						strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
					} else {
						printf("Warning: Bad NPC Script %d with problem no backpack for loot.\n", npcNum);
					}
				}
				else if ((!(strcmp("LOCKPICKING",script1)))||(!(strcmp("SKILL24",script1)))) chars[c].baseskill[LOCKPICKING] = getstatskillvalue(script2);
				else if ((!(strcmp("LUMBERJACKING",script1)))||(!(strcmp("SKILL44",script1)))) chars[c].baseskill[LUMBERJACKING] = getstatskillvalue(script2);
				else if (!(strcmp("LODAMAGE",script1))) chars[c].lodamage=str2num(script2);

				break;

			case 'm':
			case 'M':
				if ((!(strcmp("MAGERY",script1)))||(!(strcmp("SKILL25",script1)))) chars[c].baseskill[MAGERY] = getstatskillvalue(script2);
				else if ((!(strcmp("MAGICRESISTANCE",script1)))||(!(strcmp("RESIST",script1)))||(!(strcmp("SKILL26",script1)))) chars[c].baseskill[MAGICRESISTANCE] = getstatskillvalue(script2);
				else if ((!(strcmp("MUSICIANSHIP",script1)))||(!(strcmp("SKILL29",script1)))) chars[c].baseskill[MUSICIANSHIP] = getstatskillvalue(script2);
				else if ((!(strcmp("MACEFIGHTING",script1)))||(!(strcmp("SKILL41",script1)))) chars[c].baseskill[MACEFIGHTING] = getstatskillvalue(script2);
				else if ((!(strcmp("MINING",script1)))||(!(strcmp("SKILL45",script1)))) chars[c].baseskill[MINING] = getstatskillvalue(script2);
				else if ((!(strcmp("MEDITATION",script1)))||(!(strcmp("SKILL46",script1)))) chars[c].baseskill[MEDITATION] = getstatskillvalue(script2);

				break;

			case 'n':
			case 'N':
				if (!(strcmp("NAME",script1))) strcpy(chars[c].name, script2);
				else if (!(strcmp("NAMELIST", script1))) 
				{
					pos=ftell(scpfile);
					closescript();
					setrandomname(c,script2);
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					strcpy(script1, "DUMMY"); // To prevent accidental exit of loop.
				}
				else if (!(strcmp("NPCWANDER",script1))) chars[c].npcWander=str2num(script2);
				else if (!(strcmp("NPCAI",script1))) chars[c].npcaitype=hstr2num(script2);
				else if (!(strcmp(script1, "NOTRAIN"))) chars[c].cantrain=0;

				break;

			case 'p':
			case 'P':
				if (!(strcmp("PACKITEM",script1))) 
				{
					if (mypack==-1) 
					{
						for (z=0;z<contsp[chars[c].serial%HASHMAX].max;z++)
						{
							mypack = contsp[chars[c].serial%HASHMAX].pointer[z];
							if ( mypack > -1 && items[mypack].contserial == chars[c].serial && items[mypack].layer == 0x15 )
								break;//We found it
							else
								mypack = -1;//we didn't, try to keep the pack set to -1
						}
						if ( z == contsp[chars[c].serial%HASHMAX].max )
							mypack = -1;//try to keep the pack set to -1
					}
					if (mypack!=-1) 
					{
						storeval=str2num(script2);
						pos=ftell(scpfile);
						closescript();
						retitem=Targ->AddMenuTarget(-1, 0, storeval);
						openscript(sect);
						fseek(scpfile, pos, SEEK_SET);
						if (retitem!=-1)
						{
							setserial(retitem,mypack,1);
							items[retitem].x=50+(rand()%80);
							items[retitem].y=50+(rand()%80);
							items[retitem].z=9;
						}
						strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
					} else {
						printf("Warning: Bad NPC Script %d with problem no backpack for packitem.\n", npcNum);
					}
				}
				else if (!(strcmp("PRIV1",script1))) chars[c].priv=str2num(script2);
				else if (!(strcmp("PRIV2",script1))) chars[c].priv2=str2num(script2);
				else if (!(strcmp("POISON",script1))) chars[c].poison=str2num(script2);
				else if ((!(strcmp("PARRYING",script1)))||(!(strcmp("SKILL5",script1)))) chars[c].baseskill[PARRYING] = getstatskillvalue(script2);
				else if ((!(strcmp("PEACEMAKING",script1)))||(!(strcmp("SKILL9",script1)))) chars[c].baseskill[PEACEMAKING] = getstatskillvalue(script2);
				else if ((!(strcmp("PROVOCATION",script1)))||(!(strcmp("SKILL22",script1)))) chars[c].baseskill[PROVOCATION] = getstatskillvalue(script2);
				else if ((!(strcmp("POISONING",script1)))||(!(strcmp("SKILL30",script1)))) chars[c].baseskill[POISONING] = getstatskillvalue(script2);

				break;

			case 'r':
			case 'R':
				if (!(strcmp("RSHOPITEM",script1))) 
				{
					if (shoppack1==-1) 
					{
						for (z=0;z<contsp[chars[c].serial%HASHMAX].max;z++)
						{
							shoppack1 = contsp[chars[c].serial%HASHMAX].pointer[z];
							if ( shoppack1 > -1 && items[shoppack1].contserial == chars[c].serial && items[shoppack1].layer == 0x1A )
								break;//We found it
							else
								shoppack1 = -1;//we didn't, try to keep the pack set to -1
						}
						if ( z == contsp[chars[c].serial%HASHMAX].max )
							shoppack1 = -1;//try to keep the pack set to -1
					}
					if (shoppack1!=-1) 
					{
						storeval=str2num(script2);
						pos=ftell(scpfile);
						closescript();
						retitem=Targ->AddMenuTarget(-1, 0, storeval);
						openscript(sect);
						fseek(scpfile, pos, SEEK_SET);
						if (retitem!=-1)
						{
							setserial(retitem,shoppack1,1);
							items[retitem].x=50+(rand()%80);
							items[retitem].y=50+(rand()%80);
							items[retitem].z=9;
							if( items[retitem].name2[0] && ( strcmp( items[retitem].name2, "#" )))
								strcpy( items[retitem].name, items[retitem].name2 ); // Item identified! -- by Magius(CHE)
						}
						strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
					} else {
						printf("Warning: Bad NPC Script %d with problem no shoppack1 for item.\n", npcNum);
					}
				}
				else if (!(strcmp("REATTACKAT",script1))) chars[c].reattackat=str2num(script2);
				else if ((!(strcmp("REMOVETRAPS",script1)))||(!(strcmp("SKILL48",script1)))) chars[c].baseskill[REMOVETRAPS] = getstatskillvalue(script2);
				else if( !( strcmp( "RACE", script1 ) ) ) chars[c].race = str2num( script2 );
				else if (!(strcmp(script1, "RUNS"))) chars[c].runs = true;

				break;
			case 's':
			case 'S':
				if (!(strcmp("SKIN",script1))) 
				{
					tmp=hstr2num(script2);
					chars[c].skin1 = (unsigned char)(tmp>>8);
					chars[c].skin2 = (unsigned char)(tmp%256);
					chars[c].xskin1 = chars[c].skin1;
					chars[c].xskin2 = chars[c].skin2;
				}
				else if (!(strcmp("SHOPKEEPER", script1))) 
				{
					pos=ftell(scpfile);
					closescript();
					Commands->MakeShop(c);
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
				}
				else if (!(strcmp("SELLITEM",script1))) 
				{
					if (shoppack3==-1) 
					{
						for (z=0;z<contsp[chars[c].serial%HASHMAX].max;z++)
						{
							shoppack3 = contsp[chars[c].serial%HASHMAX].pointer[z];
							if ( shoppack3 > -1 && items[shoppack3].contserial == chars[c].serial && items[shoppack3].layer == 0x1C )
								break;//We found it
							else
								shoppack3 = -1;//we didn't, try to keep the pack set to -1
						}
						if ( z == contsp[chars[c].serial%HASHMAX].max )
							shoppack3 = -1;//try to keep the pack set to -1
					}
					if (shoppack3!=-1) 
					{
						storeval=str2num(script2);
						pos=ftell(scpfile);
						closescript();
						retitem=Targ->AddMenuTarget(-1, 0, storeval);
						openscript(sect);
						fseek(scpfile, pos, SEEK_SET);
						if (retitem!=-1)
						{
							setserial(retitem,shoppack3,1);
							items[retitem].value=items[retitem].value/2;
							items[retitem].x=50+(rand()%80);
							items[retitem].y=50+(rand()%80);
							items[retitem].z=9;
							if( items[retitem].name2[0] && ( strcmp( items[retitem].name2, "#" ) ) )
								strcpy( items[retitem].name, items[retitem].name2 );
						}
						strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
					} else {
						printf("Warning: Bad NPC Script %d with problem no shoppack3 for item.\n", npcNum);
					}
				}
				else if (!(strcmp("SHOPITEM",script1))) 
				{
					if (shoppack2==-1) 
					{
						for (z=0;z<contsp[chars[c].serial%HASHMAX].max;z++)
						{
							shoppack2 = contsp[chars[c].serial%HASHMAX].pointer[z];
							if ( shoppack2 > -1 && items[shoppack2].contserial == chars[c].serial && items[shoppack2].layer == 0x1B )
								break;//We found it
							else
								shoppack2 = -1;//we didn't, try to keep the pack set to -1
						}
						if ( z == contsp[chars[c].serial%HASHMAX].max )
							shoppack2 = -1;//try to keep the pack set to -1
					}
					if (shoppack2!=-1) 
					{
						storeval=str2num(script2);
						pos=ftell(scpfile);
						closescript();
						retitem=Targ->AddMenuTarget(-1, 0, storeval);
						openscript(sect);
						fseek(scpfile, pos, SEEK_SET);
						if (retitem!=-1)
						{
							setserial(retitem,shoppack2,1);
							items[retitem].x=50+(rand()%80);
							items[retitem].y=50+(rand()%80);
							items[retitem].z=9;
							if( items[retitem].name2[0] && ( strcmp( items[retitem].name2, "#" ) ) )
								strcpy( items[retitem].name, items[retitem].name2 );
						}
						strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
					} else {
						printf("Warning: Bad NPC Script %d with problem no shoppack2 for item.\n", npcNum);
					}
				}
				else if (!(strcmp("SAYCOLOR",script1))) 
				{
					chars[c].saycolor1 = (unsigned char)((hstr2num(script2))>>8);
					chars[c].saycolor2 = (unsigned char)((hstr2num(script2))%256);
				}
				else if (!(strcmp("SPEECH",script1))) chars[c].speech=str2num(script2);
				else if (!(strcmp("SPATTACK",script1))) chars[c].spattack=str2num(script2);
				else if (!(strcmp("SPADELAY",script1))) chars[c].spadelay=str2num(script2);
				else if (!(strcmp(script1, "SPLIT"))) chars[c].split=str2num(script2);
				else if (!(strcmp(script1, "SPLITCHANCE"))) chars[c].splitchnc=str2num(script2);
				else if ((!(strcmp("SNOOPING",script1)))||(!(strcmp("SKILL28",script1)))) chars[c].baseskill[SNOOPING] = getstatskillvalue(script2);
				else if ((!(strcmp("SPIRITSPEAK",script1)))||(!(strcmp("SKILL32",script1)))) chars[c].baseskill[SPIRITSPEAK] = getstatskillvalue(script2);
				else if ((!(strcmp("STEALING",script1)))||(!(strcmp("SKILL33",script1)))) chars[c].baseskill[STEALING] = getstatskillvalue(script2);
				else if ((!(strcmp("STR",script1)))||(!(strcmp("STRENGTH",script1)))) 
				{
					chars[c].st  = getstatskillvalue(script2);
					chars[c].st2 = chars[c].st;
					chars[c].hp  = chars[c].st;
				}
				else if ((!(strcmp("SWORDSMANSHIP",script1)))||(!(strcmp("SKILL40",script1)))) chars[c].baseskill[SWORDSMANSHIP] = getstatskillvalue(script2);
				else if ((!(strcmp("STEALTH",script1)))||(!(strcmp("SKILL47",script1)))) chars[c].baseskill[STEALTH] = getstatskillvalue(script2);
				else if (!(strcmp("SKINLIST",script1)))
				{
					pos=ftell(scpfile);
					closescript();
					storeval=addrandomcolor(c,script2);
					chars[c].skin1 = (unsigned char)((storeval)>>8);
					chars[c].skin2 = (unsigned char)((storeval)%256);
					chars[c].xskin1 = chars[c].skin1;
					chars[c].xskin2 = chars[c].skin2;
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					strcpy(script1, "DUMMY"); // To prevent accidental exit of loop.
				}
				else if (!(strcmp("SKILL", script1))) 
				{
					gettokennum(script2, 0);
					z=str2num(gettokenstr);
					gettokennum(script2, 1);
					chars[c].baseskill[z]=str2num(gettokenstr);
				}
				else if(!(strncmp(script1, "SKILL", 5)))
				{
					skl = str2num( &script1[5] );
					sklvalue = str2num( script2 );
					chars[c].baseskill[skl] = sklvalue;
				}

				break;
			case 't':
			case 'T':
				if (!(strcmp("TITLE",script1))) strcpy(chars[c].title, script2);
				else if ((!(strcmp("TOTAME", script1)))||(!(strcmp("TAMING", script1)))) chars[c].taming=str2num(script2);
				else if ((!(strcmp("TACTICS",script1)))||(!(strcmp("SKILL27",script1)))) chars[c].baseskill[TACTICS] = getstatskillvalue(script2);
				else if ((!(strcmp("TAILORING",script1)))||(!(strcmp("SKILL34",script1)))) chars[c].baseskill[TAILORING] = getstatskillvalue(script2);
				else if ((!(strcmp("TAMING",script1)))||(!(strcmp("SKILL35",script1)))) chars[c].baseskill[TAMING] = getstatskillvalue(script2);
				else if ((!(strcmp("TASTEID",script1)))||(!(strcmp("SKILL36",script1)))) chars[c].baseskill[TASTEID] = getstatskillvalue(script2);
				else if ((!(strcmp("TINKERING",script1)))||(!(strcmp("SKILL37",script1)))) chars[c].baseskill[TINKERING] = getstatskillvalue(script2);
				else if ((!(strcmp("TRACKING",script1)))||(!(strcmp("SKILL38",script1)))) chars[c].baseskill[TRACKING] = getstatskillvalue(script2);
				break;

			case 'v':
			case 'V':
				if (!(strcmp("VALUE",script1))) if (retitem!=-1) items[retitem].value=(str2num(script2));
				else if ((!(strcmp("VETERINARY",script1)))||(!(strcmp("SKILL39",script1)))) chars[c].baseskill[VETERINARY] = getstatskillvalue(script2);
				break;

			case 'w':
			case 'W':
				if ((!(strcmp("WRESTLING",script1)))||(!(strcmp("SKILL43",script1)))) chars[c].baseskill[WRESTLING] = getstatskillvalue(script2);
				break;
			default:
				printf( "Unknown tag in AddRespawnNPC\n" );
				break;
			}
		}
	}
	while (script1[0]!='}');
	closescript();
   
   // Now that we have created the NPC, lets place him
   if (type==1) 
   {
	   if (triggerx)
	   {
		   chars[c].x=triggerx;
		   chars[c].y=triggery;
		   chars[c].dispz=chars[c].z=triggerz;
		   triggerx=c;
	   } else 
	   {
			// see if we are item spawning or region spawning
			if( s != -1 )
			{
				int awayX = 0;
				int awayY = 0;
				if ((items[s].type==69)&&(items[s].contserial==-1))   
                {   
					awayX = items[s].more3;
					awayY = items[s].more4;
                }
				else if( ( items[s].type == 125 ) && ( items[s].contserial == -1 ) ) // Dupois used for item spawning Escort NPC's
				{
					awayX = items[s].more3;
					awayY = items[s].more4;
				}

				FindSpotForNPC( c, items[s].x, items[s].y, awayX, awayY, items[s].z );
			}
			else
			{
				int xAway = (spawnregion[region].x2 - spawnregion[region].x1) / 2;
				int yAway = (spawnregion[region].y2 - spawnregion[region].y1) / 2;
				int originX = spawnregion[region].x1 + xAway;
				int originY = spawnregion[region].y1 + yAway;
				if (xAway <= 0)
				{
					printf("Error with spawn region %d, x1 >= x2\n", region - 1);
					xAway = 10;
				}
				else if (yAway <= 0)
				{
					printf("Error with spawn region %d, y1 >= y2\n", region - 1);
					yAway = 10;
				}
				FindSpotForNPC( c, originX, originY, xAway, yAway, illegal_z );
			}
		}
   }
   else 
   {
	   if( s!=-1 )
	   {
		   if (chars[c].fx1==-1)
		   {
			   chars[c].fx1=(buffer[s][11]<<8)+buffer[s][12]+xos;
			   chars[c].fy1=(buffer[s][13]<<8)+buffer[s][14]+yos;
			   if (chars[c].fz1!=-1) chars[c].fz1=buffer[s][16]+Map->TileHeight((buffer[s][17]<<8)+buffer[s][18]);
		   }
		   chars[c].x=(buffer[s][11]<<8)+buffer[s][12]+xos;
		   chars[c].y=(buffer[s][13]<<8)+buffer[s][14]+yos;
		   chars[c].dispz=chars[c].z=buffer[s][16]+Map->TileHeight((buffer[s][17]<<8)+buffer[s][18]);
	   }
   }

   // now that its been placed, lets setup a proper wander area

   if (-1 != s)
     setserial(c,s,6);
   
   chars[c].region=calcRegionFromXY(chars[c].x, chars[c].y);
   
   //Now find real 'skill' based on 'baseskill' (stat modifiers)
   for(z=0;z<TRUESKILLS;z++)
   {
	   Skills->updateSkillLevel(c,z);
   }
   
   if (donpcupdate==0) 
   {
	   updatechar(c);
   }
   //Char mapRegions
   mapRegions->RemoveItem(c+1000000);
   mapRegions->AddItem(c+1000000);
   setcharflag( c );
   // Dupois - Added Dec 20, 1999
   // After the NPC has been fully initialized, then post the message  (if its a quest spawner) type == 125
   if( s != -1 )
	   if( items[s].type == 125 )
		   MsgBoardQuestEscortCreate( c );
	// End - Dupois
   return c;
}

// try to come up with a sensible radius given the values from npc.scp
// this is used for both rectangular and circular areas really
int GimmeRadius(int c)
{
	// see if they supplied a 'radius'
	if (chars[c].fx2 > 0)
	{
		// if they were supplying a bounding area, use the radius from that
		if (chars[c].fx1 > 0)
			chars[c].fx2 = abs(chars[c].fx1 - chars[c].fx2);
	}
	// ensure its not something bogus
	if (chars[c].fx2 <= 0 || chars[c].fx2 > 100)
		chars[c].fx2 = 10;
	return chars[c].fx2;
}

// setup the wander area if the npcwander is rect or circle
// try to use the fx1, fy1, fx2, fxy2, fz1 variables defined in npc.scp if possible
void InitializeWanderArea(int c, int originX, int originY, int xAway, int yAway)
// PARAM WARNING: originX and originY are not used!
{
	// compute the rectangular bounding area
	if (3 == chars[c].npcWander)
	{
		// if they provided a legal rectangle and
		// ensure the bounding rect contains the current location
		// if it doesn't the monster will never move!
		if (chars[c].fx1 >= 0 && chars[c].fy1 >= 0 && chars[c].fy2 >= 0 && chars[c].fx2 >= 0 &&
			checkBoundingBox(chars[c].x, chars[c].y, chars[c].fx1, chars[c].fy1, chars[c].fz1, chars[c].fx2, chars[c].fy2))
		{
			// don't do anything to use what they specified in npc.scp		  
		}
		else
		{
			// if they provided a 'radius' in the npc.scp use that
			if (chars[c].fx2 > 0)
				xAway = yAway = GimmeRadius(c);
			// setup info for rectangular areas
			chars[c].fx1 = chars[c].x - xAway;
			chars[c].fy1 = chars[c].y - yAway;
			chars[c].fx2 = chars[c].x + xAway;
			chars[c].fy2 = chars[c].y + yAway;
		}
	}
	else if (4 == chars[c].npcWander)
	{
		// if they provided a legal circle and
		// ensure the bounding circle contains the current location
		// if it doesn't the monster will never move!
		if (chars[c].fx1 >= 0 && chars[c].fy1 >= 0 && chars[c].fx2 >= 0 &&
			checkBoundingCircle(chars[c].x, chars[c].y, chars[c].fx1, chars[c].fy1, chars[c].fz1, chars[c].fx2))
		{
			// don't do anything to use what they specified in npc.scp		  
		}
		else
		{
			// if they provided a 'radius' in the npc.scp use that
			if (chars[c].fx2 > 0)
				xAway = yAway = GimmeRadius(c);
			// setup info for circular areas
			chars[c].fx1 = chars[c].x;
			chars[c].fy1 = chars[c].y;
			chars[c].fx2 = xAway;
			chars[c].fy2 = -1;
		}
	}
	// setting fz1 actually makes it check against the height and slows the system down a lot
	// does anyone really need to constrain the height at which a monster can move?? - fur
	//chars[c].fz1 = chars[c].z;
	
#ifdef DEBUG_SPAWN
	printf("Bounding area for this monster is fx1: %d, fx2: %d, fy1: %d, fy2: %d\n", chars[c].fx1, chars[c].fx2, chars[c].fy1, chars[c].fy2);		   
#endif
}

void cCharStuff::FindSpotForNPC(int c, int originX, int originY, int xAway, int yAway, int elev)
{
  /*Zippy's Code chages for area spawns --> (Type 69) xos and yos (X OffSet, Y OffSet) 
    are used to find a random number that is then added to the spawner's x and y (Using 
    the spawner's z) and then place the NPC anywhere in a square around the spawner. 
    This square is random anywhere from -10 to +10 from the spawner's location (for x and 
    y) If the place chosen is not a valid position (the NPC can't walk there) then a new 
    place will be chosen, if a valid place cannot be found in a certain # of tries (50), 
    the NPC will be placed directly on the spawner and the server op will be warned. */

#ifdef DEBUG_SPAWN
	printf("Going to spawn at (%d,%d) within %d by %d\n", originX, originY, xAway, yAway);
#endif
	
	int k = xAway * yAway / 2;
	int xos = 0, yos = 0;
	bool foundSpot = false;
	if( k > 50 ) 
		k = 50;
	
	while( !foundSpot )
    {
		if( --k < 0 ) //this CAN be a bit laggy. adjust as nessicary
		{
			if( xAway > 0 && yAway > 0 )
				printf("UOX3: Problem area spawner found, NPC placed at default location.\n");
			xos=originX;
			yos=originY;
			foundSpot = true;
			break;
		}
		
		xos = originX + RandomNum(-xAway, xAway);
		yos = originY + RandomNum(-yAway, yAway);
		
		if ((xos >= 1) && (yos >= 1))
			foundSpot = Map->CanMonsterMoveHere(xos, yos, elev);					 
		
    }
	
	chars[c].x = xos;
	chars[c].y = yos;
	if (illegal_z == elev)
		elev = Map->Height(xos, yos, 0);
	chars[c].dispz = chars[c].z = elev;
	
	InitializeWanderArea(c, originX, originY, xAway, yAway);
}



// type 0 -> s = spawner item #
// type 1 -> s = socket
// whohooo, nice programming style
// LB 

int cCharStuff::AddNPCxyz(int s, int npcNum, int type, int x1, int y1, signed char z1) //Morrolan - replacement for old AddNPCxyz(), fixes a LOT of problems.
{
	int tmp, z,c,n, lovalue, hivalue, mypack, retitem;
	char sect[512];
	long int pos;
	int k=0, xos=0, yos=0, lb;
	int storeval, shoppack1, shoppack2, shoppack3;
	char rndlootlist[20];
	int haircolor; //(we need this to remember the haircolor)
	haircolor=-1;
	
	mypack=-1;
	retitem=-1;
	storeval=-1;
	shoppack1=-1;
	shoppack2=-1;
	shoppack3=-1;
	
	//
	// First things first...lets find out what NPC# we should spawn
	//

	openscript("npc.scp");
	sprintf(sect, "NPC %i", npcNum);
	if (!i_scripts[npc_script]->find(sect)) {
		closescript();
		if (n_scripts[custom_npc_script][0]!=0)
		{
			openscript(n_scripts[custom_npc_script]);
			if (!i_scripts[custom_npc_script]->find(sect))
				return -1;
		} else return -1;
	}
	
	do
	{
		read2();
		if (script1[0]!='}')
		{
			if (!(strcmp("NPCLIST", script1)))
			{
				pos=ftell(scpfile);
				closescript();
				if (type==1)
				{
					npcNum=AddRandomNPC(s,script2,1);
					if (npcNum==-1) return -1;
				}
				else
				{
					npcNum=AddRandomNPC(s,script2,-1);
					if (npcNum==-1) return -1;
				}
				break;  //got the NPC number to add stop reading
			}
		}
	} while (script1[0]!='}');
	closescript();
	
	c=MemCharFree ();
	if( c == -1 )
		return -1;
	
	InitChar(c);
	if (type==1)
	{
		chars[c].x=items[s].x;
		chars[c].y=items[s].y;
		chars[c].dispz=chars[c].z=items[s].z;
		setserial(c,s,6);
	}
	else
	{
		chars[c].x=x1;
		chars[c].y=y1;
		chars[c].dispz=chars[c].z=z1;
		//setserial(c,s,6); // lb 
		                    // lb a few day later: damn it this was BEbugging, shame on me ...
	}
	
	chars[c].priv=0x10;
	chars[c].npc=1;
	chars[c].att=1;
	chars[c].def=1;
	chars[c].spawnserial = 0;
//	chars[c].spawnserial=-1; //Zippy
	
	
	// Now let's spawn it.
	int skl, sklvalue;
	
	openscript("npc.scp");
	sprintf(sect, "NPC %i", npcNum);
	if (!i_scripts[npc_script]->find(sect)) {
		closescript();
		if (n_scripts[custom_npc_script][0]!=0)
		{
			openscript(n_scripts[custom_npc_script]);
			if (!i_scripts[custom_npc_script]->find(sect))
				return -1;
			else strcpy(sect, n_scripts[custom_npc_script]);
		} else return -1;
	} else strcpy(sect, "npc.scp");
	do {
		read2();
		if (script1[0]!='}') 
		{
			switch( script1[0] )
			{
			case 'a':
			case 'A':
				if ((!(strcmp("ALCHEMY",script1)))||(!(strcmp("SKILL0",script1)))) chars[c].baseskill[ALCHEMY] = getstatskillvalue(script2);
				else if ((!(strcmp("ANATOMY",script1)))||(!(strcmp("SKILL1",script1)))) chars[c].baseskill[ANATOMY] = getstatskillvalue(script2);
				else if ((!(strcmp("ANIMALLORE",script1)))||(!(strcmp("SKILL2",script1)))) chars[c].baseskill[ANIMALLORE] = getstatskillvalue(script2);
				else if ((!(strcmp("ARMSLORE",script1)))||(!(strcmp("SKILL4",script1)))) chars[c].baseskill[ARMSLORE] = getstatskillvalue(script2);
				else if ((!(strcmp("ARCHERY",script1)))||(!(strcmp("SKILL31",script1)))) chars[c].baseskill[ARCHERY] = getstatskillvalue(script2);

				break;

			case 'b':
			case 'B':
				if (!(strcmp("BACKPACK", script1))) 
				{
					if (mypack==-1) 
					{
						for (z=0;z<contsp[chars[c].serial%HASHMAX].max;z++)
						{
							mypack = contsp[chars[c].serial%HASHMAX].pointer[z];
							if ( mypack > -1 && !items[mypack].free && items[mypack].contserial == chars[c].serial && items[mypack].layer == 0x15 )
								break;//We found it
							else
								mypack = -1;//we didn't, try to keep the pack set to -1
						}
						if ( z == contsp[chars[c].serial%HASHMAX].max )
							mypack = -1;//try to keep the pack set to -1
					}
					if (mypack==-1)
					{
						pos=ftell(scpfile);
						closescript();
						chars[c].packitem=n=Items->SpawnItem(calcSocketFromChar(c),c,1,"Backpack",0,0x0E,0x75,0,0,0,0);
						if( n != -1 )
						{
							items[n].x=0;
							items[n].y=0;
							items[n].z=0;
							setserial(n,c,4);
							items[n].layer=0x15;
							items[n].type=1;
							items[n].dye=1;
							mypack=n;
							retitem=n;
						}
						openscript(sect);
						fseek(scpfile, pos, SEEK_SET);
						strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
					}
				}
				else if ((!(strcmp("BEGGING",script1)))||(!(strcmp("SKILL6",script1)))) chars[c].baseskill[BEGGING] = getstatskillvalue(script2);
				else if ((!(strcmp("BLACKSMITHING",script1)))||(!(strcmp("SKILL7",script1)))) chars[c].baseskill[BLACKSMITHING] = getstatskillvalue(script2);
				else if ((!(strcmp("BOWCRAFT",script1)))||(!(strcmp("SKILL8",script1)))) chars[c].baseskill[BOWCRAFT] = getstatskillvalue(script2);

				break;

			case 'c':
			case 'C':
				if (!(strcmp("COLOR",script1))) {
					if (retitem!=-1)
					{
					  items[retitem].color1 = (unsigned char)((hstr2num(script2))>>8);
					  items[retitem].color2 = (unsigned char)((hstr2num(script2))%256);
					}
				}
				else if ((!(strcmp("CAMPING",script1)))||(!(strcmp("SKILL10",script1)))) chars[c].baseskill[CAMPING] = getstatskillvalue(script2);
				else if ((!(strcmp("CARPENTRY",script1)))||(!(strcmp("SKILL11",script1)))) chars[c].baseskill[CARPENTRY] = getstatskillvalue(script2);
				else if ((!(strcmp("CARTOGRAPHY",script1)))||(!(strcmp("SKILL12",script1)))) chars[c].baseskill[CARTOGRAPHY] = getstatskillvalue(script2);
				else if ((!(strcmp("COOKING",script1)))||(!(strcmp("SKILL13",script1)))) chars[c].baseskill[COOKING] = getstatskillvalue(script2);
				else if (!(strcmp("COLORMATCHHAIR",script1)))
				{
					if (retitem>-1)
					{
					  items[retitem].color1 = (unsigned char)((haircolor)>>8);
					  items[retitem].color2 = (unsigned char)((haircolor)%256);
					}
				}
				else if (!(strcmp("COLORLIST",script1)))
				{
					pos=ftell(scpfile);
					closescript();
					storeval=addrandomcolor(c,script2);
					if (retitem>-1)
					{
					  items[retitem].color1 = (unsigned char)((storeval)>>8);
					  items[retitem].color2 = (unsigned char)((storeval)%256);
					}
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					strcpy(script1, "DUMMY"); // To prevent accidental exit of loop.
				}

				break;

			case 'd':
			case 'D':
				if (!(strcmp("DIRECTION",script1))) {
					if (!(strcmp("NE",script2))) chars[c].dir=1;
					if (!(strcmp("E",script2))) chars[c].dir=2;
					if (!(strcmp("SE",script2))) chars[c].dir=3;
					if (!(strcmp("S",script2))) chars[c].dir=4;
					if (!(strcmp("SW",script2))) chars[c].dir=5;
					if (!(strcmp("W",script2))) chars[c].dir=6;
					if (!(strcmp("NW",script2))) chars[c].dir=7;
					if (!(strcmp("N",script2))) chars[c].dir=0;
				}
				else if ((!(strcmp("DEX",script1)))||(!(strcmp("DEXTERITY",script1)))) 
				{
					chars[c].dx  = getstatskillvalue(script2);
					chars[c].dx2 = chars[c].dx;
					chars[c].stm = chars[c].dx;
				}
				else if ((!(strcmp("DETECTINGHIDDEN",script1)))||(!(strcmp("SKILL14",script1)))) chars[c].baseskill[DETECTINGHIDDEN] = getstatskillvalue(script2);
				else if ((!(strcmp("DAMAGE",script1)))||(!(strcmp("ATT",script1)))) 
				{
					gettokennum(script2, 0);
					lovalue=str2num(gettokenstr);
					gettokennum(script2, 1);
					hivalue=str2num(gettokenstr);
					chars[c].lodamage = lovalue;
					chars[c].hidamage = lovalue;
					if(hivalue) {
						chars[c].hidamage = hivalue;
					}
				}
				else if (!(strcmp("DEF",script1))) chars[c].def = getstatskillvalue(script2);

				break;

			case 'e':
			case 'E':
				if (!(strcmp("EMOTECOLOR",script1))) {
					chars[c].emotecolor1 = (unsigned char)((hstr2num(script2))>>8);
					chars[c].emotecolor2 = (unsigned char)((hstr2num(script2))%256);
				}
				else if ((!(strcmp("ENTICEMENT",script1)))||(!(strcmp("SKILL15",script1)))) chars[c].baseskill[ENTICEMENT] = getstatskillvalue(script2);
				else if ((!(strcmp("EVALUATINGINTEL",script1)))||(!(strcmp("SKILL16",script1)))) chars[c].baseskill[EVALUATINGINTEL] = getstatskillvalue(script2);

				break;

			case 'f':
			case 'F':
				if (!(strcmp("FAME",script1))) chars[c].fame=str2num(script2);
				else if (!(strcmp("FX1",script1))) chars[c].fx1=chars[c].x+str2num(script2);  // new NPCWANDER implementation
				else if (!(strcmp("FX2",script1))) chars[c].fx2=chars[c].x+str2num(script2);
				else if (!(strcmp("FY1",script1))) chars[c].fy1=chars[c].y+str2num(script2);
				else if (!(strcmp("FY2",script1))) chars[c].fy2=chars[c].y+str2num(script2);
				else if (!(strcmp("FZ1",script1))) chars[c].fz1=chars[c].z+str2num(script2);
				else if (!(strcmp("FLEEAT",script1))) chars[c].fleeat=str2num(script2);
				else if ((!(strcmp("FISHING",script1)))||(!(strcmp("SKILL18",script1)))) chars[c].baseskill[FISHING] = getstatskillvalue(script2);
				else if ((!(strcmp("FORENSICS",script1)))||(!(strcmp("SKILL19",script1)))) chars[c].baseskill[FORENSICS] = getstatskillvalue(script2);
				else if ((!(strcmp("FENCING",script1)))||(!(strcmp("SKILL42",script1)))) chars[c].baseskill[FENCING] = getstatskillvalue(script2);

				break;

			case 'g':
			case 'G':
				if (!(strcmp("GOLD", script1))) 
				{
					if (mypack==-1) {
						for (z=0;z<contsp[chars[c].serial%HASHMAX].max;z++)
						{
							mypack = contsp[chars[c].serial%HASHMAX].pointer[z];
							if ( mypack > -1 && !items[mypack].free && items[mypack].contserial == chars[c].serial && items[mypack].layer == 0x15 )
								break;//We found it
							else
								mypack = -1;//we didn't, try to keep the pack set to -1
						}
						if ( z == contsp[chars[c].serial%HASHMAX].max )
							mypack = -1;//try to keep the pack set to -1
					}
					if (mypack!=-1)
					{ 
						pos=ftell(scpfile);
						closescript();
						n=Items->SpawnItem(calcSocketFromChar(c),c,1,"#",1,0x0E,0xED,0,0,1,0);
						openscript(sect);
						fseek(scpfile, pos, SEEK_SET);
						if( n != -1 )
						{
							items[n].priv=items[n].priv|0x01;
							gettokennum(script2, 0);
							lovalue=str2num(gettokenstr);
							gettokennum(script2, 1);
							hivalue=str2num(gettokenstr);
							if (hivalue==0) {
								items[n].amount=lovalue/2 + (rand()%(lovalue/2));
							} else {
								items[n].amount=lovalue + (rand()%(hivalue-lovalue));
							}
							setserial(n,mypack,1);
						}
					} else {
						printf("Warning: Bad NPC Script %d with problem no backpack for gold.\n", npcNum);
					}
				}

				break;

			case 'h':
			case 'H':
				if ((!(strcmp("HEALING",script1)))||(!(strcmp("SKILL17",script1)))) chars[c].baseskill[HEALING] = getstatskillvalue(script2);
				else if ((!(strcmp("HERDING",script1)))||(!(strcmp("SKILL20",script1)))) chars[c].baseskill[HERDING] = getstatskillvalue(script2);
				else if ((!(strcmp("HIDING",script1)))||(!(strcmp("SKILL21",script1)))) chars[c].baseskill[HIDING] = getstatskillvalue(script2);
				else if (!(strcmp("HAIRCOLOR",script1)))
				{
					pos=ftell(scpfile);
					closescript();
					haircolor=addrandomhaircolor(charcount,script2);
					if (retitem>-1)
					{
					  items[retitem].color1 = (unsigned char)((haircolor)>>8);
					  items[retitem].color2 = (unsigned char)((haircolor)%256);
					}
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					strcpy(script1, "DUMMY"); // To prevent accidental exit of loop.
				}
				else if (!(strcmp("HIDAMAGE",script1))) chars[c].hidamage=str2num(script2);

				break;

			case 'i':
			case 'I':
				if (!(strcmp("ID",script1))) 
				{
					tmp=hstr2num(script2);
					chars[c].id1 = (unsigned char)(tmp>>8);
					chars[c].id2 = (unsigned char)(tmp%256);
					chars[c].xid1=chars[c].id1;
					chars[c].xid2=chars[c].id2;
					chars[c].orgid1=chars[c].id1;
					chars[c].orgid2=chars[c].id2;
				}
				else if (!(strcmp("ITEM",script1))) 
				{
					storeval=str2num(script2);
					pos=ftell(scpfile);
					closescript();
					retitem=Targ->AddMenuTarget(-1, 0, storeval);
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					if (retitem!=-1)
					{
						setserial(retitem,c,4);
						if (items[retitem].layer==0) {
							printf("Warning: Bad NPC Script %d with problem item %d executed!\n", npcNum, storeval);
						}
					}
					strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
				}
				else if ((!(strcmp("INT",script1)))||(!(strcmp("INTELLIGENCE",script1)))) 
				{
					chars[c].in  = getstatskillvalue(script2);
					chars[c].in2 = chars[c].in;
					chars[c].mn  = chars[c].in;
				}
				else if ((!(strcmp("ITEMID",script1)))||(!(strcmp("SKILL3",script1)))) chars[c].baseskill[ITEMID] = getstatskillvalue(script2);
				else if ((!(strcmp("INSCRIPTION",script1)))||(!(strcmp("SKILL23",script1)))) chars[c].baseskill[INSCRIPTION] = getstatskillvalue(script2);

				break;

			case 'k':
			case 'K':
				if (!(strcmp("KARMA",script1))) chars[c].karma=str2num(script2);

				break;

			case 'l':
			case 'L':
				if (!(strcmp("LOOT",script1))) 
				{
					if (mypack==-1) 
					{
						for (z=0;z<contsp[chars[c].serial%HASHMAX].max;z++)
						{
							mypack = contsp[chars[c].serial%HASHMAX].pointer[z];
							if ( mypack > -1 && !items[mypack].free && items[mypack].contserial == chars[c].serial && items[mypack].layer == 0x15 )
								break;//We found it
							else
								mypack = -1;//we didn't, try to keep the pack set to -1
						}
						if ( z == contsp[chars[c].serial%HASHMAX].max )
							mypack = -1;//try to keep the pack set to -1
					}
					if (mypack!=-1) 
					{
						strcpy(rndlootlist, script2);
						pos=ftell(scpfile);
						closescript();
						retitem=AddRandomLoot(mypack, rndlootlist);
						openscript(sect);
						fseek(scpfile, pos, SEEK_SET);
						strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
					} else {
						printf("Warning: Bad NPC Script %d with problem no backpack for loot.\n", npcNum);
					}
				}
				else if ((!(strcmp("LOCKPICKING",script1)))||(!(strcmp("SKILL24",script1)))) chars[c].baseskill[LOCKPICKING] = getstatskillvalue(script2);
				else if ((!(strcmp("LUMBERJACKING",script1)))||(!(strcmp("SKILL44",script1)))) chars[c].baseskill[LUMBERJACKING] = getstatskillvalue(script2);
				else if (!(strcmp("LODAMAGE",script1))) chars[c].lodamage=str2num(script2);

				break;

			case 'm':
			case 'M':
				if ((!(strcmp("MAGERY",script1)))||(!(strcmp("SKILL25",script1)))) chars[c].baseskill[MAGERY] = getstatskillvalue(script2);
				else if ((!(strcmp("MAGICRESISTANCE",script1)))||(!(strcmp("RESIST",script1)))||(!(strcmp("SKILL26",script1)))) chars[c].baseskill[MAGICRESISTANCE] = getstatskillvalue(script2);
				else if ((!(strcmp("MUSICIANSHIP",script1)))||(!(strcmp("SKILL29",script1)))) chars[c].baseskill[MUSICIANSHIP] = getstatskillvalue(script2);
				else if ((!(strcmp("MACEFIGHTING",script1)))||(!(strcmp("SKILL41",script1)))) chars[c].baseskill[MACEFIGHTING] = getstatskillvalue(script2);
				else if ((!(strcmp("MINING",script1)))||(!(strcmp("SKILL45",script1)))) chars[c].baseskill[MINING] = getstatskillvalue(script2);
				else if ((!(strcmp("MEDITATION",script1)))||(!(strcmp("SKILL46",script1)))) chars[c].baseskill[MEDITATION] = getstatskillvalue(script2);

				break;

			case 'n':
			case 'N':
				if (!(strcmp("NAME",script1))) strcpy(chars[c].name, script2);
				else if (!(strcmp("NAMELIST", script1))) 
				{
					pos=ftell(scpfile);
					closescript();
					setrandomname(c,script2);
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					strcpy(script1, "DUMMY"); // To prevent accidental exit of loop.
				}
				else if (!(strcmp("NPCWANDER",script1))) chars[c].npcWander=str2num(script2);
				else if (!(strcmp("NPCAI",script1))) chars[c].npcaitype=hstr2num(script2);
				else if (!(strcmp(script1, "NOTRAIN"))) chars[c].cantrain=0;

				break;

			case 'p':
			case 'P':
				if (!(strcmp("PACKITEM",script1))) 
				{
					if (mypack==-1) 
					{
						for (z=0;z<contsp[chars[c].serial%HASHMAX].max;z++)
						{
							mypack = contsp[chars[c].serial%HASHMAX].pointer[z];
							if ( mypack > -1 && !items[mypack].free && items[mypack].contserial == chars[c].serial && items[mypack].layer == 0x15 )
								break;//We found it
							else
								mypack = -1;//we didn't, try to keep the pack set to -1
						}
						if ( z == contsp[chars[c].serial%HASHMAX].max )
							mypack = -1;//try to keep the pack set to -1
					}
					if (mypack!=-1) 
					{
						storeval=str2num(script2);
						pos=ftell(scpfile);
						closescript();
						retitem=Targ->AddMenuTarget(-1, 0, storeval);
						openscript(sect);
						fseek(scpfile, pos, SEEK_SET);
						if (retitem!=-1)
						{
							setserial(retitem,mypack,1);
							items[retitem].x=50+(rand()%80);
							items[retitem].y=50+(rand()%80);
							items[retitem].z=9;
						}
						strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
					} else {
						printf("Warning: Bad NPC Script %d with problem no backpack for packitem.\n", npcNum);
					}
				}
				else if (!(strcmp("PRIV1",script1))) chars[c].priv=str2num(script2);
				else if (!(strcmp("PRIV2",script1))) chars[c].priv2=str2num(script2);
				else if (!(strcmp("POISON",script1))) chars[c].poison=str2num(script2);
				else if ((!(strcmp("PARRYING",script1)))||(!(strcmp("SKILL5",script1)))) chars[c].baseskill[PARRYING] = getstatskillvalue(script2);
				else if ((!(strcmp("PEACEMAKING",script1)))||(!(strcmp("SKILL9",script1)))) chars[c].baseskill[PEACEMAKING] = getstatskillvalue(script2);
				else if ((!(strcmp("PROVOCATION",script1)))||(!(strcmp("SKILL22",script1)))) chars[c].baseskill[PROVOCATION] = getstatskillvalue(script2);
				else if ((!(strcmp("POISONING",script1)))||(!(strcmp("SKILL30",script1)))) chars[c].baseskill[POISONING] = getstatskillvalue(script2);

				break;

			case 'r':
			case 'R':
				if (!(strcmp("RSHOPITEM",script1))) 
				{
					if (shoppack1==-1) 
					{
						//Alert!
						for (z=0;z<contsp[chars[c].serial%HASHMAX].max;z++)
						{
							shoppack1 = contsp[chars[c].serial%HASHMAX].pointer[z];
							if ( shoppack1 > -1 && !items[shoppack1].free && items[shoppack1].contserial == chars[c].serial && items[shoppack1].layer == 0x1A )
								break;//We found it
							else
								shoppack1 = -1;//we didn't, try to keep the pack set to -1
						}
						if ( z == contsp[chars[c].serial%HASHMAX].max )
							shoppack1 = -1;//try to keep the pack set to -1
					}
					if (shoppack1!=-1) 
					{
						storeval=str2num(script2);
						pos=ftell(scpfile);
						closescript();
						retitem=Targ->AddMenuTarget(-1, 0, storeval);
						openscript(sect);
						fseek(scpfile, pos, SEEK_SET);
						if (retitem!=-1)
						{
							setserial(retitem,shoppack1,1);
							items[retitem].x=50+(rand()%80);
							items[retitem].y=50+(rand()%80);
							items[retitem].z=9;
						}
						strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
					} else 
					{
						printf("Warning: Bad NPC Script %d with problem no shoppack1 for item.\n", npcNum);
					}
				}
				else if (!(strcmp("REATTACKAT",script1))) chars[c].reattackat=str2num(script2);
				else if (!(strcmp("RACE",script1))) chars[c].race=str2num(script2);
				else if (!(strcmp(script1, "RUNS"))) chars[c].runs = true;

				break;

			case 's':
			case 'S':
				if (!(strcmp("SKIN",script1))) 
				{
					tmp=hstr2num(script2);
					chars[c].skin1 = (unsigned char)(tmp>>8);
					chars[c].skin2 = (unsigned char)(tmp%256);
					chars[c].xskin1 = chars[c].skin1;
					chars[c].xskin2 = chars[c].skin2;
				}
				else if (!(strcmp("SHOPKEEPER", script1))) Commands->MakeShop(c);
				else if (!(strcmp("SELLITEM",script1))) 
				{
					if (shoppack3==-1) 
					{
						//Alert!
						for (z=0;z<contsp[chars[c].serial%HASHMAX].max;z++)
						{
							shoppack3 = contsp[chars[c].serial%HASHMAX].pointer[z];
							if ( shoppack3 > -1 && !items[shoppack3].free && items[shoppack3].contserial == chars[c].serial && items[shoppack3].layer == 0x1C )
								break;//We found it
							else
								shoppack3 = -1;//we didn't, try to keep the pack set to -1
						}
						if ( z == contsp[chars[c].serial%HASHMAX].max )
							shoppack3 = -1;//try to keep the pack set to -1
					}
					if (shoppack3!=-1) 
					{
						storeval=str2num(script2);
						pos=ftell(scpfile);
						closescript();
						retitem=Targ->AddMenuTarget(-1, 0, storeval);
						openscript(sect);
						fseek(scpfile, pos, SEEK_SET);
						if (retitem!=-1)
						{
							setserial(retitem,shoppack3,1);
							items[retitem].value=items[retitem].value/2;
							items[retitem].x=50+(rand()%80);
							items[retitem].y=50+(rand()%80);
							items[retitem].z=9;
						}
						strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
					} else {
						printf("Warning: Bad NPC Script %d with problem no shoppack3 for item.\n", npcNum);
					}
				}
				else if (!(strcmp("SHOPITEM",script1))) 
				{
					if (shoppack2==-1) 
					{
						for (z=0;z<contsp[chars[c].serial%HASHMAX].max;z++)
						{
							shoppack2 = contsp[chars[c].serial%HASHMAX].pointer[z];
							if ( shoppack2 > -1 && !items[shoppack2].free && items[shoppack2].contserial == chars[c].serial && items[shoppack2].layer == 0x1B )
								break;//We found it
							else
								shoppack2 = -1;//we didn't, try to keep the pack set to -1
						}
						if ( z == contsp[chars[c].serial%HASHMAX].max )
							shoppack2 = -1;//try to keep the pack set to -1
					}
					if (shoppack2!=-1) 
					{
						storeval=str2num(script2);
						pos=ftell(scpfile);
						closescript();
						retitem=Targ->AddMenuTarget(-1, 0, storeval);
						openscript(sect);
						fseek(scpfile, pos, SEEK_SET);
						if (retitem!=-1)
						{
							setserial(retitem,shoppack2,1);
							items[retitem].x=50+(rand()%80);
							items[retitem].y=50+(rand()%80);
							items[retitem].z=9;
						}
						strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
					} else {
						printf("Warning: Bad NPC Script %d with problem no shoppack2 for item.\n", npcNum);
					}
				}
				else if (!(strcmp("SAYCOLOR",script1))) {
					chars[c].saycolor1=(hstr2num(script2))/256;
					chars[c].saycolor2=(hstr2num(script2))%256;
				}
				else if (!(strcmp("SPEECH",script1))) chars[c].speech=str2num(script2);
				else if (!(strcmp("SPATTACK",script1))) chars[c].spattack=str2num(script2);
				else if (!(strcmp("SPADELAY",script1))) chars[c].spadelay=str2num(script2);
				else if (!(strcmp(script1, "SPLIT"))) chars[c].split=str2num(script2);
				else if (!(strcmp(script1, "SPLITCHANCE"))) chars[c].splitchnc=str2num(script2);
				else if ((!(strcmp("STR",script1)))||(!(strcmp("STRENGTH",script1)))) {
					chars[c].st  = getstatskillvalue(script2);
					chars[c].st2 = chars[c].st;
					chars[c].hp  = chars[c].st;
				}
				else if ((!(strcmp("SNOOPING",script1)))||(!(strcmp("SKILL28",script1)))) chars[c].baseskill[SNOOPING] = getstatskillvalue(script2);
				else if ((!(strcmp("SPIRITSPEAK",script1)))||(!(strcmp("SKILL32",script1)))) chars[c].baseskill[SPIRITSPEAK] = getstatskillvalue(script2);
				else if ((!(strcmp("STEALING",script1)))||(!(strcmp("SKILL33",script1)))) chars[c].baseskill[STEALING] = getstatskillvalue(script2);
				else if ((!(strcmp("SWORDSMANSHIP",script1)))||(!(strcmp("SKILL40",script1)))) chars[c].baseskill[SWORDSMANSHIP] = getstatskillvalue(script2);
				else if (!(strcmp("SKINLIST",script1)))
				{
					pos=ftell(scpfile);
					closescript();
					storeval=addrandomcolor(c,script2);
					chars[c].skin1=(storeval)/256;
					chars[c].skin2=(storeval)%256;
					chars[c].xskin1=chars[c].skin1;
					chars[c].xskin2=chars[c].skin2;
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					strcpy(script1, "DUMMY"); // To prevent accidental exit of loop.
				}
				else if (!(strcmp("SKILL", script1))) 
				{
					gettokennum(script2, 0);
					z=str2num(gettokenstr);
					gettokennum(script2, 1);
					chars[c].baseskill[z]=str2num(gettokenstr);
				}
				else if(!(strncmp(script1, "SKILL", 5)))
				{
					skl = str2num( &script1[5] );
					sklvalue = str2num( script2 );
					chars[c].baseskill[skl] = sklvalue;
				}

				break;

			case 't':
			case 'T':
				if (!(strcmp("TITLE",script1))) strcpy(chars[c].title, script2);
				else if ((!(strcmp("TACTICS",script1)))||(!(strcmp("SKILL27",script1)))) chars[c].baseskill[TACTICS] = getstatskillvalue(script2);
				else if ((!(strcmp("TAILORING",script1)))||(!(strcmp("SKILL34",script1)))) chars[c].baseskill[TAILORING] = getstatskillvalue(script2);
				else if ((!(strcmp("TAMING",script1)))||(!(strcmp("SKILL35",script1)))) chars[c].baseskill[TAMING] = getstatskillvalue(script2);
				else if ((!(strcmp("TASTEID",script1)))||(!(strcmp("SKILL36",script1)))) chars[c].baseskill[TASTEID] = getstatskillvalue(script2);
				else if ((!(strcmp("TINKERING",script1)))||(!(strcmp("SKILL37",script1)))) chars[c].baseskill[TINKERING] = getstatskillvalue(script2);
				else if ((!(strcmp("TRACKING",script1)))||(!(strcmp("SKILL38",script1)))) chars[c].baseskill[TRACKING] = getstatskillvalue(script2);
				else if ((!(strcmp("TOTAME", script1)))||(!(strcmp("TAMING", script1)))) chars[c].taming=str2num(script2);

				break;
			case 'v':
			case 'V':
				if (!(strcmp("VALUE",script1))) if (retitem!=-1) items[retitem].value=(str2num(script2));
				else if ((!(strcmp("VETERINARY",script1)))||(!(strcmp("SKILL39",script1)))) chars[c].baseskill[VETERINARY] = getstatskillvalue(script2);
				break;
			case 'w':
			case 'W':
				if ((!(strcmp("WRESTLING",script1)))||(!(strcmp("SKILL43",script1)))) chars[c].baseskill[WRESTLING] = getstatskillvalue(script2);
				break;
			}
		}
   }
   while (script1[0]!='}');
   closescript();
   
   // Now that we have created the NPC, lets place him
   if (type==1) 
   {
	   if (triggerx)
	   {
		   chars[c].x=triggerx;
		   chars[c].y=triggery;
		   chars[c].dispz=chars[c].z=triggerz;
		   triggerx=c;
	   } else 
	   {
	   /*Zippy's Code chages for area spawns --> (Type 69) xos and yos (X OffSet, Y OffSet) 
	   are used to find a random number that is then added to the spawner's x and y (Using 
	   the spawner's z) and then place the NPC anywhere in a square around the spawner. 
	   This square is random anywhere from -10 to +10 from the spawner's location (for x and 
	   y) If the place chosen is not a valid position (the NPC can't walk there) then a new 
	   place will be chosen, if a valid place cannot be found in a certain # of tries (50), 
		   the NPC will be placed directly on the spawner and the server op will be warned. */
		   
		   if ((items[s].type==69)&&(items[s].contserial==-1))
		   {
			   if (items[s].more3==0) items[s].more3=10;
			   if (items[s].more4==0) items[s].more4=10;
			   
//			   long int pos/*, pos2, length*/;
//			   int x1, x2;
//			   int y1, y2;
			   //signed char z, ztemp, found;
			   
			   do
			   {
				   if (k>=50) //this CAN be a bit laggy. adjust as nessicary
				   {
					   printf("UOX3: Problem area spawner found at [%i,%i,%i]. NPC placed at default location.\n",items[s].x,items[s].y,items[s].z);
					   xos=0;
					   yos=0;
					   break;
				   }
				   xos=RandomNum(-items[s].more3,items[s].more3);
				   yos=RandomNum(-items[s].more4,items[s].more4);
				   k++;
				   
				   if ((items[s].x+xos<1) || (items[s].y+yos<1)) lb=0; 
				   else lb=Movement->validNPCMove(items[s].x+xos,items[s].y+yos,items[s].z,c);
//				   else lb = Movement->CanCharWalk( c, items[s].x + xos, items[s].y + yos, myz );
				   
				   //Bug fix Monsters spawning on water:

				   MapStaticIterator msi(items[s].x + xos, items[s].y + yos);

				   staticrecord *stat;
				   while (stat = msi.Next())
				   {
					   tile_st tile;
					   msi.GetTile(&tile);
					   if(!(strcmp((char *) tile.name, "water")))//Water
					   {//Don't spawn on water tiles... Just add other stuff here you don't want spawned on.
						   lb=0;
					   }
				   }
			   } while (!lb);
		   } // end Zippy's changes (exept for all the +xos and +yos around here....)
		   if (chars[c].fx1==-1)
		   {
			   chars[c].fx1=items[s].x+xos;
			   chars[c].fy1=items[s].y+yos;
			   if (chars[c].fz1!=-1) chars[c].fz1=items[s].z;
		   }
		   chars[c].x=items[s].x+xos;
		   chars[c].y=items[s].y+yos;
		   chars[c].dispz=chars[c].z=items[s].z;
		   setserial(c,s,6);
	   } // end of if !triggerx && type==1
   } // if type == 1 
   else 
   { // if type == 0
	   if( s!=-1)
	   {
		   if (chars[c].fx1==-1)
		   {
			   chars[c].fx1=(buffer[s][11]<<8)+buffer[s][12]+xos;
			   chars[c].fy1=(buffer[s][13]<<8)+buffer[s][14]+yos;
			   if (chars[c].fz1!=-1) chars[c].fz1=buffer[s][16]+Map->TileHeight((buffer[s][17]<<8)+buffer[s][18]);
		   }
//		   chars[c].x=(buffer[s][11]<<8)+buffer[s][12]+xos;
//		   chars[c].y=(buffer[s][13]<<8)+buffer[s][14]+yos;
//		   chars[c].dispz=chars[c].z=buffer[s][16]+Map->TileHeight(buffer[s][17]<<8+buffer[s][18]);
	   }
   }
   
   chars[c].region=calcRegionFromXY(chars[c].x, chars[c].y);
   
   //Now find real 'skill' based on 'baseskill' (stat modifiers)
   for(z=0;z<TRUESKILLS;z++)
   {
	   Skills->updateSkillLevel(c,z);
   }
   
   if (donpcupdate==0) 
   {
	   updatechar(c);
   }
   //Char mapRegions
   mapRegions->RemoveItem(c+1000000);
   mapRegions->AddItem(c+1000000);
   setcharflag( c );
   return c;
}

int cCharStuff::Split(int k) // For NPCs That Split during combat
{
	int c,serial,z;
	
	c=MemCharFree ();
	
	InitChar(c);
	serial=chars[c].serial;
	memcpy(&chars[c],&chars[k],sizeof(char_st));
	chars[c].ser1 = (unsigned char)(serial>>24);
	chars[c].ser2 = (unsigned char)(serial>>16);
	chars[c].ser3 = (unsigned char)(serial>>8);
	chars[c].ser4 = (unsigned char)(serial%256);
	chars[c].serial=serial;
	chars[c].ftarg=-1;
	mapRegions->RemoveItem(c+1000000);
	chars[c].x=chars[k].x+1;
	chars[c].y=chars[k].y;
	mapRegions->AddItem(c+1000000);
	chars[c].kills=0;
	chars[c].hp=chars[k].st;
	chars[c].stm=chars[k].dx;
	chars[c].mn=chars[k].in;
	z=rand()%35;
	if (z==5) chars[c].split=1; else chars[c].split=0;	
	updatechar(c);
	return -1;
}

int cCharStuff::FindItem( CHARACTER toFind, unsigned char type )
// PRE:		CharStuff exists, toFind exists
// POST:	If item exists anywhere in their pack with type type or its on their body, return it
//			else return -1
{
	int serial = chars[toFind].serial;
	int serhash = serial%HASHMAX;
	int counter = 0;
	int packSearchResult = -1;

	ITEM toCheck;
	for( counter = 0; counter < contsp[serhash].max; counter++ )
	{
		toCheck = contsp[serhash].pointer[counter];
		if( toCheck != -1 && items[toCheck].contserial == serial )
		{
			if( items[toCheck].type == type )	// it's in our hand
			{
				return toCheck;					// we've found the first occurance on the person!
			}
			else if( items[toCheck].layer == 0x15 )	// could use packitem, but we're already in the same type of loop, so we'll check it ourselves
			{
				packSearchResult = SearchSubPackForItem( toCheck, type );
				if( packSearchResult != -1 )
				{
					return packSearchResult;
				}
			}
		}
	}
	// if we haven't hit it by now, we won't hit it at all
	return -1;
}

int cCharStuff::SearchSubPackForItem( ITEM toSearch, unsigned char type )
// PRE:		CharStuff exists, toFind exists
// POST:	If item with type type exists anywhere in the specified pack or on their body, return it
//			else return -1
{
	int serial = items[toSearch].serial;
	int serhash = serial%HASHMAX;
	int counter = 0;
	int packSearchResult = -1;

	ITEM toCheck;
	for( counter = 0; counter < contsp[serhash].max; counter++ )
	{
		toCheck = contsp[serhash].pointer[counter];
		if( toCheck != -1 && items[toCheck].contserial == serial )
		{
			if( items[toCheck].type == type )	// it's in our hand
			{
				return toCheck;					// we've found the first occurance on the person!
			}
			else if( items[toCheck].type == 1 || items[toCheck].type == 8 )	// search any subpacks, specifically pack and locked containers
			{ 
				packSearchResult = SearchSubPackForItem( toCheck, type );
				if( packSearchResult != -1 )
				{
					return packSearchResult;
				}
			}
		}
	}
	// if we haven't hit it by now, we won't hit it at all
	return -1;
}

int cCharStuff::FindItem( CHARACTER toFind, unsigned char id1, unsigned char id2 )
// PRE:		CharStuff exists, toFind exists
// POST:	If item exists anywhere in their pack with id id1 id2 or its on their body, return it
//			else return -1
{
	int serial = chars[toFind].serial;
	int serhash = serial%HASHMAX;
	int counter = 0;
	int packSearchResult = -1;

	ITEM toCheck;
	for( counter = 0; counter < contsp[serhash].max; counter++ )
	{
		toCheck = contsp[serhash].pointer[counter];
		if( toCheck != -1 && items[toCheck].contserial == serial )
		{
			if( items[toCheck].id1 == id1 && items[toCheck].id2 == id2 )	// it's in our hand
			{
				return toCheck;					// we've found the first occurance on the person!
			}
			else if( items[toCheck].layer == 0x15 )	// could use packitem, but we're already in the same type of loop, so we'll check it ourselves
			{
				packSearchResult = SearchSubPackForItem( toCheck, id1, id2 );
				if( packSearchResult != -1 )
				{
					return packSearchResult;
				}
			}
		}
	}
	// if we haven't hit it by now, we won't hit it at all
	return -1;
}

int cCharStuff::SearchSubPackForItem( ITEM toSearch, unsigned char id1, unsigned char id2 )
// PRE:		CharStuff exists, toFind exists
// POST:	If item with id id1 id2 exists anywhere in the specified pack or on their body, return it
//			else return -1
{
	int serial = items[toSearch].serial;
	int serhash = serial%HASHMAX;
	int counter = 0;
	int packSearchResult = -1;

	ITEM toCheck;
	for( counter = 0; counter < contsp[serhash].max; counter++ )
	{
		toCheck = contsp[serhash].pointer[counter];
		if( toCheck != -1 && items[toCheck].contserial == serial )
		{
			if( items[toCheck].id1 == id1 && items[toCheck].id2 == id2 )	// it's in our hand
			{
				return toCheck;					// we've found the first occurance on the person!
			}
			else if( items[toCheck].type == 1 || items[toCheck].type == 8 )	// search any subpacks, specifically pack and locked containers
			{ 
				packSearchResult = SearchSubPackForItem( toCheck, id1, id2 );
				if( packSearchResult != -1 )
				{
					return packSearchResult;
				}
			}
		}
	}
	// if we haven't hit it by now, we won't hit it at all
	return -1;
}

void npcaction( CHARACTER npc, int x ) // NPC character does a certain action
{
	unsigned int i;
	
	doact[1] = chars[npc].ser1;
	doact[2] = chars[npc].ser2;
	doact[3] = chars[npc].ser3;
	doact[4] = chars[npc].ser4;
	doact[5] = x>>8;
	doact[6] = x%256;
	for( i = 0; i < now; i++ ) 
		if( perm[i] && inrange1p( currchar[i], npc ) ) 
			Network->xSend( i, doact, 14, 0 );
}

void restockNPC(unsigned int currenttime, int i)
{
	int a, b, c, ci, tmp;
	
	if (chars[i].shop != 1 || !chars[i].npc)
		return;
	
	if (!(shoprestocktime <= currenttime || (overflow)))
		return;
	
	int hash, hashI;
	
	hash = chars[i].serial%HASHMAX;
	for (a = 0; a < contsp[hash].max; a++)
	{
		ci = contsp[hash].pointer[a];
		if (ci != -1 && items[ci].contserial == chars[i].serial && items[ci].layer == 0x1A)
		{
			hashI = items[ci].serial%HASHMAX;
			for (b = 0; b < contsp[hashI].max; b++)
			{
				c = contsp[hashI].pointer[b];
				if (c != -1 && items[c].contserial == items[ci].serial && items[c].restock)
				{
					tmp = min(items[c].restock, (items[c].restock/2) + 1);
					items[c].amount += tmp;
					items[c].restock -= tmp;
					// MAgius(CHE): All items in shopkeeper need a new randomvaluerate.
					if (server_data.trade_system == 1) 
						StoreItemRandomValue(c, calcRegionFromXY(chars[i].x, chars[i].y));
				}
			}// for b
		}
	}// for a
}

void checkNPC(int i, int currenttime)// Char mapRegions
{
	int pcalc;
	char t[120];
	
	Npcs->CheckAI(currenttime, i);// Lag fix
	Movement->NpcMovement(currenttime, i);// Lag fix
	setcharflag(i);		// possibly not...
	
	if (!chars[i].dead && chars[i].swingtarg==-1)
		Combat->DoCombat(i, currenttime);
	else if (!chars[i].dead &&(chars[i].swingtarg >= 0 && chars[i].timeout <= currenttime))
		Combat->CombatHit(i, chars[i].swingtarg, currenttime);
	
	Magic->CheckFieldEffects2(currenttime, i, 0);// Lag fix
	
	restockNPC(currenttime, i);
	
	if (!chars[i].free) // bud
	{
		if ( chars[i].disabled>0  && ( chars[i].disabled <= currenttime || overflow ) )
		{
			chars[i].disabled = 0;
		}
		if (chars[i].summontimer <= currenttime || (overflow))
		{
			if (chars[i].summontimer>0)
			{
				// Dupois - Added Dec 20, 1999
				// QUEST expire check - after an Escort quest is created a timer is set
				// so that the NPC will be deleted and removed from the game if it hangs around
				// too long without every having its quest accepted by a player so we have to remove 
				// its posting from the messageboard before icing the NPC
				// Only need to remove the post if the NPC does not have a follow target set
				if ((chars[i].questType == ESCORTQUEST) &&(chars[i].ftarg == -1))
				{
					MsgBoardQuestEscortRemovePost(i);
					MsgBoardQuestEscortDelete(i);
					return;
				}
				// Dupois - End
				soundeffect2(i, 0x01, 0xFE);
				chars[i].dead = 1;
				Npcs->DeleteChar(i);
				return;
			}
		}
	}
	
	if ( chars[i].fleeat == 0 )
		chars[i].fleeat = NPC_BASE_FLEEAT;

	if ( chars[i].reattackat == 0 )
		chars[i].reattackat = NPC_BASE_REATTACKAT;
	
	if (!(chars[i].npcWander == 5)&&
		(chars[i].hp < chars[i].st*chars[i].fleeat/100))
	{
		chars[i].oldnpcWander = chars[i].npcWander;
		chars[i].npcWander = 5;
		chars[i].npcmovetime = (unsigned int)((uiCurrentTime + double(NPCSPEED*CLOCKS_PER_SEC)));
	}
	
	if ((chars[i].npcWander == 5)&&
		(chars[i].hp>chars[i].st*chars[i].reattackat/100))
	{
		chars[i].npcWander = chars[i].oldnpcWander;
		chars[i].npcmovetime = (unsigned int)((uiCurrentTime + double(NPCSPEED*CLOCKS_PER_SEC)));
		chars[i].oldnpcWander = 0; // so it won't save this at the wsc file
	}
	// end of flee code
	
	// new poisoning code, Lord Binary
	if (chars[i].poisoned && !(chars[i].priv&4))
	{
		if ((chars[i].poisontime <= currenttime) || (overflow))
		{
			if (chars[i].poisonwearofftime>currenttime) // lb, makes poison wear off pc's
			{
				switch (chars[i].poisoned)
				{
					case 1:
						chars[i].poisontime = (unsigned int)(currenttime +(5 * CLOCKS_PER_SEC));
						if ((chars[i].poisontxt <= currenttime) || (overflow))
						{
							chars[i].poisontxt = (unsigned int)(currenttime +(10 * CLOCKS_PER_SEC));
							sprintf(t, "* %s looks a bit nauseous *", chars[i].name);
							chars[i].emotecolor1 = 0x00;
							chars[i].emotecolor2 = 0x26;
							npcemoteall(i, t, 1);
						}
						chars[i].hp = chars[i].hp - RandomNum(1, 2);
						updatestats(i, 0);
						break;
					case 2:
						chars[i].poisontime = (unsigned int)(currenttime +(4 * CLOCKS_PER_SEC));
						if ((chars[i].poisontxt <= currenttime) || (overflow))
						{
							chars[i].poisontxt = (unsigned int)(currenttime +(10 * CLOCKS_PER_SEC));
							sprintf(t, "* %s looks disoriented and nauseous! *", chars[i].name);
							chars[i].emotecolor1 = 0x00;
							chars[i].emotecolor2 = 0x26;
							npcemoteall(i, t, 1);
						}
						pcalc = (chars[i].hp*RandomNum(2, 5)/100) + RandomNum(0, 2); // damage: 1..2..5% of hp's+ 1..2 constant
						chars[i].hp -= pcalc;
						updatestats(i, 0);
						break;
					case 3:
						chars[i].poisontime = (unsigned int)(currenttime +(3 * CLOCKS_PER_SEC));
						if ((chars[i].poisontxt <= currenttime) || (overflow))
						{
							chars[i].poisontxt = (unsigned int)(currenttime +(10 * CLOCKS_PER_SEC));
							sprintf(t, "* %s is in severe pain! *", chars[i].name);
							chars[i].emotecolor1 = 0x00;
							chars[i].emotecolor2 = 0x26;
							npcemoteall(i, t, 1);
						}
						pcalc = (chars[i].hp/10) + RandomNum(1, 3); // damage: 5..10% of hp's+ 1..2 constant
						chars[i].hp -= pcalc;
						updatestats(i, 0);
						break; // lb !!!
					case 4:
						chars[i].poisontime = (unsigned int)(currenttime +(3 * CLOCKS_PER_SEC));
						if ((chars[i].poisontxt <= currenttime) || (overflow))
						{
							chars[i].poisontxt = (unsigned int)(currenttime +(10 * CLOCKS_PER_SEC));
							sprintf(t, "* %s looks extremely weak and is wrecked in pain! *", chars[i].name);
							chars[i].emotecolor1 = 0x00;
							chars[i].emotecolor2 = 0x26;
							npcemoteall(i, t, 1);
						}
						pcalc = (chars[i].hp/5) + RandomNum(3, 6); // damage: 20% of hp's+ 3..6 constant, quite deadly <g>
						chars[i].hp -= pcalc;
						updatestats(i, 0);
						break;
					default:
						printf("ERROR: Fallout of switch statement without default. uox3.cpp, checkNPC()\n"); // Morrolan
						chars[i].poisoned = 0;
						return;
				}
				if (chars[i].hp < 1)
				{
					deathstuff(i);
				} 
			} // end switch
		}  // end if poison-wear off-timer
	} // end if poison-damage timer
	
	if (chars[i].poisoned && chars[i].poisonwearofftime <= currenttime)
	{
		chars[i].poisoned = 0;
		impowncreate(calcSocketFromChar(i), i, 1); // updating to blue stats-bar ...
	}	
}
