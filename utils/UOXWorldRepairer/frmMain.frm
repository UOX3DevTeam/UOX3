VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "comdlg32.ocx"
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Object = "{5E9E78A0-531B-11CF-91F6-C2863C385E30}#1.0#0"; "MSFLXGRD.OCX"
Begin VB.Form frmMain 
   Caption         =   "UOXWorldRepairer"
   ClientHeight    =   7155
   ClientLeft      =   165
   ClientTop       =   735
   ClientWidth     =   13185
   LinkTopic       =   "Form1"
   ScaleHeight     =   7155
   ScaleWidth      =   13185
   StartUpPosition =   3  'Windows Default
   Begin VB.Frame fraTab 
      Height          =   6375
      Index           =   1
      Left            =   360
      TabIndex        =   3
      Top             =   2520
      Width           =   7095
      Begin VB.CommandButton cmdItemErrorFixStatBugs 
         Caption         =   "Fix item stat bugs"
         Height          =   255
         Left            =   120
         TabIndex        =   184
         Top             =   2280
         Width           =   2535
      End
      Begin VB.CommandButton cmdItemErrorFixBadOwners 
         Caption         =   "Reset bad owners"
         Height          =   255
         Left            =   120
         TabIndex        =   168
         Top             =   2040
         Width           =   2535
      End
      Begin VB.CommandButton cmdItemErrorFixContainers 
         Caption         =   "Fix all container problems"
         Height          =   255
         Left            =   120
         TabIndex        =   157
         Top             =   1800
         Width           =   2535
      End
      Begin VB.TextBox txtItemErrorSpawner 
         Height          =   285
         Left            =   1320
         TabIndex        =   149
         Top             =   1440
         Width           =   1335
      End
      Begin VB.CheckBox chkItemErrorSpawner 
         Caption         =   "Spawn"
         Height          =   255
         Left            =   120
         TabIndex        =   148
         Top             =   1440
         Width           =   1095
      End
      Begin VB.TextBox txtItemErrorOwner 
         Height          =   285
         Left            =   1320
         TabIndex        =   147
         Top             =   1200
         Width           =   1335
      End
      Begin VB.CheckBox chkItemErrorOwner 
         Caption         =   "Owner"
         Height          =   255
         Left            =   120
         TabIndex        =   146
         Top             =   1200
         Width           =   1095
      End
      Begin VB.TextBox txtItemErrorColour 
         Height          =   285
         Left            =   1320
         TabIndex        =   145
         Top             =   960
         Width           =   1335
      End
      Begin VB.CheckBox chkItemErrorColour 
         Caption         =   "Colour"
         Height          =   255
         Left            =   120
         TabIndex        =   144
         Top             =   960
         Width           =   1095
      End
      Begin VB.Frame fraItemErrorLocation 
         Caption         =   "Location Issues"
         Height          =   1095
         Left            =   2880
         TabIndex        =   130
         Top             =   2760
         Width           =   2535
         Begin VB.TextBox txtItemErrorLocation 
            Height          =   285
            Index           =   2
            Left            =   1080
            TabIndex        =   136
            Top             =   720
            Width           =   1335
         End
         Begin VB.TextBox txtItemErrorLocation 
            Height          =   285
            Index           =   1
            Left            =   1080
            TabIndex        =   135
            Top             =   480
            Width           =   1335
         End
         Begin VB.TextBox txtItemErrorLocation 
            Height          =   285
            Index           =   0
            Left            =   1080
            TabIndex        =   134
            Top             =   240
            Width           =   1335
         End
         Begin VB.CheckBox chkItemErrorLocation 
            Caption         =   "Z"
            Height          =   255
            Index           =   2
            Left            =   120
            TabIndex        =   133
            Top             =   720
            Width           =   855
         End
         Begin VB.CheckBox chkItemErrorLocation 
            Caption         =   "Y"
            Height          =   255
            Index           =   1
            Left            =   120
            TabIndex        =   132
            Top             =   480
            Width           =   855
         End
         Begin VB.CheckBox chkItemErrorLocation 
            Caption         =   "X"
            Height          =   255
            Index           =   0
            Left            =   120
            TabIndex        =   131
            Top             =   240
            Width           =   855
         End
      End
      Begin VB.TextBox txtItemErrorContainer 
         Height          =   285
         Left            =   1320
         TabIndex        =   129
         Top             =   720
         Width           =   1335
      End
      Begin VB.CheckBox chkItemErrorContainer 
         Caption         =   "Container"
         Height          =   255
         Left            =   120
         TabIndex        =   128
         Top             =   720
         Width           =   1095
      End
      Begin VB.Frame fraItemErrorStat 
         Caption         =   "Stat Issues"
         Height          =   2055
         Left            =   2880
         TabIndex        =   120
         Top             =   600
         Width           =   4095
         Begin VB.TextBox txtItemStatIssue 
            Height          =   285
            Index           =   6
            Left            =   1800
            TabIndex        =   143
            Top             =   1680
            Width           =   1455
         End
         Begin VB.TextBox txtItemStatIssue 
            Height          =   285
            Index           =   5
            Left            =   1800
            TabIndex        =   142
            Top             =   1440
            Width           =   1455
         End
         Begin VB.TextBox txtItemStatIssue 
            Height          =   285
            Index           =   4
            Left            =   1800
            TabIndex        =   141
            Top             =   1200
            Width           =   1455
         End
         Begin VB.TextBox txtItemStatIssue 
            Height          =   285
            Index           =   3
            Left            =   1800
            TabIndex        =   140
            Top             =   960
            Width           =   1455
         End
         Begin VB.TextBox txtItemStatIssue 
            Height          =   285
            Index           =   2
            Left            =   1800
            TabIndex        =   139
            Top             =   720
            Width           =   1455
         End
         Begin VB.TextBox txtItemStatIssue 
            Height          =   285
            Index           =   1
            Left            =   1800
            TabIndex        =   138
            Top             =   480
            Width           =   1455
         End
         Begin VB.TextBox txtItemStatIssue 
            Height          =   285
            Index           =   0
            Left            =   1800
            TabIndex        =   137
            Top             =   240
            Width           =   1455
         End
         Begin VB.CheckBox chkItemStatIssue 
            Caption         =   "Max Health"
            Height          =   255
            Index           =   6
            Left            =   120
            TabIndex        =   127
            Top             =   1680
            Width           =   1455
         End
         Begin VB.CheckBox chkItemStatIssue 
            Caption         =   "Health"
            Height          =   255
            Index           =   5
            Left            =   120
            TabIndex        =   126
            Top             =   1440
            Width           =   1455
         End
         Begin VB.CheckBox chkItemStatIssue 
            Caption         =   "Mana"
            Height          =   255
            Index           =   4
            Left            =   120
            TabIndex        =   125
            Top             =   1200
            Width           =   1455
         End
         Begin VB.CheckBox chkItemStatIssue 
            Caption         =   "Stamina"
            Height          =   255
            Index           =   3
            Left            =   120
            TabIndex        =   124
            Top             =   960
            Width           =   1455
         End
         Begin VB.CheckBox chkItemStatIssue 
            Caption         =   "Strength"
            Height          =   255
            Index           =   2
            Left            =   120
            TabIndex        =   123
            Top             =   720
            Width           =   1455
         End
         Begin VB.CheckBox chkItemStatIssue 
            Caption         =   "Intelligence"
            Height          =   255
            Index           =   1
            Left            =   120
            TabIndex        =   122
            Top             =   480
            Width           =   1455
         End
         Begin VB.CheckBox chkItemStatIssue 
            Caption         =   "Dexterity"
            Height          =   255
            Index           =   0
            Left            =   120
            TabIndex        =   121
            Top             =   240
            Width           =   1455
         End
      End
      Begin VB.ComboBox cmbItemError 
         Height          =   315
         Left            =   120
         Style           =   2  'Dropdown List
         TabIndex        =   119
         Top             =   240
         Width           =   3495
      End
   End
   Begin VB.Frame fraTab 
      Height          =   3735
      Index           =   5
      Left            =   4680
      TabIndex        =   7
      Top             =   1320
      Width           =   6735
      Begin VB.TextBox txtItemInfoLayer 
         Height          =   285
         Left            =   4680
         TabIndex        =   167
         Top             =   1680
         Width           =   1935
      End
      Begin VB.TextBox txtItemInfoSpawn 
         Height          =   285
         Left            =   4680
         TabIndex        =   166
         Top             =   1440
         Width           =   1935
      End
      Begin VB.TextBox txtItemInfoType 
         Height          =   285
         Left            =   4680
         TabIndex        =   165
         Top             =   1200
         Width           =   1935
      End
      Begin VB.TextBox txtItemInfoContainer 
         Height          =   285
         Left            =   4680
         TabIndex        =   164
         Top             =   960
         Width           =   1935
      End
      Begin VB.TextBox txtItemInfoOwner 
         Height          =   285
         Left            =   4680
         TabIndex        =   163
         Top             =   720
         Width           =   1935
      End
      Begin VB.ComboBox cmbItemInfo 
         Height          =   315
         Left            =   120
         Style           =   2  'Dropdown List
         TabIndex        =   27
         Top             =   240
         Width           =   2535
      End
      Begin VB.ComboBox cmbItemInfoItemBlock 
         Height          =   315
         Left            =   4320
         Style           =   2  'Dropdown List
         TabIndex        =   26
         Top             =   240
         Width           =   1455
      End
      Begin VB.TextBox txtItemInfoName 
         Height          =   285
         Left            =   1560
         TabIndex        =   25
         Top             =   720
         Width           =   1935
      End
      Begin VB.TextBox txtItemInfoSerial 
         Height          =   285
         Left            =   1560
         TabIndex        =   24
         Top             =   960
         Width           =   1935
      End
      Begin VB.TextBox txtItemInfoStrength 
         Height          =   285
         Left            =   1560
         TabIndex        =   23
         Top             =   1200
         Width           =   1935
      End
      Begin VB.TextBox txtItemInfoDexterity 
         Height          =   285
         Left            =   1560
         TabIndex        =   22
         Top             =   1440
         Width           =   1935
      End
      Begin VB.TextBox txtItemInfoIntelligence 
         Height          =   285
         Left            =   1560
         TabIndex        =   21
         Top             =   1680
         Width           =   1935
      End
      Begin VB.TextBox txtItemInfoHealth 
         Height          =   285
         Left            =   1560
         TabIndex        =   20
         Top             =   1920
         Width           =   1935
      End
      Begin VB.TextBox txtItemInfoMaxHealth 
         Height          =   285
         Left            =   1560
         TabIndex        =   19
         Top             =   2160
         Width           =   1935
      End
      Begin VB.TextBox txtItemInfoID 
         Height          =   285
         Left            =   1560
         TabIndex        =   18
         Top             =   2400
         Width           =   1935
      End
      Begin VB.TextBox txtItemInfoX 
         Height          =   285
         Left            =   1560
         TabIndex        =   17
         Top             =   2640
         Width           =   1935
      End
      Begin VB.TextBox txtItemInfoY 
         Height          =   285
         Left            =   1560
         TabIndex        =   16
         Top             =   2880
         Width           =   1935
      End
      Begin VB.TextBox txtItemInfoZ 
         Height          =   285
         Left            =   1560
         TabIndex        =   15
         Top             =   3120
         Width           =   1935
      End
      Begin VB.TextBox txtItemInfoColour 
         Height          =   285
         Left            =   1560
         TabIndex        =   14
         Top             =   3360
         Width           =   1935
      End
      Begin VB.Label lblItemInfoLayer 
         BackStyle       =   0  'Transparent
         Caption         =   "Layer"
         Height          =   255
         Left            =   3600
         TabIndex        =   162
         Top             =   1680
         Width           =   1095
      End
      Begin VB.Label lblItemInfoSpawn 
         BackStyle       =   0  'Transparent
         Caption         =   "Spawn"
         Height          =   255
         Left            =   3600
         TabIndex        =   161
         Top             =   1440
         Width           =   1095
      End
      Begin VB.Label lblItemInfoType 
         BackStyle       =   0  'Transparent
         Caption         =   "Type"
         Height          =   255
         Left            =   3600
         TabIndex        =   160
         Top             =   1200
         Width           =   1095
      End
      Begin VB.Label lblItemInfoContainer 
         BackStyle       =   0  'Transparent
         Caption         =   "Container"
         Height          =   255
         Left            =   3600
         TabIndex        =   159
         Top             =   960
         Width           =   1095
      End
      Begin VB.Label lblItemInfoOwner 
         BackStyle       =   0  'Transparent
         Caption         =   "Owner"
         Height          =   255
         Left            =   3600
         TabIndex        =   158
         Top             =   720
         Width           =   1095
      End
      Begin VB.Label lblItemInfoItemBlock 
         BackStyle       =   0  'Transparent
         Caption         =   "Item Block"
         Height          =   255
         Left            =   2880
         TabIndex        =   40
         Top             =   240
         Width           =   1215
      End
      Begin VB.Label lblItemInfoColour 
         BackStyle       =   0  'Transparent
         Caption         =   "Colour"
         Height          =   255
         Left            =   120
         TabIndex        =   39
         Top             =   3360
         Width           =   1335
      End
      Begin VB.Label lblItemInfoMaxHealth 
         BackStyle       =   0  'Transparent
         Caption         =   "Max Health"
         Height          =   255
         Left            =   120
         TabIndex        =   38
         Top             =   2160
         Width           =   1335
      End
      Begin VB.Label lblItemInfoSerial 
         BackStyle       =   0  'Transparent
         Caption         =   "Serial"
         Height          =   255
         Left            =   120
         TabIndex        =   37
         Top             =   960
         Width           =   1335
      End
      Begin VB.Label lblItemInfoZ 
         BackStyle       =   0  'Transparent
         Caption         =   "Z"
         Height          =   255
         Left            =   120
         TabIndex        =   36
         Top             =   3120
         Width           =   1335
      End
      Begin VB.Label lblItemInfoY 
         BackStyle       =   0  'Transparent
         Caption         =   "Y"
         Height          =   255
         Left            =   120
         TabIndex        =   35
         Top             =   2880
         Width           =   1335
      End
      Begin VB.Label lblItemInfoItemHealth 
         BackStyle       =   0  'Transparent
         Caption         =   "Health"
         Height          =   255
         Left            =   120
         TabIndex        =   34
         Top             =   1920
         Width           =   1335
      End
      Begin VB.Label lblItemInfoIntelligence 
         BackStyle       =   0  'Transparent
         Caption         =   "Intelligence"
         Height          =   255
         Left            =   120
         TabIndex        =   33
         Top             =   1680
         Width           =   1335
      End
      Begin VB.Label lblItemInfoDexterity 
         BackStyle       =   0  'Transparent
         Caption         =   "Dexterity"
         Height          =   255
         Left            =   120
         TabIndex        =   32
         Top             =   1440
         Width           =   1335
      End
      Begin VB.Label lblItemInfoStrength 
         BackStyle       =   0  'Transparent
         Caption         =   "Strength"
         Height          =   255
         Left            =   120
         TabIndex        =   31
         Top             =   1200
         Width           =   1335
      End
      Begin VB.Label lblItemInfoX 
         BackStyle       =   0  'Transparent
         Caption         =   "X"
         Height          =   255
         Left            =   120
         TabIndex        =   30
         Top             =   2640
         Width           =   1335
      End
      Begin VB.Label lblItemInfoName 
         BackStyle       =   0  'Transparent
         Caption         =   "Name"
         Height          =   255
         Left            =   120
         TabIndex        =   29
         Top             =   720
         Width           =   1335
      End
      Begin VB.Label lblItemInfoID 
         BackStyle       =   0  'Transparent
         Caption         =   "ID"
         Height          =   255
         Left            =   120
         TabIndex        =   28
         Top             =   2400
         Width           =   1335
      End
   End
   Begin VB.TextBox txtLoggingOutput 
      Height          =   735
      Left            =   8040
      Locked          =   -1  'True
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   156
      TabStop         =   0   'False
      Top             =   6000
      Visible         =   0   'False
      Width           =   4935
   End
   Begin VB.CommandButton cmdTab 
      Caption         =   "Item Info"
      Enabled         =   0   'False
      Height          =   375
      Index           =   5
      Left            =   5520
      TabIndex        =   13
      Top             =   480
      Width           =   1095
   End
   Begin VB.CommandButton cmdTab 
      Caption         =   "Char Info"
      Enabled         =   0   'False
      Height          =   375
      Index           =   4
      Left            =   4440
      TabIndex        =   12
      Top             =   480
      Width           =   1095
   End
   Begin VB.CommandButton cmdTab 
      Caption         =   "Multis"
      Enabled         =   0   'False
      Height          =   375
      Index           =   3
      Left            =   3360
      TabIndex        =   11
      Top             =   480
      Width           =   1095
   End
   Begin VB.CommandButton cmdTab 
      Caption         =   "Statistics"
      Enabled         =   0   'False
      Height          =   375
      Index           =   2
      Left            =   2280
      TabIndex        =   10
      Top             =   480
      Width           =   1095
   End
   Begin VB.CommandButton cmdTab 
      Caption         =   "Item Errors"
      Enabled         =   0   'False
      Height          =   375
      Index           =   1
      Left            =   1200
      TabIndex        =   9
      Top             =   480
      Width           =   1095
   End
   Begin VB.CommandButton cmdTab 
      Caption         =   "Char Errors"
      Enabled         =   0   'False
      Height          =   375
      Index           =   0
      Left            =   120
      TabIndex        =   8
      Top             =   480
      Width           =   1095
   End
   Begin VB.Frame fraTab 
      Height          =   1335
      Index           =   3
      Left            =   0
      TabIndex        =   5
      Top             =   8160
      Width           =   1095
   End
   Begin MSComctlLib.ImageList imlIcon2 
      Left            =   6000
      Top             =   4680
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   12632256
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   3
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":0000
            Key             =   ""
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":0112
            Key             =   ""
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":0224
            Key             =   ""
         EndProperty
      EndProperty
   End
   Begin MSComctlLib.Toolbar tbToolBar 
      Align           =   1  'Align Top
      Height          =   420
      Left            =   0
      TabIndex        =   1
      Top             =   0
      Width           =   13185
      _ExtentX        =   23257
      _ExtentY        =   741
      ButtonWidth     =   609
      ButtonHeight    =   582
      Appearance      =   1
      ImageList       =   "imlIcon2"
      _Version        =   393216
      BeginProperty Buttons {66833FE8-8583-11D1-B16A-00C0F0283628} 
         NumButtons      =   3
         BeginProperty Button1 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "New"
            Object.ToolTipText     =   "New"
            ImageIndex      =   1
         EndProperty
         BeginProperty Button2 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "Open"
            Object.ToolTipText     =   "Open"
            ImageIndex      =   2
         EndProperty
         BeginProperty Button3 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "Save"
            Object.ToolTipText     =   "Save"
            ImageIndex      =   3
         EndProperty
      EndProperty
      BorderStyle     =   1
   End
   Begin MSComctlLib.StatusBar sbStatusBar 
      Align           =   2  'Align Bottom
      Height          =   255
      Left            =   0
      TabIndex        =   0
      Top             =   6900
      Width           =   13185
      _ExtentX        =   23257
      _ExtentY        =   450
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   3
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Object.Width           =   7408
            MinWidth        =   7408
         EndProperty
         BeginProperty Panel2 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   6
            TextSave        =   "03/10/2001"
         EndProperty
         BeginProperty Panel3 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   5
            TextSave        =   "20:05"
         EndProperty
      EndProperty
   End
   Begin MSComDlg.CommonDialog dlgCommonDialog 
      Left            =   11640
      Top             =   1440
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.Frame fraTab 
      Height          =   3495
      Index           =   2
      Left            =   1680
      TabIndex        =   4
      Top             =   1800
      Width           =   6255
      Begin VB.CommandButton cmdFixLayers 
         Caption         =   "Fix Layers"
         Height          =   375
         Left            =   4680
         TabIndex        =   100
         Top             =   720
         Visible         =   0   'False
         Width           =   1455
      End
      Begin VB.Frame fraStatsItem 
         Caption         =   "Item Related"
         Height          =   1215
         Left            =   240
         TabIndex        =   109
         Top             =   240
         Width           =   4215
         Begin VB.Label lblStatsItemCount 
            Appearance      =   0  'Flat
            BackColor       =   &H80000005&
            BackStyle       =   0  'Transparent
            Caption         =   "Count"
            ForeColor       =   &H80000008&
            Height          =   255
            Left            =   240
            TabIndex        =   115
            Top             =   360
            Width           =   975
         End
         Begin VB.Label lblStatsItemCritError 
            Appearance      =   0  'Flat
            BackColor       =   &H80000005&
            BackStyle       =   0  'Transparent
            Caption         =   "Critical Errors"
            ForeColor       =   &H80000008&
            Height          =   255
            Left            =   240
            TabIndex        =   114
            Top             =   600
            Width           =   975
         End
         Begin VB.Label lblStatsItemErrors 
            Appearance      =   0  'Flat
            BackColor       =   &H80000005&
            BackStyle       =   0  'Transparent
            Caption         =   "Normal Errors"
            ForeColor       =   &H80000008&
            Height          =   255
            Left            =   240
            TabIndex        =   113
            Top             =   840
            Width           =   975
         End
         Begin VB.Label lblStatsItemActualCount 
            BackStyle       =   0  'Transparent
            BorderStyle     =   1  'Fixed Single
            Height          =   255
            Left            =   1440
            TabIndex        =   112
            Top             =   360
            Width           =   1335
         End
         Begin VB.Label lblStatsItemActualCritErrors 
            BackStyle       =   0  'Transparent
            BorderStyle     =   1  'Fixed Single
            Height          =   255
            Left            =   1440
            TabIndex        =   111
            Top             =   600
            Width           =   1335
         End
         Begin VB.Label lblStatsItemActualNormErrors 
            BackStyle       =   0  'Transparent
            BorderStyle     =   1  'Fixed Single
            Height          =   255
            Left            =   1440
            TabIndex        =   110
            Top             =   840
            Width           =   1335
         End
      End
      Begin VB.Frame fraStatsChar 
         Caption         =   "Char Related"
         Height          =   1215
         Left            =   240
         TabIndex        =   102
         Top             =   1560
         Width           =   4215
         Begin VB.Label lblStatsCharNormError 
            Appearance      =   0  'Flat
            BackColor       =   &H80000005&
            BackStyle       =   0  'Transparent
            Caption         =   "Normal Errors"
            ForeColor       =   &H80000008&
            Height          =   255
            Left            =   240
            TabIndex        =   108
            Top             =   840
            Width           =   975
         End
         Begin VB.Label lblStatsCharCritError 
            Appearance      =   0  'Flat
            BackColor       =   &H80000005&
            BackStyle       =   0  'Transparent
            Caption         =   "Critical Errors"
            ForeColor       =   &H80000008&
            Height          =   255
            Left            =   240
            TabIndex        =   107
            Top             =   600
            Width           =   975
         End
         Begin VB.Label lblStatsCharCount 
            Appearance      =   0  'Flat
            BackColor       =   &H80000005&
            BackStyle       =   0  'Transparent
            Caption         =   "Count"
            ForeColor       =   &H80000008&
            Height          =   255
            Left            =   240
            TabIndex        =   106
            Top             =   360
            Width           =   975
         End
         Begin VB.Label lblStatsCharActualCount 
            BackStyle       =   0  'Transparent
            BorderStyle     =   1  'Fixed Single
            Height          =   255
            Left            =   1440
            TabIndex        =   105
            Top             =   360
            Width           =   1335
         End
         Begin VB.Label lblStatsCharActualCritError 
            BackStyle       =   0  'Transparent
            BorderStyle     =   1  'Fixed Single
            Height          =   255
            Left            =   1440
            TabIndex        =   104
            Top             =   600
            Width           =   1335
         End
         Begin VB.Label lblStatsCharActualNormError 
            BackStyle       =   0  'Transparent
            BorderStyle     =   1  'Fixed Single
            Height          =   255
            Left            =   1440
            TabIndex        =   103
            Top             =   840
            Width           =   1335
         End
      End
      Begin VB.CommandButton cmdAnalyzeLayers 
         Caption         =   "Analyze Layers"
         Height          =   375
         Left            =   4680
         TabIndex        =   101
         Top             =   360
         Visible         =   0   'False
         Width           =   1455
      End
      Begin VB.Frame fraMultiStats 
         Caption         =   "Multi Related"
         Height          =   975
         Left            =   240
         TabIndex        =   97
         Top             =   2880
         Width           =   4215
         Begin VB.Label lblMultiCount 
            BackStyle       =   0  'Transparent
            Caption         =   "Count"
            Height          =   255
            Left            =   240
            TabIndex        =   99
            Top             =   240
            Width           =   1095
         End
         Begin VB.Label lblMultiActualCount 
            BorderStyle     =   1  'Fixed Single
            Height          =   255
            Left            =   1440
            TabIndex        =   98
            Top             =   240
            Width           =   1335
         End
      End
   End
   Begin VB.Frame fraTab 
      Height          =   4575
      Index           =   4
      Left            =   3960
      TabIndex        =   6
      Top             =   1320
      Width           =   7575
      Begin VB.ComboBox cmbCharInfo 
         Height          =   315
         Left            =   120
         Style           =   2  'Dropdown List
         TabIndex        =   76
         Top             =   240
         Width           =   2535
      End
      Begin VB.TextBox txtCharInfoName 
         Height          =   285
         Left            =   1560
         TabIndex        =   75
         Top             =   720
         Width           =   1935
      End
      Begin VB.TextBox txtCharInfoSerial 
         Height          =   285
         Left            =   1560
         TabIndex        =   74
         Top             =   960
         Width           =   1935
      End
      Begin VB.TextBox txtCharInfoAccount 
         Height          =   285
         Left            =   1560
         TabIndex        =   73
         Top             =   1200
         Width           =   1935
      End
      Begin VB.TextBox txtCharInfoTitle 
         Height          =   285
         Left            =   1560
         TabIndex        =   72
         Top             =   1440
         Width           =   1935
      End
      Begin VB.TextBox txtCharInfoStrength 
         Height          =   285
         Left            =   1560
         TabIndex        =   71
         Top             =   1680
         Width           =   1935
      End
      Begin VB.TextBox txtCharInfoDexterity 
         Height          =   285
         Left            =   1560
         TabIndex        =   70
         Top             =   1920
         Width           =   1935
      End
      Begin VB.TextBox txtCharInfoIntelligence 
         Height          =   285
         Left            =   1560
         TabIndex        =   69
         Top             =   2160
         Width           =   1935
      End
      Begin VB.TextBox txtCharInfoHealth 
         Height          =   285
         Left            =   1560
         TabIndex        =   68
         Top             =   2400
         Width           =   1935
      End
      Begin VB.TextBox txtCharInfoMana 
         Height          =   285
         Left            =   1560
         TabIndex        =   67
         Top             =   2640
         Width           =   1935
      End
      Begin VB.TextBox txtCharInfoStamina 
         Height          =   285
         Left            =   1560
         TabIndex        =   66
         Top             =   2880
         Width           =   1935
      End
      Begin VB.TextBox txtCharInfoFame 
         Height          =   285
         Left            =   1560
         TabIndex        =   65
         Top             =   3120
         Width           =   1935
      End
      Begin VB.TextBox txtCharInfoKarma 
         Height          =   285
         Left            =   1560
         TabIndex        =   64
         Top             =   3360
         Width           =   1935
      End
      Begin VB.TextBox txtCharInfoBody 
         Height          =   285
         Left            =   1560
         TabIndex        =   63
         Top             =   3600
         Width           =   1935
      End
      Begin VB.TextBox txtCharInfoEmote 
         Height          =   285
         Left            =   1560
         TabIndex        =   62
         Top             =   3840
         Width           =   1935
      End
      Begin VB.TextBox txtCharInfoSpeech 
         Height          =   285
         Left            =   1560
         TabIndex        =   61
         Top             =   4080
         Width           =   1935
      End
      Begin VB.CheckBox chkCharInfoPriv1 
         Caption         =   "GM"
         Height          =   255
         Index           =   0
         Left            =   3600
         TabIndex        =   60
         Top             =   960
         Width           =   1335
      End
      Begin VB.CheckBox chkCharInfoPriv1 
         Caption         =   "Broadcast"
         Height          =   255
         Index           =   1
         Left            =   3600
         TabIndex        =   59
         Top             =   1200
         Width           =   1335
      End
      Begin VB.CheckBox chkCharInfoPriv1 
         Caption         =   "Invulnerable"
         Height          =   255
         Index           =   2
         Left            =   3600
         TabIndex        =   58
         Top             =   1440
         Width           =   1335
      End
      Begin VB.CheckBox chkCharInfoPriv1 
         Caption         =   "Serial Display"
         Height          =   255
         Index           =   3
         Left            =   3600
         TabIndex        =   57
         Top             =   1680
         Width           =   1335
      End
      Begin VB.CheckBox chkCharInfoPriv1 
         Caption         =   "No show skill titles"
         Height          =   255
         Index           =   4
         Left            =   3600
         TabIndex        =   56
         Top             =   1920
         Width           =   1695
      End
      Begin VB.CheckBox chkCharInfoPriv1 
         Caption         =   "GM Pageable"
         Height          =   255
         Index           =   5
         Left            =   3600
         TabIndex        =   55
         Top             =   2160
         Width           =   1335
      End
      Begin VB.CheckBox chkCharInfoPriv1 
         Caption         =   "Can snoop"
         Height          =   255
         Index           =   6
         Left            =   3600
         TabIndex        =   54
         Top             =   2400
         Width           =   1335
      End
      Begin VB.CheckBox chkCharInfoPriv1 
         Caption         =   "Counselor"
         Height          =   255
         Index           =   7
         Left            =   3600
         TabIndex        =   53
         Top             =   2640
         Width           =   1335
      End
      Begin VB.CheckBox chkCharInfoPriv2 
         Caption         =   "All Move"
         Height          =   255
         Index           =   0
         Left            =   5520
         TabIndex        =   52
         Top             =   960
         Width           =   1455
      End
      Begin VB.CheckBox chkCharInfoPriv2 
         Caption         =   "Frozen"
         Height          =   255
         Index           =   1
         Left            =   5520
         TabIndex        =   51
         Top             =   1200
         Width           =   1455
      End
      Begin VB.CheckBox chkCharInfoPriv2 
         Caption         =   "View house as icons"
         Height          =   255
         Index           =   2
         Left            =   5520
         TabIndex        =   50
         Top             =   1440
         Width           =   1815
      End
      Begin VB.CheckBox chkCharInfoPriv2 
         Caption         =   "Perm Hidden"
         Height          =   255
         Index           =   3
         Left            =   5520
         TabIndex        =   49
         Top             =   1680
         Width           =   1455
      End
      Begin VB.CheckBox chkCharInfoPriv2 
         Caption         =   "No mana req"
         Height          =   255
         Index           =   4
         Left            =   5520
         TabIndex        =   48
         Top             =   1920
         Width           =   1455
      End
      Begin VB.CheckBox chkCharInfoPriv2 
         Caption         =   "Dispellable"
         Height          =   255
         Index           =   5
         Left            =   5520
         TabIndex        =   47
         Top             =   2160
         Width           =   1455
      End
      Begin VB.CheckBox chkCharInfoPriv2 
         Caption         =   "Perm Reflect"
         Height          =   255
         Index           =   6
         Left            =   5520
         TabIndex        =   46
         Top             =   2400
         Width           =   1455
      End
      Begin VB.CheckBox chkCharInfoPriv2 
         Caption         =   "No reag req"
         Height          =   255
         Index           =   7
         Left            =   5520
         TabIndex        =   45
         Top             =   2640
         Width           =   1455
      End
      Begin VB.TextBox txtCharInfoX 
         Height          =   285
         Left            =   4320
         TabIndex        =   44
         Top             =   3120
         Width           =   1095
      End
      Begin VB.TextBox txtCharInfoY 
         Height          =   285
         Left            =   4320
         TabIndex        =   43
         Top             =   3360
         Width           =   1095
      End
      Begin VB.TextBox txtCharInfoZ 
         Height          =   285
         Left            =   4320
         TabIndex        =   42
         Top             =   3600
         Width           =   1095
      End
      Begin VB.TextBox txtCharInfoSkin 
         Height          =   285
         Left            =   4320
         TabIndex        =   41
         Top             =   3840
         Width           =   1095
      End
      Begin VB.Label lblCharInfoName 
         BackStyle       =   0  'Transparent
         Caption         =   "Name"
         Height          =   255
         Left            =   120
         TabIndex        =   96
         Top             =   720
         Width           =   1335
      End
      Begin VB.Label lblCharInfoTitle 
         BackStyle       =   0  'Transparent
         Caption         =   "Title"
         Height          =   255
         Left            =   120
         TabIndex        =   95
         Top             =   1440
         Width           =   1335
      End
      Begin VB.Label lblCharInfoStrength 
         BackStyle       =   0  'Transparent
         Caption         =   "Strength"
         Height          =   255
         Left            =   120
         TabIndex        =   94
         Top             =   1680
         Width           =   1335
      End
      Begin VB.Label lblCharInfoDexterity 
         BackStyle       =   0  'Transparent
         Caption         =   "Dexterity"
         Height          =   255
         Left            =   120
         TabIndex        =   93
         Top             =   1920
         Width           =   1335
      End
      Begin VB.Label lblCharInfoIntelligence 
         BackStyle       =   0  'Transparent
         Caption         =   "Intelligence"
         Height          =   255
         Left            =   120
         TabIndex        =   92
         Top             =   2160
         Width           =   1335
      End
      Begin VB.Label lblCharInfoHealth 
         BackStyle       =   0  'Transparent
         Caption         =   "Health"
         Height          =   255
         Left            =   120
         TabIndex        =   91
         Top             =   2400
         Width           =   1335
      End
      Begin VB.Label lblCharInfoMana 
         BackStyle       =   0  'Transparent
         Caption         =   "Mana"
         Height          =   255
         Left            =   120
         TabIndex        =   90
         Top             =   2640
         Width           =   1335
      End
      Begin VB.Label lblCharInfoStamina 
         BackStyle       =   0  'Transparent
         Caption         =   "Stamina"
         Height          =   255
         Left            =   120
         TabIndex        =   89
         Top             =   2880
         Width           =   1335
      End
      Begin VB.Label lblCharInfoFame 
         BackStyle       =   0  'Transparent
         Caption         =   "Fame"
         Height          =   255
         Left            =   120
         TabIndex        =   88
         Top             =   3120
         Width           =   1335
      End
      Begin VB.Label lblCharInfoKarma 
         BackStyle       =   0  'Transparent
         Caption         =   "Karma"
         Height          =   255
         Left            =   120
         TabIndex        =   87
         Top             =   3360
         Width           =   1335
      End
      Begin VB.Label lblCharInfoSerial 
         BackStyle       =   0  'Transparent
         Caption         =   "Serial"
         Height          =   255
         Left            =   120
         TabIndex        =   86
         Top             =   960
         Width           =   1335
      End
      Begin VB.Label lblCharInfoAccount 
         BackStyle       =   0  'Transparent
         Caption         =   "Account"
         Height          =   255
         Left            =   120
         TabIndex        =   85
         Top             =   1200
         Width           =   1335
      End
      Begin VB.Label lblCharInfoBody 
         BackStyle       =   0  'Transparent
         Caption         =   "Body"
         Height          =   255
         Left            =   120
         TabIndex        =   84
         Top             =   3600
         Width           =   1335
      End
      Begin VB.Label lblCharInfoEmote 
         BackStyle       =   0  'Transparent
         Caption         =   "Emote Colour"
         Height          =   255
         Left            =   120
         TabIndex        =   83
         Top             =   3840
         Width           =   1335
      End
      Begin VB.Label lblCharInfoSpeech 
         BackStyle       =   0  'Transparent
         Caption         =   "Speech Colour"
         Height          =   255
         Left            =   120
         TabIndex        =   82
         Top             =   4080
         Width           =   1335
      End
      Begin VB.Label lblCharInfoPrivs 
         BackStyle       =   0  'Transparent
         Caption         =   "Prvilieges"
         Height          =   255
         Left            =   3600
         TabIndex        =   81
         Top             =   720
         Width           =   855
      End
      Begin VB.Label lblCharInfoX 
         BackStyle       =   0  'Transparent
         Caption         =   "X"
         Height          =   255
         Left            =   3600
         TabIndex        =   80
         Top             =   3120
         Width           =   255
      End
      Begin VB.Label lblCharInfoY 
         BackStyle       =   0  'Transparent
         Caption         =   "Y"
         Height          =   255
         Left            =   3600
         TabIndex        =   79
         Top             =   3360
         Width           =   255
      End
      Begin VB.Label lblCharInfoZ 
         BackStyle       =   0  'Transparent
         Caption         =   "Z"
         Height          =   255
         Left            =   3600
         TabIndex        =   78
         Top             =   3600
         Width           =   255
      End
      Begin VB.Label lblCharInfoSkin 
         BackStyle       =   0  'Transparent
         Caption         =   "Skin"
         Height          =   255
         Left            =   3600
         TabIndex        =   77
         Top             =   3840
         Width           =   615
      End
   End
   Begin VB.Frame fraTab 
      Height          =   4695
      Index           =   0
      Left            =   120
      TabIndex        =   2
      Top             =   960
      Visible         =   0   'False
      Width           =   8175
      Begin VB.CommandButton cmdCharErrorsFixStatBug 
         Caption         =   "Fix char stat bugs"
         Height          =   255
         Left            =   120
         TabIndex        =   185
         Top             =   1560
         Width           =   2535
      End
      Begin VB.Frame Frame1 
         Caption         =   "Stat Issues"
         Height          =   2055
         Left            =   3960
         TabIndex        =   169
         Top             =   240
         Width           =   4095
         Begin VB.CheckBox chkCharStatIssue 
            Caption         =   "Dexterity"
            Height          =   255
            Index           =   0
            Left            =   120
            TabIndex        =   183
            Top             =   240
            Width           =   1455
         End
         Begin VB.CheckBox chkCharStatIssue 
            Caption         =   "Intelligence"
            Height          =   255
            Index           =   1
            Left            =   120
            TabIndex        =   182
            Top             =   480
            Width           =   1455
         End
         Begin VB.CheckBox chkCharStatIssue 
            Caption         =   "Strength"
            Height          =   255
            Index           =   2
            Left            =   120
            TabIndex        =   181
            Top             =   720
            Width           =   1455
         End
         Begin VB.CheckBox chkCharStatIssue 
            Caption         =   "Stamina"
            Height          =   255
            Index           =   3
            Left            =   120
            TabIndex        =   180
            Top             =   960
            Width           =   1455
         End
         Begin VB.CheckBox chkCharStatIssue 
            Caption         =   "Mana"
            Height          =   255
            Index           =   4
            Left            =   120
            TabIndex        =   179
            Top             =   1200
            Width           =   1455
         End
         Begin VB.CheckBox chkCharStatIssue 
            Caption         =   "Health"
            Height          =   255
            Index           =   5
            Left            =   120
            TabIndex        =   178
            Top             =   1440
            Width           =   1455
         End
         Begin VB.CheckBox chkCharStatIssue 
            Caption         =   "Max Health"
            Height          =   255
            Index           =   6
            Left            =   120
            TabIndex        =   177
            Top             =   1680
            Width           =   1455
         End
         Begin VB.TextBox txtCharStatIssue 
            Height          =   285
            Index           =   0
            Left            =   1800
            TabIndex        =   176
            Top             =   240
            Width           =   1455
         End
         Begin VB.TextBox txtCharStatIssue 
            Height          =   285
            Index           =   1
            Left            =   1800
            TabIndex        =   175
            Top             =   480
            Width           =   1455
         End
         Begin VB.TextBox txtCharStatIssue 
            Height          =   285
            Index           =   2
            Left            =   1800
            TabIndex        =   174
            Top             =   720
            Width           =   1455
         End
         Begin VB.TextBox txtCharStatIssue 
            Height          =   285
            Index           =   3
            Left            =   1800
            TabIndex        =   173
            Top             =   960
            Width           =   1455
         End
         Begin VB.TextBox txtCharStatIssue 
            Height          =   285
            Index           =   4
            Left            =   1800
            TabIndex        =   172
            Top             =   1200
            Width           =   1455
         End
         Begin VB.TextBox txtCharStatIssue 
            Height          =   285
            Index           =   5
            Left            =   1800
            TabIndex        =   171
            Top             =   1440
            Width           =   1455
         End
         Begin VB.TextBox txtCharStatIssue 
            Height          =   285
            Index           =   6
            Left            =   1800
            TabIndex        =   170
            Top             =   1680
            Width           =   1455
         End
      End
      Begin VB.CheckBox chkCharErrorLoc 
         Caption         =   "Z"
         Height          =   255
         Index           =   2
         Left            =   120
         TabIndex        =   155
         Top             =   1200
         Width           =   855
      End
      Begin VB.CheckBox chkCharErrorLoc 
         Caption         =   "Y"
         Height          =   255
         Index           =   1
         Left            =   120
         TabIndex        =   154
         Top             =   960
         Width           =   855
      End
      Begin VB.CheckBox chkCharErrorLoc 
         Caption         =   "X"
         Height          =   255
         Index           =   0
         Left            =   120
         TabIndex        =   153
         Top             =   720
         Width           =   855
      End
      Begin VB.TextBox txtCharErrorLoc 
         Height          =   285
         Index           =   2
         Left            =   1080
         TabIndex        =   152
         Top             =   1200
         Width           =   1215
      End
      Begin VB.TextBox txtCharErrorLoc 
         Height          =   285
         Index           =   1
         Left            =   1080
         TabIndex        =   151
         Top             =   960
         Width           =   1215
      End
      Begin VB.TextBox txtCharErrorLoc 
         Height          =   285
         Index           =   0
         Left            =   1080
         TabIndex        =   150
         Top             =   720
         Width           =   1215
      End
      Begin VB.ComboBox cmbCharErrors 
         Height          =   315
         Left            =   120
         Style           =   2  'Dropdown List
         TabIndex        =   118
         Top             =   240
         Width           =   3495
      End
      Begin VB.CommandButton cmdCharFixLayer 
         Caption         =   "Fix Layers"
         Enabled         =   0   'False
         Height          =   315
         Left            =   3720
         TabIndex        =   116
         Top             =   240
         Visible         =   0   'False
         Width           =   1575
      End
      Begin MSFlexGridLib.MSFlexGrid flxCharLayer 
         Height          =   2055
         Left            =   120
         TabIndex        =   117
         Top             =   2520
         Width           =   7695
         _ExtentX        =   13573
         _ExtentY        =   3625
         _Version        =   393216
         Cols            =   5
         ScrollBars      =   2
      End
   End
   Begin VB.Image imgTest 
      Height          =   255
      Left            =   240
      Top             =   5160
      Visible         =   0   'False
      Width           =   615
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuFileNew 
         Caption         =   "&New"
         Enabled         =   0   'False
         Shortcut        =   ^N
      End
      Begin VB.Menu mnuFileOpen 
         Caption         =   "&Open"
         Shortcut        =   ^O
      End
      Begin VB.Menu mnuFileClose 
         Caption         =   "&Close"
         Enabled         =   0   'False
      End
      Begin VB.Menu mnuFileBar1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuFileSave 
         Caption         =   "&Save"
         Enabled         =   0   'False
         Shortcut        =   ^S
      End
      Begin VB.Menu mnuFileSaveAs 
         Caption         =   "Save &As..."
         Enabled         =   0   'False
      End
      Begin VB.Menu mnuFileSaveAll 
         Caption         =   "Save A&ll"
         Enabled         =   0   'False
      End
      Begin VB.Menu mnuFileBar2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuFileProperties 
         Caption         =   "Propert&ies"
         Enabled         =   0   'False
      End
      Begin VB.Menu mnuFileBar3 
         Caption         =   "-"
      End
      Begin VB.Menu mnuFilePageSetup 
         Caption         =   "Page Set&up..."
         Enabled         =   0   'False
      End
      Begin VB.Menu mnuFilePrintPreview 
         Caption         =   "Print Pre&view"
         Enabled         =   0   'False
      End
      Begin VB.Menu mnuFilePrint 
         Caption         =   "&Print..."
         Enabled         =   0   'False
         Shortcut        =   ^P
      End
      Begin VB.Menu mnuFileBar4 
         Caption         =   "-"
      End
      Begin VB.Menu mnuFileSend 
         Caption         =   "Sen&d..."
         Enabled         =   0   'False
      End
      Begin VB.Menu mnuFileBar5 
         Caption         =   "-"
      End
      Begin VB.Menu mnuFileMRU 
         Caption         =   ""
         Index           =   0
         Visible         =   0   'False
      End
      Begin VB.Menu mnuFileMRU 
         Caption         =   ""
         Index           =   1
         Visible         =   0   'False
      End
      Begin VB.Menu mnuFileMRU 
         Caption         =   ""
         Index           =   2
         Visible         =   0   'False
      End
      Begin VB.Menu mnuFileMRU 
         Caption         =   ""
         Index           =   3
         Visible         =   0   'False
      End
      Begin VB.Menu mnuFileBar6 
         Caption         =   "-"
         Visible         =   0   'False
      End
      Begin VB.Menu mnuFileExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu mnuOptions 
      Caption         =   "&Options"
      Begin VB.Menu mnuOptionsLargeWorld 
         Caption         =   "&Large World"
      End
      Begin VB.Menu mnuOptionsThresholds 
         Caption         =   "&Thresholds"
         Enabled         =   0   'False
      End
      Begin VB.Menu mnuOptionsContLocations 
         Caption         =   "&Container Locations"
      End
   End
   Begin VB.Menu mnuLogging 
      Caption         =   "&Logging"
      Begin VB.Menu mnuLoggingMode 
         Caption         =   "&Off"
         Checked         =   -1  'True
         Index           =   0
      End
      Begin VB.Menu mnuLoggingMode 
         Caption         =   "&Text Window"
         Index           =   1
      End
      Begin VB.Menu mnuLoggingMode 
         Caption         =   "&File"
         Index           =   2
      End
   End
   Begin VB.Menu mnuHelp 
      Caption         =   "&Help"
      Begin VB.Menu mnuHelpContents 
         Caption         =   "&Contents"
      End
      Begin VB.Menu mnuHelpSearch 
         Caption         =   "&Search For Help On..."
      End
      Begin VB.Menu mnuHelpBar1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuHelpAbout 
         Caption         =   "&About UOXWorldRepairer..."
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
Private dMode As Long

