#ifndef __CGUMP_H__
#define __CGUMP_H__

#include <cstdint>
#include <string>
#include <vector>

#include "typedefs.h"

class CSocket;

void MultiGumpCallback(CSocket *mySocket, serial_t GumpSerial, std::uint32_t Button);

class CGump {
private:
    std::vector<std::string> TagList;
    std::vector<std::string> TextList;
    bool NoMove;
    bool NoClose;
    std::uint32_t Serial;
    std::uint16_t PageCount; // Shouldn't have more than 65535 pages
    std::uint32_t Type;
    
public:
    CGump(bool myNoMove, bool myNoClose);
    ~CGump();
    
    void Add(const std::string &Tag, const std::string &Text);
    void Send(CSocket *target);
    
    // Common add functions
    void AddBackground(std::uint16_t x, std::uint16_t y, std::uint16_t GumpId, std::uint16_t width, std::uint16_t height);
    void AddGump(std::uint16_t x, std::uint16_t y, std::uint16_t GumpId);
    void AddButton(std::uint16_t x, std::uint16_t y, std::uint16_t ImageUp, std::uint16_t ImageDown, std::uint16_t Behaviour, std::uint16_t Page,
                   std::uint32_t UniqueId);
    void AddText(std::uint16_t x, std::uint16_t y, std::uint16_t hue, std::string Text);
    std::uint16_t StartPage(void);
    
    void SetNoMove(bool myNoMove);
    void SetNoClose(bool myNoClose);
    
    void SetType(std::uint32_t newType);
    void SetSerial(std::uint32_t newSerial);
};

class CGumpDisplay {
private:
    struct GumpInfo_st {
        std::string name;
        std::uint32_t value;
        std::uint8_t type;
        std::string stringValue;
        // acceptable type values
        // 0 -> straight plain std::uint32_t
        // 1 -> hex std::uint32_t
        // 2 -> std::uint32_t separated into 4 parts, decimal
        // 3 -> std::uint32_t separated into 4 parts, hex
        // 4 -> string
        // 5 -> 2 byte hex display
        // 6 -> 2 byte decimal display
    };
    
    std::vector<GumpInfo_st *> gumpData;
    std::uint16_t width, height; // gump width / height
    CSocket *toSendTo;
    std::vector<std::string> one, two;
    std::string title;
    
public:
    void AddData(GumpInfo_st *toAdd);
    void AddData(std::string toAdd, std::uint32_t value, std::uint8_t type = 0);
    void AddData(std::string toAdd, const std::string &toSet, std::uint8_t type = 4);
    CGumpDisplay(CSocket *target);
    CGumpDisplay(CSocket *target, std::uint16_t gumpWidth, std::uint16_t gumpHeight);
    ~CGumpDisplay();
    void SetTitle(const std::string &newTitle);
    void Send(std::uint32_t gumpNum, bool isMenu, serial_t serial);
    void Delete(void);
};

#endif
