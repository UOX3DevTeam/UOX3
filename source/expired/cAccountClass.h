// o------------------------------------------------------------------------------------------------o
//|	File		-	cAccountClass.h
//|	Date		-	12/6/2002 4:33:44 AM
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	Header file for the cAccountClass class. See xRFC0004.txt for more
// details.
// o------------------------------------------------------------------------------------------------o
#ifndef __CACCOUNTCLASS_H__
#define __CACCOUNTCLASS_H__

#include "typedefs.h"
#include <iosfwd>

// Enums
enum CAccountBlock_Errors { AB_INVALID_ID = 0xFFFF };

enum CAccountBlock_Flags {
    AB_FLAGS_BANNED = 0,
    AB_FLAGS_SUSPENDED = 1,
    AB_FLAGS_PUBLIC = 2,
    AB_FLAGS_ONLINE = 3,
    AB_FLAGS_CHARACTER1 = 4,
    AB_FLAGS_CHARACTER2 = 5,
    AB_FLAGS_CHARACTER3 = 6,
    AB_FLAGS_CHARACTER4 = 7,
    AB_FLAGS_CHARACTER5 = 8,
    AB_FLAGS_CHARACTER6 = 9,
    AB_FLAGS_CHARACTER7 = 10,
    AB_FLAGS_YOUNG = 11,
    AB_FLAGS_UNUSED10 = 12,
    AB_FLAGS_SEER = 13,
    AB_FLAGS_COUNSELOR = 14,
    AB_FLAGS_GM = 15,
    AB_FLAGS_ALL = 16
};

// o------------------------------------------------------------------------------------------------o
//|	Struct		-	CAccountBlock_st
//|	Date		-	12/6/2002 5:46:10 AM
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-	All the data has been moved to once again stored in the
//|						accounts.adm file. So changes to this typedef were
// needed
//|						to reflect the members being moved back, and the
// inclusion |						of PATH, and FLAGS to cover the storage
// locations.
//|
//|						sUsername: username for this account
//|						sPassword: password for this account
//|						aPath    : path to where the username.uad file will
//be stored |						sContact : typically the email address for
//this account |						wAccountIndex: which block is this
// accounts. |						wFlags   : flags controlling the allowed
//accounts functions
//|						dwLastIP : ip address of the last connection from
//this
// account |						dwCharacters[5]: straight ID number for this
// account |						lpCharacters[5]:
// o------------------------------------------------------------------------------------------------o
typedef struct CAccountBlock_st {
#if _NOACTCOPY_
  private:
    CAccountBlock_st::CAccountBlock_st(const CAccountBlock_st &);
    CAccountBlock_st &CAccountBlock_st::operator=(const CAccountBlock_st &);
#endif
  public:
    CAccountBlock_st(void)
        : sUsername(""), sPassword(""), sPath(""), sContact(""), wAccountIndex(0xFFFF),
          wTimeBan(0x0000), wFirstLogin(0x0000), dwInGame(INVALIDSERIAL), dwLastIP(0x00000000),
          bChanged(false), dwLastClientVer(0), dwLastClientType(0), dwLastClientVerShort(0),
          wTotalPlayTime(0x00000000) {
        for (std::uint8_t i = 0; i < 7; ++i) {
            dwCharacters[i] = 0xFFFFFFFF;
            lpCharacters[i] = nullptr;
        }
    }
    void reset(void) {
        sUsername = "";
        sPassword = "";
        sContact = "";
        sPath = "";
        wAccountIndex = 0xFFFF;
        wTimeBan = 0x00000000;
        wFirstLogin = 0x00000000;
        wTotalPlayTime = 0x00000000;
        dwInGame = 0xFFFFFFFF;
        dwLastIP = 0x00000000;
        bChanged = false;
        for (std::uint8_t i = 0; i < 7; ++i) {
            dwCharacters[i] = 0xFFFFFFFF;
            lpCharacters[i] = nullptr;
        }
        wFlags.reset();
    }
    std::string sUsername;
    std::string sPassword;
    std::string sPath;
    std::string sContact;
    std::uint16_t wAccountIndex;
    std::bitset<AB_FLAGS_ALL> wFlags;
    std::uint32_t wTimeBan;
    std::uint32_t wFirstLogin;
    std::uint32_t dwInGame;
    std::uint32_t dwLastIP;
    bool bChanged;
    std::uint32_t dwLastClientVer;
    std::uint8_t dwLastClientType;
    std::uint8_t dwLastClientVerShort;
    std::uint32_t wTotalPlayTime;
    std::uint32_t dwCharacters[7];
    CChar *lpCharacters[7];
} CAccountBlock_st;