Private Sub cmbCharErrors_Click()
    flxCharLayer.Clear
    flxCharLayer.FormatString = "<Layer |<Serial   |<Name   "
    Dim i As Long
    For i = 0 To 4
        flxCharLayer.ColWidth(i) = flxCharLayer.Width / 5
    Next i
    Dim ourIndex As Long
    Dim errorCount As Long
    ourIndex = cmbCharErrors.ListIndex
    errorCount = 0
    Dim charTest As Long
    Dim layerCounter As Long
    charTest = CharErrors(ourIndex).Index
    If CharErrors(ourIndex).Layer Then
        For layerCounter = 0 To MaxLayers
            If UBound(Chars(charTest).Layers(layerCounter).Contents) > 1 Then
                errorCount = errorCount + UBound(Chars(charTest).Layers(layerCounter).Contents)
            End If
            If UBound(Chars(charTest).Layers(layerCounter).Contents) = 1 Then
                If Chars(charTest).Layers(layerCounter).Contents(0) <> -1 And Chars(charTest).Layers(layerCounter).Contents(1) <> -1 Then
                    errorCount = errorCount + 2
                End If
            End If
        Next layerCounter
        flxCharLayer.Rows = flxCharLayer.FixedRows + errorCount
        Dim currentRow As Long
        currentRow = flxCharLayer.FixedRows
        Dim layerCount As Long
        For layerCounter = 0 To MaxLayers
            If UBound(Chars(charTest).Layers(layerCounter).Contents) > 1 Then
                For layerCount = 0 To UBound(Chars(charTest).Layers(layerCounter).Contents)
                    If Chars(charTest).Layers(layerCounter).Contents(layerCount) <> -1 Then
                        flxCharLayer.Row = currentRow
                        flxCharLayer.Col = 0
                        flxCharLayer.Text = layerCounter
                        flxCharLayer.Col = 1
                        flxCharLayer.Text = CStr(Items(Chars(charTest).Layers(layerCounter).Contents(layerCount)).Serial)
                        flxCharLayer.Col = 2
                        flxCharLayer.Text = Items(Chars(charTest).Layers(layerCounter).Contents(layerCount)).Name
                        currentRow = currentRow + 1
                    End If
                Next layerCount
            End If
            If UBound(Chars(charTest).Layers(layerCounter).Contents) = 1 Then
                If Chars(charTest).Layers(layerCounter).Contents(0) <> -1 And Chars(charTest).Layers(layerCounter).Contents(1) <> -1 Then
                    errorCount = errorCount + 1
                End If
            End If
        Next layerCounter
    End If
    Dim iCounter As Long
    If CharErrors(ourIndex).Location <> 0 Then
        For iCounter = 0 To 2
            If CharErrors(ourIndex).Location And (2 ^ iCounter) Then
                chkCharErrorLoc(iCounter).Value = vbChecked
                txtCharErrorLoc(iCounter).Text = CharErrors(ourIndex).BadLocation(iCounter)
            Else
                chkCharErrorLoc(iCounter).Value = vbUnchecked
                txtCharErrorLoc(iCounter).Text = ""
            End If
        Next iCounter
    End If
    Dim sCounter As Long
    If CharErrors(ourIndex).Stats <> 0 Then
        For sCounter = 0 To 6
            If CharErrors(ourIndex).Stats And (2 ^ sCounter) Then
                chkCharStatIssue(sCounter).Value = vbChecked
                txtCharStatIssue(sCounter).Text = CStr(CharErrors(ourIndex).BadStats(sCounter))
            Else
                chkCharStatIssue(sCounter).Value = vbUnchecked
                txtCharStatIssue(sCounter).Text = ""
            End If
        Next sCounter
    Else
        For sCounter = 0 To 6
            chkCharStatIssue(sCounter).Value = vbUnchecked
            txtCharStatIssue(sCounter).Text = ""
        Next sCounter
    End If
