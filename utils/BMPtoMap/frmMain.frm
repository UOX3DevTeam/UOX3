VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Object = "{86CF1D34-0C5F-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCT2.OCX"
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "comdlg32.ocx"
Begin VB.Form frmMain 
   AutoRedraw      =   -1  'True
   BackColor       =   &H00000000&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "UOX BMP to Map converter"
   ClientHeight    =   7935
   ClientLeft      =   45
   ClientTop       =   615
   ClientWidth     =   13305
   FillStyle       =   0  'Solid
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   529
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   887
   StartUpPosition =   2  'CenterScreen
   Begin VB.Frame fraProperty 
      Caption         =   "Properties"
      Height          =   3615
      Left            =   0
      TabIndex        =   3
      Top             =   4080
      Width           =   1815
      Begin VB.CommandButton cmdOptionAdd 
         Caption         =   "Add"
         Height          =   375
         Left            =   120
         TabIndex        =   19
         Top             =   3120
         Width           =   1575
      End
      Begin VB.TextBox txtColourLo 
         Enabled         =   0   'False
         Height          =   285
         Left            =   120
         TabIndex        =   15
         Text            =   "0"
         Top             =   2400
         Width           =   1215
      End
      Begin MSComCtl2.UpDown udAltitude 
         Height          =   285
         Left            =   856
         TabIndex        =   13
         Top             =   1800
         Width           =   240
         _ExtentX        =   423
         _ExtentY        =   503
         _Version        =   393216
         BuddyControl    =   "txtAltitude"
         BuddyDispid     =   196612
         OrigLeft        =   1080
         OrigTop         =   1800
         OrigRight       =   1320
         OrigBottom      =   2055
         Max             =   127
         Min             =   -127
         SyncBuddy       =   -1  'True
         BuddyProperty   =   65547
         Enabled         =   -1  'True
      End
      Begin VB.TextBox txtAltitude 
         Height          =   285
         Left            =   120
         TabIndex        =   12
         Top             =   1800
         Width           =   735
      End
      Begin VB.TextBox txtTileHi 
         Enabled         =   0   'False
         Height          =   285
         Left            =   960
         TabIndex        =   9
         Text            =   "0"
         Top             =   1200
         Width           =   495
      End
      Begin MSComCtl2.UpDown udLoTile 
         Height          =   285
         Left            =   600
         TabIndex        =   8
         Top             =   1200
         Width           =   240
         _ExtentX        =   423
         _ExtentY        =   503
         _Version        =   393216
         BuddyControl    =   "txtTileLo"
         BuddyDispid     =   196614
         OrigLeft        =   720
         OrigTop         =   1200
         OrigRight       =   960
         OrigBottom      =   1455
         Max             =   16384
         SyncBuddy       =   -1  'True
         BuddyProperty   =   65547
         Enabled         =   -1  'True
      End
      Begin VB.TextBox txtTileLo 
         Enabled         =   0   'False
         Height          =   285
         Left            =   120
         TabIndex        =   7
         Text            =   "0"
         Top             =   1200
         Width           =   735
      End
      Begin VB.TextBox txtName 
         Height          =   285
         Left            =   120
         TabIndex        =   5
         Top             =   600
         Width           =   1575
      End
      Begin MSComCtl2.UpDown udTileHi 
         Height          =   285
         Left            =   1440
         TabIndex        =   10
         Top             =   1200
         Width           =   240
         _ExtentX        =   423
         _ExtentY        =   503
         _Version        =   393216
         BuddyControl    =   "txtTileHi"
         BuddyDispid     =   196613
         OrigLeft        =   720
         OrigTop         =   1200
         OrigRight       =   960
         OrigBottom      =   1455
         Max             =   16384
         SyncBuddy       =   -1  'True
         BuddyProperty   =   0
         Enabled         =   -1  'True
      End
      Begin VB.TextBox txtColourHi 
         Enabled         =   0   'False
         Height          =   285
         Left            =   120
         TabIndex        =   16
         Text            =   "0"
         Top             =   2760
         Width           =   1215
      End
      Begin VB.Label lblHiColour 
         BorderStyle     =   1  'Fixed Single
         Height          =   255
         Left            =   1440
         TabIndex        =   18
         Top             =   2760
         Width           =   255
      End
      Begin VB.Label lblLoColour 
         BorderStyle     =   1  'Fixed Single
         Height          =   255
         Left            =   1440
         TabIndex        =   17
         Top             =   2400
         Width           =   255
      End
      Begin VB.Label lblColourRange 
         BackStyle       =   0  'Transparent
         Caption         =   "Colour Range"
         Height          =   255
         Left            =   120
         TabIndex        =   14
         Top             =   2160
         Width           =   1575
      End
      Begin VB.Label lblAltitude 
         Caption         =   "Altitude"
         Height          =   255
         Left            =   120
         TabIndex        =   11
         Top             =   1560
         Width           =   1575
      End
      Begin VB.Label lblTileRange 
         BackStyle       =   0  'Transparent
         Caption         =   "Tile Range"
         Height          =   255
         Left            =   120
         TabIndex        =   6
         Top             =   960
         Width           =   1575
      End
      Begin VB.Label lblName 
         BackStyle       =   0  'Transparent
         Caption         =   "Name"
         Height          =   255
         Left            =   120
         TabIndex        =   4
         Top             =   360
         Width           =   615
      End
   End
   Begin MSComctlLib.TreeView tvSetup 
      Height          =   4095
      Left            =   0
      TabIndex        =   2
      Top             =   0
      Width           =   1815
      _ExtentX        =   3201
      _ExtentY        =   7223
      _Version        =   393217
      HideSelection   =   0   'False
      Indentation     =   992
      Style           =   6
      BorderStyle     =   1
      Appearance      =   1
   End
   Begin VB.PictureBox pctMain 
      AutoRedraw      =   -1  'True
      BackColor       =   &H00000000&
      BorderStyle     =   0  'None
      Height          =   7680
      Left            =   1800
      ScaleHeight     =   512
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   768
      TabIndex        =   1
      Top             =   0
      Width           =   11520
   End
   Begin MSComctlLib.StatusBar sbStatusBar 
      Align           =   2  'Align Bottom
      Height          =   255
      Left            =   0
      TabIndex        =   0
      Top             =   7680
      Width           =   13305
      _ExtentX        =   23469
      _ExtentY        =   450
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   3
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Object.Width           =   19844
            MinWidth        =   19844
         EndProperty
         BeginProperty Panel2 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Object.Width           =   1270
            MinWidth        =   1270
            Object.ToolTipText     =   "X Location"
         EndProperty
         BeginProperty Panel3 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Object.Width           =   1270
            MinWidth        =   1270
            Object.ToolTipText     =   "Y Location"
         EndProperty
      EndProperty
   End
   Begin MSComDlg.CommonDialog dlgCommon 
      Left            =   1920
      Top             =   3360
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuOpenPic 
         Caption         =   "Open &Pic"
      End
      Begin VB.Menu mnuFileSavePic 
         Caption         =   "&Save Pic"
      End
      Begin VB.Menu mnuFileLoadTranslation 
         Caption         =   "&Load Translation"
      End
      Begin VB.Menu mnuFileSaveTranslation 
         Caption         =   "Sa&ve Translation"
      End
      Begin VB.Menu mnuFileExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu mnuMap 
      Caption         =   "&Map"
      Begin VB.Menu mnuMapMakeCrap 
         Caption         =   "&Convert"
      End
   End
   Begin VB.Menu mnuBrushSize 
      Caption         =   "&Brush Size"
      Begin VB.Menu mnuBrushSizeA 
         Caption         =   "&1x1"
         Checked         =   -1  'True
         Index           =   0
      End
      Begin VB.Menu mnuBrushSizeA 
         Caption         =   "&3x3"
         Index           =   1
      End
      Begin VB.Menu mnuBrushSizeA 
         Caption         =   "&5x5"
         Index           =   2
      End
      Begin VB.Menu mnuBrushSizeA 
         Caption         =   "&7x7"
         Index           =   3
      End
      Begin VB.Menu mnuBrushSizeA 
         Caption         =   "&9x9"
         Index           =   4
      End
      Begin VB.Menu mnuBrushSizeA 
         Caption         =   "11x11"
         Index           =   5
      End
      Begin VB.Menu mnuBrushSizeA 
         Caption         =   "13x13"
         Index           =   6
      End
      Begin VB.Menu mnuBrushSizeA 
         Caption         =   "15x15"
         Index           =   7
      End
      Begin VB.Menu mnuBrushSizeA 
         Caption         =   "17x17"
         Index           =   8
      End
      Begin VB.Menu mnuBrushSizeA 
         Caption         =   "19x19"
         Index           =   9
      End
      Begin VB.Menu mnuBrushSizeA 
         Caption         =   "21x21"
         Index           =   10
      End
      Begin VB.Menu mnuBrushSizeA 
         Caption         =   "23x23"
         Index           =   11
      End
      Begin VB.Menu mnuBrushSizeA 
         Caption         =   "25x25"
         Index           =   12
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Private Const TERRAIN_WATER = 0
Private Const TERRAIN_GRASS = 1
Private Const TERRAIN_MOUNTAIN = 2
Private Const TERRAIN_SNOW = 3
Private Const TERRAIN_DESERT = 4
Dim m_ScaleX As Single, m_ScaleY As Single
Private Type Cell
    CellID As Integer
    Z As Byte
