Attribute VB_Name = "Module1"
Option Explicit
Public fMainForm As frmMain
Public appControl As ControlInformation
Public Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)
Public Declare Function GetCurrentTime Lib "kernel32" Alias "GetTickCount" () As Long

Public Const PLAYER_AUTH As Byte = 0
Public Const COUNSELOR_AUTH As Byte = 1
Public Const GM_AUTH As Byte = 2
Public Const ADMIN_AUTH As Byte = 3

Public Const XGM_CLIENT_PROTOCOL_MAJOR As Byte = 0
Public Const XGM_CLIENT_PROTOCOL_MINOR As Byte = 0
Public Const XGM_CLIENT_PROTOCOL_REVISION As Byte = 1
Public Const XGM_CLIENT_PROTOCOL_EXTRA As Byte = 0

Public Const XGM_WHO_ONLINE As Byte = 0
Public Const XGM_WHO_OFFLINE As Byte = 1
Public Const XGM_WHO_LOGGING As Byte = 2

Type ControlInformation
    remoteHostName As String
    remoteHostIP As String
    remoteHostPort As Long
    clearanceLevel As Byte
End Type

Sub Main()
    frmSplash.Show
    frmSplash.Refresh
    LoadConfigData
    Load frmMain
    Sleep 3000
    Unload frmSplash
    frmMain.Show
End Sub

Sub LoadResStrings(frm As Form)
    On Error Resume Next

    Dim ctl As Control
    Dim obj As Object
    Dim fnt As Object
    Dim sCtlType As String
    Dim nVal As Integer

    'set the form's caption
    frm.Caption = LoadResString(CInt(frm.Tag))
    
    'set the font
    Set fnt = frm.Font
    fnt.Name = LoadResString(20)
    fnt.Size = CInt(LoadResString(21))
    
    'set the controls' captions using the caption
    'property for menu items and the Tag property
    'for all other controls
    Dim i As Long
    For Each ctl In frm.Controls
        sCtlType = TypeName(ctl)
        If sCtlType = "Label" Then
            ctl.Caption = LoadResString(CInt(ctl.Tag))
        ElseIf sCtlType = "Menu" Then
            ctl.Caption = LoadResString(CInt(ctl.Caption))
        ElseIf sCtlType = "TabStrip" Then
            For Each obj In ctl.Tabs
                obj.Caption = LoadResString(CInt(obj.Tag))
                obj.ToolTipText = LoadResString(CInt(obj.ToolTipText))
            Next
        ElseIf sCtlType = "Toolbar" Then
            For Each obj In ctl.Buttons
                obj.ToolTipText = LoadResString(CInt(obj.ToolTipText))
            Next
        ElseIf sCtlType = "ListView" Then
            For Each obj In ctl.ColumnHeaders
                obj.Text = LoadResString(CInt(obj.Tag))
            Next
        ElseIf sCtlType = "StatusBar" Then
            For i = 0 To ctl.Panels.Count
                ctl.Panels(i).Text = LoadResString(CInt(ctl.Panels(i).Tag))
            Next i
        Else
            nVal = 0
            nVal = Val(ctl.Tag)
            If nVal > 0 Then
                ctl.Caption = LoadResString(nVal)
            End If
            nVal = 0
            nVal = Val(ctl.ToolTipText)
            If nVal > 0 Then
                ctl.ToolTipText = LoadResString(nVal)
            End If
        End If
    Next

End Sub

Public Sub LoadConfigData()

    appControl.remoteHostIP = GetSetting(App.Title, "ConfigSettings", "RemoteHostIP", "")
    appControl.remoteHostName = GetSetting(App.Title, "ConfigSettings", "RemoteHostName", "")
    appControl.remoteHostPort = GetSetting(App.Title, "ConfigSettings", "RemotePort", 6666)
    appControl.clearanceLevel = GetSetting(App.Title, "ConfigSettings", "ClearanceLevel", PLAYER_AUTH)
    
End Sub

