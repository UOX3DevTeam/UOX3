//------------------------------------------------------------------------
//  network.cpp
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

#ifdef __LINUX__
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif


#define DBGFILE "Network.cpp"
static unsigned int bit_table[257][2] =
{
	0x02, 0x00, 	0x05, 0x1F, 	0x06, 0x22, 	0x07, 0x34, 	0x07, 0x75, 	0x06, 0x28, 	0x06, 0x3B, 	0x07, 0x32, 
	0x08, 0xE0, 	0x08, 0x62, 	0x07, 0x56, 	0x08, 0x79, 	0x09, 0x19D,	0x08, 0x97, 	0x06, 0x2A, 	0x07, 0x57, 
	0x08, 0x71, 	0x08, 0x5B, 	0x09, 0x1CC,	0x08, 0xA7, 	0x07, 0x25, 	0x07, 0x4F, 	0x08, 0x66, 	0x08, 0x7D, 
	0x09, 0x191,	0x09, 0x1CE, 	0x07, 0x3F, 	0x09, 0x90, 	0x08, 0x59, 	0x08, 0x7B, 	0x08, 0x91, 	0x08, 0xC6, 
	0x06, 0x2D, 	0x09, 0x186, 	0x08, 0x6F, 	0x09, 0x93, 	0x0A, 0x1CC,	0x08, 0x5A, 	0x0A, 0x1AE,	0x0A, 0x1C0, 
	0x09, 0x148,	0x09, 0x14A, 	0x09, 0x82, 	0x0A, 0x19F, 	0x09, 0x171,	0x09, 0x120, 	0x09, 0xE7, 	0x0A, 0x1F3, 
	0x09, 0x14B,	0x09, 0x100,	0x09, 0x190,	0x06, 0x13, 	0x09, 0x161,	0x09, 0x125,	0x09, 0x133,	0x09, 0x195, 
	0x09, 0x173,	0x09, 0x1CA,	0x09, 0x86, 	0x09, 0x1E9, 	0x09, 0xDB, 	0x09, 0x1EC,	0x09, 0x8B, 	0x09, 0x85, 
	0x05, 0x0A, 	0x08, 0x96, 	0x08, 0x9C, 	0x09, 0x1C3, 	0x09, 0x19C,	0x09, 0x8F, 	0x09, 0x18F,	0x09, 0x91, 
	0x09, 0x87, 	0x09, 0xC6, 	0x09, 0x177,	0x09, 0x89, 	0x09, 0xD6, 	0x09, 0x8C, 	0x09, 0x1EE,	0x09, 0x1EB, 
	0x09, 0x84, 	0x09, 0x164, 	0x09, 0x175,	0x09, 0x1CD, 	0x08, 0x5E, 	0x09, 0x88, 	0x09, 0x12B,	0x09, 0x172, 
	0x09, 0x10A,	0x09, 0x8D, 	0x09, 0x13A,	0x09, 0x11C, 	0x0A, 0x1E1,	0x0A, 0x1E0, 	0x09, 0x187,	0x0A, 0x1DC, 
	0x0A, 0x1DF,	0x07, 0x74, 	0x09, 0x19F,	0x08, 0x8D,		0x08, 0xE4, 	0x07, 0x79, 	0x09, 0xEA, 	0x09, 0xE1, 
	0x08, 0x40, 	0x07, 0x41, 	0x09, 0x10B,	0x09, 0xB0, 	0x08, 0x6A, 	0x08, 0xC1, 	0x07, 0x71, 	0x07, 0x78, 
	0x08, 0xB1, 	0x09, 0x14C, 	0x07, 0x43, 	0x08, 0x76, 	0x07, 0x66, 	0x07, 0x4D, 	0x09, 0x8A, 	0x06, 0x2F, 
	0x08, 0xC9,		0x09, 0xCE, 	0x09, 0x149,	0x09, 0x160, 	0x0A, 0x1BA, 	0x0A, 0x19E, 	0x0A, 0x39F, 	0x09, 0xE5, 
	0x09, 0x194, 	0x09, 0x184, 	0x09, 0x126, 	0x07, 0x30, 	0x08, 0x6C, 	0x09, 0x121, 	0x09, 0x1E8, 	0x0A, 0x1C1, 
	0x0A, 0x11D, 	0x0A, 0x163, 	0x0A, 0x385, 	0x0A, 0x3DB, 	0x0A, 0x17D, 	0x0A, 0x106, 	0x0A, 0x397, 	0x0A, 0x24E, 
	0x07, 0x2E, 	0x08, 0x98, 	0x0A, 0x33C, 	0x0A, 0x32E, 	0x0A, 0x1E9, 	0x09, 0xBF, 	0x0A, 0x3DF, 	0x0A, 0x1DD, 
	0x0A, 0x32D, 	0x0A, 0x2ED, 	0x0A, 0x30B, 	0x0A, 0x107, 	0x0A, 0x2E8, 	0x0A, 0x3DE, 	0x0A, 0x125, 	0x0A, 0x1E8, 
	0x09, 0xE9, 	0x0A, 0x1CD, 	0x0A, 0x1B5, 	0x09, 0x165, 	0x0A, 0x232, 	0x0A, 0x2E1, 	0x0B, 0x3AE, 	0x0B, 0x3C6, 
	0x0B, 0x3E2, 	0x0A, 0x205, 	0x0A, 0x29A, 	0x0A, 0x248, 	0x0A, 0x2CD, 	0x0A, 0x23B, 	0x0B, 0x3C5, 	0x0A, 0x251, 
	0x0A, 0x2E9, 	0x0A, 0x252, 	0x09, 0x1EA, 	0x0B, 0x3A0, 	0x0B, 0x391, 	0x0A, 0x23C, 	0x0B, 0x392, 	0x0B, 0x3D5, 
	0x0A, 0x233, 	0x0A, 0x2CC, 	0x0B, 0x390, 	0x0A, 0x1BB, 	0x0B, 0x3A1, 	0x0B, 0x3C4, 	0x0A, 0x211, 	0x0A, 0x203, 
	0x09, 0x12A, 	0x0A, 0x231, 	0x0B, 0x3E0, 	0x0A, 0x29B, 	0x0B, 0x3D7, 	0x0A, 0x202, 	0x0B, 0x3AD, 	0x0A, 0x213, 
	0x0A, 0x253, 	0x0A, 0x32C, 	0x0A, 0x23D, 	0x0A, 0x23F, 	0x0A, 0x32F, 	0x0A, 0x11C, 	0x0A, 0x384, 	0x0A, 0x31C, 
	0x0A, 0x17C, 	0x0A, 0x30A, 	0x0A, 0x2E0, 	0x0A, 0x276, 	0x0A, 0x250, 	0x0B, 0x3E3, 	0x0A, 0x396, 	0x0A, 0x18F, 
	0x0A, 0x204, 	0x0A, 0x206, 	0x0A, 0x230, 	0x0A, 0x265, 	0x0A, 0x212, 	0x0A, 0x23E, 	0x0B, 0x3AC, 	0x0B, 0x393, 
	0x0B, 0x3E1, 	0x0A, 0x1DE, 	0x0B, 0x3D6, 	0x0A, 0x31D, 	0x0B, 0x3E5, 	0x0B, 0x3E4, 	0x0A, 0x207, 	0x0B, 0x3C7, 
	0x0A, 0x277, 	0x0B, 0x3D4, 	0x08, 0xC0,		0x0A, 0x162, 	0x0A, 0x3DA, 	0x0A, 0x124, 	0x0A, 0x1B4, 	0x0A, 0x264, 
	0x0A, 0x33D, 	0x0A, 0x1D1, 	0x0A, 0x1AF, 	0x0A, 0x39E, 	0x0A, 0x24F, 	0x0B, 0x373, 	0x0A, 0x249, 	0x0B, 0x372, 
	0x09, 0x167, 	0x0A, 0x210, 	0x0A, 0x23A, 	0x0A, 0x1B8, 	0x0B, 0x3AF, 	0x0A, 0x18E, 	0x0A, 0x2EC, 	0x07, 0x62, 
	0x04, 0x0D
};

