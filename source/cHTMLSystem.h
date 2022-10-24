//o-------------------------------------------------------------------------------------------------o
//|	File		-	cHTMLSystem.h
//o-------------------------------------------------------------------------------------------------o
//|	Purpose		-	Interface for the cHTMLSystem class
//o-------------------------------------------------------------------------------------------------o

#ifndef __CHTMLSYSTEM_H__
#define __CHTMLSYSTEM_H__

#if defined(_MSC_VER)
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif

enum ETemplateType
{
	ETT_ALLTEMPLATES = -1,
	ETT_GMSTATUS =0 ,
	ETT_GUILD,
	ETT_OFFLINE,
	ETT_PLAYER,
	ETT_ONLINE,
	ETT_INVALIDTEMPLATE = 0xFF
};

class cHTMLTemplate
{
private:
	UI32			updateTimer;
	std::string		inputFile;
	bool			loaded;
	ETemplateType	type;
	std::string		content;
	std::string		outputFile;
	std::string		name;
	UI32			scheduledUpdate;

public:
	cHTMLTemplate();
	~cHTMLTemplate();
	void			Process( void );
	void			Poll( void );
	void			LoadTemplate( void );
	void			UnloadTemplate( void );
	void			Load( CScriptSection *found );

	// Some Getters
	std::string		GetName( void ) const;
	std::string		GetOutput( void ) const;
	std::string		GetInput( void ) const;
	ETemplateType	GetTemplateType( void ) const;
	UI32			GetScheduledUpdate( void ) const;
	UI32			GetUpdateTimer( void ) const;
};

class cHTMLTemplates
{
private:
	std::vector<cHTMLTemplate*> Templates;

public:
	cHTMLTemplates() = default;
	~cHTMLTemplates();

	void Load( void );
	void Unload( void );
	void Poll( ETemplateType nTemplateId = ETT_ALLTEMPLATES );
	void TemplateInfoGump( CSocket *mySocket );
};

extern cHTMLTemplates	*HTMLTemplates;

#endif
