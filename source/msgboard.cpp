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
//|	Function		-	std::string GetMsgBoardFile( const SERIAL msgBoardSer, const UI08 msgType )
//|	Date			-	8/6/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Creates the proper MessageBoard filename based on the messageType and Borad Serial
//o--------------------------------------------------------------------------o
std::string GetMsgBoardFile( const SERIAL msgBoardSer, const UI08 msgType )
{
	std::string fileName;
	switch( msgType )
	{
		case PT_GLOBAL:			fileName = "global.bbf";												break;
		case PT_REGIONAL:		CItem *msgBoard;
								CTownRegion *mbRegion;
								msgBoard = calcItemObjFromSer( msgBoardSer );
								mbRegion = calcRegionFromXY( msgBoard->GetX(), msgBoard->GetY(), msgBoard->WorldNumber() );
								fileName = "region" + UString::number( mbRegion->GetRegionNum() ) + ".bbf";
																										break;
		case PT_LOCAL:			fileName = UString::number( msgBoardSer, 16 ) + ".bbf";					break;
		default:				Console.Error( 1, "GetMsgBoardFile() Invalid post type, aborting" );
																										break;
	}
	return fileName;
}

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
	const SERIAL boardSer = mSock->GetDWord( 1 );

	CItem *msgBoard = calcItemObjFromSer( boardSer );
	if( !ValidateObject( msgBoard ) )
		return;

	char buffer[4];
	std::string fileName, dirPath;

	mSock->PostClear();

	CPOpenMessageBoard mbPost( mSock );
	mSock->Send( &mbPost );

	CPSendMsgBoardPosts mbSend;

	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
		dirPath = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD );


	for( UI08 currentFile = 1; currentFile < 4 && mSock->PostCount() < MAXPOSTS; ++currentFile )
	{
		fileName = dirPath + GetMsgBoardFile( boardSer, currentFile );

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
				SERIAL repliedTo	= 0;

				file.read( buffer, 2 );
				tmpSize = static_cast<UI16>((buffer[0]<<8) + buffer[1]);

				file.read( buffer, 1 );
				tmpToggle = buffer[0];

				file.read( buffer, 4 );
				repliedTo = calcserial( buffer[0], buffer[1], buffer[2], buffer[3] );

				file.seekg( tmpSize-11, std::ios::cur );

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
	std::string fileName, dirPath;

	CItem *msgBoard = calcItemObjFromSer( mSock->GetDWord( 4 ) );
	if( !ValidateObject( msgBoard ) )
		return;

	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
		dirPath = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD );

	for( UI08 currentFile = 1; currentFile < 4 && mSock->PostCount() > 0; ++currentFile )
	{
		fileName = dirPath + GetMsgBoardFile( msgBoard->GetSerial(), currentFile );

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
bool GetMaxSerial( const std::string fileName, UI08 *nextMsgID, const PostTypes msgType )
{
	SERIAL msgIDSer = ( calcserial( nextMsgID[0], nextMsgID[1], nextMsgID[2], nextMsgID[3] ) );
	if( msgIDSer == INVALIDSERIAL )
	{
		switch( msgType )
		{
		case PT_GLOBAL:		msgIDSer = BASEGLOBALPOST;		break;
		case PT_REGIONAL:	msgIDSer = BASEREGIONPOST;		break;
		case PT_LOCAL:		msgIDSer = BASELOCALPOST;		break;
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
//|	Function		-	void MsgBoardWritePost( std::ofstream& mFile, const msgBoardPost_st& msgBoardPost )
//|	Date			-	8/10/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Writes a new post to the .bbf file
//o--------------------------------------------------------------------------o
void MsgBoardWritePost( std::ofstream& mFile, const msgBoardPost_st& msgBoardPost )
{
	char wBuffer[4];

	wBuffer[0] = static_cast<char>(msgBoardPost.Size>>8);
	wBuffer[1] = static_cast<char>(msgBoardPost.Size%256);
	mFile.write( (const char *)&wBuffer, 2 );

	mFile.write( (const char *)&msgBoardPost.Toggle, 1 );

	wBuffer[0] = static_cast<char>(msgBoardPost.ParentSerial>>24);
	wBuffer[1] = static_cast<char>(msgBoardPost.ParentSerial>>16);
	wBuffer[2] = static_cast<char>(msgBoardPost.ParentSerial>>8);
	wBuffer[3] = static_cast<char>(msgBoardPost.ParentSerial%256);
	mFile.write( (const char *)&wBuffer, 4 );

	mFile.write( (const char *)&msgBoardPost.PosterLen, 1 );
	mFile.write( (const char *)&msgBoardPost.Poster, msgBoardPost.PosterLen );
	mFile.write( (const char *)&msgBoardPost.SubjectLen, 1 );
	mFile.write( (const char *)&msgBoardPost.Subject, msgBoardPost.SubjectLen );
	mFile.write( (const char *)&msgBoardPost.DateLen, 1 );
	mFile.write( (const char *)&msgBoardPost.Date, msgBoardPost.DateLen );

	mFile.write( (const char *)&msgBoardPost.Lines, 1 );
	STRINGLIST_CITERATOR lIter;
	for( lIter = msgBoardPost.msgBoardLine.begin(); lIter != msgBoardPost.msgBoardLine.end(); ++lIter )
	{
		const UI08 lineSize = static_cast<UI08>((*lIter).size());
		mFile.write( (const char *)&lineSize, 1 );
		mFile.write( (*lIter).c_str(), lineSize );
	}

	wBuffer[0] = static_cast<char>(msgBoardPost.Serial>>24);
	wBuffer[1] = static_cast<char>(msgBoardPost.Serial>>16);
	wBuffer[2] = static_cast<char>(msgBoardPost.Serial>>8);
	wBuffer[3] = static_cast<char>(msgBoardPost.Serial%256);
	mFile.write( (const char *)&wBuffer, 4 );
}

//o--------------------------------------------------------------------------o
//|	Function		-	SERIAL MsgBoardWritePost( msgBoardPost_st& msgBoardPost, const std::string fileName, PostTypes msgType )
//|	Date			-	7/22/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Writes a new post to the .bbf file, returning the messages SERIAL
//o--------------------------------------------------------------------------o
SERIAL MsgBoardWritePost( msgBoardPost_st& msgBoardPost, const std::string fileName, const PostTypes msgType )
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
		file.close();
	}

	if( !GetMaxSerial( fileName, &nextMsgID[0], msgType ) )
		return msgID;
	else
		msgID = calcserial( nextMsgID[0], nextMsgID[1], nextMsgID[2], nextMsgID[3] );

	time_t now;
	time( &now );
	tm timeOfPost			= *localtime( &now );
	UString time			= UString::sprintf( "Day %i @ %i:%02i\0", (timeOfPost.tm_yday+1), timeOfPost.tm_hour, timeOfPost.tm_min );
	time.resize( time.size()+1 );
	const UI08 timeSize		= static_cast<UI08>(time.size());
	const UI08 posterSize	= static_cast<UI08>(msgBoardPost.PosterLen);
	const UI08 subjSize		= static_cast<UI08>(msgBoardPost.SubjectLen);
	UI16 totalSize			= static_cast<UI16>(15 + posterSize + subjSize + timeSize);
	STRINGLIST_CITERATOR lIter;
	for( lIter = msgBoardPost.msgBoardLine.begin(); lIter != msgBoardPost.msgBoardLine.end(); ++lIter )
		totalSize += 1 + (*lIter).size();

	msgBoardPost.Size			= totalSize;
	msgBoardPost.DateLen		= timeSize;
	strncpy( msgBoardPost.Date, time.c_str(), timeSize );
	msgBoardPost.Serial			= msgID;

	std::ofstream mFile ( fullFile.c_str(), std::ios::out | std::ios::app | std::ios::binary );

	MsgBoardWritePost( mFile, msgBoardPost );

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

	const PostTypes msgType = static_cast<PostTypes>(tChar->GetPostType());

	if( msgType != PT_LOCAL && tChar->GetCommandLevel() < GM_CMDLEVEL )
	{
		tSock->sysmessage( "Only GM's may post Global or Regional messages" );
		return;
	}

	SERIAL repliedTo = tSock->GetDWord( 8 );
	if( (repliedTo&BASEITEMSERIAL) == BASEITEMSERIAL )
		repliedTo -= BASEITEMSERIAL;

	if( repliedTo != 0 && repliedTo < BASELOCALPOST )
	{
		tSock->sysmessage( 729 );
		return;
	}

	std::string fileName = GetMsgBoardFile( tSock->GetDWord( 4 ), msgType );
	if( fileName.empty() )
	{
		tSock->sysmessage( 725 );
		return;
	}

	std::vector< UI08 > internalBuffer;

	const UI16 length = tSock->GetWord( 1 );
	internalBuffer.resize( length );
	memcpy( &internalBuffer[0], tSock->Buffer(), length );

	msgBoardPost_st msgBoardPost;

	msgBoardPost.Toggle = 0x05;

	msgBoardPost.ParentSerial = repliedTo;

	UI08 i		= 0;
	UI16 offset = 11;

	msgBoardPost.PosterLen = tChar->GetName().size()+1;
	strncpy( msgBoardPost.Poster, tChar->GetName().c_str(), msgBoardPost.PosterLen );

	msgBoardPost.SubjectLen = internalBuffer[++offset];
	strncpy( msgBoardPost.Subject, (const char*)&internalBuffer[++offset], msgBoardPost.SubjectLen );

	offset += msgBoardPost.SubjectLen;

	msgBoardPost.msgBoardLine.resize( internalBuffer[offset] );
	msgBoardPost.Lines = internalBuffer[offset];
	std::vector< std::string >::iterator lIter;
	for( lIter = msgBoardPost.msgBoardLine.begin(); lIter != msgBoardPost.msgBoardLine.end(); ++lIter )
	{
		(*lIter).resize( internalBuffer[++offset] );
		for( i = 0; i < (*lIter).size(); ++i )
			(*lIter)[i] = internalBuffer[++offset];
	}

	const SERIAL msgID = MsgBoardWritePost( msgBoardPost, fileName, msgType );
	if( msgID != INVALIDSERIAL )
	{
		CPAddItemToCont toAdd;
		toAdd.Serial( (msgID | BASEITEMSERIAL) );
		toAdd.Container( tSock->GetDWord( 4 ) );
		tSock->Send( &toAdd );

		// Setup buffer to expect to receive an ACK from the client for this posting
		tSock->PostAcked( msgID );
	}
}

//o--------------------------------------------------------------------------o
//|	Function		-	bool MsgBoardReadPost( std::ifstream& file, msgBoardPost_st& msgBoardPost, SERIAL msgSerial = INVALIDSERIAL )
//|	Date			-	8/10/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Reads in a post from its specified file.
//o--------------------------------------------------------------------------o
bool MsgBoardReadPost( std::ifstream& file, msgBoardPost_st& msgBoardPost, SERIAL msgSerial = INVALIDSERIAL )
{
	char buffer[4];
	char tmpLine[256];

	file.read( buffer, 2 );
	msgBoardPost.Size = ( (buffer[0]<<8) + buffer[1] );

	file.seekg( msgBoardPost.Size-6, std::ios::cur );

	file.read( buffer, 4 );
	msgBoardPost.Serial = calcserial( buffer[0], buffer[1], buffer[2], buffer[3] );
	if( !file.fail() && ( msgBoardPost.Serial == msgSerial || msgSerial == INVALIDSERIAL ) )
	{
		file.seekg( -(msgBoardPost.Size-2), std::ios::cur );

		file.read( buffer, 1 );
		msgBoardPost.Toggle = buffer[0];

		file.read( buffer, 4 );
		msgBoardPost.ParentSerial = calcserial( buffer[0], buffer[1], buffer[2], buffer[3] );

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

		file.seekg( 4, std::ios::cur );

		if( msgBoardPost.Toggle != 0x00 )
			return true;
	}

	return false;
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
	std::string fileName = GetMsgBoardFile( mSock->GetDWord( 4 ), (mSock->GetByte( 8 ) - 0x40) );
	if( fileName.empty() )
	{
		mSock->sysmessage( 732 );
		return;
	}

	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
		fileName = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ) + fileName;

	msgBoardPost_st msgBoardPost;
	bool foundEntry		= false;

	const SERIAL msgSerial = (mSock->GetDWord( 8 ) - BASEITEMSERIAL );
	std::ifstream file ( fileName.c_str(), std::ios::in | std::ios::binary );
	if( file.is_open() )
	{
		file.seekg( 0, std::ios::beg );
		while( file && !foundEntry )
		{
			if(	MsgBoardReadPost( file, msgBoardPost, msgSerial ) )
				foundEntry = true;
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
	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ) || !mChar->IsGM() )
	{
		mSock->sysmessage( "Only GM's may delete posts!" );
		return;
	}

	CItem *msgBoard = calcItemObjFromSer( mSock->GetDWord( 4 ) );
	if( !ValidateObject( msgBoard ) )
		return;

	std::string fileName = GetMsgBoardFile( msgBoard->GetSerial(), (mSock->GetByte( 8 ) - 0x40 ) );
	if( fileName.empty() )
		return;

	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
		fileName = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ) + fileName;

	SERIAL tmpSerial	= 0;
	size_t totalSize	= 0;
	UI16 tmpSize		= 0;
	char rBuffer[4];
	bool foundPost		= false;
	bool removeReply	= true;

	const SERIAL msgSer = (mSock->GetDWord( 8 ) - BASEITEMSERIAL );

	std::fstream file ( fileName.c_str(), std::ios::in | std::ios::out | std::ios::binary );
	if( file.is_open() )
	{
		while( !file.eof() )
		{
			removeReply = false;

			file.read( rBuffer, 2 );
			tmpSize = ( (rBuffer[0]<<8) + rBuffer[1] );

			file.seekg( 1, std::ios::cur );

			file.read( rBuffer, 4 );
			tmpSerial = calcserial( rBuffer[0], rBuffer[1], rBuffer[2], rBuffer[3] );
			if( tmpSerial == msgSer )	// We need to remove any replies to a thread
			{
				removeReply = true;
				file.seekp( totalSize+2, std::ios::beg );
				file.put( 0x00 );
			}

			if( removeReply || !foundPost )
			{
				file.seekg( tmpSize-11, std::ios::cur );

				file.read( rBuffer, 4 );
				tmpSerial = calcserial( rBuffer[0], rBuffer[1], rBuffer[2], rBuffer[3] );
				if( removeReply )
				{
					CPRemoveItem toRemove;
					toRemove.Serial( (tmpSerial + BASEITEMSERIAL) );
					mSock->Send( &toRemove );
				}
				else if( tmpSerial == msgSer )	// Mark it found, but don't break out of the loop, in order to get all replies
				{
					foundPost = true;

					file.seekp( totalSize+2, std::ios::beg );
					file.put( 0x00 );

					mSock->sysmessage( 734 );
				}
			}

			totalSize += tmpSize;
			file.seekg( totalSize, std::ios::beg );
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
bool MsgBoardPostQuest( CChar *mNPC, const QuestTypes questType )
{
	msgBoardPost_st msgBoardPost;
	UString sect, tag, data;
	std::string fileName		= "region" + UString::number( mNPC->GetQuestOrigRegion() ) + ".bbf";
	ScriptSection *EscortData	= NULL, *Escort = NULL;
	size_t totalEntries			= 0;
	std::string tmpSubject		= "";

	switch( questType )
	{
	case QT_ESCORTQUEST:
		msgBoardPost.Toggle		= QT_ESCORTQUEST;
		tmpSubject				= Dictionary->GetEntry( 735 );
		Escort					= FileLookup->FindEntry( "ESCORTS", msgboard_def );
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
				fullLine.replace( position, 2, cwmWorldState->townRegions[mNPC->GetQuestDestRegion()]->GetName() );
				position = fullLine.find( "%r" );
			}

			position = fullLine.find( "%R" );
			while( position != std::string::npos )
			{
				fullLine.replace( position, 2, mNPC->GetRegion()->GetName() );
				position = fullLine.find( "%R" );
			}

			fullLine.resize( fullLine.size() + 1 );
			msgBoardPost.msgBoardLine.push_back( fullLine );
		}
		break;
	case QT_BOUNTYQUEST:
		tmpSubject				= Dictionary->GetEntry( 736 );
		msgBoardPost.Toggle		= QT_BOUNTYQUEST;
		break;
	case QT_ITEMQUEST:
		tmpSubject				= Dictionary->GetEntry( 737 );
		msgBoardPost.Toggle		= QT_ITEMQUEST;
		break;
	default:
		Console.Error( 1, "MsgBoardPostQuest() Invalid questType %d", questType );
		return false;
	}

	msgBoardPost.Lines = static_cast<UI08>(msgBoardPost.msgBoardLine.size());

	msgBoardPost.SubjectLen = static_cast<UI08>(tmpSubject.size() + 1);
	strncpy( msgBoardPost.Subject, tmpSubject.c_str(), tmpSubject.size() );

	msgBoardPost.PosterLen = static_cast<UI08>(mNPC->GetName().size() + 1);
	strncpy( msgBoardPost.Poster, mNPC->GetName().c_str(), mNPC->GetName().size() );

	msgBoardPost.ParentSerial = mNPC->GetSerial();

	if( MsgBoardWritePost( msgBoardPost, fileName, PT_REGIONAL ) != INVALIDSERIAL )
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
	const UI08 npcRegion		= mNPC->GetRegionNum();
	UI08 destRegion				= npcRegion;
	const size_t escortSize		= cwmWorldState->escortRegions.size();

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

	CTownRegion *destReg = cwmWorldState->townRegions[mNPC->GetQuestDestRegion()];

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

//o--------------------------------------------------------------------------o
//|	Function		-	MsgBoardRemoveFile( const SERIAL msgBoardSer )
//|	Date			-	8/10/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Removes the MessageBoard .bbf file attached to the specified serial
//o--------------------------------------------------------------------------o
void MsgBoardRemoveFile( const SERIAL msgBoardSer )
{
	std::string fileName;

	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
		fileName = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD );

	fileName += UString::number( msgBoardSer, 16 ) + ".bbf";

	remove( fileName.c_str() );

	Console.Print( "Deleted MessageBoard file for Board Serial 0x%X", msgBoardSer );
}

//o--------------------------------------------------------------------------o
//|	Function		-	MsgBoardMaintenance()
//|	Date			-	8/10/2005
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//o--------------------------------------------------------------------------o
//|	Description		-	Finds all .bbf files and cleans any deleted posts from them
//|							It will also remove any empty .bbf files if necesarry.
//o--------------------------------------------------------------------------o
void MsgBoardMaintenance( void )
{
	std::vector< msgBoardPost_st > mbMessages;
	std::vector< msgBoardPost_st >::const_iterator mbIter;

	std::string dirName = "./msgboards/";
	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
		dirName = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD );

	cDirectoryListing fileList( dirName, "*.bbf" );

	STRINGLIST *mbFiles = fileList.List();
	STRINGLIST_ITERATOR fIter;
	for( fIter = mbFiles->begin(); fIter != mbFiles->end(); ++fIter )
	{
		std::ifstream file ( (*fIter).c_str(), std::ios::in | std::ios::binary );
		if( file.is_open() )
		{
			file.seekg( 0, std::ios::beg );
			while( file )
			{
				msgBoardPost_st toAdd;

				if( MsgBoardReadPost( file, toAdd ) )
					mbMessages.push_back( toAdd );
			}
			file.close();

			if( mbMessages.empty() )
				remove( (*fIter).c_str() );
			else
			{
				std::ofstream mFile ( (*fIter).c_str(), std::ios::out | std::ios::trunc | std::ios::binary );

				mFile.seekp( 0, std::ios::beg );
				for( mbIter = mbMessages.begin(); mbIter != mbMessages.end(); ++mbIter )
				{
					MsgBoardWritePost( mFile, (*mbIter) );
				}
				mFile.close();
			}

			mbMessages.clear();
		}
		else
			Console.Error( 2, "Failed to open MessageBoard file for reading: %s", (*fIter).c_str() );
	}
}

}