End Sub

Private Sub cmbCharInfo_Click()
    With Chars(cmbCharInfo.ListIndex)
        txtCharInfoName.Text = .Name
        txtCharInfoTitle.Text = .Title
        txtCharInfoHealth.Text = .HitPoints
        txtCharInfoStamina.Text = .Stamina
        txtCharInfoMana.Text = .Mana
        txtCharInfoStrength.Text = .Strength
        txtCharInfoDexterity.Text = .Dexterity
        txtCharInfoIntelligence.Text = .Intelligence
        txtCharInfoFame.Text = .Fame
        txtCharInfoKarma.Text = .Karma
        txtCharInfoSerial.Text = .Serial
        txtCharInfoAccount.Text = .Account
        txtCharInfoBody.Text = .Body
        txtCharInfoEmote.Text = .Emotecolour
        txtCharInfoSpeech.Text = .Speechcolour
        txtCharInfoX.Text = .x
        txtCharInfoY.Text = .y
        txtCharInfoZ.Text = .z
        txtCharInfoSkin.Text = .Skin
        Dim i As Long
        For i = 0 To 7
            If .Priv1 And (2 ^ i) Then
                chkCharInfoPriv1(i).Value = vbChecked
            Else
                chkCharInfoPriv1(i).Value = vbUnchecked
            End If
            If .priv2 And (2 ^ i) Then
                chkCharInfoPriv2(i).Value = vbChecked
            Else
                chkCharInfoPriv2(i).Value = vbUnchecked
            End If
        Next i
    End With
