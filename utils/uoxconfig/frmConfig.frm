VERSION 5.00
Begin VB.Form frmConfig 
   BackColor       =   &H00000000&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "UOX3 Config"
   ClientHeight    =   3330
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   5295
   ForeColor       =   &H00FFFFFF&
   Icon            =   "frmConfig.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3330
   ScaleWidth      =   5295
   StartUpPosition =   2  'CenterScreen
   Begin VB.CheckBox chkUpdatepath 
      BackColor       =   &H00000000&
      ForeColor       =   &H00FFFFFF&
      Height          =   255
      Left            =   1740
      TabIndex        =   7
      Top             =   2100
      Value           =   1  'Checked
      Width           =   2115
   End
   Begin VB.CheckBox cmdSave 
      BackColor       =   &H00000000&
      Caption         =   "Save"
      ForeColor       =   &H00FFFFFF&
      Height          =   375
      Left            =   1740
      Style           =   1  'Graphical
      TabIndex        =   5
      Top             =   2760
      Width           =   1815
   End
   Begin VB.TextBox txtShardname 
      BackColor       =   &H00000000&
      ForeColor       =   &H00FFFFFF&
      Height          =   375
      Left            =   1740
      TabIndex        =   0
      Text            =   "Unnamed"
      Top             =   1140
      Width           =   3315
   End
   Begin VB.TextBox txtPort 
      BackColor       =   &H00000000&
      ForeColor       =   &H00FFFFFF&
      Height          =   375
      Left            =   4200
      TabIndex        =   2
      Text            =   "2593"
      Top             =   1620
      Width           =   855
   End
   Begin VB.TextBox txtIP 
      BackColor       =   &H00000000&
      ForeColor       =   &H00FFFFFF&
      Height          =   375
      Left            =   1740
      TabIndex        =   1
      Text            =   "127.0.0.1"
      Top             =   1620
      Width           =   2355
   End
   Begin VB.Label Label5 
      BackStyle       =   0  'Transparent
      Caption         =   "Update filepath:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   315
      Left            =   240
      TabIndex        =   6
      Top             =   2100
      Width           =   1455
   End
   Begin VB.Label Label4 
      BackStyle       =   0  'Transparent
      Caption         =   "Shardname:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   315
      Left            =   240
      TabIndex        =   4
      Top             =   1140
      Width           =   1335
   End
   Begin VB.Label Label1 
      BackStyle       =   0  'Transparent
      Caption         =   "Internal ip/port:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   315
      Left            =   240
      TabIndex        =   3
      Top             =   1620
      Width           =   1395
   End
   Begin VB.Image Image1 
      Height          =   780
      Left            =   120
      Picture         =   "frmConfig.frx":030A
      Top             =   120
      Width           =   2010
   End
End
Attribute VB_Name = "frmConfig"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

' ================================================================================
' UOX3 Configuration tool
' By Mr. Fixit
' ================================================================================

Option Explicit

' Registery stuff
Private Declare Function RegOpenKey Lib "advapi32.dll" Alias "RegOpenKeyA" (ByVal hKey As Long, ByVal lpSubKey As String, phkResult As Long) As Long
Private Declare Function RegQueryValueEx Lib "advapi32.dll" Alias "RegQueryValueExA" (ByVal hKey As Long, ByVal lpValueName As String, ByVal lpReserved As Long, lpType As Long, lpData As Any, lpcbData As Long) As Long
Private Declare Function RegCloseKey Lib "advapi32.dll" (ByVal hKey As Long) As Long
Private Const HKEY_LOCAL_MACHINE = &H80000002

' Winsock stuff
Private Const WS_VERSION_REQD = &H101
Private Const WS_VERSION_MAJOR = WS_VERSION_REQD \ &H100 And &HFF&
Private Const WS_VERSION_MINOR = WS_VERSION_REQD And &HFF&
Private Const MIN_SOCKETS_REQD = 1
Private Const SOCKET_ERROR = -1
Private Const WSADescription_Len = 256
Private Const WSASYS_Status_Len = 128
Private Type HOSTENT
    hName As Long
    hAliases As Long
    hAddrType As Integer
    hLength As Integer
    hAddrList As Long
End Type
Private Type WSADATA
    wversion As Integer
    wHighVersion As Integer
    szDescription(0 To WSADescription_Len) As Byte
    szSystemStatus(0 To WSASYS_Status_Len) As Byte
    iMaxSockets As Integer
    iMaxUdpDg As Integer
    lpszVendorInfo As Long
End Type
Private Declare Function WSAGetLastError Lib "WSOCK32.DLL" () As Long
Private Declare Function WSAStartup Lib "WSOCK32.DLL" (ByVal wVersionRequired&, lpWSAData As WSADATA) As Long
Private Declare Function WSACleanup Lib "WSOCK32.DLL" () As Long
Private Declare Function gethostname Lib "WSOCK32.DLL" (ByVal hostname$, ByVal HostLen As Long) As Long
Private Declare Function gethostbyname Lib "WSOCK32.DLL" (ByVal hostname$) As Long
Private Declare Sub RtlMoveMemory Lib "kernel32" (hpvDest As Any, ByVal hpvSource&, ByVal cbCopy&)



