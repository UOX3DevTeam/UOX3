VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "comdlg32.ocx"
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Object = "{248DD890-BB45-11CF-9ABC-0080C7E7B78D}#1.0#0"; "MSWINSCK.OCX"
Object = "{BDC217C8-ED16-11CD-956C-0000C04E4C0A}#1.1#0"; "TABCTL32.OCX"
Object = "{5E9E78A0-531B-11CF-91F6-C2863C385E30}#1.0#0"; "MSFLXGRD.OCX"
Begin VB.Form frmMain 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "xGMClient"
   ClientHeight    =   5535
   ClientLeft      =   150
   ClientTop       =   720
   ClientWidth     =   8535
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5535
   ScaleWidth      =   8535
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox txtServerProtocolVersion 
      ForeColor       =   &H80000009&
      Height          =   285
      Left            =   3240
      Locked          =   -1  'True
      TabIndex        =   8
      Top             =   120
      Width           =   1575
   End
   Begin TabDlg.SSTab tabMain 
      Height          =   4695
      Left            =   0
      TabIndex        =   3
      Top             =   480
      Width           =   8535
      _ExtentX        =   15055
      _ExtentY        =   8281
      _Version        =   393216
      Style           =   1
      TabHeight       =   520
      TabCaption(0)   =   "Server Stats"
      TabPicture(0)   =   "frmMain.frx":0000
      Tab(0).ControlEnabled=   -1  'True
      Tab(0).Control(0)=   "flxServerStatistics"
      Tab(0).Control(0).Enabled=   0   'False
      Tab(0).Control(1)=   "cmdStatsRefresh"
      Tab(0).Control(1).Enabled=   0   'False
      Tab(0).ControlCount=   2
      TabCaption(1)   =   "Online status"
      TabPicture(1)   =   "frmMain.frx":001C
      Tab(1).ControlEnabled=   0   'False
      Tab(1).Control(0)=   "cmdOnlineRefresh"
      Tab(1).Control(1)=   "tabWho"
      Tab(1).ControlCount=   2
      TabCaption(2)   =   "Other"
      TabPicture(2)   =   "frmMain.frx":0038
      Tab(2).ControlEnabled=   0   'False
      Tab(2).ControlCount=   0
      Begin TabDlg.SSTab tabWho 
         Height          =   3855
         Left            =   -74880
         TabIndex        =   9
         Top             =   720
         Width           =   8295
         _ExtentX        =   14631
         _ExtentY        =   6800
         _Version        =   393216
         Style           =   1
         TabHeight       =   520
         TabCaption(0)   =   "Online"
         TabPicture(0)   =   "frmMain.frx":0054
         Tab(0).ControlEnabled=   -1  'True
         Tab(0).Control(0)=   "flxWhoOnline(0)"
         Tab(0).Control(0).Enabled=   0   'False
         Tab(0).ControlCount=   1
         TabCaption(1)   =   "Offline"
         TabPicture(1)   =   "frmMain.frx":0070
         Tab(1).ControlEnabled=   0   'False
         Tab(1).Control(0)=   "flxWhoOnline(1)"
         Tab(1).ControlCount=   1
         TabCaption(2)   =   "Logging"
         TabPicture(2)   =   "frmMain.frx":008C
         Tab(2).ControlEnabled=   0   'False
         Tab(2).Control(0)=   "flxWhoOnline(2)"
         Tab(2).ControlCount=   1
         Begin MSFlexGridLib.MSFlexGrid flxWhoOnline 
            Height          =   3255
            Index           =   0
            Left            =   120
            TabIndex        =   10
            Top             =   480
            Width           =   8055
            _ExtentX        =   14208
            _ExtentY        =   5741
            _Version        =   393216
            FixedCols       =   0
            ScrollBars      =   2
         End
         Begin MSFlexGridLib.MSFlexGrid flxWhoOnline 
            Height          =   3255
            Index           =   1
            Left            =   -74880
            TabIndex        =   11
            Top             =   480
            Width           =   8055
            _ExtentX        =   14208
            _ExtentY        =   5741
            _Version        =   393216
            FixedCols       =   0
            ScrollBars      =   2
         End
         Begin MSFlexGridLib.MSFlexGrid flxWhoOnline 
            Height          =   3255
            Index           =   2
            Left            =   -74880
            TabIndex        =   12
            Top             =   480
            Width           =   8055
            _ExtentX        =   14208
            _ExtentY        =   5741
            _Version        =   393216
            FixedCols       =   0
            ScrollBars      =   2
         End
      End
      Begin VB.CommandButton cmdOnlineRefresh 
         Caption         =   "Refresh"
         Enabled         =   0   'False
         Height          =   255
         Left            =   -74880
         TabIndex        =   6
         Top             =   360
         Width           =   1695
      End
      Begin VB.CommandButton cmdStatsRefresh 
         Caption         =   "Refresh"
         Enabled         =   0   'False
         Height          =   255
         Left            =   120
         TabIndex        =   5
         Top             =   360
         Width           =   1695
      End
      Begin MSFlexGridLib.MSFlexGrid flxServerStatistics 
         Height          =   3855
         Left            =   120
         TabIndex        =   4
         Top             =   720
         Width           =   8295
         _ExtentX        =   14631
         _ExtentY        =   6800
         _Version        =   393216
         Rows            =   23
         FixedCols       =   0
         ScrollBars      =   2
         FormatString    =   "^Statistic |^Value"
      End
   End
   Begin VB.CommandButton cmdOptions 
      Caption         =   "Options"
      Height          =   375
      Left            =   7320
      TabIndex        =   2
      Top             =   0
      Width           =   1215
   End
   Begin MSWinsockLib.Winsock wsXGM 
      Left            =   6840
      Top             =   0
      _ExtentX        =   741
      _ExtentY        =   741
      _Version        =   393216
      RemotePort      =   6666
   End
   Begin VB.CommandButton cmdXGMConnect 
      Caption         =   "Connect"
      Height          =   375
      Left            =   0
      TabIndex        =   1
      Top             =   0
      Width           =   1335
   End
   Begin MSComctlLib.StatusBar sbStatusBar 
      Align           =   2  'Align Bottom
      Height          =   270
      Left            =   0
      TabIndex        =   0
      Top             =   5265
      Width           =   8535
      _ExtentX        =   15055
      _ExtentY        =   476
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   4
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Object.Width           =   9260
            Text            =   "Status"
            TextSave        =   "Status"
         EndProperty
         BeginProperty Panel2 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   6
            AutoSize        =   2
            Object.Width           =   1773
            MinWidth        =   1764
            TextSave        =   "25/09/2001"
         EndProperty
         BeginProperty Panel3 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   5
            AutoSize        =   2
            Object.Width           =   1773
            MinWidth        =   1764
            TextSave        =   "15:47"
         EndProperty
         BeginProperty Panel4 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Object.Width           =   2117
            MinWidth        =   2117
            Text            =   "Unconnected"
            TextSave        =   "Unconnected"
            Object.Tag             =   "1026"
            Object.ToolTipText     =   "Indicates if it is connected or not"
         EndProperty
      EndProperty
   End
   Begin MSComDlg.CommonDialog dlgCommonDialog 
      Left            =   6360
      Top             =   0
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.Label lblServerVersion 
      BackStyle       =   0  'Transparent
      Caption         =   "Server Protocol Version"
      Height          =   255
      Left            =   1440
      TabIndex        =   7
      Top             =   120
      Width           =   1695
   End
   Begin VB.Menu mnuHelp 
      Caption         =   "1000"
      Begin VB.Menu mnuHelpContents 
         Caption         =   "1001"
      End
      Begin VB.Menu mnuHelpSearchForHelpOn 
         Caption         =   "1002"
      End
      Begin VB.Menu mnuHelpBar0 
         Caption         =   "-"
      End
      Begin VB.Menu mnuHelpAbout 
         Caption         =   "1003"
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Private Declare Function OSWinHelp% Lib "user32" Alias "WinHelpA" (ByVal hwnd&, ByVal HelpFile$, ByVal wCommand%, dwData As Any)
Dim cOnline As Long
Dim cOffline As Long
Dim cLogging As Long
Dim cCurrentTime As Long


