#ifndef __GUMP_H__
#define __GUMP_H__

void MultiGumpCallback( CSocket *mySocket, SERIAL GumpSerial, UI32 Button );

class CGump
{
private:
	STRINGLIST TagList;
	STRINGLIST TextList;
	bool NoMove;
	bool NoClose;
	UI32 Serial;
	UI16 PageCount;		// Shouldn't have more than 65535 pages
	UI32 Type;
public:
	CGump( bool myNoMove, bool myNoClose );
	~CGump();

	void Add( const std::string& Tag, const std::string& Text );
	void Send( CSocket *target );

	// Common add functions
	void AddBackground( UI16 x, UI16 y, UI16 GumpID, UI16 width, UI16 height );
	void AddGump( UI16 x, UI16 y, UI16 GumpID );
	void AddButton( UI16 x, UI16 y, UI16 ImageUp, UI16 ImageDown, UI16 Behaviour, UI16 Page, UI32 UniqueID  );
	void AddText( UI16 x, UI16 y, UI16 hue, std::string Text );
	UI16 StartPage( void );

	void SetNoMove( bool myNoMove );
	void SetNoClose( bool myNoClose );

	void SetType( UI32 newType );
	void SetSerial( UI32 newSerial );
};

class GumpDisplay
{
private:
	struct GumpInfo
	{
		std::string name;
		UI32 value;
		UI08 type;
		std::string stringValue;
		// acceptable type values
		// 0 -> straight plain UI32
		// 1 -> hex UI32
		// 2 -> UI32 separated into 4 parts, decimal
		// 3 -> UI32 separated into 4 parts, hex
		// 4 -> string
		// 5 -> 2 byte hex display
		// 6 -> 2 byte decimal display
	};

	std::vector< GumpInfo * > gumpData;
	UI16 width, height;	// gump width / height
	CSocket *toSendTo;
	STRINGLIST one, two;
	std::string title;
public:
	void AddData( GumpInfo *toAdd );
	void AddData( std::string toAdd, UI32 value, UI08 type = 0 );
	void AddData( std::string toAdd, const std::string &toSet, UI08 type = 4 );
	GumpDisplay( CSocket *target );
	GumpDisplay( CSocket *target, UI16 gumpWidth, UI16 gumpHeight );
	~GumpDisplay();
	void SetTitle( const std::string& newTitle );
	void Send( UI32 gumpNum, bool isMenu, SERIAL serial );
	void Delete( void );
};

#endif