End Type
Private Type ColourEntry
    LoTile As Integer
    HiTile As Integer
    BaseAltitude As Byte
    colour As Long
    LoColour As Long
    HiColour As Long
    Name As String
End Type
Dim colourArray() As ColourEntry
Dim MAP As New CFile
Dim curNode As Long
Dim isAdding As Boolean
Const nuName As Long = 0
Const nuLoTile As Long = 1
Const nuHiTile As Long = 2
Const nuAltitude As Long = 3
Dim bSize As Long

' Shanira - Heightmap and BMP stuff
Private Type BITMAPFILEHEADER
        bfType As Integer
        bfSize As Long
        bfReserved1 As Integer
        bfReserved2 As Integer
        bfOffBits As Long
End Type
Private Type BITMAPINFOHEADER '40 bytes
        biSize As Long
        biWidth As Long
        biHeight As Long
        biPlanes As Integer
        biBitCount As Integer
        biCompression As Long
        biSizeImage As Long
        biXPelsPerMeter As Long
        biYPelsPerMeter As Long
        biClrUsed As Long
        biClrImportant As Long
End Type
Dim ImageMap(1536, 4096) As Long
Dim HeightBMP(767, 511) As Byte
Dim picFile As String

' Shanira - Load the heightmap BMP (should be 256 colors)
Private Sub LoadHeightBMP(ByVal FileName As String)
    Dim Pos As Long
    Dim ISize As Long
    Dim FSize As Long
    Dim y As Long
    Dim x As Long
    Dim ArrayPos As Long
    Dim BMPInfo As BITMAPINFOHEADER
    Dim FileNum As Integer
    Dim Character As String * 1
    Dim n As Long

    BMPInfo = GetBMPInfo(FileName)
    ISize = BMPInfo.biSizeImage
    ISize = ISize - 1
    
    FSize = FileLen(FileName)
    
    Pos = FSize - ISize
    
    FileNum = FreeFile
    Open FileName For Binary Access Read As #FileNum
    y = BMPInfo.biHeight - 1
    x = 0
    For n = 0 To ISize
        If x >= BMPInfo.biWidth Then
            x = 0
            y = y - 1
        End If
        ArrayPos = x + y * BMPInfo.biWidth
        
        Get #FileNum, Pos + ArrayPos, Character
        HeightBMP(x, BMPInfo.biHeight - (y + 1)) = Asc(Character)
        
        x = x + 1
    Next n
    Close #FileNum
