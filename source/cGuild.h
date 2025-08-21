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

typedef std::map<GUILDID, GUILDRELATION>	GUILDREL;
typedef std::map<GUILDID, GUILDRELATION>::iterator GUILDREL_ITERATOR;
typedef std::map<GUILDID, GUILDRELATION>::iterator GUILDREL_ITERATOR;

class CGuild
{
private:
	std::string		name;
	std::string		abbreviation;
	GuildType		gType;
	std::string		charter;
	std::string		webpage;
	SERIAL			stone;
	SERIAL			master;
	std::vector<SERIAL>	recruits;
	std::vector<SERIAL>	members;
	std::vector<SERIAL> veterans;
	std::vector<SERIAL> officers;
	std::vector<SERIAL> invites;
	GUILDREL		relationList;

	std::vector<SERIAL>::iterator	recruitPtr;
	std::vector<SERIAL>::iterator	memberPtr;
	std::vector<SERIAL>::iterator	veteranPtr;
	std::vector<SERIAL>::iterator	officerPtr;
	std::vector<SERIAL>::iterator	invitePtr;

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
	const std::string	Name() const;
	auto Abbreviation() const -> const std::string&;
	GuildType Type() const;
	const std::string	Charter() const;
	const std::string	Webpage() const;
	SERIAL		Stone() const;
	SERIAL		Master() const;
	SERIAL		FirstRecruit();
	SERIAL		NextRecruit();
	bool		FinishedRecruits();
	SERIAL		RecruitNumber( size_t rNum ) const;
	SERIAL		FirstMember();
	SERIAL		NextMember();
	bool		FinishedMember();
	SERIAL		MemberNumber( size_t rNum ) const;

	void		Name( std::string txt );
	auto		Abbreviation( const std::string &value) -> void;
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

	size_t		NumMembers() const;
	size_t		NumRecruits() const;

	SERIAL      FirstVeteran();
	SERIAL      NextVeteran();
	bool        FinishedVeterans();
	SERIAL      VeteranNumber( size_t rNum ) const;
	size_t      NumVeterans() const;

	void        NewVeteran( CChar &newVeteran );
	void        NewVeteran( SERIAL newVeteran );
	void        RemoveVeteran( CChar &newVeteran );
	void        RemoveVeteran( SERIAL newVeteran );
	bool        IsVeteran( CChar &toCheck ) const;
	bool        IsVeteran( SERIAL toCheck ) const;

	SERIAL      FirstOfficer();
	SERIAL      NextOfficer();
	bool        FinishedOfficers();
	SERIAL      OfficerNumber( size_t rNum ) const;
	size_t      NumOfficers() const;

	void        NewOfficer( CChar &newOfficer );
	void        NewOfficer( SERIAL newOfficer );
	void        RemoveOfficer( CChar &newOfficer );
	void        RemoveOfficer( SERIAL newOfficer );
	bool        IsOfficer( CChar &toCheck ) const;
	bool        IsOfficer( SERIAL toCheck ) const;

	GUILDRELATION	RelatedToGuild( GUILDID otherGuild ) const;
	bool		IsAtWar( GUILDID otherGuild ) const;
	bool		IsNeutral( GUILDID otherGuild ) const;
	bool		IsAlly( GUILDID otherGuild ) const;
	bool		IsAtPeace() const;

	void		SetGuildFaction( GuildType newFaction );
	void		SetGuildRelation( GUILDID otherGuild, GUILDRELATION toSet );
	void		Save( std::ostream &toSave, GUILDID gNum );
	void		Load( CScriptSection *toRead );

	GUILDREL *	GuildRelationList();	// NOTE: This is aimed ONLY at menu stuff

	void		CalcMaster();

	void		TellMembers( SI32 dictEntry, ... );

	const std::string TypeName();

	bool		IsRecruit( SERIAL toCheck ) const;
	bool		IsMember( SERIAL toCheck ) const;
	bool		IsRecruit( CChar &toCheck ) const;
	bool		IsMember( CChar &toCheck ) const;

	SERIAL      FirstInvite();
	SERIAL      NextInvite();
	bool        FinishedInvites();
	SERIAL      InviteNumber( size_t rNum ) const;
	size_t      NumInvites() const;

	void        AddInvite( CChar &addinvite );
	void        AddInvite( SERIAL addinvite );
	void        RemoveInvite( CChar &addinvite );
	void        RemoveInvite( SERIAL addinvite );
	bool        IsInvited( CChar &toCheck ) const;
	bool        IsInvited( SERIAL toCheck ) const;
};

typedef std::map< GUILDID, CGuild * > GUILDLIST;
class CGuildCollection
{
private:
	GUILDLIST		gList;

	void			ToggleAbbreviation( CSocket *s );
	void			TransportGuildStone( CSocket *s, GUILDID guildId );
	void			Erase( GUILDID toErase );
	GUILDID			MaximumGuild( void );
public:
	void			Resign( CSocket *s );
	CGuildCollection() = default;
	size_t			NumGuilds( void ) const;
	GUILDID			NewGuild( void );
	GUILDID			FindGuildId( const CGuild * );
	CGuild *		Guild( GUILDID num ) const;
	CGuild *		operator[]( GUILDID num );
	void			Save();
	void			Load();
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

