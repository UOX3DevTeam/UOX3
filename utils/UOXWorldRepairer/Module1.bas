Attribute VB_Name = "Module1"
Public fMainForm As frmMain
Public Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)
Public Const BlockSize As Long = 20000
Public maximumCharSect As Long
Public maximumItemSect As Long
Public Chars() As Char_st
Public Items() As Item_st
Public CharsSect() As Long
Public ItemsSect() As Long
Public CharErrors() As CharError
Public ItemErrors() As ItemError
Public NumCharError As Long, NumItemError As Long
Public itemsWsc As Long, charsWsc As Long
Public itemsOpen As Boolean, charsOpen As Boolean
Public itemCritErrorCount As Long, itemNormErrorCount As Long
Public charCritErrorCount As Long, charNormErrorCount As Long, charLayerErrorCount As Long
Public multiCount As Long
Public logFile As Long
Public charsWscString As String, itemsWscString As String

Public Const badDexterity As Long = 1
Public Const badIntelligence As Long = 2
Public Const badStrength As Long = 4
Public Const badStamina As Long = 8
Public Const badMana As Long = 16
Public Const badHP As Long = 32
Public Const badMaxHP As Long = 64
Public Const badDexterity2 As Long = 128
Public Const badStrength2 As Long = 256
Public Const badIntelligence2 As Long = 512

Public Const Dexterity As Long = 0
Public Const Intelligence As Long = 1
Public Const Strength As Long = 2
Public Const Stamina As Long = 3
Public Const Mana As Long = 4
Public Const HP As Long = 5
Public Const Dex2 As Long = 6
Public Const Str2 As Long = 7
Public Const Int2 As Long = 8

Public Const MaxLayers As Long = 30

Public Const ItemMin As Long = &H40000000
Public Const CacheMax As Byte = 19

Type Location_st
    x As Long
    y As Long
    z As Long
End Type

Public ContainerLocation As Location_st

Type LayerList
    Contents() As Long
End Type

Type Char_st
    iSect As Long
    Fixed As Boolean
    Serial As Long
    Owner As Long
    Multi As Long
    
    HitPoints As Long
    Stamina As Long
    Mana As Long
    Strength As Long
    Dexterity As Long
    Intelligence As Long
    Dexterity2 As Long
    Strength2 As Long
    Intelligence2 As Long
    
    Layers(0 To MaxLayers) As LayerList
    
    Name As String
    Title As String
    Account As Long
    
    FilePos As Long
    
    x As Long
    y As Long
    z As Long
    Direction As Byte
    
    Priv1 As Byte
    priv2 As Byte
    
    Speechcolour As Integer
    Emotecolour As Integer
    
    Skin As Long
    Body As Integer
    
    Trigword As String
    
    CanTrain As Boolean
    GuildNumber As Long
    Skills(0 To 49) As Integer
    
    Fame As Integer
    Karma As Integer
    Kills As Long
End Type

Type Item_st
    Colour As Long
    Cont As LayerList
    Container As Long
    Dexterity As Long
    FilePos As Long
    Fixed As Boolean
    HitPoints As Long
    ID As Integer
    Intelligence As Long
    iSect As Long
    iType As Integer
    Layer As Byte
    Mana As Long
    MaxHealth As Long
    Multi As Long
    Name As String
    Owner As Long
    Serial As Long
    Spawn As Long
    Stamina As Long
    Strength As Long
    x As Integer
    y As Integer
    z As Integer
    
End Type

Type Guild_st
    Name As String
End Type

Type CharError
    Index As Long           ' index into Chars()
    Layer As Boolean        ' Player has layer issues
    Account As Boolean      ' Player is an NPC with an account, or a PC with no account
    Stats As Long           ' invalid stats
    BadStats(0 To 8) As Long ' bad stats information
    Location As Byte        ' location errors
    BadLocation(0 To 2) As Long ' bad locations
    
End Type

Type ItemError
    Index As Long           ' index into Items()
    Stats As Byte           ' invalid stats
    Container As Boolean    ' self-referencing or nonexistent container
    Spawner As Boolean      ' self-referencing or nonexistent Spawner
    Owner As Boolean        ' self-referencing owner, or nonexistent owner
    SpawnData As Boolean    ' if spawner, then it spawns a nonexistent or bad item/npc
    Colour As Boolean       ' bad colour
    BadColour As Long
    Location As Byte        ' bad location
    BadLocations(0 To 2) As Long    ' actual bad location
    BadStats(0 To 6) As Long
End Type

Sub Main()
    itemCritErrorCount = 0
    charCritErrorCount = 0
    itemNormErrorCount = 0
    charNormErrorCount = 0
    charLayerErrorCount = 0
    NumCharError = 0
    NumItemError = 0
    frmSplash.Show
    frmSplash.Refresh
    LoadSkin
    Set fMainForm = New frmMain
    Load fMainForm
    Sleep 3000
    Unload frmSplash
    fMainForm.Show
    multiCount = 0
