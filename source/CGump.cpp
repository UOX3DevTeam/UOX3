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
	{
		TagList.push_back( Tag );
	}

	if( !Text.empty() )
	{
		TextList.push_back( Text );
	}
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump::Send()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends gump to socket
//o------------------------------------------------------------------------------------------------o
void CGump::Send( CSocket *target )
{
	if( target == nullptr )
		return;

	CPSendGumpMenu toSend;
	toSend.GumpId( Type );
	toSend.UserId( Serial );

	std::for_each( TagList.begin(), TagList.end(), [&toSend]( const std::string &entry )
	{
		toSend.addCommand( entry );
	});

	std::for_each( TextList.begin(), TextList.end(), [&toSend]( const std::string &entry )
	{
		toSend.addCommand( entry );
	});

	toSend.Finalize();
	target->Send( &toSend );
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump::SetNoMove()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Toggle whether a gump can be moved or not
//o------------------------------------------------------------------------------------------------o
void CGump::SetNoMove( bool myNoMove )
{
	NoMove = myNoMove;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump::SetNoClose()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Toggle whether a gump can be closed or not
//o------------------------------------------------------------------------------------------------o
void CGump::SetNoClose( bool myNoClose )
{
	NoClose = myNoClose;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump::SetType()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets Gump's Type
//o------------------------------------------------------------------------------------------------o
void CGump::SetType( UI32 newType )
{
	Type = newType;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump::SetSerial()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Sets Gump's Serial
//o------------------------------------------------------------------------------------------------o
void CGump::SetSerial( UI32 newSerial )
{
	Serial = newSerial;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump::AddBackground()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a background picture
//o------------------------------------------------------------------------------------------------o
void CGump::AddBackground( UI16 x, UI16 y, UI16 gumpId, UI16 width, UI16 height )
{
	TagList.push_back( oldstrutil::format( "resizepic %u %u %u %u %u", x, y, gumpId, width, height ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump::AddGump()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a gump
//o------------------------------------------------------------------------------------------------o
void CGump::AddGump( UI16 x, UI16 y, UI16 gumpId )
{
	TagList.push_back( oldstrutil::format( "gumppic %u %u %u", x, y, gumpId ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump::AddButton()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds a button
//o------------------------------------------------------------------------------------------------o
void CGump::AddButton( UI16 x, UI16 y, UI16 imageUp, UI16 imageDown, UI16 behaviour, UI16 page, UI32 uniqueId  )
{
	TagList.push_back( oldstrutil::format( "button %u %u %u %u %u %u %u", x, y, imageUp, imageDown, behaviour, page, uniqueId ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump::AddText()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Adds an "auto-text"
//o------------------------------------------------------------------------------------------------o
void CGump::AddText( UI16 x, UI16 y, UI16 hue, std::string text )
{
	UI32 textId = static_cast<UI32>( TextList.size() );

	TextList.push_back( text );
	TagList.push_back( oldstrutil::format( "text %u %u %u %u", x, y, hue, textId ));
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	CGump::StartPage()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Starts a new page and return the new page no.
//o------------------------------------------------------------------------------------------------o
UI16 CGump::StartPage( void )
{
	TagList.push_back( oldstrutil::format( "page %u", PageCount ));

	++PageCount;
	return PageCount - 1;
}

//o------------------------------------------------------------------------------------------------o
//|	Function	-	MultiGumpCallback()
//o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Callback for gumps.cpp
//o------------------------------------------------------------------------------------------------o
void MultiGumpCallback( [[maybe_unused]] CSocket *mySocket, SERIAL gumpSerial, [[maybe_unused]] UI32 button )
{
	if( gumpSerial == 0 ) // Do nothing on close gump
		return;

	switch( gumpSerial )
	{
			// Custom Gump Callbacks
		case 1:
		{
		}
	};
}
