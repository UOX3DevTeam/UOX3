//o---------------------------------------------------------------------------o
//| This is the implementation of CItem based cItemHandler
//o---------------------------------------------------------------------------o

#include "uox3.h"

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::cItemHandle()
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
cItemHandle::cItemHandle( void )
{
	DefaultItem = new CItem( INVALIDSERIAL, false );
	
	//setup the item's important properties (ones that might be checked if its used, and need to be this way
	DefaultItem->SetFree( true );
	Actual = Free = 0;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::~cItemHandle()
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
cItemHandle::~cItemHandle()
{
	Cleanup();

	Items.clear();
	Items.resize( 0 );
	
	FreeNums.clear();
	FreeNums.resize( 0 );

	delete DefaultItem;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::New()
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Free memory for an item, create the item in memory 
//|					(returns item number)
//o---------------------------------------------------------------------------o
UI32 cItemHandle::New( bool zeroSer, UI08 itemType )
{
	UI32 i;
	
	if( Free > 0 )
	{
		i = FreeNums[FreeNums.size()-1];//get the oldest entry
		FreeNums.resize( FreeNums.size() - 1 ); //Delete it cause it ain't free no more.
		Free = max( Free-1, (UI32)0 );

		if( Items[i] != NULL )
		{
			delete Items[i];
			Items[i] = NULL;
		}

		switch( itemType )
		{
		default:
		case 0:		Items[i] = new CItem( i, zeroSer );	break;
		case 1:		Items[i] = new CMultiObj( i, zeroSer );			break;
		}

	} 
	else 
	{
		i = Items.size();
		switch( itemType )
		{
		default:
		case 0:		Items.push_back( new CItem( i, zeroSer ) );		break;
		case 1:		Items.push_back( new CMultiObj( i, zeroSer ) );				break;
		}
	}

	Actual++;

	if( i == cwmWorldState->GetItemCount() ) 
		cwmWorldState->IncItemCount();

	return i;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::Delete( SI32 Num )
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Free item memory that's used by this item (delete it)
//o---------------------------------------------------------------------------o
void cItemHandle::Delete( UI32 Num )
{
	if( Num != INVALIDSERIAL && Num < Items.size() )
	{
		if( Items[Num] != NULL )
		{
			delete Items[Num];
			Items[Num] = NULL;

			FreeNums.insert( FreeNums.begin(), Num );
			Free++;
			Actual = max( Actual - 1, (UI32)0 );
		} 
		else if( !isFree( Num ) )
		{
			FreeNums.insert( FreeNums.begin(), Num );
			Free++;
			Actual = max( Actual - 1, (UI32)0 );
		}
	}
}

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::Size()
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Get the size (in bytes) that items are taking up in memory
//o---------------------------------------------------------------------------o
UI32 cItemHandle::Size( void )
{
	UI32 sz;

	sz = Items.size() * 4;
	sz += FreeNums.size() * sizeof(UI32);
	sz += sizeof(CItem) * Actual;
	sz += sizeof(cItemHandle);

	return sz;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::Reserve( UI32 )
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Reserve memory for Num number of items (unused)
//o---------------------------------------------------------------------------o
void cItemHandle::Reserve( UI32 Num )
{
	UI32 i, cs = FreeNums.size(), is = Items.size();
	
	Free+=Num;
	FreeNums.resize( cs + Num );
	Items.resize( is+Num );

	for( i = cs; i < ( cs + Num ); i++ )
	{// is + (cs+(Num-1))-i) is acctual number ( 0 to Num )
		FreeNums[i] = is + ((cs+(Num-1))-i);
		Items[(is + (cs+(Num-1))-i)] = NULL;
	}
}

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::isFree( UI32 )
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Check to see if item Num is marked free (unused (slow))
//o---------------------------------------------------------------------------o
bool cItemHandle::isFree( UI32 Num )
{
	for( UI32 i = 0; i < FreeNums.size(); i++ )
	{
		if( FreeNums[i] == Num )
			return true;
	}
	return false;
}
//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::operator[]( UI32 Num )
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Reference item Num -  Check to make sure Num is a valid 
//|					item number and exists in memory
//o---------------------------------------------------------------------------o
CItem& cItemHandle::operator[] ( UI32 Num )
{
	if( Num != INVALIDSERIAL && Num < Items.size() )
	{
		if( Items[Num] != NULL )
			return *Items[Num];	
	}
	else
		Console.Error( 4, "WARNING: Items[%i] referenced is invalid. Crash averted!\n", Num );
	//Make sure these props are always this way, they may have been chaged by other functions, so put them back
	DefaultItem->SetFree( true );
	DefaultItem->SetSerial( INVALIDSERIAL, INVALIDSERIAL );
	DefaultItem->SetOwner( NULL );
	DefaultItem->SetSpawn( INVALIDSERIAL, INVALIDSERIAL );
	DefaultItem->SetCont( NULL );
	DefaultItem->SetX( 0 );
	DefaultItem->SetY( 0 );
	return *DefaultItem;
}

//o---------------------------------------------------------------------------o
//|	Class		:	cItemHandle::Count()
//|	Date		:	1/6/00
//|	Programmer	:	Zippy
//o---------------------------------------------------------------------------o
//| Purpose		:	Return the number of items in the world (Acctual)
//o---------------------------------------------------------------------------o
UI32 cItemHandle::Count( void )
{
	return Actual;
}

//o---------------------------------------------------------------------------o
//|	Class		:	void cItemHandle::Cleanup( void )
//|	Date		:	August 23rd, 2001
//|	Programmer	:	Abaddon
//o---------------------------------------------------------------------------o
//| Purpose		:	Cleans up any items that have been allocated, preparing for
//|					shutdown
//o---------------------------------------------------------------------------o
void cItemHandle::Cleanup( void )
{
	for( UI32 i = 0; i < Items.size(); i++ ) //Memory Cleanup
	{
		if( Items[i] != NULL )
		{
			delete Items[i];
			Items[i] = NULL;
		}
	}
}

//End of cItemHandle class//

