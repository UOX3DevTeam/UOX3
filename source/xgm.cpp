//------------------------------------------------------------------------
//  xgm.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1999 - 2001 by unknown real name (Zippy)
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

void cRemote::Verify(UOXSOCKET s)
{
	unsigned int len, i,a;
	char temp[512], name[64],pass[64], valid=0;

	recv(client[s],(char *)&buffer[s][5],2,0);
	len=(buffer[s][5]>>8)+(buffer[s][6]%256);
	recv(client[s],(char *)&buffer[s][7],len,0);
	strupr((char *)buffer[s]);

	a=0;
	for (i=7;i<71;i++)
	{
		if (buffer[s][i]!=0xFF && buffer[s][i] != 0 )
			name[a]=buffer[s][i];
		else {
			i++;
			break;
		}
		a++;
	}
	name[a] = 0;
	a=0;
	for (i=i;i<i+64;i++)
	{
		if (buffer[s][i]!=0xFF && buffer[s][i]!=0 )
			pass[a]=buffer[s][i];
		else break;
		a++;
	}
	pass[a] = 0;
	strupr( name ); strupr( pass );

	for (i=0;i<acctcount;i++)
	{
		strcpy(temp,acctx[i].name);
		strupr(temp);
		if (!(strcmp(temp,name)))
		{
			strcpy(temp,acctx[i].pass);
			strupr(temp);
			if (!(strcmp(temp,pass)) && (!acctx[i].ban))
			{
				valid=1;
				break;
			}
		}
	}

	if (!acctx[i].xGM) valid=0;

	if (valid==0) 
	{
		Send(s, "Account not valid!");
		isClient=0;
		Network->Disconnect(s);
		return;
	}

	Send(s, "Login sucessful!");
	Acct=i;
	isClient=1;
}

void cRemote::GMQue( UOXSOCKET s )
{
	char out[2048];
	char type;//0 = All Default (as player) | 1 = All Gm | 2 = All Counselor | 3 = Next Gm | 4 = Next Counselor

	int i, pos=3;

	recv(client[s],out,1,0);

	type = out[0];
		
	memset(out, 0, 2048);
	out[0] = (unsigned char)(0xBB);

	if (type == 0)
	{
		if (chars[currchar[ClientSock]].priv&1)
			type = 1;
		else if (chars[currchar[ClientSock]].priv&0x80 || chars[currchar[ClientSock]].priv&80)
			type = 2;
		else 
		{
			Send(s, "Access to Que denied.");
			return;
		}
	}
	

	switch(type)
	{
	case 1:
		for(i=1;i<MAXPAGES;i++)
		{
			if (gmpages[i].handled==0)
			{
				pos+=1+sprintf(&out[pos],"%s", gmpages[i].name);
				pos+=1+sprintf(&out[pos],"%s", gmpages[i].reason);
				//pos+=1+sprintf(&out[pos],"%x%x%x%x", gmpages[i].ser1, gmpages[i].ser2, gmpages[i].ser3, gmpages[i].ser4);
				
				out[pos] = gmpages[i].ser1;
				out[pos+1] = gmpages[i].ser2;
				out[pos+2] = gmpages[i].ser3;
				out[pos+3] = gmpages[i].ser4;
				pos+=4;
				pos+=1+sprintf(&out[pos],"%s", gmpages[i].timeofcall);
			}
		}
		break;
	
	case 2:
		for(i=1;i<MAXPAGES;i++)
		{
			if (counspages[i].handled==0)
			{
				pos+=1+sprintf(&out[pos],"%s", counspages[i].name);
				pos+=1+sprintf(&out[pos],"%s", counspages[i].reason);
				//pos+=1+sprintf(&out[pos],"%x%x%x%x", gmpages[i].ser1, gmpages[i].ser2, gmpages[i].ser3, gmpages[i].ser4);
				
				out[pos] = counspages[i].ser1;
				out[pos+1] = counspages[i].ser2;
				out[pos+2] = counspages[i].ser3;
				out[pos+3] = counspages[i].ser4;
				pos+=4;
				pos+=1+sprintf(&out[pos],"%s", counspages[i].timeofcall);
			}
		}
		break;
	case 3:
		for(i=1;i<MAXPAGES;i++)
		{
			if (gmpages[i].handled==0)
			{
				pos+=1+sprintf(&out[pos],"%s", gmpages[i].name);
				pos+=1+sprintf(&out[pos],"%s", gmpages[i].reason);
				//pos+=1+sprintf(&out[pos],"%x%x%x%x", gmpages[i].ser1, gmpages[i].ser2, gmpages[i].ser3, gmpages[i].ser4);
				
				out[pos] = gmpages[i].ser1;
				out[pos+1] = gmpages[i].ser2;
				out[pos+2] = gmpages[i].ser3;
				out[pos+3] = gmpages[i].ser4;
				pos+=4;
				pos+=1+sprintf(&out[pos],"%s", gmpages[i].timeofcall);
				break;
			}
		}
		break;
	case 4:
		for(i=1;i<MAXPAGES;i++)
		{
			if (counspages[i].handled==0)
			{
				pos+=1+sprintf(&out[pos],"%s", counspages[i].name);
				pos+=1+sprintf(&out[pos],"%s", counspages[i].reason);
				//pos+=1+sprintf(&out[pos],"%x%x%x%x", gmpages[i].ser1, gmpages[i].ser2, gmpages[i].ser3, gmpages[i].ser4);
				
				out[pos] = counspages[i].ser1;
				out[pos+1] = counspages[i].ser2;
				out[pos+2] = counspages[i].ser3;
				out[pos+3] = counspages[i].ser4;
				pos+=4;
				pos+=1+sprintf(&out[pos],"%s", counspages[i].timeofcall);
				break;
			}
		}
		break;
	default:
		Send(s, "Que Type %i unknown!", type);
		return;
	}//switch

	out[1] = pos >> 8;
	out[2] = pos %256;
	Send(s, out, pos);
}