#ifndef __NT__
#define closesocket(s)	close(s)
#endif

#if CLIENTVERSION_M==26
void cNetworkStuff::DoStreamCode( int s )
{
	if( s==-1 ) return;
	int len = Pack( outbuffer[s], xoutbuffer, boutlength[s] );
	send( client[s], xoutbuffer, len, 0 );
}
#endif


int cNetworkStuff::xRecv(int s) // Better Receive routine than the one currently used; not intergrated yet
{
	int count, i;
#ifdef DEBUG_NETWORK
	int x, y = 0, j;
	char st[10], txt[17];
#endif
	
	count = recv(client[s], (char *)&buffer[s][binlength[s]], MAXBUFFER-binlength[s], 0);
#ifdef DEBUG_NETWORK
	printf("REC:\n");
	x=0;
#endif
	i = binlength[s] + count;
	binlength[s]+=count;
#ifdef DEBUG_NETWORK
	if ((y)!=15)
	{
		txt[y+1]=0;
		for (j=15-y;j>0;j--) printf("   ");
		printf("%s\n",txt);
	}
#endif
	return count;
}

void cNetworkStuff::Processed(int s, int i) // To be used later together with xrecv
{
	memcpy(buffer[s], &buffer[s][i], binlength[s]-i);
	binlength[s]-=i;
	if (binlength[s]<0) binlength[s]=0;
}

void cNetworkStuff::FlushBuffer(int s) // Sends buffered data at once
{
	if (boutlength[s]>0)
	{
		//  printf("S = %i, bout = %i, cc = %i\n", s, boutlength[s], cryptclient[s]);
		if (cryptclient[s])
		{
			DoStreamCode(s);
		}
		else
		{
			send( client[s], (char *)outbuffer[s], boutlength[s], 0);
		}
		boutlength[s]=0;
		//  printf("Done\n");
	}
}

