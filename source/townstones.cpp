//------------------------------------------------------------------------
//  townstones.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by unknown real name (Krozy)
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

#include "uox3.h"
#include "debug.h"
#define DBGFILE "uox3.cpp"


//BEGIN TOWNSTONE FUNCTIONS. LAST UPDATED ON AUG 26, 1998
//PLEASE DO NOT MODIFY THESE FUNCTIONS.  I (Krozy) will add functionality
//to them.  I have the plans for townstones worked out, and would
//appreciate it if nobody 'ruined' it by making changes.  In the event I
//do not update the townstone code over a period of 45 days, then you
//may assume that I have left the uox project. -Krozy

// Abaddon and Daemar will be taking over maintenance of this in the next few months
// We have a very solid set idea as to the way it is heading, so please do not change
// anything that you may see here, unless you can help it!

cTownStones::~cTownStones()
{
}

//void towninit(void){
cTownStones::cTownStones()
{
	int a,b,c;
	struct tm *newtime;
	time_t aclock;
	time(&aclock);
	newtime = localtime(&aclock);
	
	//get real life day of month
	b = newtime->tm_mday;
	//get real weekday
	c = newtime->tm_wday;
	
	//repair any messed up npcs!
	for(a=0;a<charcount;a++) {
		if(chars[a].npc) {
			chars[a].town=0;
			chars[a].townvote1=0;
			chars[a].townvote2=0;
			chars[a].townvote3=0;
			chars[a].townvote4=0;
			chars[a].towntitle=0;
			chars[a].townpriv=0;
		}
	}
	
	for(a=0;a<itemcount;a++){
		if ((items[a].id1==0x0E)&&((items[a].id2==0xDD)||(items[a].id2==0xDE))) {
			if((items[a].morey!=b)&&(items[a].morez==c)) {
				//Weekday is the same, but its not the same day of the month, so
				//a week must have passed.
				items[a].morey=b;
				items[a].morez=c;
				Towns->CalcNewMayor(calcRegionFromXY(items[a].x, items[a].y));
			}
		}
	}
}

void cTownStones::CalcNewMayor(int j)
{
	int a, b, c = 0, d = 0, e = 0;
	struct tm *newtime;
	time_t aclock;
	newtime = localtime(&aclock);
	for(a=0;a<charcount;a++) {
		if(chars[a].town==0) {
			chars[a].townpriv=0;
			chars[a].towntitle=0;
		}
		if(chars[a].town==j) {
			if(chars[a].townpriv==2) chars[a].townpriv=1; //reset current mayor.
			for(b=0;b<charcount;b++) {
				if((chars[b].town==j)&&
					(chars[b].townvote1==chars[a].ser1)&&
					(chars[b].townvote2==chars[a].ser2)&&
					(chars[b].townvote3==chars[a].ser3)&&
					(chars[b].townvote4==chars[a].ser4)) {
					c++; //c is the number of votes that chars[a] has.
				}
			}
			if(c>d){ //if number of votes is higher then max then
				d=c; //set new max votes
//				e=((chars[a].ser1<<24)+(chars[a].ser2<<16)+(chars[a].ser3<<8)+(chars[a].ser4));
				e = calcserial(chars[a].ser1, chars[a].ser2, chars[a].ser3, chars[a].ser4);
			}
		}
	}
	//Handle wilderness Townstones
	if( !strcmp( Towns->TownName( j, 3 ), "the wilderness" ) )
		e = 0;
	
	//Set all townstones in this region to have the new mayor.
	c=newtime->tm_mday; //we're going to set all townstones in this region
	d=newtime->tm_wday; //to the most recent update time.
	for(a=0;a<itemcount;a++) {
		if ((items[a].id1==0x0E)&&((items[a].id2==0xDD)||(items[a].id2==0xDE))) {
			b=calcRegionFromXY(items[a].x, items[a].y);
			if(b==j) {
				items[a].morex=e;
				items[a].morey=c;
				items[a].morez=d;
			}
		}
	}
	//Set the Town privledges of the new mayor to '2'.
	a = calcCharFromSer( e );
	if( a != -1 )
	{
		chars[a].townpriv = 2;
		chars[a].towntitle = 1;
	}
}