End Sub
' Shanira - Gets the BMP info needed for the heightmap loader
Private Function GetBMPInfo(ByVal BmpFile As String) As BITMAPINFOHEADER
    Dim BFHEADER As BITMAPFILEHEADER                                'bitmap file header structure
    Dim BINFOHEADER As BITMAPINFOHEADER                             'bitmap information structure
    Dim intCount As Integer 'count variable
    Dim FileNum As Integer
    
    FileNum = FreeFile
    Open BmpFile For Binary Access Read As #FileNum   'open the bitmap file for reading
    Get #FileNum, , BFHEADER                          'fill the file header
    Get #FileNum, , BINFOHEADER                       'fill the info header
    If BINFOHEADER.biClrUsed = 0 Then BINFOHEADER.biClrUsed = 256   'make sure that we grab a valid number of palette entries
    Close #FileNum
    
    'For n = 0 To 255
        'BMPPal(n) = RGB(PalEntries(n).Red, PalEntries(n).Green, PalEntries(n).Blue)
        
    'Next n
    
    GetBMPInfo = BINFOHEADER
End Function


Private Sub cmdOptionAdd_Click()
    If isAdding Then
        ' do save code
        cmdOptionAdd.Caption = "Add"
        Dim curUBound As Long
        curUBound = UBound(colourArray)
        ReDim Preserve colourArray(0 To curUBound + 1)
        curUBound = curUBound + 1
        With colourArray(curUBound)
            .Name = txtName.Text
            .LoColour = txtColourLo.Text
            .HiColour = txtColourHi.Text
            .BaseAltitude = UnsignedHeight(txtAltitude.Text)
            .HiTile = txtTileHi.Text
            .LoTile = txtTileLo.Text
        End With
        tvSetup.Nodes.Add "Root", tvwChild, "Col" & CStr(curUBound), colourArray(curUBound).Name
    Else
        ' do add code
        curNode = -1
        txtAltitude.Text = "0"
        txtColourHi.Text = "0"
        txtColourLo.Text = "0"
        txtTileLo.Text = "0"
        txtTileHi.Text = "0"
        txtName.Text = "Unknown"
        cmdOptionAdd.Caption = "Save"
        lblLoColour.BackColor = 0
        lblHiColour.BackColor = 0
    End If
    isAdding = Not isAdding