Private Sub LoadFlexColumn()

    Dim i As Long
    For i = 1 To 22
        flxServerStatistics.TextMatrix(i, 0) = LoadResString(2000 + i - 1)
    Next i
End Sub
Private Sub UpdateWinsock()

    If Len(appControl.remoteHostIP) > 0 Then
        wsXGM.RemoteHost = appControl.remoteHostIP
    Else
        wsXGM.RemoteHost = appControl.remoteHostName
    End If
    wsXGM.RemotePort = appControl.remoteHostPort

End Sub

Private Sub cmdOnlineRefresh_Click()

    If tabWho.Tab >= 1 And appControl.clearanceLevel < COUNSELOR_AUTH Then
        MsgBox "Insufficient privileges to execute, you must be counselor or better"
        Exit Sub
    End If
    Select Case tabWho.Tab
    Case 0: SendWhoRequest wsXGM, XGM_WHO_ONLINE
    Case 1: SendWhoRequest wsXGM, XGM_WHO_OFFLINE
    Case 2: SendWhoRequest wsXGM, XGM_WHO_LOGGING
    End Select
End Sub

Private Sub cmdOptions_Click()
    Load frmOptions
    frmOptions.Show vbModal, Me
    UpdateWinsock
End Sub
Private Sub DoConnect()

    sbStatusBar.Panels(1).Text = "Connecting to " & wsXGM.RemoteHost & " on port " & wsXGM.RemotePort

    wsXGM.Connect
    While wsXGM.State = 6
        DoEvents
    Wend
    If wsXGM.State <> 7 Then
        sbStatusBar.Panels(4).Text = LoadResString(1026)
        sbStatusBar.Panels(1).Text = "Connection failed"
        Exit Sub
    End If
    sbStatusBar.Panels(1).Text = ""
    sbStatusBar.Panels(4).Text = LoadResString(1027)
    cmdXGMConnect.Caption = "Disconnect"
    BackColor = vbGreen
    SendLoginPacket wsXGM
    UpdateRefreshButtons True