' --------------------------------------------------------------------------
' Finds the pathway to the uo directory
' --------------------------------------------------------------------------
Private Function GetUOPath() As String
    
    ' Variables
    Dim keyhand&
    Dim datatype&
    Dim lResult As Long
    Dim lValueType As Long
    Dim lDataBufSize As Long
    Dim strBuf As String
    
    ' Open value
    lResult = RegOpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\Origin Worlds Online\Ultima Online\1.0", keyhand&)
    
    ' Get value stats
    lResult = RegQueryValueEx(keyhand&, "ExePath", 0&, lValueType, ByVal 0&, lDataBufSize)

    ' If error, put in default value
    If lResult <> 0 Then
       GetUOPath = ""
       Exit Function
    End If

    ' Set up buffer and get the value
    strBuf = String(lDataBufSize, " ")
    lResult = RegQueryValueEx(keyhand&, "ExePath", 0&, 0&, ByVal strBuf, lDataBufSize)
    
    ' Remove terminators
    GetUOPath = Replace(StripTerminator(strBuf), "client.exe", "")

    ' Return value
    lResult = RegCloseKey(keyhand&)


End Function


' --------------------------------------------------------------------------
' Strips away some junk
' --------------------------------------------------------------------------
Private Function StripTerminator(sInput As String) As String
    Dim ZeroPos As Integer
    ZeroPos = InStr(1, sInput, vbNullChar)
    If ZeroPos > 0 Then
        StripTerminator = Left$(sInput, ZeroPos - 1)
    Else
        StripTerminator = sInput
    End If
End Function


' --------------------------------------------------------------------------
' Makes the directories if they dont exist
' --------------------------------------------------------------------------
Private Function MakeDirectory(Directory As String) As String
   
   ' Variables
   Dim ShortDir As String
   
   ' Return value
   MakeDirectory = Directory
   
   ' Cut down the \
   ShortDir = Left(Directory, Len(Directory) - 1)
   
   ' Ignore errors
   On Error Resume Next
   
   ' Make directory
   MkDir ShortDir

End Function