const char *cTownStones::TownName(int x, int type)
{
	char *s;
	const char checkword[]="of ";
	int calcreg,len;
	//checkword="of ";
	len = strlen(checkword);
	if (type==1) { //GET TOWNNAME FROM THIS OBJECT.
		calcreg=calcRegionFromXY(items[x].x, items[x].y);
	} else
		if (type==2) { //GET TOWNNAME FROM THIS CHARACTER (GM /townname COMMAND).
			calcreg=calcRegionFromXY(chars[currchar[x]].x, chars[currchar[x]].y);
		} else {
			calcreg=x;    //GET TOWNNAME FROM THIS REGION (ALL OTHERS)
		}
		strcpy(temp, region[calcreg].name);
		if ((s = strstr(temp,checkword))!= NULL) {
			for (len=len;len > 0;len--) s++;
		} else {
			return "the wilderness";
		}
   return s;
}

const char *cTownStones::TownMayor(int j)
{
	const char *s = NULL;
	int a;
	if(items[j].morex==0) {
		s= "unruled region";
	} else {
		a = calcCharFromSer( items[j].morex );
		if( a != -1 )
			s = strstr( chars[a].name, chars[a].name );
	}
	return s;
}

const char * cTownStones::MayorVote(int s)
{
	int i;
	const char *t = "none";

	unsigned char s1 = chars[currchar[s]].townvote1;
	unsigned char s2 = chars[currchar[s]].townvote2;
	unsigned char s3 = chars[currchar[s]].townvote3;
	unsigned char s4 = chars[currchar[s]].townvote4;

    long int serial = calcserial( s1, s2, s3, s4 );
	i = findbyserial(&charsp[serial%HASHMAX],serial,1); // lets find the new position of the old_serial in newly ordered array

	if( i != -1 )
	{
		t = chars[i].name;
	}
/*	for (i=0;i<charcount;i++) {
		if ((chars[i].ser1==chars[currchar[s]].townvote1)&&
      (chars[i].ser2==chars[currchar[s]].townvote2)&&
      (chars[i].ser3==chars[currchar[s]].townvote3)&&
      (chars[i].ser4==chars[currchar[s]].townvote4)) {
			t=chars[i].name;
		}
	}*/
	return t;
}

int cTownStones::Population(int j)
{
	int b=0;
	unsigned int a;
	for (a=0;a<charcount;a++) {
		if(chars[a].town==j) b++;
	}
	if( !strcmp( Towns->TownName( j, 3 ), "the wilderness" ) )
		b = -1;
	return b;
}

void cTownStones::VoteForMayorTarget(int s)
{
  int i,serial,serhash,ci;
  char temp2[512];
   
	
  serial=calcserial(buffer[s][7], buffer[s][8], buffer[s][9], buffer[s][10]);
  serhash=serial%HASHMAX;
  for (ci=0;ci<charsp[serhash].max;ci++)
  {
    i=charsp[serhash].pointer[ci];
	if (i!=-1)
    if ((chars[i].serial==serial))
    {
      if(chars[i].town==chars[currchar[s]].town)
      {
        chars[currchar[s]].townvote1=buffer[s][7];
        chars[currchar[s]].townvote2=buffer[s][8];
        chars[currchar[s]].townvote3=buffer[s][9];
        chars[currchar[s]].townvote4=buffer[s][10];
        sprintf(temp2, "You have voted for %s to be Mayor of %s", chars[i].name, Towns->TownName(chars[i].town,3));
        sysmessage(s, temp2);
      }
      else
      {
        sysmessage(s, "The person you have voted for is not of your town.");
      }
      return;
    }
  }
  sysmessage(s,"That is not a person.");
}

