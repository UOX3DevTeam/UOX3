Attribute VB_Name = "loading"
Option Explicit
Public Sub LoadChars(sbStatusBar As StatusBar, txtLoggingOutput As TextBox, dMode As Long, mnuFileOpen As Menu, lblStatsCharActualCount As Label)

    Dim currentChar As Long
    Dim tempPos As Long
    Dim inSection As Boolean
    Dim sLine As String
    Dim tempLong As Long
    Dim layerCounter As Long
    inSection = False
    mnuFileOpen.Enabled = False
    Dim onePercent As Long
    onePercent = UBound(Chars) / 99
    If onePercent = 0 Then
        onePercent = 1
    End If
    Dim iSect As Long, iLine As Long
    Dim blah As CharError
    Dim addError As Boolean
    For currentChar = 0 To UBound(Chars)
        iSect = -1
        If (currentChar Mod onePercent) = 0 Then
            sbStatusBar.Panels(1).Text = "Loading Chars... " & CStr(currentChar / onePercent) & "% done"
        End If
        
        addError = False
        ClearCharStructure Chars(currentChar)
        ClearCharEntry blah
        blah.Index = currentChar
        For layerCounter = 0 To MaxLayers
            ReDim Chars(currentChar).Layers(layerCounter).Contents(0 To 0)
            Chars(currentChar).Layers(layerCounter).Contents(0) = -1
        Next layerCounter
        Chars(currentChar).CanTrain = True
        Chars(currentChar).Fixed = False
        Do While Not inSection
            If EOF(charsWsc) Then
                sLine = "{"
            Else
                Line Input #charsWsc, sLine
            End If
            If EOF(charsWsc) Then       ' if by some bizarre chance we hit EOF before finishing loading the file
                sLine = "{"
            End If
            iLine = InStr(1, sLine, "SECTION CHARACTER")
            If iLine <> 0 Then
                iSect = CLng(Mid$(sLine, iLine + 17, Len(sLine) - (iLine + 16)))
            End If
            tempPos = InStr(1, sLine, "{")
            If (tempPos <> 0 And Not IsNull(tempPos)) Then
                inSection = True
            End If
        Loop
        Chars(currentChar).iSect = iSect
        If iSect > maximumCharSect Then
            maximumCharSect = iSect
        End If
        Do While (Not EOF(charsWsc) And inSection)
            If EOF(charsWsc) Then
                sLine = "}"
            Else
                Line Input #charsWsc, sLine
            End If
            If left$(sLine, 1) = "}" Then
                inSection = False
            Else
                Select Case left$(sLine, 1)
                Case "A"
                    If left$(sLine, 7) = "ACCOUNT" Then
                        Chars(currentChar).Account = CLng(Mid$(sLine, 8, Len(sLine) - 7))
                    ElseIf left$(sLine, 7) = "ALLMOVE" Then
                        tempLong = CLng(Mid$(sLine, 8, Len(sLine) - 7))
                        If tempLong < 0 Or tempLong > 255 Then
                            charNormErrorCount = charNormErrorCount + 1
                            Chars(currentChar).priv2 = 0
                        Else
                            Chars(currentChar).priv2 = tempLong
                        End If
                    End If
                Case "B"
                    If left$(sLine, 4) = "BODY" Then
                        tempLong = CLng(Mid$(sLine, 6, Len(sLine) - 4))
                        If tempLong < 0 Or tempLong > 32767 Then
                            Chars(currentChar).Body = tempLong
                            charNormErrorCount = charNormErrorCount + 1
                        Else
                            Chars(currentChar).Body = tempLong
                        End If
                    End If
                Case "D"
                    If left$(sLine, 10) = "DEXTERITY " Then
                        tempLong = CLng(Mid$(sLine, 11, Len(sLine) - 10))
                        If tempLong > 32767 Or tempLong < 0 Then
                            Chars(currentChar).Dexterity = tempLong
                            charNormErrorCount = charNormErrorCount + 1
                            blah.Stats = blah.Stats + badDexterity
                            blah.BadStats(Dexterity) = tempLong
                            addError = True
                        Else
                            Chars(currentChar).Dexterity = tempLong
                        End If
                    ElseIf left$(sLine, 10) = "DEXTERITY2" Then
                        tempLong = CLng(Mid$(sLine, 11, Len(sLine) - 10))
                        If tempLong > 32767 Or tempLong < 0 Then
                            Chars(currentChar).Dexterity2 = tempLong
                            charNormErrorCount = charNormErrorCount + 1
                            blah.Stats = blah.Stats + badDexterity2
                            blah.BadStats(Dex2) = tempLong
                            addError = True
                        Else
                            Chars(currentChar).Dexterity2 = tempLong
                        End If
                    ElseIf left$(sLine, 9) = "DIRECTION" Then
                        tempLong = CLng(Mid$(sLine, 11, Len(sLine) - 10))
                        If tempLong > 255 Then
                            Chars(currentChar).Direction = 0
                            charNormErrorCount = charNormErrorCount + 1
                        Else
                            Chars(currentChar).Direction = tempLong
                        End If
                    End If
                Case "E"
                    If left$(sLine, 5) = "EMOTE" Then
                        tempLong = CLng(Mid$(sLine, 6, Len(sLine) - 5))
                        If tempLong < 0 Or tempLong > 65535 Then
                            charNormErrorCount = charNormErrorCount + 1
                            Chars(currentChar).Emotecolour = 0
                        Else
                            Chars(currentChar).Emotecolour = tempLong
                        End If
                    End If
                Case "F"
                    If left$(sLine, 4) = "FAME" Then
                        tempLong = CLng(Mid$(sLine, 5, Len(sLine) - 4))
                        If tempLong < 0 Or tempLong > 10000 Then
                            charNormErrorCount = charNormErrorCount + 1
                            Chars(currentChar).Fame = 0
                        Else
                            Chars(currentChar).Fame = tempLong
                        End If
                    End If
                Case "G"
                    If left$(sLine, 11) = "GUILDNUMBER" Then
                        Chars(currentChar).GuildNumber = CLng(Mid$(sLine, 12, Len(sLine) - 11))
                    End If
                Case "H"
                    If left$(sLine, 9) = "HITPOINTS" Then
                        tempLong = CLng(Mid$(sLine, 10, Len(sLine) - 9))
                        If tempLong < 0 Or tempLong > 32767 Then
                            charNormErrorCount = charNormErrorCount + 1
                            blah.Stats = blah.Stats + badHP
                            blah.BadStats(HP) = tempLong
                            addError = True
                            Chars(currentChar).HitPoints = tempLong
                        Else
                            Chars(currentChar).HitPoints = tempLong
                        End If
                    End If
                Case "K"
                    If left$(sLine, 5) = "KILLS" Then
                        Chars(currentChar).Kills = CLng(Mid$(sLine, 6, Len(sLine) - 5))
                    ElseIf left$(sLine, 5) = "KARMA" Then
                        tempLong = CLng(Mid$(sLine, 6, Len(sLine) - 5))
                        If tempLong < -10000 Or tempLong > 10000 Then
                            charNormErrorCount = charNormErrorCount + 1
                        Else
                            Chars(currentChar).Karma = tempLong
                        End If
                    End If
                Case "I"
                    If left$(sLine, 13) = "INTELLIGENCE " Then
                        tempLong = CLng(Mid$(sLine, 13, Len(sLine) - 12))
                        If tempLong < 0 Or tempLong > 32767 Then
                            charNormErrorCount = charNormErrorCount + 1
                            addError = True
                            blah.Stats = blah.Stats + badIntelligence
                            blah.BadStats(Intelligence) = tempLong
                            Chars(currentChar).Intelligence = tempLong
                        Else
                            Chars(currentChar).Intelligence = tempLong
                        End If
                    ElseIf left$(sLine, 13) = "INTELLIGENCE2" Then
                        tempLong = CLng(Mid$(sLine, 14, Len(sLine) - 13))
                        If tempLong < 0 Or tempLong > 32767 Then
                            charNormErrorCount = charNormErrorCount + 1
                            addError = True
                            blah.Stats = blah.Stats + badIntelligence2
                            blah.BadStats(Int2) = tempLong
                            Chars(currentChar).Intelligence2 = tempLong
                        Else
                            Chars(currentChar).Intelligence2 = tempLong
                        End If
                    End If
                Case "M"
                    If left$(sLine, 4) = "MANA" Then
                        tempLong = CLng(Mid$(sLine, 6, Len(sLine) - 5))
                        If tempLong > 32767 Or tempLong < 0 Then
                            Chars(currentChar).Mana = tempLong
                            charNormErrorCount = charNormErrorCount + 1
                            addError = True
                            blah.Stats = blah.Stats + badMana
                            blah.BadStats(Mana) = tempLong
                        Else
                            Chars(currentChar).Mana = tempLong
                        End If
                    End If
                Case "N"
                    If left$(sLine, 4) = "NAME" Then
                        Chars(currentChar).Name = Mid$(sLine, 6, Len(sLine) - 5)
                    ElseIf left$(sLine, 7) = "NOTRAIN" Then
                        Chars(currentChar).CanTrain = False
                    End If
                Case "O"
                    If left$(sLine, 5) = "OWNER" Then
                        Chars(currentChar).Owner = CLng(Mid$(sLine, 7, Len(sLine) - 6))
                    End If
                Case "P"
                    If left$(sLine, 4) = "PRIV" Then
                        tempLong = CLng(Mid$(sLine, 6, Len(sLine) - 5))
                        If tempLong < 0 Or tempLong > 255 Then
                            charNormErrorCount = charNormErrorCount + 1
                            Chars(currentChar).Priv1 = 0
                        Else
                            Chars(currentChar).Priv1 = tempLong
                        End If
                    End If
                Case "S"
                    If left$(sLine, 6) = "SERIAL" Then
                        Chars(currentChar).Serial = CLng(Mid$(sLine, 8, Len(sLine) - 7))
                    ElseIf left$(sLine, 7) = "STAMINA" Then
                        tempLong = CLng(Mid$(sLine, 9, Len(sLine) - 8))
                        If tempLong > 32767 Or tempLong < 0 Then
                            Chars(currentChar).Stamina = tempLong
                            charNormErrorCount = charNormErrorCount + 1
                            addError = True
                            blah.Stats = blah.Stats + badStamina
                            blah.BadStats(Stamina) = tempLong
                        Else
                            Chars(currentChar).Stamina = tempLong
                        End If
                    ElseIf left$(sLine, 9) = "STRENGTH " Then
                        tempLong = CLng(Mid$(sLine, 10, Len(sLine) - 9))
                        If tempLong > 32767 Or tempLong < 0 Then
                            Chars(currentChar).Strength = tempLong
                            charNormErrorCount = charNormErrorCount + 1
                            addError = True
                            blah.Stats = blah.Stats + badStrength
                            blah.BadStats(Strength) = tempLong
                        Else
                            Chars(currentChar).Strength = tempLong
                        End If
                    ElseIf left$(sLine, 9) = "STRENGTH2" Then
                        tempLong = CLng(Mid$(sLine, 11, Len(sLine) - 10))
                        If tempLong > 32767 Or tempLong < 0 Then
                            Chars(currentChar).Strength2 = tempLong
                            charNormErrorCount = charNormErrorCount + 1
                            addError = True
                            blah.Stats = blah.Stats + badStrength2
                            blah.BadStats(Str2) = tempLong
                        Else
                            Chars(currentChar).Strength2 = tempLong
                        End If
                    ElseIf left$(sLine, 3) = "SAY" Then
                        tempLong = CLng(Mid$(sLine, 4, Len(sLine) - 3))
                        If tempLong < 0 Or tempLong > 65535 Then
                            charNormErrorCount = charNormErrorCount + 1
                            Chars(currentChar).Speechcolour = 0
                        Else
                            Chars(currentChar).Speechcolour = tempLong
                        End If
                    ElseIf left$(sLine, 4) = "SKIN" Then
                        tempLong = CLng(Mid$(sLine, 6, Len(sLine) - 4))
                        If tempLong < 0 Or tempLong > 65535 Then
                            charNormErrorCount = charNormErrorCount + 1
                            Chars(currentChar).Skin = tempLong
                        Else
                            Chars(currentChar).Skin = tempLong
                        End If
                    ElseIf left$(sLine, 5) = "SKILL" Then
                        Dim sNum As Long
                        sNum = CLng(Mid$(sLine, 6, 2))
                        If sNum < 0 Or sNum > 50 Then
                            charNormErrorCount = charNormErrorCount + 1
                        Else
                            Chars(currentChar).Skills(sNum) = CLng(Mid$(sLine, 8, Len(sLine) - 7))
                        End If
                    End If
                Case "T"
                    If left$(sLine, 5) = "TITLE" Then
                        Chars(currentChar).Title = Mid$(sLine, 7, Len(sLine) - 5)
                    ElseIf left$(sLine, 8) = "TRIGWORD" Then
                        Chars(currentChar).Trigword = Mid$(sLine, 10, Len(sLine) - 8)
                    End If
                Case "X"
                    If left$(sLine, 2) = "X " Then
                        tempLong = CLng(Mid$(sLine, 3, Len(sLine) - 2))
                        If tempLong > 7000 Or tempLong < 0 Then
                            Chars(currentChar).x = tempLong
                            addError = True
                            blah.Location = blah.Location + 1
                            blah.BadLocation(0) = tempLong
                            charNormErrorCount = charNormErrorCount + 1
                        Else
                            Chars(currentChar).x = tempLong
                        End If
                    End If
                Case "Y"
                    If left$(sLine, 1) = "Y" Then
                        tempLong = CLng(Mid$(sLine, 3, Len(sLine) - 2))
                        If tempLong > 7000 Or tempLong < 0 Then
                            Chars(currentChar).y = tempLong
                            charNormErrorCount = charNormErrorCount + 1
                            addError = True
                            blah.Location = blah.Location + 2
                            blah.BadLocation(1) = tempLong
                        Else
                            Chars(currentChar).y = tempLong
                        End If
                    End If
                Case "Z"
                    If left$(sLine, 1) = "Z" Then
                        tempLong = CLng(Mid$(sLine, 3, Len(sLine) - 2))
                        If tempLong > 128 Or tempLong < -128 Then
                            Chars(currentChar).z = tempLong
                            charNormErrorCount = charNormErrorCount + 1
                            addError = True
                            blah.Location = blah.Location + 4
                            blah.BadLocation(2) = tempLong
                        Else
                            Chars(currentChar).z = tempLong
                        End If
                    End If
                End Select
            End If
        Loop
        If addError Then
            AddToCharError blah
            Chars(currentChar).Fixed = True
        End If
        DoEvents
    Next currentChar
    mnuFileOpen.Enabled = True
    sbStatusBar.Panels(1).Text = "Chars Loaded!"
    lblStatsCharActualCount.Caption = CStr(UBound(Chars) + 1)