void cRemote::CheckMsg(UOXSOCKET s)
{
	if (!isClient) return;
	if( recv( client[s], (char *)&buffer[s][0] ,1, 0 ) > 0 )
	{
		recv(client[s], (char *)&buffer[s][1], 1, 0);
		unsigned char len = 2, numPoint = 2;
		printf("Received a message from xgm client %i! (Message %x)\n", s, buffer[s][1] );

		// this could be sped up by passing the string length
		while( buffer[s][len] != '\0' )
		{
			len++;
		}
		while( numPoint < len && buffer[s][numPoint] != '\n' && buffer[s][numPoint] != ' ' && buffer[s][numPoint] != '\0' )
		{
			numPoint++;
		}
		numPoint -= 2;
		switch(buffer[s][1])
		{
		case 0x01:			// Save new world
			cwmWorldState->savenewworld(1);
			Send(s, "World data saved.");
			printf("World data saved. ");
			break;
		case 0x02:			// Save accounts
			cwmWorldState->saveAccount();
			Send(s, "Accounts Updated.");
			printf("Acounts Updated. ");
			break;
		case 0x03:			// Add item
			addmitem[ClientSock] = grabNumber( 2, numPoint, ClientSock ); // Anthracks' fix
			char temp[256];
			sprintf(temp, "Select location for item. [Number: %i]", addmitem[ClientSock]);
			target(ClientSock, 0, 1, 0, 26, temp);
			break;
		case 0x04:			// where am I?
			Send( s, "X %d Y %d Z %d", chars[currchar[ClientSock]].x, chars[currchar[ClientSock]].y, chars[currchar[ClientSock]].z );
			break;
		case 0x05:			// kill
			target( ClientSock, 0, 1, 0, 20, "Select character to kill." );
			break;
		case 0x06:
			all_items( ClientSock ); 
			teleport( currchar[ClientSock] );
			break;
		case 0x07:			// teleport
			target( ClientSock, 0, 1, 0, 2, "Select teleport target." );
			break;
		case 0x08:			// Make GM
			target( ClientSock, 0, 1, 0, 14, "Select character to make a GM." );
			break;
		case 0x09:			// Jail
			target( ClientSock, 0, 1, 0, 126, "Select player to jail." );
			break;
		case 0x0A:			// xbank
			target( ClientSock, 0, 1, 0, 107, "Select target to open bank of." );
			break;
		case 0x0B:
			GMQue(s);
			break;
		default:
			Send(s, "Sever did not recognize message %c", buffer[s][1]);
			printf("XGM: Unknown message %c\n",buffer[s][1]);
			return;
		}
	}
	else
	{
		Network->Disconnect( s );
	}
}

void __cdecl cRemote::Send(UOXSOCKET s, char *fmt, ...)
{
	int len;
	char *out, built[512];

	if (!isClient) return;
	va_list argptr;

	va_start(argptr, fmt);
	vsprintf(built, fmt, argptr);
	va_end(argptr);

	out = new char[strlen(built)+4];

	sprintf(out, "XGM%s", built );
	len=strlen(out);
	send(client[s],out,len,0);
	delete out;
}

/*void cRemote::Send( UOXSOCKET s, char *toSend )
{
	int len;

	if (!isClient) return;

	sprintf(toSend, "XGM%s", toSend );
	len=strlen(toSend);
	send(client[s],toSend,len,0);
}*/

void cRemote::Send( UOXSOCKET s, char *toSend, unsigned int len )
{
	if (!isClient) return;
	char *Out;
	Out=new char[len+3];
	Out[0]='X'; Out[1]='G'; Out[2]='M';

	memcpy(&Out[3], toSend, len);
	len+=3;
	send(client[s],Out,len,0);
}

SI32 grabNumber( int startPos, int length, UOXSOCKET s, char base )
// base defaults to 10, if you raise it, it is undefined (need to fix for hex)
// when called from CheckMsg, startPos == 2, length == numPoint
// Reads decimal numbers so convert from hex first!
{
	int i = 0;
	SI32 number = 0;
/*	char strToCheck[256];
	char *strPoint;
	strcpy( strToCheck, buffer[s] );
	strPoint = &strToCheck[startPos]; */

	for( i = 0; i < length; i++ )
	{
		number = base * number + ( buffer[s][startPos+i] - '0' );
	}
	return number;
}
