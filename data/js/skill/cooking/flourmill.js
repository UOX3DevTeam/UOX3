function onUseChecked ( pUser, iUsed )
{
	var srcSock = pUser.socket;

	if (TriggerEvent(103, "RangeCheck", srcSock, pUser))
	{
		srcSock.SysMessage(GetDictionaryEntry(393, srcSock.language)); // That is too far away.
		return;
	}

	CreateDFNItem(srcSock, pUser, "0x1045", 1, "ITEM", false); // makes a sack of flour
	srcSock.SysMessage(GetDictionaryEntry(6083, srcSock.language)); // You grind some wheat and put a sack of flour in your pack!
	AreaItemFunction("myAreaFunc", pUser, 4, iUsed);
	if (iUsed.id == 0x1927)
	{
		iUsed.id = 0x1922;
	}
}
function myAreaFunc(srcChar, trgItem, iUsed)
{
	if (trgItem.id == 0x1921)
		trgItem.id = 0x1920;
}

function FlourMill(tileID)
{
	return (tileID == 0x188b || (tileID >= 0x1920 && tileID <= 0x1923) || (tileID >= 0x1925 && tileID <= 0x1927) ||
		(tileID >= 0x192c && tileID <= 0x192f) || (tileID >= 0x1931 && tileID <= 0x1933));
}