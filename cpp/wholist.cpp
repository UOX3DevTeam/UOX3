//------------------------------------------------------------------------
//  wholist.cpp
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 2000 - 2001 by Daniel Stratton (Abaddon)
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

void cWhoList::ResetUpdateFlag( void )
// PRE:		WhoList class is active
// POST:	Resets the update flag
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	needsUpdating = false;
	
}

cWhoList::cWhoList( bool trulyOnline )
// PRE:		On creation
// POST:	Initializes the variables of the wholist
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	FlagUpdate();					// first use will force it to update
	gmCount = 0;					// no GMs currently using it
	whoMenuData.resize( 0 );
	online = trulyOnline;
}

cWhoList::~cWhoList()
// PRE:		WhoList class is active
// POST:	Memory cleared/destroyed
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	ResetUpdateFlag();
	gmCount = 0;
	Delete();
}
void cWhoList::FlagUpdate( void )
// PRE:		WhoList class is active
// POST:	Flags the who list as needing to be updated
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	needsUpdating = true;

}
void cWhoList::SendSocket( UOXSOCKET toSendTo, unsigned char option )
// PRE:		WhoList class is active
// POST:	Sends the wholist to the socket, depending on option
//			0 = On/off line players
//			1 = Online players
//			2 = Offline players
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	GMEnter();
	if( needsUpdating && gmCount <= 1 )	// If we need to update, do it now while we still can
		Update();

	if( online )
		SendVecsAsGump( toSendTo, one, two, 4 );
	else
		SendVecsAsGump( toSendTo, one, two, 11 );
}

void cWhoList::GMEnter( void )
// PRE:		WhoList class is active
// POST:	Increments the GM counter
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	gmCount++;
}

void cWhoList::GMLeave( void )
// PRE:		WhoList class is active
// POST:	Decrements the GM counter
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	if( gmCount != 0 )
		gmCount--;
}

long int cWhoList::GrabSerial( int index )
// PRE:		WhoList class is active
// POST:	Returns serial as refered to by index
// CODER:	Abaddon
// DATE:	12th February, 2000
{
//	if( index >= onlineEntries.size() )
//		return offlineEntries[index - onlineEntries.size()]->player;
//	return onlineEntries[index]->player;
	return -1;
}

void cWhoList::Delete( void )
// PRE:		WhoList class is active
// POST:	Deletes all the entries
// CODER:	Abaddon
// DATE:	12th February, 2000
{
	unsigned int i = 0;
	unsigned int minSize;
	minSize = min( one.size(), two.size() );
	for( i = 0; i < minSize; i++ )
	{
		delete one[i];
		delete two[i];
	}
	for( i = minSize; i < one.size(); i++ )
		delete one[i];
	for( i = minSize; i < two.size(); i++ )
		delete two[i];
	one.resize( 0 );
	two.resize( 0 );

	whoMenuData.resize( 0 );

}

void cWhoList::AddSerial( SERIAL toAdd )
// PRE:		WhoList class is active
// POST:	Adds an entry into the whoMenuData array
// CODER:	Abaddon
// DATE:	23rd February, 2000
{
	int size = whoMenuData.size();
	whoMenuData.resize( size + 1 );
	whoMenuData[size] = toAdd;
}

