#ifndef __HANDLERS_H__
#define __HANDLERS_H__

class cCharacterHandle
{
protected:
	CChar *	DefaultChar;//The item send if an out of bounds is referenced.

	CHARLIST Chars;			// Vector of pointers to items, NULL if no item at that pos
	UI32	Actual;					// Number of items in existance

	std::vector< UI32 > FreeNums;		// Vector of free item numbers
	UI32	Free;					// Number of free spaces in Acctual (Recyle item numbers)
	bool	isFree( UI32 Num );		// Check to see if this item is marked free

public:
			cCharacterHandle( void );	// Class Constructor
	virtual	~cCharacterHandle();		// Class Destructor

	UI32	New( bool zeroSer );		// Get Memory for a new character, Returns char number
	void	Delete( UI32 Num );			// Free memory used by this character
	UI32	Size( void );				// Return the size (in bytes) of ram characters are taking up
	void	Reserve( UI32 );			// Reserve memory for this number of characters (UNUSED)
	UI32	Count( void );				// Return Actual-> the number of characters in world.

	void	Cleanup( void );

	CChar&	operator[] ( UI32 );//Reference a character
};

class cItemHandle
{
protected:
	CItem *	DefaultItem;				// The item send if an out of bounds is referenced.

	ITEMLIST Items;			// Vector of pointers to items, NULL if no item at that pos
	UI32	Actual;						// Number of items in existance

	std::vector< UI32 > FreeNums;			// Vector of free item numbers
	UI32	Free;						// Number of free spaces in Acctual (Recyle item numbers)
	bool	isFree( UI32 Num );			// Check to see if this item is marked free

public:
			cItemHandle( void );		// Class Constructor
	virtual	~cItemHandle();				// Class Destructor

	UI32	New( bool zeroSer, UI08 itemType = 0 );	// Get Memory for a new item, Returns Item number
	void	Delete( UI32 Num );			// Free memory used by this item
	UI32	Size( void );				// Return the size (in bytes) of ram items are taking up
	void	Reserve( UI32 );			// Reserve memory for this number of items (UNUSED)
	UI32	Count( void );				// Return Acctual-> the number of items in world.

	void	Cleanup( void );

	CItem&	operator[] ( UI32 );//Reference an item
};


#endif