void cNetworkStuff::ClearBuffers() // Sends ALL buffered data
{
	int i;
	
	for (i=0;i<now;i++)
	{
		FlushBuffer(i);
	}
}

void cNetworkStuff::xSend(int s, void *point, int length, int test) // Buffering send function
{
	if( s == -1 ) return;
	if (boutlength[s]+length>MAXBUFFER) FlushBuffer(s);
#ifdef DEBUG_NETWORK
	int buf2c, i, j;
	char buf2[32];
	
	printf("*** xSend\n"); fflush(stdout);
	
	buf2c=0;
	for(i=0;i<length;i++)
	{
		buf2[buf2c]=((unsigned char *)point)[i];
		buf2c++;
		if ((buf2c==16)||(i==length-1))
		{
			printf("C%02x %02x : ",s,((i%256)/16)*16);
			for (j=0;j<buf2c;j++)
			{
				printf("%2x ",buf2[j]);
			}
			
			for (j=0;j<buf2c;j++)
			{
				if ((isalnum(buf2[j]))||(isprint(buf2[j]))) printf("%c",buf2[j]); else printf(".");
			}
			for(;j<16;j++) printf(".");
			buf2c=0;
			printf("]\n");
		}
	}
	fflush(stdout);
#endif
	memcpy(&outbuffer[s][boutlength[s]], point, length);
	boutlength[s]+=length;
	// printf("%i\n", boutlength[s]);
}

// set the laston character member value to the current date/time
void setLastOn(UOXSOCKET s)
{
	assert(s != -1);
	
	time_t ltime;
	time( &ltime );
	char *t = ctime(&ltime);
	// just to be paranoid and avoid crashing
	if (NULL == t)
		t = "";
	else
	{
		// some ctime()s like to stick \r\n on the end, we don't want that
		for (int end = strlen(t) - 1; end >= 0 && isspace(t[end]); --end)
			t[end] = '\0';
	}
	safeCopy(chars[currchar[s]].laston, t, MAX_LASTON);
}

void cNetworkStuff::Disconnect (int s) // Force disconnection of player //Instalog
{
	int j;
	
	setLastOn( s );
	if( xgm )
	{
		if( xGM[s]->isClient )
		{
			printf("UOX3: Client %i (XGM) disconnected. [Total:%i]\n", s, now-1 );
			xGM[s]->isClient = 0;
		} 
		else
			printf("UOX3: Client %i disconnected. [Total: %i]\n", s, now - 1 );
	}
	else
		printf("UOX3: Client %i disconnected. [Total:%i]\n",s,now-1);

	FlushBuffer( s );
	closesocket( client[s] );
	
	if ((chars[currchar[s]].account==acctno[s])&&(server_data.partmsg)) 
	{
		sprintf(temp,"%s has left the realm",chars[currchar[s]].name);
		sysbroadcast(temp);//message upon leaving a server 
	} 
	if (acctno[s]!=-1) 
		acctinuse[acctno[s]]=0; //Bug clearing logged in accounts!
	acctno[s]=-1;
	idleTimeout[s] = -1;
	for( j = s; j < now - 1; j++ )
	{
		client[j] = client[j+1];
		newclient[j] = newclient[j+1];
		currchar[j] = currchar[j+1];
		clientip[j][0] = clientip[j+1][0];
		clientip[j][1] = clientip[j+1][1];
		clientip[j][2] = clientip[j+1][2];
		clientip[j][3] = clientip[j+1][3];
		acctno[j] = acctno[j+1];
		//  war[j]=war[j+1];
		perm[j] = perm[j+1];
		addid1[j] = addid1[j+1];
		addid2[j] = addid2[j+1];
		addid3[j] = addid3[j+1];
		addid4[j] = addid4[j+1];
		addx[j] = addx[j+1];
		addy[j] = addy[j+1];
		addz[j] = addz[j+1];
		cryptclient[j] = cryptclient[j+1];
		idleTimeout[j] = idleTimeout[j+1];
		firstpacket[j] = firstpacket[j+1];
	}
	//Instalog
	LogOut(s);
	currchar[now]=0;
	now--;
	WhoList->FlagUpdate();
	OffList->FlagUpdate();
}

const signed long LOGIN_NOT_FOUND = -3;
const signed long BAD_PASSWORD = -4;
const signed long ACCOUNT_BANNED = -5;
const signed long ACCOUNT_WIPE = -6;

