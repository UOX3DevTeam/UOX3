#ifndef __SPEECH_H__
#define __SPEECH_H__

#include <cstdint>
#include <string>
#include <vector>

#include "typedefs.h"

enum SpeechTarget {
    SPTRG_NULL = -1,
    SPTRG_INDIVIDUAL = 0, // aimed at individual person
    SPTRG_PCS,            // all PCs in range
    SPTRG_PCNPC,          // all NPCs and PCs in range
    SPTRG_BROADCASTPC,    // ALL PCs everywhere + NPCs in range
    SPTRG_BROADCASTALL,
    SPTRG_ONLYRECEIVER // only the receiver
};

enum SpeakerType { SPK_NULL = -1, SPK_SYSTEM = 0, SPK_CHARACTER, SPK_ITEM, SPK_UNKNOWN };

const std::string DistinctLanguageNames[DL_COUNT] = {"ZRO", "ENG", "GER", "SPA", "FRE",
    "PTG", "ITA", "CSY", "POL"};

const std::string LanguageCodes[TOTAL_LANGUAGES] = {
    "ZRO", // default server language
    "ARA", // Arabic Saudi Arabia
    "ARI", // Arabic Iraq
    "ARE", // Arabic Egypt
    "ARL", // Arabic Libya
    "ARG", // Arabic Algeria
    "ARM", // Arabic Morocco
    "ART", // Arabic Tunisia
    "ARO", // Arabic Oman
    "ARY", // Arabic Yemen
    "ARS", // Arabic Syria
    "ARJ", // Arabic Jordan
    "ARB", // Arabic Lebanon
    "ARK", // Arabic Kuwait
    "ARU", // Arabic U.A.E.
    "ARH", // Arabic Bahrain
    "ARQ", // Arabic Qatar
    "BGR", // Bulgarian Bulgaria
    "CAT", // Catalan Spain
    "CHT", // Chinese Taiwan
    "CHS", // Chinese PRC
    "ZHH", // Chinese Hong Kong
    "ZHI", // Chinese Singapore
    "ZHM", // Chinese Macau
    "CSY", // Czech Czech Republic
    "DAN", // Danish Denmark
    "DEU", // German Germany
    "DES", // German Switzerland
    "DEA", // German Austria
    "DEL", // German Luxembourg
    "DEC", // German Liechtenstein
    "ELL", // Greek Greece
    "ENU", // English United States
    "ENG", // English United Kingdom
    "ENA", // English Australia
    "ENC", // English Canada
    "ENZ", // English New Zealand
    "ENI", // English Ireland
    "ENS", // English South Africa
    "ENJ", // English Jamaica
    "ENB", // English Caribbean
    "ENL", // English Belize
    "ENT", // English Trinidad
    "ENW", // English Zimbabwe
    "ENP", // English Philippines
    "ESP", // Spanish Spain (Traditional Sort)
    "ESM", // Spanish Mexico
    "ESN", // Spanish Spain (International Sort)
    "ESG", // Spanish Guatemala
    "ESC", // Spanish Costa Rica
    "ESA", // Spanish Panama
    "ESD", // Spanish Dominican Republic
    "ESV", // Spanish Venezuela
    "ESO", // Spanish Colombia
    "ESR", // Spanish Peru
    "ESS", // Spanish Argentina
    "ESF", // Spanish Ecuador
    "ESL", // Spanish Chile
    "ESY", // Spanish Uruguay
    "ESZ", // Spanish Paraguay
    "ESB", // Spanish Bolivia
    "ESE", // Spanish El Salvador
    "ESH", // Spanish Honduras
    "ESI", // Spanish Nicaragua
    "ESU", // Spanish Puerto Rico
    "FIN", // Finnish Finland
    "FRA", // French France
    "FRB", // French Belgium
    "FRC", // French Canada
    "FRS", // French Switzerland
    "FRL", // French Luxembourg
    "FRM", // French Monaco
    "HEB", // Hebrew Israel
    "HUN", // Hungarian Hungary
    "ISL", // Icelandic Iceland
    "ITA", // Italian Italy
    "ITS", // Italian Switzerland
    "JPN", // Japanese Japan
    "KOR", // Korean (Extended Wansung) Korea
    "NLD", // Dutch Netherlands
    "NLB", // Dutch Belgium
    "NOR", // Norwegian Norway (Bokm�l)
    "NON", // Norwegian Norway (Nynorsk)
    "PLK", // Polish Poland
    "PTB", // Portuguese Brazil
    "PTG", // Portuguese Portugal
    "ROM", // Romanian Romania
    "RUS", // Russian Russia
    "HRV", // Croatian Croatia
    "SRL", // Serbian Serbia (Latin)
    "SRB", // Serbian Serbia (Cyrillic)
    "SKY", // Slovak Slovakia
    "SQI", // Albanian Albania
    "SVE", // Swedish Sweden
    "SVF", // Swedish Finland
    "THA", // Thai Thailand
    "TRK", // Turkish Turkey
    "URP", // Urdu Pakistan
    "IND", // Indonesian Indonesia
    "UKR", // Ukrainian Ukraine
    "BEL", // Belarusian Belarus
    "SLV", // Slovene Slovenia
    "ETI", // Estonian Estonia
    "LVI", // Latvian Latvia
    "LTH", // Lithuanian Lithuania
    "LTC", // Classic Lithuanian Lithuania
    "FAR", // Farsi Iran
    "VIT", // Vietnamese Viet Nam
    "HYE", // Armenian Armenia
    "AZE", // Azeri Azerbaijan (Latin)
    "EUQ", // Basque Spain
    "MKI", // Macedonian Macedonia
    "AFK", // Afrikaans South Africa
    "KAT", // Georgian Georgia
    "FOS", // Faeroese Faeroe Islands
    "HIN", // Hindi India
    "MSL", // Malay Malaysia
    "MSB", // Malay Brunei Darussalam
    "KAZ", // Kazak Kazakstan
    "SWK", // Swahili Kenya
    "UZB", // Uzbek Uzbekistan (Cyrillic)
    "TAT", // Tatar Tatarstan
    "BEN", // Bengali India
    "PAN", // Punjabi India
    "GUJ", // Gujarati India
    "ORI", // Oriya India
    "TAM", // Tamil India
    "TEL", // Telugu India
    "KAN", // Kannada India
    "MAL", // Malayalam India
    "ASM", // Assamese India
    "MAR", // Marathi India
    "SAN", // Sanskrit India
    "KOK"  // Konkani India
};

