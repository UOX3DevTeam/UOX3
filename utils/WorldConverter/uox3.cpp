/*

  Ultima Offline eXperiment III (UOX3)
  UO Server Emulation Program
  
	Copyright 1997, 98 by Marcus Rating (Cironian)
	
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
			
			  You can contact the author by sending email to <cironian@stratics.com>.
			  
*/
#ifdef __LINUX__
#include <errno.h>
#include <signal.h>
#endif

#include "uox3.h"

#include <queue>
#include <set>
#include <sstream>

using namespace std;

void savenewworld( void );
void saveoldworld( void );
void readwscline( FILE *wscfile, char *temp );
void readw3( FILE *wscfile, char *script1, char *script2, char *script3 );
void loadchar( FILE *wscfile );
void loaditem( FILE *wscfile );

//o---------------------------------------------------------------------------o
//|	Function	-	SI32 makeNum( const char *s )
//|	Programmer	-	seank
//o---------------------------------------------------------------------------o
//|	Purpose		-	Converts a string to an integer
//o---------------------------------------------------------------------------o
SI32 makeNum( const char *data )
{
	if( data == NULL )
		return 0;

	char o, h;
	// NOTE: You MUST leave ret as an unsigned 32 bit integer rather than signed, due to the stupidity of VC
	// VC will NOT cope with a string like "4294967295" if it's SI32, but will if it's UI32.  Noteably,
	// VC WILL cope if it's UI32 and the string is "-1".

	UI32 ret = 0;

	string s( data );
	istringstream ss( s );
	ss >> o;
	if( o == '0' )			// oct and hex both start with 0
	{
		ss >> h;
		if( h == 'x' || h == 'X' )
			ss >> hex >> ret >> dec;	// it's hex
		else
		{
			ss.unget();
			ss >> oct >> ret >> dec;	// it's octal
		}
	}
	else
	{
		ss.unget();
		ss >> dec >> ret;		// it's decimal
	}
	return ret;
}

void DeleItem( int i )
{
	if( !items[i].isFree() )
	{
		if( items[i].GetCont() == 0xFFFFFFFF )
			MapRegion->RemoveItem( &items[i] );
		else
		{
			items[i].SetCont( 0xFFFFFFFF );
			MapRegion->RemoveItem( &items[i] );
		}
		nitemsp.Remove( items[i].GetSerial(), i );

		items[i].SetOwner( 0xFFFFFFFF );
		items[i].SetFree( true );
		items[i].SetX( 20 );
		items[i].SetY( 50 );
		items[i].SetZ( 9 );

	}
	items.Delete( i );
}


#ifdef __LINUX__
	typedef void *HANDLE;
#endif

#undef DBGFILE
#define DBGFILE "uox3.cpp"

