VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmMain 
   AutoRedraw      =   -1  'True
   BorderStyle     =   1  'Fixed Single
   Caption         =   "UOX Region Displayer"
   ClientHeight    =   8955
   ClientLeft      =   150
   ClientTop       =   435
   ClientWidth     =   13095
   ClipControls    =   0   'False
   FillColor       =   &H00FFFFFF&
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   597
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   873
   StartUpPosition =   2  'CenterScreen
   Begin VB.Frame fraProperty 
      Caption         =   "Properties"
      ClipControls    =   0   'False
      Height          =   5715
      Left            =   11640
      TabIndex        =   1
      Top             =   2880
      Width           =   1575
      Begin VB.OptionButton optAppearance 
         Caption         =   "Unknown"
         Height          =   255
         Index           =   5
         Left            =   120
         TabIndex        =   18
         Top             =   5400
         Width           =   1335
      End
      Begin VB.OptionButton optAppearance 
         Caption         =   "Desolation"
         Height          =   255
         Index           =   4
         Left            =   120
         TabIndex        =   17
         Top             =   5160
         Width           =   1335
      End
      Begin VB.OptionButton optAppearance 
         Caption         =   "Winter"
         Height          =   255
         Index           =   3
         Left            =   120
         TabIndex        =   16
         Top             =   4920
         Width           =   1335
      End
      Begin VB.OptionButton optAppearance 
         Caption         =   "Autumn"
         Height          =   255
         Index           =   2
         Left            =   120
         TabIndex        =   15
         Top             =   4680
         Width           =   1335
      End
      Begin VB.OptionButton optAppearance 
         Caption         =   "Summer"
         Height          =   255
         Index           =   1
         Left            =   120
         TabIndex        =   14
         Top             =   4440
         Width           =   1335
      End
      Begin VB.OptionButton optAppearance 
         Caption         =   "Spring"
         Height          =   255
         Index           =   0
         Left            =   120
         TabIndex        =   13
         Top             =   4200
         Width           =   1335
      End
      Begin VB.CheckBox chkGuarded 
         Caption         =   "Guarded"
         Height          =   255
         Left            =   120
         TabIndex        =   9
         Top             =   3840
         Width           =   1095
      End
      Begin VB.CheckBox chkMagicDamage 
         Caption         =   "Magic Damage"
         Height          =   375
         Left            =   120
         TabIndex        =   8
         Top             =   2760
         Width           =   1335
      End
      Begin VB.CheckBox chkMark 
         Caption         =   "Mark"
         Height          =   255
         Left            =   120
         TabIndex        =   7
         Top             =   3120
         Width           =   975
      End
      Begin VB.CheckBox chkGate 
         Caption         =   "Gate"
         Height          =   255
         Left            =   120
         TabIndex        =   6
         Top             =   3360
         Width           =   975
      End
      Begin VB.CheckBox chkRecall 
         Caption         =   "Recall"
         Height          =   255
         Left            =   120
         TabIndex        =   5
         Top             =   3600
         Width           =   975
      End
      Begin VB.TextBox txtTownName 
         Height          =   675
         Left            =   120
         MultiLine       =   -1  'True
         TabIndex        =   4
         Top             =   2040
         Width           =   1335
      End
      Begin VB.TextBox txtGuardOwner 
         Height          =   645
         Left            =   120
         MultiLine       =   -1  'True
         TabIndex        =   3
         Top             =   480
         Width           =   1335
      End
      Begin VB.TextBox txtWeather 
         Height          =   285
         Left            =   120
         MultiLine       =   -1  'True
         TabIndex        =   2
         Top             =   1440
         Width           =   1335
      End
      Begin VB.Label lblGuardOwner 
         BackStyle       =   0  'Transparent
         Caption         =   "Guard Owner"
         Height          =   255
         Left            =   120
         TabIndex        =   12
         Top             =   240
         Width           =   1095
      End
      Begin VB.Label lblTownName 
         BackStyle       =   0  'Transparent
         Caption         =   "Town Name"
         Height          =   255
         Left            =   120
         TabIndex        =   11
         Top             =   1800
         Width           =   1095
      End
      Begin VB.Label lblWeather 
         Caption         =   "Weather System"
         Height          =   255
         Left            =   120
         TabIndex        =   10
         Top             =   1200
         Width           =   1215
      End
   End
   Begin MSComctlLib.StatusBar sbMain 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   0
      Top             =   8580
      Width           =   13095
      _ExtentX        =   23098
      _ExtentY        =   661
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   3
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Object.Width           =   17859
            MinWidth        =   17859
         EndProperty
         BeginProperty Panel2 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Alignment       =   1
            Object.Width           =   1058
            MinWidth        =   1058
            Object.ToolTipText     =   "Location Count"
         EndProperty
         BeginProperty Panel3 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Alignment       =   1
            Object.Width           =   1058
            MinWidth        =   1058
            Object.ToolTipText     =   "Region Count"
         EndProperty
      EndProperty
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "Book Antiqua"
         Size            =   9
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuFileRegionsLoc 
         Caption         =   "Set &Regions Location"
      End
      Begin VB.Menu mnuFileExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu mnuZoom 
      Caption         =   "&Zoom"
      Begin VB.Menu mnuZoom1 
         Caption         =   "1/&8"
         Checked         =   -1  'True
         Index           =   0
         Tag             =   "8"
      End
      Begin VB.Menu mnuZoom1 
         Caption         =   "1/1&2"
         Index           =   1
         Tag             =   "12"
      End
      Begin VB.Menu mnuZoom1 
         Caption         =   "1/1&6"
         Index           =   2
         Tag             =   "16"
      End
   End
   Begin VB.Menu mnuMap 
      Caption         =   "&Map"
      Begin VB.Menu mnuMapDisplay 
         Caption         =   "&Display"
      End
   End
   Begin VB.Menu mnuHelp 
      Caption         =   "&Help"
      Begin VB.Menu mnuHelpAbout 
         Caption         =   "&About"
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Dim regionLoc As String
Dim locationCount As Integer
Dim regionCount As Integer
Dim lastRegion As Integer
Dim scaleFactor As Long
Private Type Cell
    CellID As Integer
    Z As Byte
