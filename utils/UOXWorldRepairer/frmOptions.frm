VERSION 5.00
Object = "{BDC217C8-ED16-11CD-956C-0000C04E4C0A}#1.1#0"; "TABCTL32.OCX"
Begin VB.Form frmOptions 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "UOXWR Options"
   ClientHeight    =   3195
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdSave 
      Caption         =   "&Save"
      Height          =   255
      Left            =   3240
      TabIndex        =   2
      Top             =   2880
      Width           =   1335
   End
   Begin VB.CommandButton cmdReturn 
      Caption         =   "&Return"
      Height          =   255
      Left            =   120
      TabIndex        =   1
      Top             =   2880
      Width           =   1335
   End
   Begin TabDlg.SSTab SSTab1 
      Height          =   2655
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   4455
      _ExtentX        =   7858
      _ExtentY        =   4683
      _Version        =   393216
      Style           =   1
      Tabs            =   1
      TabHeight       =   520
      TabCaption(0)   =   "Container Locations"
      TabPicture(0)   =   "frmOptions.frx":0000
      Tab(0).ControlEnabled=   -1  'True
      Tab(0).Control(0)=   "lblContainerLocationX"
      Tab(0).Control(0).Enabled=   0   'False
      Tab(0).Control(1)=   "lblContainerLocationY"
      Tab(0).Control(1).Enabled=   0   'False
      Tab(0).Control(2)=   "lblContainerLocationZ"
      Tab(0).Control(2).Enabled=   0   'False
      Tab(0).Control(3)=   "txtContainerLocationX"
      Tab(0).Control(3).Enabled=   0   'False
      Tab(0).Control(4)=   "txtContainerLocationY"
      Tab(0).Control(4).Enabled=   0   'False
      Tab(0).Control(5)=   "txtContainerLocationZ"
      Tab(0).Control(5).Enabled=   0   'False
      Tab(0).ControlCount=   6
      Begin VB.TextBox txtContainerLocationZ 
         Height          =   285
         Left            =   1080
         TabIndex        =   8
         Top             =   1200
         Width           =   1095
      End
      Begin VB.TextBox txtContainerLocationY 
         Height          =   285
         Left            =   1080
         TabIndex        =   7
         Top             =   840
         Width           =   1095
      End
      Begin VB.TextBox txtContainerLocationX 
         Height          =   285
         Left            =   1080
         TabIndex        =   6
         Top             =   480
         Width           =   1095
      End
      Begin VB.Label lblContainerLocationZ 
         BackStyle       =   0  'Transparent
         Caption         =   "Z"
         Height          =   285
         Left            =   120
         TabIndex        =   5
         Top             =   1200
         Width           =   735
      End
      Begin VB.Label lblContainerLocationY 
         BackStyle       =   0  'Transparent
         Caption         =   "Y"
         Height          =   285
         Left            =   120
         TabIndex        =   4
         Top             =   840
         Width           =   735
      End
      Begin VB.Label lblContainerLocationX 
         BackStyle       =   0  'Transparent
         Caption         =   "X"
         Height          =   285
         Left            =   120
         TabIndex        =   3
         Top             =   480
         Width           =   735
      End
   End
End
Attribute VB_Name = "frmOptions"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub cmdReturn_Click()
    
    Unload Me

End Sub

Private Sub cmdSave_Click()

    ContainerLocation.x = txtContainerLocationX.Text
    ContainerLocation.y = txtContainerLocationY.Text
    ContainerLocation.z = txtContainerLocationZ.Text
    
    SaveSetting App.Title, "Options", "ContainerX", ContainerLocation.x
    SaveSetting App.Title, "Options", "ContainerY", ContainerLocation.y
    SaveSetting App.Title, "Options", "ContainerZ", ContainerLocation.z
    
End Sub

Private Sub Form_Load()

    txtContainerLocationX.Text = ContainerLocation.x
    txtContainerLocationY.Text = ContainerLocation.y
    txtContainerLocationZ.Text = ContainerLocation.z
    
End Sub