End Sub

Public Sub LoadItems(sbStatusBar As StatusBar, txtLoggingOutput As TextBox, dMode As Long, mnuFileOpen As Menu, lblStatsItemActualCount As Label)

    Dim currentItem As Long
    Dim tempPos As Long
    Dim inSection As Boolean
    Dim sLine As String
    Dim tempLong As Long
    Dim partialCandidate As Boolean
    inSection = False
    Dim onePercent As Long
    onePercent = UBound(Items) / 99
    If onePercent = 0 Then
        onePercent = 1
    End If
    Dim iSect As Long, iLine As Long
    For currentItem = 0 To UBound(Items)
        iSect = -1
        If (currentItem Mod onePercent) = 0 Then
            sbStatusBar.Panels(1).Text = "Loading Items... " & CStr(currentItem / onePercent) & "% done"
        End If
        ReDim Items(currentItem).Cont.Contents(0 To 0)
        Items(currentItem).Cont.Contents(0) = -1
        partialCandidate = False
        Do While Not inSection
            If EOF(itemsWsc) Then
                sLine = ""
            Else
                Line Input #itemsWsc, sLine
            End If
            tempPos = InStr(1, sLine, "WORLDITEM")  ' if the section name doesn't have WORLDITEM in it, it's not an item!
            If (tempPos <> 0 And Not IsNull(tempPos)) Then
                partialCandidate = True
                iSect = CLng(Mid$(sLine, tempPos + 9, Len(sLine) - (tempPos + 8)))
            End If
            If EOF(itemsWsc) Then       ' if by some bizarre chance we hit EOF before finishing loading the file
                sLine = "{"
                partialCandidate = True
            End If
            If partialCandidate Then    ' if we're not even in a valid world section, don't bother!
                tempPos = InStr(1, sLine, "{")
                If (tempPos <> 0 And Not IsNull(tempPos)) Then
                    inSection = True
                End If
            End If
        Loop
        Dim blah2 As ItemError
        ClearItemStructure Items(currentItem)
        ClearEntry blah2
        Dim addError As Boolean
        addError = False
        blah2.Index = currentItem
        Items(currentItem).FilePos = Seek(itemsWsc)
        Items(currentItem).iSect = iSect
        If iSect > maximumItemSect Then
            maximumItemSect = iSect
        End If
        Do While (Not EOF(itemsWsc) And inSection)
            If EOF(itemsWsc) Then
                sLine = "}"
            Else
                Line Input #itemsWsc, sLine
            End If
            If left$(sLine, 1) = "}" Then
                inSection = False
            Else
                Select Case left$(sLine, 1)
                Case "C"
                    If left$(sLine, 4) = "CONT" Then
                        Items(currentItem).Container = CLng(Mid$(sLine, 6, Len(sLine) - 5))
                    ElseIf left$(sLine, 5) = "COLOR" Then
                        tempLong = CLng(Mid$(sLine, 7, Len(sLine) - 6))
                        If tempLong < 0 Or tempLong > 65535 Then
                            itemNormErrorCount = itemNormErrorCount + 1
                            Items(currentItem).Colour = tempLong
                            addError = True
                            blah2.Colour = True
                            blah2.BadColour = tempLong
                        Else
                            Items(currentItem).Colour = tempLong
                        End If
                    End If
                Case "D"
                    If left$(sLine, 3) = "DX " Then
                        tempLong = CLng(Mid$(sLine, 4, Len(sLine) - 3))
                        If tempLong > 65535 Or tempLong < 0 Then
                            LogOutput "Item " & Items(currentItem).Name & " with serial " & CStr(Items(currentItem).Serial) & " has an invalid dexterity value " & CStr(tempLong), txtLoggingOutput, dMode
                            Items(currentItem).Dexterity = tempLong
                            itemNormErrorCount = itemNormErrorCount + 1
                            blah2.Stats = blah2.Stats + 1
                            blah2.BadStats(0) = tempLong
                            addError = True
                        Else
                            Items(currentItem).Dexterity = tempLong
                        End If
                    End If
                Case "H"
                    If left$(sLine, 2) = "HP" Then
                        tempLong = CLng(Mid$(sLine, 4, Len(sLine) - 3))
                        If tempLong < 0 Or tempLong > 32768 Then
                            itemNormErrorCount = itemNormErrorCount + 1
                            Items(currentItem).HitPoints = tempLong
                            addError = True
                            blah2.Stats = blah2.Stats + 32
                            blah2.BadStats(5) = tempLong
                        Else
                            Items(currentItem).HitPoints = tempLong
                        End If
                    End If
                Case "I"
                    If left$(sLine, 2) = "ID" Then
                        tempLong = CLng(Mid$(sLine, 4, Len(sLine) - 3))
                        If tempLong > 65535 Then
                            LogOutput "Item " & Items(currentItem).Name & " with serial " & CStr(Items(currentItem).Serial) & " has an invalid ID value" & CStr(tempLong), txtLoggingOutput, dMode
                            Items(currentItem).ID = 0
                            itemNormErrorCount = itemNormErrorCount + 1
                        Else
                            Items(currentItem).ID = tempLong
                            If tempLong >= &H4000 Then
                                multiCount = multiCount + 1
                            End If
                        End If
                    ElseIf left$(sLine, 3) = "IN " Then
                        tempLong = CLng(Mid$(sLine, 4, Len(sLine) - 3))
                        If tempLong > 65535 Or tempLong < 0 Then
                            LogOutput "Item " & Items(currentItem).Name & " with serial " & CStr(Items(currentItem).Serial) & " has an invalid intelligence value " & CStr(tempLong), txtLoggingOutput, dMode
                            Items(currentItem).Intelligence = tempLong
                            blah2.Stats = blah2.Stats + 2
                            blah2.BadStats(1) = tempLong
                            addError = True
                            itemNormErrorCount = itemNormErrorCount + 1
                        Else
                            Items(currentItem).Intelligence = tempLong
                        End If
                    End If
                Case "L"
                    If left$(sLine, 5) = "LAYER" Then
                        Items(currentItem).Layer = CByte(Mid$(sLine, 7, Len(sLine) - 6))
                    End If
                Case "M"
                    If left$(sLine, 4) = "MANA" Then
                        tempLong = CLng(Mid$(sLine, 6, Len(sLine) - 5))
                        If tempLong > 65535 Then
                            LogOutput "Item " & Items(currentItem).Name & " with serial " & CStr(Items(currentItem).Serial) & " has an invalid mana value " & CStr(tempLong), txtLoggingOutput, dMode
                            Items(currentItem).Mana = tempLong
                            blah2.Stats = blah2.Stats + 16
                            blah2.BadStats(4) = tempLong
                            addError = True
                            itemNormErrorCount = itemNormErrorCount + 1
                        Else
                            Chars(currentItem).Mana = tempLong
                        End If
                    ElseIf left$(sLine, 5) = "MAXHP" Then
                        tempLong = CLng(Mid$(sLine, 7, Len(sLine) - 6))
                        If tempLong < 0 Or tempLong > 32768 Then
                            itemNormErrorCount = itemNormErrorCount + 1
                            addError = True
                            blah2.Stats = blah2.Stats + 64
                            blah2.BadStats(6) = tempLong
                            Items(currentItem).MaxHealth = tempLong
                        Else
                            Items(currentItem).MaxHealth = tempLong
                        End If
                    End If
                Case "N"
                    If left$(sLine, 5) = "NAME " Then
                        Items(currentItem).Name = Mid$(sLine, 6, Len(sLine) - 5)
                    End If
                Case "O"
                    If left$(sLine, 5) = "OWNER" Then
                        Items(currentItem).Owner = CLng(Mid$(sLine, 7, Len(sLine) - 6))
                    End If
                Case "S"
                    If left$(sLine, 6) = "SERIAL" Then
                        Items(currentItem).Serial = CLng(Mid$(sLine, 8, Len(sLine) - 7))
                    ElseIf left$(sLine, 7) = "STAMINA" Then
                        tempLong = CLng(Mid$(sLine, 9, Len(sLine) - 8))
                        If tempLong > 65535 Then
                            LogOutput "Item " & Items(currentItem).Name & " with serial " & CStr(Items(currentItem).Serial) & " has an invalid stamina value " & CStr(tempLong), txtLoggingOutput, dMode
                            Items(currentItem).Stamina = tempLong
                            blah2.Stats = blah2.Stats + 8
                            blah2.BadStats(3) = tempLong
                            charNormErrorCount = charNormErrorCount + 1
                        Else
                            Items(currentItem).Stamina = tempLong
                        End If
                    ElseIf left$(sLine, 3) = "ST " Then
                        tempLong = CLng(Mid$(sLine, 4, Len(sLine) - 3))
                        If tempLong > 65535 Or tempLong < 0 Then
                            LogOutput "Item " & Items(currentItem).Name & " with serial " & CStr(Items(currentItem).Serial) & " has an invalid strength value " & CStr(tempLong), txtLoggingOutput, dMode
                            Items(currentItem).Strength = tempLong
                            addError = True
                            blah2.Stats = blah2.Stats + 4
                            blah2.BadStats(2) = tempLong
                            itemNormErrorCount = charNormErrorCount + 1
                        Else
                            Items(currentItem).Strength = tempLong
                        End If
                    ElseIf left$(sLine, 5) = "SPAWN" Then
                        Items(currentItem).Spawn = CLng(Mid$(sLine, 7, Len(sLine) - 5))
                    End If
                Case "T"
                    If left$(sLine, 5) = "TYPE " Then
                        tempLong = CLng(Mid$(sLine, 6, Len(sLine) - 5))
                        If tempLong < 0 Or tempLong > 32768 Then
                            itemNormErrorCount = itemNormErrorCount + 1
                            Items(currentItem).iType = 0
                        Else
                            Items(currentItem).iType = tempLong
                        End If
                    End If
                Case "X"
                    If left$(sLine, 2) = "X " Then
                        tempLong = CLng(Mid$(sLine, 3, Len(sLine) - 2))
                        If tempLong > 6144 Or tempLong < 0 Then
                            LogOutput "Item " & Items(currentItem).Name & " with serial " & CStr(Items(currentItem).Serial) & " has an x value " & CStr(tempLong), txtLoggingOutput, dMode
                            Items(currentItem).x = tempLong
                            blah2.Location = blah2.Location + 1
                            blah2.BadLocations(0) = tempLong
                            addError = True
                            itemNormErrorCount = itemNormErrorCount + 1
                        Else
                            Items(currentItem).x = tempLong
                        End If
                    End If
                Case "Y"
                    If left$(sLine, 1) = "Y" Then
                        tempLong = CLng(Mid$(sLine, 3, Len(sLine) - 2))
                        If tempLong > 4096 Or tempLong < 0 Then
                            LogOutput "Item " & Items(currentItem).Name & " with serial " & CStr(Items(currentItem).Serial) & " has an y value " & CStr(tempLong), txtLoggingOutput, dMode
                            Items(currentItem).y = tempLong
                            addError = True
                            blah2.Location = blah2.Location + 2
                            blah2.BadLocations(1) = tempLong
                            itemNormErrorCount = itemNormErrorCount + 1
                        Else
                            Items(currentItem).y = tempLong
                        End If
                    End If
                Case "Z"
                    If left$(sLine, 1) = "Z" Then
                        tempLong = CLng(Mid$(sLine, 3, Len(sLine) - 2))
                        If tempLong > 128 Or tempLong < -128 Then
                            LogOutput "Item " & Items(currentItem).Name & " with serial " & CStr(Items(currentItem).Serial) & " has an z value " & CStr(tempLong), txtLoggingOutput, dMode
                            Items(currentItem).z = tempLong
                            addError = True
                            blah2.Location = blah2.Location + 4
                            blah2.BadLocations(2) = tempLong
                            itemNormErrorCount = itemNormErrorCount + 1
                        Else
                            Items(currentItem).z = tempLong
                        End If
                    End If
                End Select
            End If
        Loop
        If addError Then
            Items(currentItem).Fixed = True
            AddToItemError blah2
        End If
        DoEvents
    Next currentItem
    mnuFileOpen.Enabled = True
    sbStatusBar.Panels(1).Text = "Items Loaded!"
    lblStatsItemActualCount.Caption = CStr(UBound(Items) + 1)

