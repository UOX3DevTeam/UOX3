//o-----------------------------------------------------------------------------------------------o
//|	File		-	msgboard.cpp
//|	Date		-	16/04/1999
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Message Board Handling
//o-----------------------------------------------------------------------------------------------o
//| Changes		-	Version History
//|
//|						1.0			16th April 1999 - 0.69.03
//|						Initial implimentation
//|
//|						1.1			19th January 2000 - 0.70.02
//|						Additional features added to messageboard
//|						Escort quests added / integrated with message board enhancements
//|
//|						2.0			10th August 2005 - 0.98-3.0g
//|						Complete rewrite of msgboard.cpp
//|						Modified file i/o to use fstreams for faster file reads/writes.
//|						Completely re-formatted the message board files to remove the need for two files, and reduce waste.
//|						Made use of packet classes to simplify sending & receiving data from the client.
//|						Added a maintenance function that will automatically clean up the message board files.
//|						Added support for message board file deletion upon deleting the associated world object.
//|
//o-----------------------------------------------------------------------------------------------o
#include "uox3.h"
#include "msgboard.h"
#include "townregion.h"
#include "cServerDefinitions.h"
#include "ssection.h"
#include "cEffects.h"
#include "CPacketSend.h"
#include "classes.h"
#include "Dictionary.h"
#include "StringUtility.hpp"
#include <filesystem>

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	std::string GetMsgBoardFile( const SERIAL msgBoardSer, const UI08 msgType )
//|	Date		-	8/6/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates the proper MessageBoard filename based on the messageType and board Serial
//o-----------------------------------------------------------------------------------------------o
std::string GetMsgBoardFile( const SERIAL msgBoardSer, const UI08 msgType )
{
	std::string fileName;
	switch( msgType )
	{
		case PT_GLOBAL:			fileName = "global.bbf";												break;
		case PT_REGIONAL:		CItem *msgBoard;
			CTownRegion *mbRegion;
			msgBoard = calcItemObjFromSer( msgBoardSer );
			mbRegion = calcRegionFromXY( msgBoard->GetX(), msgBoard->GetY(), msgBoard->WorldNumber(), msgBoard->GetInstanceID() );
			fileName = std::string("region") + strutil::number( mbRegion->GetRegionNum() ) + std::string(".bbf");
			break;
		case PT_LOCAL:			fileName = strutil::number( msgBoardSer, 16 ) + std::string(".bbf");					break;
		default:				Console.error( "GetMsgBoardFile() Invalid post type, aborting" );
			break;
	}
	return fileName;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MsgBoardOpen( CSocket *mSock )
//|	Date		-	7/16/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens the Messageboard and Advises the client of the messages
//o-----------------------------------------------------------------------------------------------o
void MsgBoardOpen( CSocket *mSock )
{
	const SERIAL boardSer = mSock->GetDWord( 1 );

	CItem *msgBoard = calcItemObjFromSer( boardSer );
	if( !ValidateObject( msgBoard ) )
	{
		return;
	}

	char buffer[4];
	std::string fileName, dirPath;

	mSock->PostClear();

	CPOpenMessageBoard mbPost( mSock );
	mSock->Send( &mbPost );

	CPSendMsgBoardPosts mbSend;

	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
	{
		dirPath = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD );
	}

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
						mbSend.CopyData( mSock, tmpSerial, tmpToggle, boardSer );
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MsgBoardList( CSocket *mSock )
//|	Date		-	7/16/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Sends the summary of each message to the client
//o-----------------------------------------------------------------------------------------------o
void MsgBoardList( CSocket *mSock )
{
	char buffer[4];
	std::string fileName, dirPath;

	CItem *msgBoard = calcItemObjFromSer( mSock->GetDWord( 4 ) );
	if( !ValidateObject( msgBoard ) )
	{
		return;
	}

	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
	{
		dirPath = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD );
	}

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
						{
							file.seekg( 4, std::ios::cur );
						}

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
							Console.warning( strutil::format("Malformed MessageBoard post, MessageID: 0x%X", msgBoardPost.Serial ));
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
					{
						postAck = mSock->NextPostAck();
					}
				}
				totalSize += msgBoardPost.Size;
				file.seekg( totalSize, std::ios::beg );
				if( totalSize > 3000 || msgBoardPost.Serial == 0 ) //HACCKKKK!!!1one. Without this in place, it will loop for ever :(
				{
					file.close();
				}
			}
			file.close();
		}
	}

	if( !mSock->FinishedPostAck() )
	{
		mSock->PostClear();
		Console.error( strutil::format("Failed to list all posts for MessageBoard ID: 0x%X", mSock->GetDWord( 4 )) );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool GetMaxSerial( const std::string& fileName, UI08 *nextMsgID, const PostTypes msgType )
//|	Date		-	7/22/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Updates nextMsgID with the next available message serial.
//o-----------------------------------------------------------------------------------------------o
bool GetMaxSerial( const std::string& fileName, UI08 *nextMsgID, const PostTypes msgType )
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
	{
		++msgIDSer;
	}

	nextMsgID[0] = static_cast<UI08>(msgIDSer>>24);
	nextMsgID[1] = static_cast<UI08>(msgIDSer>>16);
	nextMsgID[2] = static_cast<UI08>(msgIDSer>>8);
	nextMsgID[3] = static_cast<UI08>(msgIDSer%256);

	if( nextMsgID[1] == 0xFF && nextMsgID[2] == 0xFF && nextMsgID[3] == 0xFF )
	{
		Console.warning(strutil::format("Maximum Posts reached for board %s, no further posts can be created", fileName.c_str()) );
		return false;
	}

	return true;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MsgBoardWritePost( std::ofstream& mFile, const msgBoardPost_st& msgBoardPost )
//|	Date		-	8/10/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes a new post to the .bbf file
//o-----------------------------------------------------------------------------------------------o
void MsgBoardWritePost( std::ofstream& mFile, const msgBoardPost_st& msgBoardPost )
{
	char wBuffer[4];

	// Add post size to 2 bytes of buffer, and write buffer to file
	wBuffer[0] = static_cast<SI08>(msgBoardPost.Size>>8);
	wBuffer[1] = static_cast<SI08>(msgBoardPost.Size%256);
	mFile.write( (const char *)&wBuffer, 2 );

	// Write the value of Toggle as the next byte
	mFile.write( (const char *)&msgBoardPost.Toggle, 1 );

	// Add parent serial to 4 bytes of buffer, and write buffer to file
	wBuffer[0] = static_cast<SI08>(msgBoardPost.ParentSerial>>24);
	wBuffer[1] = static_cast<SI08>(msgBoardPost.ParentSerial>>16);
	wBuffer[2] = static_cast<SI08>(msgBoardPost.ParentSerial>>8);
	wBuffer[3] = static_cast<SI08>(msgBoardPost.ParentSerial%256);
	mFile.write( (const char *)&wBuffer, 4 );

	// Write length of poster's name as next byte, and
	// then write poster's name as next X bytes
	mFile.write( (const char *)&msgBoardPost.PosterLen, 1 );
	mFile.write( (const char *)&msgBoardPost.Poster, msgBoardPost.PosterLen );

	// Write length of post's subject in next byte, and
	// then write post subject as next X bytes
	mFile.write( (const char *)&msgBoardPost.SubjectLen, 1 );
	mFile.write( (const char *)&msgBoardPost.Subject, msgBoardPost.SubjectLen );

	// Write length of post's date string in next byte, and
	// then write date string as next X bytes
	mFile.write( (const char *)&msgBoardPost.DateLen, 1 );
	mFile.write( (const char *)&msgBoardPost.Date, msgBoardPost.DateLen );

	// Write number of lines in post as next byte, and
	// then loop through each line
	mFile.write( (const char *)&msgBoardPost.Lines, 1 );
	STRINGLIST_CITERATOR lIter;
	for( lIter = msgBoardPost.msgBoardLine.begin(); lIter != msgBoardPost.msgBoardLine.end(); ++lIter )
	{
		// Write length of line as next byte, and
		// then write the line as next X bytes
		const UI08 lineSize = static_cast<UI08>((*lIter).size());
		mFile.write( (const char *)&lineSize, 1 );
		mFile.write( (*lIter).c_str(), lineSize );
	}

	// Finally, add actual post serial to 4 bytes of buffer, and write buffer to file
	wBuffer[0] = static_cast<SI08>(msgBoardPost.Serial>>24);
	wBuffer[1] = static_cast<SI08>(msgBoardPost.Serial>>16);
	wBuffer[2] = static_cast<SI08>(msgBoardPost.Serial>>8);
	wBuffer[3] = static_cast<SI08>(msgBoardPost.Serial%256);
	mFile.write( (const char *)&wBuffer, 4 );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	SERIAL MsgBoardWritePost( msgBoardPost_st& msgBoardPost, const std::string& fileName, const PostTypes msgType )
//|	Date		-	7/22/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Writes a new post to the .bbf file, returning the messages SERIAL
//o-----------------------------------------------------------------------------------------------o
SERIAL MsgBoardWritePost( msgBoardPost_st& msgBoardPost, const std::string& fileName, const PostTypes msgType )
{
	SERIAL msgID = INVALIDSERIAL;

	std::string fullFile;
	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
	{
		fullFile = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ) + fileName;
	}

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
	{
		return msgID;
	}
	else
	{
		msgID = calcserial( nextMsgID[0], nextMsgID[1], nextMsgID[2], nextMsgID[3] );
	}

	auto timet = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	auto timeOfPost = *std::localtime(&timet);
	auto time			= strutil::format( "Day %i @ %i:%02i\0", (timeOfPost.tm_yday+1), timeOfPost.tm_hour, timeOfPost.tm_min );
	time.resize( time.size()+1 );
	const UI08 timeSize		= static_cast<UI08>(time.size());
	const UI08 posterSize	= static_cast<UI08>(msgBoardPost.PosterLen);
	const UI08 subjSize		= static_cast<UI08>(msgBoardPost.SubjectLen);
	UI16 totalSize			= static_cast<UI16>(15 + posterSize + subjSize + timeSize);
	STRINGLIST_CITERATOR lIter;
	for( lIter = msgBoardPost.msgBoardLine.begin(); lIter != msgBoardPost.msgBoardLine.end(); ++lIter )
	{
		totalSize += 1 + static_cast<UI16>( ( *lIter ).size() );
	}

	msgBoardPost.Size			= totalSize;
	msgBoardPost.DateLen		= timeSize;
	strncpy( msgBoardPost.Date, time.c_str(), timeSize );
	msgBoardPost.Serial			= msgID;

	std::ofstream mFile ( fullFile.c_str(), std::ios::out | std::ios::app | std::ios::binary );

	MsgBoardWritePost( mFile, msgBoardPost );

	mFile.close();

	return msgID;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MsgBoardPost( CSocket *tSock )
//|	Date		-	7/22/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles the client requesting to post a new message
//o-----------------------------------------------------------------------------------------------o
void MsgBoardPost( CSocket *tSock )
{
	CChar *tChar = tSock->CurrcharObj();
	if( !ValidateObject( tChar ) )
	{
		return;
	}

	const PostTypes msgType = static_cast<PostTypes>(tChar->GetPostType());

	if( msgType != PT_LOCAL && tChar->GetCommandLevel() < CL_GM )
	{
		tSock->sysmessage( "Only GM's may post Global or Regional messages" );
		return;
	}

	SERIAL repliedTo = tSock->GetDWord( 8 );
	if( (repliedTo&BASEITEMSERIAL) == BASEITEMSERIAL )
	{
		repliedTo -= BASEITEMSERIAL;
	}

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

	msgBoardPost.PosterLen = static_cast<UI08>(tChar->GetName().size()) + 1;
	strncpy( msgBoardPost.Poster, tChar->GetName().c_str(), msgBoardPost.PosterLen );

	msgBoardPost.SubjectLen = internalBuffer[++offset];
	strncpy( msgBoardPost.Subject, (const char*)&internalBuffer[++offset], msgBoardPost.SubjectLen );

	offset += msgBoardPost.SubjectLen;

	msgBoardPost.msgBoardLine.resize( internalBuffer[offset] );
	msgBoardPost.Lines = internalBuffer[offset];
	std::vector< std::string >::iterator lIter;
	UI08 j = 0;
	for( lIter = msgBoardPost.msgBoardLine.begin(); lIter != msgBoardPost.msgBoardLine.end(); ++lIter )
	{
		(*lIter).resize( internalBuffer[++offset] );
		for( i = 0; i < (*lIter).size(); ++i )
		{
			(*lIter)[i] = internalBuffer[++offset];
		}

		// Check if line is null-terminated, and null-terminate it if not
		if( msgBoardPost.msgBoardLine[j][msgBoardPost.msgBoardLine[j].size()] > 0 )
		{
			auto checkForNullTerminator = msgBoardPost.msgBoardLine[j][msgBoardPost.msgBoardLine[j].size()-1];
			if( checkForNullTerminator != '\0' )
			{
				msgBoardPost.msgBoardLine[j][msgBoardPost.msgBoardLine[j].size()-1] = '\0';
			}
		}
		j++;
	}

	const SERIAL msgID = MsgBoardWritePost( msgBoardPost, fileName, msgType );
	if( msgID != INVALIDSERIAL )
	{
		CPAddItemToCont toAdd;
		if( tSock->ClientVerShort() >= CVS_6017 )
		{
			toAdd.UOKRFlag( true );
		}
		toAdd.Serial( (msgID | BASEITEMSERIAL) );
		toAdd.Container( tSock->GetDWord( 4 ) );
		tSock->Send( &toAdd );

		// Setup buffer to expect to receive an ACK from the client for this posting
		tSock->PostAcked( msgID );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool MsgBoardReadPost( std::ifstream& file, msgBoardPost_st& msgBoardPost, SERIAL msgSerial = INVALIDSERIAL )
//|	Date		-	8/10/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Reads in a post from its specified file.
//o-----------------------------------------------------------------------------------------------o
bool MsgBoardReadPost( std::ifstream& file, msgBoardPost_st& msgBoardPost, SERIAL msgSerial = INVALIDSERIAL )
{
	char buffer[4];

	// Read two first bytes of post, which contains total size
	file.read( buffer, 2 );

	if( file.gcount() != 2 )
	{
		// Return early if we failed to read 2 bytes into buffer
		return false;
	}

	msgBoardPost.Size = ( (buffer[0]<<8) + buffer[1] );

	// Seek to position of msgboardpost serial stored at end of the post, and read it into buffer
	file.seekg( msgBoardPost.Size-6, std::ios::cur );
	file.read( buffer, 4 );
	msgBoardPost.Serial = calcserial( buffer[0], buffer[1], buffer[2], buffer[3] );
	if( !file.fail() && ( msgBoardPost.Serial == msgSerial || msgSerial == INVALIDSERIAL ) )
	{
		char tmpLine[256];
		// Seek to start of post, right after 2 bytes containing size
		file.seekg( -(msgBoardPost.Size-2), std::ios::cur );

		// Get toggle value from next byte
		file.read( buffer, 1 );
		msgBoardPost.Toggle = buffer[0];

		// Get parent serial from next 4 bytes
		file.read( buffer, 4 );
		msgBoardPost.ParentSerial = calcserial( buffer[0], buffer[1], buffer[2], buffer[3] );

		// Get size of poster's name from next byte, and then get poster's name
		file.read( buffer, 1 );
		file.read( msgBoardPost.Poster, buffer[0] );
		msgBoardPost.PosterLen = buffer[0];

		// Get size of post's subject from next byte, and then get subject string
		file.read( buffer, 1 );
		file.read( msgBoardPost.Subject, buffer[0] );
		msgBoardPost.SubjectLen = buffer[0];

		// Get size of post's date string from next byte, and then get date string
		file.read( buffer, 1 );
		file.read( msgBoardPost.Date, buffer[0] );
		msgBoardPost.DateLen = buffer[0];

		// Get number of lines from next byte, then read each line from post
		file.read( buffer, 1 );
		msgBoardPost.Lines = buffer[0];
		for( UI08 i = 0; i < msgBoardPost.Lines; ++i )
		{
			// Get line length from next byte, then read line string into tmpLine
			file.read( buffer, 1 );
			file.read( tmpLine, buffer[0]);

			// Stuff contents of tmpLine into msgBoardLine STRINGLIST in msgBoardPost struct, then add null terminator
			msgBoardPost.msgBoardLine.push_back( tmpLine );
			msgBoardPost.msgBoardLine[msgBoardPost.msgBoardLine.size()-1] += '\0';
		}

		file.seekg( 4, std::ios::cur );

		if( msgBoardPost.Toggle != 0x00 )
		{
			return true;
		}
	}

	// If Toggle value has been set to 0x00 for a post, return false so it won't be included when writing posts back to file later
	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MsgBoardOpenPost( CSocket *mSock )
//|	Date		-	7/16/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Opens a post on a message board when double clicked.
//o-----------------------------------------------------------------------------------------------o
void MsgBoardOpenPost( CSocket *mSock )
{
	std::string fileName = GetMsgBoardFile( mSock->GetDWord( 4 ), (mSock->GetByte( 8 ) - 0x40) );
	if( fileName.empty() )
	{
		mSock->sysmessage( 732 );
		return;
	}

	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
	{
		fileName = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ) + fileName;
	}

	msgBoardPost_st msgBoardPost;
	bool foundEntry		= false;

	const SERIAL msgSerial = (mSock->GetDWord( 8 ) - BASEITEMSERIAL );
	std::ifstream file ( fileName.c_str(), std::ios::in | std::ios::binary );
	if( file.is_open() )
	{
		file.seekg( 0, std::ios::beg );
		while( file && !foundEntry )
		{
			if( MsgBoardReadPost( file, msgBoardPost, msgSerial ) )
			{
				foundEntry = true;
			}
		}
		file.close();
	}
	else
	{
		Console.error( strutil::format("Failed to open MessageBoard file for reading MessageID: 0x%X", msgSerial) );
	}

	if( foundEntry )
	{
		CPOpenMsgBoardPost mbPost( mSock, msgBoardPost, true );
		mSock->Send( &mbPost );
	}
	else
	{
		Console.warning(strutil::format( "Failed to find MessageBoard file for MessageID: 0x%X", msgSerial ));
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MsgBoardRemovePost( CSocket *mSock )
//|	Date		-	7/17/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes a post from a message board
//|
//|						Note: The client should trigger this function, but I have
//|						been unable to find a way through the client to send the
//|						remove post packet.
//o-----------------------------------------------------------------------------------------------o
void MsgBoardRemovePost( CSocket *mSock )
{
	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ) || ( ValidateObject( mChar ) && !mChar->IsGM() ))
	{
		mSock->sysmessage( "Only GMs may delete posts!" );
		return;
	}

	// If the messageboard is not found, do nothing
	CItem *msgBoard = calcItemObjFromSer( mSock->GetDWord( 4 ) );
	if( !ValidateObject( msgBoard ) )
	{
		return;
	}

	// If the messageboard filename is not returned properly, do nothing
	std::string fileName = GetMsgBoardFile( msgBoard->GetSerial(), (mSock->GetByte( 8 ) - 0x40 ) );
	if( fileName.empty() )
	{
		return;
	}

	// If the folder storing messageboard files exists, construct a path for the filename
	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
	{
		fileName = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ) + fileName;
	}

	bool foundPost		= false;

	// Fetch the serial of the messageboard post in need of removal
	const SERIAL msgSer = (mSock->GetDWord( 8 ) - BASEITEMSERIAL );

	// Let's grab the size of the file so we can use it later
	std::size_t fileSize = 0;
	try {
		fileSize = std::filesystem::file_size( fileName );
	} catch( ... ) {
		fileSize = 0;
	} 

	// If file size is zero, not much point in continuing!
	if( fileSize == 0 )
	{
		if( ValidateObject( mChar ))
		{
			mSock->sysmessage( "Failed to remove post; file size not found! Check server log for more details." );
		}
		Console.error( strutil::format("Could not fetch file size for file %s", fileName.c_str()) );
		return;
	}

	// Open a filestream in binary mode to read in data from the messageboard file
	std::fstream file ( fileName.c_str(), std::ios::in | std::ios::out | std::ios::binary );
	if( file.is_open() )
	{
		SERIAL tmpSerial	= 0;
		size_t totalSize	= 0;
		UI16 tmpSize		= 0;
		char rBuffer[4];
		bool removeReply	= true;

		while( !file.eof() )
		{
			removeReply = false;

			// Read first 2 characters from filestream into rBuffer, starting from first line in file
			file.read( rBuffer, 2 );

			// Shift first byte up eight bytes, add to second byte
			tmpSize = ( (rBuffer[0]<<8) + rBuffer[1] );

			// Move stream's "get" position relative to current position
			file.seekg( 1, std::ios::cur );

			// Read the next 4 characters from filestream into rBuffer, and use this to attempt calculating post serial
			file.read( rBuffer, 4 );
			tmpSerial = calcserial( rBuffer[0], rBuffer[1], rBuffer[2], rBuffer[3] );

			// If the serial matches the serial of post to be removed, prepare to check for
			// any replies to the post/thread as well, which will also need to be removed
			if( tmpSerial == msgSer )
			{
				removeReply = true;

				// Move stream's "set" position to the start of the post in question, and overwrite first 2 bytes with 0x00 to mark it for deletion
				// This will hide it from view, but not actually delete it from the file itself; that's handled by message board maintenance
				file.seekp( totalSize+2, std::ios::beg );
				file.put( 0x00 );
			}

			// If looking for replies to remove, or we haven't found the post to remove yet
			if( removeReply || !foundPost )
			{
				// Move stream's "get" position relative to current position, to... what? Look for a reply?
				file.seekg( tmpSize-11, std::ios::cur );
				
				// Read another serial candidate into the buffer, and try to calculate post serial from buffer
				file.read( rBuffer, 4 );
				tmpSerial = calcserial( rBuffer[0], rBuffer[1], rBuffer[2], rBuffer[3] );

				if( removeReply )
				{
					// Send packet to remove item (each post is an item in a container - the container being the message board) visually from message board
					CPRemoveItem toRemove;
					toRemove.Serial( (tmpSerial + BASEITEMSERIAL) );
					mSock->Send( &toRemove );
				}
				else if( tmpSerial == msgSer )
				{
					// Mark post as found, but don't break out of the loop, in order to get all replies
					foundPost = true;

					// Move stream's "set" position to the start of the post in question, and overwrite first 2 bytes with 0x00 to mark it for deletion
					// This will hide it from view, but not actually delete it from the file itself; that's handled by message board maintenance
					file.seekp( totalSize+2, std::ios::beg );
					file.put( 0x00 );

					mSock->sysmessage( 734 );
				}
			}

			// Make sure we don't get stuck in an infinite loop... don't allow seeking past the end of the file!
			totalSize += tmpSize;
			if( totalSize < fileSize )
			{
				file.seekg( totalSize, std::ios::beg );
			}
			else
			{
				break;
			}
		}
		file.close();
	}
	else
	{
		Console.error( strutil::format("Could not open file %s for reading", fileName.c_str()) );
	}

	// If we found the post to remove previously, remove it at this point, since all potential replies to it have been removed already
	if( foundPost )
	{
		CPRemoveItem toRemove;
		toRemove.Serial( mSock->GetDWord( 8 ) );
		mSock->Send( &toRemove );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool CPIMsgBoardEvent::Handle( void )
//|	Date		-	7/22/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles a mesage board event from the client.
//o-----------------------------------------------------------------------------------------------o
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

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	bool MsgBoardPostQuest( CChar *mNPC, const QuestTypes questType )
//|	Date		-	7/23/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates an escort quest post on regional messageboards
//o-----------------------------------------------------------------------------------------------o
bool MsgBoardPostQuest( CChar *mNPC, const QuestTypes questType )
{
	msgBoardPost_st msgBoardPost;
	std::string sect, tag, data;
	std::string fileName		= std::string("region") + strutil::number( mNPC->GetQuestOrigRegion() ) + std::string(".bbf");
	ScriptSection *EscortData	= nullptr, *Escort = nullptr;
	size_t totalEntries			= 0;
	std::string tmpSubject		= "";

	switch( questType )
	{
		case QT_ESCORTQUEST:
			msgBoardPost.Toggle		= QT_ESCORTQUEST;
			tmpSubject				= Dictionary->GetEntry( 735 );
			Escort					= FileLookup->FindEntry( "ESCORTS", msgboard_def );
			if( Escort == nullptr )
			{
				return false;
			}

			totalEntries = Escort->NumEntries();
			if( totalEntries == 0 )
			{
				Console.error( "MsgBoardPostQuest() No msgboard dfn entries found" );
				return false;
			}

			Escort->MoveTo( RandomNum( static_cast<size_t>(0), totalEntries-1 ) );
			sect		= "ESCORT " + Escort->GrabData();
			EscortData	= FileLookup->FindEntry( sect, msgboard_def );
			if( EscortData == nullptr )
			{
				Console.error( strutil::format("MsgBoardPostQuest() Couldn't find entry %s", sect.c_str()) );
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
					fullLine.replace( position, 2, strutil::format( "%d %d", mNPC->GetX(), mNPC->GetY() ) );
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
			Console.error( strutil::format("MsgBoardPostQuest() Invalid questType %d", questType) );
			return false;
	}

	msgBoardPost.Lines = static_cast<UI08>(msgBoardPost.msgBoardLine.size());

	msgBoardPost.SubjectLen = static_cast<UI08>(tmpSubject.size() + 1);
	strncpy( msgBoardPost.Subject, tmpSubject.c_str(), tmpSubject.size() );

	msgBoardPost.PosterLen = static_cast<UI08>(mNPC->GetName().size() + 1);
	strncpy( msgBoardPost.Poster, mNPC->GetName().c_str(), mNPC->GetName().size() );

	msgBoardPost.ParentSerial = mNPC->GetSerial();

	if( MsgBoardWritePost( msgBoardPost, fileName, PT_REGIONAL ) != INVALIDSERIAL )
	{
		return true;
	}

	return false;
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MsgBoardQuestEscortCreate( CChar *mNPC )
//|	Date		-	7/23/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Creates an escort quest assigning it a valid escort region
//o-----------------------------------------------------------------------------------------------o
void MsgBoardQuestEscortCreate( CChar *mNPC )
{
	const UI16 npcRegion		= mNPC->GetRegionNum();
	UI16 destRegion				= npcRegion;
	const size_t escortSize		= cwmWorldState->escortRegions.size();

	if( !cwmWorldState->escortRegions.empty() && ( escortSize > 1 || cwmWorldState->escortRegions[0] != npcRegion )  )
	{
		for( UI16 i = 0; i < 50 && npcRegion == destRegion; ++i )
			destRegion = cwmWorldState->escortRegions[RandomNum( static_cast<size_t>(0), (escortSize-1) )];
	}

	if( destRegion == 0 || destRegion == npcRegion )
	{
		Console.error( strutil::format("MsgBoardQuestEscortCreate() No valid regions defined for escort quests") );
		mNPC->Delete();
		return;
	}

	mNPC->SetQuestDestRegion( destRegion );

	// Set quest type to escort
	mNPC->SetQuestType( QT_ESCORTQUEST );

	// Make sure they don't move until an player accepts the quest
	mNPC->SetNpcWander( WT_NONE );
	mNPC->SetNPCAiType( AI_NONE );
	mNPC->SetQuestOrigRegion( npcRegion );

	if( cwmWorldState->ServerData()->SystemTimer( tSERVER_ESCORTWAIT ) )
	{
		mNPC->SetTimer( tNPC_SUMMONTIME, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_ESCORTWAIT ) );
	}

	if( !MsgBoardPostQuest( mNPC, QT_ESCORTQUEST ) )
	{
		Console.error(strutil::format( "MsgBoardQuestEscortCreate() Failed to add quest post for %s", mNPC->GetName().c_str() ));
		mNPC->Delete();
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MsgBoardQuestEscortArrive( CSocket *mSock, CChar *mNPC )
//|	Date		-	7/23/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Handles payment and release upon arrival of the escort
//o-----------------------------------------------------------------------------------------------o
void MsgBoardQuestEscortArrive( CSocket *mSock, CChar *mNPC )
{
	CChar *mChar = mSock->CurrcharObj();
	if( !ValidateObject( mChar ) )
	{
		return;
	}

	CTownRegion *destReg = cwmWorldState->townRegions[mNPC->GetQuestDestRegion()];

	// Calculate payment for services rendered, partly based on escort's fame and partly based on a percentage (25%) of the amount of gold they're carrying
	UI16 questReward = (( mNPC->GetFame() > 0 ? (( mNPC->GetFame() / 100 ) * 50 ) : 0 ) + std::round( GetItemAmount( mNPC, 0x0EED ) * 0.25 ));

	// If they have no money, well, oops!
	if( questReward == 0 )
	{
		mNPC->TextMessage( mSock, 738, TALK, 0, mChar->GetName().c_str(), destReg->GetName().c_str() );
	}
	else // Otherwise pay the poor sod for his time
	{
		Items->CreateScriptItem( mSock, mChar, "0x0EED", questReward, OT_ITEM, true );
		Effects->goldSound( mSock, questReward );
		mNPC->TextMessage( mSock, 739, TALK, 0, mChar->GetName().c_str(), destReg->GetName().c_str() );
	}

	// Inform the PC of what he has just been given as payment
	mSock->sysmessage( 740, questReward, mNPC->GetName().c_str(), mNPC->GetTitle().c_str() );

	// Take the NPC out of quest mode
	mNPC->SetNpcWander( WT_FREE );         // Wander freely
	mNPC->SetFTarg( nullptr );			 // Reset follow target
	mNPC->SetQuestType( 0 );         // Reset quest type
	mNPC->SetQuestDestRegion( 0 );   // Reset quest destination region

	// Set a timer to automatically delete the NPC
	mNPC->SetTimer( tNPC_SUMMONTIME, cwmWorldState->ServerData()->BuildSystemTimeValue( tSERVER_ESCORTDONE ) );
	mNPC->SetOwner( nullptr );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MsgBoardQuestEscortRemovePost( CChar *mNPC )
//|	Date		-	7/23/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes an escort quest post on regional messageboards
//o-----------------------------------------------------------------------------------------------o
void MsgBoardQuestEscortRemovePost( CChar *mNPC )
{
	std::string fileName;
	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
	{
		fileName = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD );
	}

	fileName += std::string("region") + strutil::number( mNPC->GetQuestOrigRegion() ) + std::string(".bbf");
	std::size_t fileSize = 0;
	try {
		fileSize = std::filesystem::file_size( fileName );
	} catch( ... ) {
		fileSize = 0;
	} 

	if( fileSize == 0 )
	{
		return;
	}

	std::fstream file;
	file.open( fileName.c_str(), std::ios::in | std::ios::out | std::ios::binary );
	if( file.is_open() )
	{
		SERIAL tmpSerial	= 0;
		size_t totalSize	= 0;
		UI16 tmpSize		= 0;
		char rBuffer[4];

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
				if( totalSize < fileSize )
				{
					file.seekg( totalSize, std::ios::beg );
				}
				else
				{
					Console.error( strutil::format("Attempting to seek past end of file in %s", fileName.c_str()) );
					break;
				}
			}
		}
		file.close();
	}
	else
	{
		Console.error( strutil::format("Could not open file %s for reading", fileName.c_str()) );
	}
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MsgBoardRemoveFile( const SERIAL msgBoardSer )
//|	Date		-	8/10/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Removes the MessageBoard .bbf file attached to the specified serial
//o-----------------------------------------------------------------------------------------------o
void MsgBoardRemoveFile( const SERIAL msgBoardSer )
{
	std::string fileName;

	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
	{
		fileName = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD );
	}

	fileName += strutil::number( msgBoardSer, 16 ) + std::string(".bbf");

	[[maybe_unused]] int removeResult = remove( fileName.c_str() );

	Console.print( strutil::format("Deleted MessageBoard file for Board Serial 0x%X\n", msgBoardSer) );
}

//o-----------------------------------------------------------------------------------------------o
//|	Function	-	void MsgBoardMaintenance( void )
//|	Date		-	8/10/2005
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Finds all .bbf files and cleans any deleted posts from them
//|							It will also remove any empty .bbf files if necessary.
//o-----------------------------------------------------------------------------------------------o
void MsgBoardMaintenance( void )
{
	std::vector< msgBoardPost_st > mbMessages;
	std::vector< msgBoardPost_st >::const_iterator mbIter;

	std::string dirName = "./msgboards/";
	if( !cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD ).empty() )
	{
		dirName = cwmWorldState->ServerData()->Directory( CSDDP_MSGBOARD );
	}

	// Fetch file listing from msgboards directory
	for( auto &entry : std::filesystem::directory_iterator(dirName) )
	{
		if( std::filesystem::is_regular_file(entry.path()) )
		{
			// It is a regular file
			if( entry.path().extension() == ".bbf" )
			{
				// Open file stream for reading in binary mode
				std::ifstream file ( entry.path().string(), std::ios::in | std::ios::binary );
				if( file.is_open() )
				{
					// Seek start of file
					file.seekg( 0, std::ios::beg );
					
					// As long as end of file hasn't been reached, continue reading in more posts
					while( !file.eof() )
					{
						msgBoardPost_st toAdd;
						
						// If post had Toggle value other than 0x00, add it to the list of posts to write back out to file later
						if( MsgBoardReadPost( file, toAdd ) )
						{
							mbMessages.push_back( toAdd );
						}
					}
					file.close();
					
					// If there are no more posts in the file, delete the file
					if( mbMessages.empty() )
					{
						std::filesystem::remove( entry.path() );
					}
					else
					{
						// Open an output file stream for the .bbf file in binary mode
						std::ofstream mFile ( entry.path(), std::ios::out | std::ios::trunc | std::ios::binary );
						
						// Write each of the posts we want to keep back into the file
						mFile.seekp( 0, std::ios::beg );
						for( mbIter = mbMessages.begin(); mbIter != mbMessages.end(); ++mbIter )
						{
							MsgBoardWritePost( mFile, (*mbIter) );
						}
						mFile.close();
					}
					
					// Clear the message vector to prepare it for next file (if any)
					mbMessages.clear();
				}
				else
				{
					Console.error( strutil::format("Failed to open MessageBoard file for reading: %s", entry.path().string().c_str() ));
				}

			}
		}
	}
}