void cTownStones::Line(int line, int j, char type, int s)
// PARAM WARNING: s is an unreferenced parameter
{
	//type will be used for privledges by player (non, resident, mayor, etc..)
	//0 = nonresident, 1 = resident, 2 = mayor, 3 = town council, 4 = guard,
	//5 = guard captain, 6 = merchant, 7 = head merchant, 8 = clergy
	line--; if (line==0) strcpy(script1, "nomove");
	line--; if (line==0) strcpy(script1, "noclose");
	line--; if (line==0) strcpy(script1, "page 0");
	line--; if (line==0) strcpy(script1, "resizepic 0 0 5120 290 350");    //The background
	line--; if (line==0) strcpy(script1, "button 20 310 2130 2129 1 0 1"); //OKAY
	line--; if (line==0) strcpy(script1, "text 80 310 500 0");            //Phase info
	line--; if (line==0) strcpy(script1, "page 1");
	line--; if (line==0) strcpy(script1, "text 20 10 500 1");   //Townstone for <townname>
	line--; if (line==0) strcpy(script1, "gumppic 10 50 1141"); //Mayor
	line--; if (line==0) strcpy(script1, "text 20 51 0 2");
	line--; if (line==0) strcpy(script1, "text 120 51 0 3");
	line--; if (line==0) strcpy(script1, "gumppic 10 70 1141"); //Population
	line--; if (line==0) strcpy(script1, "text 20 71 0 4");
	line--; if (line==0) strcpy(script1, "text 120 71 0 5");
	line--; if (line==0) strcpy(script1, "button 10 120 2151 2152 1 0 2");  //Leave or Join
	line--; if (line==0) strcpy(script1, "text 50 125 500 6");
	if(type>0) {
		line--; if (line==0) strcpy(script1, "button 10 150 2151 2152 1 0 3"); //Toggle Town Title
		line--; if (line==0) strcpy(script1, "text 50 155 500 7");
		if( strcmp( Towns->TownName( j, 1 ), "the wilderness" ) )
		{
			line--; if(line==0) strcpy(script1, "button 10 180 2151 2152 1 0 4"); //Vote for Mayor
			line--; if(line==0) strcpy(script1, "text 50 185 500 8");
			line--; if(line==0) strcpy(script1, "text 50 205 500 9");
			line--; if(line==0) strcpy(script1, "text 50 225 500 10");
		}
	}
	line--; if (line==0) strcpy(script1, "}"); //Terminate it
}

void cTownStones::Text(int line, int j, char type, int s)
{
	//type will be used for privledges by player (non, resident, mayor, etc..)
	//0 = nonresident, 1 = resident, 2 = mayor, 3 = town council, 4 = guard,
	//5 = guard captain, 6 = merchant, 7 = head merchant, 8 = clergy
	char town[80];
	int townpop;
	strcpy(town, Towns->TownName(j,1));
	townpop=Towns->Population(calcRegionFromXY(items[j].x, items[j].y));
	line--; if (line==0) strcpy(script1,"Townstones Phase 1 of 7");
	line--; if (line==0) sprintf(script1,"Townstone for %s", town);
	line--; if (line==0) strcpy(script1,"Mayor");
	//line--; if (line==0) strcpy(script1, townmayor(calcRegionFromXY(items[j].x, items[j].y)));
	line--; if (line==0) strcpy(script1, Towns->TownMayor(j));
	line--; if (line==0) strcpy(script1,"Population");
	if(townpop>=0) {
		line--; if (line==0) sprintf(script1,"%i", townpop);
	} else {
		line--; if (line==0) strcpy(script1,"unknown");
	}
	line--; if (line==0) {
		if( strcmp( Towns->TownName( j, 1 ), "the wilderness" ) )
		{
			if(type<1) {
				sprintf(script1, "Become a resident of %s", Towns->TownName(j,1));
			} else {
				sprintf(script1, "Leave residency of %s", Towns->TownName(j,1));
			}
		} else {
			if(type<1) {       //Non-Residents get 1 option.
				strcpy(script1, "Become a resident of the wild.");
			} else {           //Residency option #1
				strcpy(script1, "Leave residency of the wild.");
			}
		}
	}
	if (type>0) {        //List all options for residents.
		line--; if (line==0) {
			if(chars[currchar[s]].towntitle) {
				strcpy(script1, "Toggle Town Title off");
			} else {
				strcpy(script1, "Toggle Town Title on");
			}
		}
		if( strcmp( Towns->TownName( j, 1 ), "the wilderness" ) )
		{
			line--; if (line==0) strcpy(script1, "Vote for Mayor");
			line--; if (line==0) strcpy(script1, "Currently voting for:");
			line--; if (line==0) strcpy(script1, Towns->MayorVote(s));
		}
	}
	line--; if (line==0) strcpy(script1,"}"); //Terminate it
}