signed long cNetworkStuff::Authenticate( const char *username, const char *pass )
{
	int i = 0;

	// We want case sensitive password/usernames
	bool loginfound = false;
	int loginlength = strlen( username );	// pre calc login length
	while( !loginfound && i < acctcount )
	{
		if( strlen( acctx[i].name ) == loginlength )
			loginfound = ( strcmp( acctx[i].name, username ) == 0 );
		if( !loginfound )
			i++;
	}
	if( !loginfound )
		return LOGIN_NOT_FOUND;
	if( !strcmp( acctx[i].pass, pass ) )
	{
		if( acctx[i].ban == 1 )
			return ACCOUNT_BANNED;
		else if( acctx[i].wipe == 1 )
			return ACCOUNT_WIPE;
		else 
			return i;
	}
	else
		return BAD_PASSWORD;
}

void cNetworkStuff::Login1(int s) // Initial login (Login on "loginserver", new format) // Revana*
{
	int i;
	unsigned long int j, tlen, t;
	unsigned long int ip;
	char nopass[3] = "\x82\x03";
	char acctused[3]="\x82\x01";
	char newlist1[7]="\xA8\x01\x23\xFF\x00\x01";
	char newlist2[41]="\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x12\x01\x7F\x00\x00\x01";
	
	acctno[s]=-1;

	pSplit( (char *)&buffer[s][31] );
	i = Authenticate( (char *)&buffer[s][1], pass1 );
	if( i >= 0 )
		acctno[s] = i;
	else
	{
		// Check for error
		switch( i )
		{
		case BAD_PASSWORD:
			acctno[s] = -1;
			xSend( s, nopass, 2, 0 );
			Disconnect( s );
			break;
		case ACCOUNT_BANNED:
			acctno[s] = -1;
			xSend( s, acctblock, 2, 0 );
			Disconnect( s );
			break;
		case ACCOUNT_WIPE:
			acctno[s] = -1;
			xSend( s, noaccount, 2, 0 );
			Disconnect( s );
			break;
		case LOGIN_NOT_FOUND:
			if( !server_data.auto_a_create )
			{
				acctno[s] = -1;
				xSend( s, noaccount, 2, 0 );
				Disconnect( s );
			}
			else
			{
				t = 1;
				sprintf( temp, "%x.%x.%x.%x", clientip[s][0], clientip[s][1], clientip[s][2], clientip[s][3] );
				for( j = 0; j < acctcount; j++ )
				{
					if ( (!(strcmp(temp,acctx[j].tempIP))) || ( acctx[j].ip1 == clientip[s][0] && acctx[j].ip2 == clientip[s][1] && acctx[j].ip3 == clientip[s][2] && acctx[j].ip4 == clientip[s][3] ) )
					{
						t = 0;
						break;
					}
				}
				
				if( t )
				{
					printf("AUTOACCT: New account \"%s\":\"%s\" created for %x.%x.%x.%x\n",&buffer[s][1],&buffer[s][31],clientip[s][0],clientip[s][1],clientip[s][2],clientip[s][3]);
					memset(acctx + acctcount,0,sizeof(acct_st));
					strcpy(acctx[acctcount].name, (char *)&buffer[s][1]);
					strcpy(acctx[acctcount].pass, (char *)&buffer[s][31]);
					acctno[s] = acctcount;
					acctx[acctcount].ip1 = clientip[s][0];
					acctx[acctcount].ip2 = clientip[s][1];
					acctx[acctcount].ip3 = clientip[s][2];
					acctx[acctcount].ip4 = clientip[s][3];
					acctx[acctcount].listpublic = true;
					cwmWorldState->saveAccount();
					acctcount++;
				} 
				else 
				{
					xSend(s, acctused, 2, 0);
					Disconnect( s );	// Should kick them once it fails
					return;
				}
				t = 0;
			}
			break;
		}
	}
	if( acctinuse[acctno[s]] )
	{
		for( int tmpJ = 0; tmpJ < now; tmpJ++ )
		{
			if( acctno[tmpJ] == acctno[s] && tmpJ != s )
			{
				Disconnect( tmpJ );
			}
		}
	}
	if( acctno[s] != -1 )
	{
#ifndef __NT__
		acctx[acctno[s]].ip1 = (char) clientip[s][0];
		acctx[acctno[s]].ip2 = (char) clientip[s][1];
		acctx[acctno[s]].ip3 = (char) clientip[s][2];
		acctx[acctno[s]].ip4 = (char) clientip[s][3];
#else
		acctx[acctno[s]].ip1 = client_addr.sin_addr.S_un.S_un_b.s_b1;
		acctx[acctno[s]].ip2 = client_addr.sin_addr.S_un.S_un_b.s_b2;
		acctx[acctno[s]].ip3 = client_addr.sin_addr.S_un.S_un_b.s_b3;
		acctx[acctno[s]].ip4 = client_addr.sin_addr.S_un.S_un_b.s_b4;
#endif
		sprintf(temp,"Client [%i.%i.%i.%i] connected using Account '%s'.\n",
			acctx[acctno[s]].ip1, acctx[acctno[s]].ip2, acctx[acctno[s]].ip3, acctx[acctno[s]].ip4, &buffer[s][1]);
		savelog(temp,"server.log");
		printf( temp );
		
		acctinuse[acctno[s]]=1;
		tlen=6+(servcount*40);
		newlist1[1]=(char)(tlen>>8);
		newlist1[2]=(char)(tlen%256);
		newlist1[4]=(char)(servcount>>8);
		newlist1[5]=(char)(servcount%256);
		xSend(s, newlist1, 6, 0);
		for (i=0;i<servcount;i++)
		{
#if CLIENTVERSION_M==26
			newlist2[0]=(char)((i+1)>>8);
			newlist2[1]=(char)((i+1)%256);
#endif
			strcpy(&newlist2[2], serv[i][0]);
			ip=htonl(inet_addr(serv[i][1]));
			newlist2[36]=(char) (ip>>24);
			newlist2[37]=(char) (ip>>16);
			newlist2[38]=(char) (ip>>8);
			newlist2[39]=(char) (ip%256);
			xSend(s, newlist2, 40, 0);
		}
	}
}

