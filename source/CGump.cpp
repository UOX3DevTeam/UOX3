/***************************/
/*	CGump Class for easy   */
/*  gump handling		   */
/***************************/

#include "uox3.h"
#include "gump.h"
#include "CPacketSend.h"
#include <cstdint>


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
	if( target == nullptr )
		return;

	CPSendGumpMenu toSend;
	toSend.GumpID( Type );
	toSend.UserID( Serial );

	STRINGLIST_CITERATOR tIter;
	for( tIter = TagList.begin(); tIter != TagList.end(); ++tIter )
		toSend.addCommand( (*tIter) );

	for( tIter = TextList.begin(); tIter != TextList.end(); ++tIter )
		toSend.addText( (*tIter) );

	toSend.Finalize();
	target->Send( &toSend );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetNoMove( bool myNoMove )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Toggle whether a gump can be moved or not
//o-----------------------------------------------------------------------------------------------o
void CGump::SetNoMove( bool myNoMove )
{
	NoMove = myNoMove;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetNoClose( bool myNoClose )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Toggle whether a gump can be closed or not
//o-----------------------------------------------------------------------------------------------o
void CGump::SetNoClose( bool myNoClose )
{
	NoClose = myNoClose;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetType( UI32 newType )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets Gump's Type
//o-----------------------------------------------------------------------------------------------o
void CGump::SetType( UI32 newType )
{
	Type = newType;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void SetSerial( UI32 newSerial )
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
	TagList.push_back( strutil::format("resizepic %u %u %u %u %u", x, y, GumpID, width, height ) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddGump( UI16 x, UI16 y, UI16 GumpID )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a gump
//o-----------------------------------------------------------------------------------------------o
void CGump::AddGump( UI16 x, UI16 y, UI16 GumpID )
{
	TagList.push_back( strutil::format( "gumppic %u %u %u", x, y, GumpID ) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddButton( UI16 x, UI16 y, UI16 ImageUp, UI16 ImageDown, UI16 Behaviour, UI16 Page, UI32 UniqueID  )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a button
//o-----------------------------------------------------------------------------------------------o
void CGump::AddButton( UI16 x, UI16 y, UI16 ImageUp, UI16 ImageDown, UI16 Behaviour, UI16 Page, UI32 UniqueID  )
{
	TagList.push_back( strutil::format("button %u %u %u %u %u %u %u", x, y, ImageUp, ImageDown, Behaviour, Page, UniqueID ) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void AddText( UI16 x, UI16 y, UI16 hue, std::string Text )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds an "auto-text"
//o-----------------------------------------------------------------------------------------------o
void CGump::AddText( UI16 x, UI16 y, UI16 hue, std::string Text )
{
	UI32 TextID = static_cast<std::uint32_t>(TextList.size());

	TextList.push_back( Text );
	TagList.push_back( strutil::format( "text %u %u %u %u", x, y, hue, TextID ) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	UI16 StartPage( void )
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Starts a new page and return the new page no.
//o-----------------------------------------------------------------------------------------------o
UI16 CGump::StartPage( void )
{
	TagList.push_back( strutil::format( "page %u", PageCount ) );

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