void cWhoList::ButtonSelect( UOXSOCKET toSendTo, unsigned short int buttonPressed, unsigned char type )
// PRE:		WhoList class is active
// POST:	Does action based upon button selected
// CODER:	Abaddon
// DATE:	23rd February, 2000
{
	CHARACTER targetChar, sourceChar;
	sourceChar = currchar[toSendTo];

	if( buttonPressed < 200 )
	{		
		buttonPressed -= 7;
		chars[sourceChar].making = buttonPressed;
		Command( toSendTo, type, buttonPressed );
		return;
	}

	GMLeave();
	short int i = (short int)chars[sourceChar].making;

	if( i < 0 )
	{
		sysmessage( toSendTo, "Selected character not found" );
		return;
	}

	SERIAL charSerial = whoMenuData[i];
	int serhash = charSerial % HASHMAX;

	targetChar = findbyserial( &charsp[serhash], charSerial, 1 ); // find selected char ...
	if( targetChar == -1 )
	{
		sysmessage( toSendTo, "Selected character not found" );
		return;
	}
	switch( buttonPressed )
	{
	case 200://gochar
		if( chars[targetChar].commandLevel > chars[sourceChar].commandLevel )
		{
			sysmessage( toSendTo, "You cannot go to such powerful beings" );
			return;
		}
		mapRegions->RemoveItem( sourceChar + 1000000 );
		chars[sourceChar].x = chars[targetChar].x;
		chars[sourceChar].y = chars[targetChar].y;
		chars[sourceChar].z = chars[targetChar].z;
		chars[sourceChar].dispz = chars[targetChar].dispz;
		teleport( sourceChar );
		mapRegions->AddItem( sourceChar + 1000000 );
		break;
	case 201://xtele
		if( chars[targetChar].commandLevel > chars[sourceChar].commandLevel )
		{
			sysmessage( toSendTo, "Such powerful beings don't come on your whim!" );
			return;
		}
		mapRegions->RemoveItem( targetChar + 1000000 );
		chars[targetChar].x = chars[sourceChar].x;
		chars[targetChar].y = chars[sourceChar].y;
		chars[targetChar].z = chars[sourceChar].z;
		chars[targetChar].dispz = chars[sourceChar].dispz;
		teleport( targetChar );
		mapRegions->AddItem( targetChar + 1000000 );
		break;
	case 202://jail char
		if( sourceChar == targetChar )
		{
			sysmessage( toSendTo, "You cannot jail yourself!" );
			break;
		}
		else
		{
			if( chars[targetChar].commandLevel > chars[sourceChar].commandLevel )
			{
				sysmessage( toSendTo, "You cannot jail someone superior to you" );
				return;
			}
			Targ->JailTarget( toSendTo, chars[targetChar].serial );
			break;
		}						 
	case 203://release
		if( chars[targetChar].commandLevel > chars[sourceChar].commandLevel )
		{
			sysmessage( toSendTo, "You release someone more powerful than you" );
			return;
		}
		Targ->ReleaseTarget( toSendTo, chars[targetChar].serial );
		break;
	case 204:
		if(targetChar == sourceChar)
		{
			sysmessage( toSendTo, "You cannot kick yourself" );
			break; // lb
		}
		else
		{
			if( !::online( targetChar ) )	// local var overloads it
			{
				sysmessage( toSendTo, "You can't kick an offline player" );		// you realize the break isn't necessary?
				break;
			}
			if( chars[targetChar].commandLevel > chars[sourceChar].commandLevel )
			{
				sysmessage( toSendTo, "You cannot kick someone more powerful than you" );
				return;
			}
			sysmessage( toSendTo, "Kicking player" );
			Network->Disconnect( calcSocketFromChar( targetChar ) );
			break;
		}
	case 205:	// not active currently, a remote where
		// make it prettier later!
		if( chars[targetChar].commandLevel > chars[sourceChar].commandLevel )
		{
			sysmessage( toSendTo, "You cannot spy on your betters" );
			return;
		}
		sysmessage( toSendTo, "X: %i Y: %i Z: %i", chars[targetChar].x, chars[targetChar].y, chars[targetChar].z );
		break;
	case 206:	// remote cstats
		if( chars[targetChar].commandLevel > chars[sourceChar].commandLevel )
		{
			sysmessage( toSendTo, "You have not the right to see the intimate details of your betters" );
			return;
		}
		buffer[toSendTo][7]  = chars[targetChar].ser1;
		buffer[toSendTo][8]  = chars[targetChar].ser2;
		buffer[toSendTo][9]  = chars[targetChar].ser3;
		buffer[toSendTo][10] = chars[targetChar].ser4;
		Targ->CstatsTarget( toSendTo );
		break;
	case 207:	// remote tweak
		if( chars[targetChar].commandLevel > chars[sourceChar].commandLevel )
		{
			sysmessage( toSendTo, "You certainly cannot adjust those that are your betters!" );
			return;
		}
		buffer[toSendTo][7]  = chars[targetChar].ser1;
		buffer[toSendTo][8]  = chars[targetChar].ser2;
		buffer[toSendTo][9]  = chars[targetChar].ser3;
		buffer[toSendTo][10] = chars[targetChar].ser4;
		Targ->TweakTarget( toSendTo );
		break;
	default:
		printf("ERROR: Fallout of switch statement without default. wholist.cpp, ButtonSelect()\n"); //Morrolan
	}
}

