#ifndef __CGUILD__
#define __CGUILD__

class CPIGumpInput;

enum GuildType
{
	GT_STANDARD = 0,
	GT_ORDER,
	GT_CHAOS,
	GT_UNKNOWN,
	GT_COUNT
};

enum GUILDRELATION
{
	GR_NEUTRAL = 0,
	GR_WAR,
	GR_ALLY,
	GR_UNKNOWN,
	GR_SAME,
	GR_COUNT
};

const std::string GTypeNames[GT_COUNT] = { "Standard", "Order", "Chaos", "Unknown" };
const std::string GRelationNames[GR_COUNT] = { "Neutral", "War", "Ally", "Unknown", "Same" };
const SI16 BasePage = 8000;

typedef std::map< GUILDID, GUILDRELATION >	GUILDREL;
typedef std::map< GUILDID, GUILDRELATION >::iterator GUILDREL_ITERATOR;
typedef std::map< GUILDID, GUILDRELATION >::iterator GUILDREL_ITERATOR;

class CGuild
{
private:
	std::string		name;
	char			abbreviation[4];
	GuildType		gType;
	std::string		charter;
	std::string		webpage;
	SERIAL			stone;
	SERIAL			master;
	SERLIST			recruits;
	SERLIST			members;
	GUILDREL		relationList;

	SERLIST_ITERATOR	recruitPtr;
	SERLIST_ITERATOR	memberPtr;

	GUILDREL_ITERATOR	warPtr;
	GUILDREL_ITERATOR	allyPtr;
public:

	GUILDID		FirstWar( void );
	GUILDID		NextWar( void );
	bool		FinishedWar( void );

	GUILDID		FirstAlly( void );
	GUILDID		NextAlly( void );
	bool		FinishedAlly( void );

	CGuild();
	~CGuild();
	const std::string	Name( void ) const;
	const char *		Abbreviation( void ) const;
	GuildType			Type( void ) const;
	const std::string	Charter( void ) const;
	const std::string	Webpage( void ) const;
	SERIAL		Stone( void ) const;
	SERIAL		Master( void ) const;
	SERIAL		FirstRecruit( void );
	SERIAL		NextRecruit( void );
	bool		FinishedRecruits( void );
	SERIAL		RecruitNumber( size_t rNum ) const;
	SERIAL		FirstMember( void );
	SERIAL		NextMember( void );
	bool		FinishedMember( void );
	SERIAL		MemberNumber( size_t rNum ) const;

	void		Name( std::string txt );
	void		Abbreviation( const char *txt );
	void		Type( GuildType nType );
	void		Charter( const std::string &txt );
	void		Webpage( const std::string &txt );
	void		Stone( SERIAL newStone );
	void		Stone( CItem &newStone );
	void		Master( SERIAL newMaster );
	void		Master( CChar &newMaster );
	void		NewRecruit( CChar &newRecruit );
	void		NewRecruit( SERIAL newRecruit );
	void		NewMember( CChar &newMember );
	void		NewMember( SERIAL newMember );
	void		RemoveRecruit( CChar &newRecruit );
	void		RemoveRecruit( SERIAL newRecruit );
	void		RemoveMember( CChar &newMember );
	void		RemoveMember( SERIAL newMember );
	void		RecruitToMember( CChar &newMember );
	void		RecruitToMember( SERIAL newMember );

	size_t		NumMembers( void ) const;
	size_t		NumRecruits( void ) const;

	GUILDRELATION	RelatedToGuild( GUILDID otherGuild ) const;
	bool		IsAtWar( GUILDID otherGuild ) const;
	bool		IsNeutral( GUILDID otherGuild ) const;
	bool		IsAlly( GUILDID otherGuild ) const;

	void		SetGuildFaction( GuildType newFaction );
	void		SetGuildRelation( GUILDID otherGuild, GUILDRELATION toSet );
	void		Save( std::ofstream &toSave, GUILDID gNum );
	void		Load( ScriptSection *toRead );

	GUILDREL *	GuildRelationList( void );	// NOTE: This is aimed ONLY at menu stuff

	void		CalcMaster( void );

	void		TellMembers( SI32 dictEntry, ... );

	const std::string TypeName( void );

	bool		IsRecruit( SERIAL toCheck ) const;
	bool		IsMember( SERIAL toCheck ) const;
	bool		IsRecruit( CChar &toCheck ) const;
	bool		IsMember( CChar &toCheck ) const;
};

typedef std::map< GUILDID, CGuild * > GUILDLIST;
class CGuildCollection
{
private:
	GUILDLIST		gList;

	void			ToggleAbbreviation( CSocket *s );
	void			TransportGuildStone( CSocket *s, GUILDID guildID );
	void			Erase( GUILDID toErase );
	GUILDID			MaximumGuild( void );
public:
	void			Resign( CSocket *s );
	CGuildCollection();
	size_t			NumGuilds( void ) const;
	GUILDID			NewGuild( void );
	CGuild *		Guild( GUILDID num ) const;
	CGuild *		operator[]( GUILDID num );
	void			Save( void );
	void			Load( void );
	GUILDRELATION	Compare( GUILDID srcGuild, GUILDID trgGuild ) const;
	GUILDRELATION	Compare( CChar *src, CChar *trg ) const;
	void			Menu( CSocket *s, SI16 menu, GUILDID trgGuild = -1, SERIAL plID = INVALIDSERIAL );
	void			GumpInput( CPIGumpInput *gi );
	void			GumpChoice( CSocket *s );
	void			PlaceStone( CSocket *s, CItem *deed );
	bool			ResultInCriminal( GUILDID srcGuild, GUILDID trgGuild ) const;
	bool			ResultInCriminal( CChar *src, CChar *trg ) const;
	void			DisplayTitle( CSocket *s, CChar *src ) const;
	~CGuildCollection();
};

extern CGuildCollection	*GuildSys;

#endif

