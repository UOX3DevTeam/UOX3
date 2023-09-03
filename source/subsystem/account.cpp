//

#include "account.hpp"

#include <fstream>
#include <stdexcept>

#include "utility/strutil.hpp"

#include "other/uoxversion.hpp"
#include "subsystem/console.hpp"

#include "funcdecl.h"
#include "cchar.h"

using namespace std::string_literals;

//======================================================================
// AccountCharacter
//======================================================================
//======================================================================
AccountCharacter::AccountCharacter(account::serial_t serialNumber, const std::string &name) {
    this->serialNumber = serialNumber;
    this->name = name;
    this->pointer = nullptr;
}
//======================================================================
AccountCharacter::AccountCharacter(const std::string &line) : AccountCharacter() {
    auto [ser, charname] = util::split(line, " ");
    this->serialNumber = static_cast<account::serial_t>(std::stoul(ser, nullptr, 0));
    this->name = util::contentsOf(charname, "[", "]");
    if (serialNumber != INVALID_SERIAL) {
        this->pointer = CalcCharObjFromSer(serialNumber);
        if (pointer == nullptr) {
            serialNumber = INVALID_SERIAL;
            name = "UNKNOWN";
        }
        else {
            name = pointer->GetName();
        }
    }
}
//======================================================================
auto AccountCharacter::describe() const -> std::string {
    const auto format = "0x%0.8x [%s]";
    return util::format(format, serialNumber, name.c_str());
}
//======================================================================
auto AccountCharacter::realName() -> std::string {
    if (pointer != nullptr) {
        // return pointer->GetName() ;
        return std::string();
    }
    else {
        return this->name;
    }
}
//======================================================================
// AccountEntry
//======================================================================
//======================================================================
auto AccountEntry::findFreeCharacter() const -> account::charnum_t {
    auto charNum = CHARACTERCOUNT;
    for (account::charnum_t i = 0; i < CHARACTERCOUNT; i++) {
        if (character.at(i).serialNumber == AccountCharacter::INVALID_SERIAL) {
            charNum = i;
            break;
        }
    }
    return charNum;
}
//======================================================================
auto AccountEntry::writeOrphanHeader(const std::filesystem::path &path) const
    -> const AccountEntry & {
    if (!std::filesystem::exists(path)) {
        auto output = std::ofstream(path);
        if (output.is_open()) {
            // output << "//OI3.0" << "-UV" << CVersionClass::GetVersion() << "-BD" <<
            // CVersionClass::GetBuild() << "-DS" << time( nullptr ) << "-ED" <<
            // CVersionClass::GetRealBuild() << "\n";
            output << "version version," << UOXVersion::version << ":build," << UOXVersion::build
                   << ":realbuild," << UOXVersion::realBuild << "\n";
            output << "//"
                      "----------------------------------------------------------------------------"
                      "--\n";
            output << "// Orphans.Adm \n";
            output << "//"
                      "----------------------------------------------------------------------------"
                      "--\n";
            output
                << "// UOX3 uses this file to store any characters that have been removed from\n";
            output << "// an account. They are stored here so there is some history of user "
                      "deltions\n";
            output << "// of their characters. At best the co-ordinate may be available.\n";
            output << "// Name, X/Y/Z values if available will be displayed to ease locating "
                      "trouble\n";
            output << "// users, and where they deleted their characters last.\n";
            output << "// \n";
            output << "// The format is as follows:\n";
            output << "// \n";
            output << "//    username=ID,CharacterName,Serial#,X,Y,Z\n";
            output << "// \n";
            output << "// NOTE: CharacterName, and Coordinates may not be available.\n";
            output << "//"
                      "----------------------------------------------------------------------------"
                      "--\n\n";
        }
    }
    return *this;
}
//======================================================================
auto AccountEntry::writeOrphan(AccountCharacter entry) -> bool {
    if (entry.pointer != nullptr) {
        auto path = Account::accountDirectory / std::filesystem::path("orphan.adm");
        writeOrphanHeader(path);
        auto output = std::ofstream(path.string());
        if (output.is_open()) {
            output << "username = " << this->username << "," << entry.pointer->GetName() << ",0x"
                   << std::hex << entry.pointer->GetSerial() << std::dec << ","
                   << entry.pointer->GetX() << "," << entry.pointer->GetY() << entry.pointer->GetZ()
                   << std::endl;
        }
    }
    return true;
}

