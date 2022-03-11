#ifndef __CJSMAPPING_H__
#define __CJSMAPPING_H__

enum SCRIPTTYPE
{
	SCPT_NORMAL		= 0,
	SCPT_COMMAND,
	SCPT_MAGIC,
	SCPT_SKILLUSE,
	SCPT_PACKET,
	SCPT_CONSOLE,
	SCPT_COUNT
};

const std::string ScriptNames[SCPT_COUNT] = { "SCRIPT_LIST", "COMMAND_SCRIPTS", "MAGIC_SCRIPTS", "SKILLUSE_SCRIPTS", "PACKET_SCRIPTS", "CONSOLE_SCRIPTS" };

class CJSMappingSection
{
private:
	std::map< UI16, cScript * >				scriptIDMap;
	std::map< JSObject *, UI16 >			scriptJSMap;

	std::map< UI16, cScript * >::iterator	scriptIDIter;

	SCRIPTTYPE								scriptType;
public:
	CJSMappingSection( SCRIPTTYPE sT );
	~CJSMappingSection();


	void		Reload( UI16 toLoad );
	void		Parse( Script *fileAssocData );

	bool		IsInMap( UI16 scriptID );

	UI16		GetScriptID( JSObject *toFind );
	cScript *	GetScript( UI16 toFind );
	cScript *	GetScript( JSObject *toFind );

	cScript *	First( void );
	cScript *	Next( void );
	bool		Finished( void );
};

class CJSMapping
{
private:
	CJSMappingSection *				mapSection[SCPT_COUNT];

	CEnvoke *						envokeByID;
	CEnvoke *						envokeByType;

	void				Cleanup( void );
	void				Parse( SCRIPTTYPE toParse = SCPT_COUNT );

public:
	CJSMapping( void );
	~CJSMapping();
	void				ResetDefaults( void );

	void				Reload( UI16 scriptID = 0xFFFF );
	void				Reload( SCRIPTTYPE sectionID );

	CJSMappingSection *	GetSection( SCRIPTTYPE toGet );

	UI16				GetScriptID( JSObject *toFind );

	cScript *			GetScript( UI16 toFind );
	cScript *			GetScript( JSObject *toFind );

	CEnvoke *			GetEnvokeByID( void );
	CEnvoke *			GetEnvokeByType( void );
};

class CEnvoke
{
private:
	std::map< UI16, UI16 >	envokeList;
	std::string				envokeType;

public:
	CEnvoke( const std::string &envokeType );
	~CEnvoke();

	void	Parse( void );
	bool	Check( UI16 envokeID ) const;
	UI16	GetScript( UI16 envokeID ) const;
};

extern CJSMapping *JSMapping;

#endif  // __CJSMAPPING_H__