void cNetworkStuff::Relay(int s) // Relay player to a certain IP
{
#ifdef DEBUG_NETWORK
	printf("Going to relay...\n");
#endif
	
	unsigned long int ip;
	ip = htonl(inet_addr( serv[buffer[s][2]-1][1] ));
	//C00 00 : 8c ce e4 3d 6d  a 21 7f  0  0  1 ...=m.!.........]
	//C00 00 : 8c 6d 3d e4 ce  a 21 7f  0  0  1 .m=...!.........]
	
	login03[1]=(char) (ip>>24);
	login03[2]=(char) (ip>>16);
	login03[3]=(char) (ip>>8);
	login03[4]=(char) (ip%256);
	login03[5]=UOX_PORT>>8;
	login03[6]=UOX_PORT%256;
	srand(ip+acctno[s]+now+uiCurrentTime); // Perform randomize
	login03[7]=127;
	login03[8]=0;
	login03[9]=0;
	login03[10]=1;
	xSend(s, login03, 11, 0);
}

void cNetworkStuff::GoodAuth(int s) // Revana*
{
	int i, j, tlen;
	bool passed = false;
	
	tlen=4+(5*60)+1+(startcount*63);
	login04a[1]=tlen>>8;
	login04a[2]=tlen%256;
	login04a[3]=5;
	xSend(s, login04a, 4, 0);
	j=0;
	for (i=0;i<charcount;i++)
	{
		if ((chars[i].account==acctno[s])&&(chars[i].free==0)&&(strlen(chars[i].name)>0))
		{
			passed = false;
			if( passed == false )
			{
				strcpy(login04b, chars[i].name);
				xSend(s, login04b, 60, 0);
				j++;
			}
		}
	}
	for (i=0;i<60;i++) login04b[i]=0;
	for (i=j;i<5;i++)
	{
		xSend(s, login04b, 60, 0);
	}
	buffer[s][0]=startcount;
	xSend(s, buffer[s], 1, 0);
	for (i=0;i<startcount;i++)
	{
		login04d[0]=i;
		for (j=0;j<=strlen(start[i][0]);j++) login04d[j+1]=start[i][0][j];
		for (j=0;j<=strlen(start[i][1]);j++) login04d[j+32]=start[i][1][j];
		xSend(s, login04d, 63, 0);
	}
	if( xgm )
	{
		for( i = 0; i < now; i++ )
		{
			if( xGM[i]->isClient && xGM[i]->Acct == acctno[s] && acctx[acctno[s]].xGM )
			{ // Check to see if they are an XGM, if so, log 'em in.
				xGM[i]->ClientSock = s;
				break;
			}
		}
	}
}

void cNetworkStuff::FailAuth(int s)
{
	char noauth[3]="\x53\x01";
#ifdef DEBUG_NETWORK
	printf("Character doesn't exist!\n");
#endif
	xSend(s, noauth, 2, 0); // Say "Character doesnt exist" and close client
	Disconnect(s);
	return;
}

void cNetworkStuff::AuthTest(int s)
{
	int auth;
	unsigned char answer[3];
	
	auth=1;
	recv( server[s], (char *)answer, 2, 0);
	if ((answer[0]!=0xA8)&&(answer[0]!=0x81)) auth=0;
	closesocket(server[s]);
	server[s]=-1;
	if (auth)
	{
		printf("AUTH: Authentication successful\n");
		GoodAuth(s);
	}
	else
	{
		printf("AUTH: Authentication failed\n");
		FailAuth(s);
	}
	// sysbroadcast("Normal server operation resumed.");
}

