#ifndef __MSGBOARD_H__
#define __MSGBOARD_H__

#include <cstdint>
#include <string>
#include <vector>

#include "typedefs.h"

class CChar;
class CSocket;

// Different types of user posts
enum PostTypes { PT_GLOBAL = 1, PT_REGIONAL, PT_LOCAL };

const std::uint32_t BASEGLOBALPOST = 0x01000000;
const std::uint32_t BASEREGIONPOST = 0x02000000;
const std::uint32_t BASELOCALPOST = 0x03000000;

// Different types of QUESTS (nQuestType)
// Added for Quests (ESCORTS)
// Reason for starting high and counting down, is that I store the entire user
// posted message as is (which includes the message type).  The message type
// for a user post is 0x05 and I also use this field to determine whether the
// post is marked for deletion (0x00).  In order to allow for the maximum number
// of different quest types, I opted to start high and count down.
enum QuestTypes {
    QT_NOQUEST = 0x00,
    QT_ITEMQUEST = 0xFD,
    QT_BOUNTYQUEST = 0xFE,
    QT_ESCORTQUEST = 0xFF
};

struct MsgBoardPost_st {
    std::uint32_t serial;
    std::uint16_t size;
    std::uint8_t posterLen;
    char poster[MAX_NAME];
    std::uint8_t subjectLen;
    char subject[256];
    std::uint8_t dateLen;
    char date[256];
    std::uint8_t lines;
    std::uint32_t parentSerial;
    std::uint8_t toggle;
    
    std::vector<std::string> msgBoardLine;
    
    MsgBoardPost_st()
    : serial(0), size(0), posterLen(0), subjectLen(0), dateLen(0), lines(0), parentSerial(0),
    toggle(0) {
        msgBoardLine.resize(0);
    }
};

// Function Prototypes
void MsgBoardQuestEscortCreate(CChar *mNPC);
void MsgBoardQuestEscortArrive(CSocket *mSock, CChar *mNPC);
void MsgBoardQuestEscortRemovePost(CChar *mNPC);
void MsgBoardRemoveFile(const serial_t msgBoardSer);
void MsgBoardMaintenance(void);

#endif
