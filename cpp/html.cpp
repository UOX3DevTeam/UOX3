//------------------------------------------------------------------------
//  html.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1998 - 2001 by Unknown real name (Zippy)
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

//:Terrin: I was out to fix sprintf(s, "%s%c", s, c ) but here they were
//         mostly cases of someone not knowing out to do a backslash
//         in a constant string.
void guildpage(int, char *);
void guildhtml(void);

void offlinehtml()//HTML
{
	char sect[512], hfile[512],time_str[256];
	unsigned int total,hr,min,sec; // bugfix LB
	FILE *html;
	
	total = (unsigned int)( ( uiCurrentTime - starttime ) / MY_CLOCKS_PER_SEC );
	hr=total/3600;
	total-=hr*3600;
	min=total/60;
	total-=min*60;
	sec=total;

	strcpy(sect,"OFFLINE_PAGE");
	openscript("htmlstrm.scp");
	if(!i_scripts[html_script]->find(sect))
	{
		closescript();
		return;
	}
	read1();
	strcpy(hfile, script1);
	html=fopen(hfile,"w");
	if (html==NULL) 
	{
		closescript();	// don't leave this open! - fur
		ConOut("Could not create HTML file '%s', please check the path in htmlstrm.scp or the file permissions.\n", hfile );
		return;
	}
	do {
		read2();
		if(!(strcmp(script1, "FILE"))) 
		{
			if (html != NULL) 
				fclose(html);
			strcpy(hfile, script2);
			html = fopen(hfile, "w");
		}
		if(!(strcmp(script1,"LINE"))) fprintf(html,script2);
		if(!(strcmp(script1,"TIME"))) fprintf(html,"%s",RealTime(time_str));
		if(!(strcmp(script1,"UPTIME"))) fprintf(html,"%i:%i:%i",hr,min,sec);
	} while(script1[0]!='}');
	fclose(html);
	closescript();
}

