// Book class by Lord Binary, 7'th december 1999

// new books readonly -> morex  999
// new books writeable -> morex 666
// old books from misc.scp, readonly: neither type 666 nor 999, section# = more-value
// of course ALL books need a type of 11

// note to everyone who wants to learn C++ -> this is NOT stricly OOP and has no good OOD.
// hence no real good example for OO-P :)
// for example inheritence could be used, public attributes arnt nice etc,etc.


#ifndef __books_h
#define __books_h

namespace UOX
{

class cBooks
{
private:

	// that private methods read and write from the *.bok files
	void WriteAuthor( CItem *id, CSocket *s );
	void WriteTitle( CItem *id, CSocket *s );
	void WriteLine( CItem *id, UI16 page, UI16 line, char linestr[34] );

	std::string ReadAuthor( CItem *id );
	std::string ReadTitle( CItem *id );
	UI08 getNumberOfPages( CItem *id );
	std::string ReadLine( CItem *id, UI08 page, UI08 linenumber );

	bool CreateBook( std::string fileName, CItem *id ); // "formats and creates a new bok file"

public:
	bool changeAT; // flag -> set if author and title changed		

	void OpenPreDefBook( CSocket *s, CItem *i ); // opens old-readonly books, takes data from misc.scp
	void OpenBook( CSocket *s, CItem *i, bool isWriteable ); // opens new books
	void ReadPreDefBook( CSocket *mSock, CItem *i, UI16 p ); // reads books from misc.scp, readonly = old books
	void ReadNonWritableBook( CSocket *s, CItem *i, UI16 p );	// reads new books readonly ( from *.bok file )
	void ReadWritableBook( CSocket *s, CItem *i, UI16 p, UI16 l ); // writes changes to a new book opened in writable mode 
	void DeleteBook( CItem *id ); // deletes bok-file.
};

extern cBooks *Books;

}

#endif
