//------------------------------------------------------------------------
//  msgboard.h
//
//------------------------------------------------------------------------
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1999 - 2001 by Unknown real name (Dupois)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//	
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//   
//------------------------------------------------------------------------
// msgboard.h
//

// Maximum number of posts per board
// BUFFER SIZE = 2560 
// Therefore 0x3c max size = 2560 - 5 ( 0x3c header info ) 
//                         = 2550 
//
//                           2550 / 19 ( item segment size per msg ) 
//                         = 134 
// Round down to 128 Messages allowable on a message board (better safe than sorry)
// MAXPOSTS = 128 
#define MAXPOSTS        128

// Maximum number of entries in a ESCORTS list in the MSGBOARD.SCP file
#define MAXENTRIES      256

// Different types of user posts
#define LOCALPOST       0
#define REGIONALPOST    1
#define GLOBALPOST      2

// Different types of QUESTS (nQuestType)
// Added for Quests (ESCORTS)
// Reason for starting high and counting down, is that I store the entire user
// posted message as is (which includes the message type).  The message type
// for a user post is 0x05 and I also use this field to determine whether the
// post is marked for deletion (0x00).  In order to allow for the maximum number
// of different quest types, I opted to start high and count down.
#define ESCORTQUEST 0xFF
#define BOUNTYQUEST 0xFE
#define ITEMQUEST   0xFD


// Function Prototypes 
void    MsgBoardEvent( int nSerial );
void    MsgBoardSetPostType( int nSerial, int nPostType );
void    MsgBoardGetPostType( int nSerial );
int     MsgBoardPostQuest( int nSerial, int nQuestType );
void    MsgBoardQuestEscortCreate( int nSerial );
void    MsgBoardQuestEscortArrive( int nNPCIndex, int nPCIndex );
void    MsgBoardQuestEscortDelete( int nNPCIndex );
void    MsgBoardQuestEscortRemovePost( int nNPCIndex );
void    MsgBoardMaintenance( void );
#ifdef __linux__
std::vector<std::string> MsgBoardGetFile( char* pattern, char* path) ;
#endif