End Sub

Private Sub cmbItemError_Click()
    Dim iIndex As Long
    iIndex = cmbItemError.ListIndex
    If ItemErrors(iIndex).Container Then
        chkItemErrorContainer.Value = vbChecked
        txtItemErrorContainer.Text = CStr(Items(ItemErrors(iIndex).Index).Container)
    Else
        chkItemErrorContainer.Value = vbUnchecked
        txtItemErrorContainer.Text = ""
    End If
    Dim sCounter As Long
    If ItemErrors(iIndex).Stats <> 0 Then
        For sCounter = 0 To 6
            If ItemErrors(iIndex).Stats And (2 ^ sCounter) Then
                chkItemStatIssue(sCounter).Value = vbChecked
                txtItemStatIssue(sCounter).Text = CStr(ItemErrors(iIndex).BadStats(sCounter))
            Else
                chkItemStatIssue(sCounter).Value = vbUnchecked
                txtItemStatIssue(sCounter).Text = ""
            End If
        Next sCounter
    Else
        For sCounter = 0 To 6
            chkItemStatIssue(sCounter).Value = vbUnchecked
            txtItemStatIssue(sCounter).Text = ""
        Next sCounter
    End If
    If ItemErrors(iIndex).Location <> 0 Then
        For sCounter = 0 To 2
            If ItemErrors(iIndex).Location And (2 ^ sCounter) Then
                chkItemErrorLocation(sCounter).Value = vbChecked
                txtItemErrorLocation(sCounter).Text = CStr(ItemErrors(iIndex).BadLocations(sCounter))
            Else
                chkItemErrorLocation(sCounter).Value = vbUnchecked
                txtItemErrorLocation(sCounter).Text = ""
            End If
        Next sCounter
    Else
        For sCounter = 0 To 2
            chkItemErrorLocation(sCounter).Value = vbUnchecked
            txtItemErrorLocation(sCounter).Text = ""
        Next sCounter
    End If
    If ItemErrors(iIndex).Colour Then
        chkItemErrorColour.Value = vbChecked
        txtItemErrorColour.Text = CStr(ItemErrors(iIndex).BadColour)
    Else
        chkItemErrorColour.Value = vbUnchecked
        txtItemErrorColour.Text = ""
    End If
    If ItemErrors(iIndex).Owner Then
        chkItemErrorOwner.Value = vbChecked
        txtItemErrorOwner.Text = CStr(Items(ItemErrors(iIndex).Index).Owner)
    Else
        chkItemErrorOwner.Value = vbUnchecked
        txtItemErrorOwner.Text = ""
    End If
    If ItemErrors(iIndex).Spawner Then
        chkItemErrorSpawner.Value = vbChecked
        txtItemErrorSpawner.Text = CStr(Items(ItemErrors(iIndex).Index).Spawn)
    Else
        chkItemErrorSpawner.Value = vbUnchecked
        txtItemErrorSpawner.Text = ""
    End If
