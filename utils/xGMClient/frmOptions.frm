VERSION 5.00
Object = "{BDC217C8-ED16-11CD-956C-0000C04E4C0A}#1.1#0"; "TABCTL32.OCX"
Begin VB.Form frmOptions 
   Caption         =   "Options"
   ClientHeight    =   3195
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   6765
   LinkTopic       =   "Form1"
   ScaleHeight     =   3195
   ScaleWidth      =   6765
   StartUpPosition =   3  'Windows Default
   Begin TabDlg.SSTab tabOptions 
      Height          =   2535
      Left            =   120
      TabIndex        =   3
      Top             =   120
      Width           =   6615
      _ExtentX        =   11668
      _ExtentY        =   4471
      _Version        =   393216
      Style           =   1
      Tabs            =   2
      Tab             =   1
      TabHeight       =   520
      TabCaption(0)   =   "Remote Host"
      TabPicture(0)   =   "frmOptions.frx":0000
      Tab(0).ControlEnabled=   0   'False
      Tab(0).Control(0)=   "lblRemoteHostName"
      Tab(0).Control(1)=   "lblRemoteHostIP"
      Tab(0).Control(2)=   "lblRemoteHostPort"
      Tab(0).Control(3)=   "txtRemoteHostName"
      Tab(0).Control(4)=   "txtRemoteHostIP"
      Tab(0).Control(5)=   "txtRemoteHostPort"
      Tab(0).ControlCount=   6
      TabCaption(1)   =   "Authentication"
      TabPicture(1)   =   "frmOptions.frx":001C
      Tab(1).ControlEnabled=   -1  'True
      Tab(1).Control(0)=   "lblAuthenticationUsername"
      Tab(1).Control(0).Enabled=   0   'False
      Tab(1).Control(1)=   "lblAuthenticationClearance"
      Tab(1).Control(1).Enabled=   0   'False
      Tab(1).Control(2)=   "Label1"
      Tab(1).Control(2).Enabled=   0   'False
      Tab(1).Control(3)=   "txtAuthenticationUsername"
      Tab(1).Control(3).Enabled=   0   'False
      Tab(1).Control(4)=   "optClearance(0)"
      Tab(1).Control(4).Enabled=   0   'False
      Tab(1).Control(5)=   "optClearance(1)"
      Tab(1).Control(5).Enabled=   0   'False
      Tab(1).Control(6)=   "optClearance(2)"
      Tab(1).Control(6).Enabled=   0   'False
      Tab(1).Control(7)=   "optClearance(3)"
      Tab(1).Control(7).Enabled=   0   'False
      Tab(1).Control(8)=   "txtPassword"
      Tab(1).Control(8).Enabled=   0   'False
      Tab(1).ControlCount=   9
      Begin VB.TextBox txtPassword 
         Height          =   285
         Left            =   1320
         TabIndex        =   18
         Top             =   840
         Width           =   1695
      End
      Begin VB.OptionButton optClearance 
         Caption         =   "Admin"
         Height          =   255
         Index           =   3
         Left            =   3120
         TabIndex        =   16
         Top             =   1560
         Width           =   1575
      End
      Begin VB.OptionButton optClearance 
         Caption         =   "GM"
         Height          =   255
         Index           =   2
         Left            =   3120
         TabIndex        =   15
         Top             =   1320
         Width           =   1575
      End
      Begin VB.OptionButton optClearance 
         Caption         =   "Counselor"
         Height          =   255
         Index           =   1
         Left            =   3120
         TabIndex        =   14
         Top             =   1080
         Width           =   1575
      End
      Begin VB.OptionButton optClearance 
         Caption         =   "Player"
         Height          =   255
         Index           =   0
         Left            =   3120
         TabIndex        =   13
         Top             =   840
         Width           =   1575
      End
      Begin VB.TextBox txtAuthenticationUsername 
         Height          =   285
         Left            =   1320
         TabIndex        =   11
         Top             =   480
         Width           =   1695
      End
      Begin VB.TextBox txtRemoteHostPort 
         Height          =   285
         Left            =   -73440
         TabIndex        =   9
         Top             =   1200
         Width           =   1815
      End
      Begin VB.TextBox txtRemoteHostIP 
         Height          =   285
         Left            =   -73440
         TabIndex        =   8
         Top             =   840
         Width           =   1815
      End
      Begin VB.TextBox txtRemoteHostName 
         Height          =   285
         Left            =   -73440
         TabIndex        =   7
         Top             =   480
         Width           =   1815
      End
      Begin VB.Label Label1 
         BackStyle       =   0  'Transparent
         Caption         =   "Password"
         Height          =   255
         Left            =   120
         TabIndex        =   17
         Top             =   840
         Width           =   975
      End
      Begin VB.Label lblAuthenticationClearance 
         BackStyle       =   0  'Transparent
         Caption         =   "Clearance"
         Height          =   255
         Left            =   3120
         TabIndex        =   12
         Top             =   480
         Width           =   1455
      End
      Begin VB.Label lblAuthenticationUsername 
         BackStyle       =   0  'Transparent
         Caption         =   "Username"
         Height          =   255
         Left            =   120
         TabIndex        =   10
         Top             =   480
         Width           =   975
      End
      Begin VB.Label lblRemoteHostPort 
         BackStyle       =   0  'Transparent
         Caption         =   "Remote Port"
         Height          =   255
         Left            =   -74880
         TabIndex        =   6
         Tag             =   "1030"
         Top             =   1200
         Width           =   1455
      End
      Begin VB.Label lblRemoteHostIP 
         BackStyle       =   0  'Transparent
         Caption         =   "Remote Host IP"
         Height          =   255
         Left            =   -74880
         TabIndex        =   5
         Tag             =   "1029"
         Top             =   840
         Width           =   1455
      End
      Begin VB.Label lblRemoteHostName 
         BackStyle       =   0  'Transparent
         Caption         =   "Remote Hostname"
         Height          =   255
         Left            =   -74880
         TabIndex        =   4
         Tag             =   "1028"
         Top             =   480
         Width           =   1455
      End
   End
   Begin VB.CommandButton cmdRefresh 
      Caption         =   "R&efresh"
      Height          =   375
      Left            =   4560
      TabIndex        =   2
      Top             =   2760
      Width           =   2175
   End
   Begin VB.CommandButton cmdSave 
      Caption         =   "&Save"
      Height          =   375
      Left            =   2280
      TabIndex        =   1
      Top             =   2760
      Width           =   2175
   End
   Begin VB.CommandButton cmdReturn 
      Caption         =   "&Return"
      Height          =   375
      Left            =   0
      TabIndex        =   0
      Top             =   2760
      Width           =   2175
   End
