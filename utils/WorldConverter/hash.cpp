#include "uox3.h"

#define __HASH_SAFETY__

void HashBucket::GarbageCollect( void )
{
	for( SI32 i = listData.size() - 1; i >= 0; i-- )
	{
		if( listData[i].index == -1 )
		{
			for( SI32 j = i; j < listData.size() - 1; j++ )
				listData[j] = listData[j+1];
		}
	}
}

void HashTableChar::GarbageCollect( void )
{
	for( UI32 i = 0; i < data.size(); i++ )
		data[i].GarbageCollect();
}

int HashTableChar::FindBySerial( SERIAL toFind )
{
	int hash = toFind%data.size();
	return data[hash].FindBySerial( toFind );
}

void HashTableItem::GarbageCollect( void )
{
	for( UI32 i = 0; i < data.size(); i++ )
		data[i].GarbageCollect();
}

int HashTableItem::FindBySerial( SERIAL toFind )
{
	int hash = toFind%data.size();
	return data[hash].FindBySerial( toFind );
}

HashTableChar::HashTableChar()
{
	data.resize( HASHMAX );
}

HashTableChar::HashTableChar( int hashSize )
{
	data.resize( hashSize );
}

HashTableChar::~HashTableChar()
{
	data.resize( 0 );
}

HashTableItem::HashTableItem()
{
	data.resize( HASHMAX );
}

HashTableItem::HashTableItem( int hashSize )
{
	data.resize( hashSize );
}

HashTableItem::~HashTableItem()
{
	data.resize( 0 );
}

HashBucket::~HashBucket()
{
	listData.resize( 0 );
}

HashBucket::HashBucket()
{
	listData.resize( 0 );
}

int HashBucket::FindBySerial( SERIAL toFind )
{
	bool charType = false;
	if( (UCHAR)(toFind>>24) < 0x40 )
		charType = true;
	for( UI32 i = 0; i < listData.size(); i++ )
	{
		if( listData[i].index != -1 )
		{
			if( charType )
			{
				if( listData[i].index > cmem )
					continue;
				if( chars[listData[i].index].GetSerial() == toFind )
					return listData[i].index;
			}
			else
			{
				if( listData[i].index > imem )
					continue;
				if( items[listData[i].index].GetSerial() == toFind )
					return listData[i].index;
			}
		}
	}
	return -1;
}

int HashBucketChar::FindBySerial( SERIAL toFind )
{
	for( UI32 i = 0; i < listData.size(); i++ )
	{
		if( listData[i].index != -1 )
		{
			if( listData[i].index > cmem )
				continue;
			if( chars[listData[i].index].GetSerial() == toFind )
				return listData[i].index;
		}
	}
	return -1;
}

int HashBucketItem::FindBySerial( SERIAL toFind )
{
	for( UI32 i = 0; i < listData.size(); i++ )
	{
		if( listData[i].index != -1 )
		{
			if( listData[i].index > imem )
				continue;
			if( items[listData[i].index].GetSerial() == toFind )
				return listData[i].index;
		}
	}
	return -1;
}

void HashTableChar::AddSerial( SERIAL toAdd, int index )
{
	int hash = toAdd%data.size();
	data[hash].AddSerial( toAdd, index );
}

void HashTableItem::AddSerial( SERIAL toAdd, int index )
{
	int hash = toAdd%data.size();
	data[hash].AddSerial( toAdd, index );
}

void HashBucketChar::AddSerial( SERIAL toAdd, int index )
{
#ifdef __HASH_SAFETY__
	for( int i = 0; i < listData.size(); i++ )
	{
		if( listData[i].index == index )
		{
			printf( "ERROR: Hash Bucket Char error: index already exists (%i for serial %i)", index, toAdd );
			return;
		}
	}
#endif
	HashEntry newAdd;
	newAdd.index = index;
	listData.push_back( newAdd );
}


void HashBucketItem::AddSerial( SERIAL toAdd, int index )
{
#ifdef __HASH_SAFETY__
	for( int i = 0; i < listData.size(); i++ )
	{
		if( listData[i].index == index )
		{
			printf( "ERROR: Hash Bucket Item error: index already exists (%i for serial %i)", index, toAdd );
			return;
		}
	}
#endif
	HashEntry newAdd;
	newAdd.index = index;
	listData.push_back( newAdd );
}

void HashBucketChar::Remove( int index )
{
	for( UI32 i = 0; i < listData.size(); i++ )
	{
		if( listData[i].index == index )
		{
			listData[i].index = -1;			// for garbage collecting later
			return;
		}
	}
}

void HashBucketChar::RemoveBySer( SERIAL removing )
{
	for( UI32 i = 0; i < listData.size(); i++ )
	{
		if( listData[i].index != -1 && chars[listData[i].index].GetSerial() == removing )
		{
			listData[i].index = -1;			// for garbage collecting later
			return;
		}
	}
}

void HashTableChar::Remove( SERIAL toRemove, int index )
{
	int hash = toRemove%data.size();
	data[hash].Remove( index );
}
void HashTableChar::RemoveBySer( SERIAL toRemove, SERIAL removing )
{
	int hash = toRemove%data.size();
	data[hash].RemoveBySer( removing );
}

void HashBucketItem::Remove( int index )
{
	for( UI32 i = 0; i < listData.size(); i++ )
	{
		if( listData[i].index == index )
		{
			listData[i].index = -1;			// for garbage collecting later
			return;
		}
	}
}

void HashBucketItem::RemoveBySer( SERIAL removing )
{
	for( UI32 i = 0; i < listData.size(); i++ )
	{
		if( listData[i].index != -1 && items[listData[i].index].GetSerial() == removing )
		{
			listData[i].index = -1;			// for garbage collecting later
			return;
		}
	}
}

void HashTableItem::Remove( SERIAL toRemove, int index )
{
	int hash = toRemove%data.size();
	data[hash].Remove( index );
}
void HashTableItem::RemoveBySer( SERIAL toRemove, SERIAL removing )
{
	int hash = toRemove%data.size();
	data[hash].RemoveBySer( removing );
}

HashBucketChar *HashTableChar::GetBucket( int index )
{
	if( index < 0 || index >= data.size() )
		return NULL;
	return &data[index];
}

HashBucketItem *HashTableItem::GetBucket( int index )
{
	if( index < 0 || index >= data.size() )
		return NULL;
	return &data[index];
}

int HashBucketChar::NumEntries( void )
{
	return listData.size();
}
int HashBucket::NumEntries( void )
{
	return listData.size();
}
int HashBucketItem::NumEntries( void )
{
	return listData.size();
}

int HashBucketChar::GetEntry( int entryNum )
{
	if( entryNum >= NumEntries() )
		return -1;
	return listData[entryNum].index;
}
int HashBucket::GetEntry( int entryNum )
{
	if( entryNum >= NumEntries() )
		return -1;
	return listData[entryNum].index;
}
int HashBucketItem::GetEntry( int entryNum )
{
	if( entryNum >= NumEntries() )
		return -1;
	return listData[entryNum].index;
}
