Attribute VB_Name = "modColours"

Public Sub Generate32BitColour(ByRef memory() As Byte, x As Long, y As Long, colour As Long)

    Dim b As Long, r As Long, g As Long
    b = colour Mod 256
    colour = colour - b
    g = (colour Mod 65536) / 256
    colour = colour - (colour Mod 65536)
    r = colour / 65536
    ' * 4 for 4 byte wide pixels
    memory(x * 4, y) = r
    memory(x * 4 + 1, y) = g
    memory(x * 4 + 2, y) = b

End Sub

Public Sub Generate16BitColour(ByRef memory() As Byte, x As Long, y As Long, colour As Long)

    Dim targetColour As Long, half1 As Long, half2 As Long
    targetColour = colour / 256
    half1 = targetColour Mod 256
    targetColour = targetColour - half1
    half2 = targetColour / 256
    ' * 2 for 2 byte wide pixels
    memory(x * 2, y) = half2
    memory(x * 2 + 1, y) = half1
End Sub

Public Sub Generate24BitColour(ByRef memory() As Byte, x As Long, y As Long, colour As Long)

    Dim b As Long, r As Long, g As Long
    b = colour Mod 256
    colour = colour - b
    g = (colour Mod 65536) / 256
    colour = colour - (colour Mod 65536)
    r = colour / 65536
    ' * 3 for 3 byte wide pixels
    memory(x * 3, y) = r
    memory(x * 3 + 1, y) = g
    memory(x * 3 + 2, y) = b

End Sub

Public Sub Generate8BitColour(ByRef memory() As Byte, x As Long, y As Long, colour As Long)

    Dim b As Long, r As Long, g As Long
    b = colour Mod 256
    colour = colour - b
    g = (colour Mod 65536) / 256
    colour = colour - (colour Mod 65536)
    r = colour / 65536
    Dim targetColour As Long
    'targetColour = colour Mod 256
    targetColour = (r + g + b) / 3
    memory(x, y) = targetColour

End Sub


