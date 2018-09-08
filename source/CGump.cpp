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

//o---------------------------------------------------------------------------o
//|   Function    -  SetNoMove( bool myNoMove )
//|   Date        -  Unknown
//|   Programmer  -  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     -  Toggle whether a gump can be moved or not
//o---------------------------------------------------------------------------o
void CGump::SetNoMove( bool myNoMove )
{
	NoMove = myNoMove;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SetNoClose( bool myNoClose )
//|   Date        -  Unknown
//|   Programmer  -  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     -  Toggle whether a gump can be closed or not
//o---------------------------------------------------------------------------o
void CGump::SetNoClose( bool myNoClose )
{
	NoClose = myNoClose;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SetType( UI32 newType )
//|   Date        -  Unknown
//|   Programmer  -  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets Gump's Type
//o---------------------------------------------------------------------------o
void CGump::SetType( UI32 newType )
{
	Type = newType;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SetSerial( UI32 newSerial )
//|   Date        -  Unknown
//|   Programmer  -  UOX3 DevTeam
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets Gump's Serial
//o---------------------------------------------------------------------------o
void CGump::SetSerial( UI32 newSerial )
{
	Serial = newSerial;
}

// Add a background picture
void CGump::AddBackground( UI16 x, UI16 y, UI16 GumpID, UI16 width, UI16 height )
{
	char temp[64];
	sprintf( temp, "resizepic %u %u %u %u %u", x, y, GumpID, width, height );
	TagList.push_back( temp );
}

// Add a gump
void CGump::AddGump( UI16 x, UI16 y, UI16 GumpID )
{
	char temp[64];
	sprintf( temp, "gumppic %u %u %u", x, y, GumpID );
	TagList.push_back( temp );
}

// Add a Button
void CGump::AddButton( UI16 x, UI16 y, UI16 ImageUp, UI16 ImageDown, UI16 Behaviour, UI16 Page, UI32 UniqueID  )
{
	char temp[128];
	sprintf( temp, "button %u %u %u %u %u %u %lu", x, y, ImageUp, ImageDown, Behaviour, Page, UniqueID );
	TagList.push_back( temp );
}

// Add an "auto-Text"
void CGump::AddText( UI16 x, UI16 y, UI16 hue, std::string Text )
{
	char temp[128];
	size_t TextID = TextList.size();
	sprintf( temp, "text %u %u %u %u", x, y, hue, TextID );

	TextList.push_back( Text );
	TagList.push_back( temp );
}

// Start a new page and return the new page no.
UI16 CGump::StartPage( void )
{
	char temp[32];
	sprintf( temp, "page %u", PageCount );
	TagList.push_back( temp );

	++PageCount;
	return PageCount - 1;
}

// Callback for gumps.cpp
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

