#ifndef __CMULTIOBJ_H__
#define __CMULTIOBJ_H__

typedef map< SERIAL, cBaseObject *> baseMap;
typedef baseMap::iterator			baseMapIterator;
typedef baseMap::const_iterator		baseMapConstIterator;

typedef map< SERIAL, CChar *>		baseCharMap;
typedef baseCharMap::iterator		baseCharMapIterator;
typedef baseCharMap::const_iterator	baseCharMapConstIterator;

class CMultiObj : public CItem
{
protected:
	baseMap				itemInMulti;
	baseMap				charInMulti;
	baseCharMap			owners;
	baseCharMap			banList;
	vector< CItem * >	lockedList;
	UI16				maxLockedDown;

	baseMapIterator		itemMultiIterator;
	baseMapIterator		charMultiIterator;

	bool				CharIsInMulti( cBaseObject *toFind ) const;
	bool				ItemIsInMulti( cBaseObject *toFind ) const;

	void				RemoveCharFromMulti( cBaseObject *toFind );
	void				RemoveItemFromMulti( cBaseObject *toFind );

	void				AddCharToMulti( cBaseObject *toFind );
	void				AddItemToMulti( cBaseObject *toFind );
public:
						CMultiObj();
	virtual				~CMultiObj();

	virtual	void		AddToBanList( CChar *toBan );
	virtual	void		RemoveFromBanList( CChar *toRemove );

	virtual	void		AddAsOwner( CChar *newOwner );
	virtual	void		RemoveAsOwner( CChar *toRemove );
	virtual	void		ClearOwners( void );

	virtual bool		IsInMulti( cBaseObject *toFind ) const;
	virtual bool		IsOnBanList( CChar *toBan ) const;
	virtual bool		IsOwner( CChar *toFind ) const;

	virtual void		AddToMulti( cBaseObject *toAdd );
	virtual void		RemoveFromMulti( cBaseObject *toRemove );

	virtual UI16		GetLockDownCount( void ) const;
	virtual UI16		GetMaxLockDowns( void ) const;
	virtual void		SetMaxLockDowns( UI16 newValue );
	virtual void		LockDownItem( CItem *toLock );
	virtual void		RemoveLockDown( CItem *toRemove );

	virtual bool		Save( ofstream &outStream, int mode );
	virtual bool		DumpHeader( ofstream &outStream, int mode ) const;
	virtual bool		DumpBody( ofstream &outStream, int mode ) const;
	virtual bool		DumpFooter( ofstream &outStream, int mode ) const;
	virtual bool		Load( ifstream &inStream, int mode, int arrayOffset );
	virtual bool		HandleLine( char *tag, char *data );

	virtual CChar *		FirstCharMulti( void );
	virtual CChar *		NextCharMulti( void );
	virtual bool		FinishedCharMulti( void ) const;

	virtual CItem *		FirstItemMulti( void );
	virtual CItem *		NextItemMulti( void );
	virtual bool		FinishedItemMulti( void ) const;
	virtual void		PostLoadProcessing( SI32 index );

	virtual void		SetOwner( CChar *newOwner );
};


#endif