' --------------------------------------------------------------------------
' Saves the settings
' --------------------------------------------------------------------------
Private Sub cmdSave_Click()

   ' Variables
   Dim sRead As String
   Dim lRead As Long
   Dim sWrite As String
   Dim lWrite As Long
   Dim sServer As String
   Dim AppPath As String
   Dim sDirectory As String
   Dim sDataDirectory As String
   Dim sDefsDirectory As String
   Dim sBooksDirectory As String
   Dim sActsDirectory As String
   Dim sScriptsDirectory As String
   Dim sBackupDirectory As String
   Dim sMsgboardDirectory As String
   Dim sSharedDirectory As String
   
   ' Trap errors
   On Error GoTo errTrap
   
   ' Keep sub from running two times
   If cmdSave.Tag = "STOP" Then
      cmdSave.Tag = ""
      Exit Sub
   End If
   cmdSave.Tag = "STOP"
   
   ' Set button to upright postion again
   cmdSave.Value = 0
   
   ' Chack if file exist
   If Dir("uox.ini") = "" Then
      MsgBox "Cannot find uox.ini. Are you sure you run this app from the UOX3 directory?", vbInformation, "Saving information"
      Exit Sub
   End If
   
   ' Create the serverline
   sServer = "SERVERLIST=" & Replace(txtShardname.Text, ",", "") & "," & txtIP.Text & "," & txtPort.Text
   
   ' Create the directories
   AppPath = App.Path
   If (Right(AppPath, 1) <> "\") Then AppPath = AppPath & "\"
   
   ' Save shardname
   SaveSetting "Mr. Fixit", "UOX3 Config", "shardname", txtShardname.Text & ""
   
   ' Open existing file
   lRead = FreeFile
   Open "uox.ini" For Input As #lRead
   
   ' Open new file
   lWrite = FreeFile
   Open "uox.new" For Output As #lWrite
   
   Do
   
      ' Quit if end of file
      If EOF(lRead) Then Exit Do
      Line Input #lRead, sRead
      
      ' Check for serverlist post
      sWrite = sRead
      If InStr(1, sWrite, "SERVERLIST=") Then
         sWrite = sServer
         sServer = ""
      End If
      
      ' Update paths
      If chkUpdatepath Then
         sDirectory = "DIRECTORY=" & AppPath
         sDataDirectory = "DATADIRECTORY=" & GetUOPath
         sDefsDirectory = "DEFSDIRECTORY=" & MakeDirectory(AppPath & "dfndata\")
         sBooksDirectory = "BOOKSDIRECTORY=" & MakeDirectory(AppPath & "books\")
         sActsDirectory = "ACTSDIRECTORY=" & MakeDirectory(AppPath & "accounts\")
         sScriptsDirectory = "SCRIPTSDIRECTORY=" & MakeDirectory(AppPath & "scripts\")
         sBackupDirectory = "BACKUPDIRECTORY=" & MakeDirectory(AppPath & "archives\")
         sMsgboardDirectory = "MSGBOARDDIRECTORY=" & MakeDirectory(AppPath & "msgboards\")
         sSharedDirectory = "SHAREDDIRECTORY=" & MakeDirectory(AppPath & "shared\")
         If Left(sWrite, Len("DIRECTORY=")) = "DIRECTORY=" Then sWrite = sDirectory
         If Left(sWrite, Len("DATADIRECTORY=")) = "DATADIRECTORY=" Then sWrite = sDataDirectory
         If Left(sWrite, Len("DEFSDIRECTORY=")) = "DEFSDIRECTORY=" Then sWrite = sDefsDirectory
         If Left(sWrite, Len("BOOKSDIRECTORY=")) = "BOOKSDIRECTORY=" Then sWrite = sBooksDirectory
         If Left(sWrite, Len("ACTSDIRECTORY=")) = "ACTSDIRECTORY=" Then sWrite = sActsDirectory
         If Left(sWrite, Len("SCRIPTSDIRECTORY=")) = "SCRIPTSDIRECTORY=" Then sWrite = sScriptsDirectory
         If Left(sWrite, Len("BACKUPDIRECTORY=")) = "BACKUPDIRECTORY=" Then sWrite = sBackupDirectory
         If Left(sWrite, Len("MSGBOARDDIRECTORY=")) = "MSGBOARDDIRECTORY=" Then sWrite = sMsgboardDirectory
         If Left(sWrite, Len("SHAREDDIRECTORY=")) = "SHAREDDIRECTORY=" Then sWrite = sSharedDirectory
      End If
      
      ' Save changes
      Print #lWrite, sWrite
      
   Loop
   
   ' Close files
   Close #lWrite
   Close #lRead
   
   ' Delete old backup
   If Dir(AppPath & "uox.bak") <> "" Then Kill "uox.bak"
   
   ' Switch files
   Name AppPath & "uox.ini" As AppPath & "uox.bak"
   Name AppPath & "uox.new" As AppPath & "uox.ini"
   
   ' Show confirmation
   MsgBox "Settings saved!", vbInformation, "Saving information"

   ' Trap errors
   Exit Sub
errTrap:
   MsgBox "An error accured when saving the settings:" & vbCrLf & vbCrLf _
      & "Description: " & Err.Description & vbCrLf, vbInformation, "Error while saving information"

End Sub


' --------------------------------------------------------------------------
' Finds your ip
' --------------------------------------------------------------------------
Private Function GetIP() As String

   ' Variables
   Dim hostname As String * 256
   Dim hostent_addr As Long
   Dim host As HOSTENT
   Dim hostip_addr As Long
   Dim temp_ip_address() As Byte
   Dim i As Integer
   Dim ip_address As String
   Dim IP As String

   ' Defaults to loopback
   GetIP = "127.0.0.1"

   ' Winsock initalized?
   If gethostname(hostname, 256) = SOCKET_ERROR Then
      Exit Function
   End If
   
   ' Get addr
   hostname = Trim$(hostname)
   hostent_addr = gethostbyname(hostname)
   If hostent_addr = 0 Then
       Exit Function
   End If
    
   ' Copy memory
   RtlMoveMemory host, hostent_addr, LenB(host)
   RtlMoveMemory hostip_addr, host.hAddrList, 4
   
   ' Loop thrught addresses
   Do
      ip_address = ""
      ReDim temp_ip_address(1 To host.hLength)
      RtlMoveMemory temp_ip_address(1), hostip_addr, host.hLength
      For i = 1 To host.hLength
         ip_address = ip_address & temp_ip_address(i) & "."
      Next
      ip_address = Mid$(ip_address, 1, Len(ip_address) - 1)
      host.hAddrList = host.hAddrList + LenB(host.hAddrList)
      RtlMoveMemory hostip_addr, host.hAddrList, 4
   Loop While (hostip_addr <> 0)
    
   ' Save address
   GetIP = ip_address
   
End Function


' --------------------------------------------------------------------------
' Initializes winsock
' --------------------------------------------------------------------------
Private Sub SocketsInitialize()

   ' Variables
   Dim WSAD As WSADATA
   Dim iReturn As Integer
   Dim sLowByte As String, sHighByte As String, sMsg As String
   
   ' Startup winsock
   iReturn = WSAStartup(WS_VERSION_REQD, WSAD)
   If iReturn <> 0 Then
      Exit Sub
   End If
   
End Sub


' --------------------------------------------------------------------------
' Get past the VB limitations
' --------------------------------------------------------------------------
Function hibyte(ByVal wParam As Integer)
    hibyte = wParam \ &H100 And &HFF&
End Function
Function lobyte(ByVal wParam As Integer)
    lobyte = wParam And &HFF&
End Function


' --------------------------------------------------------------------------
' Set up some default values
' --------------------------------------------------------------------------
Private Sub Form_Load()

   ' Get IP
   SocketsInitialize
   txtIP.Text = GetIP

   ' Get shardname
   txtShardname.Text = GetSetting("Mr. Fixit", "UOX3 Config", "shardname", "UOX3 Shard")

End Sub
