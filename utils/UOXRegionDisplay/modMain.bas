Attribute VB_Name = "modMain"
Public Declare Function SetPixelV Lib "GDI32" (ByVal hdc As Long, ByVal x As Long, ByVal y As Long, ByVal crColor As Long) As Long
Public Declare Function Rectangle Lib "GDI32" (ByVal hDestDC As Long, ByVal X1 As Long, ByVal Y1 As Long, ByVal X2 As Long, ByVal Y2 As Long) As Long

Public Enum WorldType
    WRLD_SPRING = 0
    WRLD_SUMMER
    WRLD_AUTUMN
    WRLD_WINTER
    WRLD_DESOLATION
    WRLD_UNKNOWN
    WLRD_COUNT
End Enum

Type Region_st  ' Create user-defined type.
    Guarded As Boolean
    Name As String
    Guardowner As String
    Weather As Integer
    MagicDamage As Boolean
    Mark As Boolean
    Gate As Boolean
    Recall As Boolean
    Appearance As WorldType
End Type

Type Location_st
    X1 As Integer
    Y1 As Integer
    X2 As Integer
    Y2 As Integer
    Region As Integer
End Type

Public WorldTypeNames(WRLD_SPRING To WRLD_UNKNOWN) As String
Public regionNum(0 To 4000) As Location_st
Public regions(0 To 255) As Region_st

Option Explicit

Public Sub DoStartup()

    WorldTypeNames(0) = "Spring"
    WorldTypeNames(1) = "Summer"
    WorldTypeNames(2) = "Autumn"
    WorldTypeNames(3) = "Winter"
    WorldTypeNames(4) = "Desolation"
    WorldTypeNames(5) = "Unknown"
    
    Dim i As Integer
    For i = 0 To 4000
        regionNum(i).Region = 255
        regionNum(i).X1 = 0
        regionNum(i).X2 = 0
        regionNum(i).Y1 = 0
        regionNum(i).Y2 = 0
    Next i
    For i = 0 To 255
        regions(i).Appearance = WRLD_UNKNOWN
        regions(i).Weather = 255
    Next i
    
End Sub

Public Function Max(one As Single, two As Single) As Single
    If one > two Then
        Max = one
    Else
        Max = two
    End If
End Function
