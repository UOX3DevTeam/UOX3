//------------------------------------------------------------------------
//  newbie.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 2000 - 2001 by Jeremy Labit (Thyme)
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
//Newbie Stuff Code -- By McCleod

#include <uox3.h>
#include <debug.h>
#include <algorithm>

#define DBGFILE "newbie.cpp"

// Newbie code redone by Thyme
// Thanks for letting me learn a little about STL!

class cSkillClass
{
public:
	int skill;
	int value;

	cSkillClass(int nskill, int nvalue) : skill(nskill), value(nvalue) {}

};


bool operator==(const cSkillClass& x, const cSkillClass& y)
{
    return ( x.value == y.value );
}

bool operator<(const cSkillClass& x, const cSkillClass& y)
{
	return ( ( x.value == y.value ) ? ( x.skill < y.skill ) : ( x.value < y.value ) );
}

bool operator>(const cSkillClass& x, const cSkillClass& y)
{
	return ( ( x.value == y.value ) ? ( x.skill > y.skill ) : ( x.value > y.value ) );
}

void addnewbieitem( UOXSOCKET s, CHARACTER c, char* str)
{

	char sect[512];
	long int pos;
	UOXSOCKET socket = s;

#if DEBUG
	ConOut("addnewbieitem: %s %s\n", chars[c].name, str);
#endif

	openscript("newbie.scp");
	strcpy(sect, str);
	if (!i_scripts[newbie_script]->find(sect)) 
	{
		closescript();
		return;
	}
	do
	{
		read2();
		if (script1[0]!='}')
		{ 
			if (!(strcmp("PACKITEM",script1)))
			{
				unsigned int storeval=str2num(script2);
				pos=ftell(scpfile);
				closescript();
					unsigned int n=Items->SpawnItemBackpack2( socket, c , storeval, 0); // Tauriel 11-24-98
				if (n!=-1) 
					items[n].priv=items[n].priv|0x02; //Mark as a newbie item
				strcpy(script1, "DUMMY");
				openscript("newbie.scp");
				fseek(scpfile, pos, SEEK_SET);
			}
		}  
	}
	while (script1[0]!='}');
	closescript();

}

void newbieitems(unsigned int c)
{
	vector<cSkillClass> nSkills;
	char whichsect[15];
	int i;
	for ( i = 0 ; i < TRUESKILLS ; i++ )
		nSkills.push_back(cSkillClass(i, chars[c].baseskill[i]));

	sort(nSkills.rbegin(), nSkills.rend());

	UOXSOCKET socket = calcSocketFromChar( c );
	for ( i = 0 ; i < 3 ; i++ )
	{
		if ( nSkills[i].value > 0 )
		{
			sprintf( whichsect, "BESTSKILL %i", nSkills[i].skill );
			addnewbieitem( socket, c, whichsect );
		}
		else
			continue;
	}
	addnewbieitem( socket, c, "DEFAULT");
}

