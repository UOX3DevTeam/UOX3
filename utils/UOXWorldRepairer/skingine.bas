Attribute VB_Name = "skingine"
Option Explicit
#Const DebugCode = False
Public pref1 As Integer, pref2 As Integer, pref3 As Integer
Public tFontSize As Integer

' Skin version 1 support
Type skinData_st
    tagName As String
    FileName As String
End Type

Type mainSkin_st
    mainFrm As String
    aboutButton As String
    houseButton As String
    floorButton As String
    wallButton As String
    stairButton As String
    furnButton As String
    roofButton As String
    forestButton As String
    landscapeButton As String
    specialButton As String
    gmmenuButton As String
    exitButton As String
    configButton As String
    worldButton As String
End Type
Type SkinForm
    Data() As skinData_st
    Loaded As Boolean
End Type
Type Skin_st
    formData(0 To 1) As SkinForm
    mainFrm As mainSkin_st
End Type

Public Const skinGlobal As Long = 0
Public Const skinMain As Long = 1

Public SkinData As Skin_st

Public Sub LoadSkin()

    pref1 = -1:    pref2 = -1:  pref3 = -1: tFontSize = 12
    Dim wbFSO As New FileSystemObject
    Dim wbStream As TextStream
#If DebugCode = True Then
    If Not wbFSO.FileExists("c:\dev\tools\uox\tileviewer\skin.lng") Then
#Else
    If Not wbFSO.FileExists(App.Path & "\skin.lng") Then
#End If
        Exit Sub
    End If
#If DebugCode = True Then
    Set wbStream = wbFSO.OpenTextFile("C:\dev\tools\uox\tileviewer\skin.lng")
#Else
    Set wbStream = wbFSO.OpenTextFile(App.Path & "\skin.lng")
#End If
    Dim sLine As String
    Dim outOfSection As Boolean
'    frmTileViewer.sbStatusBar.Panels(1).Text = "Loading skin now"
'    frmTileViewer.sbStatusBar.Refresh
    Dim sSection As String
    Dim f1Name As String, f2Name As String, f3Name As String
    f1Name = ""
    f2Name = ""
    f3Name = ""
    Do While Not wbStream.AtEndOfStream
        If sSection = "" Then
            sLine = wbStream.ReadLine
        Else
            sLine = sSection
            sSection = ""
        End If
        If UCase(Trim$(sLine)) = "" Then
        ElseIf UCase(left$(sLine, 6)) = "[MAIN]" Then
            sSection = GenerateFormSkinStruct(skinMain, wbStream)
        ElseIf UCase(left(sLine, 8)) = "[GLOBAL]" Then
            sSection = GenerateFormSkinStruct(skinGlobal, wbStream)
        ElseIf UCase(left$(sLine, 6)) = "[FONT]" Then
            outOfSection = False
            Do While Not outOfSection And Not wbStream.AtEndOfStream
                sLine = wbStream.ReadLine
                If UCase(Trim$(sLine)) = "" Then
                
                ElseIf left$(sLine, 1) = "[" Then
                    outOfSection = True
                ElseIf UCase(left$(sLine, 5)) = "PREF1" Then
                    f1Name = right$(sLine, Len(sLine) - 6)
                ElseIf UCase(left$(sLine, 5)) = "PREF2" Then
                    f2Name = right$(sLine, Len(sLine) - 6)
                ElseIf UCase(left$(sLine, 5)) = "PREF3" Then
                    f3Name = right$(sLine, Len(sLine) - 6)
                ElseIf UCase(left$(sLine, 4)) = "SIZE" Then
                    tFontSize = CLng(right$(sLine, Len(sLine) - 5))
                End If
            Loop
            If outOfSection Then
                sSection = sLine
            End If
        End If
    Loop
 '   frmTileViewer.sbStatusBar.Panels(1).Text = "Finding preferred fonts"
'    frmTileViewer.sbStatusBar.Refresh
    Dim tmpFont As New StdFont
    Dim i As Long
    frmSplash.pgbProgress.Min = 0
    frmSplash.pgbProgress.Max = Screen.FontCount - 1
    If f1Name <> "" Or f2Name <> "" Or f3Name <> "" Then
        For i = 0 To Screen.FontCount - 1
            tmpFont.Name = Screen.Fonts(i)
            frmSplash.pgbProgress.Value = i
            frmSplash.pgbProgress.Refresh
            If f1Name <> "" And InStr(1, tmpFont.Name, f1Name, vbTextCompare) Then
                pref1 = i
            ElseIf f2Name <> "" And InStr(1, tmpFont.Name, f2Name, vbTextCompare) Then
                pref2 = i
            ElseIf f3Name <> "" And InStr(1, tmpFont.Name, f3Name, vbTextCompare) Then
                pref3 = i
            End If
            If pref1 <> -1 And pref2 <> -1 And pref3 <> -1 Then
                Exit For
            End If
        Next i
    End If