End Sub

Private Sub DoDisconnect()

    wsXGM.Close
    sbStatusBar.Panels(1).Text = ""
    sbStatusBar.Panels(4).Text = LoadResString(1026)
    cmdXGMConnect.Caption = "Connect"
    BackColor = vbRed
    UpdateRefreshButtons False

End Sub

Private Sub cmdStatsRefresh_Click()

    Dim i As Byte
    For i = 0 To 21
        flxServerStatistics.TextMatrix(i + 1, 1) = ""
    Next i
    For i = 0 To 21
        If i = 3 Then
            cCurrentTime = GetCurrentTime
        End If
        SendStatPacket wsXGM, i
    Next i
End Sub

Private Sub cmdXGMConnect_Click()

    If cmdXGMConnect.Caption = "Connect" Then
        DoConnect
    Else
        DoDisconnect
    End If
End Sub

Private Sub UpdateRefreshButtons(status As Boolean)
    cmdStatsRefresh.Enabled = status
    cmdOnlineRefresh.Enabled = status
End Sub
Private Sub Form_Load()
    LoadResStrings Me
    UpdateWinsock
    Dim i As Long, j As Long
    For i = 0 To flxServerStatistics.Cols - 1
        flxServerStatistics.ColWidth(i) = flxServerStatistics.Width \ flxServerStatistics.Cols
    Next i
    For j = 0 To 2
        For i = 0 To flxWhoOnline(j).Cols - 1
            flxWhoOnline(j).ColWidth(i) = flxWhoOnline(j).Width \ flxWhoOnline(j).Cols
        Next i
    Next j
    LoadFlexColumn
End Sub


Private Sub Form_Unload(Cancel As Integer)
    Dim i As Integer

    'close all sub forms
    For i = Forms.Count - 1 To 1 Step -1
        Unload Forms(i)
    Next
    If Me.WindowState <> vbMinimized Then
        SaveSetting App.Title, "Settings", "MainLeft", Me.Left
        SaveSetting App.Title, "Settings", "MainTop", Me.Top
        SaveSetting App.Title, "Settings", "MainWidth", Me.Width
        SaveSetting App.Title, "Settings", "MainHeight", Me.Height
    End If
End Sub