Public Function min(i As Long, j As Long)

    If i < j Then
        min = i
    Else
        min = j
    End If
    
End Function
Public Sub SendLoginPacket(wsXGM As Winsock)

'Size 62 bytes
'Minimum access level Player
'Possible responses 0x00
'
'UI08 commandByte;       // 0x00
'UI08 username[30];
'UI08 password[30];
'UI08 clearanceRequested;    // takes values of 0->3

    Dim dataToSend(0 To 61) As Byte, i As Long, sUsername As String, sPassword As String
    sUsername = GetSetting(App.Title, "ConfigSettings", "Username", "")
    sPassword = GetSetting(App.Title, "ConfigSettings", "Password", "")
    dataToSend(0) = 0
    For i = 1 To min(Len(sUsername), 29)
        dataToSend(i) = Asc(Mid$(sUsername, i, 1))
    Next i
    For i = 1 To min(Len(sPassword), 29)
        dataToSend(30 + i) = Asc(Mid$(sPassword, i, 1))
    Next i
    dataToSend(61) = appControl.clearanceLevel
    wsXGM.SendData dataToSend
End Sub

Public Sub SendStatPacket(wsXGM As Winsock, statID As Byte)

'Size 2 bytes
'Minimum access level Player
'Possible responses 0x12
'
'UI08 commandByte;       // 0x09
'UI08 statID;            // ID of the stat you want
'//      0       # players online
'//      1       # characters total
'//      2       # items total
'//      3       ping response
'//      4       # accounts
'//      5       Simulation cycles
'//      6       # of races
'//      7       # of regions
'//      8       # of weather systems
'//      9       time since last restart
'//      10      time since last world save
'//      11      last player logged in
'//      12      # of GM pages
'//      13      # of CNS pages
'//      14      # of ghosts
'//      15      # of murderers
'//      16      # of blues
'//      17      # of houses
'//      18      total gold in circulation
'//      19      peak connection count
'//      20      # of GMs logged in
'//      21      # of counselors logged in

    Dim dataToSend(0 To 1) As Byte
    dataToSend(0) = 15
    dataToSend(1) = statID
    wsXGM.SendData dataToSend
End Sub

Public Sub SendClientVersionPacket(wsXGM As Winsock)

'XGM Client Protocol Version
'Size 5 bytes
'Minimum access level Player
'Possible responses None

'UI08 commandByte;       // 0x08
'UI32 xgmClientVersion;

    Dim dataToSend(0 To 4) As Byte
    dataToSend(0) = 8
    dataToSend(1) = XGM_CLIENT_PROTOCOL_MAJOR
    dataToSend(2) = XGM_CLIENT_PROTOCOL_MINOR
    dataToSend(3) = XGM_CLIENT_PROTOCOL_REVISION
    dataToSend(4) = XGM_CLIENT_PROTOCOL_EXTRA
    wsXGM.SendData dataToSend
End Sub

Public Sub SendServerVersionRequest(wsXGM As Winsock)

'XGM Server Protocol Version Request
'Size 1 bytes
'Minimum access level Player
'Possible responses 0x0A

'UI08 commandByte;       // 0x09

    Dim dataToSend(0 To 0) As Byte
    dataToSend(0) = 9
    wsXGM.SendData dataToSend
    
End Sub

Public Sub SendWhoRequest(wsXGM As Winsock, status As Byte)

'Who 's online request
'Size 1 bytes
'Minimum access level Player
'Possible responses 0x02

'UI08 commandByte;       // 0x02

'Who 's offline request
'Size 1 bytes
'Minimum access level Counselor
'Possible responses 0x02

'UI08 commandByte;       // 0x03

'Who 's logging out request
'Size 1 bytes
'Minimum access level Counselor
'Possible responses 0x02

'UI08 commandByte;       // 0x04

    Dim dataToSend(0 To 0) As Byte
    dataToSend(0) = 2 + status
    wsXGM.SendData dataToSend
    
End Sub