End Sub
Private Sub cmbItemInfo_Click()
    Dim baseNum As Long
    baseNum = BlockSize * cmbItemInfoItemBlock.ListIndex
    With Items(cmbItemInfo.ListIndex + baseNum)
        txtItemInfoName.Text = .Name
        txtItemInfoHealth.Text = .HitPoints
        txtItemInfoMaxHealth.Text = .MaxHealth
        txtItemInfoStrength.Text = .Strength
        txtItemInfoDexterity.Text = .Dexterity
        txtItemInfoIntelligence.Text = .Intelligence
        txtItemInfoSerial.Text = .Serial
        txtItemInfoID.Text = .ID
        txtItemInfoX.Text = .x
        txtItemInfoY.Text = .y
        txtItemInfoZ.Text = .z
        txtItemInfoColour.Text = .Colour
        txtItemInfoLayer.Text = .Layer
        txtItemInfoOwner.Text = .Owner
        txtItemInfoSpawn.Text = .Spawn
        txtItemInfoContainer.Text = .Container
        txtItemInfoType.Text = .iType
    End With
End Sub

Private Sub cmbItemInfoItemBlock_Click()
    Dim baseNum As Long, nextNum As Long, foundPos As Long
    baseNum = BlockSize * cmbItemInfoItemBlock.ListIndex
    If UBound(Items) < baseNum + BlockSize Then
        nextNum = UBound(Items)
    Else
        nextNum = baseNum + BlockSize
    End If
    cmbItemInfo.Clear
    Dim onePercent As Long
    onePercent = (nextNum - baseNum) \ 100
    If onePercent = 0 Then
        onePercent = 1
    End If
    For foundPos = baseNum To nextNum
        If (foundPos - baseNum) Mod onePercent = 0 Then
            sbStatusBar.Panels(1).Text = "Filling block... " & (foundPos - baseNum) \ onePercent & "% done"
        End If
        cmbItemInfo.AddItem (CStr(Items(foundPos).Serial) & " - " & Items(foundPos).Name), foundPos - baseNum
    Next foundPos
    sbStatusBar.Panels(1).Text = ""
