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

class cBooks
{
	private:
		
		// that private methods read and write from the *.bok files
		void WriteAuthor( CItem *id,cSocket *s );
		void WriteTitle( CItem *id, cSocket *s );
		void WriteLine( CItem *id, UI16 page, int line, char linestr[34] );
		
		void ReadAuthor( CItem *id, UI08 auth[31] );
		void ReadTitle( CItem *id, UI08 title[61] );
		UI08 getNumberOfPages( CItem *id );
		void ReadLine( CItem *id, int page,int linenumber, char line[33] );

		bool CreateBook( char *fileName, CItem *id ); // "formats and creates a new bok file"
					
	public:
		bool changeAT; // flag -> set if author and title changed		
	
	    cBooks();

    void OpenPreDefBook( cSocket *s, CItem *i ); // opens old-readonly books, takes data from misc.scp
		void OpenBook( cSocket *s, CItem *i, bool isWriteable ); // opens new books
		void ReadPreDefBook( cSocket *mSock, CItem *i, UI16 p ); // reads books from misc.scp, readonly = old books
		void ReadNonWritableBook( cSocket *s, CItem *i, UI16 p );     // reads new books readonly ( from *.bok file )
		void ReadWritableBook( cSocket *s, CItem *i, UI16 p, int l ); // writes changes to a new book opened in writable mode 
		void DeleteBook( CItem *id ); // deletes bok-file.

		virtual ~cBooks();
        	
};
#endif
