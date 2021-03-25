#ifndef __SPEECH_H__
#define __SPEECH_H__

enum SpeechTarget
{
	SPTRG_NULL = -1,
	SPTRG_INDIVIDUAL = 0,		// aimed at individual person
	SPTRG_PCS,					// all PCs in range
	SPTRG_PCNPC,				// all NPCs and PCs in range
	SPTRG_BROADCASTPC,				// ALL PCs everywhere + NPCs in range
	SPTRG_BROADCASTALL,
	SPTRG_ONLYRECEIVER			// only the receiver
};

enum SpeakerType
{
	SPK_NULL = -1,
	SPK_SYSTEM = 0,
	SPK_CHARACTER,
	SPK_ITEM,
	SPK_UNKNOWN
};

const std::string DistinctLanguageNames[DL_COUNT] =
{
	"UNK",
	"ZRO",
	"ENG",
	"GER",
	"SPA",
	"FRE",
	"JPN"
};

const std::string LanguageCodes[TOTAL_LANGUAGES] =
{
	"ZRO",	// default server language
	"ARA",	// Arabic Saudi Arabia
	"ARI",	// Arabic Iraq
	"ARE",	// Arabic Egypt
	"ARL",	// Arabic Libya
	"ARG",	// Arabic Algeria
	"ARM",	// Arabic Morocco
	"ART",	// Arabic Tunisia
	"ARO",	// Arabic Oman
	"ARY",	// Arabic Yemen
	"ARS",	// Arabic Syria
	"ARJ",	// Arabic Jordan
	"ARB",	// Arabic Lebanon
	"ARK",	// Arabic Kuwait
	"ARU",	// Arabic U.A.E.
	"ARH",	// Arabic Bahrain
	"ARQ",	// Arabic Qatar
	"BGR",	// Bulgarian Bulgaria
	"CAT",	// Catalan Spain
	"CHT",	// Chinese Taiwan
	"CHS",	// Chinese PRC
	"ZHH",	// Chinese Hong Kong
	"ZHI",	// Chinese Singapore
	"ZHM",	// Chinese Macau
	"CSY",	// Czech Czech Republic
	"DAN",	// Danish Denmark
	"DEU",	// German Germany
	"DES",	// German Switzerland
	"DEA",	// German Austria
	"DEL",	// German Luxembourg
	"DEC",	// German Liechtenstein
	"ELL",	// Greek Greece
	"ENU",	// English United States
	"ENG",	// English United Kingdom
	"ENA",	// English Australia
	"ENC",	// English Canada
	"ENZ",	// English New Zealand
	"ENI",	// English Ireland
	"ENS",	// English South Africa
	"ENJ",	// English Jamaica
	"ENB",	// English Caribbean
	"ENL",	// English Belize
	"ENT",	// English Trinidad
	"ENW",	// English Zimbabwe
	"ENP",	// English Philippines
	"ESP",	// Spanish Spain (Traditional Sort)
	"ESM",	// Spanish Mexico
	"ESN",	// Spanish Spain (International Sort)
	"ESG",	// Spanish Guatemala
	"ESC",	// Spanish Costa Rica
	"ESA",	// Spanish Panama
	"ESD",	// Spanish Dominican Republic
	"ESV",	// Spanish Venezuela
	"ESO",	// Spanish Colombia
	"ESR",	// Spanish Peru
	"ESS",	// Spanish Argentina
	"ESF",	// Spanish Ecuador
	"ESL",	// Spanish Chile
	"ESY",	// Spanish Uruguay
	"ESZ",	// Spanish Paraguay
	"ESB",	// Spanish Bolivia
	"ESE",	// Spanish El Salvador
	"ESH",	// Spanish Honduras
	"ESI",	// Spanish Nicaragua
	"ESU",	// Spanish Puerto Rico
	"FIN",	// Finnish Finland
	"FRA",	// French France
	"FRB",	// French Belgium
	"FRC",	// French Canada
	"FRS",	// French Switzerland
	"FRL",	// French Luxembourg
	"FRM",	// French Monaco
	"HEB",	// Hebrew Israel
	"HUN",	// Hungarian Hungary
	"ISL",	// Icelandic Iceland
	"ITA",	// Italian Italy
	"ITS",	// Italian Switzerland
	"JPN",	// Japanese Japan
	"KOR",	// Korean (Extended Wansung) Korea
	"NLD",	// Dutch Netherlands
	"NLB",	// Dutch Belgium
	"NOR",	// Norwegian Norway (Bokmål)
	"NON",	// Norwegian Norway (Nynorsk)
	"PLK",	// Polish Poland
	"PTB",	// Portuguese Brazil
	"PTG",	// Portuguese Portugal
	"ROM",	// Romanian Romania
	"RUS",	// Russian Russia
	"HRV",	// Croatian Croatia
	"SRL",	// Serbian Serbia (Latin)
	"SRB",	// Serbian Serbia (Cyrillic)
	"SKY",	// Slovak Slovakia
	"SQI",	// Albanian Albania
	"SVE",	// Swedish Sweden
	"SVF",	// Swedish Finland
	"THA",	// Thai Thailand
	"TRK",	// Turkish Turkey
	"URP",	// Urdu Pakistan
	"IND",	// Indonesian Indonesia
	"UKR",	// Ukrainian Ukraine
	"BEL",	// Belarusian Belarus
	"SLV",	// Slovene Slovenia
	"ETI",	// Estonian Estonia
	"LVI",	// Latvian Latvia
	"LTH",	// Lithuanian Lithuania
	"LTC",	// Classic Lithuanian Lithuania
	"FAR",	// Farsi Iran
	"VIT",	// Vietnamese Viet Nam
	"HYE",	// Armenian Armenia
	"AZE",	// Azeri Azerbaijan (Latin)
	"EUQ",	// Basque Spain
	"MKI",	// Macedonian Macedonia
	"AFK",	// Afrikaans South Africa
	"KAT",	// Georgian Georgia
	"FOS",	// Faeroese Faeroe Islands
	"HIN",	// Hindi India
	"MSL",	// Malay Malaysia
	"MSB",	// Malay Brunei Darussalam
	"KAZ",	// Kazak Kazakstan
	"SWK",	// Swahili Kenya
	"UZB",	// Uzbek Uzbekistan (Cyrillic)
	"TAT",	// Tatar Tatarstan
	"BEN",	// Bengali India
	"PAN",	// Punjabi India
	"GUJ",	// Gujarati India
	"ORI",	// Oriya India
	"TAM",	// Tamil India
	"TEL",	// Telugu India
	"KAN",	// Kannada India
	"MAL",	// Malayalam India
	"ASM",	// Assamese India
	"MAR",	// Marathi India
	"SAN",	// Sanskrit India
	"KOK"	// Konkani India
};