void cNetworkStuff::CharList(int s) // Gameserver login and character listing // Revana*
{
	int i;
#ifdef LSERVCHECK
#ifdef NOSINGLEONLY
	int ls, ls2, auth;
	int lcode, positive;
	char verify1[5]="\x12\x34\x56\x78";
	char verify2[63]="\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
#endif
#endif
	char nopass[3] = "\x82\x03";
	
	acctno[s] = -1;
	pSplit((char *)&buffer[s][35]);
	i = Authenticate( (char *)&buffer[s][5], pass1 );
	if( i >= 0 )
		acctno[s] = i;
	else
	{
		switch( i )
		{
		case ACCOUNT_WIPE:
#ifdef DEBUG_NETWORK
			printf("No account!\n");
#endif
			acctno[s] = -1;
			xSend( s, noaccount, 2, 0 );
			Disconnect( s );
			return;
		case BAD_PASSWORD:
#ifdef DEBUG_NETWORK
			printf("No password!\n");
#endif
			acctno[s]=-1;
			xSend(s, nopass, 2, 0);
			Disconnect(s);
			return;
		case ACCOUNT_BANNED:
#ifdef DEBUG_NETWORK
			printf( "Player blocked!\n" );	// banned
#endif
			acctno[s]=-1;
			xSend(s, acctblock, 2, 0);
			Disconnect(s);
			return;
		default:
#ifdef DEBUG_NETWORK
			printf("No account!\n");
#endif
			xSend(s, noaccount, 2, 0);
			Disconnect(s);
			return;
		}
	}
	GoodAuth( s );
}

void cNetworkStuff::pSplit (char *pass0) // Split login password into UOX password and UO password
{
	int i;
	i=0;
	pass1[0]=0;
	while (pass0[i]!='/' && pass0[i]!=0) i++;
	strncpy(pass1,pass0,i);
	pass1[i]=0;
	if (pass0[i]!=0) strcpy(pass2, pass0+i+1);
}

//Boats->added multi checking to instalog.
void cNetworkStuff::LogOut( UOXSOCKET s )//Instalog
{
	int p = currchar[s], a, valid=0;
	int x=chars[currchar[s]].x, y=chars[currchar[s]].y;
	int multi = -1, b, ci;

	if(chars[p].priv&1 || chars[p].priv&80 || chars[p].account==0) 
		valid=1;
	else 
	{
		for(a=0;a<logoutcount;a++)
		{
			if (logout[a].x1<=x && logout[a].y1<=y && logout[a].x2>=x && logout[a].y2>=y)
			{
				valid=1;
				break;
			}
		}
	}

	if ( !valid )
	{
		if (chars[p].multis==-1)
			multi=findmulti(chars[p].x,chars[p].y,chars[p].z);
		else multi=findbyserial(&itemsp[chars[p].multis%HASHMAX],chars[p].multis,0);
	}
	
	if (multi!=-1 && !valid)//It they are in a multi... and it's not already valid (if it is why bother checking?)
	{
		b=packitem(p);
		if (b!=-1)
		{
			for (a=0;a<contsp[items[b].serial%HASHMAX].max;a++)
			{
				ci=contsp[items[b].serial%HASHMAX].pointer[a];
				if (ci!=-1)
				{
					if (items[ci].type==7 && items[ci].contserial == items[b].serial && (
						items[ci].more1==items[multi].ser1 && items[ci].more2==items[multi].ser2 &&
						items[ci].more3==items[multi].ser3 && items[ci].more4==items[multi].ser4))
					{//a key to this multi
						valid=1;//Log 'em out now!
						break;
					}
				}
			}
		}
	}
	
	if( !valid )  // If we are not in a valid logout location by now, we will check for campfire or bedroll - Retalin 1/15/2001
	{
		bool fire = false, roll = false;
		unsigned short realID = 0;
		int	StartGrid = mapRegions->StartGrid( chars[p].x, chars[p].y );		
		unsigned int increment = 0;
		for( unsigned int checkgrid = StartGrid + ( increment * mapRegions->GetColSize() ); increment < 3; increment++, checkgrid = StartGrid + ( increment * mapRegions->GetColSize() ) )
		{
			for( int a = 0; a < 3; a++ )
			{
				int mapitemptr = -1;
				int	mapitem = -1;
				do //check all items in this cell
				{
					mapitemptr = mapRegions->GetNextItem( checkgrid + a, mapitemptr );
					if( mapitemptr == -1 ) 
						break;
					mapitem = mapRegions->GetItem( checkgrid + a, mapitemptr );
					if( mapitem != -1 )
					{ // Instalog // AntiChrist
						if( mapitem < 999999 && mapitem < imem ) 
						{
							if( iteminrange( s, mapitem, 5 ) )  // iteminrange requires us to enter 3 parameters socket,item,distance for us thats player,mapitem,distance
							{
								realID = (items[mapitem].id1<<8) + items[mapitem].id2;
								if( realID >= 0x0DE3 && realID <= 0x0DEA )	// check for campfire
									fire = true; // valid fire type sets the fire variable to true
								else if( realID == 0x0A55 || realID == 0x0A56 )	// check for bedroll
									roll = true;  // valid bedroll sets the roll variable to true				

								if( roll && fire )
								{
									valid = 1; // Valid instalog location - Retalin 1/14/2001
									break;	   // Don't search any further  1/14/2001
								}
							}
						}
					}
				} while( mapitem != -1 ); 
				if( valid )
					break;  // stops the second mapregion check
			}//a<3
		}//for checkgrid
	}

	if (valid)// || region[chars[p].region].priv&0x17 )
	{
		inworld[chars[p].account]=-1;
		chars[p].logout=-1;
	} 
	else 
	{
		inworld[chars[p].account]=p;
		chars[p].logout = (SI32)(uiCurrentTime+server_data.quittime*CLOCKS_PER_SEC);
	}
	teleport(p);
}