End Sub

Public Function IsInCharError(Index As Long) As Boolean
    
    Dim i As Long
    For i = 0 To NumCharError - 1
        If CharErrors(i).Index = Index Then
            IsInCharError = True
            Exit Function
        End If
    Next i
    IsInCharError = False

End Function

Public Function IsInItemError(Index As Long) As Boolean

    Dim i As Long
    For i = 0 To NumItemError - 1
        If ItemErrors(i).Index = Index Then
            IsInItemError = True
            Exit Function
        End If
    Next i
    IsInItemError = False

End Function
Public Function ItemErrorLocation(Index As Long) As Long
    Dim i As Long
    For i = 0 To NumItemError - 1
        If ItemErrors(i).Index = Index Then
            ItemErrorLocation = i
            Exit Function
        End If
    Next i
    ItemErrorLocation = -1
End Function
Public Function charErrorLocation(Index As Long) As Long
    Dim i As Long
    For i = 0 To NumCharError - 1
        If CharErrors(i).Index = Index Then
            charErrorLocation = i
            Exit Function
        End If
    Next i
    charErrorLocation = -1
End Function
Public Sub AddToCharError(desc As CharError)
    ReDim Preserve CharErrors(0 To NumCharError)
    CharErrors(NumCharError) = desc
    NumCharError = NumCharError + 1
End Sub

Public Sub AddToItemError(desc As ItemError)
    ReDim Preserve ItemErrors(0 To NumItemError)
    ItemErrors(NumItemError) = desc
    NumItemError = NumItemError + 1
End Sub

Public Sub ClearEntry(ByRef newClean As ItemError)
    newClean.Colour = False
    newClean.Container = False
    newClean.Index = -1
    newClean.Location = 0
    newClean.Owner = False
    newClean.SpawnData = False
    newClean.Spawner = False
    newClean.Stats = 0
End Sub

Public Sub ClearCharEntry(ByRef newClean As CharError)
    newClean.Account = False
    newClean.Index = -1
    newClean.Layer = False
    newClean.Location = 0
    newClean.Stats = 0
End Sub

Public Sub LogOutput(toLog As String, textOutput As TextBox, writeMode As Long)

    Select Case writeMode
    Case 0  ' no logging
    Case 1  ' text logging
        textOutput.Text = textOutput.Text & toLog & vbCrLf
    Case 2  ' file logging
        Print #logFile, toLog
    End Select
End Sub

Public Sub AnalyzeLayers(sbStatus As StatusBar, cmbCharErrors As ComboBox, lblCritCharError As Label, cmdFixLayers As CommandButton)

    Dim charCounter As Long, layerCounter As Long, onePercent As Long, charErrorLoc As Long
    Dim tempDesc As CharError
    charLayerErrorCount = 0
    onePercent = (UBound(Chars) + 1) / 100
    If onePercent = 0 Then
        onePercent = 1
    End If
    For charCounter = 0 To UBound(Chars)
        tempDesc.Index = charCounter
        tempDesc.Account = False
        tempDesc.Layer = False
        tempDesc.Stats = False
        If (charCounter Mod onePercent) = 0 Then
            sbStatus.Panels(1).Text = "Analyzing layers.... " & CStr(charCounter / onePercent) & "% done"
        End If
        For layerCounter = 0 To MaxLayers
            If UBound(Chars(charCounter).Layers(layerCounter).Contents) > 1 Then
                charLayerErrorCount = charLayerErrorCount + 1
                charErrorLoc = charErrorLocation(charCounter)
                If charErrorLoc = -1 Then
                    tempDesc.Layer = True
                    AddToCharError tempDesc
                Else
                    CharErrors(charErrorLoc).Layer = True
                End If
            End If
            If UBound(Chars(charCounter).Layers(layerCounter).Contents) = 1 Then
                If Chars(charCounter).Layers(layerCounter).Contents(0) <> -1 And Chars(charCounter).Layers(layerCounter).Contents(1) <> -1 Then
                    charLayerErrorCount = charLayerErrorCount + 1
                    charErrorLoc = charErrorLocation(charCounter)
                    If charErrorLoc <> -1 Then
                        CharErrors(charErrorLoc).Layer = True
                    Else
                        tempDesc.Layer = True
                        AddToCharError tempDesc
                    End If
                End If
            End If
        Next layerCounter
    Next charCounter
    If charLayerErrorCount > 0 Then
        sbStatus.Panels(1).Text = "Layer Errors!  Found " & CStr(charLayerErrorCount)
        lblCritCharError.Caption = CStr(charCritErrorCount + charLayerErrorCount)
        cmdFixLayers.Enabled = True
    Else
        sbStatus.Panels(1).Text = "Finished Analyzing Layers"
    End If
End Sub