const DistinctLanguage LanguageCodesLang[TOTAL_LANGUAGES] =
{
	DL_DEFAULT,	// default server language
	DL_UNKNOWN,	// Arabic Saudi Arabia
	DL_UNKNOWN,	// Arabic Iraq
	DL_UNKNOWN,	// Arabic Egypt
	DL_UNKNOWN,	// Arabic Libya
	DL_UNKNOWN,	// Arabic Algeria
	DL_UNKNOWN,	// Arabic Morocco
	DL_UNKNOWN,	// Arabic Tunisia
	DL_UNKNOWN,	// Arabic Oman
	DL_UNKNOWN,	// Arabic Yemen
	DL_UNKNOWN,	// Arabic Syria
	DL_UNKNOWN,	// Arabic Jordan
	DL_UNKNOWN,	// Arabic Lebanon
	DL_UNKNOWN,	// Arabic Kuwait
	DL_UNKNOWN,	// Arabic U.A.E.
	DL_UNKNOWN,	// Arabic Bahrain
	DL_UNKNOWN,	// Arabic Qatar
	DL_UNKNOWN,	// Bulgarian Bulgaria
	DL_UNKNOWN,	// Catalan Spain
	DL_UNKNOWN,	// Chinese Taiwan
	DL_UNKNOWN,	// Chinese PRC
	DL_UNKNOWN,	// Chinese Hong Kong
	DL_UNKNOWN,	// Chinese Singapore
	DL_UNKNOWN,	// Chinese Macau
	DL_UNKNOWN,	// Czech Czech Republic
	DL_UNKNOWN,	// Danish Denmark
	DL_GERMAN,	// German Germany
	DL_GERMAN,	// German Switzerland
	DL_GERMAN,	// German Austria
	DL_GERMAN,	// German Luxembourg
	DL_GERMAN,	// German Liechtenstein
	DL_UNKNOWN,	// Greek Greece
	DL_ENGLISH,	// English United States
	DL_ENGLISH,	// English United Kingdom
	DL_ENGLISH,	// English Australia
	DL_ENGLISH,	// English Canada
	DL_ENGLISH,	// English New Zealand
	DL_ENGLISH,	// English Ireland
	DL_ENGLISH,	// English South Africa
	DL_ENGLISH,	// English Jamaica
	DL_ENGLISH,	// English Caribbean
	DL_ENGLISH,	// English Belize
	DL_ENGLISH,	// English Trinidad
	DL_ENGLISH,	// English Zimbabwe
	DL_ENGLISH,	// English Philippines
	DL_SPANISH,	// Spanish Spain (Traditional Sort)
	DL_SPANISH,	// Spanish Mexico
	DL_SPANISH,	// Spanish Spain (International Sort)
	DL_SPANISH,	// Spanish Guatemala
	DL_SPANISH,	// Spanish Costa Rica
	DL_SPANISH,	// Spanish Panama
	DL_SPANISH,	// Spanish Dominican Republic
	DL_SPANISH,	// Spanish Venezuela
	DL_SPANISH,	// Spanish Colombia
	DL_SPANISH,	// Spanish Peru
	DL_SPANISH,	// Spanish Argentina
	DL_SPANISH,	// Spanish Ecuador
	DL_SPANISH,	// Spanish Chile
	DL_SPANISH,	// Spanish Uruguay
	DL_SPANISH,	// Spanish Paraguay
	DL_SPANISH,	// Spanish Bolivia
	DL_SPANISH,	// Spanish El Salvador
	DL_SPANISH,	// Spanish Honduras
	DL_SPANISH,	// Spanish Nicaragua
	DL_SPANISH,	// Spanish Puerto Rico
	DL_UNKNOWN,	// Finnish Finland
	DL_FRENCH,	// French France
	DL_FRENCH,	// French Belgium
	DL_FRENCH,	// French Canada
	DL_FRENCH,	// French Switzerland
	DL_FRENCH,	// French Luxembourg
	DL_FRENCH,	// French Monaco
	DL_UNKNOWN,	// Hebrew Israel
	DL_UNKNOWN,	// Hungarian Hungary
	DL_UNKNOWN,	// Icelandic Iceland
	DL_UNKNOWN,	// Italian Italy
	DL_UNKNOWN,	// Italian Switzerland
	DL_JAPANESE,	// Japanese Japan
	DL_UNKNOWN,	// Korean (Extended Wansung) Korea
	DL_UNKNOWN,	// Dutch Netherlands
	DL_UNKNOWN,	// Dutch Belgium
	DL_UNKNOWN,	// Norwegian Norway (Bokmål)
	DL_UNKNOWN,	// Norwegian Norway (Nynorsk)
	DL_UNKNOWN,	// Polish Poland
	DL_UNKNOWN,	// Portuguese Brazil
	DL_UNKNOWN,	// Portuguese Portugal
	DL_UNKNOWN,	// Romanian Romania
	DL_UNKNOWN,	// Russian Russia
	DL_UNKNOWN,	// Croatian Croatia
	DL_UNKNOWN,	// Serbian Serbia (Latin)
	DL_UNKNOWN,	// Serbian Serbia (Cyrillic)
	DL_UNKNOWN,	// Slovak Slovakia
	DL_UNKNOWN,	// Albanian Albania
	DL_UNKNOWN,	// Swedish Sweden
	DL_UNKNOWN,	// Swedish Finland
	DL_UNKNOWN,	// Thai Thailand
	DL_UNKNOWN,	// Turkish Turkey
	DL_UNKNOWN,	// Urdu Pakistan
	DL_UNKNOWN,	// Indonesian Indonesia
	DL_UNKNOWN,	// Ukrainian Ukraine
	DL_UNKNOWN,	// Belarusian Belarus
	DL_UNKNOWN,	// Slovene Slovenia
	DL_UNKNOWN,	// Estonian Estonia
	DL_UNKNOWN,	// Latvian Latvia
	DL_UNKNOWN,	// Lithuanian Lithuania
	DL_UNKNOWN,	// Classic Lithuanian Lithuania
	DL_UNKNOWN,	// Farsi Iran
	DL_UNKNOWN,	// Vietnamese Viet Nam
	DL_UNKNOWN,	// Armenian Armenia
	DL_UNKNOWN,	// Azeri Azerbaijan (Latin)
	DL_UNKNOWN,	// Basque Spain
	DL_UNKNOWN,	// Macedonian Macedonia
	DL_UNKNOWN,	// Afrikaans South Africa
	DL_UNKNOWN,	// Georgian Georgia
	DL_UNKNOWN,	// Faeroese Faeroe Islands
	DL_UNKNOWN,	// Hindi India
	DL_UNKNOWN,	// Malay Malaysia
	DL_UNKNOWN,	// Malay Brunei Darussalam
	DL_UNKNOWN,	// Kazak Kazakstan
	DL_UNKNOWN,	// Swahili Kenya
	DL_UNKNOWN,	// Uzbek Uzbekistan (Cyrillic)
	DL_UNKNOWN,	// Tatar Tatarstan
	DL_UNKNOWN,	// Bengali India
	DL_UNKNOWN,	// Punjabi India
	DL_UNKNOWN,	// Gujarati India
	DL_UNKNOWN,	// Oriya India
	DL_UNKNOWN,	// Tamil India
	DL_UNKNOWN,	// Telugu India
	DL_UNKNOWN,	// Kannada India
	DL_UNKNOWN,	// Malayalam India
	DL_UNKNOWN,	// Assamese India
	DL_UNKNOWN,	// Marathi India
	DL_UNKNOWN,	// Sanskrit India
	DL_UNKNOWN	// Konkani India
};