void cWhoList::Command( UOXSOCKET toSendTo, unsigned char type, unsigned short int buttonPressed )
{
	int k;
	CHARACTER targetChar;
	SERIAL serial;
	int serhash;
	
	k = buttonPressed;
	serial = whoMenuData[buttonPressed];
	serhash = serial%HASHMAX;

	targetChar = findbyserial( &charsp[serhash], serial, 1 ); // find selected char ...

	if( targetChar == -1 )
	{
		sysmessage( toSendTo, "Selected character not found" );
		return;
	}
	
	stringList one, two;
	//--static pages
	one.push_back( new string( "nomove" ) );
	one.push_back( new string( "noclose" ) );
	one.push_back( new string( "page 0" ) );
	char temp[512];
	sprintf( temp,"resizepic 0 0 %i 260 340", 2600 );
	one.push_back( new string( temp ) );
	sprintf( temp, "button 30 300 %i %i 1 0 1", 4017, 4017 + 1); //OKAY
	one.push_back( new string( temp ) );
	sprintf( temp, "text 30 40 %i 0", 0 );           //text <Spaces from Left> <Space from top> <Length, Color?> <# in order>
	one.push_back( new string( temp ) );
	sprintf( temp, "text 30 60 %i 1", 0 );
	one.push_back( new string( temp ) );

	one.push_back( new string( "page 1" ) );

	sprintf( temp, "text 50 90 %i 2", 0 );	//goto text
	one.push_back( new string( temp ) );
	sprintf( temp, "text 50 110 %i 3", 0 );	//gettext
	one.push_back( new string( temp ) );
	sprintf( temp, "text 50 130 %i 4", 0 );	//Jail text
	one.push_back( new string( temp ) );
	sprintf( temp, "text 50 150 %i 5", 0 );	//Release text
	one.push_back( new string( temp ) );
	sprintf( temp, "text 50 170 %i 6", 0 );	//Kick user text
	one.push_back( new string( temp ) );
	sprintf( temp, "text 50 190 %i 7", 0 );
	one.push_back( new string( temp ) );
	sprintf( temp, "text 50 210 %i 8", 0 );
	one.push_back( new string( temp ) );
	sprintf( temp, "text 50 230 %i 9", 0 );
	one.push_back( new string( temp ) );
	sprintf( temp, "text 50 270 %i 10", 0 );
	one.push_back( new string( temp ) );
	sprintf( temp, "button 20 90 %i %i 1 0 200", 4005, 4005 + 1 ); //goto button
	one.push_back( new string( temp ) );
	sprintf( temp, "button 20 110 %i %i 1 0 201", 4005, 4005 + 1 ); //get button
	one.push_back( new string( temp ) );
	sprintf( temp, "button 20 130 %i %i 1 0 202", 4005, 4005 + 1 ); //Jail button
	one.push_back( new string( temp ) );
	sprintf( temp, "button 20 150 %i %i 1 0 203", 4005, 4005 + 1 ); //Release button
	one.push_back( new string( temp ) );
	sprintf( temp, "button 20 170 %i %i 1 0 204", 4005, 4005 + 1 ); //kick button
	one.push_back( new string( temp ) );
	sprintf( temp, "button 20 190 %i %i 1 0 205", 4005, 4005 + 1 ); //Where button
	one.push_back( new string( temp ) );
	sprintf( temp, "button 20 210 %i %i 1 0 206", 4005, 4005 + 1 ); //Cstats button
	one.push_back( new string( temp ) );
	sprintf( temp, "button 20 230 %i %i 1 0 207", 4005, 4005 + 1 ); //Tweak button
	one.push_back( new string( temp ) );


	sprintf( temp, "User %i selected (account %i)",buttonPressed, chars[targetChar].account );
	two.push_back( new string( temp ) );
	sprintf( temp, "Name: %s",chars[targetChar].name);   
	two.push_back( new string( temp ) );
	two.push_back( new string( "Goto Character") );
	two.push_back( new string( "Get Character") );
	two.push_back( new string( "Jail Character") );
	two.push_back( new string( "Release Character") );
	two.push_back( new string( "Kick Character") );
	two.push_back( new string( "Location Info" ) );
	two.push_back( new string( "Stats" ) );
	two.push_back( new string( "Tweak" ) );
	sprintf( temp, "Serial#[%x %x %x %x]", chars[targetChar].ser1, chars[targetChar].ser2, chars[targetChar].ser3, chars[targetChar].ser4 );   
	two.push_back( new string( temp ) );

	SendVecsAsGump( toSendTo, one, two, type );
	unsigned int i;
	for( i = 0; i < one.size(); i++ )
		delete one[i];
	for( i = 0; i < two.size(); i++ )
		delete two[i];
}

void cWhoList::ZeroWho( void )
{
	FlagUpdate();
	gmCount = 0;
}

