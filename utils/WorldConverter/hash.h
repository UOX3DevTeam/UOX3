struct HashEntry
{
	int index;
};

class HashBucket
{
protected:
	vector< HashEntry > listData;
public:
	HashBucket( void );
	~HashBucket();

	void GarbageCollect( void );
	virtual int FindBySerial( SERIAL toFind );
	virtual void AddSerial( SERIAL toAdd, int index ) = 0;
	virtual void Remove( int index ) = 0;
	virtual int  NumEntries( void ) = 0;
	virtual int  GetEntry( int entryNum ) = 0;
};

class HashBucketChar : public HashBucket
{
public:
	virtual int FindBySerial( SERIAL toFind );
	virtual void AddSerial( SERIAL toAdd, int index );
	virtual void Remove( int index );
	virtual void RemoveBySer( SERIAL removing );
	virtual int  NumEntries( void );
	virtual int  GetEntry( int entryNum );
};

class HashBucketItem : public HashBucket
{
public:
	virtual int FindBySerial( SERIAL toFind );
	virtual void AddSerial( SERIAL toAdd, int index );
	virtual void Remove( int index );
	virtual void RemoveBySer( SERIAL removing );
	virtual int  NumEntries( void );
	virtual int  GetEntry( int entryNum );
};

class HashTable
{
protected:
public:
	virtual void GarbageCollect( void ) = 0;
	virtual int FindBySerial( SERIAL toFind ) = 0;
	virtual void AddSerial( SERIAL toAdd, int index ) = 0;
	virtual void Remove( SERIAL toRemove, int index ) = 0;
};

class HashTableChar : public HashTable
{
protected:
	vector< HashBucketChar > data;
public:
	HashTableChar( void );
	HashTableChar( int hashSize );
	~HashTableChar();
	virtual void GarbageCollect( void );
	virtual int FindBySerial( SERIAL toFind );
	virtual void AddSerial( SERIAL toAdd, int index );
	virtual void Remove( SERIAL toRemove, int index );
	HashBucketChar *GetBucket( int index );
	void RemoveBySer( SERIAL toRemove, SERIAL removing );
};

class HashTableItem : public HashTable
{
protected:
	vector< HashBucketItem > data;
public:
	HashTableItem( void );
	HashTableItem( int hashSize );
	~HashTableItem();
	virtual void GarbageCollect( void );
	virtual int FindBySerial( SERIAL toFind );
	virtual void AddSerial( SERIAL toAdd, int index );
	virtual void Remove( SERIAL toRemove, int index );
	HashBucketItem *GetBucket( int index );
	void RemoveBySer( SERIAL toRemove, SERIAL removing );
};