void updatehtml()//HTML
{
	double eps = 0.00000000001;
	char sect[512],time_str[80],hfile[512],sh[3],sm[3],ss[3];
	int a, n=0;
	//unsigned long int ip;
	int gm=0,cns=0,ccount=0, npccount = 0;
	unsigned long int total,hr,min,sec; // bugfix, LB
	FILE *html;

	guildhtml();
	strcpy(sect, "ONLINE_PAGE");
	openscript("htmlstrm.scp");
	if(!i_scripts[html_script]->find(sect))
	{
		closescript();
		return;
	}
	read1();
	strcpy(hfile, script1);

	html=fopen(hfile,"w"); // remove old one first
	if (html==NULL) // LB
	{
		closescript();
		ConOut("Could not create HTML file '%s', please check the path in htmlstrm.scp or the file permissions.\n", hfile );
		return;
	}
	do {
		read2();
		if (script1[0] == '}') break;
		else if(!(strcmp(script1, "FILE"))) 
		{
			if (html != NULL) 
				fclose(html);
			strcpy(hfile, script2);
			html = fopen(hfile, "w");
		}
    else if(!(strcmp(script1,"BUILD"))) fprintf(html, "%s",BUILD);
		else if(!(strcmp(script1,"LINE"))) fprintf(html,"%s\n",script2);
		else if(!(strcmp(script1,"TIME"))) fprintf(html,"%s <BR>",RealTime(time_str));
		else if(!(strcmp(script1,"NOW")))
		{
			if(online(currchar[n]) && perm[n]) // bugfix LB
			{
				fprintf(html,"%s",chars[currchar[n]].name);
				n++;
			}
		}
		else if(!(strcmp(script1,"WHOLIST")))
		{
			a=0;
			for (n=0;n<now;n++)
			{ 
				if (online(currchar[n]) && perm[n]) // bugfix, LB
				{
					a++;
					fprintf(html,"%i - %s <BR>\n",a,chars[currchar[n]].name);
				}
			}
		}
		else if(!(strcmp(script1,"NOWNUM"))) fprintf(html,"%i",now);
		else if(!(strcmp(script1,"ACCOUNTNUM"))) fprintf(html,"%i",acctcount);
		else if(!(strcmp(script1,"CHARCOUNT")))
		{
			fprintf(html,"%i",chars.Count());
		}
		else if(!(strcmp(script1,"NPCS")))
		{
			if(npccount==0)
			{
				ccount=0;
				for(a=0;a<charcount;a++)
					if(chars[a].npc && !chars[a].free) npccount++; // bugfix LB
			}
			fprintf(html,"%i",npccount);
		}
		else if(!(strcmp(script1,"ITEMCOUNT"))) 
		{
			fprintf(html,"%i",items.Count());
		}
		else if(!(strcmp(script1,"UPTIME")))
		{
			total = (unsigned int)( ( uiCurrentTime - starttime ) / MY_CLOCKS_PER_SEC );
			hr=total/3600;
			if(hr<10 && hr<=60) sprintf(sh,"0%i",hr);
			else sprintf(sh,"%i",hr);
			total-=hr*3600;
			min=total/60;
			if(min<10 && min<=60) sprintf(sm,"0%i",min);
			else sprintf(sm,"%i",min);
			total-=min*60;
			sec=total;
			if(sec<10 && sec <=60) sprintf(ss,"0%i",sec);
			else sprintf(ss,"%i",sec);
			fprintf(html,"%s:%s:%s",sh,sm,ss);	
		}
		else if(!(strcmp(script1,"IP")))
		{
			//ip=inet_addr(serv[str2num(script2)-1][1]);
			fprintf(html,"%s",serv[str2num(script2)-1][1]);
		}
		else if(!(strcmp(script1,"GMNUM")))
		{
			if(gm==0)
			{
				for(a=0;a<now;a++)
				{
					if(chars[currchar[a]].priv&1) gm++;
					else if(chars[currchar[a]].priv&0x80) cns++; // bugfix LB
				}
			}
			fprintf(html,"%i",gm);
		}
		else if(!(strcmp(script1,"CNSNUM")))
		{
			if(cns==0)
			{
				for(a=0;a<now;a++)
				{
					if(chars[currchar[a]].priv&1) gm++;
					else if(chars[currchar[a]].priv&0x80) cns++; // bugfix LB
				}
			}
			fprintf(html,"%i",cns);
		}
		else if(!(strcmp(script1,"PDUMP"))) 
		{
			fprintf(html,"Network code: %fmsec [%i samples] <BR>",(float)((float)networkTime/(float)networkTimeCount),  networkTimeCount);
			fprintf(html,"Timer code: %fmsec [%i samples] <BR>" , (float)((float)timerTime/(float)timerTimeCount) , timerTimeCount);
			fprintf(html,"Auto code: %fmsec [%i samples] <BR>" , (float)((float)autoTime/(float)autoTimeCount) , autoTimeCount);
			fprintf(html,"Loop Time: %fmsec [%i samples] <BR>" , (float)((float)loopTime/(float)loopTimeCount) , loopTimeCount);
			//fprintf(html,"Characters: %i/Dynamic    Items: %i/Dynamic <BR>" , chars.Count(), items.Count());
			if(!(loopTime < eps ||  loopTimeCount < eps ) ) // Bugfix LB
				fprintf(html,"Simulation Cycles: %f per sec <BR>" ,(1000.0*(1.0/(float)((float)loopTime/(float)loopTimeCount))));
			else fprintf( html, "Simulation Cycles: Greater than 10000 <BR> " );
		}
		else if(!(strcmp(script1,"SIMCYC")))  // bugfix LB
		{
			if(!(loopTime < eps || loopTimeCount < eps ))
				fprintf(html,"%f" , (1000.0*(1.0/(float)((float)loopTime/(float)loopTimeCount))));
			else fprintf( html, "Greater than 10000" );
		}
		else if(!(strcmp(script1,"UDTIME")))	fprintf(html,"%f",(float)(server_data.html/60));
		else if(!(strcmp(script1,"LINEFEED"))) fprintf(html,"\n");
#ifndef __linux__
		else if(!(strcmp(script1,"VER"))) fprintf(html,"%s [WIN32]",VER);
	#else
		else if(!(strcmp(script1,"VER"))) fprintf(html,"%s [LINUX]",VER);
#endif
		else ConOut("HTML: WARNING: Unknown tag \"%s\" in \"%s\", ignored.\n", script1, hfile);
	} while(script1[0]!='}');
	fclose(html);
	closescript();
}

