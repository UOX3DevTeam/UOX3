/***************************/
/*	CGump Class for easy   */
/*  gump handling		   */
/***************************/

#include "uox3.h"

CGump::CGump( bool myNoMove, bool myNoClose )
{
	NoMove = myNoMove;
	NoClose = myNoClose;

	Type = 21; // Default Type
	Serial = 0; // Do nothing on close
	PageCount = 0;
}

CGump::~CGump()
{
}

// Procedure for "raw" adding new tags/texts
void CGump::Add( std::string Tag, std::string Text )
{
	if( Tag.size() > 0 )
		TagList.push_back( Tag );
	
	if( Text.size() > 0 )
		TextList.push_back( Text );
}

void CGump::Send( cSocket *target )
{
	if( target == NULL )
		return;

	UI08 i = 0;
	char sect[512];

	UI32 length = 21;
	UI32 length2 = 1;

	UI32 TagCount = TagList.size();
	UI32 TextCount = TextList.size();
	UI32 Line, TextLines;

	for( Line = 0; Line < TagCount; Line++ )
	{
		if( TagList[Line].length() == 0 )
			break;
		length += TagList[Line].length() + 4;
		length2 += TagList[Line].length() + 4;
	}
	
	length += 3;
	TextLines = 0;

	for( Line = 0; Line < TextCount; Line++ )
	{
		if( TextList[Line].length() == 0 )
			break;
		length += TextList[Line].length() * 2 + 2;
		TextLines++;
	}
	
	gump1[1] = (UI08)(length>>8);
	gump1[2] = (UI08)(length%256);
	
	if( Serial != INVALIDSERIAL )
	{
		gump1[3] = (UI08)(Serial>>24);
		gump1[4] = (UI08)(Serial>>16);
		gump1[5] = (UI08)(Serial>>8);
		gump1[6] = (UI08)(Serial%256);
	}

	// gump1[3] -> gump1[6] are UID
	gump1[7] = (UI08)(Type>>24);
	gump1[8] = (UI08)(Type>>16);
	gump1[9] = (UI08)(Type>>8);
	gump1[10] = (UI08)(Type%256); // Gump Number
	gump1[19] = (UI08)(length2>>8);
	gump1[20] = (UI08)(length2%256);
	target->Send( gump1, 21 );
	
	for( Line = 0; Line < TagCount; Line++ )
	{
		sprintf( sect, "{ %s }", TagList[Line].c_str() );
		target->Send( sect, strlen( sect ) );
	}
	
	gump2[1] = (UI08)(TextLines>>8);
	gump2[2] = (UI08)(TextLines%256);
	
	target->Send( gump2, 3 );

	for( Line = 0; Line < TextCount; Line++ )
	{
		if( TextList[Line].length() == 0 )
			break;
		gump3[0] = (UI08)(( TextList[Line].length() )>>8);
		gump3[1] = (UI08)(( TextList[Line].length() )%256);
		target->Send( gump3, 2 );
		gump3[0]=0;
		for( i = 0; i < TextList[Line].length(); i++ )
		{
			gump3[1] = (TextList[Line])[i];
			target->Send( gump3, 2 );
		}
	}
}

void CGump::SetNoMove( bool myNoMove )
{
	NoMove = myNoMove;
}

void CGump::SetNoClose( bool myNoClose )
{
	NoClose = myNoClose;
}

void CGump::SetType( UI32 newType )
{
	Type = newType;
}

void CGump::SetSerial( UI32 newSerial )
{
	Serial = newSerial;
}

// Add a background picture
void CGump::AddBackground( UI16 x, UI16 y, UI16 GumpID, UI16 width, UI16 height )
{
	char temp[64];
	sprintf( temp, "resizepic %i %i %i %i %i", x, y, GumpID, width, height );
	TagList.push_back( temp );
}

// Add a gump
void CGump::AddGump( UI16 x, UI16 y, UI16 GumpID )
{
	char temp[64];
	sprintf( temp, "gumppic %i %i %i", x, y, GumpID );
	TagList.push_back( temp );
}

// Add a Button
void CGump::AddButton( UI16 x, UI16 y, UI16 ImageUp, UI16 ImageDown, UI16 Behaviour, UI16 Page, UI32 UniqueID  )
{
	char temp[128];
	sprintf( temp, "button %i %i %i %i %i %i %i", x, y, ImageUp, ImageDown, Behaviour, Page, UniqueID );
	TagList.push_back( temp );
}

// Add an "auto-Text"
void CGump::AddText( UI16 x, UI16 y, UI16 hue, std::string Text )
{
	char temp[128];
	UI32 TextID = TextList.size();
	sprintf( temp, "text %i %i %i %i", x, y, hue, TextID );

	TextList.push_back( Text );
	TagList.push_back( temp );
}

// Start a new page and return the new page no.
UI32 CGump::StartPage( void )
{
	char temp[32];
	sprintf( temp, "page %i", PageCount );
	TagList.push_back( temp );

	PageCount++;
	return PageCount - 1;
}

// Callback for gumps.cpp
void MultiGumpCallback( cSocket *mySocket, SERIAL GumpSerial, UI32 Button )
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

// EOF
 