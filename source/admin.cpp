//------------------------------------------------------------------------
//  admin.cpp
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

/* Admin code by Cironian */

#include "uox3.h"
#include "debug.h"

#define DBGFILE "admin.cpp"

//void account(FILE *infile)

void cAdmin::GumpAMenu(int s, int j)//Revana*
{
	char sect[512];
	short int length, length2, textlines;
	unsigned int i;
	int line;
	int account = chars[j].account;
	
	length=21;
	length2=1;
	line=0;
	do
	{
		line++;
		GumpALine(line);
		if (script1[0]!='}')
		{
			length+=strlen(script1)+4;
			length2+=strlen(script1)+4;
		}
	}
	while (script1[0]!='}');
	length+=3;
	textlines=0;
	line=0;
	do
	{
		line++;
		GumpAText(line, account);
		if (script1[0]!='}')
		{
			length+=(strlen(script1)*2)+2;
			textlines++;
		}
	}
	while (script1[0]!='}');
	gump1[1]=(unsigned char)(length>>8);
	gump1[2]=(unsigned char)(length%256);
	gump1[3]=chars[j].ser1;
	gump1[4]=chars[j].ser2;
	gump1[5]=chars[j].ser3;
	gump1[6]=chars[j].ser4;
	gump1[7]=0;
	gump1[8]=0;
	gump1[9]=0;
	gump1[10]=7; // Gump Number
	gump1[19]=(unsigned char)(length2>>8);
	gump1[20]=(unsigned char)(length2%256);
	Network->xSend(s, gump1, 21, 0);
	line=0;
	do
	{
		line++;
		GumpALine(line);
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
		GumpAText(line, account);
		if (script1[0]!='}')
		{
			gump3[0] = (char)strlen(script1)>>8;
			gump3[1] = (char)strlen(script1)%256;
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

void cAdmin::GumpAText(int line, int s)//Revana*
{
	int i = 0;
	
	line--; if (line==0) strcpy(script1,"Account Manager");
	line--; if (line==0) sprintf(script1,"Username = %s", acctx[s].name);

	line--; if (line==0) strcpy(script1,"Password = ****");
	line--; if (line==0) sprintf(script1,"Current Warning Level = %i",acctx[s].warning);

	if (acctx[s].wipe==1) i = 1;
	if (acctx[s].ban==1) i = 2;

	switch(i)
	{
	case 1:
		line--; if (line==0) strcpy(script1,"Account Status = Wiped");
		break;
	case 2:
		line--; if (line==0) strcpy(script1,"Account Status = Banned");
		break;
	case 0:
		line--; if (line==0) strcpy(script1,"Account Status = Active");
		break;
	}

	line--; if (line==0) strcpy(script1,"Ban Account");
	line--; if (line==0) strcpy(script1,"Time Ban Account");
	line--; if (line==0) strcpy(script1,"Wipe Account");
	line--; if (line==0) strcpy(script1,"Lock All Players On Account");
	line--; if (line==0) strcpy(script1,"Lock Character");
	line--; if (line==0) strcpy(script1,"Place Warning");
	line--; if (line==0) strcpy(script1,"Set Avaible Time");
	line--; if (line==0) strcpy(script1,"}");
}

void cAdmin::GumpALine(int line)//Revana*
{
	line--; if (line==0) strcpy(script1, "nomove");
	line--; if (line==0) strcpy(script1, "page 0");
	line--; if (line==0) strcpy(script1, "resizepic 0 0 5054 450 300");
	line--; if (line==0) strcpy(script1, "text 155 10 0 0");

	line--; if (line==0) strcpy(script1, "text 30 50 0 1");
	line--; if (line==0) strcpy(script1, "text 30 65 0 2");

	line--; if (line==0) strcpy(script1, "text 130 90 0 3");
	line--; if (line==0) strcpy(script1, "text 145 105 0 4");

	line--; if (line==0) strcpy(script1, "text 40 155 0 5");
	line--; if (line==0) strcpy(script1, "text 40 175 0 6");
	line--; if (line==0) strcpy(script1, "text 40 195 0 7");
	line--; if (line==0) strcpy(script1, "text 40 215 0 8");

	line--; if (line==0) strcpy(script1, "text 300 155 0 9");
	line--; if (line==0) strcpy(script1, "text 300 175 0 10");
	line--; if (line==0) strcpy(script1, "text 300 195 0 11");

	line--; if (line==0) strcpy(script1, "button 23 159 2117 2118 1 0 2");
	line--; if (line==0) strcpy(script1, "button 23 179 2117 2118 1 0 3");
	line--; if (line==0) strcpy(script1, "button 23 199 2117 2118 1 0 4");
	line--; if (line==0) strcpy(script1, "button 23 219 2117 2118 1 0 5");

	line--; if (line==0) strcpy(script1, "button 283 159 2117 2118 1 0 6");
	line--; if (line==0) strcpy(script1, "button 283 179 2117 2118 1 0 7");
	line--; if (line==0) strcpy(script1, "button 283 199 2117 2118 1 0 8");
	
	line--; if (line==0) strcpy(script1, "button 200 250 2074 2075 1 0 1");
	line--; if (line==0) strcpy(script1, "}");
}

void cAdmin::Wiped()//Revana*
{
	do
	{
		read2();
		if (!(strcmp(script1, "NAME")))
		{
			strcpy(wiped[wipecount].name, script2);
		}

		if (!(strcmp(script1, "CONTACT"))) strcpy(wiped[wipecount].contact, script2);

		if (!(strcmp(script1, "PASS")))
		{
			strcpy(wiped[wipecount].pass, script2);
		}

		if (!(strcmp(script1, "WIPESERIAL"))) wiped[wipecount].wipeSerial = str2num(script2);
	}
	while (strcmp(script1, "}") && strcmp(script1, "EOF"));
}

void cAdmin::LoadWipe()//Revana*
{
	// FILE *infile;
//	int i;
	wipecount=0;
	openscript("wiped-acct.adm");
	do
	{
		read2();
		if (!(strcmp(script1, "WIPED")))
		{
			//   account(infile);
			Wiped();
			wipecount++;
		}
	}
	while (strcmp(script1, "EOF"));
	
	printf("UOX3: %i wiped accounts stored\n",wipecount);
	closescript();
}

void cAdmin::CheckLocks(int nAcct)//Revana*
{
	bool found = false;
	int curLock = 0, killed = 0;
	unsigned int i;

	for(curLock=0;curLock< MAX_ACCT_LOCK;curLock++)
	{
		for(i=0;i<charcount;i++)
		{
			if (chars[i].serial==acctx[nAcct].lock[curLock])
			{
				found = true;
				break;
			}
		}

		if (found==false)
		{
			acctx[nAcct].lock[curLock]=0;
			killed++;
		}
	}
	printf("Deleted %i lock serials..\n",killed);
}
		

void cAdmin::Account()//Revana*
{
	int i = 0;
	int curLock = 0;

	acctx[acctcount].ip1 = 0;
	acctx[acctcount].ip2 = 0;
	acctx[acctcount].ip3 = 0;
	acctx[acctcount].ip4 = 0;

	acctx[acctcount].tempIP[0] = 0;

	acctx[acctcount].name[0] = 0;
	acctx[acctcount].pass[0] = 0;

	acctx[acctcount].contact[0] = 0;

	acctx[acctcount].ban = 0;
	acctx[acctcount].banSerial = 0;

	acctx[acctcount].wipe = 0;
	acctx[acctcount].wipeSerial = 0;

	acctx[acctcount].saveChar = 0;

	for(i=0;i< MAX_ACCT_LOCK ;i++) acctx[acctcount].lock[i] = 0;

	do
	{
		read2();

		if (!(strcmp(script1, "LASTIP"))) strcpy(acctx[acctcount].tempIP, script2);
		
		else if (!(strcmp(script1, "NAME")))
		{
			strcpy(acctx[acctcount].name, script2);
		}

		else if (!(strcmp(script1, "PASS")))
		{
			strcpy(acctx[acctcount].pass, script2);
		}

		else if (!(strcmp(script1, "BAN"))) acctx[acctcount].ban = 1;
		else if (!(strcmp(script1, "NOSAVECHAR"))) acctx[acctcount].saveChar = 1;

		else if (!(strcmp(script1, "LOCK")))
		{
			if (curLock < MAX_ACCT_LOCK)
			{
				acctx[acctcount].lock[curLock] = str2num(script2);
				curLock++;
			}
		}

		else if (!(strcmp(script1, "WIPE")))
		{
			if (script2[0]==0 || script2[0]==' ')
			{
				acctx[acctcount].wipeSerial = str2num(script2);
			}
			else acctx[acctcount].wipeSerial = -1;

			acctx[acctcount].wipe = 1;
		}
		else if (!(strcmp(script1, "CONTACT"))) strcpy(acctx[acctcount].contact, script2);

		else if (!(strcmp(script1, "WARNINGS"))) acctx[acctcount].warning = str2num(script2);

		else if (!(strcmp(script1, "TIMEBAN")))
		{
			acctx[acctcount].ban = 1;

			acctx[acctcount].banTime = str2num(script2);
		}
		else if (!(strcmp( script1, "XGM" ))) acctx[acctcount].xGM = true;

	}
	while (strcmp(script1, "}") && strcmp(script1, "EOF"));

//	CheckLocks(acctcount);
}

void cAdmin::LoadAccounts()//Revana*
{
	// FILE *infile;
	int /*i, */guestnum=0;
	acctcount=0;
	openscript("accounts.adm");
	do
	{
		read2();
		if (!(strcmp(script1, "SECTION")))
		{
			//   account(infile);
			Account();
			acctcount++;
		}
		// Guest accounts chages = "GUEST X" X=Number of accounts... anywhere in accounts.adm(Zippy)
		if (!(strcmp(script1, "GUEST")))
		{
			int i = 0;
			guestnum=str2num(script2);
			//printf("DEBUG: %i guest accounts\n",guestnum);
			for (i=1;i<guestnum+1;i++)
			{
				sprintf(acctx[acctcount].name, "guest%i", i);
				sprintf(acctx[acctcount].pass, "guest%i", i);
				//printf("DEBUG: Guest Account %i [L: %s] [P: %s]\n", i,acctx[acctcount][0],acctx[acctcount][1]);
				acctcount++;
			}
			//printf("UOX3: %i guest accounts loaded.\n", guestnum);
		}
		//end Guest changes.....	
	}
	while (strcmp(script1, "EOF"));
	printf("UOX3: %i accounts loaded from accounts.adm [Including %i guest accounts]\n",acctcount,guestnum);
	closescript();
}


void cAdmin::ReadString()
{
	int i=0;
	char c;
	while ((c= (char) fgetc(infile)) != 10)
	{
		if (c!=13)
		{
			temp[i]=c;
			i++;
		}
		if (feof(infile))
		{
			printf("ERROR: readstring() unexpectedly reached EOF.\n");
			printf("There is an error at the end of you uox3.ini file.\n");
			exit(1);
		}
	}
	temp[i]=0;
}

void cAdmin::ReadIni()
{
	int keep;
	unsigned int uiTempi;

	LoadAccounts();
	infile=fopen("uox3.ini","r");
	if (infile==NULL)
	{
		printf("ERROR: UOX3.INI not found...\n");
		error=1;
		keeprun=0;
		return;
	}
	keep=1;
	servcount=0;
	do
	{
		ReadString();
		if ((temp[0]=='#')&&(temp[1]=='#')&&(temp[2]=='#'))
		{
			keep=0;
		}
		else
		{
			for (uiTempi=0;uiTempi<=strlen(temp);uiTempi++) serv[servcount][0][uiTempi]=temp[uiTempi];
			ReadString();
			for (uiTempi=0;uiTempi<=strlen(temp);uiTempi++) serv[servcount][1][uiTempi]=temp[uiTempi];
			servcount++;
		}
	}
	while (keep);
	keep=1;
	startcount=0;
	do
	{
		ReadString();
		if ((temp[0]=='#')&&(temp[1]=='#')&&(temp[2]=='#'))
		{
			keep=0;
		}
		else
		{
			for (uiTempi=0;uiTempi<=strlen(temp);uiTempi++) start[startcount][0][uiTempi]=temp[uiTempi];
			ReadString();
			for (uiTempi=0;uiTempi<=strlen(temp);uiTempi++) start[startcount][1][uiTempi]=temp[uiTempi];
			ReadString();
			for (uiTempi=0;uiTempi<=strlen(temp);uiTempi++) start[startcount][2][uiTempi]=temp[uiTempi];
			ReadString();
			for (uiTempi=0;uiTempi<=strlen(temp);uiTempi++) start[startcount][3][uiTempi]=temp[uiTempi];
			ReadString();
			for (uiTempi=0;uiTempi<=strlen(temp);uiTempi++) start[startcount][4][uiTempi]=temp[uiTempi];
			startcount++;
		}
	}
	while (keep);

	#if CLIENTVERSION_M==26
			  
	          startcount=9;			  
    #endif
	
	ReadString();
//	sprintf(sScriptPath, "%s", temp );		// new script stuff, commented out for the moment
//	ReadString();

	assert(Map);
	strcpy(Map->mapname, temp);
	ReadString();
	strcpy(Map->sidxname, temp);
	ReadString();
	strcpy(Map->statname, temp);
	ReadString();
	strcpy(Map->vername, temp);
	ReadString();
	strcpy(Map->tilename, temp);
	ReadString();
	strcpy(Map->multiname, temp);
	ReadString();
	strcpy(Map->midxname, temp);

	ReadString();
	// this was a global var, and wasn't big enough previously - fur
	char saveintervalstr[20];
	strcpy(saveintervalstr, temp);
	saveinterval=atoi(saveintervalstr);
	ReadString();
	heartbeat=0;
	if (temp[0]=='1') heartbeat=1;
	ReadString();
	strcpy(goldamountstr, temp);
	goldamount=atoi(goldamountstr);
	ReadString();
	strcpy(defaultpriv1str, temp);
	defaultpriv1=hstr2num(defaultpriv1str);
	ReadString();
	strcpy(defaultpriv2str, temp);
	defaultpriv2=hstr2num(defaultpriv2str);
	//printf("%s\n%s\n",defaultpriv1str,defaultpriv2str);
	fclose(infile);
	infile = NULL;	// Dodger
}