End Type

Private Sub Form_Load()
    regionLoc = "C:\"
    DoStartup
    If (App.Comments = "") Then
        Me.Caption = App.Title & " - " & App.Major & "." & App.Minor & " Revision " & App.Revision
    Else
        Me.Caption = App.Title & " - " & App.Major & "." & App.Minor & " Revision " & App.Revision & " (" & App.Comments & " Version)"
    End If
    lastRegion = -1
    scaleFactor = 8
End Sub

Private Sub Form_MouseDown(Button As Integer, Shift As Integer, x As Single, y As Single)
    Dim stringToDisplay As String
    Dim myX As Integer
    Dim myY As Integer
    Dim myRegion As Integer
    myX = x * scaleFactor
    myY = y * scaleFactor
    myRegion = calcRegionFromXY(myX, myY)
    stringToDisplay = "X " & CStr(myX) & vbCrLf & "Y " & CStr(myY) & vbCrLf & "Region " & CStr(myRegion)
    MsgBox stringToDisplay
End Sub

Private Sub Form_MouseMove(Button As Integer, Shift As Integer, x As Single, y As Single)
    Dim myRegion As Integer
    myRegion = calcRegionFromXY(x * scaleFactor, y * scaleFactor)
    If (myRegion <> -1 And myRegion <> lastRegion) Then
        DisplayPropertyOf myRegion
        lastRegion = myRegion
    End If
End Sub

Private Sub mnuFileExit_Click()
    End
End Sub

Private Sub mnuFileRegionsLoc_Click()

    regionLoc = InputBox("Please enter the path to your regions.scp file", "Regions.scp")
    sbMain.Panels(1).Text = "Loading " & regionLoc & "\regions.scp"
    Dim currentLoc As Integer, currentRegion As Integer
    currentLoc = 1
    currentRegion = 0
    Randomize
    
    Dim Guarded As Boolean
    Dim MyString As String
    Dim FileNumber As Integer
    Dim instrResult As Integer
    Dim X1 As Integer, X2 As Integer, Y1 As Integer, Y2 As Integer
    FileNumber = FreeFile   ' Get unused file
