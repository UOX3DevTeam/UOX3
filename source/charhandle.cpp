#include "uox3.h"

// Significant modification of Zippy's original CharacterHandle class
// now makes use of CChar, rather than char_st, and is cleaner, tighter

//o---------------------------------------------------------------------------o
//|	Class		:	cCharacterHandle::cCharacterHandle()
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//| Modification:	Abaddon, April 3rd, 2000
//o---------------------------------------------------------------------------o

cCharacterHandle::cCharacterHandle( void )
{
	DefaultChar = new CChar( INVALIDSERIAL, true );
	
	//setup the item's important properties (ones that might be checked if its used, and need to be this way
	DefaultChar->setFree( true );
	Actual = Free = 0;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cCharacterHandle::~cCharacterHandle()
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
cCharacterHandle::~cCharacterHandle()
{

	Chars.clear();
	Chars.resize( 0 );
	
	FreeNums.clear();
	FreeNums.resize( 0 );

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
UI32 cCharacterHandle::New( bool zeroSer = false )
{
	UI32 i;
	
	if( Free > 0 )
	{
		i = FreeNums[FreeNums.size()-1];//get the oldest entry
		FreeNums.resize( FreeNums.size() - 1 ); //Delete it cause it ain't free no more.
		Free = max( Free-1, (UI32)0 );

		if( Chars[i] != NULL )
			delete Chars[i];
		Chars[i] = new CChar( i, zeroSer );
	} 
	else 
	{
		i = Chars.size();
		Chars.push_back( new CChar( i, zeroSer ) );
	}

	if( i == charcount )
		charcount++;
	Actual++;
	return i;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cCharacterHandle::Delete( UI32 Num )
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Free character memory (delete the char)
//o---------------------------------------------------------------------------o
void cCharacterHandle::Delete( UI32 Num )
{
	if( Num != INVALIDSERIAL && Num < Chars.size() )
	{
		if( Chars[Num] != NULL  )
		{
			delete Chars[Num];
			Chars[Num] = NULL;

			FreeNums.insert( FreeNums.begin(), Num );
			Free++;
			Actual = max( Actual - 1, (UI32)0 );
		} 
		else if( !isFree( Num ) )
		{
			FreeNums.insert(FreeNums.begin(), Num);
			Free++;
			Actual = max( Actual-1, (UI32)0 );
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
UI32 cCharacterHandle::Size( void )
{
	UI32 sz;

	sz = Chars.size() * 4;
	sz += FreeNums.size() * sizeof(UI32);
	sz += sizeof(CChar) * Actual;
	sz += sizeof(cCharacterHandle);

	return sz;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cCharacterHandle::Reserve( UI32 )
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Reserve memory for Num number of characters (unused)
//o---------------------------------------------------------------------------o
void cCharacterHandle::Reserve( UI32 Num )
{
	UI32 i, cs = FreeNums.size(), is = Chars.size();
	
	Free+=Num;
	FreeNums.resize( cs + Num );
	Chars.resize( is+Num );

	for( i = cs; i < ( cs + Num ); i++ )
	{// is + (cs+(Num-1))-i) is acctual number ( 0 to Num )
		FreeNums[i] = is + ((cs+(Num-1))-i);
		Chars[(is + (cs+(Num-1))-i)] = NULL;
	}
}

//o---------------------------------------------------------------------------o
//|	Class		:	cCharacterHandle::isFree( UI32 )
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Check to see if item Num is marked free (unused (slow))
//o---------------------------------------------------------------------------o
bool cCharacterHandle::isFree( UI32 Num )
{
	for( UI32 i = 0; i < FreeNums.size(); i++ )
	{
		if( FreeNums[i] == Num )
			return true;
	}
	return false;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cCharacterHandle::operator[]( UI32 Num )
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Reference character Num -  Check to make sure Num is a 
//|					valid character number and exists in memory
//o---------------------------------------------------------------------------o
CChar& cCharacterHandle::operator[] ( UI32 Num )
{
	if( Num < Chars.size() )
	{
		if( Chars[Num] != NULL )
			return *Chars[Num];
	} 
	else
		Console.Error( 4, "Chars[%i] referenced is invalid. Crash averted!", Num ); 
  //Make sure these props are always this way, they may have been chaged by other functions, so put them back
	DefaultChar->SetX( 0 );
	DefaultChar->SetY( 0 );
	return *DefaultChar;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cCharacterHandle::Count()
//|	Date		:	1/7/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Return the number of characters in the world (Acctual)
//o---------------------------------------------------------------------------o
UI32 cCharacterHandle::Count( void )
{
	return Actual;
}

//o---------------------------------------------------------------------------o
//|	Class		:	void cCharacterHandle::Cleanup( void )
//|	Date		:	August 23rd, 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Cleans up any chars that have been allocated, preparing for
//|					shutdown
//o---------------------------------------------------------------------------o

void cCharacterHandle::Cleanup( void )
{
	for( UI32 i = 0; i < Chars.size(); i++ )//Memory Cleanup
	{
		if( Chars[i] != NULL )
		{
			delete Chars[i];
			Chars[i] = NULL;
		}
	}
}
//End of cCharacterHandle class//