End Sub

Private Sub Form_Load()
    Me.Caption = "UOX Bmp to Map Converter - " & App.Major & "." & App.Minor & " Build " & App.Revision
    isAdding = False
    tvSetup.Nodes.Add , , "Root", "Customization"
    tvSetup.Nodes.Item(1).Expanded = True
    pctMain.ScaleMode = vbPixels
    m_ScaleX = pctMain.ScaleWidth / 768
    m_ScaleY = pctMain.ScaleHeight / 512
    ReDim colourArray(0 To 5)
    With colourArray(0)
    .colour = vbBlue: .BaseAltitude = 251: .LoTile = &HA8: .HiTile = &HAB: .Name = "Water": .HiColour = .colour: .LoColour = .colour:
    End With
    With colourArray(1)
    .colour = vbGreen: .BaseAltitude = 0: .LoTile = &H3: .HiTile = &H6: .Name = "Grass": .HiColour = .colour: .LoColour = .colour
    End With
    With colourArray(2)
    .colour = &H646464: .BaseAltitude = 0: .LoTile = &H22C: .HiTile = &H22F: .Name = "Mountain": .HiColour = .colour: .LoColour = .colour
    End With
    With colourArray(3)
    .colour = vbWhite: .BaseAltitude = 0: .LoTile = &H11A: .HiTile = &H11D: .Name = "Snow": .HiColour = .colour: .LoColour = .colour
    End With
    With colourArray(4)
    .colour = vbYellow: .BaseAltitude = 0: .LoTile = &H16: .HiTile = &H19: .Name = "Desert": .HiColour = .colour: .LoColour = .colour
    End With
    With colourArray(5)
    .colour = vbRed: .BaseAltitude = 0: .Name = "Lava": .HiColour = .colour: .LoColour = .colour: .LoTile = &H1F4: .HiTile = &H1F7
    End With
    DisplayNodes
    curNode = -1
End Sub

Private Sub Form_Unload(Cancel As Integer)
    If Not MAP Is Nothing Then
        MAP.CloseFile
    End If
End Sub