Private Sub mnuHelpAbout_Click()
    frmAbout.Show vbModal, Me
End Sub

Private Sub mnuHelpSearchForHelpOn_Click()
    Dim nRet As Integer

    'if there is no helpfile for this project display a message to the user
    'you can set the HelpFile for your application in the
    'Project Properties dialog
    If Len(App.HelpFile) = 0 Then
        MsgBox "Unable to display Help Contents. There is no Help associated with this project.", vbInformation, Me.Caption
    Else
        On Error Resume Next
        nRet = OSWinHelp(Me.hwnd, App.HelpFile, 261, 0)
        If Err Then
            MsgBox Err.Description
        End If
    End If

End Sub

Private Sub mnuHelpContents_Click()
    Dim nRet As Integer

    'if there is no helpfile for this project display a message to the user
    'you can set the HelpFile for your application in the
    'Project Properties dialog
    If Len(App.HelpFile) = 0 Then
        MsgBox "Unable to display Help Contents. There is no Help associated with this project.", vbInformation, Me.Caption
    Else
        On Error Resume Next
        nRet = OSWinHelp(Me.hwnd, App.HelpFile, 3, 0)
        If Err Then
            MsgBox Err.Description
        End If
    End If

End Sub

Private Sub wsXGM_Close()
    DoDisconnect
End Sub
Private Sub HandleServerWhoResponse(wsXGM As Winsock, bleh() As Byte)
    Dim iNumPlayers As Long, iStatus As Byte, iTotal As Long, backColour As Long
    iNumPlayers = CLng(bleh(1)) * 256 + bleh(2)
    iStatus = bleh(3)
    Select Case iStatus
    Case 0: cOnline = iNumPlayers: backColour = vbGreen
    Case 1: cOffline = iNumPlayers: backColour = vbRed
    Case 2: cLogging = iNumPlayers: backColour = vbYellow
    End Select
    iTotal = cOnline + cOffline + cLogging
    
    flxWhoOnline(iStatus).Rows = 1 + iNumPlayers
    
    Dim iCounter As Long, iBase As Long, iCtr2 As Long
    Dim iString As String
    For iCounter = 0 To iNumPlayers - 1
        flxWhoOnline(iStatus).Row = 1 + iCounter
        iBase = 4 + iCounter * 35
        
        flxWhoOnline(iStatus).Col = 0
        flxWhoOnline(iStatus).Text = CLng(bleh(iBase)) * 16777216 + CLng(bleh(iBase + 1)) * 65536 + CLng(bleh(iBase + 2)) * 256 + CLng(bleh(iBase + 3))
        flxWhoOnline(iStatus).CellBackColor = backColour
        flxWhoOnline(iStatus).Col = 1
        iString = ""
        For iCtr2 = 0 To 30
            If bleh(iBase + 4 + iCtr2) <> 0 Then
                iString = iString & Chr$(bleh(iBase + 4 + iCtr2))
            Else
                Exit For
            End If
        Next iCtr2
        flxWhoOnline(iStatus).Text = iString
        flxWhoOnline(iStatus).CellBackColor = backColour
    Next iCounter
End Sub

Private Sub HandleServerStatResponse(wsXGM As Winsock, bleh() As Byte)
    Dim iVal As Long
    Select Case bleh(1)
    Case 0, 1, 2, 4, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21
        flxServerStatistics.Row = bleh(1) + 1
        flxServerStatistics.Col = 1
        iVal = CLng(bleh(2)) * 16776216 + CLng(bleh(3)) * 65536 + CLng(bleh(4)) * 256 + CLng(bleh(5))
        flxServerStatistics.Text = CStr(iVal)
    Case 5
        flxServerStatistics.Row = bleh(1) + 1
        flxServerStatistics.Col = 1
        If bleh(2) = 255 Then
            flxServerStatistics.Text = "Infinite"
        Else
            iVal = CLng(bleh(2)) * 16776216 + CLng(bleh(3)) * 65536 + CLng(bleh(4)) * 256 + CLng(bleh(5))
            flxServerStatistics.Text = CStr(iVal)
        End If
    Case 3
        Dim cRespTime As Long
        cRespTime = GetCurrentTime
        flxServerStatistics.Row = bleh(1) + 1
        flxServerStatistics.Col = 1
        iVal = cRespTime - cCurrentTime
        flxServerStatistics.Text = CStr(iVal) & " ms"
    Case 11
    End Select
