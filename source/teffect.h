#ifndef __TEFFECT_H__
#define __TEFFECT_H__

#include <cstdint>
#include <ostream>
#include <string>

#include "typedefs.h"

class CBaseObject;

class CTEffect {
  private:
    SERIAL source;
    SERIAL dest;
    std::uint32_t expiretime;
    std::uint8_t num;
    std::uint16_t more1;
    std::uint16_t more2;
    std::uint16_t more3;
    bool dispellable;
    CBaseObject *objptr;
    std::uint16_t assocScript;

  public:
    CTEffect()
        : source(INVALIDSERIAL), dest(INVALIDSERIAL), expiretime(0), num(0), more1(0), more2(0),
          more3(0), dispellable(false), objptr(nullptr), assocScript(0xFFFF) {}

    std::uint16_t AssocScript(void) const { return assocScript; }
    CBaseObject *ObjPtr(void) const { return objptr; }
    bool Dispellable(void) const { return dispellable; }
    std::uint32_t ExpireTime(void) const { return expiretime; }
    SERIAL Source(void) const { return source; }
    SERIAL Destination(void) const { return dest; }
    std::uint8_t Number(void) const { return num; }
    std::uint16_t More1(void) const { return more1; }
    std::uint16_t More2(void) const { return more2; }
    std::uint16_t More3(void) const { return more3; }

    void Source(SERIAL value) { source = value; }
    void Destination(SERIAL value) { dest = value; }
    void ExpireTime(std::uint32_t value) { expiretime = value; }
    void Number(std::uint8_t value) { num = value; }
    void More1(std::uint16_t value) { more1 = value; }
    void More2(std::uint16_t value) { more2 = value; }
    void More3(std::uint16_t value) { more3 = value; }
    void Dispellable(bool value) { dispellable = value; }
    void ObjPtr(CBaseObject *value) { objptr = value; }
    void AssocScript(std::uint16_t value) { assocScript = value; }

    bool Save(std::ostream &effectDestination) const; // saves the current effect
};

#endif
