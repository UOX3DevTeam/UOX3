VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
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
      Begin MSComctlLib.ProgressBar pgbProgress 
         Height          =   350
         Left            =   80
         TabIndex        =   10
         Top             =   4200
         Width           =   7215
         _ExtentX        =   12726
         _ExtentY        =   609
         _Version        =   393216
         Appearance      =   1
         Scrolling       =   1
      End
      Begin VB.PictureBox picLogo 
         Height          =   3705
         Left            =   150
         Picture         =   "frmSplash.frx":0000
         ScaleHeight     =   3645
         ScaleWidth      =   3225
         TabIndex        =   2
         Top             =   255
         Width           =   3285
      End
      Begin VB.Label lblLicenseTo 
         Alignment       =   1  'Right Justify
         Caption         =   "LicenseTo"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   270
         TabIndex        =   1
         Tag             =   "LicenseTo"
         Top             =   300
         Width           =   6855
      End
      Begin VB.Label lblProductName 
         AutoSize        =   -1  'True
         BackStyle       =   0  'Transparent
         Caption         =   "Product"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   24
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   555
         Left            =   3480
         TabIndex        =   9
         Tag             =   "Product"
         Top             =   1200
         Width           =   1785
      End
      Begin VB.Label lblCompanyProduct 
         AutoSize        =   -1  'True
         BackStyle       =   0  'Transparent
         Caption         =   "CompanyProduct"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   18
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   435
         Left            =   3480
         TabIndex        =   8
         Tag             =   "CompanyProduct"
         Top             =   765
         Width           =   3000
      End
      Begin VB.Label lblPlatform 
         Alignment       =   1  'Right Justify
         AutoSize        =   -1  'True
         Caption         =   "Platform"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   15.75
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   360
         Left            =   5730
         TabIndex        =   7
         Tag             =   "Platform"
         Top             =   2400
         Width           =   1275
      End
      Begin VB.Label lblVersion 
         Alignment       =   1  'Right Justify
         AutoSize        =   -1  'True
         Caption         =   "Version"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   12
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Left            =   6120
         TabIndex        =   6
         Tag             =   "Version"
         Top             =   2760
         Width           =   885
      End
      Begin VB.Label lblWarning 
         Caption         =   "Warning"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   195
         Left            =   240
         TabIndex        =   3
         Tag             =   "Warning"
         Top             =   3960
         Width           =   6855
      End
      Begin VB.Label lblCompany 
         Caption         =   "Company"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   4710
         TabIndex        =   5
         Tag             =   "Company"
         Top             =   3330
         Width           =   2415
      End
      Begin VB.Label lblCopyright 
         Caption         =   "Copyright"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   4710
         TabIndex        =   4
         Tag             =   "Copyright"
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
    lblVersion.Caption = "Version " & App.Major & "." & App.Minor & " Release " & App.Revision
    lblProductName.Caption = App.Title
    lblPlatform.Caption = "Win32"
    lblCopyright.Caption = App.LegalCopyright
    lblCompany.Caption = App.CompanyName
    lblLicenseTo.Caption = "Licensed to: UOX User"
    lblCompanyProduct.Caption = App.ProductName
End Sub