Private Sub lblHiColour_Click()
    If curNode <> -1 Or isAdding Then
        dlgCommon.Color = txtColourHi.Text
        dlgCommon.CancelError = True
        On Error GoTo errhandler
        Dim myCol As Long, loCol As Long
        dlgCommon.Flags = cdlCCRGBInit
        dlgCommon.ShowColor
        myCol = dlgCommon.Color
        loCol = txtColourLo.Text
        If myCol < loCol Then
            txtColourLo.Text = myCol
            lblLoColour.BackColor = myCol
            txtColourHi.Text = loCol
            lblHiColour.BackColor = loCol
            If curNode <> -1 Then
                colourArray(curNode).LoColour = myCol
                colourArray(curNode).HiColour = loCol
            End If
        Else
            txtColourHi.Text = myCol
            lblHiColour.BackColor = myCol
            If curNode <> -1 Then
                colourArray(curNode).HiColour = myCol
            End If
        End If
    End If
errhandler:
End Sub

Private Sub lblLoColour_Click()
    If curNode <> -1 Or isAdding Then
        dlgCommon.Color = txtColourLo.Text
        dlgCommon.CancelError = True
        On Error GoTo errhandler
        Dim myCol As Long, hiCol As Long
        dlgCommon.Flags = cdlCCRGBInit
        dlgCommon.ShowColor
        myCol = dlgCommon.Color
        hiCol = txtColourHi.Text
        If myCol > hiCol Then
            txtColourLo.Text = hiCol
            lblLoColour.BackColor = hiCol
            txtColourHi.Text = myCol
            lblHiColour.BackColor = myCol
            If curNode <> -1 Then
                colourArray(curNode).LoColour = hiCol
                colourArray(curNode).HiColour = myCol
            End If
        Else
            txtColourLo.Text = myCol
            lblLoColour.BackColor = myCol
            If curNode <> -1 Then
                colourArray(curNode).LoColour = myCol
            End If
        End If
    End If
errhandler:
End Sub

Private Sub mnuBrushSizeA_Click(Index As Integer)
    bSize = Index
    Dim i As Long
    For i = 0 To mnuBrushSizeA.UBound
        If i <> Index Then
            mnuBrushSizeA(i).Checked = False
        Else
            mnuBrushSizeA(i).Checked = True
        End If
    Next i
End Sub

Private Sub mnuFileExit_Click()
    End
End Sub

Private Sub mnuFileLoadTranslation_Click()
    Dim sFile As String

    On Error GoTo errhandler
    With dlgCommon
        .Filter = "BMP2MAP Translation (*.btm)|*.btm"
        .CancelError = True
        .ShowOpen
        If Len(.FileName) = 0 Then
            Exit Sub
        End If
        sFile = .FileName
    End With
    On Error GoTo 0
    Dim myFile As Long
    myFile = FreeFile
    Open sFile For Input As #myFile
    Dim i As Long
    Dim currentEntry As Long
    Dim sLine As String
    currentEntry = -1
    ReDim colourArray(0 To 0)
    Do While Not EOF(myFile)
        Line Input #myFile, sLine
        If Left$(sLine, 1) = "[" Then
            ' found section!
            currentEntry = currentEntry + 1
            ReDim Preserve colourArray(0 To currentEntry)
            colourArray(currentEntry).Name = Mid$(sLine, 2, Len(sLine) - 2)
        Else
            If currentEntry <> -1 Then
                If Len(sLine) > 0 Then
                    Select Case Left$(sLine, 1)
                    Case "A"
                        If Left$(sLine, 3) = "Alt" Then
                            colourArray(currentEntry).BaseAltitude = UnsignedHeight(CLng(Right$(sLine, Len(sLine) - 4)))
                        End If
                    Case "H"
                        If Left$(sLine, 8) = "HiColour" Then
                            colourArray(currentEntry).HiColour = CLng("&H" & Right$(sLine, Len(sLine) - 9))
                        ElseIf Left$(sLine, 6) = "HiTile" Then
                            colourArray(currentEntry).HiTile = CLng("&H" & Right$(sLine, Len(sLine) - 7))
                        End If
                    Case "L"
                        If Left$(sLine, 8) = "LoColour" Then
                            colourArray(currentEntry).LoColour = CLng("&H" & Right$(sLine, Len(sLine) - 9))
                        ElseIf Left$(sLine, 6) = "LoTile" Then
                            colourArray(currentEntry).LoTile = CLng("&H" & Right$(sLine, Len(sLine) - 7))
                        End If
                    End Select
                End If
            End If
        End If
    Loop
    Close myFile
    DisplayNodes