void guildhtml(void)
{
	char sect[512],time_str[80],hfile[512],hdir[256];//sh[3],sm[3],ss[3];
	int i, nxtg = 1;
	long int pos;
	FILE *html;

	strcpy(sect,"GUILD_MAIN");
	openscript("htmlstrm.scp");
	if(!i_scripts[html_script]->find(sect))
	{
		closescript();
		return;
	}
	read1();
	strcpy(hfile, script1);
	read1();
	strcpy(hdir, script1);
	html=fopen(hfile,"w");
	if (html==NULL) 
	{
		closescript();
		return;
	}
	do {
		read2();
		if(!(strcmp(script1,"LINE"))) fprintf(html,"%s\n",script2);
		else if(!(strcmp(script1,"TIME"))) fprintf(html,"%s",RealTime(time_str));
		else if(!(strcmp(script1,"GUILDLIST")))
		{
			for (i=1;i<MAXGUILDS;i++)
			{
				if (!Guilds->guilds[i].free && strlen( Guilds->guilds[i].name ) > 1 )
				{
					fprintf(html, "<A HREF=\"%s\\%i.html\">%s [%s]</A>&nbsp;&nbsp;\n",hdir,i,Guilds->guilds[i].name,Guilds->guilds[i].abbreviation);
					if (Guilds->guilds[i].type==1) fprintf(html,"[Order]");
					else if(Guilds->guilds[i].type==2) fprintf(html,"[Chaos]");
					fprintf(html, "<br>\n");
					pos=ftell(scpfile);
					closescript();
					guildpage(i,hdir);
					openscript("htmlstrm.scp");
					fseek(scpfile,pos,SEEK_SET);
					strcpy(script1, "NODATA :o)");
				}
			}
		}
		else if(!(strcmp(script1,"NEXTGUILD")))
		{
			if (!Guilds->guilds[nxtg].free && strlen( Guilds->guilds[nxtg].name ) > 1 )
			{
				i=nxtg;
				fprintf(html, "<A HREF=\"%s\\%i.html\">%s [%s]</A>&nbsp;&nbsp;\n",hdir,i,Guilds->guilds[i].name,Guilds->guilds[i].abbreviation);
				if (Guilds->guilds[i].type==1) fprintf(html,"[Order]");
				else if(Guilds->guilds[i].type==2) fprintf(html,"[Chaos]");
				fprintf(html, "<br>\n");
				pos=ftell(scpfile);
				closescript();
				guildpage(i,hdir);
				openscript("htmlstrm.scp");
				fseek(scpfile,pos,SEEK_SET);
				strcpy(script1, "NODATA :o)");
			}
			nxtg++;
		}
	} while(script1[0]!='}');
	fclose(html);
	closescript();
}