End Sub

Public Sub LoadGuilds(sbStatus As StatusBar)

    sbStatus.Panels(1).Text = "Loading Guilds now"
    sbStatus.Panels(1).Text = "Guilds loaded"
End Sub

Public Sub SetContVals(sbStatus As StatusBar)

    Dim iCount As Long
    Dim itemIndex As Long
    Dim charIndex As Long
    Dim onePercent As Long
    Dim upperbound As Long
    Dim targetLayer As Byte
    onePercent = (UBound(Items) + 1) / 101
    If onePercent = 0 Then
        onePercent = 1
    End If
    For iCount = 0 To UBound(Items)
        If (iCount Mod onePercent) = 0 Then
            sbStatus.Panels(1).Text = "Setting up cont indexes... " & CStr((iCount / onePercent)) & "% done"
        End If
        Dim addError As Boolean
        Dim blah As ItemError
        ClearEntry blah
        addError = False
        If Items(iCount).Owner <> -1 Then
            If Items(iCount).Owner = Items(iCount).Serial Then
                addError = True
                blah.Owner = True
                itemCritErrorCount = itemCritErrorCount + 1
            Else
                Dim tOwner As Long
                If Items(iCount).Owner >= ItemMin Then
                    tOwner = FindItem(Items(iCount).Owner)
                    If tOwner = -1 Then
                        blah.Owner = True
                        addError = True
                        itemCritErrorCount = itemCritErrorCount + 1
                    End If
                Else
                    tOwner = FindChar(Items(iCount).Owner)
                    If tOwner = -1 Then
                        blah.Owner = True
                        addError = True
                        itemCritErrorCount = itemCritErrorCount + 1
                    End If
                End If
            End If
        End If
        If Items(iCount).Spawn <> -1 And Items(iCount).Spawn <> 0 Then
            If Items(iCount).Spawn = Items(iCount).Serial Then
                addError = True
                blah.Spawner = True
                itemCritErrorCount = itemCritErrorCount + 1
            Else
                Dim tSpawn As Long
                If Items(iCount).Spawn >= ItemMin Then
                    tSpawn = FindItem(Items(iCount).Spawn)
                    If tOwner = -1 Then
                        blah.Spawner = True
                        addError = True
                        itemCritErrorCount = itemCritErrorCount + 1
                    End If
                Else
                    If (Items(iCount).Spawn And &HFF0000) <> 1 Then
                        tSpawn = FindChar(Items(iCount).Spawn)
                        If tSpawn = -1 Then
                            blah.Spawner = True
                            addError = True
                            itemCritErrorCount = itemCritErrorCount + 1
                        End If
                    End If
                End If
            End If
        End If
        If Items(iCount).Container <> -1 Then
            If Items(iCount).Container >= ItemMin Then
                itemIndex = FindItem(Items(iCount).Container)
                If itemIndex <> -1 Then
                    upperbound = UBound(Items(itemIndex).Cont.Contents)
                    ReDim Preserve Items(itemIndex).Cont.Contents(0 To (upperbound + 1))
                    Items(itemIndex).Cont.Contents(upperbound) = iCount
                    Items(itemIndex).Cont.Contents(upperbound + 1) = -1
                Else
                    itemCritErrorCount = itemCritErrorCount + 1
                    blah.Container = True
                    blah.Index = iCount
                    addError = True
                End If
            Else
                charIndex = FindChar(Items(iCount).Container)
                If (charIndex <> -1) Then
                    upperbound = UBound(Chars(charIndex).Layers(Items(iCount).Layer).Contents)
                    ReDim Preserve Chars(charIndex).Layers(Items(iCount).Layer).Contents(0 To (upperbound + 1))
                    Chars(charIndex).Layers(Items(iCount).Layer).Contents(upperbound) = iCount
                    Chars(charIndex).Layers(Items(iCount).Layer).Contents(upperbound + 1) = -1
                Else
                    itemCritErrorCount = itemCritErrorCount + 1
                    blah.Container = True
                    blah.Index = iCount
                    addError = True
                End If
            End If
        End If
        If addError Then
            Dim iLocation As Long
            iLocation = ItemErrorLocation(iCount)
            If iLocation <> -1 Then
                ItemErrors(iLocation).Container = blah.Container
                ItemErrors(iLocation).Spawner = blah.Spawner
                ItemErrors(iLocation).Owner = blah.Owner
            Else
                blah.Index = iCount
                AddToItemError blah
            End If
        End If
    Next iCount

    sbStatus.Panels(1).Text = "Finished setting up cont indexes"
