/*

  Ultima Offline eXperiment III (UOX3)
  UO Server Emulation Program
  
  Copyright 1997 - 2001 by Marcus Rating (Cironian)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
	  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
	
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
		  
  * In addition to that license, if you are running this program or modified  *
  * versions of it on a public system you HAVE TO make the complete source of *
  * the version used by you available or provide people with a location to    *
  * download it.                                                              *
			
  You can contact the author by sending email to <cironian@stratics.com>
			  
*/

#ifdef __LINUX__
#include <errno.h>
#include <signal.h>
typedef void *HANDLE;
#endif


#include <uox3.h>
#include <cmdtable.h>
#include <debug.h>

#define __CLUOX__

//------------------------------------------------------------
#ifdef _BORLAND_
USEUNIT("admin.cpp");
USEUNIT("archive.cpp");
USEUNIT("boats.cpp");
USEUNIT("cmdtable.cpp");
USEUNIT("combat.cpp");
USEUNIT("commands.cpp");
USEUNIT("cRaces.cpp");
USEUNIT("crypt.cpp");
USEUNIT("cWeather.cpp");
USEUNIT("dbl_sinlge_click.cpp");
USEUNIT("debug.cpp");
USEUNIT("fileio.cpp");
USEUNIT("globals.cpp");
USEUNIT("guildstones.cpp");e
USEUNIT("gumps.cpp");
USEUNIT("house.cpp");
USEUNIT("html.cpp");
USEUNIT("im.cpp");
USEUNIT("items.cpp");
USEUNIT("magic.cpp");
USEUNIT("mapstuff.cpp");
USEUNIT("msgboard.cpp");
USEUNIT("necro.cpp");
USEUNIT("Network.cpp");
USEUNIT("newbie.cpp");
USEUNIT("npcs.cpp");
USEUNIT("p_ai.cpp");
USEUNIT("pointer.cpp");
USEUNIT("regions.cpp");
USEUNIT("scriptc.cpp");
USEUNIT("shop.cpp");
USEUNIT("skills.cpp");
USEUNIT("targeting.cpp");
USEUNIT("townstones.cpp");
USEUNIT("trigger.cpp");
USEUNIT("walking.cpp");
USEUNIT("weight.cpp");
USEUNIT("worldmain.cpp");
USEUNIT("typedefs.h");
USEUNIT("xgm.cpp");
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

#define DBGFILE "uox3.cpp"

void PlayerAttack( UOXSOCKET s );
void aus(int signal)
// PARAM WARNING: signal never used
{
	printf("Server crash averted! Floating point exception caught.\n");
} 

#ifdef __NT__
///////////////////

HANDLE hco;
CONSOLE_SCREEN_BUFFER_INFO csbi;


void gotoxy(int x, int y)
{
	COORD xy;
	
	xy.X=x;
	xy.Y=y;
	SetConsoleCursorPosition(hco, xy);
}

void clearscreen( void )
{
	unsigned long int y;
	COORD xy;
	
	xy.X=0;
	xy.Y=0;
	FillConsoleOutputCharacter(hco, ' ', 80*25, xy, &y);
	SetConsoleCursorPosition(hco, xy);
}

void constart( void )
{
	hco=GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hco, &csbi);
}

#endif

#ifdef __CLUOX__
bool cluox_io          = false;   // is cluox-IO enabled?
bool cluox_nopipe_fill = false;   // the stdin-pipe is known to be none-empty, no need to fill it.
HANDLE cluox_stdin_writeback = 0; // the write-end of the stdin-pipe

//o---------------------------------------------------------------------------o
//|	Function	-	cl_getch
//|	Programmer	-	knoxos
//o---------------------------------------------------------------------------o
//|	Purpose		-	Read a character from stdin, in a cluox compatble way.
//|                 This routine is non-blocking!
//|	Returns		-	>0 -> character read
//|                 -1 -> no char aviable.
//o---------------------------------------------------------------------------o
//
// now cluox is GUI wrapper over uox using stdin and stdout redirection to capture
// the console, if it is active uox can't use kbhit() to determine if there is a 
// character aviable, it can only get one directly by getch().
// However the problem arises that uox will get blocked if none is aviable.
// The solution to this problem is that cluox also hands over the second pipe-end
// of stdin so uox can write itself into this stream. To determine if a character is 
// now done that way. UOX write's itself a ZERO on the other end of the pipe, and reads
// a charecter, if it is again the same ZERO just putted in nothing was entered. However
// it is not a zero the user has entered a char.
// 
int cl_getch( void )
{
#ifdef __LINUX__
	// first the linux style, don't change it's behavoir
	int s;
	unsigned char c = 0;
	fd_set  KEYBOARD;
	FD_ZERO( &KEYBOARD );
	FD_SET( 0, &KEYBOARD );
	s = select( 1, &KEYBOARD, NULL, NULL, &uoxtimeout );
	if( s < 0 )
	{
		printf("Error scanning key press\n" );
		Shutdown( 10 );
	}
	if( s > 0 )
	{
		read( 0, &c, 1 );
		if( c == 0x0a )
		{ 
			return -1;
		}
	}
#elif defined(__NT__)
	// now the windows one
	if( !cluox_io ) 
	{
		// uox is not wrapped simply use the kbhit routine
		if( kbhit() )
		{
			return getch();
		} 
		else 
		{
			return -1;
		}
	}
	// the wiered cluox getter.
	unsigned char c = 0;
	unsigned long bytes_written = 0;
	int asw = 0;
	if (!cluox_nopipe_fill) 
	{
		asw = WriteFile(cluox_stdin_writeback, &c, 1, &bytes_written, NULL);
	}
	if ((bytes_written != 1) || (asw == 0)) 
	{
		printf("Error using cluox-io\n" );
		Shutdown( 10 );
	}
	c = fgetc( stdin );
	if (c == 0) 
	{
		cluox_nopipe_fill = false;
		return -1;
	}
	// here an actual charater is read in
	return c;

#else
#  error Unknown operating system.
#endif
}
#endif

#ifndef __NT__
void closesocket( UOXSOCKET s )
{
	shutdown(s, 2);
	close(s);
}
#endif

int str2num(char *s) // Convert string to integer
{
	unsigned int i;
	int n = 0;
	int neg = 0;
	unsigned int length = strlen(s);
	for (i = 0; i < length; i++)
	{
		if (s[i] == '-') 
			neg = 1;
		n *= 10; // Multiply by 10
		if (isdigit(s[i]))
			n += s[i] - 48; // Convert char to number from 0 to 9
	}
	if (neg) 
		n = -n;
	return n;
}

int hstr2num(char *s) // Convert hex string to integer
{
	unsigned int i;
	int n = 0;
	
	for (i = 0; i < strlen(s); i++)
	{
		n *= 16;
		if (isdigit(s[i]))
			n += s[i] - 48; // Convert char to number from 0 to 9
		if ((s[i]>=65) && (s[i]<=70)) // Uppercase A-F
			n += s[i] - 65 + 10;
		if ((s[i]>=97) && (s[i]<=102)) // Lowercase A-F
			n += s[i] - 97 + 10;
	}
	if (s[0]== '-') 
		n = -n;
	return n;
}

// LB, wrapping of the stdlib num-2-str functions
void numtostr( int i, char *string )
{
#ifdef __NT__
	itoa( i, string, 10 );
#else
	sprintf(string, "%d", i);
#endif
}

void hextostr( int i, char *string )
{
#ifdef __NT__
	itoa( i, string, 16 );
#else
	sprintf(string, "%x", i);
#endif
}

//o---------------------------------------------------------------------------o
//|	Function	-	void readscript(void)
//|	Date		-	Uknown
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Reads lines from the *.wsc files until it finds a
//|					non-commented, usable line. Commented lines are
//|					preceeded with // chars.
//o---------------------------------------------------------------------------o
void readscript( void ) // Read a line from the opened script file
{
	if( scpfile == NULL )
	{
		strcpy( temp, "EOF" );
		return;
	}
	int i, valid=0;
	char c;
	temp[0]=0;
	while(!valid)
	{
		i=0;
		if (feof(scpfile))
		{
			strcpy( temp, "EOF" );
			return;
		}
		c=(char)fgetc(scpfile);
		while (c!=10 && i < 1024)
		{
			if (c!=13)// && c!=0)
			{
				temp[i]=c;
				i++;
			}
			if (feof(scpfile)) return;
			c=(char)fgetc(scpfile);
			if( c == 9 ) c = 32;	// if we actually have a tab, then replace it with a space!
		}
		temp[i]=0;
		valid=1;
		if (temp[0]=='/' && temp[1]=='/') valid=0;
		if (temp[0]=='{') valid=0;
		if (temp[0]==0) valid=0;
	}
}

//o---------------------------------------------------------------------------o
//|	Function	-	void readscript( FILE *toReadFrom )
//|	Date		-	1st March, 2000
//|	Programmer	-	Abaddon
//o---------------------------------------------------------------------------o
//|	Purpose		-	Reads lines from the toReadFrom until it finds a
//|					non-commented, usable line. Commented lines are
//|					preceeded with // chars.
//o---------------------------------------------------------------------------o
void readscript( FILE *toReadFrom ) // Read a line from the opened script file
{
	if( toReadFrom == NULL )
	{
		strcpy( temp, "EOF" );
		return;
	}
	bool valid = false;
	int i;
	char c;
	temp[0] = 0;
	while( !valid )
	{
		i = 0;
		if( feof( toReadFrom ) ) 
			return;
		c = (char)fgetc( toReadFrom );
		while ( c != 10 && i < 1024 )	// don't overflow our buffer
		{
			if( c != 13 )// && c!=0)
			{
				temp[i] = c;
				i++;
			}
			if( feof( toReadFrom ) ) 
				return;
			c = (char)fgetc( toReadFrom );
			if( c == 9 ) c = 32;	// if we actually have a tab, then replace it with a space!
		}
		temp[i] = 0;
		valid = true;
		if( temp[0] == '/' && temp[1] == '/' ) 
			valid = false;
		if( temp[0] == '{') 
			valid = false;
		if( temp[0] == 0) 
			valid = false;
	}
}

//void openscript (char *name) // Open script file
// [10/2/99 krazyglue] - Added return value if unable to open file.  Calls to this 
//                       function should check return code to avoid crashing.
unsigned char openscript( char *name ) // Open script file
{
	scpfile=fopen(name,"r");
	if (scpfile==NULL)
	{
#ifdef __LINUX__
		printf("ERROR: Unable to open file '%s':\n", name );
#else
		printf("ERROR: Unable to open file '%s': %s\n", name, strerror(errno));
#endif
		error=1;
		keeprun=0;
		return 0xFF;
	}
	openings++;
	//assert(openings == 1);
	// printf("Openings: %i\n",openings);
	return 0;
}

FILE *openscript( char *name, FILE *toOpen ) // Open script file with file pointer toOpen
{
	toOpen = fopen( name, "r" );
	if (toOpen == NULL)
	{
#ifdef __LINUX__
		printf("ERROR: Unable to open file '%s':\n", name );
#else
		printf("ERROR: Unable to open file '%s': %s\n", name, strerror(errno));
#endif
		error = 1;
		keeprun = 0;
		return NULL;
	}
	return toOpen;
}

void closescript( void )
{
	if( scpfile )
	{
		fclose(scpfile);
		scpfile = NULL;
	}
	openings--;
}

void read1( void ) // Read script line without splitting parameters
{
	readscript();
	strcpy(script1, temp);
}

void read2( void ) // Read line from script
{
	int i;
	readscript();
	i=0;
	script1[0]=0;
	script2[0]=0;
	while(temp[i]!=0 && temp[i]!=' ' && temp[i]!='=')
	{
		i++;
	}
	strncpy(script1, temp, i);
	script1[i]=0;
	if (script1[0]!='}' && temp[i]!=0) strcpy(script2, temp+i+1);
	return;
}

void read2( FILE *toRead ) // Read line from script
{
	int i;
	readscript( toRead );
	i = 0;
	script1[0] = 0;
	script2[0] = 0;
	while( temp[i] != 0 && temp[i] != ' ' && temp[i] != '=' )
	{
		i++;
	}
	strncpy(script1, temp, i);
	script1[i] = 0;
	if ( script1[0] != '}' && temp[i] != 0 ) 
		strcpy( script2, temp+i+1 );
	return;
}


int inrange1( UOXSOCKET a, UOXSOCKET b ) // Are players from sockets a and b in visual range (Obsolete)
{
	if( a == b )
		return 1;
	short range = Races->getVisRange( chars[currchar[a]].race );
	short dx = abs( chars[currchar[a]].x - chars[currchar[b]].x );
	if( dx > range )
		return 0;
	short dy = abs( chars[currchar[a]].y - chars[currchar[b]].y );
	if( dy > range )
		return 0;
	return 1;
}

int inrange1p( CHARACTER a, CHARACTER b ) // Are characters a and b in visual range
{
	if( a < 0 || a > cmem || b < 0 || b > cmem )
		return 0;
	if( a == b )
		return 1;
	short range = Races->getVisRange( chars[a].race );
	short dx = abs( chars[a].x - chars[b].x );
	if( dx > range )
		return 0;
	short dy = abs( chars[a].y - chars[b].y );
	if( dy > range )
		return 0;
	return 1;
}

unsigned int chardist( CHARACTER a, CHARACTER b ) // Distance between characters a and b
{
	if( a == -1 || b == -1 ) 
		return 30;
	short dx = abs( chars[a].x - chars[b].x );
	short dy = abs( chars[a].y - chars[b].y );
	return (unsigned int)(hypot( dx, dy ));
}

// calculates distance between item i and player a
unsigned int itemdist( CHARACTER a, int i )
{
	if( a == -1 || i == -1 ) 
		return 30;
	short dx = abs( chars[a].x - items[i].x );
	short dy = abs( chars[a].y - items[i].y );
	return (unsigned int)(hypot( dx, dy ));
}


int inrange2 (UOXSOCKET s, ITEM i) // Is item i in visual range for player on socket s
{
	if( s > now || i < 0 || i > imem )
		return 0;
	short vr = Races->getVisRange( chars[currchar[s]].race );
	if( items[i].id1 >= 0x40 )	// High ID is 0x40 or above, which is reserved for multis
		vr = BUILDRANGE;

	short dx = abs( chars[currchar[s]].x - items[i].x );
	if( dx > vr )
		return 0;
	short dy = abs( chars[currchar[s]].y - items[i].y );
	if( dy > vr )
		return 0;
	return 1;
}



// safely copy a string that might be longer than the destination, truncating if needed,
// but never copy over to much so it might crash.
// probably not the best place for this function, but...
void safeCopy(char *dest, const char *src, unsigned int maxLen)
{
	assert(src);
	assert(dest);
	assert(maxLen);
	
	strncpy(dest, src, maxLen);
	dest[maxLen - 1] = '\0';
}

int online(CHARACTER c) // Is the player owning the character c online
{
	unsigned int i;
	if( c == -1 ) 
		return 0;		// invalid subscript stuff
	else if( chars[c].npc ) 
		return 0;
	else if( inworld[chars[c].account] == c ) 
		return 1;//Instalog
	else 
	{
		for (i=0;i<now;i++) 
		{
			if ( currchar[i]==c && perm[i] )
				return 1;
		}
	}
	return 0;
}

int bestskill(CHARACTER p) // Which skill is the highest for character p
{
	int i,a=0,b=0;

	for (i=0;i<TRUESKILLS;i++) // krazyglue - sorry was hard to read as for(...) if (...) 9th October
	{
		if (chars[p].baseskill[i]>b)
		{
			a=i;
			b=chars[p].baseskill[i];
		}
	}
	return a;
}

void loadcustomtitle( void ) // for custom titles
{ 
	int titlecount=0;
	char sect[512]; 
	
	openscript("titles.scp");
	strcpy(sect,"SKILL");
	if(!i_scripts[titles_script]->find(sect))
	{
		closescript();
		return;
	}
	do
	{
		read2();
		if (script1[0]!='}')
		{
			strcpy(title[titlecount].skill, script1);
			titlecount++;
		}
	}
	while (script1[0]!='}');
	closescript();
	script1[0]=0;
	titlecount=0;
	openscript("titles.scp");
	strcpy(sect,"PROWESS");
	if(!i_scripts[titles_script]->find(sect))
	{
		closescript();
		return;
	}
	do
	{
		read2();
		if (script1[0]!='}')
		{
			strcpy(title[titlecount].prowess, script1);
			titlecount++;
		}
	}
	while (script1[0]!='}');
	closescript();
	script1[0]=0;
	titlecount=0;
	openscript("titles.scp");
	strcpy(sect,"FAME");
	if(!i_scripts[titles_script]->find(sect))
	{
		closescript();
		return;
	}
	do
	{
		read2();
		if (script1[0]!='}')
		{
			strcpy(title[titlecount].fame, script1);
			if (titlecount == 23)
			{
				title[titlecount].fame[0] = 0;
				strcpy(title[++titlecount].fame, script1);
			}
			titlecount++;
		}
	}
	while (script1[0]!='}');
	closescript();
}

char *title1(CHARACTER p) // Paperdoll title for character p (1)
{
	int titlenum;
    int x = chars[p].baseskill[bestskill(p)];

	if( x >= 1000 )
		titlenum = 10;
	else if( x >= 960 )
		titlenum = 9;
	else if( x >= 920 ) 
		titlenum = 8;
	else if( x >= 820 ) 
		titlenum = 7;
	else if( x >= 770 ) 
		titlenum = 6;
	else if( x >= 720 ) 
		titlenum = 5;
	else if( x >= 610 ) 
		titlenum = 4;
	else if( x >= 510 ) 
		titlenum = 3;
	else if( x >= 410 ) 
		titlenum = 2;
	else if( x >= 300 ) 
		titlenum = 1;
	else
		titlenum = 0;
	
	strcpy(prowesstitle, title[titlenum].prowess);
	return prowesstitle;
}

char *title2(CHARACTER p) // Paperdoll title for character p (2)
{
	int titlenum = bestskill( p ) + 1;
	strcpy(skilltitle, title[titlenum].skill);
	return skilltitle;
}

char *title3( CHARACTER p ) // Paperdoll title for character p (3)
{
	char thetitle[50];
	int titlenum=0;
	int k;
	unsigned int f;
	
	k = chars[p].karma;
	f = chars[p].fame;
	thetitle[0] = 0;
	
	if( k >= 10000 )
	{
		if( f >= 5000 )
			titlenum = 0;
		else if( f >= 2500 )
			titlenum = 1;
		else if( f >= 1250 )
			titlenum = 2;
		else
			titlenum = 3;
	}
	else if( k >= 5000 && k < 9999 )
	{
		if( f >= 5000 ) 
			titlenum = 4;
		else if( f >= 2500 ) 
			titlenum = 5;
		else if( f >= 1250 ) 
			titlenum = 6;
		else
			titlenum = 7;
	}
	else if( k >= 2500 && k < 5000 )
	{
		if( f >= 5000 ) 
			titlenum = 8;
		else if( f >= 2500 ) 
			titlenum = 9;
		else if( f >= 1250 ) 
			titlenum = 10;
		else
			titlenum = 11;
	}
	else if( k >= 1250 && k < 2500 )
	{
		if( f >= 5000 ) 
			titlenum = 12;
		else if( f >= 2500 ) 
			titlenum = 13;
		else if( f >= 1250 ) 
			titlenum = 14;
		else
			titlenum = 15;
	}
	else if( k >= 625 && k < 1250 )
	{
		if( f >= 5000 ) 
			titlenum = 16;
		else if( f >= 1000 ) 
			titlenum = 17;
		else if( f >= 500 )
			titlenum = 18;
		else
			titlenum = 19;
	}
	else if( k > -625 && k < 625 )
	{
		if( f >= 5000 ) 
			titlenum = 20;
		else if( f >= 2500 ) 
			titlenum = 21;
		else if( f >= 1250 ) 
			titlenum = 22;
		else
			titlenum = 23;
	}
	else if( k > -1250 && k <= -625 )
	{
		if( f >= 10000 ) 
			titlenum = 28;
		else if( f >= 5000 ) 
			titlenum = 27;
		else if( f >= 2500 ) 
			titlenum = 26;
		else if( f >= 1250 ) 
			titlenum = 25;
		else
			titlenum = 24;
	}
	else if( k > -2500 && k <= -1250 )
	{
		if( f >= 5000 ) 
			titlenum = 32;
		else if( f >= 2500 ) 
			titlenum = 31;
		else if( f >= 1250 ) 
			titlenum = 30;
		else
			titlenum = 29;
	}
	else if( k > -5000 && k <= -2500 )
	{
		if( f >= 10000 ) 
			titlenum = 37;
		else if( f >= 5000 ) 
			titlenum = 36;
		else if( f >= 2500 ) 
			titlenum = 35;
		else if( f >= 1250 ) 
			titlenum = 34;
		else
			titlenum = 33;
	}
	else if( k >-10000 && k <= -5000 )
	{
		if( f >= 5000 ) 
			titlenum = 41;
		else if( f >= 2500 ) 
			titlenum = 40;
		else if( f >= 1250 ) 
			titlenum = 39;
		else
			titlenum = 38;
	}
	else if( k <= -10000 )
	{
		if( f >= 5000 ) 
			titlenum = 45;
		else if( f >= 2500 ) 
			titlenum = 44;
		else if( f >= 1250 ) 
			titlenum = 43;
		else
			titlenum = 42;
	}
	if( chars[p].race != 0 && chars[p].race != 65535 )
		sprintf( thetitle, "%s %s ", title[titlenum].fame, Races->getName( chars[p].race ) );
	else
		sprintf( thetitle, "%s ", title[titlenum].fame );
	if( f >= 10000 ) // Morollans bugfix for repsys
	{
		if( chars[p].kills > repsys.maxkills )
        {
			if( chars[p].id2 == 0x91 ) 
				sprintf( fametitle, "The Murderous %s Lady ", Races->getName( chars[p].race ) );
			else 
				sprintf( fametitle, "The Murderer %s Lord ", Races->getName( chars[p].race ) );
        }
		else if( chars[p].id2 == 0x91 ) 
			sprintf( fametitle, "The %sLady ", thetitle );
		else 
			sprintf( fametitle, "The %sLord ", thetitle );
	}
	else
	{
		if( chars[p].kills > repsys.maxkills )
        {
			strcpy( fametitle, "The Murderer " ); //Morrolan rep
        }
		else if( strcmp( thetitle, " " ) )               
			sprintf( fametitle, "The %s", thetitle );
		else 
			fametitle[0] = 0;
	}
	
	//
	// before
	//
	
	return fametitle;
}

void gcollect( void ) // Remove items which were in deleted containers 
// remarks : Okay LB... I'll just keep re-writing it until someone shuts up.
{
	unsigned char a, b, c, d;
	int j, removed, rtotal = 0, serial;
	int idelete;
	unsigned int i;
	uiCurrentTime = 0;
	
	printf( "Performing Garbage Collection...");
	do
	{
		removed=0;
		for (i=0;i<itemcount;i++)
		{
			if (items[i].free==0)
			{
				idelete=0;
				a=items[i].cont1;
				b=items[i].cont2;
				c=items[i].cont3;
				d=items[i].cont4;
				serial = calcserial( a, b, c, d );
				if (!((a==255)&&(b==255)&&(c==255)&&(d==255)))
				{
					idelete=1;
					if (a<0x40) // container is a character...verify the character??
					{
						j=findbyserial(&charsp[serial%HASHMAX],serial,1);
						if (j!=-1)
						{
							if (!chars[j].free)
								idelete=0;
						}
					} else {// find the container if there is one.
						j=findbyserial(&itemsp[serial%HASHMAX],serial,0);
						if (j!=-1)
						{
							if (!items[j].free)
								idelete=0;
						}
					}
				}
				if (idelete)
				{
					Items->DeleItem(i);
					removed++;
				}
			}
		}
		rtotal=rtotal+removed;
	} while (removed>0);
	
	
	uiCurrentTime = getclock();
    printf(" Removed %i items.\n",rtotal);
}

//o---------------------------------------------------------------------------o
//|	Function	-	void readwscline(void)
//|	Date		-	Uknown
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Reads lines from the *.wsc files until it finds a
//|					non-commented, usable line. Commented lines are
//|					preceeded with // chars.
//o---------------------------------------------------------------------------o
void readwscline ()
{
	int i, valid=0;
	char c;
	temp[0]=0;
	while (!valid)
	{
		i=0;
		if (feof(wscfile)) return;
		c=(char)fgetc(wscfile);
		while (c!=10)
		{
			if (c!=13)// && c!=0)
			{
				temp[i]=c;
				i++;
			}
			if (feof(wscfile)) return;
			c=(char)fgetc(wscfile);
		}
		temp[i]=0;
		valid=1;
		if (temp[0]=='/' && temp[1]=='/') valid=0;
		if (temp[0]=='{') valid=0;
		if (temp[0]==0) valid=0;
		if (temp[0]==10) valid=0;
		if (temp[0]==13) valid=0;
	}
}

void readw2 ( void )
{
	int i=0;
	
	readwscline();
	script1[0]=0;
	script2[0]=0;
	script3[0]=0;
	while(temp[i]!=0 && temp[i]!=' ' && temp[i]!='=') i++;
	strncpy(script1, temp, i);
	script1[i]=0;				 // terminate with null
	if (script1[0]!='}' && temp[i]!=0) strcpy(script2, temp+i+1);
	return;
}

void readw3 ( void )
{
	int i=0,j;
	
	readwscline();
	script1[0]=0;
	script2[0]=0;
	script3[0]=0;
	while(temp[i]!=0 && temp[i]!=' ' && temp[i]!='=') i++;
	strncpy(script1, temp, i);
	script1[i]=0;             // terminate with null
	if (script1[0]=='}' || temp[i]==0) return;
	i++;
	j=i;
	while(temp[i]!=0 && temp[i]!=' ' && temp[i]!='=') i++;
	strncpy(script2, temp+j, i-j);
	script2[i-j]=0;
	strcpy(script3, temp+i+1);
}

void loadatrophy( CHARACTER c, char *astr )
{
	char a[3];
	unsigned int l, n=0, sl=strlen(astr);

  //07 12 34 45 etc..
	//number1 number2 space...
	for ( l=0;l<sl;l+=3 )
	{
		a[0] = astr[l];
		a[1] = astr[l+1];
		a[2] = 0;

		chars[c].atrophy[n]=str2num(a);
		n++;
	}
}

void loadchar(int x) // Load a character from WSC
{
	unsigned long int i,k;
	int j,a=0, loops=0;
	char newpoly=0;

	x = Npcs->MemCharFree();
	if( x == -1 ) 
		return;
	Npcs->InitChar( x, 0 );

	chars[x].dir = 4;
	chars[x].hp = chars[x].st = 10;
	chars[x].stm = chars[x].dx = 10;
	chars[x].mn = chars[x].in = 10;
	chars[x].race = 0;
	chars[x].raceGate = 65535;
	chars[x].tamed = false;
	chars[x].runs = false;
	chars[x].guarded = false;
	chars[x].step = 1;
	signed char lockstate = 0;
	int skl = 0;
	newpoly = 0; // this goes with "ORGBODY" but its setting a default up front
	for (i=0;i<TRUESKILLS;i++)
	{
		chars[x].baseskill[i]=1;
		chars[x].skill[i]=1;
	}
	chars[x].namedeed = 5;
	do
	{
		readw2();
		switch( script1[0] )
		{
		case 'a':
		case 'A':
			if( !strcmp( script1, "ACCOUNT" ) ) chars[x].account = str2num( script2 ); 
			else if( !strcmp( script1, "ADVOBJ" ) ) chars[x].advobj = str2num( script2 ); 
			else if( !strcmp( script1, "ALLMOVE" ) ) chars[x].priv2 = str2num( script2 );
			else if( !strcmp( script1, "ATT" ) ) chars[x].att = str2num( script2 );
			else if( !strcmp( script1, "ATROPHY" ) ) loadatrophy( x, script2 );
			break;
		case 'b':
		case 'B':
			if( !strcmp( script1, "BODY" ) )
			{
				i = str2num( script2 );
				chars[x].id1 = (unsigned char)(i>>8);
				chars[x].id2 = (unsigned char)(i%256);
				chars[x].orgid1 = (unsigned char)(i>>8);
				chars[x].orgid2 = (unsigned char)(i%256);
			}
			break;
		case 'c':
		case 'C':
			if( !strcmp( script1, "COMMANDLEVEL" ) )
				chars[x].commandLevel = str2num( script2 );
			break;
		case 'd':
		case 'D':
			if( !strcmp( script1, "DEAD" ) ) chars[x].dead = str2num( script2 );
			else if( !strcmp( script1, "DEATHS" ) ) chars[x].deaths = str2num( script2 );
			else if( !strcmp( script1, "DEF" ) ) chars[x].def = str2num(script2);
			else if( !strcmp( script1, "DEXTERITY" ) ) chars[x].dx = str2num( script2 );
			else if( !strcmp( script1, "DEXTERITY2" ) ) chars[x].dx2 = str2num( script2 );
			else if( !strcmp( script1, "DIR" ) ) chars[x].dir = str2num( script2 ) & 0x0F;
			else if( !strcmp( script1, "DISABLED" ) ) chars[x].disabled = str2num( script2 );
			else if( !strcmp( script1, "DISPZ" ) ) chars[x].dispz = str2num( script2 );
			break;
		case 'e':
		case 'E':
			if( !strcmp( script1, "EMOTE" ) )
			{
				i = str2num(script2);
				chars[x].emotecolor1 = (unsigned char)(i>>8);
				chars[x].emotecolor2 = (unsigned char)(i%256);
			}
			break;
		case 'f':
		case 'F':
			if( !strcmp( script1, "FAME" ) ) chars[x].fame=str2num(script2);
			else if( !strcmp( script1, "FIXEDLIGHT" ) ) chars[x].fixedlight = str2num( script2 ); 
			else if( !strcmp( script1, "FLEEAT" ) ) chars[x].fleeat = str2num( script2 );
			else if( !strcmp( script1, "FONT" ) ) chars[x].fonttype = str2num( script2 );
			else if( !strcmp( script1, "FX1" ) ) chars[x].fx1 = str2num( script2 );
			else if( !strcmp( script1, "FY1" ) ) chars[x].fy1 = str2num( script2 );
			else if( !strcmp( script1, "FZ1" ) ) chars[x].fz1 = str2num( script2 );
			else if( !strcmp( script1, "FX2" ) ) chars[x].fx2 = str2num( script2 );
			else if( !strcmp( script1, "FY2" ) ) chars[x].fy2 = str2num( script2 );
			break;
		case 'g':
		case 'G':
			// Begin of Guild related character stuff
			if( !strcmp( script1, "GUILDTOGGLE" ) ) chars[x].guildtoggle = str2num( script2 ); 
			else if( !strcmp( script1, "GUILDNUMBER" ) ) chars[x].guildnumber = str2num( script2 ); 
			else if( !strcmp( script1, "GUILDTITLE" ) ) safeCopy( chars[x].guildtitle, script2, MAX_GUILDTITLE ); 
			else if( !strcmp( script1, "GUILDFEALTY" ) ) chars[x].guildfealty = str2num( script2 ); 
			// End of guild stuff
			break;
		case 'h':
		case 'H':
			if( !strcmp( script1, "HIDAMAGE" ) ) chars[x].hidamage = str2num( script2 );
			else if( !strcmp( script1, "HIDDEN" ) ) chars[x].hidden = str2num( script2 );
			else if( !strcmp( script1, "HITPOINTS" ) ) chars[x].hp = str2num( script2 );
			else if( !strcmp( script1, "HUNGER" ) ) chars[x].hunger = str2num( script2 );
			break;
		case 'i':
		case 'I':
			if( !strcmp( script1, "INTELLIGENCE" ) ) chars[x].in = str2num( script2 );
			else if( !strcmp( script1, "INTELLIGENCE2" ) ) chars[x].in2 = str2num( script2 );
			break;
		case 'k':
		case 'K':
			if( !strcmp( script1, "KARMA" ) ) chars[x].karma = str2num( script2 );
			else if( !strcmp( script1, "KILLS" ) ) chars[x].kills = str2num( script2 );
			break;
			
		case 'l':
		case 'L':
			if( !strcmp( script1, "LASTON" ) ) 
			{
				safeCopy( chars[x].laston, script2, MAX_LASTON ); //load last time character was on
			}
			else if( !strcmp( script1, "LODAMAGE" ) ) chars[x].lodamage = str2num( script2 ); 
			break;
			
		case 'm':
		case 'M':
			if( !strcmp( script1, "MANA" ) ) chars[x].mn = str2num( script2 );
			else if( !strcmp( script1, "MURDERRATE" ) ) chars[x].murderrate = str2num( script2 );
			break;
			
		case 'n':
		case 'N':
			if( !strcmp( script1, "NAME" ) ) strcpy( chars[x].name, script2 );
			else if( !strcmp( script1, "NOTRAIN" ) ) chars[x].cantrain = 0;
			else if( !strcmp( script1, "NPC" ) ) chars[x].npc = str2num( script2 );
			else if( !strcmp( script1, "NPCAITYPE" ) ) chars[x].npcaitype = str2num( script2 );
			else if( !strcmp( script1, "NPCWANDER" ) ) chars[x].npcWander = str2num( script2 );
			break;
		case 'o':
		case 'O':
			if( !strcmp( script1, "OLDNPCWANDER" ) ) chars[x].oldnpcWander = str2num( script2 );
			else if( !strcmp( script1, "OLDX" ) ) chars[x].oldx = str2num( script2 );
			else if( !strcmp( script1, "OLDY" ) ) chars[x].oldy = str2num( script2 );
			else if( !strcmp( script1, "OLDZ" ) ) chars[x].oldz = str2num( script2 );
			else if( !strcmp( script1, "ORGBODY" ) )
			{
				i = str2num( script2 );
				chars[x].orgid1 = (unsigned char)(i>>8);
				chars[x].orgid2 = (unsigned char)(i%256);
				newpoly = 1;
			}
			else if( !strcmp( script1, "OWN" ) )
			{
				i = str2num( script2 );
				chars[x].own1 = (unsigned char)(i>>24);
				chars[x].own2 = (unsigned char)(i>>16);
				chars[x].own3 = (unsigned char)(i>>8);
				chars[x].own4 = (unsigned char)(i%256);
				chars[x].ownserial = i;
				if( chars[x].ownserial != -1 ) 
				{
					setptr( &cownsp[i%HASHMAX], x ); //Load into charsp array
					chars[x].tamed = true;			// Abaddon
				}
			}
			break;
		case 'p':
		case 'P':
			if( !strcmp( script1, "PACKITEM" ) ) chars[x].packitem = str2num( script2 );
			else if( !strcmp( script1, "POISON" ) ) chars[x].poison = str2num( script2 );
			else if( !strcmp( script1, "POISONED" ) ) chars[x].poisoned = str2num( script2 );
			else if( !strcmp( script1, "PRIV" ) ) chars[x].priv = str2num( script2 );
			break;
		case 'q':
		case 'Q':
			if( !strcmp( script1,"QUESTTYPE" ) ) chars[x].questType = str2num( script2 );
			else if( !strcmp( script1,"QUESTDESTREGION" ) ) chars[x].questDestRegion = str2num( script2 );
			else if( !strcmp( script1,"QUESTORIGREGION" ) ) chars[x].questOrigRegion = str2num( script2 );
			break;						
		case 'r':
		case 'R':
			if( !strcmp( script1, "RACE" ) ) chars[x].race = str2num( script2 );
			else if( !strcmp( script1, "RACEGATE" ) ) chars[x].raceGate = str2num( script2 );
			else if( !strcmp( script1, "REATTACKAT" ) ) chars[x].reattackat = str2num( script2 );
			else if( !strcmp( script1, "RESERVED1" ) ) chars[x].cell = str2num( script2 );
			else if( chars[x].cell == 255 ) chars[x].cell = 0;
			// no cells have been marked as -1, but no cell=0
			// with that line its not necassairy to take it manually out. 
			else if( !strcmp( script1, "ROBE" ) )
			{
				i = str2num( script2 );
				chars[x].robe1 = (unsigned char)i>>24;
				chars[x].robe2 = (unsigned char)(i>>16);
				chars[x].robe3 = (unsigned char)(i>>8);
				chars[x].robe4 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "RUNS" ) ) chars[x].runs = true;
			break;
		case 's':
		case 'S':
			if( !strcmp( script1, "SAY" ) )
			{
				i = str2num( script2 );
				chars[x].saycolor1 = (unsigned char)(i>>8);
				chars[x].saycolor2 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "SERIAL" ) )
			{
				i = str2num( script2 );
				if( charcount2 <= i ) 
					charcount2 = i + 1;
				chars[x].ser1 = (unsigned char)(i>>24);
				chars[x].ser2 = (unsigned char)(i>>16);
				chars[x].ser3 = (unsigned char)(i>>8);
				chars[x].ser4 = (unsigned char)(i%256);
				chars[x].serial = i;
				setptr( &charsp[i%HASHMAX], x ); //Load into charsp array
			}
			else if( !strcmp( script1, "SHOPSPAWN" ) ) chars[x].shopSpawn = str2num( script2 );
			else if( !strncmp( script1, "SKILL", 5 ) )
			{
				chars[x].baseskill[ j = str2num( &script1[5] ) ] = str2num( script2 );
				Skills->updateSkillLevel( x, j );
			}
			else if( !strncmp( script1, "SKL", 3 ) )		// for skill locking
			{
				skl = str2num( &script1[3] );
				lockstate = str2num( script2 );
				if( lockstate > 2 )	//punt
					lockstate = 0;
				chars[x].lockState[skl] = lockstate;
			}
			else if( !strcmp( script1, "SKIN" ) )
			{
				i = str2num( script2 );
				chars[x].skin1 = (unsigned char)(i>>8);
				chars[x].skin2 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "SPAWN" ) )
			{
				i = str2num( script2 );
				chars[x].spawn1=(unsigned char)(i>>24);
				chars[x].spawn2=(unsigned char)(i>>16);
				chars[x].spawn3=(unsigned char)(i>>8);
				chars[x].spawn4=(unsigned char)(i%256);
				chars[x].spawnserial=i;
				if( chars[x].spawn1 < 0x40 ) //Not an item (region spawn) //New -- Zippy respawn area (in reality, it's < 0x40, all item serials START at 0x40
					spawnregion[chars[x].spawn3].current++;                                                       
				if( chars[x].spawnserial != 0 && chars[x].spawnserial != -1 )	// redefined to be 0, not -1	// legacy support until next build
					setptr( &cspawnsp[i%HASHMAX], x ); //Load into charsp array
				
			}
			else if( !strcmp( script1, "SPATTACK" ) ) chars[x].spattack = str2num( script2 ); 
			else if( !strcmp( script1, "SPADELAY" ) ) chars[x].spadelay = str2num( script2 ); 
			else if( !strcmp( script1, "SHOP" ) ) chars[x].shop = str2num( script2 ); 
			else if( !strcmp( script1, "SPEECH" ) ) chars[x].speech = str2num( script2 ); 
			else if( !strcmp( script1, "SPLIT" ) ) chars[x].split = str2num( script2 ); 
			else if( !strcmp( script1, "SPLITCHANCE" ) ) chars[x].splitchnc = str2num( script2 ); 
			else if( !strcmp( script1, "STAMINA" ) ) chars[x].stm = str2num( script2 ); 
			else if( !strcmp( script1, "STRENGTH" ) ) chars[x].st = str2num( script2 ); 
			else if( !strcmp( script1, "STRENGTH2" ) ) chars[x].st2 = str2num( script2 ); 
			else if( !strcmp( script1, "SUMMONTIMER" ) ) chars[x].summontimer = str2num( script2 ); 
			break;
		case 't':
		case 'T':
			if( !strcmp( script1, "TAMING" ) ) chars[x].taming = str2num( script2 ); 
			else if( !strcmp( script1, "TITLE" ) ) strcpy(chars[x].title, script2); 
			else if( !strcmp( script1, "TOWN" ) ) chars[x].town = str2num( script2 );
			else if( !strcmp( script1, "TOWNPRIV" ) ) chars[x].townpriv = str2num( script2 );
			else if( !strcmp( script1, "TOWNTITLE" ) ) chars[x].towntitle = str2num( script2 );
			else if( !strcmp( script1, "TOWNVOTE" ) )
			{
				i = str2num( script2 );
				chars[x].townvote1 = (unsigned char)(i>>24);
				chars[x].townvote2 = (unsigned char)(i>>16);
				chars[x].townvote3 = (unsigned char)(i>>8);
				chars[x].townvote4 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "TRIGGER" ) ) chars[x].trigger = str2num( script2 );
			else if( !strcmp( script1, "TRIGWORD" ) ) strcpy( chars[x].trigword, script2 ); 
			break;
			
		case 'w':
		case 'W':
			if( !strcmp( script1, "WAR" ) ) chars[x].war = str2num( script2 ); 
			break;
		case 'x':
		case 'X':
			if( !strcmp( script1, "X" ) ) chars[x].x = str2num( script2 ); 
			else if( !strcmp( script1, "XBODY" ) )
			{
				i = str2num( script2 );
				chars[x].xid1 = (unsigned char)(i>>8);
				chars[x].xid2 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "XSKIN" ) )
			{
				i = str2num( script2 );
				chars[x].xskin1 = (unsigned char)(i>>8);
				chars[x].xskin2 = (unsigned char)(i%256);
			}
			break;
		case 'y':
		case 'Y':
			if( !strcmp( script1, "Y" ) ) chars[x].y = str2num( script2 );
			break;
		case 'z':
		case 'Z':
			if( !strcmp( script1, "Z" ) ) chars[x].dispz=chars[x].z = str2num( script2 );
			break;
		}
		loops++;
	}
	while( strcmp( script1, "}" ) && loops <= 200 );
	if( chars[x].npc && chars[x].war ) 
		chars[x].war = 0;
 
	setcharflag( x );
 
	 if( chars[x].priv&0x80 && chars[x].commandLevel < 1 && chars[x].priv&0x01 != 0x01 ) // interim line to retain compatibility, MUST BE TAKEN out in the long term!
		 chars[x].commandLevel = 1;
	 if( chars[x].priv&0x01 && chars[x].commandLevel < 2 )	// interim line to retain compatibility, MUST BE TAKEN out in the long term!
		 chars[x].commandLevel = 2;
	 
	 ////////////////////////////////////////////////////////////////////
	 
	 chars[x].region=calcRegionFromXY(chars[x].x,chars[x].y); //LB bugfix
	 chars[x].antispamtimer = 0; // LB - AntiSpam -
	 // Restore original shape if char Polimorphed -  AntiChrist (9/99)
	 if( chars[x].id1 != chars[x].orgid1 || chars[x].id2!= chars[x].orgid2 )
	 {
		 chars[x].id1=chars[x].orgid1;
		 chars[x].id2=chars[x].orgid2;
	 }
	 
	 // lord binarys body/skin autocorrection code

	 k=(chars[x].id1<<8)+chars[x].id2;
	 if (k>=0x000 && k<=0x3e1) 
	 { 
/*		 c1=(chars[x].skin1<<8)+chars[x].skin2; 
		 b=c1&0x4000; 
		 if ((b==16384 && (k >=0x0190 && k<=0x03e1)) || c1==0x8000)
		 {
			 if (c1!=0xf000)
			 {
				 chars[x].skin1=chars[x].xskin1=0xf0;
				 chars[x].skin2=chars[x].xskin2=0;
				 printf("char/player: %s : %i correted problematic skin hue\n",chars[x].name,chars[x].serial);
			 }
		 }*/
	 } 
	 else  // client crashing body --> delete if non player esle put onl”x a warning on server screen
	 {	 // we dont want to delete that char, dont we ?
		 if (chars[x].account==-1) 
		 { 
			 printf("npc: %i[%s] with bugged body value detected, deleted for stability reasons\n",chars[x].serial,chars[x].name);
			 Npcs->DeleteChar(x); 
		 } 
		 else 
		 { 
			 chars[x].id1=0x01;
			 chars[x].id2=0x90;
		 }
	 }
	 
	 a = mapRegions->AddItem( x + 1000000 );
	 
	 if ((chars[x].x < 150 && chars[x].y < 150 && chars[x].account ==-1) || ((chars[x].x>6144 || chars[x].y>4096 || chars[x].x<0 || chars[x].y<0) && chars[x].account==-1)) 
	 { 
		 Npcs->DeleteChar(x); //character in an invalid location
	 }
	 if ((chars[x].x < 100 && chars[x].y < 100 && chars[x].account !=-1) || ((chars[x].x>6144 || chars[x].y>4096 || chars[x].x<0 || chars[x].y<0) && chars[x].account!=-1))  
	 { 
		 mapRegions->RemoveItem( x + 1000000 );
		 chars[x].x = 1000; //player in an invalid location
		 chars[x].y = 1000;
		 chars[x].z = 0;
		 mapRegions->AddItem( x + 1000000 );
	 }
}

void loaditem (int x) // Load an item from WSC
{
	unsigned long int i;
	int loops=0;
	
	x = Items->MemItemFree();
	if( x == -1 ) 
		return;
	Items->InitItem( x, 0 );
	items[x].ser1 = 0x40;
	items[x].id1 = 0x0F;
	items[x].id2 = 0xA6;
	do
	{
		readw2();
		// krazyglue [10/10/99]
		// strcmp's can be pretty slow when you call a lot of them, and switch
		// is very fast check on the first byte to sort out the possibilities.
		switch( script1[0] )
		{
		case 'a':
		case 'A':
			if( !strcmp( script1, "AMOUNT" ) ) items[x].amount=str2num(script2);
			else if( !strcmp( script1, "ATT" ) ) items[x].att=str2num(script2); 
			else if( !strcmp( script1, "AC" ) ) items[x].armorClass = str2num( script2 ); 
			break;
			
		case 'c':
		case 'C':
			if( !strcmp( script1, "COLOR" ) )
			{
				i = str2num( script2 );
				items[x].color1 = (unsigned char)(i>>8);
				items[x].color2 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "CONT" ) )
			{
				i = str2num( script2 );
				items[x].cont1 = (unsigned char)(i>>24);
				items[x].cont2 = (unsigned char)(i>>16);
				items[x].cont3 = (unsigned char)(i>>8);
				items[x].cont4 = (unsigned char)(i%256);
				items[x].contserial = i;
				if( i != -1 ) 
					setptr( &contsp[i%HASHMAX], x ); //set item in pointer array
			}
			else if( !strcmp( script1, "CORPSE" ) ) items[x].corpse = str2num( script2 );
			else if( !strcmp( script1, "CREATOR" ) ) strcpy( items[x].creator, script2 );
			break;
			
		case 'd':
		case 'D':
			if( !strcmp( script1, "DEF" ) ) items[x].def = str2num( script2 );
			else if( !strcmp( "DEX", script1 ) ) items[x].dx = str2num( script2 );
			else if( !strcmp( "DEXADD", script1 ) ) items[x].dx2 = str2num( script2 );
			else if( !strcmp( script1, "DIR" ) ) items[x].dir = str2num( script2 );
			else if( !strcmp( script1, "DISABLED" ) ) items[x].disabled = str2num( script2 );
			else if( !strcmp( script1, "DOORFLAG" ) ) items[x].doordir = str2num( script2 );
			else if( !strcmp( "DYE", script1 ) ) items[x].dye = str2num( script2 );
			else if( !strcmp( "DECAY", script1 ) ) items[x].priv |= 0x01;
			else if( !strcmp( "DISPELLABLE", script1 ) ) items[x].priv |= 0x04;
			else if( !strcmp( script1, "DX" ) ) items[x].dx = str2num( script2 );
			else if( !strcmp( script1, "DX2" ) ) items[x].dx2 = str2num( script2 );
			else if( !strcmp( script1, "DYEABLE" ) ) items[x].dye = str2num( script2 );
			else if( !strcmp( script1, "DESC" ) ) strcpy( items[x].desc, script2 );
			break;
			
		case 'g':
		case 'G':
			if( !strcmp( script1, "GLOW" ) ) items[x].glow = str2num( script2 );
			else if( !strcmp( script1, "GLOWBC" ) )
			{
				i = str2num( script2 );
				items[x].glow_c1 = (unsigned char)(i>>8);
				items[x].glow_c2 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "GLOWTYPE" ) ) items[x].glow_effect = str2num( script2 );
			else if( !strcmp( script1, "GOOD" ) ) items[x].good = str2num( script2 );
			break;
			
		case 'h':
		case 'H':
			if( !strcmp( script1, "HIDAMAGE" ) ) items[x].hidamage = str2num( script2 );
			else if( !strcmp( script1, "HP" ) ) items[x].hp = str2num( script2 ); 
			break;
			
		case 'i':
		case 'I':
			if( !strcmp( script1, "ID" ) )
			{
				i = str2num( script2 );
				items[x].id1 = (unsigned char)(i>>8);
				items[x].id2 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "IN" ) ) items[x].in = str2num( script2 );
			else if( !strcmp( script1, "IN2" ) ) items[x].in2 = str2num( script2 );
			else if( !strcmp( script1, "INT" ) ) items[x].in = str2num( script2 );
			else if( !strcmp( script1, "INTADD" ) ) items[x].in2 = str2num( script2 );
			else if( !strcmp("ITEMHAND", script1 ) ) items[x].itmhand = str2num( script2 );
			break;
			
		case 'l':
		case 'L':
			if( !strcmp( script1, "LAYER" ) ) items[x].layer = str2num( script2 );
			if( !strcmp( script1, "LODAMAGE" ) ) items[x].lodamage = str2num( script2 );
			break;
			
		case 'm':
		case 'M':
			if( !strcmp( script1, "MAXHP" ) ) items[x].maxhp = str2num( script2 );
			else if( !strcmp( script1, "MORE" ) )
			{
				i = str2num( script2 );
				items[x].more1 = (unsigned char)(i>>24);
				items[x].more2 = (unsigned char)(i>>16);
				items[x].more3 = (unsigned char)(i>>8);
				items[x].more4 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "MORE2" ) )
			{
				i = str2num( script2 );
				items[x].moreb1 = (unsigned char)(i>>24);
				items[x].moreb2 = (unsigned char)(i>>16);
				items[x].moreb3 = (unsigned char)(i>>8);
				items[x].moreb4 = (unsigned char)(i%256);
			}
			else if( !strcmp( script1, "MOREX" ) ) items[x].morex = str2num( script2 );
			else if( !strcmp( script1, "MOREY" ) ) items[x].morey = str2num( script2 );
			else if( !strcmp( script1, "MOREZ" ) ) items[x].morez = str2num( script2 );
			else if( !strcmp( script1, "MOVABLE" ) ) items[x].magic = str2num( script2 );
			break;
			
		case 'n':
		case 'N':
			if( !strcmp( script1, "NAME" ) ) 
			{ 
				if( !strlen( script2 ) )
					strcpy( items[x].name, "#" );
				else
					strcpy( items[x].name, script2 );
			}
			else if( !strcmp( script1, "NAME2" ) ) 
			{ 
				if( !strlen( script2 ) )
					strcpy( items[x].name, "#" );
				else
					strcpy( items[x].name2, script2 ); 
			}
			break;
			
		case 'o':
		case 'O':
			if( !strcmp( script1, "OFFSPELL" ) ) items[x].offspell = str2num( script2 );
			else if( !strcmp( script1, "OWNER" ) )
			{
				i = str2num( script2 );
				items[x].owner1 = (unsigned char)(i>>24);
				items[x].owner2 = (unsigned char)(i>>16);
				items[x].owner3 = (unsigned char)(i>>8);
				items[x].owner4 = (unsigned char)(i%256);
				items[x].ownserial = i;
				setptr( &ownsp[i%HASHMAX], x ); //set item in pointer array
			}
			break;
			
		case 'p':
		case 'P':
			if( !strcmp( script1, "PILEABLE" ) ) items[x].pileable = str2num( script2 );
			else if( !strcmp( script1, "POISONED" ) ) items[x].poisoned = str2num( script2 );
			else if( !strcmp( script1, "PRIV" ) ) items[x].priv = str2num( script2 );
			break;
			
		case 'r':
		case 'R':
			if( !strcmp( script1, "RESTOCK" ) ) items[x].restock = str2num( script2 );
			else if( !strcmp( "RACE", script1 ) ) items[x].racialEffect = str2num( script2 );
			else if( !strcmp( script1, "RANK" ) ) 
			{
				items[x].rank = str2num( script2 ); // By Magius(CHE)
				if( items[x].rank <= 0 ) 
					items[x].rank = 10;
			}
			break;
			
		case 's':
		case 'S':
			if( !strcmp( script1, "SERIAL" ) )
			{
				i = str2num( script2 );
				if( itemcount2 <= i ) 
					itemcount2 = i + 1;
				items[x].ser1 = (unsigned char)(i>>24);
				items[x].ser2 = (unsigned char)(i>>16);
				items[x].ser3 = (unsigned char)(i>>8);
				items[x].ser4 = (unsigned char)(i%256);
				items[x].serial = i; //Tauriel
				setptr( &itemsp[i%HASHMAX], x ); //set item in pointer array
			}
			else if( !strcmp( script1, "SPAWN" ) )
			{
				i = str2num( script2 );
				items[x].spawn1 = (unsigned char)(i>>24);
				items[x].spawn2 = (unsigned char)(i>>16);
				items[x].spawn3 = (unsigned char)(i>>8);
				items[x].spawn4 = (unsigned char)(i%256);
				items[x].spawnserial = i;
				setptr( &spawnsp[i%HASHMAX], x ); //set item in pointer array
			}
			else if( !strcmp( script1, "SPD" ) ) items[x].spd = str2num( script2 );
			else if( !strcmp( script1, "ST" ) ) items[x].st = str2num( script2 );
			else if( !strcmp( script1, "ST2" ) ) items[x].st2 = str2num( script2 );
			else if( !strcmp( script1, "SK_MADE" ) ) items[x].madewith = str2num( script2 );
			else if( !strcmp( script1, "STR" ) ) items[x].st = str2num( script2 );
			else if( !strcmp( script1, "STRADD" ) ) items[x].st2 = str2num( script2 );
			break;
			
		case 't':
		case 'T':
			if( !strcmp( script1, "TRIGGER" ) ) items[x].trigger = str2num( script2 );
			else if( !strcmp( script1, "TRIGTYPE" ) ) items[x].trigtype = str2num( script2 );
			else if( !strcmp( script1, "TYPE" ) ) items[x].type = str2num( script2 );
			else if( !strcmp( script1, "TYPE2" ) ) items[x].type2 = str2num( script2 );
			break;
			
		case 'u':
		case 'U':
			if( !strcmp( script1, "USES" ) ) items[x].tuses = str2num( script2 );
			break;
			
		case 'v':
		case 'V':
			if( !strcmp( script1, "VISIBLE" ) ) items[x].visible = str2num( script2 );
			else if( !strcmp( script1, "VALUE" ) ) items[x].value = str2num( script2 );
			break;
			
		case 'w':
		case 'W':
			if( !strcmp( script1, "WEIGHT" ) ) 
			{
				i = str2num( script2 );			
				items[x].weight = i;
			} 
			else 
				items[x].weight = 0;
			
			if( !strcmp( script1, "WIPE" ) ) items[x].wipe = str2num( script2 );
			break;
			
		case 'x':
		case 'X':
			if( !strcmp( script1, "X" ) ) items[x].x = str2num( script2 );
			break;
			
		case 'y':
		case 'Y':
			if( !strcmp( script1, "Y" ) ) items[x].y = str2num( script2 );
			break;
		case 'z':
		case 'Z':
			if( !strcmp( script1, "Z" ) ) items[x].z = str2num( script2 );
			break;
		}
		
		loops++;
	}
	while( strcmp( script1, "}" ) && loops <= 200 );
 
	StoreItemRandomValue( x, -1 ); // Magius(CHE) (2)
 
 
	//add item weight if item doesn't have it yet
	if (items[x].weight<=0) // LB, changed from 29 to 0
	{
		items[x].weight=0;
	}

	if( items[x].maxhp == 0 ) 
		items[x].maxhp = items[x].hp;
    // Tauriel adding region pointers
 
	if (items[x].contserial==-1)
	{ 
		da = mapRegions->AddItem(x);	// it returns 1 if inalid, if invalid it DOESNT get added !!!
		if( items[x].x < 0 || items[x].y < 0 || items[x].x > 6144 || items[x].y > 4096 )
		{  
			Items->DeleItem( x );	//these are invalid locations, delete them!	    
		}
	}          
}

// lb, check for bugged items and autocorrect
void item_test( void )
{
	int a, serial;
	uiCurrentTime = 0;
	
	printf("Starting item-consistency check..." );
	for( a = 0; a < itemcount; a++ )
	{
		serial = items[a].serial;
		
		if( serial == items[a].contserial )
		{
			printf("\nALERT ! item %s [%i] [serial: %i] has dangerous container value, autocorrecting\n", items[a].name, a, items[a].serial );
			items[a].contserial = -1;
			removefromptr( &contsp[serial%HASHMAX], a );
			items[a].cont1 = 255;
			items[a].cont2 = 255;
			items[a].cont3 = 255;
			items[a].cont4 = 255;
		}
		if( serial == items[a].ownserial )
		{
			printf("\nALERT ! item %s [%i] [serial: %i] has dangerous owner value\n", items[a].name, a, items[a].serial );
			items[a].ownserial = -1;
			removefromptr( &ownsp[serial%HASHMAX], a );
			items[a].owner1 = 255;
			items[a].owner2 = 255;
			items[a].owner3 = 255;
			items[a].owner4 = 255;
		}
		if( serial == items[a].spawnserial )
		{
			printf("\nALERT ! item %s [%i] [serial: %i] has dangerous spawner value\n", items[a].name, a, items[a].serial );
			items[a].spawnserial = 0;
			removefromptr( &spawnsp[serial%HASHMAX], a );
			items[a].spawn1 = 255;
			items[a].spawn2 = 255;
			items[a].spawn3 = 255;
			items[a].spawn4 = 255;
		}
	}
	uiCurrentTime = getclock();
	printf(" Done!\n" );
}

char *RealTime(char *time_str)
{
	struct tm *curtime;
	time_t bintime;
	time(&bintime);
	curtime = localtime(&bintime);
	strftime(time_str, 256, "%B %d %I:%M:%S %p", curtime);
	return time_str;
}

void savelog(const char *msg, char *logfile)
{
	if (server_data.log>=1)
	{
		char time_str[256];
		FILE *file;
		
		file=fopen(logfile,"a");
		if (file)
		{
			fprintf(file,"[%s] %s",RealTime(time_str),msg);
			
#ifdef DEBUG
			printf("DEBUG: Logging to %s\n", logfile);
#endif
			
			fclose(file);
		}
	}
}
// Load character and item data from load chars.wsc and items.wsc
void loadnewworld ( void )
{
	char outper[4] = {0,}; // Magius(CHE) (1)
	unsigned int i=0;
	unsigned int percent = 0, a = 0, pred = 0, maxm = 0; // Magius(CHE) (1)
	cmem=0;
	imem=0;
	printf("Loading World, Building map Regions, checking Item weight...\n");
	charcount=0;
	itemcount=0;
	charcount2=1;
	itemcount2=0x40000000;
	
	wscfile=fopen("chars.wsc","r");
	if (wscfile==NULL)
	{
		printf("WARNING: Chars.wsc not found. Defaulting to uox3.wsc\n");
		cmem=100;
		
		printf(" Allocating inital dynamic Character memory of %i... ",cmem);
		
		chars.Reserve( cmem );
		
		if(( clickx = new int[cmem] ) == NULL ) Shutdown( FATAL_UOX3_CLICKX );
		if(( clicky = new int[cmem] ) == NULL ) Shutdown( FATAL_UOX3_CLICKY );
		if(( currentSpellType = new int[cmem] )== NULL ) Shutdown( FATAL_UOX3_CURRENTSPELLTYPE );
		if(( targetok = new char[cmem] ) == NULL ) Shutdown( FATAL_UOX3_TARGETOK );   // shouldn't it be sizeof( char )
		
		printf("Done\n");
	} 
	else 
	{
		//Get number for inital character memory needed ->
		readw3();
		if (!(strcmp(script1, "INITMEM"))) cmem=str2num(script2);
		maxm = cmem; // Magius(CHE) (1)
		if (cmem<100) cmem=100;
		
		printf("Allocating inital dynamic Character memory of %i... ",cmem);
		
		chars.Reserve( cmem );
		
		if(( clickx = new int[cmem] ) == NULL ) Shutdown( FATAL_UOX3_CLICKX );
		if(( clicky = new int[cmem] ) == NULL ) Shutdown( FATAL_UOX3_CLICKY );
		if(( currentSpellType = new int[cmem] ) == NULL ) Shutdown( FATAL_UOX3_CURRENTSPELLTYPE );
		if(( targetok = new char[cmem] ) == NULL ) Shutdown( FATAL_UOX3_TARGETOK );   // shouldn't it be sizeof( char )
		printf("Done\n");
		
		printf("  Loading characters ");
		do
		{
			readw3();
			if (!(strcmp(script1, "SECTION")))
			{
				if (!(strcmp(script2, "CHARACTER")))
				{
					loadchar(str2num(script3));
					a++;
					pred = percent; // Magius(CHE) (1)
					if (maxm <= 1)
						percent = 100;
					else
						percent = (int)(a*100)/(maxm-1); // Magius(CHE) (1)
					if( strlen( outper ) > 0 ) { // Magius(CHE) (1)
						for( i = 1; i <= strlen( outper ) + 1; i++ ) printf("\b" );
						outper[0] = 0;
					}
				}
			}
			if (percent> pred ) // Changed by MAgius(CHE) (1)
			{
				numtostr( percent, outper );
				printf("%s%%", outper );
				pred = percent;
				// a=0; MAgius(CHE) (1)
			}
		} while (strcmp(script1,"EOF") && !feof(wscfile));
		fclose(wscfile);
		wscfile = NULL;
		if( strlen( outper ) > 0 ) { // Magius(CHE) (1)
			for( i = 1; i <= strlen( outper ) + 1; i++ )
				printf( "\b" );
			outper[0] = 0;
		}
		printf("Done.\n");
		wscfile=fopen("items.wsc", "r");
		if (wscfile==NULL)
		{
			printf("ERROR: Items.wsc not found. No items will be loaded.\n");
			imem=100;
			
			// 10th October, 1999 removed * sizeof( int ) by Abaddon
			printf("Allocating inital dynamic Item memory of %i... ",imem);
			
			items.Reserve( imem );
			
			if(( loscache = new int[imem] ) == NULL ) Shutdown( FATAL_UOX3_LOSCACHE );
			if(( itemids = new int[imem] ) == NULL ) Shutdown( FATAL_UOX3_ITEMIDS );
			printf("Done\n");
		} 
		else 
		{
			//Get number for inital character memory needed ->
			readw3();
			if (!(strcmp(script1, "INITMEM"))) imem=str2num(script2);
			maxm = imem; 
			if (imem<100) imem=100;
			
			printf("Allocating inital dynamic Item memory of %i... ",imem);
			
			items.Reserve( imem );
			
			if(( loscache = new int[imem] ) == NULL ) Shutdown( FATAL_UOX3_LOSCACHE );
			if(( itemids = new int[imem] ) == NULL ) Shutdown( FATAL_UOX3_ITEMIDS );
			printf("Done\n");
			
			a = 0; // Magius(CHE) (2)
			printf("  Loading items ");
			do {
				readw3();
				if (!(strcmp(script1, "SECTION")))
				{
					if (!(strcmp(script2, "WORLDITEM"))) 
					{
						loaditem(str2num(script3));
					}
					if (!(strcmp(script2, "GUILD")))
						Guilds->Read(str2num(script3));
					a++;
					pred = percent; // Magius(CHE) (1)
					if (maxm <=1 )
						percent = 100;
					else
						percent = (int)(a*100)/(maxm - 1); // Magius(CHE) (1)
					if( strlen( outper ) > 0 ) { // Magius(CHE) (1)
						for( i = 1; i <= strlen( outper ) + 1; i++ ) 
							printf("\b" );
						outper[0] = 0;
					}
				}
				if( percent > pred ) // Changed by MAgius(CHE) (1)
				{
					numtostr( percent, outper );
					printf("%s%%", outper );
					pred = percent;
				}
				
			} while (strcmp(script1, "EOF") && !feof(wscfile));
			fclose(wscfile);
			wscfile = NULL;
		}
		if( strlen( outper ) > 0 ) { // Magius(CHE) (1)
			for( i = 1; i <= strlen( outper ) + 1; i++ ) 
				printf( "\b" );
			outper[0] = 0;
		}
		printf("Done.\n" ); // Magius(CHE)
		printf("World Loaded.\n");
		return;
	}
	imem=100;
	// 10th October, 1999 removed * sizeof( int ) by Abaddon
	printf("Allocating inital dynamic Item memory of %i... ",imem);
	
	items.Reserve( imem );
	
	if(( loscache = new int[imem] ) == NULL ) Shutdown( FATAL_UOX3_LOSCACHE );
	if(( itemids = new int[imem] ) == NULL ) Shutdown( FATAL_UOX3_ITEMIDS );
	printf("Done\n");
	
	wscfile=fopen("uox3.wsc", "r");
	if(wscfile==NULL) 
		printf("ERROR: World data not found, using blank world instead.\n");
	else {
		do
		{
			readw3();
			if (!(strcmp(script1, "SECTION")))
			{
				if (!(strcmp(script2, "CHARACTER")))
					loadchar(str2num(script3));
				if (!(strcmp(script2, "WORLDITEM")))
					loaditem(str2num(script3));
				if (!(strcmp(script2, "GUILD")))
					Guilds->Read(str2num(script3));
			}
		}
		while (strcmp(script1, "EOF") && !feof(wscfile));

		printf("Saving world in new format.");
		do {
			cwmWorldState->savenewworld(1);
		} while ( cwmWorldState->Saving() );
		printf(" Done.\n");

		fclose(wscfile);
		wscfile = NULL;
		printf("Done.\n");
	}
}

void splitline() // For putting single words of cline into comm array
{
	int i=0;
	char *s;
	char *d;
	
	d=" ";
	s=strtok(cline,d);
	while (s!=NULL)
	{
		comm[i]=s;
		i++;
		s=strtok(NULL,d);
	}
	tnum=i;
}

int makenumber (int countx) // Converts decimal string comm[count] to int
{
	int i;
	if( comm[countx] == NULL )
		return 0;
	sscanf( comm[countx], "%i", &i );
	return i;
}

int hexnumber(int countx) // Converts hex string comm[count] to int
{
	int i;
	if( comm[countx] == NULL )
		return 0;
	sscanf( comm[countx], "%x", &i );
	return i;
}

int makenum2(char *s) // Converts string to integer
{
	int i;
	if( s == NULL )
		return 0;
	sscanf( s, "%i", &i );
	return i;
}


int cgold2(int item) // Calculate total gold
{
	int i, total=0, serial,ci;
	
	serial = items[item].serial;
	for (ci = 0;ci < contsp[serial%HASHMAX].max; ci++)
	{
		i = contsp[serial%HASHMAX].pointer[ci];
		if (i!=-1)
			if (items[i].contserial == serial)
			{
				if ((items[i].id1 == 0x0E) && (items[i].id2 == 0xED))
				{
					total += items[i].amount;
				}
				if ((items[i].type == 1) || (items[i].type==8))
				{
					total += cgold2(i);
				}
			}
	}
	return total;
}

int calcgold(int p) // Calculate total gold
{
	int i,ci,serial;
	
	serial = chars[p].serial;
	for (ci = 0;ci < contsp[serial%HASHMAX].max; ci++)
	{
		i=contsp[serial%HASHMAX].pointer[ci];
		if (i != -1 && items[i].contserial == serial && items[i].layer == 0x15)
			return (cgold2(i));
	}
	return 0;
}

int packitem(int p) // Find packitem
{
	int serial, j, ci;
	if( p == -1 ) return -1;
	int i=chars[p].packitem;
	//printf("%i %i %i %i %i %i %i\n",imem,cmem,i,items[i].contserial,chars[p].serial,items[i].layer,p);
	
	if (i>-1 && i<imem && p>-1 && p<cmem)
	{
		if (items[i].contserial==chars[p].serial && items[i].layer==0x15)
		{
			return chars[p].packitem;
		}
	}
	
	// - For some reason it's not defined, so go look for it.
	serial = chars[p].serial;
	j=serial%HASHMAX;
	for (ci=0;ci<contsp[j].max;ci++)
	{
		i=contsp[j].pointer[ci];
		if (i!=-1)
		{
			if ((items[i].contserial==serial) &&
				(items[i].layer==0x15))
			{
				chars[p].packitem=i;  //Record it for next time
				return (i);
			}
		}
	}
	return -1;
}

// Play sound effect for player or all players in range depending on bAllHear
// (default only player)

void soundeffects( int s, unsigned char a, unsigned char b, bool bAllHear )
{
	sfx[2] = a;
	sfx[3] = b;
	sfx[6] = chars[currchar[s]].x>>8;
	sfx[7] = chars[currchar[s]].x%256;
	sfx[8] = chars[currchar[s]].y>>8;
	sfx[9] = chars[currchar[s]].y%256;
	
	if( bAllHear )
	{
		for( int i = 0; i < now; i++ )
			if( ( perm[i] ) && ( inrange1p( s, i ) ) )
			{
				Network->xSend( i, sfx, 12, 0 );
			}
	}
	else
		Network->xSend( s, sfx, 12, 0 );
	return;
}



void soundeffect(int s, unsigned char a, unsigned char b) // Play sound effect for player
{
	int i;
	
	sfx[2]=a;
	sfx[3]=b;
	sfx[6]=chars[currchar[s]].x>>8;
	sfx[7]=chars[currchar[s]].x%256;
	sfx[8]=chars[currchar[s]].y>>8;
	sfx[9]=chars[currchar[s]].y%256;
	for (i=0;i<now;i++)
		if ((perm[i])&&((inrange1(s,i))||(s==i)))
		{
			Network->xSend(i, sfx, 12, 0);
		}
}

void soundeffect2(int p, unsigned char a, unsigned char b)
{
	int i;
	
	sfx[2]=a;
	sfx[3]=b;
	sfx[6]=chars[p].x>>8;
	sfx[7]=chars[p].x%256;
	sfx[8]=chars[p].y>>8;
	sfx[9]=chars[p].y%256;
	for (i=0;i<now;i++)
		if ((perm[i])&&(inrange1p(p, currchar[i])))
		{
			Network->xSend(i, sfx, 12, 0);
		}
}

void soundeffect5( UOXSOCKET s, unsigned char a, unsigned char b )
{
	sfx[2] = a;
	sfx[3] = b;
	sfx[6] = (unsigned char)(chars[currchar[s]].x>>8);
	sfx[7] = (unsigned char)(chars[currchar[s]].x%256);
	sfx[8] = (unsigned char)(chars[currchar[s]].y>>8);
	sfx[9] = (unsigned char)(chars[currchar[s]].y%256);
	if( s != -1 ) Network->xSend( s, sfx, 12, 0 );
}

void soundeffect3(int p, unsigned char a, unsigned char b)
{
	int i;
	
	sfx[2]=a;
	sfx[3]=b;
	sfx[6]=items[p].x>>8;
	sfx[7]=items[p].x%256;
	sfx[8]=items[p].y>>8;
	sfx[9]=items[p].y%256;
	for (i=0;i<now;i++)
		if ((perm[i])&&(inrange2(i, p)))
		{
			Network->xSend(i, sfx, 12, 0);
		}
}

void soundeffect4( int p, UOXSOCKET s, unsigned char a, unsigned char b )
{
	sfx[2] = a;
	sfx[3] = b;
	sfx[6] = (unsigned char)(items[p].x>>8);
	sfx[7] = (unsigned char)(items[p].x%256);
	sfx[8] = (unsigned char)(items[p].y>>8);
	sfx[9] = (unsigned char)(items[p].y%256);
	if( s != -1 ) Network->xSend( s, sfx, 12, 0 );
}

void action( UOXSOCKET s, int x) // Character does a certain action
{
	unsigned int i;
	
	doact[1] = chars[currchar[s]].ser1;
	doact[2] = chars[currchar[s]].ser2;
	doact[3] = chars[currchar[s]].ser3;
	doact[4] = chars[currchar[s]].ser4;
	doact[5] = x>>8;
	doact[6] = x%256;
	Network->xSend( s, doact, 14, 0 );
	for( i = 0; i < now; i++ ) 
		if( perm[i] && inrange1( s, i ) ) 
			Network->xSend( i, doact, 14, 0 );
}

void sysbroadcast( char *txt ) // System broadcast in bold text
{
	int tl, i;
	
	tl=44+strlen(txt)+1;
	talk[1]=tl>>8;
	talk[2]=tl%256;
	talk[3]=1;
	talk[4]=1;
	talk[5]=1;
	talk[6]=1;
	talk[7]=1;
	talk[8]=1;
	talk[9]=1;
	talk[10]=0x08;
	talk[11]=0x4d;
	talk[12]=0;
	talk[13]=0;
	for (i=0;i<now;i++)
	{
		if (perm[i])
		{
			Network->xSend(i, talk, 14, 0);
			Network->xSend(i, sysname, 30, 0);
			Network->xSend(i, txt, strlen(txt)+1, 0);
		}
	}
	Network->ClearBuffers();
}

//char sysname[31]="System\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

void __cdecl sysmessage(int s, char *txt, ...) // System message (In lower left corner)
{
	va_list argptr;
	if(s==-1) return;
	//	int tl=44+strlen(txt)+1;
	int tl;
	char msg[512];
	va_start( argptr, txt );
	vsprintf( msg, (char *)txt, argptr );
	va_end( argptr );
	tl = 44 + strlen( msg ) + 1;
	talk[1]=tl>>8;
	talk[2]=tl%256;
	talk[3]=1;
	talk[4]=1;
	talk[5]=1;
	talk[6]=1;
	talk[7]=1;
	talk[8]=1;
	talk[9]=0;
	talk[10]=0x00;
	talk[11]=0x40;
	talk[12]=0;
	talk[13]=3;
	Network->xSend(s, talk, 14, 0);
	Network->xSend(s, sysname, 30, 0);
	Network->xSend(s, msg, strlen(msg)+1, 0);
}

void itemmessage(UOXSOCKET s, char *txt, unsigned char a1, unsigned char a2, unsigned char a3, unsigned char a4, unsigned char loColour, unsigned char hiColour ) // The message when an item is clicked
{
	int tl;
	tl = 44 + strlen(txt)+1;
	talk[1] = tl>>8;
	talk[2] = tl%256;
	talk[3] = a1;
	talk[4] = a2;
	talk[5] = a3;
	talk[6] = a4;
	talk[7] = 1;
	talk[8] = 1;
	talk[9] = 6; // Mode: "You see"
	talk[10] = loColour;
	talk[11] = hiColour;
	talk[12] = 0;
	talk[13] = 3;
	Network->xSend(s, talk, 14, 0);
	Network->xSend(s, sysname, 30, 0);
	Network->xSend(s, txt, strlen(txt)+1, 0);
}

void wornitems( UOXSOCKET s, CHARACTER j) // Send worn items of player j
{
	int i,serial,serhash,ci;

	chars[j].onhorse=0;
	serial=chars[j].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if (i!=-1)
			if ((items[i].contserial==serial) && (items[i].free==0))
			{
				if (items[i].layer==0x19) chars[j].onhorse=1;
				wearitem[1]=items[i].ser1;
				wearitem[2]=items[i].ser2;
				wearitem[3]=items[i].ser3;
				wearitem[4]=items[i].ser4;
				wearitem[5]=items[i].id1;
				wearitem[6]=items[i].id2;
				wearitem[8]=items[i].layer;
				wearitem[9]=items[i].cont1;
				wearitem[10]=items[i].cont2;
				wearitem[11]=items[i].cont3;
				wearitem[12]=items[i].cont4;
				wearitem[13]=items[i].color1;
				wearitem[14]=items[i].color2;
				Network->xSend(s, wearitem, 15, 0);
			}
	}
}

void backpack(UOXSOCKET s, unsigned char a1, unsigned char a2, unsigned char a3, unsigned char a4) // Send Backpack (with items)
{
	int i, count=0,serial,serhash,ci;
	char bpopen[13]="\x24\x40\x0B\x00\x1A\x00\x3C\x3C\x00\x05\x00\x00";
	serial=calcserial(a1,a2,a3,a4);
	if( serial == -1 ) return;
	serhash=serial%HASHMAX;
	for (i=0;i<contsp[serhash].max;i++)
	{
		ci=contsp[serhash].pointer[i];
		if (ci!=-1)
			if ((items[ci].contserial==serial) && (items[ci].free==0))
			{
				count++;
			}
	}
	bpopen[10]=count>>8;
	bpopen[11]=count%256;
	count=(count*19)+5;
	bpopen[8]=count>>8;
	bpopen[9]=count%256;
	bpopen[1]=a1;
	bpopen[2]=a2;
	bpopen[3]=a3;
	bpopen[4]=a4;
	bpopen[5]=0x00;
	bpopen[6]=0x47;
	i=findbyserial(&itemsp[serhash], serial, 0);
	if (i==-1)
	{
		printf("UOX3.CPP: backpack() couldn't find backpack: %d.\n",serial);
		return;
	}
	
	// krazyglue 10/12/99 - rewriten from if-structure to switch-case for speed
	switch(items[i].id1)
	{
	case 0x0E:
		switch(items[i].id2)
		{
		case 0x75:      // backpack
		case 0x79:      // box/pouch
			bpopen[6]=0x3C;
			break;
		case 0x76:      // leather bag
			bpopen[6]=0x3D;
			break;
		case 0x77:      // barrel
		case 0x7F:      // keg
		case 0x83:		// tub
			bpopen[6]=0x3E;
			break;
		case 0x7A:      // square basket
			bpopen[6]=0x3F;
			break;
		case 0x40:      // metal & gold chest
		case 0x41:      // metal & gold chest
			bpopen[6]=0x42;
			break;
		case 0x7D:      // wooden box
			bpopen[6]=0x43;
			break;
		case 0x3C:      // large wooden crate
		case 0x3D:      // large wooden crate
		case 0x3E:      // small wooden create
		case 0x3F:      // small wooden crate
		case 0x7E:      // wooden crate
			bpopen[6]=0x44;
			break;
		case 0x42:      // wooden & gold chest
		case 0x43:      // wooden & gold chest
			bpopen[6]=0x49;
			break;
		case 0x7C:      // silver chest
			bpopen[6]=0x4A;
			break;
		case 0x80:      // brass box
			bpopen[6]=0x4B;
			break;
		}
		break;
		
	case 0x09:
		switch(items[i].id2)
		{
		case 0xAA:      // wooden box
			bpopen[6]=0x43;
			break;
        case 0xA8:      // metal box
			bpopen[6]=0x4B;	// fix from Daemar
			break;
        case 0x90:      // round basket
			bpopen[6]=0x41;
			break;
        case 0xA9:      // small wooden crate
			bpopen[6]=0x44;
			break;
        case 0xAB:      // metal & silver chest
			bpopen[6]=0x4A;
			break;
		case 0xAC:
		case 0xB1:
			bpopen[6] = 0x41;
			break;
		case 0xB0:
			bpopen[6] = 0x3C;
			break;
        case 0xB2:      // bank box ..OR.. backpack 2
			if (items[i].morex == 1) bpopen[6]=0x4A;
			else bpopen[6]=0x3C;
			break;
		}
		break;
			
	case 0x0A:
		switch(items[i].id2)
		{
		case 0x30:   // chest of drawers (fancy)
		case 0x38:   // chest of drawers (fancy)
			bpopen[6]=0x48;
			break;
			
		case 0x4C:   // fancy armoire (open)
		case 0x4D:   // fancy armoire
		case 0x50:   // fancy armoire (open)
		case 0x51:   // fancy armoire
			bpopen[6]=0x4E;
			break;
			
		case 0x4E:   // wooden armoire (open)
		case 0x4F:   // wooden armoire
		case 0x52:   // wooden armoire (open)
		case 0x53:   // wooden armoire
			bpopen[6]=0x4F;
			break;
			
		case 0x97:   // bookcase
		case 0x98:   // bookcase
		case 0x99:   // bookcase
		case 0x9A:   // bookcase
		case 0x9B:   // bookcase
		case 0x9C:   // bookcase
		case 0x9D:   // bookcase (empty)
		case 0x9E:	 // bookcase (empty)
			bpopen[6]=0x4D;
			break;
			
		case 0x2C:   // chest of drawers (wood)
		case 0x34:   // chest of drawers (wood)
		case 0x35:   // dresser
		case 0x3C:   // dresser
		case 0x3D:   // dresser
		case 0x44:   // dresser
			bpopen[6]=0x51;
			break;
		}
		break;
				
	case 0x20:
		switch(items[i].id2)
		{
		case 0x06:      // coffin
			bpopen[6]=0x09;
			break;
		}
		break;
					
	case 0x3E:            // boats
		bpopen[6]=0x4C;
		break;

	case 0x0F:	// barrel with lids
		if( items[i].id2 == 0xAE )
			bpopen[6] = 0x3E;
		break;

	case 0x1A:	// potion kegs
		if( items[i].id2 == 0xD7 )
			bpopen[6] = 0x3E;
		break;

	case 0x19:	// barrel with lids
		if( items[i].id2 == 0x40 )
			bpopen[6] = 0x3E;
		break;
   }
   
   Network->xSend(s, bpopen, 12, 0);
   for (i=0;i<contsp[serhash].max;i++)
   {
      ci=contsp[serhash].pointer[i];
      if (ci!=-1)
      if ((items[ci].contserial==serial) && (items[ci].free==0))
		   {
			   //fix location of items if they mess up. (needs tweaked for container types)
			   if (items[ci].x>150) items[ci].x=150;
			   if (items[ci].y>140) items[ci].y=140;
			   //end fix
			   bpitem[0]=items[ci].ser1;
			   bpitem[1]=items[ci].ser2;
			   bpitem[2]=items[ci].ser3;
			   bpitem[3]=items[ci].ser4;
			   bpitem[4]=items[ci].id1;
			   bpitem[5]=items[ci].id2;
			   bpitem[7]=items[ci].amount>>8;
			   bpitem[8]=items[ci].amount%256;
			   bpitem[9]=items[ci].x>>8;
			   bpitem[10]=items[ci].x%256;
			   bpitem[11]=items[ci].y>>8;
			   bpitem[12]=items[ci].y%256;
			   bpitem[13]=a1;
			   bpitem[14]=a2;
			   bpitem[15]=a3;
			   bpitem[16]=a4;
			   bpitem[17]=items[ci].color1;
			   bpitem[18]=items[ci].color2;
			   //   bpitem[21]=items[ci].decaytime=0;// reseting the decaytimer in the backpack
			   bpitem[19]=items[ci].decaytime=0;//HoneyJar // reseting the decaytimer in the backpack
			   Network->xSend(s, bpitem, 19, 0);
		   }
   }
   // Network->xSend(s, restart, 2, 0);
}

void backpack2(UOXSOCKET s, unsigned char a1, unsigned char a2, unsigned char a3, unsigned char a4) // Send corpse stuff
{
	int i, count=0, count2,serial,serhash,ci;
	char bpopen2[6]="\x3C\x00\x05\x00\x00";
	char display1[8]="\x89\x00\x0D\x40\x01\x02\x03";
	char display2[6]="\x01\x40\x01\x02\x03";
	
	serial=calcserial(a1,a2,a3,a4);
	serhash=serial%HASHMAX;
	for (i=0;i<contsp[serhash].max;i++)
	{
		ci=contsp[serhash].pointer[i];
		if (ci!=-1)
			if ((items[ci].contserial==serial) && (items[ci].layer!=0) && (items[ci].free==0))
			{
				count++;
			}
	}
	count2=(count*5)+7 + 1 ; // 5 bytes per object, 7 for this header and 1 for
	// terminator
	display1[1]=count2>>8;
	display1[2]=count2%256;
	display1[3]=a1;
	display1[4]=a2;
	display1[5]=a3;
	display1[6]=a4;
	Network->xSend(s, display1, 7, 0);
	for (i=0;i<contsp[serhash].max;i++)
	{
		ci=contsp[serhash].pointer[i];
		if (ci!=-1)
			if ((items[ci].contserial==serial) && (items[ci].layer!=0))
			{
				display2[0]=items[ci].layer;
				display2[1]=items[ci].ser1;
				display2[2]=items[ci].ser2;
				display2[3]=items[ci].ser3;
				display2[4]=items[ci].ser4;
				Network->xSend(s, display2, 5, 0);
			}
	}
	
	// Terminate with a 0
	char nul = 0;
	Network->xSend(s, &nul, 1, 0);
	
	// printf("Items: %i\n",count);
	bpopen2[3]=count>>8;
	bpopen2[4]=count%256;
	count2=(count*19)+5;
	bpopen2[1]=count2>>8;
	bpopen2[2]=count2%256;
	// Network->xSend(s, pause, 2, 0);
	Network->xSend(s, bpopen2, 5, 0);
	for (i=0;i<contsp[serhash].max;i++)
	{
		ci=contsp[serhash].pointer[i];
		if (ci!=-1)
			if ((items[ci].contserial==serial) && (items[ci].layer!=0))
			{
				bpitem[0]=items[ci].ser1;
				bpitem[1]=items[ci].ser2;
				bpitem[2]=items[ci].ser3;
				bpitem[3]=items[ci].ser4;
				bpitem[4]=items[ci].id1;
				bpitem[5]=items[ci].id2;
				bpitem[7]=items[ci].amount>>8;
				bpitem[8]=items[ci].amount%256;
				bpitem[9]=items[ci].x>>8;
				bpitem[10]=items[ci].x%256;
				bpitem[11]=items[ci].y>>8;
				bpitem[12]=items[ci].y%256;
				bpitem[13]=a1;
				bpitem[14]=a2;
				bpitem[15]=a3;
				bpitem[16]=a4;
				bpitem[17]=items[ci].color1;
				bpitem[18]=items[ci].color2;
				bpitem[19]=items[ci].decaytime=0;// reseting the decaytimer in the backpack  //moroallan
				Network->xSend(s, bpitem, 19, 0);
			}
	}
	// Network->xSend(s, restart, 2, 0);
}

void sendbpitem(int s, int i) // Update single item in backpack
{
//	int count,x1,x2,x3,x4,j,x,c,change,serial;
	int count, j, x, c, change, serial;
	unsigned char x1, x2, x3, x4;
	
	char display3[2]="\x25";
	bpitem[0]=items[i].ser1;
	bpitem[1]=items[i].ser2;
	bpitem[2]=items[i].ser3;
	bpitem[3]=items[i].ser4;
	//AntiChrist - world light sources stuff
	//if players is a gm, this item
	//is shown like a candle (so that he can move it),
	//....if not, the item is a normal
	//invisible light source!
	if( chars[currchar[s]].priv&1 && items[i].id1 == 0x16 && items[i].id2 == 0x47 )
	{
		bpitem[4]=0x0A;
		bpitem[5]=0x0F;
	}
	else
	{
		bpitem[4]=items[i].id1;
		bpitem[5]=items[i].id2;
	}
	bpitem[7]=items[i].amount>>8;
	bpitem[8]=items[i].amount%256;
	bpitem[9]=items[i].x>>8;
	bpitem[10]=items[i].x%256;
	bpitem[11]=items[i].y>>8;
	bpitem[12]=items[i].y%256;
	bpitem[13]=items[i].cont1;
	bpitem[14]=items[i].cont2;
	bpitem[15]=items[i].cont3;
	bpitem[16]=items[i].cont4;
	if( chars[currchar[s]].priv&1 && items[i].id1 == 0x16 && items[i].id2 == 0x47 )
	{ // let's show the lightsource like a blue item
		bpitem[17] = 0x00;
		bpitem[18] = (unsigned char)(0xC6);
	}
	else
	{
		bpitem[17]=items[i].color1;
		bpitem[18]=items[i].color2;
	}
	//bpitem[21]=items[i].decaytime=0; // reseting the decaytimer in the backpack
	bpitem[19]=items[i].decaytime=0; // HoneyJar, array range is 0-19 ! //reseting the decaytimer in the backpack
	
	// we need to find the topmost container that the item is in
	// be it a character or another container.
	
	c = -1;
	x = -1;
	x1 = items[i].cont1;
	x2 = items[i].cont2;
	x3 = items[i].cont3;
	x4 = items[i].cont4;
	serial = items[i].contserial = calcserial(x1,x2,x3,x4);  //error correcting code
	count=0;
	do
	{
		change=0;
		if (x1>=0x40)
		{
			j=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
			if (j!=-1)
			{
				x = j;
				change=1;
				x1 = items[j].cont1;
				x2 = items[j].cont2;
				x3 = items[j].cont3;
				x4 = items[j].cont4;
				serial=items[j].contserial=calcserial(x1,x2,x3,x4);  //error correcting code
			}
		}
		else
		{
			j=findbyserial(&charsp[serial%HASHMAX], serial, 1);
			if (j!=-1)
			{
				change=1;
				c = j;
				x1 = 255;
			}
		}
		count++;
	} while ( ( x1 != 255 && change == 1 ) && count < 999 );
	
	if (!change)
	{
		// printf("UOX3: Sendbpitem bug. Item %i not in container.\n",serial); // commented out by LB, myrdaal lag-bugfix generates a lot of harmless msendbp essages
		return;
	}
	if (count>=999) printf("sendbpitem: inf-loop avoid\n");
	if (((c!=-1)&&(inrange1p(currchar[s],c)))|| // if item is in a character's
		//pack (or subcontainer) and player is in range
		((c==-1)&&(inrange2(s,x))))  // or item is in container on ground and
		// container is in range
	{
		Network->xSend(s, display3, 1, 0);
		Network->xSend(s, bpitem, 19, 0);
	}
	
	// chars[currchar[s]].weight=calcweight(s);
	Weight->NewCalc(currchar[s]);  // Ison 2-20-99
}

void senditem(UOXSOCKET s, ITEM i) // Send items (on ground)
{
	int j,pack,serial;
	unsigned char itmput[20]="\x1A\x00\x13\x40\x01\x02\x03\x20\x42\x00\x32\x06\x06\x06\x4A\x0A\x00\x00\x00";
	
	if (i==-1) return; //LB
	if( items[i].visible >= 1 && !(chars[currchar[s]].priv&1 ) )
		return;

	// meaning of the item's attribute visible
	// Visible 0 -> visible to everyone
	// Visible 1 -> only visible to owner and gm's (for owners normal for gm's grayish/hidden color)
	// Visible 2 -> only visible to gm's (greyish/hidden color)
	if (items[i].cont1!=255)
	{
		pack=1;
		if (items[i].cont1<0x40)
		{
			serial=items[i].contserial;
			if( serial == -1 ) return;
			j=findbyserial(&charsp[serial%HASHMAX], serial, 1);
			if (j!=-1)
				if (chars[j].serial==serial) pack=0;
		}
		if (pack)
		{
			if( items[i].id1 < 0x40 ) // Client crashfix, no show multis in BP
			{
				sendbpitem(s,i);
				return;
			}
		}
	}
	
	if ((items[i].contserial==-1) && (inrange2(s,i)))
	{
		itmput[3]=(items[i].ser1)+0x80; // Enable Piles
		itmput[4]=items[i].ser2;
		itmput[5]=items[i].ser3;
		itmput[6]=items[i].ser4;

		// if player is a gm, this item
		// is shown like a candle (so that he can move it),
		// ....if not, the item is a normal
		// invisible light source!
		if( chars[currchar[s]].priv&1 && items[i].id1 == 0x16 && items[i].id2 == 0x47 )
		{
			itmput[7] = 0x0A;
			itmput[8] = 0x0F;
		}
		else
		{
			itmput[7]=items[i].id1;
			itmput[8]=items[i].id2;
		}

		itmput[9]=items[i].amount>>8;
		itmput[10]=items[i].amount%256;
		itmput[11]=items[i].x>>8;
		itmput[12]=items[i].x%256;
		itmput[13]=(items[i].y>>8)+0xC0; // Enable Dye and Move
		itmput[14]=items[i].y%256;
		itmput[15]=items[i].z;
		if( chars[currchar[s]].priv&1 && items[i].id1 == 0x16 && items[i].id2 == 0x47 )
		{
			itmput[16] = 0x00;
			itmput[17] = 0xC6;
		}
		else
		{
			itmput[16]=items[i].color1;
			itmput[17]=items[i].color2;
		}
		itmput[18]=0;
		int dontsendcandidate = 0;
		if (items[i].visible==1)
		{
			if (chars[currchar[s]].serial!=items[i].ownserial )
			{
				dontsendcandidate = 1;
				itmput[18] += 0x80;
			}
		}
		if( dontsendcandidate && ( chars[currchar[s]].priv*0x01 ) == 0 ) return;
		if (items[i].visible==2 )
		{
				itmput[18] += 0x80;
		}
		
		if (items[i].magic==1) itmput[18]+=0x20;
		if (chars[currchar[s]].priv2&1) itmput[18]+=0x20;
		if ((items[i].magic==3 || items[i].magic==4) && chars[currchar[s]].serial==items[i].ownserial)
			itmput[18]+=0x20;
		if (chars[currchar[s]].priv2&4)
		{
			if ((items[i].id1==0x40) && (items[i].id2<=0xFF))
			{
				itmput[7]=0x14;
				itmput[8]=0xf0;
			}
		}
		int dir = 0;
		if (items[i].dir)
		{
			dir = 1;
			itmput[19]=itmput[18];
			itmput[18]=itmput[17];
			itmput[17]=itmput[16];
			itmput[16]=itmput[15];
			itmput[15]=items[i].dir;
			itmput[2]=0x14;
			itmput[11]+=0x80;
		}
			itmput[2] = 0x13+dir;
			Network->xSend(s, itmput, 19+dir, 0);
		if ((items[i].id1==0x20)&&(items[i].id2==0x06))
		{
			backpack2(s, items[i].ser1, items[i].ser2, items[i].ser3, items[i].ser4);
		}
	}
}
// sends item in differnt color and position than it actually is
// used for LSd potions now, LB 5'th nov 1999

void senditem_lsd(UOXSOCKET s, ITEM i,char color1, char color2, int x, int y, signed char z) 
{
	
	unsigned char itmput[20]="\x1A\x00\x13\x40\x01\x02\x03\x20\x42\x00\x32\x06\x06\x06\x4A\x0A\x00\x00\x00";
	
	if (i==-1) return; //LB

	if( items[i].visible >= 1 && !(chars[currchar[s]].priv&1 ) ) return;
	
	if ((items[i].contserial==-1))
	{
		itmput[3]=(items[i].ser1)+0x80; // Enable Piles
		itmput[4]=items[i].ser2;
		itmput[5]=items[i].ser3;
		itmput[6]=items[i].ser4;
		itmput[7]=items[i].id1;
		itmput[8]=items[i].id2;
		itmput[9]=items[i].amount>>8;
		itmput[10]=items[i].amount%256;
		itmput[11]=x>>8;
		itmput[12]=x%256;
		itmput[13]=(y>>8)+0xC0; // Enable Dye and Move
		itmput[14]=y%256;
		itmput[15]=z;
		itmput[16]=color1;
		itmput[17]=color2;
		itmput[18]=0;
		
		if (items[i].visible==1)
		{
			if (chars[currchar[s]].serial!=items[i].ownserial)
			{
				itmput[18]+=(unsigned char)(0x80);
			}
		}
		if (items[i].visible==2)
		{
			itmput[18]+=(unsigned char)(0x80);
		}
		if (items[i].visible==3)
		{
			if ((chars[currchar[s]].id1==0x03 && chars[currchar[s]].id2==0xDB) || !chars[currchar[s]].priv&1)
				itmput[18]+=(unsigned char)(0x80);
		}
		
		if (items[i].magic==1) itmput[18]+=0x20;
		
		if (chars[currchar[s]].priv2&1) itmput[18]+=0x20;
		if (( items[i].magic==3 || items[i].magic == 4 ) && chars[currchar[s]].serial==items[i].ownserial)
			itmput[18]+=0x20;
		if (chars[currchar[s]].priv2&4)
		{
			if ((items[i].id1==0x40) && (items[i].id2<=0xFF))
			{
				itmput[7]=(unsigned char)(0x14);
				itmput[8]=(unsigned char)(0xF0);
			}
		}
		if (items[i].dir)
		{
			itmput[19]=itmput[18];
			itmput[18]=itmput[17];
			itmput[17]=itmput[16];
			itmput[16]=itmput[15];
			itmput[15]=items[i].dir;
			itmput[2]=0x14;
			itmput[11]+=(unsigned char)(0x80);
			Network->xSend(s, itmput, 20, 0);
		} else
		{
			itmput[2]=0x13;
			Network->xSend(s, itmput, 19, 0);
		}
		
		if ((items[i].id1==0x20)&&(items[i].id2==0x06))
		{
			backpack2(s, items[i].ser1, items[i].ser2, items[i].ser3, items[i].ser4);
		}
	}
	
}


void do_lsd(UOXSOCKET s)
{
	if (rand()%15==0)
	{
		int i,c1,c2,color,ctr=0,b,xx,yy,di,icnt=0;
		signed char zz;
		int  StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
		unsigned int increment=0;
		for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
		{
			for (int a=0;a<3;a++)
			{
				int mapitemptr=-1;
				int mapitem=-1;
				int mapchar=-1;
				do //check all items in this cell
				{
					mapchar=-1;
					mapitemptr=mapRegions->GetNextItem(checkgrid+a, mapitemptr);
					if (mapitemptr==-1) break;
					mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
					if(mapitem>999999) mapchar=mapitem-1000000;
					if (mapitem!=-1 && mapitem<1000000)
					{
						i=mapitem;
						color=(items[i].color1<<8)+items[i].color2; // fetch item's color and covert to 16 bit
						if (rand()%44==0) color+=items[i].x-items[i].y; else
							color+=chars[currchar[s]].x+chars[currchar[s]].y;
						color+=rand()%3; // add random "noise"
						ctr++; 
						// lots of color consistancy checks
						color=color%0x03E9; 
						c1=color>>8;
						c2=color%256;		
						if ((((c1<<8)+c2)<0x0002) || (((c1<<8)+c2)>0x03E9) )
						{
							c1=0x03;
							c2=0xE9;
						}
						b=((((c1<<8)+c2)&0x4000)>>14)+((((c1<<8)+c2)&0x8000)>>15);   
						if (b)
						{
							c1=0x1;
							c2=rand()%255;
						}
						
						if (rand()%10==0) zz=items[i].z+rand()%33; else zz=items[i].z;
						if (rand()%10==0) xx=items[i].x+rand()%3; else xx=items[i].x;
						if (rand()%10==0) yy=items[i].y+rand()%3; else yy=items[i].y;
						di=itemdist(currchar[s],i);
						if (di<13) if (rand()%7==0) 
						{  
							icnt++;
							if (icnt%10==0 || icnt<10) senditem_lsd(s, i,c1,c2,xx,yy,zz); // attempt to cut packet-bombing by this thing
						}
					}								
					
				} while (mapitem!=-1);
			}
		} // end of mapregions loop
		
		if (rand()%33==0) 
		{
			if (rand()%10>3) soundeffect5(s, 0x00, 0xF8); // lsd sound :)
			else { int snd=rand()%19; if (snd>9) soundeffect5(s,0x01,snd-10);
			else soundeffect5(s,0,246+snd);
			}
		}
	} 
	
}

void all_items(int s) // Send ALL items to player
{
	int i;
	for (i=0;i<itemcount;i++) 
		if (items[i].free==0) 
			if( ( !items[i].visible ) || ( ( items[i].visible ) && ( chars[currchar[s]].priv&0x01 ) ) )// we're a GM, or not hidden
				senditem(s, i);
}

void chardel (int s) // Deletion of character
{
	int i, j, k, tlen, b;
	
	if (acctno[s]!=-1)
	{
		j=0;
		k=-1;
		for (i=0;i<charcount;i++)
		{
			if (acctno[s]==chars[i].account) // used to have && chars[i].free, but that would never be true
			{
				for( b = 0; b < MAX_ACCT_LOCK; b++ )
				{
					if( acctx[chars[i].account].lock[b] == chars[i].serial )
					{
						Network->Disconnect( s );
						return;
					}
				}
				if (j==buffer[s][0x22]) 
				{
					k=i;
					break;
				}
				j++;
			}
		}
		if (k!=-1)
		{
			Npcs->DeleteChar(k);
		}
		tlen=4+(5*60)+1+(startcount*63);
		login04a[1]=tlen>>8;
		login04a[2]=tlen%256;
		Network->xSend(s, login04a, 4, 0);
		j=0;
		for (i=0;i<charcount;i++)
		{
			if ((chars[i].account==acctno[s])&&(chars[i].free==0))
			{
				strcpy(login04b, chars[i].name);
				Network->xSend(s, login04b, 60, 0);
				j++;
			} 
			
		}
		for (i=0;i<60;i++) login04b[i]=0;
		for (i=j;i<5;i++)
		{
			Network->xSend(s, login04b, 60, 0);
		}
		buffer[s][0]=startcount;
		Network->xSend(s, buffer[s], 1, 0);
		for (i=0;i<startcount;i++)
		{
			login04d[0]=i;
			for (j=0;j<=strlen(start[i][0]);j++) login04d[j+1]=start[i][0][j];
			for (j=0;j<=strlen(start[i][1]);j++) login04d[j+32]=start[i][1][j];
			Network->xSend(s, login04d, 63, 0);
		}
	}
}

unsigned short GetFlagColour( CHARACTER src, CHARACTER trg )
{
	unsigned char guild, race;
	guild = Guilds->Compare( src, trg );
	race = Races->Compare( src, trg );
	if( chars[trg].kills > repsys.maxkills )
		return 0x0026;
	else if (guild == 1 || race == 2 ) //Same guild (Green)
		return 0x0043;
	else if (guild == 2 || race == 1 ) //enemy (Orange)
		return 0x0030;
//	else if( !chars[trg].npc && chars[trg].commandLevel > 0 )
//		return 0x0001;
	else 
	{
		if( chars[trg].flag&0x01 )
			return 0x0026;
		else if( chars[trg].flag&0x02 )
			return 0x03B2;
		else if( chars[trg].flag&0x08 )
			return 0x0049;
		else if( chars[trg].flag&0x10 )
			return 0x0030;
		else
			return 0x005A;
	}
	return 0;
}

void textflags (UOXSOCKET s, int i, char *name)
{
	unsigned char a1, a2, a3, a4;
	char name2[150];
	
	strcpy( name2, name );
	a1=chars[i].ser1;
	a2=chars[i].ser2;
	a3=chars[i].ser3;
	a4=chars[i].ser4;
	if( !chars[i].npc && !( chars[i].priv&1 || chars[i].priv&80 ) && (chars[i].fame >= 10000 ) ) // Morollan, only normal players have titles now
	{
		if (chars[i].id2==0x91) sprintf(name2, "Lady %s", name); //Morrolan, added Lord/Lady to title overhead
		else if (chars[i].id2==0x90) sprintf(name2, "Lord %s", name);
	}
	
	//if (chars[i].npc) itemmessage( s, "[NPC]", a1, a2, a3, a4 );
	if( chars[i].priv&4 ) strcat( name2, " (invulnerable)" );
	if(chars[i].priv2&2) strcat( name2, " (frozen) ");
	if( chars[i].guarded ) strcat( name2, " (guarded)" );
	if( chars[i].tamed && chars[i].npc && chars[i].ownserial > -1 && chars[i].npcaitype != 17 ) strcat( name2, " (tame) ");
	if( chars[i].townpriv == 2 ) sprintf( name2, "Mayor %s", name2 );
	if( !chars[i].npc && chars[i].race != 0 && chars[i].race != 65535 )	// need to check for placeholder race (Abaddon)
		sprintf( name2, "%s (%s)", name2, Races->getName( chars[i].race ) );
	if( chars[i].kills > repsys.maxkills ) strcat( name2, " [Murderer]" );
	
	
	Guilds->Title(s,i);
	
	int tl;
	tl=44+strlen(name2)+1;
	talk[1]=tl>>8;//AntiChrist
	talk[2]=tl%256;
	talk[3]=a1;
	talk[4]=a2;
	talk[5]=a3;
	talk[6]=a4;
	talk[7]=1;
	talk[8]=1;
	talk[9]=6; // Mode: "You see"
	unsigned short targColour;
	targColour = GetFlagColour( currchar[s], i );
	talk[10] = targColour>>8;
	talk[11] = targColour%256;
	talk[12]=0;
	talk[13]=3;
	Network->xSend(s, talk, 14, 0);
	Network->xSend(s, sysname, 30, 0);
	Network->xSend(s, name2, strlen(name2)+1, 0);
	//	itemmessage(s, name2, a1, a2, a3, a4);
}

void showcname (int s, int i, char b) // Singleclick text for a character
{
	unsigned char a1, a2, a3, a4;
	
	a1 = chars[i].ser1;
	a2 = chars[i].ser2;
	a3 = chars[i].ser3;
	a4 = chars[i].ser4;
    if (chars[i].squelched) itemmessage(s, " [squelched]",a1,a2,a3,a4);
	
	if ((chars[currchar[s]].priv&8)||b)
	{
		sprintf(temp, "%s [%x %x %x %x]", chars[i].name, a1, a2, a3, a4);
	}
	else
	{
		if (!(chars[i].npc))
		{
			if (chars[currchar[s]].priv&1)
			{
				sprintf(temp, "[%x %x %x %x]", a1, a2, a3, a4 );
				itemmessage(s,temp,a1,a2,a3,a4);
			}
			if (!online(i)) sprintf(temp, "%s (OFF)", chars[i].name);
			else strcpy(temp, chars[i].name);
		}
		else
		{
#ifdef IF_THIS_IS_WHAT_IT_LOOKS_LIKE
			int c;
			int x;
			temp[0]=0;
			x=0;
			//\/ Revert to old code for a sec..testing for bug Krozy mentioned....
			/*  while (chars[i].name[x]!=0)
			{
			y=x;
			while (chars[i].name[x]!='_' && chars[i].name[x]!=0) x++;
			strncpy(temp+y,&chars[i].name[y],x-y);
			if (chars[i].name[x]=='_')
			{
			temp[x]=' ';
			x++;
			}
			}
			temp[x]=0; */
			do
			{
				c=chars[i].name[x];
				if (/*(c!=' ')&&*/(c!=0))
				{
					//					if(c=='_')
					//						c=' ';
					//					sprintf(temp, "%s%c", temp, c);
					char local_str[2];
					if( c == '_' ) c = ' ';
					local_str[0] = c;
					local_str[1] = '\0';
					strcat( temp, local_str );
					//					sprintf( temp, "%s%c", temp, c ); // krazyglue - undefined results
				}
				x++;
			}
			while (/*(chars[i].name[x-1]!=' ')&&*/(c!=0));
			//\/
#else
			// :Terrin: Just to replace '_'s? How about this (faster/cleaner):
			strcpy( temp, chars[i].name );
			for( char *cp = strchr( temp, '_' ); cp; cp = strchr( cp, '_' ) )
			{
				*cp = ' ';
			}
#endif
		}
	}
	textflags(s, i, temp);
}

void teleport(int s) // Teleports character to its current set coordinates
{
	int i;
	int k=calcSocketFromChar(s);
	int visrange = Races->getVisRange( chars[s].race );
	
	if (k!=-1)  // If a player, move them to the appropriate XYZ
	{
		removeitem[1]=chars[s].ser1;
		removeitem[2]=chars[s].ser2;
		removeitem[3]=chars[s].ser3;
		removeitem[4]=chars[s].ser4;
		
		
		goxyz[1]=chars[s].ser1;
		goxyz[2]=chars[s].ser2;
		goxyz[3]=chars[s].ser3;
		goxyz[4]=chars[s].ser4;
		goxyz[5]=chars[s].id1;
		goxyz[6]=chars[s].id2;
		goxyz[8]=chars[s].skin1;
		goxyz[9]=chars[s].skin2;
		if(chars[s].poisoned) goxyz[10]=goxyz[10]|0x04; else goxyz[10]=0x00; //AntiChrist -- thnx to SpaceDog
		if (chars[s].hidden) goxyz[10]=goxyz[10]|0x80;
		goxyz[11]=chars[s].x>>8;
		goxyz[12]=chars[s].x%256;
		goxyz[13]=chars[s].y>>8;
		goxyz[14]=chars[s].y%256;
		goxyz[17]=chars[s].dir|0x80;
		goxyz[18]=chars[s].dispz;
		Network->xSend(k, goxyz, 19, 0);
		Weight->NewCalc(s);  // Ison 2-20-99
		statwindow(k, s);  // Ison 2-20-99
		walksequence[k]=-1;
	}
	
	for (i=0;i<now;i++) // Send the update to all players.
	{
		if (perm[i])
		{
			Network->xSend(i, removeitem, 5, 0);
			if ( inrange1p(s, currchar[i]) )
				impowncreate(i, s, 1);
		}
	}
	if (k!=-1)
	{
		int	StartGrid=mapRegions->StartGrid(chars[s].x,chars[s].y);
		
		unsigned int increment=0;
		for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
		{
			for (int a=0;a<3;a++)
			{
				int mapitemptr=-1;
				int	mapitem=-1;
				int mapchar=-1;
				do //check all items in this cell
				{
					mapchar=-1;
					mapitemptr=mapRegions->GetNextItem(checkgrid+a, mapitemptr);
					if (mapitemptr==-1) break;
					mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
					if(mapitem>999999) mapchar=mapitem-1000000;
					if (mapitem!=-1 && mapitem>=1000000)
					{
						i=mapchar;
						if ((chars[i].npc||online(i)||chars[s].priv&1)&&(s!=i)&&(inrange1p(s, i) && server_data.showloggedoutpcs ))
						{
							impowncreate(k, i, 1);
						}
					} else if (mapitem!=-1) {
						if(iteminrange(k,mapitem, visrange ))
						{ 
							if( ( !items[mapitem].visible ) || ( ( items[mapitem].visible ) && ( chars[k].priv&0x01 ) ) )// we're a GM, or not hidden
								senditem(k, mapitem);
						}
					}
				} while (mapitem!=-1);
			}
		}
	}
	
	checkregion(s);
}

void checktele(int s)
{
	if ((chars[currchar[s]].x==1296)&&(chars[currchar[s]].y==1081))
	{
		chars[currchar[s]].x=5587;
		chars[currchar[s]].y=631;
		chars[currchar[s]].z=0;
		chars[currchar[s]].dispz=0;
		teleport(currchar[s]);
	}
}


void teleport2(int s) // used for /RESEND only - Morrolan, so people can find their corpses
{
	int i;
	int k=calcSocketFromChar(s);
	
	for (i=0;i<now;i++)
		if ((perm[i])&&(i!=k))
		{
			removeitem[1]=chars[s].ser1;
			removeitem[2]=chars[s].ser2;
			removeitem[3]=chars[s].ser3;
			removeitem[4]=chars[s].ser4;
			Network->xSend(i, removeitem, 5, 0);
		}
		if (k!=-1)  // If a player, move them to the appropriate XYZ
		{
			goxyz[1]=chars[s].ser1;
			goxyz[2]=chars[s].ser2;
			goxyz[3]=chars[s].ser3;
			goxyz[4]=chars[s].ser4;
			goxyz[5]=chars[s].id1;
			goxyz[6]=chars[s].id2;
			goxyz[8]=chars[s].skin1;
			goxyz[9]=chars[s].skin2;
			goxyz[10]=0;
			if (chars[s].hidden) goxyz[10]=0x80;
			goxyz[11]=chars[s].x>>8;
			goxyz[12]=chars[s].x%256;
			goxyz[13]=chars[s].y>>8;
			goxyz[14]=chars[s].y%256;
			goxyz[17]=chars[s].dir|0x80;
			goxyz[18]=chars[s].dispz;
			Network->xSend(k, goxyz, 19, 0);
			all_items(k);
			printf("ALERT: all_items() called in teleport2(). This function could cause a lot of lag!" );
			Weight->NewCalc(s);  // Ison 2-20-99
			statwindow(k, s);  // Ison 2-20-99
			walksequence[k]=-1;
		}
		for (i=0;i<now;i++) // Send the update to all players.
		{
			// Dupois - had to remove the && (k!=i)), doesn update the client
			// Added Oct 08, 1998
			if ( perm[i] && inrange1p(s, currchar[i]) )// && (k!=i)) // If inrange, and a player
			{
				impowncreate(i, s, 1);
			}
		}
		if (k!=-1)
		{
			for (i=0;i<charcount;i++)
			{ //Tauriel only send inrange people (walking takes care of out of view)
				if ((online(i)||chars[i].npc||(chars[s].priv&1))&&(s!=i)&&(inrange1p(s, i)))
				{
					impowncreate(k, i, 1);
				}
			}
			if (perm[k]) dolight(k, worldcurlevel);
		}
		checkregion(s);
}


void updatechar(int c) // If character status has been changed (Polymorph), resend him
{
	int i;
	
	setcharflag( c ); // AntiChrist - bugfix for highlight color not being updated
	for (i=0;i<now;i++)
		if (perm[i])
		{
			removeitem[1]=chars[c].ser1;
			removeitem[2]=chars[c].ser2;
			removeitem[3]=chars[c].ser3;
			removeitem[4]=chars[c].ser4;
			Network->xSend(i, removeitem, 5, 0);
			if (currchar[i]==c)
			{
				goxyz[1]=chars[c].ser1;
				goxyz[2]=chars[c].ser2;
				goxyz[3]=chars[c].ser3;
				goxyz[4]=chars[c].ser4;
				goxyz[5]=chars[c].id1;
				goxyz[6]=chars[c].id2;
				goxyz[8]=chars[c].skin1;
				goxyz[9]=chars[c].skin2;
				if(chars[c].poisoned) goxyz[10]=0x04; else goxyz[10]=0x00;  //AntiChrist -- thnx to SpaceDog
				if (chars[c].hidden) goxyz[10]=goxyz[10]|0x80;
				goxyz[11]=chars[c].x>>8;
				goxyz[12]=chars[c].x%256;
				goxyz[13]=chars[c].y>>8;
				goxyz[14]=chars[c].y%256;
				goxyz[17]=chars[c].dir|0x80;
				goxyz[18]=chars[c].z;
				Network->xSend(i, goxyz, 19, 0);
				walksequence[i]=-1;
			}
			if (inrange1p(c, currchar[i]))
			{
				impowncreate(i, c, 0);
			}
		}
}


void target(UOXSOCKET s, unsigned char a1, unsigned char a2, unsigned char a3, unsigned char a4, char *txt) // Send targetting cursor to client
{
	char tarcrs[20]="\x6C\x01\x40\x01\x02\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	
	targetok[s]=1;
	tarcrs[2]=a1;
	tarcrs[3]=a2;
	tarcrs[4]=a3;
	tarcrs[5]=a4;
	sysmessage(s, txt);
	Network->xSend(s, tarcrs, 19, 0);
}



void teleporters(int s)
{
	int i;
	
	for(i=0;i<max_tele_locations;i++)
	{
		if(chars[s].x == tele_locations[i][0])
		{
			if(chars[s].y == tele_locations[i][1])
			{
				if((tele_locations[i][2] == 999)||(chars[s].z == tele_locations[i][2]))
				{
					mapRegions->RemoveItem(s+1000000);
					chars[s].x=tele_locations[i][3];
					chars[s].y=tele_locations[i][4];
					chars[s].dispz=chars[s].z=tele_locations[i][5];
					mapRegions->AddItem(s+1000000);
					teleport(s);
					
				}
			}
			else
				if(chars[s].y < tele_locations[i][1])
					break;
		}
		else
			if(chars[s].x < tele_locations[i][0])
				break;
	}
}

void read_in_teleport(void)
{
	FILE *fp;
	char text[256];
	int i;
	char seps[]   = " ,\t\n";
	char *token;
	
	fp = fopen("teleport.scp","r");
	max_tele_locations=0;
	
	if(fp==NULL)
	{
		printf("ERROR: Teleport Data not found\n");
		error=1;
		keeprun=0;
		return;
	}
	
	while((!feof(fp))&&(max_tele_locations<MAX_TELE_LOCATIONS))
	{
		fgets(text,255,fp);
		
		if(text[0]!=';')
		{
			tele_locations[max_tele_locations][0] = 0;
			tele_locations[max_tele_locations][1] = 0;
			tele_locations[max_tele_locations][2] = 0;
			tele_locations[max_tele_locations][3] = 0;
			tele_locations[max_tele_locations][4] = 0;
			tele_locations[max_tele_locations][5] = 0;
			token = strtok( text, seps );
			i = 0;
			while( token != NULL )
			{
				if(i==2)
					if(token[0] == 'A')
						tele_locations[max_tele_locations][i] = 999;
					else
						tele_locations[max_tele_locations][i] = atoi(token);
					else
						tele_locations[max_tele_locations][i] = atoi(token);
					
					i++;
					if(i==6)
						break;
					
					token = strtok( NULL, seps );
			}
			max_tele_locations++;
		}
	}
	
	fclose(fp);
}


void usehairdye(int s, int x)
{
	//s is the player, x is the hair dye bottle object number
	int hairobject=-1, beardobject=-1,ci,j,dest,serial,serhash;
		
	//Find hair object.(layer 11, 0x0B)
	//Find beard object.(layer 16, 0x10)

	dest=currchar[s];
	serial=chars[dest].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		j=contsp[serhash].pointer[ci];
			if (j>-1)
			{
				if(items[j].layer==0x10)//beard
					beardobject=j;
				if(items[j].layer==0x0B)//hairs
					hairobject=j;
			}
		}
	
	//Now change the color to the hair dye bottle color!

	if (hairobject>-1) 
	{
		items[hairobject].color1=items[x].color1;
		items[hairobject].color2=items[x].color2;
		//for (j=0;j<now;j++) if (perm[j]) senditem(j,hairobject);
		RefreshItem(hairobject);//AntiChrist
	}
	if (beardobject>-1) 
	{
		items[beardobject].color1=items[x].color1;
		items[beardobject].color2=items[x].color2;
		RefreshItem(beardobject);//AntiChrist
	}
	

	//Now delete the hair dye bottle!
	Items->DeleItem(x);
}


void explodeitem(int s, unsigned int nItem)
{
	unsigned int dmg=0,len=0,c;
	int x, y,dx,dy,dz;
	// - send the effect (visual and sound)
	if( items[nItem].contserial > -1 ) // bugfix LB
	{
		staticeffect(currchar[s], 0x36, 0xB0, 0x00, 0x09);
		c=currchar[s];
		items[nItem].x=chars[c].x;
		items[nItem].y=chars[c].y;
		items[nItem].z=chars[c].z;
		soundeffect2(currchar[s], 0x02, 0x07);
		
	}
	else
	{
		staticeffect2(nItem, 0x36, 0xB0, 0x00, 0x09, 0x00);
		soundeffect3(nItem, 0x02, 0x07);
	}
	len=items[nItem].morex/250; //4 square max damage at 100 alchemy
	switch (items[nItem].morez)
	{
	case 1:	dmg = RandomNum( 5, 10 );	break;
	case 2:	dmg = RandomNum( 10, 20 );	break;
	case 3:	dmg = RandomNum( 20, 40 );	break;
	default:
		printf("ERROR: Fallout of switch statement without default. uox3.cpp, explodeitem()\n"); //Morrolan
		dmg = RandomNum(5,10);
		break;
	}
	
	if ( dmg < 5 ) 
		dmg=RandomNum(5,10);  // 5 points minimum damage
	if ( len < 2 ) 
		len=2;  // 2 square min damage range
	x = items[nItem].x;
	y = items[nItem].y;
	
	//Char mapRegions
	int getcell=mapRegions->GetCell(x,y);
	int mapitem=-1;
	int mapitemptr=-1;
	int mapchar=-1;
	int chain = 0;
	do //check all items in this cell
	{
		mapitemptr=mapRegions->GetNextItem(getcell, mapitemptr);
		if (mapitemptr==-1) break;
		mapitem=mapRegions->GetItem(getcell, mapitemptr);
		if (mapitem>99999) mapchar=mapitem-1000000;
		if (mapitem!=-1 && mapitem>=1000000)
		{
			c=mapchar;
			dx=abs(chars[c].x-items[nItem].x);
			dy=abs(chars[c].y-items[nItem].y);
			dz=abs(chars[c].z-items[nItem].z);
			if ( dx <= len && dy <= len && dz <= len )
			{
				if( !(chars[c].priv&0x01) && ( chars[c].npc || online( c ) ) )
				{
					chars[c].hp -= ( dmg + ( 2 - min( dx, dy ) ) );
					updatestats(c, 0);
					if (chars[c].hp<=0)
					{
						deathstuff(c);
					}
					else
					{
						npcattacktarget(c, currchar[s]);
					}
				}
			}
		}	
		else if( mapitem < 999999 && mapitem > -1 ) // LB's chain-reaction for explosion potions
		{
			int xItem = mapitem;
			if( items[xItem].id1 == 0x0F && items[xItem].id2 == 0x0D && items[xItem].type == 19 ) // check for expl-potions
			{
				c = currchar[s];
				dx = abs( items[nItem].x - items[xItem].x );
				dy = abs( items[nItem].y - items[xItem].y );
				dz = abs( items[nItem].z - items[xItem].z );
				
				if( dx <= 2 && dy <= 2 && dz <= 2 && chain == 0 ) // only trigger if in a 2*2*2 cube
				{
					if( !( dx == 0 && dy == 0 && dz == 0 ) )
					{
						if( rand()%2 == 1 ) chain = 1; // LB - more aggressive - :)
						tempeffect2( c, xItem, 17, 0, 1, 0 ); // trigger ...
					}
				}
			}
		}
	} while (mapitem!=-1);
	Items->DeleItem(nItem);
}

// s: player socket, I: send bolt if it rains or not ?

void skillwindow(UOXSOCKET s) // Opens the skills list
{
	int i,k;
	//unsigned short int tempskill;
	// char skillstart[5]="\x3A\x00\xBE\x00"; 
	/*	struct // size = ??? = Fill in the skills list.
	{
	BYTE m_Cmd;		// 0= 0x3A
	NWORD m_len;	// 1= varies
	BYTE m_single;	// 3=0 = all w/ 0 term, ff=single and no terminator
	struct
	{
	NWORD m_index;	// 1 based, 0 = terminator. (no val)
	NWORD m_val;	// Skill * 10 (stat modified!)
	NWORD m_valraw; // Skill * 10 (stat unmodified!)
	BYTE m_lock;	// current lock mode (0 = none, 1 = down, 2 = up, 3 = locked)
	} skills[1];
} Skill;*/
	
	//	char skillstart[5]="\x3A\x00\xCA\x00"; // hack for that 3 new skills !!!, lord binary
	
	//	char skillmid[5]="\x00\x00\x00\x00";
	//	char skillend[3]="\x00\x00";
	
	char skillstart[5] = "\x3A\x01\x5D\x00";		
	
	char skillmid[8] = "\x00\x00\x00\x00\x00\x00\x00";
	char skillend[3] = "\x00\x00";
	
	k = currchar[s];
	
	Network->xSend(s, skillstart, 4, 0);
	for (i=0;i<TRUESKILLS;i++)
	{
		Skills->updateSkillLevel(k,i);
		skillmid[1]=i+1;
		skillmid[2]=chars[currchar[s]].skill[i]>>8;
		skillmid[3]=chars[currchar[s]].skill[i]%256;
		skillmid[4]=chars[currchar[s]].baseskill[i]>>8;			// unmodified will be the same as modified for the moment
		skillmid[5]=chars[currchar[s]].baseskill[i]%256;
		//  If all chars are unsigned the >= 0 is always true, get rid of it  (punt)
		//if( !( chars[currchar[s]].lockState[i] >= 0 && chars[currchar[s]].lockState[i] <= 2 ) )
		if ( !(chars[currchar[s]].lockState[i] <= 2) ) 	//punt
			chars[currchar[s]].lockState[i] = 0;
		skillmid[6]=chars[currchar[s]].lockState[i];										// leave it unlocked, regardless
		
		Network->xSend(s, skillmid, 7, 0);
	}
	Network->xSend(s, skillend, 2, 0);
}

void updatestats(int c, char x)
{
	int i, a=0, b=0;
	char updater[10]="\xA1\x01\x02\x03\x04\x01\x03\x01\x02";
	
	updater[0]=0xA1+x;
	updater[1]=chars[c].ser1;
	updater[2]=chars[c].ser2;
	updater[3]=chars[c].ser3;
	updater[4]=chars[c].ser4;
	if (x==0)
	{
		a=chars[c].st;
		b=chars[c].hp;
	}
	if (x==2)
	{
		a=chars[c].dx;
		b=chars[c].stm;
	}
	if (x==1)
	{
		a=chars[c].in;
		b=chars[c].mn;
	}
	updater[5]=a>>8;
	updater[6]=a%256;
	updater[7]=b>>8;
	updater[8]=b%256;
	for (i=0;i<now;i++) 
		if (perm[i] && inrange1p(currchar[i], c) ) 
			Network->xSend(i, updater, 9, 0);
}

void statwindow(int s, int i) // Opens the status window
{
	int x;
	char statstring[67]="\x11\x00\x42\x00\x05\xA8\x90XYZ\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x12\x00\x34\xFF\x01\x00\x00\x5F\x00\x60\x00\x61\x00\x62\x00\x63\x00\x64\x00\x65\x00\x00\x75\x30\x01\x2C\x00\x00";
	char ghost;
	
	if (s<0 || s>=MAXCLIENT || i<0 || i>cmem) return; // lb, fixes a few (too few) -1 crashes ...
	
	if ((chars[i].id1==0x01 && chars[i].id2==0x92) || (chars[i].id1==0x01 && chars[i].id2==0x93)) ghost=1; else ghost=0;
	
	statstring[3]=chars[i].ser1;
	statstring[4]=chars[i].ser2;
	statstring[5]=chars[i].ser3;
	statstring[6]=chars[i].ser4;
	strcpy(&statstring[7], chars[i].name);
	
	if (!ghost) 
	{ 
		statstring[37]=chars[i].hp>>8;
		statstring[38]=chars[i].hp%256;
		statstring[39]=chars[i].st>>8;
		statstring[40]=chars[i].st%256;
	} else 
	{
		statstring[37]=statstring[38]=statstring[39]=statstring[40]=0;
	}
	
	
	//if ((chars[currchar[s]].priv&1)&&(currchar[s]!=i)) //Bug Fix -- Zippy
	if (((chars[currchar[s]].priv&1)||(chars[i].ownserial==chars[currchar[s]].serial))&&(currchar[s]!=i))
	{
		statstring[41]=(unsigned char)0xFF;
	}   else if ((chars[currchar[s]].serial==chars[i].ownserial)&&(currchar[s]!=i)) //Morrolan - from Banter
	{
		statstring[41]=(unsigned char)0xFF;
	}
	else
	{
		statstring[41]=0x00;
	}
	
	if (ghost) statstring[41]=0x00;
	
	// packet #42 has some problems, dont try to be smart and replace the workaround by
	// if (ghost) statstring[42]=0; else statstring[42]=1, LB
	
	if ((chars[i].id1==0x01)&&(chars[i].id2==0x91)) statstring[43]=1; 
	else if ((chars[i].id1==0x01) && (chars[i].id2==0x93)) statstring[43]=1;
	else statstring[43]=0; // LB, prevents very female looking male players ... :-)
	
	
	if (!ghost)
	{
		statstring[44]=chars[i].st>>8;
		statstring[45]=chars[i].st%256;
		statstring[46]=chars[i].dx>>8;
		statstring[47]=chars[i].dx%256;
		statstring[48]=chars[i].in>>8; // Real INT
		statstring[49]=chars[i].in%256;
		statstring[50]=chars[i].stm>>8;
		statstring[51]=chars[i].stm%256;
		statstring[52]=chars[i].dx>>8;
		statstring[53]=chars[i].dx%256;
		statstring[54]=chars[i].mn>>8;
		statstring[55]=chars[i].mn%256;
		statstring[56]=chars[i].in>>8; // MaxMana
		statstring[57]=chars[i].in%256;
	} 
	else
	{
		memset(statstring+44, 0, 14);
	}
	
	x=calcgold(i);
	statstring[58]=x>>24;
	statstring[59]=x>>16;
	statstring[60]=x>>8;
	statstring[61]=x%256;
	x=Combat->CalcDef(i,0);
	statstring[62]=x>>8; // AC
	statstring[63]=x%256;
	x=(int)(chars[i].weight);
	statstring[64]=x>>8;
	statstring[65]=x%256;
	Network->xSend(s, statstring, 66, 0);
}

void srequest(int s)
{
	if (buffer[s][5]==4) statwindow(s, calcCharFromSer(buffer[s][6], buffer[s][7], buffer[s][8], buffer[s][9]));
	if (buffer[s][5]==5) skillwindow(s);
}

void updates(int s) // Update Window
{
	int x, y, pos, j;
	char* null;
	null = " ";
	
	openscript("misc.scp");
	if (!i_scripts[misc_script]->find("MOTD"))
	{
		closescript();
		return;
	}
	pos=ftell(scpfile);
	x=-1;
	y=-2;
	do
	{
		read1();
		x++;
		y+=strlen(script1)+1;
	}
	while (strcmp(script1, "}"));
	y+=10;
	fseek(scpfile, pos, SEEK_SET);
	updscroll[1]=y>>8;
	updscroll[2]=y%256;
	updscroll[3]=2;
	updscroll[8]=(y-10)>>8;
	updscroll[9]=(y-10)%256;
	Network->xSend(s, updscroll, 10, 0);
	for (j=0;j<x;j++)
	{
		read1();
		Network->xSend(s, script1, strlen(script1), 0);
		// we are sending a space to separate each line in the motd, not a packet 0x20 - fur
		Network->xSend(s, null, 1, 0);
	}
	closescript();
}

void tips(int s, int i) // Tip of the day window
{
	int x, y, pos, j;
	char *null;
	null = " ";
	
	if (i==0) i=1;
	openscript("misc.scp");
	if (!i_scripts[misc_script]->find("TIPS"))
	{
		closescript();
		return;
	}
	x=i;
	do
	{
		read2();
		if (!(strcmp("TIP", script1))) x--;
	}
	while ((x>0)&&script1[0]!='}'&&script1[0]!=0);
	closescript();
	if (!(strcmp("}", script1)))
	{
		tips(s, 1);
		return;
	}
	openscript("misc.scp");
	sprintf(temp, "TIP %i", str2num(script2));
	if (!i_scripts[misc_script]->find(temp))
	{
		closescript();
		return;
	}
	pos=ftell(scpfile);
	x=-1;
	y=-2;
	do
	{
		read1();
		x++;
		y+=strlen(script1)+1;
	}
	while (strcmp(script1, "}"));
	y+=10;
	fseek(scpfile, pos, SEEK_SET);
	updscroll[1]=y>>8;
	updscroll[2]=y%256;
	updscroll[3]=0;
	updscroll[7]=i;
	updscroll[8]=(y-10)>>8;
	updscroll[9]=(y-10)%256;
	Network->xSend(s, updscroll, 10, 0);
	for (j=0;j<x;j++)
	{
		read1();
		Network->xSend(s, script1, strlen(script1), 0);
		// we are sending a space to separate each line in the tip, not a packet 0x20 - fur
		Network->xSend(s, null, 1, 0);
	}
	closescript();
}

// Dupois - added doorsfx() to be used with dooruse()
// Added Oct 8, 1998
// Plays the proper door sfx for doors/gates/secretdoors
void doorsfx(int item, int x, int y)
{
	const int OPENWOOD = 0xEA;
	const int OPENGATE = 0xEB;
	const int OPENSTEEL = 0xEC;
	const int OPENSECRET = 0xED;
	const int CLOSEWOOD = 0xF1;
	const int CLOSEGATE = 0xF2;
	const int CLOSESTEEL = 0xF3;
	const int CLOSESECRET = 0xF4;
	
	if (y==0) // Request open door sfx
	{
		if (((x>=0x0695)&&(x<0x06C5))|| // Open wooden / ratan door
			((x>=0x06D5)&&(x<=0x06F4)))
			soundeffect3(item, 0x00, OPENWOOD);
		
		if (((x>=0x0839)&&(x<=0x0848))|| // Open gate
			((x>=0x084C)&&(x<=0x085B))||
			((x>=0x0866)&&(x<=0x0875)))
			soundeffect3(item, 0x00, OPENGATE);
		
		if (((x>=0x0675)&&(x<0x0695))|| // Open metal
			((x>=0x06C5)&&(x<0x06D5)))
			soundeffect3(item, 0x00, OPENSTEEL);
		
		if ((x>=0x0314)&&(x<=0x0365)) // Open secret
			soundeffect3(item, 0x00, OPENSECRET);
	}
	else if (y==1) // Request close door sfx
	{
		if (((x>=0x0695)&&(x<0x06C5))|| // close wooden / ratan door
			((x>=0x06D5)&&(x<=0x06F4)))
			soundeffect3(item, 0x00, CLOSEWOOD);
		
		if (((x>=0x0839)&&(x<=0x0848))|| // close gate
			((x>=0x084C)&&(x<=0x085B))||
			((x>=0x0866)&&(x<=0x0875)))
			soundeffect3(item, 0x00, CLOSEGATE);
		
		if (((x>=0x0675)&&(x<0x0695))|| // close metal
			((x>=0x06C5)&&(x<0x06D5)))
			soundeffect3(item, 0x00, CLOSESTEEL);
		
		if ((x>=0x0314)&&(x<=0x0365)) // close secret
			soundeffect3(item, 0x00, CLOSESECRET);
	}
} // doorsfx() END


void dooruse( UOXSOCKET s, int item)
{
	int i, db, x;
	char changed = 0;
	
	if ((iteminrange(s,item,2)==0)&& s>-1) 
	{
		sysmessage(s, "You cannot reach the handle from here");
		return;
	}
	for (i=0;i<DOORTYPES;i++)
	{
		db = doorbase[i];
		
		x=((items[item].id1<<8)+items[item].id2);

		if (x==(db+0))
		{
			items[item].id2++;
			if( db != 0x190E )
			{
				items[item].x--;
				items[item].y++;
			}
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 0);
			tempeffect2(0, item, 13, 0, 0, 0);
		} else if (x==(db+1))
		{
			items[item].id2--;
			if( db != 0x190E )
			{
				items[item].x++;
				items[item].y--;
			}
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 1);
			items[item].dooropen=0;
		} else if (x==(db+2))
		{
			items[item].id2++;
			items[item].x++;
			items[item].y++;
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 0);
			tempeffect2(0, item, 13, 0, 0, 0);
		} else if (x==(db+3))
		{
			items[item].id2--;
			items[item].x--;
			items[item].y--;
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 1);
			items[item].dooropen=0;
		} else if (x==(db+4))
		{
			items[item].id2++;
			items[item].x--;
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 0);
			tempeffect2(0, item, 13, 0, 0, 0);
		} else if (x==(db+5))
		{
			items[item].id2--;
			items[item].x++;
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 1);
			items[item].dooropen=0;
		} else if (x==(db+6))
		{
			items[item].id2++;
			items[item].x++;
			items[item].y--;
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 0);
			tempeffect2(0, item, 13, 0, 0, 0);
		} else if (x==(db+7))
		{
			items[item].id2--;
			items[item].x--;
			items[item].y++;
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 1);
			items[item].dooropen=0;
		} else if (x==(db+8))
		{
			items[item].id2++;
			items[item].x++;
			items[item].y++;
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 0);
			tempeffect2(0, item, 13, 0, 0, 0);
		} else if (x==(db+9))
		{
			items[item].id2--;
			items[item].x--;
			items[item].y--;
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 1);
			items[item].dooropen=0;
		} else if (x==(db+10))
		{
			items[item].id2++;
			items[item].x++;
			items[item].y--;
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 0);
			tempeffect2(0, item, 13, 0, 0, 0);
		} else if (x==(db+11))
		{
			items[item].id2--;
			items[item].x--;
			items[item].y++;
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 1);
			items[item].dooropen=0;
		} 
		else if (x==(db+12))
		{
			items[item].id2++;
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 0);
			tempeffect2(0, item, 13, 0, 0, 0);
		} else if (x==(db+13))
		{
			items[item].id2--;
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 1);
			items[item].dooropen=0;
		} else if (x==(db+14))
		{
			items[item].id2++;
			items[item].y--;
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 0);
			tempeffect2(0, item, 13, 0, 0, 0);
		} else if (x==(db+15))
		{
			items[item].id2--;
			items[item].y++;
			RefreshItem( item ); // AntiChrist
			changed=1;
			doorsfx(item, x, 1);
			items[item].dooropen=0;
		}
 }
 if ( changed == 0 && s > -1 )
	 sysmessage( s, "This doesnt seem to be a valid door type. Contact a GM." );
}


// this function SCREAMES for a rewrite !! LB !!

void get_item(int s) // Client grabs an item
{
	int i, x,  npc = -1, c, amount, packnum, update = 0,serial;
	tile_st tile;
	int z;//antichrist for trade fix
	
	serial = calcserial( buffer[s][1], buffer[s][2], buffer[s][3], buffer[s][4] );
	if( serial == -1 ) 
		return;
	i = calcItemFromSer( serial );
	
	if( i <= -1 ) 
		return;
	breakConcentration( currchar[s], s );
	
	//Zippy's stealing changes  
	x=i;
	int b;
	
	if (items[x].contserial!=-1)  //Find character owning item
	{
		b=0;
		do  //Find character owning item
		{
			b++;
			if (items[x].cont1<0x40) // it's a character
			{
				npc=findbyserial(&charsp[items[x].contserial%HASHMAX], items[x].contserial, 1);
			} else  //its an item
			{
				if (items[x].contserial==-1)
				{
					npc=-1;
					break;
				}
				x=findbyserial(&itemsp[items[x].contserial%HASHMAX], items[x].contserial, 0);
				// ANTICHRIST -- SECURE TRADE FIX
				if (x!=-1) //LB overwriting x is essential here, odnt change it!!!
				{
					if (items[x].layer==0 && items[x].id1==0x1E && items[x].id2==0x5E)
					{
						// Trade window???
						serial=calcserial(items[x].moreb1, items[x].moreb2, items[x].moreb3, items[x].moreb4);
						if( serial == -1 ) return;
						z=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
						if (z!=-1)
						{
							if ((items[z].morez || items[x].morez))
							{
								items[z].morez=0;
								items[x].morez=0;
								sendtradestatus(z, x);
							}
							// Default item pick up sound sent to other player involved in trade
							soundeffects( calcSocketFromChar( calcCharFromSer( items[z].contserial ) ), 0x00, 0x57, false );
						}
					}
					if (items[x].corpse!=0) npc=0;
				} // end if x!=-1
				
				if (x==-1) npc=0; 
			}
		} while (npc==-1 && b < 100);
	}
	
	if (npc>0) // 0=corpse, hence >0 ..
	{
		if (!(chars[currchar[s]].priv&0x01) && npc!=currchar[s] && chars[npc].ownserial!=chars[currchar[s]].serial)
		{//Own serial stuff by Zippy -^ Pack aniamls and vendors.
			bounce[1]=0;
			Network->xSend(s, bounce, 2, 0);
			return;
		}
	}
	//End Zippy's change
	
	//Boats->
	if (x!=-1 && npc!=-1)
	{
		if( items[x].multis != -1 )
			unsetserial( x, 7 );
		items[x].decaytime=server_data.decaytimer*CLOCKS_PER_SEC+uiCurrentTime;
		//End Boats Change
		
		// AntiChrist -- for poisoned items
		if(items[x].layer>0) 
		{
			//AntiChrist -- remove BONUS STATS given by equipped special items
			chars[npc].st-=items[x].st2;
			chars[npc].dx-=items[x].dx2;
			chars[npc].in-=items[x].in2;
			//AntiChrist -- for poisoned items
			if(items[x].poisoned) 
			{
				chars[npc].poison-=items[x].poisoned;
				if(chars[npc].poison<0) chars[npc].poison=0;
			}
		}
	}
	
	if (i!=-1)
	{
		if (items[i].corpse!=1)
		{
			packnum=packitem(currchar[s]);
			if (Weight->CheckWhereItem(packnum, i, s))
			{
				//additemweight(i, currchar[s]);
				statwindow(s,currchar[s]);
				//updateweight(s);      not used yet
			}
			Map->SeekTile((items[i].id1<<8)+items[i].id2, &tile);
			if ((((items[i].magic==2)||((tile.weight==255)&&(items[i].magic!=1)))&&((chars[currchar[s]].priv2&1)==0)) ||
				(items[i].magic==3 && !(items[i].ownserial==chars[currchar[s]].serial)))
			{
				bounce[1]=0;
				Network->xSend(s, bounce, 2, 0);
				if (items[i].id1>=0x40) senditem(s, i);
			} // end if not corspe
			else
			{
				items[i].layer=0;
				if (items[i].cont1!=0xff) soundeffect5(s,0x00,0x57);
				if (items[i].amount>1)
				{
					amount=(buffer[s][5]<<8)+buffer[s][6];
					if (amount>items[i].amount) amount=items[i].amount;
					if (amount<items[i].amount)
					{
						c=Items->MemItemFree();
						Items->InitItem(c, 0);
						memcpy(&items[c], &items[i], sizeof(item_st));  // Tauriel reduce code faster too
						items[c].ser1=itemcount2>>24;
						items[c].ser2=itemcount2>>16;
						items[c].ser3=itemcount2>>8;
						items[c].ser4=itemcount2%256;  //lb
						items[c].serial=itemcount2;
						items[c].amount=items[i].amount-amount;
						//Tauriel sorry, there is no way to make this call the item creation stuff
						setptr(&itemsp[itemcount2%HASHMAX], c);
						itemcount2++; // important bugfix for items disappearing, lb
						if (items[c].contserial!=-1) setptr(&contsp[items[c].contserial%HASHMAX], c);
						if (items[c].ownserial!=-1) setptr(&ownsp[items[c].ownserial%HASHMAX], c);
						if( items[c].spawnserial != 0 && items[c].spawnserial != -1 )	// legacy support until the next build
							setptr( &spawnsp[items[c].spawnserial%HASHMAX], c );
//						if (items[c].spawnserial!=-1) setptr(&spawnsp[items[c].spawnserial%HASHMAX], c);
						
						statwindow(s,currchar[s]);
						//						for (j=0;j<now;j++) if (perm[j]) senditem(j,c);
						RefreshItem( c ); // AntiChrist
						if (c==itemcount) itemcount++;
						itemcount2++;
					}
					
					if (items[i].id1==0x0E && items[i].id2==0xED) // gold coin
					{
						if (packnum!=-1) //lb
							if (items[i].contserial==items[packnum].serial)
								update=1;
					}
					
					items[i].amount=amount;
					//					for (j=0;j<now;j++) if (j!=s) senditem(j,i);
					//					RefreshItem( i ); // AntiChrist
				} // end if corpse
				
				// Tauriel remove item from world mapcells
				mapRegions->RemoveItem(i); //remove this item from a map cell
				items[i].x=0;
				items[i].y=0;
				items[i].z=0;
				
				if (items[i].contserial!=-1) removefromptr(&contsp[items[i].contserial%HASHMAX], i);
				items[i].cont1=255;
				items[i].cont2=255;
				items[i].cont3=255;
				items[i].cont4=255;
				items[i].contserial=-1;
			}
		}
	} // end of if i!=-1
	if( i != -1 )
	{
		chars[currchar[s]].weight += ( items[i].amount * Weight->ItemWeight( i ) / 100 );
		update = 1;
	}
	if (update) statwindow(s,currchar[s]);
}


void wear_item(int s) // Item is dropped on paperdoll
{
	unsigned char a1, a2, a3, a4;
	int cserial, iserial;
	int i, j, k, pack;
	tile_st tile;
	int serial, serhash, ci, i2, letsbounce = 0; // AntiChrist (5) - new ITEMHAND system

	a1 = buffer[s][1];
	a2 = buffer[s][2];
	a3 = buffer[s][3];
	a4 = buffer[s][4];
	cserial = calcserial( buffer[s][6],buffer[s][7],buffer[s][8],buffer[s][9] );
	if( cserial == -1 ) return;
	iserial = calcserial( a1, a2, a3, a4 );
	if( iserial == -1 ) return;
	//k=-1;
	k = findbyserial( &charsp[cserial%HASHMAX], cserial, 1 );
	i = findbyserial( &itemsp[iserial%HASHMAX], iserial, 0 );
	
	if (i!=-1 && (k==currchar[s] || chars[currchar[s]].priv&1))
	{
		if( k == -1 ) 
			return;
		if( chars[currchar[s]].dead )
		{
			sysmessage( s, "You can't do much in your current state." );
			return;
		}
		if( Races->getArmorRestrict( chars[currchar[s]].race ) < Items->ArmorClass( i ) && Races->getArmorRestrict( chars[currchar[s]].race ) != 0 )
		{
			sysmessage(s, "You are unable to equip that due to your race." );
			bounce[1]=5;
			Network->xSend(s, bounce, 2, 0);
			if(items[i].id1>=0x40) senditem(s, i);
			pack=packitem(currchar[s]);
			if (Weight->CheckWhereItem(pack, i, s))
			{
				//subtractitemweight( i, currchar[s]);
				statwindow(s,currchar[s]);
			}
			return;
		}
		if (k==currchar[s] && items[i].st>chars[k].st)
		{
			sysmessage(s,"You are not strong enough to use that.");
			bounce[1]=5;
			Network->xSend(s, bounce, 2, 0);
			if(items[i].id1>=0x40) senditem(s, i);
			pack=packitem(currchar[s]);
			if (Weight->CheckWhereItem(pack, i, s))
			{
				//subtractitemweight( i, currchar[s]);
				statwindow(s,currchar[s]);
			}
			itemsfx( s, items[i].id1, items[i].id2 ); // antichrist
			return;
		}
		Map->SeekTile((items[i].id1<<8)+items[i].id2, &tile);
		if ((((items[i].magic==2)||((tile.weight==255)&&(items[i].magic!=1)))&&((chars[currchar[s]].priv2&1)==0)) ||
			(items[i].magic==3 && !(items[i].ownserial==chars[currchar[s]].serial)))
		{
			bounce[1]=5;
			Network->xSend(s, bounce, 2, 0);
			if (items[i].id1>=0x40) senditem(s, i);
			return;
		}
		
		// - AntiChrist (4) - checks for new ITEMHAND system
		// - now you can't equip 2 hnd weapons with 1hnd weapons nor shields!!
		serial = chars[currchar[s]].serial;
		serhash = serial%HASHMAX;
		for( ci = 0; ci < contsp[serhash].max; ci++ )
		{
			i2 = contsp[serhash].pointer[ci];
			if(( i2 != -1 ) && ( items[i2].contserial == serial ) && ( items[i2].itmhand == 2 ) && ( items[i].itmhand == 1 ))
			{
				sysmessage( s, "Your hands are both occupied!" );
				letsbounce = 1;
			}
			if(( i2 != -1 ) && ( items[i2].contserial == serial ) && ( items[i2].itmhand == 1 ) && ( items[i].itmhand == 2 ))
			{
				sysmessage( s, "You cannot equip a two handed weapon with a weapon equipped!" );
				letsbounce = 1;
			}
			if(( i2 != -1 ) && ( items[i2].contserial == serial ) && ( items[i2].itmhand == 2 ) && ( items[i].itmhand == 3 ))
			{
				sysmessage( s, "You cannot equip a shield with a two handed weapon equipped!" );
				letsbounce = 1;
			}
			if(( i2 != -1 ) && ( items[i2].contserial == serial ) && ( items[i2].itmhand == 3 ) && ( items[i].itmhand == 2 ))
			{
				sysmessage( s, "You cannot equip a two handed weapon with a shield equipped!" );
				letsbounce = 1;
			}
			if( letsbounce ) // Let's bounce the item
			{
				bounce[1] = 5;
				Network->xSend( s, bounce, 2, 0 );
				if( items[i].id1 >= 0x40 ) senditem( s, i );
				pack = packitem( currchar[s] );
				if( Weight->CheckWhereItem( pack, i, s ) )
				{
					statwindow( s, currchar[s] );
				}
				itemsfx( s, items[i].id1, items[i].id2 );  // antichrist
				return;
			}
		}
		// - AntiChrist (5) - ITEMHAND CHECKS END -
		// probably these next checks could be removed with the ITEMHAND thing - AntiChrist
		
		
		if ( (!(Skills->GetSecondHand(currchar[s])==-1)) && (buffer[s][5]==2) ) //Morrolan test
		{
			bounce[1]=5;
			Network->xSend(s, bounce, 2, 0);
			sysmessage(s, "You are already holding something in your left hand.");
			if (items[i].id1>=0x40) senditem(s, i);
			return;
		}
		if( Combat->GetWeapon( currchar[s] ) != -1 && ( (buffer[s][5] == 1) || ( buffer[s][5] == 2 && !Items->isLeftHandType( i ) ) ) )//Morrolan test
		{
			bounce[1]=5;
			Network->xSend(s, bounce, 2, 0);
			sysmessage(s, "You already have a weapon equipped!");
			if (items[i].id1>=0x40) senditem(s, i);
			return;
		}
		if (!(chars[currchar[s]].priv&0x01)) // Ripper.. players cant equip items on other players or npc`s paperdolls. // GM PRIVS
		{
			if((k!=currchar[s])&&(chars[s].npc!=k))
			{
				bounce[1]=5;
				Network->xSend( s, bounce, 2, 0 );
				sysmessage( s, "You can't put items on other players!" );
				if( items[i].id1>=0x40) senditem( s, i );
				return;
			}
		}
		items[i].cont1=buffer[s][6];
		items[i].cont2=buffer[s][7];
		items[i].cont3=buffer[s][8];
		items[i].cont4=buffer[s][9];
		items[i].contserial=calcserial(items[i].cont1,items[i].cont2,items[i].cont3,items[i].cont4);
		if (items[i].contserial!=-1) setptr(&contsp[items[i].contserial%HASHMAX], i);
		items[i].layer=buffer[s][5];
		chars[currchar[s]].st = min(max((chars[currchar[s]].st + items[i].st2),1),65535);
		chars[currchar[s]].dx = min(max((chars[currchar[s]].dx + items[i].dx2),1),65535);
		chars[currchar[s]].in = min(max((chars[currchar[s]].in + items[i].in2),1),65535);
		// AntiChrist -- for poisoned items
		if(items[i].poisoned) {
			if(chars[currchar[s]].poison<items[i].poisoned)
				chars[currchar[s]].poison=items[i].poisoned;
		}
		if (showlayer) printf("Item equipped on layer %i.\n",items[i].layer);
		removeitem[1]=items[i].ser1;
		removeitem[2]=items[i].ser2;
		removeitem[3]=items[i].ser3;
		removeitem[4]=items[i].ser4;
		for (j=0;j<now;j++)
		{
			//if ((abs(chars[currchar[j]].x-items[i].x)<Races->getVisRange( chars[currchar[j]] ) )&&(abs(chars[currchar[j]].y-items[i].y)< Races->getVisRange( chars[currchar[j]] ) ) )
			//{
			if (perm[j]) Network->xSend(j, removeitem, 5, 0);
			//}
		}
		wearitem[1]=items[i].ser1;
		wearitem[2]=items[i].ser2;
		wearitem[3]=items[i].ser3;
		wearitem[4]=items[i].ser4;
		wearitem[5]=items[i].id1;
		wearitem[6]=items[i].id2;
		wearitem[8]=items[i].layer;
		wearitem[9]=items[i].cont1;
		wearitem[10]=items[i].cont2;
		wearitem[11]=items[i].cont3;
		wearitem[12]=items[i].cont4;
		wearitem[13]=items[i].color1;
		wearitem[14]=items[i].color2;
		Network->xSend(s, wearitem, 15, 0);
		wornitems(s, k); // send update to current socket
		for (j=0;j<now;j++)
		{
			if (perm[j] && inrange1p(k, currchar[j]) && ( j != s ) )  // and to all inrange sockets (without re-sending to current socket) //AntiChrist
				wornitems(j, k);
		}
		// Dupois - see itemsfx() for details
		// Added Oct 09, 1998
		itemsfx(s, items[i].id1, items[i].id2);
		//soundeffect(s, 0x00, 0x48);
		// Dupois - end
		Weight->NewCalc(currchar[s]);  // Ison 2-20-99
		statwindow(s,currchar[s]);
		// Lord Binary
		if( items[i].glow > 0 )
		{
			removefromptr( &glowsp[chars[currchar[s]].serial%HASHMAX], i ); // if gm equips a different player it needs to be deleted out of the hashtable
			setptr( &glowsp[chars[k].serial%HASHMAX], i );
			Items->GlowItem( k, i ); // LB's glowing items stuff
		}
	}
}

void dump_item(int s) // Item is dropped on ground
{
	int i, j, k, t,serial,serhash, nChar;
	//int multi ;
	tile_st tile;
	
	nChar=currchar[s];  //chars[] array #
	serial=calcserial(buffer[s][1],buffer[s][2],buffer[s][3],buffer[s][4]);
	serhash=serial%HASHMAX;
	i=findbyserial(&itemsp[serhash], serial, 0);
	
	if (i==-1) 
	{ 
		all_items(s);
		printf("ALERT: all_items() called in dump_item().  This function could cause a lot of lag!" ); // AntiChrist
	}
	else
	{
		//test UOP blocking Tauriel 1-12-99
		if (items[i].contserial!=-1)
		{
			bounce[1]=5;
			Network->xSend(s, bounce, 2, 0);
			if (items[i].id1>=0x40) senditem(s, i);
			return;
		}
		
		Map->SeekTile((items[i].id1<<8)+items[i].id2, &tile);
		
		if ((((items[i].magic==2)||((tile.weight==255)&&(items[i].magic!=1)))&&((chars[currchar[s]].priv2&1)==0)) ||
			(items[i].magic==3 && !(items[i].ownserial==chars[currchar[s]].serial)))
		{
			bounce[1]=5;
			Network->xSend(s, bounce, 2, 0);
			if (items[i].id1>=0x40) senditem(s, i);
			return;
		}
		
		if (buffer[s][5] != 0xFF )
		{
			mapRegions->RemoveItem(i); //lb
			
			items[i].x=(buffer[s][5]<<8)+buffer[s][6];
			items[i].y=(buffer[s][7]<<8)+buffer[s][8];
			// Quippl: check for a wall torch or related items for proper z-placement
			int torchItemCheck = (items[i].id1<<8)+items[i].id2;
			if ( (torchItemCheck >= 2555) && (torchItemCheck <= 2574))
				// it's a wall torch-like item, so place higher
				items[i].z=buffer[s][9]+14;
			else
				items[i].z=buffer[s][9];
			if (items[i].contserial!=-1) removefromptr(&contsp[items[i].serial%HASHMAX], i);
			items[i].cont1=255;
			items[i].cont2=255;
			items[i].cont3=255;
			items[i].cont4=255;
			items[i].contserial=-1;
			// Tauriel add item to world mapcells
			mapRegions->AddItem(i); //add this item to a map cell
			if( items[i].glow )
				removefromptr( &glowsp[chars[currchar[s]].serial%HASHMAX], i );
			Items->GlowItem( currchar[s], i ); // LB's glowing item stuff
			removeitem[1]=items[i].ser1;
			removeitem[2]=items[i].ser2;
			removeitem[3]=items[i].ser3;
			removeitem[4]=items[i].ser4;
			
			for (j=0;j<now;j++)
			{
				if (perm[j])
				{
					Network->xSend(j, removeitem, 5, 0);
				}
			}
			RefreshItem( i );
		}
		else
		{
			t=calcCharFromSer(buffer[s][10], buffer[s][11], buffer[s][12], buffer[s][13]);
			if (t != -1)
				if (t!=currchar[s])
				{
					if (chars[t].npc)
					{
						//This crazy training stuff done by Anthracks (fred1117@tiac.net)
						if(!(chars[t].id1==0x01 && (chars[t].id2==0x90 || chars[t].id2==0x91)))
						{
							bounce[1]=5;
							Network->xSend(s, bounce, 2, 0);
							if(items[i].contserial==-1) //1==255 && items[i].cont2==255 && items[i].cont3==255 && items[i].cont4==255)
							{
								if (items[i].id1>=0x40) senditem(s, i);
							}
							else
								senditem(s, i);
							return;
						}
						if(chars[nChar].trainer!=chars[t].serial)
						{
							npctalk(s, t, "Thank thee kindly, but I have done nothing to warrant a gift.", 0);
							bounce[1]=5;
							Network->xSend(s, bounce, 2, 0);
							if(items[i].contserial==-1) //1==255 && items[i].cont2==255 && items[i].cont3==255 && items[i].cont4==255)
							{
								if (items[i].id1>=0x40) senditem(s, i);
							}
							else
								senditem(s, i);
							return;
						}
						else // The player is training from this NPC
						{ // This NPC is trainging the player
							if(items[i].id1==0x0e && items[i].id2==0xed)
							{ // They gave the NPC gold
								npctalk(s, t, "I thank thee for thy payment. That should give thee a good start on thy way. Farewell!", 0);
								int oldskill=chars[nChar].baseskill[chars[t].trainingplayerin]; 
                                chars[nChar].baseskill[chars[t].trainingplayerin]+=items[i].amount;
								if(chars[nChar].baseskill[chars[t].trainingplayerin]>250) chars[nChar].baseskill[chars[t].trainingplayerin]=250;
								Skills->updateSkillLevel(nChar, chars[t].trainingplayerin);
								updateskill(s,chars[t].trainingplayerin);
								if(items[i].amount>250)
								{ // Paid too much
									items[i].amount-=250-oldskill;
									bounce[1]=5;
									Network->xSend(s, bounce, 2, 0);
									if(items[i].contserial==-1) //1==255 && items[i].cont2==255 && items[i].cont3==255 && items[i].cont4==255)
									{
										if (items[i].id1>=0x40) senditem(s, i);
									}
									else
										senditem(s, i);
								}
								else {items[i].free=1; Items->DeleItem(i);} // Gave exact change
								chars[nChar].trainer=-1;
								chars[t].trainingplayerin=255;
								itemsfx( s, items[i].id1, items[i].id2 ); // AntiChrist - do the gold sound
								return;
							}
							else // Did not give gold
							{
								npctalk(s, t, "I am sorry, but I can only accept gold.", 0);
								bounce[1]=5;
								Network->xSend(s, bounce, 2, 0);
								if(items[i].contserial==-1) //1==255 && items[i].cont2==255 && items[i].cont3==255 && items[i].cont4==255)
								{
									if (items[i].id1>=0x40) senditem(s, i);
								}
								else
									senditem(s, i);
								return;
							}
						}
					}
					else // end of training stuff
					{
						j = tradestart( s, t ); //trade-stuff
						unsetserial( i, 1 );
						setserial(i, j, 1);
						items[i].x=30;
						items[i].y=30;
						items[i].z=9;
						removeitem[1]=items[i].ser1;
						removeitem[2]=items[i].ser2;
						removeitem[3]=items[i].ser3;
						removeitem[4]=items[i].ser4;
						for (k=0;k<now;k++)
						{
							if (perm[k])
							{
								Network->xSend(k, removeitem, 5, 0);
							}
							RefreshItem( i ); // AntiChrist
						}
					}
				}
				else // dumping stuff to bp !
				{
					if( items[i].glow > 0 )
					{
						setptr( &glowsp[chars[currchar[s]].serial%HASHMAX], i );
						Items->GlowItem( currchar[s], i );
					}
					int pack=packitem(currchar[s]); // LB ...
                    if (pack==-1) // if player has no pack, put it at its feet
					{ 
						mapRegions->RemoveItem(i);
						items[i].x=chars[currchar[s]].x;
						items[i].y=chars[currchar[s]].y;
						items[i].z=chars[currchar[s]].z;
						mapRegions->AddItem(i);
						//						senditem(s,i);
						RefreshItem( i ); // AntiChrist
						
					} else
					{
						if (items[i].contserial!=-1) removefromptr(&contsp[items[pack].serial%HASHMAX], i);
						
						removeitem[1] = items[i].ser1;
						removeitem[2] = items[i].ser2;
						removeitem[3] = items[i].ser3;
						removeitem[4] = items[i].ser4;
						
						//AUTOSTACK! Yippee!
						int ptr, stack,hash;
						hash=items[pack].serial%HASHMAX;
						for(ptr=0;ptr<contsp[hash].max;ptr++)
						{
							stack = contsp[hash].pointer[ptr];
							if( stack != -1 )
							{
								if( items[stack].contserial == items[pack].serial )
								{										
									
									if (items[stack].pileable && items[i].pileable &&
										items[stack].serial!=items[i].serial &&
										(items[stack].id1==items[i].id1 &&
										items[stack].id2==items[i].id2)
										&& (items[stack].color1==items[i].color1 &&
										items[stack].color2==items[i].color2)
										)
									{//Time to stack.
										if (items[stack].amount+items[i].amount>65535)
										{
											items[i].x=items[stack].x;
											items[i].y=items[stack].y;
											items[i].z=9;
											items[i].amount=(items[stack].amount+items[i].amount)-65535;
											items[stack].amount=65535;
											unsetserial( i, 1 );
											setserial(i, pack,1); // add to container hash
											Network->xSend(s, removeitem, 5, 0);
											//											  senditem(s, i);
											RefreshItem( i ); // AntiChrst
										} else 
										{
											items[stack].amount=items[stack].amount+items[i].amount;
											Items->DeleItem(i);
										}
										Network->xSend(s, removeitem, 5, 0);//Morrolan fix
										senditem(s, stack);
										Weight->NewCalc(currchar[s]);
										statwindow(s,currchar[s]);
										itemsfx(s, items[stack].id1, items[stack].id2); // AntiChrist
										return;
									} // end if pillable
								}// if stack !=-1
							}
							else // no autostacking
							{ 
								unsetserial( i, 1 );
								setserial(i, pack,1);      // no autostacking -> add it to container hash ! 
								items[i].x=20+(rand()%100);// and set new random pack coords
								items[i].y=40+(rand()%80);
								items[i].z=9;
								
								for (k=0;k<now;k++) 
								{
									if (perm[k])
									{											
										Network->xSend(k, removeitem, 5, 0);
									}
								} // end for k
								RefreshItem( i );
								
								Weight->NewCalc( currchar[s] ); // AntiChrist bugfixes
								statwindow( s, currchar[s] );
								itemsfx( s, items[i].id1, items[i].id2 );
								return;
							} // else
						} // end for
					} // end of if player has no pack
				}
			} 
			
			Weight->NewCalc(currchar[s]);  // Ison 2-20-99
			statwindow(s,currchar[s]);
			itemsfx(s, items[i].id1, items[i].id2, true);
			
			//Boats !
			if( chars[currchar[s]].multis > 0 ) //How can they put an item in a multi if they aren't in one themselves Cut lag by not checking everytime sonething is put down
			{
				int multi=findmulti(items[i].x,items[i].y,items[i].z);
				if( multi != -1 ) 
					setserial( i, multi, 7 );
				else if( items[i].multis != -1 )
					unsetserial( i, 7 );
			}
			//End Boats
	}
  
}


void pack_item(int s) // Item is put into container
{
	int nCont=-1, nItem=-1;
	int j, k, z, serial, serhash;
	tile_st tile;
	char bufftemp[50];
	strncpy( bufftemp, (char *)buffer[s], 49 );	// buffer is MUCH longer than bufftemp.  Be careful
	bufftemp[50] = 0;
	
	serial=calcserial(buffer[s][10],buffer[s][11],buffer[s][12],buffer[s][13]);
	serhash=serial%HASHMAX;
	nCont=findbyserial(&itemsp[serhash], serial, 0);
	
	serial=calcserial(buffer[s][1],buffer[s][2],buffer[s][3],buffer[s][4]);
	serhash=serial%HASHMAX;
	nItem=findbyserial(&itemsp[serhash], serial, 0);
	
	if (nCont==-1)
	{
		RefreshItem( nCont ); // AntiChrist
		return;
	} 
	
	if (nItem==-1) return; //LB
	
	if (items[nCont].layer==0 && items[nCont].id1==0x1E && items[nCont].id2==0x5E &&
		items[nCont].contserial==chars[currchar[s]].serial)
	{
		// Trade window???
		serial=calcserial(items[nCont].moreb1, items[nCont].moreb2, items[nCont].moreb3, items[nCont].moreb4);
		// z = other players trade window container
		z=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
		if (z!=-1)
		{
			if ((items[z].morez || items[nCont].morez))
			{
				items[z].morez=0;
				items[nCont].morez=0;
				sendtradestatus(z, nCont);
			}
			// Send item sfx for dropping into other players trade window
			itemsfx( calcSocketFromChar( calcCharFromSer( items[z].contserial ) ), items[nCont].id1, items[nCont].id2 );
		}
	}
	
	// 
	// AntiChrist - Special Bank Stuff
	//
	//if morey == 123   - gold only bank
	//
	if( server_data.usespecialbank ) // only if special bank is activated
	{
		if( items[nCont].morey == 123 && items[nCont].morex == 1 && items[nCont].type == 1 )
		{
			if(( items[nItem].id1 == 0x0E ) && ( items[nItem].id2 == 0xED ) )
			{ // if they're gold ok
				goldsfx( s, 2 );
			}
			else
			{ // if they're not gold.. bounce on ground
				sysmessage( s, "You can only put gold in this bank!" );
				
				if( items[nItem].contserial != -1 ) 
					removefromptr( &contsp[items[nItem].serial%HASHMAX], nItem );
				items[nItem].cont1 = 255;
				items[nItem].cont2 = 255;
				items[nItem].cont3 = 255;
				items[nItem].cont4 = 255;
				items[nItem].contserial = -1;
				
				mapRegions->RemoveItem( nItem );
				items[nItem].x = chars[currchar[s]].x;
				items[nItem].y = chars[currchar[s]].y;
				items[nItem].z = chars[currchar[s]].z;
				mapRegions->AddItem( nItem );
				RefreshItem( nItem ); // AntiChrist
				itemsfx( s, items[nItem].id1, items[nItem].id2 );
				return;
			}
		}
	}
				
	
	//testing UOP Blocking Tauriel 1-12-99
	if (items[nItem].contserial!=-1)
	{
		bounce[1]=5;
		Network->xSend(s, bounce, 2, 0);
		if (items[nItem].id1>=0x40) senditem(s, nItem);
		return;
	}
	
	Map->SeekTile((items[nItem].id1<<8)+items[nItem].id2, &tile);
	if ((((items[nItem].magic==2)||((tile.weight==255)&&(items[nItem].magic!=1)&&(items[nCont].corpse!=1)))&&((chars[currchar[s]].priv2&1)==0)) ||
		(items[nItem].magic==3 && !(items[nItem].ownserial==chars[currchar[s]].serial)))
	{
		bounce[1]=5;
		Network->xSend(s, bounce, 2, 0);
		if (items[nCont].id1>=0x40) senditem(s, nCont);
		return;
	}
	// - Trash container
	if (items[nCont].type==87)
	{
		Items->DeleItem(nItem);
		sysmessage(s, "As you let go of the item it disappears.");
		return;
	}
	// - Spell Book
	if (items[nCont].type==9)
	{
		if (items[nItem].id1!=0x1F || items[nItem].id2<0x2D || items[nItem].id2>0x72)
		{
			bounce[1]=5;
			Network->xSend(s, bounce, 2, 0);
			sysmessage(s, "You can only place spell scrolls in a spellbook!");
			if (items[nCont].id1>=0x40) 
				senditem(s, nCont);
			return;
		}
		z=packitem(currchar[s]);
		if (z!=-1) // lb
		{
			if ((!(items[nCont].contserial==chars[currchar[s]].serial)) &&
				(!(items[nCont].contserial==items[z].serial)) && (!(chars[currchar[s]].priv&0x40)))
			{
				bounce[1]=5;
				Network->xSend(s, bounce, 2, 0);
				sysmessage(s, "You cannot place spells in other peoples spellbooks.");
				if (items[nCont].id1>=0x40) senditem(s, nCont);
				return;
			}
		}
		
		if( items[nItem].name[0] == '#' )
			getname( nItem, temp2 );
		else
			strcpy( temp2, items[nItem].name );

		if( items[nCont].more1 == 1 )	// Using more1 to "lock" a spellbook for RP purposes. Thunderstorm
		{
			sysmessage( s, "There are no empty pages left in your book" );
			bounce[1] = 5;
			Network->xSend( s, bounce, 2, 0 );
			return;
		}

		if( !strcmp( temp2, "All-Spell Scroll" ) )
		{
			if( items[nCont].morex == 0xFFFFFFFF && items[nCont].morey == 0xFFFFFFFF && items[nCont].morez == 0xFFFFFFFF )
			{
				sysmessage( s, "You already have a full book!" );
				bounce[1] = 5;
				Network->xSend( s, bounce, 2, 0 );
				return;
			}
			items[nCont].morex = 0xFFFFFFFF;
			items[nCont].morey = 0xFFFFFFFF;
			items[nCont].morez = 0xFFFFFFFF;
		}
		else
		{
			int targSpellNum = ((items[nItem].id1<<8) + items[nItem].id2) - 0x1F2D;
			if( Magic->HasSpell( nCont, targSpellNum ) )
			{
				bounce[1] = 5;
				sysmessage( s, "You already have that spell" );
				Network->xSend( s, bounce, 2, 0 );
				return;
			}
			else
			{
				Magic->AddSpell( nCont, targSpellNum );
			}
		}
		soundeffect5( s, 0x00, 0x42 );	// make dropping noise
		if( items[nItem].amount > 1 )
		{
			items[nItem].amount--;
			RefreshItem( nItem );
		}
		else
			Items->DeleItem( nItem );
		return;
	}
	// player run vendors
	if (!(((items[nCont].pileable)&&(items[nItem].pileable)&&
		(items[nCont].id1==items[nItem].id1)&&(items[nCont].id2==items[nItem].id2)) ||
		((items[nCont].type!=1)&&(items[nCont].type!=9))))
	{
		if (items[nItem].contserial!=-1) removefromptr(&contsp[items[nItem].contserial%HASHMAX], nItem);
		setserial(nItem, nCont, 1);
		if( buffer[s][5] != 0xFF )
		{
			items[nItem].x=(buffer[s][5]<<8)+buffer[s][6];
			items[nItem].y=(buffer[s][7]<<8)+buffer[s][8];
			j=GetPackOwner(nCont);
			if( j > -1 )
			{
				if (chars[j].npcaitype==17 && chars[j].npc && chars[j].ownserial==chars[currchar[s]].serial)
				{
					chars[currchar[s]].fx1=nItem;
					chars[currchar[s]].fx2=17;
					sysmessage(s, "Set a price for this item.");
				}
			}
		} 
		else 
		{
			items[nItem].x=95;
			items[nItem].y=80;
			j=GetPackOwner(nCont);
			if( j > -1 )
			{
				if (chars[j].npcaitype==17 && chars[j].npc && chars[j].ownserial==chars[currchar[s]].serial)
				{
					chars[currchar[s]].fx1=nItem;
					chars[currchar[s]].fx2=17;
					sysmessage(s, "Set a price for this item.");
				}
			}
			//New Autostack for player run vendors --> zippy
			int ptr, stack;
			for(ptr=0;ptr<contsp[items[nCont].serial%HASHMAX].max;ptr++)
			{
				stack=contsp[items[nCont].serial%HASHMAX].pointer[ptr];
				if (stack!=-1 && items[stack].contserial==items[nCont].serial) //LB
				{
					if (items[stack].pileable && items[nItem].pileable && items[stack].serial!=items[nItem].serial &&
						(items[stack].id1==items[nItem].id1 && items[stack].id2==items[nItem].id2) && 
						( items[stack].color1 == items[nItem].color1 && items[stack].color2 == items[nItem].color2 ) )
					{//Time to stack.
						if (items[stack].amount+items[nItem].amount>65535)
						{
							items[nItem].x=items[stack].x;
							items[nItem].y=items[stack].y;
							items[nItem].z=9;
							items[nItem].amount=(items[stack].amount+items[nItem].amount)-65535;
							items[stack].amount=65535;
							//							senditem(s, nItem);
							RefreshItem( nItem ); // AntiChrist
						} else {
							items[stack].amount=items[stack].amount+items[nItem].amount;
							Items->DeleItem(nItem);
						}
						RefreshItem( stack ); // AntiChrist
						itemsfx(s, items[nItem].id1, items[nItem].id2);
						statwindow(s,currchar[s]);
						break;
					}
				}// if pileable
			}// for
		}
		items[nItem].z=9;
		removeitem[1]=items[nItem].ser1;
		removeitem[2]=items[nItem].ser2;
		removeitem[3]=items[nItem].ser3;
		removeitem[4]=items[nItem].ser4;
		for (j=0;j<now;j++)
		{
			if (perm[j]) Network->xSend(j, removeitem, 5, 0);
		}
		RefreshItem( nItem ); // AntiChrist
		// Dupois - see itemsfx() for details
		// Added Oct 09, 1998
		itemsfx(s, items[nItem].id1, items[nItem].id2);
		// Dupois - end
		statwindow(s,currchar[s]);
	}
	// end of player run vendors
	
	else
		// - Unlocked item spawner or unlockable item spawner
		if (items[nCont].type==63 || items[nCont].type==65)
		{
			items[nItem].cont1=buffer[s][10];
			items[nItem].cont2=buffer[s][11];
			items[nItem].cont3=buffer[s][12];
			items[nItem].cont4=buffer[s][13];
			items[nItem].contserial=calcserial(buffer[s][10],buffer[s][11],buffer[s][12],buffer[s][13]);
			
			// lb bugfix
			items[nItem].x = ( buffer[s][5]<<8 ) + buffer[s][6];
			items[nItem].y = ( buffer[s][7]<<8 ) + buffer[s][8];
			items[nItem].z = buffer[s][9];
			
			removeitem[1]=items[nItem].ser1;
			removeitem[2]=items[nItem].ser2;
			removeitem[3]=items[nItem].ser3;
			removeitem[4]=items[nItem].ser4;
			for (j=0;j<now;j++)
			{
				if (perm[j]) 
				{ 
					Network->xSend(j, removeitem, 5, 0);
				}
			}			
			RefreshItem( nItem ); // AntiChrist
			itemsfx(s, items[nItem].id1, items[nItem].id2);
			
		}
		else  // - Pileable
			if ((items[nCont].pileable) && (items[nItem].pileable) &&
				(items[nCont].id1==items[nItem].id1)&&(items[nCont].id2==items[nItem].id2))
			{
				removeitem[1]=items[nItem].ser1;
				removeitem[2]=items[nItem].ser2;
				removeitem[3]=items[nItem].ser3;
				removeitem[4]=items[nItem].ser4;
				
				if ((items[nCont].amount+items[nItem].amount) > 65535)
				{
					items[nItem].amount -= (65535-items[nCont].amount);
					Commands->DupeItem(s, nCont, items[nItem].amount);
					items[nCont].amount = 65535;
					Items->DeleItem(nItem);
				}
				else
				{
					items[nCont].amount=items[nCont].amount+items[nItem].amount;
					itemsfx(s, items[nItem].id1, items[nItem].id2);
					Items->DeleItem(nItem);
					
				}
				
				for (j=0;j<now;j++) if (perm[j]) 
				{
					Network->xSend(j, removeitem, 5, 0);
				}
				RefreshItem( nCont ); // AntiChrist
			}
			else
			{
				items[nItem].x=(buffer[s][5]<<8)+buffer[s][6];
				items[nItem].y=(buffer[s][7]<<8)+buffer[s][8];
				items[nItem].z=buffer[s][9];
				if (items[nItem].contserial!=-1) removefromptr(&contsp[items[nItem].contserial%HASHMAX], nItem);
				items[nItem].cont1=255;
				items[nItem].cont2=255;
				items[nItem].cont3=255;
				items[nItem].cont4=255;
				items[nItem].contserial=-1;
				// Tauriel add item to world mapcells
				mapRegions->AddItem(nItem); //add this item to a map cell
				
				removeitem[1]=items[nItem].ser1;
				removeitem[2]=items[nItem].ser2;
				removeitem[3]=items[nItem].ser3;
				removeitem[4]=items[nItem].ser4;
				for (k=0;k<now;k++)
				{
					if (perm[k]) Network->xSend(k, removeitem, 5, 0);
				}
				RefreshItem( nCont ); // AntiChrist
			}
			
			// - Spell Book
			if (items[nCont].type==9) Magic->SpellBook(s); 
			
			if( items[nItem].glow > 0 ) // LB's glowing items stuff
			{
				int p = GetPackOwner( nCont );
				removefromptr( &glowsp[chars[currchar[s]].serial%HASHMAX], nItem ); // if gm put glowing object in another pack, handle glowsp correctly!
				if( p != -1 )
				{
					setptr( &glowsp[chars[p].serial%HASHMAX], nItem );
					Items->GlowItem( p, nItem );
				}
			}
}



void startchar(int s) // Send character startup stuff to player
{
	char zbuf[255];
	char modeset[6]="\x72\x00\x00\x32\x00";
	char techstuff[21]="\x69\x00\x05\x01\x00\x69\x00\x05\x02\x00\x69\x00\x05\x03\x00\x55\x5B\x0C\x13\x03";
	char startup[38]="\x1B\x00\x05\xA8\x90\x00\x00\x00\x00\x01\x90\x06\x08\x06\x49\x00\x0A\x04\x00\x00\x00\x7F\x00\x00\x00\x00\x00\x07\x80\x09\x60\x00\x00\x00\x00\x00\x00";
	int i,serial,serhash,ci;
	
	for( i = 0; i < MAX_ACCT_LOCK; i++ )
	{
		if( acctx[chars[currchar[s]].account].lock[i] == chars[currchar[s]].serial ) Network->Disconnect( currchar[s] );
	}
	
	perm[s]=1;
	targetok[s]=0;
	startup[1]=chars[currchar[s]].ser1;
	startup[2]=chars[currchar[s]].ser2;
	startup[3]=chars[currchar[s]].ser3;
	startup[4]=chars[currchar[s]].ser4;
	startup[9]=chars[currchar[s]].id1;
	startup[10]=chars[currchar[s]].id2;
	startup[11]=chars[currchar[s]].x>>8;
	startup[12]=chars[currchar[s]].x%256;
	startup[13]=chars[currchar[s]].y>>8;
	startup[14]=chars[currchar[s]].y%256;
	startup[16]=chars[currchar[s]].z;
	startup[17]=chars[currchar[s]].dir;
	startup[28]=0;
	if(chars[currchar[s]].poisoned) startup[28]=0x04; else startup[28]=0x00; //AntiChrist -- thnx to SpaceDog
	chars[currchar[s]].spiritspeaktimer=0;  // initially set spiritspeak timer to 0
	
	// Retain previous hidden status
	if( !( chars[currchar[s]].priv&0x01 ) )
	{
		chars[currchar[s]].stealth = -1;
		chars[currchar[s]].hidden = 0;
	}
	
	Network->xSend(s, startup, 37, 0);
	chars[currchar[s]].war=0;
	wornitems(s, currchar[s]);
	teleport(currchar[s]);
	Network->xSend(s, modeset, 5, 0);
	impowncreate(s, currchar[s], 0);
	Network->xSend(s, techstuff, 20, 0);
	Weight->NewCalc(currchar[s]);  // Ison 2-20-99
#ifdef __NT__
	sprintf(idname, "%s %s(Build:%s) [WIN32] Compiled by %s ", PRODUCT, VER, BUILD, NAME);
#else
	sprintf(idname, "%s %s(Build:%s) [LINUX] Compiled by %s", PRODUCT, VER, BUILD, NAME);
#endif
	sysmessage(s, idname );
	sprintf( idname, "Programmed by: %s", PROGRAMMERS );
	sysmessage(s, idname);
	//weather(s);
	// Network->xSend(s, restart, 2, 0);
	chars[currchar[s]].region=255;
	checkregion(currchar[s]);
	//Tauriel set packitem at login
	serial=chars[currchar[s]].serial;
	serhash=serial%HASHMAX;
	for (i=0;i<contsp[serhash].max;i++)
	{
		ci=contsp[serhash].pointer[i];
		if (ci >-1)
			if (items[ci].contserial==serial && (items[i].layer==0x15))
			{
				chars[currchar[s]].packitem=ci;
			}
	}
	// log last time signed on
	setLastOn(s);
	
	if (server_data.joinmsg)
	{
		if(!(strcmp( chars[currchar[s]].name, "ptr Slot --" ) ) ) 
			strcpy( chars[currchar[s]].name, "A new Character" ); // AntiChrist
		sprintf(temp,"%s entered the realm",chars[currchar[s]].name);//message upon entering a server
		sysbroadcast(temp);//message upon entering a server
	}
	sprintf(zbuf,"%s Logged in the game",chars[currchar[s]].name); //for logging to UOXmon
	// if(heartbeat) Writeslot(zbuf);
	acctinuse[acctno[s]]=1;
	teleport(currchar[s]);
	chars[currchar[s]].step = 1;
	updates( s ); 
}

int validhair(int a, int b) // Is selected hair type valid
{
	int x = 1;
	
	if( a != 0x20 ) 
		return 0;
	switch( b )
	{
	case 0x3B:
	case 0x3C:
	case 0x3D:
	case 0x44:
	case 0x45:
	case 0x46:
	case 0x47:
	case 0x48:
	case 0x49:
	case 0x4A:
		return 1;
	default:
		return 0;
	}
	return 1;
}

int validbeard(int a, int b) // Is selected beard type valid
{
	if( a != 0x20 ) 
		return 0;
	switch( b )
	{
	case 0x3E:
	case 0x3F:
	case 0x40:
	case 0x41:
	case 0x4B:
	case 0x4C:
	case 0x4D:
		return 1;
	default:
		return 0;
	}
	return 1;
}

void charcreate(int s) // All the character creation stuff
{
	int i,n;

	int totalstats,totalskills;
	unsigned int c;
	c = Npcs->MemCharFree ();
	
	Npcs->InitChar( c );
	currchar[s] = c;
	
	for (i=0;i<=strlen((char *)&buffer[s][10]);i++) 
		chars[c].name[i]=buffer[s][10+i];
	chars[c].account = acctno[s];
	if( buffer[s][0x46] != 0x00 )
	{
		chars[c].id2 = 0x91;
		chars[c].xid2 = 0x91;
		chars[c].orgid2 = 0x91;
	}
	chars[c].skin1=buffer[s][0x50]|0x80;
	chars[c].skin2=buffer[s][0x51];
	if ((((chars[c].skin1<<8)+chars[c].skin2)<0x83EA) ||
		(((chars[c].skin1<<8)+chars[c].skin2)>0x8422) )
	{
		chars[c].skin1=0x83;
		chars[c].skin2=0xEA;
	}
	chars[c].xskin1=chars[c].skin1;
	chars[c].xskin2=chars[c].skin2;
	chars[c].priv=defaultpriv1;
	chars[c].priv2=defaultpriv2;
	
	
    if (acctno[s]==0) 
	{ 
		chars[c].priv=0xE7;
		chars[c].priv = 0xD9;	// default initialization like in makeGM
		chars[c].commandLevel = 2;
	}
	
	mapRegions->RemoveItem(c+1000000); // should not be in da regions at this point, but who knows ...
	chars[c].x=makenum2(start[buffer[s][0x5B]][2]);
	chars[c].y=makenum2(start[buffer[s][0x5B]][3]);
	chars[c].dispz=chars[c].z=makenum2(start[buffer[s][0x5B]][4]);
	mapRegions->AddItem(c+1000000);
	
	chars[c].dir=4;
	// EviLDeD - January 29, 2000
  // Fix for Eagles NameDeed Stuff. Kinda Q & D but here ya go Eagle.
  chars[c].namedeed=5;
  // EviLDeD - End
	chars[c].hp=chars[c].st=buffer[s][0x47];
	if (chars[c].st>45) chars[c].st=45;          // fix for hack exploit
	if (chars[c].st<10) chars[c].st=10;
	totalstats=chars[c].st;
	chars[c].stm=chars[c].dx=buffer[s][0x48];
	if (chars[c].dx>45) chars[c].dx=45;          // fix for hack exploit
	if (chars[c].dx<10) chars[c].dx=10;
	if (chars[c].dx+totalstats>65) chars[c].dx=65-totalstats;
	totalstats+=chars[c].dx;
	chars[c].mn=chars[c].in=buffer[s][0x49];
	if (chars[c].in>45) chars[c].in=45;          // fix for hack exploit
	if (chars[c].in<10) chars[c].in=10;
	if (chars[c].in+totalstats>65) chars[c].in=65-totalstats;
	
	if (buffer[s][0x4b]>50) buffer[s][0x4b]=50; // fixes for hack exploit
	totalskills=buffer[s][0x4b];
	if (buffer[s][0x4d]>50) buffer[s][0x4d]=50;
	if (buffer[s][0x4d]+totalskills>100) buffer[s][0x4d]=100-totalskills;
	totalskills+=buffer[s][0x4d];
	if (buffer[s][0x4f]>50) buffer[s][0x4f]=50;
	if (buffer[s][0x4f]+totalskills>100) buffer[s][0x4f]=100-totalskills;
	
	chars[c].race=0;
	chars[c].raceGate=65535;
	for (i=0;i<TRUESKILLS;i++)
	{
		chars[c].baseskill[i]=0;
		if (i==buffer[s][0x4a]) chars[c].baseskill[buffer[s][0x4a]]=buffer[s][0x4b]*10;
		else if (i==buffer[s][0x4c]) chars[c].baseskill[buffer[s][0x4c]]=buffer[s][0x4d]*10;
		else if (i==buffer[s][0x4e]) chars[c].baseskill[buffer[s][0x4e]]=buffer[s][0x4f]*10;
		Skills->updateSkillLevel(c, i);
	}
	
	if (validhair(buffer[s][0x52],buffer[s][0x53]))
	{
		
		n = Items->SpawnItem(s, c, 1, "#", 0, buffer[s][0x52], buffer[s][0x53], buffer[s][0x54], buffer[s][0x55],0,0);
		if( n != -1 )
		{
			if ((((items[n].color1<<8)+items[n].color2)<0x044E) ||
				(((items[n].color1<<8)+items[n].color2)>0x04AD) )
			{
				items[n].color1=0x04;
				items[n].color2=0x4E;
			}
			setserial(n, c, 4);
			items[n].layer=0x0B;
		}
	}
	
	if ( (validbeard(buffer[s][0x56],buffer[s][0x57])) && (chars[c].id2==0x90) )
	{
		n = Items->SpawnItem(s, c, 1, "#", 0, buffer[s][0x56], buffer[s][0x57], buffer[s][0x58], buffer[s][0x59],0,0);
		if( n != -1 )
		{
			if ((((items[n].color1<<8)+items[n].color2)<0x044E) ||
				(((items[n].color1<<8)+items[n].color2)>0x04AD) )
			{
				items[n].color1=0x04;
				items[n].color2=0x4E;
			}
			setserial(n, c, 4);
			items[n].layer=0x10;
		}
	}
	// - create the backpack
	chars[c].packitem = n = Items->SpawnItem(s, c, 1, "#", 0, 0x0E, 0x75, 0, 0,0,0);
	if( n != -1 )
	{
		setserial(n, c, 4);
		items[n].layer=0x15;
		items[n].type=1;
		items[n].dye=1;
	}
	n=Items->SpawnItem(s, c, 1,"#",0,0x09,0x15,0,0,0,0);
	if( n == -1 ) return;

	switch( rand()%2 )
	{
	case 0:
		if( ( chars[c].id2== 0x90 ) && ( chars[c].xid2 == 0x90 ) )
		{
			items[n].id1=0x15;
			items[n].id2=0x39;
			items[n].layer=0x04; // pant
		} 
		else
		{
			items[n].id1=0x15;
			items[n].id2=0x16;
			items[n].layer=23; // skirt
		}
		break;
	case 1:
		if( ( chars[c].id2 == 0x90 ) && ( chars[c].xid2 == 0x90 ) )
		{
			items[n].id1=0x15;
			items[n].id2=0x2E;
			items[n].layer=0x04;
		} 
		else
		{
			items[n].id1=0x15;
			items[n].id2=0x37;
			items[n].layer=23;
		}
		break;
	}
	// pant/skirt color -> old client code, random colour
	items[n].color1 = buffer[s][102];
	items[n].color2 = buffer[s][103];
	setserial(n, c, 4);
	items[n].type=0;
	items[n].dye=1;
	
	n=Items->SpawnItem(s, c, 1,"#",0,0x09,0x15,0,0,0,0); // spawn pants
	if( n == -1 ) return;
	
	switch( rand()%2 )
	{
	case 0:		items[n].id1 = 0x1E;		items[n].id2 = 0xFD;		break;
	case 1:		items[n].id1 = 0x15;		items[n].id2 = 0x17;		break;
	}
	items[n].color1 = buffer[s][100];
	items[n].color2 = buffer[s][101];
	setserial(n, c, 4);
	items[n].layer=0x05;
	items[n].dye=1;
	items[n].def=1;
	
	n=Items->SpawnItem(s, c, 1,"#",0,0x17,0x0F,0x02,0x87,0,0);
	if( n == -1 ) return;
	setserial(n, c, 4);
	items[n].layer=0x03;
	items[n].dye=1;
	items[n].def=1;
	
	n=Items->SpawnItem(s, c, 1,"#",0,0x0F,0x51,0,0,0,0);
	if( n == -1 ) return;
	setserial(n, c, 4);
	items[n].layer=0x01;
	items[n].att=5;
	
#ifdef SPECIAL
	n=Items->SpawnItem(s, c, 1,"#",0,0x09,0x15,0,0,0,0);
	if( n == -1 ) return;
	switch( rand()%7 )
	{
	case 0:		items[n].id1 = 0x15;		items[n].id2 = 0x4b;		break;
	case 1:		items[n].id1 = 0x15;		items[n].id2 = 0x45;		break;
	case 2:		items[n].id1 = 0x15;		items[n].id2 = 0x47;		break;
	case 3:		items[n].id1 = 0x15;		items[n].id2 = 0x49;		break;
	case 4:		items[n].id1 = 0x17;		items[n].id2 = 0x1C;		break;
	case 5:		items[n].id1 = 0x1f;		items[n].id2 = 0x0b;		break;
	case 6:		items[n].id1 = 0x14;		items[n].id2 = 0x51;		break;
	}
	setserial(n, c, 4);
	items[n].layer=0x06;
#endif
	
	// Give the character some gold
	n = Items->SpawnItem(s, c, goldamount,"#",1,0x0E,0xED,0,0,1,0);
	if( n == -1 ) return;
	items[n].layer = 0x01;
	items[n].att = 5;
	newbieitems( c );
	perm[s] = 1;
	startchar( s );
}

void charplay (int s) // After hitting "Play Character" button //Instalog
{
	int i, j, k;
	
	if (acctno[s]>-1)
	{
		j=0;
		k=-1;
		for (i=0;i<charcount;i++)
		{
			if ((chars[i].account==acctno[s])&&(chars[i].npc==0)&&(chars[i].free==0))
			{
				if (j==buffer[s][0x44]) {
					k=i;
					break;
				}
				j++;
			}
		}
		if (k!=-1)
		{
			WhoList->FlagUpdate();
			OffList->FlagUpdate();	// offline list changes too
			if( inworld[acctno[s]] >= 0 )
			{
				if ( inworld[acctno[s]] != -1 && (chars[inworld[acctno[s]]].logout<=uiCurrentTime || overflow))
				{
					inworld[acctno[s]]=-1;
				}
			}
			else
			{
				inworld[acctno[s]] = -1;
			}
			if (inworld[acctno[s]]==-1 || inworld[acctno[s]]==k || chars[k].priv&0x20)//AntiChrist
			{
				inworld[acctno[s]]=k;
				chars[k].logout=-1;
				currchar[s]=k;
				startchar(s);
			} else {
#ifdef DEBUG
				printf("DEBUG: Attempted %i, %i from this account (%i) is in world.\n",k,inworld[acctno[s]],acctno[s]);
#endif
				char msg[3];
				msg[0]=0x53;
				msg[1]=0x05;
				/*
				0x53 Client Message:
				
				  0x01=Char doesn't exist
				  0x02=Char Already Exists 
				  0x03,0x04=Cannot Attach to server 
				  0x05=Char in world Msg
				  0x06=Login server Syc Error
				  0x07=Idle msg
				  0x08 (and up?)=Can't Attach
				*/
				Network->xSend(s, msg, 2, 0);
			}
		}
	}
	else
	{
		Network->Disconnect(s);
	}
}

int validtelepos(int s)
{
	int z;
	
	z=-1;
	if ((chars[currchar[s]].x>=1397)&&(chars[currchar[s]].x<=1400)&&
		(chars[currchar[s]].y>=1622)&&(chars[currchar[s]].y<=1630))
		z=28;
	if ((chars[currchar[s]].x>=1510)&&(chars[currchar[s]].x<=1537)&&
		(chars[currchar[s]].y>=1455)&&(chars[currchar[s]].y<=1456))
		z=15;
	return z;
}

int unmounthorse( UOXSOCKET s ) // Get off a horse (Remove horse item and spawn new horse)
{
	int k,c,ci,serial,serhash;
	
	serial = chars[currchar[s]].serial;
	serhash = serial%HASHMAX;
	for (k=0;k<contsp[serhash].max;k++)
	{
		ci=contsp[serhash].pointer[k];
		if ((ci > -1) &&	//HoneyJar
			(items[ci].contserial==serial) && (items[ci].layer==0x19)&&(items[ci].free==0))
		{
			c=Npcs->MemCharFree ();
			
			Npcs->InitChar(c);
			chars[currchar[s]].onhorse=0;
			strcpy(chars[c].name, items[ci].name);
			chars[c].id1=0x00;
			
			// krazyglue 12 October, 1999 - if, if, if... is now if, else if, else if...
			switch( items[ci].id2 )
			{
			case 0x9F:	chars[c].id2 = 0xC8;	break;
			case 0xA0:	chars[c].id2 = 0xE2;	break;
			case 0xA1:	chars[c].id2 = 0xE4;	break;
			case 0xA2:	chars[c].id2 = 0xCC;	break;
			case 0xA3:	chars[c].id2 = 0xD2;	break;
			case 0xA4:	chars[c].id2 = 0xDA;	break;
			case 0xA5:	chars[c].id2 = 0xDB;	break;
			case 0xA6:	chars[c].id2 = 0xDC;	break;
			}
			
			chars[c].orgid1=chars[c].xid1=chars[c].id1;
			chars[c].orgid2=chars[c].xid2=chars[c].id2;
			chars[c].skin1=items[ci].color1;
			chars[c].skin2=items[ci].color2;
			chars[c].xskin1=items[ci].color1;
			chars[c].xskin2=items[ci].color2;
			chars[c].priv=0x10;
			
			mapRegions->RemoveItem(c+1000000);
			
			chars[c].x=chars[currchar[s]].x;
			chars[c].y=chars[currchar[s]].y;
			
			mapRegions->AddItem(c+1000000);
			
			
			chars[c].dispz=chars[c].z=chars[currchar[s]].z;
			chars[c].dir=chars[currchar[s]].dir;
			chars[c].npc=1;
			chars[c].own1=chars[currchar[s]].ser1;
			chars[c].own2=chars[currchar[s]].ser2;
			chars[c].own3=chars[currchar[s]].ser3;
			chars[c].own4=chars[currchar[s]].ser4;
			chars[c].ownserial=chars[currchar[s]].serial;
			chars[c].tamed = true; // JM bugfix
			setptr(&cownsp[chars[c].ownserial%HASHMAX], c);
			chars[c].npcWander=items[ci].moreb1;
			chars[c].ftarg=-1;
			chars[c].fx1=items[ci].x;
			chars[c].fx2=items[ci].att;
			chars[c].fy1=items[ci].y;
			chars[c].fy2=items[ci].def;
			chars[c].fz1=items[ci].z;
			
			// AntiChrist bugfixes - 11/10/99
			if( items[ci].moreb2 )
				chars[c].st = items[ci].moreb2;
			else
				chars[c].st = 1;	// old horses will live
			chars[c].dx = items[ci].moreb3;
			chars[c].in = items[ci].moreb4;
			chars[c].hp = items[ci].hp;
			chars[c].fame = items[ci].lodamage;
			chars[c].karma = items[ci].hidamage;
			chars[c].poisoned = items[ci].poisoned;
			if( items[ci].decaytime != 0 )
				chars[c].summontimer = items[ci].decaytime;
			updatechar(c);
			Items->DeleItem(ci);
			return 0;
		}
	}
	return -1;
}

void endmessage(int x) // If shutdown is initialized
{
	x=0;
	int igetclock = uiCurrentTime;
	if (endtime<igetclock) endtime=igetclock;
	sprintf(temp, "Server going down in %i minutes.",
		((endtime-igetclock)/CLOCKS_PER_SEC)/60);
	sysbroadcast(temp);
}

void illinst(int x=0) //Thunderstorm linux fix
{
	sysbroadcast("Fatal Server Error! Bailing out - Have a nice day!");
	printf("Illegal Instruction Signal caught - attempting shutdown");
	endmessage(x);
}

void weblaunch(int s, char *txt) // Direct client to a web page
{
	int l;
	char launchstr[4]="\xA5\x00\x00";
	
	sysmessage(s, "Launching your web browser. Please wait...");
	l=strlen(txt)+4;
	launchstr[1]=l>>8;
	launchstr[2]=l%256;
	Network->xSend(s, launchstr, 3, 0);
	Network->xSend(s, txt, strlen(txt)+1, 0);
}



void scriptcommand (int s, char *script1, char *script2) // Execute command from script
{
	char tstring[1024];
	int total, ho, mi, se;
	int tmp, i;
	int c;
	
	strupr( script1 );
	strupr( script2 );
	
	if (!(strcmp("GMMENU", script1)))
	{
		gmmenu(s, str2num(script2));
		return;
	}
	if (!(strcmp("ITEMMENU", script1)))
	{
		i = str2num(script2);
		
		//if (i > 0 || script2[1] == 0)
		itemmenu(s, i);
		//else 
		//	itemmenu(s, script2);
		return;
	}
	if (!(strcmp("WEBLINK", script1)))
	{
		weblaunch(s, script2);
		return;
	}
	if (!(strcmp("SYSMESSAGE", script1)))
	{
		sysmessage(s, script2);
		return;
	}
	if (!(strcmp("GMPAGE", script1)))
	{
		Commands->GMPage(s, script2);
		return;
	}
	if (!(strcmp("CPAGE", script1)))
	{
		Commands->CPage(s, script2);
		return;
	}
	if (!(strcmp("VERSION", script1)))
	{
		sysmessage(s, idname);
		return;
	}
	
	// AntiChrist - no need of skill type check
	if( !( strcmp( "ADDITEM", script1 ) ) ) 
	{
		addmitem[s] = str2num( script2 );
		Skills->MakeMenuTarget( s, addmitem[s], chars[currchar[s]].making );
		chars[currchar[s]].making = 0;
		return;
	}
	if (!(strcmp("BATCH", script1)))
	{
		executebatch=str2num(script2);
		return;
	}
	if (!(strcmp("INFORMATION", script1)))
	{
		total=(uiCurrentTime-starttime)/CLOCKS_PER_SEC;
		ho=total/3600;
		total-=ho*3600;
		mi=total/60;
		total-=mi*60;
		se=total;
		total=0;
		c=0;
		sysmessage(s, "Time up [%i:%i:%i] Connected players [%i out of %i accounts] Items [%i] Characters [%i]",
			ho,mi,se,now,acctcount,items.Count(),chars.Count());
		
		return;
	}
	if (!(strcmp("NPC", script1)))
	{
		addmitem[s]=str2num(script2);
		sprintf(tstring, "Select location for NPC. [Number: %i]", addmitem[s]);
		target(s, 0, 1, 0, 27, tstring);
		return;
	}
	if (!(strcmp("POLY", script1)))
	{
		tmp=hstr2num(script2);
		chars[currchar[s]].id1=tmp>>8;
		chars[currchar[s]].xid1=tmp>>8;
		chars[currchar[s]].orgid1=tmp>>8;
		chars[currchar[s]].id2=tmp%256;
		chars[currchar[s]].xid2=tmp%256;
		chars[currchar[s]].orgid2=tmp%256;
		teleport(currchar[s]);
		return;
	}
	if (!(strcmp("SKIN", script1)))
	{
		tmp=hstr2num(script2);
		chars[currchar[s]].skin1=tmp>>8;
		chars[currchar[s]].xskin1=tmp>>8;
		chars[currchar[s]].skin2=tmp%256;
		chars[currchar[s]].xskin2=tmp%256;
		teleport(currchar[s]);
		return;
	}
	if (!(strcmp("LIGHT", script1)))
	{
		worldfixedlevel=hstr2num(script2);
		if (worldfixedlevel!=255) setabovelight(worldfixedlevel);
		else setabovelight(worldcurlevel);
		return;
	}
	if (!(strcmp("DRY", script1)))
	{
		wtype=0;
		for (i=0;i<now;i++) if (perm[i]) weather(i,0); 
		return;
	}
	
	
	
	
	if (!(strcmp("RAIN", script1)))
	{
		if (wtype==2)
		{
			wtype=0;
			for (i=0;i<now;i++) if (perm[i]) weather(i,0); //lb
		}
		wtype=1;
		for (i=0;i<now;i++) if (perm[i]) weather(i,0); //lb
		return;
	}
	if (!(strcmp("SNOW", script1)))
	{
		if (wtype==1)
		{
			wtype=0;
			for (i=0;i<now;i++) if (perm[i]) weather(i,0); // lb
		}
		wtype=2;
		for (i=0;i<now;i++) if (perm[i]) weather(i,0);
		return;
	}
	if (!(strcmp("GCOLLECT", script1)))
	{
		gcollect();
		return;
	}
	if (!(strcmp("GOPLACE", script1)))
	{
		tmp=str2num(script2);
		Commands->MakePlace (s, tmp);
		if (addx[s]!=0)
		{
			mapRegions->RemoveItem(currchar[s]+1000000);
			chars[currchar[s]].x=addx[s];
			chars[currchar[s]].y=addy[s];
			chars[currchar[s]].dispz=chars[currchar[s]].z=addz[s];
			mapRegions->AddItem(currchar[s]+1000000);
			teleport(currchar[s]);
		}
		return;
	}
	if (!(strcmp("MAKEMENU", script1)))
	{
		Skills->MakeMenu(s, str2num(script2), chars[currchar[s]].making);
		return;
	}
	if (!(strcmp("CREATETRACKINGMENU", script1)))
	{
		Skills->CreateTrackingMenu(s, str2num(script2));
		return;
	}
	if(!(strcmp("TRACKINGMENU", script1)))
	{
		Skills->TrackingMenu(s, str2num(script2));
		return;
	}
}


void batchcheck(int s) // Do we have to run a batch file
{
	sprintf(temp, "BATCH %i", executebatch);
	openscript("menus.scp");
	if (!i_scripts[menus_script]->find(temp))
	{
		closescript();
		return;
	}
	do
	{
		read2();
		if (script1[0]!='}') scriptcommand(s, script1, script2);
	}
	while (script1[0]!='}');
	closescript();
	executebatch=0;
}


void broadcast( UOXSOCKET s ) // GM Broadcast (Done if a GM yells something)
{
	int i,tl;
	char nonuni[512]; 
	
	if( chars[currchar[s]].unicode )
		for (i=13;i<(buffer[s][1]<<8)+buffer[s][2];i=i+2)
		{
			nonuni[(i-13)/2]=buffer[s][i];
		} 
		
	if(!(chars[currchar[s]].unicode))
	{
		tl=44+strlen((char *)&buffer[s][8])+1;
		talk[1]=tl>>8;
		talk[2]=tl%256;
		talk[3]=chars[currchar[s]].ser1;
		talk[4]=chars[currchar[s]].ser2;
		talk[5]=chars[currchar[s]].ser3;
		talk[6]=chars[currchar[s]].ser4;
		talk[7]=chars[currchar[s]].id1;
		talk[8]=chars[currchar[s]].id2;
		talk[9]=1;
		talk[10]=buffer[s][4];
		talk[11]=buffer[s][5];
		talk[12]=buffer[s][6];
		talk[13]=chars[currchar[s]].fonttype;
		for (i=0;i<now;i++)
		{
			if (perm[i])
			{
				Network->xSend(i, talk, 14, 0);
				Network->xSend(i, chars[currchar[s]].name, 30, 0);   
				Network->xSend(i, &buffer[s][8], strlen((char *)&buffer[s][8])+1, 0);   
			}
		}
	} // end unicode IF
	else
	{
		tl=44+strlen(&nonuni[0])+1;
		
		talk[1]=tl>>8;
		talk[2]=tl%256;
		talk[3]=chars[currchar[s]].ser1;
		talk[4]=chars[currchar[s]].ser2;
		talk[5]=chars[currchar[s]].ser3;
		talk[6]=chars[currchar[s]].ser4;
		talk[7]=chars[currchar[s]].id1;
		talk[8]=chars[currchar[s]].id2;
		talk[9]=1;
		talk[10]=buffer[s][4];
		talk[11]=buffer[s][5];
		talk[12]=buffer[s][6];
		talk[13]=chars[currchar[s]].fonttype;
		
		for (i=0;i<now;i++)
		{
			if (perm[i])
			{
				Network->xSend(i, talk, 14, 0);
				Network->xSend(i, chars[currchar[s]].name, 30, 0);     
				Network->xSend(i, &nonuni[0], strlen(&nonuni[0])+1, 0);   
			}
		}
	}
}

void itemtalk(int s, int item, char *txt) // Item "speech"
{
	int tl;
	
	if (item==-1 || s==-1) return;
	
	tl=44+strlen(txt)+1;
	talk[1]=tl>>8;
	talk[2]=tl%256;
	talk[3]=items[item].ser1;
	talk[4]=items[item].ser2;
	talk[5]=items[item].ser3;
	talk[6]=items[item].ser4;
	talk[7]=items[item].id1;
	talk[8]=items[item].id2;
	talk[9]=0; // Type
	talk[10]=0;
	talk[11]=0;
	talk[12]=0;
	talk[13]=3;
	Network->xSend(s, talk, 14, 0);
	Network->xSend(s, items[item].name, 30, 0);
	Network->xSend(s, txt, strlen(txt)+1, 0);
}

void callguards( int p )
{
	if( p == -1 ) return;
	if (!(region[chars[p].region].priv&1 ) || !server_data.guardsactive )
		return;
	unsigned short x = chars[p].x;
	unsigned short y = chars[p].y;
	int StartGrid = mapRegions->StartGrid( x, y );
	
	unsigned int increment = 0;
	for( unsigned int checkgrid = StartGrid + ( increment * mapRegions->GetColSize() ); increment < 3; increment++, checkgrid = StartGrid + ( increment * mapRegions->GetColSize() ) )
	{
		for( int i = 0; i < 3; i++ )
		{
			int mapitem = -1;
			int mapitemptr = -1;
			do //check all items in this cell
			{
				mapitemptr = mapRegions->GetNextItem( checkgrid + i, mapitemptr );
				if( mapitemptr == -1 ) break;
				mapitem = mapRegions->GetItem( checkgrid + i, mapitemptr );
				int mapchar = mapitem - 1000000;
				if( mapitem != -1 && mapitem >= 1000000 )
				{
					if( mapchar > -1 && mapchar < cmem )
					{
						if( chardist( p, mapchar ) < 15 )
						{
							if( !chars[mapchar].dead && !(chars[mapchar].flag&0x04) )
//							if( !chars[mapchar].dead && !(chars[mapchar].flag & 0x04) && !(chars[mapchar].flag & 0x02) || chars[mapchar].crimflag != -1 )
								Combat->SpawnGuard( mapchar, mapchar, chars[mapchar].x, chars[mapchar].y, chars[mapchar].z );
						}
					}
				}
			} while ( mapitemptr != -1 );
		}
	}
}

void mounthorse( UOXSOCKET s, int x) // Remove horse char and give player a horse item
{
	int j,c;
	static int count = 1;
	
	if(npcinrange(s,x,2)==0) 
		return;
	if( chars[currchar[s]].priv&0x01 )
	{
		setserial( x, currchar[s], 5 );
		chars[x].npcWander = 0;
		chars[x].npcaitype = 0;
		chars[x].tamed = true;
	}
	if( chars[x].ownserial == chars[currchar[s]].serial )
	{
		if (chars[currchar[s]].onhorse)
		{
			sysmessage(s,"You are already on a mount.");
			return;
		}
		strcpy(temp, chars[x].name);
		chars[currchar[s]].onhorse=1;
		c=Items->SpawnItem(s, 1, temp, 0, 0x09, 0x15, chars[x].skin1, chars[x].skin2, 0, 0);
		if( c == -1 )
			return;
		
		items[c].id1 = 0x3E;
		// krazyglue 12 October, 1999 - if, if, if... is now if, else if, else if
		switch( chars[x].id2 )
		{
		case 0xC8:	items[c].id2 = 0x9F;	break;
		case 0xE2:	items[c].id2 = 0xA0;	break;
		case 0xE4:	items[c].id2 = 0xA1;	break;
		case 0xCC:	items[c].id2 = 0xA2;	break;
		case 0xD2:	items[c].id2 = 0xA3;	break;
		case 0xDA:	items[c].id2 = 0xA4;	break;
		case 0xDB:	items[c].id2 = 0xA5;	break;
		case 0xDC:	items[c].id2 = 0xA6;	break;
		}		
		setserial(c,currchar[s], 4);
		items[c].layer=0x19;
		
        mapRegions->RemoveItem(c);
		
		items[c].x=chars[x].fx1;
		items[c].y=chars[x].fy1;
		items[c].z=chars[x].fz1;
		
		
        mapRegions->AddItem(c); // lord Binary
		
		items[c].moreb1=chars[x].npcWander;
		items[c].att=chars[x].fx2;
		items[c].def=chars[x].fy2;
		
		// AntiChrist bugfixes - 11/10/99
		items[c].moreb2 = chars[x].st;
		items[c].moreb3 = chars[x].dx;
		items[c].moreb4 = chars[x].in;
		items[c].hp = chars[x].hp;
		items[c].lodamage = chars[x].fame;
		items[c].hidamage = chars[x].karma;
		items[c].poisoned = chars[x].poisoned;
		if( chars[x].summontimer != 0 )
			items[c].decaytime = chars[x].summontimer;
		
		wornitems(s, currchar[s]); // send update to current socket
		for (j=0;j<now;j++) // and to all inrange sockets (without re-sending to current socket )
		{
			if( perm[j] && s != j && inrange1(s, j) ) 
				wornitems( j, currchar[s] );
		}
		Npcs->DeleteChar(x);
	}
	else
	{
		sysmessage(s, "You dont own that creature.");
	}
}

//o----------------------------------------------------------------------------o
//|   Function -  void checkkey()
//|   Date     -  Unknown
//|   Programmer  -  Unknown  (Touched up by EviLDeD)
//o----------------------------------------------------------------------------o
//|   Purpose     -  Facilitate console control. SysOp keys, and localhost 
//|               controls.
//o----------------------------------------------------------------------------o

void processkey( int c )
{
	if( c == 'S' )
	{
		if (secure)
		{
			printf("UOX3: Secure mode disabled. Press ? for a commands list.\n");
			secure=0;
			return;
		}
		else
		{
			printf("UOX3: Secure mode re-enabled.\n");
			secure=1;
			return;
		}
	} 
	else 
	{
		int i, j;
		if( secure == 1 && c != '?' )
		{
			printf("UOX3: Secure mode prevents keyboard commands! Press 'S' to disable.\n");
			return;
		}
		
		switch( c )
		{
		case 0x1B:
		case 'Q':
			printf("UOX3: Immediate Shutdown initialized!\n");
			keeprun=0;
			break;
		case 'T':
			endtime = uiCurrentTime + ( CLOCKS_PER_SEC * 600 );
			endmessage( 0 );
			printf( "UOX3: Server going down in %i minutes.\n", ( ( endtime - uiCurrentTime ) / CLOCKS_PER_SEC ) / 60 );
			break;
		case '#':
			if ( !cwmWorldState->Saving() )
			{
				cwmWorldState->savenewworld(1);
				saveserverscript(1);
			}
			break;
		case 'L':
			if (showlayer)
			{
				printf("UOX3: Layer display disabled.\n");
				showlayer=0;
			}
			else
			{
				printf("UOX3: Layer display enabled.\n");
				showlayer=1;
			}
			break;
		case 'I':
			Admin->ReadIni();
			printf("UOX3: INI file reloaded.\n");
			break;
		case  'D':    // Disconnect account 0 (useful when client crashes)
			for( i = 0; i < now; i++ )
				if( acctno[i] == 0 )
				{
					Network->Disconnect( i );
					printf( "Account 0 disconnected\n" );
					break;
				}
				printf( "Account 0 was not connected\n" );
			break;
		case 'H':                // Enable/Disable heartbeat
			if (heartbeat==1) printf("UOX3: Heartbeat Disabled\n");
			else printf("UOX3: Heartbeat Enabled\n");
			heartbeat = !heartbeat;
			break;
		case 'P':                // Display profiling information
			LogMessage("Performace Dump:\n");
			LogMessage("Network code: %fmsec [%i samples]\n" _ (float)((float)networkTime/(float)networkTimeCount) _ networkTimeCount);
			LogMessage("Timer code: %fmsec [%i samples]\n" _ (float)((float)timerTime/(float)timerTimeCount) _ timerTimeCount);
			LogMessage("Auto code: %fmsec [%i samples]\n" _ (float)((float)autoTime/(float)autoTimeCount) _ autoTimeCount);
			LogMessage("Loop Time: %fmsec [%i samples]\n" _ (float)((float)loopTime/(float)loopTimeCount) _ loopTimeCount);
			LogMessage("Characters: %i/%i - Items: %i/%i (Dynamic)\n" _ chars.Count() _ cmem _ items.Count() _ imem);
			LogMessage("Simulation Cycles: %f per sec\n" _ (1000.0*(1.0/(float)((float)loopTime/(float)loopTimeCount))));
			break;
		case 'W':                // Display logged in chars
			printf("Current Users in the World:\n");
			j = 0;
			for (i=0;i<now;i++)
			{
				if(perm[i]) //Keeps NPC's from appearing on the list
				{
					printf("%i) %s [%x %x %x %x]\n", j, chars[currchar[i]].name, chars[currchar[i]].ser1, chars[currchar[i]].ser2, chars[currchar[i]].ser3, chars[currchar[i]].ser4);
					j++;
				}
			}
			printf("Total Users Online: %d\n", j);
			break;
		case 'A': //reload the accounts file
			printf( "UOX3: Reloading Accounts file..." );
			Admin->LoadAccounts();
			printf( "Done!" );
			break;
		case 'R':
			printf("UOX3: Reloading Server/Spawn/Regions/Spells Script files:\n");
			printf( "Loading spawn regions..." );
			loadspawnregions();
			printf( "Done!\nLoading regions..." );
			loadregions();
			printf( "Done!\nLoading server.scp..." );
			loadserverscript();
			printf( "Done!\nLoading Spells.scp...");
			Magic->LoadScript();
			printf( "Done!\n" );
			Commands->Load();
			printf( "Reloading races.scp..." );
			delete Races;
			if(( Races = new cRaces )             == NULL ) Shutdown( FATAL_UOX3_ALLOC_RACES );
			Races->load();
			printf( "Done!\n" );
			
			break;
		case 'M':
			unsigned long int tmp, total;
			total = 0;
			tmp = 0;
			printf("UOX3 Memory Information:\n" );
			printf("        Cache:\n");
			printf("                Tiles: %li bytes\n", Map->TileMem );
			printf("                Statics: %li bytes\n", Map->StaMem );
			printf("                Version: %li bytes\n", Map->versionMemory );
			printf("                Map0: %i bytes [%i Hits - %i Misses]\n", 9*MAP0CACHE, Map->Map0CacheHit, Map->Map0CacheMiss );
			total += tmp = chars.Size() + cmem*sizeof( teffect_st ) + cmem*sizeof(char) + cmem*sizeof(int)*5;
			printf("        Characters: %li bytes [%i chars (%i allocated)]\n", tmp, chars.Count(), cmem );
			total += tmp = items.Size() + imem*sizeof(int)*4;
			printf("        Items: %li bytes [%i items (%i allocated)]\n", tmp, items.Count(), imem );
			printf("                You save I:%li & C:%li bytes!\n", ((imem*sizeof(item_st))-items.Size()), (((cmem*sizeof(char_st))-chars.Size())+((sizeof(teffect_st)*5*cmem)-(sizeof(teffect_st)*cmem))));
			total+= tmp = 69 * sizeof( splInfo_st );
			printf("        Spells: %i bytes\n", tmp );
			printf("        Sizes:\n" );
			printf("                Item_st: %i bytes\n", sizeof( item_st ) );
			printf("                Char_st: %i bytes\n", sizeof( char_st ) );
			printf("                TEffect: %i bytes (%i total)\n", sizeof( teffect_st ), sizeof( teffect_st ) * Effects->Count() );
			printf("                Int    : %i bytes\n", sizeof( int ) );
			printf("                Short  : %i bytes\n", sizeof( short int ) );
			total+= tmp = Map->TileMem + Map->StaMem + Map->versionMemory;
			printf("Approximate Total: %i bytes\n", total );
			//printf("End of Memory Information.\n" );
			break;
			
		case '?':
			printf("Console commands:\n");
			printf("	<Esc> or Q: Shutdown the server.\n");
			printf("	# - Save world\n");
			printf("	A - Reload accounts file\n");
			printf("	C - Dump Items.scp menu into a file.\n" );
			printf("	D - Disconnect Account 0\n");
			printf("	H - Toggle hearbeat\n");
			printf("	I - Reload INI file.\n");
			printf("	L - Toggle layer Display\n");
			printf("	M - Display Memory Information\n" );
			printf("	P - Performance Dump\n");
			printf("	R - Reload server, spawn, commands, regions and races scripts.\n");
			printf("	S - Toggle console secure mode ");	// This key was missing
			if( secure )
				printf( "[Enabled]\n" );
			else
				printf( "[Disabled]\n" );
			printf("	T - System Message: The server is shutting down in 10 minutes.\n");
			printf("	W - Display logged in characters\n");
			printf("	X - Mass Disconnect\n" );
			printf("	? - Commands list (this)\n");
			printf("End of commands list.\n");
			break;

		case 'b':
		case 'B':
				printf( "Item_st: %i\nTile_st: %i\nChar_st: %i\nmulti_st: %i\nland_st: %i\n", sizeof( item_st ), sizeof( tile_st ), sizeof( char_st ), sizeof( st_multi ), sizeof( land_st ) );
			break;

		case 'X':
			for( i = now - 1; i >= 0; i-- )
				Network->Disconnect( i );
			break;
		case 0x0D:	// User hit [Enter]
			break;
		default:
			printf("UOX3: Key %c [%x] does not perform a function. Press ? for help\n",c,c);	// Looks Better
			break;
		}
	}
}
#ifdef __CLUOX__
void checkkey( void )
{
	int c = cl_getch();
	if (c > 0)	
	{
		if ((cluox_io) && (c==250)) 
		{  // knox force unsecure mode, need this since cluox can't know
			//      how the toggle status is.
			if( secure )
			{
				printf("UOX3: Secure mode disabled. Press ? for a commands list.\n");
				secure=0;
				return;
			}
		}
		c = toupper(c);
		processkey( c );
	}
}

#else
void checkkey( void )
{
	char c;
	int j=0;	
	int indexcount=0, kill=0;
	
#ifdef __NT__
	
	if (kbhit())
	{
		c=toupper(getch());
#endif
#ifdef __LINUX__
		int s;
		fd_set  KEYBOARD;
		FD_ZERO( &KEYBOARD );
		FD_SET( 0, &KEYBOARD );
		s = select( 1, &KEYBOARD, NULL, NULL, &uoxtimeout );
		if( s < 0 )
		{
			printf("Error scanning key press\n" );
			exit( 10 );
		}
		if( s > 0 )
		{
			read( 0, &c, 1 );
			if( c != 0x0a )
			{
#endif
			processkey( c );
#ifdef __LINUX__
		}
#endif
		}
	}
}
#endif
inline void checktimers( void ) // Check shutdown timers
{
	overflow = ( lclock > uiCurrentTime );	// Small optimization
	if( endtime )
	{
		if( endtime <= uiCurrentTime ) 
			keeprun = 0;
	}
	lclock = uiCurrentTime;
}

// given a region, return a random spot within it thats valid for placing items
bool FindSpotForItem(int r, int &x, int &y, int &z)
{
	int a=0;
	do {
		x=RandomNum(spawnregion[r].x1,spawnregion[r].x2);
		y=RandomNum(spawnregion[r].y1,spawnregion[r].y2);
		z=Map->MapElevation( x, y );
		
		if (Map->CanMonsterMoveHere(x, y, z))
			return true;
	} while (++a < 100);
	printf("UOX3: Problem regionspawn [%i] found. Nothing will be spawned.\n",r);
	return false;
}

//NEW REGIONSPAWNS ZIPPY CODE STARTS HERE -- AntiChrist merging codes -- (24/6/99)
void doregionspawn(int r)//Regionspawns
{
	if (spawnregion[r].current<0) spawnregion[r].current=0;
	if (spawnregion[r].current >= spawnregion[r].max || spawnregion[r].max == 0) return;
	
	// from what i can tell, this was supposed to kind of pick one method of spawning at random
	// of course, if one is not available choose another
	int npc = -1, item = -1;
	int tries = 0, choice = rand()%3, counter = 0;
	while (tries++ < 3 && item == -1 && npc == -1)
	{
		switch( choice )
		{
		case 0:
			if( spawnregion[r].totalnpclists > 0 )
			{
				for( counter = 0; counter < spawnregion[r].call; counter++ )
					npc = Npcs->AddRespawnNPC( -1, r, spawnregion[r].npclists[rand()%spawnregion[r].totalnpclists], 1 );
				// changed last arg to 1 to fix spawning bug - fur
			}
			break;
		case 1:
			if( spawnregion[r].totalitemlists > 0 )
			{
				for( counter = 0; counter < spawnregion[r].call; counter++ )
				{
					char list[512];
					sprintf( list, "%i", spawnregion[r].itemlists[rand()%spawnregion[r].totalitemlists] ); // morrolan
					int num = Items->CreateRandomItem( list );
					item = Items->CreateScriptItem( -1, num, 0 );
					RefreshItem( item );
				}
			}
			break;
		case 2:
			if( spawnregion[r].totalnpcs>0 )
			{
				npc = Npcs->AddRespawnNPC( -1, r, spawnregion[r].npcs[rand()%spawnregion[r].totalnpcs], 1 );
				// changed last arg to 1 to fix spawning bug - fur
			}
			break;
		}
		if (++choice > 2)
			choice = 0;
	}
	if (npc==-1 && item==-1)
	{
#ifdef DEBUG_SPAWN
		printf("Warning: Region spawn %i [%s] couldn't find anything to spawn, check scripts.\n",r-1,spawnregion[r].name);
#endif
		return;
	}
	
	if (-1 != item)
	{
		int x = 0, y = 0, z = 0;
		if( FindSpotForItem( r, x, y, z ) )
		{
			mapRegions->RemoveItem( item ); // shouldn't be in the regions, at all, but who knows ...
			items[item].x=x;
			items[item].y=y;
			items[item].z=z;
			items[item].spawn1 = 0;
			items[item].spawn2 = 1;
			items[item].spawn3 = r;
			items[item].spawn4 = 0;
			items[item].spawnserial = (1<<16) + (r<<8);
			mapRegions->AddItem(item);
		} else {
			Items->DeleItem(item);
			spawnregion[r].current--;
		}
	}
	spawnregion[r].current++;
	
}
//NEW REGIONSPAWNS ZIPPY CODE ENDS HERE -- AntiChrist merging codes --

void doLightEffect(int i, int currenttime)
{
	bool didDamage = false;
	
	if( !chars[i].npc && online( i ) && !(chars[i].priv&0x01) && Races->getLightAffect( chars[i].race ) )
	{
		if (chars[i].fixedlight!=255 )
		{
			if( hour < 5 && ampm==1 || hour >= 5 && ampm==0 )	// time of day we can be burnt
			{
				if( chars[i].weathDamage[LIGHT] != 0 )
				{
					if( chars[i].weathDamage[LIGHT] <= currenttime )
					{
						sysmessage( calcSocketFromChar(i), "You are scalded by the fury of the light!" );
						chars[i].hp -= Races->getLightDamage( chars[i].race );
						chars[i].weathDamage[LIGHT] = currenttime + CLOCKS_PER_SEC*Races->getLightSecs( chars[i].race );
						staticeffect(i, 0x37, 0x09, 0x09, 0x19);
						soundeffect2(i, 0x02, 0x08);     
						didDamage = true;
					}
				}
				else
				{
					chars[i].weathDamage[LIGHT] = currenttime + CLOCKS_PER_SEC*Races->getLightSecs( chars[i].race );
				}
				
			}
			else if( hour < 6 && ampm==1 || hour >= 4 && ampm==0 )	// slightly burnt at this time of day
			{
				if( chars[i].weathDamage[LIGHT] != 0 )
				{
					if( chars[i].weathDamage[LIGHT] <= currenttime )
					{
						sysmessage( calcSocketFromChar(i), "You are burnt by the light's fury!" );
						chars[i].hp -= Races->getLightDamage( chars[i].race ) / 2;
						chars[i].weathDamage[LIGHT] = currenttime + CLOCKS_PER_SEC*Races->getLightSecs( chars[i].race )*2;
						staticeffect(i, 0x37, 0x09, 0x09, 0x19);
						soundeffect2(i, 0x02, 0x08);     
						didDamage = true;
					}
				}
			}
			else
			{
				chars[i].weathDamage[LIGHT] = 0;
				if( hour > 3 && hour < 4 && ampm==0 )
					sysmessage( calcSocketFromChar(i), "The sun will rise soon!" );
			}
		}
		else
		{
			if( !indungeon(i) )
			{
				if( hour < 5 && ampm==1 || hour >= 5 && ampm==0 )
				{
					if( chars[i].weathDamage[LIGHT] != 0 )
					{
						if( chars[i].weathDamage[LIGHT] <= currenttime )
						{
							sysmessage( calcSocketFromChar(i), "You are scalded by the fury of the light!" );
							chars[i].hp -= Races->getLightDamage( chars[i].race );
							chars[i].weathDamage[LIGHT] = currenttime + CLOCKS_PER_SEC*Races->getLightSecs( chars[i].race );
							staticeffect(i, 0x37, 0x09, 0x09, 0x19);
							soundeffect2(i, 0x02, 0x08);     
							didDamage = true;
						}
					}
					else
					{
						chars[i].weathDamage[LIGHT] = currenttime + CLOCKS_PER_SEC*Races->getLightSecs( chars[i].race );
					}
					
				}
				else if( hour < 6 && ampm==1 || hour >= 4 && ampm==0 )
				{
					if( chars[i].weathDamage[LIGHT] != 0 )
					{
						if( chars[i].weathDamage[LIGHT] <= currenttime )
						{
							sysmessage( calcSocketFromChar(i), "You are burnt by the light's fury!" );
							chars[i].hp -= Races->getLightDamage( chars[i].race ) / 2;
							chars[i].weathDamage[LIGHT] = currenttime + CLOCKS_PER_SEC*Races->getLightSecs( chars[i].race )*2;
							staticeffect(i, 0x37, 0x09, 0x09, 0x19);
							soundeffect2(i, 0x02, 0x08);     
							didDamage = true;
						}
					}
				}
				else
				{
					chars[i].weathDamage[LIGHT] = 0;
					if( hour > 3 && hour < 4 && ampm==0 )
						sysmessage( calcSocketFromChar(i), "The sun will rise soon!" );
				}
			}
			else
			{
				if( hour >= 5 && hour <= 6 && ampm==1 && chars[i].weathDamage[LIGHT] <= currenttime )
				{
					sysmessage( calcSocketFromChar(i), "The sun will set soon!" );
					chars[i].weathDamage[LIGHT] = currenttime + CLOCKS_PER_SEC*Races->getLightSecs( chars[i].race )*2;
				}
			}
		}
	}
	if (didDamage)
		updatestats(i, 0);
}

//NEW LAGFIX ZIPPY CODE STARTS HERE -- AntiChrist merging codes -- (24/6/99)
void genericCheck(int i, int currenttime)//Char mapRegions
{
	int c;
	
	// if they are already dead we don't need to check anything
	if (chars[i].dead)
		return;
	
	
	if (chars[i].hp>chars[i].st)
	{
		chars[i].hp=chars[i].st;
		updatestats(i, 0);
	}
	if (chars[i].stm>chars[i].dx)
	{
		chars[i].stm=chars[i].dx;
		updatestats(i, 2);
	}
	if (chars[i].mn>chars[i].in)
	{
		chars[i].mn=chars[i].in;
		updatestats(i, 1);
	}
	if ((chars[i].regen<=currenttime)||(overflow))
	{
		if (chars[i].hp<chars[i].st && chars[i].hunger > server_data.hungerThreshold || server_data.hungerrate==0)
		{
			for(c=0;c<chars[i].st+1;c++)
			{
				if(chars[i].regen+(c*server_data.hitpointrate*CLOCKS_PER_SEC)<=currenttime && chars[i].hp<=chars[i].st)
				{
					if (chars[i].skill[17]<500) chars[i].hp++;
					else if (chars[i].skill[17]<800) chars[i].hp += 2;
					else chars[i].hp += 3;
					if (chars[i].hp>chars[i].st) 
					{
						chars[i].hp=chars[i].st; 
						break;
					}
					updatestats(i, 0);
				}
			}
		}
		chars[i].regen=currenttime+(server_data.hitpointrate*CLOCKS_PER_SEC);
	}
	if ((chars[i].regen2<=currenttime)||(overflow))
	{
		for(c=0;c<chars[i].dx+1;c++)
		{
			if(chars[i].regen2+(c*server_data.staminarate*CLOCKS_PER_SEC)<=currenttime && chars[i].stm<=chars[i].dx)
			{
				chars[i].stm++;
				if (chars[i].stm>chars[i].dx) 
				{
					chars[i].stm=chars[i].dx; 
					break;
				}
				updatestats(i, 2);
			}
		}
		chars[i].regen2=currenttime+(server_data.staminarate*CLOCKS_PER_SEC);
	}
	if ((chars[i].regen3<=currenttime)||(overflow))
	{  
		for(c=0;c<chars[i].in+1;c++)
		{
			if (chars[i].regen3+(c*server_data.manarate*CLOCKS_PER_SEC)<=currenttime && chars[i].mn<=chars[i].in)
			{
				chars[i].mn++;
				if (chars[i].mn>chars[i].in) 
				{
					if (chars[i].med) // Morrolan = Meditation
					{
						int s = calcSocketFromChar(i);
						sysmessage(s, "You are at peace.");
						chars[i].med=0;
					}
					chars[i].mn=chars[i].in; 
					break;
				}
				updatestats(i, 1);
			}
		}
		if(server_data.armoraffectmana) // Morrolan - Meditation
		{
			if(chars[i].med)
			{
				chars[i].regen3=currenttime+((server_data.manarate*CLOCKS_PER_SEC)/2);
			}
			else chars[i].regen3=currenttime+((server_data.manarate+(Combat->CalcDef(i,0)-2))*CLOCKS_PER_SEC);    
		}
		else chars[i].regen3=currenttime+(server_data.manarate*CLOCKS_PER_SEC);  
	}
	if ((chars[i].hidden==2)&&((chars[i].invistimeout<=currenttime)||(overflow)) && ( !(chars[i].priv2&8 )))
	{
		chars[i].hidden=0;
		chars[i].stealth = -1;
		updatechar(i);
	}
	
	doLightEffect(i, currenttime);
	doRainEffect(i, currenttime);
	doSnowEffect(i, currenttime);
	
	if (chars[i].hp<=0 && !chars[i].dead) deathstuff(i);
}

void checkPC(int i, int currenttime, bool doWeather)// Char mapRegions
{
	int pcalc, timer;
	char t[120];
	
	LIGHTLEVEL toShow;
	int s = calcSocketFromChar(i);// Only calc socket once!
	
	Magic->CheckFieldEffects2(currenttime, i, 1);// Lag fix
	
	if (!chars[i].dead && chars[i].swingtarg == -1)
		Combat->DoCombat(i, currenttime);
	else if (!chars[i].dead &&(chars[i].swingtarg != -1 && chars[i].timeout <= currenttime))
		Combat->CombatHit(i, chars[i].swingtarg, currenttime);
	
	
	//	if( lighttime <= currenttime || (overflow)) // saves tons of bandwidth -> LB
	if (doWeather)
	{
		if (Races->getVisLevel(chars[i].race) > worldcurlevel)
			toShow = 0;
		else
			toShow = worldcurlevel - Races->getVisLevel(chars[i].race);
		dolight(s, toShow);
		Weather->doPlayerStuff(i);
	}
	
	if (chars[i].smoketimer > currenttime)
	{
		if (chars[i].smokedisplaytimer <= currenttime)
		{
			chars[i].smokedisplaytimer = currenttime + 5*CLOCKS_PER_SEC;
			staticeffect(i, 0x37, 0x35, 0, 30);
			soundeffect2(i, 0x00, 0x2B);
			switch( rand()%14 )
			{
			case 0:		npcemote( s, i, "*Drags in deep*", 1 ); break;
			case 1:		npcemote( s, i, "*Coughs*", 1 ); break;
			case 2:		npcemote( s, i, "*Retches*", 1 ); break;
			case 3:		npcemote( s, i, "*Hacking cough*", 1 ); break;
			case 4:		npcemote( s, i, "*Sighs in contentment*", 1 ); break;
			case 5:		npcemote( s, i, "*Puff puff*", 1 ); break;
			case 6:		npcemote( s, i, "Wheeeee!!! Xuri's smoking!", 1 ); break;
			default:	break;
			}
		}
	}
	
	if (LSD[s])
		do_lsd(s); // LB's LSD potion-stuff
	
	
	if (!chars[i].npc && online(i) && chars[i].squelched == 2)
	{
		if (chars[i].mutetime!=-1)
		{
			if (chars[i].mutetime <= currenttime || overflow)
			{
				chars[i].squelched = 0;
				chars[i].mutetime=-1;
				sysmessage(s, "You are no longer squelched!");
			}
		}
	}
	
	if (!chars[i].npc && online(i))
	{
		if ((chars[i].crimflag > 0) &&(chars[i].crimflag <= currenttime || overflow) &&(chars[i].flag & 0x02))
		{
			sysmessage(s, "You are no longer a criminal.");
			chars[i].crimflag=-1;
			setcharflag(i);
		}
		chars[i].murderrate--;
		
		if (chars[i].murderrate <= currenttime)
		{
			if (chars[i].kills>0)
				chars[i].kills--;
			if ((chars[i].kills == repsys.maxkills) && (repsys.maxkills>0))
				sysmessage(s, "You are no longer a murderer.");
			// printf("%i Murderrate (Decay %i) (%i kills)",chars[i].murderrate,repsys.murderdecay,chars[i].kills);
			chars[i].murderrate = (repsys.murderdecay*CLOCKS_PER_SEC) + currenttime;// Murder rate (in mins) to seconds. (checkauto is done about 25 times per second)
		}
		setcharflag(i);
	}
	
	if (!chars[i].npc && chars[i].casting == 1)// PC casting a spell
	{
		chars[i].nextact--;
		if (chars[i].spelltime <= currenttime || overflow)// Spell is complete target it.
		{
			if (Magic->requireTarget(chars[i].spellCast))
			{
				target(s, 0, 1, 0, 100, spells[chars[i].spellCast].strToSay);
			}
			else
			{
				Magic->NewCastSpell(s);
			}
			chars[i].casting = -1;
			chars[i].spelltime = 0;
			chars[i].priv2 &= 0xFD;
		} 
		else if (chars[i].nextact <= 0)// redo the spell action
		{
			chars[i].nextact = 75;
			if (!chars[i].onhorse)
				impaction(s, chars[i].spellaction);
		}
	}
	
	if (server_data.bg_sounds >= 1)
	{
		if (server_data.bg_sounds>10)
			server_data.bg_sounds = 10;
		timer = server_data.bg_sounds*100;
		if ((online(i)) && (!(chars[i].npc)) && (!(chars[i].dead)) && ((rand()%(timer)) == (timer/2)))
			bgsound(i); // lb, bgsound uses array positions not sockets !
	}
	
	
	//	if(chars[i].spiritspeaktimer) chars[i].spiritspeaktimer--;
	// modifyed by AntiChrist
	if (chars[i].spiritspeaktimer > 0 && chars[i].spiritspeaktimer < uiCurrentTime)
		chars[i].spiritspeaktimer = 0;
	
	if (chars[i].trackingtimer>currenttime && online(i))
	{
		if (chars[i].trackingdisplaytimer <= currenttime)
		{
			chars[i].trackingdisplaytimer = currenttime + tracking_data.redisplaytime*CLOCKS_PER_SEC;
			Skills->Track(i);
		}
	}
	else
	{
		if (chars[i].trackingtimer >(currenttime / 10)) // dont send arrow-away packet all the time
		{
			chars[i].trackingtimer = 0;
#if CLIENTVERSION_M == 26
			unsigned char arrow[7];
			arrow[0] = 0xBA;
			arrow[1] = 0;
			arrow[2] = (chars[chars[currchar[s]].trackingtarget].x - 1) >> 8;
			//			arrow[3]=(chars[chars[currcha
			arrow[3] = (chars[chars[currchar[s]].trackingtarget].x - 1)%256;
			arrow[4] = (chars[chars[currchar[s]].trackingtarget].y) >> 8;
			arrow[5] = (chars[chars[currchar[s]].trackingtarget].y)%256;
			Network->xSend(s, arrow, 6, 0);
#endif
		}
	}
	
	if (chars[i].fishingtimer)
	{
		if (chars[i].fishingtimer <= uiCurrentTime)
		{
			Skills->Fish(i);
			chars[i].fishingtimer = 0;
		}
	}
	if (server_data.hungerrate>1 &&(chars[i].hungertime <= currenttime || overflow))
	{
		if (chars[i].npc)
		{
			if (chars[i].hunger)
				chars[i].hunger--; // Morrolan GMs and Counselors don't get hungry
		}
		else
		{
			if (!(chars[i].priv&0x80) && !(chars[i].priv&0x01) &&(chars[i].hunger) &&(online(i)))
			{
				chars[i].hunger--; // Morrolan GMs and Counselors don't get hungry
			}
		}
		
		switch (chars[i].hunger)
		{
			case 6: 
				break; // Morrolan
			case 5:
				sysmessage(s, "You are still stuffed from your last meal");
				break;
			case 4:
				sysmessage(s, "You are not very hungry but could eat more");
				break;
			case 3:
				sysmessage(s, "You are feeling fairly hungry");
				break;
			case 2:
				sysmessage(s, "You are extremely hungry");
				break;
			case 1:
				sysmessage(s, "You are very weak from starvation");
				break;
			case 0:
				if (chars[i].priv&0x80 == 0) 
					sysmessage(s, "You must eat very soon or you will die!");
				break;
		}
		chars[i].hungertime = currenttime + (server_data.hungerrate*CLOCKS_PER_SEC); // Bookmark
	}
	if (((hungerdamagetimer <= currenttime) || (overflow)) && (server_data.hungerdamage>0)) // Damage them if they are very hungry
	{
		hungerdamagetimer = currenttime + (server_data.hungerdamagerate*CLOCKS_PER_SEC); /** set new hungertime **/	     
		if (chars[i].hp>0 && chars[i].hunger < 2 &&(!(chars[i].priv&0x80)) &&(!(chars[i].dead)))
		{     
			sysmessage(s, "You are starving !");
			chars[i].hp -= server_data.hungerdamage;
			updatestats(i, 0);
			if (chars[i].hp <= 0 && !chars[i].dead)
			{ 
				sysmessage(s, "You have died of starvation");
				deathstuff(i);
			}
		}
	}
	
	// new math + poison wear off timer added by lord binary !
	
	if ((chars[i].poisoned) &&(online(i) ||(chars[i].npc)) && !(chars[i].priv&4))
	{
		if ((chars[i].poisontime <= currenttime) || (overflow))
		{
			if (chars[i].poisonwearofftime>currenttime) // lb, makes poison wear off pc's
			{
				switch (chars[i].poisoned)
				{
					case 1:
						chars[i].poisontime = currenttime + (5*CLOCKS_PER_SEC);
						if ((chars[i].poisontxt <= currenttime) || (overflow))
						{
							chars[i].poisontxt = currenttime + (10*CLOCKS_PER_SEC);
							sprintf(t, "* %s looks a bit nauseous *", chars[i].name);
							chars[i].emotecolor1 = 0x00;
							// buffer[s][4];
							chars[i].emotecolor2 = 0x26;
							// buffer[s][5];
							npcemoteall(i, t, 1);
						}
						// npctalkall(i,t);
						chars[i].hp -= RandomNum(1, 2);
						updatestats(i, 0);
						break;
					case 2:
						chars[i].poisontime = currenttime + (4*CLOCKS_PER_SEC);
						if ((chars[i].poisontxt <= currenttime) || (overflow))
						{
							chars[i].poisontxt = currenttime + (10*CLOCKS_PER_SEC);
							sprintf(t, "* %s looks disoriented and nauseous! *", chars[i].name);
							chars[i].emotecolor1 = 0x00;
							// buffer[s][4];
							chars[i].emotecolor2 = 0x26;
							// buffer[s][5];
							npcemoteall(i, t, 1);
							// npctalkall(i,t);     
						}
						pcalc = (chars[i].hp*RandomNum(2, 5)/100) + RandomNum(0, 2); // damage: 1..2..5% of hp's+ 1..2 constant
						chars[i].hp -= pcalc;
						updatestats(i, 0);
						break;
					case 3:
						chars[i].poisontime = currenttime + (3*CLOCKS_PER_SEC);
						if ((chars[i].poisontxt <= currenttime) || (overflow))
						{
							chars[i].poisontxt = currenttime + (10*CLOCKS_PER_SEC);
							sprintf(t, "* %s is in severe pain! *", chars[i].name);
							chars[i].emotecolor1 = 0x00;
							// buffer[s][4];
							chars[i].emotecolor2 = 0x26;
							// buffer[s][5];
							npcemoteall(i, t, 1);
						}
						pcalc = (chars[i].hp/10) + RandomNum(1, 3); // damage: 5..10% of hp's+ 1..2 constant
						chars[i].hp -= pcalc;
						updatestats(i, 0);
						break;
					case 4:
						chars[i].poisontime = currenttime + (3*CLOCKS_PER_SEC);
						if ((chars[i].poisontxt <= currenttime) || (overflow))
						{
							chars[i].poisontxt = currenttime + (10*CLOCKS_PER_SEC);
							sprintf(t, "* %s looks extremely weak and is wrecked in pain! *", chars[i].name);
							chars[i].emotecolor1 = 0x00;
							// buffer[s][4];
							chars[i].emotecolor2 = 0x26;
							// buffer[s][5];
							npcemoteall(i, t, 1);
							// npctalkall(i,t);
						}
						pcalc = (chars[i].hp/5) + RandomNum(3, 6); // damage: 20% of hp's+ 3..6 constant, quite deadly <g>
						chars[i].hp -= pcalc;
						updatestats(i, 0);
						break;
					default:
						printf("ERROR: Fallout of switch statement without default. uox3.cpp, checkPC()\n"); // Morrolan
						chars[i].poisoned = 0;
						return;
				}
				if (chars[i].hp < 1)
				{
					deathstuff(i);
					sysmessage(s, "The poison has killed you.");
				} 
			} // end switch
		}  // end if poison-wear off-timer
	} // end if poison-damage timer
	
	if (chars[i].poisonwearofftime <= currenttime && chars[i].poisoned && online(i))
	{
		chars[i].poisoned = 0; 
		impowncreate(s, i, 1); // updating to blue stats-bar ...
		sysmessage(s, "The poison has worn off.");
	}
	
	if (chars[i].onhorse)
	{
		int horseItem = -1;
		ITEM toCheck = -1;
		for (int counter = 0; counter < contsp[chars[i].serial%HASHMAX].max; counter++)
		{
			toCheck = contsp[chars[i].serial%HASHMAX].pointer[counter];
			if (toCheck != -1 && items[toCheck].contserial == chars[i].serial)
			{
				if (items[toCheck].layer == 0x19)	
				{
					horseItem = toCheck;
				}
			}
		}
		if (horseItem == -1)
			chars[i].onhorse = false;	// turn it off, we aren't on one because there's no item!
		else
			if (items[horseItem].decaytime != 0 &&(items[horseItem].decaytime <= uiCurrentTime || overflow))
			{
				chars[i].onhorse = false;
				Items->DeleItem(horseItem);
			}
	}
}

void checkauto(void) // Check automatic/timer controlled stuff (Like fighting and regeneration)
{
	unsigned int currenttime = uiCurrentTime; //\/ getclock only once
	static unsigned int checkspawnregions = 0; 
	static unsigned int checknpcs = 0;
	static unsigned int checkitemstime = 0;
	static unsigned int htmltime = 0;
	static unsigned int generateweather = 0;
	static unsigned int uiSetFlagTime = 0;
	static unsigned int uiAccountCheck = 0;
	static unsigned int accountFlush = 0;
	static unsigned char counter = 0;
	bool doWeather = false;
	int i;
	
	if (uiAccountCheck <= currenttime || overflow)
	{
		for (int a = 0; a < acctcount; a++)
		{
			if (acctx[a].ban == 1 && acctx[a].banTime != -1 && acctx[a].banTime < uiCurrentTime)
			{
				acctx[a].ban = 0;
				acctx[a].banTime = -1;
			}
		}
		uiAccountCheck = (unsigned int)(60*CLOCKS_PER_SEC + currenttime); // 1 min per check won't hurt
	}
	
	if (speed.accountFlush != 0 && accountFlush <= currenttime || overflow)
	{
		int accountVerify = 0;
		bool reallyOn = false;
		for (int accountCheck = 0; accountCheck < MAXACCT; accountCheck++)	// let's see if each account is in use
		{
			if (acctinuse[accountCheck])	// it's considered in use
			{
				reallyOn = false;	// to start with, there's no one really on
				accountVerify = 0;
				do
				{
					if (chars[currchar[accountVerify]].account == accountCheck)	// if we really have an online player, flag it
						reallyOn = true;
					accountVerify++;
				} while (accountVerify < now && !reallyOn);                     // Stop searching when found.
				
				if (!reallyOn)	// no one's really on, let's set that
					acctinuse[accountCheck] = 0;
			}
		}
		accountFlush = (unsigned int)(speed.accountFlush*60*CLOCKS_PER_SEC + currenttime);
	}
	
	for (i = now - 1; i >= 0; i--)
	{
		if (!(chars[currchar[i]].priv&0x01) && idleTimeout[i] != -1 && uiCurrentTime >= idleTimeout[i])
		{
			idleTimeout[i] = -1;
			sysmessage(i, "You're being disconnected because you were idle too long");
			Network->Disconnect(i);
		} // put else here to send idle warning packet
	}
	
	if (checkspawnregions <= currenttime && speed.srtime != -1)// Regionspawns
	{
		for (i = 1; i < totalspawnregions; i++)
		{
			if (spawnregion[i].nexttime <= currenttime)
			{
				doregionspawn(i);
				spawnregion[i].nexttime = currenttime + (CLOCKS_PER_SEC*60*RandomNum(spawnregion[i].mintime, spawnregion[i].maxtime));
			}
		}
		checkspawnregions = uiCurrentTime + speed.srtime*CLOCKS_PER_SEC;// Don't check them TOO often (Keep down the lag)
	}
	
    if (server_data.html > 0 && (htmltime <= currenttime || overflow) )
	{
		updatehtml();
		htmltime = currenttime + (server_data.html*CLOCKS_PER_SEC);
	}

	
	if (saveinterval != 0)
	{
		if (autosaved == 0)
		{
			autosaved = 1;
#ifdef __LINUX__
			time((time_t *)&oldtime);
#else
			time(&oldtime);
#endif
		}
#ifdef __LINUX__
		time((time_t *)&newtime);
#else
		time(&newtime);
#endif
		if (difftime(newtime, oldtime) >= saveinterval || cwmWorldState->Saving())
		{
			// Dupois - Added Dec 20, 1999
			// After an automatic world save occurs, lets check to see if
			// anyone is online (clients connected).  If nobody is connected
			// Lets do some maintenance on the bulletin boards.
			if (!now && !cwmWorldState->Saving())
			{
				printf("UOX3: No players currently online. Starting bulletin board maintenance.\n");
				savelog("Bulletin Board Maintenance routine running (AUTO)\n", "server.log");
				MsgBoardMaintenance();
			}
			
			autosaved = 0;
			cwmWorldState->savenewworld(0);
		}
	}
	
	// Time functions
	if (uotickcount <= currenttime || (overflow))
	{
		if (minute < 59)
		{
			minute++;
			hbu++;
		}
		else
		{
			minute = 0;
			if (hour < 12)
				hour++;
			else
			{
				hour = 1;
				ampm = !ampm;
				if (!ampm)
					day++;
			}
		}
		uotickcount = currenttime + secondsperuominute*CLOCKS_PER_SEC;
		if (minute%8 == 0)
			moon1 = (moon1 + 1)%8;
		if (minute%3 == 0)
			moon2 = (moon2 + 1)%8;
	}
	
	if (lighttime <= currenttime ||(overflow))
	{
		counter = 0;		
		doworldlight();  // Changes lighting, if it is currently time to.
		Weather->doStuff();	// updates the weather types
		if (!ampm &&(hour == 0 && minute == 0))
		{
			Weather->newDay();
		}
		lighttime = currenttime + server_data.weathertime*CLOCKS_PER_SEC;	// for testing purposes
		doWeather = true;
	}
	else
		doWeather = false;
	
	for (i = 0; i < now; i++)
	{
		if (perm[i] && online(currchar[i]) && chars[currchar[i]].account == acctno[i])
		{
			if (uiSetFlagTime <= currenttime ||(overflow))
				setcharflag(i); // only set flag on npcs every 60 seconds (save a little extra lag)
			genericCheck(currchar[i], currenttime);
			checkPC(currchar[i], currenttime, doWeather);
			
			int	StartGrid = mapRegions->StartGrid(chars[currchar[i]].x, chars[currchar[i]].y);
			
			unsigned int increment = 0;
			for (unsigned int checkgrid = StartGrid + (increment*mapRegions->GetColSize()); increment < 3; increment++, checkgrid = StartGrid + (increment*mapRegions->GetColSize()))
			{
				for (int a = 0; a < 3; a++)
				{
					int mapitemptr=-1;
					int	mapitem=-1;
					int mapchar=-1;
					do // check all items in this cell
					{
						mapchar=-1;
						mapitemptr = mapRegions->GetNextItem(checkgrid + a, mapitemptr);
						if (mapitemptr==-1)
							break;
						mapitem = mapRegions->GetItem(checkgrid + a, mapitemptr);
						if (mapitem>999999)
							mapchar = mapitem - 1000000;
						if (mapitem>-1 && mapitem >= 1000000 &&(checknpcs <= currenttime || overflow))
						{ // Instalog // AntiChrist
							if (mapchar>-1 && mapchar < cmem) // Characters
							{
								if (chars[mapchar].npc)
									genericCheck(mapchar, currenttime); // lb, lagfix
								if (uiSetFlagTime <= currenttime ||(overflow))
									setcharflag(i); // only set flag on npcs every 60 seconds (save a little extra lag)
								if (chardist(currchar[i], mapchar) <= speed.tilecheck && chars[mapchar].npc)  // Zippy Changed to 'tilecheck' in scripts
									checkNPC(mapchar, currenttime);
								
								else if (!chars[mapchar].npc && 
									inworld[chars[mapchar].account] == mapchar && chars[mapchar].logout != -1 &&
									(chars[mapchar].logout <= currenttime || overflow ))
								{
									inworld[chars[mapchar].account]=-1;
									chars[mapchar].logout=-1;
									updatechar(mapchar);
								}
							}
						}
						else if (mapitem < 999999 && mapitem>-1 &&(checkitemstime <= currenttime || (overflow))) // 'Normal' world items
						{ // BugFix
							if (mapitem < imem) 
							{
								Items->RespawnItem(currenttime, mapitem);
								if (items[mapitem].type == 51 || items[mapitem].type == 52)
								{
									if (items[mapitem].gatetime <= currenttime) 
									{
										Items->DeleItem(mapitem);	// no guarentee on sequential nature of gates
									}
								}
								Items->DecayItem(currenttime, mapitem);
								
								if (items[mapitem].type == 88 && items[mapitem].morey < 25 && items[mapitem].morey >= 0)
								{
									if (itemdist(currchar[i], mapitem) <= items[mapitem].morey)
									{
										if (RandomNum(1, 100) <= items[mapitem].morez)
											soundeffect4(mapitem, i, items[mapitem].morex >> 8, items[mapitem].morex%256);
									}
								}
							}
						}
						else if (mapitem!=-1 && mapitem < imem)
						{
							// Boats
							if (items[mapitem].type == 117 && 
								(items[mapitem].type2 == 1 || items[mapitem].type2 == 2)&&
								(items[mapitem].gatetime <= currenttime || overflow))
							{
								if (items[mapitem].type2 == 1)
									Boats->Move(i, items[mapitem].dir, mapitem);
								else 
								{
									int dir = items[mapitem].dir + 4;
									
									if (dir>7)
										dir -= 8; // LB, BUGKILLING !!!
									
									Boats->Move(i, dir, mapitem);
								}
								items[mapitem].gatetime = (unsigned int)(currenttime + (double)(server_data.boatspeed*CLOCKS_PER_SEC));
							}
						}
					} while (mapitem!=-1); 
				}// a<3
			}// for checkgrid
		}// if online
	}// for i<now
	
	checktempeffects();
	if (uiSetFlagTime <= currenttime)
		uiSetFlagTime = (unsigned int)(30 * CLOCKS_PER_SEC + currenttime);

	// Slow down lag "needed" for setting flags, they are set often enough ;-)
	if (checknpcs <= currenttime)
		checknpcs = (unsigned int)((double)(speed.npctime*CLOCKS_PER_SEC + currenttime)); // lb

	if (checkitemstime <= currenttime)
		checkitemstime = (unsigned int)((double)(speed.itemtime*CLOCKS_PER_SEC + currenttime)); // lb
	// 2-24-00 homeys temp fix for vendor restocks, this solves the bad timer updates
	
	if ((shoprestocktime + 10000) <= currenttime)
		shoprestocktime = currenttime + (shoprestockrate*60*CLOCKS_PER_SEC);
	
	if (nextnpcaitime <= currenttime)
		nextnpcaitime = (unsigned int)((double) currenttime + (speed.npcaitime*CLOCKS_PER_SEC)); // lb
	
	if (nextfieldeffecttime <= currenttime)
		nextfieldeffecttime = (unsigned int)((double)currenttime + (0.5*CLOCKS_PER_SEC));
	
	if (server_data.UOXBot)
		checkdumpdata(currenttime); // This dumps data for Ridcully's UOXBot
	
	if (nextdecaytime <= currenttime)
		nextdecaytime = currenttime + (15*CLOCKS_PER_SEC); // lb ...	
}

//NEW LAGFIX ZIPPY CODE ENDS HERE -- AntiChrist merging codes --


void scriptlist(int x, int spc, int all)
{
	int pos, i;
	char file[512];
	
	openscript("items.scp");
	sprintf(temp, "ITEMMENU %i", x);
	if (!i_scripts[items_script]->find(temp))
	{
		closescript();
		if (n_scripts[custom_item_script][0]!=0)
		{
			openscript(n_scripts[custom_item_script]);
			if (!i_scripts[custom_item_script]->find(temp))
			{
				closescript();
				return;
			}
			else strcpy(file, n_scripts[custom_item_script]);
		} else return;
	} else strcpy(file, "items.scp");
	read1();
	do
	{
		read2();
		if (script1[0]!='}')
		{
			strcpy(temp, script2);
			read2();
			if ((all)||(!(strcmp("ITEMMENU",script1))))
			{
				for (i=0;i<spc;i++) fprintf(lstfile, " ");
				fprintf(lstfile, "%s (%s %s)\n",temp,script1,script2);
				if (!(strcmp("ITEMMENU",script1)))
				{
					if( !IsInMenuList( str2num( script2 ) ) )
					{
						AddToMenuList( str2num( script2 ) );
						pos=ftell(scpfile);
						closescript();
						scriptlist( str2num( script2 ), spc+2, all);
						openscript(file);
						fseek(scpfile, pos, SEEK_SET);
						strcpy(script1, "DUMMY");
					}
				}
			}
		}
	}
	while (script1[0]!='}');
	closescript();
}

void scriptmax(char *txt)
{
	int i, x, highest, current;
	char str[512];
	int tempLen;
	highest=-1;
	bool ok = true;

	printf( "Doing %s.....", txt );
	if( !( strcmp( "ITEM", txt ) ) || !( strcmp( "ITEMMENU", txt ) ) )
		openscript( "items.scp" );
	else if( !( strcmp( "NPC", txt ) ) )
		openscript( "npc.scp" );
	else if( !( strcmp( "SPEECH", txt ) ) )
		openscript( "speech.scp" );
	else if( !( strcmp( "BATCH", txt ) ) || !( strcmp( "GMMENU", txt ) ) )
		openscript( "menus.scp" );
	else if( !( strcmp( "LOCATION", txt ) ) )
		openscript( "location.scp" );			// locations in location.scp
	else
		openscript( "items.scp" );	// default to items.scp
	sprintf(str, "SECTION %s ", txt);
	do
	{
		readscript();
		ok = true;
		x = strlen( str );
		tempLen = strlen( temp );
		if( tempLen < x ) 
		{
			if( tempLen >= 7 )	// don't bother searching if it doesn't even contain SECTION!!!!!!!!!!
				x = strlen( temp );
			else
				continue;
		}
		for( i = 0; i < x; i++ )
		{
			if (str[i]!=temp[i]) 
			{
				ok = false;
				break;			// soon as we find one difference, clear out!
			}
		}
		if( ok )
		{
			current = str2num( &temp[x] );
			if (current==highest) 
				printf("WARNING: Duplicate section %s %i\n",txt,highest);
			if (current>highest) 
				highest=current;
		}
	}
	while( strcmp( "EOF", temp ) );
	closescript();
	fprintf(lstfile, " %s: %i\n", txt, highest);
	printf( "Done!\n" );
}

void DoorMacro( UOXSOCKET s)
{ //Zippy 11/10/99 For Door Macro use :o)
	unsigned short int xc = chars[currchar[s]].x, yc = chars[currchar[s]].y;
	//printf("DoorMacro executed!\n" );
	switch (chars[currchar[s]].dir)
	{
	case 0 : yc--;				break;
	case 1 : { xc++; yc--; }	break;
	case 2 : xc++;				break;
	case 3 : { xc++; yc++; }	break;
	case 4 : yc++;				break;
	case 5 : { xc--; yc++; }	break;
	case 6 : xc--;				break;
	case 7 : { xc--; yc--; }	break;
	}
	
	long moreSerial, moreSerial2;
	int pack, j, packItem;
	const int StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
	
	unsigned int checkgrid = 0;
	for (int increment = 0; increment < 3; increment++)
	{
		checkgrid = StartGrid + (increment * mapRegions->GetColSize());
		for (int i = 0; i < 3; i++)
		{
			int mapitem = -1;
			int mapitemptr = -1;
			do //check all items in this cell
			{
				mapitemptr = mapRegions->GetNextItem(checkgrid+i, mapitemptr);
				if (mapitemptr == -1) break;
				mapitem = mapRegions->GetItem(checkgrid+i, mapitemptr);
				if (mapitem != -1 && mapitem<imem)
				{
					if (items[mapitem].x == xc && items[mapitem].y == yc)
					{
						if( items[mapitem].type == 12 || items[mapitem].type == 13 )	// only open doors
						{
							if( items[mapitem].type == 13 )
							{
								pack = packitem(currchar[s]);
								if( pack > -1 ) //LB
								{
									for (j = 0; j < contsp[items[pack].serial%HASHMAX].max;j++)
									{
										packItem = contsp[items[pack].serial%HASHMAX].pointer[j];
										if (packItem > -1 && items[packItem].contserial == items[pack].serial )
										{
											moreSerial  = calcserial( items[packItem].more1, items[packItem].more2, items[packItem].more3, items[packItem].more4 );
											moreSerial2 = calcserial( items[mapitem].more1,  items[mapitem].more2,  items[mapitem].more3,  items[mapitem].more4 );
											if( moreSerial == moreSerial2 )	// matching key and door, already do in pack check above
											{
												npctalk( s, currchar[s],"You quickly unlock, use, and then relock the door.", 0);
												dooruse( s, mapitem);
												return;
											}//if
										}
									}//for
								} // end if p!=-1
								sysmessage(s, "This door is locked.");
								return;
							}

							dooruse(s, mapitem);
							return;
						}
					}
				}
			} while (mapitemptr != -1 );
		}//for i
	} //for inc
}

void cNetworkStuff::GetMsg(int s) // Receive message from client //Lag Fix -- Zippy
{
	int count, ho, mi, se, total, i, j, k, book,serial,serhash,ci;
	char nonuni[512];

	int myoffset = 13 ;
	int myincrement = 2 ;
	int myj = 12 ;

	if (newclient[s])
	{
		count=recv(client[s], (char *)buffer[s], 4, 0);
		if( ( buffer[s][0] == 0x21 ) && ( count < 4 ) ) // UOMon
		{
			total=(uiCurrentTime-starttime)/CLOCKS_PER_SEC;
			ho=total/3600;
			total-=ho*3600;
			mi=total/60;
			total-=mi*60;
			se=total;
			sprintf(uoxmonitor, "UOX3 LoginServer\r\nUOX3 Server: up for %ih %im %is\r\n",ho,mi,se);
			Network->xSend(s, uoxmonitor, strlen(uoxmonitor), 0);
			newclient[s]=0;
		} else
		{
			if( xgm && (buffer[s][0] == 0xFF ) && ( buffer[s][1] == 0xFF ) && ( buffer[s][2] == 0xFF ) && ( buffer[s][3] == 0xFF ) )
			{
				xGM[s]->isClient = 1;
				printf("Client %i is an xgm client, logging in...\n", s );
				xGM[s]->Verify(s);
				newclient[s] = 0;
				return;
			} else if( xgm )
			{
				xGM[s]->isClient = 0;
			}
			clientip[s][0]=buffer[s][0];
			clientip[s][1]=buffer[s][1];
			clientip[s][2]=buffer[s][2];
			clientip[s][3]=buffer[s][3];
			newclient[s]=0;
			if ((buffer[s][0]==0x12)&&(buffer[s][1]==0x34)&&(buffer[s][2]==0x56)&&(buffer[s][3]==0x78))
				Network->Disconnect(s);
			firstpacket[s] = true;
		}
	}
	else
	{
		recvcount=0;
		if( xgm )
		{
			if( xGM[s]->isClient )
			{
				xGM[s]->CheckMsg( s );
				return;
			}
		}
		if (Network->Receive(s, 1, 0)>0)
		{
			if( firstpacket[s] && buffer[s][0] != 0x80 && buffer[s][0] != 0x91 )
			{
				printf( "Non-ignition client attempting to cut in, disconnecting them: IP %i.%i.%i.%i\n", clientip[s][0], clientip[s][1], clientip[s][2], clientip[s][3] );
				Network->Disconnect( s );
				return;
			}
			if( buffer[s][0] !=0x73 && buffer[s][0] != 0xA4 && buffer[s][0] != 0x80 && buffer[s][0] != 0x91 )
				idleTimeout[s] = server_data.quittime * CLOCKS_PER_SEC + uiCurrentTime;
			switch( buffer[s][0] )
			{
			case 0x01:// Main Menu on the character select screen
				Network->Disconnect(s);
				break;
			case 0x80:// First Login
				firstpacket[s] = false;
				Network->Receive(s, 62, 1);
				Network->Login1(s);
				break;
			case 0xA0:// Server Select
				Network->Receive(s, 3, 1);
				Network->Relay(s);
				break;
			case 0x91:// Second Login
				firstpacket[s] = false;
				Network->Receive(s, 65, 1);
				cryptclient[s]=1;
				Network->CharList(s);
				break;
			case 0x83:// Character Delete
				Network->Receive(s, 0x27, 1);
				chardel(s);
				break;
			case 0x00:// Character Create
				Network->Receive( s, 104, 1 );
				charcreate(s);
				break;
			case 0x5D:// Character Select
				Network->Receive(s, 0x49, 1);
				charplay(s);
				break;
			case 0x02:// Walk
#if CLIENTVERSION_M==25
				Network->Receive(s, 3, 1);
#endif
#if CLIENTVERSION_M==26
				Network->Receive( s, 7, 1 );
#endif
				Movement->Walking(currchar[s], buffer[s][1], buffer[s][2]);
				breakConcentration( currchar[s], s );
				break;
			case 0x73:// Keep alive
				Network->Receive(s, 2, 1);
				Network->xSend(s, buffer[s], 2, 0);
				break;
			case 0x22:// Resync Request
				Network->Receive(s, 3, 1);
				teleport(currchar[s]);
				break;
			case 0x03:// Speech
				Network->Receive(s, 3, 0);
				Network->Receive(s, (buffer[s][1]<<8)+buffer[s][2], 1);
				breakConcentration( currchar[s], s );
				chars[currchar[s]].unicode=0;
				if(chars[currchar[s]].fx2==17)//Pricing an item //PlayerVendors
				{
					int i=str2num((char *)&buffer[s][8]);
					if(i>0)
					{
						items[chars[currchar[s]].fx1].value=i;
						sprintf(temp,"This item's price has been set to %i.",i);
					} else {
						sprintf(temp, "No price entered, this item's price has been set to %i.",items[chars[currchar[s]].fx1].value);
					}
					sysmessage(s, temp);
					sysmessage(s, "Enter a description for this item.");
					chars[currchar[s]].fx2=18;
					return;
				} else if (chars[currchar[s]].fx2==18)//Describing an item
				{
					strcpy(items[chars[currchar[s]].fx1].desc,(char *)&buffer[s][8]);
					chars[currchar[s]].fx1=chars[currchar[s]].fx2=-1;
					sprintf(temp, "This item is now described as %s,",&buffer[s][8]);
					sysmessage(s, temp);
					return;
				} else if(chars[currchar[s]].runenumb > -1)
				{
					sprintf(items[chars[currchar[s]].runenumb].name,"Rune to %s",buffer[s]+8);
					sprintf(temp,"Rune renamed to: Rune to %s",buffer[s]+8);
					sysmessage(s,temp);
					chars[currchar[s]].runenumb=-1;
					
				} else if(chars[currchar[s]].namedeed!=5) // eagle rename deed
				{
					sprintf(items[chars[currchar[s]].namedeed].name,"%s",buffer[s]+8);
          //  EviLDeD - January 29, 2000
          //  This was missing so the name itself was not getting copied. Kinda bug in side a bug
					strcpy(chars[currchar[s]].name,items[chars[currchar[s]].namedeed].name);
          //  EviLDeD - End
					sprintf(temp,"Your new name is: %s",buffer[s]+8);
					sysmessage(s,temp);
					chars[currchar[s]].namedeed=5;
					
				}				
				else if(chars[currchar[s]].keynumb!=-1) //Morrolan key rename stuff
				{
					strcpy(items[chars[currchar[s]].keynumb].name, (char *)buffer[s]+8);
					sprintf(temp,"Renamed to: %s",buffer[s]+8);
					sysmessage(s,temp);
					chars[currchar[s]].keynumb=-1;
				}       
				else if(chars[currchar[s]].pagegm==1)
				{
					int a1,a2,a3,a4;
					a1=chars[currchar[s]].ser1;
					a2=chars[currchar[s]].ser2;
					a3=chars[currchar[s]].ser3;
					a4=chars[currchar[s]].ser4;
					strcpy(gmpages[chars[currchar[s]].playercallnum].reason, (char *)buffer[s]+8);
					sprintf(temp, "GM Page from %s [%x %x %x %x]: %s",
						chars[currchar[s]].name, a1, a2, a3, a4, gmpages[chars[currchar[s]].playercallnum].reason);
					int x=0;
					for (i=0;i<now;i++)
						if (chars[currchar[i]].priv&0x20)
						{
							x=1;
							sysmessage(i, temp);
						}
						if (x==1)
						{
							sysmessage(s, "Available Game Masters have been notified of your request.");
						}
						else sysmessage(s, "There was no Game Master available to take your call.");
						chars[currchar[s]].pagegm=0;
				} 
				else if (chars[currchar[s]].pagegm==2) //Counselor page
				{
					int a1,a2,a3,a4;
					a1=chars[currchar[s]].ser1;
					a2=chars[currchar[s]].ser2;
					a3=chars[currchar[s]].ser3;
					a4=chars[currchar[s]].ser4;
					strcpy(counspages[chars[currchar[s]].playercallnum].reason, (char *)buffer[s]+8);
					sprintf(temp, "Counselor Page from %s [%x %x %x %x]: %s",
						chars[currchar[s]].name, a1, a2, a3, a4, counspages[chars[currchar[s]].playercallnum].reason);
					int x=0;
					for (i=0;i<now;i++)
						if (chars[currchar[i]].priv&0x20)
						{
							x=1;
							sysmessage(i, temp);
						}
						if (x==1)
						{
							sysmessage(s, "Available Counselors have been notified of your request.");
						}
						else sysmessage(s, "There was no Counselor available to take your call.");
						chars[currchar[s]].pagegm=0;
				}
				else
				{//Squelch
					if (chars[currchar[s]].squelched)
						sysmessage(s, "You have been squelched, Page a GM.");
					else 
						talking(s);
				}
				break;
			case 0xAD: // Unicode Speech
				Network->Receive(s, 3, 0);
				Network->Receive(s, ((buffer[s][1]<<8)+buffer[s][2]), 1);
				chars[currchar[s]].unicode=1;

				int myoffset, mysize;
				char mytempbuf[512];
			    // Check for command word versions of this packet														

				if ( (buffer[s][3]) >=0xc0 )
				{					
						                  
						buffer[s][3] = buffer[s][3] & 0x0F ; // set to normal (cutting off the ascii indicator since we are converting back to unicode)					

					    int num_words,/*idx=0,*/ num_unknown;				

						// number of distict trigger words
						num_words = ( (static_cast<int>(buffer[s][12])) << 24 ) + ( (static_cast<int>(buffer[s][13])) << 16 );
						num_words = num_words & 0xfff00000;
						num_words = (num_words >> 20);

						/*************************************/
						// plz dont delete yet
						// trigger word index in/from speech.mul, not required [yet]
						/*idx = ( (static_cast<int>(buffer[s][13])) << 24 ) + ( (static_cast<int>(buffer[s][14])) << 16);
						idx = idx & 0x0fff0000;
						idx = ( (idx << 4) >> 20) ;*/						                       
						//cout << "#keywords was " << hex << num_words << "\n" << hex << static_cast<int>(buffer[s][12]) << " " << hex << static_cast<int> (buffer[s][13]) << " " << static_cast<int> (buffer[s][14]) << " " << static_cast<int> (buffer[s][15]) << endl ;
						// cout << "idx: " << idx << endl;
						/*************************************/

						if ((num_words %2) == 1)  // odd number ?
						{
                          num_unknown = ( num_words / 2 )  * 3;
						} else
						{
                          num_unknown = ( ( num_words / 2 ) * 3 ) -1 ;
						}

						myoffset = 15 + num_unknown;					
				
						//
						//	Now adjust the buffer
						int iWord ;
						//int iTempBuf ;
						iWord = static_cast<int> ((buffer[s][1] << 8)) + static_cast<int> (buffer[s][2]) ;
						myj = 12 ;

						//cout << "Max length characters will be " << dec << (iWord - myoffset) << endl ;
						mysize = iWord - myoffset ;

						for (i=0; i < mysize ; i++)
						{
							mytempbuf[i] = buffer[s][i+myoffset] ;
						}

						for (i=0; i < mysize ; i++)
						{							
							myj++ ;
							buffer[s][myj] = mytempbuf[i] ;
							//iTempBuf = static_cast<int> (mytempbuf[i]) ;
							//cout << "Copying value of " << hex << iTempBuf << endl ;
							myj++;
							buffer[s][myj] = 0 ;
							
						}

						iWord = (((iWord - myoffset ) * 2) + 12) ;
						//cout << "Setting buffer size to " << dec << iWord << endl ;
						buffer[s][1] = static_cast<unsigned char> (((iWord &0xFF00) >>8)) ;
						buffer[s][2] = static_cast<unsigned char> (iWord & 0x00FF) ;															
				}	

				breakConcentration( currchar[s], s );
				for (i=13;i<(buffer[s][1]<<8)+buffer[s][2];i=i+2)
				{
					nonuni[(i-13)/2]=buffer[s][i];
				}
				if(chars[currchar[s]].fx2==17)//Pricing an item //PlayerVendors
				{
					int i=str2num(nonuni);
					if(i>0)
					{
						items[chars[currchar[s]].fx1].value=i;
						chars[currchar[s]].fx2=18;
						sprintf(temp,"This item's price has been set to %i.",i);
						sysmessage(s, temp);
						sysmessage(s, "Enter a description for this item.");
					} else {
						chars[currchar[s]].fx2=18;
						sprintf(temp, "No price entered, this item's price has been set to %i.",items[chars[currchar[s]].fx1].value);
						sysmessage(s, temp);
						sysmessage(s, "Enter a description for this item.");
					}
					return;
				} else if (chars[currchar[s]].fx2==18)//Describing an item
				{
					strcpy(items[chars[currchar[s]].fx1].desc,nonuni);
					chars[currchar[s]].fx1=chars[currchar[s]].fx2=-1;
					sprintf(temp, "This item is now described as %s,",nonuni);
					sysmessage(s, temp);
					return;
				} else if(chars[currchar[s]].runenumb > -1)
				{
					sprintf(items[chars[currchar[s]].runenumb].name,"Rune to %s",nonuni);
					sprintf(temp,"Rune renamed to: Rune to %s",nonuni);
					sysmessage(s,temp);
					chars[currchar[s]].runenumb=-1;
				} else if(chars[currchar[s]].namedeed!=5) // eagles namedeed stuff
				{
					sprintf(items[chars[currchar[s]].namedeed].name,"%s",nonuni);
					strcpy(chars[currchar[s]].name,items[chars[currchar[s]].namedeed].name);
					sprintf(temp,"Your New Name is: %s",nonuni);
					sysmessage(s,temp);
					chars[currchar[s]].namedeed=5;
				} 
				else if(chars[currchar[s]].keynumb!=-1) //Morrolan key rename stuff
                {
					strcpy(items[chars[currchar[s]].keynumb].name, nonuni);
					sprintf(temp,"Renamed to: %s",nonuni);
					sysmessage(s,temp);
					chars[currchar[s]].keynumb=-1;
                }       
				else if(chars[currchar[s]].pagegm==1)
				{
					int a1,a2,a3,a4;
					a1=chars[currchar[s]].ser1;
					a2=chars[currchar[s]].ser2;
					a3=chars[currchar[s]].ser3;
					a4=chars[currchar[s]].ser4;
					strcpy(gmpages[chars[currchar[s]].playercallnum].reason, nonuni);
					sprintf(temp, "GM Page from %s [%x %x %x %x]: %s",
						chars[currchar[s]].name, a1, a2, a3, a4, gmpages[chars[currchar[s]].playercallnum].reason);
					int x=0;
					for (i=0;i<now;i++)
						if (chars[currchar[i]].priv&0x20)
						{
							x=1;
							sysmessage(i, temp);
						}
						if (x==1)
						{
							sysmessage(s, "Available Game Masters have been notified of your request.");
						}
						else sysmessage(s, "There was no Game Master available to take your call.");
						chars[currchar[s]].pagegm=0;
				} else	if (chars[currchar[s]].pagegm==2) //Counselor page
				{
					int a1,a2,a3,a4;
					a1=chars[currchar[s]].ser1;
					a2=chars[currchar[s]].ser2;
					a3=chars[currchar[s]].ser3;
					a4=chars[currchar[s]].ser4;
					strcpy(counspages[chars[currchar[s]].playercallnum].reason, nonuni);
					sprintf(temp, "Counselor Page from %s [%x %x %x %x]: %s",
						chars[currchar[s]].name, a1, a2, a3, a4, counspages[chars[currchar[s]].playercallnum].reason);
					int x=0;
					for (i=0;i<now;i++)
						if (chars[currchar[i]].priv&0x20)
						{
							x=1;
							sysmessage(i, temp);
						}
						if (x==1)
						{
							sysmessage(s, "Available Counselors have been notified of your request.");
						}
						else sysmessage(s, "There was no Counselor available to take your call.");
						chars[currchar[s]].pagegm=0;
				} else
				{//Squelch
					if (chars[currchar[s]].squelched)
						sysmessage(s, "You have been squelched.");
					else unicodetalking(s);
				}
				break;    
			case 0x06:// Doubleclick
				Network->Receive(s, 5, 1);
				doubleclick(s);
				breakConcentration( currchar[s], s );
				break;
			case 0x09:// Singleclick
				Network->Receive(s, 5, 1);
				singleclick(s);
//				breakConcentration( currchar[s], s );
				break;
			case 0x6C:// Targeting
				Network->Receive(s, 19, 1);
				if(targetok[s]) Targ->MultiTarget(s);
				breakConcentration( currchar[s], s );
				break;
			case 0x13:// Equip Item
				Network->Receive(s, 10, 1);
				wear_item(s);
				breakConcentration( currchar[s], s );
				break;
			case 0x07:// Get Item
				Network->Receive(s, 7, 1);
				get_item(s);
				breakConcentration( currchar[s], s );
				break;
			case 0x08:// Drop Item
				Network->Receive(s, 14, 1);
				if ( (buffer[s][10]>=0x40) && (buffer[s][10]!=0xff) )
					pack_item(s);
				else 
					dump_item(s);
				breakConcentration( currchar[s], s );
				break;
			case 0x72:// Combat Mode
				Network->Receive(s, 5, 1);
				chars[currchar[s]].war=buffer[s][1];
				chars[currchar[s]].targ=-1;
				chars[currchar[s]].timeout = (SI32)(uiCurrentTime + CLOCKS_PER_SEC);
				Network->xSend(s, buffer[s], 5, 0);
				Movement->CombatWalk(currchar[s]);
				dosocketmidi(s);
				breakConcentration( currchar[s], s );
				break;
			case 0x12:// Ext. Command
				Network->Receive(s, 3, 0);
				Network->Receive(s, (buffer[s][1]<<8)+buffer[s][2], 1);
				breakConcentration( currchar[s], s );
				if( buffer[s][3] == 0xC7 ) // Action
				{
					if( chars[currchar[s]].onhorse ) // Ripper
						return; // can't bow or salute on horse
					if (!(strcmp((char *)&buffer[s][4],"bow"))) action(s, 0x20);
					if (!(strcmp((char *)&buffer[s][4],"salute"))) action(s, 0x21);
					break; // Morrolan 
				} 
				else if( buffer[s][3] == 0x58 )  //Zippy 11/10/99 For Door Macro use :o)
				{
					DoorMacro(s);
					break;
				}
				else if( buffer[s][3]== 0x24 ) // Skill
				{
					i=4;
                    // printf("skill selected ???\n");
					while (buffer[s][i]!=' ') i++;
					buffer[s][i]=0;
					// printf("before skilluse\n");
					Skills->SkillUse(s, str2num((char *)&buffer[s][4]));
					// printf("after skilluse\n");
					break;
				} 
				else if( buffer[s][3] == 0x27 || buffer[s][3] == 0x56 )  // Spell
				{
					j=0;
					k=packitem(currchar[s]);
					if (k!=-1) //lb
					{
						serial=items[k].serial;
						serhash=serial%HASHMAX;
						for (i=0;i<contsp[serhash].max;i++)
						{
							ci=contsp[serhash].pointer[i];
							if (ci!=-1) //lb
								if ((items[ci].contserial==serial) && (items[ci].type==9))
								{
									j=ci;
									break;
								}
						}
					}
					if (j==0)
					{
						serial=chars[currchar[s]].serial;
						serhash=serial%HASHMAX;
						for (i=0;i<contsp[serhash].max;i++)
						{
							ci=contsp[serhash].pointer[i];
							if (ci!=-1) //lb
								if ((items[ci].contserial==serial) && (items[ci].layer==1)) 
								{
									j=ci;
								}
						}
					}
					if (j!=0)
					{
						book=buffer[s][4]-0x30;
						if (buffer[s][5]>0x20) 
						{
							book=(book*10)+(buffer[s][5]-0x30);
						}
					}
					if (j!=0 && Magic->CheckBook(((book-1)/8)+1, (book-1)%8, j)) 
						if (chars[currchar[s]].priv2&2) // REAL cant cast while frozen bugfix, lord binary
						{
							if (chars[currchar[s]].casting)
								sysmessage(s, "You are already casting a spell.");
							else
								sysmessage(s, "You cannot cast spells while frozen.");
						} 
						//						else Magic->CastSpell(s, book, 0, 0);
						else 
						{
							currentSpellType[s] = 0;
							Magic->newSelectSpell2Cast( s, book );
						}
						else sysmessage(s, "You don't have that spell.");
				} 
				else 
				{
					if( buffer[s][2] == 0x05 && buffer[s][3] == 0x43 )  // Open spell book
					{
						Magic->SpellBook(s);
					}
					break;
				}
				break; // Lord Binary !!!!
			case 0x9B:// GM Page
				Network->Receive(s, 0x102, 1);
				gmmenu(s, 1);
				break;
			case 0x7D:// Choice
				Network->Receive(s, 13, 1);
				choice(s);
				break;
			case 0x95:// Color Select
				Network->Receive(s, 9, 1);
				Commands->DyeItem(s);
				break;
			case 0x34:// Status Request
				Network->Receive(s, 10, 1);
				srequest(s);
				break;
			case 0x75:// Rename Character //Lag Fix -- Zippy //Bug Fix -- Zippy
				Network->Receive(s, 0x23, 1);
				serial=calcserial(buffer[s][1],buffer[s][2],buffer[s][3],buffer[s][4]);
				if( serial == -1 ) return;
				i=findbyserial(&charsp[serial%HASHMAX],serial,1);
				if(i!=-1)
					strncpy(chars[i].name, (char *)&buffer[s][5], 50);
				
				break;
			case 0x66:// Read Book
				int size;
				Network->Receive(s, 3, 0);
				size=(buffer[s][1]<<8)+buffer[s][2];
				Network->Receive(s, size, 1);
				serial=calcserial(buffer[s][3],buffer[s][4],buffer[s][5],buffer[s][6]);
				i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
				if (i!=-1)
				{		
					//printf("b7: %i b8: %i b9: %i b10: %i b11: %i b12: %i\n",buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10],buffer[s][11],buffer[s][12]);
					if (items[i].morex!=666 && items[i].morex!=999) 
						Books->readbook_readonly_old(s, i, (buffer[s][9]<<8)+buffer[s][10]);  // call old books read-method
					if (items[i].morex==666) // writeable book -> copy page data send by client to the class-page buffer
					{
						for (j=13;j<=size;j++) // copy (written) page data in class-page buffer
						{
							Books->pagebuffer[s][j-13]=buffer[s][j];
						}
						Books->readbook_writeable(s,i,(buffer[s][9]<<8)+buffer[s][10],(buffer[s][11]<<8)+buffer[s][12] ); 
					}
					if (items[i].morex==999) 
						Books->readbook_readonly(s,i,(buffer[s][9]<<8)+buffer[s][10]); // new books readonly
				}
				break;

            // client sends them out if the titel and/or author gets changed on a writable book
		    // its NOT send (anymore?) when a book is closed as many packet docus state.
			// LB 7-dec 1999
			case 0x93:

				int serial,i,j;
				char author[31],title[61],ch;

				Network->Receive(s, 99, 0);
				serial=calcserial(buffer[s][1],buffer[s][2],buffer[s][3],buffer[s][4]);
				i=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
				if (i==-1) return;				
				Books->a_t=1;			

				j=9;ch=1;
				while(ch!=0)
				{
					ch=buffer[s][j];
					title[j-9]=ch;
					Books->titlebuffer[s][j-9]=ch;
					j++;
					if (j>69) 
						ch=0;
				}


				j=69;ch=1;
				while(ch!=0)
				{
					ch=buffer[s][j];
					author[j-69]=ch;
					Books->authorbuffer[s][j-69]=ch;
					j++;
					if (j>99) 
						ch=0;
				}
							
				break;

			case 0xA7:// Get Tip
				Network->Receive(s, 4, 1);
				tips(s, (buffer[s][1]<<8)+buffer[s][2]+1);
				break;
			case 0xA4:// Spy
				Network->Receive(s, 0x95, 1);
				break;
			case 0x05:// Attack
				Network->Receive(s, 5, 1);
				breakConcentration( currchar[s], s );
				PlayerAttack( s );
				break;
			case 0xB1:// Gumpmenu choice
				Network->Receive(s, 3, 0);
				Network->Receive(s, (buffer[s][1]<<8)+buffer[s][2], 1);
				Gumps->Button(s, (buffer[s][13]<<8)+buffer[s][14], buffer[s][3], buffer[s][4], buffer[s][5], buffer[s][6], buffer[s][10]);
				break;
			case 0xAC:// Textentry input
				Network->Receive(s, 3, 0);
				Network->Receive(s, (buffer[s][1]<<8)+buffer[s][2], 1);
				Gumps->Input(s);
				break;
			case 0x2C:// Resurrect menu choice
				Network->Receive(s, 2, 0);
				if(buffer[s][1]==0x02) sysmessage(s, "You are now a ghost.");
				if(buffer[s][1]==0x01) sysmessage(s, "The connection between your spirit and the world is too weak.");
				break;
			case 0x3B:// Buy from vendor...
				Network->Receive(s, 3, 0);
				Network->Receive(s, (buffer[s][1]<<8)+buffer[s][2], 1);
				buyaction(s);
				break;
			case 0x9F:// Sell to vendor...
				Network->Receive(s, 3, 0);
				Network->Receive(s, (buffer[s][1]<<8)+buffer[s][2], 1);
				sellaction(s);
				break;
			case 0x69:// Client text change
				Network->Receive(s, 3, 0);// What a STUPID message...  It would be useful if
				Network->Receive(s, (buffer[s][1]<<8)+buffer[s][2], 1);// it included the color changed to, but it doesn't!
				break;
			case 0x6F:// Secure Trading message
				Network->Receive(s, 3, 0);
				Network->Receive(s, (buffer[s][1]<<8)+buffer[s][2], 1);
				trademsg(s);
				break;
			case 0xB6:// T2A Popuphelp request
				Network->Receive(s, 9, 0);
				break;
			case 0xB8:// T2A Profile request
				Network->Receive(s, 8, 0);
				break;
				// Dupois
			case 0x71:// Message Board Item
				Network->Receive(s, 3, 0);
				Network->Receive(s, (buffer[s][1]<<8)+buffer[s][2], 1);
				MsgBoardEvent(s);
				break;
				// Dupois - End
				
#if CLIENTVERSION_M==26
			case 0xbb: // No idea
                Network->Receive(s, 9, 1);
				//printf("New packet [0xbb] ignored\n");
                break;
            case 0xbd: // No idea
                Network->Receive(s, 3, 0);
                Network->Receive(s, (buffer[s][1]<<8)+buffer[s][2], 1);
				//printf("New packet [0xbd] ignored\n");
                break;
			case 0x3A:	// skills management packet
						/*				struct // size = ??? = Get a skill lock (or multiple)
						{
						BYTE m_Cmd;		// 0= 0x3A
						NWORD m_len;	// 1= varies
						struct
						{
						NWORD m_index;	// skill index
						BYTE m_lock;	// current lock mode (0 = none (up), 1 = down, 2 = locked)
						} skills[1];
				} Skill;*/
				Receive( s, 6, 1 );	// it's always 6 so Westy tells me... let's not be intelligent =)
				int skillNum;
				skillNum = (buffer[s][3]<<8) + buffer[s][4];
				chars[currchar[s]].lockState[skillNum] = buffer[s][5];
				break;

			case 0xBF:	// some joyous new packet!! Thank you OSI (Client 2.0)
				Network->Receive( s, 3, 0 );
				Network->Receive( s, (buffer[s][1]<<8) + buffer[s][2], 1 );
				break;
				
#endif
				
			default:
				FD_ZERO(&all);
				FD_SET(client[s],&all);
				nfds=client[s]+1;
				if (select(nfds, &all, NULL, NULL, &uoxtimeout)>0)
				{
					Network->Receive(s, 2560, 1);
				}
				printf("Swallowing unknown message: %x [%c]\n", buffer[s][0], buffer[s][0] );
				break;
    }
  }
  else	// Sortta error type thing, they disconnected already
  {
	  Network->Disconnect(s);
  }
 }
}

void start_glow( void )	// better to make an extra function cause in loaditem it could be the case that the 
// glower is loaded before the pack
{
	int i, j, k, l;
	for( i=0; i < itemcount; i++ )
	{
		if( items[i].glow > 0 && items[i].free == 0 )
		{
			if( items[i].contserial != -1 )
			{
				j = findbyserial( &itemsp[items[i].contserial%HASHMAX], items[i].contserial, 0 ); // find glowing item in backpack
				l = findbyserial( &charsp[items[i].contserial%HASHMAX], items[i].contserial, 1 ); // find equipped glowing items
				//printf("j: %i l: %i\n", j, l );
				if( l == -1 ) 
					k = GetPackOwner( j );
				else
					k = l;
				if( k != -1 )
				{
					setptr( &glowsp[chars[k].serial%HASHMAX], i );
					Items->GlowItem( k, i );
				}
			}
		}
	}
}


//int realmain(int argc, char *argv[])
int __cdecl main(int argc, char *argv[])
{
	int i;
	
	unsigned int uiNextCheckConn=0;
	UI32 tempSecs;
	UI32 tempMilli;
	UI32 loopSecs;
	UI32 loopMilli;
	UI32 tempTime;
	
	Loaded=ErrorCount=0;
	
#ifdef _CRASH_PROTECT_
	try {//Crappy error trapping....	// now it will never crash on the correct line
#endif
		uiCurrentTime = getclock();
		
		printf("Starting UOX3...\n");
		
		openings=0;
#ifdef __NT__
		constart();
		sprintf(temp, "%s v%s Build %s", PRODUCT, VER, BUILD );
		SetConsoleTitle(temp);
		//clearscreen();
#else
		signal(SIGPIPE, SIG_IGN); // This appears when we try to write to a broken network connection
		signal(SIGTERM, &endmessage);
		signal(SIGQUIT, &endmessage);
		signal(SIGINT, &endmessage); 
		signal(SIGILL, &illinst);
		signal(SIGFPE,&aus);
		
#endif
		
		// change from global variables to global pointers
		// crackerjack jul 31/99 - gives more control over
		// what order the compiler calls all the constructors
		// in :)
		printf("Initializing classes...");
		
		cwmWorldState = NULL;
		mapRegions = NULL;
		Admin = NULL;
		Boats = NULL;
		Combat = NULL;
		Commands = NULL;
		Guilds = NULL;
		Gumps = NULL;
		Items = NULL;
		Map = NULL;
		Npcs = NULL;
		Skills = NULL;
		Towns = NULL;
		Weight = NULL;
		Targ = NULL;
		Network = NULL;
		Magic = NULL;
		Races = NULL;
		Weather = NULL;
		Movement = NULL;
		Effects = NULL;
		WhoList = NULL;
		OffList = NULL;
		// MAKE SURE IF YOU ADD A NEW ALLOCATION HERE THAT YOU FREE IT UP IN Shutdown(...)
		if(( cwmWorldState = new CWorldMain ) == NULL ) Shutdown( FATAL_UOX3_ALLOC_WORLDSTATE );
		if(( mapRegions = new cRegion )       == NULL ) Shutdown( FATAL_UOX3_ALLOC_MAPREGIONS );
		if(( Admin = new cAdmin )             == NULL ) Shutdown( FATAL_UOX3_ALLOC_ADMIN );
		if(( Boats = new cBoat )              == NULL ) Shutdown( FATAL_UOX3_ALLOC_BOATS );
		if(( Combat = new cCombat )           == NULL ) Shutdown( FATAL_UOX3_ALLOC_COMBAT );
		if(( Commands = new cCommands )       == NULL ) Shutdown( FATAL_UOX3_ALLOC_COMMANDS );
		if(( Guilds = new cGuilds )           == NULL ) Shutdown( FATAL_UOX3_ALLOC_GUILDS );
		if(( Gumps = new cGump )              == NULL ) Shutdown( FATAL_UOX3_ALLOC_GUMPS );
		if(( Items = new cItem )              == NULL ) Shutdown( FATAL_UOX3_ALLOC_ITEMS );
		if(( Map = new cMapStuff )            == NULL ) Shutdown( FATAL_UOX3_ALLOC_MAP );
		if(( Npcs = new cCharStuff )          == NULL ) Shutdown( FATAL_UOX3_ALLOC_NPCS );
		if(( Skills = new cSkills )           == NULL ) Shutdown( FATAL_UOX3_ALLOC_SKILLS );
		if(( Towns = new cTownStones )        == NULL ) Shutdown( FATAL_UOX3_ALLOC_TOWNS );
		if(( Weight = new cWeight )           == NULL ) Shutdown( FATAL_UOX3_ALLOC_WEIGHT );
		if(( Targ = new cTargets )            == NULL ) Shutdown( FATAL_UOX3_ALLOC_TARG );
		if(( Network = new cNetworkStuff )    == NULL ) Shutdown( FATAL_UOX3_ALLOC_NETWORK );
		if(( Magic = new cMagic )             == NULL ) Shutdown( FATAL_UOX3_ALLOC_MAGIC );
		if(( Races = new cRaces )             == NULL ) Shutdown( FATAL_UOX3_ALLOC_RACES );
		if(( Weather = new cWeatherAb )       == NULL ) Shutdown( FATAL_UOX3_ALLOC_WEATHER );
		//Weather=new cWeather; //Zippy-Still to come, Phat Weather Sys. :o)
		if(( Movement = new cMovement )       == NULL ) Shutdown( FATAL_UOX3_ALLOC_MOVE );
		if(( Effects = new cTEffect )         == NULL ) Shutdown( FATAL_UOX3_ALLOC_EFFECTS );	// addition of TEffect class, memory reduction (Abaddon, 17th Feb 2000)
		if(( WhoList = new cWhoList )         == NULL ) Shutdown( FATAL_UOX3_ALLOC_WHOLIST );	// wholist
		if(( OffList = new cWhoList( false ) )== NULL ) Shutdown( FATAL_UOX3_ALLOC_WHOLIST );	// offlist
		if(( Books = new cBooks )             == NULL ) Shutdown( FATAL_UOX3_ALLOC_EFFECTS );	// writeable books
		Map->Cache = 0;	
		printf(" Done\n");

		printf("Done.\n");
		
		printf("Initalizing script pointers...\n");
		for(i=0;i<NUM_SCRIPTS;i++) 
		{
			if (i!=custom_npc_script && i!=custom_item_script)//Don't iniatlize these till we know the file names ;-)
				i_scripts[i]=new Script(n_scripts[i]);
		}
		printf("Done.\n");
		
		//Now lets load the custom scripts, if they have them defined...
		i=0;
		openscript("npc.scp");
		do {
			read2();
			if (!(strcmp(script1, "CUSTOM_NPC_SCRIPT"))) strcpy(n_scripts[custom_npc_script], script2);
			i++;
		} while (n_scripts[custom_npc_script][0]==0 && i<10);
		closescript();
		if (n_scripts[custom_npc_script][0]!=0)
			i_scripts[custom_npc_script]=new Script(n_scripts[custom_npc_script]);
		
		i=0;
		openscript("items.scp");
		do {
			read2();
			if (!(strcmp(script1, "CUSTOM_ITEM_SCRIPT"))) strcpy(n_scripts[custom_item_script], script2);
			i++;
		} while (n_scripts[custom_item_script][0]==0 && i<10);
		closescript();
		if (n_scripts[custom_item_script][0]!=0)
			i_scripts[custom_item_script]=new Script(n_scripts[custom_item_script]);
		//Done with custom scripts.
		
		//LoadNameMenus(); //Coming Soon - Name Menus
		
		for (i=1;i<argc;i++)
		{
			if (!(strcmp(argv[i],"#")))
			{
				printf("\nUOX3 Menu List Generator Module\n\n");
				printf("Creating UOXMENUS.LST...\n");
				lstfile=fopen("UOXMENUS.LST","w");
				if (lstfile==NULL)
				{
					printf("ERROR: UOXMENUS.LST could not be created.\n");
					//				error=1;
					//				keeprun=0;
					Shutdown( FATAL_UOX3_CREATE_UOXMENUS );
				}
				fprintf(lstfile, "UOX3 Script Menu List\n\n");
				fprintf(lstfile, "This file is intended to allow you to do a quick lookup for certain items\n");
				fprintf(lstfile, "or menus that you might be searching.\n\n");
				fprintf(lstfile, "Maximum used numbers: (Always use numbers higher than those)\n");
				scriptmax("GMMENU");
				scriptmax("BATCH");
				scriptmax("NPC");
				scriptmax("SPEECH");
				scriptmax("ITEMMENU");
				scriptmax("ITEM");
				scriptmax("LOCATION");
				fprintf(lstfile, "\nShort List: (Menus only)\n");
				fprintf(lstfile, "GM Master Item Menu (ITEMMENU 1)\n");
				printf( "Generating Short Item List......");
				scriptlist(1, 1, 0);
				printf( "Done!\n" );
				menuList.resize( 0 );
				fprintf(lstfile, "\nLong List: (Menus and Items)\n");
				fprintf(lstfile, "GM Master Item Menu (ITEMMENU 1)\n");
				printf( "Generating Long Item List......");
				scriptlist(1, 1, 1);
				printf( "Done!\n" );
				fprintf(lstfile, "\nUOX3 Copyright 1999 by UOX Team\n");
				fclose(lstfile);
				lstfile = NULL;
				printf("List creation complete!\n");
				Shutdown( 0 );
			} 
			else if (!(strcmp( argv[i], "-xgm" ))) 
			{
				printf("XGM Enabled! Initializing... ");
				xgm = 1;
				for( i=0; i < MAXCLIENT; i++ )
				{
					xGM[i] = new cRemote;
					xGM[i]->isClient = 0;
				}
				printf("Done.\n" );
			}
			else if (!(strcmp(argv[i], "-ERROR")))
			{
				ErrorCount = str2num(argv[i+1]);
				i++;
			}
#ifdef __CLUOX__
			else if( !strcmp( argv[i], "-cluox100" ) )
			{
				printf( "Using CLUOX Streaming-IO\n" );
				setvbuf( stdout, NULL, _IONBF, 0 );
				setvbuf( stderr, NULL, _IONBF, 0 );
				cluox_io = true;
				i++;
				if( i > argc )
				{
					fprintf( stderr, "Fatal error in CLUOX arguments\n" );
					Shutdown( 10 );
				}
				char *dummy;
				cluox_stdin_writeback = (void *)strtol( argv[i], &dummy, 16 );
			}
#endif
		}
		
		printf("Initializing global variables...");
		save_counter = 0;
		for( i = 0; i < (MAXCLIENT); i++ )
		{
			LSD[i] = 0;
			firstpacket[i] = false;
		}
		imem=0;
		cmem=0;
		uoxtimeout.tv_sec=0;
		uoxtimeout.tv_usec=0;
		keeprun=1;
		error=0;
		now=0;
		secure=1;
		charcount=0;
		itemcount=0;
		charcount2=1;
		itemcount2=0x40000000;
		donpcupdate=0;
		wtype=0;
		cmemover=0;
		cmemcheck=-1;
		imemover=0;
		imemcheck=-1;
		xcounter=0;
		ycounter=0;
		stablockcachei=0;
		stablockcachehit=0;
		stablockcachemiss=0;
		globallight=0;
		executebatch=0;
		showlayer=0;
		autosaved = 0;
		
		//------------ Set Creator Variables by Magius(CHE) ----------
		strcpy( skill[ALCHEMY].madeword, "mixed" );
		strcpy( skill[ANATOMY].madeword, "made" );
		strcpy( skill[ANIMALLORE].madeword, "made" );
		strcpy( skill[ITEMID].madeword, "made" );
		strcpy( skill[ARMSLORE].madeword, "made" );
		strcpy( skill[PARRYING].madeword, "made" );
		strcpy( skill[BEGGING].madeword, "made" );
		strcpy( skill[BLACKSMITHING].madeword, "forged" );
		strcpy( skill[BOWCRAFT].madeword, "bowcrafted" );
		strcpy( skill[PEACEMAKING].madeword, "made" );
		strcpy( skill[CAMPING].madeword, "made" );
		strcpy( skill[CARPENTRY].madeword, "made" );
		strcpy( skill[CARTOGRAPHY].madeword, "wrote" );
		strcpy( skill[COOKING].madeword, "cooked" );
		strcpy( skill[DETECTINGHIDDEN].madeword, "made" );
		strcpy( skill[ENTICEMENT].madeword, "made" );
		strcpy( skill[EVALUATINGINTEL].madeword, "made" );
		strcpy( skill[HEALING].madeword, "made" );
		strcpy( skill[FISHING].madeword, "made" );
		strcpy( skill[FORENSICS].madeword, "made" );
		strcpy( skill[HERDING].madeword, "made" );
		strcpy( skill[HIDING].madeword, "made" );
		strcpy( skill[PROVOCATION].madeword, "made" );
		strcpy( skill[INSCRIPTION].madeword, "wrote" );
		strcpy( skill[LOCKPICKING].madeword, "made" );
		strcpy( skill[MAGERY].madeword, "envoked" );
		strcpy( skill[MAGICRESISTANCE].madeword, "made" );
		strcpy( skill[TACTICS].madeword, "made" );
		strcpy( skill[SNOOPING].madeword, "made" );
		strcpy( skill[MUSICIANSHIP].madeword, "made" );
		strcpy( skill[POISONING].madeword, "made" );
		strcpy( skill[ARCHERY].madeword, "made" );
		strcpy( skill[SPIRITSPEAK].madeword, "made" );
		strcpy( skill[STEALING].madeword, "made" );
		strcpy( skill[TAILORING].madeword, "sewn" );
		strcpy( skill[TAMING].madeword, "made" );
		strcpy( skill[TASTEID].madeword, "made" );
		strcpy( skill[TINKERING].madeword, "made" );
		strcpy( skill[TRACKING].madeword, "made" );
		strcpy( skill[VETERINARY].madeword, "made" );
		strcpy( skill[SWORDSMANSHIP].madeword, "made" );
		strcpy( skill[MACEFIGHTING].madeword, "made" );
		strcpy( skill[FENCING].madeword, "made" );
		strcpy( skill[WRESTLING].madeword, "made" );
		strcpy( skill[LUMBERJACKING].madeword, "made" );
		strcpy( skill[MINING].madeword, "smelted" );
		strcpy( skill[MEDITATION].madeword, "envoked" );
		strcpy( skill[STEALTH].madeword, "made" );
		strcpy( skill[REMOVETRAPS].madeword, "made" );
		// ------------ End Variable Creator by Magius(CHE) -----------
		printf(" Done.\nLoading skills...");
		loadskills(); // moved by LB, important bugfix ...
		// chars skills got loaded before the skills script was loaded
		// thus baseskills were initially equal to skills with stat modifiers on startup till skills were used
		
		for (i=0;i<MAXACCT;i++)
		{
			acctinuse[i]=0;
			inworld[i]=-1;//Instalog
		}
		for (i=0;i<STABLOCKCACHESIZE;i++) stablockcachex[i]=-1;
		for (i=0;i<MAXLAYERS;i++) layers[i]=0;
		for (i=0;i<MAXCLIENT;i++) noweather[i]=1; // players dont see any rain or snow till they move
		printf(" Done\n");
		// Tauriel item pointer lookups 12-3-09
		// This allocates arrays like itemsp[].pointer[] for setting = items[] #
		// itemsp[].max is current maximum for this serial%256 block
		printf("Building pointer arrays...");
		for (i=0;i<HASHMAX;i++)
		{
			// intialize to NULL so in case of error we can delete them safely
			itemsp[i].pointer = NULL;
			charsp[i].pointer = NULL;
			cownsp[i].pointer = NULL;
			spawnsp[i].pointer = NULL;
			//contsp[i].pointer = NULL;
			cspawnsp[i].pointer = NULL;
			ownsp[i].pointer = NULL;
			imultisp[i].pointer = NULL;
			cmultisp[i].pointer = NULL;
			glowsp[i].pointer = NULL;
			
			if(( itemsp[i].pointer = new int[25]) == NULL) Shutdown( FATAL_UOX3_ALLOC_ITEMSP );
			if(( charsp[i].pointer = new int[25]) == NULL) Shutdown( FATAL_UOX3_ALLOC_CHARSP );
			if(( cownsp[i].pointer = new int[25]) == NULL) Shutdown( FATAL_UOX3_ALLOC_COWNSP );
			if(( spawnsp[i].pointer = new int[25]) == NULL) Shutdown( FATAL_UOX3_ALLOC_SPAWNSP );
			if(( contsp[i].pointer = new int[25]) == NULL) Shutdown( FATAL_UOX3_ALLOC_CONTSP );
			if(( cspawnsp[i].pointer = new int[25]) == NULL) Shutdown( FATAL_UOX3_ALLOC_CSPAWNSP );
			if(( ownsp[i].pointer = new int[25]) == NULL) Shutdown( FATAL_UOX3_ALLOC_OWNSP );
			if(( imultisp[i].pointer = new int[25]) == NULL) Shutdown( FATAL_UOX3_ALLOC_IMULTISP );
			if(( cmultisp[i].pointer = new int[25]) == NULL) Shutdown( FATAL_UOX3_ALLOC_CMULTISP );
			if(( glowsp[i].pointer = new int[25]) == NULL) Shutdown( FATAL_UOX3_ALLOC_GLOWSP );
			
			// with 25 starting value should give about 6,150 (256*25) items initially.
			// and it will dynamically reallocate in 50 unit increments after that
			// initialize them to -1 so we know where free ones are
			itemsp[i].max=ownsp[i].max=spawnsp[i].max=contsp[i].max=cownsp[i].max=cspawnsp[i].max=charsp[i].max=imultisp[i].max=cmultisp[i].max=glowsp[i].max=25;
			for (int j=0;j<25;j++) itemsp[i].pointer[j]=ownsp[i].pointer[j]=cownsp[i].pointer[j]=spawnsp[i].pointer[j]=contsp[i].pointer[j]=cspawnsp[i].pointer[j]=charsp[i].pointer[j]= imultisp[i].pointer[j]=cmultisp[i].pointer[j]=glowsp[i].pointer[j]=-1;
		}
		
		
		printf(" Done. \n");
		Admin->ReadIni();
		
		keeprun=Network->kr; //LB. for some technical reasons global varaibles CANT be changed in constructors in c++.
		error=Network->faul;  // i hope i can find a cleaner solution for that, but this works !!!
		// has to here and not at the cal cause it would get overriten later
		
		printf("Loading teleport..."); fflush(stdout);
		read_in_teleport();
		printf(" Done.\n");
		
		srand(uiCurrentTime); // initial randomization call
		printf("Loading vital scripts...\n");
		loadserverdefaults();
		loadserverscript();	// attempt to reload for the caching
		
		// moved all the map loading into cMapStuff - fur
		Map->Load();
		
		//sockinit();
		//Guilds->Init();
		loadspawnregions();
		loadregions();
		printf("Initializing new Sosaria Magery System... ");
		Magic->LoadScript();
		Races->load();
		Weather->load();
		Weather->newDay();
		Commands->Load();
		loadnewworld();
		
		// sectioning(); // For sectioning Items and chars arrays
		printf("Clearing all trades...");
		Loaded = 1;
		
		clearalltrades();
		printf(" Done.\n");
		
		//Boats --Check the multi status of every item character at start up to get them set!
		printf("Initializing multis...");
		int multi;
		
		for (i=0;i<charcount;i++)
		{
			if (!chars[i].free)
			{
				multi=findmulti(chars[i].x,chars[i].y,chars[i].z);
				if( multi != -1 )
					setserial(i,multi,8);
				else
					chars[i].multis=-1;
			}
		}
		for (i=0;i<itemcount;i++)
		{
//			if (!items[i].free && items[i].contserial!=-1)
			if( !items[i].free && items[i].contserial == -1 )
			{
				multi = findmulti( items[i].x, items[i].y, items[i].z );
				if (multi!=-1)
				{
					if( multi != i ) 
						setserial( i, multi, 7 );
					else 
						items[i].multis = -1;
				}
				else
					items[i].multis = -1;
			}
		}
		printf(" Done.\n");
		//End Boats --^
		
		printf("Loading IM Menus...");
		im_loadmenus( "inscribe.gmp", TellScroll ); //loading gump for inscribe()
		printf(" Done.\n");
		
		starttime=uiCurrentTime;
		gcollect();
		printf( "Initializing glowing-items..." );
		start_glow();
		FD_ZERO(&conn);
		endtime=0;
		lclock=0;
		printf("Initializing Que System...");
		initque(); // Initialize gmpages[] array
		printf(" Done.\nLoading custom titles...");
		loadcustomtitle();
		printf(" Done.\n");
		//   EviLDeD  -  Make sure to set the WorldSave announce value here
		//   December 27, 1998
		cwmWorldState->announce(server_data.announceworldsaves);
		//   EviLDeD  -  End
		//AntiChrist merging codes
		
		/*if(server_data.lordblagfix==6476)
		printf("Lord Binary lag fix enabled.\n"); 
		else printf("Lord Binary lag fix disabled.\n");
		
		printf("nice-value: %i\n",speed.nice);*/
		
		printf("Initialising sounds... ");
		init_creatures(); //lb, initilises the creatures array (with soudfiles and other creatures infos)
		printf("Done.\n");
		
		Admin->LoadWipe();
		
#ifdef __NT__
		//	EviLDeD	-	February 10, 2000
		//	I am taking this out for good
		//clearscreen(); // Moved by Magius(CHE (1) and again by fur for linux
		//	EviLDeD	-	End
		//	sprintf(idname, "%s Version %s Alpha [WIN32] by %s <%s>", PRODUCT, VER, NAME, EMAIL);
		sprintf( idname, "%s %s(Build:%s) [WIN32] compiled by %s\nProgrammed by: %s", PRODUCT, VER, BUILD, NAME, PROGRAMMERS );
#else
		//	sprintf(idname, "Ultima Offline eXperiment 3 Server Version %s Alpha [LINUX] by %s <%s>", VER, NAME, EMAIL);
		sprintf( idname, "%s %s(Build:%s) [LINUX] compiled by %s\nProgrammed by: %s", PRODUCT, VER, BUILD, NAME, PROGRAMMERS );
#endif
		//	printf("\n%s V%s Alpha", PRODUCT, VER);
#ifdef __NT__
		printf(" for Win32");
		//clearscreen();
#else
		printf(" for Linux");
#endif
		printf("\n");
		printf("(Configured for connections by UO Client version 1.%i.%i%s)\n\n", CLIENTVERSION_M, CLIENTVERSION, CLIENT_SUB);
		printf("Copyright (C) 1997, 98 Marcus Rating (Cironian)\n\n");
		printf("This program is free software; you can redistribute it and/or modify\n");
		printf("it under the terms of the GNU General Public License as published by\n");
		printf("the Free Software Foundation; either version 2 of the License, or\n");
		printf("(at your option) any later version.\n\n");
		//printf("%s version %s\n", PRODUCT, VER);
		printf("%s version %s(Build:%s)\n", PRODUCT, VER, BUILD);
		printf("Compiled on %s (%s %s)\n",__DATE__,__TIME__,TIMEZONE);
		printf("Compiled by %s\n",NAME);
		printf("Contact: %s\n", EMAIL);
		//if (sizeof(tile_st)!=37)
		//printf("This version of UOX3 was complied incorrectly. sizeof(tile_st) = %d \n", sizeof(tile_st));
		
		printf("\n");
		
		// Server.scp status --- By Magius(CHE)
		printf( "Server Settings:\n" );
		
		printf( " -Archiving " );
		if( strlen( server_data.archivepath ) > 1 ) // Moved by Magius(CHE (1)
			printf( "Enabled. (%s)\n", server_data.archivepath );
		else printf( "Disabled!\n" );
		
		printf(" -Weapons & Armour Rank System: " );
		if( server_data.rank_system == 1 ) printf( "Activated!\n" );
		else printf( "Disabled!\n" );
		
		printf(" -Vendors buy by item name: " );
		if( server_data.sellbyname == 1 ) printf( "Activated!\n" );
		else printf( "Disabled!\n" );
		
		printf( " -Adv. Trade System: " ); // Magiuc(CHE)
		if( server_data.trade_system == 1 ) printf( "Activated!\n" ); // Magius(CHE)
		else printf( "Disabled!\n" ); // Magius(CHE)
		
		printf( " -Special Bank stuff: " ); // AntiChrist
		if( server_data.usespecialbank == 1 ) printf( "Activated!\n" ); // AntiChrist - Special Bank
		else printf( "Disabled!\n" ); // AntiChrist - Special Bank
		
		printf( " -Crash Protection: " );//Zippy
		if (server_data.crashprotect < 1) printf( "Disabled!\n" );
#ifndef _CRASH_PROTECT_
		else printf("Unavailable in this version.\n");
#else
		else if ( server_data.crashprotect == 1) printf( "Save on crash.\n");
		else printf( "Save & Restart Server.\n" );
#endif

		printf(" -AntiLag Server Save: ");
		if ( cwmWorldState->LoopSaveAmt() > 0 )	printf("%li items & chars per cycle.\n", cwmWorldState->LoopSaveAmt());
		else printf("Disabled!\n");

		printf( " -xGM Remote: " ); // Eagle --- xGM
		if ( !xgm ) printf ( "Disabled!\n" ); // Eagle -- xGM
		else printf( "Activated!\n" );// Eagle -- xGM
		
		item_test(); // LB
		
		
		printf("UOX3: Startup Complete.\n\n");
		savelog("-=Server Startup=-\n=======================================================================\n","server.log");
		uiCurrentTime=getclock();

		/* MAIN SYSTEM LOOP */
		while (keeprun)
		{
			checkkey();
#ifdef _MSVC
			switch(speed.nice)
			{
			case 0: break;  // very unnice - hog all cpu time
			case 1:
				if (now!=0) Sleep(10);
				else Sleep(90);
				break;
			case 2: Sleep(10); break;
			case 3: Sleep(40); break;// very nice
				// feel free to define more ,lb
			default: Sleep(10); break;
			}
#else
#ifdef __NT__
			switch(speed.nice)
			{
			case 0: break;  // very unnice - hog all cpu time
			case 1:
				if (now!=0) delay(10);
				else delay(90); 
				break;
			case 2: delay(10); break;
			case 3: delay(40); break;// very nice
				// feel free to define more ,lb
				
			default: delay(10); break;
			}
			
#else
			switch(speed.nice)
			{
			case 0: break;;  // very unnice - hog all cpu time
			case 1: if (now!=0) usleep(10000);
				else usleep(90000);
				break;
			case 2: usleep(10000); break; 
			case 3: usleep(40000); break;// very nice
				// feel free to define more ,lb
				
			default: usleep(10000); break;
			}
#endif
#endif
			
			if(loopTimeCount >= 1000)       {
				loopTimeCount = 0;
				loopTime = 0;
			}
			loopTimeCount++;
			
			StartMilliTimer(loopSecs, loopMilli);
			
			if(networkTimeCount >= 1000)    {
				networkTimeCount = 0;
				networkTime = 0;
			}
			
			StartMilliTimer(tempSecs, tempMilli);
			if( uiNextCheckConn<=uiCurrentTime || overflow) // Cut lag on CheckConn by not doing it EVERY loop.
			{
				Network->CheckConn();
				uiNextCheckConn = (unsigned int)( uiCurrentTime + ( double )( 3 * CLOCKS_PER_SEC ) );
			}
			Network->CheckMessage();
			
			tempTime = CheckMilliTimer(tempSecs, tempMilli);
			networkTime += tempTime;
			networkTimeCount++;
			
			if(timerTimeCount >= 1000)      {
				timerTimeCount = 0;
				timerTime = 0;
			}
			
			StartMilliTimer(tempSecs, tempMilli);
			
			checktimers();
			uiCurrentTime=getclock(); // getclock() only once
			tempTime = CheckMilliTimer(tempSecs, tempMilli);
			timerTime += tempTime;
			timerTimeCount++;
			
			if(autoTimeCount >= 1000)       {
				autoTimeCount = 0;
				autoTime = 0;
			}
			StartMilliTimer(tempSecs, tempMilli);
			
			checkauto();
			
		
			tempTime = CheckMilliTimer(tempSecs, tempMilli);
			autoTime  += tempTime;
			autoTimeCount++;
			StartMilliTimer( tempSecs, tempMilli );
			Network->ClearBuffers();
			tempTime = CheckMilliTimer( tempSecs, tempMilli );
			networkTime += tempTime;
			tempTime = CheckMilliTimer(loopSecs, loopMilli);
			loopTime += tempTime;
			
	}
	
	sysbroadcast("The server is shutting down.");
	if (server_data.html>0) 
	{
		printf("Writing offline HTML page...");
		offlinehtml();//HTML  // lb, the if prevents a crash on shutdown if html deactivated ...
		printf(" Done.\n");
	}
	//Writeslot("shutdown");
	printf("Clearing IM Menus...");
	im_clearmenus();
	printf(" Done.\nClosing sockets...");
	Network->SockClose();
	printf(" Done.\n");

	if ( !cwmWorldState->Saving() )
	{
		do {
			cwmWorldState->savenewworld(1);
		} while ( cwmWorldState->Saving() );
	}

	printf("Saving Server.scp...\n");
	saveserverscript(1);
	printf("\n");
	
	printf("UOX3: Server shutdown complete!\n");
	savelog("Server Shutdown!\n=======================================================================\n\n\n","server.log");
	
	Shutdown( 0 );
	
#ifdef _CRASH_PROTECT_
	} catch ( ... ) 
	{//Crappy error handling...
		printf("Unknown exception caught, hard crash avioded!\n");
		Shutdown( UNKNOWN_ERROR );
	}
#endif
	
	return( 0 );	
}


//o---------------------------------------------------------------------------o
//|            Function     - Restart()
//|            Date         - 1/7/00
//|            Programmer   - Zippy
//o---------------------------------------------------------------------------o
//|            Purpose      - Restarts the server, passes the server number of 
//|								Number of crashes so far, if < 10 then the
//|								Server will restart itself.
//o---------------------------------------------------------------------------o
void Restart( unsigned short ErrorCode = UNKNOWN_ERROR )
{
	if (!ErrorCode)
		return;
	
	if (server_data.crashprotect>1)
	{		
		if (ErrorCount < 10)
		{
			ErrorCount++;
			
			sprintf(temp, "Server crash #%i from unknown error, restarting.", ErrorCount);
			savelog(temp,"server.log");
			printf(temp);
			printf("\n");
			
			sprintf(temp, "uox3.exe -ERROR %i", ErrorCount);
			
			if (xgm)
				strcat(temp, " -xgm");
			
			system( temp );
			exit(ErrorCode); // Restart successful Don't give them key presses or anything, just go out.
		} else {
			savelog("10th Server crash, server shutting down.", "server.log");
			printf("10th Server crash, server shutting down.\n");
		}
	} else 
		savelog("Server crash!","server.log");
}

//o---------------------------------------------------------------------------o
//|            Function     - void Shutdown( int retCode )
//|            Date         - Oct. 09, 1999
//|            Programmer   - Krazyglue
//o---------------------------------------------------------------------------o
//|            Purpose      - Handled deleting / free() ing of pointers as neccessary
//|                                   as well as closing open file handles to avoid file
//|                                   file corruption.
//|                                   Exits with proper error code.
//o---------------------------------------------------------------------------o
void Shutdown( int retCode )
{
	if ( server_data.crashprotect >= 1 && retCode && Loaded && cwmWorldState && !cwmWorldState->Saving() )
	{//they want us to save, there has been an error, we have loaded the world, and WorldState is a valid pointer.
		do {
			cwmWorldState->savenewworld(1);
		} while ( cwmWorldState->Saving() );
	}
	
	offlinehtml();//display server shutdown page
	
	// delete any objects that were created (delete takes care of NULL check =)
	delete cwmWorldState;
	delete mapRegions;
	delete Admin;
	delete Boats;
	delete Combat;
	delete Commands;
	delete Guilds;
	delete Gumps;
	delete Items;
	delete Map;
	delete Npcs;
	delete Skills;
	delete Towns;
	delete Weight;
	delete Targ;
	delete Magic;
	delete Races;
	delete Weather;
	delete Movement;
	delete Network;
	delete Effects;
	delete WhoList;
	delete OffList;
	delete Books;
	
	// don't leave file pointers open, could lead to file corruption
	if( infile )
	{
		fclose(infile);
		infile = NULL;
	}
	if( scpfile )
	{
		fclose(scpfile);
		scpfile = NULL;
	}
	if( lstfile )
	{
		fclose(lstfile);
		scpfile = NULL;
	}
	if( wscfile )
	{
		fclose(wscfile);
		wscfile = NULL;
	}
	
	
	int i;
	
	for ( i = 0; i < HASHMAX; i++)
	{
		delete [] itemsp[i].pointer;
		delete [] charsp[i].pointer;
		delete [] cownsp[i].pointer;
		delete [] spawnsp[i].pointer;
		delete [] contsp[i].pointer;
		delete [] cspawnsp[i].pointer;
		delete [] ownsp[i].pointer;
		delete [] imultisp[i].pointer;
		delete [] cmultisp[i].pointer;
		delete [] glowsp[i].pointer;
	}
	
	if( clickx )           delete [] clickx;
	if( clicky )           delete [] clicky;
	if( currentSpellType ) delete [] currentSpellType;
	if( targetok )         delete [] targetok;

	if( loscache )    delete [] loscache;
	if( itemids )     delete [] itemids;
	
	
	if ( retCode && Loaded )//do restart unless we have crashed with some error.
		Restart( retCode );
	
	
	
	// dispay what error code we had
	// don't report errorlevel for no errors, this is confusing ppl - fur
	if (retCode)
		printf("\nExiting UOX3 with errorlevel %d...\n", retCode);
	else
		printf("\nExiting UOX3 with no errors...\n");
	
	// let windows users see what happened during shutdown
#ifdef __NT__
	if( retCode )
	{
		printf("\nPress any key to continue.");
		getch();
	}
#endif
	
	exit(retCode);
}




char iteminrange( UOXSOCKET s, ITEM i, int distance )
{
//	if( s > now || i < 0 || i > imem )
//		return 0;
	if( chars[currchar[s]].priv&0x01 )	// GM
		return 1;
	short dx = abs( chars[currchar[s]].x - items[i].x );
	if( dx > distance )
		return 0;
	short dy = abs( chars[currchar[s]].y - items[i].y );
	if( dy > distance )
		return 0;
	return 1;
}

char npcinrange( UOXSOCKET s, int i, int distance )
{
	if( s > now || i < 0 || i > cmem )
		return 0;
	if( chars[currchar[s]].priv&0x01 )
		return 1;
	short dx = abs( chars[currchar[s]].x - chars[i].x );
	if( dx > distance )
		return 0;
	short dy = abs( chars[currchar[s]].y - chars[i].y );
	if( dy > distance )
		return 0;
	return 1;
}

int ishuman( CHARACTER p )
{
	// Check if the player or Npc is human! -- by Magius(CHE_
	if(( chars[p].xid1 == 0x01 ) && ( chars[p].xid2 == 0x90 || chars[p].xid2 == 0x91 ) ) 
		return 1;
	else 
		return 0;
}

void npcact(int s)
{
	int i,serial;
	
	
	serial=calcserial(buffer[s][7],buffer[s][8],buffer[s][9],buffer[s][10]);
	i=findbyserial(&charsp[serial%HASHMAX], serial, 1);
	if (i!=-1)
	{
		npcaction(i,addid1[s]);
	}
}

void objTeleporters(int s)
{
	int x=chars[s].x, y=chars[s].y;
	
	int     StartGrid=mapRegions->StartGrid(chars[s].x,chars[s].y);
	
	unsigned int increment=0;
	for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
	{
		for (int a=0;a<3;a++)
		{
			int mapitemptr=-1;
			int mapitem=-1;
			int mapchar=-1;
			do //check all items in this cell
			{
				mapchar=-1;
				mapitemptr=mapRegions->GetNextItem(checkgrid+a, mapitemptr);
				if (mapitemptr==-1) break;
				mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
				if(mapitem>999999) mapchar=mapitem-1000000;
				if (mapitem!=-1 && mapitem<1000000)
				{
					if (items[mapitem].x==x && items[mapitem].y==y &&
						((abs(items[mapitem].z) + 10 ) >= abs(chars[s].z)) && ((abs(items[mapitem].z) - 10 ) <= abs(chars[s].z)))
					{
						if ((items[mapitem].type==60)&&(items[mapitem].morex+items[mapitem].morey+items[mapitem].morez >0))
						{
							mapRegions->RemoveItem(s+1000000); //LB, remove with oldx,y
							chars[s].x=items[mapitem].morex;
							chars[s].y=items[mapitem].morey;
							chars[s].dispz=chars[s].z=items[mapitem].morez;
							mapRegions->AddItem(s+1000000); //LB, add with new x,y
							teleport(s);
						}
						
						// advancement objects
						if ((items[mapitem].type==80)&&!(chars[s].npc))
							if(items[mapitem].more1!=0 || items[mapitem].more2!=0 || items[mapitem].more3!=0 || items[mapitem].more4!=0)
							{
								if(chars[s].ser1==items[mapitem].more1 && chars[s].ser2==items[mapitem].more2 && chars[s].ser3==items[mapitem].more3 && chars[s].ser4==items[mapitem].more4)
									advancementobjects(s,items[mapitem].morex,0);
							}
							else
								advancementobjects(s,items[mapitem].morex,0);
							
						if ((items[mapitem].type==81)&&!(chars[s].npc))
							if(items[mapitem].more1!=0 || items[mapitem].more2!=0 || items[mapitem].more3!=0 || items[mapitem].more4!=0)
							{
								if(chars[s].ser1==items[mapitem].more1 && chars[s].ser2==items[mapitem].more2 && chars[s].ser3==items[mapitem].more3 && chars[s].ser4==items[mapitem].more4)
									advancementobjects(s,items[mapitem].morex,1);
							}
							else
								advancementobjects(s,items[mapitem].morex,1);
							// The above code lets you restrict a gate's use by setting its MORE values to a char's
							// serial #
							
						// damage objects
						if (!(chars[s].priv&4)&&(items[mapitem].type==85) && !chars[s].dead )	// don't hurt dead things
						{
							chars[s].hp=chars[s].hp-(items[mapitem].morex+RandomNum(items[mapitem].morey,items[mapitem].morez));
							if (chars[s].hp<1) chars[s].hp=0;
							updatestats(s, 0);
							if (chars[s].hp<=0) deathstuff(s);
						}
						// monster gates
						if (items[mapitem].type==82)
							monstergate(s,items[mapitem].morex);
						
						if (items[mapitem].type==83)				// Race gates, Abaddon
						{
							if( items[mapitem].morey != 0 )
								Races->gate( s, items[mapitem].morex, items[mapitem].morey );
							else
								Races->gate( s, items[mapitem].morex );
						}
						
						if (items[mapitem].type==111)				// Zero kill gate, Abaddon
						{
							chars[s].kills = 0;
							updatechar( s );
						}
						
						// sound objects
						if (items[mapitem].type==86)
						{
							if (RandomNum(1,100)<=items[mapitem].morez)
								soundeffect3(mapitem, items[mapitem].morex, items[mapitem].morey);
						}
					}
				}
			} while (mapitem!=-1);
		}
	}
}

void npcToggleCombat(int s)
{
	chars[s].war=(!(chars[s].war));
	Movement->CombatWalk(s);
}

int chardir(int a, int b)   // direction from character a to char b
{
	int dir,xdif,ydif;
	
	xdif = chars[b].x-chars[a].x;
	ydif = chars[b].y-chars[a].y;
	
	if ((xdif==0)&&(ydif<0)) dir=0;
	else if ((xdif>0)&&(ydif<0)) dir=1;
	else if ((xdif>0)&&(ydif==0)) dir=2;
	else if ((xdif>0)&&(ydif>0)) dir=3;
	else if ((xdif==0)&&(ydif>0)) dir=4;
	else if ((xdif<0)&&(ydif>0)) dir=5;
	else if ((xdif<0)&&(ydif==0)) dir=6;
	else if ((xdif<0)&&(ydif<0)) dir=7;
	else dir=-1;
	
	return dir;
}


int calcSocketFromChar(int i)
{
	int j;
	
	if ((i<0) || (i >= cmem))
	{
		//LogMessage("calcSocketFromChar() - Bad char number (%i)\n" _ i);
		return -1;
	}
	if (chars[i].npc) return -1;
	for (j=0; j<now;j++)
	{
		if (currchar[j]==i && (perm[j])) return j;
	}
	return -1;
}

int calcCharFromSer(unsigned char ser1, unsigned char ser2, unsigned char ser3, unsigned char ser4)
{
	int serial;
	
	serial = calcserial(ser1, ser2, ser3, ser4);
	return(findbyserial(&charsp[serial%HASHMAX], serial, 1));
}

int calcCharFromSer(int serial)
{
	return(findbyserial(&charsp[serial%HASHMAX], serial, 1));
}

int calcSerFromChar(int i)
{
	if ((i>=0) && (i < cmem))
		return (chars[i].ser1<<24)+(chars[i].ser2<<16)+(chars[i].ser3<<8)+chars[i].ser4;
	else
	{
		LogMessage("calcSerFromChar() - char does not exist (%i)"_ i);
		return -1;
	}
}

int calcItemFromSer(unsigned char ser1, unsigned char ser2, unsigned char ser3, unsigned char ser4)
{
	int serial;
	serial=calcserial(ser1, ser2, ser3, ser4);
	return findbyserial(&itemsp[serial%HASHMAX], serial, 0);
}

int calcItemFromSer( int ser ) // Added by Magius(CHE) (2)
{
	return findbyserial( &itemsp[ser%HASHMAX], ser, 0 );
}



int checkBoundingBox(int xPos, int yPos, int fx1, int fy1, int fz1, int fx2, int fy2)
{
	if (xPos>=((fx1<fx2)?fx1:fx2) && xPos<=((fx1<fx2)?fx2:fx1))
		if (yPos>=((fy1<fy2)?fy1:fy2) && yPos<=((fy1<fy2)?fy2:fy1))
			if (fz1==-1 || abs(fz1-Map->Height(xPos, yPos, fz1))<=5)
				return 1;
			return 0;
}

int checkBoundingCircle(int xPos, int yPos, int fx1, int fy1, int fz1, int radius)
{
	if ( (xPos-fx1)*(xPos-fx1) + (yPos-fy1)*(yPos-fy1) <= radius * radius)
		if (fz1==-1 || abs(fz1-Map->Height(xPos, yPos, fz1))<=5)
			return 1;
		return 0;
}



void respawn(unsigned int currenttime)
{
	int i, j, k,m,serial,serhash,ci;
	if (gRespawnItem>=itemcount) gRespawnItem=0;  //set global last respawned item
	for(i=gRespawnItem;i<itemcount && i<(gDecayItem+1000);i++) //look at 1000 items at a time
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
				if ((ci > -1) && //HoneyJar
					items[i].serial==items[ci].spawnserial && (items[ci].free==0))
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
					items[i].gatetime=(rand()%((int)(1+((items[i].morez-items[i].morey)*(CLOCKS_PER_SEC*60))))) +
						(items[i].morey*CLOCKS_PER_SEC*60)+currenttime;
				}
				if ((items[i].gatetime<=currenttime ||(overflow)) && items[i].morex!=0)
				{
					Items->AddRespawnItem(i, items[i].morex, 0);
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
				if ((ci > -1) && //HoneyJar
					chars[ci].spawnserial==serial && chars[ci].free==0)
				{
					k++;
				}
			}
			if (k<items[i].amount)
			{
				if (items[i].gatetime==0)
				{
					items[i].gatetime=(rand()%((int)(1+
						((items[i].morez-items[i].morey)*(CLOCKS_PER_SEC*60))))) +
						
						(items[i].morey*CLOCKS_PER_SEC*60)+currenttime;
				}
				if ((items[i].gatetime<=currenttime || (overflow)) && items[i].morex!=0)
				{
					Npcs->AddRespawnNPC(i, -1, items[i].morex,1);
					//					Npcs->AddRespawnNPC( i, items[i].morex, 1 );
					items[i].gatetime=0;
				}
			}
		}
		if ((items[i].type==63)||(items[i].type==64)||(items[i].type==65))
		{
			serial=items[i].serial;
			serhash=serial%HASHMAX;
			for (j=0;j<spawnsp[serhash].max;j++)
			{
				ci=spawnsp[serhash].pointer[j];
				if ((ci > -1) && //HoneyJar
					items[ci].spawnserial==serial && items[ci].free==0)
				{
					m++;
				}
			}
			if(m<6)
			{
				k=0;
				serial=items[i].serial;
				serhash=serial%HASHMAX;
				for (j=0;j<spawnsp[serhash].max;j++)
				{
					ci=spawnsp[serhash].pointer[j];
					if ((ci > -1) && //HoneyJar
						items[i].serial==items[ci].spawnserial && (items[ci].free==0))
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
						items[i].gatetime=(rand()%((int)(1+((items[i].morez-items[i].morey)*(CLOCKS_PER_SEC*60))))) +
							(items[i].morey*CLOCKS_PER_SEC*60)+uiCurrentTime;
					}
					if ((items[i].gatetime<=currenttime ||(overflow)) && items[i].morex!=0)
					{
						if(items[i].type==63) items[i].type=64; //Lock the container 
						Items->AddRespawnItem(i, items[i].morex, 1);
						items[i].gatetime=0;
					}
				}
			}
		}
	}
	gRespawnItem= (i<itemcount) ? i : 0;  //set global last respawned item
}



// clock() is supposed to return CPU time used - it doesn't on Windows, but
// does on Linux. Thus it doesn't go up on Linux, and time seems to stand
// still. This function emulates clock()
#ifndef __NT__
unsigned long int getclock( void )
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	// We want to keep the value within 32 bits; we could also substract
	// startup time I suppose
	return ((tv.tv_sec - 900000000) * CLOCKS_PER_SEC) +
		tv.tv_usec / (1000000 / CLOCKS_PER_SEC);
}
#endif

void staticeffect(int player, unsigned char eff1, unsigned char eff2, char speed, char loop)
{
	char effect[29];
	int j;
	memset (&effect[0], 0, 29);

	effect[0]=0x70; // Effect message
	effect[1]=0x03; // Static effect
	effect[2]=chars[player].ser1;
	effect[3]=chars[player].ser2;
	effect[4]=chars[player].ser3;
	effect[5]=chars[player].ser4;
	//[6] to [9] are the target ser, not applicable here.
	effect[10]=eff1;// Object id of the effect
	effect[11]=eff2;
	effect[12]=chars[player].x>>8;
	effect[13]=chars[player].x%256;
	effect[14]=chars[player].y>>8;
	effect[15]=chars[player].y%256;
	effect[16]=chars[player].z;
	//[17] to [21] are the target's position, not applicable here.
	effect[22]=speed;
	effect[23]=loop; // 0 is really long.  1 is the shortest.
	effect[24] = 0; // This value is unknown
	effect[25] = 0; // This value is unknown
	effect[26]=1; // LB client crashfix
	effect[27]=0; // This value is used for moving effects that explode on impact.
	for (j=0;j<now;j++)
	{
		if ( perm[j] && inrange1p(currchar[j],player) )
		{
			Network->xSend(j, effect, 28, 0);
		}
	}
	// printf("CRASH3??\n");
}

// staticeffect3 is for effects on items
void staticeffect3(UI16 x, UI16 y, SI08 z, unsigned char eff1, unsigned char eff2, char speed, char loop, char explode)
{
	char effect[29];
	int j;
	memset ( &effect[0], 0, 29 );

	effect[0]=0x70; // Effect message
	effect[1]=0x02; // Static effect
	//[6] to [9] are the target ser, not applicable here.
	effect[10]=eff1;// Object id of the effect
	effect[11]=eff2;
	effect[12]=x>>8;
	effect[13]=x%256;
	effect[14]=y>>8;
	effect[15]=y%256;
	effect[16]=z;
	effect[17]=x>>8;
	effect[18]=x%256;
	effect[19]=y>>8;
	effect[20]=y%256;
	effect[21]=z;
	//[17] to [21] are the target's position, not applicable here.
	effect[22]=speed;
	effect[23]=loop; // 0 is really long.  1 is the shortest.
	effect[24] = 0; // This value is unknown
	effect[25] = 0; // This value is unknown
	effect[26]=1; // LB changed to 1
	effect[27]=explode; // This value is used for moving effects that explode on impact.
	for (j=0;j<now;j++)
	{  // if inrange of effect and online send effect
		Network->xSend(j, effect, 28, 0);
	}
}

// staticeffect2 is for effects on items
void staticeffect2(int nItem, unsigned char eff1, unsigned char eff2, char speed, char loop, char explode)
{
	char effect[29];
	int j;
	memset ( &effect[0], 0, 29 );

	effect[0]=0x70; // Effect message
	effect[1]=0x02; // Static effect
	effect[2]=items[nItem].ser1;
	effect[3]=items[nItem].ser2;
	effect[4]=items[nItem].ser3;
	effect[5]=items[nItem].ser4;
	effect[6]=items[nItem].ser1;
	effect[7]=items[nItem].ser2;
	effect[8]=items[nItem].ser3;
	effect[9]=items[nItem].ser4;
	//[6] to [9] are the target ser, not applicable here.
	effect[10]=eff1;// Object id of the effect
	effect[11]=eff2;
	effect[12]=items[nItem].x>>8;
	effect[13]=items[nItem].x%256;
	effect[14]=items[nItem].y>>8;
	effect[15]=items[nItem].y%256;
	effect[16]=items[nItem].z;
	effect[17]=items[nItem].x>>8;
	effect[18]=items[nItem].x%256;
	effect[19]=items[nItem].y>>8;
	effect[20]=items[nItem].y%256;
	effect[21]=items[nItem].z;
	//[17] to [21] are the target's position, not applicable here.
	effect[22]=speed;
	effect[23]=loop; // 0 is really long.  1 is the shortest.
	effect[24] = 0; // This value is unknown
	effect[25] = 0; // This value is unknown
	effect[26]=1; // LB changed to 1
	effect[27]=explode; // This value is used for moving effects that explode on impact.
	for (j=0;j<now;j++)
	{  // if inrange of effect and online send effect
		if ( perm[j] && inrange2(j,nItem) )
		{
			Network->xSend(j, effect, 28, 0);
		}
	}
}

void bolteffect(int player)
{
	char effect[29];
	int j;
	memset ( &effect[0], 0, 29 );

	effect[0]=0x70; // Effect message
	effect[1]=0x01; // Bolt effect
	effect[2]=chars[player].ser1;
	effect[3]=chars[player].ser2;
	effect[4]=chars[player].ser3;
	effect[5]=chars[player].ser4;
	//[6] to [11] are  not applicable here.
	effect[12]=chars[player].x>>8;
	effect[13]=chars[player].x%256;
	effect[14]=chars[player].y>>8;
	effect[15]=chars[player].y%256;
	effect[16]=chars[player].z;
	//[17] to [27] are not applicable here.
	
	effect[26] = 1;
	effect[27] = 0;
	for (j=0;j<now;j++)
	{
		if ( perm[j] && inrange1p(currchar[j],player) )
		{
			Network->xSend(j, effect, 28, 0);
		}
	}
}


void bolteffect2(int player,char a1,char a2)  // experimenatal, lb
{
	char effect[29];
	int j,x2,x,y2,y;
	memset ( &effect[0], 0, 29 );

	effect[0]=0x70; // Effect message
	effect[1]=0x00; // effect from source to dest
	effect[2]=chars[player].ser1;
	effect[3]=chars[player].ser2;
	effect[4]=chars[player].ser3;
	effect[5]=chars[player].ser4;
	
	effect[10]=a1;
	effect[11]=a2;
	
	y=rand()%15;
	x=rand()%15;	
	if (rand()%2==1) x=x*-1;
	if (rand()%2==1) y=y*-1;
	x2=chars[player].x+x;
	y2=chars[player].y+y;
	if (x2<0) x2=0;
	if (y2<0) y2=0;
    if (x2>6144) x2=6144;
	if (y2>4096) y2=4096;
	
	effect[12]=chars[player].x>>8; // source coordinates
	effect[13]=chars[player].x%256;
	effect[14]=chars[player].y>>8;
	effect[15]=chars[player].y%256;
	//effect[16]=chars[player].z;
	effect[16]=0;
	
    effect[17]=x2>>8;  //target coordiantes
	effect[18]=x2%256;
	effect[19]=y2>>8;
	effect[20]=y2%256;
	effect[21]=127;
	
	//[22] to [27] are not applicable here.
	
	effect[26]=1; // client crash bugfix
	effect[27]=0;
	
	for (j=0;j<now;j++)
	{
		if ( perm[j] && inrange1p(currchar[j],player) )
		{
			Network->xSend(j, effect, 28, 0);
		}
	}
}


void movingeffect(int source, int dest, unsigned char eff1, unsigned char eff2, char speed, char loop, char explode)
{
	//0x0f 0x42 = arrow 0x1b 0xfe=bolt
	char effect[29];
	int j;
	memset ( &effect[0], 0, 29 );

	effect[0]=0x70; // Effect message
	effect[1]=0x00; // Moving effect
	effect[2]=chars[source].ser1;
	effect[3]=chars[source].ser2;
	effect[4]=chars[source].ser3;
	effect[5]=chars[source].ser4;
	effect[6]=chars[dest].ser1;
	effect[7]=chars[dest].ser2;
	effect[8]=chars[dest].ser3;
	effect[9]=chars[dest].ser4;
	effect[10]=eff1;// Object id of the effect
	effect[11]=eff2;
	effect[12]=chars[source].x>>8;
	effect[13]=chars[source].x%256;
	effect[14]=chars[source].y>>8;
	effect[15]=chars[source].y%256;
	effect[16]=chars[source].z;
	effect[17]=chars[dest].x>>8;
	effect[18]=chars[dest].x%256;
	effect[19]=chars[dest].y>>8;
	effect[20]=chars[dest].y%256;
	effect[21]=chars[dest].z;
	effect[22]=speed;
	effect[23]=loop; // 0 is really long.  1 is the shortest.
	effect[24]=0; // This value is unknown
	effect[25]=0; // This value is unknown
	effect[26]=0; // LB changed to 1
	effect[27]=explode; // This value is used for moving effects that explode on impact.
	for (j=0;j<now;j++)
	{
		if ( perm[j] && inrange1p(currchar[j],source) && inrange1p(currchar[j],dest) )
		{
			Network->xSend(j, effect, 28, 0);
		}
	}
}

//	- Movingeffect2 is used to send an object from a char
//    to another object (like purple potions)
void movingeffect2(int source, int dest, char eff1, char eff2, char speed, char loop, char explode)
{
	//0x0f 0x42 = arrow 0x1b 0xfe=bolt
	char effect[29];
	int j;
	memset ( &effect[0], 0, 29 );

	effect[0]=0x70; // Effect message
	effect[1]=0x00; // Moving effect
	effect[2]=chars[source].ser1;
	effect[3]=chars[source].ser2;
	effect[4]=chars[source].ser3;
	effect[5]=chars[source].ser4;
	effect[6]=items[dest].ser1;
	effect[7]=items[dest].ser2;
	effect[8]=items[dest].ser3;
	effect[9]=items[dest].ser4;
	effect[10]=eff1;// Object id of the effect
	effect[11]=eff2;
	effect[12]=chars[source].x>>8;
	effect[13]=chars[source].x%256;
	effect[14]=chars[source].y>>8;
	effect[15]=chars[source].y%256;
	effect[16]=chars[source].z;
	effect[17]=items[dest].x>>8;
	effect[18]=items[dest].x%256;
	effect[19]=items[dest].y>>8;
	effect[20]=items[dest].y%256;
	effect[21]=items[dest].z;
	effect[22]=speed;
	effect[23]=loop; // 0 is really long.  1 is the shortest.
	effect[24]=0; // This value is unknown
	effect[25]=0; // This value is unknown
	effect[26]=0; // This value is unknown
	effect[27]=explode; // This value is used for moving effects that explode on impact.
	for (j=0;j<now;j++)
	{   // - If in range of source person or destination position and online send effect
		if ( perm[j] && ( inrange1p(currchar[j], source) || inrange2(j, dest) ) )
		{
			Network->xSend(j, effect, 28, 0);
		}
	}
}

//	- Movingeffect3 is used to send an object from a char
//    to another object (like purple potions)
void movingeffect3(CHARACTER source, unsigned short x, unsigned short y, signed char z, char eff1, char eff2, char speed, char loop, char explode)
{
	//0x0f 0x42 = arrow 0x1b 0xfe=bolt
	char effect[29];
	int j;

	memset ( &effect[0], 0, 29 );
	effect[0]=0x70; // Effect message
	effect[1]=0x00; // Moving effect
	effect[2]=chars[source].ser1;
	effect[3]=chars[source].ser2;
	effect[4]=chars[source].ser3;
	effect[5]=chars[source].ser4;

	effect[10]=eff1;// Object id of the effect
	effect[11]=eff2;
	effect[12]=chars[source].x>>8;
	effect[13]=chars[source].x%256;
	effect[14]=chars[source].y>>8;
	effect[15]=chars[source].y%256;
	effect[16]=chars[source].z;
	effect[17] = (char)x>>8;
	effect[18] = (char)x%256;
	effect[19] = (char)y>>8;
	effect[20] = (char)y%256;
	effect[21]=z;
	effect[22]=speed;
	effect[23]=loop; // 0 is really long.  1 is the shortest.

	effect[27]=explode; // This value is used for moving effects that explode on impact.
	for (j=0;j<now;j++)
	{   // - If in range of source person or destination position and online send effect
		Network->xSend(j, effect, 28, 0);
	}
}


void setabovelight(char lightchar)
{
	int i;
	if (lightchar != worldcurlevel)
	{
		worldcurlevel=lightchar;
		for (i=0;i<now;i++)
		{
			if (perm[i]) dolight(i, worldcurlevel);
		}
	}
}

void dolight(int s, char level)
{
	char light[3]="\x4F\x00";
	
	if ((s==-1)||(!perm[s])) return;
	light[1]=level;
	if (worldfixedlevel!=255)
	{
		light[1]=worldfixedlevel;
	} else {
		if (chars[currchar[s]].fixedlight!=255)
		{
			light[1]=chars[currchar[s]].fixedlight;
		} else {
			if (indungeon(currchar[s]))
			{
				int toShow;
				if( Races->getVisLevel( chars[currchar[s]].race ) > dungeonlightlevel )
					toShow = 0;
				else
					toShow = dungeonlightlevel - Races->getVisLevel( chars[currchar[s]].race );
				light[1]=toShow;
//				printf("Light level %i", toShow );
			}
			else
			{
				light[1]=level;
			}
		}
	}
	Network->xSend(s, light, 2, 0);
}

void doworldlight(void)
{
	unsigned char c = 255;
	int i=-1;
	int realhour = hour;
	//	if( ampm )
	//		realhour = 12 - hour;
	float hourIncrement = (float)((float)worlddarklevel - (float)worldbrightlevel) / 12;
	float minuteIncrement = hourIncrement / 60;
	float lightValue = 0;
	if( !ampm )
	{
		lightValue = hour * hourIncrement + minute * minuteIncrement;
	}
	else
	{
		realhour = 12 - hour;
		lightValue = realhour * hourIncrement + (60-minute) * minuteIncrement;
	}
	i = worlddarklevel - (int)lightValue;
	//printf( "Light level: %f\n", lightValue );
	//	i = ( ( (12-realhour)*60 ) + minute ) / 720 * ( worlddarklevel - worldbrightlevel );
	//	if (wtype) i=i+2;
	//	if (moon1+moon2<4) i=i+1;
	//	if (moon1+moon2<10) i=i+1;
	c=i;
	if (c!=worldcurlevel)
	{
		worldcurlevel=c;
		//		setabovelight(c);
	}
}


void telltime( UOXSOCKET s )
{
	char tstring[60]; 
	char tstring2[60];
	int lhour;
	lhour=hour;
	
	if ((minute>=0)&&(minute<=14)) strcpy(tstring,"It is");
	else if ((minute>=15)&&(minute<=30)) strcpy(tstring,"It is a quarter past");
	else if ((minute>=30)&&(minute<=45)) strcpy(tstring,"It is half past");
	else
	{
		strcpy(tstring,"It is a quarter till");
		lhour++;
		if (lhour==0) lhour=12;
	}
	
	switch (lhour) 
	{
	case 1:		sprintf(tstring2,"%s one o'clock",tstring);		break;
	case 2:		sprintf(tstring2,"%s two o'clock",tstring);		break;
	case 3:		sprintf(tstring2,"%s three o'clock",tstring);	break;
	case 4:		sprintf(tstring2,"%s four o'clock",tstring);	break;
	case 5:		sprintf(tstring2,"%s five o'clock",tstring);	break;
	case 6:		sprintf(tstring2,"%s six o'clock",tstring);		break;
	case 7:		sprintf(tstring2,"%s seven o'clock",tstring);	break;
	case 8:		sprintf(tstring2,"%s eight o'clock",tstring);	break;
	case 9:		sprintf(tstring2,"%s nine o'clock",tstring);	break;
	case 10:	sprintf(tstring2,"%s ten o'clock",tstring);		break;
	case 11:	sprintf(tstring2,"%s eleven o'clock",tstring);	break;
	case 12:
		if (ampm)
			sprintf(tstring2,"%s midnight.",tstring);
		else 
			sprintf(tstring2,"%s noon.",tstring);
		break;
	}

	if (lhour == 12) strcpy(tstring, tstring2);
	else if (ampm)
	{
		if ((lhour>=1)&&(lhour<6)) sprintf(tstring,"%s in the afternoon.",tstring2);
		else if ((lhour>=6)&&(lhour<9)) sprintf(tstring,"%s in the evening.",tstring2);
		else sprintf(tstring,"%s at night.",tstring2);
	}
	else
	{
		if ((lhour>=1)&&(lhour<5)) sprintf(tstring,"%s at night.",tstring2);
		else sprintf(tstring,"%s in the morning.",tstring2);
	}
	
	sysmessage(s,tstring);
}

void updateskill(int s, int skillnum)
{
	unsigned char update[11];
	//int i;
	
	// for (i=0;i<8;i++) //lb, no need ...
	//{
	//	update[i]=0;
	//}
	
	update[0] = 0x3A; // Skill Update Message
	update[1] = 0x00; // Length of message
	update[2] = 0x0B; // Length of message
	update[3] = 0xff; // single list
	
	update[4]=0x00;
	update[5]=skillnum;
	update[6]=chars[currchar[s]].skill[skillnum]>>8;
	update[7]=chars[currchar[s]].skill[skillnum]%256;
	update[8]=chars[currchar[s]].baseskill[skillnum]>>8;
	update[9]=chars[currchar[s]].baseskill[skillnum]%256;
	update[10]=chars[currchar[s]].lockState[skillnum];
	
	// if (skillnum>45) printf("skill:%i\n",skillnum);
	if (s!=-1) Network->xSend(s, update, 11, 0);
}




void impaction(int s, int act)
{
	if (chars[currchar[s]].onhorse && (act==0x10 || act==0x11))
	{
		action(s, 0x1b);
		return;
	}
	if ((chars[currchar[s]].onhorse || (chars[currchar[s]].id1<1 && chars[currchar[s]].id2<90))
		&& (act==0x22))
	{
		return;
	}
	action(s, act);
}

int chardirxyz(CHARACTER a, int x, int y, int z)   // direction from character a to char b
// PARAM WARNING: z is never referenced
{
	int dir,xdif,ydif;
	
	xdif = x-chars[a].x;
	ydif = y-chars[a].y;
	
	if ((xdif==0)&&(ydif<0)) dir=0;
	else if ((xdif>0)&&(ydif<0)) dir=1;
	else if ((xdif>0)&&(ydif==0)) dir=2;
	else if ((xdif>0)&&(ydif>0)) dir=3;
	else if ((xdif==0)&&(ydif>0)) dir=4;
	else if ((xdif<0)&&(ydif>0)) dir=5;
	else if ((xdif<0)&&(ydif==0)) dir=6;
	else if ((xdif<0)&&(ydif<0)) dir=7;
	else dir=-1;
	
	return dir;
}

int fielddir(CHARACTER s, int x, int y, int z)
{
	int dir=chardirxyz(s, x, y, z);
	switch (dir)
	{
	case 0:
	case 4:
		return 0;
		break;
	case 2:
	case 6:
		return 1;
		break;
	case 1:
	case 3:
	case 5:
	case 7:
	case -1:
		switch(chars[s].dir) // crashfix, LB
		{
		case 0:
		case 4:
			return 0;
			break;
		case 2:
		case 6:
			return 1;
			break;
		case 1:
		case 3:
		case 5:
		case 7:
			return 1;
		default:
			printf("ERROR: Fallout of switch statement without default. uox3.cpp, fielddir()\n"); //Morrolan
			return 0;
		}
		default:
			printf("ERROR: Fallout of switch statement without default. uox3.cpp, fielddir()\n"); //Morrolan
			return 0;
	}
	return 1;
}

int checkforchar(int x, int y, int z)
{
	int i;
	//for (i=0;i<charcount;i++)
	//{
	
	//Char mapRegions
	int getcell=mapRegions->GetCell(x,y);
	int mapitem=-1;
	int mapitemptr=-1;
	int mapchar=-1;
	do //check all items in this cell
	{
		mapitemptr=mapRegions->GetNextItem(getcell, mapitemptr);
		if (mapitemptr==-1) break;
		mapitem=mapRegions->GetItem(getcell, mapitemptr);
		if (mapitem>99999) mapchar=mapitem-1000000;
		if (mapitem!=-1 && mapitem>=1000000)
		{
			i=mapchar;
			if (online(i) || chars[i].npc)
			{
				if (chars[i].x==x && chars[i].y==y && chars[i].z==z)
				{
					return 1;
				}
			}
		}
	} while (mapitem!=-1);
	return 0;
}


void tempeffectsoff(void)
{
	int s;
	teffect_st *Effect;
	for( Effect = Effects->First(); !Effects->AtEnd(); Effect = Effects->Next() )
	{
		s = calcCharFromSer( Effect->dest1, Effect->dest2, Effect->dest3, Effect->dest4 );
		if ( s != -1 )
		{
			switch( Effect->num )
			{
			case 1:
				chars[s].priv2=chars[s].priv2&0xFD;
				break;
			case 2:
				chars[s].fixedlight=255;
				break;
			case 3:
				chars[s].dx=chars[s].dx + Effect->more1;
				break;
			case 4:
				chars[s].in=chars[s].in + Effect->more1;
				break;
			case 5:
				chars[s].st=chars[s].st + Effect->more1;
				break;
			case 6:
				chars[s].dx=chars[s].st + Effect->more1;
				break;
			case 7:
				chars[s].in=chars[s].in - Effect->more1;
				break;
			case 8:
				chars[s].st=chars[s].st - Effect->more1;
				break;
			case 11:
				chars[s].st=chars[s].st - Effect->more1;
				chars[s].dx=chars[s].dx - Effect->more2;
				chars[s].in=chars[s].in - Effect->more3;
				break;
			case 12:
				chars[s].st=chars[s].st + Effect->more1;
				chars[s].dx=chars[s].dx + Effect->more2;
				chars[s].in=chars[s].in + Effect->more3;
				break;
			case 25:
				chars[s].usepotion = 0;
				break;
			default:
				//printf("ERROR: Fallout of switch statement without default. uox3.cpp, tempeffectsoff()\n"); //Morrolan
				break;
			}
		}
	} // end of !=-1
}

void tempeffectson()
{
	int s;
	teffect_st *Effect;
	for( Effect = Effects->First(); !Effects->AtEnd(); Effect = Effects->Next() )
	{
		s = calcCharFromSer( Effect->dest1, Effect->dest2,	Effect->dest3, Effect->dest4);
		if (s!=-1)
		{
			switch( Effect->num )
			{
			case 1:
				chars[s].priv2=chars[s].priv2|0x02;
				break;
			case 2:
				chars[s].fixedlight=worldbrightlevel;
				break;
			case 3:
				chars[s].dx=chars[s].dx - Effect->more1;
				break;
			case 4:
				chars[s].in=chars[s].in - Effect->more1;
				break;
			case 5:
				chars[s].st=chars[s].st - Effect->more1;
				break;
			case 6:
				chars[s].dx=chars[s].dx + Effect->more1;
				break;
			case 7:
				chars[s].in=chars[s].in + Effect->more1;
				break;
			case 8:
				chars[s].st=chars[s].st + Effect->more1;
				break;
			case 11:
				chars[s].st=chars[s].st + Effect->more1;
				chars[s].dx=chars[s].dx + Effect->more2;
				chars[s].in=chars[s].in + Effect->more3;
				break;
			case 12:
				chars[s].st=chars[s].st - Effect->more1;
				chars[s].dx=chars[s].dx - Effect->more2;
				chars[s].in=chars[s].in - Effect->more3;
				break;
			default:
				//printf("ERROR: Fallout of switch statement without default. uox3.cpp, tempeffectson()\n"); //Morrolan
				break;
			}
		}
	} // end of if !=-1
}

void checktempeffects()
{
	int s, mortar,k;
	unsigned int j=uiCurrentTime;
	teffect_st *Effect;
	for( Effect = Effects->First(); !Effects->AtEnd(); Effect = Effects->Next() )
	{
		if( Effect == NULL )
			continue;
		if( Effect->expiretime <= j )
		{

			s=calcCharFromSer( Effect->dest1, Effect->dest2, Effect->dest3, Effect->dest4);
			if( ( Effect->num != 9 ) && ( Effect->num != 10 ) && ( Effect->num != 17 ) )
			{
				//Added by TANiS to fix errors, memory corruption and door auto-close 10-6-98
				// Check to see if it's a dead char and delete the wrong effect, or if it's just
				// a door auto-close effect and process it the right way.
				if( ( ( s < 0 ) || ( s >=cmem ) ) && ( Effect->num != 14 ) && ( Effect->num != 13 ) && ( Effect->num != 9 ) && ( Effect->num != 10 ) )
				{ 
					//LogMessage("checktempeffects() - char overflow s (%i) i (%i)\n" _ s _ i);
					//LogMessage("teffects num = (%i)\n" _ teffects[i].num);
					//LogMessage("Fixing on the fly..\n");
					
					Effects->DelCurrent();
					break;
				} //End of TANiS' change
			}
			//if (s != -1)
			switch( Effect->num )
			{
			case 1:
				if (chars[s].priv2&0x02)
				{
					chars[s].priv2=chars[s].priv2&0xFD;
					s=calcSocketFromChar(s);
					if (s!=-1) sysmessage(s, "You are no longer frozen.");
				}
				break;
			case 2:
				chars[s].fixedlight=255;
				dolight(calcSocketFromChar(s), worldbrightlevel);
				break;
			case 3:
				chars[s].dx = chars[s].dx + Effect->more1;
				statwindow(calcSocketFromChar(s), s);
				break;
			case 4:
				chars[s].in=chars[s].in + Effect->more1;
				statwindow(calcSocketFromChar(s), s);
				break;
			case 5:
				chars[s].st = chars[s].st + Effect->more1;
				statwindow(calcSocketFromChar(s), s);
				break;
			case 6:
				chars[s].dx=chars[s].dx - Effect->more1;
				chars[s].stm=min(chars[s].stm, chars[s].dx);
				statwindow(calcSocketFromChar(s), s);
				break;
			case 7:
				chars[s].in = chars[s].in - Effect->more1;
				chars[s].mn=min(chars[s].mn, chars[s].in);
				statwindow(calcSocketFromChar(s), s);
				break;
			case 8:
				chars[s].st = chars[s].st - Effect->more1;
				chars[s].hp=min(chars[s].hp, chars[s].st);
				statwindow(calcSocketFromChar(s), s);
				break;
			case 9:
				switch( Effect->more1)
				{
				case 0:
					if (Effect->more2!=0)
					{
						sprintf(temp, "*%s continues grinding.*", chars[s].name);
						npcemoteall(s, temp,1);
					}
					soundeffect2(s, 0x02, 0x42);
					break;
				}
				break;
			case 10:
				s = calcCharFromSer( Effect->sour1, Effect->sour2, Effect->sour3, Effect->sour4);
				mortar = calcItemFromSer( Effect->dest1, Effect->dest2, Effect->dest3, Effect->dest4);
				Skills->CreatePotion(s, Effect->more1, Effect->more2, mortar);
				break;
			case 11:
				chars[s].st = chars[s].st - Effect->more1;
				chars[s].hp=min(chars[s].hp, chars[s].st);
				chars[s].dx = chars[s].dx - Effect->more2;
				chars[s].stm=min(chars[s].stm, chars[s].dx);
				chars[s].in = chars[s].in - Effect->more3;
				chars[s].mn=min(chars[s].mn, chars[s].in);
				statwindow(calcSocketFromChar(s), s);
				break;
			case 12:
				chars[s].st = chars[s].st + Effect->more1;
				chars[s].dx = chars[s].dx + Effect->more2;
				chars[s].in = chars[s].in + Effect->more3;
				statwindow(calcSocketFromChar(s), s);
				break;
			case 13: // door
				mortar = calcItemFromSer( Effect->dest1, Effect->dest2, Effect->dest3, Effect->dest4);
		        if (mortar>-1) // crashfix, LB
				{
					if (items[mortar].dooropen==0) break;
					items[mortar].dooropen=0;
					dooruse(calcSocketFromChar(s), mortar);
				}
				break;
			case 14: //- training dummies Tauriel check to see if item moved or not before searching for it
				j = Effect->itemptr;
				if (items[j].ser1 == Effect->dest1 && items[j].ser2 == Effect->dest2 &&	items[j].ser3 == Effect->dest3 && items[j].ser4 == Effect->dest4)
				{
					mortar = j;
				}
				else 
					mortar=calcItemFromSer( Effect->dest1, Effect->dest2, Effect->dest3, Effect->dest4);
				if ((items[mortar].id1==0x10) && (items[mortar].id2==0x71))
				{
					items[mortar].id2=0x70;
					items[mortar].gatetime=0;
					//for (j=0;j<now;j++) if (perm[j]) senditem(j,mortar);
					RefreshItem(mortar);//AntiChrist
				} else if ((items[mortar].id1==0x10) && (items[mortar].id2==0x75))
				{
					items[mortar].id2=0x74;
					items[mortar].gatetime=0;
					//for (j=0;j<now;j++) if (perm[j]) senditem(j,mortar); 
					RefreshItem(mortar);//AntiChrist
				} 
				break;
			case 15: //reactive armor
				chars[s].ra=0;
				break;
			case 16: //Explosion potion messages  Tauriel
				// Abaddon fix for sysmessage spamming (I hope) 12th November 1999
	  			s = calcCharFromSer( Effect->sour1, Effect->sour2, Effect->sour3, Effect->sour4);
				if( chars[s].antispamtimer < uiCurrentTime )
				{
				  chars[s].antispamtimer = uiCurrentTime + (CLOCKS_PER_SEC*1);	// was >>1, now is *
				  sprintf(temp, "%i", Effect->more3);
					sysmessage(calcSocketFromChar(s), temp); // crashfix, LB
				} 
				break;
			case 17: //Explosion potion explosion  Tauriel
			  s=calcCharFromSer( Effect->sour1, Effect->sour2, Effect->sour3, Effect->sour4);
			  explodeitem(calcSocketFromChar(s), Effect->itemptr); //explode this item
				break;
			case 18: //Polymorph spell by AntiChrist
				//printf("polymorph effect finished\n");
				chars[s].id1=chars[s].orgid1;
				chars[s].id2=chars[s].orgid2;
				teleport(s);
				break;
			case 19: //Incognito spell by AntiChrist
//				printf("INCOGNITO SPELL FINISHED!!\n");
				int serhash,ci,serial;
				
				// ------ SEX ------
				chars[s].id2=chars[s].orgid2;
				
				// ------ NAME -----
				strcpy(chars[s].name,chars[s].orgname);
				
				serial=chars[s].serial;
				serhash=serial%HASHMAX;
				
				for (ci=0;ci<contsp[serhash].max;ci++)
				{
					j=contsp[serhash].pointer[ci];
					if (j!=-1) {
						// ------ HAIR -----
						if(items[j].layer==0x0B) { //change hair style/color
							//printf("HAIR FOUND!!\n");
							//stores old hair values
							items[j].color1=chars[s].haircolor1;
							items[j].color2=chars[s].haircolor2;
							items[j].id1=chars[s].hairstyle1;
							items[j].id2=chars[s].hairstyle2;
							//wornitems(calcSocketFromChar(s), s);
							//teleport(currchar[s]);
							//impowncreate(calcSocketFromChar(s), s, 0);
						}
						
						// -------- BEARD --------
						// only if a men :D
						if(chars[s].id2==0x90)
							if(items[j].layer==0x10) { //change beard style/color
								//printf("BEARD FOUND!!\n");
								//stores old beard values
								items[j].color1=chars[s].beardcolor1;
								items[j].color2=chars[s].beardcolor2;
								items[j].id1=chars[s].beardstyle1;
								items[j].id2=chars[s].beardstyle2;
								//wornitems(calcSocketFromChar(s), s);
								//teleport(currchar[s]);
								//impowncreate(calcSocketFromChar(s), s, 0);
							}
					}
				}
				wornitems(calcSocketFromChar(s), s);
				teleport(currchar[s]);
				impowncreate(calcSocketFromChar(s), s, 0);
				break;
				
			case 20: // LSD potions, LB 5'th nov 1999
				k=calcSocketFromChar(s);
				if (k==-1) return;
				LSD[k]=0;
				sysmessage(k,"LSD has worn off");
				all_items(k); // absolutely necassairy here, AC !!!
				chars[s].stm=3; // stamina near 0
				chars[s].mn=3;
				chars[s].hp=chars[s].hp/7;
				impowncreate(k,s,0);
				break;
	        case 21:
				  int toDrop;
				  toDrop = Effect->more1;
				  if( ( chars[s].baseskill[PARRYING] - toDrop ) < 0 )
					  chars[s].baseskill[PARRYING] = 0;
				  else
					  chars[s].baseskill[PARRYING] -= toDrop;
				  break;
			case 22:	// heal
			case 23:	// resurrect
			case 24:	// cure
				int src, targ, srcSock, targSock;
				short int newHealth;
				src = calcCharFromSer( Effect->sour1, Effect->sour2, Effect->sour3, Effect->sour4 );
				targ = calcCharFromSer( Effect->dest1, Effect->dest2, Effect->dest3, Effect->dest4 );
				j = Effect->itemptr;
				if( src != -1 && targ != -1 )
				{
					srcSock = calcSocketFromChar( src );
					targSock = calcSocketFromChar( targ );
					if( Effect->num == 22 )
					{
						newHealth = chars[targ].hp + ( chars[src].skill[ANATOMY] / 50 + RandomNum( 3, 10 ) + RandomNum( chars[src].skill[HEALING] / 50, chars[src].skill[HEALING] / 20 ) );
						chars[targ].hp = min( (short)chars[targ].st, newHealth );
						updatestats( targ, 0 );
						sysmessage( srcSock, "You apply the bandages and the patient looks a bit healthier." );
					}
					else if( Effect->num == 23 )
					{
						Targ->NpcResurrectTarget( targ );
						sysmessage( srcSock, "You successfully resurrected the patient" );
					}
					else
					{
						chars[targ].poisoned = 0;
						if( targSock != -1 )
						{
							staticeffect( targSock, 0x37, 0x3A, 0, 15);
							soundeffect2( targSock, 0x01, 0xE0); //cure sound - SpaceDog
							sysmessage(   targSock, "You have been cured of poison.");
							impowncreate( targSock, targ, 1 );
						}
						if( srcSock != -1 )
							sysmessage( srcSock,  "You have cured the poison.");
					}
				}
				if( items[j].amount > 1 )
				{
					items[j].amount--;
				}
				else 
					Items->DeleItem( j );
				break;
			case 25:
				chars[s].usepotion = 0;
				break;
			case 26:
				src = calcCharFromSer( Effect->sour1, Effect->sour2, Effect->sour3, Effect->sour4 );
				targ = calcCharFromSer( Effect->dest1, Effect->dest2, Effect->dest3, Effect->dest4 );
				Magic->playSound( src, 43 );
				Magic->doMoveEffect( 43, targ, src );
				Magic->doStaticEffect( targ, 43 );
				Magic->MagicDamage( targ, Effect->more1, src );				
				break;
			default:
				printf("ERROR: Fallout of switch statement without default. uox3.cpp, checktempeffects()\n"); //Morrolan
				return;
			}
   
			Effects->DelCurrent();
			Items->CheckEquipment( s ); //AntiChrist - checks equipments for stats requirements
		}
	}
}


char tempeffect(int source, int dest, int num, char more1, char more2, char more3, int targItemPtr )
{
	int ic; // antichrist' changes
	
	teffect_st toAdd;
	teffect_st *Effect;

	long sourSer;
	
	toAdd.sour1 = chars[source].ser1;
	toAdd.sour2 = chars[source].ser2;
	toAdd.sour3 = chars[source].ser3;
	toAdd.sour4 = chars[source].ser4;
	sourSer = chars[source].serial;
	toAdd.dest1 = chars[dest].ser1;
	toAdd.dest2 = chars[dest].ser2;
	toAdd.dest3 = chars[dest].ser3;
	toAdd.dest4 = chars[dest].ser4;
	for( Effect = Effects->First(), ic = 0; !Effects->AtEnd(); Effect = Effects->Next(), ic++ )
	{
		if( Effect->dest1 == chars[dest].ser1 && Effect->dest2 == chars[dest].ser2 && Effect->dest3 == chars[dest].ser3 && Effect->dest4 == chars[dest].ser4)
		{
			if( Effect->num == num )
			{
				switch( Effect->num )
				{
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 11:
				case 12:
				case 18:
				case 19:
				case 21:
					reverse_effect( ic );
					break;
				default:
					break;
				}
			}
		}
	}
	
	switch (num)
	{
	case 1:
		chars[dest].priv2=chars[dest].priv2|0x02;
		toAdd.expiretime=uiCurrentTime+((chars[source].skill[MAGERY]/100)*CLOCKS_PER_SEC);
		toAdd.num=1;
		toAdd.more1=0;
		toAdd.more2=0;
		toAdd.dispellable=1;
		break;
	case 2:
		chars[dest].fixedlight=worldbrightlevel;
		dolight(calcSocketFromChar(dest), worldbrightlevel);
		// Changed to allow Night Sight to last for 1 full day ( 24 hour period = 1440 minutes)
		toAdd.expiretime=uiCurrentTime+(1440*(((secondsperuominute/60)+1)*CLOCKS_PER_SEC));
		toAdd.num=2;
		toAdd.more1=0;
		toAdd.more2=0;
		toAdd.dispellable=1;
		break;
	case 3:
		if (chars[dest].dx<more1)
			more1=chars[dest].dx;
		chars[dest].dx=chars[dest].dx-more1;
		chars[dest].stm=min(chars[dest].stm, chars[dest].dx);
		statwindow(calcSocketFromChar(dest), dest);
		toAdd.expiretime=uiCurrentTime+((chars[source].skill[MAGERY]/10)*CLOCKS_PER_SEC);
		toAdd.num=3;
		toAdd.more1=more1;
		toAdd.more2=0;
		toAdd.dispellable=1;
		break;
	case 4:
		if (chars[dest].in<more1)
			more1=chars[dest].in;
		chars[dest].in=chars[dest].in-more1;
		chars[dest].mn=min(chars[dest].mn, chars[dest].in);
		statwindow(calcSocketFromChar(dest), dest);
		toAdd.expiretime=uiCurrentTime+((chars[source].skill[MAGERY]/10)*CLOCKS_PER_SEC);
		toAdd.num=4;
		toAdd.more1=more1;
		toAdd.more2=0;
		toAdd.dispellable=1;
		break;
	case 5:
		if (chars[dest].st<more1)
			more1=chars[dest].st;
		chars[dest].st=chars[dest].st-more1;
		chars[dest].hp=min(chars[dest].hp, chars[dest].st);
		statwindow(calcSocketFromChar(dest), dest);
		toAdd.expiretime=uiCurrentTime+((chars[source].skill[MAGERY]/10)*CLOCKS_PER_SEC);
		toAdd.num=5;
		toAdd.more1=more1;
		toAdd.more2=0;
		toAdd.dispellable=1;
		break;
	case 6:
		if (chars[dest].dx+more1>255)
			more1=chars[dest].dx-255;
		chars[dest].dx=chars[dest].dx+more1;
		statwindow(calcSocketFromChar(dest), dest);
		toAdd.expiretime=uiCurrentTime+((chars[source].skill[MAGERY]/10)*CLOCKS_PER_SEC);
		toAdd.num=6;
		toAdd.more1=more1;
		toAdd.more2=0;
		toAdd.dispellable=1;
		break;
	case 7:
		if (chars[dest].in+more1>255)
			more1=chars[dest].in-255;
		chars[dest].in=chars[dest].in+more1;
		statwindow(calcSocketFromChar(dest), dest);
		toAdd.expiretime=uiCurrentTime+((chars[source].skill[MAGERY]/10)*CLOCKS_PER_SEC);
		toAdd.num=7;
		toAdd.more1=more1;
		toAdd.more2=0;
		toAdd.dispellable=1;
		break;
	case 8:
		if (chars[dest].st+more1>255)
			more1=chars[dest].st-255;
		chars[dest].st=chars[dest].st+more1;
		statwindow(calcSocketFromChar(dest), dest);
		toAdd.expiretime=uiCurrentTime+((chars[source].skill[MAGERY]/10)*CLOCKS_PER_SEC);
		toAdd.num=8;
		toAdd.more1=more1;
		toAdd.more2=0;
		toAdd.dispellable=1;
		break;
	case 9:
		toAdd.expiretime=uiCurrentTime+(more2*CLOCKS_PER_SEC);
		toAdd.num=9;
		toAdd.more1=more1;
		toAdd.more2=more2;
		toAdd.dispellable=0;
		break;
	case 10:
		toAdd.expiretime=uiCurrentTime+(12*CLOCKS_PER_SEC);
		toAdd.dispellable=0;
		toAdd.more1=more1;
		toAdd.more2=more2;
		toAdd.num=10;
		break;
	case 11: // Bless
		if (chars[dest].st+more1>255)
			more1=chars[dest].st-255;
		if (chars[dest].dx+more2>255)
			more2=chars[dest].dx-255;
		if (chars[dest].in+more3>255)
			more3=chars[dest].in-255;
		chars[dest].st=chars[dest].st+more1;
		chars[dest].dx=chars[dest].dx+more2;
		chars[dest].in=chars[dest].in+more3;
		statwindow(calcSocketFromChar(dest), dest);
		toAdd.expiretime=uiCurrentTime+((chars[source].skill[MAGERY]/10)*CLOCKS_PER_SEC);
		toAdd.num=11;
		toAdd.more1=more1;
		toAdd.more2=more2;
		toAdd.more3=more3;
		toAdd.dispellable=1;
		break;
	case 12: // Curse
		if (chars[dest].st<more1)
			more1=chars[dest].st;
		if (chars[dest].dx<more2)
			more2=chars[dest].dx;
		if (chars[dest].in<more3)
			more3=chars[dest].in;
		chars[dest].st=chars[dest].st-more1;
		chars[dest].dx=chars[dest].dx-more2;
		chars[dest].in=chars[dest].in-more3;
		statwindow(calcSocketFromChar(dest), dest);
		toAdd.expiretime=uiCurrentTime+((chars[source].skill[MAGERY]/10)*CLOCKS_PER_SEC);
		toAdd.num=12;
		toAdd.more1=more1;
		toAdd.more2=more2;
		toAdd.more3=more3;
		toAdd.dispellable=1;
		break;
	case 15: // Reactive armor
		toAdd.expiretime=uiCurrentTime+((chars[source].skill[MAGERY]/10)*CLOCKS_PER_SEC);
		toAdd.num=15;
		toAdd.dispellable=1;
		break;
	case 16: //Explosion potions  Tauriel
		toAdd.expiretime=uiCurrentTime+(more2*CLOCKS_PER_SEC);
		toAdd.num=16;
		toAdd.more1=more1; //item/potion
		toAdd.more2=more2; //seconds
		toAdd.more3=more3; //countdown#
		toAdd.dispellable=0;
		break;
	case 18:	// Polymorph - Antichrist 09/99
		toAdd.expiretime=uiCurrentTime+(server_data.polyduration*CLOCKS_PER_SEC);
		toAdd.num=18;
		toAdd.dispellable=0;
		
		int c1, b, k;
		// Grey flag when polymorphed - AntiChrist (9/99)
		chars[dest].crimflag=(server_data.polyduration*CLOCKS_PER_SEC)+uiCurrentTime;
		if( chars[dest].onhorse) 
			k = unmounthorse(dest);
		k = (more1<<8)+more2;
		
		if (k>=0x000 && k<=0x3e1) // lord binary, body-values >0x3e crash the client
		{ 
			chars[dest].xid1=chars[dest].id1=k>>8; // allow only non crashing ones
			chars[dest].xid2=chars[dest].id2=k%256;	 
			
			c1=(chars[dest].skin1<<8)+chars[dest].skin2; // transparency for monsters allowed, not for players, 
			// if polymorphing from monster to player we have to switch from transparent to semi-transparent
			b=c1&0x4000; 
			if (b==16384 && (k >=0x0190 && k<=0x03e1))
			{
				if (c1!=0x8000) {
					chars[dest].skin1=chars[dest].xskin1=0xF0;
					chars[dest].skin2=chars[dest].xskin2=0;
				}
			}
		}
		break;
	case 19: // incognito spell - AntiChrist (10/99)
		toAdd.expiretime = uiCurrentTime + 90 * CLOCKS_PER_SEC; // 90 seconds
		toAdd.num = 19;
		toAdd.dispellable = 0;
		break;
		
	case 20: // LSD potions, LB 5'th November 1999
		k = calcSocketFromChar( source );
		if( k == -1 )
			return 0;
		sysmessage( k, "Hmmm, tasty, LSD, you feel ... strong ... and .. allmighty and ... strange ..." );
		LSD[k] = 1;
		toAdd.expiretime = uiCurrentTime + 90 * CLOCKS_PER_SEC; // 90 seconds
		toAdd.num = 20;
		toAdd.dispellable = 0;
		chars[source].hp = chars[source].st;
		chars[source].mn = chars[source].in;
		impowncreate( k, source, 0 );
		break;
	case 21:		// protection
		toAdd.expiretime = uiCurrentTime + 120 * CLOCKS_PER_SEC;
		toAdd.dispellable=1;
		toAdd.more1=more1;
		toAdd.num=21;
		chars[dest].baseskill[PARRYING] += more1;
		break;
	case 22:		// healing skill, normal heal
	case 23:		// healing skill, resurrect
	case 24:		// healing skill, cure
		char temp[100];
		toAdd.num = num;
		teffect_st *Test;
		long checkChar;
		long oldTarg;
		for( Test = Effects->First(); !Effects->AtEnd(); Test = Effects->Next() )	// definitely not friendly and scalable, but it stops all prior healing attempts
		{	// another option would be to do a bit set, to specify already healing
			if( Test == NULL )
				continue;
			switch( Test->num )
			{
			case 22:
			case 23:
			case 24:
				checkChar = calcserial( Test->sour1, Test->sour2, Test->sour3, Test->sour4 );
				if( checkChar == sourSer )
				{
					oldTarg = calcCharFromSer( calcserial( Test->dest1, Test->dest2, Test->dest3, Test->dest4 ) );
					if( Test->num == 22 )
						sprintf( temp, "*%s breaks off healing %s*", chars[source].name, chars[oldTarg].name );
					else if( Test->num == 23 )
						sprintf( temp, "*%s breaks off the attempt to resurrect %s*", chars[source].name, chars[oldTarg].name );
					else if( Test->num == 24 )
						sprintf( temp, "*%s breaks off the curing of %s*", chars[source].name, chars[oldTarg].name );
					npcemoteall( source, temp, 0 );
					Effects->DelCurrent();		// we're already involved in some form of healing, BREAK IT
				}
				break;
			default:
				break;
			}
		}
		if( num == 22 )
		{
			if( dest == source )
				toAdd.expiretime = uiCurrentTime + ( 13 * CLOCKS_PER_SEC ) + ( ( 5 - RandomNum( 0, chars[source].skill[HEALING] / 200 ) ) * CLOCKS_PER_SEC );
			else
				toAdd.expiretime = uiCurrentTime + ( 4 * CLOCKS_PER_SEC ) + ( ( 3 - RandomNum( 0, chars[source].skill[HEALING] / 333 ) ) * CLOCKS_PER_SEC );
			sprintf( temp, "*%s begins to heal %s*", chars[source].name, chars[dest].name );
			npcemoteall( source, temp, 1 );
		}
		else if( num == 23 )
		{
			toAdd.expiretime = uiCurrentTime + ( 15 * CLOCKS_PER_SEC );
			sprintf( temp, "*%s begins to resurrect %s*", chars[source].name, chars[dest].name );
			npcemoteall( source, temp, 1 );
		}
		else if( num == 24 )
		{
			if( dest == source )
				toAdd.expiretime = uiCurrentTime + RandomNum( 15 * CLOCKS_PER_SEC, 18 * CLOCKS_PER_SEC );
			else
				toAdd.expiretime = uiCurrentTime + ( 6 * CLOCKS_PER_SEC );
			sprintf( temp, "*%s begins to cure %s*", chars[source].name, chars[dest].name );
			npcemoteall( source, temp, 1 );
		}
		toAdd.dispellable = 0;
		toAdd.itemptr = targItemPtr;	// the bandage we are using to achieve this
		toAdd.more1 = more1;			// the skill we end up using (HEALING for players, VETERINARY for monsters)
		break;
	case 25:
		toAdd.expiretime = uiCurrentTime + CLOCKS_PER_SEC * server_data.potiondelay;
		toAdd.num = 25;
		chars[dest].usepotion = 1;
		break;
	case 26:
		toAdd.expiretime = (unsigned int)( uiCurrentTime + CLOCKS_PER_SEC * combat.explodeDelay);
		toAdd.num = 26;
		toAdd.more1 = more1;
		break;
	default:
		printf("ERROR: Fallout of switch statement (%d) without default. uox3.cpp, tempeffect()\n", num ); //Morrolan
		return 0;
	}
	Effects->Add( toAdd );
	return 1;
}

char tempeffect2(int source, int dest, int num, char more1, char more2, char more3, int targItemPtr )
// PARAM WARNING: more3 is never referenced
{
	teffect_st toAdd;
	toAdd.sour1=chars[source].ser1;
	toAdd.sour2=chars[source].ser2;
	toAdd.sour3=chars[source].ser3;
	toAdd.sour4=chars[source].ser4;
	toAdd.dest1=items[dest].ser1;
	toAdd.dest2=items[dest].ser2;
	toAdd.dest3=items[dest].ser3;
	toAdd.dest4=items[dest].ser4;
	switch (num)
	{
	case 10:
		toAdd.expiretime = (unsigned int)(uiCurrentTime+(12*CLOCKS_PER_SEC));
		toAdd.dispellable=0;
		toAdd.more1=more1;
		toAdd.more2=more2;
		toAdd.num=10;
		break;
	case 13:
		if (items[dest].dooropen)
		{
			items[dest].dooropen=0;
			return 0;
		}
		toAdd.expiretime=uiCurrentTime + 10 * CLOCKS_PER_SEC;
		toAdd.num=13;
		toAdd.dispellable=0;
		items[dest].dooropen=1;
		break;
	case 14: //Tauriel training dummies swing for 5(?) seconds
		toAdd.expiretime = uiCurrentTime + 5 * CLOCKS_PER_SEC;
		toAdd.num=14;
		toAdd.dispellable=0;
		toAdd.itemptr=dest; //used to try and cut search time down
		toAdd.more2=0;
		break;
	case 17: //Explosion potion (explosion)  Tauriel (explode in 4 seconds)
		toAdd.expiretime = uiCurrentTime + 4 * CLOCKS_PER_SEC;
		toAdd.num=17;
		toAdd.more1=more1;
		toAdd.more2=more2;
		toAdd.itemptr=dest;
		toAdd.dispellable=0;
		break;
	default:
		printf("ERROR: Fallout of switch statement without default. uox3.cpp, tempeffect2()\n"); //Morrolan
		return 0;
	}
	Effects->Add( toAdd );
	return 1;
}

char indungeon(int s)
{
	int x1;
	if (chars[s].x<5119) 
		return 0;
	x1 = (chars[s].x-5119)>>8;

	switch( chars[s].y>>8 )
	{
	case 0:
		return 1;
	case 1:
		if (x1 != 0) 
			return 1;
		else
			return 0;
	case 2:
	case 3:
		if (x1 < 3) 
			return 1;
		else
			return 0;
	case 4:
		if (x1 == 0) 
			return 1;
		return 0;
	case 5:
		return 1;
	case 6:
		if ( x1 == 0 )
			return 1;
		else
			return 0;
	case 7:
		if (x1 < 2)
			return 1;
		return 0;
	}
	return 0;
}

void npcattacktarget( CHARACTER target, CHARACTER source )
{	
	if( source == target ) 
		return;
	if( source < 0 || target < 0 || source > cmem || target > cmem ) 
		return;
	if( chars[source].dead || chars[target].dead )
		return;
	unsigned short srcX = chars[source].x;
	unsigned short srcY = chars[source].y;
	signed char    srcZ = chars[source].z;
	unsigned short trgX = chars[target].x;
	unsigned short trgY = chars[target].y;
	signed char    trgZ = chars[target].z;
	if( !line_of_sight( -1, trgX, trgY, trgZ, srcX, srcY, srcZ, WALLS_CHIMNEYS + DOORS + FLOORS_FLAT_ROOFING ) )
		return;	// no line of sight
	playmonstersound( source, chars[source].id1, chars[source].id2, SND_STARTATTACK );

	int i;
#ifdef __COMBAT_DEBUG__
	if( chars[source].targ != -1 )
		throw "Holy shit on a stick, you're changing the target of a npc with a target!";
#endif
	chars[source].targ = target;
	chars[source].attacker = target;
	chars[source].attackfirst = 1;

	// The way the code was working was that if the thing being attacked (B) had a target (C), then it would check the attacker (A)
	// to see if the distance from B->A was less than B->C.  If B->A is less than B->C, it changed targets
	// I yanked this code, as a test
	// B only attacks A if it doesn't already have a target
	bool returningAttack = false;
	if( chars[target].targ == -1 )
	{
		if( chars[target].npcaitype != 17 && !(chars[target].priv&0x04) )	// invulnerable and player vendors don't fight back!
		{
			chars[target].targ = source;
			chars[target].attacker = source;
			chars[target].attackfirst = 0;
			returningAttack = true;
		}
	}
	// If Attacker is hidden, show them!
	if( chars[source].hidden && !( chars[source].priv2&8 ) )
	{
		chars[source].hidden = 0;
		chars[source].stealth = -1;
		updatechar( source );
	}
	// If attacker is meditating, break it!
	if( chars[source].med )
		chars[source].med = 0; //Morrolan - Meditation

	// Only unhide the defender, if they're going to return the attack (otherwise they're doing nothing!)
	if( returningAttack )
	{
		if( chars[target].hidden && !( chars[target].priv2&8 ) )
		{
			chars[target].hidden = 0;
			chars[target].stealth = -1;
			updatechar( target );
		}
		if( chars[target].med )
			chars[target].med = 0;
	}
	// if the source is an npc, make sure they're in war mode and reset their movement time
	if( chars[source].npc )
	{
		if( !chars[source].war ) 
			npcToggleCombat( source );
		chars[source].npcmovetime = (unsigned int)(uiCurrentTime+(double)(NPCSPEED*CLOCKS_PER_SEC));
	}
	// if the target is an npc, and not a guard, make sure they're in war mode and update their movement time
	// ONLY IF THEY'VE CHANGED ATTACKER
	if( returningAttack && chars[target].npc && chars[target].npcaitype != 4 )
	{
		if( !chars[target].war )
			npcToggleCombat( target );
		chars[target].npcmovetime = (unsigned int)(uiCurrentTime+(double)(NPCSPEED*CLOCKS_PER_SEC));
	}

	sprintf( temp, "You see %s attacking %s!", chars[source].name, chars[target].name );

	int gCompare, rCompare;
	gCompare = Guilds->Compare( source, target );
	rCompare = Races->Compare( source, target );
	
	// If the target is an innocent, not a racial or guild ally/enemy, then flag them as criminal
	// and, of course, call the guards ;>
	if( (chars[target].flag&0x04) && gCompare == 0 && rCompare == 0 )
	{
		bool regionGuarded = ( ( region[chars[target].region].priv&0x01 ) == 0x01 );
		if( server_data.guardsactive && regionGuarded )
		{
			if( chars[target].npc && chars[target].npcaitype != 4 )
				npctalkall( target, "Help! Guards! I've been attacked!", 1 ); // AntiChrist 11/19/99 :)
			criminal( source );
#ifdef DEBUG
			//	EviLDeD -	March 1, 2000
			//	Debugging messages
			printf( "DEBUG: [npcattacktarget)] %s is being set to criminal\n", chars[source].name );
			//	EviLDeD -	End
#endif
		}
	}
	// was emoting for the source npc
	for( i = 0; i < now; i++ )
	{
		if( inrange1p( currchar[i], source ) && perm[i] )
		{
			npcemote( i, currchar[i], temp, 1 );
		}
	}
}


void npcsimpleattacktarget( CHARACTER defender, CHARACTER attacker )
{
	if( attacker < 0 || defender < 0 || attacker > cmem || defender > cmem )
		return;
	if( ( chars[attacker].targ == defender ) && ( chars[defender].targ == attacker ) ) 
		return;
	if( chars[attacker].dead || chars[defender].dead ) 
		return;
	chars[attacker].targ = defender;
	chars[attacker].attacker = defender;
	chars[attacker].attackfirst = 1;
	if( chars[defender].targ == -1 )
	{
		chars[defender].targ = attacker;
		chars[defender].attacker = attacker;
		chars[defender].attackfirst=0;
	}
	if( chars[attacker].hidden && ( !( chars[attacker].priv2&8 ) ) )
	{
		chars[attacker].hidden = 0;
		chars[attacker].stealth = -1;
		updatechar( attacker );
	}
	if( chars[attacker].med )
	{
		chars[attacker].med = 0; //Morrolan - Meditation
	}
	if( chars[defender].hidden && ( !(chars[defender].priv2&8 ) ) )
	{
		chars[defender].hidden = 0;
		chars[defender].stealth = -1;
		updatechar( defender );
	}
	if( chars[defender].med )
	{
		chars[defender].med = 0; //Morrolan - Meditation
	}
	if( chars[attacker].npc )
	{
		if( !chars[attacker].war  ) 
			npcToggleCombat( attacker );
		chars[attacker].npcmovetime = (unsigned int)( uiCurrentTime + (double)( NPCSPEED * CLOCKS_PER_SEC ) );
	}
	if( chars[defender].npc && chars[defender].npcaitype != 4 ) // changed from 0x40 to 4, LB
	{
		if( !chars[defender].war ) 
			npcToggleCombat( defender );
		chars[defender].npcmovetime = (unsigned int)( uiCurrentTime + (double)( NPCSPEED * CLOCKS_PER_SEC ) );
	}
//	chars[target2].timeout = uiCurrentTime + ( Combat->GetSwingRate( target2, -1 ) ) * CLOCKS_PER_SEC;
//	chars[target].timeout = uiCurrentTime + ( Combat->GetSwingRate( target, -1 ) ) * CLOCKS_PER_SEC;
}


void openbank(int s, int i)
{
	int j,c,serial,serhash,ci;
	serial=chars[i].serial;
	serhash=serial%HASHMAX;

  for (ci=0;ci<ownsp[serhash].max;ci++)
	{
		j=ownsp[serhash].pointer[ci];
		if (j!=-1)
		{
			if (items[j].ownserial==serial &&
				items[j].type==1 && items[j].morex==1)
			{
				if( server_data.usespecialbank )
				{
					if( items[j].morey == 0 && items[j].morez == 0 ) // if not initialized yet for the special bank
					{
						items[j].morey = 123; // convert to new special bank
					}
				}
				if( server_data.usespecialbank )
				{
					if( items[j].morey == 123 ) // check if a goldbank
					{
						wearitem[1]=items[j].ser1;
						wearitem[2]=items[j].ser2;
						wearitem[3]=items[j].ser3;
						wearitem[4]=items[j].ser4;
						wearitem[5]=items[j].id1;
						wearitem[6]=items[j].id2;
						wearitem[8]=items[j].layer;
						wearitem[9]=items[j].cont1;
						wearitem[10]=items[j].cont2;
						wearitem[11]=items[j].cont3;
						wearitem[12]=items[j].cont4;
						wearitem[13]=items[j].color1;
						wearitem[14]=items[j].color2;
						Network->xSend(s, wearitem, 15, 0);
						backpack(s, items[j].ser1, items[j].ser2, items[j].ser3, items[j].ser4);
						return;
					}
				} else// else if not using specialbank
				{ // don't check for goldbank
					wearitem[1]=items[j].ser1;
					wearitem[2]=items[j].ser2;
					wearitem[3]=items[j].ser3;
					wearitem[4]=items[j].ser4;
					wearitem[5]=items[j].id1;
					wearitem[6]=items[j].id2;
					wearitem[8]=items[j].layer;
					wearitem[9]=items[j].cont1;
					wearitem[10]=items[j].cont2;
					wearitem[11]=items[j].cont3;
					wearitem[12]=items[j].cont4;
					wearitem[13]=items[j].color1;
					wearitem[14]=items[j].color2;
					Network->xSend(s, wearitem, 15, 0);
					backpack(s, items[j].ser1, items[j].ser2, items[j].ser3, items[j].ser4);
					return;
				}
				
			}
		}
	} // end of !=-1
	
	sprintf(temp, "%s's bank box.", chars[i].name);
	c=Items->SpawnItem(s,1,temp,0,0x09,0xAB,0,0,0,0);
	if( c == -1 )
		return;
	
	items[c].layer=0x1d;
	setserial(c,i,3);
	setserial(c,i,4);
	items[c].morex=1;
	if( server_data.usespecialbank ) // AntiChrist - Special Bank
		items[c].morey = 123; // gold only bank
	items[c].type=1;
	wearitem[1]=items[c].ser1;
	wearitem[2]=items[c].ser2;
	wearitem[3]=items[c].ser3;
	wearitem[4]=items[c].ser4;
	wearitem[5]=items[c].id1;
	wearitem[6]=items[c].id2;
	wearitem[8]=items[c].layer;
	wearitem[9]=items[c].cont1;
	wearitem[10]=items[c].cont2;
	wearitem[11]=items[c].cont3;
	wearitem[12]=items[c].cont4;
	wearitem[13]=items[c].color1;
	wearitem[14]=items[c].color2;
	Network->xSend(s, wearitem, 15, 0);
	backpack(s, items[c].ser1, items[c].ser2, items[c].ser3, items[c].ser4);
}

//
//special bank - AntiChrist
//
//If activated, you can only put golds into normal banks
//and there are special banks (for now we still use normal bankers,
//but u have to say the SPECIALBANKTRIGGER word to open it)
//where u can put all the items: one notice: the special bank
//is characteristic of regions.... so in Britain you don't find
//the items you leaved in Minoc!
//All this for increasing pk-work and commerce! :)
//( and surely the Mercenary work, so now have to pay strong
//warriors to escort u during your travels! )
//

void openspecialbank( int s, int i )
{
	int j, c, serial, serhash, ci;
	serial = chars[i].serial;
	serhash = serial%HASHMAX;
	for( ci = 0; ci < ownsp[serhash].max; ci++ )
	{
		j = ownsp[serhash].pointer[ci];
		if( j != -1 )
		{
			if( items[j].ownserial == serial &&
				items[j].type == 1 && items[j].morex == 1 &&
				items[j].morey != 123 ) // specialbank and the current region - AntiChrist
			{
				if( items[j].morez == 0 ) // convert old banks into new banks
					items[j].morez = chars[currchar[s]].region;
				if( items[j].morez == chars[currchar[s]].region )
				{
					wearitem[1] = items[j].ser1;
					wearitem[2] = items[j].ser2;
					wearitem[3] = items[j].ser3;
					wearitem[4] = items[j].ser4;
					wearitem[5] = items[j].id1;
					wearitem[6] = items[j].id2;
					wearitem[8] = items[j].layer;
					wearitem[9] = items[j].cont1;
					wearitem[10] = items[j].cont2;
					wearitem[11] = items[j].cont3;
					wearitem[12] = items[j].cont4;
					wearitem[13] = items[j].color1;
					wearitem[14] = items[j].color2;
					Network->xSend( s, wearitem, 15, 0 );
					backpack( s, items[j].ser1, items[j].ser2, items[j].ser3, items[j].ser4 );
					return;
				}
			}
		}
	} // end of != -1
	
	sprintf( temp, "%s's items bank box.", chars[i].name );
	c = Items->SpawnItem( s, 1, temp, 0, 0x09, 0xAB, 0, 0, 0, 0 );
	if( c == -1 )
		return;
	
	items[c].layer = 0x1D;
	setserial( c, i, 3 );
	setserial( c, i, 4 );
	items[c].morex = 1;
	items[c].morey = 0; // this's an all-items bank
	items[c].morez = chars[currchar[s]].region; // let's store the region
	items[c].type = 1;
	
	wearitem[1] = items[c].ser1;
	wearitem[2] = items[c].ser2;
	wearitem[3] = items[c].ser3;
	wearitem[4] = items[c].ser4;
	wearitem[5] = items[c].id1;
	wearitem[6] = items[c].id2;
	wearitem[8] = items[c].layer;
	wearitem[9] = items[c].cont1;
	wearitem[10] = items[c].cont2;
	wearitem[11] = items[c].cont3;
	wearitem[12] = items[c].cont4;
	wearitem[13] = items[c].color1;
	wearitem[14] = items[c].color2;
	Network->xSend( s, wearitem, 15, 0 );
	backpack( s, items[c].ser1, items[c].ser2, items[c].ser3, items[c].ser4 );
}


char inbankrange(int i)
{
	int j;
	if (i<0 || i>cmem) return 0;
	//for (j=0;j<charcount;j++)
	//{
	int x=chars[i].x, y=chars[i].y;//, z=chars[i].z;
	
	//Char mapRegions
	int getcell=mapRegions->GetCell(x,y);
	int mapitem=-1;
	int mapitemptr=-1;
	int mapchar=-1;
	do //check all items in this cell
	{
		mapitemptr=mapRegions->GetNextItem(getcell, mapitemptr);
		if (mapitemptr==-1) break;
		mapitem=mapRegions->GetItem(getcell, mapitemptr);
		if (mapitem>999999) mapchar=mapitem-1000000;	// this was short one 9
		if (mapitem!=-1 && mapitem>=1000000)
		{
			j=mapchar;
			if (chars[j].npcaitype&0x08)
			{
				if (chardist(i, j)<=6)
				{
					return 1;
				}
			}
		}
	} while( mapitem != -1 );
	return 0;
}

void deathaction(int s, int x) // Character does a certain action
{
	int i;
	char deathact[14] = "\xAF\x01\x02\x03\x04\x01\x02\x00\x05\x00\x00\x00\x00";
	//                     0   1   2   3   4   5   6   7   8   9   10  11  12
	deathact[1] = chars[s].ser1;
	deathact[2] = chars[s].ser2;
	deathact[3] = chars[s].ser3;
	deathact[4] = chars[s].ser4;
	deathact[5] = items[x].ser1;
	deathact[6] = items[x].ser2;
	deathact[7] = items[x].ser3;
	deathact[8] = items[x].ser4;
	deathact[12] = (char)RandomNum( 0, 1 );	// 0 = backward, 1 = forward
	for( i = 0; i < now; i++ ) 
		if( ( inrange1p( s, currchar[i] ) ) && ( perm[i] ) && ( currchar[i] != s ) ) 
			Network->xSend( i, deathact, 13, 0 );
}

void deathmenu(int s) // Character sees death menu
{
	char testact[3]="\x2C\x00";
	Network->xSend(s, testact, 2, 0);
}


int getamount(int s, unsigned char id1, unsigned char id2)
{
	int i,p,serial,serhash,ci;
	int total=0;
	
	p=packitem(s);
	if (p==-1) return 0; // LB
	serial=items[p].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if ((i > -1) && items[i].contserial==serial)
		{
			if (items[i].id1==id1 && items[i].id2==id2) total=total+items[i].amount;
			if (items[i].type==1) total=total+getsubamount(i, id1, id2);
		}
	}
	return total;
}

int getsubamount(int p, char id1, char id2)
{
	int i,serial,serhash,ci;
	int total=0;
	serial=items[p].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if ((i > -1) && items[i].contserial==serial)
		{
			if (items[i].id1==id1 && items[i].id2==id2) total=total+items[i].amount;
			if (items[i].type==1) total=total+getsubamount(i, id1, id2);
		}
	}
	return total;
}

int delequan(int s, int id1, int id2, int amount )
{
	int i, p, total, serial, serhash, ci;
	total = amount;
	p = packitem( s );
	if( p == -1 ) 
		return 0;
	serial = items[p].serial;
	serhash = serial%HASHMAX;
	for( ci = 0; ci < contsp[serhash].max; ci++ )
	{
		i = contsp[serhash].pointer[ci];
		if( i != -1 )
		{
			if( items[i].contserial == serial )
			{
				if( items[i].type == 1 )
				{
					total -= delesubquan(i, id1, id2, total);
				}
				if( items[i].id1 == id1 && items[i].id2 == id2 )
				{
					if( items[i].amount <= total )
					{
						total -= items[i].amount;
						Items->DeleItem( i );
					}
					else
					{
						items[i].amount -= total;
						total = 0;
						RefreshItem( i );
					}
				}
				if( total == 0 ) 
					return 0;
			}
		}
	}
	return total;
}

int delesubquan(int p, int id1, int id2, int amount)
{
	int i, k, serial, serhash, ci;
	int total, totaldel = 0;
	total = amount;
	serial = items[p].serial;
	serhash = serial%HASHMAX;
	for (ci = 0; ci < contsp[serhash].max; ci++)
	{
		i = contsp[serhash].pointer[ci];
		if (i>-1)
			if (items[i].contserial == serial)
			{
				if (items[i].type == 1)
				{
					k = delesubquan(i, id1, id2, total);
					total = total - k;
					totaldel = totaldel + k;
				}
				if (items[i].id1 == id1 && items[i].id2 == id2)
				{
					if (items[i].amount <= total)
					{
						total = total - items[i].amount;
						totaldel = totaldel + items[i].amount;
						Items->DeleItem(i);
					}
					else
					{
						items[i].amount = items[i].amount - total;
						totaldel = totaldel + total;
						total = 0;
						RefreshItem(i); // AntiChrist
					}
				}
				if (total == 0)
					return totaldel;
			}
	}
	return totaldel;
}




void impowncreate(int s, int i, int z) //socket, player to send
{
	int j, k,ci;
	char oc[1024];
	
	if ( (i < 0) || (i > cmem))
	{
#ifdef DEBUG
		printf("impowncreate -> i overflow. (%i)", i);
#endif
		i = 0;
	}
	
	if (s==-1) return; //lb
	
	
	//Zippy->Don't send hidden players to non GMs/Counselors...
	if ( !(chars[currchar[s]].priv&1 || chars[currchar[s]].priv&80) && (chars[i].hidden || (!chars[i].npc && !online(i))))
		return;
	//	if( chars[i].priv2&0x08 && !(chars[currchar[s]].priv&0x01) )	// causes a number of bugs (need to use /unhide on regular players)
	//		return;			// don't have privs to see!!!!!!!
	
	oc[0]=0x78; // Message type 78
	
	oc[3]=chars[i].ser1; // Character serial number
	oc[4]=chars[i].ser2; // Character serial number
	oc[5]=chars[i].ser3; // Character serial number
	oc[6]=chars[i].ser4; // Character serial number
	oc[7]=chars[i].id1; // Character art id
	oc[8]=chars[i].id2; // Character art id
	oc[9]=chars[i].x>>8;  // Character x position
	oc[10]=chars[i].x%256; // Character x position
	oc[11]=chars[i].y>>8; // Character y position
	oc[12]=chars[i].y%256; // Character y position
	if (z) oc[13]=chars[i].dispz; // Character z position
	else oc[13]=chars[i].z;
	oc[14]=chars[i].dir; // Character direction
	oc[15]=chars[i].skin1; // Character skin color
	oc[16]=chars[i].skin2; // Character skin color
	oc[17]=0; // Character flags
	if (chars[i].hidden || !(online(i)||chars[i].npc)) oc[17]=oc[17]|0x80; // Show hidden state correctly
	if(chars[i].poisoned) oc[17]=oc[17]|0x04; //AntiChrist -- thnx to SpaceDog 
	k=19;
	int guild, race;
	guild=Guilds->Compare(currchar[s],i);
	race=Races->Compare( currchar[s], i );
	if( chars[i].kills > repsys.maxkills )
		oc[18] = 6;
	else if (guild==1 || race==2)//Same guild (Green)
		oc[18]=2;
	else if (guild==2 || race==1) // Enemy guild.. set to orange
		oc[18]=5;
	else
	{
		if( chars[i].flag&0x01 )
			oc[18] = 6;
		else if( chars[i].flag&0x04 )
			oc[18] = 1;
		else if( chars[i].flag&0x08 )
			oc[18] = 2;
		else if( chars[i].flag&0x10 )
			oc[18] = 5;
		else
			oc[18] = 3;
	}
	
	
	for (j=0;j<MAXLAYERS;j++) layers[j] = 0;
	
	for (ci=0;ci<contsp[chars[i].serial%HASHMAX].max;ci++)
	{
		j=contsp[chars[i].serial%HASHMAX].pointer[ci];
		
		if (j!=-1)
			if (items[j].contserial==chars[i].serial && (!items[j].free))
			{
				if ( layers[items[j].layer] == 0 )
				{
					oc[k+0]=items[j].ser1;
					oc[k+1]=items[j].ser2;
					oc[k+2]=items[j].ser3;
					oc[k+3]=items[j].ser4;
					oc[k+4]=items[j].id1;
					oc[k+5]=items[j].id2;
					oc[k+6]=items[j].layer;
					k=k+7;
					if (items[j].color1!=0 || items[j].color2!=0)
					{
						oc[k-3]=oc[k-3]|0x80;
						oc[k+0]=items[j].color1;
						oc[k+1]=items[j].color2;
						k=k+2;
					}
					layers[items[j].layer] = 1;
				}
				else
				{
#ifdef DEBUG
					printf("Double layer (%i) on Item (%i) on Char (%i)\n", items[j].layer , j , i);
					sprintf(temp, "Double layer (%i) on Item (%2x %2x %2x %2x) on Char (%2x %2x %2x %2x)\n",
						items[j].layer, items[j].ser1, items[j].ser2, items[j].ser3, items[j].ser4,
						chars[i].ser1, chars[i].ser2, chars[i].ser3, chars[i].ser4);
					sysbroadcast(temp);
#endif
				}
			}
	}
	
	oc[k+0]=0;// Not well understood.  It's a serial number.  I set this to my serial number,
	oc[k+1]=0;// and all of my messages went to my paperdoll gump instead of my character's
	oc[k+2]=0;// head, when I was a character with serial number 0 0 0 1.
	oc[k+3]=0;
	k=k+4;
	
	oc[1]=k>>8;
	oc[2]=k%256;
	Network->xSend(s, oc, k, 0);
}

void gettokennum(char * s, int num)
{
	int i, j;
	
	for (i=0;i<255;i++)
	{
		gettokenstr[i]=0;
	}
	
	i=0;
	
	while(num!=0)
	{
		if (s[i]==0)
		{
			num=num-1;
		}
		else
		{
			if (s[i]==' ' && i!=0 && s[i-1]!=' ')
			{
				num=num-1;
			}
			i++;
		}
	}
	j=0;
	while(num!=-1)
	{
		if (s[i]==0)
		{
			num=num-1;
		}
		else
		{
			if (s[i]==' ' && i!=0 && s[i-1]!=' ')
			{
				num=num-1;
			}
			else
			{
				gettokenstr[j]=s[i];
				j++;
			}
			i++;
		}
	}
}



void setrandomname(int s, char * namelist)
{
	char sect[512];
	int i=0,j=0;
	openscript("npc.scp");
	
	sprintf(sect, "RANDOMNAME %s", namelist);
	
	if (!i_scripts[npc_script]->find(sect))
	{
		sprintf(chars[s].name, "Error Namelist %s Not Found", namelist);
		closescript();
		if (n_scripts[custom_npc_script][0]!=0)
		{
			openscript(n_scripts[custom_npc_script]);
			if (!i_scripts[custom_npc_script]->find(sect))
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
	sprintf(chars[s].name,"namecount %i",i);
	if(i>0)
	{
		i=rand()%(i);
		openscript("npc.scp");
		if(!i_scripts[npc_script]->find(sect))
		{
			sprintf(chars[s].name, "Error Namelist %s Not Found", namelist);
			closescript();
			if (n_scripts[custom_npc_script][0]!=0)
			{
				openscript(n_scripts[custom_npc_script]);
				if (!i_scripts[custom_npc_script]->find(sect))
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
					strcpy(chars[s].name, script1);
					j++;
				}
				else j++;
			}
		}
		while (script1[0]!='}');
		closescript();
	}
}


void initque( void ) // Initilizes the gmpages[] and counspages[] arrays and also jails
{
	int i;
	for( i = 1; i < MAXPAGES; i++ )
	{
		gmpages[i].name[0] = 0;
		gmpages[i].reason[0] = 0;
		gmpages[i].ser1 = 0x00;
		gmpages[i].ser2 = 0x00;
		gmpages[i].ser3 = 0x00;
		gmpages[i].ser4 = 0x00;
		gmpages[i].timeofcall[0]=0;
		gmpages[i].handled=1;
	}
	for(i = 1; i < MAXPAGES; i++)
	{
		counspages[i].name[0] = 0;
		counspages[i].reason[0] = 0;
		counspages[i].ser1 = 0x00;
		counspages[i].ser2 = 0x00;
		counspages[i].ser3 = 0x00;
		counspages[i].ser4 = 0x00;
		counspages[i].timeofcall[0]=0;
		counspages[i].handled=1;
	}
	jails[1].x=5276; // Jail1
	jails[1].y=1164;
	jails[1].z=0;
	jails[1].occupied=0;
	
	jails[2].x=5286; // Jail2
	jails[2].y=1164;
	jails[2].z=0;
	jails[2].occupied=0;
	
	jails[3].x=5296; // Jail3
	jails[3].y=1164;
	jails[3].z=0;
	jails[3].occupied=0;
	
	jails[4].x=5306; // Jail4
	jails[4].y=1164;
	jails[4].z=0;
	jails[4].occupied=0;
	
	jails[5].x=5276; // Jail5
	jails[5].y=1174;
	jails[5].z=0;
	jails[5].occupied=0;
	
	jails[6].x=5286; // Jail6
	jails[6].y=1174;
	jails[6].z=0;
	jails[6].occupied=0;
	
	jails[7].x=5296; // Jail7
	jails[7].y=1174;
	jails[7].z=0;
	jails[7].occupied=0;
	
	jails[8].x=5306; // Jail8
	jails[8].y=1174;
	jails[8].z=0;
	jails[8].occupied=0;
	
	jails[9].x=5283; // Jail9
	jails[9].y=1184;
	jails[9].z=0;
	jails[9].occupied=0;
	
	jails[10].x=5304; // Jail10
	jails[10].y=1184;
	jails[10].z=0;
	jails[10].occupied=0;
}


void donewithcall(int s, int type)
{
	if(chars[currchar[s]].callnum!=0) //Player is on a call
	{
		if(type==1) //Player is a GM
		{
			gmpages[chars[currchar[s]].callnum].handled=1;
			gmpages[chars[currchar[s]].callnum].name[0] = 0;
			gmpages[chars[currchar[s]].callnum].reason[0] = 0;
			gmpages[chars[currchar[s]].callnum].ser1=0;
			gmpages[chars[currchar[s]].callnum].ser2=0;
			gmpages[chars[currchar[s]].callnum].ser3=0;
			gmpages[chars[currchar[s]].callnum].ser4=0;
			chars[currchar[s]].callnum=0;
			sysmessage(s,"Call removed from the GM queue.");
		}
		else //Player is a counselor
		{
			counspages[chars[currchar[s]].callnum].handled=1;
			counspages[chars[currchar[s]].callnum].name[0] = 0; 
			counspages[chars[currchar[s]].callnum].reason[0] = 0; 
			counspages[chars[currchar[s]].callnum].ser1=0;
			counspages[chars[currchar[s]].callnum].ser2=0;
			counspages[chars[currchar[s]].callnum].ser3=0;
			counspages[chars[currchar[s]].callnum].ser4=0;
			chars[currchar[s]].callnum=0;
			sysmessage(s,"Call removed from the Counselor queue.");
		}
	}
	else
	{
		sysmessage(s,"You are currently not on a call");
	}
}

int response(int s)
{
	char buffer1[MAXBUFFER];
	int i,j;
	int k, skill=-1;
	char *comm;
	
	char *response1;
	char *response2;
	char *response3;
	
	char search1[50];
	char search2[50];
	char search3[50];
	
	char nonuni[512];
	char temp[512];
	char temp2[512];
	int x=-1;
	int y=0;
	
	if(chars[currchar[s]].unicode)
	{
		for (i=13;i<(buffer[s][1]<<8)+buffer[s][2];i=i+2)
		{
			nonuni[(i-13)/2]=buffer[s][i];
		} 
	}
	
	if(!(chars[currchar[s]].unicode))
	{
		for (i=7; i < MAXBUFFER; i++)
		{
			tbuffer[i]=buffer1[i];
			buffer1[i]=toupper(buffer[s][i]);
		}
	}
	else
	{
		for (i=0; i < MAXBUFFER-8; i++)
		{
			tbuffer[i+8]=buffer1[i+8];
			buffer1[i+8]=toupper(nonuni[i]);
		}
	}
	
	
	
	comm=&buffer1[8];
	strcpy( search1, "KILLS" ); 
	response1 = (strstr( comm, search1 ) );
	if( response1 && online( currchar[s] ) && (!(chars[currchar[s]].dead )))
	{
		i = chars[currchar[s]].kills;
		if( chars[currchar[s]].kills == 0 )
		{
			sysmessage( s, "You are an upstanding Citizen... no kills." );
		}
		else if ( chars[currchar[s]].kills > repsys.maxkills )
		{
			sysmessage( s, "You are a very evil person... %i kills.", i );
		}
		else
		{
			sysmessage( s, "You have %i kills.", i );
			return 1;
		}
	}
	
	//for(k=0;k<charcount;k++)
	x=chars[currchar[s]].x;
	y=chars[currchar[s]].y;//, z=chars[i].z;
	
	//Char mapRegions
	int getcell=mapRegions->GetCell(x,y);
	int mapitem=-1;
	int mapitemptr=-1;
	int mapchar=-1;
	do //check all items in this cell
	{
		mapitemptr=mapRegions->GetNextItem(getcell, mapitemptr);
		if (mapitemptr==-1) break;
		mapitem=mapRegions->GetItem(getcell, mapitemptr);
		if (mapitem>99999) mapchar=mapitem-1000000;
		if (mapitem!=-1 && mapitem>=1000000)
		{
			k=mapchar;
			if (abs(chars[currchar[s]].x-chars[k].x)<=2 &&
				abs(chars[currchar[s]].y-chars[k].y)<=2 &&
				abs(chars[currchar[s]].z-chars[k].z)<=5)
			{
				if (chars[k].trigger)
				{
					if (strlen(chars[k].trigword))
					{
						strcpy(search1, chars[k].trigword);
						strupr(search1);
						response1=(strstr( comm, search1));
						if (response1 && (!(chars[currchar[s]].dead)))
						{
							if (chars[k].disabled)
							{
								npctalkall(k,"I'm a little busy now! Leave me be!", 0);
							} else {
								triggernpc(s,k);
							}
							return 1;
						}
					}
				}
			}
			// Dupois - Added Dec 20, 1999
			// Escort text matches
			strcpy(search1,"I WILL TAKE THEE");
			strcpy(search2,"DESTINATION");
			response1=(strstr( comm, search1));
			response2=(strstr( comm, search2));
			
			// If either of the above responses match, then find the NPC we are talking to
			if ( response1 || response2 )
			{
				// If the PC is dead then break out, The dead cannot accept quests
				if ( chars[currchar[s]].dead ) 
					return 0;
				
				// Search for the NPC we are talking to
				//for (k=0;k<charcount;k++)
				//{
				int	StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
				//int	getcell=mapRegions->GetCell(chars[currchar[s]].x,chars[currchar[s]].y);
				
				unsigned int increment=0;
				for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
				{
					for (int a=0;a<3;a++)
					{
						int mapitemptr=-1;
						int mapitem=-1;
						int mapchar=-1;
						do //check all items in this cell
						{
							mapchar=-1;
							mapitemptr=mapRegions->GetNextItem(checkgrid+a, mapitemptr);
							if (mapitemptr==-1) break;
							mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
							if(mapitem>999999) mapchar=mapitem-1000000;
							if (mapitem>=1000000)
							{
								k=mapchar;
								// If this is an NPC then check it 
								if ( chars[k].npc == 1 )
								{
									// I WILL TAKE THEE
									// If this is a request for hire and the PC is within range of the NPC and the NPC is waiting for an ESCORT
									if ( (response1 && (chardist(k, currchar[s])<=1) && (chars[k].questType==ESCORTQUEST) && (chars[k].ftarg==-1)) )
									{
										// Set the NPC to follow the PC
										chars[k].ftarg = currchar[s];
										
										// Set the NPC to wander freely
										chars[k].npcWander = 1;
										
										// Set the expire time if nobody excepts the quest
										chars[k].summontimer = uiCurrentTime + ( CLOCKS_PER_SEC * server_data.escortactiveexpire );
										
										// Send out the rant about accepting the escort
										sprintf(temp, "Lead on! Payment shall be made when we arrive at %s.", region[chars[k].questDestRegion].name);
										npctalkall(k,temp, 0);
										
										// Remove post from message board (Mark for deletion only - will be cleaned during cleanup)
										MsgBoardQuestEscortRemovePost( k );
										
										// Return 1 so that we indicate that we handled the message
										return 1;
									}
									else if ( (response1 && (chardist(k, currchar[s])<=1) && (chars[k].questType==ESCORTQUEST)) )
									{
										// If the current NPC already has an ftarg then respond to query for quest
										response2 = response1;
									}
									
									// DESTINATION
									// If this is a request to find out where a NPC wants to go and the PC is within range of the NPC and the NPC is waiting for an ESCORT
									if ( (response2 && (chardist(k, currchar[s])<=1) && (chars[k].questType==ESCORTQUEST)) )
									{
										if ( chars[k].ftarg == currchar[s] )
										{
											// Send out the rant about accepting the escort
											sprintf(temp, "Lead on to %s. I shall pay thee when we arrive.", region[chars[k].questDestRegion].name);
											npctalkall(k,temp, 0);
										}
										else if ( chars[k].ftarg == -1 )  // If nobody has been accepted for the quest yet
										{
											// Send out the rant about accepting the escort
											sprintf(temp, "I am seeking an escort to %s. Wilt thou take me there?", region[chars[k].questDestRegion].name);
											npctalkall(k,temp, 0);
										}
										else // The must be enroute
										{
											// Send out a message saying we are already being escorted
											sprintf(temp, "I am already being escorted to %s by %s.", region[chars[k].questDestRegion].name, chars[chars[k].ftarg].name );
											npctalkall(k,temp, 0);
										}
										
										// Return success ( we handled the message )
										return 1;
									}
								}
							}//if mapitem
						} while (mapitemptr != -1);
					}//for
				}//for
			}
				}// Dupois - End
				 /*
				 strcpy(search1,"BANK");
				 response1=(strstr( comm, search1));
				 if (response1 && (!(chars[currchar[s]].dead)) && inbankrange(currchar[s]))
				 {
				 openbank(s, currchar[s]);
			}*/
			
			strcpy(search1,"BANK");
			strcpy(search2,"BALANCE");
			strcpy(search3, server_data.specialbanktrigger );
			response1=(strstr( comm, search1));
			response2=(strstr( comm, search2));
			if (response1 && (!(chars[currchar[s]].dead)) && inbankrange(currchar[s]))
			{
				openbank(s, currchar[s]);
			}
			// this only if special bank is enabled - AntiChrist
			if( server_data.usespecialbank )
			{
				strcpy( search1, server_data.specialbanktrigger );
				response1 = ( strstr( comm, search1 ) );
				if( response1 && ( !(chars[currchar[s]].dead )) && inbankrange( currchar[s] ) )
				{
					openspecialbank( s, currchar[s] );
				}
			}
			if (response2 && (!(chars[currchar[s]].dead)) && inbankrange(currchar[s]))	
			{
				int goldCount = GetBankCount( currchar[s], 0x0EED );
				bool found = false;
				int counter2 = 0;
				int	StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
				
				unsigned int increment=0;
				for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
				{
					for (int a=0;a<3;a++)
					{
						int mapitemptr=-1;
						int mapitem=-1;
						int mapchar=-1;
						do //check all items in this cell
						{
							mapchar=-1;
							mapitemptr=mapRegions->GetNextItem(checkgrid+a, mapitemptr);
							if( mapitemptr == -1 ) 
								break;
							mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
							if( mapitem > 999999 ) 
								mapchar = mapitem - 1000000;
							if( mapitem >= 1000000 )
							{
								counter2 = mapchar;
								if( chars[counter2].npcaitype&0x08 && chardist( currchar[s], counter2 ) <= 12 ) // 12 for bank?
								{
									found = true;
									sprintf(temp, "%s's balance as of now is %i.", chars[currchar[s]].name, goldCount);
									npctalk(s, counter2, temp, 1);
								}
							}
						} while (mapitemptr != -1 && !found);
						if( found ) 
							break;
					}
					if( found ) 
						break;
				}
			}
			// This training code is by Anthracks (fred1117@tiac.net) and really psychotic
			// if it doesn't work or you can't decipher it, you know who to blame
			strcpy(search1,"TRAIN");
			strcpy(search2,"TEACH");
			strcpy(search3,"LEARN");
			response1=(strstr( comm, search1));
			response2=(strstr( comm, search2));
			response3=(strstr( comm, search3));
			if (response1 || response2 || response3) //if the player wants to train
			{
				// Stop the NPC from moving for a minute while talking with the player
				chars[k].npcmovetime = (unsigned int)( uiCurrentTime + ( 60 * CLOCKS_PER_SEC ) );
				unsigned int nChar=currchar[s];  //for the chars[] #
				chars[nChar].trainer=-1; //this is to prevent errors when a player says "train <skill>" then doesn't pay the npc
				for(i=0;i<ALLSKILLS;i++)
				{
					if(strstr(comm, skillname[i]))
					{
						skill=i;  //Leviathan fix
						break;
					}
				}
				if(skill==-1) // Didn't ask to be trained in a specific skill - Leviathan fix
				{
					if(chars[nChar].trainer==-1) //not being trained, asking what skills they can train in
					{
						int	StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
						
						unsigned int increment=0;
						for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
						{
							for (int a=0;a<3;a++)
							{
								int mapitemptr=-1;
								int mapitem=-1;
								int mapchar=-1;
								do //check all items in this cell
								{
									mapchar=-1;
									mapitemptr=mapRegions->GetNextItem(checkgrid+a, mapitemptr);
									if (mapitemptr==-1) break;
									mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
									if(mapitem>999999) mapchar=mapitem-1000000;
									else mapchar = -1;
									if (mapitem>=1000000)
									{
										k=mapchar;
										if (chars[k].npc)
										{
											if (chardist(k, currchar[s])<=3 && chars[k].id1==0x01 && (chars[k].id2==0x90 || chars[k].id2==0x91))
											{
												if(!chars[k].cantrain)
												{
													npctalk(s, k, "I am sorry, but I have nothing to teach thee", 0);
													return 1;
												}
												chars[k].trainingplayerin=255; // Like above, this is to prevent  errors when a player says "train <skill>" then doesn't pay the npc
												strcpy(temp,"I can teach thee the following skills: ");
												for(j=0;j<ALLSKILLS;j++)
												{
													if(chars[k].baseskill[j]>10)
													{
														sprintf(temp2,"%s, ", strlwr(skillname[j]));
														strupr(skillname[j]); // I found out strlwr changes the actual  string permanently, so this undoes that
														if(!y) temp2[0]=toupper(temp2[0]); // If it's the first skill,  capitalize it.
														strcat(temp,temp2);
														y++;
													}
												}
												if(y)
												{
													temp[strlen(temp)-2]='.'; // Make last character a . not a ,  just to look nicer
													npctalk(s, k, temp, 0);
												}
												else
												{
													npctalk(s, k, "I am sorry, but I have nothing to teach thee", 0);
												}
												return 1;
											}
										}
									}// if
								} while (mapitemptr != -1);
							}//for
						}//for
					} // End it .trainer if statement
				} // The if for if they are asking to train in a specific skill
				else // They do want to learn a specific skill
				{
					int	StartGrid=mapRegions->StartGrid(chars[currchar[s]].x,chars[currchar[s]].y);
					
					unsigned int increment=0;
					for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
					{
						for (int a=0;a<3;a++)
						{
							int mapitemptr=-1;
							int mapitem=-1;
							int mapchar=-1;
							int gg = 0;
							int ges = 0;
							do //check all items in this cell
							{
								mapchar=-1;
								mapitemptr=mapRegions->GetNextItem(checkgrid+a, mapitemptr);
								if (mapitemptr==-1) break;
								mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
								if(mapitem>999999) mapchar=mapitem-1000000;
								else mapchar = -1;
								if (mapitem>=1000000)
								{
									k=mapchar;
									if (chars[k].npc)
									{
										if (chardist(k, nChar)<=3 && (chars[k].id1==0x01 && ((chars[k].id2==0x90) || (chars[k].id2==0x91))))
										{
											if(!chars[k].cantrain)
											{
												npctalk(s, k, "I am sorry, but I have nothing to teach thee", 0);
												return 1;
											}
											// Start skill cap check - UltimaX DevTeam kkung
											if( !chars[currchar[s]].priv&0x01 ) // is not gm
											{
												for( gg = 0; gg < ALLSKILLS; gg++ )
													ges += chars[currchar[s]].baseskill[gg];
												if( ges > server_data.skillcap - 250 )
												{
													npcemote( s, k, "Sorry, you have reached the skill-cap!", 0 );
													return 1;
												}
											}
											else
												ges = 0;
											if(chars[k].baseskill[skill]>10)
											{
												sprintf(temp,"Thou wishest to learn of  %s?",strlwr(skillname[skill]));
												strupr(skillname[skill]); // I found out strlwr changes the actual string permanently, so this undoes that
												if(chars[nChar].baseskill[skill]>=250)
												{
													strcat(temp, " I can teach thee no more than thou already knowest!");
												}
												else
												{
													if(chars[k].baseskill[skill]<=250)
													{
														sprintf(temp2, " Very well I, can train thee up to the level of %i percent for %i gold. Pay for less and I shall teach thee less.",(int)(chars[k].baseskill[skill]/2/10),(int)(chars[k].baseskill[skill]/2)-chars[nChar].baseskill[skill]);
													}
													else
													{
														sprintf(temp2, " Very well I, can train thee up to the level of %i percent for %i gold. Pay for less and I shall teach thee less.",25,250-chars[nChar].baseskill[skill]);
													}
													strcat(temp, temp2);
													chars[nChar].trainer=chars[k].serial;
													chars[k].trainingplayerin=skill;
												}
												npctalk(s, k, temp, 0);
												return 1;
											} // They cannot teach x skill
											else
											{npctalk(s, k, "I am sorry but I cannot train thee in that skill.", 0); return 1; }
										}
									}
								}//if
							} while (mapitemptr != -1);
						}//for
					}//for
				} // end the else
			}
			
			strcpy(search1," FOLLOW");
			strcpy(search2," ME");
			response1=(strstr( comm, search1));
			response2=(strstr( comm, search2));
			
			if (response1) //if follow
			{
				chars[currchar[s]].guarded = false;
				for (i=0;i<cownsp[chars[currchar[s]].serial%HASHMAX].max;i++)
				{
					k=cownsp[chars[currchar[s]].serial%HASHMAX].pointer[i];
					if (k!=-1)
					{
						if (chars[k].ownserial==chars[currchar[s]].serial || ( chars[currchar[s]].priv&0x01 )) // owner of the char || a GM
						{
							strcpy(search3,chars[k].name);
							strupr(search3);
							response3=(strstr( comm, search3));
							if (response3) //if petname is in
							{
								if( chars[k].npcaitype == 17 ) return 0;  // ripper
								if (chardist(k, currchar[s])<=7)
								{
									if (response2) //if me is in
									{
										chars[k].ftarg = (currchar[s]);
										chars[k].npcWander = 1;
										playmonstersound(k, chars[k].id1, chars[k].id2, SND_STARTATTACK);
										return 1;
									}
									else
									{
										//add pet follow code here
										addid1[s] = chars[k].ser1;
										addid2[s] = chars[k].ser2;
										addid3[s] = chars[k].ser3;
										addid4[s] = chars[k].ser4;
										target(s, 0, 1, 0, 117, "Click on the target to follow.");
										return 1;
									}
								}
							}
						}
					}
				}
			}
			strcpy(search1," KILL");
			strcpy(search2," ATTACK");
			response1=(strstr( comm, search1));
			response2=(strstr( comm, search2));
			if ((response1)||(response2)) //if kill||attack
			{
				chars[currchar[s]].guarded = false;
				for (i=0;i<cownsp[chars[currchar[s]].serial%HASHMAX].max;i++)
				{
					k=cownsp[chars[currchar[s]].serial%HASHMAX].pointer[i];
					if (k!=-1)
					{
						if (chars[k].ownserial==chars[currchar[s]].serial || ( chars[currchar[s]].priv&0x01 ))
						{
							strcpy(search3,chars[k].name);
							strupr(search3);
							response3=(strstr( comm, search3));
							if (response3) //if petname is in
							{
								if( chars[k].npcaitype == 17 ) return 0;  // ripper
								if (chardist(k, currchar[s])<=7)
								{
									addid1[s]=chars[k].ser1;
									addid2[s]=chars[k].ser2;
									addid3[s]=chars[k].ser3;
									addid4[s]=chars[k].ser4;
									//pet kill code here
									target(s, 0, 1, 0, 118, "Select the target to attack.");
									return 1;
								}
							}
						}
					}
				}
			}
			strcpy(search1," FETCH");
			strcpy(search2," GET");
			response1=(strstr( comm, search1));
			response2=(strstr( comm, search2));
			if ((response1)||(response2)) //if fetch||get
			{
				chars[currchar[s]].guarded = false;
				for (i=0;i<cownsp[chars[currchar[s]].serial%HASHMAX].max;i++)
				{
					k=cownsp[chars[currchar[s]].serial%HASHMAX].pointer[i];
					if (k!=-1)
					{
						if (chars[k].ownserial==chars[currchar[s]].serial || ( chars[currchar[s]].priv&0x01 ))
						{
							strcpy(search3,chars[k].name);
							strupr(search3);
							response3=(strstr( comm, search3));
							if (response3) //if petname is in
							{
								if( chars[k].npcaitype == 17 ) return 0;  // ripper
								if (chardist(k, currchar[s])<=7)
								{
									addid1[s]=chars[k].ser1;
									addid2[s]=chars[k].ser2;
									addid3[s]=chars[k].ser3;
									addid4[s]=chars[k].ser4;
									//pet fetch code here
									target(s, 0, 1, 0, 120, "Click on the object to fetch.");
									return 1;
								}
							}
						}
					}
				}
			}
			
			strcpy(search1," COME");
			response1=(strstr( comm, search1));
			if (response1) //if come
			{
				chars[currchar[s]].guarded = false;
				int k;
				for (i=0;i<cownsp[chars[currchar[s]].serial%HASHMAX].max;i++)
				{
					k=cownsp[chars[currchar[s]].serial%HASHMAX].pointer[i];
					if (k!=-1)
					{
						if (chars[k].ownserial==chars[currchar[s]].serial || ( chars[currchar[s]].priv&0x01 ))
						{
							strcpy(search3,chars[k].name);
							strupr(search3);
							response3=(strstr( comm, search3));
							if (response3) //if petname is in
							{
								if( chars[k].npcaitype == 17 ) return 0;  // ripper
								if (chardist(k, currchar[s])<=7)
								{
									chars[k].ftarg=(currchar[s]);
									chars[k].npcWander=1;
									sysmessage(s, "Your pet begins following you.");
									return 1;
								}
							}
						}
					}
				}
			}
			
			strcpy(search1," GUARD");
			strcpy(search2," ME");
			response1=(strstr( comm, search1));
			response2=(strstr( comm, search2));
			if (response1) //if guard
			{
				for (i=0;i<cownsp[chars[currchar[s]].serial%HASHMAX].max;i++)
				{
					k=cownsp[chars[currchar[s]].serial%HASHMAX].pointer[i];
					if (k!=-1)
					{
						if (chars[k].ownserial==chars[currchar[s]].serial || ( chars[currchar[s]].priv&0x01 ))
						{
							strcpy(search3,chars[k].name);
							strupr(search3);
							response3=(strstr( comm, search3));
							if (response3) //if petname is in
							{
								if (chardist(k, currchar[s])<=7)
								{
									if( chars[k].npcaitype == 17 ) return 0;
									if (response2) //if me is in
									{
										sysmessage(s, "Your pet is now guarding you.");
										chars[k].npcaitype = 32;											// 32 is guard mode
										chars[currchar[s]].guarded = true;
										chars[k].ftarg=(currchar[s]);
										chars[k].npcWander=1;
										//add pet guard me code here
										return 1;
									}
									else
									{
										addid1[s] = chars[k].ser1;
										addid2[s] = chars[k].ser2;
										addid3[s] = chars[k].ser3;
										addid4[s] = chars[k].ser4;
										sysmessage(s, "Your pet is now guarding you.");
										chars[currchar[s]].guarded = true;
										chars[k].ftarg=(currchar[s]);
										chars[k].npcWander=1;
										chars[k].npcaitype = 32;											// 32 is guard mode
										//add pet guard code here
										return 1;
									}
								}
							}
						}
					}
				}
			}
			
			strcpy(search1," STOP");
			strcpy( search2, " STAY" );
			response1=(strstr( comm, search1));
			response2 = ( strstr( comm, search2 ) );
			if( (response1) || (response2) ) //if stop||stay
			{
				chars[currchar[s]].guarded = false;
				for (i=0;i<cownsp[chars[currchar[s]].serial%HASHMAX].max;i++)
				{
					k=cownsp[chars[currchar[s]].serial%HASHMAX].pointer[i];
					if (k!=-1)
					{
						if (chars[k].ownserial==chars[currchar[s]].serial || ( chars[currchar[s]].priv&0x01 ))
						{
							strcpy(search3,chars[k].name);
							strupr(search3);
							response3=(strstr( comm, search3));
							if (response3) //if petname is in
							{
								if( chars[k].npcaitype == 17 ) return 0;
								if (chardist(k, currchar[s])<=7)
								{
									//pet stop code here
									chars[k].ftarg=-1;
									chars[k].targ=-1;
									if (chars[k].war) npcToggleCombat(k);
									chars[k].npcWander=0;
									return 1;
								}
							}
						}
					}
				}
			}
			strcpy(search1," TRANSFER");
			response1=(strstr( comm, search1));
			if (response1) //if transfer
			{
				chars[currchar[s]].guarded = false;
				for (i=0;i<cownsp[chars[currchar[s]].serial%HASHMAX].max;i++)
				{
					k=cownsp[chars[currchar[s]].serial%HASHMAX].pointer[i];
					if (i!=-1)
					{
						if (chars[k].ownserial==chars[currchar[s]].serial || ( chars[currchar[s]].priv&0x01 ))
						{
							strcpy(search3,chars[k].name);
							strupr(search3);
							response3=(strstr( comm, search3));
							if (response3) //if petname is in
							{
								if( chars[k].npcaitype == 17 ) return 0;
								if (chardist(k, currchar[s])<=7)
								{
									//pet transfer code here
									addid1[s]=chars[k].ser1;
									addid2[s]=chars[k].ser2;
									addid3[s]=chars[k].ser3;
									addid4[s]=chars[k].ser4;
									target(s, 0, 1, 0, 119, "Select character to transfer your pet to.");
									return 1;
								}
							}
						}
					}
				}
			}
			strcpy(search1," RELEASE");
			response1=(strstr( comm, search1));
			if (response1) //if transfer
			{
				chars[currchar[s]].guarded = false;
				for (i=0;i<cownsp[chars[currchar[s]].serial%HASHMAX].max;i++)
				{
					k=cownsp[chars[currchar[s]].serial%HASHMAX].pointer[i];
					if (k!=-1)
					{
						if (chars[k].ownserial==chars[currchar[s]].serial || ( chars[currchar[s]].priv&0x01 ))
						{
							strcpy(search3,chars[k].name);
							strupr(search3);
							response3=(strstr( comm, search3));
							if (response3) //if petname is in
							{
								if( chars[k].npcaitype == 17 ) return 0;
								if (chardist(k, currchar[s])<=7)
								{
									if (chars[k].summontimer)
									{
										chars[k].summontimer=uiCurrentTime;
									}
									//pet release code here
									chars[k].ftarg=-1;
									chars[k].npcWander=2;
									removefromptr(&cownsp[chars[k].ownserial%HASHMAX], k);
									chars[k].own1=255;
									chars[k].own2=255;
									chars[k].own3=255;
									chars[k].own4=255;
									chars[k].ownserial=-1;
									chars[k].tamed = false;
									sprintf(temp, "*%s appears to have decided that it is better off without a master *", chars[k].name);
									npctalkall(k,temp, 0);
									if (chars[k].summontimer)
									{
										soundeffect2(i, 0x01, 0xFE);
										Npcs->DeleteChar(k) ;
									}
									return 1;
								}
							}
						}
					}
				}
			}
			
			//	}//if !=-1
	} while (mapitem!=-1);
	return 0;
}

ITEM GetRootPack( ITEM p )
{
	if( p == -1 || p >= imem )
		return -1;
	int a = 0;
	ITEM x = p;
	ITEM currentContainer = -1;
	do 
	{
		if( x == -1 )				// Non existent item
			return currentContainer;
		if( items[x].contserial == -1 )	// It's on the ground, so it MUST be the root pack
			return x;
		if( items[x].cont1 < 0x40 )		// It's a character
			return currentContainer;	// Return current container
		
		x = calcItemFromSer( items[x].contserial );
		if( x != -1 )
			currentContainer = x;
		a++;
	} while( a < 50 );
	return currentContainer;
}

int GetPackOwner( int p )
{
	int a = 0, b;
	int x = p;
	
	if( p == -1 ) 
		return -1;
	if( items[p].contserial == -1 ) 
		return -1;
	if( items[p].cont1 < 0x40 ) 
		return calcCharFromSer( items[p].contserial ); 
	do 
	{
		if( a >= 50 ) 
			return -1;//Too many packs! must stop endless loop!
		if( items[x].contserial == -1 ) 
			return -1;
		
		if( items[x].cont1 >= 0x40 )//inside an item
			x = calcItemFromSer( items[x].contserial );
		if( x != -1 ) 
			b = items[x].cont1; 
		else 
			b = 0x42;
		a++;
	} while( b >= 0x40 );
	return calcCharFromSer( items[x].contserial );
}

void PlVGetgold(int s, int v)//PlayerVendors
{
	unsigned int pay=0, give=chars[v].holdg, t=0;
	if (chars[currchar[s]].serial==chars[v].ownserial)
	{
		if (chars[v].holdg<1)
		{
			npctalk(s,v,"I have no gold waiting for you.", 0);
			chars[v].holdg=0;
			return;
		} else if(chars[v].holdg<=65535)
		{
			if (chars[v].holdg>9)
			{
				pay=(int)(chars[v].holdg*.1);
				give-=pay;
			} else {
				pay=chars[v].holdg;
				give=0;
			}
			chars[v].holdg=0;
		} else {
			t=chars[v].holdg-65535;
			chars[v].holdg=65535;
			pay=6554;
			give=58981;
		}
		if( give ) 
			Items->SpawnItem( s, give, "#", 1, 0x0E, 0xED, 0, 0, 1, 1 );
		
		sprintf(temp, "Today's purchases total %i gold. I am keeping %i gold for my self. Here is the remaining %i gold. Have a nice day.",chars[v].holdg,pay,give);
		npctalk(s,v,temp, 0);
		chars[v].holdg=t;
	} else npctalk(s,v,"I don't work for you!", 0);
}

void responsevendor(int s) //Modified by AntiChrist
{
	char buffer1[MAXBUFFER];
	int i;
	int k;
	char *comm;
	
	char *response1;
	char *response2;
	char *response3;
	char *response4;
	
	char search1[50];
	char search2[50];
	char search3[50];
	char search4[50];
	
	char nonuni[512];
	
	int x=chars[currchar[s]].x, y=chars[currchar[s]].y;//, z=chars[i].z;
	int getcell=mapRegions->GetCell(x,y);
	int mapitem=-1;
	int mapitemptr=-1;
	int mapchar=-1;
	
	//Char mapRegions
	
	if(chars[currchar[s]].unicode)
		for (i=13;i<(buffer[s][1]<<8)+buffer[s][2];i=i+2)
		{
			nonuni[(i-13)/2]=buffer[s][i];
		} 
		
		if(!(chars[currchar[s]].unicode))
		{
			for (i=7; i < MAXBUFFER; i++)
			{
				tbuffer[i]=buffer1[i];
				buffer1[i]=toupper(buffer[s][i]);
			}
		}
		else
		{
			for (i=0; i < MAXBUFFER-8; i++)
			{
				tbuffer[i+8]=buffer1[i+8];
				buffer1[i+8]=toupper(nonuni[i]);
			}
		}
		
		comm=&buffer1[8];
		
		strcpy(search1, "VENDOR");
		strcpy(search2, "SHOPKEEPER");
		strcpy(search4, " BUY");
		response1=(strstr(comm, search1));
		response2=(strstr(comm, search2));
		response4=(strstr(comm, search4));
		
		//if ((response1 || response2 || response3)&&response4)
		if (response4)//AntiChrist
		{
			//for(k=0;k<charcount;k++)
			do //check all items in this cell
			{
				mapitemptr=mapRegions->GetNextItem(getcell, mapitemptr);
				if (mapitemptr==-1) break;
				mapitem=mapRegions->GetItem(getcell, mapitemptr);
				if (mapitem>99999) mapchar=mapitem-1000000;
				if (mapitem!=-1 && mapitem>=1000000)
				{
					k=mapchar;
					// Stop the NPC from moving for a minute while talking with the player
					chars[k].npcmovetime = (unsigned int)( uiCurrentTime + ( CLOCKS_PER_SEC * 60 ) );
					strcpy(search3,chars[k].name);
					strupr(search3);
					response3=(strstr( comm, search3));
					if (response3 || response2 || response1)
					{
						if (abs(chars[currchar[s]].x-chars[k].x)<=3 &&
							abs(chars[currchar[s]].y-chars[k].y)<=3 &&
							abs(chars[currchar[s]].z-chars[k].z)<=5)
						{ //PlayerVendors
							if(chars[k].npcaitype==17)
							{
								addx[s]=k;
								npctalk(s,k,"What would you like to buy?", 0);
								target(s,0,1,0,224," ");
								k=charcount;
							} else if(Targ->BuyShop(s, k)) k=charcount;
						}
					}
				}
			} while (mapitem!=-1);
		}
		
		
		//PlayerVendors
		response4=0;
		strcpy(search4, " GOLD");
		response4=(strstr( comm, search4));
		if (response4)//AntiChrist
		{
			//for(k=0;k<charcount;k++)
			do //check all items in this cell
			{
				mapitemptr=mapRegions->GetNextItem(getcell, mapitemptr);
				if (mapitemptr==-1) break;
				mapitem=mapRegions->GetItem(getcell, mapitemptr);
				if (mapitem>99999) mapchar=mapitem-1000000;
				if (mapitem!=-1 && mapitem>=1000000)
				{
					k=mapchar;
					strcpy(search3,chars[k].name);
					strupr(search3);
					response3=(strstr( comm, search3));
					if (response3 || response2 || response1)
					{
						if (abs(chars[currchar[s]].x-chars[k].x)<=3 &&
							abs(chars[currchar[s]].y-chars[k].y)<=3 &&
							abs(chars[currchar[s]].z-chars[k].z)<=5)
						{
							if (chars[k].npcaitype==17)
							{
								PlVGetgold(s, k);
								return;
							}
						}
					}
				}
			} while (mapitem!=-1);
		}
		//end PlayerVendors --^
		
		response4=0;
		strcpy(search4, " SELL");
		response4=(strstr( comm, search4));
		if (response4)//AntiChrist
		{
			//for(k=0;k<charcount;k++)
			do //check all items in this cell
			{
				mapitemptr=mapRegions->GetNextItem(getcell, mapitemptr);
				if (mapitemptr==-1) break;
				mapitem=mapRegions->GetItem(getcell, mapitemptr);
				if (mapitem>99999) mapchar=mapitem-1000000;
				if (mapitem!=-1 && mapitem>=1000000)
				{
					k=mapchar;
					strcpy(search3,chars[k].name);
					strupr(search3);
					response3=(strstr( comm, search3));
					if (response3 || response2 || response1)
					{
						if (abs(chars[currchar[s]].x-chars[k].x)<=3 &&
							abs(chars[currchar[s]].y-chars[k].y)<=3 &&
							abs(chars[currchar[s]].z-chars[k].z)<=5)
						{
							// Stop the NPC from moving for a minute while talking with the player
							chars[k].npcmovetime = (unsigned int)( uiCurrentTime + ( CLOCKS_PER_SEC * 60 ) );
							//if (sellstuff(s, k)) k=charcount; //Morrolan bugfix
							sellstuff(s, k);
						}
					}
				}
			} while (mapitem!=-1);
		}
		return;
}



void sendshopinfo(int s, int c, int i)
{
	unsigned char m1[4096];
	unsigned char m2[4096];
	char itemname[256];
	char cFoundItems=0;
	memset(m1,0,4096);
	memset(m2,0,4096);
	memset(itemname,0,256);
	int j, k, m1t, m2t, value,serial,serhash,ci;
	
	m1[0]=0x3C; // Container content message
	m1[1]=0;// Size of message
	m1[2]=0;// Size of message
	m1[3]=0;// Count of items
	m1[4]=0;// Count of items
	m2[0]=0x74;// Buy window details message
	m2[1]=0;// Size of message
	m2[2]=8;// Size of message
	m2[3]=items[i].ser1;// Container object
	m2[4]=items[i].ser2;// Container object
	m2[5]=items[i].ser3;// Container object
	m2[6]=items[i].ser4;// Container object
	m2[7]=0; // Count of items;
	m1t=5;
	m2t=8;
	serial=items[i].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		j=contsp[serhash].pointer[ci];
		if (j!=-1)
			if ((items[j].contserial==serial) &&
				(m2[7]!=255) && (items[j].amount!=0)) // 255 items max per shop container
			{
				m1[m1t+0]=items[j].ser1;//Item serial number
				m1[m1t+1]=items[j].ser2;//Item serial number
				m1[m1t+2]=items[j].ser3;//Item serial number
				m1[m1t+3]=items[j].ser4;//Item serial number
				m1[m1t+4]=items[j].id1; //Item art id number
				m1[m1t+5]=items[j].id2; //Item art id number
				m1[m1t+6]=0;            //Always zero
				m1[m1t+7]=items[j].amount>>8;//Amount for sale
				m1[m1t+8]=items[j].amount%256;//Amount for sale
				m1[m1t+9]=j>>8;//items[j].x/256; //Item x position
				m1[m1t+10]=j%256;//items[j].x%256;//Item x position
				m1[m1t+11]=j>>8;//items[j].y/256;//Item y position
				m1[m1t+12]=j%256;//items[j].y%256;//Item y position
				m1[m1t+13]=items[i].ser1; //Container serial number
				m1[m1t+14]=items[i].ser2; //Container serial number
				m1[m1t+15]=items[i].ser3; //Container serial number
				m1[m1t+16]=items[i].ser4; //Container serial number
				m1[m1t+17]=items[j].color1;//Item color
				m1[m1t+18]=items[j].color2;//Item color
				m1[4]++; // Increase item count.
				m1t=m1t+19;
				value=items[j].value;
				value=calcValue(j, value);
				if( server_data.trade_system == 1 ) 
					value = calcGoodValue( c, j, value, 0 ); // by Magius(CHE)
				m2[m2t+0]=value>>24;// Item value/price
				m2[m2t+1]=value>>16;//Item value/price
				m2[m2t+2]=value>>8; // Item value/price
				m2[m2t+3]=value%256; // Item value/price
				m2[m2t+4]=getname(j, itemname); // Item name length
				for(k=0;k<m2[m2t+4];k++)
				{
					m2[m2t+5+k]=itemname[k];
				}
				m2t=m2t+(m2[m2t+4])+5;
				m2[7]++;
				cFoundItems=1; //we found items so send message
			}
	}
	m1[1]=m1t>>8;
	m1[2]=m1t%256;
	m2[1]=m2t>>8;
	m2[2]=m2t%256;
	if (cFoundItems==1)
	{
		Network->xSend(s, m1, m1t, 0);
		Network->xSend(s, m2, m2t, 0);
	}
}

int getname(int i, char* itemname)
{
	tile_st tile;
	int j, len, mode, used, ok, namLen;
	if (items[i].name[0]!='#')
	{
		strcpy(itemname, items[i].name);
		return strlen(itemname)+1;
	}
	Map->SeekTile((items[i].id1<<8)+items[i].id2, &tile);
	if (tile.flag2&0x80) strcpy(itemname, "an ");
	else if (tile.flag2&0x40) strcpy(itemname, "a ");
	else itemname[0]=0;
	namLen = strlen( itemname );
	mode=0;
	used=0;
	len=strlen((char *) tile.name);
	for (j=0;j<len;j++)
	{
		ok=0;
		if ((tile.name[j]=='%')&&(mode==0)) mode=2;
		else if ((tile.name[j]=='%')&&(mode!=0)) mode=0;
		else if ((tile.name[j]=='/')&&(mode==2)) mode=1;
		else if (mode==0) ok=1;
		else if ((mode==1)&&(items[i].amount==1)) ok=1;
		else if ((mode==2)&&(items[i].amount>1)) ok=1;
		if (ok)
		{
			//:Terrin: faster/cleaner and will work in all compilers
			//			sprintf(itemname, "%s%c", itemname, tile.name[j]);
			itemname[namLen++] = tile.name[j];
			itemname[namLen  ] = '\0';
			if (mode) used=1;
		}
	}
	return strlen(itemname)+1;
}

void goldsfx(int s, int goldtotal, bool bAllHear )
{
	if (goldtotal==1) soundeffects(s, 0x00, 0x35, bAllHear);
	if ((goldtotal>1)&&(goldtotal<6)) soundeffects(s, 0x00, 0x36, bAllHear);
	else soundeffects(s, 0x00, 0x37, bAllHear);
	return;
}

void buyaction(int s)
{
	char clearmsg[8];
	int clear, i, j;
	int bitems[512];
	int amount[512];
	int layer[512];
	int playergoldtotal;
	int goldtotal;
	int itemtotal;
	int npc;
	int soldout;
	int p;
	
	p = packitem(currchar[s]);
	if( p == -1 ) 
		return;
	npc = calcCharFromSer( buffer[s][3], buffer[s][4], buffer[s][5], buffer[s][6] );
	
	clear = 0;
	goldtotal = 0;
	soldout = 0;
	itemtotal = (((256*(buffer[s][1]))+buffer[s][2])-8)/7;
	if( itemtotal > 511 ) 
		return;
	for(i=0;i<itemtotal;i++)
	{
		layer[i]=buffer[s][8+(7*i)];
		bitems[i]=calcItemFromSer(buffer[s][8+(7*i)+1], buffer[s][8+(7*i)+2],
			buffer[s][8+(7*i)+3], buffer[s][8+(7*i)+4]);
		amount[i]=(256*(buffer[s][8+(7*i)+5]))+buffer[s][8+(7*i)+6];
		goldtotal=goldtotal+(amount[i]*(items[bitems[i]].value));
/*		if( bitems[i] > -1 )
		{
			tmpvalue = items[bitems[i]].value;
			tmpvalue = calcValue( bitems[i], tmpvalue );
			if( server_data.trade_system ) tmpvalue = calcGoodValue( s, bitems[i], tmpvalue, 0 );
			goldtotal += amount[i]*tmpvalue;
		}*/
	}
	bool useBank;
	useBank = (goldtotal >= server_data.buyThreshold );
	if( useBank )
		playergoldtotal = GetBankCount( currchar[s], 0x0EED );
	else
		playergoldtotal = getamount( currchar[s], 0x0E, 0xED );
	if( playergoldtotal >= goldtotal || ( chars[currchar[s]].priv&1 ) )
	{
		for( i = 0; i < itemtotal; i++ )
		{
			if( items[bitems[i]].amount < amount[i] )
			{
				soldout = 1;
				break;		// no point searching the rest of the items
			}
		}
		if( soldout )
		{
			npctalk( s, npc, "Alas, I no longer have all those goods in stock.  Let me know if there is something else thou wouldst buy.", 0 );
			clear = 1;
		}
		else
		{
			if( chars[currchar[s]].priv&1 )
				sprintf( temp, "Here you are, %s. Someone as special as thee will receive my wares for free of course.", chars[currchar[s]].name );
			else
			{
				if( goldtotal == 1 )
					sprintf( temp, "Here you are, %s.  That will be %d gold coin.  I thank thee for thy business.", chars[currchar[s]].name, goldtotal );
				else
					sprintf( temp, "Here you are, %s.  That will be %d gold coins.  I thank thee for thy business.", chars[currchar[s]].name, goldtotal );
				goldsfx( s, goldtotal );
			}
			npctalkall( npc, temp, 0 );
			clear = 1;
			if( !(chars[currchar[s]].priv&1 ) ) 
			{
				int tAmount = 0;
				if( useBank )
					tAmount = DeleBankItem( currchar[s], 0x0EED, 0, goldtotal );
				else
					tAmount = delequan( currchar[s], 0x0E, 0xED, goldtotal );
				if( tAmount > 0 )
				{
					sprintf( temp, "Naughty naughty, you're trying to rip me off!  So you've just lost %i gold, bucko!", goldtotal - tAmount );
					sysmessage( s, temp );
					clear = 1;
					itemtotal = -1;
				}
			}
			for( i = 0; i < itemtotal; i++ )
			{
				if( items[bitems[i]].amount > amount[i] )
				{
					if( items[bitems[i]].pileable )
						Commands->DupeItem( s, bitems[i], amount[i] );
					else
					{
						for( j = 0; j < amount[i]; j++ )
							Commands->DupeItem( s, bitems[i], 1 );
					}
					items[bitems[i]].amount -= amount[i];
					items[bitems[i]].restock += amount[i];
				}
				else
				{
					switch( layer[i] )
					{
					case 0x1A:
						if( items[bitems[i]].pileable )
							Commands->DupeItem( s, bitems[i], amount[i] );
						else
						{
							for( j = 0; j < amount[i]; j++ )
								Commands->DupeItem( s, bitems[i], 1 );
						}
						items[bitems[i]].amount -= amount[i];
						items[bitems[i]].restock += amount[i];
						break;
					case 0x1B:
						if( items[bitems[i]].pileable )
						{
							if( items[bitems[i]].contserial != -1 )
								unsetserial( bitems[i], 1 );
							setserial( bitems[i], p, 1 );
							RefreshItem( bitems[i] ); // AntiChrist
						}
						else
						{
							for( j = 0; j < amount[i] - 1; j++ )
								Commands->DupeItem( s, bitems[i], 1 );
							if( items[bitems[i]].contserial != -1 )
								unsetserial( bitems[i], 1 );
							setserial( bitems[i], p, 1 );
							items[bitems[i]].amount = 1;
							RefreshItem( bitems[i] ); // AntiChrist
						}
						break;
					default:
						printf("ERROR: Fallout of switch statement without default. uox3.cpp, buyaction()\n"); //Morrolan
					}
				}
			}
		}
	}
	else
		npctalkall( npc, "Alas, thou dost not possess sufficient gold for this purchase!", 0 );
	
	if( clear )
	{
		clearmsg[0] = 0x3B;
		clearmsg[1] = 0x00;
		clearmsg[2] = 0x08;
		clearmsg[3] = buffer[s][3];
		clearmsg[4] = buffer[s][4];
		clearmsg[5] = buffer[s][5];
		clearmsg[6] = buffer[s][6];
		clearmsg[7] = 0x00;
		Network->xSend( s, clearmsg, 8, 0 );
	}
	Weight->NewCalc( currchar[s] );  // Ison 2-20-99
	statwindow( s, currchar[s] );
}




//NEW RESTOCKITEM FUNCTION STARTS HERE -- AntiChrist merging codes -- (26/4/99)
void restockitem(int i, unsigned int currenttime)
{
	int a,serial,ci;
	if (items[i].restock && items[i].cont1>=0x40 && (shoprestocktime<=currenttime||(overflow)))
	{
		serial=items[i].contserial;
		if( serial == -1 ) return;
		ci=findbyserial(&itemsp[serial%HASHMAX], serial, 0);
		if ( ci != -1 && items[ci].layer == 0x1A && items[i].restock>0 )
		{
			a=min(items[i].restock, (items[i].restock/2)+1);
			items[i].amount += a;
			items[i].restock -= a;
		}
	}
}
//NEW RESTOCKITEM FUNCTION ENDS HERE -- AntiChrist merging codes --
void restock(int s)
{
	int i, a, ci;
	for (i = 0; i < itemcount; i++)
	{
		// Dupois - added this check to only restock items that ... well ... have a restock value >0
		// Added Oct 25, 1998
		if (items[i].restock && items[i].cont1 >= 0x40)
		{
			if (items[i].contserial > -1)
			{
				ci = calcItemFromSer(items[i].contserial);
				if (ci != -1 && items[ci].layer == 0x1A)
				{
					int tChar = GetPackOwner(ci);
					if (tChar != -1)
					{
						if (chars[tChar].npc && chars[tChar].shop)
						{
							if (s)
							{
								items[i].amount += items[i].restock;
								items[i].restock = 0;
							}
							else
							{
								if (items[i].restock > 0)
								{
									a = min(items[i].restock, (items[i].restock/2) + 1);
									items[i].amount += a;
									items[i].restock -= a;
								}
							}
						}
					}
				}
			}
			// MAgius(CHE): All items in shopkeeper need a new randomvaluerate.
			if (server_data.trade_system == 1)
				StoreItemRandomValue(i, -1); // Magius(CHE) (2)
		}
	}// printf(UOX3:  restock() - time to execute =%d\n", (uiCurrentTime-tt));
}



void playmonstersound( CHARACTER monster, int id1, int id2, MonsterSoundType sfx)
// Slightly revised by Leafbeach in November-December 2000 (fixed combat sounds for soundflag 2 and 3)
{
	int basesound=0,x;
#ifdef __LINUX__
	char sf; short offset;
#else
	char sf,offset;
#endif
	
	x = ( id1<<8 ) + id2;
	basesound = creatures[x].basesound;
	sf = creatures[x].soundflag;
	offset = sfx;
	
	if( basesound != 0 )
	{
		switch(sf)
		{
		case 0: ; break; // in normal case the offset is correct
		case 1: ; break; // birds sounds will be implmented later
		case 2:  // only attack, defend & dying sounds available
			switch( sfx )
			{
			case 2:	offset = 0;	break;	// idle, defend ? play nothing
			case 3:	offset = 1;	break;	// correct offset
			case 4:	offset = 2;	break;
			}
			break;
        case 3: // only start-attack, attack, defense & dying
			switch( sfx )
			{
			case 1: offset = 0;	break; // idle -> play nothing
			case 2: offset = 1;	break; // otherwise correct offsets
			case 3: offset = 2;	break;
			case 4: offset = 3;	break;
			}
            break;
		case 4: // only a single sound
			if( sfx != 0 ) 
				offset = -1; 
			else 
				offset = 0;
			break;
		}
		
		basesound = basesound + offset;
		
		if( offset != -1 ) 
			soundeffect2( monster, basesound>>8, basesound%256 );
	}
}

// npc - npc vendor index into chars[]
// p = containers index in items[]
// q = vendors item index in items[] that we are trying to find a match for
// m1 = message 1
// m1t = message 1 total size
void getsellsubitem(int npc, int p, int q, unsigned char *m1, int &m1t)
{
	int i,serial,serhash,ci,value,z;
	char ciname[256];
	char cinam2[256];
	char itemname[256];
	
	serial=items[p].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if ((i > -1) && items[i].contserial==serial)
		{
			strcpy( ciname, items[i].name );
			strcpy( cinam2, items[q].name );
			strupr( ciname );
			strupr( cinam2 );
			
			if (items[i].type==1)
				getsellsubitem( npc, i, q, m1, m1t );
			else if ((items[i].contserial==serial) &&
				(items[i].id1==items[q].id1) && (items[i].id2==items[q].id2) &&
				(items[i].type==items[q].type) && (m1[8]<60) &&
				(( server_data.sellbyname == 0 ) || ( server_data.sellbyname == 1 && (!strcmp( ciname, cinam2 )))))
			{
				m1[m1t+0]=items[i].ser1;
				m1[m1t+1]=items[i].ser2;
				m1[m1t+2]=items[i].ser3;
				m1[m1t+3]=items[i].ser4;
				m1[m1t+4]=items[i].id1;
				m1[m1t+5]=items[i].id2;
				m1[m1t+6]=items[i].color1;
				m1[m1t+7]=items[i].color2;
				m1[m1t+8]=items[i].amount>>8;
				m1[m1t+9]=items[i].amount%256;
				value=items[q].value;
				value=calcValue(i, value);
				if( server_data.trade_system == 1 )
					value = calcGoodValue( npc, q, value, 1 ); // by Magius(CHE)
				m1[m1t+10]=value>>8;
				m1[m1t+11]=value%256;
				m1[m1t+12]=0;// Unknown... 2nd length byte for string?
				m1[m1t+13]=getname(i, itemname);
				m1t+=14;
				for(z=0;z<m1[m1t-1];z++)
				{
					m1[m1t+z]=itemname[z];
				}
				m1t+=m1[m1t-1];
				m1[8]++;
			}
		}
	}
	return;
}



int sellstuff( UOXSOCKET s, int i)
{
	char itemname[256];
	int vendorPack = -1, j, q, m1t, pack, z, value;
	int serial,serhash,ci,serial1,serhash1,ci1;
	unsigned char m1[2048];
	char m2[2];
	char ciname[256]; // By Magius(CHE)
	char cinam2[256]; // By Magius(CHE)
	
	serial = chars[i].serial;
	serhash = serial%HASHMAX;
	for( ci = 0; ci < contsp[serhash].max; ci++ )			// to find the sell layer of the vendor?
	{
		q = contsp[serhash].pointer[ci];
		if( q != -1 )
		{
			if( items[q].contserial == serial && items[q].layer == 0x1C )
			{
				vendorPack = q;
				break;
			}
		}
	}
	if( vendorPack == -1 ) 
		return 0;	// no layer
	
	m2[0] = 0x33;
	m2[1] = 0x01;
	Network->xSend(s, m2, 2, 0);
	
	pack = packitem( currchar[s] );				// no pack for the player
	if( pack == -1 ) 
		return 0;
	
	m1[0] = 0x9E; // Header
	m1[1] = 0; // Size
	m1[2] = 0; // Size
	m1[3] = chars[i].ser1;
	m1[4] = chars[i].ser2;
	m1[5] = chars[i].ser3;
	m1[6] = chars[i].ser4;
	m1[7] = 0; // Num items
	m1[8] = 0; // Num items
	m1t=9;
	
	
	serial = items[vendorPack].serial;
	serhash = serial%HASHMAX;
	for( ci = 0; ci < contsp[serhash].max; ci++ )		// each item in the p pack
	{
		q = contsp[serhash].pointer[ci];
		if( q != -1 )
		{
			if( items[q].contserial == serial )
			{
				serial1 = items[pack].serial;
				serhash1 = serial1%HASHMAX;
				for( ci1 = 0; ci1 < contsp[serhash1].max; ci1++ )
				{
					j = contsp[serhash1].pointer[ci1];
					if( j != -1 )
					{
						if( items[j].contserial == serial1 )
						{
							strcpy( ciname, items[j].name );
							strcpy( cinam2, items[q].name );
							strupr( ciname );
							strupr( cinam2 );
							if( items[j].type == 1 )
								getsellsubitem( i, j, q, m1, m1t );
							else if( (items[j].id1==items[q].id1) && (items[j].id2==items[q].id2) &&
								(items[j].type==items[q].type) && (m1[8]<60) &&
								(( server_data.sellbyname == 0 ) || ( server_data.sellbyname == 1 && (!strcmp( ciname, cinam2 ))))) // If the names are the same! 
							{
								if( !items[j].pileable && items[j].amount > 1 )
								{
									if( ( items[j].type <= 60 || items[j].type >= 70 ) && items[j].type != 125 ) // not a spawner
									{
										items[j].amount = 1;
										RefreshItem( j );
									}
								}
								m1[m1t+0] = items[j].ser1;
								m1[m1t+1] = items[j].ser2;
								m1[m1t+2] = items[j].ser3;
								m1[m1t+3] = items[j].ser4;
								m1[m1t+4] = items[j].id1;
								m1[m1t+5] = items[j].id2;
								m1[m1t+6] = items[j].color1;
								m1[m1t+7] = items[j].color2;
								m1[m1t+8] = items[j].amount>>8;
								m1[m1t+9] = items[j].amount%256;
								value = items[q].value;
								value = calcValue( j, value );
								if( server_data.trade_system == 1 )
									value = calcGoodValue( i, j, value, 1 ); // by Magius(CHE)
								m1[m1t+10] = value>>8;
								m1[m1t+11] = value%256;
								m1[m1t+12] = 0;// Unknown... 2nd length byte for string?
								m1[m1t+13] = getname(j, itemname);
								m1t = m1t + 14;
								for( z = 0; z < m1[m1t-1]; z++ )
									m1[m1t+z] = itemname[z];
								m1t = m1t+m1[m1t-1];
								m1[8]++;
							}
						}
					}
				}
			}
		}
	}
	
	m1[1] = m1t>>8;
	m1[2] = m1t%256;
	if( m1[8] != 0 )
	{
		Network->xSend(s, m1, m1t, 0);
	}
	else
	{
		npctalkall( i, "Thou doth posses nothing of interest to me.", 0 );
	}
	m2[0] = 0x33;
	m2[1] = 0x00;
	Network->xSend( s, m2, 2, 0 );
	return 1;
}

void sellaction( UOXSOCKET s )
{
	int n, npa = -1, npb = -1, npc = -1, i, j, k, value=0, join, totgold=0, z;
	int serial,serhash,ci;
	char clearmsg[9];
	
	unsigned int maxsell, amt; // by Magius(CHE)
	char tmpmsg[256]; // by Magius(CHE)
	tmpmsg[0] = 0;

	if( buffer[s][8] != 0 )
	{
		n = calcCharFromSer(buffer[s][3], buffer[s][4], buffer[s][5], buffer[s][6]);
		serial = chars[n].serial;
		serhash = serial%HASHMAX;
		for( ci = 0; ci < contsp[serhash].max; ci++ )
		{
			i = contsp[serhash].pointer[ci];
			if( i != -1 )
				if( items[i].contserial == serial )
				{
					if( items[i].layer == 0x1A ) 
						npa = i;
					if( items[i].layer == 0x1B ) 
						npb = i;
					if( items[i].layer == 0x1C ) 
						npc = i;
				}
		}
		// Pre Calculate Total Amount of selling items to STOPS if the items if greater than SELLMAXITEM - Magius(CHE)
		maxsell = 0;
		for( i = 0; i < buffer[s][8]; i++ )
		{
			j=calcItemFromSer(buffer[s][9+((6*i)+0)], buffer[s][9+((6*i)+1)],
				buffer[s][9+((6*i)+2)], buffer[s][9+((6*i)+3)]);
			amt=(256*buffer[s][9+((6*i)+4)])+buffer[s][9+((6*i)+5)];
			maxsell += amt;
		}
		
		// STOP Message - Magius(CHE)
		if( maxsell > server_data.sellmaxitem )
		{
			sprintf( tmpmsg, "Sorry %s but I can buy only %i items at a time!", chars[currchar[s]].name, server_data.sellmaxitem );
			npctalkall( n, tmpmsg, 0 );
			return;
		}
		// End MaxItems Sell Addon - Magius(CHE)
		int tNum = 0;
		for( i = 0; i < buffer[s][8]; i++ )
		{
			tNum = 6 * i;
			j = calcItemFromSer( buffer[s][9 + tNum], buffer[s][10 + tNum], buffer[s][11 + tNum], buffer[s][12 + tNum] );
			amt = ((buffer[s][13 + tNum])<<8) + buffer[s][14 + tNum];
//			j=calcItemFromSer(buffer[s][9+((6*i)+0)], buffer[s][9+((6*i)+1)],
//				buffer[s][9+((6*i)+2)], buffer[s][9+((6*i)+3)]);
//			amt = (256*buffer[s][9+((6*i)+4)])+buffer[s][9+((6*i)+5)];
			if( j != -1 )
			{
				if( items[j].amount < amt )
				{
					npctalkall(n, "Cheating scum! Leave now, before I call the guards!", 0);
					return;
				}
				join = -1;
				if( npa != -1 )
				{
					serial = items[npa].serial;
					serhash = serial%HASHMAX;
					for( ci = 0; ci < contsp[serhash].max; ci++ )
					{
						k = contsp[serhash].pointer[ci];
						if( k != -1 )
						{
							if( items[k].contserial == serial )
							{
								if( items[k].id1 == items[j].id1 && items[k].id2 == items[j].id2 &&
									items[j].type==items[k].type)
								{
									join = k;
								}
							}
						}
					}
				}
				if( npc != -1 )
				{
					serial = items[npc].serial;
					serhash = serial%HASHMAX;
					for( ci = 0; ci < contsp[serhash].max; ci++ )
					{
						k = contsp[serhash].pointer[ci];
						if( k != -1 )
						{
							if( ( items[k].contserial == serial ) &&
								( items[k].id1 == items[j].id1 ) && ( items[k].id2 == items[j].id2 ) &&
								( items[j].type == items[k].type ) )
							{
								value = items[k].value;
								value = calcValue( j, value );
	//							if( server_data.trade_system == 1 ) value=calcGoodValue( s, j, value, 1 );
							}
						}
					}
				}
				if( join != -1 )
				{
					items[join].amount += amt;
					items[join].restock -= amt;
					totgold += ( amt * value );
					if( items[j].amount == amt )
						Items->DeleItem( j );
					else
					{
						items[j].amount -= amt;
						RefreshItem( j );
					}
				}
				else
				{
					if( npb != -1 )
					{
						totgold += ( amt * value );
						unsetserial( j, 1 );
						setserial( j, npb, 1 );
						removeitem[1] = items[j].ser1;
						removeitem[2] = items[j].ser2;
						removeitem[3] = items[j].ser3;
						removeitem[4] = items[j].ser4;
						for( z = 0; z < now; z++ )
						{
							if( perm[z] ) 
								Network->xSend( z, removeitem, 5, 0 );
						}
						if( items[j].amount != amt ) 
							Commands->DupeItem( s, j, items[j].amount - amt );
					}
				}
			} 
		}// end of != -1
		addgold( s, totgold );
		goldsfx( s, totgold );
	}
	
	clearmsg[0] = 0x3B;
	clearmsg[1] = 0x00;
	clearmsg[2] = 0x08;
	clearmsg[3] = buffer[s][3];
	clearmsg[4] = buffer[s][4];
	clearmsg[5] = buffer[s][5];
	clearmsg[6] = buffer[s][6];
	clearmsg[7] = 0x00;
	Network->xSend( s, clearmsg, 8, 0 );
}

void addgold(int s, int totgold)
{
	int c;
	c=Items->SpawnItem(s,totgold,"#",1,0x0E,0xED,0,0,1,1);
}

void playmidi(int s, char num1, char num2)
{
	char msg[3];
	msg[0]=0x6D;
	msg[1]=num1;
	msg[2]=num2;
	Network->xSend(s, msg, 3, 0);
}


void usepotion(int p, int i)//Reprogrammed by AntiChrist
{
	int s/*, j*/, x;
	
	s=calcSocketFromChar(p);
	if( server_data.potiondelay != 0 )
		tempeffect( p, p, 25, 0, 0, 0 );
	//printf("Used potion %i!\n",items[i].morey);	
	switch(items[i].morey)
	{
	case 1: // Agility Potion
		staticeffect(p, 0x37, 0x3a, 0, 15); 
		switch(items[i].morez)
		{
		case 1:
			tempeffect(currchar[s], p, 6, 5+RandomNum(1,10), 0, 0);
			sysmessage(s, "You feel more agile!");
			break;
		case 2:
			tempeffect(currchar[s], p, 6, 10+RandomNum(1,20), 0, 0);
			sysmessage(s, "You feel much more agile!");
			break;
		default:
			printf("ERROR: Fallout of switch statement without default. uox3.cpp, usepotion()\n"); //Morrolan
			return;
		}
		soundeffect2(p, 0x01, 0xE7);
		if (s!=-1) updatestats(p, 2);
		break;
	case 2: // Cure Potion
		if (chars[p].poisoned<1) sysmessage(s,"The potion had no effect.");
		else
		{
			switch(items[i].morez)
			{
			case 1:
				x=RandomNum(1,100);
				if (chars[p].poisoned==1 && x<81) chars[p].poisoned=0;
				if (chars[p].poisoned==2 && x<41) chars[p].poisoned=0;
				if (chars[p].poisoned==3 && x<21) chars[p].poisoned=0;
				if (chars[p].poisoned==4 && x<6)  chars[p].poisoned=0;
				break;
			case 2:
				x=RandomNum(1,100);
				if (chars[p].poisoned==1) chars[p].poisoned=0;
				if (chars[p].poisoned==2 && x<81) chars[p].poisoned=0;
				if (chars[p].poisoned==3 && x<41) chars[p].poisoned=0;
				if (chars[p].poisoned==4 && x<21)  chars[p].poisoned=0;
				break;
			case 3:
				x=RandomNum(1,100);
				if (chars[p].poisoned==1) chars[p].poisoned=0;
				if (chars[p].poisoned==2) chars[p].poisoned=0;
				if (chars[p].poisoned==3 && x<81) chars[p].poisoned=0;
				if (chars[p].poisoned==4 && x<61)  chars[p].poisoned=0;
				break;
			default:
				printf("ERROR: Fallout of switch statement without default. uox3.cpp, usepotion()\n"); //Morrolan
				return;
			}
			
			if (chars[p].poisoned) sysmessage(s,"The potion was not able to cure this poison."); else
			{
				staticeffect(p, 0x37, 0x3A, 0, 15);
				soundeffect2(p, 0x01, 0xE0); //cure sound - SpaceDog
				sysmessage(s,"The poison was cured.");
			} 
			
			
		}
		impowncreate(calcSocketFromChar(p),p,1); //Lb, makes the green bar blue or the blue bar blue ! 
		break; // ANTICHRIST -----
	case 3: // Explosion Potion
		if( region[chars[currchar[s]].region].priv&0x01 == 1 ) // Ripper 11-14-99
		{
			sysmessage( s, "You can't use that in town!" );
			return;
		}
		addid1[s] = items[i].ser1;
		addid2[s] = items[i].ser2;
		addid3[s] = items[i].ser3;
		addid4[s] = items[i].ser4;
		strcpy(temp, "Now would be a good time to throw it!");
		sysmessage( s, temp ); 
		tempeffect( currchar[s], currchar[s], 16, 0, 1, 3 );
		tempeffect( currchar[s], currchar[s], 16, 0, 2, 2 );
		tempeffect( currchar[s], currchar[s], 16, 0, 3, 1 );
		tempeffect2( currchar[s], i, 17, 0, 4, 0 );
		target( s, 0, 1, 0, 207, "" );
		return;
		break;
	case 4: // Heal Potion
		switch( items[i].morez )
		{
		case 1:
			chars[p].hp=min(chars[p].hp+5+RandomNum(1,5)+chars[p].skill[17]/100,chars[p].st);
			sysmessage(s, "You feel a little better!");
			break;
		case 2:
			chars[p].hp=min(chars[p].hp+15+RandomNum(1,10)+chars[p].skill[17]/50, chars[p].st);
			sysmessage(s, "You feel better!");
			break;
		case 3:
			chars[p].hp=min(chars[p].hp+20+RandomNum(1,20)+chars[p].skill[17]/40, chars[p].st);
			sysmessage(s, "You feel much better!");
			break;
		default:
			printf("ERROR: Fallout of switch statement without default. uox3.cpp, usepotion()\n"); //Morrolan
			return;
		}
		if (s!=-1) updatestats(p, 0);
		staticeffect(p, 0x37, 0x6A, 0x09, 0x06); // Sparkle effect
		soundeffect2(p, 0x01, 0xF2); //Healing Sound - SpaceDog
		break;
	case 5: // Night Sight Potion
		//{
		staticeffect(p, 0x37, 0x6A, 0x09, 0x06);
		tempeffect(currchar[s], p, 2, 0, 0, 0);
		soundeffect2(p, 0x01, 0xE3);
		//printf("POTN: Nightsight used by %s\n",chars[p].name);
		break;
		//}
	case 6: // Poison Potion
		if(chars[p].poisoned<items[i].morez) chars[p].poisoned=items[i].morez;
		if(items[i].morez>4) items[i].morez=4;
		chars[p].poisonwearofftime = (unsigned int)(uiCurrentTime+(CLOCKS_PER_SEC*server_data.poisontimer)); // lb, poison wear off timer setting
		impowncreate(calcSocketFromChar(p),p,1); //Lb, sends the green bar ! 
		soundeffect2(p, 0x02, 0x46); //poison sound - SpaceDog
		sysmessage(s, "You poisoned yourself! *sigh*"); //message -SpaceDog
		break;
	case 7: // Refresh Potion
		switch(items[i].morez)
		{
		case 1:
			chars[p].stm=min(chars[p].stm+20+RandomNum(1,10), chars[p].dx);
			sysmessage(s, "You feel more energetic!");
			break;
		case 2:
			chars[p].stm=min(chars[p].stm+40+RandomNum(1,30), chars[p].dx);
			sysmessage(s, "You feel much more energetic!");
			break;
		default:
			printf("ERROR: Fallout of switch statement without default. uox3.cpp, usepotion()\n"); //Morrolan
			return;
		}
		if (s!=-1) updatestats(p, 2);
		staticeffect(p, 0x37, 0x6A, 0x09, 0x06); // Sparkle effect
		soundeffect2(p, 0x01, 0xF2); //Healing Sound
		break;
	case 8: // Strength Potion
		staticeffect(p, 0x37, 0x3a, 0, 15); 
		switch(items[i].morez)
		{
		case 1:
			tempeffect(currchar[s], p, 8, 5+RandomNum(1,10), 0, 0);
			sysmessage(s, "You feel stronger!");
			break;
		case 2:
			tempeffect(currchar[s], p, 8, 10+RandomNum(1,20), 0, 0);
			sysmessage(s, "You feel much stronger!");
			break;
		default:
			printf("ERROR: Fallout of switch statement without default. uox3.cpp, usepotion()\n"); //Morrolan
			return;
		}
		soundeffect2(p, 0x01, 0xEE);     
		break;
	case 9: // Mana Potion
		switch(items[i].morez)
		{
		case 1:
			chars[p].mn=min( chars[p].mn+10 + items[i].morex/100, (unsigned int)chars[p].in);
			break;
		case 2:
			chars[p].mn=min(chars[p].mn+20+items[i].morex/50, (unsigned int)chars[p].in);
			break;
		default:
			printf("ERROR: Fallout of switch statement without default. uox3.cpp, usepotion()\n"); //Morrolan
			return;
		}
		if (s!=-1) updatestats(p, 1);
		staticeffect(p, 0x37, 0x6A, 0x09, 0x06); // Sparkle effect
		soundeffect2(p, 0x01, 0xE7); //agility sound - SpaceDog
		break;
	case 10: // LB's LSD potion, 5'th November 1999
		if( !(items[i].id1 == 0x18 && items[i].id2 == 0x41 ) ) return; // only works with an special flask
		if( s == -1 ) return;
		if( LSD[s] == 1 )
		{
			sysmessage( s, "No,no,no,can't you get enough ?" );
			return;
		}
		tempeffect( p, p, 20, 60+RandomNum( 1, 120 ), 0, 0 ); // trigger effect
		staticeffect( p, 0x37, 0x6A, 0x09, 0x06 ); // Sparkle effect
		soundeffect5( calcSocketFromChar( p ), 0x00, 0xF8 ); // lsd sound :)
		break;
							
	default:
		printf("ERROR: Fallout of switch statement without default. uox3.cpp, usepotion()\n"); //Morrolan
		return;
	}
	soundeffect2(p, 0x00, 0x30);
	if( chars[p].id1 >=1 && chars[p].id2 > 90 && chars[p].onhorse == 0 )
		npcaction(p, 0x22);
	//empty bottle after drinking - Tauriel
	if (items[i].amount!=1)
	{
		items[i].amount--;
	}
	//empty bottle after drinking - Tauriel
	if( items[i].contserial != -1 )
		removefromptr(&contsp[items[i].contserial%HASHMAX], i);
	if (items[i].morey!=3)
	{
		int lsd = items[i].morey; // save morey before overwritten
		unsigned char k1 = items[i].ser1;
		unsigned char k2 = items[i].ser2;
		unsigned char k3 = items[i].ser3;
		unsigned char k4 = items[i].ser4;
		int kser = items[i].serial;
		Items->InitItem(i, 0);
		items[i].ser1 = k1;
		items[i].ser2 = k2;
		items[i].ser3 = k3;
		items[i].ser4 = k4;
		items[i].serial = kser;
		items[i].id1 = 0x0F;
		items[i].id2 = 0x0E;
		if( lsd == 10 ) // empty Lsd potions
		{
			items[i].id1 = 0x18;
			items[i].id2 = 0x3D;
		}
		items[i].pileable = 1;
		mapRegions->RemoveItem( i );
		int pItem = packitem( p );
		if( pItem != -1 )
		{
			items[i].cont1 = items[pItem].ser1;
			items[i].cont2 = items[pItem].ser2;
			items[i].cont3 = items[pItem].ser3;
			items[i].cont4 = items[pItem].ser4;
			items[i].contserial = items[pItem].serial;
		}
		else
		{

			items[i].x = chars[p].x;
			items[i].y = chars[p].y;
			items[i].z = chars[p].z;
			mapRegions->AddItem( i );
		}
		items[i].priv |= 0x01;
		//AntiChrist fix with Zippy code --

	}
	else
	{
		Items->DeleItem(i);
	}
	RefreshItem( i ); // AntiChrist
}

int calcValue(int i, int value)
{
	int mod=10;
	
	if (items[i].type==19)
	{
		if (items[i].morex>500) mod=mod+1;
		if (items[i].morex>900) mod=mod+1;
		if (items[i].morex>1000) mod=mod+1;
		if (items[i].morez>1) mod=mod+(3*(items[i].morez-1));
		value=(value*mod)/10;
	}
	
	// Lines added for Rank System by Magius(CHE)
	if( items[i].rank > 0 && items[i].rank < 10 && server_data.rank_system == 1 )
	{
		value = (int)( items[i].rank * value ) / 10;
	}
	if( value < 1 ) value = 1;
	// end addon
	
	// Lines added for Trade System by Magius(CHE) (2)
	if( !items[i].rndvaluerate ) items[i].rndvaluerate = 0;
	if( items[i].rndvaluerate != 0 && server_data.trade_system == 1 )
	{
		value += (int)(value*items[i].rndvaluerate)/1000;
	}
	if( value < 1 ) value = 1;
	// end addon
	return value;
}

int calcGoodValue( int npcnum2, int i, int value, int goodtype )
{ // Function Created by Magius(CHE) for trade System 
	int actreg = calcRegionFromXY( chars[npcnum2].x, chars[npcnum2].y );
	int good = items[i].good;
	int regvalue = 0;
	int x;
	
	if( good <= -1 )
		return value;
	
	if( goodtype == 1 )
		regvalue = region[actreg].goodsell[items[i].good]; //  Vendor SELL
	if( goodtype == 0 )
		regvalue = region[actreg].goodbuy[items[i].good]; // Vendor BUY
	
	x = (int)( value * abs( regvalue ) ) / 1000;
	
	if( regvalue < 0 )
		value -= x;
	else
		value += x;
	
	if( value <= 0 )
		value = 1; // Added by Magius(CHE) (2)
	
	return value;
}

// Functions created by Magius(CHE)
// This function calculate the last container of an item
// and return its Number.
// return script1="NPC" if last container is a npc
// return script1="ITEM" if last container is an item
int calcLastContainerFromSer( int ser )
{ // Magius(CHE) (2)
	int a = -1, exi = 0, newser = ser;
	do
	{
		a = calcCharFromSer( newser );
		if( a > -1 )
		{
			newser = chars[a].serial;
			exi = 2;
		}
		else
		{
			a = calcItemFromSer( newser );
			if( a > -1 )
			{
				newser = items[a].contserial;
				if( newser == items[a].serial )
					printf( "UOX3.CPP: Loop error in calcLastContainerFromSer().\n   Item [%i] [ID: %x%x] %s has contained in itself.\n   Check CONT variables into WSC!\n", a, items[a].id1, items[a].id2, items[a].name );
			}
			else
				exi = 1;
		}
	} while( exi == 0 );
	
	if( exi == 2 )
	{
		strcpy( script1, "NPC" );
		return a; // npc
	}
	else
	{
		strcpy( script1, "ITEM" );
		return a; // item
	}
}

void StoreItemRandomValue( int i, int tmpreg )
{ // Function Created by Magius(CHE) (2) for trade System
	int max = 0, min = 0;
	int a = -1;
	if( items[i].good < 0 ) return;
	if( tmpreg < 0 )
	{
		a = calcLastContainerFromSer( items[i].contserial );
		if( a > -1 )
		{
			if(!(strcmp(script1, "NPC" ) ) ) tmpreg = calcRegionFromXY( chars[a].x, chars[a].y );
			if(!(strcmp(script1, "ITEM" ) ) ) tmpreg = calcRegionFromXY( items[a].x, items[a].y );
		}
	}
	
	if( tmpreg < 0 )
		return;
	
	min = region[tmpreg].goodrnd1[items[i].good];
	max = region[tmpreg].goodrnd2[items[i].good];
	
	if( max != 0 || min != 0 )
	{
		items[i].rndvaluerate = (int) RandomNum( min, max );
	}
}
int tradestart(int s, int i)
{
	int ps, pi, bps, bpi, s2,c;
	char msg[90];
	
	bps=packitem(currchar[s]);
	bpi=packitem(i);
	s2=calcSocketFromChar(i);
	
	if (bps==-1) //LB
	{
		sysmessage(s,"Time to buy a backpack!");
		sprintf(temp,"%s doesnt have a backpack!",chars[currchar[s]].name);
		sysmessage(s2,temp);
		return 0;
	}
	
	if (bpi==-1)
	{
		sysmessage(s2,"Time to buy a backpack!");
		sprintf(temp,"%s doesnt have a backpack!",chars[currchar[s2]].name);
		sysmessage(s,temp);
		return 0;
	}
	
	
	c=Items->SpawnItem(s2,1,"#",0,0x1E,0x5E,0,0,0,0);
	if( c == -1 ) 
		return 0;
	items[c].x=26;
	items[c].y=0;
	items[c].z=0;
	setserial(c,currchar[s],4);
	items[c].layer=0;
	items[c].type=1;
	items[c].dye=0;
	ps=c;
	sendbpitem(s, ps);
	if (s2!=-1) 
		sendbpitem(s2, ps);
	
	c=Items->SpawnItem(s2,1,"#",0,0x1E,0x5E,0,0,0,0);
	if( c == -1 ) 
		return 0;
	items[c].x=26;
	items[c].y=0;
	items[c].z=0;
	setserial(c,i,4);
	items[c].layer=0;
	items[c].type=1;
	items[c].dye=0;
	pi=c;
	sendbpitem(s, pi);
	if (s2!=-1) 
		sendbpitem(s2, pi);
	
	items[pi].moreb1=items[ps].ser1;
	items[pi].moreb2=items[ps].ser2;
	items[pi].moreb3=items[ps].ser3;
	items[pi].moreb4=items[ps].ser4;
	items[ps].moreb1=items[pi].ser1;
	items[ps].moreb2=items[pi].ser2;
	items[ps].moreb3=items[pi].ser3;
	items[ps].moreb4=items[pi].ser4;
	items[pi].morez=0;
	items[ps].morez=0;
	
	msg[0]=0x6F; // Header Byte
	msg[1]=0; // Size
	msg[2]=47; // Size
	msg[3]=0; // Initiate
	msg[4]=chars[i].ser1;
	msg[5]=chars[i].ser2;
	msg[6]=chars[i].ser3;
	msg[7]=chars[i].ser4;
	msg[8]=items[ps].ser1;
	msg[9]=items[ps].ser2;
	msg[10]=items[ps].ser3;
	msg[11]=items[ps].ser4;
	msg[12]=items[pi].ser1;
	msg[13]=items[pi].ser2;
	msg[14]=items[pi].ser3;
	msg[15]=items[pi].ser4;
	msg[16]=1;
	strcpy(&(msg[17]), chars[i].name);
	Network->xSend(s, msg, 47, 0);
	
	msg[0]=0x6F; // Header Byte
	msg[1]=0; // Size
	msg[2]=47; // Size
	msg[3]=0; // Initiate
	msg[4]=chars[currchar[s]].ser1;
	msg[5]=chars[currchar[s]].ser2;
	msg[6]=chars[currchar[s]].ser3;
	msg[7]=chars[currchar[s]].ser4;
	msg[8]=items[pi].ser1;
	msg[9]=items[pi].ser2;
	msg[10]=items[pi].ser3;
	msg[11]=items[pi].ser4;
	msg[12]=items[ps].ser1;
	msg[13]=items[ps].ser2;
	msg[14]=items[ps].ser3;
	msg[15]=items[ps].ser4;
	msg[16]=1;
	strcpy(&(msg[17]), chars[currchar[s]].name);
	if (s2!=-1) 
		Network->xSend(s2, msg, 47, 0);
	
	return ps;
}

void clearalltrades()
{
	int i, j, k, p,serial,serhash,ci;
	for (i=0;i<itemcount;i++)
	{
		if (items[i].type==1 && items[i].x==26 && items[i].y==0 && items[i].z==0 &&
			items[i].id1==0x1E && items[i].id2==0x5E)
		{
			k = calcCharFromSer( items[i].contserial );
			p=packitem(k);
			serial=items[i].serial;
			serhash=serial%HASHMAX;
			for (ci=0;ci<contsp[serhash].max;ci++)
			{
				j=contsp[serhash].pointer[ci];
				if (j!=-1)
					if ((items[j].contserial==serial))
					{
						if( p > -1 )
						{
							unsetserial( j, 1 );
							setserial(j,p,1);
						}
					}
			}
			Items->DeleItem(i);
			printf("Trade cleared\n");
		}
	}
}

void trademsg(int s)
{
	int cont1 = -1, cont2 = -1;
	switch(buffer[s][3])
	{
	case 0://Start trade - Never happens, sent out by the server only.
		break;
	case 1://Cancel trade.  Send each person cancel messages, move items.
		endtrade(buffer[s][4], buffer[s][5], buffer[s][6], buffer[s][7]);
		break;
	case 2://Change check marks.  Possibly conclude trade
		cont1=calcItemFromSer(buffer[s][4], buffer[s][5], buffer[s][6], buffer[s][7]);
		if( cont1 > -1 )
			cont2=calcItemFromSer(items[cont1].moreb1, items[cont1].moreb2, items[cont1].moreb3, items[cont1].moreb4);
		if( cont2 > -1 ) // lb crashfix
		{
			items[cont1].morez=buffer[s][11];
			sendtradestatus(cont1, cont2);
			if (items[cont1].morez && items[cont2].morez)
			{
				dotrade(cont1, cont2);
				endtrade(buffer[s][4], buffer[s][5], buffer[s][6], buffer[s][7]);
			}
		}
		break;
	default:
		printf("ERROR: Fallout of switch statement without default. uox3.cpp, trademsg()\n"); //Morrolan
	}
}

void sendtradestatus(int cont1, int cont2)
{
	char msg[30];
	int p1, p2, s1, s2;
	
	p1 = calcCharFromSer( items[cont1].contserial );
	p2 = calcCharFromSer( items[cont2].contserial );
	s1 = calcSocketFromChar( p1 );
	s2 = calcSocketFromChar( p2 );
	
	msg[0] = 0x6F;//Header
	msg[1] = 0x00;//Size
	msg[2] = 0x11;//Size
	msg[3] = 0x02;//State
	msg[4] = items[cont1].ser1;
	msg[5] = items[cont1].ser2;
	msg[6] = items[cont1].ser3;
	msg[7] = items[cont1].ser4;
	msg[8] = 0;
	msg[9] = 0;
	msg[10] = 0;
	msg[11] = (unsigned char)(items[cont1].morez%256);
	msg[12] = 0;
	msg[13] = 0;
	msg[14] = 0;
	msg[15] = (unsigned char)(items[cont2].morez%256);
	msg[16] = 0; // No name in this message
	if (s1!=-1) 
		Network->xSend(s1, msg, 17, 0);
	
	msg[4] = items[cont2].ser1;
	msg[5] = items[cont2].ser2;
	msg[6] = items[cont2].ser3;
	msg[7] = items[cont2].ser4;
	msg[11] = (unsigned char)(items[cont2].morez%256);
	msg[15] = (unsigned char)(items[cont1].morez%256);
	if (s2!=-1) 
		Network->xSend(s2, msg, 17, 0);
}

void endtrade(int b1, int b2, int b3, int b4)
{
	int cont1, cont2, p1, p2, bp1, bp2, s1, s2, i;
	int serial,serhash,ci;
	char msg[30];
	
	cont1 = calcItemFromSer(b1, b2, b3, b4);
	if( cont1 < 0 )
		return; // LB, crashfix
	cont2=calcItemFromSer(items[cont1].moreb1, items[cont1].moreb2, items[cont1].moreb3, items[cont1].moreb4);
	if( cont2 < 0 )
		return; // LB, crashfix
	p1 = calcCharFromSer( items[cont1].contserial );
	p2 = calcCharFromSer( items[cont2].contserial );
	bp1=packitem(p1);
	if( bp1 < 0 ) 
		return;
	bp2=packitem(p2);
	if( bp2 < 0 ) 
		return;
	s1 = calcSocketFromChar( p1 );
	s2 = calcSocketFromChar( p2 );
	
	msg[0]=0x6F;//Header Byte
	msg[1]=0x00;//Size
	msg[2]=0x11;//Size
	msg[3]=0x01;//State byte
	msg[4]=items[cont1].ser1;
	msg[5]=items[cont1].ser2;
	msg[6]=items[cont1].ser3;
	msg[7]=items[cont1].ser4;
	msg[8]=0;
	msg[9]=0;
	msg[10]=0;
	msg[11]=0;
	msg[12]=0;
	msg[13]=0;
	msg[14]=0;
	msg[15]=0;
	msg[16]=0;
	if (s1!=-1) 
		Network->xSend(s1, msg, 17, 0);
	
	msg[0]=0x6F;//Header Byte
	msg[1]=0x00;//Size
	msg[2]=0x11;//Size
	msg[3]=0x01;//State byte
	msg[4]=items[cont2].ser1;
	msg[5]=items[cont2].ser2;
	msg[6]=items[cont2].ser3;
	msg[7]=items[cont2].ser4;
	msg[8]=0;
	msg[9]=0;
	msg[10]=0;
	msg[11]=0;
	msg[12]=0;
	msg[13]=0;
	msg[14]=0;
	msg[15]=0;
	msg[16]=0;
	if (s2!=-1) 
		Network->xSend(s2, msg, 17, 0);
	
	serial=items[cont1].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if (i!=-1)
			if ((items[i].contserial==serial))
			{
				if( items[i].glow > 0 ) 
					removefromptr( &glowsp[chars[currchar[s2]].serial%HASHMAX], i ); // LB, glowing stuff
				unsetserial( i, 1 );
				setserial(i,bp1,1);
				if( items[i].glow > 0 ) 
					setptr( &glowsp[chars[currchar[s1]].serial%HASHMAX], i );
				items[i].x=50+(rand()%80);
				items[i].y=50+(rand()%80);
				items[i].z=9;
				if (s1!=-1) 
					RefreshItem( i ); // AntiChrist
			}
	}
	serial=items[cont2].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if (i!=-1)
			if ((items[i].contserial==serial))
			{
				if( items[i].glow > 0 ) 
					removefromptr( &glowsp[chars[currchar[s2]].serial%HASHMAX], i ); // LB, glowing stuff
				unsetserial( i, 1 );
				setserial(i,bp2,1);  
				if( items[i].glow > 0 ) 
					setptr( &glowsp[chars[currchar[s1]].serial%HASHMAX], i );
				items[i].x=50+(rand()%80);
				items[i].y=50+(rand()%80);
				items[i].z=9;
				if (s2!=-1) 
					RefreshItem( i ); // AntiChrist
			}
	}
	Items->DeleItem(cont1);
	Items->DeleItem(cont2);
	Weight->NewCalc( p1 );
	Weight->NewCalc( p2 );
}

void dotrade(int cont1, int cont2)
{
	int p1, p2, bp1, bp2, s1, s2, i;
	int serial,serhash,ci;
	
	p1 = calcCharFromSer( items[cont1].contserial );
	if( p1 < 0 ) 
		return;
	p2 = calcCharFromSer( items[cont2].contserial );
	if( p2 < 0 ) 
		return;
	bp1=packitem(p1);
	if( bp1 < 0 ) 
		return;
	bp2=packitem(p2);
	if( bp2 < 0 ) 
		return;
	s1=calcSocketFromChar(p1);
	s2=calcSocketFromChar(p2);
	
	serial=items[cont1].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if (i!=-1)
			if ((items[i].contserial==serial))
			{
				unsetserial( i, 1 );
				if( items[i].glow > 0 ) 
					removefromptr( &glowsp[chars[currchar[s2]].serial%HASHMAX], i ); // LB glowing stuff
				setserial(i,bp2,1);
				if( items[i].glow > 0 ) 
					setptr( &glowsp[chars[currchar[s1]].serial%HASHMAX], i );
				items[i].x=50+(rand()%80);
				items[i].y=50+(rand()%80);
				items[i].z=9;
				if (s1!=-1) 
					RefreshItem( i ); // AntiChrist
				if (s2!=-1) 
					RefreshItem( i ); // AntiChrist
				
			}
	}
	serial=items[cont2].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if (i!=-1)
			if ((items[i].contserial==serial))
			{
				unsetserial( i, 1 );
				if( items[i].glow > 0 ) 
					removefromptr( &glowsp[chars[currchar[s2]].serial%HASHMAX], i ); // LB glowing stuff
				setserial( i, bp1, 1 );
				if( items[i].glow > 0 ) 
					setptr( &glowsp[chars[currchar[s1]].serial%HASHMAX], i );
				items[i].x=50+(rand()%80);
				items[i].y=50+(rand()%80);
				items[i].z=9;
				if (s2!=-1)
					RefreshItem( i ); // AntiChrist
				if (s1!=-1)
					RefreshItem( i ); // AntiChrist
				
			}
	}
	Weight->NewCalc( p1 );
	Weight->NewCalc( p2 );
}

//NEW RESPAWNREGIONS ZIPPY CODE STARTS HERE -- AntiChrist merging codes -- (26/4/99)

void loadspawnregions()//Regionspawns
{
	int i=0;
	
	spawnregion[i].totalnpclists=0;
	spawnregion[i].max=0;
	//spawnregion[i].current=0;
	spawnregion[i].nexttime=0;
	spawnregion[i].call = 1;
	
	i++;
	
	totalspawnregions=0;
	
	openscript("spawn.scp");
	do
	{
		read2();
		if((!(strcmp(script1, "SECTION")))&&(script2[0]=='R'))
		{
			spawnregion[i].totalnpclists=0;
			spawnregion[i].max=0;
			spawnregion[i].x1=0;
			spawnregion[i].x2=0;
			spawnregion[i].y1=0;
			spawnregion[i].y2=0;
			spawnregion[i].totalnpcs=0;
			spawnregion[i].totalitemlists=0;
			spawnregion[i].call = 1;
			
			do
			{
				read2();
				if(script1[0]!='}')
				{
					if(!(strcmp("NLIST", script1)))
					{
						spawnregion[i].npclists[spawnregion[i].totalnpclists]=str2num(script2);
						spawnregion[i].totalnpclists++;
					}
					else if(!(strcmp("NPC",script1)))
					{
						spawnregion[i].npcs[spawnregion[i].totalnpcs]=str2num(script2);
						spawnregion[i].totalnpcs++;
					}
					else if(!(strcmp("ILIST",script1)))
					{
						spawnregion[i].itemlists[spawnregion[i].totalitemlists]=str2num(script2);
						spawnregion[i].totalitemlists++;
					}
					else if(!(strcmp("MAX", script1))) spawnregion[i].max=str2num(script2);
					else if(!(strcmp("X1", script1))) spawnregion[i].x1=str2num(script2);
					else if(!(strcmp("X2", script1))) spawnregion[i].x2=str2num(script2);
					else if(!(strcmp("Y1", script1))) spawnregion[i].y1=str2num(script2);
					else if(!(strcmp("Y2", script1))) spawnregion[i].y2=str2num(script2);
					else if(!(strcmp("MINTIME", script1))) spawnregion[i].mintime=str2num(script2);
					else if(!(strcmp("MAXTIME", script1))) spawnregion[i].maxtime=str2num(script2);
					else if(!(strcmp("NAME", script1))) strcpy( spawnregion[i].name, script2 );
					else if(!(strcmp("CALL", script1))) spawnregion[i].call = str2num( script2 );
				}
			}while(script1[0]!='}');
			//spawnregion[i].nexttime=uiCurrentTime+(60*CLOCKS_PER_SEC*((rand()%(spawnregion[i].maxtime-spawnregion[i].mintime+1))+spawnregion[i].mintime));
			i++;
		}//if Section
	}while(strcmp(script1, "EOF"));
	closescript();
	totalspawnregions=i;
	printf("UOX3: %i spawn regions loaded from script.\n",i-1);
}

void loadpredefspawnregion(int r, char *name)//Load predefined spawnregion //Regionspawns
{
	char sect[512];
	sprintf(sect, "PREDEFINED_SPAWN %s", name);
	openscript("spawn.scp");
	if(! i_scripts[spawn_script]->find(sect))
	{
		printf("WARNING: Undefined region spawn %s, check your regions.scp and spawn.scp files.\n",name);
		return;
	}
	spawnregion[r].totalnpclists=0;
	spawnregion[r].call = 1;
	do {
		read2();
		if(!(strcmp(script1, "NLIST")))
		{
			spawnregion[r].npclists[spawnregion[r].totalnpclists]=str2num(script2);
			spawnregion[r].totalnpclists++;
		}
		if(!(strcmp("NPC",script1)))
		{
			spawnregion[r].npcs[spawnregion[r].totalnpcs]=str2num(script2);
			spawnregion[r].totalnpcs++;
		}
		if(!(strcmp("ILIST",script1)))
		{
			spawnregion[r].itemlists[spawnregion[r].totalitemlists]=str2num(script2);
			spawnregion[r].totalitemlists++;
		}
		if(!(strcmp("MINTIME", script1))) spawnregion[r].mintime=str2num(script2);
		if(!(strcmp("MAXTIME", script1))) spawnregion[r].maxtime=str2num(script2);
		if(!(strcmp("MAX", script1))) spawnregion[r].max=str2num(script2);
		if(!(strcmp("CALL", script1))) spawnregion[r].call = str2num( script2 );
	} while(script1[0]!='}');
	//spawnregion[r].nexttime=uiCurrentTime+(60*CLOCKS_PER_SEC*((rand()%(spawnregion[r].maxtime-spawnregion[r].mintime+1))+spawnregion[r].mintime));
	closescript();
	printf("UOX3: %s loaded into spawn region #%i.\n",sect,r);
}
//NEW RESPAWNREGIONS ZIPPY CODE ENDS HERE -- AntiChrist merging codes

void loadregions()//New -- Zippy spawn regions
{
	int i, noregion, l=0, a=0;
	char sect[512];
	int actgood = -1; // Magius(CHE)
	for (i=0;i<256;i++)
	{
		region[i].midilist=0;
		region[i].priv=0;
		region[i].snowchance=0;		// moving this to weather
		region[i].rainchance=0;		// moving this to weather
		region[i].name[0]=0;
		region[i].weather = 255;
		//region[i].name[0]=0;
		noregion=0;
		for (a=0;a<10;a++)
		{
			region[i].guardnum[a]=RandomNum(1000,1001);
		}
		a=0;
		for( a = 0; a < 100; a++ )
		{
			region[i].goodsell[a] = 0;
			region[i].goodbuy[a] = 0;
			region[i].goodrnd1[a] = 0;
			region[i].goodrnd2[a] = 0;
		}
		a = 0;
		// end added by Magius(CHE)
		openscript("regions.scp");
		sprintf(sect, "REGION %i", i);
		if (!i_scripts[regions_script]->find(sect))
		{
			noregion=1;
		}
		do
		{
			read2();
			if (script1[0]!='}')
			{
				if (!(strcmp("GUARDNUM",script1)))
				{
					if (a<10)
					{
						region[i].guardnum[a]=str2num(script2);
						//printf("DEBUG: Guardnum %i for Region %i\n",region[i].guardnum[a],i);
						a++;
					}
					else
					{
						printf("ERROR: Region %i has more than 10 'GUARDNUM', The ones after 10 will not be used\n",i);
					}
				}
				if (!(strcmp("NAME",script1)))
				{
					strcpy(region[i].name, script2);
					actgood = -1; // Magius(CHE)
				}
				// Dupois - Added April 5, 1999
				// To identify whether this region is escortable or not
				else if (!(strcmp("ESCORTS",script1)))
				{
					// Load the region number in the global array of valid escortable regions
					if( str2num( script2 ) == 1 )
					{	// Store the region index into the valid escort region array
						validEscortRegion[escortRegions] = i;
						escortRegions++;
					}
				} // End - Dupois
				else if (!(strcmp("GUARDOWNER",script1)))
				{
					strcpy(region[i].guardowner, script2);
				}
				else if (!(strcmp("MIDILIST",script1)))
				{
					region[i].midilist=str2num(script2);
				}
				else if (!(strcmp("GUARDED",script1)))
				{
					if( str2num(script2) ) 
						region[i].priv |= 0x01;   
				}
				else if (!(strcmp("MAGICDAMAGE",script1)))
				{
					if( str2num(script2) ) 
						region[i].priv |= 0x40;
				}
				else if (!(strcmp("MARK",script1)))
				{
					if (str2num(script2)) 
						region[i].priv |= 0x02;
				}
				else if (!(strcmp("GATE",script1)))
				{
					if (str2num(script2)) 
						region[i].priv |= 0x04;
				}
				else if (!(strcmp("RECALL",script1)))
				{
					if (str2num(script2)) 
						region[i].priv |= 0x08;
				}
				else if (!(strcmp("SNOWCHANCE", script1)))
				{
					region[i].snowchance = (char)str2num(script2);
				}
				else if (!(strcmp("RAINCHANCE", script1)))
				{
					region[i].rainchance = (char)str2num(script2);
				}
				else if(!(strcmp( "GOOD", script1 ) ) ) // Magius(CHE)
				{
					actgood = str2num( script2 );
				}
				else if(!(strcmp( "BUYABLE", script1 ) ) ) // Magius(CHE)
				{
					if( actgood >-1 )
						region[i].goodbuy[actgood] = str2num( script2 );
					else
						printf("Uox3.cpp: ERROR in regions.scp. You must write BUYABLE after GOOD <num>!\n" );
				}
				else if(!(strcmp( "SELLABLE", script1 ) ) ) // Magius(CHE)
				{
					if( actgood > -1 )
						region[i].goodsell[actgood] = str2num( script2 );
					else
						printf("Uox3.cpp: ERROR in regions.scp. You must write SELLABLE after GOOD <num>!\n" );
				}
				else if(!(strcmp( "RANDOMVALUE", script1 ) ) ) // Magius(CHE) (2)
				{
					if( actgood > -1 )
					{
						gettokennum( script2, 0 );
						region[i].goodrnd1[actgood] = str2num( gettokenstr );
						gettokennum( script2, 1 );
						region[i].goodrnd2[actgood] = str2num( gettokenstr );
						if( region[i].goodrnd2[actgood] < region[i].goodrnd1[actgood] )
						{
							printf( "Uox3.cpp: ERROR in regions.scp. You must write RANDOMVALUE NUM2[%i] greater than NUM1[%i].\n", region[i].goodrnd2[actgood], region[i].goodrnd1[actgood] );
							region[i].goodrnd2[actgood] = region[i].goodrnd1[actgood] = 0;
						}
					}
					else
						printf( "Uox3.cpp: ERROR in regions.scp. You must write RANDOMVALUE after GOOD <num>!\n" );
				}
				else if (!(strcmp("X1", script1)))
				{
					location[l].x1=str2num(script2);
				}
				else if (!(strcmp("X2", script1)))
				{
					location[l].x2=str2num(script2);
				}
				else if (!(strcmp("Y1", script1)))
				{
					location[l].y1=str2num(script2);
				}
				else if (!(strcmp("Y2", script1)))
				{
					location[l].y2=str2num(script2);
					location[l].region = (char)i;
					l++;
				}
				else if (!(strcmp("SPAWN", script1)))
				{
					long int pos;
					spawnregion[totalspawnregions].x1=location[l-1].x1;
					spawnregion[totalspawnregions].y1=location[l-1].y1;
					spawnregion[totalspawnregions].x2=location[l-1].x2;
					spawnregion[totalspawnregions].y2=location[l-1].y2;
					pos=ftell(scpfile);
					closescript();
					loadpredefspawnregion(totalspawnregions, script2);
					openscript("regions.scp");
					fseek(scpfile, pos, SEEK_SET);
					strcpy(script1, "SOMECRAZYREGION"); // To prevent accidental exit of loop.
					totalspawnregions++;
				}
				else if (!(strcmp("ABWEATH", script1)))
				{
					region[i].weather = str2num( script2 );
				}
			}
		}
		while (script1[0]!='}' && !noregion);
		closescript();
	}
	locationcount=l;
	logoutcount=0;
	//Instalog
	openscript("regions.scp");
	strcpy(sect, "INSTALOG");
	if (!i_scripts[regions_script]->find(sect)) 
	{
		closescript();
		return;
	}
	do {
		read2();
		if(!(strcmp(script1,"X1"))) logout[logoutcount].x1=str2num(script2);
		if(!(strcmp(script1,"Y1"))) logout[logoutcount].y1=str2num(script2);
		if(!(strcmp(script1,"X2"))) logout[logoutcount].x2=str2num(script2);
		if(!(strcmp(script1,"Y2")))
		{
			logout[logoutcount].y2=str2num(script2);
			logoutcount++;
		}
	} while (script1[0]!='}');
	closescript();//AntiChrist
}

#ifdef __LINUX__
short calcRegionFromXY( int x, int y )
#else
char calcRegionFromXY(int x, int y)
#endif
{
	int i;
	for (i=0;i<locationcount;i++)
	{
		if (location[i].x1<=x && location[i].y1<=y && location[i].x2>=x &&
			location[i].y2>=y)
		{
			return location[i].region;
		}
	}
	return -1;
}

void checkregion(int i)
{
	int calcreg, s, j;
	
	calcreg=calcRegionFromXY(chars[i].x, chars[i].y);
	if (calcreg!=chars[i].region)
	{
		s=calcSocketFromChar(i);
		if (s!=-1)
		{
			// Drake 08-30-99 If region name are the same, do not display message
			//                for playing music when approaching Tavern
			j=strcmp( region[chars[i].region].name, region[calcreg].name );
			if( j )
			{
				if (region[chars[i].region].name[0]!=0)
				{
					sprintf(temp, "You have left %s.", region[chars[i].region].name);
					sysmessage(s, temp);
				}
				if (region[calcreg].name[0]!=0)
				{
					sprintf(temp, "You have entered %s.", region[calcreg].name);
					sysmessage(s, temp);
				}
			}
			j=strcmp(region[calcreg].guardowner, region[chars[i].region].guardowner);
			if ( (region[calcreg].priv&0x01)!=(region[chars[i].region].priv&0x01) ||
				(region[calcreg].priv&0x01 && j))
			{
				if (region[calcreg].priv&0x01)
				{
					if (region[calcreg].guardowner[0]==0)
					{
						sysmessage(s, "You are now under the protection of the guards.");
					}
					else
					{
						sprintf(temp, "You are now under the protection of %s guards.", region[calcreg].guardowner);
						sysmessage(s, temp);
					}
				} 
				else
				{
					if (region[chars[i].region].guardowner[0]==0)
					{
						sysmessage(s, "You are no longer under the protection of the guards.");
					}
					else
					{
						sprintf(temp, "You are no longer under the protection of %s guards.", region[chars[i].region].guardowner);
						sysmessage(s, temp);
					}
				}
			}
		}
		chars[i].region = (unsigned char)calcreg;
		if( s != -1 )
		{
			dosocketmidi( s );
			dolight( s, worldcurlevel );
			Weather->doPlayerStuff( i );
		}
	}
}

void dosocketmidi(int s)
{
	int i=0;
	char midiarray[50];
	char sect[512];
	
	openscript("regions.scp");
	if (chars[currchar[s]].war)
	{
		strcpy(sect, "MIDILIST COMBAT");
	}
	else
	{
		sprintf(sect, "MIDILIST %i", region[chars[currchar[s]].region].midilist);
	}
	if (region[chars[currchar[s]].region].midilist!=0 && !i_scripts[regions_script]->find(sect))
	{
		closescript();
		return;
	}
	do
	{
		read2();
		if (script1[0]!='}')
		{
			if (!(strcmp("MIDI",script1)))
			{
				midiarray[i]=str2num(script2);
				i++;
			}
		}
	}
	while (script1[0]!='}');
	closescript();
	if (i!=0)
	{
		i=rand()%(i);
		playmidi(s, 0, midiarray[i]);
	}
}


void respawnnow( void )
{
	int i, j, k,serial,serhash,ci;
	
	for(i=1;i<totalspawnregions;i++) //New -- Zippy region spawner
		doregionspawn(i);
	
	for(i=0;i<itemcount;i++)  // Item Spawner
	{
		if( items[i].type == 61 )
		{
			k = 0;
			serial = items[i].serial;
			serhash = serial%HASHMAX;
			for( ci = 0; ci < spawnsp[serhash].max; ci++ )
			{
				j = spawnsp[serhash].pointer[ci];
				if( j != -1 )
					if( i != j && items[j].x == items[i].x && items[j].y == items[i].y && items[j].z == items[i].z )
					{
						if( items[i].serial == items[j].spawnserial )
						{
							k = 1;
							break;
						}
					}
			}
			if( k == 0 )
			{
				Items->AddRespawnItem(i, items[i].morex,0);
			}
		}
		
		if( items[i].type == 62 || items[i].type == 69 || items[i].type == 125 )  // NPC Spawner
		{
			k = 0;
			serial = items[i].serial;
			serhash = serial%HASHMAX;
			for( ci = 0; ci < cspawnsp[serhash].max; ci++ )
			{
				j = cspawnsp[serhash].pointer[ci];
				if( j != -1 )
					if( items[i].serial == chars[j].spawnserial )
					{
						k++;
					}
			} 
			
			if( k < items[i].amount )  /** lord binary, adds spawn amount checking **/
			{
				Npcs->AddRespawnNPC(i, -1, items[i].morex,1);
				items[i].gatetime=0;
			}
		} /* end if npc spawner */
	} /* end of item-search-loop */
}



void loadskills()
{
	int i, noskill, l=0;
	char sect[512];
	
	for (i=0;i<SKILLS;i++) // lb
	{
		skill[i].st=0;
		skill[i].dx=0;
		skill[i].in=0;
		skill[i].advance_index=l;
		noskill=0;
		openscript("skills.scp");
		sprintf(sect, "SKILL %i", i);
		if (!i_scripts[skills_script]->find(sect))
		{
			noskill=1;
		}
		do
		{
			read2();
			if (script1[0]!='}')
			{
				if (!(strcmp("STR", script1)))
				{
					skill[i].st=str2num(script2);
				}
				if (!(strcmp("DEX", script1)))
				{
					skill[i].dx=str2num(script2);
				}
				if (!(strcmp("INT", script1)))
				{
					skill[i].in=str2num(script2);
				}
				if (!(strcmp("SKILLPOINT", script1)))
				{
					statadvance[l].skill=i;
					gettokennum(script2, 0);
					statadvance[l].base=str2num(gettokenstr);
					gettokennum(script2, 1);
					statadvance[l].success=str2num(gettokenstr);
					gettokennum(script2, 2);
					statadvance[l].failure=str2num(gettokenstr);
					l++;
				}
			}
		}
		while (script1[0]!='}' && !noskill);
		closescript();
	}
}

void tellmessage(int i, int s, unsigned char *txt)
{
	int tl;
	
	sprintf(temp, "GM tells %s: %s", chars[currchar[s]].name, (char *)txt);
	
	tl = 44 + strlen(temp) + 1;
	talk[1] = (unsigned char)(tl>>8);
	talk[2] = (unsigned char)(tl%256);
	talk[3] = 1;
	talk[4] = 1;
	talk[5] = 1;
	talk[6] = 1;
	talk[7] = 1;
	talk[8] = 1;
	talk[9] = 0;
	talk[10] = 0x00; //First Part  \_Yellow
	talk[11] = 0x35; //Second Part /
	talk[12] = 0;
	talk[13] = 3;
	Network->xSend(s, talk, 14, 0);
	Network->xSend(s, sysname, 30, 0);
	Network->xSend(s, temp, strlen(temp)+1, 0);
	Network->xSend(i, talk, 14, 0);//So Person who said it can see too
	Network->xSend(i, sysname, 30, 0);
	Network->xSend(i, temp, strlen(temp)+1, 0);
}


#ifdef __NT__
void Writeslot(LPSTR lpszMessage)
{
	BOOL fResult;
	HANDLE hFile;
	DWORD cbWritten;
	
	hFile = CreateFile("\\\\*\\mailslot\\uoxmail", GENERIC_WRITE,FILE_SHARE_READ,
		(LPSECURITY_ATTRIBUTES) NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
		(HANDLE) NULL);
	fResult = WriteFile(hFile,lpszMessage,(DWORD) lstrlen(lpszMessage) + 1,
		&cbWritten,(LPOVERLAPPED) NULL);
	fResult = CloseHandle(hFile);
	if(!atoi(lpszMessage)) printf("UOX3: %s\n",lpszMessage);
}
#else
void Writeslot(char * lpszMessage)
{
	FILE *f;
	
	f=fopen("UOX3.log","a");
	if(f==NULL) return;
	fprintf(f,"%s\n",lpszMessage);
	if(!atoi(lpszMessage)) printf("UOX3: %s\n",lpszMessage);
	fclose(f);
	return;
}
#endif


void saveserverscript(char x)
// PARAM WARNING: x is never referenced
{
	FILE *file;
	file=fopen("server.scp", "w");
	if (!file)
	{
		printf("Error, could not open server.scp for writing. Check file permissions.\n");
		return;
	}
	
	fprintf( file, "// UOX3 Server Script\n");
	fprintf( file, "// Generated by UOX3 Version %s\n\n",VER);
	
	fprintf( file, "SECTION SERVER\n");
	fprintf( file, "{\n");
	fprintf( file, "CRASH_PROTECTION %i\n", server_data.crashprotect);
	
	fprintf( file, "SKILLCAP %i\n", server_data.skillcap );
	fprintf( file, "STATCAP %i\n", server_data.statcap );
	
	// AntiChrist - specialbank
	
	fprintf( file, "USESPECIALBANK %i\n", server_data.usespecialbank );
	fprintf( file, "SPECIALBANKTRIGGER %s\n", server_data.specialbanktrigger );
	fprintf( file, "DECAYTIMER %i\n",server_data.decaytimer);
	fprintf( file, "PLAYERCORPSEDECAYMULTIPLIER %i\n", server_data.playercorpsedecaymultiplier);
	fprintf( file, "LOOTDECAYSWITHCORPSE %i\n", server_data.lootdecayswithcorpse);
	fprintf( file, "INVISTIMER %i\n",server_data.invisibiliytimer);
	fprintf( file, "SKILLDELAY %i\n",server_data.skilldelay);
	fprintf( file, "SKILLLEVEL %i\n", server_data.skilllevel ); // By Magius(CHE)
	fprintf( file, "OBJECTDELAY %i\n",server_data.objectdelay);
	fprintf( file, "GATETIMER %i\n",server_data.gatetimer);
	fprintf( file, "SHOWDEATHANIM %i\n",server_data.showdeathanim);
	fprintf( file, "GUARDSACTIVE %i\n",server_data.guardsactive);
	fprintf( file, "BG_SOUNDS %i\n",server_data.bg_sounds);
	//   EviLDeD  -  Output current serverstatus regarding world saves
	//   December 27, 1998
	fprintf( file, "ANNOUNCE_WORLDSAVES %i\n",server_data.announceworldsaves);
	//	February 10, 2000
	fprintf( file, "WWWACCOUNTS %s\n",server_data.wwwaccounts==true?"1":"0");
	//   EviLDeD  -  End
	fprintf( file, "ARCHIVEPATH %s\n",server_data.archivepath);
	fprintf( file, "SAVE_PER_LOOP %li\n", cwmWorldState->LoopSaveAmt() );
	fprintf( file, "BACKUP_SAVE_RATIO %i\n", server_data.backup_save_ratio ); // LB
	fprintf( file, "UOXBOT %i\n",server_data.UOXBot);
	fprintf( file, "POTIONDELAY %i\n", server_data.potiondelay );
	fprintf( file, "POISONTIMER %i\n",server_data.poisontimer); // lb
	fprintf( file, "JOINMSG %i\n",server_data.joinmsg);
	fprintf( file, "PARTMSG %i\n",server_data.partmsg);
	fprintf( file, "LOG %i\n",server_data.log);
	fprintf( file, "ROGUE %i\n",server_data.rogue);
	fprintf( file, "WEATHERTIME %i\n", server_data.weathertime);
	fprintf( file, "SHOPTIME %i\n", server_data.shopSpawnTime);
	fprintf( file, "SHOWLOGGEDOUTPCS %i\n", server_data.showloggedoutpcs ); // Melen - 9/7/99
	fprintf( file, "CHAR_TIME_OUT %i\n",server_data.quittime);//Instalog
	fprintf( file, "MAXSTEALTHSTEPS %i\n",server_data.maxstealthsteps);//Instalog
	fprintf( file, "RUNNINGSTAMINASTEPS %i\n",server_data.runningstaminasteps);//Instalog
    fprintf( file, "BOAT_SPEED %f\n",server_data.boatspeed);//Boats
    fprintf( file, "HTML %i\n",server_data.html);
	fprintf( file, "CUT_SCROLL_REQUIREMENTS %i\n", server_data.cutscrollreq ); // AntiChrist
	fprintf( file, "PERSECUTION %i\n", server_data.persecute ); // AntiChrist
	fprintf( file, "AUTO_CREATE_ACCTS %i\n", server_data.auto_a_create);
	fprintf( file, "MSGBOARDPATH %s\n",server_data.msgboardpath);              // Dupois - Added Dec 20, 1999 for msgboard.cpp
	fprintf( file, "MSGPOSTACCESS %i\n",server_data.msgpostaccess);            // Dupois - Added Dec 20, 1999 for msgboard.cpp
	fprintf( file, "MSGPOSTREMOVE %i\n",server_data.msgpostremove);            // Dupois - Added Dec 20, 1999 for msgboard.cpp
	fprintf( file, "MSGRETENTION %i\n",server_data.msgretention);              // Dupois - Added Dec 20, 1999 for msgboard.cpp
	fprintf( file, "ESCORTACTIVE %i\n",server_data.escortactive);              // Dupois - Added Dec 20, 1999 for escorts
	fprintf( file, "ESCORTINITEXPIRE %i\n",server_data.escortinitexpire);      // Dupois - Added Dec 20, 1999 for escorts
	fprintf( file, "ESCORTACTIVEEXPIRE %i\n",server_data.escortactiveexpire);  // Dupois - Added Dec 20, 1999 for escorts
	fprintf( file, "ESCORTDONEEXPIRE %i\n",server_data.escortdoneexpire);      // Dupois - Added Dec 20, 1999 for escorts
	if( server_data.footSteps )	// Abaddon 17th February, 2000 Allows us to turn off footsteps sounds
		fprintf( file, "FOOTSTEPS 1\n" );
	else
		fprintf( file, "FOOTSTEPS 0\n" );
	fprintf( file, "COMMANDPREFIX %c\n", server_data.commandPrefix ); 
	fprintf( file, "STATADVANCE %i\n", server_data.stat_advance ); // Gunther stat advance tweak	
	fprintf( file, "PORT %i\n", UOX_PORT );

	fprintf( file,"}\n\n");
	
	fprintf( file, "SECTION SPEED\n"); //Lag Fix -- Zippy
	fprintf( file, "{\n");
    fprintf( file, "NICE %i\n",speed.nice);
	fprintf( file, "LORDB_LAG_FIX %i\n",server_data.lordblagfix);
	fprintf( file, "CHECK_ITEMS %f\n",speed.itemtime);
	fprintf( file, "CHECK_NPCS %f\n",speed.npctime);
	fprintf( file, "NPC_TILE_CHECK %i\n", speed.tilecheck );
	fprintf( file, "CHECK_NPCAI %f\n",speed.npcaitime);
	fprintf( file, "CHECK_SPAWNREGIONS %i\n",speed.srtime);
	fprintf( file, "CHECKMEM %i\n", speed.checkmem);
	fprintf( file, "CACHE_MULS %i\n", Map->Cache );
	fprintf( file, "ACCOUNTFLUSH %i\n", speed.accountFlush );
	fprintf( file, "}\n\n");
	
	fprintf( file, "SECTION COMBAT\n" );
	fprintf( file, "{\n" );
	fprintf( file, "MAXRANGE %i\n", combat.maxRange );
	fprintf( file, "WRESTLESPEED %i\n", combat.wrestleSpeed );
	fprintf( file, "MAX_RANGE_SPELL %i\n", combat.maxRangeSpell );
	fprintf( file, "MAXDMG %i\n", combat.maxDmg );
	fprintf( file, "DEATH_ON_THROAT %i\n", combat.deathOnThroat );
	fprintf( file, "DIST_TO_POISON %i\n", combat.dToPoison );
	fprintf( file, "COMBAT_HIT_MESSAGE %i\n",server_data.combathitmessage);
	fprintf( file, "MAX_ABSORBTION %i\n", server_data.maxabsorbtion );
	fprintf( file, "MAX_NON_HUMAN_ABSORBTION %i\n", server_data.maxnohabsorbtion );
	fprintf( file, "MONSTERS_VS_ANIMALS %i\n",server_data.monsters_vs_animals); 
	fprintf( file, "ANIMALS_ATTACK_CHANCE %i\n",server_data.animals_attack_chance); 
	fprintf( file, "ANIMALS_GUARDED %i\n",server_data.animals_guarded); 
	fprintf( file, "NPC_DAMAGE_RATE %i\n", server_data.npcdamage );
	fprintf( file, "NPC_BASE_FLEEAT %i\n",server_data.npc_base_fleeat);
	fprintf( file, "NPC_BASE_REATTACKAT %i\n",server_data.npc_base_reattackat);
	fprintf( file, "ATTACKSTAMINA %i\n", server_data.attackstamina ); // antichrist (6) - for ATTACKSTAMINA
	fprintf( file, "EXPLODEDELAY %f\n", combat.explodeDelay );
	fprintf( file, "}\n\n" );
	
	fprintf( file, "SECTION VENDOR\n" );
	fprintf( file, "{\n" );
	fprintf( file, "SELLBYNAME %i\n", server_data.sellbyname ); // Magius(CHE)
	fprintf( file, "SELLMAXITEM %i\n", server_data.sellmaxitem ); // Magius(CHE)
	fprintf( file, "TRADESYSTEM %i\n", server_data.trade_system ); // Magius(CHE)
	fprintf( file, "RANKSYSTEM %i\n", server_data.rank_system ); // Magius(CHE)
	fprintf( file, "BUYTHRESHOLD %i\n", server_data.buyThreshold );
	fprintf( file, "}\n\n" );
	
	fprintf( file, "SECTION REGENERATE\n" );
	fprintf( file, "{\n" );
	fprintf( file, "HITPOINTS_REGENRATE %i\n",server_data.hitpointrate);
	fprintf( file, "STAMINA_REGENRATE %i\n",server_data.staminarate);
	fprintf( file, "MANA_REGENRATE %i\n",server_data.manarate);
	fprintf( file, "ARMOR_AFFECT_MANA_REGEN %i\n",server_data.armoraffectmana);
	fprintf( file, "}\n\n" );
	
	fprintf( file, "SECTION HUNGER\n" );
	fprintf( file, "{\n" );
	fprintf( file, "HUNGERRATE %i\n",server_data.hungerrate);
	fprintf( file, "HUNGER_DAMAGE %i\n",server_data.hungerdamage);
	fprintf( file, "HUNGER_DAMAGE_RATE %i\n",server_data.hungerdamagerate);
	fprintf( file, "HUNGERTHRESHOLD %i\n", server_data.hungerThreshold );
	fprintf( file, "}\n\n" );
	
	fprintf( file, "SECTION RESOURCE\n" );
	fprintf( file, "{\n" );
	fprintf( file, "MINECHECK %i\n",server_data.minecheck );
	fprintf( file, "ORE_PER_AREA %i\n",resource.ore );
	fprintf( file, "ORE_RESPAWN_TIME %i\n",resource.oretime );
	fprintf( file, "ORE_RESPAWN_AREA %i\n",resource.orearea );
	fprintf( file, "LOGS_PER_AREA %i\n",resource.logs );
	fprintf( file, "LOG_RESPAWN_TIME %i\n",resource.logtime );
	fprintf( file, "LOG_RESPAWN_AREA %i\n",resource.logarea );
	fprintf( file, "}\n\n" );
	
	fprintf( file, "SECTION REPSYS\n");
	fprintf( file, "{\n");
	fprintf( file, "MURDER_DECAY %i\n", repsys.murderdecay);
	fprintf( file, "MAXKILLS %i\n", repsys.maxkills);
	fprintf( file, "CRIMINAL_TIME %i\n", repsys.crimtime);
	fprintf( file, "SNOOP_IS_CRIME %i\n", server_data.snoopiscrime ? 1 : 0 );
	fprintf( file, "}\n\n");
	
	fprintf( file, "SECTION TRACKING\n");
	fprintf( file, "{\n");
	fprintf( file, "BASE_TRACKING_RANGE %i\n",tracking_data.baserange);
	fprintf( file, "MAX_TRACKING_TARGETS %i\n",tracking_data.maxtargets);
	fprintf( file, "BASE_TRACKING_TIME %i\n",tracking_data.basetimer);
	fprintf( file, "TRACKING_MESSAGE_REDISPLAY_TIME %i\n",tracking_data.redisplaytime);
	fprintf( file, "}\n\n");
	
	fprintf( file, "SECTION BEGGING\n");
	fprintf( file, "{\n");
	fprintf( file, "BEGGING_RANGE %i\n",begging_data.range);
	fprintf( file, "BEGGING_TEXT0 %s\n",begging_data.text[0]);
	fprintf( file, "BEGGING_TEXT1 %s\n",begging_data.text[1]);
	fprintf( file, "BEGGING_TEXT2 %s\n",begging_data.text[2]);
	fprintf( file, "}\n\n");
	
	fprintf( file, "SECTION FISHING\n");
	fprintf( file, "{\n");
	fprintf( file, "BASE_FISHING_TIME %i\n",fishing_data.basetime);
	fprintf( file, "RANDOM_FISHING_TIME %i\n",fishing_data.randomtime);
	fprintf( file, "}\n\n");
	
	fprintf( file, "SECTION SPIRITSPEAK\n");
	fprintf( file, "{\n");
	fprintf( file, "SPIRITSPEAKTIMER %i\n",spiritspeak_data.spiritspeaktimer);
	fprintf( file, "}\n\n");
	
	fprintf( file, "SECTION TIME_LIGHT\n");
	fprintf( file, "{\n");
	fprintf( file, "DAY %i\n", day);
	fprintf( file, "HOUR %i\n", hour);
	fprintf( file, "MINUTE %i\n", minute);
	fprintf( file, "AMPM %i\n", ampm);
	fprintf( file, "MOON1UPDATE %i\n", moon1update);
	fprintf( file, "MOON2UPDATE %i\n", moon2update);
	fprintf( file, "MOON1 %i\n", moon1);
	fprintf( file, "MOON2 %i\n", moon2);
	fprintf( file, "DUNGEONLIGHTLEVEL %i\n", dungeonlightlevel);
	fprintf( file, "WORLDFIXEDLEVEL %i\n", worldfixedlevel);
	fprintf( file, "WORLDCURLEVEL %i\n", worldcurlevel);
	fprintf( file, "WORLDBRIGHTLEVEL %i\n", worldbrightlevel);
	fprintf( file, "WORLDDARKLEVEL %i\n", worlddarklevel);
	fprintf( file, "SECONDSPERUOMINUTE %i\n", secondsperuominute); // lb
	fprintf( file, "}\n\n");
	
	fprintf(file,"EOF\n\n");
	fclose(file);
	file = NULL;
	//if (x) printf("UOX3: Server data saved.(Manual)\n");
	//else printf("UOX3: Server data saved.(AUTO)\n");
}

void loadspeed()//Lag Fix -- Zippy -- NEW FUNCTION
{
	do
	{
		readw2();
		
        if(!(strcmp(script1,"NICE"))) speed.nice=str2num(script2);
		else if(!(strcmp(script1,"CHECK_ITEMS"))) speed.itemtime=atof(script2);
		else if(!(strcmp(script1,"CHECK_SPAWNREGIONS"))) speed.srtime=str2num(script2);
		else if(!(strcmp(script1,"CHECK_NPCS"))) speed.npctime=atof(script2);
        else if(!(strcmp(script1,"CHECK_NPCAI"))) speed.npcaitime=atof(script2);
		else if(!(strcmp(script1,"LORDB_LAG_FIX"))) server_data.lordblagfix=str2num(script2);
		else if(!(strcmp(script1,"CHECKMEM"))) speed.checkmem=str2num(script2);
		else if(!(strcmp(script1,"CACHE_MULS")) && Map != NULL) Map->Cache = str2num( script2 );
		else if(!(strcmp(script1,"NPC_TILE_CHECK"))) speed.tilecheck = str2num( script2 );
		else if(!(strcmp(script1,"ACCOUNTFLUSH" ))) speed.accountFlush = str2num( script2 );
	}
	while (strcmp(script1, "}"));
}

void loadserverscript() // Load server script
{
	wscfile=fopen("server.scp", "r");
	if(wscfile==NULL)
	{
		printf("server.scp not found...defaults are loaded\n");
		return;
	}
	do
	{
		readw2();
		if (!(strcmp(script1, "SECTION")))
		{
			if(!(strcmp(script2, "SERVER"))) loadserver();
			else if(!(strcmp(script2, "SPEED"))) loadspeed();//Lag Fix -- Zippy
			else if(!(strcmp(script2, "RESOURCE"))) loadresources();
			else if(!(strcmp(script2, "REPSYS"))) loadrepsys();
			else if(!(strcmp(script2, "TRACKING"))) loadtracking();
			else if(!(strcmp(script2, "BEGGING"))) loadbegging();
			else if(!(strcmp(script2, "FISHING"))) loadfishing();
			else if(!(strcmp(script2, "SPIRITSPEAK"))) loadspiritspeak();
			else if(!(strcmp(script2, "TIME_LIGHT"))) loadtime_light();
			else if(!(strcmp(script2, "COMBAT" ))) loadCombat();
			//	Magius(CHE)	-	(Date Unknown)
			else if(!(strcmp(script2, "HUNGER" ))) loadhunger();
			else if(!(strcmp(script2, "VENDOR" ))) loadvendor();
			else if(!(strcmp(script2, "REGENERATE" ))) loadregenerate();
			//	Magius(CHE)	- End
		}
	}
	while (strcmp(script1, "EOF"));
	fclose(wscfile);
	wscfile = NULL;
}

void loadCombat()
{
	do
	{
		readw2();
		if( !strcmp( script1, "ANIMALS_ATTACK_CHANCE" ) ) server_data.animals_attack_chance = str2num( script2 );
		else if( !strcmp( script1, "ANIMALS_GUARDED" ) ) server_data.animals_guarded = str2num( script2 );
		else if( !strcmp( script1, "ATTACKSTAMINA" ) ) server_data.attackstamina = str2num( script2 ); // antichrist (6) - for ATTACKSTAMINA
		else if( !strcmp( script1, "COMBAT_HIT_MESSAGE" ) ) server_data.combathitmessage = str2num( script2 );
		else if( !strcmp( script1, "DEATH_ON_THROAT" ) ) combat.deathOnThroat = str2num( script2 );
		else if( !strcmp( script1, "DIST_TO_POISON" ) ) combat.dToPoison = str2num( script2 );
		else if( !strcmp( script1, "EXPLODEDELAY" ) ) combat.explodeDelay = atof( script2 );
		else if( !strcmp( script1, "MAX_ABSORBTION" ) ) server_data.maxabsorbtion = str2num( script2 ); // MAgius(CHE) 
		else if( !strcmp( script1, "MAX_NON_HUMAN_ABSORBTION" ) ) server_data.maxnohabsorbtion = str2num( script2 ); // MAgius(CHE) (2)
		else if( !strcmp( script1, "MAX_RANGE_SPELL" ) ) combat.maxRangeSpell = str2num( script2 );
		else if( !strcmp( script1, "MAXDMG" ) ) combat.maxDmg = str2num( script2 );
		else if( !strcmp( script1, "MAXRANGE" ) ) combat.maxRange = str2num( script2 );
		else if( !strcmp( script1, "MONSTERS_VS_ANIMALS" ) ) server_data.monsters_vs_animals = str2num( script2 );
		else if( !strcmp( script1, "NPC_BASE_FLEEAT" ) ) server_data.npc_base_fleeat = str2num( script2 );
		else if( !strcmp( script1, "NPC_BASE_REATTACKAT" ) ) server_data.npc_base_reattackat = str2num( script2 );
		else if( !strcmp( script1, "NPC_DAMAGE_RATE" ) ) server_data.npcdamage = str2num( script2 ); // MAgius(CHE) (3)
		else if( !strcmp( script1, "WRESTLESPEED" ) ) combat.wrestleSpeed = str2num( script2 );
	}
	while( strcmp( script1, "}" ));
}

void loadregenerate( void ) // by Magius(CHE)
{
	
	do
	{
		readw2();
		if( !strcmp( script1, "HITPOINTS_REGENRATE" ) ) 
			server_data.hitpointrate = str2num(script2);
		else if( !strcmp( script1, "STAMINA_REGENRATE" ) ) 
			server_data.staminarate = str2num(script2);
		else if( !strcmp( script1, "MANA_REGENRATE" ) ) 
			server_data.manarate = str2num(script2);
		else if( !strcmp( script1, "ARMOR_AFFECT_MANA_REGEN" ) ) 
			server_data.armoraffectmana = str2num(script2);
	}
	while( strcmp( script1, "}" ) );
}

void loadhunger( void ) // by Magius(CHE)
{
	
	do
	{
		readw2();
		if( !strcmp( script1, "HUNGERRATE" ) ) 
			server_data.hungerrate = str2num( script2 );
		else if( !strcmp( script1, "HUNGER_DAMAGE" ) ) 
			server_data.hungerdamage = str2num(script2);
		else if( !strcmp( script1, "HUNGER_DAMAGE_RATE" ) ) 
			server_data.hungerdamagerate = str2num(script2);
		else if( !strcmp( script1, "HUNGERTHRESHOLD" ) ) server_data.hungerThreshold = str2num( script2 );
	}
	while (strcmp( script1, "}" ) );
}

void loadvendor( void ) // by Magius(CHE )
{
	do
	{
		readw2();
		if(!(strcmp(script1, "SELLBYNAME" )))
		{
			server_data.sellbyname = str2num( script2 );
			if( server_data.sellbyname != 0 ) server_data.sellbyname = 1; // MAgius(CHE)
		}
		else if(!(strcmp(script1, "SELLMAXITEM" )))
		{
			server_data.sellmaxitem = str2num( script2 );
			//			if( server_data.sellmaxitem != 0 ) server_data.sellmaxitem = 1; // Magiu(CHE)
			// bugfix, LB
		}
		else if(!(strcmp(script1, "TRADESYSTEM" )))
		{
			server_data.trade_system = str2num( script2 );
			if( server_data.trade_system != 0 ) server_data.trade_system = 1; // Magiu(CHE)
		}
		else if(!(strcmp(script1, "RANKSYSTEM" )))
		{
			server_data.rank_system = str2num( script2 );
			if( server_data.rank_system != 0 ) server_data.rank_system = 1; // Magius(CHE)
		}
		else if( !strcmp( script1, "BUYTHRESHOLD" ) )
		{
			server_data.buyThreshold = str2num( script2 );
		}
	}
	while( strcmp( script1, "}" ) );
}

void loadserver( void )
{
	do
	{
		readw2();
		if(!(strcmp(script1, "SKILLCAP" ))) 
		{
			if ( str2num( script2 ) < 1500 ) //assume they missed the . (1000 = 100.0)
				server_data.skillcap = str2num( script2 )*10;
			else 
				server_data.skillcap = str2num( script2 );

			if ( server_data.skillcap == 0 )
				server_data.skillcap = 7000;
		}
		else if(!(strcmp( script1, "CRASH_PROTECTION"))) server_data.crashprotect = str2num( script2 );
		else if(!(strcmp( script1, "STATCAP" ))) server_data.statcap = str2num( script2 );
		else if(!(strcmp( script1, "USESPECIALBANK" ))) server_data.usespecialbank = str2num( script2 ); // AntiChrist - Special Bank
		else if(!(strcmp( script1, "SPECIALBANKTRIGGER" ))) // AntiCrhist - Special Bank
		{
			strcpy( server_data.specialbanktrigger, script2 );
			for( int i = 0; i < strlen( server_data.specialbanktrigger ); i++ )
				server_data.specialbanktrigger[i] = toupper( server_data.specialbanktrigger[i] );
		}
		else if(!(strcmp(script1,"DECAYTIMER"))) server_data.decaytimer=str2num(script2);
		else if(!(strcmp(script1,"PLAYERCORPSEDECAYMULTIPLIER"))) server_data.playercorpsedecaymultiplier=str2num(script2);//JM
		else if(!(strcmp(script1,"LOOTDECAYSWITHCORPSE"))) server_data.lootdecayswithcorpse=str2num(script2);
		else if(!(strcmp(script1,"INVISTIMER"))) server_data.invisibiliytimer=str2num(script2);
		else if(!(strcmp(script1,"SKILLDELAY"))) server_data.skilldelay=str2num(script2);
		else if(!(strcmp(script1, "SKILLLEVEL" )))
		{
			server_data.skilllevel = str2num( script2 ); // MAgius(CHE)
			if( server_data.skilllevel < 1 || server_data.skilllevel > 10 )
				server_data.skilllevel = 5;
		}
		else if(!(strcmp(script1,"OBJECTDELAY"))) server_data.objectdelay=str2num(script2);
		else if(!(strcmp(script1,"GATETIMER"))) server_data.gatetimer=str2num(script2);
		else if(!(strcmp(script1,"SHOWDEATHANIM"))) server_data.showdeathanim=str2num(script2);
		else if(!(strcmp(script1,"GUARDSACTIVE"))) server_data.guardsactive = (unsigned char)str2num(script2);
		//  EviLDeD  -  Server information token for toggling world save announcements on or off
		//  December 27, 1998
		else if(!(strcmp(script1,"ANNOUNCE_WORLDSAVES"))) server_data.announceworldsaves = (char)str2num(script2);
		//	February 10, 2000
		else if(!(strcmp(script1,"WWWACCOUNTS"))) server_data.wwwaccounts=(strcmp(script2,"1")==0)?true:false;
		//  EviLDeD  -  End
		else if(!(strcmp(script1,"BG_SOUNDS"))) server_data.bg_sounds=str2num(script2);
		else if(!(strcmp(script1,"ARCHIVEPATH"))) strcpy(server_data.archivepath,script2);
		else if(!(strcmp(script1, "BACKUP_SAVE_RATIO" ))) 
		{ // LB
			server_data.backup_save_ratio = str2num( script2 );
			if( server_data.backup_save_ratio < 1 )
				server_data.backup_save_ratio = 1;
		}
		else if(!(strcmp(script1,"SAVE_PER_LOOP"))) cwmWorldState->SetLoopSaveAmt( str2num( script2 ) );
		else if(!(strcmp(script1,"UOXBOT"))) server_data.UOXBot=str2num(script2);
        else if(!(strcmp(script1,"POISONTIMER"))) server_data.poisontimer=str2num(script2); // lb
		else if(!(strcmp(script1,"POTIONDELAY"))) server_data.potiondelay = str2num( script2 );
		else if(!(strcmp(script1,"JOINMSG"))) server_data.joinmsg=str2num(script2);
		else if(!(strcmp(script1,"PARTMSG"))) server_data.partmsg=str2num(script2);
		else if(!(strcmp(script1,"LOG"))) server_data.log=str2num(script2);
		else if(!(strcmp(script1,"ROGUE"))) server_data.rogue=str2num(script2);
		else if(!(strcmp(script1,"CHAR_TIME_OUT"))) server_data.quittime=str2num(script2);//Instalog
		else if(!(strcmp(script1,"MAXSTEALTHSTEPS"))) server_data.maxstealthsteps=str2num(script2);//AntiChrist
		else if(!(strcmp(script1,"RUNNINGSTAMINASTEPS"))) server_data.runningstaminasteps=str2num(script2);//AntiChrist
        else if(!(strcmp(script1,"BOAT_SPEED"))) server_data.boatspeed=atof(script2);//Boats
		else if(!(strcmp(script1,"HTML"))) server_data.html=str2num(script2);//HTML
		else if(!(strcmp(script1, "CUT_SCROLL_REQUIREMENTS" ))) server_data.cutscrollreq = str2num( script2 ); // AntiChrist
		else if(!(strcmp(script1,"AUTO_ACCT"))) server_data.auto_acct=str2num(script2);	// adding auto accounts
		else if(!(strcmp(script1,"WEATHERTIME")))
		{
			server_data.weathertime=str2num(script2);
			weathertime=server_data.weathertime;
		}
		else if(!(strcmp(script1,"SHOPTIME")))
		{
			server_data.shopSpawnTime=str2num(script2);
			shopTime=server_data.shopSpawnTime;
		}
		else if( !strcmp( script1, "SHOWLOGGEDOUTPCS" ) ) 
			server_data.showloggedoutpcs = str2num( script2 );  // Melen 9/7/99
		else if( !strcmp( script1, "PERSECUTION" ) ) 
			server_data.persecute = str2num( script2 ); // AntiChrist
		else if( !strcmp( script1, "AUTO_CREATE_ACCTS" ) ) 
			server_data.auto_a_create = str2num( script2 );
		else if( !strcmp( script1, "MSGBOARDPATH" ) ) 
			strcpy( server_data.msgboardpath, script2 );               // Dupois - Added Dec 20, 1999 for message boards
		else if( !strcmp( script1, "MSGPOSTACCESS" ) ) 
			server_data.msgpostaccess = str2num( script2 );            // Dupois - Added Dec 20, 1999 for message boards
		else if( !strcmp( script1, "MSGPOSTREMOVE" ) ) 
			server_data.msgpostremove = str2num( script2 );            // Dupois - Added Dec 20, 1999 for message boards
		else if( !strcmp( script1, "MSGRETENTION" ) ) 
			server_data.msgretention = str2num( script2 );              // Dupois - Added Dec 20, 1999 for message boards
		else if( !strcmp( script1, "ESCORTACTIVE" ) ) 
			server_data.escortactive = str2num( script2 );              // Dupois - Added Dec 20, 1999 for escorts
		else if( !strcmp( script1, "ESCORTINITEXPIRE" ) ) 
			server_data.escortinitexpire = str2num( script2 );      // Dupois - Added Dec 20, 1999 for escorts
		else if( !strcmp( script1, "ESCORTACTIVEEXPIRE" ) ) 
			server_data.escortactiveexpire = str2num( script2 );  // Dupois - Added Dec 20, 1999 for escorts
		else if( !strcmp( script1, "ESCORTDONEEXPIRE" ) ) 
			server_data.escortdoneexpire = str2num( script2 );      // Dupois - Added Dec 20, 1999 for escorts
		else if( !strcmp( script1, "FOOTSTEPS" ) ) 
			server_data.footSteps = ( str2num( script2 ) != 0 );		// Abaddon - 17th February 2000, turn on/off footstep sounds
		else if( !strcmp( script1, "COMMANDPREFIX" ) ) 
			server_data.commandPrefix = script2[0]; 
		else if( !strcmp( script1, "STATADVANCE" ) ) 
			server_data.stat_advance = ( str2num( script2 )); //Gunther Stat advancement percentage tweakable
		else if( !strcmp( script1, "PORT" ) )
		{
			UOX_PORT = str2num( script2 );
		}

	}
	while (strcmp(script1, "}"));
}

void loadtracking() // Load scriptable tracking data
{
	do
	{
		readw2();
		if(!(strcmp(script1,"BASE_TRACKING_RANGE"))) tracking_data.baserange=str2num(script2);
		else if(!(strcmp(script1,"MAX_TRACKING_TARGETS")))
		{
			tracking_data.maxtargets=str2num(script2);
			if( tracking_data.maxtargets > MAXTRACKINGTARGETS ) 
				tracking_data.maxtargets = MAXTRACKINGTARGETS;
		}
		else if(!(strcmp(script1,"BASE_TRACKING_TIME"))) tracking_data.basetimer=str2num(script2);
		else if(!(strcmp(script1,"TRACKING_MESSAGE_REDISPLAY_TIME"))) tracking_data.redisplaytime=str2num(script2);
	}
	while (strcmp(script1, "}"));
}

void loadbegging()
{
	do
	{
		readw2();
		if(!(strcmp(script1,"BEGGING_RANGE"))) begging_data.range=str2num(script2);
		else if(!(strcmp(script1,"BEGGING_TEXT0"))) strcpy(begging_data.text[0],script2);
		else if(!(strcmp(script1,"BEGGING_TEXT1"))) strcpy(begging_data.text[1],script2);
		else if(!(strcmp(script1,"BEGGING_TEXT2"))) strcpy(begging_data.text[2],script2);
	}
	while (strcmp(script1, "}"));
}

void loadfishing()
{
	do
	{
		readw2();
		if(!(strcmp(script1,"BASE_FISHING_TIME"))) fishing_data.basetime=str2num(script2);
		else if(!(strcmp(script1,"RANDOM_FISHING_TIME"))) fishing_data.randomtime=str2num(script2);
	}
	while (strcmp(script1, "}"));
}

void loadspiritspeak()
{
	do
	{
		readw2();
		if(!(strcmp(script1,"SPIRITSPEAKTIMER"))) spiritspeak_data.spiritspeaktimer=str2num(script2);
	}
	while (strcmp(script1, "}"));
}

void loadtime_light()
{
	do
	{
		readw2();
		if(!(strcmp(script1,"DAY"))) day=str2num(script2);
		else if(!(strcmp(script1,"HOUR"))) hour=str2num(script2);
		else if(!(strcmp(script1,"MINUTE"))) minute=str2num(script2);
		else if(!(strcmp(script1,"AMPM"))) ampm=str2num(script2);
		else if(!(strcmp(script1,"MOON1UPDATE"))) moon1update=str2num(script2);
		else if(!(strcmp(script1,"MOON2UPDATE"))) moon2update=str2num(script2);
		else if(!(strcmp(script1,"MOON1"))) moon1 = (char)str2num(script2);
		else if(!(strcmp(script1,"MOON2"))) moon2 = (char)str2num(script2);
		else if(!(strcmp(script1,"DUNGEONLIGHTLEVEL"))) dungeonlightlevel = (char)str2num(script2);
		else if(!(strcmp(script1,"WORLDFIXEDLEVEL"))) worldfixedlevel = (char)str2num(script2);
		else if(!(strcmp(script1,"WORLDCURLEVEL"))) worldcurlevel = (char)str2num(script2);
		else if(!(strcmp(script1,"WORLDBRIGHTLEVEL"))) worldbrightlevel = (char)str2num(script2);
		else if(!(strcmp(script1,"WORLDDARKLEVEL"))) worlddarklevel = (char)str2num(script2);
		else if(!(strcmp(script1,"SECONDSPERUOMINUTE"))) secondsperuominute=str2num(script2);
	}
	while (strcmp(script1, "}"));
}

void loadserverdefaults( void )
{
	server_data.crashprotect = 1;
	server_data.auto_a_create = 0;
	server_data.snoopiscrime = false;
	combat.wrestleSpeed = 8;
	combat.maxDmg = 120;
	combat.dToPoison = 2;
	combat.maxRange = 10;
	combat.maxRangeSpell = 10;
	combat.deathOnThroat = 1;
	combat.explodeDelay = 0;
	strcpy( server_data.specialbanktrigger, SPECIALBANKTRIGGER ); // AntiChrist - Special Bank word trigger
	server_data.usespecialbank = USESPECIALBANK; // AntiChrist - 1 = Special Bank enabled
	server_data.stat_advance=20;			// Gunther - Added 09/23/2000 for stat advance fix
	
	cwmWorldState->SetLoopSaveAmt( 250 );
	
	// load defaults values
	shopTime=server_data.shopSpawnTime=60;		// Revana
	server_data.poisontimer=POISONTIMER; // lb
	server_data.potiondelay = POTIONDELAY;
	server_data.decaytimer=DECAYTIMER;
	server_data.skillcap = SKILLCAP; // LB, default = no cap
	server_data.statcap = STATCAP; // AntiChrist, default = no cap
	server_data.playercorpsedecaymultiplier=PLAYERCORPSEDECAYMULTIPLIER;
	server_data.lootdecayswithcorpse=LOOTDECAYSWITHCORPSE;
	server_data.invisibiliytimer=INVISTIMER;
	server_data.hungerrate=HUNGERRATE;
	server_data.hungerdamagerate=HUNGERDAMAGERATE;//AntiChrist
	server_data.hungerdamage=HUNGERDAMAGE;//AntiChrist
	server_data.hungerThreshold = 3;
	server_data.skilldelay=SKILLDELAY;
	server_data.objectdelay=OBJECTDELAY;
	server_data.hitpointrate=REGENRATE1;
	server_data.staminarate=REGENRATE2;
	server_data.manarate=REGENRATE3;
	server_data.gatetimer=GATETIMER;
	server_data.minecheck=MINECHECK;
	server_data.showdeathanim=SHOWDEATHANIM;
	server_data.showloggedoutpcs = SHOWLOGGEDOUTPCS;	// Melen 9/7/99
	server_data.combathitmessage=COMBATHITMESSAGE;
	server_data.attackstamina = ATTACKSTAMINA; // AntiChrist
	server_data.monsters_vs_animals=MONSTERS_VS_ANIMALS;
	server_data.animals_attack_chance=ANIMALS_ATTACK_CHANCE;
	server_data.animals_guarded=ANIMALS_GUARDED;
	server_data.npc_base_fleeat=NPC_BASE_FLEEAT;
	server_data.npc_base_reattackat=NPC_BASE_REATTACKAT;
	server_data.maxabsorbtion = MAX_ABSORBTION; // Magius(CHE)
	server_data.maxnohabsorbtion = MAX_NON_HUMAN_ABSORBTION; // Magius(CHE)
	server_data.sellbyname = SELLBYNAME; // Magius(CHE)
	server_data.skilllevel = SKILLLEVEL; // Magius(CHE)
	server_data.sellmaxitem = SELLMAXITEM; // Magius(CHE)
	server_data.npcdamage = NPC_DAMAGE_RATE; // Magius(CHE)
	server_data.rank_system = RANKSYSTEM; // Rank-System by Magius (CHE)
	server_data.buyThreshold = 2000;
	server_data.guardsactive=1;
	server_data.polyduration = 90;
	//  EviLDeD  -  Set default  of worldsave saves to 0(false)
	//  December 27, 1998
	server_data.announceworldsaves=1;
	server_data.wwwaccounts=0;
	server_data.bg_sounds=5;	// Made this every 5 seconds (ARRRG Damn dog!)
	server_data.joinmsg=1;
	server_data.partmsg=1;
	server_data.log=1;
	server_data.rogue=1;
	weathertime=server_data.weathertime=60;		// Revana
	server_data.quittime=300;//Instalog
	server_data.archivepath[0] = 0; // was strcpy(server_data.archivepath,"");
	server_data.backup_save_ratio = 1; // LB
	server_data.UOXBot=0;
	//LordB Lag Fix
	server_data.maxstealthsteps=10;// AntiChrist - stealth=100.0 --> 10 steps allowed
	server_data.runningstaminasteps=15;// AntiChrist - how often ( in steps ) stamina decreases while running
	
	server_data.boatspeed=0.75;//Boats
	server_data.cutscrollreq = 1; // AntiChrist
	server_data.persecute = 1; // AntiChrist
	server_data.html=-1;//HTML
	
	server_data.msgboardpath[0] = 0;       // Dupois - Added Dec 20, 1999 for message boards (current dir)
	server_data.msgpostaccess=0;           // Dupois - Added Dec 20, 1999 for message boards (GM only)
	server_data.msgpostremove=0;           // Dupois - Added Dec 20, 1999 for message boards (GM only)
	server_data.msgretention=30;           // Dupois - Added Dec 20, 1999 for message boards (30 Days)
	server_data.escortactive=1;            // Dupois - Added Dec 20, 1999 for message boards (Active)
	server_data.escortinitexpire=86400;    // Dupois - Added Dec 20, 1999 for message boards (24 hours)
	server_data.escortactiveexpire=1800;   // Dupois - Added Dec 20, 1999 for message boards (1/2 Hour)
	server_data.escortdoneexpire=1800;     // Dupois - Added Dec 20, 1999 for message boards (1/2 Hour)
	server_data.footSteps = false;			   // No footstep sounds by default (Abaddon 17th February, 2000)
	server_data.commandPrefix = '\''; 
	
	server_data.lordblagfix=0;
	speed.srtime=30;
	speed.itemtime=1.5;
	speed.npctime=1.0; // lb, why -1 zippy ??????
	speed.npcaitime=2;
	speed.nice=2;
	speed.checkmem=30;
	//	Map->Cache=0;
	speed.tilecheck = 24;
	speed.accountFlush = 0;
	
	resource.logs=3;
	resource.logtime=600;
	resource.logarea=10;
	resource.ore=10;
	resource.oretime=600;
	resource.orearea=10;
	//REPSYS
	repsys.crimtime=120;
	repsys.maxkills=4;
	repsys.murderdecay=60;
	//RepSys ---^
	tracking_data.baserange=TRACKINGRANGE;
	tracking_data.maxtargets=MAXTRACKINGTARGETS;
	tracking_data.basetimer=TRACKINGTIMER;
	tracking_data.redisplaytime=TRACKINGDISPLAYTIME;
	begging_data.range=BEGGINGRANGE;
	strcpy(begging_data.text[0],"Could thou spare a few coins?");
	strcpy(begging_data.text[1],"Hey buddy can you spare some gold?");
	strcpy(begging_data.text[2],"I have a family to feed, think of the children.");
	fishing_data.basetime=FISHINGTIMEBASE;
	fishing_data.randomtime=FISHINGTIMER;
	spiritspeak_data.spiritspeaktimer=SPIRITSPEAKTIMER;
}

int numbitsset( int number )
{
	int bitsset = 0;
	
	while( number )
	{
		if( number & 0x1 ) 
			bitsset++;
		number >>= 1;
	}
	return bitsset;
}

int whichbit( int number, int bit )
{
	int i, setbits = 0, whichbit = 0, intsize = sizeof( int ) * 8;
	
	for( i = 0; i < intsize; i++ )
	{
		if( number & 0x1 ) 
			setbits++;
		
		if( setbits == bit )
		{
			whichbit = i + 1;
			break;
		}
		number >>= 1;
	}
	
	return whichbit;
}

// Added by Krozy on 7-Sep-98
// New getstatskillvalue function.
// Takes a string, gets the tokens.
// If its one value - It returns that value.
// If its two values - It gets a random number between the values
unsigned short int getstatskillvalue(char *stringguy) {
	char values[512];
	unsigned short int lovalue,hivalue,retcode;
	
	strcpy(values, stringguy);
	gettokennum(values, 0);
	lovalue = (unsigned short int)str2num(gettokenstr);
	gettokennum(values, 1);
	hivalue = (unsigned short int)str2num(gettokenstr);
	
	if (hivalue) {
		retcode = (unsigned short int)RandomNum(lovalue, hivalue);
	} else {
		retcode = lovalue;
	}
	return retcode;
}

void advancementobjects(int s, int x, int allways)
{
	char sect[512];
	int val = 0;
	
	int packnum,hairobject=-1, beardobject=-1;
	int /*j,*/pos,i,retitem=-1;
	if ((chars[s].advobj==0)||(allways==1))
	{
		staticeffect(s, 0x37, 0x3A, 0, 15);
		soundeffect2(s, 0x01, 0xE9);
		chars[s].advobj=x;
		openscript("advance.scp");
		sprintf(sect, "ADVANCEMENT %i", x);
		if (!i_scripts[advance_script]->find(sect))
		{
			closescript();
			printf("ADVANCEMENT OBJECT: Script section not found. Aborting.\n");
			chars[s].advobj=0;
			return;
		}
		else
			do 
			{
				read2();
				if (script1[0]!='}')
				{
					switch(script1[0])
					{
					case 'a':
					case 'A':
						if (!(strcmp("ALCHEMY",script1))) val = ALCHEMY;
						else if (!(strcmp("ANATOMY",script1))) val = ANATOMY;
						else if (!(strcmp("ANIMALLORE",script1))) val = ANIMALLORE;
						else if (!(strcmp("ARMSLORE",script1))) val = ARMSLORE;
						else if (!(strcmp("ARCHERY",script1))) val = ARCHERY;
						else if (!(strcmp("ADVOBJ",script1)))
						{
							x=str2num(script2);
							chars[s].advobj=x;
						}
						break;
						
					case 'b':
					case 'B':
						if (!(strcmp("BEGGING",script1))) val = BEGGING;
						else if (!(strcmp("BLACKSMITHING",script1))) val = BLACKSMITHING;
						else if (!(strcmp("BOWCRAFT",script1))) val = BOWCRAFT;
						break;
						
					case 'c':
					case 'C':
						if (!(strcmp("CAMPING",script1))) val = CAMPING;
						else if (!(strcmp("CARPENTRY",script1))) val = CARPENTRY;
						else if (!(strcmp("CARTOGRAPHY",script1))) val = CARTOGRAPHY;
						else if (!(strcmp("COOKING",script1))) val = COOKING;
						break;
						
					case 'd':
					case 'D':
						if ((!(strcmp("DEX",script1)))||(!(strcmp("DEXTERITY",script1))))
						{
							chars[s].dx  = getstatskillvalue(script2);
							chars[s].dx2 = chars[s].dx;
						}
						if (!(strcmp("DETECTINGHIDDEN",script1))) val = DETECTINGHIDDEN;
						if ((!(strcmp("DYEHAIR",script1)))) 
						{
							int serial,serhash,ci;
							serial=chars[s].serial;
							serhash=serial%HASHMAX;
							for (ci=0;ci<contsp[serhash].max;ci++)
							{
								i=contsp[serhash].pointer[ci];
								if (i!=-1)
								{
									if ((items[i].layer==0x0B) && (items[i].contserial==serial))
									{
										hairobject=i;
										break;
									}
								}
							}
							if (hairobject>-1)
							{
								x = hstr2num(script2);
								items[hairobject].color1 = (unsigned char)(x>>8);
								items[hairobject].color2 = (unsigned char)(x%256);
								//									for (j=0;j<now;j++) if (perm[j]) senditem(j,hairobject);  
								RefreshItem( hairobject ); // AntiChrist
								teleport(s);
							}
						}
						
						if ((!(strcmp("DYEBEARD",script1)))) 
						{
							int serial,serhash,ci;
							serial=chars[s].serial;
							serhash=serial%HASHMAX;
							for (ci=0;ci<contsp[serhash].max;ci++)
							{
								i=contsp[serhash].pointer[ci];
								if (i!=-1)
								{
									if ((items[i].layer==0x10) && (items[i].contserial==serial))
									{
										beardobject=i;
										break;
									}
								}
							}
							if (beardobject>-1)
							{
								x = hstr2num(script2);
								items[beardobject].color1 = (unsigned char)(x>>8);
								items[beardobject].color2 = (unsigned char)(x%256);
								RefreshItem( beardobject ); // AntiChrist
								teleport(s);
							}
						}
						
						break;
						
					case 'e':
					case 'E':
						if (!(strcmp("ENTICEMENT",script1))) val = ENTICEMENT;
						else if (!(strcmp("EVALUATINGINTEL",script1))) val = EVALUATINGINTEL;
						break;
						
					case 'f':
					case 'F':
						if( !(strcmp( "FAME", script1 ) ) ) chars[s].fame = str2num( script2 );
						else if (!(strcmp("FENCING",script1))) val = FENCING;
						else if (!(strcmp("FISHING",script1))) val = FISHING;
						else if (!(strcmp("FORENSICS",script1))) val = FORENSICS;
						break;
						
					case 'h':
					case 'H':
						if (!(strcmp("HEALING",script1))) val = HEALING;
						else if (!(strcmp("HERDING",script1))) val = HERDING;
						else if (!(strcmp("HIDING",script1))) val = HIDING;
						break;
						
					case 'i':
					case 'I':
						if ((!(strcmp("INT",script1)))||(!(strcmp("INTELLIGENCE",script1))))
						{
							chars[s].in  = getstatskillvalue(script2);
							chars[s].in2 = chars[s].in;
						}
						else if (!(strcmp("ITEMID",script1))) val = ITEMID;
						else if (!(strcmp("INSCRIPTION",script1))) val = INSCRIPTION;
						else if (!(strcmp("ITEM",script1)))
						{
							x=str2num(script2);            
							pos=ftell(scpfile);
							closescript();  /* lord binary */
							retitem=Targ->AddMenuTarget(-1, 0, x);
							openscript("advance.scp");
							fseek(scpfile, pos, SEEK_SET);
							strcpy(script1, "DUMMY");   
							packnum=packitem(s);
							if (retitem>-1)
							{
								items[retitem].x = (short int)50+(rand()%80);
								items[retitem].y = (short int)50+(rand()%80);
								items[retitem].z=9;
								if(items[retitem].layer==0x0b || items[retitem].layer==0x10)
								{
									setserial(retitem,s,4);
								}
								else
								{
									setserial(retitem,packnum,1);
								}
								//								  for (i=0;i<now;i++) if (perm[i])
								//									senditem(i,retitem);                
								RefreshItem( retitem ); // AntiChrist
								teleport(s);
							}
						}
						break;
						
					case 'k':
					case 'K':
						if( !(strcmp("KARMA", script1 ) ) ) chars[s].karma = str2num( script2 );
						else if (!(strcmp("KILLHAIR",script1)))
						{
							int serial,serhash,ci;
							serial=chars[s].serial;
							serhash=serial%HASHMAX;
							for (ci=0;ci<contsp[serhash].max;ci++)
							{
								i=contsp[serhash].pointer[ci];
								if (i!=-1)
									if ((items[i].layer==0x0B) && (items[i].contserial==serial))
									{
										Items->DeleItem(i);
										break;
									}   
							}  
						}
						else if (!(strcmp("KILLBEARD",script1)))
						{
							int serial,serhash,ci;
							serial=chars[s].serial;
							serhash=serial%HASHMAX;
							for (ci=0;ci<contsp[serhash].max;ci++)
							{
								i=contsp[serhash].pointer[ci];
								if (i!=-1)
									if ((items[i].layer==0x10) && (items[i].contserial==serial))
									{
										Items->DeleItem(i);
										break;
									}   
							} 
						}
						else if (!(strcmp("KILLPACK",script1)))
						{
							int serial,serhash,ci;
							serial=chars[s].serial;
							serhash=serial%HASHMAX;
							for (ci=0;ci<contsp[serhash].max;ci++)
							{
								i=contsp[serhash].pointer[ci];
								if (i!=-1)
									if ((items[i].layer==0x15) && (items[i].contserial==serial))
									{
										Items->DeleItem(i);
										break;
									}   
							} 
						}
						break;
						
					case 'l':
					case 'L':
						if (!(strcmp("LOCKPICKING",script1))) val = LOCKPICKING;
						else if (!(strcmp("LUMBERJACKING",script1))) val = LUMBERJACKING;
						break;
						
					case 'm':
					case 'M':
						if (!(strcmp("MAGERY",script1))) val = MAGERY;
						else if (!(strcmp("MAGICRESISTANCE",script1))) val = MAGICRESISTANCE;
						else if (!(strcmp("MACEFIGHTING",script1))) val = MACEFIGHTING;
						else if (!(strcmp("MEDITATION",script1))) val = MEDITATION;
						else if (!(strcmp("MINING",script1))) val = MINING;
						else if (!(strcmp("MUSICIANSHIP",script1))) val = MUSICIANSHIP;
						break;
						
					case 'p':
					case 'P':
						if (!(strcmp("PARRYING",script1))) val = PARRYING;
						else if (!(strcmp("PEACEMAKING",script1))) val = PEACEMAKING;
						else if (!(strcmp("POISONING",script1))) val = POISONING;
						else if (!(strcmp("PROVOCATION",script1))) val = PROVOCATION;
						else if (!(strcmp("POLY",script1)))
						{
							x=hstr2num(script2);
							chars[s].id1 = (unsigned char)(x>>8);
							chars[s].xid1 = (unsigned char)(x>>8);
							chars[s].orgid1 = (unsigned char)(x>>8);
							chars[s].id2 = (unsigned char)(x%256);
							chars[s].xid2 = (unsigned char)(x%256);
							chars[s].orgid2 = (unsigned char)(x%256);
							teleport(s);  
						}
						break;
						
					case 'r':
					case 'R':
						if (!(strcmp("REMOVETRAPS",script1))) val = REMOVETRAPS;
						break;
						
					case 's':
					case 'S':
						if ((!(strcmp("STR",script1)))||(!(strcmp("STRENGTH",script1))))
						{
							chars[s].st  = getstatskillvalue(script2);
							chars[s].st2 = chars[s].st;
						}
						else if (!(strncmp("SKILL",script1,5)))
						{
							// get number code
							val = str2num(script1+5);
						}
						else if (!(strcmp(script1,"SKIN")))
						{
							i=hstr2num(script2);
							chars[s].skin1 = (unsigned char)(i>>8);
							chars[s].skin2 = (unsigned char)(i%256);
							chars[s].xskin1 = (unsigned char)(i>>8);
							chars[s].xskin2 = (unsigned char)(i%256);
							teleport(s);
						}
						else if (!(strcmp("SNOOPING",script1))) val = SNOOPING;
						else if (!(strcmp("SPIRITSPEAK",script1))) val = SPIRITSPEAK;
						else if (!(strcmp("STEALING",script1))) val = STEALING;
						else if (!(strcmp("STEALTH",script1))) val = STEALTH;
						else if (!(strcmp("SWORDSMANSHIP",script1))) val = SWORDSMANSHIP;
						break;
						
					case 't':
					case 'T':
						if (!(strcmp("TACTICS",script1))) val = TACTICS;
						else if (!(strcmp("TAILORING",script1))) val = TAILORING;
						else if (!(strcmp("TAMING",script1))) val = TAMING;
						else if (!(strcmp("TASTEID",script1))) val = TASTEID;
						else if (!(strcmp("TINKERING",script1))) val = TINKERING;
						else if (!(strcmp("TRACKING",script1))) val = TRACKING;
						break;
						
					case 'v':
					case 'V':
						if (!(strcmp("VETERINARY",script1))) val = VETERINARY;
						break;
						
					case 'w':
					case 'W':
						if (!(strcmp("WRESTLING",script1))) val = WRESTLING;
						break;
					}
					
					// set skill
					if (val > 0)
					{
						chars[s].baseskill[val] = getstatskillvalue(script2);
						val = 0;			// reset for next time through
					}
				}
			}
			while (script1[0]!='}');
			closescript();
	}
	else sysmessage(calcSocketFromChar(s),"You have already used an advancement object with this character.");
}

// Dupois - added to do easy item sound effects based on an 
//          items id1 and id2 fields in struct items. Then just define the CASE statement
//          with the proper sound function to play for a certain item as shown.
//          Use the DEFAULT case for ranges of  items (like all ingots make the same thump).
// Sounds - coins dropping (all the same no matter what amount because all id's equal 0x0EED
//          ingot dropping (makes a big thump - used the heavy gem sound)
//          gems dropping (two type broke them in half to make some sound different then others)
// NOTE   - I wasn't sure what the different soundeffect() func's did so I just used 
//          soundeffect() and it seemed to work fairly well.
// Added Oct 09, 1998
void itemsfx(int s, int ID1, int ID2, bool bAllHear)
{
	int item=0;
	
	item = (ID1<<8) + ID2;
	
	if (item==0x0EED)
	{
		goldsfx(s, 2, bAllHear);
		//break;
	}
	else if ((item>=0x0F0F)&&(item<=0x0F20))  // Any gem stone (typically smaller)
	{
		soundeffects(s, 0x00, 0x32, bAllHear);
		//break;
	}
	else if ((item>=0x0F21)&&(item<=0x0F30))  // Any gem stone (typically larger)
	{
		soundeffects(s, 0x00, 0x34, bAllHear);
		//break;
	}
	else if ((item>=0x1BE3)&&(item<=0x1BFA))  // Any Ingot
	{
		soundeffects(s, 0x00, 0x33, bAllHear);
		//break;
	}
	else
		soundeffects(s, 0x00, 0x42, bAllHear); 
} // Dupois - end 

void init_creatures(void) // assigns the basesound, soundflag, who_am_i flag of all npcs
// Checked and revised by Leafbeach in November-December 2000 (added sounds to T2A creatures and fixed some others)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// soundflags  0: normal, 5 sounds (attack-started,idle, attack, defence, dying, see uox.h)
    //             1: birds .. only one "bird-shape" and zillions of sounds ...
	//             2: only 3 sounds ->  (attack,defence,dying)    
	//             3: only 4 sounds ->   (attack-started,attack,defnce,dying)
	//             4: only 1 sound !!
	//
	// who_am_i bit # 1 creature can fly (must have the animations, so better not change)
	//              # 2 anti-blink: these creatures don't have animation #4, if not set creature will randomly disappear in battle
	//                              if you find a creature that blinks while fighting, set that bit
	//              # 3 animal-bit (currently not used/set)
	//              # 4 water creatures (currently not used/set)
	// icon: used for tracking, to set the appropriate icon
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	for( int a = 0; a < 2048; a++ )
	{ 
		creatures[a].basesound = creatures[a].soundflag =
			creatures[a].who_am_i = creatures[a].icon=0;
	}
	
	creatures[0x01].basesound = 0x01AB;                            // Ogre
	creatures[0x01].icon = 8415;

    creatures[0x02].basesound = 0x016F;                            // Ettin (unarmed)
	creatures[0x02].icon = 8408;

    creatures[0x03].basesound = 0x01D7;                            // Zombie
	creatures[0x03].icon = 8428;

    creatures[0x04].basesound = 0x0174;                            // Gargoyle
	creatures[0x04].who_am_i+= 1; // set can_fly_bit
	creatures[0x04].icon = 8409;

    creatures[0x05].basesound = 0x008F;                            // Eagle
	creatures[0x05].who_am_i += 1; // set can_fly bit
	creatures[0x05].who_am_i += 2; // set anti blink bit
	creatures[0x05].icon = 8477;

    creatures[0x06].basesound = 0x007D;                            // Bird
	creatures[0x06].who_am_i += 1; //set fly bit
	creatures[0x06].soundflag = 1; // birds need special treatment cause there are about 20 bird-sounds
	creatures[0x06].icon = 8474;

	creatures[0x07].basesound = 0x01B0;                            // Orc (with armor and axe)                      
	creatures[0x07].icon = 8416;

    creatures[0x08].basesound = 0x0161;                            // Corpser
	creatures[0x08].soundflag = 3;
	creatures[0x08].icon = 8402;

	creatures[0x09].basesound = 0x0165;                            // Daemon (unarmed) 
	creatures[0x09].who_am_i += 1;
	creatures[0x09].icon = 8403;

	creatures[0x0a].basesound = 0x0165;                            // Daemon with sword
	creatures[0x0a].who_am_i += 1;
	creatures[0x0a].icon = 8403;
	
	
	creatures[0x0c].basesound = 0x016A;							// Dragon (grey)
	creatures[0x0c].who_am_i += 1; // flying creature
	creatures[0x0c].icon = 8406;

	creatures[0x0d].basesound = 0x0107;								// Air elemental
	creatures[0x0d].icon = 8429;

	creatures[0x0e].basesound = 0x010C;								// Earth elemental	
	creatures[0x0e].icon = 8407;

	creatures[0x0f].basesound = 0x0111;								// Fire elemental
	creatures[0x0f].icon = 8435;

	creatures[0x10].basesound = 0x0116;								// Water elemental
	creatures[0x10].icon = 8459;

    creatures[0x11].basesound = 0x01B0;                            // Orc (unarmed)
	creatures[0x11].icon = 8416;

    creatures[0x12].basesound = 0x016F;                           // Ettin with hammer
	creatures[0x12].icon=8408;
	
	
	creatures[0x15].basesound = 0x00DB;                               // Giant serpent
	creatures[0x15].icon = 8444;

	creatures[0x16].basesound = 0x0179;                               // Gazer
	creatures[0x16].icon = 8436;
	
	creatures[0x18].basesound = 0x019D;                               // Lich
	creatures[0x18].icon = 8440;
	
	creatures[0x1a].basesound = 0x017E;                               // Shade, Spectre, Ghoul & Wraith
	creatures[0x1a].icon = 8457;
	
	creatures[0x1c].basesound = 0x0183;                               // Giant spider
    creatures[0x1c].icon = 8445;

	creatures[0x1d].basesound = 0x009E;                               // Gorilla
	creatures[0x1d].icon = 8437;

	creatures[0x1e].basesound = 0x0192;                               // Harpy			
	creatures[0x1e].icon = 8412;

	creatures[0x1f].basesound = 0x0197;                               // Headless
	creatures[0x1f].icon = 8458;
	
	creatures[0x21].basesound = 0x01A1;							// Lizardman (unarmed)
	creatures[0x21].icon = 8414;

	creatures[0x23].basesound = 0x01A1;							// Lizardman with spear
	creatures[0x23].icon = 8414;

	creatures[0x24].basesound = 0x01A1;							// Lizardman with mace
	creatures[0x24].icon = 8414;

	creatures[0x25].basesound = 0x01A1;
	creatures[0x25].icon = 8414;

	creatures[0x26].basesound = 0x01A1;
	creatures[0x26].icon = 8414;
	
	
	creatures[0x27].basesound = 0x01A6;                               // Mongbat
	creatures[0x27].who_am_i += 1; // yes, they can fly
	creatures[0x27].icon = 8441;
	
	creatures[0x29].basesound = 0x01B0;								// Orc with club
	creatures[0x29].icon = 8416;

	creatures[0x2a].basesound = 0x01B5;								// Ratman (unarmed)
	creatures[0x2a].icon = 8419;
	
	creatures[0x2c].basesound = 0x01B5;								// Ratman with axe
	creatures[0x2c].icon = 8419; 

	creatures[0x2d].basesound = 0x01B5;								// Ratman with sword
	creatures[0x2d].icon = 8419;
	
	creatures[0x2f].basesound = 0x01BA;								// Reaper			
	creatures[0x2f].icon = 8442;

	creatures[0x30].basesound = 0x018D;								// Giant scorprion	
	creatures[0x30].icon = 8420;
	
	creatures[0x32].basesound = 0x01C3;								// Skeleton (unarmed)
	creatures[0x32].icon = 8423;

	creatures[0x33].basesound = 0x01C8;								// Slime	
    creatures[0x33].icon = 8424;

	creatures[0x34].basesound = 0x00DC;								// Snake
	creatures[0x34].icon = 8446;

    creatures[0x35].basesound = 0x01CD;								// Troll with axe				
    creatures[0x35].icon = 8425;

    creatures[0x36].basesound = 0x01CD;								// Troll (unarmed)
	creatures[0x36].icon = 8425;

    creatures[0x37].basesound = 0x01CD;								// Troll with maul
	creatures[0x37].icon = 8425;

    creatures[0x38].basesound = 0x01C3;								// Skeleton with axe
	creatures[0x38].icon = 8423;

	creatures[0x39].basesound = 0x01C3;								// Skeleton with sword and shield
	creatures[0x39].icon = 8423;
	
	creatures[0x3a].basesound = 0x01D2;								// Wisp	                      
	creatures[0x3a].icon = 8448;
	
	creatures[0x3b].basesound = 0x016A;								// Dragon (red)
	creatures[0x3b].who_am_i += 1; // set fly bit
	creatures[0x3b].icon = 8406;

	creatures[0x3c].basesound = 0x016A;								// Drake (grey)
	creatures[0x3c].who_am_i += 1;
	creatures[0x3c].icon = 8406;
	
	creatures[0x3d].basesound = 0x016A;								// Drake (red)
	creatures[0x3d].who_am_i += 1;
	creatures[0x3d].icon = 8406;
	
	creatures[0x46].basesound = 589;		// Terathan Warrior
	creatures[0x46].icon = 8490;

	creatures[0x47].basesound = 594;		// Terathan Drone
	creatures[0x47].icon = 8491;

	creatures[0x48].basesound = 599;		// Terathan Matriarch
	creatures[0x48].icon = 8492;

	creatures[0x4b].basesound = 604;		// Cyclops
	creatures[0x4b].icon = 8493;

	creatures[0x4c].basesound = 609;		// Titan
	creatures[0x4c].icon = 8494;

	creatures[0x50].basesound = 619;		// Giant toad
	creatures[0x50].icon = 8495;

	creatures[0x51].basesound = 614;		// Bullfrog
	creatures[0x51].icon = 8496;

	creatures[0x55].basesound = 639;		// Ophidian Mage
	creatures[0x55].icon = 8498;

	creatures[0x56].basesound = 634;		// Ophidian Warrior
	creatures[0x56].icon = 8499;

	creatures[0x57].basesound = 644;		// Ophidian Matriarch
	creatures[0x57].icon = 8500;

	creatures[0x96].basesound = 447;                               // Sea serpent
	creatures[0x96].soundflag = 3;
	creatures[0x96].icon = 8443; // Sea serpent-looking icon

	creatures[0x97].basesound = 138;								// Dolphin
	creatures[0x97].icon = 8433;
	
	creatures[0xc8].basesound = 168;								// Horse (light brown)
    creatures[0xc8].icon = 8484;

	creatures[0xc9].basesound = 105;								// Cat
	creatures[0xc9].who_am_i += 2; // set blink flag
    creatures[0xc9].icon = 8475;

	creatures[0xca].basesound = 90;									// Alligator
    creatures[0xca].icon = 8410;

    creatures[0xcb].basesound = 196;								// Pig
    creatures[0xcb].icon = 8449;

    creatures[0xcc].basesound = 168;                               // Horse (brown)
    creatures[0xcc].icon = 8481;

	creatures[0xcd].basesound = 201;                               // Rabbit
	creatures[0xcd].soundflag = 2;                                 // rabbits only have 3 sounds, thus need special treatment
	creatures[0xcd].icon=8485;
	
	creatures[0xce].basesound = 649;		// Lava lizard
	creatures[0xce].icon = 8497;

	creatures[0xcf].basesound = 214;                               // Sheep (unshorn)
	creatures[0xcf].icon = 8422;
	
	creatures[0xd0].basesound = 110;                               // Chicken
	creatures[0xd0].icon = 8401;

	creatures[0xd1].basesound = 153;                               // Goat
	creatures[0xd1].icon = 8427;
	
	creatures[0xd2].basesound = 624;		// Desert ostard
	creatures[0xd2].icon = 8501;

	creatures[0xd3].basesound = 95;                                // Bear (small)
    creatures[0xd3].icon = 8399;

	creatures[0xd4].basesound = 163;                                // Grizzly bear
	creatures[0xd4].icon = 8411;

	creatures[0xd5].basesound = 95;                                // Polar bear	
	creatures[0xd5].icon = 8417;

	creatures[0xd6].basesound = 186;                               // Panther
	creatures[0xd6].who_am_i += 2;
    creatures[0xd6].icon = 8473;

	creatures[0xd7].basesound = 392;                               // Giant rat
	creatures[0xd7].icon = 8400;

	creatures[0xd8].basesound = 120;                               // Cow (black and white)
    creatures[0xd8].icon = 8451;

	creatures[0xd9].basesound = 133;								// Dog
	creatures[0xd9].who_am_i += 2;									// set anti blink bit
	creatures[0xd9].icon = 8476;
	
	creatures[0xda].basesound = 629;		// Frenzied ostard
	creatures[0xda].icon = 8502;

	creatures[0xdb].basesound = 624;		// Forest ostard
	creatures[0xdb].icon = 8503;

	creatures[0xdc].basesound = 183;                               // Llama
	creatures[0xdc].soundflag = 2;
	creatures[0xdc].icon = 8438;
	
	creatures[0xdd].basesound = 224;                               // Walrus
	creatures[0xdd].icon = 8447;
	
	creatures[0xdf].basesound = 214;                               // Sheep (shorn)
	creatures[0xdf].icon = 8422;

	creatures[0xe1].basesound = 229;                               // Wolf
	creatures[0xe1].who_am_i += 2; // set anti blink bit
    creatures[0xe1].icon = 8426;

	creatures[0xe2].basesound = 168;								// Horse (white)
	creatures[0xe2].icon = 8479;
	
	creatures[0xe4].basesound = 168;                               // Horse (beige with white forehead)
	creatures[0xe4].icon = 8480;
	
	creatures[0xe7].basesound = 120;                               // Cow (brown and white)
	creatures[0xe7].who_am_i += 2;
	creatures[0xe7].icon = 8451;

	creatures[0xe8].basesound = 100;                               // Bull (one color)
	creatures[0xe8].who_am_i += 2;
	creatures[0xe8].icon = 8431;

	creatures[0xe9].basesound = 100;                               // Bull (spotted)
	creatures[0xe9].who_am_i += 2;
	creatures[0xe9].icon = 8432;

	creatures[0xea].basesound = 130;                               // Hart
	creatures[0xea].soundflag = 2;
	creatures[0xea].icon = 8404;
	
	creatures[0xed].basesound = 130;                               // Deer
	creatures[0xed].soundflag = 2;
    creatures[0xed].icon = 8404;

	creatures[0xee].basesound = 204;                               // Rat
	creatures[0xee].icon=8483;
	
	creatures[0x122].basesound = 196;                                // Boar
    creatures[0x122].icon = 8449;

	creatures[0x123].basesound = 168;                                // Pack horse
	creatures[0x123].icon = 8486;

	creatures[0x124].basesound = 183;                                // Pack llama	
	creatures[0x124].soundflag = 2;
	creatures[0x124].icon = 8487;
	
	creatures[0x190].icon = 8454;			// Male

	creatures[0x191].icon = 8455;			// Female

	creatures[0x23d].basesound=263;                                 // Death vortex
	creatures[0x23e].basesound=512;                                 // Blade spirit
	creatures[0x23e].soundflag=4;
	
	creatures[0x600].basesound=115;                                // cougar;
	creatures[0x600].icon=8473;
	
	
}


void bgsound( CHARACTER s ) // Plays background sounds of the game
// Slightly revised by Leafbeach in November-December 2000 (took out strange idle sounds for soundflag 2 and 3)
{
	int ds, dx, sound;
	//	int distance=(VISRANGE+5);
	int distance;
	int inrange[15];
	int y=0;
	int basesound=0;
	int bigf,xx,i;
	char sf,bfs;
	
	if (s>-1 && s<cmem); else return;
	distance=Races->getVisRange( chars[s].race );
	
	// lagfix, LB
	int	StartGrid=mapRegions->StartGrid(chars[s].x,chars[s].y);
	
	unsigned int increment=0;
	for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
		for (int a=0;a<3;a++)
		{
			int mapitemptr=-1;
			int mapitem=-1;
			int mapchar=-1;
			do //check all items in this cell
			{
				mapchar=-1;
				mapitemptr=mapRegions->GetNextItem(checkgrid+a, mapitemptr);
				if (mapitemptr==-1) break;
				mapitem=mapRegions->GetItem(checkgrid+a, mapitemptr);
				if(mapitem>999999) mapchar=mapitem-1000000;
				if (mapitem!=-1 && mapitem>=1000000)
				{
					i=mapchar;
					//printf("DEBUG: Mapchar %i [%i]\n",mapchar,mapitem);
					if((chars[i].npc)&&(!(chars[i].dead))&&(!(chars[i].war))&&(y<=10))
					{ 
						if (!chars[i].free) // lb, bugfix !
						{
							ds=abs(chars[s].x-chars[i].x);
							dx=abs(chars[s].y-chars[i].y);
							if((ds<=distance)&&(dx<=distance)) 
							{
								y++;
								inrange[y]=i;
							}
						}
					} 
				}
			}
			
			while (mapitem!=-1);
		}
		
		if (y>0)
		{
			
			sound=((rand()%(y))+1);				
			//		pl=rand()%66;
			
			xx=(chars[inrange[sound]].id1<<8)+chars[inrange[sound]].id2;
			if (xx>-1 && xx<2048) 
			{ 
				basesound=creatures[xx].basesound; 
				sf=creatures[xx].soundflag;
			}
			else return;
			if( basesound != 0 )
			{
				switch(sf) // play only idle sounds, if there arnt any, dont play them !
				{
					
				case 0: ; basesound++; break;  // normal case -> play idle sound
					
				case 1: ; basesound++; break; // birds sounds will be implmented later
					
				case 2:  // no idle sound, so dont play it !
					basesound = -1;
					break;
				case 3: // no idle sound, so dont play it !
					basesound = -1;
					break;
				case 4: // only a single sound, play it !
					break;
				}
				
				if( bgsound != 0 ) // bugfix lb
				{
					sfx[2] = (unsigned char)(basesound>>8);
					sfx[3] = (unsigned char)(basesound%256);
					sfx[6] = (unsigned char)(chars[inrange[sound]].x>>8);
					sfx[7] = (unsigned char)(chars[inrange[sound]].x%256);
					sfx[8] = (unsigned char)(chars[inrange[sound]].y>>8);
					sfx[9] = (unsigned char)(chars[inrange[sound]].y%256);
					Network->xSend(calcSocketFromChar(s), sfx, 12, 0); //bugfix, LB
				}
			}
		}
		
		// play random mystic-sounds also if no creature is in range ...
		
		bigf=rand()%3333; 
		
		basesound=0;
		
		if (bigf==33) 
		{
			bfs = rand()%7;    
			switch(bfs)
			{
			case 0: basesound=595;break; // gnome sound
			case 1: basesound=287;break; // bigfoot 1
			case 2: basesound=288;break; // bigfoot 2
			case 3: basesound=639;break; // old snake sound
			case 4: basesound=179;break; // lion sound 
			case 5: basesound=246;break; // mystic
			case 6: basesound=253;break; // mystic II
				
			}
			
			if (basesound !=0)
			{ 
				sfx[2] = (unsigned char)(basesound>>8);
				sfx[3] = (unsigned char)(basesound%256);
				sfx[6] = (unsigned char)(chars[s].x>>8);
				sfx[7] = (unsigned char)(chars[s].x%256);
				sfx[8] = (unsigned char)(chars[s].y>>8);
				sfx[9] = (unsigned char)(chars[s].y%256);
				Network->xSend(calcSocketFromChar(s), sfx, 12, 0); //bugfix LB
			}
		}
		
} 


void monstergate(int s, int x)
{
	int tmp, n, z, lovalue, hivalue, mypack, retitem, j;
	int storeval, shoppack1, shoppack2, shoppack3;
	//unsigned short int tempskill;
	char sect[512];
	long int pos;
	char rndlootlist[20];
	
  if (chars[s].npc) return;
	
	mypack=-1;
	retitem=-1;
	storeval=-1;
	shoppack1=-1;
	shoppack2=-1;
	shoppack3=-1;
	
	openscript("npc.scp");
	sprintf(sect, "NPC %i", x);
	if (!i_scripts[npc_script]->find(sect))
	{
		closescript();
		if (n_scripts[custom_npc_script][0]!=0)
		{
			openscript(n_scripts[custom_npc_script]);
			if (!i_scripts[custom_npc_script]->find(sect))
			{
				closescript();
				return;
			}
			else strcpy(sect, n_scripts[custom_npc_script]);
		} else return;
	} else strcpy(sect, "npc.scp");
	
	*(chars[s].title)='\0'; // was sprintf(chars[s].title, "");
	//for(z=0;z<itemcount;z++)
	for(j=0;j<contsp[chars[s].serial%HASHMAX].max;j++)
	{
		z=contsp[chars[s].serial%HASHMAX].pointer[j];
		if (z!=-1 && items[z].contserial==chars[s].serial &&
			items[z].layer!=0x15 && items[z].layer!=0x1D &&
			items[z].layer!=0x10 && items[z].layer!=0x0B && !items[z].free)
		{
			if (mypack==-1)
			{
				mypack=packitem(s);
			}
			if (mypack==-1)
			{
				pos=ftell( scpfile );
				closescript();
				chars[s].packitem=n=Items->SpawnItem(calcSocketFromChar(s),s,1,"#",0,0x0E,0x75,0,0,0,0);
				if( n == -1 ) return;
				openscript( sect );
				fseek( scpfile, pos, SEEK_SET );
				setserial(n,s,4);
				items[n].layer=0x15;
				items[n].type=1;
				items[n].dye=1;
				mypack=n;
				retitem=n;
			}
			items[z].x=(rand()%80)+50;
			items[z].y=(rand()%80)+50;
			items[z].z=9;
//			if (items[z].contserial!=-1) removefromptr(&contsp[items[z].contserial%HASHMAX], z);
			unsetserial( z, 1 );
			setserial(z,mypack,1);
			items[z].layer=0x00;
			
			removeitem[1]=items[z].ser1;
			removeitem[2]=items[z].ser2;
			removeitem[3]=items[z].ser3;
			removeitem[4]=items[z].ser4;
			RefreshItem( z ); // AntiChrist
		}
		else if (items[z].contserial==chars[s].serial &&
			(items[z].layer==0x0B || items[z].layer==0x10))
		{
			Items->DeleItem(z);
		}
	}
	
	do
	{
		read2();
		if (script1[0]!='}')
		{
			switch( script1[0] )
			{

			case 'a':
			case 'A':
				if ((!(strcmp("ALCHEMY",script1)))||(!(strcmp("SKILL0",script1))))
					chars[s].baseskill[ALCHEMY] = getstatskillvalue(script2);
				else if ((!(strcmp("ANATOMY",script1)))||(!(strcmp("SKILL1",script1))))
					chars[s].baseskill[ANATOMY] = getstatskillvalue(script2);
				else if ((!(strcmp("ANIMALLORE",script1)))||(!(strcmp("SKILL2",script1))))
					chars[s].baseskill[ANIMALLORE] = getstatskillvalue(script2);
				else if ((!(strcmp("ARMSLORE",script1)))||(!(strcmp("SKILL4",script1))))
					chars[s].baseskill[ARMSLORE] = getstatskillvalue(script2);
				else if ((!(strcmp("ARCHERY",script1)))||(!(strcmp("SKILL31",script1))))
					chars[s].baseskill[ARCHERY] = getstatskillvalue(script2);
				break;
			case 'b':
			case 'B':

				if ((!(strcmp("BEGGING",script1)))||(!(strcmp("SKILL6",script1))))
					chars[s].baseskill[BEGGING] = getstatskillvalue(script2);
				else if ((!(strcmp("BLACKSMITHING",script1)))||(!(strcmp("SKILL7",script1))))
					chars[s].baseskill[BLACKSMITHING] = getstatskillvalue(script2);
				else if ((!(strcmp("BOWCRAFT",script1)))||(!(strcmp("SKILL8",script1))))
					chars[s].baseskill[BOWCRAFT] = getstatskillvalue(script2);

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
				else if ((!(strcmp("CAMPING",script1)))||(!(strcmp("SKILL10",script1))))
					chars[s].baseskill[CAMPING] = getstatskillvalue(script2);
				else if ((!(strcmp("CARPENTRY",script1)))||(!(strcmp("SKILL11",script1))))
					chars[s].baseskill[CARPENTRY] = getstatskillvalue(script2);
				else if ((!(strcmp("CARTOGRAPHY",script1)))||(!(strcmp("SKILL12",script1))))
					chars[s].baseskill[CARTOGRAPHY] = getstatskillvalue(script2);
				else if ((!(strcmp("COOKING",script1)))||(!(strcmp("SKILL13",script1))))
					chars[s].baseskill[COOKING] = getstatskillvalue(script2);
				break;

			case 'd':
			case 'D':

				if ((!(strcmp("DEX",script1)))||(!(strcmp("DEXTERITY",script1))))
				{
					chars[s].dx = getstatskillvalue(script2);
					chars[s].dx2 = chars[s].dx;
					chars[s].stm = chars[s].dx;
				}
				else if ((!(strcmp("DAMAGE",script1)))||(!(strcmp("ATT",script1))))
				{
					gettokennum(script2, 0);
					lovalue=str2num(gettokenstr);
					gettokennum(script2, 1);
					hivalue=str2num(gettokenstr);
					chars[s].lodamage = lovalue;
					chars[s].hidamage = lovalue;
					if(hivalue) chars[s].hidamage = hivalue;
				}
				else if (!(strcmp("DEF",script1))) chars[s].def = getstatskillvalue(script2);
				else if ((!(strcmp("DETECTINGHIDDEN",script1)))||(!(strcmp("SKILL14",script1))))
					chars[s].baseskill[DETECTINGHIDDEN] = getstatskillvalue(script2);
					break;
			case 'e':
			case 'E':
				if ((!(strcmp("ENTICEMENT",script1)))||(!(strcmp("SKILL15",script1))))
					chars[s].baseskill[ENTICEMENT] = getstatskillvalue(script2);
				else if ((!(strcmp("EVALUATINGINTEL",script1)))||(!(strcmp("SKILL16",script1))))
					chars[s].baseskill[EVALUATINGINTEL] = getstatskillvalue(script2);
				break;

			case 'f':
			case 'F':
				if (!(strcmp("FAME",script1))) chars[s].fame=str2num(script2);
				else if ((!(strcmp("FISHING",script1)))||(!(strcmp("SKILL18",script1))))
					chars[s].baseskill[FISHING] =      getstatskillvalue(script2);
				else if ((!(strcmp("FORENSICS",script1)))||(!(strcmp("SKILL19",script1))))
					chars[s].baseskill[FORENSICS] = getstatskillvalue(script2);
				else if ((!(strcmp("FENCING",script1)))||(!(strcmp("SKILL42",script1))))
					chars[s].baseskill[FENCING] = getstatskillvalue(script2);
				break;

			case 'g':
			case 'G':
				if (!(strcmp("GOLD", script1)))
				{
					pos = ftell( scpfile );
					closescript();
					retitem=n=Items->SpawnItem(calcSocketFromChar(s),s,1,"#",1,0x0E,0xED,0,0,1,0);
					if( n == -1 ) return;
					openscript( sect );
					fseek( scpfile, pos, SEEK_SET );
					strcpy( script1, "DUMMY" );
					gettokennum(script2, 0);
					lovalue=str2num(gettokenstr);
					gettokennum(script2, 1);
					hivalue=str2num(gettokenstr);
					if (hivalue==0)
					{
						items[n].amount=lovalue/2 + (rand()%(lovalue/2));
					}
					else
					{
						items[n].amount=lovalue + (rand()%(hivalue-lovalue));
					}
				}
				break;

			case 'h':
			case 'H':
				if (!(strcmp("HIDAMAGE",script1))) chars[s].hidamage=str2num(script2);
				else if ((!(strcmp("HEALING",script1)))||(!(strcmp("SKILL17",script1))))
					chars[s].baseskill[HEALING] = getstatskillvalue(script2);
				else if ((!(strcmp("HERDING",script1)))||(!(strcmp("SKILL20",script1))))
					chars[s].baseskill[HERDING] = getstatskillvalue(script2);
				else if ((!(strcmp("HIDING",script1)))||(!(strcmp("SKILL21",script1))))
					chars[s].baseskill[HIDING] = getstatskillvalue(script2);

				break;

			case 'i':
			case 'I':
				if (!(strcmp("ID",script1)))
				{
					tmp=hstr2num(script2);
					chars[s].id1 = (unsigned char)(tmp>>8);
					chars[s].id2 = (unsigned char)(tmp%256);
					chars[s].xid1=chars[s].id1;
					chars[s].xid2=chars[s].id2;
					chars[s].orgid1=chars[s].id1;
					chars[s].orgid2=chars[s].id2;
				}
				else if (!(strcmp("ITEM",script1)))
				{
					storeval=str2num(script2);
					pos=ftell(scpfile);
					closescript();
					retitem=Targ->AddMenuTarget(-1, 0, storeval);
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					if (retitem>-1)
					{
						setserial(retitem,s,4);
						if (items[retitem].layer==0)
						{
							printf("Warning: Bad NPC Script %d with problem item %d executed!\n", x, storeval);
						}
					}
					strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
				}
				else if ((!(strcmp("INT",script1)))||(!(strcmp("INTELLIGENCE",script1))))
				{
					chars[s].in = getstatskillvalue(script2);
					chars[s].in2 = chars[s].in;
					chars[s].mn = chars[s].in;
				}
				else if ((!(strcmp("ITEMID",script1)))||(!(strcmp("SKILL3",script1))))
					chars[s].baseskill[ITEMID] = getstatskillvalue(script2);
				else if ((!(strcmp("INSCRIPTION",script1)))||(!(strcmp("SKILL23",script1))))
					chars[s].baseskill[INSCRIPTION] = getstatskillvalue(script2);
				break;

			case 'k':
			case 'K':
				if (!(strcmp("KARMA",script1))) chars[s].karma=str2num(script2);
				break;

			case 'l':
			case 'L':
				if (!(strcmp("LOOT",script1)))
				{
					strcpy(rndlootlist, script2);
					pos=ftell(scpfile);
					closescript();
					retitem=Npcs->AddRandomLoot(mypack, rndlootlist);
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
				}
				else if (!(strcmp("LODAMAGE",script1))) chars[s].lodamage=str2num(script2);
				else if ((!(strcmp("LOCKPICKING",script1)))||(!(strcmp("SKILL24",script1))))
					chars[s].baseskill[LOCKPICKING] = getstatskillvalue(script2);
				else if ((!(strcmp("LUMBERJACKING",script1)))||(!(strcmp("SKILL44",script1))))
					chars[s].baseskill[LUMBERJACKING] = getstatskillvalue(script2);
				break;

			case 'm':
			case 'M':
				if ((!(strcmp("MACEFIGHTING",script1)))||(!(strcmp("SKILL41",script1))))
					chars[s].baseskill[MACEFIGHTING] = getstatskillvalue(script2);
				else if ((!(strcmp("MINING",script1)))||(!(strcmp("SKILL45",script1))))
					chars[s].baseskill[MINING] = getstatskillvalue(script2);
				else if ((!(strcmp("MEDITATION",script1)))||(!(strcmp("SKILL46",script1)))) 
					chars[s].baseskill[MEDITATION] = getstatskillvalue(script2);
				else if ((!(strcmp("MAGERY",script1)))||(!(strcmp("SKILL25",script1))))
					chars[s].baseskill[MAGERY] = getstatskillvalue(script2);
				else if ((!(strcmp("MAGICRESISTANCE",script1)))||(!(strcmp("RESIST",script1)))||(!(strcmp("SKILL26",script1))))
					chars[s].baseskill[MAGICRESISTANCE] = getstatskillvalue(script2);
				else if ((!(strcmp("MUSICIANSHIP",script1)))||(!(strcmp("SKILL29",script1))))
					chars[s].baseskill[MUSICIANSHIP] = getstatskillvalue(script2);

				break;
			case 'n':
			case 'N':
				if (!(strcmp("NAME",script1)))
				{
					strcpy(chars[s].name, script2);
				}
				else if (!(strcmp("NAMELIST", script1)))
				{
					pos=ftell(scpfile);
					closescript();
					setrandomname(s,script2);
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					strcpy(script1, "DUMMY"); // To prevent accidental exit of loop.
				}
				break;

			case 'p':
			case 'P':
				if (!(strcmp("PACKITEM",script1)))
				{
					storeval=str2num(script2);
					pos=ftell(scpfile);
					closescript();
					retitem=Targ->AddMenuTarget(-1, 0, storeval);
					openscript(sect);
					fseek(scpfile, pos, SEEK_SET);
					if (retitem >-1)
					{
						setserial(retitem,mypack,1);
						items[retitem].x = (short int)50+(rand()%80);
						items[retitem].y = (short int)50+(rand()%80);
						items[retitem].z = 9;
					}
					strcpy(script1, "DUMMY"); // Prevents unexpected matchups...
					
				}
				else if (!(strcmp("POISON",script1))) chars[s].poison=str2num(script2);
				if ((!(strcmp("PARRYING",script1)))||(!(strcmp("SKILL5",script1))))
					chars[s].baseskill[PARRYING] = getstatskillvalue(script2);
				else if ((!(strcmp("PEACEMAKING",script1)))||(!(strcmp("SKILL9",script1))))
					chars[s].baseskill[PEACEMAKING] = getstatskillvalue(script2);
				else if ((!(strcmp("PROVOCATION",script1)))||(!(strcmp("SKILL22",script1))))
					chars[s].baseskill[PROVOCATION] = getstatskillvalue(script2);
				else if ((!(strcmp("POISONING",script1)))||(!(strcmp("SKILL30",script1))))
					chars[s].baseskill[POISONING] = getstatskillvalue(script2);
				break;

			case 'r':
			case 'R':
				if ((!(strcmp("REMOVETRAPS",script1)))||(!(strcmp("SKILL48",script1)))) 
					chars[s].baseskill[REMOVETRAPS] = getstatskillvalue(script2);
				break;

			case 's':
			case 'S':
				if (!(strcmp("SKIN",script1)))
				{
					tmp=hstr2num(script2);
					chars[s].skin1 = (unsigned char)(tmp>>8);
					chars[s].skin2 = (unsigned char)(tmp%256);
					chars[s].xskin1=chars[s].skin1;
					chars[s].xskin2=chars[s].skin2;
				}
				else if ((!(strcmp("STR",script1)))||(!(strcmp("STRENGTH",script1))))
				{
					chars[s].st = getstatskillvalue(script2);
					chars[s].st2 = chars[s].st;
					chars[s].hp = chars[s].st;
				}
				else if (!(strcmp("SKILL", script1)))
				{
					gettokennum(script2, 0);
					z=str2num(gettokenstr);
					gettokennum(script2, 1);
					chars[s].baseskill[z] = (unsigned short int)str2num(gettokenstr);
				}
				else if(!(strncmp(script1, "SKILL", 5)))
				{
					z = str2num( &script1[5] );
					chars[s].baseskill[z] = (unsigned short int)str2num( script2 );
				}
				else if ((!(strcmp("SNOOPING",script1)))||(!(strcmp("SKILL28",script1))))
					chars[s].baseskill[SNOOPING] = getstatskillvalue(script2);
				else if ((!(strcmp("SPIRITSPEAK",script1)))||(!(strcmp("SKILL32",script1))))
					chars[s].baseskill[SPIRITSPEAK] = getstatskillvalue(script2);
				else if ((!(strcmp("STEALING",script1)))||(!(strcmp("SKILL33",script1))))
					chars[s].baseskill[STEALING] = getstatskillvalue(script2);
				else if ((!(strcmp("SWORDSMANSHIP",script1)))||(!(strcmp("SKILL40",script1))))
					chars[s].baseskill[SWORDSMANSHIP] = getstatskillvalue(script2);
				else if ((!(strcmp("STEALTH",script1)))||(!(strcmp("SKILL47",script1)))) 
					chars[s].baseskill[STEALTH] = getstatskillvalue(script2);
				break;
			
			case 't':
			case 'T':
				if (!(strcmp("TITLE",script1))) strcpy(chars[s].title, script2);
				else if ((!(strcmp("TAILORING",script1)))||(!(strcmp("SKILL34",script1))))
					chars[s].baseskill[TAILORING] = getstatskillvalue(script2);
				else if ((!(strcmp("TAMING",script1)))||(!(strcmp("SKILL35",script1))))
					 chars[s].baseskill[TAMING] = getstatskillvalue(script2);
				else if ((!(strcmp("TASTEID",script1)))||(!(strcmp("SKILL36",script1))))
					chars[s].baseskill[TASTEID] = getstatskillvalue(script2);
				else if ((!(strcmp("TINKERING",script1)))||(!(strcmp("SKILL37",script1))))
					chars[s].baseskill[TINKERING] = getstatskillvalue(script2);
				else if ((!(strcmp("TRACKING",script1)))||(!(strcmp("SKILL38",script1))))
					chars[s].baseskill[TRACKING] = getstatskillvalue(script2);
				else if ((!(strcmp("TACTICS",script1)))||(!(strcmp("SKILL27",script1))))
					chars[s].baseskill[TACTICS] = getstatskillvalue(script2);

				break;

			case 'v':
			case 'V':
				if ((!(strcmp("VETERINARY",script1)))||(!(strcmp("SKILL39",script1))))
					chars[s].baseskill[VETERINARY] = getstatskillvalue(script2);
				break;
			
			case 'w':
			case 'W':
				if ((!(strcmp("WRESTLING",script1)))||(!(strcmp("SKILL43",script1))))
					chars[s].baseskill[WRESTLING] = getstatskillvalue(script2);
				break;

			default:
				break;
			}
		}
	} while (script1[0]!='}');
 closescript();
 
 //Now find real 'skill' based on 'baseskill' (stat modifiers)
 for(z=0;z<TRUESKILLS;z++)
 {
	 Skills->updateSkillLevel(s,z);
 }
 if (donpcupdate==0)
 {
	 //printf("1 "); /*DEBUG*/
	 updatechar(s);
	 //printf("2 "); /*DEBUG*/
	 staticeffect(s, 0x37, 0x3A, 0, 15);
	 //printf("3 "); /*DEBUG*/
	 soundeffect2(s, 0x01, 0xE9);
 }
}

void Karma(int nCharID,int nKilledID, int nKarma)
{                                                                                               // nEffect = 1 positive karma effect
	int nCurKarma=0, nChange=0, nEffect=0;
	
	nCurKarma = chars[nCharID].karma;
	if((nCurKarma>10000)||(nCurKarma<-10000))
		if(nCurKarma>10000)
			chars[nCharID].karma=10000;
		else
			chars[nCharID].karma=-10000;
		if(nCurKarma<nKarma && nKarma>0)
		{
			nChange=((nKarma-nCurKarma)/75);
			chars[nCharID].karma=(nCurKarma+nChange);
			nEffect=1;
		}
		if((nCurKarma>nKarma)&&(chars[nKilledID].karma>0)||
			(nCurKarma>nKarma)&&(nKilledID==-1))
		{
			nChange=((nCurKarma-nKarma)/50);
			chars[nCharID].karma=(nCurKarma-nChange);
			nEffect=0;
		}
		if((nChange==0)||(chars[nCharID].npc==1))
			return;
		if(nChange<=25)
			if(nEffect)
			{
				sysmessage(calcSocketFromChar(nCharID),
					"You have gained a little karma.");
				return;
			}
			else
			{
				sysmessage(calcSocketFromChar(nCharID),
					"You have lost a little karma.");
				return;
			}
			if(nChange<=50)
				if(nEffect)
				{
					sysmessage(calcSocketFromChar(nCharID),
						"You have gained some karma.");
					return;
				}
				else
				{
					sysmessage(calcSocketFromChar(nCharID),
						"You have lost some karma.");
					return;
				}
				if((nChange<=100)||(nChange>100))
					if(nEffect)
					{
						sysmessage(calcSocketFromChar(nCharID),
							"You have gained alot of karma.");
						return;
					}
					else
					{
						sysmessage(calcSocketFromChar(nCharID),
							"You have lost alot of karma.");
						return;
					}
}
//added by Genesis 11-8-98
void Fame(int nCharID, int nFame)   
{
	int nCurFame, nChange=0, nEffect=0;
	
	nCurFame  = chars[nCharID].fame;
	if(nCurFame>nFame) // if player fame greater abort function
	{
		if(nCurFame>10000)
			chars[nCharID].fame=10000;
		return;
	}
	if(nCurFame<nFame)
	{
		nChange=(nFame-nCurFame)/75;
		chars[nCharID].fame=(nCurFame+nChange);
		nEffect=1;
	}
	if(chars[nCharID].dead==1)
	{
		if(nCurFame<=0)
			chars[nCharID].fame=0;
		else
		{
			nChange=(nCurFame-0)/25;
			chars[nCharID].fame=(nCurFame-nChange);
		}
		chars[nCharID].deaths++;
		nEffect=0;
	}
	if((nChange==0)||(chars[nCharID].npc==1))
		return;
	if(nChange<=25)
		if(nEffect)
		{
			sysmessage(calcSocketFromChar(nCharID),
				"You have gained a little fame.");
			return;
		}
		else
		{
			sysmessage(calcSocketFromChar(nCharID),
				"You have lost a little fame.");
			return;
		}
		if(nChange<=50)
			if(nEffect)
			{
				sysmessage(calcSocketFromChar(nCharID),
					"You have gained some fame.");
				return;
			}
			else
			{
				sysmessage(calcSocketFromChar(nCharID),
					"You have lost some fame.");
				return;
			}
			if((nChange<=100)||(nChange>100))
				if(nEffect)
				{
					sysmessage(calcSocketFromChar(nCharID),
						"You have gained alot of fame.");
					return;
				}
				else
				{
					sysmessage(calcSocketFromChar(nCharID),
						"You have lost alot of fame.");
					return;
				}
}

void enlist(int s, int listnum) // listnum is stored in items morex
{
	int x,pos/*,i*/,j;
	char sect[50];
	
	openscript("items.scp");
	sprintf(sect, "ITEMLIST %i", listnum);
	if (!i_scripts[items_script]->find(sect))
	{
		closescript();
		if (n_scripts[custom_item_script][0]!=0)
		{
			openscript(n_scripts[custom_item_script]);
			if (!i_scripts[custom_item_script]->find(sect))
			{
				printf("UOX3: ITEMLIST not found, aborting.\n");
				closescript();
				return;
			} else strcpy(sect, n_scripts[custom_item_script]);
		} else {
			printf("UOX3: ITEMLIST not found, aborting.\n");
			return;
		}
	} else strcpy(sect, "items.scp");
	
	do
	{
		read2();
		if (script1[0]!='}')
		{
			x=str2num(script1);
			pos=ftell(scpfile);
			closescript();
			j=Items->SpawnItemBackpack2(s, currchar[s], x, 0);
			openscript(sect);
			fseek(scpfile, pos, SEEK_SET);
			strcpy(script1, "DUMMY");
			//			for (i=0;i<now;i++) if (perm[i]) senditem(i,j);
			RefreshItem( j ); // AntiChrist
		}
	} 
	while(strcmp(script1,"}"));
	closescript();
}


//o
//o---------------------------------------------------------------------------o
//|	Class		:	void checkdumpdata(unsigned int currenttime)
//|	Date		:	1-5-99
//|	Programmer	:	Ridcully
//o---------------------------------------------------------------------------o
//| Purpose		:Used for Creating file read by UOXBot
//o---------------------------------------------------------------------------o
void checkdumpdata(unsigned int currenttime) // This dumps data for Ridcully's UOXBot 0.02 (jluebbe@hannover.aball.de)
{
	static unsigned int lastdump;
	if ((currenttime - lastdump)<30000)
	{
		return;
	}
	FILE *datafile;
	unsigned int i;
	
	//printf("UOX3: Dumping data!\n");
	
	datafile=fopen("botdata.txt","w");
	if (datafile==NULL)
	{
		printf("ERROR: botdata.txt could not be created.\n");
		return;
	}
	
	fprintf(datafile, "ONLINEPLAYERS = (");
	int firstplayer = 1;
	for (i=0;i<now;i++)
	{
		if(perm[i]) //Keeps NPC's from appearing on the list
		{
			if(firstplayer)
			{
				fprintf(datafile, "'%s'", chars[currchar[i]].name);
			}
			else
			{
				fprintf(datafile, ", '%s'", chars[currchar[i]].name);
			}
			firstplayer = 0;
		}
	}
	if(firstplayer)
	{
		fprintf(datafile, ")\n");
	}
	else
	{
		fprintf(datafile, ",)\n");
	}
	
	fprintf(datafile, "UOSERVER = '%s:%i'\n", serv[0][1], UOX_PORT);
	
	fprintf(datafile, "UOSERVERUP = 1\n");
	
	fclose(datafile);
	
	lastdump = currenttime;
}


char line_of_sight(int s, short int x1, short int y1, int z1, short int x2, short int y2, int z2, int checkfor)
{
/*
Char (x1, y1, z1) is the char(pc/npc),  Target (x2, y2, z2) is the target.
s is for pc's, in case a message needs to be sent.
the checkfor is what is checked for along the line of sight.  
Look at uox3.h to see options. Works like npc magic.

  #define TREES_BUSHES 1 // Trees and other large vegetaion in the way
  #define WALLS_CHIMNEYS 2  // Walls, chimineys, ovens, etc... in the way
  #define DOORS 4 // Doors in the way
  #define ROOFING_SLANTED 8  // So can't tele onto slanted roofs, basically
  #define FLOORS_FLAT_ROOFING 16  //  For attacking between floors
  #define LAVA_WATER 32  // Don't know what all to use this for yet
  
	Just or (|) the values for the diff things together to get what to search for.
	So put in place of the paramater checkfor for example
	
	  if (line_of_sight(s, x1, y1, z1, x2, y2, z2, WALLS_CHIMNEYS | DOORS | ROOFING_SLANTED))
	  
		
		  This whole thing is based on the Pythagorean Theorem.  What I have done is
		  taken the coordinates from both chars and created a right triange with the
		  hypotenuse as the line of sight.  Then by sectioning off side "a" into a number
		  of equal lengths and finding the other sides lengths according to that one,  I 
		  have been able to find the coordinates of the tiles that lie along the line of
		  sight(line "c").  Then these tiles are searched from an item that would block 
		  the line of sight.
	*/
	
	int a, b;     //  Lengths of sides a & b
	int zcheck=z1, prexcheck=-128, preycheck=-128, prezcheck=-128;
	short int xcheck = x1, ycheck = y1;
	double c;      //  Length of side c,  Line of sight
	int asquared, bsquared, csquared;  // Squares of a, b, c
	double a_divide;      //  this is how many times the entire line is checked 
	double a1_incrament= .1;  // the line is checked by this length incrament, can be chenged, the larger the num less checks, more misses
	double a2_incrament= a1_incrament;
	double c1_incrament, c2_incrament;  //  c/a_divide,  this give the length incrament along c
	double b1_incrament, b2_incrament, aplus=0, bplus=0, b2plus=0;  //  the length of b1 at a1,c1
	int checkcount, dyncount;
	char blocked=0;					// could potentially replace returning a char with a bool... you either see it or you don't (Abaddon)
	char not_blocked=1;
	double a2, b2, c2;
	UOXFile *mfile;
	st_multi multi;
	map_st map1;
	SI32 length, i,j;
	/////what is checked for
	int checkitemcount=0;
	int checkthis[ITEM_TYPE_CHOICES];
	int checkthistotal;
	int itemtype;
	/////item cahcing until item lookup is implimented
	int loscachecount=0;
	if( ( x1 <= 200 && y1<= 200 ) || ( x2 <= 200 && y2 <= 200 ) ) 
		return not_blocked;
	if( ( abs( x1 - x2 ) > 18 ) || ( abs( y1 - y2 ) > 18 ) ) 
		return blocked;
	///////////////////////////////////////////////////////////
	/////////////////  These next lines initialize arrays
	/*
	This function has to search the items array a number of times which
	caused a bit of lag.  I made this item cache to be used instead.  
	The items array is only search once for items in a 40 tile area.
	if an item is found, it is put into the cache, which is then used
	by the rest of the function.  This way it doesn't have to check the 
	entire array each time.
	*/
	// - Tauriel's region stuff 3/6/99
	int StartGrid=mapRegions->StartGrid(x1,y1);
	
	// - This needs to use a pointer/array to cut down on walking lag...
	unsigned int increment=0;
	for (unsigned int checkgrid=StartGrid+(increment*mapRegions->GetColSize());increment<3;increment++, checkgrid=StartGrid+(increment*mapRegions->GetColSize()))
		for (i=0;i<3;i++)
		{
			int mapitem=-1;
			int mapitemptr=-1;
			do //check all items in this cell
			{
				mapitemptr=mapRegions->GetNextItem(checkgrid+i, mapitemptr);
				if (mapitemptr==-1) break;
				mapitem=mapRegions->GetItem(checkgrid+i, mapitemptr);
				if (mapitem!=-1 && mapitem<1000000)
				{
					if (
						(items[mapitem].x<= x1 +20)&& 
						(items[mapitem].x>= x1 -20)&&
						(items[mapitem].y<= y1 +20)&&
						(items[mapitem].y>= y1 -20)
						)
					{
						loscache[loscachecount]=mapitem;
						loscachecount++;
					}
					//					}
				}
			} while (mapitemptr!=-1);
		}
		for ( i = 0; i < ITEM_TYPE_CHOICES; i++ ) 
			checkthis[i] = 0 ; //Null is for pointer types
		// Zippy moved ----^ to main for initalization.
		
		
		////////////End Initilzations
		//////////////////////////////////////////////////////////////
		/*
		char(player/npc)
		1 2  *_____a_____
		1      \          |
		2        \        |
		c      b   x -->
		\    |   y  ^  
		\  |      |
		\|
		*
		target
		*/

		// Can't occur, outside scope of variable size
//		if( x2 == 65535 && y2 == 65535 )  
//			return not_blocked;  // target cancled
		
		
		
		a=abs(x1-x2)+1;  // length of side a
		b=abs(y1-y2)+1;  // length of side b 
		asquared=(a * a);
		bsquared=(b * b);
		csquared=(asquared + bsquared);
		c=sqrt(csquared);// length of c(hypotenuse==line of sight)
		
		a_divide = (a / a1_incrament);
		c1_incrament = (c / a_divide);
		
		if(a==1)
		{
			c1_incrament=1;
			a1_incrament=0;
			a_divide = b;  
		}
		
		b1_incrament=sqrt((c1_incrament*c1_incrament)-(a1_incrament*a1_incrament));
		if (b==1)
		{
			a1_incrament=1;
			a_divide=a;
			b1_incrament=0;
		}
		//////////////////////////////////////////////////////////
		// X position
		if (x2==x1) // Target has same x value
		{
			a1_incrament=(a1_incrament * 0); // sets x incraments to zero,  no incrament
		}
		if (x2>x1) // Target has greater x value
		{
			a1_incrament=(a1_incrament * 1); // sets x incrament positive
		}
		if (x2<x1) // Target has lesser x value
		{
			a1_incrament=(a1_incrament * -1); // sets x incrament negative, postitive initially
		}
		
		// Y position
		if (y2==y1) // Target has same y value
		{
			b1_incrament=(b1_incrament * 0); //  sets y incrament to zero, no incrament
		}
		if (y2>y1) // Target has greater y value
		{
			b1_incrament=(b1_incrament * 1); // sets y incraments positive
		}
		if (y2<y1) // Target has lesser y value
		{
			b1_incrament=(b1_incrament * -1);  // sets y incrament negative, it's positive initially
		}
		//////////////////////////////////////////////////////////////////////////////
		/*
		To find the tiles along the z axis another right triangle is formed.  This triangle
		is formed along the line of sight between Char1 & Target, where the length of that line
		is the length of the base of the triange.  The hiegth of the triangle is the targets z 
		position.  We now have two sides, and to find the third we use the pathagorean theorem 
		again, and that gives us the true line of sight in 3 dimentions: X, Y, Z.
		
		*/
		a2=c;  // length of base
		b2=abs(z2-z1)+1;  // hieght of side 
		asquared=int(a2 * a2);
		bsquared=int(b2 * b2);
		csquared=(asquared + bsquared);
		c2=sqrt(csquared); //  length of true line of sight 3D
		
		a_divide = (a2 / a2_incrament);
		c2_incrament = (c2 / a_divide);
		b2_incrament=sqrt((c2_incrament*c2_incrament)-(a2_incrament*a2_incrament));  
		
		if (z1>z2) // going down
		{
			b2_incrament = (b2_incrament * -1);
		}
		if (z1==z2) // level ground
		{
			b2_incrament = 0;
		}
		
		aplus=a1_incrament;
		bplus=b1_incrament;
		b2plus=b2_incrament; // going up or down
		////////////////////////////////////////////////////////
		////////////////  This determines what to check for
		i=0;
		itemtype=1;
		checkthistotal=0;
		while (checkfor)
		{
			if ((checkfor>=itemtype)&&(checkfor<(itemtype * 2))&&(checkfor))
			{
				checkthis[i]=itemtype;
				++i;
				checkfor = (checkfor - itemtype);
				++checkthistotal;
				itemtype=1;
			}
			else
				if (checkfor)
				{
					itemtype=(itemtype * 2);
				}
		}    
		///////////////////////////////////////////////////////////////////////////
		////////////////////  This next stuff is what searches each tile for things
		for (checkcount=1; checkcount<=a_divide; checkcount++)
		{
			if (xcheck!=x2) xcheck=int(x1 + aplus); // x coord to check
			if (ycheck!=y2) ycheck=int(y1 + bplus); // y coord to check
			if (zcheck!=z2) zcheck=int(z1 + b2plus); // z coord to check
			if ((xcheck!=prexcheck)||(ycheck!=preycheck)||(zcheck!=prezcheck))
			{ 
				// Texture mapping  
				//x1check=xcheck/8; // Block
				//y1check=ycheck/8;
				//xoff=(xcheck-(x1check*8)); // Offset
				//yoff=(ycheck-(y1check*8));
				map1 = Map->SeekMap0( xcheck, ycheck );
				if (map1.id!=2) 
				{
					if ( // Mountain walls
						((map1.id>=431)&&(map1.id<=432))||
						((map1.id>=467)&&(map1.id<=475))||
						((map1.id>=543)&&(map1.id<=560))||
						((map1.id>=1754)&&(map1.id<=1757))||
						((map1.id>=1787)&&(map1.id<=1789))||
						((map1.id>=1821)&&(map1.id<=1824))||
						((map1.id>=1851)&&(map1.id<=1854))||
						((map1.id>=1881)&&(map1.id<=1884))
						)
					{
						sysmessage(s, "There seems to be something in the way!");
						return blocked;
					}
				}	 
				
				// Statics
				MapStaticIterator msi(xcheck, ycheck);
				tile_st tile;
				staticrecord *stat;
				while (stat = msi.Next())
				{
					msi.GetTile(&tile);
					if (	(zcheck>=stat->zoff)&&
						(zcheck<=(stat->zoff+tile.height)))
					{	  
						itemids[checkitemcount]=stat->itemid;
						checkitemcount++;	  
					}// if
				}
				
				
				// Items
				for (i=0;i<loscachecount;i++)
				{
					dyncount=loscache[i];
					if (items[dyncount].id1<0x40)
					{ // Dynamic items
						Map->SeekTile((items[dyncount].id1<<8)+items[dyncount].id2, &tile);
						if ((items[dyncount].x==xcheck)&&
							(items[dyncount].y==ycheck)&&
							(zcheck>=items[dyncount].z)&&
							(zcheck<=(items[dyncount].z+tile.height))&&
							(items[dyncount].visible==0))
						{	 
							itemids[checkitemcount]=((items[dyncount].id1 * 256) + items[dyncount].id2);
							checkitemcount++;
						}
					}
					else
					{// Multi's
						if ((abs(x1-x2)<=BUILDRANGE)&&(abs(y1-y2)<=BUILDRANGE))
						{
							Map->SeekMulti(((items[dyncount].id1<<8)+items[dyncount].id2)-0x4000, &mfile, &length);
							length=length/sizeof(st_multi);
							if (length == -1 || length>=17000000)//Too big... bug fix hopefully (Abaddon 13 Sept 1999)
								//							if (length == -1)
							{
								printf("LoS - Bad length in multi file. (Item serial %i) Avoiding stall.\n", items[dyncount].serial );
								length = 0;
							}
							for (j=0;j<length;j++)
							{
								mfile->get_st_multi(&multi);
								if ((multi.visible)&&
									(items[dyncount].x+multi.x == xcheck)&&
									(items[dyncount].y+multi.y == ycheck))			
								{
									//pos=mfile->tell();
									Map->SeekTile(multi.tile, &tile);
									//mfile->seek(pos, SEEK_SET);
									if ((zcheck>=items[dyncount].z+multi.z)&&
										(zcheck<=(items[dyncount].z+multi.z + tile.height)))
									{
										itemids[checkitemcount]=multi.tile;
										checkitemcount++;
									}
								}
							}
						}
					}// end else
				} // for
				if ((xcheck==x2)&&(ycheck==y2)&&(zcheck==z2)) checkcount=int(a_divide+1);
				prexcheck=xcheck;
				preycheck=ycheck;
				prezcheck=zcheck;
			} // if statment
			if (xcheck!=x2) aplus=aplus+a1_incrament;
			if (ycheck!=y2) bplus=bplus+b1_incrament;
			if (zcheck!=z2) b2plus=b2plus+b2_incrament;
 } // for loop
 
 for (i=0;i<checkitemcount;i++)
 {
	 for (j=0;j<checkthistotal;j++)
	 {
		 switch(checkthis[j])
		 {
		 case 1 : // Trees, Shrubs, bushes
			 if ((itemids[i]==3240)||(itemids[i]==3242)||((itemids[i]>=3215)&&(itemids[i]<=3218))||
				 ((itemids[i]>=3272)&&(itemids[i]<=3280))||(itemids[i]==3283)||(itemids[i]==3286)||
				 (itemids[i]==3288)||(itemids[i]==3290)||(itemids[i]==3293)||(itemids[i]==3296)||
				 (itemids[i]==3299)||(itemids[i]==3302)||(itemids[i]==3305)||(itemids[i]==3306)||
				 (itemids[i]==3320)||(itemids[i]==3323)||(itemids[i]==3326)||(itemids[i]==3329)||
				 (itemids[i]==3381)||(itemids[i]==3383)||(itemids[i]==3384)||(itemids[i]==3394)||
				 (itemids[i]==3395)||((itemids[i]>=3416)&&(itemids[i]<=3418))||
				 (itemids[i]==3440)||(itemids[i]==3461)||(itemids[i]==3476)||(itemids[i]==3480)||
				 (itemids[i]==3484)||(itemids[i]==3488)||(itemids[i]==3492)||(itemids[i]==3496)||
				 (itemids[i]==3512)||(itemids[i]==3513)||((itemids[i]>=4792)&&(itemids[i]<=4795)))
			 {
				 //			  sprintf(temp, "You can't see the forest for the trees!");
				 //			  sysmessage(s, temp);
				 return blocked;
			 }
			 break;
		 case 2 : // Walls, Chimneys, ovens, not fences
			 if (((itemids[i]>=6)&&(itemids[i]<=748))||((itemids[i]>=761)&&(itemids[i]<=881))||
				 ((itemids[i]>=895)&&(itemids[i]<=1006))||((itemids[i]>=1057)&&(itemids[i]<=1061))||
				 (itemids[i]==1072)||(itemids[i]==1073)||((itemids[i]>=1080)&&(itemids[i]<=1166))||
				 ((itemids[i]>=2347)&&(itemids[i]<=2412))||((itemids[i]>=16114)&&(itemids[i]<=16134))||
				 ((itemids[i]>=8538)&&(itemids[i]<=8553))||((itemids[i]>=9535)&&(itemids[i]<=9555))||
				 (itemids[i]==12583))
			 {
				 //			  sprintf(temp, "There seems to be some sort of wall in the way!");
				 //			  sysmessage(s, temp);
				 return blocked;
			 }
			 break;
		 case 4 : // Doors, not gates
			 if (((itemids[i]>=1653)&&(itemids[i]<=1782))||((itemids[i]>=8173)&&(itemids[i]<=8188)))
			 {
				 //			  sprintf(temp, "Only ghosts do things through doors!");
				 //			  sysmessage(s, temp);
				 return blocked;
			 }
			 break;
		 case 8 : // Roofing Slanted
			 if (((itemids[i]>=1414)&&(itemids[i]<=1578))||((itemids[i]>=1587)&&(itemids[i]<=1590))||
				 ((itemids[i]>=1608)&&(itemids[i]<=1617))||((itemids[i]>=1630)&&(itemids[i]<=1652))||
				 ((itemids[i]>=1789)&&(itemids[i]<=1792)))
			 {
				 //			  sprintf(temp, "The roof is too steep!");
				 //			  sysmessage(s, temp);
				 return blocked;
			 }
			 break;
		 case 16 : // Floors & Flat Roofing (Attacking through floors Roofs)
			 if (((itemids[i]>=1169)&&(itemids[i]<=1413))||((itemids[i]>=1508)&&(itemids[i]<=1514))||
				 ((itemids[i]>=1579)&&(itemids[i]<=1586))||((itemids[i]>=1591)&&(itemids[i]<=1598)))
			 {
				 if (z1==z2) // in case of char and target on same roof
				 {
					 return not_blocked;
				 }
				 else
				 {
					 //               sprintf(temp, "You would love to do that, but the is a floor in the way!");
					 //               sysmessage(s, temp);
					 return blocked;
				 }
			 }
			 break;
		 case 32 :  // Lava, water
			 if (((itemids[i]>=4846)&&(itemids[i]<=4941))||((itemids[i]>=6038)&&(itemids[i]<=6066))||
				 ((itemids[i]>=12934)&&(itemids[i]<=12977))||((itemids[i]>=13371)&&(itemids[i]<=13420))||
				 ((itemids[i]>=13422)&&(itemids[i]<=13638))||((itemids[i]>=13639)&&(itemids[i]<=13665)))
			 {
				 //			   sprintf(temp, "Yah, you wish!");
				 //			   sysmessage(s, temp);
				 return blocked;
			 }
			 break;
		 default:
			 //printf("ERROR: Fallout of switch statement without default. uox3.cpp, line_of_sight()"); //Morrolan
			 return not_blocked;
		 } // switch
	 } //for
 } //for
 return not_blocked;
} //function

int addrandomcolor(int s, char *colorlist)
{
	char sect[512];
	int i = 0, j = 0, storeval = 0;
	openscript("colors.scp");
	sprintf(sect, "RANDOMCOLOR %s", colorlist);
	if (!i_scripts[colors_script]->find(sect)) 
	{
		closescript();
		sprintf(chars[s].name, "Error Colorlist %s Not Found(1)", colorlist);
		return 0;
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
		i=rand()%i;
		i++;
		openscript("colors.scp");
		if(!i_scripts[colors_script]->find(sect))
		{
			closescript();
			sprintf(chars[s].name, "Error Colorlist %s Not Found(2)", colorlist);
			return 0;
		}
		do
		{
			read1();
			if (script1[0]!='}')
			{
				j++;
				if(j==i)
				{
					storeval=hstr2num(script1);
				}
			}
		}
		while (script1[0]!='}');
		closescript();
	}
	return (storeval);
}

int addrandomhaircolor(int s, char *colorlist)
{
	char sect[512];
	int i = 0, j = 0, haircolor = 0;

	openscript("colors.scp");
	sprintf(sect, "RANDOMCOLOR %s", colorlist);
	if (!i_scripts[colors_script]->find(sect)) 
	{
		closescript();
		sprintf(chars[s].name, "Error Colorlist %s Not Found(1)", colorlist);
		return 0;
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
		i=rand()%i;
		i++;
		openscript("colors.scp");
		if(!i_scripts[colors_script]->find(sect))
		{
			closescript();
			sprintf(chars[s].name, "Error Colorlist %s Not Found(2)", colorlist);
			return 0;
		}
		do
		{
			read1();
			if (script1[0]!='}')
			{
				j++;
				if(j==i)
				{
					haircolor=hstr2num(script1);
				}
			}
		}
		while (script1[0]!='}');
		closescript();
	}
	return (haircolor);
}

void criminal( CHARACTER c ) //Repsys
{
	if( !(chars[c].flag&0x02) )	// if we're not a criminal already
	{
		chars[c].crimflag = (int)((repsys.crimtime*CLOCKS_PER_SEC) + uiCurrentTime);
		sysmessage( calcSocketFromChar( c ), "You are now a criminal!" );
		setcharflag( c );
		if( (region[chars[c].region].priv&1) && server_data.guardsactive )//guarded
			Combat->SpawnGuard( c, c, chars[c].x, chars[c].y, chars[c].z );
	}
	else	// they're already a criminal, and have done something MORE criminal
	{	// let's update their flag, as another criminal act will reset the timer
		chars[c].crimflag = (int)((repsys.crimtime * CLOCKS_PER_SEC ) + uiCurrentTime);
		// check to see if there is a guard nearby, otherwise spawn us a new one
		if( !(region[chars[c].region].priv&1) || !server_data.guardsactive )
			return;
		unsigned short x = chars[c].x;
		unsigned short y = chars[c].y;
		int StartGrid = mapRegions->StartGrid( x, y );
		
		unsigned int increment = 0;
		for( unsigned int checkgrid = StartGrid + ( increment * mapRegions->GetColSize() ); increment < 3; increment++, checkgrid = StartGrid + ( increment * mapRegions->GetColSize() ) )
		{
			for( int i = 0; i < 3; i++ )
			{
				int mapitem = -1;
				int mapitemptr = -1;
				do //check all items in this cell
				{
					mapitemptr = mapRegions->GetNextItem( checkgrid + i, mapitemptr );
					if( mapitemptr == -1 ) break;
					mapitem = mapRegions->GetItem( checkgrid + i, mapitemptr );
					int mapchar = mapitem - 1000000;
					if( mapitem != -1 && mapitem >= 1000000 )
					{
						if( mapchar > -1 && mapchar < cmem )
						{
							if( chars[mapchar].npcaitype == 0x04 )	// we have a guard!
							{
								if( chardist( mapchar, c ) < 18 )
								{
									npcattacktarget( c, mapchar );
									return;
								}
							}
						}
					}
				} while ( mapitemptr != -1 );
			}
		}
		Combat->SpawnGuard( c, c, chars[c].x, chars[c].y, chars[c].z );
	}
}

void setcharflag(int c)//repsys
{
	if( !chars[c].npc )
	{
		if( chars[c].kills > repsys.maxkills ) 
		{
			chars[c].flag |= 0x01;
			chars[c].flag &= 0x1B;
		}
		else if(chars[c].crimflag == -1 || chars[c].crimflag == 0 ) 
		{
			chars[c].flag |= 0x04;
			chars[c].flag &= 0x1C;
		}
		else if( chars[c].crimflag > 0 )
		{
			chars[c].flag |= 0x02;
			chars[c].flag &= 0x1B;
		}
		else 
		{
			chars[c].flag |= 0x04;
			chars[c].flag &= 0x1C;
		}
	}
	else 
	{
		switch( chars[c].npcaitype )
		{
		case 2://evil
		case 0x50://?
		case 666://evil healer
		case 0x58://bs/ev
			chars[c].flag |= 0x01;
			chars[c].flag &= 0x1B;
			break;
		case 1://good
		case 17://Player Vendor
		case 4://healer
		case 30://?
		case 40://?
		case 0x08://banker
			chars[c].flag |= 0x04;
			chars[c].flag &= 0x1C;
			break;
		default:
			unsigned short charID; charID = (chars[c].id1 << 8) + chars[c].id2;
			if( charID == 0x0190 || charID == 0x0191 )
			{
				chars[c].flag |= 0x04;
				chars[c].flag &= 0x1C;
				break;
			}
			else if( server_data.animals_guarded )
			{
				if( region[chars[c].region].priv&0x01 )	// in a guarded region, with guarded animals, animals == blue
					chars[c].flag = 0x04;
				else				// if the region's not guarded, they're gray
					chars[c].flag = 0x02;
			}
			else	// if it's not a human form, and animal's aren't guarded, then they're gray
			{
				chars[c].flag = 0x02;
			}
			if( chars[c].ownserial != 0xFFFFFFFF && chars[c].tamed )
			{
				int i = calcCharFromSer( chars[c].ownserial );
				if( i != -1 )
					chars[c].flag = chars[i].flag;
				else
				{
					chars[c].flag |= 0x04;
					chars[c].flag &= 0x1C;
				}
				if( chars[c].flag == 0x04 && !server_data.animals_guarded )
				{
					chars[c].flag |= 0x04;
					chars[c].flag &= 0x1C;
				}
				break;
			}
			break;
		}
	}
}

void loadrepsys( void ) //Repsys
{
	do
	{
		readw2();
		if(!(strcmp(script1,"MURDER_DECAY"))) repsys.murderdecay=str2num(script2);
		if(!(strcmp(script1,"MAXKILLS"))) repsys.maxkills=str2num(script2);
		if(!(strcmp(script1,"CRIMINAL_TIME"))) repsys.crimtime=str2num(script2);
		if( !strcmp( script1, "SNOOP_IS_CRIME" ) )
			server_data.snoopiscrime = ( str2num( script2 ) != 0 );
	}
	while (strcmp(script1, "}"));
	
	if (!repsys.murderdecay) repsys.murderdecay=420;
	if (!repsys.maxkills) repsys.maxkills=4;
	if (!repsys.crimtime) repsys.crimtime=120;
}

void loadresources()
{
	do
	{
		readw2();
		if(!(strcmp(script1, "MINECHECK" ))) server_data.minecheck = str2num( script2 ); // Moved by Magius(CHE)
		else if(!(strcmp(script1,"LOGS_PER_AREA"))) resource.logs=str2num(script2);
		else if(!(strcmp(script1,"LOG_RESPAWN_TIME"))) resource.logtime=str2num(script2);
		else if(!(strcmp(script1,"LOG_RESPAWN_AREA"))) resource.logarea=str2num(script2);
		else if(!(strcmp(script1,"ORE_PER_AREA"))) resource.ore=str2num(script2);
		else if(!(strcmp(script1,"ORE_RESPAWN_TIME"))) resource.oretime=str2num(script2);
		else if(!(strcmp(script1,"ORE_RESPAWN_AREA"))) resource.logarea=str2num(script2);
	}
	while (strcmp(script1, "}"));
}


void reverse_effect(int i)  // i = teffect[i] from checktempeffects()  // Morrolan bugfix 
{
	int s;
	teffect_st *Effect;
	Effect = Effects->GrabSpecific( i );
	s = calcCharFromSer( Effect->dest1, Effect->dest2, Effect->dest3, Effect->dest4 );
	switch( Effect->num )
	{
	case 1:
		chars[s].priv2=chars[s].priv2&0xFD;
		break;
	case 2:
		chars[s].fixedlight=255;
		break;
	case 3:
		chars[s].dx=chars[s].dx + Effect->more1;
		break;
	case 4:
		chars[s].in=chars[s].in + Effect->more1;
		break;
	case 5:
		chars[s].st=chars[s].st + Effect->more1;
		break;
	case 6:
		chars[s].dx=chars[s].dx - Effect->more1;
		break;
	case 7:
		chars[s].in=chars[s].in - Effect->more1;
		break;
	case 8:
		chars[s].st=chars[s].st - Effect->more1;
		break;
	case 11:
		chars[s].st=chars[s].st - Effect->more1;
		chars[s].dx=chars[s].dx - Effect->more2;
		chars[s].in=chars[s].in - Effect->more3;
		break;
	case 12:
		chars[s].st=chars[s].st + Effect->more1;
		chars[s].dx=chars[s].dx + Effect->more2;
		chars[s].in=chars[s].in + Effect->more3;
		break;
	case 18: // Polymorph spell by AntiChrist
		chars[s].id1=chars[s].orgid1;
		chars[s].id2=chars[s].orgid2;
		teleport( s );
		break;
	case 19: // Incognito spell by AntiChrist
		printf("INCOGNITO SPELL REVERSED!!\n" );
		int serhash, ci, j, serial;
		
		// ------ SEX ------
		chars[s].id2 = chars[s].orgid2;
		// ------ NAME -----
		strcpy( chars[s].name, chars[s].orgname );
		serial = chars[s].serial;
		serhash = serial%HASHMAX;
		for( ci = 0; ci < contsp[serhash].max; ci++ )
		{
			j = contsp[serhash].pointer[ci];
			if( j != -1 )
			{
				// ------ HAIR -----
				if( items[j].layer == 0x0B )
				{ // change hair style/color
					printf("HAIR FOUND!!\n" );
					// stores old hair values
					items[j].color1 = chars[s].haircolor1;
					items[j].color2 = chars[s].haircolor2;
					items[j].id1 = chars[s].hairstyle1;
					items[j].id2 = chars[s].hairstyle2;
				}
				// -------- BEARD --------
				// only if a man :D
				if( chars[s].id2 == 0x90 )
					if( items[j].layer == 0x10 ) 
					{ // change beard style/color
						printf("BEARD FOUND!!\n" );
						// stores old beard values
						items[j].color1 = chars[s].beardcolor1;
						items[j].color2 = chars[s].beardcolor2;
						items[j].id1 = chars[s].beardstyle1;
						items[j].id2 = chars[s].beardstyle2;
					}
			}
		}
		// only refresh once
		wornitems( calcSocketFromChar( s ), s );
		teleport( s );			// avoid crashing was currchar[s] (Abaddon)
		impowncreate( calcSocketFromChar( s ), s, 0 );
		break;
	case 21:
		unsigned short toDrop;
		toDrop = Effect->more1;
		if( ( chars[s].baseskill[PARRYING] - toDrop ) < 0 )
			chars[s].baseskill[PARRYING] = 0;
		else
			chars[s].baseskill[PARRYING] -= toDrop;
		break;
	case 25:
		chars[s].usepotion = 0;
		break;
	default:
		printf("ERROR: Fallout of switch statement without default. uox3.cpp, reverse_effect()\n"); //Morrolan
		return;
	}//switch
	Effects->DelCurrent();
	
	Items->CheckEquipment( s ); // AntiChrist - checks equipments for stats requirements
}//reverse_effect


void playTileSound( UOXSOCKET s )
//PRE:   s is a valid socket
//POST:  proper walking sound is played, depending on tile walking on
//DEV:   Abaddon
//DATE:  October 5, 1999
{
	int x, y;
	tile_st tile;
	char search1[10];
	unsigned char sndid1 = 0, sndid2 = 0;
	unsigned char tileType = 0;							// tile type 0 normal
	//           1 water
	//           2 stone
	//           3 other
	//           4 wooden
	//           5 grass
	bool onHorse;
	
	if ( chars[currchar[s]].hidden ) //Don't play sounds for hidden people --Zippy
		return;
	
	if( chars[currchar[s]].onhorse )
		onHorse = true;
	else
		onHorse = false;
	
	if( chars[currchar[s]].step == 1 || chars[currchar[s]].step == 0 )	// if we play a sound
	{
		x = chars[currchar[s]].x;
		y = chars[currchar[s]].y;
		
		MapStaticIterator msi(x, y);
		staticrecord *stat = msi.Next();
		if (stat)
			msi.GetTile(&tile);
	}
	
	if (tile.flag1&0x80)
		tileType = 1;
	strcpy( search1, "wood" );
	if( strstr( (char *) tile.name, search1 ) )
		tileType = 4;
	strcpy( search1, "ston" );
	if( strstr( (char *) tile.name, search1 ) )
		tileType = 2;
	strcpy( search1, "gras" );
	if( strstr( (char *) tile.name, search1 ) )
		tileType = 5;
	
	switch( chars[currchar[s]].step )	// change step info
	{
	case 0:		chars[currchar[s]].step = 3;	// step 2
		switch( tileType )
		{
		case 0:
			if( onHorse )
			{
				sndid1 = 0x02;
				sndid2 = 0x4C;
				// horse normal tile step 2
			}
			else
			{
				sndid1 = 0x01; 
				sndid2 = 0x2B; 
			}
			break;
		case 1:	// water
			break;
		case 2: // stone
			sndid1 = 0x01;
			sndid2 = 0x30;
			break;
		case 3: // other
		case 4: // wooden
			sndid1 = 0x01;
			sndid2 = 0x23;
			break;
		case 5: // grass
			sndid1 = 0x01;
			sndid2 = 0x2D;
			break;
		}
		break;
		case 1:		chars[currchar[s]].step = 0;	// step 1
			switch( tileType )
			{
			case 0:
				if( onHorse )
				{
					sndid1 = 0x02;
					sndid2 = 0x4B;
				}
				else
				{
					sndid1 = 0x01; 
					sndid2 = 0x2C; 
				}
				break;
			case 1:	// water
				break;
			case 2: // stone
				sndid1 = 0x01;
				sndid2 = 0x2F;
				break;
			case 3: // other
			case 4: // wooden
				sndid1 = 0x01;
				sndid2 = 0x22;
				break;
			case 5: // grass
				sndid1 = 0x01;
				sndid2 = 0x2E;
				break;
			}
			break;
			case 2:
			case 3:		chars[currchar[s]].step = 1;	// pause
				break;
			default:	chars[currchar[s]].step = 1;	// pause
				break;
	}
	if( sndid1 != 0 && sndid2 != 0 )			// if we have a valid sound
		soundeffect( s, sndid1, sndid2 );
}
bool IsInMenuList( int toCheck )
// PRE:		TRUE
// POST:	returns true if item is in menu list, otherwise returns false on all other conditions
{
	int counter = 0;
	for( counter = 0; counter < menuList.size(); counter++ )
	{
		if( menuList[counter] == toCheck )
			return true;
	}
	return false;
}

void AddToMenuList( int toAdd )
// PRE:		TRUE
// POST:	Item is added to menu list
{
	int size = menuList.size();
	menuList.resize( size + 1 );
	menuList[size] = toAdd;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void deathstuff(int i)
//|   Date        -  UnKnown
//|   Programmer  -  UnKnown  (Touched tabstops by Tauriel Dec 28, 1998)
//o---------------------------------------------------------------------------o
//|   Purpose     -  Performs death stuff. I.E.- creates a corpse, moves items
//|                  to it, take out of war mode, does animation and sound, etc.
//o---------------------------------------------------------------------------o
void deathstuff(int i)
{
	int p, j, ele, corpsenum, c = -1;
	char murderername[50]; // AntiChrist
	
	if( chars[i].dead )	// DON'T KILL DEAD THINGS
		return;

	UOXSOCKET playerSock = calcSocketFromChar( i );
	int nType=0;

	if( chars[i].id1 != chars[i].orgid1 || chars[i].id2!= chars[i].orgid2 )
	{
		chars[i].id1=chars[i].orgid1;
		chars[i].id2=chars[i].orgid2;
		teleport( i );
	}
	
	chars[i].xid1 = chars[i].id1;
	chars[i].xid2 = chars[i].id2;
	chars[i].xskin1 = chars[i].skin1;
	chars[i].xskin2 = chars[i].skin2;
//	if( ( chars[i].id1 == 0x00 ) && ( ( chars[i].id2 == 0x0C ) || ( chars[i].id2 >= 0x3b ) && ( chars[i].id2 <= 0x3d ) ) )
//	{
//		nType=1;
//	}
	

	if( chars[i].attacker != -1 ) 
		strcpy( murderername, chars[chars[i].attacker].name ); 
	else 
		murderername[0] = 0;

	p = packitem(i);
	if( playerSock != -1 )
		j = unmounthorse( playerSock );
	KillTrades( i );

	ele = 0;
	
	if( !chars[i].npc ) 
		chars[i].id1 = 0x01; // Character is a ghost
	if( chars[i].xid2 == 0x91 )
	{
		chars[i].id2 = 0x93;  // Male or Female
	}
	else
	{
		chars[i].id2 = 0x92;
	}
	PlayDeathSound( i );
	chars[i].skin1=0x00; // Undyed
	chars[i].skin2=0x00;
	chars[i].dead=1;     // Dead
	chars[i].spellCast = 0;
	chars[i].casting = 0;
	chars[i].hp=0;       // With no hp left
	chars[i].poisoned=0;
	chars[i].poison=0;	//AntiChrist
	wtype = 0;
	weather( playerSock, 0 );
#ifdef DEBUG
	printf("%s killed by %s.\n",chars[i].name,chars[chars[i].attacker].name);
#endif

	corpsenum = GenerateCorpse( i, nType, murderername );
	if( corpsenum != -1 )
		ele = items[corpsenum].amount;
	if (chars[i].npc==0)
	{ 
		strcpy( temp, "a Death Shroud" );
		c = Items->SpawnItem( playerSock, 1, temp, 0, 0x20, 0x4E, 0, 0, 0, 0);
		if( c == -1 ) 
			return;
		chars[i].robe1 = items[c].ser1;
		chars[i].robe2 = items[c].ser2;
		chars[i].robe3 = items[c].ser3;
		chars[i].robe4 = items[c].ser4;
		setserial( c, i, 4 );
		items[c].layer = 0x16;
		items[c].def = 1;
	}
	if (server_data.showdeathanim) 
		deathaction( i, corpsenum );
	if( chars[i].account != -1 )
	{
		teleport(i);
		if ( playerSock != -1 ) 
			deathmenu( playerSock );
	}		
	
	if ((ele==13)||(ele==15)||(ele==16)||(ele==574))
	{
		strcpy(items[c].name,"a backpack");
		items[corpsenum].color1=0;
		items[corpsenum].color2=0;
		items[corpsenum].amount = 1;
		items[corpsenum].id1=0x09;
		items[corpsenum].id2=0xB2;
		items[corpsenum].corpse=0;
	}

	if( chars[i].summontimer )
	{
		staticeffect( i, 0x37, 0x2A, 0x09, 0x06 );
		soundeffect2( i, 0x01, 0xFE );
		Items->DeleItem( corpsenum );
		teleport( i );
		return;
	}
	RefreshItem( corpsenum ); // AntiChrist
	if( chars[i].npc ) 
		Npcs->DeleteChar(i);
	if( ele == 65535 )
		Items->DeleItem( corpsenum );
}



void NeutralizeEnemies( CHARACTER i )
{
	int t;
	for (t=0;t<charcount;t++)
	{
		if (chars[t].targ==i && !chars[t].free)
		{
			if (chars[t].npcaitype==4) //LB change from 0x40 to 4
			{
				chars[t].summontimer = (unsigned int)(uiCurrentTime+(CLOCKS_PER_SEC*20));    
				chars[t].npcWander=2;
				chars[t].npcmovetime=(unsigned int)((uiCurrentTime+double(NPCSPEED*CLOCKS_PER_SEC)));
				npctalkall(t,"Thou have suffered thy punishment, scoundrel.", 0);
			}
			chars[t].targ=-1;
			if (chars[t].attacker>-1 && chars[t].attacker < cmem )
			{
				chars[chars[t].attacker].attackfirst=0;
				chars[chars[t].attacker].attacker=-1;
			}
			chars[t].attacker=-1;
			chars[t].attackfirst=0;
			if( chars[t].npc && !chars[t].dead && chars[t].npcaitype != 17 && chars[t].war ) 
				npcToggleCombat( t ); // ripper
		}
	}
}

void KillTrades( CHARACTER i )
{
	int serial, serhash, ci, j;
	serial=chars[i].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		j=contsp[serhash].pointer[ci];
		if (j!=-1)
			if ((items[j].type==1) && (items[j].x==26) && (items[j].y==0) &&
				(items[j].z==0) && (items[j].id1==0x1E) && (items[j].id2==0x5E) &&
				(items[j].contserial==chars[i].serial))
			{
				endtrade(items[j].ser1, items[j].ser2, items[j].ser3, items[j].ser4);
			}
	}
}

void PlayDeathSound( CHARACTER i )
{
	if (chars[i].xid1==0x01 && chars[i].xid2==0x91)
	{
		int a = rand() % 4;
		switch( a ) // AntiChrist - uses all the sound effects
		{
		case 0:		soundeffect2( i, 0x01, 0x50 );	break;// Female Death
		case 1:		soundeffect2( i, 0x01, 0x51 );	break;// Female Death
		case 2:		soundeffect2( i, 0x01, 0x52 );	break;// Female Death
		case 3:		soundeffect2( i, 0x01, 0x53 );	break;// Female Death
		default:	soundeffect2( i, 0x01, 0x50 );	break;// Female Death
		}
	}
	if (chars[i].xid1==0x01 && chars[i].xid2==0x90)
	{
		int a = rand() % 4;
		switch( a ) // AntiChrist - uses all the sound effects
		{
		case 0:		soundeffect2( i, 0x01, 0x5A );	break;// Male Death
		case 1:		soundeffect2( i, 0x01, 0x5B );	break;// Male Death
		case 2:		soundeffect2( i, 0x01, 0x5C );	break;// Male Death
		case 3:		soundeffect2( i, 0x01, 0x5D );	break;// Male Death
		default:	soundeffect2( i, 0x01, 0x5A );	break;// Male Death
		}
	}
	else
	{
		playmonstersound(i, chars[i].xid1, chars[i].xid2, SND_DIE);
	}
}

int GenerateCorpse( CHARACTER i, int nType, char *murderername )
{
	int serial, serhash, serial1, serhash1, ci, ci1;
	UOXSOCKET playerSock = calcSocketFromChar( i );
	char temp[512];
	int c;
	int j, k;
	char clearmsg[8];
	// Make the corpse

	unsigned int l;
	int p = packitem( i );
	sprintf( temp, "corpse of %s", chars[i].name );
	
	c = Items->SpawnItem( playerSock, i, 1, temp, 0, 0x20, 0x06, chars[i].xskin1, chars[i].xskin2, 0, 0 );
	if( c == -1 ) 
		return -1;
	items[c].amount = ((chars[i].xid1)<<8)+chars[i].xid2; // Amount == corpse type
	items[c].morey = ishuman( i ); // is human?? - AntiChrist
	strcpy( items[c].name2, chars[i].name );
	items[c].magic = 2; // make the corpse unmovable - AntiChrist
	items[c].carve = chars[i].carve;
	mapRegions->RemoveItem( c );
	items[c].type = 1;
	items[c].x = chars[i].x;
	items[c].y = chars[i].y;
	items[c].z = chars[i].z;
	
	items[c].more1 = (unsigned char)nType;
	items[c].dir = chars[i].dir;
	items[c].corpse = 1;
	items[c].decaytime = (unsigned int)( uiCurrentTime + ( server_data.decaytimer * CLOCKS_PER_SEC ) );

	if( chars[i].npc != 1 )
	{
		items[c].ownserial = chars[i].serial;
		items[c].owner1 = (unsigned char)(chars[i].serial>>24);
		items[c].owner2 = (unsigned char)(chars[i].serial>>16);
		items[c].owner3 = (unsigned char)(chars[i].serial>>8);
		items[c].owner4 = (unsigned char)(chars[i].serial%256);
		items[c].more4 = char( server_data.playercorpsedecaymultiplier&0xff ); // how many times longer for the player's corpse to decay
	}
	//AntiChrist -- stores the time and the murderer's name
	strcpy( items[c].murderer, murderername );
	items[c].murdertime = uiCurrentTime;
	
	//AntiChrist -- fix
	
	mapRegions->AddItem( c );
	
	// Put objects on corpse
	serial = chars[i].serial;
	serhash = serial%HASHMAX;
	for( ci = 0; ci < contsp[serhash].max; ci++ )
	{
		j = contsp[serhash].pointer[ci];
		if( j != -1 )
		{
			if( items[j].contserial == serial )
			{
				if( items[j].st2 ) 
					chars[i].st -= items[j].st2;
				if( items[j].dx2 ) 
					chars[i].dx -= items[j].dx2;
				if( items[j].in2 ) 
					chars[i].in -= items[j].in2;
				if( items[j].layer != 0x0B && items[j].layer != 0x10 )
				{
					if( items[j].type == 1 && !( items[j].layer >= 0x1A && items[j].layer <= 0x1D) )
					{	// search backpack for spellbook && newbie items
						serial1 = items[j].serial;
						serhash1 = serial1%HASHMAX;
						for( ci1 = 0; ci1 < contsp[serhash1].max; ci1++ )
						{
							k = contsp[serhash1].pointer[ci1];
							if( k != -1 )
							{
								if( items[k].contserial == serial1 && !( items[k].priv&0x02 ) && items[k].type != 9 )
								{
									// I think the lack of this line below is the source of some of the vendor sell issues
									contsp[serhash1].pointer[ci1] = -1;	// it's no longer in this container, it's now on the corpse!
									setserial( k, c, 1 );
									items[k].x = (short int)(20 + ( rand()%50 ));
									items[k].y = (short int)(85 + ( rand()%75 ));
									items[k].z = 9;
									RefreshItem( k );
								}
							}
						}
					}
					else
					{
						if( items[j].layer == 0x1A )
						{
							clearmsg[0] = 0x3B;
							clearmsg[1] = 0x00;
							clearmsg[2] = 0x08;
							clearmsg[3] = chars[i].ser1;
							clearmsg[4] = chars[i].ser2;
							clearmsg[5] = chars[i].ser3;
							clearmsg[6] = chars[i].ser4;
							clearmsg[7] = 0x00;
							for( l = 0; l < now; l++ )
							{
								if( perm[l] && inrange1p( i, currchar[l] ) ) 
									Network->xSend( l, clearmsg, 8, 0 );
							}
						}
					}  // keep newbie stuff and bank with player
					if( !(items[j].priv&0x02) && items[j].layer != 0x1D )
					{
						if( j != p )
						{
							removefromptr( &contsp[serhash], j );
							setserial( j, c, 1 );
						}
					}
					else
					{
						if( p != -1 && items[j].layer != 0x1D )
						{
							if( j != p )
							{
								items[j].layer = 0;
								removefromptr( &contsp[serhash], j );
								setserial( j, p, 1 );
							}
						}
					}
					if( items[j].layer == 0x15 && !chars[i].shop ) 
						items[j].layer = 0x1A;
					items[j].x = 20 + ( rand()%50 );
					items[j].y = 85 + ( rand()%75 );
					items[j].z = 9;
					removeitem[1] = items[j].ser1;
					removeitem[2] = items[j].ser2;
					removeitem[3] = items[j].ser3;
					removeitem[4] = items[j].ser4;
					for( k = 0; k < now; k++ )
					{
						if( perm[k] ) 
							Network->xSend( k, removeitem, 5, 0 );
					}
					RefreshItem( j );
				}
				else
				{
					strcpy( items[j].name, "Hair/Beard" );
					items[j].x = 0x47;
					items[j].y = 0x93;
					items[j].z = 0;
				}
			}
		}
	} // end of !=-1
	return c;
}

void breakConcentration( CHARACTER p, UOXSOCKET s )
{
	if( chars[p].med )
	{
		chars[p].med = 0;
		if( s != -1 )
			sysmessage( s, "You break your concentration." );
	}
}

//o---------------------------------------------------------------------------o
//|     Class         :          int GetBankCount( CHARACTER p, unsigned short itemID, unsigned short colour = 0x0000 )
//|     Date          :          October 23rd, 2000
//|     Programmer    :          Abaddon
//o---------------------------------------------------------------------------o
//|     Purpose       :          Searches through the bank to count the amount
//|                              of items with a specific ID and colour
//|                              
//o---------------------------------------------------------------------------o
int GetBankCount( CHARACTER p, unsigned short itemID, unsigned short colour )
{
	if( p < 0 || p > cmem )
		return 0;
	unsigned char id1 = (char)(itemID>>8);
	unsigned char id2 = (char)(itemID%256);
	unsigned char cid1 = (char)(colour>>8);
	unsigned char cid2 = (char)(colour%256);
	SERIAL serial = chars[p].serial;
	int serhash = serial%HASHMAX;
	long int goldCount = 0;
	int counter2 = 0;
	int j, i, ci;
	for( ci = 0; ci < ownsp[serhash].max; ci++ )
	{
		j = ownsp[serhash].pointer[ci];
		if( j != -1 )
		{
			if( items[j].ownserial == serial && items[j].type == 1 && items[j].morex == 1 )
			{
				int newHash = ( items[j].serial%HASHMAX );
				for( counter2 = 0; counter2 < contsp[newHash].max; counter2++ )
				{
					i = contsp[newHash].pointer[counter2];
					if( i != -1 )
					{
						if( items[i].contserial == items[j].serial )
						{
							if( items[i].id1 == id1 && items[i].id2 == id2 )
							{
								if( items[i].color1 == cid1 && items[i].color2 == cid2 )
									goldCount += items[i].amount;
							}
						}
					}
				}
			}
		}
	}
	return goldCount;
}

//o---------------------------------------------------------------------------o
//|     Class         :         int DeleBankItem( CHARACTER p, unsigned short itemID, unsigned short colour, int amt )
//|     Date          :         October 23rd, 2000
//|     Programmer    :         Abaddon
//o---------------------------------------------------------------------------o
//|     Purpose       :         Searches through the bank to and deletes a 
//|                             certain amount of a certain item
//|								Returns how many left over
//o---------------------------------------------------------------------------o
int DeleBankItem( CHARACTER p, unsigned short itemID, unsigned short colour, int amt )
{
	if( p < 0 || p > cmem )
		return amt;
	unsigned char id1 = (char)(itemID>>8);
	unsigned char id2 = (char)(itemID%256);
	unsigned char cid1 = (char)(colour>>8);
	unsigned char cid2 = (char)(colour%256);
	SERIAL serial = chars[p].serial;
	int serhash = serial%HASHMAX;
	int counter2 = 0;
	int j, i;
	int total = amt;
	int ci;
	for( ci = 0; ci < ownsp[serhash].max && total > 0; ci++ )
	{
		j = ownsp[serhash].pointer[ci];
		if( j != -1 )
		{
			if( items[j].ownserial == serial && items[j].type == 1 && items[j].morex == 1 )
			{
				int newHash = ( items[j].serial%HASHMAX );
				for( counter2 = 0; counter2 < contsp[newHash].max && total > 0; counter2++ )
				{
					i = contsp[newHash].pointer[counter2];
					if( i != -1 )
					{
						if( items[i].contserial == items[j].serial )
						{
							if( items[i].id1 == id1 && items[i].id2 == id2 )
							{
								if( items[i].color1 == cid1 && items[i].color2 == cid2 )
								{
									if( total >= items[i].amount )
									{
										total -= items[i].amount;
										Items->DeleItem( i );
									}
									else
									{
										items[i].amount -= total;
										total = 0;
										RefreshItem( i );
									}

								}
							}
						}
					}
				}
			}
		}
	}
	return total;
}

void UseHairDye( UOXSOCKET s, short int colour, int x )	// s for socket, colour to dye, x is the item
{
	int hairobject = -1, beardobject = -1, dest;
	int ci, j;
	dest = currchar[s];
	for( ci = 0; ci < contsp[chars[dest].serial%HASHMAX].max; ci++ )
	{
		j = contsp[chars[dest].serial%HASHMAX].pointer[ci];
		if( j!= -1 )
		{
			if( items[j].layer == 0x10 )//beard
				beardobject = j;
			if( items[j].layer == 0x0B )//hairs
				hairobject = j;
		}
	}

	if( hairobject != -1 )
	{
		items[hairobject].color1 = (char)(colour>>8);
		items[hairobject].color2 = (char)(colour%256);
		RefreshItem( hairobject );
	}
	if( beardobject != -1 )
	{
		items[beardobject].color1 = (char)(colour>>8);
		items[beardobject].color2 = (char)(colour%256);
		RefreshItem( beardobject );
	}
	Items->DeleItem( x );
}