//	EviLDeD	-	June 21, 1999
//	Ok here is thread number one its a simple thread for the checkkey() function
int hstr2num(char *s) // Convert hex string to integer
{
	int n=0;
	
	for( UI32 i = 0; i < strlen( s ); i++ )
	{
		n*=16;
		if( isdigit(s[i] ) )
			n += ( s[i] - 48 ); // Convert char to number from 0 to 9
		if( s[i] >= 65 && s[i] <= 70 ) // Uppercase A-F
			n += ( s[i] - 55 );
		if( s[i] >= 97 && s[i] <= 102 ) // Lowercase A-F
			n += ( s[i] - 87 );
	}
	if( s[0] == '-' ) 
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

void gcollect( void ) // Remove items which were in deleted containers 
// remarks : Okay LB... I'll just keep re-writing it until someone shuts up.
{
	UI08 a;
	int i, j, removed, serial;
	SI32 rtotal = 0;
	bool idelete = false;
	
	printf( "| Performing Garbage Collection..." );
	do
	{
		removed=0;
		for( i = 0; i < itemcount; i++ )
		{
			if (!items[i].isFree() )
			{
				idelete = false;
				a = items[i].GetCont( 1 );
				serial = items[i].GetCont() ;
				if( serial != 0xFFFFFFFF )
				{
					idelete = true;
					if( a < 0x40 ) // container is a character...verify the character??
					{
						j = calcCharFromSer( serial );
						if( j != -1 )
						{
							if( !chars[j].isFree() )
								idelete = false;
						}
					} 
					else 
					{// find the container if there is one.
						j = calcItemFromSer( serial );
						if( j != -1 )
						{
							if( !items[j].isFree() )
								idelete = false;
						}
					}
				}
				if( idelete )
				{
					DeleItem( i );
					removed++;
				}
			}
		}
		rtotal += removed;
	} while (removed>0);
	
	
    printf( " Removed %i items.\n", rtotal );
}

// lb, check for bugged items and autocorrect
void item_test( void )
{
	int serial;
	printf( "| Starting item-consistency check...\n" );
	for( SI32 a = 0; a < itemcount; a++ )
	{
		if( !items[a].isFree() )
		{
			serial = items[a].GetSerial() ;
			if( serial == items[a].GetCont() && serial != 0xFFFFFFFF )
			{
				printf( "| ALERT ! item %s [%i] [serial: %i] has dangerous container value, autocorrecting", items[a].GetName(), a, items[a].GetSerial() );
				items[a].SetCont( 0xFFFFFFFF );
			}
			if( serial == items[a].GetOwner() && serial != 0xFFFFFFFF )
			{
				printf( "| ALERT ! item %s [%i] [serial: %i] has dangerous container value, autocorrecting", items[a].GetName(), a, items[a].GetSerial() );
				items[a].SetOwner( 0xFFFFFFFF );
			}
			if( serial == items[a].GetSpawn() && serial != 0 )
			{
				printf( "| ALERT ! item %s [%i] [serial: %i] has dangerous container value, autocorrecting", items[a].GetName(), a, items[a].GetSerial() );
				items[a].SetSpawn( 0xFFFFFFFF, a );
			}
		}
	}
	printf( "| Consistency check done!\n" );
}

ITEM packitem( CHARACTER p ) // Find packitem
{
	if( p == -1 ) 
		return -1;
	int i = chars[p].GetPackItem();
	
	if( i > -1 && i < imem && p > -1 && p < cmem )
	{
		if( items[i].GetCont() == chars[p].GetSerial() && items[i].GetLayer() == 0x15 )
			return chars[p].GetPackItem();
	}
	
	// - For some reason it's not defined, so go look for it.
	// convert this to a call to FindItemOnLayer???? (EviLDeD look at)
	int packItem = FindItemOnLayer( p, 0x15 );
	if( packItem != -1 )
	{
		chars[p].SetPackItem( packItem );
		return packItem;
	}
	return -1;
}

CItem *packitem( CChar *p )
{
	if( p == NULL ) 
		return NULL;
	CItem *i = p->GetPackItemObj();
	
	if( i != NULL )
	{
		if( i->GetCont() == p->GetSerial() && i->GetLayer() == 0x15)
		{
			return i;
		}
	}
	CItem *packItem = FindItemOnLayer( p, 0x15 );
	if( packItem != NULL )
	{
		p->SetPackItem( calcItemFromSer( packItem->GetSerial() ) );
		return packItem;
	}
	return NULL;
}

#ifdef __LINUX__
void illinst( int x = 0 ) //Thunderstorm linux fix
{
	sysbroadcast("Fatal Server Error! Bailing out - Have a nice day!");
	Console.Error( "Illegal Instruction Signal caught - attempting shutdown" );
	endmessage( x );
}

void aus( int signal )
// PARAM WARNING: signal never used
{
	Console.Error( "Server crash averted! Floating point exception caught." );
} 

#endif
void InitClasses( void )
{
	printf( "| Initializing classes...\n" );
	
	MapRegion = NULL;
	printf( "Classes prepared..." );

	// MAKE SURE IF YOU ADD A NEW ALLOCATION HERE THAT YOU FREE IT UP IN Shutdown(...)
	if(( MapRegion = new cMapRegion )	  == NULL ) Shutdown( FATAL_UOX3_ALLOC_MAPREGION );
	printf( "| Done loading classes!\n" );
	
}

void ResetVars( void )
{
	memset( erroredLayers, 0, sizeof( erroredLayers[0] ) * MAXLAYERS );
	imem = 0;
	cmem = 0;
	charcount = 0;
	itemcount = 0;
	charcount2 = 1;
	itemcount2 = 0x40000000;
}

int main( int argc, char *argv[] )
{
	if( argc != 2 )
	{
		printf( "Takes 1 parameters, a single number\n0 - old world -> new\n1 - new world -> old\n" );
		return 1;
	}

	int convType = atoi( argv[1] );

	errorLog = fopen( "error.log", "a+" );

	printf( "| Configuring UOX3!\n" );
	InitClasses();
	ResetVars();

	printf( "| Loading World now...\n" );
	if( convType == 0 )
	{
		loadoldworld();
		for( int i = 0; i < itemcount; i++ )
			items[i].PostLoadProcessing( i );
		for( int j = 0; j < charcount; j++ )
			chars[j].PostLoadProcessing( j );
		printf( "| Preparing to convert from old world format to new\n" );
	}
	else
	{
		loadnewworld();
		printf( "| Preparing to convert from new world format to old\n" );
	}
	printf( "| Done loading world!\n| Garbage collecting and verifying\n" );
	
	printf( "| Item count: %i\n| Character count: %i\n", itemcount, charcount );

	if( convType == 0 )
		savenewworld();
	else
		saveoldworld();

	printf( "| Written out\n| Item count: %i\n| Character count: %i\n", itemsWritten, charsWritten );
	while( !kbhit() )
	{
		Sleep( 100 );
	}
	for( int jk = 0; jk < MAXLAYERS; jk++ )
	{
		if( erroredLayers[jk] > 0 )
		{
			fprintf( errorLog, "There were %i errors on the layer %i\n", erroredLayers[jk], jk );
		}
	}
	fclose( errorLog );
	Shutdown( 0 );
	return( 0 );	
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
void Shutdown( SI32 retCode )
{
	delete MapRegion;
	exit( retCode );
}

int calcCharFromSer( UI08 ser1, UI08 ser2, UI08 ser3, UI08 ser4 )
{
	SERIAL serial = calcserial( ser1, ser2, ser3, ser4 );
	return calcCharFromSer( serial );
}

int calcCharFromSer( SERIAL serial )
{
	return ncharsp.FindBySerial( serial );
}

CChar *calcCharObjFromSer( SERIAL targSerial )
{
	CHARACTER targChar = calcCharFromSer( targSerial );
	if( targChar != -1 )
		return &chars[targChar];
	else
		return NULL;
}

int calcItemFromSer( UI08 ser1, UI08 ser2, UI08 ser3, UI08 ser4 )
{
	SERIAL serial = calcserial( ser1, ser2, ser3, ser4 );
	return calcItemFromSer( serial );
}

int calcItemFromSer( SERIAL ser ) // Added by Magius(CHE) (2)
{
	return nitemsp.FindBySerial( ser );
}

CItem *calcItemObjFromSer( SERIAL targSerial )
{
	ITEM targItem = calcItemFromSer( targSerial );
	if( targItem != -1 )
		return &items[targItem];
	else
		return NULL;
}

int FindItem( CChar *toFind, UI08 type )
// PRE:		CharStuff exists, toFind exists
// POST:	If item exists anywhere in their pack with type type or its on their body, return it
//			else return -1
{
	int packSearchResult = -1;

	for( ITEM toCheck = toFind->FirstItem(); !toFind->FinishedItems(); toCheck = toFind->NextItem() )
	{
		if( toCheck != -1 )
		{
			if( items[toCheck].GetType() == type )	// it's in our hand
				return toCheck;					// we've found the first occurance on the person!
			else if( items[toCheck].GetLayer() == 0x15 )	// could use packitem, but we're already in the same type of loop, so we'll check it ourselves
			{
				packSearchResult = SearchSubPackForItem( &items[toCheck], type );
				if( packSearchResult != -1 )
					return packSearchResult;
			}
		}
	}
	// if we haven't hit it by now, we won't hit it at all
	return -1;
}

int SearchSubPackForItem( CItem *toSearch, UI08 type )
// PRE:		CharStuff exists, toFind exists
// POST:	If item with type type exists anywhere in the specified pack or on their body, return it
//			else return -1
{
	int packSearchResult = -1;

	for( ITEM toCheck = toSearch->FirstItem(); !toSearch->FinishedItems(); toCheck = toSearch->NextItem() )
	{
		if( toCheck != -1 )
		{
			if( items[toCheck].GetType() == type )	// it's in our hand
				return toCheck;					// we've found the first occurance on the person!
			else if( items[toCheck].GetType() == 1 || items[toCheck].GetType() == 8 )	// search any subpacks, specifically pack and locked containers
			{ 
				packSearchResult = SearchSubPackForItem( &items[toCheck], type );
				if( packSearchResult != -1 )
					return packSearchResult;
			}
		}
	}
	// if we haven't hit it by now, we won't hit it at all
	return -1;
}

int FindItem( CChar *toFind, UI16 itemID )
// PRE:		CharStuff exists, toFind exists
// POST:	If item exists anywhere in their pack with id id1 id2 or its on their body, return it
//			else return -1
{
	int packSearchResult = -1;

	for( ITEM toCheck = toFind->FirstItem(); !toFind->FinishedItems(); toCheck = toFind->NextItem() )
	{
		if( toCheck != -1 )
		{
			if( items[toCheck].GetID() == itemID )	// it's in our hand
				return toCheck;					// we've found the first occurance on the person!
			else if( items[toCheck].GetLayer() == 0x15 )	// could use packitem, but we're already in the same type of loop, so we'll check it ourselves
			{
				packSearchResult = SearchSubPackForItem( &items[toCheck], itemID );
				if( packSearchResult != -1 )
					return packSearchResult;
			}
		}
	}
	// if we haven't hit it by now, we won't hit it at all
	return -1;
}

int SearchSubPackForItem( CItem *toSearch, UI16 itemID )
// PRE:		CharStuff exists, toFind exists
// POST:	If item with id id1 id2 exists anywhere in the specified pack or on their body, return it
//			else return -1
{
	int packSearchResult = -1;

	for( ITEM toCheck = toSearch->FirstItem(); !toSearch->FinishedItems(); toCheck = toSearch->NextItem() )
	{
		if( toCheck != -1 )
		{
			if( items[toCheck].GetID() == itemID )	// it's in our hand
				return toCheck;					// we've found the first occurance on the person!
			else if( items[toCheck].GetType() == 1 || items[toCheck].GetType() == 8 )	// search any subpacks, specifically pack and locked containers
			{ 
				packSearchResult = SearchSubPackForItem( &items[toCheck], itemID );
				if( packSearchResult != -1 )
					return packSearchResult;
			}
		}
	}
	// if we haven't hit it by now, we won't hit it at all
	return -1;
}

int FindItemOnLayer( CHARACTER toFind, UI08 layer )
// PRE:		CharStuff exists, toFind exists
// POST:	If item exists anywhere in their pack with id id1 id2 or its on their body, return it
//			else return -1
{
	CItem *found = chars[toFind].GetItemAtLayer( layer );
	if( found == NULL )
		return -1;
	return calcItemFromSer( found->GetSerial() );
}

CItem *FindItemOnLayer( CChar *toFind, UI08 layer )
// PRE:		CharStuff exists, toFind exists
// POST:	If item exists anywhere in their pack with id id1 id2 or its on their body, return it
//			else return -1
{
	return toFind->GetItemAtLayer( layer );
}

short DeleteItemsFromChar( CChar *toFind, short int itemID )
// removes all instances of itemID from the player
{
	int item = -1;
	short int deleteCount = 0;
	short int tempDelete;
	for( item = toFind->FirstItem(); !toFind->FinishedItems(); item = toFind->NextItem() )
	{
		if( item != -1 )
		{
			if( items[item].GetID() == itemID )
			{
				DeleItem( item );
				deleteCount++;
			}
			else if( items[item].GetLayer() == 0x15 )
			{
				tempDelete = DeleteItemsFromPack( &items[item], itemID );
				deleteCount += tempDelete;
			}
		}
	}
	return deleteCount;
}

short DeleteItemsFromPack( CItem *item, short int itemID )
{
	int itemToFind = -1;
	short int deleteCount = 0;
	short int tempDelete;
	for( itemToFind = item->FirstItem(); !item->FinishedItems(); itemToFind = item->NextItem() )
	{
		if( itemToFind != -1 )
		{
			if( items[itemToFind].GetID() == itemID )
			{
				DeleItem( itemToFind );
				deleteCount++;
			}
			else if( items[itemToFind].GetType() == 1 || items[itemToFind].GetType() == 8 )	// search any subpacks, specifically pack and locked containers
			{
				tempDelete = DeleteItemsFromPack( &items[itemToFind], itemID );
				deleteCount += tempDelete;
			}
		}
	}
	return deleteCount;
}

void ReadWorldTagData( ifstream &inStream, char *tag, char *data )
{
	char temp[1024];
	tag[0] = 0;
	data[0] = 0;
	bool bValid = false;
	while( !bValid )
	{
		if( inStream.eof() || inStream.fail() )
		{
			strcpy( tag, "o---o" );
			strcpy( data, "o---o" );
			return;
		}
		inStream.getline( temp, 1024 );
		if( temp[0] == 'o' && !strcmp( temp, "o---o" ) )
		{
			strcpy( tag, "o---o" );
			return;
		}
		if( temp[0] == '/' || temp[0] == 13 || temp[0] == 10 )	// invalid
			continue;
		char *equalOffset = strstr( temp, "=" );
		if( equalOffset == NULL )	// no pairing
			continue;
		strncpy( tag, temp, equalOffset - temp );
		tag[equalOffset - temp] = 0;
		strcpy( data, equalOffset + 1 );
		bValid = true;
	}
}

int MemCharFree( bool zeroSer )
{
	SI32 nChar = chars.New( zeroSer );
	if( charcount >= cmem ) //Lets ASSUME theres no more memory left instead of acctually checking all the items to find a free one.
		cmem++;
	return nChar;
}

void DeleteChar( CHARACTER k ) // Delete character
{
	ncharsp.Remove( chars[k].GetSerial(), k );
	
	chars[k].SetOwner( 0xFFFFFFFF );
	
	MapRegion->RemoveChar( &chars[k] );
	
	chars[k].setFree( true );
	chars[k].SetX( 20 );
	chars[k].SetY( 50 );
	chars[k].SetZ( 9 );
	chars[k].SetSummonTimer( 0 );
	chars.Delete( k );
}


int MemItemFree( UI08 itemType )
{
	SI32 nItem = -1;
	char memerr = 0;

	nItem = items.New( itemType );

	int slots = 4000;
	if( itemcount >= imem )
	{
		imem += slots;
	}

	return nItem;
}

void InitItem( ITEM nItem, bool ser )
{
	if( nItem == -1 ) 
		return;

	if( ser )
	{
		items[nItem].SetSerial( itemcount2 );
		nitemsp.AddSerial( itemcount2, nItem );
		itemcount2++;
	} 
	else 
	{
		items[nItem].SetSerial( INVALIDSERIAL );
	}
	if( nItem == itemcount ) 
		itemcount++;

}

void loadnewworld( void )
{
	cmem = 0;
	imem = 0;
	printf( "| Loading World and Guilds, Building Map Regions...\n" );
	charcount = 0;
	itemcount = 0;
	charcount2 = 1;
	itemcount2 = 0x40000000;
	MapRegion->Load();
}

void savenewworld( void )
{
	ncharsp.GarbageCollect();
	nitemsp.GarbageCollect();
	printf( "| SAVE: Starting automatic world data save, saving all items & chars now!\n" );
	MapRegion->Save();
	printf( "| SAVE: World save complete\n" );
}


void SaveChar( long i, FILE *cWsc )
{
	charsWritten++;
	int j;
	fprintf( cWsc, "SECTION CHARACTER %i\n", i);
	fprintf( cWsc, "{\n");
	fprintf( cWsc, "SERIAL %i\n", chars[i].GetSerial() );
	fprintf( cWsc, "NAME %s\n", chars[i].GetName() );
	if( chars[i].GetTitle() != NULL )
		fprintf(cWsc, "TITLE %s\n", chars[i].GetTitle() );
	fprintf( cWsc, "ACCOUNT %i\n", chars[i].GetAccount() );
	if( chars[i].GetLastOn() != NULL )
		fprintf(cWsc, "LASTON %s\n", chars[i].GetLastOn() );
	if( chars[i].GetX() )
		fprintf(cWsc, "X %i\n", chars[i].GetX() );
	if (chars[i].GetY() )
		fprintf(cWsc, "Y %i\n", chars[i].GetY() );
	if (chars[i].GetZ() )
		fprintf(cWsc, "Z %i\n", chars[i].GetZ() );
	if( chars[i].GetDispZ() )
		fprintf(cWsc, "DISPZ %i\n", chars[i].GetDispZ() );
	if( chars[i].GetOldX() )
		fprintf(cWsc, "OLDX %i\n", chars[i].GetOldX() );
	if( chars[i].GetOldY() )
		fprintf(cWsc, "OLDY %i\n", chars[i].GetOldY() );
	if( chars[i].GetOldZ() )
		fprintf(cWsc, "OLDZ %i\n", chars[i].GetOldZ() );
	if( chars[i].GetDir() )
		fprintf(cWsc, "DIR %i\n", chars[i].GetDir() );
	if( chars[i].GetID() )
		fprintf( cWsc, "BODY %i\n", chars[i].GetID() );
	if( chars[i].GetxID() )
		fprintf(cWsc, "XBODY %i\n", chars[i].GetxID() );
	if( chars[i].GetOrgID() )
		fprintf( cWsc, "ORGBODY %i\n", chars[i].GetOrgID() );
	if( chars[i].GetColour() )
		fprintf(cWsc, "SKIN %i\n", chars[i].GetColour() );
	if( chars[i].GetxSkin() )
		fprintf(cWsc, "XSKIN %i\n", chars[i].GetxSkin() );
	if( chars[i].GetPriv() )
		fprintf(cWsc, "PRIV %i\n", chars[i].GetPriv() );
	if( chars[i].CanRun() && chars[i].IsNpc() )				// Abaddon, running
		fprintf(cWsc, "RUNS\n" );
	
	if( chars[i].GetPriv2() )
		fprintf(cWsc, "ALLMOVE %i\n", chars[i].GetPriv2() );
	if( chars[i].GetFontType() )
		fprintf(cWsc, "FONT %i\n", chars[i].GetFontType() );
	if( chars[i].GetSayColour() )
		fprintf(cWsc, "SAY %i\n", chars[i].GetSayColour() );
	if( chars[i].GetEmoteColour() )
		fprintf(cWsc, "EMOTE %i\n", chars[i].GetEmoteColour() );
	if( chars[i].ActualStrength() )
		fprintf(cWsc, "STRENGTH %i\n", chars[i].ActualStrength() );
	if( chars[i].Strength2() )
		fprintf(cWsc, "STRENGTH2 %i\n", chars[i].Strength2() );
	if( chars[i].ActualDexterity() )
		fprintf(cWsc, "DEXTERITY %i\n", chars[i].ActualDexterity() );
	if( chars[i].Dexterity2() )
		fprintf(cWsc, "DEXTERITY2 %i\n", chars[i].Dexterity2() );
	if( chars[i].ActualIntelligence() )
		fprintf(cWsc, "INTELLIGENCE %i\n", chars[i].ActualIntelligence() );
	if( chars[i].Intelligence2() )
		fprintf(cWsc, "INTELLIGENCE2 %i\n", chars[i].Intelligence2() );
	if( chars[i].GetHP() )
		fprintf(cWsc, "HITPOINTS %i\n", chars[i].GetHP() );
	if( chars[i].GetStamina() )
		fprintf(cWsc, "STAMINA %i\n", chars[i].GetStamina() );
	if( chars[i].GetMana() )
		fprintf(cWsc, "MANA %i\n", chars[i].GetMana() );
	if( chars[i].IsNpc() )
		fprintf(cWsc, "NPC 1\n" );
	if( chars[i].IsShop() )
		fprintf(cWsc, "SHOP 1\n" );
	if( chars[i].GetOwner() )
		fprintf( cWsc, "OWN %i\n", chars[i].GetOwner() );
	if( chars[i].GetRobe() )
		fprintf( cWsc, "ROBE %i\n", chars[i].GetRobe() );
	if( chars[i].GetKarma() )
		fprintf(cWsc, "KARMA %i\n", chars[i].GetKarma() );
	if( chars[i].GetFame() )
		fprintf(cWsc, "FAME %i\n", chars[i].GetFame() );
	if( chars[i].GetKills() )
		fprintf(cWsc, "KILLS %i\n", chars[i].GetKills() );
	if( chars[i].GetDeaths() )
		fprintf(cWsc, "DEATHS %i\n", chars[i].GetDeaths() );
	if( chars[i].IsDead() )
		fprintf(cWsc, "DEAD 1\n" );
	if( chars[i].GetPackItem() )
		fprintf(cWsc, "PACKITEM %i\n", chars[i].GetPackItem() );
	if( chars[i].GetFixedLight() )
		fprintf(cWsc, "FIXEDLIGHT %i\n", chars[i].GetFixedLight() );
	if( chars[i].IsDisabled() )
		fprintf(cWsc, "DISABLED 1\n" );
	
	for( j = 0; j < TRUESKILLS; j++ )
	{
		if( chars[i].GetBaseSkill( j ) != 10 && chars[i].GetBaseSkill( j ) > 1 ) 
			fprintf( cWsc, "SKILL%i %i\n", j, chars[i].GetBaseSkill( j ) );
		if( chars[i].GetSkillLock( j ) <= 2 )
			fprintf(cWsc, "SKL%i %i\n", j, chars[i].GetSkillLock( j ) );
	}
	if( !chars[i].CanTrain() )
		fprintf(cWsc, "NOTRAIN\n");
	
	if( chars[i].GetCell() )
		fprintf(cWsc, "RESERVED1 %i\n", chars[i].GetCell() );
	if( chars[i].GetDef() )
		fprintf(cWsc, "DEF %i\n", chars[i].GetDef() );
	if (chars[i].GetLoDamage() )
		fprintf(cWsc, "LODAMAGE %i\n", chars[i].GetLoDamage() );
	if (chars[i].GetHiDamage() )
		fprintf(cWsc, "HIDAMAGE %i\n", chars[i].GetHiDamage() );
	if( chars[i].IsAtWar() )
		fprintf(cWsc, "WAR 1\n" );
	if( chars[i].GetNpcWander() )
		fprintf(cWsc, "NPCWANDER %i\n", chars[i].GetNpcWander() );
	if( chars[i].GetOldNpcWander() )
		fprintf(cWsc, "OLDNPCWANDER %i\n", chars[i].GetOldNpcWander() );
	if( chars[i].GetFx( 1 ) )
		fprintf(cWsc, "FX1 %i\n", chars[i].GetFx( 1 ) );
	if (chars[i].GetFy( 1 ) )
		fprintf(cWsc, "FY1 %i\n", chars[i].GetFy( 1 ) );
	if (chars[i].GetFz() )
		fprintf(cWsc, "FZ1 %i\n", chars[i].GetFz() );
	if (chars[i].GetFx( 2 ) )
		fprintf(cWsc, "FX2 %i\n", chars[i].GetFx( 2 ) );
	if( chars[i].GetFy( 2 ) )
		fprintf(cWsc, "FY2 %i\n", chars[i].GetFy( 2 ) );
	if( chars[i].GetSpawn() > -1 )
		fprintf( cWsc, "SPAWN %i\n", chars[i].GetSpawn() );
	if (chars[i].GetHidden() )
		fprintf(cWsc, "HIDDEN %i\n", chars[i].GetHidden() );
	if (chars[i].GetHunger() )
		fprintf(cWsc, "HUNGER %i\n", chars[i].GetHunger() );
	if (chars[i].GetNPCAiType() )
		fprintf(cWsc, "NPCAITYPE %i\n", chars[i].GetNPCAiType() );
	if (chars[i].GetSpAttack() )
		fprintf(cWsc, "SPATTACK %i\n", chars[i].GetSpAttack() );
	if (chars[i].GetSpDelay() )
		fprintf(cWsc, "SPADELAY %i\n", chars[i].GetSpDelay() );
	if (chars[i].GetTaming() )
		fprintf(cWsc, "TAMING %i\n", chars[i].GetTaming() );
	if (chars[i].GetSummonTimer() )
		fprintf(cWsc, "SUMMONTIMER %i\n", chars[i].GetSummonTimer() );
	if( chars[i].GetTown() )
		fprintf(cWsc, "TOWN %i\n", chars[i].GetTown() );
	if( chars[i].GetTownVote() )
		fprintf( cWsc, "TOWNVOTE %i\n", chars[i].GetTownVote() );
	if (chars[i].GetTownTitle() )
		fprintf(cWsc, "TOWNTITLE %i\n", chars[i].GetTownTitle() );
	if (chars[i].GetTownPriv() )
		fprintf(cWsc, "TOWNPRIV %i\n", chars[i].GetTownPriv() );
	if (chars[i].GetAdvObj() )
		fprintf(cWsc, "ADVOBJ %i\n", chars[i].GetAdvObj() );
	if (chars[i].GetPoison() )
		fprintf(cWsc, "POISON %i\n", chars[i].GetPoison() );
	if (chars[i].GetPoisoned() )
		fprintf(cWsc, "POISONED %i\n", chars[i].GetPoisoned() );
	if (chars[i].GetFleeAt() )
		fprintf(cWsc, "FLEEAT %i\n", chars[i].GetFleeAt() );
	if (chars[i].GetReattackAt() )
		fprintf(cWsc, "REATTACKAT %i\n", chars[i].GetReattackAt() );
	if (chars[i].GetSplit() )
		fprintf(cWsc, "SPLIT %i\n", chars[i].GetSplit() );
	if (chars[i].GetSplitChance() )
		fprintf(cWsc, "SPLITCHANCE %i\n", chars[i].GetSplitChance() );
	if (chars[i].GetGuildToggle() )
		fprintf(cWsc, "GUILDTOGGLE 1\n" );
	if (chars[i].GetGuildNumber() )
		fprintf(cWsc, "GUILDNUMBER %i\n", chars[i].GetGuildNumber() );
	if (chars[i].GetGuildTitle() )
		fprintf(cWsc, "GUILDTITLE %s\n", chars[i].GetGuildTitle() );  
	if (chars[i].GetGuildFealty() )
		fprintf(cWsc, "GUILDFEALTY %i\n", chars[i].GetGuildFealty() );
	if (chars[i].GetMurderRate() )			// used to be guildfealty
		fprintf(cWsc, "MURDERRATE %li\n",chars[i].GetMurderRate() );
	fprintf(cWsc, "RACE %i\n", chars[i].GetRace() );
	fprintf(cWsc, "RACEGATE %i\n", chars[i].GetRaceGate() );
	fprintf(cWsc, "COMMANDLEVEL %i\n", chars[i].GetCommandLevel() );	// command level
	if( chars[i].GetQuestType() )
		fprintf(cWsc, "QUESTTYPE %i\n", chars[i].GetQuestType() );	
	if( chars[i].GetQuestDestRegion() )
		fprintf(cWsc, "QUESTDESTREGION %i\n", chars[i].GetQuestDestRegion() );	
	if( chars[i].GetQuestOrigRegion() )
		fprintf(cWsc, "QUESTORIGREGION %i\n", chars[i].GetQuestOrigRegion() );	
	if( chars[i].GetHoldG() )
		fprintf( cWsc, "HOLDG %i\n", chars[i].GetHoldG() );

	if ( !(chars[i].IsNpc() || chars[i].IsGM() || chars[i].IsCounselor() ) )
	{
		fprintf( cWsc, "ATROPHY" );
		for ( j=0;j<ALLSKILLS;j++ )
		{
			if ( chars[i].GetAtrophy( j ) >= 10 )
				fprintf(cWsc, " %i", chars[i].GetAtrophy( j ) );
			else 
				fprintf(cWsc, " 0%i", chars[i].GetAtrophy( j ) );
		}
		fprintf(cWsc, "\n");
	}
	
	fprintf(cWsc, "}\n\n");
}

void SaveItem( long i, FILE *iWsc )
{
	itemsWritten++;
	fprintf( iWsc, "SECTION WORLDITEM %i\n", i);
	fprintf( iWsc, "{\n");
	fprintf( iWsc, "SERIAL %i\n", items[i].GetSerial() );
	fprintf( iWsc, "NAME %s\n", items[i].GetName() );
	if (strlen(items[i].GetName2() )>0)
		fprintf(iWsc, "NAME2 %s\n", items[i].GetName2() );
	if( strlen( items[i].GetCreator() ) > 0 ) 
		fprintf(iWsc, "CREATOR %s\n", items[i].GetCreator() ); // by Magius(CHE)
	if( items[i].GetMadeWith() ) 
		fprintf( iWsc, "SK_MADE %i\n", items[i].GetMadeWith() ); // by Magius(CHE)
	
	fprintf(iWsc, "ID %i\n", items[i].GetID() );
	
	if (items[i].GetX() )
		fprintf(iWsc, "X %i\n", items[i].GetX());
	if (items[i].GetY() )
		fprintf(iWsc, "Y %i\n", items[i].GetY());
	if (items[i].GetZ() )
		fprintf(iWsc, "Z %i\n", items[i].GetZ());
	if( items[i].GetColour() )
		fprintf(iWsc, "COLOR %i\n", items[i].GetColour() );
	if( items[i].GetCont() )
		fprintf( iWsc, "CONT %i\n", items[i].GetCont() );
	if( items[i].GetLayer() )
		fprintf(iWsc, "LAYER %i\n", items[i].GetLayer() );
	if( items[i].GetItmHand() )
		fprintf( iWsc, "ITEMHAND %i\n", items[i].GetItmHand() );
	if (items[i].GetType() )
		fprintf(iWsc, "TYPE %i\n", items[i].GetType() );
	if (items[i].GetType2() )
		fprintf(iWsc, "TYPE2 %i\n", items[i].GetType2() );
	if (items[i].GetOffSpell() )
		fprintf(iWsc, "OFFSPELL %i\n", items[i].GetOffSpell() );
	if( items[i].GetMore() )
		fprintf(iWsc, "MORE %i\n", items[i].GetMore() );
	if( items[i].GetMoreB() )
		fprintf(iWsc, "MORE2 %i\n", items[i].GetMoreB() );
	if( items[i].GetMoreX() )
		fprintf(iWsc, "MOREX %i\n", items[i].GetMoreX() );
	if (items[i].GetMoreY() )
		fprintf(iWsc, "MOREY %i\n", items[i].GetMoreY() );
	if (items[i].GetMoreZ() )
		fprintf(iWsc, "MOREZ %i\n", items[i].GetMoreZ() );
	if (items[i].GetAmount() )
		fprintf(iWsc, "AMOUNT %i\n", items[i].GetAmount() );
	if (items[i].isPileable() )
		fprintf(iWsc, "PILEABLE 1\n" );
	if( items[i].GetDoorDir() )
		fprintf(iWsc, "DOORFLAG %i\n", items[i].GetDoorDir() );
	if (items[i].isDyeable() )
		fprintf(iWsc, "DYEABLE 1\n" );
	if (items[i].isCorpse() )
		fprintf(iWsc, "CORPSE 1\n" );
	if (items[i].GetDef() )
		fprintf(iWsc, "DEF %i\n", items[i].GetDef() );
	if (items[i].GetHiDamage() )
		fprintf(iWsc, "HIDAMAGE %i\n", items[i].GetHiDamage() );
	if (items[i].GetLoDamage() )
		fprintf(iWsc, "LODAMAGE %i\n", items[i].GetLoDamage() );
	if (items[i].GetStrength() )
		fprintf(iWsc, "ST %i\n", items[i].GetStrength() );
	if (items[i].GetWeight() )
		fprintf(iWsc, "WEIGHT %i\n", items[i].GetWeight() );
	if (items[i].GetHP() )
		fprintf(iWsc, "HP %i\n", items[i].GetHP() );
	if( items[i].GetMaxHP() )
		fprintf( iWsc, "MAXHP %i\n", items[i].GetMaxHP() );
	if( items[i].GetRank() )
		fprintf( iWsc, "RANK %i\n", items[i].GetRank() );
	if (items[i].Strength2() )
		fprintf(iWsc, "ST2 %i\n", items[i].Strength2() );
	if (items[i].GetDexterity() )
		fprintf(iWsc, "DX %i\n", items[i].GetDexterity() );
	if (items[i].Dexterity2() )
		fprintf(iWsc, "DX2 %i\n", items[i].Dexterity2() );
	if (items[i].GetIntelligence() )
		fprintf(iWsc, "IN %i\n", items[i].GetIntelligence() );
	if (items[i].Intelligence2() )
		fprintf(iWsc, "IN2 %i\n", items[i].Intelligence2() );
	if (items[i].GetSpeed() )
		fprintf(iWsc, "SPD %i\n", items[i].GetSpeed() );
	if (items[i].GetPoisoned() )
		fprintf(iWsc, "POISONED %i\n", items[i].GetPoisoned() );
	if( items[i].GetMagic() )
		fprintf(iWsc, "MOVABLE %i\n", items[i].GetMagic() );
	if( items[i].GetOwner() != 0xFFFFFFFF )
		fprintf(iWsc, "OWNER %i\n", items[i].GetOwner() );
	if( items[i].GetVisible() )
		fprintf(iWsc, "VISIBLE %i\n", items[i].GetVisible() );
	if( items[i].GetSpawn() != 0xFFFFFFFF )
		fprintf( iWsc, "SPAWN %i\n", items[i].GetSpawn() );
	if (items[i].GetDir() )
		fprintf(iWsc, "DIR %i\n", items[i].GetDir() );
	if (items[i].GetPriv() )
		fprintf(iWsc, "PRIV %i\n", items[i].GetPriv() );
	if (items[i].GetValue() )
		fprintf(iWsc, "VALUE %i\n", items[i].GetValue() );
	if (items[i].GetRestock() )
		fprintf(iWsc, "RESTOCK %i\n", items[i].GetRestock() );
	if (items[i].IsDisabled() )
		fprintf(iWsc, "DISABLED 1\n" );
	if (items[i].GetGood() >= 0 )
		fprintf(iWsc, "GOOD %i\n", items[i].GetGood() );
	if( items[i].GetGlow() )
		fprintf( iWsc, "GLOW %i\n", items[i].GetGlow() );
	if( items[i].GetGlowColour() )
		fprintf( iWsc, "GLOWBC %i\n", items[i].GetGlowColour() );
	if( items[i].GetGlowEffect() )
		fprintf( iWsc, "GLOWTYPE %i\n", items[i].GetGlowEffect() );
	if( items[i].GetRace() != 65535 )
		fprintf(iWsc, "RACE %i\n", items[i].GetRace() );
	if( strlen( items[i].GetDesc() ) > 0 )
		fprintf( iWsc, "DESC %s\n", items[i].GetDesc() );	// save out our vendor description
	
	fprintf(iWsc, "}\n\n");
}


void saveoldworld( void )
{
	FILE *cWsc = NULL, *iWsc = NULL;
	printf( "SAVE: Starting automatic world data save\n" );
	cWsc = fopen( "chars.wsc", "w" );
	if( cWsc ) 
	{
		fprintf( cWsc, "//o--------------------------------------------------------------------\n");
		fprintf( cWsc, "//| UOX3 - Character World Script \n" );
		fprintf( cWsc, "//| Generated by UOX3 Version 0.95 or above\n" );
		fprintf( cWsc, "//o--------------------------------------------------------------------\n");
		fprintf( cWsc, "INITMEM %i //Do NOT edit this line!\n\n", chars.Count() + 1 );
	} 
	else 
	{
		printf("Error, couldn't open chars.wsc for writing. Check file permissions.\n");
		return;
	}

	iWsc = fopen( "items.wsc", "w" );
	if( iWsc )
	{
		fprintf( iWsc, "//o------------------------------------------------------------------------------\n");
		fprintf( iWsc, "//| UOX3 - Item World Script\n");
		fprintf( iWsc, "//| Generated by UOX3 Version 0.95 or above\n" );
		fprintf( iWsc, "//o------------------------------------------------------------------------------\n");
		fprintf( iWsc, "INITMEM %i //Do NOT edit this line!\n\n", items.Count() + 1 );
	} 
	else 
	{
		printf("Error, couldn't open items.wsc for writing. Check file permissions.\n");
		return;
	}

	int oiCount = itemcount;
	int ocCount = charcount;
	int Cur = 0;
	int Max = max( oiCount, ocCount );

	for( ; Cur<Max ; Cur++ )
	{
		if( Cur < ocCount )
			SaveChar( Cur, cWsc );

		if( Cur < oiCount )
			SaveItem( Cur, iWsc );
	}	// end for loop

	fprintf( cWsc, "\nEOF\n\n" );
	fprintf( iWsc, "\nEOF\n\n" );

	fclose( cWsc );
	fclose( iWsc );

	iWsc = cWsc = NULL;
}

void loadoldworld( void )
{
	char script1[512], script2[512], script3[512];

	char outper[4] = { 0, };
	unsigned int i=0;
	unsigned int percent = 0, a = 0, pred = 0, maxm = 0;
	cmem = 0;
	imem = 0;
	printf( "Loading World, Building map Regions, checking Item weight...\n" );
	charcount = 0;
	itemcount = 0;
	charcount2 = 1;
	itemcount2 = 0x40000000;
	
	FILE *wscfile = fopen( "chars.wsc", "r" );
	if( wscfile == NULL )
	{
		printf( "WARNING: Chars.wsc not found. ABORTING\n" );
		Shutdown( 1 );
	} 
	else 
	{	//Get number for inital character memory needed ->
		readw3( wscfile, script1, script2, script3 );
		if( !strcmp( script1, "INITMEM" ) )
			cmem = atoi( script2 );
		maxm = cmem;
		if( cmem < 100 ) 
			cmem = 100;
		
		printf( "Allocating inital dynamic Character memory of %i... ", cmem );
		
		chars.Reserve( cmem );
		printf( "Done\n" );
		
		printf("  Loading characters ");
		do
		{
			readw3( wscfile, script1, script2, script3 );
			if( !strcmp( script1, "SECTION" ) )
			{
				if( !strcmp( script2, "CHARACTER" ) )
				{
					loadchar( wscfile );
					a++;
					pred = percent;
					if( maxm <= 1 )
						percent = 100;
					else
						percent = (int)(a*100)/(maxm-1);
					if( strlen( outper ) > 0 ) 
					{
						for( i = 1; i <= strlen( outper ) + 1; i++ ) 
							printf("\b" );
						outper[0] = 0;
					}
				}
			}
			if( percent > pred )
			{
				numtostr( percent, outper );
				printf( "%s%%", outper );
				pred = percent;
			}
		} while( strcmp( script1, "EOF" ) && !feof( wscfile ) );
		fclose( wscfile );
		wscfile = NULL;
		if( strlen( outper ) > 0 ) 
		{
			for( i = 1; i <= strlen( outper ) + 1; i++ ) 
				printf( "\b" );
			outper[0] = 0;
		}
		printf( "Done.\n" );
		wscfile = fopen( "items.wsc", "r" );
		if( wscfile == NULL )
		{
			printf("ERROR: Items.wsc not found. No items will be loaded.\n");
		} 
		else 
		{
			readw3( wscfile, script1, script2, script3 );
			if( !strcmp( script1, "INITMEM" ) ) 
				imem = atoi( script2 );
			maxm = imem; 
			if( imem < 100 ) 
				imem = 100;
			
			printf( "Allocating inital dynamic Item memory of %i... ", imem );
			items.Reserve( imem );
			printf( "Done\n" );
			
			a = 0;
			printf("  Loading items ");
			do 
			{
				readw3( wscfile, script1, script2, script3 );
				if( !strcmp( script1, "SECTION" ) )
				{
					if( !strcmp( script2, "WORLDITEM" ) ) 
					{
						loaditem( wscfile );
					}
					a++;
					pred = percent;
					if( maxm <= 1 )
						percent = 100;
					else
						percent = (int)(a*100)/(maxm - 1);
					if( strlen( outper ) > 0 ) 
					{
						for( i = 1; i <= strlen( outper ) + 1; i++ ) 
							printf("\b" );
						*outper = '\0';
					}
				}
				if( percent > pred )
				{
					numtostr( percent, outper );
					printf("%s%%", outper );
					pred = percent;
				}
				
			} while( strcmp( script1, "EOF" ) && !feof( wscfile ) );
			fclose( wscfile );
			wscfile = NULL;
		}
		if( strlen( outper ) > 0 ) 
		{
			for( i = 1; i <= strlen( outper ) + 1; i++ ) 
				printf( "\b" );
			*outper = '\0';
		}

		for( UI32 itemidx = 0; itemidx < items.Count(); itemidx++ )
			items[itemidx].SetCont( items[itemidx].GetContSerial() );

		printf( "Done.\n" );
		printf( "World Loaded.\n" );
		return;
	}
}

void readw3( FILE *wscfile, char *script1, char *script2, char *script3 )
{
	char temp[1024];
	int i = 0, j;
	readwscline( wscfile, temp );
	script1[0] = 0;
	script2[0] = 0;
	script3[0] = 0;
	while( temp[i] != 0 && temp[i] != ' ' && temp[i] != '=' && i < 1024 ) 
		i++;
	strncpy( script1, temp, i );
	script1[i] = 0;             // terminate with null
	if( script1[0] == '}' || temp[i] == 0 ) 
		return;
	i++;
	j = i;
	while( temp[i] != 0 && temp[i] != ' ' && temp[i] != '=' && i < 1024 ) 
		i++;
	strncpy( script2, temp + j, i - j );
	script2[i-j] = 0;
	strcpy( script3, temp + i + 1 );
}

void readwscline( FILE *wscfile, char *temp )
{
	bool valid = false;
	int i;
	char c;
	temp[0] = 0;
	while( !valid )
	{
		i = 0;
		if( feof( wscfile ) ) 
			return;
		c = (char)fgetc( wscfile );
		while( c != 10 )
		{
			if( c != 13 )
				temp[i++] = c;
			if( feof( wscfile ) ) 
				return;
			c = (char)fgetc( wscfile );
		}
		temp[i] = 0;
		valid = true;
		if( temp[0] == '/' && temp[1] == '/' ) 
			valid = false;
		if( temp[0] == '{' )
			valid = false;
		if( temp[0] == 0 )
			valid = false;
		if( temp[0] == 10 )
			valid = false;
		if( temp[0] == 13 )
			valid = false;
	}
}

void readw2( FILE *wscfile, char *script1, char *script2 )
{
	int i = 0;
	char temp[1024];
	readwscline( wscfile, temp );
	script1[0] = 0;
	script2[0] = 0;
	while( temp[i] != 0 && temp[i] != ' ' && temp[i] != '=' ) 
		i++;
	strncpy( script1, temp, i );
	script1[i] = 0;				 // terminate with null
	if( script1[0] != '}' && temp[i] != 0 ) 
		strcpy( script2, temp + i + 1 );
	return;
}

void loadatrophy( CHARACTER c, char *astr )
{
	char a[3];
	UI32 l, n = 0, sl = strlen( astr );

	for ( l = 0; l < sl;l += 3 )
	{
		a[0] = astr[l];
		a[1] = astr[l+1];
		a[2] = 0;
		chars[c].SetAtrophy( atoi( a ), n );
		n++;
	}
}

void loadchar( FILE *wscfile )
{
	char script1[512], script2[512];

	UI32 i;
	int a = 0, loops = 0;
	char newpoly = 0;

	CHARACTER x = MemCharFree( true );
	if( x == -1 ) 
		return;

	signed char lockstate = 0;
	int skl = 0;
	newpoly = 0; // this goes with "ORGBODY" but its setting a default up front
	do
	{
		readw2( wscfile, script1, script2 );
		switch( script1[0] )
		{
		case 'a':
		case 'A':
			if( !strcmp( script1, "ACCOUNT" ) ) chars[x].SetAccount( atoi( script2 ) ); 
			else if( !strcmp( script1, "ADVOBJ" ) ) chars[x].SetAdvObj( atoi( script2 ) ); 
			else if( !strcmp( script1, "ALLMOVE" ) ) chars[x].SetPriv2( atoi( script2 ) );
			else if( !strcmp( script1, "ATT" ) ) { chars[x].SetLoDamage( atoi( script2 ) ); chars[x].SetHiDamage( atoi( script2 ) ); }
			else if( !strcmp( script1, "ATROPHY" ) ) loadatrophy( x, script2 );
			break;
		case 'b':
		case 'B':
			if( !strcmp( script1, "BODY" ) )
			{
				i = atoi( script2 );
				chars[x].SetID( i );
				chars[x].SetOrgID( i );
			}
			break;
		case 'c':
		case 'C':
			if( !strcmp( script1, "COMMANDLEVEL" ) )
				chars[x].SetCommandLevel( atoi( script2 ) );
			break;
		case 'd':
		case 'D':
			if( !strcmp( script1, "DEAD" ) ) chars[x].SetDead( atoi( script2 ) != 0 );
			else if( !strcmp( script1, "DEATHS" ) ) chars[x].SetDeaths( atoi( script2 ) );
			else if( !strcmp( script1, "DEF" ) ) chars[x].SetDef( atoi( script2 ) );
			else if( !strcmp( script1, "DEXTERITY" ) ) chars[x].SetDexterity( atoi( script2 ) );
			else if( !strcmp( script1, "DEXTERITY2" ) ) chars[x].Dexterity2( atoi( script2 ) );
			else if( !strcmp( script1, "DIR" ) ) chars[x].SetDir( atoi( script2 ) & 0x0F );
			else if( !strcmp( script1, "DISABLED" ) ) chars[x].SetDisabled( atoi( script2 ) != 0 );
			else if( !strcmp( script1, "DISPZ" ) ) chars[x].SetDispZ( atoi( script2 ) );
			break;
		case 'e':
		case 'E':
			if( !strcmp( script1, "EMOTE" ) )
				chars[x].SetEmoteColour( atoi( script2 ) );
			break;
		case 'f':
		case 'F':
			if( !strcmp( script1, "FAME" ) ) chars[x].SetFame( atoi( script2 ) );
			else if( !strcmp( script1, "FIXEDLIGHT" ) ) chars[x].SetFixedLight( atoi( script2 ) ); 
			else if( !strcmp( script1, "FLEEAT" ) ) chars[x].SetFleeAt( atoi( script2 ) );
			else if( !strcmp( script1, "FONT" ) ) chars[x].SetFontType( atoi( script2 ) );
			else if( !strcmp( script1, "FX1" ) ) chars[x].SetFx( atoi( script2 ), 1 );
			else if( !strcmp( script1, "FY1" ) ) chars[x].SetFy( atoi( script2 ), 1 );
			else if( !strcmp( script1, "FZ1" ) ) chars[x].SetFz( atoi( script2 ) );
			else if( !strcmp( script1, "FX2" ) ) chars[x].SetFx( atoi( script2 ), 2 );
			else if( !strcmp( script1, "FY2" ) ) chars[x].SetFy( atoi( script2 ), 2 );
			break;
		case 'g':
		case 'G':
			// Begin of Guild related character stuff
			if( !strcmp( script1, "GUILDTOGGLE" ) ) chars[x].SetGuildToggle( atoi( script2 ) != 0 ); 
			else if( !strcmp( script1, "GUILDNUMBER" ) ) chars[x].SetGuildNumber( atoi( script2 ) ); 
			else if( !strcmp( script1, "GUILDTITLE" ) ) chars[x].SetGuildTitle( script2 ); 
			else if( !strcmp( script1, "GUILDFEALTY" ) ) chars[x].SetGuildFealty( atoi( script2 ) );
			// End of guild stuff
			break;
		case 'h':
		case 'H':
			if( !strcmp( script1, "HIDAMAGE" ) ) chars[x].SetHiDamage( atoi( script2 ) );
			else if( !strcmp( script1, "HIDDEN" ) ) chars[x].SetHidden( atoi( script2 ) );
			else if( !strcmp( script1, "HITPOINTS" ) ) chars[x].SetHP( atoi( script2 ) );
			else if( !strcmp( script1, "HUNGER" ) ) chars[x].SetHunger( atoi( script2 ) );
			else if( !strcmp( script1, "HOLDG" ) ) chars[x].SetHoldG( atoi( script2 ) );
			break;
		case 'i':
		case 'I':
			if( !strcmp( script1, "INTELLIGENCE" ) ) chars[x].SetIntelligence( atoi( script2 ) );
			else if( !strcmp( script1, "INTELLIGENCE2" ) ) chars[x].Intelligence2( atoi( script2 ) );
			break;
		case 'k':
		case 'K':
			if( !strcmp( script1, "KARMA" ) ) chars[x].SetKarma( atoi( script2 ) );
			else if( !strcmp( script1, "KILLS" ) ) chars[x].SetKills( atoi( script2 ) );
			break;
			
		case 'l':
		case 'L':
			if( !strcmp( script1, "LASTON" ) ) 
				chars[x].SetLastOn( script2 ); //load last time character was on
			else if( !strcmp( script1, "LODAMAGE" ) ) chars[x].SetLoDamage( atoi( script2 ) ); 
			break;
			
		case 'm':
		case 'M':
			if( !strcmp( script1, "MANA" ) ) chars[x].SetMana( atoi( script2 ) );
			else if( !strcmp( script1, "MURDERRATE" ) ) chars[x].SetMurderRate( atoi( script2 ) );
			break;
			
		case 'n':
		case 'N':
			if( !strcmp( script1, "NAME" ) ) chars[x].SetName( script2 );
			else if( !strcmp( script1, "NOTRAIN" ) ) chars[x].SetCanTrain( false );
			else if( !strcmp( script1, "NPC" ) ) chars[x].SetNpc( atoi( script2 ) != 0 );
			else if( !strcmp( script1, "NPCAITYPE" ) ) chars[x].SetNPCAiType( atoi( script2 ) );
			else if( !strcmp( script1, "NPCWANDER" ) ) chars[x].SetNpcWander( atoi( script2 ) );
			break;
		case 'o':
		case 'O':
			if( !strcmp( script1, "OLDNPCWANDER" ) ) chars[x].SetOldNpcWander( atoi( script2 ) );
			else if( !strcmp( script1, "OLDX" ) ) chars[x].SetOldX( atoi( script2 ) );
			else if( !strcmp( script1, "OLDY" ) ) chars[x].SetOldY( atoi( script2 ) );
			else if( !strcmp( script1, "OLDZ" ) ) chars[x].SetOldZ( atoi( script2 ) );
			else if( !strcmp( script1, "ORGBODY" ) )
				chars[x].SetOrgID( atoi( script2 ) );
			else if( !strcmp( script1, "OWN" ) )
				chars[x].SetOwner( atoi( script2 ) );
			break;
		case 'p':
		case 'P':
			if( !strcmp( script1, "PACKITEM" ) ) chars[x].SetPackItem( atoi( script2 ) );
			else if( !strcmp( script1, "POISON" ) ) chars[x].SetPoison( atoi( script2 ) );
			else if( !strcmp( script1, "POISONED" ) ) chars[x].SetPoisoned( atoi( script2 ) );
			else if( !strcmp( script1, "PRIV" ) ) chars[x].SetPriv( atoi( script2 ) );
			break;
		case 'q':
		case 'Q':
			if( !strcmp( script1,"QUESTTYPE" ) ) chars[x].SetQuestType( atoi( script2 ) );
			else if( !strcmp( script1,"QUESTDESTREGION" ) ) chars[x].SetQuestDestRegion( atoi( script2 ) );
			else if( !strcmp( script1,"QUESTORIGREGION" ) ) chars[x].SetQuestOrigRegion( atoi( script2 ) );
			break;						
		case 'r':
		case 'R':
			if( !strcmp( script1, "RACE" ) ) chars[x].SetRace( atoi( script2 ) );
			else if( !strcmp( script1, "RACEGATE" ) ) chars[x].SetRaceGate( atoi( script2 ) );
			else if( !strcmp( script1, "REATTACKAT" ) ) chars[x].SetReattackAt( atoi( script2 ) );
			else if( !strcmp( script1, "RESERVED1" ) ) 
			{
				chars[x].SetCell( atoi( script2 ) );
				if( chars[x].GetCell() == 0 )	// convert from old->new
					chars[x].SetCell( -1 );
			}
			else if( !strcmp( script1, "ROBE" ) )
				chars[x].SetRobe( atoi( script2 ) );
			else if( !strcmp( script1, "RUNS" ) ) chars[x].SetRun( true );
			break;
		case 's':
		case 'S':
			if( !strcmp( script1, "SAY" ) )
				chars[x].SetSayColour( atoi( script2 ) );
			else if( !strcmp( script1, "SERIAL" ) )
				chars[x].serial = atoi( script2 );
			else if( !strncmp( script1, "SKILL", 5 ) )
				chars[x].SetBaseSkill( atoi( script2 ), atoi( &script1[5] ) );
			else if( !strncmp( script1, "SKL", 3 ) )		// for skill locking
			{
				skl = atoi( &script1[3] );
				lockstate = atoi( script2 );
				if( lockstate > 2 )
					lockstate = 0;
				chars[x].SetSkillLock( lockstate, skl );
			}
			else if( !strcmp( script1, "SKIN" ) )
				chars[x].SetSkin( atoi( script2 ) );
			else if( !strcmp( script1, "SPAWN" ) )
				chars[x].spawnserial = (cBaseObject *)atoi( script2 );
			else if( !strcmp( script1, "SPATTACK" ) ) chars[x].SetSpAttack( atoi( script2 ) ); 
			else if( !strcmp( script1, "SPADELAY" ) ) chars[x].SetSpDelay( atoi( script2 ) ); 
			else if( !strcmp( script1, "SHOP" ) ) chars[x].SetShop( atoi( script2 ) != 0 );
			else if( !strcmp( script1, "SPLIT" ) ) chars[x].SetSplit( atoi( script2 ) ); 
			else if( !strcmp( script1, "SPLITCHANCE" ) ) chars[x].SetSplitChance( atoi( script2 ) ); 
			else if( !strcmp( script1, "STAMINA" ) ) chars[x].SetStamina( atoi( script2 ) ); 
			else if( !strcmp( script1, "STRENGTH" ) ) chars[x].SetStrength( atoi( script2 ) ); 
			else if( !strcmp( script1, "STRENGTH2" ) ) chars[x].Strength2( atoi( script2 ) );
			else if( !strcmp( script1, "SUMMONTIMER" ) ) chars[x].SetSummonTimer( atoi( script2 ) );
			break;
		case 't':
		case 'T':
			if( !strcmp( script1, "TAMING" ) ) chars[x].SetTaming( atoi( script2 ) ); 
			else if( !strcmp( script1, "TITLE" ) ) chars[x].SetTitle( script2 ); 
			else if( !strcmp( script1, "TOWN" ) ) chars[x].SetTown( atoi( script2 ) );
			else if( !strcmp( script1, "TOWNPRIV" ) ) chars[x].SetTownpriv( atoi( script2 ) );
			else if( !strcmp( script1, "TOWNTITLE" ) ) chars[x].SetTownTitle( atoi( script2 ) != 0 );
			else if( !strcmp( script1, "TOWNVOTE" ) )
				chars[x].SetTownVote( atoi( script2 ) );
			break;
			
		case 'w':
		case 'W':
			if( !strcmp( script1, "WAR" ) ) chars[x].SetWar( atoi( script2 ) != 0 ); 
			break;
		case 'x':
		case 'X':
			if( !strcmp( script1, "X" ) ) 
				chars[x].SetX( atoi( script2 ) ); 
			else if( !strcmp( script1, "XBODY" ) )
				chars[x].SetxID( atoi( script2 ) );
			else if( !strcmp( script1, "XSKIN" ) )
				chars[x].SetxSkin( atoi( script2 ) );
			break;
		case 'y':
		case 'Y':
			if( !strcmp( script1, "Y" ) ) chars[x].SetY( atoi( script2 ) );
			break;
		case 'z':
		case 'Z':
			if( !strcmp( script1, "Z" ) ) 
			{
				chars[x].SetDispZ( atoi( script2 ) );
				chars[x].SetZ( atoi( script2 ) );
			}
			break;
		}
		loops++;
	}
	while( strcmp( script1, "}" ) && loops <= 200 );

	if( charcount2 <= chars[x].serial ) 
		charcount2 = chars[x].serial + 1;
	chars[x].SetSerial( chars[x].serial, x );

	MapRegion->AddChar( &chars[x] );
}

void loaditem( FILE *wscfile ) // Load an item from WSC
{
	SI32 loops = 0;
	
	ITEM x = MemItemFree( 0 );
	if( x == -1 ) 
		return;
	InitItem( x, 0 );
	char script1[512], script2[512];
	do
	{
		readw2( wscfile, script1, script2 );
		// krazyglue [10/10/99]
		// strcmp's can be pretty slow when you call a lot of them, and switch
		// is very fast check on the first byte to sort out the possibilities.
		switch( script1[0] )
		{
		case 'a':
		case 'A':
			if( !strcmp( script1, "AMOUNT" ) ) items[x].SetAmount( atoi( script2 ) );
			else if( !strcmp( script1, "ATT" ) )
			{
				items[x].SetHiDamage( atoi( script2 ) ); 
				items[x].SetLoDamage( atoi( script2 ) ); 
			}
			else if( !strcmp( script1, "AC" ) ) 
				items[x].SetArmourClass( atoi( script2 ) );
			break;
			
		case 'c':
		case 'C':
			if( !strcmp( script1, "COLOR" ) )
				items[x].SetColour( atoi( script2 ) );
			else if( !strcmp( script1, "CONT" ) )
				items[x].SetContSerial( makeNum( script2 ) );
				// Removed by DarkStorm - 
				// We may encounter the contained item before the
				// container in OLD saves!!!!
				//items[x].SetCont( makeNum( script2 ) );
			else if( !strcmp( script1, "CORPSE" ) ) items[x].SetCorpse( atoi( script2 ) != 0 );
			else if( !strcmp( script1, "CREATOR" ) ) items[x].SetCreator( script2 );
			break;
			
		case 'd':
		case 'D':
			if( !strcmp( script1, "DEF" ) ) items[x].SetDef( atoi( script2 ) );
			else if( !strcmp( "DEX", script1 ) ) items[x].SetDexterity( atoi( script2 ) );
			else if( !strcmp( "DEXADD", script1 ) ) items[x].Dexterity2( atoi( script2 ) );
			else if( !strcmp( script1, "DIR" ) ) items[x].SetDir( atoi( script2 ) );
			else if( !strcmp( script1, "DISABLED" ) ) items[x].SetDisabled( atoi( script2 ) != 0 );
			else if( !strcmp( script1, "DOORFLAG" ) ) items[x].SetDoorDir( atoi( script2 ) );
			else if( !strcmp( "DYE", script1 ) ) items[x].SetDye( atoi( script2 ) != 0 );
			else if( !strcmp( "DECAY", script1 ) ) items[x].SetPriv( items[x].GetPriv() | 0x01 );
			else if( !strcmp( "DISPELLABLE", script1 ) ) items[x].SetPriv( items[x].GetPriv() | 0x04 );
			else if( !strcmp( script1, "DX" ) ) items[x].SetDexterity( atoi( script2 ) );
			else if( !strcmp( script1, "DX2" ) ) items[x].Dexterity2( atoi( script2 ) );
			else if( !strcmp( script1, "DYEABLE" ) ) items[x].SetDye( atoi( script2 ) != 0 );
			else if( !strcmp( script1, "DESC" ) ) items[x].SetDesc( script2 );
			break;
			
		case 'g':
		case 'G':
			if( !strcmp( script1, "GLOW" ) ) items[x].SetGlow( atoi( script2 ) );
			else if( !strcmp( script1, "GLOWBC" ) )
				items[x].SetGlowColour( atoi( script2 ) );
			else if( !strcmp( script1, "GLOWTYPE" ) ) items[x].SetGlowEffect( atoi( script2 ) );
			else if( !strcmp( script1, "GOOD" ) ) items[x].SetGood( atoi( script2 ) );
			break;
			
		case 'h':
		case 'H':
			if( !strcmp( script1, "HIDAMAGE" ) ) items[x].SetHiDamage( atoi( script2 ) );
			else if( !strcmp( script1, "HP" ) ) items[x].SetHP( atoi( script2 ) );
			break;
			
		case 'i':
		case 'I':
			if( !strcmp( script1, "ID" ) )
				items[x].SetID( atoi( script2 ) );
			else if( !strcmp( script1, "IN" ) ) items[x].SetIntelligence( atoi( script2 ) );
			else if( !strcmp( script1, "IN2" ) ) items[x].Intelligence2( atoi( script2 ) );
			else if( !strcmp( script1, "INT" ) ) items[x].SetIntelligence( atoi( script2 ) );
			else if( !strcmp( script1, "INTADD" ) ) items[x].Intelligence2( atoi( script2 ) );
			else if( !strcmp("ITEMHAND", script1 ) ) items[x].SetItmHand( atoi( script2 ) );
			break;
			
		case 'l':
		case 'L':
			if( !strcmp( script1, "LAYER" ) ) items[x].SetLayer( atoi( script2 ) );
			if( !strcmp( script1, "LODAMAGE" ) ) items[x].SetLoDamage( atoi( script2 ) );
			break;
			
		case 'm':
		case 'M':
			if( !strcmp( script1, "MAXHP" ) ) items[x].SetMaxHP( atoi( script2 ) );
			else if( !strcmp( script1, "MORE" ) )
				items[x].SetMore( atoi( script2 ) );
			else if( !strcmp( script1, "MORE2" ) )
				items[x].SetMoreB( atoi( script2 ) );
			else if( !strcmp( script1, "MOREX" ) ) items[x].SetMoreX( atoi( script2 ) );
			else if( !strcmp( script1, "MOREY" ) ) items[x].SetMoreY( atoi( script2 ) );
			else if( !strcmp( script1, "MOREZ" ) ) items[x].SetMoreZ( atoi( script2 ) );
			else if( !strcmp( script1, "MOVABLE" ) ) items[x].SetMagic( atoi( script2 ) );
			break;
			
		case 'n':
		case 'N':
			if( !strcmp( script1, "NAME" ) ) 
			{ 
				if( !strlen( script2 ) )
					items[x].SetName( "#" );
				else
					items[x].SetName( script2 );
			}
			else if( !strcmp( script1, "NAME2" ) ) 
			{ 
				if( !strlen( script2 ) )
					items[x].SetName2( "#" );
				else
					items[x].SetName2( script2 ); 
			}
			break;
			
		case 'o':
		case 'O':
			if( !strcmp( script1, "OFFSPELL" ) ) items[x].SetOffSpell( atoi( script2 ) );
			else if( !strcmp( script1, "OWNER" ) )
				items[x].owner = (cBaseObject *)atoi( script2 );
			break;
			
		case 'p':
		case 'P':
			if( !strcmp( script1, "PILEABLE" ) ) items[x].SetPileable( atoi( script2 ) != 0 );
			else if( !strcmp( script1, "POISONED" ) ) items[x].SetPoisoned( atoi( script2 ) );
			else if( !strcmp( script1, "PRIV" ) ) items[x].SetPriv( atoi( script2 ) );
			break;
			
		case 'r':
		case 'R':
			if( !strcmp( script1, "RESTOCK" ) ) items[x].SetRestock( atoi( script2 ) );
			else if( !strcmp( "RACE", script1 ) ) items[x].SetRace( atoi( script2 ) );
			else if( !strcmp( script1, "RANK" ) ) 
				items[x].SetRank( atoi( script2 ) );
			break;
			
		case 's':
		case 'S':
			if( !strcmp( script1, "SERIAL" ) )
				items[x].serial = atoi( script2 );
			else if( !strcmp( script1, "SPAWN" ) )
				items[x].spawnserial = (cBaseObject *)atoi( script2 );
			else if( !strcmp( script1, "SPD" ) ) items[x].SetSpeed( atoi( script2 ) );
			else if( !strcmp( script1, "ST" ) ) items[x].SetStrength( atoi( script2 ) );
			else if( !strcmp( script1, "ST2" ) ) items[x].Strength2( atoi( script2 ) );
			else if( !strcmp( script1, "SK_MADE" ) ) items[x].SetMadeWith( atoi( script2 ) );
			else if( !strcmp( script1, "STR" ) ) items[x].SetStrength( atoi( script2 ) );
			else if( !strcmp( script1, "STRADD" ) ) items[x].Strength2( atoi( script2 ) );
			break;
			
		case 't':
		case 'T':
			if( !strcmp( script1, "TYPE" ) ) items[x].SetType( atoi( script2 ) );
			else if( !strcmp( script1, "TYPE2" ) ) items[x].SetType2( atoi( script2 ) );
			break;
			
		case 'v':
		case 'V':
			if( !strcmp( script1, "VISIBLE" ) ) items[x].SetVisible( atoi( script2 ) );
			else if( !strcmp( script1, "VALUE" ) ) items[x].SetValue( atoi( script2 ) );
			break;
			
		case 'w':
		case 'W':
			if( !strcmp( script1, "WEIGHT" ) ) 
				items[x].SetWeight( atoi( script2 ) );
			break;
			
		case 'x':
		case 'X':
			if( !strcmp( script1, "X" ) ) items[x].SetX( atoi( script2 ) );
			break;
			
		case 'y':
		case 'Y':
			if( !strcmp( script1, "Y" ) ) items[x].SetY( atoi( script2 ) );
			break;
		case 'z':
		case 'Z':
			if( !strcmp( script1, "Z" ) ) items[x].SetZ( atoi( script2 ) );
			break;
		}
		
		loops++;
	}
	while( strcmp( script1, "}" ) && loops <= 200 );
 
	if( itemcount2 <= items[x].serial ) 
		itemcount2 = items[x].serial + 1;

	if( items[x].GetCont() == 0xFFFFFFFF )
	{ 
		MapRegion->AddItem( &items[x] );	// it returns 1 if inalid, if invalid it DOESNT get added !!!
	}
}