errhandler:
End Sub

Private Sub mnuFileSavePic_Click()
    SavePicture pctMain.Image, "savedpic.bmp"
End Sub

Private Sub mnuFileSaveTranslation_Click()

    Dim myFile As Long
    myFile = FreeFile
    Open "trans.btm" For Output As #myFile
    Dim i As Long
    For i = LBound(colourArray) To UBound(colourArray)
        Print #myFile, "[" & colourArray(i).Name & "]"
        Print #myFile, "Alt=" & SignedHeight(colourArray(i).BaseAltitude)
        Print #myFile, "HiColour=" & Hex$(colourArray(i).HiColour)
        Print #myFile, "LoColour=" & Hex$(colourArray(i).LoColour)
        Print #myFile, "HiTile=" & Hex$(colourArray(i).HiTile)
        Print #myFile, "LoTile=" & Hex$(colourArray(i).LoTile)
        Print #myFile, ""
    Next i
    Close #myFile
End Sub

Private Sub mnuMapMakeCrap_Click()

    ' Shanira - Open heightmap picking dialog thingy
    Dim sFile As String

    With dlgCommon
        .Filter = "UO World Heightmap (*.bmp)|*.bmp"
        .ShowOpen
        If Len(.FileName) = 0 Then
            Exit Sub
        End If
        sFile = .FileName
    End With
    LoadHeightBMP sFile
    ' -


    Dim blah2 As Long
    blah2 = FreeFile
    Open App.Path & "\map0.mul" For Binary As #blah2
    Close #blah2
    MAP.OpenFile App.Path & "\Map0.mul"
    
    Dim blah(0 To 63) As Cell
    Dim counter As Long
    Dim sTime As Long, eTime As Long
    sTime = GetTickCount
    Dim x As Long, y As Long
    Dim vPtr As Long
    Dim trgColour As Long, tileID As Integer
    Dim targHeight As Byte
    Dim iCounter As Long
    Dim iSegment As Long
    vPtr = VarPtr(blah(0))
    For iSegment = 0 To 3
        FillDisplaySegment iSegment
        For x = 0 To 767
            Me.sbStatusBar.Panels(1).Text = x & "/767 (" & (x + 1) / 768 * 100 & ")"
            DoEvents
            For y = 0 To 511
                trgColour = GetPixel(pctMain.hdc, x, y)
                iCounter = GetIndexFromColour(trgColour)
                For counter = 0 To 63
                    tileID = -1
                    If iCounter <> -1 Then
                        tileID = Rnd * (colourArray(iCounter).HiTile - colourArray(iCounter).LoTile) + colourArray(iCounter).LoTile
    '                    targHeight = colourArray(iCounter).BaseAltitude
                    End If
                    If tileID = -1 Then
                        tileID = 0
                        targHeight = 0
                    End If
                    blah(counter).CellID = tileID
    '                blah(counter).Z = targHeight
                    blah(counter).Z = HeightBMP(x, y)
                Next counter
                Call FillBlock6(blah)
            Next y
        Next x
    Next iSegment
    eTime = GetTickCount
    sbStatusBar.Panels(1).Text = "Elapsed ticks: " & (eTime - sTime)
End Sub

Private Sub mnuOpenPic_Click()

    With dlgCommon
        .Filter = "UO World Image (*.bmp)|*.bmp"
        .ShowOpen
        If Len(.FileName) = 0 Then
            Exit Sub
        End If
        picFile = .FileName
    End With
    pctMain.Picture = LoadPicture(picFile)
    Dim blah As Long
    blah = FreeFile
End Sub