Main:
    On Error GoTo ErrorTest
    Open regionLoc & "\regions.scp" For Input As #FileNumber ' Create filename.

    Dim validRegion As Boolean
    Dim tempnum As Integer
    X1 = 0
    Y1 = 0
    X2 = 0
    Y2 = 0
    validRegion = False
    regionCount = 0
    Do While Not EOF(FileNumber) ' Loop until end of file.
        Input #1, MyString ' Read data in
        instrResult = InStr(Mid$(MyString, 1, 7), "SECTION")
        If (instrResult <> 0 And Not IsNull(instrResult)) Then
            validRegion = False
            instrResult = InStr(Mid$(MyString, 1, 14), "SECTION REGION")
            If (instrResult <> 0 And Not IsNull(instrResult)) Then
                currentRegion = Val(Mid$(MyString, 15, Len(MyString) - 14))
                Guarded = False
                validRegion = True
                regions(currentRegion).Weather = 255
                regions(currentRegion).Guarded = False
                regions(currentRegion).MagicDamage = False
                regions(currentRegion).Gate = False
                regions(currentRegion).Mark = False
                regions(currentRegion).Recall = False
                regionCount = regionCount + 1
                If currentRegion = 255 Then
                    regions(currentRegion).Name = "the Wilderness"
                End If
            End If
        End If
        instrResult = InStr(Mid$(MyString, 1, 4), "MARK")
        If (instrResult <> 0 And Not IsNull(instrResult)) Then
            If Val(Mid$(MyString, 5, Len(MyString) - 4)) <> 0 Then
                regions(currentRegion).Mark = True
            End If
        End If
        instrResult = InStr(Mid$(MyString, 1, 4), "GATE")
        If (instrResult <> 0 And Not IsNull(instrResult)) Then
            If Val(Mid$(MyString, 5, Len(MyString) - 4)) <> 0 Then
                regions(currentRegion).Gate = True
            End If
        End If
        instrResult = InStr(Mid$(MyString, 1, 6), "RECALL")
        If (instrResult <> 0 And Not IsNull(instrResult)) Then
            If Val(Mid$(MyString, 7, Len(MyString) - 6)) <> 0 Then
                regions(currentRegion).Recall = True
            End If
        End If
        instrResult = InStr(Mid$(MyString, 1, 2), "X1")
        If (instrResult <> 0 And Not IsNull(instrResult) And validRegion) Then
            regionNum(currentLoc).X1 = Val(Mid$(MyString, 3, Len(MyString) - 2))
        End If
        instrResult = InStr(Mid$(MyString, 1, 2), "X2")
        If (instrResult <> 0 And Not IsNull(instrResult) And validRegion) Then
            regionNum(currentLoc).X2 = Val(Mid$(MyString, 3, Len(MyString) - 2))
            If (regionNum(currentLoc).X2 < regionNum(currentLoc).X1) Then
                tempnum = regionNum(currentLoc).X1
                regionNum(currentLoc).X1 = regionNum(currentLoc).X2
                regionNum(currentLoc).X2 = tempnum
            End If
        End If
        instrResult = InStr(Mid$(MyString, 1, 7), "GUARDED")
        If (instrResult <> 0 And Not IsNull(instrResult) And validRegion) Then
            tempnum = Val(Mid$(MyString, 8, Len(MyString) - 7))
            regions(currentRegion).Guarded = (tempnum <> 0)
        End If
        instrResult = InStr(Mid$(MyString, 1, 11), "MAGICDAMAGE")
        If (instrResult <> 0 And Not IsNull(instrResult) And validRegion) Then
            tempnum = Val(Mid$(MyString, 12, Len(MyString) - 11))
            regions(currentRegion).MagicDamage = (tempnum <> 0)
        End If
        instrResult = InStr(Mid$(MyString, 1, 7), "ABWEATH")
        If (instrResult <> 0 And Not IsNull(instrResult) And validRegion) Then
            tempnum = Val(Mid$(MyString, 8, Len(MyString) - 7))
            regions(currentRegion).Weather = tempnum
        End If
        instrResult = InStr(Mid$(MyString, 1, 4), "NAME")
        If (instrResult <> 0 And Not IsNull(instrResult) And validRegion) Then
            regions(currentRegion).Name = Mid$(MyString, 6, Len(MyString) - 4)
        End If
        instrResult = InStr(Mid$(MyString, 1, 10), "GUARDOWNER")
        If (instrResult <> 0 And Not IsNull(instrResult) And validRegion) Then
            regions(currentRegion).Guardowner = Mid$(MyString, 12, Len(MyString) - 10)
        End If
        instrResult = InStr(Mid$(MyString, 1, 2), "Y1")
        If (instrResult <> 0 And Not IsNull(instrResult) And validRegion) Then
            regionNum(currentLoc).Y1 = Val(Mid$(MyString, 3, Len(MyString) - 2))
        End If
        instrResult = InStr(Mid$(MyString, 1, 2), "Y2")
        If (instrResult <> 0 And Not IsNull(instrResult) And validRegion) Then
            regionNum(currentLoc).Y2 = Val(Mid$(MyString, 3, Len(MyString) - 2))
            If (regionNum(currentLoc).Y2 < regionNum(currentLoc).Y1) Then
                tempnum = regionNum(currentLoc).Y1
                regionNum(currentLoc).Y1 = regionNum(currentLoc).Y2
                regionNum(currentLoc).Y2 = tempnum
            End If
            If (currentRegion <> 255) Then
                Dim lX As Long, lY As Long, hX As Long, hY As Long
                FillStyle = 0
                If regions(currentRegion).Guarded Then
                    FillColor = vbBlue
                Else
                    FillColor = vbRed
                End If
                lX = regionNum(currentLoc).X1 \ scaleFactor
                lY = regionNum(currentLoc).Y1 \ scaleFactor
                hY = (regionNum(currentLoc).Y2 - regionNum(currentLoc).Y1) \ scaleFactor
                hX = (regionNum(currentLoc).X2 - regionNum(currentLoc).X1) \ scaleFactor
                Call Rectangle(Me.hdc, lX, lY, hX + lX, hY + lY)
            End If
            regionNum(currentLoc).Region = currentRegion
            currentLoc = currentLoc + 1
        End If
        instrResult = InStr(Mid$(MyString, 1, 10), "APPEARANCE")
        If (instrResult <> 0 And Not IsNull(instrResult) And validRegion) Then
            Dim xString As String
            xString = Right$(MyString, Len(MyString) - 10)
            Dim iBlah As WorldType
            For iBlah = WRLD_SPRING To WRLD_UNKNOWN
                If Trim$(xString) = WorldTypeNames(iBlah) Then
                    regions(currentRegion).Appearance = iBlah
                    Exit For
                End If
            Next
        End If
    Loop
    Me.Refresh
    Me.fraProperty.ZOrder vbBringToFront
    locationCount = currentLoc
    sbMain.Panels(1).Text = "Working on file " & regionLoc & "\regions.scp"
    sbMain.Panels(2).Text = locationCount
    sbMain.Panels(3).Text = regionCount
    Close #FileNumber    ' Close file.
    Exit Sub