void cTownStones::Menu(int s, int j, int type)
{
	char sect[512];
	short int length, length2, textlines;
	int i;
	int line;
	int townpriv=0;
	
	//Set the townpriv dependent upon their privledges and their town vs. the
	//region of the stone.  If they are not a member of this town or any town
	//they are considered a 'non-resident', which holds the value of 0, which
	//has been defined above.
	//Currently this doesnt check for all wilderness areas as one.. but..
	//since wilderness people can only be a wilderness resident or toggle
	//their title, it doesnt mean much since if their region doesn't match,
	//they can change to the region of the stone they are using, and set their
	//title again.
	if((calcRegionFromXY(items[j].x,items[j].y))==(chars[currchar[s]].town)) {
		//They are a resident of this Townstone, set townpriv to their priv.
		townpriv=chars[currchar[s]].townpriv;
	}
	
	length=21;
	length2=1;
	line=0;
	do
	{
		line++;
		Towns->Line(line, j, townpriv, s);
		if (script1[0]!='}')
		{
			length += (short int)strlen(script1)+4;
			length2 += (short int)strlen(script1)+4;
		}
	}
	while (script1[0]!='}');
	length+=3;
	textlines=0;
	line=0;
	do
	{
		line++;
		Towns->Text(line, j, townpriv, s);
		if (script1[0]!='}')
		{
			length+=(strlen(script1)*2)+2;
			textlines++;
		}
	}
	while (script1[0]!='}');
	gump1[1]=length>>8;
	gump1[2]=length%256;
	//type3
	gump1[3]=items[j].ser1;
	gump1[4]=items[j].ser2;
	gump1[5]=items[j].ser3;
	gump1[6]=items[j].ser4;
	//type3
	gump1[7]=0;
	gump1[8]=0;
	gump1[9]=0;
	gump1[10] = (char)type; // Gump Number
	gump1[19] = (char)length2>>8;
	gump1[20] = (char)length2%256;
	Network->xSend(s, gump1, 21, 0);
	line=0;
	do
	{
		line++;
		Towns->Line(line, j, townpriv, s);
		if (script1[0]!='}')
		{
			sprintf(sect, "{ %s }", script1);
			Network->xSend(s, sect, strlen(sect), 0);
		}
	}
	while (script1[0]!='}');
	gump2[1]=textlines>>8;
	gump2[2]=textlines%256;
	Network->xSend(s, gump2, 3, 0);
	line=0;
	do
	{
		line++;
		Towns->Text(line, j, townpriv, s);
		if (script1[0]!='}')
		{
			gump3[0]=strlen(script1)>>8;
			gump3[1]=strlen(script1)%256;
			Network->xSend(s, gump3, 2, 0);
			gump3[0]=0;
			for (i=0;i<strlen(script1);i++)
			{
				gump3[1]=script1[i];
				Network->xSend(s, gump3, 2, 0);
			}
		}
	}
	while (script1[0]!='}');
}

//END TOWNSTONE FUNCTIONS.-Krozy