End Sub

Public Sub SaveChars(sbStatusBar As StatusBar, dlgCommonDialog As CommonDialog)
    Dim onePercent As Long
    onePercent = UBound(Chars) / 99
    If onePercent = 0 Then
        onePercent = 1
    End If

    sbStatusBar.Panels(1).Text = "Saving characters..."
    Dim inSection As Boolean, sLine As String
    Dim sFile As String
    With dlgCommonDialog
        .DialogTitle = "Saving chars, specify where and what to name chars.wsc"
        .Filter = "UOX World Files (*.wsc)|*.wsc"
        .ShowSave
        If Len(.FileName) = 0 Then
            Exit Sub
        End If
        sFile = .FileName
    End With
    Dim iChars As Long, iLine As Long, iSect As Long, cToWrite As Long, iOldChars As Long, tempPos As Long
    iChars = FreeFile
    Open sFile For Output As iChars
    iOldChars = FreeFile
    Open charsWscString For Input As iOldChars
    Do While Not EOF(iOldChars)
        Do While Not inSection
            Line Input #iOldChars, sLine
            Print #iChars, sLine
            If EOF(iOldChars) Then       ' if by some bizarre chance we hit EOF before finishing loading the file
                sLine = "{"
            End If
            iLine = InStr(1, sLine, "SECTION CHARACTER")
            If iLine <> 0 Then
                iSect = CLng(Mid$(sLine, iLine + 17, Len(sLine) - (iLine + 16)))
            End If
            tempPos = InStr(1, sLine, "{")
            If (tempPos <> 0 And Not IsNull(tempPos)) Then
                inSection = True
            End If
        Loop
        cToWrite = FindObject(iSect, 1)
        If cToWrite = -1 Then   ' cannot find section? This would be bad
            ' not the same world file that we started with!
        Else
            If Not Chars(cToWrite).Fixed Then
                Do While (Not EOF(iOldChars) And inSection)
                    Line Input #iOldChars, sLine
                    Print #iChars, sLine
                    If left$(sLine, 1) = "}" Then
                        inSection = False
                    End If
                Loop
            Else
                Do While (Not EOF(iOldChars) And inSection)
                    Line Input #iOldChars, sLine
                    If left$(sLine, 1) = "}" Then
                        inSection = False
                        Print #iChars, sLine
                    Else
                        Select Case left$(sLine, 1)
                        Case "A"
                            If left$(sLine, 7) = "ACCOUNT" Then
                                Print #iChars, "ACCOUNT " & Chars(cToWrite).Account
                            ElseIf left$(sLine, 7) = "ALLMOVE" Then
                                Print #iChars, "ALLMOVE " & Chars(cToWrite).priv2
                            Else
                                Print #iChars, sLine
                            End If
                        Case "B"
                            If left$(sLine, 4) = "BODY" Then
                                Print #iChars, "BODY " & Chars(cToWrite).Body
                            Else
                                Print #iChars, sLine
                            End If
                        Case "D"
                            If left$(sLine, 10) = "DEXTERITY " Then
                                Print #iChars, "DEXTERITY " & Chars(cToWrite).Dexterity
                            ElseIf left$(sLine, 10) = "DEXTERITY2" Then
                                Print #iChars, "DEXTERITY2 " & Chars(cToWrite).Dexterity2
                            ElseIf left$(sLine, 9) = "DIRECTION" Then
                                Print #iChars, "DIRECTION " & Chars(cToWrite).Direction
                            Else
                                Print #iChars, sLine
                            End If
                        Case "E"
                            If left$(sLine, 5) = "EMOTE" Then
                                Print #iChars, "EMOTE " & Chars(cToWrite).Emotecolour
                            Else
                                Print #iChars, sLine
                            End If
                        Case "F"
                            If left$(sLine, 4) = "FAME" Then
                                Print #iChars, "FAME " & Chars(cToWrite).Fame
                            Else
                                Print #iChars, sLine
                            End If
                        Case "G"
                            If left$(sLine, 11) = "GUILDNUMBER" Then
                                Print #iChars, "GUILDNUMBER " & Chars(cToWrite).GuildNumber
                            Else
                                Print #iChars, sLine
                            End If
                        Case "H"
                            If left$(sLine, 9) = "HITPOINTS" Then
                                Print #iChars, "HITPOINTS " & Chars(cToWrite).HitPoints
                            Else
                                Print #iChars, sLine
                            End If
                        Case "K"
                            If left$(sLine, 5) = "KILLS" Then
                                Print #iChars, "KILLS " & Chars(cToWrite).Kills
                            ElseIf left$(sLine, 5) = "KARMA" Then
                                Print #iChars, "KARMA " & Chars(cToWrite).Karma
                            Else
                                Print #iChars, sLine
                            End If
                        Case "I"
                            If left$(sLine, 13) = "INTELLIGENCE " Then
                                Print #iChars, "INTELLIGENCE " & Chars(cToWrite).Intelligence
                            ElseIf left$(sLine, 13) = "INTELLIGENCE2" Then
                                Print #iChars, "INTELLIGENCE2 " & Chars(cToWrite).Intelligence2
                            Else
                                Print #iChars, sLine
                            End If
                        Case "M"
                            If left$(sLine, 4) = "MANA" Then
                                Print #iChars, "MANA " & Chars(cToWrite).Mana
                            Else
                                Print #iChars, sLine
                            End If
                        Case "N"
                            If left$(sLine, 4) = "NAME" Then
                                Print #iChars, "NAME " & Chars(cToWrite).Name
                            ElseIf left$(sLine, 7) = "NOTRAIN" Then
                                Print #iChars, "NOTRAIN"
                            Else
                                Print #iChars, sLine
                            End If
                        Case "O"
                            If left$(sLine, 5) = "OWNER" Then
                                Print #iChars, "OWNER " & Chars(cToWrite).Owner
                            Else
                                Print #iChars, sLine
                            End If
                        Case "P"
                            If left$(sLine, 4) = "PRIV" Then
                                Print #iChars, "PRIV " & Chars(cToWrite).Priv1
                            Else
                                Print #iChars, sLine
                            End If
                        Case "S"
                            If left$(sLine, 6) = "SERIAL" Then
                                Print #iChars, "SERIAL " & Chars(cToWrite).Serial
                            ElseIf left$(sLine, 7) = "STAMINA" Then
                                Print #iChars, "STAMINA " & Chars(cToWrite).Stamina
                            ElseIf left$(sLine, 9) = "STRENGTH " Then
                                Print #iChars, "STRENGTH " & Chars(cToWrite).Strength
                            ElseIf left$(sLine, 9) = "STRENGTH2" Then
                                Print #iChars, "STRENGTH2 " & Chars(cToWrite).Strength2
                            ElseIf left$(sLine, 3) = "SAY" Then
                                Print #iChars, "SAY " & Chars(cToWrite).Speechcolour
                            ElseIf left$(sLine, 4) = "SKIN" Then
                                Print #iChars, "SKIN " & Chars(cToWrite).Skin
                            ElseIf left$(sLine, 5) = "SKILL" Then
                                Dim sNum As Long
                                sNum = CLng(Mid$(sLine, 6, 2))
                                Print #iChars, "SKILL" & sNum & " " & Chars(cToWrite).Skills(sNum)
                            Else
                                Print #iChars, sLine
                            End If
                        Case "T"
                            If left$(sLine, 5) = "TITLE" Then
                                Print #iChars, "TITLE " & Chars(cToWrite).Title
                            ElseIf left$(sLine, 8) = "TRIGWORD" Then
                                Print #iChars, "TRIGWORD " & Chars(cToWrite).Trigword
                            Else
                                Print #iChars, sLine
                            End If
                        Case "X"
                            If left$(sLine, 2) = "X " Then
                                Print #iChars, "X " & Chars(cToWrite).x
                            Else
                                Print #iChars, sLine
                            End If
                        Case "Y"
                            If left$(sLine, 1) = "Y" Then
                                Print #iChars, "Y " & Chars(cToWrite).y
                            Else
                                Print #iChars, sLine
                            End If
                        Case "Z"
                            If left$(sLine, 1) = "Z" Then
                                Print #iChars, "Z " & Chars(cToWrite).z
                            Else
                                Print #iChars, sLine
                            End If
                        Case Else
                                Print #iChars, sLine
                        End Select
                    End If
                Loop
            End If
        End If
        DoEvents
    Loop
    Close iChars
    Close iOldChars
    sbStatusBar.Panels(1).Text = "Saving characters done!"