End Sub


Private Sub cmdCharErrorsFixStatBug_Click()
    
    Dim iUpperBound As Long, counter As Long, iEntry As Long, onePercent As Long, sCounter As Long
    If cmbItemError.ListCount = 0 Then
        Exit Sub
    End If
    
    iUpperBound = UBound(Chars)
    onePercent = iUpperBound \ 100
    For counter = 0 To iUpperBound
        If (counter Mod onePercent) = 0 Then
            sbStatusBar.Panels(1).Text = "Fixing... " & counter \ onePercent & "% done"
        End If
        If Chars(counter).Dexterity < 0 Then
            Chars(counter).Dexterity = 0
            Chars(counter).Fixed = True
        ElseIf Chars(counter).Dexterity > 30000 Then
            Chars(counter).Dexterity = 30000
            Chars(counter).Fixed = True
        End If
        If Chars(counter).Intelligence < 0 Then
            Chars(counter).Intelligence = 0
            Chars(counter).Fixed = True
        ElseIf Chars(counter).Intelligence > 30000 Then
            Chars(counter).Intelligence = 30000
            Chars(counter).Fixed = True
        End If
        If Chars(counter).Strength < 0 Then
            Chars(counter).Strength = 0
            Chars(counter).Fixed = True
        ElseIf Chars(counter).Strength > 30000 Then
            Chars(counter).Strength = 30000
            Chars(counter).Fixed = True
        End If
        If Chars(counter).Stamina < 0 Then
            Chars(counter).Stamina = 0
            Chars(counter).Fixed = True
        ElseIf Chars(counter).Stamina > 30000 Then
            Chars(counter).Stamina = 30000
            Chars(counter).Fixed = True
        End If
        If Chars(counter).Mana < 0 Then
            Chars(counter).Mana = 0
            Chars(counter).Fixed = True
        ElseIf Chars(counter).Mana > 30000 Then
            Chars(counter).Mana = 30000
            Chars(counter).Fixed = True
        End If
        If Chars(counter).HitPoints < 0 Then
            Chars(counter).HitPoints = 0
            Chars(counter).Fixed = True
        ElseIf Chars(counter).HitPoints > 30000 Then
            Chars(counter).HitPoints = 30000
            Chars(counter).Fixed = True
        End If
        If Chars(counter).Dexterity2 < 0 Then
            Chars(counter).Dexterity2 = 0
            Chars(counter).Fixed = True
        ElseIf Chars(counter).Dexterity2 > 30000 Then
            Chars(counter).Dexterity2 = 30000
            Chars(counter).Fixed = True
        End If
        If Chars(counter).Strength2 < 0 Then
            Chars(counter).Strength2 = 0
            Chars(counter).Fixed = True
        ElseIf Chars(counter).Strength2 > 30000 Then
            Chars(counter).Strength2 = 30000
            Chars(counter).Fixed = True
        End If
        If Chars(counter).Intelligence2 < 0 Then
            Chars(counter).Intelligence2 = 0
            Chars(counter).Fixed = True
        ElseIf Chars(counter).Intelligence2 > 30000 Then
            Chars(counter).Intelligence2 = 30000
            Chars(counter).Fixed = True
        End If
    Next counter
    sbStatusBar.Panels(1).Text = "Stats fixed"