const distinctlanguage_t LanguageCodesLang[TOTAL_LANGUAGES] = {
    DL_DEFAULT,    // default server language
    DL_DEFAULT,    // Arabic Saudi Arabia
    DL_DEFAULT,    // Arabic Iraq
    DL_DEFAULT,    // Arabic Egypt
    DL_DEFAULT,    // Arabic Libya
    DL_DEFAULT,    // Arabic Algeria
    DL_DEFAULT,    // Arabic Morocco
    DL_DEFAULT,    // Arabic Tunisia
    DL_DEFAULT,    // Arabic Oman
    DL_DEFAULT,    // Arabic Yemen
    DL_DEFAULT,    // Arabic Syria
    DL_DEFAULT,    // Arabic Jordan
    DL_DEFAULT,    // Arabic Lebanon
    DL_DEFAULT,    // Arabic Kuwait
    DL_DEFAULT,    // Arabic U.A.E.
    DL_DEFAULT,    // Arabic Bahrain
    DL_DEFAULT,    // Arabic Qatar
    DL_DEFAULT,    // Bulgarian Bulgaria
    DL_DEFAULT,    // Catalan Spain
    DL_DEFAULT,    // Chinese Taiwan
    DL_DEFAULT,    // Chinese PRC
    DL_DEFAULT,    // Chinese Hong Kong
    DL_DEFAULT,    // Chinese Singapore
    DL_DEFAULT,    // Chinese Macau
    DL_CZECH,      // Czech Czech Republic
    DL_DEFAULT,    // Danish Denmark
    DL_GERMAN,     // German Germany
    DL_GERMAN,     // German Switzerland
    DL_GERMAN,     // German Austria
    DL_GERMAN,     // German Luxembourg
    DL_GERMAN,     // German Liechtenstein
    DL_DEFAULT,    // Greek Greece
    DL_ENGLISH,    // English United States
    DL_ENGLISH,    // English United Kingdom
    DL_ENGLISH,    // English Australia
    DL_ENGLISH,    // English Canada
    DL_ENGLISH,    // English New Zealand
    DL_ENGLISH,    // English Ireland
    DL_ENGLISH,    // English South Africa
    DL_ENGLISH,    // English Jamaica
    DL_ENGLISH,    // English Caribbean
    DL_ENGLISH,    // English Belize
    DL_ENGLISH,    // English Trinidad
    DL_ENGLISH,    // English Zimbabwe
    DL_ENGLISH,    // English Philippines
    DL_SPANISH,    // Spanish Spain (Traditional Sort)
    DL_SPANISH,    // Spanish Mexico
    DL_SPANISH,    // Spanish Spain (International Sort)
    DL_SPANISH,    // Spanish Guatemala
    DL_SPANISH,    // Spanish Costa Rica
    DL_SPANISH,    // Spanish Panama
    DL_SPANISH,    // Spanish Dominican Republic
    DL_SPANISH,    // Spanish Venezuela
    DL_SPANISH,    // Spanish Colombia
    DL_SPANISH,    // Spanish Peru
    DL_SPANISH,    // Spanish Argentina
    DL_SPANISH,    // Spanish Ecuador
    DL_SPANISH,    // Spanish Chile
    DL_SPANISH,    // Spanish Uruguay
    DL_SPANISH,    // Spanish Paraguay
    DL_SPANISH,    // Spanish Bolivia
    DL_SPANISH,    // Spanish El Salvador
    DL_SPANISH,    // Spanish Honduras
    DL_SPANISH,    // Spanish Nicaragua
    DL_SPANISH,    // Spanish Puerto Rico
    DL_DEFAULT,    // Finnish Finland
    DL_FRENCH,     // French France
    DL_FRENCH,     // French Belgium
    DL_FRENCH,     // French Canada
    DL_FRENCH,     // French Switzerland
    DL_FRENCH,     // French Luxembourg
    DL_FRENCH,     // French Monaco
    DL_DEFAULT,    // Hebrew Israel
    DL_DEFAULT,    // Hungarian Hungary
    DL_DEFAULT,    // Icelandic Iceland
    DL_ITALIAN,    // Italian Italy
    DL_ITALIAN,    // Italian Switzerland
    DL_DEFAULT,    // Japanese Japan
    DL_DEFAULT,    // Korean (Extended Wansung) Korea
    DL_DEFAULT,    // Dutch Netherlands
    DL_DEFAULT,    // Dutch Belgium
    DL_DEFAULT,    // Norwegian Norway (Bokm�l)
    DL_DEFAULT,    // Norwegian Norway (Nynorsk)
    DL_POLISH,     // Polish Poland
    DL_PORTUGUESE, // Portuguese Brazil
    DL_PORTUGUESE, // Portuguese Portugal
    DL_DEFAULT,    // Romanian Romania
    DL_DEFAULT,    // Russian Russia
    DL_DEFAULT,    // Croatian Croatia
    DL_DEFAULT,    // Serbian Serbia (Latin)
    DL_DEFAULT,    // Serbian Serbia (Cyrillic)
    DL_DEFAULT,    // Slovak Slovakia
    DL_DEFAULT,    // Albanian Albania
    DL_DEFAULT,    // Swedish Sweden
    DL_DEFAULT,    // Swedish Finland
    DL_DEFAULT,    // Thai Thailand
    DL_DEFAULT,    // Turkish Turkey
    DL_DEFAULT,    // Urdu Pakistan
    DL_DEFAULT,    // Indonesian Indonesia
    DL_DEFAULT,    // Ukrainian Ukraine
    DL_DEFAULT,    // Belarusian Belarus
    DL_DEFAULT,    // Slovene Slovenia
    DL_DEFAULT,    // Estonian Estonia
    DL_DEFAULT,    // Latvian Latvia
    DL_DEFAULT,    // Lithuanian Lithuania
    DL_DEFAULT,    // Classic Lithuanian Lithuania
    DL_DEFAULT,    // Farsi Iran
    DL_DEFAULT,    // Vietnamese Viet Nam
    DL_DEFAULT,    // Armenian Armenia
    DL_DEFAULT,    // Azeri Azerbaijan (Latin)
    DL_DEFAULT,    // Basque Spain
    DL_DEFAULT,    // Macedonian Macedonia
    DL_DEFAULT,    // Afrikaans South Africa
    DL_DEFAULT,    // Georgian Georgia
    DL_DEFAULT,    // Faeroese Faeroe Islands
    DL_DEFAULT,    // Hindi India
    DL_DEFAULT,    // Malay Malaysia
    DL_DEFAULT,    // Malay Brunei Darussalam
    DL_DEFAULT,    // Kazak Kazakstan
    DL_DEFAULT,    // Swahili Kenya
    DL_DEFAULT,    // Uzbek Uzbekistan (Cyrillic)
    DL_DEFAULT,    // Tatar Tatarstan
    DL_DEFAULT,    // Bengali India
    DL_DEFAULT,    // Punjabi India
    DL_DEFAULT,    // Gujarati India
    DL_DEFAULT,    // Oriya India
    DL_DEFAULT,    // Tamil India
    DL_DEFAULT,    // Telugu India
    DL_DEFAULT,    // Kannada India
    DL_DEFAULT,    // Malayalam India
    DL_DEFAULT,    // Assamese India
    DL_DEFAULT,    // Marathi India
    DL_DEFAULT,    // Sanskrit India
    DL_DEFAULT     // Konkani India
};