ErrorTest:
    Select Case Err.Number
    Case 53
        MsgBox "The file you tried to reference (" & regionLoc & "\regions.scp) does not exist, please try again", vbInformation + vbOKOnly, App.Title
    Case Else
        MsgBox Err.Description
    End Select
End Sub

Public Function calcRegionFromXY(x As Integer, y As Integer) As Integer
    
    Dim i As Integer
    For i = 1 To locationCount
        If (regionNum(i).X1 <= x And regionNum(i).Y1 <= y And regionNum(i).X2 >= x And regionNum(i).Y2 >= y) Then
            calcRegionFromXY = regionNum(i).Region
            Exit Function
        End If
    Next i
    calcRegionFromXY = -1

End Function

Private Sub mnuHelpAbout_Click()
    frmAbout.Show
End Sub

Private Sub mnuMapDisplay_Click()
    Dim hDestDC As Long
    Dim BlockX As Long
    Dim BlockY As Long
    Dim CellID As Integer
    Dim Color16 As Integer
    Dim UOPath As String
    UOPath = InputBox("Please enter the path to your map0.mul file", "Path to Map0.MUL", "C:\program files\Ultima Online")
    sbMain.Panels(1).Text = "Loading " & UOPath & "\map0.mul"
   
   Dim Map As CFile, Radar As CFile
   On Error GoTo CleanUp
   
   Set Map = New CFile
   Set Radar = New CFile
   
   If Not Map.OpenFile(UOPath & "\Map0.mul") Then
      MsgBox "Failed to open Map0.mul", vbApplicationModal Or vbOKOnly Or vbCritical, "Error"
      GoTo CleanUp
   End If
   
   
   If Not Radar.OpenFile(UOPath & "\RadarCol.mul") Then
      MsgBox "Failed to open RadarCol.mul", vbApplicationModal Or vbOKOnly Or vbCritical, "Error"
      GoTo CleanUp
   End If
   
   hDestDC = hdc
   
   For BlockX = 0 To 6143 Step scaleFactor
        For BlockY = 0 To 4095 Step scaleFactor
            Map.MoveTo (((((BlockX \ 8) * 512) + (BlockY \ 8)) * 196) + 4 + (((BlockY Mod 8) * 8 + (BlockX Mod 8)) * 3))
            CellID = Map.ReadInt
            Radar.MoveTo CellID * 2
            Color16 = Radar.ReadInt
            Call SetPixelV(hDestDC, BlockX \ scaleFactor, BlockY \ scaleFactor, Color32(Color16))
        Next
        DoEvents
        Me.Refresh
   Next
   RedrawShapes
