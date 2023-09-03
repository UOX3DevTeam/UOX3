
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
    SERIAL helpReqId;
    SERIAL playerPaging;   // player who is paging
    SERIAL playerHandling; // player who is handling it
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

    SERIAL WhoPaging(void) const;
    SERIAL WhoHandling(void) const;
    std::int8_t Priority(void) const;
    bool IsHandled(void) const;
    time_t TimeOfPage(void) const;
    std::string Reason(void) const;
    SERIAL RequestId(void) const;

    void WhoPaging(SERIAL pPaging);
    void WhoHandling(SERIAL pHandling);
    void Priority(std::int8_t pPriority);
    void IsHandled(bool pHandled);
    void TimeOfPage(time_t pTime);
    void Reason(const std::string &pReason);
    void RequestId(SERIAL hrid);
};

class PageVector {
  private:
    std::vector<CHelpRequest *> requestQueue;
    std::vector<CHelpRequest *>::iterator currentPos;
    std::string title; // GM/Counselor Queue

    [[maybe_unused]] R32 avgEntryTime;
    [[maybe_unused]] R32 maxEntryTime;
    [[maybe_unused]] R32 minEntryTime;
    SERIAL maxId;

    void KillQueue(void);

  public:
    SERIAL Add(CHelpRequest *toAdd);
    bool Remove(void);
    CHelpRequest *First(void);
    CHelpRequest *Next(void);
    CHelpRequest *Current(void);
    bool AtEnd(void) const;
    size_t NumEntries(void) const;
    PageVector();
    PageVector(const std::string &newTitle);
    ~PageVector();
    void SendAsGump(CSocket *toSendTo);
    void SetTitle(const std::string &newTitle);
    bool GotoPos(std::int32_t pos);
    std::int32_t CurrentPos(void) const;
    SERIAL GetCallNum(void) const;
    std::int32_t FindCallNum(SERIAL callNum);
    bool AnswerNextCall(CSocket *mSock, CChar *mChar);
};

extern PageVector *GMQueue;
extern PageVector *CounselorQueue;

#endif
