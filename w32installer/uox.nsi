!include "MUI.nsh"

!define MUI_PRODUCT "Ultima Offline eXperiment 3"
!define MUI_VERSION "0.97.06 Build 1o"

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Modern UI Configuration

  !define MUI_ABORTWARNING
  !define MUI_COMPONENTSPAGE_SMALLDESC
  !define MUI_FINISHPAGE_RUN "notepad"
  !define MUI_FINISHPAGE_RUN_PARAMETERS "$INSTDIR\uox.ini"
  !define MUI_FINISHPAGE_LINK "UOX3 Sourceforge page"
  !define MUI_FINISHPAGE_LINK_LOCATION "http://www.sourceforge.net/projects/uox3"
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Data
  
  LicenseData "docs\GNU General Public License.rtf"

; Installer Info
Caption "UOX3 Installer"
CRCCheck on
SetCompressor bzip2
SetDatablockOptimize on

; Icons
Icon "source\uox3.ico"
UninstallIcon "source\uox3.ico"

; File to write
OutFile "UOX30960100.exe"

; Colors (Hex format)
InstallColors FFFFFF 000000
InstProgressFlags smooth colored

; Installation Types
InstType Full
InstType Standard
InstType Minimal

; Enabled and disabled buttons
; EnabledBitmap ""
; DisabledBitmap ""

; Default install Dir
InstallDir $PROGRAMFILES\UOX3
InstallDirRegKey HKLM SOFTWARE\UOX3 "Install_Dir"

; Toggle allowing user to specify install dir
DirShow "show"

AutoCloseWindow "false"
ShowInstDetails "show"

; Primary install files
Section ""
  SetOverwrite "ifnewer"

  ; Set output path
  SetOutPath $INSTDIR

  ; Write files
  File "*.*"

  ; Write the Install path to registry
  WriteRegStr HKLM SOFTWARE\UOX3 "Install_Dir" "$INSTDIR"

  ; Write the Uninstall Data to registry
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UOX3" "UOX3" "Ultima Offline eXperiment 3 (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UOX3" "UninstallString" '$INSTDIR\uninst-UOX3.exe'

  WriteUninstaller "uninst-UOX3.exe"
SectionEnd

; Help installed stuff
Section "UOX3 Help (Recommended)"
  SectionIn 1,2
  File /nonfatal /r "help" 
  File /r "docs"
SectionEnd

; Scripts install files
Section "UOX3 Scripts (Recommended)"
  ; Set type of install
  SectionIn 1,2

  SetOverwrite "ifnewer"

  ; Set output path
  File /r "js"
  File /r "dfndata"
  File /r "dictionaries"

SectionEnd

Section "Other Data (Recommended)"
  SectionIn 1,2
  File /nonfatal /r "accounts"
  File /nonfatal /r "archives"
  File /nonfatal /r "books"
  File /nonfatal /r "html"
  File /nonfatal /r "logs"
  File /nonfatal /r "msgboards"
  File /nonfatal /r "shared"

SectionEnd

; Shortcuts (Optional)
Section "Start Menu Shortcuts"
  ; Set type of install
  SectionIn 1,2

  CreateDirectory "$SMPROGRAMS\UOX3"
  SetOutPath "$INSTDIR"
  CreateShortCut "$SMPROGRAMS\UOX3\UOX3.lnk" "$INSTDIR\UOX.exe" "" "$INSTDIR\UOX.exe" 0
  SetOutPath "$SMPROGRAMS\UOX3"
  CreateShortCut "$SMPROGRAMS\UOX3\Uninstall.lnk" "$INSTDIR\uninst-uox3.exe" "" "$INSTDIR\uninst-uox3.exe" 0
  CreateShortCut "$SMPROGRAMS\UOX3\Readme.lnk" "notepad" "$INSTDIR\help\readme.txt"
  CreateShortCut "$SMPROGRAMS\UOX3\Script Changes.lnk" "notepad" "$INSTDIR\help\scriptchanges.txt"
  CreateShortCut "$SMPROGRAMS\UOX3\Program Updates.lnk" "notepad" "$INSTDIR\help\updates.txt"

SectionEnd

; Quicklaunch Icon (Optional)
Section "Quicklaunch Icon"
  ; Set type of install
  SectionIn 1

  CreateShortCut "$QUICKLAUNCH\UOX3.lnk" "$INSTDIR\UOX.exe" "" "$INSTDIR\UOX.exe" 0
SectionEnd

Section "Uninstall"
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UOX3"
  DeleteRegKey HKLM SOFTWARE\UOX3

  ; Remove files
  ; Remove shortcuts (if any)
  Delete "$SMPROGRAMS\UOX3\*.*"
  Delete "$QUICKLAUNCH\UOX3.lnk"

  ; Remove directories used.
  RMDir /r "$SMPROGRAMS\UOX3"
  RMDir /r "$INSTDIR"
SectionEnd