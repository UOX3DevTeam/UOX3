#include "uox3.h"
#include "msgboard.h"
#include "townregion.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "classes.h"
#include "Dictionary.h"

namespace UOX
{

//o--------------------------------------------------------------------------o
//|	Function		-	MsgBoardOpen( CSocket *mSock )
//|	Date			-	7/16/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Opens the Messageboard and Advises the client of the messages
//o--------------------------------------------------------------------------o
void MsgBoardOpen( CSocket *mSock )
{
	SERIAL boardSer = mSock->GetDWord( 1 );

	CItem *msgBoard = calcItemObjFromSer( boardSer );
	if( !ValidateObject( msgBoard ) )
		return;

	char buffer[4];
	UString fileName, dirPath;
	UI16 postCount = 0;

	CPOpenMessageBoard mbPost( mSock );
	mSock->Send( &mbPost );

	CPSendMsgBoardPosts mbSend;

	CTownRegion *mbRegion = calcRegionFromXY( msgBoard->GetX(), msgBoard->GetY(), msgBoard->WorldNumber() );

	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
		dirPath = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD );

	for( UI08 currentFile = 0; currentFile < 3 && mSock->PostCount() < MAXPOSTS; ++currentFile )
	{
		switch( currentFile )
		{
		case 0:							// Start with the GLOBAL.bbf file first
			fileName = dirPath + "global.bbf";
			break;
		case 1:							// Set fileName to REGIONAL.bbf
			fileName = dirPath + "region" + UString::number( mbRegion->GetRegionNum() ) + ".bbf";
			break;
		case 2:							// Set fileName to LOCAL.bbf
			fileName = dirPath + UString::number( boardSer, 16 ) + ".bbf";
			break;
		}

		std::ifstream file ( fileName.c_str(), std::ios::in | std::ios::binary );
		if( file.is_open() )
		{
			msgBoardPost_st msgBoardPost;

			file.seekg( 0, std::ios::beg );
			while( file && mSock->PostCount() < MAXPOSTS )
			{
				SERIAL tmpSerial	= 0;
				UI08 tmpToggle		= 0;
				UI16 tmpSize		= 0;

				file.read( buffer, 2 );
				tmpSize = static_cast<UI16>((buffer[0]<<8) + buffer[1]);

				file.read( buffer, 1 );
				tmpToggle = buffer[0];

				file.seekg( tmpSize-7, std::ios::cur );

				file.read( buffer, 4 );
				tmpSerial = calcserial( buffer[0], buffer[1], buffer[2], buffer[3] );

				if( !file.fail() )
				{
					if( tmpToggle )	// If it's 0, flagged for deletion
					{
						mbSend.CopyData( tmpSerial, tmpToggle, boardSer );
						mSock->PostAcked( tmpSerial );
					}
				}
			}
			file.close();
		}
	}
	if( mSock->PostCount() > 0 )
	{
		mbSend.Finalize();
		mSock->Send( &mbSend );
		mSock->PostAckCount( 0 );
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	MsgBoardList( CSocket *mSock )
//|	Date			-	7/16/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Sends the summary of each message to the client
//o--------------------------------------------------------------------------o
void MsgBoardList( CSocket *mSock )
{
	char buffer[4];
	UString fileName, dirPath;

	CItem *msgBoard = calcItemObjFromSer( mSock->GetDWord( 4 ) );
	if( !ValidateObject( msgBoard ) )
		return;

	CTownRegion *mbRegion = calcRegionFromXY( msgBoard->GetX(), msgBoard->GetY(), msgBoard->WorldNumber() );

	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
		dirPath = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD );

	for( UI08 currentFile = 0; currentFile < 3 && mSock->PostCount() > 0; ++currentFile )
	{
		switch( currentFile )
		{
		case 0:							// Start with the GLOBAL.bbf file first
			fileName = dirPath + "global.bbf";
			break;
		case 1:							// Set fileName to REGIONAL.bbf
			fileName = dirPath + "region" + UString::number( mbRegion->GetRegionNum() ) + ".bbf";
			break;
		case 2:							// Set fileName to LOCAL.bbf
			fileName = dirPath + UString::number( mSock->GetDWord( 4 ), 16 ) + ".bbf";
			break;
		}

		std::ifstream file ( fileName.c_str(), std::ios::in | std::ios::binary );
		if( file.is_open() )
		{
			size_t totalSize = 0;
			msgBoardPost_st msgBoardPost;

			file.seekg( 0, std::ios::beg );
			while( file )
			{
				file.read( buffer, 2 );
				msgBoardPost.Size = static_cast<UI16>((buffer[0]<<8) + buffer[1]);

				file.seekg( msgBoardPost.Size-6, std::ios::cur );

				file.read( buffer, 4 );
				msgBoardPost.Serial = calcserial( buffer[0], buffer[1], buffer[2], buffer[3] );
				for( SERIAL postAck = mSock->FirstPostAck(); !mSock->FinishedPostAck(); )
				{
					if( msgBoardPost.Serial == postAck )
					{
						postAck = mSock->RemovePostAck();

						file.seekg( totalSize+2, std::ios::beg );

						file.read( buffer, 1 );
						if( buffer[0] == 0x05 )
						{
							file.read( buffer, 4 );
							msgBoardPost.ParentSerial = calcserial( buffer[0], buffer[1], buffer[2], buffer[3] );
						}
						else
							file.seekg( 4, std::ios::cur );

						file.read( buffer, 1 );
						file.read( msgBoardPost.Poster, buffer[0] );
						msgBoardPost.PosterLen = buffer[0];

						file.read( buffer, 1 );
						file.read( msgBoardPost.Subject, buffer[0] );
						msgBoardPost.SubjectLen = buffer[0];

						file.read( buffer, 1 );
						file.read( msgBoardPost.Date, buffer[0] );
						msgBoardPost.DateLen = buffer[0];

						if( file.fail() )
						{
							Console.Warning( 2, "Malformed MessageBoard post, MessageID: 0x%X", msgBoardPost.Serial );
							file.close();
						}
						else
						{
							CPOpenMsgBoardPost mbPost( mSock, msgBoardPost, false );
							mSock->Send( &mbPost );
						}
						break;
					}
					else
						postAck = mSock->NextPostAck();
				}

				totalSize += msgBoardPost.Size;
				file.seekg( totalSize, std::ios::beg );
			}
			file.close();
		}
	}

	if( !mSock->FinishedPostAck() )
	{
		mSock->PostClear();
		Console.Error( 2, "Failed to list all posts for MessageBoard ID: 0x%X", mSock->GetDWord( 4 ) );
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool GetMaxSerial( std::string fileName, UI08 *nextMsgID, PostTypes msgType )
//|	Date			-	7/22/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Updates nextMsgID with the next available message serial.
//o--------------------------------------------------------------------------o
bool GetMaxSerial( const std::string fileName, UI08 *nextMsgID, PostTypes msgType )
{
	SERIAL msgIDSer = ( calcserial( nextMsgID[0], nextMsgID[1], nextMsgID[2], nextMsgID[3] ) );
	if( msgIDSer == INVALIDSERIAL )
	{
		switch( msgType )
		{
		case PT_GLOBAL:		msgIDSer = 0x01000000;		break;
		case PT_REGIONAL:	msgIDSer = 0x02000000;		break;
		case PT_LOCAL:		msgIDSer = 0x03000000;		break;
		}
	}
	else
		++msgIDSer;

	nextMsgID[0] = static_cast<UI08>(msgIDSer>>24);
	nextMsgID[1] = static_cast<UI08>(msgIDSer>>16);
	nextMsgID[2] = static_cast<UI08>(msgIDSer>>8);
	nextMsgID[3] = static_cast<UI08>(msgIDSer%256);

	if( nextMsgID[1] == 0xFF && nextMsgID[2] == 0xFF && nextMsgID[3] == 0xFF )
	{
		Console.Warning( 0, "Maximum Posts reached for board %s, no further posts can be created", fileName.c_str() );
		return false;
	}

	return true;
}

//o--------------------------------------------------------------------------o
//|	Function		-	SERIAL MsgBoardWritePost( msgBoardNewPost_st mbNewPost, std::string fileName, PostTypes msgType )
//|	Date			-	7/22/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Writes a new post to the .bbf file, returning the messages SERIAL
//o--------------------------------------------------------------------------o
SERIAL MsgBoardWritePost( const msgBoardNewPost_st& mbNewPost, const std::string fileName, PostTypes msgType )
{
	SERIAL msgID = INVALIDSERIAL;

	std::string fullFile;
	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
		fullFile = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ) + fileName;

	std::ifstream file ( fullFile.c_str(), std::ios::in | std::ios::ate | std::ios::binary );

	UI08 nextMsgID[4];
	memset( nextMsgID, 0xFF, 4 );
	if( file.is_open() )
	{
		file.seekg( -4, std::ios::cur );
		file.read( (char *)&nextMsgID, 4 );
	}

	if( !GetMaxSerial( fileName, &nextMsgID[0], msgType ) )
	{
		file.close();
		return msgID;
	}
	else
		msgID = calcserial( nextMsgID[0], nextMsgID[1], nextMsgID[2], nextMsgID[3] );

	file.close();

	time_t now;
	time( &now );
	tm timeOfPost	= *localtime( &now );
	UString time	= UString::sprintf( "Day %i @ %i:%02i\0", (timeOfPost.tm_yday+1), timeOfPost.tm_hour, timeOfPost.tm_min );
	time.resize( time.size()+1 );
	UI08 timeSize	= static_cast<UI08>(time.size());
	UI08 posterSize = static_cast<UI08>(mbNewPost.poster.size());
	UI08 subjSize	= static_cast<UI08>(mbNewPost.subject.size());
	UI16 totalSize	= static_cast<UI16>(15 + posterSize + subjSize + timeSize);
	std::vector< std::string >::const_iterator lIter;
	for( lIter = mbNewPost.msgBoardLine.begin(); lIter != mbNewPost.msgBoardLine.end(); ++lIter )
		totalSize += 1 + (*lIter).size();

	char writeSize[2];
	writeSize[0] = static_cast<UI08>(totalSize>>8);
	writeSize[1] = static_cast<UI08>(totalSize%256);

	std::ofstream mFile ( fullFile.c_str(), std::ios::out | std::ios::app | std::ios::binary );

	mFile.write( (const char *)&writeSize, 2 );
	mFile.write( (const char *)&mbNewPost.toggle, 1 );
	mFile.write( (const char *)&mbNewPost.assocID, 4 );
	mFile.write( (const char *)&posterSize, 1 );
	mFile.write( mbNewPost.poster.c_str(), posterSize );
	mFile.write( (const char *)&subjSize, 1 );
	mFile.write( mbNewPost.subject.c_str(), subjSize );
	mFile.write( (const char *)&timeSize, 1 );
	mFile.write( time.c_str(), timeSize );

	UI08 mbLineSize = static_cast<UI08>(mbNewPost.msgBoardLine.size());
	mFile.write( (const char *)&mbLineSize, 1 );

	for( lIter = mbNewPost.msgBoardLine.begin(); lIter != mbNewPost.msgBoardLine.end(); ++lIter )
	{
		UI08 lineSize = static_cast<UI08>((*lIter).size());
		mFile.write( (const char *)&lineSize, 1 );
		mFile.write( (*lIter).c_str(), lineSize );
	}

	mFile.write( (const char *)&nextMsgID, 4 );

	mFile.close();

	return msgID;
}

//o--------------------------------------------------------------------------o
//|	Function		-	MsgBoardPost( CSocket *tSock )
//|	Date			-	7/22/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handles the client requesting to post a new message
//o--------------------------------------------------------------------------o
void MsgBoardPost( CSocket *tSock )
{
	CChar *tChar = tSock->CurrcharObj();
	if( !ValidateObject( tChar ) )
		return;

	PostTypes msgType = static_cast<PostTypes>(tChar->GetPostType());

	if( msgType != PT_LOCAL && tChar->GetCommandLevel() < GM_CMDLEVEL )
	{
		tSock->sysmessage( "Only GM's may post Global or Regional messages" );
		return;
	}

	SERIAL repliedTo = tSock->GetDWord( 8 );
	if( repliedTo > 0 && repliedTo < 0x03000000 )
	{
		tSock->sysmessage( 729 );
		return;
	}

	UString fileName;
	switch( msgType )
	{
		case PT_GLOBAL:			fileName = "global.bbf";												break;
		case PT_REGIONAL:		CItem *msgBoard;
								CTownRegion *mbRegion;
								msgBoard = calcItemObjFromSer( tSock->GetDWord( 4 ) );
								mbRegion = calcRegionFromXY( msgBoard->GetX(), msgBoard->GetY(), msgBoard->WorldNumber() );
								fileName = "region" + UString::number( mbRegion->GetRegionNum() ) + ".bbf";
																										break;
		case PT_LOCAL:			fileName = UString::number( tSock->GetDWord( 4 ), 16 ) + ".bbf";		break;
		default:				Console.Error( 1, "MsgBoardPost() Invalid post type, aborting post" );
								tSock->sysmessage( 725 );
																										return;
	}

	std::vector< UI08 > internalBuffer;

	UI16 length = tSock->GetWord( 1 );
	internalBuffer.resize( length );
	memcpy( &internalBuffer[0], tSock->Buffer(), length );

	msgBoardNewPost_st mbNewPost;

	mbNewPost.toggle = 0x05;

	memcpy( mbNewPost.assocID, &internalBuffer[8], 4 );

	UI08 i		= 0;
	UI16 offset = 11;

	mbNewPost.poster.resize( tChar->GetName().size()+1 );
	for( i = 0; i < tChar->GetName().size(); ++i )
		mbNewPost.poster[i] = tChar->GetName()[i];

	mbNewPost.subject.resize( internalBuffer[++offset] );
	for( i = 0; i < mbNewPost.subject.size(); ++i )
		mbNewPost.subject[i] = internalBuffer[++offset];

	mbNewPost.msgBoardLine.resize( internalBuffer[++offset] );
	std::vector< std::string >::iterator lIter;
	for( lIter = mbNewPost.msgBoardLine.begin(); lIter != mbNewPost.msgBoardLine.end(); ++lIter )
	{
		(*lIter).resize( internalBuffer[++offset] );
		for( i = 0; i < (*lIter).size(); ++i )
			(*lIter)[i] = internalBuffer[++offset];
	}

	SERIAL msgID = MsgBoardWritePost( mbNewPost, fileName, msgType );
	if( msgID != INVALIDSERIAL )
	{
		CPAddItemToCont toAdd;
		toAdd.Serial( msgID );
		toAdd.Container( tSock->GetDWord( 4 ) );
		tSock->Send( &toAdd );

		// Setup buffer to expect to receive an ACK from the client for this posting
		tSock->PostAckCount( 0 );
		tSock->PostAcked( msgID );
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	MsgBoardOpenPost( CSocket *mSock )
//|	Date			-	7/16/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Opens a post on a message board when double clicked.
//o--------------------------------------------------------------------------o
void MsgBoardOpenPost( CSocket *mSock )
{
	std::string fileName;
	msgBoardPost_st msgBoardPost;
	char buffer[4];
	char tmpLine[256];
	size_t totalSize	= 0;
	bool foundEntry		= false;

	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
		fileName = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD );

	SERIAL msgSerial = mSock->GetDWord( 8 );
	if( msgSerial >= 0x01000000 && msgSerial <= 0x01FFFFFF )
		fileName += "global.bbf";
	else if( msgSerial >= 0x02000000 && msgSerial <= 0x02FFFFFF )
	{
		CItem *msgBoard = calcItemObjFromSer( mSock->GetDWord( 4 ) );
		CTownRegion *mbRegion = calcRegionFromXY( msgBoard->GetX(), msgBoard->GetY(), msgBoard->WorldNumber() );
		fileName += "region" + UString::number( mbRegion->GetRegionNum() ) + ".bbf";
	}
	else if( msgSerial >= 0x03000000 && msgSerial <= 0xFFFFFFFF )
		fileName += UString::number( mSock->GetDWord( 4 ), 16 ) + ".bbf";
	else
	{
		Console.Error( 1, "MsgBoardOpenPost() Invalid message SN: 0x%X", mSock->GetDWord( 8 ) );
		mSock->sysmessage( 732 );
		return;
	}

	std::ifstream file ( fileName.c_str(), std::ios::in | std::ios::binary );
	if( file.is_open() )
	{
		file.seekg( 0, std::ios::beg );
		while( file && !foundEntry )
		{
			file.read( buffer, 2 );
			msgBoardPost.Size = ( (buffer[0]<<8) + buffer[1] );

			file.seekg( msgBoardPost.Size-6, std::ios::cur );

			file.read( buffer, 4 );
			msgBoardPost.Serial = calcserial( buffer[0], buffer[1], buffer[2], buffer[3] );
			if( msgBoardPost.Serial == mSock->GetDWord( 8 ) )
			{
				foundEntry = true;

				file.seekg( totalSize+7, std::ios::beg );

				file.read( buffer, 1 );
				file.read( msgBoardPost.Poster, buffer[0] );
				msgBoardPost.PosterLen = buffer[0];

				file.read( buffer, 1 );
				file.read( msgBoardPost.Subject, buffer[0] );
				msgBoardPost.SubjectLen = buffer[0];

				file.read( buffer, 1 );
				file.read( msgBoardPost.Date, buffer[0] );
				msgBoardPost.DateLen = buffer[0];

				file.read( buffer, 1 );
				msgBoardPost.Lines = buffer[0];

				for( UI08 i = 0; i < msgBoardPost.Lines; ++i )
				{
					file.read( buffer, 1 );
					file.read( tmpLine, buffer[0] );

					msgBoardPost.msgBoardLine.push_back( tmpLine );
				}
				if( file.fail() )
				{
					Console.Warning( 2, "Malformed MessageBoard post, MessageID: 0x%X", msgSerial );
					file.close();
					return;
				}
			}
			else
			{
				totalSize += msgBoardPost.Size;
				file.seekg( totalSize, std::ios::beg );
			}
		}
		file.close();
	}
	else
		Console.Error( 2, "Failed to open MessageBoard file for reading MessageID: 0x%X", msgSerial );

	if( foundEntry )
	{
		CPOpenMsgBoardPost mbPost( mSock, msgBoardPost, true );
		mSock->Send( &mbPost );
	}
	else
		Console.Warning( 2, "Failed to find MessageBoard file for MessageID: 0x%X", msgSerial );
}

//o--------------------------------------------------------------------------o
//|	Function		-	MsgBoardRemovePost( CSocket *mSock )
//|	Date			-	7/17/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Removes a post from a message board
//|
//|						Note: The client should trigger this function, but I have
//|						been unable to find a way through the client to send the
//|						remove post packet.
//o--------------------------------------------------------------------------o
void MsgBoardRemovePost( CSocket *mSock )
{
	CItem *msgBoard = calcItemObjFromSer( mSock->GetDWord( 4 ) );
	if( !ValidateObject( msgBoard ) )
		return;

	UString fileName;

	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
		fileName = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD );

	switch( mSock->GetByte( 8 ) )
	{
	case 0x01:
		fileName += "global.bbf";
		break;
	case 0x02:
		CTownRegion *mbRegion;
		mbRegion = calcRegionFromXY( msgBoard->GetX(), msgBoard->GetY(), msgBoard->WorldNumber() );
		fileName += "region" + UString::number( mbRegion->GetRegionNum() ) + ".bbf";
		break;
	default:
		fileName += UString::number( mSock->GetDWord( 4 ), 16 ) + ".bbf";
		break;
	}

	SERIAL tmpSerial	= 0;
	size_t totalSize	= 0;
	UI16 tmpSize		= 0;
	char rBuffer[4];
	bool foundPost = false;

	std::fstream file;
	file.open( fileName.c_str(), std::ios::in | std::ios::out | std::ios::binary );
	if( file.is_open() )
	{
		while( !file.eof() )
		{
			file.read( rBuffer, 2 );
			tmpSize = ( (rBuffer[0]<<8) + rBuffer[1] );

			file.seekg( tmpSize-6, std::ios::cur );

			file.read( rBuffer, 4 );
			tmpSerial = calcserial( rBuffer[0], rBuffer[1], rBuffer[2], rBuffer[3] );
			if( tmpSerial == mSock->GetDWord( 8 ) )
			{
				foundPost = true;

				file.seekp( totalSize+2, std::ios::beg );
				file.put( 0x00 );

				mSock->sysmessage( 734 );
			}
			else
			{
				totalSize += tmpSize;
				file.seekg( totalSize, std::ios::beg );
			}
		}
		file.close();
	}
	else
		Console.Error( 1, "Could not open file %s for reading", fileName.c_str() );

	if( foundPost )
	{
		CPRemoveItem toRemove;
		toRemove.Serial( mSock->GetDWord( 8 ) );
		mSock->Send( &toRemove );
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool CPIMsgBoardEvent::Handle( void )
//|	Date			-	7/22/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handles a mesage board event from the client.
//o--------------------------------------------------------------------------o
bool CPIMsgBoardEvent::Handle( void )
{
	UI08 msgType = tSock->GetByte( 3 );

	if( tSock->GetByte( 0 ) == 0x06 )
		msgType = 0;

	switch( msgType )
	{
	case 0:			MsgBoardOpen( tSock );				break;
	case 3:			MsgBoardOpenPost( tSock );			break;
	case 4:
					tSock->PostAckCount( tSock->PostAckCount() + 1 );
					if( tSock->PostAckCount() != tSock->PostCount() )
						return true;

					tSock->PostAckCount( 0 );
					MsgBoardList( tSock );
														break;
	case 5:			MsgBoardPost( tSock );				break;
	case 6:			MsgBoardRemovePost( tSock );		break;
	default:											break;
	}
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool MsgBoardPostQuest( CChar *mNPC, QuestTypes questType )
//|	Date			-	7/23/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Creates an escort quest post on regional messageboards
//o--------------------------------------------------------------------------o
bool MsgBoardPostQuest( CChar *mNPC, QuestTypes questType )
{
	msgBoardNewPost_st mbNewPost;
	UString sect, tag, data;
	std::string fileName		= "region" + UString::number( mNPC->GetQuestOrigRegion() ) + ".bbf";
	ScriptSection *EscortData	= NULL, *Escort = NULL;
	size_t totalEntries			= 0;

	switch( questType )
	{
	case QT_ESCORTQUEST:
		mbNewPost.toggle	= QT_ESCORTQUEST;
		mbNewPost.subject	= Dictionary->GetEntry( 735 );
		Escort				= FileLookup->FindEntry( "ESCORTS", msgboard_def );
		if( Escort == NULL )
			return false;

		totalEntries = Escort->NumEntries();
		if( totalEntries == 0 )
		{
			Console.Error( 1, "MsgBoardPostQuest() No msgboard dfn entries found" );
			return false;
		}

		Escort->MoveTo( RandomNum( static_cast<size_t>(0), totalEntries-1 ) );
		sect		= "ESCORT " + Escort->GrabData();
		EscortData	= FileLookup->FindEntry( sect, msgboard_def );
		if( EscortData == NULL )
		{
			Console.Error( 1, "MsgBoardPostQuest() Couldn't find entry %s", sect.c_str() );
			return false;
		}
		for( tag = EscortData->First(); !EscortData->AtEnd(); tag = EscortData->Next() )
		{
			std::string fullLine = tag;

			size_t position = fullLine.find( "%n" );
			while( position != std::string::npos )
			{
				fullLine.replace( position, 2, mNPC->GetName() );
				position = fullLine.find( "%n" );
			}

			position = fullLine.find( "%l" );
			while( position != std::string::npos )
			{
				fullLine.replace( position, 2, UString::sprintf( "%d %d", mNPC->GetX(), mNPC->GetY() ) );
				position = fullLine.find( "%l" );
			}

			position = fullLine.find( "%t" );
			while( position != std::string::npos )
			{
				fullLine.replace( position, 2, mNPC->GetTitle() );
				position = fullLine.find( "%t" );
			}

			position = fullLine.find( "%r" );
			while( position != std::string::npos )
			{
				fullLine.replace( position, 2, regions[mNPC->GetQuestDestRegion()]->GetName() );
				position = fullLine.find( "%r" );
			}

			position = fullLine.find( "%R" );
			while( position != std::string::npos )
			{
				fullLine.replace( position, 2, mNPC->GetRegion()->GetName() );
				position = fullLine.find( "%R" );
			}

			fullLine.resize( fullLine.size() + 1 );
			mbNewPost.msgBoardLine.push_back( fullLine );
		}
		break;
	case QT_BOUNTYQUEST:
		mbNewPost.subject	= Dictionary->GetEntry( 736 );
		mbNewPost.toggle	= QT_BOUNTYQUEST;
		break;
	case QT_ITEMQUEST:
		mbNewPost.subject	= Dictionary->GetEntry( 737 );
		mbNewPost.toggle	= QT_ITEMQUEST;
		break;
	default:
		Console.Error( 1, "MsgBoardPostQuest() Invalid questType %d", questType );
		return false;
	}

	mbNewPost.subject.resize( mbNewPost.subject.size() + 1 );

	UI08 i = 0;

	mbNewPost.poster.resize( mNPC->GetName().size()+1 );
	for( i = 0; i < mNPC->GetName().size(); ++i )
		mbNewPost.poster[i] = mNPC->GetName()[i];

	for( i = 0; i < 4; ++i )
		mbNewPost.assocID[i] = mNPC->GetSerial( i+1 );

	if( MsgBoardWritePost( mbNewPost, fileName, PT_REGIONAL ) != INVALIDSERIAL )
		return true;

	return false;
}

//o--------------------------------------------------------------------------o
//|	Function		-	MsgBoardQuestEscortCreate( CChar *mNPC )
//|	Date			-	7/23/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Creates an escort quest assigning it a valid escort region
//o--------------------------------------------------------------------------o
void MsgBoardQuestEscortCreate( CChar *mNPC )
{
	UI08 npcRegion		= mNPC->GetRegionNum();
	UI08 destRegion		= npcRegion;
	size_t escortSize	= cwmWorldState->escortRegions.size();

	if( !cwmWorldState->escortRegions.empty() && ( escortSize > 1 || cwmWorldState->escortRegions[0] != npcRegion )  )
	{
		for( UI08 i = 0; i < 50 && npcRegion == destRegion; ++i )
			destRegion = cwmWorldState->escortRegions[RandomNum( static_cast<size_t>(0), (escortSize-1) )];
	}

	if( destRegion == 0 || destRegion == npcRegion )
	{
		Console.Error( 2, "MsgBoardQuestEscortCreate() No valid regions defined for escort quests" );
		mNPC->Delete();
		return;
	}

	mNPC->SetQuestDestRegion( destRegion );
	
	// Set quest type to escort
	mNPC->SetQuestType( QT_ESCORTQUEST );
	
	// Make sure they don't move until an player accepts the quest
	mNPC->SetNpcWander( 0 );
	mNPC->SetNPCAiType( aiNOAI );
	mNPC->SetQuestOrigRegion( npcRegion );
	
	if( cwmWorldState->ServerData()->SystemTimer( tSERVER_ESCORTWAIT ) )
		mNPC->SetTimer( tNPC_SUMMONTIME, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_ESCORTWAIT ) );

	if( !MsgBoardPostQuest( mNPC, QT_ESCORTQUEST ) )
	{
		Console.Error( 3, "MsgBoardQuestEscortCreate() Failed to add quest post for %s", mNPC->GetName().c_str() );
		mNPC->Delete();
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	MsgBoardQuestEscortArrive( CSocket *mSock, CChar *mNPC )
//|	Date			-	7/23/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Handles payment and release upon arrival of the escort
//o--------------------------------------------------------------------------o
void MsgBoardQuestEscortArrive( CSocket *mSock, CChar *mNPC )
{
	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ) )
		return;

	CTownRegion *destReg = regions[mNPC->GetQuestDestRegion()];

	// Calculate payment for services rendered
	UI16 servicePay = ( RandomNum(0, 20) * RandomNum(1, 30) );  // Equals a range of 0 to 600 possible gold with a 5% chance of getting 0 gold

	// If they have no money, well, oops!
	if( servicePay == 0 )
		mNPC->talk( mSock, 738, false, mChar->GetName().c_str(), destReg->GetName().c_str() );
	else // Otherwise pay the poor sod for his time
	{
		// Less than 75 gold for a escort is pretty cheesey, so if its between 1 and 75, add a randum amount of between 75 to 100 gold
		if( servicePay < 75 ) 
			servicePay += RandomNum( 75, 100 );
		Items->CreateItem( mSock, mChar, 0x0EED, servicePay, 0, OT_ITEM, true );
		Effects->goldSound( mSock, servicePay );
		mNPC->talk( mSock, 739, false, mChar->GetName().c_str(), destReg->GetName().c_str() );
	}
	
	// Inform the PC of what he has just been given as payment
	mSock->sysmessage( 740, servicePay, mNPC->GetName().c_str(), mNPC->GetTitle().c_str() );
	
	// Take the NPC out of quest mode
	mNPC->SetNpcWander( 2 );         // Wander freely
	mNPC->SetFTarg( NULL );			 // Reset follow target
	mNPC->SetQuestType( 0 );         // Reset quest type
	mNPC->SetQuestDestRegion( 0 );   // Reset quest destination region
	
	// Set a timer to automatically delete the NPC
	mNPC->SetTimer( tNPC_SUMMONTIME, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_ESCORTDONE ) );
	mNPC->SetOwner( NULL );
}

//o--------------------------------------------------------------------------o
//|	Function		-	MsgBoardQuestEscortRemovePost( CChar *mNPC )
//|	Date			-	7/23/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Removes an escort quest post on regional messageboards
//o--------------------------------------------------------------------------o
void MsgBoardQuestEscortRemovePost( CChar *mNPC )
{
	UString fileName;
	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
		fileName = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD );

	fileName += "region" + UString::number( mNPC->GetQuestOrigRegion() ) + ".bbf";

	SERIAL tmpSerial	= 0;
	size_t totalSize	= 0;
	UI16 tmpSize		= 0;
	char rBuffer[4];

	std::fstream file;
	file.open( fileName.c_str(), std::ios::in | std::ios::out | std::ios::binary );
	if( file.is_open() )
	{
		file.seekg( 0, std::ios::beg );
		while( !file.eof() )
		{
			file.read( rBuffer, 2 );
			tmpSize = ( (rBuffer[0]<<8) + rBuffer[1] );

			file.seekg( 1, std::ios::cur );

			file.read( rBuffer, 4 );
			tmpSerial = calcserial( rBuffer[0], rBuffer[1], rBuffer[2], rBuffer[3] );
			if( tmpSerial == mNPC->GetSerial() )
			{
				file.seekp( totalSize + 2, std::ios::beg );
				file.put( 0x00 );
				break;
			}
			else
			{
				totalSize += tmpSize;
				file.seekg( totalSize, std::ios::beg );
			}
		}
		file.close();
	}
	else
		Console.Error( 1, "Could not open file %s for reading", fileName.c_str() );
}


//////////////////////////////////////////////////////////////////////////////
// FUNCTION:    MsgBoardMaintenance( void )
//
// PURPOSE:     Cleans out old posts which are older than the MSGRETENTION
//              period set in SERVER.SCP and any posts that have been marked
//              for deletion (such as escort quests after they have been 
//              accepted or posts that have been "removed" through the user
//              interface.  This is called as a cleanup routine on server
//              startup.  Compacts and reassigns message serial numbers.
//
// PARAMETERS:  void
//
// RETURNS:     void
//
// NOTES:       This function uses the _findfirst() and _findnext() functions
//              which are OS specific.  There will definetly be an issue in 
//              compiling this on LINUX because I have no idea what the
//              structure to be passed to the functions needs to be for LINUX.
//              This will definetly have to be #ifdef'd to handle this.
//              Anyone with LINUX experience please feel free to fix it up.
//////////////////////////////////////////////////////////////////////////////
void MsgBoardMaintenance( void )
{
   return;
}

}
