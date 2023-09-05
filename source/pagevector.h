
// o------------------------------------------------------------------------------------------------o
//| File		-	pagevector.h
// o------------------------------------------------------------------------------------------------o
//| Purpose		-	CHelpRequest and PageVector classes. For use in the GM and Couselor
// queues
// o------------------------------------------------------------------------------------------------o

#ifndef __PAGEVECTOR_H__
#define __PAGEVECTOR_H__

#include <cstdint>
#include <string>
#include <vector>

#include "typedefs.h"

class CChar;

class CHelpRequest {
private:
    serial_t helpReqId;
    serial_t playerPaging;   // player who is paging
    serial_t playerHandling; // player who is handling it
    std::int8_t priority;         // priority of the page
    bool handled;          // has it been handled?
    time_t timeOfPage;     // when was the page reported?
    std::string reason;    // reason for the page
    
public:
    CHelpRequest()
    : helpReqId(INVALIDSERIAL), playerPaging(INVALIDSERIAL), playerHandling(INVALIDSERIAL),
    priority(0), handled(false), timeOfPage(0) {
        reason = "";
    }
    ~CHelpRequest();
    
    serial_t WhoPaging() const;
    serial_t WhoHandling() const;
    std::int8_t Priority() const;
    bool IsHandled() const;
    time_t TimeOfPage() const;
    std::string Reason() const;
    serial_t RequestId() const;
    
    void WhoPaging(serial_t pPaging);
    void WhoHandling(serial_t pHandling);
    void Priority(std::int8_t pPriority);
    void IsHandled(bool pHandled);
    void TimeOfPage(time_t pTime);
    void Reason(const std::string &pReason);
    void RequestId(serial_t hrid);
};

class PageVector {
private:
    std::vector<CHelpRequest *> requestQueue;
    std::vector<CHelpRequest *>::iterator currentPos;
    std::string title; // GM/Counselor Queue
    
    [[maybe_unused]] R32 avgEntryTime;
    [[maybe_unused]] R32 maxEntryTime;
    [[maybe_unused]] R32 minEntryTime;
    serial_t maxId;
    
    void KillQueue();
    
public:
    serial_t Add(CHelpRequest *toAdd);
    bool Remove();
    CHelpRequest *First();
    CHelpRequest *Next();
    CHelpRequest *Current();
    bool AtEnd() const;
    size_t NumEntries() const;
    PageVector();
    PageVector(const std::string &newTitle);
    ~PageVector();
    void SendAsGump(CSocket *toSendTo);
    void setTitle(const std::string &newTitle);
    bool GotoPos(std::int32_t pos);
    std::int32_t CurrentPos() const;
    serial_t GetCallNum() const;
    std::int32_t FindCallNum(serial_t callNum);
    bool AnswerNextCall(CSocket *mSock, CChar *mChar);
};

extern PageVector *GMQueue;
extern PageVector *CounselorQueue;

#endif
