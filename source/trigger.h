#ifndef __TRIGGER_H__
#define __TRIGGER_H__

namespace UOX
{

	const std::string ScriptNames[SCPT_COUNT] = { "SCRIPT_LIST", "COMMAND_SCRIPTS", "MAGIC_SCRIPTS" };

	class CEnvoke
	{
	private:
		std::map< UI16, UI16 >	envokeList;
		std::string				envokeType;

	public:
				CEnvoke( std::string envokeType );
				~CEnvoke();

		void	Parse( void );
		bool	Check( UI16 envokeID ) const;
		UI16	GetScript( UI16 envokeID ) const;
	};

	class CTriggerScripts
	{
	private:
		std::map< JSObject *, cScript * >			scriptMapping;
		std::map< JSObject *, cScript * >::iterator scriptIterator;

		SCRIPTTYPE									scriptType;

	public:
					CTriggerScripts( SCRIPTTYPE sT );
					~CTriggerScripts();

		void		Parse( void );
		void		RegisterObject( JSObject *toReg, cScript *tNum );
		void		UnregisterObject( JSObject *toUnreg );

		cScript *	GetScript( JSObject *toFind );
		cScript *	First( void );
		cScript *	Next( void );
		bool		Finished( void );
	};

class CTrigger
{
private:
	std::map< UI16, cScript * >					scriptTriggers;
	std::map< JSObject *, UI16 >				scriptToTriggerMapping;

	CEnvoke *			envokeByID;
	CEnvoke *			envokeByType;

	CTriggerScripts *	magicScripts;
	CTriggerScripts *	commandScripts;

	void				ParseScript( void );

public:
						CTrigger();
						~CTrigger();

	void				Cleanup();
	void				ReloadJS( void );

	cScript *			GetScript( UI16 triggerNumber );
	cScript *			GetAssociatedScript( JSObject *toFind );
	UI16				GetScriptNumber( JSObject *toFind );

	void				RegisterObject( JSObject *toReg, UI16 tNum );
	void				UnregisterObject( JSObject *toUnreg );

	CEnvoke *			GetEnvokeByID( void );
	CEnvoke *			GetEnvokeByType( void );

	CTriggerScripts *	GetMagicScripts( void );
	CTriggerScripts *	GetCommandScripts( void );
};

extern CTrigger *Trigger;

}

#endif

