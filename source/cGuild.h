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

// ----- Dynamic Ranks (data-driven) -----
struct RankDef
{
    std::string name;   // e.g. "Recruit", "Member", "Officer"
    SI32        prio;   // promotion order (lower = lower rank)
    UI32        flags;  // optional permissions bitmask
};

// Stable rank storage:
//  - ranks: append-only; index == rankId (STABLE)
//  - rankOf: char serial -> rankId
//  - orderByPrio: rankIds sorted by prio (for Promote/Demote)
//  - idToOrderIndex: rankId -> index in orderByPrio
std::vector<RankDef>                 ranks;
std::unordered_map<SERIAL, size_t>   rankOf;
std::vector<size_t>                  orderByPrio;
std::vector<size_t>                  idToOrderIndex;

// Rebuild the promotion order after ranks change
void RebuildRankOrder_();

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

//o------------------------------------------------------------------------------------------------o
//|  RANK REGISTRY (data-driven ranks)
//o------------------------------------------------------------------------------------------------o

//o------------------------------------------------------------------------------------------------o
//| Function    -  AddRank()
//| Prototype   -  size_t AddRank( const std::string& name, SI32 prio, UI32 flags = 0 )
//| Purpose     -  Add new rank or update existing one by name. Returns stable rankId.
//o------------------------------------------------------------------------------------------------o
size_t AddRank( const std::string& name, SI32 prio, UI32 flags = 0 );

//o------------------------------------------------------------------------------------------------o
//| Function    -  RemoveRankByName()
//| Prototype   -  bool RemoveRankByName( const std::string& name )
//| Purpose     -  Remove a rank definition by name. Fails if any member holds this rank.
//| Notes       -  Implementation keeps rankId space stable by tombstoning (not reindexing).
//o------------------------------------------------------------------------------------------------o
bool RemoveRankByName( const std::string& name );

//o------------------------------------------------------------------------------------------------o
//| Function    -  RenameRank()
//| Prototype   -  bool RenameRank( const std::string& oldName, const std::string& newName )
//| Purpose     -  Rename a rank; ID, prio and flags remain unchanged.
//o------------------------------------------------------------------------------------------------o
bool RenameRank( const std::string& oldName, const std::string& newName );

//o------------------------------------------------------------------------------------------------o
//| Function    -  FindRankIdByName()
//| Prototype   -  SI32 FindRankIdByName( const std::string& name ) const
//| Purpose     -  Get rankId for a rank name (case-insensitive). Returns -1 if not found.
//o------------------------------------------------------------------------------------------------o
SI32 FindRankIdByName( const std::string& name ) const;
SI32 GetRankId(SERIAL s) const;

//o------------------------------------------------------------------------------------------------o
//| Function    -  NumRanks()/GetRankDef()
//| Prototype   -  size_t NumRanks() const
//|               const RankDef* GetRankDef( size_t rankId ) const
//| Purpose     -  Introspection of rank registry.
//o------------------------------------------------------------------------------------------------o
size_t NumRanks() const;
const RankDef* GetRankDef( size_t rankId ) const;

//o------------------------------------------------------------------------------------------------o
//|  MEMBER <-> RANK ASSIGNMENT
//o------------------------------------------------------------------------------------------------o

//o------------------------------------------------------------------------------------------------o
//| Function    -  SetRank()/HasRank()
//| Prototype   -  bool SetRank( CChar& c, const std::string& rankName )
//|               bool SetRank( SERIAL s, const std::string& rankName )
//|               bool HasRank( CChar& c, const std::string& rankName ) const
//|               bool HasRank( SERIAL s, const std::string& rankName ) const
//| Purpose     -  Assign/check a character’s rank by name.
//o------------------------------------------------------------------------------------------------o
bool SetRank( CChar& c, const std::string& rankName );
bool SetRank( SERIAL s, const std::string& rankName );
bool HasRank( CChar& c, const std::string& rankName ) const;
bool HasRank( SERIAL s, const std::string& rankName ) const;

//o------------------------------------------------------------------------------------------------o
//| Function    -  GetRankName()
//| Prototype   -  const std::string GetRankName( CChar& c ) const
//|               const std::string GetRankName( SERIAL s ) const
//| Purpose     -  Fetch current rank name for a character; empty if none.
//o------------------------------------------------------------------------------------------------o
const std::string GetRankName( CChar& c ) const;
const std::string GetRankName( SERIAL s ) const;

//o------------------------------------------------------------------------------------------------o
//| Function    -  Promote()/Demote()
//| Prototype   -  bool Promote( CChar& c )
//|               bool Demote( CChar& c )
//| Purpose     -  Change rank by the next higher/lower priority rank.
//o------------------------------------------------------------------------------------------------o
bool Promote( CChar& c );
bool Demote( CChar& c );

//o------------------------------------------------------------------------------------------------o
//| Function    -  CountInRank()/ListRank()
//| Prototype   -  size_t CountInRank( const std::string& rankName ) const
//|               void   ListRank( const std::string& rankName, std::vector<SERIAL>& out ) const
//| Purpose     -  Query membership for a given rank.
//o------------------------------------------------------------------------------------------------o
size_t CountInRank( const std::string& rankName ) const;
void   ListRank( const std::string& rankName, std::vector<SERIAL>& out ) const;
// ----- Rank accessors (safe to expose to JS) -----
SI32   GetRankPrioById(size_t id) const;      // returns INT_MAX if out of range/deleted
std::string GetRankNameById(size_t id) const; // returns "" if out of range/deleted

// Optional helpers used by your JS:
bool   RemoveRankById(size_t id);             // tombstones if not in use
bool   SetRankById(SERIAL s, size_t id);      // assign by id

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

