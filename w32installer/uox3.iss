[_ISTool]
EnableISX=true
UseAbsolutePaths=true

[Setup]
Compression=bzip
AppCopyright=© 2002 The UOX3 Development team
AppName=UOX3
AppVerName=Build 0.97.00 Build 5
AdminPrivilegesRequired=true
DefaultGroupName=Ultima Offline eXperiment
DefaultDirName=C:\UOX3
DisableStartupPrompt=false
ExtraDiskSpaceRequired=90000
DirExistsWarning=no
DisableReadyMemo=true
DisableReadyPage=false
Uninstallable=true
OutputDir=C:\Dokumente und Einstellungen\Sebastian Hartte\Desktop\
OutputBaseFilename=uox3-02-23-2002
CreateAppDir=true
DisableProgramGroupPage=false
EnableDirDoesntExistWarning=true
DisableAppendDir=true
AlwaysShowComponentsList=false
ShowComponentSizes=false
FlatComponentsList=false
UsePreviousAppDir=false
CodeFile=C:\Dokumente und Einstellungen\Sebastian Hartte\Desktop\uopath.ifs
InfoBeforeFile=C:\Dokumente und Einstellungen\Sebastian Hartte\Desktop\base\updates.txt
LicenseFile=C:\Dokumente und Einstellungen\Sebastian Hartte\Desktop\base\GNU GENERAL PUBLIC LICENSE.rtf
AppMutex=Ultima Offline eXperiment
AllowNoIcons=true
AlwaysCreateUninstallIcon=true
UninstallIconName=Uninstall UOX3
AppPublisher=The UOX3 Development Team
AppPublisherURL=http://uox3.sourceforge.net
AppSupportURL=http://www.uoxdev.com
AppUpdatesURL=http://www.uoxdev.com
AppVersion=0.96.02
AppID=uox3_uninstall
UninstallDisplayIcon={app}\UOX3.exe
UninstallDisplayName=Uninstall Ultima Offline eXperiment 3

[Files]
Source: base\cluox.exe; DestDir: {app}
Source: base\INSCRIBE.GMP; DestDir: {app}
Source: base\js32.dll; DestDir: {app}
Source: base\readme.txt; DestDir: {app}
Source: base\scriptchanges.txt; DestDir: {app}
Source: base\UOX3.exe; DestDir: {app}
Source: base\uox3.gif; DestDir: {app}
Source: base\uox.ini; DestDir: {app}
Source: base\uox.ini.dist; DestDir: {app}
Source: base\updates.txt; DestDir: {app}
Source: base\accounts\accounts.adm; DestDir: {app}\accounts
Source: base\dfndata\*.*; DestDir: {app}\dfndata; Flags: recursesubdirs
Source: base\js\*.*; DestDir: {app}\js; Flags: recursesubdirs
Source: base\scripts\*.*; DestDir: {app}\scripts; Flags: recursesubdirs
Source: base\docs\*.*; DestDir: {app}\docs

[Dirs]
Name: {app}\dfndata
Name: {app}\scripts
Name: {app}\logs
Name: {app}\js
Name: {app}\accounts
Name: {app}\archives
Name: {app}\books
Name: {app}\msgboards
Name: {app}\shared
Name: {app}\html
Name: {app}\docs

[Registry]

[Run]

[Messages]

[Icons]
Name: {group}\Cloak UOX (Wrapper); Filename: {app}\cluox.exe; WorkingDir: {app}; Comment: A UOX3 watchdog which is able to minimize your UOX3 server to the tray.; IconIndex: 0
Name: {group}\Start UOX3 Server; Filename: {app}\UOX3.exe; Comment: This starts the UOX3 Server; IconIndex: 0; WorkingDir: {app}
Name: {group}\Visit UOXDev.com - Support; Filename: http://www.uoxdev.com; IconFilename: {app}\UOX3.exe; Comment: UOXDev.com - The official UOX3 Support community.; IconIndex: 0
Name: {group}\Visit uox3.sourceforge.net - Development; Filename: http://uox3.sourceforge.net; IconFilename: {app}\UOX3.exe; Comment: This website is for the developers of the UOX3 server platform.; IconIndex: 0

[INI]
Filename: {app}\uox.ini; Section: directories; Key: DIRECTORY; String: {app}