void guildpage(int i, char *gdir)
{
	char sect[32],time_str[80],gfile[512];
	int a;
	unsigned int nxtmem=1,nxtwar=0;
	FILE *ghtml;

	strcpy(sect,"GUILD_PAGE");
	openscript("htmlstrm.scp");
	if(!i_scripts[html_script]->find(sect))
	{
		closescript();
		return;
	}
	sprintf(gfile,"%s\\%i.html",gdir,i);
	ghtml=fopen(gfile,"w");
	if (ghtml==NULL)
	{
		closescript();
		return;
	}
	do {
		read2();
		if(!(strcmp(script1,"LINE"))) fprintf(ghtml,"%s\n",script2);
		else if(!(strcmp(script1,"NAME"))) fprintf(ghtml, "%s",Guilds->guilds[i].name);
		else if(!(strcmp(script1,"TYPE"))) 
		{
			if (Guilds->guilds[i].type==1) fprintf(ghtml,"[Order]");
			else if (Guilds->guilds[i].type==2) fprintf(ghtml, "[Chaos]");
		}
		else if(!(strcmp(script1,"ABREV"))) fprintf(ghtml, "%s", Guilds->guilds[i].abbreviation);
		else if(!(strcmp(script1,"SITE"))) fprintf(ghtml, "%s",Guilds->guilds[i].webpage);
		else if(!(strcmp(script1,"CHARTER"))) fprintf(ghtml,"%s",Guilds->guilds[i].charter);
		else if(!(strcmp(script1,"MASTER"))) fprintf(ghtml, "%s",chars[calcCharFromSer(Guilds->guilds[i].master)].name);
		else if(!(strcmp(script1,"LOGO"))) fprintf(ghtml, "%s\\%i.jpg",gdir,i);
		else if(!(strcmp(script1,"TIME"))) fprintf(ghtml,"%s",RealTime(time_str));
		else if(!(strcmp(script1,"WARLIST")))
		{
			for (a=1;a<MAXGUILDWARS;a++)
			{
				if (Guilds->guilds[i].war[a]!=0 && strlen( Guilds->guilds[Guilds->guilds[i].war[a]].name) > 1 )
				{
					fprintf(ghtml,"<A HREF=\"%s\\%i.html\">%s&nbsp;[%s]</A>&nbsp;&nbsp;",gdir,Guilds->guilds[i].war[a],Guilds->guilds[Guilds->guilds[i].war[a]].name,Guilds->guilds[Guilds->guilds[i].war[a]].abbreviation);
					if (Guilds->guilds[Guilds->guilds[i].war[a]].type==1) fprintf(ghtml,"[Order]");
					else if(Guilds->guilds[Guilds->guilds[i].war[a]].type==2) fprintf(ghtml,"[Chaos]");
					fprintf(ghtml, "<br>\n");
				}
			}
		}
		else if(!(strcmp(script1,"NEXTWAR")))
		{
			if (Guilds->guilds[i].war[nxtwar]!=0 && strlen( Guilds->guilds[Guilds->guilds[i].war[nxtwar]].name) > 1 )
			{
				fprintf(ghtml, "<A HREF=\"%s\\%i.html\">%s&nbsp;[%s]</A>&nbsp;&nbsp;",gdir,Guilds->guilds[i].war[nxtwar],Guilds->guilds[Guilds->guilds[i].war[nxtwar]].name,Guilds->guilds[Guilds->guilds[i].war[nxtwar]].abbreviation);
				if (Guilds->guilds[Guilds->guilds[i].war[nxtwar]].type==1) fprintf(ghtml,"[Order]");
				else if(Guilds->guilds[Guilds->guilds[i].war[nxtwar]].type==2) fprintf(ghtml,"[Chaos]");
			}
			nxtwar++;
		}
		else if(!(strcmp(script1,"MEMBERLIST")))
		{
			for (a=1;a < Guilds->guilds[i].members;a++)
			{
				if (Guilds->guilds[i].member[a]!=0)
					fprintf(ghtml,"%s&nbsp;[%s] <BR>\n",chars[calcCharFromSer(Guilds->guilds[i].member[a])].name,chars[calcCharFromSer(Guilds->guilds[i].member[a])].guildtitle);
			}
		}
		else if(!(strcmp(script1,"NEXTMEMBER")))
		{
			if (Guilds->guilds[i].member[nxtmem]!=0)
				fprintf(ghtml, "%s&nbsp;[%s]",chars[calcCharFromSer(Guilds->guilds[i].member[nxtmem])].name,chars[calcCharFromSer(Guilds->guilds[i].member[nxtmem])].guildtitle);
			nxtmem++;
		}
	} while(script1[0]!='}');
	fclose(ghtml);
	closescript();
}
