#ifndef __CGUILD__
#define __CGUILD__

#include <string>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

const string GTypeNames[GT_COUNT] = { "Standard", "Order", "Chaos", "Unknown" };
const string GRelationNames[GR_COUNT] = { "Neutral", "War", "Ally", "Unknown", "Same" };
const SI16 BasePage = 8000;

typedef map< GuildID, GuildRelation >	GUILDREL;
typedef vector< SERIAL > SERLIST;

class CGuild
{
protected:
	string		name;
	char		abbreviation[4];
	GuildType	gType;
	string		charter;
	string		webpage;
	SERIAL		stone;
	SERIAL		master;
	SERLIST		recruits;
	SERLIST		members;
	GUILDREL	relationList;

	SERLIST::iterator	recruitPtr;
	SERLIST::iterator	memberPtr;

	GUILDREL::iterator	warPtr;
	GUILDREL::iterator	allyPtr;
public:

	GuildID		FirstWar( void );
	GuildID		NextWar( void );
	bool		FinishedWar( void );

	GuildID		FirstAlly( void );
	GuildID		NextAlly( void );
	bool		FinishedAlly( void );

				CGuild();
	virtual		~CGuild();
	const char *Name( void ) const;
	const char *Abbreviation( void ) const;
	GuildType	Type( void ) const;
	const char *Charter( void ) const;
	const char *Webpage( void ) const;
	SERIAL		Stone( void ) const;
	SERIAL		Master( void ) const;
	SERIAL		FirstRecruit( void );
	SERIAL		NextRecruit( void );
	bool		FinishedRecruits( void );
	SERIAL		RecruitNumber( UI32 rNum ) const;
	SERIAL		FirstMember( void );
	SERIAL		NextMember( void );
	bool		FinishedMember( void );
	SERIAL		MemberNumber( UI32 rNum ) const;

	void		Name( const char *txt );
	void		Abbreviation( const char *txt );
	void		Type( GuildType nType );
	void		Charter( const char *txt );
	void		Webpage( const char *txt );
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

	int			NumMembers( void ) const;
	int			NumRecruits( void ) const;

	GuildRelation	RelatedToGuild( GuildID otherGuild ) const;
	bool		IsAtWar( GuildID otherGuild ) const;
	bool		IsNeutral( GuildID otherGuild ) const;
	bool		IsAlly( GuildID otherGuild ) const;

	void		SetGuildRelation( GuildID otherGuild, GuildRelation toSet );
	void		Save( ofstream &toSave, GuildID gNum );
	void		Load( ScriptSection *toRead );

	GUILDREL *	GuildRelationList( void );	// NOTE: This is aimed ONLY at menu stuff

	void		CalcMaster( void );

	void		TellMembers( char *txt, ... );
	void		TellMembers( SI32 dictEntry, ... );

	const char *TypeName( void );

	bool		IsRecruit( SERIAL toCheck ) const;
	bool		IsMember( SERIAL toCheck ) const;
	bool		IsRecruit( CChar &toCheck ) const;
	bool		IsMember( CChar &toCheck ) const;
};

typedef map< GuildID, CGuild * > GUILDLIST;
class CGuildCollection
{
protected:
	GUILDLIST		gList;

	void			ToggleAbbreviation( cSocket *s );
	void			Erase( GuildID toErase );
	GuildID			MaximumGuild( void );
public:
	void			Resign( cSocket *s );
					CGuildCollection();
	UI32			NumGuilds( void ) const;
	GuildID			NewGuild( void );
	CGuild *		Guild( GuildID num ) const;
	CGuild *		operator[]( GuildID num );
	void			Save( void );
	void			Load( void );
	GuildRelation	Compare( GuildID srcGuild, GuildID trgGuild ) const;
	GuildRelation	Compare( CChar *src, CChar *trg ) const;
	void			Menu( cSocket *s, SI16 menu, GuildID trgGuild = -1, SERIAL plID = INVALIDSERIAL );
	void			GumpInput( cSocket *s );
	void			GumpChoice( cSocket *s );
	void			PlaceStone( cSocket *s, CItem *deed );
	bool			ResultInCriminal( GuildID srcGuild, GuildID trgGuild ) const;
	bool			ResultInCriminal( CChar *src, CChar *trg ) const;
	void			DisplayTitle( cSocket *s, CChar *src ) const;
	virtual			~CGuildCollection();
};
#endif