void cWhoList::Update( void )
// PRE:		WhoList class is active
// POST:	If multiple GMs using, doesn't actually update (deferred update)
//			Otherwise updates the arrays, and resets the update flag
// CODER:	Abaddon
// DATE:	12th February, 2000
// REVISION 1.1 June 29th, 2000
//				Written to use SendVecsAsGump, as well as making it flexible enough to show offline players
{
	if( gmCount > 1 )
	{
		return;
	}
	// do code
	Delete();

	char temp[512];
	int i,k;
	int numPerPage = 13;
	unsigned int pagenum = 1, position = 40, linenum = 1, buttonnum = 7;
	
	k = now;
	int realCount = 1;
	//--static pages
	one.push_back( new string( "noclose" ) );
	one.push_back( new string( "page 0"  ) );
	sprintf( temp, "resizepic 0 0 %i 550 340", 2600 );	// modified by Xuri
	one.push_back( new string( temp ) );
	sprintf( temp, "button 250 15 %i %i 1 0 1", 4017, 4017 + 1 );
	one.push_back( new string( temp ) );
	sprintf( temp, "text 45 15 %i 0", 0 );
	one.push_back( new string( temp ) );

	sprintf( temp, "page %i", pagenum );
	one.push_back( new string( temp ) );

	sprintf( temp, "Wholist" );
	two.push_back( new string( temp ) );

	//--Start User List
	if( online )
	{
		for( i = 0; i < k; i++ )
		{
			if( perm[i] )
			{
				if( realCount > 0 && !(realCount%numPerPage) )
				{
					position = 40;
					pagenum++;
					sprintf(temp, "page %i", pagenum );
					one.push_back( new string( temp ) );
				}
				sprintf( temp, "text 50 %i %i %i", position, 0, linenum ); 
				one.push_back( new string( temp ) );
				sprintf( temp, "button 20 %i %i %i %i 0 %i", position, 4005, 4005 + 1, pagenum, buttonnum );
				one.push_back( new string( temp ) );

				sprintf( temp, " %s",chars[currchar[i]].name );
				two.push_back( new string( temp ) );
				AddSerial( chars[currchar[i]].serial );

				position += 20;
				linenum++;
				buttonnum++;
				realCount++;
			}
		}
		
		sprintf( temp, "Wholist [%i online]", realCount - 1 );
		*(two[0]) = temp;

		pagenum = 1; 
		k = realCount;
		for( i = 0; i < k; i += numPerPage )
		{
			sprintf( temp, "page %i", pagenum );
			one.push_back( new string( temp ) );
			if( i >= numPerPage )
			{
				sprintf( temp, "button 50 300 %i %i 0 %i", 4014, 4014 + 1, pagenum - 1 ); //back button
				one.push_back( new string( temp ) );
			}
			if( ( k > numPerPage ) && ( ( i + numPerPage ) < k ) )
			{
				sprintf( temp, "button 260 300 %i %i 0 %i", 4005, 4005 + 1, pagenum + 1 );
				one.push_back( new string( temp ) );
			}
			pagenum++;
		}
	}
	else
	{
		k = charcount;
		for( i = 0; i < k; i++ )
		{
			if( !chars[i].npc && !::online( i ) && chars[i].account != -1 )
			{
				if( realCount > 0 && !(realCount%numPerPage) )
				{
					position = 40;
					pagenum++;
					sprintf(temp, "page %i", pagenum );
					one.push_back( new string( temp ) );
				}
				sprintf( temp, "text 50 %i %i %i", position, 0, linenum ); 
				one.push_back( new string( temp ) );
				sprintf( temp, "button 20 %i %i %i %i 0 %i", position, 4005, 4005 + 1, pagenum, buttonnum );
				one.push_back( new string( temp ) );

				sprintf( temp, " %s (%s)", chars[i].name, chars[i].laston );
				two.push_back( new string( temp ) );
				AddSerial( chars[i].serial );

				position += 20;
				linenum++;
				buttonnum++;
				realCount++;
			}
		}
		
		sprintf( temp, "Wholist [%i offline]", realCount-1 );
		*(two[0]) = temp;

		pagenum = 1; 
		k = realCount;
		for( i = 0; i < k; i += numPerPage )
		{
			sprintf( temp, "page %i", pagenum );
			one.push_back( new string( temp ) );
			if( i >= numPerPage )
			{
				sprintf( temp, "button 50 300 %i %i 0 %i", 4014, 4014 + 1, pagenum - 1 ); //back button
				one.push_back( new string( temp ) );
			}
			if( ( k > numPerPage ) && ( ( i + numPerPage ) < k ) )
			{
				sprintf( temp, "button 260 300 %i %i 0 %i", 4005, 4005 + 1, pagenum + 1 );
				one.push_back( new string( temp ) );
			}
			pagenum++;
		}
	}		
	ResetUpdateFlag();
}
