#ifndef __CMULTIOBJ_H__
#define __CMULTIOBJ_H__

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "genericlist.h"
#include "typedefs.h"

#include "citem.h"

class CChar;

class CMultiObj : public CItem {
  protected:
    std::string deed;

    std::map<CChar *, std::uint8_t> housePrivList;

    std::vector<CItem *> lockedList;
    std::uint16_t maxLockdowns;

    std::vector<CItem *> secureContainerList;
    std::uint16_t maxSecureContainers;

    std::uint16_t maxFriends;
    std::uint16_t maxGuests;
    std::uint16_t maxOwners;
    std::uint16_t maxBans;

    std::uint16_t maxVendors;

    std::vector<CItem *> trashContainerList;
    std::uint16_t maxTrashContainers;

    std::vector<CChar *> vendorList;

    std::int16_t banX;
    std::int16_t banY;

    bool isPublic;
    std::uint16_t numberOfVisits;

    time_t buildTimestamp;
    time_t tradeTimestamp;

    virtual bool LoadRemnants(void) override;

    GenericList<CItem *> itemInMulti;
    GenericList<CChar *> charInMulti;
    GenericList<CChar *> ownersOfMulti;
    GenericList<CChar *> friendsOfMulti;
    GenericList<CChar *> bannedFromMulti;
    GenericList<CChar *> guestsOfMulti;

  public:
    CMultiObj();
    virtual ~CMultiObj();

    // Banlist
    void AddToBanList(CChar *toBan);
    void RemoveFromBanList(CChar *toRemove);
    void ClearBanList(void);
    std::uint16_t GetBanCount(void);
    std::uint16_t GetMaxBans(void) const;
    void SetMaxBans(std::uint16_t newValue);

    // Owner/CoOwnerlist
    void AddAsOwner(CChar *newOwner);
    void RemoveAsOwner(CChar *toRemove);
    void ClearOwnerList(void);
    std::uint16_t GetOwnerCount(void);
    std::uint16_t GetMaxOwners(void) const;
    void SetMaxOwners(std::uint16_t newValue);

    // Friendlist
    void AddAsFriend(CChar *newFriend);
    void RemoveAsFriend(CChar *toRemove);
    void ClearFriendList(void);
    std::uint16_t GetFriendCount(void);
    std::uint16_t GetMaxFriends(void) const;
    void SetMaxFriends(std::uint16_t newValue);

    // Guestlist (not saved)
    void AddAsGuest(CChar *newGuest);
    void RemoveAsGuest(CChar *toRemove);
    void ClearGuestList(void);
    std::uint16_t GetGuestCount(void);
    std::uint16_t GetMaxGuests(void) const;
    void SetMaxGuests(std::uint16_t newValue);

    // Lockdowns
    size_t GetLockdownCount(void) const;
    std::uint16_t GetMaxLockdowns(void) const;
    void SetMaxLockdowns(std::uint16_t newValue);
    void LockDownItem(CItem *toLock);
    void ReleaseItem(CItem *toRemove);

    // Secure Containers
    size_t GetSecureContainerCount(void) const;
    std::uint16_t GetMaxSecureContainers(void) const;
    void SetMaxSecureContainers(std::uint16_t newValue);
    bool IsSecureContainer(CItem *toCheck);
    void SecureContainer(CItem *toSecure);
    void UnsecureContainer(CItem *toUnsecure);

    // Trash Containers
    size_t GetTrashContainerCount(void) const;
    std::uint16_t GetMaxTrashContainers(void) const;
    void SetMaxTrashContainers(std::uint16_t newValue);
    void AddTrashContainer(CItem *toAdd);
    void RemoveTrashContainer(CItem *toRemove);

    // Player Vendor list
    size_t GetVendorCount(void) const;
    std::uint16_t GetMaxVendors(void) const;
    void SetMaxVendors(std::uint16_t newValue);
    void AddVendor(CChar *toAdd);
    void RemoveVendor(CChar *toRemove);

    bool IsOnBanList(CChar *toBan) const;
    bool IsOnFriendList(CChar *toCheck) const;
    bool IsOnGuestList(CChar *toCheck) const;
    bool IsOnOwnerList(CChar *toCheck) const;
    bool CheckForAccountCoOwnership(CChar *toCheck) const;
    bool IsOwner(CChar *toFind) const;

    void AddToMulti(CBaseObject *toAdd);
    void RemoveFromMulti(CBaseObject *toRemove);

    bool GetPublicStatus(void) const;
    void SetPublicStatus(const bool newBool);

    std::int16_t GetBanX(void) const;
    void SetBanX(const std::int16_t newVal);
    std::int16_t GetBanY(void) const;
    void SetBanY(const std::int16_t newVal);

    std::string GetBuildTimestamp(void) const;
    void SetBuildTimestamp(time_t newTime);
    std::string GetTradeTimestamp(void) const;
    void SetTradeTimestamp(time_t newTime);

    virtual bool Save(std::ostream &outStream) override;
    virtual bool DumpHeader(std::ostream &outStream) const override;
    virtual bool DumpBody(std::ostream &outStream) const override;
    virtual bool HandleLine(std::string &UTag, std::string &data) override;

    virtual void SetOwner(CChar *newOwner) override;

    std::string GetDeed(void) const;
    void SetDeed(const std::string &newDeed);

    virtual void Cleanup(void) override;

    virtual bool CanBeObjType(ObjectType toCompare) const override;

    GenericList<CChar *> *GetOwnersOfMultiList(bool clearList = false);
    GenericList<CChar *> *GetFriendsOfMultiList(bool clearList = false);
    GenericList<CChar *> *GetGuestsOfMultiList(bool clearList = false);
    GenericList<CChar *> *GetBannedFromMultiList(bool clearList = false);
    GenericList<CChar *> *GetCharsInMultiList(void);
    GenericList<CItem *> *GetItemsInMultiList(void);
};

class CBoatObj : public CMultiObj {
  protected:
    serial_t tiller;
    serial_t planks[2];
    serial_t hold;
    std::int8_t moveType;

    timerval_t nextMoveTime;

  private:
    virtual bool DumpHeader(std::ostream &outStream) const override;
    virtual bool DumpBody(std::ostream &outStream) const override;

    virtual bool HandleLine(std::string &UTag, std::string &data) override;

  public:
    CBoatObj();
    virtual ~CBoatObj();

    serial_t GetTiller(void) const;
    serial_t GetPlank(std::uint8_t plankNum) const;
    serial_t GetHold(void) const;
    std::int8_t GetMoveType(void) const;

    void SetPlank(std::uint8_t plankNum, serial_t newVal);
    void SetTiller(serial_t newVal);
    void SetHold(serial_t newVal);
    void SetMoveType(std::int8_t newVal);

    timerval_t GetMoveTime(void) const;
    void SetMoveTime(timerval_t newVal);

    virtual bool CanBeObjType(ObjectType toCompare) const override;
};

#endif