//======================================================================
auto AccountEntry::packCharacters() -> void {
    auto temp = std::vector<AccountCharacter>();
    for (const auto &entry : character) {
        if (entry.serialNumber != AccountCharacter::INVALID_SERIAL) {
            temp.push_back(entry);
        }
    }
    std::fill(character.begin(), character.end(), AccountCharacter());
    auto slot = 0;
    for (const auto &entry : temp) {
        character.at(slot) = entry;
        slot++;
    }
}
//======================================================================
AccountEntry::AccountEntry() {
    flag.reset();

    changed = false;
    accountNumber = INVALID_ACCOUNT;
    firstLoginTime = 0;
    timeBan = 0;
    totalPlayingTime = 0;
    inGame = std::numeric_limits<std::uint32_t>::max();
    lastIP = 0;

    lastClientVersion = 0;
    lastClientType = 0;
    lastClientVersionShort = 0;

    std::fill(character.begin(), character.end(), AccountCharacter());
}
//======================================================================
AccountEntry::AccountEntry(const std::string &username, const std::string &password,
                           account::attribute_t attribute, const std::string &contact)
    : AccountEntry() {
    this->username = util::lower(username);
    this->password = password;
    flag = std::bitset<AttributeFlag::COUNT>(attribute);
    this->contact = contact;
}
//======================================================================
auto AccountEntry::save(std::ostream &output) const -> void {
    output << "SECTION ACCOUNT " << accountNumber << "\n{\n";
    output << "\tNAME " << username << "\n";
    output << "\tPASS " << password << "\n";
    output << "\tFLAGS " << util::format("0x%0.4x", flag.to_ullong()) << "\n";
    output << "\tTIMEBAN " << util::format("0x%x", timeBan) << "\n";
    output << "\tFIRSTLOGIN " << util::format("0x%x", firstLoginTime) << "\n";
    output << "\tTOTALPLAYTIME " << util::format("0x%x", totalPlayingTime) << "\n";
    output << "\tLASTIP " << util::format("0x%0.8x", lastIP) << "\n";
    output << "\tCONTACT " << contact << "\n";
    for (auto i = 0; i < CHARACTERCOUNT; i++) {
        output << "\tCHARACTER-" << i + 1 << " " << character.at(i).describe() << "\n";
    }
    output << "}\n\n";
}
//====================================================================
auto AccountEntry::load(account::acctnum_t accountNumber, std::istream &input) -> AccountEntry & {
    this->accountNumber = accountNumber;
    auto buffer = std::vector<char>(4096, 0);
    while (input.good() && !input.eof()) {
        input.getline(buffer.data(), buffer.size() - 1);
        if (input.gcount() > 0) {
            buffer[input.gcount()] = 0;
            std::string line = buffer.data();
            line = util::trim(util::strip(line, "//"));
            if (!line.empty()) {
                if (line[0] == '}') {
                    break;
                }
                auto [key, value] = util::split(line, " ");
                key = util::lower(key);
                if (key == "name") {
                    this->username = value;
                }
                else if (key == "pass") {
                    this->password = value;
                }
                else if (key == "flags") {
                    flag = std::bitset<AttributeFlag::COUNT>(std::stoul(value, nullptr, 0));
                }
                else if (key == "timeban") {
                    this->timeBan = static_cast<std::uint32_t>(std::stoul(value, nullptr, 0));
                }
                else if (key == "firstlogin") {
                    this->firstLoginTime =
                        static_cast<std::uint32_t>(std::stoul(value, nullptr, 0));
                }
                else if (key == "lastip") {
                    this->lastIP = static_cast<std::uint32_t>(std::stoul(value, nullptr, 0));
                }
                else if (key == "contact") {
                    this->contact = value;
                }
                else if (key.find("character-") == 0) {
                    // It is a character
                    key = key.substr(10);
                    auto accountSlot = std::stoi(key) - 1;
                    if (accountSlot >= 0 && accountSlot < CHARACTERCOUNT) {
                        character.at(accountSlot) = AccountCharacter(value);
                    }
                }
            }
        }
    }
    packCharacters();
    return *this;
}

//====================================================================
auto AccountEntry::operator[](account::charnum_t characterSlot) const -> const AccountCharacter & {
    return character.at(characterSlot);
}
//====================================================================
auto AccountEntry::operator[](account::charnum_t characterSlot) -> AccountCharacter & {
    return character.at(characterSlot);
}

//====================================================================
auto AccountEntry::test(AttributeFlag flag) const -> bool { return this->flag.test(flag); }
//====================================================================
auto AccountEntry::set(AttributeFlag flag, bool state) -> void { this->flag[flag] = state; }