CleanUp:
   Map.CloseFile
   Radar.CloseFile
   Set Map = Nothing
   Set Radar = Nothing
End Sub


Private Sub mnuZoom1_Click(Index As Integer)

    scaleFactor = mnuZoom1(Index).Tag
    mnuZoom1(0).Checked = (Index = 0)
    mnuZoom1(1).Checked = (Index = 1)
    mnuZoom1(2).Checked = (Index = 2)
    
    Select Case Index
    Case 0
        Height = 590 * Screen.TwipsPerPixelY
        Width = 885 * Screen.TwipsPerPixelX
    Case 1
        Height = 450 * Screen.TwipsPerPixelY
        Width = 650 * Screen.TwipsPerPixelX
    Case 2
        Height = 450 * Screen.TwipsPerPixelY
        Width = 550 * Screen.TwipsPerPixelX
    End Select
    
    Cls
    RedrawShapes
    fraProperty.Top = sbMain.Top - fraProperty.Height
    fraProperty.Left = sbMain.Width - fraProperty.Width
    fraProperty.Refresh
End Sub

Public Sub RedrawShapes()
    Dim i As Long
    Dim curRegion As Long
    Dim lX As Long, lY As Long, hY As Long, hX As Long
    For i = 0 To locationCount - 2
        If regions(regionNum(i).Region).Guarded Then
            FillColor = vbBlue
        Else
            FillColor = vbRed
        End If
        FillStyle = 0
        lX = regionNum(i).X1 \ scaleFactor
        lY = regionNum(i).Y1 \ scaleFactor
        hY = (regionNum(i).Y2 - regionNum(i).Y1) \ scaleFactor
        hX = (regionNum(i).X2 - regionNum(i).X1) \ scaleFactor
        Call Rectangle(Me.hdc, lX, lY, lX + hX, lY + hY)
    Next i
    Me.Refresh
    Me.fraProperty.Refresh
End Sub

Public Sub DisplayPropertyOf(myRegion As Integer)
    txtGuardOwner.Text = regions(myRegion).Guardowner
    txtTownName.Text = regions(myRegion).Name
    txtWeather.Text = regions(myRegion).Weather
    If regions(myRegion).Mark Then
        chkMark.Value = vbChecked
    Else
        chkMark.Value = vbUnchecked
    End If
    If regions(myRegion).Gate Then
        chkGate.Value = vbChecked
    Else
        chkGate.Value = vbUnchecked
    End If
    If regions(myRegion).Recall Then
        chkRecall.Value = vbChecked
    Else
        chkRecall.Value = vbUnchecked
    End If
    If regions(myRegion).Guarded Then
        chkGuarded.Value = vbChecked
    Else
        chkGuarded.Value = vbUnchecked
    End If
    If regions(myRegion).MagicDamage Then
        chkMagicDamage.Value = vbChecked
    Else
        chkMagicDamage.Value = vbUnchecked
    End If
    optAppearance(regions(myRegion).Appearance).Value = True
End Sub
Public Function Color32(ByVal Color16 As Integer) As Long
    Color32 = _
    (((Color16 And &H1F) * &HFF \ &H1F) * &H10000) Or _
    (CLng((((Color16 \ &H20) And &H1F) * &HFF \ &H1F)) * &H100) Or _
    ((((Color16 \ &H400) And &H1F) * &HFF \ &H1F))
End Function