#define MAX_SPEECH 255

class CSpeechEntry {
private:
    speechtype_t typeOfSpeech;
    bool antiSpam;
    bool unicode;
    colour_t speechColour;
    serial_t speaker;
    serial_t spokenTo; // Only in a case of SPTRG_INDIVIDUAL
    SpeechTarget targType;
    fonttype_t targFont;
    unicodetypes_t targLanguage;
    std::int32_t timeToSayAt; // time when it should be said
    std::string toSay;
    std::string sName;
    std::uint8_t minCmdLevelToReceive;
    
public:
    CSpeechEntry()
    : typeOfSpeech(TALK), antiSpam(true), unicode(true), speechColour(0),
    speaker(INVALIDSERIAL), spokenTo(INVALIDSERIAL), targType(SPTRG_PCS),
    targFont(FNT_NORMAL), targLanguage(UT_ENU), timeToSayAt(-1), minCmdLevelToReceive(0) {
        toSay.reserve(MAX_SPEECH);
        sName.reserve(MAX_NAME);
    }
    ~CSpeechEntry() {}
    
    speechtype_t Type() const { return typeOfSpeech; }
    bool AntiSpam() const { return antiSpam; }
    bool Unicode() const { return unicode; }
    colour_t Colour() const { return speechColour; }
    serial_t Speaker() const { return speaker; }
    serial_t SpokenTo() const { return spokenTo; }
    SpeechTarget TargType() const { return targType; }
    fonttype_t Font() const { return targFont; }
    unicodetypes_t Language() const { return targLanguage; }
    std::int32_t At() const { return timeToSayAt; }
    const std::string Speech() const { return toSay; }
    const std::string SpeakerName() const { return sName; }
    std::uint8_t CmdLevel() const { return minCmdLevelToReceive; }
    