//====================================================================
auto AccountEntry::addCharacter(account::charnum_t characterSlot,
                                const AccountCharacter &accountCharacter) -> bool {

    if (characterSlot < CHARACTERCOUNT) {
        character.at(characterSlot) = AccountCharacter(accountCharacter);
        return true;
    }
    return false;
}
//====================================================================
auto AccountEntry::addCharacter(const AccountCharacter &accountCharacter) -> account::charnum_t {

    auto first = findFreeCharacter();
    if (first != CHARACTERCOUNT) {
        character.at(first) = AccountCharacter(accountCharacter);
        return first;
    }

    return AccountEntry::CHARACTERCOUNT;
}
//====================================================================
auto AccountEntry::delCharacter(account::charnum_t characterSlot) -> bool {
    if (character.at(characterSlot).pointer != nullptr) {
        writeOrphan(character.at(characterSlot));
    }
    character.at(characterSlot) = AccountCharacter();
    packCharacters();

    return true;
}

//======================================================================
// Account
//======================================================================
AccountEntry Account::INVALID_ACCOUNT_ENTRY = AccountEntry();
std::filesystem::path Account::accountDirectory = std::filesystem::path();
//======================================================================
Account::Account() { resetAccount(); }
//======================================================================
auto Account::writeImportHeader(const std::filesystem::path &path) const -> const Account & {
    auto output = std::ofstream(path);
    if (output.is_open()) {
        // output << "//AIMP3.0" << "-UV" << CVersionClass::GetVersion() << "-BD" <<
        // CVersionClass::GetBuild() << "-DS" << time( nullptr ) << "-ED" <<
        // CVersionClass::GetRealBuild() << std::endl;
        output << "version version," << UOXVersion::version << ":build," << UOXVersion::build
               << ":realbuild," << UOXVersion::realBuild << "\n";
        output
            << "//------------------------------------------------------------------------------\n";
        output
            << "// UOX3 uses this file to store new accounts that are to be imported on the next\n";
        output << "// time the server does a world save, or world load.\n";
        output
            << "//------------------------------------------------------------------------------\n";
        output << "// FORMAT: \n";
        output << "//";
        output << "//    USER=username,password,flags,contact\n";
        output << "//\n";
        output << "// WHERE: username   - Username of the accounts to create.\n";
        output << "//        password   - Password of the account to create.\n";
        output << "//        flags      - See accounts.adm for correct flag values.\n";
        output << "//        contact    - Usually this is the email address, but can be used as a "
                  "comment or ICQ\n";
        output << "//\n";
        output << "// NOTE: Flags, and contact values are not required, defaults will be used.\n";
        output
            << "//"
               "------------------------------------------------------------------------------\n\n";
    }
    return *this;
}
//======================================================================
auto Account::version(std::ostream &output) const -> const Account & { return *this; }
//======================================================================
auto Account::header(std::ostream &output) const -> const Account & {
    output << "version version," << UOXVersion::version << ":build," << UOXVersion::build
           << ":realbuild," << UOXVersion::realBuild << "\n";
    output << "//------------------------------------------------------------------------------\n";
    output << "//accounts.adm[TEXT] : UOX3 uses this file for shared accounts access between "
              "servers\n";
    output << "//\n";
    output << "//   Format:\n";
    output << "//      SECTION ACCOUNT 0\n";
    output << "//      {\n";
    output << "//         NAME username\n";
    output << "//         PASS password\n";
    output << "//         FLAGS 0x0000\n";
    output << "//         TIMEBAN 0\n";
    output << "//         FIRSTLOGIN 0\n";
    output << "//         TOTALPLAYTIME 0\n";
    output << "//         LASTIP 127.0.0.1\n";
    output << "//         CONTACT NONE\n";
    output << "//         CHARACTER-1 0xffffffff\n";
    output << "//         CHARACTER-2 0xffffffff\n";
    output << "//         CHARACTER-3 0xffffffff\n";
    output << "//         CHARACTER-4 0xffffffff\n";
    output << "//         CHARACTER-5 0xffffffff\n";
    output << "//         CHARACTER-6 0xffffffff\n";
    output << "//         CHARACTER-7 0xffffffff\n";
    output << "//      }\n";
    output << "//\n";
    output << "//   FLAGS:\n";
    output << "//      Bit:  0x0001) Banned           0x0002) Suspended        0x0004) Public      "
              "     0x0008) Currently Logged In\n";
    output << "//            0x0010) Char-1 Blocked   0x0020) Char-2 Blocked   0x0040) Char-3 "
              "Blocked   0x0080) Char-4 Blocked\n";
    output << "//            0x0100) Char-5 Blocked   0x0200) Char-6 Blocked   0x0400) Char-7 "
              "Blocked   0x0800) Young\n";
    output << "//            0x1000) Unused           0x2000) Seer             0x4000) Counselor   "
              "     0x8000) GM Account\n";
    output << "//\n";
    output << "//   TIMEBAN:\n";
    output << "//      This would be the end date of a timed ban.\n";
    output << "//\n";
    output << "//   FIRSTLOGIN:\n";
    output << "//      This would be timestamp of the account's first login.\n";
    output << "//\n";
    output << "//   TOTALPLAYTIME:\n";
    output << "//      This would be total playtime on account in minutes across all characters, "
              "since first login.\n";
    output << "//\n";
    output << "//   CONTACT:\n";
    output << "//      Usually this is the email address, but can be used as a comment or ICQ\n";
    output << "//\n";
    output << "//   LASTIP:\n";
    output << "//      The last IP this account was used from.\n";
    output
        << "//------------------------------------------------------------------------------\n\n";
    return *this;
}