End Sub

Private Sub cmdItemErrorFixBadOwners_Click()

    Dim iUpperBound As Long, counter As Long, iEntry As Long, onePercent As Long
    If cmbItemError.ListCount = 0 Then
        Exit Sub
    End If
    
    iUpperBound = UBound(ItemErrors)
    onePercent = iUpperBound \ 100
    For counter = 0 To iUpperBound
        If (counter Mod onePercent) = 0 Then
            sbStatusBar.Panels(1).Text = "Fixing... " & counter \ onePercent & "% done"
        End If
        If ItemErrors(counter).Owner Then
            ItemErrors(counter).Owner = False
            iEntry = ItemErrors(counter).Index
            Items(iEntry).Owner = -1
            Items(iEntry).Fixed = True
        End If
    Next counter
    sbStatusBar.Panels(1).Text = "Owners reset"

End Sub

Private Sub cmdItemErrorFixContainers_Click()

    Dim iUpperBound As Long, counter As Long, iEntry As Long, onePercent As Long
    If cmbItemError.ListCount = 0 Then
        Exit Sub
    End If
    
    iUpperBound = UBound(ItemErrors)
    onePercent = iUpperBound \ 100
    For counter = 0 To iUpperBound
        If (counter Mod onePercent) = 0 Then
            sbStatusBar.Panels(1).Text = "Fixing... " & counter \ onePercent & "% done"
        End If
        If ItemErrors(counter).Container Then
            ItemErrors(counter).Container = False
            iEntry = ItemErrors(counter).Index
            Items(iEntry).Container = -1
            Items(iEntry).x = ContainerLocation.x
            Items(iEntry).y = ContainerLocation.y
            Items(iEntry).z = ContainerLocation.z
            Items(iEntry).Fixed = True
        End If
    Next counter
    sbStatusBar.Panels(1).Text = "Containers fixed"
End Sub

Private Sub cmdItemErrorFixStatBugs_Click()

    Dim iUpperBound As Long, counter As Long, iEntry As Long, onePercent As Long, sCounter As Long
    If cmbItemError.ListCount = 0 Then
        Exit Sub
    End If
    
    iUpperBound = UBound(Items)
    onePercent = iUpperBound \ 100
    For counter = 0 To iUpperBound
        If (counter Mod onePercent) = 0 Then
            sbStatusBar.Panels(1).Text = "Fixing... " & counter \ onePercent & "% done"
        End If
        If Items(counter).Dexterity < 0 Then
            Items(counter).Dexterity = 0
            Items(counter).Fixed = True
        ElseIf Items(counter).Dexterity > 30000 Then
            Items(counter).Dexterity = 30000
            Items(counter).Fixed = True
        End If
        If Items(counter).Intelligence < 0 Then
            Items(counter).Intelligence = 0
            Items(counter).Fixed = True
        ElseIf Items(counter).Intelligence > 30000 Then
            Items(counter).Intelligence = 30000
            Items(counter).Fixed = True
        End If
        If Items(counter).Strength < 0 Then
            Items(counter).Strength = 0
            Items(counter).Fixed = True
        ElseIf Items(counter).Strength > 30000 Then
            Items(counter).Strength = 30000
            Items(counter).Fixed = True
        End If
        If Items(counter).Stamina < 0 Then
            Items(counter).Stamina = 0
            Items(counter).Fixed = True
        ElseIf Items(counter).Stamina > 30000 Then
            Items(counter).Stamina = 30000
            Items(counter).Fixed = True
        End If
        If Items(counter).Mana < 0 Then
            Items(counter).Mana = 0
            Items(counter).Fixed = True
        ElseIf Items(counter).Mana > 30000 Then
            Items(counter).Mana = 30000
            Items(counter).Fixed = True
        End If
        If Items(counter).HitPoints < 0 Then
            Items(counter).HitPoints = 0
            Items(counter).Fixed = True
        ElseIf Items(counter).HitPoints > 30000 Then
            Items(counter).HitPoints = 30000
            Items(counter).Fixed = True
        End If
        If Items(counter).MaxHealth < 0 Then
            Items(counter).MaxHealth = 0
            Items(counter).Fixed = True
        ElseIf Items(counter).MaxHealth > 30000 Then
            Items(counter).MaxHealth = 30000
            Items(counter).Fixed = True
        End If
    Next counter
    sbStatusBar.Panels(1).Text = "Stats fixed"
    
End Sub

Private Sub cmdTab_Click(Index As Integer)
    fraTab(Index).Visible = True
    Dim i As Long
    For i = 0 To cmdTab.UBound
        If i <> Index Then
            fraTab(i).Visible = False
        End If
    Next i
End Sub
Private Sub LoadConfigData()

    left = GetSetting(App.Title, "Settings", "MainLeft", 1000)
    Top = GetSetting(App.Title, "Settings", "MainTop", 1000)
    Width = GetSetting(App.Title, "Settings", "MainWidth", 6500)
    Height = GetSetting(App.Title, "Settings", "MainHeight", 6500)
    
    ContainerLocation.x = GetSetting(App.Title, "Options", "ContainerX", 5900)
    ContainerLocation.y = GetSetting(App.Title, "Options", "ContainerY", 2000)
    ContainerLocation.z = GetSetting(App.Title, "Options", "ContainerZ", 0)

End Sub

Private Sub Form_Load()
    LoadConfigData
    Caption = App.ProductName & " " & App.Major & "." & App.Minor & " Release " & App.Revision
    Dim j As Long
    For j = 1 To fraTab.UBound
        fraTab(j).left = fraTab(0).left
        fraTab(j).Top = fraTab(0).Top
        fraTab(j).Height = fraTab(0).Height
        fraTab(j).Width = fraTab(0).Width
        fraTab(j).Visible = False
    Next j
    itemsOpen = False
    charsOpen = False
    sbStatusBar.Panels(1).Width = 2 * (Width / 3)
    sbStatusBar.Panels(2).Width = Width / 6
    sbStatusBar.Panels(3).Width = Width / 6
    flxCharLayer.Clear
    flxCharLayer.FormatString = "<Layer |<Serial   |<Name   "
    Dim i As Long
    For i = 0 To 4
        flxCharLayer.ColWidth(i) = flxCharLayer.Width / 5
    Next i
    lblItemInfoItemBlock.Visible = False
    cmbItemInfoItemBlock.Visible = False
    LoadFormSkinStruct Me, skinMain, imgTest
End Sub

Private Sub Form_Resize()
    
    sbStatusBar.Panels(1).Width = 2 * (Width / 3)
    sbStatusBar.Panels(2).Width = Width / 6
    sbStatusBar.Panels(3).Width = Width / 6
    
    txtLoggingOutput.left = 0
    txtLoggingOutput.Top = sbStatusBar.Top - txtLoggingOutput.Height
    txtLoggingOutput.Width = Width - 50

End Sub

Private Sub Form_Unload(Cancel As Integer)
    Dim i As Integer

    'close all sub forms
    For i = Forms.Count - 1 To 1 Step -1
        Unload Forms(i)
    Next
    If Me.WindowState <> vbMinimized Then
        SaveSetting App.Title, "Settings", "MainLeft", Me.left
        SaveSetting App.Title, "Settings", "MainTop", Me.Top
        SaveSetting App.Title, "Settings", "MainWidth", Me.Width
        SaveSetting App.Title, "Settings", "MainHeight", Me.Height
    End If
    If charsOpen Then
        Close charsWsc
    End If
    If itemsOpen Then
        Close itemsWsc
    End If
End Sub

Private Sub mnuHelpAbout_Click()
    frmAbout.Show vbModal, Me
End Sub

Private Sub mnuLoggingMode_Click(Index As Integer)
    Select Case Index
    Case 0
        mnuLoggingMode(1).Checked = False
        mnuLoggingMode(2).Checked = False
        txtLoggingOutput.Visible = False
        Close logFile
    Case 1
        mnuLoggingMode(0).Checked = False
        mnuLoggingMode(2).Checked = False
        txtLoggingOutput.Visible = True
        Close logFile
    Case 2
        mnuLoggingMode(0).Checked = False
        mnuLoggingMode(1).Checked = False
        txtLoggingOutput.Visible = False
        logFile = FreeFile
        Open App.Path & "\logoutput.txt" For Output As logFile
    End Select
    mnuLoggingMode(Index).Checked = True
    dMode = Index
End Sub

Private Sub mnuOptionsContLocations_Click()

    frmOptions.Show vbModal, Me
    
End Sub

Private Sub mnuOptionsLargeWorld_Click()
    On Local Error GoTo errorhandler
    If mnuOptionsLargeWorld.Checked Then
        If UBound(Items) > 10000 Then
            MsgBox "Cannot turn off large world for such a big world"
            Exit Sub
        End If
    End If
    mnuOptionsLargeWorld.Checked = Not mnuOptionsLargeWorld.Checked
    cmbItemInfo.Clear
    cmbItemInfoItemBlock.Clear
    Dim foundPos As Long
    If mnuOptionsLargeWorld.Checked Then
        cmbItemInfoItemBlock.Visible = True
        lblItemInfoItemBlock.Visible = True
        For foundPos = 0 To (UBound(Items) / BlockSize)
            cmbItemInfoItemBlock.AddItem "Block " & foundPos
        Next foundPos
    Else
        cmbItemInfoItemBlock.Visible = False
        lblItemInfoItemBlock.Visible = False
        For foundPos = 0 To UBound(Items)
            cmbItemInfo.AddItem (CStr(Items(foundPos).Serial) & " - " & Items(foundPos).Name), foundPos
        Next foundPos
    End If
    Exit Sub
