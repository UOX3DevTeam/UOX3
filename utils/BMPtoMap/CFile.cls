VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "CFile"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Private Declare Function GetFileSize Lib "Kernel32" (ByVal hFile As Long, ByVal lpFileSizeHigh As Long) As Long
Private Declare Function lclose Lib "Kernel32" Alias "_lclose" (ByVal hFile As Long) As Long
Private Declare Function llseek Lib "Kernel32" Alias "_llseek" (ByVal hFile As Long, ByVal lOffset As Long, ByVal iOrigin As Long) As Long
Private Declare Function lopen Lib "Kernel32" Alias "_lopen" (ByVal lpPathName As String, ByVal iReadWrite As Long) As Long
Private Declare Function lread Lib "Kernel32" Alias "_lread" (ByVal hFile As Long, lpBuffer As Any, ByVal wBytes As Long) As Long
Private Declare Function lwrite Lib "Kernel32" Alias "_lwrite" (ByVal hFile As Long, ByVal lpBuffer As Long, ByVal wBytes As Long) As Long

Private m_hFile As Long, m_Pointer As Long

Public Property Get Pointer() As Long
   Pointer = llseek(m_hFile, 0, 1)
End Property

Public Sub WriteInt(ByVal ToWrite As Integer)
   Call lwrite(m_hFile, VarPtr(ToWrite), 2)
End Sub

Public Sub WriteLong(ByVal ToWrite As Long)
   Call lwrite(m_hFile, VarPtr(ToWrite), 4)
End Sub

Public Sub WriteString(ByRef ToWrite As String, ByVal Length As Long)
   Dim s As String, B() As Byte, i As Integer
   ReDim B(1 To Length)
   
   s = ToWrite
   
   For i = 1 To Length
      B(i) = Asc(Mid$(s, i, 1))
   Next
   
   Call WriteChunk(VarPtr(B(1)), Length)
End Sub

Public Sub WriteChunk(ByVal SrcPtr As Long, ByVal Bytes As Long)
   Call lwrite(m_hFile, SrcPtr, Bytes)
End Sub

Public Property Get Length() As Long
   Length = GetFileSize(m_hFile, 0&)
End Property

Public Sub MoveTo(ByVal NewPosition As Long)
   Call llseek(m_hFile, NewPosition, 0)
End Sub

Public Sub Shift(ByVal ByteOffset As Long)
   Call llseek(m_hFile, ByteOffset, 1)
End Sub

Public Function ReadSignedByte() As Integer
   ReadSignedByte = (((ReadByte And &HFF) Xor &H80) - 128)
End Function

Public Function ReadByte() As Byte
   Call lread(m_hFile, ReadByte, 1)
End Function

Public Function ReadInt() As Integer
   Call lread(m_hFile, ReadInt, 2)
End Function

Public Function ReadLong() As Long
   Call lread(m_hFile, ReadLong, 4)
End Function

Public Function ReadString(ByVal Length As Long) As String
   Dim B() As Byte, s As String
   ReDim B(1 To Length)
   
   Call ReadChunk(VarPtr(B(1)), Length)
   s = B
   s = StrConv(s, vbUnicode)
   Length = InStr(1, s, vbNullChar) - 1
   
   If Length > 0 Then
      s = Left$(s, Length)
   End If
   
   ReadString = s
End Function

Public Sub ReadChunk(ByVal DestPtr As Long, ByVal Bytes As Long)
   Call lread(m_hFile, ByVal DestPtr, Bytes)
End Sub

Public Function OpenFile(ByRef Path As String) As Boolean
   m_hFile = lopen(Path, &H2)
   OpenFile = (m_hFile <> -1)
End Function

Public Sub CloseFile()
   If m_hFile <> -1 Then
      Call lclose(m_hFile)
   End If
End Sub

