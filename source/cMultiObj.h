#ifndef __CMULTIOBJ_H__
#define __CMULTIOBJ_H__

namespace UOX
{

class CMultiObj : public CItem
{
protected:
	std::string			deed;

	CHARLIST			owners;
	CHARLIST			banList;
	ITEMLIST			lockedList;
	UI16				maxLockedDown;

	virtual bool		LoadRemnants( void );

	CDataList< CItem * >	itemInMulti;
	CDataList< CChar * >	charInMulti;

public:
						CMultiObj();
	virtual				~CMultiObj();

	void				AddToBanList( CChar *toBan );
	void				RemoveFromBanList( CChar *toRemove );

	void				AddAsOwner( CChar *newOwner );
	void				RemoveAsOwner( CChar *toRemove );
	void				ClearOwners( void );

	bool				IsOnBanList( CChar *toBan ) const;
	bool				IsOwner( CChar *toFind ) const;

	void				AddToMulti( CBaseObject *toAdd );
	void				RemoveFromMulti( CBaseObject *toRemove );

	size_t				GetLockDownCount( void ) const;
	UI32				GetMaxLockDowns( void ) const;
	void				SetMaxLockDowns( UI16 newValue );
	void				LockDownItem( CItem *toLock );
	void				RemoveLockDown( CItem *toRemove );

	virtual bool		Save( std::ofstream &outStream );
	virtual bool		DumpHeader( std::ofstream &outStream ) const;
	virtual bool		DumpBody( std::ofstream &outStream ) const;
	virtual bool		HandleLine( UString &UTag, UString &data );

	virtual void		SetOwner( CChar *newOwner );

	std::string			GetDeed( void ) const;
	void				SetDeed( std::string newDeed );

	virtual void		Cleanup( void );

	virtual bool		CanBeObjType( ObjectType toCompare ) const;

	CDataList< CChar * > *	GetCharsInMultiList( void );
	CDataList< CItem * > *	GetItemsInMultiList( void );
};

class CBoatObj : public CMultiObj
{
protected:
	SERIAL				tiller;
	SERIAL				planks[2];
	SERIAL				hold;
	UI08				moveType;

	TIMERVAL			nextMoveTime;

private:
	virtual bool		DumpHeader( std::ofstream &outStream ) const;
	virtual bool		DumpBody( std::ofstream &outStream ) const;

	virtual bool		HandleLine( UString &UTag, UString &data );

public:
						CBoatObj();
	virtual				~CBoatObj();

	SERIAL				GetTiller( void ) const;
	SERIAL				GetPlank( UI08 plankNum ) const;
	SERIAL				GetHold( void ) const;
	UI08				GetMoveType( void ) const;

	void				SetPlank( UI08 plankNum, SERIAL newVal );
	void				SetTiller( SERIAL newVal );
	void				SetHold( SERIAL newVal );
	void				SetMoveType( UI08 newVal );

	TIMERVAL			GetMoveTime( void ) const;
	void				SetMoveTime( TIMERVAL newVal );

	virtual bool		CanBeObjType( ObjectType toCompare ) const;
};

}

#endif