End Sub

Public Sub LoadFormSkinStruct(trgForm As Form, Index As Long, backPic As Image)

    Dim cCounter As Long
    Dim cCount As Long, eCount As Long  ' cCount is control count, eCount is entry count
    Dim iCounter As Long
    Dim sTag As String, sFile As String
    Dim bFound As Boolean
    Dim fTest As New FileSystemObject
    Dim tmpControl As Control
    trgForm.sbStatusBar.Panels(1).Text = "Applying skin"
    trgForm.sbStatusBar.Refresh
    If (pref1 <> -1) Then
        trgForm.Font = Screen.Fonts(pref1)
        trgForm.Font.Size = tFontSize
    ElseIf (pref2 <> -1) Then
        trgForm.Font = Screen.Fonts(pref2)
        trgForm.Font.Size = tFontSize
    ElseIf (pref3 <> -1) Then
        trgForm.Font = Screen.Fonts(pref3)
        trgForm.Font.Size = tFontSize
    End If
    If SkinData.formData(Index).Loaded Then
        For iCounter = LBound(SkinData.formData(Index).Data) To UBound(SkinData.formData(Index).Data)
            If SkinData.formData(Index).Data(iCounter).tagName = "BACKPIC" Then
                backPic.Picture = LoadPicture(SkinData.formData(Index).Data(iCounter).FileName)
                backPic.Stretch = True
                backPic.Move 0, 0, trgForm.ScaleWidth, trgForm.ScaleHeight
                backPic.Visible = True
                backPic.ZOrder 1
                iCounter = UBound(SkinData.formData(Index).Data) + 1
            End If
        Next iCounter
    End If
    For cCounter = 0 To trgForm.Controls.Count - 1
        Set tmpControl = trgForm.Controls(cCounter)
        If TypeOf tmpControl Is Label Then
            tmpControl.Font = trgForm.Font
            tmpControl.Font.Size = trgForm.Font.Size
        End If
        bFound = False
        If SkinData.formData(0).Loaded Then
            For iCounter = LBound(SkinData.formData(0).Data) To UBound(SkinData.formData(0).Data)
                sTag = SkinData.formData(0).Data(iCounter).tagName
                sFile = SkinData.formData(0).Data(iCounter).FileName
                If DoTagPictureLoad(tmpControl, sTag, sFile) Then
                    bFound = True
                    Exit For
                End If
            Next iCounter
        End If
        If Not bFound And SkinData.formData(Index).Loaded Then
            For iCounter = LBound(SkinData.formData(Index).Data) To UBound(SkinData.formData(Index).Data)
                sTag = SkinData.formData(Index).Data(iCounter).tagName
                sFile = SkinData.formData(Index).Data(iCounter).FileName
                If DoTagPictureLoad(tmpControl, sTag, sFile) Then
                    Exit For
                End If
            Next iCounter
        End If
    Next cCounter
    Dim targColour As Long
    If SkinData.formData(0).Loaded Then
        For eCount = 0 To UBound(SkinData.formData(0).Data)
            If SkinData.formData(0).Data(eCount).tagName = "BACKCOLOUR" Then
                targColour = CLng("&H" & SkinData.formData(0).Data(eCount).FileName)
                trgForm.BackColor = targColour
                DoBackColour trgForm, targColour
            ElseIf SkinData.formData(0).Data(eCount).tagName = "FORECOLOUR" Then
                targColour = CLng("&H" & SkinData.formData(0).Data(eCount).FileName)
                trgForm.ForeColor = targColour
                DoForeColour trgForm, targColour
            End If
        Next eCount
    End If
    trgForm.sbStatusBar.Panels(1).Text = "Skin applied... ready to go"
    trgForm.Refresh
End Sub

Public Function GenerateFormSkinStruct(trgForm As Long, fileStream As TextStream) As String

    Dim outOfSection As Boolean
    outOfSection = False
    Dim sLine As String
    Dim fTest As New FileSystemObject
    Dim cCounter As Long
    Dim spcCount As Long
    Dim spcPos As Long
    Dim sTag As String
    Dim sFile As String
    Dim iTargElement As Long, iUpperBound As Long
    On Error GoTo errorcatch
    Do While Not fileStream.AtEndOfStream And Not outOfSection
        sLine = fileStream.ReadLine
        If left$(sLine, 1) = "[" Then
            outOfSection = True
        ElseIf Len(Trim$(sLine)) = 0 Then
            ' skip over
        Else
            SkinData.formData(trgForm).Loaded = True
            spcCount = SpaceCount(sLine)
            If spcCount >= 1 Then
                spcPos = GetSpaceLocation(sLine, 1)
                sTag = Trim$(left$(sLine, spcPos - 1))
                Select Case sTag
                Case "BACKCOLOUR", "FORECOLOUR"
                    sFile = right$(sLine, Len(sLine) - spcPos + 1)
                Case Else