int cNetworkStuff::Receive(int s, int x, int a) // Old socket receive function (To be replaced soon)
{
	//	int i, count;
	int count;
#ifdef DEBUG_NETWORK
	int j;
	int buf2c;
	char buf2[32];
#endif
	
	if (a) count=0; // Just to get rid of warning message
	do
	{
		count=recv(client[s], (char *)&buffer[s][recvcount], x-recvcount, 0);
		if (count>0)
		{
			recvcount+=count;
#ifdef DEBUG_NETWORK
			buf2c=0;
			if (((recvcount==x)||(x==2560))&&(a))
			{
				printf("*** xRecv\n");
				for (int i=0;i<recvcount;i++)
				{
					buf2[buf2c]=buffer[s][i];
					buf2c++;
					if ((buf2c==16)||(i==recvcount-1))
					{
						printf("C%i %2x : ",s,((i%256)/16)*16);
						for (j=0;j<buf2c;j++)
						{
							printf("%2x ",buf2[j]);
						}
						printf("[");
						for (j=0;j<buf2c;j++)
						{
							if ((isalnum(buf2[j]))||(isprint(buf2[j]))) printf("%c",buf2[j]); else printf(".");
						}
						buf2c=0;
						printf("]\n");
					}
				}
			}
#endif
		}
	}
	while ((x!=2560)&&(x!=recvcount)&&(count>0));
	return count;
}

