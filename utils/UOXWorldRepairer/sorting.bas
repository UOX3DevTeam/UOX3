Attribute VB_Name = "sorting"
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
''''''''''''''''''''''''''''''''''''Heapsort for Chars'''''''''''''''''''''''''''''''''''''''
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Public Sub heapsortchars(ByRef Data() As Char_st, sbStatus As StatusBar)
    Dim unsorted As Long
    Dim n As Long
    Dim temp As Char_st
    
    n = UBound(Data, 1)
    
    makeheapchars Data, n, sbStatus
    
    unsorted = n
    sbStatus.Panels(1).Text = "Sorting chars"
    sbStatus.Refresh
    While (unsorted > 0)
        If ((n - unsorted) Mod 100 = 0) Then
            sbStatus.Panels(1).Text = "Sorting char: " & n - unsorted
        End If
        unsorted = unsorted - 1
        temp = Data(0)
        Data(0) = Data(unsorted + 1)
        Data(unsorted + 1) = temp
        
        reheapifydownchars Data, unsorted
    Wend
End Sub

Public Sub makeheapchars(ByRef Data() As Char_st, n As Long, sbStatus As StatusBar)
    Dim i As Long
    Dim k As Long
    Dim temp As Char_st
    Dim onePercent As Long
    onePercent = n \ 100
    If onePercent = 0 Then
        onePercent = 1
    End If
    For i = 0 To n
        k = i
        If i Mod onePercent = 0 Then
            sbStatus.Panels(1).Text = "Making heap char... " & i \ onePercent & "% done"
            sbStatus.Refresh
        End If
        DoEvents
        While (k > 0 And Data(k).Serial > Data(Abs(floor((k - 1) / 2))).Serial)
            temp = Data(k)
            Data(k) = Data(Abs(floor((k - 1) / 2)))
            Data(Abs(floor((k - 1) / 2))) = temp
            k = Abs(floor((k - 1) / 2))
        Wend
    Next i
    
End Sub

Public Sub reheapifydownchars(ByRef Data() As Char_st, n As Long)
    Dim current As Long
    Dim big_child_index As Long
    Dim left As Long
    Dim right As Long
    Dim heap_ok As Boolean
    Dim temp As Char_st
    
    current = 0
    heap_ok = False
    
    While ((Not heap_ok) And ((current * 2) + 1 <= n))
        left = (current * 2) + 1
        right = (current * 2) + 2
        If right > n Then
            big_child_index = left
        Else
            If Data(left).Serial < Data(right).Serial Then
                big_child_index = right
            Else
                big_child_index = left
            End If
        End If
        
        If (Data(current).Serial < Data(big_child_index).Serial) Then
            temp = Data(current)
            Data(current) = Data(big_child_index)
            Data(big_child_index) = temp
            current = big_child_index
        Else
            heap_ok = True
        End If
    Wend
    
End Sub

'Unused...but if you're going to put in functionality for adding items/chars...then you'll need to make this sub
Sub reheapifyup(ByRef Data() As Object, n As Long)

End Sub


Public Sub heapsortitems(ByRef Data() As Item_st, sbStatus As StatusBar)
    Dim unsorted As Long
    Dim n As Long
    Dim temp As Item_st
    
    n = UBound(Data, 1)
    
    makeheapitems Data, n, sbStatus
    
    unsorted = n
    
    frmMain.sbStatusBar.Panels(1).Text = "Sorting!"
    While (unsorted > 0)
        If ((n - unsorted) Mod 100 = 0) Then
            sbStatus.Panels(1).Text = "Sorting item: " & n - unsorted
        End If
        unsorted = unsorted - 1
        temp = Data(0)
        Data(0) = Data(unsorted + 1)
        Data(unsorted + 1) = temp
        
        reheapifydownitems Data, unsorted
    Wend
    
End Sub

Public Sub makeheapitems(ByRef Data() As Item_st, n As Long, sbStatus As StatusBar)
    Dim i As Long
    Dim k As Long
    Dim temp As Item_st
    Dim onePercent As Long
    onePercent = n \ 100
    If onePercent = 0 Then
        onePercent = 1
    End If
    For i = 0 To n
        If i Mod onePercent = 0 Then
            sbStatus.Panels(1).Text = "Making heap item... " & i \ onePercent & "% done"
            sbStatus.Refresh
        End If
        DoEvents
        k = i
        While (k > 0 And Data(k).Serial > Data(Abs(floor((k - 1) / 2))).Serial)
            temp = Data(k)
            Data(k) = Data(Abs(floor((k - 1) / 2)))
            Data(Abs(floor((k - 1) / 2))) = temp
            k = Abs(floor((k - 1) / 2))
        Wend
    Next i
    
End Sub

Public Sub reheapifydownitems(ByRef Data() As Item_st, n As Long)
    Dim current As Long
    Dim big_child_index As Long
    Dim left As Long
    Dim right As Long
    Dim heap_ok As Boolean
    Dim temp As Item_st
    
    current = 0
    heap_ok = False
    
    While ((Not heap_ok) And ((current * 2) + 1 <= n))
        left = (current * 2) + 1
        right = (current * 2) + 2
        If right > n Then
            big_child_index = left
        Else
            If Data(left).Serial < Data(right).Serial Then
                big_child_index = right
            Else
                big_child_index = left
            End If
        End If
        
        If (Data(current).Serial < Data(big_child_index).Serial) Then
            temp = Data(current)
            Data(current) = Data(big_child_index)
            Data(big_child_index) = temp
            current = big_child_index
        Else
            heap_ok = True
        End If
    Wend
    
End Sub

Public Function Round(num As Variant)

    If Fix(num + 0.5) = Fix(num) Then   ' it's less than x.5
        Round = Fix(num)
    Else
        Round = Fix(num + 0.5)
    End If
    
End Function

'Yep, you guessed it...Visual Basic doesn't have a floor function
Public Function floor(num As Variant)
    If num >= 0 Then
        If num < Round(num) And num > Round(num) - 1 Then
            floor = Round(num) - 1
        Else
            floor = Round(num)
            End If
    Else
        If num > Round(num) And num < Round(num) + 1 Then
            floor = Round(num) + 1
        Else
            floor = Round(num)
        End If
    End If
End Function

Public Function FindChar(serialToFind As Long) As Long

    Dim found As Boolean, first As Long, last As Long, middle As Long
    found = False
    first = 0
    last = UBound(Chars)
    Do While Not found And first <= last
        middle = (first + last) / 2
        If Chars(middle).Serial > serialToFind Then
            last = middle - 1
        ElseIf Chars(middle).Serial < serialToFind Then
            first = middle + 1
        Else
            found = True
        End If
    Loop
    If Not found Then
        FindChar = -1
    Else
        FindChar = middle
    End If
    
End Function

Public Function FindItem(serialToFind As Long) As Long

    Dim found As Boolean, first As Long, last As Long, middle As Long
    found = False
    first = 0
    last = UBound(Items)
    Do While Not found And first <= last
        middle = (first + last) / 2
        If Items(middle).Serial > serialToFind Then
            last = middle - 1
        ElseIf Items(middle).Serial < serialToFind Then
            first = middle + 1
        Else
            found = True
        End If
    Loop
    If Not found Then
        FindItem = -1
    Else
        FindItem = middle
    End If
    
End Function


Public Function FindObject(iSect As Long, iType As Long) As Long

    If iType <> 0 Then  ' character
        If iSect < 0 Or iSect > UBound(CharsSect) Then
            FindObject = -1
            Exit Function
        End If
        FindObject = CharsSect(iSect)
        Exit Function
    Else
        If iSect < 0 Or iSect > UBound(ItemsSect) Then
            FindObject = -1
            Exit Function
        End If
        FindObject = ItemsSect(iSect)
        Exit Function
    End If
    FindObject = -1
End Function
