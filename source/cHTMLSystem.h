// cHTMLSystem.h: Schnittstelle für die Klasse cHTMLSystem.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHTMLSYSTEM_H__5B019431_A3D8_4968_8527_0789FDDB5A1D__INCLUDED_)
#define AFX_CHTMLSYSTEM_H__5B019431_A3D8_4968_8527_0789FDDB5A1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "uox3.h"
#include "ssection.h"

enum ETemplateType {
	ETT_PERIODIC = 1,
	ETT_OFFLINE,
	ETT_GUILD,
	ETT_PLAYER
};

class cHTMLTemplate
{
protected:
	UI32	UpdateTimer;
	char	InputFile[MAX_PATH];
	bool	Loaded;
	UI08	Type;
	string	Content;
	char	OutputFile[MAX_PATH];
	string	Name;
	UI32	ScheduledUpdate;

public:
	cHTMLTemplate();
	virtual ~cHTMLTemplate();
	void Process( void );
	void Poll( bool Force = false );
	void LoadTemplate( void );
	void UnloadTemplate( void );
	void Load( ScriptSection *found );

	// Some Getters
	string	GetName( void );
	string	GetOutput( void );
	string	GetInput( void );
	UI08	GetTemplateType( void );	
	UI32	GetScheduledUpdate( void );
	UI32	GetUpdateTimer( void );
};

class cHTMLTemplates
{
protected:
	vector < cHTMLTemplate* > Templates;

public:
	cHTMLTemplates();
	virtual ~cHTMLTemplates();

	void Load( void );
	void Unload( void );
	void Poll( UI08 TemplateType );
	bool Refresh( UI32 TemplateID );
	void TemplateInfoGump( cSocket *mySocket );
};

#endif // !defined(AFX_CHTMLSYSTEM_H__5B019431_A3D8_4968_8527_0789FDDB5A1D__INCLUDED_)