End Sub

Private Sub HandleServerLoginResponse(wsXGM As Winsock, bleh() As Byte)
    Dim iResponse As Byte
    iResponse = bleh(1)
    If iResponse <> 0 Then
        If iResponse = 1 Then
            MsgBox "Error connecting to server, bad username / pass"
        ElseIf iResponse = 2 Then
            MsgBox "Error connecting to server, invalid clearance level"
        Else
            MsgBox "Unknown error connecting to server"
        End If
        DoDisconnect
        Exit Sub
    End If
    SendClientVersionPacket wsXGM
    SendServerVersionRequest wsXGM
    Exit Sub
End Sub

Private Sub HandleServerProtocolVersion(wsXGM As Winsock, bleh() As Byte)
    txtServerProtocolVersion.ForeColor = vbBlue
    txtServerProtocolVersion.Text = bleh(1) & "." & bleh(2) & "." & bleh(3) & "." & bleh(4)
    If bleh(1) = XGM_CLIENT_PROTOCOL_MAJOR And bleh(2) = XGM_CLIENT_PROTOCOL_MINOR And bleh(3) = XGM_CLIENT_PROTOCOL_REVISION And bleh(4) = XGM_CLIENT_PROTOCOL_EXTRA Then
        txtServerProtocolVersion.BackColor = vbGreen
    Else
        txtServerProtocolVersion.BackColor = vbRed
    End If
End Sub
Private Sub wsXGM_DataArrival(ByVal bytesTotal As Long)
    Dim command As Byte
    wsXGM.PeekData command, vbByte, 1
    Dim bleh() As Byte
    ReDim bleh(0 To bytesTotal - 1)
    wsXGM.GetData bleh, vbArray + vbByte
    Select Case command
    Case 0  ' login response
        HandleServerLoginResponse wsXGM, bleh
    Case 1  ' access level change response
        Debug.Print "Access Level Change Response"
    Case 2  ' who's on/off/logging response
        HandleServerWhoResponse wsXGM, bleh
    Case 3  ' show queue response
        Debug.Print "Show Queue Response"
    Case 4  ' handle queue entry response 1
        Debug.Print "Handle Queue Entry Response 1"
    Case 5  ' handle queue entry response 2
        Debug.Print "Handle QUeue Entry Response 2"
    Case 6  ' retrieve account names
        Debug.Print "Retrieve Account Names Response"
    Case 7  ' retrieve account details
        Debug.Print "Retrieve Account Details Response"
    Case 8  ' ban duration response
        Debug.Print "Ban Duration Response"
    Case 9  ' account add/update/remove response
        Debug.Print "Account Add/Update/Remove Response"
    Case 10 ' XGM Server Protocol Version Response
        HandleServerProtocolVersion wsXGM, bleh
    Case 11 ' Script Reload response
        Debug.Print "Script Reload Response"
    Case 12 ' Installed scripts response
        Debug.Print "Installed Scripts Response"
    Case 13 ' Get Script Response
        Debug.Print "Get Script Response"
    Case 14 ' Get Script Data Chunk
        Debug.Print "Get Script Data Chunk Response"
    Case 15 ' Get Script Section Names Response
        Debug.Print "Get Script Section Names Response"
    Case 16 ' Get Script Section Names Data Chunk
        Debug.Print "Get Script Section Names Data Chunk Response"
    Case 17 ' Get Script Section Response
        Debug.Print "Get Script Section Response"
    Case 18 ' Server Stat Response
        HandleServerStatResponse wsXGM, bleh
    End Select
End Sub

Private Sub wsXGM_Error(ByVal Number As Integer, Description As String, ByVal Scode As Long, ByVal Source As String, ByVal HelpFile As String, ByVal HelpContext As Long, CancelDisplay As Boolean)
    Select Case Number
    Case 10061
        DoDisconnect
        sbStatusBar.Panels(1).Text = "10061"
    Case Default
        sbStatusBar.Panels(1).Text = Number & " " & Description
    End Select
End Sub