#If DebugCode = True Then
                    sFile = "C:\dev\tools\uox\tileviewer\skin\" & right$(sLine, Len(sLine) - spcPos + 1)
#Else
                    sFile = App.Path & "\Skin\" & right$(sLine, Len(sLine) - spcPos + 1)
#End If
                End Select
                iUpperBound = UBound(SkinData.formData(trgForm).Data)
                ReDim Preserve SkinData.formData(trgForm).Data(0 To iUpperBound + 1)
                iTargElement = iUpperBound + 1
                SkinData.formData(trgForm).Data(iTargElement).FileName = sFile
                SkinData.formData(trgForm).Data(iTargElement).tagName = sTag
            Else
                MsgBox "Malformed skin line at line " & fileStream.Line
            End If
        End If
    Loop
    If outOfSection Then
        GenerateFormSkinStruct = sLine
    End If
    Exit Function
errorcatch:
    If Err.Number = 9 Then  ' subscript out of range, hence, our array isn't inited
        iUpperBound = -1    ' we'll redim it from 0 to 0
        Resume Next
    Else
        MsgBox Err.Number & ": " & Err.Description, vbCritical + vbOKOnly, App.Title
    End If
End Function


Public Function SpaceCount(sLine As String) As Long

    Dim startPos As Long
    Dim foundPos As Long
    Dim backupPos As Long
    startPos = 1
    foundPos = 1
    
    SpaceCount = 0
    Do While (Not IsNull(foundPos) And foundPos <> 0)
        backupPos = foundPos
        foundPos = InStr(startPos, sLine, " ")
        If Not IsNull(foundPos) And foundPos <> 0 Then
            SpaceCount = SpaceCount + 1
        End If
        startPos = foundPos + 1
    Loop

End Function
Public Function GetSpaceLocation(sLine As String, iLocation As Long) As Long

    If iLocation = 0 Then
        GetSpaceLocation = 1
        Exit Function
    End If
    
    Dim startPos As Long
    Dim foundPos As Long
    Dim backupPos As Long
    startPos = 1
    foundPos = 1
    Dim currentCount As Long
    currentCount = 0
    Do While (Not IsNull(foundPos) And foundPos <> 0 And iLocation <> currentCount)
        backupPos = foundPos
        foundPos = InStr(startPos, sLine, " ")
        If Not IsNull(foundPos) And foundPos <> 0 Then
            currentCount = currentCount + 1
        End If
        startPos = foundPos + 1
    Loop
    If iLocation = currentCount Then
        GetSpaceLocation = foundPos + 1
    Else
        GetSpaceLocation = -1
    End If
End Function

Public Sub DoBackColour(trgForm As Form, backColour As Long)
    Dim cCounter As Long, tmpControl As Control
    For cCounter = 0 To trgForm.Controls.Count - 1
        Set tmpControl = trgForm.Controls(cCounter)
        If TypeOf tmpControl Is Frame Then
            tmpControl.BackColor = backColour
        ElseIf TypeOf tmpControl Is OptionButton Then
            tmpControl.BackColor = backColour
        ElseIf TypeOf tmpControl Is CheckBox Then
            tmpControl.BackColor = backColour
        ElseIf TypeOf tmpControl Is Label Then
            tmpControl.BackColor = backColour
        End If
    Next cCounter
End Sub

Public Sub DoForeColour(trgForm As Form, foreColour As Long)
    Dim cCounter As Long, tmpControl As Control
    For cCounter = 0 To trgForm.Controls.Count - 1
        Set tmpControl = trgForm.Controls(cCounter)
        If TypeOf tmpControl Is Frame Then
            tmpControl.ForeColor = foreColour
        ElseIf TypeOf tmpControl Is OptionButton Then
            tmpControl.ForeColor = foreColour
        ElseIf TypeOf tmpControl Is CheckBox Then
            tmpControl.ForeColor = foreColour
        ElseIf TypeOf tmpControl Is Label Then
            tmpControl.ForeColor = foreColour
        End If
    Next cCounter
End Sub

Public Function DoTagPictureLoad(tmpControl As Control, sTag As String, sFile As String) As Boolean
    Dim fTest As New FileSystemObject
    If tmpControl.Tag = sTag Then
        If fTest.FileExists(sFile) Then
            tmpControl.Picture = LoadPicture(sFile)
            tmpControl.ToolTipText = tmpControl.Caption
            tmpControl.Caption = ""
            DoTagPictureLoad = True
            Exit Function
        End If
    End If
    DoTagPictureLoad = False
End Function