//======================================================================
auto Account::delAccount(std::map<account::acctnum_t, AccountEntry>::iterator &iter) -> bool {
    if (iter != accounts.end()) {
        for (auto i = 0; i < AccountEntry::CHARACTERCOUNT; ++i) {
            iter->second.delCharacter(i);
        }
        accounts.erase(iter);
        return true;
    }
    return false;
}
//======================================================================
auto Account::save(std::ostream &output) const -> void {
    this->header(output);
    this->version(output).header(output);
    for (const auto &[id, entry] : accounts) {
        entry.save(output);
    }
}
//======================================================================
auto Account::load(std::istream &input) -> void {
    auto buffer = std::vector<char>(4096, 0);
    while (input.good() && !input.eof()) {
        input.getline(buffer.data(), buffer.size() - 1);
        if (input.gcount() > 0) {
            buffer[input.gcount()] = 0;
            std::string line = buffer.data();
            line = util::trim(util::strip(line, "//"));
            if (!line.empty()) {
                auto [first, rest] = util::split(line, " ");
                first = util::lower(first);
                if (first == "section") {
                    auto [type, num] = util::split(rest, " ");
                    type = util::lower(type);
                    if (type == "account") {
                        auto number = static_cast<account::acctnum_t>(std::stoul(num));
                        auto entry = AccountEntry();
                        entry.load(number, input);
                        if (!exists(entry.username)) {
                            accounts.insert_or_assign(number, std::move(entry));
                        }
                    }
                }
            }
        }
    }
}
//======================================================================
auto Account::resetAccount() -> void {
    accounts.clear();
    auto account = AccountEntry("admin", "admin", 0x8000);
    account.accountNumber = 0;
    accounts.insert_or_assign(0, account);
    account = AccountEntry("guest1", "guest1", 0x0);
    account.accountNumber = 1;
    accounts.insert_or_assign(1, account);
}

//======================================================================
//======================================================================
auto Account::shared() -> Account & {
    static Account instance;
    return instance;
}
//======================================================================
auto Account::operator[](account::acctnum_t accountNumber) const -> const AccountEntry & {
    auto iter = accounts.find(accountNumber);
    if (iter != accounts.end()) {
        return iter->second;
    }
    return Account::INVALID_ACCOUNT_ENTRY;
    // throw std::runtime_error("Account not found for number: "s+std::to_string(accountNumber));
}
//======================================================================
auto Account::operator[](account::acctnum_t accountNumber) -> AccountEntry & {
    auto iter = accounts.find(accountNumber);
    if (iter != accounts.end()) {
        return iter->second;
    }
    return Account::INVALID_ACCOUNT_ENTRY;
    // throw std::runtime_error("Account not found for number: "s+std::to_string(accountNumber));
}
//======================================================================
auto Account::operator[](const std::string &name) const -> const AccountEntry & {
    auto lname = util::lower(name);
    auto iter = std::find_if(accounts.begin(), accounts.end(),
                             [&lname](const std::pair<int, AccountEntry> &entry) {
                                 return lname == entry.second.username;
                             });
    if (iter != accounts.end()) {
        return iter->second;
    }
    return INVALID_ACCOUNT_ENTRY;
}
//======================================================================
auto Account::operator[](const std::string &name) -> AccountEntry & {
    auto lname = util::lower(name);
    auto iter = std::find_if(accounts.begin(), accounts.end(),
                             [&lname](const std::pair<int, AccountEntry> &entry) {
                                 return lname == entry.second.username;
                             });
    if (iter != accounts.end()) {
        return iter->second;
    }
    return INVALID_ACCOUNT_ENTRY;
}

