VERSION 5.00
Begin VB.Form frmSplash 
   BorderStyle     =   3  'Fixed Dialog
   ClientHeight    =   4710
   ClientLeft      =   45
   ClientTop       =   45
   ClientWidth     =   7455
   ControlBox      =   0   'False
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4710
   ScaleWidth      =   7455
   ShowInTaskbar   =   0   'False
   StartUpPosition =   2  'CenterScreen
   Visible         =   0   'False
   Begin VB.Frame fraMainFrame 
      Height          =   4590
      Left            =   45
      TabIndex        =   0
      Top             =   -15
      Width           =   7380
      Begin VB.PictureBox picLogo 
         BorderStyle     =   0  'None
         Height          =   3765
         Left            =   120
         ScaleHeight     =   3765
         ScaleWidth      =   3285
         TabIndex        =   2
         Top             =   240
         Width           =   3285
      End
      Begin VB.Label lblProductName 
         AutoSize        =   -1  'True
         BackStyle       =   0  'Transparent
         Caption         =   "Product"
         BeginProperty Font 
            Name            =   "Microsoft Sans Serif"
            Size            =   29.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   660
         Left            =   3480
         TabIndex        =   8
         Tag             =   "1010"
         Top             =   1200
         Width           =   2145
      End
      Begin VB.Label lblLicenseTo 
         Alignment       =   1  'Right Justify
         BackStyle       =   0  'Transparent
         Caption         =   "LicenseTo"
         Height          =   255
         Left            =   270
         TabIndex        =   1
         Tag             =   "1011"
         Top             =   300
         Width           =   6855
      End
      Begin VB.Label lblPlatform 
         Alignment       =   1  'Right Justify
         AutoSize        =   -1  'True
         BackStyle       =   0  'Transparent
         Caption         =   "Platform"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   13.5
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   360
         Left            =   5865
         TabIndex        =   7
         Tag             =   "1008"
         Top             =   2400
         Width           =   1140
      End
      Begin VB.Label lblVersion 
         Alignment       =   1  'Right Justify
         AutoSize        =   -1  'True
         BackStyle       =   0  'Transparent
         Caption         =   "Version"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   12
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   300
         Left            =   6075
         TabIndex        =   6
         Tag             =   "1007"
         Top             =   2760
         Width           =   930
      End
      Begin VB.Label lblWarning 
         BackStyle       =   0  'Transparent
         Caption         =   "Warning"
         Height          =   435
         Left            =   300
         TabIndex        =   3
         Tag             =   "1006"
         Top             =   4080
         Width           =   6855
      End
      Begin VB.Label lblCompany 
         BackStyle       =   0  'Transparent
         Caption         =   "Company"
         Height          =   255
         Left            =   4710
         TabIndex        =   5
         Tag             =   "1005"
         Top             =   3330
         Width           =   2415
      End
      Begin VB.Label lblCopyright 
         BackStyle       =   0  'Transparent
         Caption         =   "Copyright"
         Height          =   255
         Left            =   4710
         TabIndex        =   4
         Tag             =   "1004"
         Top             =   3120
         Width           =   2415
      End
   End
End
Attribute VB_Name = "frmSplash"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Form_Load()
    'LoadResStrings Me
    lblVersion.Caption = "Version " & App.Major & "." & App.Minor & "." & App.Revision
    lblProductName.Caption = App.Title
    lblCopyright.Caption = App.LegalCopyright
    lblCompany.Caption = App.CompanyName
    picLogo.Picture = LoadPicture(App.Path & "\uoxgm.gif")
End Sub