Private Sub FillBlock6(blah() As Cell)
    Dim pvData As Long
    Dim pvTileID As Long
    Dim cX As Byte, cY As Byte
    Dim dtOffset As Long
    Dim tileID As Long
    Dim Data(0 To 195) As Byte
    pvData = VarPtr(Data(0))
    For cY = 0 To 7
        dtOffset = (4 + (cY * (8 * 3)))
        For cX = 0 To 7
            Call RtlMoveMemory(pvData + dtOffset + (cX * 3), VarPtr(blah(cY * 8 + cX)), 3)
        Next
    Next
    MAP.WriteChunk pvData, 196
End Sub

Private Sub FillBlock(ByVal TerrainID As Long, ByVal x As Long, ByVal y As Long)
   Dim pvData As Long
   Dim pvTileID As Long
   Dim cX As Byte, cY As Byte
   Dim dtOffset As Long
   Dim tileID As Long
   Dim Data(0 To 195) As Byte
   
   pvData = VarPtr(Data(0))
   pvTileID = VarPtr(tileID)
   
   For cY = 0 To 7
      dtOffset = (4 + (cY * (8 * 3)))
      For cX = 0 To 7
         Select Case TerrainID
            Case 0:               tileID = ((Rnd * (&HAB - &HA8)) + &HA8)
            Case 1:               tileID = ((Rnd * (&H6 - &H3)) + &H3)
            Case 2:               tileID = ((Rnd * (&H22F - &H22C)) + &H22C)
            Case 3:               tileID = ((Rnd * (&H11D - &H11A)) + &H11A)
            Case 4:               tileID = ((Rnd * (&H12D - &H12A)) + &H12A)
         End Select
   
         Call RtlMoveMemory(pvData + dtOffset + (cX * 3), pvTileID, 2)
      Next
   Next
   
   'MAP.ReadChunk pvData, 4
   'MAP.Shift -4
   MAP.WriteChunk pvData, 196
End Sub

Private Sub pctMain_MouseMove(Button As Integer, Shift As Integer, x As Single, y As Single)
    sbStatusBar.Panels(2).Text = x
    sbStatusBar.Panels(3).Text = y
    If curNode <> -1 And Button = vbLeftButton Then
        Dim j As Long, k As Long
        For k = -bSize To bSize
            For j = -bSize To bSize
                SetPixelV pctMain.hdc, x + k, y + j, colourArray(curNode).LoColour
            Next j
        Next k
        pctMain.Refresh
    Else
        Dim trgColour As Long
        trgColour = GetPixel(pctMain.hdc, x, y)
        Dim iIndex As Long
        iIndex = GetIndexFromColour(trgColour)
        If iIndex <> -1 Then
            sbStatusBar.Panels(1).Text = colourArray(iIndex).Name & " tile at min height " & SignedHeight(colourArray(iIndex).BaseAltitude)
        Else
            sbStatusBar.Panels(1).Text = "Unknown tile at unknown height"
        End If
    End If
End Sub


Private Sub tvSetup_NodeClick(ByVal Node As MSComctlLib.Node)
    Dim st As String, iDex As Long
    st = Node.key
    If st <> "Root" Then
        iDex = CLng(Right$(st, Len(st) - 3))
        With colourArray(iDex)
            curNode = -1
            txtName.Text = .Name
            txtTileLo.Text = .LoTile
            txtTileHi.Text = .HiTile
            If .BaseAltitude > 127 Then
                txtAltitude.Text = .BaseAltitude - 256
            Else
                txtAltitude.Text = .BaseAltitude
            End If
            txtColourLo.Text = .LoColour
            txtColourHi.Text = .HiColour
            lblLoColour.BackColor = .LoColour
            lblHiColour.BackColor = .HiColour
        End With
        curNode = iDex
    Else
        curNode = -1
    End If
End Sub


Private Sub txtName_Change()
    If curNode <> -1 Then
        colourArray(curNode).Name = txtName.Text
        UpdateNode "Col" & CStr(curNode), nuName
    End If
End Sub

Private Function UpdateNode(key As String, toUpdate As Long)
    Dim i As Long
    For i = 1 To tvSetup.Nodes.Count
        If tvSetup.Nodes(i).key = key Then
            Select Case toUpdate
            Case nuName
                tvSetup.Nodes(i).Text = colourArray(curNode).Name
            Case nuLoTile
            Case nuHiTile
            Case nuAltitude
            End Select
            Exit Function
        End If
    Next i
