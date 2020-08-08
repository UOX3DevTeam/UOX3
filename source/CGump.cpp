/***************************/
/*	CGump Class for easy   */
/*  gump handling		   */
/***************************/

#include "uox3.h"
#include "gump.h"
#include "CPacketSend.h"

namespace UOX
{

CGump::CGump( bool myNoMove, bool myNoClose )
{
	NoMove = myNoMove;
	NoClose = myNoClose;

	Type = 21; // Default Type
	Serial = 0;	// Do nothing on close
	PageCount = 0;
}

CGump::~CGump()
{
}

// Procedure for "raw" adding new tags/texts
void CGump::Add( const std::string& Tag, const std::string& Text )
{
	if( !Tag.empty() )
		TagList.push_back( Tag );

	if( !Text.empty() )
		TextList.push_back( Text );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void CGump::Send( CSocket *target )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends gump to socket
//o-----------------------------------------------------------------------------------------------o
void CGump::Send( CSocket *target )
{
	if( target == NULL )
		return;

	CPSendGumpMenu toSend;
	toSend.GumpID( Type );
	toSend.UserID( Serial );

	STRINGLIST_CITERATOR tIter;
	for( tIter = TagList.begin(); tIter != TagList.end(); ++tIter )
		toSend.AddCommand( (*tIter) );

	for( tIter = TextList.begin(); tIter != TextList.end(); ++tIter )
		toSend.AddText( (*tIter) );

	toSend.Finalize();
	target->Send( &toSend );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetNoMove( bool myNoMove )
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Toggle whether a gump can be moved or not
//o-----------------------------------------------------------------------------------------------o
void CGump::SetNoMove( bool myNoMove )
{
	NoMove = myNoMove;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetNoClose( bool myNoClose )
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Toggle whether a gump can be closed or not
//o-----------------------------------------------------------------------------------------------o
void CGump::SetNoClose( bool myNoClose )
{
	NoClose = myNoClose;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetType( UI32 newType )
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets Gump's Type
//o-----------------------------------------------------------------------------------------------o
void CGump::SetType( UI32 newType )
{
	Type = newType;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetSerial( UI32 newSerial )
//|	Org/Team	-	UOX3 DevTeam
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets Gump's Serial
//o-----------------------------------------------------------------------------------------------o
void CGump::SetSerial( UI32 newSerial )
{
	Serial = newSerial;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddBackground( UI16 x, UI16 y, UI16 GumpID, UI16 width, UI16 height )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a background picture
//o-----------------------------------------------------------------------------------------------o
void CGump::AddBackground( UI16 x, UI16 y, UI16 GumpID, UI16 width, UI16 height )
{
	char temp[64];
	sprintf( temp, "resizepic %u %u %u %u %u", x, y, GumpID, width, height );
	TagList.push_back( temp );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddGump( UI16 x, UI16 y, UI16 GumpID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a gump
//o-----------------------------------------------------------------------------------------------o
void CGump::AddGump( UI16 x, UI16 y, UI16 GumpID )
{
	char temp[64];
	sprintf( temp, "gumppic %u %u %u", x, y, GumpID );
	TagList.push_back( temp );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddButton( UI16 x, UI16 y, UI16 ImageUp, UI16 ImageDown, UI16 Behaviour, UI16 Page, UI32 UniqueID  )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a button
//o-----------------------------------------------------------------------------------------------o
void CGump::AddButton( UI16 x, UI16 y, UI16 ImageUp, UI16 ImageDown, UI16 Behaviour, UI16 Page, UI32 UniqueID  )
{
	char temp[128];
	sprintf( temp, "button %u %u %u %u %u %u %lu", x, y, ImageUp, ImageDown, Behaviour, Page, UniqueID );
	TagList.push_back( temp );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddText( UI16 x, UI16 y, UI16 hue, std::string Text )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds an "auto-text"
//o-----------------------------------------------------------------------------------------------o
void CGump::AddText( UI16 x, UI16 y, UI16 hue, std::string Text )
{
	char temp[128];
	unsigned int TextID = TextList.size();
	sprintf( temp, "text %u %u %u %u", x, y, hue, TextID );

	TextList.push_back( Text );
	TagList.push_back( temp );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 StartPage( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Starts a new page and return the new page no.
//o-----------------------------------------------------------------------------------------------o
UI16 CGump::StartPage( void )
{
	char temp[32];
	sprintf( temp, "page %u", PageCount );
	TagList.push_back( temp );

	++PageCount;
	return PageCount - 1;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MultiGumpCallback( CSocket *mySocket, SERIAL GumpSerial, UI32 Button )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Callback for gumps.cpp
//o-----------------------------------------------------------------------------------------------o
void MultiGumpCallback( CSocket *mySocket, SERIAL GumpSerial, UI32 Button )
{
	if( GumpSerial == 0 ) // Do nothing on close gump
		return;

	switch( GumpSerial )
	{
		// Custom Gump Callbacks
		case 1:
			{

			}
	};
}

}

