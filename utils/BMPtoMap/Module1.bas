Attribute VB_Name = "Module1"
Option Explicit
Public Declare Function SetPixelV Lib "gdi32" (ByVal hdc As Long, ByVal x As Long, ByVal y As Long, ByVal Color As Long) As Long
Public Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)
Public Declare Function GetTickCount Lib "kernel32" () As Long
Public Declare Function RtlMoveMemory Lib "kernel32" (ByVal pvDest As Long, ByVal pvSrc As Long, ByVal Length As Long) As Long
Public Declare Function GetPixel Lib "gdi32" (ByVal hdc As Long, ByVal x As Long, ByVal y As Long) As Long
Type MapDataBlock
    ID1 As Byte
    ID2 As Byte
'    ID As Integer
    Altitude As Byte
End Type
Public radarcols(0 To 65535) As Long



Public Function Color32(ByVal Color16 As Integer) As Long
Color32 = _
(((Color16 And &H1F) * &HFF \ &H1F) * &H10000) Or _
(CLng((((Color16 \ &H20) And &H1F) * &HFF \ &H1F)) * &H100) Or _
((((Color16 \ &H400) And &H1F) * &HFF \ &H1F))
End Function
