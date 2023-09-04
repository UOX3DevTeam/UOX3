#ifndef __WHOLIST_H__
#define __WHOLIST_H__

#include <cstdint>
#include <string>
#include <vector>

#include "typedefs.h"

class CSocket;

class CWhoList {
  private:
    bool needsUpdating; // true if the list needs updating (new player online, new char made)
    std::int32_t gmCount;       // number of GMs already in it
    bool online;

    std::vector<serial_t> whoMenuData;
    std::vector<std::string> one, two; // replacement for entries1, entries2

    void Update(); // force the list to update
    void ResetUpdateFlag();
    void AddSerial(serial_t toAdd);
    void Delete();
    void Command(CSocket *toSendTo, std::uint8_t type, std::uint16_t buttonPressed);

  public:
    CWhoList(bool trulyOnline = true);
    ~CWhoList();
    void FlagUpdate();
    void SendSocket(CSocket *toSendTo);
    void GMEnter();
    void GMLeave();
    void ButtonSelect(CSocket *toSendTo, std::uint16_t buttonPressed, std::uint8_t type);
    void ZeroWho();
    void SetOnline(bool newValue);
};

extern CWhoList *WhoList;
extern CWhoList *OffList;

#endif