#define MAX_SPEECH 255

class CSpeechEntry
{
private:
	SpeechType		typeOfSpeech;
	bool			antiSpam;
	bool			unicode;
	COLOUR			speechColour;
	SERIAL			speaker;
	SERIAL			spokenTo;		// Only in a case of SPTRG_INDIVIDUAL
	SpeechTarget	targType;
	FontType		targFont;
	UnicodeTypes	targLanguage;
	SI32			timeToSayAt;	// time when it should be said
	std::string		toSay;
	std::string		sName;
	UI08			minCmdLevelToReceive;

public:
	CSpeechEntry() : typeOfSpeech( TALK ), antiSpam( true ), unicode( true ), speechColour( 0 ), speaker( INVALIDSERIAL ), spokenTo( INVALIDSERIAL ), targType( SPTRG_PCS ), targFont( FNT_NORMAL ), targLanguage( UT_ENU ), timeToSayAt( -1 ), minCmdLevelToReceive( 0 )
	{
		toSay.reserve( MAX_SPEECH );
		sName.reserve( MAX_NAME );
	}
	~CSpeechEntry()
	{
	}

	SpeechType			Type( void ) const				{	return typeOfSpeech;			}
	bool				AntiSpam( void ) const			{	return antiSpam;				}
	bool				Unicode( void ) const			{	return unicode;					}
	COLOUR				Colour( void ) const			{	return speechColour;			}
	SERIAL				Speaker( void ) const			{	return speaker;					}
	SERIAL				SpokenTo( void ) const			{	return spokenTo;				}
	SpeechTarget		TargType( void ) const			{	return targType;				}
	FontType			Font( void ) const				{	return targFont;				}
	UnicodeTypes		Language( void ) const			{	return targLanguage;			}
	SI32				At( void ) const				{	return timeToSayAt;				}
	const std::string	Speech( void ) const			{	return toSay;					}
	const std::string	SpeakerName( void ) const		{	return sName;					}
	UI08				CmdLevel( void ) const			{	return minCmdLevelToReceive;	}