    void Type(speechtype_t type) { typeOfSpeech = type; }
    void AntiSpam(bool value) { antiSpam = value; }
    void Unicode(bool value) { unicode = value; }
    void Colour(colour_t value) { speechColour = value; }
    void Speaker(serial_t value) { speaker = value; }
    void SpokenTo(serial_t value) { spokenTo = value; }
    void TargType(SpeechTarget type) { targType = type; }
    void Font(fonttype_t type) { targFont = type; }
    void Language(unicodetypes_t val) { targLanguage = val; }
    void At(std::int32_t newTime) { timeToSayAt = newTime; }
    void CmdLevel(std::uint8_t nLevel) { minCmdLevelToReceive = nLevel; }
    
    void Speech(const std::string &said) { toSay = said.substr(0, MAX_SPEECH - 1); }
    void SpeakerName(const std::string &spkName) { sName = spkName.substr(0, MAX_NAME - 1); }
    
    SpeakerType SpkrType() const {
        if (speaker == INVALIDSERIAL) {
            return SPK_SYSTEM;
        }
        else if (speaker < BASEITEMSERIAL) {
            return SPK_CHARACTER;
        }
        else {
            return SPK_ITEM;
        }
    }
};


class CSpeechQueue {
private:
    std::int32_t pollTime; // MILLISECONDS How often to poll the queue
    std::vector<CSpeechEntry *> speechList;
    bool runAsThread;
    
    void SayIt(CSpeechEntry &toSay);
    bool InternalPoll();
    
public:
    CSpeechQueue();
    ~CSpeechQueue();
    auto startup() -> void;
    bool poll();         // Send out any pending speech, returning true if entries were sent
    CSpeechEntry &Add(); // Make space in queue, and return pointer to new entry
    std::int32_t PollTime() const;
    void PollTime(std::int32_t value);
    void RunAsThread(bool newValue);
    bool RunAsThread() const;
    void DumpInFile();
};

extern CSpeechQueue *SpeechSys;

#endif