errorhandler:
    If Err.Number = 9 Then
        cmbItemInfoItemBlock.Visible = mnuOptionsLargeWorld.Checked
        lblItemInfoItemBlock.Visible = mnuOptionsLargeWorld.Checked
    Else
        MsgBox Err.Number & ": " & Err.Description
    End If
End Sub

Private Sub mnuOptionsThresholds_Click()

    MsgBox "All current stat thresholds are set at 65535", vbInformation + vbOKOnly
End Sub

Private Sub tbToolBar_ButtonClick(ByVal Button As MSComctlLib.Button)

    Select Case Button.Key
        Case "New"
            mnuFileNew_Click
        Case "Open"
            mnuFileOpen_Click
        Case "Save"
            mnuFileSave_Click
        Case "Print"
            mnuFilePrint_Click
    End Select
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
Private Sub mnuHelpSearch_Click()
    
    Dim nRet As Integer

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
Private Sub mnuFileOpen_Click()
    Dim sFile As String

    With dlgCommonDialog
        .Filter = "UOX World Files (*.wsc)|*.wsc"
        .ShowOpen
        If Len(.FileName) = 0 Then
            Exit Sub
        End If
        sFile = .FileName
    End With
    Dim FileName As String
    Dim filePath As String
    Dim lastSlash As Long
    lastSlash = GetLastSlash(sFile)
    FileName = right$(sFile, Len(sFile) - lastSlash)
    filePath = left$(sFile, lastSlash)
    
    itemsWsc = FreeFile
    charsWscString = filePath & "chars.wsc"
    itemsWscString = filePath & "items.wsc"
    Open itemsWscString For Input As itemsWsc
    charsWsc = FreeFile
    Open charsWscString For Input As charsWsc
    
    Dim sLine As String
    Dim initFound As Boolean
    initFound = False
    Dim foundPos As Long
    Dim targetNum As Long
    Dim targetString As String
    Dim nextPos As Long
    Dim uCharsBound As Long, uItemsBound As Long
    
    ' okay, we're subtracting 2 from INITMEM for a reason
    ' first reason is that INITMEM = char or itemcount +1
    ' so we have to reduce by 1, because we only want to read the right amount
    ' second of all, the targetnum is 1 based
    ' our array is 0 based!
    ' so we're 1 above again already
    
    Do While Not initFound
        Line Input #itemsWsc, sLine
        foundPos = InStr(1, sLine, "INITMEM")
        If (foundPos <> 0 And Not IsNull(foundPos)) Then
            foundPos = InStr(foundPos + 1, sLine, " ")
            nextPos = InStr(foundPos + 1, sLine, " ")
            targetString = Mid$(sLine, foundPos, nextPos - foundPos)
            targetNum = CLng(targetString)
            initFound = True
            ReDim Preserve Items(0 To targetNum - 2)
        End If
    Loop
    
    initFound = False
    Do While Not initFound
        Line Input #charsWsc, sLine
        foundPos = InStr(1, sLine, "INITMEM")
        If (foundPos <> 0 And Not IsNull(foundPos)) Then
            foundPos = InStr(foundPos + 1, sLine, " ")
            nextPos = InStr(foundPos + 1, sLine, " ")
            targetString = Mid$(sLine, foundPos, nextPos - foundPos)
            targetNum = CLng(targetString)
            initFound = True
            ReDim Preserve Chars(0 To targetNum - 2)
        End If
    Loop
    
    Screen.MousePointer = vbHourglass
    DoEvents
    LoadChars sbStatusBar, txtLoggingOutput, dMode, mnuFileOpen, lblStatsCharActualCount
    sbStatusBar.Panels(1).Text = "Sorting Chars"
    heapsortchars Chars, sbStatusBar
    LoadItems sbStatusBar, txtLoggingOutput, dMode, mnuFileOpen, lblStatsItemActualCount
    sbStatusBar.Panels(1).Text = "Sorting Items"
    heapsortitems Items, sbStatusBar
    LoadGuilds sbStatusBar
    Screen.MousePointer = vbDefault
    DoEvents
    sbStatusBar.Panels(1).Text = "Setting up containers now"
    SetContVals sbStatusBar
    lblStatsCharActualCritError.Caption = CStr(charCritErrorCount)
    lblStatsCharActualNormError.Caption = CStr(charNormErrorCount)
    lblStatsItemActualCritErrors.Caption = CStr(itemCritErrorCount)
    lblStatsItemActualNormErrors.Caption = CStr(itemNormErrorCount)
    cmdAnalyzeLayers.Enabled = True
    lblMultiActualCount.Caption = CStr(multiCount)
    sbStatusBar.Panels(1).Text = "Preparing combo boxes"
    cmbCharInfo.Clear
    uCharsBound = UBound(Chars)
    uItemsBound = UBound(Items)
    For foundPos = 0 To uCharsBound
        cmbCharInfo.AddItem (CStr(Chars(foundPos).Serial) & " - " & Chars(foundPos).Name), foundPos
    Next foundPos
    cmbItemInfo.Clear
    cmbItemInfoItemBlock.Clear
    If mnuOptionsLargeWorld.Checked Then
        For foundPos = 0 To (uItemsBound \ BlockSize)
            cmbItemInfoItemBlock.AddItem "Block " & foundPos
        Next foundPos
    Else
        If uItemsBound > 10000 Then
            MsgBox "You might wish to turn on big world!"
        Else
            For foundPos = 0 To uItemsBound
                cmbItemInfo.AddItem (CStr(Items(foundPos).Serial) & " - " & Items(foundPos).Name), foundPos
            Next foundPos
        End If
    End If
    If itemCritErrorCount + itemNormErrorCount > 0 Then
        For foundPos = 0 To UBound(ItemErrors)
            cmbItemError.AddItem CStr(Items(ItemErrors(foundPos).Index).Serial) & " - " & Items(ItemErrors(foundPos).Index).Name, foundPos
        Next foundPos
    End If
    AnalyzeLayers sbStatusBar, cmbCharErrors, lblStatsCharActualCritError, cmdFixLayers
    Dim charErrorCounter As Long
    cmbCharErrors.Clear
    For charErrorCounter = 0 To NumCharError - 1
        cmbCharErrors.AddItem (CStr(Chars(CharErrors(charErrorCounter).Index).Serial) & " - " & Chars(CharErrors(charErrorCounter).Index).Name), charErrorCounter
    Next charErrorCounter
    sbStatusBar.Panels(1).Text = "Complete"
    For foundPos = 0 To cmdTab.UBound
        cmdTab(foundPos).Enabled = True
    Next foundPos
    Close #itemsWsc, #charsWsc
    
    ' cache section information
    ReDim Preserve CharsSect(0 To maximumCharSect)
    ReDim Preserve ItemsSect(0 To maximumItemSect)
    Dim iCharCounter As Long, iItemCounter As Long, iCharSect As Long, iItemSect As Long
    
    ' reset section cache to -1 for all of them
    For iCharCounter = 0 To maximumCharSect
        CharsSect(iCharCounter) = -1
    Next iCharCounter
    For iItemCounter = 0 To maximumItemSect
        ItemsSect(iItemCounter) = -1
    Next iItemCounter
    
    ' for each char, store it in the cache
    For iCharCounter = 0 To uCharsBound
        iCharSect = Chars(iCharCounter).iSect
        If iCharSect <> -1 Then
            CharsSect(iCharSect) = iCharCounter ' store our current character in the right section number
        End If
    Next iCharCounter

    ' for each item, store it in the cache
    For iItemCounter = 0 To uItemsBound
        iItemSect = Items(iItemCounter).iSect
        If iItemSect <> -1 Then
            ItemsSect(iItemSect) = iItemCounter ' store our current character in the right section number
        End If
    Next iItemCounter
End Sub
Private Sub mnuFileClose_Click()
    'To Do
    MsgBox "Close Code goes here!"
End Sub
Private Sub mnuFileSave_Click()

    SaveChars sbStatusBar, dlgCommonDialog
    SaveItems sbStatusBar, dlgCommonDialog
    
End Sub
Private Sub mnuFileSaveAs_Click()
    'To Do
    'Setup the common dialog control
    'prior to calling ShowSave
    dlgCommonDialog.ShowSave
End Sub
Private Sub mnuFileSaveAll_Click()
    'To Do
    MsgBox "Save All Code goes here!"
End Sub
Private Sub mnuFileProperties_Click()
    'To Do
    MsgBox "Properties Code goes here!"
End Sub
Private Sub mnuFilePageSetup_Click()
    dlgCommonDialog.ShowPrinter
End Sub
Private Sub mnuFilePrintPreview_Click()
    'To Do
    MsgBox "Print Preview Code goes here!"
End Sub
Private Sub mnuFilePrint_Click()
    'To Do
    MsgBox "Print Code goes here!"
End Sub
Private Sub mnuFileSend_Click()
    'To Do
    MsgBox "Send Code goes here!"
End Sub
Private Sub mnuFileMRU_Click(Index As Integer)
    'To Do
    MsgBox "MRU Code goes here!"
End Sub
Private Sub mnuFileExit_Click()
    'unload the form
    Unload Me
End Sub
Private Sub mnuFileNew_Click()
    'To Do
    MsgBox "New File Code goes here!"
End Sub
Private Function GetLastSlash(sToParse As String) As Long

    Dim startPos As Long
    Dim foundPos As Long
    Dim backupPos As Long
    startPos = 1
    foundPos = 1
    
    Do While (Not IsNull(foundPos) And foundPos <> 0)
        backupPos = foundPos
        foundPos = InStr(startPos, sToParse, "\")
        startPos = foundPos + 1
    Loop
    GetLastSlash = backupPos
End Function