//======================================================================
auto Account::save() const -> const Account & {
    auto path = accountDirectory / std::filesystem::path("accounts.adm");
    auto output = std::ofstream(path.string());
    if (output.is_open()) {
        save(output);
    }
    return *this;
}
//======================================================================
auto Account::load() -> Account & {
    accounts.clear();

    auto path = accountDirectory / std::filesystem::path("accounts.adm");
    auto input = std::ifstream(path.string());
    if (input.is_open()) {

        load(input);
    }
    else {
        // This doesnt exist, we should create it
        resetAccount();

        save();
    }
    return *this;
}

//======================================================================
auto Account::exists(const std::string &username) const -> bool {
    auto lname = util::lower(username);
    auto iter = std::find_if(accounts.begin(), accounts.end(),
                             [&lname](const std::pair<int, AccountEntry> &entry) {
                                 return lname == entry.second.username;
                             });
    return iter != accounts.end();
}
//======================================================================
auto Account::createAccount(const std::string &username, const std::string &password,
                            account::attribute_t attributes, const std::string &contact)
    -> account::acctnum_t {
    // we should check, if this user exists
    if (exists(username)) {
        return AccountEntry::INVALID_ACCOUNT;
    }
    auto entry = AccountEntry(username, password, attributes, contact);
    auto current = accounts.rbegin()->first + 1;
    entry.accountNumber = current;
    accounts.insert_or_assign(entry.accountNumber, std::move(entry));
    return current;
}
//======================================================================
auto Account::delAccount(const std::string &username) -> bool {
    auto lname = util::lower(username);
    auto iter = std::find_if(accounts.begin(), accounts.end(),
                             [lname](const std::pair<account::acctnum_t, AccountEntry> &entry) {
                                 return entry.second.username == lname;
                             });
    return delAccount(iter);
}

//======================================================================
auto Account::delAccount(account::acctnum_t accountNumber) -> bool {
    auto iter = accounts.find(accountNumber);
    return delAccount(iter);
}

//======================================================================
auto Account::importAccounts() -> std::uint16_t {
    auto rvalue = std::uint16_t(0);
    auto path = accountDirectory / std::filesystem::path("newaccounts.adm");
    auto input = std::ifstream(path.string());
    if (input.is_open()) {
        auto buffer = std::vector<char>(4096, 0);
        while (input.good() && !input.eof()) {
            input.getline(buffer.data(), buffer.size() - 1);
            if (input.gcount() > 0) {
                buffer[input.gcount()] = 0;
                std::string line = buffer.data();
                line = util::trim(util::strip(line, "//"));
                if (!line.empty()) {
                    auto [key, rest] = util::split(line, "=");
                    key = util::lower(key);
                    if (key == "user") {
                        auto values = util::parse(rest, ",");
                        if (!values.empty()) {
                            if (!exists(values[0])) {
                                // Doesn't exist, lets import it
                                auto account = AccountEntry();
                                account.username = util::lower(values[0]);
                                switch (values.size()) {
                                default:
                                case 4:
                                    account.contact = values[3];
                                    [[fallthrough]];
                                case 3:
                                    account.flag = std::bitset<AccountEntry::AttributeFlag::COUNT>(
                                        std::stoul(values[2], nullptr, 0));
                                    [[fallthrough]];
                                case 2:
                                    account.password = values[1];
                                    [[fallthrough]];
                                case 1:
                                case 0:
                                    break;
                                }
                                auto current = accounts.rbegin()->first + 1;
                                account.accountNumber = current;
                                accounts.insert_or_assign(current, account);
                                rvalue++;
                            }
                        }
                    }
                }
            }
        }
        if (rvalue > 0) {
            this->save();
        }
        input.close();
        writeImportHeader(path);
    }
    return rvalue;
}
//======================================================================
auto Account::clear() -> void { return accounts.clear(); }

//======================================================================
auto Account::size() const -> size_t { return accounts.size(); }
//======================================================================
auto Account::allAccounts() const -> const std::map<account::acctnum_t, AccountEntry> & {
    return accounts;
}
//======================================================================
auto Account::allAccounts() -> std::map<account::acctnum_t, AccountEntry> & { return accounts; }