// Class typdefs to help simplify the use of map STL
typedef std::map<std::string, CAccountBlock_st *> MAPUSERNAME;
typedef std::map<std::uint16_t, CAccountBlock_st> MAPUSERNAMEID;
typedef std::map<std::string, CAccountBlock_st *>::iterator MAPUSERNAME_ITERATOR;
typedef std::map<std::string, CAccountBlock_st *>::const_iterator MAPUSERNAME_CITERATOR;
typedef std::map<std::uint16_t, CAccountBlock_st>::iterator MAPUSERNAMEID_ITERATOR;
typedef std::map<std::uint16_t, CAccountBlock_st>::const_iterator MAPUSERNAMEID_CITERATOR;
// o------------------------------------------------------------------------------------------------o
//|	Class		-	class cAccountClass
//|	Date		-	12/6/2002 5:46:02 AM
// o------------------------------------------------------------------------------------------------o
//|	Purpose		-
// o------------------------------------------------------------------------------------------------o
class cAccountClass {
  public:
    // Construction/Destruction
    cAccountClass();
    cAccountClass(const std::string &sAccountsPath);
    ~cAccountClass();
    // Operator overloads
    cAccountClass &operator++();
    cAccountClass &operator--(std::int32_t);
    std::uint16_t CreateAccountSystem(void);
    std::uint16_t ImportAccounts(void);
    void WriteAccountSection(CAccountBlock_st &actbTemp, std::fstream &fsOut);
    std::uint16_t AddAccount(std::string sUsername, std::string sPassword,
                    const std::string &sContact = "NONE", std::uint16_t wAttributes = 0x0000);
    bool DelAccount(std::string sUsername);
    bool DelAccount(std::uint16_t wAccountId);
    bool SetPath(const std::string &sPath);
    std::string GetPath(void);
    std::uint16_t Save(bool bForceLoad = false);
    std::uint16_t Load(void);
    std::uint32_t size(void);
    bool clear(void);
    bool IsUser(std::string sUsername);
    bool AddCharacter(std::uint16_t wAccountId, CChar *lpObject);
    bool AddCharacter(std::uint16_t wAccountId, std::uint32_t dwCharacterId, CChar *lpObject);
    std::int8_t DelCharacter(std::uint16_t wAccountId, std::uint8_t nSlot);
    bool TransCharacter(std::uint16_t wSAccountId, std::uint16_t wSSlot, std::uint16_t wDAccountId);
    CAccountBlock_st &GetAccountByName(std::string sUsername);
    CAccountBlock_st &GetAccountById(std::uint16_t wAccountId);
    MAPUSERNAMEID_ITERATOR &Begin(void);
    MAPUSERNAMEID_ITERATOR &End(void);
    MAPUSERNAMEID_ITERATOR &Last(void);
    // Member variables
    MAPUSERNAMEID_ITERATOR I;

  private:
    // Member Functions
    void WriteAccountsHeader(std::fstream &fsOut);
    void WriteAccessHeader(std::fstream &fsOut);
    void WriteOrphanHeader(std::fstream &fsOut);
    void WriteUADHeader(std::fstream &fsOut, CAccountBlock_st &actbTemp);
    void WriteImportHeader(std::fstream &fsOut);

    CAccountBlock_st actbInvalid;
    MAPUSERNAME m_mapUsernameMap;
    MAPUSERNAMEID m_mapUsernameIdMap;
    std::uint16_t m_wHighestAccount;
    std::string m_sAccountsDirectory;
};

extern cAccountClass *Accounts;

#endif // __CACCOUNTCLASS_H__
