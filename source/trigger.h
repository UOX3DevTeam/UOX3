#ifndef __TRIGGER_H__
#define __TRIGGER_H__

struct periodicTrigger
{
	long		nextRun;
	cScript *	trigScript;
	periodicTrigger( cScript *t, long nr ) : trigScript( t ), nextRun( nr ) { }
};

class Triggers
{
private:
	map< UI16, UI16 >			envokeList;		// item ID -> script ID mapping
	map< UI16, cScript * >		scriptTriggers;
	vector< periodicTrigger >	periodicTriggers;
	map< JSObject *, UI16 >		scriptToTriggerMapping;

	virtual void				ParseEnvoke( void );
	virtual void				ParseScript( void );
public:
								Triggers();
			void				Cleanup();
	virtual						~Triggers();
	virtual bool				CheckEnvoke( UI16 itemID ) const;
	virtual cScript *			GetScript( UI16 triggerNumber );
	virtual void				ReloadJS( void );
	virtual void				PeriodicTriggerCheck( void );
	virtual void				PushPeriodicTrigger( cScript *trigScript, SI32 nextRun );
	virtual void				RegisterObject( JSObject *toReg, UI16 tNum );
	virtual void				UnregisterObject( JSObject *toUnreg );
	virtual UI16				FindObject( JSObject *toFind );
	virtual UI16				GetScriptFromEnvoke( UI16 itemID ) const;
};

#endif