void cNetworkStuff::sockInit( void )
{
	int i;
	char h1=0, h2=0, h3=0, h4=0;
	int bcode;
	
	kr=1;
	faul=0;
	
#ifdef __NT__
	wVersionRequested=MAKEWORD(2, 0);
	err=WSAStartup(wVersionRequested, &wsaData);
	if (err)
	{
		printf("\nERROR: Winsock 2.0 not found...\n");
		keeprun=0;
		error=1;
		kr=0;
		faul=1;
		Shutdown( FATAL_UOX3_ALLOC_NETWORK );
		return;
	}
#endif
	
	a_socket=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (a_socket<0)
	{
		printf("\nERROR: Unable to create socket ");
#ifdef __NT__
		printf("error code %i\n", WSAGetLastError());
#else
		printf("\n");
#endif
		keeprun=0;
		error=1;
		kr=0;
		faul=1;
		Shutdown( FATAL_UOX3_ALLOC_NETWORK );
		return;
	}
#ifndef __NT__
	int on = 1;
	setsockopt(a_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
#endif
	
	len_connection_addr=sizeof (struct sockaddr_in);
	memset((char *) &connection, 0, len_connection_addr);
	connection.sin_family=AF_INET;
	connection.sin_addr.s_addr=htonl(INADDR_ANY);
	connection.sin_port=htons(UOX_PORT);
	bcode=bind(a_socket, (struct sockaddr *)&connection, len_connection_addr);
	
	if (bcode<0)
	{
		printf("\nERROR: Unable to bind socket 1 - Error code: %i\n",bcode);
		keeprun=0;
		error=1;
		kr=0;
		faul=1;
		Shutdown( FATAL_UOX3_ALLOC_NETWORK );
		return;
	}
    
	
	listen(a_socket, 42);
	login03[1] = h1;
	login03[2] = h2;
	login03[3] = h3;
	login03[4] = h4;
	login03[7] = h1;
	login03[8] = h2;
	login03[9] = h3;
	login03[10] = h4;
	for( i = 0; i < servcount; i++ ) 
		if( serv[i][1][0] == '*' ) 
			sprintf( serv[i][1], "%i.%i.%i.%i", h1, h2, h3, h4 );
	ph1 = h1;
	ph2 = h2;
	ph3 = h3;
	ph4 = h4;
}

void cNetworkStuff::SockClose () // Close all sockets for shutdown
{
	int i;
	closesocket(a_socket);
	for (i=0;i<now;i++) closesocket(client[i]);
}

void cNetworkStuff::CheckConn( void ) // Check for connection requests
{
	int s;
	int len;
	
	if (now<MAXIMUM)
	{
		FD_ZERO(&conn);
		FD_SET(a_socket, &conn);
		nfds=a_socket+1;
		s=select(nfds, &conn, NULL, NULL, &uoxtimeout);
		if (s>0)
		{
			len=sizeof (struct sockaddr_in);
			//   printf("Waiting at accept()\n");
#ifndef __LINUX__
			client[now]=accept(a_socket, (struct sockaddr *)&client_addr, &len);
#else
			client[now]=accept(a_socket, (struct sockaddr *)&client_addr, (unsigned int *)&len );
#endif
			//   printf("Done! :)\n");
			if (client[now]<0)
			{
				printf("ERROR: Error at client connection!\n");
				error=1;
				keeprun=1;			// Leviathan suggested, Abaddon put in... old clients shut the server down
				return;
			}
			server[now]=-1;
			newclient[now]=1;
			acctno[now]=-1;
			addid1[now]=0;
			addid2[now]=0;
			addid3[now]=0;
			addid4[now]=0;
			perm[now]=0;
			binlength[now]=0;
			boutlength[now]=0;
			cryptclient[now]=0;
			walksequence[now]=-1;
			idleTimeout[now] = -1;
#ifdef __NT__
			printf("UOX3: Client %i [%i.%i.%i.%i] connected [Total:%i].\n",now,client_addr.sin_addr.S_un.S_un_b.s_b1 _ client_addr.sin_addr.S_un.S_un_b.s_b2 _ client_addr.sin_addr.S_un.S_un_b.s_b3 _ client_addr.sin_addr.S_un.S_un_b.s_b4,now+1);
#else
			printf("UOX3: Client %i connected [Total:%i].\n",now,now+1);
#endif
			now++;
			return;
		}
		if (s<0)
		{
			printf("ERROR: Select (Conn) failed!\n");
			keeprun=0;
			error=1;
			return;
		}
	}
}

cNetworkStuff::~cNetworkStuff()
{
	int i, s=0;
	for (i=0;i<now;i++)
	{
		FlushBuffer(i);
		closesocket(client[i]);
		s = max(s, client[i]+1);
	}
	closesocket(s);
#ifdef _WIN32
	WSACleanup();
#endif
}

void cNetworkStuff::CheckMessage( void ) // Check for messages from the clients
{
	int s, i, oldnow;
	
	FD_ZERO(&all);
	FD_ZERO(&errsock);
	nfds=0;
	for (i=0;i<now;i++)
	{
		FD_SET(client[i],&all);
		FD_SET(client[i],&errsock);
		if (client[i]+1>nfds) nfds=client[i]+1;
		if (server[i]>=0)
		{
			FD_SET(server[i],&all);
			FD_SET(server[i],&errsock);
			if (server[i]+1>nfds) nfds=server[i]+1;
		}
	}
	s=select(nfds, &all, NULL, &errsock, &uoxtimeout);
	if (s>0)
	{
		oldnow=now;
		for (i=0;i<now;i++)
		{
			if (FD_ISSET(client[i],&errsock))
			{
				Disconnect(i);
			}
			if (server[i] >= 0) {
				if (FD_ISSET(server[i],&errsock))
				{
					closesocket(server[i]);
					FailAuth(i);
				}
			}
			if ((FD_ISSET(client[i],&all))&&(oldnow==now))
			{
				GetMsg(i);
				if (executebatch) batchcheck(i); //LB, bugfix (uncommenting this line) for server info 
				// not working and much more scripted things
				// it's an odd place to do that check
				// feel free to place it straticaly better,
				// but DONT leave it out, that breaks a LOT !!!!
				// (whoever commented it out, it took me hours to find it, grrr)
				
				
			}
			if (server[i] >= 0)
			{
				if ((FD_ISSET(server[i],&all))&&(oldnow==now))
				{
					AuthTest(i);
				}
			}
		}
	}
}


cNetworkStuff::cNetworkStuff() // Initialize sockets
{
	sockInit();
}

int cNetworkStuff::Pack(void *pvIn, void *pvOut, int len)
{
	unsigned char *pIn = (unsigned char *)pvIn;
	unsigned char *pOut = (unsigned char *)pvOut;

	int actByte = 0;
	int bitByte = 0;
	int nrBits;
	unsigned int value;

	while(len--)
	{
		nrBits = bit_table[*pIn][0];
		value = bit_table[*pIn++][1];

		while(nrBits--)
		{
			pOut[actByte] = (pOut[actByte] << 1) | (unsigned char)((value >> nrBits) & 0x1);

			bitByte = (bitByte + 1) & 0x07;
			if(!bitByte) actByte++;
		}
	}

	nrBits = bit_table[256][0];
	value = bit_table[256][1];

	while(nrBits--)
	{
		pOut[actByte] = (pOut[actByte] << 1) | (unsigned char)((value >> nrBits) & 0x1);

		bitByte = (bitByte + 1) & 0x07;
		if(!bitByte) actByte++;
	}

	if(bitByte)
	{
		while(bitByte < 8)
		{
			pOut[actByte] <<= 1;
			bitByte++;
		}

		actByte++;
	}

	return actByte;
}