	void				Type( SpeechType type )			{	typeOfSpeech = type;			}
	void				AntiSpam( bool value )			{	antiSpam = value;				}
	void				Unicode( bool value )			{	unicode = value;				}
	void				Colour( COLOUR value )			{	speechColour = value;			}
	void				Speaker( SERIAL value )			{	speaker = value;				}
	void				SpokenTo( SERIAL value )		{	spokenTo = value;				}
	void				TargType( SpeechTarget type )	{	targType = type;				}
	void				Font( FontType type )			{	targFont = type;				}
	void				Language( UnicodeTypes val )	{	targLanguage = val;				}
	void				At( SI32 newTime )				{	timeToSayAt = newTime;			}
	void				CmdLevel( UI08 nLevel )			{	minCmdLevelToReceive = nLevel;	}

	void Speech( const std::string& said )
	{
		toSay = said.substr( 0, MAX_SPEECH - 1 );
	}
	void SpeakerName( const std::string& spkName )
	{
		sName = spkName.substr( 0, MAX_NAME - 1 );
	}

	SpeakerType	SpkrType( void ) const
	{
		if( speaker == INVALIDSERIAL )
			return SPK_SYSTEM;
		else if( speaker < BASEITEMSERIAL )
			return SPK_CHARACTER;
		else
			return SPK_ITEM;
	}
};

typedef std::vector< CSpeechEntry * >	SPEECHLIST;
typedef std::vector< CSpeechEntry * >::iterator SPEECHLIST_ITERATOR;
typedef std::vector< CSpeechEntry * >::reverse_iterator SPEECHLIST_RITERATOR;
typedef std::vector< CSpeechEntry * >::const_iterator SPEECHLIST_CITERATOR;

class CSpeechQueue
{
private:
	SI32				pollTime;		// MILLISECONDS How often to poll the queue
	SPEECHLIST		speechList;
	bool			runAsThread;

	void			SayIt( CSpeechEntry& toSay );
	bool			InternalPoll( void );
public:
	CSpeechQueue( void );
	~CSpeechQueue();

	bool			Poll( void );		// Send out any pending speech, returning true if entries were sent
	CSpeechEntry& 	Add( void );		// Make space in queue, and return pointer to new entry
	SI32			PollTime( void ) const;
	void			PollTime( SI32 value );
	void			RunAsThread( bool newValue );
	bool			RunAsThread( void ) const;
	void			DumpInFile( void );
};

extern CSpeechQueue *SpeechSys;

#endif

