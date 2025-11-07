//usage: TriggerEvent(18001, "modifyContextMenu", socket, targObj, list, useKR);

function modifyContextMenu(socket, targObj, entries, useKR)
{
	var count = entries.length | 0;
	var packetLen = 12 + (count * 8);

	var p = new Packet();
	p.ReserveSize(packetLen);

	p.WriteByte(0, 0xBF);
	p.WriteShort(1, packetLen);
	p.WriteShort(3, 0x14);                           // context menu subcmd
	p.WriteShort(5, useKR ? 0x0002 : 0x0001);        // KR/EC vs 2D
	p.WriteLong(7, targObj.serial);
	p.WriteByte(11, count);

	var off = 12;

	if (useKR)
	{
		for (var i = 0; i < count; i++)
		{
			var e = entries[i];
			var textID = (e.text <= 65535) ? (3000000 + (e.text | 0)) : (e.text | 0);
			p.WriteLong(off, textID);
			p.WriteShort(off + 4, e.id | 0);
			p.WriteShort(off + 6, e.flags | 0);
			off += 8;
		}
	}
	else
	{
		for (var j = 0; j < count; j++)
		{
			var e2 = entries[j];
			var num16 = (e2.text | 0);
			if (num16 > 65535) num16 = 65535;

			p.WriteShort(off, num16);
			p.WriteShort(off + 2, e2.id | 0);
			p.WriteShort(off + 4, e2.flags | 0);
			p.WriteShort(off + 6, e2.hue | 0);
			off += 8;
		}
	}

	socket.Send(p);
	p.Free();
}