//

#ifndef account_hpp
#define account_hpp

#include <array>

#include <bitset>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <limits>
#include <map>
#include <ostream>
#include <string>

class CChar;
namespace account {
using serial_t = std::uint32_t;
using acctnum_t = std::uint16_t;
using charnum_t = std::uint8_t;
using attribute_t = std::uint16_t;
} // namespace account

//======================================================================
// AccountCharacter
//======================================================================
//======================================================================
struct AccountCharacter {
    static constexpr auto INVALID_SERIAL = std::numeric_limits<account::serial_t>::max();

    account::serial_t serialNumber;
    std::string name;
    CChar *pointer;
    AccountCharacter(account::serial_t serialNumber = INVALID_SERIAL,
                     const std::string &name = "UNKNOWN");
    AccountCharacter(const std::string &line);
    auto describe() const -> std::string;
    auto realName() -> std::string;
};
//======================================================================
// AccountEntry
//======================================================================
//======================================================================
struct AccountEntry {
  private:
    auto findFreeCharacter() const -> account::charnum_t;
    [[maybe_unused]] auto writeOrphanHeader(const std::filesystem::path &path) const
        -> const AccountEntry &;
    auto writeOrphan(AccountCharacter entry) -> bool;
    auto packCharacters() -> void;

  public:
    static constexpr auto CHARACTERCOUNT = static_cast<account::charnum_t>(7);
    static constexpr auto INVALID_ACCOUNT = std::numeric_limits<account::acctnum_t>::max();

    enum AttributeFlag {
        BANNED = 0,
        SUSPENDED,
        PUBLIC,
        ONLINE,
        CHARACTER1,
        CHARACTER2,
        CHARACTER3,
        CHARACTER4,
        CHARACTER5,
        CHARACTER6,
        CHARACTER7,
        YOUNG,
        UNUSED,
        SEER,
        COUNSELOR,
        GM,
        COUNT
    };
    std::string username;
    std::string password;
    std::string contact;

    std::bitset<AttributeFlag::COUNT> flag;

    bool changed;
    account::acctnum_t accountNumber; // Why std::uint16_t ?  why not just an int

    // the sizes are based on UOX3
    std::uint32_t firstLoginTime;
    std::uint32_t timeBan;
    std::uint32_t totalPlayingTime;
    std::uint32_t inGame;
    std::uint32_t lastIP;

    std::uint32_t lastClientVersion;
    std::uint32_t lastClientType;
    std::uint8_t lastClientVersionShort;

    std::array<AccountCharacter, CHARACTERCOUNT> character;

    AccountEntry();
    AccountEntry(const std::string &username, const std::string &password,
                 account::attribute_t attribute = 0, const std::string &contact = "");
    [[maybe_unused]] auto load(account::acctnum_t accountNumber, std::istream &input)
        -> AccountEntry &;
    auto save(std::ostream &output) const -> void;
    auto operator[](account::charnum_t characterSlot) const -> const AccountCharacter &;
    auto operator[](account::charnum_t characterSlot) -> AccountCharacter &;
    // some helper methods
    auto test(AttributeFlag flag) const -> bool;
    auto set(AttributeFlag flag, bool state) -> void;

    [[maybe_unused]] auto addCharacter(account::charnum_t characterSlot,
                                       const AccountCharacter &accountCharacter) -> bool;
    [[maybe_unused]] auto addCharacter(const AccountCharacter &accountCharacter)
        -> account::charnum_t;
    [[maybe_unused]] auto delCharacter(account::charnum_t characterSlot) -> bool;
};

//======================================================================
// Account
//======================================================================
//======================================================================
class Account {

    Account();
    std::map<account::acctnum_t, AccountEntry> accounts;

    [[maybe_unused]] auto writeImportHeader(const std::filesystem::path &path) const
        -> const Account &;
    [[maybe_unused]] auto version(std::ostream &output) const -> const Account &;
    [[maybe_unused]] auto header(std::ostream &output) const -> const Account &;

    auto delAccount(std::map<account::acctnum_t, AccountEntry>::iterator &iter) -> bool;

    auto save(std::ostream &output) const -> void;
    auto load(std::istream &input) -> void;
    auto resetAccount() -> void;

  public:
    static AccountEntry INVALID_ACCOUNT_ENTRY;
    static std::filesystem::path accountDirectory;

    Account(const Account &) = delete;
    Account(Account &&) = delete;
    auto operator=(const Account &) = delete;
    auto operator=(Account &&) = delete;
    static auto shared() -> Account &;

    auto operator[](account::acctnum_t accountNumber) const -> const AccountEntry &;
    auto operator[](account::acctnum_t accountNumber) -> AccountEntry &;
    auto operator[](const std::string &name) const -> const AccountEntry &;
    auto operator[](const std::string &name) -> AccountEntry &;

    [[maybe_unused]] auto save() const -> const Account &;
    [[maybe_unused]] auto load() -> Account &;
    auto exists(const std::string &username) const -> bool;
    auto createAccount(const std::string &username, const std::string &password,
                       account::attribute_t attributes = 0x0000, const std::string &contact = "")
        -> account::acctnum_t;
    auto delAccount(const std::string &username) -> bool;
    auto delAccount(account::acctnum_t accountNumber) -> bool;

    auto size() const -> size_t;
    auto clear() -> void;

    auto importAccounts() -> std::uint16_t;

    auto allAccounts() const -> const std::map<account::acctnum_t, AccountEntry> &;
    auto allAccounts() -> std::map<account::acctnum_t, AccountEntry> &;
};

#endif /* account_hpp */