End Function

Private Sub udAltitude_Change()
    If curNode <> -1 Then
        Dim tLong As Long
        tLong = CLng(txtAltitude.Text)
        If tLong < 0 Then
            tLong = tLong + 256
        End If
        colourArray(curNode).BaseAltitude = tLong
    End If
End Sub

Private Sub udLoTile_Change()
    If curNode <> -1 Then
        colourArray(curNode).LoTile = CLng(txtTileLo.Text)
    End If
End Sub

Private Sub udTileHi_Change()
    If curNode <> -1 Then
        colourArray(curNode).HiTile = CLng(txtTileHi.Text)
    End If
End Sub

Private Function GetIndexFromColour(trgColour As Long) As Long
    Dim iCounter As Long
    GetIndexFromColour = -1
    For iCounter = 0 To UBound(colourArray)
        With colourArray(iCounter)
            If .LoColour <= trgColour And .HiColour >= trgColour Then
                GetIndexFromColour = iCounter
                Exit Function
            End If
        End With
    Next iCounter
End Function

Private Function SignedHeight(ByVal trgHeight As Long) As Long
    If trgHeight > 127 Then
        SignedHeight = trgHeight - 256
    Else
        SignedHeight = trgHeight
    End If
End Function

Private Function UnsignedHeight(ByVal trgHeight As Long) As Long
    If trgHeight < 0 Then
        UnsignedHeight = trgHeight + 256
    Else
        UnsignedHeight = trgHeight
    End If
End Function

Private Function DisplayNodes()
    Dim i As Long
    For i = tvSetup.Nodes.Count To 2 Step -1
        tvSetup.Nodes.Remove i
    Next i
    For i = 0 To UBound(colourArray)
        tvSetup.Nodes.Add "Root", tvwChild, "Col" & CStr(i), colourArray(i).Name
    Next i
    curNode = -1
    tvSetup.Refresh
End Function

Private Sub FillDisplaySegment(segNumber As Long)
    Dim Pos As Long, ISize As Long, FSize As Long, y As Long, x As Long, ArrayPos As Long
    Dim BMPInfo As BITMAPINFOHEADER
    Dim FileNum As Integer
    Dim Character As Long
    Dim n As Long
    Dim fillHeight As Long, fillWidth As Long

    BMPInfo = GetBMPInfo(picFile)
    
    If BMPInfo.biBitCount <> 32 Then
        MsgBox "You MUST have a 32 bit BMP"
        Exit Sub
    End If
    
    Select Case BMPInfo.biHeight
    Case 512, 1024, 2048, 4096:
        fillHeight = 4096 / BMPInfo.biHeight
    Case Else
        MsgBox "Invalid BMP file, bad height"
        Exit Sub
    End Select
    
    Select Case BMPInfo.biWidth
    Case 768, 1536, 3072, 6144:
        fillHeight = 6144 / BMPInfo.biWidth
    Case Else
        MsgBox "Invalid BMP file, bad width"
        Exit Sub
    End Select
    
    ISize = BMPInfo.biSizeImage
    ISize = ISize - 1
    
    FSize = FileLen(picFile)
    
    Pos = FSize - ISize
    
    FileNum = FreeFile
    Open picFile For Binary Access Read As #FileNum
    Dim i1 As Long, i2 As Long
    y = BMPInfo.biHeight - 1
    x = 0
    
    Dim loBound As Long, hiBound As Long, segmentLength As Long
    segmentLength = ISize \ 4
    loBound = segNumber * segmentLength
    hiBound = loBound + segmentLength
    Pos = Pos + loBound
    For n = loBound To hiBound
        If x >= BMPInfo.biWidth Then
            x = 0
            y = y - 1
        End If
        ArrayPos = x + y * BMPInfo.biWidth
        
        Get #FileNum, Pos + ArrayPos, Character
        HeightBMP(x, BMPInfo.biHeight - (y + 1)) = Asc(Character)
        
        x = x + 1
    Next n
    Close #FileNum
End Sub