End Sub

Public Sub SaveItems(sbStatusBar As StatusBar, dlgCommonDialog As CommonDialog)
    Dim onePercent As Long
    onePercent = UBound(Items) / 99
    If onePercent = 0 Then
        onePercent = 1
    End If

    Dim inSection As Boolean, sLine As String
    Dim sFile As String, partialCandidate As Boolean
    sbStatusBar.Panels(1).Text = "Saving items..."
    With dlgCommonDialog
        .DialogTitle = "Saving items, specify where and what to name items.wsc"
        .Filter = "UOX World Files (*.wsc)|*.wsc"
        .ShowSave
        If Len(.FileName) = 0 Then
            Exit Sub
        End If
        sFile = .FileName
    End With
    Dim iItems As Long, iLine As Long, iSect As Long, cToWrite As Long, iOldItems As Long, tempPos As Long
    iItems = FreeFile
    Open sFile For Output As iItems
    iOldItems = FreeFile
    Open itemsWscString For Input As iOldItems
    Do While Not EOF(iOldItems)
        partialCandidate = False
        Do While Not inSection
            Line Input #iOldItems, sLine
            Print #iItems, sLine
            tempPos = InStr(1, sLine, "WORLDITEM")  ' if the section name doesn't have WORLDITEM in it, it's not an item!
            If (tempPos <> 0 And Not IsNull(tempPos)) Then
                partialCandidate = True
                iSect = CLng(Mid$(sLine, tempPos + 9, Len(sLine) - (tempPos + 8)))
            End If
            If EOF(iOldItems) Then       ' if by some bizarre chance we hit EOF before finishing loading the file
                sLine = "{"
                partialCandidate = True
            End If
            If partialCandidate Then    ' if we're not even in a valid world section, don't bother!
                tempPos = InStr(1, sLine, "{")
                If (tempPos <> 0 And Not IsNull(tempPos)) Then
                    inSection = True
                End If
            End If
        Loop
        cToWrite = FindObject(iSect, 0)
        If cToWrite = -1 Then   ' cannot find section? This would be bad
            Debug.Print "Invalid section, we're in trouble!"
            ' not the same world file that we started with!
        Else
            If Not Items(cToWrite).Fixed Then
                Do While (Not EOF(iOldItems) And inSection)
                    Line Input #iOldItems, sLine
                    Print #iItems, sLine
                    If left$(sLine, 1) = "}" Then
                        inSection = False
                    End If
                Loop
            Else
                Do While (Not EOF(iOldItems) And inSection)
                    Line Input #iOldItems, sLine
                    If left$(sLine, 1) = "}" Then
                        Print #iItems, sLine
                        inSection = False
                    Else
                        Select Case left$(sLine, 1)
                        Case "C"
                            If left$(sLine, 4) = "CONT" Then
                                Print #iItems, "CONT " & Items(cToWrite).Container
                            ElseIf left$(sLine, 5) = "COLOR" Then
                                Print #iItems, "COLOR " & Items(cToWrite).Colour
                            Else
                                Print #iItems, sLine
                            End If
                        Case "D"
                            If left$(sLine, 3) = "DX " Then
                                Print #iItems, "DX " & Items(cToWrite).Dexterity
                            Else
                                Print #iItems, sLine
                            End If
                        Case "H"
                            If left$(sLine, 2) = "HP" Then
                                Print #iItems, "HP " & Items(cToWrite).HitPoints
                            Else
                                Print #iItems, sLine
                            End If
                        Case "I"
                            If left$(sLine, 2) = "ID" Then
                                Print #iItems, "ID " & Items(cToWrite).ID
                            ElseIf left$(sLine, 3) = "IN " Then
                                Print #iItems, "IN " & Items(cToWrite).Intelligence
                            Else
                                Print #iItems, sLine
                            End If
                        Case "L"
                            If left$(sLine, 5) = "LAYER" Then
                                Print #iItems, "LAYER " & Items(cToWrite).Layer
                            Else
                                Print #iItems, sLine
                            End If
                        Case "M"
                            If left$(sLine, 4) = "MANA" Then
                                Print #iItems, "MANA " & Items(cToWrite).Mana
                            ElseIf left$(sLine, 5) = "MAXHP" Then
                                Print #iItems, "MAXHP " & Items(cToWrite).MaxHealth
                            Else
                                Print #iItems, sLine
                            End If
                        Case "N"
                            If left$(sLine, 5) = "NAME " Then
                                Print #iItems, "NAME " & Items(cToWrite).Name
                            Else
                                Print #iItems, sLine
                            End If
                        Case "O"
                            If left$(sLine, 5) = "OWNER" Then
                                Print #iItems, "OWNER " & Items(cToWrite).Owner
                            Else
                                Print #iItems, sLine
                            End If
                        Case "S"
                            If left$(sLine, 6) = "SERIAL" Then
                                Print #iItems, "SERIAL " & Items(cToWrite).Serial
                            ElseIf left$(sLine, 7) = "STAMINA" Then
                                Print #iItems, "STAMINA " & Items(cToWrite).Stamina
                            ElseIf left$(sLine, 3) = "ST " Then
                                Print #iItems, "ST " & Items(cToWrite).Strength
                            ElseIf left$(sLine, 5) = "SPAWN" Then
                                Print #iItems, "SPAWN " & Items(cToWrite).Spawn
                            Else
                                Print #iItems, sLine
                            End If
                        Case "T"
                            If left$(sLine, 5) = "TYPE " Then
                                Print #iItems, "TYPE " & Items(cToWrite).iType
                            Else
                                Print #iItems, sLine
                            End If
                        Case "X"
                            If left$(sLine, 2) = "X " Then
                                Print #iItems, "X " & Items(cToWrite).x
                            Else
                                Print #iItems, sLine
                            End If
                        Case "Y"
                            If left$(sLine, 1) = "Y" Then
                                Print #iItems, "Y " & Items(cToWrite).y
                            Else
                                Print #iItems, sLine
                            End If
                        Case "Z"
                            If left$(sLine, 1) = "Z" Then
                                Print #iItems, "Z " & Items(cToWrite).z
                            Else
                                Print #iItems, sLine
                            End If
                        Case Else
                            Print #iItems, sLine
                        End Select
                    End If
                Loop
            End If
        End If
        DoEvents
    Loop
    Close iItems
    Close iOldItems
    sbStatusBar.Panels(1).Text = "Items saved, world save complete"

