#ifndef __TRIGGER_H__
#define __TRIGGER_H__

namespace UOX
{

class Triggers
{
private:

	std::map< UI16, UI16 >						envokeList;		// item ID -> script ID mapping
	std::map< UI16, cScript * >					scriptTriggers;
	std::map< JSObject *, UI16 >				scriptToTriggerMapping;
	std::map< JSObject *, cScript * >			commandScripts;
	std::map< JSObject *, cScript * >			magicScripts;
	std::map< JSObject *, cScript * >::iterator commandScpIterator;
	std::map< JSObject *, cScript * >::iterator magicScpIterator;

	void				ParseEnvoke( void );
	void				ParseScript( void );
	void				ParseCommands( void );
	void				ParseMagic( void );
public:
						Triggers();
	void				Cleanup();
						~Triggers();
	bool				CheckEnvoke( UI16 itemID ) const;
	cScript *			GetScript( UI16 triggerNumber );
	cScript *			GetAssociatedScript( JSObject *toFind );
	void				ReloadJS( void );
	void				RegisterObject( JSObject *toReg, UI16 tNum );
	void				UnregisterObject( JSObject *toUnreg );
	void				RegisterCommandObject( JSObject *toReg, cScript *tNum );
	void				RegisterMagicObject( JSObject *toReg, cScript *tNum );
	void				UnregisterCommandObject( JSObject *toUnreg );
	void				UnregisterMagicObject( JSObject *toUnreg );
	UI16				GetScriptFromEnvoke( UI16 itemID ) const;

	cScript *			FirstCommand( void );
	cScript *			NextCommand( void );
	bool				FinishedCommands( void );

	cScript *			FirstSpell( void );
	cScript *			NextSpell( void );
	bool				FinishedSpells( void );
};

extern Triggers *Trigger;

}

#endif