End
Attribute VB_Name = "frmOptions"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub cmdRefresh_Click()

    LoadConfigData
    
    txtRemoteHostIP.Text = appControl.remoteHostIP
    txtRemoteHostName.Text = appControl.remoteHostName
    txtRemoteHostPort.Text = appControl.remoteHostPort
    optClearance(appControl.clearanceLevel).Value = True
    
    txtAuthenticationUsername.Text = GetSetting(App.Title, "ConfigSettings", "Username", "")
    txtPassword.Text = GetSetting(App.Title, "ConfigSettings", "Password", "")

End Sub

Private Sub cmdReturn_Click()
    Unload Me
End Sub

Private Sub cmdSave_Click()

    appControl.remoteHostIP = txtRemoteHostIP.Text
    appControl.remoteHostName = txtRemoteHostName.Text
    appControl.remoteHostPort = txtRemoteHostPort.Text
    Dim i As Long
    For i = PLAYER_AUTH To ADMIN_AUTH
        If optClearance(i).Value = True Then
            appControl.clearanceLevel = i
            Exit For
        End If
    Next i
    
    SaveSetting App.Title, "ConfigSettings", "RemoteHostIP", appControl.remoteHostIP
    SaveSetting App.Title, "ConfigSettings", "RemoteHostName", appControl.remoteHostName
    SaveSetting App.Title, "ConfigSettings", "RemotePort", appControl.remoteHostPort
    SaveSetting App.Title, "ConfigSettings", "ClearanceLevel", appControl.clearanceLevel
    SaveSetting App.Title, "ConfigSettings", "Username", txtAuthenticationUsername.Text
    SaveSetting App.Title, "ConfigSettings", "Password", txtPassword.Text

End Sub

Private Sub Form_Load()
    cmdRefresh_Click
End Sub
