#define __HASH_SAFETY__
#define HASHMAX 1031 // hashmax must be a prime for maximum performce.

template< class EntryType, class LookupType = SERIAL >
class HashBucketUnique
{
	typedef map< LookupType, EntryType >	LEMap;
	typedef LEMap::iterator					iterator;

protected:
	LEMap	listData;
	bool	hashSafety;

public:
	HashBucketUnique() : hashSafety( true ) {}
	virtual ~HashBucketUnique() {}

	void GarbageCollect( void )
	{
		for( iterator i = listData.begin(); i != listData.end(); i++ )
		{
			if( i->second == INVALIDSERIAL )
				listData.erase( i );
		}
	}
	
	virtual EntryType FindBySerial( LookupType toFind )
	{
		iterator i = listData.find( toFind );
		if( i != listData.end() )
			return i->second;
		else
			return INVALIDSERIAL;
	}

	virtual void AddSerial( LookupType toAdd, EntryType index )
	{
		if( hashSafety )
		{
			iterator i = listData.find( toAdd );
			if( i != listData.end() )
			{
				Console.Error( 4, "Hash Bucket error: index already exists (%i for serial %i)", index, toAdd );
				return;
			}
		}
		listData[toAdd] = index;
	}

	virtual void Remove( EntryType index )
	{
		for( iterator i = listData.begin(); i != listData.end(); i++ )
		{
			if( i->second == index )
			{
				listData.erase( i );
				return;
			}
		}
	}

	virtual void RemoveBySer( LookupType removing )
	{
		iterator i = listData.find( removing );
		if( i != listData.end() )
			listData.erase( i );
	}

	virtual UI32 NumEntries( void )
	{
		return listData.size();
	}

	EntryType GetEntry( UI32 entryNum )
	{//DO NOT USE THIS...
		iterator i = listData.begin();
		if( entryNum >= NumEntries() )
			return INVALIDSERIAL;
		else 
		{
			for( UI32 c = 0; c < entryNum; c++ )
				i++;//goto this number inside the map, map iterators don't have +/+= operators like vectors
			return i->second;
		}
	}
};

template< class EntryType, class LookupType = SERIAL >
class HashBucketMulti
{
protected:
	vector< EntryType >	listData;
	bool	hashSafety;

public:
	HashBucketMulti() : hashSafety( false ) {}
	virtual ~HashBucketMulti() 
	{
		listData.resize( 0 );
	}

	void GarbageCollect( void )
	{
		for( SI32 i = listData.size() - 1; i >= 0; i-- )   
		{   
			if( listData[i] == -1 )   
			{   
				for( SI32 j = i; j < (SI32)listData.size() - 1; j++ )   
					listData[j] = listData[j+1];   
			}   
		} 
	}
	
	virtual EntryType FindBySerial( LookupType toFind )
	{
		throw "Multi buckets can't do this";
		return INVALIDSERIAL;
	}

	virtual void AddSerial( LookupType toAdd, EntryType index )
	{
		if( hashSafety )
		{
			for( UI32 i = 0; i < listData.size(); i++ )   
			{   
				if( listData[i] == index )   
				{   
					Console.Error( 4, "Hash Bucket error: index already exists (%i for serial %i)", index, toAdd );   
					return;   
				}   
			}   
		}
		listData.push_back( index ); 
	}

	virtual void Remove( EntryType index )
	{
		for( UI32 i = 0; i < listData.size(); i++ )   
		{   
			if( listData[i] == index )   
			{   
				listData[i] = INVALIDSERIAL;		// for garbage collecting later   
				return;   
			}   
		} 
	}

	virtual void RemoveBySer( LookupType removing )
	{
		throw "Multi buckets can't do this";
	}

	virtual UI32 NumEntries( void )
	{
		return listData.size();
	}

	EntryType GetEntry( UI32 entryNum )
	{
		if( entryNum >= NumEntries() )
			return INVALIDSERIAL;
		else 
			return listData[entryNum];
	}
};

template <class EntryType, class LookupType = SERIAL >
class HashTable
{
	typedef HashBucketUnique< EntryType, LookupType > bucket;

protected:
	vector< bucket > data;

public:
	HashTable( void ) 
	{ 
		data.resize( HASHMAX ); 
	}
	
	HashTable( UI32 hashSize ) 
	{ 
		data.resize( hashSize ); 
	}

	~HashTable() {  }

	virtual void GarbageCollect( void )
	{
		for( UI32 i = 0; i < data.size(); i++ )
			data[i].GarbageCollect();
	}

	virtual UI32 FindBySerial( LookupType toFind )
	{
		return data[toFind%data.size()].FindBySerial( toFind );
	}

	virtual void AddSerial( LookupType toAdd, EntryType index )
	{
		data[toAdd%data.size()].AddSerial( toAdd, index );
	}

	virtual void Remove( LookupType toRemove, EntryType index )
	{
		data[toRemove%data.size()].RemoveBySer( toRemove );
	}

	bucket *GetBucket( EntryType index )
	{
		if( index == INVALIDSERIAL || index >= data.size() )
			return NULL;
		return &data[index];
	}

	void RemoveBySer( LookupType toRemove, LookupType removing )
	{
		data[toRemove%data.size()].RemoveBySer( removing );
	}
};
 
template <class EntryType, class LookupType = SERIAL >
class HashTableMulti
{
	typedef HashBucketMulti< EntryType, LookupType > bucket;

protected:
	vector< bucket > data;

public:
	HashTableMulti( void ) 
	{ 
		data.resize( HASHMAX ); 
	}
	
	HashTableMulti( UI32 hashSize ) 
	{ 
		data.resize( hashSize ); 
	}

	~HashTableMulti() {  }

	virtual void GarbageCollect( void )
	{
		for( UI32 i = 0; i < data.size(); i++ )
			data[i].GarbageCollect();
	}

	virtual UI32 FindBySerial( LookupType toFind )
	{
		return data[toFind%data.size()].FindBySerial( toFind );
	}

	virtual void AddSerial( LookupType toAdd, EntryType index )
	{
		data[toAdd%data.size()].AddSerial( toAdd, index );
	}

	virtual void Remove( LookupType toRemove, EntryType index )
	{
		data[toRemove%data.size()].Remove( index );
	}

	bucket *GetBucket( EntryType index )
	{
		if( index == INVALIDSERIAL || index >= data.size() )
			return NULL;
		return &data[index];
	}

	void RemoveBySer( LookupType toRemove, LookupType removing )
	{
		data[toRemove%data.size()].RemoveBySer( removing );
	}
};