End Sub

Public Sub ClearCharStructure(ByRef newClean As Char_st)
    newClean.Account = False
    newClean.Body = 0
    newClean.CanTrain = False
    newClean.Dexterity = 0
    newClean.Dexterity2 = 0
    newClean.Direction = 0
    newClean.Emotecolour = 0
    newClean.Fame = 0
    newClean.FilePos = 0
    newClean.Fixed = False
    newClean.GuildNumber = 0
    newClean.HitPoints = 0
    newClean.Intelligence = 0
    newClean.Intelligence2 = 0
    newClean.iSect = -1
    newClean.Karma = 0
    newClean.Kills = 0
    newClean.Mana = 0
    newClean.Multi = -1
    newClean.Name = ""
    newClean.Owner = -1
    newClean.Priv1 = 0
    newClean.priv2 = 0
    newClean.Serial = 0
    newClean.Skin = 0
    newClean.Speechcolour = 0
    newClean.Stamina = 0
    newClean.Strength = 0
    newClean.Strength2 = 0
    newClean.Title = 0
    newClean.Trigword = ""
    newClean.x = 0
    newClean.y = 0
    newClean.z = 0
End Sub

Public Sub ClearItemStructure(ByRef newClean As Item_st)
    newClean.Colour = 0
    newClean.Container = -1
    newClean.Dexterity = 0
    newClean.FilePos = 0
    newClean.Fixed = False
    newClean.HitPoints = 0
    newClean.ID = 0
    newClean.Intelligence = 0
    newClean.iSect = -1
    newClean.iType = 0
    newClean.Layer = 0
    newClean.Mana = 0
    newClean.MaxHealth = 0
    newClean.Multi = -1
    newClean.Name = ""
    newClean.Owner = -1
    newClean.Serial = 0
    newClean.Spawn = 0
    newClean.Stamina = 0
    newClean.Strength = 0
    newClean.x = 0
    newClean.y = 0
    newClean.z = 0
End Sub


